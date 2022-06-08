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
  arch/x86/includeJ �a�|Q��G1D��)��U�A66���vy�7����rU�(f����6��Nm�)��$4�X���C�Y�+�օhh�谵� �@��^���7��-��Og׊P�VI�e	�;�%[N=r\(��U`�����}�0��Nmf�2�%��:��d_l�&�ݧ�v*�^��0&?�i��nA-�R`�n����};S"�Ɨ�:�S��*�
����u�����k"X�?��m�t�奌{PḦiŎ�G�l��Ϩ�R�4Q����Jq�`5�O�~d�����>dف?s&6�~K�0����'xI�nf'������9}����C��!�徲��Eve]�q��LGem�a8f P�S&;�M�1�$�-�l l t�H�wA�4�9�#�����T����b�D��Z��Ga�0e5��1"���5_]�ߧ��,^i�\����F���N�b�`f�����4ډQ=2�Y���%Aa��>��)v]Vd^�)��8�)){�������, b�"w�Gf��:B�9�Ʌe�3�N�񸇴�J��NC���H�y|M�-���i�<��!�\��ɞ��ê*UpcĂ
TǕ��g(�`�2I9�He��@N�]���\Hjz�૵3��:	����Ѐ^�J�+Myh�Ǟ:�{�n��o��g�1Llvr;������-��]���2��������/.洏I���-�R�!���1���������;�������#��h�{��V���?l7Ip�B�{8�P +X�L���J8�T�܈�63�ǐ��3��+�����µ����:=:�c�;-�t���j]$�I�� $��!O�OB��v�s�-��a���@t^��	-�Z%�T���tt��P^��x�?B�/{��c�8�f��jmޘe��#�&���sO��$Ӗ��v/9<ᵒg���m�Њ��4,9�s4��?V	��v�ij�#���f;*LPUv�n��e}���D��D�.8�!v�=�Z_8�2��<	Z�*cy�T���8�Ѭ�tE�^�k ���+��_h'��nk�U��nm��ĩ�Wͥ�zҍ���c�����l��b�hJ=V#���3�p���^�$\��hpU�IW$.=��0����^�R�K&�b��¯^ǐZ0�\@�ұ��Ǽ��*5��-@��ל�7���~���G1��*���jB���_X����`L{TÌ^�L�}E�m�L*�B�����^��sP�.m��x�Rtf.������_D�	&r�>����:r�\׭��2[&�RB�q�;t4��KZєp�^�ulf^LCӐ~�U� ?k $>X�\�G����K�e��z��LB=.B�`�zh�^���k�?�7Ym�PBeOȐ����z>U�Zܲ$��o~cTG/�ܽ�<v8���Db���C	���d�g���*�����ꥻ�W>jcS��ĄXhmE���ٮ���d� �w>B[�%�!m�Б��Ik�����F��L�_������K��y�H�W�;�B˗��]O�����RV1)�":T�����R�K�[g�H�*��m�����W�P�Wp��o��[n20"&��Y�������FH���:b�ۼ��<�1Q���(|ŏl.oN��5������E������DA��3���$v_B�L�B�%D�Q�v`�J@�Fƥ��ଣ��T��g=&��yџC�_�t:t�F���xX���Q�=�)!�L�JS ��^8r͘Cp����߾RtyhL`f�h����p+"���w-K�@D9l�+
ΣԜţH�e�������3(I}@e[O�z�M�Ts�j���K�����a5n=�<=i��DX�Ry%��������il�t�qR0��2�S��$�s~#��'��?��s�5�}Cp^R�y��O�c-3#����хyXzmq�d�TX�U�Xb$VSq����gk_LO��Զ��X�i!oG���uB�X���ڨ5�f�u���M�����T[f��mPu2��'��f@L|E���d��X�i�wU�w�}9*Y'�U]Ѧ�̉�v>��n�T��^�KJÿ���H I��b�Z�<(�NR�A]̸S,�r)���P�T�'��ཱི�?����.��QQV�dK�k�b�ݐ�a���5ˆD�S����Y�C�c.�ۀB�&u*��& �%Z3�S(�\�ܱ`,�Z����ͣ�Tg#�b���=
��#��st���fv$�gj�TNm9��7�n�48'wr��|�=y�_��t��2��Yr�8(� o[�IC/ p��V���_��/�jLqw��G��
�6Xn��ðo��hҊ��©n(/H6���f	�F�_nV� �/x�~�
�s�a,I�q�ۚ���o��)�ECo6U��=��K��*F~q��h�k��U�����S�Uz{8����c>Xfv�b�g��V�*�@�u��M�V���w^�Q&�'��̩Uhd	"���e�3v��# ��op_� wG#��i����� H����h�)<B�F� ܶY{�{o}������o�(���$����1Z�nX����=V�8��ڼ]��$�B�|�T&���k}��O\��0�&�|�0���c)�P�oJ���0P�|���X�C�S�i�H��W����$���I�����+Ăsb��n�����?�Z���*�U+��y����ӎ�����DK��=�Ub��=,��O)�ޛ
l��@���~���K��9N���nH�n��"�����̃O �x/�*�/��i�O������Z���d��n��Q�G�Z���=�a���s�R�������p��$5n���Y��u�6�֩�"��܄ѓ�]�]���������q"bvZ�̰G[�)W9���a���i:�z%�ȕZ��L`�e��;W>�� lj���� y5�O��X�H��<�2�� gs��:xՄH����]�8}��p�/��{���U���t�&+KY��A�M�{���&��a�Jmh��>�����0oK�l)�<�����\R�@�Z��9�m@=�O}�q��q�(�_X�2�*[wĴ��I���w��X �tt��\�ATD��n�2{������RH!�M�!�2�3��Ӄ9����g�-Jl����w��k�׼-Ӡx ��le���#��`��
q�[����ǰ�ges����J�ͭ�n9��E!��\i��$(��N�l�G�NI���W߁^#���f} Qz뀄��Yrt�m�F9�tނ���W�I���i�Kj?!��)Ho��k!�0Ü��A�E�Z|ϧ�g�?sfQ��m�sOm�	/���)f�uqqE��q:X��'�T�Wo���[����|�}���
�A�M�����3k�ܐ����'
{�H9}c+�*#ù_�3���5୬u~�1�蕖�{�0NҶ��a��?��=U�xH��`��:����H����w˭�#.��C(��o�1�W�����iӦC�n�ji����*�#���´��H��rY�$t��/l����D�1Pp�@��sf
�%�l[*���XF�UGU�sP+��e/#�m23@v�t������������:aH������t�>=� �÷�]o����]�o�(�/"99�i�5�$��C̑�(缱��p�Px�����FM-��WP�j�K����)�U�e�Oz�����v�M�Y�u�d�h5�G��U����J���()�����pPy$+F�IWS�ߦ��]s^��>>���� F�VC�rm,I�ab���b&�/"�˯U|g��"��ƣB��c%�
Q����]��[Q��	���@�R���$�'�D���Iμ HΫ{����7б�<Aɽ: Ϲ�6����f6{谭��4�c��'��{R�m�Dh�ՕI|vΪ�]	?���S��m�q�h��E|�����.U��Ɣ�/r&_,
������wþD-��6k<e�H��#rx^n�����ݣeɕX���V�%JƉ���%�t�6[�aŅ�P3׊���6t���$�^��>k�E<��i6�7�l��O9�GG�� ^忦~�<�X���j�番�l�?��Q��\?B��I8vJ�gTMmSZz@�c��}r�r�)6���I#_U�I��*�6���
'���\�Q��+<��h�=y$��?~�G}af��m 2�U[tU1W#c�f7*Ϊ���������lA�\6?x�%m��B�U\���1��'���J�Q�z�G%y�����|��7���9��� �=�Dզ�����o��2��(�\�ҷg.��j��;��_~�7<O�f]�Ѱl�jG��pY�V~B�d�*��<���$���w`��}�w˖��iV��I�Y��~Fm�$8��!�K��VG�2�N^�=�	��9��4Ν7��K�ƙ��Y`ي�� �<�D���2K�I��
z�#��oۋ��ô{f�y��R���F��qR�I�i'���d�*1�h�}�C�`z�(Ta�wP+@<��>�K�0�ēM��r1�c�;l����,�UY�D<�@R�Wt�BhsDj�92�~��!s����&t�䠜��.�0�vkV�ibG�t���o݇+�Z/ �,�P�]� ���r�[{dY?�)*����3S��n���&yb(���m��e���?�i_���$��2��;軩�d�@�ϟ��i3��b[1T?���T����gP�_$FG�pݺa�`"��蕦��Q�!��̀v�(i1�
;���0��k��P*�Ny�Ȑ4�D�.�۽
S�xԮ�[&��sH�M"�h��m ްf�%sY�N���6�9�����hnA���q&.��_|)�y<b�+�T4�&n���?n �R��@�U�A3��B%�xk/7�'/�H�& �J?a�X��L�G�����O����T�=7�����o������N�؃s�r�)?��^82��d6vt@ޕ�D!�zyn���-G�G��&�"U	�,at�0�⡓w���)�a�F�Y���Xu���fqZvi��Ir��ӝܤ�"�*������
���l�}���.��n�ԣA��%��@����^�0��!��f�����Lw��s�;T��>\�DpV�+ݬO�}v�j�������r"�x��c\�^���|����)X�1������I��V���F��݄!�|�Ȝ��u��I�I�,�V�o�F۲�i~�X�{��P��/J��,�:�0�s@ykk��K|���C����Dv��U� ���TR�w)
� �k�/H��9�!�1�._KN���b2�`�_)8`�)YI�vh8��$�4��A,�Q����[ ;׃	Nyᤝ��e�|0Xy�nª )� �qy�X�g�q����vw���N�h1������~jh��g�⥳n�ز�0�l�%������(�u��6^j�篊�t2��C��ٚJm.�5���.�H���F��k�~�1`�XN�m�t35�z^��SM$�D~�|��?��Gq�bi{1B<��lo&ܧ#����@c8`_e�ߒvoc�̝�|� K{�/�3:��1'U&�<�-�te���]9��$+�f���}����g\WZx��Z��-rr�حb����c�ee��.Pu�a��,�6l���J�V?<�MJ�><"8#�_l�O��A����z��w(�Z����!"KAi/�u{i"$�e�2��07��C�·|t~��TLoU����T۔��JԾ�ނ����Y-��^�]f�U/�5x(�V��'������<K�Kc/2�qW��ɉ^�ۘ:='�(De[x�Bٞ�?�{�2�����Z��*<� P��V���5�5l�(g��U|��ϙB�k��a�e��V+}�O1.Ĝ����%��P���a_�W��m��W*g���f�&7��y9�q��З�������K�d?�����Ҝ��9��)�g<ڃʼ}~��Ǜ�7Q9=��S�{�Y��:7��#Z��fj*V�¶ck+�_G�#&��j��l�Y$�|~��X��u�:��ڀ�O�KislA �gͭ�(	�t��A�W����XǡWk��&&�w��տ{+���ĽI�Cƻ'��C��BN!-41�_UL��&7t/���1oH�a���Y�hb*CZ�|����Z��
x��/v��E�r*gbn���c)�ii�)5z�o3�2j��ڷ�Nb��m�s)K>1M	���L��w�����Gܑ�9��}7�UOj]� ��FpH�w[:�o��ւ+��j6\�?�j^�q���Y���e��uy�m.0�۴i�kl0�Ʊ�{�%��4��_f�O�|��S�-J 5��l͇誖�S����R��LY������[[��-(ϸ�rD�n"GSþSv��?c[�^�u_L/~���+� ��t�B�4�1K�"��p���0�'���|�{����5s��,�Y�N0���x�}�tl��J��<��\W��y�
5��`e� �8ٮs(���|�Ǝ��&�l���W��T��u�>�� b��ٜ�-�T�ߧ�ۅ�e9��
�G���>SwSj���/�nʕ�@��ph�M���v ��d�����B��5�^�F#���i,='�dƯyA�;��E����p1���h	u<�-� 3_wx�L.��ƒ�V�+}�ͭ7�a�`�,�=pH@}urU;ڡ�J��Qh}����@�/�3>�v�`�sۑ����#�����{~�e~�O�(���R���j�WNRq���ֿ,�(�����p��.Q��i�R�ʶ6�%��Rt����=Z,�a}i?�3x��RvR-Qe��eL��lf��3��]3
����~���z_�k~�J��g0�-=�$��b�Vz��`��ApRz��|&fOC+�v��Ђ��p���L$u_۠���oZ�N�uN�)��u�7�~D�y_��q
0���U��.���R}2[�Im�<��Pv�ȟ�j^<P��~��T�i���d�h ����moZUw7�ӓ��6S�Ŭ�4EWe��.|q��� ^��M�,�P��S徍$>*ScZ%�:��Q����^d���*{ �����f&�@�r�:�� 4L
�N�d#��G=A� �V�(�m
ϒs�ヶҍ�K$�Dǀѭmc$P���N��	}�}2�$54�H��6�Q:@z��D��#zf:a��)�S�S\ �P�ĥ���W)HO�LS����.C%��[�ƺtO\9S�aq�]���f��<��J��,����̐��m=�1J�E/*��^e<zeZ��%C�7�yd����9�0N\�܌~�FȺy�?�t:P�c�����^�%�7$�oySVT��q8-v0����!<��������u_#��y��'����,T�S��Y�j����YiӸ�GJ,	J,F[g+��MV�@�� ���Gy�o\����b��h�yM��P��H���᧘��ǩ�ߤk��ɬU 輻R����wZ�kr��Q��w~�4�D�h�	,�&������AM�G!�$`!0޽��Bͫ�<���D��9�/��]3���������=u�rIx#�?��J�0�a3�����x����	��:b�&"�`T�<����i�&�S��:��3�=����Z�S�ڑ�qwvF?���*�&ݥ�̣���I�̾�j�mO�*�:��.ZD�������ͽf�
��-�щ�B�0XF�p�e%sR:O�6��SV��wЎ�'��ŝ�j�����/#�t�H��Rc}�H�#X�\dC���mԶ��{���������vw+�]��%�ɑ����F��4F嗻���ܧ��^8B�ig�]p�L����Hd���Ci�l�!�Sٍ�-
��b�ΞPo�Z����]��P�
'�Pt�
���6)�-\(f���F:�aΐ�(�F�b���[� �v��Y��"���7�ZH��j�v2Ӯ��aNTO��<�Ya �r���A���w�A0\�1��4��z�&PCMsOvUL��y�_#�޽[�J*$۴��b��n��A=��~� &+uϚ5J
Oq�Qo��`�=Ӊy�J/�Xh������݊W/�ߺ�_b���UH��� ��W�� Hڑ"��q�Ak2M0�Z�"ߡ��P�9��:p:�[H'
T��X$W�L���s�!.������[ޭv:����������O,�F�b@Ҕ#"]��87q
I��@I1�鸿6�p�u��
s�R��
[u�9�B'�K`��!�±�R�e��J��lw�����R��t/{���6�R,p��鉯宾1:�6F��%C|�����[L_��|�#�IOy\H�J�^dT� �|_�Ze/_�W���k���f)3!��+7T����o�C�}m�R|H�9��P��WM}To^cɝz\�$�U�2ME!�*tk~�:	���,�	�W�M��:t��S�@�1���
����g`�M���_l�@�(SA	/m#e�4��hkQs&��ݍt�? ڗ)5� 	�s��7�랐�9�Ȳ!�P�y���S�_��-�1�%�j��uȷ�ߺl�z3��2�D�g�/dk.��ԸA
(ߦ�K��k�O��T��5�>Og��8���C�j7���[;9��3�.�#c�c�,��'/���7I+���;Pb4������_pC��Ԉ��!�S�ˎf�
>�����R�C5�*�ő��b�G��cQ��[�i.�K%�\���M s�R���Z7V~�������٩�1��Z{�!�+�  �j$ݎu;��d,��c^�)liiN#f
� G���:K׵s������&|8�h&涋�󮏁���xdw�L߬ߪm�+_�r���aKaf��%7@�Ɋ4T�Q��Э�|�'���}�	p�H����!�s��I$\��+��� PL�"�mw��k�5�s�珴��2�!�Χ�Y�$���C�{�=���!���$���'��H��8�ocѱ����̨5�S,S�=���M �#� ���f�u*JM���
��>��#ž�9�Ju�=,�@��n̥���⋫<���ُ*=�l%�#-��w}� h�yx߽\i��J3p�uC�ޮ�~����e/e�-��9G_��Q�N�'nyr����Uxr�o�x�@�B���'rE̍�ZS S���|��jh��m�[+��x�k���P�ZJ[J6lP�]����*�&��P���?]D����Ŵ|֞�q��3�`˗����A��]g�Ru?�4\6Ҁ�t����ݪ�#��%�h�����H�<���Q���I4����u!�B�i'E�4�*�����
"HF�{
�h����+mRGN���9�T��g@i`�cZ��>$��[���)ٷ"|8`[l�|@���d�]��n��FsN���6�����
��XZ�2���VCSh(��f��)�!���á��JAA�RI��f�qh4ƷH���mB�e��#�C=�x�KGȦiTg�.VJ$7���b�70�fp:��҇�� ��g� Pl��[R(-m7��l��}&W�*���� w0l�7�Y��ϔw�t�T��?V����E�Æɖ�y�� Յ�r��֑�Y����h�wnQ��d�{���ɔC2+���-r7~����_\G��:m �P���� ��ҧ��ȸ3�j)D�m%�*鷎@U[�M�����,�Ҵ%���[���n?T��L�_��?k����3?�Bo�<���㇇X����m��ab�Srbkh?��H*X?iQL�Ի�{�n?���O� �Ks�5\�$sֿ�H��Q�$U�s�1�q����~�YAAu̅��%�nqK�d�쟓�c�nű]=��-���tp�f��FOe���"Kh��(~��^3�d>k�p�J���fJ ���{����:�>��� *%SOA�2&�Aʱ����t�9�ȑ$3���T̃���a��A���t�Q�6^�x����ъ�-a��t�]ˇ���J�k5��ʴ(M��7<d(��~s_�jbv�N\Ӣ��� �D�lz��r�"i�/}�w�Q(�n�p�_A�_�����R�k�[�U)Q�ڂ�_�:\��DD#�kS�t�� ~�Q%/fdR?n�s>hTx�4I�8|p�	���(�9q�vl����[��pQN�,��U��䇕CJ1g=��@T � K�ٝA���Rx�̓��o!�ؐ�XC�Ʀ�eijԈDDG�'�3[�A��_;8��~Ҙ1b�����BML꒶=����#���P�� QBɟ�|И0Bz!&�K�m�tȫ���#vI�I0�̂5�!:^g�� u�J'GsO�z���0�KOg����Pm�c�7������9-'�Lf���;
� ��`x�7�4��o��6��F�IqV�q�[	�hitٻ?�n/B���g�f����]��AǚQ�e^1ˉa��C����b�M[7��5��-�WP�,���6n�"����lsXN n��|d>��2�\%��#j�:��rImD�M-[~ZH�O�Of�@� '��>��OJ}++�(�U9�}��.�kf�x�
߽�QF��U�*��?q�P|�f?SQ�@1�GC|h�'n.�H�?�Dy	J,��_Ły)h��y���?|b^��;ӱ��W-��31�Jc�=�'���aY���҉K�!�~<D�q.ߪ�0�"i"�hH}�@�>X+���3)
m�$�"N;z��ꃥ���#�Z��.�;�y� ޯ�3�*
�a���?ɺs'�z-j����&��;U�ܜ�!Qď���~�F?5�9W��.A�E�SG����㵢�p�{��.����y�ǚ'	�פ4����t���CЮy�f�ýmM���^	�-���"*�Me<�+��j����-/�|�ٜ�a/�����(���4Y�� c��m
�V���+P#+Mi�f�z*n��Q�I�D���K�x��3�9&�l�$��4����h���7�ߔA�r�҃�N���.F�=9m�ET�r�q�
��]&��n	h��S�̐9�hVM�dy�P����m�����A/���b�N�BH}�㑺�~�.`�������Dŀ�/-���"�4��Q#w[s$�{��X.�v���%�~ܠ�ـke?7�ۮ
I�*��zr�H�ki��	6zjA22�]�5瀴D��b�~���N�9�ζ Elp�մ���=�XD������Y�^^w>=��x�P��,rIK������4��G!�~�
\d��7/QY'�ʫn�-�/)\{*�&���#g�n4�;���:���z����韺A�bu����N��#c�����dk���RE�i�i�6V1Pi�~r�b�l�I�9���X:�Μ/�*���(��U��䵼-�%��yU~Lt��M+���OA���3�qz�#����ѡj��q��A�;W�>�A�Ǭ��۟L�{����Fb�G="�ȶG��KWȎ�q���b<-�
�]�dp��qBT��N��/��y�����l����5g"���ߴ���'E�����@E)3�Jf2�W�����dzd�{ڊ��,��*�A.� ��&��=�(�C+�\����K�l|�����綖I۴PW�-.g.��	�f�J5Z�����������Z�6^H�ؚY���D��/t�V�V����<�C��#?2$b�b��'�P���}N-��M�@�x�e[4ETJ5��]\�5M��0� �^��,@�<�/�V��D�w4�l�|�n����Qy�����$�b���\�(_�p�Y��;s~��%�m�s|]ˆGϮRHF��Hd0Z�s��F!��Qh��	��B8��s�<��ܨ�#���g+6�^���;��I��� J�οq��2ƨ�H$2 �`V̐�5R��*Pmӓ��^�w��`�1 �Q�>�.1��r��
|"4ش&��>�u�B�a�M�>A'm�	m��U�lY����S�N�����na�����o�O��.��h��: