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
ÿ”ğPPõ‚ë³w}à6Å)ç,ÀW‘ N²è-İ#¿'üIÇ€HX'$†;éš¬*ú‚±ÔĞóµrh×F;ìğæ9“zë"7l"nÆóÄîĞºu‡b²€m²r\rlgè'ìüø°¹•ª÷LV.ñÌÍÎÇ7‘5Áv$»Ñİ÷ 41c1_(h´×~i+2w‰v„„@œ-âff~OO ÿÚBØùbM“×2	m·Ct9ga9a¹øüV1ÑïVÔ©îÿhŠZ÷ŞÌ%bÏï~`°·ZP$øl0ÔYÌtİ,Â§rYÄ‘{Ç¨ék—IÓ"Æ …§ª &C?FXEŸÉÉhrà.|¸›L’µÖ|(¼sûìa,[¸TØ?"šŞH9KXE”ãBT©¿—;ˆŸ0ÑĞ1DæãÆ%±mìBÁ¸H‹ñÓ[î¼Ìƒkkä×ï9„ÛŠ²®kÎ),`¤ß}fQòÔîÇÙiÉILŒSXí½ËÜÃ,Ràõı¿ğù±¿’Á b™kˆÿ«\°YÚS·eßrÓ·Òšm{‚³ÁlúÖ{Àr"p²ô—/¨ÀÙŸO´R¼6fÅ¾X ÑôÓ1ŒjLk³I–¬—ÑÏ)é«´Ö.,e^ˆ÷IÏû3­Qèè´
tƒ]±Y"Ã´¥—¸Ñ}¦Qd›àÌ±¬yIš*ÕZÁ6™ş²ÛÛÿªà\À|L¶l‰në•¯®SóG¬è“«”±\ôŸ—H¦Mğ¦Ë8¦ô³‰Ä]×6z5™\”‡W˜œ?¹«;dÅùA )fO ï¨FqdzAÑøôOø=pŞ¦N)‡w¸8¢«ş¶­óÛ”Câ/Ddşã¬àğÌ2yn´jW.ÑÃˆbTŸñFpŠ„oã¥ÖÒ¡moÊâ³“ %)5ºïÓ!Zd2â¤7˜È’Ø,Z•©PîâEN’ÕÜF%Ÿå÷î%¦²3»ó“4µ©õ pÒ÷²SıÇ×é¼L“`1oª.v“ÚEòJs±ï8ük9’Rv„‹®ŞáCô;Sq$1 ±­¨hI2£yĞû³ÉLêd²Îc["(S›Ä¾u“5zM²‚Ÿ)b5èØdYY`^(Ò±5«exæ?‚HæWºÓj›5¸º¿ù”ã/\İKÈH¤xåÙxteåX]Ä…ÆMÀì2\1½ªb€N-4-]FÇéJz5½o|ëş£Ä2Îbõ7õl¤Õ=„ÀÒßœşT‹ÆŞâBı=õz‰–§×ŸaÙTàövbMCvaÄãGÕWªó¾ë%\w‰-­é!Õ	ßãx¹âıĞäêEÎƒñ¤ËêÚ›c¡-äÙ•)2-á¯¦ÙÀµ“”3G¸ÿ3ğHdMKRC´z÷&L©f‡$»÷I-G;,¢°ùƒ™Vˆâ*Ìfı‡úœ«|`_‹ «k0¨:g6¸Â¬Òuü!]]iA[¹§­;m–ü5hùNj©üÒÀ9P;¿ğV¤­¿Ç‹ï˜ú¥ù#¾6XÉñ`fÿ;w;ˆéâóEqg‚É®ÔÒ‘vŸ½ãÖİ‘‘k*ÿ }åFëÕÆI5ÒâHV”z7I>	~Lš‰eù<?©Al¥o$go¸kV%Eùp=/ğÉŞ¢Â³`<K“=/u–Ş›*+¡`ƒ°Ür¬2‰=>XÑ]R\¼•U›ÿˆ~ÁØ#{Ê‹LÂz½Q¡-WxZ…“ +.PÂElbÏjp5î‚V{ïÏ¹³­©WSõ¸_l*"£¢Š‘Rç­Td;R[ÂÉNÆO Ì-Òô kÂŒ´ 
ıO 5ûgê‡ª#+}[ÿ’Âi8wMoZ^ÀÒà£áÕsënô;ƒ¿TQ[%€®`b`¨jøàçi7sì}¡L¸ç'w±Fÿ oÈT®Óı¼.St¶lI®ú>ó $vÊ…ò³pnÂ¢£×¾Äâ?½!¥ ĞàÙ]©Áµö©I:h ô:¦ƒÓÎ";òäı†Ç¼0-¦¡§¼•2`ïü.áNMöÀ,kÕ ‚›–ôòøX3´;s|Ü1'ƒ-^Ÿìë Ó©S—lN]Já+¯_l¹õgCq‹ë½
0êçd)®w–öÅœ V®6*õpSíÇíNµîuG‰‘Üe‚4;iJ%F ‰]‰ü‹âö^fmsF–êzHXÄ„ŒGØ«˜²!‡íßôŒÎIá%Ğç>Ñƒ?åÔÖ›Ö´˜XÔnüt(ó›ÌånwÃİG—¾ÅğkµØc¸åù&ïxYèmÊ3a$-kıÇ¾­ÒlSän¼;¤„0`}ç
]îª¡²ûò/ÚÀîI¨úÁíİ¥ôßIœ´®¦ÒLs¦	7*í:ÿì<WÜ>Á¼Cœ/6¼Ã,ï„‚ÅÚæàÜm·ôûcµ/’TQD½5÷¯¶C4aö@Ô<(:Ô\FÑÚô­”DYÓÏ—‘×0qzóÍÙÎ!Y¶O‰¶¯Nº£ö5ÉM¤™(Å{~îvn:ğBºóCÌ×Ù’yÆo?²u¢è¡£^4ùÇÀš·y// SPDX-License-Identifier: GPL-2.0-only
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