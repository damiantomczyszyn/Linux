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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     free cgroup_mutex mutex_lock mutex_unlock system_wq queue_work_on seq_printf __stack_chk_guard bpf_stats_enabled_key __x86_indirect_thunk_ecx sched_clock __x86_indirect_thunk_esi debug_smp_processor_id __per_cpu_offset __stack_chk_fail bpf_sysctl_set_new_value memcpy memset bpf_sysctl_get_current_value bpf_sysctl_get_new_value migrate_disable __rcu_read_lock __rcu_read_unlock migrate_enable strscpy strnlen fortify_panic bpf_sysctl_get_name bpf_ktime_get_coarse_ns_proto bpf_strtol_proto bpf_get_current_cgroup_id_proto bpf_get_current_uid_gid_proto bpf_base_func_proto bpf_strtoul_proto bpf_event_output_data_proto bpf_get_local_storage_proto __kmalloc css_next_descendant_pre percpu_ref_is_zero bpf_prog_put __x86_indirect_thunk_ebx bpf_prog_array_alloc bpf_prog_array_free static_key_slow_dec __x86_indirect_thunk_edx bpf_tcp_sock_proto bpf_sk_setsockopt_proto bpf_sk_storage_delete_proto bpf_sk_getsockopt_proto bpf_sk_storage_get_proto bpf_cgroup_storage_unlink bpf_cgroup_storage_free percpu_ref_exit cgroup_storage_lookup bpf_cgroup_storage_link kmalloc_caches kmem_cache_alloc_trace static_key_slow_inc bpf_cgroup_storage_alloc cgroup_bpf_offline percpu_ref_kill_and_confirm cgroup_bpf_inherit percpu_ref_init cgroup_bpf_prog_attach cgroup_get_from_fd bpf_prog_get_type_dev cgroup_bpf_prog_detach cgroup_bpf_link_attach bpf_link_init bpf_link_prime bpf_link_settle bpf_link_cleanup cgroup_bpf_prog_query bpf_prog_array_length bpf_prog_array_copy_to_user __cgroup_bpf_check_dev_permission __cgroup_bpf_run_filter_sysctl __kmalloc_track_caller __x86_indirect_thunk_edi __cgroup_bpf_run_filter_setsockopt __check_object_size _copy_from_user lock_sock_nested release_sock __cgroup_bpf_run_filter_getsockopt __get_user_4 __put_user_4 __cgroup_bpf_run_filter_getsockopt_kern cg_sockopt_prog_ops cg_sockopt_verifier_ops cg_sysctl_prog_ops cg_sysctl_verifier_ops cg_dev_verifier_ops cg_dev_prog_ops      F     G  !   F  *   G  A   F  ñ  F    J  1  F  Q  F  a  F  q  F  {  L  ƒ  M  ›  L     N  Ñ  F  ê  O  ñ      F    L    M  +  L  0  N  :    @  Q  m  R    T  ¦  R  Á  U  Ò  V  Ü  W  ã  X  ø  U  ¡  Y  ±  F  	  [  A  F  j  [  Š  \  §  \  Á  F  ñ  F  	  \  a	  F  x	  R  
  _  
  `  %
  G  m
  G  x
  a  }
  b  ´
  R  ÿ
  _    `    G  g  G  r  a  w  b  ³  Y  Á  F  í  c  	      d  7    <  c    F  —  R  ¼  c  Ï  R    Y  !  F  ‘  F  !  F  !  F  :  :  V  g  b  h  z  :  ‰  :  Ÿ  i  ©  j  Á  l  Ñ  :  á  :  ñ  :    m    n  !  F  b  o  ¡  F  °  L  ¸  M  æ  :  N  p  v  p  ‹  q  ¶  L  »  N  1  p  F  r  q  F  ‡  R  ¡  _  ¦  `  ¯  G  â  T    G    a    b  -  R  A  U  R  V  \  W  c  X  x  U  ¢  Y  ±  F  À  R  Ş  _  ã  `  ì  G  "  T  N  G  Y  a  ^  b  m  R    U  ’  V  œ  W  £  X  ¸  U  â  Y  ñ  F  
  R  p  _  u  `  ~  G  ´  T  å  G  ğ  a  õ  b    R  !  U  4  s  >  W  E  X  Z  U  Ÿ  Y  ±  F  Â  i  ß  :  ñ  j    :    m  !  n  1  F  Ë  t  q  F  ‚  p  —  q  ½  p  Ò  p  ï  p     q    u  *  p  E  p  W  u  j  p    F  ¥  :  º  K  Ó  r  Ü  @  á  v    F    L    M  F  L  K  N  a  `  ‚  L  «  w  Á  F  á  F    F    x  3  :  J  y  a  z  u  {  —  :  ­  i  »  j  Ñ  |  á  :  ñ  m    n    F    @  /  L  4  M  l  K  s  v  œ  r  ¸  `  Õ  a  í  u  ü  @  #  }  ,  ~  >  L  C  N  V  `  h  a  {    ª  T  ·  `  ô  w  !  w  1  F  M  R  Ô  :  ¹!  €  ô!  R  ‰"  r  "    ¬"    ¶"  ‚  Å"  ƒ  *#  @  /#  „  E#  …  ‹#  ~  ”#  ~  Ü#  ~  å#  ~  $  K   $  ~  )$  ~  =$  Y  Q$  F  q$  `  €$  a  ±$  F  ¼$    Ê$  R  ë$  ‰  %  `  %  a  v%  u  †%  `  ˜%  a  ¬%    ·%  R  ñ%  u  /&  w  9&  Y  A&  F  U&  ‹  t&  L  &  M  ˜&  L  &  N  À&  Œ  Ñ&  L  Ü&  M  õ&  L  ú&  N  '  r  '  `  "'  a  Q'  `  `'  a  7¼ÄÜg0idÀ?½ÕvJù¬‰E(Yí.¬èA”¸q¼ğd_(ñõD4:Fİ¤DçY¾©z¸ÎßÁ3ÅÕ ÇSêï‚»©ÙY*t—oé2…)lï.şJa6ÙQxè» !£°«­)R.qCN¼×]•t¯ïc8-KÅ&ì¸oÇs™ë-a¢†}6H/“ïx¨9º,¼ËÃõÈÉ­k››‰ûœ_oxÚ,w PU:üå$uùÍD™“‹Éq5-°œ4ÚLšŞ§Éè(Ïàájj#Í[ïÿÓÍãƒH]½ƒ~8Ü§E‹´+Qö+†"JƒY—p2r¸S²R¤½ZÕ[™¼^…rşò½/ˆüÏt Ê+]\!õ‹øV²¤jFö¶W~\èfäDõÜ|~`8Äİè¦“¤ òc}Ò¡h‡‰Fñ½ ÚP2Pˆğ¹¹ŞCÍø·Ÿâ«­¥6uCšşî®gwÔ2“Æ‹ñ%¬í6T\{ç§,ş¼wfícw˜]ÌšqÈ»ƒ“e½¥RmÖsôæ¯„x€–Ì»ÕşÆçÛÃQË’U‘òZRR RíÓ¸‘úífEuz´¸ˆ¯æ*Ù˜Åã—ß2Şqf]m9e-(”İk™ÌÜ!h1áã¼Ö›9~U{•)0W©³¶»/Å=5ÒŸ1¤mï‹õ1ÜÂYËuLñvıÀê™e•z×†—ï‚x$ßúvß‹(çNoƒÏ	ÖèxmÀCò3G¢è97lKqßÀÍĞHMåÔ9g“k!®À[%CµégÔ–XdCY#I¸›1YÑö5OeDTÚaK‹ÇŞµÛTpûKß®£åO[›ç]“'üg9DÁÏ@à@W"cd“”í¬¨NÅR7á¾x›İXÍ)H¨å…ÜÂ'€·–ip/
Í•;…tO)®v0%¼ş?9* ıİlîğIXJ&ö¶–aĞ©KÃĞËQZ@ÓæÜ›h<_\XàøÆÿXH¾òúÄ¨ÓÉ¶à•U­ÖØ‡wñI›ó9O?  Ÿ#‘äØ¾ûÔ&_3‹C°ş(	ÌŞ¯yÉ§İ MN#)P(ÙOÔêª¸ÀõN™œfqœ†‹¿i/]&°]ãaŒ©g†WÂßitI¼Ñ=©L¡¦½1¶ísWh
æ%†{ÀLÿşR¸Ù-OCÿ
´®¶˜Q<vIèÛº/A½ÖœpŞlïµŒ•Ù/)seŞ?P­PßÑpw2³gå-’·&Œ´³Wì8fOıä:ìGc‹›ÉŒÕšÀciL÷Â±h¦ÖS¡·¬ÿ‡3G7^{ãPxãù&ıĞşvßiŒRHG—œ”F•==4i+E{ƒ˜²rÂS:~]ÎN7oõ¨ÿÑŒs¾Ót©jè×qÛ”î].=aˆ<GÎZàşê<yO2Q±¢“ÜiÇD{Vï´Vdp·bO§7îÄ²~¿¾{™•y‹òh‰ÆÍMPèGåÂù§rÍ]>£‚Ó5o¶h‡UÄ¶h÷àƒçwwxd-œ#XØx‰ÓX®Ö¤²_ßm˜b'±'¸,T°X¨ãÏ12O¯[ôkQ¦ë’EL¥ )ÜcRŠıù¤¾
Œ0\CŞÈ”—SMwÀçLUÁÇmDÆóÉàY9L‰‹ju…‚Àé5Iˆ­÷I£%Uö$v¢Â©"ı,ç,GÇ™´áí~ _ÄyœÉas.«˜ Ô”Âİ8e¬¤Op¶AµÑİ£@\èqjÿ†+eš\Ë«hm¥ò)Ü²ê¼æ¡Şè=›d¾†?]5ôxœ¤Œ3™¹G›¤Høë„ßDM{ãÉ::	=7lˆØu½˜u¡òĞt0¦ü‘aí{†‘á ¯çŸDª ¥¡ç×ŸÈœº¥0g6•ú»Õö(ËÔüol
Ë8Çd¯ —ğÍWC¯Ğı·…0ÌùëÍ­`V¾ìrØyä7‹ú°¾}œĞ«@2¼Ø©Ã3ä´?ÏZÒ3á	òìØD(0wú°å÷?¶ÀÿuAšwƒn,‡ı²Ü)–Z+k£óŠïz–ÖüøÃ.­¬ÆÜŠNo2„7:¹ƒ:<ÿîõ¿‹Á²·ªNÏé&XÉ<˜Xc(^F{Â}…èF-
7èkN‚Ë~7=(eywÃŸ“¡€š)‡†å1ÏüàÜÜKŠËC0…¼Ò¼ëFÒÉÿf`¡{ú4ŒØ±í[D6¬ÁT[¦u}/œÌg€'Ò–8Ì¡Ñ¬€(õn”óŸT[Ÿ¬'óÚ~H&)zc0w]Îï¿moÏ ò~p)s.#µÜ'-™ö„GùÜ™eU¢~¸œäKõPYC;k{?`ì7ÛÕp£áò—|¨RçÓJş6Bêz[°Ú0tí
`ü£İZO\/¥Ä4¦ˆhÀàeĞâˆæiÚÈ=&èŞ­ª‚µ`×Â_óÔˆeB?¾„ÀdÑ{}]Ûm¤³yºªP·ÃKcGöJ>ª›M®	a·,¾Â1üÂC9¦ ª‡‰„G‡Rëùi?Ãª_QÊRLA†‰ç·Ÿ’{ğN_u2äõÄ²¼í2}1©ğòc:FÈ
[Éô.x¬áË´Sß®%'Oó³èP•@qğ…†åÄM¬Â×–ûÉ0F\»™_´D–£ ºJ}ô¾5iv³˜=ïğ>¤ïDñ2İyí¾|òÄ®Bô¸îô¢)7K”€S«RO¼LĞ÷Â5nD‰•¬ci´¨ À şÑèÈ[ÒºŸ\-=F×d—ºá`Ù¾¨¬YÃıÒlé„¼‡Ê®I2š£ä²¯Õ¥gmš –›"4WÕ_CYZd€ÔO‚Ú¦¤¯5ªÄª±Š÷ßwH—ÂS½òÙ7Fq´ÀĞÚ>ïïåŸ6 ÷7)!{ÌF}(†.
.c+äYò¦È·=|–‘hn°’w¸Ïz­¡‚nY6údğßÌÌü[-ğ ö&Äpk_ƒ\¦¬”ECÈÕÃ/UóOÈÒ"úôÂhƒ0Ú#üûSˆ>T‰äÕ6_zı*#çÌNŒ*†—ÂĞ…¾b)JßTcqV­»b*Ã› åj“ç‘Õ„ä1tBÃrÓŸ	xUl#W¾²ó´ònĞ>‰™K·Gˆ,Bd¿^ğ¤$¾¯‚Â?¸A»Æ¨º6¬Qû€<fl[_È¼ƒpk«^83Nôq×köÌvAÔÿÔı(å*òsÉ÷òò“5.ßƒş/Ng4ÅÈ–&²°¿ª¯nÁ]·bpBÉÊì’ÕGyÑ_¿‚dk8²†ú_d¯õ_UÅYN·(iêá&ÑÕ3¤Âªrwë<Î"m$ÀÜã#ı¬<£lÿå¿Œ¤FÇ"ºaª6éˆ»AÖ˜ë¯¸\0ò’@‘×§Ç…tML\ò…!×©y«'\à
¿ev†%ï9}Ãí8x€µ•†ªBo—–ÍåŒ½]Ñ!“çJŞà
enö2+|'"úÒD(lş_­\rŸe3ˆeX}.¥ÿ,f¥#©‘7°6‰³IfY‚»LÒÔ=gŒ€°æRëQé2Ææ˜Bœ«%Ô7¶l{ ¯5C÷V:¼Õ((R¬í3BevÔĞv%,Fa±¹m½PB0g˜ÁW^ûœúûGOD4¹ÏL`?ı¨Œ´ûşÖ>'ëŠ›u™üoVI|½&$A—q=«§HzüÛ~ºUEğ­ ‹E¢»‚×Dšè“Õ8¾vFÀ”d§+9ô>(OB"÷YdÕµDfyµQ¢§³e"N3×5 `YĞ—’2z‘"råòw­PœŸüõ1âöt¥)‰vÆã{K3²·Ù«
BŸî&P4z–éjPápŒ‹Äsælwûò®sÿOÊË¨HU“o)Ÿ>&²hm¸“¡ÿ©fû—r"£t,-%$pŸÁ|SƒÎù
#â_üLYõûÓU.ØÉ[ãéÛÿ®„î¦Ÿİvæ§©şó"³xb|óœÎó@ğ÷ğÔÊ+ò†ö3Y&K^]?‰5ü8š]âò8^(K0¢p`tÊ¥â<öj8-imÈ²˜Mç¿Ì£é…ê‰XÊÈòÖyŸ1×{)kIÛZYxFF>)Ÿ eé¥=€¬z}A9,_T~Bï»Ğ5Iá€N¹K¼ı`üâ;%Ë¿ÕŸë5‹ßì'1P+R·qaq÷A9EW
G6‰ı»¥û~	¡C™TÕÃ=Kïeÿ~ß‘u]ÇÇ?‰$£åi£ÉÄà×5ğ4ìä‹»¹óù*¦ùµ´phÌ¯Œ9ËÉüˆA~Mš…†Ş”ÁF¤'õ5?ÂjSuFVÏbÒ(EX%ş%†åJğ„õî‰ÍµàQ‰îIÕïW¾ø¶ÿm ‚g¾}ešR!;¬õrgü‰QIËÈ¶¹(s×Ğã_Á*HdNjà5ÄÇ,XéWÌ‚SbóŸªæ¡º|tFÒ‡L_†;/f7£/Ù¯Eå„Œ`€!Qè«‰ğáğä®®xg]~³ )Î~=˜­[—É§Ÿ!,—Gj÷ºµ¥dc”ÛÖ(Ïšğï’Ïå:|V<Éÿw«àPø[Èßœ©*ıÿ1hm7%Ú¼5áVBnO¨Áp&ß# $T`ñçÂWßŒg¯/
-¼‘h>ÁımÇ‰‘ÖŠ¢)íE‹UŠ#¬ŞAm¢„Ş•6ºæÑ#Y%š
|#’Cû ìÙñå—6¦}-Ç´ûNÿ<-o›Cå€qr&¢ıPÑòVVíqGV8ó)aÁø,ÛÀ›[,z•bŞZÈïoOGö(ÆŠia}Äg]\ÆwuÌ‰¾
œ`ıœ·Nì§ö”9¦@Ylµ£º?î·wE÷×\ôöoi2}—…ran-Ò&äÃJ áQ]Õ	Ş¾#·ç2İx@~z~Ç‹,·ç ¿°Äy½ÁÍ,ÉâÑT0úŸâºN˜µR¨P~‡â˜PêEßä
Ææƒ˜ònF‰ŠDlÎ!{°9ò"é÷ı/i™ÆhI¸k~Jõeıä‡ÂdÔ†g!HÔÛĞ¦“RóğíœY0¬¥#şÌÈÌ-Èş÷
Y¯(s=¥´Z¡ñTî}òCs‹f§œº‰DñTMœê ÉfC÷)M<§7»£Õ ^9ÜØ¯±oU‘ô plîŞ¿Â{öôšá›ğ†^…¼%ˆ>:*—$÷V¸=ÂÇU¹/uV[-–†ÎŒ•=©X¸Dƒ5Z0Û¸Tí•Ê˜z×	cyN,I"cÓåk­	ê;€æÚÁÊeµMò(@ÏD˜Ø‰ß›İ•/ÜÊ¯š+¦“°$òëEøÇÓ*.]âãRîÏ2R«çQØA²	7—Ø÷Á`ñEC*jèB!"5Œ7h­ ÿ†f¸³ruúáokx¢¸‚­‹CQ˜Şşr»¬k}Ó4bRÚ¢‘È©øO*vø©ætÜR´\æİÃ7	5ğ_e9ÒÎ¯@†v×‚$m¡‡¶ö7·ÎŸ‘—oÁtØ]êº	Rum¨!DŞñF9 q>‚Ø©ÀB…÷W£ZiO âDU{Œ Ÿ%­7¿¨T]‡-`¼Êd0¤uëè.A…c§á B®HÛÊ`šS	Ğ‚3YiØ¨o¾­ÔºmYé??âÒ½«O*0µıªú¢#¹_ø˜ãã¦ı«ütBûÂb wÀ£ïãÚ šŒÆädËÙBU!š`FiåhÍ…Ùİ.Ô‘Lcµ¤qñ——÷)©â°nšÑ.¤‹Á[î–‡ÁT?Zm¯P·1Ñ]‹Š:I1ÛbAÈ¶‰ADPƒz—úšŠ\¦¢~Öz–¬Ğ³z?ÀV¨kğ¸Ø>Š™½~;m3n¤Øåä½.c!!Ÿqão9Š÷œ”LQûöÈu¨V—+ZÇcY\²½Ø(ƒ½éqÄŒ¢ãs»X„I]doü½¢Y	r*ua#¡Ö\æm÷­ÄKÈMÌ—æ$k0/ƒÆá;ÍI‰ëGd%üÏ×ËûXŞü!ÃPªz¾ô¹Ô¸ŠB·d¤˜B¦ÿ¶dş)g	{§£‘±	+áé2#Ùîâ‹õ,¾‡Àå¿ÕküQB@:¢_N­	[{”æNN'+ÕÄÒ~&D=ÂQ5Oı°Tûÿ	|Ÿ(‡Õçµ$Mmm»â:vB~yı•ÿÜæ¾$¾«aDa¤PØt{Vê®Q0——aí‘WÄj¸EÜ3U'käÇ÷˜}©ë‹x qbÛ˜)s!:gœÜ‘e ?Ç•—8áğ£´)š´eÓE§Õ£|†]³z‘C7—{Ê=Ş™ˆezäk]½í<W†İ§“cäádxV´wÈ‚XÎn_ÆıW±AÚÉ´•+K·Ä¾¥Côè¦â½¾íœAüOÊ3GÙµôB!WA6 ¾Y‘êöÌ EœáN5ÃT}Ùşà^í»Áf!ë¡z²¨¶;nà·Èğå‹ñÄ¤SŸğÇÈˆ˜Ç–%‹"›Sí+	‰|ïLTÚQi_Vú”Mó9åÈ{YáoúQ(Ê8wLí“‡V*H·D$-# %UÌ8HlB0&Åk#g;}FpCÇ“N}®EŠf…ÉÑáíËˆÖß€Šãká*yË@$ÿ¤ÇĞÓMctÑN–‹5‹®Ö«miù•ß®Z'	Ïµbÿ—ÕDk7UA3"*üSÕúœÜ“?m8d²¯‘µKü/»ùùÂ <->ÕÏöÓıyRª¦X±Ğ1G—ã©§e|íÎs$BOÌê7
Ê¦â¶ÜöF˜L…úÉ£ÅóC]­q‘“À–[èß)İÄL¤Ê¹ñGÀı*cœ+[D¦’ûŞ°…ó+0?1SZ¥gHXÆşúÃßâh8[B#B¯Ÿ˜ÊU!iÿi”!Ïù xòò,šZĞË=X„Mm0H¥÷k×PÚVá“ğÈÒµ=)‰Ö2ô¡sÔÿ{«£yéîŠÓuN‘ŸÆ}@²fåg "İ¦ŠM­´¾8ßQ0{t°=¨Y¨U8×=sn^ïl³O³ñª³2ö*à„Ò ƒüx<Buu;hQmŒ¹ßYÙ!P²xB‹ÃøÜüã4á%r7]º¢^W!î<IA—YĞuÉIAäõ¶r¨Ã§
Ço#¹aMi¼£20v€²Ù–CÁË[ı•ÇtàÖéğÒêG8¡uú^
æäÇÜ–şY;­ø¤¢Ì²†e'òr‡$—†õÜc?g’APªŒ¡’CŸ~¬#ñÃ±—Õû¬ë¸2f(ÓÕbı®Y ät7BªL§]09şÇlÚ¶‹¹×,r+ßş×=²=ŒÅĞj§j}Âıè¡h±vêEw¸^Hj©ÔÎ–ûâPvŸ¯£w$‘úçb–éœçÖl•+Ôıia~jŞˆY.¥¡È©™›PÎ§ïÒè±/xW¨*”3BãSgd²l:ÉTBÖáü!FƒVşê_è±‘™:»·WWx«,ĞæráÍ¨ğóİ}È9¢¨fïˆ•è5Šğ2œÆZÛê‰eğ–¡b]¹q#‡-ŒhßÏ]iÔ±àüÅZö®½Htaàkdï*­ëk®KŠó¾Vî‚C¯lØÿlã%9½ ’m%·Q‚ozŒ3a]×~+/+(\t›&ûú`#¶”÷ÌK-ÓŞeÂÅ÷KèĞàçÑ—ì¹¶!ílæ¸9GŞÕë—Â­&£r–Çæ”Ìàì+/œNİ®rƒ†·27BâÀ¼}1½Ë©5Û˜¹M—r'ªØÛC¼o_EVˆŒÇ$…>?g‹3¼D`Í;õ|§_İò$šM·eeXySmÖbÍ†å‡òËhxêÏgPËØ¾‡–ËÅòdmYqHhÅõuú6{êö)/şû½ëË¬Â†A‚‚ù¦Y%}ß&´¾<?ù¶€P`ÖWÃÏ/ËI³aèÄè!=˜4)rbóÓe·thzCÿQ•9˜ØH47÷Ëw/I1“ì‹…wïyÒÑâ½ï#~ñ`¡ÑJ\^B@Q%JI6ÿjxæDÈ4gbáEÚçî¶Í§Y2I¹J„ˆzçÊsƒ+õlí0Ó9äñ
àß”zË•e–Ü÷¿”Ä©ÆIjÇÙé«@Ki-9ö’PYN¨İY†0N†ßƒ¥‰Aˆa> Å.i›BCMà¡ã5l”ÎÓâ´"
ìyÃ“8Ü¦Õ“’“jËS
áf½;œğƒoÔRôOÏ^¸n?–[,)ƒ¼{¾ƒüÄ[3SÅÔ<?†«´ğƒí (aßâÏY7ä>ş
ÿj<Ğ 1©—åqç½‰o\.PêÌ<„W2zŠ\¡	geªhï¦løƒàÿ·«µ®ü#Ê&¥/û#úx\ûòàs˜FàOnÁh"ıúå¸¬%„ô‡Elš€Ÿ?ñ9kÇyi;5ÿ„¤¥>s3ÿ æzÄİ¥üZ·J‘jq“è¦Å=nÕCÚ¥øò³Qz·O¹ñ›ê+€ÃÃ2sğtÒTøóHÖìÙ^m-±å6Ô’Âµ¡=Š² °Œ<Z$’qí{7Ò¼•ÿZ¢é¯æE>»T1Hùı{y¿{¥™(ÔEô!#Ã
DK…ŒÌGµD÷6]°NùÑN[¹ãPÍğÓ¼4#c¶x~8“Üîö]\F›{Ï‹=4“k	ñSıÎJzEI’†X}ªpfüî?şêÑâ^í’O Ú;	
Jg$…´ï‹¯Õ ùx± ×òŞ´€PÁ'Ü{¼.“>”}ª9¬ñÚúZ5ë1ON>ó¬&†Ê"·.â'6y;d´w[à0ğÉrj¿ªóÎ.-vï)¼‰Q_ ÷’­‹ˆñau¬ã @Ğì˜£z'ì¬ –b6U ¸>$3è4±÷>£,¦Ö,.ƒ‘í;0 O/Nx¸JÀzñXIi‡­„íŒ;)ïÒ|Ë©Q3—ëÜ”fûìgÃÅlF‚/¤2X­tÕ;{(|»MÜ¶Ö=üE.C5áQ²şá¤^èÓ_IeP§§ig‹Ô`ê8RÜµe¢¹£e§md¨œĞ•R"€Ìl,^Ö'—¸”}[¢³YÕpæá­Ka£·FîİÅ
4×µd$U/ãWÇÛøwÂÊä¶b¦4Æïhå½ƒˆLWô¥$»qtÔqÓåªW`©ùN 2»Œ–«0Y3ğÌq¤¸î©ıÓfUÓ#IÕ‡zYvZ‘ïRÛøu9M³œŠÅÆé#úåÛØ¶¿6®(ä¥ºGñÛ#I9p6f_<UÜzÌú¾1ËÅÚGHªûİz“O´„2V@jÙÒÂàNÔ
.Ë5\ÅÙÕf÷wñ8C”—sŒôG\Ûø±•“ 1¬*?.ö’C2Å<FŸ=‹¦[,T¼k\j½‚ˆÍc™Œp ~ã/KŠ°ö‚M[¾4¨ë©­…ìã	¶Ì‹ópBÅ¡k\ÀŠ’LŸîâ«!vÁº ¸¶J(0ò9Î÷'Jómn—k-‘…Dk'ÌëV¡+–¨şP­qT//ıYÓóÑLZCv¸¹1¼q˜Ï.ü(0…ädïpÁJù¶9úˆÏ8µünoİ[»iÅa¦!â%µôNşˆDHyJ"BáÛ’øxpñ<ègŠVê­=ì¾pÕ&`qòp/ş Ñ×®ŒJÚg]A=
– ¤õöh=®…ˆYÚ˜…Sç·sB››[øÇæ1uÓs ×ƒ»qäG ¾ÁØy2¥930ù|-ã¹‡òæ­í{z;KNAq$Ğ=`*™ nú¦~f’8¬5÷b'N¦ç©¯ç ÿ†]*Í^[c/5Ç—ö‹™*óì‹_ğŒ{ú+¬Ç÷œí,´äÔÒ‘à”6§FÏç<Ç-¤¨Z¤®Œ² Ò€¥Ö«Ìµ£\°ÖÑCûwâè5”tª<).¿á£!Ş†¸geÀåŠ.Ğ£WVPıî…¥„´m§[‚¦­`zŸà$Å—ÍNH…›¼”#Š\®íæô;ÒŠİŒÃÒ“ÖÏKËÿÏœGŒS[œw9˜Á`[Â$A×ÜX‘S$®©5Ôª½êµ )%ËÓK)Òp©HKÍUˆë(ÿIïiŸ>mN0ÀŞ}óıÕLÍ”i™Åwó÷Q—~½»â©¦ò¼¨vô7—ÓÆ.!=!yè@0ÿß¯WÇÕ# Ÿ)l†ÂNöŞj3j¶®Š¹ŸõÅÆ²Ör7	Û8ÔRøù¸Ú¢‚³ éGµéŞM$× ±]ŒAnº„®=Ãàô§ğlÿTÆWyGˆ<‚H¤~¨Bç:ßJ"8Ï›öO™WóKï)èæáôH[è0°Ù,‚	íéy%å¢·FÄ¸* àï›¸VY;}äƒÉ]Päaİ{)(P¾ÍpL:Ji'l™@[;-Áw• juÌ.¦àdæ0_Œ@9=±«šHŞÜlj£T‘eêŞŠÔ@GIµÁ×cR ²öf»ó¿k)†¯ğàØğ*ãÀG1r—qğh`û°)2„ñLß‚*ªÇ3q·ÔvèT2¦ZM6B·pRÄåÀê¤OŒ†½¢7¾ß>;½æƒ‰m=İˆ"!¬ĞO¯ßÊ³Ë’§pÙÆÓ‰#½iÌ¸äGÜ@nLĞü:"·caÓ×.×O8£´]!oR9C=3Ó €h T¸U¶»³ÇÆÏ61³JÉÑ+³Ï°j*|¬…
;!˜A¨ÚF|(gz=0ÿpÌõÖ2¾ÚÿÃèçµ¶ˆbáœ¥ÎÓ@nÙutr;vèú •´PÍoµÃèÀËyÂË
8m›_é‘%ÃÏs/Ó:ûÔR h {ØîßÖ :7«§ìB}m½ç”Ù¦¢<Û$fÎmF—õu`cI5W†¸¬cLÅ–{áSb3şcà78Äñ^ \ÔNÄ˜\ç'Hb©õØŠ/>“§@ĞfÜŞÁ<¾¹–%‡–÷8ï.MCk`eO/“n$5šÑÌvÜ£%œFOÚr\ªXçv’.ıGU%æyê…Àxè •Û0üøJµĞDÃÚq¾q™gÂë£t€V³õâøj­ö`oí\Û¬Û¯àÒJ’éUÁ "äÙ0^†÷Y©##×]eE’Óä2<ğœ°i?ÍµPqtR%õŒTèÜÑbÃ„ˆ»Œì^÷“]ÄÓó:Á~•Jİ‹Œı
}Dëö5è‚ç¬ôÔ4
ÔqS*•ôzY‹¯è1‡ö‹¹?¦+;$)œ‚œ ¶ÙTTg7c.*Ëİ"UÒ„$âĞY%¤¤JÕ¥¶L,[Î˜ø¦ Ğba…¤iM]ı…«Çu;¦b,jÁOæVc~¤KbÃ­ÿ‘3ÙvÜÂryYw)k-Ê¸†û^Ò3fGQ1·Êæ­=`¯!îv—1†UVİEâ—:¦l9G)©Ë/Ç¡?€=<»×¾V¨>”	k;W¶û*ÍÚÄº[LVN´VAs’yĞåÆDtØ½ªS¯+#¬@’Oc¢»³ò)²>›È×$&Ü™cü-:‹M… `¢;?`ørJ!Jt°Õÿ8«tîÖQÓ×J¹ÄÒEUıZİ<ÌûT€®°aõb%Ò¸¨uåÖV)ÃŞ˜tŞÒÍ-Ã›'¶mL@´ã’JP@‡–Lªèñ\R0!‡"1#h…Eö~YY2dÿi·7£2ñÃ\©uÉÜv’#«IãDìî,3İ3·ôMN…s—2ñ^á±f&iÓR¨ïÄ¬~]Q¢CÒG=#ÿ
%Q›t–»û™ZÀ ‹¬f{ ]1,²7* J8’ĞùÅ“jßÈ™Ñ„jG<¼:ªuşEm8X|X7ú|~¥ó€áÈø>Yˆ§‹ßü€SBrõR…rµ'Ö”KLÇI„jä¾’ÖÍøÚWqÍE¬Ã•¤0¸$î.]D|º®kÙqlÁèş¢ĞÆv"×ËÓ×5´èøX<ïl˜2@dtù€@m„²R±îù­_÷q‘Í9°£`#Î±µ5»tß“µ(c~D7ïN?_?ù]Ò=4Ø/Íş±ù˜÷÷I®4^+6FûïÎ‡àîÙ åş¯<D)…Ü€Ênšz	¸Ñ3ä=‹‚J¿òl|ÊíR&Õt›ú-D…ÎmãpV…;Úì;u©ÿ*[Ggú&ÄÿiÍ5¯o YkÓr(¡¿¤ªáµo‡¸HMÉqPO°¦áIä¤¼—–ÿØ2çÖ §añ:ç£oës€H¶oŞœ-ÍqK¥ÄÕÑ›P6 „&FÂäez·Ê×êªÜ¹ğÖóŠÆ÷ä
R{äü±:èìdöÅZı8˜‰nV0)¦-­Ú8Ä3&ïÒ·©à(ñ‡	ã¿Ğî¶ğŞ¥¯E+£¢ÿfZ’É¾LESfÄØ`†(ºlˆ§SñÀX·ë´şĞé5/üÁ,fü6„Aü+´¡€Ş½¸°v›Õiék†6J°£GÒû °#6·(Ù9„¡g'8›—âÏG>wÂù†òÒÂÂç´;1”É›.]…Q}ülå,N¹1¶ Ñ#(Àa¼ë„Ò61œß"THÁé±»RªÏ™paÌ¾-ÙSÜŸ@8'ßD^öNça*•µ)ïEb+q*²ÂXbÕİ6–ôŞ\4ß¹ïÃ™Å\šĞ¼›˜½§åö±!«Wß.é‰üiv{E³¥KÓw‹£ÚÈÛ…ÛŸv1H‡Ä¬W•¶5k+µæ·h7íğKúr;ÜÃ¥è¦~¬»Z×DÇ HHvb$ÇÀa…ı”ÍNò[@ØíÔ{¾åO¨Çn
ëòrQÊp“¨1y×öLçd zŒé›h5ˆÃº§şÃbJ]ŠËr9gşó·1H–ŞĞ©½§ªÏ†ÖíƒÒ8CËOu<±ö	ÔÚÄ;ØQRÄ”@¸ıwsœŸòŠ¶b ·¡]úÙ‰‰ë•ÖÔÔ¸g` ínC_Öıˆ°Å)Ä‹/õGâ¶ı!‘«uxş{,„ÌğKùF&SaFlíÖ\úC*0'NÕ,’¿G¿ÃçV½Áe/í2Ødúù]úBz'çúó©5pœn9t†ª?Û‚òš'fİ1Ú¹|Ñµò¡¨`Š6ãşÀ~ˆ¦Ò°O8a0pïÖš´¡çSk”’ìTúhNÚ„æ0nÏÉ¨(P¶è‰İªŞ<"RõT2x{%Tw¦~Û4V¶8ßWõÖfp–$ÏCµ/,’î›xAÊRû 3ÁÒŸ@ª-ÔqÒzÚÅCÔò©ta¾ò´™,IûF¦„äÇ\½K'@ÿ3+ªçÖß)_¸‰Ö^áÛ=6	o(æá›Íø8j"õIh°V,n´u÷Ó<¸¹bó#h>Ì?ö…;ÍîÓÖúò¼{€ÎVÙû”£VäX¡œ«£Â¨×<ÃÎ$||Uj]ó(U5°X-M”²¦î’»vëMõ÷ğç¯Æ™c›c–¤aç4¦óiyŸô­”€°°™˜2İ©T6!ğåè´ğIE;+No§²$jˆkÏ•6]BqY-D‡®«»ºÿ—*W‰…Ëøb¯r_i.Éhü/EâĞ±0†·¢^ŒºZUspçÜşºt"p}G[Ã>m_çoı¬”a¨
 —ªcö‹HÁîÜl4Šæ‡ÕiÌ8Lù N&Í~0õúóO’+ã¨¨s€Ñ–¡Ü¾Ù›ØĞTTqâp–ˆó…l™»ZJÁõB'ü†·Æô˜$·±t´.Qj:Á¿PÚ3G4®(üìu:]Æt›ëq©vPaJÓ ­S:J¬£×ü¸Ì§iK¼òùÉ f¤ÕºBZµNp§6ÉöûYmûÃ‰ç¦!å?ä¼Bç—¢ƒèN²d!;;SÂÜ
æ[ˆ8(|Hc÷=Ç³…ºeŠ¯ï÷•’¤.ÃcÓ>}Å^FkµÉ³i+Ìœa#­ï‚Ùˆ>e¡y—oKhœ?’G§s²¾ÈS‘I3_‡8C‚ëåÇËGÆRöşSÚZ¢&Ê›óS\1Ês»ü&‡ú•ÄãÆ&YW+†Å
½>Xù¥:aAÇ|5xÓ²%à¦ŸXÔJnûş'ÅÏ¼NÕqsäĞèh&I/ã†tTH2ßÆõ&&G½VƒNocæ2’ÌiÒª1ˆyŸy˜_/µ)í2ø¹›z1Ô`ÈEÁ$L·S'Ó¯Û7b:¿äØÊ¥»4Îà„(Ê§€°ß™mBd·ÁÄÒ8X`‰|´š°-–Bs‚¡Úeâ„HEô´@–;¤Êâ%¦NO+É¨j¹u„ZiTÈá—|çûœk@fÍïkê6?’õ¬b—ƒ)êĞ¼é,P±¾nŸ[Öz-Dİ5ª€WÒ#Ó™‹¨LoÄ–BqIéô™š1¥ˆÖ½¿<\Ê+VRæyaGeg9Œ	»<©÷‘PßêäEÎ%Ğ¶€E¡¤+a` @¼}f¯
‡ñ8}é^~ÿC•İhf­)Â)`ñ\<wÜH™àfÓr»+›&q²ğ:F ï&giùŸ0è=UWØ¦º?i¤H‚R?Š³¿óÍ*4Õ¸O!;âŠ€8.AICb<~jœÄÇó½ó~3õ$F>],2Êä­9ñ]3AR´}7Œ{N÷ú¶¬<«]‘ê$?†%yâ\p—'ÄÊÚjĞåÆªÿÈ©Oz’ö©NŠj˜®™ĞÁïM•ÅVì²ªu*ïB"a.îñ1¾1W4È6Y–ú¬ìw¥XÕ©ñâİçM\Cšu;<rœŠ¯y¹­o>Dmó±Ké¸d¿#˜ÂË[¹¾:ß:]wı}„C3Š+/‚2È!Æ?iË ±ĞâèÌÿû2{m`Ó
—šßğ””k*ÍÒ£l©˜#;­´í/W)Ôğ9Z™‰QŠSLœ¡IcQ@:†²ê(½Ä°e£,±HÜzwãøb3X«FèjğTT‡ı¤l*yl@uØ)f˜mûßï¼•ïÿµUGÌ½Òl¨.dÊ”,ÀÃtc/kyBnÏğHB‰Û¹y–‚ô¨¤ß×v]4vö¡‚§dqŠ~×¨5›‰c¹ôu÷S÷ˆşMñ¯†É2²wË¸ò¯âÙ¦x%&E¾hú5¦œyAtÆ<Ôñ„U›4mäÌzÙ0T`Zgy¥°û4>{¼"v 
î?­wKx%÷€Î,1E*X~òı—’svxğè^R’}å¨qÚî
JçóŒğ…X¨½n"\]îÃ-´&³Š[Çß©ßûXis.‹h@ND¦™Ï1°;éœ­º–ëˆ~ÊMßù‘Å+ú¯_ /|’Ïçêx\I	Ú³ÿÕ^=æù«»¨KãîÌó®–3ÛR‡]´lj:0:£l-şÎÅ÷@kV)_(mÑæMe6–øºêÀâ>–º¹\xíòhøöÉVà3N—ı·1ÍÖL€"Í_Œğ}æÏú÷°;¡,I[$m •l—İÄsù·aª¡m'“÷ê[Ö¬m¯I«|(
ÁÕ‡A¶ô©:Š†®l%I¢	°éê6<èû]cyiĞD×<ïİ!}ÄfÈïq0¸ÈŠ«<èÎ}Ê€\¾ÃİàÌr´¾tğêrÕØiKÕÜ\%{ø]$qÇS©òs·ÿÉ¬dB[ÒÛŞqsæd›·¨;…BÂšÊ¹96ÄøQ‘@v(Š.·Úœ‹Úò~yzÕS¢"ÜI×·úf¦IÓı7O{¦W%IltÔXàc†‡ŒÆ2·ç¡@ùÊ%KG>‰o/díÆ,Ä‚çÆ—†füDX/´ƒÜeZh‘ÍOñİÃzTJù‚Z4?C@_*eOTÍÉ1‚i«ùED$¸‹/½¸¯hañ»C ØÀñ°¦qØqOjÜ›¥|•}`…aWd Å¾ì§”bÚgŠ,çÖ×]Àè³ñ¤½y>ô‚yeÈ°j^eõŸ¬¾W¿~½0RâBÒV1Éğòu”%ACôëùìåüÎ·ÿ²œ\j8”ÒS>Z…YHÕ’ğô /ŒâecÌåcÏÓÈš¢#\;æ†ßs5+»§å¹›÷„E#7VgŸœwdWİ›g¾Î ˜cq´šÕiÒØ+2ÜêÍ
U©gP&“íQgÊÙz¾ï
U¸€v‚ù¦%ü6 ¾+Ç˜¨)¹qêì„ÏÅ9ùß-®’Ù\äŒù¡×À>øBG1MËú¯l’	hÌø*™ıÀ;aM¶)ùm¢–G€paN°?ŠÙw ûYCè‰mXÓãÒÓÉ0(›c³yNŞïR<%ùø.~Â,ƒ‡G~è÷2bJ¯‰
ok¯'ÙÎºY÷š¡¶à¥Ü^Ïñ©Sñ^­·,—¥‚´şw!»Á×‹ÆËyUv‚¸Ö›ƒÚÉÅ¦†”rÙØ‡}Š—«låwCì±ŞÓëÙnép‡®ÙÅÿF+ĞÚJ¾•‹è*Îl64ÈÃ~dAûH º±Â]a¦o/i(úr}€Œ¨á;»‰|aQŠ"¦ú¡: æC<`í²(n	æcÿîB!—©”Ş/ãÉ	ë@nù…¤Eõ"ÃŒ¯ ‡ R§(Æ±vnk…Œ—ğâX¾hXlÛ®ı¨¨Î
7¤ş|¬±¸c<÷¨g•,9jÚãógß“Q=*ÎEÚâüˆÃªÛ I ºu“½nÂ€SoıË¤»@*Vì *Œ4<ê§Z<sëÎùÅL~1±«8/Š©^déUH{~4¶G)Ãö?ê³`Ÿ°Û´&{¢™øáÑ3î´"Ç=‘±yUZ:ó#Ô;?ÙwÛDÉ¥`”*îSiV\ë/«‹°¹=Z÷!ç«¼¤rçF«†øĞw‚¤İ$9q {x†uÃÄlÍYàAN|lû™¶6ŠSŸqîV¾Ôğ¹ú½˜fĞ/ğäÒ4w×v|ùuGo†®$¤ÅWØñ¸tkhÉB#=vT3{dW¨™şùå0¯S#¤ÌÚØ9®ê$İ¼ ş)šíÅÌ÷ØTCí›>ŞúuÕ;f¾G"ü×0*NÎ	lUÕ2õ	´:¼Æñÿ°°ó, ëbØé]B¹Ğî`³‹'Dã~¶íM.ëÆ±‰ õœä™DØnÒ@{Ä0ËùkIi(X!’=š'Àı¤@ş03ÈÕ,Ø=»Mç%vÅhY¦íìïRrÈ²ñÀÊjî†¥‰4ö’^M7‰Q9¬¨úOëíöiÂK°^>.¦( Cú­	ò*÷'7B<8Ì³o§¾±A8ßo-9Ğ)Øô..ò7i:DT$‡£9~8Ò†\Lêd4@ÔûÕÆı€2«ë—sæu G©áÄ‡­‰8À¸fÂà9MvwÆL·Jq	5”ŠŠ½Í@ŞÍ¾”š~§ñWø¼â€CöáIR;`_?Uüˆ”÷>áó¨‚•’¶í{ïåuPqVBš¥¾æcî¬P û¨É—x{òCdFsŸİ¤8øö»®R·(¾j\ğÇ…À‹È™P&fO¶Ô1êhËŠ#cĞVW-q‡Ğ¬G#›86)ğùŸ ª¸_¦àÑæÕˆO0¦”†À<…+‘Ò“U– j6şš=¼QuÚÖÂ…RÀxÔ¦Räƒ¨dãSH»€¸êuê…Ø_ËEï‰0î4§`YÕĞñÓrXƒµBci¾ÖËÂiTI§ t1â–„ñÑ\JÜq5ù
¾:CälTt>ÒõVÛ>”d‰&3Ğ×*` Ü3Ï"ÉnŒÚdrCãû_Èöóo¨k‹œôtš™ÍØYø@ñcÈ;Œ$-G Ê›·¯XW‰ª	t³ş-ëio×rmt^`¿âë¾÷×‡×ğÀ`gÚôh‰xVzz[]#’+÷ˆ}cU®…%Ço’ƒ³}ÄpCq•b'MÜgh€Fİ ŠVâchSWšnƒş²6˜*qÖ6ªw€ú İ¸6dOe'«¿c²E[mÅ>b@EIqÇĞß“u©õ]C•ü˜%ş_¶;æ&Êÿ4Åß6´,¼¿]mÔúwÚF›Şé#F”îQ|r	ßÉ²7ÃBB_•3ëd‡ÌKU³÷¹àMÇŸUû ¨İa«6CJŒü›/Wæò”‰ÎØŸ•²‡Ü¼£ø•7Ùµ “¤Ü¨€l<‰Çô-éWš2kùŒ'¤ÊæjÃn3Æ·ôLgÜ#›*’ĞÛXÒ6İ½WoÎ#NœO­´7ÖäLÛÏhc¬W«–àœ’âDŒÑæÂ’¿ş(Á°×âRıxÉVâ½‰b|NIÇŒÀ”)Ûuµ¢¤Î ĞíêÛñ=,(»·@Ë‹Œ\P=»j“§UKYz­Í^4³-äƒX®üë¨ÛI‹í¸{¯T?}Œí©ük½“ƒî‰[4øözµ3ÙÜ´¬$dO%%ÈB˜<Êª…!ğî´¶¾³{Ÿ‰“KÀuÇ/Xæ±‹ÈZ@ıVv»”)8µ¦İZuQ§Ë³/’Ì5È>7Ò
t#Òã¢r@ ¯Ù§.¢©è‡âÜcfÀÄ!½éqnµr(®6Ñ7¢N±â›NÖä’-YV±ÁVo;SB›"…–±·q³™„)h˜ıÈâä×é·JV…ár[ùÓJºixìµT¢R5 ä·TtNN2ó iI‚áË¯Æhğ×ûd
›4bI·qgf `R9îù@‡N^N[ÿT×T'ıBwmÃèNg<Vwû³c¥Ø¬læ/ x
ş­aÚ_´xµ£”¶İÙ5µß9ø)Y3¿é57šÒ=Ò°Ú¡«sÚRT¦ì?û{îC¶sjH68§«‚@…¡Ş½ÆÓK3¯3v½ø‘÷•dâ€rœí%`¶Œap†FN*ùk`ºÎ’Ã†hÅ3ù­‰F„Ùeıë‡ş[¼P¾ï˜t0'¢	ãIÚ<Õ·uz ]ã‚‹Sd»‹’X&SŠöØÃ&”KèYØ¸xãıF Š‹ëÄ>ÍÔá„¢—¡èçY%àLõ8Å`’`áZÙÏØTš«	l¾dñ£| \ŠÙfhˆ„{D)]FP×ÕE'F¥ãPÙ,Î@±7öôÈ‚¨ÁÍéBäÛG3Tà™V˜"*U¼oöôe¸BJ£m­	Ù8q4“Ûœ4˜¾‚SÙ½°lDıÅ@‚t"D¢nÄøÓØ+P€‡?¯éN†R¼‚%Şî±D’m·)¥&[±TÎâdòy¡•î×OG437:Tï=cp…:=]Ş¶Õóïº*ìM[æd;³šg^Çc§?áµ¬ˆ§%›&¸ıÇ±äl~HJN-ê#/(ïJívó]ñNÙ’‹"NµE,´|A{±˜õˆOû‹^!2İ‚+pŒ§¢²ÜÜû+¤zoíÃ@0%l­ŸK5åò˜À…QÑY"q§î_Flx6­ÿ¬ÍE˜\_ÆÇ×ËÑ;
t9/•ò.ĞÚ{lôú¡Ûæï‘êaMË"Ó¾Šßë¾}ü„ûï:`lÇ”ï$¬i³ÀÿÙ`ùÁ/!ò^½7µØ?çñÁÏWÍ]¯ßWE8ËÍÉ ñŸƒ”‚n­u:È3Ö3k~F½/r?7ß‡yJùäPù>}¸Ø§¹7Hå…^ş[÷˜ÉSı×k ‡?ÏÂWà£x,?
†‹4Ç;Bá‘–TÈZHñ}ù+Ö8kjÛ‹¦ï€I´_Tß·üîOŠxê›¨ûp…9¥1ëzWoõj‚'zPC•Á¯)y3ølfÇk	LŒô“AÃ³ßœàr_G7o÷ k¶>…B&2j2İšKÆ{¡L B@å’ ÔšuQ:5Å‘Ûãxºo=M" =‘2ƒpa§<ï0	.´YÉ-Îğ¯¿fGi¨¯ßi(9vªs-I;]Æ>vÛàVu<Ş ø=#³ÉôÏÜ~/í†^ª£†SçuñûİÔ<ò³q*¿MÔ>ìÏ?ô(èUıQ^ƒ·İ·Ì{â+‡ù·[×e·[4/'ükÉ‡6Iíª[ĞÆ~Æ•ÎhQë!|%±Üß…Â¢¥cø9y7š?ô¨`9-İïã·{†"qô£²	 ßfÇˆO±$P1<á¡ù4ñ´óàOİ>Y@y<l åA³1;C	@Àhöû¼Y"P‹*=¤RLØëˆ`à!dõ¡‰q'Şãx$ƒiÃ§AJmaÿÃ0œc‡ÔÖiùD†’¶u/¦¨Ã—:BfÚ±MGîç¼%s5lkÏ·{4…ú€ù# Ÿ_ª Z¡MWl‘Ûf:³£ñµ ¢PÕ¸¨fuÄ/‚ ÃµM>ˆ©äè%C§6sv¤Ìˆø³‰"ìvJ:˜c†D@œÖÅ#wfzÅ×3µD±ç^Æ©“öE™€+Ë ™ã‘cU?¦}÷ Û”ïÿÇ5m¶/eÀmo	ºn­*ÚğHJ®¢İ|Î!à„CS­W~˜³›âá±T01Àù~mû¾3Ü¼m—èlØ«ÂÚZ•øõ%»J÷òn°újnÂ~Eşiºœ¯ÉÛ‚ÀpÃÏ^.Møò­¶j£V3iq¸j‡»i¡‹R¬ë ü%~@wÖs4Á³F¿Ã…?á{eé!‡y%2ÁŒüŸ0&ß6±ÊZ²»=Ö"IçÜ5Ã‘è‚VÁ bY£‰ÜxõµAÇ½ÿı~µ‡}RØ[6½û"ĞõÙç®„pjáı¾MÍÚ%Ô6ËY[‰”…t›´wâ‡T|ı,ØÑ¼­÷H¿cóR‚ˆô:Ÿ3šÆS˜ss®Gn‘{fÀf!ŞP–#àÄbI[ÑNH·YQïéP¾ÈEcüóÏ@?¯Ù…5ô‰ñ±FuKjO½N>TøÖ&0C»ïÑGõ¦“Ëß¦E*q9{V0‡¸Ë¿PsÊ)ê?à ©‡4ßÓŒ^Õ½†{ü£¥ğWC*=ƒ)Lä©^³}½$Œam(Çátu”8œ˜D½,ÕõÊyi€¶º]û.‘Ğ1íHt ªÛEXş—Üùé¾›ÌmFxÙ3¨
.¶‡s*bm
/ÈIÑ›6y£‡Ç%ZC¦aë|øIFrt²$Ÿ;ÜI&ˆÄB.0ÙƒêSŸ f xı—ÛiñöpÍØC²öHÏl«Û=Ñjù2-J] —Ó½5ÎL®ág^0yÚnÑÄvÍšD’îÕ`+Ğ@âÛ¯MU«SõCWá>™Doã)İ¬Öñ¥À?,È?6èªÎş¬Ünà(Èâ=Èmm’kDÇ®Ê–è{ÖºÈIRQ) << LOCK_USAGE_READ_MASK;

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