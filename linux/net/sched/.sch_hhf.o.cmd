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
  arch/x86/includeg&Ëƒ{¤¢~ªĞ)Z®7ÖãÓu]œ…T¿˜ğ²¨êr…[Q9‰Ağûnÿ3³ v†æœQ’”^1„ ¡ÇÎ™ú<š?”Œä‘æg2@ÁåÀ¶ö÷8(;OYHºI2CœÊ@HjÇßY@»¨°¹»qôÒî¦t
Xp†€®?öfx›0¢•±.ƒ™làÑ i.ÌgÉ”í ¬˜(5B02–Àu¨¢îÊíPĞæãY€66×E ·|ñ+_.9õcÂ>‘Öú“Àã±zXu¯Y2Eİ`éM&ÏR¶JÌ÷‰†–Öì.æ¿é:,'ÙuX°xkaú‰ÿyÃ$ä©¥œ$3”Ù™£ŒŞSN#ğíVÈ¥
ÍÜÿD¶Æ;dqh¢›¦£3G-Uö@	©T¡NìËÿÛ<é´ŠYoUŞ%-øˆXÎÓ¸X·´©W÷€ B™LvĞñòqãU›D\»ÒJCïº¹³o!‘hõÅ8i„YöOmšp×$÷”@CqW½6˜-C ‰B†–o•;Ñk ÑM³‰îÕ¬ĞóÇ¨(˜äôY_c˜ÛÖ3ˆtXº®JBŞ½ÍÈıGn}ã /ôÉ4%48öært¯ûïriâ¡Oô²jºÆñÍ>âËÍS¶u]*f	ˆÓ3[y2œXƒ™5ùtÄ9bÊ~úÆ¢`âËN¶|¥Ä“T4i”ãÆsÿ”“šZù/‹Vkãû|PüÓ‰êíBÏöğÔ`Ğ¦î!z!“â?§è;èWU¨J¹tÊó ´†)¦#~*z±Ã¿Ò>ÒkİSX‡‹ûôşû¶Ú2<¡¾¼è2="¢ßû¾${Òn ìˆi^;£'´Š!¢t­i€*¦ø5ªİ·è›s’ãÄ1ˆæB¿+©v·Xl6y©s; 7.ÁmøåìY[òvlà´<ã3™ÆÄ–«3B¼”¦a"ü÷î6‚ñ#D‚tŞÛw1‚Ù¦‚J›	^<{°–‡Çc^›ZlEr^„Ş¯}@Œ)¼êÑÜöÃü&{ÖöæËÀ7PıŒ_æ³›¬ˆ½ÛÇŸ‚H bãTøIoÑâyÚ¿"Ï¨•­hy\N³¾Uˆ•ó {˜õĞPÜjMU
ÿfrjòìD?_zÓ}!i÷—%ÿ)”e*jîZ£¢2Ù&Ãœß‹ìŠ6•Ã>¥h»ÙX¤Š3-Å:9yB?MQ7gµ€A‚äğ~ç ³º—·C‰Üôfx@X«tKá¹Ğ_Áñ=|»Né¾âÛ¹_ğ)Ôºç6½fÏûyRlY´®Ác(d1T Î8ŸïÙ¡p„÷¼ +´\T¥:ó’<bğ»
%T_¬·-DŠï2‹D»ino÷ĞjÊ°¬n‰4æ•±®;ØHz8 #Ÿ2{-Ê¾‹ë³é‡D@óˆn×‹
ò°ş™ò2Ú¹Ãî]V¹âP}Ğ8Mûs|_ÒcÚ8Ó-ÿêd`TÒê•9'<÷«&rÇ”CÆg“ßØ7ë(~éœ€CR¢/&yv®×">±dsúQ.×ÈåLD¾¨ 2nÈønü±x­ bkÀ"JOŠ«¦ÿÙ&­èj<Í³nÉêÈeZ¡Ù²¬úRNB¦ÚÚP7¯)çªoËÔvÍ*'¿e×éåjÙó»eÆ²Ì‘¡åUZ+ÜêÜWO–ôc°æéYŞèMø/á†6êTê›êF<öE»_œ6î„šˆË×<şÖQ/±ø¾gİR–KçJ¦hp¨¡·>ï6RZ´ ˜Põ£ß¾|^‰šÿ0ô
\(L»˜2Æ$QãÙ?Ö3ú„£lw‹5”Æe:jñQ¦ı”ÜUeòÖ÷$df°0âeÉNdr›ld×ñÅUéªEIÛäC‚9	¼‰XÚ:ŸÎ2@ø¡ÂãQ	ùMîòú`]¸“‰¯	†¥$¿’(Î:õ€åë?Hôºê)2ÒDlÚ^‚ë‡z
ñp&şY²³§] KKZÌ4òfŒõ'{åï?ıƒ6ÜùX3Ò°ßaí”7e9&}ğ7İc¦›#bÆ‘_İ¯¯ãäxÂA›Tá«`V"‘snªfòÇ'`Ğr±ù£C?Ó‘¸ŠöÌÎ;é"€L£é:nm‰ú¥‹•|û_)ƒ#yâîÅeíjMu4FŞ3ûìT øçúmß#&céíñqI
vVåsÑdøvee›¡¶DÁgççˆŒ$šda ‰q³î7”K¾Ùáø\[›[²MS¿ó’ÍKÑgecÙPe»Å„H”gÊf;Al¢H+|ÿ¶å¶ÉŸDb)Ì~wf_}¥+µ â&ØL
¡úÈİbS!Ö_×ë7É IúßxªG’÷®ß‡ÿ' ú©Àä¨cöeØ-Ã˜>{œ»©}£:¾€]1ÇÎ¢sd	p&zÙ ;ºÜ”#–º–“Àµ‹)£ä€S×EJÍ¸-ob7ôFŞ“‡c­4¯Ëb¼®—¼É·pˆüò®qô]> ˆÁıÔŸ¿2ë
áQD4—‚:üpÓh0KV˜,8³yÏ#&Oo(ëILŒü5ƒà™–ñÔ5@£Ÿg,é5:·¦‡·dÓ‘B)NwûM‰aªÀÓm»«¿}¯;v¿Ö–´ÀÍ$œ<~fİ‚c«Â“Ú~øÒBÛBÎ2°úİæAÚ›\¢ÌÊËl¯Ø³€¯m»:ü¤ÅİqÓ¢m¹`ñN
í>g–ŞĞYí©°öMğ…+Q&},\9gÃP,üLŸÍñ’ßŞäá^i
\Ü_GÕ\™²6r‘4=ÁP¥b¹µÊ¨şé[íC6EÇÍ3¿†Ş2f á+/!ø‘–µõî±¬ıqÍ»´´İl“öË›ÈÓ<½@ëº†Êiºb*º÷˜HÄo·#¢ÆÓˆ1Ê§‚²‰–l-ö
õŸiÿÂÇ~%–â(HÔî—ûA¹v¡‚—ü^ü®İ"	‹í™ïšà/`r-8Ğù¥…ÓD‡NÛ3PöĞ™fßËïù”ì»6ÏàÍmÄ0®Jê7m§Ì¤1ˆÉä‘&T)äáÊsF²ËhsŒ ²ƒr*ì€×øğ»uÁecÍw`¸²¸YB{,“oÜ¶{|lëğ7ıéËn,Ãï¤À¼”mµõaErïp}òsÓ£¾Â&¬òÒªzø]Õ³7gßN„`%÷4¥Ÿ}zPy.I½ò'ğ$g3¸¹AJæÏ~xQ™Az†°‹ök/À$ —€ÛÉÅ¯×o#€ì¸¢1Ht-Ôb~\ÉúÔc,tk	Qİ›ñ]29“Â¥‘é'šD‘øïM—ÍÔaŒxĞ;}ƒ¾~vÛ¼|FÜ·vŸİÊéÉ°yÀğ•ÇØ?_üt¸ø#	¿­ê²õÿL¼?vÅXB(–«Ã7
°«’wµ—Ê#˜9,?£4 èÛ)îÑ:£76'ßìÖDõbÌ +"×™ù’m”¼³3eÉ€‘Øñ70å¸Ãyºxygß
”±\ÜÖq*áÚ8•æ hYeòşÖ"r¦Åmor‚9×ÅÅ!Ó«ÍGò.íFŸ	Ä#ò‡<àUşˆB¶^5LVQƒ¶İ<½xuˆècª<t%'ôe¹AîŠÙÊ
@©lƒa*ÆjEAã	A6IxyPBòÅ?ôM[´’Y×ı½5ûÆ%c.L ¦øáÖ‘
îñ§z?Æó¨•6I»DìünXA×ßhzÔ³òûÛ!”òräúå4ÿ…‘d.CB«>uúú¨ZöĞl¦×ê°Ú°¥j]8Œ›õ+°:AkÑï}mk·½“V(»|¹?Şÿ{.êíO5t&/óS4pUòn_JØkVÑˆ6ŒjŒmDÊ¨ÕSKÇLì‡.Pgë¶ Ø;ÌŞNF>¯í¸|n7ğzğÑeı+Qú1wBûáÊãÚ‘± _½XŸÄõp“.Ç(óBqñ”©¾“0®û¬İ‡áE!ín›·!×’µ#Ğ ¯3$)ËØT ¯›Õ¦p†¼M M‰ÔX×Y±¡ÃdWQi“ÊV¬r©xªùÜ«_ïz}bá—­ªDƒú0Õpç—a‹–Ã °5	‹/Œ`Ó•1	@üZ”¯SÀ]ıpuÉÌŠ†ìY®‰›zM’ƒQA–æ®)§‘ğİÙŞúéY»ÆÜ@İ@®µŞ½q u™”ıü›‰_0şX8Í¾4®¿yš*¡”¤Sìş1¶6¾L)0ÄáÿÑ1yK‚Åö®¬1>÷¡ÄªZ>Â›qf¥îŠ¿¤5'ÌÃêøİWK<õF´®§xÚ©“$`~‰ZCşî«Õ©Ğl*Zé¢¶¶¢Cô‘;¼oPÑ»=†òn0ˆ‚µ€T†k~©Åˆ¬ÈÆ÷s¸¨;gfŞ,ŠÒªÔı*õx-ØÀ³óëéÿ…\¤©kú5Ë¿²ç³Zi§&Qã7ÉãYfZsñÂSo<—¿3¿£’ÕwİÛ àÃ¯›ÁÏ­
+ËâQ5Èx_Á5ëæo‹ù»d\1÷;-A¬5­“Vt‰} §•0iAğ­¡èpŒà¬™ø’í	ÛçX¬ù=)dˆ ÷Öçû}2?*¹diÅJ²°b,ıÔL[ñOÛ},V”¢k€aŸ¼àa­3Ehê¨õGÒÄÍM/Ë)¦9écE+HĞmƒ­Átó5
,Õoeã¦,ı›‰únÛªI»µuÆÁ~„'Ú4€>×>*å©ZïÔíáñ_Ad;¸vı‹NBf€Mì‹çô²í~}@ÜeWA“b[“?yA«ˆÈÏ•pG“ï—6¡¤ÏöÂPÔ¬ì¶˜Ôe:öiJ·H”w='`’0Íóäãİè(ü¹áf¹3èn£³˜xj‡ïÓç+7 ı¦º}ğF£ dĞQ¢à4ã¦‰ĞBÓé´"ObôI€T.Š’1µ9‚ÕÌûÕƒR¢ÍÒmÉ™F’nÚµO¶Qé›rs…{npÌb6„Á´:PğIÁ·ì`6KâéÁE§`<é_ÃØ0êí!Ñçd‹cqÀ`¨¢VPK¨}aTá/‡Ë~Ñ±oDJ1_6Éˆ”ùÎ8Ã.ÚãlÑkf“ÈŸšÙpÊ×ø	G´sº{/Œ¢êDºyÈw›¦[ïà4Hv#,ŸõR€İÑ\ˆP#`Æ‡]YêøçN‘ š†¬Î8¸ËebÆ»4¾}­¨N£fkG?æ$îqòwNi¸P,m’ë†ìƒ©·^‰fø•¹a*.xFf iXG¼o;ÅzA§u•’]¹&ÔäF ââDr4g,V
ƒ¥½¹ˆ½Q<Ó®L^}cÈi”3ÿoJĞ³­/oïU›°³}°a‰§hs,ÖUzFÏØ‡oeİ¶Úté^œÀÕO.7Ño hzŸÆJø‘Gu¿UÛ°ıä|½Ee¶|,‰x€xºJäw˜RË6äDş±X÷s=ˆ ï&¾q3 [G0³h@ÅÆÍ³œ/ã¬°›•?şmì¼,{:tˆÂò6f²š•yœ’8_k ŠÈ¬ìS]—‡«Êïª.ààTÛE
Òºõ).:Åä&Êòâ ^ö„¬ÀÍMÈK¨ä7]¢¾À÷8Ğ¶ó ƒ\¾f«[±XÍK£Öôf=F3è°U«Üµğ˜÷‰$cAq³ZÌÆ`;=^Ñ§Ûzî-·ĞœôšnCF2Y›%ÆS-‚h&ÃR¯§F3¢iP·ëDŞ7-3plÊ7A{¥Q**óÊ&²Á¹8yÙb¶ıWÔ»â-`æ-k:œ?®|¢Şw¯$,B>uwœïÍµŸLRÎ.]’Á«t¬bíA¬zlßo7Œl³0%Í1ÙpšÀ6Y.¸ı»ÉÔ¶Ô3$ÍÕ$•Á0rz!Èæé%¥rPó.Ó:Ôwa^uÿ‹D`Lü§£8l”;*™­„ìA»½'`
\F–îÔéË¸S0Ù`|AÛè™îwˆÂjú û]¸‘åg‡ïÜ·pAj!Ğ²WÊç†f—­…Y}}Şª`t¦öœQ$LŠÄñŞ#ÉŸˆzN„êa=Ï
uq|÷ÃE¹-SÖ®«]ÅõÏë™uç$’a‰Û€[s@øèáU{Ø®º¿N,·6È;Àù{ÉøöØÚøP™Æù|Ìqî,¨öÑ‹OU2ÑÜ¿£â·æYA¤{örˆ³§ğ¿{ÕºŸÜÀ¡0Š¿
ºÿT›ˆ@…Î½,zgì„&”,°µ@} 8'<m*[à±Ìºú,¯4pÂóÇ|Â2%Aw¬lŠÿÄ™¡&÷Ø‡^	Ùìvÿ,éà½"€îY²é+Ë¶‘3Çl…¤ÉVWx³Íê´×SËå²Ç+ˆ"nša-dÙ«Go·ñİ4ì÷S›C¤*²™{¼m6ñé~ˆÚÓbqÿjŠå‰ª>7	EÛr±•òDÎš0˜—G3Nòb˜¬ZZª¼ˆH¯äÖ«6%­ñ]£Bm¾„SÉƒ>wÅÆL-x5zf"Læ‘E 
``ü>^]kA9™n€PKò‰³^š³É÷T¹¾Í¿¾TôÑÿ•òRÛE›"WÕDÜE8ò.²"´¼o)_PG9£ú  =¥Z…ûC]hÆ>Si9bÆü)BD@åŸ@Òyósé{dMOW¶tÿ%í¢¨É/Ä÷€q,ôî]¨–#ÑYöYdK¦‚¯OóçœÂFî¸1¨§Å[IY™3»Ig_t5sì˜HåHèXJûGQÀv İ3õ¥xVïD$ş±Ã·‘ÒKĞ^…£#ø¦*[E¿§ÃM”Ş?…dñÚ†Ñë¿"~¯Ö—·ş(ÌZ?>ı˜PòŞ¸I®è‡$ş
Óçñ ß±2TïS©œƒ¢R„±ŠkPù=6Û–uÖTòı7%W€jZ=ÉTÏÖ–ò¨ÌïcÖ·<ò”§Í"@øn»y,©
ÀöÛš¶Ü:$fÅÄ™3ü¸2/ÃÎ¹¬¬³)ksç}êëí7Æ´/ªĞ8¸š-H>
Uãp(Ë3Dd|À·ÔLšå&[qO›œGßÖªfÖËD_Ü
½·›h•Œq)êÉw±Ÿlnÿ*}}<j[)÷±€ç£ÄáéĞ¥{²×ZÕ¼êÕ¥…ºä8š á©z@ÔÒA§MY¾"Iú²xWİ]ı‡@Ë"0.öÑ¾€Şák1‚ vRa3í-N†ô}İu†Rß‡²ZĞÛwÎ îoNiËèKÑQ­Nvß^
SPs¡ğJ€øŸİŒM@Ò§54…Ñ… d s5¯Ã;í"–N„ã7Ñ­¤í!Ì=·,“h¿›‚¾ö-Öw'ºæ
æ-æÁÉ_ æğ}1E_.|4‰{Ôû—·ê_-9Ã½ñ@Š/b‚êH‘q¬uøJX5¯gbíø~šd9÷C8õ‚Åƒç±Sùı|§‚sWxŞá5uN6í½j-íºƒj-¯ÃüµHn&Ğ"—t›ğy§Fxk©+­ÜØÓ©PÁÕÓÖw¾Yì¤Ë’pÓ‹¼ƒ6°Y‘‘0Ëc(H.!XôAU;w±ñöç˜uxÊ«#øk¿¥Ô¢°.ßÁyº;[%öL²ZYEMœíì§åLeÚ&°îğ³ÿøù cF¹;c¢DaÍ
µ- ½Œ,›éUT4\|bz¬^<,pÁğÔ‡³:õ%²+XxˆÛ±t5¹´FÜ£ÊWèï÷@Ü²Ue;{‚e«­İàc_€Cô¥»û€3wK{9Ìóˆ.7Z¤s¡Ê—#Óå3×ß¾bß¤¸Ğ=;Ò‰[š(U7ÚlÿœkªÀhÓÌZ¸õ3MëĞB^»ÿ¶TßÖ€áµB·zÁÜ˜j˜è=" hÚ<ñÙAò³e§÷òıüNúÖ	äª+… ¸ò6©úïÓîóã¢[7+ƒè¶—Xoû @¿°Üé~e>EëÂµ\nF’’¶< *›'ºkç0™UôÒ>c)ÕQZG]KªHN'*zõ˜Iÿô¾:¸H¼^Xä—5ûˆƒJé Ğß£Ò8-à)Z¯E(AÒ†yF~¾#ˆãû$`xYîæ(oIDOáE—ÆĞ,ù	‰0`b—Å:ã¦Gu<"İ°»«ãME:êa±1Õí°î8‰¥Ãñ(¤ƒB[ œn>ß)'îA}K¿/9\ŞàlÀ	
¬-± Ò5d všÈr³v…“ËÑã•_foj%ÂJ´0®\ó:¥°Ë	bÙ‰ä¦ãx’³YÌ}œ1¡ŞŸˆ&Å=ıYĞ÷có]ëcÑLt9¤ÙùÂİ:.¢û´áxİĞ¿ÉÉÈ0p©BA†9sá¾‡‘æ¢Şj´®ŸWŠv¹SI©U€úâKGn.Ÿ-ajª¶ÔÍvïµo~jÎÁM#"˜k/xß3bä%–ì$Ú«ÀòÇL³> ì Ÿ•i¼…=8Ì$Ô¿iìó¢Ó•YÏvY
	ŒyÕRi÷É;#ø´2GÔ£•É±ÅÀÄ¹BË%>è[©w8ËÁ†väbÁ&ÜÈ²§m~íÆEÀsl6ŠQŞ#H|Wâ‘éÔíÙ‡ùí"#¤*RgêÜ ¾`Óº~‚ßGw$Œç¬Œ¼¸«½·thßo¶û’Õ¼ã-ß­Í*Á.½r×Ñ©’ù0o)§5ï‘…­
ü…âõ¯evğNtÆ{e³ùõÍMhx
'Ú“üÇğl<<Úşêµ,QMl|â'gŸAoæ=*˜-OÊYÔÒ-âeW &w)§Ê šÎÉtæ•ŸÈ C·¹Á²åCÃ`P‹úÊpí?YïÔá$>CeòÊAúÇzu˜‚‚²‘.øk.Á¿H“€®#U‚Xƒã“™=…5c©P?ƒ/ªÅœn7Î¦–lÓEY¥e ¬Ï§üï7û¿r>§á$“šê-¿pÂŸ0c4f 6Šiº[Ÿ^XÂÅ‰†7 Z³O0íMÜ•çªÚ#HÔ•*’ÀvÁ6ø|Õb¼sÀäîø°•Â!y@G-Q÷½˜+®AÔæµm«,mŸÁ?ù²™5X>$ì±Ù±Ô®³2À}ÂÃúÍ¦"I‘\Kn¤ÒÄ5J0ß	²†¥ú)‹XN\º±Wdá³/¾·ƒ—Î/„$é•LÙ¶Mæ1	îÒì›‘^uîì£æ@P”CÅÉŒ¿¬Iô‹Œ*ápu%vLG[!¨àğ¹C-ÑøÜE÷;Ş+É{g)ş‹d0¢ùWKkÜı²²·I!<Ä*e©­£¤Öe‰Š_YûmÅØÄ@ÉŞ‘ô::¶qD‡´¿ı^}%¼V¦›½îìúAH?½Î Ø¹¿%íM÷^kN_×Š”<q¦pæHcÔÁÿÊ®@~˜„ùÌ jÈ¶Şÿ6/÷Şk
â®ÙÌûÕ¹ùÔ~J¬l1Ñ|şÆŠª)ãÆ¤ä¨B½.^Ò
¡f4CıÏå<È¦˜[$^C_3•+Kº[í”V¾Sc' ôãùÊ;%€»â@¥·ÓF]õà\MßK2ñ`©ø„‰¾`t“™
³íGw©b—óöT°rĞï<óglğÒ¨wÄ]€iEÅ*ùŞc¥‰ç,²ëŞ¤¿cî"”$ö@­j/?
 ñÙì¼`ïæp•°E`ë¿šëAŸµÕ3Xh-)e=GcØ%.a'Úzns$UŒ|W{xÔ9b›E Ğµ×›øK¦²Œvéñ´…„î4e¥‰;òŠ?çfé:*«şo??RJş®Í‘auû}%®Õ™éí¢DÙj˜cpÉÿ}ŞÙ§‘%ø‘ÏÒMÿ‹vf²@;jé‰oˆˆBŠM}¡Î§˜7AuNã•·Šº3°Rp0ŒwË£¬2ÄÉÂ	µ›ıÍàµë¸zOô±ÆYÈâAÜx€o¡ÓkyÚj°ôKz«}$…'÷ã ³jÑ2µü¢m 3éóEyı]RG·RT?6UÄÃAÆ¤yHIPÏæ:U"é*rœ¡=ñq…®t… EIŠå€¦`#|¼÷ÎùüÕ^˜
±3Òd”‰ÙË`ç²·k¯g9=nn'
2ÖX¹˜OA.KXƒš*¬ÚtšÉã¾Jé_^{¬öƒHV·ÌÔµ|©FE’ÊŠ·¸ƒ¨ÚBg´´´®„“lGL‰>:€ØÂ½°PÙ•8îİXb™@›R’ò3ÔÛ6}ÀİüYÏ¡|h*öâÉ ®‰Ê¯¼û„‹ƒa1~ıÆ}cV ópÑlAa›BÚ±7ÿbM­˜Ï¹,ö~áGÁH4j5ÒKU¼ÔÇbÁMêeé;¾Ÿt ”dP?	…TŞ%)Å-{ƒPr‹ÿ­±,ãjòüµ$õs‰5lug“ä –@s®¨á“T(Ú¼ËõÙ•m=V½ŞH1‚#¢êñkR¥K±}¸G=Í•gkò”ôİ ²ş§Õ"Fº_Vb{ù	uú+úLÏ¾´°‰0‘œzÇJ(Ş‘äÓùhıŸ>òë„™­¢íÃ˜J{^€€ÔRÙÿ÷Ş5ˆ;3­ñèJØj¼:Èb©?¿sîBRw}õŞ4/VEr]ôá™ØÓ]Q°î&Lø@z”wÉØ¿×Êï’¨vßt¸Q;O}SàViRo2%$aı3á ºÁP9 ĞŞ_ì¥7»×–»dyµúõ¢·š&Cêhî3†Ø²ÚÁLhÏd+¡½ˆÿèğéwQäñ2tUZ«öË´8½yºZU^°êÇß`œ¿è•²——ñ64>_µmÉ_‘1«©ÿ»õ5í—k÷`"æ#Ô±_«¡"7¬›ïS€2-T¯méËº‹‚È‘Àµ¹Ÿ/[2‘uYdşR{$¯âb	 šgh¹²›W„¼B·ÌåË’ÃMk'sU¼> #5
ªgª9åázBb(3¦Q±–™qˆƒùFÙÆ/éBIä€×ğZ MK±F<[¿PàVWD>I¸#Ó_é[šù—´Ih[‡^hš'KßÒßŸ¹ß¨ş¶4ı³˜#ôü}¢Öágo<çÏ¾âğ]0ò™[	“qhYè»ÖÒ¥=@	)ç\j.gM–néŞü°Ü‚‚ó`´÷YuHˆ_&ÚªEäys\¿'Nãi|&İÊbuñÀÚ«ZÒ«Æ÷_G³f¹9€GsÇÇ_ƒ~ØW{lÛ6™¤d­gf3V¥ù“4d’Ë$	w¥²g?+÷»Ì °eIˆ¿§¨„NoÖ<æ(~~bŠ	R|—û;BêWšv-ØCíO<‰·µ^>´ê0úüÙª UCZ¦ÅÖUìÅF8”©`À
¨IğB6dµÁø–TE)°ğxÂLºÒ ,pŒ*ãpıewöHĞ)l‡ı‚†®öøà¸ÄØ·kíüû_…ü)Å®»[!%qqpÊ‹‹ –•ÜÇÿdî”’æƒ-Hç›O¿˜ãñ;øVçÇ >^¦éçÉpé4ÏY%+9Qÿ“küXÊÒìN—’¤Î8bU±M™âÜR1š5C«¨Œ•DGöÒ43‚CøÊ¿àñ¢uFGš–½ø&a¹£Ä'vÖU¹¦„.} Pn2=‡?EN-°(3¬x¡3>»ö”Upgi7ãf ıè¹½{ıƒÅ'ÜÒÓ\–S…æhnaušˆˆê‹{Ùf$(+Së"×>Ês«&¾ˆÚ:¢¬®æˆ$pãMó+'9BÏ8ÙÄ.üüSdüå%"}ßÙø†võ„Ú»‚Šºm•óı'İÀd}ø2²c ¶¼™ÅœAˆ¨»XôºpÀM4®§ëÛç2&ÑY™y´Ô¥×]%KÜˆiüÄ„‚_é~–‡Šş˜C|Béì…9J\˜ÅLëŸdó¦+wrø‚êÁ>OÕĞ	„6¬ä0r	UVœåAÜC¬hªV­´æG¤·©‘5˜G”GI69|\.± Ï÷]¿$¸a[6Z(vp·]âv¹g
{Å,}Ù®Pi|İûYıE07SÔı	„#Ü›T.ãz“<Gş`‘ˆoÄ¢œŠLêiX‘øıVên*Ô˜æãdú2â¥÷%ŠäåÒ‰Kæ˜9í½$Ü)Oq,cğŒwÆzW<U‚GSÌşgª ñì-Æ²§­›V×®U¹mæj†—t:—ÀOàT¾:âi0¨µv=>8úlRt¯ñš|ûyao‡Ñ]KÑé?á³/kJp¦-bî"—!¬O½¿ Iªï“¼'Ñ°•mûÛ–»’‰-K¦¥–Ş'H–@|*—µÂY"uæ„.‹¡z[RÔ‡Áõ¸‚”Wtq!6ºÃUv—Øà	n†È”Frå@ìÈÆ¢B¦‹ˆ™Ş±·úV“§·=Ñ&mµ†¼h”@üñ•_ÏËĞí‰r'õT_ù^`”úKÙ5.q›Ç‰Vûõ#0Í¹ùg©Æ6 Ó9·6ÎËdm&ÉB§ïÅœ~L Ù…¯šË:ú„¬+[é‡À	è‰rëÏ½˜şh	ÔªEkL¸ÿµ”›»»Y¿\H¡†€ãa<XO‘·›³˜œŠ£=Ài
ìku{»ƒÇÛ}WÕ›áÚtûh_|S8ıX"·Ìøâ4WÍÂKö~[V(ô~M^xâµÑCYÎpõ)8eñ#j…ç»¼#3E°Â•baI‚ËÌØğøV=yrpNï¸‘¾8×â0€ e;?íÊKÇ=×ÑtÌ‚ ¹bì_C+´ñÜş´\ëø—µaf sGº¼»&“Ó»ğ'Fš={}¿.h†¤ÿñˆınÁìY Õ§¢ÊŒ‡ ªhÔ¾‘ tˆĞŠ¸svšgÃFá±@HCzEÈáC{yjB9éÂ6Ç®Emj¶­–mÉ[­Bzf©y¾Jíê°ĞW-şa>³ğ 5'm`8ù5n´pocS‚o÷ı30	Òı>½#í'Şà_(%Ïò0ÇüG:ğZê|­ÅÚÆúšåw¯Çmï³Ög†..¹¤>üÜEŞ³=¸ä=å!Í™œ#¥N­ÎöaI°û$áÀ£lŠÿyo±£h…²Ï	S§Üë¹B¼ê¬“‚˜½á!•æè©>³¶Ú˜Ø§™IË%1!0’ßøa|1´Ì„Ùyès?e¢™!NŠ!ug­*¯'DŠË– %Sçÿ"ÏüÊÅéôÿ@
&Ç{òh1Y ñcËER4CõyHwİòÊÔëî
ƒZ,>¸&í Ğµv§€¨R¨Ã’<…õcRl…±t®\ŸïM$õ®a—Ô§IÓ0»üÆ‰İğrC„œ;ªQĞ:Ğhœ“—ˆÔ³æİmÌèïcFDÖ4Ì”]Ë æÏ¡gzj±òW /¨rÎr^eşşSJps‚ÇL_~‹Ø,D69''µÜË7¼™¸U½é+’™8&~	»~ß!¢ÉÄrgLÊ>p\°5£&5Íüõò¤Y{ènQÚü´ÃˆÃ7A`NÏÑ•	û¼·+(#^ãÅ€(#³±×w}¦4õŸÛV‘â‘@)iù½'‚Æ‘_‚øIXÈ¯dQì·êÔ%Ò[&}ç8ÍÑ²AiÔö"Ø-yg°V†4*Is>Š	²ã³IÒ\\ˆòÉè»±¨Ú^yLöOÊNdb$FÎìåAcm¾6§uÚwø€¥Yáï.ÓXg²Å¤4k†f~3Z;;ÿXŸô¨mæ£RPğİ=ˆEì KŠ÷^([K®ö±À­`’XWà1¤!}h7»pšY3 Ô¡å¡Ä¬7AODZ·ç®ñ':vqæÛ¡¤ÓbĞÚ˜”Şökã¿`æãœlq£ººdŒATŠLV4¨ÿû.%ÍØkoXE3<’šOÍÓ‚Cö»¹u¸ M|˜À›èF"l£Nï³T×6»óªX	våÈ‡ZA^Ë?Š–º)•%@µX‡·®õÕî‰ÙÎ1}ï’NLdbÂ½Ñİï7*m"«U0n[í¨ß¿‘ ­y“i2ó ÇÁKY IvÜGqQ]änò1ä5"â3³Äô>¼VHî Ğ]ê^LÚÎÑ;ÖKVÏy.4ÿÂê¢S$'h´Í‡d³E…¨Û V"­H™‹>YsÒ	m"ªÈÚ#ò¬^…åÀ4F}èDñ3×opg‹s8t³lÅ°> ®´óUè]öÁÛu:ÃŠB-ÀîwÌ	éÍ—äaˆõ'IÆ<î¢Ğ.²º¥J“ØXF“@gŞh¼ŠpŞIjÜ<µú&œöãR¥å<ßzûb©”sí²8Ê±<Ï¡óŸåïã(DZ˜ºÌI‰8¹¨Ûo®õÑÕÜ¡Ï„¨ª‡ò§Åß‹àd¾¹x&™Ö]¬ºƒ#W„n´XœÂ°p>¶*ÿÆL¨›ğ[–STI}$Å¿$£ÕêÓıÿ\qgH5N+ 8‰Ce½](*ÆOQi©Ï±À¶{Æ{1¿cÀ…öÇ~_I Û ô@:sj\ûÜh`ÿøÌ	îíÔg9r¹sœ»:Â–„°Ò¶”	ÿ’œ™ö4á©*Ãüış"M]Œfûîì€†ŠsÂÔ|0b`ÿÃ+Esí«„¡÷Â©ÂZaŒªÄİAo’ÙqÇ/¿RÜÿ†WöÒËĞ~§-¥~ğ·XùİŠÁ˜
SS´i=åÎ¬÷´@³&w! üle‚@X.Ÿwsã$&²åàí=ƒÉ}P`Ö ×S‰#ê~ªCùºşØÉ›ÔØ¨µŒö³¤ƒWúW1R“oz! B[lz˜ŞW:6à™0êZ_2Ÿ:ûê•}ÉVÏıyôÉm²-T7Ü«}¿”ÓıÉç,€ğU©MºÏÊÎJ„ñ±à‰6‚rc_1ÌİA ÌæÂ¡™#+‰¬Õ†ñ¦Û›'äO4³"ë C$ÙÛ\U:“=¨”VöÁ-áêÁ`†åê‘Î7á+ßİ]À%7sÊSaPª•JFM’†”%»Xh¥æ¿œLšğLQí(uwòy<Ëâ—0
Ü–$Zkü”ñ ”tŸaá#°qO¿ªQ £Ğ&ÆwùØì@[¢¤2æ^a²\j›qœgÎµì[m€%øjAätÔ@Oµü¸›äo·¸bO+aëğ]ˆõÆ¨¡#zØùĞåwùÅµôÚÕ&%eD	>,š[d^7µ¬0S¼ÜPüËŠãj5ÜoJqŒ‰wOôv	 	ú€æ’é³Låø–j"ßŞ R_– )i&ƒ©}·®æ¹ß~‘ã_!Ò´^*Bƒ„Y¥ŠA=hbr	D7dZAœ¥a\'²2ïó8W€Ì‹.§`l˜SÙ}À_¥œÙ/RµÔ7²Vö-“Ô"‰1’µ$„#U3cÛÃ¯ãm±¢ã+ş\ZsBñK+µ]SWY¡…3ójÑàG¯t¢ uÌã°ª!Öï‰½l¢À4ˆ–}Ü_¥±¨SÔZÍ0íäV$f]“_H4Ô>÷ Jsàh|æíÃ;aºã m¡­ŸÒ#¦ı¡!5líÙÈ;3Éë±à”IHÇ/îfr„è‰E|ş$h––÷Ğ êS c#¥e÷"¯AŞ”,İ_Š&;1j®çªœ4}sQ°•ã·‰ÚYgßšRÔHƒ—³°y’x†|?æÅ1Y ’±3İKÃrh\À[]œUÈ¾S7È¨eßp'?|rZ÷ªóDTşé)l†ˆü+«j’q%Êb7›·¡BÆgÿ²”g¤	À¾¨ì¥q¡ïqğ³Ùa*¼4DƒzünPw¼£O~}øDs¨…KCéj!ŒX¿HF†?"çï*·ÖDVøø&ŞœØ7×Xlˆ‚äıšË4fuFõOÚæ»·lpYØl“+|eâ	 p!4F84ær"äÅMÈAÖìÁç7ú¦c½E¿rºx½¿†EC õ	("‡TÃñGINß¾hYU½³Ô5>gú^o=TÆFUqî[>[ÉfĞcl¡Ÿu»ÎgÈvš­îrú<>ïÌë^y[>që¡Ù‡İ;	x¸ßFÇá“Ä»_êOÊñ]å-ƒØÒ‹£ ™Òlì¨wâİ—ÌÛŒáyølà‹©wà‰âXdV·zˆeì[,äl¤‘œ‰æáo4W AÕÓœÕ  á‡„!F"4+‰ñÀBÎŒ÷áİ9Á¹ØŒ„ÖC’Æ±%Ê6ŞìöœÁ%~ÁBò "¤Ít¯ I-lïk±]‚lß‚åÜ>4a?dÌ+¦Ógr‘„,NÍªVŒÍÀ|ã‘‡ÿõ¥™9qÕvôlè›dH@Û­E{í«µGÜ¦§X‚Hñq™ËØô¹¯‚ªQr®t(L4++£U×{m:bÈ°I}³ŠÏ¾{g4ó>†(Uï¤CU@²1e"T¹?Ìî¥KMs×ÜœÇ±HzîÖ›HAËì†®¼F ÅiÕµÎ‹¯7×”—è¶Ïªê(kgÈ]€æ!¯mÄ/Ô¶\üœ—ÚpçqşèAZ`e…çïˆ~êñ,†nİ¬eü9ÓòğG"ÓrUÄœ”ô8f Áî&!èfİÕœLÆ 2»¤iİØ!IÅvé¹Û`‹kœWŠK3N;Ö€¬P½¶M	Æt“ß1YØû­Ê±àŸk÷¯W,úA_U½2Ø¸ÃÚÑL(Æ;Ÿ:üx¸8Hw>ÛV'A)"ï.—Ï¥n%ƒÓJ€Ä8Û6va½©ĞnşOoÜõœèH_n‰btDhHBs°†/+‘òÓtW­û†^À:ûØP½öŒów`mƒÊ¯º`ÀÀˆ@7ï­ÕÛ­, ;``ìåâÃeg[oÒ¥«µ±³%ÿıó°q"ÊF{’ñw™¸sŠºpR=G}@UÆN¯Ú”àê)Š½<{ñÖ‹t*íîz³ZX$„~şQù‹P&.%äVgßO8)÷àÊJğ¢õî4–wF?ï*Ü«FhKl/Â–…oZÔTŠŸ£3gşƒ¼ŞN2»Àa¼½„²œ°H4VÕüåÆÆÊ]¦”‡ˆwa{^ÈŸ†æ;‹6 cìàoõÛM0ß¨ø‘¤ª?367ÒŸĞ(˜u¢Äv“ãœçÔÜå¼ÿËØuXKæQ°}ÙpBÿşÉd¾#—óŞñ%·r.úÇvÍ÷£êòWÚë;˜ö¾™C]XÈÌİvÜ@MŒ´£§†¤}fZë0½Õÿ]27åöÉÙ‚QfïŞ7_K{zĞs-W½ßémÛÀÊ»@ {ûÿŸ®9eÔÊf;½œh£Šüu4Ì6OŸØ<†K©4SşºSı¸yÊ¬ZÏKÇ±u0|•ƒA7¸ê/‹t‚µT	Êt—D4ŒãÔ¿¿°Rí²ğ¦Ì9$]ûÃ´Æ«!I‹ãö;;<‹z¡]˜DÖÆí¶YÓ¬Âjf®G´E»<L»r6uıÌÅ¦ï÷ğ<››“Üa­N5®¾·wİWCÇàøÀ‰¤éİ‡Üèäi.0Ûè¡9L+Í>¸ekiÀã)BÚµä'ä\lEˆ2_˜6oM¹*EˆlÄiZÔÆºû\¦ó"qğK;dÛ˜>sŠ¶œ 8.0?àIçÊíµìƒÍ˜ úãÒ®»;b„¬$âÃ>”§±€¡2cYãÚ<$¥èıüWÃ³ú¾õç§­£¨?—B«ïÔ œ^FZşA·ıéê‹©/üÇräL3‚UánK¢"ŠÚ‚Qîhó£r‘b
g~ÒX¢œG1”ÚvrFağÓmMáç…áõs––µrŞrIô ÊïÜ€¦ôÑD•2 ¦4Q°lÎAß²
3
CB¯ªÌÏ‘NĞ›“}J_)ô™ë„K{+TØejÚ8C
1lç|y‰-¬Šg›İŞ	˜ĞÆÇ³ĞjÂ]ë8|RÃ ¹\åü~Ûa¦wÂ|¿+­^bòòâ-´‡ªd%ÔàğZEbİf8;P^ÿp9FÖáß‚Ç¤2Ñø'£ønã4¸¨($„ep¾ysÁ uqî‡¾7)º5¾IÂŞ9îOiRÊrş[‰%DJ7Üä?„#s´»ıÑ#¸'FFŞÌ<[ƒâ¬¬®£û|‘t’<±1ìªøùê8r§4=Ípr‹´“Oê‡R%Ë¼GV6-µ:t<yl¹ô(“Íòü[b¡IeSÂtÅKÜÆ‹g‚ü»¦—i›9ÌH¿*ÄX-/8ºÆç×uRG? ×«yjl»“ÿIé<ÃâcñfËøü-6‡Ùí13Ë-–¬ùÈÀU‹÷€W4®VéºË×Ò¬ğY‘·í7€Ñ™g-\	½|ê€nèûÆ¾­>‹AÆ
ıâÉ8¡|ó‡£Él³ävcÅ&×µÙî=ÕR¨¿ò»Ç±G]°Ÿ$wôì<]Â€Ğ¸5–Ïğ>#”+˜µEnw‹¦3üÖ™;Í¤1Ÿ®c q1'q‹³*ç!Ã!ÉF`œÒEbävÅp¶lWéäØmô©ÂÚ–G9ßo(‹ 1B‹\Ğr‡j®	íÛç¹,Ló<3¿Ã70şŸ¡vÊê½oCq·Ûx"~s&dá ‘Uí®]±,kùWªÀ"äLQ°ï`•[qÿ½5¡L‰ÒS?}ßó@ô¬Cc?yß¢
n€!d‚01 kB‰0D#ë¥¦&˜¥bxŒ×±® —âåï°$å™aÁ¯sj¨ƒÙ[hİİ¼•jÅUÁ}§µ¸w€†¾±.ìèŸ<©½bANOCö(Gi|^ËÈ|ÑZÊª„[5©‡ª±Q0ĞSexíZoì¹&4ûóÙ…ã!®$ÚnîÇÃÙóx>wn´I’~ÛœÃ×³µéšîª—ÊVÑ2>ÍÒÇÓ‚<£é®û¶#Ó¯rRUé`iC¡JTÉQL?V{ÂHÍ‹)ùY.#§Ö2åbuhş_ƒåç{s.E)®oN·î¸óë^Ÿ¿˜«Œ\|£õN„“®³ÛQ Ó¥š‡Ó“‹Iaª)cò»ær¬ñ9Ÿæv»å–^ÄGZTfÛr_-o/¸ÄKæWtO4õMeJ6,Õ…ˆ&ıÔîÏ¬˜ƒ~‹5û&Í«Q!šs±†í÷ÅÅ}kèf ¦1å‚¿(µ"&J—æÓyÄ0²³Óƒ†Ep‡lMg^Ùì‹±ï3ãé€/`âI·{ÓÑBù~Øö
Gïç4¬ò y…œ:×X}"¡¿ş„5DtU—“©]TëØö´÷f*pĞ59–Ş˜h”‚jï°jLÃ€a{ìĞ¢_fö?²‚ÄáëM›9\JRö«ÓãÙ*\mÖ8w‚_¼2ã\ÅïøÇŸ¦«nr…ÌÀX á6aÓbÃ
ÄÁxHps×ÂÙhÄ">÷e<¡È/º¹ıaá9
Q#»Øa˜à-ç
#§öRG!7{÷½&¦£D˜:5uòyªåÁä´ã;ıÒ¼qkª…ì ¨šb‘Q_M
~JÓ Ü] –tF•€¼vôzÈ¡İÏ¹iTV7^›Ã["álÙDí?†Æ`ñÑ\|—}Ê‚R'ß7í­Ü‚€jŞºŠ^J—é±)şï³ÁôDF87¸zÿŞ!h&$ŠYväêˆX»êj7ñ ¬0•>™Îé÷K2ëÏ¬ƒ²+3‹2…G€Pà÷¢÷48ğÊRŠÂ&
³PÖ°ÎÇküWŞZ¸õsÌgŞ›' ²µì–N¶¢
³pBœ»uàÂ;ÎuIWÁŸ°^Q{º-” ¿µ£oÚ+ú%”¨m{¹O;À-9¨ÄXÚ»NÜBp”9÷0û`Ván‡şAZ+K´ ëÎqñ˜ï¯òEX|Rÿ@w­2œ'¾İ}`;ğ‚ŠTÚf¶	Š¯KS8¾ø=£™½cvÇÃø6»Ò—óIáBêhq­LÑ²œ·ÌØï‰å³TÄíé,ñâ‡.¨¾oïƒ$A›«‚ÿİ­™+	c?c¤gÅcqÕÎUÀæ*(¨rW'’Şêâ‚V="»3=.Ã=ô™C»wO;ØS”etœ¡³]'Š¹ó“]Bï@Ø#dö|+‰#£`?şøjÜ}¦Bê%ŠÍ)?RÑ
@Êü#‹ æØ_8àX‡‡£!·ë$Î4µ…÷¸2?†C-å4pJÇÑ:ZO÷¸¸N¥»N+Sìá¼z¼·bºî(vpáoL|aÒ°ÂPä;ƒ“
H,O©$äRÍ‘²¥µªR ¥¹­ª>g›ıê"§XB1‚K¼Ó³W³·»ÅİÉP„_ÿ;ÅîÜãK˜M6ÛeVĞ|™Ò&ÒÕÚ!(¼ÿX˜S<‹MWË<Ôxé‚á<ÿWç|3ô§ã_Ï×œO¼Şgw?]VOgÃp•·[•$·^TÇÖ¥4àfÓ5ëDUÑ<æ²›ŠZš
aıQû•©ˆæ‚¨9ìß;[Jcèá!U=>ß\>@æ‚úDŒİ®XCjç1û~)öäÛuÂSĞ¥ÛÀ7“; SL0²®èI%0¢3HÆÓñšYšà—îÑÊ
HÈ¥¥g¾óø°Z¤ù8z¶rğÃ›h¨û\}¦,­ÇV_e‘ú-’cç™R¡ÌshË¬`BªEJæ1¤ÅV/ÛÌY²[€=ĞXÔkµ~ãUà’?ÓIWÔ]I(bø€ÿ˜a÷¹óà¾'V*›èlÅ +tOŞ²Ú>È†·s¤ÄáAhšô×†fú”ù"èÎó5¸·Wº©QÕsŠ½ù$+Ø Ä7|¿®P½ıÊÖ¹± Qª0UwÜì”¿Ê¡´FèÅå8Ö¾~–ÅB¼²ğEµ7í¶©_·¯61à#¬üáÖÔjáÏq	K/&¤(*™@mWİ_pL¬ÆYqğ©ÚÅëÜûãå>ŒWÎ,›oUd“7ø­b¦¡Fh©fn[t:‰ÀTÌâŒË)V¢ç'k¸Á·„.~‚6Z‡³|mH½ïÁµ¶Êa:¬Qñ†û&Åè"oÁ°ú÷Ì¸KíáìùM#çôl•4ÌÜÚŒ×âã„¥1È>¯lUñJœhMbJ%ª+Z;¸™Ÿú(ŞşÑ¤ä.ˆº“Ãßµ‡×°¬)¹5yó4×ñĞ¿„Å’l«¦ıü¨UÅÄÈ³!Ñ±¿±•gc–A×Âh™ƒfŞÓN
`Çã:v³Ï]_¹ân2J5«"¼©>gúƒT KI¨át™
¾Æl%Í–VÏx©ğ	e for the next lock_list
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