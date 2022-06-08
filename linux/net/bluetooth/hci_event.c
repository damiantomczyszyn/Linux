ed/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/overflow.h \
  include/linux/sizes.h \
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
  arch/x86/include/ae/config/ACPI_APEI_GHES) \
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
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
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
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
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
  include/media/i2c/tvaudio.h \
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

drivers/media/i2c/tvaudio.o: $(deps_drivers/media/i2c/tvaudio.o)

$(deps_drivers/media/i2c/tvaudio.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
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
    $(wildcard include/config/Fit(void)
{
	int i, j;
	int firsterr = 0;
	static struct lock_torture_ops *torture_ops[] = {
		&lock_busted_ops,
		&spin_lock_ops, &spin_lock_irq_ops,
		&rw_lock_ops, &rw_lock_irq_ops,
		&mutex_lock_ops,
		&ww_mutex_lock_ops,
#ifdef CONFIG_RT_MUTEXES
		&rtmutex_lock_ops,
#endif
		&rwsem_lock_ops,
		&percpu_rwsem_lock_ops,
	};

	if (!torture_init_begin(torture_type, verbose))
		return -EBUSY;

	/* Process args and tell the world that the torturer is on the job. */
	for (i = 0; i < ARRAY_SIZE(torture_ops); i++) {
		cxt.cur_ops = torture_ops[i];
		if (strcmp(torture_type, cxt.cur_ops->name) == 0)
			break;
	}
	if (i == ARRAY_SIZE(torture_ops)) {
		pr_alert("lock-torture: invalid torture type: \"%s\"\n",
			 torture_type);
		pr_alert("lock-torture types:");
		for (i = 0; i < ARRAY_SIZE(torture_ops); i++)
			pr_alert(" %s", torture_ops[i]->name);
		pr_alert("\n");
		firsterr = -EINVAL;
		goto unwind;
	}

	if (nwriters_stress == 0 &&
	    (!cxt.cur_ops->readlock || nreaders_stress == 0)) {
		pr_alert("lock-torture: must run at least one locking thread\n");
		firsterr = -EINVAL;
		goto unwind;
	}

	if (nwriters_stress >= 0)
		cxt.nrealwriters_stress = nwriters_stress;
	else
		cxt.nrealwriters_stress = 2 * num_online_cpus();

	if (cxt.cur_ops->init) {
		cxt.cur_ops->init();
		cxt.init_called = true;
	}

#ifdef CONFIG_DEBUG_MUTEXES
	if (str_has_prefix(torture_type, "mutex"))
		cxt.debug_lock = true;
#endif
#ifdef CONFIG_DEBUG_RT_MUTEXES
	if (str_has_prefix(torture_type, "rtmutex"))
		cxt.debug_lock = true;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
	if ((str_has_prefix(torture_type, "spin")) ||
	    (str_has_prefix(torture_type, "rw_lock")))
		cxt.debug_lock = true;
#endif

	/* Initialize the statistics so that each run gets its own numbers. */
	if (nwriters_stress) {
		lock_is_write_held = false;
		cxt.lwsa = kmalloc_array(cxt.nrealwriters_stress,
					 sizeof(*cxt.lwsa),
					 GFP_KERNEL);
		if (cxt.lwsa == NULL) {
			VERBOSE_TOROUT_STRING("cxt.lwsa: Out of memory");
			firsterr = -ENOMEM;
			goto unwind;
		}

		for (i = 0; i < cxt.nrealwriters_stress; i++) {
			cxt.lwsa[i].n_lock_fail = 0;
			cxt.lwsa[i].n_lock_acquired = 0;
		}
	}

	if (cxt.cur_ops->readlock) {
		if (nreaders_stress >= 0)
			cxt.nrealreaders_stress = nreaders_stress;
		else {
			/*
			 * By default distribute evenly the number of
			 * readers and writers. We still run the same number
			 * of threads as the writer-only locks default.
			 */
			if (nwriters_stress < 0) /* user doesn't care */
				cxt.nrealwriters_stress = num_online_cpus();
			cxt.nrealreaders_stress = cxt.nrealwriters_stress;
		}

		if (nreaders_stress) {
			cxt.lrsa = kmalloc_array(cxt.nrealreaders_stress,
						 sizeof(*cxt.lrsa),
						 GFP_KERNEL);
			if (cxt.lrsa == NULL) {
				VERBOSE_TOROUT_STRING("cxt.lrsa: Out of memory");
				firsterr = -ENOMEM;
				kfree(cxt.lwsa);
				cxt.lwsa = NULL;
				goto unwind;
			}

			for (i = 0; i < cxt.nrealreaders_stress; i++) {
				cxt.lrsa[i].n_lock_fail = 0;
				cxt.lrsa[i].n_lock_acquired = 0;
			}
		}
	}

	lock_torture_print_module_parms(cxt.cur_ops, "Start of test");

	/* Prepare torture context. */
	if (onoff_interval > 0) {
		firsterr = torture_onoff_init(onoff_holdoff * HZ,
					      onoff_interval * HZ, NULL);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (shuffle_interval > 0) {
		firsterr = torture_shuffle_init(shuffle_interval);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (shutdown_secs > 0) {
		firsterr = torture_shutdown_init(shutdown_secs,
						 lock_torture_cleanup);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (stutter > 0) {
		firsterr = torture_stutter_init(stutter, stutter);
		if (torture_init_error(firsterr))
			goto unwind;
	}

	if (nwriters_stress) {
		writer_tasks = kcalloc(cxt.nrealwriters_stress,
				       sizeof(writer_tasks[0]),
				       GFP_KERNEL);
		if (writer_tasks == NULL) {
			TOROUT_ERRSTRING("writer_tasks: Out of memory");
			firsterr = -ENOMEM;
			goto unwind;
		}
	}

	if (cxt.cur_ops->readlock) {
		reader_tasks = kcalloc(cxt.nrealreaders_stress,
				       sizeof(reader_tasks[0]),
				       GFP_KERNEL);
		if (reader_tasks == NULL) {
			TOROUT_ERRSTRING("reader_tasks: Out of memory");
			kfree(writer_tasks);
			writer_tasks = NULL;
			firsterr = -ENOMEM;
			goto unwind;
		}
	}

	/*
	 * Create the kthreads and start torturing (oh, those poor little locks).
	 *
	 * TODO: Note that we interleave writers with readers, giving writers a
	 * slight advantage, by creating its kthread first. This can be modified
	 * for very specific needs, or even let the user choose the policy, if
	 * ever wanted.
	 */
	for (i = 0, j = 0; i < cxt.nrealwriters_stress ||
		    j < cxt.nrealreaders_stress; i++, j++) {
		if (i >= cxt.nrealwriters_stress)
			goto create_reader;

		/* Create writer. */
		firsterr = torture_create_kthread(lock_torture_writer, &cxt.lwsa[i],
						  writer_tasks[i]);
		if (torture_init_error(firsterr))
			goto unwind;

	create_reader:
		if (cxt.cur_ops->readlock == NULL || (j >= cxt.nrealreaders_stress))
			continue;
		/* Create reader. */
		firsterr = torture_create_kthread(lock_torture_reader, &cxt.lrsa[j],
						  reader_tasks[j]);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (stat_interval > 0) {
		firsterr = torture_create_kthread(lock_torture_stats, NULL,
						  stats_task);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	torture_init_end();
	return 0;

unwind:
	torture_init_end();
	lock_torture_cleanup();
	if (shutdown_secs) {
		WARN_ON(!IS_MODULE(CONFIG_LOCK_TORTURE_TEST));
		kernel_power_off();
	}
	return firsterr;
}

module_init(lock_torture_init);
module_exit(lock_torture_cleanup);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           // SPDX-License-Identifier: GPL-2.0+
/*
 * Module-based torture test facility for locking
 *
 * Copyright (C) IBM Corporation, 2014
 *
 * Authors: Paul E. McKenney <paulmck@linux.ibm.com>
 *          Davidlohr Bueso <dave@stgolabs.net>
 *	Based on kernel/rcu/torture.c.
 */

#define pr_fmt(fmt) fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched/rt.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>
#include <linux/rtmutex.h>
#include <linux/atomic.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/torture.h>
#include <linux/reboot.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul E. McKenney <paulmck@linux.ibm.com>");

torture_param(int, nwriters_stress, -1,
	     "Number of write-locking stress-test threads");
torture_param(int, nreaders_stress, -1,
	     "Number of read-locking stress-test threads");
torture_param(int, onoff_holdoff, 0, "Time after boot before CPU hotplugs (s)");
torture_param(int, onoff_interval, 0,
	     "Time between CPU hotplugs (s), 0=disable");
torture_param(int, shuffle_interval, 3,
	     "Number of jiffies between shuffles, 0=disable");
torture_param(int, shutdown_secs, 0, "Shutdown time (j), <= zero to disable.");
torture_param(int, stat_interval, 60,
	     "Number of seconds between stats printk()s");
torture_param(int, stutter, 5, "Number of jiffies to run/halt test, 0=disable");
torture_param(int, verbose, 1,
	     "Enable verbose debugging printk()s");

static char *torture_type = "spin_lock";
module_param(torture_type, charp, 0444);
MODULE_PARM_DESC(torture_type,
		 "Type of lock to torture (spin_lock, spin_lock_irq, mutex_lock, ...)");

static struct task_struct *stats_task;
static struct task_struct **writer_tasks;
static struct task_struct **reader_tasks;

static bool lock_is_write_held;
static atomic_t lock_is_read_held;
static unsigned long last_lock_release;

struct lock_stress_stats {
	long n_lock_fail;
	long n_lock_acquired;
};

/* Forward reference. */
static void lock_torture_cleanup(void);

/*
 * Operations vector for selecting different types of tests.
 */
struct lock_torture_ops {
	void (*init)(void);
	void (*exit)(void);
	int (*writelock)(int tid);
	void (*write_delay)(struct torture_random_state *trsp);
	void (*task_boost)(struct torture_random_state *trsp);
	void (*writeunlock)(int tid);
	int (*readlock)(int tid);
	void (*read_delay)(struct torture_random_state *trsp);
	void (*readunlock)(int tid);

	unsigned long flags; /* for irq spinlocks */
	const char *name;
};

struct lock_torture_cxt {
	int nrealwriters_stress;
	int nrealreaders_stress;
	bool debug_lock;
	bool init_called;
	atomic_t n_lock_torture_errors;
	struct lock_torture_ops *cur_ops;
	struct lock_stress_stats *lwsa; /* writer statistics */
	struct lock_stress_stats *lrsa; /* reader statistics */
};
static struct lock_torture_cxt cxt = { 0, 0, false, false,
				       ATOMIC_INIT(0),
				       NULL, NULL};
/*
 * Definitions for lock torture testing.
 */

static int torture_lock_busted_write_lock(int tid __maybe_unused)
{
	return 0;  /* BUGGY, do not use in real life!!! */
}

static void torture_lock_busted_write_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_lock_busted_write_unlock(int tid __maybe_unused)
{
	  /* BUGGY, do not use in real life!!! */
}

static void torture_boost_dummy(struct torture_random_state *trsp)
{
	/* Only rtmutexes care about priority */
}

static struct lock_torture_ops lock_busted_ops = {
	.writelock	= torture_lock_busted_write_lock,
	.write_delay	= torture_lock_busted_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_lock_busted_write_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "lock_busted"
};

static DEFINE_SPINLOCK(torture_spinlock);

static int torture_spin_lock_write_lock(int tid __maybe_unused)
__acquires(torture_spinlock)
{
	spin_lock(&torture_spinlock);
	return 0;
}

static void torture_spin_lock_write_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/* We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2 * shortdelay_us)))
		udelay(shortdelay_us);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_spin_lock_write_unlock(int tid __maybe_unused)
__releases(torture_spinlock)
{
	spin_unlock(&torture_spinlock);
}

static struct lock_torture_ops spin_lock_ops = {
	.writelock	= torture_spin_lock_write_lock,
	.write_delay	= torture_spin_lock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_spin_lock_write_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "spin_lock"
};

static int torture_spin_lock_write_lock_irq(int tid __maybe_unused)
__acquires(torture_spinlock)
{
	unsigned long flags;

	spin_lock_irqsave(&torture_spinlock, flags);
	cxt.cur_ops->flags = flags;
	return 0;
}

static void torture_lock_spin_write_unlock_irq(int tid __maybe_unused)
__releases(torture_spinlock)
{
	spin_unlock_irqrestore(&torture_spinlock, cxt.cur_ops->flags);
}

static struct lock_torture_ops spin_lock_irq_ops = {
	.writelock	= torture_spin_lock_write_lock_irq,
	.write_delay	= torture_spin_lock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_lock_spin_write_unlock_irq,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "spin_lock_irq"
};

static DEFINE_RWLOCK(torture_rwlock);

static int torture_rwlock_write_lock(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	write_lock(&torture_rwlock);
	return 0;
}

static void torture_rwlock_write_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/* We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	else
		udelay(shortdelay_us);
}

static void torture_rwlock_write_unlock(int tid __maybe_unused)
__releases(torture_rwlock)
{
	write_unlock(&torture_rwlock);
}

static int torture_rwlock_read_lock(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	read_lock(&torture_rwlock);
	return 0;
}

static void torture_rwlock_read_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 10;
	const unsigned long longdelay_ms = 100;

	/* We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealreaders_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	else
		udelay(shortdelay_us);
}

static void torture_rwlock_read_unlock(int tid __maybe_unused)
__releases(torture_rwlock)
{
	read_unlock(&torture_rwlock);
}

static struct lock_torture_ops rw_lock_ops = {
	.writelock	= torture_rwlock_write_lock,
	.write_delay	= torture_rwlock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_rwlock_write_unlock,
	.readlock       = torture_rwlock_read_lock,
	.read_delay     = torture_rwlock_read_delay,
	.readunlock     = torture_rwlock_read_unlock,
	.name		= "rw_lock"
};

static int torture_rwlock_write_lock_irq(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	unsigned long flags;

	write_lock_irqsave(&torture_rwlock, flags);
	cxt.cur_ops->flags = flags;
	return 0;
}

static void torture_rwlock_write_unlock_irq(int tid __maybe_unused)
__releases(torture_rwlock)
{
	write_unlock_irqrestore(&torture_rwlock, cxt.cur_ops->flags);
}

static int torture_rwlock_read_lock_irq(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	unsigned long flags;

	read_lock_irqsave(&torture_rwlock, flags);
	cxt.cur_ops->flags = flags;
	return 0;
}

static void torture_rwlock_read_unlock_irq(int tid __maybe_unused)
__releases(torture_rwlock)
{
	read_unlock_irqrestore(&torture_rwlock, cxt.cur_ops->flags);
}

static struct lock_torture_ops rw_lock_irq_ops = {
	.writelock	= torture_rwlock_write_lock_irq,
	.write_delay	= torture_rwlock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_rwlock_write_unlock_irq,
	.readlock       = torture_rwlock_read_lock_irq,
	.read_delay     = torture_rwlock_read_delay,
	.readunlock     = torture_rwlock_read_unlock_irq,
	.name		= "rw_lock_irq"
};

static DEFINE_MUTEX(torture_mutex);

static int torture_mutex_lock(int tid __maybe_unused)
__acquires(torture_mutex)
{
	mutex_lock(&torture_mutex);
	return 0;
}

static void torture_mutex_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 5);
	else
		mdelay(longdelay_ms / 5);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_mutex_unlock(int tid __maybe_unused)
__releases(torture_mutex)
{
	mutex_unlock(&torture_mutex);
}

static struct lock_torture_ops mutex_lock_ops = {
	.writelock	= torture_mutex_lock,
	.write_delay	= torture_mutex_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_mutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "mutex_lock"
};

#include <linux/ww_mutex.h>
/*
 * The torture ww_mutexes should belong to the same lock class as
 * torture_ww_class to avoid lockdep problem. The ww_mutex_init()
 * function is called for initialization to ensure that.
 */
static DEFINE_WD_CLASS(torture_ww_class);
static struct ww_mutex torture_ww_mutex_0, torture_ww_mutex_1, torture_ww_mutex_2;
static struct ww_acquire_ctx *ww_acquire_ctxs;

static void torture_ww_mutex_init(void)
{
	ww_mutex_init(&torture_ww_mutex_0, &torture_ww_class);
	ww_mutex_init(&torture_ww_mutex_1, &torture_ww_class);
	ww_mutex_init(&torture_ww_mutex_2, &torture_ww_class);

	ww_acquire_ctxs = kmalloc_array(cxt.nrealwriters_stress,
					sizeof(*ww_acquire_ctxs),
					GFP_KERNEL);
	if (!ww_acquire_ctxs)
		VERBOSE_TOROUT_STRING("ww_acquire_ctx: Out of memory");
}

static void torture_ww_mutex_exit(void)
{
	kfree(ww_acquire_ctxs);
}

static int torture_ww_mutex_lock(int tid)
__acquires(torture_ww_mutex_0)
__acquires(torture_ww_mutex_1)
__acquires(torture_ww_mutex_2)
{
	LIST_HEAD(list);
	struct reorder_lock {
		struct list_head link;
		struct ww_mutex *lock;
	} locks[3], *ll, *ln;
	struct ww_acquire_ctx *ctx = &ww_acquire_ctxs[tid];

	locks[0].lock = &torture_ww_mutex_0;
	list_add(&locks[0].link, &list);

	locks[1].lock = &torture_ww_mutex_1;
	list_add(&locks[1].link, &list);

	locks[2].lock = &torture_ww_mutex_2;
	list_add(&locks[2].link, &list);

	ww_acquire_init(ctx, &torture_ww_class);

	list_for_each_entry(ll, &list, link) {
		int err;

		err = ww_mutex_lock(ll->lock, ctx);
		if (!err)
			continue;

		ln = ll;
		list_for_each_entry_continue_reverse(ln, &list, link)
			ww_mutex_unlock(ln->lock);

		if (err != -EDEADLK)
			return err;

		ww_mutex_lock_slow(ll->lock, ctx);
		list_move(&ll->link, &list);
	}

	return 0;
}

static void torture_ww_mutex_unlock(int tid)
__releases(torture_ww_mutex_0)
__releases(torture_ww_mutex_1)
__releases(torture_ww_mutex_2)
{
	struct ww_acquire_ctx *ctx = &ww_acquire_ctxs[tid];

	ww_mutex_unlock(&torture_ww_mutex_0);
	ww_mutex_unlock(&torture_ww_mutex_1);
	ww_mutex_unlock(&torture_ww_mutex_2);
	ww_acquire_fini(ctx);
}

static struct lock_torture_ops ww_mutex_lock_ops = {
	.init		= torture_ww_mutex_init,
	.exit		= torture_ww_mutex_exit,
	.writelock	= torture_ww_mutex_lock,
	.write_delay	= torture_mutex_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_ww_mutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "ww_mutex_lock"
};

#ifdef CONFIG_RT_MUTEXES
static DEFINE_RT_MUTEX(torture_rtmutex);

static int torture_rtmutex_lock(int tid __maybe_unused)
__acquires(torture_rtmutex)
{
	rt_mutex_lock(&torture_rtmutex);
	return 0;
}

static void torture_rtmutex_boost(struct torture_random_state *trsp)
{
	const unsigned int factor = 50000; /* yes, quite arbitrary */

	if (!rt_task(current)) {
		/*
		 * Boost priority once every ~50k operations. When the
		 * task tries to take the lock, the rtmutex it will account
		 * for the new priority, and do any corresponding pi-dance.
		 */
		if (trsp && !(torture_random(trsp) %
			      (cxt.nrealwriters_stress * factor))) {
			sched_set_fifo(current);
		} else /* common case, do nothing */
			return;
	} else {
		/*
		 * The task will remain boosted for another ~500k operations,
		 * then restored back to its original prio, and so forth.
		 *
		 * When @trsp is nil, we want to force-reset the task for
		 * stopping the kthread.
		 */
		if (!trsp || !(torture_random(trsp) %
			       (cxt.nrealwriters_stress * factor * 2))) {
			sched_set_normal(current, 0);
		} else /* common case, do nothing */
			return;
	}
}

static void torture_rtmutex_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/*
	 * We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2 * shortdelay_us)))
		udelay(shortdelay_us);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rtmutex_unlock(int tid __maybe_unused)
__releases(torture_rtmutex)
{
	rt_mutex_unlock(&torture_rtmutex);
}

static struct lock_torture_ops rtmutex_lock_ops = {
	.writelock	= torture_rtmutex_lock,
	.write_delay	= torture_rtmutex_delay,
	.task_boost     = torture_rtmutex_boost,
	.writeunlock	= torture_rtmutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "rtmutex_lock"
};
#endif

static DECLARE_RWSEM(torture_rwsem);
static int torture_rwsem_down_write(int tid __maybe_unused)
__acquires(torture_rwsem)
{
	down_write(&torture_rwsem);
	return 0;
}

static void torture_rwsem_write_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 10);
	else
		mdelay(longdelay_ms / 10);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rwsem_up_write(int tid __maybe_unused)
__releases(torture_rwsem)
{
	up_write(&torture_rwsem);
}

static int torture_rwsem_down_read(int tid __maybe_unused)
__acquires(torture_rwsem)
{
	down_read(&torture_rwsem);
	return 0;
}

static void torture_rwsem_read_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealreaders_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 2);
	else
		mdelay(longdelay_ms / 2);
	if (!(torture_random(trsp) % (cxt.nrealreaders_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rwsem_up_read(int tid __maybe_unused)
__releases(torture_rwsem)
{
	up_read(&torture_rwsem);
}

static struct lock_torture_ops rwsem_lock_ops = {
	.writelock	= torture_rwsem_down_write,
	.write_delay	= torture_rwsem_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_rwsem_up_write,
	.readlock       = torture_rwsem_down_read,
	.read_delay     = torture_rwsem_read_delay,
	.readunlock     = torture_rwsem_up_read,
	.name		= "rwsem_lock"
};

#include <linux/percpu-rwsem.h>
static struct percpu_rw_semaphore pcpu_rwsem;

static void torture_percpu_rwsem_init(void)
{
	BUG_ON(percpu_init_rwsem(&pcpu_rwsem));
}

static void torture_percpu_rwsem_exit(void)
{
	percpu_free_rwsem(&pcpu_rwsem);
}

static int torture_percpu_rwsem_down_write(int tid __maybe_unused)
__acquires(pcpu_rwsem)
{
	percpu_down_write(&pcpu_rwsem);
	return 0;
}

static void torture_percpu_rwsem_up_write(int tid __maybe_unused)
__releases(pcpu_rwsem)
{
	percpu_up_write(&pcpu_rwsem);
}

static int torture_percpu_rwsem_down_read(int tid __maybe_unused)
__acquires(pcpu_rwsem)
{
	percpu_down_read(&pcpu_rwsem);
	return 0;
}

static void torture_percpu_rwsem_up_read(int tid __maybe_unused)
__releases(pcpu_rwsem)
{
	percpu_up_read(&pcpu_rwsem);
}

static struct lock_torture_ops percpu_rwsem_lock_ops = {
	.init		= torture_percpu_rwsem_init,
	.exit		= torture_percpu_rwsem_exit,
	.writelock	= torture_percpu_rwsem_down_write,
	.write_delay	= torture_rwsem_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_percpu_rwsem_up_write,
	.readlock       = torture_percpu_rwsem_down_read,
	.read_delay     = torture_rwsem_read_delay,
	.readunlock     = torture_percpu_rwsem_up_read,
	.name		= "percpu_rwsem_lock"
};

/*
 * Lock torture writer kthread.  Repeatedly acquires and releases
 * the lock, checking for duplicate acquisitions.
 */
static int lock_torture_writer(void *arg)
{
	struct lock_stress_stats *lwsp = arg;
	int tid = lwsp - cxt.lwsa;
	DEFINE_TORTURE_RANDOM(rand);

	VERBOSE_TOROUT_STRING("lock_torture_writer task started");
	set_user_nice(current, MAX_NICE);

	do {
		if ((torture_random(&rand) & 0xfffff) == 0)
			schedule_timeout_uninterruptible(1);

		cxt.cur_ops->task_boost(&rand);
		cxt.cur_ops->writelock(tid);
		if (WARN_ON_ONCE(lock_is_write_held))
			lwsp->n_lock_fail++;
		lock_is_write_held = true;
		if (WARN_ON_ONCE(atomic_read(&lock_is_read_held)))
			lwsp->n_lock_fail++; /* rare, but... */

		lwsp->n_lock_acquired++;
		cxt.cur_ops->write_delay(&rand);
		lock_is_write_held = false;
		WRITE_ONCE(last_lock_release, jiffies);
		cxt.cur_ops->writeunlock(tid);

		stutter_wait("lock_torture_writer");
	} while (!torture_must_stop());

	cxt.cur_ops->task_boost(NULL); /* reset prio */
	torture_kthread_stopping("lock_torture_writer");
	return 0;
}

/*
 * Lock torture reader kthread.  Repeatedly acquires and releases
 * the reader lock.
 */
static int lock_torture_reader(void *arg)
{
	struct lock_stress_stats *lrsp = arg;
	int tid = lrsp - cxt.lrsa;
	DEFINE_TORTURE_RANDOM(rand);

	VERBOSE_TOROUT_STRING("lock_torture_reader task started");
	set_user_nice(current, MAX_NICE);

	do {
		if ((torture_random(&rand) & 0xfffff) == 0)
			schedule_timeout_uninterruptible(1);

		cxt.cur_ops->readlock(tid);
		atomic_inc(&lock_is_read_held);
		if (WARN_ON_ONCE(lock_is_write_held))
			lrsp->n_lock_fail++; /* rare, but... */

		lrsp->n_lock_acquired++;
		cxt.cur_ops->read_delay(&rand);
		atomic_dec(&lock_is_read_held);
		cxt.cur_ops->readunlock(tid);

		stutter_wait("lock_torture_reader");
	} while (!torture_must_stop());
	torture_kthread_stopping("lock_torture_reader");
	return 0;
}

/*
 * Create an lock-torture-statistics message in the specified buffer.
 */
static void __torture_print_stats(char *page,
				  struct lock_stress_stats *statp, bool write)
{
	long cur;
	bool fail = false;
	int i, n_stress;
	long max = 0, min = statp ? data_race(statp[0].n_lock_acquired) : 0;
	long long sum = 0;

	n_stress = write ? cxt.nrealwriters_stress : cxt.nrealreaders_stress;
	for (i = 0; i < n_stress; i++) {
		if (data_race(statp[i].n_lock_fail))
			fail = true;
		cur = data_race(statp[i].n_lock_acquired);
		sum += cur;
		if (max < cur)
			max = cur;
		if (min > cur)
			min = cur;
	}
	page += sprintf(page,
			"%s:  Total: %lld  Max/Min: %ld/%ld %s  Fail: %d %s\n",
			write ? "Writes" : "Reads ",
			sum, max, min,
			!onoff_interval && max / 2 > min ? "???" : "",
			fail, fail ? "!!!" : "");
	if (fail)
		atomic_inc(&cxt.n_lock_torture_errors);
}

/*
 * Print torture statistics.  Caller must ensure that there is only one
 * call to this function at a given time!!!  This is normally accomplished
 * by relying on the module system to only have one copy of the module
 * loaded, and then by giving the lock_torture_stats kthread full control
 * (or the init/cleanup functions when lock_torture_stats thread is not
 * running).
 */
static void lock_torture_stats_print(void)
{
	int size = cxt.nrealwriters_stress * 200 + 8192;
	char *buf;

	if (cxt.cur_ops->readlock)
		size += cxt.nrealreaders_stress * 200 + 8192;

	buf = kmalloc(size, GFP_KERNEL);
	if (!buf) {
		pr_err("lock_torture_stats_print: Out of memory, need: %d",
		       size);
		return;
	}

	__torture_print_stats(buf, cxt.lwsa, true);
	pr_alert("%s", buf);
	kfree(buf);

	if (cxt.cur_ops->readlock) {
		buf = kmalloc(size, GFP_KERNEL);
		if (!buf) {
			pr_err("lock_torture_stats_print: Out of memory, need: %d",
			       size);
			return;
		}

		__torture_print_stats(buf, cxt.lrsa, false);
		pr_alert("%s", buf);
		kfree(buf);
	}
}

/*
 * Periodically prints torture statistics, if periodic statistics printing
 * was specified via the stat_interval module parameter.
 *
 * No need to worry about fullstop here, since this one doesn't reference
 * volatile state or register callbacks.
 */
static int lock_torture_stats(void *arg)
{
	VERBOSE_TOROUT_STRING("lock_torture_stats task started");
	do {
		schedule_timeout_interruptible(stat_interval * HZ);
		lock_torture_stats_print();
		torture_shutdown_absorb("lock_torture_stats");
	} while (!torture_must_stop());
	torture_kthread_stopping("lock_torture_stats");
	return 0;
}

static inline void
lock_torture_print_module_parms(struct lock_torture_ops *cur_ops,
				const char *tag)
{
	pr_alert("%s" TORTURE_FLAG
		 "--- %s%s: nwriters_stress=%d nreaders_stress=%d stat_interval=%d verbose=%d shuffle_interval=%d stutter=%d shutdown_secs=%d onoff_interval=%d onoff_holdoff=%d\n",
		 torture_type, tag, cxt.debug_lock ? " [debug]": "",
		 cxt.nrealwriters_stress, cxt.nrealreaders_stress, stat_interval,
		 verbose, shuffle_interval, stutter, shutdown_secs,
		 onoff_interval, onoff_holdoff);
}

static void lock_torture_cleanup(void)
{
	int i;

	if (torture_cleanup_begin())
		return;

	/*
	 * Indicates early cleanup, meaning that the test has not run,
	 * such as when passing bogus args when loading the module.
	 * However cxt->cur_ops.init() may have been invoked, so beside
	 * perform the underlying torture-specific cleanups, cur_ops.exit()
	 * will be invoked if needed.
	 */
	if (!cxt.lwsa && !cxt.lrsa)
		goto end;

	if (writer_tasks) {
		for (i = 0; i < cxt.nrealwriters_stress; i++)
			torture_stop_kthread(lock_torture_writer,
					     writer_tasks[i]);
		kfree(writer_tasks);
		writer_tasks = NULL;
	}

	if (reader_tasks) {
		for (i = 0; i < cxt.nrealreaders_stress; i++)
			torture_stop_kthread(lock_torture_reader,
					     reader_tasks[i]);
		kfree(reader_tasks);
		reader_tasks = NULL;
	}

	torture_stop_kthread(lock_torture_stats, stats_task);
	lock_torture_stats_print();  /* -After- the stats thread is stopped! */

	if (atomic_read(&cxt.n_lock_torture_errors))
		lock_torture_print_module_parms(cxt.cur_ops,
						"End of test: FAILURE");
	else if (torture_onoff_failures())
		lock_torture_print_module_parms(cxt.cur_ops,
						"End of test: LOCK_HOTPLUG");
	else
		lock_torture_print_module_parms(cxt.cur_ops,
						"End of test: SUCCESS");

	kfree(cxt.lwsa);
	cxt.lwsa = NULL;
	kfree(cxt.lrsa);
	cxt.lrsa = NULL;

end:
	if (cxt.init_called) {
		if (cxt.cur_ops->exit)
			cxt.cur_ops->exit();
		cxt.init_called = false;
	}
	torture_cleanup_end();
}

static int __init lock_torture_in��Xrd��P�C�����%t�QƫΐS6�0%>k������v����u�I�QL�Bp���8�o�(���Hb������$�^������p��++;�H%��G#��&f�����1x}{�}�J���H����s}���n��L��Q0��߇�\�g��+	°���|_ޞPf�0�"�9����f�t�T��� ���:ٝ���+��3m�
e�R���AQ�����>���g�vg�%	6\�bB�8?�5����>`o$����Y����6�	�M7�;l���m�%�d���^qà����ڛކl���%v�2v�Je&����N����αWk�x��ܔ%���$v6�$�i�63���Bu@`'�/)�X��b��~}��q�֮���n5|S�(�}d�'����:�~a����p�|"��/HB7hA��5G�0|+qR�]��6�BN�R����������s���'0�`^���e�j�F��T�Y6(���	I�����I�@P`,�T��ЇÂ!B���*�Ms:,�2No�{��oA���szW9�om�u?�f�� �~aW�1X�m<++�3`?��n�F�ǲ�]�N8����=D��<"��p���,��;�Y'�~ͪ��+��Ҡ� Dg�̅#U�U�d���U���t�MH�j���;����3I,L�.���H�Y}ԟ��d�Ͼ��I4�h)6X�DE4��u�Q�O��������E���]�������D;�2H �荓�5yR�Ų�3��y���#(ɓmhꔂ��;d7k�rt���"�J�����قkOa���l����^ii�v�+�U�8P���0���U�~��r�@I�ɼ�E��,N�!d�.���4�.X��y�k�~=���Ȳ���>ߕ���U�k�K�Ӝܧ�'�6A�F���n�N�E_��8Xβ��Ҟ�U+����C�'z@诉����{���:t��p�j$gT����ۢS��s�)5�j9��4��XW��]5	0�dJ�&kڞ��0-�����sΊiݿ�ݬld�n���0�g�-��D���X��_�fh���=d��l#=�Y�V��a��x��%d�f�_浰�=�B@�V��p>fz3��M��1l/eQ���
\+Uv�1l�R�Q��@�)�.��������5P�jT��UN[�{Uv�	)ُbv��q;��� �'��KT�]^�d{1;3 �H��!A��O������W���"?��'
�!����'C����ں@S}���d����E��ȵ���Xѵ�$O�6��/���*�����'�������$�Tj�
ʐ�C}3|�# �-����i���g1����>��z�1�s\�1��Rc�Dg��;��U0�y���-��UFo�6B��%�)�(�"��(g���)��ǋ�q��w�����Xn?�-���징k��Tl�j����� ���l=�����	҆����pг�Y��z�(�S��[�`V��d��w��n~&�~\!�"�0\����>�O������Ys8�iE��Hd	
��8r(�cN�,sx����B����>�qW�L&�V˚�g4"�l;�o�6��]�Yg��`6�����62�{�2T��V�%��P��R���3��l
���2b��GrGMpu1����~�W�ؑņC��I��%2��(T���$u�����/��(u_`����E/���&�v^v��k�	&X�x�; ��-<l�^�2��`��p��_��������W��Q=Az�C JBBT>��9�Z��/v�ql	YN���E��i�6��}	S1mw�(�A��S��~W�ZΩ��G�Y�vIw�q�,��VB�;�IPȵ����� %�0{�	.��{1;��Ә��5��*K?A�s
.yF���`Y�݆�Qs�ڭ;ݥ��LW�*d���K��j���c�gS�S����It
)�)n\E_;E넒	$U��v>R�Y���JB����$5�z��S$Dj3��E�1�@��M��?�2�}�����̃�E������6'Y�:盉dѕNV/��آg�s��d"V�k�yK�V Vvŷfcj����R�����.:O)����*gp��J�=�]3C�t���>�.����"�z���{Z���}��͡�����Cp����F!�Sj�_ܔ�^d8u��hʏ-ԝNYI]�Gl��k[w����p�wb� i�}Z?�r_�&��)����줙�j'g6l����U��?2���.����H�_K�`���/��G����nt�Da�>��X�kHC�;3�q��R���Ȥ�Fcp���}*V��m��Q��[B�8��s�E ����y�G�rS�@��%���
7��X5�.���-b�To�/� N+�����Z����E�H9���uȹa�
50������~��}��7�+�K��
��O'r\F?S+WS�Wj��4̐t�%��LM�踣`ovۏ;�ab�;^�a{Zzi~Hں	x��y�{0�������Zla��( ��y��il��|��M��Pr��i����U�����]{nx���̩s������2��8&�����m�^�`ͩ:ß+,��¸/�&���A(����<�B�m)����7�tjŰ��P?{T;Ů�3|f�E����S��7�D�Շ_.��;}*�5>|n�8�ε�����h���E<K�+�T�#�4q�A�[̘�HK�{��&�{ڟ�+�O�ejq�&Jis/���a$�w��+a9�W�����p.��B� h�O�f�*���IB�[�o#�Y����~�~/�U��-�4����Q�T:�rY�����%���@�e�%��e�����YУ���]��٣7�;��K8����6KmE��#}Z����tȀ��7��:�e(�^a;�kz��g�؇�/9/�v�F2%C[d�w+��3�A�?�U_ݖ���s��T�a9c���:|d�)S�)O3��M!�F0GA��Y$ؓ9n����~\�Lf-��ժ��ۭ�S�&���+���V\e@��{"`��̚kã7;ƌc��,�`�����W�j�^x�&[shi��'ˣ*���p*���p����U��|�Ox�1�0���X����V_�Ҳ�N�/p���R�	�+
����������4-U��FdR"K/��D;y��>���=[VEy�YM[3`�x�>ːС�?�@��V��Ʃ7�����YÌ����zu ��{=����O��D��U��ke�}��]F�T,qba��!@=g-^39J���e0��V��d1+��.�D2L�e1J�Q��3����pq�����F��&����$���A�����H�����f t��y�)��"^h�d�$FĞ�}t=��MO�;箤�#�Z�6����/����?�������x�t�OJȧ��=Y���C˒^��ĕ�u�B��Ѿu���J�[Jz��]G��2�i�ˀ|�h?)��0.1�o���᫯G�h<���}&�$�����|��
9=��/��!�}5�4o���>�s08L�ٵۃQ�G��t�(�u�#�A��qW�d4D�����GK�br: ���Y�<ܪ�=.b���1׫`w��'�$l1O��&z]Y�$M��UU^�ϋ�?��2�UF2��t�SVe���ӹ��+ �~�rU�h�!��ܨ
�Іc�'�F"zǀ� �7���D�W���Z��6?J��=E���mS�s��q?tM�pS?��}%�)��ó���K�,g��ƞJk=3i;f�E��7�q���s�[^'��8sM�V�8�l��a�G�7��w��hQ
S�Z�+��HL�\��w]'���R���t+b����C�|O�ƿg�ȈR�F��bRI����׉�~��h�vѿule����|�`X���HuN��zm������`@Q�r����]�dC���P�R�|�U��U����;IQ�'�Қ��_~j���y��t{��lLH�#��-�&�H8�٥.��_ED��㎴����0�nd�{���|��K-�Y�>!�CCA%p�m9����?��H;��֏����v�=�q�aN=�P0��|r�� '�cȷiGO@��z��Ӓ�b�3�
��OD�@g8B�R��Z�dk��a�c�,;+ �z'�^&K:��<���F0Z��
 s#l��h��J�m༛Ws��+d��d��$Zn]7��7	���|���5BsKN�zn�;�y����	���*��I�-w2���#�g��t�k|�F���h��<����yέW4��0Ԫ"s>�F��h��Z���U�sψ!g,��&��p�?X3`C�������{W�j�t����h�|c9P�Ǉ%���Yq���W�8�?��˓Jb@%+�<�cEܚ����j?Wz˟�}D�;�Z��Հ��Q_�����6DLd�b�cf���X|��>����%���g%҂l�D^�,"V��Y��ZT�f<ξ���%*��m5V�s�.���#�X�H� ��cI�j�!�������J�"�,����q��$3��;�
63v$Ƃ��u|���v"h���C��}�+e_KvTq�����HZ׽�|�����Sh�{LG������`��J&��q?Z��VC�0r�%4%�v���=k8t>Pd�O�8�Y�Vk���495 �V���a�jLm�^7��:�d�>'#ݭM�	-�P�P���	�ڏ��1녴�LC��i����@�/z���=��P��̖���x)qD�J:�o�m>$�@��6B-}a��R���,���U�7q�xQ��E����V�p�7W��j��C�ŏ��D�YwA�q62W9�Q�:宀�����܌Ey(�Y`AH(TB�v�9�%��Ak�b����yu�OG?�s�!�8懗	C�~my���5ű�@p{-]�����֨q��έ��B��,3��m��cg����T&^�� )o�tQ6�IޫIݛ�5Yim���LI#*�M�	`���d̐4D*�;l�4BD��`e�H$�I��R�3�1�ʸ�?=t�����}�H�o�%|�܇�]���ڄJ냧�z6%~��Ώ�"�I�n��'�۲!Ȱzzc�$�$�!yw���0���)���()��yh��q+��E��JcЎ�C���t�tM}�uc�;b^��&�i�ʩ�LZT�	�����@�JL-3Vz��Մ1�4���4J�N%�]���tw�#�i�
i�>�̈́�-���w�྄K�(�3�p9����O�6�������2�Q�/
&ӗ%\��^�p���m8���j/�R��܊��$,�����V[�0�}R�����k���N��k������x-_��o-ר���o�$Y�Ъ�Q8TȤDH�E�]7�G��G�45k7�P��s2!��0���S��o���ơ�2��1K� d�~��E,��O�B�J� 5�T�R�z�����FJ"�5����
�p��8*À&�,a�c%�vBNqkw����%��U�]�ׇ�pk1��F�F��ux8>~���
��/l׃�Ye�0�����B���#�n�^���z�M�6��>�d_�����ٙ;�삈1;���a�۪�D�\˙6s���K���	�a���4	U4�QA� Di0'M@[1�����B�ٹ���`�?�]%J"� ���.��T�a�#"�d����`����VB��v���]|�:�Cp�}��h�>f�U��_I���=��7�*l�!��V�;�e��[NT��Ot�?����ǔ�.{"���ΐ��:wo�VV�Db����?�Kg:8��1���?�~Hw	���x�R�ZA`��~�5P'yuW�����2�Ո�L-�~�X�) ����Č�x!m�y�!�������ձ��+��k���ER�L�S��{ko8=��s9hc��:M
���@y3:*�_�=��&	�Bg��4ˤh~��+X�"�8��
�!f����F��9/��� �k��@�5Y��( =���U�KgN���z2jkX����_�h�Tj%6uUrH�ƫfl�~��.�D��IG�Oƛ��_CI!?e���(��)�K�xd�c-����Q���O�ݳ� E*�#�+;���/2�y�o�@V���%�g���NCKM1�Xr���N/�O����M1��+	���z��#'��[R���+��#���Aޣ�cNȸ�\�+'�i
�H"��ǐ#{�Ip~&=�ij����I����f�JlmS�X�m�!�'���v=.>��Hjo��s	蔷B½b��uA�sc����D�:	X�4Y�p�Ѓ�CW�ھ푍Q,P=)����/�Wr@%�q���vt��;��h��X�?�';�&����^�k��XD4B�&+L�_�J:���̓9e�[Z��zc���D�C���{oIm.�7�8'��ĉv��3=� �K�x�7�#���������|�� �@�賛nDǨU�Hh�:�Z��q���xr�>�1�-���,o�4T���ε��Y�c�B�˅�v�U�j�fuu�'�䒿����o��|�jd}�FZ�[r`�"�~�N�}e:�콄ʭȐͯzY\��(L���r�$8�C0<E1���r��A�R�7TQY$��b� R_i	'��6 u���H\}[��>�	!@���
Mc�
�Z�4������]N�|����ME&�R�1��D�1����F�p��Y�s��#���|���1��\G��/�� "x=S�W��HSe���D�n��ZK���t��Q
��"C��G$"���H^�a)��Z�>(:��?����4� ���@����<ݜ�`�uwp���������X���[k���cf�
�R�ۘL�z����PI4'�2D����`���S'R	���#�\=��N�1��9��������؆�)��8�"�Fm�Th��V�wp_�_-�b���e_����+�^۟���B.�mZ|�*��Z%�6x4������Eld��iY
|��s2�g8�?8���1I��"\*��>�#:`Dcv%H���͵A9��)L%���|����H��1��ۋ?K�31��Y���z��)��x�e��V-,�����D1�D����&`t<<.gm�¹����H9׾=MRw*����i��Q5}���.�C��C���:�,\�L�
��Kp]����= =��|�p�=�Ґa�R*�l��l�ND��O���fr($i�$���Z8�R�l�3�{��4�>�����;�� �x�(<D�1~ӌ^\�w_��,`i�0���e���_���2*d��Þ~��!W`8]�EޅhF<�A�����5Ad�v ,����o��Sl��$R�׀_��5��$��M��`���3DE��b_m��Y��kH�a��*s�l�g�.��K?��ˁ��;׏�
d�k/V��P�ɡ�D�﯅^Qi�ߞO�r6%'×$�{<��+�}�R(���^��9	F(�A�e�^��m�6����;���:����.�������Hd-��=���ƓN����H0~��?�kx�/����aB�/+��}�ZB��̴R>��SkY#b�U�)�Xk`D�
�֎��ۜ /c�y�B�Z�?ND��E��%��q:�$f�(H�Ej	�چ9é��m�'��%0%;0�:0S��Z��캲�|�]'Hi���_M��۾sKEi�F��i�ͻN�Ga�����dq�o�V�E������y*��S�^9��#2��ￓ]������P�3�b��	#.��%�U�4I|��N�� 9i'Y��$�d���PV�8��馯�65��[���"����(Ϳn�RY-�r_���%n�������)#\D�K2q��
��G��%��e�3�LrCvM�a�$Ǚ>�YU�������J��lZ��q�ٖ�t�K�� �3y�Z����("���i�D=8�*K�� ���~.��n{!&����������R+	��5Y.ԢG ���xG�l�� 2h^�Y
f$�C��إ��3,�lg�m��x#�p��t�{�>Nė!E�,&�=������P�ep�".g?���+W��+E����Q��<nF�{O�3�~�Ψ(Y�\�����Mc���'�za(o��9H:�c�ˬ/D�H���9��8��_�K��]��c�.����B�ҹ������ӶP�Bs��&$CgdS�
���}H/0T�:��z�� aG��3�c|�a�&����G�T�si���5�s�e�xb�f��%�L�M���Hd7�I>�L���%�(�'QCa.� �]�^/G���6Ws�%�h�5E�GKMU�׿<��A�8V,�� �r���I�V�Ŵv[r���dM��mXd�'���"kȥ��Pz�;	���� 5�;!�̈́��6�*��B\����/2���3k�]0��[�s���p3j�5Ip����d��t���	��r
#�)YH��b}�NMS+g*��5G�Bֺ�����7]���2F�&FE�Zۿs�Wt�s��(������p�qBS��)�vX���{"����W�"����E�D��(�3��G����0_�1C��	=�8A�G'���z���)�]�lh�ffH����] �&2�"6��1VN�]�h�B�TI>��:�>��=��nӿgݾ��������C�%�ƌ��c��'T��*I�b�qVݯ�ׯe��K��'�]��2پ��?����8��<���S�4��zmhiG�7�=��<�n���`�X���邃Y�ҵ$C�Eh����y�;ZY?��R��+�0�v�\�λ��B�8	�
-�8��I=�lB[7;t� >��ul�4&I��zP�4��.���+[�L��������4��"R�H�D��5��tt}�]��4�#Òin6�C���{���e������Q玖��I?Gm<#Y�B�H����E�1��v�>������=(,]�D�l0��� �fQ���� dTqw �������4U1w�Ry�iQ��]�n�O�0zGZ�a#0��y�h-A�r��
����H�O�{������T��fC3�\�6K��x���͢s&t�d�m��C���	�F�E6y���z���"��7�����>���0
K+f�L�/``-�� ������z��Ng��}0�G�� �U�L��Z8�l���� ��\a���t�9�N� W�&L�i�=�P�U��?�)�ɦ��Lz_��v�ܙ�v}:�7s�\��[i�G@	�(p�+G���g�B+��m�e�����]^:�W;�%�a;�[Ho� V6��]��3���i�&�����]]ʲ���p�Z���p#%��`�Ģ�Ao�FFy�����\&�&h||�
�2'��b?�\�x$)?���a{U�<��Z��({���P��"���/��[����M2��jE�I�4�q��$����Rh���a�`���1+9o�O��~x�-����>9��F�O{�L�֚�_x*'�I��AKс���硌3M)�)��%��a������Kf"O�s�x�ܐpY��g�*5(�����o�kJ�^t0����9D�R4� �ߎ�;a�g�� ToX;�b��#��/������F�Y�G��}�V�B���d  jwe�}�s���R_�����$��Rf��[ap5߅D�65�{�\�Q����`�l4�΅��G5"����s/�Saĕ6g��5O�'��t}c���_���n� d��s���3�}�a�&0?�'���B�J9O$�P���w�1UXLL�����zthI7�K��v��Dd�X��Q|� ]"_��3�2q�=U��%L©��.N�N0-B�Ą�K�R�2೻�i=*�z@�a�X�u#��X�fߩ*���=��n D�˪|h�7N���/�|!���8Iy�����7�k��7c��ԃ�oJ���3h����+�}�E.[��Wف�N>������)�w��`q��K%���i���r���7� c!�d��Ƚ�l��)��U���5`�z��O-�).�x�x[��yh9x�G5�s���J���~sW��pZR�ЍxN}џ�cyv��%
�y�Z�4�j��T�$����v��$]L(�s�Lt�ь�7.�Ԏ��������)4��W�e�rS���6�<�ϳ��r���6���@����J�Tt�0����i�rj����ݍ��E�F�)�BꟌ�.��R�ՎN��R�� ĵ���L���6��~Ҧ �՘U�tm���+t\��j��R`bR3��Bq����s"�\�7���e�x��y>,a̴1�ҲD>�C�y4J���W׿8ni'��A��Ck�x�3�w�kǻLѹ~�9����+Q&���ڵ�2M�@�ہ�
�r;���������ۿ��#:�p�I�ݴ�̳Ϛ/4�w�E����6-��+>5o5_�Ml�զ��3��^7X�3����(�b�6�a���zP�������s���%`�nm�&�P�,�Ш�����0�.:bQ�QV������w��+�����י�iKӮyHl�`mX��ʻkB���V�|��*�����m���St8���w���!�-�Eh`�p�M�ҙ,��){�+�}�B.J��z��f/����{��J!�7D	"��YJOc���s�[b~��Po=�dr�ʰi��_��	�� Ȋ!bƗ�U��Mym	L��7����ĚꮄȦn�꼼�,;������p�W2�Q��0|3ܔ�Lv,z�C��m����f������cC�⸌P�ވ3
�݂m�}$�Ħ*?v��og�žg'$�S�vU�R��Ǿ�YF5��6�m2Jª�*ͽ�  �2����z���pK�a�����b��:��	C��Y�+��.+IE���X��H�Du��"`%��C���sX���U��� �:��?j�yP��	0��I��6����fQ`�v L��^{��LX�I �K~ݠ�[$��R4ʐ�'t�Ƒ���o��}�\�`pX8�nLL��-���i�W�#�oߒ�̵͗b��c4gݕR[V&���� �a��+S�ѽ��&�eb�*��X���@t��H���C�Ŏ�6��d�b�4#�jD��tX9J,1�]�_4@�/���G��l�������7�3%��.�A����Aw*}�~���wS 4hh�}J|v��#hs4��Z`t��y�Mϋ���z(A�x:�uTm���m��4>M�V=�ku$G���6{l����=7�3����S��	U
���������@a�ы�£�����#*��#?��=�Z��m��sٱشA�8e0g�⏵'_w�#(l��#~�_Q^��u�
 ֦~h�H@�b㝄�� ��g� �xj�{ى(�?�)���)l��X5�v!��5�.Cݢ�X��C�"��e��X�H��TZ��8h���N�I��,��+pv�=�c3kxƘ�KY*�?ŦgUް�ϥ�a��u��
L�y+�$ޯ� �r!������Rv���
�׽m�����H�8.4ؠ/y�����<�Z�O��?b�Tǝ��"tW��*���\�2T>�[�5�(��"9� i,-j���քw�p	�O�+GSv&���͇f�!��7���<)���a�ҳu^���	��ǰ��$� PضH�mmܫ�`U�jK�|!E�MU@�a�:RyTx&���svf��mh�(C���J'�l/�:�Խ���!�	�o,���)�{-{#����F��'��Bf�(bQ{4�s���s��u���E�N;˗fzgz�~G	��]7c�u�t1":?*�͝K���I�ǟqZV����F�6_�P�<H�&��B�N�������E��!��);7�T�"��({p��о��s[�3�l�T�Y�X�t�6A�I�����U���}Q��R�\�姏���88k�5��k_Xw<�kxDC�U���]�s#���l�FXxB�Uh�*1�u"*��K�`���40>��́G�[�B����&�2��z��ێyw<�����vs�=��$��wF���Ս�d��<]�ܔ������������`�-w�jg.�?P��:�1��"._�	��Uĕ�I��;M,S
3�k�\���}�k>����v�#�ڥwf�������o�\|siJ��p��5���-�]�W }@k���<�Gsg=�8�DJ��C�h��b������Ű��H��J����4¢#��#�#0��)>����a+����d�/dEx���TJaJS5V�e	�3w �J-Z3�-�E2s�.ru����i�Oزt����>��D��5����i��Lv�z�{h0��QQ�J����%�؝��Dk��W;{L�9�l�-OBe]X��c��5 dw&JټM��|�{'�ҟ���d��I���ֳ��d؊L�6��|��X�����mg�og\Iw�������t�_����0ގGa���yR_5Ѽ���.��B̗�$�R����D<��~����4\%��Ye�4ˊ��˽�������n��p9Y���cz�#P��-���{p���C�k����3� `_ �QN%n�f����J�'��%FZ���<��|f:�E�*�l1"d�|e�P�);竏���^o@��|��!��>�{����CY�>DP� 5;/L��!N4�T/~��g�6*�_�	h���0yЊK�b�]�M0w�p���0�n'@��j�b��'S��\o���)P�qYf����"NXZ�Mi9٤n�9�����X ��(���RG�j��'^DZ���{!���+kR�h�HT��f���qg8�w^����J�� �75�欁�<,J������wb	�p�r^�������i2.����=usJ�,��ϯJ�2d��Ol�Ie�图}?����� �0��\h������0Y`��- ��
��+�&{��XVP��4h~��Uw��ڥ�IL�b]�nʐ����~��钣��z-dDc.mX�mU�+�G9�*r��/o��ډ�:̧u��o�����27	��K��Wdݵq+��KT;[���
�l���n��|��J6�[����vj>rKD��T�r�@�g���;�e k�9z{j̝,�4�Q�����ar��vm��"�͡��:t�< Tb߷TQ~�J�u�N��}r���jM'���������ޟh�F���*��r;��v$�˸zl͟�l��'�����5;��yJ0��7��1��I�0�ĝʛvo>8�_օ�&i�C�n�B�X�ߍo��a\�?�^������8�h�/ȷ�O�]����f3ėV0B�7}�"wP��<���[v�]���QL2�(mf\i޼
h���oG��ʟt�֮q�D2�M��~��p�=��B�>���E����U�Q��?�+�
:�`�K>D�
-���!V�ǞS���.�rC��FL�x	�`YWۛ׏�t񷕏��Y
l2�����l�ݒԮ�@3H Z�z �O�eK�d˜}��-tIK��^�-�a6GQr/D�A���Mh�!n�<�Z�h|����՚0�~(my�����`靗����v�5��}�<6�F�H`�f�oY�k��(���5E\IA�A��TB^b�O����+h'���X/"��=������B�b�~h���ϼ�[�| c�|69��R<@G�u�;�s�>oee�K�������&mr�?��h�C�M�����X3�o�1�*U���/
䐈&���d������N�Fe�o+�E�/ND��z�Ak�e��lcP(×j�T��Jܒ���fkNB~���R���󊡕��?N�6��������:�|���D�,�B�ՑY_�����+�!� x�O���z�������VPY'��Z�����8p]1sm�bO��lax���K��I���[�
}�ZMχ^�)A{�wg<����Ȁ�̴o��ѝ�/0ǻ�� ˞)�'usU�2.�J�y;3� �n\��t��m��Yk�X��}X'51ˋ�"�x1�-������f��V�v�gw`��e�����V�9��.
ȤCX��>�/�L)���6��T҄�n����1��H�]{�mĦ3HAU��l���Я�g44��|�$@w�T9Ƞ�AA,�S4�o��hp#��C'�E��Zt|�pȞ�3������
��AE�hʮ�i�q�H23�Y����3O�܋��0�DŒI�_^c(4�f��й����yk�9;A6�M���H ��`M+�}��l��7-XC��$�%o�*�sCQT�eY 0��}=Mi�yđ	z�����m@��8��+�p �G�\D�4�5/e���Pv�� 	����5Ux&��}�%���Nv}��NQ�q~s#��o��Ĵ�7`C��k�T�f�x$�r"���vvw�\�l��^?���/�9�Ga�J�îGt�m�zw�=Ӻ)
��)p��؅uH���}�3� -0`#3��]Xi�y����e�*��!~����*��Q���R��E�s�n��	u)�w�)~�b�/N&�u!l:(O�y��o7sJT	8H[���=j�VI�$+{ �Sw)�&�N��_���0�l�bICa&�|KQm��>pñ^���T������B�:�������z.�W�0��\U@< �iEv�j�ۆ_4i0��>��Z��@u�O�р��~��J�Q�WȮ5Tl$�0�Dq���c��kɻ�iݤ�W�K�ϥ�/�8J��:���w
6�N���b�}* ���l�$ǰ��W��0�Gl����O�39�dT����bﻈx�(���Z؅�Gļ�B]� Ԡ$�&?a!e�rŃ�q�liQ:�d���蚍����`,��՘x4zTnHps�d��_e�p��d驨&�(�w��Ԏ���-��q�
����Fy���!��
���'��t�h�a}�٘��j�@��nȡ#�C����a�r.�[�����c�#j���4���/AyJW~�,�uf�6"�������-N)������܎�VݣЦ<�m%�X(e���u0M��� Dfˑ3Sb�I
����6!�m�A�0z �#N�� ��o
��
����(P<?���=R�j��������B��]���gc��mX���ߓ@�ٖ{퉖J3.fX�!�p�;8u���Ɵ���Y}�$|dtn�!�Ȧ�mF<-*k�����Kꓞ����/���-ɂ�������OM{N��Ǣ��kX��VI�L��Ndh�H�E������������Z1
I$��Qm�x���c-�q\ͮ_m�]0qu�y˪��W�\�Y��P�A��t��3@��ULp�� �{�d�$�(!�I���,�ݖI7l;��mn�i�""]���m�%R1',�htV�ɨ�.�Q�ޒy!/\<q�ͷ���?�[�N�d����=Gm��Q
||}x9V��TP�ﳈ;9�d!��@	$3��Qt�ކfw� c7�#�Zq�3���C���I�J�QFVuD����}�-��u���I.�k"���v#��)}ά�'�HVO�/4`a��$���r`�25x%����`����C�n��Yе�t/I��ɥ$�y0c�fr�Zj]KY4`<=�y�JpRXǏ���çD<'b%���U+��Z�׉�T���e� ��~>snn��0Ll�*��O�^��s劜6�^o�h�ǑP��yM�p"��O�
	��u��RKT�JY[V�Ok��LC�3P����K����`ͬWeh��:l]��Gr�`M��ܫ�]�Q�5E�)&|���͹��{�jp��A���; ��;_�4�qy��k#X��]G�k< \����LY�!�����~
X�H{��=l�맊<DV���1�����WG��S�?�\��#[���^N��8�"Hc���9�E��t��Y	X���8Z�:4	���C���w�Nb>c�Il��+��^�h�ǓZ�tG&������O��$~.�#C����o�6.���+,&�mLQ3��RQ_�R�R����z��O�<j�*�w����``��
V¨�|���c�Y��1Qd��d~7A�RiiXW�E�I��%���F����COE) \
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
    $(wildcard include/config/SECURITY_NELF                       �      4     (            GNU  �       �          ��������    ��������� � t2�� @ uǀx     1���Í�&    �v �������Í�&    ǀx      1���������W�JV�2S#�d  #�h  ���   �2�=    �J[1�^_Ë�d  ��h  ����T  �����WVPh    Sh   ��������ȍ�&    �t& �������L  ��u�R89��  t���  ���   �������1�������UWVS�Ã�8���   d�    �D$41���P  �.   ��������.   ��&    ������z$u=V�ƃ���L  ��u��P  t,���  �B4���  �J(p�B8��1�^Í�&    1�Ít& ����   �$������$����&    ��    ��������   �@\���  ��t'���  �   �  ������P�   j �����X1�Í�&    ��&    ��������    ����  �@@���  ��t'���  �   �  ������P�   j �����X1���������WVS��L  ���   ��u��`  ��\  1�1�	�tJ��\  ��`  ǀL      �C\���  ��t'���  �   �  ������P�   j �����X[1�^_Ít& �����S�ù   ��8d�    �D$41��C�D$    �=    �D$    f�$f�D$f�D$f�D$$�D$2�D$(    �D$  �D$   �D$  �D$   f�L$�D$     �D$&  �D$,��  �C�   �����������  �C�   �T$���������  �C�   �T$���������  1��T$4d+    u��8[��������    �����U1�W�Ϲ   V��S�Ã�(d�    �D$$1��C�D$    �T$!�D$    f�D$f�D$�D$�D$�F�D$!��f���D$    f�D$"�Cf�L$�T$�D$  �$�C�   �T$��������  �D$f���=    ����  �T$$d+    u��([^_]�������v �����UW��V��S�Ã�d�    �D$1��ȈT$f��f�D$�D$$f���=    f�D$
��  �C1�$j �   �؍T$�����Y���R  1��T$d+    u��[^_]��������&    ������UWVS�p�X|�@(�~�=	� �P  vN=	� ��  =
	� �  �۸   �   �   E؉�S�����~~ Xuu�t& �1�[^_]Í�&    f�=	� ti=	� ��  �ۍ��   �   �   H؉����À����S������~~ ]t�S�0   �   ������Y뙍�&    f�S�3   ����&    ����   ���   �-    �@|��	��  �ٺ� ���)ى���
����)������2  S1ɺ   ���<���X�݃�U�   �   ���$������    X��   �~~ �����S�   �I�����[�� �����  0 ������
���   ��)˹   ��S������~~ X�����S�1   ������t& ��[�� �����  0 ������
���   ��)˹   ��S�{����~~ X�`���S�2   �����t& ���>S�   1ɉ��   �H���Z����f�U�@   �   ���.���X��������������  �  �t& ������    S�X���]  �������1�[Í�&    ��&    �����V��S�X\����������  ��t���  ���������������   �����1�[^������U��WVS�� ��u�@�   �+  ����������������t  �E��  ��  �D$������Å��f  �   ��������C   ���  ��ǃ�     �   ǃ�     ǃ�     ǃ�     ������ƅ���   ǃ\   �  ���  �   �F  ǃ`      ǃd  ��� ǃh      ǃ�     ǃ�  ����ǃx      ������   �   ��ǃ�      ǃ�  B   �����D$��0  ���u&�    ����  ��&    f�������� ��[^_]Ð�   �   ���_�����0  ��4  �|$�=    ���}  �|$���t��D$	�t��t$�������N@���$���D$�D$�ԉ׺���̉��|$�����T$�������)Ǎ�N�  �t$i�'  ȉ�8  �D$�������<  �<$F�D$
���D$
������=  �����D$�t$��>  	�	ȃ<$C��A  ��@  ��!΃|$�   ��  ��B  1Ƀ<$C��C  N��T$�L$
��D  ����F  !у<$G��G  ������E  ��!�����H  ��!��|$��I  !Ѓ|$��!Ј�J  ��   ��v �<$F~ǃP     ��t& �1��<$C����P  j ���   1ɺ   ���������G   ^��   j �	� �0  ��h �  j h�  j h��  j j ������	� �0  �� ���  ��j h �  j h�  j h��  j j ������		� �0  ���� j j j jj jj j ��������   �{l���  �� ����   ����������� [^_]Í�&    f������   1��n�����&    j �	� �0  ��h �  j h�  j h��  j j ������	� �0  ���� j h �  j h�  j h��  j j ������
	� �0  ���� j j j jj jj j ������� �����t& �ƃB  ��C  �������    �    ���������  ������������  �   ��������������=   �u  �W  �}  ��&    ��&    ������Ѻ   ������&    ��&    �������Ѻ   ������&    ��&    ������Q�Ѻ   �N���ZÍ�&    �t& ������Q�Ѻ   �.���ZÍ�&    �t& ������WVS�ÉЉʋs\��t  ��w��v?�`  �=    ��p  ���*  Q�   �   ���������E   Zu:[^_Í�    �ɍ|�<�`  ���t��`  ��#�p  	�렍�&    �v ��x  �ع@   �   �y���X[^_Ít& �����UW��V��S�Éȃ����   �<$�׃��|$���������|$�L$9��  �  ���  �   1����  �   �����Dǃ���u���	   ǃ�     ET$1ɉ�X  ��������T$�   ��������T$�   ����������������   �   ��P  �0   E؉���������$��%��  ����   	�R�   �n����F\Z���  ��t'���  �   �  ������P�   j �����X��1�[^_]Í�&    9��  ������፴&    �v �����UWV��S�Í��  d�-    ��d�    �D$1�����$    �D$    �D$    �l$�D$    ������������u��xN��������������������������    d�    ��u4���  ���T$d+    u1��[^_]Ít& ��   �E�����믐���������t�1��������������&    �����S�Ë��  �@�@��������  ���   �P|���  ���   �P|���  �~������  [�@�@�������&    �t& ������VS��L  ���   ��ug�    ��ǀL     ���T  ��P  ���  ���tF����w4�F\���  ��t'���  �   �  ������P�   j �����X1�[^Ít& ���   �������r�� �r�� ������������1�[^�f�f�f�f�f�f�f��������=     WV��S�Ëx\tW�   �   ������   �	   ���������
   ���������   ���������   ���������C   u$[^_É�   �������	   �������뭍t& ��غA   [^_�������&    ��&    ������WVS�Éȃ��{\���  ��w;�4�4  ��   ��u7���� ��5    �҅��o  ����[^_������v �    �   ��tɃ�
~��I   t�p��	��f�����	�뮍�&    �����U�   WVS�h\�Ë��  ���������H������   ���:�����	   �������&�����
   ������������C   u���غ   [��^_��]��������A   ������������э�&    �v �����S��  ��1ɸ�  ���&    �v �B��t�
��9�u�[Í�    ��  [Ð�����W��V�Ή���S���  �ú�   ���������   ���������   �؁��  ���������   ���������1�[�V   ^_�������&    f������U��  WV��S���x\���  ��w��H  ��P  �    ty��X  ��	�:  ���  ��to���  ���   F݃�u]���B  ���U  ��  ����  ��GZ �GZ ������    ���<  1��}�t& ����   ��[^_]�1ۃ�t���   ���:  ����   ����  ���   ��  ����  �    1҅���  ����   ����   ����   ��X  ��  ���  �0   EС    ����  ����[^_]������������U����v ����   ~����  ��[^_]Ít& �t���^  ��l  1҅������W������u��� 늍�    ��t�늍�&    f��� �a � �f ������    ����  1҃��L�����&    �����  1�����������  1���������u� �a �  d ��������    ���  1��������    ��us�� �a � �f ������    ����  1��������    ����  �   �|�����    �C����Z���� 0  �   ��������    1��D�����    ���T����`�����&    �v �    ���z  �   1ɉ�������   �>�����&    �v �0  �   ��������    ���[  1�������&    �t& �����U��WVS�Ã��@\�$���  ���D$�    ������  �$�|� ����   ����   �   ��X  ��  ǀ�     ��������t& ���`  �   ���������s�   �   �ؾ   ������@   �   �������1ɺ   ��������v ���x  �   ���������s�D� ��   �4�`  �؋N<������N4�V0��������V@���&����   �   ��������ND�   ��������$��<   u
��[^_]�f����ع Z  �   [^_]�������&    ������S�P\�Ë�P  ��~��u��T   t,��[������t& ��y[Í�&    �v ��[������&    ����  t���  �����t�    �    �������몍�    �!   �    �������둍�&    ��    �����UWV��S���x\��P  ����   ��u�   ������ŉ���1���   ��@��X  	tK�   ��D؋��  ���  ��t��T   ��  Ǉ�     �   �=     �L  9�������[^_]Ít& ��y��1�[^_]�f���l  ��X  �˃���   �P�1���wV�#   ��������=    �#  �Ë��  �   ��uT�t& 9���   �    ���  ���  ��l  �   9��e����    ���/  ��l  ���   [^_]Ð������t6�   ��t����   E�뎍�    ��
�   E��x�����&    ��   �f�����    �   �������=�  ��  ��O=    ��  ���  1�=    �= ����   �   M������v �����  ����f���l  1������x  ��&    ��&    �����U��WVS���X\�    ���.	  ǃd  ��� ǃh      ������E�$�v �=    �	  �������������    ����	  ���  ���������E  ��X  	�(  ǃ�     �ؾ@   �������L  ��u��\  % �  ������� ���  ���   ��ǃl      ������ǡ    ����  �=    ��  ���   1�1��"��&    �v ���@�լ  ��t�ՠ  9�u�    ����  �@��T  �=     �Š  ǃ�     �P�@��|  ���  �]  ��L  ���O  ��\    � �?  ��	�6  �    ����  �	   �    ��ǃT  	   ������t& ����  ǃX     ǃl     �   �   �������� 0  �   ���������<   ��  ��E   ��  ���z�����ǃ�      ��������  ������   �C�t& ���  ���������uD����}\�������uO�=     t���  ����  �������u���  ��������ƅ�t��    ��������d	  ��&    f��������먍�&    1�� �t& ����Ŭ  ����   �Š  9�u���Ŵ  �Ű  ��d  ��h  ��F��	��  ���D  ��	�����������������  ǃX     ������&    f��� �/  ��@�q���ǃX     ��   ǃ�     ������r�� �r�� ��������;�����&    ��    ���	  ǃ�      ��������m����t& ����� 1��%����t& ��x  �@   �������� ����� Z  �   ��������������&    �v �   �    ��������%�t& ��~   �������=�  ~e�=    ��  �d   ���������t��7����v �   ���  ��X  ǃl     �R�����&    ǃX     �<�����&    f����I�����&    f��    ���I	  ��1�[^_]�1���
����둸�  ��t& ���;1tF�P����u��	  �v �D$��  ��  1����    �J����  ���9�u�D$��  �	  ��&    ��&    �����UWV��S���=    �X\����
  ǃd  ��� �~ǃh      ������    �<$�t& ����u
  �������������    ���U
  ���:
  ���  ���������  ��X  	�  �n\��ǅT     ���  ���������E   �r  �F\��L  ���!  ��\  ��`  �р� 	���  �р��	��  �с�  @ 	��{  �p  ����  ���0   ����   	����������������   �   �������� 0  �   ���������<   ��  �    �"   ���������L  �@   ��u��J   �    t���    Dȉ�T  �    ���������T  ��ub�    ����	  �
   �t& �d   �����������  �~   �������=�  �5  �=    ��	  ��uË�T  ���%  �-    ����  1�1�����@�<լ  ����   �ՠ  9�u��@�Ŵ  �Ű  ��d  ��h  ��	�b  ���������p  �   ����������  ��T   ��   �    ����f��    ����  ǃ�      ��������    �o�����&    ���� 1��u����t& �    ���h  �   ������&    �� Z  �   ��������<�����&    �v ��x  �@   ��������w�����&    f����  ���~\���������   �=     t.���  ��%�t& ��=     t���  ���  ���������u(���  ��������~\�������tŉ������뼍t& �    ������    � |  �   �����������f��    ���H  �   ������&    f��    ���(  �   �������&    f�����T  ��������    ���������
  �t& �������������t& �    ���  �@  �����    ����
  �`  �|����    ����
  1���[^_]Ã������[^_]ø�  1���  ���&    �P���
  �8��9�u��
  ��&    �����UW��VS���p\�    ���0  ǆd  ��� ǆh      ������G�D$f��=    �  �������������    ����  ���  ����������  ��L  ��u[��X  	tRǆ�     �������1҉���������  ���   ��ǆl      ������$��t<�    ��~��  �t& ��-    ����  ǆ�      ��������<����t& �=     ��   ��\  �  � tv�    ���l  �  ��|  �  � �  ��<   ��  ��   �   ���  ����������  ǆl     ǆd  � 2 ǆh      ��  �v �D$����1�1��D��&    �v �   ���������  ��=�  O�9�}�\$�ŋ    ���.  ����t.��   ����������d   ���������t��    �����t& �D$����   �D$   ��   �=     ��  �D$����1��F�t& ��   ���������  ��=�  O�;$~�$�\$�    ���  ��;\$��  �T� ����������d   ���������t��    ������&    ����  �=     ��  ��   �D$������|  �|$~E�|$�  �D$������  �|$ u?��\  ������    ���Q  ��1�[^_]ËD$��uǆd   �  ǆh      ���  ���   �������|  ���  ���������ǆ�      �������������    ���  ���  ������   �I��&    ��  ����������>��������o\�������uE�=     t���  ����  �F�����u���  ��������Å���������o\�������t��������벍t& ��D$��   ��|  �������ǆd    �D$ǆh      ����  �|$�������<   �������   �   ���  ����������  ǆl     ������_\��|  ;��  ��   ���K  ��X  �P���ve����   �    ���������  ��&    f�ǆd     �   ��ǆh      ǆ�    [ ��������  ǆl     �%����-    ���O����  �t& ����m�����  �v ���,������  9�|  �����    �������  ��&    �v �D$�   ���   ���  ����������  ǆd  � 2 ǆh      ������\  �  � �Q����L$��   ��|  ����������+������&������1���ǆd    ǆh      ��   �   ���  ����������  ������   �   ���  ����������  ǆd    � ǆh      �������\  �  � �M����D$��   ��|  ��u4ǆd    ǆh      �����=     u�D$   ��   �T����D$�����������\  �  � t������R�T$h    ���   ���D$Qh   ������D$�T$���   �~����4  ��0  Vh    W������CpPh8   j4�t$ V��������   ����������X  ��	vL�   ��P  ����   ���  �+   �$   ��D΀��3   �,   D�QRPhU   W����������    u�PhG   W����������  h   W��������  ���  h@   W�������p  h�   W�������(�D$4d+    uc��81�[^_]Ã�uPhG   W����������  �+   �$   �,   �D��3   D���T  �����UVPhj   W��������T��������h�   P�D$h   ������D$���  ��h�   S�����X���Z�  �Ch�   Ph   ��������a  P���D$WVh�   Sh   ������D$���  WVU��hl   �t$������
   ���������uh�   �t$������������X���Z�  �L  WVU��h�   �t$������
   ���������u@h�   �t$������������X���Z�
  �C�t$$QRh�   Ph   ���������  ��  ���   �I|����   ���  �  �  D�Q�   RP�Gh�   Ph   �����1ɺ   ��S�  ���f  ���   �p|�Gh  h  h�   Ph   �����1ɺ   ��S�  ���a  �  �  �h  Ph   ��������  �+   ��<   �  D׋�P  �4��   WRP�E  P�D$(��P�D$$�ă�@P�t$�t$<�t$(h(  �t$D�������P  ��0���Q	  ��    ���Q	  hk  j�UP������ǃ�= �����   ��������  �E\���   �Q	  ���  �   �  ������P�   j �����[�Q	  �+   �  �  ��<   D�D��%  ������4  �t$hQ  �t$$h   �������0  �D$ ����t��4  �|$	ǉD$��	  �=     �L	  h�   ������t$h   ��������Q	  �Eh3  �����Ph   ��������Q	  ���  hs  �t$�����ZY�����QR�4�@  �Chd  Ph   �������p  ���  �Fh�  Ph   ���������  R�T$Q�L$P�Ch�  Ph�  ������L$�T$���  �FRh�  Ph�  ��������  �Fjh(  Ph�  �����1҃��  R�F�T$h  Ph�  ������T$���m  �F�$Sh(  Ph�  ������T$�����  ����  ���D  ����  �`  �FRh�  Ph�  ��������  �FRh�  Ph�  ������    ���  ��h  Vh�  ���������  �FRh�  Ph�  ������    1҃��"  �FRh�  Ph�  ������    1҃��"  �Fjh(  Ph�  �����1҃��`  �FSh(  Ph�  �����1҃�������Sh�  Vh�  ���������  �Fj h(  Ph�  �����1҃��D  �Fjh(  Ph�  �����1҃���  �Fh8  Ph�  ��������
  �FRh�  Ph�  ������    �   ���"  ���`  ��  �C�L$RhN  Ph�  ������L$���  P�V�D$h\  Rh�  ������D$���#  �FURhD  Ph�  ��������r  ��SQh�  Vh�  ��������  ���$SUQRhd  Vh�  ��������  �D$����  ������R���D$R�Vhx  Rh�  ������D$���F  Vh:  h�  �t$h�  ��������$  h  �t$h�  ���������  �D$��  �D$VQh�  �t$h�  ������D$���  h�  �t$h�  ���������  �Eh�  Ph�  ��������  ��hQ  Uh�  ��������I   h�  �t$h�  ���������  h  �t$h�  ��������9  h�  �t$h�  ������    ���%  ��  VRh�  �t$h�  ��������s  h  �t$h�  ���������"  h   �t$h�  ��������"  ��  QRh�  �t$h�  �������T  ����"  h�  �t$h�  ��������?!  h�  �t$h�  ������    ���7!  hx  �t$h�  ��������!  �@h_  Ph�  ���������   ��hQ  Vh�  �����1����r%  h�  �t$h�  ������`  ����!  hH  �t$h�  ������    ���!  h�  �t$h�  ������@  ����!  h�  �t$h�  ������   ����!  h  �t$h�  ������   ����!  h�  �t$h�  ������   ����!  h  �t$h�  ��������#  �Ȼ�  �����  ����Ȼ�� R���Ph0  �t$h�  ��������Y)  �Ƚ�  �����  ����Ƚ�� R���Ph�  �t$h�  ��������*  h9  �t$h�  ��������	&  �Gh!  Ph�  ���������%  �Ƚ�  �����  ����Ƚ�� R���Ph�  �t$h�  ��������*  ���  ��  �ș����  ����Ƚ�� R���Ph�  �t$h�  �������X  ���*  ���  ��  �ș����  ����Ƚ�� R���Ph  �t$h�  �������X  ���$+  �t�Ph�  �t$h�  ��������@(  �4�  Ph{  �t$h�  ��������'  �GhQ  Ph�  ���������(  hh  ��   �t$h�  ��������D$   �D$   ��'  h  �t$h�  ��������&  h�  �t$h�  ��������#&  hP  �t$h�  ������    ���&  ������    �    ������    �����                                  �     �  �  �  �         %                          msp3400                                                                 �     �     �                                                                 4  ;  F                             `  @  �                          0                          �  �          @  �                                                   P          `                                                                                                                              0      `  d  h  l  p  w  3   ~                   ��������      @  @         @     @      @  �0     �  �   �0      �        0  dolby   amsound standard    stereo_threshold    debug   once    opmode              0                                  �  �  �  �  �      K      $   #   '   (   K      $   #   '   (   �cL �cL �           0  ������������   ;   ~   ������������   ;   ~   �cL �cL �           0  ��������      N   k   ��������      N   k   r�� r�� �   �       0           0   B   H            0   B   H   �cL �cL �   �  0    0     	         !   %            0   B   H   GZ GZ �   �       0  ������������
   2   V            0   B   H   �cL �cL �   @       0        ��������(   ^            0   B   H   UUS UUS �   @       0  ������������
   2   V   ������������   O   ~   GZ GZ �   @      |              �  ���                �  ���         �> 9�A �   �         �cL ��O            GZ  �V #  � 2        GZ 9�] A  � 2        GZ GZ _  � 2        GZ ��O x  � 2        �cL  @Q :        	   GZ  @Q �    �     
   UUS ��Z �            GZ  @Q �  � 2        GZ  @Q T  � 2        GZ  @Q t  � 2          �>  �> �         !    �>  �> �         0    �>  �>           @   r�� r��   ���     P   GZ GZ 1  ���     Q    �a   d ?  ���     `     d   d U  ���     ����                    Setting I2S speed to %d
 7 unknown , dual  stereo mono %s: Audio:     Mode:     %s
 Mode:     %s (%s%s)
 Standard: %s (%s%s)
 Audmode:  0x%04x
 ACB:      0x%04x
 resume
 msp_reset
 chip reset failed
 msp_read(0x%x, 0x%x): 0x%x
 msp_write(0x%x, 0x%x, 0x%x)
 no on yes off suspend
 nicam  and  radio msp3400 msp3400 not found
 &state->wq rev1=0x%04x, rev2=0x%04x
 msp34xx kernel_thread() failed
 switching to radio mode
 AM (for carrier detect) FM Radio Terrestrial FM-mono/stereo Satellite FM-mono NICAM/FM (B/G, D/K) NICAM/FM (I) NICAM/AM (L) BTSC External input manual autodetect autodetect and autoselect in1 in2 in3 in4 in1 da in2 da mute 7 unknown FM set_audmode: %s
 SAT set_audmode: %s
 NICAM set_audmode: %s
 BTSC set_audmode: %s
 extern set_audmode: %s
 FM-Radio set_audmode: %s
 mono set_audmode
 set audmode %d
 switching to AM mono
 set_mode: %d
 stereo detect register: %d
 nicam sync=%d, mode=%d
 watch: nicam %d => %d
 msp3410 daemon started
 msp3410 thread: sleep
 msp3410 thread: wakeup
 thread: restart scan
 thread: no carrier scan
 detection still in progress
 5.5/5.85  B/G NICAM FM thread: exit
 msp34xxg daemon started
 msp34xxg thread: sleep
 msp34xxg thread: wakeup
 selected radio modus
 selected M (EIA-J) modus
 selected M (A2) modus
 selected SECAM-L modus
 selected M (BTSC) modus
 msp3400 daemon started
 msp3400 thread: sleep
 msp3400 thread: wakeup
 AM sound override
 carrier1 val: %5d / %s
 carrier2 val: %5d / %s
 mono stereo lang2 lang1 lang1+lang2 5.85  PAL D/K + SECAM NICAM 6.25  PAL D/K1 FM-stereo 6.74  PAL D/K2 FM-stereo 7.02  PAL SAT FM-stereo s/b 7.20  PAL SAT FM-stereo s 7.38  PAL SAT FM-stereo b 5.742 PAL B/G FM-stereo 5.85  PAL B/G NICAM 4.5   NTSC 5.5   PAL B/G 6.0   PAL I 6.5   PAL D/K + SAT + SECAM autodetect start 4.5/4.72  M Dual FM-Stereo 5.5/5.74  B/G Dual FM-Stereo 6.5/6.25  D/K1 Dual FM-Stereo 6.5/6.74  D/K2 Dual FM-Stereo 6.5  D/K FM-Mono (HDEV3) 6.5/5.74  D/K3 Dual FM-Stereo 6.5/5.85  L NICAM AM 6.0/6.55  I NICAM FM 6.5/5.85  D/K NICAM FM 4.5  M BTSC-Stereo 4.5  M BTSC-Mono + SAP 4.5  M EIA-J Japan Stereo 10.7  FM-Stereo Radio 6.5  SAT-Mono 7.02/7.20  SAT-Stereo 7.2  SAT ADR     `   �      @  �     P  �  �  �  0  �  �           @  `  �  0  �  p  �      �  @  �  p  �     `  �  `  �  �   �%    detected standard: %s(0x%08Lx)
 %s rev1 = 0x%04x rev2 = 0x%04x
 Routing:  0x%08x (input) 0x%08x (output)
   I/O error #%d (read 0x%02x/0x%02x)
 resetting chip, sound will go off.
 I/O error #%d (write 0x%02x/0x%02x)
    mute=%s scanning=%s volume=%d
  not an msp3400 (cannot read chip version)
  MSP%d4%02d%c-%c%d found on %s: supports %s%s%s, mode is %s
 scart switch: %s => %d (ACB=0x%04x)
    set source to %d (0x%x) for output %02x
    could not detect sound standard set_audmode called with mode=%d instead of set_source (ignored)
    set_audmode final source/matrix = 0x%x
 watch: rxsubchans %02x => %02x
 status=0x%x, stereo=%d, bilingual=%d -> rxsubchans=%d
  setting standard: %s (0x%04x)
  current standard: %s (0x%04x)
  autodetection failed, switching to backup standard: %s (0x%04x)
    started autodetect, waiting for result
 detection still in progress after 10 tries. giving up.
 detected standard: %s (0x%04x)
 mono sound carrier: %d.%03d MHz
    main sound carrier: %d.%03d MHz
    NICAM/FM carrier  : %d.%03d MHz
    NICAM/AM carrier  : %d.%03d MHz
    FM-stereo carrier : %d.%03d MHz
    6.5/5.85  D/K NICAM FM (HDEV2)  6.5/5.85  D/K NICAM FM (HDEV3) parm=dolby:Activates Dolby processing parm=amsound:Hardwire AM sound at 6.5Hz (France), FM can autoscan parm=standard:Specify audio standard: 32 = NTSC, 64 = radio, Default: Autodetect parm=stereo_threshold:Sets signal threshold to activate stereo parm=debug:Enable debug messages [0-3] parm=once:No continuous stereo monitoring parm=opmode:Forces a MSP3400 opmode. 0=Manual, 1=Autodetect, 2=Autodetect and autoselect parmtype=dolby:bool parmtype=amsound:bool parmtype=standard:int parmtype=stereo_threshold:int parmtype=debug:int parmtype=once:bool parmtype=opmode:int license=GPL author=Gerd Knorr description=device driver for msp34xx TV sound processor �          ��     �          ��     �          ��     �          ��     �          ��               ��               $� �             �                                  �                  +                                                              �           `                       �     ����                     @Q �   �V �  9�]    �a     d 9   �f S  ��O m   @Q �   �> �  �cL �  UUS �  GZ �   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                           ��       `     '       2     A   `   e     N   �   0     Z      9     i   2   a    }       (    
 �   @  c     �   �  B     �      P     �   �        �   P  |     �           �           �            �   �  3     �   �  �     �   �  p       �  �       V  k     &  0  \    1  �  �     A  �  "     M  a       ^  �  @     i     �    s          
 {           �  �        �  0      
 �  y  �    �  �       
 �          
 �  `  l    
 �  .  *     �  @       
 �  0  V    �  �  �     �  X                        �            
     0  �   \    
 ?  `   0    
 F     0    
 S  `  ,    
 a  @      
 o  �  P    
 }      &     �  &   B     �  h   Q     �  �   ?     �  �   '     �    *     
  I  Y        �       9           G  �      
 Y  �       t          �  �      
 �  �       �  (        �  �  	    
 �  �       �  <          �      
 4          M  P        [  �      
 m         �  d        �        
 �  &       �  x        �        
 �  :       �  F         X  9     '           ��:  �  �     I  @  �     ]  4      
 g         
 q  s  .     �  �  �     �  �     
 �     5    �  H      
 �  �  -    �  `  @   
 �  �  $       �  �     %  �  3    :  �	  �    O           l  �   0     �  �        �  �  O    �             �          �             �             �                                         �  B    /             L      
     [  �        m  `       {             �           �             �          �             �  �  g     �             �                            `      ,             7           C             S             b  p  ?     x  �  �     �             �             �             �             �  �  �     �             �             �             	             &	  �        3	             >	  �  �     H	             _	             l	             	             �	             �	  �  �     �	             �	             �	  �%  
    �	  `  X    �	  @        
             
  p  T     )
             1
             >
             M
             _
             v
             �
             �
  `   0    
 �
             �
          �
          �
             �
             �
               �   �                 -          6              msp3400-driver.c msp_s_i2s_clock_freq msp_s_i2s_clock_freq.cold msp_querystd msp_s_tuner msp_log_status msp_log_status.cold CSWTCH.74 msp_g_tuner msp_s_frequency msp_resume msp_resume.cold msp_s_std reset_off.1 reset_on.2 write.3 msp_reset.cold msp_read msp_read.cold msp_write msp_write.cold msp_s_ctrl msp_s_ctrl.cold msp_suspend msp_suspend.cold msp_remove msp_probe msp_ops __key.4 msp_ctrl_ops msp_probe.cold opmode_str CSWTCH.84 scarts msp_set_scart.cold scart_names msp_s_routing msp_s_radio msp_s_radio.cold msp_driver_init msp_driver msp_driver_exit msp3400_pm_ops msp_id msp_core_ops msp_tuner_ops msp_audio_ops msp_video_ops __UNIQUE_ID_dolby282 __UNIQUE_ID_amsound281 __UNIQUE_ID_standard280 __UNIQUE_ID_stereo_threshold279 __UNIQUE_ID_debug278 __UNIQUE_ID_once277 __UNIQUE_ID_opmode276 __UNIQUE_ID_dolbytype275 __param_dolby __param_str_dolby __UNIQUE_ID_amsoundtype274 __param_amsound __param_str_amsound __UNIQUE_ID_standardtype273 __param_standard __param_str_standard __UNIQUE_ID_stereo_thresholdtype272 __param_stereo_threshold __param_str_stereo_threshold __UNIQUE_ID_debugtype271 __param_debug __param_str_debug __UNIQUE_ID_oncetype270 __param_once __param_str_once __UNIQUE_ID_opmodetype269 __param_opmode __param_str_opmode __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 msp3400-kthreads.c msp_set_source msp34xxg_set_source CSWTCH.91 CSWTCH.92 msp34xxg_set_source.cold msp34xxg_set_sources msp_stdlist msp3400c_set_audmode strmode.0 msp3400c_set_audmode.cold msp3400c_init_data msp3400c_set_mode.cold msp_detect_stereo.cold msp3410d_thread.cold msp34xxg_thread.cold msp3400c_carrier_detect_main msp3400c_carrier_detect_65 msp3400c_carrier_detect_55 msp3400c_thread.cold v4l2_i2c_subdev_init msp_debug devm_kmalloc __msecs_to_jiffies __this_module snprintf __stack_chk_guard msp3410d_thread v4l2_ctrl_handler_init_class cleanup_module msp_stereo_thresh msp_write_dsp set_freezable msp_dolby schedule_timeout_interruptible msp_amsound add_wait_queue msp3400c_set_carrier i2c_transfer_buffer_flags kthread_should_stop __wake_up msp_detect_stereo __fentry__ init_module wake_up_process __refrigerator msp_standard_std_name msp_reset v4l2_ctrl_handler_log_status v4l2_ctrl_handler_setup schedule __stack_chk_fail msp_set_audmode _dev_info i2c_register_driver v4l2_device_unregister_subdev _dev_err msp_standard mutex_lock msp_sleep media_entity_pads_init kthread_stop freezing_slow_path current_task _dev_warn msp_set_scart default_wake_function __init_waitqueue_head msp3400c_thread msp3400c_set_mode msp_write_dem kthread_create_on_node msp_update_volume strscpy mutex_unlock param_ops_bool v4l2_ctrl_new_std v4l2_ctrl_handler_free system_freezing_cnt i2c_transfer __mod_i2c__msp_id_device_table remove_wait_queue msp_read_dsp msp_read_dem i2c_del_driver param_ops_int _dev_printk msp34xxg_thread v4l2_ctrl_cluster msp_once __SCT__might_resched     �  
   u       a   �  �   u  �   �  �   	  �     �   �  �   �  �   �    �    z  )    0  �  5    A  �  �  �  �  �  �  �    �  
  u      E  �  Q  �  �  �  �  �  �  z  �  u  2    B    O    a    p  �  y    �  �  �    �  �  �    �  z  �  �  �  �  �  z  ]  �  f    u  u      �  z  �  �  �  �  �  z  �  u  �    �  �        z  $  �  1  �    u  �    �    �  �  �  u  �    �  �  �  �  �  �  �  �  �  �  �  �    �         �  '  �  F  v  U    \  t  �  �  �    �    	  �  ;	  u  �	  u  �	    �
    )  |  C    a  �  k    �  �  �    �  �  �  �      .  �  8    [  �  e      �  �  u  �    �  �  �  �  �    �    �      �  !  �  A  �  a  �  �  �  �  u  �    �    �    1  �  �  �  �  �  �  �  �  �  b  �  �  �  �  �  �  z  �  �  �  �  �  �  �  w  �  �    �    �    �    �  -  z  I  �  S  �  ^  �  e  �  q  �  �  �  �  �  �  �  �  u  �    I  �  `  �  q  �  x  �  �  �  �  �  �    �    �    �    �            -    A  �  `    g    y  u  �    �    �  �  q  �  w    �  	  �    �  �  �  �  �  �  �  �    �    �  !  �  '    D    Q  u  �    �  �  �  u  �    �           u  *    e  u  m    �    �    �  �    u  
    $    4    R  �  W  u  _    �  �  �  u  �    �    �    �  u    u  
      �  =    B  u  J    a  �  �  u  �    �    �  �  �    �  �  �  �    �    �  #    /  �  D    N  �  [  �  w    �    �    �  �  4  �  M  �  a  �  �  �  �  u  �    9  �  ?  u  j  u  r    �  u  �      �    u  #    n    �  �  �  u  �    �  �  �  u  �    �  �  �  u  �    �  �    �  W  �  ^  u  l  u  �    �    �  u  �  �  �      u  $    ?  �  p    �    �  �  �  �  �  �  �  �    �  %  u  3    C  �  a    p    ~    �      �    �  1  u  9    J  �  v  �  �    �  �  �  �  �  u  �    �  �  A   u  I     e     �     �     �   	  �     �     �     �   �  �   u  �     !  �  !  u  !    )!  �  .!  u  7!    ?!    K!  �  }!  �  �!  �  "    !"    4"  ~  @"  �  \"  �  {"  �  �"  u  �"    �"  �  �"  �  �"  u  �"    �"  u  	#    #    &#    -#    I#  �  [#    u#  u  �#  u  �#    �#  �  �#  u  �#  u  �#    �#    $  �  -$  �  ;$  �  R$  �  h$  �  �$  �  �$  �  �$  u  �$    �$  u  �$    �$  u  �$    %  u  %    +%  �  :%  u  B%    Q%  u  Y%    h%  u  p%    �%    �%  	  �%    �%    �%  �  �%  u  �%    �%  �  &  u  	&    &  �  &  u  #&    /&  �  [&  �  d&  �  �&  �  �&  u  �&    �&  u  �&    �&  �  �&  �  �&  u  �&    �&    '    ''  �  x'  �  �'  u  �'    �'    �'  �  �'  �  �'  u  �'    �'  �  (  �  8(  u  @(    Z(  �  f(  �  o(  u  �(  �  �(    �(  u  �(    !)  �  4)  �  L)  �  Q)  u  �)  �  �)  �  �)  �  �)  �  �)  �  �)  �  �)    C*    U*  �  �*    �*  u  �*    �*  �  +  u  +    !+    E+  u  R+    l+    y+  �  �+    �+    	,  �  ,    ,,  �  h,    �,  �  �,                 �  .     F   	  L   �  U     a   y  p   �  �     �     �     �     �     �     �   �  �     �     �   �  �     �   �     	    �        �  %  z  <    B  �  P    U    Z    f    t  �  |    �  �  �  �  �    �    �  �  �    �    �  �  �    �    �    �  �  �    �    �    �  �        	    �  &  �  3  	  <  �  C  �  M    R    ]  	  f  �  p  �  }  	  �  �  �  �  �    �    �    �  �  �    �    �    �    �    �  	  �    �  �          %    *    /  	  5    :  �  I    Q    V    [    b    h    m  �  u    z    �    �    �  	  �  �  �    �    �    �       �    �  +    Q  �  W    \    a    f    x    �    �    �  �  �    �  u  �    �  	  �    �  �  �    �          �          #  �  3    ;  	  A    F  �  T    \    b    g  �  o    �  	  �    �  �  �    �    �    �  �  �    �    �    �  �  �    �  	  �    �  �  �              �  #    4    =    B    K    Q    V  �  ^    g    m    r  �  x  u  �    �    �    �  �  �    �    �    �  �  �  u  �    �    �    �  �  �  u  �    �    �    �  �                �  )  	  /    4  �  <    F    L    Q  �  [    e    k    p  �  z    �    �    �  �  �    �    �    �  �  �  u  �    �    �    �    �    �  �  �    �          �        	  "    '  �  /    9    ?    D  �  L    [  	  a    f  �  x    �    �    �  �  �    �    �  	  �    �  �  �    �    �    �  �  �    �    �  	  	    	  �  	    	    !	    &	  �  .	    6	    <	    A	  �  I	    Q	    W	    \	  �  d	    i	    r	    w	  �  	    �	    �	    �	  �  �	    �	    �	    �	  �  �	  u  �	    �	    �	  	  �	    �	  �  �	    �	    �	    �	  �  �	    �	  	  
    

  �  
    
    
  	  '
    ,
  �  :
    ?
    H
    M
  �  U
    Z
    c
    h
  �  m
  u  u
    z
    �
    �
  �  �
    �
    �
    �
  �  �
    �
    �
    �
  �  �
    �
    �
    �
  �  �
    �
  	  �
    �
  �     u                �  (    -    6    ;  �  H    M    V    [  �  h    m    v    {  �  �    �    �    �  �  �    �  	  �    �  �  �    �  	        �          #    (  �  0    8    >    C  �  K    n  	  w    |  �  �    �  	  �    �  �  �    �  	  �       �          !    &  �  .    5    ;    D    I  �  Q    Y    _    d  �  l    q    v        �  �  �    �    �    �  �  �    �    �    �  �  �    �    �    �  �  �  u  �       �          x     �          �      �     {     �        $     (     ,     0     4     8     <     @     D     �     �     �     �     �     �     �                   $    (    ,    D    H    d    h    t    x    �    �         8    @    D    H    L    P    T    X    \    H    L    P    T    X    �  	  �    �    �        $    <    T    l    �    �    �    �  	  �  	  �        ,    D    \    t    �                                                   $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     �     �     �     �     �              x     �     �          x     �  $   �  (     ,   x  0   �  8   �  <     @   x  D   �  L   ~  P     T   x  X   �  `   u  d     h   x  l   �  t   �  x     |   x  �   �  �     $     (     <     |     �     �     �     �     �     �     �     �     �                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.text.unlikely .rel.init.text .rel.exit.text .rel.rodata .rodata.str1.1 .rel__mcount_loc .rodata.str1.4 .modinfo .rel__param .rel.data .bss .comment .note.GNU-stack                                                         4   (                  2             `   �,                 .   	   @       �k  �              <             *-  �                 8   	   @       �y  �              O              ;                    K   	   @       L�                  ^             4;  
                  Z   	   @       l�                 m             @;  �                  i   	   @       |�  0     
         u      2       �B  b                �             RK  �                  �   	   @       ��  (              �      2       �K  �                �             {P  �                 �             S  �                  �   	   @       ԉ  �               �             �S                     �   	   @       ��  �               �             �T                    �      0       �T  &                 �              �T                                   �T  �     t         	              �`  K                               <�  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ETWORK) \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    *pf;
	unsigned long flags;
	bool found = false;

	might_sleep();

	if (WARN_ON_ONCE(static_obj(key)))
		return;

	raw_local_irq_save(flags);
	lockdep_lock();

	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (k == key) {
			hlist_del_rcu(&k->hash_entry);
			found = true;
			break;
		}
	}
	WARN_ON_ONCE(!found && debug_locks);
	if (found) {
		pf = get_pending_free();
		__lockdep_free_key_range(pf, key, 1);
		call_rcu_zapped(pf);
	}
	lockdep_unlock();
	raw_local_irq_restore(flags);

	/* Wait until is_dynamic_key() has finished accessing k->hash_entry. */
	synchronize_rcu();
}
EXPORT_SYMBOL_GPL(lockdep_unregister_key);

void __init lockdep_init(void)
{
	printk("Lock dependency validator: Copyright (c) 2006 Red Hat, Inc., Ingo Molnar\n");

	printk("... MAX_LOCKDEP_SUBCLASSES:  %lu\n", MAX_LOCKDEP_SUBCLASSES);
	printk("... MAX_LOCK_DEPTH:          %lu\n", MAX_LOCK_DEPTH);
	printk("... MAX_LOCKDEP_KEYS:        %lu\n", MAX_LOCKDEP_KEYS);
	printk("... CLASSHASH_SIZE:          %lu\n", CLASSHASH_SIZE);
	printk("... MAX_LOCKDEP_ENTRIES:     %lu\n", MAX_LOCKDEP_ENTRIES);
	printk("... MAX_LOCKDEP_CHAINS:      %lu\n", MAX_LOCKDEP_CHAINS);
	printk("... CHAINHASH_SIZE:          %lu\n", CHAINHASH_SIZE);

	printk(" memory used by lock dependency info: %zu kB\n",
	       (sizeof(lock_classes) +
		sizeof(lock_classes_in_use) +
		sizeof(classhash_table) +
		sizeof(list_entries) +
		sizeof(list_entries_in_use) +
		sizeof(chainhash_table) +
		sizeof(delayed_free)
#ifdef CONFIG_PROVE_LOCKING
		+ sizeof(lock_cq)
		+ sizeof(lock_chains)
		+ sizeof(lock_chains_in_use)
		+ sizeof(chain_hlocks)
#endif
		) / 1024
		);

#if defined(CONFIG_TRACE_IRQFLAGS) && defined(CONFIG_PROVE_LOCKING)
	printk(" memory used for stack traces: %zu kB\n",
	       (sizeof(stack_trace) + sizeof(stack_trace_hash)) / 1024
	       );
#endif

	printk(" per task-struct memory footprint: %zu bytes\n",
	       sizeof(((struct task_struct *)NULL)->held_locks));
}

static void
print_freed_lock_bug(struct task_struct *curr, const void *mem_from,
		     const void *mem_to, struct held_lock *hlock)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=========================\n");
	pr_warn("WARNING: held lock freed!\n");
	print_kernel_ident();
	pr_warn("-------------------------\n");
	pr_warn("%s/%d is freeing memory %px-%px, with a lock still held there!\n",
		curr->comm, task_pid_nr(curr), mem_from, mem_to-1);
	print_lock(hlock);
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static inline int not_in_range(const void* mem_from, unsigned long mem_len,
				const void* lock_from, unsigned long lock_len)
{
	return lock_from + lock_len <= mem_from ||
		mem_from + mem_len <= lock_from;
}

/*
 * Called when kernel memory is freed (or unmapped), or if a lock
 * is destroyed or reinitialized - this code checks whether there is
 * any held lock in the memory range of <from> to <to>:
 */
void debug_check_no_locks_freed(const void *mem_from, unsigned long mem_len)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	unsigned long flags;
	int i;

	if (unlikely(!debug_locks))
		return;

	raw_local_irq_save(flags);
	for (i = 0; i < curr->lockdep_depth; i++) {
		hlock = curr->held_locks + i;

		if (not_in_range(mem_from, mem_len, hlock->instance,
					sizeof(*hlock->instance)))
			continue;

		print_freed_lock_bug(curr, mem_from, mem_from + mem_len, hlock);
		break;
	}
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(debug_check_no_locks_freed);

static void print_held_locks_bug(void)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("====================================\n");
	pr_warn("WARNING: %s/%d still has locks held!\n",
	       current->comm, task_pid_nr(current));
	print_kernel_ident();
	pr_warn("------------------------------------\n");
	lockdep_print_held_locks(current);
	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

void debug_check_no_locks_held(void)
{
	if (unlikely(current->lockdep_depth > 0))
		print_held_locks_bug();
}
EXPORT_SYMBOL_GPL(debug_check_no_locks_held);

#ifdef __KERNEL__
void debug_show_all_locks(void)
{
	struct task_struct *g, *p;

	if (unlikely(!debug_locks)) {
		pr_warn("INFO: lockdep is turned off.\n");
		return;
	}
	pr_warn("\nShowing all locks held in the system:\n");

	rcu_read_lock();
	for_each_process_thread(g, p) {
		if (!p->lockdep_depth)
			continue;
		lockdep_print_held_locks(p);
		touch_nmi_watchdog();
		touch_all_softlockup_watchdogs();
	}
	rcu_read_unlock();

	pr_warn("\n");
	pr_warn("=============================================\n\n");
}
EXPORT_SYMBOL_GPL(debug_show_all_locks);
#endif

/*
 * Careful: only use this function if you are sure that
 * the task cannot run in parallel!
 */
void debug_show_held_locks(struct task_struct *task)
{
	if (unlikely(!debug_locks)) {
		printk("INFO: lockdep is turned off.\n");
		return;
	}
	lockdep_print_held_locks(task);
}
EXPORT_SYMBOL_GPL(debug_show_held_locks);

asmlinkage __visible void lockdep_sys_exit(void)
{
	struct task_struct *curr = current;

	if (unlikely(curr->lockdep_depth)) {
		if (!debug_locks_off())
			return;
		pr_warn("\n");
		pr_warn("================================================\n");
		pr_warn("WARNING: lock held when returning to user space!\n");
		print_kernel_ident();
		pr_warn("------------------------------------------------\n");
		pr_warn("%s/%d is leaving the kernel with locks still held!\n",
				curr->comm, curr->pid);
		lockdep_print_held_locks(curr);
	}

	/*
	 * The lock history for each syscall should be independent. So wipe the
	 * slate clean on return to userspace.
	 */
	lockdep_invariant_state(false);
}

void lockdep_rcu_suspicious(const char *file, const int line, const char *s)
{
	struct task_struct *curr = current;
	int dl = READ_ONCE(debug_locks);

	/* Note: the following can be executed concurrently, so be careful. */
	pr_warn("\n");
	pr_warn("=============================\n");
	pr_warn("WARNING: suspicious RCU usage\n");
	print_kernel_ident();
	pr_warn("-----------------------------\n");
	pr_warn("%s:%d %s!\n", file, line, s);
	pr_warn("\nother info that might help us debug this:\n\n");
	pr_warn("\n%srcu_scheduler_active = %d, debug_locks = %d\n%s",
	       !rcu_lockdep_current_cpu_online()
			? "RCU used illegally from offline CPU!\n"
			: "",
	       rcu_scheduler_active, dl,
	       dl ? "" : "Possible false positive due to lockdep disabling via debug_locks = 0\n");

	/*
	 * If a CPU is in the RCU-free window in idle (ie: in the section
	 * between rcu_idle_enter() and rcu_idle_exit(), then RCU
	 * considers that CPU to be in an "extended quiescent state",
	 * which means that RCU will be completely ignoring that CPU.
	 * Therefore, rcu_read_lock() and friends have absolutely no
	 * effect on a CPU running in that state. In other words, even if
	 * such an RCU-idle CPU has called rcu_read_lock(), RCU might well
	 * delete data structures out from under it.  RCU really has no
	 * choice here: we need to keep an RCU-free window in idle where
	 * the CPU may possibly enter into low power mode. This way we can
	 * notice an extended quiescent state to other CPUs that started a grace
	 * period. Otherwise we would delay any grace period as long as we run
	 * in the idle task.
	 *
	 * So complain bitterly if someone does call rcu_read_lock(),
	 * rcu_read_lock_bh() and so on from extended quiescent states.
	 */
	if (!rcu_is_watching())
		pr_warn("RCU used illegally from extended quiescent state!\n");

	lockdep_print_held_locks(curr);
	pr_warn("\nstack backtrace:\n");
	dump_stack();
}
EXPORT_SYMBOL_GPL(lockdep_rcu_suspicious);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0 */
/*
 * kernel/lockdep_internals.h
 *
 * Runtime locking correctness validator
 *
 * lockdep subsystem internal functions and variables.
 */

/*
 * Lock-class usage-state bits:
 */
enum lock_usage_bit {
#define LOCKDEP_STATE(__STATE)		\
	LOCK_USED_IN_##__STATE,		\
	LOCK_USED_IN_##__STATE##_READ,	\
	LOCK_ENABLED_##__STATE,		\
	LOCK_ENABLED_##__STATE##_READ,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	LOCK_USED,
	LOCK_USED_READ,
	LOCK_USAGE_STATES,
};

/* states after LOCK_USED_READ are not traced and printed */
static_assert(LOCK_TRACE_STATES == LOCK_USAGE_STATES);

#define LOCK_USAGE_READ_MASK 1
#define LOCK_USAGE_DIR_MASK  2
#define LOCK_USAGE_STATE_MASK (~(LOCK_USAGE_READ_MASK | LOCK_USAGE_DIR_MASK))

/*
 * Usage-state bitmasks:
 */
#define __LOCKF(__STATE)	LOCKF_##__STATE = (1 << LOCK_##__STATE),

enum {
#define LOCKDEP_STATE(__STATE)						\
	__LOCKF(USED_IN_##__STATE)					\
	__LOCKF(USED_IN_##__STATE##_READ)				\
	__LOCKF(ENABLED_##__STATE)					\
	__LOCKF(ENABLED_##__STATE##_READ)
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	__LOCKF(USED)
	__LOCKF(USED_READ)
};

#define LOCKDEP_STATE(__STATE)	LOCKF_ENABLED_##__STATE |
static const unsigned long LOCKF_ENABLED_IRQ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_USED_IN_##__STATE |
static const unsigned long LOCKF_USED_IN_IRQ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_ENABLED_##__STATE##_READ |
static const unsigned long LOCKF_ENABLED_IRQ_READ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_USED_IN_##__STATE##_READ |
static const unsigned long LOCKF_USED_IN_IRQ_READ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKF_ENABLED_IRQ_ALL (LOCKF_ENABLED_IRQ | LOCKF_ENABLED_IRQ_READ)
#define LOCKF_USED_IN_IRQ_ALL (LOCKF_USED_IN_IRQ | LOCKF_USED_IN_IRQ_READ)

#define LOCKF_IRQ (LOCKF_ENABLED_IRQ | LOCKF_USED_IN_IRQ)
#define LOCKF_IRQ_READ (LOCKF_ENABLED_IRQ_READ | LOCKF_USED_IN_IRQ_READ)

/*
 * CONFIG_LOCKDEP_SMALL is defined for sparc. Sparc requires .text,
 * .data and .bss to fit in required 32MB limit for the kernel. With
 * CONFIG_LOCKDEP we could go over this limit and cause system boot-up problems.
 * So, reduce the static allocations for lockdeps related structures so that
 * everything fits in current required size limit.
 */
#ifdef CONFIG_LOCKDEP_SMALL
/*
 * MAX_LOCKDEP_ENTRIES is the maximum number of lock dependencies
 * we track.
 *
 * We use the per-lock dependency maps in two ways: we grow it by adding
 * every to-be-taken lock to all currently held lock's own dependency
 * table (if it's not there yet), and we check it for lock order
 * conflicts and deadlocks.
 */
#define MAX_LOCKDEP_ENTRIES	16384UL
#define MAX_LOCKDEP_CHAINS_BITS	15
#define MAX_STACK_TRACE_ENTRIES	262144UL
#define STACK_TRACE_HASH_SIZE	8192
#else
#define MAX_LOCKDEP_ENTRIES	(1UL << CONFIG_LOCKDEP_BITS)

#define MAX_LOCKDEP_CHAINS_BITS	CONFIG_LOCKDEP_CHAINS_BITS

/*
 * Stack-trace: tightly packed array of stack backtrace
 * addresses. Protected by the hash_lock.
 */
#define MAX_STACK_TRACE_ENTRIES	(1UL << CONFIG_LOCKDEP_STACK_TRACE_BITS)
#define STACK_TRACE_HASH_SIZE	(1 << CONFIG_LOCKDEP_STACK_TRACE_HASH_BITS)
#endif

/*
 * Bit definitions for lock_chain.irq_context
 */
#define LOCK_CHAIN_SOFTIRQ_CONTEXT	(1 << 0)
#define LOCK_CHAIN_HARDIRQ_CONTEXT	(1 << 1)

#define MAX_LOCKDEP_CHAINS	(1UL << MAX_LOCKDEP_CHAINS_BITS)

#define MAX_LOCKDEP_CHAIN_HLOCKS (MAX_LOCKDEP_CHAINS*5)

extern struct lock_chain lock_chains[];

#define LOCK_USAGE_CHARS (2*XXX_LOCK_USAGE_STATES + 1)

extern void get_usage_chars(struct lock_class *class,
			    char usage[LOCK_USAGE_CHARS]);

extern const char *__get_key_name(const struct lockdep_subclass_key *key,
				  char *str);

struct lock_class *lock_chain_get_class(struct lock_chain *chain, int i);

extern unsigned long nr_lock_classes;
extern unsigned long nr_zapped_classes;
extern unsigned long nr_zapped_lock_chains;
extern unsigned long nr_list_entries;
long lockdep_next_lockchain(long i);
unsigned long lock_chain_count(void);
extern unsigned long nr_stack_trace_entries;

extern unsigned int nr_hardirq_chains;
extern unsigned int nr_softirq_chains;
extern unsigned int nr_process_chains;
extern unsigned int nr_free_chain_hlocks;
extern unsigned int nr_lost_chain_hlocks;
extern unsigned int nr_large_chain_blocks;

extern unsigned int max_lockdep_depth;
extern unsigned int max_bfs_queue_depth;
extern unsigned long max_lock_class_idx;

extern struct lock_class lock_classes[MAX_LOCKDEP_KEYS];
extern unsigned long lock_classes_in_use[];

#ifdef CONFIG_PROVE_LOCKING
extern unsigned long lockdep_count_forward_deps(struct lock_class *);
extern unsigned long lockdep_count_backward_deps(struct lock_class *);
#ifdef CONFIG_TRACE_IRQFLAGS
u64 lockdep_stack_trace_count(void);
u64 lockdep_stack_hash_count(void);
#endif
#else
static inline unsigned long
lockdep_count_forward_deps(struct lock_class *class)
{
	return 0;
}
static inline unsigned long
lockdep_count_backward_deps(struct lock_class *class)
{
	return 0;
}
#endif

#ifdef CONFIG_DEBUG_LOCKDEP

#include <asm/local.h>
/*
 * Various lockdep statistics.
 * We want them per cpu as they are often accessed in fast path
 * and we want to avoid too much cache bouncing.
 */
struct lockdep_stats {
	unsigned long  chain_lookup_hits;
	unsigned int   chain_lookup_misses;
	unsigned long  hardirqs_on_events;
	unsigned long  hardirqs_off_events;
	unsigned long  redundant_hardirqs_on;
	unsigned long  redundant_hardirqs_off;
	unsigned long  softirqs_on_events;
	unsigned long  softirqs_off_events;
	unsigned long  redundant_softirqs_on;
	unsigned long  redundant_softirqs_off;
	int            nr_unused_locks;
	unsigned int   nr_redundant_checks;
	unsigned int   nr_redundant;
	unsigned int   nr_cyclic_checks;
	unsigned int   nr_find_usage_forwards_checks;
	unsigned int   nr_find_usage_backwards_checks;

	/*
	 * Per lock class locking operation stat counts
	 */
	unsigned long lock_class_ops[MAX_LOCKDEP_KEYS];
};

DECLARE_PER_CPU(struct lockdep_stats, lockdep_stats);

#define __debug_atomic_inc(ptr)					\
	this_cpu_inc(lockdep_stats.ptr);

#define debug_atomic_inc(ptr)			{		\
	WARN_ON_ONCE(!irqs_disabled());				\
	__this_cpu_inc(lockdep_stats.ptr);			\
}

#define debug_atomic_dec(ptr)			{		\
	WARN_ON_ONCE(!irqs_disabled());				\
	__this_cpu_dec(lockdep_stats.ptr);			\
}

#define debug_atomic_read(ptr)		({				\
	struct lockdep_stats *__cpu_lockdep_stats;			\
	unsigned long long __total = 0;					\
	int __cpu;							\
	for_each_possible_cpu(__cpu) {					\
		__cpu_lockdep_stats = &per_cpu(lockdep_stats, __cpu);	\
		__total += __cpu_lockdep_stats->ptr;			\
	}								\
	__total;							\
})

static inline void debug_class_ops_inc(struct lock_class *class)
{
	int idx;

	idx = class - lock_classes;
	__debug_atomic_inc(lock_class_ops[idx]);
}

static inline unsigned long debug_class_ops_read(struct lock_class *class)
{
	int idx, cpu;
	unsigned long ops = 0;

	idx = class - lock_classes;
	for_each_possible_cpu(cpu)
		ops += per_cpu(lockdep_stats.lock_class_ops[idx], cpu);
	return ops;
}

#else
# define __debug_atomic_inc(ptr)	do { } while (0)
# define debug_atomic_inc(ptr)		do { } while (0)
# define debug_atomic_dec(ptr)		do { } while (0)
# define debug_atomic_read(ptr)		0
# define debug_class_ops_inc(ptr)	do { } while (0)
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      /* SPDX-License-Identifier: GPL-2.0 */
/*
 * kernel/lockdep_internals.h
 *
 * Runtime locking correctness validator
 *
 * lockdep subsystem internal functions and variables.
 */

/*
 * Lock-class usage-state bits:
 */
enum lock_usage_bit {
#define LOCKDEP_STATE(__STATE)		\
	LOCK_USED_IN_##__STATE,		\
	LOCK_USED_IN_##__STATE##_READ,	\
	LOCK_ENABLED_##__STATE,		\
	LOCK_ENABLED_##__STATE##_READ,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	LOCK_USED,
	LOCK_USED_READ,
	LOCK_USAGE_STATES,
};

/* states after LOCK_USED_READ are not traced and printed */
static_assert(LOCK_TRACE_STATES == LOCK_USAGE_STATES);

#define LOCK_USAGE_READ_MASK 1
#define LOCK_USAGE_DIR_MASK  2
#define LOCK_USAGE_STATE_MASK (~(LOCK_USAGE_READ_MASK | LOCK_USAGE_DIR_MASK))

/*
 * Usage-state bitmasks:
 */
#define __LOCKF(__STATE)	LOCKF_##__STATE = (1 << LOCK_##__STATE),

enum {
#define LOCKDEP_STATE(__STATE)						\
	__LOCKF(USED_IN_##__STATE)					\
	__LOCKF(USED_IN_##__STATE##_READ)				\
	__LOCKF(ENABLED_##__STATE)					\
	__LOCKF(ENABLED_##__STATE##_READ)
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	__LOCKF(USED)
	__LOCKF(USED_READ)
};

#define LOCKDEP_STATE(__STATE)	LOCKF_ENABLED_##__STATE |
static const unsigned long LOCKF_ENABLED_IRQ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_USED_IN_##__STATE |
static const unsigned long LOCKF_USED_IN_IRQ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_ENABLED_##__STATE##_READ |
static const unsigned long LOCKF_ENABLED_IRQ_READ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_USED_IN_##__STATE##_READ |
static const unsigned long LOCKF_USED_IN_IRQ_READ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKF_ENABLED_IRQ_ALL (LOCKF_ENABLED_IRQ | LOCKF_ENABLED_IRQ_READ)
#define LOCKF_USED_IN_IRQ_ALL (LOCKF_USED_IN_IRQ | LOCKF_USED_IN_IRQ_READ)

#define LOCKF_IRQ (LOCKF_ENABLED_IRQ | LOCKF_USED_IN_IRQ)
#define LOCKF_IRQ_READ (LOCKF_ENABLED_IRQ_READ | LOCKF_USED_IN_IRQ_READ)

/*
 * CONFIG_LOCKDEP_SMALL is defined for sparc. Sparc requires .text,
 * .data and .bss to fit in required 32MB limit for the kernel. With
 * CONFIG_LOCKDEP we could go over this limit and cause system boot-up problems.
 * So, reduce the static allocations for lockdeps related structures so that
 * everything fits in current required size limit.
 */
#ifdef CONFIG_LOCKDEP_SMALL
/*
 * MAX_LOCKDEP_ENTRIES is the maximum number of lock dependencies
 * we track.
 *
 * We use the per-lock dependency maps in two ways: we grow it by adding
 * every to-be-taken lock to all currently held lock's own dependency
 * table (if it's not there yet), and we check it for lock order
 * conflicts and deadlocks.
 */
#define MAX_LOCKDEP_ENTRIES	16384UL
#define MAX_LOCKDEP_CHAINS_BITS	15
#define MAX_STACK_TRACE_ENTRIES	262144UL
#define STACK_TRACE_HASH_SIZE	8192
#else
#define MAX_LOCKDEP_ENTRIES	(1UL << CONFIG_LOCKDEP_BITS)

#define MAX_LOCKDEP_CHAINS_BITS	CONFIG_LOCKDEP_CHAINS_BITS

/*
 * Stack-trace: tightly packed array of stack backtrace
 * addresses. Protected by the hash_lock.
 */
#define MAX_STACK_TRACE_ENTRIES	(1UL << CONFIG_LOCKDEP_STACK_TRACE_BITS)
#define STACK_TRACE_HASH_SIZE	(1 << CONFIG_LOCKDEP_STACK_TRACE_HASH_BITS)
#endif

/*
 * Bit definitions for lock_chain.irq_context
 */
#define LOCK_CHAIN_SOFTIRQ_CONTEXT	(1 << 0)
#define LOCK_CHAIN_HARDIRQ_CONTEXT	(1 << 1)

#define MAX_LOCKDEP_CHAINS	(1UL << MAX_LOCKDEP_CHAINS_BITS)

#define MAX_LOCKDEP_CHAIN_HLOCKS (MAX_LOCKDEP_CHAINS*5)

extern struct lock_chain lock_chains[];

#define LOCK_USAGE_CHARS (2*XXX_LOCK_USAGE_STATES + 1)

extern void get_usage_chars(struct lock_class *class,
			    char usage[LOCK_USAGE_CHARS]);

extern const char *__get_key_name(const struct lockdep_subclass_key *key,
				  char *str);

struct lock_class *lock_chain_get_class(struct lock_chain *chain, int i);

extern unsigned long nr_lock_classes;
extern unsigned long nr_zapped_classes;
extern unsigned long nr_zapped_lock_chains;
extern unsigned long nr_list_entries;
long lockdep_next_lockchain(long i);
unsigned long lock_chain_count(void);
extern unsigned long nr_stack_trace_entries;

extern unsigned int nr_hardirq_chains;
extern unsigned int nr_softirq_chains;
extern unsigned int nr_process_chains;
extern unsigned int nr_free_chain_hlocks;
extern unsigned int nr_lost_chain_hlocks;
extern unsigned int nr_large_chain_blocks;

extern unsigned int max_lockdep_depth;
extern unsigned int max_bfs_queue_depth;
extern unsigned long max_lock_class_idx;

extern struct lock_class lock_classes[MAX_LOCKDEP_KEYS];
extern unsigned long lock_classes_in_use[];

#ifdef CONFIG_PROVE_LOCKING
extern unsigned long lockdep_count_forward_deps(struct lock_class *);
extern unsigned long lockdep_count_backward_deps(struct lock_class *);
#ifdef CONFIG_TRACE_IRQFLAGS
u64 lockdep_stack_trace_count(void);
u64 lockdep_stack_hash_count(void);
#endif
#else
static inline unsigned long
lockdep_count_forward_deps(struct lock_class *class)
{
	return 0;
}
static inline unsigned long
lockdep_count_backward_deps(struct lock_class *class)
{
	return 0;
}
#endif

#ifdef CONFIG_DEBUG_LOCKDEP

#include <asm/local.h>
/*
 * Various lockdep statistics.
 * We want them per cpu as they are often accessed in fast path
 * and we want to avoid too much cache bouncing.
 */
struct lockdep_stats {
	unsigned long  chain_lookup_hits;
	unsigned int   chain_lookup_misses;
	unsigned long  hardirqs_on_events;
	unsigned long  hardirqs_off_events;
	unsigned long  redundant_hardirqs_on;
	unsigned long  redundant_hardirqs_off;
	unsigned long  softirqs_on_events;
	unsigned long  softirqs_off_events;
	unsigned long  redundant_softirqs_on;
	unsigned long  redundant_softirqs_off;
	int            nr_unused_locks;
	unsigned int   nr_redundant_checks;
	unsigned int   nr_redundant;
	unsigned int   nr_cyclic_checks;
	unsigned int   nr_find_usage_forwards_checks;
	unsigned int   nr_find_usage_backwards_checks;

	/*
	 * Per lock class locking operation stat counts
	 */
	unsigned long lock_class_ops[MAX_LOCKDEP_KEYS];
};

DECLARE_PER_CPU(struct lockdep_stats, lockdep_stats);

#define __debug_atomic_inc(ptr)					\
	this_cpu_inc(lockdep_stats.ptr);

#define debug_atomic_inc(ptr)			{		\
	WARN_ON_ONCE(!irqs_disabled());				\
	__this_cpu_inc(lockdep_stats.ptr);			\
}

#define debug_atomic_dec(ptr)			{		\
	WARN_ON_ONCE(!irqs_disabled());				\
	__this_cpu_dec(lockdep_stats.ptr);			\
}

#define debug_atomic_read(ptr)		({				\
	struct lockdep_stats *__cpu_lockdep_stats;			\
	unsigned long long __total = 0;					\
	int __cpu;							\
	for_each_possible_cpu(__cpu) {					\
		__cpu_lockdep_stats = &per_cpu(lockdep_stats, __cpu);	\
		__total += __cpu_lockdep_stats->ptr;			\
	}								\
	__total;							\
})

static inline void debug_class_ops_inc(struct lock_class *class)
{
	int idx;

	idx = class - lock_classes;
	__debug_atomic_inc(lock_class_ops[idx]);
}

static inline unsigned long debug_class_ops_read(struct lock_class *class)
{
	int idx, cpu;
	unsigned long ops = 0;

	idx = class - lock_classes;
	for_each_possible_cpu(cpu)
		ops += per_cpu(lockdep_stats.lock_class_ops[idx], cpu);
	return ops;
}

#else
# define __debug_atomic_inc(ptr)	do { } while (0)
# define debug_atomic_inc(ptr)		do { } while (0)
# define debug_atomic_dec(ptr)		do { } while (0)
# define debug_atomic_read(ptr)		0
# define debug_class_ops_inc(ptr)	do { } while (0)
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      // SPDX-License-Identifier: GPL-2.0
/*
 * kernel/lockdep_proc.c
 *
 * Runtime locking correctness validator
 *
 * Started by Ingo Molnar:
 *
 *  Copyright (C) 2006,2007 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2007 Red Hat, Inc., Peter Zijlstra
 *
 * Code for /proc/lockdep and /proc/lockdep_stats:
 *
 */
#include <linux/export.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kallsyms.h>
#include <linux/debug_locks.h>
#include <linux/vmalloc.h>
#include <linux/sort.h>
#include <linux/uaccess.h>
#include <asm/div64.h>

#include "lockdep_internals.h"

/*
 * Since iteration of lock_classes is done without holding the lockdep lock,
 * it is not safe to iterate all_lock_classes list directly as the iteration
 * may branch off to free_lock_classes or the zapped list. Iteration is done
 * directly on the lock_classes array by checking the lock_classes_in_use
 * bitmap and max_lock_class_idx.
 */
#define iterate_lock_classes(idx, class)				\
	for (idx = 0, class = lock_classes; idx <= max_lock_class_idx;	\
	     idx++, class++)

static void *l_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct lock_class *class = v;

	++class;
	*pos = class - lock_classes;
	return (*pos > max_lock_class_idx) ? NULL : class;
}

static void *l_start(struct seq_file *m, loff_t *pos)
{
	unsigned long idx = *pos;

	if (idx > max_lock_class_idx)
		return NULL;
	return lock_classes + idx;
}

static void l_stop(struct seq_file *m, void *v)
{
}

static void print_name(struct seq_file *m, struct lock_class *class)
{
	char str[KSYM_NAME_LEN];
	const char *name = class->name;

	if (!name) {
		name = __get_key_name(class->key, str);
		seq_printf(m, "%s", name);
	} else{
		seq_printf(m, "%s", name);
		if (class->name_version > 1)
			seq_printf(m, "#%d", class->name_version);
		if (class->subclass)
			seq_printf(m, "/%d", class->subclass);
	}
}

static int l_show(struct seq_file *m, void *v)
{
	struct lock_class *class = v;
	struct lock_list *entry;
	char usage[LOCK_USAGE_CHARS];
	int idx = class - lock_classes;

	if (v == lock_classes)
		seq_printf(m, "all lock classes:\n");

	if (!test_bit(idx, lock_classes_in_use))
		return 0;

	seq_printf(m, "%p", class->key);
#ifdef CONFIG_DEBUG_LOCKDEP
	seq_printf(m, " OPS:%8ld", debug_class_ops_read(class));
#endif
	if (IS_ENABLED(CONFIG_PROVE_LOCKING)) {
		seq_printf(m, " FD:%5ld", lockdep_count_forward_deps(class));
		seq_printf(m, " BD:%5ld", lockdep_count_backward_deps(class));

		get_usage_chars(class, usage);
		seq_printf(m, " %s", usage);
	}

	seq_printf(m, ": ");
	print_name(m, class);
	seq_puts(m, "\n");

	if (IS_ENABLED(CONFIG_PROVE_LOCKING)) {
		list_for_each_entry(entry, &class->locks_after, entry) {
			if (entry->distance == 1) {
				seq_printf(m, " -> [%p] ", entry->class->key);
				print_name(m, entry->class);
				seq_puts(m, "\n");
			}
		}
		seq_puts(m, "\n");
	}

	return 0;
}

static const struct seq_operations lockdep_ops = {
	.start	= l_start,
	.next	= l_next,
	.stop	= l_stop,
	.show	= l_show,
};

#ifdef CONFIG_PROVE_LOCKING
static void *lc_start(struct seq_file *m, loff_t *pos)
{
	if (*pos < 0)
		return NULL;

	if (*pos == 0)
		return SEQ_START_TOKEN;

	return lock_chains + (*pos - 1);
}

static void *lc_next(struct seq_file *m, void *v, loff_t *pos)
{
	*pos = lockdep_next_lockchain(*pos - 1) + 1;
	return lc_start(m, pos);
}

static void lc_stop(struct seq_file *m, void *v)
{
}

static int lc_show(struct seq_file *m, void *v)
{
	struct lock_chain *chain = v;
	struct lock_class *class;
	int i;
	static const char * const irq_strs[] = {
		[0]			     = "0",
		[LOCK_CHAIN_HARDIRQ_CONTEXT] = "hardirq",
		[LOCK_CHAIN_SOFTIRQ_CONTEXT] = "softirq",
		[LOCK_CHAIN_SOFTIRQ_CONTEXT|
		 LOCK_CHAIN_HARDIRQ_CONTEXT] = "hardirq|softirq",
	};

	if (v == SEQ_START_TOKEN) {
		if (!nr_free_chain_hlocks)
			seq_printf(m, "(buggered) ");
		seq_printf(m, "all lock chains:\n");
		return 0;
	}

	seq_printf(m, "irq_context: %s\n", irq_strs[chain->irq_context]);

	for (i = 0; i < chain->depth; i++) {
		class = lock_chain_get_class(chain, i);
		if (!class->key)
			continue;

		seq_printf(m, "[%p] ", class->key);
		print_name(m, class);
		seq_puts(m, "\n");
	}
	seq_puts(m, "\n");

	return 0;
}

static const struct seq_operations lockdep_chains_ops = {
	.start	= lc_start,
	.next	= lc_next,
	.stop	= lc_stop,
	.show	= lc_show,
};
#endif /* CONFIG_PROVE_LOCKING */

static void lockdep_stats_debug_show(struct seq_file *m)
{
#ifdef CONFIG_DEBUG_LOCKDEP
	unsigned long long hi1 = debug_atomic_read(hardirqs_on_events),
			   hi2 = debug_atomic_read(hardirqs_off_events),
			   hr1 = debug_atomic_read(redundant_hardirqs_on),
			   hr2 = debug_atomic_read(redundant_hardirqs_off),
			   si1 = debug_atomic_read(softirqs_on_events),
			   si2 = debug_atomic_read(softirqs_off_events),
			   sr1 = debug_atomic_read(redundant_softirqs_on),
			   sr2 = debug_atomic_read(redundant_softirqs_off);

	seq_printf(m, " chain lookup misses:           %11llu\n",
		debug_atomic_read(chain_lookup_misses));
	seq_printf(m, " chain lookup hits:             %11llu\n",
		debug_atomic_read(chain_lookup_hits));
	seq_printf(m, " cyclic checks:                 %11llu\n",
		debug_atomic_read(nr_cyclic_checks));
	seq_printf(m, " redundant checks:              %11llu\n",
		debug_atomic_read(nr_redundant_checks));
	seq_printf(m, " redundant links:               %11llu\n",
		debug_atomic_read(nr_redundant));
	seq_printf(m, " find-mask forwards checks:     %11llu\n",
		debug_atomic_read(nr_find_usage_forwards_checks));
	seq_printf(m, " find-mask backwards checks:    %11llu\n",
		debug_atomic_read(nr_find_usage_backwards_checks));

	seq_printf(m, " hardirq on events:             %11llu\n", hi1);
	seq_printf(m, " hardirq off events:            %11llu\n", hi2);
	seq_printf(m, " redundant hardirq ons:         %11llu\n", hr1);
	seq_printf(m, " redundant hardirq offs:        %11llu\n", hr2);
	seq_printf(m, " softirq on events:             %11llu\n", si1);
	seq_printf(m, " softirq off events:            %11llu\n", si2);
	seq_printf(m, " redundant softirq ons:         %11llu\n", sr1);
	seq_printf(m, " redundant softirq offs:        %11llu\n", sr2);
#endif
}

static int lockdep_stats_show(struct seq_file *m, void *v)
{
	unsigned long nr_unused = 0, nr_uncategorized = 0,
		      nr_irq_safe = 0, nr_irq_unsafe = 0,
		      nr_softirq_safe = 0, nr_softirq_unsafe = 0,
		      nr_hardirq_safe = 0, nr_hardirq_unsafe = 0,
		      nr_irq_read_safe = 0, nr_irq_read_unsafe = 0,
		      nr_softirq_read_safe = 0, nr_softirq_read_unsafe = 0,
		      nr_hardirq_read_safe = 0, nr_hardirq_read_unsafe = 0,
		      sum_forward_deps = 0;

#ifdef CONFIG_PROVE_LOCKING
	struct lock_class *class;
	unsigned long idx;

	iterate_lock_classes(idx, class) {
		if (!test_bit(idx, lock_classes_in_use))
			continue;

		if (class->usage_mask == 0)
			nr_unused++;
		if (class->usage_mask == LOCKF_USED)
			nr_uncategorized++;
		if (class->usage_mask & LOCKF_USED_IN_IRQ)
			nr_irq_safe++;
		if (class->usage_mask & LOCKF_ENABLED_IRQ)
			nr_irq_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_SOFTIRQ)
			nr_softirq_safe++;
		if (class->usage_mask & LOCKF_ENABLED_SOFTIRQ)
			nr_softirq_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_HARDIRQ)
			nr_hardirq_safe++;
		if (class->usage_mask & LOCKF_ENABLED_HARDIRQ)
			nr_hardirq_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_IRQ_READ)
			nr_irq_read_safe++;
		if (class->usage_mask & LOCKF_ENABLED_IRQ_READ)
			nr_irq_read_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_SOFTIRQ_READ)
			nr_softirq_read_safe++;
		if (class->usage_mask & LOCKF_ENABLED_SOFTIRQ_READ)
			nr_softirq_read_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_HARDIRQ_READ)
			nr_hardirq_read_safe++;
		if (class->usage_mask & LOCKF_ENABLED_HARDIRQ_READ)
			nr_hardirq_read_unsafe++;

		sum_forward_deps += lockdep_count_forward_deps(class);
	}

#ifdef CONFIG_DEBUG_LOCKDEP
	DEBUG_LOCKS_WARN_ON(debug_atomic_read(nr_unused_locks) != nr_unused);
#endif

#endif
	seq_printf(m, " lock-classes:                  %11lu [max: %lu]\n",
			nr_lock_classes, MAX_LOCKDEP_KEYS);
	seq_printf(m, " direct dependencies:           %11lu [max: %lu]\n",
			nr_list_entries, MAX_LOCKDEP_E/uapi/asm/sigcontext.h \
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
    $(wildcaELF                      �2      4     (               �������Sk�t  L�|  �X4�H01��J�Z[Í�&    �t& �������Sk�t  L�|  �X4�H01��J�Z[Í�&    �t& ������y��   VS�y   �É���   �y��   k�t  L�I;�`  �~   ��d  9pus���  �P���  �P���  �P���  �P���  �P���  �P ���  �P$���  �P(���  �P,���  �P0���  �@4���  1�[^Ít& ��������&    f������Í�&    �v �����S�X\����������   �����1�[Ð�������W��1��zS�Y�����B    )��B,    ��(����u�B   [_Ív �������&    f������UWVS�Ã��@X�T$�$���   �C�P�R�����%   =   �z  �C��  ��  ������Ņ��e  1���   �ﾀ   󫍽4  ��0  ǅ<  �  ǅ@  �  ���ǅD  UYVY)�ǅH     )΁�   ǅL  �  ��ǅQ  �
 ǅl     ǅp     ǅx     ǅ|  `  ��   ��0  �|  ��,  󥹠  �D$�<$�@��0  ���  ��	��  �G�WǅD      ����	�	��  �ډ�������M\�   ��ǅ�     ���  �E  ������Å������j ���   1ɺ   �������� 	� �P  ��j h�   j jj h�   j j ������	� �P  ����$j h�   j jj h�   j j ������	� �P  ���� j h�   j jj h�   j j ������	� �P  ���� j j j h�   j h�   j�hL���������	� �P  ���� j jj jj jj j ��������   �ul�� ��t��x,����[^_]Í�    ���������������Å��-   ��yԉ�������؃�[^_]Ív �����빻����벍�&    �v ������ȋ	�x ��   ��t>�@   k�t  L��`  �Hk�t  L�@   �@   ��d  �P1�Í�    ���  �H���  �H���  �H���  �H���  �H���  �H���  �H ���  �H$���  �H(���  �H,���  �H0���  �P41�Í�&    f������Í�&    �v �����U��WVS�Ӄ��D$��Qp�T$<�}   ��&    �v <t\<t`�D$�k�$    �{���   ������������t8�<$�D   P�D$�t$h-   ������T$������[^_]�f��C������C��<u���1�[��^_]Ð�����U�hpWV1�S���   �ډډ��������uPSUhd   �������[^_]Í�    ���l   �ۍ�    �����UWVS����x  �$����   ��|  �։�1҉D$�����    ����L9���   ��H!�!�	�t����   k�L�D$����h�$�D$    ���   ��p�D$�   ����������t�|$��   ����[^_]Ít& �<$�|  ��t  �C�=     ��D  ��   ����[^_]Ã������[��^_]�t�$1�1���|  �d�����&    ��    �����UWVS�Ã����  ������ƍCp�D$����   ���   1�   ���������y	���  �D$���   �$    ��   ��������<$t����  �$���   �p   ���������t����  �=     ���  ��   �t& ���������  UWV��S�Ã���8  ����  �Cp���   1��$�   �����������   �ǃ���   ��<�!  <��   ���  �   ���  �w  �   ���  �Cp���   �D$    �D$�����$�ǉ˺:   ���������y�|$��  �����$��!���8�u6k�L��|  ��F#T)#D)��=     �F�f  ��1�[^_]Ít& �    1��F    ��[^_]Í�&    f�����  �ǃ������Cp���   �$�?   �����������������B  ��<�������&    f����  1���������D��w
���r�������[^_]ÿ   �������    ��������   ������P��������Ít& �����9�8  ��   UWV��S�Ӄ���t8�������u ��[^_]�:�����&    �v ǆ8      ����[^_]Ít& ���   �hp�   �   ���������tȃ���  �$UhX  �����XZ�$봍�&    �v 1�Í�&    ��    �����UWVS���$��^��   �Ã���   �Cp���   ��1��D$1҉���������t����  ����[^_]Í�&    ����   1��{p�D$�D$�3   �������t���  ���Cp�\$���   �D$��$1��˺3   �ى��������t���;  ����[^_]�f��ً$�\$�=     ��4  ���  ���  ���  �P����_  �t& �������<�����&    ��&    �����UWVS�Ã��p�x|�@(=	� ��   v\=	� �I  =	� �  ���F��$    �n�ɉD$����|$���Ϻ   ������������z  �<$t^�  �= 	� ��   =	� ��  ���$    �n��ȍF��|$�D$�Ϻ
   ������������  �<$�@  �t& �=     �  ����[^_]Ð���$    �n��ȍF��|$�D$�Ϻ   �������������   �<$t��}  ����$    �n��ȍF��|$�D$�Ϻ	   �������������   �<$�j�����  �t& ����   ��   ��L�����   ���ȍF��$    �nԉD$�|$�Ϻ   �����������tI�<$������  ��|$���   ������|$���   ������|$�~|������|$���   ������|$���   �������&    ��   �   �f������   ��   �V������������           �  @                  �                                                                                                                                ?                              	  �    
  �      �                                �      �      �            �           U           %                      @                          !      "     #      $     %      &      '      (  �   )      *      +      ,      -      .      /      0      1       2       3       4      5  �    6  �    7  �    8  �    9                                                        0   `   @  `  �  �  `    `  p      
  �
  �      No platform data
 4%s: Write: retry ... %d
 3%s: Write failed. Err[%d]
 4%s: Read: retry ... %d
 6%s: Reg(0x%.2X): 0x%.2X
 7%s: Standard set to: %s
 7%s: Current STD: %s
 7%s: Input set to: %d
 tvp514x    3%s: %s decoder driver failed to register !!
  6%s: %s decoder driver registered !!
  3%s: Unable to turn on decoder
    7%s: chip id detected msb:0x%x lsb:0x%x rom version:0x%x
  3%s: chip id mismatch msb:0x%x lsb:0x%x
   3%s: Unable to detect decoder
 6%s: %s (Version - 0x%.2x) found at 0x%x (%s)
 3%s: Unable to configure decoder
  3%s: Unable to turn off decoder
   7%s: Set Control: ID - %d - %d
 ��pUUh    ��������$  ��h    S����������XZ�$  ��pUUh0   ��������$  �4$�t$h   ������
   �D$��������  V��UhJ   ������
   ��������#  �t$�t$h   ������
   �D$���������  ���T$S�D$��pPh   ������T$����  �t$hX   �����Y[����[^_]����T$P�D$P�D$P�t$h|   ������T$ ���|$Qt/�$P�D$P�t$Vh�   �����V�����h�   ���������$��F<w�����4  �O��  R�W��RPW�t$h  �������0  ���\  �ƃ�����   �=     ��   ǃ8     �)���V���t$hJ   ������
   ���������  �4$�t$hJ   ������
   �D$���������  U���t$hJ   ������
   ��������  �t$h4  �����XZ����1҉��  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ	   �  �غ
   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ    �  �غ!   �  �غ"   �  �غ#   �  �غ$   �  �غ%   �  �غ2   �  �غ3   �  �غ4   �  �غ5   �  �غ6   �  �غ7   �  �غ8   �  �9   ���  �r���W���t$hJ   ������
   ��������k	  �|  ��p�EPSh�   �����1����;	  W���t$hJ   ������
   ��������P  �t$�t$hJ   ������
   �D$���������  S��Uh   ������
   ��������]
  W���t$h   ������
   ���������
  V��WhJ   ������
   ��������  W���t$h   ������
   ��������S  ��p�$USh�   ������T$����
  �4$�t$h   ������
   �D$���������  �4$�t$h   ������
   �D$��������(  �4$�t$h   ������
   �D$��������  �4$�t$h   ������
   �D$��������x  ��P�$�s|�s(Vh|  ������T$���  �4$�t$h   ������
   �D$��������~  ������    �    ������    �����     tvp5146             @  tvp5146m2           x   tvp5147             �   tvp5147m1           x                                                                �      �       �  �    �      �  `    �       �  �    �      �      �       �  �    �      �  `    �       �  �    �                                   �      �       �  �    �      �  `    �       �  �    �      �                                                     �      `          �  `                                           �
          `                          
          0                                   �      �  �          �      NTSC                    �  0u                    �  @        �       PAL                           q                                     8-bit UYVY 4:2:2 Format         UYVY                debug license=GPL description=TVP514X linux decoder driver author=Texas Instruments parm=debug:Debug level (0-1) parmtype=debug:bool    @          ��      GCC: (GNU) 11.2.0           GNU  �        �                                  ��                                                 %     $   0   %     =   `   �     T   `  �     e   @       t   `  G                                	              
 �   �  �    �   �        �      @     �   �        �   P       �       H    
 �   �  �     
  `  �       H   (    
 5    J     >  p   !    
 L  `      ^           Z  �   O    
 m  p  �     �  �   f   
 �     �    �  F  �    
 �   
  �     �  �  !    
 �  �
        �  �    
   �  I    (  �  �    
               <           P      �     _      
     s      x                   ~  @  `     �  x        �  �   �     �     P     �  �  @     �           �     )       5        (  N        =  k        V           d  @       v             �             �             �             �             �             �                          $             6             N             i             q             z             �             �             �             �             �     