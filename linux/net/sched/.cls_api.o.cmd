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
    $(wildcard include/config/F�:�=#������i�P@�|2�bb�U�i.�;7ǖ�WN3'����87��JB��he.�x�Z��`��-'D�	5|3��"�d���B�g���Y��.����ȗJ���=S
�_S-`�Գ��j�����6�ځ�m,�%�D�0�.=�4A�fAz�	� Q�����}��Q��ėڼ4�m����FK�I� ��ڎZ2gV����R�=�80��{��]����2�꬗�.G����$�3^j�" �e��vZ1�UP51�MA<p�Ԋ���/��8��b�w���K����|s�o[^�EPT"�"�YmIPD��.�wW^�x6�0`
z`�O%9��4E�m��T�TM��_��]Be-�^z��q��ۊ���	��hl��^!�>w�T���^�	#cX���K��3��Sƀ�=*<d4+��U~�|�U��B4��ݤ۝��m;�{O;)��Bu(imJ{6�&�5����١*q���AD��fgF���]}F���Y,ɟ�3Kne��V\��A��P�L��iф`u����[V[��9*�ԓe�`(+�[HPw�K�HH)��iՕ��
��Nm^��7�/y��av�i�����1���gei�����6W������}���S�S�a���G#3v�f�Y�0u��۾>�`����B�΁�o�j!���>�8���~�NK���G��X�E�}�����A����x#�FٱGT-��t�H�H4� [vN�,\��i�ު
�@��o���X��3ήZ	��� h.^����.Vk. �(��#~3 ��.����ʓ�9^�|s*�=�[�8�V܍A�>�YB(�;G�n�ⓣ��5W���VZ�bjf��.�W�z���,_3��I�.��(��\m�^?ԼP��q�H�1'2YX�և�rV��L��(I�3W�`����@�W��s��ѷ��JuRv��zFnsqT�H���z�&�#Z�����Ur��׻"b�z��L���ٕ�"`�-3hiד�rm��`Az��2���5�ko�ars���_�3�'.�w<�F3]�]B�����"�tp��j�Ӷ��@��	��\J_6��Xsq>>�Ⴆ�^���ϧ����Ҷ
c9��p�Qf���'b_c���0Ǉ:QF5[�)��%�8�ʀ+���V�B���U�_|rξd����i��\U�F�s�Ҵm��<��?���+�I��Aը[�93i���r��Lc���+��(�J�#W�Z��e�.%E!�Qt�{����ж'~��Y��-��=�)�%�Z�,����D�	����H�2&��T��Z!�Eo�����J�V������6��^��1���;"�}���<[��J�{�xrm*��9Od�!w���ۡ=M-�)��,��¤�K��q�����VӅY�w��ض�x���&?r��s�p�	���Q\\�����[�j��YGg�녹�V6P<N���u�3��U�Y��엄���ǖ��^?���q������MD��nؿ�`��f�����G-TE,s��:�Ck�ʷXc=d"(�7s'gƾ�^g3����k��%���1u����u�t��c�a�� �K�t��ȨnA\Aaq��f���{����������WW��k+������_F�/�ƜZ;�8�A\l�;�<��؟��e� ��V�Y����iyU��N5��x��r��4���GuNĽњ�$����c�����ڇS�eE�p��2���\��.A"��Lݥ�)���z��9��jgѫg�I�ȠS嚐P�z�f5�o�k������r�1�_֒j� ����|!aM$̭�=0�#��z����0ՕYX�C��ۙ�<�[�6����P��!i���� �oD��rMԸ_o�X�ֺ�L�g pc���h�)����Jy85�B�� ��}>�u���i�M�a�:ϕ��tz��%1��:��o��j�T�؆�J9��ı�tق��[Fꄚ��UIU�J��B*�6^JQ�i�,���B�(��D�v����?�قm��]6G�=�zL^�K�n�`�4ҋ�K� pv�- u��hAѴ(��~���~�GL7O<4׏+t[g��bSu��
��},�i��kRqI���p�+�];����k>:Gv����x�1=�T�3e����r�@�e���IQ�
�Y7ՔG�qW�Ƒ��b������\,,�����$��u�u���������� ���
����Rj�߻ߘ�Ҋ�3|� ��^t/�1t�1��U�јZ��9��T��h$�aG^�5��H#CX8��U�ӷk�C=X���2���줄/i�-[��W�L��P��#��*����N�{��W�ؒ�Ղ�)�ܷ��5C� miy�b�^�%�[
�6F��z�0�j��g�O#�k�Z}�]-��<K�e�y(h�'tⱩck���X.9�qo
b0.�;�:YXv]�f�_"�:�GUR ��v0As���{����y-������ !̸i:�Ѵ�Iz����M�<d��I<�]����p�z��u���H��P� dNX3��M��w�H�p�� ݘLt�QUݱ�q��iy���A��ɵ'r��uwp�[�ׇ������DJV�3��ܽ�mB�Ud �S�^z6*!�O>�8�ҶżpD��?�ړ���]q��#S��c�T�1��,-��Yz�D�����u�&�������1d��*��u�ҙ�#aS~�	���7���1�H�����Z
G*H�N��pū�^��%�^���W��!B�u�t]d���:BWi���p�֠�$)����M�꒗�����2�{�j��G�{��ة�ԏMZ�$�RPt� };�u��,�����4�����߂C)Uan��c�i��'O;S2��'���[���k���5��v�/<\%��_*�&�#L�Eu�IH���pM��ͦ��X{�tW(6Ho�P�w�^nQ,u��Ґv�W7@;��g;����m��Q���^�)�6�5���e4j(a�����)U�A�Z̑�C�\ ���ďYj9�xt�)^�>�jp:
�'�gO��G�ǥ��F��Q����N��q���^���N}���If��W��CD�.�Ʊ���|ԧ�#���8{
�/$�8��\b](�jF���)¿��1�1=�B�IO�ȃ���*���O������+G������)�Z2)�:2�*�k����MR��������+D�|Ig�~�3��c��A1����DM��y��͓��n�a5"u׮����kO�Ũ�(����_<�|W�T��\�H����zNÚ�VD|j!I:2:W��/G��P9�3q?7�5i��\#��k���F�+<"�g)��ԷV6�d�*��:\9K�n?�G˷ݬ�Nb�G�bz[�J F��ɱ1d�$uB�a���fcg�r��¿�7H�l���C�� ��z�.:Ň
{S,��y�)��V,s��k#�$ Eq�%׆FX���;����3��˜`����f�LC����e�{�8��f!��'I�
���M �ڏ�+�+�,�IIM��/���Ãt��@L�;l�ې7%U^=��tr��.˩�)�[��"���ۭt���ؓF�ҫ��k�zhp��yF��?�,~���0�99��;���xVz��������#�+�����+1*a x/~�n���Į4]�n���A�W�����S>��h�8"�ō,��r�AՂrW�p�H:Sєlb�g_�kַ�p^�h��Z|������v5���3_|��.�/I�0_T,Ru��K�����0�l�@�����������Jː��i����x�٢ �ͳ��UM�?O�,R切G�td���Bj>�pGp[ge3� ہߐ�z\���i�⶷�Z���o�%�d�_���}v�R�afz�a�]�1�S怚��[;5���7����?�{{Bo��Z�h�H��h�Jި:�I���MQ����������U�����₄�ng�
�}kZ|�I���+�?��&�����'�����\o��-o��x-����7��ꓨ��`��Z�6�;\���������+�נ����Y��H���RcS��f���J`=�ۮg�ޑm�{���T�N�z���K�Q�n�ᕁFB&�h#؎x��)�H�i9�f�d�����K�����9g�!�������ĥ�$��3�X���[��Z���`��]���6)�4�?J��' ��Z��\D���ֻ���-���鵢��\Az�+�up3�̈́I��~Cij_L����ր�ЫM��
�5�4.�����; �Β��N[C��D�N���9�DײU�~����E��f>��Yپ&]��Ǎ戀�i��{n�O�p]ʛ���z� �Eɫ��W�"]/P�򋓓�q�J5Aa���b���X����@#�4�e�@4��Q��9ٻ����^�>������Kn�>n��/Ss ��^�lF��l�1��	����l���EV��E�3Qzs7b@&>1�������W��.��Km�4<�t��k�0�L5��L�mC��v���3`�M0i礏�(�*��A�ɽL�PI&V�_K�Q�&լ�_'�5���^���f�����jN� <{�4Y8t^�7:���L���x�-J)�Z�ǎJ�L�J��m�|V��K��xt>��M��#�$��	�>�
Ok�
�>���&�?-?r�i��i��׿#O�Xe�ԁ�I���f�)�Rk��������V��%Hw�&��,=��������.w�`�ȵ?�6q���d����hZ�����@	��͞������R��Oɑ��+%��#�_N����;��S����K2O-��l��xf�a�4�W�{��IY��7E���� ��g�B
�D�`�r�p��S�"!J
�/������8i�G	�33ϗ����J�}A3/#�$�c9���^oȼ�B�s��-��?!GQ@|���&��,�d�M!G�����;"��ܚ˾��^���x6�V��i�`��I�#,ΕG���\�&�]����d�_�?}Kïdo��"�+��q�;��!%�+�K^�#�^��?�?��f'�7��%��'$������"n`U�J��r4��������1��ہ�ЌbV��'�8�j��Ņ/'�mN����	U 2LjF�J����hBA� �r�\hڧ8U��&��� 3�
�̎��-�]k	��욛��%��Q��{��튙���
�!?��"__�1	�6=f���9j�K�H�T}�ikz6
ܪd�t�<7S�Y�ړʷ&	1M�*�Ʌ�E�p��&�!v��3� � =u����G���:[}#?B�I���~%����� �����ٽݖ�?�Uu��$��ߢҾ��v1�D��j��ڏKi�:��O�����WtJNĥ=H�s��jMϞ�`��f����5�.��IP�0�Q×��5��$�fjB��nW��
Q8s��	|Ti���8��;���&��>�7e�����N+	L�tɰZ���z���!͂.]�3%`�kݵHy�M7׳�� DR�a8��1Nmh��/�yr2bV#��ܡ��q�irMd�nr�)��D��KH�G�>E��!.�L�c4�\������TK�kQ�`��+��V��K����E����fxDZ���2p����*k2h��\%NjnX�|��nΟ�i'�ڇ\R\�$@y�x�U�H	t�	���Z%�O�p�����rv����@��������tF��(;9���Q���V��>0x�K̕�.$N���*� 5��-8-�����ܻK�}ftb����.-��Qg���w���S'rPo���,����Ba�dq�mȭ�����oa_��)Tt^T[H��:�հeZ���[A���A<���%��{�}_3�:��h�p�<���k����-�'�C���Z�yLw���Z�_��7��(��S�T�W�# ��K�v+)��m&���\>�"�D2E��Bv�i�������7C�qwzj�:F�$yU4����K
a$��� x5�~��%���K�23�Y�ւ�B���? �w܁����ӵ��"�>��hOT%���1o���qNsVSm9����Y�jI�
m��'`��S������NvW:C���P1m�0��r��绝�	}G��J^L��v_2����%�R����V�H���%�s�����г�#��н�Tk�P%9,��O��������nmJ��e;.>(��Gq܆\�n/[��ڹ7� �S��r'�dd\�]��T.|M�}�5�p�
U"�~	b�	ț�b� ��ʴa?tJ�����zm�R�����n���X�T�^>W"L�<�U����E��?��Ҕ�T�"���Lb�o����YaԻ���7Ș����MJ�7[y�99V{Z�U4YB��
��.�����O }�UJ8'a&�E[����e���sGHێ1خs)<"}�;�U�*򌿮g�r7G�?PǘG����nv��th�D�S0‗L�~.Ry�~Βd+��vK���#�/	&W︷���D�6��`�(���S9�$��>��2[���6�Ӿ	�ahk��R���^��'������9Z057XÎٖ1/s�TmX�P��Zʂx�=b�R\lw=�� ѭ RΫ�����j�/I��"���G��}!�F#o�[9���- ��[��I��Sx������鎵��D��_��ǊAC��:ιʅ��C�?�dQ��)�_rFZ!��d�4Z���!Nħw4����\-ivb���.��=oI��	5Q#��,���(��Xu���}��HA���&y��_ށx����xS�݈��|%[d�QwyJ�����ݵ�A���9�T>��1��G��e}T��j.�ج"i�UD�⹶�-�(N�^��>�W�wM#�`�������
�lC���4��A"�ZhK@z��1��{�w;��!`1����O,��ayS��a�̩�M�;H�ϡᵘH)�<qey��o�����>��<��(B-�Zx��U�\}&��F��'Ҙ�!�Ŗ�*wr������&K⠮H<��q�F�lэ\Ŭ�5�5��
�Ö}��U��id�H?�_�o����^"�l�tnH��R`U�AU��d��]��Ԝ�>5>.9*�.�P4�����D���ѫ��>�f�h-���ERW��ɖ���a��koX��G,�=��G@3�r3��&�!���MU ���)	�!���<�T!�����(Mw�>���9�o��1��/bk�b���g#��Ky��G��%\�]O����̞�'���@|��u&�l.\<;��NL�\r�r�cL�$��~��3uxt5��ѧ'e��K8�}�r/��d<���駾�Q6����z�D1_D<7�OBM�+��Qr𤾋�Lk��$K�*����,^M��4��,�FŤ%��Ы���ŝ�S�2�0q��u �~��#�'�{~���a*�VB��c��Ӎ���h_y#A{���� �+�n���&9���$OY�_�M�N�Vp.+&,���CS������z���V8N�z�1'��y��5v�#��S2�(e�WI��{x���̊���l&C�|G��t ��^e��\�O&5�5�_�V-\����EĻ#�:���	u��dVVs�ͳ�6�~�t1�ԩ�N��� ��)'ο<��:8*��%
7�KO�!���eC���|�o�z�&�ϊ͜P#��SaP��Z��$$�Sx.�7����ZOVoa^b,��Y�9*��~y��"��V���M>*�S�.�4ũ}'��A���\L��"���5ڿ�^�.����*���d��;>^�GLr�m��5�*�&��$6�Meؚ��?[i?/�z�:D�#�l�����s�-�ө׋ �6������D��_jX?���;P4���op���c������U��Iab�F�G��v4+NN�^M�����؎�L�gb���#F�2d9۞2�B�?[K�R�BϷ����Y�\H��乎M����m��E��^�'��
����r;�����L��W�p6��!Ľӡ��"_�r�;S��_�2��ho[�~$�sX���R���hۿ���5n��[��^a�>]�_������)��.?�(SޚY�)�4�$�l�8�WQ�:oyAk�/7\��O ����E:�C�|��q�X9I><��/�7ې���5j��sjȂ��MCڙ�0٨�U\�n�p��#ȏ�#�3� e��Mͱ,S�:���A�jdmh|��-��<I��Fgh����<F�.�%2��ˎ����l�]�� �,��v�v�L�!+���0���W��rD���Gǘ���+\�̃�xn-ن�zK�"�O�~�-�d(�*l�r1���j~I�P��s:$|�N�{�#����:�,����x�C8�n>�����~�2��=���'�i�&$��zg��5|R�#R�즋J��R=BU�K$�
ݡ���z�Q�b�ƉK�s_���N�
\Ώ^��<��퀹��|�(�~L$��x1;�Q��V�s�/]��뚞����tXn��������(�"Y��[�tG*���JkQ��_����Ey�pDS�'�����Ǯ��b2��� &�W��9�x	�2(¸^L���Nt8�)z�� �s.����=���>�IgB���iL�R=>���z�����L-V�V�k�!HKeߌvN�G�Q1ҷ�86fC���t�_�O���"W�R��I(zt$�
|	:�q] i9�_R�a�F$i�F���HF-~왏c߲.
vJ�&ǂЈ��-#���8�7z�}0��'�γ�j��4�J�V�;�1i�w��;�w���Q�ڗic�6z+U˰�~%@j�Όͭk(��:P$o	]m@r������#��6#��,G���4u��3<AїP1���l��V	�$�F�ʛ ��3��2Цh��P����cڻ,�U&C�\�1L�%i�A��k;�N8�^��~#�L�rq�@��h�Z��œ� J�S�#ZR�)���E��-�_��ԂSPLr�X��nL�R��G��ϭ���\�?�m-�E� ��c]�d5�ӣ���Ӥ�$�_�X�����]���C�T-���d��&��7{��»�њ9�U�y��2�hdE�ȻY=�r��S暃P!(�8Ft�W��u̜ݻ�7��[&K�:�.R�i?��]���)�EF�
i�I@�" ��IA�M��)}�ۂF��^5�o�.�G �k �������6��i���=�z����U!ؘ�z�:"���.�M���h��'��,���!�����>�,���Nj��{sv05��s0x�7S��h0��z͋�/�B�|}N�:k-%������&�m.ǎg4`%� ��
��r�.O�A��7��Y6�"��p�pP���p�('c���}���"e���R��+8�՚?�jZ,�t�����36���Px�����Ј:�M�e:�Xi��6��N<�	Y�y���7��e��ܬu=��n�X�;y�S�#VK�R����W�Ռ��^���� �7�r�YJ�$u���章Ȯ0�ԃ�(�M���E��no�
0���d�Y&V����[b��*x�꾁){�*%���1�N�?�1W�s�.�B�iT�d(����3�%5����i&||���>�Q�
>��o	�k:츀�_�b!����*�=���5S{K;�J�·UQ�����(��Q��<͵g�P#�Y[��`������g��p�xA;��hڮ���Lٙ�Cgw���ޛ�ψ�l�:6-:R���M03f4���i��s�X��ƹ@�"J��?�x;P1��9���U #2����+���qD<���5B�q��Ȇ%*z�[����Lc��"��E���9���v����o˅��{iilO�O,]S��2����aJ�[�1��W(.���]������s�o�sۜ0 -ן���"�y���x��r�3�2�\>V���
��8۱;:p�ǙnV�� z�(Z�MV��f�XA��c�������eݨ��̹���%*�޶���E�K�Sh��׹�m�!��	�J�4�i�_�ʴv���1�l�th���R��"X�Jx'��VP�*�8�jKT�aΔ{f>�h��!�?�	)l��j�f�˰��F��P1����]�i��9x���t����7��0� L���qQ�.|�ǘ9��"��5�9���P���Q���p#?#����Zs%I+~?g��G���S��0=���y�}|k<�s�:m�
�{ƌ�ϸ=�Ԫv���*�Tӏ�%Sl���TR�f�%D�T�]��v��tZ4��*n�A5 �>����K-U����z�	s�v|6M�N��۰�?ύxz���S�l����[����N����d�`*C�p�v?�q�N�tB'���QM�s6�tg.� �UŴ��z�`�H�yd1�.�'5�ǻ�K`q��3�z��Ũ `M��\u��)*��1,*�n�j�\jf*��JF@/�Mϊ@�Ѡ8{�\�l�_'5��7@>��OcAZ?����Ja�)���`�oi=sa����v'J��܎�№��gBp�8�/Y��v��-��pX�a_����_S1��p���b���0S 0'�d�`Q|Q$���̼RoW���T�"C��=Q��Ga����d�f0�g���dC�T�a��ja' 5����d?8T������y����=�JY�[����`��tmf�#��R����~T$�?�s��6�����̋Ò�()gq=�vl�����
2�)*���< �E3�k7U���Q6�<'�����8�5��/�[��� +�c�O���?Ksg�C��M���8!�>.�`��P5��1猄��/�
��JMo �K[f�����������}���vK2H�`�딈�|<��o	\e6J���\�\���I��b���ݧ�`��`ړ�7�u�	��͎��O,1Zt��`���!�i\�1�q�����agP��.-�Ψ�!����L�!�� ���oٍ�!��ҭ-�/��/��Ty�,��
�FL��T7�����N��g���|�FI�$�o�:��!��5�:��Ir|=;�A��Y$�(��]�=���>��U?mŇ���^(j�k�:�)af���`�6;I�������tE#�ܝû󉱼�r��NR�,L���RWH�Q��Ap|�V����$%�J��/܆FL�2�=T=s:#�zAk�$�R���g��+���.}��
� `[@��Ab�4RQi���wk�>-����h"�U(��ܚA*#��h�}a%����:^���N�K��U�py�91CG5���JF�`l5��r�\ `c�3�ȳ�dֹ{���Dy����e</���ZԱd��ǫeѝq�0~Z�e�m'ˁ�I�K���0���컬j
F�_�|�"��z�q#�?h�X+W�PR[_B	,d��]�����ž3mY���~6�=�ÿM�u�%�<o�4c��{cc�[�ȹ��Pju�C���S�����8�,Z���Rw��$�|pt�;�TlL���1�;3�fߵFߩ9���c<Pt��bOk
XOĐ�{�R��N]�W2�Ͽ�{r��Ұ�6ʗ.&��U�� ϕ �kB�����l��D�CB�0O,|aX�����ː���6H����35���z�o��2�9�a�@1����p�j�{A�qݞ��>��λ ����C����[W[��]��2LO]��� ��Xm�W$T�U���#[`c������r%�>�sDIiYb<�����m�	�m|>�����e�'�@�kX]�4���(�fc��_U�3����#�ʻ4|Q��)�֣�X�c���j��Y��6�-�j�zi�<�@Vy�F�%.�V����25�۟��ѡ�ٓ4�|h�wA�������������� J�~UB���~��ZH{X���e�������5(�0�gI�oe��O׌�.�xU��)$�GE��̐Q�S����E�=`��;˟gh=P�s=R8y�u���i�h�&��w�/v~v~���]�4�1��">L_܏!EgTl��+n�ޠ)c�nN�;B��a
蝌�Rm-��c��^  (��	�3�ݛ<�A~-��$)��BZ`�j�q< ��@�F\��y(��e:a�B�����v@��R��ۭt���(��Ӟe8�*����=�4.2���s�"��.~��Z ���8U�<��N
��pW3���ê�b�fU��BW�Ӝ�舿���[�<?�'M�R����Gh��@t#D�S'Q�s$�l5^G˅<��w���*\�*@�u5�l� I�`���5�X�]^F��2	�Ύ1Jz`�� U�.�q��j�T.{R�k��g�f�e�<r���oiH8����Eǒ]�ts�(6^&��#�.�#��v�aJ�F�ԁ�V�|�5m�T$~����Lt�i#�q�[��w{̘�n��\�-�L��N���TMD�Si���u�����%�Զ��[��S���36���V��X	�gK7�hA������4��G�j���ɫ��	,��� �$� �OlK�Bhmw�Ԛ�9�x�X.��v搃 �9*����ngm�6m��1��8J���:��p;T����N��=5�8[N�j��)���락�M$=q��S�|A&�ֽ�<>�]�s;�KěSoe
�VP	�q��2�$݅��?xך1������#��n{�4�m���9�J�
�^v`I�ӓ����#��.�ԕ�Ք(�t�4�j �%��4�;L�&�l1��oa����x7{]3a�y��Dw-*�,n ��H�$�@�G�J�L��=ϝ��-�v���9v��m�ﴂ�@��j�R�#Bަ�k���Y7MH[�r�lj2�a��<v����w����T#<p��0�Lgz��pPog��W;�����!��ctA����8X�c�\?e��`sW���'��ic��P(�ݱܳe���z��(�E�'�¼h�FtV��d�ۛN�Z����s^���y��	�[Z�iS����9֭H��Y ����ƈ1c�骆Y��
��M+�������lc��&�:$oזBU�P��З}ɇ?�X���g�RɒV0yYj�lMZ���G�D�D�i����a�(�C��@�E�����y��DE����$�Koa���ާ\��{�v/����ixxT�\� ��b��$���s��y���F�ʠ������D�
)�:�U�G�L����Za��U9�,��i�|:b�?q2 �´ݰ���7�\B �9�*I��B��3��%ay�z/��|wb}�#���;�`�3j�1$�$�p���ǿ�f�mz`��}5|�y��]e#��0E��)�9��~eq��K\P�՞�#m�ύ ��JC�����T�"H[�;�v�EO�}����\BAZ�n�8	#����q�F�Q�	X;�\�3	����;j�~�n�.�����d 8l��T���v{��1W<;^��.�@9�����5����K������ F,4�GFo����~�6�v���9&��B����:�R+g�
/�LW�x~�*c,�?|z��	7��P�� #66+����U<����kϞ��y���P@G��� _7�6DZ��]����1v�����|%��
�ā����b�=�h����|E�x�>����=c^�ɯ`�[h�.�0�"���.�H�����ϥ��%�'�;	m�4yl�FF�������vHB\����O���$�3�|���oL @q�!:�kZ��q\(���L�Lt��u��v��#j^�I�Lj�vC�L�&���n~��	�#T��:�w��$�*���k}d��^oL?s���B�[6qH�J����}S޳�� �>D)c��^|$ssj�^�4<ێ�/пs���t����M�m\�0�𤆼�P�ܴ�K����p���������ٝñ��N��^|�p���dr}��ĺ��;�}�	 !"�v���	�f��q��G?�� �8bC~aEL=�:���m������ {};�x���q�ʥɍf\	u��t��ɷJ9���'-��=�� �&/o�F�,�3eҬ��1p*\殧�|�i`x�7L�ǖ�<��F���ݕ���
���PP���w}�6�)��,�W��N��-�#�'�IǀHX'$�;�隬*�������rh�F;���9��z�"7l"nƝ���кu�b��m�r\rlg�'��������LV.�����7�5�v$���� 41c1_(h��~i+2w�v��@�-�ff~�OO���B��bM��2	m�Ct9ga9�a���V1��Vԩ��h�Z���%b��~`��ZP$�l0�Y�t�,§rY��{Ǩ�k��I�"� ����&C?FXE���hr�.|��L���|(�s��a�,[�T�?"�ސH9KXE��BT���;��0���1D���%�m�B���H���[��̃kk���9�ۊ��k�),`��}fQ�����i�IL�SX��ܝ�,R�����������b�k���\�Y�S�e�r��Қm{���l��{�r"p���/��ٟO�R�6fžX����1�jLk�I�����)鍫��.,e^��I��3�Q��
t�]�Y"ô����}�Qd��̱�yI�*�Z�6����������\�|L�l�n땯�S�G�荓���\���H�M��8�����]�6z5�\��W��?��;d��A�)fO� �FqdzA���O�=pަN)�w�8������۔C�/Dd�����2yn�jW.�ÈbT��Fp��o����mo�ⳓ %)5���!Zd2�7�Ȓ�,Z��P��EN���F%����%��3���4��� p���S����L�`1o�.v��E�Js��8�k9��Rv������C�;Sq$1�����hI2�y����L�d��c["(S�ľu�5zM���)b5��dYY`^(ұ5�ex�?�H�W��j�5������/\�K�H�x��xte�X]ą�M��2\1��b�N-4-]F��J�z5�o|����2�b�7�l��=���ߜ�T�Ɛ��B�=�z�����a�T��vbMCva��G�W����%\w���-��!�	��x������E������ڛc��-�ٕ)2-���������3G��3�HdMKRC�z�&L�f�$���I-G;�,�����V���*�f�����|`_� ��k0�:g6��¬�u�!]]iA[���;m���5h�Nj����9P�;��V����������#�6X��`f�;w;����Eqg�ɮ�ґv����ݑ�k*� }�F���I5��HV�z7I>	~L��e�<?�Al�o$go�kV%E�p=/��ޢ³`<K�=/u�ޛ*+�`���r�2�=>X��]R\��U���~����#{�ʋL�z�Q�-WxZ�� +.P�Elb�j�p5�V{�Ϲ����WS��_l*"����R�Td;R[��N�O��-�� k� 
�O 5�gꇪ#+}[���i8wMoZ^�����s�n�;��TQ[%��`b`�j���i7s�}�L��'w�F� o�T����.St�lI��>� $vʅ��pn¢�׾��?�!� ���]�����I:h �:�����";����Ǽ0-�����2`��.�NM��,kՠ�������X3�;s|�1'�-^��� өS�lN]J�+�_�l��gCq��
0��d)�w��Ŝ V�6*�pS���N��uG���e�4;iJ%F� �]�����^fmsF��zHXď��Gث��!������I�%��>у?��֛ִ�X�n�t(���nw��G����k��c���&�xY�m�3a$-k�Ǿ��lS�n�;��0`}�
]���/���I����ݥ���I�����Ls�	7*�:��<W�>��C�/6��,������m���c�/�TQD�5����C4a�@�<(:�\F������DYӏ�ϗ��0qz����!Y�O���N���5�M��(�{�~�vn:�B��C��ْy�o?�u����^4�����y// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/lockdep.c
 *
 * Runtime locking correctness validator
 *
 * Started by Ingo Molnar:
 *
 *  Copyright (C) 2006,2007 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2007 Red Hat, Inc., Peter Zijlstra
 *
 * this code maps all the lock dependencies as they occur in a live kernel
 * and will warn about the following classes of locking bugs:
 *
 * - lock inversion scenarios
 * - circular lock dependencies
 * - hardirq/softirq safe/unsafe locking bugs
 *
 * Bugs are reported even if the current locking scenario does not cause
 * any deadlock at this point.
 *
 * I.e. if anytime in the past two locks were taken in a different order,
 * even if it happened for another task, even if those were different
 * locks (but of the same class as this lock), this code will detect it.
 *
 * Thanks to Arjan van de Ven for coming up with the initial idea of
 * mapping lock dependencies runtime.
 */
#define DISABLE_BRANCH_PROFILING
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/interrupt.h>
#include <linux/stacktrace.h>
#include <linux/debug_locks.h>
#include <linux/irqflags.h>
#include <linux/utsname.h>
#include <linux/hash.h>
#include <linux/ftrace.h>
#include <linux/stringify.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/gfp.h>
#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/nmi.h>
#include <linux/rcupdate.h>
#include <linux/kprobes.h>
#include <linux/lockdep.h>

#include <asm/sections.h>

#include "lockdep_internals.h"

#define CREATE_TRACE_POINTS
#include <trace/events/lock.h>

#ifdef CONFIG_PROVE_LOCKING
int prove_locking = 1;
module_param(prove_locking, int, 0644);
#else
#define prove_locking 0
#endif

#ifdef CONFIG_LOCK_STAT
int lock_stat = 1;
module_param(lock_stat, int, 0644);
#else
#define lock_stat 0
#endif

DEFINE_PER_CPU(unsigned int, lockdep_recursion);
EXPORT_PER_CPU_SYMBOL_GPL(lockdep_recursion);

static __always_inline bool lockdep_enabled(void)
{
	if (!debug_locks)
		return false;

	if (this_cpu_read(lockdep_recursion))
		return false;

	if (current->lockdep_recursion)
		return false;

	return true;
}

/*
 * lockdep_lock: protects the lockdep graph, the hashes and the
 *               class/list/hash allocators.
 *
 * This is one of the rare exceptions where it's justified
 * to use a raw spinlock - we really dont want the spinlock
 * code to recurse back into the lockdep code...
 */
static arch_spinlock_t __lock = (arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED;
static struct task_struct *__owner;

static inline void lockdep_lock(void)
{
	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	__this_cpu_inc(lockdep_recursion);
	arch_spin_lock(&__lock);
	__owner = current;
}

static inline void lockdep_unlock(void)
{
	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	if (debug_locks && DEBUG_LOCKS_WARN_ON(__owner != current))
		return;

	__owner = NULL;
	arch_spin_unlock(&__lock);
	__this_cpu_dec(lockdep_recursion);
}

static inline bool lockdep_assert_locked(void)
{
	return DEBUG_LOCKS_WARN_ON(__owner != current);
}

static struct task_struct *lockdep_selftest_task_struct;


static int graph_lock(void)
{
	lockdep_lock();
	/*
	 * Make sure that if another CPU detected a bug while
	 * walking the graph we dont change it (while the other
	 * CPU is busy printing out stuff with the graph lock
	 * dropped already)
	 */
	if (!debug_locks) {
		lockdep_unlock();
		return 0;
	}
	return 1;
}

static inline void graph_unlock(void)
{
	lockdep_unlock();
}

/*
 * Turn lock debugging off and return with 0 if it was off already,
 * and also release the graph lock:
 */
static inline int debug_locks_off_graph_unlock(void)
{
	int ret = debug_locks_off();

	lockdep_unlock();

	return ret;
}

unsigned long nr_list_entries;
static struct lock_list list_entries[MAX_LOCKDEP_ENTRIES];
static DECLARE_BITMAP(list_entries_in_use, MAX_LOCKDEP_ENTRIES);

/*
 * All data structures here are protected by the global debug_lock.
 *
 * nr_lock_classes is the number of elements of lock_classes[] that is
 * in use.
 */
#define KEYHASH_BITS		(MAX_LOCKDEP_KEYS_BITS - 1)
#define KEYHASH_SIZE		(1UL << KEYHASH_BITS)
static struct hlist_head lock_keys_hash[KEYHASH_SIZE];
unsigned long nr_lock_classes;
unsigned long nr_zapped_classes;
unsigned long max_lock_class_idx;
struct lock_class lock_classes[MAX_LOCKDEP_KEYS];
DECLARE_BITMAP(lock_classes_in_use, MAX_LOCKDEP_KEYS);

static inline struct lock_class *hlock_class(struct held_lock *hlock)
{
	unsigned int class_idx = hlock->class_idx;

	/* Don't re-read hlock->class_idx, can't use READ_ONCE() on bitfield */
	barrier();

	if (!test_bit(class_idx, lock_classes_in_use)) {
		/*
		 * Someone passed in garbage, we give up.
		 */
		DEBUG_LOCKS_WARN_ON(1);
		return NULL;
	}

	/*
	 * At this point, if the passed hlock->class_idx is still garbage,
	 * we just have to live with it
	 */
	return lock_classes + class_idx;
}

#ifdef CONFIG_LOCK_STAT
static DEFINE_PER_CPU(struct lock_class_stats[MAX_LOCKDEP_KEYS], cpu_lock_stats);

static inline u64 lockstat_clock(void)
{
	return local_clock();
}

static int lock_point(unsigned long points[], unsigned long ip)
{
	int i;

	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		if (points[i] == 0) {
			points[i] = ip;
			break;
		}
		if (points[i] == ip)
			break;
	}

	return i;
}

static void lock_time_inc(struct lock_time *lt, u64 time)
{
	if (time > lt->max)
		lt->max = time;

	if (time < lt->min || !lt->nr)
		lt->min = time;

	lt->total += time;
	lt->nr++;
}

static inline void lock_time_add(struct lock_time *src, struct lock_time *dst)
{
	if (!src->nr)
		return;

	if (src->max > dst->max)
		dst->max = src->max;

	if (src->min < dst->min || !dst->nr)
		dst->min = src->min;

	dst->total += src->total;
	dst->nr += src->nr;
}

struct lock_class_stats lock_stats(struct lock_class *class)
{
	struct lock_class_stats stats;
	int cpu, i;

	memset(&stats, 0, sizeof(struct lock_class_stats));
	for_each_possible_cpu(cpu) {
		struct lock_class_stats *pcs =
			&per_cpu(cpu_lock_stats, cpu)[class - lock_classes];

		for (i = 0; i < ARRAY_SIZE(stats.contention_point); i++)
			stats.contention_point[i] += pcs->contention_point[i];

		for (i = 0; i < ARRAY_SIZE(stats.contending_point); i++)
			stats.contending_point[i] += pcs->contending_point[i];

		lock_time_add(&pcs->read_waittime, &stats.read_waittime);
		lock_time_add(&pcs->write_waittime, &stats.write_waittime);

		lock_time_add(&pcs->read_holdtime, &stats.read_holdtime);
		lock_time_add(&pcs->write_holdtime, &stats.write_holdtime);

		for (i = 0; i < ARRAY_SIZE(stats.bounces); i++)
			stats.bounces[i] += pcs->bounces[i];
	}

	return stats;
}

void clear_lock_stats(struct lock_class *class)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct lock_class_stats *cpu_stats =
			&per_cpu(cpu_lock_stats, cpu)[class - lock_classes];

		memset(cpu_stats, 0, sizeof(struct lock_class_stats));
	}
	memset(class->contention_point, 0, sizeof(class->contention_point));
	memset(class->contending_point, 0, sizeof(class->contending_point));
}

static struct lock_class_stats *get_lock_stats(struct lock_class *class)
{
	return &this_cpu_ptr(cpu_lock_stats)[class - lock_classes];
}

static void lock_release_holdtime(struct held_lock *hlock)
{
	struct lock_class_stats *stats;
	u64 holdtime;

	if (!lock_stat)
		return;

	holdtime = lockstat_clock() - hlock->holdtime_stamp;

	stats = get_lock_stats(hlock_class(hlock));
	if (hlock->read)
		lock_time_inc(&stats->read_holdtime, holdtime);
	else
		lock_time_inc(&stats->write_holdtime, holdtime);
}
#else
static inline void lock_release_holdtime(struct held_lock *hlock)
{
}
#endif

/*
 * We keep a global list of all lock classes. The list is only accessed with
 * the lockdep spinlock lock held. free_lock_classes is a list with free
 * elements. These elements are linked together by the lock_entry member in
 * struct lock_class.
 */
static LIST_HEAD(all_lock_classes);
static LIST_HEAD(free_lock_classes);

/**
 * struct pending_free - information about data structures about to be freed
 * @zapped: Head of a list with struct lock_class elements.
 * @lock_chains_being_freed: Bitmap that indicates which lock_chains[] elements
 *	are about to be freed.
 */
struct pending_free {
	struct list_head zapped;
	DECLARE_BITMAP(lock_chains_being_freed, MAX_LOCKDEP_CHAINS);
};

/**
 * struct delayed_free - data structures used for delayed freeing
 *
 * A data structure for delayed freeing of data structures that may be
 * accessed by RCU readers at the time these were freed.
 *
 * @rcu_head:  Used to schedule an RCU callback for freeing data structures.
 * @index:     Index of @pf to which freed data structures are added.
 * @scheduled: Whether or not an RCU callback has been scheduled.
 * @pf:        Array with information about data structures about to be freed.
 */
static struct delayed_free {
	struct rcu_head		rcu_head;
	int			index;
	int			scheduled;
	struct pending_free	pf[2];
} delayed_free;

/*
 * The lockdep classes are in a hash-table as well, for fast lookup:
 */
#define CLASSHASH_BITS		(MAX_LOCKDEP_KEYS_BITS - 1)
#define CLASSHASH_SIZE		(1UL << CLASSHASH_BITS)
#define __classhashfn(key)	hash_long((unsigned long)key, CLASSHASH_BITS)
#define classhashentry(key)	(classhash_table + __classhashfn((key)))

static struct hlist_head classhash_table[CLASSHASH_SIZE];

/*
 * We put the lock dependency chains into a hash-table as well, to cache
 * their existence:
 */
#define CHAINHASH_BITS		(MAX_LOCKDEP_CHAINS_BITS-1)
#define CHAINHASH_SIZE		(1UL << CHAINHASH_BITS)
#define __chainhashfn(chain)	hash_long(chain, CHAINHASH_BITS)
#define chainhashentry(chain)	(chainhash_table + __chainhashfn((chain)))

static struct hlist_head chainhash_table[CHAINHASH_SIZE];

/*
 * the id of held_lock
 */
static inline u16 hlock_id(struct held_lock *hlock)
{
	BUILD_BUG_ON(MAX_LOCKDEP_KEYS_BITS + 2 > 16);

	return (hlock->class_idx | (hlock->read << MAX_LOCKDEP_KEYS_BITS));
}

static inline unsigned int chain_hlock_class_idx(u16 hlock_id)
{
	return hlock_id & (MAX_LOCKDEP_KEYS - 1);
}

/*
 * The hash key of the lock dependency chains is a hash itself too:
 * it's a hash of all locks taken up to that lock, including that lock.
 * It's a 64-bit hash, because it's important for the keys to be
 * unique.
 */
static inline u64 iterate_chain_key(u64 key, u32 idx)
{
	u32 k0 = key, k1 = key >> 32;

	__jhash_mix(idx, k0, k1); /* Macro that modifies arguments! */

	return k0 | (u64)k1 << 32;
}

void lockdep_init_task(struct task_struct *task)
{
	task->lockdep_depth = 0; /* no locks held yet */
	task->curr_chain_key = INITIAL_CHAIN_KEY;
	task->lockdep_recursion = 0;
}

static __always_inline void lockdep_recursion_inc(void)
{
	__this_cpu_inc(lockdep_recursion);
}

static __always_inline void lockdep_recursion_finish(void)
{
	if (WARN_ON_ONCE(__this_cpu_dec_return(lockdep_recursion)))
		__this_cpu_write(lockdep_recursion, 0);
}

void lockdep_set_selftest_task(struct task_struct *task)
{
	lockdep_selftest_task_struct = task;
}

/*
 * Debugging switches:
 */

#define VERBOSE			0
#define VERY_VERBOSE		0

#if VERBOSE
# define HARDIRQ_VERBOSE	1
# define SOFTIRQ_VERBOSE	1
#else
# define HARDIRQ_VERBOSE	0
# define SOFTIRQ_VERBOSE	0
#endif

#if VERBOSE || HARDIRQ_VERBOSE || SOFTIRQ_VERBOSE
/*
 * Quick filtering for interesting events:
 */
static int class_filter(struct lock_class *class)
{
#if 0
	/* Example */
	if (class->name_version == 1 &&
			!strcmp(class->name, "lockname"))
		return 1;
	if (class->name_version == 1 &&
			!strcmp(class->name, "&struct->lockfield"))
		return 1;
#endif
	/* Filter everything else. 1 would be to allow everything else */
	return 0;
}
#endif

static int verbose(struct lock_class *class)
{
#if VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static void print_lockdep_off(const char *bug_msg)
{
	printk(KERN_DEBUG "%s\n", bug_msg);
	printk(KERN_DEBUG "turning off the locking correctness validator.\n");
#ifdef CONFIG_LOCK_STAT
	printk(KERN_DEBUG "Please attach the output of /proc/lock_stat to the bug report\n");
#endif
}

unsigned long nr_stack_trace_entries;

#ifdef CONFIG_PROVE_LOCKING
/**
 * struct lock_trace - single stack backtrace
 * @hash_entry:	Entry in a stack_trace_hash[] list.
 * @hash:	jhash() of @entries.
 * @nr_entries:	Number of entries in @entries.
 * @entries:	Actual stack backtrace.
 */
struct lock_trace {
	struct hlist_node	hash_entry;
	u32			hash;
	u32			nr_entries;
	unsigned long		entries[] __aligned(sizeof(unsigned long));
};
#define LOCK_TRACE_SIZE_IN_LONGS				\
	(sizeof(struct lock_trace) / sizeof(unsigned long))
/*
 * Stack-trace: sequence of lock_trace structures. Protected by the graph_lock.
 */
static unsigned long stack_trace[MAX_STACK_TRACE_ENTRIES];
static struct hlist_head stack_trace_hash[STACK_TRACE_HASH_SIZE];

static bool traces_identical(struct lock_trace *t1, struct lock_trace *t2)
{
	return t1->hash == t2->hash && t1->nr_entries == t2->nr_entries &&
		memcmp(t1->entries, t2->entries,
		       t1->nr_entries * sizeof(t1->entries[0])) == 0;
}

static struct lock_trace *save_trace(void)
{
	struct lock_trace *trace, *t2;
	struct hlist_head *hash_head;
	u32 hash;
	int max_entries;

	BUILD_BUG_ON_NOT_POWER_OF_2(STACK_TRACE_HASH_SIZE);
	BUILD_BUG_ON(LOCK_TRACE_SIZE_IN_LONGS >= MAX_STACK_TRACE_ENTRIES);

	trace = (struct lock_trace *)(stack_trace + nr_stack_trace_entries);
	max_entries = MAX_STACK_TRACE_ENTRIES - nr_stack_trace_entries -
		LOCK_TRACE_SIZE_IN_LONGS;

	if (max_entries <= 0) {
		if (!debug_locks_off_graph_unlock())
			return NULL;

		print_lockdep_off("BUG: MAX_STACK_TRACE_ENTRIES too low!");
		dump_stack();

		return NULL;
	}
	trace->nr_entries = stack_trace_save(trace->entries, max_entries, 3);

	hash = jhash(trace->entries, trace->nr_entries *
		     sizeof(trace->entries[0]), 0);
	trace->hash = hash;
	ha