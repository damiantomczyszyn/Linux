\
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
  include/media/i2c/upd64031a.h \

drivers/media/i2c/upd64031a.o: $(deps_drivers/media/i2c/upd64031a.o)

$(deps_drivers/media/i2c/upd64031a.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      R  �  �     a      
     u  $        �     i     �  �  �     �     �     �     �     �     <     �  �  <       �  <     -  @  <     P  �   <     o     <     �     r     �  �  �    �      #     �  �  w     �  0  7    �  p      �  �  �        �     ,  �       D  �       [     
    q  `   <     �  0      �  \                     �           �          :          r          �          X       �     �   `	  W    9  �  6    �  �  �    <  p  6    �           �             �                          �  3     2             ;             A             N             Y             f             p             ~             �             �             �             �             �             �                                       (  �  �     A             H             O  �  (     l  �  n     �             �             �             �             �             �             �             �  �  �     �             	             '	             G	             e	             y	             �	             �	             �	             �	             �	             �	             
             
             3
             G
             [
             t
             �
             �
             �
             �
             �
                                       .             D             \             k             �             �             �  P$  Q     �             �  �$  �    �               @&  b                 +             A  �'  �     X  �(  E    o             }             �             �             �  �)  �    �             �             �             �  �,  b       .  �    6             M             f  �0  
    �             �             �             �             �   4  �    �             �               �6  �    0  ,        D  0        \  �        o  �        �  �       �  �        cgroup.c __kstrtab_cgroup_bpf_enabled_key __kstrtabns_cgroup_bpf_enabled_key __ksymtab_cgroup_bpf_enabled_key __kstrtab___cgroup_bpf_run_filter_skb __kstrtabns___cgroup_bpf_run_filter_skb __ksymtab___cgroup_bpf_run_filter_skb __kstrtab___cgroup_bpf_run_filter_sk __kstrtabns___cgroup_bpf_run_filter_sk __ksymtab___cgroup_bpf_run_filter_sk __kstrtab___cgroup_bpf_run_filter_sock_addr __kstrtabns___cgroup_bpf_run_filter_sock_addr __ksymtab___cgroup_bpf_run_filter_sock_addr __kstrtab___cgroup_bpf_run_filter_sock_ops __kstrtabns___cgroup_bpf_run_filter_sock_ops __ksymtab___cgroup_bpf_run_filter_sock_ops sysctl_convert_ctx_access cg_sockopt_convert_ctx_access cg_sockopt_get_prologue bpf_cgroup_link_dealloc bpf_cgroup_link_fill_link_info cgroup_bpf_release_fn cgroup_bpf_release bpf_cgroup_link_show_fdinfo __bpf_prog_run_save_cb copy_sysctl_value sysctl_cpy_dir sysctl_cpy_dir.cold __func__.0 cgroup_dev_is_valid_access sysctl_is_valid_access cg_sockopt_is_valid_access sysctl_func_proto bpf_sysctl_get_new_value_proto bpf_set_retval_proto bpf_sysctl_get_name_proto bpf_sysctl_get_current_value_proto bpf_sysctl_set_new_value_proto bpf_get_retval_proto sockopt_alloc_buf cgroup_bpf_replace CSWTCH.197 cgroup_dev_func_proto compute_effective_progs update_effective_progs __cgroup_bpf_detach bpf_cgroup_link_release.part.0 bpf_cgroup_link_release bpf_cgroup_link_detach cg_sockopt_func_proto bpf_get_netns_cookie_sockopt_proto __cgroup_bpf_attach bpf_cgroup_link_lops __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_ops491 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_addr490 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sk489 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_skb488 __UNIQUE_ID___addressable_cgroup_bpf_enabled_key483 bpf_get_retval __fentry__ current_task bpf_set_retval bpf_get_netns_cookie_sockopt init_net k�dN�7�=�h#9�s������yO�T�,[9!�&{�S_1$�� >���w��*-'H��(���N��lV�a!J�{z����h�X�C�m'M��Z������؏�InpL�27�&87^Vc%�@ˤ�BZі�8.n�w5!}�G�	s�����R!�y3�"�@Ʋ��@�*V�Er��T�ZO�S��G��쌻ൻ��S�j�Yq�ȅ�����&�e6M�EĤ�h�!�Qkդ\K�zoO<����@U��x�aT���"��Es=��@���n��$_�<)�	�h��j(̎��"9�|����ޢx'�c�=���E�Y�rB��l3*`�2Z�&D�<H�������5�2H[�&Va�V�T�~�k�s�x�5ڗ^2�r���';D�1��F�⟧;�*?>��E�G.�$_�iX֝���%E���@��WF��+O`C��<K{B�g��s�	*
J&�i�g���N:�dݲ�x��S�P�\9
�-��jp���Ǳ�ڀQl�^ā|@�A"[��vF�hnp�$�2�7�R���ٌr�1PO�	����<��a)0�;�W�B�[�t$	��ݛ
Kس�Y�Dg(���o�2�m�K�5��΂*����ҵ�9���H;��G��?������y���0_�Sx�K�f����������[��%�s׆L�@M8��3�z���*m�����t3���@	/�=v،+[�?r�죻�n�Ƒ���i��8� ��է��"c�@�7�mX�|T �/�Z�ռˑ^��N"��+答=&	,-����x������,~7\��C��}jP08������2�&+�&'�>�;���/p�7#J�d�N�e�5��Lf�����a��,���~?�|�">�Dk��R���y.�\U�'F�e���mFg�cF�78pu�DKtA��l|u2nÃ��b̂�쳹��&iU�wfK,OQ�B�r�"ߔDi��K�� i=���[��[k
�EE��lZ+~�Г�ݿ̋�o�g7�.��ە���I�9��Mb��֍���Tn�&�b��O��BD�Td�6ه���s��+H��>���T^��%�k Y�ڑ��;h��a�YwG�~�����XNԗK_]n��t=����SzhƁ~�>��}crq�mqf:�0�@��Bl5%!���������jH0C�?��j�8B�Z����4B��P�{0(c:�Hn5���/OJ�Tuͤ�7�����A��B�^��PTu�b�f�Ն`�iwJ��s���|��Ή��=*�@s෹0��J�����p����胢{Q�M����u���i;6��B�%S������������H)- � R+��Z2a1�{��V>�0%�����$X���FU?��uԜs�,t��+ ��A�?d����o��ר��c7����	�AF��Sq��ޓ�]]`����u8
��a�9�h|AQX��GU�	���V-&:�.��\�5���3���[�_��\]�]����-�~�w� r�∻.�k=�UD=�@�{��k%ϼ���H[i|Qj��T����Qז�����I�]��J����a��G�����_O��G��X�ϛmLn�%�c4I#�<�K�	�q����4V��r��,�)���,��&�+�����Jo	��%�״*�7��	��K����v���Zn�Ƶ�ӿ\�*LԽ���}Z�{��qT�_�2G�~?��Tc�G�V8ش�P���t����2����7K�.5a�݇0�,��tec���ۆl�u"Y��Fᐯ�?r�/�Q,
���0��}�jX�����&Z��;��� �Q���j<,aȄ�J�%*��1���Q2���.00-��n��1��`� �bK��!��"��~RȌ�E�j���q�x�F��Z�G�!cUy��|�q�R�����Z��g�~ǊO=�I���̋r��N�B�5�Xy�m�r��7�!�����?`C���>�*  �����V���q
��0�5���NY��r�an�0��K���A�{
�z�Q�����v�wm�Q�W��	Y�_˙\�]�7Nq�,y(�.��%�
��<�[��V�|et[�R�����}��U-�C˳iݟ�E'ᒡ@f$�*�IMGrI~l�a�����}ÚmCdF9�/�������ܨJ"P��j`5i�NM�b3�{nr֣�!�����ϒ�R��hUy�j�1�T�T���L�7�
rQ�#�y��[zq"��/$v��R.�G��$�I��E�I�pU[�!�EtQh��f
�3�m_l?	Q��'V��:���BA��4�~���.�(\�?�֊���@?zT8�[���P�)0��`丁�����[uׂɒ��}��"]�o/�K߰t�n*��{hP�4WX-���?}�ޤ^�[WF����K�q��q��&�g�RX���mپ���(dM6��(L;�_�>i�隆?G����`p�#�^��q4t$m�����Է�r��)&%�����E@���4� L��&���X2�Z��o:Z�[�#��݅����7<��	��e<|P\n�䮙_W����G�MhHX�x�=��+2z����u@j��-aA���x�(4OI˺+6I�&��e�`��ԣR,��D�Լ��D'��X��m�ʻ��)��/��=>�ehn���,�3�ǇІY�H�#2��j�a�ǜ��1�M��QӬ���iuF�6[26}�b.�4p�c�|��al��u���z1ʙ��4zu��u'k�	�-1H!�?>2��K���]��E�OT���g{��2�9@ۨp�%]m�+�l <�@Ht�*
؁X#��{�|b 4!����7���B�oc*�b_wR�G�S7a�U�J��n��9�1�3�$�S��XI\��G}z�C�tQO��Pǩ7�)��":?w�@��X�Oh�݄��x�L���F�}	K�����6iM^�J�S�@�&�����t~}h�ch||fIG)��Lv'eh�Ƭ��L)`���꘥�B)\�~��$}��V6t�K�/I��g���Mq��S���u%?��K
\m����+�k
�O$��<��Bbx��(5���=�9U��ʺ�!��1�/]�-0A�һP�8#D�4s��RսB�䅥�^��K��׿(�|O,h"�:�ƇL�t��nt���=VÁ�';��*���7�E�:��`|���(�t�B�A�SPtI](��JlG�@�І���-4��Eև�H� �Lcktkqc~��W�T4��(I��膇����d]��y"k��'N�ɬ��f<��P}�~��W�����@����� =��&,�N�Y�O���H@S>��������������j{�)Y%!�A'9��r���3�����ģ��%��J�鼫����($^�vb϶�8��-9h�՗��-�[MT
����U_�7d�����,�H���bn'`���sB%��|��Z=.j����bl��\���$��X�W��}?&�������A��P��A2�=��|�9���Z$�kJ�ѣ2A����I�m����-�)��N�Uy��
Z!V��R(�]�QT%�W2fӦ	��^L���oB@��2o_$���rn��	:���op�#��Z�4n�N�m'��$�M�
��#�j�P��m�a b7i�:�&��Rd4�~�פ>�"A�3k!��NhB�f�B;��P7Mm��V묀E\D`3�ᯒ�l��#Hh>:=]o���#@�m�����1C4�td9|���l�x��&<�v��xW��D�B� �s�4pC��8�{yL�������Dc�u��jQ.�8��Lկo'����@8T����G��5�Q�eFjԅ|�.�	���Ga(�Y�	`9V�~E�e����������@kN,c�l�,6�h�Jg�g�*>:���Ek�5�O��SM�$�7��ow��҅��E�é�΁�j�	�?�gH�Ѕw�>V�-E�σ*�$��eV�0Z=ˡb�j�p�2=��,Sk"�W=2��T"b~Eo.��;�6Q�"�9,^��G��YP���Qz��;0P_<2�,|�0ÖF}μC���ֈ�j��qOĕ��;bM\�]8�KF���V,�|x�@�p�
חLy�����a����P�K�v�d�
��2�4eY� t��c4��9N��-�.�{�-3�u q{�����c�u�U��X��	ٽ�@A�
AIc`�o��K��>S�G"c#on�,����1s���m�n�(!~���S�Ϊ����#V l�)ʬ��OV[����RU�l��n��e6A��l}�'u<t
?��h)Y�"��u��y,��]�iFspΪxJ1t��6�/�adIxKҖ�u	����S=��#4��:�]~j:
(���חW���p#�Ut\��|)��ݠcR�����"��U2����C�l�s+q1U�.e^��b�}n5�U/��Y���DH:5C��{#�
��y��� Bh�	����W��=5���k�P��<�5�xa	��YSm'T�@Lē�	�����iK��%8� ��{��*��>��<w�l��ZY��O��D�ԝ�qUu:^��̜VE�Q�%���m1ǟ�q� {:z#c�TiU�}) �<M�.B_� 3J�B��x������i�Pl�CM�;�p�XH�J��F&�^e�XS1)��Ζ�a�n�[y\���lNü.�)dq�u�1�`��Aÿ��<0�@�P/�:���p��ޭE���� �i��33��wk�8��"�]Xբ��$�� ��}d 7�e�aʜ�al�����O����k�׫go�32jn ��A��s�z(�[a�7Ѱ�cW6�Y��]��Y�},:u�RN��)��p�B	�W���*g�Yɘ��W�D��R���]D,n�J��q�5����O���UB`#�&Ibm<l8��Z��J/L)g;ͧZ�/M�+N�K%qb?�-Rn���k�\#<�E��h�����e��=��Tf-�vê~C=m2�n)�c�_��Y K�r�om�U��:�F�����ԫ+���A[=��/�<#�D9"�Dt�U+/��7�jk�[�9X��|�[<���i��	���9�	o�
j]�fzکF�v�*� IQ���)\ÜGD!���;4�a�A��e����Y�X��[�����R5�s�V����V%��[�o|y�񾚖g�^j����$|���ĳ�*��y2���3^S��f�I��$���כ�*e���zڟK^�T�ϲZ����0��P������bLJ�E"P'�c箫��<���;t*����f���6��Ȇ��vϔ�ĳ0�s��o��W��P���a0����+[��`VmO���F�I��v�b%;�T�f�g��a~¯���򃬮p�@wè7$��z�#ŉzqc�mL}��"�[��s��q��m�H������g)G9���)������ш�ـ�C�p��c��f�?!�M]�\�ʼ0a���p���W�o��r�Z�2$<��%m����w# {�DŐ��2o�oC�W��k��)/��I:�ʤ�\����lr�mҔ���Ayv�(f��@���� #%mu,^*�=2(����O��h����" ���^K���>xq=���hd8c��A@����v���9\���(m�5;[�r��n!�o3`���1R�Ϗ�)Q3!�3ӻEU��Oa�C�)F�A�C5�_@S 1�7�eDM��~)�U��H��H:�R�7#���gc���兘��Ģk���V�0w>�]V�s�y�*���}1>A$�@1�z��D']�볒k�-hK���I`��YM/]]
h���L�=l�-�����@�j���8�����4qnw��#9�4�7�:���#}��%I�g�`� S�G"�M��X�Ã�FvgK^�����<rWR��hI�7����[�J6W��=���ۦ̙)Ɗ3��+#�c!���SS���H�I73�jq��>�^��_����b)"=�O�um�*�
O��XX⍢�uѹ�yKv�k�9�ꮣ�5ЁG�z�jfR���sI���"�:��,ٌ���� �A!B�����K|�U6�����G#��j� �L$�ͭ�24h�����B3������)4ݻ>�':����"�4� G�flw�����6 ��*�k��s��p��+�fo�`g�o5ca�xd�gZ$�;�gD��H3cb�˺��������ʸi��N6�%If��d�	ls;.;�<�!@�*�A߉�Џ��w���6)?w���2�IwP�YA�ް��V8���-5���*�!�V���٠�p$[�!b�m%��� �OZ�	|�lޭ���z��}υ��$0��}k��qxp��T�f���6�-BN�(s����%����иG%p;9�hM�((�F(�,���+|,BkD� �7����"a���jI���4� ��>{c��E��R=�i��b��܅[�/�@�P_�}��J{h�i1x�xά
]CL�󰱮�V/l,�Le�����H:ؚ�s�R�m-a1; �C��+z��{�XC�_��c���o��%�|��4mv���j�oǥ����iHښz�����z�`6�yqR/�
�<��Z��Đ��3�g��Tm���Ff�����l@(FxcQѻ��n��q�
IǕ����)�fק�G�܁χU� �G�=�T)e�&�����mg[5���'-�7&yV�)�3qm�>��� �]�I��j�7�7����tS�ۿQ�Iѯ��"B�E�܄�I�,,#E�0ω���,�Y�O��v�=ĵ�놩�o�K���B:k���(wt�b��*�c
�ۺ�y@]I���4#�KZ�՗���ћICh�EL]����n�f5k���]ǜi�dsb&R���D��@���$tQ��XףB�	�·s<\?��(�c�U4C�mm&������V{,5��Eal�qY�VӲ��̃�_����Ԇ����2J�_��$�_�w��mD0U/�a�ѓI�
G��\�EKb�K����<n���U4�FuM�����:����4�4SR��=�?�ER�>������m��0�F��+EZ٥`Mr��+��<(������Qsz��k�\����</~9*?i:�-�ln\yI��yI�@����T&4=R(�~a��L��A�](�#�������jCP�2lX���=t�g�����E�E�0�L1L%���ɉu=�-�F1UyҨ;���.��&?N	����.�3�W~X��nL:[e�?��Y@�3R�h�� q}�8�g o�.(�_\ ��_*��hF�*�:�8�"�7&�j֞aD���)e05������?�cI�1�7n�'vaN_' 
�z+u�"=r��t��'���W$��	��U�Pq+�π���W�S��U@�/��A��d3��� Z�x�@˽��{7b�{�DK�����]�c\g8�a�#�����;#�����&{ǉ��N���\�]+�[a�w�W�N�!8W�����{B;�F�[�5>��5����-�ƒ �����̕s\�[_w�;�8�iB"�ї���bv�A�?��ݵ�G�
ؠ>J�����ٵwx�D�Vd,��I�=����p��ėx%�G�/X7U7��U��lz4��"f�%�%�jL�Ā��eUf�FR�5��V�/5o7��@�E�ֱ��)�~�v�R��6J�
�o[-�b�P;.��P�t�|<k�	�Lo�R�\m>��T�8���L��	�ь�_e�Ș[S���^Fs�����1�1zFLk3ޮ�U�j8/Yk����LQ�J\�]U#ӞS>�i��aI�����_h�!1Zɒ@Wj��"<�`E0ӛ$��9�g�j�r �O��fŧt��(HS���n8�*�E�hu :¡7�c��4��H+և�"ơ6p�
�e ��v3��i�6����!�����^��e�N8�>�J�~d�E4o�í��/.�����)�8��D7��D`H��G����'�c�S���<�����o��5*��.J�R�h�rP�%���Rk�|�h��{������,/�I {L�c�/b�gwI�ߵ\�E�n8:��v�0�?/����Qld7�Ԅ��v�����O�56'�9x1B�.���/��eBHe�i��b+P����<ڑ$���_2xY)��c½�ݫ3�҄GYL%�Yr�U����e-��'6��']s�I�_��+:�ͥ>Ϣ7�����GD6�=֔w�{OA�����{�ҙ��s5�߅��$�3��t&��7��
�B��`Z��r>�E�MQ�3Ǜ����5c����&v?�=`�>t��FO�g���?�D�Z�r�UY�� z�!�%	-l��s����1��
�y����s�$L������4ڱ���)������ϟ�c�^����Fw��g�XhU�>��	*Q$��&�1l�  4�])ۗn�|Kw��	+���,�/�\^1����?d�.��2au*C��:��KW��cCe�����o_�Dlr�>R��qE���>եFd0��E�×U�u��^��XK����_-�>���s7l���ޑ�9d��3`�\+q�Ѽ�r�0z��� ��L�$&��R��Rsc���������Vxo��L�| �̠<->��� �����Ðs�
��2��hb�������]�iR,h?a�L��ZVQ�8ø�gJɢ�&I�f���Y�����;4 ����V�
��Q�ᢔ@�ֽ������0s�),d�91;}7�x^z�j�"v����i�Lǻc���f��c1_��K4�vO�v��ե���8s�4zzU�i�v�������G�ߡ��6�ux4�C��H�"?\�f�(I�K��W`�i�"+�C�x�w��ֲ�"�¸�|���!��C���w'tC9s�����S���� �'����(����t�+z�>� N`����3f�9u��=�c�f_����;,d���uE<筿x-2���TԆ�W
�B���,�n%S!:ji�a��`�W���"GA^)u����0�k��]u�gb��� L�̐�:Z~�N�[=���O���"�Z0U�	�.�X|����&�8d�9bڍ����Jc|��PL0��4&V6�?f	׹���u̢���#�1���D���̟�5��/XE�bv���B�Z�BL����kp�1����PT�-����i�kU|����f���2v�����!�h��؆�A
T6��RѶ��r�~h�u3Ha��_������wG{2.(M�o�fv���hM�%(h$1�� ��5V��d35J�Z�<�\��j������t���	����L��K��bXQ@����۟�\�u@������B���!
K�o��g��BF�Ag�\W0Tz�H�󁌝6�k�m�\����G�8����n�7e��Zr��k`Ǆɡh�j�AR���~�m�����SC�C����Kd5���H�ՙƋ�����/�ad����0Nb�2��9���z��7v[��{��Yx���*G�u��wo<�y�ƍ~�{�Cr���q�he��2�{H$
3(oAE��}����S�/95�|�Y0.��e{Gr:O�;O"���s�!�Gc���#;[��P�%������g<�|���!�Ļ�����+ȐV�n�Z"���r;=�kA�*�2T���Iq���پm�����j�D=�1�Qz'LR8�hZ�J�%��>؀0o���aB'�G`�.?Q������H5�J#�ƈ 5㣈S�n�U ��VV���
M5�UO���[�}�;���렌Z!�?��c-X��!tʑ�O�H�#�t+���FH��n��&�_ذ&�c$��v:�L+��>�(Jo56����ơ�l/���|`��L�K�q�����؀����+vt5]1�j�>��Qn��#���,K'����_7���RF�F��XR��@�֚�4��>�Е�C�\�Zv�]��P�%�N9�����Ӏ8�U�J쮑��0���4�R�M�`��GH��4�A����1jbOX(�!�)�R���''��������bzK�-&T�۪~�A$8l:#P��oՌ/�r������:a��M�N�!�TV(�Cˏ�4�E'���%���I�-:+��z�f �7gF�¡<�JhO\֓� �%�B��*	O���f�}o�b�P�|����E.�t�zr����bG� V8���hL<>�N���&�׸�4f�%�����'`�Ÿ��c� �Yr��y)���+����1G�7�%8��N�gԫ����5H�E
�����\�Šd��ԉ��̌r�V��xU�ޫQ�~��ED��XeP�`�k �A�f���5���1��,�>ͣ<���f6����z�J��F�r��KA1@n	�d��uݪ#d.�W�2�"N�*<����ܔ�6������ĩY�<����2��j�q�=0�B2�����U�pu�Q/@ӻ�8��<v�^�d&�v���~U��k���W0Ѕ2����i#�!I?��/����T-1����m����6����ƍq���c�g��Z�r�&j��Uj�`,��<m/�����sN�L���׼E�T�.l�W�fB���o�~�*v�L��0�%(���Z���3Q���4��w>����N�)��b�_YJ��Ȓ	lBz���^�����Q_*|���'e�����}u��t�%��kL�"�:.m�n��ç�m�0�����컁���c�+� g��;��@~�<�f�i�͈�]�#Tr�d$7�ړY��&�.�$�pS�C��)���� /ε���%p0_��z���t̤��g,�Q�U��2'j2Gz�y{��?(��k�RP���ܬ��W{l��x�S&|Rۓ*_h ��cM�q�Mf�z��5�|`�������U�lhH����x�2rd']c��tH*�g�~E���Q[ݨDC/v�(O_��߼�wkz6��tf;�'�Gq�-�f��Fn/�*p_&G�O�D}��s(����W����v��^�F�;F	�调� ��l��̐�t���U�/W��@�ț ��g*�娝8�v�'�(��65�m�^t��N.ѩa�j��,�|���-!�n���Zn�?m�T�Q�.P�$�&�V�7�=�QGo��1x�H��E���3��A�i/���E�E~tB�ǼO����D(���D>$Y��ɖ����f[����7K��F̬��Z[s���ٷ��cNRp���q�wm?�ϙ'nmZ	t�����/�b�M'����-�&�9��k��5�.��C���\�T�w���]��x	
I d|5NH����n����(JK(���V��D�4��X�V,�>���(��[�����Gg�
�n�8�q/5��v=�}�w^�d���=������e�n�ۇG�'mcE��d���5Zy)9�xa;A\v+���RCRR�&9�yjH��B\h:�a�f��z��0�)�es��g�[�P�JZ�0�]j"�CS<���ۀ��3���(�},pò%��=�&�x�!�ޣ,z�Nd�������X���v�Uӵ�v��h �Ѵ��B�}^=*r�@�|�]z�!E��ݢ�4+Uo����H|c����U|+鿕�r�J�J�h1�����^R�6,����}��K��Hv�q,�e��l�M�s�$:~j�kM�&�SM�9x�O�����p38��Ip?���Z��b�k�Ȳ�$�֋�S!�J� ��٬�Wj4�K�'ƺJ����d�QB��;Y {Neӻ��"Y`7���M����r0S6�)h�3U=�B���]b�G��4{Ld����*��F�v��_Q�P���̷����I��.g�PAذ^�k"��ޚ�����<�\�B^<�Z��9�6	I���K����Q��l�����߱���^�E	I9�Uh �H�GR��-q���pؕkǦ��V	 Ϗ�P�Wi�E��2�}YJ��N҃��c���gL�:ˋn��N:����ጬ��S�8�R3���F���r9��Oo�)������U�=��xg�%y����f���O&��-{�XC�'+s"��v���(���8�$n�~#Q�h��� J_�c^/IF���Hi���yz��9��y	�H_D�Ӷ��?����nf��.��V�kWnD���.Evh�N�2��g���`��>�R����#a�����4&
��zٙK�8PH;����QЀuS�n�
y��kN}�:�oN���@������^A���v�Ꮑ��7�D��q���iSVqLb0u�D0�2[	�<�qɟ�,��\�#�\0�O:ߏ���x!���+y\~ә�D�s���z/�IN��Y�@6%"2�N�.���Y�G��NF�a9�6|w<��[���_	
�[-�E����c,��8�%�ɤyw��nU�o!]�_��i��:U�fAJ�Y�r�y3JR�f���+�d$�r��$��Va��?�j��U �?�0��D�cd��c3�9W�
�^qʷ77&Tc���_2~+W�!��j�K��:ԅs������45mb�l��4&hZ�x�|���CuyG��<��`TN�<'\'��!�"9�Y�gJX&,;�`۾����1����(#��Ahs�ݾ&�eY�4��v�2$�����\pv,랍^���y�	��/u��{�����U�%��"N�˻�Ƨ�	vll��Ϙ0H�.8�+v��S� ���0�'�̢8A'���v��je���ԗ|����ޘ_N1��sI��vȈn�u�LE��->�pX���H	�H(�Eu`6ϰ���|����"��T	�D.�5|̲�(��g+��ut�l�v
��P�5�z���̹����3�5d��Ҩ=8�þ���lJMSJ��A6�? p�xV#za��{��P�n-����<ճ[���g�j獕U���$�� g��nX����sl�����Y�)D���"[��FG�I��{z�Tנ�׷��UOyzr�!��E�n-9]/�Ow��zI>�DF�:�qm�B� t(��	���~��j�g�%K�iXNŰ� NZ���I��k��v�YsI��)?>�I�]�g\��Ul`
Ê��؟��@_( �o����-�$�PNT3��;��k*K�e-γK��>Z7� v��Ĩm���+�ɖz���W���ۨ�]�Q0WFj/ʚ��?�PD�2UCo0f�q�N�P��a��_��%�MT
�(V�Qc��(�����p1�V�\�֛�h��ko��n����w�ѝ1}!�_��<[5s��*����.|��Y8��Ղ���U�	䑠�Q�W��n"�B����lZr�����:�^Ѯ��%Eԓ��h��;vE�\��sZ�u���H<�@�1R.�T�dJ�Rf_M(���OUf��7�7 Sf�<`(1p���(�����c�X�T�Ǥ�cd܉�<]��#0����bų"�"����yջ)m���hIl*p�s�Ӂ������5cj�DO�k7�)��w[���C�+�~ܝf&%A�SX
����=�õ�YE��a/���NK���蛯5�Vv����s�H3��x�(�*�P+��f&�KhP{�8����w��Q��8#���FH��;���&KR��$�+D~�|���`PI�c6��vp�j�Bv&���_��3U��a�~�>J����F��$�5�x�aY�Ƚ}g�	��g�[�e<|kQ>�Ĺ���J�?��"��e$���Gbc5�MrQ�P���?~�UÐ��r:K[q���A(K�s2]�͂��J���+�pJZ�a���������-5Q3UANk���Z��!T�[OK��n4v6��/��C��A�/{� �~�vV�70!*@̹�r����z�j?/��:��O���.��٘����G�5�c6�E��2Q����-+�+t^Pd�ֳ3/#�c���{��b���7{_���$��E��(�(�ǴJP�&&\%g�7(��B���(�uR^~�H��ӣ��1�F�8]w�ɉ3��F̰Ǆ ��7-xh�5�����oaF�ؖV�b0�=ӱ��ڀ��NZ�
�6�֒�������/$ƫW�:���w�r�7έ�����K��%ܶ�9��rۅ��?�H4{� AyTlf]U`����CX�^��ǯ���ϳ��z\��fʋ�[�E#���ﹽ�D3��$�_�j�cz�4鋫�����`��2Ɗ�E�w[9`��d��/��UF����_���ug9n����W�x��5�i��v�����������r�
iE��U�:���@M�	��z� �U�I�/u� gΩ�zUqI��djEf�(1K�֨r��+r�xRHȰ�4�-n��ܚq�֒xl�7���˘7�Z�('���Ks#�����5;+'�G����l�"�:"���z��_6�K���X\���P�	A�����e�����*5\k���>� +�a~�,;-�xX&I�s~;��{t�_��l�j�G�.����O���a"�ab.�i�Q�.�ޔ]=MYF���̆��jBi@B��)нe*`���p�z=^�$1H5�:�v6>U��)�}%�����4;d�٘�hp#�M��(#:9��N��q��a�OKH鱗HEpi���+���l����,*��P�9�d�+�-˄25V,�'�k�r�Z>v���O��Z��[{'��_j�lMujv�qNO�2�M�	v[H�
<�<�W��^�5>�Gg4�2 *[��� �:��h�U�}ֳ)o!�IyJ��6&��zPv�����@9����� ��	��Z蔼��gn!.���_�|�H5�³��ۅ��^?����ާ%��%ıu~,�x�	�m������|.���?b@��㚃�i6�%6"bB����btp�,�M/��cQ�ِ�����!$�͒2=��'�K�8!s�i�Z��	bdұ+��:���K�q'����7ӛbÁ��Lfve�Fh�v��2�_�1�;�s�/�oK��*l~��e���8��
�Q��:���+���:ew(Y��C��N
��3K���VE�Dr��F`ȍ>5'~��2�N��-z����t�<�nWT]53�!��~<�L�	E^�*
�}@H��S�󢭬ŵ���2T��wJ�Tq������(B��^�/}��F�#Ca��J���D��"�h	��y�Yo�)��Ɏ�c�&����OOR
g��w߾��A���Q}�W	���Yp��@x�՞�Sn�O.YP�t)�ģ�h������p���4,3��E��4�Pt\� s����m#��8Q��9�65�����89hT����KXf�c�W ��[�tZ�z�DP)���6y��v�е���?�k��s벮��z#>�:��e�~�`r�D�6ˋ���%oYi���I���X�G#en@C���Q4�����d� >��uwK�[o]'�=^��1�	M/�n�J^�a�e^Ơ ���R�\�;���7�-Y`U+�e���'�3���B�WΎ�)��g�O����x�W��0V.�JW~@��>2"�	YX�.��ʗ�Tjp��Z���2�Hc���N���ܙ��	p��	��-�h�T_��-���S����q#������K��p'4:��m�m��w��U^ᜨ����:��D��O��s&+O����Y���
�$('�;
Nu�"U�<�7��b]i���o��t�U�.P������{PO6�l���p/7 �(��r3����QUޙ\�4I�����R��$ާ�r��;��sz�c	��\|̓���$В���$���Q��V�y�!��P��	�,���,�I��O�H�	��t*xYmb�����A�h{������23��;z��z#A�֒6�����ك�t{��׎�/�����
t���M�8Q�hA�2pO�簱�/��kr%�,jn�]�J�L����[0�eu:�J��8GY',y��7��}�l�k�b�iՍ6�%�Q?����	��f��eh�4
�'vV/q=%�W��Jl��!U~	��f&6��ز�Z�si�3⅝��E�Ԧ���RAS�/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth))
		return 0;

	return 1;
}

/*
 * Remove the lock from the list of currently held locks - this gets
 * called on mutex_unlock()/spin_unlock*() (or on a failed
 * mutex_lock_interruptible()).
 */
static int
__lock_release(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 1;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * So we're all set to release this lock.. wait what lock? We don't
	 * own any locks, you've been drinking again?
	 */
	if (depth <= 0) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	/*
	 * Check whether the lock exists in the current stack
	 * of held locks:
	 */
	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	if (hlock->instance == lock)
		lock_release_holdtime(hlock);

	WARN(hlock->pin_count, "releasing a pinned lock\n");

	if (hlock->references) {
		hlock->references--;
		if (hlock->references) {
			/*
			 * We had, and after removing one, still have
			 * references, the current lock stack is still
			 * valid. We're done!
			 */
			return 1;
		}
	}

	/*
	 * We have the right lock to unlock, 'hlock' points to it.
	 * Now we remove it from the stack, and add back the other
	 * entries (if any), recalculating the hash along the way:
	 */

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	/*
	 * The most likely case is when the unlock is on the innermost
	 * lock. In this case, we are done!
	 */
	if (i == depth-1)
		return 1;

	if (reacquire_held_locks(curr, depth, i + 1, &merged))
		return 0;

	/*
	 * We had N bottles of beer on the wall, we drank one, but now
	 * there's not N-1 bottles of beer left on the wall...
	 * Pouring two of the bottles together is acceptable.
	 */
	DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged);

	/*
	 * Since reacquire_held_locks() would have called check_chain_key()
	 * indirectly via __lock_acquire(), we don't need to do it again
	 * on return.
	 */
	return 0;
}

static __always_inline
int __lock_is_held(const struct lockdep_map *lock, int read)
{
	struct task_struct *curr = current;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (read == -1 || !!hlock->read == read)
				return LOCK_STATE_HELD;

			return LOCK_STATE_NOT_HELD;
		}
	}

	return LOCK_STATE_NOT_HELD;
}

static struct pin_cookie __lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return cookie;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			/*
			 * Grab 16bits of randomness; this is sufficient to not
			 * be guessable and still allows some pin nesting in
			 * our u32 pin_count.
			 */
			cookie.val = 1 + (prandom_u32() >> 16);
			hlock->pin_count += cookie.val;
			return cookie;
		}
	}

	WARN(1, "pinning an unheld lock\n");
	return cookie;
}

static void __lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			hlock->pin_count += cookie.val;
			return;
		}
	}

	WARN(1, "pinning an unheld lock\n");
}

static void __lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (WARN(!hlock->pin_count, "unpinning an unpinned lock\n"))
				return;

			hlock->pin_count -= cookie.val;

			if (WARN((int)hlock->pin_count < 0, "pin count corrupted\n"))
				hlock->pin_count = 0;

			return;
		}
	}

	WARN(1, "unpinning an unheld lock\n");
}

/*
 * Check whether we follow the irq-flags state precisely:
 */
static noinstr void check_flags(unsigned long flags)
{
#if defined(CONFIG_PROVE_LOCKING) && defined(CONFIG_DEBUG_LOCKDEP)
	if (!debug_locks)
		return;

	/* Get the warning out..  */
	instrumentation_begin();

	if (irqs_disabled_flags(flags)) {
		if (DEBUG_LOCKS_WARN_ON(lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-off.\n");
		}
	} else {
		if (DEBUG_LOCKS_WARN_ON(!lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-on.\n");
		}
	}

#ifndef CONFIG_PREEMPT_RT
	/*
	 * We dont accurately track softirq state in e.g.
	 * hardirq contexts (such as on 4KSTACKS), so only
	 * check if not in hardirq contexts:
	 */
	if (!hardirq_count()) {
		if (softirq_count()) {
			/* like the above, but with softirqs */
			DEBUG_LOCKS_WARN_ON(current->softirqs_enabled);
		} else {
			/* lick the above, does it taste good? */
			DEBUG_LOCKS_WARN_ON(!current->softirqs_enabled);
		}
	}
#endif

	if (!debug_locks)
		print_irqtrace_events(current);

	instrumentation_end();
#endif
}

void lock_set_class(struct lockdep_map *lock, const char *name,
		    struct lock_class_key *key, unsigned int subclass,
		    unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_set_class(lock, name, key, subclass, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_set_class);

void lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_downgrade(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_downgrade);

/* NMI context !!! */
static void verify_lock_unused(struct lockdep_map *lock, struct held_lock *hlock, int subclass)
{
#ifdef CONFIG_PROVE_LOCKING
	struct lock_class *class = look_up_lock_class(lock, subclass);
	unsigned long mask = LOCKF_USED;

	/* if it doesn't have a class (yet), it certainly hasn't been used yet */
	if (!class)
		return;

	/*
	 * READ locks only conflict with USED, such that if we only ever use
	 * READ locks, there is no deadlock possible -- RCU.
	 */
	if (!hlock->read)
		mask |= LOCKF_USED_READ;

	if (!(class->usage_mask & mask))
		return;

	hlock->class_idx = class - lock_classes;

	print_usage_bug(current, hlock, LOCK_USED, LOCK_USAGE_STATES);
#endif
}

static bool lockdep_nmi(void)
{
	if (raw_cpu_read(lockdep_recursion))
		return false;

	if (!in_nmi())
		return false;

	return true;
}

/*
 * read_lock() is recursive if:
 * 1. We force lockdep think this way in selftests or
 * 2. The implementation is not queued read/write lock or
 * 3. The locker is at an in_interrupt() context.
 */
bool read_lock_is_recursive(void)
{
	return force_read_lock_recursive ||
	       !IS_ENABLED(CONFIG_QUEUED_RWLOCKS) ||
	       in_interrupt();
}
EXPORT_SYMBOL_GPL(read_lock_is_recursive);

/*
 * We are not always called with irqs disabled - do that here,
 * and also avoid lockdep recursion:
 */
void lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check,
			  struct lockdep_map *nest_lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquire(lock, subclass, trylock, read, check, nest_lock, ip);

	if (!debug_locks)
		return;

	if (unlikely(!lockdep_enabled())) {
		/* XXX allow trylock from NMI ?!? */
		if (lockdep_nmi() && !trylock) {
			struct held_lock hlock;

			hlock.acquire_ip = ip;
			hlock.instance = lock;
			hlock.nest_lock = nest_lock;
			hlock.irq_context = 2; // XXX
			hlock.trylock = trylock;
			hlock.read = read;
			hlock.check = check;
			hlock.hardirqs_off = true;
			hlock.references = 0;

			verify_lock_unused(lock, &hlock, subclass);
		}
		return;
	}

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_acquire(lock, subclass, trylock, read, check,
		       irqs_disabled_flags(flags), nest_lock, ip, 0, 0);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquire);

void lock_release(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_release(lock, ip);

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	if (__lock_release(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_release);

noinstr int lock_is_held_type(const struct lockdep_map *lock, int read)
{
	unsigned long flags;
	int ret = LOCK_STATE_NOT_HELD;

	/*
	 * Avoid false negative lockdep_assert_held() and
	 * lockdep_assert_not_held().
	 */
	if (unlikely(!lockdep_enabled()))
		return LOCK_STATE_UNKNOWN;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	ret = __lock_is_held(lock, read);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return ret;
}
EXPORT_SYMBOL_GPL(lock_is_held_type);
NOKPROBE_SYMBOL(lock_is_held_type);

struct pin_cookie lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return cookie;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	cookie = __lock_pin_lock(lock);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return cookie;
}
EXPORT_SYMBOL_GPL(lock_pin_lock);

void lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_repin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_repin_lock);

void lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_unpin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_unpin_lock);

#ifdef CONFIG_LOCK_STAT
static void print_lock_contention_bug(struct task_struct *curr,
				      struct lockdep_map *lock,
				      unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=================================\n");
	pr_warn("WARNING: bad contention detected!\n");
	print_kernel_ident();
	pr_warn("---------------------------------\n");
	pr_warn("%s/%d is trying to contend lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no locks held!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static void
__lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	int i, contention_point, contending_point;

	depth = curr->lockdep_depth;
	/*
	 * Whee, we contended on this lock, except it seems we're not
	 * actually trying to acquire anything much at all..
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, ip);
		return;
	}

	if (hlock->instance != lock)
		return;

	hlock->waittime_stamp = lockstat_clock();

	contention_point = lock_point(hlock_class(hlock)->contention_point, ip);
	contending_point = lock_point(hlock_class(hlock)->contending_point,
				      lock->ip);

	stats = get_lock_stats(hlock_class(hlock));
	if (contention_point < LOCKSTAT_POINTS)
		stats->contention_point[contention_point]++;
	if (contending_point < LOCKSTAT_POINTS)
		stats->contending_point[contending_point]++;
	if (lock->cpu != smp_processor_id())
		stats->bounces[bounce_contended + !!hlock->read]++;
}

static void
__lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	u64 now, waittime = 0;
	int i, cpu;

	depth = curr->lockdep_depth;
	/*
	 * Yay, we acquired ownership of this lock we didn't try to
	 * acquire, how the heck did that happen?
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, _RET_IP_);
		return;
	}

	if (hlock->instance != lock)
		return;

	cpu = smp_processor_id();
	if (hlock->waittime_stamp) {
		now = lockstat_clock();
		waittime = now - hlock->waittime_stamp;
		hlock->holdtime_stamp = now;
	}

	stats = get_lock_stats(hlock_class(hlock));
	if (waittime) {
		if (hlock->read)
			lock_time_inc(&stats->read_waittime, waittime);
		else
			lock_time_inc(&stats->write_waittime, waittime);
	}
	if (lock->cpu != cpu)
		stats->bounces[bounce_acquired + !!hlock->read]++;

	lock->cpu = cpu;
	lock->ip = ip;
}

void lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_contended(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_contended(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_contended);

void lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquired(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_acquired(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquired);
#endif

/*
 * Used by the testsuite, sanitize the validator state
 * after a simulated failure:
 */

void lockdep_reset(void)
{
	unsigned long flags;
	int i;

	raw_local_irq_save(flags);
	lockdep_init_task(current);
	memset(current->held_locks, 0, MAX_LOCK_DEPTH*sizeof(struct held_lock));
	nr_hardirq_chains = 0;
	nr_softirq_chains = 0;
	nr_process_chains = 0;
	debug_locks = 1;
	for (i = 0; i < CHAINHASH_SIZE; i++)
		INIT_HLIST_HEAD(chainhash_table + i);
	raw_local_irq_restore(flags);
}

/* Remove a class from a lock chain. Must be called with the graph lock held. */
static void remove_class_from_lock_chain(struct pending_free *pf,
					 struct lock_chain *chain,
					 struct lock_class *class)
{
#ifdef CONFIG_PROVE_LOCKING
	int i;

	for (i = chain->base; i < chain->base + chain->depth; i++) {
		if (chain_hlock_class_idx(chain_hlocks[i]) != class - lock_classes)
			continue;
		/*
		 * Each lock class occurs at most once in a lock chain so once
		 * we found a match we can break out of this loop.
		 */
		goto free_lock_chain;
	}
	/* Since the chain has not been modified, return. */
	return;

free_lock_chain:
	free_chain_hlocks(chain->base, chain->depth);
	/* Overwrite the chain key for concurrent RCU readers. */
	WRITE_ONCE(chain->chain_key, INITIAL_CHAIN_KEY);
	dec_chains(chain->irq_context);

	/*
	 * Note: calling hlist_del_rcu() from inside a
	 * hlist_for_each_entry_rcu() loop is safe.
	 */
	hlist_del_rcu(&chain->entry);
	__set_bit(chain - lock_chains, pf->lock_chains_being_freed);
	nr_zapped_lock_chains++;
#endif
}

/* Must be called with the graph lock held. */
static void remove_class_from_lock_chains(struct pending_free *pf,
					  struct lock_class *class)
{
	struct lock_chain *chain;
	struct hlist_head *head;
	int i;

	for (i = 0; i < ARRAY_SIZE(chainhash_table); i++) {
		head = chainhash_table + i;
		hlist_for_each_entry_rcu(chain, head, entry) {
			remove_class_from_lock_chain(pf, chain, class);
		}
	}
}

/*
 * Remove all references to a lock class. The caller must hold the graph lock.
 */
static void zap_class(struct pending_free *pf, struct lock_class *class)
{
	struct lock_list *entry;
	int i;

	WARN_ON_ONCE(!class->key);

	/*
	 * Remove all dependencies this lock is
	 * involved in:
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		entry = list_entries + i;
		if (entry->class != class && entry->links_to != class)
			continue;
		__clear_bit(i, list_entries_in_use);
		nr_list_entries--;
		list_del_rcu(&entry->entry);
	}
	if (list_empty(&class->locks_after) &&
	    list_empty(&class->locks_before)) {
		list_move_tail(&class->lock_entry, &pf->zapped);
		hlist_del_rcu(&class->hash_entry);
		WRITE_ONCE(class->key, NULL);
		WRITE_ONCE(class->name, NULL);
		nr_lock_classes--;
		__clear_bit(class - lock_classes, lock_classes_in_use);
		if (class - lock_classes == max_lock_class_idx)
			max_lock_class_idx--;
	} else {
		WARN_ONCE(true, "%s() failed for class %s\n", __func__,
			  class->name);
	}

	remove_class_from_lock_chains(pf, class);
	nr_zapped_classes++;
}

static void reinit_class(struct lock_class *class)
{
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	memset_startat(class, 0, key);
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
}

static inline int within(const void *addr, void *start, unsigned long size)
{
	return addr >= start && addr < start + size;
}

static bool inside_selftest(void)
{
	return current == lockdep_selftest_task_struct;
}

/* The caller must hold the graph lock. */
static struct pending_free *get_pending_free(void)
{
	return delayed_free.pf + delayed_free.index;
}

static void free_zapped_rcu(struct rcu_head *cb);

/*
 * Schedule an RCU callback if no RCU callback is pending. Must be called with
 * the graph lock held.
 */
static void call_rcu_zapped(struct pending_free *pf)
{
	WARN_ON_ONCE(inside_selftest());

	if (list_empty(&pf->zapped))
		return;

	if (delayed_free.scheduled)
		return;

	delayed_free.scheduled = true;

	WARN_ON_ONCE(delayed_free.pf + delayed_free.index != pf);
	delayed_free.index ^= 1;

	call_rcu(&delayed_free.rcu_head, free_zapped_rcu);
}

/* The caller must hold the graph lock. May be called from RCU context. */
static void __free_zapped_classes(struct pending_free *pf)
{
	struct lock_class *class;

	check_data_structures();

	list_for_each_entry(class, &pf->zapped, lock_entry)
		reinit_class(class);

	list_splice_init(&pf->zapped, &free_lock_classes);

#ifdef CONFIG_PROVE_LOCKING
	bitmap_andnot(lock_chains_in_use, lock_chains_in_use,
		      pf->lock_chains_being_freed, ARRAY_SIZE(lock_chains));
	bitmap_clear(pf->lock_chains_being_freed, 0, ARRAY_SIZE(lock_chains));
#endif
}

static void free_zapped_rcu(struct rcu_head *ch)
{
	struct pending_free *pf;
	unsigned long flags;

	if (WARN_ON_ONCE(ch != &delayed_free.rcu_head))
		return;

	raw_local_irq_save(flags);
	lockdep_lock();

	/* closed head */
	pf = delayed_free.pf + (delayed_free.index ^ 1);
	__free_zapped_classes(pf);
	delayed_free.scheduled = false;

	/*
	 * If there's anything on the open list, close and start a new callback.
	 */
	call_rcu_zapped(delayed_free.pf + delayed_free.index);

	lockdep_unlock();
	raw_local_irq_restore(flags);
}

/*
 * Remove all lock classes from the class hash table and from the
 * all_lock_classes list whose key or name is in the address range [start,
 * start + size). Move these lock classes to the zapped_classes list. Must
 * be called with the graph lock held.
 */
static void __lockdep_free_key_range(struct pending_free *pf, void *start,
				     unsigned long size)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i;

	/* Unhash all classes that were created by a module. */
	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			if (!within(class->key, start, size) &&
			    !within(class->name, start, size))
				continue;
			zap_class(pf, class);
		}
	}
}

/*
 * Used in module.c to remove lock classes from memory that is going to be
 * freed; and possibly re-used by other modules.
 *
 * We will have had one synchronize_rcu() before getting here, so we're
 * guaranteed nobody will look up these exact classes -- they're properly dead
 * but still allocated.
 */
static void lockdep_free_key_range_reg(void *start, unsigned long size)
{
	struct pending_free *pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	pf = get_pending_free();
	__lockdep_free_key_range(pf, start, size);
	call_rcu_zapped(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	/*
	 * Wait for any possible iterators from look_up_lock_class() to pass
	 * before continuing to free the memory they refer to.
	 */
	synchronize_rcu();
}

/*
 * Free all lockdep keys in the range [start, start+size). Does not sleep.
 * Ignores debug_locks. Must only be used by the lockdep selftests.
 */
static void lockdep_free_key_range_imm(void *start, unsigned long size)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_free_key_range(pf, start, size);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_free_key_range(void *start, unsigned long size)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_free_key_range_imm(start, size);
	else
		lockdep_free_key_range_reg(start, size);
}

/*
 * Check whether any element of the @lock->class_cache[] array refers to a
 * registered lock class. The caller must hold either the graph lock or the
 * RCU read lock.
 */
static bool lock_class_cache_is_registered(struct lockdep_map *lock)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i, j;

	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			for (j = 0; j < NR_LOCKDEP_CACHING_CLASSES; j++)
				if (lock->class_cache[j] == class)
					return true;
		}
	}
	return false;
}

/* The caller must hold the graph lock. Does not sleep. */
static void __lockdep_reset_lock(struct pending_free *pf,
				 struct lockdep_map *lock)
{
	struct lock_class *class;
	int j;

	/*
	 * Remove all classes this lock might have:
	 */
	for (j = 0; j < MAX_LOCKDEP_SUBCLASSES; j++) {
		/*
		 * If the class exists we look it up and zap it:
		 */
		class = look_up_lock_class(lock, j);
		if (class)
			zap_class(pf, class);
	}
	/*
	 * Debug check: in the end all mapped classes should
	 * be gone.
	 */
	if (WARN_ON_ONCE(lock_class_cache_is_registered(lock)))
		debug_locks_off();
}

/*
 * Remove all information lockdep has about a lock if debug_locks == 1. Free
 * released data structures from RCU context.
 */
static void lockdep_reset_lock_reg(struct lockdep_map *lock)
{
	struct pending_free *pf;
	unsigned long flags;
	int locked;

	raw_local_irq_save(flags);
	locked = graph_lock();
	if (!locked)
		goto out_irq;

	pf = get_pending_free();
	__lockdep_reset_lock(pf, lock);
	call_rcu_zapped(pf);

	graph_unlock();
out_irq:
	raw_local_irq_restore(flags);
}

/*
 * Reset a lock. Does not sleep. Ignores debug_locks. Must only be used by the
 * lockdep selftests.
 */
static void lockdep_reset_lock_imm(struct lockdep_map *lock)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_reset_lock(pf, lock);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_reset_lock(struct lockdep_map *lock)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_reset_lock_imm(lock);
	else
		lockdep_reset_lock_reg(lock);
}

/*
 * Unregister a dynamically allocated key.
 *
 * Unlike lockdep_register_key(), a search is always done to find a matching
 * key irrespective of debug_locks to avoid potential invalid access to freed
 * memory in lock_class entry.
 */
void lockdep_unregister_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head = keyhashentry(key);
	struct lock_class_key *k;
	struct pending_free *pf;
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        free cgroup_mutex mutex_lock mutex_unlock system_wq queue_work_on seq_printf __stack_chk_guard bpf_stats_enabled_key __x86_indirect_thunk_ecx sched_clock __x86_indirect_thunk_esi debug_smp_processor_id __per_cpu_offset __stack_chk_fail bpf_sysctl_set_new_value memcpy memset bpf_sysctl_get_current_value bpf_sysctl_get_new_value migrate_disable __rcu_read_lock __rcu_read_unlock migrate_enable strscpy strnlen fortify_panic bpf_sysctl_get_name bpf_ktime_get_coarse_ns_proto bpf_strtol_proto bpf_get_current_cgroup_id_proto bpf_get_current_uid_gid_proto bpf_base_func_proto bpf_strtoul_proto bpf_event_output_data_proto bpf_get_local_storage_proto __kmalloc css_next_descendant_pre percpu_ref_is_zero bpf_prog_put __x86_indirect_thunk_ebx bpf_prog_array_alloc bpf_prog_array_free static_key_slow_dec __x86_indirect_thunk_edx bpf_tcp_sock_proto bpf_sk_setsockopt_proto bpf_sk_storage_delete_proto bpf_sk_getsockopt_proto bpf_sk_storage_get_proto bpf_cgroup_storage_unlink bpf_cgroup_storage_free percpu_ref_exit cgroup_storage_lookup bpf_cgroup_storage_link kmalloc_caches kmem_cache_alloc_trace static_key_slow_inc bpf_cgroup_storage_alloc cgroup_bpf_offline percpu_ref_kill_and_confirm cgroup_bpf_inherit percpu_ref_init cgroup_bpf_prog_attach cgroup_get_from_fd bpf_prog_get_type_dev cgroup_bpf_prog_detach cgroup_bpf_link_attach bpf_link_init bpf_link_prime bpf_link_settle bpf_link_cleanup cgroup_bpf_prog_query bpf_prog_array_length bpf_prog_array_copy_to_user __cgroup_bpf_check_dev_permission __cgroup_bpf_run_filter_sysctl __kmalloc_track_caller __x86_indirect_thunk_edi __cgroup_bpf_run_filter_setsockopt __check_object_size _copy_from_user lock_sock_nested release_sock __cgroup_bpf_run_filter_getsockopt __get_user_4 __put_user_4 __cgroup_bpf_run_filter_getsockopt_kern cg_sockopt_prog_ops cg_sockopt_verifier_ops cg_sysctl_prog_ops cg_sysctl_verifier_ops cg_dev_verifier_ops cg_dev_prog_ops      F     G  !   F  *   G  A   F  �  F    J  1  F  Q  F  a  F  q  F  {  L  �  M  �  L  �  N  �  F  �  O  �      F    L    M  +  L  0  N  :    @  Q  m  R  �  T  �  R  �  U  �  V  �  W  �  X  �  U  �  Y  �  F  	  [  A  F  j  [  �  \  �  \  �  F  �  F  	  \  a	  F  x	  R  
  _  
  `  %
  G  m
  G  x
  a  }
  b  �
  R  �
  _    `    G  g  G  r  a  w  b  �  Y  �  F  �  c  	      d  7    <  c  �  F  �  R  �  c  �  R    Y  !  F  �  F  !  F  !  F  :  :  V  g  b  h  z  :  �  :  �  i  �  j  �  l  �  :  �  :  �  :    m    n  !  F  b  o  �  F  �  L  �  M  �  :  N  p  v  p  �  q  �  L  �  N  1  p  F  r  q  F  �  R  �  _  �  `  �  G  �  T    G    a    b  -  R  A  U  R  V  \  W  c  X  x  U  �  Y  �  F  �  R  �  _  �  `  �  G  "  T  N  G  Y  a  ^  b  m  R  �  U  �  V  �  W  �  X  �  U  �  Y  �  F  
  R  p  _  u  `  ~  G  �  T  �  G  �  a  �  b    R  !  U  4  s  >  W  E  X  Z  U  �  Y  �  F  �  i  �  :  �  j    :    m  !  n  1  F  �  t  q  F  �  p  �  q  �  p  �  p  �  p     q    u  *  p  E  p  W  u  j  p  �  F  �  :  �  K  �  r  �  @  �  v    F    L    M  F  L  K  N  a  `  �  L  �  w  �  F  �  F    F    x  3  :  J  y  a  z  u  {  �  :  �  i  �  j  �  |  �  :  �  m    n    F    @  /  L  4  M  l  K  s  v  �  r  �  `  �  a  �  u  �  @  #  }  ,  ~  >  L  C  N  V  `  h  a  {    �  T  �  `  �  w  !  w  1  F  M  R  �  :  �!  �  �!  R  �"  r  �"  �  �"  �  �"  �  �"  �  *#  @  /#  �  E#  �  �#  ~  �#  ~  �#  ~  �#  ~  $  K   $  ~  )$  ~  =$  Y  Q$  F  q$  `  �$  a  �$  F  �$    �$  R  �$  �  %  `  %  a  v%  u  �%  `  �%  a  �%    �%  R  �%  u  /&  w  9&  Y  A&  F  U&  �  t&  L  &  M  �&  L  �&  N  �&  �  �&  L  �&  M  �&  L  �&  N  '  r  '  `  "'  a  Q'  `  `'  a  ELF                      �      4     (               ������@\�����1�������S�ù   ��p��d�    �D$�C$j�T$������D$P�D$P�D$P�D$P�D$P�D$P�D$PSh    �������(�D$d+    u��1�[�������t& �����UW��V��S�Ã����   d�    �D$1��=     �T$�L$�   j �   ��T$�����Z��������D$d+    u��[^_]��������������wvVS�ÉЃ���tX���   �������   ���� ���   ��?��߈��   	�	։���1��=�����   �ȉ��,���1�[^Í�&    ������&    f������Í�&    �v �����UWVS�Ë@�P�R�����%   =   �<   �����[^_]�                                 �                                                                                                                                 �      �      6%s: Status: SA00=%02x SA01=%02x SA02=%02x SA03=%02x SA04=%02x SA05=%02x SA06=%02x
    7%s: write reg: %02x val: %02x
    3%s: I/O error write 0x%02x/0x%02x
    6%s %d-%04x: chip found @ 0x%x (%s)
 ����p������WVSh|   ���������   ��P��P�CpPhX   ���������   �S�C��  Q� QP��   �CT�0h�   ������C��  ��   ������Ń���u
������  �`   �ڍ��   1�������    �   fǅ�     ����   �ډ���   ��u�1��          �-)6�VH :�D`R�S`         upd64083                                                        �           �                                                                                                                                                                 debug ������    �    ������    �����upd64083 parm=debug:Debug level (0-1) parmtype=debug:bool license=GPL author=T. Adachi, Takeru KOMORIYA, Hans Verkuil description=uPD64083 driver             ��      GCC: (GNU) 11.2.0           GNU  �        �                                  ��                                                                          |                  	 0   �   o     7           ?       @    	 S      �                   f   �  3     u   @   �    	 �   `                       �            �       �     �       
                   �       0     �   �   0     �   �   P                          /           =         O  1        f  =   0     |  m        �             �             �             �             �             �                                       +             @             N             b           n             }      
     �      0     �              upd64083.c upd64083_remove upd64083_log_status upd64083_write upd64083_write.cold upd64083_s_routing upd64083_probe upd64083_probe.cold upd64083_ops upd64083_driver_init upd64083_driver upd64083_driver_exit upd64083_id upd64083_core_ops upd64083_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev __stack_chk_guard i2c_transfer_buffer_flags _printk __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__upd64083_id_device_table param_ops_bool        	   !        %   "  7   #  g     l   $  z   "  �   %  �      �   "  �     �   #  �   "  �   %       �     �  &  �     �     �                   h                                            $  /     4   $  _     d   $  v   '  �     �   (  �     �          <     �     �     `     l     �     �                   )     *          ,           )     /        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rel.text.unlikely .rel.rodata .rel.init.text .rel.exit.text .rodata.str1.1 .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   �                    	   @       8  �               )                �                   %   	   @       �                  /             �                    8             �                    4   	   @          0               E      2       �  �                 X             �  �                  T   	   @       0  x      	         k             `                    g   	   @       �                  w             v                    s   	   @       �                  �             �  
                  �   	   @       �                 �      2       �  	                 �             �  �                  �             (                    �   	   @       �                  �      0       <                   �              O                     �             P  (                                x                  	              x	  �                                 �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              �'  w  �'  w  �'  F  �'  �  �'  �  �'  L  �'  M  �'  L  (  N  (  r  !(  `  0(  a  Q(  M  d(  L  i(  N  �(  w  �(  F  �(  R  �(  �  �(  �  �(  �  �(  :  �(  �  
)  �  )  L  )  M  4)  L  9)  N  G)  �  T)  R  k)  K  v)  `  �)  a  �)  �  �)  w  �)  Y  �)  F  �)  R  �)  �  *  L  *  M  .*  :  w*  �  �*  �  �*  �  &+  �  h+  L  m+  N  ~+  R  �+  `  �+  a  g,  w  {,  �  �,  Y  �,  F  �,  R  �,  G  �,  `  �,  _  �,  `  ,-  T  X-  G  c-  a  h-  b  q-  a  |-  R  �-  U  �-  V  �-  W  �-  X  �-  U  �-  Y  .  F  -.  R  t.  �  �.  �  �.  `  �.  G  �.  _  �.  `  ,/  T  X/  G  c/  a  h/  b  q/  a  z/  K  �/  K  �/  R  �/  U  �/  V  �/  W  �/  X   0  U  /0  K  _0  �  �0  Y  �0  F  11  R  �1  �  �1  �  �1  K  �1  R  2  �  "2  _  *2  `  02  G  d2  T  �2  G  �2  a  �2  b  �2  �  !3  U  23  V  <3  W  C3  X  X3  U  �3  o  �3  [  �3  Y  4  F  64  R  r4  G  �4  �  �4  �  �4  �  �4  �  5  _  
5  `  5  G  D5  T  p5  G  {5  a  �5  b  �5  �  �5  �  �5  �  �5  �  �5  K  �5  R  16  U  B6  V  L6  W  S6  X  h6  U  �6  Y  �6  F  �6  R  7  G  7  _  7  `  T7  T  �7  G  �7  a  �7  b  �7  R  �7  U  �7  V  �7  W  �7  X   8  U  78  Y  f  K  �  P  (    �  k  �  k  r  a  �  N  �  k  �  a  h$  �  �$  �      @                A                B     	     
      C                D                                                           $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     �     �     �     �     �     �     �     �     �     �     �     �     �     �     �                   S               S                S  $     (     ,   S  0     4     8   S  <     @     D   S  H     L     P   S  T     X     \   S  `     d     h   S                                 $     (     0     4        :     e                                                 $     (     ,     0     4     8     @     `   I  �     �     �     �   Z     ^  @  ]  �  f  �    �    �  H     E  \    `    d    h    l    p        D     C     B     A     @   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab+cgroup_bpf_enabled_key .rel___ksymtab+__cgroup_bpf_run_filter_skb .rel___ksymtab+__cgroup_bpf_run_filter_sk .rel___ksymtab+__cgroup_bpf_run_filter_sock_addr .rel___ksymtab+__cgroup_bpf_run_filter_sock_ops .rel__mcount_loc .rodata.str1.4 .rel__jump_table .rodata.str1.1 .rel__bug_table .rel.text.unlikely .rel.smp_locks .rel.rodata .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                                         @   ;8                    	   @       pW  �  #            %             {8                     +             �8  �                   0      2       �8  �                 F             9                    B   	   @       `e     #            l             $9                    h   	   @       xe     #            �             09                    �   	   @       �e     #   
         �             <9                    �   	   @       �e     #            �             H9                    �   	   @       �e     #            "            T9  �                    	   @       �e  x  #            /     2       :  !                 B            4:  l                  >  	   @       Pg  �   #            O     2       �:  I                 b            �:  <                  ^  	   @       (h  P   #            r            %;  
                  n  	   @       xh     #            �            0;  0                  �  	   @       �h  `   #            �            `;  t                  �  	   @       �h  �   #            �            �=                    �  	   @       �i  (   #            �     0       �=                   �             �=                     �            �=  (                                $>  �
  $   @         	              �H  �                               �i  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_drivers/media/i2c/upd64083.o := gcc -Wp,-MMD,drivers/media/i2c/.upd64083.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"upd64083"' -DKBUILD_MODNAME='"upd64083"' -D__KBUILD_MODNAME=kmod_upd64083 -c -o drivers/media/i2c/upd64083.o drivers/media/i2c/upd64083.c 

source_drivers/media/i2c/upd64083.o := drivers/media/i2c/upd64083.c

deps_drivers/media/i2c/upd64083.o := \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
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
    $(wildcard in