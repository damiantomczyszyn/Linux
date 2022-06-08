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
  include/media/v4l2-subdev.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
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
  include/media/v4l2-device.h \
  include/media/media-device.h \
  include/media/media-devnode.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/tw2804.o: $(deps_drivers/media/i2c/tw2804.o)

$(deps_drivers/media/i2c/tw2804.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ude/asm-generic/ioctl.h \
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
  arch/x86/includeg&˃{��~��)Z�7֎��u�]��T����r�[Q9�A��n��3��v��Q��^1� ��Ι��<�?�����g2@������8(;OYH�I2C��@Hj��Y@�����q���t
Xp���?�fx�0���.��l�Ѡi.�gɔ� ��(5B0�2��u�����P���Y��66�E �|�+_�.9�c�>������zXu�Y2E�`�M&�R�J��������.��:,'�uX�xka���y��$䩁��$3�ٙ���S�N#��V�ȥ
���D��;dqh����3G-U�@	�T�N����<����YoU�%-��X�ӸX�����W�� B�Lv���q�U�D\��JCﺹ�o!�h��8i�Y�Om�p�$��@CqW�6�-C �B��o�;�k��M����լ��Ǩ(���Y_c���3�tX��JB޽���Gn}� /��4%48��rt���r�i��O��j����>���S�u]*f	��3[y2�X��5�tĐ9b�~�Ƣ`��N�|�ēT4i���s����Z�/�Vk��|P�Ӊ��B�����`Ц�!z!��?��;�WU�J�t��󠴆)�#~*�z�ÿ�>�k�SX��������2<����2="����${�n �i^;�'��!�t�i�*��5�ݷ�s���1��B�+�v�Xl6y�s; 7.�m���Y[�vl�<�3��Ė��3B���a"���6��#D�t��w1�٦�J�	^<{����c^�ZlEr^�ޯ}@��)�������&{�����7P��_泛��������H b�T�Io��yڿ"Ϩ��hy\N��U��� {���P�jMU
�frj��D?_z�}!i��%�)�e*j�Z���2�&Üߋ�6��>�h��X��3-ŏ:9yB?MQ7g��A���~�� ����C���f�x@X�tK��_��=|�N��۹_�)Ժ�6�f��yRlY���c(d1T��8��١p��� +�\T�:��<b��
%T_��-D��2�D��ino��jʰ�n�4敱�;�Hz8 #�2{-ʾ���D@�n����
����2ڏ���]V��P}�8M�s|_�c�8�-��d`T��9'<��&r��C�g���7�(~需CR�/&yv��">�ds�Q.���LD���2n��n��x��bk��"JO������&�螐j<ͳn���eZ�ٲ���RNB���P7�)��o��v�*'��e���j��e��Ʋ̑��UZ+���WO��c���Y��M��/�6��T��F<�E��_�6���<��Q/���g�R�K��J�hp����>�6RZ� ��P��߾|^���0�
\(L��2�$Q��?�3���lw�5��e:j�Q����Ue���$df�0�e�Ndr�ld���U�EI��C�9	��X�:���2@����Q	�M���`]����	��$��(�:����?H����)2�Dl�^��z
�p&�Y���]�KKZ�4�f���'{��?��6��X3Ұ�a��7e9&}�7�c��#bƑ_ݯ����x�A�T�`V"�sn�f��'`�r���C?ӑ�����;�"�L��:nm�����|�_)�#y���e�jMu4F�3���T ���m�#&c���qI
vV�s�d�vee����D�g�爌$�da �q��7�K����\[�[�MS���K�gecُPe�ńH�g�f;Al�H+|���ɟDb)�~wf_}�+� �&�L
����bS!�_��7� I��x�G����߇�' �����c�e�-Ø>{���}�:��]1�΢sd	p&z٠;�ܔ#��������)��S�EJ͸-ob7�Fޓ�c�4��b����ɷp���q�]>����ԟ�2�
�QD4��:�p�h0KV�,8�y�#&Oo(�IL��5������5@��g,�5:����dӑB)Nw�M�a���m���}�;v�֐����$�<~f݂c��~��B�B�2����Aڛ\����l�����m�:����qӢm�`�N
�>g���Y��M��+Q&},\9g�P,�L�������^i
\�_G�\��6r�4=�P�b��ʨ��[�C6E��3���2�f �+/!������qͻ���l������<�@뺆�i�b*���H�o�#��ӈ1ʧ����l-�
��i���~%���(H���A�v����^���"	�����/`r-8�����D�N�3P�Йf������6���m�0�J�7m�̤1���&T)���sF��hs� ��r*�����u�ec�w`���YB{,�oܶ{|l��7���n,�����m��aEr�p�}�sӣ��&���Ҫz�]ճ7g�N�`%�4��}zPy.I��'�$g3��AJ��~xQ�Az����k/�$ �������o#�츢1Ht-�b~\���c,tk	Qݛ�]�29�����'�D���M���a�x�;}��~vۼ|Fܷv����ɰy����?_�t��#	�����L�?v�XB(���7
���w���#�9,?�4 ��)��:�76�'���D�b�� +"����m���3eɀ���70��y�xyg�
���\�֞q*��8���hYe���"�r��mor�9���!ӫ�G�.�F�	�#��<�U��B�^5LVQ���<�xu��c���<�t%'�e�A���
@��l�a*�jEA�	A6IxyPB��?�M[��Y���5��%c.L ���֑
��z?�󝨕6�I�D��nXA�מ�hzԳ���!��r���4���d.CB�>u���Z��l���ڰ�j]8���+�:Ak��}mk���V(�|�?��{.��O5t&/�S4pU�n_J�kVш6�j�mDʨ�SK�L�.Pg� �;��NF>��|n7�z��e�+Q�1wB����ڑ��_�X���p�.�(�Bq񔩾�0������E!�n��!���#� �3$)��T���զp��M�M��X�Y���dWQi��V�r�x��ܫ_�z}bᗭ�D��0�p�a��� �5	�/�`��1	@�Z��S�]�pu�̊��Y���zM��QA���)��������Y���@�@���޽q u������_0�X8��4��y�*���S��1�6�L)0����1y�K�����1>��ĪZ>��qf����5'�Í���WK<�F���xک�$`~�ZC��թ�l*Z颶���C��;�oPѻ=��n0�����T�k~�ň����s���;gf�,�Ҫ��*�x-؍�������\��k�5˿��Zi�&Q�7��YfZs��S�o<��3�����w�� �ï��ϭ
+��Q5�x_�5��o���d\1�;-A�5��Vt�} ��0iA��p�������	��X��=)d������}2?*�di�J��b,���L[�O�},V��k�a���a�3Eh��G���M/�)�9�cE+H�m���t�5
,�oe�,����n۪I��u��~�'ځ4�>�>*�Z�����_Ad;�v��NBf�M�����~}�@�eWA�b[�?yA���ϕpG���6�����PԬ춘�e:�iJ�H�w='`�0������(���f�3�n���xj����+7����}�F��d�Q��4㦉�B��"Ob�I�T.��1�9������R���mə�F�nڵO�Q�rs�{np�b6���:P�I���`6K���E�`<�_��0��!��d�cq�`��VPK�}aT�/��~ѱoDJ1_6Ɉ���8�.��l�kf�ȁ���p���	G�s�{/���D�y�w��[��4Hv#,��R��ў\��P#`Ƈ]Y���N� ����8��ebƻ4�}��N�fkG?�$�q�wNi�P,m��샩�^�f���a*.xFf iXG�o;�zA��u��]�&��F ��Dr�4g,V
������Q<ӮL^}c�i�3�oJ���/o�U���}�a��hs,�UzF�؇oeݶ�t�^���O.7��o hz��J��Gu�U۰��|�Ee�|,�x�x�J�w�R�6�D��X�s=���&�q3 [G0�h@��ͳ��/㬰��?�m��,{:t���6f���y��8_k� �Ȭ�S]���ʍ�.��T�E
Һ�).:��&��� ^�����M�K��7]������8ж� �\�f�[�X�K���f=F3��U�ܵ���$cAq�Z��`;=^ѧ�z�-��М�n�CF2Y�%�S-�h&�R��F�3�iP��D�7-�3pl�7A{�Q**��&�����8y�b��W�Ի�-`�-�k:�?�|��w�$,B>uw��͵�LR�.]���t�b�A�zl��o7�l�0%�1�p��6Y.�����Զ�3$��$��0rz!���%�rP�.�:�wa^u��D`L���8l�;*����A��'`
\F����˸S0�`|A�虐�w��j���]���g��܍�pAj!вW��f���Y}}��`t���Q$L����#ɟ�zN��a=�
uq|��E�-S֮�]����u��$�a�ۀ[s@���U{خ��N,��6�;��{������P�Ǝ�|�q�,����ыOU2�ܿ���YA�{�r����{պ����0��
��T��@�ν,zg�&�,��@} 8'<m*[�̺�,�4p���|�2%Aw�l��ę�&�؇^	��v�,��"��Y��+˶�3�l���VWx����S���+�"n�a-d٫Go���4��S�C�*��{�m6��~���bq�j�剪>7	E�r���DΚ0��G3N�b���ZZ���H�����6%��]�Bm��S�Ƀ>w��L-�x5zf"L�E�
``�>^]kA9�n�PK�^����T��Ϳ�T�����R�E��"W�D�E8�.��"��o)_PG9��� =�Z��C]h�>Si9b��)BD�@�@�y�s�{dMOW�t�%��/���q,��]��#�Y�YdK���O���F�1���[IY�3�Ig_t5s�H�H�XJ�GQ�v��3��xV�D$��÷��K�^��#��*[E���M��?�d�چ��"~�����(�Z?>��P�޸I��$�
���� ߱2�T��S����R���kP��=6ۖu�T��7%W�jZ=�T�֖���cַ<��"@�n�y,�
��ۚ��:$f�ę3��2/�ι���)ks�}���7ƴ/��8���-�H>
U�p(�3Dd|���L��&[qO��Gߏ֪f�ˏD_�
����h��q)��w��l�n�*}}<j[)�������Х{��Zռ��ե���8���z@��A��MY�"I��xW�]��@�"0.�Ѿ���k1��vRa3�-N��}�u�R���Z��wΠ�oNi��K�Q�Nv��^
SPs��J�����M@��5�4��х d s5��;�"��N��7����!�=�,�h�����-�w'��
�-��ɝ_ ��}1E_.|4�{�����_-9ý�@�/b��H�q�u�JX�5�g�b��~�d9�C8��Ń�S��|��sWx��5uN6��j-�j-������Hn&�"��t��y�Fxk�+�����P����w�Y����pӋ��6�Y��0�c(H.!X�AU;w�����uxʫ#�k��Ԣ�.��y�;[%�L�ZYEM����Le�&������ cF�;c�Da�
�- ��,��UT4\|bz�^<,p��ԇ�:�%�+X�x�۱t5��Fܣ�W���@ܲUe;{�e����c_�C������3wK{9��.7Z�s�ʏ�#��3�߾bߤ��=;҉[�(U7�l��k��h��Z��3M�ОB^���T�ր�B�z�ܘj��=" h�<��A�e�����N���	�+����6�������[7+�趗�Xo� @����~e>E��µ\nF���<�*�'�k�0�U��>c)�QZG]K�HN'*z��I���:�H�^X�5���J� �ߣ�8-�)Z�E(A҆yF~�#���$`xY��(oIDO�E���,�	�0`b��:�Gu�<"ݰ���ME:�a�1����8����(��B[ �n>�)'�A}K��/9�\��l�	
�-� �5d v�ȝ�r�v�����_foj%�J�0�\�:���	bى��x��Y�}�1�ޟ���&�=�Y��c�]�c�Lt9�����:.����x�п���0p�B�A��9�sᾇ����j���W�v�SI�U���KG�n.�-aj����v�o�~j��M#"�k/x�3b�%��$ګ���L�>�� ��i��=8�$Կi�󍢝ӕY�vY
	�y��Ri��;#��2Gԣ�ɱ��ĹBˁ%>�[�w�8���v�b�&�Ȳ�m~��E�sl6�Q�#H|W����ه��"#�*Rg�� �`Ӂ�~��Gw$�笌�����th�o����ռ�-߭�*�.�r�ѩ��0o)�5�
�����ev�Nt�{e����Mhx
'ړ���l<<����,QMl|�'g�Ao�=*�-O�Y��-�eW &w)������t敟� C�����C�`P���p�?Y���$>�Ce��A��zu������.�k.��H����#U�X�㓙=�5c��P?�/���n7Φ�l�EY�e��ϧ��7��r>��$���-�p��0c4f 6�i�[�^X�ŉ�7 Z�O0�Mܕ玁��#Hԕ*��v�6�|Վb�s�������!y@G-Q���+�A��m�,m��?���5X>$�ٱԮ�2�}���ͦ"I�\Kn���5J0߁	����)�XN\��Wd�/�����/�$��LٶM�1	��웑^u���@P�C�Ɍ��I�*�pu%vLG[!���C-���E�;�+�{g)��d0��WKk�����I!<�*e�����e��_Y�m���@����::�qD����^}%�V������AH?�� ع��%�M�^kN_׊�<q�p�Hc���ʮ@~���� jȶ��6/��k
�����չ��~J�l1�|�Ɗ�)�Ƥ��B�.^�
�f4C���<���[$^C_3�+K�[�V�S�c'�����;%����@���F]��\M�K2�`�����`t��
��Gw�b���T�r��<�gl�Ҩw�]�iE�*��c���,���ޤ�c��"�$�@�j/?�
 ����`��p��E`뿚�A���3Xh-)e=Gc�%.a'�zns$U�|W{x�9b�E е�כ�K����v�񴅄��4e��;�?�f�:*��o??RJ��͑au�}%�����D�j�c�p��}��٧�%����M��vf�@;j�o��B�M}����7AuN㕷��3�Rp0�wˣ�2���	��������zO���Y��A�x�o��ky�j��Kz�}$�'�㠳j�2���m�3��Ey�]RG�RT?6U��AƤyHIP��:U�"�*r��=�q��t��EI�倦`#|������^�
�3�d����`粷k�g9=nn'
2�X��OA.KX��*��t���J�_^{���HV��Ե|�FE�ʊ������Bg������lGL�>:��½�Pٕ8��Xb�@�R��3��6}����Yϡ|h*��� ��ʯ�����a1~��}cV��p�lAa�Bڱ7�bM���Ϲ,�~�G�H4j5�KU���b�M�e�;����t �dP?	�T�%)�-{�Pr����,�j���$�s�5lug�䠖@s����T(ڼ��ٕm=V�ލH1�#���kR�K�}�G=͕gk��� ����"F�_Vb{�	u�+�L�Ͼ���0��z��J(ޑ���h���>�넙���ØJ{^���R����5�;3���J�j�:�b�?�s�BRw}��4/VEr]����]Q��&L�@z�w�ؿ��v�t�Q;O}S�ViRo2%$a�3ᠺ�P9 ��_쥞7�ז�dy������&�C�h�3�ز��Lh�d+�������wQ��2tUZ��˴8�y�ZU^����`��蕲���64>_�m�_�1�����5�k�`"�#Ա_��"7���S�2-T�m�˺��ȑ����/[2�uYd�R{$��b	 ��gh���W��B���˒��Mk'sU�>�#�5
�g�9��zBb(3�Q���q���F��/�BI���Z�MK�F<[�P�VWD>I�#�_�[����Ih[�^h�'K��ߟ�ߨ��4���#��}����g�o<�Ͼ��]0�[	�qhY��ҥ=@	)�\j.gM�n����܂��`���YuH�_&ڪE�ys\��'N��i|&��bu��ګZҫ��_G�f�9�Gs��_�~�W{l�6��d�gf3V����4d��$�	w��g?+��� �eI�����No�<�(~~b�	R|��;B�W�v-�C�O<���^>��0��٪�UCZ����U��F8��`�
�I�B6d����TE)��x�L�� ,p�*�p�ew�H�)l��������พ�طk����_��)���[!%qqpʋ�������d�-H�O����;��V�� >^����p�4�Y%+9Q��k�X���N����8bU�M���R1�5C����DG��43��C�ʿ��uFG����&a����'v�U���.}�Pn2=�?�EN-�(3�x�3>���Upgi7�f �蹽{���'���\�S���hnau�����{�f$(+S�"�>�s�&���:�����$p�M�+'9B�8��.��Sd��%"}����v��ڻ���m���'��d}�2�c ���ŜA���X��p�M4�����2&�Y�y�ԥמ]%K܈i�Ą�_�~�����C|B��9J\��L�d�+wr�����>O��	�6��0r	UV��A�C�h�V���G����5�G�GI69|\.����]�$�a[6Z(vp�]�v�g
{�,}��Pi|��Y�E07S��	�#ܛT.�z�<G�`��oĢ��L�i�X����V�n*Ԙ��d�2��%���҉K�9�$�)Oq,c��w�zW<U�GS��g� ���-Ʋ���V׮U�m�j��t:���O�T�:�i0���v=>�8�lRt��|�yao��]K��?�/kJp�-b�"�!�O�� I�'Ѱ�m�ۖ���-K�����'H�@|*���Y"u�.��z[R��������Wtq!6��Uv���	n�ȔFr�@�ȏƢB����ޱ���V���=�&m���h�@��_����r'�T_�^`��K�5.q�ǉV��#0͹�g��6��9�6��dm&�B����~L م���:���+[��	�r�Ͻ��h	��EkL�������Y�\H����a<XO��������=�i
�ku{���ۍ}W՛��t�h_|S8�X"����4W��K�~[V(��~M^x���CYΞp�)8e�#j��综#3E�baI������V=yrpN����8���0���e;?��K�=�ўt̂ �b�_C+�����\�����af�sG����&�ӻ�'F�={}�.h�����n��Y����ʎ�� ��hԾ� t�����sv�g�F�@HCzE��C{yjB9��6ǮEmj���m�[�Bzf�y�J����W�-�a�>��5'm`8�5n��pocS�o��30	��>�#�'��_(%��0��G:�Z�|��������w��m��g�..���>��E޳=��=�!͙�#�N���aI��$���l��yo��h���	S���B�ꬓ����!����>��ژا�I�%1!0���a|1�̄�y�s?e���!N�!ug�*�'D����%S��"�������@
&�{�h1Y��c�ER4C�yHw������
�Z,>�&�еv����R�Ò<��cRl��t�\��M$��a�ԧI�0��Ɖ��rC��;�Q�:��h����Գ��m���cFD�4̎�]� �ϡgzj��W /��r�r^e��SJps���L_~��,D69�''���7���U��+��8&~	�~�!���rgL�>p\�5�&5�����Y{�nQ����È�Á7A`N�ѕ	���+(#^�ŀ(#���w}�4���V��@�)i��'�Ƒ_��I�XȯdQ����%�[&}�8���Ai���"�-yg�V�4*Is>�	��I�\\��������^yL�O�Ndb$F���Acm�6�uڎw���Y��.�Xg���4k�f~3Z;;�X���m�RP��=�E� K��^([K�����`�XW�1�!�}h7�p�Y3 ԡ���7AODZ���':vq�ۡ��b�ژ����k��`��lq���d�AT�LV4���.%��koXE3<��O�ӂC���u� M|����F"l�N�T�6��X	v�ȇZA^�?���)�%@�X��������1}�NLdb½���7*m"�U0n[���� �y�i2� ��KY I�v�GqQ]�n�1�5�"�3���>�VH� �]�^L���;�KV��y.4����S$�'h�͇d�E��۠V"��H���>Ys�	m�"���#��^���4F}��D�3�opg�s8t�lŰ> ���U�]���u:Ê�B-��w�	�͗�a��'I�<��.���J��XF�@g�h��p�Ij�<��&���R��<�z�b��s�8ʱ<ϡ����(DZ���I�8���o�������������ߋ�d��x&��]���#W�n�X��p>�*��L���[�STI}$ſ$������\qgH5N+ 8�Ce�](*�OQi�ϱ��{�{1�c����~_I � �@:sj\��h`���	���g9r�s���:��Ҷ�	�����4�*����"M]�f�����s��|0b`��+Es����©�Za����Ao��q�/�R���W����~�-�~���X�݊���
SS�i=�ά��@�&w!��le�@X.�ws�$&����=��}P`� �S�#�~�C����ɛ�ب������W�W1R�oz! B[lz��W:6��0�Z_2�:��}�V��y��m�-T7ܫ}������,��U�M���ʏ�J����6�rc_1��A ��¡��#+��Ն��ۛ'�O4�"�C$��\U:�=��V���-���`����7�+��]�%7s�SaP��JFM����%�Xh����L��LQ�(uw�y<��0
��$Zk��� �t�a��#�qO��Q���&�w���@[��2�^a�\j�q�gε�[m�%�jA�t��@O�����o��bO+a��]��ƨ�#z؞���w�ŵ���&%eD	>,�[d^7��0�S��P�ˊ�j5�oJq��wO�v	 �	������L���j"�ޠR_�� )i&��}����~��_!Ҵ^*B��Y��A=hbr	D7dZA��a\'�2��8W�̋.�`l�S�}��_���/R��7�V�-��"�1��$�#U3c�ï�m���+�\ZsB�K+�]SWY��3�j��G�t� u�㰪!�l��4��}�_���S�Z�0��V$f]�_H4�>��Js�h|���;a�� m����#���!5l���;3����IH�/�fr��E|�$h���� �S�c#�e�"��Aޔ,�_�&�;1j���4}sQ���㷉�YgߚR�H����y�x�|?��1Y���3�K�rh\�[]�UȾS7��e�p'?|rZ���DT��)l���+�j�q%�b7���B�g���g�	����q���q����a*�4D�z�n�Pw��O~}�Ds��KC�j!��X�HF�?"��*��DV��&ޜ�7�Xl������4fuF�O�滷lpY�l�+|e�	 p!�4F84�r"��M�A������7��c�E�r�x���EC �	("�T��G�IN߾hYU����5>g�^o=T�FUq�[>[�f�cl��u��g�v���r�<>���^y[>q����;	x��F���Ļ_�O��]�-��؍ҋ� ��l��w�ݗ�ی�y�l���w���XdV�z�e�[,�l������o4W�A�Ӝՠ ᇄ!F"4+���BΌ���9������C�Ʊ%�6�����%~�B� "��t� I-l�k�]�lߏ���>4a?d�+��gr��,N�ͪV���|������9q�v��l�dH@ۭE{�Gܦ�X�H�q��؁�����Qr�t(L4+�+�U�{m:bȰI}��Ͼ{g4�>�(U�CU@�1e"T�?��KMs�ܜǱHz���HA����F Ŏiյ΋�7����Ϫ�(kg�]��!�m�/Զ\����p�q��AZ`e���~��,�nݬe�9���G"�rUĜ��8f ��&!�f�՜LƠ2��i��!I�v��`�k�W�K3N;ր�P��M	�t��1Y���ʍ���k��W,�A_U�2ظ���L(�;�:�x�8�Hw>�V'A)"��.�ϥn%��J���8�6�va���n�Oo����H_n�btDhHB�s��/+���tW���^�:��P����w`m����`���@7��ۭ,�;``����eg[oҥ����%���q"�F{��w��s��pR=G}@U�N�ڔ��)��<{�֋t*��z�ZX$�~�Q���P&.%�Vg�O8)���J���4�wF?�*ܫFhKl/�oZ�T���3g����N2��a�������H4V������]����wa{^ȟ��;�6 c��o��M0ߨ����?367ҟЁ(�u��v���������uXK�Q�}�pB���d�#����%�r.�Ǐv������W��;����C]X���v�@M������}fZ�0���]27���قQf��7_K{z�s-W���m����@�{����9e��f;��h���u4��6O��<�K�4S��S��yʬZ�KǱu0|��A7��/�t��T	�t�D4��Կ��R���9$]�ôƫ!I���;;<�z�]�D���YӬ�jf�G�E�<L�r6u��Ŧ���<�����a�N5���w�WC�������݇���i.0���9L+�>�eki��)Bڵ�'�\lE�2_�6oM�*E�l�i�Z�ƺ�\����"q��K;dۘ>s�����8.0?�I�����͘ ���Ү�;b��$��>������2cY��<$����Wó���秭��?��B��Ԡ�^FZ�A���ꋩ/��r�L3��U�nK�"���Q�h�r��b
g~�X��G1��vrFa��mM����s���r�rI� ��܀���D�2 �4Q�l�A߲
3
CB����ϑN���}J_)����K{+T�ej�8C
�1l��|y�-��g���	����ǳ�j�]�8|Rà�\��~�a�w�|�+�^b���-���d%���ZEb�f8;P�^�p9F��߂Ǥ2��'��n�4��($�ep�ys��uq7)�5�I��9�OiR�r�[�%DJ7��?�#s�����#�'FF��<[�������|�t�<�1����8r�4=�pr���O��R%˼GV6-�:t<yl��(����[b�IeS��t�K�Ƌg�����i�9�H�*�X-/8����uRG?���yjl���I�<��c�f���-6���13�-�����U�����W4�V���Ҭ�Y���7�љg-\	�|��n��ƾ�>�A�
���8�|���l��vc�&����=�R����ǱG]��$w��<]��и5���>#�+��Enw��3�֙;ͤ1��c�q�1'q���*�!�!�F`��Eb�v�p�lW���m�����G9�o(��1B�\�r�j�	���,L�<3��70���v��oCq��x"~s&d� �U�]�,k�W��"�LQ��`�[�q��5�L��S?}��@��Cc?yߢ
n�!d�01�kB�0D#륦&��bx�ױ� ����$�a���sj���[h�ݼ�j�U�}���w����.���<��bANOC�(Gi|^��|�Zʪ�[5����Q0�Sex�Zo�&4��م�!�$�n�����x>wn�I�~���׳�����V�2>���ӂ<����#ӯrRU�`iC�JT�QL?V{�H͋)�Y.#��2�buh�_���{s.E)�oN����^�����\|��N�����Q ӥ��ӓ�Ia�)c��r��9��v��^�GZTf�r_-o/��K�WtO4�MeJ6,Յ�&���Ϭ��~�5�&���Q!�s������}k�f �1傿(�"&J���y�0�����Ep�l�Mg^�싱�3��/`�I�{��B�~��
G��4��y��:�X}"�����5DtU���]T�����f*p�59��ޘh��j�jL��a{�Т_f��?�����M�9\JR�����*\m֍8w�_�2�\���ǟ��nr����X��6a�b�
��xHp�s���hč">�e<��/���a�9
Q#��a��-�
#��RG!7{��&��D�:5u�y�����;�Ҽqk��� ��b�Q_M
~J� �] �tF���v�zȡ�ϹiTV7^��["�l�D�?��`��\|�}ʂR'�7����j���^J���)����DF87��z���!h&$�Yv��X��j7��0��>�����K2�Ϭ���+3�2�G�P�����48��R��&
�Pְ��k�W�Z��s�g���' ���N��
�pB��u��;�uIW���^Q{�-� ���o�+�%��m{��O;�-9��XڻN�Bp�9�0�`V�n���AZ+K� ��q���EX|R�@w�2�'���}`;���T�f�	��KS8��=���cv���6��җ�I�B�hq�LѲ������T���,��.��o�$A����ݭ�+	c?c�g�cq��U��*(�rW'����V="�3=.�=��C�wO;�S�et���]'���]B�@�#d��|+�#�`?��j��}�B�%��)?R�
@��#� ��_8�X���!��$�4����2?�C-�4pJ��:ZO���N��N+S��z��b��(vp�oL|aҰ�P�;��
H,O�$�R͑����R ����>g���"�XB1�K���W������P�_�;����K�M6��eV�|��&���!(��X��S<�MW�<�x���<�W�|3���_�לO��gw?]VOg�p��[�$�^T�֥4�f�5�DU�<沛�Z�
a�Q����您9��;[Jc��!U=>�\>@��D��ݮXCj�1�~)���u�S��۞�7�; SL0���I%0�3H���Y������
Hȥ�g����Z��8z�r�Ûh���\}��,��V_e��-�c�R��shˬ`B�EJ�1�ōV/�̏Y�[�=�X�k�~�U��?�IW�]�I(b����a����'V*��l� +tO���>Ȇ�s���Ah��׆f���"���5���W��Q�s���$+ؠ�7|��P���ֹ� Q�0Uw�씿ʡ�F���8־~��B����E�7����_��61�#�����j��q	K/&�(*�@mW�_pL��Yq��������>�W�,��oUd�7��b��Fh�fn[t:��T���)V���'k����.~�6�Z��|mH������a:�Q���&��"o����̸K����M#��l�4��ڌ�����1�>��lU�J�hMbJ%�+Z;����(��Ѥ�.����ߵ�װ�)�5y�4��п�Œl�����U����!ѱ���gc�A�h��f��N
`��:v��]_��n2J5�"��>g��T KI��t�
��l%͖V�x��	e for the next lock_list
		 * entry, see the comments for the function.
		 */
		trace = entry->trace;

		if (depth == 0 && (entry != root)) {
			printk("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}

		entry = get_lock_parent(entry);
		depth--;
	} while (entry && (depth >= 0));
}

static void
print_irq_lock_scenario(struct lock_list *safe_entry,
			struct lock_list *unsafe_entry,
			struct lock_class *prev_class,
			struct lock_class *next_class)
{
	struct lock_class *safe_class = safe_entry->class;
	struct lock_class *unsafe_class = unsafe_entry->class;
	struct lock_class *middle_class = prev_class;

	if (middle_class == safe_class)
		middle_class = next_class;

	/*
	 * A direct locking problem where unsafe_class lock is taken
	 * directly by safe_class lock, then all we need to show
	 * is the deadlock scenario, as it is obvious that the
	 * unsafe lock is taken under the safe lock.
	 *
	 * But if there is a chain instead, where the safe lock takes
	 * an intermediate lock (middle_class) where this lock is
	 * not the same as the safe lock, then the lock chain is
	 * used to describe the problem. Otherwise we would need
	 * to show a different CPU case for each link in the chain
	 * from the safe_class lock to the unsafe_class lock.
	 */
	if (middle_class != unsafe_class) {
		printk("Chain exists of:\n  ");
		__print_lock_name(safe_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(middle_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(unsafe_class);
		printk(KERN_CONT "\n\n");
	}

	printk(" Possible interrupt unsafe locking scenario:\n\n");
	printk("       CPU0                    CPU1\n");
	printk("       ----                    ----\n");
	printk("  lock(");
	__print_lock_name(unsafe_class);
	printk(KERN_CONT ");\n");
	printk("                               local_irq_disable();\n");
	printk("                               lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("                               lock(");
	__print_lock_name(middle_class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_bad_irq_dependency(struct task_struct *curr,
			 struct lock_list *prev_root,
			 struct lock_list *next_root,
			 struct lock_list *backwards_entry,
			 struct lock_list *forwards_entry,
			 struct held_lock *prev,
			 struct held_lock *next,
			 enum lock_usage_bit bit1,
			 enum lock_usage_bit bit2,
			 const char *irqclass)
{
	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================================\n");
	pr_warn("WARNING: %s-safe -> %s-unsafe lock order detected\n",
		irqclass, irqclass);
	print_kernel_ident();
	pr_warn("-----------------------------------------------------\n");
	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] is trying to acquire:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		curr->softirq_context, softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		curr->softirqs_enabled);
	print_lock(next);

	pr_warn("\nand this task is already holding:\n");
	print_lock(prev);
	pr_warn("which would create a new lock dependency:\n");
	print_lock_name(hlock_class(prev));
	pr_cont(" ->");
	print_lock_name(hlock_class(next));
	pr_cont("\n");

	pr_warn("\nbut this new dependency connects a %s-irq-safe lock:\n",
		irqclass);
	print_lock_name(backwards_entry->class);
	pr_warn("\n... which became %s-irq-safe at:\n", irqclass);

	print_lock_trace(backwards_entry->class->usage_traces[bit1], 1);

	pr_warn("\nto a %s-irq-unsafe lock:\n", irqclass);
	print_lock_name(forwards_entry->class);
	pr_warn("\n... which became %s-irq-unsafe at:\n", irqclass);
	pr_warn("...");

	print_lock_trace(forwards_entry->class->usage_traces[bit2], 1);

	pr_warn("\nother info that might help us debug this:\n\n");
	print_irq_lock_scenario(backwards_entry, forwards_entry,
				hlock_class(prev), hlock_class(next));

	lockdep_print_held_locks(curr);

	pr_warn("\nthe dependencies between %s-irq-safe lock and the holding lock:\n", irqclass);
	print_shortest_lock_dependencies_backwards(backwards_entry, prev_root);

	pr_warn("\nthe dependencies between the lock to be acquired");
	pr_warn(" and %s-irq-unsafe lock:\n", irqclass);
	next_root->trace = save_trace();
	if (!next_root->trace)
		return;
	print_shortest_lock_dependencies(forwards_entry, next_root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static const char *state_names[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE),
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static const char *state_rnames[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE)"-READ",
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline const char *state_name(enum lock_usage_bit bit)
{
	if (bit & LOCK_USAGE_READ_MASK)
		return state_rnames[bit >> LOCK_USAGE_DIR_MASK];
	else
		return state_names[bit >> LOCK_USAGE_DIR_MASK];
}

/*
 * The bit number is encoded like:
 *
 *  bit0: 0 exclusive, 1 read lock
 *  bit1: 0 used in irq, 1 irq enabled
 *  bit2-n: state
 */
static int exclusive_bit(int new_bit)
{
	int state = new_bit & LOCK_USAGE_STATE_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * keep state, bit flip the direction and strip read.
	 */
	return state | (dir ^ LOCK_USAGE_DIR_MASK);
}

/*
 * Observe that when given a bitmask where each bitnr is encoded as above, a
 * right shift of the mask transforms the individual bitnrs as -1 and
 * conversely, a left shift transforms into +1 for the individual bitnrs.
 *
 * So for all bits whose number have LOCK_ENABLED_* set (bitnr1 == 1), we can
 * create the mask with those bit numbers using LOCK_USED_IN_* (bitnr1 == 0)
 * instead by subtracting the bit number by 2, or shifting the mask right by 2.
 *
 * Similarly, bitnr1 == 0 becomes bitnr1 == 1 by adding 2, or shifting left 2.
 *
 * So split the mask (note that LOCKF_ENABLED_IRQ_ALL|LOCKF_USED_IN_IRQ_ALL is
 * all bits set) and recompose with bitnr1 flipped.
 */
static unsigned long invert_dir_mask(unsigned long mask)
{
	unsigned long excl = 0;

	/* Invert dir */
	excl |= (mask & LOCKF_ENABLED_IRQ_ALL) >> LOCK_USAGE_DIR_MASK;
	excl |= (mask & LOCKF_USED_IN_IRQ_ALL) << LOCK_USAGE_DIR_MASK;

	return excl;
}

/*
 * Note that a LOCK_ENABLED_IRQ_*_READ usage and a LOCK_USED_IN_IRQ_*_READ
 * usage may cause deadlock too, for example:
 *
 * P1				P2
 * <irq disabled>
 * write_lock(l1);		<irq enabled>
 *				read_lock(l2);
 * write_lock(l2);
 * 				<in irq>
 * 				read_lock(l1);
 *
 * , in above case, l1 will be marked as LOCK_USED_IN_IRQ_HARDIRQ_READ and l2
 * will marked as LOCK_ENABLE_IRQ_HARDIRQ_READ, and this is a possible
 * deadlock.
 *
 * In fact, all of the following cases may cause deadlocks:
 *
 * 	 LOCK_USED_IN_IRQ_* -> LOCK_ENABLED_IRQ_*
 * 	 LOCK_USED_IN_IRQ_*_READ -> LOCK_ENABLED_IRQ_*
 * 	 LOCK_USED_IN_IRQ_* -> LOCK_ENABLED_IRQ_*_READ
 * 	 LOCK_USED_IN_IRQ_*_READ -> LOCK_ENABLED_IRQ_*_READ
 *
 * As a result, to calculate the "exclusive mask", first we invert the
 * direction (USED_IN/ENABLED) of the original mask, and 1) for all bits with
 * bitnr0 set (LOCK_*_READ), add those with bitnr0 cleared (LOCK_*). 2) for all
 * bits with bitnr0 cleared (LOCK_*_READ), add those with bitnr0 set (LOCK_*).
 */
static unsigned long exclusive_mask(unsigned long mask)
{
	unsigned long excl = invert_dir_mask(mask);

	excl |= (excl & LOCKF_IRQ_READ) >> LOCK_USAGE_READ_MASK;
	excl |= (excl & LOCKF_IRQ) << LOCK_USAGE_READ_MASK;

	return excl;
}

/*
 * Retrieve the _possible_ original mask to which @mask is
 * exclusive. Ie: this is the opposite of exclusive_mask().
 * Note that 2 possible original bits can match an exclusive
 * bit: one has LOCK_USAGE_READ_MASK set, the other has it
 * cleared. So both are returned for each exclusive bit.
 */
static unsigned long original_mask(unsigned long mask)
{
	unsigned long excl = invert_dir_mask(mask);

	/* Include read in existing usages */
	excl |= (excl & LOCKF_IRQ_READ) >> LOCK_USAGE_READ_MASK;
	excl |= (excl & LOCKF_IRQ) << LOCK_USAGE_READ_MASK;

	return excl;
}

/*
 * Find the first pair of bit match between an original
 * usage mask and an exclusive usage mask.
 */
static int find_exclusive_match(unsigned long mask,
				unsigned long excl_mask,
				enum lock_usage_bit *bitp,
				enum lock_usage_bit *excl_bitp)
{
	int bit, excl, excl_read;

	for_each_set_bit(bit, &mask, LOCK_USED) {
		/*
		 * exclusive_bit() strips the read bit, however,
		 * LOCK_ENABLED_IRQ_*_READ may cause deadlocks too, so we need
		 * to search excl | LOCK_USAGE_READ_MASK as well.
		 */
		excl = exclusive_bit(bit);
		excl_read = excl | LOCK_USAGE_READ_MASK;
		if (excl_mask & lock_flag(excl)) {
			*bitp = bit;
			*excl_bitp = excl;
			return 0;
		} else if (excl_mask & lock_flag(excl_read)) {
			*bitp = bit;
			*excl_bitp = excl_read;
			return 0;
		}
	}
	return -1;
}

/*
 * Prove that the new dependency does not connect a hardirq-safe(-read)
 * lock with a hardirq-unsafe lock - to achieve this we search
 * the backwards-subgraph starting at <prev>, and the
 * forwards-subgraph starting at <next>:
 */
static int check_irq_usage(struct task_struct *curr, struct held_lock *prev,
			   struct held_lock *next)
{
	unsigned long usage_mask = 0, forward_mask, backward_mask;
	enum lock_usage_bit forward_bit = 0, backward_bit = 0;
	struct lock_list *target_entry1;
	struct lock_list *target_entry;
	struct lock_list this, that;
	enum bfs_result ret;

	/*
	 * Step 1: gather all hard/soft IRQs usages backward in an
	 * accumulated usage mask.
	 */
	bfs_init_rootb(&this, prev);

	ret = __bfs_backwards(&this, &usage_mask, usage_accumulate, usage_skip, NULL);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}

	usage_mask &= LOCKF_USED_IN_IRQ_ALL;
	if (!usage_mask)
		return 1;

	/*
	 * Step 2: find exclusive uses forward that match the previous
	 * backward accumulated mask.
	 */
	forward_mask = exclusive_mask(usage_mask);

	bfs_init_root(&that, next);

	ret = find_usage_forwards(&that, forward_mask, &target_entry1);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/*
	 * Step 3: we found a bad match! Now retrieve a lock from the backward
	 * list whose usage mask matches the exclusive usage mask from the
	 * lock found on the forward list.
	 *
	 * Note, we should only keep the LOCKF_ENABLED_IRQ_ALL bits, considering
	 * the follow case:
	 *
	 * When trying to add A -> B to the graph, we find that there is a
	 * hardirq-safe L, that L -> ... -> A, and another hardirq-unsafe M,
	 * that B -> ... -> M. However M is **softirq-safe**, if we use exact
	 * invert bits of M's usage_mask, we will find another lock N that is
	 * **softirq-unsafe** and N -> ... -> A, however N -> .. -> M will not
	 * cause a inversion deadlock.
	 */
	backward_mask = original_mask(target_entry1->class->usage_mask & LOCKF_ENABLED_IRQ_ALL);

	ret = find_usage_backwards(&this, backward_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (DEBUG_LOCKS_WARN_ON(ret == BFS_RNOMATCH))
		return 1;

	/*
	 * Step 4: narrow down to a pair of incompatible usage bits
	 * and report it.
	 */
	ret = find_exclusive_match(target_entry->class->usage_mask,
				   target_entry1->class->usage_mask,
				   &backward_bit, &forward_bit);
	if (DEBUG_LOCKS_WARN_ON(ret == -1))
		return 1;

	print_bad_irq_dependency(curr, &this, &that,
				 target_entry, target_entry1,
				 prev, next,
				 backward_bit, forward_bit,
				 state_name(backward_bit));

	return 0;
}

#else

static inline int check_irq_usage(struct task_struct *curr,
				  struct held_lock *prev, struct held_lock *next)
{
	return 1;
}

static inline bool usage_skip(struct lock_list *entry, void *mask)
{
	return false;
}

#endif /* CONFIG_TRACE_IRQFLAGS */

#ifdef CONFIG_LOCKDEP_SMALL
/*
 * Check that the dependency graph starting at <src> can lead to
 * <target> or not. If it can, <src> -> <target> dependency is already
 * in the graph.
 *
 * Return BFS_RMATCH if it does, or BFS_RNOMATCH if it does not, return BFS_E* if
 * any error appears in the bfs search.
 */
static noinline enum bfs_result
check_redundant(struct held_lock *src, struct held_lock *target)
{
	enum bfs_result ret;
	struct lock_list *target_entry;
	struct lock_list src_entry;

	bfs_init_root(&src_entry, src);
	/*
	 * Special setup for check_redundant().
	 *
	 * To report redundant, we need to find a strong dependency path that
	 * is equal to or stronger than <src> -> <target>. So if <src> is E,
	 * we need to let __bfs() only search for a path starting at a -(E*)->,
	 * we achieve this by setting the initial node's ->only_xr to true in
	 * that case. And if <prev> is S, we set initial ->only_xr to false
	 * because both -(S*)-> (equal) and -(E*)-> (stronger) are redundant.
	 */
	src_entry.only_xr = src->read == 0;

	debug_atomic_inc(nr_redundant_checks);

	/*
	 * Note: we skip local_lock() for redundant check, because as the
	 * comment in usage_skip(), A -> local_lock() -> B and A -> B are not
	 * the same.
	 */
	ret = check_path(target, &src_entry, hlock_equal, usage_skip, &target_entry);

	if (ret == BFS_RMATCH)
		debug_atomic_inc(nr_redundant);

	return ret;
}

#else

static inline enum bfs_result
check_redundant(struct held_lock *src, struct held_lock *target)
{
	return BFS_RNOMATCH;
}

#endif

static void inc_chains(int irq_context)
{
	if (irq_context & LOCK_CHAIN_HARDIRQ_CONTEXT)
		nr_hardirq_chains++;
	else if (irq_context & LOCK_CHAIN_SOFTIRQ_CONTEXT)
		nr_softirq_chains++;
	else
		nr_process_chains++;
}

static void dec_chains(int irq_context)
{
	if (irq_context & LOCK_CHAIN_HARDIRQ_CONTEXT)
		nr_hardirq_chains--;
	else if (irq_context & LOCK_CHAIN_SOFTIRQ_CONTEXT)
		nr_softirq_chains--;
	else
		nr_process_chains--;
}

static void
print_deadlock_scenario(struct held_lock *nxt, struct held_lock *prv)
{
	struct lock_class *next = hlock_class(nxt);
	struct lock_class *prev = hlock_class(prv);

	printk(" Possible unsafe locking scenario:\n\n");
	printk("       CPU0\n");
	printk("       ----\n");
	printk("  lock(");
	__print_lock_name(prev);
	printk(KERN_CONT ");\n");
	printk("  lock(");
	__print_lock_name(next);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
	printk(" May be due to missing lock nesting notation\n\n");
}

static void
print_deadlock_bug(struct task_struct *curr, struct held_lock *prev,
		   struct held_lock *next)
{
	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("============================================\n");
	pr_warn("WARNING: possible recursive locking detected\n");
	print_kernel_ident();
	pr_warn("--------------------------------------------\n");
	pr_warn("%s/%d is trying to acquire lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(next);
	pr_warn("\nbut task is already holding lock:\n");
	print_lock(prev);

	pr_warn("\nother info that might help us debug this:\n");
	print_deadlock_scenario(next, prev);
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Check whether we are holding such a class already.
 *
 * (Note that this has to be done separately, because the graph cannot
 * detect such classes of deadlocks.)
 *
 * Returns: 0 on deadlock detected, 1 on OK, 2 if another lock with the same
 * lock class is held but nest_lock is also held, i.e. we rely on the
 * nest_lock to avoid the deadlock.
 */
static int
check_deadlock(struct task_struct *curr, struct held_lock *next)
{
	struct held_lock *prev;
	struct held_lock *nest = NULL;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		prev = curr->held_locks + i;

		if (prev->instance == next->nest_lock)
			nest = prev;

		if (hlock_class(prev) != hlock_class(next))
			continue;

		/*
		 * Allow read-after-read recursion of the same
		 * lock class (i.e. read_lock(lock)+read_lock(lock)):
		 */
		if ((next->read == 2) && prev->read)
			continue;

		/*
		 * We're holding the nest_lock, which serializes this lock's
		 * nesting behaviour.
		 */
		if (nest)
			return 2;

		print_deadlock_bug(curr, prev, next);
		return 0;
	}
	return 1;
}

/*
 * There was a chain-cache miss, and we are about to add a new dependency
 * to a previous lock. We validate the following rules:
 *
 *  - would the adding of the <prev> -> <next> dependency create a
 *    circular dependency in the graph? [== circular deadlock]
 *
 *  - does the new prev->next dependency connect any hardirq-safe lock
 *    (in the full backwards-subgraph starting at <prev>) with any
 *    hardirq-unsafe lock (in the full forwards-subgraph starting at
 *    <next>)? [== illegal lock inversion with hardirq contexts]
 *
 *  - does the new prev->next dependency connect any softirq-safe lock
 *    (in the full backwards-subgraph starting at <prev>) with any
 *    softirq-unsafe lock (in the full forwards-subgraph starting at
 *    <next>)? [== illegal lock inversion with softirq contexts]
 *
 * any of these scenarios could lead to a deadlock.
 *
 * Then if all the validations pass, we add the forwards and backwards
 * dependency.
 */
static int
check_prev_add(struct task_struct *curr, struct held_lock *prev,
	       struct held_lock *next, u16 distance,
	       struct