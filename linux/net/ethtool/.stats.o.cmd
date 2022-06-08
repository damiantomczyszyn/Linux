nclude/linux/page-flags-layout.h \
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
  include/media/v4l2-subdev.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
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
  include/media/v4l2-device.h \
  include/media/media-device.h \
  include/media/media-devnode.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/tw2804.o: $(deps_drivers/media/i2c/tw2804.o)

$(deps_drivers/media/i2c/tw2804.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ude/asm-generic/ioctl.h \
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
  arch/x86/includeg&ÀÉ{§¢~™–)ZÆ7÷é„”uè]úÖTøò≤®ÍrÖ[Q9âA˚nˇè3≥†vÜÊúQíî^1Ñ °«Œôû˙<ö?îå‰ëÊg2@¡Â¿∂ˆ˜8(;OYH∫I2Cú @Hj«ﬂY@ª®∞πªqÙ“Ó¶t
XpÜÄÆ?ˆfxõ0¢ï±.Éôl‡—†i.Ãg…îÌ ¨ò(5B0ç2ñ¿u®¢Ó ÌP–Ê„YÄÅ66◊E ∑|Ò+_è.9ıc¬>ë÷˙ì¿„±zXuØY2E›`ÈM&œR∂JÃ˜âÜñÅ÷Ï.ÊøÈ:,'ŸuX∞xka˙âˇyç√$‰©Å•ú$3îŸô£åﬁSùN#ÌVê»•
Õ‹ˇD∂∆;dqh¢õ¶£3G-Uˆ@	©T°NÏÀˇ€<ûÈ¥äYoUﬁ%-¯àXŒ”∏X∑¥ù©ûW˜Ä BôLv–ÒÚq„UõD\ª“JCÔ∫π≥o!ëhı≈8iÑYˆOmöp◊$˜î@CqWΩ6ò-C âBÜñoï;—k†—MÅ≥âÓ’¨–Û«®(ò‰ÙY_cò€÷3àtX∫ÆJBﬁΩÕ»˝Gn}„ /Ù…4%48ˆÊrtØ˚Ôrùi‚°OÙ≤j∫∆ÒÕ>‚ÀÕS∂u]*f	à”3[y2úXÉô5˘tƒê9b ~˙∆¢`‚ÀN∂|•ƒìT4iî„∆sˇîìöZ˘/ãVk„˚|P¸”âÍÌBœˆù‘`–¶Ó!z!ì‚?ßË;ËWU®Jπtù Û†¥Ü)¶#~*Åz±√ø“>“k›SXáã˚Ù˛˚∂⁄2<°æºË2="¢ﬂ˚æ${“n Ïài^;£'¥ä!¢t≠iÄ*¶¯5™›∑Ëõsí„ƒ1àÊBø+©v∑Xl6y©s; 7.¡m¯ÂÏY[Úvl‡¥<„3ô∆ƒñ´Å3Bºî¶a"¸˜Ó6ÇÒ#DÇtﬁ€w1ÇŸ¶ÇJõ	^<{∞ñá«c^õZlEr^ÑﬁØ}@èå)ºÍ—‹ˆ√¸&{÷ˆÊÀ¿7P˝å_Ê≥õ¨àΩ€«éüÇH b„èT¯Io—‚y⁄ø"œ®ï≠hy\N≥æUàïÛ {òı–P‹jMU
ˇfrjÚÏD?_z”}!i˜ó%ˇ)îe*jÓZ£¢û2Ÿ&√úﬂãÏä6ï√>•hªŸX§ä3-≈è:9yB?MQ7gµÄAÇ‰~çÁ ≥∫ó∑Câ‹Ùféx@X´tK·π–_¡Ò=|ªNÈæ‚€π_)‘∫Á6Ωfœ˚yRlY¥Æ¡c(d1T†Œ8üÔŸ°pÑ˜º +¥\T•:Ûí<bª
%T_¨∑-DäÔ2ãDªéino˜–j ∞¨nâ4Êï±Æ;ÿHz8 #ü2{- æãÎ≥ÈáD@Ûànç◊ãè
Úû∞˛ôÚ2⁄èπ√Ó]Vπ‚P}–8M˚s|_“c⁄8”-ˇÍd`T“Íï9'<˜´&r«îC∆gìﬂÿ7Î(~ÈúÄCR¢/&yvÆ◊">±ds˙Q.◊»ÂLDæ®†2n»¯n¸±x≠†bkû¿"JOä´¶ˇŸè&≠Ëûêj<Õ≥n…Í»eZ°Ÿ≤¨Å˙RNB¶⁄⁄P7Ø)Á™oÀ‘vÕ*'çøe◊ÈÂjŸÛªeèÅ∆≤Ãë°ÂUZ+‹Íè‹WOñÙc∞ÊÈYﬁËM¯è/·Ü6ÅÍTÍõÍF<ˆEûª_ú6ÓÑöàÀ◊<˛÷Q/±¯æg›RñKêÁJ¶hp®°∑ç>Ô6RZ¥ òêPı£ﬂæ|^âöˇ0Ù
\(Lªò2∆$Q„Ÿ?÷3˙Ñ£lwã5î∆e:jÒQ¶˝î‹UeÚ÷˜$df∞0‚e…Ndrõld◊Ò≈UÈ™EI€‰CÇ9	ºâX⁄:üèŒ2@¯°¬„Q	˘MÓÚ˙`]∏ìâØ	Ü•$øí(Œ:ıÄÂûÎ?HÙèÅ∫Í)2“Dl⁄^ÇÎáz
Òp&˛Y≤≥ß]†KKZÃ4Úfåıê'{ÂÔ?˝É6‹˘X3“∞ﬂaÌî7e9&}7›c¶õ#b∆ë_›ØØé„‰x¬AõT·´`V"ësn™fÚ«'`–r±˘£C?”ë∏äˆÃŒ;È"ÄL£È:nmâ˙•ãï|˚_)É#y‚Ó≈eÌjMu4Fﬁ3è˚ÏT ¯Á˙mﬂ#&cÈÌÒqI
vVÂs—d¯veeêõ°∂D¡gÁÁàå$öda âq≥Ó7îKæŸ·¯\[õ[≤MSøÛíÕK—gecŸèPeª≈ÑHîg f;Al¢H+|ˇ∂Â∂…üDb)Ã~wf_}•+µ ‚&ÿL
°˙»›bS!÷_◊Î7… I˙ﬂx™Gí˜Æêﬂáˇ' ˙©¿‰®cˆeÿ-√ò>{úª©}£:æÄ]1«Œ¢sd	p&zŸ†;∫‹î#ñ∫ñì¿µêã)£‰ÄS◊EJÕ∏-ob7ÙFﬁìác≠4ØÀbºÆóº…∑pà¸ÚÆqÙ]>†à¡˝‘üø2Î
·QD4óÇ:¸p”h0KVò,8≥yœ#&Oo(ÎILå¸5É‡ôñÒ‘5@£üg,È5:∑¶á∑d”ëB)Nw˚Mâa™¿”mª´ø}Ø;vø÷êñ¥¿Õ$ú<~f›Çc´¬ì⁄~¯“B€BŒ2∞˙›ÊA⁄õ\¢Ã ÀlØÿ≥ÄØmª:¸§≈›q”¢mπ`ÒN
Ì>gñﬁ–YÌ©∞ˆMÖ+Q&},\9g√P,¸LüÕÒíﬂﬁ‰·^i
\‹_G’\ô≤6rë4=¡P•bπµ ®˛È[ÌC6E«Õ3øÜﬁ2çf ·+/!¯ëñµıÓ±¨˝qÕª¥¥›lìˆÀõ»”<Ω@Î∫Ü i∫b*∫˜òHƒo∑#¢∆”à1 ßÇ≤âñl-ˆ
ıüiˇ¬«~%êñ‚(H‘Óó˚Aπv°Çó¸^¸Æ›"	ãÌôÔö‡û/`r-8–˘•Ö”DáN€3Pˆ–ôfﬂÀÔ˘îÏª6œ‡Õmƒ0ÆJÍ7mßÃ§1à…‰ë&T)‰· sF≤Àhså ≤Ér*ÏÄ◊¯èªu¡ecÕw`∏≤∏YB{,ìo‹∂{|lÎ7˝ÈÀn,√Ô§¿ºîmµıaErÔpé}Ús”£æ¬&¨ÅÚ“™z¯]’≥7gﬂNÑ`%˜4•ü}zPy.IΩÚ'$g3∏πAJÊœ~xQôAzÜ∞ãˆk/¿$ óÄ€…≈Ø◊o#ÄÏ∏¢1Ht-‘b~\…˙‘c,tk	Q›õÒé]é29ì¬•ëÈ'öDë¯ÔMóÕ‘aåx–;}Éæ~v€º|F‹∑vü› È…∞y¿ï«ÿ?_¸t∏¯#	ø≠Í≤ıˇLº?v≈XB(ñ´√7
∞´íwµó #ò9,?£4 Ë€)Ó—:£76ù'ﬂÏ÷DıbÃé +"◊ô˘ímîº≥3e…ÄëÿÒ70Â∏√y∫xygﬂ
î±ê\‹÷ûq*·⁄8ïÊ†hYeÚ˛÷"êr¶≈morÇ9◊≈≈!”´ÕGÚ.ÌFü	ƒ#Úá<‡U˛àB∂^5LVQÉ∂›<ΩxuàËcêê™<êt%'ÙeπAÓäŸ 
@û©lÉa*∆jEA„	A6IxyPBÚ≈?ÙM[¥íY◊˝Ω5˚∆%c.L ¶¯·÷ë
ÓÒßz?∆Ûù®ï6ûIªDÏ¸nXAè◊ûﬂhz‘≥Ú˚€!îÚér‰˙Â4ˇÖëd.CB´>u˙˙®Zˆ–l¶◊Í∞⁄∞•j]8åõı+∞:Ak—Ô}mk∑ΩìV(ª|π?ﬁˇ{.ÍÌO5t&/ÛS4pUÚn_JÿkV—à6åjåmD ®’SK«LÏá.PgÎ∂ ÿ;ÃﬁNF>ØÌ∏|n7z—e˝+Q˙1wB˚· „⁄ë±†_ΩXüƒıpì.«(ÛBqÒî©æì0Æ˚¨›á·E!Ìnõ∑!◊íµ#– Ø3$)ÀÿT†Øõ’¶pÜºM†Mâ‘X◊Y±°√dWQiì V¨r©x™˘‹´_Ôz}b·ó≠™DÉ˙0’pÁóaãñ√ ∞5	ã/å`”ï1	@¸ZîØS¿]˝pu…ÃäÜÏYÆâõzMíÉQAñÊÆ)ßë›Ÿﬁ˙ÈYª∆‹@›@ÆçµﬁΩq uôî˝¸õâ_0˛X8Õæ4Æøyö*°î§SÏ˛1∂6æL)0ƒ·ˇ—1yéKÇ≈ˆÆ¨1>˜°ƒ™Z>¬õqf•Óäø§5'Ã√çÍ¯›WK<ıF¥Æßx⁄©ì$`~âZC˛Ó´’©–l*ZÈ¢∂∂é¢CÙë;ºoP—ª=ÜÚn0ûàÇµÄTÜk~©≈à¨»∆˜s∏®è;gfﬁ,ä“™‘˝*ıx-ÿç¿≥ÛÎÈˇÖ\§©k˙5Àø≤Á≥Ziß&Q„7…„YfZsÒ¬Sêo<óø3ø£éí’w›€ ‡√Øõ¡œ≠
+À‚Q5»x_¡5ÎÊoã˘ªd\1˜;-A¨5≠ìVtâ} ßï0iA≠°Ëpå‡¨ô¯íÌ	€ÁX¨˘=)dà†˜÷Á˚}2?*πdi≈J≤∞b,ê˝‘L[ÒO€},Vî¢kÄaüº‡a≠3EhÍ®ıG“ƒÕM/À)¶9ÈcE+H–mÉ≠¡tÛ5
,’oe„¶,˝õâ˙n€™Iªµu∆¡~Ñ'⁄Å4Ä>◊>*Â©ZÔ‘Ì·Ò_Ad;∏v˝ãNBfÄMÏãÁÙ≤Ì~}ê@‹eWAìb[ì?yA´à»œïpGìÔó6°§œˆ¬P‘¨Ï∂ò‘e:ˆiJ∑Hîw='`í0ÕÛ‰„›Ë(¸π·fπ3Ën£≥òxjáÔ”Á+7†˝¶∫}F£†d–Q¢‡4„¶â–B”È¥"ObÙIÄT.äí1µ9Ç’Ã˚’ÉR¢Õ“m…ôèFín⁄µO∂QÈõrsÖ{npÃb6Ñ¡¥:PI¡∑Ï`6K‚È¡Eß`<È_√ÿ0ÍÌ!—Ádãcq¿`®¢VPK®}aT·/áÀ~—±oDJ1_6…àî˘Œ8√.⁄„l—kfì»ÅüöŸp ◊¯	G¥s∫{/å¢ÍD∫y»wõ¶[Ô‡4Hv#,üıRÄ›—û\àêP#`∆á]YÍ¯ÁNë öÜ¨Œ8∏Àeb∆ª4æ}≠®N£fkG?Ê$ÓqÚwNi∏P,míÎÜÏÉ©∑^âf¯ïπa*.xFf iXGºo;≈zAçßuïí]π&‘‰F ‚‚Drç4g,V
É•ΩπàΩQ<”ÆL^}c»iî3ˇoJ–≥≠/oÔUõ∞≥}∞aâßhs,÷UzFœÿáoe›∂⁄tÈ^ú¿’O.7è—o hzü∆J¯ëGuøU€∞˝‰|ΩEe∂|,âxÄx∫J‰wòRÀ6‰D˛±X˜s=à†Ô&æq3 [G0≥h@≈∆Õ≥úè/„¨∞õï?˛mÏº,{:tà¬Ú6f≤öïyúí8_kù ä»¨ÏS]óá´ çÔ™.‡‡T€E
“∫ı).:≈‰& Úè‚ ^ˆÑ¨¿ÕM»K®‰7]êé¢æ¿˜8–∂Û É\æf´[±XÕK£÷Ùf=F3Ë∞U´‹µò˜â$cAq≥ZÃ∆`;=^—ß€zÓ-∑û–úÙéönèCF2Yõ%∆S-Çh&√RØßFù3¢iP∑ÎDﬁ7-è3pl 7A{•Q**Û &≤¡êûπ8yŸb∂˝Wê‘ª‚-`Ê-ùk:ú?Æ|¢ﬁwØ$,B>uwúÔÕµüLRŒ.]í¡´t¨bÌA¨zlêﬂo7ål≥0%Õ1Ÿpö¿6Y.∏è˝ª…‘∂‘3$Õ’$ï¡0rz!»ÊÈè%•rPÛ.”:‘wa^uˇãD`L¸ß£8lî;*ô≠ÑÏAªΩ'`
\FñÓ‘ÈÀ∏S0Ÿ`|A€ËôêÓwà¬j˙†˚]∏ëÂgáÔ‹ç∑pAj!–≤W ÁÜfó≠ÖY}}ﬁ™`t¶ˆúQ$LäƒÒﬁ#…üàzNÑÍa=œ
uq|˜√Eπ-S÷Æ´]≈ıœÎôuèÁç$íaâ€Ä[s@¯Ë·U{ÿÆ∫øN,∑è6»;¿˘{…¯ˆÿ⁄¯Pô∆é˘|ÃqÓ,ç®ˆé—ãOU2—‹ø£‚∑ÊYA§{ˆrà≥ßø{’∫ü‹¿°0äø
∫ˇTõà@ÖŒΩ,zgÏÑ&î,∞µ@} 8'<m*[‡±Ã∫˙,Ø4p¬Û«|¬2%Aw¨läˇƒô°&˜ÿá^	ŸÏvˇ,È‡Ω"ÄÓY≤È+À∂ë3«lÖ§…VWx≥ÕÍ¥◊SÀÂ≤«+à"nöa-dŸ´Go∑Ò›4Ï˜SõC§*≤ô{ºm6ÒÈ~à⁄”bqˇjäÂâ™>7	E€r±ïÚDŒö0òóG3NÚbûò¨ZZ™ºàHØê‰÷´6%≠Ò]£BmæÑSè…É>w≈∆L-ùx5zf"LÊëE†
``¸>^]kA9ônÄPKÚâ≥^ö≥…˜TπæÕøæTÙ—ˇïÚR€Eêõ"W’D‹E8Ú.≤ç"¥ºo)_PG9£˙† =•ZÖ˚C]h∆>Si9b∆¸)BDç@Âü@“yÛsÈ{dMOW∂tˇ%Ì¢®…/ƒ˜Äq,ÙÓ]®ñ#—YˆYdK¶ÇØOÛÁú¬FÓ∏1®ß≈[IYô3ªIg_t5sÏòHÂHËXJ˚GQ¿v†›3ı•xVÔD$˛±√∑ë“K–^Ö£#¯¶*[Eøß√Mîﬁ?ÖdÒ⁄Ü—Îø"~Ø÷ó∑˛(ÃZ?>˝òPÚﬁ∏IÆËá$˛
è”ÁÒ ﬂ±2ûTûÔS©úÉ¢RÑ±äkPù˘=6€ñu÷TÚç˝7%WÄjZ=…Tœ÷ñÚ®ÃÔc÷∑<ÚîßÕ"@¯nªy,©
¿ˆ€ö∂‹:$f≈ƒô3¸∏2/√Œπ¨¨≥)ksÁ}ÍÎÌ7∆¥/™–8∏öÅ-éH>
U„p(À3Dd|¿∑‘LöÂ&[qOõúGﬂè÷™f÷ÀèD_‹
Ωè∑õhïåq)Í…w±ülçnˇ*}}<j[)˜±ÄÁ£ƒ·È–•{≤◊Z’ºÍç’•Ö∫‰8ö†·©z@‘“AùßMYæ"I˙≤xW›]˝á@À"0.ˆ—æÄﬁ·k1Ç†vRa3Ì-NÜÙ}›uÜRﬂá≤Z–€wŒ†ÓoNiÀËK—Q≠Nvﬂù^
SPs°JÄ¯ü›åM@“ß5ç4Öû—Ö d s5Ø√;Ì"êñNÑ„7—≠§Ì!Ã=∑,ìhøõÇæˆ-÷w'∫Ê
Ê-Ê¡…ù_ Ê}1E_.|4â{‘˚ó∑Í_-9√ΩÒ@ä/bÇÍHëq¨u¯JXè5ØgêbÌ¯~öd9˜C8ıÇ≈ÉÁ±S˘˝|ßÇsWxﬁ·5uN6ÌΩj-Ì∫Éj-ùØ√¸ÅµHn&–"ÅótõyßFxk©+≠‹ÿ”©P¡’”÷wæYÏ§Àíp”ãºÉ6∞Yëë0Àc(H.!XÙAU;w±ÒˆÁêòux ´#¯kø•‘¢∞.ﬂ¡y∫;[%ˆL≤ZYEMúÌÏßÂLe⁄&∞Ó≥ˇ¯˘ cFπ;c¢DaÕ
µ- Ωå,õÈUT4\|bz¨^<,p¡‘á≥:ı%≤+Xêxà€±t5π¥F‹£ WËÔ˜@‹≤Ue;{Çe´≠›‡c_ÄCÙ•êª˚Ä3wK{9ÃÛà.7Z§s° èó#”Â3◊ﬂæbﬂ§∏–=;“â[ö(U7⁄lˇúk™¿h”ÃZ∏ı3MÎ–ûB^ªˇ∂Tﬂ÷Ä·µB∑z¡‹òjòË=" h⁄<ÒŸAÚ≥eß˜Ú˝¸N˙û÷	‰™+Ö†∏Ú6©˙Ô”ÓÛ„¢[7+ÉË∂óçXo˚ @ø∞‹È~e>EèÎ¬µ\nFíí∂<†*õ'∫kÁ0ôUÙ“>c)’QZG]K™HN'*zıòIˇÙæ:∏Hº^X‰ó5˚àÉJÈ –ﬂ£“8-‡)ZØE(A“ÜyF~æ#à„˚$`xYÓÊ(oIDO·Eó∆–,˘	â0`bó≈:„¶Gué<"›∞ª´„ME:Ía±1’Ì∞Ó8â•√Ò(§ÉB[ ún>ﬂ)'ÓA}Kûø/9ù\ﬁ‡l¿	
¨-± “5d vö»ùér≥vÖìÀ—„ï_foj%¬J¥0Æ\Û:•∞À	bŸâ‰¶„xí≥YÃ}ú1°ﬁüéàÅ&≈=˝Y–˜cÛ]Îc—Lt9§Ÿ˘¬›:.¢˚¥·x›–ø……»0p©BÅAÜù9çs·æáëçÊ¢ﬁj¥ÆüWävπSI©UÄ˙‚KGén.ü-aj™∂‘ÕvÔµoû~jŒ¡M#"òk/xﬂ3b‰%ñÏ$⁄´¿Ú«L≥>†Ï üïiºÖ=8Ã$‘øiÏÛç¢ù”ïYœvY
	åyù’Ri˜…;#¯¥2G‘£ï…±≈¿ƒπBÀÅ%>Ë[©wé8À¡Üv‰b¡&‹»≤ßm~Ì∆E¿sl6äQﬁ#H|W‚ëÈ‘ÌŸá˘Ì"#§*RgÍ‹ æ`”Å∫~ÇﬂGw$åÁ¨åº∏´Ω∑thﬂo∂è˚í’º„-ﬂ≠Õ*¡.Ωr◊—©í˘0o)ß5ÔëÖ≠
¸Ö‚ıØevNt∆{e≥˘ıÕMhx
'⁄ì¸«l<<⁄˛Íµ,QMl|‚'güAoÊ=*ò-O Y‘“-‚eW &w)ß †öŒ…tÊïü» C∑π¡≤ÂC√`Pã˙ pÌ?YÔ‘·$>çCeÚ A˙«zuòèÇÇ≤ë.¯k.¡øHùìÄÆ#UÇXÉ„ìô=Ö5c©êP?É/™≈ún7Œ¶ñl”EY•e†¨œß¸Ô7˚ør>ß·$ìöÍ-øp¬ü0c4f 6äi∫[ü^X¬≈âÜ7 Z≥O0ÌM‹ïÁéÅ™⁄#H‘ï*í¿v¡6¯|’ébºs¿‰Ó¯∞ï¬!y@G-Q˜Ωò+ÆA‘Êµm´,mü¡?˘≤ô5X>$Ï±Ÿ±‘Æ≥2¿}¬√˙Õ¶"Ië\Kn§“ƒ5J0ﬂÅ	≤Ü•˙)ãXN\∫±Wd·≥/æ∑ÉóŒ/Ñ$ÈïLŸ∂MÊ1	Ó“Ïõë^uÓÏ£Ê@PîC≈…åø¨IÙãå*·pu%vLG[!®‡πC-—¯‹E˜;ﬁ+…{g)˛ãd0¢˘WKk‹˝≤≤∑I!<ƒ*e©≠£§÷eâä_Y˚m≈ÿƒ@…ﬁëÙ::∂qDá¥ø˝^}%ºV¶õΩÓÏ˙AH?ΩŒ ÿπøé%ÌM˜^kN_◊äî<q¶pÊHc‘¡ˇ Æ@~òÑ˘Ã j»∂ﬁˇ6/˜ﬁk
‚ÆŸÃ˚’π˘‘~J¨l1—|˛∆ä™)„∆§ê‰®BΩ.^“
°f4C˝œÂ<»¶ò[$^C_3ï+K∫[ÌîVæSçc'†Ù„˘ ;%èÄª‚@•∑”F]ı‡\MﬂK2Ò`©¯Ñâæ`tìô
≥ÌGw©bóÛˆT∞r–Ô<Ûgl“®wƒ]ÄiE≈*˘ﬁc•âÁ,Å≤Îﬁ§øcçÓ"î$ˆ@≠j/?é
 éÒŸÏº`ÔÊpï∞E`ÎøöÎAüµ’3Xh-)e=Gcÿ%.a'⁄zns$Uå|W{x‘9bõE –µÅ◊õ¯K¶≤ÅåvÈÒ¥ÖÑèÓ4e•â;Úä?ÁfÈ:*´˛o??RJ˛ÆÕëau˚}%Æ’ôÈÌ¢DŸjòcèp…ˇ}êﬁŸßë%¯ëœ“Mˇãvf≤@;jÈâoààBäM}°Œßò7AuN„ï∑ä∫3∞Rp0åwÀ£¨2ƒ…¬	µõ˝Õ‡èµÎ∏zOÙ±∆Y»‚A‹xÄo°”ky⁄j∞ÙKz´}$Ö'˜„†≥j—2µ¸¢m†3ÈÛEy˝]RG∑RT?6Uƒ√A∆§yHIPœÊ:UÅ"È*rú°=ÒqÖÆtÖ†EIäÂÄ¶`#|º˜Œ˘¸’^ò
±3“dîâŸÀ`Á≤∑kØg9=nn'
2÷XπòOA.KXÉö*¨⁄tö…„æJÈ_^{¨ˆÉHV∑Ã‘µ|©FEí ä∑∏èÉ®⁄Bg¥¥¥ÆÑìlGLâ>:Äÿ¬Ω∞PŸï8Óù›Xbô@õRíÚ3‘€6}¿›ù¸Yœ°|h*ˆ‚… Æâ Øº˚ÑãÉa1~˝∆}cV†Ûp—lAaõB⁄±7ˇbM≠òçœπ,ˆ~·G¡H4j5“KUº‘«b¡MÍeÈ;èæçüt îdP?	ÖTﬁ%)≈-{ÉPrãˇ≠±,„jÚ¸µ$ısâ5lugì‰†ñ@sÆ®·ìT(⁄ºÀıŸïm=VΩﬁçH1Ç#¢ÍÒkR•K±}∏G=ÕïgkÚîÙ› ≤˛ß’"F∫_Vb{˘	u˙+˙Lûœæ¥∞â0ëúzç«J(ﬁë‰”˘h˝üè>ÚÎÑô≠¢Ì√òJ{^ÄÄ‘RŸˇ˜ﬁ5à;3≠ÒËJÿjº:»b©?øsÓBRw}ıﬁ4/VEr]Ù·ôÿ”]Q∞Ó&L¯@zîw…ÿø◊ Ôí®vﬂt∏Q;O}S‡ViRo2%$a˝3·†∫¡P9 –ﬁ_Ï•û7ª◊ñªdyµ˙ı¢∑ö&êCÍhÓ3Üÿ≤⁄¡Lhœd+°ΩàˇËÈwQ‰Ò2tUZ´ˆÀ¥8Ωy∫ZU^∞Í«ﬂ`úøËï≤óóÒ64>_µm…_ë1´©ˇªı5Ìók˜`"Ê#‘±_´°"7¨õÔSÄ2-TØmÈÀ∫ãÇ»ë¿µπü/[2ëuYd˛R{$Ø‚b	 öèghπ≤õWÑºB∑ÃÂÀíê√Mk'sUº>†#û5
™g™9Â·zBb(3¶Q±ñôqàÉ˘FŸ∆/ÈBI‰Ä◊Z†MK±F<[øP‡VWD>I∏#”_È[ö˘ó¥Ih[á^hö'Kﬂ“ﬂüπﬂ®˛∂4˝≥ò#Ù¸}¢÷ù·gèo<Áœæ‚]0Úô[	ìqhYËª÷“•=@	)Á\j.gMñnÈéﬁ¸∞‹ÇÇÛè`è¥˜YuHà_&⁄™E‰ys\êø'Nù„i|&› buÒ¿⁄´Z“´∆˜_G≥fπ9ÄGs««_É~ÿW{l€6ô§d≠gf3Vé•˘ì4díÀ$ç	w•≤g?+˜ªÃ ∞eIàøß®ÑNo÷<Ê(~~bä	R|ó˚;BÍWöv-ÿCÌO<â∑µ^>¥Í0˙¸Ÿ™†UCZ¶û≈÷UÏ≈F8î©`¿
®IB6dµ¡¯ñTE)∞x¬L∫“ ,på*„p˝ewˆH–)lá˝ÇûÜÆˆ¯‡∏ûƒÿ∑kÌ¸é˚_Ö¸)≈Æª[!%qqp ãã†ñï‹«ˇdÓîíÊÉ-HÁõOøò„Ò;¯ûVÁ« >^¶ÈÁ…pÈ4œY%+9Qˇìk¸X “ÏNóí§Œ8bU±Mô‚‹R1ö5C´®åïDGˆ“43ÅÇC¯ ø‡Ò¢uFGöñΩ¯&aèπ£ƒ'v÷Uπ¶Ñ.}†Pn2=á?ùEN-∞(3¨x°3>ªˆîUpgi7„f ˝ËπΩ{˝É≈'‹“”\ñSÖçÊhnauûöààÍã{Ÿf$(+SÎ"◊> s´&æà⁄:¢¨ÆÊà$p„MÛ+'9Bœ8Ÿƒ.¸¸Sd¸Â%"}ﬂŸ¯ÜvıÑ⁄ªÇä∫mïÛ˝'›¿d}¯2≤c ∂ºô≈úAà®ªXÙ∫p¿M4ÆßÎ€Á2&—Yôy¥‘•◊û]%K‹ài¸ƒÑÇ_È~ñáä˛òC|BÈÏÖ9J\ò≈LÎüdÛ¶+wrç¯ÇÍ¡>O’–	Ñ6¨‰0r	UVúÂA‹C¨h™V≠¥ÊG§∑©ë5òGîGI69|\.±†œ˜]ø$∏a[6Z(vp∑]‚évπg
{≈,}ŸÆPi|›˚Y˝E07S‘˝	Ñ#‹õT.„zì<G˛`ëàoƒ¢úäLÍiÅXë¯˝éVÍn*‘òÊ„d˙2‚•˜%ä‰Â“âKÊò9ÌΩ$‹)Oq,cåw∆zW<UÇGSÃ˛g™ çÒÏ-∆≤ß≠õV◊ÆUπmÊjÜót:óè¿O‡Tæ:‚i0ê®µv=>è8˙lRtØÒö|˚yaoá—]K—È?·≥/kJp¶-bÓ"ó!¨OΩø I™Ôìº'—∞ïm˚€ñªíâ-K¶•ñêﬁ'Hñ@|*óµ¬Y"uÊÑ.ã°z[R‘áé¡ı∏ÇîWtq!6∫√Uvóÿ‡	nÜ»îFrÂ@Ï»è∆¢B¶ãàôﬁ±∑ù˙Vìß∑=—&mµÜºhî@¸Òï_œÀ–Ìâr'ıT_˘^`î˙KŸ5.qõ«âV˚ı#0Õπ˘g©∆6†”9∑6ŒÀdm&…BßÔ≈ú~L ŸÖØöÀ:˙Ñ¨+[Èá¿	ËârÎœΩò˛h	‘™EkL∏ˇµîõªªYø\H°ÜÄ„a<XOë∑õ≥òúä£=¿i
Ïku{ªÉ«€ç}W’õ·⁄t˚h_|S8˝X"∑Ã¯‚4WÕ¬Kˆ~[V(èÙ~M^x‚µ—CYŒûpı)8eÒ#jÖûÁªº#3E∞¬ïbaIÇÀÃÿê¯V=yrpNÔ∏ëæ8◊ç‚0Ä†Åe;?Ì K«=◊—ûtÃÇ πbÏ_C+¥Ò‹˛¥\Î¯óµèaf†sG∫ºªû&ì”ª'Fö={}ø.hÜ§ˇÒà˝n¡ÏY†’ß¢ éåá ™ûh‘æë tçà–ä∏svög√F·±@HCzE»·C{yjB9È¬6«ÆEmj∂≠ñm…[≠Bzf©yæJçÌÍ∞–Wé-˛aè>≥†5'm`8˘5nÅ¥pocSÇo˜˝30	“˝>Ω#Ì'ﬁ‡_(%œÚ0«¸G:ZÍ|≠≈⁄∆˙ûöÂwØ«mÔ≥÷gÜ..πç§>¸‹Eﬁ≥=∏‰=Â!Õôú#•N≠ŒˆaI∞˚$·¿£läˇyo±£hÖ≤œ	Sß‹ÎπBºÍ¨ìÇòΩ·!ïÊË©>≥∂⁄òÿßôIÀ%1!0íﬂ¯a|1¥ÃÑŸyËs?eù¢ô!Nä!ug≠*Ø'DäÀñ†%SÁˇ"œ¸ ≈ÈÙˇ@
&«{Úh1Y†ÒcÀER4CıyHw›Ú ‘ÎÓ
ÉZ,>∏&Ì†–µvßçÄ®R®√í<ÖıcRlÖ±tÆ\üÔM$ıÆaó‘ßI”0ª¸∆â›rCÑú;™Q–:–êhúìóà‘≥Ê›mÃËÔcFD÷4Ãéî]À Êœ°gzj±ÚW /ç®rŒr^e˛˛SJpsÇê«L_~ãÿ,D69ç''µ‹À7ºô∏UΩÈ+íô8&~	ª~ﬂ!¢…ƒrgL >p\∞5£&5çÕ¸ıÚ§Y{ËnQ⁄¸¥è√àÅ√Å7A`Nœ—ï	˚º∑+(#^„≈Ä(#≥±◊w}¶4ıü€Vë‚ë@ç)i˘Ω'Ç∆ë_Ç¯IçX»ØdQÏ∑Í‘%“[&}Á8Õ—≤Aié‘ˆ"ÿ-yg∞VÜ4*Is>ä	≤„≥I“\\àÚ…Ëª±®⁄^yLˆO Ndb$FŒÏÂAcmæ6ßu⁄éw¯Ä•Y·Ô.”Xg≤≈§4kÜf~3Z;;ˇXüÙ®mÊ£RP›=àEÏ Kä˜^([KÆˆ±¿≠`íXW‡1§!é}h7ªpöY3 ‘°Â°ƒ¨7AODZ∑ÁÆÒ':vqÊ€°§”b–⁄òîéﬁˆk„ø`Ê„úlq£∫∫dåATäLV4®ˇ˚.%ÕÿkoXE3<íöOÕ”ÇCˆªπu∏ M|ò¿õËF"l£NÔ≥T◊6ªÛ™X	vÂê»áZA^À?äñ∫)ï%@µXá∑Æı’ÓâŸŒ1}ÔíNLdb¬Ω—›Ôè7*m"´U0n[Ì®ﬂøë ≠yìi2Û «¡KY Içv‹GqQ]‰nÚ1‰5è"‚3≥ƒÙ>ºVHÓ –]Í^L⁄Œ—;÷KVùœy.4ˇÅ¬Í¢S$ù'h¥Õád≥EÖ®€†V"è≠Hôãç>Ys“	mè"™»⁄#Ú¨^ÖÂ¿4F}ÅËDÒ3◊opgãs8t≥l≈∞> Æ¥ÛUË]ˆ¡€u:√äçB-¿ÓwÃ	ÈÕó‰ûaàı'I∆<Ó¢–.≤∫•JìÿXFì@gﬁhºäpﬁIj‹<µ˙&úˆ„R•Â<ﬂz˚b©îsÌ≤8 ±<œ°ÛüÂÔ„(DZò∫ÃIâ8π®€oÆı—’‹°œÑ®™áÚß≈ﬂã‡dæπx&ô÷]¨∫É#WÑn¥Xú¬é∞p>∂*ˇ∆L®õ[ñSTI}$≈ø$£’Í”˝ˇ\qgH5N+ 8âCeΩ](*∆OQi©œ±¿∂{∆{1øc¿Öˆ«~_I € Ù@:sj\˚‹h`ˇ¯Ã	ÓÌ‘g9rπsúéª:¬ñÑ∞“∂î	ˇíúôˆ4·©*√¸˝˛"M]åf˚ÓÏÄÜäs¬‘|0b`ˇ√+EsÌ´Ñ°˜Å¬©¬Zaå™ƒ›AoíŸq«/øR‹ˇÜWˆ“À–~ß-•~è∑X˘›ä¡èò
SS¥i=ÂŒ¨˜¥@≥&w!†¸leÇ@X.üws„$&≤Â‡Ì=É…}P`÷ ◊Sâ#Í~™C˘∫˛ÿ…õ‘ÿ®µåˆ≥§ÉW˙W1Rìoz! B[lzòﬁW:6‡ô0ÍZ_2ü:˚Íï}…Vœ˝yÙ…m≤-T7‹´}øî”˝…Á,ÄU©Mù∫œ èŒJÑÒ±‡â6Çrc_1Ã›A ÃÊ¬°èô#+â¨’ÜÒ¶€õ'‰O4≥"Î†C$Ÿ€\U:ì=®îVùˆ¡-·Í¡`ÜÂÍëŒ7·+ﬂ›]¿%7s SaP™ïJFMíÜêî%ªXh•ÊøúLöLQÌ(uwÚy<À‚ó0
‹ñ$Zk¸îÒ îtüa·ê#∞qOø™Q†£–&∆w˘ÿÏ@[¢§2Ê^a≤\jõqúgŒµÏ[mÄ%¯jA‰tê‘@Oµ¸∏õ‰o∑∏bO+aÎ]àı∆®°#zÿû˘–Âw˘≈µÙ⁄’&%eD	>,ö[d^7µ¨0èSº‹P¸Àä„j5‹oJqåâwOÙçv	 ç	˙ÄêÊíÈ≥LÂ¯ñj"ﬂﬁ†R_ñç )i&É©}∑ÆÊπﬂ~ë„_!“¥^*BÉÑY•äA=hbr	D7dZAú•a\'≤2ÔÛ8WÄÃã.ß`lòSŸ}¿é_•úŸ/Rµ‘7≤Vˆ-ì‘"â1íµ$Ñ#U3c€√Ø„m±¢„+˛\ZsBÒK+µ]SWY°Ö3Ûj—‡GØt¢ uÃ„∞™!÷ÔâΩl¢¿4àñ}‹_•±®S‘ZÕ0Ì‰V$f]ì_H4‘>˜†Js‡h|ÊÌ√;a∫„ m°≠ü“#¶˝°!5lÌÅŸ»;3…Î±‡îIH«/ÓfrÑËâE|˛$hññ˜– ÍS†c#•e˜"éØAﬁî,›_ä&ê;1jÆÁ™ú4}sQé∞ï„∑â⁄YgﬂöR‘HÉó≥∞yíxÜ|?Ê≈1Y†í±3›K√rh\¿[]úU»æS7»®eﬂp'?|rZ˜™ÛDT˛È)lÜà¸+´jíq% b7õ∑°B∆gˇ≤îg§	¿æ®Ï•qê°ÔqÅ≥Ÿa*º4DÉz¸nçPwº£O~}¯Ds®ÖKCÈj!åûXøHFÜ?"ÁÔ*∑÷DV¯¯&ﬁúÿ7◊XlàÇ‰˝öÀ4fuFıO⁄Êª∑lpYÿlì+|e‚	 p!é4F84Êr"‰≈M»Aç÷Ï¡éÁç7˙¶cΩEør∫xΩøÜEC ı	("áT√ÒGéINﬂæhYUçΩ≥‘5>g˙^o=T∆FUqÓ[>[…f–cl°üuªŒg»vö≠Ór˙<>ÔÃÎ^y[>qÎ°Ÿá›;	x∏ﬂF«·ìƒª_ÍO Ò]Â-ÅÉÿç“ã£ ô“lÏ®w‚›óÃ€å·y¯l‡ã©w‡â‚êXdV∑zàeÏ[,‰l§ëúâÊ·o4W†A’”ú’† ·áÑ!F"4+âÒ¿BŒå˜·›9¡πÿåÑ÷Cí∆±% 6ﬁÏˆú¡%~¡BÚ "§ÕtØ I-lÔk±]ÇlﬂèÇÂ‹>4a?dÃ+¶”grëÑ,NûÕ™VåÕ¿|„ëáˇı•ô9q’vûÙlËõdH@€≠E{Ì´µG‹¶ßXÇHÒqôÀÿÅÙπØÇ™QrÆt(L4+é+£U◊{m:b»∞I}≥äœæ{g4Û>Ü(UÔ§CU@≤1e"Tπ?ÃÓ•KMs◊‹ú«±HzÓ÷õHAÀÏÜÆºF ≈éi’µŒãØ7◊îóË∂œ™Í(kg»]ÄÊ!Ømƒ/‘∂\¸úó⁄pÁq˛ËÅAZ`eÖÁÔà~ÍÒ,Ün›¨e¸9”ÚG"”rUƒúîÙ8f ¡Ó&!Ëf›’úL∆†2ª§i›ÿ!I≈vÈπ€`ãkúWäK3N;÷Ä¨PΩ∂M	∆tìﬂ1Yÿ˚≠ ç±‡ük˜ØW,˙A_UΩ2ÿ∏√⁄—L(∆;ü:¸x∏8ûHw>€V'A)"êÔ.óœ•n%É”JêÄƒ8€6êvaΩ©–n˛Oo‹ıúËH_nâbtDhHBès∞Ü/+ëÚ”tW≠˚Ü^¿:˚ÿPΩˆåÛw`mÉ Ø∫`¿¿à@7Ô≠’€≠,†;``ÏÂ‚√eg[o“•´µ±≥%ˇ˝Û∞q" F{íÒwô∏sä∫pR=G}@U∆NØ⁄î‡Í)äΩ<{Ò÷ãt*ÌÓz≥ZX$Ñ~˛Q˘ûãP&.%‰VgﬂO8)˜‡ J¢ıÓ4ñwF?Ô*‹´FhKl/¬ñÖoZ‘Täü£3g˛ÉºﬁN2ª¿aºΩÑû≤ú∞H4V’¸Â∆∆ ]¶îáàwa{^»üÜÊ;ã6 cÏ‡oı€M0ﬂ®¯ë§™?367“ü–Å(òu¢ƒvì„úÁ‘‹ÂºˇÀÿuXKÊQ∞}ŸpBˇ˛…dæ#óÛﬁÒ%∑r.˙«èvçÕ˜£ÍÚW⁄Î;òˆæôC]X»Ã›v‹@Må¥£ßÜ§}fZÎ0Ω’ˇ]27Âˆ…ŸÇQfÔﬁ7_K{z–s-WΩﬂÈm€¿ ª@†{˚ˇüÆ9e‘ f;Ωúh£ä¸u4éÃ6Oüÿ<ÜK©4S˛∫S˝∏y ¨ZœK«±u0|ïÉA7∏Í/ãtÇµT	 tóD4å„‘øø∞RÌ≤ù¶Ã9$]˚√¥∆´!Iã„ˆ;;<ãz°]òD÷∆Ì∂Y”¨¬jfÆG¥Eª<Lªr6u˝Ã≈¶Ô˜<ùõõì‹a≠N5Ææ∑w›WC«‡¯¿â§È›á‹Ë‰i.0€Ë°9L+Õ>∏eki¿„)B⁄µ‰'‰\lEà2_ò6oMπ*EàlƒiçZ‘∆∫˚\ùû¶Û"qçK;d€ò>säê∂ú†8.0?‡IÁ ÌµÏÉÕò ˙ê„“Æª;bÑ¨$‚√>îß±Äè°2cY„⁄<$•Ë˝¸W√≥˙æıÁß≠£®?óçB´Ô‘†ú^FZ˛A∑˝ÈÍã©/¸«r‰L3ûÇU·nK¢"ä⁄ÇQÓhÛ£rëêb
g~“X¢úG1î⁄vrFa”mM·ÁÖ·ısññµrﬁrIÙ  Ô‹Ä¶Ù—Dï2 ¶4Q∞lŒAﬂ≤
3
CBùØ™ÃœëN–õì}J_)ûÙôÎÑK{+Tÿej⁄8C
è1léÁ|yâ-¨ägõ›ﬁ	Åò–∆«≥–j¬]Î8|R√†π\Â¸~€a¶w¬|ø+≠^bÚÚ‚-¥á™d%‘‡ZEb›f8;Pé^ˇp9F÷·ﬂÇ«§2—¯'£¯n„4∏®($Ñepæys¡†uqÓáæ7)∫5æI¬ﬁ9ÓOiR r˛[â%DJ7‹‰?Ñ#sê¥ª˝—#∏'FFﬁÃ<[É‚¨¨Æ£˚|ëtí<±1Ï™¯˘Íû8rß4=Õprã¥ìOÍáR%ÀºGV6-µ:t<ylπÙ(ìÕÚ¸[b°IeSÅ¬t≈K‹∆ãgÇ¸ª¶óiõ9ÃHø*ƒX-/8∫∆Á◊uRG?†◊´yjlªìˇIÈ<√‚cÒfÀ¯¸-6áŸÌ13À-ñ¨˘»¿Uã˜çÄçW4ÆVÈ∫À◊“¨Yë∑Ì7Ä—ôg-\	Ω|ûÍÄnË˚∆æ≠>ãA∆
˝‚…8°|Ûá£ê…l≥‰vc≈&◊µŸÓç=’R®øÚª«±G]∞ü$wÙÏ<]¬Ä–∏5ñœ>#î+òµEnwã¶3¸÷ô;Õ§1üÆc†qÅ1'qèã≥*Á!√!…F`ú“Eb‰v≈p∂lWÈ‰ÿmÙ©¬⁄ñG9ﬂo(ã†1Bã\–rájÆ	Ì€Áπ,LÛ<3ø√70˛ü°v ÍΩoCq∑€x"~s&d· ëUÌÆ]±,k˘W™¿"‰LQ∞Ô`ï[ûqˇΩ5°Lâ“S?}ﬂÛ@Ù¨Cc?yﬂ¢
nÄ!dÇ01†kBâ0D#Î•¶&ò•bxå◊±Æ ó‚ÂÔ∞$Âôa¡Øésj®ÉŸ[h››ºïj≈U¡}ßµ∏wÄÜæ±.ÏËü<©ΩbANOCˆ(Gi|^À»|—Z ™Ñ[5©á™±Q0–SexÌZoÏπ&4˚ÛŸÖ„!Æ$⁄nÓ«√ŸÛx>wn¥Ií~€ú√◊≥µÈöÓ™ó V—2>Õ“«”Ç<£ÈÆ˚∂#”ØrRUÈ`iC°JT…QL?V{¬HÕã)˘Y.#ß÷2Âbuh˛_ÉÂÁ{s.E)ÆoN∑Ó∏ÛÅÎ^üøò´å\|£ıNÑìÆ≥€Q ”•öá”ìãIa™)cÚªÊr¨Ò9üÊvªÂñ^ƒGZTf€r_-o/∏ƒKÊWtO4ıMeJ6,’Öà&˝‘Óœ¨òÉ~ã5˚&ûÕ´Q!ös±ÜÌ˜≈≈}kËèf ¶1ÂÇø(µ"&JóÊ”yƒ0≤≥”ÉÜEpálûMg^ŸÏã±Ô3„ÈÄ/`‚I∑{”—B˘~ÿˆ
GÔÁ4¨Ú†yÖú:◊X}"°êø˛Ñ5DtUóì©]TÎÿˆ¥˜f*p–59ñùﬁòhîÇjÔ∞jL√Äa{Ï–¢_fùˆ?≤Çƒ·ÎMõ9\JRˆ´”„Ÿ*\m÷ç8wÇ_º2„\≈Ô¯«ü¶´nrÖÃ¿ûX†·6a”b√
ƒ¡xHpçs◊¬Ÿhƒç">˜e<°»/∫π˝a·9
Q#ªÿaò‡-Á
#ßˆRG!7{˜Ω&¶£Dò:5uÚy™Â¡‰¥„;˝“ºqk™ÖÏ ®öbëQ_M
~J” ‹] ñtFïÄºvÙz»°›œπiTV7^õ√["·lŸDÌ?Ü∆`Ò—\|ó} ÇR'ﬂ7Ì≠‹ÇÄjﬁ∫ä^JóÈ±)˛Ô≥¡ÙDF87∏ûzˇèﬁ!h&$äYv‰ÍàXªÍj7Ò†¨0ïè>ôèŒÈ˜K2Îœ¨éÉ≤+3ã2ÖGÄP‡˜¢˜û48 Rä¬&
≥P÷∞Œ«k¸WﬁZ∏ısÃgûﬁõ' ≤µÏñN∂¢
≥pBúªu‡¬;ŒuIW¡ü∞^Q{∫-î øµ£o⁄+˙%î®m{ûπO;¿-9®ƒX⁄ªN‹Bpî9˜0˚`V·náç˛AZ+K¥ ÎŒqÒòÔØÚEX|Rˇ@w≠2ú'æê›}`;ÇäT⁄f∂	äØKS8æ¯=£ôΩcv«√¯6èª“óÛI·BÍhq≠L—≤ú∑ÃÿÔâÂ≥TƒÌÈ,Ò‚á.®æoÔÉ$Aõ´Çˇ›≠ô+	c?c§g≈cq’ŒU¿Ê*(®rW'íﬁÍ‚ÇV="ª3=.√=ÙôCªwO;ÿSîetú°≥]'äπÛì]BÔ@ÿ#dùˆ|+â#£`?˛¯j‹Å}¶BÍ%äÕ)?R—
@ ¸#ã Êÿ_8‡Xáá£!∑Î$Œ4µÖ˜∏2?ÜC-Â4pJ«—:ZO˜∏∏N•ªN+SÏ·ºzº∑b∫Ó(vp·oL|a“∞¬P‰;Éì
H,O©$‰RÕë≤•µ™R •π≠™>gõ˝Í"ßXB1ÇKº”≥W≥∑ª≈›…PÑ_ˇ;≈Ó‹„KòM6Å€eV–|ô“&“’⁄!(ºˇXòçS<ãMWÀ<‘xÈÇ·è<ˇWÁ|3Ùß„_œ◊úOºﬁgw?]VOg√pï∑[ï$∑^T«÷•4‡f”5ÎDU—<Ê≤õäZö
a˝Q˚ï©àÊÇ®9Ïﬂ;[JcË·Å!U=>ﬂ\>@ÊÇ˙DÅå›ÆXCjÁ1˚~)ˆ‰€u¬S–•€û¿7ì; SL0≤ÆËI%0¢3H∆”ÒöYö‡óÓç— 
H»••gæÛ¯∞Z§˘8z∂r√õh®˚û\}¶ç,≠«V_eë˙-ícÁôR°ÃshÀ¨`B™EJÊ1§≈çV/€ÃèY≤[Ä=–X‘kµ~„U‡í?”IW‘]ûI(b¯Äˇòa˜πÛ‡æ'V*õËl≈ +tOﬁ≤⁄>»Ü∑s§ƒ·AhöÙ◊Üf˙î˘"ËŒÛ5∏∑ùW∫©Q’säΩ˘$+ÿ†ƒ7|øÆPΩ˝ ÷π± Q™0Uw‹Ïîø °¥FË≈Â8÷æ~ñ≈Bºé≤Eµ7Ì∂ç©_∑Ø61‡#¨¸·÷‘j·œq	K/&§(*ô@mW›_pL¨∆Yq©⁄≈Î‹˚„Â>åWŒ,õêoUdì7¯≠b¶°Fh©fn[t:â¿TÃ‚åÀ)V¢Áè'k∏¡∑Ñ.~Ç6êZá≥|mHΩÔ¡µ∂ a:¨QÒÜ˚&≈Ë"o¡∞˙˜Ã∏KÌ·Ï˘M#ÁÙlï4Ã‹⁄å◊‚„Ñ•1»>ØÅlUÒJúhMbJ%™+Z;∏ôü˙(ﬁ˛—§‰.à∫ì√ﬂµá◊∞¨)π5yÛ4◊Ò–øÑ≈íl´¶˝¸®U≈ƒ»≥!—±ø±ïgcñA◊¬èhôÉfﬁ”N
`«„:v≥œ]_π‚n2J5´"º©>g˙ÉT KI®·tô
æ∆l%ÕñVœx©	e for the next lock_list
		 * entry, see the comments for the function.
		 */
		trace = entry->trace;

		if (depth == 0 && (entry != root)) {
			printk("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}

		entry = get_lock_parent(entry);
		depth--;
	} while (entry && (depth >= 0));
}

static void
print_irq_lock_scenario(struct lock_list *safe_entry,
			struct lock_list *unsafe_entry,
			struct lock_class *prev_class,
			struct lock_class *next_class)
{
	struct lock_class *safe_class = safe_entry->class;
	struct lock_class *unsafe_class = unsafe_entry->class;
	struct lock_class *middle_class = prev_class;

	if (middle_class == safe_class)
		middle_class = next_class;

	/*
	 * A direct locking problem where unsafe_class lock is taken
	 * directly by safe_class lock, then all we need to show
	 * is the deadlock scenario, as it is obvious that the
	 * unsafe lock is taken under the safe lock.
	 *
	 * But if there is a chain instead, where the safe lock takes
	 * an intermediate lock (middle_class) where this lock is
	 * not the same as the safe lock, then the lock chain is
	 * used to describe the problem. Otherwise we would need
	 * to show a different CPU case for each link in the chain
	 * from the safe_class lock to the unsafe_class lock.
	 */
	if (middle_class != unsafe_class) {
		printk("Chain exists of:\n  ");
		__print_lock_name(safe_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(middle_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(unsafe_class);
		printk(KERN_CONT "\n\n");
	}

	printk(" Possible interrupt unsafe locking scenario:\n\n");
	printk("       CPU0                    CPU1\n");
	printk("       ----                    ----\n");
	printk("  lock(");
	__print_lock_name(unsafe_class);
	printk(KERN_CONT ");\n");
	printk("                               local_irq_disable();\n");
	printk("                               lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("                               lock(");
	__print_lock_name(middle_class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_bad_irq_dependency(struct task_struct *curr,
			 struct lock_list *prev_root,
			 struct lock_list *next_root,
			 struct lock_list *backwards_entry,
			 struct lock_list *forwards_entry,
			 struct held_lock *prev,
			 struct held_lock *next,
			 enum lock_usage_bit bit1,
			 enum lock_usage_bit bit2,
			 const char *irqclass)
{
	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================================\n");
	pr_warn("WARNING: %s-safe -> %s-unsafe lock order detected\n",
		irqclass, irqclass);
	print_kernel_ident();
	pr_warn("-----------------------------------------------------\n");
	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] is trying to acquire:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		curr->softirq_context, softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		curr->softirqs_enabled);
	print_lock(next);

	pr_warn("\nand this task is already holding:\n");
	print_lock(prev);
	pr_warn("which would create a new lock dependency:\n");
	print_lock_name(hlock_class(prev));
	pr_cont(" ->");
	print_lock_name(hlock_class(next));
	pr_cont("\n");

	pr_warn("\nbut this new dependency connects a %s-irq-safe lock:\n",
		irqclass);
	print_lock_name(backwards_entry->class);
	pr_warn("\n... which became %s-irq-safe at:\n", irqclass);

	print_lock_trace(backwards_entry->class->usage_traces[bit1], 1);

	pr_warn("\nto a %s-irq-unsafe lock:\n", irqclass);
	print_lock_name(forwards_entry->class);
	pr_warn("\n... which became %s-irq-unsafe at:\n", irqclass);
	pr_warn("...");

	print_lock_trace(forwards_entry->class->usage_traces[bit2], 1);

	pr_warn("\nother info that might help us debug this:\n\n");
	print_irq_lock_scenario(backwards_entry, forwards_entry,
				hlock_class(prev), hlock_class(next));

	lockdep_print_held_locks(curr);

	pr_warn("\nthe dependencies between %s-irq-safe lock and the holding lock:\n", irqclass);
	print_shortest_lock_dependencies_backwards(backwards_entry, prev_root);

	pr_warn("\nthe dependencies between the lock to be acquired");
	pr_warn(" and %s-irq-unsafe lock:\n", irqclass);
	next_root->trace = save_trace();
	if (!next_root->trace)
		return;
	print_shortest_lock_dependencies(forwards_entry, next_root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static const char *state_names[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE),
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static const char *state_rnames[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE)"-READ",
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline const char *state_name(enum lock_usage_bit bit)
{
	if (bit & LOCK_USAGE_READ_MASK)
		return state_rnames[bit >> LOCK_USAGE_DIR_MASK];
	else
		return state_names[bit >> LOCK_USAGE_DIR_MASK];
}

/*
 * The bit number is encoded like:
 *
 *  bit0: 0 exclusive, 1 read lock
 *  bit1: 0 used in irq, 1 irq enabled
 *  bit2-n: state
 */
static int exclusive_bit(int new_bit)
{
	int state = new_bit & LOCK_USAGE_STATE_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * keep state, bit flip the direction and strip read.
	 *