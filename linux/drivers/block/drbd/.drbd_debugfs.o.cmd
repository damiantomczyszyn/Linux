ildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ELF                      X)      4     (               Ë¸ˇˇˇWçzVã∞<  ÉÁ¸ãâãÜÄ   âÇÄ   )˙1¿çäÑ   )÷¡ÈÛ•^_√ç¥&    fêË¸ˇˇˇãBÖ¿u$WVã2É˛w+iˆò   çzπ!   1¿Å∆@  Û•^_√∏Íˇˇˇ√ç¥&    çv ∏ÍˇˇˇÎÊç¥&    fêË¸ˇˇˇãAÖ¿u«A   1¿√ç¥&    çv ∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇâ¬«A   â»ãä<  ãIâHãä<  ãIâHãä<  ãâå   âHãí<  ãíà   âP1¿√ç¥&    Ë¸ˇˇˇU∂ÈWV∂ÚSâ√É√pÉÏã∏î   «$    âÈâÚâ¯Ë¸ˇˇˇÖ¿à¸ˇˇˇ1“Éƒâ–[^_]√ç¥&    ç∂    Ë¸ˇˇˇVSâ√ã@Å{(	ò uKç∞@ˇˇˇ∂K|∫
   âËÖˇˇˇÖ¿t	[^√ç∂    ∂K|∫	   âËhˇˇˇ∂K|Ö¿uﬂâ[∫   ^ÈRˇˇˇfê∏ÍˇˇˇÎ»ç¥&    fêË¸ˇˇˇÄ=     SãX\ÖM   âÿË¸ˇˇˇçÉ¿   Ë¸ˇˇˇ1¿[√çt& Ë¸ˇˇˇâ¬«A   â»ãä<  ãIâHãä<  ãIâHãä<  ãâå   âHãí<  ãíà   âP1¿√ç¥&    Ë¸ˇˇˇUWVSãÖ€u5â≈çz1ˆç¥&    çv çìD  π   â¯Ë¸ˇˇˇÖ¿tÉ∆Å√ò   É˛u‹∏Íˇˇˇ[^_]√fêiˆò   Å√@  âù<  ãûƒ  ∂Ä˙\uÎ◊ç∂    ∂SÉ√Ä˙\t≈Ä{uÓÖ¿uÍ∂KâËË'˛ˇˇÎ›çt& êË¸ˇˇˇUWVSâ√ÅÏà   ãsXd°    âÑ$Ñ   1¿ÖˆÑƒ  ãCãPãRË¸ˇˇˇ%   =   Öû   çCπ¿  ∫`  Ë¸ˇˇˇâ≈Ö¿Ñ§   â∞0  π@   â⁄æ   «Ä<  @  Ë¸ˇˇˇãC∑Sçà  QRRˇ∞   ãCTˇ0h0   Ë¸ˇˇˇãΩî   ÉƒÎç¥&    çv ∏
   Ë¸ˇˇˇÉÓÑﬁ  1“â¯Ë¸ˇˇˇâ√Ö¿xﬁÈh   ª˚ˇˇˇãÑ$Ñ   d+    uÅƒà   âÿ[^_]√ªÙˇˇˇÎ‹Ë¸ˇˇˇç¥&    çt& Ë¸ˇˇˇ9ê8  tSUWVâ÷Sâ√ç@pÉÏÉ˙ãx$«$    ÌâD$ÉÂâÈ∫   â¯Ë¸ˇˇˇÖ¿à  â≥8  1“Éƒâ–[^_]√ç¥&    1¿√ç¥&    ç∂    Ë¸ˇˇˇUWø   Vâ÷SÉÏ«   ã®î   â$Îçt& ∏
   Ë¸ˇˇˇÉÔÑÖ  ∫7   âËË¸ˇˇˇâ√Ö¿x€ã$â\$ø   ã®î   Îç¥&    ∏
   Ë¸ˇˇˇÉÔÑ¡  ∫8   âËË¸ˇˇˇâ√Ö¿x€âD$ã\$ø   ã$â\$ã®î   Îç¥&    ∏
   Ë¸ˇˇˇÉÔÑ‹  ∫9   âËË¸ˇˇˇâ√Ö¿x€âD$ã$ã∏î   ∏   â˝â«Îçv ∏
   Ë¸ˇˇˇÉÔÑ˜  ∫:   âËË¸ˇˇˇâ√Ö¿x€∂|$â≈ã\$∫Ó  ¡Â«    1…â¯∂€ÅÂ   ¡ˇ¡‡ÉÁ%   	√∂D$	≈∏@  Îç¥&    É¡ò   â=h  tW∑êí   9⁄u‰f;∏ê   u€∑êî   fÉ˙ˇt9Íw ∑êñ   9Írøç¥&    Ä=     Ö  1€Éƒâÿ[^_]√ç¥&    ∂    É˘u‘Ñ¿Ö†  ªΩˇˇˇÎ‘ç¥&    ç∂    Ë¸ˇˇˇWVSâ”ÉÏdã    âT$â‚ËÒ˝ˇˇÖ¿u6i$ò   ç{ÉÁ¸ç≤@  ãí@  âãñÄ   âìÄ   )˚çãÑ   )ﬁ¡ÈÛ•ãT$d+    uÉƒ[^_√Ë¸ˇˇˇç¥&    ç¥&    Ë¸ˇˇˇVSâ√çspÉÏd°    âD$1¿â‚âÿËl˝ˇˇãÉ<  ˇpˇpVh¨   Ë¸ˇˇˇãD$ÉƒÉ¯Ö/  Èî          –                ‡                                                                                                       @   ê   ¿     `  –     P      Ä  p        4%s: Write: retry ... %d
 3%s: TVP7002 write error %d
 No platform data
 6%s: Rev. %02x detected.
 3%s: TVP7002 read error %d
 7%s: Fail to set streaming
 7%s: detected timings: %d
 yes no 6%s: Streaming enabled: %s
 tvp7002 âD$ˇ4$Sh    Ë¸ˇˇˇ∏
   Ë¸ˇˇˇÉD$ãD$ÉƒãT$É¯Ö0  RâT$Sh   Ë¸ˇˇˇãT$ÉƒÈE  ∑@PçCpPh    Ë¸ˇˇˇÉƒÈ‚  ∂¿çupPVhK   Ë¸ˇˇˇÉƒÄ˚tVh\   Ë¸ˇˇˇXZ1€1“æ@  Ä~u∂NÖ€u	âËË  â√∂VÉ∆Ä˙\u›Ö€àÿ  ãÖ0  ∫   ∂H∂@¡·¡‡	¡âËÉ… Ë  â√Ö¿àÿ  ãï0  ∂J∂B¡·¿	¡∂∫   	¡âËÉ…Ë  â√Ö¿àÿ  â‚âËãµ<  âÁπ!   Û•ËL  ÉM\∫   âË«Ö\     çç@  «E  Ë¸ˇˇˇâ√Ö¿àÿ  j çµ¿   1…∫   âË¸ˇˇˇπ	ò ∫0  âj j j jj hˇ   j j Ë¸ˇˇˇãù¸   âulÉƒ$Ö€t=âË¸ˇˇˇÈÿ  É√h9   SË¸ˇˇˇY[ªÍˇˇˇÈÿ  É≈pSUhf   Ë¸ˇˇˇÉƒÈÿ  âË¸ˇˇˇâËË¸ˇˇˇâ√Ö¿uØÈÿ  âD$ˇ4$ˇt$h    Ë¸ˇˇˇ∏
   Ë¸ˇˇˇÉD$ãD$ÉƒãT$É¯Ö=  RâT$ˇt$h   Ë¸ˇˇˇãT$ÉƒÄ=     Ñ[  ˇt$hÉ   Ë¸ˇˇˇXZã$È[  SãD$É¿pPhf   Ë¸ˇˇˇÉƒÈ3  USãD$ªΩˇˇˇÉ¿pPhÄ   Ë¸ˇˇˇÉƒÈ3  SãD$É¿pPhf   Ë¸ˇˇˇÉƒÈ3  SãD$É¿pPhf   Ë¸ˇˇˇÉƒÈ3  SãD$É¿pPhf   Ë¸ˇˇˇÉƒÈ3  QãD$1€É¿pPh†   Ë¸ˇˇˇÉƒÈ3  i¿ò   ˇ∞H  ˇ∞D  VhÙ   Ë¸ˇˇˇÉƒÉª8   ∫¿   ∏º   D¬PVh√   Ë¸ˇˇˇçÉ¿   âÚË¸ˇˇˇÉƒãD$d+    uÉƒ1¿[^√Vh–   Ë¸ˇˇˇXZÎ¨Ë¸ˇˇˇ7%s: Removing tvp7002 adapteron address 0x%x
  6%s %d-%04x: tvp7002 found @ 0x%02x (%s)
  6%s: Unknown revision detected.
   7%s: detection failed: lpf = %x, cpl = %x
 6%s: Selected DV Timings: %ux%u
   6%s: Detected DV Timings: None
    6%s: Detected DV Timings: %ux%u
 Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇ                            tvp7002                                                                    †               `       ê           ¿                          @                                                                         P      p                                                                                    `             –         ˜l    n   (   ‹                           í                                                              ¿         Óá ô     Ä  8        ˜l    X   ,   î                        ö                                                              Ä          eµ Õ     Ä  8        ˜l      ,   î                        ò                                                                         eŸ ı        –         ˜l    ∏  (   ‹                           ê                                                              Ä         Ó£ ∑     Ä  8          ÓŸ    X   ,   î         $                  í                                                              @         eZ f     –  ‡          ¿¸õ       >   <   	                        –                                                                      ˇˇˇˇ    –  @          ¿¸õ       @   D         '                  –                                                            ¿         qˇˇˇˇ                        {¿ò@GABKCDE F-G 1Z2  \ˇ             g †@GABKCDE F-G 1Z2   \ˇ             • ò@ABäCDEFG1Z2  \ˇ             âÄ‡@ABäCDEFG1Z2  \ˇ             âÄò@ABäCDEFG1Z2  \ˇ             6 @õA BC D E F-G 1\ˇ             5†@ëA BC DEFG1\ˇ              ˇ g †Ä2 ` 	 
 ÄÄÄ .]G  ˇ G  gw  !" #ˇ $ˇ %ˇ &Ä'ˇ (S)*+ ,P- .Ä/å01Z23`4ˇ56 7ˇ 8ˇ 9ˇ :ˇ ;ˇ <ˇ =>`?@A,BC,DE FG H I J„KLOMNŒOP´QÛR STUU¸VxWÒXàY˛Z [\ˇ debug parm=debug:Debug level (0-2) parmtype=debug:bool license=GPL author=Santiago Nunez-Corrales <santiago.nunez@ridgerun.com> description=TI TVP7002 Video and Graphics Digitizer driver  X          §ˇ      GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                                                 7         @   G     8   @  (    H   ê   &     _   ¿   I                                	 v     C     Ñ       Q    	 ó   `  g                   ¶   –  ,                µ   Q       	 …      I     ‡   P  õ     ı           @           l   ¶   	 "  @      7  0       H    c     Y    w    	 o  Ä  „    Ä  â  ™    	 ñ  p  r     Ø    P     ¬  3  y    	               ⁄           Ó      Ä     ˝      
           0                        0     -  †   P     ?  `   @     O  ¿  3     d  Ä  3     z     3     ê  Ä  3     •  @  3     ª     3     Œ  ¿  3     ·           ˆ                       X       /  1        F  =   =     \  z   ;     w             Ç             ú             §             π             ÷             Ì             Ù                                       ,             A             Z             k             Ç             ü             ±             ∫             “             Ì             
                          ,           8             G      
     V      0     y              tvp7002.c tvp7002_g_dv_timings tvp7002_enum_dv_timings tvp7002_timings tvp7002_enum_mbus_code tvp7002_get_pad_format tvp7002_write tvp7002_write.cold tvp7002_s_ctrl tvp7002_remove tvp7002_remove.cold tvp7002_set_pad_format tvp7002_s_dv_timings tvp7002_probe tvp7002_ops tvp7002_probe.cold tvp7002_init_default tvp7002_ctrl_ops tvp7002_s_stream tvp7002_s_stream.cold tvp7002_query_dv tvp7002_query_dv.cold tvp7002_query_dv_timings tvp7002_log_status tvp7002_log_status.cold tvp7002_driver_init tvp7002_driver tvp7002_driver_exit tvp7002_id tvp7002_core_ops tvp7002_video_ops tvp7002_pad_ops tvp7002_parms_720P60 tvp7002_parms_1080I60 tvp7002_parms_1080I50 tvp7002_parms_720P50 tvp7002_parms_1080P60 tvp7002_parms_480P tvp7002_parms_576P __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ i2c_smbus_write_byte_data _printk msleep_interruptible v4l2_async_unregister_subdev v4l2_ctrl_handler_free memcmp __stack_chk_guard __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init i2c_smbus_read_byte_data __stack_chk_fail media_entity_pads_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std _dev_err v4l2_ctrl_handler_setup v4l2_async_register_subdev v4l2_ctrl_handler_log_status __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tvp7002_id_device_table param_ops_bool    8  A   8  g   &  ë   8  ¡   8    8  ;  9  a  8  —  8  ◊    È  <  Ù  =    8  Q  8  r  &  ~  >  ®  &  ¥  &  Ò  8    ?  %  @  G  A  \  &  m  &  r  B  í    ó  :  ∂  ;  »  C  Ê  ?    D    8  K  9  Å  8  Æ  ;  √  C  Ó  ;    C  6  ;  K  C  v  ;  ã  C  ”  &  Î  &  *    K    q  8  Ä  ?  ¢  &  ®  &  –  ?  ﬁ  D  Ò  8    ?  "    '  :  C    ‚        ø    ”    S    ∑    ˜    ?        1    X    7    <                 
  h   &                                                 $     (     ,     0     4     8   !  	   
     :     ;  <   
  A   :  [     `   :  u   
  z   :  à     ç   :  ò   &  ≠     Ë         =    d  E  Ñ  F  é  &  ®  G  ø  =  Ã  
  “  H  Ë  
  Ì  :  ¸  I    J    
  #  :  -  ;  T  
  Y  :  f    v  
  {  :  ì  
  ò  :  ¥    π  :  œ  
  ‘  :  Í  
  Ô  :    
  
  :  "  
  '  :  ;  &  A  &  G    L  :  [  
  `  
  j  
  o  :  |  K  ä  ?  ö    ü  :  ®  D  1     M     h     √     Ú     %    n    ƒ    ﬁ    ı        F    m    Ö    †    ¡    ‹    ˜        /       8          L     M          O  @   &  L   &  \   &  d     p     t     å     »     ÿ     ‹     ‡          8    ƒ  &  \  &  Ù  &  å  &  $  &  º  &  T  &      &     L     R        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   @                    	   @       <"  0              )             Ä  Ä                   %   	   @       l$                  /                                  8                <                  4   	   @       å$  x               E      2       <  Ë                 X             $	  ¨                 T   	   @       %  Ä     	         g      2       –                  z             Ê                    v   	   @       Ñ'                  â             ˙  
                  Ö   	   @       §'                 ò                ^                  î   	   @       ¥'  †               †             ~  µ                  ≠             4                    ©   	   @       T(                  µ      0       H                   æ              [                     Œ             \  (                                Ñ  0     8         	              ¥  à                               t(  ·                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ELF                      X)      4     (               Ë¸ˇˇˇWçzVã∞<  ÉÁ¸ãâãÜÄ   âÇÄ   )˙1¿çäÑ   )÷¡ÈÛ•^_√ç¥&    fêË¸ˇˇˇãBÖ¿u$WVã2É˛w+iˆò   çzπ!   1¿Å∆@  Û•^_√∏Íˇˇˇ√ç¥&    çv ∏ÍˇˇˇÎÊç¥&    fêË¸ˇˇˇãAÖ¿u«A   1¿√ç¥&    çv ∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇâ¬«A   â»ãä<  ãIâHãä<  ãIâHãä<  ãâå   âHãí<  ãíà   âP1¿√ç¥&    Ë¸ˇˇˇU∂ÈWV∂ÚSâ√É√pÉÏã∏î   «$    âÈâÚâ¯Ë¸ˇˇˇÖ¿à¸ˇˇˇ1“Éƒâ–[^_]√ç¥&    ç∂    Ë¸ˇˇˇVSâ√ã@Å{(	ò uKç∞@ˇˇˇ∂K|∫
   âËÖˇˇˇÖ¿t	[^√ç∂    ∂K|∫	   âËhˇˇˇ∂K|Ö¿uﬂâ[∫   ^ÈRˇˇˇfê∏ÍˇˇˇÎ»ç¥&    fêË¸ˇˇˇÄ=     SãX\ÖM   âÿË¸ˇˇˇçÉ¿   Ë¸ˇˇˇ1¿[√çt& Ë¸ˇˇˇâ¬«A   â»ãä<  ãIâHãä<  ãIâHãä<  ãâå   âHãí<  ãíà   âP1¿√ç¥&    Ë¸ˇˇˇUWVSãÖ€u5â≈çz1ˆç¥&    çv çìD  π   â¯Ë¸ˇˇˇÖ¿tÉ∆Å√ò   É˛u‹∏Íˇˇˇ[^_]√fêiˆò   Å√@  âù<  ãûƒ  ∂Ä˙\uÎ◊ç∂    ∂SÉ√Ä˙\t≈Ä{uÓÖ¿uÍ∂KâËË'˛ˇˇÎ›çt& êË¸ˇˇˇUWVSâ√ÅÏà   ãsXd°    âÑ$Ñ   1¿ÖˆÑƒ  ãCãPãRË¸ˇˇˇ%   =   Öû   çCπ¿  ∫`  Ë¸ˇˇˇâ≈Ö¿Ñ§   â∞0  π@   â⁄æ   «Ä<  @  Ë¸ˇˇˇãC∑Sçà  QRRˇ∞   ãCTˇ0h0   Ë¸ˇˇˇãΩî   ÉƒÎç¥&    çv ∏
   Ë¸ˇˇˇÉÓÑﬁ  1“â¯Ë¸ˇˇˇâ√Ö¿xﬁÈh   ª˚ˇˇˇãÑ$Ñ   d+    uÅƒà   âÿ[^_]√ªÙˇˇˇÎ‹Ë¸ˇˇˇç¥&    çt& Ë¸ˇˇˇ9ê8  tSUWVâ÷Sâ√ç@pÉÏÉ˙ãx$«$    ÌâD$ÉÂâÈ∫   â¯Ë¸ˇˇˇÖ¿à  â≥8  1“Éƒâ–[^_]√ç¥&    1¿√ç¥&    ç∂    Ë¸ˇˇˇUWø   Vâ÷SÉÏ«   ã®î   â$Îçt& ∏
   Ë¸ˇˇˇÉÔÑÖ  ∫7   âËË¸ˇˇˇâ√Ö¿x€ã$â\$ø   ã®î   Îç¥&    ∏
   Ë¸ˇˇˇÉÔÑ¡  ∫8   âËË¸ˇˇˇâ√Ö¿x€âD$ã\$ø   ã$â\$ã®î   Îç¥&    ∏
   Ë¸ˇˇˇÉÔÑ‹  ∫9   âËË¸ˇˇˇâ√Ö¿x€âD$ã$ã∏î   ∏   â˝â«Îçv ∏
   Ë¸ˇˇˇÉÔÑ˜  ∫:   âËË¸ˇˇˇâ√Ö¿x€∂|$â≈ã\$∫Ó  ¡Â«    1…â¯∂€ÅÂ   ¡ˇ¡‡ÉÁ%   	√∂D$	≈∏@  Îç¥&    É¡ò   â=h  tW∑êí   9⁄u‰f;∏ê   u€∑êî   fÉ˙ˇt9Íw ∑êñ   9Írøç¥&    Ä=     Ö  1€Éƒâÿ[^_]√ç¥&    ∂    É˘u‘Ñ¿Ö†  ªΩˇˇˇÎ‘ç¥&    ç∂    Ë¸ˇˇˇWVSâ”ÉÏdã    âT$â‚ËÒ˝ˇˇÖ¿u6i$ò   ç{ÉÁ¸ç≤@  ãí@  âãñÄ   âìÄ   )˚çãÑ   )ﬁ¡ÈÛ•ãT$d+    uÉƒ[^_√Ë¸ˇˇˇç¥&    ç¥&    Ë¸ˇˇˇVSâ√çspÉÏd°    âD$1¿â‚âÿËl˝ˇˇãÉ<  ˇpˇpVh¨   Ë¸ˇˇˇãD$ÉƒÉ¯Ö/  Èî          –                ‡                                                                                                       @   ê   ¿     `  –     P      Ä  p        4%s: Write: retry ... %d
 3%s: TVP7002 write error %d
 No platform data
 6%s: Rev. %02x detected.
 3%s: TVP7002 read error %d
 7%s: Fail to set streaming
 7%s: detected timings: %d
 yes no 6%s: Streaming enabled: %s
 tvp7002 âD$ˇ4$Sh    Ë¸ˇˇˇ∏
   Ë¸ˇˇˇÉD$ãD$ÉƒãT$É¯Ö0  RâT$Sh   Ë¸ˇˇˇãT$ÉƒÈE  ∑@PçCpPh    Ë¸ˇˇˇÉƒÈ‚  ∂¿çupPVhK   Ë¸ˇˇˇÉƒÄ˚tVh\   Ë¸ˇˇˇXZ1€1“æ@  Ä~u∂NÖ€u	âËË  â√∂VÉ∆Ä˙\u›Ö€àÿ  ãÖ0  ∫   ∂H∂@¡·¡‡	¡âËÉ… Ë  â√Ö¿àÿ  ãï0  ∂J∂B¡·¿	¡∂∫   	¡âËÉ…Ë  â√Ö¿àÿ  â‚âËãµ<  âÁπ!   Û•ËL  ÉM\∫   âË«Ö\     çç@  «E  Ë¸ˇˇˇâ√Ö¿àÿ  j çµ¿   1…∫   âË¸ˇˇˇπ	ò ∫0  âj j j jj hˇ   j j Ë¸ˇˇˇãù¸   âulÉƒ$Ö€t=âË¸ˇˇˇÈÿ  É√h9   SË¸ˇˇˇY[ªÍˇˇˇÈÿ  É≈pSUhf   Ë¸ˇˇˇÉƒÈÿ  âË¸ˇˇˇâËË¸ˇˇˇâ√Ö¿uØÈÿ  âD$ˇ4$ˇt$h    Ë¸ˇˇˇ∏
   Ë¸ˇˇˇÉD$ãD$ÉƒãT$É¯Ö=  RâT$ˇt$h   Ë¸ˇˇˇãT$ÉƒÄ=     Ñ[  ˇt$hÉ   Ë¸ˇˇˇXZã$È[  SãD$É¿pPhf   Ë¸ˇˇˇÉƒÈ3  USãD$ªΩˇˇˇÉ¿pPhÄ   Ë¸ˇˇˇÉƒÈ3  SãD$É¿pPhf   Ë¸ˇˇˇÉƒÈ3  SãD$É¿pPhf   Ë¸ˇˇˇÉƒÈ3  SãD$É¿pPhf   Ë¸ˇˇˇÉƒÈ3  QãD$1€É¿pPh†   Ë¸ˇˇˇÉƒÈ3  i¿ò   ˇ∞H  ˇ∞D  VhÙ   Ë¸ˇˇˇÉƒÉª8   ∫¿   ∏º   D¬PVh√   Ë¸ˇˇˇçÉ¿   âÚË¸ˇˇˇÉƒãD$d+    uÉƒ1¿[^√Vh–   Ë¸ˇˇˇXZÎ¨Ë¸ˇˇˇ7%s: Removing tvp7002 adapteron address 0x%x
  6%s %d-%04x: tvp7002 found @ 0x%02x (%s)
  6%s: Unknown revision detected.
   7%s: detection failed: lpf = %x, cpl = %x
 6%s: Selected DV Timings: %ux%u
   6%s: Detected DV Timings: None
    6%s: Detected DV Timings: %ux%u
 Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇ                            tvp7002                                                                    †               `       ê           ¿                          @                                                                         P      p                                                                                    `             –         ˜l    n   (   ‹                           í                                                              ¿         Óá ô     Ä  8        ˜l    X   ,   î                        ö                                                              Ä          eµ Õ     Ä  8        ˜l      ,   î                        ò                                                                         eŸ ı        –         ˜l    ∏  (   ‹                           ê                                                              Ä         Ó£ ∑     Ä  8          ÓŸ    X   ,   î         $                  í                                                              @         eZ f     –  ‡          ¿¸õ       >   <   	                        –                                                                      ˇˇˇˇ    –  @          ¿¸õ       @   D         '                  –                                                            ¿         qˇˇˇˇ                        {¿ò@GABKCDE F-G 1Z2  \ˇ             g †@GABKCDE F-G 1Z2   \ˇ             • ò@ABäCDEFG1Z2  \ˇ             âÄ‡@ABäCDEFG1Z2  \ˇ             âÄò@ABäCDEFG1Z2  \ˇ             6 @õA BC D E F-G 1\ˇ             5†@ëA BC DEFG1\ˇ              ˇ g †Ä2 ` 	 
 ÄÄÄ .]G  ˇ G  gw  !