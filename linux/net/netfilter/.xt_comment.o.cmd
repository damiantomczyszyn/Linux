) \
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
U��v���%�6 �+ǘ�)�q����9��-���\�����>�BG1M���l�	h��*����;aM�)�m���G�paN�?��w �YC�mX�����0(�c�yN��R�<%��.~�,��G~��2bJ��
ok�'�κY������^��S�^���,�����w!��׋��yU�v��֛���Ŧ��r�؇}���l�wC�����n�p�������F+��J����*ΐl64��~dA�H����]a�o/i(�r�}����;��|aQ��"���: �C<`�(n	�c��B!����/��	�@n���E�"Ì� ��R�(Ʊvnk�����X�hXlۮ����
7��|���c�<��g��,9j���gߓQ=*�E������� I �u��nSo�ˤ�@*�V� *�4<�Z<s����L~1��8/��^d�UH{~4�G)��?�`��۴&{�����3��"�=��yUZ:�#�;?�w�Dɥ`�*�SiV\�/����=Z�!竼�r�F����w���$9q�{x�u��l�Y��AN|l����6�S�q�V������f�/���4w�v|�uGo��$��W��tkh�B#=vT3{dW�����0�S#����9��$ݼ �)������TC�>��u�;f�G"���0*N�	lU�2�	�:�������, ��b��]B���`���'D�~��M.�Ʊ� ��䎙D�n�@{�0��kIi(X!�=�'���@�03��,�=�M�%v��hY����RrȲ���j�4���^M7�Q9���O���i�K�^>.�( C���	�*�'7B<8̳o���A8�o-9�)��..�7i:DT$��9~8�҆\L�d4@������2��s�u G��ć��8��f��9Mvw�L�Jq	5�����@�;��~��W���C��IR;`_?U����>������{��uPqVB����c�P ����x{�CdFs��ݤ8�����R�(�j\�ǅ��șP&fO��1�hˊ#cНVW-q�ЬG�#�86)��� ���_����ՈO0����<�+��ғU� j6��=�Qu��R�xԦR䃨d�SH����u��_�E�0�4�`Y����rX��Bci����iTI� t1▄�ў\J�q5�
�:C�lTt>��V�>�d�&3��*` �3�"�n��drC���_���o�k���t����Y�@�c�;�$-G ʛ��XW��	t��-�io�rmt^`����ׇ���`g��h�xVzz[]#�+���}cU��%�o����}�pCq�b'M�gh�F� �V�chSW�n���6�*q��6�w�� ݸ6d�Oe'��c�E[m�>b@EIq��ߓu��]C���%�_�;�&��4��6�,��]m��w�F���#F��Q|r	�ɲ7�BB_�3�d��KU����MǟU� ��a�6CJ���/W��Ύ؍������ܼ���7ٵ ��ܨ�l<���-�W�2k��'���j�n3Ʒ�Lg�#�*���X�6ݽWoΝ#N�O��7��L��hc�W������D�����(����R�x�V⽉b|NIǌ���)�u���Π�����=,(��@ˋ�\P=�j��UKYz��^4�-�X����I��{�T?�}���k�����[4��z�3�ܴ�$dO%�%ȁB�<ʪ�!���{���K�u�/X汋�Z@�Vv���)8���ZuQ�˳/����5�>7�
t#��r@ �ِ�.�����cf��!��qn�r(�6�7�N��N���-YV��V�o;SB�"�����q���)h��Ȟ����JV��r[��J�ix�T�R5 �TtNN2�iI��˯�h���d
�4bI�qgf `R9��@�N^�N[�T�T'�Bwm��N�g<Vw��c�جl�/ x
��a�_�x������5��9�)�Y3��57��=Ұڡ�s�RT��?�{�C�sjH6�8����@������K3�3v������d��r��%`��ap�FN*�k`�ΒÆh�3���F��e���[�P��t0'�	�I�<շuz �]���Sd���X&S�����&�K�Yظx��F ����>��ᄢ����Y%�L�8�`�`�Z���T��	l�d��|�\��fh��{D)]FP��E'F��P�,�@�7��Ȃ�����B��G3T��V�"*U��o��e�BJ�m�	�8q4�ۜ4���S���lD��@�t"D�n����+P��?��N�R��%��D�m�)��&[�T��d�y����OG437:T�=cp�:=]޶���*�M[�d;��g^�c�?ᵬ��%�&���Ǳ�l~HJN-�#/(�J�v�]�Nْ�"N�E,�|A{����O��^!2݂+p��������+�zo��@0%l��K5����Q�Y"q��_Flx6����E�\_�����;
t9/��.��{l�������aM�"Ӿ��됾}����:`lǔ�$�i����`��/!�^�7��?����W�]��WE8��� 񟃔�n�u:�3�3k~F�/r?7߇yJ��P�>}�ا�7H�^�[����S��k �?��W����x,?
��4�;B���T�ZH�}�+�8kjۋ��I��_T߷��O��xꐛ��p�9�1�zWo�j�'zPC���)y3�lf�k	L���Aóߜ�r_G�7o��k�>�B&2j2ݚK�{�L�B@�� �ԚuQ:5ő���x�o=M"�=�2�pa�<�0	.�Y�-��fGi���i(9v�s-I;]�>v��Vu<ޠ�=#������~/�^���S�u����<�q*�M�>��?��(�U�Q^����̎{�+���[�e�[4/'�kɇ6I�[��~�����hQ�!|%��߅¢�c�9y7�?��`�9-���{��"q���	��fǈ�O�$P1<ᡎ�4���O�>Y@�y<l �A�1;C	@�h���Y"P�*=�RL���`�!d���q'��x$�içAJma��0�c����i��D����u/���×:BfڱMG���%s5lkϷ{4����# �_���Z�MWl��f:��� �Pո�fuĎ/� õM>����%C�6sv�̈���"�vJ:�c�D@���#wfz��3�D��^Ʃ��E���+� ��cU?�}��۔���5m�/e�mo	�n��*��HJ���|�!��CS�W~�������T01��~m���3ܼm��lث�ڞZ����%�J��n��jn�~E�i����ۂ�p��^.M���j���V3iq�j��i��R�� �%~@w�s4��F�Å?�{e�!�y%2�����0&�6��Z��=�"I��5���V��bY���x��Aǽ��~��}R�[6��"���箄pj�����M��%�6�Y[���t��w�T|�,�Ѽ��H�c�R���:�3��S�ss�Gn�{f�f!�P�#��bI[�NH�YQ��P��Ec���@?�م5���FuKjO�N>T���&0C���G�����ߦE*q9{V0��˿Ps�)�?� ��4�ӌ^ս�{����WC*=�)L�^�}�$�am(��tu�8��D�,���yi���]�.��1�Ht���EX����龛�mFx�3�
.��s*bm
/�Iћ6y���%ZC��a�|�IFrt�$�;�I&��B.0ك�S� f x���i��p��C��H�l��=�j�2-J] ���5�L��g^0y�n��v͚D���`+�@���MU���S�CW�>�Do�)ݬ���?,ȁ?6�����n�(��=�mm�kDǮʖ�{ֺ�IRQ) << LOCK_USAGE_READ_MASK;

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
	       struct lock_trace **const trace)
{
	struct lock_list *entry;
	enum bfs_result ret;

	if (!hlock_class(prev)->key || !hlock_class(next)->key) {
		/*
		 * The warning statements below may trigger a use-after-free
		 * of the class name. It is better to trigger a use-after free
		 * and to have the class name most of the time instead of not
		 * having the class name available.
		 */
		WARN_ONCE(!debug_locks_silent && !hlock_class(prev)->key,
			  "Detected use-after-free of lock class %px/%s\n",
			  hlock_class(prev),
			  hlock_class(prev)->name);
		WARN_ONCE(!debug_locks_silent && !hlock_class(next)->key,
			  "Detected use-after-free of lock class %px/%s\n",
			  hlock_class(next),
			  hlock_class(next)->name);
		return 2;
	}

	/*
	 * Prove that the new <prev> -> <next> dependency would not
	 * create a circular dependency in the graph. (We do this by
	 * a breadth-first search into the graph starting at <next>,
	 * and check whether we can reach <prev>.)
	 *
	 * The search is limited by the size of the circular queue (i.e.,
	 * MAX_CIRCULAR_QUEUE_SIZE) which keeps track of a breadth of nodes
	 * in the graph whose neighbours are to be checked.
	 */
	ret = check_noncircular(next, prev, trace);
	if (unlikely(bfs_error(ret) || ret == BFS_RMATCH))
		return 0;

	if (!check_irq_usage(curr, prev, next))
		return 0;

	/*
	 * Is the <prev> -> <next> dependency already present?
	 *
	 * (this may occur even though this is a new chain: consider
	 *  e.g. the L1 -> L2 -> L3 -> L4 and the L5 -> L1 -> L2 -> L3
	 *  chains - the second one will be new, but L1 already has
	 *  L2 added to its dependency list, due to the first chain.)
	 */
	list_for_each_entry(entry, &hlock_class(prev)->locks_after, entry) {
		if (entry->class == hlock_class(next)) {
			if (distance == 1)
				entry->distance = 1;
			entry->dep |= calc_dep(prev, next);

			/*
			 * Also, update the reverse dependency in @next's
			 * ->locks_before list.
			 *
			 *  Here we reuse @entry as the cursor, which is fine
			 *  because we won't go to the next iteration of the
			 *  outer loop:
			 *
			 *  For normal cases, we return in the inner loop.
			 *
			 *  If we fail to return, we have inconsistency, i.e.
			 *  <prev>::locks_after contains <next> while
			 *  <next>::locks_before doesn't contain <prev>. In
			 *  that case, we return after the inner and indicate
			 *  something is wrong.
			 */
			list_for_each_entry(entry, &hlock_class(next)->locks_before, entry) {
				if (entry->class == hlock_class(prev)) {
					if (distance == 1)
						entry->distance = 1;
					entry->dep |= calc_depb(prev, next);
					return 1;
				}
			}

			/* <prev> is not found in <next>::locks_before */
			return 0;
		}
	}

	/*
	 * Is the <prev> -> <next> link redundant?
	 */
	ret = check_redundant(prev, next);
	if (bfs_error(ret))
		return 0;
	else if (ret == BFS_RMATCH)
		return 2;

	if (!*trace) {
		*trace = save_trace();
		if (!*trace)
			return 0;
	}

	/*
	 * Ok, all validations passed, add the new lock
	 * to the previous lock's dependency list:
	 */
	ret = add_lock_to_list(hlock_class(next), hlock_class(prev),
			       &hlock_class(prev)->locks_after,
			       next->acquire_ip, distance,
			       calc_dep(prev, next),
			       *trace);

	if (!ret)
		return 0;

	ret = add_lock_to_list(hlock_class(prev), hlock_class(next),
			       &hlock_class(next)->locks_before,
			       next->acquire_ip, distance,
			       calc_depb(prev, next),
			       *trace);
	if (!ret)
		return 0;

	return 2;
}

/*
 * Add the dependency to all directly-previous locks that are 'relevant'.
 * The ones that are relevant are (in increasing distance from curr):
 * all consecutive trylock entries and the final non-trylock entry - or
 * the end of this context's lock-chain - whichever comes first.
 */
static int
check_prevs_add(struct task_struct *curr, struct held_lock *next)
{
	struct lock_trace *trace = NULL;
	int depth = curr->lockdep_depth;
	struct held_lock *hlock;

	/*
	 * Debugging checks.
	 *
	 * Depth must not be zero for a non-head lock:
	 */
	if (!depth)
		goto out_bug;
	/*
	 * At least two relevant locks must exist for this
	 * to be a head:
	 */
	if (curr->held_locks[depth].irq_context !=
			curr->held_locks[depth-1].irq_context)
		goto out_bug;

	for (;;) {
		u16 distance = curr->lockdep_depth - depth + 1;
		hlock = curr->held_locks + depth - 1;

		if (hlock->check) {
			int ret = check_prev_add(curr, hlock, next, distance, &trace);
			if (!ret)
				return 0;

			/*
			 * Stop after the first non-trylock entry,
			 * as non-trylock entries have added their
			 * own direct dependencies already, so this
			 * lock is connected to them indirectly:
			 */
			if (!hlock->trylock)
				break;
		}

		depth--;
		/*
		 * End of lock-stack?
		 */
		if (!depth)
			break;
		/*
		 * Stop the search if we cross into another context:
		 */
		if (curr->held_locks[depth].irq_context !=
				curr->held_locks[depth-1].irq_context)
			break;
	}
	return 1;
out_bug:
	if (!debug_locks_off_graph_unlock())
		return 0;

	/*
	 * Clearly we all shouldn't be here, but since we made it we
	 * can reliable say we messed up our state. See the above two
	 * gotos for reasons why we could possibly end up here.
	 */
	WARN_ON(1);

	return 0;
}

struct lock_chain lock_chains[MAX_LOCKDEP_CHAINS];
static DECLARE_BITMAP(lock_chains_in_use, MAX_LOCKDEP_CHAINS);
static u16 chain_hlocks[MAX_LOCKDEP_CHAIN_HLOCKS];
unsigned long nr_zapped_lock_chains;
unsigned int nr_free_chain_hlocks;	/* Free chain_hlocks in buckets */
unsigned int nr_lost_chain_hlocks;	/* Lost chain_hlocks */
unsigned int nr_large_chain_blocks;	/* size > MAX_CHAIN_BUCKETS */

/*
 * The first 2 chain_hlocks entries in the chain block in the bucket
 * list contains the following meta data:
 *
 *   entry[0]:
 *     Bit    15 - always set to 1 (it is not a class index)
 *     Bits 0-14 - upper 15 bits of the next block index
 *   entry[1]    - lower 16 bits of next block index
 *
 * A next block index of all 1 bits means it is the end of the list.
 *
 * On the unsized bucket (bucket-0), the 3rd and 4th entries contain
 * the chain block size:
 *
 *   entry[2] - upper 16 bits of the chain block size
 *   entry[3] - lower 16 bits of the chain block size
 */
#define MAX_CHAIN_BUCKETS	16
#define CHAIN_BLK_FLAG		(1U << 15)
#define CHAIN_BLK_LIST_END	0xFFFFU

static int chain_block_buckets[MAX_CHAIN_BUCKETS];

static inline int size_to_bucket(int size)
{
	if (size > MAX_CHAIN_BUCKETS)
		return 0;

	return size - 1;
}

/*
 * Iterate all the chain blocks in a bucket.
 */
#define for_each_chain_block(bucket, prev, curr)		\
	for ((prev) = -1, (curr) = chain_block_buckets[bucket];	\
	     (curr) >= 0;					\
	     (prev) = (curr), (curr) = chain_block_next(curr))

/*
 * next block or -1
 */
static inline int chain_block_next(int offset)
{
	int next = chain_hlocks[offset];

	WARN_ON_ONCE(!(next & CHAIN_BLK_FLAG));

	if (next == CHAIN_BLK_LIST_END)
		return -1;

	next &= ~CHAIN_BLK_FLAG;
	next <<= 16;
	next |= chain_hlocks[offset + 1];

	return next;
}

/*
 * bucket-0 only
 */
static inline int chain_block_size(int offset)
{
	return (chain_hlocks[offset + 2] << 16) | chain_hlocks[offset + 3];
}

static inline void init_chain_block(int offset, int next, int bucket, int size)
{
	chain_hlocks[offset] = (next >> 16) | CHAIN_BLK_FLAG;
	chain_hlocks[offset + 1] = (u16)next;

	if (size && !bucket) {
		chain_hlocks[offset + 2] = size >> 16;
		chain_hlocks[offset + 3] = (u16)size;
	}
}

static inline void add_chain_block(int offset, int size)
{
	int bucket = size_to_bucket(size);
	int next = chain_block_buckets[bucket];
	int prev, curr;

	if (unlikely(size < 2)) {
		/*
		 * We can't store single entries on the freelist. Leak them.
		 *
		 * One possible way out would be to uniquely mark them, other
		 * than with CHAIN_BLK_FLAG, such that we can recover them when
		 * the block before it is re-added.
		 */
		if (size)
			nr_lost_chain_hlocks++;
		return;
	}

	nr_free_chain_hlocks += size;
	if (!bucket) {
		nr_large_chain_blocks++;

		/*
		 * Variable sized, sort large to small.
		 */
		for_each_chain_block(0, prev, curr) {
			if (size >= chain_block_size(curr))
				break;
		}
		init_chain_block(offset, curr, 0, size);
		if (prev < 0)
			chain_block_buckets[0] = offset;
		else
			init_chain_block(prev, offset, 0, 0);
		return;
	}
	/*
	 * Fixed size, add to head.
	 */
	init_chain_block(offset, next, bucket, size);
	chain_block_buckets[bucket] = offset;
}

/*
 * Only the first block in the list can be deleted.
 *
 * For the variable size bucket[0], the first block (the largest one) is
 * returned, broken up and put back into the pool. So if a chain block of
 * length > MAX_CHAIN_BUCKETS is ever used and zapped, it will just be
 * queued up after the primordial chain block and never be used until the
 * hlock entries in the primordial chain block is almost used up. That
 * causes fragmentation and reduce allocation efficiency. That can be
 * monitored by looking at the "large chain blocks" number in lockdep_stats.
 */
static inline void del_chain_block(int bucket, int size, int next)
{
	nr_free_chain_hlocks -= size;
	chain_block_buckets[bucket] = next;

	if (!bucket)
		nr_large_chain_blocks--;
}

static void init_chain_block_buckets(void)
{
	int i;

	for (i = 0; i < MAX_CHAIN_BUCKETS; i++)
		chain_block_buckets[i] = -1;

	add_chain_block(0, ARRAY_SIZE(chain_hlocks));
}

/*
 * Return offset of a chain block of the right size or -1 if not found.
 *
 * Fairly simple worst-fit allocator with the addition of a number of size
 * specific free lists.
 */
static int alloc_chain_hlocks(int req)
{
	int bucket, curr, size;

	/*
	 * We rely on the MSB to act as an escape bit to denote freelist
	 * pointers. Make sure this bit isn't set in 'normal' class_idx usage.
	 */
	BUILD_BUG_ON((MAX_LOCKDEP_KEYS-1) & CHAIN_BLK_FLAG);

	init_data_structures_once();

	if (nr_free_chain_hlocks < req)
		return -1;

	/*
	 * We require a minimum of 2 (u16) entries to encode a freelist
	 * 'pointer'.
	 */
	req = max(req, 2);
	bucket = size_to_bucket(req);
	curr = chain_block_buckets[bucket];

	if (bucket) {
		if (curr >= 0) {
			del_chain_block(bucket, req, chain_block_next(curr));
			return curr;
		}
		/* Try bucket 0 */
		curr = chain_block_buckets[0];
	}

	/*
	 * The variable sized freelist is sorted by size; the first entry is
	 * the largest. Use it if it fits.
	 */
	if (curr >= 0) {
		size = chain_block_size(curr);
		if (likely(size >= req)) {
			del_chain_block(0, size, chain_block_next(curr));
			add_chain_block(curr + req, size - req);
			return curr;
		}
	}

	/*
	 * Last resort, split a block in a larger sized bucket.
	 */
	for (size = MAX_CHAIN_BUCKETS; size > req; size--) {
		bucket = size_to_bucket(size);
		curr = chain_block_buckets[bucket];
		if (curr < 0)
			continue;

		del_chain_block(bucket, size, 