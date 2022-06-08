ldcard include/config/X86_INTERNODE_CACHE_SHIFT) \
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
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/linux/of_graph.h \
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
  include/media/v4l2-fwnode.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \
  include/media/i2c/tvp514x.h \
  drivers/media/i2c/tvp514x_regs.h \

drivers/media/i2c/tvp514x.o: $(deps_drivers/media/i2c/tvp514x.o)

$(deps_drivers/media/i2c/tvp514x.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ELF                       j      4     (               Ë¸ˇˇˇSãòX  ãàT  1¿âZ[â
√çt& êË¸ˇˇˇãÄ|  «AA	  â1¿√ç¥&    fêË¸ˇˇˇãÖ“uãAÖ¿u«A   1¿√çt& ∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇÉ9w6Åy   u-ãêD  âQâQãêH  —ÍâQãÄH  —ËâA1¿√ç¥&    ê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇÅ˙å   vçäPˇˇˇÉ˘w ∏ ¡ ”ËÉ‡√çt& êÅ˙É   ó¿√ç∂    1¿√çt& êË¸ˇˇˇãP@ãÇP  Ö¿u√çt& êãÇL  j π@   ∫¡   j j j@Ë¸ˇˇˇÉƒ√ç¥&    Ë¸ˇˇˇãP@ãÇP  Ö¿u√çt& êãÇL  j π@   ∫¡   j j j Ë¸ˇˇˇÉƒ√ç¥&    Ë¸ˇˇˇÉÏãÄL  ∂“dã    âL$1…â·Ë¸ˇˇˇÖ¿I$ãT$d+    uÉƒ√Ë¸ˇˇˇç¥&    ç¥&    êË¸ˇˇˇUWVSâ√â–â â¡Äı@ÉÏ	—t}â¡Äı	—t<â∆â◊Å∆ ˛ˇˇÉ◊ˇÅÊˇ˝ˇˇâ˘	Òtkâ¡Å· ∞  uqÑ¿u}%  ˇ â«˜ﬂ…É·â»Îçv π   ∏   ã    Ö“è¸ˇˇˇãÉL  ∫(   Ë¸ˇˇˇ1¿Éƒ[^_]√ç∂    π
   ∏
   Î∆çt& π   ∏   Î∂çt& π   ∏   Î¶çt& π   ∏   Îñç¥&    çt& êË¸ˇˇˇWVSãòX  ã∞T  1À1÷	Ûtdã∞»  ãòƒ  ÖˆtDã{ãs!Œ!◊âÛ	˚tJâ”â∏T  Å„ ˘  â∞X  É˚€É„`Å√‡  âòH  [^_È¥˛ˇˇçt& øˇˇˇ 1ˆÎπç¥&    1¿[^_√∏ÍˇˇˇÎıçv Ë¸ˇˇˇÖ…tGÉyuASãòD  âYãêH  «A   —Í«A   âQ«A   ã    Ö…è!   1¿[√çt& ê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇãÄ†   ∫   Ë¸ˇˇˇ1¿√ç¥&    êË¸ˇˇˇWVSâ”ã    Ö“è›   ç{âŸ1¿«    ÉÁ¸«Cp    ∫»  )˘É¡t¡ÈÛ´fêÉ¬$Å˙ú  t/ãz¸ÖˇtÓããJâ˛9»wçv f	tCÉ¿9»vÙÉ¬$f	;Å˙ú  u—[1¿^_√Ë¸ˇˇˇSãÉ˚tÉ˚u[È¸ˇˇˇç¥&    ê[È¸ˇˇˇç¥&    çv ∏Íˇˇˇ[√ç¥&    fêË¸ˇˇˇã    Uâ≈WVSãx\Ö“è˙   ãá»  ç∑   1€Ö¿tJfêâÉ√Å∆å   Ë¸ˇˇˇãá»  9ÿwÊÖ¿t*ç∑@  1€ç¥&    çv âÉ√Å∆å   Ë¸ˇˇˇ9ü»  wËâ¯É≈Ë¸ˇˇˇçáÃ  Ë¸ˇˇˇâË∫   Ë¸ˇˇˇâË∫   Ë¸ˇˇˇ1¿[^_]√çt& Ë¸ˇˇˇVâ∆SÅzGREYâ”t<É{t1¿[^√êÉ{uÙãÜL  1…∫   Ë¸ˇˇˇãÜL  π   ∫   Ë¸ˇˇˇÎÀêãÄL  πp   ∫   Ë¸ˇˇˇÎ≠ç¥&    fêË¸ˇˇˇUâ≈WVSÉÏfÉ: â$ÑÊ   1€çv ã$1…fâLX2∑|XãÖT  â¬ÅÚˇˇˇ ïX  Ñ  ©ˇˇ çsDÛ1…çF˙âD$É¯wl∏»  1…É¿$∫ú  9¬t#ãP¸Ö“tÌÖ◊t	;0r;pvÉ¿$∫ú  É¡9¬u›ã|$âŒãÖL  ÅŒ   ˇâÒçW–∂“Ë¸ˇˇˇãÖL  çW—âÒ∂“Ë¸ˇˇˇâÒã$fâLXÉ√É˚Ö>ˇˇˇãÖL  π   ∫Õ   Ë¸ˇˇˇ1¿Éƒ[^_]√ãÄL  1…∫Õ   ª–   Ë¸ˇˇˇãÖL  1…∫œ   Ë¸ˇˇˇç∂    ãÖL  â⁄πˇ   É√Ë¸ˇˇˇÅ˚¸   u„Éƒ1¿[^_]√ç¥&    fêË¸ˇˇˇVSâ”ÉÏã≤L  ∫¿   d°    âD$1¿çL$«$    â«D$    Ë¸ˇˇˇãL$Ö…ukâ·∫á   âË¸ˇˇˇã$Ö¿u&ãD$d+    Ö∞   Éƒ∏   [^√ç¥&    çv çL$∫Ü   â«D$    Ë¸ˇˇˇãL$Ö…t∫∫   âË¸ˇˇˇÎ¨fê∫¿   âË¸ˇˇˇãD$®@tñã    %Ä   ïÉt  Ö“è7  âÿ∫@  Ë¸ˇˇˇ1¿Äªt   tãÉl  j π   ∫   j j PâË¸ˇˇˇÉƒÈ?ˇˇˇË¸ˇˇˇË¸ˇˇˇSãò†   ∫   âÿË¸ˇˇˇ1“Ö¿x
â–[√ç∂    â¬ãÉÙ   Ö¿tÍçHˇ±ãÙ   uÔâ–[√ç¥&    çt& Ë¸ˇˇˇVSâ√∑Äv  f=PQÑ∂   1ˆf=QQÑÇ   ãÉd  Ö¿tâÚÉŒÉ É¯DÚã    Ö“èd  ãÉL  1…∫   Ë¸ˇˇˇãÉL  âÒ1“Ë¸ˇˇˇ∫@   ∏   Éªd  E¬ãõL  j πP   j ∫   j PâÿË¸ˇˇˇÉƒ[^√çv fÅªx   Öoˇˇˇããp  1¿Ö…î¿ç4≈    ÈVˇˇˇ1ˆfÅªx   ÖEˇˇˇÎ‘ç¥&    ç∂    Ë¸ˇˇˇãP(ãHÅ˙	ò tmv;Å˙	ò Ñ   Å˙	ü Ö´   ã@|Ö¿î¿∂¿âÅ§   çÅ4˝ˇˇË«˛ˇˇ1¿√çt& Å˙ 	ò thÅ˙	ò uxãP|ãÅÄ   â—∫   Ë¸ˇˇˇ1¿√ç¥&    êãP|ãÅÄ   â—∫
   Ë¸ˇˇˇ1¿√ç¥&    êãP|ãÅÄ   â—∫   Ë¸ˇˇˇ1¿√ç¥&    êãP|ãÅÄ   â—∫	   Ë¸ˇˇˇ1¿√∏Íˇˇˇ√fêË¸ˇˇˇâêd  1“É˘ï¬âàh  âêp  Ë˛ˇˇ1¿√ç¥&    fêË¸ˇˇˇUâ≈W1ˇVç∞,     SÉÏãù»  âD$Ö€uÈZ  fêÉ«Å∆å   9Ω»  ÜC  i«å   1“ç^çN`ËãÄ,  9D¬ãV¸ã@«F|   â$∫   âD$1¿Öˇî¿âD$âÿË¸ˇˇˇÖ¿à≠   ãE`â⁄ã@Ë¸ˇˇˇÖ¿àò   ˇt$∑D$âÈ1“PâÿË¸ˇˇˇZYÖ¿x}É<$tOÖˇÖ_ˇˇˇãT$1¿Ö“ï¿â$ã$âÖd  âËË˝ˇˇãD$ãï8  ãç<  âÖƒ  âËË4˜ˇˇÈˇˇˇç¥&    ˇt$ãFâÈ1“∑@PâÿË¸ˇˇˇY[Ö¿xÖˇt¶ÈÚ˛ˇˇfêâ¬ãÖ»  Ö¿t.çµ@  1€â◊ç¥&    çv âÉ√Å∆å   Ë¸ˇˇˇ9ù»  wËâ˙Éƒâ–[^_]√çt& Éƒ1“[â–^_]√ç¥&    çt& êË¸ˇˇˇUWVSâ√ÉÏ∑Bfâ$ãBÉ‡Öã  â÷∑$âœÉÚ∑ âL$ãã»  Ö…Ñ~  ãoçì@  Îçv É¿Å¬å   9»Ñ_  9’uÎi¿å   ã    ãÑ(  Ö…èë  É¯Ö@  Ω   çì¿   â–â$Ë¸ˇˇˇÖ¿t!ã$Ë¸ˇˇˇÖ¿ÑX  «@$    ã@ «@$    çì‡   â–â$Ë¸ˇˇˇÖ¿t!ã$Ë¸ˇˇˇÖ¿Ñ#  «@$    ã@ «@$    ãD$$âÈÉ‡â¬ÉÚÑ…u~∑Nâìh  âãd  Ö¿t~«Ép     âÿË1˚ˇˇãì»  Ö“ttçGîãìT  âÉƒ  ãO¨ãÉT  ão∞1ˇ! 1–ÉX  â÷tIÖ“uâŒâÔâÚâ˘âÿË)ıˇˇ1¿Éƒ[^_]√ç¥&    «Éd     âìh  Ö¿u4«Ép      âÿË≥˙ˇˇ1¿Éƒ[^_]√ç¥&    fê°    Ö¿M1ÌÈæ˛ˇˇ«Ép     âÿË˙ˇˇãT$â¯É¬¡‚⁄Ë¸ˇˇˇÖ¿tãt$$âp$ã@ âp$È%ˇˇˇ∏ÍˇˇˇÎûÈµ  ÈW  çt& êË¸ˇˇˇUâ≈1…WVSª¿  ÉÏã∞L  1¿∂–â˜Îçt& ∂KãΩL  â¯É√Ë¸ˇˇˇ∂Ä˙ˇu‰ãïP  Ö“Ñ  1…∫   âË¸ˇˇˇπ   ∫¬   âË¸ˇˇˇπ   ∫   âË¸ˇˇˇãµL  1…∫œ   ª–   âË¸ˇˇˇçt& êâ⁄πˇ   âÉ√Ë¸ˇˇˇÅ˚¸   uÁª–  â,$ç{$Å˚Ä  tçt& ãGÙÖ¿u1â˚ç{$Å˚Ä  uÏã,$âËËR˘ˇˇçÖÃ  Ë¸ˇˇˇ1¿Éƒ[^_]√çt& ê∑o∫≈   âÉ√âÈf¡È∑…Ë¸ˇˇˇ∑Õ∫ƒ   âË¸ˇˇˇçt& ê∂K∫√   âÉ√Ë¸ˇˇˇ9˚uÈâ˚ÎÜπ   ∫   âË¸ˇˇˇπ   ∫¬   âË¸ˇˇˇ1…∫   âË¸ˇˇˇÈ˙˛ˇˇç¥&    êË¸ˇˇˇUâ≈WVS1€ÉÏâT$d°    âD$â–çxçH«@    ÉÁ¸«@^    1¿)˘É¡`¡ÈÛ´«D$    ÎCçv ãD$Ö¿àì  É‡É¯
ç¿4Öƒ  É«;<$uXâ∑ˆ	t$ãL$fâDYÉ√É˚t`ãÖT  â¬ÅÚˇˇˇ ïX  Ñ≥  ©ˇˇ çSD”1¿ÉÍÉ˙w¿çT–1ˆ∂˙çGâ$ãÖL  çL$â˙Ë¸ˇˇˇÖ¿âiˇˇˇÈì  çt& â»∑L$fâãD$d+    u
Éƒ1¿[^_]√Ë¸ˇˇˇç¥&    çv Ë¸ˇˇˇSâ”ÉÏãÄL  dã    âT$1“â·∫à   Ë¸ˇˇˇ∫    Ö¿I$É‡É¯∏ˇˇ  E¬âC<ãD$d+    uÉƒ1¿[√Ë¸ˇˇˇêË¸ˇˇˇVSâ√ÉÏã≥†   d°    âD$1¿Ö“Ñ¢   ∫   âË¸ˇˇˇâ¬Ö¿à$  π   ∫   æî  Îçv ∂NãÉL  É∆Ë¸ˇˇˇ∂Ä˙ˇuÊãìT  ããX  â–5ˇˇˇ 	»Ñ˚   âÿËˇˇãÉ|  É¯Ñï   É¯Ñ|   ãÉP  Ö¿ubã≥l  ∫@  âÿË¸ˇˇˇÎç∂    â∫   1ˆË¸ˇˇˇj ãÉL  ∫   π   j j VË¸ˇˇˇ1“ÉƒãD$d+    Öª   Éƒâ–[^√1ˆÄªt   tôÎëçv «Él  	   Èuˇˇˇêj ãÉL  π   ∫   j j j Ë¸ˇˇˇ«Él     ÉƒÈEˇˇˇêãÜÙ   Ö¿tççHˇ±éÙ   Ñ|ˇˇˇÎÈçv ãÉL  â·∫å   Ë¸ˇˇˇÖ¿I$É‡ÉËÉ¯
v1“1…ÈŸ˛ˇˇçt& ã≈    ã≈   È¬˛ˇˇË¸ˇˇˇç¥&    êË¸ˇˇˇVâ÷Sâ√ÉÏd°    âD$1¿ãÉP  Ö¿t,∂Ét  Ö¿uF1¿1“ââVãD$d+    ucÉƒ1¿[^√çv ãÉL  â·∫à   Ë¸ˇˇˇã$É‡É¯î¿∂¿Ö¿t∫ãÉL  â·∫å   Ë¸ˇˇˇÖ¿I$É‡ÉËÉ¯
wóã≈   ã≈    ÎãË¸ˇˇˇçv Ë¸ˇˇˇVSâ√ÉÏd°    âD$ãÉT  5ˇˇˇ ÉX  tãD$d+    Ö·   Éƒ1¿[^√ç∂    ãÉL  â·∫å   Ë¸ˇˇˇãã\  ãì`  Ö¿I$É‡ÉËÉ¯
vI	 tÆ«É\      «É`      «É@      «É<      «ÉD  –  «ÉH  @  Èmˇˇˇçt& ã4≈    ã≈   1Ò1¬	 ÑOˇˇˇâ≥\  ÅÊ ˘  âÉ`  «É@      «É<      «ÉD  –  t°«ÉH  ‡  ÈˇˇˇË¸ˇˇˇË¸ˇˇˇU1“WVSâ√ÉÏd°    âD$ãÉL  çL$Ë¸ˇˇˇÖ¿à–  ÈÃ  ç¥&    çv Ë¸ˇˇˇUWâ«VSÉÏd°    âD$ãGãPãRË¸ˇˇˇ%   =   Öö  çoπ   ∫  âËË¸ˇˇˇâ√= ˇˇáY  Ö¿t1“Ë¸ˇˇˇ∏   Ë¸ˇˇˇπ   ∫  âËË¸ˇˇˇâ√= ˇˇá&  Ö¿t'∏ƒ	  Ë¸ˇˇˇ1“âÿË¸ˇˇˇπ   ∫˙   ∏»   Ë¸ˇˇˇπ¿  ∫Ä  âËË¸ˇˇˇâ∆Ö¿Ñ  j 1…∫Ä   â¯Ë¸ˇˇˇ[â√= ˇˇá    âÜL  π¿  â˙âË¸ˇˇˇ∫   âÉN\çé¿   «Fhê  «Ü|     «F8  «F  «Ü‹      «Üÿ      «Ü¸      «Ü¯      «Ü     «Ü     Ë¸ˇˇˇâ√Ö¿u?ãÜî   ∫Ä   â$ãÜL  jçL$Ë¸ˇˇˇYâ√Ö¿â$  È˝  ç¥&    êÖ€Ñ˝˛ˇˇãD$d+    uÉƒâÿ[^_]√êª˚ˇˇˇÎ·Ë¸ˇˇˇªÙˇˇˇÎ’ç¥&    ç∂    Ë¸ˇˇˇUWVâ∆SÉÏãYd°    âD$1¿Ö€Ö´  °    â◊âÀÖ¿èÈ  ãk1¿ãSÖÌHË∏ˇ  9≈OË1¿Ö“H–∏   âk9¬O–âSãÜT  â¡ÅÒˇˇˇ éX  Ñö  % ˘  É¯¿É‡`‡  )–j π–  ∫—   j )Í)ÈçkPÉËPçCPâËjË¸ˇˇˇããCÉƒÖ“Ñ  ∏ÍˇˇˇÉ˙tãT$d+    Ö  Éƒ[^_]√çt& çæ<  Åˇ ˇˇáı  ãÜD  9CÑO  ãéT  ãÜL  â ÅÚˇˇˇ ñX  Ñg  ÄÂ˘ÑÅ  Ω‡  ãK∫   Ë¸ˇˇˇãKãÜL  ∫   K)ÈË¸ˇˇˇãKãÜL  ∫   ¡˘Ë¸ˇˇˇãKãÜL  ∫   É…Ë¸ˇˇˇãSãCçå˛ˇˇãÜL  ∫   ¡ÈË¸ˇˇˇãSãCçå˛ˇˇãÜL  ∫   Ë¸ˇˇˇÎ$ê∑N 9»ÉÏ   çÄ¡‚çz0Åˇ ˇˇá  ãCâãCâGãCâGãCâG1¿È¬˛ˇˇç¥&    ãÜL  â·∫å   Ë¸ˇˇˇãSãkÖ¿I$É‡çHˇ∏@  É˘
áF˛ˇˇãÕ    È(˛ˇˇç¥&    çv ãÜH  9CÖ¢˛ˇˇãÜ@  9CÖì˛ˇˇãÜ<  9CÖÑ˛ˇˇÈaˇˇˇç∂    â·∫å   Ë¸ˇˇˇÖ¿I$É‡ÉËÉ¯
v+ãÜL  Ω@  Èz˛ˇˇçv Èˇˇˇê∏ÍˇˇˇÈ˘˝ˇˇç∂    ã≈    ãÜL  È?˛ˇˇË¸ˇˇˇâ¯È’˝ˇˇfêË¸ˇˇˇVâ∆SâÀÉÏãId°    âD$1¿Ö…to∏ÍˇˇˇÉ˘uE«C    «C    «C–  ãñT  â–5ˇˇˇ ÜX  Ñá   ÄÊ˘Ñ°   «C‡  1¿çt& êãT$d+    Ö√   Éƒ[^√ç¥&    fêããKÖ¿tÉ¯u2çÜ<  = ˇˇw≈ãâSãPâSãPâSã@âC1¿Î™ç¥&    çv ∏ÍˇˇˇÎôç¥&    fêãÜL  â·∫å   Ë¸ˇˇˇÖ¿I$É‡ÉËÉ¯
v-«C@  1¿È_ˇˇˇç¥&    ∑v 9Òs çâ¡‡É¿0Ètˇˇˇã≈    Èˇˇˇçt& Î‚Ë¸ˇˇˇ               Ä                 [                                                              ¿           Ä                                                         ¿                                               ˇ       $                                                                                             @   p   ¿      @  Ä  –  ∞  @  F   †  ¿  @  Ä  0  †        p  `	  @
  p
    0  ¿  ‡  @    ¿  ‡             Set video std register to %d.
  tvp5150.c: removing tvp5150 adapter on address 0x%x
    VBI can't be configured without knowing number of lines
    Selecting video route: route input=%s, output=%s => tvp5150 input=0x%02x, opmode=0x%02x
    link setup '%s':%d->'%s':%d[%d] tvp5150: Video input source selection #1 = 0x%02x
  tvp5150: Analog channel controls = 0x%02x
  tvp5150: Operation mode controls = 0x%02x
  tvp5150: Miscellaneous controls = 0x%02x
   tvp5150: Autoswitch mask= 0x%02x
   tvp5150: Color killer threshold control = 0x%02x
   tvp5150: Luminance processing controls #1 #2 and #3 = %02x %02x %02x
   tvp5150: Brightness control = 0x%02x
   tvp5150: Color saturation control = 0x%02x
 tvp5150: Hue control = 0x%02x
  tvp5150: Contrast control = 0x%02x
 tvp5150: Outputs and data rates select = 0x%02x
    tvp5150: Configuration shared pins = 0x%02x
    tvp5150: Active video cropping start = 0x%02x%02x
  tvp5150: Active video cropping stop  = 0x%02x%02x
  tvp5150: Genlock/RTC = 0x%02x
  tvp5150: Horizontal sync start = 0x%02x
    tvp5150: Vertical blanking start = 0x%02x
  tvp5150: Vertical blanking stop = 0x%02x
   tvp5150: Chrominance processing control #1 and #2 = %02x %02x
  tvp5150: Interrupt reset register B = 0x%02x
   tvp5150: Interrupt enable register B = 0x%02x
  tvp5150: Interrupt configuration register B = 0x%02x
   tvp5150: Video standard = 0x%02x
   tvp5150: Chroma gain factor: Cb=0x%02x Cr=0x%02x
   tvp5150: Macrovision on counter = 0x%02x
   tvp5150: Macrovision off counter = 0x%02x
  tvp5150: ITU-R BT.656.%d timing(TVP5150AM1 only)
   tvp5150: Device ID = %02x%02x
  tvp5150: ROM version = (hex) %02x.%02x
 tvp5150: Vertical line count = 0x%02x%02x
  tvp5150: Interrupt status register B = 0x%02x
  tvp5150: Interrupt active register B = 0x%02x
  tvp5150: Status regs #1 to #5 = %02x %02x %02x %02x %02x
   tvp5150: Teletext filter enable = 0x%02x
   tvp5150: Interrupt status register A = 0x%02x
  tvp5150: Interrupt enable register A = 0x%02x
  tvp5150: Interrupt configuration = 0x%02x
  tvp5150: VDP status register = 0x%02x
  tvp5150: FIFO word count = 0x%02x
  tvp5150: FIFO interrupt threshold = 0x%02x
 tvp5150: Line number interrupt = 0x%02x
    tvp5150: Pixel alignment register = 0x%02x%02x
 tvp5150: FIFO output control = 0x%02x
  tvp5150: Full field enable = 0x%02x
    tvp5150: Full field mode register = 0x%02x
 reading ID registers failed: %d
    tvp%04x (%u.%u) chip found @ 0x%02x (%s)
   *** unknown tvp%04x chip detected.
 %s left=%d, top=%d, width=%d, height=%d
 7 width = %d, height = %d
 %s reg %02x = %*ph
 g_sliced_vbi_cap
 ck ss sync lo%s signal
 normal black-frame-gen svideo aip1a aip1b %s: failed with error = %d
 Teletext filter 1 Teletext filter 2 tvp5150: FIFO reset = 0x%02x
 CC   data WSS  data VPS  data VITC data Line mode pdn reset tvp5150a detected.
 tvp5150am1 detected.
 tvp5151 detected.
 tvp5150 include/media/v4l2-subdev.h Disabled Black screen â$Ph    ˇ≥†   h    Ë¸ˇˇˇãL$ÉƒÈD  RSh   ˇ∞†   h    Ë¸ˇˇˇ1¿ÉƒÈÖ  Ë¸ˇˇˇUWâ«VS∂ŸÉÏâT$∂l$0d°    âD$1¿â,$â˝9$~Qã4$)ﬁ;t$4Ot$41ˇ9˜}çâË∂“Ë|  àD<GÎËçD$PVSˇt$h   ˇµ†   h    Ë¸ˇˇˇ\$PÉƒÎ™ãD$d+    tË¸ˇˇˇÉƒ[^_]√h0   ˇ∞†   h    Ë¸ˇˇˇÉƒÈ‘  ∑@¿Ph    ˇ∑†   h    Ë¸ˇˇˇÉƒÈò  hX   ˇµ†   Ë¸ˇˇˇ1…XZÈj  Ö¿∫E   ∏B   D¬PhH   ˇ≥†   h    Ë¸ˇˇˇÉƒÈ⁄  Éªh   ∫Z   πa   E—Ö¿t2É¯πq   ∏~   E¡j VRPhî   ˇ≥†   h    Ë¸ˇˇˇÉƒÈ∏  ∏x   Î◊ãL$$âD$É·Q∑T$âL$RãVˇr∑WRˇuh   ˇ≥†   h    Ë¸ˇˇˇãD$,Éƒ É¯Ö’  É=     ãL$Ω   éö  QãD$¡‡ÿ∑ê‘   RãÄ–   ˇp∑GPãGˇph   ˇ≥†   h    Ë¸ˇˇˇÉƒ Èö  ãD$$É‡P∑D$PãFˇp∑GPãGˇph   ˇ≥†   h    Ë¸ˇˇˇÉƒ È’  PhX   hÑ   ˇµ†   Ë¸ˇˇˇ1¿ÉƒÈ;  hX   ˇµ†   Ë¸ˇˇˇX1¿ZÈ;  ãD$Ph  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿â'
  PhD  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿â˚	  Php  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿âœ	  Phú  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿â£	  Ph»  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿âw	  PhÏ  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇâ«ÉƒÖ¿âI	  ãÉL  çL$∫   Ë¸ˇˇˇâ∆Ö¿xãt$ãÉL  çL$∫   Ë¸ˇˇˇÖ¿xãD$WVPh   ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫	   çL$Ë¸ˇˇˇÉƒÖ¿â·  Phh  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫
   çL$Ë¸ˇˇˇÉƒÖ¿âµ  Phê  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿ââ  Phº  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿â]  Ph‹  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿â1  Ph   ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿â  Ph4  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇâ∆ÉƒÖ¿â◊  ãÉL  çL$∫   Ë¸ˇˇˇÖ¿xãD$VPhd  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇâ∆ÉƒÖ¿âå  ãÉL  çL$∫   Ë¸ˇˇˇÖ¿xãD$VPhò  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿âC  PhÃ  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿â  PhÏ  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿âÎ  Ph  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿âø  PhD  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇâ∆ÉƒÖ¿âë  ãÉL  çL$∫   Ë¸ˇˇˇÖ¿xãD$VPhp  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿âH  Ph∞  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿â  Ph‡  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫   çL$Ë¸ˇˇˇÉƒÖ¿â  Ph  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫(   çL$Ë¸ˇˇˇÉƒÖ¿âƒ  PhH  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫-   çL$Ë¸ˇˇˇâ∆ÉƒÖ¿âñ  ãÉL  çL$∫,   Ë¸ˇˇˇÖ¿xãD$VPhl  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫.   çL$Ë¸ˇˇˇÉƒÖ¿âM  Ph†  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫/   çL$Ë¸ˇˇˇÉƒÖ¿â!  PhÃ  ˇ≥†   h    Ë¸ˇˇˇãÉL  ∫0   çL$Ë¸ˇˇˇÉƒÖ¿âı  É‡∫   )¬Rh¯  ˇ≥†   h    Ë¸ˇˇˇ∫Å   âÿË|  ∫Ä   â∆âÿË|  VPh,  ˇ≥†   h    Ë¸ˇˇˇ∫É   âÿÉƒ$Ë|  ∫Ç   â∆âÿË|  VPhL  ˇ≥†   h    Ë¸ˇˇˇ∫Ö   âÿË|  ∫Ñ   â∆âÿË|  VPht  ˇ≥†   h    Ë¸ˇˇˇ∫Ü   âÿÉƒ(Ë|  Ph†  ˇ≥†   h    Ë¸ˇˇˇ∫á   âÿË|  Ph–  ˇ≥†   h    Ë¸ˇˇˇ∫å   âÿÉƒ Ë|  ∫ã   â$âÿË|  ∫ä   â≈âÿË|  ∫â   â«âÿË|  ∫à   â∆âÿË|  ã$QUWVPh   ˇ≥†   h    Ë¸ˇˇˇπ±   ∫†   âÿÉƒ jhµ   Ë§ıˇˇπ∂   ∫≤   âÿjh∫   Ëåıˇˇ∫ª   âÿË|  Ph<  ˇ≥†   h    Ë¸ˇˇˇ∫¿   âÿÉƒ Ë|  Phh  ˇ≥†   h    Ë¸ˇˇˇ∫¡   âÿË|  Phò  ˇ≥†   h    Ë¸ˇˇˇ∫¬   âÿÉƒ Ë|  Ph»  ˇ≥†   h    Ë¸ˇˇˇ∫∆   âÿË|  PhÙ  ˇ≥†   h    Ë¸ˇˇˇ∫«   âÿÉƒ Ë|  Ph  ˇ≥†   h    Ë¸ˇˇˇ∫»   âÿË|  Ph@  ˇ≥†   h    Ë¸ˇˇˇ∫…   âÿÉƒ Ë|  Phƒ   ˇ≥†   h    Ë¸ˇˇˇ∫    âÿË|  Phl  ˇ≥†   h    Ë¸ˇˇˇ∫À   âÿÉƒ Ë|  ∫Ã   â∆âÿË|  VPhò  ˇ≥†   h    Ë¸ˇˇˇ∫Õ   âÿË|  Ph»  ˇ≥†   h    Ë¸ˇˇˇ∫œ   âÿÉƒ$Ë|  Ph  ˇ≥†   h    Ë¸ˇˇˇ∫¸   âÿË|  Ph	  ˇ≥†   h    Ë¸ˇˇˇπê   ∫‚   âÿÉƒ jhì   ËñÛˇˇπî   ∫Ï   âÿjhô   Ë~Ûˇˇπö   ∫ˆ   âÿjh¶   ËfÛˇˇπß   ∫   âÿj
hØ   ËNÛˇˇ∫
  âÿπ–   Éƒ jh˚   Ë3ÛˇˇXZãD$d+    Ö˝   Éƒ1¿[^_]√ãD$È–ıˇˇãD$È¸ıˇˇãD$È(ˆˇˇãD$ÈTˆˇˇãD$ÈÄˆˇˇã|$ÈÆˆˇˇãD$È˜ˇˇãD$ÈB˜ˇˇãD$Èn˜ˇˇãD$Èö˜ˇˇãD$È∆˜ˇˇãD$ÈÚ˜ˇˇãt$È ¯ˇˇãt$Èk¯ˇˇãD$È¥¯ˇˇãD$È‡¯ˇˇãD$È˘ˇˇãD$È8˘ˇˇãt$Èf˘ˇˇãD$ÈØ˘ˇˇãD$È€˘ˇˇãD$È˙ˇˇãD$È3˙ˇˇãt$Èa˙ˇˇãD$È™˙ˇˇãD$È÷˙ˇˇãD$È˚ˇˇË¸ˇˇˇ∑D$∑T$ã$∂L$f¡¿f¡¬fâÜv  ∑¿fâñx  ãSâ$Å¬  R∑S“R∂T$RQPhh	  ˇ∂†   Ë¸ˇˇˇ∑Üv  Éƒf=PQÖâ   ∑éx  ãñ†   fÅ˘!Öö   h  RË¸ˇˇˇXZãû»  1¿9ÿÑ¡   i»å   ãñT  É¿î8  â$ãñX  î<  â—ã$âéX  âñT  ÎøPhD	  ãD$É¿PË¸ˇˇˇÉƒÈ‘  ãñ†   f=QQu$fÅæx   uhH  RË¸ˇˇˇXZÈsˇˇˇfÅ˘ tPhî	  ˇ∂†   Ë¸ˇˇˇÉƒÈSˇˇˇh2  RË¸ˇˇˇãÜL  1…∫0   Ë¸ˇˇˇY[È/ˇˇˇÖ¿u«ÜT  ˇˇˇ «ÜX      çÜÃ  1…∫   «Ü\      â$â√j «Ü`      «Üd     «Üp     Ë¸ˇˇˇπ 	ò ∫  âÿj hÄ   j jj hˇ   j j Ë¸ˇˇˇπ	ò ∫  âÿÉƒ$j hÄ   j jj hˇ   j j Ë¸ˇˇˇπ	ò ∫  âÿÉƒ j hÄ   j jj hˇ   j j Ë¸ˇˇˇπ	ò ∫  âÿÉƒ j j j jj jjˇjÄË¸ˇˇˇπ	ü ∫  âÿÉƒ j h¿¸õj jj h¿¸õj h¿¸õË¸ˇˇˇ∫  âÿπ	ü Éƒ h  j j j jË¸ˇˇˇâ^lãû  Éƒâ⁄Ö€tã$âT$Ë¸ˇˇˇã\$È‘  ãÜL  çL$∫å   Ë¸ˇˇˇÖ¿xãD$É‡∫@  ÉËÉ¯
Ü®   ãá‡  âñH  1“«Ü@      âÜP  â«Ü<      «ÜD  –  Ë,  ãó‡  Ö“t&V1…âËh[  h   h   Ë¸ˇˇˇâ¬ÉƒÖ¿ÖRˇˇˇâË¸ˇˇˇâ¬Ö¿àAˇˇˇÉ=    ~âË‹  1“âËË¸ˇˇˇâËË¸ˇˇˇ1“âËË¸ˇˇˇÈ‘  ã≈    % ˘  É¯“É‚`Å¬‡  È9ˇˇˇˇqˇqˇqˇqhh   h∏	  ˇ∂†   h    Ë¸ˇˇˇÉƒ ÈB     W  ≠  ÿ  c  Î	P  c  Î	Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇ           ∞              ˇ                                              @                ˇ     tvp5150_get_vbi tvp5150_set_selection   tvp5150                                                                                                                                         @           à  @  
                                                (   (   ,   0   Ä   å   ê   ª   ¿   Õ   œ   ¸   p
         †                                  ‡  †      @             ‡  ¿  @   p       @  @                                                  ¿  0  ¿  †      @
          ∞                        @                                                                          ‡                                          ‡              0                      @              `	                                                                                                                                                                                                      0               ™™ˇˇ'. +¶r                                                                                                                                                                                                       ™*ˇ?Qniå	   '    @           [U≈ˇ qnB¶Õ   :                                                                                                             ê               ™™ˇˇ∫Œ+¶⁄   ` o GT' ˇˇ                               ` 	Ä
Ä ÄG     Ä     ( ./ª ¿ ¡ ¬»Ä…   ÀNÃ Õœ – ¸ˇˇdebug parm=debug:Debug level (0-2) parmtype=debug:int license=GPL v2 author=Mauro Carvalho Chehab description=Texas Instruments TVP5150A/TVP5150AM1/TVP5151 video decoder driver              §ˇ      GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                                                                  1   @   &     H   p   F     `   ¿   ;     u      9     å   @  9     §   Ä  A                                	              
 ±   –  ‘     y           ¡       %    
 ÷   ∞  ç     ‰   @  V     ı   %   !    
   F   õ    
   †       (  ¿  Ä     A  ¿  ‘    Q  ·       
 o  @  7     á  Ä  ¨     ñ  ˛   $    
 ™  0  g     º  †  W  