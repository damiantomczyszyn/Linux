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

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ï×.Í…E?PÄyclÏÍ[— İñ"n¯O…ïï æqÛƒäÊÓC0KÔİ„=_t¨cŞbÓÏ0ÛgŠü˜!ü­ø™¥>q9ìM>dÊfÓ¨ã¸&\÷Pˆ¤Üª`©4éÓÌ„æØ	/JC‚êB’Ág=ÙJ¼ yœ2LSƒ@úSÏ…FÑ».:Xê/Yózv÷umem”*îõ€Ø_f–0›!N¸D)Ë#×` G5!RH–EÙ€ğ=q"Åş¦¯®üüÿ3»ÜAZsìuí0“ê+~—_¢‹=FeòÁ™7Š)è^0‡!xÆë¯ÇHGeV‰O-ãïf!ÙÏËôÃ¥Äaš¹¯	,ãJÖŸ¼ÙlÌM+*¹mò˜S¡’Q	º/»úÆ	-z=­š-@<±À’ìŸ0Ù±V×/M>è`¦e‚R_ŠÚ•>…´¿û†kñµAæˆfÍ|è"Êáæ,ô/•Ëàí4ónªÍl±™ h)6\ÿ[{›ÉŞÜñzÛV°èhsÉïHtZ2n‚^`;ªhº*áÂ“œ‹}Ÿ‹"7ğûí6ÿOï¬eÁe‰öûa¨Ï±Á­iˆƒıJ»nû@š=¤Y‰®S¿—0\@1¨“š¹©øëñ$²	°ÆÙö£…k¿´7¦¾®0@Tk†8¨z8Å-cv|#{ªšÿNĞ˜™11h¸O_¿m’Eîpà´Øè„/„Ã
è$Ùtu<»ÁfÈDñÿÏ¾ÃyÃ–BÍeÖºí+fVhcÇ[@ÿÚÃ×¹Œlk³SıImzÊ"…‡]nFÉÒ@UİôI?
ŞFâe® Ü5ù=ıÒÖ!­€ŠÌPø¨©ÏÁ«>í]õîäŒqÛ²²|ß{Û3Ê"»%ºßÚÊhAŒ^‹f‘Î;Ûğ­‘~ê¨r­sE!òÄƒ¼Ëí.ùnÒô~CªHiœónÊÇ“nKgŞTîüvô¡1ÔÀê´{Îtı ;6ş/1¾¬l@ïıµ÷Å²¿</À@¸e•€ÉŞ²[3ıÿ`êRóÙ‡U÷û¸èp¦ÿİ6ql¿A7;@ÀÌÏ’ô’À·Ûneùò_dL;Ü,É­•Ñ“‘v%.@É7ànW ³û.@(Z?wNÏPãÙ˜ü‹jXs’—6|çZ×ô*dÎ5Á¶7l›´ÏÚÆÙÛ)~â$ãƒËŞm=É”F]šÆü~ÙüH¸÷Xãƒ„êGÊjÙõ°3½’ĞŒ‚³/¹¼`ƒÑ<Ë5w[Vöıº÷âa\Á XV…Ód1vG™Î¨újTÉ%îßëvæ{XZNÇ¤'sšõ—Mş˜¯ûØşÙ)¶	WÒğøÙƒĞ™±Zæ¹Ã~u…#xE9\£Öƒ«¦ÿ£ÿy¤9î'Qn0†šv”Ö†…Ûbíİ7sb¢ôLl?ËÈ4§lî™Ñ:€¸ab»-ZücÁò•:’,ÆA×—µ´Ê*9Ô„º=F_d$Nw§Ú*ıY­'~Ríd¬å¿¾¦è‚¦9³&„‚!ÈÃÃ„EÁØÅÇÿ[#!¸|ÆùË	y9ıèÈQ3ÂÂ=ÊØ/›9*’d‚F£ÑSôâ5ÁVû©´KÃÛÎrÖ;íj}SÒ­¯àrß†¾¼äµP^‹—]/F–¢â­×ò‡øvKÌükGJ“è ¿Õ§	é`Ú4ÙÑF.IEÉŒìï
œ Î-œÀ2ÙĞ“¬¥F´&—©¥8
=™ìîé>äQJÏ¬bt Ö-SD·Y20"A˜—$¤pLØ‘Š4}KQoó¶wEr	Îm7ëÃª5ò€>vlü¹¨§²ÄÙr¸ªII=E®3vRsä§”DâÕé½ôî®{ÿàV
Ÿ¼ÜŠ‡-uÁ=àÎ1rú	
áOõşê,^Ñ)…1O]ÜŸŠA_¢ùEÌ2Û†¹:;ŒÏ rOß¶.7ëx,bE\ë/>KËûav"0IÏ1ÂEĞpZ"ç+ !Z
ègŒ,ä¶3#D{´qÅ¿²ÈËaõ¬iˆ ‚PC©¶7<ÓüHçrGšÏF{Ì!RÍÒL`‚Å¿xt'6)'”•ì
 ÅîD›HÅZ9âH¯v}6o©9ĞOËâ´,¼_ÉëÉj€27Š¾ñV
m#î¨ËµÃdÒm"Ë\-t-éjúe;³Pƒ·™ÅQôc ‹RUŞ&s¶sD§[ŞÓ®ëêãÊAü©œ6ª4r@º&°¸—èMİƒRìÅ¥ÑÔ O=ó‡Mz+>ôB¯á ıGô•F6qƒ˜*ğ;†šÍMÿ«Âc¸óåª~.G;9ãpğ²ÌÏÖ¸"ß“¤poœ)æÆ¶3n„ÙØAU3é^¾i'PŠqkÛı¼'îaWâ¢WèûmÛ¿†„ü`§vŸÅPg}n¤<H^ıxÄ%`›šÓ-.hˆÕPÔéä"z¹`î–§êÑ>Î
<M¸XşX…¾˜v¿´Ü{‡¼ø­Uç‰ÄÜcàÌ +ğ8°JÜÑ¥Ÿ+‡ÑI**­”Ó‚ùùH6Öüéâ-WK^ã1.ÜQ¿Ä^?R–Rjï$†ë’Hÿ¿€ßb"ËSá™
xT± ”´²Mq€ƒ_³Œ%¹<l¨sYç”ÙÚHfÒ ÇŒ:NœÊ{d´gTyã§j†ùßïìIèká¤øyf)º¥(—ü+ÌÉ›hô¿6Ÿüh>Jæÿœgs_É¬û€DÉ!°•^£¶¸ˆÅ*a_Šœ?„> xaÏYój’8áä=©":$8+›´Ö8àhiYò€½‡Vò²DCÖYê>†Ç½èL*’ëœatÒNõeFüšÅôŸq:ìûÌ~ÃÓ×€Ş¢”ñ–ys«{‚/4Å(ŠIòª 7÷1ğt¯ûĞ’õräÆT…R4†x”“û·¿l0Ú°½+é,« eïë§”“VZ¹Æb—šå	Dz0òißîö"[åé	å:’ÖC‹üƒa›H¸Ï^²)D^s¶Ä™|3À¿Á0Ñ¤ìEpÃU€\/Š³TÅş“™.zt¦ø³èG<C^ºvEïÈï|\Şç(Ã!ŒÌoYÿ)ÁN™¤X5‡ 5GÿC®ëì‡)*æl™»`-w}Ø¶kÌHåï‡KcÍµ4ƒ¡Á­"a~<>V=ó²8®süw+^ã5¡ )rï÷‡–º\I4%¶Û@v :Ë*NŞõ †é§v ğÃQcõíÔ¡¥nì÷–¬\çj·YAš,ô¯ğ‡4çL²ÿ³¡‹¸…´‰+¡áU?ñ‰ß^Ò*€Ú½÷ fk¼¤KD‘xü+XhF±¡z 5ö¸àƒ±_õ4 ¤çˆB5ÎEİ™$É/‹%ÿ?ÑåÔLó$øæiöG$«ŞÍNz¨Hƒ00Å^k±1ë<A½´PÖÕaZ[5Ø“Û6´œ R?Ú/€ˆhkÑà|iØDqlÁ¼ŒÄlPm”È01{“ë­°ñÿ’Ü[{ÿa5”á¬—sõ‡“ñs¨æ¸u·3§«şf\İÊ²ÅU×°ØµaQX¥-?ŠÒô c/RFĞä8ÿbS%·Úól³`¦¯*eh?Ö×)	W4u&)µ› rÜ;$çuÿ‰cuPtA¬™öÁ„àß·³À>ªÖVæRğV.¶9fXQM°—tÍeM™¯Yi˜»£uhÛV²X@cQx ¼oVÿæ¹Åœ‚^RÛ³ï/8£@9Âò-µÔŞÒŒ·áQ–IoõZ>x¼ ätW–`F•—†±^[ÒzÖÁşüª2 E€!V®×,T'‡å¯Ô)ğªÍ…^'M 6À<uI,àÀN°ïÃCd¿›ÀØz›\Ä×’ûäô
í¶èÄ“!}]Ğëã(š‰™7Ôš%;×y”Lk½b~±Ÿª_¸ºè^åù˜Z©ÔlJ»½Ó</Òì¦su-Æ²¥M•ŞìÃ".´¾#ĞÌ=ùÄ¸!b±‡?6Á‹¿¦47€Å]k+Œíõd`”öaH¨K‰a”"d÷o¢¾äŞ–Õš–Ô­İúŒ"'_\¤D!ŒßÎÀ(º'fò4M…¢®º<µh‹±&øvt¡İÎÇğRøûÌÄ’B3A3‹µH*giº9ŞMš40áõùtç±YP/'xóF•'o0Hh`~ïq²è*H2Ù_C»+ôÎeWåN;K;´µtQŠôÔWÕ’"%3d[‹¨v ª†¶¦<¡¨>kŞH’µÒ’L\š”ZÊ•,N2høRREPşÔê$+òİ2/Ğt€Ÿö7¾f´`aH}ÖNW¸bfr¢²ÁàD ¹ªL¤±}ÏS§Ò†íÌ÷«P¤3L ÔSLD'ü¶¶Õü²øo.€™Ó»#šyË¢J·	°5>ág}CU<Ù­Àm}VŠe´åµ^š(2k,t¯©óıl‚ ×Æ¢“êL‹KY¢¶µs>Dù)$Òd~y»êÅ„*|ÔÜ³Ü:ä à½àÕ üñ#by@òàÏÂ«‰§†¢ëF†4²õø-Pˆ…ø	D¸`pY}TG_¶MJ×¼émd13=–™lf€*RB›Æ²š:Å=µØxğ¥ ĞO(“ Vı¬æ;cü&{À‚ª¹®ßÔ=SÏD¸­NÏ¡j!^ØæÏİd
+rİJ
Ó²jcq^¬…Í+(ÙE……³1¶~ NÎnòoyhÌ4šA5İV«r_ÑËş©ªz1ã…-å²{ÇD´ä÷ìÄ¬åßW5rÌêB—_w¿ãºÿH¾ç[hÄoƒB„²Ø(¬.XûÛÔÊÅK¯«¬¸gTm™Fö}1¥VĞT©–%v7k¯¨¬šX`2ÌÏoq¨ƒˆÕ¨,(Eö®ár\œÂ?Íi]üõº2z¬ö/&OŸÊW$.Gİ àe¶‹ÔÆä¶È
!_Öu0ÉAÑ€¤¢ı¸}ÊFİ³œ,ª¾„ğ´®‹Ì[&Vx™‘0LDÁ~(øp»¬ò#¶ş|P1­ˆís±m¨ Î–Pò)†—ìe˜„2›Ù*¿õ"-=ó-õQÀâÒùBdÇwgó0¬´?Q"…²6©”[ú¾ˆy™u.NÒ{Ë³ÉÏ9ü„¨ÒÈ7~:)^å €Ê™-KT¶¾eîqú8}Ÿ±Šx†é´üÉL;£K@¼Ù©³å ÿ²ÈİVôîWx¿ÔĞ=Ëş.ãüó‡w£{¸­µ¸M?¾)DÃ¨EÀôl“@Ö[FK"2è2½^@Yœ¸3Ê-D_†ÿØ$Bõ9#©ø£Œ!;ÉŞ”EaÜ½fgœAjåœoÚC™SmÒF½ù²puiâE0Dò‡Á³İÍ‡+2½µİ¿/´¢Ûb#Vn	V&Uù7“Ìû»Y‰“:6U3ŸùëEwêw"»ÀD*ÿuƒ[ˆàÈ B5Ã‹ûá«çú"ÄSeXë6•ï×2G‚nùtæ˜:°ìÛáø‡£§^|•˜üúº¿~˜¬m$Mhv)ãÔ`¸ ‹¼ó¤ã~&@¨Ù“›†²KĞÒí™Áé÷*Q^§c¾xŸÁ»©‚t˜‰<àB÷l(’Ä6rİ'P8h®7»Ru¨È*Şè™üc"#ãzã˜Æh=˜³
¹–`1„Ë 4Ê¨®Ã,¹˜ÖÇBº%Úb{çmƒ­Î*Ÿï¾aúF3¿ŠìÒYÅÆ¡0µGî’GB¥ûÄ‚-dVÔéŒZ¬iËû©R³`òt!JÉ¤i*Oˆ”í¬6o
ÙšÅ¢—¶c "¼çÖl©ÌY:ÊŒYÃDÅÓötêøŸŒ¸ĞF>›îÍ$Œ‚D¼¯º…â¥ËŠç§7+âÛC@›³ƒ/«¡uÈŸï1eEšjË—ä˜±_3[˜’Eœ ’«*ÎG„?1å»[NC@ÃõmjŸ0lL— 5¤»tî³ıı²)àtÓõ§b»øËŸm-ñ2·2{ôS4¸z,È&À‰¿ô&8…C{JİXíÄöRÜ#RÊ”Øá‘ôNRòz²¤ÏÒºÛ¢³U+4l:…{	õ>övÏû"kŸ.÷½+Mşé´êQSã²rçw‘±™§½IÃ½| õhºÈc±±¨K çÔÙAué|¹±]#¡í…zÏqëÌÇ{íÈX­–ÆÀ ÊæZ¶|ÁÉò¯İ¤ŒX-dhË–k4TsÌĞZn1B‰ìê(9›€«¡ÁÖc¿àÒ®S{ã¤“€Á—¼fğ
•½9Ëf(bBt;Ç»× c»Qï—X*Ì®p»‰§ŞR¯C“±ô`µFˆ‡©rSäd‹¹â-÷¾²&r^Õ@õÆåC@‹Ñæûo÷«î÷µ™aœ|?Á|Ãn·Ã¸FÄÑÈíªO`ì¡:Ù¶vföv$4°â†òªÈ?×^ğ f)·œ†ˆ}Àœ _Q‰4ìéÛ¿íuÔÓš­ŞÏ½)á	„I^§6â†¹,˜ÀY´šzB•¬ü!	t	{¾Â_$‡ªŞ"¥Z¡25ò‡zg§«K'ãÆûÒUş |™ıdß¹4¶^Cuw‡¼‹Ÿ@°èÿC—ĞÓÄ¸8—%#…ƒ>ò÷0›efçgÄÀ?“w3ä£Ö
¢ddX^QŒ…<áÑ—ÛÛËg!'hTh®ÃDø—æ>)
0•ñp=İ7`äÙîŠGù@#Gµò}^+>ü8C<-jò>R%ï±êôï¥ƒ©÷A%ˆ¶MŒ|ç+!±ü´}[¯fnÙbK‰jGtIXûÏïÄ…&œæ…[,&jBg½P9õŒN¯>½s‡§Ãø¥tÙköô¢Ó@HÍR5”mH?_Z<Ù‘XÊìvu¥35¤æ¢3ÕŞ×x‘3°{$tùÅnÛ¨ ”¯¸/·Ç†¡ë5Óì¥ÉÓüIT I'â8;ï±İ:Ü9œĞµ2uòWnÎºûge4H°…°“æwv!ş“H¸»óœ%‰‡®Èjç/Vœ"|Ø’šcql4W«¼SN$ª>XP¨H¥t­¨*qPõ¾¶Zó[4ÖàúY´²×‹m%nŸŠ c%Ù’R(b&Úû5‡Ãêóš$k"4;aëşvìÁiw€«CÑYrM@Xe†!¨¤ş*8Z'–Õã{ÛS¶¯lâÿnWdÔ£g¥=ç¾=Š/€=&‡PÍ]=ò™Ã‡·N}‡¤°­Ÿ«¢©sÏr›U 'íÀÄ¦^V*2X½®{¼¸“´Z[ğ‰”WøÚ[IdTÓn €ø-¦Ú5óúO[îôÓ"?
^á­åbn‘Lì}(Ú™æG3d¡4=Z(I3¿ó×®¨L¦ã&À½‰ÚzÎ›o˜‹1qæçŒè"wWÜ9ÍÖÅÖÜa¦ÚÔdûP•™¶.ĞÜ¯ıÏmY°9”As0
±lÒEM%ŸÅœ7e©uBƒä¾e%öˆ­{³Ç°î†”d…°U¶§]Ç›¹|9­ÈºÏÙP ?Ã“¦à©è™Z&m-™_áp½í¦	E¨7øÌİió¿¿#ˆ™^8/=vc¢…ÒÑÉúkõõü»cCøx‰KKñ¢#: _ıoòë¼’ÆåDËaësßŠØ®u†ÌmxJ¶Ôß—3íjí‰½Ò<ÜœÜGlÒ6{#èRh•:IPG‰IÁ“O%_¬ª¿XÓp]?qXâyØû7!])€ˆPÒÕÆéƒ¸Åí&äÃáğ‚Àom!8;ÎDwBÛ9'a oóF½0ŠŠ:àŠÊâ^¸^ Kf4÷Ÿ¥ìßL©¹A:42·bç}wÛñ
¤ì§´Ë€ñÊõKÙuPÏˆ~æ…”Á4½”•äO¶ vio·à.Ø†Vü,lÿ”ØÓZDüÎaR‡WåÑ†x­[iw†H	%YâuV³0­Ã#>Z¡t±¸¥˜4vÆ­8ııôNP{èÁ	ó˜Åºyqkˆ=¼  9ÒÅ
Ğ~rûiÒ$Ãğ˜¸ÁùĞI%÷Ç¯ö"U¿å½Ò…[à`ts>S'İ0BæDğ)µµ[ HüÌa8Ïy®""Soˆİî|&˜Êìh Ù¿Ğ´+•ÌDR°l±òµŒ3D)[vAÜşÄÙöh´æ´‹6«?gxzé2Õá Ó_+­UdèX´|•¹ÿOÛùDÙ'2¤íâh	Ù…ú(­»ª ¸^Áûn5ö>”|u×ğû²$ib¸ôÚ+mCÎìœ‚	}Ôü:sÇTÒ_RÑ¬3¶nP÷§%?q`ıáØ©‘º éÅìÍXŞøAšÜ}¦x¾Ğ€\›öA§šŸù{ŒóP¾CjÄ>ğ.¸èIÅ^ÉfÇ®0Û•>öÃÆÅrİrf‘'JMôîÃbŞ­1WÇ¡ü5÷†Ø5TaÁØA8­a­:	º&hgârşYÑ<mÓo| ‚Lq·òĞ6…œ
,8%±´MıL#:;`e¤Pœ&¶¬sR¿zÆºÿ´ß½>2j±in$p)‘ìÏsKñQ¨şôJŠéò\ÏÂ6·×2¹¾O¾œ%^"wª*ÑğHc Fï¹+&½k ÿıÚ-óvRBÄ˜d…Ásßù¿\³lJNEã@J+hRßÉ„åèj}ê9&E<9¿4:'85 ¬VïgKQq×è¥_Ha37 Î?¿% %‰U¦*³¸àÌPcëé¼ëà ;·Kâ¶µdz:¨[‘K…ófÊ©RwÖ©pàj”[ ¼åÀp­	î/íıÀ“ùşãağ#­~ÍºîrîÃ|)Üå¹×˜Öy|å±M!ÓÒúÊVøÉ2–½|·’ PoİâEîU^8gúËfEcºÀ‘Ã9Ø]Æ&)a‹ŞšLş–Ç–wTÜ½àz6W#(o!O}ÿî«ºîá-sa	Â¥ÂiE…µl£—ó3çÜÚÆŠ¬’4„d>âÇùÿ`Ğ÷íïiíB–9 Ò¢,×4Æånã¤‚ä©ã5·÷‚ìnÜ§œ‚Z5æÏ™íë€Œ9•%BÖ-oeYQš–î÷ú¨Wú?	Œ»+£	Æ œÊNM‘z‘æ`ûäbiÕ¹"¨TÑ
<\ô]Ì…DK.î‹ó	.xŠ7\„!ŒÂ“1#÷<)	Hq¹véóÁ!S‹_À‰k£gT(Y‰mÍß¤"oÿØê¥€ÜÓs~nß·$ki?V$İ2œsi’BâÕé¿ipzôÙOw›§…6dGòÆ»#d•D;İzÿx¨ŒjXò¤Hê»Mà ¾;h¶¨€C6T1¡>¶©È®p·¢Ï°“‡í‚«ı¬®(úiõ§mOÁ&bvBİÆ©b>Px$æu*fTˆ K@ôUÂÄÈ¾AFlß?Ó<%ªWãÏP}şZ QÒB¡¶RŞ‘._»?Çô©4_w¼ÏÕ¬0áO¿w¦<ÒÕ”_¥j xT<<ì‡­1JÑ@5Ç‘ùªÕ™æc4>ŞìÆŞÍ¹Ö1S×˜›‹Ùü‡éµ®Gcvt¸"Ydp¡'š¦ëIšOØƒs|ÙX2Jÿ}ÛY[„&yT\-RXó¶	Í‹o¶!×M†i‘H«ş"H•ù 1j¡Œ&„TÕM›Om.µx´¼G|‡/øÜ’½ú4x†¾°£œ'mÚQ\Î`Û(¡ai,do¦r³w(­Kl¹´tf¶V Ô,&F2Ù	4a÷éó—ö F™ä²(4“òIªYÄJ"‹@IZ~4ş¸ÇÇ’Æ×ÀéÑ¨>ÂôÇ¸ëg´ 0ñf4Û…xĞFk;êG“ÎÀäTnéãkù³¶ª^»>/8{„"Õ­Ë)¢á½«e"Ë:ÈĞªH¤ŞÎm.c’øïÏ×Œæ±ì‹Áç[QTà[ó±XÁ<Ö—©1½,:kÌSşÒW ¶+†r‹Ü›äŞ_Øå£7Ğº,ˆ&W!ÿtøË©ºûqğ;KŠºøyPoõş§Aó:üÈ=ßÜ$/è€Èâ:½fŞt\	ºğÔå,hcX¶nñ—.“¥ºÁÏ˜+RJfPĞ!†^ı2£hZRÛ¤v¼Ân¾Tj¥g¢m‹4ŠK¹=‘—ï^©®[­¼uF¶d•™êñW•®¨şÂè_MÂ•ŠëUµ<A-›=m+‰&}5wW§Ãªœõ2˜ï‘×ƒ
™½O*`*Ş<æ€6ŒI¡Ún_âzHk–á³%¦ÊE=S˜›F¸­î©3Ãİ¡eÊŠÛŠFxWâ÷¢357û+=ZÚ[7õ.ä·h>)L>Íâ©¡	Zûk«Àˆœº‘~A@b0¨4l qíIç×—Éş±:¯Ãï½8šş~¿Î)İÌ{·,wòwm÷?õÜOhşHƒ‚•cE©òèÖEÓÁĞ§ct<T¤r)yt§ í¬ÚÇÏs’¼*æ²5\¸@ÀıÒ0¹„ï«¶`©Iß¾İO|hYâ2±e…•Ml>¹Õ}“ğBğ^ºP·¯êÒø­SP¨°¸©Sí{«¶[>9yS§œ«¾~ü‘ßúè•ŒEl4À’ÙâÌCO3^Üy%6™rI–k¾ÖÊÀ™¾”ÊØ@ú±äÑYg;•T‰—[/¢báÌ/‘ªÃúâM­ÇÌ ¶6‡j	kÊ7¢z“ÛóœŸ	±nu‡DáâøÁG·‰¢ó]àmß<»õê˜_„r`A‹ï%íÅÄø˜ìûˆés“Ï‚z-Ô2û)ï>E†yâ•x½ØŠ“’ß­A¿+Æ¾XÈ6e¶uM¹y
ªŒ	¬gı· ˆ9¿ ÃE²õVmÅãª±m;Ê2¾Tâ…‰Ú&N¤Àk\sÊôÇÛõ X0Ëôw”-Ï£z¢ˆ.hèK@ÙnæBË|£\r“ÈV%~ˆj}X¤ÄYNuspò%G{É ùåıXÓıuy1ˆ•™'›ñY‹ƒªz8ÉI‚Ih«K²ÇWºrŒóZ7s„0pù­Úæ=,ä5q÷làÁ ò»~÷bdnd!ÀÃñNWSq¹ØÏ©{X’MŞoÑ	fô8…ãù{då§¶3Õ%Î×B'ÃSñPÏ¡Q‰6i•2`%r·¡ëdÄÄ[Ü.¬¤ÿ‹4QGûßù#“ŠUuÊ1Ğ™œäxí©ãƒà©q¦œ¼ÉÂ‹%ÿêmºgÚA´E,t€ÏIhóĞˆk×‡Æ¹È!ïÛËª7zT!‘qà(G°|…|¢ºEe±H¾'qÃ:g¨©ÇõCµ ã)àä³Ê'j¦>\(§op£Šè~\å2îmäù ÿ9ØN‡ÈšÌŸİ\á÷åœ¦óm.ûrõdc^÷Wª.|`ù|›Ã›Q)Ñï|	ù÷”MüZÍErmîùR‡á6Ä_“Ay ¡Û8C¿·ĞBt0V1€lå<”‡µ£s+ìqõ-­*NÊ­¿ÖˆÇØóŸš«ü|1ŞFØŒ"†¯Ú=3ï,.À`X. 	SÙ8geCË±*üÁ {v9>Ó‹¢vÉ(u% €Ê§arKğË‹ãŞßÃDº«`uû²Vé$&Í4ØÌ©xÙãİ5R½S¢³²`¹Ò“—ûƒEàX³X$¹„‚h#ˆûØ+/<oÛæĞÇFŞõôï­fí	->|â–ø-³N4.uki¢˜H¼'yBÒ×şLÎ/«ä¿~ı4}—™iä×Æ÷TkêÕAéÃ6ÃÉÊÓc#A=ŒsÀÇµ*±Gı2İÁfl…÷˜y±6 aĞŒƒœı³ğ>ƒù™ı™—oZ<ò7¾;}<îãˆRŠ˜Ì/N[\0ˆL)uÙxŒøœª·éÔJ÷iWÆ-¾jtsç:ˆÿ³r® ?“ç_;'×ùZÃÛ³#ÖüœÂc´¾í¿E¿[Ó¿~Fj}.öØCªÒ¼CúğÃR˜&¥È#ÊhJ÷øb¾ÑÊBo¥Y¯éëZ=]Îo¹ÚÛIİ=¼&ü·å„úÁAz•k\¤‘†yáøéáåÎë­™ÙÉÎ}À-å°à
İ!”VaF‘27…c•í¡b¾~I£7X¡˜Ò€›İ»‡æZ+Pñ±+ªo0¨áÑ/â_ÈòÆJi_Ó$ğÀœB¡)C»#?ñKÂıe&§íİ]¸ÉOì¯¯8<{ÿh¦ñ³8¢qµ¢öx{¼–I¨;µ’R3¤Ö·L¶«+zÒ/_UBh&éùëÔú²öºö«µúxõhM½Â÷üä>£†vŞeÌ¡©¢¹NSrBŠ P¶Ñhæ»Ñ¡b¾2œ^²%ô¤BLÉDyşpŒ…Ãgâ@`šæDÊÇ–¯Á3åÂËÇBßm8K°Õ}­÷["—’êL–ŒÌ<™[Á°´j ?$§gAëÊ;‚ÍF IJñAŒÇÛˆ¿;QZD7å,Ç(†‹ÉÏ˜ˆ˜8*kàşİy_ƒÜ~ò@×ĞŠàŒV]U•:"
ıw!|,'¡)kîÚµV¡:ĞVTSq5œJ6*0Ç(í$ÕrY=“`§·–D¸+ĞéÕü¶J%5®˜XxÿgbÚY&såK(‰û¥4PØ–Œ1(¼]}K¢ÙÌm;kX¤V¬›y5­MmRS£¤YÈK?õç¦½_æYİ€=ÆÏ/úóÇŒ±(ßZ9H'Öu‰MÌ ]ÖôSŸUˆüØÚ°7jÌŒÚš[.‡8OôD~Q˜«2œEøÿ¬TZØ8 âé ’šOÚxù¯éOgŒ­5D¼ĞF_Á¡¬°î—´O¤áÌ–Ú|±»ş¦ó[ãç£Ø5Ì²¹ª­ïX6+g&åşà“N#æ”HÎ¢@„ºĞõÙ(&y»Ë¦m²_‹ó:†NÏ±†`¬„Ğ‹wr™DxEÉÁ¼¶!«/v˜ß¤j.Õ¦‰tmQ!ö÷„ğÃ‡¼Ç1íïÄÜ¸'‡p€%ÿàCÄ×BŞhÍuİ™5f¼oRÖ+ıª½ÑSI)Ôb×ÉëüâsnÇ¸X¿Øfí`Xóºd‡ÈîY&=™Ü®ÍÃŒú¢;b¬/–Ê„)‚œ¥%ş•±[#‡¿käİ¡>Õë„é‡ÖB8³e.µ7„v6bsŠ¥òÆÕhÃ_'ïãZøÍ³t.Éz*ñè–á@<Š*ö(Èß×–<½ÒxTöu€°İ_ÜÃ—ñ£Óê‘³ÜÂP6Ô2¼O”øÜ)€`|Ã{ÓŞĞ`é"¶¯}6À~Ê ğS"–L[ÈEmš4øg´Ï¨tói7•).j‡ñÌÄ[Ëş™‰OÆò>I™‘ªÔÕù8Æ¥–¿ãa…îÿòk5v)è…>½çîD„F2{bâ*=±V3‰E¦Ïİ4ùb­k’jß ë‹éKG~=rq›’¸Q?]š‹e!ªøÉ¾"“ê<¦Éo…Ò®;Ö#@²S3ëºúâ8÷aû7=wöÁO¸(;:iÀ×n·”i¿½.…FF‘Ä	ÿ¯ı$–§Ÿd[Wî)gú‹½¡ÄQŠ@áåÈ
{kE1»Â}$!;ç>âæš`¾šs3”oÃ›lj`R}¹–çˆä^n{›ãhË¦Ğ£_üÓZâjzí„ŸÅxá3¿ç‡CÇ&ó#ÇıÚsz6†ˆ±ßÊKÀf½¼½²ô:ÏËÓ»|>5Fzu(ªÏ_é²Ù×–Ó°Ş0H®¬ú¸¹r¡ìo¸âçó«ÔòËqãaº'lgíio¡6ß–Èñ‚× Iıf»Sº§Jøğ?Ê_Qj]Å‚ÆƒøÕ‚FØ;o­²pšjœ0[tz‚Ke«bµª9ä§Ñş{ÕSØ	® ÉŸk¥)³núKï${@½$½s
Ú0W%íVNEÖÀ3¥âË3ã®é•±Kø1¦K™ïVsÃ„È…°äÅ'ÅÑ¯T¾u{È9”Œú§;%*ĞTÅGó{õ¿‚8$g$%>œ^ê®‰µóÉ¸e¨{
¢ó ÙŞ¼- ±úš9¦í±¥âÉà’ä¡àsß±§Wc÷"ÑËß/JÙwF£•ªq
š-×Ô)÷
:HşYç[9ÄMx'³¢u]­æô/ÙxÑæ<$²#ÁÅäZù,!Ãõtà+šØa˜°™=]÷tŞ™Ô>µoS2¼æ¿˜¼aöjT¤[ë«”6K¦©ùèDE'Š«¯«Ûé™¬ğlQ±ÙÔë¸¤®$8¡ÂÑ8«*ŞwhÂq§-Á§!"'›#>˜V—`®¹8¼Q˜sjşšİ0ø#$àğÊ‰BÍ&Îÿ¯ªF#\Şß¹×½Ö›pšmÚğXm,ü¯ÉÔ³›· „–Ğjf+Ëîk {XÈj\×Oá<bá ºû^y“ZUa½PÌçKb™CP¶ÿ´'tàYÃ`PS#‚TršÍÇô-.tqÿÙÕøv4«hò·*—Ø³ÈÀL7ä&éIïSµÉÍ¼ áL#°ñN¾}¡bĞ²6×h€™ÕS:N” zŸ—J&g†ÈĞDpÓ&üôÂ<gê6By·KJú…5«úã.ô_(#2=€†Tğüõ3ş”`-¥öR FÃg¿ã¬Y]J°áèÒ(9²ÊˆØ4OqHºx)ÿ X)|…}i%ÊvüËe™šN—Îã×µJÌ6ÄÒ#"’÷TWXõ
Ÿ:M˜'¢u™5fæıäRí«XV;}ÂJÜŞtÜÊzÕí&´I„¹ûdÉ¢|ØõJÅ°Lwômi.2Š­öé+.Ë˜ ØØyz÷6gmûC9 ¦ü¤q‘™#®8<šîYãVŸÏ’iãÁ ~x0®ƒò–>æYzxh\ş§È£€À ­É+ÌëeCÖ>L1h8ß8ò‘¥£*Ç`â&ùy«lxh>!PˆaVBQù±¦…ÆÛHŒEÏXêQO2ÜO¢}Câïe–Ü“{Ò„¨Uª·Ğ¾£:Tdº½bò©2Íé<š•ëíeGX»`"Çw¹Z2Y3õáµOÖ¡­3?uŞÔºoÚÏğşgºZ¥·ŠÂÔMX]¤ïšáó¬§±[Ê×;–şXÈç*=¸²!:ïO¤4`ß«’jX¶Û›xNõ>eÛ‡•ÓjÇ/Šìá¨b"Åbs[rpçòìßGæÂÖdn/“ğ­+\¡ß¹N&V.a÷]:şÂ"½½Ÿ¨ŒÆ/nÓyÏÛÉ¯M^ÈfçDe»òu’½pZŠpx_¹L&ä+@PñìÒGõ;ËÅõ®™L"ÄTÄ‡SŒ âBYª+â¿í¼y ¾ÉiS1­ùí€•ÚV%W4¶>@ÈªK3¤))`£]ù'@×7VÛåĞÒqL"İşÃ–‡ê5Ì/Ó{Q‡/¨Ì8¨µÛ£¥û ‹Év‡÷dD¬NJß°–}Ôûõ£úYí¯µli½Ù±£Up*ª*(±n ®zwj/Z7Èí‘
1_‹¹Û2G@_½Ÿ«~[(VÃ.ıW&Ibƒs…FÒïÉ™	<@IPçŠd±*KR;íó£1rh©XC£t&Ï,ı4®¹ŒĞÏœE…!-„ı"äi@ÃáAø¯\Ej%"äl˜;"ïÉïâ}DyÿÊG™¼_D•	ƒßx‘^İW‰41ó«[“£°*èV¹ê2A^yÅÿ,f¤ ŸƒA…ÿşpnãú€·Ûí“(ÙµÒÈ²ßÉB\çK9yOñ˜	7ÚÉH`æÄ’¦_}yï%“çòŠw´‹Ó'$È`ßğÕ™ö1éŞRıì2*ÀÌÄ_¿_TN ç¤~›dlqdÊ06rù±ØÓï?2’é…"s^/r±=N¶ğ>kr¼F…#Ô>‡8{ñ—L‘´àÚõj9˜Öşƒê–FePŸGI y–æ†ö0NZIt®
ãUšjzáßá8Yğ3_ö C8@BÚ`Ù¢îï?3ôı¯Œ
$¦1ÛxÏºò?ü‰:{r_ø‹˜)$4Œ}âÉ] BtÀP €0øOß+¸MN‘Áf+SÁªÕlts]Ş@*+€Z±¶º6„éì¤°A
È!?Xÿ4İW‚¿‚2Ç/ÑÃUN§lDSô„vDH³ĞÄÆ…÷~*ÕV[5èÜDãV,îgi.!V£5ãñu1
ÂåZã’èúl‚|‚½¡xÌÌw£v——£Õi6ò¨?ÂÄ¸*Ç·2Cß›ıgsL•(Œ^ÒW
Òû‰âtN\" ×¾½û#<cY?tFÉ¬‰Ê…¤Û²ì,sQ{~0â4±ÏyÔNïÈÕyç|s±I,óH·—w4O_L¾¡]¢¡l§ÖšÖ©AÉGÔ:–ÅÛVyŠ;¦½6;­yşë-}ˆ&mvzÊãí„(‰Z:ÆÄo«r5ÀØà&ê”.éãpt`–®áÑğ^¤Ù?Üiû)Yt´BŞK´0%»&->§snÉÍŠ<‘y¢É\lï=ù#G`§y.Ì)ŒÃŸü-·á¹A]ö¡]ìkHÔ^ÀX×+è×b'Øı‚9 ¿b¦š/—‹êKLOT?—Á‚poQv«C6Ö<¤•£m™Òı ÍØò°#FA.H…Wé"ùÿHJ^/‹ñQ(tDê®O´Y6åå~Œ{•ÌÀ²ïfe§RËcÈÕ˜2?) ßîFFŞÒ‹‡¾N§'p‡Ş´ñj›/µ:&é9œ'»×ìIr”}¡æ·Ã€pÊnÁl™¬äñÿxG3.:Ó“O‘-e;Y³c®dÀõ¯Ç¥hš–‚Š˜1œ²ûÙ&Û¢:hb!¸&¼Íº5z`ú…¯y’Qd#¡à­E.-ß.5DÔ}L$]qQÎ¬ü>Ï\‹éÕÅŠ5Gx‘¨ÚÔ(>ïÜõØ_ú8ŠvĞ±d!6®e>g)¼Ë?1)êÒa®…©’,,t2Ks’·À:ÙYÂ{–*ô`}ò£3³PBPsZí1Ã‹¥b§
‹"¢ãİRï¯è%zRÁ×d±}Îçï_èü7…W^Ç|½BÿOiqIf¸ya„:Éï>G^z—œWhÉï8u ù‰0SÊãâEXMâpPœ×;gºÊD„òğ‘I|[TH­²zäÖkÛBW«ÿ{áfê¡a+í
šÀfÇN‚Uö=C¸0Ïsõâ«¡¸‰&? ËV8Œ³×ÖÀö›Çı	tˆÚî(´db£×9š8É*ŠÊ7SdwúúÑ@ã+ò‹”V`¸HMU$ÑšR2ÕÌip?õÆÄƒCÊÚ«|ÔÇn]N÷Hÿ$U’f<M6Z¦@Kæ„‰¸•í_ÆpÉÛó*ñĞ¢šK:%º<F»ş#¬%ZWk¢N £Á‚y®fˆÔÖÍÁ<.}©{*xşJ0š$éHêF[Kb«Ûğ²¤åá‚|Àbº@óÄ¦ï¶îHˆªl'¨#¢÷T¸«üpÛ1;WF£gëÍ!Í6²¶Ò"òáW"‹
*A´p†ÕïâÒö[šåÄe†Wä„ÉcĞè¬ÁS4êÚ—vDÒA8Á]ş uMóÔ
Û1MrŠ§ğŞËSk¤¶÷!o¯¾ZÛ¾EU® àâo}%m5E“:5ey±w¢NÄ‘*cølz‘˜TÓÁŸÜ¥	M"¿v!ßé®*ÄA$®J7A. ËTe*¹¡¼©åúÀˆsÇE*—Ó(WØÏ•ÈØi?.o:ºq”KBaaã³5½Ôı:‡ßz-Â´­Tì#3ãà34¨×A>(™ú1Õ­º†w×Bcä‘]#°ãfÎ°
f§´FDm¢œ÷›ğ"îˆQ²&;ĞçBšò»tpë•~UÎ^cñ¿‡ä`¹ñ³ã5:	iÓ¥VšTÅùÉøE¸%ÜZ_7®Jó !Ç¾³L©Y_¾(¨ÂÁ‚gx²Ú·"4Lÿ•¸	v’É}ù·A¸GóL°ßpïW$Ã2uÕçz¾˜û§(*éÉ‰hÑéü0$@êêà»WÜßJİÔ×À‚ÚFõ-3àº;	Ü÷ ˆÅ.ãõoÙSªvºj^^štı×B¡Œi=Ú=2ÄhÈ‰XMgÍVföËøü·ÃX7ämn¡\İ
P.™hb~¸N’S§”‘ëÂDÁr™†œçIÅ =ô5“TvWnÂirÔV\·¦úBRİóİ|ûQT°¬ıI3¤¢ÂãÏ>mû•ƒj#…¥5©ø
©¥œ%·°Ü2oÖ$Ê"9ÈÜ[àšÓJ9'.±5£ƒÊÑút¹·µ {²zÖFe¡a)¥š˜êAıdŸ>=÷Åãy[½ÙÎ±”ò“øZ\¼6×A>,ÁW+/Şzp- ùD-ÉY5õn]ÂÖdAÀW.:Ëƒ‹OS€
ûÃb#5Q$uñOŸ‹S©w]À‚Å„Í}ÑKó™xl%QÙVÉÇ^¤ójcóÃ‡¹j_)KmÙÈP¼
Á+T~€³!ÙWº¨½ ‘]×8q{\{ÅÆöá1¾úƒtÌ€ÓîÿˆV`Pì/$ûüïPhæêiıŞàBo’KqøóUlÁM©àSRŸú-b)î˜7‚LmT}æÁÁåO22á6èã Äªã´”ìõåeM±¤<WÚìèTVÚkÅœ¤3_Áktº]]Ü>Ğ/ñ¶¢'Ë+Åß—ÊŸ\9‘ıe¶V¢7ù4)
}„>4ø‰µò’Ä$YÑ<Ğè¶²ö×LäDù>¥½ñÁÉûÎïÁ0é¯F~ ˜(ö­¶s Ô»p.¶¾m©'=¬pÿy©ûõÍUë‡2åü,;‰O_Î‘*YµÒÏv`Gİr’?ãÂrèê)üÑÒõçË˜órO¿ó©/åé€éû€Ü¹.³_Ô¹ññfˆöEØQÌŒÕüüYıL'‹@U Aià_ïvØ!ÏyJT °„dÅß¶„ó=á=¡œ`€VÖéj®¯Ì™^£ÖÖ…¯Lè†¯Å7Y¤†{MÀ*‹‹lÃb"LàwuLã =ïTQ,Ç=ÃğÒámj€6kYµ¾óİ·2—”Hnˆ©ôÆeRhòÌP(ÇËê=wĞ^D¯¦îì|(İöğaSÏ|(œ9İ$ƒckQÎ
ñ¶Éƒ¬8İCŸmïRüßáÉæ€¼UÉW-·1M‹‘õßy
È=ZcõŞ¦úay½y`’÷e†$°Cí©ÆÄ>(×wğ(Û$5»mcYe£`ã˜ŠÌ'õãã×WmnûOmeÊ˜Û•_-¨Ìã÷‚¸âqiĞ!€MU€ÿ~vœƒx€óòİo`G'#«’­«»şF¥ÔAöA»»Y€IñõF,¾±‡òù#W6ÁyV¿ñÙMIVî*İZò¾+Hù)CIx–†2Ğô…‘^öªI8dğ†R,HµjTúñaÍP!Å sÖÉ2„Ye{GKm ”9:7Š½ÉYÿít93vª¨&Š,¶évDŒaÙLÔ(g	Í¬Ë¸ü»eå1Ô ‰Æ°sıñ.¸+\±ïŸñ@L¸ªæŸãˆš÷0:RÊÇÉl;¦1ƒr/“ÒÖáû3çlë§/‹'‡€0<ïï‘(]¬&	¨ iõ‡ôç	…uKü+÷'İxfrCbBÖîûòÍZrj`µÎpÖÁÁu¤²üXcƒZK6ÁŸKlÙ»ybŸÈä©¬$¥Îeñe‘B Íú§Š!…»L²ÏGæ¶ì¦‚­òq²`‚wÈ]Ë_[—Vå‘_7Ú–(×>ÎT‚³fü[8Uh_`³‘‚ÙOÛdlÄ`¢ÁÁ†ƒ«íY´¯z”ŸDx€ßÕ0ˆ'…ÏBù¸¢2’+İdg™Añ¾<ÿa‚ï¿n`&#ì»90qtßå"ªÛ]°?¸—éøD ú:ëŞ àë>±ù²tŒo0\*õÆ‰Ğ­¨V¨ë¥¾a’øÛyQ|ñ	®\>C-§¸ªàö²,Àµÿ¸&bfÂA-ô«ƒ¹v8a«G!ü‰C_˜µå¤şèFj2Bš{_
ó[Èú4ŠDÅÍ5üÂ{’ŞDÛÚE¾PZÚñK×œyú>Íø_zØ&ôíJĞ2w½(7à¹Ğíù`IŒeÚeĞk…]JRû9ùx¨tÈël9BÓ/.í´}Û
èÄ&oï¨Ğ·g ±èË³¬¬¼Ù&«-ãŞÏö®[ ;‰•A¶Ê§šCxŸôÁzH¡
ı¾„¥J‚Ñy@ãQ˜”ÈÇjòÄÅEÊ³~Mºàª“ø¥…PÚpÂ@ÔMè0Òş G‹âë³t@ø±m»ÎØ°İe_5rFÏ`x›PŞ8[ê`L=@™'ŠÏÉŞÇÔâ«æøZD–ªkŞÆ6ì×ò±í¾—ÒZ÷Íx{êZı2yw³{Zd©¬ZôH-lƒ§˜ÇêQ …vÑ®ˆ,Ø•£ûówğú¥-»j›äY¼?®\;İg‰X•õÊD£æiË'K¹ù°`¥‹öÄE¬±‹àm¦'ÆtÏ*,çUä}&ÿ$	{! *bÉÛƒûFÌ.‡OV/Ó7È~»r0°lë# åäìWu% [c¸O–×§=ÍR]^ÿÕÜÅ5r°aà4jÅw$ÖÀ˜ë‚ìÄ ÿÛœfî4üY.PÃn_I©]’ò=†Jmlà•,a›#~àxnì°¹yÂ"Ï'Š"Yóã¸CJÅ­b1R°Õ `Ê¶‘L†U„Åtxf‰“0vIŠmµƒ¸øX2/¼ôÇ]>ÿëH×Àõ—×Ï†MÑø:a|†ˆ}÷Ác¾¸úŸéKJôøwËŒ)‡ù¦F~R†ßZºÙC&3uD³œAU6ŞOÜ¢=àUóÇÕë‚zÄ6[„ ÎÌJ²kƒ<‘ä¼7°m-[6ÓÿnÒ_5Îµkú‚CåiLx¨¬fşáH;Œ¤}0Ğá—b~#˜²xTÌ£-L¸|±Wœi>—‡¿°ªíŠDÄÆPÆ'Ç×Û×`Øi‘+¯9S<ö}±Ì†ê¯\[¢R±«o¢2 „ù:e½OÎ¯u]ï1‹Ë€\´>ü›T^%‚)½w‰ÿÇy+<Õôšô©«ìN²ˆÊµY
„ŸûõB¶q¡…ÿ˜Aé
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
  include/uapi/linuxELF                       j      4     (               èüÿÿÿS‹˜X  ‹ˆT  1À‰Z[‰
Ãt& èüÿÿÿ‹€|  ÇAA	  ‰1ÀÃ´&    fèüÿÿÿ‹…Òu‹A…ÀuÇA   1ÀÃt& ¸êÿÿÿÃ´&    v èüÿÿÿƒ9w6y   u-‹D  ‰Q‰Q‹H  Ñê‰Q‹€H  Ñè‰A1ÀÃ´&    ¸êÿÿÿÃ´&    v èüÿÿÿúŒ   vŠPÿÿÿƒùw ¸ Á ÓèƒàÃt& úƒ   —ÀÃ¶    1ÀÃt& èüÿÿÿ‹P@‹‚P  …ÀuÃt& ‹‚L  j ¹@   ºÁ   j j j@èüÿÿÿƒÄÃ´&    èüÿÿÿ‹P@‹‚P  …ÀuÃt& ‹‚L  j ¹@   ºÁ   j j j èüÿÿÿƒÄÃ´&    èüÿÿÿƒì‹€L  ¶Òd‹    ‰L$1É‰áèüÿÿÿ…ÀI$‹T$d+    uƒÄÃèüÿÿÿ´&    ´&    èüÿÿÿUWVS‰Ã‰Ğ‰Ê‰Á€õ@ƒì	Ñt}‰Á€õ	Ñt<‰Æ‰×Æ şÿÿƒ×ÿæÿıÿÿ‰ù	ñtk‰Áá °  uq„Àu}%  ÿ ‰Ç÷ßÉƒá‰Èëv ¹   ¸   ‹    …Òüÿÿÿ‹ƒL  º(   èüÿÿÿ1ÀƒÄ[^_]Ã¶    ¹
   ¸
   ëÆt& ¹   ¸   ë¶t& ¹   ¸   ë¦t& ¹   ¸   ë–´&    t& èüÿÿÿWVS‹˜X  ‹°T  1Ë1Ö	ótd‹°È  ‹˜Ä  …ötD‹{‹s!Î!×‰ó	ûtJ‰Ó‰¸T  ã ù  ‰°X  ƒûÛƒã`Ãà  ‰˜H  [^_é´şÿÿt& ¿ÿÿÿ 1öë¹´&    1À[^_Ã¸êÿÿÿëõv èüÿÿÿ…ÉtGƒyuAS‹˜D  ‰Y‹H  ÇA   ÑêÇA   ‰QÇA   ‹    …É!   1À[Ãt& ¸êÿÿÿÃ´&    v èüÿÿÿ‹€    º   èüÿÿÿ1ÀÃ´&    èüÿÿÿWVS‰Ó‹    …Òİ   {‰Ù1ÀÇ    ƒçüÇCp    ºÈ  )ùƒÁtÁéó«fƒÂ$úœ  t/‹zü…ÿtî‹‹J‰ş9Èwv f	tCƒÀ9ÈvôƒÂ$f	;úœ  uÑ[1À^_ÃèüÿÿÿS‹ƒûtƒûu[éüÿÿÿ´&    [éüÿÿÿ´&    v ¸êÿÿÿ[Ã´&    fèüÿÿÿ‹    U‰ÅWVS‹x\…Òú   ‹‡È  ·   1Û…ÀtJf‰ğƒÃÆŒ   èüÿÿÿ‹‡È  9Øwæ…Àt*·@  1Û´&    v ‰ğƒÃÆŒ   èüÿÿÿ9ŸÈ  wè‰øƒÅèüÿÿÿ‡Ì  èüÿÿÿ‰èº   èüÿÿÿ‰èº   èüÿÿÿ1À[^_]Ãt& èüÿÿÿV‰ÆSzGREY‰Ót<ƒ{t1À[^Ãƒ{uô‹†L  1Éº   èüÿÿÿ‹†L  ¹   º   èüÿÿÿëË‹€L  ¹p   º   èüÿÿÿë­´&    fèüÿÿÿU‰ÅWVSƒìfƒ: ‰$„æ   1Ûv ‹$1Éf‰LX2·|X‹…T  ‰Âòÿÿÿ •X  „  ©ÿÿ sDó1ÉFú‰D$ƒøwl¸È  1ÉƒÀ$ºœ  9Ât#‹Pü…Òtí…×t	;0r;pvƒÀ$ºœ  ƒÁ9Âuİ‹|$‰Î‹…L  Îğ   ÿ‰ñWĞ¶Òèüÿÿÿ‹…L  WÑ‰ñ¶Òèüÿÿÿ‰ñ‹$f‰LXƒÃƒû…>ÿÿÿ‹…L  ¹   ºÍ   èüÿÿÿ1ÀƒÄ[^_]Ã‹€L  1ÉºÍ   »Ğ   èüÿÿÿ‹…L  1ÉºÏ   èüÿÿÿ¶    ‹…L  ‰Ú¹ÿ   ƒÃèüÿÿÿûü   uãƒÄ1À[^_]Ã´&    fèüÿÿÿVS‰Óƒì‹²L  ºÀ   d¡    ‰D$1ÀL$Ç$    ‰ğÇD$    èüÿÿÿ‹L$…Éuk‰áº‡   ‰ğèüÿÿÿ‹$…Àu&‹D$d+    …°   ƒÄ¸   [^Ã´&    v L$º†   ‰ğÇD$    èüÿÿÿ‹L$…Étºº   ‰ğèüÿÿÿë¬fºÀ   ‰ğèüÿÿÿ‹D$¨@t–‹    %€   •ƒt  …Ò7  ‰Øº@  èüÿÿÿ1À€»t   t‹ƒl  j ¹   º   j j P‰ğèüÿÿÿƒÄé?ÿÿÿèüÿÿÿèüÿÿÿS‹˜    º   ‰Øèüÿÿÿ1Ò…Àx
‰Ğ[Ã¶    ‰Â‹ƒô   …ÀtêHÿğ±‹ô   uï‰Ğ[Ã´&    t& èüÿÿÿVS‰Ã·€v  f=PQ„¶   1öf=QQ„‚   ‹ƒd  …Àt‰òƒÎƒÊƒøDò‹    …Òd  ‹ƒL  1Éº   èüÿÿÿ‹ƒL  ‰ñ1Òèüÿÿÿº@   ¸   ƒ»d  EÂ‹›L  j ¹P   j º   j P‰ØèüÿÿÿƒÄ[^Ãv f»x   …oÿÿÿ‹‹p  1À…É”À4Å    éVÿÿÿ1öf»x   …EÿÿÿëÔ´&    ¶    èüÿÿÿ‹P(‹Hú	˜ tmv;ú	˜ „   ú	Ÿ …«   ‹@|…À”À¶À‰¤   4ıÿÿèÇşÿÿ1ÀÃt& ú 	˜ thú	˜ ux‹P|‹€   ‰Ñº   èüÿÿÿ1ÀÃ´&    ‹P|‹€   ‰Ñº
   èüÿÿÿ1ÀÃ´&    ‹P|‹€   ‰Ñº   èüÿÿÿ1ÀÃ´&    ‹P|‹€   ‰Ñº	   èüÿÿÿ1ÀÃ¸êÿÿÿÃfèüÿÿÿ‰d  1Òƒù•Â‰ˆh  ‰p  èşÿÿ1ÀÃ´&    fèüÿÿÿU‰ÅW1ÿV°,     Sƒì‹È  ‰D$…ÛuéZ  fƒÇÆŒ   9½È  †C  iÇŒ   1Ò^N`è‹€,  9ğDÂ‹Vü‹@ÇF|   ‰$º   ‰D$1À…ÿ”À‰D$‰Øèüÿÿÿ…Àˆ­   ‹E`‰Ú‹@èüÿÿÿ…Àˆ˜   ÿt$·D$‰é1ÒP‰ØèüÿÿÿZY…Àx}ƒ<$tO…ÿ…_ÿÿÿ‹T$1À…Ò•À‰$‹$‰…d  ‰èèıÿÿ‹D$‹•8  ‹<  ‰…Ä  ‰èè4÷ÿÿéÿÿÿ´&    ÿt$‹F‰é1Ò·@P‰ØèüÿÿÿY[…Àx…ÿt¦éòşÿÿf‰Â‹…È  …Àt.µ@  1Û‰×´&    v ‰ğƒÃÆŒ   èüÿÿÿ9È  wè‰úƒÄ‰Ğ[^_]Ãt& ƒÄ1Ò[‰Ğ^_]Ã´&    t& èüÿÿÿUWVS‰Ãƒì·Bf‰$‹Bƒà…‹  ‰Ö·$‰Ïƒò·Ê‰L$‹‹È  …É„~  ‹o“@  ëv ƒÀÂŒ   9È„_  9ÕuëiÀŒ   ‹    ‹„(  …É‘  ƒø…@  ½   “À   ‰Ğ‰$èüÿÿÿ…Àt!‹$èüÿÿÿ…À„X  Ç@$    ‹@ Ç@$    “à   ‰Ğ‰$èüÿÿÿ…Àt!‹$èüÿÿÿ…À„#  Ç@$    ‹@ Ç@$    ‹D$$‰éƒà‰Âƒò„Éu~·N‰“h  ‰‹d  …Àt~Çƒp     ‰Øè1ûÿÿ‹“È  …ÒttG”‹“T  ‰ƒÄ  ‹O¬‹ƒT  ‹o°1ÿ!Ê1ĞƒX  ‰ÖtI…Òu‰Î‰ï‰ò‰ù‰Øè)õÿÿ1ÀƒÄ[^_]Ã´&    Çƒd     ‰“h  …Àu4Çƒp      ‰Øè³úÿÿ1ÀƒÄ[^_]Ã´&    f¡    …ÀM1íé¾şÿÿÇƒp     ‰Øèúÿÿ‹T$‰øƒÂÁâÚèüÿÿÿ…Àt‹t$$‰p$‹@ ‰p$é%ÿÿÿ¸êÿÿÿëéµ  éW  t& èüÿÿÿU‰Å1ÉWVS»À  ƒì‹°L  1À¶Ğ‰÷ët& ¶K‹½L  ‰øƒÃèüÿÿÿ¶€úÿuä‹•P  …Ò„  1Éº   ‰ğèüÿÿÿ¹   ºÂ   ‰ğèüÿÿÿ¹   º   ‰ğèüÿÿÿ‹µL  1ÉºÏ   »Ğ   ‰ğèüÿÿÿt& ‰Ú¹ÿ   ‰ğƒÃèüÿÿÿûü   uç»Ğ  ‰,${$û€  tt& ‹Gô…Àu1‰û{$û€  uì‹,$‰èèRùÿÿ…Ì  èüÿÿÿ1ÀƒÄ[^_]Ãt& ·oğºÅ   ‰ğƒÃ‰éfÁé·Éèüÿÿÿ·ÍºÄ   ‰ğèüÿÿÿt& ¶KºÃ   ‰ğƒÃèüÿÿÿ9ûué‰ûë†¹   º   ‰ğèüÿÿÿ¹   ºÂ   ‰ğèüÿÿÿ1Éº   ‰ğèüÿÿÿéúşÿÿ´&    èüÿÿÿU‰ÅWVS1Ûƒì‰T$d¡    ‰D$‰ĞxHÇ@    ƒçüÇ@^    1À)ùƒÁ`Áéó«ÇD$    ëCv ‹D$…Àˆ“  ƒàƒø
À4…Ä  ƒÇ;<$uX‰ğ·ö	t$‹L$f‰DYƒÃƒût`‹…T  ‰Âòÿÿÿ •X  „³  ©ÿÿ SDÓ1ÀƒêƒúwÀTĞ1ö¶úG‰$‹…L  L$‰úèüÿÿÿ…À‰iÿÿÿé“  t& ‰È·L$f‰‹D$d+    u
ƒÄ1À[^_]Ãèüÿÿÿ´&    v èüÿÿÿS‰Óƒì‹€L  d‹    ‰T$1Ò‰áºˆ   èüÿÿÿº    …ÀI$ƒàƒø¸ÿÿ  EÂ‰C<‹D$d+    uƒÄ1À[ÃèüÿÿÿèüÿÿÿVS‰Ãƒì‹³    d¡    ‰D$1À…Ò„¢   º   ‰ğèüÿÿÿ‰Â…Àˆ$  ¹   º   ¾”  ëv ¶N‹ƒL  ƒÆèüÿÿÿ¶€úÿuæ‹“T  ‹‹X  ‰Ğ5ÿÿÿ 	È„û   ‰Øèğÿÿ‹ƒ|  ƒø„•   ƒø„|   ‹ƒP  …Àub‹³l  º@  ‰Øèüÿÿÿë¶    ‰ğº   1öèüÿÿÿj ‹ƒL  º   ¹   j j Vèüÿÿÿ1ÒƒÄ‹D$d+    …»   ƒÄ‰Ğ[^Ã1ö€»t   t™ë‘v Çƒl  	   éuÿÿÿj ‹ƒL  ¹   º   j j j èüÿÿÿÇƒl     ƒÄéEÿÿÿ‹†ô   …ÀtHÿğ±ô   „|ÿÿÿëév ‹ƒL  ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
v1Ò1ÉéÙşÿÿt& ‹Å    ‹Å   éÂşÿÿèüÿÿÿ´&    èüÿÿÿV‰ÖS‰Ãƒìd¡    ‰D$1À‹ƒP  …Àt,¶ƒt  …ÀuF1À1Ò‰‰V‹D$d+    ucƒÄ1À[^Ãv ‹ƒL  ‰áºˆ   èüÿÿÿ‹$ƒàƒø”À¶À…Àtº‹ƒL  ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
w—‹Å   ‹Å    ë‹èüÿÿÿv èüÿÿÿVS‰Ãƒìd¡    ‰D$‹ƒT  5ÿÿÿ ƒX  t‹D$d+    …á   ƒÄ1À[^Ã¶    ‹ƒL  ‰áºŒ   èüÿÿÿ‹‹\  ‹“`  …ÀI$ƒàƒèƒø
vI	Êt®Çƒ\      Çƒ`      Çƒ@      Çƒ<      ÇƒD  Ğ  ÇƒH  @  émÿÿÿt& ‹4Å    ‹Å   1ñ1Â	Ê„Oÿÿÿ‰³\  æ ù  ‰ƒ`  Çƒ@      Çƒ<      ÇƒD  Ğ  t¡ÇƒH  à  éÿÿÿèüÿÿÿèüÿÿÿU1ÒWVS‰Ãƒìd¡    ‰D$‹ƒL  L$èüÿÿÿ…ÀˆĞ  éÌ  ´&    v èüÿÿÿUW‰ÇVSƒìd¡    ‰D$‹G‹P‹Rèüÿÿÿ%   =   …š  o¹   º  ‰èèüÿÿÿ‰Ã= ğÿÿ‡Y  …Àt1Òèüÿÿÿ¸   èüÿÿÿ¹   º  ‰èèüÿÿÿ‰Ã= ğÿÿ‡&  …Àt'¸Ä	  èüÿÿÿ1Ò‰Øèüÿÿÿ¹   ºú   ¸È   èüÿÿÿ¹À  º€  ‰èèüÿÿÿ‰Æ…À„  j 1Éº€   ‰øèüÿÿÿ[‰Ã= ğÿÿ‡Ê   ‰†L  ¹À  ‰ú‰ğèüÿÿÿº   ‰ğƒN\À   ÇFh  Ç†|     ÇF8  ÇF  Ç†Ü      Ç†Ø      Ç†ü      Ç†ø      Ç†     Ç†     èüÿÿÿ‰Ã…Àu?‹†”   º€   ‰$‹†L  jL$èüÿÿÿY‰Ã…À‰$  éı  ´&    …Û„ışÿÿ‹D$d+    uƒÄ‰Ø[^_]Ã»ûÿÿÿëáèüÿÿÿ»ôÿÿÿëÕ´&    ¶    èüÿÿÿUWV‰ÆSƒì‹Yd¡    ‰D$1À…Û…«  ¡    ‰×‰Ë…Àé  ‹k1À‹S…íHè¸ÿ  9ÅOè1À…ÒHĞ¸   ‰k9ÂOĞ‰S‹†T  ‰Áñÿÿÿ X  „š  % ù  ƒøÀƒà`à  )Ğj ¹Ğ  ºÑ   j )ê)ékPƒèPCP‰èjèüÿÿÿ‹‹CƒÄ…Ò„  ¸êÿÿÿƒút‹T$d+    …  ƒÄ[^_]Ãt& ¾<  ÿ ğÿÿ‡õ  ‹†D  9C„O  ‹T  ‹†L  ‰Êòÿÿÿ –X  „g  €åù„  ½à  ‹Kº   èüÿÿÿ‹K‹†L  º   K)éèüÿÿÿ‹K‹†L  º   Áùèüÿÿÿ‹K‹†L  º   ƒÉèüÿÿÿ‹S‹CŒşÿÿ‹†L  º   Áéèüÿÿÿ‹S‹CŒşÿÿ‹†L  º   èüÿÿÿë$·N 9Èƒì   €Áâz0ÿ ğÿÿ‡  ‹C‰‹C‰G‹C‰G‹C‰G1ÀéÂşÿÿ´&    ‹†L  ‰áºŒ   èüÿÿÿ‹S‹k…ÀI$ƒàHÿ¸@  ƒù
‡Fşÿÿ‹Í    é(şÿÿ´&    v ‹†H  9C…¢şÿÿ‹†@  9C…“şÿÿ‹†<  9C…„şÿÿéaÿÿÿ¶    ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
v+‹†L  ½@  ézşÿÿv éÿÿÿ¸êÿÿÿéùıÿÿ¶    ‹Å    ‹†L  é?şÿÿèüÿÿÿ‰øéÕıÿÿfèüÿÿÿV‰ÆS‰Ëƒì‹Id¡    ‰D$1À…Éto¸êÿÿÿƒùuEÇC    ÇC    ÇCĞ  ‹–T  ‰Ğ5ÿÿÿ †X  „‡   €æù„¡   ÇCà  1Àt& ‹T$d+    …Ã   ƒÄ[^Ã´&    f‹‹K…Àtƒøu2†<  = ğÿÿwÅ‹‰S‹P‰S‹P‰S‹@‰C1Àëª´&    v ¸êÿÿÿë™´&    f‹†L  ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
v-ÇC@  1Àé_ÿÿÿ´&    ·v 9ñs ‰ÁàƒÀ0étÿÿÿ‹Å  