ldcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
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
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/err.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/sysfs.h \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
    $(wildcard include/config/SYSFS) \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/mutex.h \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
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
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/kstrtox.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/kern_levels.h \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/ratelimit_types.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
    $(wildcard include/config/PREEMPT_COUNT) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
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
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  include/linux/preempt.h \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPTION) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  include/linux/time64.h \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/vdso/math64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/COMPAT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
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
  include/uapi/video/edid.h \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
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
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/TRACEPOINTS) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
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
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  include/linux/osq_lock.h \
  include/linux/debug_locks.h \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/gfp.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/spinlock.h \
  include/linux/bottom_half.h \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
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
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/linux/seqlock.h \
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
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/kobject_ns.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/fwnode.h \
  include/linux/resource_ext.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
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
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
  include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/rhashtable-types.h \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
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
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
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
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
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
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  include/linux/ctype.h \
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
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
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
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
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
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
    $(wildcard include/config/CGROUP_WRITEBACK) \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/list_lru.h \
  include/linux/shrinker.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
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
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
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
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
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
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/linux/of_graph.h \
  include/linux/pm_runtime.h \
  include/linux/regmap.h \
    $(wildcard include/config/REGMAP) \
  include/linux/iopoll.h \
  include/media/v4l2-async.h \
  include/media/v4l2-device.h \
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
  include/media/v4l2-subdev.h \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
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
  include/media/v4l2-event.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \
  include/media/v4l2-fwnode.h \
  include/media/v4l2-mc.h \
  include/media/v4l2-rect.h \
  drivers/media/i2c/tvp5150_reg.h \

drivers/media/i2c/tvp5150.o: $(deps_drivers/media/i2c/tvp5150.o)

$(deps_drivers/media/i2c/tvp5150.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        EMMAP_DEFAULT_ON) \
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
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
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
  include/linux/overflow.h \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
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
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
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
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/cmd_kernel/irq/cpuhotplug.o := gcc -Wp,-MMD,kernel/irq/.cpuhotplug.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned    -DKBUILD_MODFILE='"kernel/irq/cpuhotplug"' -DKBUILD_BASENAME='"cpuhotplug"' -DKBUILD_MODNAME='"cpuhotplug"' -D__KBUILD_MODNAME=kmod_cpuhotplug -c -o kernel/irq/cpuhotplug.o kernel/irq/cpuhotplug.c 

source_kernel/irq/cpuhotplug.o := kernel/irq/cpuhotplug.c

deps_kernel/irq/cpuhotplug.o := \
    $(wildcard include/config/GENERIC_IRQ_EFFECTIVE_AFF_MASK) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/RETPOLINE) \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/interrupt.h \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/PROC_FS) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  include/linux/compiler_types.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
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
  arch/x86/inclu%(*qìSÜ˜eáÞ]îZ¬}‹³G»˜¤cdq2°­‘ÆÙ¿
+“þ»¿þ”„íÓ
¥òÝ•‘™ž!÷¦š­¥c¢B/ë×"B¤Ú¸:®»4…ª%	þH££³ìâDèì¼(¡¾@÷ÍYÐ‚¹èJ/ÛVéz-ÞmNâÝ…
äxJ93H¾N ©AœÇé·Kµ21ÀÊ²qzÔ´#ÉµØ:rÍ±ÞW£\¶Lá$v[M˜Âj	›ÉÊ¤¾@[\ñnvÎ§cnvj”o”¢KQéIÇ©K`2ÂŽ˜§È!¸ð
Ñ_î ³2¦·6#(û&æa«5—!Žº;;âì¡j¡ÆÍà>OÏ±3…Ãëô¢oZ’&rdj!²dD÷4vN–UŽÅºÖn!¹	ì2eÎWªö€åã?§dK2¡ÙX^#_bº¶£Œ€¦—Y"aÔì¤ÜµFG[û8`õê°AaEÙOåôÙÄ"}YJñÅíã=Ç´ð×´Q/Èó¹Y¬ëFJéHÛZåêC ñsÏ[}&ÜIø¤ßáaâ×ƒWèm'Œ±¨œÙ“‘ÏQ”ƒ-£–§C#% RÓÑ,õä9:Ù@x§˜3Qj;Þ3ÎYcŽD	3ø/tkèºUuÝÛã„zžzcœê'6<\•d´…rS¶b±l_ME\ËÚÖ*te7 êÕ+ûlÅcåÇÁ0ê CE¦Ø€6±!uµÉõR®¿eäp¥Ü¬-
WÀQÞ{xßÍ‹]¸¯U¤ÌÖºŽLµ”âÙÝ¯¯_ªAˆÔ%?ø®jŠÒòÃëØÛª£ëR
Ó¯xØ-Êé–… ÛM¹ïý,­½%3rÂÛ€Eï\ŽN«¥„ú[ç 3šLaò¥˜/	¾@…ªâµJ”|”¼xË°_ zIÐ10)B®í½‡/ÙñÒL:A¼à{uç:þ›<g§' ëúMŸ/êÿÎuSÈqHOe€ÜRJF`µT×)?ä€›y Éši°—(loýû¢k“óq¦Š)ô½‡£f[óêÙ6Ú2…Ï )8ý ˆH[©î3™$à´lùS­ËzŸ[%a`$¤4êœkðíúS]vO;“ÓxZ5x™¡jItmþÕÙúÄM
âdM÷<ÕTf
ÍÒ¦,ä5¿¢Íä¢#È+ª•ÝÐ•¨×™VëË ¨‘Ùöá0¹>õ»Õ÷ÕÏì	Ä=¬IýúQeÈÍ&#÷}+‹œ~ÄOàWwØ˜yŽ]’¿‘ÿ– deHšžŽQl™žÙÕ9ÚZwA¹Ù õÔ|C¬Ñ{LËÍùQÔéx¶¸Â¶)Çh	kd·1fo¨;ðƒþµfâÙÜT×fÆúp¥µ)pqmCMïý‡;ßËÂÉH9®Û´&Pr¢Âì¿$æRÉÓý„äàû…¬?ÒÙ©Ë¼‡Éž7pÑŠã@K¼«gapgUšY´lW³Ò}t @òŸ VŠµIûÝ-m#ÐQ$.6=Ü‹=ÙÈçÓŠ8ÈPâ{I{a£uÓY6‰Þ£lŒº…©ù™(+ãWýˆ»[gÅºow‹æ­‚3î€kŽ™‰Ã‘
ðÔr„ ùsì“g¦?ÐB!¼™±…2´)Ñh¦ÄÙ‚pÜqŠÚ™“€8}nëh—:Õ£Þõ.ªè‘ûJœ$.§°y›Z#ò#fˆõ_šÞßœÌÃËLÀ÷€{vNE*:m·ºÞ%A6ö•®P´ÒâÛêü½Û4x\d¿ÎX8¯°IiŸÜT&g-FÔâ‚ýç8‡Ú£' Æ+,÷Ô…—cý7~´áZiêÃúÌg«…á³¯4’Cþ¾×a˜<à¸S²ÅÞüh6(
*ÿ¢k3›â
€zàc Q•tìáX`iýÔêQ4FÌŒàº&áþ]GçÓ,CRðZM4ÙÇxší!!'b:ƒê
ÆŒæïÍÌŒíÿ¿n~Ú¨KÑ]Ûl·U1Û`Ø÷¬Ýá#’Ù®&ºnPèÌ­²4K™ùù§8KkIE_ûÛd÷béª”«@#«$þÕÂyþ|U\HPèm-ÆÄ1Üö«—apŽ‹;)X=Ýó‚¯‘÷žúÿ¿eBêó¨®üÌd%Å5ü`A=ÿü‘õ#i3|ÇšA’GÏ~"€qqÐŽ{}Ú”H¼£ëOÆ¦¹ØGíë<¸~¡øîðÀe( á4¸Òô¥£)$ú)ïº¨Eæ!ï4¥›“´z*9Ê¢Uíi©jžäJ?î&˜¦1nÅ‡Yt6-¦t,™qz•˜©<bïöB±¥98Fƒ×áµ¦:íãz©°Q…‘’ºý”
>—>àÃÚÍGëÜhòSìæ%wg‘è™¨làÛë\‚cá&'˜0åa–»!_.bˆ‰B 7ÛAðåì_Ñ#YÝ°ùwfÍ<ý	•s)!€;‹Pµ×*éË‰3ÌÑ¹Úvïf¤Q¡E¢îžd^¦fN`W)NÊ´@°¹šµVBÖ(®c¥È¡^/ÍiÙó>>¥`B—:N?É! 51JC©alºð¨ŠÃ?Â,m+Ïå—A™‚±£`ÿ-s¿%ým»Å4æéI	J³X"\FúH™-sV´w}žqê
p¥j¹Ù¥Ê£Î‚Ö£åË11ÏÁ‘SËË†¯K¨áòEùÌ2¦ˆÍ­JhXNêgÝüÆ{Œª­YÉ¦˜&Yµš[[ #ÖÖÕT«S«xz5D\3B;ÊÂ:¹H:6ß¸ÓÅ…foÞ“éav¥³dµÖËnµÀÙ»Ñ™!ø‰õVB¤òë^<°Äíf3vy&PÕ5·ì$)š)Š2Ž;?w,›rËÍœ|V ~ Ð€F¿<¤¤&©µKFÄoPìÞ±‹[ø‘þ¶ÜôMƒÓØÁ.T
‰þTHŽ¨æŒØ…º AtbÙ´}ËÅåRúoRŒÿïÖ‘ÐÁVð· F, ÒÄFÏ™)xÑÇ2!çŒÚ\¡X‘ôf*NðììaaXà/{ë~=l…DÉþÊp"}<l¶â¶ßfIì†’ívœèË¼ºÎE˜ÿ†«'LhÙó œÃâVèÄ¤ê´äYÕW$,wípºJÇ™¤qGTm¸ÙiÚ¯xGk½9µý³ÁZ¦]ÁJSxëÈZDc Ì˜p'z¢BT`Ctî´f°ñ™œÍë¼M˜–¦ÆhÎ±bïQeNO‚„H²YØA¹oWAàºô.¼ÜGöT>W¬”àO¨\.ú‘?o÷»cm*›òàÈ¾²ì3÷ŽÛ¯f´^”OE	BÅSã¬×¥öÙ%Môã~¹á˜é9Á5ü~?Siÿ`9WqGæÕ<†íñHE_$Ñ2ˆ2´®&¤í¦	‰Ïl:àÁ$òc9³›ÕõÏN$É;N?´™€…æ•Žé„$õªá(³¥Uè_’íÞ ÔñçØõ”#|’PR<°™íÙÝ¿ÌŒ‰øÂ»SÃ!u³„÷ëlöZ´5®:&+J9Ç3‘œ ŽŠ 0/Š'¼[?äJ!(œ>dÞøíD8sãN¦ê­èÕýwuð‰J_³ñ…‚ÑR‰¶£^ž?zm~ÚÑà¿¤*RLñuFàˆ“ùô^ŒÅœ0ãó[ìÝ“M³5'ôÚÿÛë	9Ž¤ÑŽcÎ	ukA\Û÷}Ï1Þ½ËþžÙ©yÊ´Rv:[8ÜT‘þ7‘S#eI‰…
µ½?E˜î„…>1ëìr¨ÁXêû_NQ¹¢ebŠü^?8vßÕÔ"Œ?YìéÞ•h-dHö0»ø7Ô]·ê-£Í<"è}©¤4À†5®°J;PÝ?KW›ˆ¢hhú7¥3•X­&­ZRog_ó=»CúGùA&|’<7£›áÆÖ@-Q±ãÅ¬)kÑ±Øu,%à-Â¨þÎ˜!ŸÇ,’2 ˜,c„Bvº¤‚,:Ë¶‰¢Ð\(eQ>Œ Zq·~`¯Õ;±)³/ìcÃ|?t^¸dñÎ¬wDû/­M-N; RFmÔxzaÊ8æ‚4zrÎô,O7F/­Äu‘õãSO¢N¹`wè“ Rœ—T(Ã`™eÆja/Ò£û	Š,/P· „Ã%´—õ,Ðú¨GW€¶R·$%QÐw| Âò`±6ô>`ï8ƒr€èÀ@¹øínŸä
%Ù õ„½uWçA´Sð½>Å[¼y‡…YJ¾^Á -–…Úý
òþp½½¸¹Ð:9Ê¿sÊ³ˆBžFRØG¡úl5éVdó«’’†JÚài5T•˜ŒÔà‹ðƒ†÷z.t@ÊÚÜòcÚ·ëÿ;ÈPÈ¶¢9¿ÒÕµ’(~Ú©[û;v0ˆ>qþÜŽNàÝÈ>Q4‡ó»wjDÏÅõUÕºjØÝ»tÌVq.&;´^Ý[,#äB­V~þ&eú$Œ…œˆà(~¡D]—™Ö»Ä2¶‰‹\Ï­ÙØÑ—L/ýZ»x¿ú©Xjz64#_3ûYJ¿¨£VšýKtïi29F©ˆÿ·C³á‹:š}ÀIÎžóFþEoZX^)u	š,ÌHÀÈ­ÝQÓ|ÊŸ—Ó8ÜGYoC1A¿g.7ŒÆé-ü_|„e’–~GîIãî–UÕç,½=€ûº ãõÌ<† _PÆsÌŽþ¢é?—R¼2O¨ß¥]÷ì„àX‘šÇ³#èøiW˜P£ ²Ò— ÍÍîŽQF¸Ä›@T;‡¿‰Àö>Ü$µyÄ®ùq°ÛÑ˜&Š(|m÷—€¥É 
2œ9AbçJò¯âÝµë'û(0Õ£}L¯YÃÉAØ	Ie#\&ÒKêÐÁô{ÊZ)œÙ[ué"ëç%tšjç#§ŒlãW˜•7}»®Ž´æl=¤Kíˆ¾	gâ¸N›¾ŸûãŽ“Ã¨o 8Óçœgé=:¬­%û±„õ/÷ç÷LØ¶NBŽnõQNúÃBWOè3H‡˜ë4ÂwÉÄÒÙûj.û't)å£WtVºžÙ]ociê'E
sèS¸æí¯	S_óbýã[˜~9M½ûälÏû3dVßÑ®ð5$0¶9_èQ â=hOË‡…`ûôÉ^Ö±Ü„Ãû<œ¶0³4_s½%ºÐPÍ˜3ÈL!m{Ò8:±Ø'ÛµiÐHï2d	¶HK¦N§úÓ‘-²ñU‡»EKó"ÊÛ4ò¶F_Û<¥ÞiY§ËýÓùc3H_sšÿ¯k¥Ž’ÇâmWTŽ±å€FwüÂìÝ«aàCZcN;l)Øãøá¾„rÑÄ´Ç¼cÅôÎö´¼/[¤f¾
Í*®	zõ±}û9Ç$Æ÷+­”Y0‡nÍ¹WÌ¼Í'{‘-Ýª ÷’äÝ`Båæ;rÐ•Ö·Ä2G“•±²wÖÓ±î/a#ÆSM2»üˆÂêùQ*Bžèì[
¥ñ+òï.÷Êh 4ö"s¤"3%ÅË8Û¥Õ ¾f¾Sžéù@A8I6 IOdÚg’¼g­ÈvŸ.1q¡Óî(Àëh‚ò"WÕ½$™ŒEtwÈVWú´“#Eä¹µø7¶HÎ†Ri2Š¡Êˆì'9/êŒÎ‰ÓgC;ö§;¬î,º§ò…Š48cdA	bž)BSrØÒãÞŸ¤d,ä@rª›yõšòJÀE+rãûpò§‰>9€GmÃVíE8o™+O¨Úß\ŸØ"ŸÖoh´åËªÕ‹Ëa9Ïá{TZ"G‡~Ý|V}A"7$y&×HÈ>"ÉHqXïÅUûSb•Z«¾é¬­‡¸AÔl¨3m>üivï{Rµù9{A(=£$^O`¼+s>ÓïÏá¾ákÞAÔ¨$[}‘Ìï)gc˜?þéf“Ë4·:êú]²Ï	ºç¢ÂßcOÀ ˆBV»¶ZåeˆÉÌ„/ZŸç–h[ÁÈµèCE4}Èu ¬}Ob$¨þÄövÕ‚0ü±—tæOÉ‡ßž¦ÌÌC6ä0YþBb¥	ð±^pšÑ{ºÁgE`á»†õuKE.Å»ª£¥ÒVê}$²´ÎæY©ûÙ+îsÉ‡£©¦—S¿•øè‚Æ3˜fD›,+–¸øD‡u×²oŠ"ö¤š^¶ap^'mS]ˆZpn¬P?ÅÝyÐÑ“ ¥|b`÷ãìúL&bœ^]Ì}dÜÅ5úDÙ«ä£Êê$c³ó+ùífü%¬™"# ‚=Syrív÷.ßRâµ©ŠJŽK„xšâ2ØÀÏJÑpŽOwªËÀ‹sAQÂÏ›âˆE•¹à‹åÄµÓ²Atå;ë©ñf€­ …ÑÃq	îªÝ¯éq/öŸÍ­Æ¤¥û.µVK6Á]¡Q&ÄÅƒáÓ!Ølý‹= É’
âuFëTr£zæwô§^hÚº¢3´™“Š~ú­àÕƒ_OÇÙƒLfÿêuS1ÉKO¼ÎƒÐgø„…™ZDÂ!pNXX…œ•oL±¯%XøºsÝ<—;7žŸCaœÃE“0´>q5÷û]»yM°'ÇÏc³õúò_‡µ@ÃC¾,vöž,Ã|Åß×$¡§ßPæÆÛªSw[—ÖÇä°
BÆÝÊªeñáeuæ£ _ËÞ|:Ô¾\7*2û„Hœ²^â±¿¹'-¹~~NU:‡U¶*”HæïÎ0uð‡§IÒFö^[–îàúëªrÿ
÷TÖ4Îñ¨ñCÇí‡/úDWXxaëÍuÆ[îÕòÓjñ5C‡L?L†²ém!ZûŒ;b›N…mÙ|7],7ã€Û•ˆC…QÒf.Ye^…·	' Uc»ÕG*ÈÉI^.;Z+¾–qœíÊ¨©£pÀ„\I„\·	zCY=“–?›@#b¨ßìàŽ`0=BÄGJÆsÕvx”c-0 ÷ôÜ´­Îãià^ Î<“}‹sU»ÕîÖÊ¡yÈ(â´cóÄmã‚„ç&
Ÿ_M*<s’Ÿ?ÇÂyaz¶ôìË´,eg²€¶Ü3TØcÏ£u{b(ÇWoŽ˜N‚ÝZ}…%6Ó½ŽMöÏÀD× îÞ½œQ¿|9YrË)Ä
¬r±	rD’ª«u´MXéw·ÿH5Õú¸°½ó Ô*‰þ¶¤]R&¸:Xp! z6«nV¬$^`›hxŽ†!ÃogrGöGlÂñ«“xScÃ¸ìTËpÊš¥€ý¡ÛöÊ#Hlä¡õb÷Ü$ßôî6Ìì¸¦Ì3û‚?2ƒWUý.òºýc*Ij“mbæYÞÒq÷‚W D3—ð´îï×Á„Ú ÙÖ]Ù½NÍt´Ý/-ÂQ>¨1ý+…
jèÐšê—¶	Á§('o)Ô›‰‡ðÓè&Ôòdƒ	jHÃœNJÜY0¿z–++šãJkŸ›=Xçé¤6ÏT
‡Òê‡÷ E †Gb©, ‘1Ež?Öº¸.wa{¯_~æ1Êî7¸ìàLªdP2`ùÌ•R&qÞŸ=k0¼j"KÇ.‘‚Zý±4pHd‚Û[;d$µaÑm,ßÖ¾LÚ¤&¼‚û‘'yÒïŠMWÈÊÚý4{ä³@ÆSçTe{è¶ÞrbÐ&s·Æé˜qÆÖ}9	pƒä==‰*O!lb!9”ß žiéõE£ŽŽ²Ù¶Èž&'âµÄÂìe#[Ã¾—y– Då-¹S¨	9L;ëKw#‰ÖLÉ
(ÿdL2lvíT*„ÎðsëÐðßÞ¬—=]@_™Zž
¿©TÛM¿}ÌÿªÖB’t×7c°_“/Ulí=†Ø‘#CÅ±c_2”É¥G6V¯@©+}ÎÕ>ÕVãe0îdáÖ„‘ùG½ÐxÑ-÷ª©IÎî7¼e	¦!7ÊíwxÙfxèšcü‚Xm¡¿J"¸âü+±R˜uw2©!fšž5!Îe½ÆãúEâ$2/ä†~ŠÕŒ¢Ò–…°_”Àê%mVeŽy·†“{ù¾-TìÎ4TÁ]—Œx<×žU mödXçOÉB‘"UhÝYeŸýcŠ~~„Ì'/|Å»9Þì?o}JàYÄEdø‰¸J$gÁþê\
¿ .ÞÕ±±ìI•·¼ÕEØòR#š£	Rf–r@`:ÛÓÆøˆNt‹sÁåSI‡$>`Û0MŠòVü}¬:kK€9F×^Ó&LÄË£u;ìO)T©ðíkM®yÅÇÔ¬µánÊX'Á0vîÈ%'qô§ÁÒÔåêò¢þ£Ñ_)´ÜÚú~KÒt“Ö¨ýƒ¾>-n×ýe—™¹ª<!YKˆw#;‚ºzƒï‹øìY0öë‡DÕ†
åõÊ,øí³ßò79B¦À	Ù˜±À-ûø7J÷WËçûŠXƒ¸äÈÔ‰a±óÏ»ùUüÁÈRd&,·‚¿#´°›Ä!@gÿ)à?eÖ¾–0xR ÅŠ©ƒÊú—V£]›êÆ£©¯9:êRÀXù™Ój€]åÍ4à}|åóXPŸ^¡}O{ü[3¿e±ãò*ƒ3=öãÉtã*$ñh-ÏÍI×ç¨ânÄ Sí¬ý”èjTý7änoPÅK¨÷T¤ôkÚ#j0¹$ÓÙb(õÍ°3¹VÂ„ªf€ï™ð>1ÕXH5­p;;èX%a N§“ä9
ëöð‡tÊ`È^µ`·çŸÞþ_IÜÑŸ< ÿ0TÎN]`¿@DjŒ'ÅÐ¸ðÂ…ÀK*©®/’‡(ï;î¸ˆ®÷I3µ- Éã¿UùR>£ÖÂ’Ô¤U,nh1öm_•²ž’-$ß‘þr´u%•Ú\~jFïç§è€£w4ø."êšL¢ÁWx h#®)ä•|úÝ¬kL‰à÷”¦
Ûàzy …ïçtN—}!qZŸ?`€ª­ÔþðCs->ÉØÚ€ÎŽd"4©†¼ûl©›ÝeúÏÈ´7æ­Ø‰Yçû^7¸…ŒZk„ -ò	-Q†lyäRÖhï•¦Õíd”È¼RÚ@vòÕœï‰u–D…júJ ÷ÂU˜¸??$Ò{‡èœ«ï,^tãõ‡ò¹ôkÇ)I³+ ï4„<séy¯à}AhL[¾VÀ°³çª(âÓ@2.	?ÅÎÞ²‰@#ÀèÃ_ÂG¶Bñ´Ùˆ |4Ã"o+†G¨˜Çxh×Bè)<”bQÂ{Æp£áÇdü­·)Äb¶ËÀàƒÉµ‹;ƒº¸Ÿ9OÛ}n¢óOž¨º:šõßÈµu‘xcè—~«×•Ì‡Îÿëm ×³{fd½ÈÏ÷0(¯/(V%ÏôÍEÚüN½àâ<ÌN¬þ!£ðùÖ¤‰Ïg i8÷Pä‡VˆžøÒŒ“~ÝX±]5AGá°ìâ¼ÚËˆñê!KŽMº·GD£ òâ–‘bsÅ‘o¸dfx¤Ék" "þ4kØpFƒ¿ãÇ"àÿÉÓ²H‰÷¬2õµzSYšÙ¨i¾^i‰‚J/ä²ßJ˜j‚VZP#ÒðuÆgÆyY?º“ý–ÑE8>&…ÆAÕjÑ‹Ý!ÙÐN\Ùý†´M1ð¬ÃrÕi¥Ðç5Ï|Î(ãÉ¤œ™ÆAæ\O(ñ„ä²WË¿LµR^:øù6Î¬ŠâŠxfÀíXf)åÁÚ±JÉ}Î”F)8«8ðôSÊVÃlöeÿkYKÆ€4¥¨ë–²Ñj*S«8rt„à„*}¢…8fNÿ"02QµKŠ—mý†ÌLÿQçQ×&2•ËÇ+S—Eº£‘Uz~©¼^Å=Fj¹ÞxœìYÝV/Üò-Fâ÷oºÆx(¼ÏÍÌ`˜„Þà± Ÿìà›{:‹Ó“â |};þ£ò—‘	ö‹•vô*Ï¬®
Ã^°kØ!ÏâÒ÷çÛ$ÿÕ7òk¿‹Š{HoK”Vú™(v¢:±Ò.ëëäÖé—ôÀ÷ý¯«8ßV#^ƒ?3‡âñoLKX_±ëS÷
>Sþr?ÐÚ}ÚÃpÃÀ(‰ªœû‚Æ"sÝ¬OÜÿ6…§û†ðWáNú[Z†öøMê§‹o(Ÿà¿ì¯Cúâ¬Å–Vž ]÷Aç&ÇCšT’˜°r4fò*ÒÑøZž+Ý:e~œ:[&òÅ,}¡ÝPÔEl&×øMT 9kDú}¯(|Oì5wÕsü´ˆ³_Ý”•ÄyÞ¿åX¤ªi[L¿Š| ûï8(Ó$,lð‹/e4N•/—ƒàÀàVŸ¾®êTÉpøèHÐxÚ’XkHï YÏ%=ôçÑßÅ®d¨•4ÇöšÍÊÚëê"ò:¸Q¾a¡Iº€Ö‹˜^yÂë€Êà©A»þI§3îd6ì@ÔÚÈ'äÓ¤À· ó£Ü|—ö·s^9•g:§R:ÄK§S)§®ºþZþß«wO~&„ß¾Yd.Df–5®7‹—p¡”¤sEÏ¢ICq‹¶È[•CöP¾J§†WÀ^ÛUOêÆ<\;C8@?”AÑ´¢˜CuGÆ6Ñ‹ñoà˜¼)ñ£vù,ÑÆLyœÄÔ ýl®;Ä-?ÞÛõÉFxeŒµ¾)O¨+ 
XËtAÎ0*¿—{J³™oQÇOàÍ(MÃÏaË%aMZ™üô…§Ij­'L‡3Xu{äìù#o#(·”N§Ëˆz±Ü›“…Z”ßs{g¦‚–gÆëVå‹0ñöŠæ·5|³TFf¯(Dä÷‰ñë…¹ßdRZëV¤è ¢ZØ‡úÈç(±8VÀN·Û¨×43§‡>ð  
êª´²`!B–¡3É÷Ê*†\e&kIï/9“7[îÑ_„<ªG¤\aàMn5Cc:Úf°û÷rÖº;à%Pœ˜Âüs)±ÚQ8{á©ß§¾¾òœ}ççb10^%mY«cB¦”‰Œ>Eð¡dt£C>–wP)ýžÊ'»J¸Ežî½,cç>Ï]âÄ±C=@ß+ëre­2„N6HˆÓ’»6“ÃŽ^éA†iu1<·Þ“Ü‰eÁÝí÷Ä)>=ˆîã>nZÈÕb,~Ï=µ±‘ÔýÊþð€¤ü C¼÷ÛÁ>Œ¶ª?nÆËWÇýö@^eD7÷ƒÊØ€©Ýôä2Ð:ÿ¦»vN"¶WÒèÿN1˜½¨©¶ò†#ÑG˜´) m–rP*s‰X¤i­:Fÿ< –qj¨9reJàiäÕ£3êÖ>”ZKÛü¡Qœ”1ÜÏBfÞ8‡Ê2HòÄõ³®µ§|¡ÚœÖ—.ÿ˜_£ÚAé¯ 
]ï±!±´ÑqÁŽÿc‡i·îynNþ&’’>„0¿bûŽ³ž¶èçîE4l‚mì-°oê˜j‡A4ìy:¼Þ_å(*(A/ß.-ã,tÀkIE}Ag0`‘Òß
Ëä ¸4èÖÆÁvÖ~>Ë.f<UÈ
.°2èâ«Îþ^¹›.l¹¹²mœg¬Ž¯)óÊu›¸mYx‚á¾íåSdf©ŸÚÝ÷êöà‚YæN‰ËC€‡’i §»s\™‰tJ¦d\¶Ou¿,HÛS}Ú*¸ù¦s¾õU,nEC˜©ìr€S¤"–"Ïâ¤L<uVÄ›¬VÌ&\W“äñ[#Èô=¤`>¼$>M„3)ª­iÜ~2åäf!Ékâ ó³¶‡?­Ÿ]ë¹+«clSú·ýr°öZB-=—c=ç{K–Úƒ’“9ë`¨Pdû‡ÖÂx@•gñ½!ßÿ´íX¼&Ïæ¦Y1ëx¤U†^a>Ÿhpæ¤Ñ÷¿xÖÖ¨Œã¡£~Dkz¾˜‡ i­€MèGÜú¸JÎƒušÒ]’}¦ÉnüßùÈƒèýñ<xxˆ.,À'¸Q¿œœ:"Ö[ÌycþÜPL4k_XAû5RÙƒµà˜y‡kËô~Ó°Ÿ;I3%à‘¿äëâ:GÍX2€¼Ýý É™[a& åÔR”tÑììC¼ßM›©qµV?.WHòó"¯ef²á¡¹î)ˆ¶èE(\ÉT|Î(„
ò¸©Þp× 27§¬dKïO{zJ™\#™u6ç§æiÝGwu§>OGØ¾ùæ¨ÝÒŽøäØY•JÍ(õ÷ ÙÝT‡Å¢Ö[û#ªtU+e%~ÛV
.Q‡mã3;NwÚëßì²£çÐMõ(¿˜4Ñ]³''8Žqþº=å»“¸=G ò¹^5@Õ÷ß?”féaŠïröîÛ–=€þÏéE¬’9Â°7I®¬‰*äc/q1×¿x+!TÔK£ÅiÊ0N´(Õ4º=»ã<x°Ì/¼NÂWØ>Ç­G¾Îˆ¼	É„cÒ=åÈôºŠPÉR‚&gþà+[\´~D"¤…VviS’¤‹@ìäÒÈ°ûô\XÃÍ fkfY£{³Üï…Ïz<sC€õDÐm"ÿxÝéM
^P´¼\‹T´‡"ÌM>$~eF ƒbazOÎŠŸÉbµÌ "OÂÇ@°N6^²+_ý:3Ï\CKÝ\B÷ÖV¬-å(2Òzî|„‰•üÝs¸¸ÅË‰k¦s¬m²c¬Ü·Œ\àÑ.· Çò¯Æ]	ÚéÉSYš u+­H÷ú0$|±å;¯°öíªYÙŠ^dÕõãN–Q°þ6nyÂßMè$õ[Üœjz½lö™°@»¡ºêÇ¬—”â¿2Ä`gˆô‡	@af¶$ ICztm1=Nß„…U_%Î®–ã)Ó­­Û=`¬ÙïÑ`Òî™1sú7xý«wt[$vl÷c'‘m?gð„,€Š6÷¶òÃs¼À·)Ã€jÖÚ
'4éU÷3F]µÿL›Šø^éãdˆ«æ»?àÍ8JçÉ,¯r{µa“9çï$¸œØºÁ'0}EÇ}0W=té“Á®cùYvs¾3ÑB§…ìyôRfÓGØÅu`Õ`î’¶Ëtc¨OR†c/_]!³“ãL·¿ó @[ä˜Œ;2ôþ‰?E7^Ì«C¾E9Ãç*ßŽ…ìv¬o8 ¬WKïËX	c¿Üàž‡”ŠøB­ÃìóAÞ`„Qu›	ñH#3e_’ }rlÿù‹ÿC•I÷ÔðYòpuå!áFÚ×xqcpM†õ‚ò $Yðm[v ]%Fé<I¹X/ÈŸÃÉ¹ÇB½âgà‡·´RWôqwnpâªÔãö™¡°JÞ;)ñV%à{! ’Y½h¹‚~`E5Ð˜úðÕ‹·ºy[ªH"ê$áÞî>)ª3 ';¿Ã:¹ÏùÓS"øW¨üÜé×O³÷ÜžÓ•BGð./SÃÙvH^w;|ÛtsI[šÎM%œø¤“d…‰×+}e®WÅ­|)bN‹›},êwŒÞšLzðÏhä¯–ÃîŽ465÷§†¥
”žz.=?ê§r
a“¯“NmÚƒãp©°®Æ±K×]–fMžX|E0¾yXÅKéfgêØ%à™§º¡Šû½1¡¥ºË5*”
6£Ê¿ÚôcK4j#1,nËGY˜RXow×ôú6Ü_¿vg&áÒ}|z¼B>Ó¼))¦–˜A8ËžGâ=½ðïYs†ñÈuÎâÍhþl)±¼ÑóZòÞå}“û…DWçµÍP8rÓÎµÞà?¥‡ƒYúJáTœ-,&@ÀÈ1œ÷*þÚÂÃE¸ê~‘Ô´s!£y$M[ê#SÖIwdîô©ðÏb6ûåä(ªc§žne®uqÆ»ÔbÔ–ç¤WºœüêðgY³wûuÊÒlïƒ*Q÷\ò§£Ù§	 _sÄüuS°ZW±*¦¢™-|(d‘.5nþ|wG14‡ååªÀ¡«'¢¨2!
·5@~2¯þ§Lð¦ÝAöA¶H{j<’÷RoçbFŠu1|gð 90òÈúÍÌëa¼²bÿ§½ùs>Í^+û-žkÚ>PkqøµðgõmK_fAÈ)tNep»$0áòÏ‘þ‡Új!O9±Yí1Î)n&0!C¨Šº÷‹×Rëq×3°üGÍÊ-ê1Ç±ÑT‘"™}h…ÑŸ ¼ÐÓ`'w~×0‘[±jwÑòóßµ(bk2 ÐšµS+	»ì„{}íHÓàÞ°(¶µÊ£~vÞ3
sÞfùQà´ å(÷ZLeZBÍ„…]DjbLŒìÄ–ùzó”^gÇÐª_øÝXO7—®rŸy¬Ûk²L*–AÚ‹ˆÆ7äŒ76£Ù”áé£›ÙùUm0Â{1­UŒÈ¹g`GÚîŠ‹Bž=_¯á×³> ñ7Óºyq5ð{?‰‡¾Œî³oÜºã}Ÿ°C¹¬ì‘øüþ
û_K¦¸™®
0kÞ‚ˆ+?áÃ|+JûÚ¾IùèÔ˜€Sƒ²0ï¶SÓŸ¢‡	\Êòa¦Mµlk5U.×OïFJ¢mnsÑs.·]g/,¢Œ×NŽŠ`¿I— jaÎô%•ÓÊ˜^kŸ¬˜Ox63=é}0´¶ˆ°›°(ßCÎ% ¾òsc³Ö@V¢¶âæ%¹ª2á"æT^¸jÐrV<h£ä'’Åß2¥&›¦NšÒI‘ŸaxÁ‚½ÅÅr^ç2–w‚™4»ˆ’©ü˜RÄ$ÂuËVÉá€’y×Ü­$”žŒ–5D%¥TU•Q‡¹1Ê4™	­µŸ¤WK;kâ&-­yÆº\†ì3_³aÓî®°§w3«=v7û¸ðÌOó…õ*¼ÈÄOkÉ
µ'Yœƒ¼‘ÚˆÄ°Å
ÛëÙ¹¢T¶_…o$íÉLFI”Oò­8¾ô‹È3g&µ9/Ó³b=n—ax`X…„8%t’6Í÷æ”×¦é»/ûši7­¼$Ö³\­t\‡â ÁÏÝ¾‹Çº¨sòð{AOe£‰-	%osˆ‡v·ðHDœ“ÒQ§ƒÝ-jÁ!Ò¹RÞŸ*2Úg†P*LoÊÓŸž£ë.F,Á£Ü$H^ªÅÎ´¼ÿâZÚ0™°÷gÂÀ1›Q­³ÔKˆb²Åºƒˆ­XEµ¤‰X>L(èËïzËuFƒFŠ1¼Œ2#²ÜÞe]MóÌÝë¤q<Žð>Ô÷Ñx~PR{`/díÈë÷<žœ4æ9Í)2ðO/ÂÌ4áâlÄj}AnÁDgõxômø_]Ì˜-Ÿ¨šT(Vz~3¨›|#Ç}‘¢î£U¼&Ì™cÙÇÏsîNV8À°á<–ÎL¤ñ6=î+Éy”%új‹nÞÔIê%sIgÿFÔPIÙ—¯ÉÙ‘ ½np¨Xµ»6ƒœ_Ì°þÿ+ÀS‹<%;Uˆ‚¼ú«SØèÏ¹#?q^]®N/kµ»¥j–¢ùÍKk¢ã­ü¿‘Q{GÅ¦ˆ¬]}‰„ZŒÛUcÝ‚ÛœáE£ˆ– 1ToP8½DlÏòC…çÞ²Ýoã²ú3Ý€Úmñ®@¼‰Î¾Àqš[æí¶›Ø—6à0¸ @R®c£é6RÅò}L3ÄÑÅº‘ìRA€¼E>%Àƒ¶„Ì1“Zm®¡ìñÝÒ»£á¢ÚîgaN(´û³dìWÚó­/ ?¯ž¶ÊVkBÓ	<zÓ˜,ûÂ¦ò3#<éŒ18$ë½	WÓ= ýu˜^}¸m­‘ƒ`KBm?Ì’œWu±™}XáwÐ€ S®{{s´‘0ÖLÜ ï¼t¸sI–.ÅÌ×‰‚0–FÐBlz'GGa†Ól}éº=ïšfsóÑU	1×›ò—ÝçµNtcÚƒÿ³Pw7-9•A
dá½J|èäÏ‘kÞà3EÇž±fì‘Z‡2Ií
º§-0€›Cv³b‘Y3ƒè†ý"­*ñ[Ø)_Oï¿‚Þ1{Å,\rÅ~ó®ìàh½|EœâJki)·ä\K¢hi¾PÉ¤b¥gOÚ[yêþÑëµý9hJ±¹'b2¦÷ìz¤"ÔÜà~ßW´•îY¾·ø¼ôù®~Ý“„}lZb†xV•kàƒL²¤X5„’í–hªö?Rúñûâj…a'“] ¾`‘å¡H¡?–Ê(£F”Î!·pÙ€ M‘¥7ˆo9)þ¶(ÅÙ+G
@q„Û^g<™¶*+úµàÀáJ­ÆrSw÷JöQžUTU9ñzjž²á'¼´.šNpÇÞˆÒ…d>íoÉz4}Ž	Ü­szé;÷u›ˆž•…éÈNÉµÅxÿ%<œ"êÚXê¶Þÿ\$/ˆ˜”C÷„ŒI Ò•Ûjz-ÃK­¸N\7?™«ó’ý:j&pÍ=•yâ<¿~ 3êùäkDË<#Xvò÷mG!ã6«êb‡D×• yÛ†ë£uá‡¢èEE³÷õ5þ|Ä¬aÒàëªá‡½hžÿ7Æ$LùXæ'a‰¡1X³ÕP š(÷°4j¤kÖ0ÖÇ!Çá[kkuÌ#-É[Q¢ =Í—63y8%g÷¼
æ^xÉÑ13ÝÕ9=°¥7(PÓ:Äkß^!ãÎù@6óung_6qpŒÜ{”Ûãœ*þçÖÓcTN@œÌHP˜äÁSûdgÎã»Û‚2€ë"Ü³NgóÁ¢"Ì¾)¿Toà´$tº»õ«ë²
WÇ¾–é WÄgÈº“fk4]Þ…ÝûÜ$F;“¤]x¡Y©§ð„\‰·Œ.­›jü³HoïØá%ÏP4%t?)^À–xan'ú+ÅUö\ßªîX$´ÙüU‹æoé^A0šá Àüeç³»‘XòáôÊEç<3|T Ó½ñx¯Scc—xL”ü¿ù–PˆOÅÑ`È‚GGÌòöîŠý3øá™âàŽáKàaªdpwß`°Ú*¡½¥Þ’¼Û€2Qmö3xÛL²£Òœ÷w­µ\>µ]üAìä°Ç|™_3b¨åÎqJ¤0¢1Ç5I‚rõßŒ	¡9eŠdÄbß+§8o¦ÈP°ôÌMj'@Á¿û‹|rÅt$`æW˜€òñÀ³–¹åû	¸Øc¡ÊAïJà$Þl[W½A·%ÀuÙk9ðir¬pãJË|6à,iUÕï¤'½¯ÂGAÁºÒ~¿%¤ç¯³eÞLS¶T¿õ,ÀÙ¸©3ÕöZ–Í—¦Ä‘¦¸œ(
û/ÔÑ[²[ìl6(ÿÄIŸg,r¥zCÓS7ÿ*ù¥ž24Ôbë—xÐÛò«x1 åOß»Ü«%ˆŒèHÄwËgÎØ0Ô»Û”¤ä/wPŒ0Kè=©—‹:—»³t‡ÑÆÒô¯¼cÖJÉ"eÿØ=PFÖ(-%r9Ö˜‚úùdÍZ2œ*Uo^‘ÏâGpAñƒÚ_èÊ]ÚQ^¸pj¦9cÕnw£^	Þ‚±ü~	´qJ)×±÷|ê!s ùR•‘zÐ™“pæ€)`»¨±Û½¯ˆð€¿Æ^£-“"vUG|‰2ì ýmcÐŠÔ´¨‹bUSÏPù_½ã©‚ÉOì+&˜ê“6•F€\Âåúìi=ýÙEÝ5 ivËÈÈx'œ Œg5·’ædø:NÐÒßôP³g2÷B­fzqI`+hfüŸß±)¢%j»ÐŠñV©žÉÒÆtþèOùó˜!1Œeó™aÔ= âÈ©úÎ†h…. ]¦ê†düï´Àxç‹Œ{Sú…=„µFò/‚q2Tm›Ô}Wâ=,»6–ä4ÕïÝ%éc‘œœõïñ”€.üáFa7Ýø¡Gt.T%á¿Ä\×ÒP’Ÿ}Øê¦3ðT‡.#àÝõ¢0<¡é#Ôâ,‘Â®ÆAU½¾kŒ€NËDæp/5¢}iª+(›¹ì¦HnÂ¯4Æ'dâ00¸´JÿiZ²˜öÞŽØ[Å+ƒôÏLÒ‰ó§pÈÎpn½à» iÑº¹ ‰‚¡T™¾ÿ€Ä² Œb»<‘ŒÎ¼¦ÈVœ"§ÕB
\‰a4Myôõ &{jµ7Ðr|3û*Rt]`˜Q{°ôîÅëØm6uÈÅ#x]¤%Ø¬²²,ÚŽÊŸYÇg,ðunûgèNûºWÓƒÜ[°ï¨‚‡?¢?Ž}ÝòkúXy†2fHÔ2Î3T%(…«Þ–+MŒdÚDÿJñóa9
¾YAÛšm gA°+‚ºŸJçb¤¡?«L²-á¸ží¨¥IíßÜEQéÆïÏµB-£©ê—½É‰Ìï;ãd¤Ö¡ä0/ÈDw›ëFY‚3XÁ­[fêG°Á…wP(”7¤ü
ÀéNu,ÍXí:þv±Bð%²k;%ê¥ô9Ü/küŽÐïî|Çs×ÿ·T®¡äE¸ }Ì
QÛÍQ>]‰ë,=U7Æ¿ô¢“øÉGãVÒ §4hçjÐqÇŠ©|aÆ½+ÑÚNæ£i’m)9Q¦–(éN3Ý[Bïó¤ÿPÝÖþy3S1ÈD }ã‡Áåè(åìë2ù
óž§¡ÑB=¥ÿ¢Ð5ˆ'€Uô<‚ž7+³o3êë’öö•¯ˆhä#…p'~¿]5ÿ¨¬3Hùï$rÍž!ó‘È2•Î&Í¨r5©Øì(`¿ø›Ê–±-‹u–œû²
G”KÚø);ùôô‡);ÐÀ]k¼tU e\âB Þ–‚¸ý§‹¶êÖ³Uè’_s4£1¶Ž1E]Vþh„=½ ¦MØ2%› A.5C˜¿Dƒ©ŒÀFô#¬t«þ1<xþå¹ŠWÐuê¦XCmBýM5³M¹k÷µ•ñZ
àQû:Ó³ÝêZåa±<OW¶5®Š#­‡[('3¸ZoDŸ„$ZÔ‹¿–?©Ú‘ñŒ®,G¬7³[…<‘ýûÕ–¿t,	—VlYËNÝøc©vh„î*D¼käÚæB‘Ç	Øí*Ô‘‚Bœ2gx ù¡çžr¢€
LÎÌ“×Ø[AóåÌ|¼©'ëSý
ÆðÅ]†	Ì¨ÁÊ!†æ³hOñ'[³
Í	$§°±_TÇ!fÐègŒõ‰¹í`\ 2^A<ÔI=F]„ñJô´‡ÿÏ%9Õ²(}{ÖÀ.íô,Ê¼	˜«P+MÇ¿Eë$p#wFO¦O‰;éæYY\ë„ø¢ƒÏ8ì²¬]Ì$×ãüÂ(a«Ü$}â.¢JXOëçi£4pý5”8€ÚAYŽPGƒÛ«ÕS£·‡ø‹K-¡R¨‚­ô˜öšì8qiêÊkÈ7ºq:Šøå‘·)ì}ä@6ÈA§Ô… L˜V?8ÎÒ	ñ7Póú:4ì<€é¿žÁz­¬·LÙ±@´¡©ÉÑéJR0àpÓƒKœ&%žöí	ÌoU™Ô‰S~’S¬éÌÞÚ×K9g­<;zypƒ—w}!¶ð¸IpgÐ¶Ú™Ó¦7È·¨Ò‚³g\´Ø£IâÖaá¾p‚–¦c¤lRþlÓÛ;‰ü«aª)jƒ^óÉrlH+äð;®êN‚,ÐŸö$òD˜“·Ù“{e*<Ëü' U<8EÇþÀ2íX­Ò‹ÏÕÇ\ž ¬÷cØûÉÖøuÏo"“;–rŽžJ_RžU5úõžSæAø>žr7É“ýY0mˆV´ø’	\à>OøõËwÊ®»!uç…1˜±ÛpQWlìi~­ƒÃU¶Ï.A‹ EK\·Gº8k$Ê”Ð”
‡S“žH(:œU–r‡ÝÎµ§´ÉÄ‡çÕC+Ñ·-à‚Wþ¿5èx|ÃÀâ¶|^ŒD0*)içÄ0"´25†æéNÆ=D:‘³n0‰ªüàÙùá
mÃù³b)LlmL$·9Jp b­‰Ëöà¦r"¢kžŒé9ßãfÔ¨xÙ8Ÿ%ŸÁŽ\‘®Ùç2LHB·È›í<gŠÇZeÂK»{]#DˆÞŽ9v®Ì£©p J†KÏ‚fô×„êwƒ¹#ZN"+ì"‚c4²ul@òIr{î²ÄÇñs/’€åv1h¬(£wIj&E^¬6šÊ*á|¡R\_$B£3GWÓ‚á_÷QOÌk¢S•HætnGó–€R­Gr´S»QáAV¶¨‚¡…Ä5¢s®BX /_bÀ4|Ó¶
f>`gÛóœ.ñÜJõ‚È†˜®Y0¾¡u—ù ì’;Wúþ’«%êÑ`pDü*MŽçTQtq•ë¼•9¤÷xí:1*#ûLæ¤¶¡%æ]W–S
=¬ÇÆX8š¢¡¡%ˆ%Sd;Aä«]Cº\óhüÐR`[çc=ÚJíe@²ù¢ö#(½‹[| þ}Þ;ÕTcÏj‘8—?†<£ÌÑ`Z?¢ý¥B]ì9	~	·™¹ÌEÀô@O!å
5TìÌÌoÌéV,¬‹¾Á¤ë­ŸÜ¬ìëh¥ÄÄ–’°xˆðÞÊÅÍGGvþª>&QQ»Ù‡Óø¶#-eÜ0Ÿ\¤^Yö†ù8
ù›$ñÛ#„Jµ`¹-Šv¼ãµ@_f:†¯È­Gž©ªf^âµ9ï+QÍTaeÄž›aŸÿcR>‡’·?wYFðm[?V K²³e£9ÑÜ<rÃ¢G6,¢é(>ùuÓÑðýp#Dc†HÌb}XƒrÍ²ùU€ƒíHÐhIS’Ð\5lTØ*?)¬£d_á™í‹bˆÎHnõMá°Tõû KÊ\¡°XÀGó£&ãp-YA™öÞqª¥qÀn'V9²NŽ'´§~Ä.¬Þ¾â–G‰ÜL,òô/›,4È|D¨4÷Ö›êzîÕ•{=‚õÅýYc€Ã80ã@f¢ð‘»5Ö”°Nˆˆlia©¥¬Õ¥”Ò‡ù!9´Só¨`7/	žÁ®’àÞ¢³KÏ"ŽH§§‹úw¬bÅn-+š[ÓÙÚ~{%±€cýÌÍ¦{GÕ+Ùs\‹kfÉÌo…5f%:,(gÂ¿ÒZÔòÚÓ}|éW….†ˆØ¹VmµWWÅsRó/uþZIsî:H²“rñöËZ"]!¼™‰ÜÕÔSQ7Ì ‰=¿!¢=ç¢­\q!ã¶<F1ŽGV¨¾Ú Ê¥|PëMR°@K
€îQ\äÂ&1ìV‡kšN_k¿#Æ¬x^üÍx²ây8#‹™š£Wùæ=ü‡tí £EÞ“ew9`I”¼] ÎÀwnï›ûó=zàºïQdì~u„7×KaÈÞß_üb£º¹Â\›8•¹I¦Ù¸‘Øû¥¹‹yJˆ›7ß
‘¡ZŸnôj‘8ø^O±€d[ý’‡1	¹íí˜1€ÔyÊÕÄp~ÇòL”ÙW6í¦[pgU[Ã ‘Ç_Ó0]õ™è€Ò­xHjöoW|l+˜ùû#9L_DQ-ˆélQc1ê¡ºÄO›U‰Ltå‰PEÙÅ_·øRAØí›zRp¶¬ØËÕ”ý¼ ÷É¾˜ðÀÞ/1¯nj:fÛ¸•w65:Ä%3àOgíf5Ûm:Ü/Ì	à‚[:ñe0…cbäæ½±QioªVetË”^×úÞ¹Dr×iæ„ °vMãq…}Ž|:5·b¹Q>JÃTu”^°?»¹0Ú;€æÝ*³.B¬;™é2H¿ ‡„Ý™‚EŠµU/NŠÛ'/ÛÍÏõM®–ÒðByÍó¶Ð:’Œý}øL™ÏÈõ?òÄêîŽ#uîãX•Aðºaˆ.LýÆ¼š j0­x™¯R½ýÉ
iMiè(Nö;£ðÿ«“Cý»­lr¹ŸØ¨-Âj{`ƒ­¡úËãø›}û Ùƒ/Ò¹(7ž`½ŠÜ“ _ëçq|ŽV¥sßµ;hêDƒ@•À£( íj½aVµ{ïÃ¾ ¶Qñ“×*à\ÝGjþ;¶±s’ëøÿW¦FZàÎáü™Yj™2ê@µ¼ze=\êízô“.‘v:»H”_Ñ/F’µQFkŽð¸_òÞ«¿D¢"éZñ'ÞÄ}Ë+†šep`Îß‡ÐïÞ,˜(Z•Q…^	q¾GÓuŒˆÃ›š×ù£!µ©´¼#D™³±öE1“Qi[|ÑÚè×r6üODCŸ­åáï¶Jò}XÏÂ·ïOâÖ„ç¾'½y¼†¥­¦"nî×Š7Òy¢úy.Ëá›Ä#8,Èè´+©œâ¨sd¼iÛ'Á]%Ñnq¡Ë’¾H]Ž%úÜø ÌÆ›{Ö’ÁP}Ïc!!·DpÌÔ”è@!*r9FOp_4ó.*èXo4¤N³@è\q Ô{aÁöøCïnƒD¹qdgA–Û²|'„Ì£1Ïe76™#´ëhŒ\¤Ñãò-¢HÕÐ1·ª‰Î¨æ(Œx¶6á?-à¡þBB&å:üŒºƒ<Mš‰bå6–L5>+ÔMådÄò_©0>:Jáÿ
\jsþe¶1‚ëü Í¨¥FvÍxä.ðEõ#Ó>ý0¥¸Ã—T8rZ0àÎ¦p hößšK_·{É,*q§¾ëÄŠØü€½]ãDºˆŸuG‚ØtúýéÆó\ ›û•¹¡ž.—7œŒ;*¤oEöGêœ[í2†Z 4°!Fób&ER} ÊàTŽÕ«)8RÿR^ÓFN;El	Çˆ[Ë’žd[yàYóÌàu_ÒKçVÐOBñÅx”›s	=™öCûºgS=O4¬ÝOœnÈÍ‹j8l,Òfmà‹HÞ†[nr}x/)¶BâYqEZÒ8“£ÏS8‚X´ÃmƒðÝ;`OÊ÷bŽoŒŸ‚t_X©ÂNb_ÓX$TÁâ¢ó‚ËÝk!"°_‰`ÕòÇ]ýÐ&S9½"	ËXQÜ%iVåtÜ0$}³~À„`qUÐòåŽW;ßB i=d^I0HwÇš	ŽCÂ;v·Sú–6U¼f‡…aÓ Iûòiñ‹È,Ñnk‹œò@’GòÑº(råÖ#s¨YY}O#g$NÒ„Ã½C9Æð"ŸÞP÷÷_¥z»D4¾¬¯sÔ×ê™|ŠåK7ÏÎ
ú:B]’µM¡Q…õØ£=Y©RJõñ÷‹}/x= Hn£(b„ƒjû<‹DÇ^èm6S‹Ããsz^Â
0/
(ûö&vní¥…9Õ`»Ó#¬3ŒÀ³r…aÚµ:cY"EK„ïØÈãÛÁ¯Õ¸èøÆ¥:zuÎè
eÞCˆ íêÛzu”„‡nZ~º¤ð€EX¡…/ô<š0¸Â&™B¤tÓï.o¦°±8QãÔðÂ,Ëß
(íCk|²ÜC
>ng|²&EP”1ðAÂüßIdÅ‘}R©ä¶­–ÅÀì-Û%WßÇÆœPø*¡²©ÝŽn(ƒ »û(:-$¹Q?ƒè¯`E|´ß”±ºñ/´G )À‹B¤•ø„ÐKçûÌ9ã]ÞÔÙZWR‘ÿË¨•WÚÙ¢ÿø³pÂ<š÷ÉNÀùRå›ÆS.&6Þ@¶ØÝG¼4	»$×ÝŽ,d¿¤éÓTPÖ½ËNÉ¤“JæÜ~ø|¦É¸«H?ÖZXìTn1ZðÙ7)&Ý¤YžzàE!¿¼’»–;>¬I‰Æýå<B]<°ßØ,%ñ{>”]hsÙÚÿîÜÉ +IJ
»Qê	G3‚+t>+¼Ó’ìÙÍ»×MlEq7ŸŠyÕªÙW¦¹¶Ò¾Ž‡$oì“êB÷¢Î¬F}ög‰Æèi ´úõ¯–“¢ƒ•Xv:’ý8ˆ® ¸¨L—o·ƒŒÍ)ý½Gu¹b¸ÄÁ6¾„EIgsŽ»ûh°ñÉ>Ü‘íÝn!q§Íø3ä­ŒõQI1_£}l´†¿ÉR}ª³öbêIe¹}(ª#•ö÷°)ªrÒGƒ0¿6Ó^gÿ ÞBŽDeÉÕSRd~	¨vÆxˆId´Œý
ów'6óþ%[æ[“rF×ÉòõœÑq‚àFÑÓaHÌ¯¼)B75A>÷ñ
°“N8„;ÜÌQ­bh™õ5cÃ¿‘9”âBäÄ×4a7 ŒÔÙÌþ¯Ü•ùZ}ròo¡²é·N9€þ?Ê²6¤2ÄCJ¾ØÚáÁ«YîÜÃh´K9ŠõøºÛF™2Æ…0œ:&KÚ¥žûª2ºŒÊó+}L'ª Ÿ|¯°|îé	R…Èt¨^h}°úÓïc¢˜>îí·ÅÒ~ÙÉ6ådc'ªŒ•ÔŸÚÕ“—É9Op¯©¨å#	î|ú…ô¼û}[u“lk®$£êkP­ÙÌS«íµdAG©?…®4Æz:oZßÿûÓâ€hË8«ñÃh+þP´,©ZÁ†ñC ÏPW-~ÃÍ°èê7§7—=ˆm5<ñŠ³€„í)•¯œEÃøy“.A¦ §ÙÏt¾÷¤ì+]_¡$lÒcUƒ–	+ém¥¹4¿«!%FrBe~âÃ<Ø:ù<×•^4ý¶ø"…¾¶ÌËGÁ't|–­5kó%}W¡•ÔÂ$ÄB+sp ÜíŒùsŠ¤‹¢V¯ºñ”Ú^c(¿ÚI!W+¨h°¨l¶Ú, A¤¬dîËJêXÎìæ¡ÐV¡5¥p–3"ÚU"^#óˆjè¢¬DþÅak§ïm‡mqGu ËF45Òc)Í8ü¸‡jZÓ€¯Â’)\‚6ßÎ#CFœÍO7‘Ô¿È)‹#mÀ&"C<PNà¶±a NÑT&Wuù\‡òù³øÚÆãC2.`8§·µ %Žìáô«ÿ9Ý}5Ê/¾F½ã~Ž»ê åoþïŠÄ±zN0Sê"¨¨à¤‘3™Tðuë™’àbæ²ÂJd¿"’®?‹ø±É~ÄžÕ)ƒª¥/ÆKA0x‚jN6§i¬7ÃLªQØÜšñR†Gó¢ÏŽm®Ø2û%);«t‡¨èèíeV¢yõG£¤”LL£DÂ>ÖÓJ©I)Èìµ¨Š=VB÷H[»Ý¶wÖÈ=Ž†ï_­ãì¾ñ5!kxM2‰ªGjù%ßÚ»‰°"tmr2ÈX/·õ×<;ÕÍ–ê¶f-ÓlˆÀI®­™L²NOÒß¸·DÀWK7Ò°ÂâÀÜýÙ”sû{™ô¤±ÎÝžl—n4¶4ã`1¥{e@xÀr3Ìä±;]ÀoÙÇ½j¸-êŒ9)eÞŠ™Šcèm[7Œ²ëÆIx;ÅhÇLKžFãV@èœ9±ùI˜"3ôƒ>šÞ¥—·A‚ŸH ¥næy‘—±Czô0~þôSI</{h›mäðúŽõä,«½(Ú#£lôŠ4§À¹+òýà‰6øS½Ñ(¨2ôÜÜëiß¤…R›g¶9ïHÓ˜ûˆÞÛ¥­‚ãÿÙ@XaÊw™? ‚ZVhQ÷ˆúœt¬¶IæùµÁƒŠ+—·‡U\X59
;±˜bXè¤º	¹}¸‘4m¡q^—eÃ„ì»çf4ØT„'b%BUÛ‹hp¦¼Èº4l®=½ýñ|;¬E¡ªÑ„’º´,MžqÚà#Éß®?ÊÀ%)»´ú.ÑÉ \5ÚšXÏ-#âAú™ñhËG÷ÎêÛ
²r·0•'Y#¼Øz˜ÙÙµâ„zRzzäýffO•Èéñ=‘ð>ÄŸ‚,IIå”Ã­€&•Ž,èG	—,SF„-üÆiueÂ8¶Òg¯?Œ¤+•ª­8íñ÷~Šª”2jãëøE_™TnYÐµ-®Î¼T8Ïg¿–oe#ÄR‹¬ŠÚ¦sš_ø¾®¤Œä­iñ€Øæ&"¿¯|^ÿïôÒ«7¥ói¤™³•”-÷Ü2k]±8°˜6p?°·˜ Øã]Ax8SÛž-+\¥?ÞÿØ‚ï~Ù9Øåârx¯¶yåÍ¹ZçÅMÕÁ]ÿ ¤‰".Ð’ Xº[äÅPžak"¡§áo‚—ãûs¨Û¡dzî8CD›rß¾7o½½f3Ûk#5Dã…V…ô~tÇîòÚ„ä­çD
GC_õ“¥ éäÀ"+²œ”áêêœÊ!Z¾‡×ÜKÁÈ-û“\oh.¦cË¦ÈÙtLNý8±$ñ_Œ ÓZº!–´jÔKŽUÏ¢}F¸˜÷hŒHgÒLr{ÿpîeó¯J˜W§ÆEaE¦…oymþÄ×ò nnšâô¾"Gç¯ª·ÃA£øÑƒŸ™).òøÛb S,|=„òðØœÔaU²)}Síô7¼\©¦NeïÎ–ÅœˆR¾c¹Þ¶|g;·—?­²aý$‘¬÷LÓêAGt~l€èø¼uEsqm™ëk_ÃÂ9Á6™-ÅÂìãöwP“YàM¦L	8§ùªRÐZDî7Kó]z'Fg¼³‰Ð&åíî#³1~”
þú€EïŠçÅ«‚UBÝ·j¸åß‘Ùäü@©4x´ÊŽy15¯>ŠŸw§¹Nq²ùÈD„ã±ÿÁJkº‚ÌÏé[F÷¾V,¸tàgf«8	ë@	?ãµ±f=Ñd·%ŽØ,Õrí¢‰üEe¢²ØQó^ØA¤±Eß§­˜º§‡Í„s£EL~¿®ÏŽ¨)üå&‹aðÇ›¯ÓO>MÅ'Q†„Dº®-Üv•b„†'Re5¬·íT “)KöKzUMzË˜²¯?e¡Œ±G¨ž)XÜ/Å~ð’#ryŠò¦Ør`]=AéÆJ£¢öv·Mx"†ôjP÷+õ^u¹^ÇVë¿…'|RÞîº3èpäÃŽ¬-«¢´œ¹á/sèyC®¨Ò“ fšìÛ‹$ÌCo¸[ÝÍÕX
–>fÓ è”¤}Íôê–€õßÄH_¬Ììú‹ûéÊ„lÃkÈwÞPcF£ ò¨ç«KZ(“Ö{³ ©_q£°/•^O6ÿMµ¼+…\‡?EW»(ŽvƒYW2Ó}HOŸI'ltÑSxU‚$„¢{?HðŸ}$\b>Yrd°âgž°ºº´E`ø©!ËOC:w±M m³!Ñ2°èµ"Õä¡/&)‹ªî©ZÁ¸jf’3\$ñ\›¬ufZLýê‰¯yåŠŠ¼ØšwO%pÝÀTXvâí>ñþÕÑµUcîc·C¢µT"BÜšÎX²àÕ¿þŽb-´õü-Žés5´†O(šñ?ýúcpBç¬==éS“…ÄÉ
L<qEýº
gž	{Uj!“÷·šÀDP@À²o“íë—&Xmÿ2á×©·¥WÀŽÞŒ	XúWXÆÞË·hG‚NÉÊç7ÜéGô?xj$q$Î ãd;–ÿÜ€“Ç]…?e©Xš#¯^BÊbúÀ¤,cm=jÚû‘.¶rÚ„. u×jñCø©­Ï"è1ÈßJþÙ¤SÙ.¶p…cË,•>,ö¼^ÕÞ¹-yÙŸüßÛ
³, üÞ«6µ½h‘ÔMÌÚVÚ­f1±`–æ„GY?åÍ‚–=íÃ4×8ù<˜œ§|m±ÿ×'tÅwŸÙ
±ëëÍÈå¼V½&¯+ñ@ÑÅihÖK¾9îÖrms¸òÅ‹û/ 4:çÕa¾¡RÃÛ¸d¾d3-®áÐ²Ü[£Êt¾D6º·ÏUk]‰È7vøKÌuÆá‚JiúÚJ:÷Yy­žf$à3.õ{ùýL~¢†…Y8fÑ/¤|¥g=Ë.eD|¾òal°ý×sJ%W6ðZ}a¶6nå Îöv­´ê&]ógìûgß+­}Î¢¸³ö¦ÌŸfÂJî0–©V…c¼Yðc‘˜ÓƒQ±õ.]QP¡klÂ¬ËÔ…¶0(/s[!{×R8xÇážŸ$Ú(—’søŒ‡:¸Wg£µ!²2Ž™óåLÌjDÛ¶Kº`œWz/ôhvåêG‹8Wž2_ äÔ'ªÓ/¦Óz{~òÝtAr'öàaw¼lš;Žâ€0ÕžŽqêÖôÍ¢V_QÜèŽèL\g“•§8	+FÈ‚çeÒãg{wÞhd<‡¥ºAÓ=ç¼c'Íj³«}ýa¬€8é”ƒ¯º’²‡ðÑ4bÞ¶ë<Kÿtô¸®o@ö&YQ¡ÀŒWy·zÐ$_•DDÚûîiZáúxÈabÑB'.Õßg„„í)€Ð†Ú[öŠ\¡²sªë„Ê·–UÑ¨åØ9\«›fžG5÷54¿ƒ¾o)ê†šÂ´¹U0æP¤I˜oŠ¢ö>!»ÝC¿"Ê”ÒcôzÊY—çq€¶l†C©ü¥z(³º›<¨Ò$aèËG,ŸVŽOCOÎz4e¥ZÒ?í8’aÚ¥²ë\êó[ÏMÉöm¯9sOŠ&•ÒàUÑÙvE³°$ŠŒ†° hpû…"tŠ3öcuÁMjÚ¹úMäu}ÓÛ ±Ë$1ÖøÀY´¨ä/ÉvøiÈXO1zDÔeDj^åÎî¼NÚ€ÐÜÕÜ¼4'”Ÿ,¾Â`cfäÏÉ“÷`T¯è´¤1™9	1.¯m.*é£‹Lyvb®Ér&ñtgqé8Wðo ÑÒGn¶ç Áâ’Îó,{&3ßíX;¡áÆNIRlÚœ4÷01Ó8*¸ÖEØ=·Vºfì×çž=þö OO¯ÖÁ®Ï‹ÎåeÔ˜”¸M2Æeçññg¤PÈŽ­æ*PŸˆ‹
MSŽÏ“Ù^<.ä‚ó«)¹ô”÷rq¼ñcÄõ9_7“Â\ê‰9M8«Q­Õ>+œ7¢$_80	AKX®÷—RþIÛ½Éï­íPW#,©é‘Í>­)ÏÕç›!ô˜Õ³t4öl»;#E›û¿hq†Î$â…É·ôe9º åáB)è9¼ê	BŒó$BuXULÖ¯"f}áV¼U ”Eùœõö1‘E</¾=¶2ÃÖêJx.µùhhSÆòË±Œ½€¶BÖÝ`þy´E]ù‡ý|KÙ((ÚÍÜã±’@¶ä.ëqX¢#,wÝÆG*2#Þ ± .“e\,»”
NlX/ù@™tWÌ9tûÔ‘Ô$ÏíZ–c@Ä<pîJkK@4 cb$œ‰C^s¦ÐÛr=\W0à–ëx{­i’p(öâVUÇC¤ï<oá”P*NÀ@vÈ+ža–^ŒÆŠE‰¼Ïn¼”O:Ê;øŠfWÇC©Îô€„±,=öÕu‰ˆ_éW±Ãå
	Â;/”vuA	ªàJ4åšD€l„t‰
 zôƒY*•¸¡ ¾Ùý7º\]Š›=÷©AÖfÞœ£,è…üþÁö®slñõt‹¦ÇÔÝÂhªéNª’Ö
5ÀÎ œ0^ö«ïEpòÛ„Àòº52ñmËn9Î®eŽ"˜/þ×£µä21ÞÝZŸlš-Ã£.¸‰Ž?tcŠ™`3Bø°ÌŒ¶!hÐP5÷’Ã´¶]¹É×Œˆˆn	ózìvÌîî-a"œ²ó5:µjÎßÎAð l}æ /ìúüËôÒúdMKx'kp[åÛå¯°QÂPã› ™ìã0Û=ËX‹A/=k‘ëŸá—dUÌ7@úÊ|(‡‘Ica€¯Vö?¨ØxSÏBþÖZòW\}M7„ÖÄXþã¾ŠƒÄi/Wú„¼¹¾}¸0°·ì-£ˆT=Ìž4{”ÒÒ'Èµ¹©†šAù”ï@O‘#”ÔÐœÆŒâ—‹T~a*“¡÷¯vF€µ[KúŸó“…ì¶^§÷žijZ •»ÓÏNHë^àÍúÏ²z+ëø¼›±tÕ±'wóÁ%ë3¥Ã€`¨…‹rxÙ[ÝtõiÎBãêˆÊ•›Ä”G'†‰Sêo)6>˜2¥29\øùÌ»i­‰v¤­üTk‹bðÞ…®(F„”6NÜ[»úÈ1ÓGª * ³àG4™º62ÓÿFJsëâ”×ªÅ^÷&ë‰¨² Š>_ðvK*'’Ÿk.°B¼¥w âù¦†U[S¹ü6öæª…ï#qÑ«­Æ»?˜qQkß»½œ¼5½¸5¤r=Œ„=¡wRC<û?AEþr›9]—ŽC8oßÌ+Ïw‹x:ªºDÁÞi‘ìŽÅº&eÎ
›qÎÝtQû`¾¬ìs	£Ï1·Æ¦™‡'³M—PwfíYÒ»‚;ÊþÇº•!0zØD­êi1ÆæÔT^{ þ¸pàBý°L˜P›YÜnüî;š˜†ÙŒ¤éSð†‡Ÿ´SY…#4¨v._8?Ãì[œÀœÖ
3–e˜ EIçä;%ï÷TXí<ŽK¾GØ‡Ò Ì§à1xìé;s‹ä†0•ó;#‚Å	è¤E'£)›qáøŸ¼ýSU+ÓÏ›b~ò¾k±€!J (âÙƒâ!³Paä®O¬	”GèÉ™_ì¢]a9(ß.Ò'Þ3ÿ`F¦öµîkè‘ƒK0’àÄxªøt¢0g,„Ø:0&v¯ ÝŽ®¾Ó(±”š¢ ¢Í„›†^¶1ðê@µI×6Aœ_V$)ûÇ©m$ªVö*Œ´ª@©š·,OÊ¨qA”I[:þ][ø>ö£ÓÓ7-ÜrRÿmò0yŠù¸‡óuo¦DXøƒ/ö*<º	9{Fjv‹Ê¶™¥6š‚øJâÞ€é=£nØÃ•”»PØÈû«·!¬Îf¬9¨v:^»Èç<;Hq,Dx´O\æ^€½uÌ:À ©þ7OÛ½ ¾5ÑnØ
«UPzè–k»rÏî„†c4}
ÍÃL?Uäª·àÞh¥Èªê	,Ðz+^¤³£¬W½?¤û£Þ8êš ‘”huü6JtmÏ”õŸpéÝRæg*³É>FúÂE{ðÙŽâá|DôI·¶ŒB‹)bé­Âwó}éº–—ˆE˜®áíK·ØmžòÆÝgãÒu{ú.uçŸÿÆ?;!êêñ@?G Å ÝH½û‚º,E÷‚Ï)†×uS‘ÊªWòÄ;Cm€KzLì°š#ðnPP=Öý:Éûœð.ÜPÒ"ÉlXÿ\Cae"Æÿ"T„ìé€ ãh®çŒ—/,æ;9j;0H D—›ÕR³s$˜NÐþ~¬wáå4õtà­š.õoš†îæe|§ËŒØÒÈî»âl|7Q³òrã³G*œ-É	Ÿ8RÒj0ÞÇ«©ò/@Ë.¢žBal”«uv.ÏåpôˆØóïµµ™Ÿ¢vñ¾˜ÎÝYT×ðWœ4Æ?lRÎÊµnW0†«:H><El7†².ê¬õ“–7÷“Î@Í]Œ¼××oŒ|)H‚I÷˜)† H sL²n”D‹QœÑBÈçÏ‰‡	é»ZéKÔ|§¯ò¶ëM•t¶]°
HÚ,5hTM*FÒ­€[ó¤CÎØH&¥1'ƒ1Ù$Ö-ÆÜwÍ‡*RÃîX>Jâs-Ì…¿äÓ<<Ú–Þ[Žß”^ï‘W¤ÏíZÞ‰•˜(à2dª²û½ÃÜV­Ðv×þãh|kÞÉÂËš½Ø”à/þ¼›/­¬³5¿• lÅâÆ•‚P· KfàºÿÑq[š8g/ÿôf„H°yàTö  ‡8'6Wý\KÿizŽÖÐ@Mkéo\ŸÝbÏgã¦ÚˆÈSØ%úrÕ€yõrÖsÁ):›J¶ÑŸt•¼¿LŸ_tÙZ	V áø³ZŽd‹KÓ
“5—¦vÍ]Ò‚Ó"ëKt¶^›“òCÏãt4e»/€òV>Õ;4srÓi‘“Í§þJTCLG†¿Êlð,£®Šlç×ÜP»J¸Xto—I:Sð  då3¹x8AJwø?—º!â°JÌ®<î£  T±cs"7Ú…¶xC­áøSÉ-×[©Pþ8lÆø°Fqï³ôE„öØXàVªAkºvŠ¦È×«¡0ÕÑž#ÄE®K%†!}×úî·Žu·álèDLÛ}€½Árã¾N€sT±|žÆº°@Ø·?%C\Ÿ
<CÇ‡{ßí3†‡¯YÒÊ4[½uáS2…×êÎ›yÔŽ¿IûTÕ¦>/ÇÃ¢jì!ps¦?§‡á©$ÖSº;Ú-¾(1kÍÉ0¾¥/¬Vl‰ Õ•OZ¡žt£÷¹¦)ñvVItxlBUÑRëƒÊƒz¥¨•âÀzH¨Q,ƒC£òcåÒA"Lˆ•óí–‰*x;_l?ãÏ”R˜¸“¤˜Óˆˆ¹aRÒa§6ûÖ^"ð÷f¯3iM¤¤ÑVù?ˆ–Wm½úã0Ò‹6W_ª3ôµ	¸Çf?4È}0xf4rœÃÀB½Ôó÷K­"Glï6¬`ÔVY¤va?Îó›Û‡’Záš¼!@™SoNžµlëÎ%sû8J“)40þ¿µ}Ã¡1ó´×EÃøý€re¼¾ïøC½!—›ÿý6yk‰'¬é¥s€9Ki‰bXÔWxü.OéÐ€ÏYOãn8mÌZ,Ô™”Ð¡ÌÖ"rg0ÕÕ…¿™³¦×~øSø"ÝG]è¥•a$ŠüugŠ =-7ÂÃ	N­[• …´¶²Ü©÷hëº5:¿v´µØ7±C¿˜5m^^³¨'ªúc0Çœ¹åP{ÌÉ”:zË¬ZÞÛ‰“HHÌ4¨k`Ñ[Á„/i‰;šÔq=£Îì·IÀTÉµ—nŽúyÑ·Ì.@ÔÐxö„
Atb¸ªsÁÞÊ+–ê(5@:¢òôt›qq
á¢¨¦ÆÑŠÿ¹ Ü/
(¤)jën[ïxú_HÎ4Öõª\µ`	ØVò ôŠ!ª@·Ê>x‘ö—¸43gi¨·~&·H§þ5'kÕÐFÎÎ éeÊ
£h¢'LNfzÓwì¾†Î›€@ØÆGç?P'’”‘æƒùéUr)['eæ=ìWrE&@£Äˆ“1tï¹GD•ˆ1iRT ¹¡žÙ t¬`ˆóÖÖ/S˜èòþx_º²•0_ªÏ$.ïUöõ›-
cMšŠ? ¢‹15ÒÃ´ÛOÞÆÇOøÔ´ë€y>Î“VN9Z×8BS6Ú?î-¯ÛÿLSzÅqjùÄ™ …4»`R'(:Ôi{Ø‘ÚºÇÿY%ÿœh/RÂUd¬Õ[•`C®màMY‚‚`H›G¢}Å_ƒ¬¸T:´“c6oÒ&|Àþóší¨Ÿf)qèiÚßYÉÃøÂ‚Uiq&ü B-gªf–dÏŸõ¹*7@üZlWZ'}îÓiBjØã’Šsåw¹¾ä£Y(À©±ÁÈÒ±2y‘Ãs&ì‘.,Øa¤ëßÞ¸Ü#FH3ž…Ì v?£ã&)ÓMHÇÍrÉ'=x‚±Þ°ù±É®ÔmÄ9y ÃãÏìÀŒåN
uÀ²÷EÇvµšf<±ƒXƒº$ÇYãõÐ}Cróû¹fbk†}D!¸KBŒ#X°UO±eR´‹Çi¯ªè›cÓcíDÙk!X+\îPÃÎw4~.p"Ý†w‘1Ú„½ßS?Ð˜ôh?Îæ1Y°$™Ä¹·4*äbÕ FC\„<«Ðìô~…úeŽd÷=Câc°r]â˜ƒ_Ñ)Ž¡ÓN×ü†â0ãÛ ƒ;3Æ"7sw®ÞSáÈ-‹µp‚X16°3a>Àì. V mSYÂ¤:²zjî„~ø`£ÑzŽvŽ´ÿY#±Ö7í~Ÿå¸Š¶ƒ“Œ7õ,î7/¶ù™r¬è#¡nX¦rÞùr¹æëÞÆ„,zËQòë¶lŒÏóE«žÀI3pz–óø´SyæLPòŸ¨ø;¹*ØIÚ‡êÊŸÚ…lM#Ê*´ü#*Ì\Ú2Û—¬T§S2Ií­Ë×Á/H?9rXa4Vý_;Î€ÁËù/±-„úãr0ÐÝöd9„~ÕÏ$¬VFfèAlóËbÙ\­á+tx §õ¿YáÂ‡AprÄñclZ·~±­Á}Ü×Ýö„¶=»S¸äÚY¨i*à‘×¯ý`íØöŒ1ô¥?·Â¼zÐçˆçÀßmå ¿ùr;6`!·šyÞõýQÕù™_Ò±µ;H‚ÃîGþÔ/1ñ+± È2ì(£·N † 4õuÞGŠ8­IÀEGÄ"ñçq¢ïúe^SÄ/d»a4ÕsoP² jG%èFB&@Ê)âoú–&_µ•—¬ÎòÿÃO>a.zí'b$-†,’5ë{ÔÁe©ßqatëœøzíÁ^Üäe.^X[ÕÄJkàzžÒŸcÄjC•8àE}\ˆ¬Ÿb0ðNÃXPÚëü¯âþF×õÁˆÊC#‰¤ùÀsÏ¸þ$n‰¿ÞDû¼àM;‹ˆ†Gò€öZHä=³ˆ2–Áº
3;^óŠ§Ä’#yªTÇj2/© Ìà¿§Ú­Œ­±¨G&>ÉƒW—øI"¢Dwì0OŠÝtLO×üÅÆ¾×æý‚§.4Ê×PEÒ9o>dÑFÉC1g#CuhÉ”\"ê}/í|!€E!í[Ü/XNèc2¬Úç´Õ.ýí>ÙÆN^Gˆ±vˆ†§ïŒÁÝJ¸Žè	Ð+{MT8EÅÞ­ûj+:û÷Î™…û;èÂv´uÖ4—_"ÂãÊÉ@áÊÄfˆ_bx*þó6ÀkòdÐ¬£+ÏÛQn2úZòu¯¢éÜQßmÁ%sR|6h‹‘w¾	ôàøHôeª&XbŸ0X†¤èÃUz<‰QÑâàŠŒ3Ž¶²Ïë¬[{–¹RÅ€
Hv5z®JB0RHþ€$¡_¾¦ÀnËzTú]^ÉVÈƒ.g ?®Ø•½ÐcO©®]4ý™0šåÎ»²šÀÄîô}ŠÌÏ}÷xqn«vD¹—ïÆðyRÝêé·¥UòÊ øõ:õÊ×­âY°ljuYe”j`„ $k”±D‘Ó¦ÞÜT¨×¯+Ä©jìz½®KP¸À†¬m¦ÑvÂ	piä3v×ß®†Ö"e…y‚Å‘gž…d¢´JDØ‚¦fê¸·óaœ|4ŸG’û$ÏD$©f¯Od6g²ê»û=-ü“î]Éø¢C¸Ä·“Õñz29H»˜!ŒÓV=ŠŽvi:’Œðh15µ×˜þ4&Dz–Q˜3X‹bx&Ž'²½fºŠ”%Ä+6yÐ£ß¼0ôŽn<øòGˆÆÒêÈÃùBDˆõ¢W*d|áÙdÉn¦¿w`ª™æÉÖ—Ü”ºÖãª‘lV^÷Så&Â¬B'ƒ²x·ü ÝAz¤ŽRÒ”¤kT€«¤SäqÛ^Ñ"Pdõ‚Ê«»vŸan4¨s°\Òœ•×¤D– ûT9˜þò8Á7_¦¥lz=çÙQnàçº³ÖÉè€\oS¦£ªLö!cèìÖ0–«âB~8”÷m •£µ81ð9Ñ®çèè¹¸.º&¼»¤¶ÓjMO¬Qí%úu›©Ö4×H*Y‹Qx®÷`GÎHaô#³ƒ¿‘Y¢H½Ë­V£Ò5z¸Mg®Ö&IÊÄ¾íŠ‚§«îw·rO¸ïÃYèó=ëHÕ~&âî+n5«4ß>ÅÌõp^u¢÷Ûë:d5ö8«BŒY
¢h÷ÅÿèŒ¬ÌªNu/¼¬¹¹ÃR\öÂ£¸â¸ìíh‰^)Â±j^"¥kYnC(éBE’›·„‡°Ò¿åCs™M8t™ Jð¨0ÚTyÒ¦ŒÀËû±Jô‚.qÔMÈ«g)Ý¬ä’u“<€YØƒb¹ÀäÕ'YnZšå¼È¿¹‡O¨8^`fÂÿöŸP^<Æ¼¹Í	Ñ_ÕÍU"b]ÖùMRô”S·àÚ:ÂÀ 	..-AŽ´=¬OâI$©Ñ~H	FI½AT…œAn‰mø‘µŽr—vïïâRÙ~„i.ÅGã¸È†-Ñˆ#wË·ŒH>Ôk9c!QHdœ¾vÜéÔøM RÁgu]X\äcg gÏ¼vtŽ·èg‡ŠÃš FÀ AÇöÓÖOªh`äIŒÛŠw¤o&ûÓý+MõàrD5Ñg€¤¬ha»æØŸÁ_:^ý'¶¦•SŒÓêœÑ*”±’W)Öx¦\\ÑÒ†0ô;2á]ï¤ë¼YÈŠdPùMDÏöLS´£þýa~ïeCëo¦õ·Š´¥@]²
P§Ü!PUñÚ¾0Ý%îùï#®BØK!£Ás ¹9FKÈ`K`jŸZª%H§4åˆñãŽ"¾\ÖÖt?†³Sâj7%¿DÁÊQ7°=§¡†€9Ž(´…›)Ítjö.¶ 87ÕLÕ¬¿${Iµµ?×tÚÄ)ðe0¤ì\¦;Gìš•màg¦Œi~y°vréI-«“=å°ŽÀ[,!oÙÚ6­¾¯Ý¯:gÎÖ+7ÜB£°T¥oŠŸ! Ñœj Ñ–Ÿã”ø0)P&íÜ>'¾}ù<ÿ®9âåÁo×Éhrvc/ª MeLÇ„'˜ÔÓe=Ñó®¿æ´K5š}ÊœÍˆ´ÆÈUo9­±{×£A*@¼™ƒÝºÐÞÞ“!<;¿GPÙŽ	R n=Ý)»ùj°DQ§'èˆ‘v;«cdÆŒM¢Ôä1ªWÚ†«>0‡)×Þ
yÔ5/º`ÁøõË´ÖT"¥:F¨
þûvÈ.›¡ÖÿÆésÉð³×D5ßQ«ò[ˆQíÙsÍÙÒ?ù|¼QŠ5™¡eÜð‚]ò¦ó~šÅ4rÒ`Õ
€2¥\.S9â—¿5Ébª£’¶,ÉãS¨êx#%ÎvZuù¿+éÇ	$	ImVàY¬6„—zÓCtê¡*‚h êÉü)lÇÐuP!L¿…ËÅT~~ÜÈ2XÚŽa:€êÊÞ}îMËvtÆÔi×N‘×ü¶à8ˆšÍmÃŒ°¿ä²ÔÞ‹*À„ã‰ÛùAU¾i3õÃÑ-ØŸ³ÕZg‡a°Ÿ(gË•hJ¼ $ËuÍŸÖj"CZ`kù¦ÌÍó°ˆp6¼QTÅ}	@¼«ì­aªëîœâQ”•³S*‡õø„ÆÐS¹ºzœ•åj…A?ñÒºÒÓî Y´ËR^lí=Í˜f‡Ù}Agw«/5…Û@ª1¥Ö°¿®yöëG^R
èÁË(ŒRÝ`5žÜÎgk6sž:>Zzú-—½™Ú+ÄòÎ!Š„ô&¦Búó`,©óq¼|i2l*n©«òÕõŠ[Ôí1ÅU%Z‚‚÷l0Åáò q‚N][k8“œ}L®mÚJˆbûã-#©chiY± <%8âÒ(Àý¡‰ûrÐ÷ á3±Î;'ïêû¸¶‚'$ ô]>í:ÿÉ×L&~#ZÑ'±jÜ^W9°•'ÒQ¼Ð¢|ŠŒ— †Ñ¨pƒs—k°0±’CÎZø+‘D¯,EUü„iw>Ý©í»&ùü^\¾¯Ø“˜™‘p½Üd´[++€ìçuY´1ëfQƒåˆá§HüŽg'ö³~³Ò~i³ëö¢+XIZýü¼#~™¶;Þ=àTLˆ½•ä[Î¹ÉZÅEw^jzâZ4_ÖmØDÈ¬ãú®êm‹sÃMO%Ü( µ€Ÿ–Ÿ“®á£;h
y°–švÔ&Š¢È‘–²)ngýÑØ3÷£ó³hYÊmºÉË´º³¸y@›K[ÝPDŒìS"j>¾6ÆÚàÇµh{üEfüŠðßÊåpeÒ‹}3tÞÜn~åÍ9êk9jD*ùå<¼_³÷¶ªû¦à>„s«GwÓ{„¶r2²TuôbR“]£ ªÊ¨|Ç}‰‡[#ÛEÊ¹²úÇg#å|&‘>£:g$ú|ñã¼Ñ'H¥tkÿOJÿ]FåÌ ‚¶›ä²þ8Ø)Ì’/ížIæ
Í©ç@¥š’™7´ZWvû.VtÎ€õB…n®O+	7âq¯xÝaÈ[ÖAKæÊÒô*Sè§2×ä/DtY¼^úà aòw8þÙ¬¶žFÙT¸l«²Ol¥lb]~	·[?˜?kþV~¥$öl:Ù Ôß±MsŽ<¨®ìÄ]t]¨/íJè\hhêÛ1IJá8#_ÓÿïõƒÞûþ›iÒAÌÆ@i.þVÑ;æd“ gž¬sÓLÀ%¾Ã·E,6S†qŠšŽRŠg]ð™‹õÛ•!.LŸÚò¹ìl©Ÿi^7Fï€ÝÄYYÞ›ó1N†×û`¥_®-î^•[
Bdßúo&æ‰Q]¾mŽf~ò¹­ë8óîïEâµÉØ˜/üJF˜·üé”ý•¢Ùø(¡LüØðê/ÝôIÛe™'FØÖ
ôY5å§.BÏ@ÿš®Ç1i%éioÖ,ß’=•ìêñ‰% Ç½è9?_ÀÙCx5vÒ^Ûu7%îA_òm·¬Ò*¹â¶£ˆŽîÃŽrŽGb¹„L 2ÔBALË÷tÍÒßGHuýê£ÙÛ—ˆ“&iGÁØ­Jë¥³×ñ€ãuŸyðº³Ù[×`ûk¡sPhŠ÷Kº¾"dîL6ç‰†³R¼ßVÀx<èj2Fzy¢è£·­·rÐgPç4É„v¬ê
t€ÀùxzaÄôwµMÁ$éïÄ¦k—{iÅ'”b¨ÅÖ½{Ð6aÓ&þèš§[8ôQ)ÓæFšO %ö(©ã[þB´ªû³ñã÷PµþíÖOÂˆFØ³ÒGÉ¥Žñ‚‰6UÅˆÔð ±6ä„a“'&GÇ†y;Âf(èø³Ê›Mˆ~ÏÔtZu:^.ÚŒµ¹ÔEäÚæíZWâÁœ¾¯Z±‚ØÝlÉ:Ò ´† ¿ÁÌ~dôoµÑÝÀàÁ8]Ê‡[}{ÔýýµFˆ\ùY¤ÄìÂ£ÚØÕjü}âRŸà®µ€ç#"ÎDüSÇÖ[d¢ýWIô5Ï†·ë+´n»s ™[©(m~µjó¨ÍváÕ
[gÈ:{òÿÊzî»Á(Å)°?nzqIîY{Ê†õövã	ÕâêY_Åj—¢ŸÐÚèöù)L öY®N-œÊ‚UÈ ,þïBÒ)Ž(„”ƒqî!ú{`Š†œj¼a(£6=D&áwl±v¼(M ³à–ð´ðAVÃ¡÷óæŸóûÄp0•·ob=/]¿ßþÞ‘¿3ép/»[‡\Ï30»üé-ŒÅº3Ç&ç`+(Ø8Üâé(t-Þà£';—S	Åp¥ÏÁ·kF,¨0…¡ˆkg|/)ˆtå]Y>úJ^„jšoù»‹Ô6W7Â	Å‡¨¶”0>~ÅæQU€kÎ%»£æA…,<`uÝK…í6¾Ê\N¦šÓÝð>ÿÆkX ¯ºÎf§Œž+f/¼›"®(~ð€íŸÜÈË™¦c¿@?s‘1, Ž."I™8'°ù5D¸¨NE±Ueú€b‰13™Oeø|Ù-4cÉ-•è­iãJËUmâ01Zs-î/=5Ñ Ð`ZŠ-YÒ•92R©•«ì„rèE[ˆ³&e'ÉÄ½æLÕ7®*±âÚQÒz¶^³z»o¡aqŽFÖ7æ=nÌJz[êhmØÙf«Ä–3:(!Ê¶é¢à„àˆ¼È_ÑA½«š'ÇÄ¤˜ã6…¿F{Øó|ÄA 
„]PÊF‡mî¥($¬TiÕz!©gxX«[Þ?ˆ<•AŠ	¾Ã‚ä"Üù	spXŸ´èå3ÎÅmÈºQíd) !¥}¦#ëƒ°¥h%Ìu]W‘¾áßÁˆÜ=ZG.Y®'hÍÝTÁÒMáSìèÂm“,H‘éåjW]—‰¾H/Ab×4ü‘1Óûé†2J½¿š}ág¬ÃÙ/ÝÄ—”æazo…2˜8<”¾VƒUþ1ì„-úM,¦"ÔÝ÷gë¤Ì›Z‰V‡6^¥ŸÚbó¥Ñ]†[âŽ^¯½±ko1©!:~ð„¸‚¤­×Ýæ>zAÚº;ÉŠ¦n»~t2íT²µZµ—]âms5© Qg#´)¬x:R<ül0Íç#ÚéÅý•K^$¯¶šÝEzùÃg,]ÀF:c‘•¶î©Uû³Y‚îÖËb®ìc³ºtâu‘Ü›B)œÈh½W¦ùƒ€œvš‰´ˆZA$Ð8{šïèfe’èöPûðWÊå(!±UÕÐ«EHÏBñ*®Ól(þ%Xè¨ÉÏŠ}üÝ„…^§ôÅçÓ+ê“2mFD
åÚW»ò¹8ê";®ï#Û ;¤ó|òõLR
\¼OVÞ¿Î+¤Ñ¯Pó‘ZC×Â#{Ê¬¿©;j¿÷Ê> ~g÷ˆÛ„Ç‹~yPèg˜áz^’ŽŒÒãfØj$¯‚–˜ãaµ‚â|‡:l0
û„s3Ñ÷íŽ06:AçHÓCu|SSà¶ÒÖÍ_í`â	Q`uh-MA;aØ…íAS?dåúHäBµì@^yšiž†mÙþïBÀXÑ“þ¬!e$aßKô·G<{íuÖºmž‚(^'>¥ïÁ!ÀÁ,ƒžªúBµ(cÕ·tÝ•ù¦·™dù›¥’pÀSêé]èá­Ñ‚àƒÊÁþø¯–¦ò™ü&J^ÊÓd¤eÁ1¯nÍW#Ö¯?á—ÂMÎÕ¨\*o?îß]èÙ>TºöØµÃ@–„‡uPjýDC¶IiÓ\rÓ«gÀ-}©nUƒl‚Žó>TH¼b'!óhëðTœëJðEbÀœ1’Äõmå*ˆVÈ (ó2GÅŠ¿VÓ êýñ3»Hä ‚EB!uH5Å!=‰>ÃË¤¥™cŒ9«5éõGñ“ŒÊ¹ôN­EøâË¤ À·£qsòv‹5Y'¥ü&£ÅÇ8{áœÂ¬Š±ºÊ•!B1ëàgòlpXNLM{©
”äØÁ§úryño"_a¦4ê®1ú-£Ã]Y ÿñs6û§üøÒ7—œG›oµôÜ’Â3iÕ>>ËÐ¾Ù•Í–t¥êÅ†Õ`³ÛTLÿ–Hü=:F‡EÒ¨Î£Ñû=1·–¼)KîâxIpŽÍñÏÕ;wË2fÓ_ðÞ^ÿ-ëU#­€ÈðXõ9W ¡é(´
;¨’
½Ä#!¨šWÕ\~A£ËA¬y6ÛóÐ
!É/_:‡?ãª£èR!›OI5í¬R\ëë•Ñ6éQ÷fÙ÷Ã£T+ïÿøº$“RYdúDÎµúìÁ«†PuäëØíÕ­®¾Ú>í'ªFÒã—¤z÷  ·6h3^òc9#tÚ¦÷Ì½#FEh—YlB5™ºa°|ºdHò©é%üˆ…tn(ªL]²E&ÃyÔµÔäŸç—æRçé<€›Á]®AƒLœîÓ²{"°ˆ]:l¿£H\©<›­}š»Ý KwŠw†äIvg&_ý¦=ú˜eIJ6¿sïÏý;u‚”ð›ƒæ>¨¤uDŽŠÚW÷ *$˜„¾ˆTa’w¥²¯*–:åïßUŠªPÓ¼ÝvÜÙEEvÔ&õQ0]ø‡’y“6KLN“2¨úaFi#;ére®ÏßÙ­Þœéû0//Ü»ø5©Ÿ“
îFZ0#ÔR6Ò¡[ËŽê“Ûý;jGÆ2;?LËªê¨r¼¯0n€o{º0&3…[í˜ËÀ7¹Ûø0c­˜¡€ýÂÌÛPpûúYwì©Ø@îÁŽWþ«C²ét{tßžÉIeJ„e»‰Ø 9àp‰A V& 0–]¸m­Wíc~Mã'w3îAÊ;pú‰Â†²™Á\Àúºü™çýÜÄéOê?šÙû,ë	…)Ù4©ñ^=`=²°Ò~37_Dc	ëÅ~jháã'¥’m5°¨ÌBk™MR>Ýl®
Éž?ÚGÔ(T–›IìÌåqò?;¸!0æ ´‘Ç:‰ÝÒ:§hâe™+4'é‰F…'5;v?[H™åúò§¸&±£§kj¿²¨ÓÃA©ÐyÍÇ§JOùH€)ÕãAj‚ºiÂJÉËwíÐF%¶«[F ˆ}p;ª!a¢ó©HÂ;2~êTì5˜sâÆk:–œ¨G˜2öüs
6çûez—$FÃ­¦–§-’•Ýzi®íÀŸŠþÓ‚D`
;™j**¡(és7Óƒ6‡"ñe)SÊ
E!S/¢W‘*Œús}óÀ÷R’ù·§Lp¢$}ÏêªÖjßf}’Œ6½,£d<ÐzkP¸›1©œß»lJœ¼"ºŠÈEö­¹àq¿Ê‘	Kr‚Œ|Ó2Xm”Áp^ó~ÝBc,xF˜] |ñ „´—ÚEè.¤;+"í6ùbŒýhaó ñ—$–¨TlÖk¢\ÿ=ßCs‹Sb{™+ÏJòíGe#%uPBÒ«‡P¹'ä3P~vH>*%òwnÚÀ¼GDž2ƒûQ‰¶õÄ ×À×&lµigÓGŽÝÿÎø;Q}úË¥–!bÚF|Ø®<nÑ’UÝøèpì’™Ý±J`<(åsÍ3¤Çîk)îþ6D¢m”kNÔ¼@Ghçzø™\àõ%’DÒZæðDçdªŒSüù<ÜÆ@×ZV–ý	«OÑ´Æj¿9Z}®öÍ#ýu€fseFhŠ›lò¬”ºÜrË%Ó Ú=9Žo·"×_™Ç¡a±Ó>$ä1`-(Ik0£õÒè=ë¯ªìB›Äsg°ë.}5,Eï“DbÔÖ‡Ïvê~sž^T’û[Y}Á<¥…˜uðö-¢ßYOÞÆ@¾Íôa‡ÍýZ6¾WIªíÊdüá§,À€O5ñC¨‹«ûÐ­
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPTION) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  arch/x86/include/asm/preempt.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/asm-generic/percpu.h \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  include/linux/time64.h \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/vdso/math64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/COMPAT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
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
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/FLATMEM) \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/TRACEPOINTS) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
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
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/IOMMU_SVA) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/DEBUG_MUTEXES) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPUCOE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/NET_L3_MASTER_DEV) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/DECNET) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/MCTP) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/net_namespace.h \
    $(wildcard include/config/IEEE802154_6LOWPAN) \
    $(wildcard include/config/IP_SCTP) \
    $(wildcard include/config/NETFILTER) \
    $(wildcard include/config/WEXT_CORE) \
    $(wildcard include/config/MPLS) \
    $(wildcard include/config/CAN) \
    $(wildcard include/config/CRYPTO_USER) \
    $(wildcard include/config/SMC) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/NET_NS_REFCNT_TRACKER) \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/XFRM_STATISTICS) \
    $(wildcard include/config/TLS) \
  include/net/snmp.h \
  include/uapi/linux/snmp.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/IP_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_MROUTE) \
    $(wildcard include/config/IP_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_MULTIPATH) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/IPV6_MULTIPLE_TABLES) \
    $(wildcard include/config/IPV6_SUBTREES) \
    $(wildcard include/config/IPV6_MROUTE) \
    $(wildcard include/config/IPV6_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/NF_DEFRAG_IPV6) \
  include/net/dst_ops.h \
  include/uapi/linux/icmpv6.h \
  include/net/netns/nexthop.h \
  include/net/netns/ieee802154_6lowpan.h \
  include/net/netns/sctp.h \
  include/net/netns/netfilter.h \
    $(wildcard include/config/NETFILTER_FAMILY_ARP) \
    $(wildcard include/config/NETFILTER_FAMILY_BRIDGE) \
    $(wildcard include/config/NF_DEFRAG_IPV4) \
  include/linux/netfilter_defs.h \
  include/uapi/linux/netfilter.h \
  include/linux/in.h \
  include/uapi/linux/in.h \
  include/net/netns/conntrack.h \
    $(wildcard include/config/NF_CT_PROTO_DCCP) \
    $(wildcard include/config/NF_CT_PROTO_SCTP) \
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
  include/linux/ref_tracker.h \
    $(wildcard include/config/REF_TRACKER) \
  include/linux/seq_file_net.h \
  include/net/netprio_cgroup.h \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/SECURITY_NELF                      X)      4     (               èüÿÿÿWzV‹°<  ƒçü‹‰‹†€   ‰‚€   )ú1ÀŠ„   )ÖÁéó¥^_Ã´&    fèüÿÿÿ‹B…Àu$WV‹2ƒþw+iö˜   z¹!   1ÀÆ@  ó¥^_Ã¸êÿÿÿÃ´&    v ¸êÿÿÿëæ´&    fèüÿÿÿ‹A…ÀuÇA   1ÀÃ´&    v ¸êÿÿÿÃ´&    v èüÿÿÿ‰ÂÇA   ‰È‹Š<  ‹I‰H‹Š<  ‹I‰H‹Š<  ‹‰Œ   ‰H‹’<  ‹’ˆ   ‰P1ÀÃ´&    èüÿÿÿU¶éWV¶òS‰ÃƒÃpƒì‹¸”   Ç$    ‰é‰ò‰øèüÿÿÿ…Àˆüÿÿÿ1ÒƒÄ‰Ð[^_]Ã´&    ¶    èüÿÿÿVS‰Ã‹@{(	˜ uK°@ÿÿÿ¶K|º
   ‰ðè…ÿÿÿ…Àt	[^Ã¶    ¶K|º	   ‰ðèhÿÿÿ¶K|…Àuß‰ð[º   ^éRÿÿÿf¸êÿÿÿëÈ´&    fèüÿÿÿ€=     S‹X\…M   ‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[Ãt& èüÿÿÿ‰ÂÇA   ‰È‹Š<  ‹I‰H‹Š<  ‹I‰H‹Š<  ‹‰Œ   ‰H‹’<  ‹’ˆ   ‰P1ÀÃ´&    èüÿÿÿUWVS‹…Ûu5‰Åz1ö´&    v “D  ¹   ‰øèüÿÿÿ…ÀtƒÆÃ˜   ƒþuÜ¸êÿÿÿ[^_]Ãfiö˜   Ã@  ‰<  ‹žÄ  ¶€ú\uë×¶    ¶SƒÃ€ú\tÅ€{uî…Àuê¶K‰èè'þÿÿëÝt& èüÿÿÿUWVS‰Ãìˆ   ‹sXd¡    ‰„$„   1À…ö„Ä  ‹C‹P‹Rèüÿÿÿ%   =   …ž   C¹À  º`  èüÿÿÿ‰Å…À„¤   ‰°0  ¹@   ‰Ú¾   Ç€<  @  èüÿÿÿ‹C·Sˆ  QRRÿ°   ‹CTÿ0h0   èüÿÿÿ‹½”   ƒÄë´&    v ¸
   èüÿÿÿƒî„Þ  1Ò‰øèüÿÿÿ‰Ã…ÀxÞéh   »ûÿÿÿ‹„$„   d+    uÄˆ   ‰Ø[^_]Ã»ôÿÿÿëÜèüÿÿÿ´&    t& èüÿÿÿ98  tSUWV‰ÖS‰Ã@pƒìƒú‹x$Ç$    í‰D$ƒå‰éº   ‰øèüÿÿÿ…Àˆ  ‰³8  1ÒƒÄ‰Ð[^_]Ã´&    1ÀÃ´&    ¶    èüÿÿÿUW¿   V‰ÖSƒìÇ   ‹¨”   ‰$ët& ¸
   èüÿÿÿƒï„…  º7   ‰èèüÿÿÿ‰Ã…ÀxÛ‹$‰\$¿   ‹¨”   ë´&    ¸
   èüÿÿÿƒï„Á  º8   ‰èèüÿÿÿ‰Ã…ÀxÛ‰D$‹\$¿   ‹$‰\$‹¨”   ë´&    ¸
   èüÿÿÿƒï„Ü  º9   ‰èèüÿÿÿ‰Ã…ÀxÛ‰D$‹$‹¸”   ¸   ‰ý‰Çëv ¸
   èüÿÿÿƒï„÷  º:   ‰èèüÿÿÿ‰Ã…ÀxÛ¶|$‰Å‹\$ºî  ÁåÇ    1É‰ø¶Ûå   ÁÿÁàƒç%   	Ã¶D$	Å¸@  ë´&    ƒÁ˜   ‰=h  tW·’   9Úuäf;¸   uÛ·”   fƒúÿt9êwÊ·–   9êr¿´&    €=     …  1ÛƒÄ‰Ø[^_]Ã´&    ¶    ƒùuÔ„À…   »½ÿÿÿëÔ´&    ¶    èüÿÿÿWVS‰Óƒìd‹    ‰T$‰âèñýÿÿ…Àu6i$˜   {ƒçü²@  ‹’@  ‰‹–€   ‰“€   )û‹„   )ÞÁéó¥‹T$d+    uƒÄ[^_Ãèüÿÿÿ´&    ´&    èüÿÿÿVS‰Ãspƒìd¡    ‰D$1À‰â‰Øèlýÿÿ‹ƒ<  ÿpÿpVh¬   èüÿÿÿ‹D$ƒÄƒø…/  é”          Ð  ð              à                                                                                                       @      À     `  Ð     P  ð    €  p  ð      4%s: Write: retry ... %d
 3%s: TVP7002 write error %d
 No platform data
 6%s: Rev. %02x detected.
 3%s: TVP7002 read error %d
 7%s: Fail to set streaming
 7%s: detected timings: %d
 yes no 6%s: Streaming enabled: %s
 tvp7002 ‰D$ÿ4$Sh    èüÿÿÿ¸
   èüÿÿÿƒD$‹D$ƒÄ‹T$ƒø…0  R‰T$Sh   èüÿÿÿ‹T$ƒÄéE  ·@PCpPh    èüÿÿÿƒÄéâ  ¶ÀupPVhK   èüÿÿÿƒÄ€ûtVh\   èüÿÿÿXZ1Û1Ò¾@  €~u¶N…Ûu	‰èè  ‰Ã¶VƒÆ€ú\uÝ…ÛˆØ  ‹…0  º   ¶H¶@ÁáÁà	Á‰èƒÉ è  ‰Ã…ÀˆØ  ‹•0  ¶J¶BÁáÀ	Á¶º   	Á‰èƒÉè  ‰Ã…ÀˆØ  ‰â‰è‹µ<  ‰ç¹!   ó¥èL  ƒM\º   ‰èÇ…\     @  ÇE  èüÿÿÿ‰Ã…ÀˆØ  j µÀ   1Éº   ‰ðèüÿÿÿ¹	˜ º0  ‰ðj j j jj hÿ   j j èüÿÿÿ‹ü   ‰ulƒÄ$…Ût=‰ðèüÿÿÿéØ  ƒÃh9   SèüÿÿÿY[»êÿÿÿéØ  ƒÅpSUhf   èüÿÿÿƒÄéØ  ‰ðèüÿÿÿ‰èèüÿÿÿ‰Ã…Àu¯éØ  ‰D$ÿ4$ÿt$h    èüÿÿÿ¸
   èüÿÿÿƒD$‹D$ƒÄ‹T$ƒø…=  R‰T$ÿt$h   èüÿÿÿ‹T$ƒÄ€=     „[  ÿt$hƒ   èüÿÿÿXZ‹$é[  S‹D$ƒÀpPhf   èüÿÿÿƒÄé3  US‹D$»½ÿÿÿƒÀpPh€   èüÿÿÿƒÄé3  S‹D$ƒÀpPhf   èüÿÿÿƒÄé3  S‹D$ƒÀpPhf   èüÿÿÿƒÄé3  S‹D$ƒÀpPhf   èüÿÿÿƒÄé3  Q‹D$1ÛƒÀpPh    èüÿÿÿƒÄé3  iÀ˜   ÿ°H  ÿ°D  Vhô   èüÿÿÿƒÄƒ»8   ºÀ   ¸¼   DÂPVhÃ   èüÿÿÿƒÀ   ‰òèüÿÿÿƒÄ‹D$d+    uƒÄ1À[^ÃVhÐ   èüÿÿÿXZë¬èüÿÿÿ7%s: Removing tvp7002 adapteron address 0x%x
  6%s %d-%04x: tvp7002 found @ 0x%02x (%s)
  6%s: Unknown revision detected.
   7%s: detection failed: lpf = %x, cpl = %x
 6%s: Selected DV Timings: %ux%u
   6%s: Detected DV Timings: None
    6%s: Detected DV Timings: %ux%u
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                            tvp7002                                                                                    `                  À                          @                                                                         P      p                              ð                                                      `             Ð         ÷l    n   (   Ü                           ’                                                              À         î‡ ™     €  8        ÷l    X   ,   ”                        š                                                              €          eµ Í     €  8        ÷l      ,   ”                        ˜                                                                         eÙ õ        Ð         ÷l    ¸  (   Ü                                                                                         €         î£ ·     €  8          îÙ    X   ,   ”         $                  ’                                                              @         eZ f     Ð  à          Àü›       >   <   	                        Ð                                                                      ÿÿÿÿ    Ð  @          Àü›       @   D         '                  Ð                                                            À         qÿÿÿÿ                        {À˜@GABKCDE F-G 1Z2  \ÿ             g  @GABKCDE F-G 1Z2   \ÿ             ¥ ˜@ABŠCDEFG1Z2  \ÿ             ‰€à@ABŠCDEFG1Z2  \ÿ             ‰€˜@ABŠCDEFG1Z2  \ÿ             6 @›A BC D E F-G 1\ÿ             5 @‘A BC DEFG1\ÿ              ÿ g  €2 ` 	 
 €€€ .]G  ÿ G  gw  !" #ÿ $ÿ %ÿ &€'ÿ (S)*+ ,P- .€/Œ01Z23`4ÿ56 7ÿ 8ÿ 9ÿ :ÿ ;ÿ <ÿ =>`?@A,BC,DE FG H I JãKLOMNÎOP«QóR STUUüVxWñXˆYþZ [\ÿ debug parm=debug:Debug level (0-2) parmtype=debug:bool license=GPL author=Santiago Nunez-Corrales <santiago.nunez@ridgerun.com> description=TI TVP7002 Video and Graphics Digitizer driver  X          ¤ÿ      GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                                                 7         @   G     8   @  (    H      &     _   À   I                                	 v     C     „       Q    	 —   `  g                   ¦   Ð  ,                µ   Q       	 É      I     à   P  ›     õ   ð        @           l   ¦   	 "  @      7  0       H    c     Y    w    	 o  €  ã    €  ‰  ª    	 –  p  r     ¯  ð  P     Â  3  y    	               Ú           î      €     ý      
           0                        0     -      P     ?  `   @     O  À  3     d  €  3     z     3       €  3     ¥  @  3     »     3     Î  À  3     á           ö                       X       /  1        F  =   =     \  z   ;     w             ‚             œ             ¤             ¹             Ö             í             ô                                       ,             A             Z             k             ‚             Ÿ             ±             º             Ò             í             
                          ,           8             G      
     V      0     y              tvp7002.c tvp7002_g_dv_timings tvp7002_enum_dv_timings tvp7002_timings tvp7002_enum_mbus_code tvp7002_get_pad_format tvp7002_write tvp7002_write.cold tvp7002_s_ctrl tvp7002_remove tvp7002_remove.cold tvp7002_set_pad_format tvp7002_s_dv_timings tvp7002_probe tvp7002_ops tvp7002_probe.cold tvp7002_init_default tvp7002_ctrl_ops tvp7002_s_stream tvp7002_s_stream.cold tvp7002_query_dv tvp7002_query_dv.cold tvp7002_query_dv_timings tvp7002_log_status tvp7002_log_status.cold tvp7002_driver_init tvp7002_driver tvp7002_driver_exit tvp7002_id tvp7002_core_ops tvp7002_video_ops tvp7002_pad_ops tvp7002_parms_720P60 tvp7002_parms_1080I60 tvp7002_parms_1080I50 tvp7002_parms_720P50 tvp7002_parms_1080P60 tvp7002_parms_480P tvp7002_parms_576P __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ i2c_smbus_write_byte_data _printk msleep_interruptible v4l2_async_unregister_subdev v4l2_ctrl_handler_free memcmp __stack_chk_guard __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init i2c_smbus_read_byte_data __stack_chk_fail media_entity_pads_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std _dev_err v4l2_ctrl_handler_setup v4l2_async_register_subdev v4l2_ctrl_handler_log_status __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tvp7002_id_device_table param_ops_bool    8  A   8  g   &  ‘   8  Á   8    8  ;  9  a  8  Ñ  8  ×    é  <  ô  =    8  Q  8  r  &  ~  >  ¨  &  ´  &  ñ  8    ?  %  @  G  A  \  &  m  &  r  B  ’    —  :  ¶  ;  È  C  æ  ?    D    8  K  9    8  ®  ;  Ã  C  î  ;    C  6  ;  K  C  v  ;  ‹  C  Ó  &  ë  &  *    K    q  8  €  ?  ¢  &  ¨  &  Ð  ?  Þ  D  ñ  8    ?  "    '  :  C    â        ¿    Ó    S    ·    ÷    ?        1    X    7    <                 
  h   &                                                 $     (     ,     0     4     8   !  	   
     :     ;  <   
  A   :  [     `   :  u   
  z   :  ˆ        :  ˜   &  ­     è         =    d  E  „  F  Ž  &  ¨  G  ¿  =  Ì  
  Ò  H  è  
  í  :  ü  I    J    
  #  :  -  ;  T  
  Y  :  f    v  
  {  :  “  
  ˜  :  ´    ¹  :  Ï  
  Ô  :  ê  
  ï  :    
  
  :  "  
  '  :  ;  &  A  &  G    L  :  [  
  `  
  j  
  o  :  |  K  Š  ?  š    Ÿ  :  ¨  D  1     M     h     Ã     ò     %    n    Ä    Þ    õ        F    m    …         Á    Ü    ÷        /       8          L     M          O  @   &  L   &  \   &  d     p     t     Œ     È     Ø     Ü     à          8    Ä  &  \  &  ô  &  Œ  &  $  &  ¼  &  T  &      &     L     R        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   @                    	   @       <"  0              )             €  €                   %   	   @       l$                  /                                  8                <                  4   	   @       Œ$  x               E      2       <  è                 X             $	  ¬                 T   	   @       %  €     	         g      2       Ð                  z             æ                    v   	   @       „'                  ‰             ú  
                  …   	   @       ¤'                 ˜                ^                  ”   	   @       ´'                                ~  µ                  ­             4                    ©   	   @       T(                  µ      0       H                   ¾              [                     Î             \  (                                „  0     8         	              ´  ˆ                               t(  á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  _RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/UPROBES) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/rcupdate.h \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/bottom_half.h \
  include/linux/rcutree.h \
  include/linux/wait.h \
  include/linux/spinlock.h \
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
  include/uapi/linux/wait.h \
  include/linux/refcount.h \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/rhashtable-types.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
  include/linux/osq_lock.h \
  include/linux/debug_locks.h \
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
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/nodemask.h \
    $(wildcard include/config/HIGHMEM) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/mm_types_task.h \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_ETWORK) \
  include/linux/security.h \
    $(wildcard include/config/SECURITY_INFINIBAND) \
    $(wildcard include/config/SECURITY_NETWORK_XFRM) \
    $(wildcard include/config/SECURITY_PATH) \
    $(wildcard include/config/SECURITYFS) \
  include/linux/kernel_read_file.h \
  include/uapi/linux/netlink.h \
  include/uapi/linux/netdevice.h \
  include/linux/if_ether.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
  include/uapi/linux/if_bonding.h \
  include/uapi/linux/pkt_cls.h \
  include/uapi/linux/pkt_sched.h \
  include/linux/hashtable.h \
  include/linux/etherdevice.h \
  include/linux/crc32.h \
  include/linux/bitrev.h \
    $(wildcard include/config/HAVE_ARCH_BITREVERSE) \
  arch/x86/include/generated/asm/unaligned.h \
  include/asm-generic/unaligned.h \
  include/linux/unaligned/packed_struct.h \
  include/linux/rtnetlink.h \
    $(wildcard include/config/NET_INGRESS) \
    $(wildcard include/config/NET_EGRESS) \
  include/uapi/linux/rtnetlink.h \
  include/uapi/linux/if_addr.h \
  include/uapi/linux/if_vlan.h \
  include/crypto/sha1.h \
  include/net/sch_generic.h \
  include/uapi/linux/pkt_cls.h \
  include/net/gen_stats.h \
  include/uapi/linux/gen_stats.h \
  include/net/rtnetlink.h \
  include/net/netlink.h \
  include/net/flow_offload.h \
  include/uapi/linux/filter.h \
  include/linux/bpf-cgroup.h \
  include/net/sock.h \
    $(wildcard include/config/SOCK_RX_QUEUE_MAPPING) \
    $(wildcard include/config/SOCK_VALIDATE_XMIT) \
  include/linux/page_counter.h \
  include/linux/memcontrol.h \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/rculist_nulls.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/indirect_call_wrapper.h \
  include/net/dst.h \
  include/net/neighbour.h \
  include/net/tcp_states.h \
  include/uapi/linux/net_tstamp.h \
  include/net/l3mdev.h \
  include/net/fib_rules.h \
  include/uapi/linux/fib_rules.h \
  include/net/fib_notifier.h \
  include/net/bpf_sk_storage.h \
  include/uapi/linux/sock_diag.h \
  include/uapi/linux/btf.h \
  include/linux/bpf_local_storage.h \
  kernel/bpf/../cgroup/cgroup-internal.h \
  include/linux/fs_parser.h \
    $(wildcard include/config/VALIDATE_FS_PARSER) \
  include/linux/fs_context.h \

kernel/bpf/cgroup.o: $(deps_kernel/bpf/cgroup.o)

$(deps_kernel/bpf/cgroup.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    cmd_drivers/media/i2c/tvp7002.o := gcc -Wp,-MMD,drivers/media/i2c/.tvp7002.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"tvp7002"' -DKBUILD_MODNAME='"tvp7002"' -D__KBUILD_MODNAME=kmod_tvp7002 -c -o drivers/media/i2c/tvp7002.o drivers/media/i2c/tvp7002.c 

source_drivers/media/i2c/tvp7002.o := drivers/media/i2c/tvp7002.c

deps_drivers/media/i2c/tvp7002.o := \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/MEDIA_CONTROLLER) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/RETPOLINE) \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/delay.h \
  include/linux/math.h \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  include/linux/compiler_types.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/vdso/bits.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
    $(wildcard include/config/SMP) \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
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
  include/linux/sched.h \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/IOMMU_SVA) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/DEBUG_MUTEXES) \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/COMPAT) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/UPROBES) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/PREEMPTION) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
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
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/kern_levels.h \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/ratelimit_types.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/rcupdate.h \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/irqflags.h \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
    $(wildcard include/config/DEBUG_ENTRY) \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/processor-flags.h \
    $(wildcard include/config/VM86) \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
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
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/segment.h \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
    $(wildcard include/config/BUG) \
  include/linux/bug.h \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  include/linux/restart_block.h \
  include/linux/time64.h \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/vdso/math64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
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
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
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
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/TRACEPOINTS) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
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
  include/linux/rcutree.h \
  include/linux/wait.h \
  include/linux/spinlock.h \
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
  include/uapi/linux/wait.h \
  include/linux/refcount.h \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/rhashtable-types.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
  include/linux/osq_lock.h \
  include/linux/debug_locks.h \
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
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/nodemask.h \
    $(wildcard include/config/HIGHMEM) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/mm_types_task.h \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
    $(wildcard include/config/ACPI) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/ioport.h \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/irqhandler.h \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/gfp.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags-layout.h \
  include/generated/bounds.h \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/uprobes.h \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
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
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/fwnode.h \
  include/linux/resource_ext.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
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
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
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
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
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
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
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
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  include/linux/ctype.h \
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
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
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
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
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
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
    $(wildcard include/config/CGROUP_WRITEBACK) \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/list_lru.h \
  include/linux/shrinker.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
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
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
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
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
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
  include/linux/of_graph.h \
  include/uapi/linux/v4l2-dv-timings.h \
  include/media/i2c/tvp7002.h \
  include/media/v4l2-async.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
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
  include/media/v4l2-subdev.h \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
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
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \
  include/media/v4l2-fwnode.h \
  drivers/media/i2c/tvp7002_reg.h \

drivers/media/i2c/tvp7002.o: $(deps_drivers/media/i2c/tvp7002.o)

$(deps_drivers/media/i2c/tvp7002.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_kernel/bpf/cgroup.o := gcc -Wp,-MMD,kernel/bpf/.cgroup.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned    -DKBUILD_MODFILE='"kernel/bpf/cgroup"' -DKBUILD_BASENAME='"cgroup"' -DKBUILD_MODNAME='"cgroup"' -D__KBUILD_MODNAME=kmod_cgroup -c -o kernel/bpf/cgroup.o kernel/bpf/cgroup.c 

source_kernel/bpf/cgroup.o := kernel/bpf/cgroup.c

deps_kernel/bpf/cgroup.o := \
    $(wildcard include/config/NET) \
    $(wildcard include/config/INET) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/RETPOLINE) \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch¤$9_q-¥rz‹Âíºäâ$Xñzb!úø^ÆF»ð Q\Éyñ”n80ú¹Ã‚()bÚ¬$~^¹ª¼Ãyú\@W ¨27ó–Qí jž†¹;&ó”J¨x&:ÌŒÍ‹¤¿ˆñÈq71x`»hæøÞUKp;Þ¤°Igh d­ðcñÍý½áú2¹ÔÜ:\Kœ¡Y»¸;‡…­Úá9UÕÍQeIri“QPÇEJxjëZ•´J+¶^ïaµ¥©GôAÛ¡U`úR!¤d4­Õnw…U¡Î%™û?›c´òªïÝjxïkSnß÷dÅ«=¥©úSMs¥D¢¾‰ÉV|kåÔO6uÊ¼ÿº0áÚðËO…NZ§™áÃú‹N…q \ãÝÎësÃ…¤Ó§ÌEërDò~Û¤¡Þ›Þì£Ý+ÁY„—H´œªœ^¸Ó8“íû› þçU•«`©˜Ü*õð¼Mè¤!è„ü;‚½ƒ„vi‰,”Äœw‡×9{Ý’ÕµJEGpÑ
wLK;™eÒ–h9C…Þ£,ŽewPÌ•ËÖp°Î+¬Þ;Íå)z­) £T;=uTòiv:Ž(p–qâ$ç”£­/à/ÑÓI’ýo?÷ ¹Á*Þ`X/Ý]ßéó£Ü7š–5þ–¯\¶Š®Au»Øà©Xñq9†[œch)}˜„O¤å0¿
Z&äf0iúæZ|â;¯Pj®æø¶¤ðŒ¥Î\Á	 b†èê«5D}èá€r±æÀ³üÜH¾f‹‡€ØšpÝ	uÄ$¶,	z"Z†;ÖCt¹*wDä©3^0äÐ\J;ðGW«Ô5¦u¥~JÈúSô§5··çcÁ{lò…—ªÎ¾©m‡¥ƒ?¿üÆƒ,[æ‹_”™1“D’çþÈOëJ§?z §UÒ;0\ó™‘ãíZ>ÛCŽNƒ§‘ü:Ú°•tm0`µ) Np9+awk­aÏY3~NÀüe8¨Iç[SÙ+^§¼C@áÇ|ˆÁ…eb‘'²‡«†Æ¯·9˜|áËÎz¢.jŸ’>é¹L 3Y¼¿Nløå¶ìŒa½ ?Ôu	Ô![„‘1ÅíVèT-½Ï|ê¤lyw4MÒÑ$â6ß_DÔ&‡Ê±“‰pWØKo•èÒ_õÅýÈÿ¿";Ýã¾ý-J †á;:n5ä·k÷æ~ÃGøå°ž>à°‚ÄçàÄÍ¹Cêm”Fð¸g–‘ÿÔ·µoŽéÍtÐYjäâ<ãJ_sB)SÌ€×ë§é:Þôâé+ŠWàa÷P{¹…æmþºž›ºw&Í’»Ìˆ˜èZêùuõà-ÂÆ3°G,ÿ=VZeÎIðÏ›”x•'ÈÌ®©A“—!¦ö«¾:<K$ŸÚq,W¯¼ Gö´šœ¦–.†å¯°#Â”­njOŸy$b.˜ÂK^`ý9ú=1r¼_WàöóïÄTšlbÑáÍ?¼•@Õ˜Ÿ \·ÙÆ“æ´Ç©lâ¿é¦0ŽFýIä/mrî»"ìà;4Ž¶•l3;z`š‘JÉ£ÃŠ¨¸Î‰ Ÿlž§l¤oS™«­¢ž„ÁœOÃî¹µî¹¸UÓaÜ¢Ëâ²CòJLø_½M4ÛßŒ‡qJœ`ƒ•ãÓ•‰YëÈ©Ñ_
)ÓéˆæS­·®ÌÕ‹áÛç±­Ékì„¿9œ$VÄå„fŠ•§è´Ö/9ý+É’öÙ‘pñÐô âmIí{ªßŸ*CßJKYÑK¿Xûƒˆ‰ ÷íXWz×lqÃßÒ:eGõ¹.Æwóü>’sE 8(ç·Œè¹G‡KŽ‹tã>ÇŒ8x9uõFšì‹xZÀî,›ž@køG'W<ÞÿXŒGšþ^KGYO†³Ü;nã8ÔÌÀN†Ì29àˆÞ£tŸ‘Qá‡‹ ó\MóïÞEQPî6ºPÆKð!IÃº"&Ó1kÖvÆå„ÉHžv"9Zy—>Á>ÞÍðwäNòe0Uƒ¢D®^î¾÷Þ]Üóé Óa*š¨´ç‘hCí‹{Í`°¨µ®)ÑWƒL»Dîc·tÒû¼0l€ñ,ZoB…ìjgzÃ•×«z+.<õœ@#rxD_;‡6Ë`ežPäg%Ì€.3mŽ`µd›¢§œƒ2¦ì8›²	ÂB±“bÍx¶Ii3ÉÍß]]â(ïs·=(z>+#´æWPFHüz‰ÞŸì„rV<ŽØ®2 uq0Îõ~ONŸ¢¤+ã¶¼z¨P†/|Zƒ±¸±h¾¡¸áQ‹ËA±lB¤}H\ZóŸË‹Pî¢?gN,{¨èF¦-	-“„€1­L ~Xh üaO)FW{B2y²êí°&e….SSÛjÞUXž„Ð6lö:·ì£#.ü%xf7Íh³ä>$™¾zª¬€c5×$	»*ã”Ž“ñ¤f|§=<¿e‡øØ”ÎNý6¢‚oo±‘…z¦oDå¥&ÁAO+Ýèñ	Hü³\Z¥xÎZ;pW€Ñ"ðž+Ê®;—ø·UÇïŒÛÚÙª•*å9n€dÕnU0\.#Ïä²t’=A7V!¾&K¦³KM®(d2Z×Þ„Oœy€P#Y©dm(„ôSô,‡ï;Ššrï4[:|·)m{`(tžYû]r…Ì}8V
&ã¥_–úLvÎÒ¨§ŠBl9-b_îzY’^-ie\GL7†…ÙŸ*,’!•³Ùc‹Õ‰tþˆ§«>CuSmŠ¬Wm³ÅØg™›A„Š—u#3=jÔf¡ê(ký¾·¸Åä(G„æ›Äh5øüfRdŽYÕXRè[óšT	nOù¶–¤¼*I¯•áüB˜Õ4úúõF1l’ÕCñgšàsRO¯…U¼òWê¸;/ô@Éó
Jy;ƒ2Žß„£ô/Á‚‹“a8lj¾Ëè:ÓÃ‚™†öj 0Ñ”˜ÔãrFYÜ¯»ýÊ®ðÀ<1SÈ±F¸<™Ü"¨YæjÄ ªžV9ÐaH	TYâxÝ ø&wWªÒE¡ñSyü¯Ð¶å« Æ(ž>FÖ]O¿×pâ}¥[g”É¹3¤t1ÁÎnŒÖ<ñD‡>ÇšÎ¡aç@lœNÓ¨@Dc‡ìÊçŽÌòû4Ø¤‡
NaY-7@Z(ùôÃsº÷HÄy¢%”52¿¹µ–0òzîGŒc#ñ{Ø®y ¶[²‡ÛY±©Žì<mšÞïkãÐ~¹šÓ¶†¸_®–‰rÊ¶µâIØkñ~nÂ~ñSÿõöÆÖÈŽÉi3¦íŠ­ùï(7t”4îT¬ÆDÝ "RÃ¨4CˆwÛ-£„“ëom=Î‘ìúýÆ™êï‰igaMgz©u>UEÆ¸RqHºyP]k«Dô¡æ¥‰—Ÿ›7Ÿ;,3²À÷Ú”J¾%é+`§òžsÌÈ3µéXtn¥Ø}_s®;Ï'á]Ì%H> ÓŸ–D±iW¸`:}XG–™LîMœ/:Ç|œe5
ï¤åC#î³¢ê-”Ý›Fo„FqDDiÜá¥c\$h»1®öÔw~U"ÑÄA¿Æ5–ç³ærÎE¯ñÙ$Ò»]#¦eõîÕÅùGÞ•®ù÷äÒžšû+Õ®ýÖ-zÎ°ÛÇâaJDûHXÀ&ó(‹	ŠÈÓ´Ìh!æ[ºŠ¼0µL‰ùóx09ý1Ÿ­':·3…·K¹ˆÃ»°t$Ã†Ÿ\àÂÉ¶WŠ£š%¦ÈkÍ×]-øeAÖC/¸g¬"v¦ÝvZ«sý¡O
iH-þß]•Ž½úU£Û„ÅVCT Hr:Ž"a÷ð)j×v™ˆ›¦[Ë=q—žJH¼qµ!:¶ùRkDdaçòøãCÈëO'Â^M¾½jÃ°é…å“C¯|%ÿœïA„óR}´üŽ¬7‡çšË hì2#,åÏž€Ìã˜»’Æ,µrÐY¡·Ôìýê÷Âgf9QGÌ9—à.Rˆcå°«8ÞÖƒª=ÝkÃ]§@)¼·”S"1äüF©å	Â‚]¾
œÇyþ¬Ð"UçÂDñ€ÆüH‡8ëú åAÆ²}$Œ½%¤%ñ‘_Q @äº;3ªpë—.ú=ö‘9ÚºîïoY‘FÓØOßbõý°à·®·wœç£‘¸ú´Û>d_º·žºb©‰®]}PZa œáÛ§­3FYž³Cé’:Ë]kÙ*²«ï¬CêÅØùþÓç:yŽIQsO¶m	ûµ(Elà¦n.,ß‚P7K—žór,½{i“út—!CMSS¢Q­Öy]ê€¡|Ëð@gÖè$9n¯ò+œì¥üà„°ÙÚô¿x„
ž£¼’¢ìUÄ8+?V¥eIŸ5E_‚]£¹óˆ+)ÛLÊÎ“/˜$(k–«ÔbàÊÒèÅ¶)Ã¡ÌüqÒ¨?Ùš»³Óé3óŒ-qµ2Ië>ëú3r3Þæ
#8¢$¼kÜZ@ÅæšIˆ/°û	1¡G+€î­Óx?¾C÷•Så¶üY&qV€S6q©e´%C‹nØZ{:¦´¬'ú •ª"èàIÊ°_l„åó€&‚>	x^*º7iÜ
TÂ¸à³×óÌjhtÇØGˆñ.°–9Äx H§w¬*ÖÊ\A–Ñ™¾N-<o0`+3ÿÍÅ—Ë!.¾}UŠ^jtJBéI[¢1¿ƒä­IA":$¨ÇôP4±YxØ2å¢Lîo]iI¨t8pÁÈwï+È$_'83¢jØÀÑÂpV=€ô§ë›Jô™\Ôheä‰óâ8	wø¦f'ú£; å§ËðÈ>Îr |DOÀaE#’H»ýàvAo°GÀu°ó8ÂaÚ¤Íþ…¶c8ÆE{5pœJœX  ð"³È[˜ý\ˆŒ9£|µØ¸'Ã-r€ëdR†ïÆí0‡¯Ê0³Ëœ;Ãr9Ÿ¦/,JxçûAÔ¹ÖRþµWpŸÎ±{ÈqõcŸÊ
ØØüxÀ~ä¶¬wÚ¶\Ô×A
)÷ûVá*Jv°üV­Þ?l¢,|i]ùŒn5öD“¿Opî¾g"…¹­ òd’ï¤Týˆ8;é)eŒMjÉÕºR®U:Ä)ž™X±5ÎIÈSfý÷PGJ7äscDÂf”ÚÍR¦3Ñ5F_öÎdUÇ[%Â¨*û‰(šØÉûÙ
Øw¿dX`Âò8¬W8}OqÌƒ¦Úå—Ýú°^OÁ¥Ö5mÆÑ4+F£´-Ë†%™»ôeoÙ\¢Ñ*ß,ÏDƒµÞÞZú…½#×É×êjÂV9ª ä=ýPèÞCéàB·>}|­©k¨9¾RH]OT)ª¤õBî[s‹ƒu3EÅçq\pš³ÛûåŠñ€¢unuÌ·‚¨–à"¡æ°Žc„DŽtVº;GüžŒåW[ïúAŒé›
±tNd³FXL[µUhå÷ÍÅãb„,’e\–ûE›À™•ä3&ôôÝãlÜß;·IÎ9º[ÅZ,ÝJB`öBª Í%—ØVQ¤æ«î{²¸÷·ö†ÓG‹MtxÌ‘7á-\³¼ÌÀdú*ËŽT±º>8\è{BôŠ’c¯úKÿW(bVÚ‡GJš˜U½åÂÙ°ß6Ù-Sž“l_JÚÛ¢ÕfÆÏHâÕ¡LFrqÍ"9ŒÖ÷@vÆ¨·ž"[†ÎŠŠ¯ˆ®#7(O›§v<U#‹^mÁ2-ß'ô©º(XÍ_›êÞfšË9ì2äÑØ¸!qyÁ¦ó¬¢Dc6>XË!o€ú}“ TØÕ q3™(kW6wkÀ¬@|Ø8]îs†ƒË%ýœîk¿Gste‚^æ`ÅX·MtÉ¿ëHMÒï§ü1 kIØg½¨9ÁÍ$«SbÀOÐs‚AÄm.!9Æ
eŽ^Å¥ÐÆ
9f¨ù¹<ýèL ŒkºÔû›P¼€Ë 2øž8°I*áåYžýëVËóäT…‘añ‚Jõ²ýåùq¤mì†rõSïç£tÆ½lÿ“±ˆ…þ¥D55C”þ^/$‹•¨&)Èç#	’õPMyÃŽ…4ÆUgØY”’ÌÒ&s¾;¥]ñÏµ|×[rGŠ×¿6GtaDãØklÐ\ nƒ?:Y™ËB·Êee²{üöh.èùÖÒ9_žˆ]Š»'„Å²ñIJ˜úáWš$]Œ¼pSO<ä½qï—åŽÝ{+š ±:xúIÎfoZ•*ð¨‰ú×y^ùŽá`î¹Ïeù?©å{ÙøÜQ:™ÿñfMbûìcr©(i-ñ>™X»ÐjõJe Ÿž|ÝGW&Ô¯{i¶ž}skÑûa=bíŽ¯¨óûOÆôyË`oGlÊAìÖûõ~.,çP¯YþF–»œ÷Èž4>®1ìîÌð³[±Í ë|·¯fáÉŒëÀó<\ËªÃ»‡—aO‰ëí*Ã¬Œß8([ÖUmgòÂŽj·Ÿ¬^³Óãîù¯ŽgOéïõ×ðäÊÜwæ%Õòp˜·ºz9?F‘þ$·ŒãVß'<†ÓÆž·ËÅ´ŽÍròX¸
V&ÁHCø(ÃÍELK4|ÄŽ2Ñ4á*¥¿›s8æöàÖÁÜâ,9Qš-+1Š…¹Ø©èà“:Ûú#ñLÝÁ»N–}\ãgdÇ :‹Â­ÎKÅ’j Xs7ÞžšJê}³_;²B)O[w…"8àÂù\s_0À&d9ØCüÇfiJrO7b)K£àßõ’el,‚ùGÑy²”ç({[ÐÚ…’ŸçáÑ5çå×í+c{°‡-	?ä29Ôîg™¿ŒãªÞ7ÚãBb<ì‰žðô4¡lv‰”„ãNb’íËÐqðkÿ=nŽ Ü&<ð5‹¸3â,.Nxeýn·6•Ðªpm~iŒ.¼T¬ÔÚÛx²ËáEÏ ‹ºóÖN=AÈP‚eØù“«_“W\‘~SònÔ(Û¤ññê»(ó75RiWù8gâ³•ïâe>s'BG\a”¦šlÛ€"™å`V8‚Dƒ`¼¡W]i¿:wíåÏ=´Šk2ú³Ìº±Ò>Œ²°W@5ÝüÔZ‡‚ÑtºîJÄ”ïà‚Û›n's¥Foà*»Ýíg?¨§€lIè>ÕQ«£¢À÷Ý<+©öì×Ÿ	Cã²,½Þ92÷‡	_‘Vþ1åçÕ`’r<»TÙxÍÍ†‹Í+H$Wœ= Þ–P2û_Ú e‚ÅZøù*¾ab‡žö.éûD7¼¹¾™<ùïÎ¹ÔŽ^~gM)jŽBzJ°ÇI²lµ~nÔã•^ÔÛíð6èh~†ûoO­GþÛbJ6#
Ï'—cÝƒ–lÅ¡ ¸:>X¯™ÑÄïÜÊ=Þ)xÝÿ µrêÏmCfl>Të÷‘Á	fM½gëºQ¡÷J¡š13|"’§I˜@4÷B¼¯‰º©ˆ‡KcBcÄéÛ“†|ðFZ€/W”Ô( 8V«â\^l‚Æq]k·æ#nÉîîë—R÷Kqåä—Cü£éÚ­${>²ôhþ¯^,ÿ$ìphñ¿+à,¨´ñ(€yÕ=vÒÑ”å'žcnã1J0ìSbºÚ¤cÍU(Ë’¡½áè=(BÅ&ðœÅ‘u[Ò†ÑqÄñ˜)ùà_ú	Ws_,'ÍB¬RN±…Ó¥©f™¿óAð½[iéÕ'øÌ¦ì|_M‚W*7”&{'éŒ6š‡nËš‹	XŸ¹Þ’ I¨y¢@ªŒË{€¼Âs9HijQ«4§/”/«ß×¿™©§çw.¼Ðß·$vîµ‘û“CŒ²
Îùý‹6Q¦tfˆ©èKÈ²n°Q†…KŠÔâ¸¶Qõ´0PØh2ù]BÎ=aÚ>õªx¨Ùæ´œtÒtÅQ'UøÅ§{ZË©téeVoß±:PvúŽ%B©Ãž¬û´G¢þÑÚ\ú¸³%>:ŸŠ¯. Ð hgi!ª¯=Õ·«3X2ùWÓüZ…°)-_vYË­"t+ò~ŠÉ=Ð£l¢Ù4×o$‹{°½o²ŒO®rUíqTæðâ•GôÀ‚Ýð,EÕŒd4ø±J†ˆÏÞ{Wê ø¿Lµ´ë¸ìÁÈo¨¹‚ï˜r:ÿšýîEÞ‹lÎz¡¢j™ãÕ¤Í

¯Õâ}¿/ËÅ^Ž¢P´dØ×Se`}èŽsk×uùá‡½E7÷NhK«Ò¹Û–?¿ŒºökûžJ­Q‰¶Ñ4;ßCû°œèìuR.üYycˆ¯$¹>¸#paGM"ƒ¯“ã/A~H¸oz“Z /ÆOpƒTë-ËŒ+Ÿ'Æå‘=ðñ´¸¸ÚÒË¼&Ìã²ÊZš58H®\Âiž)7^Öži]lÐ(®¼QŽ1:(¼°¹ˆjGÉ›¸ÙžÓÔ›Œ(ÿÈáºãl›´¿¼ùI\º—Ñ¿[ àV'çDŠ­†yh×ó²±P_@8æ*T«ÐoFè’„¢¹ZÃdW¤{e©[‰%Ùq	!ZðßtÐ&©r	&T9jKÚ\€§Qÿd|î·k=Çv’ð-L±pÞl›ÚELžé®£|ãtÑâ@’?Á&po¤yøy»©ó:Ÿjt8Jj¿?È×:Û-´(ž'Èt•'W´ðU ¹{U©ãkÀ= íˆ0>Åµãt2 œ<¾Íû ÁÑVÕx`$Cñ•†*BÑ²ƒy^ìØjÔ{Ñæ”ìgÜlAU€Ù:»=Uš~8EŠ­ýÍ÷!ß‰ÕOÇÄ>±"ÃáKw£î¥?1ýÖÌ°M°XãAl}r3VÓlmñ;õ{Á¾¤Ð0^ÆÓNDbõ~ÖlÖ2‡ß¢5w}”w¹ûßaF[<8o¡Cqwû#bÙÿ|ë5‹¢Wÿq­Pï0F?>¡¬@ŸA_Â>Æ÷ñ*- ÅÈg®TC€;¼ºr¹]«˜’“÷óŠÑM6”xìtwîœ7;£–X’ëfÀ¡Ùîw„ÊÏÐ\[SåÞÍ~	õ$5° pI
i¦æ3ù,s´îJÀÕÝSHgÊ±ÆL	1V÷óÐŸ%]ÕÊœ$Ø@ÓëWÀf'Ed,Yç0¼rÌ%'Y±Áûþ4ÿ¼ÿCz×ÆƒÃç0‹ëX@øRKµ‰ƒ~ælF$e®M”C==Mo¯78ÝÍ“FÆ|Õé'Æ¨*óƒr;m hvMØ-Èÿ½SäÇzÀ%›@ìž0œW
‚ËÞ=pkÏ1|žJ‡]Ìë"²ô¡È…ác¨òøú°›>åáOêräU€”oß~ëmóµÉ’jnÉFÎ€È-µG}aqÝ’}ê$¢YhƒFi:‚J…9_,Æ<ÁôYÍ®%°~Û'C[s7?êVÖþÎO`ñ”œíkìÎ)@Ëº«Ï_ô>;mØ‰´W¶U}nº¹h{ŠoÙZÍ&ƒÇ5ü¦¸bz»¶ç‚#›ÙÖSjÖÔ(Z0_¸ž‘«3£bœ<¾NµDì2¬ª›Îw þ’Âu7Tp@mRÎ’ìôcÀþÁLÊTâ‚ñ5l„^3<8›ToÜ{j¯\î³<ïg¼4 ?VÔìþ\s}G/þ\^_Y¥Ü÷Þ„îK»6ÙLºÈ Pt—•ÁuGs¸§=Û`áñ×ó&¦xUtÍ³ìPÌ'™«~ª#ðãoN­?¡H"s<álµPtÁ¿çVe¶xü:Œ¬ûŸ\H†J€GÉUà5e0>:QµT¸{ÐÁÝˆX–5éj4E_ÏhûØ&Œµ¤!ä£X¨³'ßTÐï#ýÃì—bã<ƒˆ–twâV¶Hz›OK`ÔÍ%:=:D ®ïJ¬¼L»Ö%ø–ŒL´w¬Sì&(A58’SÎ®¿Ûq¹wu®#Þ¶od»ƒ»wü0Ãa¦˜Ú”j¦u46Áó^UZ°qÜ(‰²ê™±N+§š€-¾v\¿µ|hZä®4˜ÖÝ-¤.ê	!GŸv…P”«®vóÜ9ò‰ñÁ˜‹jdµ‘~ºFÜµ¡¥ZýåšÔ­ÞÕ._W¹3B’žLe>R§¢Jóf¦áN#ƒY„iêAjœïLmë¼N°ÆÞV©ÇÞs¾Lªµ³ŸžÄH:hReã-0éÇ2""®.¸lL¿ƒÅZg2¿ëdI0	h”i¢Dún O+g g\±K^Ÿ›Ñ3Ø) ¨ª!/»²ù#~ôéÆ ž=a~@C,öhú0[xY«S˜üƒ9þäf÷…I¼ÝøAÿe"Œ¸Rþ?|?Þ±Ñ‡Eé'LÉ{r‡’=iÔ_R[©¯e²…Çå÷*«X]ögÇmÞÆU¸¤¾l€h$L _xLª+§›/>Ÿ	›8»j;w?3ZŸl¦ï-xEXÒ¬×ö²…œÀž‹ÜäWyU¹"µ5ø+£yKvµ®¨ýËI]W}#
™Vk±g$ß“÷ÝÛ Àì(ÒÉÜôQ³Ó-pñ6Ùp@ÿ× ·£èÍÈ&;‡g•úSæY0ÆZ÷{{’ŒQû¦CÀo†êEtIX,bO Å|Ïù]‹ð{P[3"
¹ž¡- ÂèÒ¦ñöèÇ"^Q
râ< Ü«GÓ‚ýSoå~ó§$§Ö&¾Æ{’ÜŒhÀ¬+Kö1Fû¹†³S¥u<’5‰­â¿[ÿï1ZUï#ÿ±Å?"5‰aÕJÏ·	pøýq•Æd	faƒñ?ø°7”\á´VNùyÐèZÞ;"WæH“ÿãöt¬f=É#·9Ž"ôú’„×øFÇù-gcZXã£	ãÄÎúlbÄ®8¾‡--ú ¢%ÎŽ9È®ò«|Ô†1ˆmÍz¼MKÈ¶R XÚsâ´Ã`|9÷eÂˆæc	'føwÿiÏ‹•)á±ï¬Èb™Àz… ºæ¼h›H0$¿ƒù`ÆŒ‚Á›¿J[;%G­ZCXo‘³ yÑÉ'R5Zë£:ìJ5ýø[õÚ×FãíO÷M´=äÈÄ¹FâDº´ÒŒé2}wÄ‰ ÛÃEÆ!ŒŒ÷òô4:yù0 õ u„»Ëû c”¿¬SBiÿ°*ñdð_Vô¡•Nl…¹'„î¿°Ô_;öª)bRz`W´Íª‹}á	1±*©v-ßEN$€ŒŸÃˆÀÿ&€Å7ãŽ€¨Ïj‰PŽ3×qÂ¾wwD$Ržœ$«?Që…Ì´;·^·ˆù%è.lìÞJaâ²ý–LF}ãÎ€6A· u³×—ŸÆç^=™öª?QgÙ’LÊ7ÁjÒ9ÁF´n£50®k½«%åŸRÌ¬ì÷õ\[Ï/'«:`ÆµÑ—.±ËØ*¦ÒA€MÕÀ­o*YsíJß’£j{ìJ\“ƒbªˆ¼øTdæ`WÏìÌw›fÿ»µSú_š"à¦P¾4ë`ˆÚu §YÜæ˜Jäi‰ œƒNÞ6p7"ýÃ"xå®•‘g‚÷‘pùž­kÇ´ªO-´ßâ¾ýYâŸ3ê¶@<¢Mù¸†3öA›çì:ÐçR/ôTI–o]û¥ì‘âñ:ÔW8#“CÌTÇ¾ê„¿qŠÐeCœSÖ|c›ÇÐ£œ)¼~¢%yÆðŒÁÓsŠho¼€¯oäi£ßä(’‡$‹‡jÑ>,–³ÅXòHL.è}øùŠäWy"äx GO‚úÊ5ì`èé,Mi‹¥öD>Ï¹3¸²Ö¦È2¹ø;üÜXºzrèo²8Õ81ÊÊdÑ¢36äžGH^²K3£³¶¯öt¹_TeäJ;&½][•ˆË¾G¦éFŸ-I™Â›8FðŠ0qÊ­,‡8¸Œ„ºUB©u|ÔCßò!^m±TÞ­4üÚû”Ós&äöÝÁ‡+CÒƒÌ lÝ‰·àÝx?Ä–$vJ}µ¿µA8¸T6ÁÎçÚlóæDŠRöÁKnÍp…R—V¾\e"ÙýŽB)¹û__wÏ‡#³MuŸBà/‹oÔ­6³¬Ô=3oÙÑpSêÌâ­*3ÚéÌ˜S‹@óX¤O¡ë–¼hEqPrü]HøÿV—¨¾;Ä¨nüèBç*nÎˆ5gd}¤#¼O§Ù¹¥Òn»¢ƒþ¨,â»Ž:â?Ñ¨v«ŽîÌœr}1¥agC^¤S$£œ+öl)påSŽ[?”!}<ÄTÄÐ¤©É’ñ–‰µ2ƒ…‚o¢3r;¬æ7ú“à)¨’yÏ»“²\+Y(ï-,{Æ'@‘í±~:	,Æ—k TË]ëD@¥DØž¢vØ‘¥B63Èm×¿W'’2PÏš)^‰8H+Xá²¨N›ÊC2ÚÓ§ã»Ú¯e?åQm¬€,ü!¬$pI,‘JËô)Í–égò`²ÛËûò¿¥è¹x*4wXau>-0€üH±Bì•¥7Ôv<hë®e2ÃÉR¯bÀÊÔ®‹þk-?ÒÅëKR ê±%Ðß×¡ªÊ #­º±º)dõºy š‚S•±é	ÊÕÆˆ©{˜Œ‰®›è8^/Tk›_á3½/A‡–«¯q2šxi›¨¯R•Ð±äÁRf¶ÖøW¼´óÃw^ÃÝ÷,-`ÜCHk.æÛ}û$cÅ5Ž½8…[`“Ì¸¦!7uÿ‹„iG¢7âô£ùÂ—€[ÓÕçBˆqëBéG„=©ÀÌk¨;Éuo&Ú½Í=j>¹'QŸŠáL³áRhu Þþ"ôGe&3³ô0NÍÚ–€¸\jÅc°Ê¹ÔšžÐMd–ÇŒ&™ÐÕµÀß’©ÜÑöœ¿¬O)Ü`Ðd¥ÌÊ§$¬@”còv54U·ÁzõÀ£;*
ùá{ca—
ƒ{`IUu9ÖšðÃ©ýÄUo×7ÏcŸ¬þSœÖo®L+–üZ¡÷¹¶Ezú¨f¬›,ï©ÿ21eäÜËþTŒ5eK‚ÐŽµ;×0cá7ÝÉL}EôÂ¶PÛÙö×J:NÂFQt‹KRþè„…«¯ß€€ËŠÔµÍˆœóbÿ&yVcƒÑy1¶W´ƒXoøæ¿([¢ÌC]‘¸¿b‡rõyp]mÆ(pPµ3ê&dÕ¾ßÝ¯=Ñ#Ï®œíŽ—ìè¥)öUA_‹¸’ADL(‹ö;óa3?è¥Ü†§ s‚çû?†ÃŽü5Jfgïø\ª•gú¸>ícÓÞ¡0äï
rFp-“¾Wä&Ï”éG>,=®cq¯œ¶ïŒ“®¯‹š4bK×Î3!$à
‡wéF„vÑP[Cx>ü»Ò£_np+W¢_0±Ù~ýÂ<l¬€;hÔr(-ÇRÎÈþÐhZ0g0suÅÌêZüµ:W¿ÛñÐ>ãbûM»Cå"«¼Céò’ÕÒÄ"•ìÜ«{m1ÔÖEñ)×©aÀmÓ%Ý QáÉ_+ »±ð0`ë`s-¬+Eû´ràÀu±ø^É²S’,·¼™Æi¦ÄC%„p.»À@\@„·¶Xç÷
ùP—oürô ™Aö»+4`‘\Hƒµ
k²®ŒáÍ›å$×˜ëÞ37W ‚¡7‰+pEÉ}žþ¾ºÜE–Õý6{ŠRr&
}[×l²íþß=¡+4²‚E‹YBDú“FöX©ax¾¨qÎÒë£ô™~#$jí5a”fÝlÄRlßt…áA9ì¥ž ˜Sò¹(q‹
m¾jœ”À3Ð.)Š²œ†"ñõâZ¡>R ‡‘áãg<óÓ¶˜7°({œ5>$êmDy’Ð·È«„aåo?JÉ–jôþ”=@„ÍÐæ<Ü«Þg_V7vpÁ‚˜ëÜÀM2øùæ‚ÞDT·©Q™àô1 KE¤ï¦21«™1,ýónÝæt/Öaú¢çÐÜÞ°Ç:ÑY±\ÀÃÝöG™KzB* =0ÕÌ–\O²ß;ë›D|™ ûïæöP;.U­l±À´tÍuÆcìÍ !-ùW(ªúu’ñÓÂóehÀ™SGg@¬;ÿ€í>V@½$êå4wÙe
œ>Œ„¿ØK¢l“™U¶ãzi:fJ£@XKp]f??q‚²®¦s(ÎÛÇlOªÿãÌÚMŸÿÇïñ	ÜÈ;“|ÝbßªÒç‚îo­¸ÊÞ_9üÚŠ‹”×õ‡?ß/GýòŒÐ¨˜ptMYb15nÏß.Qò1áº³(R«!ÿËì8#·¢)Ñ|á£•GÚÊz / ˜¾3í†á¡ü „l…Ä
Â9ažeÐC‚O§}8]w[%cwŠçh½]àÊå`!ºèª-À<3L¨ävÒ½Á1@$	½cõÙýF	’r™è:7B
°
	x
ÚE)ªÓŠôøg	¶éÑ³K#Ç"`÷‚¿TƒhêŠMÄr’ªU|?¯.Iñì†ƒæ‘çÇ±ÓL‚eÊJî8\YÁi=yÁÌ¬|Åïx8næ
Îo¸æ³’v\S>¡fèR ®ð©îÎÏÁìó¤ï¾û«¾íK˜Ep»” Âü¢Ž?%gtÝG]$…uƒ¥Üö‘‘x"Z€„ÔRË¸}ON×ÆÅTí¦Q¬yƒ1áN«¦§ì"¦(0lËnüôŽ†'^eT–Œhªë|	þùóö”õ®×üÖ°D[Ì,¿C±*ßF.Ÿò2cu`(ØÅÛ)Ït\ae·Ê|¨ØG?&Ä—ã®	èŒBÈè0H*t@È‚h%bO¤"ïþ»™ÍÞþ•æZÄÈçÂÁKbÏ"j¤+B;j—u5ŸüÙ÷oëØTY¡¾í§2r
j¨÷wh i8lrî”¥ê‰¤g@vQ‡#´ÚštAñÇ£È˜ÍíZfu©mSªÓ_qT‘þ«oh@)<Ç•7^ÜÅ$ðïw.sÌÃ6}¤zW¢_î¦ÔðvD†§)ìÍ›bçä´U–ËÂ«È0Äà@ZÏ’qaúnÍ…¢l'™ŽàÓW$”8¹ÇË:£ÆJÿítÒÈÉ¼×Š`ö×L#
}¢rQÇ/º
ó|Ùœm@ß‘.;N>xášZINLÌéÔM€>Z‘*ƒpYqÓ o'ÞZ(ºôCã`IFµžÕ	`•iÖ±‚¾Ã5ñOhW#³IDßµ¸iOÔÇ­ÖOêzäÓœêÿõ9ˆõŸû›+Š,Ð…Ýûd|Z2]V	£µ~É©eñãÏ©(ÒæÂã­O¨íd¼¯
42
¹Â¾æÑ<Ñ9!°•[Í[í"ëšC6ÃN6Ôë-*ðŒ:4óC|ùHPe ÈÓs²rP‹tWè¥RA´—Ï‚3”§Üá<AÅ‰MZv	It’:ì‚˜3HãÖæH	 F¢û''µ'Æáx€ßííº7Þ#3§³Ô««$;–/Cd<nÒo˜ÑÅc2Æ>ÓÔe^l§Wd©yGwø~ÃÄãPÄÞUòñ¨—2“hpõ	¦ï]Ê€ò#À¡~[2Om=…­—wÃ„ë¶ëNuþþ@qéGË¡#ý”yHÁåÜA”b¼Ù^ÞÚP†PW´/}a(È}¾Ñ,›ÅNÊ‡ÌQjh?üZI_:Çá^=:Ÿÿ¼ý“éòW•[@vzX€°”ÇÖ‹%:J´0ÐZUˆ„Ë’“T|·†NË /<¹3e¾ÛÓæ¶ëÉ0ØJ§­ÇµQ>@î¤…t¾Í×½¹›ŠàäÎÁZÃo‰Õo¶|–-PQâçÍýn„iÔFN
Öh í‡!·Nì‚gÙµH¬ãŸ=+yuú^ÓêJL‰åÐŠ×¹¥Þ1´ÅT–<	¡áàßäŽyí¨ˆ9Ú¹wÁNö!v‘ƒ[Õlo_¸…óGhºòwggå¹—y¢Æó„Sýø¶6|Ë¸º/Òù¾ÑêGè¢ov¬!Ïyƒ|€J³³Är¹¦ÚrÕÊÖž^²ÁÝMåónQ ……Ë	ØU“‡hU‘èk`bó…Ñ­cs¹½Ê8Æ!êùÙÙjÑ—>¿!Z4{UƒrÎx¥õp(¶tº/WÒL ‹ ØÞ·Ã´o(i€Ü¿¸×ÝGlÝÛ¢Ù¼È3è}Ì.,;¶ûhÚý1¿ìw©eá=‰r¾¶ìz% Üú>vÛJa¦1î=S,¡<+Ìg?iX
oû€ˆJ´êáSo+,V†¡’ú'By'üæ±0»ÅÞª¾ëÇÃ_j’ð×²=¤ù3Ö1Ú6\7ËR6Ç®
6]þWA¹ž.š‡IAåÎqGÃµªý~Ë ž®¯‚Ôs)K
[%™¬Â¸ì^@ËíS3f”FfV[ùbc”¿ä6îR<hÏMlo3ÒÊ?AÿŠy3Í»k:"#÷—?âÐñ5xgÓqÏŒ.*õŸµ²5€¨p-ÅÑDp£Y•Ï—ÑUïº¾eÜ]r7ÕS	0û4Î¨9@[‡öÆoQ‰³è3t›#á‘¥D¡C=#ƒo]éºØ‹Wó1¥¼«<.óôqøV Õ:³Zâv÷\8Rp‰À»Ãæº¢é²Ç)p:šŸŒXj<&ãÚdÊ2Uœo¨Ó½Áu:(ÛfÔ„ò€±ôÖ³K;5¶Ê-„ÊãLÚyÿâm8ó[D|"»Ïò—í…ÙO×#¹÷öúøÆ)êÞížf°‚	¢B95ý}P±æm^ˆxãs"‘\ÐêPESzSJ¸™å‹mÐä¤º7é¨.B:ÏlÚ=[­­FÃ´ç§IK?ñP9 (#A…q~'§ñØÙÜ¸|3pžî\Ó¦&ÛL%`z(bÙ3(h« ãÝ*—Õs&K—xETÊ<˜Í~åôT bŸþdGr…šík¼€ñl¤‡~î8h½:z!¾)äM”(²Kf@Ü¦
[<ó¾vÛsZ¹ê=I²Vë)Ž¯1Ö ï/HÜZšØÕ8Ôè"–Xzn­_òþØõ)×ÔEOuÂ¸LÆ {Ý«‚æ ^±Q8¥ô|í$ÓNV­]ðô/¬@®éI[ŒËWfaw'Ýú_DÌõñ>ˆ-ÂÆÚhñ@(-y/¶üKÈgiÿn²ýø ƒÚÌñhÆNÈù^ZÒ‡—_Çk¢‹;»_Ã%ðl<·%Ðù²Ö3^tRM:pp}Öq„÷j™ÖïwFgÓxû²/)¤ó¶jú—ŸÞáàA¶åÖ"K¼4¶¾>]÷1žÔ¶n"àC¶–Ÿà¼dˆxHQ®b<½„÷;ž¡ª;m:slÎ«¤Jiù2Cv#Š8ü€>ÉˆŠPNâXúàŸKÕÖ¦]Qq™¼&¨<O¾Ž€tOú¢?c°% [=üM`m3Ùíþq467A~ôÆb57‘#Ñ,¡µôÂ;êiÇ‰!0ë+!¾Û,ÌM_¿¯ƒßjïuëY„KDVUR¹ÿãzßÝFü@
ÿ,	G[æÇ…:MÕ‚íÐ½³÷­˜§³'Ÿö‹3x|ÆÁ¹Páª +»–oÒÓŽœÍw_¸ðd|‘Mú
Ä.Þ¦¥Åˆ~7Õ•éÔ°Ùõó<¢ á5mf¦ýånßkx(š:7 ,áFÄîç 4oÁ¼;rUÁÃõ\Nø¨O³˜4˜TÄƒÑæ€1Ôºz®
1¤‰<`„§¹€xRJyÏQ™µºT‡‹]ûeCÂGT1•r=a¸âê-[.~I¬z—ñÜô†Åäz³Â%r CÝÑ^,îg3àZ-4^Áî.>qR²9š·hè„zšNäˆ¥mÄ µí6f!cÆüìeúCYƒÑÛ§…DPtºÍ6€ÞðÔ~F‘ %]hÛ!Ù´šC Ñ Ø­Géj‡¤èÎàæ!K¶.>¹›à¬ÅÓ@ì,²âŒí`†Â<V¯0Ì{rŽ/Sµ}ÇÌRW=,lRúr7»•ö6rT]HP±;Ô-J"êè)Ž½œaÌ$O#cÂÌq>ñœ~ÎmÍ<‰>à„&OŒé¾’ðË¹6ìÔÛH|NsYgþ"_ö‰ÿþ`[ZûÿÂ¶…+D‰É=cC	¿‹iÁ ÷–fhöÕöÐ®—¦³uoÁŽ¼ñÕKR)uóøM`ŒdD ˜ÕgRòÖOÜ„« ÙDAª‡å|>p¸MP¸`ñWü'—vðÕšç–Š	cR?eù£Þ' H è¿URíôñT%ð|je-˜ŒÁ»º`ß€ÂŽ!úÏ±¹;óuÆLç‘ŠPÍóe–ÃNŠßý–(f¡¼\hdé@u;×Íµðêñˆ€®*íšÖôè8£Zv÷Áu€;ˆ¡‚à]Ö_âìµ\ÓïYÏ1®ýÜ–~šjG8/î3æ¯ú½VÎ-#®€<zk©ÜCç¢Wûc­jl âDâÄap<Out gu%Pw_Htÿ¡‡Ÿß	¬$aðì²¢0Š“G¥µ,S~ñò7šRE4v Y£3=È’RlØc®}Z«xõ~¢Ymz`¦&Ø¢²™…pˆiâ'©« eÑÞCó5ËGLtké¾‡ÏbPžç*®BJÌË©Y3¢U¢Ä^’kShh½…â?ŽpG|*bJ>qøŸJ¨ÆÐkO:/Qÿ¦^©BIŒjmb€­ßŸôœ½,3Í¹Ù`ù# ,Ñî+]6
0ãÓ$Ž°«ËÂ^b_3.+u"Æ›M:KÓCâÝ´×Þyˆ|’éë ££ËmZŠSÇõ4rÖ¯_¤¨<~@Îb›X:  ÌÃ„·M«~Í¾äu;™ûm;³¦¢Ã5ZU&“PÁ:î@˜_ÑÚþ†kgQ!†,¨3Óò5„ Mýìx‹‰{FÁÝ‡O
$´1¨â‘=ñ§¿bÄHç÷Ž²>+¤hßˆë¨þ7!¤¡
°ÿÕãš•ŠCK½n9pTWB8$’m‘ùÓ>l|×}E>žjÉWDÚ÷kàNAüK¾Am
 [Ú- N'JE¿O	âZuÆÁ¢ÄÃ¡Æƒ×¡À,R—üò¹Šµz´è/0.ÔèöÓŠå†e%²7/=ÓÒŠwe 7LK_”aSÊ‡®ìS§LOËÇ¾ƒYV›ˆNGªlR´l[{-D^ý9rm™Ã½#Ý{ tí-âÇ¥¯µù9€½ØG}à*ËèÝ„VçDÞZáÉG§c·ø‘ÚïÚ·ëšÃŽwXAœ\áP·Î5êò!juýÓ0Á›¥µ¾šh3ÁÈ}Šf/sÒ.·Ä{ø…èHƒU”P uœb|†Ë£ÅHó¼s´¾c}ûúZ“è¬mæ«Ý¾==}>Y”Äî¥‹ŽNUìN˜@žô¦f¦Zlœ+Úâ·vÒ…QB’î'Õ¿Jgmj}JþŸ·Œh"%¿ÜœµPî	°ÞWh’Ä•5V[úCóXúä¼×O†K©šü7@Å½?øBºŠà²y€+ƒ1ÓÌðZ“BVoË{åžÚ*ÑP†ºöÜNÅa<WÚi¬%îÂŸ–h3þ'$ñHäNy·¯Ö‡.Á5=vq\>Û¦ˆX«Õï÷€=y£¨A~¢m0;8¢Uñ"æþ¬>´¿Úâ™à}»¹?§ˆŽµaŠÅÐ™šDZaexOZw~¾mG\ØcŒA´v¼äz†K~ Štžpõ¸Óðþ7¶WŽ´Õb¨Î’ÊÖ<õ‘òRrž´üô:ÐmÐôm[A`°ž•õ¬¡h4þŠ1-ahp^¼×)®”S‰‘*HÏ–#âÃv‚Ú1-ödfß©Â…ðvÓ›ÇÕUjz…¾c´$ÔåâÌq¹qôü¢prµÃs='èk£‚ËÊGf/´­ÊK{G…—ä„Ùh¡íuÝ¤TÕç‚zÃ½ÁAÃ?6Ý÷¡¡DVƒ²¬Zù…SÀÑí½x8Z m¤’¾FG&}ï½œ9|žÔV:®~ý—Ò\òUÊió¬´ß¥>yHu5(JVj|Ð4Ç9IÚ‹pÓbF¹{y;1k9õaa¯píH0H–hÄâZ²XFªiêú8È‡zl¾Üý2‰³ê¨ÀÏv4OÑéžš‚eÞð]“Dy3ƒžO{¾ð}€òLu×
C¨ƒ<}dÅbÊxÛ<pH–à[A*’‡NÍ/Ý¯\…SkG$I³ é2Òq­BŸŸÔYFà²,ØÅüh‚¤PÎ„&Z;¤¤Tk?Où.§h¸	¬ËØ“Zw´ÙÜŒ%(ZÂëÑÔ*ª7¹Õ
âø¼¨}·Óë,íÙÜÔt“2Bèomæîc†Š†f´¯­BX—o<(ó]öw `*|Ô1)FXy3'ð¯AÓ‚Û’–6°>É¥ùdÕX(!:Z±©#EEÌí*š;£\ý¾E¨XrŠD³$=l,Ô¼Ü¹Œ	è÷ví`°‰‡&÷ËÑÌ‰GÉÀ©[öa“Ž‘]LäN(V:‚ÿ
ÝúÕ6qöXÈÅ&j¬=µü Ë ƒí5Ì?ØF<¡öoh`$ï„çœjÿg&o¥¼—Ö(Èq€ÄÿYú4é8£d[X9Öµ¶ùò>[½Ö ÿí‡¯¹;ŒÐ¶0'þfîXºÔy w¾Zˆ î©ÕÈ6†gz1¿üÖÞÂµWDéqÇ¿;<R2@J)¡! .…†‡Ž-ÈÆÖý	¨HÂýÍZNAC•)–ç¼¢¿õCcUUë¾ô;m1	³aHˆà2ª–xDÈ^µ%^ƒÅª58Ö”XŸ¯I½íýõIèƒÅë ™"Aæèj÷«™¿2±0æk+àÂ<.5}|ÜÿuÑ<LÝÇJ0ÁÝ©gÕŒ¤)€I#süSq>Žî\Cs¯ã6q€ój‚àHWòY@¤RáD°Ä[{F‡„ äýF¨Ç,#n>¿JŸ—œ&Šÿëº§lèŒ¯E‹ž`¨/pŒ¡nŒ’!ÇÅçÐÀ$0p”GÆÜS`•ÿäÚŒ_á˜Õ‘ i1e^÷XÂãdaþÎ."ÄZî2|ïÈL‡$9‡gÇ6UÑÝS1@·íàœ³Ë–®ã*÷N/ÈÝâÛwsÇH#‚á#obýÇÒN½kÖZn/5´cÇãþ šÊàh_ÓîöUA:	Þ<ˆsðL‰
¿g™P¹‰¢|¥­ÁÈ£ªBÅ¨d{ÖÃëœÔVÙÇ PöÚ—aßh¯+‘‘3'È
±ˆ‹HÈ»¬¦V()Ù¦²GÓ¸£sÒ=ƒbßÊèÇáV8Ö+£_è´]-!X¾û±¸Býñ6JÇ€»¨QíÁÄ€Ç‹×‚WÃÅýº²œ ˜Ç°ê(üñ-ð_LOCK_DEPTH))
		return 0;

	class_idx = class - lock_classes;

	if (depth) { /* we're holding locks */
		hlock = curr->held_locks + depth - 1;
		if (hlock->class_idx == class_idx && nest_lock) {
			if (!references)
				references++;

			if (!hlock->references)
				hlock->references++;

			hlock->references += references;

			/* Overflow */
			if (DEBUG_LOCKS_WARN_ON(hlock->references < references))
				return 0;

			return 2;
		}
	}

	hlock = curr->held_locks + depth;
	/*
	 * Plain impossible, we just registered it and checked it weren't no
	 * NULL like.. I bet this mushroom I ate was good!
	 */
	if (DEBUG_LOCKS_WARN_ON(!class))
		return 0;
	hlock->class_idx = class_idx;
	hlock->acquire_ip = ip;
	hlock->instance = lock;
	hlock->nest_lock = nest_lock;
	hlock->irq_context = task_irq_context(curr);
	hlock->trylock = trylock;
	hlock->read = read;
	hlock->check = check;
	hlock->hardirqs_off = !!hardirqs_off;
	hlock->references = references;
#ifdef CONFIG_LOCK_STAT
	hlock->waittime_stamp = 0;
	hlock->holdtime_stamp = lockstat_clock();
#endif
	hlock->pin_count = pin_count;

	if (check_wait_context(curr, hlock))
		return 0;

	/* Initialize the lock usage bit */
	if (!mark_usage(curr, hlock, check))
		return 0;

	/*
	 * Calculate the chain hash: it's the combined hash of all the
	 * lock keys along the dependency chain. We save the hash value
	 * at every step so that we can get the current hash easily
	 * after unlock. The chain hash is then used to cache dependency
	 * results.
	 *
	 * The 'key ID' is what is the most compact key value to drive
	 * the hash, not class->key.
	 */
	/*
	 * Whoops, we did it again.. class_idx is invalid.
	 */
	if (DEBUG_LOCKS_WARN_ON(!test_bit(class_idx, lock_classes_in_use)))
		return 0;

	chain_key = curr->curr_chain_key;
	if (!depth) {
		/*
		 * How can we have a chain hash when we ain't got no keys?!
		 */
		if (DEBUG_LOCKS_WARN_ON(chain_key != INITIAL_CHAIN_KEY))
			return 0;
		chain_head = 1;
	}

	hlock->prev_chain_key = chain_key;
	if (separate_irq_context(curr, hlock)) {
		chain_key = INITIAL_CHAIN_KEY;
		chain_head = 1;
	}
	chain_key = iterate_chain_key(chain_key, hlock_id(hlock));

	if (nest_lock && !__lock_is_held(nest_lock, -1)) {
		print_lock_nested_lock_not_held(curr, hlock, ip);
		return 0;
	}

	if (!debug_locks_silent) {
		WARN_ON_ONCE(depth && !hlock_class(hlock - 1)->key);
		WARN_ON_ONCE(!hlock_class(hlock)->key);
	}

	if (!validate_chain(curr, hlock, chain_head, chain_key))
		return 0;

	curr->curr_chain_key = chain_key;
	curr->lockdep_depth++;
	check_chain_key(curr);
#ifdef CONFIG_DEBUG_LOCKDEP
	if (unlikely(!debug_locks))
		return 0;
#endif
	if (unlikely(curr->lockdep_depth >= MAX_LOCK_DEPTH)) {
		debug_locks_off();
		print_lockdep_off("BUG: MAX_LOCK_DEPTH too low!");
		printk(KERN_DEBUG "depth: %i  max: %lu!\n",
		       curr->lockdep_depth, MAX_LOCK_DEPTH);

		lockdep_print_held_locks(current);
		debug_show_all_locks();
		dump_stack();

		return 0;
	}

	if (unlikely(curr->lockdep_depth > max_lockdep_depth))
		max_lockdep_depth = curr->lockdep_depth;

	return 1;
}

static void print_unlock_imbalance_bug(struct task_struct *curr,
				       struct lockdep_map *lock,
				       unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================\n");
	pr_warn("WARNING: bad unlock balance detected!\n");
	print_kernel_ident();
	pr_warn("-------------------------------------\n");
	pr_warn("%s/%d is trying to release lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no more locks to release!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static noinstr int match_held_lock(const struct held_lock *hlock,
				   const struct lockdep_map *lock)
{
	if (hlock->instance == lock)
		return 1;

	if (hlock->references) {
		const struct lock_class *class = lock->class_cache[0];

		if (!class)
			class = look_up_lock_class(lock, 0);

		/*
		 * If look_up_lock_class() failed to find a class, we're trying
		 * to test if we hold a lock that has never yet been acquired.
		 * Clearly if the lock hasn't been acquired _ever_, we're not
		 * holding it either, so report failure.
		 */
		if (!class)
			return 0;

		/*
		 * References, but not a lock we're actually ref-counting?
		 * State got messed up, follow the sites that change ->references
		 * and try to make sense of it.
		 */
		if (DEBUG_LOCKS_WARN_ON(!hlock->nest_lock))
			return 0;

		if (hlock->class_idx == class - lock_classes)
			return 1;
	}

	return 0;
}

/* @depth must not be zero */
static struct held_lock *find_held_lock(struct task_struct *curr,
					struct lockdep_map *lock,
					unsigned int depth, int *idx)
{
	struct held_lock *ret, *hlock, *prev_hlock;
	int i;

	i = depth - 1;
	hlock = curr->held_locks + i;
	ret = hlock;
	if (match_held_lock(hlock, lock))
		goto out;

	ret = NULL;
	for (i--, prev_hlock = hlock--;
	     i >= 0;
	     i--, prev_hlock = hlock--) {
		/*
		 * We must not cross into another context:
		 */
		if (prev_hlock->irq_context != hlock->irq_context) {
			ret = NULL;
			break;
		}
		if (match_held_lock(hlock, lock)) {
			ret = hlock;
			break;
		}
	}

out:
	*idx = i;
	return ret;
}

static int reacquire_held_locks(struct task_struct *curr, unsigned int depth,
				int idx, unsigned int *merged)
{
	struct held_lock *hlock;
	int first_idx = idx;

	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return 0;

	for (hlock = curr->held_locks + idx; idx < depth; idx++, hlock++) {
		switch (__lock_acquire(hlock->instance,
				    hlock_class(hlock)->subclass,
				    hlock->trylock,
				    hlock->read, hlock->check,
				    hlock->hardirqs_off,
				    hlock->nest_lock, hlock->acquire_ip,
				    hlock->references, hlock->pin_count)) {
		case 0:
			return 1;
		case 1:
			break;
		case 2:
			*merged += (idx == first_idx);
			break;
		default:
			WARN_ON(1);
			return 0;
		}
	}
	return 0;
}

static int
__lock_set_class(struct lockdep_map *lock, const char *name,
		 struct lock_class_key *key, unsigned int subclass,
		 unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	struct lock_class *class;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	lockdep_init_map_waits(lock, name, key, 0,
			       lock->wait_type_inner,
			       lock->wait_type_outer);
	class = register_lock_class(lock, subclass, 0);
	hlock->class_idx = class - lock_classes;

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged))
		return 0;
	return 1;
}

static int __lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	WARN(hlock->read, "downgrading a read lock");
	hlock->read = 1;
	hlock->acquire_ip = ip;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/* Merging can't happen with unchanged classes.. */
	if (DEBUG_LOCKS_WARN_ON(merged))
		return 0;

	/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth))
		return 0;

	return 1;
}

/*
 * Remove the lock from the list of currently held locks - this gets
 * called on mutex_unlock()/spin_unlock*() (or on a failed
 * mutex_lock_interruptible()).
 */
static int
__lock_release(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 1;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * So we're all set to release this lock.. wait what lock? We don't
	 * own any locks, you've been drinking again?
	 */
	if (depth <= 0) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	/*
	 * Check whether the lock exists in the current stack
	 * of held locks:
	 */
	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	if (hlock->instance == lock)
		lock_release_holdtime(hlock);

	WARN(hlock->pin_count, "releasing a pinned lock\n");

	if (hlock->references) {
		hlock->references--;
		if (hlock->references) {
			/*
			 * We had, and after removing one, still have
			 * references, the current lock stack is still
			 * valid. We're done!
			 */
			return 1;
		}
	}

	/*
	 * We have the right lock to unlock, 'hlock' points to it.
	 * Now we remove it from the stack, and add back the other
	 * entries (if any), recalculating the hash along the way:
	 */

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	/*
	 * The most likely case is when the unlock is on the innermost
	 * lock. In this case, we are done!
	 */
	if (i == depth-1)
		return 1;

	if (reacquire_held_locks(curr, depth, i + 1, &merged))
		return 0;

	/*
	 * We had N bottles of beer on the wall, we drank one, but now
	 * there's not N-1 bottles of beer left on the wall...
	 * Pouring two of the bottles together is acceptable.
	 */
	DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged);

	/*
	 * Since reacquire_held_locks() would have called check_chain_key()
	 * indirectly via __lock_acquire(), we don't need to do it again
	 * on return.
	 */
	return 0;
}

static __always_inline
int __lock_is_held(const struct lockdep_map *lock, int read)
{
	struct task_struct *curr = current;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (read == -1 || !!hlock->read == read)
				return LOCK_STATE_HELD;

			return LOCK_STATE_NOT_HELD;
		}
	}

	return LOCK_STATE_NOT_HELD;
}

static struct pin_cookie __lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	st