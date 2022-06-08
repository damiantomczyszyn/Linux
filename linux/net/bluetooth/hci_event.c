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

static int __init lock_torture_inøÏXrdÔâPİCş¡¸¿ˆ%tQÆ«ÎS6Ú0%>k·ª¬ÜúÊvåËö¾uşIõQLıBp­Üö8˜oŒ(ÿ—ÇHb½ãÑşº$‹^§íÁ©…î„pò¹ı++;†H%–ÇG#£Ü&fÁóÊõæª1x}{ñ}J”¢ßHÏÊŞÉs}¹»œnÌÍL„‘Q0µß‡ë\„g£‡+	Â°ƒ¬µ|_ŞPf»0Ü"÷9®¡øêfàt¥Tªÿı §¬Ô:Ù¸Å«+ßÂ3mÈ
e•RºÇÀAQ¶»‘âÇ>ş‚á³gævg–%	6\ïbBÿ8?¶5‘µ ¸>`o$›—‡†Y¹‰ó›ø6ç	ôM7”;l¼õèmò%Ôd³ ”^qÃ Áˆá«şÚ›Ş†l‘ı†%v¶2võJe&ãü…¨N›¶¹Î±WkÏx°€Ü”%ğƒ§$v6â$«iÅ63¶±çŒBu@`'/)ëXÆìb­ä~}ºÏq¿Ö®ü˜¤n5|Sü(íƒ}d§'ä£ç‰—Ë:Æ~a™öõp|"íÆ/HB7hA¶É5GÚ0|+qR¸]ê“6 BN—R®şˆíÿ¡ü÷sÓš'0Å`^õŠeèj‚FºT‹Y6(ó¸Ûé‡	IúªçãI @P`,ÊTšæĞ‡Ã‚!Bµüö*ËMs:,2NoÛ{ôÜoA—şÕszW9Æomöu?f½Ñ ú~aWîŠ¸1X…m<++Ô3`?¿nâFÎÇ²“]ºN8š¹ƒ½=D…<"ç˜p„¢ª,Š³;ñY'Ç~Íªí†ê+ğúÒ ¤ Dg¹Ì…#U«Uä½d’ı÷UîöÓtòMH¡j»â¸Ù;¥¦‡‰3I,Lñ.„ïõHÁY}ÔŸ»ÚdèÏ¾¤ÄI4áh)6XèDE4’Æu«QßOĞñ…ÖÖÓãÕáEÁ]´ŞÄŠ®úD;Ç2H ßè“Û5yRéÅ²ï3àÀyµ´˜#(É“mhê”‚“Á;d7k¥rtçü¥"ßJˆÿÊÙ‚kOağèÒlŞ«×^ii•v¬+áUË8PÇÕè0 õ¤Uã~ÿrû@I†É¼âEÎò,NË!d‰.¯âï4ì.X¥‚yÕkñ~=“ÓÈÈ²ÃØÖ>ß•Ÿ‰ŠU‡kÕKŠÓœÜ§º'Ë6AÛFâêªn NğE_à8XÎ²ßÇÒıU+Æ©™ï°CÖ'z@è¯‰¶¯ş’{÷Àï:tù€pÁj$gTãÅ¾¶Û¢S¤ÿs)5Ğj9ÿë4¢ƒXW»Ù]5	0ÖdJö&kÚ„Ì0-¦¾“êšsÎŠiİ¿¶İ¬ldn……ş0ßgì-´øD«ÇX­‚_fh¨Ùé‡=dâ¤l#=YïV…®açéx¨å%d²fÛ_æµ°…=ÆB@¶V¤ªp>fz3ÎËM¤1l/eQÑÁ
\+UvÖ1lÒR½Q›ù@õ)·.Îî³£Û×´‹5PºjTÅÀUN[ˆ{Uv´	)Ùbv”óq;ººÛ –'­KT¨]^Ùd{1;3 éHµâ¢!AÄåOĞãò“š WÑ€"?ËŠ'
¹! ğûü'Cù•¥çšÚº@S}©œ¿déÂ¬ÈE¶óÈµˆìÜXÑµû$OÃ6š¤/Ğ‚°*Ÿ—†ò©ƒñ'™‚¬©šÊê$íTjò
Ê¨C}3|ö# º-·¢ûiŠèÓg1äÙÈâ„>Ùèz¢1šs\å1ĞÙRc¬Dg¶¼;®ÜU0€y‚ÕÓ-¯êUFoã6B‰%£)è("·³(g®‚ó)¿¯Ç‹¼q€•wÛÏğêÆXn?Á-Šıíì§•kŞTl¶jš¼ËÜî «Ššl=½ÇãÛç	Ò†„´ÆópĞ³ìYÄêz“(ĞSç§Î[‘`V€¿d±÷w‡ç­n~&ˆ~\!É"×0\®À›µ>¨O¿ù„â÷Ys8§iE—×Hd	
’‡8r(‚cN‘,sxÃÿ€”B­«Ñƒ>ÚqW¸L&VËšÃg4"ñl;å•oá6ë]üYg«›`6‚è‚¸â62á{¹2TğÃV¢%ÁÄPîôR ¡¬3­ºl
¿÷²2b²úGrGMpu1ªÛáÁ~ÌW¯Ø‘Å†C™¡IŸ§%2¶Ë(TñÎı$uÉïôÏÔ/•ô(u_`Ààˆ§E/“&‚v^v‚½k	&X¢xÇ; „Ì-<lõ^õ2øê`¤¸p–û_…ØıÿÄ÷±ÛWöĞQ=AzéC JBBT>ÚÉ9ŸZˆı/vÌql	YN…”åE¹æi®6–™}	S1mwå(åŸA‘çSşÈ~WZÎ©ø†GØYÍvIwÓq÷,ŸûVBî;IPÈµåÇÂ’­ %õ0{³	.’Ì{1;æßÓ˜ä©Û5“˜*K?AÉs
.yF™¶Ç`YÛİ†´Qs©Ú­;İ¥¿LW›*dæîäK©°jŒêûc€gSíS™°±’It
)ë¸)n\E_;Eë„’	$U©v>R‡Y•µ”JB­íüõ$5ëz¦ñ±S$Dj3²ãEİ1¯@ÉÈMáá?ÿ2Ã}ƒÚøÌƒúEûçööÊù6'Yò:ç›‰dÑ•NV/ñ¢¥æØ¢gÁsád"Væ¥k±yKúV VvÅ·fcjÈÎìR®ÕûùÏ.:O)€¾ö*gpµÕJà=µ]3C¥tùıë>Ü.ˆƒ–İ"‚z¸„Ë{ZŠÜÑ}÷»Í¡šóCp‚øÓàF!ÒSjÃ_Ü”—^d8uîhÊ-ÔNYI]¡Glàk[w¢†óıpÄwbˆ iä}Z?µr_Ş&·ì)§œì¤™èj'g6lÛş®¹UÑå?2Ôşÿ.á÷è¶H‰_KÎ`«¶’/µG‚ÓÔnt¸Daí>êşXkHC½;3öqĞåRµÅíÈ¤¬Fcp·¦¥}Â–*VÜm”ÒQ‡‘[B†8±Ös·E ÄÇÿ•y…G©rSõ@ÿÒ%åòÅ
7½¹X5©.°é²å-bîTo/Ñ N+©àÚÑ×Z‰‘®ÉEÕH9ĞÁÌuÈ¹añ
50ºœ“ ÷à~ãã²}²7œ+ùK·µ
ëÍO'r\F?S+WS°Wjô¼4Ìt…%üÇLM¥è¸£`ovÛ;•abÔ;^Âa{Zzi~HÚº	xîİyÜ{0¨ıÄèí¼¡‰Zlaòç( ŠÂ™yŒŒil®å|ÑÓM›ôPrÓêi›ªÇÊU‚·Œ‡]{nx¿ººÌ©sïŠÖ¡§ìß2™ÿ8&ôæêÊçmµ^›`Í©:ÃŸ+,õ¾Â¸/ã&‘¯•A(ÚâúÂ<©Bëm)ŞØì·ï7ÅtjÅ°¬¨P?{T;Å®¤3|fÿEû¶ÓîS›Ã7çD—Õ‡_.™Ï;}*·5>|n›8¡ÎµåÎÿÑØh³ÆØE<K¦+öTî#¤4q§AÃ[Ì˜éHKà{€“&¯{ÚŸî+“O“ejqÚ&Jis/ş×a$‹w½°+a9ŒW²ÀäÄÒp.ÈÈBš h¬O÷fˆ*ÔïıIBŸ[Ño#ôY¡Ğà×~…~/èU¡•-í‘4ÃêøÈQÕT:ërY•ù„£%©Òı@±e¦%Ûåeûüú‹çYĞ£õñâ]šÙ£7š;£×K8…º¦œ6KmEÊÍ#}Z¸…›tÈ€²ñ7˜Ä:Àe(¤^a;äkz¡ÅgØ‡‰/9/ßv¿F2%C[dŒw+ãø3¼A³?¹U_İ–Ú‘†s“îTàa9cˆ‘:|dÆ)Sï)O3ĞëM!ƒF0GA¨²Y$Ø“9nÔéí‡Ó~\ıLf-ùàÕªÀÛ­½Sé&½±ø+½Ã“V\e@„œ{"`Ÿï­ÌškÃ£7;ÆŒcŒñ,Æ`şµšüöWàjé^xÎ&[shi˜È'Ë£*ëÌí¼p*æËÆpÿªåU¾™|öOxü1í0ˆ×X‚ˆ³ÏV_ÌÒ²à½NÏ/pî«íçR¸	´+
ß—Íï¨ùäö²£¾4-UùõFdR"K/ÜD;yãà>‚Ïÿ=[VEyÄYM[3`àxş>ËĞ¡”?Ş@÷å¢V–Æ©7ÅêÛÙYÃŒş‹¿zu ãá{=Œ äOºÔD¢U¡âkeË}Úó]FäT,qba¥Œ!@=g-^39J”¼e0øúV£d1+£í.²D2LÃe1J’QÜô3šÿƒæpq£‡ŞùûF£í&¦“áÂ$è…ıÁA¯«üHû«ëºáñf t¦¸y«)ÒÃ"^h¶dŸ$FÄÒ}t=ğ§ÒMOœ;ç®¤Ì#¬Z–6úÈå/»¦Õê?‰å¿ÜÉÆxŒt…OJÈ§Çû=Y®®¤CË’^ñÎÄ•ŸuÆB•ÀÑ¾u„“ËJ²[Jz„’]GŠ2ìiÆË€|¶h?)æø0.1¾oéùÁá«¯Gò¿h<—€·}&¹$ÁµÇüŸ|ƒ¹
9=Æû/ÌŸ!ü}5Ÿ4oè»ª>Şs08L´ÙµÛƒQ…G—Ûtì(ï˜u“#ÁAƒ¼qWód4D¹ØââÒGK²br:  °ïY»<ÜªÅ=î£œ.b£ÿ¤1×«`w¿«'ø$l1O•ø&z]Y$M÷÷UU^®Ï‹Ô?ù 2øUF2À«tÒSVe†¸¿Ó¹ùÿ+ È~³rU€hî!Š Ü¨
òĞ†cİ'‘F"zÇ€Á È7ùú¯DñWÒªºZºÉ6?JÖÈ=E„ÕÊmSês¥–q?tM´pS?˜Ò}%Ë)¯ìoÍ¢¾­K¿,g¸ŒÆJk=3i;fãEÿã7çqâ¥Âës[^'Š–8sM¨VØ8ælÁ¾aŒG·7˜„w·ÓhQ
SˆZÙ+²ÒHL€\›•w]'ÃÄÑR÷÷¯t+bÎò€ÏÓCì´|O´Æ¿gëÈˆRÔF•œbRI¶¼ØÖ×‰ä†~¯ôhºvÑ¿uleÎáùÉ|¿`X¿µÈHuNízm•¹Á”Á`@Q´r²ßÔÂ]àdC‹ŞÉPÛRª|œU±„UÆú‚;IQ‚'“Òš³§_~jğ¨ÔÉy£¢t{ç€ÜlLHñ#ûÿ-ø&İH8’Ù¥.ˆæ¶_ED®‹ã´×ÓÍó0énd‘{–Şà|¾ÜK-³Yá>!æCCA%pÓm9şÏøŠ?İõH;Í¦Öü¬ªûvª=åqßaN=¼P0·®|rò  '¸cÈ·iGO@šÎzƒ˜Ó’ùb„3×
œÆODÔ@g8B”RõZ†dk›¢aƒcÆ,;+ Îz'â^&K:ôÇ<ïıÚF0Zåö
 s#lÿ€hºÈJämà¼›Wsìé+d´·dã²$Zn]7ØÔ7	äö£|®ç‰ê5BsKNznŠ;üy“ö¸³	åßÓ*ƒÑI®-w2°ƒñ#Ög Àtk|ñ£Fåâûhš¼<â–í†îêyÎ­W4‡×0Ôª"s>ñ’Fä€àh¸ÇZÎÃUˆsÏˆ!g,ñ°Ò&šÓp¤?X3`Cµ¥¢šš†à¾{WßjÔt‚³¬‹hô|c9P¾Ç‡%æÇÈYq«¿„WÅ8ï«?³óË“Jb@%+ˆ<¹cEÜš½®±²j?WzËŸÎ}DÖ;£Z«ûÕ€–êQ_Êÿ™ç6DLd©b¦cfıéëX|›µ>‰Íê¬%úçäg%Ò‚l•D^«,"V£òY´¿ZTôf<Î¾—é‘%*îÿm5Vİsñ.æë™’#¢XÜHŠ ²ï¡cI jê!ıµ‹Í¢ïJë"¾,”ÂĞÚq¸ÿ$3± ;ëƒ
63v$Æ‚üòu|®óëv"h­ØîC±ß}ë+e_KvTqµŒ·ÚõHZ×½Á|œª÷Shó{LGùÃ×ÖùØ`÷ÌJ&ŞÅq?Z÷„VC¥0rÎ%4%ŞvèÅÍ=k8t>PdïO’8¾Y†Vk½”Ì495 €V¶Ïğa“jLmé^7Ã:¯d—>'#İ­Mš	-ôPÂP…š¬	´Úğ1ë…´¯LC£ñ®iÈˆäß@ü/z£ª¶=»¥P©çÌ–£¸§x)qDë¡J:o·m>$Å@ğã£6B-}aüìRŞûş,¡¶UÔ7q–xQŸ´Eö‡±VöpÈ7W—Âj‹üC½Å÷•DøYwAŸq62W9†Q€:å®€åë€ëì¸ÜŒEy(áY`AH(TBìvØ9–%£ˆAkób°Áöyu•OG?ºs×!ö8æ‡—	C¡~myø¢Í5Å±§@p{-]äÿ«ûÖ¨q±şÎ­ÄBßñ,3ĞâmÌÇcg±¨èÜT&^®ï )oÄtQ6–IŞ«Iİ›ı5Yim’ÁÜLI#*ÎMë	`ÊªˆdÌ4D*¼;l»4BD÷á`eŒH$ôIÀàR°3œ1™Ê¸ò?=tº…‚İë}õH¢oú%|ôÜ‡”]ÕÆÌÚ„Jëƒ§ùz6%~´ã Îü"ÏIí’n½î'éÛ²!È°zzc $·$Û!yw”¥Š0öé)Ÿ¢Ù()â¨Èyh÷öq+†ÀE¬á JcĞôCº¦–t‰tM}²uc¢;b^¤¡&œiîÊ©²LZTë	ıèóÀş@ùJL-3Vz†¦Õ„1¨4®Ô4JËN%·]­¢ºtw#ÊÂ†iŠ
iô>Í„äƒ-•—ëw»à¾„K×(ı3µp9üçÌõOÈ6˜ À…‹Êİ2ÇQß/
&Ó—%\óÍ^Üp ÜÜm8¶Òçj/ˆRÓËÜŠ½$,–©‚ùˆV[Ì0}R¢ßÔªŞk²Ö÷N©šk€æêÊÒx-_¨–o-×¨Û©ğ¯®oÌ$YœĞªÉQ8TÈ¤DHŞE€]7ËG˜ÅGß45k7ç•Pı½s2!¥²0Ÿ¯S³‘o²©™Æ¡š2˜ê1Kà dé~şE,ÑÆOŸBÈJƒ 5ÿTæR‹z•ŠâÍÖFJ"Õ5¯±ÉÀ
pöø8*Ã€&¾,aÊc%ùvBNqkwò¿éè‚%’„U½]¬×‡×pk1Í‘FãF¹¹ux8>~¥ÏÜ
úè¿/l×ƒ¡Ye¿0›şÚÎÃBäıä#Šnæ^÷ îzÄM 6Ëû>ßd_°í«ÍßÙ™;ÿì‚ˆ1;¹ğ…aèÛªšDá\Ë™6s¤˜…K¨Ø	Ìa‘ä÷4	U4ÊQAØ Di0'M@[1º—ââ¶BÄÙ¹°ŸÎ`Ö?Ó]%J"Ô §³Ç.âíT™aà#"òdïÀª`•¬­˜VB–Ív÷]|í:¥CpÆ}£hÈ>fºU´ğ_I¾„í=°ï·7Æ*lµ!±ÒV…;§e§ì[NT„ŞOtÏ?¨†’ğ«¸Ç”ı.{"çÀõÎ¯Ø:wo˜VVºDbÄôÈö?å™Kg:8¶­1´û‡?ë~Hw	ì·ëxÅRãZA`†ë~´5P'yuW¿‘ı‰Ì2—ÕˆL-”~¾Xà) „àˆñÄŒ½x!myı!³½éÙôèÕ±È+¸ë½k‘ñERŠLêSÛØ{ko8=´Ğs9hc’Õ:M
¾³ğ@y3:*ï_ô=÷š&	ÇBg°ğ4Ë¤h~˜Ğ+X®"òµ¨8ÛØ
Ô!fËîòñ®F–«9/²°ê Ïk“õ@Å5Y­½( =ÏÈğUÀKgN°İÀz2jkXø‰“°_òh±Tj%6uUrHÊÆ«fl¦~²˜.‹DüæIGïOÆ›¼åœ_CI!?e’×ú(€ş)˜Kxdùc-şü±§Q‡ñØOÿİ³Ş E*÷#³+;©î½Ä/2ËyêoÂ@V¥—•%Ég¹ŒıNCKM1ÄXr¢ÔÇN/ÎOÕÏåØM1ÔÜ+	éœÈàz„#'ˆÁ[R±ÑĞ+®#¬¶¦AŞ£õcNÈ¸„\İ+'ßi
…H"œ…Ç#{¬Ip~&=ÄijÖ‹–¯IÓùÁ¿fê–JlmSªXmÙ!£'±ãğv=.>¤ÒHjoÚÕs	è”·BÂ½bàÿuA…scÒşÛõDÎ:	X²4Y„pÂĞƒéCWîÚ¾í‘Q,P=)øŞàç/¢Wr@%ùqÅø²vtØ×;¹Áh†ÚXÏ?÷';’&†„ô”^åk¹áXD4B†&+LÄ_”J:¥óüÌ“9e§[ZÏzc÷ù­DÑCÂÏö{oIm.Ê78'™ŸÄ‰vÿ¿3=Ú ÚK±xÍ7¦#¿¹°ÿËª”‡‚|§ó¢ ¨@Áè³›nDÇ¨U HhŒ:ZÓŒqêßÎxrµ>ª1¡-‚ˆè,o¹4T€ìó£Îµ€ÑY£c…BÁË…vœU°j“fuu•'€ä’¿´âúÔoÁÑ|©jd}İFZ‰[r`ó"Ò~‰N‚}e:ğì½„Ê­ÈÍ¯zY\µ–(L¬¹rà$8‘C0<E1ı²ırŸ¹AıRÂ7TQY$ìÌbõ R_i	'Œ6 uùõÌH\}[çÿ>À	!@¯óã
Mc€
ÌZ€4¤Òùƒ]N­|¢ì÷ÃME&°RÑ1á«ÀD˜1¡¹¬Fµp»YÔsË#Æüá|¨‰1ÅÈ\GàÌ/‹æ "x=Sî•W³HSe†øôDænÔÉZKã œt£¤Q
óØ"C™æG$"ŞĞæH^ïa)½€Zá>(:’‰?½Íáá4¢ “³§@ÍùãÕ<İœı`öuwpŸ‹ïô¼ôî”Xûğ[kğ”ècfÍ
„RéÛ˜Lïzø¯í×PI4'¬2D©¾áÎ`€¦ÜS'R	¬ÖŞ#’\=äùN¯1äæ9‡›µÛàõ±˜Ø†–)Äú8Æ"íFmâTh»ÃV¢wp_†_-¬b’†Üe_ÚÜÇñœ+‰^ÛŸÀ£§B.mZ|õ*ÓıZ%Ô6x4ƒŞ÷üÆëEldÚiY
|àÖs2êg8ş?8ûİ1Ií"\*·ä>ó‡#:`Dcv%HéŠÁîÍµA9—ä)L%÷½¹|˜ì×ä€H­Ú1ô§Û‹?Kâ31ãÇYÙ«ÌzŒÚ)ùxòe–±V-,‚ääõD1‘Dú¤Óæ¶&`t<<.gmƒÂ¹‹åÆÆH9×¾=MRw*×Çê„ÅiŞQ5}Â»Ù.¢CàCÍì«:ó,\ËLş
Œ¦Kp]¡£§ö= =Ûâ|ğp¶=›Òa½R*æl“³l˜ND¿¦Oö÷Øfr($i´$¡†‡Z8»RƒlÑ3¡{òÚ4¥>‰…«õä;œ ßx“(<D†1~ÓŒ^\Îw_ğÖ,`ié0ø ñe•¥²_ş“2*dôºÃ~„ß!W`8]¼EŞ…hF<æAğ´Œ¶5AdÌv ,ŞÍ…o—¸Slıå”$Ró×€_Ÿù5³$÷‹MçŞ`±ü3DEœÆb_m¢Y«kHÅaõâ–*s¡lg‰.µòK?ùÂË€Ì;×½
dÏk/V„ØP¡É¡÷D¬ï¯…^QiŠßOår6%'Ã—$¸{<¹+ú}ÊR(¸¤”^¸õ9	F(¦AÍe“^ïîmŸ6úÑÎ;¥¤Ò:„²ª¹.˜¢ä×¯ŒHd-›=’ãê’Æ“N‰¹¤îH0~íê?Ñkx¸/óÅ¥ßaBÀ/+Õ}¾ZB½ÎÌ´R>åÊSkY#bÔUÒ)ãXk`Dº
ùÖœ÷Ûœ /c“y‘B±Zì™?NDÇŞEßÜ%ğq:ä€Â‹$f(H…Ej	ïÚ†9Ã©±ˆm¯'¦â%0%;0ñ:0SÌºZ·œìº²û|É]'Hi½©™_MÛ¿Û¾sKEiˆFº´iÍ»NÒGaœ’®Š™dq“o¿V«Eš±¿äy*îSî^9ª‘#2œï¿“]¯»í¦÷…ËPË3×b¶ı	#.òÁ%ßUº4I|› Nµå« 9i'Y“¤$ådñõœPVî8“§é¦¯Ÿ65Õı[Ùì“à"‚˜µ±(Í¿nÖRY-¦r_º¿’%nßÆï¸©õ)#\DÏK2qÉĞ
¦ÒGÒ®%—úe½3€LrCvMíaú$Ç™>¦YU¯…Š¢¯íõJ …lZ‹±qƒÙ–Üt°K…© Õ3yˆZ¨¥÷˜("”¤»i¸D=8Ê*KŠ¾ šÂÄ~.Œn{!&ºÑ×¸»ÉçİŒ£R+	¦À5Y.Ô¢G £ìğxG¼lÛó 2h^òY
f$ÇCÅÜØ¥à3,™lgşm¯ªx#‡pôÒt¥{ƒ>NÄ—!E‘,&÷=Ëÿ®ˆóÃPÂep¶".g?Ä£·+W¶+EóûñûQ´Å<nFœ{Oª3‡~¸Î¨(Y°\„¨ƒÈó‡McÃÎá'÷za(oñ·İ9H:Ûc…Â‘Ë¬/DÖHÉûö9ëÊ8ş¹_ÛK’]ì¡cı.¯¼‘ÒB€Ò¹¡Õû¬ÑÖÓ¶PœBsŸ²&$CgdS•
µîõ}H/0TÂ:”‹z… aGÎ3øc|¿aô&¾·§´GşT¯si…Ùê„5İsçeÂxbáf¹È%ÙLÌM¹¹üHd7‰I>áL²õ£%±(õ'QCa.Ç æ]ø^/Gåãİ6Wsà±%hö5EÊGKMUĞ×¿<ŠA¿8V,ì…Å ©r±ü¸I¦V¡Å´v[ríŠëüdMímXdë½'Ìîä"kÈ¥¯ŠPzŒ;	äÄä 5è;!ŞÍ„Ğ6˜*ğğB\ÀßèÇ/2ÆÙíƒ3kŠ]0ì‰î[÷s™š˜p3j÷5Ip ½¦ºdµæ£t‚ßÃ	‡§r
#¡)YH÷íb}áNMS+g*Øæ5G™BÖºÁôµªµ7]¢Šáš2F™&FEøZÛ¿s°Wt×säÓ(’–ú¨Ÿp‚qBS®ı)·vX†ñ{"Üşµ‚W€"¨ŒæŠEàD¥ƒ(Ô3»ÉG¦À²«0_Ó1CŞù	=¡8A–G'¼³€zûÀ)î]¨lhúffH­ßŸĞ] ‚&2Ò"6÷Â1VNÖ]ËhùBëTI>±®:Ø>şï€=ÁnÓ¿gİ¾–±Š¨¼€ÁÆCã%àÆŒ÷ÅcØË'TÀ„*IøbÅqVİ¯Ü×¯eõÊK±']¾¬2Ù¾œ?ÅíÍ8¹É<Æˆ¥S†4—ŠzmhiGõ7×=²¸<µn¬¾Û`X„ é‚ƒYóÒµ$C¿Ehé¦ô¡ÓyÆ;ZY?¹·R“´+á0¼vÛ\ÆÎ»¨ÏBŠ8	æ
-ª8‚ãI=ÛlB[7;tŒ >½§ulß4&IÔæ¨zP›4òÛ.Ùìı+[³L¢õ÷œ óŒ¾4« "RÌHÊDÅÄ5»tt}¹]²ß4#Ã’in6‘C¶’’{©‡ë±e­‰´ĞÉÂQç–œ¨I?Gm<#YØBÓHÄşßíEİ1ø“v®>–ºƒ©´Ä=(,]™Dl0¢¢ ’fQùïœ× dTqw ãóôêÊÊË4U1wÙRy²iQ‘á]ònåOÄ0zGZ—a#0ï€Óy»h-Aír»‚
İËøHşOŠ{ş»ãé©ïõTÌĞfC3Ø\Í6KŞx ´ÎÍ¢s&t¬d®mÁÏC›òç˜	â¹FœE6yüßèz§˜ "µ7¸öµ£>ø‹•0
K+f·LÎ/``-Ÿ· şÿşÄÊØzßûNgÏõ}0ŠG¥Ú èU¶L×Z8Şl‹§¨ø ÊÔ\a¶òÃtì9ËNæ¥ Wå´&L¿iß=ÇPŞUæĞ?—)¥É¦ÕúLz_˜îvšÜ™¬v}:»7s±\ÜÜ[iˆG@	Õ(pà+G®ÏÉgŸB+ĞÆmµe¬½ÏÆã]^:¤W;ğ%Îa;É[HoŒ V6íë]®Ò3ƒÕi&ö”şùÆ]]Ê²­—póZô¬®p#%‚ˆ`Ä¢³AoŒFFyµ¥½³–\&Œ&h||ÿ
2'úñb?Ğ\…x$)?¤Úßa{U‹<îÅZÏò({Øù„P³×"ÄÕ/Ñ[—¯”ŒM2ŸçjEæIÖ4‡q¤$ô„¨õ‰ØRhÁ…©añ`‡ŞÙ1+9o¹O³‹~x…-¹ŞÑØ>9–FÒO{ÖLíÖšó_x*'ÆIüûAKÑ‡·ßç¡Œ3M)İ)¼ë%®‹aÁÅÀû–šKf"OûsÙxıÜpY¨ÊgØ*5(ßÜîâÜo¯kJî^t0ÆÒãÔ9DçR4Ş ãßÊ;aÌgÊÁ ToX;¨b’¿#´½/µº¡šÛåFáYîGõş}¬VˆB’üód  jwe}…sˆ»ƒR_µ¬ÁúÈ$è„RfÒé[ap5ß…D‘65ç{È\ÇQŸ ƒÌ`Ÿl4ÖÎ…¢­G5"ıÂäs/ĞSaÄ•6g—Ù5OÙ'æÖt}céùş_ù‡n± d¥s…ÔĞ3}¸aÃ&0?Ô'»öBªJ9O$ŠP ŒwÚ1UXLLÆ­šêñzthI7¡K™çvëÑDd­Xƒ½Q|œ ]"_®3ş2q‘=U»‚%LÂ©µú.NêƒN0-BµÄ„KçRŒ2à³»ói=*Ôz@Úa¯Xùu#ÒÅX–fß©*¬µÿ=÷®n DÕËª|hª7N‘¹—/±|!’Œ§8Iyü­ˆê™Ì7µk÷«7c±ÆÔƒ¨oJ²†Ü3húŠü +é}¨E.[ÀÜWÙ·N>¶·‘ğ¯¸¿)ÉwµÀ`qêŒÈK%·…i˜’•rœ »7 c!•dšÉÈ½¸lˆş)ÕöUúƒÃ5`âzíî™O-ì).ˆxØx[«½yh9xÏG5És‘°JĞïğ«~sW‘pZRõĞxN}ÑŸ÷cyv§á%
·yŒZ—4¿jÂÕTƒ$öüˆv½ã$]L(ısêLtĞÑŒµ7.ÖÔÆÁÂÅı÷Ş)4³ëW»eårSØĞÛ6Ö<Ï³™·râƒ•6¤—@÷ıÆJèTt¢0¶¬iÏrjŞçÂáİ¥êE–F’)ÑBêŸŒ¾.¢¦Ró¶ÕN•÷R´© Äµ¿ Lœÿ²6•Ï~Ò¦ ºÕ˜U…tmòøí¹+t\»ÉjÄí˜R`bR3Á˜Bq¿íÛ€s"À\Å7œÃeæx’©y>,aÌ´1Ò²D>áœCÃy4J¯¥¼W×¿8ni'ÎA’Ck²xÌ3¾wékÇ»LÑ¹~Ê9øşî×+Q&åÚäÚµ 2Mù@æÛõ
£r;øªˆ¶×àÉÜÛ¿¾¨#:¥pÆIêİ´ÑÌ³Ïš/4¬wÈE‘™–¾6-€Ê+>5o5_ŠMl¤Õ¦¼Ë3°í^7XŸ3ÉèÖÔ(öb‡6åaàÃzPü°ª›ÔÄÎs×éÜ%`nm¦&ØP‡,¬Ğ¨¡û‡öÒ0ç.:bQ’QV¶Îåœá˜ìwĞ+áñÇÈİ×™âiKÓ®yHlÒ`mXµ²Ê»kBùŸ°V¶|±»*Öı”…†m¯¤ûSt8„®îwùäü!©-àEh`ÙpöM«Ò™,§Ï){©+æ}İB.J‘³zäÖf/‘×ğ{ÉóJ!è7D	"ñÖYJOcÇåñs‚[b~ÏÎPo=şdrî“»òÊ°iÁô_÷§	±ô ÈŠ!bÆ—‡U—õMym	L±Ø7Ú¶òï–Äšê®„È¦n¯ê¼¼ß,;ÔèÉú±ûpœW2ÒQÿÌ0|3Ü”„Lv,zŒCÿ…m¾ààá”f¬¼¸›¾¤cCîâ¸ŒPõŞˆ3
‚İ‚mò}$ÀÄ¦*?v—»ogÅ¾g'$šSívU‘R´–Ç¾ÆYF5Çì6…m2JÂª*Í½à  ¦2……ŸïzÚûñpK…aÖ£‡…ÓbÄ×:†“	CŒ¾YÁ+ª².+IEš´µX¨ÛH¯Du¢Ç"`%«âCÿÀ¯sXİş®UïÏê ˜:Ê ?j¾yPğÀ	0Iéñ6º¿×ÑfQ`ûv L·ø^{™ÆLXÓI ¾K~İ Á[$±ÌR4Êû'tæÆ‘ç·o´ˆ}ı\¬`pX8çnLL¯-ˆô…ÉiÂWØ#Ãoß’ŞÍ—Ìµb£¼c4gİ•R[V&­Á¦ äaà…+S—Ñ½‹ˆ&Íeb*…ÌXƒºö@tñÁH ·êCÛÅæ­6éó dÊbÕ4#ÉjDáÓtX9J,1Æ]‘_4@³/òÍ÷G²Äl™Á½ëö›7è«3%€‘.ÁAŠ«¨Aw*}«~êäÚwS 4hhæ}J|vîû#hs4ÒÏZ`tòÓyöMÏ‹ÜõÙz(Ağ’x:ãuTmØà…m„¯4>MºV=Àku$GêŞÖ6{l»¡‚à=73ÍÁŒ’Sıİ	U
†¡ğí‘Òø˜Á@a–Ñ‹øÂ£¹·™Ä÷#*±Ë#?ª°=µZ‰â½m¿sÙ±Ø´Aı8e0g÷âµ'_wª#(l§#~£_Q^Ë‘uó’§
 Ö¦~h°H@Âbã„Ğ® ¹g¢ ‚xjÑ{Ù‰(‰?åš)•’¿)lÔåX5•v!æ†Ü5ÿ.Cİ¢øXô„»ğCï"¼Ñe‹ÓXËHñĞTZ¹×8h›N©I×Ø,çÙ+pvµ=Ëc3kxÆ˜çKY*ı?Å¦gUŞ°ÈÏ¥ëaÊìu‘
L†y+è$Ş¯­ €r!»åŞş¼óŠªRv§Ëñ¥
´×½mÏøşøÿHš8.4Ø /y¯¼ ’Í<ªZŠOÙ†?b¶TÇ’¢"tW¾ç*ï¹¸\¶2T>Ã[€5Ï(ñô"9ë¥ i,-j§ä¼Ö„wîp	´O¿+GSv&‘¢ÇÍ‡f›!ÔÒ7†¼Ç<)…¡—aˆÒ³u^¹„˜	‡ÙÇ°Œ€$ÿ PØ¶HÊmmÜ«Å`UåjKß|!EúMU@a¥:RyTx&ş±svfâÔmhÂ†ÿ(CØÙ´J'šl/¼:ÏÔ½¿Ñá!Ä	ôo,¼÷æ)ü{-{#ôäşúFº­'çÚBfæ»(bQ{4“sû¡ûsÿ uÜ“âEˆN;Ë—fzgzÎ~G	½ù]7céuªt1":?*ŞÍK„ÁÙI¹ÇŸqZVƒİ×„Fñ‡6_óP‰<HŒ&åöBıN†ˆ‚”‡Å‹E“Ú!¸Î);7ĞTÎ"ûé«({p¯æĞ¾–ès[‰3âl¥T†Y‹X–tÚ6A I÷¦ş‚¼UØÁ}Q‰ÄRº\Ùå§ÿ³88kŞ5¬¯k_Xw<ñkxDCúU«ÒÒ]ís#€Ál¿FXxBUh˜*1éu"*ºKŞ`¯¬ƒ40>÷…ÌG[œBªğäÁ&Ñ2Ş€zˆğÛyw<ÿç®õÈvsÖ=úæ$öÎwF´ø†ÕÚd¯‡<]ÕÜ”ŞÀ…•­±ƒªç·†Â`Ã-wœjg.ô?P¹Ë:å1­Ä"._¥	–»UÄ•ÿI£Š;M,S
3ïk©\Æª©}»k>Ÿ ·ÈvÏ#×Ú¥wf³€§×èoÛ\|siJïİp¹İ5‘º±-À]ÒW }@k¡¨‡<–Gsg=Ğ8â£DJÀŠCâhæbŸ¶°ÃœÇÅ°¸ŸH«ÒJ‹Êş4Â¢#ğà#–#0Öì)>¹šÇa+ö¡÷…dè/dExšéİTJaJS5V«e	¥3w ’J-Z3â-£E2sğ.ruìõ´‡i­OØ²tœ‘Ïö>“ì¨DæÍ5¨›«™işë‰Lvùz{h0µ¿QQÆJ·½õƒ%¨Ø¼®DkúW;{Lù9òlÍ-OBe]XÎå›c±5 dw&JÙ¼Mü™|{'©ÒŸÙïÔd·×I€åê¡Ö³£±dØŠL¹6Àà|ÿ°Xœºmg³og\Iw´¨²øÍ¾êt¯_±ı‹£0ŞÂGaáÌÒyR_5Ñ¼¿©Ê.¬„BÌ—‘$öRƒƒğòD<»é~£¢§”4\%ÂŸèÍYeÑ4ËŠÕşË½©‹¨ÿü¡§n¬ã¤p9Y”ºczÇ#Pô-ĞÊò{pîòêCk¶ıªˆ3İ `_ ÑQN%nófôö™ßJ•'Šƒ%FZÀ¸Å<›µ|f:E *Íl1"dô|e‘Pèƒ);ç«º‘Ğ^o@µğ“|ıÕ!‘¾>º{¢½çôCY¼>DP 5;/Lÿæ!N4ÕT/~›µgÀ6*–_Ì	h°‡ñ0yĞŠKÑb‰]½M0wıp®ÇÄ0¿n'@¥ÜjèÂbæì'SÓÁ\oè¯‚¸)PÒqYf°§©"NXZÊMi9Ù¤n©9úæÇèX —ş(¯üÎRGÎjåÁ'^DZ¿¡{!¬­Ò+kRøh¦HTõûfı€Åqg8Ñw^›îÉÕJ‡Á ô75èæ¬Œ<,JÂŠü—ı•àwb	¤pèr^şñ†öç£ëèi2.Œ½³=usJì,‹ûÏ¯J©2d¬êOlIeğå›¾}?â„€ô¶ ¤0…\hÊöŒ¯’0Y` ¡- ûÉ
áÙ+ã&{ûÌXVPŠÂ4h~„Uw²Ú¥ÕIL¶b]ò©nÊ²¦µ»~¸éé’£¸Òz-dDc.mXĞmU”+„G9•*rœ«/o¦Ú‰ :Ì§uÂÕo²¦«ù¥27	ïì¨K¶†Wdİµq+şüKT;[Şî·Ä
§lÂÑän¬²|‹ùJ6Ì[íÀÜÍvj>rKDùèŸT‘r¸@—gÿú;£e k9z{jÌ,ğ4ì¾QˆãÂ¿•ar±‚vm«ü"…Í¡“™:t£< Tbß·TQ~®Jìu‹N‰ç}r‚±jM'…ó…ıíü‹èŞŸhüFÇé*®ár;àÖv$ÚË¸zlÍŸ l‰ò»'ÀÈÂÏÆ5;ÅÊyJ0ÇÄ7Õñ1˜ÓIú0ˆÄÊ›vo>8_Ö…Ÿ&i™C§n¨BæXæßo£÷a\“?´^§ãìÁõ—8¡hÁ/È·İOë]åèßÈf3Ä—V0B¤7}Ë"wPÖĞ<¥Ñı[v‰]¤°ñQL2î(mf\iŞ¼
h‹ãÖoG ÊŸtÖ®q÷D2âM•ó½¦~¾çpó=Ö‹B>¤‘ôE»£€–UÄQáó¹?¬+Ü
:Š`¤K>D
-±ñì!Vî±ÇS²ÇÀ.ÒrCúŠFL©x	Ÿ`YWÛ›×¨tñ·•¯ Y
l2´ğ‰Âülñİ’Ô®£@3H ZÍz µO´eKËdËœ}™¨-tIK‰²^Ü-¨a6GQr/D¼AÁ«¡MhÄ!nê<òZ„h|´÷åèÕš0 ~(myû³«Š¼`é—ä×Ìóv¢5÷¦}ş<6€Fà¢H`ó˜f¿oYškÍÃ(¼õ»5E\IAÓAµèTB^b¬O¢À‚ì¸+h'ˆ–àX/"üç=öŞ…ŒâÒB¸b«~hº­ÚÏ¼¿[Ô| c¥|69œR<@G¶uİ;µsÌ>oee¥K¹¾ÃüïÓú&mrĞ?ãï¤h–CMĞÌåÊÙX3Ïoë1ƒ*UÂö¾/
äˆ&¬½òd©ËçˆøNÜFeùo+üEÓ/NDë™ÀzåAkµe‡¥lcP(Ã—j¸T€™JÜ’´…´fkNB~˜µ¥Rİû¸óŠ¡•Â?Nº6·œ¹ıúğô¦:ç²|šæÃDÏ,—B´Õ‘Y_õ‚¢ùù+ı!æ xßOÚøïz›¬€–ÄÖçVPY'¦„Zª×øÜÈ8p]1smñbOƒlax÷ªÑKœ»IèÕ÷[¼
}ŒZMÏ‡^¦)A{ªwg<¨ùèƒİÈ€óÌ´o³åÑ¨/0Ç»•Š Ë)Ï'usUá2.®Jºy;3Ä Ün\ëâtí¾ºmíïYkÜX¥}X'51Ë‹Á"æºx1“-©Ÿ´Üì°fö¤V–vŠgw`ŞÖe¾Ä†•¬Vù9 Í.
È¤CXÌò>Š/¨L)¿Âı6¶ÕTÒ„ÿn‰ÚáŞ1ÄÿHÂ]{ÔmÄ¦3HAU¹ lˆªÖĞ¯ò«g44À|ä$@w†T9È ¹AA,ŞS4ªoâÀhp#Üï˜¶‡C'ì»E…—Zt|©pÈÊ3ñú¸‘Û
©¹AEhÊ®åiƒqÜH23ÓY®Ôı’3O›Ü‹äŒÜ0”DÅ’Iæ_^c(4äfá€¾Ğ¹ö¾İyk³9;A6úMÜçÿH øğ`M+©}ê©ıl³7-XC¢Â$–%o¨*¹sCQTîeY 0Ÿ¸}=MiÙyÄ‘	z„ˆ·›m@²û8­Ã+İp ³GË\DÚ4Û5/e‡ÕşPvá 	¦Ñ‰5Ux&ˆ“}š%»‹òNv}úÕNQÄq~s#ÿÍoáŞÄ´Í7`CÁ‘kÿT€fºx$˜r"ªÈŠvvwØ\ÚlÍã^?¶¾ı/Ö9ÔGa®J´Ã®Gt m‰zwş=Óº)
æ„Ò)pƒöØ…uHª‰á}‘3„ -0`#3×Ë]XiĞy÷Á±µe”*®!~³»—*“Q•ùÒRŸëEsÈn¬	u)÷w—)~Œb¥/N&çu!l:(O¿y­±o7sJT	8H[ø±š=jäœVIŸ$+{ ÁSw)‚&ÑN¼á_º±ù0ñlÆbICa&ä|KQm¦>pÃ±^²óÜTŒ”‘—ûÏB–:¯úı¬æã¶z.¬W0İã\U@< şiEvìjäÛ†_4i0í‰>ôêZ‹Ù@u€OÄÑ€Òß~¾æJÖQşWÈ®5Tl$ß0…DqÀ©ã€cÆÀkÉ»œiİ¤ÖW†K¸Ï¥£/Ú8J¬:óŞw
6ÁN¼ŒbÒ}* õºÄl»$Ç°Â†äÓWö¢0Gl¡ û¦O¸39›dT¥ƒ¿Übï»ˆxâ(¾´ZØ…¹GÄ¼ŸB]å Ô $¥&?a!e¢rÅƒÕq×liQ:ÿdšú¹èš”†÷§`,ÈâÕ˜x4zTnHps´d®‰_e¼p¹çdé©¨&¨(ğwòìÔØä½ı-şq©
ëÕö°Fy˜ŠÑ!‰²
¬ÂÌ'ˆÑt†hÓa}”Ù˜Ÿµj¬@€ànÈ¡#ğCÏŞı¦aÔr.¯[´¤…Ç¸c–#j²ñ›õ4†÷‰/AyJW~½,ufÔ6"Ÿ½–±À‚í-N)ãÃÎÊêĞÜ¬Vİ£Ğ¦<ïm%Âˆ”X(e¢ªu0Mğ‹œ DfË‘3SbµI
÷Á²´6!ïm…Aİ0z ƒ#N°  †o
äÒ
¯ÏÆ¥(P<?øËù=Rã jĞÁ¸¶ŞÔù™B˜Ë]îµäägcŠämXáÆñß“@†Ù–{í‰–J3.fXƒ!ğp¦;8uÃšÙÆŸ£ÔïY}§$|dtnŸ!àÈ¦mF<-*káÆöÒóKê“… £ä/Ïî•Ë-É‚±¦ÖÂ§¨ÎOM{N†ÙÇ¢»˜kX™ŒVI‚L‹ŒNdhŞH—E…¢¶³†›øÀæàõËZ1
I$®ÅQm´xšüÀc-Ñq\Í®_m¼]0quÒyËªş‘WÌ\ˆYÊP¹AÉôt¢«3@êÍULpÿ» Ì{ã¤dë$Â(!í´´I¤÷‰,áİ–I7l;úmn¨iæ""]”÷ŞmÖ%R1',´htV«É¨Ø.QèœŞ’y!/\<qŞÍ·ìæì?À[çNÌdì½ñ‰¥û=GmåQ
||}x9VºÍTP´ï³ˆ;9•d!Ùó@	$3çàQtÁŞ†fwÎ c7Ú#šZq…3¡«¥Cñ†ö»I¯JæQFVuD´üîÈ}¯-»Šu²¥I.çk"¿³½v#ÕíŒ)}Î¬›'çHVO™/4`aşì$…ÊÑr`ı25x%Œõ£`ÚÇö½CÏnã£YĞµõt/I¬ÃÉ¥$”y0cŸfr­Zj]KY4`<=ØyõJpRXÇ©¿šÃ§D<'b%ÉåÏU+ğ†Z×‰·T°îÏeÕ îˆ«î§ºæí~>snn–Ç0Lló…*¼ŸOÒ^¬såŠœ6Ú^o‡hËÇ‘P‘yM¶p"¾“OÌ
	çıu›ïRKT¬JY[V˜Ok™ÔLCâ£3P÷«’K®ÙÁÆ`Í¬WehùË:l]²î‰Grî`MÌóˆƒÜ«ë]œQÕ5E©)&|šÊùÍ¹¢Ä{™jpÚÚA¥; ÿŞ;_ã4ÎqyÄäk#X…À]Gşk< \Øë÷ÆLY´!Éùä²ø~
XãH{÷=lªë§Š<DV‡¾•1Ñ‡Áé»WG²ÂS?î\‚÷#[¥–˜^Nî™8û"HcŠ²Ø9”EŸşt¼úY	X¯ØÃ8Z¾:4	³ù½C†ó÷w­Nb>cõIl–Ä+ŠÇ^çhşÇ“ZêtG&¾œğû¸Ošî³¹$~.ç#CãÓÊËoè6.–‘ª+,&ÈmLQ3ÙÏRQ_ÃRíR•º…˜zÓãO½<j‘*Àw¯ºìı``¶“
VÂ¨|¤ÜÓc¢Y‘ñ1Qd÷«d~7A»RiiXWìEˆIğ%Ù•‘FêÀÔCOE) \
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
    $(wildcard include/config/SECURITY_NELF                       Œ      4     (            GNU  À       À          èüÿÿÿƒì‹    …Éüÿÿÿú   t2ú @ uÇ€x     1ÀƒÄÃ´&    v ¸êÿÿÿƒÄÃ´&    Ç€x      1ÀƒÄÃèüÿÿÿW‹JV‹2S#°d  #ˆh  ‹˜”   ‰2ƒ=    ‰J[1À^_Ã‹°d  ‹¸h  ƒÃ‹€T  èüÿÿÿWVPh    Sh   èüÿÿÿƒÄëÈ´&    t& èüÿÿÿ‹ˆL  …Éu‹R89  t‰  ‹ˆ”   ‰Èèüÿÿÿ1ÀÃèüÿÿÿUWVS‰Ãƒì8‹³”   d¡    ‰D$41Àƒ»P  ….   ‰ğèüÿÿÿé.   ´&    èüÿÿÿƒz$u=V‰Æƒì‹€L  …Àuƒ¾P  t,‹†”  ‰B4‹†  ƒJ(p‰B8ƒÄ1À^Ã´&    1ÀÃt& ‹†”   ‰$èüÿÿÿ‹$ëÁ´&    ¶    èüÿÿÿ‹€”   ‹@\‹¤  …Òt'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿX1ÀÃ´&    ´&    èüÿÿÿƒì‹    …É  ‹@@‹¤  …Òt'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿX1ÀƒÄÃèüÿÿÿWVS‹°L  ‹˜”   …öu‹°`  ‹¸\  1Î1×	ştJ‰\  ‰ˆ`  Ç€L      ‹C\‹¤  …Òt'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿX[1À^_Ãt& èüÿÿÿS‰Ã¹   ƒì8d¡    ‰D$41À·CÇD$    ƒ=    ÇD$    f‰$f‰D$f‰D$f‰D$$D$2ÇD$(    ÇD$  ÇD$   ÇD$  ÇD$   f‰L$ÇD$     ÇD$&  ‰D$,Ç  ‹C¹   ‰âèüÿÿÿƒø…¯  ‹C¹   T$èüÿÿÿƒø…¯  ‹C¹   T$èüÿÿÿƒø…¯  1À‹T$4d+    uƒÄ8[Ãèüÿÿÿ¶    èüÿÿÿU1íW‰Ï¹   V‰ÖS‰Ãƒì(d¡    ‰D$$1À·CÇD$    T$!ÇD$    f‰D$f‰D$D$‰D$FˆD$!‰øfÁÀÇD$    f‰D$"Cf‰L$‰T$ÇD$  ‰$‹C¹   T$èüÿÿÿƒø…  ·D$fÁÀƒ=    ·Àâ  ‹T$$d+    uƒÄ([^_]Ãèüÿÿÿv èüÿÿÿUW‰ÏV‰ÖS‰Ãƒìd¡    ‰D$1À‰ÈˆT$fÁÀf‰D$·D$$fÁÀƒ=    f‰D$
—  C1í‰$j ¹   ‰ØT$èüÿÿÿYƒø…R  1À‹T$d+    uƒÄ[^_]Ãèüÿÿÿ´&    èüÿÿÿUWVS‹p‹X|‹@(‹~Ô=	˜ „P  vN=	˜ „“  =
	˜ …  …Û¸   ¹   º   EØ‰øSèÿÿÿ€~~ Xuut& 1À[^_]Ã´&    f=	˜ ti=	˜ …Æ  …Ûƒÿ   ¹   º   HØ‰øÁûƒÃ€¶ÛÁãSèÅşÿÿ€~~ ]t®S¹0   º   ‰øè¬şÿÿYë™´&    fS¹3   ëà´&    ‹†Ü   ‹–à   ‹-    ‹@|‰Á	Ñ…  ‰Ùº€ €Áá)Ù‰È÷ê
ÁùÁû)ËÁã…í2  S1Éº   ‰øè<şÿÿX‰İƒÍU¹   º   ‰øè$şÿÿ€¾„    X…Ö   €~~ „üşÿÿS¹   éIÿÿÿ[º€ €Ááé  0 ‰È÷ê‰ø
Áùº   Áû)Ë¹   ÁãSèËıÿÿ€~~ X„°şÿÿS¹1   éışÿÿt& [º€ €Ááé  0 ‰È÷ê‰ø
Áùº   Áû)Ë¹   ÁãSè{ıÿÿ€~~ X„`şÿÿS¹2   é­şÿÿt& …í>Sº   1É‰ø½   èHıÿÿZéÿÿÿfU¹@   º   ‰øè.ıÿÿXéÿÿÿ¸êÿÿÿéşÿÿé½  é  t& èüÿÿÿ‹    SXä…Ò]  ‰Øèüÿÿÿ1À[Ã´&    ´&    èüÿÿÿV‰ÆS‹X\‰Øèüÿÿÿ‹ƒ¤  …Àt€‹´  èüÿÿÿ‰ğèüÿÿÿƒÀ   èüÿÿÿ1À[^ÃèüÿÿÿU‰ÅWVSƒì …Òu@¹   º+  èüÿÿÿ‰èèüÿÿÿƒøÿ„t  E¹À  ºø  ‰D$èüÿÿÿ‰Ã…À„f  ¹   ‰êèüÿÿÿÇC   ‹¸  ‰ØÇƒÔ     º   ÇƒĞ     Çƒô     Çƒğ     èüÿÿÿ‰Æ…Àˆª   Çƒ\   °  ƒ¨  ¹   ºF  Çƒ`      Çƒd  ÿÿÿ Çƒh      Çƒ     Çƒ„  ÿÿÿÿÇƒx      èüÿÿÿ¹   º   ‰èÇƒˆ      ÇƒŒ  B   è¥úÿÿ‰D$‰ƒ0  ƒøÿu&¡    …À¦  ´&    f¾íÿÿÿƒÄ ‰ğ[^_]Ã¹   º   ‰èè_úÿÿ‹»0  ‰ƒ4  ‰|$‹=    …ÿ}  ‹|$ƒÿÿt¸‰D$	øt°‹t$‰ğƒæÁøN@ƒà‰$ƒÀ‰D$‹D$¶Ô‰×ºÍÌÌÌ‰ø‰|$÷âÁê‰T$’‰úÀ’)Ç„N   ‹t$iÎ'  È‰ƒ8  ‹D$ƒàûƒø”ƒ<  ƒ<$FŸD$
ƒÿ¶D$
Áƒşˆƒ=  ŸÀƒÿD$¶t$ˆ‹>  	Æ	Èƒ<$C‰òˆƒA  ˆ“@  ŸÂ‰Ö!Îƒ|$¹   „  ˆ‹B  1Éƒ<$CˆƒC  NÁ¶T$¶L$
ˆƒD  ‰ğˆƒF  !Ñƒ<$Gˆ“G  ”Àƒÿˆ‹E  ”Á!Áƒÿˆ‹H  ”Á!Áƒ|$”Âˆ‹I  !Ğƒ|$”Â!ĞˆƒJ  ¡¨   ƒøv ƒ<$F~ÇƒP     ët& 1Àƒ<$CŸÀ‰ƒP  j »À   1Éº   ‰øèüÿÿÿ€»G   ^…Í   j ¹	˜ º0  ‰øh æ  j h  j hÿÿ  j j èüÿÿÿ¹	˜ º0  ƒÄ ‰ƒ˜  ‰øj h €  j h  j hÿÿ  j j èüÿÿÿ¹		˜ º0  ‰øƒÄ j j j jj jj j èüÿÿÿ‹³ü   ‰{l‰ƒœ  ƒÄ …ö„î   ‰øèüÿÿÿ‰ğƒÄ [^_]Ã´&    f‰ò„Ò…–   1Éénşÿÿ´&    j ¹	˜ º0  ‰øh €  j h  j hÿÿ  j j èüÿÿÿ¹	˜ º0  ‰øƒÄ j h €  j h  j hÿÿ  j j èüÿÿÿ¹
	˜ º0  ‰øƒÄ j j j jj jj j èüÿÿÿƒÄ é°şÿÿt& ÆƒB  ˆƒC  éâıÿÿ¶    ¡    …À›üÿÿéï  ¾ôÿÿÿé‘üÿÿ“˜  ¸   èüÿÿÿ‰øèüÿÿÿ€»=   „u  éW  é}  ´&    ´&    èüÿÿÿ‰Ñº   é¿öÿÿ´&    ´&    èüÿÿÿ‰Ñº   éŸöÿÿ´&    ´&    èüÿÿÿQ‰Ñº   èN÷ÿÿZÃ´&    t& èüÿÿÿQ‰Ñº   è.÷ÿÿZÃ´&    t& èüÿÿÿWVS‰Ã‰Ğ‰Ê‹s\‰†t  ƒøwƒùv?¹`  ‹=    ‰p  …ÿ*  Qº   ¹   ‰ØèÔöÿÿ€¾E   Zu:[^_Ã¶    É|‹<½`  ƒÿÿtã‹`  ÷Ñ#p  	ùë ´&    v ÿ¶x  ‰Ø¹@   º   èyöÿÿX[^_Ãt& èüÿÿÿUW‰×VÁïS‰Ã‰Èƒì‹³”   ‰<$‰×ƒç‰|$‰ÏÁéƒçƒá‰|$‰L$9“ˆ  „  ‰“ˆ  ¹   1ÿ‰ƒŒ  ¸   ‰ÕÓíƒåDÇƒÁƒùuî…Àº	   Çƒ”     EÂ‹T$1É‰ƒX  ‰ğèüÿÿÿ‹T$¹   ‰ğèüÿÿÿ‹T$¹   ‰ğèüÿÿÿ‰ğèüÿÿÿ¸»   º   ƒ»P  »0   EØ‰ğ‰ÙèÀôÿÿ‹$‰Ù%ÿş  Áââ   	Â‰ğRº   ènõÿÿ‹F\Z‹ˆ¤  …Ét'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿXƒÄ1À[^_]Ã´&    9ƒŒ  …òşÿÿëá´&    v èüÿÿÿUWV‰ÖS‰Ã»¨  d‹-    ƒìd¡    ‰D$1À‰â‰øÇ$    ÇD$    ÇD$    ‰l$ÇD$    èüÿÿÿèüÿÿÿ„Àu…öxN‰ğèüÿÿÿèüÿÿÿ‰â‰øèüÿÿÿèüÿÿÿ¡    d‹    …Àu4¶ƒ´  ƒà‹T$d+    u1ƒÄ[^_]Ãt& ¸   ‡Eèüÿÿÿë¯‰Ğèüÿÿÿ„ÀtÁ1Àèüÿÿÿë¸èüÿÿÿ´&    èüÿÿÿS‰Ã‹€˜  ‹@‹@èüÿÿÿ‹ƒ˜  ‹€   ‰P|‹ƒœ  ‹€   ‰P|‹ƒ˜  è~ôÿÿ‹ƒ˜  [‹@‹@éüÿÿÿ´&    t& èüÿÿÿVS‹˜L  ‹°”   …Ûug‹    ‰ÃÇ€L     …ÉT  ‹ƒP  €£´  ı…ÀtFƒèƒøw4‹F\‹¤  …Òt'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿX1À[^Ãt& ‰ğº   èüÿÿÿ‰ğ¹rœ” ºrœ” èüÿÿÿ‰Øèüÿÿÿ1À[^Ãfffffffèüÿÿÿ€=     WV·òS‰Ã‹x\tW¹   º   èüÿÿÿ¹   º	   ‰Øèüÿÿÿ‰ñº
   ‰Øèüÿÿÿ‰ñº   ‰Øèüÿÿÿ‰ñº   ‰Øèüÿÿÿ€¿C   u$[^_Ã‰ñº   èüÿÿÿ‰ñº	   ‰Øèüÿÿÿë­t& ‰ñ‰ØºA   [^_éüÿÿÿ´&    ´&    èüÿÿÿWVS‰Ã‰Èƒì‹{\‹  ƒùw;‹44  ‹   …Àu7ÁæƒÎ ‰ñ‹5    ·Ò…öo  ƒÄ‰Ø[^_éüÿÿÿv ¹    ¾   …ÀtÉƒø
~€¿I   tpÁæ	ñë¹f‰ÆÁæ	ñë®´&    èüÿÿÿUº   WVS‹h\‰Ã‹µˆ  ‰÷Áïƒç‰ùèHÿÿÿ‰ùº   ‰Øè:ÿÿÿ‰ñº	   ‰ØÁéƒáè&ÿÿÿ‰ñº
   ‰ØÁéƒáèÿÿÿ€½C   uÁî‰Øº   [‰ñ^_ƒá]éñşÿÿ‰ñºA   ‰ØÁéƒáèÜşÿÿëÑ´&    v èüÿÿÿSº¸  ‰Ã1É¸¸  ë´&    v ‹B…Àt‹
ƒÂ9Ëuğ[Ã¶    ¸†  [ÃèüÿÿÿW‰×V‰Î‰ÑÁÿSáÿ  ‰Ãº“   èüÿÿÿ‰ùº›   ‰Øèüÿÿÿ‰ñº£   ‰Øáÿ  Áşèüÿÿÿ‰ñº«   ‰Øèüÿÿÿ‰Ø1É[ºV   ^_éüÿÿÿ´&    fèüÿÿÿUº†  WV‰ÆSƒì‹x\‹Ÿ  ƒûw‹H  ƒ¿P  ‹    ty‹‡X  ƒø	„:  ‹¯”  ƒıtoƒå„  ƒû½   Fİƒøu]…ÉB  ƒû„U  Ï  …Û…—  ‰ğ¹GZ ºGZ èüÿÿÿ¡    …À<  1Òë}t& …É   ƒÄ[^_]Ã1Ûƒøt££   ƒø„:  ƒø…œ   …É  ƒû„   ¢  …Û„¦  ‹    1Ò…Éü  ƒû„Ù   ƒû   …Û…˜   ƒ¿X  „«  ö‡”  ¸0   EĞ¡    …ÀØ  ƒÄ‰ğ[^_]éüÿÿ…Û•Ã¶ÛéUÿÿÿv ƒø„‡   ~…É€  ƒÄ[^_]Ãt& té…É^  ‹¯l  1Ò…í•ÂÁâéWÿÿÿƒûuœƒÊ ëŠ¶    ƒûtğëŠ´&    f‰ğ¹ €a º €f èüÿÿÿ¡    …À¹  1ÒƒÊéLÿÿÿ´&    …É›  1Òéÿşÿÿ…É¿  1Òéïşÿÿƒûu¹ €a º  d ‰ğèüÿÿÿ¡    …À  1ÒéÍşÿÿ¶    ƒûus‰ğ¹ €a º €f èüÿÿÿ¡    …Àã  1Òé×şÿÿ¶    …É•  º   é|şÿÿ¶    Cşƒø‡Zşÿÿ¹ 0  º   ‰ğèüÿÿÿ‹    1ÒéDşÿÿ¶    ƒû„Tÿÿÿé`ÿÿÿ´&    v ‹    …Òz  º   1É‰ğèüÿÿÿº   é>şÿÿ´&    v ¹0  º   ‰ğèüÿÿÿ¡    …À[  1Òé€şÿÿ´&    t& èüÿÿÿU‰ÕWVS‰Ãƒì‹@\‰$‹€ˆ  ‰Á‰D$¡    Áé…ÀÊ  ‹$|í Ááº»   Áçá   ¾   ‰¨X  ˜  Ç€”     ‰Øèüÿÿÿt& ‹Œ·`  º   ‰Øèüÿÿÿƒîsè¹   º   ‰Ø¾   èüÿÿÿ¹@   º   ‰Øèüÿÿÿ1Éº   ‰Øèüÿÿÿv ‹Œ·x  º   ‰ØèüÿÿÿƒîsèDí ºƒ   4Å`  ‰Ø‹N<èüÿÿÿ‹N4‹V0‰Øèüÿÿÿ·V@‰Øè&ùÿÿ¹   º   ‰Øèüÿÿÿ‹NDº   ‰Øèüÿÿÿ‹$€¸<   u
ƒÄ[^_]ÃfƒÄ‰Ø¹ Z  º   [^_]éüÿÿÿ´&    èüÿÿÿS‹P\‰Ã‹‚P  ƒø~ƒøuƒºT   t,‰Ø[éäùÿÿt& …Ày[Ã´&    v ‰Ø[éûÿÿ´&    ö‚”  t‹‚  ƒèƒàıt¹    º    ‰Øèüÿÿÿëª¶    ¹!   º    ‰Øèüÿÿÿë‘´&    ¶    èüÿÿÿUWV‰ÆSƒì‹x\‹ŸP  ƒû€   ƒûuº   èüÿÿÿ‰Å‰Á‰Â1Àå   ƒá@ƒ¿X  	tK¸   …ÉDØ‹‡”  ‰Ÿ”  …ítƒ¿T   „€  Ç‡”     »   ƒ=     L  9Ø•À¶ÀƒÄ[^_]Ãt& …ÛyƒÄ1À[^_]Ãf‹l  ‹‡X  ‰Ëƒø„Ù   Pû1ÀƒúwVº#   ‰ğèüÿÿÿƒ=    #  ‰Ã‹—”  ½   ƒãuTt& 9ê„ø   ‹    …É  ‰¯”  ‹l  ¸   9Ù„eÿÿÿ¡    …À/  ‰Ÿl  ƒÄ¸   [^_]ÃÑøƒàƒøt6½   …Àt›ƒø½   Eëë¶    ƒø
½   Eëéxÿÿÿ´&    ½   éfÿÿÿ¶    º   ‰ğèüÿÿÿ=ÿ    ÿÿOÂƒ=    î  ‹—”  1Û=    º= ğÿÿ½   ¸   Mèéÿÿÿv ƒË‰Ÿ”  éşÿÿf‹l  1Àéÿÿÿéx  ´&    ´&    èüÿÿÿU‰ÅWVSƒì‹X\¡    …À.	  Çƒd  ÿÿÿ Çƒh      èüÿÿÿE‰$v ƒ=    	  ºÿÿÿÿ‰Øèüÿÿÿ¡    ƒøš	  €£´  şèüÿÿÿ„À…E  ƒ»X  	„(  Çƒ      ‰Ø¾@   èüÿÿÿ‹»L  …ÿu‹ƒ\  % °  ƒøöƒæáƒÆ €£´  ıºÈ   ‰ØÇƒl      èüÿÿÿ‰Ç¡    …ÿ…¿  ƒ=    í  ƒş„   1Ò1Àë"´&    v ƒÀ@‹Õ¬  …Ét‹Õ   9Öuä‹    …ÒÎ  @‰³T  €=     Å   Çƒ”     ‹P‹@‰“|  ‰ƒ€  „]  ‹ƒL  …À…O  ÷ƒ\    ÿ „?  ƒş	„6  ‹    …É©  ¹	   º    ‰èÇƒT  	   èüÿÿÿt& €‹´  ÇƒX     Çƒl     ¹   º   ‰èèüÿÿÿ¹ 0  º   ‰èèüÿÿÿ€»<   …î  €»E   …É  ‰èèzöÿÿ‰ØÇƒ       èüÿÿÿöƒ´  „üıÿÿ¾   ëCt& ºè  ‰Øèüÿÿÿ…ÀuDƒî‰è‹}\èüÿÿÿ…ÀuO€=     t€§´  ıöƒ´  „²ıÿÿ…öu¾ºˆ  ‰Øèüÿÿÿ‰Æ…Àt¿¡    ƒøµıÿÿéd	  ´&    f‰èèüÿÿÿë¨´&    1Àë t& ƒÇ‹Å¬  …Ò„ë   ‹Å   9Æuà‹Å´  ‹Å°  ‰ƒd  ‰“h  ƒşFƒş	´  ƒş„D  ƒş	„“şÿÿƒîƒş‡¢şÿÿ€‹´  ÇƒX     éŒşÿÿ´&    fƒş „/  ƒş@…qşÿÿÇƒX     ‰èº   Çƒ”     èüÿÿÿ¹rœ” ºrœ” ‰èèüÿÿÿé;şÿÿ´&    ¡    …À	  Çƒ       ‰Øèüÿÿÿémüÿÿt& ¸ÿÿÿ 1Òé%ÿÿÿt& ‹‹x  º@   ‰èèüÿÿÿé şÿÿ¹ Z  º   ‰èèüÿÿÿéüıÿÿ´&    v ¹   º    ‰èèüÿÿÿë%t& º~   ‰èèüÿÿÿ=ş  ~eƒ=    Ê  ºd   ‰Øèüÿÿÿ…ÀtĞé7şÿÿv ¸   €‹´  ‰ƒX  Çƒl     éRıÿÿ´&    ÇƒX     é<ıÿÿ´&    f‰ÆéIüÿÿ´&    f¡    …ÀI	  ƒÄ1À[^_]Ã1Àƒş
”ÀƒÀë‘¸¸  ët& ƒÀ;1tF‹P‰Á…Òuğéº	  v ‰D$º¸  ¹¸  1Àë¶    ‹J…É„å  ‹ƒÂ9Æuì‹D$éî  é¿	  ´&    ´&    èüÿÿÿUWV‰ÆSƒì‹=    ‹X\…ÿ
  Çƒd  ÿÿÿ ~Çƒh      èüÿÿÿ¡    ‰<$t& ƒøu
  ºÿÿÿÿ‰Øèüÿÿÿ¡    ƒøU
  …À:
  €£´  şèüÿÿÿ„À…  ƒ»X  	„  ‹n\‰ğÇ…T     ‹½ˆ  Áïèüÿÿÿ€½E   …r  ‹F\‹L  …Ò…!  ‹\  ‹€`  ‰Ñ€õ 	Á„€  ‰Ñ€õ€	Á„  ‰Ññ  @ 	Á„{  ¸p  €æ·…İ  Áçº0   ‰ùá   	Á‰ğèüÿÿÿ‰ğèÍğÿÿ¹   º   ‰ğèüÿÿÿ¹ 0  º   ‰ğèüÿÿÿ€½<   …®  ‹    º"   ‰ğèüÿÿÿ‹ƒL  ¹@   …Àu€»J   ‹    tƒù¸    DÈ‰‹T  º    ‰ğèüÿÿÿ‹‹T  ƒùub¡    …À÷	  ¿
   t& ºd   ‰Øèüÿÿÿ…À…ä  º~   ‰ğèüÿÿÿ=ş  5  ƒ=    Ü	  ƒïuÃ‹‹T  ƒù„%  ‹-    …í‘  1Ò1ÀëƒÀ@‹<Õ¬  …ÿ„›   ‹Õ   9Ñuà@‹Å´  ‹Å°  ‰ƒd  ‰“h  ƒù	„b  ‰Øèüÿÿÿ‹‹p  º   ‰ğèüÿÿÿ…À…  ƒ»T   „¬   ¡    éšıÿÿf‹    …Éˆ  Çƒ       ‰Øèüÿÿÿ¡    éoıÿÿ´&    ¸ÿÿÿ 1Òéuÿÿÿt& ‹    …Éh  ¸   éşÿÿ´&    ¹ Z  º   ‰ğèüÿÿÿé<şÿÿ´&    v ‹x  º@   ‰ğèüÿÿÿéwıÿÿ´&    f€‹´  ‰ğ‹~\èüÿÿÿ…À…ï   €=     t.€§´  ıë%t& €=     t€§´  ıºˆ  ‰Øèüÿÿÿ…Àu(öƒ´  „÷şÿÿ‰ğ‹~\èüÿÿÿ…ÀtÅ‰ğèüÿÿÿë¼t& ¡    é™üÿÿ¶    ¹ |  º   ‰ğèüÿÿÿéˆşÿÿf¡    …ÀH  ¸   éıÿÿ´&    f¡    …À(  ¸   éìüÿÿ´&    f‰Á‰ƒT  ƒù…Ûıÿÿ¡    …Àúûÿÿéè
  t& ‰ğèüÿÿÿéÿÿÿt& ‹    …Ò  ¸@  é“üÿÿ¡    …ÀÈ
  ¸`  é|üÿÿ¡    …À«
  1ÀƒÄ[^_]ÃƒÄ¸ÿÿÿÿ[^_]Ã¸¸  1ÿº¸  ë´&    ‹P…Ò„
  ‹8ƒÀ9ùuìé
  ´&    èüÿÿÿUW‰ÇVSƒì‹p\¡    …À0  Ç†d  ÿÿÿ Ç†h      èüÿÿÿG‰D$fƒ=      ºÿÿÿÿ‰ğèüÿÿÿ¡    ƒøÒ  €¦´  şèüÿÿÿ„À…¥  ‹†L  …Àu[ƒ¾X  	tRÇ†      ‰ğèüÿÿÿ1Ò‰øèüÿÿÿ€¦´  ıºÈ   ‰ğÇ†l      èüÿÿÿ‰$…Àt<¡    ƒø~‘é·  t& ‹-    …íœ  Ç†       ‰ğèüÿÿÿé<ÿÿÿt& €=     „ƒ   ‹†\  ©  ÿ tv‹    …Ûl  ‹  ‰–|  ©  ÿ …  €¾<   „ÿ  ¡À   º   ‰†€  ‰øèüÿÿÿ€´  Ç†l     Ç†d  à 2 Ç†h      éĞ  v ÇD$ÿÿÿÿ1Û1íëD´&    v º   ‰øèüÿÿÿ  ÿÿ=ÿ  OÂ9Å}‰\$‰Å‹    …É.  ƒÃƒût.‹İ   ‰ø‰Ñèüÿÿÿºd   ‰ğèüÿÿÿ…Àt¦¡    é½şÿÿt& ‹D$ƒø…›   ÇD$   ½ğ   €=     …  ÇD$ÿÿÿÿ1ÛëFt& º   ‰øèüÿÿÿ  ÿÿ=ÿ  OÂ;$~‰$‰\$‹    …Ò  ƒÃ;\$Ÿ  ‹Tİ ‰ø‰Ñèüÿÿÿºd   ‰ğèüÿÿÿ…Àt¢¡    éşÿÿ´&    ƒø„  €=     …º  ‹Å   ÇD$ÿÿÿÿ‰–|  ƒ|$~Eƒ|$„  ‹D$ƒèƒø†“  ƒ|$ u?‹†\  éşÿÿ¡    …ÀQ  ƒÄ1À[^_]Ã‹D$…ÀuÇ†d   ·  Ç†h      ‰–€  ‰øº   èüÿÿÿ‹|  ‹–€  ‰øèüÿÿÿ‰øÇ†       è×êÿÿ‰ğèüÿÿÿ¡    …À…  ö†´  „–üÿÿ»   ëI´&    ºè  ‰ğèüÿÿÿ…À…>şÿÿ‰øƒë‹o\èüÿÿÿ…ÀuE€=     t€¥´  ıö†´  „Füÿÿ…Ûuººˆ  ‰ğèüÿÿÿ‰Ã…À…öıÿÿ‰ø‹o\èüÿÿÿ…Àt»‰øèüÿÿÿë²t& ‹D$‹Å   ‰–|  ƒø…¡şÿÿÇ†d    ‹D$Ç†h      …À„Ì  ƒ|$…Şşÿÿ€¾<   „Ñşÿÿ¡ø   º   ‰†€  ‰øèüÿÿÿ€´  Ç†l     é¶şÿÿ‹_\‹‹|  ;‹€  „“   …ÀK  ‹ƒX  Pûƒúveƒø…„   ‹    …É«şÿÿéÉ  ´&    fÇ†d     º   ‰øÇ†h      Ç†€    [ èüÿÿÿ€´  Ç†l     é%şÿÿ‹-    …íOşÿÿé„  t& …ÀmÿÿÿéÜ  v ƒø…,şÿÿ‹‹€  9‹|  „şÿÿ‹    …Òşÿÿé£  ´&    v ‹D$º   ‹ÅÀ   ‰†€  ‰øèüÿÿÿ€´  Ç†d  à 2 Ç†h      éˆıÿÿ‹†\  ©  ÿ „Qüÿÿ‹L$‹Í   ‰–|  ƒù„øşÿÿƒù„+ûÿÿ…É„&ıÿÿƒù…1ıÿÿÇ†d    Ç†h      ¡ğ   º   ‰†€  ‰øèüÿÿÿ€´  éıÿÿ¡À   º   ‰†€  ‰øèüÿÿÿ€´  Ç†d    À Ç†h      éÕüÿÿ‹†\  ©  ÿ …Mÿÿÿ‹D$‹Å   ‰–|  ƒøu4Ç†d    Ç†h      éƒüÿÿ€=     uÇD$   ½À   éTûÿÿÇD$ÿÿÿÿéóûÿÿ‹†\  ©  ÿ tÔéäşÿÿR‰T$h    ‹ˆ”   ƒÁ‰D$Qh   èüÿÿÿ‹D$‹T$ƒÄé   ~ƒÆÿ³4  ÿ³0  Vh    WèüÿÿÿCpPh8   j4t$ VèüÿÿÿƒÀ   ‰ô‰òèüÿÿÿ‹ƒX  ƒø	vL¸   ‹“P  …Ò…£   ‹“”  ¾+   ¹$   öÂDÎ€â¾3   º,   DÖQRPhU   WèüÿÿÿƒÄë‹…    u°PhG   Wèüÿÿÿ‰ôÿ³  h   Wèüÿÿÿÿ³Œ  ÿ³ˆ  h@   Wèüÿÿÿÿ³p  h‘   WèüÿÿÿƒÄ(‹D$4d+    ucƒÄ81À[^_]ÃƒêuPhG   WèüÿÿÿƒÄ‹ƒ”  º+   ½$   ¾,   ¨Dê¨¸3   Dğ‹ƒT  èüÿÿÿUVPhj   WèüÿÿÿƒÄéTÿÿÿèüÿÿÿh£   P‰D$h   èüÿÿÿ‹D$ƒÄé  ƒÃh¶   SèüÿÿÿXƒÈÿZé¯  Ch«   Ph   èüÿÿÿƒÄéa  PƒÃ‰D$WVhÉ   Sh   èüÿÿÿ‹D$ƒÄé  WVUƒÅhl   ÿt$èüÿÿÿ¸
   èüÿÿÿƒÄƒıuh   ÿt$èüÿÿÿ‰ØèüÿÿÿXƒÈÿZé  éL  WVUƒÅh´   ÿt$èüÿÿÿ¸
   èüÿÿÿƒÄƒıu@h   ÿt$èüÿÿÿ‰ØèüÿÿÿXƒÈÿZé
  Cÿt$$QRhå   Ph   èüÿÿÿƒÄéä  éì  ‹Ø   ‹I|…Ò„ƒ   …À½  ¸  º  DÅQ½   RPGhÜ   Ph   èüÿÿÿ1Éº   ‰øSèœ  ƒÄéf  ‹†Ø   ÿp|Gh  h  hÜ   Ph   èüÿÿÿ1Éº   ‰øSèœ  ƒÄéa  º  ¸  ë…h  Ph   èüÿÿÿƒÄé£  º+   €»<   ¸  DÂ‰×‹‹P  ÿ4ü   WRP‹E  P‹D$(ƒàP‹D$$¶ÄƒÀ@Pÿt$ÿt$<ÿt$(h(  ÿt$Dèüÿÿÿ‹ƒP  ƒÄ0ƒø‡Q	  ‹…    …À„Q	  hk  jÿUPèüÿÿÿ‰ÇƒÄ= ğÿÿ‡ÿ   èüÿÿÿ‰»¤  ‹E\ƒ¸¤   „Q	  ¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿ[éQ	  ¹+   º  ¸  €»<   DÑDÁ¿%  éÿÿÿÿ³4  ÿt$hQ  ÿt$$h   èüÿÿÿ‹ƒ0  ‰D$ ƒÄƒÀt‹ƒ4  ‹|$	Ç‰D$…œ	  ƒ=     L	  hü   ¾íÿÿÿÿt$h   èüÿÿÿƒÄéQ	  Eh3  ¾íÿÿÿPh   èüÿÿÿƒÄéQ	  ‰ƒ¤  hs  ÿt$èüÿÿÿZYéñşÿÿQRÿ4…@  Chd  Ph   èüÿÿÿ‹p  ƒÄé¶  Fh‹  Ph   èüÿÿÿƒÄéı  R‰T$Q‰L$PChŒ  Phƒ  èüÿÿÿ‹L$‹T$ƒÄé„  FRh  Phƒ  èüÿÿÿƒÄé  Fjh(  Phƒ  èüÿÿÿ1ÒƒÄé  RF‰T$h  Phƒ  èüÿÿÿ‹T$ƒÄém  F‰$Sh(  Phƒ  èüÿÿÿ‹T$ƒÄƒû„  ƒû  …Û„D  ƒë„Ñ  é`  FRh¢  Phƒ  èüÿÿÿƒÄé  FRh·  Phƒ  èüÿÿÿ‹    ƒÄé¶  ƒÆh  Vhƒ  èüÿÿÿƒÄéÔ  FRhÎ  Phƒ  èüÿÿÿ‹    1ÒƒÄé"  FRhü  Phƒ  èüÿÿÿ‹    1ÒƒÄé"  Fjh(  Phƒ  èüÿÿÿ1ÒƒÄé`  FSh(  Phƒ  èüÿÿÿ1ÒƒÄéÿÿÿƒÆShØ  Vhƒ  èüÿÿÿƒÄéÔ  Fj h(  Phƒ  èüÿÿÿ1ÒƒÄéD  Fjh(  Phƒ  èüÿÿÿ1ÒƒÄéÑ  Fh8  Phƒ  èüÿÿÿƒÄé
  FRhä  Phƒ  èüÿÿÿ‹    º   ƒÄé"  ƒû…`  éÑ  C‰L$RhN  Phƒ  èüÿÿÿ‹L$ƒÄé  PV‰D$h\  Rhƒ  èüÿÿÿ‹D$ƒÄé#  FURhD  Phƒ  èüÿÿÿƒÄér  ƒÆSQh  Vhƒ  èüÿÿÿƒÄé˜  ƒÆ‰$SUQRhd  Vhƒ  èüÿÿÿ‹Ÿ”  ‹D$ƒÄéè  ‰ÂÑúƒâR‰Âƒâ‰D$RVhx  Rhƒ  èüÿÿÿ‹D$ƒÄéF  Vh:  hÜ  ÿt$hƒ  èüÿÿÿƒÄé$  h  ÿt$hƒ  èüÿÿÿƒÄéÔ  ‹D$¹†  ‰D$VQh¼  ÿt$hƒ  èüÿÿÿ‹D$ƒÄé¶  h¿  ÿt$hƒ  èüÿÿÿƒÄéÉ  Eh§  Phƒ  èüÿÿÿƒÄéš  ƒÅhQ  Uhƒ  èüÿÿÿƒÄéI   hî  ÿt$hƒ  èüÿÿÿƒÄéã  h  ÿt$hƒ  èüÿÿÿƒÄé9  hÖ  ÿt$hƒ  èüÿÿÿ¡    ƒÄé%  º†  VRhœ  ÿt$hƒ  èüÿÿÿƒÄés  h  ÿt$hƒ  èüÿÿÿƒÄéÔ"  h   ÿt$hƒ  èüÿÿÿƒÄé“"  º†  QRh€  ÿt$hƒ  èüÿÿÿ‹‹T  ƒÄéö"  hî  ÿt$hƒ  èüÿÿÿƒÄé?!  h  ÿt$hƒ  èüÿÿÿ¡    ƒÄé7!  hx  ÿt$hƒ  èüÿÿÿƒÄé!  @h_  Phƒ  èüÿÿÿƒÄéë   ƒÆhQ  Vhƒ  èüÿÿÿ1ÀƒÄér%  hğ  ÿt$hƒ  èüÿÿÿ¸`  ƒÄéß!  hH  ÿt$hƒ  èüÿÿÿ¡    ƒÄé!  h¿  ÿt$hƒ  èüÿÿÿ¸@  ƒÄéß!  hÙ  ÿt$hƒ  èüÿÿÿ¸   ƒÄéß!  h  ÿt$hƒ  èüÿÿÿ¸   ƒÄéß!  h©  ÿt$hƒ  èüÿÿÿ¸   ƒÄéß!  h  ÿt$hƒ  èüÿÿÿƒÄéŠ#  ‰È»  ™÷û»è  ™÷û‰È»°â R™÷ûPh0  ÿt$hƒ  èüÿÿÿƒÄéY)  ‰È½  ™÷ı½è  ™÷ı‰È½°â R™÷ıPh   ÿt$hƒ  èüÿÿÿƒÄé‰*  h9  ÿt$hƒ  èüÿÿÿƒÄé	&  Gh!  Phƒ  èüÿÿÿƒÄéÚ%  ‰È½  ™÷ı½è  ™÷ı‰È½°â R™÷ıPhÄ  ÿt$hƒ  èüÿÿÿƒÄé‰*  ‹‹€  ½  ‰È™÷ı½è  ™÷ı‰È½°â R™÷ıPhè  ÿt$hƒ  èüÿÿÿ‹ƒX  ƒÄé—*  ‹‹€  ½  ‰È™÷ı½è  ™÷ı‰È½°â R™÷ıPh  ÿt$hƒ  èüÿÿÿ‹ƒX  ƒÄé$+  ÿtİPh“  ÿt$hƒ  èüÿÿÿƒÄé@(  ÿ4İ  Ph{  ÿt$hƒ  èüÿÿÿƒÄé'  GhQ  Phƒ  èüÿÿÿƒÄéé(  hh  ½À   ÿt$hƒ  èüÿÿÿƒÄÇD$   ÇD$   éî'  h  ÿt$hƒ  èüÿÿÿƒÄéª&  hî  ÿt$hƒ  èüÿÿÿƒÄé#&  hP  ÿt$hƒ  èüÿÿÿ¡    ƒÄé&  èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                                  ¤     ¼  Å  à  ò         %                          msp3400                                                                                                                                            4  ;  F                             `  @                             0                          Ğ  °          @  Ğ                                                   P          `                                                                                                                              0      `  d  h  l  p  w  3   ~                   ÿÿÿÿÿÿÿÿ      @  @         @     @      @  €0     €  €   €0      €        0  dolby   amsound standard    stereo_threshold    debug   once    opmode              0                                  «  °  ·  ½  Ã      K      $   #   '   (   K      $   #   '   (   cL cL Ğ           0  ÿÿÿÿÿÿÿÿøÿÿÿ   ;   ~   ÿÿÿÿÿÿÿÿøÿÿÿ   ;   ~   cL cL Ğ           0  øÿÿÿøÿÿÿ      N   k   øÿÿÿøÿÿÿ      N   k   rœ” rœ” Ğ   €       0           0   B   H            0   B   H   cL cL Ğ   €  0    0     	         !   %            0   B   H   GZ GZ Æ   €       0  şÿÿÿøÿÿÿöÿÿÿ
   2   V            0   B   H   cL cL Ğ   @       0        úÿÿÿüÿÿÿ(   ^            0   B   H   UUS UUS Ğ   @       0  şÿÿÿøÿÿÿöÿÿÿ
   2   V   üÿÿÿôÿÿÿ÷ÿÿÿ   O   ~   GZ GZ Æ   @      |              ¸  ÿÿÿ                Ú  ÿÿÿ         €> 9A ë   ·         cL ªÀO            GZ  êV #  à 2        GZ 9¤] A  à 2        GZ GZ _  à 2        GZ ªÀO x  à 2        cL  @Q :        	   GZ  @Q –    À     
   UUS ãøZ «            GZ  @Q À  à 2        GZ  @Q T  à 2        GZ  @Q t  à 2          €>  €> ×         !    €>  €> ê         0    €>  €>           @   rœ” rœ”   ÿÿÿ     P   GZ GZ 1  ÿÿÿ     Q    €a   d ?  ÿÿÿ     `     d   d U  ÿÿÿ     ÿÿÿÿ                    Setting I2S speed to %d
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
 mono stereo lang2 lang1 lang1+lang2 5.85  PAL D/K + SECAM NICAM 6.25  PAL D/K1 FM-stereo 6.74  PAL D/K2 FM-stereo 7.02  PAL SAT FM-stereo s/b 7.20  PAL SAT FM-stereo s 7.38  PAL SAT FM-stereo b 5.742 PAL B/G FM-stereo 5.85  PAL B/G NICAM 4.5   NTSC 5.5   PAL B/G 6.0   PAL I 6.5   PAL D/K + SAT + SECAM autodetect start 4.5/4.72  M Dual FM-Stereo 5.5/5.74  B/G Dual FM-Stereo 6.5/6.25  D/K1 Dual FM-Stereo 6.5/6.74  D/K2 Dual FM-Stereo 6.5  D/K FM-Mono (HDEV3) 6.5/5.74  D/K3 Dual FM-Stereo 6.5/5.85  L NICAM AM 6.0/6.55  I NICAM FM 6.5/5.85  D/K NICAM FM 4.5  M BTSC-Stereo 4.5  M BTSC-Mono + SAP 4.5  M EIA-J Japan Stereo 10.7  FM-Stereo Radio 6.5  SAT-Mono 7.02/7.20  SAT-Stereo 7.2  SAT ADR     `   Ğ      @  °     P  Ğ  Ğ     0    À           @  `  €  0    p  Ğ        @  Ğ  p  °     `  À  `  €  Ğ   À%    detected standard: %s(0x%08Lx)
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
    6.5/5.85  D/K NICAM FM (HDEV2)  6.5/5.85  D/K NICAM FM (HDEV3) parm=dolby:Activates Dolby processing parm=amsound:Hardwire AM sound at 6.5Hz (France), FM can autoscan parm=standard:Specify audio standard: 32 = NTSC, 64 = radio, Default: Autodetect parm=stereo_threshold:Sets signal threshold to activate stereo parm=debug:Enable debug messages [0-3] parm=once:No continuous stereo monitoring parm=opmode:Forces a MSP3400 opmode. 0=Manual, 1=Autodetect, 2=Autodetect and autoselect parmtype=dolby:bool parmtype=amsound:bool parmtype=standard:int parmtype=stereo_threshold:int parmtype=debug:int parmtype=once:bool parmtype=opmode:int license=GPL author=Gerd Knorr description=device driver for msp34xx TV sound processor Ì          ¤ÿ     Ô          ¤ÿ     Ü          ¤ÿ     è          ¤ÿ     ü          ¤ÿ               ¤ÿ               $ÿ ¨             €                                  À                  +                                                                          `                            ÿÿÿÿ                     @Q Ï   êV ë  9¤]    €a     d 9   €f S  ªÀO m   @Q …   €> ™  cL ¤  UUS ²  GZ ¾   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                           ñÿ       `     '       2     A   `   e     N   Ğ   0     Z      9     i   2   a    }       (    
 ‡   @  c     “   °  B     £      P     ®   “        ¾   P  |     È           Ô           ß            ç   ³  3     ö   Ğ  Í     ÿ   æ  p          ˆ       V  k     &  0  \    1  Á        A    "     M  a       ^  À  @     i     ò    s          
 {           Å  ¨        ƒ  0      
   y  µ    Ÿ  ü       
 ª          
 ´  `  l    
 »  .  *     Î  @       
 Ú  0  V    è  Ğ  ±     ô  X                        €            
     0      \    
 ?  `   0    
 F     0    
 S  `  ,    
 a  @      
 o     P    
 }      &     ’  &   B     ©  h   Q     Á  ¹   ?     á  ø   '     ö    *     
  I  Y        ¢       9           G  Ì      
 Y  ¶       t          „  Ô      
 ˜  Ì       ´  (        Å  Ü  	    
 Ú  â       ş  <          è      
 4          M  P        [  ü      
 m         …  d        ’        
 £  &       ½  x        Ì        
 ß  :       ö  F         X  9     '           ñÿ:    ¡     I  @  ‰     ]  4      
 g         
 q  s  .     Š  Ğ  –     Ÿ        
 «     5    À  H      
 Ê  ¡  -    ä  `  @   
 ÷  Î  $       ò  »     %  ­  3    :  à	  Ç    O           l  À   0     ‡  ğ        ¢  §  O    ·             Ì          Ö             ã             ö                                         €  B    /             L      
     [           m  `       {             ‰           “             ²          ¾             Í  °  g     â             ü                            `      ,             7           C             S             b  p  ?     x  Ğ  ú     ‚             Ÿ             ·             À             Ñ  À  “     á             ë             ÿ             	             &	  ¤        3	             >	    Ù     H	             _	             l	             	             Œ	             –	  €  ¬     ¤	             º	             Ğ	  À%  
    à	  `  X    ò	  @        
             
  p  T     )
             1
             >
             M
             _
             v
             Š
             —
  `   0    
 ¶
             È
          Õ
          â
             ñ
             ÿ
               Ğ   é                 -          6              msp3400-driver.c msp_s_i2s_clock_freq msp_s_i2s_clock_freq.cold msp_querystd msp_s_tuner msp_log_status msp_log_status.cold CSWTCH.74 msp_g_tuner msp_s_frequency msp_resume msp_resume.cold msp_s_std reset_off.1 reset_on.2 write.3 msp_reset.cold msp_read msp_read.cold msp_write msp_write.cold msp_s_ctrl msp_s_ctrl.cold msp_suspend msp_suspend.cold msp_remove msp_probe msp_ops __key.4 msp_ctrl_ops msp_probe.cold opmode_str CSWTCH.84 scarts msp_set_scart.cold scart_names msp_s_routing msp_s_radio msp_s_radio.cold msp_driver_init msp_driver msp_driver_exit msp3400_pm_ops msp_id msp_core_ops msp_tuner_ops msp_audio_ops msp_video_ops __UNIQUE_ID_dolby282 __UNIQUE_ID_amsound281 __UNIQUE_ID_standard280 __UNIQUE_ID_stereo_threshold279 __UNIQUE_ID_debug278 __UNIQUE_ID_once277 __UNIQUE_ID_opmode276 __UNIQUE_ID_dolbytype275 __param_dolby __param_str_dolby __UNIQUE_ID_amsoundtype274 __param_amsound __param_str_amsound __UNIQUE_ID_standardtype273 __param_standard __param_str_standard __UNIQUE_ID_stereo_thresholdtype272 __param_stereo_threshold __param_str_stereo_threshold __UNIQUE_ID_debugtype271 __param_debug __param_str_debug __UNIQUE_ID_oncetype270 __param_once __param_str_once __UNIQUE_ID_opmodetype269 __param_opmode __param_str_opmode __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 msp3400-kthreads.c msp_set_source msp34xxg_set_source CSWTCH.91 CSWTCH.92 msp34xxg_set_source.cold msp34xxg_set_sources msp_stdlist msp3400c_set_audmode strmode.0 msp3400c_set_audmode.cold msp3400c_init_data msp3400c_set_mode.cold msp_detect_stereo.cold msp3410d_thread.cold msp34xxg_thread.cold msp3400c_carrier_detect_main msp3400c_carrier_detect_65 msp3400c_carrier_detect_55 msp3400c_thread.cold v4l2_i2c_subdev_init msp_debug devm_kmalloc __msecs_to_jiffies __this_module snprintf __stack_chk_guard msp3410d_thread v4l2_ctrl_handler_init_class cleanup_module msp_stereo_thresh msp_write_dsp set_freezable msp_dolby schedule_timeout_interruptible msp_amsound add_wait_queue msp3400c_set_carrier i2c_transfer_buffer_flags kthread_should_stop __wake_up msp_detect_stereo __fentry__ init_module wake_up_process __refrigerator msp_standard_std_name msp_reset v4l2_ctrl_handler_log_status v4l2_ctrl_handler_setup schedule __stack_chk_fail msp_set_audmode _dev_info i2c_register_driver v4l2_device_unregister_subdev _dev_err msp_standard mutex_lock msp_sleep media_entity_pads_init kthread_stop freezing_slow_path current_task _dev_warn msp_set_scart default_wake_function __init_waitqueue_head msp3400c_thread msp3400c_set_mode msp_write_dem kthread_create_on_node msp_update_volume strscpy mutex_unlock param_ops_bool v4l2_ctrl_new_std v4l2_ctrl_handler_free system_freezing_cnt i2c_transfer __mod_i2c__msp_id_device_table remove_wait_queue msp_read_dsp msp_read_dem i2c_del_driver param_ops_int _dev_printk msp34xxg_thread v4l2_ctrl_cluster msp_once __SCT__might_resched     Š  
   u       a   Š  ƒ   u  ©     ±   	  ·     ¼   ¶  Ñ   Š  ù   ”    Š    z  )    0  ‰  5    A  Š  š  ‰  ±  Š  ê  ˆ    Š  
  u      E  ˆ  Q  Š  Á  ˆ  Ñ  Š  â  z  ú  u  2    B    O    a    p  ¯  y    Š  ¯  “    ¤  ¯  ­    º  z  Æ  “  Ñ  Š  ë  z  ]  ¯  f    u  u      Š  z  ™  “  ¡  Š  ´  z  Ø  u  ä    ş  †        z  $  “  1  Š    u  ƒ    ˆ    ‘  Š  —  u  £    ª    Á  Š  Ï  —  å    ì    ÷  ­    Š         ©  '    F  v  U    \  t    œ  ¼    Á    	  £  ;	  u  ƒ	  u  ‹	    è
    )  |  C    a  ¬  k    ”  ¬      ¸  ¬  Ù  ­      .  ¬  8    [  ¬  e      ¬  ©  u  ¶    Ğ  ¸  ×  ‘  ä    é    î      Š  !  Š  A  Š  a  Š    Š  ¨  u  ¶    ê    ö    1  Š  Ã  ¡  Ó  ¡  ã  ¡  ê  ”  b  ˆ  ‘  Š  ¦  Ÿ  ¯  z  Ü  ¢  á  „  æ  ‡  õ  w  ú  ‚    ±    º    ®    Ÿ  -  z  I  ’  S    ^    e  “  q  Š  …  š  À  ª  Ñ  Š  é  u  ı    I  ˆ  `  ¥  q  …  x  ¨  ‘  Š  —    ´    Å    Ó    á    ï            -    A  Š  `    g    y  u  „    ‘    Ñ  Š  q  Š  w    €  	  ©    ±  Š  Ï  ¦  İ  ¦  ô  ¦    ¦    ¦  !  Š  '    D    Q  u      ¶  …  »  u  Ã    Ô           u  *    e  u  m    Ÿ    ¶    ı  …    u  
    $    4    R  …  W  u  _    ‚  …  ‡  u      ¤    Ñ    ×  u    u  
      ¡  =    B  u  J    a  Š  ƒ  u      ·    È  ¦  Ó    ß  ¦  ú  ¦    ¦    ¦  #    /  ¦  D    N  ¦  [  …  w    †    ´    Á  Š  4  ¦  M  ¦  a  Š  ‹  ³  á  u  è    9  ³  ?  u  j  u  r      u  ˜      ²    u  #    n      Š  ’  u  š    ³  €  Â  u  É    Õ  ›  Ú  u  ã    ï  ‡    ¨  W  ›  ^  u  l  u  ™    ¤    ®  u  Å  ƒ  Í      u  $    ?  ¦  p        ³  ¨  Ø  ›  é  ‰  ó  ¹    ›  %  u  3    C  ”  a    p    ~    …      ¥    …  1  u  9    J  ¨  v  ¦      ­  ¦  À  ³  Í  u  Ô    à  ›  A   u  I     e               ’   	  §     ¹     ¾     Ñ   Š  à   u  ë     !  €  !  u  !    )!  ›  .!  u  7!    ?!    K!  ‡  }!    ø!  ¦  "    !"    4"  ~  @"  ¦  \"  ™  {"  ¦  ‹"  u  “"    ¨"  ›  ¼"  ³  Í"  u  Ô"    î"  u  	#    #    &#    -#    I#  ¨  [#    u#  u  ‚#  u  Š#    ›#  ¨   #  u  Â#  u  Ê#    í#    $  ¦  -$  ‰  ;$  ¹  R$  ¹  h$  ›  ƒ$  ‰  $  ”  ™$  u  µ$    Á$  u  É$    á$  u  é$    %  u  %    +%  ”  :%  u  B%    Q%  u  Y%    h%  u  p%    Œ%    “%  	  §%    µ%    Á%  Š  Ò%  u  Ú%    ó%  €  &  u  	&    &  ›  &  u  #&    /&  ‡  [&  ¨  d&  ¥  &  ›  &  u  —&    ¢&  u  ª&    »&  ¨  Ê&  ƒ  ä&  u  ì&    ò&    '    ''  ¥  x'  ²  –'  u  '    ­'    ¶'  …  Â'  ›  Ë'  u  î'    ô'  ƒ  (  ²  8(  u  @(    Z(  …  f(  ›  o(  u  ‹(  ƒ  ™(    á(  u  é(    !)  ¥  4)  …  L)  ¨  Q)  u  €)  ›  •)  ‰  Ÿ)  ¹  Æ)  ›  Ú)  ‰  å)  ”  ÷)    C*    U*  ¥  ‰*    ¦*  u  ³*    æ*  ¥  +  u  +    !+    E+  u  R+    l+    y+  ¥  µ+    ÷+    	,  ¥  ,    ,,  ¥  h,    ’,  ƒ  ¢,                 ¶  .     F   	  L   •  U     a   y  p     €     ™          ¬     ±     ¼     Â   •  Î     Ö     Ü   •  é     ï   •     	    •        •  %  z  <    B  •  P    U    Z    f    t    |    ‚  •    “  ”        £  ¶  ¯    ·    ½  ˜  Ç    Ï    Õ    Ú  ¶  â    ñ    ÷    ü  ¶        	       &  ‚  3  	  <     C    M    R    ]  	  f     p  ‚  }  	  †         —    ¥    «    °  ¶  ¸    ½    Õ    Ú    ß    ò  	  ø    ı  ¶          %    *    /  	  5    :  ¶  I    Q    V    [    b    h    m  ¶  u    z    †    ˜    È  	  Ñ  •  ã    ê    ò    ÷       §    Œ  +    Q  ˆ  W    \    a    f    x    Œ    •    š  ¶  Â    È  u  Ï    Ô  	  â    ç  ¶  ï    ÷          ¶          #     3    ;  	  A    F  ¶  T    \    b    g  ¶  o    ‚  	  ˆ      ¶      ¦    ¬    ±  ¶  ¹    Ã    É    Î  ¶  Ø    å  	  ë    ğ  ¶  ü              ¶  #    4    =    B    K    Q    V  ¶  ^    g    m    r  ¶  x  u  €    ˆ        “  ¶  ›    ¤    ª    ¯  ¶  µ  u  ¿    È    Î    Ó  ¶  Ù  u  ã    í    ó    ø  ¶                ¶  )  	  /    4  ¶  <    F    L    Q  ¶  [    e    k    p  ¶  z    ‚    ˆ      ¶  •        ¤    ©  ¶  ¯  u  ¼    Å    Ê    ×    İ    â  ¶  î    û          ¶        	  "    '  ¶  /    9    ?    D  ¶  L    [  	  a    f  ¶  x    ’    ˜      ¶  ©    ¯    ´  	  ½    Â  ¶  Ê    Ï    Ø    İ  ¶  å    î    ù  	  	    	  ¶  	    	    !	    &	  ¶  .	    6	    <	    A	  ¶  I	    Q	    W	    \	  ¶  d	    i	    r	    w	  ¶  	    „	    	    ’	  ¶  š	    Ÿ	    ¨	    ­	  ¶  ²	  u  º	    ¿	    Æ	  	  Ï	    Ô	  ¶  Ü	    á	    ê	    ï	  ¶  ÷	    ü	  	  
    

  ¶  
    
    
  	  '
    ,
  ¶  :
    ?
    H
    M
  ¶  U
    Z
    c
    h
  ¶  m
  u  u
    z
    ƒ
    ˆ
  ¶  
    ˜
    
    £
  ¶  «
    ³
    ¹
    ¾
  ¶  È
    Í
    Ö
    Û
  ¶  è
    í
  	  ö
    û
  ¶     u                ¶  (    -    6    ;  ¶  H    M    V    [  ¶  h    m    v    {  ¶  ˆ        –    ›  ¶  £    Æ  	  Ï    Ô  ¶  Ü    ÿ  	        ¶          #    (  ¶  0    8    >    C  ¶  K    n  	  w    |  ¶  „    ­  	  ¶    »  ¶  É    ò  	  û       ¶          !    &  ¶  .    5    ;    D    I  ¶  Q    Y    _    d  ¶  l    q    v        „  ¶  œ    ¡    ª    ¯  ¶  ·    ¼    Å    Ê  ¶  Ò    ×    à    å  ¶  ê  u  ò       Š          x     –          ´      ¤     {     ·        $     (     ,     0     4     8     <     @     D     ¨     ¬     °     ´     ¸     ¼     ü                   $    (    ,    D    H    d    h    t    x    ¬    ¸         8    @    D    H    L    P    T    X    \    H    L    P    T    X    ¬  	  Ä    Ü    ô        $    <    T    l    „    œ    ´    Ì  	  ä  	  ü        ,    D    \    t    Œ                                                   $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     €     „     ˆ     Œ                   x     «               x     «  $   ƒ  (     ,   x  0   µ  8   ™  <     @   x  D   µ  L   ~  P     T   x  X   µ  `   u  d     h   x  l   «  t   ¹  x     |   x  €   µ  ˆ     $     (     <     |     ˆ     Ä     Ì     Ô     Ü     ä     ì     ô     ü                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.text.unlikely .rel.init.text .rel.exit.text .rel.rodata .rodata.str1.1 .rel__mcount_loc .rodata.str1.4 .modinfo .rel__param .rel.data .bss .comment .note.GNU-stack                                                         4   (                  2             `   Ê,                 .   	   @       äk  à              <             *-  ö                 8   	   @       Äy  ˆ              O              ;                    K   	   @       L†                  ^             4;  
                  Z   	   @       l†                 m             @;  °                  i   	   @       |†  0     
         u      2       ğB  b                ˆ             RK  ”                  „   	   @       ¬ˆ  (              •      2       èK  “                ¤             {P  ‘                 ±             S  Œ                  ­   	   @       Ô‰  à               ½              S                     ¹   	   @       ´Š  ˆ               Ã             ÀT                    È      0       ÀT  &                 Ñ              æT                                   èT  °     t         	              ˜`  K                               <‹  á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ETWORK) \
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
    $(wildcaELF                      ¼2      4     (               èüÿÿÿ‰ÁSk€t  L|  ‹X4‹H01À‰J‰Z[Ã´&    t& èüÿÿÿ‰ÁSk€t  L|  ‹X4‹H01À‰J‰Z[Ã´&    t& èüÿÿÿƒy…Á   VSy   ‰Ã‰È…   ƒy…”   k“t  L‹I;Š`  …~   ‹²d  9pus‰‹¨  ‹P‰“¬  ‹P‰“°  ‹P‰“´  ‹P‰“¸  ‹P‰“¼  ‹P ‰“À  ‹P$‰“Ä  ‹P(‰“È  ‹P,‰“Ì  ‹P0‰“Ğ  ‹@4‰ƒÔ  1À[^Ãt& ¸êÿÿÿëñ´&    f¸êÿÿÿÃ´&    v èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[Ãèüÿÿÿ‰ÊWƒÁ1ÀzS‹YüƒçüÇB    )ùÇB,    ƒÁ(Áéó«…ÛuÇB   [_Ãv ¸êÿÿÿëó´&    fèüÿÿÿUWVS‰Ãƒì‹@X‰T$‰$…À„   ‹C‹P‹Rèüÿÿÿ%   =   …z  C¹À  ºÜ  èüÿÿÿ‰Å…À„e  1À¹÷   ‰ï¾€   ó«½4  0  Ç…<  Ğ  Ç…@  à  ƒçüÇ…D  UYVY)ùÇ…H     )ÎÁ   Ç…L     ÁéÇ…Q  Œ
 Ç…l     Ç…p     Ç…x     Ç…|  `  ¡€   ‰…0  ¡|  ‰…,  ó¥¹   ‹D$‹<$‹@‰½0  ‰…Ø  ¶À	…Ô  ¶G¶WÇ…D      ÁàÁâ	Ğ	…Ä  ‰Ú‰èèüÿÿÿƒM\º   ‰èÇ…¤     ˆ  ÇE  èüÿÿÿ‰Ã…Àˆüÿÿÿj µÀ   1Éº   ‰ğèüÿÿÿ¹ 	˜ ºP  ‰ğj h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºP  ‰ğƒÄ$j h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºP  ‰ğƒÄ j h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºP  ‰ğƒÄ j j j h´   j h´   jÿhLÿÿÿèüÿÿÿ¹	˜ ºP  ‰ğƒÄ j jj jj jj j èüÿÿÿ‹ü   ‰ulƒÄ …Ût…Ûx,ƒÄ‰Ø[^_]Ã¶    ‰ğèüÿÿÿ‰èèüÿÿÿ‰Ã…À„-   …ÛyÔ‰ğèüÿÿÿ‰ØƒÄ[^_]Ãv »ûÿÿÿë¹»ôÿÿÿë²´&    v èüÿÿÿ‰Â‰È‹	ƒx …»   ƒùt>Ç@   kŠt  L‹‰`  ‰Hk’t  LÇ@   Ç@   ‹’d  ‰P1ÀÃ¶    ‹Š¨  ‰H‹Š¬  ‰H‹Š°  ‰H‹Š´  ‰H‹Š¸  ‰H‹Š¼  ‰H‹ŠÀ  ‰H ‹ŠÄ  ‰H$‹ŠÈ  ‰H(‹ŠÌ  ‰H,‹ŠĞ  ‰H0‹’Ô  ‰P41ÀÃ´&    f¸êÿÿÿÃ´&    v èüÿÿÿU‰ÁWVS‰Óƒì‰D$¶Qp‰T$<„}   ´&    v <t\<t`‹D$¶kÇ$    ¶{‹°”   ‰é‰ú‰ğèüÿÿÿ…Àt8ƒ<$…D   P‰D$ÿt$h-   èüÿÿÿ‹T$ƒÄƒÄ‰Ğ[^_]Ãf‹Cèüÿÿÿ¶CƒÃ<uƒÄ1Ò[‰Ğ^_]ÃèüÿÿÿUhpWV1öS‹¸”   ¶Ú‰Ú‰øèüÿÿÿƒşuPSUhd   èüÿÿÿƒÄ[^_]Ã¶    …Àˆl   ëÛ¶    èüÿÿÿUWVSƒì‹¨x  ‰$…íÒ   ‹€|  ‰Ö‰Ï1Ò‰D$ƒÀë¶    ƒÂƒÀL9Õ„š   ‹‹H!ó!ù	Ùtåƒú„„   kÚL‹D$‰ÖØ¶h‹$ÇD$    ‹¸”   ƒÀp‰D$º   ‰é‰øèüÿÿÿ‰Â…Àtƒ|$…   ƒÄ‰Ğ[^_]Ãt& ‹<$Ÿ|  ‰·t  ¶C€=     ‰‡D  …¶   ƒÄ‰Ğ[^_]ÃƒÄºêÿÿÿ[‰Ğ^_]Ãtï‹$1Û1ö‹€|  édÿÿÿ´&    ¶    èüÿÿÿUWVS‰Ãƒì‹Ø  è×ıÿÿ‰ÆCp‰D$…ö…Ü   ‹»”   1íº€   ‰øèüÿÿÿ…Ày	ƒı…  ‰D$‹«”   Ç$    º   ‰èèüÿÿÿƒ<$t…Àˆç  ‰$‹«”   ºp   ‰èèüÿÿÿƒşt…ÀˆÃ  €=     ‰Â„  éö   t& èüÿÿÿ…Ò„É  UWV‰ÖS‰Ãƒì‹€8  …À„š  Cp‹«”   1ÿ‰$º   ‰èèüÿÿÿƒÿ…ç   ‰Çƒç„ï   ƒà<„!  <…©   ‹ƒ€  ½   ƒø‡  ºw  ¿   £Âƒ  Cp‹‹”   ÇD$    ‰D$‰ğ‰ş‰$‰Ç‰Ëº:   ‰Øèüÿÿÿ…Àyƒ|$…¬  ‰ù‰÷‹$‰Î!ø‰ù8Èu6kíL‹‹|  ‹‹F#T)#D)‰€=     ‰F…f  ƒÄ1À[^_]Ãt& Ç    1ÀÇF    ƒÄ[^_]Ã´&    f…Àˆˆ  ‰Çƒç…ÿÿÿCp‹«”   ‰$º?   ‰èèüÿÿÿƒÿ„ğşÿÿ…ÀˆB  ƒà<…èşÿÿ´&    f‹ƒ€  1íƒø†êşÿÿƒèDƒøw
º£ÂrƒÄ¸êÿÿÿ[^_]Ã¿   éÔşÿÿ¶    ‰Øè‰ıÿÿ¸È   èüÿÿÿéPşÿÿ¸êÿÿÿÃt& èüÿÿÿ98  „   UWV‰ÆS‰Óƒì…Òt8ºíÿÿÿƒûu ƒÄ[^_]é:ıÿÿ´&    v Ç†8      ƒÄ‰Ğ[^_]Ãt& ‹¸”   hpº   ¹   ‰øèüÿÿÿ‰Â…ÀtÈƒû…Õ  ‰$UhX  èüÿÿÿXZ‹$ë´´&    v 1ÀÃ´&    ¶    èüÿÿÿUWVSƒì‰$ƒú^‡ğ   ‰Ãƒù‡å   Cp‹³”   ‰Õ1ÿ‰D$1Ò‰é‰ğèüÿÿÿ‰Â…Àtƒÿ…ö  ƒÄ‰Ğ[^_]Ã´&    ‹ƒ”   1ö{p‰D$‹D$º3   èüÿÿÿƒşt…Àˆ  ‰ÁCp‰\$‹³”   ‰D$ƒá$1ÿ‰Ëº3   ‰Ù‰ğèüÿÿÿ‰Â…Àtƒÿ…;  ƒÄ‰Ğ[^_]Ãf‰Ù‹$‹\$€=     ‰«4  ‰‹Ì  ‰«€  ‰ƒ„  „Pÿÿÿé_  t& ºêÿÿÿé<ÿÿÿ´&    ´&    èüÿÿÿUWVS‰Ãƒì‹p‹x|‹@(=	˜ „Î   v\=	˜ „I  =	˜ …  ƒÿF°Ç$    ‹nÔÉ‰D$ƒáı‰|$ƒÁ‰Ïº   ‰ù‰èèüÿÿÿ‰Â…À„z  ƒ<$t^é¥  = 	˜ „¥   =	˜ …©  ‰øÇ$    ‹nÔ¶ÈF°‰|$‰D$‰Ïº
   ‰ù‰èèüÿÿÿ‰Â…À„  ƒ<$…@  t& €=     …  ƒÄ‰Ğ[^_]Ã‰øÇ$    ‹nÔ¶ÈF°‰|$‰D$‰Ïº   ‰ù‰èèüÿÿÿ‰Â…À„»   ƒ<$t®é}  ‰øÇ$    ‹nÔ¶ÈF°‰|$‰D$‰Ïº	   ‰ù‰èèüÿÿÿ‰Â…À„–   ƒ<$„jÿÿÿéÍ  t& ÿ´   „œ   ÿLÿÿÿ„    ‰ø¶ÈF°Ç$    ‹nÔ‰D$‰|$‰Ïº   ‰ù‰èèüÿÿÿ‰Â…ÀtIƒ<$„ÿÿÿéõ  ‹|$‰¾Ä   éùşÿÿ‹|$‰¾Ì   éêşÿÿ‹|$‰~|éŞşÿÿ‹|$‰¾¼   éÏşÿÿ‹|$‰¾Ô   éÀşÿÿ´&    ¹   ¿   éfÿÿÿ¹€   ¿€   éVÿÿÿºêÿÿÿéşÿÿ           °  @                  É                                                                                                                                ?                              	  €    
  €      €                                €      €      €            €           U           %                      @                          !      "     #      $     %      &      '      (  Ì   )      *      +      ,      -      .      /      0      1       2       3       4      5  î    6  ¯    7  ÿ    8  ÿ    9                                                        0   `   @  `  °  €  `    `  p      
  °
  Ğ      No platform data
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
 ƒÅpUUh    èüÿÿÿƒÄé$  ƒÃh    S»êÿÿÿèüÿÿÿXZé$  ƒÅpUUh0   èüÿÿÿƒÄé$  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄé­  VƒÆUhJ   èüÿÿÿ¸
   èüÿÿÿƒÄé#  ÿt$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéß  ƒÃ‰T$S‹D$ƒÀpPh   èüÿÿÿ‹T$ƒÄéş  ÿt$hX   èüÿÿÿY[ƒÄ‰ğ[^_]Ã¶À‰T$P¶D$P¶D$Pÿt$h|   èüÿÿÿ‹T$ ƒÄ€|$Qt/¶$P¶D$P‹t$Vh¸   èüÿÿÿV¾íÿÿÿhä   èüÿÿÿƒÄë—¶$ƒèF<wÆ¶ÂƒÇ‰ƒ4  ‹O‘  R·WşÒRPWÿt$h  èüÿÿÿ“0  ‰Øè\  ‰ÆƒÄ…À…Œ   €=     …”   Çƒ8     é)ÿÿÿVƒÆÿt$hJ   èüÿÿÿ¸
   èüÿÿÿƒÄéë  ÿ4$ÿt$hJ   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéÈ  UƒÅÿt$hJ   èüÿÿÿ¸
   èüÿÿÿƒÄé  ÿt$h4  èüÿÿÿXZé¤şÿÿ1Ò‰Øè  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº	   è  ‰Øº
   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº    è  ‰Øº!   è  ‰Øº"   è  ‰Øº#   è  ‰Øº$   è  ‰Øº%   è  ‰Øº2   è  ‰Øº3   è  ‰Øº4   è  ‰Øº5   è  ‰Øº6   è  ‰Øº7   è  ‰Øº8   è  º9   ‰Øè  érıÿÿWƒÇÿt$hJ   èüÿÿÿ¸
   èüÿÿÿƒÄék	  «|  ƒÃpEPShš   èüÿÿÿ1ÀƒÄé;	  WƒÇÿt$hJ   èüÿÿÿ¸
   èüÿÿÿƒÄéP  ÿt$ÿt$hJ   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéÊ  SƒÃUh   èüÿÿÿ¸
   èüÿÿÿƒÄé]
  WƒÇÿt$h   èüÿÿÿ¸
   èüÿÿÿƒÄéà
  VƒÆWhJ   èüÿÿÿ¸
   èüÿÿÿƒÄé  WƒÇÿt$h   èüÿÿÿ¸
   èüÿÿÿƒÄéS  ƒÃp‰$USh±   èüÿÿÿ‹T$ƒÄéú
  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéØ  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄé(  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄé  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéx  ƒîP‰$ÿs|ÿs(Vh|  èüÿÿÿ‹T$ƒÄé±  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄé~  èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ     tvp5146             @  tvp5146m2           x   tvp5147                 tvp5147m1           x                                                                è      é       ê  €    à      è  `    é       ê  °    à      è      é       ê       à      è  `    é       ê  °    à                                   è      é       ê  €    à      è  `    é       ê  °    à      à                                                     À      `          €  `                                           °
          `                          
          0                                   Ğ      Ğ  à          °      NTSC                    é  0u                    Ğ  @        ÿ       PAL                           q                                     8-bit UYVY 4:2:2 Format         UYVY                debug license=GPL description=TVP514X linux decoder driver author=Texas Instruments parm=debug:Debug level (0-1) parmtype=debug:bool    @          ¤ÿ      GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ                                                 %     $   0   %     =   `   Ö     T   `  ˜     e   @       t   `  G                                	              
 ‹   °  Æ    ™   €        ²      @     Ã            Ï   P       à       H    
 ó   €  Ö     
  `  ¯       H   (    
 5    J     >  p   !    
 L  `      ^           Z  ‘   O    
 m  p  ¬     …  à   f   
 ¢     Ü    ³  F  “    
 É   
  £     Ú  Ù  !    
 ğ  °
        ú  ‡    
   Ğ  I    (    ë    
               <           P      €     _      
     s      x                   ~  @  `     “  x        ©      ˜     ¾     P     Ğ  À  @     à           ÷     )       5        (  N        =  k        V           d  @       v                                       µ             Î             Û             ğ                          $             6             N             i             q             z             ”             ›             °             É             ×     