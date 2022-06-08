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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ELF                       j      4     (               èüÿÿÿS‹˜X  ‹ˆT  1À‰Z[‰
Ãt& èüÿÿÿ‹€|  ÇAA	  ‰1ÀÃ´&    fèüÿÿÿ‹…Òu‹A…ÀuÇA   1ÀÃt& ¸êÿÿÿÃ´&    v èüÿÿÿƒ9w6y   u-‹D  ‰Q‰Q‹H  Ñê‰Q‹€H  Ñè‰A1ÀÃ´&    ¸êÿÿÿÃ´&    v èüÿÿÿúŒ   vŠPÿÿÿƒùw ¸ Á ÓèƒàÃt& úƒ   —ÀÃ¶    1ÀÃt& èüÿÿÿ‹P@‹‚P  …ÀuÃt& ‹‚L  j ¹@   ºÁ   j j j@èüÿÿÿƒÄÃ´&    èüÿÿÿ‹P@‹‚P  …ÀuÃt& ‹‚L  j ¹@   ºÁ   j j j èüÿÿÿƒÄÃ´&    èüÿÿÿƒì‹€L  ¶Òd‹    ‰L$1É‰áèüÿÿÿ…ÀI$‹T$d+    uƒÄÃèüÿÿÿ´&    ´&    èüÿÿÿUWVS‰Ã‰Ğ‰Ê‰Á€õ@ƒì	Ñt}‰Á€õ	Ñt<‰Æ‰×Æ şÿÿƒ×ÿæÿıÿÿ‰ù	ñtk‰Áá °  uq„Àu}%  ÿ ‰Ç÷ßÉƒá‰Èëv ¹   ¸   ‹    …Òüÿÿÿ‹ƒL  º(   èüÿÿÿ1ÀƒÄ[^_]Ã¶    ¹
   ¸
   ëÆt& ¹   ¸   ë¶t& ¹   ¸   ë¦t& ¹   ¸   ë–´&    t& èüÿÿÿWVS‹˜X  ‹°T  1Ë1Ö	ótd‹°È  ‹˜Ä  …ötD‹{‹s!Î!×‰ó	ûtJ‰Ó‰¸T  ã ù  ‰°X  ƒûÛƒã`Ãà  ‰˜H  [^_é´şÿÿt& ¿ÿÿÿ 1öë¹´&    1À[^_Ã¸êÿÿÿëõv èüÿÿÿ…ÉtGƒyuAS‹˜D  ‰Y‹H  ÇA   ÑêÇA   ‰QÇA   ‹    …É!   1À[Ãt& ¸êÿÿÿÃ´&    v èüÿÿÿ‹€    º   èüÿÿÿ1ÀÃ´&    èüÿÿÿWVS‰Ó‹    …Òİ   {‰Ù1ÀÇ    ƒçüÇCp    ºÈ  )ùƒÁtÁéó«fƒÂ$úœ  t/‹zü…ÿtî‹‹J‰ş9Èwv f	tCƒÀ9ÈvôƒÂ$f	;úœ  uÑ[1À^_ÃèüÿÿÿS‹ƒûtƒûu[éüÿÿÿ´&    [éüÿÿÿ´&    v ¸êÿÿÿ[Ã´&    fèüÿÿÿ‹    U‰ÅWVS‹x\…Òú   ‹‡È  ·   1Û…ÀtJf‰ğƒÃÆŒ   èüÿÿÿ‹‡È  9Øwæ…Àt*·@  1Û´&    v ‰ğƒÃÆŒ   èüÿÿÿ9ŸÈ  wè‰øƒÅèüÿÿÿ‡Ì  èüÿÿÿ‰èº   èüÿÿÿ‰èº   èüÿÿÿ1À[^_]Ãt& èüÿÿÿV‰ÆSzGREY‰Ót<ƒ{t1À[^Ãƒ{uô‹†L  1Éº   èüÿÿÿ‹†L  ¹   º   èüÿÿÿëË‹€L  ¹p   º   èüÿÿÿë­´&    fèüÿÿÿU‰ÅWVSƒìfƒ: ‰$„æ   1Ûv ‹$1Éf‰LX2·|X‹…T  ‰Âòÿÿÿ •X  „  ©ÿÿ sDó1ÉFú‰D$ƒøwl¸È  1ÉƒÀ$ºœ  9Ât#‹Pü…Òtí…×t	;0r;pvƒÀ$ºœ  ƒÁ9Âuİ‹|$‰Î‹…L  Îğ   ÿ‰ñWĞ¶Òèüÿÿÿ‹…L  WÑ‰ñ¶Òèüÿÿÿ‰ñ‹$f‰LXƒÃƒû…>ÿÿÿ‹…L  ¹   ºÍ   èüÿÿÿ1ÀƒÄ[^_]Ã‹€L  1ÉºÍ   »Ğ   èüÿÿÿ‹…L  1ÉºÏ   èüÿÿÿ¶    ‹…L  ‰Ú¹ÿ   ƒÃèüÿÿÿûü   uãƒÄ1À[^_]Ã´&    fèüÿÿÿVS‰Óƒì‹²L  ºÀ   d¡    ‰D$1ÀL$Ç$    ‰ğÇD$    èüÿÿÿ‹L$…Éuk‰áº‡   ‰ğèüÿÿÿ‹$…Àu&‹D$d+    …°   ƒÄ¸   [^Ã´&    v L$º†   ‰ğÇD$    èüÿÿÿ‹L$…Étºº   ‰ğèüÿÿÿë¬fºÀ   ‰ğèüÿÿÿ‹D$¨@t–‹    %€   •ƒt  …Ò7  ‰Øº@  èüÿÿÿ1À€»t   t‹ƒl  j ¹   º   j j P‰ğèüÿÿÿƒÄé?ÿÿÿèüÿÿÿèüÿÿÿS‹˜    º   ‰Øèüÿÿÿ1Ò…Àx
‰Ğ[Ã¶    ‰Â‹ƒô   …ÀtêHÿğ±‹ô   uï‰Ğ[Ã´&    t& èüÿÿÿVS‰Ã·€v  f=PQ„¶   1öf=QQ„‚   ‹ƒd  …Àt‰òƒÎƒÊƒøDò‹    …Òd  ‹ƒL  1Éº   èüÿÿÿ‹ƒL  ‰ñ1Òèüÿÿÿº@   ¸   ƒ»d  EÂ‹›L  j ¹P   j º   j P‰ØèüÿÿÿƒÄ[^Ãv f»x   …oÿÿÿ‹‹p  1À…É”À4Å    éVÿÿÿ1öf»x   …EÿÿÿëÔ´&    ¶    èüÿÿÿ‹P(‹Hú	˜ tmv;ú	˜ „   ú	Ÿ …«   ‹@|…À”À¶À‰¤   4ıÿÿèÇşÿÿ1ÀÃt& ú 	˜ thú	˜ ux‹P|‹€   ‰Ñº   èüÿÿÿ1ÀÃ´&    ‹P|‹€   ‰Ñº
   èüÿÿÿ1ÀÃ´&    ‹P|‹€   ‰Ñº   èüÿÿÿ1ÀÃ´&    ‹P|‹€   ‰Ñº	   èüÿÿÿ1ÀÃ¸êÿÿÿÃfèüÿÿÿ‰d  1Òƒù•Â‰ˆh  ‰p  èşÿÿ1ÀÃ´&    fèüÿÿÿU‰ÅW1ÿV°,     Sƒì‹È  ‰D$…ÛuéZ  fƒÇÆŒ   9½È  †C  iÇŒ   1Ò^N`è‹€,  9ğDÂ‹Vü‹@ÇF|   ‰$º   ‰D$1À…ÿ”À‰D$‰Øèüÿÿÿ…Àˆ­   ‹E`‰Ú‹@èüÿÿÿ…Àˆ˜   ÿt$·D$‰é1ÒP‰ØèüÿÿÿZY…Àx}ƒ<$tO…ÿ…_ÿÿÿ‹T$1À…Ò•À‰$‹$‰…d  ‰èèıÿÿ‹D$‹•8  ‹<  ‰…Ä  ‰èè4÷ÿÿéÿÿÿ´&    ÿt$‹F‰é1Ò·@P‰ØèüÿÿÿY[…Àx…ÿt¦éòşÿÿf‰Â‹…È  …Àt.µ@  1Û‰×´&    v ‰ğƒÃÆŒ   èüÿÿÿ9È  wè‰úƒÄ‰Ğ[^_]Ãt& ƒÄ1Ò[‰Ğ^_]Ã´&    t& èüÿÿÿUWVS‰Ãƒì·Bf‰$‹Bƒà…‹  ‰Ö·$‰Ïƒò·Ê‰L$‹‹È  …É„~  ‹o“@  ëv ƒÀÂŒ   9È„_  9ÕuëiÀŒ   ‹    ‹„(  …É‘  ƒø…@  ½   “À   ‰Ğ‰$èüÿÿÿ…Àt!‹$èüÿÿÿ…À„X  Ç@$    ‹@ Ç@$    “à   ‰Ğ‰$èüÿÿÿ…Àt!‹$èüÿÿÿ…À„#  Ç@$    ‹@ Ç@$    ‹D$$‰éƒà‰Âƒò„Éu~·N‰“h  ‰‹d  …Àt~Çƒp     ‰Øè1ûÿÿ‹“È  …ÒttG”‹“T  ‰ƒÄ  ‹O¬‹ƒT  ‹o°1ÿ!Ê1ĞƒX  ‰ÖtI…Òu‰Î‰ï‰ò‰ù‰Øè)õÿÿ1ÀƒÄ[^_]Ã´&    Çƒd     ‰“h  …Àu4Çƒp      ‰Øè³úÿÿ1ÀƒÄ[^_]Ã´&    f¡    …ÀM1íé¾şÿÿÇƒp     ‰Øèúÿÿ‹T$‰øƒÂÁâÚèüÿÿÿ…Àt‹t$$‰p$‹@ ‰p$é%ÿÿÿ¸êÿÿÿëéµ  éW  t& èüÿÿÿU‰Å1ÉWVS»À  ƒì‹°L  1À¶Ğ‰÷ët& ¶K‹½L  ‰øƒÃèüÿÿÿ¶€úÿuä‹•P  …Ò„  1Éº   ‰ğèüÿÿÿ¹   ºÂ   ‰ğèüÿÿÿ¹   º   ‰ğèüÿÿÿ‹µL  1ÉºÏ   »Ğ   ‰ğèüÿÿÿt& ‰Ú¹ÿ   ‰ğƒÃèüÿÿÿûü   uç»Ğ  ‰,${$û€  tt& ‹Gô…Àu1‰û{$û€  uì‹,$‰èèRùÿÿ…Ì  èüÿÿÿ1ÀƒÄ[^_]Ãt& ·oğºÅ   ‰ğƒÃ‰éfÁé·Éèüÿÿÿ·ÍºÄ   ‰ğèüÿÿÿt& ¶KºÃ   ‰ğƒÃèüÿÿÿ9ûué‰ûë†¹   º   ‰ğèüÿÿÿ¹   ºÂ   ‰ğèüÿÿÿ1Éº   ‰ğèüÿÿÿéúşÿÿ´&    èüÿÿÿU‰ÅWVS1Ûƒì‰T$d¡    ‰D$‰ĞxHÇ@    ƒçüÇ@^    1À)ùƒÁ`Áéó«ÇD$    ëCv ‹D$…Àˆ“  ƒàƒø
À4…Ä  ƒÇ;<$uX‰ğ·ö	t$‹L$f‰DYƒÃƒût`‹…T  ‰Âòÿÿÿ •X  „³  ©ÿÿ SDÓ1ÀƒêƒúwÀTĞ1ö¶úG‰$‹…L  L$‰úèüÿÿÿ…À‰iÿÿÿé“  t& ‰È·L$f‰‹D$d+    u
ƒÄ1À[^_]Ãèüÿÿÿ´&    v èüÿÿÿS‰Óƒì‹€L  d‹    ‰T$1Ò‰áºˆ   èüÿÿÿº    …ÀI$ƒàƒø¸ÿÿ  EÂ‰C<‹D$d+    uƒÄ1À[ÃèüÿÿÿèüÿÿÿVS‰Ãƒì‹³    d¡    ‰D$1À…Ò„¢   º   ‰ğèüÿÿÿ‰Â…Àˆ$  ¹   º   ¾”  ëv ¶N‹ƒL  ƒÆèüÿÿÿ¶€úÿuæ‹“T  ‹‹X  ‰Ğ5ÿÿÿ 	È„û   ‰Øèğÿÿ‹ƒ|  ƒø„•   ƒø„|   ‹ƒP  …Àub‹³l  º@  ‰Øèüÿÿÿë¶    ‰ğº   1öèüÿÿÿj ‹ƒL  º   ¹   j j Vèüÿÿÿ1ÒƒÄ‹D$d+    …»   ƒÄ‰Ğ[^Ã1ö€»t   t™ë‘v Çƒl  	   éuÿÿÿj ‹ƒL  ¹   º   j j j èüÿÿÿÇƒl     ƒÄéEÿÿÿ‹†ô   …ÀtHÿğ±ô   „|ÿÿÿëév ‹ƒL  ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
v1Ò1ÉéÙşÿÿt& ‹Å    ‹Å   éÂşÿÿèüÿÿÿ´&    èüÿÿÿV‰ÖS‰Ãƒìd¡    ‰D$1À‹ƒP  …Àt,¶ƒt  …ÀuF1À1Ò‰‰V‹D$d+    ucƒÄ1À[^Ãv ‹ƒL  ‰áºˆ   èüÿÿÿ‹$ƒàƒø”À¶À…Àtº‹ƒL  ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
w—‹Å   ‹Å    ë‹èüÿÿÿv èüÿÿÿVS‰Ãƒìd¡    ‰D$‹ƒT  5ÿÿÿ ƒX  t‹D$d+    …á   ƒÄ1À[^Ã¶    ‹ƒL  ‰áºŒ   èüÿÿÿ‹‹\  ‹“`  …ÀI$ƒàƒèƒø
vI	Êt®Çƒ\      Çƒ`      Çƒ@      Çƒ<      ÇƒD  Ğ  ÇƒH  @  émÿÿÿt& ‹4Å    ‹Å   1ñ1Â	Ê„Oÿÿÿ‰³\  æ ù  ‰ƒ`  Çƒ@      Çƒ<      ÇƒD  Ğ  t¡ÇƒH  à  éÿÿÿèüÿÿÿèüÿÿÿU1ÒWVS‰Ãƒìd¡    ‰D$‹ƒL  L$èüÿÿÿ…ÀˆĞ  éÌ  ´&    v èüÿÿÿUW‰ÇVSƒìd¡    ‰D$‹G‹P‹Rèüÿÿÿ%   =   …š  o¹   º  ‰èèüÿÿÿ‰Ã= ğÿÿ‡Y  …Àt1Òèüÿÿÿ¸   èüÿÿÿ¹   º  ‰èèüÿÿÿ‰Ã= ğÿÿ‡&  …Àt'¸Ä	  èüÿÿÿ1Ò‰Øèüÿÿÿ¹   ºú   ¸È   èüÿÿÿ¹À  º€  ‰èèüÿÿÿ‰Æ…À„  j 1Éº€   ‰øèüÿÿÿ[‰Ã= ğÿÿ‡Ê   ‰†L  ¹À  ‰ú‰ğèüÿÿÿº   ‰ğƒN\À   ÇFh  Ç†|     ÇF8  ÇF  Ç†Ü      Ç†Ø      Ç†ü      Ç†ø      Ç†     Ç†     èüÿÿÿ‰Ã…Àu?‹†”   º€   ‰$‹†L  jL$èüÿÿÿY‰Ã…À‰$  éı  ´&    …Û„ışÿÿ‹D$d+    uƒÄ‰Ø[^_]Ã»ûÿÿÿëáèüÿÿÿ»ôÿÿÿëÕ´&    ¶    èüÿÿÿUWV‰ÆSƒì‹Yd¡    ‰D$1À…Û…«  ¡    ‰×‰Ë…Àé  ‹k1À‹S…íHè¸ÿ  9ÅOè1À…ÒHĞ¸   ‰k9ÂOĞ‰S‹†T  ‰Áñÿÿÿ X  „š  % ù  ƒøÀƒà`à  )Ğj ¹Ğ  ºÑ   j )ê)ékPƒèPCP‰èjèüÿÿÿ‹‹CƒÄ…Ò„  ¸êÿÿÿƒút‹T$d+    …  ƒÄ[^_]Ãt& ¾<  ÿ ğÿÿ‡õ  ‹†D  9C„O  ‹T  ‹†L  ‰Êòÿÿÿ –X  „g  €åù„  ½à  ‹Kº   èüÿÿÿ‹K‹†L  º   K)éèüÿÿÿ‹K‹†L  º   Áùèüÿÿÿ‹K‹†L  º   ƒÉèüÿÿÿ‹S‹CŒşÿÿ‹†L  º   Áéèüÿÿÿ‹S‹CŒşÿÿ‹†L  º   èüÿÿÿë$·N 9Èƒì   €Áâz0ÿ ğÿÿ‡  ‹C‰‹C‰G‹C‰G‹C‰G1ÀéÂşÿÿ´&    ‹†L  ‰áºŒ   èüÿÿÿ‹S‹k…ÀI$ƒàHÿ¸@  ƒù
‡Fşÿÿ‹Í    é(şÿÿ´&    v ‹†H  9C…¢şÿÿ‹†@  9C…“şÿÿ‹†<  9C…„şÿÿéaÿÿÿ¶    ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
v+‹†L  ½@  ézşÿÿv éÿÿÿ¸êÿÿÿéùıÿÿ¶    ‹Å    ‹†L  é?şÿÿèüÿÿÿ‰øéÕıÿÿfèüÿÿÿV‰ÆS‰Ëƒì‹Id¡    ‰D$1À…Éto¸êÿÿÿƒùuEÇC    ÇC    ÇCĞ  ‹–T  ‰Ğ5ÿÿÿ †X  „‡   €æù„¡   ÇCà  1Àt& ‹T$d+    …Ã   ƒÄ[^Ã´&    f‹‹K…Àtƒøu2†<  = ğÿÿwÅ‹‰S‹P‰S‹P‰S‹@‰C1Àëª´&    v ¸êÿÿÿë™´&    f‹†L  ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
v-ÇC@  1Àé_ÿÿÿ´&    ·v 9ñs ‰ÁàƒÀ0étÿÿÿ‹Å    éÿÿÿt& ëâèüÿÿÿ               €                 [                                                              À           €                                                         À                                               ÿ       $                                                                                             @   p   À      @  €  Ğ  °  @  F      À  @  €  0           p  `	  @
  p
    0  À  à  @    À  à             Set video std register to %d.
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
 tvp5150 include/media/v4l2-subdev.h Disabled Black screen ‰$Ph    ÿ³    h    èüÿÿÿ‹L$ƒÄéD  RSh   ÿ°    h    èüÿÿÿ1ÀƒÄé…  èüÿÿÿUW‰ÇVS¶Ùƒì‰T$¶l$0d¡    ‰D$1À‰,$‰ı9$~Q‹4$)Ş;t$4Ot$41ÿ9÷}‰è¶Òè|  ˆD<GëèD$PVSÿt$h   ÿµ    h    èüÿÿÿ\$PƒÄëª‹D$d+    tèüÿÿÿƒÄ[^_]Ãh0   ÿ°    h    èüÿÿÿƒÄéÔ  ·@ÀPh    ÿ·    h    èüÿÿÿƒÄé˜  hX   ÿµ    èüÿÿÿ1ÉXZéj  …ÀºE   ¸B   DÂPhH   ÿ³    h    èüÿÿÿƒÄéÚ  ƒ»h   ºZ   ¹a   EÑ…Àt2ƒø¹q   ¸~   EÁj VRPh”   ÿ³    h    èüÿÿÿƒÄé¸  ¸x   ë×‹L$$‰D$ƒáQ·T$‰L$R‹Vÿr·WRÿuhğ   ÿ³    h    èüÿÿÿ‹D$,ƒÄ ƒø…Õ  ƒ=     ‹L$½   š  Q‹D$ÁàØ·Ô   R‹€Ğ   ÿp·GP‹Gÿphğ   ÿ³    h    èüÿÿÿƒÄ éš  ‹D$$ƒàP·D$P‹Fÿp·GP‹Gÿphğ   ÿ³    h    èüÿÿÿƒÄ éÕ  PhX   h„   ÿµ    èüÿÿÿ1ÀƒÄé;  hX   ÿµ    èüÿÿÿX1ÀZé;  ‹D$Ph  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰'
  PhD  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰û	  Php  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰Ï	  Phœ  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰£	  PhÈ  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰w	  Phì  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿ‰ÇƒÄ…À‰I	  ‹ƒL  L$º   èüÿÿÿ‰Æ…Àx‹t$‹ƒL  L$º   èüÿÿÿ…Àx‹D$WVPh   ÿ³    h    èüÿÿÿ‹ƒL  º	   L$èüÿÿÿƒÄ…À‰á  Phh  ÿ³    h    èüÿÿÿ‹ƒL  º
   L$èüÿÿÿƒÄ…À‰µ  Ph  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰‰  Ph¼  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰]  PhÜ  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰1  Ph   ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰  Ph4  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿ‰ÆƒÄ…À‰×  ‹ƒL  L$º   èüÿÿÿ…Àx‹D$VPhd  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿ‰ÆƒÄ…À‰Œ  ‹ƒL  L$º   èüÿÿÿ…Àx‹D$VPh˜  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰C  PhÌ  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰  Phì  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰ë  Ph  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰¿  PhD  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿ‰ÆƒÄ…À‰‘  ‹ƒL  L$º   èüÿÿÿ…Àx‹D$VPhp  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰H  Ph°  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰  Phà  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰ğ  Ph  ÿ³    h    èüÿÿÿ‹ƒL  º(   L$èüÿÿÿƒÄ…À‰Ä  PhH  ÿ³    h    èüÿÿÿ‹ƒL  º-   L$èüÿÿÿ‰ÆƒÄ…À‰–  ‹ƒL  L$º,   èüÿÿÿ…Àx‹D$VPhl  ÿ³    h    èüÿÿÿ‹ƒL  º.   L$èüÿÿÿƒÄ…À‰M  Ph   ÿ³    h    èüÿÿÿ‹ƒL  º/   L$èüÿÿÿƒÄ…À‰!  PhÌ  ÿ³    h    èüÿÿÿ‹ƒL  º0   L$èüÿÿÿƒÄ…À‰õ  ƒàº   )ÂRhø  ÿ³    h    èüÿÿÿº   ‰Øè|  º€   ‰Æ‰Øè|  VPh,  ÿ³    h    èüÿÿÿºƒ   ‰ØƒÄ$è|  º‚   ‰Æ‰Øè|  VPhL  ÿ³    h    èüÿÿÿº…   ‰Øè|  º„   ‰Æ‰Øè|  VPht  ÿ³    h    èüÿÿÿº†   ‰ØƒÄ(è|  Ph   ÿ³    h    èüÿÿÿº‡   ‰Øè|  PhĞ  ÿ³    h    èüÿÿÿºŒ   ‰ØƒÄ è|  º‹   ‰$‰Øè|  ºŠ   ‰Å‰Øè|  º‰   ‰Ç‰Øè|  ºˆ   ‰Æ‰Øè|  ‹$QUWVPh   ÿ³    h    èüÿÿÿ¹±   º    ‰ØƒÄ jhµ   è¤õÿÿ¹¶   º²   ‰Øjhº   èŒõÿÿº»   ‰Øè|  Ph<  ÿ³    h    èüÿÿÿºÀ   ‰ØƒÄ è|  Phh  ÿ³    h    èüÿÿÿºÁ   ‰Øè|  Ph˜  ÿ³    h    èüÿÿÿºÂ   ‰ØƒÄ è|  PhÈ  ÿ³    h    èüÿÿÿºÆ   ‰Øè|  Phô  ÿ³    h    èüÿÿÿºÇ   ‰ØƒÄ è|  Ph  ÿ³    h    èüÿÿÿºÈ   ‰Øè|  Ph@  ÿ³    h    èüÿÿÿºÉ   ‰ØƒÄ è|  PhÄ   ÿ³    h    èüÿÿÿºÊ   ‰Øè|  Phl  ÿ³    h    èüÿÿÿºË   ‰ØƒÄ è|  ºÌ   ‰Æ‰Øè|  VPh˜  ÿ³    h    èüÿÿÿºÍ   ‰Øè|  PhÈ  ÿ³    h    èüÿÿÿºÏ   ‰ØƒÄ$è|  Phğ  ÿ³    h    èüÿÿÿºü   ‰Øè|  Ph	  ÿ³    h    èüÿÿÿ¹   ºâ   ‰ØƒÄ jh“   è–óÿÿ¹”   ºì   ‰Øjh™   è~óÿÿ¹š   ºö   ‰Øjh¦   èfóÿÿ¹§   º   ‰Øj
h¯   èNóÿÿº
  ‰Ø¹Ğ   ƒÄ jhû   è3óÿÿXZ‹D$d+    …ı   ƒÄ1À[^_]Ã‹D$éĞõÿÿ‹D$éüõÿÿ‹D$é(öÿÿ‹D$éTöÿÿ‹D$é€öÿÿ‹|$é®öÿÿ‹D$é÷ÿÿ‹D$éB÷ÿÿ‹D$én÷ÿÿ‹D$éš÷ÿÿ‹D$éÆ÷ÿÿ‹D$éò÷ÿÿ‹t$é øÿÿ‹t$ékøÿÿ‹D$é´øÿÿ‹D$éàøÿÿ‹D$éùÿÿ‹D$é8ùÿÿ‹t$éfùÿÿ‹D$é¯ùÿÿ‹D$éÛùÿÿ‹D$éúÿÿ‹D$é3úÿÿ‹t$éaúÿÿ‹D$éªúÿÿ‹D$éÖúÿÿ‹D$éûÿÿèüÿÿÿ·D$·T$‹$¶L$fÁÀfÁÂf‰†v  ·Àf‰–x  ‹S‰$Â  R·SÒR¶T$RQPhh	  ÿ¶    èüÿÿÿ·†v  ƒÄf=PQ…‰   ·x  ‹–    fù!…š   h  RèüÿÿÿXZ‹È  1À9Ø„Á   iÈŒ   ‹–T  ƒÀ”8  ‰$‹–X  ”<  ‰Ñ‹$‰X  ‰–T  ë¿PhD	  ‹D$ƒÀPèüÿÿÿƒÄéÔ  ‹–    f=QQu$f¾x   uhH  RèüÿÿÿXZésÿÿÿfù tPh”	  ÿ¶    èüÿÿÿƒÄéSÿÿÿh2  Rèüÿÿÿ‹†L  1Éº0   èüÿÿÿY[é/ÿÿÿ…ÀuÇ†T  ÿÿÿ Ç†X      †Ì  1Éº   Ç†\      ‰$‰Ãj Ç†`      Ç†d     Ç†p     èüÿÿÿ¹ 	˜ º  ‰Øj h€   j jj hÿ   j j èüÿÿÿ¹	˜ º  ‰ØƒÄ$j h€   j jj hÿ   j j èüÿÿÿ¹	˜ º  ‰ØƒÄ j h€   j jj hÿ   j j èüÿÿÿ¹	˜ º  ‰ØƒÄ j j j jj jjÿj€èüÿÿÿ¹	Ÿ º  ‰ØƒÄ j hÀü›j jj hÀü›j hÀü›èüÿÿÿº  ‰Ø¹	Ÿ ƒÄ h  j j j jèüÿÿÿ‰^l‹  ƒÄ‰Ú…Ût‹$‰T$èüÿÿÿ‹\$éÔ  ‹†L  L$ºŒ   èüÿÿÿ…Àx‹D$ƒàº@  ƒèƒø
†¨   ‹‡à  ‰–H  1ÒÇ†@      ‰†P  ‰ğÇ†<      Ç†D  Ğ  è,  ‹—à  …Òt&V1É‰èh[  h   h   èüÿÿÿ‰ÂƒÄ…À…Rÿÿÿ‰ğèüÿÿÿ‰Â…ÀˆAÿÿÿƒ=    ~‰ğèÜ  1Ò‰èèüÿÿÿ‰èèüÿÿÿ1Ò‰èèüÿÿÿéÔ  ‹Å    % ù  ƒøÒƒâ`Âà  é9ÿÿÿÿqÿqÿqÿqhh   h¸	  ÿ¶    h    èüÿÿÿƒÄ éB     W  ­  Ø  c  ë	P  c  ë	èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ           °              ÿ                                              @                ÿ     tvp5150_get_vbi tvp5150_set_selection   tvp5150                                                                                                                                         @           ˆ  @  
                                                (   (   ,   0 