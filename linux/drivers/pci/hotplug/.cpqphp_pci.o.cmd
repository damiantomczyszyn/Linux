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
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/tw9906.o: $(deps_drivers/media/i2c/tw9906.o)

$(deps_drivers/media/i2c/tw9906.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ELF                       "      4     (               �����S���   �@\���   ���   1��Z[�
Í�&    �t& ���������   �9�@\uM�ywG�A    �A    ���    �  t�A�  1��A�  Ít& �A   1��A@  Í�&    �����Í�&    �v ��������u�A��u�A   1�Ít& �����Í�&    �v ��������  1��B    Í�&    �t& �����S�X\���   ��t��������   ������������1�[�f������UWVS���   �Ë��   ��t&��������ƅ�t[��^_]Ð��������ƅ���   ���   ��t1�������   ��  ��  ��������   �    ����������   = ��������1���t��   �������  �   ��  ��������   ��t!1��������  �   ��  ��������   �����1�[��^_]Í�&    ���������,����t& �����VS���   �X\��t��[^������v ���   �����������������   ��t�   ������   ��  ��  �����1�[^Í�&    ��&    ������UWV��S�~���nX�@���g   �P�R�����%   =   �P   ��  ��   ��������Å���   ���   �@   ��������   ����������   �����   = �����   �   �<   ����������   = ����  �n\���   � �������   ���   �@\������xF1҉�����������������   ����   �~   �t& ǃ�       �v�����&    f����   ��t�$������$���   �$������$����[^_]ú�����&    f������UWVS�����   �i�r\����   ���   �ȅ�t4�W�P���   �R�@   �@   �P�@	   1���[^_]Ð���   �ư��   ��tm��   �@  �_H�D$�O�4$���3f��Q��  ��  )Ӊ���H��Y)މ���H��9�s�Չσ�9�uˋ4$�D$���   ���R�����    ������m�����    ��  농�&    f������WVS���   �C\����  ��   ���   ����U   ���   ��us���������   %�   �U   �   �   �����������ur��   ���������������uW���   ���������uE��	   �ȉ�[^_������t& �������x$%�   �U   �   ��   �������������t�[^_Í�&    ������&    f������WVS���   �ӋF\����   ���   �   ����   �   ����������~   %�   �   	������������xe���   �������ۃ�@�������xE%�   �   	������������x,�   ���������x��%�   �   	�[����^_������[^_Ít& ����  �   �S����t& ����   ����  1��9�����&    �t& �����UWVS�˃����   �D$�C�T$�Q\����  �C���  ��	��  �C�K�C   �C   �$���   �İ��   ���\  �@  �   �@  �zH�\$1�ˉ|$��������&    �B�r�$)�����H��Ή�)Ή���H��9�s�Չǃ�9T$uˋ\$����   �E�;�C�u�s��   �|$1����C�B�C�B�C�B�C�B�C�B�C �B�C$�B�C(�B �C,�B$�C0�B(�C4�B,����[^_]Í�&    �v �C	   ������t& ��  ��  ��  �����t& �S��t	��	��  �|$���   �|$�\���   �|$�ưu*����  �D$@  �   �@  � �t& ������j����D$�  ��  ��  ���\$)ω���Hω�)׉���H׋|$�,�O��H�<$�-�Q�É�)Ӊ���H��Y)߉���H��9�s�L$�Ճ�9$u΋|$�D$�\$���   ����  �|$�   ���������x���   �����������   ��  ��  ������D$�l$�   ���   �8��������ƅ��S  1����   ����   ����	���������ƅ��(  �D$�   ���   �x�G����    G���������ƅ���   ����   ���	���������ƅ���   �D$�   ���   �O
�G	�������	�����������ƅ���   �O�   ��������ƅ���   �O
�   ��������ƅ�xq�L   �k   ��}\������ƅ�xW�`   �l   ��������ƅ�x@���   ��   �D$���   �P�@���E����S�C�:����������&    f��|$�   ���������x���   ������������  �   ��  ������D$ǀ�       �����Ǉ�       �����������|$�N   ��������ƅ�x���   �N   ����������ƅ��3����a���                     �  �                   �                                                                                                       0   �   �   �         �  �  �  �  �      rstb Unable to get GPIO "rstb" xti Unable to get xti clock
 pdn Unable to get GPIO "pdn" bus width error
 Product ID error %x:%x
 tw9910 Product ID %0x:%0x
 un-supported revision
 norm select error
 Field type %d invalid
 drivers/media/i2c/tw9910.c tw9910 PAL SQ PAL CCIR601 PAL SQ (CIF) PAL CCIR601 (CIF) PAL SQ (QCIF) PAL CCIR601 (QCIF) NTSC SQ NTSC CCIR601 NTSC SQ (CIF) NTSC CCIR601 (CIF) NTSC SQ (QCIF) NTSC CCIR601 (QCIF) h   W��������   �����������������   ZY��t�   ������   ��  ��  ��������   �D  h    W�����XZ�������  h    W����������Y[��  ���   VRhj   W��������   �p\���   �����������������   ����to�   �������  �   ��  �����������  h#   W��������   _]��  hY   W�����XZ������  h@   W��������   Y^��  ������  Vjh�   W��������   ǅ�    �  ǅ�       ǅ�   �  �p\���   �����������������   ����t�   ������   ��  ��  ����������������  �  ��h�   V����������[^�  ��h�   V�����X���Z�  ��P�����h�   Q��������    TW9910: missing platform data!
 I2C-Adapter doesn't support I2C_FUNC_SMBUS_BYTE_DATA
 O  �   	������    �    ������    �����license=GPL v2 author=Kuninori Morimoto description=V4L2 driver for TW9910 video decoder                    tw9910                                                                     �               `       �           �  �  0                                                   �              �           �                                                                                                                         @    �@    �      h   2  � �   @  � �                           S  ��  [  ��  h  @�   v  h�   �  � x   �  � x    GCC: (GNU) 11.2.0           GNU  �       �                                  ��                            
       $        0   f     ,   �   &     B   �        S   �   .                                	 a      �     q       T    	 �      a                   �   �  W    �   @         �   T   p   	 �   �  H     �   �  �     �   @  H     �   �  �       �  �       �  2    	 ,  �  �    ;  �      	               O           f      �     x      
     �           �          �  (   1     �      0                   �     0     �  �   P       `   @     &             1             ;             C             `             l             w             �             �             �             �             �             �             �             �                                       -             6             Q             k             y             �           �             �      
     �      0      tw9910.c tw9910_g_std tw9910_get_selection tw9910_enum_mbus_code tw9910_g_tvnorms tw9910_remove tw9910_power_on tw9910_power_on.cold tw9910_s_power tw9910_probe tw9910_subdev_ops tw9910_probe.cold tw9910_ntsc_scales tw9910_get_fmt tw9910_pal_scales tw9910_s_std tw9910_s_stream tw9910_s_stream.cold tw9910_set_fmt tw9910_set_fmt.cold tw9910_i2c_driver_init tw9910_i2c_driver tw9910_i2c_driver_exit __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 tw9910_id tw9910_subdev_core_ops tw9910_subdev_video_ops tw9910_subdev_pad_ops __fentry__ gpiod_put clk_put v4l2_async_unregister_subdev clk_prepare clk_enable gpiod_set_value usleep_range_state gpiod_get_optional clk_unprepare _dev_info clk_disable __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init clk_get i2c_smbus_read_byte_data _dev_err v4l2_async_register_subdev i2c_smbus_write_byte_data __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw9910_id_device_table       &  1   &  �   &  �   &  �   &    '    (    )  !  &  >  *  S  +  n  ,  �  -  �  	  �  .  �  ,  �  -  �  ,  �  -  �  '    /  !  &  I  1  P  /  d  ,  x  -  �  &  �  2  �  3  �  "  �  4  �  	  �  5  #  	  *  .  o  6  �  '  �  (  �  &  i  "  �  "  �  &  &  6  K  9  f  9  x  9  �  6  �  9  �  &    6  3  9  S  6  l  9  |  6  �  &  W  "  K  "  �  "  �  "  H	  6  _	  9  s	  -  �	  6  �	  9  �	  6  
  9  A
  9  [
  9  u
  9  �
  9  �
  9  �
  6    9  '  -  b  6  |  9  �  
  �  
  �  
    
  ;  
  R  
  �  
  �  
  �  9  �  9  �  
  �  
    
               	  h   "                                                 $     (     ,     0        	     0     1     /  1   ,  E   -  U     [   7  l     r   7  �   	  �   7  �   1  �   /  �   ,  �   -  �   	  �   7    	    7    	    0  >  	  D  0  h  "  x  1    /  �  ,  �  -  �  8  �  	  �  7  �  	  �  7     	    7  P     g     ~     �     �         ,    6    �    �    �    �                 	     &          :     ;          =  @   "  L   "  \   "  d     p     t     x     �     �     �     �          @  	  L  	  X  	  d  	  p  	  |  	  �  	  �  	  �  	  �  	  �  	  �  	   .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel__bug_table .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   �                    	   @       @  �              )             �  �                   %   	   @                         /             `                     8             `  4                  4   	   @       (  h               E      2       �  �                X             @                   T   	   @       �  �     	         g      2       T  V                 z             �                    v   	   @                         �             �                    �   	   @       (                   �             �  
                  �   	   @       H                  �             �  Y                  �             @  �                  �   	   @       X   �               �      0       (                   �              ;                     �             <  (                                d        &         	              d  �                               !  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                       "      4     (               �����S���   �@\���   ���   1��Z[�
Í�&    �t& ���������   �9�@\uM�ywG�A    �A    ���    �  t�A�  1��A�  Ít& �A   1��A@  Í�&    �����Í�&    �v ��������u�A��u�A   1�Ít& �����Í�&    �v ��������  1��B    Í�&    �t& �����S�X\���   ��t��������   ������������1�[�f������UWVS���   �Ë��   ��t&��������ƅ�t[��^_]Ð��������ƅ���   ���   ��t1�������   ��  ��  ��������   �    ����������   = ��������1���t��   �������  �   ��  ��������   ��t!1��������  �   ��  ��������   �����1�[��^_]Í�&    ���������,����t& �����VS���   �X\��t��[^������v ���   �����������������   ��t�   ������   ��  ��  �����1�[^Í�&    ��&    ������UWV��S�~���nX�@���g   �P�R�����%   =   �P   ��  ��   ��������Å���   ���   �@   ��������   ����������   �����   = �����   �   �<   ����������   = ����  �n\���   � �������   ���   �@\������xF1҉�����������������   ����   �~   �t& ǃ�       �v�����&    f����   ��t�$������$���   �$������$����[^_]ú�����&    f������UWVS�����   �i�r\����   ���   �ȅ�t4�W�P���   �R�@   �@   �P�@	   1���[^_]Ð���   �ư��   ��tm��   �@  �_H�D$�O�4$���3f��Q��  ��  )Ӊ���H��Y)މ���H��9�s�Չσ�9�uˋ4$�D$���   ���R�����    ������m�����    ��  농�&    f������WVS���   �C\����  ��   ���   ����U   ���   ��us���������   %�   �U   �   �   �����������ur��   ���������������uW���   ���������uE��	   �ȉ�[^_������t& �������x$%�   �U   �   ��   �������������t�[^_Í�&    ������&    f������WVS���   �ӋF\����   ���   �   ����   �   ����������~   %�   �   	������������xe���   �������ۃ�@�������xE%�   �   	������������x,�   ���������x��%�   �   	�[����^_������[^_Ít& ����  �   �S����t& ����   ����  1��9�����&    �t& �����UWVS�˃����   �D$�C�T$�Q\����  �C���  ��	��  �C�K�C   �C   �$���   �İ��   ���\  �@  �   �@  �zH�\$1�ˉ|$��������&    �B�r�$)�����H��Ή�)Ή���H��9�s�Չǃ�9T$uˋ\$����   �E�;�C�u�s��   �|$1����C�B�C�B�C�B�C�B�C�B�C �B�C$�B�C(�B �C,�B$�C0�B(�C4�B,����[^_]Í�&    �v �C	   ������t& ��  ��  ��  �����t& �S��t	��	��  �|$���   �|$�\���   �|$�ưu*����  �D$@  �   �@  � �t& ������j����D$�  ��  ��  ���\$)ω���Hω�)׉���H׋|$�,�O��H�<$�-�Q�É�)Ӊ���H��Y)߉���H��9�s�L$�Ճ�9$u΋|$�D$�\$���   ����  �|$�   ���������x���   �����������   ��  ��  ������D$�l$�   ���   �8��������ƅ��S  1����   ����   ����	���������ƅ��(  �D$�   ���   �x�G����    G���������ƅ���   ����   ���	���������ƅ���   �D$�   ���   �O
�G	�������	�����������ƅ���   �O�   ��������ƅ���   �O
�   ��������ƅ�xq�L   �k   ��}\������ƅ�xW�`   �l   ��������ƅ�x@���   ��   �D$���   �P�@���E����S�C�:����������&    f��|$�   ���������x���   ������������  �   ��  ������D$ǀ�       �����Ǉ�       �����������|$�N   ��������ƅ�x���   �N   ����������ƅ��3����a���                     �  �                   �                                                                                                       0   �   �   �         �  �  �  �  �      rstb Unable to get GPIO "rstb" xti Unable to get xti clock
 pdn Unable to get GPIO "pdn" bus width error
 Product ID error %x:%x
 tw9910 Product ID %0x:%0x
 un-supported revision
 norm select error
 Field type %d invalid
 drivers/media/i2c/tw9910.c tw9910 PAL SQ PAL CCIR601 PAL SQ (CIF) PAL CCIR601 (CIF) PAL SQ (QCIF) PAL CCIR601 (QCIF) NTSC SQ NTSC CCIR601 NTSC SQ (CIF) NTSC CCIR601 (CIF) NTSC SQ (QCIF) NTSC CCIR601 (QCIF) h   W��������   �����������������   ZY��t�   ������   ��  ��  ��������   �D  h    W�����XZ�������  h    W����������Y[��  ���   VRhj   W��������   �p\���   �����������������   ����to�   �������  �   ��  �����������  h#   W��������   _]��  hY   W�����XZ������  h@   W��������   Y^��  ������  Vjh�   W��������   ǅ�    �  ǅ�       ǅ�   �  �p\���   �����������������   ����t�   ������   ��  ��  ����������������  �  ��h�   V����������[^�  ��h�   V�����X���Z�  ��P�����h�   Q��������    TW9910: missing platform data!
 I2C-Adapter doesn't support I2C_FUNC_SMBUS_BYTE_DATA
 O  �   	������    �    ������    �����license=GPL v2 author=Kuninori Morimoto description=V4L2 driver for TW9910 video decoder                    tw9910                                                                     �               `       �           �  �  0                                                   �              �           �                                                                                                                         @    �@    �      h   2  � �   @  � �                           S  ��  [  ��  h  @�   v  h�   �  � x   �  � x    GCC: (GNU) 11.2.0           GNU  �       �                                  ��                            
       $        0   f     ,   �   &     B   �        S   �   .                                	 a      �     q       T    	 �      a                   �   �  W    �   @         �   T   p   	 �   �  H     �   �  �     �   @  H     �   �  �       �  �       �  2    	 ,  �  �    ;  �      	               O           f      �     x      
     �           �          �  (   1     �      0                   �     0     �  �   P       `   @     &             1             ;             C             `             l             w             �             �             �             �             �             �             �             �                                       -             6             Q             k             y             �           �             �      
     �      0      tw9910.c tw9910_g_std tw9910_get_selection tw9910_enum_mbus_code tw9910_g_tvnorms tw9910_remove tw9910_power_on tw9910_power_on.cold tw9910_s_power tw9910_probe tw9910_subdev_ops tw9910_probe.cold tw9910_ntsc_scales tw9910_get_fmt tw9910_pal_scales tw9910_s_std tw9910_s_stream tw9910_s_stream.cold tw9910_set_fmt tw9910_set_fmt.cold tw9910_i2c_driver_init tw9910_i2c_driver tw9910_i2c_driver_exit __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 tw9910_id tw9910_subdev_core_ops tw9910_subdev_video_ops tw9910_subdev_pad_ops __fentry__ gpiod_put clk_put v4l2_async_unregister_subdev clk_prepare clk_enable gpiod_set_value usleep_range_state gpiod_get_optional clk_unprepare _dev_info clk_disable __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init clk_get i2c_smbus_read_byte_data _dev_err v4l2_async_register_subdev i2c_smbus_write_byte_data __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw9910_id_device_table       &  1   &  �   &  �   &  �   &    '    (    )  !  &  >  *  S  +  n  ,  �  -  �  	  �  .  �  ,  �  -  �  ,  �  -  �  '    /  !  &  I  1  P  /  d  ,  x  -  �  &  �  2  �  3  �  "  �  4  �  	  �  5  #  	  *  .  o  6  �  '  �  (  �  &  i  "  �  "  �  &  &  6  K  9  f  9  x  9  �  6  �  9  �  &    6  3  9  S  6  l  9  |  6  �  &  W  "  K  "  �  "  �  "  H	  6  _	  9  s	  -  �	  6  �	  9  �	  6  
  9  A
  9  [
  9  u
  9  �
  9  �
  9  �
  6    9  '  -  b  6  |  9  �  
  �  
  �  
    
  ;  
  R  
  �  
  �  
  �  9  �  9  �  
  �  
    
               	  h   "                                                 $     (     ,     0        	     0     1     /  1   ,  E   -  U     [   7  l     r   7  �   	  �   7  �   1  �   /  �   ,  �   -  �   	  �   7    	    7    	    0  >  	  D  0  h  "  x  1    /  �  ,  �  -  �  8  �  	  �  7  �  	  �  7     	    7  P     g     ~     �     �         ,    6    �    �    �    �                 	     &          :     ;          =  @   "  L   "  \   "  d     p     t     x     �     �     �     �          @  	  L  	  X  	  d  	  p  	  |  	  �  	  �  	  �  	  �  	  �  	  �  	   .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel__bug_table .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   �                    	   @       @  �              )             �  �                   %   	   @                         /             `                     8             `  4                  4   	   @       (  h               E      2       �  �                X             @                   T   	   @       �  �     	         g      2       T  V                 z             �                    v   	   @                         �             �                    �   	   @       (                   �             �  
                  �   	   @       H                  �             �  Y                  �             @  �                  �   	   @       X   �               �      0       (                   �              ;                     �             <  (                                d        &         	              d  �                               !  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          cmd_drivers/media/i2c/tw9910.o := gcc -Wp,-MMD,drivers/media/i2c/.tw9910.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"tw9910"' -DKBUILD_MODNAME='"tw9910"' -D__KBUILD_MODNAME=kmod_tw9910 -c -o drivers/media/i2c/tw9910.o drivers/media/i2c/tw9910.c 

source_drivers/media/i2c/tw9910.o := drivers/media/i2c/tw9910.c

deps_drivers/media/i2c/tw9910.o := \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/RETPOLINE) \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/clk.h \
    $(wildcard include/config/COMMON_CLK) \
    $(wildcard include/config/HAVE_CLK_PREPARE) \
    $(wildcard include/config/HAVE_CLK) \
    $(wildcard include/config/OF) \
  include/linux/err.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/bitops.h \