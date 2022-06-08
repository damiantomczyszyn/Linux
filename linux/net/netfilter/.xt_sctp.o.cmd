NGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/resource_ext.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  arch/x86/include/asm/acenv.h \
  include/acpi/acnames.h \
  include/acpi/actypes.h \
  include/acpi/acexcep.h \
  include/acpi/actbl.h \
  include/acpi/actbl1.h \
  include/acpi/actbl2.h \
  include/acpi/actbl3.h \
  include/acpi/acrestyp.h \
  include/acpi/platform/acenvex.h \
  include/acpi/platform/aclinuxex.h \
  include/acpi/platform/acgccex.h \
  include/acpi/acoutput.h \
  include/acpi/acpiosxf.h \
  include/acpi/acpixf.h \
  include/acpi/acconfig.h \
  include/acpi/acbuffer.h \
  include/linux/dynamic_debug.h \
  include/acpi/acpi_bus.h \
    $(wildcard include/config/X86_ANDROID_TABLETS) \
    $(wildcard include/config/ACPI_SYSTEM_POWER_STATES_SUPPORT) \
    $(wildcard include/config/ACPI_SLEEP) \
  include/acpi/acpi_drivers.h \
    $(wildcard include/config/ACPI_DOCK) \
  include/acpi/acpi_numa.h \
    $(wildcard include/config/ACPI_HMAT) \
  include/acpi/acpi_io.h \
  arch/x86/include/asm/acpi.h \
    $(wildcard include/config/ACPI_APEI) \
  include/acpi/pdc_intel.h \
  arch/x86/include/asm/numa.h \
    $(wildcard include/config/NUMA_EMU) \
  arch/x86/include/asm/numa_32.h \
  include/linux/regulator/consumer.h \
    $(wildcard include/config/REGULATOR) \
  include/linux/suspend.h \
    $(wildcard include/config/VT) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/HIBERNATION_SNAPSHOT_DEV) \
    $(wildcard include/config/PM_SLEEP_DEBUG) \
    $(wildcard include/config/PM_AUTOSLEEP) \
  include/linux/swap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
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
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/page_counter.h \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/sizes.h \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bvec.h \
  include/linux/highmem.h \
  include/linux/cacheflush.h \
  arch/x86/include/asm/cacheflush.h \
  include/asm-generic/cacheflush.h \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  include/linux/pagemap.h \
  include/linux/hugetlb_inline.h \
  include/uapi/linux/mempolicy.h \
  include/linux/freezer.h \
  include/uapi/linux/i2c.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
  include/media/media-device.h \
  include/media/media-devnode.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/hevc-ctrls.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/v4l2-subdev.h \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-image-sizes.h \
  drivers/media/i2c/vs6624_regs.h \

drivers/media/i2c/vs6624.o: $(deps_drivers/media/i2c/vs6624.o)

$(deps_drivers/media/i2c/vs6624.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ig/NF_CT_PROTO_SCTP) \
    $(wildcard include/config/NF_FLOW_TABLE) \
    $(wildcard include/config/NF_CT_PROTO_GRE) \
    $(wildcard include/config/NF_CONNTRACK_EVENTS) \
    $(wildcard include/config/NF_CONNTRACK_LABELS) \
  include/linux/list_nulls.h \
  include/linux/netfilter/nf_conntrack_tcp.h \
  include/uapi/linux/netfilter/nf_conntrack_tcp.h \
  include/linux/netfilter/nf_conntrack_dccp.h \
  include/uapi/linux/netfilter/nf_conntrack_tuple_common.h \
  include/linux/netfilter/nf_conntrack_sctp.h \
  include/uapi/linux/netfilter/nf_conntrack_sctp.h \
  include/net/netns/nftables.h \
  include/net/netns/xfrm.h \
  include/uapi/linux/xfrm.h \
  include/net/netns/mpls.h \
  include/net/netns/can.h \
  include/net/netns/xdp.h \
  include/net/netns/smc.h \
  include/net/netns/bpf.h \
  include/net/netns/mctp.h \
  include/net/net_trackers.h \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
  include/linux/ref_tracker.h \
    $(wildcard include/config/REF_TRACKER) \
  include/linux/seq_file_net.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ctype.h \
  include/net/sock.h \
    $(wildcard include/config/SOCK_RX_QUEUE_MAPPING) \
    $(wildcard include/config/SOCK_VALIDATE_XMIT) \
    $(wildcard include/config/RPS) \
  include/linux/netdevice.h \
    $(wildcard include/config/DCB) \
    $(wildcard include/config/HYPERV_NET) \
    $(wildcard include/config/WLAN) \
    $(wildcard include/config/AX25) \
    $(wildcard include/config/MAC80211_MESH) \
    $(wildcard include/config/NET_IPIP) \
    $(wildcard include/config/NET_IPGRE) \
    $(wildcard include/config/IPV6_SIT) \
    $(wildcard include/config/IPV6_TUNNEL) \
    $(wildcard include/config/NETPOLL) \
    $(wildcard include/config/BQL) \
    $(wildcard include/config/RFS_ACCEL) \
    $(wildcard include/config/FCOE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/NET_POLL_CONTROLLER) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/VLAN_8021Q) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/netprio_cgroup.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/CGROUP_DATA) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/kernel_stat.h \
  include/linux/cgroup-defs.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/uapi/linux/netdevice.h \
  include/uapi/linux/if.h \
  include/uapi/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
  include/uapi/linux/if_bonding.h \
  include/uapi/linux/pkt_cls.h \
  include/uapi/linux/pkt_sched.h \
  include/linux/hashtable.h \
  include/linux/page_counter.h \
  s);

/**
 * struct pending_free - information about data structures about to be freed
 * @zapped: Head of a list with struct lock_class elements.
 * @lock_chains_being_freed: Bitmap that indicates which lock_chains[] elements
 *	are about to be freed.
 */
struct pending_free {
	struct list_head zapped;
	DECLARE_BITMAP(lock_chains_being_freed, MAX_LOCKDEP_CHAINS);
};

/**
 * struct delayed_free - data structures used for delayed freeing
 *
 * A data structure for delayed freeing of data structures that may be
 * accessed by RCU readers at the time these were freed.
 *
 * @rcu_head:  Used to schedule an RCU callback for freeing data structures.
 * @index:     Index of @pf to which freed data structures are added.
 * @scheduled: Whether or not an RCU callback has been scheduled.
 * @pf:        Array with information about data structures about to be freed.
 */
static struct delayed_free {
	struct rcu_head		rcu_head;
	int			index;
	int			scheduled;
	struct pending_free	pf[2];
} delayed_free;

/*
 * The lockdep classes are in a hash-table as well, for fast lookup:
 */
#define CLASSHASH_BITS		(MAX_LOCKDEP_KEYS_BITS - 1)
#define CLASSHASH_SIZE		(1UL << CLASSHASH_BITS)
#define __classhashfn(key)	hash_long((unsigned long)key, CLASSHASH_BITS)
#define classhashentry(key)	(classhash_table + __classhashfn((key)))

static struct hlist_head classhash_table[CLASSHASH_SIZE];

/*
 * We put the lock dependency chains into a hash-table as well, to cache
 * their existence:
 */
#define CHAINHASH_BITS		(MAX_LOCKDEP_CHAINS_BITS-1)
#define CHAINHASH_SIZE		(1UL << CHAINHASH_BITS)
#define __chainhashfn(chain)	hash_long(chain, CHAINHASH_BITS)
#define chainhashentry(chain)	(chainhash_table + __chainhashfn((chain)))

static struct hlist_head chainhash_table[CHAINHASH_SIZE];

/*
 * the id of held_lock
 */
static inline u16 hlock_id(struct held_lock *hlock)
{
	BUILD_BUG_ON(MAX_LOCKDEP_KEYS_BITS + 2 > 16);

	return (hlock->class_idx | (hlock->read << MAX_LOCKDEP_KEYS_BITS));
}

static inline unsigned int chain_hlock_class_idx(u16 hlock_id)
{
	return hlock_id & (MAX_LOCKDEP_KEYS - 1);
}

/*
 * The hash key of the lock dependency chains is a hash itself too:
 * it's a hash of all locks taken up to that lock, including that lock.
 * It's a 64-bit hash, because it's important for the keys to be
 * unique.
 */
static inline u64 iterate_chain_key(u64 key, u32 idx)
{
	u32 k0 = key, k1 = key >> 32;

	__jhash_mix(idx, k0, k1); /* Macro that modifies arguments! */

	return k0 | (u64)k1 << 32;
}

void lockdep_init_task(struct task_struct *task)
{
	task->lockdep_depth = 0; /* no locks held yet */
	task->curr_chain_key = INITIAL_CHAIN_KEY;
	task->lockdep_recursion = 0;
}

static __always_inline void lockdep_recursion_inc(void)
{
	__this_cpu_inc(lockdep_recursion);
}

static __always_inline void lockdep_recursion_finish(void)
{
	if (WARN_ON_ONCE(__this_cpu_dec_return(lockdep_recursion)))
		__this_cpu_write(lockdep_recursion, 0);
}

void lockdep_set_selftest_task(struct task_struct *task)
{
	lockdep_selftest_task_struct = task;
}

/*
 * Debugging switches:
 */

#define VERBOSE			0
#define VERY_VERBOSE		0

#if VERBOSE
# define HARDIRQ_VERBOSE	1
# define SOFTIRQ_VERBOSE	1
#else
# define HARDIRQ_VERBOSE	0
# define SOFTIRQ_VERBOSE	0
#endif

#if VERBOSE || HARDIRQ_VERBOSE || SOFTIRQ_VERBOSE
/*
 * Quick filtering for interesting events:
 */
static int class_filter(struct lock_class *class)
{
#if 0
	/* Example */
	if (class->name_version == 1 &&
			!strcmp(class->name, "lockname"))
		return 1;
	if (class->name_version == 1 &&
			!strcmp(class->name, "&struct->lockfield"))
		return 1;
#endif
	/* Filter everything else. 1 would be to allow everything else */
	return 0;
}
#endif

static int verbose(struct lock_class *class)
{
#if VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static void print_lockdep_off(const char *bug_msg)
{
	printk(KERN_DEBUG "%s\n", bug_msg);
	printk(KERN_DEBUG "turning off the locking correctness validator.\n");
#ifdef CONFIG_LOCK_STAT
	printk(KERN_DEBUG "Please attach the output of /proc/lock_stat to the bug report\n");
#endif
}

unsigned long nr_stack_trace_entries;

#ifdef CONFIG_PROVE_LOCKING
/**
 * struct lock_trace - single stack backtrace
 * @hash_entry:	Entry in a stack_trace_hash[] list.
 * @hash:	jhash() of @entries.
 * @nr_entries:	Number of entries in @entries.
 * @entries:	Actual stack backtrace.
 */
struct lock_trace {
	struct hlist_node	hash_entry;
	u32			hash;
	u32			nr_entries;
	unsigned long		entries[] __aligned(sizeof(unsigned long));
};
#define LOCK_TRACE_SIZE_IN_LONGS				\
	(sizeof(struct lock_trace) / sizeof(unsigned long))
/*
 * Stack-trace: sequence of lock_trace structures. Protected by the graph_lock.
 */
static unsigned long stack_trace[MAX_STACK_TRACE_ENTRIES];
static struct hlist_head stack_trace_hash[STACK_TRACE_HASH_SIZE];

static bool traces_identical(struct lock_trace *t1, struct lock_trace *t2)
{
	return t1->hash == t2->hash && t1->nr_entries == t2->nr_entries &&
		memcmp(t1->entries, t2->entries,
		       t1->nr_entries * sizeof(t1->entries[0])) == 0;
}

static struct lock_trace *save_trace(void)
{
	struct lock_trace *trace, *t2;
	struct hlist_head *hash_head;
	u32 hash;
	int max_entries;

	BUILD_BUG_ON_NOT_POWER_OF_2(STACK_TRACE_HASH_SIZE);
	BUILD_BUG_ON(LOCK_TRACE_SIZE_IN_LONGS >= MAX_STACK_TRACE_ENTRIES);

	trace = (struct lock_trace *)(stack_trace + nr_stack_trace_entries);
	max_entries = MAX_STACK_TRACE_ENTRIES - nr_stack_trace_entries -
		LOCK_TRACE_SIZE_IN_LONGS;

	if (max_entries <= 0) {
		if (!debug_locks_off_graph_unlock())
			return NULL;

		print_lockdep_off("BUG: MAX_STACK_TRACE_ENTRIES too low!");
		dump_stack();

		return NULL;
	}
	trace->nr_entries = stack_trace_save(trace->entries, max_entries, 3);

	hash = jhash(trace->entries, trace->nr_entries *
		     sizeof(trace->entries[0]), 0);
	trace->hash = hash;
	hash_head = stack_trace_hash + (hash & (STACK_TRACE_HASH_SIZE - 1));
	hlist_for_each_entry(t2, hash_head, hash_entry) {
		if (traces_identical(trace, t2))
			return t2;
	}
	nr_stack_trace_entries += LOCK_TRACE_SIZE_IN_LONGS + trace->nr_entries;
	hlist_add_head(&trace->hash_entry, hash_head);

	return trace;
}

/* Return the number of stack traces in the stack_trace[] array. */
u64 lockdep_stack_trace_count(void)
{
	struct lock_trace *trace;
	u64 c = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(stack_trace_hash); i++) {
		hlist_for_each_entry(trace, &stack_trace_hash[i], hash_entry) {
			c++;
		}
	}

	return c;
}

/* Return the number of stack hash chains that have at least one stack trace. */
u64 lockdep_stack_hash_count(void)
{
	u64 c = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(stack_trace_hash); i++)
		if (!hlist_empty(&stack_trace_hash[i]))
			c++;

	return c;
}
#endif

unsigned int nr_hardirq_chains;
unsigned int nr_softirq_chains;
unsigned int nr_process_chains;
unsigned int max_lockdep_depth;

#ifdef CONFIG_DEBUG_LOCKDEP
/*
 * Various lockdep statistics:
 */
DEFINE_PER_CPU(struct lockdep_stats, lockdep_stats);
#endif

#ifdef CONFIG_PROVE_LOCKING
/*
 * Locking printouts:
 */

#define __USAGE(__STATE)						\
	[LOCK_USED_IN_##__STATE] = "IN-"__stringify(__STATE)"-W",	\
	[LOCK_ENABLED_##__STATE] = __stringify(__STATE)"-ON-W",		\
	[LOCK_USED_IN_##__STATE##_READ] = "IN-"__stringify(__STATE)"-R",\
	[LOCK_ENABLED_##__STATE##_READ] = __stringify(__STATE)"-ON-R",

static const char *usage_str[] =
{
#define LOCKDEP_STATE(__STATE) __USAGE(__STATE)
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	[LOCK_USED] = "INITIAL USE",
	[LOCK_USED_READ] = "INITIAL READ USE",
	/* abused as string storage for verify_lock_unused() */
	[LOCK_USAGE_STATES] = "IN-NMI",
};
#endif

const char *__get_key_name(const struct lockdep_subclass_key *key, char *str)
{
	return kallsyms_lookup((unsigned long)key, NULL, NULL, NULL, str);
}

static inline unsigned long lock_flag(enum lock_usage_bit bit)
{
	return 1UL << bit;
}

static char get_usage_char(struct lock_class *class, enum lock_usage_bit bit)
{
	/*
	 * The usage character defaults to '.' (i.e., irqs disabled and not in
	 * irq context), which is the safest usage category.
	 */
	char c = '.';

	/*
	 * The order of the following usage checks matters, which will
	 * result in the outcome character as follows:
	 *
	 * - '+': irq is enabled and not in irq context
	 * - '-': in irq context and irq is disabled
	 * - '?': in irq context and irq is enabled
	 */
	if (class->usage_mask & lock_flag(bit + LOCK_USAGE_DIR_MASK)) {
		c = '+';
		if (class->usage_mask & lock_flag(bit))
			c = '?';
	} else if (class->usage_mask & lock_flag(bit))
		c = '-';

	return c;
}

void get_usage_chars(struct lock_class *class, char usage[LOCK_USAGE_CHARS])
{
	int i = 0;

#define LOCKDEP_STATE(__STATE) 						\
	usage[i++] = get_usage_char(class, LOCK_USED_IN_##__STATE);	\
	usage[i++] = get_usage_char(class, LOCK_USED_IN_##__STATE##_READ);
#include "lockdep_states.h"
#undef LOCKDEP_STATE

	usage[i] = '\0';
}

static void __print_lock_name(struct lock_class *class)
{
	char str[KSYM_NAME_LEN];
	const char *name;

	name = class->name;
	if (!name) {
		name = __get_key_name(class->key, str);
		printk(KERN_CONT "%s", name);
	} else {
		printk(KERN_CONT "%s", name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		if (class->subclass)
			printk(KERN_CONT "/%d", class->subclass);
	}
}

static void print_lock_name(struct lock_class *class)
{
	char usage[LOCK_USAGE_CHARS];

	get_usage_chars(class, usage);

	printk(KERN_CONT " (");
	__print_lock_name(class);
	printk(KERN_CONT "){%s}-{%d:%d}", usage,
			class->wait_type_outer ?: class->wait_type_inner,
			class->wait_type_inner);
}

static void print_lockdep_cache(struct lockdep_map *lock)
{
	const char *name;
	char str[KSYM_NAME_LEN];

	name = lock->name;
	if (!name)
		name = __get_key_name(lock->key->subkeys, str);

	printk(KERN_CONT "%s", name);
}

static void print_lock(struct held_lock *hlock)
{
	/*
	 * We can be called locklessly through debug_show_all_locks() so be
	 * extra careful, the hlock might have been released and cleared.
	 *
	 * If this indeed happens, lets pretend it does not hurt to continue
	 * to print the lock unless the hlock class_idx does not point to a
	 * registered class. The rationale here is: since we don't attempt
	 * to distinguish whether we are in this situation, if it just
	 * happened we can't count on class_idx to tell either.
	 */
	struct lock_class *lock = hlock_class(hlock);

	if (!lock) {
		printk(KERN_CONT "<RELEASED>\n");
		return;
	}

	printk(KERN_CONT "%px", hlock->instance);
	print_lock_name(lock);
	printk(KERN_CONT ", at: %pS\n", (void *)hlock->acquire_ip);
}

static void lockdep_print_held_locks(struct task_struct *p)
{
	int i, depth = READ_ONCE(p->lockdep_depth);

	if (!depth)
		printk("no locks held by %s/%d.\n", p->comm, task_pid_nr(p));
	else
		printk("%d lock%s held by %s/%d:\n", depth,
		       depth > 1 ? "s" : "", p->comm, task_pid_nr(p));
	/*
	 * It's not reliable to print a task's held locks if it's not sleeping
	 * and it's not the current task.
	 */
	if (p != current && task_is_running(p))
		return;
	for (i = 0; i < depth; i++) {
		printk(" #%d: ", i);
		print_lock(p->held_locks + i);
	}
}

static void print_kernel_ident(void)
{
	printk("%s %.*s %s\n", init_utsname()->release,
		(int)strcspn(init_utsname()->version, " "),
		init_utsname()->version,
		print_tainted());
}

static int very_verbose(struct lock_class *class)
{
#if VERY_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

/*
 * Is this the address of a static object:
 */
#ifdef __KERNEL__
/*
 * Check if an address is part of freed initmem. After initmem is freed,
 * memory can be allocated from it, and such allocations would then have
 * addresses within the range [_stext, _end].
 */
#ifndef arch_is_kernel_initmem_freed
static int arch_is_kernel_initmem_freed(unsigned long addr)
{
	if (system_state < SYSTEM_FREEING_INITMEM)
		return 0;

	return init_section_contains((void *)addr, 1);
}
#endif

static int static_obj(const void *obj)
{
	unsigned long start = (unsigned long) &_stext,
		      end   = (unsigned long) &_end,
		      addr  = (unsigned long) obj;

	if (arch_is_kernel_initmem_freed(addr))
		return 0;

	/*
	 * static variable?
	 */
	if ((addr >= start) && (addr < end))
		return 1;

	/*
	 * in-kernel percpu var?
	 */
	if (is_kernel_percpu_address(addr))
		return 1;

	/*
	 * module static or percpu var?
	 */
	return is_module_address(addr) || is_module_percpu_address(addr);
}
#endif

/*
 * To make lock name printouts unique, we calculate a unique
 * class->name_version generation counter. The caller must hold the graph
 * lock.
 */
static int count_matching_names(struct lock_class *new_class)
{
	struct lock_class *class;
	int count = 0;

	if (!new_class->name)
		return 0;

	list_for_each_entry(class, &all_lock_classes, lock_entry) {
		if (new_class->key - new_class->subclass == class->key)
			return class->name_version;
		if (class->name && !strcmp(class->name, new_class->name))
			count = max(count, class->name_version);
	}

	return count + 1;
}

/* used from NMI context -- must be lockless */
static noinstr struct lock_class *
look_up_lock_class(const struct lockdep_map *lock, unsigned int subclass)
{
	struct lockdep_subclass_key *key;
	struct hlist_head *hash_head;
	struct lock_class *class;

	if (unlikely(subclass >= MAX_LOCKDEP_SUBCLASSES)) {
		instrumentation_begin();
		debug_locks_off();
		printk(KERN_ERR
			"BUG: looking up invalid subclass: %u\n", subclass);
		printk(KERN_ERR
			"turning off the locking correctness validator.\n");
		dump_stack();
		instrumentation_end();
		return NULL;
	}

	/*
	 * If it is not initialised then it has never been locked,
	 * so it won't be present in the hash table.
	 */
	if (unlikely(!lock->key))
		return NULL;

	/*
	 * NOTE: the class-key must be unique. For dynamic locks, a static
	 * lock_class_key variable is passed in through the mutex_init()
	 * (or spin_lock_init()) call - which acts as the key. For static
	 * locks we use the lock object itself as the key.
	 */
	BUILD_BUG_ON(sizeof(struct lock_class_key) >
			sizeof(struct lockdep_map));

	key = lock->key->subkeys + subclass;

	hash_head = classhashentry(key);

	/*
	 * We do an RCU walk of the hash, see lockdep_free_key_range().
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return NULL;

	hlist_for_each_entry_rcu_notrace(class, hash_head, hash_entry) {
		if (class->key == key) {
			/*
			 * Huh! same key, different name? Did someone trample
			 * on some memory? We're most confused.
			 */
			WARN_ON_ONCE(class->name != lock->name &&
				     lock->key != &__lockdep_no_validate__);
			return class;
		}
	}

	return NULL;
}

/*
 * Static locks do not have their class-keys yet - for them the key is
 * the lock object itself. If the lock is in the per cpu area, the
 * canonical address of the lock (per cpu offset removed) is used.
 */
static bool assign_lock_key(struct lockdep_map *lock)
{
	unsigned long can_addr, addr = (unsigned long)lock;

#ifdef __KERNEL__
	/*
	 * lockdep_free_key_range() assumes that struct lock_class_key
	 * objects do not overlap. Since we use the address of lock
	 * objects as class key for static objects, check whether the
	 * size of lock_class_key objects does not exceed the size of
	 * the smallest lock object.
	 */
	BUILD_BUG_ON(sizeof(struct lock_class_key) > sizeof(raw_spinlock_t));
#endif

	if (__is_kernel_percpu_address(addr, &can_addr))
		lock->key = (void *)can_addr;
	else if (__is_module_percpu_address(addr, &can_addr))
		lock->key = (void *)can_addr;
	else if (static_obj(lock))
		lock->key = (void *)lock;
	else {
		/* Debug-check: all keys must be persistent! */
		debug_locks_off();
		pr_err("INFO: trying to register non-static key.\n");
		pr_err("The code is fine but needs lockdep annotation, or maybe\n");
		pr_err("you didn't initialize this object before use?\n");
		pr_err("turning off the locking correctness validator.\n");
		dump_stack();
		return false;
	}

	return true;
}

#ifdef CONFIG_DEBUG_LOCKDEP

/* Check whether element @e occurs in list @h */
static bool in_list(struct list_head *e, struct list_head *h)
{
	struct list_head *f;

	list_for_each(f, h) {
		if (e == f)
			return true;
	}

	return false;
}

/*
 * Check whether entry @e occurs in any of the locks_after or locks_before
 * lists.
 */
static bool in_any_class_list(struct list_head *e)
{
	struct lock_class *class;
	int i;

	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (in_list(e, &class->locks_after) ||
		    in_list(e, &class->locks_before))
			return true;
	}
	return false;
}

static bool class_lock_list_valid(struct lock_class *c, struct list_head *h)
{
	struct lock_list *e;

	list_for_each_entry(e, h, entry) {
		if (e->links_to != c) {
			printk(KERN_INFO "class %s: mismatch for lock entry %ld; class %s <> %s",
			       c->name ? : "(?)",
			       (unsigned long)(e - list_entries),
			       e->links_to && e->links_to->name ?
			       e->links_to->name : "(?)",
			       e->class && e->class->name ? e->class->name :
			       "(?)");
			return false;
		}
	}
	return true;
}

#ifdef CONFIG_PROVE_LOCKING
static u16 chain_hlocks[MAX_LOCKDEP_CHAIN_HLOCKS];
#endif

static bool check_lock_chain_key(struct lock_chain *chain)
{
#ifdef CONFIG_PROVE_LOCKING
	u64 chain_key = INITIAL_CHAIN_KEY;
	int i;

	for (i = chain->base; i < chain->base + chain->depth; i++)
		chain_key = iterate_chain_key(chain_key, chain_hlocks[i]);
	/*
	 * The 'unsigned long long' casts avoid that a compiler warning
	 * is reported when building tools/lib/lockdep.
	 */
	if (chain->chain_key != chain_key) {
		printk(KERN_INFO "chain %lld: key %#llx <> %#llx\n",
		       (unsigned long long)(chain - lock_chains),
		       (unsigned long long)chain->chain_key,
		       (unsigned long long)chain_key);
		return false;
	}
#endif
	return true;
}

static bool in_any_zapped_class_list(struct lock_class *class)
{
	struct pending_free *pf;
	int i;

	for (i = 0, pf = delayed_free.pf; i < ARRAY_SIZE(delayed_free.pf); i++, pf++) {
		if (in_list(&class->lock_entry, &pf->zapped))
			return true;
	}

	return false;
}

static bool __check_data_structures(void)
{
	struct lock_class *class;
	struct lock_chain *chain;
	struct hlist_head *head;
	struct lock_list *e;
	int i;

	/* Check whether all classes occur in a lock list. */
	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (!in_list(&class->lock_entry, &all_lock_classes) &&
		    !in_list(&class->lock_entry, &free_lock_classes) &&
		    !in_any_zapped_class_list(class)) {
			printk(KERN_INFO "class %px/%s is not in any class list\n",
			       class, class->name ? : "(?)");
			return false;
		}
	}

	/* Check whether all classes have valid lock lists. */
	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (!class_lock_list_valid(class, &class->locks_before))
			return false;
		if (!class_lock_list_valid(class, &class->locks_after))
			return false;
	}

	/* Check the chain_key of all lock chains. */
	for (i = 0; i < ARRAY_SIZE(chainhash_table); i++) {
		head = chainhash_table + i;
		hlist_for_each_entry_rcu(chain, head, entry) {
			if (!check_lock_chain_key(chain))
				return false;
		}
	}

	/*
	 * Check whether all list entries that are in use occur in a class
	 * lock list.
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		e = list_entries + i;
		if (!in_any_class_list(&e->entry)) {
			printk(KERN_INFO "list entry %d is not in any class list; class %s <> %s\n",
			       (unsigned int)(e - list_entries),
			       e->class->name ? : "(?)",
			       e->links_to->name ? : "(?)");
			return false;
		}
	}

	/*
	 * Check whether all list entries that are not in use do not occur in
	 * a class lock list.
	 */
	for_each_clear_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		e = list_entries + i;
		if (in_any_class_list(&e->entry)) {
			printk(KERN_INFO "list entry %d occurs in a class list; class %s <> %s\n",
			       (unsigned int)(e - list_entries),
			       e->class && e->class->name ? e->class->name :
			       "(?)",
			       e->links_to && e->links_to->name ?
			       e->links_to->name : "(?)");
			return false;
		}
	}

	return true;
}

int check_consistency = 0;
module_param(check_consistency, int, 0644);

static void check_data_structures(void)
{
	static bool once = false;

	if (check_consistency && !once) {
		if (!__check_data_structures()) {
			once = true;
			WARN_ON(once);
		}
	}
}

#else /* CONFIG_DEBUG_LOCKDEP */

static inline void check_data_structures(void) { }

#endif /* CONFIG_DEBUG_LOCKDEP */

static void init_chain_block_buckets(void);

/*
 * Initialize the lock_classes[] array elements, the free_lock_classes list
 * and also the delayed_free structure.
 */
static void init_data_structures_once(void)
{
	static bool __read_mostly ds_initialized, rcu_head_initialized;
	int i;

	if (likely(rcu_head_initialized))
		return;

	if (system_state >= SYSTEM_SCHEDULING) {
		init_rcu_head(&delayed_free.rcu_head);
		rcu_head_initialized = true;
	}

	if (ds_initialized)
		return;

	ds_initialized = true;

	INIT_LIST_HEAD(&delayed_free.pf[0].zapped);
	INIT_LIST_HEAD(&delayed_free.pf[1].zapped);

	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		list_add_tail(&lock_classes[i].lock_entry, &free_lock_classes);
		INIT_LIST_HEAD(&lock_classes[i].locks_after);
		INIT_LIST_HEAD(&lock_classes[i].locks_before);
	}
	init_chain_block_buckets();
}

static inline struct hlist_head *keyhashentry(const struct lock_class_key *key)
{
	unsigned long hash = hash_long((uintptr_t)key, KEYHASH_BITS);

	return lock_keys_hash + hash;
}

/* Register a dynamically allocated key. */
void lockdep_register_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head;
	struct lock_class_key *k;
	unsigned long flags;

	if (WARN_ON_ONCE(static_obj(key)))
		return;
	hash_head = keyhashentry(key);

	raw_local_irq_save(flags);
	if (!graph_lock())
		goto restore_irqs;
	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (WARN_ON_ONCE(k == key))
			goto out_unlock;
	}
	hlist_add_head_rcu(&key->hash_entry, hash_head);
out_unlock:
	graph_unlock();
restore_irqs:
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lockdep_register_key);

/* Check whether a key has been registered as a dynamic key. */
static bool is_dynamic_key(const struct lock_class_key *key)
{
	struct hlist_head *hash_head;
	struct lock_class_key *k;
	bool found = false;

	if (WARN_ON_ONCE(static_obj(key)))
		return false;

	/*
	 * If lock debugging is disabled lock_keys_hash[] may contain
	 * pointers to memory that has already been freed. Avoid triggering
	 * a use-after-free in that case by returning early.
	 */
	if (!debug_locks)
		return true;

	hash_head = keyhashentry(key);

	rcu_read_lock();
	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (k == key) {
			found = true;
			break;
		}
	}
	rcu_read_unlock();

	return found;
}

/*
 * Register a lock's class in the hash-table, if the class is not present
 * yet. Otherwise we look it up. We cache the result in the lock object
 * itself, so actual lookup of the hash should be once per lock object.
 */
static struct lock_class *
register_lock_class(struct lockdep_map *lock, unsigned int subclass, int force)
{
	struct lockdep_subclass_key *key;
	struct hlist_head *hash_head;
	struct lock_class *class;
	int idx;

	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	class = look_up_lock_class(lock, subclass);
	if (likely(class))
		goto out_set_class_cache;

	if (!lock->key) {
		if (!assign_lock_key(lock))
			return NULL;
	} else if (!static_obj(lock->key) && !is_dynamic_key(lock->key)) {
		return NULL;
	}

	key = lock->key->subkeys + subclass;
	hash_head = classhashentry(key);

	if (!graph_lock()) {
		return NULL;
	}
	/*
	 * We have to do the hash-walk again, to avoid races
	 * with another CPU:
	 */
	hlist_for_each_entry_rcu(class, hash_head, hash_entry) {
		if (class->key == key)
			goto out_unlock_set;
	}

	init_data_structures_once();

	/* Allocate a new lock class and add it to the hash. */
	class = list_first_entry_or_null(&free_lock_classes, typeof(*class),
					 lock_entry);
	if (!class) {
		if (!debug_locks_off_graph_unlock()) {
			return NULL;
		}

		print_lockdep_off("BUG: MAX_LOCKDEP_KEYS too low!");
		dump_stack();
		return NULL;
	}
	nr_lock_classes++;
	__set_bit(class - lock_classes, lock_classes_in_use);
	debug_atomic_inc(nr_unused_locks);
	class->key = key;
	class->name = lock->name;
	class->subclass = subclass;
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	class->name_version = count_matching_names(class);
	class->wait_type_inner = lock->wait_type_inner;
	class->wait_type_outer = lock->wait_type_outer;
	class->lock_type = lock->lock_type;
	/*
	 * We use RCU's safe list-add method to make
	 * parallel walking of the hash-list safe:
	 */
	hlist_add_head_rcu(&class->hash_entry, hash_head);
	/*
	 * Remove the class from the free list and add it to the global list
	 * of classes.
	 */
	list_move_tail(&class->lock_entry, &all_lock_classes);
	idx = class - lock_classes;
	if (idx > max_lock_class_idx)
		max_lock_class_idx = idx;

	if (verbose(class)) {
		graph_unlock();

		printk("\nnew class %px: %s", class->key, class->name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		printk(KERN_CONT "\n");
		dump_stack();

		if (!graph_lock()) {
			return NULL;
		}
	}
out_unlock_set:
	graph_unlock();

out_set_class_cache:
	if (!subclass || force)
		lock->class_cache[0] = class;
	else if (subclass < NR_LOCKDEP_CACHING_CLASSES)
		lock->class_cache[subclass] = class;

	/*
	 * Hash collision, did we smoke some? We found a class with a matching
	 * hash but the subclass -- which is hashed in -- didn't match.
	 */
	if (DEBUG_LOCKS_WARN_ON(class->subclass != subclass))
		return NULL;

	return class;
}

#ifdef CONFIG_PROVE_LOCKING
/*
 * Allocate a lockdep entry. (assumes the graph_lock held, returns
 * with NULL on failure)
 */
static struct lock_list *alloc_list_entry(void)
{
	int idx = find_first_zero_bit(list_entries_in_use,
				      ARRAY_SIZE(list_entries));

	if (idx >= ARRAY_SIZE(list_entries)) {
		if (!debug_locks_off_graph_unlock())
			return NULL;

		print_lockdep_off("BUG: MAX_LOCKDEP_ENTRIES too low!");
		dump_stack();
		return NULL;
	}
	nr_list_entries++;
	__set_bit(idx, list_entries_in_use);
	return list_entries + idx;
}

/*
 * Add a new dependency to the head of the list:
 */
static int add_lock_to_list(struct lock_class *this,
			    struct lock_class *links_to, struct list_head *head,
			    unsigned long ip, u16 distance, u8 dep,
			    const struct lock_trace *trace)
{
	struct lock_list *entry;
	/*
	 * Lock not present yet - get a new dependency struct and
	 * add it to the list:
	 */
	entry = alloc_list_entry();
	if (!entry)
		return 0;

	entry->class = this;
	entry->links_to = links_to;
	entry->dep = dep;
	entry->distance = distance;
	entry->trace = trace;
	/*
	 * Both allocation and removal are done under the graph lock; but
	 * iteration is under RCU-sched; see look_up_lock_class() and
	 * lockdep_free_key_range().
	 */
	list_add_tail_rcu(&entry->entry, head);

	return 1;
}

/*
 * For good efficiency of modular, we use power of 2
 */
#define MAX_CIRCULAR_QUEUE_SIZE		(1UL << CONFIG_LOCKDEP_CIRCULAR_QUEUE_BITS)
#define CQ_MASK				(MAX_CIRCULAR_QUEUE_SIZE-1)

/*
 * The circular_queue and helpers are used to implement graph
 * breadth-first search (BFS) algorithm, by which we can determine
 * whether there is a path from a lock to another. In deadlock checks,
 * a path from the next lock to be acquired to a previous held lock
 * indicates that adding the <prev> -> <next> lock dependency will
 * produce a circle in the graph. Breadth-first search instead of
 * depth-first search is used in order to find the shortest (circular)
 * path.
 */
struct circular_queue {
	struct lock_list *element[MAX_CIRCULAR_QUEUE_SIZE];
	unsigned int  front, rear;
};

static struct circular_queue lock_cq;

unsigned int max_bfs_queue_depth;

static unsigned int lockdep_dependency_gen_id;

static inline void __cq_init(struct circular_queue *cq)
{
	cq->front = cq->rear = 0;
	lockdep_dependency_gen_id++;
}

static inline int __cq_empty(struct circular_queue *cq)
{
	return (cq->front == cq->rear);
}

static inline int __cq_full(struct circular_queue *cq)
{
	return ((cq->rear + 1) & CQ_MASK) == cq->front;
}

static inline int __cq_enqueue(struct circular_queue *cq, struct lock_list *elem)
{
	if (__cq_full(cq))
		return -1;

	cq->element[cq->rear] = elem;
	cq->rear = (cq->rear + 1) & CQ_MASK;
	return 0;
}

/*
 * Dequeue an element from the circular_queue, return a lock_list if
 * the queue is not empty, or NULL if otherwise.
 */
static inline struct lock_list * __cq_dequeue(struct circular_queue *cq)
{
	struct lock_list * lock;

	if (__cq_empty(cq))
		return NULL;

	lock = cq->element[cq->front];
	cq->front = (cq->front + 1) & CQ_MASK;

	return lock;
}

static inline unsigned int  __cq_get_elem_count(struct circular_queue *cq)
{
	return (cq->rear - cq->front) & CQ_MASK;
}

static inline void mark_lock_accessed(struct lock_list *lock)
{
	lock->class->dep_gen_id = lockdep_dependency_gen_id;
}

static inline void visit_lock_entry(struct lock_list *lock,
				    struct lock_list *parent)
{
	lock->parent = parent;
}

static inline unsigned long lock_accessed(struct lock_list *lock)
{
	return lock->class->dep_gen_id == lockdep_dependency_gen_id;
}

static inline struct lock_list *get_lock_parent(struct lock_list *child)
{
	return child->parent;
}

static inline int get_lock_depth(struct lock_list *child)
{
	int depth = 0;
	struct lock_list *parent;

	while ((parent = get_lock_parent(child))) {
		child = parent;
		depth++;
	}
	return depth;
}

/*
 * Return the forward or backward dependency list.
 *
 * @lock:   the lock_list to get its class's dependency list
 * @offset: the offset to struct lock_class to determine whether it is
 *          locks_after or locks_before
 */
static inline struct list_head *get_dep_list(struct lock_list *lock, int offset)
{
	void *lock_class = lock->class;

	return lock_class + offset;
}
/*
 * Return values of a bfs search:
 *
 * BFS_E* indicates an error
 * BFS_R* indicates a result (match or not)
 *
 * BFS_EINVALIDNODE: Find a invalid node in the graph.
 *
 * BFS_EQUEUEFULL: The queue is full while doing the bfs.
 *
 * BFS_RMATCH: Find the matched node in the graph, and put that node into
 *             *@target_entry.
 *
 * BFS_RNOMATCH: Haven't found the matched node and keep *@target_entry
 *               _unchanged_.
 */
enum bfs_result {
	BFS_EINVALIDNODE = -2,
	BFS_EQUEUEFULL = -1,
	BFS_RMATCH = 0,
	BFS_RNOMATCH = 1,
};

/*
 * bfs_result < 0 means error
 */
static inline bool bfs_error(enum bfs_result res)
{
	return res < 0;
}

/*
 * DEP_*_BIT in lock_list::dep
 *
 * For dependency @prev -> @next:
 *
 *   SR: @prev is shared reader (->read != 0) and @next is recursive reader
 *       (->read == 2)
 *   ER: @prev is exclusive locker (->read == 0) and @next is recursive reader
 *   SN: @prev is shared reader and @next is non-recursive locker (->read != 2)
 *   EN: @prev is exclusive locker and @next is non-recursive locker
 *
 * Note that we define the value of DEP_*_BITs so that:
 *   bit0 is prev->read == 0
 *   bit1 is next->read != 2
 */
#define DEP_SR_BIT (0 + (0 << 1)) /* 0 */
#define DEP_ER_BIT (1 + (0 << 1)) /* 1 */
#define DEP_SN_BIT (0 + (1 << 1)) /* 2 */
#define DEP_EN_BIT (1 + (1 << 1)) /* 3 */

#define DEP_SR_MASK (1U << (DEP_SR_BIT))
#define DEP_ER_MASK (1U << (DEP_ER_BIT))
#define DEP_SN_MASK (1U << (DEP_SN_BIT))
#define DEP_EN_MASK (1U << (DEP_EN_BIT))

static inline unsigned int
__calc_dep_bit(struct held_lock *prev, struct held_lock *next)
{
	return (prev->read == 0) + ((next->read != 2) << 1);
}

static inline u8 calc_dep(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bit(prev, next);
}

/*
 * calculate the dep_bit for backwards edges. We care about whether @prev is
 * shared and whether @next is recursive.
 */
static inline unsigned int
__calc_dep_bitb(struct held_lock *prev, struct held_lock *next)
{
	return (next->read != 2) + ((prev->read == 0) << 1);
}

static inline u8 calc_depb(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bitb(prev, next);
}

/*
 * Initialize a lock_list entry @lock belonging to @class as the root for a BFS
 * search.
 */
static inline void __bfs_init_root(struct lock_list *lock,
				   struct lock_class *class)
{
	lock->class = class;
	lock->parent = NULL;
	lock->only_xr = 0;
}

/*
 * Initialize a lock_list entry @lock based on a lock acquisition @hlock as the
 * root for a BFS search.
 *
 * ->only_xr of the initial lock node is set to @hlock->read == 2, to make sure
 * that <prev>úËqìéŸ>¸aÊ´‘aÍÊBé\ÉmØ¥ÔcéŠÙ‹„UÉõ\rº8ˆ°§İ`‚|ğß‰üâ	Y¹3@zB=;`9ì”,“ı`_¹ŠÌÈ©‚7ÜÄÉŞËk[ô©ãã‹CêtZ¨*ô«°î’¹Ej7(`!SP?ä_e ÄèÎ€ø÷6è—±/P-Qø›òÃeØ‚4·[Xxúìşu;,+ë…#ãéEÁQ9TY}Kv­Œ”Ë4õ–Ï‡¶æÖŞÃ^÷p´ôQ:u?ÕŞ©í~Ñ(6¬vÒ•!=k\åîäÀ¿^’ıš³ôws/˜3;øzu§ëLÅK81:ÿ•ˆjhÆ¥ÙÃdgƒä<Yòµ†ùšâûÈÈ\@0úŞØÉHxWñ®œŸ"¼o-p[ûb†.<¤í K$!¢Cà•WpÔ°‡ë“uµ{Y€„a¤êÕO ¸ûÃÇY¹X„+Ÿ.ÚÛ*¦Ôëè†µõ»ccl‰Z»Ä¡]uQw< êÊ×ÇÈ Fk±üífÙæÉøiÊ¸Ä@2gÑ€e¾ |IĞx¸(0,bz
Àu)Fwş`–|œ;°Nl‹%®‹4ÊÑ3Ó£óîúß{òqjLT·zØm38¥¥’6s:Íéô¨M¶qE®Ê.eë¦/Wùn¸t£/Ë<yâğõÉ‹âIâ„äPĞ¡¤É2[B˜—ÖòÅé{÷vÑ˜R\XÉ’n: ÓMê6ø›»i3üÿ;¾6±'cZ›äô|Sn’ÿDù«
t†ĞŒĞV‡<ìUUÏòne0ï§¾6‹îÖº’yÂ‹éš66@ï©Js`ŸSÅ’¬÷á¨„júUMfÒ5ÜIÏ1bxOnˆ!_ójâ Çh8ß¸M´/ğ^ÎÏÂfy%an€•>i_öœ<µÔ‚Ã"„rO1( İ0f«†^Ò(/Âj;Q6ït3˜PÄë&À©ãŠ>„Ù0{Î5e&å² ƒæaÿíxÿ«‚uŸ*ª|­¯|Ò¥oK¨µ
»ş7'¬İ9FÈ9%ˆkØ:%Èa,ı8„ËöìyÔóywîÙJÖVÁ™NşãP?ÑÀ?ætºt)9ºíüWgd7ôëß¨ ’â*Ó§ñ·µIíıGõQÇnL~+„]˜S_›'Ò·+ò\¹Y8îe÷M;Õ¬ÛµÉ;©%®CòœÕPm•ìºÏ“zÙŒxyg,èÙàşÀLŠ‹×™µåw‘~Ÿ›n’R@Ş¬Ã(F ñ@TNøÔP–UˆÑbSÌZOH‹ô}d
>ã+ÀwVmØ½C*’®u¤Êª˜f/'©©§±±ĞË0Äúêj=!`QÌÊrıYyß|?ÓèÙ•›Ÿ›Ê:7“t"´•kƒ%+Ò¯2J—~ùŒ|sMNãÛÍÖ4QS¬¿9ÃøÑ”®n Ğyÿîyç7ÜÕçµ
­dk,Wùèp»x'·l7£,†ïÅØ•òw(—yÂ ñ|(ë_‚S)à Z¢B…şÊVwLÕ7GÍë¢giçË9:pUÁKpXr(à€RXúm© ’'âL#N1ÅLã8‘Zi@UoK\¦kJô¦ƒò€^Š|¾N½(7wÿº·c; M`ÿt½,2@ĞM…7Yu¦¾qñË'(ÊC.Ş-Yaˆ  ¶èö]ÅKa„÷;¾9$N=³š
ª¿ÍàĞ3P»©óW¼ÏŒ7:ZèÁd›êİ²pHRèV^>"ºfŞáZš¿
¥Ñ·Ò0qÿ8ïØ†tiŸI;‰fq²®®¯)™>'íäƒõÔPTœÌ^À=j77Ï™2á…¿dúT+ğ}Yè‹ÂW†çbßŠ7P¬rç#î d9÷FoMµˆïjL¹Õï}º(ïJpĞş“ªäA¦—ü"ô¬ãgqYOºüŠÚKS3¢yì"(WàUú¾©ÙZ8]T½L ü$Tp½|;ÀÂéÃb&Î=sÜ
ô'xÑ¨OúS¡N®mX Àbñ8$—`Ó'üK®&ı²{£†öq()È€\3—çÃo8©=ßC5İQ!ãaH¸Ôß´E®T½s@tvîˆ2´î›î{ 7ÛzTm‚8k¾é5Æ’ª+ÿV…ÅQ1Âzë0a‚lyÓÒïåK©2ƒpìáÈ)‚tÁPyÚÅ»NĞVZ– XbÛmæö˜ˆçm@ë  0İ¥??Şv 3¬pû!³ÍŒ˜ÿ¢áÌ¾ó@€"÷y¤‘Ãæ=âCj(ÎÑüÕx¡$åÖáTg!÷»Z‡s>c¯s¿?fèï?µ°ÓhÓÑƒø®´p„Zc{üôyî¥s ŠÛˆÃè¹ğwkäœ5Jxáõµ_a­LRÑÂÇ©éÛ¼Œ€'İdóÕè3ºÑC‘$t–Ut}¸Ä6ş­©TÛÁÄ¬QÎd,í¦é¬İ†ËÃF&Òô×P¤Âá5mğ¿¹Úàw8ğŞêS{î õ×`j°vˆïÈ	PŞ<ã¢Ä€R\Ñª½¶x”×“]ª_™€Ñg%DaôÔsÒÅ52	¬÷ùŸl#ø’ùÉäÕsR½EÓBîåTY“5ƒ0-S¯Íp ªî\Ë3z·tÒD¹HY‰€,@½87<G¼Ïü”A
ØŠ¨”§-jµ_´uRÁ¶asÆzéùCğ
«Ñr:.^ÉMW&È$¹+iáuãHWŒ=u”{†c? Ş¢L²/ÅtÜÈ_³ôÍ7 A–W,<ïÛ‚›®~^S(>d´³;çƒu”çyKßPmÔúKÖ[T:Ua»Üi©`­İÃä<Çy¹¦s÷WØ†¥ğ‚C&…XÎËëÃ<êa~–«1££Çsnşo	¾4’S¢`§wnÑˆ€¿®|`óÍib7…«ó/îtÊèjÜ{ìŞ6Ÿ4Ò3éï:ÿİ¦L¶©ÇB|Vİ}“ÎOog4@®ĞmXÎzî&ÅÛ×¤•´NµŠ²cïîID„.ä ±´R‡½ÔFûG'ëx›İşbkŠ<éŸ{[‰ pr“N…"İ>ö4WLÑ%²¹ÒAE3OÍ
cÔÎ•Ä<‰OØ|ù£şŸTË¾.`2†·lê¯…Ã¢#@“ãş'VÚµn‰÷¼šY-¦XÛÕV-â3â÷‘Ej4ùñ„çÅ@nDàá¡è~­=Õ³Fx13ÿ½ ’Ã?ø}M-
—óş ©ëLUñÃƒ÷B2bpOcAºæ»/FH5å˜ı}üÎ wH/¶Hˆ¦Á‹/ÓÔo¤ä[g]¸€ú`J3€÷±íRÛkK¨°@+¨Ş®ôĞ7¶¯‹;R°¡r†ŞŒ”ÎLkÍÔø“ ã}³á·üÆ°~»Üû®®ˆŠ›VÌº¯ä—¼Yâ½9Õ÷ÔvÙ¥¾á…ß%\ç¯Ä¡÷z¾Í½½÷ëIé ~ ˆ:£>*lâ¨ß÷ƒ`E43Á±_¶c^è‰
”r‘½.êÆS8§	æ]wóZ4iİğêÓ”‚kFD wtg?ÿ+êı«+¾é¯o7bá¬ı\¿k;¡ O-Ed[Bİg„Ÿ²3¸‰·§Î‚şºOÆİYøÆ©LÍ}
jÜPuæŸ|˜ŸÈT
^±LÄ½!¡É7F3l AÖtN |İ¤Å:Íì{è’(°RQdĞ„@Û¡(ÂéOŞı™Ü£C×l:î3ƒ4ïTth+kpö’Æú9Ëé¦¾3ªoQr
ÔE¯:î=Â	7—EO~Êşù¼úáÌ‡n[$>|ã³Ä‰’8¦©GûáU‚YËSğw<§ÖâÄ”ÛÏŠaèÓÀ°­ŒnlP ‘<]~‚Z›.Šîƒ†KûéuÖRÙ9Œ©Yg<ë†×œ¬eç†ì‰ëâú•B©A»¦ğ±ş¬Dn¡6æ]z&=-ólóİõ7q'âæZ©’¡ú¶ıŸàW~”Zyb4%Õw¿Dµ‡³Õ/Y¶Ú'-„T?*·°¼îò~İˆ"ğ.¨gr$Ÿ‹‘²¿›&×}%JMª¯)µ.*Óiõî;ä/:)bW™0ÚşÕºÖhqNfêUbâ¿äâYdş¸IÑ0xå½µ«ÿO&•İÃ%lù÷yBÏ²&' °v¢Œ_}-Ù6¹Å`n¡r¡4–¿ÛvzøÀª]şllò@ÍRfÑ ÀF/>ÅğrRgÁûd;yoé
Rç(}úÎ,4oÑƒa(¶‚à/›¼œaºŞ $x.÷jŒÕ~ÅõxUçJ	¸û›¼ÔèPsùL»#â„	 Ä¸Z×‹‡¥²âÊğÃ.D~²ó*÷¸z»Ü²†ËÜªs…§¯V·=ÊHÏ4.Ñ§úã(m·Î%ïğÄôz&õ{ábËÛ;4Ö9,6O²wÏş×œãBÜª}’oGˆ~¿µqøµPàE
c„àÀ¸¿óƒáOÓRóüã®yî04Ú<½	™ï¦ıFÕ¾›³åŒQB-Ê­æĞ¢={ ’ ˆÒbøÇÁr+è‡ÔDGÕF4Ê…\dÆæíÃÕyŠ*új¾>D»ßf¸…`‡g¯>=³}!*ºL!™wn†!Ó!% <>èó&» Î„xŸ«¯”\¥(©`>×€?ó¥	ÛÕ`üj…‡)gÅ}€ûš^µëÙ:§ÍÚ'Ï¢ld5“RĞ)u¶†)©NİkÒ%éP¼¼¸º
_¿Ò¯¤­¢ ğÁ¢üa¡fŞ17áYKf÷%«ÿDmøM¿é{“¢‚Íîé[¼Ğ`•Ÿ(mÚ_(¡°%¤)È–ŒvB:İy‚ãLAsWªï6TQÁ—¸RèÜbjÙIÊ·ã"'İñ&Ÿ,²XRÓ%™‰Æúvø ìÎid{j”˜”Ò¤‚ïPÈhIËÍY§×³t§šY|7Ey
=ÑÁ*Ør~Ñü5lÔ&!”D87zu‘ğÀtq|†5	–T˜ƒêÅ‚¶bœ3|o5‰.×Ëˆ±*Ş7?K›É²“#6ÁBEŞ¯½Ò1*ÇÙ„+qí7jÿ»°½AJŠk};f;*v~l´%ŒîÇåfàöEGŞ"jşİAĞ ğÏcHUOıÏ“Š)úvmX¹Ğâ¿‰OÎª0i¨«Œ—Õ47{¨zf< ³ãWĞ2qğ¶¾ìÊ+1t$ií]Lf~÷ødù£Œ9ÍètwÆ>ôv|	ñUıåëÎ¸/)¾sÃé‹6œ
ù¾ŒmŒÅW#	<ùmïîã¶ŸÓ`|vV» @¥â¹Ğˆ]¯#¯ÖâÈ£¿ìû˜Ê<•—m,[mà§Ãl]´M~ÔÖ>@»‰ÂS…€iš7À Ìâ–ÎD¼‹1ÜUç2ø%B„,Ñ :¸ûµBX}I?«Äx8n]3>.Ø_ã¸ö	ºiÕS*f••İ#ŞÙ³ŒÕÏ½g0$4”à? ’‚ÀAŞCÂayÖïYú%%ù@€h¥oáÿş²éÿ‘©ÈÕ|w°r[>úk_˜DTëÑÆÈ0ï•é'Rî®ˆÄÂT7ÄšrğwÆÉ¬9‰ˆzˆõ–ı+Ò¶Ø6ıÔ•l2*`Ô4â9êz”i8¥¦ÿôR­–û;LK÷¹ÇŒ½P`1ëçq¥½¥û¤ĞÄxÍÂ¼I´(ñá…çOŒÁIí~~¦oœ·å¿(!‘îË¬Ÿ‰Ò¯¥¸Ôè+’åe¸nK$ Ş+ìÚ«r@È_1#ğ#ú9‚l]¨Œ®ÃÉI-0O¸ywÕÃ‡æ¢Ö¬†Îõõß¸›fù÷YFV:êÉ¯ï™@›Û"mÍ».~†º(Í™9™/ËnlO•åY¨ ¢bÖÑY—Ô? M©¯‰0²UÜä0o)´¦)ê&Óø÷ì–§>böõYCöxªG€å_›~$pÅ¦%V8L®¯·ÔhbpH’Z)-®êm[tdíT1÷Ø‹÷âAää]Ól€…â[½Ôšû†O+û
VæAX÷¡r8oõİ¦5şn+°5«(¼iFB!½;{ù/Šìà$Äyˆ5<ì-P~€‹—¡dÜ_ã§«ÊiO{VxzÈ^7”üÕgåIW=)˜`cê=¬qÊf+¥.(Ü¶¥SU.WIÖãzµû	ï©ÆòÎùî¾âHZúUbë!SÅÒ­â=är&¢tÄ
n‚ñÉ#]ŒÈÕ)ÍØY{3!î³ëÎ¤T©¦“PÃååbùl~Á5¤Èï^ÈÌ;n â¼2¢ TÙ]•ğõ˜ÜP±ä@4d©iYîÁ/Eh®D°o\5ˆ0ğH¾Uü/ÛzZİİoy÷¾å=¿Ä‹~KA,Xµç/J,íØ`›¶äNÄ
BøwnQÍæØºtÔw[–Qƒ+EvmÅ&pHİÊôñ¡£&JhJïv~X]~ûã5vÚ›)q¼_€¢Ršøš€kšõ‡vˆW#ÙÖd>ç¼7ŒƒïDBZÒ”PáÃ$Ï+ò‰Öª 6±ù#9±ÖÖW7Œj¿?M÷¶ºu/¯»«³r•‚f»³×+øiû­…¢`Hè·ùáAƒ&Ë‚‰xıáó«m¥«ÀAd	ç¼Â¦½Ó³2Ar!*Uwi“V“Hk®¶[ÑÙ¹3p+•çbÙÔ‹ÅĞímXç[ÕRë•^U„PC¥W ùtÃ¼Ï<š7{Ïúœ
ì6)‹Ş9‰,P<u×Z8)N—Ä·ÇÊy†ÃıÄ=¯E6‘`µìQ‡bÓÊ@ JÊéõ¥¦„LUŸ5ÃÉ_2ÈWJ^á>¢‡-<„™°p‹?shØ"ÓŸÖ÷ˆ¿D‹×rğÙ fƒÇ±ÒÜZhp–NZH°•]†êô,ÓÍmãËzlè¯æ4ì$«Ê"ttÆÔÅÀ].?g¡üI…0’Æ¨Õ?j0Ò@(,‹.H´Îiõ[‚ï–šÀ€æÿ0t¾)~ç#Qğ-9Z‘Sº¨Ö¦§WxK~×ç^Vnå-µS.h%ø¾²&³¤dthõÅÅ¥z¨)´áÍ1÷`z—§ıg"S‹°U÷vôI"oí‡Sÿ–R7ƒ!qT/ò¸ÇXŸşÇCNÑ-º­ÆPüãWÅ‚«—Ç“ò§”vûîW<¿*'¬k\_àB6ø/	l‹ÂJu¶Êëø@aŸ/Ûi}'h1ÀFİHî+lã³•ÀÃŒ^æ" ã{¡÷CY‘à:(ÜKöà,SÕ&¦ëBy¥Œj›­=»G÷±KÒÉWM˜ú§XDÿğPg8¶”ûËØ‰ K+1?ï¶Ù>“¼¶oU´RïfHfï¶o)mnŞKB¼ëÏ±5-¯^°®E1¯K 0d±_ïR0rqËMÍF‹Ü”ív‘á¯|ßqéjšª3APJ˜•R	Òëáš'K–µ¹(§SÕÿ•ydòÄÇ6‡ô¾ˆF§/ ò5Zñf&Aù‘G’&µ€¶*o‘Ò	HMâ$AÉ°&dDáò—{ÂdrcQ0ù`éI‚®läÕr\6­¶2Ïê5Çj¢xQÎ¸QäƒÙF`Gç°éŠEê9^Ã!»WP•Û‚-²ËÒ0ÀòdÖ£×€°ÿêŠœ:¿^Zß8›:Õ‡®I[·±Á·F–fj0/™ÍÓ`×ğúÜ°Œ&7'€¤}-ôÄaOèÑì.ÄÛÉøT›³´¥\Z·«!XÑW¡Æ‚«Ù3Ëüc¸Åï¶#DT® î­Óz§(y–‚¶úÕ¶-“LHËG‘NbFãaMÿÎSi=’vËò.JËM¿ÃSÖœa£á4F´>İïËş„=q…½»3w½ã°‚«äiŒ	ˆQ¹é…R…iŒC4£xâ*öFã¸ì(W¢ZY
|ùñ§ÿ%ÜÚ¤¸"Ú=´¯m†){;ÇÛª‡ŸÂ3 §‹é)Yó²kwoD%fv‡¶ƒ²zßy*Ò-È&3¼å-gkÃ%mGê4à“—Ú˜¢4UJ—Â\PA+ÍãÆn¿èOXs×´n!çz}Óg¬û.ı½Ïd,r4w	†¦(‰;®İ—w»üÎî e>€R}ûÛ>ÏS²•»³/-BN Œßnÿ£ˆš¨[¹HV!;õWÓÓ»F!å1h¤ø·šVTø{•LòÕ%İ/ë·²œr½ÈÉ8{MßÜ€‰E·/obÃg¯©MuË¬<Ÿ$Äˆ_¥ÄÖïÎçûöW¢€è§9yÌ—)³ñ†·˜rPŸuÏP>‘!&YÅ@ ±|…ğOúìx=ZÉ}R~ÑræÓqê‘dKA¢z7ÌÏö,'ğ§K	ç+Õ¾™67¼Rëú8?s&q(Õ.Š¢ g¿'¿Å#€Şe+¨!põzB¸ï'p\Œıë©”µë“¦J~“¾~1°±¢CUaFŒmPùYŠ”Ò2T}a™š8Jğ®}sˆò…J·[Ôø¬õ·Ôj™Kş ÈºtØ¹H{±×´üEy0û¿  5(±nè~	/Iarû¯_×@z¢È”¹	i å/ıpc!_¸£ˆæ‹¼ÉnQpKøo¬h•‘N‘7%ÖÙ—è“:ê¹×õ+CtN¡ùñM8ö­ë’9~óHP{WóŒÁ™Ö_ÄtËˆ1!R§\8»OóıÄ–~v˜ùf;e;“ÿ}{ºc÷ ï³'çp“`­§ˆüÔµ|\{xK»r²àKy«ÉæN?	3 ÙMŸóW¼!×]C¥“Y kºÈ4å?¡Ñ€”dÒLÌ+<NŞušÌ+…;aÈ\gê’_©µ)MÒ¿à»ÛÕ‹™ÜÓÁÆØA—aÌ%œÇ!ĞÕ7h#úèï®*­Ëå¸RÎ9ùb•³Ş‡ø£bû}år³Y’óXGhëÓĞ†9Q÷
•dMÓ1Ö¿[½!(±ªÑö¡àü˜Ævné\V\náî—ºYBî*M8ÏYğ?Æ÷€Cİß1—–êG1ñ-Ø'dĞR_•ïÃ‹ £«Å}ŞÒ\ŠZ|ÿÖë¾I­"Îø`¢Ì)÷ ØêVÿÜh08l1ª11³¢¹+£øU6†N¬¸ûƒûPÙïK.L|ÚH`½·'ÍÌøsk/|>uz©ª’+}*ÄËCY„¹gÁÇÄÆ³¥²ö5}p5v¿)ÇL¹ñ2$Õ§¨'	ÁÎ_xMÎº,£ñ¸×·.Î¤Âsn¿îL-Jşm…E´Ùİ¾mŸCÓ0Œò\üÒš”Z%ÕJA^{
dlÀc{(ìvtÍ¬liåei¼ˆ°Şo1øĞc¬æ ‚ÁcÉ³…îÛ„˜.d]ã-Ş/…Ã:XÒ,Öán,j£´ØRã€3ãD)ÀŠC.ÌRóxËĞÑw˜ç4r2<Â,Ğ™T¹êÆëOÃ¹K\a’$Mã¾‚âF†ÚË¿péÅ°~«Àx‘´m»³‡ª\»‹şíLgoÔt½ª2€ŒF1\Y€‹gáW¿”|ÊÔoêã•»Ç.×ŠeÎGtw%*ŠÄ^æt.ÍŞÂÿö_è”ŒàÌ9g÷¨n.,İÙË“s«n¹K*Š#	r-¥O´@` •¿™2Œãx ÊÊ}–Z…wÿ$Öù‘ÀºŠ,=Qç”QÔ‚‚3ĞêrÜÔü«ÂJN*N¡‘‚7 "‚i	scr²ùØ´7qÿ-„Ç‡šq)áR:<waéfÊfüu5/šÚ÷Î'\7÷€ïë^''ôpeİ¢x Š¤öœîÚ'ö¶$ugiê››w0ÅÚş=eôv†}€
†Š‰[š†k¹0Á²8N«dß×D^ÙˆÃBj‰|=Ï€ã«ršé^!—Å2ò=jzªGäA*+D1+ôÒ÷@
Ùi9x£¿+`Y.1R(ä¸Íká”oœº1Î=V3
&Ê.½…öÙ>tø’°¨È+93Ø6Sß=}ìï <õjZ!1¼t—íw¼9Çxİ»9‹ğü”œÊÒh½—FˆÒóÊfßéˆ$Ş:qgê½ïD2à&W,<”F†àFµ“H„™–N “vPš]°d‚ŒÑÈıg¼R0:uğöûMWùšò‹0\*(íâ
ÉìÄ.ƒĞÖip-izn$ÃPš[MÜËú5‚‹ë0(Oí{¸Êíé¾EkrÕ ¨+UÄ3ms'1”œì¡0T"t·±#Ñ°W~À)İ7mz†K	vÜ?p^:-Nû`šWpuÍ…<\ïy$$î1ÒşôM×iYñìW/@«±_"GP_Ñ—ø6‘°İÓÄClcH™<°a´zVÎã‰J_r¾A(•á“`âfAVxÔš]&1×•¸°tn)…DéÌëm¡kQa™E¡òİá@XJ,6öfj£ÊÅ°²$âĞÑ¸:î{÷’»Š$ú~êôk.Ú:¡õr§‘Y·ù R.´U @Gàê®	…mªZº¢Jp¡:Lv_ß>ôxx ¥]3‘Ğhß¢=¶Bkà¨o¯<¹Ù>nŒ Ó¢Md5ìHİ?ƒ·Ğ($“	ß|• †¡!£èw×Œ8Eo.ÂÅõõ²qmÊĞP
Çôñ?dafÃùBBaŸ6q®ÅR=€™r$]ª±÷ê«rş3Kã*µXëÉ§É][İ
±ËŠŠØÜ¾-ïg$pß-§W9öıìùµ	c³²ı‰^ƒr ‘1soŞª¾ƒ½ÖL3úgk°08õÏuÉ8qqdµx*q"òËÃËkî(…ĞñÆ\¿ªòB0]8¼?¹HûdÀÆßó È<Å´„Aˆ¦íIf¹–Ó~VhvnúZ1ÂEµlğXâú»‰`ÉTà„À½dM>}/3Q?ŒÍÖ0ğ¯"é¤Œì‰®¨Z6y:Íœ”¶Ğ¸É/Mu×·¹<ÒsŒ£K—mšû÷«Ü™<Á81¤…êPllI{\Ÿ	mrğ’Q œ"¬EÓ:Â|¾áÕT++lÑZFˆC³GÑÎ"²éôÁº£Z® 0–^õ£çÜFãc`úÇ_F‚C6hMP™víÜÔ	M—|'Î„.Ä´¶<V¹™wÏ¢4¯HV:,Ì{-¶
8e$B‹¥È½¥ƒaEkÊ¨Ûxw/¡PÀ“Ü»¿}d]ğ×oƒf®lWN×ü„äi)EJNÂ‚‚Pˆ <cìsßfÕ;ŸbóúÿŒx)nY ½åñ{6Âø‡7ÈaøamŸ¤|š5ş(ÑNÎ4{d2!{­UPoSµYúºy?¶€Á`¬aÏÇ‰àŒß†‡uYĞuÖpªÜ±?–‹ÚüŠÿæ$ÔS—½ª®uú²Ğ¢’§àSSIhœÏ|ÍôéOP‡³'pØ9Ò&
»Í¶‰Ş„·3Â'K)O‹Aryƒ…¼÷Vª&/D‘».gor­2gqîn¸_»Üğ9ÏûmjÑØŠT¢ºvz3éz9‘­}Àx†²¸én×¾-Ì˜€¦™àËÌØıK·:pXÔ‰	qLs¦öf^ş²_­DÛ«½«.¿İS•WfJï¨Òç“ƒ‰MCÃ“¿	ÿ‚±©*k$~lJvı’æZÆ•pó¡Ñıg…*^€¶¤[¦”˜±7¯Å>;WM&ËßT#Ç•ñ©J}NLt¼? ŒÏ\õ=UİÃî?×5ÉÍBÏ@“CJ=7½wæÊ¢\p´m¸]'Ğœ‡è»cût c)ûëK9Ìrã"ôyİ“K3•e‘¼1kä”Ëy§’»©€#*éş›³µ¡‡îÂRáÜ`0uï##ƒ	Š©:}®îwã•KH?¶Å)¥rˆe¬…Á„°2üıF‘†vEŞÍ²È”1] x³ahâáùE³ô·oe_S6€FáköÊwÙe•Dıà¼ü3ENE„4‹HWƒB†ç.Ÿuét…+(åT:Ss´©(Ú  )üª—Ë*aŠë\«€£õ+Àö„˜ Qüø(Ğšñ•ó ¡½€©wYÕUíü~‹ê´*Õ`-æ¼bÓE}÷ÿºş…G™cZl97	“r¡Â3ÆnˆaÍn¦Ÿx¯¸yµ¸6Áæ9J
û-¶€„^şM¹e’®û›ö5Ÿ|®9PP,V­µJœÏÜ¾˜z·ÿIbƒ®54ë}©EAºÓü"ö†º_ÍHã48jû(2ÀuT_‰R7½iæZošK}w	º¹6€¨ÑÃmú7AÁtC˜]¯÷i™=¦ÒXÒVÄªa”Ä{GFjAÙT0Ô/ZÃÓóõw%Ç%×“‹²ê¤ú÷'À»ŸAJ3ƒH…:=° pL«r¤9JJĞšã0õOIĞp3ÎU’¨ø	»–Ùãc™™|4ßÊGSP¹ÄõQCÄ¸½ş¿C{ü4§Fëcˆ/8 T&Ä›¥º¡1ŞTSc„¬áÓ˜\# d.î>–#7ó%Ì‡Ù˜÷ErˆZº¾ÉYÏYJ·N-Ë¶ã¦˜c4ü¦b{§¶ ãĞC‹Ş7f8u?éÇ:!îåQÜ{¯š
±‚y¸àßü÷q«¸ğqÑr†¡>ÛWùõµU­‰§ÑG.’&±"¦0I¶ñ,¾`Ñ'½²)k
•k«íØ
íDëŸÆ-’~õcºï›o+tRvÙğ¾uê¶£°1$â ãê®¨º&mìÖ5grü§¤jÅWY¨/fqß'\¸££DŞWZEûÒ=C±5ª¶GyrŠÉeÇé¢SM¼VÖ÷ıòõŒ$Ü~¿ĞêÍQĞy]—/)îÉÏ®ÌºÔÑ¹2ò5÷W~ù<“«3Wwa6Ïæ™ªfú;^­M4êã£›æºf`$B\K3Ì§ef»{w‡ñ†qYNúçp„°Z„@v¥+)œH9xÎÊÏûµ:´ÁK¥LHôÿıg/ÿèŸ–4KÄ™¨’DñÃ¡?ßn®ècÍáœÔØäà!…ô‡w[,´ø*n"­¯â(ş±®Õ¤õMàó_¶=I È‹ĞJ EçC¯íˆ[3{9knlÕfbé(ƒ‘*Šis:€"…]}ŞU?ŠXÕLÅ[Gî‚¥õ9pÜ\´l}Éüàà½}½“Æ4¼·ñHè¼Y'Ğ£/ç—òù!7.ygb ¶•ëäçbJdËâáå¶s´ÏÜ±`å¼ ÊØ+Û(Ü­ÇUÙõ¯<šëx‰Ô'8Ş›`Í’¯¢Ã”t¡ÂŠtóH‰{mUpÈ?£üºGŒwêS¶ÒÚ©öR?ql9#nÅguY	²®Ö¯H]”UåñÁ']tw(‰.u/6¥§²7
§‘æŒ>ryp|WØ¼òíá&Ì4Æ0]†ôq¸Î—Y_’e4iÂ½\iŠ«hÈ&à†"~ÈåM‚eÈ“>%NŒg€ç±¼õ™„ÆsÕ Z"°+Ç‰ZÀ@êr–	5Vó«7cBK¢ ™dæÁCQ\&æûI¤D!úË~À\…&.@æ¯`Hb¦)À=‘8E
Ù³€¥´úl,¿®(Jr>PÕCxæÆ3–Š€jw0×(™cUö¬TÖ©\˜6ºÛYr 9h!1˜µÂ~ÕCLÿÚşi2š‹ zÉ;,êoÊı‚©Ã­±Ñt¶'¹¸j<™‘!mQ{]	x*(‰¡İ‚’Œì¦ß
’Œg ’ö¬F¯[ëOÌÈd¯¨F&¼}§!!“nx‡µ­z%»]9İ±‡ŒMäî>~BÓfåñëgøÍÏµ×¼Ç4‰İ¸Gh•hR¡é ã·†û¥'‹©.M“q"o†öäá¶¸=Â‰‹¼H3™ô gÍ£#ƒ¾}.fßNÎìNgkCÓérµ‰×‹W×Ñ®Hã×'±Œ´±eÿÕ¤$ÿ‚î®]CJİ×b“œöákÍË
¬Œtzãd'ÛwBøPã«'òÓÏÓÀ¹ù Ñ?pÈşA…¨IŞƒ˜·KØ³h/ULfFıh-á±m¤†i	oEÕìsv“H‹¦ğëÜ	†ÉÙ#İç‡1H—~`±½Eˆ{RjaŸú`Tù+€’ÍZd­ª¾äv™0¸Cu*^Ã”zóVV{X½Ì§oTûPïBõYH$‹ú¦á·Bj÷HI5\xƒµs€Îé4¤bg1rî¸çZßCo‚U£Cêê Å(¡'ÑSõë\–-3­{â@²Şş–î“
ƒƒêã©½ëp\Ä€1'MB¯¸FÊkö­~0Ê£éüºº3 xhøX`u«”/ÇDÓ øo™QX5@?ü«¯‰àš{–nïı5ÊÄ\×Kzƒm™täşöGä@Éút£UæÅ ×xò}C¿>­*¸Ş¸ –7õ»³‰ı¾Úö\ÕhŠ1Æ--1ô™´U^ì"”k˜½#'pg„8Í\AwpÉ’±ÍLøÌôªj}Ÿ¹&4ŠkiÄôÂÇonèY<¨µS‘r%@>ë“ÈØ²ï<¸.jĞñ¹ù¸ÿQæúĞp¯Øêpa¡hAt0áÀiÆ”‘©­@¯†sJgƒÚu/½Xˆ\i%ÖWRØå+‘ê‹w2VLÙo/À#r®õˆ÷6Ÿ>ÛÖ^¿ÄÜEbsiıufª·lø(Ã»±8?é±cÿ/o¯§
2†,‘OÍÍH'øêİ±­8ŞJ0…Šù6Ä\~6‹/^:“Qç”5ÍS-8\ê”¢eİŸ÷ŠAi¹–#ˆIxdµµóòÔÅ"fzÕrÂäıÓ;ê·Q+ }‚P¨±ğ¶Fä%grÅG¡;;öD÷_!ü™v%gvZÿ±N Gó‚Íª™®ù¼VNû(}GFÃÅIœÓßª ¥n&‹l8'µ?m
LĞ"4–fí×“óƒZU*>Vá-[Â6­ÂAºÑ”…Nx™§Ø#œÁU‰[‹a:¤¨·»3¾2-¨êW™'5Üƒ)å¦sØ}¢¡Œ­	_ÆAôoDnf:jt$£\wJ´ÌiÍš¸üï¬Á=•ÀA¯Ø/·“ãG+ÖV_í…ÉÂ34ËØè|s£áy±y=ò8óÙú45Æ3Úù#%²±Ï[*zß°4j§ Vœ‡ZfçÍw9KÜ†eàëlŸ±æŸà6ãBğkmÇç{d‚è‚û°6è°1`äªWÁ ‹¾&Şß ‡Çój¼m"ş{vi<U?ÕÙûìxß™$¿¡t+zLÿyûDâsÙÅü¾ä%Uä2øû&ùõ“Í*Â;Ëåª›á9ÏYÖíµÜGèoıc{'còa9ÁäÈ™«3!ïnP’Òo›ZvÙ¤½@¢MçÔ[ÀPãû~y)k\y¯¦7òMY†)Û‰1Ç—cğÕ„«ß™Ö¸iUø¤0"1ÕÀ·ôÛÙUÔ‡Ñö*×¢ê,‹ş¬’:¨iIÌ	nZÏúşİ½*Â	ş„×‡à‰A%|­?Ç²—nIá4.<^û¿øéÅÂ{˜ J†BR¡E`E2îu1°Õ‘ÓÖI¡_CAUH;"DµT”S—fÛ?,T5Më«2íb+uÌ7}¤/j~KêIvX©+©­‡'¶/Gu™YR¤ÒàhÎÈu¬/–mõÜ¡`?ô†ÆÛ‹G#©^5ÂõaĞ•¹ànM»÷Â”òÙÕân#_Q’š71RtÑ”$%š¿µXeº½ 	>øI½}…U—"ó»â¯‹î¦šÅ€wöû{è ³lQÍ£>³ØùÍÇ”ìjIéùÛ_AtPĞH4#	bà‹ù/Añ×:K1±WzkLÔ÷—_%u›T¸4N=èu®™ecµÂjÒQlù¬ÉõÒ•ÃÅgÄòa!éÙğ¥èd¿!GCîí«Âš‘¢PL}üCny›~ïøów}OíC·WhËİ½Là% ?(õÀùV¹Æğ0OÑäå%-Dõ	
vkw7º·\änâC-œÔ0Xï¿`ËdÒÅ}~d.¤ëô‰pHXx1€Ì–è8_±ø«_ô{E²K°[¸;ûV)«=pTGŠ Q8ndò´•í˜>ˆ"È1È¬`ùN<XÅâš÷D%b³ZÔù¾õe§ŠtBŸ:e­5šâö>j'sK	Úk ,ƒù	2ĞågCê‘tû˜xq0)ù\#¥¸«ù{§°ÈOo0Œ‰Ÿ©ê”âP>4œ-ÏS#éÊĞç=ÄVC\-–ğ×L‚…)ĞK:ÿ¨™ãëïá	7ğÜg›JQ:tyâ<—n¢`¸)üÙ¢¡ÏF¹U”ü:Á‘´áæŒc‘@ÉHC.Kâ?\—×Ğê>Õ³w²À÷Vá{’Õ7AHE9~ªü7Å¥ØqA[ š^;Y¦ıßÕµ{hĞòá…ÊËgÁŠÆ(vÉæ‘\.NØj¡±hÒRHh=‘­³’Z+@öµ›ÿRÏ:!1@rğ¤€Ab lÄyœè	Â(ü±®Ş‹g‡ØO-ŠÖt€0Ê¶Œx88‚^ïò;Áé“	Ã¤:y	"ŸTÕóã,w4Á³2Cë¹ŠÕr†íp*˜ı‚iëG½RÇÿTÚ¨u^:|ÓâéG0¹Q›gã3Å¡İ°õ6tôxµtÒ*JéY
 8¼˜›/ØyÖz`wR˜³Û3˜x¾nƒÑ]d>v&“qW÷š×ß?ì`Â€ıKÌï 0 ôyFÉ5YZàQY³@0]šá~Ø’ÂsD EŸ‡oÜ5xÌï`MÆÉT—›ƒUòôóıV&U@R2~Ãˆo¼ß—-åÊ'5ûø|Ía¯Y¿ÀDÒ„”5„ —?èƒúÒÍ.Áƒ˜}†Ü˜+6h`¬çÓ°†YË•t¬_<¯HQÙSÄ™f	6¤Êş´Jh6xÑîÙ#wë+Í+fÃjs÷ªGz%Âï„æáÄ.…œNQë•:©omOoaèJ4…éUXló¬OÔK.ûÜİ¦¨nAœğSXU„î¡iæd~ğıíHª½hÆ,Øš¢»½}ÍDù}eORŠ£v~ê/]Ab›\2
µ1×¨EAk~a{zÚât™ZÓğ£ÇÚSæ_íí8µ9$ŒğÉ§‚®zëKØz_ÛŠ×`ZÀ rï4Éîü¯J<`Õ;HüÙßIt‹¡¬€¤ª®­ÎÄÇ:3î»G™À¼½ğa¾{Ÿÿ‡¤Ê/O ¢$•ˆ›šŠÖ'®?ç œôy
ÊDÓım1”%ÄHezSsä^-¨A”íõ¥m«¬ÏU™hßQ2ËW¿Ğ9oW
âQ†ÉcU¶ômÌK±BÇÚÍåK¯¼Ml­»h2B¶1‰ÓäÖõEÊCPğN“Ñ3bz#X½ş…áxuÅ±)«VLµçqŒª{ÇŸL­!××%@úµßÅü’o.Ü…~/ş#‚&ÖábÂ 9oi'±WN}íÓo‹Ÿ·¿vÄ– (Şñ³ò*Y`j™ —•ã$¤•R‹)¾ÙDN¢ˆ¤–²}ù
ôjº“!r]OÊ,…>½·r²Äƒß5Im†¶İ	¢Hœ(şøz«É)„ƒêkªm·ÊŠÏ4SÇ ïŠ3LœÀ7ª­;r¦=…ŠEµÌÙX>c³²³Ÿ÷D£‚üwÓP	† ¥û íz]Ì¼#Që-HDOâ­QL†£_e¨xe¥‚©qaÑ«°‡ğ
ÛŠ—ú˜ã°(²€‰«C<Ó²›yovåYÃN™[xæQO¾>«%Ïcq>yfšèTïg 	İéÿeÙ.†8Ù`Â´”3Œâí½5²êø†Z}ğ‡ÖR¥Ò?©å€¡ğé3åOÉ–%«¾Ç¼›ÀêÍwÇæ<~grG Îét@lÑ”>…`6‰<˜@SŸGO3Ìh©s[ËC*Q?Má˜¢£Â­Ÿz-"Í@º®±G5Çÿ½úíÿ{/
Æ›_[“)vÙâ•LO3ôZñÊÉÆü…ƒ@Hèã¾¾Ó^Ú5x=Ë79½(Ï\ö§Î×1Éá%r¿—Ùèó÷§O¦¥(›Ä3>º*´á@›E¨*X¥^ı)u)ËxDR_¡ÑÑetîå®…0b+»£ÏL)Ÿ«^ÕOÆJ8#°SÔ°]Ü¦á¥°Sˆãe®cÚà(ø.*£=JW@¬¯1]ö¦ê½G:~¿„qFŸEPİ’)]'˜}Q¾Cl8“Ãi™Ü-©÷”rK°V±Âİ!!ÏK­›.ÌÉC¨‚ğ–ıhŸ5Hœûv»p¯ƒVÙ®Ù6=ÇÊ‚¤`|*Ê*¹¬ØİÆ	TÉD‡‰?CÒôœ ,«·Góçu)koİ…2pÖå•.Ó˜ßù‘l»ÓIÄ	Ëğ3Ryîğ|ÅûØÁ„êI|oLGL»_˜ğŸçuÆåÖÁ¸”ô6o`ø®ì‚QoúÔaQµÈOyÀ.™É¦½I…öã†xŸ¹©…â*!µ7~Ö‹ÿtuÅóhÇv5rá AŞ×“%7”êbMÑ5Oe"¢“ÔeZzRÖÚzt²E „xµ?_ÍƒÀ•¶²	J(3¿™>ÇS¡"4ÏñÉÛæ}Ãºy_¹R8TÁ¾ ÊõâÔ•·¾ÅÀBVûPË4/Æú4ˆº¾K^>DÜTº•o}»P·¬ÿ;&¬O:¥Ïé%Pñª&ôr0ËOî lz­µl7Y²qF85üF—Éß!°Ãİ
OÆŞu´3X^­2uaİ~áÉê*¾^LÌh]RL}³ u…‘‰àUò‹Î¶ç¢0P#A¦L}½`Œ	 i³NêÒdKŸŸøŠô¨åá+Ì„¡›Ç3øšµÈú´¦Ç¯§2Tğø€wºYšñÎ»¹$ëújÔÄøÆÙbµ"xN•DÑ$Xbbš
Ùk·g›k›à$CvÏºoÇ~’÷.,ì¢øşì6Ê¼ÖˆúÇTÖ»çƒæº»êÚ
š^kÊ:/èy¯23)_§›‘}ì,Zòp
ÑÜâİÆ<¨