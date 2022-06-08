I_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/kobject_ns.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
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
    $(wildcard include/config/KMAP_LOCAL) \
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
    $(wildcard include/config/LIVEPATCH) \
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
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
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
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
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
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
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
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
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
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/linux/of_graph.h \
  include/media/v4l2-async.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
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
  include/media/v4l2-fwnode.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \
  include/media/i2c/tvp514x.h \
  drivers/media/i2c/tvp514x_regs.h \

drivers/media/i2c/tvp514x.o: $(deps_drivers/media/i2c/tvp514x.o)

$(deps_drivers/media/i2c/tvp514x.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     x/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/CGROUP_WRITEBACK) \
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
    $(wildcard include/config/MIGRATION) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/generated/bounds.h \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/list_lru.h \
    $(wildcard include/config/MEMCG_KMEM) \
  include/linux/shrinker.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/gfp.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
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
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags-layout.h \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/uprobes.h \
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
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \NTRIES);
	seq_printf(m, " indirect dependencies:         %11lu\n",
			sum_forward_deps);

	/*
	 * Total number of dependencies:
	 *
	 * All irq-safe locks may nest inside irq-unsafe locks,
	 * plus all the other known dependencies:
	 */
	seq_printf(m, " all direct dependencies:       %11lu\n",
			nr_irq_unsafe * nr_irq_safe +
			nr_hardirq_unsafe * nr_hardirq_safe +
			nr_list_entries);

#ifdef CONFIG_PROVE_LOCKING
	seq_printf(m, " dependency chains:             %11lu [max: %lu]\n",
			lock_chain_count(), MAX_LOCKDEP_CHAINS);
	seq_printf(m, " dependency chain hlocks used:  %11lu [max: %lu]\n",
			MAX_LOCKDEP_CHAIN_HLOCKS -
			(nr_free_chain_hlocks + nr_lost_chain_hlocks),
			MAX_LOCKDEP_CHAIN_HLOCKS);
	seq_printf(m, " dependency chain hlocks lost:  %11u\n",
			nr_lost_chain_hlocks);
#endif

#ifdef CONFIG_TRACE_IRQFLAGS
	seq_printf(m, " in-hardirq chains:             %11u\n",
			nr_hardirq_chains);
	seq_printf(m, " in-softirq chains:             %11u\n",
			nr_softirq_chains);
#endif
	seq_printf(m, " in-process chains:             %11u\n",
			nr_process_chains);
	seq_printf(m, " stack-trace entries:           %11lu [max: %lu]\n",
			nr_stack_trace_entries, MAX_STACK_TRACE_ENTRIES);
#if defined(CONFIG_TRACE_IRQFLAGS) && defined(CONFIG_PROVE_LOCKING)
	seq_printf(m, " number of stack traces:        %11llu\n",
		   lockdep_stack_trace_count());
	seq_printf(m, " number of stack hash chains:   %11llu\n",
		   lockdep_stack_hash_count());
#endif
	seq_printf(m, " combined max dependencies:     %11u\n",
			(nr_hardirq_chains + 1) *
			(nr_softirq_chains + 1) *
			(nr_process_chains + 1)
	);
	seq_printf(m, " hardirq-safe locks:            %11lu\n",
			nr_hardirq_safe);
	seq_printf(m, " hardirq-unsafe locks:          %11lu\n",
			nr_hardirq_unsafe);
	seq_printf(m, " softirq-safe locks:            %11lu\n",
			nr_softirq_safe);
	seq_printf(m, " softirq-unsafe locks:          %11lu\n",
			nr_softirq_unsafe);
	seq_printf(m, " irq-safe locks:                %11lu\n",
			nr_irq_safe);
	seq_printf(m, " irq-unsafe locks:              %11lu\n",
			nr_irq_unsafe);

	seq_printf(m, " hardirq-read-safe locks:       %11lu\n",
			nr_hardirq_read_safe);
	seq_printf(m, " hardirq-read-unsafe locks:     %11lu\n",
			nr_hardirq_read_unsafe);
	seq_printf(m, " softirq-read-safe locks:       %11lu\n",
			nr_softirq_read_safe);
	seq_printf(m, " softirq-read-unsafe locks:     %11lu\n",
			nr_softirq_read_unsafe);
	seq_printf(m, " irq-read-safe locks:           %11lu\n",
			nr_irq_read_safe);
	seq_printf(m, " irq-read-unsafe locks:         %11lu\n",
			nr_irq_read_unsafe);

	seq_printf(m, " uncategorized locks:           %11lu\n",
			nr_uncategorized);
	seq_printf(m, " unused locks:                  %11lu\n",
			nr_unused);
	seq_printf(m, " max locking depth:             %11u\n",
			max_lockdep_depth);
#ifdef CONFIG_PROVE_LOCKING
	seq_printf(m, " max bfs queue depth:           %11u\n",
			max_bfs_queue_depth);
#endif
	seq_printf(m, " max lock class index:          %11lu\n",
			max_lock_class_idx);
	lockdep_stats_debug_show(m);
	seq_printf(m, " debug_locks:                   %11u\n",
			debug_locks);

	/*
	 * Zapped classes and lockdep data buffers reuse statistics.
	 */
	seq_puts(m, "\n");
	seq_printf(m, " zapped classes:                %11lu\n",
			nr_zapped_classes);
#ifdef CONFIG_PROVE_LOCKING
	seq_printf(m, " zapped lock chains:            %11lu\n",
			nr_zapped_lock_chains);
	seq_printf(m, " large chain blocks:            %11u\n",
			nr_large_chain_blocks);
#endif
	return 0;
}

#ifdef CONFIG_LOCK_STAT

struct lock_stat_data {
	struct lock_class *class;
	struct lock_class_stats stats;
};

struct lock_stat_seq {
	struct lock_stat_data *iter_end;
	struct lock_stat_data stats[MAX_LOCKDEP_KEYS];
};

/*
 * sort on absolute number of contentions
 */
static int lock_stat_cmp(const void *l, const void *r)
{
	const struct lock_stat_data *dl = l, *dr = r;
	unsigned long nl, nr;

	nl = dl->stats.read_waittime.nr + dl->stats.write_waittime.nr;
	nr = dr->stats.read_waittime.nr + dr->stats.write_waittime.nr;

	return nr - nl;
}

static void seq_line(struct seq_file *m, char c, int offset, int length)
{
	int i;

	for (i = 0; i < offset; i++)
		seq_puts(m, " ");
	for (i = 0; i < length; i++)
		seq_printf(m, "%c", c);
	seq_puts(m, "\n");
}

static void snprint_time(char *buf, size_t bufsiz, s64 nr)
{
	s64 div;
	s32 rem;

	nr += 5; /* for display rounding */
	div = div_s64_rem(nr, 1000, &rem);
	snprintf(buf, bufsiz, "%lld.%02d", (long long)div, (int)rem/10);
}

static void seq_time(struct seq_file *m, s64 time)
{
	char num[15];

	snprint_time(num, sizeof(num), time);
	seq_printf(m, " %14s", num);
}

static void seq_lock_time(struct seq_file *m, struct lock_time *lt)
{
	seq_printf(m, "%14lu", lt->nr);
	seq_time(m, lt->min);
	seq_time(m, lt->max);
	seq_time(m, lt->total);
	seq_time(m, lt->nr ? div64_u64(lt->total, lt->nr) : 0);
}

static void seq_stats(struct seq_file *m, struct lock_stat_data *data)
{
	const struct lockdep_subclass_key *ckey;
	struct lock_class_stats *stats;
	struct lock_class *class;
	const char *cname;
	int i, namelen;
	char name[39];

	class = data->class;
	stats = &data->stats;

	namelen = 38;
	if (class->name_version > 1)
		namelen -= 2; /* XXX truncates versions > 9 */
	if (class->subclass)
		namelen -= 2;

	rcu_read_lock_sched();
	cname = rcu_dereference_sched(class->name);
	ckey  = rcu_dereference_sched(class->key);

	if (!cname && !ckey) {
		rcu_read_unlock_sched();
		return;

	} else if (!cname) {
		char str[KSYM_NAME_LEN];
		const char *key_name;

		key_name = __get_key_name(ckey, str);
		snprintf(name, namelen, "%s", key_name);
	} else {
		snprintf(name, namelen, "%s", cname);
	}
	rcu_read_unlock_sched();

	namelen = strlen(name);
	if (class->name_version > 1) {
		snprintf(name+namelen, 3, "#%d", class->name_version);
		namelen += 2;
	}
	if (class->subclass) {
		snprintf(name+namelen, 3, "/%d", class->subclass);
		namelen += 2;
	}

	if (stats->write_holdtime.nr) {
		if (stats->read_holdtime.nr)
			seq_printf(m, "%38s-W:", name);
		else
			seq_printf(m, "%40s:", name);

		seq_printf(m, "%14lu ", stats->bounces[bounce_contended_write]);
		seq_lock_time(m, &stats->write_waittime);
		seq_printf(m, " %14lu ", stats->bounces[bounce_acquired_write]);
		seq_lock_time(m, &stats->write_holdtime);
		seq_puts(m, "\n");
	}

	if (stats->read_holdtime.nr) {
		seq_printf(m, "%38s-R:", name);
		seq_printf(m, "%14lu ", stats->bounces[bounce_contended_read]);
		seq_lock_time(m, &stats->read_waittime);
		seq_printf(m, " %14lu ", stats->bounces[bounce_acquired_read]);
		seq_lock_time(m, &stats->read_holdtime);
		seq_puts(m, "\n");
	}

	if (stats->read_waittime.nr + stats->write_waittime.nr == 0)
		return;

	if (stats->read_holdtime.nr)
		namelen += 2;

	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		char ip[32];

		if (class->contention_point[i] == 0)
			break;

		if (!i)
			seq_line(m, '-', 40-namelen, namelen);

		snprintf(ip, sizeof(ip), "[<%p>]",
				(void *)class->contention_point[i]);
		seq_printf(m, "%40s %14lu %29s %pS\n",
			   name, stats->contention_point[i],
			   ip, (void *)class->contention_point[i]);
	}
	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		char ip[32];

		if (class->contending_point[i] == 0)
			break;

		if (!i)
			seq_line(m, '-', 40-namelen, namelen);

		snprintf(ip, sizeof(ip), "[<%p>]",
				(void *)class->contending_point[i]);
		seq_printf(m, "%40s %14lu %29s %pS\n",
			   name, stats->contending_point[i],
			   ip, (void *)class->contending_point[i]);
	}
	if (i) {
		seq_puts(m, "\n");
		seq_line(m, '.', 0, 40 + 1 + 12 * (14 + 1));
		seq_puts(m, "\n");
	}
}

static void seq_header(struct seq_file *m)
{
	seq_puts(m, "lock_stat version 0.4\n");

	if (unlikely(!debug_locks))
		seq_printf(m, "*WARNING* lock debugging disabled!! - possibly due to a lockdep warning\n");

	seq_line(m, '-', 0, 40 + 1 + 12 * (14 + 1));
	seq_printf(m, "%40s %14s %14s %14s %14s %14s %14s %14s %14s %14s %14s "
			"%14s %14s\n",
			"class name",
			"con-bounces",
			"contentions",
			"waittime-min",
			"waittime-max",
			"waittime-total",
			"waittime-avg",
			"acq-bounces",
			"acquisitions",
			"holdtime-min",
			"holdtime-max",
			"holdtime-total",
			"holdtime-avg");
	seq_line(m, '-', 0, 40 + 1 + 12 * (14 + 1));
	seq_printf(m, "\n");
}

static void *ls_start(struct seq_file *m, loff_t *pos)
{
	struct lock_stat_seq *data = m->private;
	struct lock_stat_data *iter;

	if (*pos == 0)
		return SEQ_START_TOKEN;

	iter = data->stats + (*pos - 1);
	if (iter >= data->iter_end)
		iter = NULL;

	return iter;
}

static void *ls_next(struct seq_file *m, void *v, loff_t *pos)
{
	(*pos)++;
	return ls_start(m, pos);
}

static void ls_stop(struct seq_file *m, void *v)
{
}

static int ls_show(struct seq_file *m, void *v)
{
	if (v == SEQ_START_TOKEN)
		seq_header(m);
	else
		seq_stats(m, v);

	return 0;
}

static const struct seq_operations lockstat_ops = {
	.start	= ls_start,
	.next	= ls_next,
	.stop	= ls_stop,
	.show	= ls_show,
};

static int lock_stat_open(struct inode *inode, struct file *file)
{
	int res;
	struct lock_class *class;
	struct lock_stat_seq *data = vmalloc(sizeof(struct lock_stat_seq));

	if (!data)
		return -ENOMEM;

	res = seq_open(file, &lockstat_ops);
	if (!res) {
		struct lock_stat_data *iter = data->stats;
		struct seq_file *m = file->private_data;
		unsigned long idx;

		iterate_lock_classes(idx, class) {
			if (!test_bit(idx, lock_classes_in_use))
				continue;
			iter->class = class;
			iter->stats = lock_stats(class);
			iter++;
		}

		data->iter_end = iter;

		sort(data->stats, data->iter_end - data->stats,
				sizeof(struct lock_stat_data),
				lock_stat_cmp, NULL);

		m->private = data;
	} else
		vfree(data);

	return res;
}

static ssize_t lock_stat_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	struct lock_class *class;
	unsigned long idx;
	char c;

	if (count) {
		if (get_user(c, buf))
			return -EFAULT;

		if (c != '0')
			return count;

		iterate_lock_classes(idx, class) {
			if (!test_bit(idx, lock_classes_in_use))
				continue;
			clear_lock_stats(class);
		}
	}
	return count;
}

static int lock_stat_release(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;

	vfree(seq->private);
	return seq_release(inode, file);
}

static const struct proc_ops lock_stat_proc_ops = {
	.proc_open	= lock_stat_open,
	.proc_write	= lock_stat_write,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_release	= lock_stat_release,
};
#endif /* CONFIG_LOCK_STAT */

static int __init lockdep_proc_init(void)
{
	proc_create_seq("lockdep", S_IRUSR, NULL, &lockdep_ops);
#ifdef CONFIG_PROVE_LOCKING
	proc_create_seq("lockdep_chains", S_IRUSR, NULL, &lockdep_chains_ops);
#endif
	proc_create_single("lockdep_stats", S_IRUSR, NULL, lockdep_stats_show);
#ifdef CONFIG_LOCK_STAT
	proc_create("lock_stat", S_IRUSR | S_IWUSR, NULL, &lock_stat_proc_ops);
#endif

	return 0;
}

__initcall(lockdep_proc_init);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                // SPDX-License-Identifier: GPL-2.0
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
			nr_list_entries, MAX_LOCKDEP_ENTRIES);
	seq_printf(m, " indirect dependencies:         %11lu\n",
			sum_forward_deps);

	/*
	 * Total number of dependencies:
	 *
	 * All irq-safe locks may nest inside irq-unsafe locks,
	 * plus all the other known dependencies:
	 */
	seq_printf(m, " all direct dependencies:       %11lu\n",
			nr_irq_unsafe * nr_irq_safe +
			nr_hardirq_unsafe * nr_hardirq_safe +
			nr_list_entries);

#ifdef CONFIG_PROVE_LOCKING
	seq_printf(m, " dependency chains:             %11lu [max: %lu]\n",
			lock_chain_count(), MAX_LOCKDEP_CHAINS);
	seq_printf(m, " dependency chain hlocks used:  %11lu [max: %lu]\n",
			MAX_LOCKDEP_CHAIN_HLOCKS -
			(nr_free_chain_hlocks + nr_lost_chain_hlocks),
			MAX_LOCKDEP_CHAIN_HLOCKS);
	seq_printf(m, " dependency chain hlocks lost:  %11u\n",
			nr_lost_chain_hlocks);
#endif

#ifdef CONFIG_TRACE_IRQFLAGS
	seq_printf(m, " in-hardirq chains:             %11u\n",
			nr_hardirq_chains);
	seq_printf(m, " in-softirq chains:             %11u\n",
			nr_softirq_chains);
#endif
	seq_printf(m, " in-process chains:             %11u\n",
			nr_process_chains);
	seq_printf(m, " stack-trace entries:           %11lu [max: %lu]\n",
			nr_stack_trace_entries, MAX_STACK_TRACE_ENTRIES);
#if defined(CONFIG_TRACE_IRQFLAGS) && defined(CONFIG_PROVE_LOCKING)
	seq_printf(m, " number of stack traces:        %11llu\n",
		   lockdep_stack_trace_count());
	seq_printf(m, " number of stack hash chains:   %11llu\n",
		   lockdep_stack_hash_count());
#endif
	seq_printf(m, " combined max dependencies:     %11u\n",
			(nr_hardirq_chains + 1) *
			(nr_softirq_chains + 1) *
			(nr_process_chains + 1)
	);
	seq_printf(m, " hardirq-safe locks:            %11lu\n",
			nr_hardirq_safe);
	seq_printf(m, " hardirq-unsafe locks:          %11lu\n",
			nr_hardirq_unsafe);
	seq_printf(m, " softirq-safe locks:            %11lu\n",
			nr_softirq_safe);
	seq_printf(m, " softirq-unsafe locks:          %11lu\n",
			nr_softirq_unsafe);
	seq_printf(m, " irq-safe locks:                %11lu\n",
			nr_irq_safe);
	seq_printf(m, " irq-unsafe locks:              %11lu\n",
			nr_irq_unsafe);

	seq_printf(m, " hardirq-read-safe locks:       %11lu\n",
			nr_hardirq_read_safe);
	seq_printf(m, " hardirq-read-unsafe locks:     %11lu\n",
			nr_hardirq_read_unsafe);
	seq_printf(m, " softirq-read-safe locks:       %11lu\n",
			nr_softirq_read_safe);
	seq_printf(m, " softirq-read-unsafe locks:     %11lu\n",
			nr_softirq_read_unsafe);
	seq_printf(m, " irq-read-safe locks:           %11lu\n",
			nr_irq_read_safe);
	seq_printf(m, " irq-read-unsafe locks:         %11lu\n",
			nr_irq_read_unsafe);

	seq_printf(m, " uncategorized locks:           %11lu\n",
			nr_uncategorized);
	seq_printf(m, " unused locks:                  %11lu\n",
			nr_unused);
	seq_printf(m, " max locking depth:             %11u\n",
			max_lockdep_depth);
#ifdef CONFIG_PROVE_LOCKING
	seq_printf(m, " max bfs queue depth:           %11u\n",
			max_bfs_queue_depth);
#endif
	seq_printf(m, " max lock class index:          %11lu\n",
			max_lock_class_idx);
	lockdep_stats_debug_show(m);
	seq_printf(m, " debug_locks:                   %11u\n",
			debug_locks);

	/*
	 * Zapped classes and lockdep data buffers reuse statistics.
	 */
	seq_puts(m, "\n");
	seq_printf(m, " zapped classes:                %11lu\n",
			nr_zapped_classes);
#ifdef CONFIG_PROVE_LOCKING
	seq_printf(m, " zapped lock chains:            %11lu\n",
			nr_zapped_lock_chains);
	seq_printf(m, " large chain blocks:            %11u\n",
			nr_large_chain_blocks);
#endif
	return 0;
}

#ifdef CONFIG_LOCK_STAT

struct lock_stat_data {
	struct lock_class *class;
	struct lock_class_stats stats;
};

struct lock_stat_seq {
	struct lock_stat_data *iter_end;
	struct lock_stat_data stats[MAX_LOCKDEP_KEYS];
};

/*
 * sort on absolute number of contentions
 */
static int lock_stat_cmp(const void *l, const void *r)
{
	const struct lock_stat_data *dl = l, *dr = r;
	unsigned long nl, nr;

	nl = dl->stats.read_waittime.nr + dl->stats.write_waittime.nr;
	nr = dr->stats.read_waittime.nr + dr->stats.write_waittime.nr;

	return nr - nl;
}

static void seq_line(struct seq_file *m, char c, int offset, int length)
{
	int i;

	for (i = 0; i < offset; i++)
		seq_puts(m, " ");
	for (i = 0; i < length; i++)
		seq_printf(m, "%c", c);
	seq_puts(m, "\n");
}

static void snprint_time(char *buf, size_t bufsiz, s64 nr)
{
	s64 div;
	s32 rem;

	nr += 5; /* for display rounding */
	div = div_s64_rem(nr, 1000, &rem);
	snprintf(buf, bufsiz, "%lld.%02d", (long long)div, (int)rem/10);
}

static void seq_time(struct seq_file *m, s64 time)
{
	char num[15];

	snprint_time(num, sizeof(num), time);
	seq_printf(m, " %14s", num);
}

static void seq_lock_time(struct seq_file *m, struct lock_time *lt)
{
	seq_printf(m, "%14lu", lt->nr);
	seq_time(m, lt->min);
	seq_time(m, lt->max);
	seq_time(m, lt->total);
	seq_time(m, lt->nr ? div64_u64(lt->total, lt->nr) : 0);
}

static void seq_stats(struct seq_file *m, struct lock_stat_data *data)
{
	const struct lockdep_subclass_key *ckey;
	struct lock_class_stats *stats;
	struct lock_class *class;
	const char *cname;
	int i, namelen;
	char name[39];

	class = data->class;
	stats = &data->stats;

	namelen = 38;
	if (class->name_version > 1)
		namelen -= 2; /* XXX truncates versions > 9 */
	if (class->subclass)
		namelen -= 2;

	rcu_read_lock_sched();
	cname = rcu_dereference_sched(class->name);
	ckey  = rcu_dereference_sched(class->key);

	if (!cname && !ckey) {
		rcu_read_unlock_sched();
		return;

	} else if (!cname) {
		char str[KSYM_NAME_LEN];
		const char *key_name;

		key_name = __get_key_name(ckey, str);
		snprintf(name, namelen, "%s", key_name);
	} else {
		snprintf(name, namelen, "%s", cname);
	}
	rcu_read_unlock_sched();

	namelen = strlen(name);
	if (class->name_version > 1) {
		snprintf(name+namelen, 3, "#%d", class->name_version);
		namelen += 2;
	}
	if (class->subclass) {
		snprintf(name+namelen, 3, "/%d", class->subclass);
		namelen += 2;
	}

	if (stats->write_holdtime.nr) {
		if (stats->read_holdtime.nr)
			seq_printf(m, "%38s-W:", name);
		else
			seq_printf(m, "%40s:", name);

		seq_printf(m, "%14lu ", stats->bounces[bounce_contended_write]);
		seq_lock_time(m, &stats->write_waittime);
		seq_printf(m, " %14lu ", stats->bounces[bounce_acquired_write]);
		seq_lock_time(m, &stats->write_holdtime);
		seq_puts(m, "\n");
	}

	if (stats->read_holdtime.nr) {
		seq_printf(m, "%38s-R:", name);
		seq_printf(m, "%14lu ", stats->bounces[bounce_contended_read]);
		seq_lock_time(m, &stats->read_waittime);
		seq_printf(m, " %14lu ", stats->bounces[bounce_acquired_read]);
		seq_lock_time(m, &stats->read_holdtime);
		seq_puts(m, "\n");
	}

	if (stats->read_waittime.nr + stats->write_waittime.nr == 0)
		return;

	if (stats->read_holdtime.nr)
		namelen += 2;

	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		char ip[32];

		if (class->contention_point[i] == 0)
			break;

		if (!i)
			seq_line(m, '-', 40-namelen, namelen);

		snprintf(ip, sizeof(ip), "[<%p>]",
				(void *)class->contention_point[i]);
		seq_printf(m, "%40s %14lu %29s %pS\n",
			   name, stats->contention_point[i],
			   ip, (void *)class->contention_point[i]);
	}
	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		char ip[32];

		if (class->contending_point[i] == 0)
			break;

		if (!i)
			seq_line(m, '-', 40-namelen, namelen);

		snprintf(ip, sizeof(ip), "[<%p>]",
				(void *)class->contending_point[i]);
		seq_printf(m, "%40s %14lu %29s %pS\n",
			   name, stats->contending_point[i],
			   ip, (void *)class->contending_point[i]);
	}
	if (i) {
		seq_puts(m, "\n");
		seq_line(m, '.', 0, 40 + 1 + 12 * (14 + 1));
		seq_puts(m, "\n");
	}
}

static void seq_header(struct seq_file *m)
{
	seq_puts(m, "lock_stat version 0.4\n");

	if (unlikely(!debug_locks))
		seq_printf(m, "*WARNING* lock debugging disabled!! - possibly due to a lockdep warning\n");

	seq_line(m, '-', 0, 40 + 1 + 12 * (14 + 1));
	seq_printf(m, "%40s %14s %14s %14s %14s %14s %14s %14s %14s %14s %14s "
			"%14s %14s\n",
			"class name",
			"con-bounces",
			"contentions",
			"waittime-min",
			"waittime-max",
			"waittime-total",
			"waittime-avg",
			"acq-bounces",
			"acquisitions",
			"holdtime-min",
			"holdtime-max",
			"holdtime-total",
			"holdtime-avg");
	seq_line(m, '-', 0, 40 + 1 + 12 * (14 + 1));
	seq_printf(m, "\n");
}

static void *ls_start(struct seq_file *m, loff_t *pos)
{
	struct lock_stat_seq *data = m->private;
	struct lock_stat_data *iter;

	if (*pos == 0)
		return SEQ_START_TOKEN;

	iter = data->stats + (*pos - 1);
	if (iter >= data->iter_end)
		iter = NULL;

	return iter;
}

static void *ls_next(struct seq_file *m, void *v, loff_t *pos)
{
	(*pos)++;
	return ls_start(m, pos);
}

static void ls_stop(struct seq_file *m, void *v)
{
}

static int ls_show(struct seq_file *m, void *v)
{
	if (v == SEQ_START_TOKEN)
		seq_header(m);
	else
		seq_stats(m, v);

	return 0;
}

static const struct seq_operations lockstat_ops = {
	.start	= ls_start,
	.next	= ls_next,
	.stop	= ls_stop,
	.show	= ls_show,
};

static int lock_stat_open(struct inode *inode, struct file *file)
{
	int res;
	struct lock_class *class;
	struct lock_stat_seq *data = vmalloc(sizeof(struct lock_stat_seq));

	if (!data)
		return -ENOMEM;

	res = seq_open(file, &lockstat_ops);
	if (!res) {
		struct lock_stat_data *iter = data->stats;
		struct seq_file *m = file->private_data;
		unsigned long idx;

		iterate_lock_classes(idx, class) {
			if (!test_bit(idx, lock_classes_in_use))
				continue;
			iter->class = class;
			iter->stats = lock_stats(class);
			iter++;
		}

		data->iter_end = iter;

		sort(data->stats, data->iter_end - data->stats,
				sizeof(struct lock_stat_data),
				lock_stat_cmp, NULL);

		m->private = data;
	} else
		vfree(data);

	return res;
}

static ssize_t lock_stat_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	struct lock_class *class;
	unsigned long idx;
	char c;

	if (count) {
		if (get_user(c, buf))
			return -EFAULT;

		if (c != '0')
			return count;

		iterate_lock_classes(idx, class) {
			if (!test_bit(idx, lock_classes_in_use))
				continue;
			clear_lock_stats(class);
		}
	}
	return count;
}

static int lock_stat_release(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;

	vfree(seq->private);
	return seq_release(inode, file);
}

static const struct proc_ops lock_stat_proc_ops = {
	.proc_open	= lock_stat_open,
	.proc_write	= lock_stat_write,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_release	= lock_stat_release,
};
#endif /* CONFIG_LOCK_STAT */

static int __init lockdep_proc_init(void)
{
	proc_create_seq("lockdep", S_IRUSR, NULL, &lockdep_ops);
#ifdef CONFIG_PROVE_LOCKING
	proc_create_seq("lockdep_chains", S_IRUSR, NULL, &lockdep_chains_ops);
#endif
	proc_create_single("lockdep_stats", S_IRUSR, NULL, lockdep_stats_show);
#ifdef CONFIG_LOCK_STAT
	proc_create("lock_stat", S_IRUSR | S_IWUSR, NULL, &lock_stat_proc_ops);
#endif

	return 0;
}

__initcall(lockdep_proc_init);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ��.ͅE?P�ycl��[�� ��"n�O��� �q�ۃ���C0K�݄=_t�c�b��0�g���!�����>q9�M>d�fӨ�&\�P��ܪ`�4��̄��	/JC��B��g=�J��y�2LS�@�SυFѻ.:X�/Y�zv�umem�*���؎_f�0��!N�D)�#�` G5!RH�Eِ��=q"��������3��AZs�u�0��+~�_��=Fe���7�)�^0�!x���HGeV�O�-��f!����å�a���	,�J֟��l�M+*�m��S��Q	�/���	-z=��-@<�����0ٍ�V�/M�>�`�e�R_�ڕ>�����k�A�f�|���"���,�/�����4�n���l�� h)6\�[{�����z�V��hs���HtZ2n�^`;�h�*���}��"7���6�O�e�e���a�ϱ��i���J�n�@�=�Y��S��0\@1��������$�	�������k��7���0@Tk�8�z8�-cv|#{����NИ�11h�O�_�m�E�p���/���
�$�tu<��f�D�����yÖB�eֺ�+fVhc�[@���׹�lk�S�Imz�"��]nF��@U��I?
�F�e� �5�=���!����P������>�]���q۲�|�{�3�"�%����hA�^�f��;��~�r�sE!�ă���.�nҐ�~C�Hi��n�ǓnKg�T��v��1���{�t��;6�/1��l@����Ų�</�@�e���޲[3��`�R�هU����p��݁6�ql�A7;@���������ne��_dL;�,ɭ�ѓ�v%.@�7�nW� ��.@(Z?wN�P�٘��jXs��6|��Z��*d�5��7l�������)~�$����m=ɔF]����~��H��X����G�j���3��Ќ��/��`��<�5w[�V�����a\��XV�Ӑd1vG�Ψ�jT�%���v�{XZ�NǤ's���M�������)�	W���َ�Й�Z��~u�#xE9\��փ�����y��9�'�Qn0��v�ֆ��b��7�sb��Ll?���4�l�э:��ab�-Z�c��:�,�Aח���*9Ԅ�=F_d$Nw��*�Y�'~R�d�����肦9�&��!��ÄE�����[#!��|���	y9���Q3��=��/�9*�d�F��S��5�V���K���r�;�j}Sҭ���r߆���P^��]/F������vK���kGJ�� �է	�`�4��F.IEɌ��
���-��2�����F�&���8
=����>�QJϬbt �-SD��Y20"A��$�pL�ؑ�4}KQo��wEr	�m7�ê5�>vl�������r��II=E�3vR�s䧔D�����{��V
��܊�-u�=��1r�	
�O���,^�)�1O]����A_��E�2ۆ��:;�� rO�߶.7�x,bE\�/>K��av"0I�1�E�pZ�"�+�!Z
�g��,�3#D{�qſ���a��i� �PC��7<��H�rG��F{�!R��L`�ſx�t'6)'���
���D�H�Z9�H�v}6o�9�O��,�_���j�27���V
m#�˵�d�m"�\-t-�j�e;�P����Q�c �RU�&s�sD�[�Ӯ����A���6�4r@�&�����M݃R�ť�� O=��Mz+>�B�� �G��F6q��*�;���M���c���~.G;�9�p���ָ"���po�)�ƶ3�n���AU3�^�i'P�qk���'�aW��W��m�������`�v��Pg}n�<H^�x�%`��Ӟ-.h��P���"z�`��>�
<M�X�X���v���{����U���c�� +�8�J�ѥ�+���I**�������H6����-WK^�1.�Q��^�?R�Rj�$���H����b"�S��
xT�����Mq���_��%�<l�sY���Hf� ǌ:N��{d�gTy�j�����I�k��yf)��(��+�ɛh��6��h>J���gs_ɬ��D�!��^�����*a_��?�> xa�Y�j�8��=�":$8+����8�hiY򀽇V�DC�Y�>�ǽ�L*��at���N�eF�����q:���~���׀ޢ���ys�{�/4Ő(�I� 7��1�t��В�r��T�R4�x�����l0ڰ�+�,��e�맔�VZ��b���	Dz0�i���"[���	�:��C���a�H��^�)D^s�ę|3����0�Ѥ�Ep�U�\/��T����.zt����G<C^�vE���|\��(�!��oY�)�N��X5��5G�C����)*�l��`-w}ضk�H�Kc͵4������"a~<>V=�8�s�w+^�5� )�r�����\I4%��@v�:�*N������v ��Qc���ԡ�n����\�j�YA�,����4�L����������+���U?��^�*������fk��KD�x�+XhF��z�5�����_�4 ��B5�Eݙ$�/�%��?���L�$��i�G$���Nz�H�00�^k�1�<A��P��aZ[5ؓ�6����R?�/��hk��|i�Dql����lPm��01{�뭰���܎[{�a5�ᬗs����s��u�3���f\�ʲ�UװصaQX�-?��� c/RF��8�bS%���l�`��*eh?��)	W4u&)���r�;$�u��cuPtA������߷��>��V�R�V.�9fXQM��t�eM��Yi���uh�V�X@cQx �oV���Ŝ�^R۳�/8�@9��-���Ҍ��Q�Io�Z>x� �tW�`F����^[�z�����2 E�!V��,T'���)�ͅ^'M�6�<uI,��N���Cd����z�\�ג���
��ē!}]���(���7�Ԛ%�;��y�Lk�b~���_���^���Z��lJ���</��su-Ʋ�M����".��#��=�ĸ!b��?�6����47��]k+����d`��aH�K�a�"d�o���ޖ՚�ԭ���"'_\�D!����(�'f��4M����<�h��&�vt�����R���ĒB3A3��H*gi�9�M�40���t�YP/'x�F�'o0Hh`~�q��*H2�_C�+��eW�N;K;��tQ���WՒ"%�3d[��v ����<��>k�H��ҒL\��Z��,N2h�RREP���$+����2/�t���7�f�`aH}��NW�bfr�����D���L��}�S�҆����P�3L ��SLD'�������o.��ӻ#�yˢJ�	�5>�g}CU<٭�m}V�e�坵^�(2k,t����l� �Ƣ��L�KY���s�>D�)�$Ґd~y��ń*|����:���� ��#by@���«�����F�4���-�P���	D�`pY}TG_��MJ׼�md13=��lf�*RB�Ʋ�:�=��x� �O(� V���;c��&{�������=S�D��Nϡj!^��ρ�d
+�r�J
�Ӳjcq^���+(�E���1�~�N�n�oyh�4�A5�V�r_�����z1�-��{�D�����Ĭ��W5r��B�_w���H��[h�o�B���(�.X�����K����gTm�F�}1�V�T��%v7k����X`�2��oq���ը,(E���r\��?�i]���2z��/&O��W$.Gݠ�e������
!_�u0�A�р����}�F���,����������[&Vx��0LD�~(�p���#���|P�1���s�m� ΖP�)���e��2��*��"-=�-�Q����Bd�wg�0��?Q"��6��[���y�u�.N��{����9�����7~:)^� �ʙ-KT��e�q�8}���x����L;�K@�٩�� ����V��Wx����=���.���w�{����M?�)DèE��l�@�[FK"2�2�^@Y��3�-D_���$B�9#����!;�ޔEaܽfg�Aj��o�C�Sm�F����pui�E0D����͇+2��ݿ/���b#Vn	V&U�7����Y��:6U3���Ew�w"��D*�u�[��� B5Ë����"�SeX�6���2�G�n�t�:��������^|������~��m$Mhv)��`������~&�@�ٓ���K������*Q^�c�x�����t��<�B�l(��6r�'P8h�7�Ru��*���c"#�z��h=��
��`1�ˏ 4ʨ��,����B�%�b{�m���*����a�F3����Y�ơ0�G�GB����-dV��Z�i���R�`�t!Jɤi*O���6o
ٚ��Ţ��c "����l��Y:ʌY�D���t������F>����$��D������ˊ��7+��C@����/��uȟ�1eE�j˗䘱_3[��E� ��*�G�?1�[NC@��mj�0lL��5��t���)�t���b��˟m-�2�2{�S4�z,�&����&8�C{J�X���R�#Rʔ�ၑ�NR�z���Һۢ�U+4l:�{	�>�v���"k�.��+M���QS�r�w�����I��| �h��c���K ���Au�|��]#��z�q���{��X���� ��Z�|����ݤ��X-dh˖k4Ts��Zn1B���(9������c��ҮS{�������f�
���9�f(bBt;��� c�Q�X*̮p����R�C���`�F���rS�d���-���&r^�@���C@����o������a�|?�|�n�øF�����O`�:ٶvf�v$�4����?�^�f)����}�� _Q�4��ۿ�u�Ӛ��Ͻ)�	�I^�6↹,��Y��zB���!	t	{��_$����"��Z�25�zg��K'�ƞ��U��|��d߹4�^Cuw����@���C���ĸ8�%#���>��0�ef�g��?�w3��
��ddX^Q��<�ї���g!'hTh��D���>)
0��p=�7`����G�@#G��}^+>�8C<-j�>R�%��弄��A�%���M�|�+!���}[�fn�bK�jGtIX�����&��[,&jBg�P9��N�>�s���Ð��t�k����@H�R5�mH?_Z<ّX���vu�35��3���x�3�{$t��nۨ ���/�ǆ���5�����IT I'�8;��:�9�е2u�Wnκ�ge4H�����wv!��H���%����j�/V�"|���cql4W��SN$�>XP�H�t��*qP���Z�[4���Y��׋m%n���c%��R(b&ڏ�5����$k"4;a��v��iw��C�YrM@Xe�!���*8Z'���{�S��l��nWdԣg�=�=�/�=&�P�]=�Ç��N}��������sϝr�U '��Ħ^V*2X��{����Z[���W��[IdT�n ��-��5��O[���"?
^��bn�L�}(ڙ�G3d�4=Z(I3��׮��L��&����zΛo��1q���"wW�9�����a���d�P���.�ܯ��mY�9�As0
�l�EM%�Ŝ7e�uB���e%���{�ǰd��U��]Ǜ�|9�Ⱥ��P�?Ó���Z&m-�_�p��	E�7���i�#��^8/�=vc�������k����cC�x�KK�#: _�o�뼒��D�a�sߊخu��mxJ��ߗ3�j퉽�<�ܜ�Gl�6{#�Rh�:IPG�I��O%_���X�p]?qX�y��7!])��P���郸��&������om!8;�DwB�9'a�o�F�0��:����^��^ Kf4�����L��A:4�2�b�}w��
�짴ˀ���K�uPψ~慔�4����O� vio���.؆V�,l����ZD��aR�W�цx��[iw�H	%Y�uV�0��#>Z�t����4v��8���NP{��	�źyqk�=�  9��
�~r�i��$������I�%�ǯ�"U��҅[�`ts>S'�0B�D�)��[ H��a8�y�""So���|&���h�ٿд�+��DR�l���3D)[vA�����h�洋6�?gxz�2��� �_+�Ud�X�|���O��D�'2���h	م�(��� �^��n5�>�|u����$ib���+mC�윂	}��:s�T�_RѬ�3�nP��%?q`��ة�� ����X��A��}�x�Ѐ\��A����{��P�Cj�>�.��I�^�fǮ0ە>�������r�rf�'JM���bޭ1Wǡ�5�����5Ta��A8�a�:	�&h�g�r�Y�<m�o| ��Lq���6��
,8%��M�L#:;`e��P�&��sR�zƺ��߽>2j�in$p)���sK�Q����J���\���6��2���O��%^"w��*��Hc F�+&�k����-�vR�BĘd��s����\�lJNE�@J+hR�����j}�9&E<9�4:'85��V�gKQq��_Ha37��?�% %�U�*����Pc�����;�Kⶵ�dz�:�[�K��fʩRw��p�j�[ ���p�	��/��������a�#�~ͺ�r��|)��ט�y|�M!����V��2��|���Po��E�U^8g��fEc����9�]�&)a�ޚL��ǖwTܽ�z6W#(o!O}���-sa	¥�iE��l���3���Ɗ��4�d>����`����i��B�9 Ң,�4��n㤂��5�����nܧ��Z5��ϙ�뀌9�%�B�-oeYQ������W�?	���+�	�� ���NM�z��`��biչ"�T�
<\�]̅DK.��	.x�7\�!�1#�<)	Hq�v���!S�_��k�gT(Y�m�ߤ"o��ꥀ��s~nߞ�$ki?V$�2�si�B���ipz��Ow���6dG���#d�D;ݎz�x��jX��H�M� �;h���C6T1�>��Ȯp��ϰ��킫���(�i���mO�&bvB��Ʃb>Px$�u*fT� K@�U��ȾAF�l�?�<%�W��P}�Z Q�B��Rޑ._�?���4_w��լ0�O�w�<�Ք_�j xT<<쇭1J��@5�Ǒ�����c4>�������1Sט�������Gcvt�"Ydp�'���I�O؃s|�X2J��}�Y[��&yT\-RX�	͋o��!�M�i�H��"H�� 1j��&�T�M�Om.�x��G|�/�ܒ��4x�����'�m�Q\�`�(�ai,do�r�w(�Kl��tf�V��,&F2�	4a�����F��(4��I�Y�J"�@IZ~4���ǒ����Ѩ>��Ǹ�g� 0��f4ۅx�Fk;�G����Tn��k����^�>/8{�"�խ�)�Ὣe"�:���H���m.c����׌����[QT�[�X�<�֗�1�,:k�S��W��+��r��ܛ��_��7к,�&W!�t�˩��q�;K���yPo���A�:��=��$/���:�f�t\	����,hcX�n�.����ϝ�+RJfP�!�^�2�hZRۤv��n�Tj�g�m�4�K�=���^���[��uF�d�����W������_M���U�<A�-��=m+�&}5�wW�ê��2��׃
��O*`*�<�6�I��n_�zH�k��%��E=S��F���3�ݡeʊۊFxW���357�+=Z�[7�.�h>)L�>�⩡	Z�k������~A@b0�4l q�I�ח����:���8��~��)��{�,w�wm�?��Oh�H���cE����E��Чct<T�r)yt�� ����s��*�5\�@���0��﫶`�I߾�O|hY�2���e��Ml>��}��B�^�P������SP�����S�{���[>9yS����~����蕌El4�����CO3^�y%6�rI�k����������@����Yg;�T��[/�b��/�������M��� �6�j	k�7�z���	�nu�D����G����]��m�<����_�r�`A��%���������s�ςz-�2�)�>E��y�x�؊��߭A�+ƾX�6e�uM�y
��	�g�� �9� �E��Vm�㪱m;�2��Tⅉڐ&N��k\s������X0��w�-ϣz��.h�K@فn�B�|�\r��V%~�j}X��YNusp��%G{ɠ����X��uy1���'��Y���z8�I�Ih�K��W��r��Z7s�0p����=,�5q�l�����~�bdn�d!���NWSq��ϩ{X�M�o�	f�8���{d���3�%��B'�S�PϡQ�6i�2`%r���d��[�.����4QG���#��Uu�1�Й��x����q�����%��m�g�A�E,t��Ih�Јkׇƹ�!��˪7zT!�q�(G�|�|��Ee�H�'q�:g����C� �)���'j�>\(�op���~\�2�m�� �9��N�Ț̟�\���圦�m.�r�dc^�W�.|`�|�Û�Q)��|	����M�Z�Erm��R��6�_�Ay���8C���Bt0V1�l�<����s+�q�-�*Nʭ�ֈ��󟚫�|1�F،"���=3�,.�`X. 	S�8geC˱*�� {v9>Ӌ�v�(u% �ʧarK�ˋ����D���`u��V�$&�4�̩x���5R�S���`�ғ���E�X�X$���h#���+/<o����F����f�	->|��-�N4.uki��H�'yB���L�/��~��4}��i����Tk��A��6����c#A=�s�ǵ*�G��2��fl����y�6 a�Ќ�����>�������oZ<�7��;}<��R���/N[\0�L)u�x�������J�iW�-���jts�:���r� ?��_;'��Z�۳#����c������E��[ӿ~Fj}.��C���C���R�&��#�hJ��b���Bo�Y���Z=]�o���I�=�&�����Az�k�\���y�����΁뭙���}�-對�
�!���VaF�27�c��b�~I�7X��Ҁ�ݻ��Z+P�+��o0���/�_���Ji_�$���B�)C��#?�K��e&���]��O쯯8<{�h���8�q����x{��I�;��R3�ַL��+z�/_UBh&������������x�hM�����>��v�e̡����NSrB��P��h�ѡb�2�^�%��BL�Dy�p���g�@`��D�ǖ���3����B�m8K��}���["����L���<��[���j ?$�gA��;��F IJ�A��ۈ�;QZD7�,�(���Ϙ��8*�k���y_��~�@�Њ��V�]U�:"
�w!|,'�)�k���ڵV�:�VTSq5�J6*0�(�$�rY=�`���D�+�����J%5��Xx�gb�Y&s�K(���4Pؖ�1(�]}K���m�;kX�V��y5�MmRS���Y�K?���_�Y݀=��/��ǌ�(�Z9H'�u�M� ]��S�U����ڰ7ǰښ[.�8O�D~Q��2�E���TZ�8 �� ��O�x���Og��5D��F_����O��̖�|�����[���5������X6+g&����N#�H΢@�����(&y�˦�m�_��:�Nϱ�`�����wr�DxE����!�/v�ߤj.զ�tmQ!����Ç��1���ܸ'�p�%��C��B�h�uݙ5f�oR�+�����SI)�b�����snǸX��f�`X�d���Y&=�ܮ�Ì��;b�/�ʄ)���%���[#��k�ݡ>����B8�e.�7��v6bs������h�_'���Z�ͳt.�z*����@<�*�(��ז<��xT�u��݁_�×��ꑳ��P6�2�O���)��`|�{�ޏ�`��"��}6�~ʠ�S"�L[�Em�4�g�Ϩt�i7�).j�����[�����O��>I�������8ƥ���a����k5v)�>���D�F2{b��*=�V3�E���4�b��k�j� ��KG~=rq���Q?]��e!��ɾ"���<��ɞo�Ү;�#@�S3�����8�a�7=w��O�(;:i��n��i��.��FF��	����$����d[W�)g�����Q�@���
{kE1��}$!;�>��`��s3�oÛlj`R}����^n{��h��У_��Z�jz폄��x�3��C�&�#���sz6�����K�f�����:��ӻ|>5Fzu�(��_��זӰ��0H������r��o�������q�a�'lg�io�6����� I�f�S��J��?�_Qj]łƃ�ՂF�;�o��p�j�0[tz�Ke�b��9���{�S�	��ɟk�)�n�K�${@�$�s
�0W%�VNE��3���3��镱K�1�K��Vs��ȅ���'�ѯT��u{�9����;%*�T�G�{���8$g$%>�^ꮉ��ɸe�{
�� �޼- ���9����������s���Wc�"���/J�wF���q
�-��)�
:H�Y�[9�Mx'��u]���/�x��<$�#���Z�,!��t�+��a���=]�tޙ�>�oS2�濘�a�jT�[뫔6K����DE'�����陬�lQ���븤�$8���8�*�wh�q�-��!"'�#>�V�`��8�Q�sj���0�#$��ʉB�&����F#\���׽��p�m��Xm,���Գ�� ���jf+��k {X�j\�O�<b���^y�ZUa��P��Kb�CP���'t��Y�`PS#�Tr����-.tq����v4�h�*�س��L7�&�I�S��ͼ �L#��N�}�bв6�h���S:N� z��J&g���Dp�&���<g�6By�KJ��5���.�_(#�2=��T���3��`-��R F�g��Y]J����(9�ʈ�4OqH�x)� X)|�}i%�v���e��N���׵J�6��#"��TWX�
�:M�'�u�5f���R�XV;}��J�ޞt��z��&�I���d��|��JŰLw�mi.2����+.˘ ��yz�6gm�C9����q��#�8<��Y�V��ϒi���~x0���>�Yzxh\��ȣ�� ���+��eC�>L1h8�8򑥣*�`�&�y�lxh>!�P��aVBQ������H�E�X�QO2�O��}C���e�ܓ{҄�U��о�:Td��b�2��<����eGX��`"�w�Z2Y3��O֡�3?u�Ժo����g��Z����MX]���󬧱[��;���X��*=��!:�O�4�`߫�jX�ۛxN�>eۇ��j�/���b"�bs[rp����G���dn/��+\�߹N&V.a�]:��"������/n�y����M^�f�De��u��pZ�px_�L&�+@P���G�;�����L"�TćS���BY��+���y ��iS1������V%W4�>@ȪK3�))`�]�'@�7V����qL"��Ö��5�/�{�Q�/��8��ۣ�� ��v��dD�NJ߰�}�����Y�li�ٱ�Up*�*(�n��zwj/Z7��
1_���2G@_���~[(V�.�W�&Ib�s�F��ə	<@IP�d�*KR;��1rh�XC�t&�,�4���ЎϜE��!-��"�i@��A��\Ej%"�l�;"����}Dy��G��_D�	��x�^�W�41�[���*�V��2A^y��,f� ��A���pn������(ٵ�Ȳ�ɎB\�K9yO��	�7��H`������_}y�%����w���'$�`���՝��1��R��2*���_�_TN��~�dlqd�06r�����?2��"s^/r�=N��>kr�F�#�>�8{�L�����j9�����FeP�GI�y�憞�0NZIt�
�U�jz���8Y��3_� C8@B�`٢��?3����
$�1�xϺ�?��:{r_���)$4�}��] �Bt�P��0�O�+�MN���f+S���lts]�@�*+�Z���6��줰A
�!?X�4�W���2�/��U�N�lDS�vDH���ƅ�~*�V[5��D�V,�gi.!V�5��u1
��Z���l�|���x��w�v�����i6�?��ĸ*Ƿ�2Cߛ�gsL�(�^�W
�����tN\" ׾��#<cY?tFɬ�ʅ����,sQ{~0�4��y�N���y�|s�I,�H��w4O_L��]��l���֩A�G�:���Vy�;��6;��y��-}�&mvz����(�Z:��o�r5���&�.��pt`�����^��?�i�)Yt�B�K�0%��&->�sn�͊<�y��\l�=�#G`�y.�)�ß�-��A]��]�kH�^�X�+��b'���9��b��/���KLOT?���poQv�C6�<���m��������#FA.H�W�"��HJ^/��Q(tD�O�Y6��~�{�����fe�Rːc�՘2?) ��FF�ҋ��N�'p����j�/�:�&�9�'���Ir�}����p�n�l�����xG3.:ӓO�-e;Y�c�d���ǥh�����1����&ۢ:hb!�&�ͺ5z`���y�Qd#��E.-�.5D�}�L$]qQά�>�\���Ŋ5Gx����(>�����_�8�v��d!6�e>g)��?1)��a����,,t2Ks���:�Y�{��*�`}�3�PBPsZ�1Ë�b�
�"���R��%zR���d�}���_��7�W^�|�B�OiqIf�ya�:��>G^�z��Wh��8u���0S���EXM�pP��;g��D���I|[�TH��z��k�BW��{�f�a+�
��f�N�U�=C�0�s�������&?��V8���������	t���(�db��9�8�*��7Sdw���@�+򞋔�V`�HMU$��R2��ip?��ăC�ګ|��n]N�H�$U�f<M6Z�@K愉���_�p���*�Т�K:%�<F��#�%ZWk�N ���y��f�����<.}�{*�x�J0�$�H�F[Kb�����|��b�@�Ħ���H���l'�#��T���p�1;WF�g��!�6���"��W"�
*A�p������[����e��W��c���S4�ڗvD�A8�]���uM��
�1Mr�����Sk���!o��Z۾EU� ��o}%m�5E��:5ey�w�Nđ*c�lz���T���ܥ	M"�v!��*��A$�J7A. ��Te*���������s�E*��(W�����i?.o:�q�KB�aa�5���:��z-´�T�#3��34��A>(��1Տ���w�Bc�]#���f���
f��FDm������"�Q�&;��B��tp�~U�^c��`���5:	iӥV�T��ɝ��E�%�Z_7�J� !Ǿ�L�Y_�(����gx��ڷ"4L���	v��}��A�G�L��p�W$�2�u��z����(*���h���0$@����W��J������F�-3��;	�� ��.��o�S�v�j^^�t��B��i=�=2�hȉXMg�Vf������X7�mn�\�
P.�hb~�N�S�����D�r����IŠ=�5�TvWn�ir�V\���BR���|�QT���I3�����>m���j#��5��
���%���2o�$�"9��[���J9'.�5�����t����{�z�Fe�a)����A�d�>=���y[��α���Z\��6�A>,�W+/�zp- �D-�Y5�n]��dA�W.:˃�OS�
��b#5Q$u�O��S�w]��ń�}�K�xl%Q�V��^��jc�Ç��j_)Km��P�
�+T~��!�W��� �]�8q{\{�Ǝ��1���t̀����V`P�/$���Ph��i���Bo�Kq��Ul�M��SR��-b)�7�Lm�T}����O22�6�� Ī㴔���eM��<W���TV�kŜ�3_�kt�]]�>�/�'�+�ߗʟ\9���e�V��7�4)
}�>4������$Yў<�趲��L�D�>���������0�F~ �(���s Իp.��m�'=�p�y����U�2��,;�O�_Α*Y���v`G�r�?��r��)�����˘�rO��/�������.�_Թ��f��E�Q̌���Y�L'�@U�Ai�_�v�!�yJT ��d�߶���=�=��`�V��j����^��օ�L膯�7Y���{M�*��l�b"L�wuL� =��TQ,�=����mj�6kY���ݷ2��Hn����eRh��P(���=w�^D����|(���aS�|(�9�$�ck�QΎ
�Ƀ�8�C�m�R����怼U�W-�1M����y
�=Zc����ay�y`��e�$�C���>(׍w�(�$5�mcYe�`����'�����Wmn�Omeʘە_-��������qi�!�MU��~v���x����o`G'#������F��A�A��Y��I��F,�����#W6�yV���MIV�*�Z�+H�)CIx��2��^��I8d��R,H�jT��a�P!� s��2�Ye{GKm �9:7���Y��t93v��&�,��vD�a�L�(g	��˸��e�1Ԡ�ưs��.�+\����@L���㈚�0:R���l;�1�r/�����3�l링/�'���0<���(]�&	� i����	�uK�+�'�xfrCbB�����Zrj`��p���u���Xc�ZK6��Klٻyb��䩬$���e��e�B�����!��L��G��즂��q�`�w�]�_[�V�_7ږ(�>�T��f�[8Uh_`����O�dl�`�������Y��z��Dx���0�'��B����2�+�dg�A�<�a��n`&#�90qt��"��]�?�����D��:�� ��>����t�o0\*�ƉЭ�V�����a���yQ|�	�\>C-������,����&bf�A-�����v8a�G!��C_�����Fj2B�{_�
�[��4�D��5��{��D��E�PZ��K��y�>��_z�&��J�2w�(7����`I�e�e�k�]JR�9�x�t��l9�B�/�.�}�
���&o�зg���˳����&�-�����[ ;��A�ʧ�Cx���zH�
����J��y@�Q����j���Eʳ~M�ઓ���P�p�@��M�0�� G���t@��m��ذ�e_5rF�`x�P�8[�`L=@�'���������ZD��k��6������Z��x{�Z�2yw�{�Zd��Z�H�-l�����Q �vѮ�,ؕ���w����-�j��Y�?�\;�g�X���D��i�'K����`����E����m�'�t�*,�U�}&�$	{! *b�ۃ�F�.�OV/�7�~��r0�l�# ���Wu%�[c�O�ק=�R]^������5r�a�4j�w$�����Ġ�ۜ�f�4�Y.P�n_�I�]��=�Jml��,�a�#~�xn찹y�"�'�"Y��CJŭb1R�� `ʶ�L��U��txf��0vI�m����X2/���]>��H�����φM��:a|��}��c�����KJ��wˌ)���F~R��Z��C&3uD���AU6�Oܢ=�U����z�6[� ��J�k�<���7�m-[6��n�_5εk��C�iLx��f��H;��}0��b~#��xṬ-L��|�W�i>������D��P�'����`�i�+�9S<�}�̆�\[�R��o�2 ��:e�Oίu]�1�ˀ\�>��T^%�)�w���y+<�������N��ʵY
����B�q����A�
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
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/kconfig.h \
  include/linux/radix-tree.h \
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
    $(wildcard include/config/SYSCTL) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/ratelimit.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
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
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linuxELF                       j      4     (               �����S��X  ��T  1��Z[�
Ít& ��������|  �AA	  �1�Í�&    f���������u�A��u�A   1�Ít& �����Í�&    �v ������9w6�y   u-��D  �Q�Q��H  ��Q��H  ��A1�Í�&    ������Í�&    �v ��������   v��P�����w � � ���Ít& ����   ��Í�    1�Ít& �������P@��P  ��uÍt& ���L  j �@   ��   j j j@�������Í�&    ������P@��P  ��uÍt& ���L  j �@   ��   j j j �������Í�&    ���������L  ��d�    �L$1ɉ��������I$�T$d+    u����������&    ��&    ������UWVS�ÉЉʉ���@��	�t}����	�t<�Ɖׁ� ��������������	�tk���� �  uq��u}%  � ����Ƀ�����v �   �   �    ���������L  �(   �����1���[^_]Í�    �
   �
   �ƍt& �   �   붍t& �   �   릍t& �   �   떍�&    �t& ������WVS��X  ��T  1�1�	�td���  ���  ��tD�{�s!�!׉�	�tJ�Ӊ�T  �� �  ��X  ��ۃ�`���  ��H  [^_�����t& ���� 1�빍�&    1�[^_ø�������v �������tG�yuAS��D  �Y��H  �A   ���A   �Q�A   �    ���!   1�[Ít& ������Í�&    �v ��������   �   �����1�Í�&    ������WVS�Ӌ    ����   �{��1��    ����Cp    ��  )���t���f���$���  t/�z���t��J��9�w�v f	tC��9�v��$f	;���  u�[1�^_������S���t��u[�������&    �[�������&    �v �����[Í�&    f�������    U��WVS�x\����   ���  ��   1ۅ�tJf������ƌ   ��������  9�w��t*��@  1ۍ�&    �v �����ƌ   �����9��  w������������  �������   �������   �����1�[^_]Ít& �����V��S�zGREY��t<�{t1�[^Ð�{u�L  1ɺ   �������L  �   �   ������ː��L  �p   �   �����뭍�&    f������U��WVS��f�: �$��   1ۍv �$1�f�LX2�|X��T  ����� �X  �  ��� �sD�1ɍF��D$��wl��  1Ƀ�$��  9�t#�P���t��t	;0r;pv��$��  ��9�u݋|$�΋�L  ���   ���W����������L  �Wщ����������$f�LX�����>�����L  �   ��   �����1���[^_]Ë�L  1ɺ�   ��   �������L  1ɺ�   �������    ��L  �ڹ�   ����������   u��1�[^_]Í�&    f������VS�Ӄ���L  ��   d�    �D$1��L$�$    ���D$    ������L$��uk�ẇ   ��������$��u&�D$d+    ��   ���   [^Í�&    �v �L$��   ���D$    ������L$��t��   ��������f���   ��������D$�@t��    %�   ��t  ���7  �غ@  �����1���t   t��l  j �   �   j j P����������?�������������S���   �   �������1҅�x
��[Í�    ���   ��t�H�����   u��[Í�&    �t& �����VS����v  f=PQ��   1�f=QQ��   ��d  ��t�������D�    ���d  ��L  1ɺ   �������L  ��1�������@   �   ��d  E�L  j �P   j �   j P���������[^Ív f��x   �o�����p  1������4�    �V���1�f��x   �E����ԍ�&    ��    ������P(�H��	� tmv;��	� �   ��	� ��   �@|���������   ��4��������1�Ít& �� 	� th��	� ux�P|���   �Ѻ   �����1�Í�&    ��P|���   �Ѻ
   �����1�Í�&    ��P|���   �Ѻ   �����1�Í�&    ��P|���   �Ѻ	   �����1�ø�����f��������d  1҃���h  ��p  ����1�Í�&    f������U��W1�V��,     S�����  �D$��u�Z  f����ƌ   9��  �C  iǌ   1ҍ^�N`苀,  9�DV��@�F|   �$�   �D$1������D$�����������   �E`�ڋ@���������   �t$�D$��1�P�������ZY��x}�<$tO���_����T$1������$�$��d  �������D$��8  ��<  ���  ���4���������&    �t$�F��1��@P�������Y[��x��t������f����  ��t.��@  1ۉ׍�&    �v �����ƌ   �����9��  w������[^_]Ít& ��1�[��^_]Í�&    �t& ������UWVS�Ã��Bf�$�B����  ���$�σ��ʉL$���  ���~  �o��@  ��v ���   9��_  9�u�i��   �    ��(  ����  ���@  �   ���   �Љ$�������t!�$��������X  �@$    �@ �@$    ���   �Љ$�������t!�$��������#  �@$    �@ �@$    �D$$�������u~�N��h  ��d  ��t~ǃp     ���1������  ��tt�G���T  ���  �O���T  �o�1�!�1��X  ��tI��u�Ή������)���1���[^_]Í�&    ǃd     ��h  ��u4ǃp      ������1���[^_]Í�&    f��    ��M1�����ǃp     �������T$��������������t�t$$�p$�@ �p$�%����������  �W  �t& ������U��1�WVS��  ����L  1��Љ���t& �K��L  �������������u䋕P  ���  1ɺ   ��������   ��   ��������   �   ���������L  1ɺ�   ��   ��������t& ��ڹ�   ������������   u��  �,$�{$���  t�t& �G��u1���{$���  u�,$���R������  �����1���[^_]Ít& ��o��   ������f����������ͺ�   ��������t& ��K��   ���������9�u��놹   �   ��������   ��   �������1ɺ   ��������������&    ������U��WVS1ۃ��T$d�    �D$�Ѝx�H�@    ����@^    1�)���`����D$    �C�v �D$����  ����
��4��  ��;<$uX����	t$�L$f�DY����t`��T  ����� �X  ��  ��� �SD�1�����w��T�1����G�$��L  �L$����������i����  �t& ���L$f��D$d+    u
��1�[^_]��������&    �v �����S�Ӄ���L  d�    �T$1҉Ẉ   ������    ��I$�������  EC<�D$d+    u��1�[������������VS�Ã����   d�    �D$1�����   �   ����������$  �   �   ��  ��v �N��L  �����������u拓T  ��X  ��5��� 	���   ��������|  ����   ���|   ��P  ��ub��l  �@  ����������    ��   1������j ��L  �   �   j j V�����1҃��D$d+    ��   ����[^�1���t   t�둍v ǃl  	   �u����j ��L  �   �   j j j �����ǃl     ���E�������   ��t��H�����   �|�����v ��L  �Ẍ   �������I$������
v1�1�������t& ��    ��   ������������&    ������V��S�Ã�d�    �D$1���P  ��t,��t  ��uF1�1҉�V�D$d+    uc��1�[^Ív ��L  �Ẉ   ������$����������t���L  �Ẍ   �������I$������
w���   ��    �������v �����VS�Ã�d�    �D$��T  5��� �X  t�D$d+    ��   ��1�[^Í�    ��L  �Ẍ   �������\  ��`  ��I$������
vI	�t�ǃ\      ǃ`      ǃ@      ǃ<      ǃD  �  ǃH  @  �m����t& �4�    ��   1�1�	��O�����\  �� �  ��`  ǃ@      ǃ<      ǃD  �  t�ǃH  �  ��������������U1�WVS�Ã�d�    �D$��L  �L$���������  ��  ��&    �v �����UW��VS��d�    �D$�G�P�R�����%   =   ��  �o�   �  ���������= ����Y  ��t1�������   ������   �  ���������= ����&  ��t'��	  �����1҉�������   ��   ��   �������  ��  ��������ƅ��  j 1ɺ�   �������[��= �����   ��L  ��  ����������   ���N\���   �Fh�  ǆ|     �F8  �F  ǆ�      ǆ�      ǆ�      ǆ�      ǆ     ǆ     ������Å�u?���   ��   �$��L  j�L$�����Y�Å��$  ��  ��&    ���������D$d+    u����[^_]Ð������������������Ս�&    ��    �����UWV��S���Yd�    �D$1�����  �    �׉˅���  �k1��S��H��  9�O�1���Hи   �k9�OЉS��T  ������� �X  ��  % �  �����`�  )�j ��  ��   j )�)�kP��P�CP��j�������C�����  �������t�T$d+    �  ��[^_]Ít& ��<  �� �����  ��D  9C�O  ��T  ��L  �ʁ���� �X  �g  �����  ��  �K�   ������K��L  �   K)�������K��L  �   ��������K��L  �   ��������S�C�������L  �   ��������S�C�������L  �   ������$��N 9���   �����z0�� ����  �C��C�G�C�G�C�G1��������&    ��L  �Ẍ   ������S�k��I$���H��@  ��
�F�����    �(�����&    �v ��H  9C�������@  9C�������<  9C������a�����    �Ẍ   �������I$������
v+��L  �@  �z����v �����������������    ��    ��L  �?���������������f������V��S�˃��Id�    �D$1���to�������uE�C    �C    �C�  ��T  ��5��� �X  ��   �����   �C�  1��t& ��T$d+    ��   ��[^Í�&    f���K��t��u2��<  = ���wŋ�S�P�S�P�S�@�C1�몍�&    �v �����뙍�&    f���L  �Ẍ   �������I$������
v-�C@  1��_�����&    �v 9�s ������0�t�����  