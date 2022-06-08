fig/GENERIC_ATOMIC64) \
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
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/ACPI) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
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
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
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
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/tw9906.o: $(deps_drivers/media/i2c/tw9906.o)

$(deps_drivers/media/i2c/tw9906.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               \
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
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/PCI) \
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
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
  include/linux/netdev_features.h \
  include/linux/sched/clock.h \
    $(wildcard include/config/HAVE_UNSTABLE_SCHED_CLOCK) \
  include/net/flow_dissector.h \
  include/uapi/linux/if_ether.h \
  include/linux/splice.h \
  include/linux/pipe_fs_i.h \
  include/uapi/linux/if_packet.h \
  include/net/flow.h \
  include/net/page_pool.h \
    $(wildcard include/config/PAGE_POOL_STATS) \
  include/linux/ptr_ring.h \
  include/linux/netfilter/nf_conntrack_common.h \
  include/uapi/linux/netfilter/nf_conntrack_common.h \
  include/linux/set_memory.h \
    $(wildcard include/config/ARCH_HAS_SET_MEMORY) \
    $(wildcard include/config/ARCH_HAS_SET_DIRECT_MAP) \
  arch/x86/include/asm/set_memory.h \
  include/asm-generic/set_memory.h \
  include/linux/if_vlan.h \
    $(wildcard include/config/VLAN_8021Q) \
    $(wildcard include/config/NET_POLL_CONTROLLER) \
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
    $(wildcard include/config/RPS) \
    $(wildcard include/config/NETPOLL) \
    $(wildcard include/config/BQL) \
    $(wildcard include/config/RFS_ACCEL) \
    $(wildcard include/config/F‚É:á=#Ñóﬁ’ÖËiƒP@¡|2¯bb÷U—i.ï;7«ñºWN3'òûç∑87ºÑJB Ìhe.’xËZóË`°µ-'D˛	5|3£Î¢"“díÑñBögàé¡Y∫´.´πæ»óJ˜Ê≤Œ=S
Ûù≠_S-`£‘≥Ñêj™§ÖÚ‹6¨⁄ÅÀm,à%ïDÁ0‚.=Å4AÕfAzƒ	‰ QÙû‘œ¬}î Q÷ˇƒó⁄º4‰®m¡ïã˚FKﬂI˝ ëœ⁄éZ2gVõ¢ÌÙR◊=£80Ü´{≤“]ˆ∂îì2ËÍ¨ó™.GƒŸ≈Ó$Ç3^jÜ" ªeù◊vZ1 UP51∏MA<p¶‘äÎŒÄ/âÈ8êãbπwûæƒKü“ıÖ|sÄo[^†EPT"„£"—YmIPDı‹.‚wW^’x6È0`
z`òO%9ï†4E¨m¡øTãTMÒÒ_∂œ]Be-÷^zôçqÜ´€äúã≤	Ø÷hlû»^!‡>wËT‚ÿŸ^’	#cXµè‰KÅı3Å†S∆Ä›=*<d4+ò“U~‹|ΩU∞µB4 Ëå›§€ù≥Ωm;≠{O;)éïBu(imJ{6´&°5èÅ¶™Ÿ°*q®•ˆADüòfgFæø£]}Féó«Y,…ü…3Kne≥‡V\Ó√A™≠P«LñŒi—Ñ`uÌœŸ»[V[ÛË9*ï‘ìeî`(+Å[HPw‰K®HH)îÀi’ïî—
‡™Nm^±¥7˛/y¿úav“i£Ω¸ùõ1è◊¿gei°¨ù˙£6Wô°ûˇ≤›}¶‰˙S≠SôaıÚ◊G#3v˝fÂòY∂0u≠…€æ>©`üåˆêBÇŒÅ›oõj!∏”€>ö8õËÿ~øNK˝â¨G◊–X“E¶}Ë”»¬‘Aúéˇµx#Ë©FŸ±GT-∑Ωt√HïH4¥ [vNÎ,\”Ûi¨ﬁ™
†@üŒoéû–XÜè3ŒÆZ	Î«∆ h.^ˆ±¥ƒ.Vk. „(Ö»#~3 ŒÈ.†‘ÒÊ ì∏9^®|s*ö=µ[…8ÜV‹çA“>⁄YB(û;GÛn·‚ì£±Œ5W¿Î„VZÀbjfô◊.óW˙zÆ¡®,_3õÿIÁ§.Ã◊(ıÁ\m€^?‘ºPäãq÷H≠1'2YXâ÷á≥rV˘àL∏ü(IÊ3WŸ`ßá¿ﬂ@òW ˙sÁﬁ—∑∑®JuRvéÛ≠zFnsqTèHµ™˚z¶&Ë#Z√‚ÉÔ”∆UrûÔ◊ª"bπzÁÕL ÎàŸïÆ"`‡-3hi◊ìÄrmÄ`Az∂√2—˝ã5´koƒarsÉ“Á_´3˚'.≈w<êF3]ﬁ]Bﬁ¿ê∫„"Îtp¥áj—”∂¸Î@Ø„	Öª\J_6ÚìÖXsq>>õ·Ç¶Ω^˘˛ÿœßâÆó—“∂
c9Í€pïQf∏≤Ã'b_cè˘∫0«á:QF5[Ï¶)∏Ü%Ô8≈ Ä+ÇÅØVÔBßæ˝UÊÖ_|rŒædˆÒ⁄¡iÆˇ\U∑F—sú“¥m ı<Ë·?∞ú√+ÅIÆŒA’®[Ò93iËÊ”rü∫Lc∂È+√Â(øJ˙#W¿Zœ˝e‘.%E!‚QtÈ{ö”ﬂÓ–∂'~ûœY›ı-∫Òã=æ)º%åZº,é‘ä©D∑	øéÇ’Hø2&ºÖTó¬Z!ëEoÛö∫Ü J√V„˜ç˚÷6Ö≠^õÛ£1È…Â;"◊}øïÆ<[ı¨J¢{Êxrm*Éú9Odˆ!w’ÿŸ€°=M-à)˙∑,¥ø¬§√K‰ïÕqœÅº˝∏V”ÖY⁄wùÀÿ∂ÔxÄØ›&?rŒÍâs›p™	√¬«Q\\•±Ñ§ö[·jí‚†YGg÷ÎÖπÔV6P<Nö…Òu¨3åÎUÓæ¶”YçƒÏóÑÆﬁ¯«ñç®^?˝˚ﬁq›˙ôÖÜöMD˙ºnÿøÓ`“fáìñ‚›G-TE,s‚È:ÈCk¥ ∑Xc=d"(¡7s'g∆æÉ^g3âÇ◊”kÔö≥ÓìÂÄ%¶øﬁ1uÔ˚Æ„uÕt›Îcµa¬‹ ˇKÛt‘’»®nA\Aaq€Ô∂f‹ÍÂ{ø§ƒËãåÙñÏ˝WW¯ík+Éº∑™Ì´†¢_F¢/Í∆úZ;Œ8µA\l˜;ª<Ωöÿüû“e“ è∫V”YÙòµ‡∞iyU€ÒN5≠óxñ—rÎéÈ£4Ç∂ÆGuNƒΩ—ö‰$éè´™c¢ß¡ÑÃ⁄áS≠eEÊpèª2º∆Ò\õÛ.A"ı±L›•∫)úé¶zÙ˙9‹Ëjg—´g∑Iô»†SÂöêPäz—f5±o˜k°ûê¥·˜rÈë1”_÷íjæ Æò®«|!aM$Ã≠…=0”#Ä¯zˇ÷˜∑0’ïYXøC·„€ôí<»[ˇ6¨é≥ŸPèè!ió¢å√ ÜoDûårM‘∏_oòX¡÷∫ŸLùg pc≤øÆhÜ)ºÄ≈≥Jy85¸BÒ† ÜÏ≤}>ÙuÃˇ°iØM˘aø:œïœÚÅtzËÕ%1ÅΩ:˘Îoõôj‹TâÿÜÓäJ9…Â•ƒ±”tŸÇ˝”[FÍÑöæ»UIUàJ˘ÁB*Ÿ6^JQ∏ií,ÈÁ›B»(«ÁDÍvùπ„Ì?ÂŸÇm¡ ]6Gù=Ó†zL^åKÛnÉ`¶4“ãÒ∫K≠ pvÑ- u÷¡hA—¥(∞Ó~ªêŒ~¨GL7O<4◊è+t[gÆπbSu¶Ë
∆ÿ},ÏiÍ˝kRqI–»„pÇ+Ê];∫´∏âk>:Gv°ìàøxì1=ÅT∫3e‡îÒÿrî@ˆeñÄﬂIQî
ŒY7’îGÄqWÅ∆ë˘˝bÖûñ–„ﬂ\,,úÖÅ∏ü$â›u⁄uÑ˘¡ô´˜¿Ø‡ƒ —Ì±¸
∏—é¸Rjﬁﬂªﬂòå“äú3|· ≠ﬂ^t/è1t•1≠©U«—òZ©ª9ùäTã√h$≠aG^‚5Õ¨H#CX8Å©UÖ”∑kÙC=Xã˘“2≤Ä˙Ï§Ñ/iâ-[›ÛWÛÖL¥ãP˚ô#µÚÉ*ÜÁÙºNï{û⁄W”ÿíΩ’Ç˛)ã‹∑ı5Cﬁ miy≠b•^Ë%æ[
Ö6Fï≈zÄ0œjÎÍgäO#‚køZ}π]-Ê¶»<K∂∏e≥y(hÁ't‚±©ck≥€∆X.9»qo
b0.Ü;›:YXv]—fÒæ_"æ:°GUR éñv0Asã˛∑{≤ê¬˜y-æ¡ûÄÅé !Ã∏i:⁄—¥ìIzÉß¨ÊÉM‘<d¡·I<Û∫]·¡é¿p≠zÉåuÓ≥∞áÔﬂHΩ†Pø dNX3ÿ∫MçìwˇHœp®± ›òLtæQU›±˝qå¨iyñπôAñØ…µ'r§Èπuwp»[å◊áèì™‹‚«DJVÙ3√‘‹ΩØmBΩUd ÔS‘^z6*!…O>œ8‚“∂≈ºpDºΩ?–⁄ìﬂÀÁ]qù¿#S∂∞c‡ªTÿ1˛ï,-∆ÁYz´DõõµÙªu“&ù∆˚í¥®€1d¨í*–ÚuÑ“ôÛπ#aS~ﬁ	¸’Ò7◊Ù¢1Ê™H©à¶ﬁﬂZ
G*HúN„‰p≈´©^ÇÜ%€^¸°ﬂW…Ï!Bˇu˙t]dƒ∂û:BWiñÎ¥„p¡÷†¶$)èÒ˚M Ííó®∫˚±2∏{°jÅÌG›{ëâÿ©∫‘èMZ£$¿RPtÀ };Ãu‹„,û≤ÁØ‡Ã4∏∫ÕÔÛﬂÇC)UanúŒcÕiØ¥'O;S2ÎÉ'ôîÔ[‘ÏÎk—ºÜ5Øùvë/<\%∞„´_*Æ&¨#LÙEu˙IH˚Í‚pMÑıÕ¶¶‹X{˚tW(6HoıPÚ¢w¯^nQ,uÆ⁄“êv•W7@;Õ—g;æ⁄’ﬂmëâQ ¸£^¯)®6†5®˘÷e4j(aö∏ÉÊÂ)UïAàZÃë†Câ\ ú˚ƒèYj9µxtˆ)^Áñ>“jp:
Ù'ÄgO∑ÇGÈ«•á‡FÅ∆Q◊ÿ˛êNÑ£qÌıé^Åü¸N}Ö©¸IfÕÂÜWﬁ‰CD˝.«∆±©ã†|‘ß±#¯Ó¡8{
È/$ç8’ﬁ\b](√jF¿œ¬)¬øú«1ó1=‚B°IOˇ»ÉæèÕ*¶àØOÀ˛ıÆ˘Òç+G˙ïøﬁ√Ù)‘Z2)–:2Ô*◊kÎÍ”·MRÉøìŒÁÖÔÍÙ+Dï|Igè~Ÿ3êÂcÎ—A1ò≤√ÊDM™´yµÎñÕìÎÌn»a5"u◊ÆåçÈ∏«kO·≈®∑(‘Ï“«_<Ω|WÛTçº\≠HıÂzN√öÁVD|j!I:2:W◊◊/Gº”P9Ê3q?75iü¥\#ˆŒküîºF˝+<"Ìg)ˇæ‘∑V6‘dö*¢√:\9Kön?ÂGÀ∑›¨»Nb‹Gãbz[ÁJ Fû£…±1d∫$uB˝a›Ÿ÷fcg¥rÂ‹¬øà7H≥lü‘ÌCÃÃ ≈±z¬.:≈á
{S,ﬂÿyç)¥†V,sÁ´ˆk#·$ Eq‘%◊ÜFXâıï;Ééò®3π˘Àú`ûí„∑ÕfﬂLC‘˛À¬eÏÇ{”8≥Îßf!±Ö'Iç
∏˛˜M î⁄è›+Ô+î,¿IIM¯˚/¬ı©√Étä…@LÁ;l €ê7%U^=ÇÈtr´Ö.À©Â)Ï´[Óíˇ"≈Ò–€≠tÂÿÌÿìFö“´øﬁkˆzhp¿·yFèÎ?è,~«˛è0ƒ99¯Å;˝µÉxVzëÍƒﬁ˝È¯≠#à+ÿ¯Å„Ø»+1*a x/~ÂnøáÚƒÆ4]çn¸ÌÑAøWë·» ‰S>ÑÍh§8"√≈ç,õ”r≈A’ÇrW◊p°H:S—îlb¿g_œk÷∑ÿp^™h≥∂Z|≤Í„†£ˇv5ûË˛3_|ÃÛ°.˘/I¯0_T,Ru˛∆Kå”ıŒÙ0ıl‡≤@áÆûêü˙ªè“à¢JÀêîÛiÑåˇ≤x„Ÿ¢ πÕ≥¨‡UM‰?OÀ,RÂàáGÀtd∑ˇ•Bj>‹pGp[ge3© €ÅﬂêÃz\Éàëi‚∂∑´ZˆóÚoı%ãd˙_“≈¯}vÊR˝afzıaÆ]ç1∞SÊÄöÑ√[;5ã◊Ä7ßäÛ˛?Ê{{Bo†íZàhóHˇŒhﬂJﬁ®:ëIÑÆ∑MQôñÎÊ‚‚÷¿˚€UÒ¸†Ñ·§‚ÇÑéngΩ
Ï}kZ|„IÁÙÎ+¬?é´&Á…⁄ˆë'¿±¯ÊÛö\oΩ¡-oÁ›x-ä∏ËÕ7œ˚Íì®é‹`§ÃZæ6˘;\≈õ™§èÙ´∆Ì+ ◊†˙ó˛ÉY≠ÈâHùòÛRcSÓµÊf†˝óJ`=å€Æg«ﬁëm†{∞øŸTÁNºzı®âK˚QÁnÊ·ïÅFB&æh#ÿéx¸Èú)÷HŒi9…fªd‘˛≤õ™KÆˇ±Ø•9g˝!ƒ€Î⁄√÷¬ƒ•À$®•3€X®ß¢[•ŒZê¯Ë`«ä]ãˇ˚6)‰4¢?J’˙' ´≤Z‘˚\D‘⁄Â÷ª»ˇ“-èúÈÈµ¢ÇÔ\Az„+˛up3∫ÕÑIÜç~Cij_LõèÆ€÷Äß–´Mùí
Áõ5«4.ªÃÕ€Ÿ; ãŒí√ìN[C¢‚™DÖN¶û¡9ˆD◊≤U±~±˘ÍEãŒf>¨µYŸæ&]˙ÒÜ«çÊàÄ”i™”{n≤O˚p] õú∑°zÃ ﬁE…´®®Wó"]/PôÚãìì„øqâJ5AaÁÅ˜”b¨ï∆XÖâ¿ÙÑ™@#Í4¥e@4ÛﬁQ∂¯9ŸªˆÑÿ¯^°>∞úô∏òÄKnü>n§›/Ss ºÆ^¡lFÒ·lˆ1⁄∂	Óˆô‚l´ßﬁEVóÍE¬3Qzs7b@&>1ê≠˛õûîˇWÚÀ.∑·Kmû4<ötâÒkæ0öL5¨ÇLÒmCªÛvõˆâ3`àM0iÁ§èú(æ*ÑàA°…ΩLÙPI&Vˇ_KûQ…&’¨µ_'Ñ5€ÁÔè^™ΩŸf∂‡Ùπ”jNÆ <{·4Y8t^ñ7:ŒÈí˝L ˘÷x˘-J)∂Z€«éJ§L≥J¡Õm∫|Vö™Kˇâxt>ü˜MåÍº#—$∂∆	ﬂ>∞
Okó
ÙÖ∂>‡©˘&‹?-?râiÃÚi∂Ûß◊ø#OÃXeó‘ÅùI˝Éõf˝)ÃRk¸úåöÂÎÊ˜V˝œ%Hw¶&∫∫,=ø∫ÔÛíó∆Á.wâ`ñ»µ?Ú6q≥åˇdß€ﬂ·áhZÙ≤¿ç„@	Å˘Õû⁄Ò≈‚◊RÆ›O…ë•–+%˙∆#º_N¶õ¸ˆ;çœSøÌ¯ÊK2O-°Úl“˝xf¯aè4ÅW˚{ñ™IYŒ»7EßÓıŸ üëg„B
ÇDú`Ωr¸pÆÇS‰Æ"!J
°/©˘ﬂË»◊8i˛G	≈33œóóˆÎˇJú}A3/#£$ìc9∑ˆµ^o»º•BÀsŸÇ-öí?!GQ@|£Ωú& ”,€dœM!GêÜˇãÍû;"Å≤‹öÀæÎÑ^§¥Åx6ÍVªÍi≤`ıïIÇ#,ŒïG®¢º\…&í]îÔÅÎ›d˘_˘?}K√Ødo‰Ôí"©+‚íq¢;éËÖ!%ò+„K^«#ß^Ω⁄?ÍØ?ˇÌf'Ì7æ˙%≤Ê'$¡õç˘Œ’"n`U»J•≈r4ú’çÈÂ™Ù¸1‚œ€Å·–åbV»Ÿ'É8⁄jö€≈Ö/'√mNÚÛº¡É	U 2LjF÷JÀÙü·hBA∑ ãr¡\h⁄ß8UçŒ&ŸË‚ 3Ø
¡ÃéÉÀ-˙]k	¸•Ïöõ§ƒ%ı¨QÖÉ{©ﬁÌäôõù€
Ø!?Ú€"__∆1	•6=fß¥˙9Ó∏õj¢KÓHÇT}–ikz6
‹™dÿtª<7SáY¸⁄ì ∑&	1Më*ì…Ö◊EÓp◊Í&Íò!vøâ3Ó ë =u≥Ω˝úGÑ·˚:[}#?B“I˚Ú¡~%öáœ˛æ ˜çãÄ˝ŸΩ›ñ∑?ËUuÌ≥õ∫$Ñ¬ﬂ¢“æÃãv1ÏDÅıjπÎ⁄èKiã:≤ÉOˆ⁄«…ÂWtJNƒ•=HƒsÙ¯jMœû `¸˚f˝»Ÿ…5´.ÑÑIP⁄0ﬂQ√óõËä5ª√$£fjBØñnWé¥
Q8sä¶	|Tiñ≈˚8¶á;®ÖÇ&Û¥ƒ>—7eàÀÒÛÍN+	LÁt…∞Z†ÆÒzÜü‰!ÕÇ.]¢3%`∏k›µHy‡¢M7◊≥ØÎ DR€a8≠—1Nmh´∆/∫yr2bV#£€‹°·…qıirMdõnr»)∏‚DæîKHƒGæ>EπÆ!.òLÉc4Ä\§Œ˝¿óÁTK®kQΩ`©ù+≥‰Vñ¡K∆À⁄ﬂEˇá˜–fxDZ˙ï©2pÇÙŒ¯*k2hßñ\%NjnX¯|˜„nŒüƒi'Ä⁄á\R\Ë$@y´xûUñH	tÿ	”ËÌZ%úOÒ•pÉñÁˇÔ°rv“Û∆ö@ÙÍˆÂˆïáætF’ú(;9îÔˆQâÿﬂVƒ>0xæKÃïÙ.$N°»¡*Á 5©ï-8-¸‹Û÷Õ‹ªKò}ftbãå„‹.-Ç„QgÃﬁáwØ¯ˆS'rPoô‘Â,ûŒ¸≈Baìdq„m»≠ëÛˇêÑoa_ƒÊ)Tt^T[HÔÏ:Ó’∞eZ¶Ê∫Œ[A¿ÒÃA<äåâ%’Í{ë}_3Ö:£∂hÓòp·<ÎùÍÆ≈kÉÙòŒ-•'·C¬ƒÌÄZßyLwˇààZö_–œ7áÅ(£ÒS˙T–Wπ# “ÌKëv+)˚ªm&’è‘\>ó"íD2E˘˙Bv‰i£ÍËÑ¿œŸ·7Cœqwzjó:Fâ$yU4±≥∫ÁK
a$§‚‡ x5ﬂ~π∂%Ìô˙êK™23∑Y€÷ÇıBÅöò? ﬂw‹Å‹Ê√›”µùÙ"–>Ó¢hOT%ûÊÚê1oÅØﬂqNsVSm9á©‰˛YÕjIΩ
m˛°'`◊–SÖ∑â¿˙˙NvW:C…”‚P1m≥0ü≠rÏ„≤Áªùá	}GÿÑJ^Läêv_2»¸óÀ%≥R˚≥·√VòH‡äï%ös©ˇÃóß–≥è#∑π–ΩéTkùP%9,∞ˆO√…‚ÃêÇñ£nmJï∏e;.>(∑‰Gq‹Ü\∂n/[ÏÁ⁄π7√ ◊S§€r'‚´dd\ø]ºõT.|MÂ}ø5Æp†
U"Å~	bŒ	»õäb˙ ≥∫ ¥a?tJ–˜ª˛¨zmR≠É® Ûn·˙Á≠X„òT˝^>W"L…<≈UΩØ≈ÂEõâ?≠˛“î§Tï"à˜˚Lbèo¯ß›¯Ya‘ª–¯7»òóˆ¡¢MJÂ7[y¬99V{Z€U4YBÒ’
Õÿ.‡ãàó°O }ÂUJ8'a&ùE[˝¯”‘eó†õsGH€é1ÿÆs)<"}ÿ;⁄Uˇ*ÚåøÆgÇr7G«?P«òG·Ó˝–nvßˆthÎÉD™S0‚ÄóLƒ~.Ry‚~Œíd+íÛvK°èõ#µ/	&WÔ∏∑◊Õ∆D∆6îù`ó(Âó‹¿S9¬$÷Ö>¨Ç2[ÅÇ≥6Ω”æ	àahk⁄¸R”œ≈^€„'„ØœÒÍ≤ˇµ9Z057X√éŸñ1/sáTmXÛP®±Z Çx’=bäR\lw=≠Ã —≠ RŒ´˝õÒµŸjû/I˙∏"åıáGôâ}!√F#o¯[9õπŒ- ˙ê[ïäI¢∑Sx‰•¿≤´÷Èéµ‡ÁDúü_ΩÍ•«äACê“:Œπ Öé–Có?∞dQ∞ﬂ)Ø_rFZ!ˇ™dﬁ4ZîÖÍÇ!Nƒßw4ò•Ä®\-ivb¨•ñ.öá=oIú¶	5Q#‡û,º–€(´¯Xu∑¸Ö}†¯HAÿÏ˚&y∏Ï_ﬁÅxÉÚÍˆxSì›àú˛|%[dÂQwyJ°Éçº…›µ¡AéëÌ9ŒT>’œ1ÓôΩ¿™Gﬂ‡e}T∏°j.∞ÿ¨"iõUDµ‚π∂ê-ó(N“^ùâ>•WîwM#Ç`—˚‘Îƒ’À
ÍlC«–œ4ÄÉA"ÀZhK@zÄ°1ì∆{Áüw;Óè!`1Ø¸©ÂO,›¿aySÁµÚ∑aÍÃ©ƒM∑;Hﬁœ°·µòH)®<qey€ŒoèªÑÔ»>Åı<°„(B-ÇZxíÃU∞\}&ÊÆ–F√≈'“òƒ!à≈ñ©*wrØ¯±ô¯&K‚†ÆH<íüqÉFël—ç\≈¨≤5Æ5ËØ˝
á√ñ}€ÚU˝˛idñH?Ñ_êo¶ªÛ‰^"ùl∞tnHﬂÛR`UêAUÑëd›œ]Ä°‘ú÷>5>.9*‚.¬P4ú£¡úÈØDÒˇˇ—´´Û>ÉfÕh-Ë˙áERW…»…ñÁ˙a¯–koXÜæG,©=‚Ø†G@3…r3Òç«&Æ!≠ÈÜƒMU ≤“ƒ)	ﬂ!≈È<™T!ºÛÂÜÁÓ(Mw˘>±ç¨9Ûüoñ¬1‹”/bkÖbÎÅÛ€g#‡õKy˜ÛìGàÅ%\ﬁ]OÉÄêÍÃûÎ'Ôª¡´@|ˆäu&ıl.\<;æöNL‹\r®rßcLÁö$¯æ~Ω«3uxt5§ƒ—ß'eÉóK8¢}ƒr/ìÛd<Ü‚øˇÈßæ≤Q6˚†ñÕzÄD1_D<7ÌØOBMø+£∫Qr§æã±Lkô $K‹*÷ÃÃ¸,^Mÿ‡4∫±,œF≈§%äπ–´€‚·≈ùŒS¥2ø0qùﬁu á~¡∂#ˇ'«{~ìèûa*˛VBÜ≥cµ‘”ç≠€’h_y#A{èÅãñ Â+øn©˛∞&9ú˛Ù$OY∂_ÕMΩNÛ´ÅVp.+&,˛òåCS˚Å™úâ≠zå©ÕV8Nªz‰1'¨∏yñÚû5vß#πÆS2á(eÂØWIèø{x™¬êÃä£Ÿl&Cí|G§«t ©”^eá‡\≥O&5 5‡_˙V-\˙ª˘ÇEƒª#û:¶ÀÙ	u∏ÁdVVs∂Õ≥≠6˚~‰t1√‘©ÜNÄ˝ò ÄÓ)'Œø<Ã:8*ı‰•%
7õKO‚!ó¡ŸeCº≥◊|ào≤zÚ≤&õœäÕúP#∆ÃSaPˆüZ˛™$$ÖSx.∏7á©í€ZOVoa^b,â≈Yﬁ9*¨±~yÆï"àßVƒ’ﬁM>*áSÃ.—4≈©}'Ñ∂A™≤¥\L•È"ß¯‰Æ5⁄ø¿^’.ûº˜˙*é£±dèÍ;>^ìGLrÏm≠›5Ú≠*Ô&£Ì$6ÙMeÿö∆Ù?[i?/œz‚:D›#´lœÂÒı†sæ-∑”©◊ã Æ6¡…«˚˛÷Dç¨_jX?ãÓê∆;P4∂ôıopØ¨©c˚€¡ê∑∂Uè˜Iab◊FÏG…›v4+NN€^MóÀˇÑƒÿéπLùgb˙Ü‹#Fß2d9€û2ŸBË?[KÍRˇBœ∑€È£ˇYÍ\Híı‰πéM£µß´mÑãE¨^Ò'°ä
Ëä˙ƒÚr;≠Î≈ˇ€L˚·Wüp6ç‰!ƒΩ”°˜ı"_«r¬;Sÿ‹_„2Øáho[º~$·sX•ÜÒÆRË”Ôh€øõç‘5nüÃ[ãã^aå>]¢_≥˚·›Ù◊)ô±.?÷(SﬁöY®)ı4Ÿ$àl¿8òWQÊ:oyAkà/7\è—O ©Ñ˛¸E:‡CØ|∏·®qñX9I><∑‹/ù7€ê¯ºÑ5j¥Úsj»Çç®MC⁄ô≈0Ÿ®ÂU\∫n¸p£“#»è∏#¬3ô e∏–MÕ±,S¶:Ç¬ˆAújdmh|≥”-˛ª<I›‰Fghµ—Ôî<F√.ø%2∞øÀé‘¢˜˛l◊]Ø¨ Õ,ª€vÜv˝LÆ!¬Ç+˚™ô0ß˘åW™örDæª’G«òª€„+\±ÃÉ◊xn-ŸÜòzK√"ﬂOŒ~—-¨d(û*l√r1ó∫∏j~IÄPê≤s:$|’Nú{π#Óü‡Øö:’,ïß¶ÿxäC8◊n>Òø°à…~Â2±Ì=≤ä«'ëiÔ&$“›zg∏ı5|Rˆ#RÈûÏ¶ãJ∞†R=BUÛK$∫
›°…íôzèQÙb”∆âKÿs_ÊÊÚNÔ
\Œè^ÂÍ<ÍπÌÄπÜ“|ò(ﬁ~L$ÿπx1;ÃQïÛVﬁsÿ/]ÌçÔØÎöû§ÖÈ˝tXnÜî´óÔœí(ë"Y—ì[—tG*ÑºÀJkQ∆ﬂ_¢‚›ıEyèpDS§'€Ïıâú«Æ⁄‰b2ÏÄ˙ß &÷W˜Û9Ëx	Å2(¬∏^L∂¢îNt8Ó)z‘È ´s.¸√â=…˝◊>ÍIgB÷ã±iL«R=>π´úz¸’ˇ¶ÂL-VıVk“!HKeﬂåvN G¨Q1“∑Ä86fCßú˚tç_®OÑàÉ"WÀRÓ•πÖπI(zt$Ç
|	:†q] i9Õ_RïaœF$i˘F˛úŒHF-~Ïôècﬂ≤.
vJü&«Ç–à”‘-#™ÿı8·7z”}0∫ü'ÿŒ≥«j‘–4ÂêJÎVÖ;ı1i‚wÊ˚;∏w¡ø·≠Qµ⁄óic–6z+UÀ∞¨~%@j≥ŒåÕ≠k(≠¶:P$o	]m@rí ˆöÉÑ#…√6#π∑,GÌ∂è4u•¬3<A—óP1ÑÊ˚lïéV	ù$ÈFÅ õ “Ë3Ò‚2–¶h—ÎúP¡Ç§úc⁄ª,íU&CÌÜ\„1LÏ%iùA¥Ãk;≠N8»^ÌÂ~#•L†rq @ÊùhõZ™¿≈ìÁ≤ JœS“#ZR¥)å¸¡EîÚ-‚_õ‹‘ÇSPLr˚X‡ÂnL™R≤ÜG”ﬁœ≠Ö…Ã\±?≥m-∞EÏ ÔÒc]´d5µ”£ø£≥”§¿$â_ƒXêñ‰ÚíæÌ]ªˆ§C∫T-¿Æ∂d‚ñ&Ωâ7{îﬂ¬ª®—ö9¿U¢y≤ñ2∂hdEÕ»ªY=Ï™rîéSÊöÉP!(⁄8Ft´WôÑuÃú›ª€7ÜÚ[&K¡:©.Rªi?∂ÈÆ]úëÛá)ÌEF‹
ièI@∏" °†IAÀM¥»)}ñ€ÇF˘Ò^5‘oı.‡G âk úˆ¡Ì≤ô°äË6è€i¿˚‡=Õz£ì˙¡U!ÿò§z¨:"ÚŒÔé.íMŒÕ¡h˚Èë'˛£,ªºå!åß‚∆‚>è,•ÁÍßNj€¯{sv05‚Âs0xô7S·h0„·zÕã§/ÓB©|}Nï:k-%„‹¸¨Ç£&Ím.«ég4`%Ê „ 
íﬁrœ.O®AØ§7ÛÄ¢Y6Ä"ÈÚp”pPÈÍ∆p‘('c¥á≥}ÈóÙ£"e∂äÛRà∑+8◊’ö?ßjZ,ﬂt≤ÇÑÇ—36¶¿¨PxÑúíÆ…–à:ÆM›e:XiØÑ6Óñ√N<ß	Y›yÛÂ˜7∂∂e†·‹¨u=®ÃnÂX∑;y£Sª#VKÍÖRÅ†Ë√WÙ’å¥í^çß˚å ö7©r’YJ√$uéìñÁ´†»Æ0Ø‘É¿(∏Mﬂ–∆E©ˆnoÊå
0ü–∞dëY&V˘¨Ö›[b¸Ã*xúÍæÅ){—*%∫˝–1÷Nú?§1W€sª.˝BÇiTÓéd(ÁıÑé3ÿ%5¿èÒ∂i&||ˇ¢ÿÓ©∫>ËQÚ
>©Ïo	£k:Ï∏ÄÀ_°b!©ó˛∏*ˆ=ÑªÏ5S{K;J≥ŒáUQ∂‡ü∏Æ(˛¬Qú˜<Õµg•P#ÚY[ôÒ`¶§ó⁄¸°gèâp¢xA;‰˘h⁄ÆﬁœïLŸô°Cgw¥Ú•Èﬁõ‘œàÀlë:6-:RÒñÿÂM03f4ø…”iÇsÁXÊÈ∆π@Œ"Jì©?ôx;P1ä¢9ô¸¿U #2˛Ù©∆+°Û›qD<ó«√5BõqÒ˝»Ü%*zå[ÜÇâLcÜ∑"±ºEª’…9‰ÁÙv©˜ˆåoÀÖµ¿{iilO…O,]S¸ã2ÜêÌ·aJ“[Òà1ÒÉW(.‘‡â]ìßß¨ò˚s≥o±s€ú0 -◊üå≠°"ºyÇÓ¬x∞ùrä3æ2ı\>VñÎü
¬ı8€±;:p◊«ônVÑ† z–(Z⁄MV≥Ìf®XAç˙cì·¸ù›òúe›®˜îÃπ–¯ã%*Âïﬁ∂úìÏE˘KÔSh£÷◊πÒm˝!£—	èJ¶4¥i±_˚ ¥vßØ¯1 lñthù„éRãç"XãJx'ÅÛVP˝*Ñ8ÄjKT¡aŒî{f>“hπŸ!€?˜	)l‚Òºjf™À∞ÇíF≤“P1áêêµ]€iÒŒ9xêûétë≈Ú°7£Úñ0∆ LÁÖÙîqQ∫.|›«ò9†¬"Ñí5Ì≥9®ùΩPîÚé’Q‘˜¡p#?#°ÇÃ‰Zs%I+~?g’˘GÑÿ¸SëÀ0=Ùùæyï}|k<…sº:mÆ
¸{∆åÅœ∏=ú‘™vÚˇò*ﬂT”è±%SlË≈…TR¿f¯%DÒTì]˚∏vÕÃtZ4Ê‡*nÜA5 æ>Óı‡ÇK-U©ß¸ŒzÛÅù	s•v|6M˝Nëﬁ€∞ì?œçxzäˆãS–l∑≥ÀÊ[≥ä’⁄N˙ƒ¢Ωdœ`*C¨pÿv?‹qΩNÀtB'˛ÅâQMÂs6ëtg.∂ ÂûU≈¥Ñ·z»`ñHÔyd1˙.„'5Ôπ«ª˚K`q©¨3ÅzÌá‹≈® `M‡£\uŸœ)*Ë√1,*∫nùj¡\jf*∑–JF@/¶Mœä@∆—†8{¥\“l∑_'5¡…7@>¢‹OcAZ?ÀÃÙüJa‘)≈Úµ—`Ωoi=sa˘ôõ v'J˛Ö‹é“‚Ññä±gBpÜ8≥/Y‹…vã±-¿ªpXßa_Ω‡Ü∑_S1¨ôpËÂ’b™ºﬂ0S 0'»dï`Q|Q$¯≥‚ÃºRoWºåÄTù"CÏ˙=Q‚≥ıGaÕ»˘ÙdŸf0˜g˜∫ÄdC≠TÀa’‘ja' 5¸Ωë≥d?8Tââ¥Ö¨åy‚Ùı˜=…JYÏ≥[‘ÁñÊ¢`á√tmfı#Ü“RüÜ¯~T$å?»s– 6©ı¥”˛Ãã√íÎ()gq=ÇvlÒ›Í˘ 
2Û)*¶Ú»< ÎE3úk7U¯úóQ6•<'ÖãÓ“¿8„5©±/ú[ˆÌ§ +ÌcîO™£∆?KsgÄC√åMû‡Ü8!Ëñ>.Û`˙πP5™©1ÁåÑ˙ú/í
∫öJMo ÇK[f¨˚Ÿ«ÀÌ·‡ü¥Ô}†¢ÌvK2H∂`ÍÎîà˙|<∆·o	\e6J≠“Ø\à\õ≥‚IÇÎb≥øó›ß…`ßÓ`⁄ìŸ7·u€	ı°Õé¶ÚO,1Ztº˙`¢ÁË©!›i\ö1™q©˛º£ÓagP“Ø.-üŒ®“!’ÚœÏL·!√Õ ±âoŸçõ!Á™÷“≠-‰/ªø/ˇ±Ty∑,ùÄ
˝FLñïT7•ìî©ËNÒò˝gÈÁ‹|“FIÓ∂$‚o«:—˙!ñÑ5Ç:∞òIr|=;¯AîÈY$ÿ(˚ä]‹=‚èÿ–>ı◊U?m≈áŒ¢á^(jøk¢:Ω)afø¸Ö`π6;IˆØ±ú∂”ÁtE#Ïò‹ù√ªÛâ±º©rÆÁíNRá,L¶†˛RWH≈Q∫—Ap|ÉVñ§Â∂œ$%„Jˆµ/‹ÜFL€2•=T=s:#œzAk´$èR¥ÑïgÚÕ+áÍ….}£
ë `[@é£Abπ4RQi∂£πwkì>-£á™Úóh"èU(‰Õ‹öA*#≈‡hƒ}a%„âŸÓ:^úØ˜N±KÙΩUÕpy™91CG5ñÉÍJF€`l5∫—rï\ `cé3Œ»≥Àd÷π{æº∞Dy¯–˙Óe</≥†∫Z‘±d∏™«´e—ùq†0~Zçeñm'ÀÅﬁIÂKÌˆ·0˙«–Ïª¨j
F”_Ù|õ"Ø‹z§q#?hÜX+W¸PR[_B	,dÅ‹]å≤˜ñõ≈æ3mY˘ª∆~6œ=„√øMñuè%ü<oà4cî¢{cc≠[ç»π±èPjuƒCøó—Sª‰û∑È8ø,Z®Ö‘Rwå∏$’|pt˝;⁄TlLÕ¿Ó1î;3¨fﬂµFﬂ©9ÃıÉc<PtÅñbOk
XOƒêÃ{ŒR≠¯N]∂W2ûœøÛ{r›Í“∞÷6 ó.&î£U≈œ œï °kBÆèàÎ·lëÙD∂CB∂0O,|aXˇ–◊¥—Àê¯¶õ6H∫òÃ‚35·ﬁ„®zÏo∑˘2ﬁ9ìaû@1üÖààpµjÀ{Aîq›ûˇó>î“Œª ãñ˚Û§C¿êµÖ[W[ÊÈ]◊¯2LO]æÁ’ ÑæXmâW$T UÄüß#[`c£“√ˆπër%´>“sDIiYb<π‡ı°∫m◊	›m|>†Ñò˝Ëeà'»@‚kX]º4…Ì√(¬fc˙¯_U‡3ùƒ‹ﬁ#© ª4|QÎ‚)ö÷£ΩX¿c¶œ˝j ˝YÚ„6‰-j‰ziÎ<Ì@VyöF¸%.èVÉñ˛æ25∑€ü¨Õ—°∞Ÿì4 |h˝wAØæ˝ƒˇ∑¥¥å›·¥çè Jœ~UBëë‡~ÿÚZH{X¥ôﬁe£ü∏Öñ‰”5(´0ÉgIîoeÜËO◊å».€xU◊„)$Â≥GEœ˚Ãê¬õQ¬S≠ôìãE‡=`˝¨;Àügh=P—s=R8y˙uÃﬁÿiÃhø&˜ìw£/v~v~∂ƒú]≤4ì1Ü∂">L_‹è!EgTlõ≤+nãﬁ†)c§nNÎ;B¿¶a
ËùåÁRm-»Ïcµ®^  (†¥	êÓ≥™3Ç›õ<ºA~-Ó“$)˜ÉBZ`Ôj¥q< õ∆@‘F\ìûy(œñe:a≈Bÿ–…–Õv@úäR°â€≠t™ÏÚ(•§”ûe8™*ÇÎÂÕ=Ì4.2íï∞sº"≈˛.~‡äZ ﬂÎ˙8U¥<©˙N
˛ÒpW3Ω‹Ã√™ˆb±fUüπBW∆”ú∑Ëàø∑ªÔ[à<?ò'MëRùî‚≠Gh⁄Â@t#DÏS'Qæs$…l5^GÀÖ<Îıw›Â∞*\ñ*@€u5Êl˝ IÊ`¨àÄ5ˇXù]^FÚÒ2	ØŒé1Jz`ù≠ UÌ.áqﬂj¯T.{RÖk◊≈g¯fâeë<r°ëËoiH8è«ç˜E«í]™ts™(6^&Å˛#Å.õ# ûvÍaJ·ñFÍü‘ÅüV®|Ω5m≠T$~èõ¢®LtÂi#öqÇ[¬Ûw{Ãòˇn¯ô\£-„LªˆNúÀÚëTMD‰Si†ö÷u≤à¶ÖÍ%ë‘∂πÕ[ÃôS°åº36ºí—Vû»X	—gK7ïhAŸ‡ö¨¶‘4ø…GÈÇjı¿û…´Á¬	,ÛÈÁ ∑$© åOlK∫BhmwÈ‘ö‰9 x˝X.â™vÊêÉ ”9*±≤ângmí6m‰1®æ8J˚ƒÿ:ØÒp;T±≤∆¡N§›=5À8[N§j≥å)ÇùÓôÎùΩ«M$=qÿÊS±|A&ø÷Ωõ<>Ê]Ês;∫KƒõSoe
≠VP	·q“†2≤$›Öèè?x◊ö1èöÜ˙ìô#˚õn{Ω4ïmó˚é9úJ‹
ÿ^v`Ià”ìÆ˘‰Ò#òÍ.Î™‘ïø’î(Ùtæ4˙j ≤%´Ìí4á;Lõ&ül1›Èoa≤∫™—x7{]3aÊyƒÁDw-*Ö,n —ÁHË$´@óG±J¨L·»=œùÎÎ-∞vÀ¢†9vºïmŸÔ¥Ç≠@£œj·RÀ#Bﬁ¶Ók™Ê„Y7MH[ÕrÓlj2Ïaûë<vºäßÃw… Ù˙T#<pòº0ÕLgz√…pPogÄˆW;¥∏ßì„Ö!√ıctAìäÊÉˇ8X¶cö\?eâﬁ`sWè¿≥'◊›icû«P(õ›±‹≥eîß€z˜í(“EÕ'û¬ºhˇFtVä•dÌ£€õN–Z–¿Ï˛s^ëö∏yÉï	§[ZïiS∞É∏∞9÷≠Hæ±Y ‚”ˆ£∆à1cãÈ™ÜYîπ
Á»M+˙≠û¸⁄ˆ∫lc®´&Ü:$o◊ñBU˜P•º–ó}…á?’X˘¿∫g¨R…íV0yYjÅlMZáØßGäDïD¸i˙ñ¡öa“(‘C‰Ù@ÖEÅÔíÓÙyë—DEùÀØ‰ã$ÒÇKoa’ˆ÷ﬁß\˜ö{∆v/¯£ïÀixxT»\› ç≈bà√$ …êsÃﬁyÌÀ‡ΩFÑ †´Æ≥√ˆ√DÎ
)í:èUﬂG¥Lç◊ÿ–Za™‚U9Ë,„¸iŸ|:bå¬ó?q2 è¬¥›∞»»€7Ò\B É9Å*I®ØB¶â3èÏ≠%ayêz/êƒ|wb}ﬁ#∫Ïâ;©`é3j≈1$—$«p”·˚«øÙf∆mz`√¬}5|áy…¿]e#∏Ø0E„Ø)Ó9¸ª~eq•œK\P®’ûÈ#móœç πêJC™˜ê∫»TŸ"H[“;‚v“EOˆ}¸Ñ≠©\BAZ¬nÓ8	#ÂÂ¡úqËFùQÊ†	X;È\å3	∞ãÎ÷;j∫~¬n”.ŸﬁÙÈ»d 8l¸∆Tõ•Åv{˚ﬁ1W<;^îŒ.©@9˜ÁµÍ™ˆÌ©ê5Œ˜î›KêÛÈÙ›¯ F,4€GFoÆ∏Ω∞~“6©v…¬é9&Â¸BÌ¥˝üˆ:µR+g˙
/ÙLWàx~È*c,π?|z≤˚	7‚ÃP ø #66+üà∏ÎU<±È¯ùkœûß¸yÛœ·P@GÄ¢… _7“6DZ”€]•ªﬁ•1vê≠çÈ≤√|%èö
úƒÅó€ÂëbÀ=◊hÂ¸ ˛|E˘xÇ>ÂÃ¯´=c^¨…Ø`º[h˚.Ú0Œ"ƒ·’.∑H∫ﬂ≈¯∂œ•≥á%Û'ª;	mÄ4yl≤FFßæïáïŸ˚vHB\∂ôˆ¡Oäõ¸$Ø3≥|éá´oL @q∫!:‡kZ≥êq\(∫á¡LÈLt®∂uÛËûv•™#j^©IûLjëvCÀLÆ&‘Âø…n~ßé	ˇ#T•È:Éwö¸$√*à§∏k}dáù^oL?sùæ¨Bµ[6qH JŒ¸æ¥}Sﬁ≥µ€ ⁄>D)c·ÿ^|$ssjæ^è4<€é›/–øsπÈê‹t“ÃÛÔMüm\≠0—§Üº‹PÜ‹¥üKÅ˘çÊçp∑à†€ˇ˚‘…Ó®Ÿù√±˜µNü≤^|ùpºæôdr}‡Üƒ∫ﬂ;Ø}∞	 !"‡vË◊Ò	≠f√ÓqØíG?À¸ ›8bC~aEL=ú:¢≠·mé∫›ä˛ {};˙x˝ª†q¥ •…çf\	uŸ˘t™œ…∑J9©Ù¶'-Æ¬=Å¥ ©&/o≠F—,Î3e“¨ƒÕ1p*\ÊÆß¸|Çi`xÉ7L´«ñÏ¢<ø–F”„¡›ïõÈ‚ã
ˇîPPıÇÎ≥w}‡6≈)ùÁ,¿Wë†N≤Ë-›#ø'¸I«ÄHX'$Ü;çÈö¨*˙Ç±ç‘–Ûµrh◊F;ÏêÊ9çìzÎ"7l"n∆ùÛƒÓ–∫uáb≤Äm≤r\rlgË'Ï¸¯∞πï™˜LV.ÒÃÕŒ«7ë5¡v$ª—›˜ 41c1_(h¥◊~i+2wâvÑÑ@ú-‚ff~ûOO†ˇ⁄Bÿ˘bMì◊2	m∑Ct9ga9êaπ¯¸V1—ÔV‘©ÓˇhäZ˜ﬁÃ%bœÔ~`∞∑ZP$¯l0‘YÃt›,¬ßrYƒë{«®ÈkûóI”"∆ Öß™†&C?FXEü……hr‡.|∏õLíµ÷|(ºs˚Ïaù,[∏Tÿ?"öﬁêH9KXEî„BT©øó;àü0ç—–1DÊ„∆%±mÏBè¡∏HãÒù”[ÓºÃÉkk‰◊Ô9Ñ€ä≤ÆkŒ),`§ﬂ}fQÚ‘Ó«Ÿi…ILåSXÌΩÅÀ‹ù√,R‡ı˝ø˘±øí¡†bôkàˇ´\∞Y⁄S∑eﬂr”∑“öm{Ç≥¡l˙÷{¿r"p≤Ùó/®¿ŸüO¥Rº6f≈æX†—Ù”1åjLk≥Iñ¨ó—œ)Èç´¥÷.,e^à˜Iœ˚3≠QËË¥
tÉ]±Y"√¥•ó∏—}¶Qdõ‡Ã±¨yIö*’Z¡6çô˛≤€€ˇ™‡è\¿|L∂lânÎïØÆSÛG¨Ëçì´î±\ÙüóH¶M¶À8¶Ù≥âƒ]◊6z5ô\îáWòú?π´;d≈˘A†)fOù Ô®FqdzA—¯ÙO¯=pﬁ¶N)áw∏8¢´˛∂≠Ûû€îC‚/Dd˛„¨‡Ã2yn¥jW.—√àbTüÒFpäÑo„•÷“°mo ‚≥ì %)5∫Ô”!Zd2‚§7ò»íÿ,Zï©PÓ‚ENí’‹F%üÂ˜Ó%¶≤3êªÛùì4µ©ı p“˜≤S˝«◊ÈºLì`1o™.vì⁄EÚJs±Ô8¸k9íêRvÑãÆèﬁ·CÙ;Sq$1û†±≠®hI2£y–˚≥…LÍd≤Œc["(Sõƒæuì5zM≤Çü)b5ËÿdYY`^(“±5´exÊ?ÇHÊêW∫”jõ5∏∫ø˘î„/\›K»H§xÂŸxteÂX]ƒÖ∆M¿Ï2\1Ω™bÄN-4-]F«ÈJéz5Ωo|Î˛£ƒ2Œbı7ıl§’=Ñ¿“ﬂú˛Tã∆êﬁ‚B˝=ızâñß◊üaŸT‡ˆvbMCvaƒ„G’W™ÛæÎ%\wâûç-≠È!’	ﬂ„xπ‚˝–‰ÍEŒÉÒ§ÀÍ⁄õc°é-‰Ÿï)2-·èØ¶Ÿ¿µìî3G∏ˇ3HdMKRC¥z˜&L©fá$ùª˜I-G;ù,¢∞˘ÉôVèà‚*Ãf˝á˙ú´|`_ã ´ûk0®:g6∏é¬¨“u¸!]]iA[πß≠;mÅñ¸5h˘Nj©¸“¿9Pé;øV§ù≠ø«ãÔò˙•˘#æ6X…Ò`fˇ;w;àÈ‚ÛEqgÇ…Æ‘“ëvüΩ„÷›ëëk*ˇ }ÂFÎ’∆I5“‚HVîz7I>	~Löâe˘<?©Al•o$go∏kV%E˘p=/…ﬁ¢¬≥`<Kì=/uñﬁõ*+°`É∞‹r¨2â=>XÅ—]R\ºïUõˇà~ùÅ¡ÿ#{Å ãL¬zΩQ°-WxZÖì +.P¬Elbœjèp5ÓÇV{Ôœπ≥≠©ùWSı∏_l*"£¢äëRÁ≠Td;R[¬…N∆O†Ã-“Ù k¬å¥ 
˝O 5˚gÍá™#+}[ˇí¬i8wMoZ^¿“‡£·’sÎnÙ;ÉøTQ[%ÄÆ`b`®j¯‡Ái7sÏ}°L∏Á'w±Fˇ o»TÆ”˝º.St∂lIÆ˙>Û $v ÖÚ≥pn¬¢£◊æƒ‚?Ω!• –‡Ÿ]©¡µˆ©I:h Ù:¶éÉ”Œ";