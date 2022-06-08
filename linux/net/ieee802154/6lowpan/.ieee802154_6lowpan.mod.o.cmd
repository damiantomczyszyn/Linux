OC_FS) \
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

drivers/media/pci/cx18/cx18-alsa.mod.o: $(deps_drivers/media/pci/cx18/cx18-alsa.mod.o)

$(deps_drivers/media/pci/cx18/cx18-alsa.mod.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               .ã{Š}äg?L¨ÿ¶kì±lÌŸkH ¬DŸ@ößkˆ/ÚÆ¼ƒßÏ›:WjT!S#½e¨d4˜ÉÈú¼ÈBÿªp,Ô«¾{½šilä	âÌ¯—¿ª
½sÍçCAÚcâÀÖ¯Ú6˜‘³¬‚@~Ğ8ò>"ÂAãskırè.l¤~¶ O1¸te§VG?µúpÍ–J:¸§µyø;ÌÃªæM7%HÂÑ›JU–•UJY" 76FWÄ3';£Ö7Z^Qrq",È÷ ËbqÀmÑFaC£æšg¢ëÙbwg—Iad¥¨7÷AU|‹“é•s® ‚­›‹ŸIãTe¨ÈWà)ØÂ|›*©;„1œ\B(G(¢ù¯~5¯¤´$¾æ»¤Ø©–N\	¸v! :oöB«%¢˜wÜ­êØu%Jú„Ü¤ÃÀÜêuÅøæeßàŞºOGÎ*hrux¤ns	 Ï¶Óª¯«Æ·Æ%ø¤ád­}{%Í¿—’åÉ9µaÅEmzÍˆšl«XG€<ïnÈé<âõ{T#†¶$ŸE˜m©RS2^*%ædàŠU„bê0ı¢ÉòxN¼U@jşÜ†Ê†AğJl´âs|AiÍÈ‚åşÈ0™#FV•èlÉ¼¢’%59‰ÒM¿S•IñæÜn^0Ï8!ÖR¶}Ï©b‰¸L‘Îh¦˜;ÚYC’°wºŸÒ&Ü$ÁÛæ·´¸Ú4™–±·ŸL?lnÈÜ{…„‰÷ë@äÄˆ»rğu\ˆC¥¶di³Mƒ0Üké,a|s¡3<;oWVŞp/¿%­0o…İ¹—7V\7U0 ·²åŞ«XÉ¸~Ì|~3}‡"2x²ÖçgÔÀlúoÓXYóÿîå+‚úí8äŸè+î½—Hòj¯\Åı«:ãŞâ˜Ş$ÙùŞÍƒ° "âé#>~Î‡ßÕê%‰Å,*¤@ş¿ğ 1Ş®øcÅÚ”C[pÿ:È‡ë‰›âvŞ__kHk=3[±ÃävRií=¨Ò¢¸‡´p@Âú Sç·¯©VÔj0dJãò{ÛÌCŸrÒát‹¦5„UÂ¥[²œYp­	aŠsYoËG@6Ö‹ákl„“£YêÀ’Ïà]iqóuƒOB{ÀQD‡tÕ£<ô3¼CÕ…NÀ6ß$ø3LõfóuDï«=ãfáyµ¯…½X—yÆ @2Ğ*ƒô<OP[‘’Q»KM½‡!-ßŞ"è]T£¶GBÌÒ>s£wT²­Åë5Ìï ã‚çsšš¹VüC/Ô'•õtc5â@FñF2ÿ2øú¶ÍãÎ5sˆ2ôÅ¿ ¥@ÕÏ'İ´ìì±ûŠÜ‹ÿ­™ˆìWt"t…“Œ+ö& «©ÊÍ?Ÿ
M{za˜ˆ(ZŞ‹gêÌ½•„_‡y?*8Óâød>ná*6f([â²ƒÈ0¯…½¨îB S€"z¡i1WtŒ›cd¥ıcp‘
+B#%ÑÛ©"Z¤ŠjøíSè*lĞÈiÌØŞ!9/.‚×ã2„ç¼''ÍÆq6¦ËSzWÓ>Í¨pê;0 .ÜÏ?jFÁÉ›ùm*ØÇ¼0İ<÷¦F»¿•`R+`,kL*¿³w¯dR#&q9Eq¢â:ç8Ÿw6tB¯vŸ–wƒĞ~Ÿ©5 Ã,ı‰/£w$šù`ù9DoÃ×CÎ¬2üõí\PˆÚf$ÌAO]:qÜ—ÀÁ´ğÁ¤]C«æRœgÖLnïFÒuÀ(ªcÙ3ò7¡wºÑ£…±æ3°V-ğò¸áƒl}M‚xÑHÁWZ´,YÒ”]-áé½PÖqK¯K$«ÿÈç*h‡S²€ù%WLJ0MëáeÊ0”†xóW`¼¯@ßs p» û3Lô˜½ô%¡È½%!˜_hèİúÏ4W;®qü ™äzv©8Yúåm9ñûÀà?á5ô…í¤5tXw¦Á¡[Ôü€rü-ßÑğ›ğÙä&.¨s57kk÷¼È~(~Ôùñ¿…İ²Î,LÃä)¸,1(,w»h4Ãh<q¾~à´«OiØ7¼è¥Ã†I³äÅ„¨|mdKÈ)EešÉÀˆ»şY2_ñû½è·Í¨ 9˜©×oû´¢åæÁ¼)í%¬oÚ^©Tÿ^°	dÚ´ú_i÷rySnøf•Ö¨£³¿r»ªƒû+3@]Ñ]`›0¿„>Z£caG”Š™ÂÍ“•fì{¡ïõˆ”ù¥§¹÷f&^RïÁ¿’ˆÜù6j ¤.DO‹è42s¢)HºM
²Ô6æĞˆï°îZYˆ³Yé‹%ãn¡vª~T÷şPae"¦DåQ2›ÉVÀ•
×âîsVoĞ9OİL!GÙø¬Ÿ¯×ÃÅ@¢ì!·­kL¦ÅNÚ1HÑ.jn¯x3?…À<b+çs²ë8 eæ>î}QŞ $°DMk³Ît™j€šs Y*š¬
–¥b“êès}9¬IÎ<ö“öÜra»ÅÄÈô¾c»LöÄ¹š%ù,î;I0-í1Òˆ•R?Rl8ùw´²™)7DËü–œÒ±ÓÌÇˆßŒÄq¶ŞÊTã]ŸŠÛ›ºNtvmDŞµÇ¡µtr–«ªá[–ÉÀS×Ï¢ì‹6ÉÃ"3ø¹4àOtŞdS''ÂÒÜÃ™0B’#×/?nÜö€À¼É8`\!Îä¬ÓÊºùUÖ „.ëWÔîû„˜9ã:ØºŸJ?pP±xÕ×óW:<zkşª]w3ğêYÅ¤ÏPêà\uŒ¿wœÜ7›¸gíå,ÆË¡Xı¶Ò¢¬°xËËÿ~fÕûŒFP‡¬÷•®Šl¦ÿÚÆyMŠEÅT‹ù3‰ØôÙÌÂtŠF‚sá(£:üÄÙ\—=†rçµJoJeŒ•ÆXƒ©-SíöTsk-ş1ZÙ5Üv#vP•Á·şpå¹”À¦9BTÿei<–RQx_›ß'ZzÌ¡_‰Ök½CÃæ²$œg4Ó:To”á¹^JLo¦2÷:£|3¡Z“è€U†‰@Ø+‘¾Jw°
¹R@#ŸhÙ-¹ş]¼T<´¨.Yrş@‹læH*2Ü‘èİZ÷ö¤©¯+ñ\sÇårwşxª`Æq)$!)²ÇÆC€“g  §²ç{Œ²€„UÒQüşŒèmuš›bÊü7œşç¡ãjaÌ«7O÷<µ§ª)\*à[{"µÉS ôI[ovSõë{©ÌİÚ¹»6EY^¢@°ğ¼=ÕºİMUMVO†ˆŸ¿L“êJ
sÇÚ À³ş‡öîÙ=ìrÎ¹Šg8Á¦é?U´ù¤ÍiÏ§a\0‘´Úûo¯ 4ÚÆ§ĞãU&|æ]™7TyÔü{sLyQPš}ËİŠcÛ;æÿ¶ëa¶£Ã¼©ikâ)¡º¥Dùê1˜ÑñÀÄ—ÂPıpİ‚LlƒÎ»*C“eá1£Á2
~ÿ¯…$C€æCş9V€_‹iyŸxE5–‘^¡`‘ÒpØ=‡H°®æ¨Ædé;÷±§$o¹Î°õ.%:¡&mÂ­ë£2é>¨ÏqÆÑmØ„513çÈ¢Æt¢Iæ'Ê:‘?—M¥¼Ì3^Liì¡`a­àxÕ'âwëxä©5ŠÌû$–í.z•ı€eªK‰á‘‡%'yÛ³vœ ıøú~´¡‡Uª<ÿh“şêFv¡I{İaœ9G€ıˆQçfıKG.û“óö ñˆ¥—v"}/ZÔšJd^µu[™<ŸTÈpU™ÍŸJFµiI
YßÎá¤‹œ®c‡›€ÈaË(’`# Y[’Ïğ¸Ş4eU”È-hGå'“èK¯Ú¬sÍgD—µºèb5Ÿë­µâ-Õõzp)òßtm×nŠ9Ô4å±ì\Ú«ªÂèÓ+0Ú˜bvÍ/“
Ç!`ò•(|1·ñ*h?h‚â83 ]Gn€B6¾…ıĞPo:kÓÎ6ö:"­m™—œàv®ó¥PG	.õ÷ªü—DÈêZo"ëF¯bÏÇ»©âA‹Úİ®”2‡O™FÕÇÒ®×dòW^Zn¿î	\·%Ôó^c:H™Í€ƒ8wq»º&¢[y’0¡›¢‘ÒLâ8l/gfdR8–ŸásILCH’Òƒím(ˆ¤d,Y^Y£”|Ñµ³O µ.4‰s½dqLJ•M8ÖîòIÔ¤aL™2VH/0uã‹oƒFDÙùì_º˜£Õé¬ƒ+Tè3â“] P¢ìxÓzg¶B÷Ì7ıÉ˜³E·ÏÃcûOéVå²%Ù§)â©zå¡nQ/–ªÍá–‚z‘£¹ºj"Ÿûår¸ç¸iIH–èx[PßÔ÷_qÒzùˆŒ³$ú:À¾ÑæøOŞXúçM”Ü@›6m¨ù»D¦¡¹´È6µ3(T|gB‹+0{º™µ4ğo3;µ.<¢[AÌ@x§-”àl\!Yîì¸6ä2	ëÜŞÊyXÎHóI9­füÒµ5éÌßK>BfÁw#CØ°nt[ˆt,_¤9â
{q+I‡Í Èªk¯9ãê„TUÙ+\Õ0;ÉÜTÜpÖx
vbZ›å­[ÅÁ³ÃœªíÛTÛ^¸ßJ(,Æó;vxè¯DsÚÄİÖUş[ì>$;şT¸ë=,z. ÿ$çÒD‰•Éªı.›/9gıI5y<[ú;X„jÈ‹}Éı4ç™EÕïûaÏ±=hÑ™™‘sPË=à$Ã|w"»û¾E|/ÒBøí5ôˆl¶RJLY¯\ùùb¢RØ¥ƒOR2õÜ«å£ÆuàFY©ÄÂv´ÎÎ’J%8Wõ`!ÿZşA2©¹JämåÄA5YÃ¶Øg‰‘s¦¬œb)ÆÉ/¤IéÏ‘
¸¶5à|ƒÊ·®5Ş—òÇ5ÆzbváHåq,Ï5Ê#IÈ<«·û…ì¸¤IÓ	Ù6mÖİ’Yv'¯üüœ}Á2ŠUşù¢_I ‚Ù"Â|ôå:B5k™âÂèK“şô9#&‘€Á÷C€û¼i€¸ =8¹ñÆ1°0 ª“K†öÇªÂÁPÏ–USs\ó’Ô K²æO€ÔŞB}Yl<õk¶Y//eir…5Hx‰®°ş[µ7«ºÏóZ¦-é®Ğ«tRp–âÉ½Éı n1"êfÈH~€lY`¼¤U:„“è!Ç¥¡VFC€Rb˜zù ã®‚¦¥ûËèè^[€ı B:†MRˆtpwO¦i•×}¢²b„ÖÊM)×ãó“àµ°=ÕgòëPrÖ8¬\ ™M E(Ê‰w:E‘Á¯U—¸M©”«ã%µÅk9ª	,Ğ]
›:–¶\Í„Ñæ~ÅtÖ~%,Os¼”†ÛÙº;TQØ£`lÏ®½3½ê§¸³¶sQi``e¶÷Xw8aA¿·"\ùÁåU©ß¨óÁ[icêµs7ùº]°ù2`yTFİ‡e»j²¼œ»nõŞRa=Ä„œ-wwá*€²¬Î/uLß«İ^m^ÃO0İgdn¦ó 1à7›šÑ„ôŒâH‚˜ÚÀ€ú¿˜*!‘ñõ«šMşùöYrb£ÄiÈÕ)nÕCõ›–[¬/	ı€``ì¸:¢H§ö×G.áhØ»ç­·ÇWÏ^¢Èö‚tô:!JëzoÔëQQ½%#v{°¡š¸a}GkCg*ÃqÎfR(…¾‡r–S}ã—ñƒÂ×-lŒ’“¹X±­6	«§,ø”G~`”ñÃ`vÚí>*ùãøèpC¡Ñÿï—˜&†ù6Éög+8})şlh¨r]4ë(Ø’)€"ÎèvüÁÆ¿Zx(p=¸ÿö"2Ê+´ ò³m3‚2¹¯ÁË´Æ³¹Ú‘¤mpû‹€ÓcY‚$	mº©•~Ôs›™Qğ©AºU(	ÄÊ¦½qé¡Š!:÷.<ÏÄEïGHˆ—NGRù¶Ô§mÒh¥ê-+¾ÍÏFQ+„üQ‹ÖØóMFe„ĞWíŒÉ!TÕy“Ÿø+¸J;’ÉÉ'Ì¡Z+ˆ‰³µêAO¡1GŠÑI^H®S5f“óÕêcU…êáG.s®K0q¼‡-çBØì[ºø'†ÁØ/àV”7íš®Ëtáşo+A5brÚhæMB¶}ú$È®ŠºšUBÓ\²ccw5…ª®jÅ…rÇÆ-¸6@~gÃv;_K›2Dæ¶ İEä\ÄrÆìœ¬:„û)FæºáÙf;’$NÂè?>’S%f%)¶û+}¶[¸¢rÃT„áÈı¤®F¢Ö?eÒÙfor~›§Æs î}ì²È³|v(¤ó©®G9Oµıÿ™;W #NÒd|÷ Jö·næ/+­Ü?j3(şöuAü¹éòêwirD÷ªòxb—9Z›˜Q¾¹I’Yº(æÙ÷#—Ò0÷º…GçR‹'uq§&©ø'à¼2¡ôşl~9	C-O1‰7ãLºH|Â¹Ôu·¨ÄÜQ£È?O9÷ÄZ`iíä*önXyÜøxEåFË]&!J<‡7#â‰.N(¿÷û-çÂ0‰Æ§TlglÏ2G0Ë^”‹"%w
ÇUõ–øİ*»Åº…=RÇQVÎ!ä¦&ƒ´¶)_³ë‡dÍ™	ÕIÓ
_àÁüµVµ“ïV'¨ÿš/;¢O^÷>ó½­¬ˆÊ(ËX/BV­ÊïˆVlºP÷õšğ©(ÎŠ´·HÂùú?¥r-Å`Rw_ú¦ª©ØnÏÑ»7nÁ´ÖÖ‡›ƒDxpCî{¬ãö}r4İ^jİÇ%ÙÃÆ11
©|>‡ÈÂµ1¤ñnµÈF½†¬œ6oÓ¬õY÷@2»t=1m±(_eâA¨Xh:Ø=Âá CÃˆÑÙr
å¬´b
ºó,¡jBœ5OxËŠG¶×vğ>a¿Ÿ Ï'Í°Ç>‡5ñ˜!Ñ€hæ-VÈ£ v‹ºóMµEu}X¢„9¼_Ò“OD+'¦óKuôÑTc{_óN>5Œ"ñ)]™[²È3[ÓŸc.©—o«ÜëÌ³‰€J¸W ¤‚¦ ğO2ÃxÏ§c‚ºQÕ÷XÃË#Ïæ#ş—OÉ¬Æt—RÜõ¿OÏäû{TU§À“_òê¦Ü>Êİ­¤_Vx£ÀFù„­ª„ÿ@ã‚a+´.·ş£HÏOW„ÍİèHö¶İ	²#-wäC ØñE/ƒu»a«Nlõ÷OcmQúŞ½¯îò°¹ YØÿ@#a=b¦Úıp-^Ğ…¯Pï}
–zˆ_çÏ	a*ùM¤àáÆ‡&>¦vì÷Õ…ø›¿»Yœ[.S¢z›—®k‰6}G…iBù"3ÄŞáÓšĞ|Vw‚E xøxÄº-nmF9~_[r”¿OÆiÈ#&#Ä‚ñò×ÄË÷7Û;xê_%‡8Á›]d×LY.z·¾‚İ¤–iñÜH”#–!»ÍŸ³ûä3¢ZÇÃ·zc£øQä—¿!Í©	SWJ–]g¦şŞ5UØ?«¾ù€R‡uÄœŸf‘l†ƒf2o¬¼”DDR3í—|m‘÷z csrñÓ‚!aİ{ê-Šì†]¤[Sİú½5g»–õôrô×ÜÀ¦X,¦QÍÿ/+A—ÿ\œ$„¾Œ’ïì§\’òş*æp@ PÒ|2Í@İ½gÙ*G|lÁO	®šz?Ë£‰uéô\Mä}“ÉÌTõ§ö¹[— Ê;E†&óñò¡J‹Ç	‚Nyßæ	z €½n.ŞùÈÄfŞÎ/Ôª)ÿGó”c¨^—[k³–éÉ0oèçûİ/h˜Õó‡Bzâ¶ØYêß2Ş¸5ó«›ö¥åİ¦¢©µœl—ÿ®OZòàÆ×iûNú½¯cÒiœ!²<V<˜äw2rñiˆZxŠ)kd’–äÌÚ¬ñò5*RtÉÅN®xÀq+Ïø3|É@™¸¬}â/PÎ.•Ò#Á\D~(d'şeŞÀ@r	+‘hŠ‡t3=o.†l„’‘4•8p´EìÀQÓ—Qy}†
c[éi¿ÆûFûDì|jÅ1®36LÜƒğ¤8I	,‘¤]ü8-³3>EO(Õµ6±TLØS§Y5ÆÃØïßĞ„¨—`ù’ ñWÏp”uV9fy¾5£Ÿ„©f™S*ïX»_mVAnl7	9Ó;Ê³ GÍÕ¥^ÉÁŒb‰ d‹ı´†] â*æyÓ0£3r ÛŞÀúİ’PÈªÜn”ş.Q†yp†V©ìsØñ1£j]cÇ6HÃÅ$|İcş+şµtĞ¦°N€\>ğÍá 9#
„Q*’£<ª&½hÄèCßªÿ:p[Ûçoå'vê4¶T· à©ñÎ&ÒüELŞÎ¦±âtØK0;ı–Ö”¾?EQ’] Gb#šõFaî©‹™’ó˜Ü¶h“š©vJ½@Ï&mHw×^N(¶#ÿĞ‘‰Z“Û!¸Ïbc}HF`·§“³ÿjü²6¦ûĞ‚†;<uÅ
íY÷ó97°ëß;ñYº£ ”ÉÿpÙòG
‘¹^;	•Ú-NK³‚œ6oÉ×š&Y«hÛº` ƒ»X+b0nä}„ƒƒ'^-xWŸP¸‰\ÕYÄ×C¶Gğë…}„·NÆèÃ®×-¾ùÿ€xo™øÚE˜½cu\õÚÆ‚ÇÓê?Ã"L“…×
êyÎ¡Àä<Ù¥…;Øıê4²Û#û‘Z^–¥ŞeK)E{¾ù¨=8p&ã}İ/^–ópŠ¼ÓHßH]
G-{d’mŠŞì…KÉ¯+´°!w›	ç_èÂô£SP'S kŒ¤g”“öµ¨ß‡~—üÀDñI©,±:w%t²o‰¬’S§%ıRÀ×.›êÜW»K¥è7(È$»ÀÅ¡—mµúü^ğşFl^Æ>xî6 Š,’aÁ½¡·VGv>š{øn.—6ÔOœ¬ã1Yó¦µ}ïKöRˆ¹›(îG2¤yd­É—U³:åš¤$¨Ú/á
mY\ÔW¶pûbÒÎN(İO÷|æyÃ÷£’ı¤ıûÍ¾c©Qjn³ÔäàaˆµrKó¬Gp`/Ò|¸ˆ>³Æ¾$„C“ôháÈ¹ÆÜùİmNÚŸ€Œ8İ^û¦aÕeÓ;? ¯ò³_:f1KËğvÂ¬¦¿ëŞåB÷ÂFK)\`ªv‘nÛ8.à	Áfí3BÙóª¦¸@WÚæ„‡.­/ê•¬xyïÍaj}S2–nzeó0˜X]àrğ©dl$ãİÔaxÑÃC÷©"_Î13i-†ocª-±×“`Æ]H¼±<wŒUå,—÷6}W³‰xV•Ì®ÿ´K§N»¸™Ã¾\À~õ8¢ßAT<§Q}ü©»pT“Ú6ÀªlEç=g‘É¶±k(_ ¢»P;hO^İnK;|šıÈ?Ç¤Ë†JsÔ0/5Ãİv4¢VÀ™ûñåÚÆ/àm¡GĞ}ş÷™=Ø±qÿşˆu"T÷Õõƒ`Pø—r.[…oùÏ-CÁŞğ8^'|5,‰Ş»¸¥×.iNÍl;‹gw=@M¯¯YF=+tt×k…Ê±”{	S-ùö¢}6«”š?¼E±#0"TØäOá@\2`Sp;$}W¢àÆA3ÑNÌ‚8ÃgE,æfi<ªljãVÌCKûfGğgåòZs§ŠÀ,ñ‰şjW,š1á\ıcºÅ4F@hÆØê°Ğ:ò¤ì…›Ğå†¾İ7óÇDg€³ ç8¾0ûåS7á¸òwºÈ¶â£—±<6Ëiÿş†¿loš±õh*Ô	EN4ãpÑÏÅ—Y+¢öUoV!l~d\Š›nC2Q<Ñ'ÁL~—k,œ9aßØ!ã[Ğ™/-ˆ=¯;†´„Z_¸ñ} “Fsàú²“@¯¢Œå¨'ˆø¤Î³{I-Ë´ã”M_Ï»ÎY˜©Ñ[½w?%_îMªôâ˜¹†ÿ	ÀàâÃ­€¾[~ŞŞfr‡³
,T4;µ%±`yYáy¢ÛyŒt¯(³1x|:?³¢Óôïæ¾\›]Æ”R/|çûç'Î¾ë†~TL*Èx~»}_Ğÿ¡òˆ™|Ééä¦Ğ[3Õlrù-ÚÅÒmÕû}Âözî»è³±TKK6eÜ“r>UTÄ‡÷»¥a™ïÇA¿ÊnĞÏş8
F=¦\ÿnrê,ûzÆ"'G7Ìyêå¯‹}ö²Ö®jPüyN×oSLå®B]ÄD$"ÕĞúâ—÷%ñ—şF+«ÚtèJB1mkTÉŠâ»YZXmÓm‡$¼ó[®4Áâğ†¸ëˆi?¬ÁcDôƒÙÎN:ìh1“ùnkFİ×Pfz^4İ„¬öš×$êˆ¨°;•êû‡S	wPÀkHZlXk¸¤}È:?>‡q"ìs«‹éuñêTÓ¦ˆr'Gx*Z<¥LYÒó”¨Å8Şİq&]àÕ’°Ã}.ú7]ÒX™ `·£¼tl(if (__wake_q_add(head, task))
		get_task_struct(task);
}

/**
 * wake_q_add_safe() - safely queue a wakeup for 'later' waking.
 * @head: the wake_q_head to add @task to
 * @task: the task to queue for 'later' wakeup
 *
 * Queue a task for later wakeup, most likely by the wake_up_q() call in the
 * same context, _HOWEVER_ this is not guaranteed, the wakeup can come
 * instantly.
 *
 * This function must be used as-if it were wake_up_process(); IOW the task
 * must be ready to be woken at this location.
 *
 * This function is essentially a task-safe equivalent to wake_q_add(). Callers
 * that already hold reference to @task can call the 'safe' version and trust
 * wake_q to do the right thing depending whether or not the @task is already
 * queued for wakeup.
 */
void wake_q_add_safe(struct wake_q_head *head, struct task_struct *task)
{
	if (!__wake_q_add(head, task))
		put_task_struct(task);
}

void wake_up_q(struct wake_q_head *head)
{
	struct wake_q_node *node = head->first;

	while (node != WAKE_Q_TAIL) {
		struct task_struct *task;

		task = container_of(node, struct task_struct, wake_q);
		/* Task can safely be re-inserted now: */
		node = node->next;
		task->wake_q.next = NULL;

		/*
		 * wake_up_process() executes a full barrier, which pairs with
		 * the queueing in wake_q_add() so as not to miss wakeups.
		 */
		wake_up_process(task);
		put_task_struct(task);
	}
}

/*
 * resched_curr - mark rq's current task 'to be rescheduled now'.
 *
 * On UP this means the setting of the need_resched flag, on SMP it
 * might also involve a cross-CPU call to trigger the scheduler on
 * the target CPU.
 */
void resched_curr(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	int cpu;

	lockdep_assert_rq_held(rq);

	if (test_tsk_need_resched(curr))
		return;

	cpu = cpu_of(rq);

	if (cpu == smp_processor_id()) {
		set_tsk_need_resched(curr);
		set_preempt_need_resched();
		return;
	}

	if (set_nr_and_not_polling(curr))
		smp_send_reschedule(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

void resched_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	unsigned long flags;

	raw_spin_rq_lock_irqsave(rq, flags);
	if (cpu_online(cpu) || cpu == smp_processor_id())
		resched_curr(rq);
	raw_spin_rq_unlock_irqrestore(rq, flags);
}

#ifdef CONFIG_SMP
#ifdef CONFIG_NO_HZ_COMMON
/*
 * In the semi idle case, use the nearest busy CPU for migrating timers
 * from an idle CPU.  This is good for power-savings.
 *
 * We don't do similar optimization for completely idle system, as
 * selecting an idle CPU will add more delays to the timers than intended
 * (as that CPU's timer base may not be uptodate wrt jiffies etc).
 */
int get_nohz_timer_target(void)
{
	int i, cpu = smp_processor_id(), default_cpu = -1;
	struct sched_domain *sd;
	const struct cpumask *hk_mask;

	if (housekeeping_cpu(cpu, HK_TYPE_TIMER)) {
		if (!idle_cpu(cpu))
			return cpu;
		default_cpu = cpu;
	}

	hk_mask = housekeeping_cpumask(HK_TYPE_TIMER);

	rcu_read_lock();
	for_each_domain(cpu, sd) {
		for_each_cpu_and(i, sched_domain_span(sd), hk_mask) {
			if (cpu == i)
				continue;

			if (!idle_cpu(i)) {
				cpu = i;
				goto unlock;
			}
		}
	}

	if (default_cpu == -1)
		default_cpu = housekeeping_any_cpu(HK_TYPE_TIMER);
	cpu = default_cpu;
unlock:
	rcu_read_unlock();
	return cpu;
}

/*
 * When add_timer_on(