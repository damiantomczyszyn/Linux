INJECTION) \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     free cgroup_mutex mutex_lock mutex_unlock system_wq queue_work_on seq_printf __stack_chk_guard bpf_stats_enabled_key __x86_indirect_thunk_ecx sched_clock __x86_indirect_thunk_esi debug_smp_processor_id __per_cpu_offset __stack_chk_fail bpf_sysctl_set_new_value memcpy memset bpf_sysctl_get_current_value bpf_sysctl_get_new_value migrate_disable __rcu_read_lock __rcu_read_unlock migrate_enable strscpy strnlen fortify_panic bpf_sysctl_get_name bpf_ktime_get_coarse_ns_proto bpf_strtol_proto bpf_get_current_cgroup_id_proto bpf_get_current_uid_gid_proto bpf_base_func_proto bpf_strtoul_proto bpf_event_output_data_proto bpf_get_local_storage_proto __kmalloc css_next_descendant_pre percpu_ref_is_zero bpf_prog_put __x86_indirect_thunk_ebx bpf_prog_array_alloc bpf_prog_array_free static_key_slow_dec __x86_indirect_thunk_edx bpf_tcp_sock_proto bpf_sk_setsockopt_proto bpf_sk_storage_delete_proto bpf_sk_getsockopt_proto bpf_sk_storage_get_proto bpf_cgroup_storage_unlink bpf_cgroup_storage_free percpu_ref_exit cgroup_storage_lookup bpf_cgroup_storage_link kmalloc_caches kmem_cache_alloc_trace static_key_slow_inc bpf_cgroup_storage_alloc cgroup_bpf_offline percpu_ref_kill_and_confirm cgroup_bpf_inherit percpu_ref_init cgroup_bpf_prog_attach cgroup_get_from_fd bpf_prog_get_type_dev cgroup_bpf_prog_detach cgroup_bpf_link_attach bpf_link_init bpf_link_prime bpf_link_settle bpf_link_cleanup cgroup_bpf_prog_query bpf_prog_array_length bpf_prog_array_copy_to_user __cgroup_bpf_check_dev_permission __cgroup_bpf_run_filter_sysctl __kmalloc_track_caller __x86_indirect_thunk_edi __cgroup_bpf_run_filter_setsockopt __check_object_size _copy_from_user lock_sock_nested release_sock __cgroup_bpf_run_filter_getsockopt __get_user_4 __put_user_4 __cgroup_bpf_run_filter_getsockopt_kern cg_sockopt_prog_ops cg_sockopt_verifier_ops cg_sysctl_prog_ops cg_sysctl_verifier_ops cg_dev_verifier_ops cg_dev_prog_ops      F     G  !   F  *   G  A   F  �  F    J  1  F  Q  F  a  F  q  F  {  L  �  M  �  L  �  N  �  F  �  O  �      F    L    M  +  L  0  N  :    @  Q  m  R  �  T  �  R  �  U  �  V  �  W  �  X  �  U  �  Y  �  F  	  [  A  F  j  [  �  \  �  \  �  F  �  F  	  \  a	  F  x	  R  
  _  
  `  %
  G  m
  G  x
  a  }
  b  �
  R  �
  _    `    G  g  G  r  a  w  b  �  Y  �  F  �  c  	      d  7    <  c  �  F  �  R  �  c  �  R    Y  !  F  �  F  !  F  !  F  :  :  V  g  b  h  z  :  �  :  �  i  �  j  �  l  �  :  �  :  �  :    m    n  !  F  b  o  �  F  �  L  �  M  �  :  N  p  v  p  �  q  �  L  �  N  1  p  F  r  q  F  �  R  �  _  �  `  �  G  �  T    G    a    b  -  R  A  U  R  V  \  W  c  X  x  U  �  Y  �  F  �  R  �  _  �  `  �  G  "  T  N  G  Y  a  ^  b  m  R  �  U  �  V  �  W  �  X  �  U  �  Y  �  F  
  R  p  _  u  `  ~  G  �  T  �  G  �  a  �  b    R  !  U  4  s  >  W  E  X  Z  U  �  Y  �  F  �  i  �  :  �  j    :    m  !  n  1  F  �  t  q  F  �  p  �  q  �  p  �  p  �  p     q    u  *  p  E  p  W  u  j  p  �  F  �  :  �  K  �  r  �  @  �  v    F    L    M  F  L  K  N  a  `  �  L  �  w  �  F  �  F    F    x  3  :  J  y  a  z  u  {  �  :  �  i  �  j  �  |  �  :  �  m    n    F    @  /  L  4  M  l  K  s  v  �  r  �  `  �  a  �  u  �  @  #  }  ,  ~  >  L  C  N  V  `  h  a  {    �  T  �  `  �  w  !  w  1  F  M  R  �  :  �!  �  �!  R  �"  r  �"  �  �"  �  �"  �  �"  �  *#  @  /#  �  E#  �  �#  ~  �#  ~  �#  ~  �#  ~  $  K   $  ~  )$  ~  =$  Y  Q$  F  q$  `  �$  a  �$  F  �$    �$  R  �$  �  %  `  %  a  v%  u  �%  `  �%  a  �%    �%  R  �%  u  /&  w  9&  Y  A&  F  U&  �  t&  L  &  M  �&  L  �&  N  �&  �  �&  L  �&  M  �&  L  �&  N  '  r  '  `  "'  a  Q'  `  `'  a  7���g0id��?��vJ���E(Y�.��A��q��d_(��D4:FݤD�Y��z����3�� �S����Y*t�o�2�)l��.�Ja6�Qx� !����)R.qCN��]�t��c8-K�&�o�s��-a��}6H/��x�9�,�������ɭk�����_ox�,w PU:��$u��D����q5-��4�L�ާ��(����jj�#�[�����H�]��~8ܧE��+Q�+�"J�Y��p2r�S�R��Z�[���^�r��/���t��+]\!����V��jF��W~\�f�D��|~`8��覓� ��c}���h��F�� �P2P���C����⫭��6uC����gw�2����%��6T\{�,��wf�cw�]̚qȻ��e��Rm�s�毄x��̻������Q˒U��ZRR R�Ӹ���fEuz�����*��ŏ��2�qf]m9e-(��k���!h1��֛9~U{�)0W����/�=5ҟ1�m��1��Y�uL�v���e�z׆���x�$��vߋ(�No��	��xm�C�3G��97lKq����HM��9g�k!��[%C��g�ԖXdCY#I��1Y��5OeDT�aK��޵�Tp�K߮��O[��]�'�g9D��@�@W"cd����N�R7�x��X�)H����'���ip/
͕;�tO)�v0%��?�9* ��l��I�XJ&���aЩK���QZ@���ܛh<�_\X����XH���Ĩ�ɶ��U��؇w�I��9O? ��#��ؾ��&_3��C���(	�ޯyɧ� MN#)P(�O�ꍪ���N��fq�����i/]&�]�a��g�W��itI��=�L���1��sWh
�%�{�L��R�ٞ-OC�
����Q<vI�ۺ/A�֜p�l﵌��/)se�?P�P��pw2�g�-��&���W�8fO��:�Gc��Ɍ՚�ciL�±h��S�����3�G7^{�Px��&����v�i�RHG����F�==4i+E{���r�S:~]�N7o����ьs��t�j��q۔�].=a�<�G�Z���<yO2Q����i�D{V���Vdp�bO��7�Ĳ~��{��y��h���MP�G����r�]>���5o�h�Uč�h����wwxd-�#X�x��X�֤�_�m�b'�'�,T��X���12O�[�kQ�뒁EL� )�cR�����
�0\C�Ȕ�SMw��LU��mD����Y9L��ju����5I���I�%U�$v�©"�,�,GǙ���~ _�y��as.��������8e���Op�A��ݣ@\�qj��+e�\˫hm��)ܲ����=�d��?]5�x���3��G��H���DM{��::	=7�l��u��u���t0���a�{��� ��D�����ןȜ��0g6������(���ol
�8�d� ���WC������0���ͭ`V��r�y�7����}�Ы@2�ة�3�?�Z�3���	���D(0w����?���uA�w��n,����)�Z+k���z�����.���܊No2�7:��:<���������N���&X�<�X�c(^F{��}��F-
7�kN���~7=(eywß����)���1�����K���C0�����F����f`�{�4�ر�[D6��T[�u}/��g�'Җ8̡Ѭ�(�n��T[��'��~H&)zc0w]���mo� �~p)s.#��'-���G�ܙeU�~����K�PYC;k{?`�7��p���|�R��J�6B�z[��0t�
`���ZO\/��4��h��e���i��=&�ޭ���`��_�ԈeB?���d�{}]�m��y��P��KcG�J>��M�	a�,���1��C9������G�R��i?ê_Q�RLA������{�N_u2��Ĳ��2}1���c:F�
[��.x��˴S߮%'O��P���@q������M��ז��0F\��_�D�� �J}��5iv���=��>��D�2�y�|�ĮB�����)7K��S�RO�L���5nD���ci�� ������[Һ�\-=F�d���`پ���Y���l鄼�ʮI2��䲯եgm� ��"4W�_CYZd��O�ڦ���5�Ī����wH��S���7Fq����>���6 �7)!{�F}(�.
.c+�Y�ȷ=|��hn��w���z���nY6�d�����[-� �&�pk_�\���EC���/U�O��"����h�0�#���S�>T���6_z�*#��N�*���Ѕ�b)J��TcqV��b*�Û��j��Մ�1tB�rӟ	xUl#W����n�>��K�G�,Bd�^�$����?�A�ƨ�6�Q��<fl[_ȼ�pk�^83N�q�k��vA����(�*�s����5.���/Ng4�Ȗ&�����n�]�bpB����G�y�_��dk8���_d���_U�YN�(i��&��3�ªrw�<�"m$���#��<�l�忌�F�"�a�6鈻A֘미\0��@��קǅtML\�!שy�'\�
�ev�%�9}��8x�����B�o����匽]�!��J��
en�2+|'"��D(l�_�\r�e3�eX}.��,f�#��7�6��IfY��L��=g����R�Q�2��B��%�7�l{���5C�V:��((R��3Bev��v%,Fa��m�P�B0g��W^����G�OD4��L`?�������>'늛u���oVI|�&$A�q=��Hz��~�UE� �E����D���8�vF��d��+9�>(OB"�YdյDfy�Q���e"N3�5�`YЗ�2z�"r��w�P����1��t�)�v��{K3��٫
B��&P4z��jP�p���s�lw��s�O�˨HU�o)�>&�hm�����f��r"�t,-%$p��|S���
#�_�LY���U.��[�������v�����"�xb|���@�����+��3Y&K^]?�5�8�]��8^(K0�p`tʎ��<�j8-imȲ�M�̣��X����y�1�{)kI�ZYxFF�>)� e�=���z�}A9,_T~B���5I�N�K��`��;%˝�՟�5���'1P+R�qaq�A9EW
G6�����~	�C�T��=K��e�~ߑu]��?�$��i�����5�4�䏋����*����ph̯�9����A~M���ޔ�F�'�5?�jSuFV�b�(EX%�%��J����͵���Q��I��W����m �g�}e�R!;��rg��Q�I�ȶ�(s���_�*HdNj�5��,X�ŴSb�桺|tF҇L_�;/f7�/ٯE��`�!Q諉���䮮xg]~� �)�~=��[�ɧ�!,�Gj����dc���(�Ϛ����:|V<��w��P�[�ߜ�*���1hm7%���5�VBnO��p&�#� $T`���W��ߌg�/
-��h>��m������)�E�U�#��Am��ޕ6����#Y%�
|#��C������6�}-Ǵ�N�<-o�C�qr&��P��VV�qGV8�)a��,���[,z�b�Z��oOG�(���ia}�g]\�wu����
�`���N���9�@Yl���?�wE��\��oi2}���ran-�&��J �Q]�	޾#��2�x@~z~ǋ,�砿��y�����,���T0���N��R�P~��P�E��
�惘�nF��Dl�!{�9�"���/i��hI�k~J�e���d�Ԇg!H��Ц�R���Y0��#����-���
Y�(s=��Z��T�}�Cs�f�����D�TM�� �fC�)M<�7��� ^9����oU�� pl�޿�{������^��%�>:*�$�V�=��U�/uV[-��Ό�=�X��D�5Z0۸T�ʘz��	cyN,I"c��k�	�;�����e�M�(@�D�؉ߛݕ/�ʯ�+���$��E����*.]��R��2R��Q�A�	7����`�EC*j�B!"5�7h� ��f��ru���okx������CQ���r��k}�4bR���ȩ�O*v���t�R�\���7	5�_e9�ί@�vׂ$m����7�Ο��o�t�]�	Rum�!D��F9 q>�ة�B��W�ZiO �DU{� �%�7��T]�-`��d0�u��.A�c�� B��H��`��S	Ђ3Yi��o��ԺmY�??�ҽ�O�*0�����#�_�������tB��b w������ �����d��BU!�`Fi�h����.ԑ�Lc��q����)���n��.���[�T?Zm��P�1�]��:I1�bA���ADP�z����\��~�z��гz?�V�k��>���~;m3n����.c!!�q�o9����LQ����u�V�+Z�cY\���(���qČ��s�X�I]do���Y	r*ua#��\�m��ĐK�M���$k0/���;�I��Gd%�����X��!�P�z���Ը�B�d��B���d�)g�	{����	+��2#����,�����k�QB@:�_N�	[{��NN'+���~&D=�Q5O��T��	|�(���$Mmm��:vB~y�����$��aDa�P�t{V��Q0��a�W�j�E�3U'k����}��x qbۘ)s!:g�ܑe ?Ǖ��8��)��e��E���|��]�z�C�7�{�=ޙ�ez�k]��<W��ݧ�c��dxV�wȂX�n_��W�A�ɴ�+K�ľ�C��⽾�A�O�3Gٵ�B!WA6��Y���� E��N5�T}���^��f!�z���;n�����ĤS���Ȉ�ǖ%��"�S�+	�|�LT�Qi_V��M�9��{Y�o�Q(�8w�L���V*H�D$-#�%U�8HlB0&�k#g;}FpCǓN}�E�f�����ˈ�����k�*y�@$�����Mct�N��5����mi�����Z'	ϵb���Dk7UA3"*�S�����?m8d����K�/���� <->�����yR��X��1G�㩧e|��s$BO��7
ʦ���F�L��ɣ��C]�q����[��)��L�ʹ�G��*c�+[D����ް���+0?1SZ�gHX������h8[B#B����U!i�i�!�� x��,�Z��=X�Mm0�H��k�P�V�����=)��2��s���{��y���uN���}@�f�g "ݦ��M���8�Q0{t��=�Y�U8�=sn^�l�O��2�*��� ��x<Buu;hQm���Y�!P�xB������4�%r7]��^W!�<IA�Y�u�IA���r���
�o#��aMi��20v���ٖC�ˍ�[���t������G8�u�^
���ܖ�Y;����̲�e'�r�$����c?g�AP����C�~�#�ñ������2f(��b��Y��t7B�L�]09��l�����,r+��׍�=�=���j�j}���h�v�Ew�^Hj��Ζ��Pv���w$���b����l�+��ia~jވY.��ȩ��PΧ��豁/xW�*��3B�Sgd�l:�TB���!F�V��_豑�:��W�Wx�,��r������}�9��f���5��2��Z��e�b�]�q#�-�hߍ�]iԏ����Z���Hta�kd�*��k�K��V�C�l��l�%�9� �m%�Q�oz�3a]�~+/+(\t��&��`#���̐K-��e���K����ї�칶!�l�9G�Ր�­&�r���������+/�Nݮr���27B���}1�˩5ۘ�M�r'���C�o_EV���$�>?g�3�D`�;�|�_��$�M�eeXySm�b�����hx��gP�؎�������dmYqHh����u�6{��)/������A����Y%}�&��<?���P`�W��/�I�a���!=�4)rb��e�thzC�Q�9��H47��w/I1�싅w�y����#~�`��J\^B�@Q%JI6�jx�D�4gb�E���ͧY2I��J���z��s�+�l�0�9��
���z˕e��������Ij���@Ki-9��PYN��Y�0N�߃��A�a>���.i�B�CM��5l����"
�yÓ8ܦՓ��j�S
�f�;���o�R��O�^�n?�[,)��{�����[3S��<?������� (a���Y7�>�
�j<� 1���q罉o\.P��<�W2z�\�	ge�h�l���������#�&�/�#�x\���s��F�On�h"��帬%��El���?�9k�yi;5����>s3� �z�ݥ�Z�J�jq���=n�C����Qz�O���+���2s�t�T��H���^m-���6Ԓµ�=�� ��<Z$�q�{7Ҽ��Z���E>�T1H��{y�{��(�E�!#�
DK���G�D�6]�N���N[��P��Ӽ4#c�x~8����]\F�{ϋ�=4�k	�S��J�zEI��X}�pf��?����^�O �;	
Jg$��ՠ�x� ��޴�P�'�{�.�>�}�9����Z�5�1ON>�&��"�.�'6y;d�w[��0��rj����.-�v�)��Q_�������au�� @���z'�� �b6U��>$3�4��>�,��,.���;0�O/Nx�J�z�XIi�����;)��|˩Q3��ܔf��g��lF�/�2X�t�;{(|�Mܶ�=�E.C5�Q���^��_IeP��ig��`�8Rܵe���e�md��ЕR"��l,^�'���}[��Y�p��Ka��F���
�4׵d$U/�W���w���b�4��h彃�LW��$�qt�q��W`��N 2����0Y3��q������fU�#I�ՇzYvZ��R��u9M������#������6�(䥺G��#I9p6f_<U�z���1���GH���z�O��2V@j����N�
.�5\���f�w�8C��s��G\������1�*?.��C2�<F�=��[,T�k\j����c��p ~�/�K����M[�4�멭���	�̋�pBšk�\���L���!v�� ��J(0�9��'J�mn�k-��Dk'��V�+����P�qT//�Y���LZCv��1�q��.�(0��d�p�J��9����8��no�[��i�a�!�%��N��DHyJ"B����xp�<�g�V�=�p�&`q�p/� �׮�J�g]A=
� ���h=���Yژ�S�sB��[���1u�s ׃�q�G����y2�930�|-㹇���{z;KNAq$�=`*��n��~f�8�5�b'N�穯���]*�^[c/5Ǘ���*���_��{�+�����,���ґ��6�F��<�-��Z�����Ҁ�֫���\���C��w��5�t�<).���!ކ�ge���.УWVP���m�[���`z��$ŗ�NH����#�\����;Ҋ݌�ғ��K��ϜG�S[�w9��`[�$A��X�S$���5Ԫ�� )%��K)�p�HK�U��(�I�i�>mN0��}���L͔i��w��Q�~������v�7�ӏ�.!=!y��@0���W��# �)l��N��j3j�������Ʋ�r7	�8��R���ڢ�����G���M$� �]�An���=�����l�T�WyG�<��H�~�B�:�J"8ϛ�O�W��K�)����H[�0���,�	��y%墷Fĸ*��VY;}��]P�a�{)(P��pL:Ji'l�@[;-�w� ju�.��d�0_�@9=���H�܏lj�T�e�ފ�@GI���cR���f���k)������*��G1r�q�h`��)2��L߂*��3q��v�T2�ZM6B�pR����O����7��>;���m=݈"!��O��ʳ˒�p��Ӊ#�i̸�G�@nL��:"�ca��.�O�8��]!�oR9C=3� ��h�T��U���Ǟ��61�J��+�Ϟ�j*|��
;!�A��F|(gz=0�p����2�����絶�b�����@n�utr;v�� ��P�o�����y��
8m�_�%��s/�:��R h {���� :7����B}m��٦�<�$f�m�F��u`cI5W���cLŖ{�Sb3�c�78��^�\�NĘ\�'Hb���؊/>���@�f���<����%���8�.MCk`eO/�n$5���vܣ%�FO�r\�X�v�.�GU%�y��x� ��0���J��D��q�q�g��t�V����j��`o�\۬ۯ��J��U� "��0^��Y�##�]eE����2<�i?͵PqtR%��T��сb������^��]���:�~�J݋��
}D��5����4
�qS*��zY���1�����?�+;$)������TTg7c.*��"U҄$��Y%��Jե�L,[Θ�� �ba��iM]�����u;�b,j�O�Vc~�Kbí��3�v��ryYw)�k-ʸ��^�3�fGQ1��歐=`�!�v�1�UV�E��:�l9G)��/ǡ?�=<��׾V�>�	k;W��*��ĺ[LVN�VA�s�y���D�tؽ�S��+#�@�Oc����)�>���$&܁�c�-:�M��`��;?`�rJ!Jt���8�t��Q��J���EU�Zݝ<��T����a�b%Ҹ�u��V)�ޘt���-Û'�m�L@��JP@��L���\R0!�"1#h�E�~YY2d��i�7�2��\�u��v�#�I�D��,3ݏ3��MN�s�2�^ᐱf&i�R��Ĭ~]Q�C�G=#�
%Q�t����Z� ��f{ ]1,�7* J8���œj�ș�ф�jG<�:�u�Em8X|X7�|~�����>�Y������SBr�R�r�'֔KL�I�j䏾�����Wq�E�Õ�0�$�.]D|��k�ql������v"����5���X<�l��2@dt��@m��R����_�q��9��`#α�5�tߓ�(c~D7�N?_?�]�=�4��/�������I�4^+6F��·��٠���<D)�܀�n�z	��3�=��J��l|��R&�t��-D��m�pV�;��;u��*[Gg�&��i�5�o Yk�r(�����o��HM�qPO���I䤼�����2�� �a�:�o�s�H�oޜ-�qK���ћP6 �&F��ez����ܹ������
R{���:��d��Z�8��nV0)�-��8�3&�ҷ��(�	�������E+���fZ�ɾLE�Sf��`�(�l��S��X�����5/��,f�6�A�+���޽��v��i�k�6J��G�� �#6�(�9��g'8����G>w��������;1�ɛ.]�Q}�l�,N�1���#(�a���61��"TH����R�ϙpa̾-�Sܟ@8'�D^�N�a*��)�Eb+q*��Xb��6���\4߹�Ù�\����������!�W�.��iv{E��K�w���ȁۅ۟v1H�ĬW��5�k+��h7��K�r;�å�~��Z�DǠHHvb$��a����N�[@���{��O��n
��rQ�p��1y��L�d z��h5�ú���bJ]��r9g���1H��������φ���8C�Ou<��	���;�QRĔ@��ws���b ��]�������Ըg` �nC_�����)ā�/��G��!���ux�{,���K�F&SaFl�֞\�C*0'N�,��G���V��e/��2�d��]�Bz'���5p�n9t��?ۂ�'f�1ڹ|ѵ�`��6���~��ҰO8a0p�֚���Sk���T�hNڄ�0n�ɨ�(P��ݪ�<"R�T2x{%Tw�~�4V�8�W��fp�$�C�/,��xA�R� 3�ҟ@�-�q�z��C��ta���,I�F����\�K'@�3+����)_���^��=6	o(����8j"�Ih�V,n�u��<��b�#h>�?���;�������{��V����V�X�����¨�<��$||Uj]�(�U5�X-M���v�M�����ƙc�c��a�4��iy���������2ݩT6!����IE;+N�o��$j�kϕ6]B�qY-D�������*W����b�r_i.�h�/E�б0���^��ZUsp����t"p}G[�>m_�o���a�
���c��H���l4���i�8L��N&��~0���O�+㨨s�і�ܾٛ��TTq�p���l��ZJ��B'������$��t�.Qj:��P�3G4�(��u:]�t��q�v�PaJӠ�S:J�����̧iK���� f�պBZ�Np�6����Ym����!�?�B痢��N�d!;�;S��
�[�8(|Hc�=ǳ��e�������.�c�>}�^Fk�ɳi+��a#��و>e�y�oKh�?�G�s����S�I3_�8C�����G�R��S�Z�&ʛ�S\1�s��&������&YW+����
��>X��:a�A�|5xӲ%টX�Jn��'���N�qs���h&I/�tTH2���&&G�V�Noc�2��iҪ1�y�y�_/�)�2���z1�`�E�$L�S'ӯ�7b:���ʥ�4���(ʧ��ߙmBd����8X`�|���-�Bs����e�HE��@�;���%�NO+ɨj�u�ZiT��|���k@f��k�6?����b��)�м�,P��n�[�z-D�5��W�#����L�o��Bq�I����1��ֽ�<\�+VR�yaGeg9�	�<����P���E�%ж�E��+a` @�}f�
��8}�^~�C��hf�)�)`�\<w�H��f�r�+�&q��:F �&gi��0�=UWئ�?i�H�R?�������*4�ոO!;���8.AICb<~j�����~3�$F>],2��9�]3AR�}7�{N����<�]��$?�%y�\p�'���j��ƪ��ȩOz����N�j������M��V��u*�B"a.��1��1W4�6Y����w�Xթ����M\C�u;<r���y��o>Dm�K鎸d�#���[��:�:]w�}�C3�+/�2�!�?iˍ �������2{m`�
�����k*�ңl��#;���/W)��9Z��Q�SL��IcQ@:���(�İe�,�H�zw��b3X��F�j�TT����l*yl@u�)f�m��５���UG̽�l�.dʔ,��tc/kyBn��HB��۹y�������v]4v����dq�~ר5��c��u�S���M����2�w˸��٦�x%&E�h�5��yAt�<��U�4m��z�0T`Zgy���4>{�"v 
�?�wKx%���,1E*X~����svx��^R�}�q��
J����X��n"\]��-�&��[�ߩ��Xis.�h@ND���1�;霭���~�M����+��_�/|����x\I	ڳ��^=�����K����3�R�]�lj:0:�l-����@kV��)_(m��Me6������>���\x��h���V�3N���1��L��"�_��}�����;�,I[$m �l���s��a��m'���[֬m�I��|(
�ՇA���:���l%I�	���6<���]cyi�D�<��!}Ďf��q0��Ȋ�<��}�ʀ\�����r��t��r��iK��\%{��]$q�S��s����dB�[���qs��d���;��B��ʹ96��Q�@v(�.������~y�z�S�"�I׷�f�I��7O{�W%Ilt�X�c����ƞ2��@��%KG>�o/d��,������f�DX/���eZh��O���zTJ��Z4?C@_*eOT��1�i��ED$��/���ha�C������q�qOjܛ�|�}`�aWd ž짔b�g�,���]���y>�yeȰj^e����W�~�0R�B�V1���u�%AC������η���\j8��S>Z�YHՒ�� /��ec��c��Ț�#\;��s�5�+��幛��E#7Vg��wdWݛg�Π�cq���i��+2���
U�gP&���Qg��z��
U��v���%�6 �+ǘ�)�q����9��-���\�����>�BG1M���l�	h��*����;aM�