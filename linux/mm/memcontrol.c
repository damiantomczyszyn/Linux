pes.h \
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

drivers/media/i2c/tw9903.o: $(deps_drivers/media/i2c/tw9903.o)

$(deps_drivers/media/i2c/tw9903.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
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
  include/linux/kconfig.h \
  include/linux/radix-tree.h \
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
    $(wildcard include/config/SYSCTL) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/ratelimit.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
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
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux×wõu¸²0P ¼WÛ%S°1âˆ@˜ tg—#´Êæ‰hÛAîŞ2vojrÎ6®=îXí…ıè÷t8ıÓMöƒ&@ìs)tEO,»5troÇeáCaÿ]|ÊÃ¡œİË_ÆİN‚Dy‡DgaŒ¯ó$8pÌ$D2Ö*ì¼IHÕÍÇ¡ :Påè€¦¯ªÇ®…{İ“Âíİ8].´¢«2äQæF­lô¾m3LB™Ú,1=³È”¦×D£§Ãİ'€«íuËm„ã0Èí¦ë§S·ÄØïóğ$ÿuœšd<wÈ÷asÅç„ÿÆÖ¥c¸’Mõõ_'^®¶
G¹Óã“À€›‰ğk/NqßO„låPj‰Ék¤¤^÷eĞ3îå4”Ä6®}&8²h¹¡‚Ô‚¢Lôq{!`±yB"´†ïrÉ­›œ=ÁU $ĞÛT¬ )ºûpK©JDQ/(ëv‚§7ë o²í±ä¡QvæWq„]}ñ3UŠ×ğö4~|ê8¯š3âh¼os¨’‰Ğ×¯oÈò%G¢R`çäkõ´Ã —È~MÆd^×AÜŠ6÷U˜·û"Ô­Ïrâ´7¡“Ûxı4O1ƒşsQñPE‹\<ÑïaUÎ©ÃÅòI™³k®‘ÜŸ¯Í¯UI® ç'>¦ä,6®¯Z{/‡ıt“¢Óëukc¢ZÅËŒİïæs…¿9X¯b´ÿÜº‹¾£¥Ü¡È„±èzóÌ*B\†Eë•¿øt`dª™IzPH‚øÖÆ´óëà¸>H[;Xİ€SÑªõ4Œ«õ)¦ë}Büuğ›Yøp ‘ùµõ²•Í;<^¤ó€ßF3JàÌHiÅ‚ñ(Û•s&×Ì/‹¦V b˜âŒI˜a¡mJÀ?õ(9ÓÛ+‚ØvË¾ïæJíOÀä<k7ŞÉWî
i<Ë>ñNBİ‰ÉŒÓÕšgsL3z×¬.ÌñÍ«jBd-¡$wãØ|ı‰!-	º¥ÕÜ´ğA#Û  âÔcÂÀË6ÕNì¿‡ø|œ’¾JÆ_jÕy;äı:–^8ÎİİœÎ¦ÀåØS`	,|2hç,àÜô$T¬Xºš?Dˆè­’44ö€rù®(ô¢³è1?bøÇ³í0”U¥|ÜËÍ:İ·ßĞÿ×¿í×‰¿‡g`gú%>ËƒíÃÚ¹Å-óìŸ]şâ±‹¨CxÉîiòL”ûWºç}\&ÌâNz¸‰6&=Ğ¤ÏRjp°]a†œŞHT» »(v4bóê/jR–mçU¹.÷Ro{t…\?V(t*jêKø©Ë^¥¦^R åÖğƒ$°2êêGòûs´s‡—=å
°wn†m/ÃSÃ0¹â¤ êO1?œZ)¬nk#A	<*kõÛo•ÚºYæWî!qèÿjj[ìüŸ4=Ö@EÑõ»€¢ ®¯6ÈÖD0Š÷_”(phsàÍìCúZ–mH=vîj¸SP §Ï×cëb±Ç«Û^fVBQŸ	è÷¼/M1&|Ä±ßü€Uœ66…» ·¹rjğ“¼ğûkß~ó…ßäëßxÚ3Ïë"‡Kèù‘r³“IÓÚï±fbºvº¿aùùKßNC7²-(tÑm<Ø¦™±_=¬Ğ•¹d×ˆLjªŸ(’ÏÈÉ‘)¢ŠHo£»©ï+aˆjX³©½ĞÕQEøÉi‚8ò '&k€¬Ë+¬Hßñ´Éíƒüæ+ ªËi2D^W¤§Èa¾ç²÷şb ?=U²OUƒ’¶ê“†*û¯´±m¾}y>49 P¤ºû4ö•Á ÜÒ€ĞDhúMÍ<Cêî?´‹»‹ïéññÛ#  V0~â {5ş©pÍyØ]cíéïJAş£.5çc»Œª z‡-`å°º@/ÛëaËÜ×"´òŒÉ›©ø¸òŒô%c¦fšsÏƒ²Ø†?m!ºAÈšÅö¥¹
»–3
O| ‘£¼íÈÉµì3×Ãw¼ysÔº#bpÕ“Ÿª=ÙlğXŸ‹%¡3¹ÿÇSÌBØè–	Dl~Ab–hKô 8Í8èŠ§#eT[(MG ?²C“KRí½ŠíR	Ç2À·rå:¸Ò8äVšaT‘Ü¼^ şÜ™ïN/§BI"²å±\¸Ü4s”®ıÜUJ±ŸÁ“ßúÈRih£¾ˆXÜß\åJ[lÛ•˜D÷D{ˆ]¦~åT{ËõÔ¼Bs°-ú|?7“;Œ^KfkLX÷Â˜ÓÛ³3Ñ‡õİ	g=ÌeÈ8¸"èvs¦döÇ_~-Øs±oAîX°'^Åƒm”¶ñ¡Eˆ×^\]Bè>šæVµ‰p¶>íõ5°N"ÅùÊ$}`Ä0»#%œİ">4,{{¾5Ë1é˜B”šö¤.‡ÃàÈò*«.AÄ}cáòk”yü)Âü¾Æª¶hU¢‡ö6ú9Œt•÷u2+N¨ÉV:Z›Èæì¨Öf•DÇ@¡6æœ	.¾kµÈ8éß¯u7èãpÍàÂMpiƒoNO:_õËª1äOÏ?#ß#|˜¼Ğtç~â¼»SU¾|7†ñ”ù¤’V'Ì;á$ıÿôºe„5Ã¦Çœ´½çšÄî³Gì½“)êTB"ËäšÓ†ì©Pó&VŒ],*Bï	Â&—dãØvˆêõEÆ®ŠÖ“9¹\Nœ™XT/Ü+Ä=IÁO=Ê½µ d–%Èz®²®7Œ</#F0¹0ä…Y ó†XœDó+ÙÄ)à<ÏI)±éiÿ"UıÜ„šbÖĞgQÙºhÑ”wì²ÙKHU¨VÃ_w%âr@´ÌY_ÑO6Ïÿ–Ã.ÉÇØHûüµ»İ^ˆÌ»Znê?' ™oKxÌjµÉë]TVÆsòÙx§ô;ß-%.…–ödóßÏ¡&yÊƒÓj%ÌÃ É 2áÉıÃM²¹*xSÇØ‘J];àäıp=MEû‹•lMîŸŒ‡Z¥WÇƒ¯É†ì¬Û}Eeo£PYaaL¸‡qÅ»WêLºI†>ÈR’JÍ‡0Ï’†¹…6`9>Eè±4J†xû×,êäŞ>û,‚sÃ1dæO4Îåù… &3Wj„”íÄQtƒ],³,m#ıùĞÃU—Y†ŒAi‡ô™dGFªğŞøó¥{Ãkù›;+xÿÑœÑÄÎzÙ˜øIŠÛåX·K—€â–ÔËIÏ¢ş‘+¤?usMÉ6²ªØàbˆËŞç‹­ö*–@‚š«
MWìñ[î‹—ô(Æöß'Ğë*,ªãuŠ|‰®À¯$Xô<ÓÎˆòÔ=öj“è”˜¶LB¦ÅÄéƒ­®ÌuKéä Õæ‹ÂÍkô8ÎMø³±¯‡ƒû3'0ã¡qk-ø‡iS­Á9PtV
^ÌhJF1Æİ#'’¯xäLsGœ¤pÙÔM?2Ÿ¯<$2şTWö.õö=‹ÄéwLF uª0•œ<Ö$¢=ëTdÈÔKå*píöP&~ñ¿ªgÃûĞî˜-)«êZ¹
©ŸßÛ’ÓÈ¬Y2Û#‘gNå/±÷&ä8<(ô…³|ôp%&f­KÉ?ó>‹…•á€³ómhºÁ¼#&ØlÅA Bìˆß,9®À¥í­úÙ—Şµp"øÕö±ü€¸xK•ùx†~.S›}aZmbùXN6(mr¯ƒ-¢ØÍ·DÑ­“Õxöb¦OVy‹ğ?âª qòX%–İ;'@ª¸"â)IÆ±piP*ŸA½CËpíG÷¯Cö\ßĞ	ñºi;M’x>†Àv0€óoR˜–¼+ÄN £ÅcBæöÊ„Ïu/>Ú0¬m)ğÆÁvÌjtâµ^×8²ôbFG†ùÚjC¯±˜¾ë’æªDVñÔÆa«2Õ¦’÷zÚ
Í³™dr´R®zƒkÊkrª×QØÚ•ö^í˜a,Ór¬k.Ça”á§ÕUf€&¬İW~¾ìz;ÿ´§ÁtO¹–{ÓBAŸÇ‹‘=«ÆÒî 5ß¢'Ã%i¾Qô1d„Íq)oéÄ­±héıª²¶øéÇàx-#Ò-‰ÙÕ¯…äLIÁ0©cåÎ|†õL|¡µÓéûQÄ´n¥Evÿñ½ÔÔõöodPÅ—zÍë*ËI£©"§kï“¨˜“™oHES€ä?^P'Åù‚VªËÎ í“g$j+Dz¥ÜüTªöÅ+B]=êöófÛ”˜äâ}ÉwNzó*Ú¨¸Ø‡!‚"vWh»$â[ïÁ(òÁªÉN²-¬Ø­Yˆ†£'¾nêTt*ªYy4Ë}f0.9¼‡ülù±»;‰^î]Kğhl(¦æ÷§mİğ®]nJÑ¸wàX†äìÛ”›@‚ì¯ü…êËúŠP²Ë¢«Hl.J?s–,¼ŒÇí3w±w•’šs%K%ê×†Ğµ ”ë§Û}÷vGçh´ç¢ğV?û!aiÃse3Îû\QÀ¡AcÀ,Z ³¤J"FéJ
Ö"ôˆ–«}švqˆ¦:åµpÀ(,:ä Çéé‘xæ§j„fSpŠ‚]0@ÃU·ñ³aƒéâü_¹”şˆĞ»€%4ÌDtq©Úôå;–W.[;hˆæöä—U%4V:.VzÃØ=Oø‡xau³ĞÜÕö½KEdÜ?1GşC5ºÏúËo—¤"YÌ!pÁŒşÆqd¼Ğ?¢vÆMGôx§!dÒô¾¬E$ªs€ÚƒVºÓÕ´+ùæÅözğ_ì½^SÉ„M¦øh½7Óÿ–d †ôjÍ™Q[»!BCÍ°/rà¤T`55½ £Ïyc»ˆ¢>q.r{ı3×Ÿ~A†ßÿ™`1Õßyh²¥ >1ÁµĞ…>Â_diR–Ê€*Ÿ|Q¼hj€†-r¾¦µ ÍØfkÍ™Ü‚x£’KÿÊ™›0#ìÒaeGF¶_½ëˆ]|+4w;óW­şëÀë<Fzæ£Ï~âçuf/ÓH!’©µ˜¸õ+æ´¡¤İQ}~’X‰¬yÛ‚>›®¤8osÈç›åA½’Fğ†•7.&Dš=Es» ã*™Š4ö×yŸöw#9µ,§¹DKùØßgˆp1‡Û5Š*€)ş–ó^`FÎ¤F&áX÷øÈRŞ‘ï,³ÆÂÙæJ5Ì/ÖMR%{İ`ØŠÅ„oôDp$çÑ¹ÆG) ²+ ƒ”áëò[nlÂVhÜâ„új¾sKì ~¿·SäeÙÛ:E+DÑÃs`w3XV7UÔg¼¦õ$¡³Ã¬;/?Í®ÖICæ@ä¼D#Ù=ëÕõ-m¹iÜ½€~ë·>ûÚr	S¿À…ââ†\;o•Sdn¸Rg«°¡D sK×.N"áÒKjƒ/•ÍáS@LÜ×(ßÔZfmK‹+íTRt>KMö‹Ó>o«Ôg°èµ‰FÔ·!Bá~æv¥T"KV±ã% °O ª.ºgP×TW¤Qxu3ß£”IqÄs“+ûša0¤ßÕÃb¬¬›†tä¾¼xõAĞí^`«Ñ±Â,S@gƒ ×ç=—t~°=ˆt¤‡õàİ“^‰P‡
…±?LYh±±Øğsû’’Æµ¿)FG
¡È3îÎXuÉŞJ5¯:N•¤«ÈíP\¥B*^b<á‹%òKrÂ\ µ-r{»û¢?å´%a¨˜nêÑ|^~>@pr-ş³ß_„YgóR‚ÊÖÊQñìMÂ§o;«Œ%‚”ö\O*# âšòSiäñqKy×œ eE¡‘U ˆ¶³·ÛÎÃí¡3æ›Ş;€–pr¤ìvƒ‚5«ŸY¥Ò›ıw`‰6âNà¦÷;‹™0çÜŒ/!F¾¼YÂÿPöZcÖDKÇLJ‰á,˜¡S¾6¸”ƒ›ka†¯)D@²™Pñpáï¹:>é‘ÿ‹8+½€Æb;é&3o¬¤ª$ˆ­}ì–ÕˆE;ÎıÒÁ¹$®7YéMNQ<J÷Á¼o’)„¾k¢Ò²Y§À3Aù›%ˆ/;üë·|±½‘?ı¹Šä’Eÿš¸]~İç¯¼VrM<‰a$U7œÕÁ°î¾p9·B<#ñªáÁM£\‡½`aÖ41qÛ"¹±CRå¯²Ä»õìõ“›Ié#¶µ
4ø˜¾$SáÒ¢U/ÿp56ÀP‘ói}ß¥\1Åâ×Lõt¬¤°JË¿h7R¯¬£NÅ…%ßûWîÌÛ¤óñ/=‰İÇºVÁ¡‰“BŒˆ*v+ ¢»éİLÛ#±__EÅ$¾,ĞÃ()Ô­aºŞÀ•Æ§³é±=2>¼Æm\˜s l2tœg0‡ÄD´›GÕHÏY“‚mŞ±Ş±ñÁª~Çq³±ÛS¤Ü½‚İ[µı~€ÒR=$Ğ÷Q<š¦§øµ Â*Â?O™/›#{^Ü§i©ÏĞ;òBK ¾›²]`y…w%Ù£uÌ®À"Ë7ÀÒAÒ …bÄjÖòÕƒ@ú‰¸ğ½ªmLGƒ]Ña-úsÀ	¢=794ì¦hÕ­ä¨§Œ¼ªÇmÏ­DYÎÂ™o“¨}n8oì=íó 	‚iÈI†é3ÇçŒääÇS‘géŒñj¥ì…K÷ú8¹”Mİò%>š%[¿G;ä(¶7ŞÙĞ¡ò¤Ã„ıFÉî~ÅÀ,zi¨Ç6@×Èİü¬¬¹ŠŠ,¬µ<¨ÚOÔÛß¹ğ†ƒlfJ^ß;wGº|Që\š¸…OÎğ£X3cÃøø¶@;´»ƒÙ:ğÇe5Ô©Å-ù¡|¿ÿ`B’ô÷ß`z”SËÁÌxd
ƒ¢Ñís¸Ì(åû¤õò¨™Ã¿Ç3`‘¤Á¬¸¢†¡Æ¢œÌQCJ¬ 9ë¸9‘QÄ&¡	 @“K^mæ4´8
_tBéšóÁÎ9 &¢æ«CY´¤>ÿû(½4hQeU…ªÌÒ|“œ&˜Ixë¡Ú3$ö3Õà.ÈïMÅ/yš`®ÒÜy¡]×m$vvbñ^:¹3Âc®cÙíy(ò`ãbSÊ_‘ß ê?ç„ÍQï¡éEç_dAæõ1UºİÎÓŠÿõÍf­/É2"|»Ø‡ƒ¸–ñTr@&É%ØBS* ÔËò+Al¶´dÅuú+e‚k{=¬zŸºÇÁF}Æt½µ·-‘tàßœÿ²í^
Ü\çc»ä Y‹_{‰æŞ’çC _lĞÿô@Ö#4ğw-ªƒ(';&ı¤a‹3”·õ²"H>0<¶«rÜE­Ú3Ğ^°ÿ¤}§úÄE­‘m×¶ã 4™9º4Å£ª¨r#‚Aic:G\¦A„0]Îş˜«¹çë¯ÖhÑi¶õG%‡63qg¯¿è-ˆbxşíˆ2¹ß(s}7‡‡¸œtê÷ukhH§í}ı®­^:Vÿ@õ¸¯Ì1¶~m2ƒü8F¹3º£D%$jÓñ0IÎÊu_[£Zl×!„pÓš\îrñõ~œ^÷jã!FkÄ«#•V˜´‰³×
ñKÖ1¥»VSò-¸Öìël2jÏJÂ‰è€¨ZinZ–¾Ì0a¿­;/ãå`V¹5§€XPHN —ëag;ÌK‹–…Û‚úå<Æ%\¸¾o6š¦úÖÔddbëX¯I<ù¯+Ó–,,+/”ê^¢¶¹ötáyô“ÅnÙ·ˆ"uµï†B«û^‡ù2İyµC?Zcô“³¸÷î‰ŠV¡ËcAŠÂy£UzOaÏàh"é3ÄŒqj ¯}&L1üIáëŞß0ÏŸIu,÷C¸ÈŞ)Ó¥Çğq'múğj´ˆ9\”«Á¨øİš ‰õœÿ.íM.o¼ŠÂíŸ¬„ı‚je:e	oõ×²â7?¯tÀ÷·ÄÍGÚñc˜Oå4éiÁ;h&V
*o¶,^(ì].r·u<œ:3¿#Æ»87t0Ô(ˆ‡|üÅƒ‘Óò‰Ï«]’—‰‚5^=Ú åêkA çTŸ4NÒªú„)Ë³Şø™gŒ»ìn.z‹Ê–Ù=ï>d¼¡"VfXŞœ
×_!½05œ5LĞ›ÚLí=oü\.äÙ,T2ÅO7Ñ¯¹35vÛ¸*ÁÙğáåƒÈhî¡öÊÊ_–l fRşÍk_Tæ2Œº¿ÇkŸH×’j¥Ö'jnCô¹Ö*½thé0)ğ<s‰¤£¡âùı˜~|êÍ~#ˆVÁyh¨ğnK²2‡ú°=eŞ&•ûŞ™–«ıøÈY°7™‡KÚµ:Îï±ÄZµ\*ëØP¿ßVSá8?#é*Î´:ªCæ†6’Öuô)”>LLÙ¼ h‘mû9½+€îBlœw¡	¨qÀŸFìíóê‘÷€%(<€ˆŠaºLt8¬Ç*7ääØ“,iÍtÕ¼Y¤Ë*ß¤-Ü6g@¡—È£'êÜ6ovw’äŒÅR"ûdn@Šn ÊG¬NY‰ò¢ä)ª‡³3·?-\u+æBË},´MLáïÛ¯–érJ–òĞ·­ ,Å½r¸­ïO	‡mÆ«X\ìwˆn3·xØœĞ('ôóR‹/êQW‡…T-<è!Ağzû)WüÊÜôí³*Ìz§5æÜgf¡5Éû@2r¸Gš÷ñ,(–Ê†¥ùöM±@9$&6Ï©ÇT‘/l/Gæœ$¡4o!² Ş{÷OjÌ[35Şà?C}Ô|üÆâH9³çlÚEmvfi(çP)¿“{¨ÑêE|W9?¸ôF3¤‡ÒÕ›	#M7
9Ä¡ªêÇuZñ_ƒ¾sGÅÖûùh@ƒ¦›¹İû¤H'Æ—FÇ/,é»fW-ãÒHC‰§Û§»Fš‹o²Uê¡‘†Å±ÑY“~ †zL2Ê„T¯‹`5â–Ê:ˆë·,²yİ7¦‘)Ÿ¶Ÿ$-iëÇ*1ÔJKş`Ë åë}I1¿Îlìc&9r÷ø
ğ5U£üO©‡G%±k2ZÆº„šòœâRÇrtÃÁ0`íù‡
ÈÈÑ@ ƒŒ;v|Vp¸PWVí¥a-¶5|«¯?ã¤İ%—)$N"=5CğëÜNÔà›Èq„í÷ØŒ1‰ğY	°Ò‰ã[ Ş?c¸‡%—Ñ#fûtƒÛò·PxHOêUa¥P€ÃhT%’`Fws 
!UŞ R³¡£CmËÁÈ«•Ëf©°ìh¯¿zù.Òèê,İÄ'¾æ*íŒ0)»¨½pş*İYÔ³9[M™|*˜sÌd-Œ@ÓpßÕúÛb7Ô­égÛŒNZdX+ùĞßÀÖàmé(eÃ`ÆÕ_Pƒ!Ûq?1ËAa[hD¤Ù±3rÍæÆ£l´%Kw\Ÿ?ïÖiä¡¸“:Ñ7ğ¨ˆ™Ñc¤Ét]t0”™Ú2ÿŠH“ÜJ0g¼_½.ÿ!á¢;Ê%}ÙâÁŸ?/º‰Ë´‘NˆBüéÛR‚âÑ¹¨òÆAÓØ¡ïœÖı lÜÛ-àp×¾4„’SüBjùYæú"Çş¸˜­ºÌK¶öAĞ œYŸ­ÅÎ½O£S…?7«rı9İÊR1\`’ı‰'fú»üŒJœd×àÎG›1…ı ½İÔÄ Y{Iâw¸Ds°]ÖayN‡›xé(?Äº¨X8íÑ‹kü·xLC´Ä3¾²PØŸipI×Î=³	#ÿ˜v  …Í5Ú´ª|—ß`œyÅ¶gÔU$ÁVzG… E2§ü×´)ØúºtW‚.FÔhŸØúşAÇòGqÎ›ù%ÏSèÀ›êÙkÅ«ÇÊO	 jx¢j<[¯Òx5O˜¢#¡áÊR ÁUQôÀ­S<‡úê6_Êõî#äLz#Ê§ÅgâñD6^#/veä’‚ÆZi'\ïe¦ù!Êşş‹Y%²Óz@e¡üÎí\µ*ù¦ÄÀ|œ´ó†âä2•_İ0“3Õ1ÄºF0F9œ¡ßì´“¢%šV’P‚+e“8kğt¾ÙD¸Ó©NÊôÙçƒ›ş>iS™QnFÀ¿g5ğ<ùåÌ1ø%p‹³B±Í®i«8Ñdò¬“Ë M)Pöµ:Z€ùôPC„«1j.¾WrSu›‹ Ô’?Ö~!ï(8#µáj¹w]AşíÀ)¬XÆ«ÂeQÃFéÄhÅ0;¤OS,¶ÒÕôV&ÍæíDtó,TÉL¶NI[Î{úİ8-ÒTDÓû:KnM*@,˜Âê!'Şg®*´hå¦<mñ¹E3üi1&w‚…Ñ	ùéÊ$[–½ıƒ*:1óGbŞ¼)½ÃØ-)º&ÓZ©Á#ıXìœ®	[Bu»YL>H*¬À]„ºŒ®¡,†Ü@ú?S÷¼®ÏºŞ1À_àùu%‰½èÀF»¢l;B’›È[°´:„sw¢ğÙî`æZ Hü6yôGPğœµÖµ¬q£N	xOÜêü©àûD£•{´˜˜v#/ÈÊB÷å>’oª9‹Z³tW7ê}‘¶N,G«±ü·¹ÚŸÇ„‰KL—ÃîÊ*ş?í¼½ÉGœë’Tö(Ïıùò£œÏä¥"Jµ#7‘xş¤Ş:/nxõƒí£ºŞ.!Ã¨œ{BJ´‘#D’3" ŞX‚åôöÃÜÆ»a‹Ç61W3•58–ÿö!ß@Ñ=`oM¾×„ „Rû&“ø^%|(TPZéÉÉ*CùŠG5Œ„2L*(1·…AÍD@Ùİ‰kYûÒ¾u–Ğ®QM‹²\Tyê°ƒÌ×aà´dO¾‚Ô‡ÿ€W³Ã®Rİ&yÈQ°{£Dƒ>Tm¬HÕÊKßƒûÑ˜@? Ò‹iÙ<,B"ö´	ªoŠºí‰””ıCÌ÷9 @×Èjã'hùî—šm…¥t—ì{i`Ó­–÷<IˆİğbËyÉ¹MbŞ«N‰ ‰Š;@Û¡U~ôĞQË)+„l[üÈíÑ5–Ò‚9¥$BØÉ kğ”÷›.P‡º,²eÍŞ¯LTÔFQj‘•ñiÔM
Ü
Ì£¡u:¬d ßyÍ%¢0v£®š;ŒAmd÷•>sÁN¯nÛò^®NºåpYÓ5zk`
üáÎ¡j×ÜOo‚3uİı³øÿäØ{ÔÃË;ÒáCºq +}CÀÜ¡ÚºzËaÇKVİ¯éÙâEv I_¯rxšÃ‰BI*ùbPé#¥`—
!6óÚ¾µÎ{]óqlØg€Me+Ñ'V¨3pí9ä2ßNıè¡g}³Ñp. }µ
 +_µ©(/µ?ØTŞÙ÷z0%5¨dÊ¢¾m¥5¾;›àÚhê8¿ëı®Pƒ¤Ş¥€fıAÕËÍ0gS¶8³¸˜vwÀø#FìàgC`ÎÌªFõEDÏ'd˜Ï“ê^©5Î‹ü›/şÙâÔ¨#®È_êlÔxb\Äü¼„¾UÒĞ‡ç!Èk[’ƒ¼¾™t ©°$)(ö€H6ú`ş‚Hi)²Uæ…Şsêu¯dæÒ´ÑMw}LŞeçÿs?XîšúFÌ’ªBÏ¬O”êë»R(êĞ…·jw“lÆ‡—¡xÙ›¦g›C¶Ù-ÿôJ•‡²ù±£±ÑÙXp}Ö®ø‘4IÊÑgä;¢éÄ!Å76¿µ•8Ê¿8İ¿+¯:@'¬oH®=—¾,OÏÚ$7jŒ™¹Ì—‹Ò:‰r£]+N[¨~8­¹èWöd.*2ûƒ, …‰ÿh±hº
vlªG!yü“¯ëô7á¦]`jÿ?…|Ğº[Ñ,ªõè§10Ì¶İ%˜úZVQ©û§-­R¢9rC[jlÃÅ×|=ÆÈºÏ6×šy?Á(ıÔÚ:ë:	h‹+ô.·ë0ÆKÀ¹}úû;İPÄ0	öóhĞ‘µñ1õ°Ù±@¬­„8Ò‹ïº(ü†ÒÕëe&¾Æ³æ»lqHõ3±¡#»ù¾Yj·Átœü/Ëûøeùœ?€ |R÷…MÓµæ»w—Æ£E©ŠA³SKÙP1håW9xaÕÚ{J%§.úÃ¸~ş9i˜¿ô´"U›\ôÂ³ÿ²N…šç$õ8ïì|…6Ÿ#ª°âŸÉ´§\gxyé—:§¸/•ï‹“÷<„kÔ?œL£9bâäÙ¯İ'÷ÄŞQŸ•|ñ1íaô[j“Àd¨éÆÕ~ZBÊ.ìÒ[ÀuL¯A<d°¨-.£ı6mˆ!íÔ¿ókE§BC-—:óU¤­
“à)g0ÒÀv‰mO{hƒB|ûGP_Ædh‘İâô^·èO1ë©lŞ4Jğ²ªv„I‘í÷;¨¹P%•¾TC°»©UfŒ%)õ"ˆ}k\ì|ÆèX.ŠŒõPCLt
Sî¨ô·®Çå7ôô}¸Íši
Œô¾”«:(·í÷í,kª³Á´é.i·ĞxZ“|ëãæÔa¡d£€Ö*â;½šÔOÓŒqH¼2~“>BøK™¡:ˆ¬h9š™%MöŸï¬ŞFÖÈŠ:Æ¸M!`Ï't DEŠkR™?® UPa™‘:)DoÖşƒ”N@JÊ,<ZÛÌ6bciMıvZo‡c×,IbV‡Ï!™ü8h­§Ñíƒ_]j¡&!„¸uøØR,¸o÷yY¬œYª3ÍF€Sıé®åcÇ$®“¥á˜)[Q4ô€ÉMçÿyHš0¨’Îì2¾P¨ÒšÑû™2F¶cWR'3qG	&<">–}‘c™úC/ƒKuÛ<¡±T‘Ä­âVÆ¡Ó‡B5„jçÃ(3İWÌçñÇ1ZdÛ¬Í[#Šïû±¶ìZT	‡PĞ½
Æ_òGVkO´Àšñ.û·etš5æ<\T^•©FùoNø¼BÔïír´WøpşLXY´êS¼R—œ7õ|öÅ1÷rP o¨WÎÂù–+-æ®Ù<ˆY:jCÏ7™6zG­4²-İËìC¼c1`ÏêC›f9@ë\HÅ“nşáv¶RÁ””¬Ñ:‚¶¯³ô
MùßKÎ‘Š¨š$M°%XıO;ñ¢6ØD#?+»’}†zÄÁ–¥»{qóÚaˆ}u•ïó.’œ ãéÖˆ-Øh[
‰òJOYRd˜XOƒğ~›àS»–¼O¬X°İe×İ!
_ÒQ¼˜ÑÇ»fÓ/:ÁËPÉ«ZÊ%ÉôüãÎsQíƒC’q”ïÒ’‚ÇÅÊÓ“ı¨i/CEü‹c':0øm…?
vå¡ÆÂ.ãê]A©íÇr•Íøw«1T>ÙÔÃ ¶œ[Î5_øyÅ‡‡+}?Vÿ­ï¤4ufœ‹›5íû[³5ÊÕÿw`b½¨Ø˜ºº²}ˆ™™$,á…6² ìm—×3öÏ›=-‚S®{ĞJ/«#š<-èqÁK©R *ìãó!ëv‘¶;YO±—01›~Ó¹³¶5w%(ÏÉMğPRG…£êœ¶ô7ÏÏĞ(¦~;S»Àˆõ«Ã¥&`Äëm„VO:åAsÕbá%fô«!-4‡cV¯Ny¥"\Ps¤&ô-~êc²M‘µy€k÷2X`Ğ°1E¤ğş†„9¶¨µ¾¼Îç<÷¤:÷ÙìG IÜ:K5Ñö©ÿ*4	á–fxÀkª;ZÄ­¤zøä³ò©µ6öµ‡…¾Æğ´-›á-‘«Z¯&¼>Ø•6  «´Ä¤JxH‹ë-¨ÜutXÄ}P'nSj1ŞÆÆ ƒT)~gQòwHò¤x	ç„×uÖÕ	Üœk„¬Cd„ıêEW6èèZÕ%6‚jRcÚvÃlõûë[RÏ’“Éê}Ô‡°%G|/IÒB˜tP˜‡q¦PÒÃà!ê'5îuƒÁ€÷Êíi=;gs›Jöï%A¦¶(4½Ğ³m?I}:³ˆˆş[NßHXì4¡`·lZlLL¡õ©‹Ó™¸‡=q#0ÖÖßŒĞ!OGŠ³F;ÏÌàÓç×aG g ½†SãÜNĞCÌPº§æÏÄ‘ˆsNØ}Å—+óC„(uŠ
³"#Ù¢5Y8?ÆÛ˜zEJµw›aD ²k{	0HDXÀÙ¸)şg¼ÜCì°¨ßÏ7
o r(Y8›‡‡¿¯¢İÚ¾E§íÏ:¦Òûk¾ÑjÖŞ›OIéiåÕ„Q-vYFÉbÁ>:hàì%ô²%võCÎoe­"kÁÙ¡—Ç8wXRôˆ*Š@Ã>¸‘EnéÔdøãÔ0b„zä;«È“CZ”*Í$07×Zü’U`[“j½’t…® \·ÊªÑtì ¬lÙ¦Ç¶iÒ©lxœ€s'#ÏPu-C7à'³3ÁLB°€›hVn¶ÔàÌ”RİÑ¹à7©·	˜š³µú
ò*t¢´já)$Şp·T Ì“ò$¥Z‹üQ
‹°¤NRê?Ø%ÏHÛŠHwÅ¬Ã4˜Å­ºläâ>BìbQÃÈP’ÈÅõıV
÷GØDj° gÄ¤c‘”oJ¬{Yzç7°¨Ë ï-ú3s¶GåêwÎyş¾ŞBK‰•g®êS…­?´>E²7İtş©ãŞû[ÈÈTbÃ+ávÒÀLZH–t8¤ñ¯ÃÒwŒïŸ'EÚ ²mTOqfnjûòßúAŒÂÆ?ªgÁ¹6+Šèè÷v4—BßC
çvr]Vp†‰vE˜Ğ%–Í/™i«aS1 ¤€´¹å ‡äòÌmÚ]–çe[êóyC»öXàÆÅ·¤§KQ³Pk”™¢áf½SP­DZL`kı
hbŒ¨ÎÀr‚x/©ù¥«ŸØx“e^«à”‡D9ô]­*ØĞ›O‘kU óPäí•ÆĞƒjÂxVa’ÿk¯ïr±î?®ŒBœ}É¨‡3Â²@ÂbW9)~éäó›¡Æß»Qå ¹‡ı4†¹ü«Oú4V—*r¼ƒ%ó)Ê¨Š&&1s·ò^fÍ8º1³Şs¤='Ôcü)6YLs™M‚¨@P(íâ£ÎÃ'”Yx"!úârZ¾7 ¢?¿b/¡U8&A÷¥pÉ£mŞåËpŸ‚DØ³Ğ,Ä“\½­>o÷©Ê¾ÖsÖP.Ô™Ê´=ò|w”ßİ“¾=~D‹T«¢f¾‚¿ÅÌRÛkcMöğàø4 „.©_É\é®Ş¦{8HQ«oîh¢å u	oûs][¦ï¯•äÂ¢zô,	6<ãz3>Ô…ŒO·àlû˜½Pû|3rWE4Gü:wyE]oÇQ$¶ˆ1=¶+£ïs›l)İ<oaÖ-!íİÍB°,ˆ5lÀêb…{OÆëëÕˆiÀ‚©öy8ÇÔuhöM‹V‘‹”Ëø5=p×Ò•Çñl5¥ªæïÇåty§',Aª4†’j„j@>$ğFm±åİV²_iq@ÿ9Ú[~Ü*^òøö+Î|v4ÛSämèz¯¸ ìpÁZÿ4©D·$©ÿüTğÒeíwb‚|ê+uXŒ™öñëàşí§9‹
lãT.*#'<j:†¢ cRLE©*AÔHœéğ^“R-ñÙ4FĞÈ9[Q¶úkC¥Ş x9 ^C&äêaêıìk<5.):Ä.İx}mÿ“yô›$u}oş ×%˜Ì¼g†ÁµYÙ,C¯~^4høûöûuc.ù3› ï,f2õ*?sÖW˜„{İ|ƒÖ‡ámoH;z!{jîL¾YZÆ†å|_P¢ÊÅV›E':±ğüôœÓìŸ5m}ë‘xqñiÂ
¦ĞÄm"bÏŸ ìÓœ;ÑEöÔC6‰!æ=eÄÄÙ‡îFˆ¿ù	‹|&B¢_qÁ„Ê<peK?Šo*üÖƒÎÚU²G€bAçmŠä@«Îıï—°p“}L‰Ho¸”O¬•3›ßö§2g+Sñ9/XA—4"­Œ×‚/F·-Ôzb"÷†oê³|éÜuIˆÓ
–µ¶­ªµ½oûI Ã£’dÙÅ.;ÏøLÆß—aGŞ¼®óÀÅx±$5Qv]Y-y‡¢4>²òDŒ>MXBy'
(Í˜6Qœş¨_3U£… ²m·Ø~à ÔGõ½—Âıİ»
•-Æ„ò>Ü'èW™òÅÒ¿m´|í¼/ÈıIqÆşyièf0Ï)hgä^lzG~ı¤îii(òü[ÀÓ“Ê¢W.øWX°ØGP+0¹¾9¹"!‚¦sº#õBì ·0›/hªºæ‰‘N‰Œ‡¼—-º&B¬MÂ J*2ìŠÙá¦¾îaıt’(Ë+dÏ9åÛáÉKÆ»Ü)"µÚümÛ9%1mäOÈıĞ"ÑàÑûÂê§pIŞÑ÷*8øÄJçßˆe+[üï:s¼bÊ¯q»ü8YPs[ÃfbwÊãß6Ù»¸öìX¨0·êÈıHÁ€I—yÙ_kçcp:­dËBzílË]}í³MŒÎ« wí³É}yhÙmtpTıX!ÕL6ËI'«-Sà}uy­ØòA3¡Š	¿2êÊî):¾¦8;8d#G·
Äœ œ¶×‰]r+â»şØüĞA'˜2–¤‹ÒBıZí‡QU„ş+ZJKc‹ÓEªH)ÎOÚ %~©€¤ù¹g£^¬†	f÷88"–ódML@ø{×<Y¾­ïT “êÓ§¾Ê¸ÃYÎ½ºú³ÙqàD~ËÕ’µèh±zÆ”jR“º/GU±Ü†’_ +o¡ÙÙã3F²|Ó‚È6^=c¹Èôè&rßÌ%¿˜Šï´­}WIô½åzj_â)-Ó]ÑCÖŒ!âv}F‚¨çå|÷p‰ñêÛcÆ#/ulGÀìš«¾ AŞ‰Ş¾?^*"î²Ö+B«+50I}GŒ˜í`9 ª`ÕLÔ`°cP0©–s£Ñ¥Åãƒÿ1¹Zî€™råÊç1k¬Œôôax%½e<|âq+ÂÕréÁä/ôC2ÆëÎÀdÑU)¢*-i•Ms ˜jƒÍìJ±Ó
½³öÏ9<—_×ıä™÷N¦V%eïª¯pÏÄÙ"Óri¬ŸäÿFø¼ne¥òköG~ˆ2efÒ²J¦6Ü\‚uaBDšÏç18d/ÄÍ0›ãıé¹g$¢#GQÇbKjä’a’kÇãÁR+šãs/#F,?;Õi– Ï˜íÚÈ¦D;P+–¨´‰µÓ„{V'@r¦cåòÄ¦ÛîŞŞĞ“ç™şíûñY1R7ˆÀ¤T·meğ(½HşA!Œ0IÍ"[uİ-Ç¯PÒ•ÏÕò¸€ßpÍäÒõj!FêJPÄ¤¾\ÈJthÏåuZ­*G¸Áİ’Ój±‹o”VP¸b)òxR]ÿ´8
* sÂà§Jq™túm–×/èX<6|‘ñ)T?(v¹±¶k ª°X¡(M3ŞuË5
=İªûìèŸRÍ&ìßuËÛ å–:ÚF#èy_U%×÷Ş.~‡ØDÒë+ï²KØãmÏÒs:»¸ÜfWê½Q‹@Æ¾ÑKõ‹D¹áåSê¤‘ì5~0èó*!½ml'Í¼¨X§$Áø‡P"(|U×ÄusŸWÃd›çêœ¦ÙÔ:"íÇ.Ø¤L‰·c%åä¢±ÊGŞv*ÌÀ÷u"±âúV¢†Û†ºú’ZïiÒ”LlC·-
¡7:§Àßí
{‡©z P¥.[Í=Ğ¨fÍÏ)°©‘Õnì)œçÒÑNÀšéä8ÔÏ,mFR¯ÑÍÚtXÖ€eÖx³m	áÌßjtÜ˜ù”§'X¡\Dâgİ+3j5®o±ú¡ØQEñ¬ñéÛJõ³ÚfcÔğ¿#$ŞueRtÕ-QDş“l ½Ä˜8ÀİÌ«n¼ş©úË†åU$NÚğJhKd¿˜*§¹èÆ*İñEëå©w¹Í…ºÅ‰ }9‚hñƒGÈ™3Nmİ3Ò„x:Ï]AÒšÍC®‡­²äÈEûÓ¥©;Ÿ5Ù©Ë”w»“@\Ù¡Ø(åò>mã\zòJ@¤˜EYHEÙF¥„lÍI•h,€ ÕÁ3T*ÁbÔ™	Ê*èè-¼Í‡Ê7à‘¸ìÏwuı£bµ+ı^ŞcaÚ¸Ÿ0DÈX¦MTñy &	x±G¡íÇê8”˜¦H¢*ü¦èB‹qõñ!=§âlŠh|Ñ¥9Dæª4:›
õHÒİù~/C~D†\F^dzn`9ÿæ£ÎD¾0€¤Dœ©dáî ïµYLìä¯Y =lgÙÛ·5àçØWz9É{!l*Êl"é p»áábQ¹>àú¬ğ¾¨=<g
ğı%•]I«'ĞÔö”Ò•m±à$ZÙ˜‹tPº…!¢Ğ	iæ&œJúJ“Vëô‚§^’ Ê~ığ¿ğµ
C˜ò#ä‘ç B¦ïÁ$q/fEjªæY©­ë¯èù¹ùâ}šÁÈN›SØ´ş+ê8Ò4ãC©Ñ^©hşÀİVˆ‰9+G±Qt!£»ìõ7'ıış–¦!·wûch'ã·ì ı¨Šî¥ÑÃ4ëJ´½šŸÙ9¼ÒşÙã†0hÄ|šz_¹L¦EOá¡|¹K¸¯Å AôkŞ”ê!ÛCmêZrF3os‡—ıj ÅúfÆóù$“%–2OœÏrÙ2Ş÷°5m­zwúK:oÊ¦˜‰úl÷NÅô6³ô Ã¦;ErÒ<"©‹øßÊ™_	ƒ’-ŠÖù‘‹|çÁ)WŠxrhâºˆµ˜ ³ôk¹d¾kû;¤šX(T=äå+´xù´Öİ‹æI×“¯Ü¨ã)eú^C¼åÃ£‰ïÂàËX¦&ÿÅrWpE|_ìQ\*AEä_µºÊU8¶Ö°m.¨ã_ŞmÓ¬ÇÅ6j@2¡A¯­-…2™à^ÄA­(–#úÆŒ×c®Ìwõ¯,C>F«ìË%‚'í‹+çÉ³Òˆ¥”MPr‘ñÖ¹E‰ş›}¸2¨…Ôkk'É‡Œ½Zz×JĞ’Òz“ljUĞó Òúä™‰Ä¹ıxx¬k‘NÿEµJ90)ÁşvU3Y ÷)³o(_[ÿ~úF0Lˆß‰É­éÏOø[¦\C´>|ÏZÛş^æjV
‹å8sZV†àçDêkjÔ–—x“.4„Vfj‰‡¸¥g{}ó\r¼’ÂôI(\˜™¤26rí*“;Y=#y‘}Ë‰ÓlùwU*Ø{‡ªÕ1-khæjs%¯¦,—û¬É˜2ÆÓ²·„‘E Á7ìs¨NQksq‚ê&p'İ)£n;…Zu`çÄ_o±ÍêŒtå ¬*ºÀ÷/&¨Í’ =vN/ê
¤Ës8dİ¶1F~OƒiF`‘.íoÒ`‘JA[8òÙçp’üÚÆÙRİÔ)j¾éEe–*bë—_XŸ?"Ì¯’mÍµye4a‰,°€‘täÓµÔ+föv¦¶\Bk÷‚2…ø³Šé:¬½ü™ŒJ$5ÆÚicms¸İH&ÌÙWäÀÓÀ`¨Gæ^I-Cá¾àYP‡!3%ñüG³ÒQé‹½Q»F@ÆË	çË]¼XSlpGƒw/;ÄÄ‚Å–ğÂñ^éÓÌö,ùÈ
Hï}Œ¾gB%*<•\ÀcÛ°œX™¨æÒm÷”Ôäa‡g‰g‡3?˜HwLßò\uìon€šÖ/{1ß±×µñÅ²ğÀ,ımW^åªïj®‘O»»ÑEº-•¢¦úªµ ÔŸ*<½Bä V®cm‰ŞIr®õê-@Ö'[¿ü*ô»A¯úŸ~qÕe–şÖú*À¢î ?€!õ¾¶wÆB-Å|V_d	Ì‚Ó‚ç{´é›Pfº×ÆL¤ª<,$.¹%×pL…z€,»EÖÀË¡<V™Ï~~îbMë‰ËLˆ<éË-÷Bå@ræJé$_K”W¦ÑÙú$ô(s_®{G®M"›Ú.lNê»	ÄÉ²¡dg5TT.IŒ ({ÑK]ˆÈÚ Ö9ş'y‘¶.ª.ÕL¿J5‚6±¤ï&xg¢9A(H“µ‚°­¹dLÔoÄJu›¼Œ‹v¡G‹8EÕÆˆı0˜í¸Òzr(a‹0NBu€çfÕ#KŒ0á:Ïï>Ó Î»ÿ;$ÃXKıèW9@IÅDe[]v]rê`ğŒ[	a;ç=µ=r}¨øSqÆ”Ÿæã~Ü]ûƒÙµG%„ÃæØ½ˆBşŞ™ù)˜OÇCBúWßş:.²¡ŸAº-üp#FÃ'¤‘ou{4ÇÊÅ,×tÕøøÄ†€İüóO'˜"ö;ÓìU€ÏnUî¦ ÛPšÌÙöÆÜ_úYŞæF·8ÿsÖÉ£=f;ı•$xİ6£q›lúèáv`·¹¡QàE¤ 0[e2$ü£·pò.R¶¦—(‘¼Aí+Œ0=À-‡N M;¨2ˆœ°mßB¹ÅÁ°×oµªiÂQˆãÔı–EUÜ–ë÷oÇ°Yn}3ÄH-»î|şi(¹ëT'‰#tèwGé+kÛqÇhªï†õv…³ÙĞä.ÀöïŸ	`§¹¸y	ÇÍ#İ]U^÷èØ©ê
ï,¥—›ÒW‡kgÍı¶Ô2×“³}™ÉãaGçÁ2:ï#ÄM¢mö@GÓp³T¾³T?ˆ·gÉ4.7[=‹2õœf_2*qN.9~ƒö4Xˆ+y‡Î—qÀíÏˆç.¼ä²ÏÆÎfrÈ<à'	§¦ã¥	O¥b„Šà\Ôp—ß€!]¤y&Ö>ÊC?VDóó:ÌòË„ÛJ§`Ev’Ú.™*u­c‹„ıÎ‡vÌ„¶k›'ã†´Â‰0>©+n™ÛW#§Sû³xç`€¦MŠ£^ ÷DpªÒpá‚DâçíàÓ1	µaV}&ë“ùŸc\9ù¼0ì½aÜRæ)‹A±ş2%òÿu¨xBEûÌFL  Jÿœ»–»„í5.a‚ãl­W˜ÀrÛg;
CWÔéC‘ãQæšÖÅÕæ¡«qç0ÕÀa~’uw•Á ´R%ß2Îÿ©"±ì¾6_ä7Iw,NùBßÕ¦¬!Î ¨&7qõİGœ=ÅyÌ!Vû-øj´$~ôºSçŸŒst_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;
	int i, j;

	/*
	 * The caller must hold the graph lock, ensure we've got IRQs
	 * disabled to make this an IRQ-safe lock.. for recursion reasons
	 * lockdep won't complain about its own locking errors.
	 */
	if (lockdep_assert_locked())
		return 0;

	chain = alloc_lock_chain();
	if (!chain) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAINS too low!");
		dump_stack();
		return 0;
	}
	chain->chain_key = chain_key;
	chain->irq_context = hlock->irq_context;
	i = get_first_held_lock(curr, hlock);
	chain->depth = curr->lockdep_depth + 1 - i;

	BUILD_BUG_ON((1UL << 24) <= ARRAY_SIZE(chain_hlocks));
	BUILD_BUG_ON((1UL << 6)  <= ARRAY_SIZE(curr->held_locks));
	BUILD_BUG_ON((1UL << 8*sizeof(chain_hlocks[0])) <= ARRAY_SIZE(lock_classes));

	j = alloc_chain_hlocks(chain->depth);
	if (j < 0) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAIN_HLOCKS too low!");
		dump_stack();
		return 0;
	}

	chain->base = j;
	for (j = 0; j < chain->depth - 1; j++, i++) {
		int lock_id = hlock_id(curr->held_locks + i);

		chain_hlocks[chain->base + j] = lock_id;
	}
	chain_hlocks[chain->base + j] = hlock_id(hlock);
	hlist_add_head_rcu(&chain->entry, hash_head);
	debug_atomic_inc(chain_lookup_misses);
	inc_chains(chain->irq_context);

	return 1;
}

/*
 * Look up a dependency chain. Must be called with either the graph lock or
 * the RCU read lock held.
 */
static inline struct lock_chain *lookup_chain_cache(u64 chain_key)
{
	struct hlist_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;

	hlist_for_each_entry_rcu(chain, hash_head, entry) {
		if (READ_ONCE(chain->chain_key) == chain_key) {
			debug_atomic_inc(chain_lookup_hits);
			return chain;
		}
	}
	return NULL;
}

/*
 * If the key is not present yet in dependency chain cache then
 * add it and return 1 - in this case the new dependency chain is
 * validated. If the key is already hashed, return 0.
 * (On return with 1 graph_lock is held.)
 */
static inline int lookup_chain_cache_add(struct task_struct *curr,
					 struct held_lock *hlock,
					 u64 chain_key)
{
	struct lock_class *class = hlock_class(hlock);
	struct lock_chain *chain = lookup_chain_cache(chain_key);

	if (chain) {
cache_hit:
		if (!check_no_collision(curr, hlock, chain))
			return 0;

		if (very_verbose(class)) {
			printk("\nhash chain already cached, key: "
					"%016Lx tail class: [%px] %s\n",
					(unsigned long long)chain_key,
					class->key, class->name);
		}

		return 0;
	}

	if (very_verbose(class)) {
		printk("\nnew hash chain, key: %016Lx tail class: [%px] %s\n",
			(unsigned long long)chain_key, class->key, class->name);
	}

	if (!graph_lock())
		return 0;

	/*
	 * We have to walk the chain again locked - to avoid duplicates:
	 */
	chain = lookup_chain_cache(chain_key);
	if (chain) {
		graph_unlock();
		goto cache_hit;
	}

	if (!add_chain_cache(curr, hlock, chain_key))
		return 0;

	return 1;
}

static int validate_chain(struct task_struct *curr,
			  struct held_lock *hlock,
			  int chain_head, u64 chain_key)
{
	/*
	 * Trylock needs to maintain the stack of held locks, but it
	 * does not add new dependencies, because trylock can be done
	 * in any order.
	 *
	 * We look up the chain_key and do the O(N^2) check and update of
	 * the dependencies only if this is a new dependency chain.
	 * (If lookup_chain_cache_add() return with 1 it acquires
	 * graph_lock for us)
	 */
	if (!hlock->trylock && hlock->check &&
	    lookup_chain_cache_add(curr, hlock, chain_key)) {
		/*
		 * Check whether last held lock:
		 *
		 * - is irq-safe, if this lock is irq-unsafe
		 * - is softirq-safe, if this lock is hardirq-unsafe
		 *
		 * And check whether the new lock's dependency graph
		 * could lead back to the previous lock:
		 *
		 * - within the current held-lock stack
		 * - across our accumulated lock dependency records
		 *
		 * any of these scenarios could lead to a deadlock.
		 */
		/*
		 * The simple case: does the current hold the same lock
		 * already?
		 */
		int ret = check_deadlock(curr, hlock);

		if (!ret)
			return 0;
		/*
		 * Add dependency only if this lock is not the head
		 * of the chain, and if the new lock introduces no more
		 * lock dependency (because we already hold a lock with the
		 * same lock class) nor deadlock (because the nest_lock
		 * serializes nesting locks), see the comments for
		 * check_deadlock().
		 */
		if (!chain_head && ret != 2) {
			if (!check_prevs_add(curr, hlock))
				return 0;
		}

		graph_unlock();
	} else {
		/* after lookup_chain_cache_add(): */
		if (unlikely(!debug_locks))
			return 0;
	}

	return 1;
}
#else
static inline int validate_chain(struct task_struct *curr,
				 struct held_lock *hlock,
				 int chain_head, u64 chain_key)
{
	return 1;
}

static void init_chain_block_buckets(void)	{ }
#endif /* CONFIG_PROVE_LOCKING */

/*
 * We are building curr_chain_key incrementally, so double-check
 * it from scratch, to make sure that it's done correctly:
 */
static void check_chain_key(struct task_struct *curr)
{
#ifdef CONFIG_DEBUG_LOCKDEP
	struct held_lock *hlock, *prev_hlock = NULL;
	unsigned int i;
	u64 chain_key = INITIAL_CHAIN_KEY;

	for (i = 0; i < curr->lockdep_depth; i++) {
		hlock = curr->held_locks + i;
		if (chain_key != hlock->prev_chain_key) {
			debug_locks_off();
			/*
			 * We got mighty confused, our chain keys don't match
			 * with what we expect, someone trample on our task state?
			 */
			WARN(1, "hm#1, depth: %u [%u], %016Lx != %016Lx\n",
				curr->lockdep_depth, i,
				(unsigned long long)chain_key,
				(unsigned long long)hlock->prev_chain_key);
			return;
		}

		/*
		 * hlock->class_idx can't go beyond MAX_LOCKDEP_KEYS, but is
		 * it registered lock class index?
		 */
		if (DEBUG_LOCKS_WARN_ON(!test_bit(hlock->class_idx, lock_classes_in_use)))
			return;

		if (prev_hlock && (prev_hlock->irq_context !=
							hlock->irq_context))
			chain_key = INITIAL_CHAIN_KEY;
		chain_key = iterate_chain_key(chain_key, hlock_id(hlock));
		prev_hlock = hlock;
	}
	if (chain_key != curr->curr_chain_key) {
		debug_locks_off();
		/*
		 * More smoking hash instead of calculating it, damn see these
		 * numbers float.. I bet that a pink elephant stepped on my memory.
		 */
		WARN(1, "hm#2, depth: %u [%u], %016Lx != %016Lx\n",
			curr->lockdep_depth, i,
			(unsigned long long)chain_key,
			(unsigned long long)curr->curr_chain_key);
	}
#endif
}

#ifdef CONFIG_PROVE_LOCKING
static int mark_lock(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit new_bit);

static void print_usage_bug_scenario(struct held_lock *lock)
{
	struct lock_class *class = hlock_class(lock);

	printk(" Possible unsafe locking scenario:\n\n");
	printk("       CPU0\n");
	printk("       ----\n");
	printk("  lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_usage_bug(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit prev_bit, enum lock_usage_bit new_bit)
{
	if (!debug_locks_off() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("================================\n");
	pr_warn("WARNING: inconsistent lock state\n");
	print_kernel_ident();
	pr_warn("--------------------------------\n");

	pr_warn("inconsistent {%s} -> {%s} usage.\n",
		usage_str[prev_bit], usage_str[new_bit]);

	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] takes:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		lockdep_softirq_context(curr), softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		lockdep_softirqs_enabled(curr));
	print_lock(this);

	pr_warn("{%s} state was registered at:\n", usage_str[prev_bit]);
	print_lock_trace(hlock_class(this)->usage_traces[prev_bit], 1);

	print_irqtrace_events(curr);
	pr_warn("\nother info that might help us debug this:\n");
	print_usage_bug_scenario(this);

	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Print out an error if an invalid bit is set:
 */
static inline int
valid_state(struct task_struct *curr, struct held_lock *this,
	    enum lock_usage_bit new_bit, enum lock_usage_bit bad_bit)
{
	if (unlikely(hlock_class(this)->usage_mask & (1 << bad_bit))) {
		graph_unlock();
		print_usage_bug(curr, this, bad_bit, new_bit);
		return 0;
	}
	return 1;
}


/*
 * print irq inversion bug:
 */
static void
print_irq_inversion_bug(struct task_struct *curr,
			struct lock_list *root, struct lock_list *other,
			struct held_lock *this, int forwards,
			const char *irqclass)
{
	struct lock_list *entry = other;
	struct lock_list *middle = NULL;
	int depth;

	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("========================================================\n");
	pr_warn("WARNING: possible irq lock inversion dependency detected\n");
	print_kernel_ident();
	pr_warn("--------------------------------------------------------\n");
	pr_warn("%s/%d just changed the state of lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(this);
	if (forwards)
		pr_warn("but this lock took another, %s-unsafe lock in the past:\n", irqclass);
	else
		pr_warn("but this lock was taken by another, %s-safe lock in the past:\n", irqclass);
	print_lock_name(other->class);
	pr_warn("\n\nand interrupts could create inverse lock ordering between them.\n\n");

	pr_warn("\nother info that might help us debug this:\n");

	/* Find a middle lock (if one exists) */
	depth = get_lock_depth(other);
	do {
		if (depth == 0 && (entry != root)) {
			pr_warn("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}
		middle = entry;
		entry = get_lock_parent(entry);
		depth--;
	} while (entry && entry != root && (depth >= 0));
	if (forwards)
		print_irq_lock_scenario(root, other,
			middle ? middle->class : root->class, other->class);
	else
		print_irq_lock_scenario(other, root,
			middle ? middle->class : other->class, root->class);

	lockdep_print_held_locks(curr);

	pr_warn("\nthe shortest dependencies between 2nd lock and 1st lock:\n");
	root->trace = save_trace();
	if (!root->trace)
		return;
	print_shortest_lock_dependencies(other, root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Prove that in the forwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_forwards(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_root(&root, this);
	ret = find_usage_forwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(read_bit));
	}

	return 0;
}

/*
 * Prove that in the backwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_backwards(struct task_struct *curr, struct held_lock *this,
		      enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_rootb(&root, this);
	ret = find_usage_backwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(read_bit));
	}

	return 0;
}

void print_irqtrace_events(struct task_struct *curr)
{
	const struct irqtrace_events *trace = &curr->irqtrace;

	printk("irq event stamp: %u\n", trace->irq_events);
	printk("hardirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->hardirq_enable_event, (void *)trace->hardirq_enable_ip,
		(void *)trace->hardirq_enable_ip);
	printk("hardirqs last disabled at (%u): [<%px>] %pS\n",
		trace->hardirq_disable_event, (void *)trace->hardirq_disable_ip,
		(void *)trace->hardirq_disable_ip);
	printk("softirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->softirq_enable_event, (void *)trace->softirq_enable_ip,
		(void *)trace->softirq_enable_ip);
	printk("softirqs last disabled at (%u): [<%px>] %pS\n",
		trace->softirq_disable_event, (void *)trace->softirq_disable_ip,
		(void *)trace->softirq_disable_ip);
}

static int HARDIRQ_verbose(struct lock_class *class)
{
#if HARDIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int SOFTIRQ_verbose(struct lock_class *class)
{
#if SOFTIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int (*state_verbose_f[])(struct lock_class *class) = {
#define LOCKDEP_STATE(__STATE) \
	__STATE##_verbose,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline int state_verbose(enum lock_usage_bit bit,
				struct lock_class *class)
{
	return state_verbose_f[bit >> LOCK_USAGE_DIR_MASK](class);
}

typedef int (*check_usage_f)(struct task_struct *, struct held_lock *,
			     enum lock_usage_bit bit, const char *name);

static int
mark_lock_irq(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit new_bit)
{
	int excl_bit = exclusive_bit(new_bit);
	int read = new_bit & LOCK_USAGE_READ_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * Validate that this particular lock does not have conflicting
	 * usage states.
	 */
	if (!valid_state(curr, this, new_bit, excl_bit))
		return 0;

	/*
	 * Check for read in write conflicts
	 */
	if (!read && !valid_state(curr, this, new_bit,
				  excl_bit + LOCK_USAGE_READ_MASK))
		return 0;


	/*
	 * Validate that the lock dependencies don't have conflicting usage
	 * states.
	 */
	if (dir) {
		/*
		 * mark ENABLED has to look backwards -- to ensure no dependee
		 * has USED_IN state, which, again, would allow  recursion deadlocks.
		 */
		if (!check_usage_backwards(curr, this, excl_bit))
			return 0;
	} else {
		/*
		 * mark USED_IN has to look forwards -- to ensure no dependency
		 * has ENABLED state, which would allow recursion deadlocks.
		 */
		if (!check_usage_forwards(curr, this, excl_bit))
			return 0;
	}

	if (state_verbose(new_bit, hlock_class(this)))
		return 2;

	return 1;
}

/*
 * Mark all held locks with a usage bit:
 */
static int
mark_held_locks(struct task_struct *curr, enum lock_usage_bit base_bit)
{
	struct held_lock *hlock;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		enum lock_usage_bit hlock_bit = base_bit;
		hlock = curr->held_locks + i;

		if (hlock->read)
			hlock_bit += LOCK_USAGE_READ_MASK;

		BUG_ON(hlock_bit >= LOCK_USAGE_STATES);

		if (!hlock->check)
			continue;

		if (!mark_lock(curr, hlock, hlock_bit))
			return 0;
	}

	return 1;
}

/*
 * Hardirqs will be enabled:
 */
static void __trace_hardirqs_on_caller(void)
{
	struct task_struct *curr = current;

	/*
	 * We are going to turn hardirqs on, so set the
	 * usage bit for all held locks:
	 */
	if (!mark_held_locks(curr, LOCK_ENABLED_HARDIRQ))
		return;
	/*
	 * If we have softirqs enabled, then set the usage
	 * bit for all held locks. (disabled hardirqs prevented
	 * this bit from being set before)
	 */
	if (curr->softirqs_enabled)
		mark_held_locks(curr, LOCK_ENABLED_SOFTIRQ);
}

/**
 * lockdep_hardirqs_on_prepare - Prepare for enabling interrupts
 * @ip:		Caller address
 *
 * Invoked before a possible transition to RCU idle from exit to user or
 * guest mode. This ensures that all RCU operations are done before RCU
 * stops watching. After the RCU transition lockdep_hardirqs_on() has to be
 * invoked to set the final state.
 */
void lockdep_hardirqs_on_prepare(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs do not (and cannot) track lock dependencies, nothing to do.
	 */
	if (unlikely(in_nmi()))
		return;

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (unlikely(lockdep_hardirqs_enabled())) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * See the fine text that goes along with this variable definition.
	 */
	if (DEBUG_LOCKS_WARN_ON(early_boot_irqs_disabled))
		return;

	/*
	 * Can't allow enabling interrupts while in an interrupt handler,
	 * that's general bad form and such. Recursion, limited stack etc..
	 */
	if (DEBUG_LOCKS_WARN_ON(lockdep_hardirq_context()))
		return;

	current->hardirq_chain_key = current->curr_chain_key;

	lockdep_recursion_inc();
	__trace_hardirqs_on_caller();
	lockdep_recursion_finish();
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on_prepare);

void noinstr lockdep_hardirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs can happen in the middle of local_irq_{en,dis}able() where the
	 * tracking state and hardware state are out of sync.
	 *
	 * NMIs must save lockdep_hardirqs_enabled() to restore IRQ state from,
	 * and not rely on hardware state like normal interrupts.
	 */
	if (unlikely(in_nmi())) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;

		/*
		 * Skip:
		 *  - recursion check, because NMI can hit lockdep;
		 *  - hardware state check, because above;
		 *  - chain_key check, see lockdep_hardirqs_on_prepare().
		 */
		goto skip_checks;
	}

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (lockdep_hardirqs_enabled()) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * Ensure the lock stack remained unchanged between
	 * lockdep_hardirqs_on_prepare() and lockdep_hardirqs_on().
	 */
	DEBUG_LOCKS_WARN_ON(current->hardirq_chain_key !=
			    current->curr_chain_key);

skip_checks:
	/* we'll do an OFF -> ON transition: */
	__this_cpu_write(hardirqs_enabled, 1);
	trace->hardirq_enable_ip = ip;
	trace->hardirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(hardirqs_on_events);
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on);

/*
 * Hardirqs were disabled:
 */
void noinstr lockdep_hardirqs_off(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * Matching lockdep_hardirqs_on(), allow NMIs in the middle of lockdep;
	 * they will restore the software state. This ensures the software
	 * state is consistent inside NMIs as well.
	 */
	if (in_nmi()) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;
	} else if (__this_cpu_read(lockdep_recursion))
		return;

	/*
	 * So we're supposed to get called after you mask local IRQs, but for
	 * some reason the hardware doesn't quite think you did a proper job.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (lockdep_hardirqs_enabled()) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		__this_cpu_write(hardirqs_enabled, 0);
		trace->hardirq_disable_ip = ip;
		trace->hardirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(hardirqs_off_events);
	} else {
		debug_atomic_inc(redundant_hardirqs_off);
	}
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_off);

/*
 * Softirqs will be enabled:
 */
void lockdep_softirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c, avoids
	 * funny state and nesting things.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		debug_atomic_inc(redundant_softirqs_on);
		return;
	}

	lockdep_recursion_inc();
	/*
	 * We'll do an OFF -> ON transition:
	 */
	current->softirqs_enabled = 1;
	trace->softirq_enable_ip = ip;
	trace->softirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(softirqs_on_events);
	/*
	 * We are going to turn softirqs on, so set the
	 * usage bit for all held locks, if hardirqs are
	 * enabled too:
	 */
	if (lockdep_hardirqs_enabled())
		mark_held_locks(current, LOCK_ENABLED_SOFTIRQ);
	lockdep_recursion_finish();
}

/*
 * Softirqs were disabled:
 */
void lockdep_softirqs_off(unsigned long ip)
{
	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		current->softirqs_enabled = 0;
		trace->softirq_disable_ip = ip;
		trace->softirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(softirqs_off_events);
		/*
		 * Whoops, we wanted softirqs off, so why aren't they?
		 */
		DEBUG_LOCKS_WARN_ON(!softirq_count());
	} else
		debug_atomic_inc(redundant_softirqs_off);
}

static int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	if (!check)
		goto lock_used;

	/*
	 * If non-trylock use in a hardirq or softirq context, then
	 * mark the lock as used in these contexts:
	 */
	if (!hlock->trylock) {
		if (hlock->read) {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_HARDIRQ_READ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_SOFTIRQ_READ))
					return 0;
		} else {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock, LOCK_USED_IN_HARDIRQ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock, LOCK_USED_IN_SOFTIRQ))
					return 0;
		}
	}
	if (!hlock->hardirqs_off) {
		if (hlock->read) {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ_READ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ_READ))
					return 0;
		} else {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ))
					return 0;
		}
	}

lock_used:
	/* mark it as used: */
	if (!mark_lock(curr, hlock, LOCK_USED))
		return 0;

	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return LOCK_CHAIN_HARDIRQ_CONTEXT * !!lockdep_hardirq_context() +
	       LOCK_CHAIN_SOFTIRQ_CONTEXT * !!task->softirq_context;
}

static int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	unsigned int depth = curr->lockdep_depth;

	/*
	 * Keep track of points where we cross into an interrupt context:
	 */
	if (depth) {
		struct held_lock *prev_hlock;

		prev_hlock = curr->held_locks + depth-1;
		/*
		 * If we cross into another context, reset the
		 * hash key (this also prevents the checking and the
		 * adding of the dependency to 'prev'):
		 */
		if (prev_hlock->irq_context != hlock->irq_context)
			return 1;
	}
	return 0;
}

/*
 * Mark a lock with a usage bit, and validate the state transition:
 */
static int mark_lock(struct task_struct *curr, struct held_lock *this,
			     enum lock_usage_bit new_bit)
{
	unsigned int new_mask, ret = 1;

	if (new_bit >= LOCK_USAGE_STATES) {
		DEBUG_LOCKS_WARN_ON(1);
		return 0;
	}

	if (new_bit == LOCK_USED && this->read)
		new_bit = LOCK_USED_READ;

	new_mask = 1 << new_bit;

	/*
	 * If already set then do not dirty the cacheline,
	 * nor do any checks:
	 */
	if (likely(hlock_class(this)->usage_mask & new_mask))
		return 1;

	if (!graph_lock())
		return 0;
	/*
	 * Make sure we didn't race:
	 */
	if (unlikely(hlock_class(this)->usage_mask & new_mask))
		goto unlock;

	if (!hlock_class(this)->usage_mask)
		debug_atomic_dec(nr_unused_locks);

	hlock_class(this)->usage_mask |= new_mask;

	if (new_bit < LOCK_TRACE_STATES) {
		if (!(hlock_class(this)->usage_traces[new_bit] = save_trace()))
			return 0;
	}

	if (new_bit < LOCK_USED) {
		ret = mark_lock_irq(curr, this, new_bit);
		if (!ret)
			return 0;
	}

unlock:
	graph_unlock();

	/*
	 * We must printk outside of the graph_lock:
	 */
	if (ret == 2) {
		printk("\nmarked lock as {%s}:\n", usage_str[new_bit]);
		print_lock(this);
		print_irqtrace_events(curr);
		dump_stack();
	}

	return ret;
}

static inline short task_wait_context(struct task_struct *curr)
{
	/*
	 * Set appropriate wait type for the context; for IRQs we have to take
	 * into account force_irqthread as that is implied by PREEMPT_RT.
	 */
	if (lockdep_hardirq_context()) {
		/*
		 * Check if force_irqthreads will run us threaded.
		 */
		if (curr->hardirq_threaded || curr->irq_config)
			return LD_WAIT_CONFIG;

		return LD_WAIT_SPIN;
	} else if (curr->softirq_context) {
		/*
		 * Softirqs are always threaded.
		 */
		return LD_WAIT_CONFIG;
	}

	return LD_WAIT_MAX;
}

static int
print_lock_invalid_wait_context(struct task_struct *curr,
				struct held_lock *hlock)
{
	short curr_inner;

	if (!debug_locks_off())
		return 0;
	if (debug_locks_silent)
		return 0;

	pr_warn("\n");
	pr_warn("=============================\n");
	pr_warn("[ BUG: Invalid wait context ]\n");
	print_kernel_ident();
	pr_warn("-----------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("other info that might help us debug this:\n");

	curr_inner = task_wait_context(curr);
	pr_warn("context-{%d:%d}\n", curr_inner, curr_inner);

	lockdep_print_held_locks(curr);

	pr_warn("stack backtrace:\n");
	dump_stack();

	return 0;
}

/*
 * Verify the wait_type context.
 *
 * This check validates we take locks in the right wait-type order; that is it
 * ensures that we do not take mutexes inside spinlocks and do not attempt to
 * acquire spinlocks inside raw_spinlocks and the sort.
 *
 * The entire thing is slightly more complex because of RCU, RCU is a lock that
 * can be taken from (pretty much) any context but also has constraints.
 * However when taken in a stricter environment the RCU lock does not loosen
 * the constraints.
 *
 * Therefore we must look for the strictest environment in the lock stack and
 * compare that to the lock we're trying to acquire.
 */
static int check_wait_context(struct task_struct *curr, struct held_lock *next)
{
	u8 next_inner = hlock_class(next)->wait_type_inner;
	u8 next_outer = hlock_class(next)->wait_type_outer;
	u8 curr_inner;
	int depth;

	if (!next_inner || next->trylock)
		return 0;

	if (!next_outer)
		next_outer = next_inner;

	/*
	 * Find start of current irq_context..
	 */
	for (depth = curr->lockdep_depth - 1; depth >= 0; depth--) {
		struct held_lock *prev = curr->held_locks + depth;
		if (prev->irq_context != next->irq_context)
			break;
	}
	depth++;

	curr_inner = task_wait_context(curr);

	for (; depth < curr->lockdep_depth; depth++) {
		struct held_lock *prev = curr->held_locks + depth;
		u8 prev_inner = hlock_class(prev)->wait_type_inner;

		if (prev_inner) {
			/*
			 * We can have a bigger inner than a previous one
			 * when outer is smaller than inner, as with RCU.
			 *
			 * Also due to trylocks.
			 */
			curr_inner = min(curr_inner, prev_inner);
		}
	}

	if (next_outer > curr_inner)
		return print_lock_invalid_wait_context(curr, next);

	return 0;
}

#else /* CONFIG_PROVE_LOCKING */

static inline int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return 0;
}

static inline int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	return 0;
}

static inline int check_wait_context(struct task_struct *curr,
				     struct held_lock *next)
{
	return 0;
}

#endif /* CONFIG_PROVE_LOCKING */

/*
 * Initialize a lock instance's lock-class mapping info:
 */
void lockdep_init_map_type(struct lockdep_map *lock, const char *name,
			    struct lock_class_key *key, int subclass,
			    u8 inner, u8 outer, u8 lock_type)
{
	int i;

	for (i = 0; i < NR_LOCKDEP_CACHING_CLASSES; i++)
		lock->class_cache[i] = NULL;

#ifdef CONFIG_LOCK_STAT
	lock->cpu = raw_smp_processor_id();
#endif

	/*
	 * Can't be having no nameless bastards around this place!
	 */
	if (DEBUG_LOCKS_WARN_ON(!name)) {
		lock->name = "NULL";
		return;
	}

	lock->name = name;

	lock->wait_type_outer = outer;
	lock->wait_type_inner = inner;
	lock->lock_type = lock_type;

	/*
	 * No key, no joy, we need to hash something.
	 */
	if (DEBUG_LOCKS_WARN_ON(!key))
		return;
	/*
	 * Sanity check, the lock-class key must either have been allocated
	 * statically or must have been registered as a dynamic key.
	 */
	if (!static_obj(key) && !is_dynamic_key(key)) {
		if (debug_locks)
			printk(KERN_ERR "BUG: key %px has not been registered!\n", key);
		DEBUG_LOCKS_WARN_ON(1);
		return;
	}
	lock->key = key;

	if (unlikely(!debug_locks))
		return;

	if (subclass) {
		unsigned long flags;

		if (DEBUG_LOCKS_WARN_ON(!lockdep_enabled()))
			return;

		raw_local_irq_save(flags);
		lockdep_recursion_inc();
		register_lock_class(lock, subclass, 1);
		lockdep_recursion_finish();
		raw_local_irq_restore(flags);
	}
}
EXPORT_SYMBOL_GPL(lockdep_init_map_type);

struct lock_class_key __lockdep_no_validate__;
EXPORT_SYMBOL_GPL(__lockdep_no_validate__);

static void
print_lock_nested_lock_not_held(struct task_struct *curr,
				struct held_lock *hlock,
				unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("==================================\n");
	pr_warn("WARNING: Nested lock was not taken\n");
	print_kernel_ident();
	pr_warn("----------------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("\nbut this task is not holding:\n");
	pr_warn("%s\n", hlock->nest_lock->name);

	pr_warn("\nstack backtrace:\n");
	dump_stack();

	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static int __lock_is_held(const struct lockdep_map *lock, int read);

/*
 * This gets called for every mutex_lock*()/spin_lock*() operation.
 * We maintain the dependency maps and validate the locking attempt:
 *
 * The callers must make sure that IRQs are disabled before calling it,
 * otherwise we could get an interrupt which would want to take locks,
 * which would end up in lockdep again.
 */
static int __lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check, int hardirqs_off,
			  struct lockdep_map *nest_lock, unsigned long ip,
			  int references, int pin_count)
{
	struct task_struct *curr = current;
	struct lock_class *class = NULL;
	struct held_lock *hlock;
	unsigned int depth;
	int chain_head = 0;
	int class_idx;
	u64 chain_key;

	if (unlikely(!debug_locks))
		return 0;

	if (!prove_locking || lock->key == &__lockdep_no_validate__)
		check = 0;

	if (subclass < NR_LOCKDEP_CACHING_CLASSES)
		class = lock->class_cache[subclass];
	/*
	 * Not cached?
	 */
	if (unlikely(!class)) {
		class = register_lock_class(lock, subclass, 0);
		if (!class)
			return 0;
	}

	debug_class_ops_inc(class);

	if (very_verbose(class)) {
		printk("\nacquire class [%px] %s", class->key, class->name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		printk(KERN_CONT "\n");
		dump_stack();
	}

	/*
	 * Add the lock to the list of currently held locks.
	 * (we dont increase the depth just yet, up until the
	 * dependency checks are done)
	 */
	depth = curr->lockdep_depth;
	/*
	 * Ran out of static storage for our per-task lock stack again have we?
	 */
	if (DEBUG_LOCKS_WARN_ON(depth >= MAX/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ctype.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
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
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
    $(wildcard include/config/HYPERV) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MSI) \
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
  include/linux/filter.h \
    $(wildcard include/config/BPF_JIT_ALWAYS_ON) \
    $(wildcard include/config/HAVE_EBPF_JIT) \
    $(wildcard include/config/IPV6) \
  include/linux/bpf.h \
  include/uapi/linux/bpf.h \
    $(wildcard include/config/BPF_LIRC_MODE2) \
    $(wildcard include/config/EFFICIENT_UNALIGNED_ACCESS) \
    $(wildcard include/config/IP_ROUTE_CLASSID) \
    $(wildcard include/config/BPF_KPROBE_OVERRIDE) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
    $(wildcard include/config/XFRM) \
  include/uapi/linux/bpf_common.h \
  include/linux/file.h \
  include/linux/rbtree_latch.h \
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
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgELF                      °      4     (               èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[ÃèüÿÿÿS‹P(‹Xú	˜ tú	˜ tT¹êÿÿÿú 	˜ t'‰È[Ãv ¶H|‹CÔº   èüÿÿÿ1É[‰ÈÃ´&    f¶H|‹CÔº   èüÿÿÿ1É[‰ÈÃ´&    f¶H|‹CÔº   èüÿÿÿ1É[‰ÈÃ´&    fèüÿÿÿ‹€”   º   ƒÉ@¶Éèüÿÿÿ1ÀÃ´&    ´&    èüÿÿÿUWVS‰Ã‹@‹P‹Rèüÿÿÿ%   =   „7   ¾ûÿÿÿ[‰ğ^_]Ã´&    t& èüÿÿÿUöÆù‰ÍW‰×º   V‰Æ¸    S»   DØƒÃë´&    ¶SƒÃ„Òt¶‹†”   èüÿÿÿ…Àyã[‰¾0  1À‰®4  ^_]Ã    à                                                                                                                                  °       à          6%s: Standard: %d Hz
 tw9906 èüÿÿÿS‰Ã‹€0  % ù  ƒøÀƒàöƒÀ<PCpPh    èüÿÿÿ‰Øèüÿÿÿ1ÀƒÄ[Ã‹S·CŠ  Q QPÿ²   ‹CTÿ0h    èüÿÿÿC¹À  º8  èüÿÿÿ‰ÇƒÄ…Àu
¾ôÿÿÿé
  ¹`   ‰Ú¯À   èüÿÿÿ1Éº   ‰èj èüÿÿÿ¹ 	˜ º  ‰èj j j jj jjÿj€èüÿÿÿ¹	˜ º  ‰èƒÄ$j j`j jj hÿ   j j èüÿÿÿ¹	˜ º  ‰èƒÄ j j j jj jjÿj€èüÿÿÿ‹·ü   ‰olƒÄ …öt‰èèüÿÿÿé
  Ç‡0   °  ½   º   Ç‡4      ¶M‹‡”   èüÿÿÿ…ÀyƒÃ¾êÿÿÿSh(   èüÿÿÿXZé
  ¶UƒÅ„ÒuÈé
   6%s %d-%04x: chip found @ 0x%02x (%s)
 3%s: error initializing TW9906
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                 	     	ğ            tw9906                                                          à           €                   °                                                                                                                                                              @¢	ğ
Ğ   `~~ W@)U k&l6mğnAo­p  license=GPL v2 description=TW9906 I2C subdev driver  GCC: (GNU) 11.2.0             GNU  À       À                                  ñÿ                            
                   ‡     &   °   !                                	 =       ;    	               O   à   5     \   ;   R   	 n   `         y          ‰      :     ›      `     ¨       
     ¶      
                   Ä            ×       €     å       
                   ø       0       à   0       €   P     #           :     %     U             `             ~             •             ¯             ·             Ó             ì             ù                          +             =             K             _           k             z      
     ‰      0      tw9906.c tw9906_remove tw9906_s_ctrl tw9906_s_video_routing tw9906_log_status tw9906_probe tw9906_probe.cold tw9906_ops tw9906_ctrl_ops initial_registers tw9906_s_std config_50hz.2 config_60hz.1 tw9906_driver_init tw9906_driver tw9906_driver_exit tw9906_id tw9906_core_ops tw9906_video_ops __UNIQUE_ID_license267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free i2c_smbus_write_byte_data _printk v4l2_ctrl_subdev_log_status __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw9906_id_device_table                 !     ]      }            ±     Ê      á     õ   #  !    7    =    e                        h                                              $     )   !  0   "  Z   
  _   !  q   $  ‰     –   %  ¦   &  °     Ç   '  Ñ     î   '  ø       '  )    =    [     m  
  r  !  „     .    y    ‰                 (     )          +  `     l     €     Œ     à          .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .comment .note.GNU-stack .note.gnu.property                                                         @   €                    	   @       Ø  €               )             À  €                   %   	   @       X                  /             @                     8             @                    4   	   @       x  8               E      2       \                   X             z                   T   	   @       °  È      	         g      2         I                 z             Q                    v   	   @       x                  ‰             e  
                  …   	   @       ˜                 ˜             €  Z                  ”   	   @       ¨  0                             Ú  4                  ©      0                          ²              !                     Â             $  (                                L  à              	              ,	  «                               Ø  Õ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          sbase.h \
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
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
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
  include/linux/kallsyms.h \
    $(wildcard include/config/KALLSYMS_ALL) \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64) \
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
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
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
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
  include/linux/sched/mm.h \
    $(wildcard include/config/ARCH_HAS_MEMBARRIER_CALLBACKS) \
  include/linux/sync_core.h \
    $(wildcard include/config/ARCH_HAS_SYNC_CORE_BEFORE_USERMODE) \
  arch/x86/include/asm/sync_core.h \
  include/linux/ioasid.h \
    $(wildcard include/config/IOASID) \
  include/linux/bpfptr.h \
  include/linux/sockptr.h \
  include/linux/bpf_types.h \
    $(wildcard include/config/BPF_LSM) \
    $(wildcard include/config/XDP_SOCKETS) \
  include/linux/compat.h \
    $(wildcard include/config/ARCH_HAS_SYSCALL_WRAPPER) \
    $(wildcard include/config/COMPAT_OLD_SIGACTION) \
    $(wildcard include/config/ODD_RT_SIGACTION) \
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
  include/uapi/linux/if.h \
  include/uapi/linux/libc-compat.h \
  include/uapi/linux/hdlc/ioctl.h \
  include/uapi/linux/aio_abi.h \
  arch/x86/include/asm/compat.h \
  include/linux/sched/task_stack.h \
    $(wildcard include/config/DEBUG_STACK_USAGE) \
  include/uapi/linux/magic.h \
  arch/x86/include/asm/user32.h \
  include/asm-generic/compat.h \
    $(wildcard include/config/COMPAT_FOR_U64_ALIGNMENT) \
  arch/x86/include/asm/syscall_wrapper.h \
  include/linux/skbuff.h \
    $(wildcard include/config/NF_CONNTRACK) \
    $(wildcard include/config/BRIDGE_NETFILTER) \
    $(wildcard include/config/NET_TC_SKB_EXT) \
    $(wildcard include/config/NET_SOCK_MSG) \
    $(wildcard include/config/SKB_EXTENSIONS) \
    $(wildcard include/config/NET_CLS_ACT) \
    $(wildcard include/config/IPV6_NDISC_NODETYPE) \
    $(wildcard include/config/NET_SWITCHDEV) \
    $(wildcard include/config/NET_REDIRECT) \
    $(wildcard include/config/NETFILTER_SKIP_EGRESS) \
    $(wildcard include/config/TLS_DEVICE) \
    $(wildcard include/config/NET_SCHED) \
    $(wildcard include/config/NET_RX_BUSY_POLL) \
    $(wildcard include/config/XPS) \
    $(wildcard include/config/NETWORK_SECMARK) \
    $(wildcard include/config/PAGE_POOL) \
    $(wildcard include/config/HAVE_EFFICIENT_UNALIGNED_ACCESS) \
    $(wildcard include/config/NETWORK_PHY_TIMESTAMPING) \
    $(wildcard include/config/MPTCP) \
    $(wildcard include/config/MCTP_FLOWS) \
    $(wildcard include/config/NETFILTER_XT_TARGET_TRACE) \
    $(wildcard include/config/NF_TABLES) \
    $(wildcard include/config/IP_VS) \
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
  include/linux/dma-mapping.h \
    $(wildcard include/config/DMA_API_DEBUG) \
    $(wildcard include/config/HAS_DMA) \
    $(wildcard include/config/NEED_DMA_MAP_STATE) \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) cmd_drivers/media/i2c/tw9906.o := gcc -Wp,-MMD,drivers/media/i2c/.tw9906.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"tw9906"' -DKBUILD_MODNAME='"tw9906"' -D__KBUILD_MODNAME=kmod_tw9906 -c -o drivers/media/i2c/tw9906.o drivers/media/i2c/tw9906.c 

source_drivers/media/i2c/tw9906.o := drivers/media/i2c/tw9906.c

deps_drivers/media/i2c/tw9906.o := \
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
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
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
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
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
    $(wildcard include/config/Fâƒ:‡=#„—ŞÕ…èiÄP@Á|2øbbÖUÑi.•;7Ç–¼WN3'˜·87¼„JBÊíhe.ÕxèZ—è`¡µ-'Dş	5|3£ë¢"Òd’„–BšgˆÁYº«.ğ«¹¾È—J÷æ²Î=S
ó­_S-`£Ô³„jª¤…òÜ6¬ÚËm,ˆ%•Dç0â.=4AÍfAzÄ	ä QôÔÏÂ}”ÊQÖÿÄ—Ú¼4ä¨mÁ•‹ûFKßIı ‘ÏÚZ2gV›¢íôR×=£80†«{²Ò]ö¶”“2èê¬—ª.GÄÙÅî$‚3^j†" »e×vZ1ÊUP51¸MA<p¦ÔŠëÎ€/‰é8‹b¹w¾ÄKŸÒõ…|s€o[^ EPT"ã£"ÑYmIPDõÜ.âwW^Õx6é0`
z`˜O%9• 4E¬mÁ¿T‹TMññ_¶Ï]Be-Ö^z™q†«ÛŠœ‹²	¯ÖhlÈ^!à>wèTâØÙ^Õ	#cXµäKõ3 SÆ€İ=*<d4+˜ÒU~Ü|½U°µB4ÊèŒİ¤Û³½m;­{O;)•Bu(imJ{6«&¡5¦ªÙ¡*q¨¥öADŸ˜fgF¾¿£]}F—ÇY,ÉŸÉ3Kne³àV\îÃAª­PÇL–ÎiÑ„`uíÏÙÈ[V[óè9*•Ô“e”`(+[HPwäK¨HH)”ËiÕ•”Ñ
àªNm^±´7ş/yÀœavÒi£½ü›1×Àgei¡¬ú£6W™¡ÿ²İ}¦äúS­S™aõò×G#3vıfå˜Y¶0u­ÉÛ¾>©`ŸŒöB‚Îİo›j!¸ÓÛ>š8›èØ~¿NKı‰¬G×ĞXÒE¦}èÓÈÂÔAœÿµx#è©FÙ±GT-·½tÃH•H4´ [vNë,\Óói¬Şª
 @ŸÎoĞX†3Î®Z	ëÇÆ h.^ö±´Ä.Vk. ã(…È#~3 Îé. ÔñæÊ“¸9^¨|s*š=µ[É8†VÜAÒ>ÚYB(;Gónáâ“£±Î5WÀëãVZËbjf™×.—Wúz®Á¨,_3›ØIç¤.Ì×(õç\mÛ^?Ô¼PŠ‹qÖH­1'2YX‰Ö‡³rVùˆL¸Ÿ(Iæ3WÙ`§‡Àß@˜WÊúsçŞÑ··¨JuRvó­zFnsqTHµªûz¦&è#ZÃâƒïÓÆUrï×»"b¹zçÍLÊëˆÙ•®"`à-3hi×“€rmğ€`Az¶Ã2Ñı‹5«koÄarsƒÒç_«3û'.Åw<F3]Ş]BŞÀºã"ëtp´‡jÑÓ¶üë@¯ã	…»\J_6ò“…Xsq>>›á‚¦½^ùşØÏ§‰®—ÑÒ¶
c9êÛp•Qf¸²Ì'b_cùº0Ç‡:QF5[ì¦)¸†%ï8ÅÊ€+‚¯VïB§¾ıUæ…_|rÎ¾döñÚÁi®ÿ\U·FÑsœÒ´mÊõ<èá?°œÃ+I®ÎAÕ¨[ñ93ièæÓrŸºLc¶ğé+Ãå(¿Jú#WÀZÏıeÔ.%E!âQté{šÓßîĞ¶'~ÏYİõ-ºñ‹=¾)¼%ŒZ¼,ÔŠ©D·	¿‚ÕH¿2&¼…T—ÂZ!‘Eoóšº†ÊJÃVãğ÷ûÖ6…­^›ó£1éÉå;"×}¿•®<[õ¬J¢{æxrm*ƒœ9Odö!wÕØÙÛ¡=M-ˆ)ú·,´¿Â¤ÃKä•ÍqÏ¼ı¸VÓ…YÚwËØ¶ïx€¯İ&?rÎê‰sİpª	ÃÂÇQ\\¥±„¤š[áj’â YGgÖë…¹ïV6P<NšÉñu¬3ŒëUî¾¦ÓYÄì—„®ŞøÇ–¨^?ıûŞqİú™…†šMDú¼nØ¿î`Òğf‡“–âİG-TE,sâé:éCk´Ê·Xc=d"(Á7s'gÆ¾ƒ^g3‰‚×Ókïš³î“å€%¦¿Ş1uïû®ãuÍtİëcµaÂÜ ÿKótÔÕÈ¨nA\AaqÛï¶fÜêå{¿¤Äè‹Œô–ìıWWø’k+ƒ¼·ªí« ¢_F¢/êÆœZ;Î8µA\l÷;»<½šØŸÒeÒ ºVÓYô˜µà°iyUÛñN5­—x–Ñrëé£4‚¶®GuNÄ½Ñšä$«ªc¢§Á„ÌÚ‡S­eEæp»2¼Æñ\›ó.A"õ±Lİ¥º)œ¦zôú9ÜèjgÑ«g·I™È SåšPŠzÑf5±o÷k¡´á÷ré‘1Ó_Ö’j¾ ®˜¨Ç|!aM$Ì­É=0Ó#€øzÿÖ÷·0Õ•YX¿CáãÛ™’<È[ÿ6¬³ÙP!i—¢ŒÃ †oDŒrMÔ¸_o˜XÁÖºÙLg pc²¿®h†)¼€Å³Jy85üBñ  †ì²}>ôuÌÿ¡i¯Mùa¿:Ï•ÏòtzèÍ%1½:ùëo›™jÜT‰Ø†îŠJ9Éå¥Ä±ÓtÙ‚ıÓ[Fê„š¾ÈUIUˆJùçB*Ù6^JQ¸i’,éçİBÈ(ÇçDêv¹ãí?åÙ‚mÁÊ]6G=î zL^ŒKónƒ`¦4Ò‹ñºK­ pv„- uÖÁhAÑ´(°î~»Î~¬GL7O<4×+t[g®¹bSu¦è
ÆØ},ìiêıkRqIĞÈãp‚+æ];º«¸‰k>:Gv¡“ˆ¿x“1=Tº3eà”ñØr”@öe–€ßIQ”
ÎY7Õ”G€qWÆ‘ùıb…–Ğãß\,,œ…¸Ÿ$‰İuÚu„ùÁ™«÷À¯àÄ Ñí±ü
¸ÑüRjŞß»ß˜ŒÒŠœ3|á ­ß^t/1t¥1­©UÇÑ˜Z©»9ŠT‹Ãh$­aG^â5Í¬H#CX8©U…Ó·kôC=X‹ùÒ2²€úì¤„/i‰-[İóWó…L´‹Pû™#µòƒ*†çô¼N•{ÚWÓØ’½Õ‚ş)‹Ü·õğ5CŞ miy­b¥^è%¾[
…6F•Åz€0ÏjëêgŠO#âk¿Z}¹]-æ¦È<Kğ¶¸e³y(hç'tâ±©ck³ÛÆX.9Èqo
b0.†;İ:YXv]Ñfñ¾_"¾:¡GUR –v0As‹ş·{²Â÷y-¾Á€ !Ì¸i:ÚÑ´“Izƒ§¬æƒMÔ<dÁáI<óº]áÁÀp­zƒŒuî³°‡ïßH½ P¿ dNX3ØºM“wÿHÏp¨± İ˜Lt¾QUİ±ıqŒ¬iy–¹™A–¯Éµ'r¤é¹uwpÈ[Œ×‡“ªÜâÇDJVô3ÃÔÜ½¯mB½Ud ïSÔ^z6*!ÉO>Ï8âÒ¶Å¼pD¼½?ĞÚ“ßËç]qÀ#S¶°cà»TØ1ş•,-ÆçYz«D››µô»uÒ&Æû’´¨Û1d¬’*Ğòu„Ò™ó¹#aS~Ş	üÕñ7×ô¢1æªH©ˆ¦ŞßZ
G*HœNãäpÅ«©^‚†%Û^ü¡ßWÉì!Bÿuút]dÄ¶:BWi–ë´ãpÁÖ ¦$)ğñûMÊê’—¨ºğû±2¸{¡jíGİ{‘‰Ø©ºÔMZ£$ÀRPtË };ÌuÜã,²ç¯àÌ4¸ºÍïóß‚C)UanœÎcÍi¯´'O;S2ëƒ'™”ï[ÔìëkÑ¼†5¯v‘/<\%°ã«_*®&¬#LôEuúIHûêâpM„õÍ¦¦ÜX{ûtW(6HoõPò¢wø^nQ,u®ÚÒv¥W7@;ÍÑg;¾ÚÕßm‘‰QÊü£^ø)¨6 5¨ùÖe4j(aš¸ƒæå)U•AˆZÌ‘ C‰\ œğûÄYj9µxtö)^ç–>Òjp:
ô'€gO·‚GéÇ¥‡àFÆQ×ØşN„£qíõ^ŸüN}…©üIfÍå†WŞäCDı.ÇÆ±©‹ |Ô§±#øîÁ8{
é/$8ÕŞ\b](ÃjFÀÏÂ)Â¿œÇ1—1=âB¡IOÿÈƒ¾Í*¦ˆ¯OËşõ®ùñ+Gú•¿ŞÃô)ÔZ2)Ğ:2ï*×këêÓáMRƒ¿“Îç…ïêô+D•|Ig~Ù3åcëÑA1˜²ÃæDMª«yµë–Í“ëínÈa5"u×®Œé¸ÇkOáÅ¨·(ÔìÒÇ_<½|WóT¼\­HõåğğzNÃšçVD|j!I:2:W××/G¼ÓP9æ3q?7ğ5iŸ´\#öÎkŸ”¼Fı+<"íg)ÿ¾Ô·V6Ôdš*¢Ã:\9Kšn?åGË·İ¬ÈNbÜG‹bz[çJ F£É±1dğº$uBıaİÙÖfcg´råÜÂ¿ˆ7H³lŸÔíCÌÌ Å±zÂ.:Å‡
{S,ßØy)´ V,sç«ök#á$ EqÔ%×†FX‰õ•;ƒ˜¨3¹ùËœ`’ã·ÍfßLCÔşËÂeì‚{Ó8³ë§f!±…'I
¸ş÷M ”Úİ+ï+”,ÀIIMøû/Âõ©ÃƒtŠÉ@Lç;lÊÛ7%U^=‚étr«….Ë©å)ì«[î’ÿ"ÅñĞÛ­tåØíØ“FšÒ«¿ŞközhpÀáyFë?,~Çş0Ä99ø;ıµƒxVz‘êÄŞıéø­#ˆ+Øøã¯È+1*a x/~ån¿‡òÄ®4]nüí„A¿W‘áÈÊäS>„êh¤8"ÃÅ,›ÓrÅAÕ‚rW×p¡H:SÑ”lbÀg_ÏkÖ·Øp^ªh³¶Z|²êã £ÿv5èş3_|Ìó¡.ù/Iø0_T,RuşÆKŒÓõÎô0õlà²@‡®Ÿú»Òˆ¢JË”ói„Œÿ²xãÙ¢ ¹Í³¬àUMä?OË,Råˆ‡GËtd·ÿ¥Bj>ÜpGp[ge3© ÛßÌz\ƒˆ‘iğâ¶·«Zö—òoõ%‹dú_ÒÅø}væRıafzõa®]1°Sæ€š„Ã[;5‹×€7§Šóş?æ{{Bo ’Zˆh—HÿÎhßJŞ¨:‘I„®·MQ™–ëæââÖÀûÛUñü „á¤â‚„ng½
ì}kZ|ãIçôë+Â?«&çÉÚö‘'À±øæóš\o½Á-oçİx-Š¸èÍ7Ïûê“¨Ü`¤ÌZ¾6ù;\Å›ª¤ô«Æí+Ê× ú—şƒY­é‰H˜óRcSîµæf ı—J`=ŒÛ®gÇŞ‘m {°¿ÙTçN¼zõ¨‰KûQçnæá•FB&¾h#Øxüéœ)ÖHÎi9Éf»dÔş²›ªK®ÿ±¯¥9gı!ÄÛëÚÃÖÂÄ¥Ë$¨¥3ÛX¨§¢[¥ÎZøè`ÇŠ]‹ÿû6)ä4¢?JÕú' «²ZÔû\DÔÚåÖ»ÈÿÒ-œééµ¢‚ï\Azã+şup3ºÍ„I†~Cij_L›®ÛÖ€§Ğ«M’
ç›5Ç4.»ÌÍÛÙ; ‹Î’Ã“N[C¢âªD…N¦Á9öD×²U±~±ùêğE‹Îf>¬µYÙ¾&]úñ†Çæˆ€ÓiªÓ{n²Oûp]Ê›œ·¡zÌ ŞEÉ«¨¨W—"]/P™ò‹““ã¿q‰J5Aaç÷Ób¬•ÆX…‰Àô„ª@#ê4´eğ@4óŞQ¶ø9Ù»ö„Øø^¡>°œ™¸˜€KnŸ>n¤İ/Ss ¼®^ÁlFñálö1Ú¶	îö™âl«§ŞEV—êEÂ3Qzs7b@&>1­ş›”ÿWòË.·áKm4<št‰ñk¾0šL5¬‚LñmC»óv›ö‰3`ˆM0iç¤œ(¾*„ˆA¡É½LôPI&Vÿ_KQÉ&Õ¬µ_'„5Ûçï^ª½Ùf¶àô¹ÓjN® <{á4Y8t^–7:Îé’ıLÊùÖxù-J)¶ZÛÇJ¤L³JÁÍmº|VšªKÿ‰xt>Ÿ÷MŒê¼#Ñ$¶Æ	ß>°
Ok—
ô…¶>à©ù&Ü?-?r‰iÌòi¶ó§×¿#OÌXe—ÔIıƒ›fı)ÌRküœŒšåëæ÷VıÏ%Hw¦&ºº,=¿ºïó’—Æç.w‰`–Èµ?ò6q³Œÿd§Ûßá‡hZô²Àã@	ùÍÚñÅâğ×R®İOÉ‘¥Ğ+%úÆ#¼_N¦›üö;ÏS¿íøæK2O-¡òlÒıxføa4Wû{–ªIYÎÈ7E§îõÙ Ÿ‘gãB
‚Dœ`½rüp®‚Sä®"!J
¡/©ùßèÈ×8işG	Å33Ï——öëÿJœ}A3/#£$“c9·öµ^oÈ¼¥BËsÙ‚-š’?!GQ@|£½œ&ÊÓ,ÛdÏM!G†ÿ‹ê;"²ÜšË¾ë„^¤´x6êV»êi²`õ•I‚#,Î•G¨¢¼\É&’]”ïëİdù_ù?}KÃ¯doäï’"©+â’q¢;è…!%˜+ãK^Ç#§^½Ú?ê¯?ÿíf'í7¾ú%²æ'$Á›ùÎÕ"n`UÈJ¥År4œÕéåªôü1âÏÛáĞŒbVÈÙ'ƒ8ÚjšÛÅ…/'ÃmNòó¼Áƒ	U 2LjFÖJËôŸáhBA· ‹rÁ\hÚ§8UÎ&Ùèâ 3¯
ÁÌƒË-ú]k	ü¥ìš›¤Ä%õ¬Q…ƒ{©ŞíŠ™›Û
¯!?òÛ"__Æ1	¥6=f§´ú9î¸›j¢KîH‚T}Ğikz6
ÜªdØt»<7S‡YüÚ“Ê·&	1M‘*“É…×Eîp×ê&ê˜!v¿‰3î ‘ =u³½ıœG„áû:[}#?BÒIûòÁ~%š‡Ïş¾ ÷‹€ıÙ½İ–·?èUuí³›º$„Âß¢Ò¾Ì‹v1ìDõj¹ëÚKi‹:²ƒOöÚÇÉåWtJNÄ¥=HÄsôøjMÏÊ`üûfıÈÙÉ5«.„„IPÚ0ßQÃ—›èŠ5»Ã$£fjB¯–nW´
Q8sŠ¦	|Ti–Åû8¦‡;¨…‚&ó´Ä>Ñ7eˆËñóêN+	LçtÉ°Z ®ñz†Ÿä!Í‚.]¢3%`¸kİµHyà¢M7×³¯ë DRÛa8­Ñ1Nmh«Æ/ºyr2bV#£ÛÜ¡áÉqõirMd›nrÈ)¸âD¾”KHÄG¾>E¹®!.˜Lƒc4€\¤ÎıÀ—çTK¨kQ½`©+³äV–ÁKÆËÚßEÿ‡÷ĞfxDZú•©2p‚ôÎø*k2h§–\%NjnXø|÷ãnÎŸÄi'€Ú‡\R\è$@y«xU–H	tØ	ÓèíZ%œOñ¥pƒ–çÿï¡rvÒóÆš@ôêöåö•‡¾tFÕœ(;9”ïöQ‰ØßVğÄ>0x¾KÌ•ô.$N¡ÈÁ*ç 5©•-8-üÜóÖÍÜ»K˜}ftb‹ŒãÜ.-‚ãQgÌŞ‡w¯øöS'rPo™Ôå,ÎüÅBa“dqãmÈ­‘óÿ„oa_Äæ)Tt^T[Hïì:îÕ°eZ¦æºÎ[AÀñÌA<ŠŒ‰%Õê{‘}_3…:£¶hî˜pá<ëê®Åkƒô˜Î-¥'áCÂÄí€Z§yLwÿˆˆZš_ĞÏ7‡(£ñSúTĞW¹# ÒíK‘v+)û»m&ÕÔ\>—"’D2EùúBväi£êè„ÀÏÙá7CÏqwzj—:F‰$yU4±³ºçK
a$¤âà x5ß~¹¶%í™úKª23·YÛÖ‚õBš˜? ßwÜÜæÃİÓµô"Ğ>î¢hOT%æò1o¯ßqNsVSm9‡©äşYÍjI½
mş¡'`×ĞS…·‰ÀúúNvW:CÉÓâP1m³0Ÿ­rìã²ç»‡	}GØ„J^LŠv_2Èü—Ë%³Rû³áÃV˜HàŠ•%šs©ÿÌ—§Ğ³#·¹Ğ½TkP%9,°öOÃÉâÌ‚–£nmJ•¸e;.>(·äGqÜ†\¶n/[ìçÚ¹7Ã ×S¤Ûr'â«dd\¿]¼›T.|Må}¿5®p 
U"~	bÎ	È›Šbú ³ºÊ´a?tJĞ÷»ş¬zmğR­ƒ¨Êónáúç­Xã˜Tı^>W"LÉ<ÅU½¯ÅåE›‰?­şÒ”¤T•"ˆ÷ûLboø§İøYaÔ»ğĞø7È˜—öÁ¢MJå7[yÂ99V{ZÛU4YBñÕ
ÍØ.à‹ˆ—¡O }åUJ8'a&E[ıøÓÔe— ›sGHÛ1Ø®s)<"}Ø;ÚUÿ*òŒ¿®g‚r7GÇ?PÇ˜GáîıĞnv§öthëƒDªS0â€—LÄ~.Ryâ~Î’d+’óvK¡›#µ/	&Wï¸·×ÍÆDÆ6”`—(å—ÜÀS9Â$Ö…>¬‚2[‚³6½Ó¾	ˆahkÚüRÓÏÅ^Ûã'ã¯Ïñê²ÿµ9Z057XÃÙ–1/s‡TmXóP¨±ZÊ‚xÕ=bŠR\lw=­Ì Ñ­ RÎ«ı›ñµÙj/Iú¸"Œõ‡G™‰}!ÃF#oø[9›¹Î- ú[•ŠI¢·Sxä¥À²«ÖéµàçDœŸ_½ê¥ÇŠACÒ:Î¹Ê…ĞC—?°dQ°ß)¯_rFZ!ÿªdŞ4Z”…ê‚!NÄ§w4˜¥€¨\-ivb¬¥–.š‡=oIœ¦	5Q#à,¼ĞÛ(«øXu·ü…} øHAØìû&y¸ì_ŞxƒòêöxS“İˆœş|%[dåQwyJ¡ƒ¼ÉİµÁA‘í9ÎT>ÕÏ1î™½ÀªGßàe}T¸¡j.°Ø¬"i›UDµâ¹¶-—(NÒ^‰>¥W”wM#‚`ÑûÔëÄÕË
êlCÇĞÏ4€ƒA"ËZhK@z€¡1“Æ{çŸw;î!`1¯ü©åO,İÀaySçµò·aêÌ©ÄM·;HŞÏ¡áµ˜H)¨<qeyÛÎo»„ïÈ>õ<¡ã(B-‚Zx’ÌU°\}&æ®ĞFÃÅ'Ò˜Ä!ˆÅ–©*wr¯ø±™ğø&Kâ ®H<’ŸqƒF‘lÑ\Å¬²5®5è¯ı
‡Ã–}ÛòUışid–H?„_o¦»óä^"l°tnHßóR`UAU„‘dİÏ]€¡ÔœÖ>5>.9*â.ÂP4œ£Áœé¯DñÿÿÑ««ó>ƒfÍh-èú‡ERWÉÈÉ–çğúaøĞkoX†¾G,©=â¯ G@3Ér3ñÇ&®!­é†ÄMU ²ÒÄ)	ß!Åéğ<ªT!¼óå†çî(Mwù>±¬9óŸo–Â1ÜÓ/bk…bëóÛg#à›Ky÷ó“Gˆ%\Ş]Oƒ€êÌë'ï»Á«@|öŠu&õl.\<;¾šNLÜ\r¨r§cLçš$ø¾~½Ç3uxt5¤ÄÑ§'eƒ—K8¢}Är/“ód<†â¿ÿé§¾²Q6û –Íz€D1_D<7í¯OBM¿+£ºQrğ¤¾‹±Lk™Ê$KÜ*ÖÌÌü,^MØà4º±,ÏFÅ¤%Š¹Ğ«ÛâáÅÎS´2¿0qŞu ‡~Á¶#ÿ'Ç{~“a*şVB†³cµÔÓ­ÛÕh_y#A{‹– å+¿n©ş°&9œşô$OY¶_ÍM½Nó«Vp.+&,ş˜ŒCSûªœ‰­zŒ©ÍV8N»zä1'¬¸y–ò5v§#¹®S2‡(eå¯WI¿{xªÂÌŠ£ğÙl&C’|G¤Çt ©Ó^e‡à\³O&5Ê5à_úV-\ú»ù‚EÄ»#:¦Ëô	u¸çdVVs¶Í³­6û~ät1ÃÔ©†N€ı˜ €î)'Î¿<ğÌ:8*õä¥%
7›KOâ!—ÁÙeC¼³×|ˆo²zò²&›ÏŠÍœP#ÆÌSaPöŸZşª$$…Sx.¸7‡©’ÛZOVoa^b,‰ÅYŞ9*¬±~y®•"ˆ§VÄÕŞM>*‡SÌ.Ñ4Å©}'„¶Aª²´\L¥é"§øä®5Ú¿À^Õ.¼÷ú*£±dê;>^“GLrìm­İ5ò­*ï&£í$6ôMeØšÆô?[i?/Ïzâ:Dİ#«lÏåñõ s¾-·Ó©×‹ ®6ÁÉÇûşÖD¬_jX?‹îÆ;P4¶™õop¯¬©cûÛÁ·¶U÷Iab×FìGÉİv4+NNÛ^M—Ëÿ„ÄØ¹Lgbú†Ü#F§2d9Û2ÙBè?[KêRÿBÏ·Ûğé£ÿYê\H’õä¹M£µ§«m„‹E¬ğ^ñ'¡Š
èŠúÄòr;­ëÅÿÛLûáWŸp6ä!Ä½Ó¡÷õ"_ÇrÂ;SØÜ_ã2¯‡ho[¼~$ásX¥†ñ®RèÓïhÛ¿›Ô5nŸÌ[‹‹^aŒ>]¢_³ûáİô×)™±.?Ö(SŞšY¨)õ4Ù$ˆlÀ8˜WQæ:oyAkˆ/7\ÑO ©„şüE:àC¯|¸á¨q–X9I><·Ü/7Ûø¼„5j´òsjÈ‚¨MCÚ™Å0Ù¨åU\ºnüp£Ò#È¸#Â3™ e¸ĞMÍ±,S¦:‚ÂöAœjdmh|³Ó-ş»<IİäFghµÑï”<FÃ.¿%2°¿ËÔ¢÷şl×]¯¬ Í,»Ûv†vıL®!Â‚+ûª™0§ùŒWªšrD¾»ÕGÇ˜»Ûã+\±Ìƒ×xn-Ù†˜zKÃ"ßOÎ~Ñ-¬d(*lÃr1—º¸j~I€P²s:$|ÕNœ{¹#îŸà¯š:Õ,•§¦ØxŠC8×n>ñ¿¡ˆÉ~å2±í=²ŠÇ'‘iï&$Òİzg¸õ5|Rö#Réì¦‹J° R=BUóK$º
İ¡É’™zQôbÓÆ‰KØs_ææòNï
\Î^åê<ê¹í€¹†Ò|˜(Ş~L$Øğ¹x1;ÌQ•óVŞsØ/]íï¯ëš¤…éıtXn†”«—ïÏğ’(‘"YÑ“[ÑtG*„¼ËJkQÆß_¢âİõEypDS¤'Ûìõ‰œÇ®Úäb2ì€ú§ &ÖW÷ó9èx	2(Â¸^L¶¢”Nt8î)zÔé «s.üÃğ‰=Éı×>êIgBÖ‹±iLÇR=>¹«œzüÕÿ¦åL-VõVğkÒ!HKeßŒvNÊG¬Q1Ò·€86fC§œût_¨O„ˆƒ"WËRî¥¹…¹I(zt$‚
|	: q] i9Í_R•aÏF$iùFşœÎHF-~ì™cß².
vJŸ&Ç‚ĞˆÓÔ-#ªØõ8á7zÓ}0ºŸ'ØÎ³ÇjÔĞ4åJëV…;õ1iâwæû;¸wÁ¿á­QµÚ—icĞ6z+UË°¬~%@j³ÎŒÍ­k(­¦:P$o	]m@r’Êöšƒ„#ÉÃ6#¹·,Gí¶4u¥Â3<AÑ—P1„æûl•V	$éFÊ› Òè3ñâ2Ğ¦hÑëœPÁ‚¤œcÚ»,’U&Cí†\ã1Lì%iA´Ìk;­N8È^íå~#¥L rqÊ@æh›ZªÀÅ“ç² JÏSÒ#ZR´)ŒüÁE”ò-â_›ÜÔ‚SPLrûXàånLªR²†GÓŞÏ­…ÉÌ\±?³m-°Eì ïñc]«d5µÓ£¿£³Ó¤À$‰_ÄX–äò’¾í]»ö¤CºT-À®¶dâ–ğ&½‰7{”ßÂ»¨Ñš9ÀU¢y²–2¶hdEÍÈ»Y=ìªr”SæšƒP!(Ú8Ft«W™„uÌœİ»Û7†ò[&KÁ:©.R»i?¶é®]œ‘ó‡)íEFÜ
iI@¸" ¡ IAËM´È)}–Û‚Fùñ^5Ôoõ.àG ‰k œöÁí²™¡Šè6ÛiÀûà=Íz£“úÁU!Ø˜¤z¬:"òÎï.’MÎÍÁhûé‘'ş£,»¼Œ!Œ§âÆâ>,¥çê§NjÛø{sv05âås0x™7Sğáh0ãázÍ‹¤/îB©|}N•:k-%ãÜü¬‚£&êm.Çg4`%æ ãÊ
’ŞrÏ.O¨A¯¤7ó€¢Y6€"éòpÓpPéêÆpÔ('c´‡³}é—ô£"e¶ŠóRˆ·+8×Õš?§jZ,ßt²‚„‚Ñ36¦À¬Px„œ’®ÉĞˆ:®Mİe:ğXi¯„6î–ÃN<§	Yİyóå÷7¶¶e áÜ¬u=¨ÌnåX·;y£S»#VKê…R èÃWôÕŒ´’^§ûŒ š7©rÕYJÃ$u“–ç« È®0¯ÔƒÀ(¸MßĞÆE©önoæŒ
0ŸĞ°d‘Y&Vù¬…İ[büÌ*xœê¾){Ñ*%ºıĞ1ÖNğœ?¤1WÛs».ıB‚iTîd(çõ„3Ø%5ğÀñ¶i&||ÿ¢Øî©º>èQò
>©ìo	£k:ì¸€Ë_¡b!©—ş¸*ö=„»ì5S{K;ğJ³Î‡UQ¶àŸ¸®(şÂQœ÷<Íµg¥P#òY[™ñ`¦¤—Úü¡g‰p¢xA;äùhÚ®ŞÏ•LÙ™¡Cgw´ò¥éŞ›ÔÏˆËl‘:6-:Rñ–ØåM03f4¿ÉÓiğ‚sçXæéÆ¹@Î"J“©?™x;P1Š¢9™üÀU #2şô©Æ+¡óİqD<—ÇÃ5B›qñıÈ†%*zŒ[ğ†‚‰Lc†·"±¼E»ÕÉ9äçôv©÷öŒoË…µÀ{iilOÉO,]Sü‹2†íáaJÒ[ñˆ1ñƒW(.Ôà‰]“§§¬˜ûs³o±sÛœ0 -×ŸŒ­¡"¼y‚îÂx°rŠ3¾2õ\>V–ëŸ
Âõ8Û±;:p×Ç™nV„  zĞ(ZÚMV³íf¨XAúc“áüİ˜œeİ¨÷”Ì¹Ğø‹%*å•Ş¶œ“ìEùKïSh£Ö×¹ñmı!£Ñ	J¦4´i±_ûÊ´v§¯ø1Êl–thãR‹"X‹Jx'óVPı*„8€jKTÁaÎ”{f>Òh¹Ù!Û?÷	)lâñ¼jğfªË°‚’F²ÒP1‡µ]ÛiñÎ9xt‘Åò¡7£ò–0Æ Lç…ô”qQº.|İÇ˜9 Â"„’5í³9¨½P”òÕQÔ÷Áp#?#¡‚ÌäZs%I+~?gÕùG„ØüS‘Ë0=ô¾y•}|k<És¼:m®
ü{ÆŒÏ¸=œÔªvòÿ˜*ßTÓ±%SlèÅÉTRÀfø%DñT“]û¸vÍÌtZ4æà*n†A5 ¾>îõà‚K-U©§üÎzó	s¥v|6MıN‘ŞÛ°“?ÏxzŠö‹SĞl·³Ëæ[³ŠÕÚNúÄ¢½dÏ`*C¬pØv?Üq½NËtB'ş‰QMås6‘tg.¶ åUÅ´„ázÈ`–Hïyd1ú.ã'5ï¹Ç»ûK`q©¬3zí‡ÜÅ¨ `Mà£\uÙÏ)*èÃ1,*ºnjÁ\jf*·ĞJF@/¦MÏŠ@ÆÑ 8{´\Òl·_'5ÁÉ7@>¢ÜOcAZ?ËÌôŸJaÔ)ÅòµÑ`½oi=saù™›Êv'Jş…ÜÒâ„–Š±gBp†8³/YÜÉv‹±-À»pX§a_½à†·_S1¬™pèåÕbª¼ß0S 0'Èd•`Q|Q$ø³âÌ¼RoW¼Œ€T"Cìú=Qâ³õGaÍÈùôdÙf0÷g÷º€dC­TËaÕÔja' 5ü½‘³d?8T‰‰´…¬Œyâôõ÷=ÉJYì³[ğÔç–æ¢`‡Ãtmfõ#†ÒRŸğ†ø~T$Œ?ÈsĞÊ6©õ´ÓşÌ‹Ã’ë()gq=‚vlñİêùÊ
2ó)*¦òÈ< ëE3œk7Uøœ—Q6¥<'…‹îÒÀ8ã5©±/œ[öí¤ +íc”Oª£Æ?Ksg€CÃŒMà†8!è–>.ó`ú¹P5ª©1çŒ„úœ/’
ºšJMo ‚K[f¬ûÙÇËíáàŸ´ï} ¢ívK2H¶`êë”ˆú|<Æáo	\e6J­Ò¯\ˆ\›³âI‚ëb³¿—İ§É`§î`Ú“Ù7áuÛ	õ¡Í¦òO,1Zt¼ú`¢çè©!İi\š1ªq©ş¼£îagPÒ¯.-ŸÎ¨Ò!ÕòÏìLá!ÃÍ ±‰ğoÙ›!çªÖÒ­-ä/»¿/ÿ±Ty·,€
ıFL–•T7¥“”©èNñ˜ıgéçÜ|ÒFIî¶$âoÇ:Ñú!–„5‚:°˜Ir|=;øA”éY$Ø(ûŠ]Ü=âØĞ>õ×U?mÅ‡Î¢‡^(j¿k¢:½)af¿ü…`¹6;Iö¯±œ¶ÓçtE#ì˜ÜÃ»ó‰±¼©r®ç’NR‡,L¦ şRWHÅQºÑAp|ƒV–¤å¶Ï$%ãJöµ/Ü†FLÛ2¥=T=s:#ÏzAk«$R´„•gòÍ+‡êÉ.}£ğ
‘ `[@ğ£Ab¹4RQi¶£¹wk“>-£‡ªò—h"U(äÍÜšA*#ÅàhÄ}a%ã‰Ùî:^œ¯÷N±Kô½UÍpyª91CG5–ƒêJFÛ`l5ºÑr•\ `c3ÎÈ³ËdÖ¹{¾¼°DyøĞúîe</³ ºZÔ±d¸ªÇ«eÑq 0~Ze–m'ËŞIåKíöá0úÇĞì»¬j
FÓ_ô|›"¯Üz¤q#ğ?h†X+WüPR[_B	,dÜ]Œ²÷–›Å¾3mYù»Æ~6Ï=ãÃ¿M–u%Ÿ<oˆ4c”¢{cc­[È¹±PjuÄC¿—ÑS»ä·é8¿,Z¨…ÔRwŒ¸$Õ|ptı;ÚTlLÍÀî1”;3¬fßµFß©9Ìõƒc<Pt–bOk
XOÄÌ{ÎR­øN]¶W2Ï¿ó{rİêÒ°Ö6Ê—.&”£UÅÏ Ï• ¡kB®ˆëál‘ôD¶CB¶0O,|aXÿĞ×´ÑËø¦›6Hº˜Ìâ35áŞã¨zìo·ù2Ş9“a@1Ÿ…ˆˆpµjË{A”qİÿ—>”ÒÎ» ‹–ûó¤CÀµ…[W[æé]×ø2LO]¾çÕ „¾Xm‰W$TÊU€Ÿ§#[`c£ÒÃö¹‘r%«>ÒsDIiYb<¹àõ¡ºm×	İm|> „˜ıèeˆ'È@âkX]¼4ÉíÃ(Âfcúø_Uà3ÄÜŞ#©Ê»4|Qëâ)šÖ£½XÀc¦ÏıjÊıYòã6ä-ğjäzië<í@VyšFü%.Vƒ–ş¾25·ÛŸ¬ÍÑ¡°Ù“4Ê|hıwA¯¾ıÄÿ·´´Œİá´ JÏ~UB‘‘à~ØòZH{X´™Şe£Ÿ¸…–äÓ5(«0ƒgIğ”oe†èO×ŒÈ.ÛxU×ã)$å³GEÏûÌÂ›QÂS­™“‹Eà=`ı¬;ËŸgh=PÑs=R8yúuÌŞØiÌh¿&÷“w£/v~v~¶Äğœ]²4“1†¶">L_Ü!EgTl›²+n‹Ş )c¤nNë;BÀ¦a
èŒçRm-Èìcµ¨^  ( ´	î³ª3‚İ›<¼A~-îÒ$)÷ƒBZ`ïj´q< ›Æ@ÔF\“y(Ï–e:aÅBØĞÉĞÍv@œŠR¡‰Û­tªìò(¥¤Óe8ª*‚ëåÍ=í4.2’•°s¼"Åş.~àŠZ ßëú8U´<©úN
şñpW3½ÜÌÃªöb±fUŸ¹BWÆÓœ·èˆ¿·»ï[ˆ<?˜'M‘R”â­GhÚå@t#DìS'Q¾s$Él5^GË…<ëõwİå°*\–*@Ûu5ælı Iæ`¬ˆ€5ÿX]^Fòñ2	¯Î1Jz`­ Uí.‡qßğjøT.{R…k×Ågøf‰e‘<r¡‘èoiH8Ç÷EÇ’]ªtsª(6^&ş#.›#ÊvêaJá–FêŸÔŸV¨|½5m­T$~›¢¨Ltåi#šq‚[Âów{Ì˜ÿnø™\£-ãL»öNœËò‘TMDäSi šÖu²ˆ¦…ê%‘Ô¶¹Í[Ì™S¡Œ¼36¼’ÑVÈX	ÑgK7•hAÙàš¬¦Ô4¿ÉGé‚jõÀÉ«çÂ	,óéç ·$© ŒOlKºBhmwéÔšä9ÊxıX.‰ªvæƒ Ó9*±²ğ‰ngm’6mğä1¨¾8JûÄØ:¯ñp;T±²ÆÁN¤İ=5Ë8[N¤j³Œ)‚î™ë½ÇM$=qØæS±|A&¿Ö½›<>æ]æs;ºKÄ›Soe
­VP	áqÒğ 2²$İ…?x×š1š†ú“™#û›n{½4•m—û9œJÜ
Ø^v`IˆÓ“®ùäñ#˜ê.ëªÔ•¿Õ”(ôt¾4új ²%«í’4‡;L›&Ÿl1İéoa²ºªÑx7{]3aæyÄçDw-*…,n ÑçHè$«@—G±J¬LáÈ=Ïëë-°vË¢ 9v¼•mÙï´‚­@£ÏjáRË#BŞ¦îkªæãY7MH[Írîlj2ìa‘<v¼Š§ÌwÉÊôúT#<p˜¼0ÍLgzÃÉpPog€öW;´¸§“ã…!ÃõctA“Šæƒÿ8X¦cš\?e‰Ş`sWÀ³'×İicÇP(›İ±Ü³e”§Ûz÷’(ÒEÍ'Â¼hÿFtVŠ¥dí£Û›NĞZĞÀìşs^‘š¸yƒ•	¤[Z•iS°ƒ¸°9Ö­H¾±Y âÓö£Æˆ1c‹éª†Y”¹
çÈM+ú­üÚöºlc¨«&†:$o×–BU÷P¥¼Ğ—}É‡?ÕXùÀºg¬RÉ’V0yYjlMZ‡¯§GŠD•Düiú–ÁğšaÒ(ÔCäô@…Eï’îôy‘ÑDEË¯ä‹$ñ‚KoaÕöÖŞ§\÷š{Æv/ø£•ËixxTÈ\İ ÅbˆÃ$ÊÉsÌŞyíËà½F„Ê «®³ÃöÃDë
)’:UßG´L×ØĞZaªâU9è,ãüiÙ|:bŒÂ—?q2 Â´İ°ÈÈÛ7ñ\B ƒ9*I¨¯B¦‰3ì­%ayz/Ä|wb}Ş#ºì‰;©`3jÅ1$Ñ$ÇpÓáûÇ¿ôfÆmz`ÃÂ}5|‡yÉÀ]e#¸¯0Eã¯)î9ü»~eq¥ÏK\P¨Õé#m—Ï ¹JCª÷ºÈTÙ"H[Ò;âvÒEOö}ü„­©\BAZÂnî8	#ååÁœqèFQæ 	X;é\Œ3	°‹ëÖ;jº~ÂnÓ.ÙŞôéÈd 8lüÆT›¥v{ûŞ1W<;^”Î.©@9÷çµêªöí©5Î÷”İKóéôİø F,4ÛGFo®¸½°~Ò6©vÉÂ9&åüBí´ıŸö:µR+gú
/ôLWˆx~é*c,¹?|z²û	7âÌPÊ¿ #66+Ÿˆ¸ëU<±éøkÏ§üyóÏáP@G€¢É _7Ò6DZÓÛ]¥»Ş¥1v­é²Ã|%š
œÄ—Ûå‘bË=×håüÊş|Eùx‚>åÌø«=c^¬É¯`¼[hû.ò0Î"ÄáÕ.·HºßÅø¶Ï¥³‡%ó'»;	m€4yl²FF§¾•‡•ÙûvHB\¶™öÁOŠ›ü$¯3³|‡«oL @qº!:àkZ³q\(º‡ÁLéLt¨¶uóèv¥ª#j^©ILj‘vCËL®&Ôå¿Én~§	ÿ#T¥é:ƒwšü$Ã*ˆ¤¸k}d‡^oL?s¾¬Bµ[6qHÊJÎü¾´}SŞ³µÛ Ú>D)cáØ^|$ssj¾^4<Ûİ/Ğ¿s¹éÜtÒÌóïMŸm\­0Ñğ¤†¼ÜP†Ü´ŸKùæp·ˆ ÛÿûÔÉî¨ÙÃ±÷µNŸ²^|p¼¾™dr}à†Äºßğ;¯}°	 !"àvè×ñ	­fÃîq¯’G?Ëü İ8bC~aEL=œ:¢­ámğºİŠş {};úxı» q´Ê¥Éf\	uÙùtªÏÉ·J9©ô¦'-®Â=´ ©&/o­FÑ,ë3eÒ¬ÄÍ1p*\æ®§ü|‚i`xƒ7L«Ç–ì¢<¿ĞFÓãÁİ•›éâ‹
ÿ”ğPPõ‚ë³w}à6Å)ç,ÀW‘ N²è-İ#¿'üIÇ€HX'$†;éš¬*ú‚±ÔĞóµrh×F;ìğæ9“zë"7l"nÆóÄîĞºu‡b²€m²r\rlgè'ìüø°¹•ª÷LV.ñÌÍÎÇ7‘5Áv$»Ñİ÷ 41c1_(h´×~i+2w‰v„„@œ-âff~OO ÿÚBØùbM“×2	m·Ct9ga9a¹øüV1ÑïVÔ©îÿhŠZ÷ŞÌ%bÏï~`°·ZP$øl0ÔYÌtİ,Â§rYÄ‘{Ç¨ék—IÓ"Æ …§ª &C?FXEŸÉÉhrà.|¸›L’µÖ|(¼sûìa,[¸T