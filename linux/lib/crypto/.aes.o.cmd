// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/lockdep.c
 *
 * Runtime locking correctness validator
 *
 * Started by Ingo Molnar:
 *
 *  Copyright (C) 2006,2007 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2007 Red Hat, Inc., Peter Zijlstra
 *
 * this code maps all the lock dependencies as they occur in a live kernel
 * and will warn about the following classes of locking bugs:
 *
 * - lock inversion scenarios
 * - circular lock dependencies
 * - hardirq/softirq safe/unsafe locking bugs
 *
 * Bugs are reported even if the current locking scenario does not cause
 * any deadlock at this point.
 *
 * I.e. if anytime in the past two locks were taken in a different order,
 * even if it happened for another task, even if those were different
 * locks (but of the same class as this lock), this code will detect it.
 *
 * Thanks to Arjan van de Ven for coming up with the initial idea of
 * mapping lock dependencies runtime.
 */
#define DISABLE_BRANCH_PROFILING
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/interrupt.h>
#include <linux/stacktrace.h>
#include <linux/debug_locks.h>
#include <linux/irqflags.h>
#include <linux/utsname.h>
#include <linux/hash.h>
#include <linux/ftrace.h>
#include <linux/stringify.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/gfp.h>
#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/nmi.h>
#include <linux/rcupdate.h>
#include <linux/kprobes.h>
#include <linux/lockdep.h>

#include <asm/sections.h>

#include "lockdep_internals.h"

#define CREATE_TRACE_POINTS
#include <trace/events/lock.h>

#ifdef CONFIG_PROVE_LOCKING
int prove_locking = 1;
module_param(prove_locking, int, 0644);
#else
#define prove_locking 0
#endif

#ifdef CONFIG_LOCK_STAT
int lock_stat = 1;
module_param(lock_stat, int, 0644);
#else
#define lock_stat 0
#endif

DEFINE_PER_CPU(unsigned int, lockdep_recursion);
EXPORT_PER_CPU_SYMBOL_GPL(lockdep_recursion);

static __always_inline bool lockdep_enabled(void)
{
	if (!debug_locks)
		return false;

	if (this_cpu_read(lockdep_recursion))
		return false;

	if (current->lockdep_recursion)
		return false;

	return true;
}

/*
 * lockdep_lock: protects the lockdep graph, the hashes and the
 *               class/list/hash allocators.
 *
 * This is one of the rare exceptions where it's justified
 * to use a raw spinlock - we really dont want the spinlock
 * code to recurse back into the lockdep code...
 */
static arch_spinlock_t __lock = (arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED;
static struct task_struct *__owner;

static inline void lockdep_lock(void)
{
	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	__this_cpu_inc(lockdep_recursion);
	arch_spin_lock(&__lock);
	__owner = current;
}

static inline void lockdep_unlock(void)
{
	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	if (debug_locks && DEBUG_LOCKS_WARN_ON(__owner != current))
		return;

	__owner = NULL;
	arch_spin_unlock(&__lock);
	__this_cpu_dec(lockdep_recursion);
}

static inline bool lockdep_assert_locked(void)
{
	return DEBUG_LOCKS_WARN_ON(__owner != current);
}

static struct task_struct *lockdep_selftest_task_struct;


static int graph_lock(void)
{
	lockdep_lock();
	/*
	 * Make sure that if another CPU detected a bug while
	 * walking the graph we dont change it (while the other
	 * CPU is busy printing out stuff with the graph lock
	 * dropped already)
	 */
	if (!debug_locks) {
		lockdep_unlock();
		return 0;
	}
	return 1;
}

static inline void graph_unlock(void)
{
	lockdep_unlock();
}

/*
 * Turn lock debugging off and return with 0 if it was off already,
 * and also release the graph lock:
 */
static inline int debug_locks_off_graph_unlock(void)
{
	int ret = debug_locks_off();

	lockdep_unlock();

	return ret;
}

unsigned long nr_list_entries;
static struct lock_list list_entries[MAX_LOCKDEP_ENTRIES];
static DECLARE_BITMAP(list_entries_in_use, MAX_LOCKDEP_ENTRIES);

/*
 * All data structures here are protected by the global debug_lock.
 *
 * nr_lock_classes is the number of elements of lock_classes[] that is
 * in use.
 */
#define KEYHASH_BITS		(MAX_LOCKDEP_KEYS_BITS - 1)
#define KEYHASH_SIZE		(1UL << KEYHASH_BITS)
static struct hlist_head lock_keys_hash[KEYHASH_SIZE];
unsigned long nr_lock_classes;
unsigned long nr_zapped_classes;
unsigned long max_lock_class_idx;
struct lock_class lock_classes[MAX_LOCKDEP_KEYS];
DECLARE_BITMAP(lock_classes_in_use, MAX_LOCKDEP_KEYS);

static inline struct lock_class *hlock_class(struct held_lock *hlock)
{
	unsigned int class_idx = hlock->class_idx;

	/* Don't re-read hlock->class_idx, can't use READ_ONCE() on bitfield */
	barrier();

	if (!test_bit(class_idx, lock_classes_in_use)) {
		/*
		 * Someone passed in garbage, we give up.
		 */
		DEBUG_LOCKS_WARN_ON(1);
		return NULL;
	}

	/*
	 * At this point, if the passed hlock->class_idx is still garbage,
	 * we just have to live with it
	 */
	return lock_classes + class_idx;
}

#ifdef CONFIG_LOCK_STAT
static DEFINE_PER_CPU(struct lock_class_stats[MAX_LOCKDEP_KEYS], cpu_lock_stats);

static inline u64 lockstat_clock(void)
{
	return local_clock();
}

static int lock_point(unsigned long points[], unsigned long ip)
{
	int i;

	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		if (points[i] == 0) {
			points[i] = ip;
			break;
		}
		if (points[i] == ip)
			break;
	}

	return i;
}

static void lock_time_inc(struct lock_time *lt, u64 time)
{
	if (time > lt->max)
		lt->max = time;

	if (time < lt->min || !lt->nr)
		lt->min = time;

	lt->total += time;
	lt->nr++;
}

static inline void lock_time_add(struct lock_time *src, struct lock_time *dst)
{
	if (!src->nr)
		return;

	if (src->max > dst->max)
		dst->max = src->max;

	if (src->min < dst->min || !dst->nr)
		dst->min = src->min;

	dst->total += src->total;
	dst->nr += src->nr;
}

struct lock_class_stats lock_stats(struct lock_class *class)
{
	struct lock_class_stats stats;
	int cpu, i;

	memset(&stats, 0, sizeof(struct lock_class_stats));
	for_each_possible_cpu(cpu) {
		struct lock_class_stats *pcs =
			&per_cpu(cpu_lock_stats, cpu)[class - lock_classes];

		for (i = 0; i < ARRAY_SIZE(stats.contention_point); i++)
			stats.contention_point[i] += pcs->contention_point[i];

		for (i = 0; i < ARRAY_SIZE(stats.contending_point); i++)
			stats.contending_point[i] += pcs->contending_point[i];

		lock_time_add(&pcs->read_waittime, &stats.read_waittime);
		lock_time_add(&pcs->write_waittime, &stats.write_waittime);

		lock_time_add(&pcs->read_holdtime, &stats.read_holdtime);
		lock_time_add(&pcs->write_holdtime, &stats.write_holdtime);

		for (i = 0; i < ARRAY_SIZE(stats.bounces); i++)
			stats.bounces[i] += pcs->bounces[i];
	}

	return stats;
}

void clear_lock_stats(struct lock_class *class)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct lock_class_stats *cpu_stats =
			&per_cpu(cpu_lock_stats, cpu)[class - lock_classes];

		memset(cpu_stats, 0, sizeof(struct lock_class_stats));
	}
	memset(class->contention_point, 0, sizeof(class->contention_point));
	memset(class->contending_point, 0, sizeof(class->contending_point));
}

static struct lock_class_stats *get_lock_stats(struct lock_class *class)
{
	return &this_cpu_ptr(cpu_lock_stats)[class - lock_classes];
}

static void lock_release_holdtime(struct held_lock *hlock)
{
	struct lock_class_stats *stats;
	u64 holdtime;

	if (!lock_stat)
		return;

	holdtime = lockstat_clock() - hlock->holdtime_stamp;

	stats = get_lock_stats(hlock_class(hlock));
	if (hlock->read)
		lock_time_inc(&stats->read_holdtime, holdtime);
	else
		lock_time_inc(&stats->write_holdtime, holdtime);
}
#else
static inline void lock_release_holdtime(struct held_lock *hlock)
{
}
#endif

/*
 * We keep a global list of all lock classes. The list is only accessed with
 * the lockdep spinlock lock held. free_lock_classes is a list with free
 * elements. These elements are linked together by the lock_entry member in
 * struct lock_class.
 */
static LIST_HEAD(all_lock_classes);
static LIST_HEAD(free_lock_classedcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/irqhandler.h \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/CMA) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/linux/seqlock.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/osq_lock.h \
  include/linux/debug_locks.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  include/linux/rbtree_types.h \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/generated/bounds.h \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/tsc.h \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
    $(wildcard include/config/PCI) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/fwnode.h \
  include/linux/vmalloc.h \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  include/linux/overflow.h \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  arch/x8
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/socket.h \
  arch/x86/include/generated/uapi/asm/socket.h \
  include/uapi/asm-generic/socket.h \
  arch/x86/include/generated/uapi/asm/sockios.h \
  include/uapi/asm-generic/sockios.h \
  include/uapi/linux/sockios.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/uapi/linux/socket.h \
  include/linux/net.h \
  include/linux/random.h \
    $(wildcard include/config/VMGENID) \
    $(wildcard include/config/ARCH_RANDOM) \
  include/linux/once.h \
  include/uapi/linux/random.h \
  include/linux/prandom.h \
  include/linux/siphash.h \
  arch/x86/include/asm/archrandom.h \
  include/uapi/linux/net.h \
  include/linux/textsearch.h \
  include/net/checksum.h \
  arch/x86/include/asm/checksum.h \
  arch/x86/include/asm/checksum_32.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/uapi/linux/libc-compat.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/DMA_API_DEBUG) \
    $(wildcard include/config/HAS_DMA) \
    $(wildcard include/config/NEED_DMA_MAP_STATE) \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/dma-direction.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  arch/x86/include/asm/io.h \
    $(wildcard includeELF                      L"      4     (               Ë¸ˇˇˇãÖ¿uãAÉ¯wã≈¨  âA1¿√∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇâ¬â»ãIÖ…upãä8  âHãä<  âHãä@  âHãäD  âHãäH  âHãäL  âHãäP  âH ãäT  âH$ãäX  âH(ãä\  âH,ãä`  âH0ãíd  âP41¿√ê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇãà4  âJãÄ0  âB1¿√ç∂    Ë¸ˇˇˇSãX\âÿË¸ˇˇˇãClË¸ˇˇˇ1¿[√çt& Ë¸ˇˇˇÉÏãÄî   dã    âL$1…πÄˇˇfâL$Ö“t7∆D$j π   çT$Ë¸ˇˇˇX∏ºç Ë¸ˇˇˇãD$d+    u1¿Éƒ√çt& ∆D$Î«Ë¸ˇˇˇçt& Ë¸ˇˇˇSâ√ÉÏãJd°    âD$1¿Ö…tãBÖ¿Öﬂ   «B   ∏   «B   âÉ0  ãB∫Äˇˇπ   fâT$âÉ4  ãÉî   ∆D$ j çT$Ë¸ˇˇˇãì0  πÅˇˇãÉî   fâL$π   àt$j çT$	Ë¸ˇˇˇãì0  πÇˇˇãÉî   fâL$	π   àT$j çT$Ë¸ˇˇˇãì4  ãÉî   ªÑˇˇfâ\$π   àT$j çT$Ë¸ˇˇˇÉƒãD$d+    uÉƒ1¿[√çv k…9»Ü)ˇˇˇÈˇˇˇË¸ˇˇˇçv Ë¸ˇˇˇUâ≈WVSÉÏd°    âD$ãEãPãRË¸ˇˇˇ®Ñ)  ãEXâ$Ö¿Ñ"  h    çuãπ   âË¸ˇˇˇâ√XÖ€Ö¸ˇˇˇ∏d   Ë¸ˇˇˇπ¿  ∫l  âË¸ˇˇˇâ√Ö¿Ñ„  π@   âÍæ†  Ë¸ˇˇˇπ   ∫ÅˇˇÎê∑Nf¡¬àL$ãÉî   É∆fâT$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÕ∫¿  ∆D$ãÉî   π   fâT$j çT$	Ë¸ˇˇˇπ¿D  ãÉî   fâL$	π   ∆D$j çT$Ë¸ˇˇˇπ   ∫¯*  ∏'  Ë¸ˇˇˇπ   ^∫oÄˇˇ_æ|  Îçt& ∑Nf¡¬àL$ãÉî   É∆fâT$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÕ∫Ñ   æ¥  ø@  Îçt& ∑Wf¡∆àT$É«π   fât$ãÉî   j çT$	Ë¸ˇˇˇ∑7XfÖˆuÕ∫%Üˇˇ∆D$ãÉî   π   fâT$ø‡  j çT$	Ë¸ˇˇˇ∫  YÎçv ∑wf¡¬âÒãÉî   É«fâT$àL$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÀ∫Äˇˇ∆D$ π   ãÉî   fâT$æÇˇˇøÑˇˇ«É0     «É4     j çT$	Ë¸ˇˇˇãì0  πÅˇˇãÉî   fâL$	π   àt$j çT$Ë¸ˇˇˇãì0  ãÉî   π   fât$ç≥¿   àT$j çT$Ë¸ˇˇˇãì4  ãÉî   π   fâ|$çª8  àT$j çT$Ë¸ˇˇˇ1¿π   ∑UÛ´ãD$«É8  Ä  «É<  ‡  «É@     «ÉD     «ÉH     ã âÉh  ãEçà  QçQRˇ∞   ãETˇ0h,   Ë¸ˇˇˇ1…∫   âÉƒ(j Ë¸ˇˇˇπ	ò ∫0  âj há   j jj hˇ   j j Ë¸ˇˇˇπ	ò ∫0  âÉƒ$j jxj jj hˇ   j j Ë¸ˇˇˇπ	ò ∫0  âÉƒ j j j jj jj j Ë¸ˇˇˇπ	ò ∫0  âÉƒ j j j jj jj j Ë¸ˇˇˇâslãõ¸   âÉƒ Ö€u6Ë¸ˇˇˇâ√Ö¿uãD$d+    u:Éƒâÿ[^_]√çt& âË¸ˇˇˇÎ‹ç¥&    Ë¸ˇˇˇÎŒª˚ˇˇˇÎ«ªÍˇˇˇÎ¿ªÙˇˇˇÎπË¸ˇˇˇç¥&    ç¥&    êË¸ˇˇˇSÉÏãHdã    âT$1“ãP(Å˙	ò Ñê   wNÅ˙	ò tnÅ˙	ò u^∫∂ˇˇãY‘ã@|fâT$àD$π   âÿj çT$Ë¸ˇˇˇX1¿ãT$d+    uZÉƒ[√çv Å˙	ò u∫ºˇˇãY‘ã@|fâT$Î∏ç∂    ∏ÍˇˇˇÎƒêãY‘π¥ˇˇã@|fâL$Îòç∂    ãY‘π∫ˇˇã@|fâL$ÎÄË¸ˇˇˇêË¸ˇˇˇUWVâ∆SÉÏd°    âD$1¿ãAÖ¿Öé   ãAâ◊âÀçê˙ﬂˇˇÉ‚˝Ñ%  =  Ñ*  «A   π   ãCãk«C   âKâ¬É‚¸=Å  ∏Ä  C–âËÉ‡¸Å˝·  Ω‡  C≈ã+âSâCÖÌtEãC=   Ñó  =   ÑD  =  tu∏ÍˇˇˇãT$d+    Ö¬  Éƒ[^_]√ç¥&    ãâãSâPãSâPãSâPãSâPãSâPãS âPãS$âPãS(âP ãS,âP$ãS0âP(ãS4âP,1¿Îñç∂    ∫∞ˇˇç|$∆D$ãÜî   fâT$π   â˙j Ë¸ˇˇˇπ%îˇˇãÜî   fâL$∆D$ j π   â˙Ë¸ˇˇˇãCY]ãÆî   =Ä  Ñ∏  =@  ÖÌ  Å{   Ñ⁄  ∏Äˇˇ∆D$π   â˙fâD$âËΩÑˇˇj Ë¸ˇˇˇãSπÉˇˇãÜî   fâL$π   àt$â˙j Ë¸ˇˇˇãSãÜî   π   fâl$	ΩàˇˇàT$â˙j Ë¸ˇˇˇãSπáˇˇãÜî   fâL$π   àt$â˙j Ë¸ˇˇˇãSãÜî   π   fâl$àT$â˙j Ë¸ˇˇˇ∫ûˇˇãÜî   π   fâT$â˙∆D$j Ë¸ˇˇˇÉƒãCâÜ8  ãCâÜ<  ãCâÜ@  ãCâÜD  ãCâÜH  ãCâÜL  ãC âÜP  ãC$âÜT  ãC(âÜX  ãC,âÜ\  ãC0âÜ`  ãC4âÜd  1¿ÈÃ˝ˇˇçt& ø∞ˇˇ∆D$ ãÜî   π   fâ|$ç|$Ω%ñˇˇj â˙Ë¸ˇˇˇãÜî   fâl$∆D$È3˛ˇˇç¥&    ∫∞ˇˇç|$∆D$ ãÜî   fâT$π   â˙j Ë¸ˇˇˇπ%ñˇˇãÜî   fâL$∆D$ÈÎ˝ˇˇç¥&    =†   u!É{xÖ˛ˇˇ∏Äˇˇ∆D$fâD$Î~ç∂    =`  u!Å{   Ö‰˝ˇˇ∏Äˇˇ∆D$fâD$ÎSçv =∞   u!Å{ê   Öº˝ˇˇ∏Äˇˇ∆D$fâD$Î+çv É¯XÖü˝ˇˇÉ{HÖï˝ˇˇπÄˇˇ∆D$fâL$ç∂    j π   â˙âËË¸ˇˇˇXÈC˛ˇˇç¥&    çv Å{‡  ÖS˝ˇˇ∫Äˇˇ∆D$fâT$Î¬fêπ   ÈË˚ˇˇç∂    π   Èÿ˚ˇˇ∏Äˇˇ∆D$fâD$ÎïË¸ˇˇˇ                    ê  ‡                                                                                                                          0   ¿   ‡      p  ê    ‡      VS6624 Chip Enable vs6624   3%s %d-%04x: failed to request GPIO %d
    6%s %d-%04x: chip found @ 0x%02x (%s)
 ã$ˇ0∑EPãEˇ∞   ãETˇ0h    Ë¸ˇˇˇÉƒÈ±  Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇlicense=GPL v2 author=Scott Jiang <Scott.Jiang.Linux@gmail.com> description=VS6624 sensor driver                                vs6624                                                                     †               `                   0   ‡                                                                                         ¿   p                                                                                                        ¥Ñ ∂u ∏ 4Ñ 6u 8 ÇÄ ÑÄ ÜÄ Ä ê˛ Ç  Äd     ( ∫  º  ã  å è  ê ï  ñ ô  ö  Ñ%! Ä  Ñ Ç Ç  ∞  ñ% Ä                            <  f e — f b      &  & &c &— &h &› &: &  Ä$  ä0 ëb íJ ïe ñ °: ¢∏  

 :  ; Ö 	; 
Ö :  0 è      oÄ å                             Å Å  … …G … …Ä …: :ê ;êG <ê …1 	…‹ 
…Ä …D Dê Eê1 Fê‚ … …‡ …Ä …G Gêê HêÉ IêÅ Jê‡ Kê` Lê Mêê Nê¿ OêC Pêt Qê Rê SêÄ Tê Uê‰ Vêê Wê¿ XêC Yê Zê [ê \êÏ …] …  …Ä …] ]ê£ ^ê _ê `ê£ aê bê cê" …r …í …Ä …d dêt eê fê gêr hêï …G …Ú …Å …i iët jë kë lëÏ më¥ në oë
 pëê qëÄ rë së‡ tëp uë vëê wë” xëƒ yë zë" …
 …æ …Ä …s sê¸ tê£ uê‡ vêı wêÇ xêå yêÉ zê£ {ê£ |ê‡ }ê¸ ~ê£ ê‡ Äê√ Åêü Çêˇ ÉêÏ Ñêû Öê˛ Üê áê
 àêÍ  …G !…8 "…Ä #…â âêÏ äê” ãêî åê  çê@ éê èê êêê ëê” íê‘ ìêÏ îê ïê ñêG óê= $…E %…  &…Ä '…ò òê ôêw öê÷ õê úêE ùêÕ (…  )…’ *…Ä +…û ûêê üêÇ †ê °ê‡ ¢ê¥ £ê §ê •êê ¶êÉ ßêø ®ê‡ ©ê` ™ê ´êê ¨êÅ ≠ê¸ Æê‡ Øêˇ ∞ê√ ±ê ≤ê ≥êê ¥êÅ µê¸ ∂ê‡ ∑êˇ ∏ê πê  ∫ê⁄ ,…p -…º .…Ä /…ª ªêê ºêÇ Ωê æê‡ øê¥ ¿ê ¡ê ¬êê √ê¡ ƒê ≈êt ∆ê «ê »êê …ê”  ê† Àê Ãêp Õêø 0…r 1…! 2…Å 3…; ;ë} <ë =ë >ë{ ?ë @ër Aë% 4…( 5…Æ 6…Ä 7…“ “ê ”êê ‘ê“ ’ê
 ÷ê ◊ê( ÿê¥ 8…( 9…± :…Ä ;…Ÿ Ÿêê ⁄êÉ €ê∫ ‹ê‡ ›êˇ ﬁêê ﬂê“ ‡ê ·ê‡ ‚ê‰ „êÔ ‰ê Âê£ Êê‡ Áêt Ëêˇ Èê Íêê Îê“ Ïê
 Ìê Óê( Ôê¥ <…) =…y >…Ä ?… ê Òêê Úê“ Ûê Ùê ıê) ˆê @…) A…| B…Ä C…˜ ˜êê ¯êÉ ˘ê∫ ˙ê‡ ˚êˇ ¸êê ˝ê“ ˛ê ˇê‡  ë‰ ëÔ ë ë£ ë‡ ët ëˇ ë ëê 	ë“ 
ë ë ë) ë D…* E…B F…Å G… ë ëê ë“ ë ë ë* ëH H…* I…E J…Å K… ëê ëÉ ë∫ ë‡ ëˇ ëê ë“ ë ë‡ ë‰ ëÔ  ë !ë£ "ë‡ #ët $ëˇ %ë &ëê 'ë“ (ë )ë *ë* +ëH  …                       GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                            
       &         ¨       /   0   Ü     >   ¿        V   ‡        d      l     t   p                                 	              
 å   ê  q    ô   @         §   †      Æ   |       ∏   @  å     «   ‡  ú     ÿ   0       Ë       *    
 ˙     œ       ‡  ê                             *      Ä     8      
     K           b     1     x  @   !     ì      0                   ù     0     ≠  †   P     æ  `   @     Õ             ÿ             ˆ                                       9             H             Y             r             à             è             ú             ±             ƒ             Ã             È             ˚                          !             5           A             P      
     _      0      vs6624.c vs6624_enum_mbus_code vs6624_formats vs6624_get_fmt vs6624_g_frame_interval vs6624_remove vs6624_s_stream vs6624_s_frame_interval vs6624_probe vs6624_ops vs6624_p1 vs6624_p2 vs6624_default vs6624_run_setup vs6624_ctrl_ops vs6624_probe.cold vs6624_s_ctrl vs6624_set_fmt vs6624_driver_init vs6624_driver vs6624_driver_exit __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 vs6624_id vs6624_core_ops vs6624_video_ops vs6624_pad_ops __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free __stack_chk_guard i2c_transfer_buffer_flags __const_udelay __stack_chk_fail __x86_indirect_thunk_edx devm_gpio_request_one msleep devm_kmalloc v4l2_i2c_subdev_init usleep_range_state _printk v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vs6624_id_device_table       $        1   $  ¡   $  ·   $  Ï   %  Ù   &    $    '  :  (  E  )  P  '  h  *  q  $  Ä  '  ‹  (    (  0  (  Z  (  h  '  â  *  ë  $  †  '  ≤  +  Õ    ﬁ  ,  Û  -    .              /  V  (  Ñ  (  ©  (  Ω  0  Œ     ˛  (       F  (  n     y  (  ∞  (  ¸  (  &  (  Q  (  |  (  Ê    Î  1  ˛  2       %  3  /     L  3  V     p  3  z     î  3  ´  4  º  '  ”  &  ·  &  ˝  *    $    '  l  (  z  '  €  *  ·  $    '  ü  '  +	  (  N	  (  †	  (  ≈	  (  Í	  (  
  (  /
  (  R
  (  ¯
  (  ;  (    (  l  *  È                   h                                                     $             1  &        $          5     6          8  @      L      \      d     p     t     »     –     ‘     8     .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rodata.str1.4 .rel.text.unlikely .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   p                    	   @       ‘  ò              )             ¿  Ä                   %   	   @       l                   /             @                     8             @  (                  4   	   @       å   P               E      2       h                   T      2       Ñ  T                 g             ÿ  *                  c   	   @       ‹         
         z                                 v   	   @       Ù                   â               
                  Ö   	   @       !                 î                a                  °             †  ƒ                  ù   	   @       $!  P               ©      0       d                   ≤              w                     ¬             x  (                                †  ∞     $         	              P  Å                               t!  ’                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           