clude/config/NO_GENERIC_PCI_IOPORT_MAP) \
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
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/linux/of_graph.h \
  include/linux/pm_runtime.h \
  include/linux/regmap.h \
    $(wildcard include/config/REGMAP) \
  include/linux/iopoll.h \
  include/media/v4l2-async.h \
  include/media/v4l2-device.h \
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
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
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
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-event.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \
  include/media/v4l2-fwnode.h \
  include/media/v4l2-mc.h \
  include/media/v4l2-rect.h \
  drivers/media/i2c/tvp5150_reg.h \

drivers/media/i2c/tvp5150.o: $(deps_drivers/media/i2c/tvp5150.o)

$(deps_drivers/media/i2c/tvp5150.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ELF                      X)      4     (               �����W�zV��<  ��������   ���   )�1����   )����^_Í�&    f�������B��u$WV�2��w+i��   �z�!   1���@  �^_ø����Í�&    �v ������捴&    f�������A��u�A   1�Í�&    �v �����Í�&    �v ��������A   �ȋ�<  �I�H��<  �I�H��<  ���   �H��<  ���   �P1�Í�&    �����U��WV��S�Ã�p�����   �$    ����������������1҃���[^_]Í�&    ��    �����VS�Ë@�{(	� uK��@����K|�
   ��������t	[^Í�    �K|�	   ���h����K|��u߉�[�   ^�R���f�������ȍ�&    f�������=     S�X\�M   ����������   �����1�[Ít& ��������A   �ȋ�<  �I�H��<  �I�H��<  ���   �H��<  ���   �P1�Í�&    �����UWVS���u5�ōz1���&    �v ��D  �   ���������t���Ø   ��uܸ����[^_]�f�i��   ��@  ��<  ���  ���\u�׍�    �S����\tŀ{u��u��K���'����ݍt& ������UWVS�Á�   �sXd�    ��$�   1�����  �C�P�R�����%   =   ��   �C��  �`  ������Ņ���   ��0  �@   �ھ   ǀ<  @  ������C�S��  QRR��   �CT�0h0   ��������   �����&    �v �
   ���������  1҉�������Å�x��h   �������$�   d+    u�Ĉ   ��[^_]û�������������&    �t& �����9�8  tSUWV��S�Í@p�����x$�$    �D$����   ����������  ��8  1҃���[^_]Í�&    1�Í�&    ��    �����UW�   V��S���   ���   �$��t& �
   ���������  �7   ��������Å�xۋ$�\$�   ���   ���&    �
   ���������  �8   ��������Å�xۉD$�\$�   �$�\$���   ���&    �
   ���������  �9   ��������Å�xۉD$�$���   �   ������v �
   ���������  �:   ��������Å�x��|$�ŋ\$��  ���    1ɉ��ہ�   ������%   	��D$	Ÿ@  ���&    ���   �=h  tW���   9�u�f;��   u����   f���t9�w����   9�r���&    �=     �  1ۃ���[^_]Í�&    �    ��uԄ���  ������ԍ�&    ��    �����WVS�Ӄ�d�    �T$���������u6i$�   �{�����@  ��@  ����   ���   )����   )����T$d+    u��[^_��������&    ��&    �����VS�Ísp��d�    �D$1�����l�����<  �p�pVh�   ������D$�����/  �          �  �              �                                                                                                       @   �   �     `  �     P  �    �  p  �      4%s: Write: retry ... %d
 3%s: TVP7002 write error %d
 No platform data
 6%s: Rev. %02x detected.
 3%s: TVP7002 read error %d
 7%s: Fail to set streaming
 7%s: detected timings: %d
 yes no 6%s: Streaming enabled: %s
 tvp7002 �D$�4$Sh    ������
   ������D$�D$���T$���0  R�T$Sh   ������T$���E  �@P�CpPh    ���������  ���upPVhK   ���������tVh\   �����XZ1�1Ҿ@  �~u�N��u	���  ���V����\u݅���  ��0  �   �H�@����	���� �  �Å���  ��0  �J�B���	���   	�����  �Å���  ��苵<  ��!   ��L  �M\�   ��ǅ\     ��@  �E  ������Å���  j ���   1ɺ   ��������	� �0  ��j j j jj h�   j j ��������   �ul��$��t=���������  ��h9   S�����Y[�������  ��pSUhf   ���������  ���������������Å�u���  �D$�4$�t$h    ������
   ������D$�D$���T$���=  R�T$�t$h   ������T$���=     �[  �t$h�   �����XZ�$�[  S�D$��pPhf   ��������3  US�D$�������pPh�   ��������3  S�D$��pPhf   ��������3  S�D$��pPhf   ��������3  S�D$��pPhf   ��������3  Q�D$1ۃ�pPh�   ��������3  i��   ��H  ��D  Vh�   ���������8   ��   ��   D�PVh�   ��������   ����������D$d+    u��1�[^�Vh�   �����XZ������7%s: Removing tvp7002 adapteron address 0x%x
  6%s %d-%04x: tvp7002 found @ 0x%02x (%s)
  6%s: Unknown revision detected.
   7%s: detection failed: lpf = %x, cpl = %x
 6%s: Selected DV Timings: %ux%u
   6%s: Detected DV Timings: None
    6%s: Detected DV Timings: %ux%u
 ������    �    ������    �����                            tvp7002                                                                    �               `       �           �                          @                                                                         P      p                              �                                                      `             �         �l    n   (   �                           �                                                              �         �� �     �  8        �l    X   ,   �                        �                                                              �          e� �     �  8        �l      ,   �                        �                                                                         e� �        �         �l    �  (   �                           �                                                              �         �� �     �  8          ��    X   ,   �         $                  �                                                              @         eZ f     �  �          ���       >   <   	                        �                                                                      ����    �  @          ���       @   D         '                  �                                                            �         q����                        {��@GABKCDE F-G 1Z2  \�             g �@GABKCDE F-G 1Z2   \�             � �@AB�CDEFG1Z2  \�             ���@AB�CDEFG1Z2  \�             ���@AB�CDEFG1Z2  \�             6 @�A BC D E F-G 1\�             5�@�A BC DEFG1\�              � g ��2 ` 	 
 ��� .]G  � G  gw  !" #� $� %� &�'� (S)*+ ,P- .�/�01Z23`4�56 7� 8� 9� :� ;� <� =>`?@A,BC,DE FG H I J�KLOMN�OP�Q�R STUU�VxW�X�Y�Z [\� debug parm=debug:Debug level (0-2) parmtype=debug:bool license=GPL author=Santiago Nunez-Corrales <santiago.nunez@ridgerun.com> description=TI TVP7002 Video and Graphics Digitizer driver  X          ��      GCC: (GNU) 11.2.0           GNU  �       �                                  ��                                                 7         @   G     8   @  (    H   �   &     _   �   I                                	 v     C     �       Q    	 �   `  g                   �   �  ,                �   Q       	 �      I     �   P  �     �   �        @           l   �   	 "  @      7  0       H    c     Y    w    	 o  �  �    �  �  �    	 �  p  r     �  �  P     �  3  y    	               �           �      �     �      
           0                        0     -  �   P     ?  `   @     O  �  3     d  �  3     z     3     �  �  3     �  @  3     �     3     �  �  3     �           �                       X       /  1        F  =   =     \  z   ;     w             �             �             �             �             �             �             �                                       ,             A             Z             k             �             �             �             �             �             �             
                          ,           8             G      
     V      0     y              tvp7002.c tvp7002_g_dv_timings tvp7002_enum_dv_timings tvp7002_timings tvp7002_enum_mbus_code tvp7002_get_pad_format tvp7002_write tvp7002_write.cold tvp7002_s_ctrl tvp7002_remove tvp7002_remove.cold tvp7002_set_pad_format tvp7002_s_dv_timings tvp7002_probe tvp7002_ops tvp7002_probe.cold tvp7002_init_default tvp7002_ctrl_ops tvp7002_s_stream tvp7002_s_stream.cold tvp7002_query_dv tvp7002_query_dv.cold tvp7002_query_dv_timings tvp7002_log_status tvp7002_log_status.cold tvp7002_driver_init tvp7002_driver tvp7002_driver_exit tvp7002_id tvp7002_core_ops tvp7002_video_ops tvp7002_pad_ops tvp7002_parms_720P60 tvp7002_parms_1080I60 tvp7002_parms_1080I50 tvp7002_parms_720P50 tvp7002_parms_1080P60 tvp7002_parms_480P tvp7002_parms_576P __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ i2c_smbus_write_byte_data _printk msleep_interruptible v4l2_async_unregister_subdev v4l2_ctrl_handler_free memcmp __stack_chk_guard __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init i2c_smbus_read_byte_data __stack_chk_fail media_entity_pads_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std _dev_err v4l2_ctrl_handler_setup v4l2_async_register_subdev v4l2_ctrl_handler_log_status __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tvp7002_id_device_table param_ops_bool    8  A   8  g   &  �   8  �   8    8  ;  9  a  8  �  8  �    �  <  �  =    8  Q  8  r  &  ~  >  �  &  �  &  �  8    ?  %  @  G  A  \  &  m  &  r  B  �    �  :  �  ;  �  C  �  ?    D    8  K  9  �  8  �  ;  �  C  �  ;    C  6  ;  K  C  v  ;  �  C  �  &  �  &  *    K    q  8  �  ?  �  &  �  &  �  ?  �  D  �  8    ?  "    '  :  C    �        �    �    S    �    �    ?        1    X    7    <                 
  h   &                                                 $     (     ,     0     4     8   !  	   
     :     ;  <   
  A   :  [     `   :  u   
  z   :  �     �   :  �   &  �     �         =    d  E  �  F  �  &  �  G  �  =  �  
  �  H  �  
  �  :  �  I    J    
  #  :  -  ;  T  
  Y  :  f    v  
  {  :  �  
  �  :  �    �  :  �  
  �  :  �  
  �  :    
  
  :  "  
  '  :  ;  &  A  &  G    L  :  [  
  `  
  j  
  o  :  |  K  �  ?  �    �  :  �  D  1     M     h     �     �     %    n    �    �    �        F    m    �    �    �    �    �        /       8          L     M          O  @   &  L   &  \   &  d     p     t     �     �     �     �     �          8    �  &  \  &  �  &  �  &  $  &  �  &  T  &      &     L     R        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   @                    	   @       <"  0              )             �  �                   %   	   @       l$                  /                                  8                <                  4   	   @       �$  x               E      2       <  �                 X             $	  �                 T   	   @       %  �     	         g      2       �                  z             �                    v   	   @       �'                  �             �  
                  �   	   @       �'                 �                ^                  �   	   @       �'  �               �             ~  �                  �             4                    �   	   @       T(                  �      0       H                   �              [                     �             \  (                                �  0     8         	              �  �                               t(  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ELF                      X)      4     (               �����W�zV��<  ��������   ���   )�1����   )����^_Í�&    f�������B��u$WV�2��w+i��   �z�!   1���@  �^_ø����Í�&    �v ������捴&    f�������A��u�A   1�Í�&    �v �����Í�&    �v ��������A   �ȋ�<  �I�H��<  �I�H��<  ���   �H��<  ���   �P1�Í�&    �����U��WV��S�Ã�p�����   �$    ����������������1҃���[^_]Í�&    ��    �����VS�Ë@�{(	� uK��@����K|�
   ��������t	[^Í�    �K|�	   ���h����K|��u߉�[�   ^�R���f�������ȍ�&    f�������=     S�X\�M   ����������   �����1�[Ít& ��������A   �ȋ�<  �I�H��<  �I�H��<  ���   �H��<  ���   �P1�Í�&    �����UWVS���u5�ōz1���&    �v ��D  �   ���������t���Ø   ��uܸ����[^_]�f�i��   ��@  ��<  ���  ���\u�׍�    �S����\tŀ{u��u��K���'����ݍt& ������UWVS�Á�   �sXd�    ��$�   1�����  �C�P�R�����%   =   ��   �C��  �`  ������Ņ���   ��0  �@   �ھ   ǀ<  @  ������C�S��  QRR��   �CT�0h0   ��������   �����&    �v �
   ���������  1҉�������Å�x��h   �������$�   d+    u�Ĉ   ��[^_]û�������������&    �t& �����9�8  tSUWV��S�Í@p�����x$�$    �D$����   ����������  ��8  1҃���[^_]Í�&    1�Í�&    ��    �����UW�   V��S���   ���   �$��t& �
   ���������  �7   ��������Å�xۋ$�\$�   ���   ���&    �
   ���������  �8   ��������Å�xۉD$�\$�   �$�\$���   ���&    �
   ���������  �9   ��������Å�xۉD$�$���   �   ������v �
   ���������  �:   ��������Å�x��|$�ŋ\$��  ���    1ɉ��ہ�   ������%   	��D$	Ÿ@  ���&    ���   �=h  tW���   9�u�f;��   u����   f���t9�w����   9�r���&    �=     �  1ۃ���[^_]Í�&    �    ��uԄ���  ������ԍ�&    ��    �����WVS�Ӄ�d�    �T$���������u6i$�   �{�����@  ��@  ����   ���   )����   )����T$d+    u��[^_��������&    ��&    �����VS�Ísp��d�    �D$1�����l�����<  �p�pVh�   ������D$�����/  �          �  �              �                                                                                                       @   �   �     `  �     P  �    �  p  �      4%s: Write: retry ... %d
 3%s: TVP7002 write error %d
 No platform data
 6%s: Rev. %02x detected.
 3%s: TVP7002 read error %d
 7%s: Fail to set streaming
 7%s: detected timings: %d
 yes no 6%s: Streaming enabled: %s
 tvp7002 �D$�4$Sh    ������
   ������D$�D$���T$���0  R�T$Sh   ������T$���E  �@P�CpPh    ���������  ���upPVhK   ���������tVh\   �����XZ1�1Ҿ@  �~u�N��u	���  ���V����\u݅���  ��0  �   �H�@����	���� �  �Å���  ��0  �J�B���	���   	�����  �Å���  ��苵<  ��!   ��L  �M\�   ��ǅ\     ��@  �E  ������Å���  j ���   1ɺ   ��������	� �0  ��j j j jj h�   j j ��������   �ul��$��t=���������  ��h9   S�����Y[�������  ��pSUhf   ���������  ���������������Å�u���  �D$�4$�t$h    ������
   ������D$�D$���T$���=  R�T$�t$h   ������T$���=     �[  �t$h�   �����XZ�$�[  S�D$��pPhf   ��������3  US�D$�������pPh�   ��������3  S�D$��pPhf   ��������3  S�D$��pPhf   ��������3  S�D$��pPhf   ��������3  Q�D$1ۃ�pPh�   ��������3  i��   ��H  ��D  Vh�   ���������8   ��   ��   D�PVh�   ��������   ����������D$d+    u��1�[^�Vh�   �����XZ������7%s: Removing tvp7002 adapteron address 0x%x
  6%s %d-%04x: tvp7002 found @ 0x%02x (%s)
  6%s: Unknown revision detected.
   7%s: detection failed: lpf = %x, cpl = %x
 6%s: Selected DV Timings: %ux%u
   6%s: Detected DV Timings: None
    6%s: Detected DV Timings: %ux%u
 ������    �    ������    �����                            tvp7002                                                                    �               `       �           �                          @                                                                         P      p                              �                                                      `             �         �l    n   (   �                           �                                                              �         �� �     �  8        �l    X   ,   �                        �                                                              �          e� �     �  8        �l      ,   �                        �                                                                         e� �        �         �l    �  (   �                           �                                                              �         �� �     �  8          ��    X   ,   �         $                  �                                                              @         eZ f     �  �          ���       >   <   	                        �                                                                      ����    �  @          ���       @   D         '                  �                                                            �         q����                        {��@GABKCDE F-G 1Z2  \�             g �@GABKCDE F-G 1Z2   \�             � �@AB�CDEFG1Z2  \�             ���@AB�CDEFG1Z2  \�             ���@AB�CDEFG1Z2  \�             6 @�A BC D E F-G 1\�             5�@�A BC DEFG1\�              � g ��2 ` 	 
 ��� .]G  � G  gw  !" #� $� %� &�'� (S)*+ ,P- .�/�01Z23`4�56 7� 8� 9� :� ;� <� =>`?@A,BC,DE FG H I J�KLOMN�OP�Q�R STUU�VxW�X�Y�Z [\� debug parm=debug:Debug level (0-2) parmtype=debug:bool license=GPL author=Santiago Nunez-Corrales <santiago.nunez@ridgerun.com> description=TI TVP7002 Video and Graphics Digitizer driver  X          ��      GCC: (GNU) 11.2.0           GNU  �       �                                  ��                                                 7         @   G     8   @  (    H   �   &     _   �   I                                	 v     C     �       Q    	 �   `  g                   �   �  ,                �   Q       	 �      I     �   P  �     �   �        @           l   �   	 "  @      7  0       H    c     Y    w    	 o  �  �    �  �  �    	 �  p  r     �  �  P     �  3  y    	               �           �      �     �      
           0                        0     -  �   P     ?  `   @     O  �  3     d  �  3     z     3     �  �  3     �  @  3     �     3     �  �  3     �           �                       X       /  1        F  =   =     \  z   ;     w             �             �             �             �             �             �             �                                       ,             A             Z             k             �             �             �             �             �             �             
                          ,           8             G      
     V      0     y              tvp7002.c tvp7002_g_dv_timings tvp7002_enum_dv_timings tvp7002_timings tvp7002_enum_mbus_code tvp7002_get_pad_format tvp7002_write tvp7002_write.cold tvp7002_s_ctrl tvp7002_remove tvp7002_remove.cold tvp7002_set_pad_format tvp7002_s_dv_timings tvp7002_probe tvp7002_ops tvp7002_probe.cold tvp7002_init_default tvp7002_ctrl_ops tvp7002_s_stream tvp7002_s_stream.cold tvp7002_query_dv tvp7002_query_dv.cold tvp7002_query_dv_timings tvp7002_log_status tvp7002_log_status.cold tvp7002_driver_init tvp7002_driver tvp7002_driver_exit tvp7002_id tvp7002_core_ops tvp7002_video_ops tvp7002_pad_ops tvp7002_parms_720P60 tvp7002_parms_1080I60 tvp7002_parms_1080I50 tvp7002_parms_720P50 tvp7002_parms_1080P60 tvp7002_parms_480P tvp7002_parms_576P __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ i2c_smbus_write_byte_data _printk msleep_interruptible v4l2_async_unregister_subdev v4l2_ctrl_handler_free memcmp __stack_chk_guard __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init i2c_smbus_read_byte_data __stack_chk_fail media_entity_pads_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std _dev_err v4l2_ctrl_handler_setup v4l2_async_register_subdev v4l2_ctrl_handler_log_status __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tvp7002_id_device_table param_ops_bool    8  A   8  g   &  �   8  �   8    8  ;  9  a  8  �  8  �    �  <  �  =    8  Q  8  r  &  ~  >  �  &  �  &  �  8    ?  %  @  G  A  \  &  m  &  r  B  �    �  :  �  ;  �  C  �  ?    D    8  K  9  �  8  �  ;  �  C  �  ;    C  6  ;  K  C  v  ;  �  C  �  &  �  &  *    K    q  8  �  ?  �  &  �  &  �  ?  �  D  �  8    ?  "    '  :  C    �        �    �    S    �    �    ?        1    X    7    <                 
  h   &                                                 $     (     ,     0     4     8   !  	   
     :     ;  <   
  A   :  [     `   :  u   
  z   :  �     �   :  �   &  �     �         =    d  E  �  F  �  &  �  G  �  =  �  
  �  H  �  
  �  :  �  I    J    
  #  :  -  ;  T  
  Y  :  f    v  
  {  :  �  
  �  :  �    �  :  �  
  �  :  �  
  �  :    
  
  :  "  
  '  :  ;  &  A  &  G    L  :  [  
  `  
  j  
  o  :  |  K  �  ?  �    �  :  �  D  1     M     h     �     �     %    n    �    �    �        F    m    �    �    �    �    �        /       8          L     M          O  @   &  L   &  \   &  d     p     t     �     �     �     �     �          8    �  &  \  &  �  &  �  &  $  &  �  &  T  &      &     L     R        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   @                    	   @       <"  0              )             �  �                   %   	   @       l$                  /                                  8                <                  4   	   @       �$  x               E      2       <  �                 X             $	  �                 T   	   @       %  �     	         g      2       �                  z             �                    v   	   @       �'                  �             �  
                  �   	   @       �'                 �                ^                  �   	   @       �'  �               �             ~  �                  �             4                    �   	   @       T(                  �      0       H                   �              [                     �             \  (                                �  0     8         	              �  �                               t(  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  cmd_drivers/media/i2c/tvp7002.o := gcc -Wp,-MMD,drivers/media/i2c/.tvp7002.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"tvp7002"' -DKBUILD_MODNAME='"tvp7002"' -D__KBUILD_MODNAME=kmod_tvp7002 -c -o drivers/media/i2c/tvp7002.o drivers/media/i2c/tvp7002.c 

source_drivers/media/i2c/tvp7002.o := drivers/media/i2c/tvp7002.c

deps_drivers/media/i2c/tvp7002.o := \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/MEDIA_CONTROLLER) \
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
  include/linux/delay.h \
  include/linux/math.h \
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
  include/linux/compiler_types.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/vdso/bits.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
    $(wildcard include/config/SMP) \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/linux/sched.h \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/PREEMPT_RT) \
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
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/IOMMU_SVA) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/DEBUG_MUTEXES) \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/COMPAT) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/UPROBES) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/MMU) \
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
    $(wildcard include/config/PREEMPTION) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/kern_levels.h \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/ratelimit_types.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/th