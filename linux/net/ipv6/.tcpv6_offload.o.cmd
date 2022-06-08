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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      R  ¿  º     a      
     u  $        Ä     i     õ  ê  å     ≤     ¸     Õ     ˙     ﬂ     <     ˛  ‡  <       Ä  <     -  @  <     P  ¿   <     o     <     Ñ     r     ñ  †  Õ    ©      #     ¥  ∞  w        0  7    ‚  p      ˘  Ä  Ö        °     ,  ¿       D  ‡       [     
    q  `   <     î  0      ®  \                     Ω           ˚          :          r          ´          X       ∏     «   `	  W    9  ∞  6    ∏    ≥    <  p  6    ﬂ           Ó             ˘                            3     2             ;             A             N             Y             f             p             ~             â             õ             ±                           ÷             Ô                                       (  ∞  ä     A             H             O  ¿  (     l    n     Ö             ï             •             ∑             ∆             Œ             ÷             ‰  Ä  ô     ¯             	             '	             G	             e	             y	             ã	             ß	             √	             Õ	             Â	             ¯	             
             
             3
             G
             [
             t
             á
             ü
             ª
             ”
             Ï
                                       .             D             \             k             Ç             ñ             Ø  P$  Q     ¬             ﬁ  ∞$  ç    Ò               @&  b                 +             A  ∞'  Ÿ     X  ê(  E    o             }             å             ú             ≠  ‡)  ¨    √             Á             Ÿ             ı  ê,  b       .  Â    6             M             f  0  
    â             ù             ≠             æ             À   4  û    Ó             ˚               †6  õ    0  ,        D  0        \  ú        o  †        Ü  º       ö  ‘        cgroup.c __kstrtab_cgroup_bpf_enabled_key __kstrtabns_cgroup_bpf_enabled_key __ksymtab_cgroup_bpf_enabled_key __kstrtab___cgroup_bpf_run_filter_skb __kstrtabns___cgroup_bpf_run_filter_skb __ksymtab___cgroup_bpf_run_filter_skb __kstrtab___cgroup_bpf_run_filter_sk __kstrtabns___cgroup_bpf_run_filter_sk __ksymtab___cgroup_bpf_run_filter_sk __kstrtab___cgroup_bpf_run_filter_sock_addr __kstrtabns___cgroup_bpf_run_filter_sock_addr __ksymtab___cgroup_bpf_run_filter_sock_addr __kstrtab___cgroup_bpf_run_filter_sock_ops __kstrtabns___cgroup_bpf_run_filter_sock_ops __ksymtab___cgroup_bpf_run_filter_sock_ops sysctl_convert_ctx_access cg_sockopt_convert_ctx_access cg_sockopt_get_prologue bpf_cgroup_link_dealloc bpf_cgroup_link_fill_link_info cgroup_bpf_release_fn cgroup_bpf_release bpf_cgroup_link_show_fdinfo __bpf_prog_run_save_cb copy_sysctl_value sysctl_cpy_dir sysctl_cpy_dir.cold __func__.0 cgroup_dev_is_valid_access sysctl_is_valid_access cg_sockopt_is_valid_access sysctl_func_proto bpf_sysctl_get_new_value_proto bpf_set_retval_proto bpf_sysctl_get_name_proto bpf_sysctl_get_current_value_proto bpf_sysctl_set_new_value_proto bpf_get_retval_proto sockopt_alloc_buf cgroup_bpf_replace CSWTCH.197 cgroup_dev_func_proto compute_effective_progs update_effective_progs __cgroup_bpf_detach bpf_cgroup_link_release.part.0 bpf_cgroup_link_release bpf_cgroup_link_detach cg_sockopt_func_proto bpf_get_netns_cookie_sockopt_proto __cgroup_bpf_attach bpf_cgroup_link_lops __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_ops491 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_addr490 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sk489 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_skb488 __UNIQUE_ID___addressable_cgroup_bpf_enabled_key483 bpf_get_retval __fentry__ current_task bpf_set_retval bpf_get_netns_cookie_sockopt init_net kµdN›7“=åh#9√s˝äÅΩ ÈyOÅT±,[9!µ&{¨S_1$¥‘ >æ¶œw˙ë*-'Hå–(ä•‚Nƒ–lV¡a!J–{zÌù˜ÁhñXæC„m'Mí‚ÇZÏà©ıöﬁÿè≈InpLù27‰&87^Vc%˘@À§˜BZ—ñì8.n∆w5!}áG›	s ◊ı•êR!ãy3á"∂@∆≤—È@£*VîErîÈT‹ZO–S∑êGÆ≈Ïåª‡µª˝ëSıjÏYqœ»Ößè¿¬˙&Ïe6MÀEƒ§Æh˚!ÌQk’§\K»zoO<Ò…˝ê@Uç‚ïx∏aT¢ºﬁ"õîEs=ôÆ@‹˛´nêˆ$_∂<)π	™hîﬂj(ÃéÆ¶"9≤|ì™π«ﬁ¢x'Ìcü=‚¸€EïY≠rB‡‘l3*`ü2Zå&D›<H∏çç‘˝†£5Û2H[Û&Va‚VˇTè~Æk˝sÍx¢5⁄ó^2⁄rüßá';DŸ1ˇ«FÌ‚üß;À*?>±≈E∆G.•$_ÂiX÷ùõ¿—%EÒÓ¸@ÙÓWFñÍ+O`Cò‰<K{BôgûÔsƒ	*
J&πi˚g“ÇÍÖN:∫d›≤¬x∞∏S‡P·\9
∞-µájpÖ»Ö«±Ú⁄ÄQl°^ƒÅ|@åA"[äËàvFühnp‡$±2ﬂ7†Rµ€ÌŸår€1PO»	À¯©ƒ<ÿúa)0∆;ªWÊBâ[Çt$	ô∂›õ
Kÿ≥•YôDg(ëÿ¯oµ2◊m»K‡5û´ŒÇ*ó¥ä≠“µÔ9àÙãî«H;Ø˚GŸ˘?ÔëÙñ–ÛÓyÎ»Âå0_∑SxKªfˇ˜πÛêé˘ˇº¥[À«%˝s◊ÜL±@M8ê∂3ÜzµìÊ´*mÌ‘€·€t3∫Áâ˚@	/º=vÿå+[˛?ríÏ£ª”n˜∆ë®≥Œi´Ñ8ë Ûí›’ß˘á"c‹@â7ÉmX˜|T ó/›Z€’ºÀë^ŒN"ÒÏ+Á≠î=&	,-®∂¥èxË¿πˇƒ“,~7\ûµC˙Î}jP08û¢Î˝Øö2Ì&+ñ&'¡>õ;ú¸Ω/pÓ7#JÚdçN√e˘5Ä®Lf„ÂÊÍàaöº,¯ΩÛ~?¸|≤">ïDk©˘R˘◊Úy.⁄\U‡'F¡eΩÚ mFg‡ΩcFè78pu‚DKtAˇçl|u2n√ÉÁÃbÃÇ∫Ï≥π–Ù&iUîwfK,OQöBr‰"ﬂîDiÍ¢ÛKúô i=öÏ◊[πÚ[k
†EEß‰lZ+~Ü–ìà›øÃã™o…g7≤.∏Ö€ï˘ØÅI«9®îMbÅ˚÷ç˙≠Ò©Tn¸&Íbà≠Oã–BDùTdâ6ŸáåÜ™s˛Ó+Hõ‰>’›ÿT^˛Ò%«k Yö⁄ë±»;hºÿaâYwG∂~∆Ì∏£‰XN‘óK_]nç≤t=©≈‚ÎSzh∆Å~◊>´á}crq«mqf:¯0÷@•∆Bl5%!¨√‹ÂÚ∑ìõµjH0C‰ö?èƒj¬8BÜZ•„ÛËô4B¨êP—{0(c:îHn5≥ïá/OJòTuÕ§†7Ù¯ã±˘Aˆ„BÊ^∫÷PTuÿb«fà’Ü`ÛiwJﬁ¿s∂øÊ|¶¶ŒâÚÏ=*∏@s‡∑π0õæJÊı¡°ÊpÜ‡Å˘ËÉ¢{QÅM»¥∞íu∞€»i;6“◊Bú%SÁÔ–¸â®®°£ÓìçH)- ” R+∫ﬁZ2a1 {ÉóV>º0%˜ûô≤…$XÏ‰FU?ë˜u‘úså,t¢Ó+ ﬁ˝AÜ?dëÜ¿üáoì¿◊®àŒc7æµ∏Î	âAF‡ÀSq¶ÍﬁìÍ]]`≤óÕÈu8
¯òaÇ9πh|AQXûäGU°	ü¢˙V-&:ø.§π\¬à‡5‚ØÛ›3ÇØ˚[ô_§†\]È]π˘Û”-É~˜wõ rı‚àª.‹k=°UD=á@ä{Ω¶k%œºö≥ÖH[i|Qjõ„T…€Ã›Q◊ñí∏˛µ«Iﬂ]ô÷JÆˇÈê˘a˛Ë£Gë¯Ö≈À_O≤‘Gå±X˜œõmLnª%ºc4I#Ú<øK„	‹qç∏Ø“4VΩårÎÒ,„â)’» ,£Îµ&¶+ß–π√ÍJo	ø∞%Ü◊¥*ò7êË	ÕÊKˆí•‘v®çƒZn˜∆µê”ø\Ú*L‘Ω˚¿ô}ZÚ{¸®qT¬_—2Gà~?í¬ìñTcçG®V8ÿ¥ˆPˇÏËt¿˚îÔ2∂◊Íπ7K÷.5aﬁ›á0√,ÉítecÆπÂ€Ülåu"Yˆ‡F·êØß?rÇ/≥Q,
˛ò„ã0öí}µjXöîûôÏ&Z˚≈;Äñ„ ±QíÊÙj<,a»Ñ¢J˘%*Øî1õ˙âQ2õ≤∫.00- ·nüª1‘˙`∫ ÁbKıÄ!†¡"¨Ω~R»åÀE«j¯£¿q¢x•F∂ÄZ”G¸!cUyùÿ|¬q–R°ÅÖî„ZÍ¶Ÿg•~«äO=®I¿±ÍÃãrã›NıBú5ïXy…mûr¸ú7≈!›Œ¯ãè?`Cÿ∞ó>Œ*  ˚∏Ú›ŒVÓÄ»q
∞”0ö5óã™NYâÕr anØ0êÃK‹‡˘Aø{
„z∞Q˙—”†Ëüv‹wmÉQ¶WÄ©	Y∏_Àô\‰]ñ7Nq,y(˜.˜Ú§%¢
 ˚<–[¸íV–|et[æR∆…“À }≠ÜU-—CÀ≥i›ü∑E'·í°@f$Õ*íIMGrI~l¯a∂ŸÓ⁄}√ömCdF9¯/µÄ±˙©≈Í‹®J"Pâ®j`5iœNMµb3‡{nr÷£¸!ôú˜ãíœíªRïÍhUy√j«1ØTÕTë˘ÓL¡7“
rQµ#öy£Ç[zq"◊√/$vˆ⁄R.™Gì©$¢IòÿEÒI⁄pU[Ö!ÚEtQh¿Äf
ã3Ωm_l?	QÜÙ'Vö∆:ìÚêøBAúÄ4·É~ˇâ˝.à(\∑?„÷äàµı@?zT8º[œ˚åPÏ)0´‚`‰∏Å∏üíÏé‚[u◊Ç…í€˘}ÜÉ"]ôo/ÛKﬂ∞tÍn*Â {hPñ4WX-∫Ñ£?}‡ﬁ§^¨[WF–ÒÈ◊K‘q®Œqí¶&¡gÉRXå§êmŸæèò©(dM6çù(L;ä_Ê>iüÈöÜ?G¡Ö≠ö`pà#ù^©≥q4t$m∂òéõ¥‘∑§rÙ±)&%Çˇ†´ÌE@ï€ö4» Lıæ&Î˝äX2“Z◊·o:Zê[¸#æ≥›Ö™∫ëì7<ÀÂ	ﬁ–e<|P\nÿ‰Æô_W•ô†òG˜MhHX–xç=§È+2z‰¿Ê–u@jÌÿ-aAâåÇxÇ(4OIÀ∫+6IÑ&ï˘eû`◊≈‘£R,¬„Dì‘º“œD'§øXãäm∆ ª≥¿)ó„/îÜ=>ºehn…ÀÛ,µ3««á–ÜYÏHñ#2øéjÍa∞«úŸ 1ÜM¡´Q”¨⁄Ê¬iuF–6[26}√b.„4pÒcí|™‚alºêu¢Ü˜z1 ô∂™4zuÛ»u'k»	ó-1H!Ú?>2‚ŸKÀÛÒ]®EÓOTó∏ëg{ªñ2“9@€®pÍΩ%]m£+õl <¢@Htﬁ*
ÿÅX#ºÎ≤{ª|b 4!òîéó7ﬁ˘∫BŒoc*†b_wR‰G„S7aÚØUñJÜ€nıÔ9ë1‘3∞$ñS·ûXI\ëôG}z∑CêtQO”ªP«©7Ç)‰¯":?w∏@∫◊X«OhÙ›ÑéØx«L€∞†Fµ}	K˜Ç®õÒ6iM^ÙJùS∂@É&Åú‹ùˆt~}h–ch||fIG)…»Lv'eh°∆¨Á«L)`ß°îÍò•áB)\‰~˛…$}ˇ©V6t∂Kﬂ/IèµgØÕÒMqü≈S‡Ô«u%?ÇK
\múÙáÏ…+∑k
˛O$¬Ÿ<õ˙Bbxäø(5«ˇ€=∑9Uêê ∫µ!˚‚1•/]Ò-0A∂“ªPè8#D∫4s¡ﬁR’ΩBπ‰Ö•ü^Ñ»K„÷◊ø(·|O,h"•:“∆áL∏t¯ÕntßÄÄ=V√ÅÊ';’˙*ı´ª7ˆE˝:≤∫`|ÔÔ¿(·tèBπAªSPtI](ä JlGÎ@¡–ÜåÛÂ-4ùôE÷á–HÍ £Lcktkqc~∫ÚWÅT4ëπ(IÓöËÜáΩÉößd]ª¡y"k∫é'Ná…¨æ˜f<îÛP}√~ä¸Wπîäâë@≠∫ÇÖÂ =Ë¬&,ÛNèY´Oƒ€Í¥H@S>®»Ã“∆Òˆª¥¢¨˙â≥j{Ÿ)Y%!⁄A'9â»rœ›È3çà∞â•ƒ£Ú«%˜ﬁJ‡Èº´ßÈµËñÒ($^„Ñvbœ∂¸8Øù-9hÙ’óÚôÈà-‘[MT
¶ö≈€U_ÛÜ7dˆâæ§ß,ŒH˜èÌbn'`ª¨’sB%’Ï≥|ºôZ=.j∂∂©ãbl√¨\¬ì®É÷$â÷X‹Wˇˇ}?&àµΩÁ‚°∆A¸„P˙˘A2í=—…|Û¶π9∞ÖÆZ$ÖkJ¡—£2A‰°‡‡˘IªmˇŸ‰Å-‘)œÕNìUy÷’
Z!VˇÉR(à]äQT%Ï∑W2f”¶	äí^L‹ˇ§oB@∫Ú2o_$ﬂ¬ˇrn∂‚	:‹¬÷¬Åop—#ıàZ„4nÙN˛m'úÜ$ÛMü
ÿ’#ñjµPÛ¸mˆa b7i¯:ô&ñıRd4€~ ◊§>ü"A¬3k!ﬁÈNhBíf¢B;ÖùP7Mm¬Ñí¿VÎ¨ÄE\D`3é·Øíól‡É#Hh>:=]oôòÇ#@§mí†Æ—·1C4≈td9|ÒÊ—l≥x±‘&<˙vÛ‘xW¿ÒDèBŸ ÁüsÂ4pCÎô’8ò{yL÷„óÍÌÌ„€Dc»uî˙jQ.®8⁄«L’Øo'˜˘ê‚@8TäÑüÀG¶·5õQüeFj‘Ö|å.»	ûûıGa(ÊYå	`9Vê~EeÑ”˙¢¯Á¸ˆ≤Ø@kN,c±l˘,6∏hŸJgêg˘*>:¸áªEké5ØOûôSM˛$»7¶Ïow»¿“Öø¯Eƒ√©õŒÅﬁj§	à?ÆgH«–Öwœ>VÔ-E’œÉ*∫$Ç˘eVç0Z=À°b–j÷pñÓçí2=ÙÀ,Sk"‡W=2≤„T"b~Eo.ûó;Æ6Q‰"˘9,^Ó£⁄G°˝YPúå∑Qzï√;0P_<2ß,|‘0√ñF}ŒºC¨±¿÷àôjÙ≈qOƒïë¥;bM\î]8ÜKF¯¢öV,§|xœ@ËpÏ
◊óLyéåˆøÉaÛ¡˘P˛K÷vﬂd≥
Ç∫2å4eYß tú»c4„∆9NÎ‡Æ-‘.¡{Ûø-3çu q{É•øŒÓc‘uãUˆ˜XœÔè	ŸΩ∑@AÁ
AIc`›oï¿Kã«>SêG"c#oní,Ö©ˇ±1s˘¸ámÿnÜ(!~ÚäÆSÿŒ™¿≤Í‘#V lØ) ¨ä·OV[ÿƒ¸»RUïlØﬁnÉõe6A£çl}Á'u<t
?¢ h)Yø"˚‰™u‡Èy,¿Û]…iFspŒ™xJ1tÁŒ6Á/‰adIxK“ñÎu	á¥√ÌS=—Ù#4Æ¿:ˆ]~j:
(´’Î¢◊óW˛í√p#ùUt\ÇÂ|)ï·›†cRßë©¶Í"Ô∑ˇU2÷‹„˙CÊlçs+q1Uƒ.e^Œ£bÌ}n5ÌU/¢´YΩâ„ôDH:5Cêõ{#¬
æ¶yÙ˘‘ Bhé	≈ËÙ¿W¥˙=5ú˚äk÷P⁄Ô< 5ìxa	•ÉYSm'TÚå@LƒìÀ	áΩ¨ôïiKª∫%8˘ ≥¡{ïÉ*àÓ>˜ù<wëlïﬁZYæºOÂùDò‘ùäqUu:^ÀÈÃúVEÊQ£%ìÜ¡m1«üÖq¯ {:z#cœTiUœ}) Á<Mî.B_û 3J÷BˇΩxµÀﬁ¯ò∂iØPl¿CMΩ;≤pÈ¢XHÇJÈ¯F&˝^eÌXS1)˙±Œñ≥aπn¿[y\¥§«lN√º.∏)dq–u¶1ﬁ`®ƒA√ø€€<0Ç@ÂP/Ä:∑êœp∂Øﬁ≠E´è˝› Êi˝Ñ33Ééwk∏8∂’"ä]X’¢Îﬁ$πÃ ﬂ–}d 7Äe°a ú∞al∫Øüº∆O¥¢™Ûkê◊´goÅ32jn æÿAâ¨sóz(•[aî7—∞ïcW6”YœÏ]àØYò},:u˜RN∑≥)¸∑p≤B	ŒWûõÇ*g©Y…òµÈWÄD¶≤R‘ÿ’]D,näJ ¿qó5êñπŸOÂ…◊UB`#«&Ibm<l8Õ˚Z◊‹J/L)g;ÕßZÎ¶/M∆+N∑K%qb?◊-RnªÅ°kë\#<îEÆÏhı±Â˘ùeü‡=‚ëÛTf-Äv√™~C=m2™n)–c§_‹ŒY K•röom¿Uª£:±F˚ÆÒñ˝Ã‘´+§‡ÌôA[=πˆ/◊<#†D9"µDt“U+/»“7ÿjkí[◊9X‡É|‘[<ﬂÙÎiÎ«	ôÒÿ9“	oÄ
j]ïfz⁄©Fƒvæ*˙ IQà≈‹)\√úGD!ä„Í;4ÙaûA´ƒe©˘≥æYÛXÅÅ[∞˜⁄‹ÂR5ÜsèVÕ„È∫“V%≠õ[ùo|yûÒæöñg¯^j›ûûµ$|ô–Ûºƒ≥Å*¨øy2¢É§3^S¿∏fÕIàø$ï¸‡◊õ¢*eÑƒËz⁄üK^–T⁄œ≤Z¶ˇ˙ñ0˜¸P¶ó˙®€ˇbLJıE"P'öcÁÆ´ê¶<∆Î¥˚;t*ÿ∆ÔÃfÔÕˆ6Ç‚ó»ÜçÌÅvœîˆƒ≥0¯s≥ˇoèﬁW·ÛP≥≥áa0ˆÓü+[ùà`VmOâˆæFëI∆vb%;ØT∑fçgÜÆa~¬Øª—ÃÚÉ¨ÆpÔ@w√®7$∏Ëzñ#≈âzqcÙmL}¸"Ê[£‘sõíqÊﬂmòH±±¯ìÈ∂g)G9Ææ€)æ˛≤è¡ò—à…ŸÄ‰CÒp∏©cÍÒfÿ?!¬âπM]ˇ\• º0aú∏òp≥ößWæo‚∏ˇrÿZ£2$<ˆ·%mÀøÚÍÑw# {ÖD≈ê£ 2oÅoC∫Wæ¶kî±)/ó≠I:∂ §¢\∆íí¬lr∂m“îêâŸAyvå(f≤Ó@Ë¡›¸ #%mu,^*ﬂ=2(ÖÙıÚOŒÀhüï°¸" «˙Â^KﬂÔ¯>xq=˚ﬂ„hd8cÄıA@≈ˆü∞v∏≠≠9\ÒÍ(m˝5;[Ärˆ€n!Õo3`‡’Ë1R◊œè´)Q3!Ö3”ªEUÑ‹Oa“CÎø)F»AÎC5Ï_@S 1ñ7çeDMÓ¶¬~)–U∂üHÉôH:ÆR¶7#¨¡gcÍÈõÔÂÖò∏ﬂƒ¢k•˘áVÒ¨0w>∂]V„sÍy…*ﬂﬁ√}1>A$ö@1Ãz∫…D']¶Î≥ík∑-hKßÁ¬I`ã–YM/]]
h⁄‰“Lπ=lÉ-ñ‚˜è›@ÒjºÒΩ˘8™«ÙÌÎ4qnwÁÈ#9À4…7”:ˇ‚¨Í#}ë÷%I⁄gà`Ø SÂ¥G"∞M≠ïX¶√ÉùFvgK^Ú≥êöÑ<rWR˙ˇhI“7∏÷Ωñ[™J6W¬¬=óœ÷€¶Ãô)∆ä3√Ûéí+#∏c!æ“€SSüºÚHÒ≤I73újqÃ˙>æ^´¢_∑êÂ˘b)"=ÉOøumµ*Ô
OŸ˛XX‚ç¢·u—πœyKvkÌ9ºÍÆ£•5–ÅGÛz˙jfRˇÓ¬sIôâ˝"ﬁ:Øª,ŸåÒø‰« ƒA!Bóû¡¯·K|»U6‡Ÿ°ú£G#òÀj¢ ëL$äÕ≠Ç24h˚≈⁄¡ûB3Øõ™”÷ÈÖ)4›ª>Õ':»Û÷˙"£4æ G≠flwé˙Ö©ü6 ¶˙*Èkµßsä∫pˇ˚+Ëfo˚`g°o5caùxdñgZ$ﬁ;ügDÍ¡H3cb”À∫ﬁ—˛∑Ñ ¸¿ ∏iñ—N6é%Ifàödå	ls;.;<ç!@¶*µAﬂâ¿–èÃ±w°Ô–6)?wÅâ∆2ÓIwPÙYAÃﬁ∞µÆV8§ı·-5æËñﬁ*Ì!áVò∂ñŸ†ïp$[Ç!b•m%îÑ© ËOZí	|•lﬁ≠˚íŸz˝‹}œÖª†$0…≈}kãÙqxp®®Tœf‰ªœı6á-BNõ(s¡·‚‚%∞Ü˘˘–∏G%p;9•hM‚((ÕF(®,ü≥Ö+|,BkDˇ œ7ÅÅÏ¨Í"a¬˚ûjIÓˇÔ4  ´§>{c¥äE¶˜R=ÅiŸÙb∏å‹Ö[ﬁ/»@∞P_ü}¢∞J{hÜi1x§xŒ¨
]CLıÛ∞±Æ≥V/l,éLeπôµ¨œH:ÿöœsüRæm-a1; ÀC∏∫+zÙΩ{íXCˆ_Æcﬁ©ôoÆº%Æ|ß´4mv˛ãûjo«•ì‹ÂúÏiH⁄öz≤ûï¨ïz∫`6òyqR/Å
<º¨Z∑ôƒê¶°3£gúTm∫≠√Ff™»œœ»l@(FxcQ—ªç±ná∞qÁù
I«ïˇÇÇº)ñf◊ßáGê‹ÅœáU± ŒG∏=ÈT)eò&Ë—ˇÖ≈mg[5Ñ∞ø'-∫7&yVÎ)Ÿ3qm∞>ñ˙ü ˚]“I∑Újé7·Ä7≤ÇîËtSÙ€øQíI—ØÂÓ"B∑EÜ‹ÑΩIÆ,,#Eá0œâ§ÃÚ,≤Y·ºO˝˝v≈=ƒµ”ÎÜ©†o˙K¸åB:k¨îŸ(wtæbìÄ*Äc
õ€∫èy@]Iû°Ó4#«KZ∂’ó±Õ—õIChÓÖEL]éÿÚnáf5kŒ˝√]«úiØdsb&Rˇ•ÚDÏÒ@â§Âù$tQ˝Ê´X◊£B¡	‚ºŒás<\?ô÷(ªc†U4Cÿmm&À˚ßªΩÚV{,5ÕÔEal∆qYùV”≤•ãÃÉÀ_Ôêÿ‹ª‘ÜïÒ«ı2J«_ùÎ$Ÿ_…wÿmD0U/õaè—ìIÕ
GÉÇ\¬EKb¥K´Øïÿ<nÓﬁ U4ÌFuM†…˘çß:èˇõÖ4˛4SRÁØÙÄ=Û?∏ERœ>∫¥—Áˆ√mô›0ˇFÑó+EZŸ•`MrπÖ+Ñé<(∫ú¢Ü±êQszˇÅkì\¿ÛëÈòŸ</~9*?i:˜-Úln\yIÏ‹yI°@Ø”‡π˘T&4=R(≈~a»”LñÚ≠ùA´](©#©¶“‹¯—˚jCP£2lX˘–…=tÌgí–˘Î·EÂEÀ0ΩL1L%≠‹ã…âu=£-ÌF1Uy“®;¶ÆŒ.¿´&?N	ÒˆÆÆ.«3êW~XÅînL:[e≈?ﬂÊY@ﬂ3R¨hÉø q}ù8êg oÚ.(Ê_\ ˆÂÇ_*öﬂhFú*†:©8í"À7&œj÷ûaD¢Üˆ)e05≠¬⁄˜¸?ØcIˇ1‹7në'vaN_' 
Íz+uÑ"=r∞Ótòï'ÅŒÒûW$Ü‚	¸·U∫Pq+ıœÄ’Í©W®SÄ¶U@≠/Ö«A˙Ú™d3¢ä¯ ZÈxÏ@ÀΩÒÿ{7b¨{•DKˇπÔ˝Æ]ñc\g8éaΩ#ÇÉ´üå;#√‡Ï⁄&{«â¡òN˘ëí\§]+Ä[a∂w”W˛Në!8W»Ù˝⁄Ïê{B;ÏFø[§5>º„5Áö¸àà-¨∆í ´¥ø’ÈÃïs\±[_w›;‘8≤iB"—óóçØbv’Aú?›√›µÇGÚ
ÿ†>J•îèÍ¨ŸµwxÌDöVd,÷ÛIò=∞±¥Úpä≠ƒóx%˛G”/X7U7••UÃ≈lz4Ø°"f√%ˇ%£jLèƒÄ◊ÕeUfπFRè5ãÂV—/5o7ø©@ÿEÿ÷±Çß)ú~ÎvÇRîÖ6JÈ
ˆo[-πbŒP;.®—Pˆt¶|<k¢	ÒLo≥R√\m>’‘Tƒ8ﬂﬁˆLì¶	‡—å¯_e—»ò[S∏™≥^Fs˚ìœ–◊1–1zFLk3ﬁÆ°Uôj8/YkÂÂ‡ÎìLQ˚J\¢]U#”ûS>çií°aIª†ı∑Ã_h¸!1Z…í@WjØ’"<√`E0”õ$∞ı9õgÑjÙr ¡O≠Êf≈ßtØ‘(HS¿ı∏n8à*ÛE®hu :¬°7©c⁄˙4Ä•H+÷á¡"∆°6p©
⁄e É¿v3ã¯iÅ6˜Ùí¢!çπŒÓ¢Á^ˆßeäN8ë>ŸJÓ¥~d©E4oë√≠ü®/.˙ÉÂ¸“)∞8ı˝D7’¸D`H∆˙GÁ´Æÿ'˘cÍS¶¸ÿ<–ÂË÷æoª≤5*ïÿ.JæR©hÛrP’%¨øπRk§|ÓhÉÎ{ÎÍõıÅ˙⁄,/¯I {L™câ/b∑gwI√ﬂµ\∂Eön8:ß∑vŸ0Óä?/Óõ¶ÄQld7¯‘ÑÖßvìõû£°O˚56'É9x1Bï.Ü–—/¶ÜeBHeÎiÃŒb+P‡óΩˇ<⁄ë$ﬁ¡˚_2xY)§µc¬ΩÍê›´3ÿ“ÑGYL%”YräUáÑ—e-ÃÛ'6•™']s«Ié_òı+:¯Õ•>œ¢7Ü˝÷‡ÀGD6˙=÷îwá{OAßΩÔÅ¸™{√“ôÄâs5ØﬂÖÿ‰$è3˚ßt&Ç“7ŸÅ
≈Bêˆ`Zú¡r>ÇEÔMQ›3«õêÒ÷Ù5céâñˇ&v?ƒ=`Ú>t—ÕFOÆgÏñÛπ?∏D∆ZørªUY‰È≠ zé!–%	-l´ sïé˜ﬁ1¶„
˚y‰óÚåÇ˛s•$L¢ß‡º·Ô–4⁄±˘∂Ë)ú˜£∏§Íœü÷c¸^‹˛”›Fwá√gØXhU©>ìº	*Q$Åã&®1lÜ  4”])€ónÂ|Kwˇ≈	+æ¬‘,‘/√\^1àı£Î?dÉ.∆˛2au*C§:ÎÛîKW€ÀcCeÁ¯Ç≈÷o_⁄Dlr>RêüqEçæñ>’•Fd0…ŸEã√óU≤uèà^íÆXKúóµÌ_-‹>…˘–s7lºÇ·ﬁëÓ9dìÀ3`Å\+q—ºørÄ0zÅı˙ íÌîLÊ$&–ÁRüæRscï¨ŒÙ««ˆèÌVxoéÜLÃ| †Ã†<->™’  Ã‚ÎÈÑÙ√ês¯
«Ù2ºˆhb°†æïƒƒ‰]öiR,h?aıLÌÛíãZVQÿ8√∏˚gJ…¢È&I„fÉÉΩY‡àû©“;4 î•åñV°
ÙêQÜ·¢î@°÷ΩËåÏ‰‹·Â0sŸ),dª91;}7µx^zœj«"v´ÓÉÌËi´L«ªcÒ∆ôf˛Âc1_ËÓK4øvO‘vß˛’•Ÿµ˚8s´4zzU·iË´vÅ¿‡é»¬Ú•Gœﬂ°˙≈6Õux4”Cå˚Hû"?\∆fï(I‘K¸ÙW`¬á‹i∑"+èC«xÔw◊√÷≤µ"‘¬∏˚|ˆÅ¸!∞ÃC˙Î·w'tC9sØÁ“ÏÇ‚S±§óß «'ÓÀ†„(õ˘Ù…tÇ+zÛ>Õ N`∑àî‰é3fÍ9uö‘=ûcóf_œ‘¯√;,d¯˙îuE<Á≠øx-2âÍßT‘Ü’W
úàB∂¶≠,Ën%S!:ji‡a ˆ`ÁôW¿”œ"GA^)u±Å°–0ÎkØá]u≠gbÉÒ∞ˇ LÍÃê‘:Z~Ë∏N˚[=ÈÍ‰O∞ò¥"≥Z0U≤	“.ˇX|æùπÊª&à8d©9b⁄ç¥ú¯»Jc|üáPL0éÿ4&V6á?f	◊π¸«ıuÃ¢˙Ø◊#à1é≤ÑD⁄¡ˆÃü´5±ê/XEˆbv¸ù¬BÎZòBLû¿∑·kp∂1àÏû∆‡´PT˛-’Û‡Éi∞kU|©Ä¡ãf«ıÊ2vÖˆÒÍÙ!√hÂ·ÿÜÈA
T6»‹R—∂éÍr·~h€u3HaÔú_ÃÀÍıÜwG{2.(M≠oêfvîÌÅ‹hMº%(h$1¢·ñ ç˛5VÈ¡d35JˇZ√<ﬁ\ªæj¸¶„óût¸©í	∂≠Ç°LÍ«KÃÕbXQ@˙Ñâ≈€ü∏\Ìu@ö∏Ö«¨˜BÙ‡˙!
Kïo˛Âg∫ƒBFèAgŸ\W0TzÖHõÛÅåù6Ák≥m‹\•—◊÷GÚ8˘ÑªÚónÏ7e˝ΩZr‡…k`«Ñ…°h«j¡ARù˝ª~ÂmÎˆ˝•åSC¸C—˝úâKd5™åòH≥’ô∆ãÕ˘éãË/˙adÊ˚≤0Nbé2››9ÑÖÆz´À7v[ö·{ÔÉœYx°’∆*G‚u£¯wo<ûy˙∆ç~◊{õCrîÙ„q—heÀ¸2˚{H$
3(oAEÜÌ}‹˘ø∏Sı/95≈|õY0.æ™e{Gr:O˜;O"Çô∫s“!âGc•í¨#;[ˇ÷PÜ%ˇâìÆâˇg<„õ|é‰û!≈ƒªßÕ¸ÚçÜÍÉ+»êVÜnΩZ"ï◊ˇr;=ıkAç*·2T¸ª–IqÌéÙÁŸæm≤æ¯∏Œj°D=Á1ëQz'LR8ûhZˆJ∞%èá>ÿÄ0oìˆÔaB'®G`°.?QîÖò¿©íH5¬J#‡∆à 5„£àS¬nÑU ‡¸VVòãà
M5÷UO§ˆÂ≤[Ü}ô;åÎ„Î†åZ!¬?≤–c-XÔéˇ!t ëÖOøH§#’t+Ó˜ëFHì≠nÊ«&Ù_ÿ∞&ﬁc$¢Ãv:ŒL+˝É>ÙÑ(Jo56›‚ˇ∆∆°∆l/Â‡ñ|`¨ÜLæKüq…˜≈ÊéÁòÿÄ®˚ÄÂ+vt5]1»jÿ>∑¬QnÌí˜#˘É—,K'£˜ùî_7ëá”RFåF•úXR√ÿ@Â÷öØ4©ö>©–ï“Cç\≤Zv©]¯≤P∂%ÔN9Ì˜ÔÑ–”Ä8«¬óU–JÏÆë‹’0¸ª§4µRïM¸`óÕGH¯˘4‘Aì£í“1jbOX(±!Ÿ)¶R¨Ç©''÷ŒÏ˘æ˙∫ÆbzKß-&T…€™~√A$8l:#PœÚo’å/⁄r‘€àé∫Ó:aü™M¬Nç!¶TV(ëCÀèÜ4∑E'·ÏÁ%ÒΩÒI¸-:+Ì·zïf “7gFª¬°<ÕJhO\÷ì… Ú%⁄B∏æ*	Oª∆ÍfÂ}oıbÛPÑ|Ê›–ÍÖE.†tÓzr∫‚†Óø–bG÷ V8£Ö∂hL<>»N˛‚µœ&®◊∏¨4f%õ‘ÎùÂÙ'`É≈∏ªˇc¶ ïYrË˛y)ò˛Õ+ê˘¿ñ1G´7£%8‡Nég‘´∞ÁÏÿ5HäE
á∑Õ¶∂\˙≈†dœ≈‘â¢ÔÃår¨VÈ√xUÙﬁ´Q£~ÙöEDôëXePœ`πk ÚA f‡∆Ô5î¨Ú1ªì,Ù>Õ£<®∫≠f6·£üˇzøJÎ…Fœr⁄‘KA1@n	õdÖƒu›™#d.€Wª2Ä"Nï*<»˙ï‹î–6Ù≠á∂Áﬁƒ©Yœ<ùôÀ˙2∂„j¯q‡=0ÅB2∂èÌÆ≤¡·UÏpu‚Q/@”ª∞8úÔ∫<v≈^Ìd&’v≤É~U±∂k•¿ÍW0–Ö2âΩ©éi#å!I?Ì√/ôãÇèT-1¡£àóm´≤¯ã6†õØˇ∆çqâﬂ∞cÈgÉ±Zÿrõ&j ‚®Ujó`,¡Í<m/£µìéÌ®sNãLÚ‰•ÿ◊ºEÃT•.l˛WˆfBºæﬂo~∂*v≈Lœﬂ0•%(¿†ÆZñ®Î3Q°úÀ4é√w>ΩÉ∏°NÎ)ç◊b‰_YJß™»í	lBz±ﬂÚ^ã⁄ÁÖ…‡Q_*|ÈÍ‡∫'eË∆Íı–}uıçt¬%¯±kL‹"˙:.mÎnıâ√ß≈m©0µˆôû´ÏªÅ„«˘c +≥ g™™;ø®@~Ÿ<¥fÖi¬ÕàË]#Trœd$7Ï⁄ìYΩí&Ü.Í®$·äpS√C ˝)ıæ∏¥ /Œµ¥˝‹%p0_Œ˜z·ÿÂtÃ§Âò˝g,ÒQπUÅ™2'j2Gzµy{∑ﬁ?(æ»kßRP“Óìˆ‹¨÷«W{lí¢xêS&|R€ì*_h Î¯cMÉq«Mfêzü≠5ﬂ|`ñÚ˜â˙‚◊UñlhHˇáùãxÄ2rd']cß—tH*µg≈~EˆîßQ[›®DC/vÃ(O_ëùﬂº†wkz6™˘tf;ç'∑Gq„-§få´Fn/É*p_&G·O¨D}™∆s(í–Òù¬WÄå™Äv◊Û^ìF‘;F	´Ë∞É˚ ¸òlŒËÃê‡t≠ÉºU«/W¬ò@Œ»õ é≠g*¨Â®ù8¯v‚ì'Œ(èÓ65»mÛ^t´°N.—©a«j⁄¥,±|ΩÄ≠-!nÚ√˘Znî?mÀTÈQÄ.Pﬂ$Ú&—V≈7˝=ÜQGo Ò1x›HâÆEÿ¯˙3óÿA‰ûi/⁄¯îE‚E~tB®«ºOçÙÛ∞«D(©µ≤D>$YˆÂ…ñıà£–f[¥≈à¸7K´˙FÃ¨™≤Z[sëœÂéŸ∑ê∂cNRp˘†œqùwm?‰œô'nmZ	t∏—°Ùæ/ëbõM'˝ö§‘-˜&Í9ã¥k∞«5â.†ãCâ≥¢\ÒTÍwÜ¶â]°Øx	
I d|5NHƒ™µ„n†≠‹’(JK(Ñ¶ßVïÖDå4Ã≈X¿V,±>ôÊñ¬(˝◊[ê•ﬁÌ©≈Ggﬂ
Âñnº8∞q/5ë„v=·}æw^Õd≥Œ’=ßÊ…òÿ◊eÃn¢€áG'mcEôÆd¢Ø˜5Zy)9“xa;A\v+ëÖÚRCRR·&9ãyjHãÌB\h:®aÛf™ëzÅÛ0›)Âes‰ˆg¡[óP£JZï0Í]j"πCS<—”‚€ÄîÊ3Õ÷Î(Œ},p√≤%£¢=π&˚xª!¡ﬁ£,zÚNdè‹⁄Ô≥ˇ®XèˇñvÄU”µ›v∏h Ÿ—¥‚ÁBú}^=*r¯@ﬁ|≤]zù!Eè√›¢Ó4+Uo˝≤‚‚H|cê±óÔU|+ÈøïÂrëJÖJ˛h1´ˇ´ç∆^R•6,ìò˝ò}í˝K¶›HvÌq,Ùe≈“l√MÄs¥$:~j¿kM˚&–SMè9xúOäÇÓ˝†p38êõIp?¸¯ÌZÜ™b«kâ»≤æ$÷ã°S!ŒJ€ ±ô€Ÿ¨ÆWj4ñKá'∆∫JëòÖdéQB»Ê;Y {Ne”ªãà"Y`7À‚€Mâû©†r0S6∏)hÆ3U=ïBù¥‚]bõGÍÊ4{LdÔ€ŒÌí*•ôF´vÙ _Q∆P¶ÆÛÃ∑¿úâ“Iıú.g¶PAÿ∞^Ÿk"äﬁöÎ Ã¶î<Ò\ÙB^<ûZ¡Â9ı6	I˘äÂKŸ¯ÆQ¸⁄lõ˚£öµﬂ± ≈˜^±E	I9íUh ßHÑGR˙ñ-q˘…Îïpÿïk«¶π€V	 œèÒP®WiØEÌ·2Û}YJ¸€N“Éñócè™ëgL®:ÀãnÜ¿N:∫ª≥ø·å¨˜°SÌå8“R3»˙ıF¶Ù‰r9ùÂOoÛ)¯ë§˛≠†Uå=∂Ùxgù%y∫´≤⁄˛fÜÜΩO&·≤ò-{ªXC˘'+s"ö˛vëõç(‡ıÖ8≥$n§~#QâhÓ≈Âõ J_¸c^/IFèõ¿HiΩâ∫yz≥¸9öÀy	¥H_DÙ”∂ß†?ﬂÂﬁﬂnf≠â.íÑV›kWnDÚ‰«.EvhèNø2ÌÓgªπî`µÂ>€R…‡˘Ö#a¥“À¿Ú4&
©ÌzŸôKá8PH;˚üä¯Q–ÄuSØn‰
yóπkN}æ:ìoNîêî@˙ää⁄Ï’^AôìËv¿·èÅêô7ÿD™Ûqò’ﬁiSVqLb0u≥D0·2[	∫<∑q…üÊ,á√\≥#Ö\0çO:ﬂè‡öúx!Ú•+y\~”ôıDísÓá∂ú‚z/∫INÒYÙ@6%"2∏N¨.πü≠Y◊GÖ˛NF∏a9ÿ6|w<ì£[†§±_	
É[-‚Eúˆªåc,Ïﬂ8ˆ%¥…§yw•≥nU¯o!]≈_¨ÁiíËï:U¢fAJàY…r≈y3JR›fêÆ„›+Ÿd$ãr·˛$ÚÑ¯Va¿ ?ËjıÅU £?Õ0ùàDÁcdÈÍÉc3≠9W…
Â^q ∑77&Tc∫˙á_2~+W!™±jÈKè‘:‘Ösœ¯∂¢≈ﬂ45mbﬂl¥‚4&hZÆx§|å§éCuyGåÜ<Æ´`TN <'\'ﬂ‰!Û"9≤YˇgJX&,;Ω`€æ¯áÑ 1öﬂ¡¥(#‚◊AhsÄ›æ&˙eY’4˝®vÏ2$£Ò˙∏È\pv,Îûç^∆Ù∆y	üâ/uèÕ{õ˙ÌÇ¡ŒUﬂ%ª´"N«Àª≥∆ßÃ	vllﬁ’œò0H§.8‰+v›⁄SÇ ˘ßπ0–'¢Ã¢8A'¥‚Ív¢∑je·ªÆ–‘ó|£¶®ìﬁò_N1£ãsIÂ¬v»àn€u‡≤LE¢Ã->˙pXµëH	ÑH(ƒEu`6œ∞√Ö¶|ÚÊ—Á"ø†T	ÃD.–5|Ã≤©(Û„Äg+ÎÛ¶utßl¯v
˛ÅP˝5Ñzê∞ìÃπé≥öö3˝5dÎ†Ò“®=8ä√æó˙™lJMSJÃŸA6»? pôxV#zaı“{ÔÚP∂n-Ëá¢ò<’≥[ã≈‡±g¨jÁçïUÛ˘¨$¡· gØ√nX∏°âåslïﬂƒƒY‚í)DªûÒ"[œÏFG•I¿◊{z∑T◊†Õ◊∑˘ﬂUOyzr∏!’êEàn-9]/…OwŒÔzI>ÌDFô:≥qm¿B˚ t(Ω	Ó˘„~€…jóg %KíiXN≈∞Ç NZ»»…I√≠kìªv÷YsIªí)?>˝I¬]‰ïg\¨·Ul`
√äæëÿü¥“@_( ıoô¬ÏÀ-©$˘PNT3éø;ôÍãk*K´e-Œ≥K‰«>Z7“ v≤Åƒ®mÅú+˛…ñz±ıìW›‡Ä€®˛]ÆQ0WFj/ öëŸ?ŒPD◊2UCo0f q≤NıPàÔÄa√◊_‹ƒ%∫MT
ó(V’Qcª∫(õÏŒ«Ïp1‡VÖ\Õ÷õıh“∞koˆÆnâÂÂﬂwé—ù1}!“_…¡<[5sôÄ¯*Ûâ˙àﬁ.|Ë«Y8ß‚’Ç†Æ∫Uò	‰ë†‡¶Q„WäÒn"ÏBÇ°ÁÀlZrÑö§ë∫:Ó^—Æ˘”%E‘ìê≈hü∑;vEõ\à¶sZ€uŸ˜‘H<›@·1R.ÂíT dJRf_M(œÈ…OUf§±7÷7 SfÑ<`(1p°òˇ(ŸÁø¶‰cﬂXÎ©Tô«§¬cd‹âÚö<]Ø∏#0Ú¬˘‰≤b≈≥"Ö"»·¡˚y’ª)m˛´ïhIl*pÔΩs«”ÅõïÆ∑êô5cjóDOÁòk7Å)˜˛w[À∏ƒCÔ+‰ò~‹ùf&%AúSX
˝®∂”=’√µßYE≠Àa/±µ°NKì˛¶ËõØ5ñVvüµ∏¿s£H3∞„xû(≤*‘P+›“f&¢KhP{–8π≠¸†wïÊQÖ–8#ñ≠±FHÏ¨;¨íˇ&KRì∑$Œ+D~á|˘°®`PIëc6≤ºvp˛jóBv&˘ûÑ_°¬3Uïßaœ~Ôß>J∑ˆﬁÄFä˝$∫5‚xÚäaYˆ»Ω}g‰í	ª¿gû[üe<|kQ>éƒπö™õJ¨?ØË"’e$È∏˙¥Gbc5≤MrQÜPÉóë?~—U√ê§’r:K[qá®ŒA(K¨s2]ßÕÇ¿˘JœËÁ+⁄pJZÁaåÎ¸ôè‡ÔÜñ-5Q3UANkÒóπZäí!T˝[OK∞Û†n4v6˙ò/™»CÍ∏ıA‚/{í Â~ÖvV°70!*@Ãπrå∏à‚z≈j?/˜Ê:öÃO“Ëä≈.™≈Ÿò«÷Ì®œGŸ5éc6êEæ£2Qïîƒ§-+¿+t^Pdì÷≥3/#√c¢©Ü{øÿbçµ°7{_ìÍ◊$‚õE´ó(¥(·«¥JP¨&&\%gõ7(∫ŸBÛ·ı(ÀuR^~˛HÈˆ”£ˆ¬1£F©8]w“…â3©ÍFÃ∞«Ñ õ˝7-xh’5÷Á˙ÛoaFåÿñV‰b0‰=”±∫ë⁄Äã¬NZ¬
≠6«÷íáù∫´«ô˛/$∆´W¢:èú¥wˆrí7Œ≠Öà§∫’KÜ€%‹∂ﬁ9˜πr€ÖæË?«H4{‡ AyTlf]U`«œ€¿CX±^Ñê«Ø‚≥€⁄œ≥¯‰Éz\±àf ãß[œE#∆ˇòÔπΩÈÑD3ÓÍ$∏_˝j czËè4Èã´√˘ª—ˆ`é˛2∆äÛE‘w[9`ËÁdâÙ/å€UFÜøÛﬁ_âıËug9n¢˘•éWÃxÅ˙5ài∏≥v˙‹ÛÛÆ¿ÅÜ–˘°ár’
iEΩßUÏ:∏îÕ@MÔ	Ä∂zô ≤UêI≤/uµ gŒ©˜zUqI®ÁdjEf∏(1K¿÷®r–Ï+r¸xRH»∞˚4Ç-nÂò˙‹öq»÷íxl¡7ü„≠Àò7¸ZÍ('ÉÀ…Ks#•ÏÊ™˜Ÿ5;+'√Góä◊Œl¯"£:"•ºÖz·ï≈_6⁄K ˘≥X\±¡‘PÉ	Aã¬‹¡‡e™úó˛Ô*5\kÖàœ>• +äa~Ã,;-√xX&I s~;ŸÒ{t‹_ˆílájÜGÄ.≤≥∆›O„˘óa"çab.ÃiÀQ».†ﬁî]=MYFñï™ÃÜäßjBi@B∏ó)–Ωe*`ÑˇÜpâz=^Æ$1H5è:µv6>U†ÁÇ)û}%á‰Ω≥ 4;d⁄Ÿò˜hp#‘Méƒ(#:9´⁄N≈≠qÃÊaË¶OKHÔß≤HEpi®úç+Ω¥Úl—¬ÓÛØ,*ùïP…9âd +Ë-ÀÑ25V,∂'ÂkïrÔZ>vË¸¨OÍÍZêØ[{'Œ¡_jÇlMujv⁄qNOÍ2⁄M≠	v[H˝
<√<µW©≥^©5>éGg4ø2 *[™Ë« ‹:îõh´U„}÷≥)o! IyJî 6&è∏zPvøıºéû@9…¯Èî≈Ò Ûﬂ	ªàZËîº∆…gn!.óŒ¬õª_¨|¨H5Òà¬≥á¡€Öôï^?∑˜ôÁﬁß%∂Ò%ƒ±u~,∏xƒ	ﬂmõµˆΩ˛ö|.õıÜ?b@“˚„öÉëi6%6"bB¢ìÍ¸btpñ,‡M/∂ócQ„Ÿêø¢Î˜ù!$∑Õí2=ú'K´8!s≈iôZ¡≈	bd“±+˙™:Å–„K´q'ÆÈî¯7”õb√ÅÉãLfveóFh»vÙÿ2´_ˇ1;ÿsÔ†/ÇoK•£*l~≤§e¥œÛóª8áû
áQá‚:Ÿ˝ñ+ùËÂ:ew(YıÊCÑÊâN
•˛3K∫¨ìVEÈDró˚F`»ç>5'~Õ◊2›NπΩ-zÂ˘∑ÍtŸ<¡nWT]53Ì!û≈~<‡L÷	E^¯*
Õ}@HÇÀS∆Û¢≠¨≈µòÅÑ2T˝©wJáTqöﬁÛêÚç¢¸â(B¢Ê£^ç/}¿°Fø#CaÏÂJÉ≥ºDú˝"πh	øØyÿYoÅ)°Ö…éÍcÿ&≠¿©ÃOOR
gÄíwﬂæã„Aéß÷Q}ÌW	ΩÄ”Yp… @xˆ’û“Sn·O.YPﬂt)Ûƒ£ªh©–Ùƒ∫Ùp¢Œ—4,3ÓEïó4ˆPt\… sº¶™ƒm#èÍ8Q‡ƒ9¯65ñ¢ôÑ¯89hTÓ‰ïœKXføcΩW æ≥[·tZπzÚΩ§DP)∞˘Ì6yöÊvÍ–µ˙û…?ËkêôsÎ≤Æ–üz#>Ûò:‡Áeß~á`rçD∂6ÀãÜù∂%oYiì¥ÌIß¿ÒX§G#en@Cìâ≥Q4Ù˚Õ˙ÍdÏ >ˆÀuwKá[o]'«=^÷¿1≥	M/∫nÆJ^˝a˚e^∆† õ±ˆR¶\â;⁄ƒ‹7Í-Y`U+çeÏ¬ '≠3æ∫ŒB˜WŒé≠)ÃÕg”O·«—ØxñWõ·0V.√JW~@ú‡>2"ç	YX‹.∞Ò∂¶ óÈTjp…ÃZ∆ƒÚ2∞Hcêî÷NÇ¬·‹ôüï	p˚ï	˙œ-ÚµühèT_˙ü-≤¡àS’¢ˇƒq#õº∑˝™µK‚Œp'4:ÉÊmãm˙æw®ŒU^·ú®™Î˜ã:Ï•⁄DˆºOÀÁs&+O≈Í∫ÕÂàYü¡Î
∫$('—;
Nu”"U˜<ê7“ÍÇb]i˚¸–o˙¨tπUÑ.P§÷˘ﬂ·Æ{PO6ﬁlØûµp/7 Õ(¯Óèr3¥˙º†QUﬁô\é4IˇÙòõ—Rˇ–$ﬁßÔrëÒ;î—szäc	£∞\|ÕÉ‹ßõ$–í≤˝÷$¥∞ﬂQ‰¡VéyÜ!æÆPÌÚ	ú,ññ„,ÉI˘‹O†H∂	ıèt*xYmbá¢ô˙ÃA“h{¡÷¬¿Üã23åí;z€Ëz#AÖ÷í6¸¸ÉÍÃŸÉÉt{ˆÈ™◊éÕ/Ωﬂ◊œ»
tÂ£ËÁM˜8QàhAÃ2pOÏÁ∞±ò/´®kr%‘,jnü]ÂJ©Líß’ﬂ[0õeu:·J‹’8GY',yıÉ7Œ⁄}•lòk˛b•i’ç6˚%¢Q?≤⁄‰—	˛ÿfõ§›ehê4
ø'vV/q=%ØWÀ•Jl†ë!U~	Ôÿf&6≠ªÿ≤ÅZ’siê3‚Öù•ŒEΩ‘¶˜ ‚RASù/*
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
	u6