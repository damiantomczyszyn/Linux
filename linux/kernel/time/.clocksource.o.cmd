ard include/config/ACPI_APEI_GHES) \
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

drivers/media/i2c/vp27smpx.o: $(deps_drivers/media/i2c/vp27smpx.o)

$(deps_drivers/media/i2c/vp27smpx.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     free cgroup_mutex mutex_lock mutex_unlock system_wq queue_work_on seq_printf __stack_chk_guard bpf_stats_enabled_key __x86_indirect_thunk_ecx sched_clock __x86_indirect_thunk_esi debug_smp_processor_id __per_cpu_offset __stack_chk_fail bpf_sysctl_set_new_value memcpy memset bpf_sysctl_get_current_value bpf_sysctl_get_new_value migrate_disable __rcu_read_lock __rcu_read_unlock migrate_enable strscpy strnlen fortify_panic bpf_sysctl_get_name bpf_ktime_get_coarse_ns_proto bpf_strtol_proto bpf_get_current_cgroup_id_proto bpf_get_current_uid_gid_proto bpf_base_func_proto bpf_strtoul_proto bpf_event_output_data_proto bpf_get_local_storage_proto __kmalloc css_next_descendant_pre percpu_ref_is_zero bpf_prog_put __x86_indirect_thunk_ebx bpf_prog_array_alloc bpf_prog_array_free static_key_slow_dec __x86_indirect_thunk_edx bpf_tcp_sock_proto bpf_sk_setsockopt_proto bpf_sk_storage_delete_proto bpf_sk_getsockopt_proto bpf_sk_storage_get_proto bpf_cgroup_storage_unlink bpf_cgroup_storage_free percpu_ref_exit cgroup_storage_lookup bpf_cgroup_storage_link kmalloc_caches kmem_cache_alloc_trace static_key_slow_inc bpf_cgroup_storage_alloc cgroup_bpf_offline percpu_ref_kill_and_confirm cgroup_bpf_inherit percpu_ref_init cgroup_bpf_prog_attach cgroup_get_from_fd bpf_prog_get_type_dev cgroup_bpf_prog_detach cgroup_bpf_link_attach bpf_link_init bpf_link_prime bpf_link_settle bpf_link_cleanup cgroup_bpf_prog_query bpf_prog_array_length bpf_prog_array_copy_to_user __cgroup_bpf_check_dev_permission __cgroup_bpf_run_filter_sysctl __kmalloc_track_caller __x86_indirect_thunk_edi __cgroup_bpf_run_filter_setsockopt __check_object_size _copy_from_user lock_sock_nested release_sock __cgroup_bpf_run_filter_getsockopt __get_user_4 __put_user_4 __cgroup_bpf_run_filter_getsockopt_kern cg_sockopt_prog_ops cg_sockopt_verifier_ops cg_sysctl_prog_ops cg_sysctl_verifier_ops cg_dev_verifier_ops cg_dev_prog_ops      F     G  !   F  *   G  A   F  Ò  F    J  1  F  Q  F  a  F  q  F  {  L  É  M  õ  L  †  N  —  F  Í  O  Ò      F    L    M  +  L  0  N  :    @  Q  m  R  è  T  ¶  R  ¡  U  “  V  ‹  W  „  X  ¯  U  °  Y  ±  F  	  [  A  F  j  [  ä  \  ß  \  ¡  F  Ò  F  	  \  a	  F  x	  R  
  _  
  `  %
  G  m
  G  x
  a  }
  b  ¥
  R  ˇ
  _    `    G  g  G  r  a  w  b  ≥  Y  ¡  F  Ì  c  	      d  7    <  c  Å  F  ó  R  º  c  œ  R    Y  !  F  ë  F  !  F  !  F  :  :  V  g  b  h  z  :  â  :  ü  i  ©  j  ¡  l  —  :  ·  :  Ò  :    m    n  !  F  b  o  °  F  ∞  L  ∏  M  Ê  :  N  p  v  p  ã  q  ∂  L  ª  N  1  p  F  r  q  F  á  R  °  _  ¶  `  Ø  G  ‚  T    G    a    b  -  R  A  U  R  V  \  W  c  X  x  U  ¢  Y  ±  F  ¿  R  ﬁ  _  „  `  Ï  G  "  T  N  G  Y  a  ^  b  m  R  Å  U  í  V  ú  W  £  X  ∏  U  ‚  Y  Ò  F  
  R  p  _  u  `  ~  G  ¥  T  Â  G    a  ı  b    R  !  U  4  s  >  W  E  X  Z  U  ü  Y  ±  F  ¬  i  ﬂ  :  Ò  j    :    m  !  n  1  F  À  t  q  F  Ç  p  ó  q  Ω  p  “  p  Ô  p     q    u  *  p  E  p  W  u  j  p  Å  F  •  :  ∫  K  ”  r  ‹  @  ·  v    F    L    M  F  L  K  N  a  `  Ç  L  ´  w  ¡  F  ·  F    F    x  3  :  J  y  a  z  u  {  ó  :  ≠  i  ª  j  —  |  ·  :  Ò  m    n    F    @  /  L  4  M  l  K  s  v  ú  r  ∏  `  ’  a  Ì  u  ¸  @  #  }  ,  ~  >  L  C  N  V  `  h  a  {    ™  T  ∑  `  Ù  w  !  w  1  F  M  R  ‘  :  π!  Ä  Ù!  R  â"  r  ù"  Å  ¨"  Å  ∂"  Ç  ≈"  É  *#  @  /#  Ñ  E#  Ö  ã#  ~  î#  ~  ‹#  ~  Â#  ~  $  K   $  ~  )$  ~  =$  Y  Q$  F  q$  `  Ä$  a  ±$  F  º$     $  R  Î$  â  %  `  %  a  v%  u  Ü%  `  ò%  a  ¨%    ∑%  R  Ò%  u  /&  w  9&  Y  A&  F  U&  ã  t&  L  &  M  ò&  L  ù&  N  ¿&  å  —&  L  ‹&  M  ı&  L  ˙&  N  '  r  '  `  "'  a  Q'  `  `'  a  7ºƒ‹g0id¿ê?Ω’vJ˘¨âE(YÌ.¨ËAî∏qºd_(ÒıD4:F›§DÁYæ©z∏Œﬂ¡3≈’ «SÍÔÇªç©ŸY*tóoÈ2Ö)lûÔ.˛Ja6ŸQxËª !£∞´≠)R.qCNº◊]ïtØÔc8-K≈&Ï∏o«sôÎ-a¢Ü}6H/ìÔx®9∫,êºéÀ√ı»…≠kõõâ˚ú_ox⁄,w PU:¸Â$u˘ÕDôìã…q5-∞ú4⁄Löﬁß…Ë(êœ‡·jjû#Õ[Ôˇ”Õ„ÉHû]ΩÉ~8‹ßEã¥+Qˆ+Ü"JÉYóûp2r∏S≤R§ΩZ’[ôºÅ^Ör˛ÚΩ/à¸œt† +]\!ùıã¯V≤§jFˆ∂W~\Ëf‰Dı‹|~`8ƒ›Ë¶ì§ èÚc}“°ÅháâFÒΩ ⁄P2PàππﬁCÕ¯∑ü‚´≠•û6uCö˛ÓÆgw‘2ì∆ãÒ%¨Ì6T\{Áß,˛ºwfÌcwò]Ãöq»ªÉìeΩ•Rm÷sÙÊØÑxÄñÃª’˛∆Á€√QÀíUëÚZRR RÌ”∏ë˙ÌfEuz¥∏àØÊ*Ÿò≈è„óﬂ2ﬁqf]m9e-(î›kôÃ‹!h1·„º÷õ9~U{ï)0W©≥∂ª/≈=5“ü1§mÔãı1‹¬YÀuLÒv˝¿Íôeïz◊ÜóçÔÇxé$ﬂ˙vﬂã(ÁNoÉœ	÷Ëxm¿CÚ3G¢Ë97lKqﬂ¿Õ–HMÂ‘9gìk!Æ¿[%CµÈgç‘ñXdCY#I∏õ1Y—ˆ5OeDT⁄aKã«ﬁµ€Tp˚KﬂÆ£ÂO[õÁ]ì'¸g9D¡œ@‡@W"cdçìîÌ¨®N≈R7·æxõ›XÕ)H®ÂÖ‹¬'Ä∑ñip/
Õï;ÖtO)Æv0%º˛?ç9* ˝›lÓIÅXJ&ˆ∂ña–©K√–ÀQZ@ù”Ê‹õh<è_\X‡¯∆ˇXHæÚ˙ƒ®”…∂‡ïU≠÷ÿáwÒIõÛ9O? †ü#ë‰ÿæ˚‘&_3èãC∞˛ê(	ÃﬁØy…ß› MN#)P(ŸO‘Íç™∏¿ıNôúfqúèÜãøi/]&∞]„aå©gÜW¬ﬂitIº—=©L°¶Ω1∂ÌsWh
Ê%Ü{¿Lˇ˛R∏Ÿû-OCˇ
¥Æ∂òQ<vIË€∫/AΩ÷úpﬁlÔµåïŸ/)seﬁ?P≠Pﬂ—pw2≥gÂ-í∑&å¥≥WÏ8fO˝‰:ÏGcãõ…å’ö¿ciL˜¬±h¶÷S°∑¨ˇá3çG7^{„èPx„˘&è˝–˛vﬂiåRHGóúéîFï==4i+E{Éò≤r¬S:~]ŒN7oıç®ˇ—åsæ”t©jË◊q€îÓ].=aà<ùGŒZ‡˛Í<yO2Q±¢ì‹i«D{VÔ¥ÅVdp∑bOûß7Óƒ≤~øæ{ôïyãÚhâ∆ÕMPËGÂ¬˘ßrÕ]>£Ç”5o∂háUƒç∂h˜‡ÉÁwwxd-ú#Xÿxâ”XÆ÷§≤_ﬂmòb'±'∏,T∞ûX®„œ12OØ[ÙkQ¶ÎíÅEL• )‹cRä˝˘§æ
å0\Cﬁ»îóSMw¿ÁLU¡«mD∆Û…‡Y9LâãjuÖÇ¿È5Ià≠˜I£%Uˆ$v¢¬©"˝,Á,G«ô¥·Ì~ _ƒyú…as.´ò†Å‘î¬›8e¨§êOp∂Aµ—›£@\ËqjˇÜ+eö\À´hm•Ú)‹≤ÍºÊ°ﬁË=õdæÜ?]5Ùxú§å3ôπGõ§H¯ÎÑﬂDM{„…::	=7êlàÿuΩòu°Ú–t0¶¸ëaÌ{Üë· ØÁüD™†•°Á◊ü»ú∫•0g6ï˙ªù’ˆ(À‘¸ol
À8«dØ óÕWCùØ–˝∑Ö0Ã˘ÎÕ≠`VæÏrÿy‰7ã˙∞æ}ú–´@2ºÿ©√3‰¥?œZ“3éè·	ÚÏÿD(0w˙∞Â˜?∂¿ˇuAöwéÉn,á˝≤‹)ñZ+k£ÛäÔzñ÷¸¯√.≠¨∆‹äNo2Ñ7:πÉ:<ˇÓıøã¡≤∑™NùœÈ&X…<òXèc(^F{ê¬}ÖËF-
7ËkNÇèÀ~7=(eyw√üì°Äö)áÜÂ1œ¸‡‹‹KäèÀC0Öº“ºÎF“…ˇûf`°{˙4åÿ±Ì[D6¨¡T[¶u}/úÃgÄ'“ñ8Ã°—¨Ä(ınîÛçüT[ü¨'Û⁄~H&)zc0w]ŒÔømoœ Ú~p)s.#µ‹'-ôˆÑG˘‹ôeU¢~è∏ú‰KıPYC;k{?`Ï7€’p£·Úó|®RÁ”J˛6BÍz[∞⁄0tÌ
`¸£›ZO\/•ƒ4¶àh¿‡e–‚àÊi⁄»=&Ëﬁ≠™Çµ`◊¬_Û‘àeB?æÑ¿d—{}]€m§≥y∫™P∑√KcGˆJ>™õMÆ	a∑,æû¬1¸¬C9¶†™áâÑGáRÎ˘i?√™_Q RLAÜâÁ∑üí{N_u2‰ıƒ≤ºÌ2}1©Úc:F»
[…Ù.x¨·À¥SﬂÆ%'OÛ≥ËPêïû@qêÖÜÂƒM¨¬◊ñ˚…0F\ªô_¥Dñ£ ∫J}Ùæ5iv≥òê=Ô>§ÔéDÒ2›yÌæ|ÚƒÆBÙ∏ÓÙ¢)7KîÄS´ROºL–˜¬5nDâï¨ci¥® ¿†˛—Ë»[“∫ü\-=F◊dó∫·`Ÿæ®¨èY√˝“lÈÑºá ÆI2ö£‰≤Ø’•gmö ñõ"4W’_CYZdÄ‘OÇ⁄¶§çØ5™ƒ™±ä˜ﬂwHó¬SΩÚŸ7Fq¥¿–⁄>ÔÔÂü6 ˜7)!{ÃF}(Ü.
.c+‰YÚ¶»∑=|ñëhn∞íw∏èœz≠°ÇnY6˙dﬂÃÃ¸[- ˆ&ƒpk_É\¶¨îEC»’√/UÛO»“"Å˙Ù¬hÉ0⁄#¸˚ÅSà>Tâ‰’6_z˝*#ÁÃNå*Üó¬–Öæb)JèﬂTcqV≠ªb*Å√õ†ÂjìÁë’Ñ‰1tB√r”ü	xUl#Wæ≤Û¥Ún–>âôK∑Gà,Bdø^§$æØÇ¬?∏Aª∆®∫6¨Q˚Ä<fl[_»ºÉpk´^83NÙq◊kˆÃvA‘ˇ‘˝(Âê*Ús…˜ÚÚì5.ﬂÉ˛/Ng4≈»ñ&≤∞ø™Øn¡]∑bpB… Ïí’Gûy—_øÇdk8≤Ü˙_dØçı_U≈YN∑(iÍ·&—’3§¬™rwÎç<Œ"m$¿‹„#˝¨<£lˇÂøå§F«"∫a™6ÈàªA÷òÎØ∏\0Úí@ëç◊ß«ÖtML\ÚÖ!◊©y´'\‡
øevÜ%Ô9}√Ì8xÄµïÜ™BÅoóñçÕÂåΩ]—!ìÁJﬁ‡
enˆ2+|'"˙“D(l˛_≠\rüe3àeX}.•ˇ,f•#©ë7∞6â≥IfYÇªL“‘=gåÄ∞ÊRÎQÈ2∆ÊòBú´%‘7∂l{†Øè5C˜V:º’((R¨Ì3Bev‘–v%,Fa±πmΩPçB0gò¡W^˚ú˙˚GÅOD4πœL`?˝®å¥˚˛÷>'Îäõuô¸ûoVI|Ω&$Aóq=´ßHz¸€~∫UE≠ ãE¢ªÇ◊DöËì’8ævF¿îdßû+9Ù>(OB"˜Yd’µDfyµQ¢ß≥e"N3◊5†`Y–óí2zë"rÂÚw≠Púü¸ı1‚ˆt•)âv∆„{K3≤∑Ÿ´
BüÓ&P4zñÈjP·påãƒsÊlw˚ÚÆsˇO À®HUìo)ü>&≤hm∏ì°ˇ©f˚ór"£t,-%$pü¡|SÉŒ˘
#‚_¸LYı˚”U.ÿ…[„ûÈ€ˇÆÑÓ¶ü›vÊßù©˛Û"≥xb|ÛúŒÛ@˜‘ +ÚÜˆ3Y&K^]?â5¸8ö]‚Ú8^(K0¢p`t é•‚<ˆj8-im»≤òMÁøÃ£ÈÖÍâX »Ú÷yü1◊{)kI€ZYxFFé>)ü eÈ•=Äù¨zç}A9,_T~BÔª–û5I·ÄNπKº˝`¸‚Å;%Àùø’üÎ5ãﬂÏ'1P+R∑qaq˜A9EW
G6â˝ª•˚~	°CôT’√=KûÔeˇ~ﬂëu]««?â$£Âi£…ƒ‡◊54Ï‰èãªπÛ˘*¶˘µ¥phÃØå9À…¸àA~MöÖÜﬁî¡F§'ı5?¬jSuFVœb“(EX%˛%ÜÂJÑıÓâÕµ‡ûçQâÓI’ÔWæ¯∂ˇm Çgæ}eöR!;¨ırg¸âQùIÀ»∂π(s◊–„_¡*HdNj‡5ƒ«,XÈWÃÇSbÛü™Ê°∫|tF“áL_Ü;/f7£/ŸØEÂÑå`Ä!QË´â·‰ÆÆxg]~≥ û)Œ~=ò≠[ó…ßü!,óGj˜∫µ•dcî€÷(ùœöÔíœÂ:|V<…ˇw´‡P¯[»ﬂú©*˝ùˇ1hm7%è⁄º5·VBnO®¡p&ﬂ#ù $T`ÒÁ¬WûûﬂågØ/
-ºëh>¡˝m«âë÷ä¢)ÌEãUä#¨ﬁAm¢Ñﬁï6∫êÊ—#Y%ö
|#èíC˚†ÏŸÒÂó6¶}-«¥˚Nˇ<-oõCÂÄqr&¢˝P—ÚVVÌqGV8Û)a¡¯,€¿õ[,zïbﬁZ»ÔoOGˆ(é∆äia}ƒg]\∆wuÃÅâæ
ú`˝ú∑NÏßˆî9¶@Ylµ£∫?Ó∑wE˜◊\Ùˆoi2}óÅÖran-“&‰√J ·Q]’	ﬁæ#∑Á2›x@~z~«ã,∑Á†ø∞ƒyùΩ¡ÅÕ,…‚—T0˙ü‚∫NòµR®P~á‚òPÍEﬂ‰
∆ÊÉòÚnFâäDlŒ!{∞9Ú"È˜˝/iô∆hI∏k~Jıe˝‰á¬dù‘Üg!H‘€–¶ìRÛÌúY0¨•#˛Ã»Ã-»˛˜
YØ(s=•¥Z°ÒTÓ}ÚCsãfùßú∫âDÒTMúÍ …fC˜)M<ß7ª£’ ^9‹ÿØ±oUëÙ plÓﬁø¬{ˆÙö·õÜ^Öº%à>:*ó$˜V∏=¬«Uπ/uV[-ñÜŒåï=©X∏èDÉ5Z0€∏TÌï òzç◊	cyN,I"c”Âk≠	Í;ÄÊ⁄¡ eµMÚ(@œDòÿâﬂõ›ï/‹ Øö+¶ì∞$ÚÎEÅ¯«”*.]‚„RÓœ2R´ÁQÿA≤	7óÿ˜¡`ÒEC*jËB!"5å7h≠ ˇÜf∏≥ru˙Å·okxù¢∏Ç≠ãCQòﬁ˛rª¨k}”4bR⁄¢ë»©¯O*v¯©Êt‹R¥\Êù›√7	5_e9“ŒØ@Üv◊Ç$m°á∂ˆ7∑Œüëóo¡tÿ]Í∫	Rum®!DﬁÒF9 q>Çÿ©¿BÖ˜W£ZiO ‚DU{å ü%≠7ø®T]á-`º d0§uÎË.AÖcß· BèÆH€ `ûöS	–Ç3Yiÿ®oæ≠‘∫mYÈê??‚“Ω´Oç*0µ˝™˙¢#π_¯ò„„¶˝´¸tB˚¬b w¿£Ô„⁄ù öåç∆‰dÀŸBU!ö`FiÂhÕÖŸ›.‘ëèLcµ§qÒóó˜)©ê‚∞nö—.§ã¡[Óñá¡T?ZmçØP∑1—]ãä:I1€bA»∂âADPÉzó˙öä\¶¢~÷zñ¨–≥z?¿V®k∏ÿ>äôΩ~;m3n§ÿÂ‰Ω.c!!üq„o9ä˜úîLQ˚ˆù»u®Vó+Z«cY\≤Ωÿ(ÉΩÈqƒå¢„sªXÑI]do¸Ω¢Y	r*ua#°÷\Êm˜≠ƒêK»MÃóÊ$k0/É∆·;ÕIâÎGd%¸œ◊À˚Xﬁ¸!√P™zæÙπ‘∏äB∑d§òB¶ˇ∂d˛)gç	{ß£ë±	+·È2#ŸÓ‚ãı,æá¿Âø’k¸QB@:¢_N≠	[{îÊNN'+’ƒ“~&D=¬Q5O˝∞T˚ˇ	|ü(á’Áµ$Mmmª‚:vB~y˝ïˇ‹Êæ$æ´aDa§Pÿt{VùÍÆQ0óóaÌëWƒj∏E‹3U'k‰«˜ò}©Îãx qb€ò)s!:gú‹ëe ?«ïèó8·£¥)ö¥eÅ”Eß’£|ûÜ]≥zëCç7ó{ =ﬁôàez‰k]ΩÌ<WÜû›ßìc‰·dxV¥w»ÇXŒn_∆˝W±A⁄…¥ï+K∑ƒæ•CÙË¶‚ΩæÌúA¸O 3GŸµÙB!WA6†æYëÍˆÃ Eú·N5√T}Ÿ˛‡^Ìª¡f!Î°z≤®∂;n‡∑»ÂãÒƒ§Sü«»àò«ñ%ùã"õSÌ+	â|ÔLT⁄Qi_V˙îMÛ9Â»{Y·o˙Q( 8wùLÌìáV*H∑D$-#†%UÃ8HlB0&≈k#g;}FpC«ìN}ÆEäfÖ…—·ÌÀà÷ﬂÄä„k·*yÀ@$ˇ§«–”Mct—Nñã5ãÆ÷´miè˘ïﬂÆZ'	œµbˇó’Dk7UA3"*¸S’˙ú‹ì?m8d≤ØëµK¸/ª˘˘¬ <->’œˆ”˝yR™¶X±–1Gó„©ße|ÌŒs$BOÃÍ7
 ¶‚∂‹ˆFòLÖ˙…£≈ÛC]≠qëì¿ñ[Ëﬂ)›ƒL§ πÒG¿˝*cú+[Dù¶í˚ﬁ∞çÖÛé+0?1SZ•gHX∆˛˙√ﬂ‚h8[B#BØüò U!iˇiî!œ˘ xÚÚ,öZ–À=XÑMm0ûH•˜k◊P⁄V·ì»“µ=)â÷2Ù°s‘ˇÅ{´£yÈÓä”uNëü∆}@≤fÂg "›¶äéM≠¥æ8ﬂQ0{t∞ù=®Y®U8◊=sn^Ôl≥O≥Ò™≥2ˆ*‡Ñ“ É¸x<Buu;hQmåπﬂYŸ!P≤xBã√¯‹¸„4·%r7]∫¢^W!Ó<IAóY–u…IA‰ı∂r®√ß
«o#πùaMiº£20vêÄ≤ŸñC¡Àçé[˝ï«t‡÷È“ÍG8°u˙^
Ê‰«‹ñ˛Y;≠¯§¢Ã≤Üe'Úrá$óÜı‹c?gíAP™å°íCü~¨#Ò√±ó’˚¨Î∏2f(”’b˝ÆY†‰t7B™Lß]09˛«l⁄∂ãπ◊,r+ﬂ˛◊çù=≤=å≈–jßj}¬˝Ë°h±vÍEw∏^Hj©‘Œñ˚‚PvüØ£w$ë˙ÁbñÈúÁ÷lï+‘˝ia~jﬁàY.•°»©ôõPŒßÔ“Ë±Å/xW®*êî3B„Sgd≤l:…TB÷·¸!FÉV˛Í_Ë±ëô:ª∑WéWx´,–Êr·Õ®ùÛç›}»9¢®fÔàïË5ä2ú∆Z€Íâeñ°bê]πq#á-åhﬂçœ]i‘è±‡¸≈ZˆÆΩHta‡kdÔ*≠ÎkÆKäÛæVÓÇCØlÿˇl„ù%é9Ω ím%∑QÇozå3a]◊~+/+(\tûõ&˚˙`#∂î˜ÃêK-”ﬁe¬≈˜KË–‡Á—óûÏπ∂!ÌlÊ∏9Gﬁ’êÎó¬≠&£rñ«ÊçûîÃ‡Ï+/úN›ÆrÉÜ∑27B‚¿º}1ΩÀ©5€òπMór'™ÿ€Cºo_EVàå«$Ö>?gã3ºD`Õ;ı|ß_›Ú$öM∑eeXySm÷bÕÜÂáÚÀhxÍçœgPÀÿéæáñÅÀ≈ÚdmYqHhê≈ıéu˙6{Íˆ)/˛˚ΩÎÀ¨¬ÜAÇÇ˘¶Y%}ﬂ&¥æ<?˘∂ÄP`÷W√œ/ÀI≥aËƒË!=ò4)rbÛ”e∑thzCˇQï9òÿH47˜Àw/I1ìÏãÖwÔy“—‚ΩÔ#~Ò`°—J\^Bû@Q%JI6ˇjxÊD»4gb·E⁄ÁÓ∂ÕßY2IπûJÑêàzÁ sÉ+ılÌ0”9‰Ò
‡ﬂîzÀïeñ‹˜øîƒ©∆Ij«ŸÈ´@Ki-9ˆíPYN®›YÜ0NÜﬂÉ•âAàa>†Å≈.iõBùCM‡°„5lîŒ”‚¥"
Ïy√ì8‹¶’ìíìjÀS
·fΩ;úÉo‘RêÙOœ^∏n?ñ[,)Éº{æÉ¸Åƒ[3S≈‘<?Ü´¥ÉÌé (aﬂ‚œY7‰>˛
ˇj<– 1©óÂqÁΩâo\.PÍÃ<ÑW2zä\°	ge™hÔ¶l¯É‡ˇ∑´µÆ¸# &•/˚#˙x\˚Ú‡sêòF‡On¡h"˝˙Â∏¨%ÑÙáElöÄü?Ò9k«yi;5ˇÑ§•>s3ˇ Êzƒ›•¸Z∑JëjqìË¶≈=n’C⁄•¯Ú≥Qz∑OπÒõÍ+Ä√√2st“T¯ÛH÷ÏŸ^m-è±Â6‘í¬µ°=ä≤ ∞å<Z$íqÌ{7“ºïˇZ¢ÈØÊE>ªT1H˘˝{yø{•ô(‘EÙ!#√
DKÖåÃGµD˜6]∞NÅ˘—N[π„PÕ”º4#c∂x~8ì‹Óˆ]\Fõ{œãê=4ìk	ÒS˝ŒJèzEIíÜX}™pf¸Ó?˛Í—‚^ÌíO ⁄;	
Jg$Ö¥ÔãØ’†˘x± ◊Úèﬁ¥ÄP¡'‹{º.ì>î}™9¨Ò⁄˙Zç5Î1ON>Û¨&Ü "∑.‚'6y;d¥w[‡ç0…rjø™ÛŒ.-ÅvÔ)ºâQ_†˜í≠ãàÒau¨„ @–Ïò£z'éÏ¨ ñb6U†∏>$3Ë4±˜>£,¶÷,.ÉëÌ;0†O/Nx∏J¿zÒXIiá≠ÑÌå;)Ô“|À©Q3óÎ‹îf˚Ïg√≈lFÇ/§2X≠t’;{(|ªM‹∂÷=¸E.C5·Q≤˛·§^Ë”_IePßßigã‘`Í8R‹µe¢π£eßmd®ú–ïR"ÄÃl,^÷'ó∏î}[¢≥Y’pÊ·≠Ka£∑FÓ›≈
Å4◊µd$U/„W«€¯w¬ ‰∂b¶4∆ÔhÂΩÉàLWÙ•$ªqt‘q”Â™W`©˘N 2ªåñ´0Y3Ãq§∏Ó©˝”fU”#Iû’ázYvZëÔR€¯u9M≥úä≈∆È#˙Â€ÿ∂ø6Æ(‰•∫GÒ€#I9p6f_<U‹zÃ˙æ1À≈⁄GH™˚›zìO¥Ñ2V@jŸ