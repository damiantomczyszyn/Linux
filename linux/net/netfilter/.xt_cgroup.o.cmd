.h \
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
  arch/x86/includeJ ¬aèŒ|Q¥éG1D„¬)öñUŸA66èï’şvy–7û¥¡ärU¯(fñ¶«ä—Û6±Nmÿ)Úã$4œX°åÏCìœYÚ+ŠÖ…hhúè°µÏ Š@»€^´‹¯7ö÷-·êOg×ŠP½VIçe	¡;º%[N=r\(ÎÒU`àïÁ¦ø}¤0¾ÂNmfÓ2À%®À:şÀd_lÀ&˜İ§Ôv*^æó0&?ği—†nA-ÑR`ÌnËğçà};S"ŞÆ—º:èS­à*‹
·‹¦îu¹µ¨»Äk"Xä?ÓúmtÙå¥Œ{PHÌˆiÅ¸G«lı˜Ï¨ÄRÙ4Qª…®Jq½`5ªO€~dû“®¤Å>dÙ?s&6Ê~K 0áï’˜¿ƒ¢'xI¡nf'Œú–­Éğº9}ğ‹¶çCò¤Í!£å¾²öÆEve]Äq¾àLGeméa8f P«S&;ÄM•1À$-öl l t˜H¦wAĞ4¥9…#•Ù•µîŸTÜöÔîb÷D¨ŸZ‚GaŠ0e5­ı1"–ˆı5_]¥ß§®é—,^iÍ\ÀÈ¼¾Fı˜NÙbÇ`fÌÚô¼¿4Ú‰Q=2ÃYêáÅ%AaÓñ†>³½)v]Vd^Í)¿€8¾)){˜ùÁÜìíœ, b½"wØGfãñ:Bµ9ÊÉ…e‚3§Néñ¸‡´¨J“­NCöğËHÓy|M£-İáŸİiÍ<Ù !±\òÙÉ×İÃª*UpcÄ‚
TÇ•Ìıg(Í`Ø2I9ıHe½´@NŒ]ûÍæ\Hjz×à«µ3œ”:	†ĞãúĞ€^ÕJÎ+Myh Ç:Ş{ÕnºoºŠgö1Llvr;›ıõÈş¬-¡·]Œ‘¯2İÎº§Âã©ÏÖ/.æ´IÂş-ÊRá!‚‘¢1ğ³•ìŠÁ›é;å–çŠ›³™#­Ùh×{æ–Vçøæ?l7IpÁB®{8†P +X³Lõõ‡J8ªTîœÜˆİ63ç–ÇÓã3¸”+¸æ»ñÛÂµ¡œšÓ:=:cá;-Ét»¯Ÿj]$Iº© $Éâ!OËOB‚¡vòs-¡ªa¢©Î@t^’™	-æZ%èTÈü·ttˆºP^˜Õx¶?B¨/{ÕÛcê8Åfı’jmŞ˜eˆ#²&ì‰´sO£Î$Ó–—ïv/9<áµ’gª€æmâ¦ĞŠ÷Ê4,9ís4•„?V	Èçv˜ij°#ˆºéf;*LPUv‘n§ïe}í¥öâD—ŸD.8å!vÓ=ÅZ_8ˆ2õÉ<	Z*cyÜTÈä÷8àÑ¬ãtE^™k ÿ­€+ä´ñ_h'²Ònkæ…U²£nm˜ôÄ©ÛWÍ¥¤zÒê™íåc‰²ºúÏl¨ŞbóhJ=V#ÀŠõ3Ñp’Œ÷^¸$\î‰hpUÒIW$.=Úé0—ˆ™ø^ñRÖK&ÃbúıÂ¯^ÇZ0˜\@ûÒ±œşÇ¼†Ë*5œ-@Üğ×œì7š¬~¥÷ÌG1¤Ğ*Îğ§ÿjBı‡İ_X¯˜‘ô`L{TÃŒ^áL¸}E™mÕL*òB¿Úï“ûÌ^áò½sPÿ.mÆå„x³Rtf.ƒê„ÀŸî_D¦	&r>íÿ²š:rñ\×­®í2[&†RB“qå´;t4æÚKZÑ”pø^©ulf^LCÓ~şU ?k $>X‹\˜G£ “K©e§ÎzşLB=.B×`zhƒ^¢¸½k¨?â7YmùPBeOÈ”˜—z>UŠZÜ²$úˆo~cTG/·Ü½ç<v8›—÷DbÿãÄC	ûÄdùg¼í…Ú*§½«Àˆê¥»±W>jcSìÊÄ„XhmE†åïÙ®ìóÛd¤ òw>B[ß%…!m¢Ğ‘²œIk‡±ŞèÆF¨şL”_Ê¨û§¬³KîçyîH´W¤;àBË—“Ô]Oœ‘€’’RV1)Ü":TÙÃáÿïRšKô[gûH*ì¼Ñm¥¼ÂÅëW‡PÑWpÍñ¨o‡¿[n20"&™İYö²õ˜°¤FHƒÜÏ:b°Û¼ùÔ<´1Qøçñ(|Ål.oN»Í5…¬§Š€ıEëà—ÊêÀDA‹ä3‹¤È$v_BøL½B%D—Q‘v`ÇJ@†FÆ¥Âåà¬£Š T“äg=&çàyÑŸC¡_‡t:tëF×Ÿ“xXÎÖ÷QØ=²)!·LËJS £ÿ^8rÍ˜Cp›Úô ß¾RtyhL`fíh¥±öp+"™—Ÿw-Kã@D9l•+
Î£ÔœÅ£H²eôø½¹üº3(I}@e[OØz‡MŸTsÁjÎá»ßKÕ¤–ßa5n=¡<=iöòƒDXòRy%®²ı¹·Ïİåiló°tÂ„äqR0Çø2S™†$®s~#§Ö'”ş?¥™sî5ä·}Cp^R£y²„O¾c-3#°¤â÷Ñ…yXzmqØdÎTX·UÃXb$VSq¹Ô ¨gk_LOÀ­Ô¶—ùX³i!oGÂíuBªX†¨÷Ú¨5’f’uõê…M¥“ÀT[f´ĞmPu2½•'êÎf@L|E³‚°d¨ÃX€i—wU‹wÆ}9*Y'ùU]Ñ¦ıÌ‰Şv>nÏTÆá^ŠKJÃ¿ø°üH I¹úb×ZÇ<(¸NRñA]Ì¸S,År)ŸÅ³PêTš'Ç†à½³Ü?‹äÕî.ˆ°QQV–dKò©k…b€İšaåµœã—5Ë†DıSãˆ±—Y…Các.ó°Û€Bƒ&u*Ñÿ& ¿%Z3§S(Á\ÃÜ±`,ÎZÛÆïÚÍ£êTg#Úb¯á¥Ö=
›#ÑÄstÏº‡fv$Ägj³TNm9²7¬n¡48'wrø |Š=yº_ÇÍtº2úYrË8(Ÿ o[ÇIC/ pVøú¡_î¥Ê/ßjLqwğ›àGüÄ
Š6XnèäÃ°o´ˆhÒŠìÙÂ©n(/H6ÃïÄf	ŸF­_nV€ ”/x‡~
úsàa,IÓqùÛšõôæµoüÒ)ÓECo6UİÒ=©ÈKŒ—*F~q­Âhúk™€U¥°ØÉ€SıUz{8±‚êšc>Xfvíb—gßVˆ*¼@Ïu–åM´Væ¦õŸw^‰Q&²'Ğ»Ì©Uhd	"á‘Õe˜3v£# ¥Ìop_Ó wG#§Îi”·ööù H‰Á†hà)<Bì®F‹ Ü¶Y{¤{o}ÄğêöÏØoôˆ(„†ø$ŒõüÓ1Z¨nX§–ÄÆ=Vß8˜€Ú¼]¿ı$»B|ìT&«èÇk}¹ÿO\ç0Ğ&|­0¾»—c)ÈPÈoJ•î¥0P‡|ø•ÕXC S«i…H¾¥Wàæ¦ğ$¦óôIû­·°+Ä‚sb–¡nÈóøí?ÏZı°*·U+í…°y§ş½ñÓ¬™÷»¿DK®Å=§Ubïµ=,ıôO)¶Ş›
l±µ@­¡~³ê‰ó¯K¤¬9NÌÖçnHŞnËñ"ú„Çà›ÌƒO âx/„*ƒ/÷¡iûO‘ş£…¼®Zâ·ÊèdîÛnªQ’G˜Z–›=¢a÷†‚sÕR½¯Š®öâ p´ù$5n«û YŸ¯u›6ÉÖ©ïª"ûËÜ„Ñ“ª]ã]—½±¤×Ïÿ…ªq"bvZúÌ°G[±)W9¬ˆa€×i:Æz%ñ¯È•Z«åL`çe–¥;W>¯² lj¬¹” y5£O²¼XÔH÷Û<Ó2Œ¶ gsşÊ:xÕ„H‚ŞÄĞ]“8}¥‡pØ/¹¾{˜œæUîÇå¾tñ«”&+KY³…A©M•{ú¯ú&˜½aØJmhàê>ô¾‘¥Œ0oKÉl)½<ùĞì­ü¿\R£@ĞZŸ†9Ÿm@=ÛO}ŸqÃéqà(ş_X¥2î§*[wÄ´ŞI×äÍwäÁX ªtt‚í\¾ATDÁñnŠ2{«ïĞâ²ÎRH!ëM˜!ş2Ô3“”Óƒ9†ğ§í gé-JlÏ´Œó€wÔâk³×¼-Ó x ìğle„…#®¡`‹
q‰[‹ƒİÓÇ°€gesæâ±“JîÍ­Ãn9±™E!¼æ\i¼©$(øÉNúlÁGÔNI‡ŸúWß^#ÈôÖf}Â Qzë€„ÇüYrtÏmæF9ğtŞ‚—ù–WéIƒöùièKj?!µø)Ho×k!—0Ãœ«æA˜E®Z|Ï§Ìg¯?sfQ±ì—mâsOmÉ	/ÃÄ’)fËuqqEƒq:XÁ'ÇTŠWoÅ×Ù[úÂØÌ|Ü}¨½Ç
’AŒMÆ¼’Î–3k¦Ü†Èûò'
{“H9}c+ô*#Ã¹_ß3Œœ„5à­¬u~¸1‡è•–Ø{ü0NÒ¶¯ıa¯ï?‹ã=UœxH™½`åà:ÂÜñ­ÁHäÖÍüwË­õ#.½ÎC(¿ôo–1»W´€ì£”iÓ¦Cªnªjiš·èÃ*¦#×ıñÂ´êõHı©rYÔ$tíÀ/l½²¬Dö1Pp@£Êsf
Ô%íl[*üâæ€XFUGUˆsP+Óçe/#é³m23@vt›î´¼ ØôÙÂÄşÙ:aHÛÎŞÈ¨µt’>=¾ ®Ã·Ç]o‡†÷û]ño—(Ã/"99³iãœ5©$¹CÌ‘Ü(ç¼±ŸÌpßPxÏÑâ÷ÊFM-‚áWPŒj‡K¯“àó)ÂUé‹eOz­£ÂŸ·€·v¼M³Y®uŒdÜh5ÅG²ÊUëááÿJúä()–ú…ØÓpPy$+F‹IWSß¦ÑĞ]s^€ë>>ŸŞé FíVCë•rm,Iºabİôúb&™/"Ë¯U|gµë"³İÆ£Bò‡c%Ó
Q›²¾„]åŞ[Qúİ	ìÔİ@üR“€ˆ$«'ÀD³¹ÙIÎ¼ HÎ«{÷áø7Ğ±É<AÉ½: Ï¹Ø6Äõäàf6{è°­­Ÿ4¼c±‰'¸‘{Rßm’DhöÕ•I|vÎªé]	?™ª‹Sµ§mìqÁh¿¿E|ş½â÷é.UäîÆ”±/r&_,
Á•…êÉßwÃ¾D-å†è6k<e™HüÂ#rx^nı“÷š³İ£eÉ•X­V‚%JÆ‰áÇæ%ÃtÚ6[ŠaÅ…úP3×Šæãç6t¬Íî$Ê^ŞÍ>kÖE<ÒØi6¢7ãl—O9GGëí ^å¿¦~ô<ÊX «èj¼ç•ªäl?…öQ³ş\?BõßI8vJ–gTMmSZz@ôc™Ÿ}r„r´)6‚¾œI#_UÜI«Œ*˜6ë¯ú›
'Ÿ½Ÿ\ÍQ‹Õ+<í•øh=y$ëÌ?~âG}af¾ìm 2ƒU[tU1W#cƒf7*Îª‡³¹†ã¿ë¾äélA³\6?x%mòB¾U\Š‡1ÿé'—¢ğ«JºQÔzûG%yŒ÷¥çİ|‰7ø¸É9£¯â Ô=çDÕ¦±†òƒ¤˜½oœá2¿°(ñ\ÙÒ·g. µj›Ò;Öğ´_~¶7<O†f]ÿÑ°l—jG×ıpYìV~BdÒ*›×<ªÄã$–ÕÀw`âğ}ÍwË–öÂiV½£IÇYâò±~Fm‘$8š­!ï‹°»K°„VGù2İN^¥=Š	çÄ9Õë4Î7»§K°Æ™ÜY`ÙŠŞ ¦<ªDÇâĞ2KèI‹Ï
z³#ûöoÛ‹†ÄÃ´{fè¹y½öR¶İÿF¬æ°qRóI§i'³ÁîdÁ*1èh·}ÚCõ`zÚ(Ta²wP+@<ìÁ>ĞKá0Ä“M‹±r1£cü;l’©Èß,óUYèD<ğ@RÕWtıBhsDjË92Ê~”•!sÅÿèÏ&tùä œ˜.¸0çvkVŒibG têüá´oİ‡+³Z/ Â,«Pó]Ä ¬åÕr­[{dY?Â)*ö©œç¬3Sˆánš°ø&yb(Ôò§ám¨Åe¯áû?³i_ü˜Û$õë´2€®;è»©÷d–@äÏŸ©äi3·b[1T?ØÂèTÈ„ìõgP´_$FG…pİºaß`"¤õè•¦îÂQÀ!‚ùÌ€vÎ(i1Ÿ
;¹‡‡0ÊÅk½´P*ñ½§Ny‰È4‘D€.Û½
SıxÔ®ß[&„¤sHİM"ÆhÊÌm Ş°f×%sYÙN¨Îõ6ÿ9Šıı¨èhnA¥¿q&.¦ÿ_|)şy<b+ªT4Ä&n€–è?n ±R¤ß@‚U¸A3ì­B%´xk/7€'/í» H¥& ÂJ?aãXÑËL»G¥ÿ‚ïÏO‰Á´T´=7µ‹ñËo»Êß«„N¶ØƒsÕrÏ)?ô³^82º«d6vt@Ş•¶D!’zyn¶Æ-GÏGŠÅ&å"U	¾,at™0¾â¡“wƒ§Œ)ñaßFµYŞŞËXuÈÿÂfqZviı±IrÖæ³ÓÜ¤Ö"å*½ƒ›çÔô
úš£lŠ}ê¶ñ.šÇnÒÔ£A…·%®ä@±ä½°¤^ñ0ğš¿!ÅÉf™şñòÛLw‡ÔsÇ;Tşñ>\ëDpV’+İ¬Oï}vìj´Ú´ñ¡›Àr"x²Ûc\í^–ÆÒ|Õş‹Ú)XÍ1íïã»áî¼I…æVŸ½F“Şİ„!í|ºÈœ¸¿u¶°IÇIæ,õV‰ošFÛ²ói~ÿXº{×P€¶/J×ò,–:ğ0Ìs@ykk²²K|‡°÷C¢éÛßDvô´U¨ —„TRşw)
’ ¶kŞ/HÈ9²!Å1¼._KN¬“‹b2À`Ø_)8`Ó)YIêvh8ãÂ$ÿ4ßæA,–Q‰«ô [ ;×ƒ	Nyá¤Ñ“e³|0XyÙnÂª )œ úqy¿X™gªqÀÆÀòvwËØä´N¬h1ÿ„øø€»~jhø¥gßâ¥³nüØ²Û0Çlé%‹¦•²óß(ïuàÎ6^jç¯Šºt2´“C™£ÙšJm.®5ì®Ì.ëH÷Ø±F­ôkİ~´1`ŒXNúmÈt35z^ù™SM$…D~|ş‹?ÂäGq¨bi{1B<½ãlo&Ü§#˜­øó@c8`_eõß’vocØÌË|‚ K{Ã/ˆ3:Ôÿ1'U&é<È-‹te„ôß]9Šå$+Ãfó›}±ˆüÏg\WZx¿¼Zªï¡-rr«Ø­bÃÀ®½ceeˆÂ.PuÙa‚Ì,§6l«’®JûV?<ÆMJô><"8#¸_l¿O«¡Aá™ÀzÅÏw(ĞZ«¨İç!"KAi/àu{i"$Íeê2¿07«õCÎ‡|t~ÏÆTLoU–ÔÉTÛ”İÅJÔ¾ºŞ‚ ¾ÿ”Y-ƒë^]f›U/İ5x(V‹Ì'¼”Ì÷Œë<KØKc/2ùqWÄËÉ‰^ÎÛ˜:='Ò(De[xòˆBÙ?ı{Ì2áŸœ¨ÃZ·í*<¸ PªîVö˜…5ë5lÚ(gøãU|«¹Ï™Bök…aäe€»V+}•O1.ÄœƒŞİÍ%±ÉP„¯a_„WŒÑmßÊW*gœÌşf»&7íÎy9‹qİ×Ğ—Éû¼¯¦¼İK»d?Êèë´¬ÒœÏÅ9¯Ú)»g<ÚƒÊ¼}~óï´Ç›»7Q9=‡ŒSÕ{ñYº«:7á¬#Z‘˜fj*VáµÂ¶ck+§_G–#&²ój„ılÆY$Ş|~úÊXï§uì:±»Ú€ñµOÃKislA ™gÍ­½(	˜t×ÂAçWç‹ù³ÇXÇ¡WkÃÇ&&æwÕôˆÕ¿{+”êÊÄ½IÌCÆ»'˜ñCÍï›BN!-41¤_ULÄö&7t/¶ù½1oH—aËÿ©Yùhb*CZê|ÕüèËZïï¦
xÍ×/v‡ÌEé½r*gbn”Äˆc)»iiñ)5z o32jßúÚ·€Nb‹ùmŞs)K>1M	÷üLÊäw¤“íÇÆGÜ‘º9Ùú}7üUOj]‹ ¯‰FpHşw[:£o„ğÖ‚+¢ã‘j6\°?èj^ÚqİY¹ª³eôüuyÄm.0ŸÛ´i«kl0ÍÆ±ä{“%–Î4Œß_füOÔ|ËÅSı-J 5á•lÍ‡èª–»SâÔñR½LYİÿÜşº[[Ï«-(Ï¸ËrDßn"GSÃ¾Sv–è?c[½^Ğu_L/~‰¯+Ú …“tÔB“4†1K"‰Ğp”¿õ0û'õÖö|¡{Ê—Üî5s—ª,¡YÂN0Šòıx}¨tlèJñê<åÄ\W›™y¶
5¡ı`eÏ ™8Ù®s(§µæ|ËÆŒí&Ál»ÌøWÆ®T—·u˜>¯º bƒÎÙœÎ-èT§ß§ë†Û…e9»
çG§£ï>SwSj¤îÚ/nÊ•İ@íıph‡M¿Ú†v ÔÊdşÏû ŸB‹ß5»^¯F#¾ÒÜi,='…dÆ¯yAå;ŞÕEº×pÂ1ªÎh	u<‘-à« 3_wxĞL.•¨Æ’×VË+}ÑÍ­7ÏaÕ`İ,«=pH@}urU;Ú¡«JÕÇQh}Š„½İ@¡/è3>ªvÎ`ßsÛ‘©˜Ù#Âõ¬å{~ñe~ĞOÃ(¦Îî…Rã´Èòj’WNRq‡ÙÚÖ¿,¨(Âüêòîpˆ.Q´àiì†RÊ¶6Ç%ƒ¬RtÑÏí=Z,¿a}i? 3xÏØRvR-Qe¢õeL‰ÊlfÂŠ—ù3Š]3
„Œ£ ~÷ßáz_Îk~“J±ùg0‘-=‚$·«b‡Vzõ¾`˜ÍApRz—¢|&fOC+Šv§ªĞ‚”âp­¨‘L$u_Û ÂÊúoZ¿N¬uN—)ŞuÄ7±~Dy_«Åq
0’šåUÙú.¿™“R}2[ÎImú<‹óŠPvæÈŸÏj^<PÙê~°†Tåi»®‘d†h ÀŒ¤«moZUw7ƒÓ“×6SÂÅ¬û4EWe‚¹.|q¸à‘ ^ÑºMù,šP³ÜSå¾$>*ScZ%é:¢ÎQçşƒ˜^d§×*{ •çãüŞf&@¥rˆ:¶ê 4L
¦N¢d#éÖG=AÑ ³Vœ(Ëm
Ï’s¯ãƒ¶ÒëK$í¾DÇ€Ñ­mc$PÆÅNì±ì	}ù}2Ï$54€H¹›6İQ:@z—ÊDš·#zf:a²Õ)¢S…S\ µPèÄ¥ˆ“ÎW)HOÚLS»îæ¤.C%É[®ÆºtO\9Såaqå]ı¤©fº‡<¨‚JïÁ,–¤”¬ÌèÒm=«1Jí¦E/*”^e<zeZ‹¬%CÊ7ydÎó†¥9Ó0N\¢ÜŒ~ÅFÈºy¹?³t:PêcÑÿ ôÏ^ƒ%ù7$ƒoySVT™úq8-v0õÚêÃ!<½÷ëÓÖì²¹u_#şŒy­“'è¶ ÷,TƒSˆµY–j¬’÷“YiÓ¸¯GJ,	J,F[g+ùˆMVÀ@õÜ °ıGyÚo\®ÄĞéb¾hàyMÀ²P„œH¹¦ıá§˜ïòÇ©Îß¤k¹²É¬U è¼»Rà‘ÌäwZÅkräQ‡áw~€4äDôh”	,Ó&œ›õ’ÖÙAMG!½$`!0Ş½£İBÍ«ˆ<‰¹êD¢¥9º/€Î]3í‚ÔçŠ £µÒÒ=uärIx#‰?©ĞJí¸0¿a3–…ºúİxü—´è	îï–:b¥&"æ`Tù<†úÅÆiê&ÁSùéŠ:•Ç3ö=½Ö´óZÖS¾Ú‘”qwvF?¨–Ê*‘&İ¥ïÌ£Âš¼çIùÌ¾Äj©mO–*ç:¸¾.ZDÛÁ‹²ç¶ÆÍ½fŸ
ŠÜ-“Ñ‰ÙB0XFÖp–e%sR:O¸6Ê×SVËĞwĞ¾'±ÀÅj°¶öàÂ/# t»HºÜRc}éHş#X’\dC„ìÔmÔ¶¦®{¤Œ œ¼Š†ıvw+×]À¢%ïÉ‘–´öÖFğè4Få—»š©ÑÜ§ÿ¬^8BÇigˆ]p L÷ÑÒ°HdóÍ¬CiÉló!¦SÙ®-
¬åb¨ÎPo¥ZªŒÅÓ]ªPİ
'¯Ptø
¦µ±6)ş-\(f°ıF:ŒaÎ(™FÏb¡øº[Ó ­vşıY¦“"À§æš7ZH©Öjªv2Ó®–íaNTOÎÀ<ÿYa r –A°¹ë‡w¬A0\ê1ÉÛ4Ÿ×z &PCMsOvULŞŞyÕ_#ËŞ½[ÃJ*$Û´úŒb…”nÃÔA=ñé¿~Ø &+uÏš5J
OqŸQoòí`õ=Ó‰y¸J/ÒXh®šÈˆ°ËİŠW/şßº‘_bôØÕUH¹ƒß ‡»Wïÿ HÚ‘"ŸqÃAk2M0ÅZ"ß¡ĞP³9İä‹:p:Î[H'
T— X$W¶LÊÉásñ¦!.ÿó‹ÁËÑç[Ş­v:›ÀÀ¿ûµ˜O,²F‚b@Ò”#"]»Ü87q
I“£@I1Ìé¸¿6Çp×u°‘
sşR®Š
[u³9ÛB'‡K`¨è¨!ïÂ±‘R×e±£Jî™êlw½¬ƒıR°ƒt/{ş‹«6ËR,púÀé‰¯å®¾1:´6F”ñ%C|‚æçĞ[L_¡–|à#´IOy\H¦JÒ^dTù Ç|_±Ze/_¯Wü Ñkù—”f)3!Ïë+7T¨ÂÁ–o»Cı}mÈR|H¶9ŸƒP´ˆWM}To^cÉz\Ú$àUù2ME!˜*tk~…:	«â,ï	ÈW‰M:tÀSë@¥1şğĞ
¹éûg`ŸM¼¸ù_lã@‡(SA	/m#eá4şë–hkQs&ôİtÈ? Ú—)5Ş 	–s¡¯7¶ëÌ9È²!ËPySû_¢Š-•1“%ÉjŞ×uÈ·Ğßºl­z3ğ¦È2ûDÿgë/dk.Î»Ô¸A
(ß¦€K¼ñkıOõTó©ô5¿>Ogü¶8ş­§Cûj7¸¶İ[;9µÌ3 .Í#c¥cş,…Û'/Şó7I+Œæå;Pb4Â×÷¢†¸_pCü–Ôˆ¹ñ»!øS½Ëfâ
>°¥À„RÁC5É*İÅ‘“ğb¸G€ócQ†ã[¶i.ÒK%à°\–¼M sœRı¿µZ7V~à‘¦§ÿ¼¨Ù©1£ïZ{¬!ú+á  Íj$İu;âÂd,Ìâc^)liiN#f
à GààÄ:K×µs¦˜Œõ‘„&|8¿h&æ¶‹Óó®¯»Ñxdwé¦Lß¬ßªmô+_œrŒâ±îaKafĞ%7@ÚÉŠ4Tñ§Q…·Ğ­ğ|î'ˆ“ô‹‚}º	pšHã¾Èçã!ès’«I$\È¦+®Ÿ PLú"àºmwÄõké5ÉsÏç´ÁÙ2û!ŞÎ§ÇYÊ$—ºùC{™=¶ÁÉ!˜ˆş$ı¢§'¢›Hƒª8µocÑ±±½¾ŒÌ¨5ÒS,Sä=„˜ÌM #¢ ıÇì„fáu*JM§„à
û¼>¥®#Å¾º9¸JuŞ=,ø@„ŸnÌ¥ü¹¡â‹«<ÎİÙ*=òl%×#-¿¼w}¿ hûyxß½\i™J3páuCçŞ®î­ä~¥¦”×e/eø-ÓÜ9G_‡æQ¢NŞ'nyrŠ†«Uxr”ox@™BÒ¢‰'rEÌÍZS S“ûí|—îjhú„mĞ[+ğÖxók·ÓıPºZJ[J6lP£]®ÍÎÖ*ß&Ûá¤P’Ğ¥?]DÓÔÂœºÕÅ´|ÖâqŒ÷3‡`Ë—–´¥Aùç]gÆRu?¾4\6Ò€ıtŒşİªœ#¨¹%·h½õ­š¸Hö<ºáßQ£¾âI4ùÄÜÖu!¶BÒi'E4ñ*´¸„ı¨
"HFÔ{
±h¤ ü+mRGNíëæ9ÂT×§g@i` cZ‚>$’İ[®ÇÇ)Ù·"|8`[lé|@µµê¢då]·¢nµâFsN¿şè˜6ğÃ¯¶‡
•XZº2³„¿VCSh(Âôf…”)´!âµçûÃ¡±ÖJAA™RIËÉf½qh4Æ·H·ªşmB¬e”Î#ÏC=òx—KGÈ¦iTgÂ.VJ$7®ñb¸70÷fp:²ˆÒ‡¸ú ÉÃgè PlÆĞ[R(-m7îñl†Â}&WÔ*¿„ş» w0lİ7íY½ñÏ”wî‘tºT›ê“?V¬™¶ï¡EÀÃ†É–Ñy“© Õ…Ãr÷÷Ö‘¿Y‚±¥hûwnQ¬Ädê{­ò²¹ŞÉ”C2+—†ı-r7~Ü½Å_\G£Ñ:m ŞPı¡‡õ ÑÆÒ§óöÈ¸3†j)DÂŠm%â*é·@U[ÓM¡ş€©“,µÒ´%¦¼[ù¨ÿn?T§ÑL™_¡ü?k«¹¦å®3?ŸBoÌ<ÂßÕã‡‡XıäÍÛm›òab®Srbkh?ı×H*X?iQL’Ô»—{‘n?òåôOê ùKsô5\Â$sÖ¿šH£ÔQò$UÅsÈ1Èq¾ù¾Ã~ûYAAuÌ…‘¸%ÃnqKÏd˜ìŸ“ícnÅ±]=‚-©áñtpò˜•f·ÅFOe®†Ÿ"Khœ(~â^3åd>kÛpØJ©í©–€fJ  êù{… ©:Í>÷¿Í *%SOA2&‡AÊ±îøŸõtç9È‘$3¢”ØTÌƒµÇóa¾ÆA—ş“t‚QØ6^•xòÚØËÑŠş-a°¨t¦]Ë‡›¯öJ¯k5†­Ê´(M˜…7<d(şÙ~s_§jbvÙN\Ó¢—‹ €DÉlz¸r½"i«/}¬w³Q(Æn…p€_AÅ_°‹‹ŒÃR­k«[ÌU)Q†Ú‚êº_ë:\§¨DD#ôkSÄt¥ ~§Q%/fdR?n˜s>hTx†4I–8|p›	ƒ³‘(áº9qÈvl¸¶²Û[üòpQNñ,×ÀU¢˜ä‡•CJ1g=şÈ@T œ KõÙAÔòÂRxÍƒŠ¤o!¢Ø¿XC‰Æ¦ñeijÔˆDDGÁ'ı3[çAô_;8Øë§~Ò˜1b¤æÓø¬BMLê’¶=àá ÎÍ#ğ«ÎõP¥ QBÉŸÜ|Ğ˜0Bz!&è†K€m’tÈ«÷ÉË#vIÁI0ÙÌ‚5»!:^gôÌ u‰J'GsO‰zöËÃ0ËKOg¦‘°íPm•cˆ7¤ÅŒœÂ9-'×Lf™“¥;
¨ Š·`xÛ7è4Óöo›¥6¥ÃFÂIqVÍqª[	ëhitÙ»?æn/B»•€gfüû¥ı]±AÇšQ¦e^1Ë‰a‹ËCõÎÁb©M[7ô°5ïÍ-öWP…,‹¸Ø6n¶"™¡©lsXN nÒÁ|d>Êµ2ƒ\Â‚%í³#j§:›ÕrImDŠM-[~ZHƒO´OfŸ@å 'ÊÍ>ÜñOJ}++Ì(ÒU9¥}¸‚.škfÑx­
ß½üQFóU°*«Í?qòP|›f?SQÿ@1ÁGC|hç'n.æHş?ÂDy	J,ŠÄ_Åy)hÌ÷yÖÆó‡•?|b^¿;Ó±î‰õW-Úâ31¿Jc¤=£'úòÈaYˆ€Ò‰Kõ!ó~<DïŠq.ßª—0–"i"î™hH}±@×>X+ù‡¶3)
m±$´"N;zÈÌêƒ¥¯»ò#’Z±Ô.Æ;öyô Ş¯Œ3î¸*
›a¡ç?Éºs'Øz-jÁß­Ü&‡­;U¬Üœ¦!QÄıç¨~‰F?5Å9WõÛ.AçE†SGÕ–¶óãµ¢ùp™{¬İ.€„ÑîyİÇš'	¢×¤4Šã…ó‘Åt÷ØCĞ®y†f Ã½mMØèÉ^	×-°ßæšÂŸ"*¨Me<ª+ó–jÖÍİã-/œ|’Ùœa/Ùü(€ĞÍ4Yõ cãûm
¢Vãô©+P#+Mi†f™z*nĞÌQêIöDıåËKx´º3¾9&€l¦$Çô‹4µôšh¡øç 7æß”A¯rÿÒƒ½NÛÀô„.Fµ=9m”ETÀrÌqØ
²ô]&°ön	hÚÑSÒÌ9ÏhVMådyºPôÁ„m¢üë÷ÁA/ÈÿÙb³NÙBH}øã‘º˜~¾.`¹·–º‹ØÓDÅ€—/-®Úş"Š4ãÿQ#w[s$¬{­ğ X.ÜvƒÑŒ%™~Ü ÀÙ€ke?7ŸÛ®
Iß*Éázr›HĞkiÂÜ	6zjA22á]‹5ç€´Dåºb–~¬åÎN’9î¤Î¶ Elp›Õ´¶ƒğ=™Xï•ºD·¬õ³„ÉY·^^w>=áèxÇPŸ,rIK¶”ğæÀ¾4¼­G!ƒ~¸
\d¶À7/QY'ÙÊ«nø-Û/)\{*³&‰™œ#gˆn4Ö;¬ò±À:¼¬âz§õßåéŸºAàbu’¯´NÎó#cğ…‚ÈÉdkã›ï¶ÍREøiÏió6V1Pió~r¾blÕIµ9æøØX:‚Îœ/¦*ˆñ±(İùU×òäµ¼-†%ùÇyU~Lt¼¦M+Œ…OAÉú3™qzÑ#™êòÂÑ¡jÁğqâÍA;Wê>üAÇ¬ò„äÛŸLÕ{²ËşƒFbÀG="·È¶G‹µKWÈ»qÒ÷Ÿb<-â
•]ãdp«‹qBT´êªNú…/ıÓy¬ëñİ¤l›²­ó5g"“£…ß´Ø˜·'EÍûÍÿ«@E)3—Jf2W‰ ĞÂàdzdÅ{ÚŠû–,ùé*ôA.™ ²ó¶&‹ª=Ş(C+Å\íÖÎŞKÁl|ı®•šç¶–IÛ´PW…-.g.©ô	”f”J5ZéÓßò˜–Ô‡‘àãıÄZ°6^H¹ØšYÊ×Dä´Ø/tòVVª»š¢<€C¨ó#?2$b»bìé'–P ·Õ}N-‚ĞMË@øxıe[4ETJ5–ñ]\5MûÆ0ş Ù^®,@î<¸/­VŸğD‚w4¸l£|õn¶©”¨Qy‘§ÆëÌ$Ñb¸ú¢\î(_ñp¨YÔá;s~¾­%¤mÁs|]Ë†GÏ®RHFÂüHd0Z†s ÷F!¿­Qh“ã	½æB8û§s½<ãÿÜ¨„#ÿó•Âg+6Ç^õ°;ÂÕI€Àñ JÒÎ¿qîÅ2Æ¨õH$2 ²`VÌî5R£Ó*PmÓ“«İ^§w¢ú`€1 İQÍ>õ.1†Œrş›
Â|"4Ø´&”Ñ>òu‡Böa¯MÑ>A'mö	mÑéU¹lYô¿´šS¢N¤ƒ¦·ína‚íÜÀÊoËO­É.ûæŸh­›:¢Ã<°ùmBĞu`´¶+ÏÈ~|–tU¼ãGƒPL¾ç†vó|µöÏ6º5®-°¨ƒá!á0 $ŒIgj[jƒ>k­ªø1J³&'`\ínjáî¨7ƒy«•ïÔ[Ö¤Éÿ}±iÇ¬Os3—nOŠÔğÜÔãë!‘pÅŸ¨Õk¬t\m2”ÜîÉC@,¤œúLV–m|ÍmôÃW½À<™!+åÌï?Æú·j‡ÕnõI¾/öÒHØàâšo‹ÛÃÁÃÇ§—×ñ<‹âjF‘¯÷ÎögO-2‚‰ôu©
b®ÔïõL
&sb?èNÒ¨ı¤Æ²üØu®‘¹è~iòõWZœ&"K:ş2ÅFĞº0¼Ñ{£ÅÄsÛ0fıĞ¢g9\G³@äÃá“°Ş×é`³Å±Êbè\™ì%ÒMí¾Ç¤0İÏGÈÇ%ô`=\gYşê™ô°u!¥Æ¹–<?“Ù¦¹ì"¨ydw"£çåo¸>²è‰¡Ÿ(³Š*“§ë9(<T©jiˆnmÀ‹Ş«qùgu¬µB»÷Ñ¹[O2ª™Css\°ùÙSˆû”–ÀaµŸİ1‹P!·5i¥°Õ?aƒn•Pùğ&6Û¦âì¡–d1ø#6ó• =³	êÛØÊş†÷69¿Ø˜qî Áh…YğÒ?³÷hzÕ‡Z‘•ƒ@pÔÓÇ ¾FRĞqõğÚÙjwn%äC¾©Äg³UŞ5&'…'›ø&Épx”mÈ *.aN¹D—Pæ4íÿPÊ@0š"¨ñÄ×Ò°é¹œ¤«
Ì	¨r†¬:/Ó“?w¦›¥şá`pNzıœ~Õ.À\O¿ÌGªqn$Adµ:mÛãB½Jtß<•2#2Æ¨êsŞ¤âòÊû	ü·?×Ğë=Ï\{ş ŠAlØğ¾ùÎhÉ³’úD<¿ù{úï9å°hi¡ID)ÜÏb£FÃ5ş:ÍN8>z¬A;hà’ä„ñš2¬Í})WA ÎQÕ‘›ã|İíˆÕaËèL„lœX¨S¸àØ;hˆåÒ~ßŸT!àÒrw$™íçÃ§î¥lİ”Kì%ÄÑ¿)ÑçêšDo¶ÿZu[±´şğf>(›6	Ü‰“{¿šXÅ:ó„"öŞOPÃî\¯õ ·‚‹xpÔkò{ù”4~iå—VñYÏsõŒ*ÅE®.‡¦Äœr³.™'ùŒÕB"¿ •*µ!çjå	ÂC8-9vvpLªÈ%ã­.ãUDÍA$Û»6{¨;ë˜ã%ı„£,Ô^Mõ×,N·ºh 0—üa*IT‰	L†çLt'?}RıuÆ~³Äd_óô€]NTváEL¡VªwèØÒüeuúMYO5¢¦ùCïW§¹_¿£o^èµ$ÎIÕnË°L•çugæTë;p,µçl®,³ñdE´ıIu¶ºí{¸{gî‘Íg°i3Aîn%’°—Ôúƒ
pfVÊ?t’@(%à;Ã¶Æ{ë8úéí”RŒ&©êáÛw|–q¿uMk\]
Ä^¸§ıë%³o¬åŸ&÷«Ş+JÕ‹	GÒ”Æ_¼®şˆ  I´ïp5©‘Ş‘Äµ»: €ß	®¹íX´†2ú‡ö”)+P?VœW-v/I7-k?&ç@€"qIg¯œå$àä r ¥¯õC‘[nü6ÑFLjáú•ÙÑ<qx†?	ú ŠbQqİt1+ò ®T®0¤\Rs…ÇZZ¿ÙCXõÇD(0è¥÷íÇ+ÑS[°Àò†É†¶Jâ’Ÿb´S£=›¨Ëİúm‚tÇê7 Â Èû`Q»^y	ÁÌKƒ!gVlÏüúÈı»¼»2² 7`?µ'vÙ–çå/Öén’›¬Zéİ&è­ëq}şºŒÍõİ¼Ékş¢ßÚŠ<ØgÔRe÷éywî$q÷Ğò ÿ}4r´'Ïïvş"¢^´f·Øë°æóQÔS“jrF”Ö€ğ±æ1=t²ÆŞ'êâ.éÕÙ DÅE_ßÃ¯Öó¢wæ²&±@¹Á|ª©^ƒÚI°+×`ƒI¥4Bï"Â—Kùêù‚¸‘Y+—ïJó*t·æ€ç·\rChJò$E:ÜsªüE¬ƒw”vŠáôÈwÒîÖ?/«qeFx ©Ú¸WË0šXçıAàWé|ãûìÏkHŸ'/¿ñT ˆw<Î˜ÂÚ-³ó¨Ï"JÔ–` v·®£Ç­'Y’Ç‡Uxöq¶[¬ÊXMlÃ{aË@\ŠM1ô£È"Tn²J—oÔÑ‰±Z9z´ì"—îş9]d¬lØ+>ÓÕ4ĞF¸Â>-»ßãe¢ë!¹„A­·Ó›à‘¼¯×vd/eJ:¶Ñ¬Ê¯¬¤BÈ’ËëÕÙO¾¼öõÏ{ŞG{eTÄãÿÕ•ˆHÚ…v"^ÙA¸Ì%(exó u%#_ÌÜùl ôéò8AœÙÔl–¨3½ID—ş„<¾	•”ÖgÜœ	ÃÑ1\p›,kÏÀ?7_3Æ)½Z2`ÑÒ‚Ó¬·LÀŞøìœã ¥í¢f7Óï‰É<ÖoôØğ¾V!•ï…÷ÒqŒ0Ù‚‘C¼ºy?m²qİÖ{&oĞş®$–Qª5ã7›ÊÈ‰Ä%rµ,dšaX®Üg^ 0gñŠ„ĞTo}ÑÈÁŒı„lßšµP[u ÁıİåÎÖ0Á›Š }Ô¾»Õ¾DZ«VŞÜàUpôÛ_D_T”âĞK¬M‘Ôßšp*Ì@r­¾u‡"!V$ÅFÑpB»€¨~öµc{½”ÁT\8Le>³;‰ˆí°	ƒ­øb(ôÚå´VtQ-Å+›È@>©.²ÛÔ4‘.¿	÷ì¦©"Ò
ú@|×ŒŸŞ3Hñ¾Fæa@†|hÇ‘‰£ñGe7!MGĞĞïŠÂ÷ÓtëÖ÷°(şÃ~ıYf)ôØn¯¸Ú
< Ë°x„Q$a•
ç¹|™«¦É'>£Ÿ?½
3;#¦í÷Œ_Aß7‚†“âQ2ùï«Âî <Å¸µPÒ´O>¼æ÷[%®ÒÇoÔ†òíûé0Œdae_*
Gà„ lºÿu$¬†èòü½'Ü-p¬{‘]Æ‰c8YÏŒ|Ê.æ†f/tœbB"ãJ†ô£ß’êçïó=%¾x¬«ÛUGbMÀÄı×Ì~ô©—Şf¶Àdp0¬#ïYÑP†‡ÇcáÎ1îj1
KùŠEéÀ©ÑĞ«lĞ*4øRğ¶®ù
Èê¤¿¸4j8¥R¾Â„&R±É?¾!i÷L[yÊ3D×~Í~d8EÀªo0°&~J#w	,bWì0wÁ4KëP£h‚æ§¢Pq|û#/[ı³"öÍZx[ÀT8êw#‚Ş|G	óá5—Ø-Ì%ıã¤,­q1ä5Ô6Í¥C‡Åö¹MyåBã¢Ã}Úm|BS¶ÄVÜí¸@T¸rŸšXCQğåıbç`ÔBiU—ı%äb]Ï6[cæ!?¨ƒ¢WË¼³Ææ§ÎÌğ;òâKN”€™€iæãc]¥ûXN±}*BeXÕ?£²Æ NGwÌ_Š0Ûpñ§1”0ÍŸnŠ 	6•{ë`dó;.ÒÑ’VMÁTÒÃĞÄÁ»<õÌÄ9ú+V.,jQ.ÍÌ€ àş&S2¤=]úŞéÇ40pú¥çcUæ€ß!ÎRr½‹urÎƒ êjÔ§êïñùª”»®ŸğçŸ;È(ùšM½ìù)’®øŞß\¼¨úéÚ¡Û§é™¿leIĞĞúY‚„ŞÍ‘Şó·G7Ú™nó»²0D/ßÍJßâeîÚ{>zR-—–¡>Vö¹Gƒ/äï¸°
EDÃÿ–İP-,\ßrï^·üQ?ÿŒ”<t46 ŠT©rÑÙ±Sı‹Í²ÆCU9í1%/Zbâ–³ÑØr¶Ê†ç+j@¤øªá¾­0%Z&N‡ttEÏ‚ºøü”k¥¦S2±,¿Ÿœâ·‘şXùõK¦@˜üwşº¢ct7;„=ic¾rˆõ`ÖéH<Ñ
noÛÏ.V±ö&İÚ‡úPÙg›”AŠmAöŒFsàU¡zÕª’$ZçÖ<eşœ)8 ûDÌØ¢€ô~¥ì˜y“’ì¶Ja7)€¶=—l½9Ö(~í7ğa«ıÄŠ1Á cÏ‘: ùŸ÷$ê©œ€şQGífĞt*÷ƒ¹6¤°É¯éRìjH•`DWd–ac¼¿µ×QÄ†ƒúÀ˜E|û«¨¤7Pk«,?ÁVˆã’1f;È÷“NÅ±c…½ïõº)gj}Wã£‘j“Ë8'Š	H?ãUÙÖ¿`\zïeaæn&E~ -	‚ÚÃÉÖ2¾.Ï¯,eHôÑ†µ
³6,sVX§Æ©2@ài/ƒÿ;cë±r’*‘K­õl™‡—˜¨AXïŞ8ê=¡7r¢V×ğÙeO£ĞYÀ)ê}³2AÚ«/6
{nåüŠôB [ymèÜigi75)
Ùø
, ³(üÔÃ`½]Ûğ­ÿœ æ-ŸZéŒ/^9È.ß®€¸®Ú€ÏY=Óé~[6´puº±gó!‘ÜNê&pVboNât&ë›ÜÀë¥ÎTÄKÜ¬9z§–ªš…ï=Õ.Ùı»ğ«€bßNA¹aâ ²›j¾áÎùÿ¨M¢@H#ÙU‰c3† !;¹jÃU©D²¨(ÜÂˆUã1üLÓ*T`£8ßyë´T‹…ÌÇ=¦$Ğ¤ƒãY7OmØ¡’W”Óÿ_emÁB‚|Gå¤âk­´ÿ›zn§cÕ">á³î¥â«-ó$õ£Udv€nÉ¯ù3ò½ê8*=µ¨ú`;ôFĞ/¦]‹
Ï­¬æ%Òä•†në§Ë±œíD‹aã¯\`#IvİeŠ¬EÊ´ã*Zæ,.¢»rûOX¬:f×?UQ õQ¹ió)³QóÄè_›Ÿ–¤8¥~Mo…ÉÓ¤š‚EJöÚ¸—Öì¶¦EÂNÆà#§—Ì°ò¤|´ËDäÃ|¯E´ÀÎÙ_ı	‡˜ÏšœĞSDm„¤©¯OìTis¬txè#™æ<Âv>hHéšO´Œpï¼ÃÃN;Àîgl––öín.L†ÏR`¾İ„:;ÓZw¯ê·Qê³ª2°ı¨Aæh¥Œ6M!ZÙAœq•­İhÊ¥c²EdyÁÇ™O>¸‰VyÇ)v×X˜Ï½<Än`z÷6 ÏÈlíô‘|~Cï7>åãWûåĞÏeú2‚§Rd‚wHó*™XUàâBŒäJw†¥ùQlz l£Éëç´$Ç¥½0Éº…•õ+!Å¾yGˆLÕEşå`k°”ÿ#();
}

/*
 * Print out an error if an invalid bit is set:
 */
static inline int
valid_state(struct task_struct *curr, struct held_lock *this,
	    enum lock_usage_bit new_bit, enum lock_usage_bit bad_bit)
{
	if (unlikely(hlock_class(this)->usage_mask & (1 << bad_bit))) {
		graph_unlock();
		print_usage_bug(curr, this, bad_bit, new_bit);
		return 0;
	}
	return 1;
}


/*
 * print irq inversion bug:
 */
static void
print_irq_inversion_bug(struct task_struct *curr,
			struct lock_list *root, struct lock_list *other,
			struct held_lock *this, int forwards,
			const char *irqclass)
{
	struct lock_list *entry = other;
	struct lock_list *middle = NULL;
	int depth;

	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("========================================================\n");
	pr_warn("WARNING: possible irq lock inversion dependency detected\n");
	print_kernel_ident();
	pr_warn("--------------------------------------------------------\n");
	pr_warn("%s/%d just changed the state of lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(this);
	if (forwards)
		pr_warn("but this lock took another, %s-unsafe lock in the past:\n", irqclass);
	else
		pr_warn("but this lock was taken by another, %s-safe lock in the past:\n", irqclass);
	print_lock_name(other->class);
	pr_warn("\n\nand interrupts could create inverse lock ordering between them.\n\n");

	pr_warn("\nother info that might help us debug this:\n");

	/* Find a middle lock (if one exists) */
	depth = get_lock_depth(other);
	do {
		if (depth == 0 && (entry != root)) {
			pr_warn("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}
		middle = entry;
		entry = get_lock_parent(entry);
		depth--;
	} while (entry && entry != root && (depth >= 0));
	if (forwards)
		print_irq_lock_scenario(root, other,
			middle ? middle->class : root->class, other->class);
	else
		print_irq_lock_scenario(other, root,
			middle ? middle->class : other->class, root->class);

	lockdep_print_held_locks(curr);

	pr_warn("\nthe shortest dependencies between 2nd lock and 1st lock:\n");
	root->trace = save_trace();
	if (!root->trace)
		return;
	print_shortest_lock_dependencies(other, root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Prove that in the forwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_forwards(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_root(&root, this);
	ret = find_usage_forwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(read_bit));
	}

	return 0;
}

/*
 * Prove that in the backwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_backwards(struct task_struct *curr, struct held_lock *this,
		      enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_rootb(&root, this);
	ret = find_usage_backwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(read_bit));
	}

	return 0;
}

void print_irqtrace_events(struct task_struct *curr)
{
	const struct irqtrace_events *trace = &curr->irqtrace;

	printk("irq event stamp: %u\n", trace->irq_events);
	printk("hardirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->hardirq_enable_event, (void *)trace->hardirq_enable_ip,
		(void *)trace->hardirq_enable_ip);
	printk("hardirqs last disabled at (%u): [<%px>] %pS\n",
		trace->hardirq_disable_event, (void *)trace->hardirq_disable_ip,
		(void *)trace->hardirq_disable_ip);
	printk("softirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->softirq_enable_event, (void *)trace->softirq_enable_ip,
		(void *)trace->softirq_enable_ip);
	printk("softirqs last disabled at (%u): [<%px>] %pS\n",
		trace->softirq_disable_event, (void *)trace->softirq_disable_ip,
		(void *)trace->softirq_disable_ip);
}

static int HARDIRQ_verbose(struct lock_class *class)
{
#if HARDIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int SOFTIRQ_verbose(struct lock_class *class)
{
#if SOFTIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int (*state_verbose_f[])(struct lock_class *class) = {
#define LOCKDEP_STATE(__STATE) \
	__STATE##_verbose,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline int state_verbose(enum lock_usage_bit bit,
				struct lock_class *class)
{
	return state_verbose_f[bit >> LOCK_USAGE_DIR_MASK](class);
}

typedef int (*check_usage_f)(struct task_struct *, struct held_lock *,
			     enum lock_usage_bit bit, const char *name);

static int
mark_lock_irq(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit new_bit)
{
	int excl_bit = exclusive_bit(new_bit);
	int read = new_bit & LOCK_USAGE_READ_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * Validate that this particular lock does not have conflicting
	 * usage states.
	 */
	if (!valid_state(curr, this, new_bit, excl_bit))
		return 0;

	/*
	 * Check for read in write conflicts
	 */
	if (!read && !valid_state(curr, this, new_bit,
				  excl_bit + LOCK_USAGE_READ_MASK))
		return 0;


	/*
	 * Validate that the lock dependencies don't have conflicting usage
	 * states.
	 */
	if (dir) {
		/*
		 * mark ENABLED has to look backwards -- to ensure no dependee
		 * has USED_IN state, which, again, would allow  recursion deadlocks.
		 */
		if (!check_usage_backwards(curr, this, excl_bit))
			return 0;
	} else {
		/*
		 * mark USED_IN has to look forwards -- to ensure no dependency
		 * has ENABLED state, which would allow recursion deadlocks.
		 */
		if (!check_usage_forwards(curr, this, excl_bit))
			return 0;
	}

	if (state_verbose(new_bit, hlock_class(this)))
		return 2;

	return 1;
}

/*
 * Mark all held locks with a usage bit:
 */
static int
mark_held_locks(struct task_struct *curr, enum lock_usage_bit base_bit)
{
	struct held_lock *hlock;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		enum lock_usage_bit hlock_bit = base_bit;
		hlock = curr->held_locks + i;

		if (hlock->read)
			hlock_bit += LOCK_USAGE_READ_MASK;

		BUG_ON(hlock_bit >= LOCK_USAGE_STATES);

		if (!hlock->check)
			continue;

		if (!mark_lock(curr, hlock, hlock_bit))
			return 0;
	}

	return 1;
}

/*
 * Hardirqs will be enabled:
 */
static void __trace_hardirqs_on_caller(void)
{
	struct task_struct *curr = current;

	/*
	 * We are going to turn hardirqs on, so set the
	 * usage bit for all held locks:
	 */
	if (!mark_held_locks(curr, LOCK_ENABLED_HARDIRQ))
		return;
	/*
	 * If we have softirqs enabled, then set the usage
	 * bit for all held locks. (disabled hardirqs prevented
	 * this bit from being set before)
	 */
	if (curr->softirqs_enabled)
		mark_held_locks(curr, LOCK_ENABLED_SOFTIRQ);
}

/**
 * lockdep_hardirqs_on_prepare - Prepare for enabling interrupts
 * @ip:		Caller address
 *
 * Invoked before a possible transition to RCU idle from exit to user or
 * guest mode. This ensures that all RCU operations are done before RCU
 * stops watching. After the RCU transition lockdep_hardirqs_on() has to be
 * invoked to set the final state.
 */
void lockdep_hardirqs_on_prepare(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs do not (and cannot) track lock dependencies, nothing to do.
	 */
	if (unlikely(in_nmi()))
		return;

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (unlikely(lockdep_hardirqs_enabled())) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * See the fine text that goes along with this variable definition.
	 */
	if (DEBUG_LOCKS_WARN_ON(early_boot_irqs_disabled))
		return;

	/*
	 * Can't allow enabling interrupts while in an interrupt handler,
	 * that's general bad form and such. Recursion, limited stack etc..
	 */
	if (DEBUG_LOCKS_WARN_ON(lockdep_hardirq_context()))
		return;

	current->hardirq_chain_key = current->curr_chain_key;

	lockdep_recursion_inc();
	__trace_hardirqs_on_caller();
	lockdep_recursion_finish();
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on_prepare);

void noinstr lockdep_hardirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs can happen in the middle of local_irq_{en,dis}able() where the
	 * tracking state and hardware state are out of sync.
	 *
	 * NMIs must save lockdep_hardirqs_enabled() to restore IRQ state from,
	 * and not rely on hardware state like normal interrupts.
	 */
	if (unlikely(in_nmi())) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;

		/*
		 * Skip:
		 *  - recursion check, because NMI can hit lockdep;
		 *  - hardware state check, because above;
		 *  - chain_key check, see lockdep_hardirqs_on_prepare().
		 */
		goto skip_checks;
	}

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (lockdep_hardirqs_enabled()) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * Ensure the lock stack remained unchanged between
	 * lockdep_hardirqs_on_prepare() and lockdep_hardirqs_on().
	 */
	DEBUG_LOCKS_WARN_ON(current->hardirq_chain_key !=
			    current->curr_chain_key);

skip_checks:
	/* we'll do an OFF -> ON transition: */
	__this_cpu_write(hardirqs_enabled, 1);
	trace->hardirq_enable_ip = ip;
	trace->hardirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(hardirqs_on_events);
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on);

/*
 * Hardirqs were disabled:
 */
void noinstr lockdep_hardirqs_off(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * Matching lockdep_hardirqs_on(), allow NMIs in the middle of lockdep;
	 * they will restore the software state. This ensures the software
	 * state is consistent inside NMIs as well.
	 */
	if (in_nmi()) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;
	} else if (__this_cpu_read(lockdep_recursion))
		return;

	/*
	 * So we're supposed to get called after you mask local IRQs, but for
	 * some reason the hardware doesn't quite think you did a proper job.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (lockdep_hardirqs_enabled()) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		__this_cpu_write(hardirqs_enabled, 0);
		trace->hardirq_disable_ip = ip;
		trace->hardirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(hardirqs_off_events);
	} else {
		debug_atomic_inc(redundant_hardirqs_off);
	}
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_off);

/*
 * Softirqs will be enabled:
 */
void lockdep_softirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c, avoids
	 * funny state and nesting things.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		debug_atomic_inc(redundant_softirqs_on);
		return;
	}

	lockdep_recursion_inc();
	/*
	 * We'll do an OFF -> ON transition:
	 */
	current->softirqs_enabled = 1;
	trace->softirq_enable_ip = ip;
	trace->softirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(softirqs_on_events);
	/*
	 * We are going to turn softirqs on, so set the
	 * usage bit for all held locks, if hardirqs are
	 * enabled too:
	 */
	if (lockdep_hardirqs_enabled())
		mark_held_locks(current, LOCK_ENABLED_SOFTIRQ);
	lockdep_recursion_finish();
}

/*
 * Softirqs were disabled:
 */
void lockdep_softirqs_off(unsigned 