ask.h \
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
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/vpx3220.o: $(deps_drivers/media/i2c/vpx3220.o)

$(deps_drivers/media/i2c/vpx3220.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ELF                      �k      4     ( & %             �����Ud�    ��p  ��]�@�Í�    �����U��d�    ��p  ��P1�1�]Ð�����WVS�Ӊ��Kf����   f��uq�K����������   ���    �a	�f�B�B�J�B    �K��B    ����a�Z�˃���	وJ	1�f�J
�|$)����   [^_Ð[1�^_Í�&    �v ��|$�K���   [��a^_�J��   f�B�   �B    Ív ��	��   ��<	��   �	   �����A���9�Dȉ��{���B    	�$   f�r��B�C�����B    ��y $ ����a   ��	�1��B�C�Bc���f�z	��B    �B�K�B �B    ����	�J�������    �������j�����&    �v �   �F�����    �����U���t� ]�@$��  ��   Ð�    ]��  ��   Í�&    ��    �����UWV��S�Ӊ��Cf����  if����  f��u)�C�a�A    �A�   f�A�   [^_]Ít& �f����   �C�a�A1�[�   ^_f�J�B    ]Í�    f�� ��   f��$��   �C���������o  ���   �a�p  	�f�r�   �B�B    �C�Ba����f�z
���B    	ȈB	�C�Ba����f�j���B    	ȈB�   �.���f��u:�C����������   �a��[^�B    	��   _]f�B�   �J�[1�^_]Í�&    f��C���������k  �a��[^�B    	��   _]f�B�   �JÍ�    �C���������#  �a��[^�B    	��   _]f�B�   �JÍ�    �C�a�A�   [�   ^_f�J�B    ]Ít& �c����<	��   �	   ��	��   �F�����9�D����{���B    	ȉ�B���   f�B���C�����B    �̓��B    ����a  	�B�ȁ�y  a p	��B�C�Bc����B    	��B�   f�B�C�B$    ����	ȉB �   �i����v �c������c������&    ��   �,���<���p�-�����&    f������1�Í�&    �������������    �����UWV�Ƹ    S��������F(��t2���   �x<�h@�    ������{�k�F,�C[1�^_]Í�&    f�1�1��ԍ�&    �v ������P�@�����H�P�P�    �@  �    �����������UWV��S�ø    ������C(��t2���   �x<�h@�    ������s,UWh    V�������[^_]Ít& 1�1��ԍ�&    �v UW��VS�Ã� d�    �D$1��C��   >�t& �K$�S0����������C��   �D$d+    ��   �� ��[^_]Í�    ������s$�S0�D$��������s�������4�    ��$��F��V ������L$1�)�FV�F�$   �z�����t�����&    �B �B     �D$�B$�B$    �D$�B(�B(    �D$�B,�B,    �D$�B0�B0    �D$�����t& ��D$�G �D$�G$�D$�G(�D$�G,�D$�G0������������&    �t& �����U��WVS���P�M�u��t]�x�ú�����������t=�{ t7�$��t=��t9�������������  w�$���������s1�1��C   ��[^_]Ít& �������������[^_]Í�    �����WVS�t$��tb�Ӆ�tB�ǉʅ�tB��t>9��F������9�w�D� �����[^_�)�71҉��������[^_Ít& ������ݐ��1҉������������ʸ�����Í�&    �����U��W�}S�H��W�p���a���Y�e�[�_]Í�&    ������U��W�}S��W�x��u?��t#��t�щ�1�������e�����������[_]Ív �e�����������[_]Í�&    �H�p�������Y�e�[�_]�f������UWV��S�Ӄ����   d�    �D$1����   ���   �$���6  ���N�   ��%������   �F���f���  �K$)׋��  �s�L$�KT�s8���   ��KT+KXʉ4$�S8����   ����  �D$    �D$�����������D$�L$d�    �h��p  ��p  �@��t,��ډl$������u�|$ ���w�D$�����E����u�d�    ��p  �����������D$��uO�ST�$)��K8�ST;SX�  ��   �|$�{�T$d+    ��   ��[^_]Í�&    �v 1��ٍt& = ��������F�뢍�&    f����  �D$    �����������ud�    ��p  �L$�L$��p  �E1��t3f��ډt$�%����u�|$ ���w�D$�������	ŋ��u҃��L$d�    ��p  �����������T$�D- ��������� ��������FЅ�   D��������&    ��������&    f������UW��V��S�Ã��@ ��t������Ņ�x|����������Å�xS�)���tG�   �   �$�.9�F�������$��v9�������P9���Eʺ   �������x,�)����[^_]Ít& 1�눍t& ���É�[^_]Ít& ���É�[^_]Ít& �����U��V��S���M�Ud�    �D$1��E�L$�$��tY��t,1ۋV������Å�IÙ�L$d+    u?��[^]Ít& ���t"�@ ��L$������Ù��x͋L$�$벍t& ������������������&    �������xO��tJV��S�Í��w4�ؙ����u+��6�D$�@   ��w�V��   ��t��&    �v 1�[^Ív 1�Ít& ���[^��Í�&    �������x?W��1�V��S�Í��w!�ؙ����u��~&����w������t3[��^_Ít& �1�Ít& ���u�D$�@   ��w����&    f��D$1��@   ��w��F�����뱍t& �������'wVV�֙S����1���u"��tr����   ��t;��uN����   [^�f���$u;1���u�D$�x���⍴&    1�Ít& ���u΋D$�    �   �f�����볍�&    ��� t�!�����u���u��D$�x��늍�&    ��$t��y�����    ���j����D$� 	   �   �V����v �D$�    �   �?����t& �������h��   w ��f��   �   ��guE��Í�&    �v ��jtk�    =�   t�    ��it�=�   t~=�   u9��  ��Í�&    ��  ��et���Qtyw҃�tb�    ��Pt��    ��t��������&    f�de/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
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
  arch/x86/include/