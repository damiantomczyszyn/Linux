/config/GENERIC_IOMAP) \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ELF                       j      4     (               �����S��X  ��T  1��Z[�
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
v-�C@  1��_�����&    �v 9�s ������0�t�����    �����t& �������               �                 [                                                              �           �                                                         �                                               �       $                                                                                             @   p   �      @  �  �  �  @  F   �  �  @  �  0  �        p  `	  @
  p
    0  �  �  @    �  �             Set video std register to %d.
  tvp5150.c: removing tvp5150 adapter on address 0x%x
    VBI can't be configured without knowing number of lines
    Selecting video route: route input=%s, output=%s => tvp5150 input=0x%02x, opmode=0x%02x
    link setup '%s':%d->'%s':%d[%d] tvp5150: Video input source selection #1 = 0x%02x
  tvp5150: Analog channel controls = 0x%02x
  tvp5150: Operation mode controls = 0x%02x
  tvp5150: Miscellaneous controls = 0x%02x
   tvp5150: Autoswitch mask= 0x%02x
   tvp5150: Color killer threshold control = 0x%02x
   tvp5150: Luminance processing controls #1 #2 and #3 = %02x %02x %02x
   tvp5150: Brightness control = 0x%02x
   tvp5150: Color saturation control = 0x%02x
 tvp5150: Hue control = 0x%02x
  tvp5150: Contrast control = 0x%02x
 tvp5150: Outputs and data rates select = 0x%02x
    tvp5150: Configuration shared pins = 0x%02x
    tvp5150: Active video cropping start = 0x%02x%02x
  tvp5150: Active video cropping stop  = 0x%02x%02x
  tvp5150: Genlock/RTC = 0x%02x
  tvp5150: Horizontal sync start = 0x%02x
    tvp5150: Vertical blanking start = 0x%02x
  tvp5150: Vertical blanking stop = 0x%02x
   tvp5150: Chrominance processing control #1 and #2 = %02x %02x
  tvp5150: Interrupt reset register B = 0x%02x
   tvp5150: Interrupt enable register B = 0x%02x
  tvp5150: Interrupt configuration register B = 0x%02x
   tvp5150: Video standard = 0x%02x
   tvp5150: Chroma gain factor: Cb=0x%02x Cr=0x%02x
   tvp5150: Macrovision on counter = 0x%02x
   tvp5150: Macrovision off counter = 0x%02x
  tvp5150: ITU-R BT.656.%d timing(TVP5150AM1 only)
   tvp5150: Device ID = %02x%02x
  tvp5150: ROM version = (hex) %02x.%02x
 tvp5150: Vertical line count = 0x%02x%02x
  tvp5150: Interrupt status register B = 0x%02x
  tvp5150: Interrupt active register B = 0x%02x
  tvp5150: Status regs #1 to #5 = %02x %02x %02x %02x %02x
   tvp5150: Teletext filter enable = 0x%02x
   tvp5150: Interrupt status register A = 0x%02x
  tvp5150: Interrupt enable register A = 0x%02x
  tvp5150: Interrupt configuration = 0x%02x
  tvp5150: VDP status register = 0x%02x
  tvp5150: FIFO word count = 0x%02x
  tvp5150: FIFO interrupt threshold = 0x%02x
 tvp5150: Line number interrupt = 0x%02x
    tvp5150: Pixel alignment register = 0x%02x%02x
 tvp5150: FIFO output control = 0x%02x
  tvp5150: Full field enable = 0x%02x
    tvp5150: Full field mode register = 0x%02x
 reading ID registers failed: %d
    tvp%04x (%u.%u) chip found @ 0x%02x (%s)
   *** unknown tvp%04x chip detected.
 %s left=%d, top=%d, width=%d, height=%d
 7 width = %d, height = %d
 %s reg %02x = %*ph
 g_sliced_vbi_cap
 ck ss sync lo%s signal
 normal black-frame-gen svideo aip1a aip1b %s: failed with error = %d
 Teletext filter 1 Teletext filter 2 tvp5150: FIFO reset = 0x%02x
 CC   data WSS  data VPS  data VITC data Line mode pdn reset tvp5150a detected.
 tvp5150am1 detected.
 tvp5151 detected.
 tvp5150 include/media/v4l2-subdev.h Disabled Black screen �$Ph    ���   h    ������L$���D  RSh   ���   h    �����1����  �����UW��VS�ك��T$�l$0d�    �D$1��,$��9$~Q�4$)�;t$4Ot$41�9�}������|  �D<G��D$PVS�t$h   ���   h    �����\$P��몋D$d+    t�������[^_]�h0   ���   h    ���������  �@�Ph    ���   h    ��������  hX   ���   �����1�XZ�j  ���E   �B   D�PhH   ���   h    ���������  ��h   �Z   �a   Eх�t2���q   �~   E�j VRPh�   ���   h    ��������  �x   �׋L$$�D$��Q�T$�L$R�V�r�WR�uh�   ���   h    ������D$,�� ����  �=     �L$�   ��  Q�D$������   R���   �p�GP�G�ph�   ���   h    ������� �  �D$$��P�D$P�F�p�GP�G�ph�   ���   h    ������� ��  PhX   h�   ���   �����1����;  hX   ���   �����X1�Z�;  �D$Ph  ���   h    �������L  �   �L$����������'
  PhD  ���   h    �������L  �   �L$�����������	  Php  ���   h    �������L  �   �L$�����������	  Ph�  ���   h    �������L  �   �L$�����������	  Ph�  ���   h    �������L  �   �L$����������w	  Ph�  ���   h    �������L  �   �L$������ǃ����I	  ��L  �L$�   ������ƅ�x�t$��L  �L$�   �������x�D$WVPh   ���   h    �������L  �	   �L$�����������  Phh  ���   h    �������L  �
   �L$�����������  Ph�  ���   h    �������L  �   �L$�����������  Ph�  ���   h    �������L  �   �L$����������]  Ph�  ���   h    �������L  �   �L$����������1  Ph   ���   h    �������L  �   �L$����������  Ph4  ���   h    �������L  �   �L$������ƃ�����  ��L  �L$�   �������x�D$VPhd  ���   h    �������L  �   �L$������ƃ�����  ��L  �L$�   �������x�D$VPh�  ���   h    �������L  �   �L$����������C  Ph�  ���   h    �������L  �   �L$����������  Ph�  ���   h    �������L  �   �L$�����������  Ph  ���   h    �������L  �   �L$�����������  PhD  ���   h    �������L  �   �L$������ƃ�����  ��L  �L$�   �������x�D$VPhp  ���   h    �������L  �   �L$����������H  Ph�  ���   h    �������L  �   �L$����������  Ph�  ���   h    �������L  �   �L$�����������  Ph  ���   h    �������L  �(   �L$�����������  PhH  ���   h    �������L  �-   �L$������ƃ�����  ��L  �L$�,   �������x�D$VPhl  ���   h    �������L  �.   �L$����������M  Ph�  ���   h    �������L  �/   �L$����������!  Ph�  ���   h    �������L  �0   �L$�����������  ���   )�Rh�  ���   h    �������   ���|  ��   �Ɖ��|  VPh,  ���   h    �������   �؃�$�|  ��   �Ɖ��|  VPhL  ���   h    �������   ���|  ��   �Ɖ��|  VPht  ���   h    �������   �؃�(�|  Ph�  ���   h    �������   ���|  Ph�  ���   h    �������   �؃� �|  ��   �$���|  ��   �ŉ��|  ��   �ǉ��|  ��   �Ɖ��|  �$QUWVPh   ���   h    �������   ��   �؃� jh�   ������   ��   ��jh�   ������   ���|  Ph<  ���   h    �������   �؃� �|  Phh  ���   h    �������   ���|  Ph�  ���   h    �������   �؃� �|  Ph�  ���   h    �������   ���|  Ph�  ���   h    �������   �؃� �|  Ph  ���   h    �������   ���|  Ph@  ���   h    �������   �؃� �|  Ph�   ���   h    �������   ���|  Phl  ���   h    �������   �؃� �|  ��   �Ɖ��|  VPh�  ���   h    �������   ���|  Ph�  ���   h    �������   �؃�$�|  Ph�  ���   h    �������   ���|  Ph	  ���   h    �������   ��   �؃� jh�   ������   ��   ��jh�   �~�����   ��   ��jh�   �f�����   �   ��j
h�   �N����
  �ع�   �� jh�   �3���XZ�D$d+    ��   ��1�[^_]ËD$������D$������D$�(����D$�T����D$�����|$�����D$�����D$�B����D$�n����D$�����D$������D$������t$� ����t$�k����D$�����D$������D$�����D$�8����t$�f����D$�����D$������D$�����D$�3����t$�a����D$�����D$������D$����������D$�T$�$�L$f��f��f��v  ��f��x  �S�$��  R�S�R�T$RQPhh	  ���   �������v  ��f=PQ��   ��x  ���   f��!��   h  R�����XZ���  1�9���   iȌ   ��T  ���8  �$��X  �<  �ы$��X  ��T  �PhD	  �D$��P���������  ���   f=QQu$f��x   uhH  R�����XZ�s���f�� tPh�	  ���   ��������S���h2  R�������L  1ɺ0   �����Y[�/�����uǆT  ��� ǆX      ���  1ɺ   ǆ\      �$��j ǆ`      ǆd     ǆp     ������ 	� �  ��j h�   j jj h�   j j ������	� �  �؃�$j h�   j jj h�   j j ������	� �  �؃� j h�   j jj h�   j j ������	� �  �؃� j j j jj jj�j�������	� �  �؃� j h���j jj h���j h���������  �ع	� �� h  j j j j������^l��  ���څ�t�$�T$������\$��  ��L  �L$��   �������x�D$���@  ����
��   ���  ��H  1�ǆ@      ��P  ��ǆ<      ǆD  �  �,  ���  ��t&V1ɉ�h[  h   h   ����������R�������������A����=    ~����  1҉�������������1҉��������  ��    % �  ��҃�`���  �9����q�q�q�qhh   h�	  ���   h    ������� �B     W  �  �  c  �	P  c  �	������    �    ������    �����           �              �                                              @                �     tvp5150_get_vbi tvp5150_set_selection   tvp5150                                                                                                                                         @           �  @  
                                                (   (   ,   0 