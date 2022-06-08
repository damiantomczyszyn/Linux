e/config/ACPI_APEI_GHES) \
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
  include/media/i2c/uda1342.h \

drivers/media/i2c/uda1342.o: $(deps_drivers/media/i2c/uda1342.o)

$(deps_drivers/media/i2c/uda1342.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ude/asm-generic/ioctl.h \
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
  arch/x86/includeJ ¬aèŒ|Q¥éG1D„¬)öñUŸA66èï’þvy–7û¥¡ärU¯(fñ¶«ä—Û6±žNmÿ)Úã$4œX°åÏCìœYÚ+ŠÖ…hhúè°µÏ Š@»€^´‹¯7ö÷-·êOg×ŠP½VIçe	¡;º%[N=r\(ÎÒU`àïÁ¦ø}¤0¾ÂNmfÓ2À%®À:þÀd_lÀ&˜Ý§Ôv*^æó0&?ði—†nA-ÑR`ÌnËðçà};S"ÞÆ—º:èS­à*‹
·‹¦îu¹µ¨»Äk"Xä?ÓúmžtÙå¥Œ{PHÌˆiÅŽ¸G«lý˜Ï¨ÄRÙ4Qª…®Jq½`5ªO€~dû“®¤Å>dÙ?s&6Ê~K 0áï’˜¿ƒ¢'xI¡nf'Œú–­Éðº9}ð‹¶çCò¤Í!£å¾²öÆEve]Äq¾àLGeméa8f P«S&;ÄM•1À$-öl l t˜H¦wAÐ4¥9…#•Ù•µîŸTÜöÔîb÷D¨ŸZ‚GaŠ0e5­ý1"–ˆý5_]¥ß§®é—,^iÍ\ÀÈ¼¾Fý˜žNÙbÇ`fÌÚô¼¿4Ú‰Q=2ÃYêáÅ%AaÓñ†>³½)v]Vd^Í)¿€8¾)){˜ùÁÜìíœ, b½"wØGfãñ:Bµ9ÊÉ…e‚3§Néñ¸‡´¨J“­NCöðËHÓy|M£-ÝáŸÝiÍ<Ù !±\òÙÉž×ÝÃª*UpcÄ‚
TÇ•Ìýg(Í`Ø2I9ýHe½´@NŒ]ûÍæ\Hjz×à«µ3œ”:	†ÐãúÐ€^ÕJÎ+Myh Çž:Þ{ÕnºoºŠgö1Llvr;›ýõÈþ¬-¡·]Œ‘¯2ÝÎº§Âã©ÏÖ/.æ´IÂþ-ÊRá!‚‘¢1ð³•ìŠÁ›é;å–çŠ›³™#­Ùh×{æ–Vçøæ?l7IpÁB®{8†P +X³Lõõ‡J8ªTîœÜˆÝ63ç–ÇÓã3¸”+¸æ»ñÛÂµ¡œšÓ:=:žcá;-Ét»¯Ÿj]$Iº© $Éâ!OËOB‚¡vòs-¡ªa¢©Î@t^’™	-æZ%èTÈü·ttˆºP^˜Õx¶?B¨/{ÕÛcê8Åfý’jmÞ˜eˆ#²&ì‰´sO£Î$Ó–—ïv/9<áµ’gª€æmâ¦ÐŠ÷Ê4,9ís4•„?V	Èçv˜ij°#ˆºéf;*LPUv‘n§ïe}í¥öâD—ŸD.8å!vÓ=ÅZ_8ˆ2õÉ<	Z*cyÜTÈä÷8àÑ¬ãtEŽ^™k ÿ­€+ä´ñ_h'²Ònkæ…U²£nm˜ôÄ©ÛWÍ¥¤zÒê™íåŽc‰²ºúÏl¨ÞbóhJ=V#ÀŠõ3Ñp’Œ÷^¸$\î‰hpUÒIW$.=Úé0—ˆ™ø^ñRÖK&ÃbúýÂ¯^ÇZ0˜\@ûÒ±œþÇ¼†Ë*5œž-@Üð×œì7š¬~¥÷ÌG1¤Ð*Îð§ŽÿjBý‡Ý_X¯˜‘ô`L{TÃŒ^áL¸}E™mÕL*òB¿Úï“ûÌ^áòž½sPÿ.mÆå„x³Rtf.ƒê„ÀŸî_D¦	&rŽ>íÿ²š:rñ\×­®í2[&†RB“qå´;t4æÚKZÑ”pø^©ulf^LCÓ~þUž ?k $>X‹\˜G£ “K©e§ÎzþLB=.B×`zhƒ^¢¸½k¨?â7YmùPBeOÈ”˜—z>UŠZÜ²$úˆo~cTG/·Ü½ç<v8›—÷DbÿãÄC	ûÄdùg¼í…Ú*§½«Àˆê¥»±W>jcSìÊÄ„XhmE†åïÙ®ìóÛd¤ òw>B[ß%…!m¢Ð‘²œIk‡±ÞèÆF¨þL”_Ê¨û§¬³KîçyîH´W¤;àBË—“Ô]Oœ‘€’’RV1)Ü":TÙÃáÿïRšKô[gûH*ì¼Ñm¥¼ÂÅëW‡PÑWpÍñ¨o‡¿[n20"&™ÝYöž²õ˜°¤FHƒÜÏ:b°Û¼ùÔ<´1Qøçñ(|Ål.oN»Í5…¬§Š€ýEëà—ÊêÀDA‹ä3‹¤È$v_BøL½B%D—Q‘v`ÇJ@†FÆ¥ÂåŽà¬£Š T“äg=&çàyÑŸC¡_‡t:tëF×Ÿ“xXÎÖ÷QØ=²)!·LËJS £ÿ^8rÍ˜Cp›Úô ß¾RtyhL`fíh¥±öžp+"™—Ÿw-Kã@D9l•+
Î£ÔœÅ£H²eôø½¹üºž3(I}@e[OØz‡MŸTsÁjÎá»ßKÕ¤–ßa5n=¡<=iöòƒDXòRy%®²ý¹·ÏÝåiló°tÂ„äqR0Çø2S™†$®s~#§Ö'”þ?¥™sî5ä·}Cp^R£y²„O¾c-3#°¤â÷Ñ…yXzmqØdÎTX·UÃXb$VSq¹Ô ¨gk_LOÀ­Ô¶—ùX³i!oGÂíuBªX†¨÷Ú¨5’f’uõê…M¥“ÀT[f´ÐmPu2½•'êÎf@L|E³‚°d¨ÃX€i—wU‹wÆ}9*Y'ùU]Ñ¦ýÌ‰Þv>nÏTÆá^ŠKJÃ¿ø°üH I¹úb×ZÇ<(¸NRñA]Ì¸S,År)ŸÅ³PêTš'Ç†à½³Ü?‹äÕî.ˆ°QQV–dKò©k…b€Ýšaåµœã—5Ë†DýSãˆ±—Y…Các.ó°Û€Bƒ&u*Ñÿ& ¿%Z3§S(Á\ÃÜ±`,ÎZÛÆïÚÍ£êTg#Úb¯á¥Ö=
›#ÑÄstÏº‡fv$Ägj³TNm9²7¬n¡48'wrø |Š=yº_ÇÍtº2úYrË8(Ÿ o[ÇIC/ pVøú¡_î¥Ê/ßjLqwð›àGüÄ
Š6XnèäžÃ°o´ˆhÒŠìÙÂ©n(/H6ÃïÄf	ŸF­_nV€ ”/x‡~
úsàa,IÓqùÛšõôæµoüÒ)ÓECo6UÝÒ=©ÈKŒ—*F~q­Âhúk™€U¥°ØÉ€SýUz{8±‚êšc>Xfvíb—gßVˆ*¼@Ïu–åM´Væ¦õŸw^‰Q&²'Ð»Ì©Uhd	"á‘Õe˜3v£# ¥Ìop_Ó wG#§Îi”·ööù HŽ‰Á†hà)<Bì®F‹ Ü¶Y{¤{o}ÄðêöÏØoôˆ(„†ø$ŒõüÓ1Z¨nX§–ÄÆ=Vß8˜€Ú¼]¿ý$»B|ìT&«èÇk}¹ÿO\ç0Ð&|­0¾»—c)ÈPÈoJ•î¥0P‡|ø•ÕXC S«i…H¾¥Wàæ¦ð$¦óôIŽû­·°+Ä‚sb–¡nÈóøí?ÏZžý°*·U+í…°y§þ½ñÓŽ¬™÷»¿DK®Å=§Ubïµ=,ýôO)¶Þ›
l±µ@­¡~³ê‰ó¯K¤¬9NÌÖçnHÞnËñ"ú„Çà›ÌƒO âx/„*ƒ/÷¡iûO‘þ£…¼®Zâ·ÊèdîÛnªQ’G˜Z–›=¢a÷†‚sÕR½¯Š®öâ p´ù$5n«û YŸ¯u›6ÉÖ©ïª"ûËÜ„Ñ“ª]ã]—½±¤×Ïÿ…ªq"bvZúÌ°G[±)W9¬ˆa€×i:Æz%ñ¯È•Z«åL`çe–¥;W>¯² lj¬¹” y5£O²¼XÔH÷Û<Ó2Œ¶ gsþÊ:xÕ„H‚ÞÄÐ]“8}¥‡pØ/¹¾{˜œæUîÇå¾tñ«”&+KY³…A©M•{ú¯ú&˜½aØJmhàê>ô¾‘¥Œ0oKÉl)½<ùÐì­ü¿\R£@ÐZŸ†9Ÿm@=ÛO}ŸqÃéqà(þ_X¥2î§*[wÄ´žÞI×äÍwäÁX ªtt‚í\¾ATDÁñnŠ2{«ïÐâ²ÎRH!ëM˜!þ2Ô3“”Óƒ9†ð§í gé-JlÏ´Œó€wÔâk³×¼-Ó x ìðle„…#®¡`ž‹
q‰[‹ƒÝÓÇ°€gesæâ±“JîÍ­Ãn9±™E!¼æ\i¼©$(øÉNúlÁGÔNI‡ŸúWß^#ÈôÖf}Â Qzë€„ÇüYrtÏmæF9ðtÞ‚—ù–WéIƒöùièKj?!µø)Ho×k!—0Ãœ«æA˜E®Z|Ï§Ìg¯?sfQ±ì—mâsOmÉ	/ÃÄ’)fËuqqEŽƒq:XÁ'ÇTŠWoÅ×Ù[úÂØÌ|Ü}¨½Ç
’AŒMÆ¼’Î–3k¦Ü†Èûò'
{“H9}c+ô*#Ã¹_ß3Œœ„5à­¬u~¸1‡è•–Ø{ü0NÒ¶¯ýa¯ï?‹ã=UœxH™½`åà:ÂÜñ­ÁHäÖÍüwË­õ#.½ÎC(¿ôo–1»W´€ì£”iÓ¦Cªnªjiš·èÃ*¦#×ýñÂ´êõHý©rYÔ$tíÀ/l½²¬Dö1Pp@£Êsf
Ô%íl[*üâæ€XFUGUˆsP+Óçe/#é³m23@vt›î´¼ ØôÙÂÄþÙ:aHÛÎÞÈ¨µt’>=¾ ®Ã·Ç]o‡†÷û]ño—(Ã/"99³iãœ5©$¹CÌ‘Ü(ç¼±ŸÌpßPxÏÑâ÷ÊFM-‚áWPŒj‡K¯“àó)ÂUé‹eOz­£ÂŸ·€·v¼M³Y®uŒdÜh5ÅG²ÊUëááÿJžúä()–ú…ØÓpPy$+F‹IWSß¦ÑÐ]s^€ë>>žŸÞé FíVCë•rm,IºabÝôúb&™/"Ë¯U|gµë"³ÝÆ£Bò‡c%Ó
Q›²¾„]åŽÞ[QúÝ	ìÔÝ@üR“€ˆ$«'ÀD³¹ÙIÎ¼ HÎ«{÷áø7Ð±É<AÉ½: Ï¹Ø6Äõäàf6{è°­­Ÿ4¼c±‰'¸‘{Rßm’DhöÕ•I|vÎªé]	?™ª‹Sµ§mìqÁh¿¿E|þ½â÷é.UäîÆ”±/r&_,
Á•…êÉßwÃ¾D-å†è6k<e™HüÂ#rx^ný“÷š³Ý£eÉ•X­V‚%JÆ‰áÇæ%ÃtÚ6[ŠaÅ…úP3×Šæãç6t¬Íî$Ê^ÞÍ>kÖE<ÒØi6¢7ãl—ŽO9žGGëí ^å¿¦~ô<ÊX «èj¼ç•ªäl?…öQ³þ\?BõßI8vJ–gTMmSZz@ôc™Ÿ}r„r´)6‚¾œI#_UÜI«Œ*˜6ë¯ú›
'Ÿ½Ÿ\ÍQ‹Õ+<í•øhŽ=y$ëÌ?~âG}af¾ìm 2ƒU[tU1W#cƒf7*Îª‡³¹†ã¿ë¾äélA³\6?x%mòB¾U\Š‡ž1ÿé'—¢ð«JºQÔzûG%yŒ÷¥çÝ|ž‰7ø¸É9£¯â Ô=çDÕ¦±†òƒ¤˜½oœá2¿°(ñ\ÙÒ·g. µj›Ò;Öð´_~¶7<O†f]ÿÑ°l—jG×ýpYìV~BždÒ*›×<ªÄã$–ÕÀw`âð}ÍwË–öÂiV½£IÇYâò±ž~Fm‘$8š­!ï‹°»K°„VGù2ÝN^¥=Š	çÄ9Õë4Î7»§K°Æ™žÜY`ÙŠÞ ¦<ªDÇâÐ2KèI‹Ï
z³#ûöoÛ‹†ÄÃ´{fè¹y½öR¶ÝÿF¬æ°qRóI§i'³ÁîdÁ*1èh·}ÚCõ`zÚ(Ta²wP+@<ìÁ>ÐKá0žÄ“M‹±r1£cü;l’©Èß,óUYèD<ð@RÕWtýBhsDjË92Ê~”•!sÅÿèÏ&tùä œ˜Ž.¸0çvkVŒibG têüá´oÝ‡+³Z/ Â,«Pó]Ä ¬åÕr­[{dY?Â)*ö©œç¬3Sˆánš°ø&yb(Ôò§ám¨Åe¯áû?³i_ü˜Û$õë´2€®;è»©÷d–@äÏŸ©äi3·b[1T?ØÂèTÈ„ìõgP´_$FG…pÝºaß`"¤õè•¦îÂQÀ!‚ùÌ€vÎ(i1Ÿ
;¹‡‡0ÊÅk½´P*ñ½§Ny‰È4‘D€.žÛ½
SýxÔ®ß[&„¤sHÝM"ÆhÊÌm Þ°f×%sYÙN¨Îõ6ÿ9Šýý¨èhnA¥¿q&.¦ÿ_|)þy<b+ªT4Ä&n€–è?n ±R¤ß@‚U¸A3žì­B%´xk/7€'/í» H¥& ÂJ?aãXÑËL»G¥ÿ‚ïÏO‰Á´T´=7µ‹ñËo»Êß«„N¶ØƒsÕrÏ)?ô³^82º«d6vt@Þ•¶D!’zyn¶Æ-GÏGŠÅ&å"U	¾,at™0¾â¡“wƒ§Œ)ñaßFµYÞÞËXuÈÿÂfqZviý±IrÖæ³ÓÜ¤Ö"å*½ƒ›çÔô
úš£lŠ}ê¶ñ.šÇnÒÔ£A…·%®ä@±ä½°¤^ñ0ðš¿!ÅÉf™þñòÛLw‡ÔsÇ;Tþñ>\ëDpV’+Ý¬Oï}vìj´Ú´ñ¡›Àr"x²Ûc\í^–ÆÒ|Õþ‹Ú)XÍ1íïã»áî¼I…æVŸ½F“ÞÝ„!í|ºÈœ¸¿u¶°IÇIæ,õV‰ošFÛ²ói~ÿXº{×P€¶/J×ò,–:ð0Ìs@ykk²²K|‡°÷C¢éÛßDvô´U¨ —„TRþw)
’ ¶kÞ/HÈ9²!Å1¼._KN¬“‹b2À`Ø_)8`Ó)YIêvh8ãÂ$ÿ4ßæA,–Q‰«ô [ ;×ƒ	Nyá¤Ñ“e³|0XyÙnÂª )œ úqy¿X™gªqÀÆÀòvwËØä´N¬h1ÿ„øø€»~jhø¥gßâ¥³nüØ²Û0Çlé%‹¦•²óß(ïuàÎ6^jç¯Šºt2´“C™£ÙšJm.®5ì®Ì.ëH÷Ø±F­ôkÝ~´1`ŒXNúmÈt35Žz^ù™SM$…D~|þ‹?ÂäGq¨bi{1B<½ãlo&Ü§#˜­øó@c8`_eõß’vocØÌË|‚ K{Ã/ˆ3:Ôÿ1'U&é<È-‹te„ôß]9Šå$+Ãfó›}±ˆüÏg\WZx¿¼Zªï¡-rr«Ø­bÃÀ®½ceeˆÂ.PuÙa‚Ì,§6l«’®JûV?<ÆMJô><"8#¸_l¿O«¡Aá™ÀzÅÏw(ÐZ«¨Ýç!"KAi/àu{i"$Íeê2¿07«õCÎ‡|t~ÏÆTLoU–ÔÉTÛ”ÝÅJÔ¾ºÞ‚ ¾ÿ”Y-ƒë^]f›U/Ý5x(V‹Ì'¼”Ì÷Œë<KØKc/2ùqWÄËÉ‰^ÎÛ˜:='Ò(De[xòˆBÙž?ý{Ì2áŸœ¨ÃZ·í*<¸ PªîVö˜…5ë5lÚ(gøãU|«¹Ï™Bök…aäe€»V+}•O1.ÄœƒÞÝÍ%±ÉP„¯a_„WŒÑmßÊW*gœÌþf»&7íÎy9‹qÝ×Ð—Éû¼¯¦¼ÝK»d?Êèë´¬ÒœÏÅ9¯Ú)»g<ÚƒÊ¼}~óï´Ç›»7Q9=‡ŒSÕ{ñYº«:7á¬#Z‘˜fj*VáµÂ¶ck+§_G–#&²ój„ýlÆY$Þ|~úÊXï§uì:±»Ú€ñµOÃKislA ™gÍ­½(	˜t×ÂAçWç‹ù³ÇXÇ¡WkÃÇ&&æwÕôˆÕ¿{+”êÊÄ½IÌCÆ»'˜ñCÍï›BN!-41¤_ULÄö&7t/¶ù½1oH—aËÿ©Yùhb*CZê|ÕüèËZïï¦
xÍ×/v‡ÌEé½r*gbn”Äˆc)»iiñ)5z o32jßúÚ·€Nb‹ùmÞs)K>1M	÷žüLÊäw¤“íÇÆGÜ‘º9Ùú}7üUOj]‹ ¯‰FpHþw[:£o„ðÖ‚+¢ã‘j6\°?èj^ÚqŽÝY¹ª³eôüuyÄm.0ŸÛ´i«kl0ÍÆ±ä{“%–Î4Œß_füOÔ|ËÅSý-J 5á•lÍ‡èª–»SâÔñRŽ½LYÝÿÜþº[[Ï«-(Ï¸ËrDßn"GSÃ¾Sv–è?c[½^Ðu_L/~‰¯+Ú …“tÔB“4†1Kž"‰Ðp”¿õ0û'õÖö|¡{Ê—Üî5s—ª,¡YÂN0Šòýx}¨tlèJñê<åÄ\W›™y¶
5¡ý`eÏ ™8Ù®s(§µæ|ËÆŽŒí&Ál»ÌøWÆ®T—·u˜>¯º bƒÎÙœÎ-èT§ß§ë†Û…že9Ž»
çG§£ï>SwSj¤îÚ/nÊ•Ý@íýph‡M¿Ú†v ÔÊdþÏû ŸB‹ß5»^¯F#¾ÒÜi,='…dÆ¯yAå;ÞÕEžº×pÂŽ1ªÎh	u<‘-à« 3_wxÐL.•¨Æ’×VË+}ÑÍ­7ÏaÕ`Ý,«=pH@}urU;Ú¡«JÕÇQh}Š„½Ý@¡/è3>ªvÎ`ßsÛ‘©˜Ù#Âõ¬å{~ñe~ÐOÃ(¦Îî…Rã´Èòj’WNRq‡ÙÚÖ¿,¨(Âüêòîpˆ.Q´àiì†RÊ¶6Ç%ƒ¬RtÑÏí=Z,¿a}i? 3xÏØRvR-Qe¢õeL‰ÊlfÂŠ—ù3Š]3
„Œ£ ~÷ßáŽz_Îk~“J±ùg0‘-=‚$·«b‡Vzõ¾`˜ÍApRz—¢|&fOC+Šv§ªÐ‚”âp­¨‘L$u_Û ÂÊúoZ¿N¬uN—)ÞuÄ7±~Dy_«Åq
0’šåŽUÙú.¿™“R}2[ÎImú<‹óŠPvæÈŸÏj^<PÙê~°†Tåi»®‘d†h ÀŒ¤«moZUw7ƒÓ“ž×6SÂÅ¬û4EWe‚¹.|q¸à‘ ^ÑºMù,šP³ÜSå¾$>*ScZ%é:¢ÎQçþƒ˜^d§ž×*{ •çãüÞf&@¥rˆ:¶ê 4L
¦N¢d#éÖG=AÑ ³Vœ(Ëm
Ï’s¯ãƒ¶ÒëK$í¾DÇ€Ñ­mc$PÆÅNì±ì	}ù}2Ï$54€H¹›6ÝQ:@z—ÊDš·#zf:a²Õ)¢S…S\ µPèÄ¥ˆ“ÎW)HOÚLS»îæ¤.C%žÉ[®ÆºtO\9Såaqå]ý¤©fº‡<¨‚JïÁ,–¤”¬ÌèÒm=«1Jí¦E/*ž”^e<zeZ‹¬%CÊ7ydÎó†¥9Ó0N\¢ÜŒ~ÅFÈºy¹?³t:PêcÑÿ ôÏ^ƒ%ù7$ƒoySVT™úq8-v0õÚêÃ!<½÷ëÓÖìŽ²¹u_#þŒy­“'è¶ ÷,TƒSˆµY–j¬’÷“YiÓ¸¯GJ,	J,F[g+ùˆMVÀ@õÜ °ýGyÚo\®ÄÐéb¾hàyMÀ²P„œH¹¦ýá§˜ïòÇ©Îß¤k¹²É¬U è¼»Rà‘ÌäwZÅkržäQ‡áw~€4äDôh”	,Ó&œ›õ’ÖÙAMŽG!½$`!0Þ½£ÝBÍ«ˆ<‰¹êD¢¥9º/€Î]3í‚ÔçŠ £µÒÒ=uärIx#‰?©ÐJí¸0¿a3–…ºúÝxü—´è	îï–:b¥&"æ`Tù<†úÅÆiê&ÁSùéŠ:•Ç3ö=½Ö´óZÖS¾Ú‘”qwvF?¨–Ê*‘&Ý¥ïÌ£ÂšŽ¼çIùÌ¾Äj©mO–*ç:¸¾.ZDÛÁ‹²ç¶ÆÍ½fŸ
ŠÜ-“Ñ‰ÙB0XFÖp–e%sR:O¸6Ê×SVËÐwÐŽ¾'±ÀÅj°¶öàÂ/# t»HºÜRc}éŽHþ#X’\dC„ìÔmÔ¶¦®{¤Œ œ¼ŠŽ†ývw+×]À¢%ïÉ‘–´öÖFðè4Få—»š©ÑÜ§ÿ¬^8BÇigˆ]p L÷ÑÒ°HdóÍ¬CiÉló!¦SÙ®-
¬åb¨ÎžPo¥ZªŒÅÓ]ªPÝ
'¯Ptø
¦µ±6)þ-\(fŽ°ýF:ŒaÎŽ(™FÏb¡øº[Ó ­vþýY¦“"À§æš7ZH©Öjªv2Ó®–íaNTOÎÀ<ÿYa žr Ž–A°¹ë‡w¬A0\ê1ÉÛ4Ÿ×z &PCMsOvULÞÞyÕ_#ËÞ½[ÃJ*$Û´úŒb…”nÃÔA=ñé¿~Ø &+uÏš5J
OqŸQoòí`õ=Ó‰y¸J/ÒXh®šÈˆ°ËÝŠW/þßº‘_bôØÕUH¹ƒß ‡»Wïÿ HÚ‘"ŸqÃAk2M0ÅZž"ß¡ÐP³9Ýä‹:p:Î[H'
T— X$W¶LÊÉásñ¦!.ÿó‹ÁËÑç[Þ­v:›ÀÀ¿ûµ˜O,²F‚b@Ò”#"]»Ü87q
I“£@I1Ìé¸¿6Çp×u°‘
sþR®Š
[u³9ÛB'‡K`¨è¨!ïÂ±‘R×e±£Jî™êlw½¬ƒýžR°ƒt/{þ‹«6ËR,púÀé‰¯å®¾1:´6F”ñ%C|‚æçÐ[L_¡–|à#´IOy\H¦JÒ^dTù Ç|_±Ze/_¯Wü Ñkù—”f)3!Ïë+7T¨ÂÁ–o»Cý}mÈR|H¶9ŸƒP´ˆWM}To^cÉz\Ú$àUù2ME!˜*tk~…:	«â,ï	ÈW‰MŽ:tÀSë@¥1þðÐ
¹éûg`ŸM¼¸ù_lã@‡(SA	/m#eá4þë–hkQs&žôÝtÈ? Ú—)5Þ 	–s¡¯7¶ëžÌ9ŽÈ²!ËPySû_¢Š-•1“%ÉjÞ×uÈ·Ðßºl­z3ð¦È2ûDÿgë/dk.Î»Ô¸A
(ß¦€K¼ñkýOõžTó©ô5¿>Ogü¶8þ­§Cûj7¸¶Ý[;9µÌ3 .Í#c¥cþ,…Û'/ÞŽó7I+Œæå;Pb4Â×÷¢†¸_pCü–Ôˆ¹ñ»!øS½ËŽfâ
>°¥À„RÁC5É*ÝÅ‘“ðb¸G€ócQ†ã[¶i.ÒK%à°\–¼ŽM sœRý¿µZ7V~à‘¦§ÿ¼¨Ù©1£ïZ{¬!ú+á  Íj$ÝŽu;âÂd,Ìâc^)liiN#f
à GààÄ:K×µs¦˜Œõ‘„&|8¿h&æ¶‹Óó®¯»Ñxdwé¦Lß¬ßªmô+_œrŒâ±îaKafÐ%7@ÚÉŠ4Tñ§Q…·Ð­ð|î'ˆ“ô‹‚}º	pšHã¾Èçã!ès’«I$\È¦+®Ÿ PLú"àºmwÄõké5ÉsÏç´ÁÙ2û!ÞÎ§ÇYÊ$—ºùC{™=¶ÁÉ!˜ˆþ$ý¢§'¢›Hƒª8µocÑ±±½¾ŒÌ¨5ÒS,Sä=„˜ÌM #¢ ýÇì„fáu*JM§„à
û¼>¥®#Å¾º9¸JuÞ=,ø@„ŸnÌ¥ü¹¡â‹«<ÎÝÙ*=òl%×#-¿¼w}¿ hûyxß½\i™J3páuCçÞ®î­ä~¥¦”×e/eø-ÓÜ9G_‡æQ¢NÞ'nyrŠ†«Uxr”oŽxŽ@™BÒ¢‰'rEÌÍZS S“ûí|—îjhú„mÐ[+ðÖxók·ÓýPºZJ[J6lP£]®ÍÎÖ*ß&Ûá¤P’Ð¥?]DÓÔÂœºÕÅ´|ÖžâqŒ÷3‡`Ë—–´¥Aùç]gÆRu?¾4\6Ò€ýtŒþÝªœ#¨¹%·h½õ­š¸Hö<ºáßQ£¾âI4ùÄÜÖu!¶BÒi'E4ñ*´¸„ý¨
"HFÔ{
±hž¤ ü+mRGNíëæ9ÂT×§g@i` cZ‚>$’Ý[®ÇÇ)Ù·"|8`[lé|@µµê¢då]·¢nµâFsN¿þè˜6ðÃ¯¶‡
•ŽXZº2³„¿VCSh(Âôf…”)´!âµçûÃ¡±ÖJAA™RIËÉf½qh4Æ·H·ªþmB¬e”Î#ÏC=òx—KGÈ¦iTgÂ.VJ$7®žñb¸70÷fp:²ˆÒ‡¸ú ÉÃgè PlÆÐ[R(-m7îñl†Â}&WÔ*¿„þ» w0lÝ7íY½ñÏ”wî‘tºT›ê“?V¬™¶ï¡EÀÃ†É–Ñy“© Õ…Ãr÷÷Ö‘¿Y‚Ž±¥hûwnQ¬Ädê{­ò²¹ÞÉ”C2+—†ý-r7~Ü½Å_\G£Ñ:m ÞPý¡‡õ ÑÆÒ§óöÈ¸3†j)DÂŠm%â*é·Ž@U[ÓM¡þ€©“,µÒ´%¦Ž¼[ù¨ÿn?T§ÑL™_¡ü?k«¹¦å®3?ŸBoÌ<ÂßÕã‡‡XýäÍÛm›òab®Srbkh?ý×H*X?iQL’Ô»—{‘n?òåôOê ùKsô5\Â$sÖ¿šH£ÔQò$UÅsÈ1Èq¾ù¾Ã~ûYAAuÌ…‘¸%ÃnqKÏd˜ìŸ“ícnÅ±]=‚-©áñtpò˜•f·ÅFOe®†Ÿ"Khœ(~â^3åd>kÛpØJ©í©–€fJ  êù{… ©:Í>÷¿Í *%SOA2&‡AÊ±îøŸõtç9ŽÈ‘$3¢”ØTÌƒµÇóa¾ÆA—þ“t‚QØ6^•xòÚØËÑŠþ-a°¨t¦]Ë‡›¯öJ¯k5†­Ê´(M˜…7<d(þÙ~s_§jbvÙN\Ó¢—‹ €DÉlz¸r½"i«/}¬w³Q(Æn…p€_AÅ_°‹‹ŒÃR­k«[ÌU)Q†Ú‚êº_ë:\§¨DD#ôkSÄt¥Ž ~§Q%/fdR?n˜s>hTx†4I–8|p›	ƒ³‘(áº9qÈvl¸¶²Û[üòpQNñ,×ÀU¢˜ä‡•CJ1g=þÈ@T œ KõÙAÔòÂRxÍƒŠ¤o!¢Ø¿XC‰Æ¦ñeijÔˆDDGÁ'ý3[çAô_;8Øë§~Ò˜1b¤æÓø¬BMLê’¶=àá ÎÍ#ð«ÎõP¥ QBÉŸÜ|Ð˜0Bz!&è†K€m’tÈ«÷ÉË#vIÁI0ÙÌ‚5»!:^gôÌ u‰J'GsO‰zöËÃ0ËKOg¦‘°íPm•cˆ7¤ÅŽŒœÂ9-'×Lf™“¥;
¨ Š·`xÛ7è4Óöo›¥6¥ÃFÂIqVÍqª[	ëhitÙ»?æn/B»•€gŽfüû¥ý]±AÇšQ¦e^1Ë‰a‹ËCõÎÁb©M[7ô°5ïÍ-öWP…,‹¸Ø6n¶"ž™¡©lsXN nÒÁ|d>Êµ2ƒ\Â‚%í³#j§:›ÕrImDŠM-[~ZHƒO´OfŸ@å 'ÊÍ>ÜñOJ}++Ì(ÒU9¥}¸‚.škfÑx­
ß½üQFóU°*«Í?qòP|›f?SQÿ@1ÁGC|hç'n.æHþ?ÂDy	J,ŠÄ_Åy)hÌ÷yÖÆó‡•?|b^¿;Ó±î‰õW-Úâž31¿Jc¤=£'úòÈaYˆž€Ò‰Kõ!ó~<DïŠq.ßª—0–"i"î™hH}±@×>X+ù‡¶3)
m±$´"N;zÈÌêƒ¥¯»ò#’Z±Ô.Æ;öyô Þ¯Œ3î¸*
›a¡ç?Éºs'Øz-jÁß­Ü&‡­;U¬Üœ¦!QÄýç¨~‰F?5Å9WõÛ.AçE†SGÕ–¶óãµ¢ùp™{¬Ý.€„ÑîyÝÇš'	¢×¤4Šã…ó‘Åt÷ØCÐ®y†f Ã½mMØèÉ^	×-°ßæšÂŸ"*¨Me<ª+ó–jÖÍÝã-/œ|’Ùœža/ÙüŽ(€ÐÍ4Yõ cãûm
¢Vãô©+P#+Mi†f™z*nÐÌQêIöDýåËKŽx´º3¾9&€l¦$Çô‹4µôšh¡øç 7æß”A¯rÿÒƒ½NÛÀô„.Fµ=9m”ETÀrÌqØ
²ô]&°ön	hÚÑSÒÌ9ÏhVMådyºPôžÁ„m¢üë÷ÁA/ÈÿÙb³NÙBH}øã‘º˜~¾.`¹·–º‹ØÓDÅ€—/-®Úþ"Š4ãÿQ#w[s$¬{­ð X.ÜvƒÑŒ%™~Ü ÀÙ€ke?7ŸÛ®
Iß*Éázr›HÐkiÂÜ	6zjA22á]‹5ç€´Dåºb–~¬åÎN’9î¤Î¶ Elp›Õ´¶ƒð=™Xï•ºD·¬õ³„ÉY·^^w>=áèxÇPŸ,rIK¶”ðæÀ¾4¼­G!ƒ~¸
\d¶À7/QY'ÙÊ«nø-Û/)\{*³&‰™œ#gˆn4Ö;¬ò±À:¼¬âz§õßåéŸºAàbu’¯´NÎó#cð…‚ÈÉdkã›ï¶ÍREøiÏió6V1Pió~r¾bŽlÕIµ9æøØX:‚Îœ/¦*ˆŽñ±(ÝùU×òäµ¼-†%ùÇyU~Lt¼¦M+Œ…OAÉúŽ3™qzÑ#™êòÂÑ¡jÁðqâÍAž;Wê>üAÇ¬ò„äÛŸLÕ{²ËþƒFbÀG="·È¶G‹µKWÈŽ»qÒ÷Ÿb<-â
•]ãdp«‹qBT´êªNú…/ýÓy¬ëñÝ¤l›²­ó5g"“£…ß´Ø˜·'EÍûÍÿ«@E)3—Jf2W‰ ÐÂàdzdÅ{ÚŠû–,ùé*ôA.™ ²ó¶&‹ª=Þ(žC+Å\íÖÎÞKÁl|ýž®•šç¶–IÛ´PW…-.g.©ô	”f”J5ZéÓßò˜–Ô‡‘àãýÄZ°6^H¹ØšYÊ×Dä´Ø/tòŽVVª»š¢<€C¨ó#?2$b»bìé'–P ·Õ}N-‚ÐMË@øxýe[4ETJ5–ñ]\5MûÆ0þ Ù^®,@î<¸/­VŸðD‚w4¸l£|õn¶©”¨Qy‘§ÆëÌ$Ñb¸ú¢\î(_ñp¨YÔá;s~¾­%¤mÁs|]Ë†GÏ®RHFÂüHd0Z†s ÷F!¿­Qh“ã	½æB8û§s½<ãÿÜ¨„#ÿó•Âg+6Ç^õž°;ÂÕI€Àñ JÒÎ¿qîÅ2Æ¨õH$2 ²`VÌî5R£Ó*PmÓ“«Ý^§w¢ú`€1 ÝQÍ>õ.1†Œrþ›
Âž|"4Ø´&”Ñ>òu‡Böa¯MÑ>A'mö	mÑéU¹lYô¿´šS¢N¤ƒ¦·ína‚íÜÀÊoËO­É.ûæŸh­›: