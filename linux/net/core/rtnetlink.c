CPI_APEI_GHES) \
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
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
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

drivers/media/i2c/wm8739.o: $(deps_drivers/media/i2c/wm8739.o)

$(deps_drivers/media/i2c/wm8739.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/refcount.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
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
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/generated/bounds.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
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
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.ho build the kernel author=Joel Fernandes license=GPL v2 depends= retpoline=Y intree=Y name=kheaders vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII  kheaders.tar.xz           GNU  À       À                GNU ])u)UŞoÇŸÚC	a·¢¼¥        Linux                Linux                                   $                                                                      kheaders                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ñÿ       #            #     *             8            J       F    
 d   F       
 y   \       
             ñÿ   k   	    
 ´   t       
 Ì   €   	    
 á   ‰       
 ô   —   7    
                                 )             ?           N             U             a             l      #     x            Œ  0?;       ¤              kheaders.c ikheaders_read ikheaders_init kheaders_attr ikheaders_cleanup __UNIQUE_ID_description96 __UNIQUE_ID_author95 __UNIQUE_ID_license94 kheaders.mod.c __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module sysfs_create_bin_file cleanup_module memcpy kernel_kobj __fentry__ init_module kernel_headers_data kernel_headers_data_end sysfs_remove_bin_file      &     (     $     &     )          (          %     "     %          *                        Ì   '  ô  #   .symtab .strtab .shstrtab .rel.text .rel.init.text .rel.exit.text .rodata .rel__mcount_loc .modinfo .rodata.str1.1 .note.gnu.property .note.gnu.build-id .note.Linux .data .rel.data..ro_after_init .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   #                     	   @       H;                )             c   #                  %   	   @       ,H; 8               8             †                     4   	   @       dH;                C             •   0?;                O             Å?;                   K   	   @       |H;                \             Í?; Î                  e      2       ›@;                  t             ¬@; (                  ‡             Ô@; $                  š             ø@; 0                  ¦             (A;                    °             @A;                     ¬   	   @       ŒH;                É             €A;            @       Å   	   @       œH;                ã             €C;                    è      0       €C; &                 ñ              ¦C;                                  ¨C; °     !         	              XF; º                               ¬H;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  cmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        cmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        # SPDX-License-Identifier: GPL-2.0
# Any varying coverage in these files is non-deterministic
# and is generally not a function of system call inputs.
KCOV_INSTRUMENT		:= n

obj-y += mutex.o semaphore.o rwsem.o percpu-rwsem.o

# Avoid recursion lockdep -> KCSAN -> ... -> lockdep.
KCSAN_SANITIZE_lockdep.o := n

ifdef CONFIG_FUNCTION_TRACER
CFLAGS_REMOVE_lockdep.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_lockdep_proc.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_mutex-debug.o = $(CC_FLAGS_FTRACE)
endif

obj-$(CONFIG_DEBUG_IRQFLAGS) += irqflag-debug.o
obj-$(CONFIG_DEBUG_MUTEXES) += mutex-debug.o
obj-$(CONFIG_LOCKDEP) += lockdep.o
ifeq ($(CONFIG_PROC_FS),y)
obj-$(CONFIG_LOCKDEP) += lockdep_proc.o
endif
obj-$(CONFIG_SMP) += spinlock.o
obj-$(CONFIG_LOCK_SPIN_ON_OWNER) += osq_lock.o
obj-$(CONFIG_PROVE_LOCKING) += spinlock.o
obj-$(CONFIG_QUEUED_SPINLOCKS) += qspinlock.o
obj-$(CONFIG_RT_MUTEXES) += rtmutex_api.o
obj-$(CONFIG_PREEMPT_RT) += spinlock_rt.o ww_rt_mutex.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock_debug.o
obj-$(CONFIG_QUEUED_RWLOCKS) += qrwlock.o
obj-$(CONFIG_LOCK_TORTURE_TEST) += locktorture.o
obj-$(CONFIG_WW_MUTEX_SELFTEST) += test-ww_mutex.o
obj-$(CONFIG_LOCK_EVENT_COUNTS) += lock_events.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       // SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/export.h>
#include <linux/irqflags.h>

noinstr void warn_bogus_irq_restore(void)
{
	instrumentation_begin();
	WARN_ONCE(1, "raw_local_irq_restore() called with IRQs enabled\n");
	instrumentation_end();
}
EXPORT_SYMBOL(warn_bogus_irq_restore);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            // SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/export.h>
#include <linux/irqflags.h>

noinstr void warn_bogus_irq_restore(void)
{
	instrumentation_begin();
	WARN_ONCE(1, "raw_local_irq_restore() called with IRQs enabled\n");
	instrumentation_end();
}
EXPORT_SYMBOL(warn_bogus_irq_restore);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <waiman.long@hpe.com>
 */

/*
 * Collect locking event counts
 */
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/fs.h>

#include "lock_events.h"

#undef  LOCK_EVENT
#define LOCK_EVENT(name)	[LOCKEVENT_ ## name] = #name,

#define LOCK_EVENTS_DIR		"lock_event_counts"

/*
 * When CONFIG_LOCK_EVENT_COUNTS is enabled, event counts of different
 * types of locks will be reported under the <debugfs>/lock_event_counts/
 * directory. See lock_events_list.h for the list of available locking
 * events.
 *
 * Writing to the special ".reset_counts" file will reset all the above
 * locking event counts. This is a very slow operation and so should not
 * be done frequently.
 *
 * These event counts are implemented as per-cpu variables which are
 * summed and computed whenever the corresponding debugfs files are read. This
 * minimizes added overhead making the counts usable even in a production
 * environment.
 */
static const char * const lockevent_names[lockevent_num + 1] = {

#include "lock_events_list.h"

	[LOCKEVENT_reset_cnts] = ".reset_counts",
};

/*
 * Per-cpu counts
 */
DEFINE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * The lockevent_read() function can be overridden.
 */
ssize_t __weak lockevent_read(struct file *file, char __user *user_buf,
			      size_t count, loff_t *ppos)
{
	char buf[64];
	int cpu, id, len;
	u64 sum = 0;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	id = (long)file_inode(file)->i_private;

	if (id >= lockevent_num)
		return -EBADF;

	for_each_possible_cpu(cpu)
		sum += per_cpu(lockevents[id], cpu);
	len = snprintf(buf, sizeof(buf) - 1, "%llu\n", sum);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

/*
 * Function to handle write request
 *
 * When idx = reset_cnts, reset all the counts.
 */
static ssize_t lockevent_write(struct file *file, const char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	int cpu;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	if ((long)file_inode(file)->i_private != LOCKEVENT_reset_cnts)
		return count;

	for_each_possible_cpu(cpu) {
		int i;
		unsigned long *ptr = per_cpu_ptr(lockevents, cpu);

		for (i = 0 ; i < lockevent_num; i++)
			WRITE_ONCE(ptr[i], 0);
	}
	return count;
}

/*
 * Debugfs data structures
 */
static const struct file_operations fops_lockevent = {
	.read = lockevent_read,
	.write = lockevent_write,
	.llseek = default_llseek,
};

#ifdef CONFIG_PARAVIRT_SPINLOCKS
#include <asm/paravirt.h>

static bool __init skip_lockevent(const char *name)
{
	static int pv_on __initdata = -1;

	if (pv_on < 0)
		pv_on = !pv_is_native_spin_unlock();
	/*
	 * Skip PV qspinlock events on bare metal.
	 */
	if (!pv_on && !memcmp(name, "pv_", 3))
		return true;
	return false;
}
#else
static inline bool skip_lockevent(const char *name)
{
	return false;
}
#endif

/*
 * Initialize debugfs for the locking event counts.
 */
static int __init init_lockevent_counts(void)
{
	struct dentry *d_counts = debugfs_create_dir(LOCK_EVENTS_DIR, NULL);
	int i;

	if (!d_counts)
		goto out;

	/*
	 * Create the debugfs files
	 *
	 * As reading from and writing to the stat files can be slow, only
	 * root is allowed to do the read/write to limit impact to system
	 * performance.
	 */
	for (i = 0; i < lockevent_num; i++) {
		if (skip_lockevent(lockevent_names[i]))
			continue;
		if (!debugfs_create_file(lockevent_names[i], 0400, d_counts,
					 (void *)(long)i, &fops_lockevent))
			goto fail_undo;
	}

	if (!debugfs_create_file(lockevent_names[LOCKEVENT_reset_cnts], 0200,
				 d_counts, (void *)(long)LOCKEVENT_reset_cnts,
				 &fops_lockevent))
		goto fail_undo;

	return 0;
fail_undo:
	debugfs_remove_recursive(d_counts);
out:
	pr_warn("Could not create '%s' debugfs entries\n", LOCK_EVENTS_DIR);
	return -ENOMEM;
}
fs_initcall(init_lockevent_counts);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     Ù~ğ$!H§qšÚH¼µ’{rèMPĞ"Üì4EjáÊØĞT˜†r*6ô3fVT»#àÌë¶¿³#y¡«Äé¿\×ÿÆ‹dÄV#L•¸øÂ	gîAç90-;êâa¶§ÒZñÒV‡&EbÔFÙ ã†>F°<•¡‹’¢ˆßÌIT°nUéĞ+Ë˜Ñ)®%?ºZ¶—½%%ÓD”¦d@öÿ’-Øl‡ˆüqœõM¶%pcÈá‚ ™8w"óšöó÷S²²ecxåêbÔ-OR0\<mÅ‰/×Fâêì&İ0]ÌØw…Ûì'‹:» M“T;2İİ„r¬ ¿,ûlÚÎT0cBÉÛfS¤¤ôtLCF8ê<|õh½„ĞyŸ¦ã_=*¦OQ©¡¦Íwq0Î…é\¨‘êçÖ/ı†–İ‘o?«Û,ƒİ'v€è¡7	¼.5…Z<‘fwô2T™KŞZ~E0QÛ9#†‘DËÒ1fS$FÑU5H„ZNÎ‹W«İÌ¾ ·¬¥•:z9…ïCyœZ=âÿŞ\èÆÜ^ôğı>|Óq5Møó®D¿XûœïS±@T2»4PÂ ¬8 ^/Z];N·²¸†»Ò¥êHb‚ƒ·!°ÑÓdj HÕeãÎËù	Vë”±¥CİwÚÒ¸åm€¨'ZÔiŞ3 íì÷iĞ^‘{èÆ?sÉtâíQCÁe`i`x¸)©\ô»Ón§cHš	ESE¯F±7 –Ô×”¿ÊúMÑûA«–ïŸÆb$	 y·×‚W§4àèÒL ;™*ép@Ûšãş…¸÷…Š£¬F°HÙeØøu6ÙVš-xxL=}†lúSå@µxƒj?Æø`‘¹ú¿
'àñÅ
&lÏN|q™¶fçæy½à€VÂÒSò­µKÿïÓ–RadB/O¿`&.|ßtÆ0Rƒ nxÇ"ÜA#~ŸÕy9¹}³ŞĞJÈ„õt\U .Sño¨[:J€1ô­CÍÉj|x©RqÔ†tíZ0(cÌõ¿p<Rª­é-HéÉ.ëÅ–ös>Å?’Øbd$ÀD^EÓ,â”@6Ëjµp¯—hLº©ÖúRÌ«<Kˆ+O%İäŞ­ç Â ş-ö,7l#íPåÕĞ¹ŸºıÍxbapğ¾)–Ü°µ©Cç.zmv$däBÁ‘çjšVİrËËµ Šì¹}²:]R­-Ö$¹œßaŞ9ïÓ»#ü”¯V;Pl²*Ø€[Ü\+w;QÛòçÓîê 4íœÖ3Çã[ÏGúkèm¢%øäI%Ë5µñ_YJiËXè…À©f7mâ¥B‰l¸;õ¯c ÏĞ¿™32Xî™]öšm‡Í¹dØ¿è]êAÕ.k‡TvHÆ?^*;ùÉ»U#¿zÚ·<×AÉq2/„y¥+¾[e©-‹òºµä6â§§%âÙJ2KĞiÜpŞ0Çf@+0~ßĞêë	LwÙ.y™+gí§=…“ßÁôKQbıĞÇñ_Hk¤GD‰ÌæUÂÑ#âòj;aß\Ájáw\€Ø8®#ªD¼ÑÿóPAÇH¿İ],¯3lù,FÓÅõ{XöÀË™ø«gÍĞM‡â®ßÄéAÔp*A¦û,GêÊAî–M-·r	áÆÍã[÷×[Ó%†â¯ïXÌ1
u8´/™ÌIf‡Û¦Ëœ‹à³LhûOw%²(Ê*–ä	±>VıRF“6J~JküFÎËî»éJÅJÍ>¹òR	™cìJ-©1şY»•-ûc¥9ç³O‹¸x9RCôû°Ë·»¶ë1†¹)q=wô@x§	'yÓÛ;köA0@şñ÷ù'Ù_İ|~NJ‚Ï”Áî¦L³ŸÊ‰)—‚ô…gş$‘s·s'Iğ·xC0ÙPYğŞşŸØcÄ<â:*ÿÃ Ìô5Ç¨,f†A6À<MXLÊ›ÏîxĞëiîŒ%ñİ|G»°ò:¶»
d|5×J—ÿÍyvçJ0Íï9Up¬o”á+Š-:²­Rã­*x3
±^–›‘òÂ±g¹Gx(¡ÏXÛ2Ÿ”ëß¡«ßÉŸ­÷z”U—ø¡G0pV¬ysDí–:Ûh¡ÕK B)É¾ŒÌoWÍwÓdñ #p‡L^™í¼m¿QöqÄ!­œ¥rÆ?ˆS½áõ`ï;Yå–
¼ÂÑÒçòé(`S #5¦ ]+¨3ªyGcëÍK€äé	§ÀXú…Ä–h÷”g+–g1°‡¿yÁIylid©XVèÜ“<óSùg±=lõ÷Ç®´¤{ñüO+¸V>i

õÀWJßı&z‘øğÚc«óÁa=>¥9¿š<0–^JM(*Bo	@)TÎ-+?J4Ràº¡, 1$xE;r#å!/®¢x„ÑÙğä¨<½÷ÿ°Ÿ®,téûõ¢Èt¼‘–×m†«)P€ÏÒâ£µù";9«ÂøµÁ_q `-‹üş›ï72xè±½ŠP¼ÉÉa—£“lyâŸ`‡ùg*Å‰±a¬›„ä
E»øÇ/pı	ÃR–€é©N­Í’5j•İõÔàHäš‰îë^»[1[…âlä¶é¬1…ïBNO‰ûS„=L%ù¹Ö2*ä‰c4lêşÇ˜JÚí>Òœğ•‰6u­sìkÖ`1ù¬ëEè|‡–‘mŒ¦[cÅı ä‰'e`ù°EajDeAÌæ
ÂDlîŒ>éµMá û¸NˆÊ'Ó¾Ô	sßr/¹§A¬*èš§yD>ÊbC'ü	æ(&æ&^ Ù
‚æbéÒüv†'@)ÄßQ±‘Îç Â¹±E·»¦‹û™áÌ<ş®~TK¶ÑõfƒU#ñ¿'7¾•1fŒ±„ÎÏ.LL>.sîAÒ¶‡	e×){ƒ×æ]€´´>~xSq‡Êtº²Zb	q$Ú^Z¾ıW‘á|-1>Ü'FÒ/[’“$ M¡i¹«4*‚lú“ûr¤l.nú†1ìãpbêq,õkzÙ²ûA:¡0P‘ãoºI»3RÙ¤U6Ó0v~â`ª[Ş=}©kIz>Ûu7ÿ­P9_Y¸â=ÛnÂeº¡ró%„â³æË´t}‚¾­Ãsç%:¡(òo¡~w\Ó«çNŞ¥X»›#±ô‹ºÕåªW¿ Šq×F,Ô9¸½º›GvÒ¼êbÒ	ˆz¹±üı0–*ÌGŸØ4`£m½çOºÕ›â(N hğ ÕŠãú =Ï÷dşbá|‰¢ˆkáIãÖ<ŞŒb6o¼nÌ¦Ü³êwG‚%JÒ»xWG²Fƒo’â»´*>) Äù—Š:œ¤ïï­g¨'3ç‘†à%”)õ42	±#&wLêf³sÁÈ£[‘3™˜îÁ¥÷ñ÷7ì8®â(Ìm$Lå{j#OÍV!m #-a‘nvoE‰D‰ôy‚Hz¿¼7úÇh†µ9¨{?¼@ÎôQûáTÅ£%*şHÿ0áÛnÕßYaæòüŸIY™Fwÿ«o0ƒü=¥mU
ê@„O‰?ˆ,ìiØ*<6ÍñPtô—?›4éÿL™›ì¿áöNyÿ
çcòªí[_¦ÇáÀyÆ‚Şn
VûGGÑd˜ˆû†3]%Ø‡€CŠÄmØPh7§Êy$*A-¦#áp‡­´WÅ^L¢éW„2 çél«<î@ƒEüb]ÏÇY|!ƒ¬6P®‰Ri÷Ğ»r“Æ¿t°%‡&W&L£í…jY b“CB/£Ñ0@‚’©¤®À¤KGnŸûcî˜{ZñC€v,¾,L
‚²¦õÇÒ¥«y²š¤6‰‚HçB³øÔÂ+GÅqyæÀæìVaC”—1šHvçòø¯;\¹ıÔfZV&©êïw½‰¾şá$ú#½xÿFeAŒÕ4Ú(	âf¹È€2±¾Œƒ%âÍƒÃ*´¤~Õø½€üXÁk£¼7Ø]ƒF)K§±=Ãû.Î±‰½>“ã’\ÓçíXå"¸IkÑƒ§É°UFÊ”[Ã`Ò7¹;ğKq›qaäèj½™¸[²…ÕÓÂMŞÈ×3=×‚Œ–mETä¦#Œ÷·´ªÆ›VnÍ.!š  F[§Ğ(î{Úp°`óæö:»54r1L]£ŸáBı9E{Ä 'IÇöO}Tªy9Ä+[G´ŸŠÏ@´/ÿë‰ cİüoÊ3*Ñ	\ê•2×4Î–Ê*®ËÒÜ'­åÅSªğ3édeY¥©éæ@ÔÙû$Ç Xxmëa?•ş¼™O\OşTãİVœLE»P«Å÷ö‚WÊi‡Şñ¹*4'Æ~Öîg^•ñë6Zº+ìfñ[«B^Ÿyxiã·‡tŒŸßÑß Ğ,éq b±B%ØîŠ¤¡‘]d°ŠÙCËS
1¥û•ôséƒi`5hµÊ‰.=*×f«í×e•¤ª·|?tQÚÄ7?ÂE±Âø(H¤ãÀ—ÊØÎªfT«2è7Dõw£şæ£„ş»ã§w‹Ş5<…Mí—Ğ†\@ÿ°e•bõ25Ës—‘8AÚñdWÑ6ñ‡€"…Ê&»L†.Æb€º¯§) sk}À…°ö’zòº¬y
ïd‚óu×ôœoí“…qdpp061·\ñj§iWç®«XÚû0v×ß—ÆÕ6ˆæî”ÄÓ:w)\µÏd‚?„Š:ıÎk<Ñ™$@Ó3ƒ¿7@L#ãÿ¥Ç6)ÀŠòmJÖ(G‘˜Í(^QŒ]±—; †R÷è5‡vëIİ˜x|Úİƒ†Ê6äÊ#8×„f57èôMÒ=“¹ÈVRìzrwfÖR Xv9ãvƒÕêè‡æŒw<¾%Şõh@ó=0sL¸«,h>8;H/¦6ñçù`%qW¨ÅÀ£¤hÁOä-×ÇÒÃ;Êcú0tm0î¸ 2!>ô˜o‹µ%n@ä Öoqyrê¡N°õ0}TŞ­0Ü$‰:ñaûqÊ$:$hÍöoRğ×Èqy) >õª¶€W‹5mAN‰év¨F>ß§ÄÖEzş–w­Bviµå©ÙJ5‘[üvÿPzfZï–”!`ã2… ƒh³((Ğ‰O³`£,¦”æq£ûá.Ë%}Hdl*şH >ê½åíéåøXfÕ» 6|hecÄ–†!¾ÖwÅşû¢oú°Ù}wQQ€°µU»Ø÷ø3'ƒ€Ú”Á®ŸäPÒ–”Û½aGƒ¤!©‡´¸†ÖÊ\M(6$®FÜs3¸†.&z¾„ÏÖ,ˆíQ; ìrª)ãBB)Ç#ÌæúPfÁàW>Ã;m:dnéŠ%#,+[„B‚¼5?ddĞíí Ÿ%>Pı1R5¾ˆÅI#‰Ñ3+>xE«(ö ;{ä.Å®"Q³Àœæ»¯¬Ø@}é ĞfQ´nFåŞcS·Ì^¶Ç­õ#Uh*Eş©{õ¤²IıKÕËôO¡.]E•ªÑ…xÿ²şÒØµl—!0G†Ç:R)³×JÕ|º_ÅµÜër?ÏøNl¡*ÏïËø¹GõgÈ91^~qÏ'Ô¸’ú#ÌoyÂ/ôtÑºjÊ‡¨Šù“É>Õ¸ãH·‹ÏNacO7¡”^-£­‘s©¡M/s0’=­n4[Àè0Ù_!‰G˜¥öŞ2t|¸qçKZ­×ÊÃùìPeô®_ö @÷S2èS¿µğÜÊÿ£õ{Ù©»nŠµá£:º—b,	CqTGç‹æú;f]‘à©ÑJsGæ%+y¾óFÔDXèlÓæ-;<ñg2×…ÈE)v„ô‰‡ç”®oß'øò‰…êdò–åÃ§ÛD@ƒg…S·?Bı<×vU¼ÕOßÿ÷z®áÏv¿yØú¬’ZÂë"Y¨2"Ö;ëç]Xã@œöÒ
†U°,[/pIÌ5s‘g	ö>ûZV`ø ¹®Ä)4Ÿş€X#Ù¨ §ÜMáòD¶ÜÃØBv¦8.Ü®)’8ñ(€q?³›ÃHm;Ä ®“á	Şâôx¦ÑG¸şé¾%}ÙşèÕˆ¿iÎ$Ë–•G¢g»yJkV’í6o	ˆá•êüLßÕÙÜaàsÒ?#"ÂCë$;æG™i˜¡tAš.F@½ÄÙ¬ñHtÃÍ½Åkˆ0Q{9•¿«õ{µiï·ıÍ2(ÒPOôp"ä/Òµäu%\==]A¶°¡Mxü.®÷ñ¿”bÜHËÖj9K„şà7ñP [òÛ«ŞGéá#j§Ñ5;$| ®AAº·¢›c^78]ÉrÇë¹¯fu¦éqSÕRyÂPfUÀàÈ Ñ-ÌqæÊ¹DjúÿI\9W	…
ø†P¢î…È;'’Ï·ÔˆÇ‡.e¥üEø‹cÖ¤¡ªY2ÁvìéHSM¡xÍııN¶¢–bUz»¦iaŠ´›Èq¸õd½Ò¦øz$}Ğ†+JdLalŠ& C ±{~TÇ{‡ôQp¾GOáéX‚;²Ğßšóò`²2i£'¢Ş|3Œ_Åªñ¿BtÈ÷îÜëè7Oıáü¼`S'V_¸7ÃÍú;¨QÜ1ÏÏŠæt™Q	lş“}¹çh§^,öÕ½•<Ë5Cx:”`E¼Vedé¼ğ ½ó¿.úds¹îÂàïå'àÏ&UM‘Sõ†És®¶‚.-†[îmì·šxÇv,¬VéA*¢Gz£O¢#^¿b¶6\È>lõ³Üx0'ëNğÛè3‚&Ï},“/g5Ñf$·>|ù„%›ÙgİiSš“ÑD½_mÛ=tDqj	Ø:^*¸ …¾ùLlÔTWİ³çy ¹«Æıâ¥è0j£±ÍÔ‡Ù¹efı«ãşÛı>¯´ŸìÖLOøS£ô_Ş€‡úï«ñƒœ´Lß£ÛN™oH/Üıh¼³¾»?4ØÄpÃëiÚ(…·ÏÂŒ•no¾¦¤\Ó—NhšÀ–F(_tAŠé©«Àb&,=S‰hí]“KX{¹dúM7†3)Ğ<¥ƒÒßx—Š.èkcì ›¹¸³Ñ×³_w’ÔÂF³ƒxL7šø…£¶şÃ„ñivœw§@2ÑGúáÿ|–”øë0òÁ)}İ^ôí‘÷‘‡@IºRïn>qÕ*”÷ÌK‡)Ù³¬>ìQª¬¶Üq¢„rÄZ†DFÓlÅĞ–†Ã ÎÛNdç¹?l|I)¶jİ&(C–‡ı	Oº<	LoæŠj÷f¹d9Mïèó!ç©?oÊõÈ”«ÛÿöåI¶çë·Òsøf<Ñ±éÎaÖÛÑœƒœ/pKüèÁzšĞ¶^¡Ì¯)Œ}.{÷h>›U(­oMR6Z8è’ÁMaÈ;Ò€2@²Kğ¼ŒşœQAÙÕ½¢t&SÌe^ËT6 V•K…^•é^[¢dÎóšs–âª¶ñl¦e+ÅTwÜšil	"W±ÚÈØt¢lÕ˜+ÎzReQPåÓ¹\ÄI`p1İXùi”ÏÄ¨ÊåÏ¢¬;ÓÜZsL(Ã7Ls' S0ÇÑ*“çÔ¨øğÆ˜%!º–ù!Ebn»32Ô3ş®¶ê¥ôvÙñzº_/z.<­^]‘¦èF‰jM!WM¯‡EÍ¯!¾¡tûÀ=…ıæTıRAİG\iˆK3Ùê^]ğ"«2Ë5"ËÊ¨<â“@¨_’×&âÔĞ€Ø_”úx0Î¶ø;‚wÑĞÚ£ç×³Ü§Ni66h¢—dEõñËãÃ¥=k)Ôßóä|–®¸×DòÈŒŞ§ŞUÔRHşİ-zâ%ûêó‹ŸøöAëe«dƒ˜ıN.k?Ÿx<ÔkN×"ò›©æ&ÊÃ«"®\gÈBÚ†EŞVÙXT`uÿ¬@òîÇö GD
kĞ¶êbùO¤|a6Íy‚½ÄôA)3!é‚]æ.ìTóa„$ˆ:š®3B¬Üv%ãgkUI~Gª˜…¬X·òZÑ]šnhæ& š+BÁ½	 ¹»¡¥·kr}Êİ_ye­Ì“èÌQ[â©¼4}Úæ^æ¯2‘€’qÙ†:4ÉÀ4)à¹SmãÂzkw?ô4á|mªêm…t„åüÄãé™@:ïZá` ¹ØK$/!ıÔ4h=¢äå:xeÁ ÒÌNN^§öMOW³`k¿ñ!"SÈw‘P'T4à6YN¯3¯úe¡Á*É=X±{açò»òg Ô'`è	èÖ'mïŒØşVJKá‚¹oŞ”ºYc“¨––BO "zë=?P¹ñ!ymÚ‘“Ãğ“{ÿnè’ô¿ä™³êçòÏ‘[4çÖÎ-p9¢/ÁîƒåŞ£6×¬C¢€íÊ¨·\ó™ÈK€—Âa®×‡ãR<²>™^ÿ-Õwk“¯uËtĞmÒæy.ÏI…g“)qEÎ¹h¤«ÉW%%0$‚>¾?“‹d¹.8g²|Xo¾œmÎ÷étøfÒx³µŞaÌÉuÅ”óãlSÕÛtX=-0µ»ƒñÂïãˆ-@ç·ïœİ¶•Ñc(=s¥É8qz†ÄrÛ&_3ÍçDvjê%J fFAµ2®)l!Zkìd‹¢Ã•«¥~¥óÑÆ{ÖÕ8‰/È"I±ÁfGÎËJm&ĞÖ¿ìfC˜a^mÌäápS£Ï[÷@5úK‹ùAL•¿­§Viû!\µ:°óÂl"êÕ'Ëí
 Sä"%ÜÿÚÓêEÚ3Ö¿wŞêCx
İr%ÿˆÂ0¡ÏàLI_:(‰¨ª]úh—ÉÈH‹/†~d¶LnÊÂ<¹W|
gt;+u$ÊÍ¾¯¨øáÙ2x,]E£cµÈ\š­jÊyÜlp8I%œxML*İ­ˆü]Øia§•šY5İ1Ñ°®X%UË$ó}›¤NÖ-Åré«*[¹ójıoˆÔ.!zì‰J‚Éö Â¨ÚKø»ÏugA(ägÚÛÊ¨=´j¶hÇjü0Dõ:‹$~d^#şÜpiéûŒo#OÙ‰prÃGy;nÙj¬¿#=ÇN~À3}Iä@”æâ\÷´Yxê¹á²y"ˆ…À<Û¬^EŞÊ•wš„O@Ë÷­L¨ëåÁu/qTk¶ÆùÿD[ÍØªøŸÁÇ¹^†1İ9áıx ô{zJInı›wYìHş9¨Ñ‰‚„§–;ù‘u7ªu¢ğşkŒƒ*•úvÇ7·¥›q‡øÅøĞ ï¸T$Ò}¥qoåœ$Í³PÙHÃ\+uúPàJÁŠ°
:’öj€"œ-p:`ø7 zS¹ò¥»´-}âhôËZ©Bò2v§O‚x:,‰J—!'º0›iùí´aN²-l‚×öĞu.×]#8$¢ë"_9ü+âİ	Íçìzî3ºsy%èQ’Švgº÷KT¹?*‰i,µ79Ü€À'¸òz¨$ÉäA½úßp9¿‘ê¥Ì1<.˜ÌF¨}S„sxŸ;=âK?\6/Ç3°¸Qz~‚nœv¯¸¿ê¿—)‡Tn“Ä€tä?€Æ9ÒÑÅ>6„<.Îµ”	j¾dZ`ã_‹/Uã¼´³'é¼=B€r²`ÁFlin[’©á®34™NàôÂ„E”ùóJt;hy!úÁ‰Š³y²şœ^Ì„wR>x”èo,´¼Œ„aÇ"}«‹ÌÒ>—ÕÒM ¸×æWE6ñ_RV©ŞÂñ?iûôŞÑ—“¬şsâKŒø»
«afĞF¤:’`ÌÈ6*AsÛ˜ï)‰Á%ìÍ{Äg¾¾§~+°=J@õ‚¿‚ğQw1âK nV#GœŞ
*ĞvĞoç}ìœÜóÍ@e… 6:{ñÒÂfU[o¿Ï6¥ç¡¿ëÜ$‘(Ò1ßÉÃüz–äÖ›CtP}“1§ùMfbE‘B]ïh‹Œ™ßÈw	ô}zLwÎWİEúÜ±6²9üĞ—Ã®pŸ Ö)·=Ú¼„™I g?Ë19Ü°[VH–_UzŞï.oæèE_Iox?±…çé ØæĞ-Tuªó"k]zHòR[ŞÙi·Œ“QÇuÊ¦/,ù$ŠÅ-&áÉTPŞË|«¿~Û¬—¼ÍŠ	'¤L•wæüG(„£ ôn½`ªªã—?c³€7òª»
ûGuj¤[5¼Nğa±óÓ"ûˆ1%ïí/ÛˆÙLhoUM´.`Çñ•ã†cˆIIK“†ğ ş:)EªŒ5_ÊÀ/íÍİÈ|ebAWãÂ–ÒXH.H­3ÅÙ¥ÃˆBêñ®-WpMIÌMöõÓºğ\˜Èå„´oo¶ålOç‰r³ˆr÷Üä©³	£&zİn¹g&–9yÛ—š><•ŸØ½–#ÿY³ĞuN+™(´³ŞñíÔ»‚Á(è\\ìË,÷ôT^ø¥°n¿iÌ»1T³‚~…l…ùúM6¤|.Æ‡k/Ã©ÖdŒ×FGÜ‘³‡Î,Óğ!	‹jNLÏyâ‹İTmÌåĞ¤÷£ÔTë8ø¤C¢#£h{%?cQâÑSd)Ñ©˜8P&¢Ë]`=•ÂÛá´´9>¸–Dıùv¬v×M¼Š*•ÛJ3ä×1í¶½üq=ÛÒ¤´M9ê?ıâ/K¡ P¹¼À+¼;R`ó«¹täÌ¶Áåãìa¨‡{° ¯Óî¨ßÅº½7§Ûÿ_Ô-v„¦&Äœ»×"™±…í¿Ïm„ôµØá 6hòlÒìç€½W®HD8b ÷Å®H6Àè÷´AÛ-š´·÷_ è¶¾Šr|jæ*Øö2uáWµÖ!ŠşOÔ¢+½¹
	ÍşPK‚–óm~Y'o×#%ši¢S“èbı¬áhX×£N†„Á‘^¡5¥‘ÜÇ6&|°ók½ËkÇ>mZ;¸_(ğA…³$k2´!éi‚MƒN¯ªÂÁ/ôl/„®ÆXáİàÉn²&¡šøJWĞœó]S¿Ê‡Í÷°%Ëyp4èĞo¶î5…7›VOÏËs4é‡Î>Àl¬'©ßMA%{t'O#ª¼NÿşJğÜ†1n(¦²›ÎÊÈ½™^Á~ûjj~Z
3WœæÊ^•T‰%‘€—€+7Üãc{5Å•„´½¡uşK|XßHdÍhÔÃNïH
j)?ÀoˆŸ0QD$€Ö›µQ‘=P+vš7¦&Ík‚GÉŸÌq[¢L%vÏtÖøå"yoOÂÀZÁ·B;\ÌOáÊv…]”pU’QŠvi·<ã,ÓRû WGë»£ï*W‘Øs´49e0|>—vŞchâ?‚Œ““ˆ:=6Õ‚#'/Ê)Ûæúæ™¯r]± 6úš~~Nú’ÛRÆNM¡ŞÑƒwŞ-Â“X(r†§-0Må¦‰oÍ}aåâpÀŸom“ÈÔ9ØäçƒX6Un'v†n\ÁÀ³«T‡æËï¸ò0şMğtÍÕÀl?“İÂà©€F"_j©ê3®?EmbÛ”S Ê·y-§Š:{£¨cC=Ä ¯¦äë]»À
·³dÃÖds0×bDi–¡~ŠBjÉ]Šî®bÛ§%6=£æh˜Gô™«¹ï¶åT	Áï¹ì¥Q|KˆÊŸÍ±¬\…i[«ÆYEb}0Z¬D8É^|95ÀQ—–àÆÉ“â¨‘®2çS—Ã{ØpŸø„÷yln×TšÖ,ôAyqÌN¥bmƒ•ÕeœÉ²;\ç@¥Š°o˜QEş¹û  Ã[–‚È›áºÒMPG—‰Î-¾Ãİ½õôFÛ-}0#K¨› $uÚåÌÙ¬€ÊÖoÄ¶À'Õ@D¡R×­­°x:TnåiÕ«ºş5ƒ«·x*æã
ú—íöî]àœ$ì2šù·&`ÿÎ˜{#=w'î–ü¸ÄØä‘ç5«.BÍ¯çÃ=
Œ%o5¢²ó„~‹M[W‘G3šö	]êË8Ã¶mºîÏAó¹²ºóƒ0~ÛCèœï}£ÏØê+¥.Lâ÷= ûFø²x×	kóŞ!§GÀ?ÉT£qàÇ÷¶Q"‰DÍDäA$@ûâ­ğÎ}¤]PFQÿ$qc†VñÄ*óp~6ü‹-úø¯n¼0w“Ë	ó x-ØÌñßÙB0)DLÄD‰ÏÜØ#|İdô®Ë&sB|nÛ¨GNê'úìóÒö{‘”}ñéÆÙX`BÚ—èm”tÿùº+§œıƒ R°„K1¹±³ºk»÷‰+•ÜüùÌs’”J]úòëy”NP¿³7vÁD_¿)iğqÂŞÅê3½Ğ¥5¯ø	ÃÉe:
¶Ò¿üMÍif"~TğFšAÂˆ’Š¢	-«¨4$êòáÒ]ÿ¦>7ğn#0÷ËÓ{Ä-¾@«%•ç©DÀ0ød‘rœš!+8ßHs‘”r/?5ŠÜï×^¬óÌG¹ïVí_]ÕœcRV#ş™¦†|+“‰·Pë-¼Kƒ.]èlÆÿ\FƒQÃÄkÍGóÒ²tşSĞ[ÒáÍrÓÜ—jthÅşUíxÖ{ğœÕ'€î(Õ,it{Fa;ßt”œÙ”2ÃµÿÔ7"WÒäÌ†+Æ”¶Q*ï_¬2xãºóÆ<ŠÕ{5Ê7]›ôc§¢q)ù°Xœh^Ò‚nº—‡„˜Éíaºõ¬×ÖKmn§Éõˆ·Ã¿GuúòcÔÏŠËÛ£ê¤ªJšM!RŸEeĞé‰¹®;8 `´äux8{Ğ›º²É‡Û°íç˜2NéïzLÅâ‚=Òg,İ2¤B%ş­ZQ¦ùg&€Bå_û3à¶QáÔ­¦¶ÙÙìôbÃmM‡9EíXx)›æA¼*&Ó9ñ½ÓŠ(l}3úœ°·J÷"¹îYW|7³óâµ›*™ÓkmÚ‰„ÌâbÜDS€ûá‰‡4ãêp·«~/ìâ|¦¾c:³÷<w†q¦¾g°-Mˆø+|ÜI¸‚€YÁÔÀ¤B¾ìşGM´oÈ€N‚Gl}ƒæ^1Ú4²Œ—¶pyóĞ}W¶”Î–»Jªxå“ı «6Z­ráÙ‚yÆôş>iÃ­O(Cµóû±è…¤ØÕ¼Nr£Oœ§>²iË¿Òs¼´§ñRª2ªHk+Gi¿Z—ÁOjù‚ÿ.74ø*ìÃBşcşº½É[“	_¹§—J(ÓÍ›*¾æSãLÂIØñ©:ÜélqËA¶CiQ®Õ IF=™ğ(¤ó÷Å+}Æí^nhwÜ[P†8¹ÆÒÌ=6ÀŞÛ¸á¦ü9Kàp‰ºg¹¦RõıÒ†>#¬^!Šèr€dµx“|» H¤ÈüÈq2»‚@&ò1«WIÚ=~ê»KHpI®÷³—Èÿ¶Ç8<Ñşæl~Õ"J|*…fûÖ¸`Pl¬Vµ+¦¸ìk—´›l^jğC;ÿ»}ÿbÈX(…§‰ĞÁ¢—ãÊ‡xñ8ÇèÔZàÜêH`·ïJíw¯ßB²‰;¶G8[,ÂjL;@gñí´Å%!‡>Q¢¦ò¨ûgB.LŠ¢½c­&º™Ê~Ôê,ú_!=¨0Á¬×?©ûÙ\
rº;´7"ƒ3üZ´”ÒÔ™Ê ˆ»ıı–— vÊ"¹Oòå[ãKò ±Xğ4Ü4Ò/:ïê"İÂLíbƒâ@>ƒûs0Ò‰ÕK&Í‰Vß
'*}ä3©–WJ›Ù¤ª)P,ÚğZ…^/´ÊŒ™\QIÆjÃBÙNŠîŸåL‚°‡:ŒãLúzÊeITŸÂÏ˜0OƒÂO1°~hRÌÕeÊ0+Ÿh±swˆ-æP[ù«(³Å˜â¯œ@hR‚ĞÖW/T«N&èOdyÇ‚ÃE_ ÑÑè…x$Ù#ëÉˆŞ]	U°"ß3°§¦ê­nªŸN‡_¾àC¦b(Äø©Öó·uk®­"ºš‹¥âoÚ'6 oî¥UWALğªk°2æÌKWâFKò–'ûsÁ=@mÕáú»ÎÜ-µn÷õñ“tóãT¸µ#Èo¦¸Ïy!×u ŸŒÈ·”cÛLñìxYî*eÒLp úU9nvªØ U3.Ób†÷Œ‹qûoîŸ¹ä´¨gİ™ÀÉ©”F —3yRG³p¹z$–¯£ĞPF±£Vcp®Ùr ÿjÅËqĞ……U­İ>9ˆ"V/-%é®Œ6#õÁ×Ö–tÎGêw<sæÓø‹™ŸrüRÆŒ+÷K¢–Ğ¨·N¥¯êäF:ÄÈ0@y¹“rİ¿IµºNápÈ“y|D.…„D“uŠèII‡ŞcTUriî¾Sh¢â½“>Ç2’.ƒ‹ğï”ñd¡
®7ÇŠraA[nVb‚vÊ$W—QÕ÷¨&«/ xcçÎLë)©IñéÍÉÚa8¬¹Xõ‹C:Ç@ñˆL=Ü½^ÈçCÓ>X‘èÏA	vtkn„É=¤hSeğ1VóØ›2Èob×˜óTŠ–œZ“mvœ¿&	Ê@ËòX#/tZŸiE $a!O¬x”xÙ$ã\a jƒKßLË¯R¨¥#Û9­`D˜F<J*=XS…§¡9¸D7mÉÕ±€£Šm*ß¾ò”…¤f˜Á€dïè Šip_Z–T×¨¾ûú#âÛ€èó?zì)dœÉrÆ©ğòö%´±rU#ù^½È”–äü¹ıÖÎ¸±­/ùZ¢é‘yšat•í]Şj¦Õ·ÆiL
÷„ôL%âCS(;¾Î•²·! |æu(¬ídàã7ş`ú®t¡ÿÇ¡ı¬Ê4ÍXoEp„‘jub£¢NR}v|¯BVø3ê£b/jTÓVhC‚
ô¯uÖ¥íæ7pÉT ‰SıeÚ9Q§FPDãBº:¨Ôİl{‘œ9†(£&eEí‡Lıï†Õ
’¯½™w^$ÎmÉéšplü$0¤º	¨¨ã4ŠÖığ…•ñâğQIÅö™¤úÕ#ÚÕ÷¤Îád v{—vk¯ëğ5ı2¾³Ù(ƒn‡„ÏVtbÌ>„[v×]udúâÁêJ²øŠó4´÷ÌAj¤^·í¼U/z»‘vd[Ò@5ÉƒÆÅ—ÌÎÅ)««%í¨r­ÃòŠw+“©ÉûšÙfe¦[@}õ”(ØU¬g1€1$’I–h E¯¸ĞÚ"#¨|¾Ò¾>XzQL»üOjRİù…2›1‹"-:3BóP†í`¶”ÖâÏ:*–Ğğê¸Aƒşó¯j–+Â"ÂÜ‚]÷Boc%ı0Wxs§ôdÓÓ’ØAİ·RH24&ô’ÒëÅxmÜ_zÑË8|ÖêÏT¾'vx2=]u›=7ÿJh‹x÷]s4Š…Wîb°"aÔ)’T§•¡¡ô_¶´¯ÅÊ/;	PïpÍÛïX‰éoöTÊÑ¦Fúæ¾Å”=P˜·½üq¬‚)¬JÏ†[®TaçáÂ÷²©]^“À’ºÜ‡Á'Jå’¥Oø×7T÷Eö‹®ò? åS$Óípš¸ *5Ê¯OI¤ÆÍ2ï‹W›)İ]¶ï4šØu5#öÔıœR¨ÿ¤üµòÕ›ÖSxõò›”Ù¨k˜m±
ws¨;QjV# ;‘bÙ‡~ ¸Ûãµˆ9ÛÚê‘Á¶*,%a!GÍ€»1c:u‚€-f'+ò}¡r ûÂğİ u1âT/X	¿¦JH„,å’ºõûÖÑ›Öõ»Óbeó×ïÆ}BÙæ“ıãœ,Ü'TÑ­t^îÕë•™œhT})ÅôÇÜµ˜ß=æ¾cPÍœğæÙşîÇ5Ã’à½—:@.Äoó×óğÅâŸ³tµuQÀx}W¼w¦!jlÃı°üÎ?^M£ßñ¡†AFöõ(^ÔXÑvPünªœWkµúOfÊ2Ë»BÅL (/:;ş[¬vz•nŠ¡äÓ§®F#]/mÂ±´K±–m°_cç5§EIG‚›w²èğ@—I2‡H'zùó–G‹à… Ğq®ÀŒ!ÀûÈ´ †ğzïÿ Ú[wÔ5#±][eÿİÎ\Y°d•.…9+šŠå‚º·s±c³†1ËÕàı~=	ö­4{‡#[#Ù·s¹ NS²D¤ö"Ûq¡š†è€*öÜ€˜ÈÕÃ4
òxúí‰›’Q4œB29ıúË-Ö’ú‰~«^óŞ´`ÕñÙ›ïUŒ÷…Ó4t‚,%	#ø·ÿö÷7ÚÜl?®¼FÂX5&ºÁ
Ùl‹8rwÅ×%$ªù3&D¿+–­*ı×I‰l(ÒL:™Ò¯¢¼¬¬ñÏ[é9qMáşÎUñåÓVÂÒYÄ¦éŒıKÍ§”'4Ó‡(%Ë´Ÿlp²İ)ŞøÚ¦œÉnj™¾_zô¡#€RÜ|s¡¤T\|f£u©’À÷n™±ÎgÕŞ‡°Í=°=”{oÁ©åçUäÈè¿
NìƒÄaì$ëT"ŠSûÏmî`z?°gwX=4a\O‰á%Ÿ™]wL?òGÑÌşm˜İ*JyhÈ	?”}I§»}\W­rÂœ¹ùÆÂ¸iQ‚÷_RŒÏZ[Pmğ£V	ÖÂ%+Üüou2ó[2qá^–4]ËD"äVX§P¡JŒÔá¼rüûkzõÖî*ø¨NºX]MÏˆ~¶µ¸mÜÎ‡¯QÁ	Ç:Ô›¶7¿j‘•MYM9™Ó`t$¸ŒÕÒõ‘ÊäÇáø=Ã9	‰O+ÊıÍnwsh›h\ GÇëv1¨‘yÑÎkjù_À&ÎšÆşºÇÇŞ!½LÉ5$^ÕØZ°-—··³á¾)BeKÒ~ül*cOB‘º1	ùŸZD:1bE
õõA!"]ªİqÊñ$Î¶¥Î¹*‹:V4x¦`Lƒ­°§mHÕŒgo‚h³­¶(g]¥Sù5¯­¨PÏòŸÁ™5ãıÔö'	ü¨Ñßhò'$æøéA‡Mâ^¸„nŞ¨<Ôì,d+†®)fU‚¯^æ×~ÖëMYLóU¯‹Á'½Gû}aoİ´ĞXe‘å­—]‘”ä**7R¥cP´ğ€rÅ Œ+[2“9fZH—’	ŞÉ\‹_Âeøtƒï_‘~¡½t Û×|Á’'§£	–¯€¬¯!Gív¼)¼{İÌv	iiœ[Ú¹7°LzÅBll†‚Å³t‘6X†O¤ éÑıÙ«Qéæytû{ş¤˜…Ú ã~K¢ı¶ı×\Ìº˜BíÖv©Í…Â(ÇF7F¼W9eù#¬Öıte)o4’şñd¼‚G{·„‡n‘nnbtèbJŞu'^—çşÊ6ìWxŞØ-v éq„ˆœŠg“ôæÌjñv0²•‚^Õ¡€4âhİ„EƒËU×ª_ÄÎ¶¥µâ›ÚCdYº¤\'“’È\c”,ıöšé’Ä <[VCPòje7ÌNMÙƒ®@•.Ézß´Xºø1i¿&œ®Jà„›jHUçĞ§·n(Ş&VK·—T2ó$UŠè_Öª#óÍ’©Ÿ#Iƒì>{çM'mR-bÈ¡g¿iÒƒP`Ş- Ä’š)F7E¢XÀëu7rìğ xä¨n~ÿ "Y4œ¾/xÇÏd5ûlª_¤ÕàŞ/@ÛùbAB_¯ˆĞlƒbš[jû¦7*ÿIÚ…œ·ü¼Äv„îâàggÕ:9á¼‡" À—[¯¶Èo‘k²2êNÜB5´J¬Å0FyÍV†ÜZ²â”é|tc R;g1™€ÏÕé0Ò2 "½N$$*Aºo«)H¢{ÃVÂš$¡»Ñn>É©IV£‡Üda7?8²“ˆ<mU:V©Ïø7jQî‹×£î>fï%©+‚œ<ÙYU“6˜¨OûÛçKi¹B;=?µî¾Ë´`ßì¶_Bfä@ªr,iuteg_Xà©W9‚ç·`¹1]›BÍk	¦nÊ¸É!æ›Ø&	¦—¸)½ÌD­t^™¦¾¹×-~ş¥MH¹Q•½ÔÎ™M_ŠEèAÔ×áÍdH ƒ¯zÍÈ'`´€Íø]şæé·ÕÆ³fOöF8oMs^± §ï¤“ïªşkœ÷Ûâ¸ïÂ¬ÕvSã;
ÊnYÃx©›<{ï£]ef;d'NÒC¿øuÇÚ¶}Ì3ãCg¯…Dùø—.Aß6¦1{æÅWGfvùqi÷…‡[£½[Sep;wéîD¶>¶$”®ø®eACOZÉiÍ(¢Äq:6&“œ\ªë-ÕŸƒÌ²Y.©áşNŞÏ…!~²¦’‹t±Õ§‡D‚0¦ü?‹øÀgæY¿X@Œ½s„œªâ—_İ/ÃTĞN²Öy$I¼ÙÆ©ğÉ
‡•5Õõpgš_‹pÌ¹3N9øn$O±kú%	:eÃ‰¤ÁLWıC¢@ô wÌ5!Êú³V¸¨LÚ£Ò)¥­K¥ñyW»ŒzÎ×£]UÃû)?"a[ÈT‹rŒLO¿¡Õ5
Tårğë=4ºH‘fkªÌÆõÒ/¶/"\·Um0ÿÑ°ÃT­¬„­|OéO2¬sâzÖz_/û•È1 íÚgJìQëva6}Y¾ô¼óyĞªEÂúsäèmÖSy4nƒ¬éG²~òŸ¯u˜å—«ÓQ<ÃôÜÊİ«ï|â²yã}—W±¨àØÓô´6ñÏ¨­yÛù%
O%¾4â-âëŠ›½õ5¨FAšM¨ ùã¸_›ry!®ÜÖ¤Ò€ynÿMGéi°+ö µ-¸p;Ô L™’Ay_˜EÕq£‘ÿŠ¹qbúòÙ‰ğ‹l!9g >$,°½@Ù|G™˜•P$*@ÏR[÷»HUe	İ3{"R£Ø&R~‹—Í©§js¦çù+(¦gbÚğ¿–©0¾ÑBâU4Ó]9tP„ºÚãHŸï|‰\&-B;ıCH¨w_¢õx	˜î³.6¦¨]__%ùÆõ«m4[¼ÓmìˆHrr¢Xò÷QÂX#Ú …Õ)rLm·ËWÖÌ1MÉÊ·~@ÉòEàmkGK—×¦mhE¡Ğo:aÕ{›öSbP:¹¥à`¼Ò%¨™GËî:P®ÎëMŠ¸ôHµ™(†Ö€õmS*fb#b.\ÓŒ”¥«OÜ“°­yG/ü	vRÀeQE)óÉ£d}CükÓöº)v”hÒÊ¬¯©´±©ºw¸í*¥¤#Á+]x6ñPÃƒ`Â^ì= æ€›ñq¶"aÈk‹¿bb5Ş5éq³¿ÓXG}ïÌêÏøAC­’Íx¹—û„ğ+¥¬ÖTƒWOìèÉ™Mõã(»fŞO×—e=$O«My
Ã°RmjĞ±ÆÍ•™ÅÊ1s`–G¢ÖsºÎ§;´²83'†“ämğ¨àZhİşdÜÛÍ"ƒæ½âõ²Évª?vKS0òÜşœ¿¿:r¥ÀG‘c%XŞ1£‘!èrÄ?åPp˜¸ôèÃKŸ@×+œ¾ªæRÉ¬ÇP½çQ
7ÑQı½&"t%ù/3ÉgÑ™gÌ¼@ÆoT“Îv^ˆ™hÍ ©CëÀ ¤úÏùZ‚ãHlö!B´ó €§Ñ³ö`Øzö¿Èùªú%¥°}°gş†ÌZ:§I!X.Rs²[ÈYD†7e62ªJW–®ônIâÄ‡p‰ï
FlşDeàˆÚz
²#¾8]»N€š¬İó¹ÍølAvJBƒ^dJPµüÈK_¸eMöö¹HÆå¢f¼  g§˜Öõó§}€^¾Ï”µ&Kûà›#óãõİ Å[´rÃÓÉºÕ®šVäêÃ…ëcIççÙ-)„õUÖè}6æ¼¨oÿÆgTd¦<’GË†¾^ĞJÔÅ_ªHÒÅm§«B¥@C@°©[Ã®äÌ³tÑsF…­<¦]Äp ;:25ï-†!“ ”¶¡¸5Iâ¿ñSëGÂ=BÓçlVÉlQKÓ
1PG@pw‹ÿWÒQUèÃCe65a|—Š“Má‡^K)[Ãş0‘Ú(ëõ11ÛN5ªÈ¨ÆÍHçì³JÊ£¿	Ô^&'$di¬@¦siBºÿÓ‚›ğ'H²q
H;Ìcxíú7‹dw.ëÖ+Ï_H¾-˜ƒÜâ›$|ñg°µ`ùVÈãîú._ºQYUÉ‰? Ÿ·C)KuUïh8Ìe0¼‚dé÷õ¤o²õfp×ƒ§$ oÅB€LèÿGªŞÎ¡oBOwIı–ƒJ5Ê²v­ë°œIaÛ¯|l¢øºşp±Wó®qª+¸VK CõÏ{6È*…-/smR˜î¼’æÀk›-B¸Ğn{5qâŒ~/4Qaî}™­;k†=â<‰†­i25¯-Á·´Gœ q²Ä‘õWÒy™óùyÔ>$3n…ÄÓ¾/°Ï‘üûCe” õ7¨üRpm¸ºõ€şÏdïƒÌÆ¦ÇÑ÷şªßªxÒ^P…ìv…Â{İİÄw¼^È±s’£Fo?"†]fÍ÷ ü_MÎFı¶ä_˜º%Ôù‘–ÒzÀ%cWÃ …3#fÇÿd´§•gU±úbsRÎÆd´‘§%·ÒuŸ•ï=‰ÁµgË´Éhc‚ÉgŸEşSùıı°„0±ê¦bsu³_»çyõw¨,:môhœeÿgêX]ğ#k p“ºôŞÚÀš¤u]
ËØóC„qò¹'Ú¦Dà<ÁêG¾À¯Ú’«¬à†[cŸl³G¹Ì%sW;çÜAŸ‘ñjA4ÕĞU¯øŠ{KmCUÿ ÁÙ‰.SæáØrLi¼ss"äb¥9@§­\Œzsæ?;É’iíŸ¯ˆÁs¤T!¾pÅQ×Q¯§#[â¢à£e‚Öä;[ñÜê˜Ç–5›á…7ûÑDë\ÏHIS¦¶ñN¯/.ñªåİ‰š|Jƒéâ0ÚË6MQd"W}­CE“0LˆäÅëø¦2éélœ±9á½ëÁàKõu‹™µ¾ûs!ãğµæÀºô‰LqÁ¿‘!!ü¯f÷~£ï´°<–ºu½pOÆØGØÇg7uZö¸wM,İ#n–‡µ&V:Âyò §’Ã <\™Õ&P)¦êÛ{&€»ÌfImìr¹ZfËÎtòa[÷a.¡\ñÌ×2ö¯_À"P% \
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
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
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
    $(wildcard include/config/STACK_GROWSUP) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/MIGRATION) \
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
  include/linux/percpu-refcount.h \
  include/linux/bit_spinlock.h \
  include/linux/shrinker.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generatELF                            4     (               èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[ÃèüÿÿÿUW¶ùV4S»   ƒì‹¨”   ‰$‰ÈfÁè‰T$	ğ‰L$‰ù¾ğ‰è‰òèüÿÿÿ…Àtƒë„0   ‰ù‰ò‰èèüÿÿÿ…ÀuèƒÄ[^_]Ã´&    ´&    èüÿÿÿUWV‰ÆS»   ƒì‹€0  ‹4  ‹h|‹†8  ·I|‹@|‰,$·ø)û¿ €  9ûOß¿ €ÿÿ¯ÙÁûf9øGÇ‹¾”   ·À¯ÁÁø‰D$1À…í•À	Ğ…|   ‰Ø½   ¶Û€Ìf‰D$‰Ùº   ‰øèüÿÿÿ…Àtƒíuéé®   ¶    ‹T$‹®”   »   ‰Ğ¶ú€Ìf‰D$‰ùº   ‰èèüÿÿÿ…Àt
ƒëuéé   ‹$…ÀtAƒÄ[^_]Ã¶†@  ‰\$ƒÈÀ¶è¸   ‰Ã‰éº*   ‰øèüÿÿÿ…ÀtFƒëuééT   t& ¶¾@  ‹®”   »   ‰ùº*   ‰èèüÿÿÿ…Àt›ƒëuééw   ´&    v ‹\$‹¾”   éÿÿÿèüÿÿÿV‰ÆSƒú‡Ì   ˆ@  ‹€0  èüÿÿÿ‰Ã…Àt1Û‰Ø[^Ã‹†4  èüÿÿÿ…Àtê‰ğº   èEşÿÿ‰Ø[^Ãèüÿÿÿ1Òè4şÿÿ1ÀÃèüÿÿÿUWV1öS‰Ã‹@X…Àt¶0‹C‹P‹Rèüÿÿÿ%   =   „è   »ûÿÿÿ‰Ø[^_]Ã´&    ´&    èüÿÿÿUWVSƒì‹P(‹pú		˜ t^vLú
	˜ ulƒx|‹nÔ»   ÿƒÇ#ƒx|À0À…  ‰$¹…   ‰ú‰èèüÿÿÿ…Àt0ƒëuéé‰  ´&    ê	˜ ¸êÿÿÿƒúw†@ÿÿÿ1Òè[ıÿÿ1ÀƒÄ[^_]ÃƒÄ¸êÿÿÿ[^_]Ã                       `                      *                                                                                                                     P  `  °      6%s: Input: %d
 3%s: Invalid input %d.
 wm8775 èüÿÿÿVppS‰Ã¶€@  PVh    èüÿÿÿƒÀ   ‰òèüÿÿÿ1ÀƒÄ[^Ãÿt$·D$P‹D$ƒÀpPh    èüÿÿÿƒÄév   Fp‹\$jUPh    èüÿÿÿ‹¾”   ƒÄé   ƒÆpjWVh    èüÿÿÿƒÄés  j¿D$PFpPh    èüÿÿÿƒÄél  j¿D$PFpPh    èüÿÿÿƒÄé4  ƒÆpR»êÿÿÿVh   èüÿÿÿƒÄé'  ‹S·CŠ  Q QPÿ²   ‹CTÿ0h0   èüÿÿÿC¹À  ºD  èüÿÿÿ‰ÇƒÄ…Àu
»ôÿÿÿé–  ‰Ú¹@   ¯À   èüÿÿÿ1Éº   ‰èj Æ‡@  èüÿÿÿ¹		˜ ºğ   ‰èj j j jj jj j èüÿÿÿ¹	˜ ºğ   ƒÄ$‰‡0  ‰èj h Ï  j h  j hÿÿ  j j èüÿÿÿ¹	˜ ºğ   ƒÄ ‰‡4  ‰èj h €  j h  j hÿÿ  j j èüÿÿÿ¹
	˜ ºğ   ƒÄ ‰‡8  ‰èj jj jj jj j èüÿÿÿ‹Ÿü   ‰ol‰‡<  ƒÄ …Ût‰èèüÿÿÿé–  1Éº   ‰øè   1Éº   ‰øè   ¹!   º   ‰øè   ¹  º   ‰øè   ‰ø1Éº   è   ‰ğ„À…   ‰ø¹Ô  º   è   ‰ø¹Ô  º   è   ‰ø¹¿  º   è   ‰ø¹…  º   è   ‰ø¹¢   º   è   ‰ø¹   º   è   ‰ø¹z   º   è   ¹  º   ‰øè   é–  ‰ø¹»  º   è   ‹‡<  º   ƒx|‰øÉ0ÉÁ…  è   ‰ø¹¢   º   è   ‰ø¹   º   è   ‰ø¹û   º   è   º   ‰øèŒ   é–  ƒîPjÿt$Vh    èüÿÿÿ1ÀƒÄé3  3%s: I2C: cannot write %03x to register R%d
   6%s %d-%04x: chip found @ 0x%02x (%s)
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ          wm8775                                                          À   €   `                                                              P                                                                                                              °  license=GPL author=Ulf Eklund, Hans Verkuil description=wm8775 driver  GCC: (GNU) 11.2.0             GNU  À       À                                  ñÿ                            
                                       	        4    	               *       b     >   4   $    	 W      o    h   X   x    	 ~      P        Ğ       	 ¥   P       ¸   `  A     Å   ì   ¡   	 ×   @         â   ğ        ò   °                	                          &      €     4      
     G      0                   Q  À   0     a  €   ,     r  `        ƒ           š           °  ,        Ë             Ö             ô                                       0             J             [             t                          –             ³             Å             Ó             ç           ó                   
           0      wm8775.c wm8775_remove wm8775_log_status wm8775_write.isra.0 wm8775_write.isra.0.cold wm8775_set_audio wm8775_set_audio.cold wm8775_s_routing wm8775_s_routing.cold wm8775_s_frequency wm8775_probe wm8775_probe.cold wm8775_ops wm8775_ctrl_ops wm8775_s_ctrl wm8775_s_ctrl.cold wm8775_driver_init wm8775_driver wm8775_driver_exit wm8775_id wm8775_core_ops wm8775_tuner_ops wm8775_audio_ops __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free _printk v4l2_ctrl_handler_log_status i2c_smbus_write_byte_data v4l2_ctrl_g_ctrl __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__wm8775_id_device_table     "     #     $  !   "  Z   '  r   '  ‘   "     '  ^  '  ¢  '  Ô  '    "    (  7  (  Q  "  a  "    )  ±  "  ÿ  '  g     .    l    °    â        ‘                       h                                                       "          %  (   &  G     L   %  d     i   %  ƒ     ˆ   %  ¡     ¦   %  ¿     Ä   %  Û     à   %        %  "  *  <    G  +  ^  ,  h      -  ‰    ²  -  ¼    å  -  ï      -  ,  $  ?    M    ^    o    }    ˜    ©    º    Ë    Ü    í    ş        %    E    V    g    x    „    ˜      %  T     w          ®     Ì     è     5    1        ‰    §       "          .     /          1  @     D     H     h     ˆ     À     ø      .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .comment .note.GNU-stack .note.gnu.property                                                         @   M                    	   @       ”  Ø               )                €                   %   	   @       l                  /                                   8                $                  4   	   @       Œ  H               E      2       D  1                 X             u  «                 T   	   @       Ô  ø     	         g      2          X                 z             x                    v   	   @       Ì                  ‰             Œ  
                  …   	   @       ì                 ˜                ü                   ”   	   @       ü  8                             œ	  F                  ©      0       â	                   ²              õ	                     Â             ø	  (                                 
  @     "         	              `  3                               4  Õ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ed/uapi/asm/resource.h \
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
		