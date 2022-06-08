include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/vdso/math64.h \
  include/linux/time64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/ratelimit_types.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/msr.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/bug.h \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/COMPAT) \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
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
  include/linux/osq_lock.h \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/debug_locks.h \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
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
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/generated/bounds.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
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
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/build-salt.h \
    $(wildcard include/config/BUILD_SALT) \
  include/linux/elfnote.h \
  include/linux/elfnote-lto.h \
    $(wildcard include/config/LTO) \
  include/linux/vermagic.h \
    $(wildcard include/config/PREEMPT_BUILD) \
  include/generated/utsrelease.h \
  arch/x86/include/asm/vermagic.h \
    $(wildcard include/config/M486SX) \
    $(wildcard include/config/M486) \
    $(wildcard include/config/M586) \
    $(wildcard include/config/M586TSC) \
    $(wildcard include/config/M586MMX) \
    $(wildcard include/config/MCORE2) \
    $(wildcard include/config/M686) \
    $(wildcard include/config/MPENTIUMII) \
    $(wildcard include/config/MPENTIUMIII) \
    $(wildcard include/config/MPENTIUMM) \
    $(wildcard include/config/MPENTIUM4) \
    $(wildcard include/config/MK6) \
    $(wildcard include/config/MK7) \
    $(wildcard include/config/MK8) \
    $(wildcard include/config/MELAN) \
    $(wildcard include/config/MCRUSOE) \
    $(wildcard include/config/MEFFICEON) \
    $(wildcard include/config/MWINCHIPC6) \
    $(wildcard include/config/MWINCHIP3D) \
    $(wildcard include/config/MCYRIXIII) \
    $(wildcard include/config/MVIAC3_2) \
    $(wildcard include/config/MVIAC7) \
    $(wildcard include/config/MGEODEGX1) \
    $(wildcard include/config/MGEODE_LX) \

drivers/media/pci/cx18/cx18.mod.o: $(deps_drivers/media/pci/cx18/cx18.mod.o)

$(deps_drivers/media/pci/cx18/cx18.mod.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ELF                      p'      4     (                Ë¸ˇˇˇÖ¿t?Sâ√ãÄà  Ö¿tãÖ“t
«Ç$&      Ë¸ˇˇˇ«Éà      «Éå      [√ç¥&    √ç¥&    Ë¸ˇˇˇUWVâ∆Sâ˜ÉÏd°    âD$1¿É«ÑŒ   ãñ  Ö“Ñ˝   ãÜ,&  Ö¿Öç   ãF«$    1…∫ˇˇˇˇTj à   h    Ë¸ˇˇˇÉƒÖ¿Öt   °   π   ∫¿  ã,$Ë¸ˇˇˇâ√Ö¿ÑZ   â8π   ∫
   âhâÖà  «Öå      ãxã(çGË¸ˇˇˇçG$ˇu¯É«Dh   j PË¸ˇˇˇˇudˇu¯h@  jPWË¸ˇˇˇâÿÉƒ$Ë¸ˇˇˇÖ¿Ö   âû,&  ã$Ë¸ˇˇˇÖ¿Ö®   ˆ    u-ãD$d+    uÉƒ1¿[^_]√çv ˆ    t›È¸ˇˇˇË¸ˇˇˇÈ‰   fêfêfêfêË¸ˇˇˇã@«@    «@    1¿√ç¥&    Ë¸ˇˇˇ1¿√ç¥&    êË¸ˇˇˇVSãpç^âÿË¸ˇˇˇãvâ¬âÿË¸ˇˇˇâ[^√ç¥&    çt& Ë¸ˇˇˇUWVSâ√ÉÏã5   â$â ÖˆÖ2  ãhÖÌÑ¬   ã}|ÖˇÑ∑   ãw`¡ÓÑù   9ÚÇ  ãè4  Ö…Ñ7  â–1“˜ˆãSâD$ãGTâD$â–Ø∆»â—âD$ãD$–â¬ãD$9¬svØt$ã$ãD$âÒË¸ˇˇˇâËË¸ˇˇˇãCãt$âCãOTâ¬) 9»C¬sâsâÒâCãGL9∆Éû   ÉƒâË[^_]È¸ˇˇˇã-   ÖÌÖ  Éƒ[^_]√ç¥&    çv â¬âL$ãD$) â—ã$ØŒâL$Ë¸ˇˇˇãD$ãL$+L$ã$»ãL$Ø∆ â¡ãá4  Ë¸ˇˇˇÈTˇˇˇç¥&    ã   Ö€Ö   Éƒ[^_]√ç¥&    çv )¡âËâKË¸ˇˇˇâËÉƒ[^_]È¸ˇˇˇç∂    ã   Ö…ÑPˇˇˇÈá  ç¥&    ç∂    Ë¸ˇˇˇWVSãpãçÉ   çªT  Ë¸ˇˇˇ1“â¯Ë¸ˇˇˇÄ£®  Ôâ¯Ë¸ˇˇˇ«É(&      ã   Ë¸ˇˇˇ1¿[^_√ç¥&    ç¥&    Ë¸ˇˇˇUâ≈WVSÉÏlãu|d°    âD$hãEãâ$çC¯âD$â«çÉ   Ë¸ˇˇˇãÉ<&  ãìî  â|$dçHâD$\çD$âã<&  âT$`Ë¸ˇˇˇÖ¿Ö"  â¬ãÉ®  Ñ¿xçÉT  âD$∫´®  s6ã$âT$ã    Ë¸ˇˇˇãT$ãD$hd+    ÖÚ   Éƒlâ–[^_]√çt& â–çæ¯   π   ∫   Û´â«Ü¯    «Ü¸      «Ü       «Ü  Ä   «Ü  Äª  «Ü  Äª  «Ü     «Ü     «Ü  ® @«Ü!  1  «Ü(  b   Ë¸ˇˇˇã<$ãL$âoâé   «É(&      Äã®  ãD$Ë¸ˇˇˇâ$ã   Ë¸ˇˇˇã$Èˇˇˇã$ã    Ë¸ˇˇˇ∫ˇˇˇÈ˝˛ˇˇË¸ˇˇˇç¥&    Ë¸ˇˇˇU1…∫   Wâ«VSÉÏã/d°    âD$1¿ãGTjj Ë¸ˇˇˇâ∆ÉƒÖ¿Ö‘  «G    ã$πÄ   ∫   Ë¸ˇˇˇ1…∫   j j ãD$Ë¸ˇˇˇã\$∫P   «C    É√Xâª`  ã}dâ¯Ë¸ˇˇˇâ¡XZÉ˘ˇÑ   É˘PtÉ¡É˘Pá¿  â˙âÿË¸ˇˇˇãD$d+    u
Éƒâ[^_]√Ë¸ˇˇˇSã@@Ö¿uh    h    Ë¸ˇˇˇXZÎ6ãê$&  Ö“uÉ¿h    Ph0   Ë¸ˇˇˇÉƒÎããBË¸ˇˇˇ«É$&      1¿[√hh  Ë¸ˇˇˇ∫    ∏   Ë¸ˇˇˇ1…1“h    Ë¸ˇˇˇ«        hå  Ë¸ˇˇˇÉƒ√h(   h    h`   Ë¸ˇˇˇÉƒÈb  É∆Ph   Vhl  Ë¸ˇˇˇÉƒã$Ö¿tË¸ˇˇˇâÿË¸ˇˇˇh(   Vh  Ë¸ˇˇˇÉƒÈb  É∆jÙh   Vh  Ë¸ˇˇˇÉƒÎªÉ∆Ph   Vh–   Ë¸ˇˇˇÉƒÎµÉ∆h(   Vhò   Ë¸ˇˇˇÉƒÈb  «Ü,&      É∆Ph   Vh®  Ë¸ˇˇˇÉƒÈaˇˇˇh(   h    Ë¸ˇˇˇY[Èb  h(   h    h‡  Ë¸ˇˇˇÉƒÈb  hd   hd   hp  Ë¸ˇˇˇÉƒÈÍ  hd   hL  Ë¸ˇˇˇ^_ÈÍ  QâL$ˇt$Phd   h¥  Ë¸ˇˇˇãkãT$ÉƒÖÌtDã}|ÖˇÖ3  É=    ÑÍ  hd   h$  Ë¸ˇˇˇXZÈÍ  hd   hú  Ë¸ˇˇˇXZÈÍ  É=    ÑÍ  hd   h¸  Ë¸ˇˇˇY[ÈÍ  ∏H   Ë¸ˇˇˇ∏@   Ë¸ˇˇˇÉ≈PhP   Uh–  Ë¸ˇˇˇÉƒÈp  Ë¸ˇˇˇhH  Ë¸ˇˇˇX1¿«    P   √           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   ∞              †  ¿      –                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P       †  ¿  –     ∞    ¿  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           §ˇ     ¿           §ˇ    ◊  è  s           GNU  ¿       ¿               GNU p¢7Úú∞J©ˆ=O"çÌ&˘ï        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 Òˇ       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 â            ò   U   ?     ß            æ           ’      #        =          O   Y       ®        4           B  8       	 T           Òˇd  †       y  ¿       é  –  %     ä          £    ¿     s   d       	 ƒ  ∞  R     ﬂ    ©    ˘  Ä   @    	   ƒ  0     +  H       	 6  @       	 A  P       	 L  ª   -     e  Ë        Ç          î  ¿   
    	 ™           Òˇ∫  ˇ   #     ”  "       È  ;         G  	       P       )  _  7     @           H          P             \           j             s                          ë             £             ±  U   ?     ¿             «             Õ             ‰             ˚             	                                      >             F             \             m             u             Ç             ç             ù             Ø             √             ﬂ             ˛                          9             A             U             c             p             á           ó     £    ≥                           ÿ  ¿  –     Ï             ˘              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q     ç     ´   B  ∞   U  ª     ¿   i  “   L  ‹     Ë     ˚       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  Ç  c  ä    è  S  î    °  N  ¡  N  —  N  ‡  K  Ï  Z    N          ô  I  †  _  ‹  R  ‚    Í      I  @  I  R    Z    x  R  Ü  e  í    ü    ±  N     V  ”  \  ‚  Y  ¯  a    N  #  E  C  V  m  X  ©  a  ∏  E  U  ]  o  d  Ä  P  è  a  ¶  a  µ  S  ¡  N  …    Ÿ  E  Ï  D  ˘          W  &  [  I  T  V    g    p  ^  {  E  å  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  á     å   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     É     à   Q  ï     õ     †   Q  ®     ª     ¡     ∆   Q  ”     ÿ     ›   Q  ‰     È     Ó     Û     ¯   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    Ä  Q  á    å    ë    ñ  Q  ù    £    ™    Ø    ¥    π  Q  ¿    ≈       M  œ    ‘  M  ›    „    Ë  Q         N          Q     `       Ä     Ñ     î     ò     †                                                    $              B     f     c          B     f  $                     Ã   O  Ù  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   ê                    	   @       ‰  à              )             –  î                  %   	   @       l"  à               8             d  Ù                 4   	   @       Ù"  H              K             X	                    G   	   @       <%  (               Z             Ä	                      V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       å%  P               Ç      2       }  +                 ë             ®  ñ                 û             @  (                  ö   	   @       ‹%  @               ™             h                    ¶   	   @       &                 µ             t                     ª             t  (                  Œ             ú  $                  ·             ¿  0                  Ò                           @       Ì   	   @       4&                                                       0          9                              9                                   <  †     A         	              ‹                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ELF                      p'      4     (                Ë¸ˇˇˇÖ¿t?Sâ√ãÄà  Ö¿tãÖ“t
«Ç$&      Ë¸ˇˇˇ«Éà      «Éå      [√ç¥&    √ç¥&    Ë¸ˇˇˇUWVâ∆Sâ˜ÉÏd°    âD$1¿É«ÑŒ   ãñ  Ö“Ñ˝   ãÜ,&  Ö¿Öç   ãF«$    1…∫ˇˇˇˇTj à   h    Ë¸ˇˇˇÉƒÖ¿Öt   °   π   ∫¿  ã,$Ë¸ˇˇˇâ√Ö¿ÑZ   â8π   ∫
   âhâÖà  «Öå      ãxã(çGË¸ˇˇˇçG$ˇu¯É«Dh   j PË¸ˇˇˇˇudˇu¯h@  jPWË¸ˇˇˇâÿÉƒ$Ë¸ˇˇˇÖ¿Ö   âû,&  ã$Ë¸ˇˇˇÖ¿Ö®   ˆ    u-ãD$d+    uÉƒ1¿[^_]√çv ˆ    t›È¸ˇˇˇË¸ˇˇˇÈ‰   fêfêfêfêË¸ˇˇˇã@«@    «@    1¿√ç¥&    Ë¸ˇˇˇ1¿√ç¥&    êË¸ˇˇˇVSãpç^âÿË¸ˇˇˇãvâ¬âÿË¸ˇˇˇâ[^√ç¥&    çt& Ë¸ˇˇˇUWVSâ√ÉÏã5   â$â ÖˆÖ2  ãhÖÌÑ¬   ã}|ÖˇÑ∑   ãw`¡ÓÑù   9ÚÇ  ãè4  Ö…Ñ7  â–1“˜ˆãSâD$ãGTâD$â–Ø∆»â—âD$ãD$–â¬ãD$9¬svØt$ã$ãD$âÒË¸ˇˇˇâËË¸ˇˇˇãCãt$âCãOTâ¬) 9»C¬sâsâÒâCãGL9∆Éû   ÉƒâË[^_]È¸ˇˇˇã-   ÖÌÖ  Éƒ[^_]√ç¥&    çv â¬âL$ãD$) â—ã$ØŒâL$Ë¸ˇˇˇãD$ãL$+L$ã$»ãL$Ø∆ â¡ãá4  Ë¸ˇˇˇÈTˇˇˇç¥&    ã   Ö€Ö   Éƒ[^_]√ç¥&    çv )¡âËâKË¸ˇˇˇâËÉƒ[^_]È¸ˇˇˇç∂    ã   Ö…ÑPˇˇˇÈá  ç¥&    ç∂    Ë¸ˇˇˇWVSãpãçÉ   çªT  Ë¸ˇˇˇ1“â¯Ë¸ˇˇˇÄ£®  Ôâ¯Ë¸ˇˇˇ«É(&      ã   Ë¸ˇˇˇ1¿[^_√ç¥&    ç¥&    Ë¸ˇˇˇUâ≈WVSÉÏlãu|d°    âD$hãEãâ$çC¯âD$â«çÉ   Ë¸ˇˇˇãÉ<&  ãìî  â|$dçHâD$\çD$âã<&  âT$`Ë¸ˇˇˇÖ¿Ö"  â¬ãÉ®  Ñ¿xçÉT  âD$∫´®  s6ã$âT$ã    Ë¸ˇˇˇãT$ãD$hd+    ÖÚ   Éƒlâ–[^_]√çt& â–çæ¯   π   ∫   Û´â«Ü¯    «Ü¸      «Ü       «Ü  Ä   «Ü  Äª  «Ü  Äª  «Ü     «Ü     «Ü  ® @«Ü!  1  «Ü(  b   Ë¸ˇˇˇã<$ãL$âoâé   «É(&      Äã®  ãD$Ë¸ˇˇˇâ$ã   Ë¸ˇˇˇã$Èˇˇˇã$ã    Ë¸ˇˇˇ∫ˇˇˇÈ˝˛ˇˇË¸ˇˇˇç¥&    Ë¸ˇˇˇU1…∫   Wâ«VSÉÏã/d°    âD$1¿ãGTjj Ë¸ˇˇˇâ∆ÉƒÖ¿Ö‘  «G    ã$πÄ   ∫   Ë¸ˇˇˇ1…∫   j j ãD$Ë¸ˇˇˇã\$∫P   «C    É√Xâª`  ã}dâ¯Ë¸ˇˇˇâ¡XZÉ˘ˇÑ   É˘PtÉ¡É˘Pá¿  â˙âÿË¸ˇˇˇãD$d+    u
Éƒâ[^_]√Ë¸ˇˇˇSã@@Ö¿uh    h    Ë¸ˇˇˇXZÎ6ãê$&  Ö“uÉ¿h    Ph0   Ë¸ˇˇˇÉƒÎããBË¸ˇˇˇ«É$&      1¿[√hh  Ë¸ˇˇˇ∫    ∏   Ë¸ˇˇˇ1…1“h    Ë¸ˇˇˇ«        hå  Ë¸ˇˇˇÉƒ√h(   h    h`   Ë¸ˇˇˇÉƒÈb  É∆Ph   Vhl  Ë¸ˇˇˇÉƒã$Ö¿tË¸ˇˇˇâÿË¸ˇˇˇh(   Vh  Ë¸ˇˇˇÉƒÈb  É∆jÙh   Vh  Ë¸ˇˇˇÉƒÎªÉ∆Ph   Vh–   Ë¸ˇˇˇÉƒÎµÉ∆h(   Vhò   Ë¸ˇˇˇÉƒÈb  «Ü,&      É∆Ph   Vh®  Ë¸ˇˇˇÉƒÈaˇˇˇh(   h    Ë¸ˇˇˇY[Èb  h(   h    h‡  Ë¸ˇˇˇÉƒÈb  hd   hd   hp  Ë¸ˇˇˇÉƒÈÍ  hd   hL  Ë¸ˇˇˇ^_ÈÍ  QâL$ˇt$Phd   h¥  Ë¸ˇˇˇãkãT$ÉƒÖÌtDã}|ÖˇÖ3  É=    ÑÍ  hd   h$  Ë¸ˇˇˇXZÈÍ  hd   hú  Ë¸ˇˇˇXZÈÍ  É=    ÑÍ  hd   h¸  Ë¸ˇˇˇY[ÈÍ  ∏H   Ë¸ˇˇˇ∏@   Ë¸ˇˇˇÉ≈PhP   Uh–  Ë¸ˇˇˇÉƒÈp  Ë¸ˇˇˇhH  Ë¸ˇˇˇX1¿«    P   √           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   ∞              †  ¿      –                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P       †  ¿  –     ∞    ¿  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           §ˇ     ¿           §ˇ    ◊  è  s           GNU  ¿       ¿               GNU p¢7Úú∞J©ˆ=O"çÌ&˘ï        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 Òˇ       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 â            ò   U   ?     ß            æ           ’      #        =          O   Y       ®        4           B  8       	 T           Òˇd  †       y  ¿       é  –  %     ä          £    ¿     s   d       	 ƒ  ∞  R     ﬂ    ©    ˘  Ä   @    	   ƒ  0     +  H       	 6  @       	 A  P       	 L  ª   -     e  Ë        Ç          î  ¿   
    	 ™           Òˇ∫  ˇ   #     ”  "       È  ;         G  	       P       )  _  7     @           H          P             \           j             s                          ë             £             ±  U   ?     ¿             «             Õ             ‰             ˚             	                                      >             F             \             m             u             Ç             ç             ù             Ø             √             ﬂ             ˛                          9             A             U             c             p             á           ó     £    ≥                           ÿ  ¿  –     Ï             ˘              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q     ç     ´   B  ∞   U  ª     ¿   i  “   L  ‹     Ë     ˚       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  Ç  c  ä    è  S  î    °  N  ¡  N  —  N  ‡  K  Ï  Z    N          ô  I  †  _  ‹  R  ‚    Í      I  @  I  R    Z    x  R  Ü  e  í    ü    ±  N     V  ”  \  ‚  Y  ¯  a    N  #  E  C  V  m  X  ©  a  ∏  E  U  ]  o  d  Ä  P  è  a  ¶  a  µ  S  ¡  N  …    Ÿ  E  Ï  D  ˘          W  &  [  I  T  V    g    p  ^  {  E  å  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  á     å   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     É     à   Q  ï     õ     †   Q  ®     ª     ¡     ∆   Q  ”     ÿ     ›   Q  ‰     È     Ó     Û     ¯   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    Ä  Q  á    å    ë    ñ  Q  ù    £    ™    Ø    ¥    π  Q  ¿    ≈       M  œ    ‘  M  ›    „    Ë  Q         N          Q     `       Ä     Ñ     î     ò     †                                                    $              B     f     c          B     f  $                     Ã   O  Ù  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   ê                    	   @       ‰  à              )             –  î                  %   	   @       l"  à               8             d  Ù                 4   	   @       Ù"  H              K             X	                    G   	   @       <%  (               Z             Ä	                      V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       å%  P               Ç      2       }  +                 ë             ®  ñ                 û             @  (                  ö   	   @       ‹%  @               ™             h                    ¶   	   @       &                 µ             t                     ª             t  (                  Œ             ú  $                  ·             ¿  0                  Ò                           @       Ì   	   @       4&                                                       0          9                              9                                   <  †     A         	              ‹                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 cmd_drivers/media/pci/cx18/cx18-alsa.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/media/pci/cx18/cx18-alsa.ko drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    cmd_drivers/media/pci/cx18/cx18-alsa.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/media/pci/cx18/cx18-alsa.ko drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 