g/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
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
  include/media/i2c/tvaudio.h \
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

drivers/media/i2c/tvaudio.o: $(deps_drivers/media/i2c/tvaudio.o)

$(deps_drivers/media/i2c/tvaudio.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ELF                       �      4     (            GNU  �       �          ��������
��  �C1�$j �   �؍T$�����Y���R  1��T$d+    u��[^_]��������&    ������UWVS�p�X|�@(�~�=	� �P  vN=	� ��  =
	� �  �۸   �   �   E؉�S�����~~ Xuu�t& �1�[^_]Í�&    f�=	� ti=	� ��  �ۍ��   �   �   H؉����À����S������~~ ]t�S�0   �   ������Y뙍�&    f�S�3   ����&    ����   ���   �-    �@|��	��  �ٺ� ���)ى���
����)������2  S1ɺ   ���<���X�݃�U�   �   ���$������    X��   �~~ �����S�   �I�����[�� �����  0 ������
���   ��)˹   ��S������~~ X�����S�1   ������t& ��[�� �����  0 ������
���   ��)˹   ��S�{����~~ X�`���S�2   �����t& ���>S�   1ɉ��   �H���Z����f�U�@   �   ���.���X��������������  �  �t& ������    S�X���]  �������1�[Í�&    ��&    �����V��S�X\����������  ��t���  ���������������   �����1�[^������U��WVS�� ��u�@�   �+  ����������������t  �E��
���D$
������=  �����D$�t$��>  	�	ȃ<$C��A  ��@  ��!΃|$�   ��  ��B  1Ƀ<$C��C  N��T$�L$
��D  ����F  !у<$G��G  ������E  ��!�����H  ��!��|$��I  !Ѓ|$��!Ј�J  ��   ��v �<$F~ǃP     ��t& �1��<$C����P  j ���   1ɺ   ���������G   ^��   j �	� �0  ��h �  j h�  j h��  j j ������	� �0  �� ���  ��j h �  j h�  j h��  j j ������		� �0  ���� j j j jj jj j ��������   �{l���  �� ����   ����������� [^_]Í�&    f������   1��n�����&    j �	� �0  ��h �  j h�  j h��  j j ������	� �0  ���� j h �  j h�  j h��  j j ������
	� �0  ���� j j j jj jj j ������� �����t& �ƃB  ��C  �������    �    ���������  ������������  �   ��������������=   �u  �W  �}  ��&    ��&    ������Ѻ   ������&    ��&    �������Ѻ   ������&    ��&    ������Q�Ѻ   �N���ZÍ�&    �t& ������Q�Ѻ   �.���ZÍ�&    �t& ������WVS�ÉЉʋs\��t  ��w��v?�`  �=    ��p  ���*  Q�   �   ���������E   Zu:[^_Í�    �ɍ|�<�`  ���t��`  ��#�p  	�렍�&    �v ��x  �ع@   �   �y���X[^_Ít& �����UW��V��S�Éȃ����   �<$�׃��|$���������|$�L$9��  �  ���  �   1����  �   �����Dǃ���u���	   ǃ�     ET$1ɉ�X  ��������T$�   ��������T$�   ����������������   �   ��P  �0   E؉���������$��%��  ����   	�R�   �n����F\Z���  ��t'���  �   �  ������P�   j �����X��1�[^_]Í�&    9��  ������፴&    �v �����UWV��S�Í��  d�-    ��d�    �D$1�����$    �D$    �D$    �l$�D$    ������������u��xN��������������������������    d�    ��u4���  ���T$d+    u1��[^_]Ít& ��   �E�����믐���������t�1��������������&    �����S�Ë��  �@�@��������  ���   �P|���  ���   �P|���  �~������  [�@�@�������&    �t& ������VS��L  ���   ��ug�
   ���������   ���������   ���������C   u$[^_É�   �������	   �������뭍t& ��غA   [^_�������&    ��&    ������WVS�Éȃ��{\���  ��w;�4�4  ��   ��u7���� ��5    �҅��o  ����[^_������v �    �   ��tɃ�
~��I   t�p��	��f�����	�뮍�&    �����U�   WVS�h\�Ë��  ���������H������   ���:�����	   �������&�����
   ������������C   u���غ   [��^_��]��������A   ������������э�&    �v �����S��  ��1ɸ�  ���&    �v �B��t�
��9�u�[Í�    ��  [Ð�����W��V�Ή���S���  �ú�   ���������   ���������   �؁��  ���������   ���������1�[�V   ^_�������&    f������U��  WV��S���x\���  ��w��H  ��P  �
��[^_]�f����ع Z  �   [^_]�������&    ������S�P\�Ë�P  ��~��u��T   t,��[������t& ��y[Í�&    �v ��[������&    ����  t���  �����t�    �    �������몍�    �!   �    �������둍�&    ��    �����UWV��S���x\��P  ����   ��u�   ������ŉ���1���   ��@��X  	tK�   ��D؋��  ���  ��t��T   ��  Ǉ�     �   �=     �L  9�������[^_]Ít& ��y��1�[^_]�f���l  ��X  �˃���   �P�1���wV�#   ��������=    �#  �Ë��  �   ��uT�t& 9���   �
�   E��x�����&    ��   �f�����    �   �������=�  ��  ��O=    ��  ���  1�=    �= ����   �   M������v �����  ����f���l  1������x  ��&    ��&    �����U��WVS���X\�    ���.	  ǃd  ��� ǃh      ������E�$�v �=    �	  �������������    ����	  ���  ���������E  ��X  	�(  ǃ�     �ؾ@   �������L  ��u��\  % �  ������� ���  ���   ��ǃl      ������ǡ    ����  �=    ��  ���   1�1��"��&    �v ���@�լ  ��t�ՠ  9�u�    ����  �@��T  �=     �Š  ǃ�     �P�@��|  ���  �]  ��L  ���O  ��\    � �?  ��	�6  �
����둸�  ��t& ���;1tF�P����u��	  �v �D$��  ��  1����    �J����  ���9�u�D$��  �	  ��&    ��&    �����UWV��S���=    �X\����
  ǃd  ��� �~ǃh      ������    �<$�t& ����u
  �������������    ���U
  ���:
  ���  ���������  ��X  	�  �n\��ǅT     ���  ���������E   �r  �F\��L  ���!  ��\  ��`  �р� 	���  �р��	��  �с�  @ 	��{  �p  ����  ���0   ����   	����������������   �
   �t& �d   �����������  �~   �������=�  �5  �=    ��	  ��uË�T  ���%  �-    ����  1�1�����@�<լ  ����   �ՠ  9�u��@�Ŵ  �Ű  ��d  ��h  ��	�b  ���������p  �   ����������  ��T   ��   �    ����f��
  �t& �������������t& �    ���  �@  �����    ����
  �`  �|����    ����
  1���[^_]Ã������[^_]ø�  1���  ���&    �P���
  �8��9�u��
  ��&    �����UW��VS���p\�    ���0  ǆd  ��� ǆh      ������G�D$f��=    �  �������������    ����
   ���������uh�   �t$������������X���Z�  �L  WVU��h�   �t$������
   ���������u@h�   �t$������������X���Z�
  �C�t$$QRh�   Ph   ���������  ��  ���   �I|����   ���  �  �  D�Q�   RP�Gh�   Ph   �����1ɺ   ��S�  ���f  ���   �p|�Gh  h  h�   Ph   �����1ɺ   ��S�  ���a  �  �  �h  Ph   ��������  �+   ��<   �  D׋�P  �4��   WRP�E  P�D$(��P�D$$�ă�@P�t$�t$<�t$(h(  �t$D�������P  ��0���Q	  ��    ���Q	  hk  j�UP������ǃ�= �����   ��������  �E\���   �Q	  ���  �   �  ������P�   j �����[�Q	  �+   �  �  ��<   D�D��%  ������4  �t$hQ  �t$$h   �������0  �D$ ����t��4  �|$	ǉD$��	  �=     �L	  h�   ������t$h   ��������Q	  �Eh3  �����Ph   ��������Q	  ���  hs  �t$�����ZY�����QR�4�@  �Chd  Ph   �������p  ���
  �FRh�  Ph�  ������
   2   V            0   B   H   �cL �cL �   @       0        ��������(   ^            0   B   H   UUS UUS �   @       0  ������������
   2   V   ������������   O   ~   GZ GZ �   @      |              �  ���                �  ���         �> 9�A �   �         �cL ��O            GZ  �V #  � 2        GZ 9�] A  � 2        GZ GZ _  � 2        GZ ��O x  � 2        �cL  @Q :        	   GZ  @Q �    �     
   UUS ��Z �            GZ  @Q �  � 2        GZ  @Q T  � 2     
 7 unknown , dual  stereo mono %s: Audio:     Mode:     %s
 Mode:     %s (%s%s)
 Standard: %s (%s%s)
 Audmode:  0x%04x
 ACB:      0x%04x
 resume
 msp_reset
 chip reset failed
 msp_read(0x%x, 0x%x): 0x%x
 msp_write(0x%x, 0x%x, 0x%x)
 no on yes off suspend
 nicam  and  radio msp3400 msp3400 not found
 &state->wq rev1=0x%04x, rev2=0x%04x
 msp34xx kernel_thread() failed
 switching to radio mode
 AM (for carrier detect) FM Radio Terrestrial FM-mono/stereo Satellite FM-mono NICAM/FM (B/G, D/K) NICAM/FM (I) NICAM/AM (L) BTSC External input manual autodetect autodetect and autoselect in1 in2 in3 in4 in1 da in2 da mute 7 unknown FM set_audmode: %s
 SAT set_audmode: %s
 NICAM set_audmode: %s
 BTSC set_audmode: %s
 extern set_audmode: %s
 FM-Radio set_audmode: %s
 mono set_audmode
 set audmode %d
 switching to AM mono
 set_mode: %d
 stereo detect register: %d
 nicam sync=%d, mode=%d
 watch: nicam %d => %d
 msp3410 daemon started
 msp3410 thread: sleep
 msp3410 thread: wakeup
 thread: restart scan
 thread: no carrier scan
 detection still in progress
 5.5/5.85  B/G NICAM FM thread: exit
 msp34xxg daemon started
 msp34xxg thread: sleep
 msp34xxg thread: wakeup
 selected radio modus
 selected M (EIA-J) modus
 selected M (A2) modus
 selected SECAM-L modus
 selected M (BTSC) modus
 msp3400 daemon started
 msp3400 thread: sleep
 msp3400 thread: wakeup
 AM sound override
 carrier1 val: %5d / %s
 carrier2 val: %5d / %s
 mono stereo lang2 lang1 lang1+lang2 5.85  PAL D/K + SECAM NICAM 6.25  PAL D/K1 FM-stereo 6.74  PAL D/K2 FM-stereo 7.02  PAL SAT FM-stereo s/b 7.20  PAL SAT FM-stereo s 7.38  PAL SAT FM-stereo b 5.742 PAL B/G FM-stereo 5.85  PAL B/G NICAM 4.5   NTSC 5.5   PAL B/G 6.0   PAL I 6.5   PAL D/K + SAT + SECAM autodetect start 4.5/4.72  M Dual FM-Stereo 5.5/5.74  B/G Dual FM-Stereo 6.5/6.25  D/K1 Dual FM-Stereo 6.5/6.74  D/K2 Dual FM-Stereo 6.5  D/K FM-Mono (HDEV3) 6.5/5.74  D/K3 Dual FM-Stereo 6.5/5.85  L NICAM AM 6.0/6.55  I NICAM FM 6.5/5.85  D/K NICAM FM 4.5  M BTSC-Stereo 4.5  M BTSC-Mono + SAP 4.5  M EIA-J Japan Stereo 10.7  FM-Stereo Radio 6.5  SAT-Mono 7.02/7.20  SAT-Stereo 7.2  SAT ADR     `   �      @  �     P  �  �  �  0  �  �      
 %s rev1 = 0x%04x rev2 = 0x%04x
 Routing:  0x%08x (input) 0x%08x (output)
   I/O error #%d (read 0x%02x/0x%02x)
 resetting chip, sound will go off.
 I/O error #%d (write 0x%02x/0x%02x)
    mute=%s scanning=%s volume=%d
  not an msp3400 (cannot read chip version)
  MSP%d4%02d%c-%c%d found on %s: supports %s%s%s, mode is %s
 scart switch: %s => %d (ACB=0x%04x)
    set source to %d (0x%x) for output %02x
    could not detect sound standard set_audmode called with mode=%d instead of set_source (ignored)
    set_audmode final source/matrix = 0x%x
 watch: rxsubchans %02x => %02x
 status=0x%x, stereo=%d, bilingual=%d -> rxsubchans=%d
  setting standard: %s (0x%04x)
  current standard: %s (0x%04x)
  autodetection failed, switching to backup standard: %s (0x%04x)
    started autodetect, waiting for result
 detection still in progress after 10 tries. giving up.
 detected standard: %s (0x%04x)
 mono sound carrier: %d.%03d MHz
    main sound carrier: %d.%03d MHz
    NICAM/FM carrier  : %d.%03d MHz
    NICAM/AM carrier  : %d.%03d MHz
    FM-stereo carrier : %d.%03d MHz
    6.5/5.85  D/K NICAM FM (HDEV2)  6.5/5.85  D/K NICAM FM (HDEV3) parm=dolby:Activates Dolby processing parm=amsound:Hardwire AM sound at 6.5Hz (France), FM can autoscan parm=standard:Specify audio standard: 32 = NTSC, 64 = radio, Default: Autodetect parm=stereo_threshold:Sets signal threshold to activate stereo parm=debug:Enable debug messages [0-3] parm=once:No continuous stereo monitoring parm=opmode:Forces a MSP3400 opmode. 0=Manual, 1=Autodetect, 2=Autodetect and autoselect parmtype=dolby:bool parmtype=amsound:bool parmtype=standard:int parmtype=stereo_threshold:int parmtype=debug:int parmtype=once:bool parmtype=opmode:int license=GPL author=Gerd Knorr description=device driver for msp34xx TV sound processor �          ��     �          ��     �          ��     �          ��     �          ��               ��               $� �             �                                  �                  +                                                              �           `                       �     ����                     @Q �   �V �  9�]    �a     d 9   �f S  ��O m   @Q �   �> �  �cL �  UUS �  GZ �   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                            
 �   @  c     �   �  B     �      P     �   �        �   P  |     �           �           �            �   �  3     �   �  �     �   �  p     
 {           �  �        �  0      
 �  y  �    �  �       
 �          
 �  `  l    
 �  .  *     �  @       
 �  0  V    �  �  �     �  X                        �            
     0  �   \    
 ?  `   0    
 F     0    
 S  `  ,    
 a  @      
 o  �  P    
 }      &     �  &   B     �  h   Q     �  �   ?     �  �   '     �    *     
  I  Y        �       9           G  �      
 Y  �       t          �  �      
 �  �       �  (        �  �  	    
 �  �       �  <          �      
 4          M  P        [  �      
 m         �  d        �        
 �  &       �  x        �        
 �  :       �  F         X  9     '           ��:  �  �     I  @  �     ]  4      
 g         
 q  s  .     �  �  �     �  �     
 �     5    �  H      
 �  �  -    �  `  @   
 �  �  $       �  �     %  �  3    :  �	  �    O           l  �   0     �  �        �  �  O    �             �          �             �             �                          
     [  �        m  `
    �	  `  X    �	  @
             
  p  T     )
             1
             >
             M
             _
             v
             �
             �
  `   0    
 �
             �
   
   
             �
             �
               �   �                 -          6              msp3400-driver.c msp_s_i2s_clock_freq msp_s_i2s_clock_freq.cold msp_querystd msp_s_tuner msp_log_status msp_log_status.cold CSWTCH.74 msp_g_tuner msp_s_frequency msp_resume msp_resume.cold msp_s_std reset_off.1 reset_on.2 write.3 msp_reset.cold msp_read msp_read.cold msp_write msp_write.cold msp_s_ctrl msp_s_ctrl.cold msp_suspend msp_suspend.cold msp_remove msp_probe msp_ops __key.4 msp_ctrl_ops msp_probe.cold opmode_str CSWTCH.84 scarts msp_set_scart.cold scart_names msp_s_routing msp_s_radio msp_s_radio.cold msp_driver_init msp_driver msp_driver_exit msp3400_pm_ops msp_id msp_core_ops msp_tuner_ops msp_audio_ops msp_video_ops __UNIQUE_ID_dolby282 __UNIQUE_ID_amsound281 __UNIQUE_ID_standard280 __UNIQUE_ID_stereo_threshold279 __UNIQUE_ID_debug278 __UNIQUE_ID_once277 __UNIQUE_ID_opmode276 __UNIQUE_ID_dolbytype275 __param_dolby __param_str_dolby __UNIQUE_ID_amsoundtype274 __param_amsound __param_str_amsound __UNIQUE_ID_standardtype273 __param_standard __param_str_standard __UNIQUE_ID_stereo_thresholdtype272 __param_stereo_threshold __param_str_stereo_threshold __UNIQUE_ID_debugtype271 __param_debug __param_str_debug __UNIQUE_ID_oncetype270 __param_once __param_str_once __UNIQUE_ID_opmodetype269 __param_opmode __param_str_opmode __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 msp3400-kthreads.c msp_set_source msp34xxg_set_source CSWTCH.91 CSWTCH.92 msp34xxg_set_source.cold msp34xxg_set_sources msp_stdlist msp3400c_set_audmode strmode.0 msp3400c_set_audmode.cold msp3400c_init_data msp3400c_set_mode.cold msp_detect_stereo.cold msp3410d_thread.cold msp34xxg_thread.cold msp3400c_carrier_detect_main msp3400c_carrier_detect_65 msp3400c_carrier_detect_55 msp3400c_thread.cold v4l2_i2c_subdev_init msp_debug devm_kmalloc __msecs_to_jiffies __this_module snprintf __stack_chk_guard msp3410d_thread v4l2_ctrl_handler_init_class cleanup_module msp_stereo_thresh msp_write_dsp set_freezable msp_dolby schedule_timeout_interruptible msp_amsound add_wait_queue msp3400c_set_carrier i2c_transfer_buffer_flags kthread_should_stop __wake_up msp_detect_stereo __fentry__ init_module wake_up_process __refrigerator msp_standard_std_name msp_reset v4l2_ctrl_handler_log_status v4l2_ctrl_handler_setup schedule __stack_chk_fail msp_set_audmode _dev_info i2c_register_driver v4l2_device_unregister_subdev _dev_err msp_standard mutex_lock msp_sleep media_entity_pads_init kthread_stop freezing_slow_path current_task _dev_warn msp_set_scart default_wake_function __init_waitqueue_head msp3400c_thread msp3400c_set_mode msp_write_dem kthread_create_on_node msp_update_volume strscpy mutex_unlock param_ops_bool v4l2_ctrl_new_std v4l2_ctrl_handler_free system_freezing_cnt i2c_transfer __mod_i2c__msp_id_device_table remove_wait_queue msp_read_dsp msp_read_dem i2c_del_driver param_ops_int _dev_printk msp34xxg_thread v4l2_ctrl_cluster msp_once __SCT__might_resched     �  
   u       a   �  �   u  �   �  �   	  �     �   �  �   �  �   �    �    z  )    0  �  5    A  �  �  �  �  �  �  �    �  
  u      E  �  Q  �  �  �  �  �  �  z  �  u  2    B  
    )  |  C    a  �  k    �  �  �    �  �  �  �      .  �  8    [  �  e      �  �  u  �    �  �  �  �  �    �    �    
    $    4    R  �  W  u  _    �  �  �  u  �    �    �    �  u    u  
      �  =    B  u  J    a  �  �  u  �    �    �  �  �    �  �  �  �    �    �  #    /  �  D    N  �  [  �  w    �    �    �  �  4  �  M  �  a  �  �  �  �  u  �    9  �  ?  u  j  u  r    �  u  �      �    u  #    n    �  �  �  u  �    �  �  �  u  �    �  �  �  u  �    �  �    �  W  �  ^  u  l  u  �    �    �  u  �  �  �      u  $    ?  �  p    �    �  �  �  �  �  �  �  �    �  %  u  3    C  �  a    p    ~    �      �    �  1  u  9    J  �  v  �  �    �  �  �  �  �  u  �    �  �  A   u  I     e     �     �     �   	  �     �     �     �   �  �   u  �     !  �  !  u  !    )!  �  .!  u  7!    ?!    K!  �  }!  �  �!  �  "    !"    4"  ~  @"  �  \"  �  {"  �  �"  u  �"    �"  �  �"  �  �"  u  �"    �"  u  	#    #    &#    -#    I#  �  [#    u#  u  �#  u  �#    �#  �  �#  u  �#  u  �#    �#    $  �  -$  �  ;$  �  R$  �  h$  �  �$  �  �$  �  �$  u  �$    �$  u  �$    �$  u  �$    %  u  %    +%  �  :%  u  B%    Q%  u  Y%    h%  u  p%    �%    �%  	  �%    �%    �%  �  �%  u  �%    �%  �  &  u  	&    &  �  &  u  #&    /&  �  [&  �  d&  �  �&  �  �&  u  �&    �&  u  �&    �&  �  �&  �  �&  u  �&    �&    '    ''  �  x'  �  �'  u  �'    �'    �'  �  �'  �  �'  u  �'    �'  �  (  �  8(  u  @(    Z(  �  f(  �  o(  u  �(  �  �(    �(  u  �(    !)  �  4)  �  L)  �  Q)  u  �)  �  �)  �  �)  �  �)  �  �)  �  �)  �  �)    C*    U*  �  �*    �*  u  �*    �*  �  +  u  +    !+    E+  u  R+    l+    y+  �  �+    �+    	,  �  ,    ,,  �  h,    �,  �  �,                 �  .     F   	  L   �  U     a   y  p   �  �     �     �     �     �     �     �   �  �     �     �   �  �     �   �     	    �        �  %  z  <    B  �  P    U    Z    f    t  �  |    �  �  �  �  �    �    �  �  �    �    �  �  �    �    �    �  �  �    �    �    �  �        	    �  &  �  3  	  <  �  