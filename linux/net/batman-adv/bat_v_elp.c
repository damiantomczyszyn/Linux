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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ELF                      ¤k      4     ( & %             èüÿÿÿUd¡    ‹€p  ‰å]‹@™Ã¶    èüÿÿÿU‰Âd¡    ‹€p  ‰å‰P1À1Ò]ÃèüÿÿÿWVS‰Ó‰Ê·Kf…É„‡   fƒùuq¶K‰ÎÀéƒæƒø„¤   Áá¸    Æa	ñf‰BBˆJÇB    ¶K¶ÇB    ƒãƒËaˆZ‰ËƒáÁã	ÙˆJ	1Éf‰J
‹|$)ĞÁøÇ   [^_Ã[1À^_Ã´&    v ¶‹|$¶KƒàÇ   [ƒÈa^_ˆJˆ¸   f‰B¸   ÇB    Ãv €ù	„¿   ‰ğ<	„   ¹	   ‰ğ¶øAÿƒà9ÏDÈ‰ÈÆ{ÁàÇB    	ğ¾$   f‰r¶ñˆB¶CÁæ‰÷ÇB    Îy $ ƒàÏa   Áà	ø1ÿ‰B¶CÆBcƒàğf‰z	ÈÇB    ˆB¶KB ÇB    ƒáÁá	ñ‰Jéúşÿÿ¶    €ù•ÁƒÁéjÿÿÿ´&    v ¹   éFÿÿÿ¶    èüÿÿÿU‰å…Àt‹ ]‹@$‹  ‹€   Ã¸    ]‹  ‹€   Ã´&    ¶    èüÿÿÿUWV‰ÆS‰Ó‰Ê·Cfƒø„“  ifƒø„Ç  fƒøu)¶CÆaÇA    ˆA¸   f‰A¸   [^_]Ãt& f…À…ÿ   ¶CÆaˆA1É[¸   ^_f‰JÇB    ]Ã¶    fƒø „Ş   fƒø$…„   ¶C‰ÁÀèƒáƒş„o  Áà¾   Æa¿p  	Èf‰r½   ˆBÇB    ¶CÆBa‰Áƒàf‰z
ÁáÇB    	ÈˆB	¶CÆBa‰Áƒàf‰jÁáÇB    	ÈˆB¸   é.ÿÿÿfƒøu:¶C‰ÁÀèƒáƒş„İ   ÆaÁà[^ÇB    	Á¸   _]f‰B¸   ˆJÃ[1À^_]Ã´&    f¶C‰ÁÀèƒáƒş„k  ÆaÁà[^ÇB    	Á¸   _]f‰B¸   ˆJÃ¶    ¶C‰ÁÀèƒáƒş„#  ÆaÁà[^ÇB    	Á¸   _]f‰B¸   ˆJÃ¶    ¶CÆaˆA¹   [¸   ^_f‰JÇB    ]Ãt& Æcéÿÿÿ<	„Ø   ¾	   €ù	„Ô   Fÿ¶ùƒà9÷Dğ‰ğÆ{ÁàÇB    	È‰ñˆB¶ù¸   f‰B‰ù¶CÁçÁáÇB    ‰ÍƒàÇB    ÁàÍa  	è‰B‰ÈÉy  a p	ø‰B¶CÆBcƒàğÇB    	ğˆB¸   f‰B¶CÇB$    ƒàÁà	È‰B ¸   éiıÿÿv ÆcéØşÿÿÆcéşÿÿ´&    ¾   é,ÿÿÿ<•Àpé-ÿÿÿ´&    fèüÿÿÿ1ÀÃ´&    èüÿÿÿéüÿÿÿ¶    èüÿÿÿUWV‰Æ¸    S‰Óèüÿÿÿ‹F(…Àt2‹€”   ‹x<‹h@¸    èüÿÿÿ‰{‰k‹F,‰C[1À^_]Ã´&    f1ÿ1íëÔ´&    v èüÿÿÿPÇ@àÿÿÿH‰P‰P‹    Ç@  ¸    éüÿÿÿèüÿÿÿUWV‰ÖS‰Ã¸    èüÿÿÿ‹C(…Àt2‹€”   ‹x<‹h@¸    èüÿÿÿÿs,UWh    VèüÿÿÿƒÄ[^_]Ãt& 1ÿ1íëÔ´&    v UW‰×VS‰Ãƒì d¡    ‰D$1ÀöC…§   >t& ‹K$S0‰øèüÿÿÿ‰ÅöC…Ù   ‹D$d+    …ğ   ƒÄ ‰è[^_]Ã¶    èüÿÿÿ‹s$S0‰D$‰øèüÿÿÿ‹s‰Åèüÿÿÿ4…    œ$úƒFƒƒV èüÿÿÿ‹L$1Ò)ÈFVƒF÷$   „zÿÿÿûétÿÿÿ´&    ‹B ÇB     ‰D$‹B$ÇB$    ‰D$‹B(ÇB(    ‰D$‹B,ÇB,    ‰D$‹B0ÇB0    ‰D$éÿÿÿt& ‹D$‰G ‹D$‰G$‹D$‰G(‹D$‰G,‹D$‰G0éÿşÿÿèüÿÿÿ´&    t& èüÿÿÿU‰åWVSƒì‹P‹M‹u…Òt]‹x‰Ãºÿÿÿÿ¸êÿÿÿ…ÿt=ƒ{ t7‰$…Ét=…öt9¸ùÿÿÿºÿÿÿÿşÿ  w‹$‰ñ‰øèüÿÿÿ‰s1À1ÒÇC   ƒÄ[^_]Ãt& ƒÄ¸êÿÿÿºÿÿÿÿ[^_]Ã¶    èüÿÿÿWVS‹t$…Àtb‰Ó…ÒtB‰Ç‰Ê…ÉtB…öt>9ó‰ñFËèüÿÿÿ9ówÆDÿ ¸ùÿÿÿ[^_Ã)ó71Ò‰Ùèüÿÿÿ‰ğ[^_Ãt& ¸ùÿÿÿëİ‰Ù1Ò‰øèüÿÿÿ¸êÿÿÿëÊ¸êÿÿÿëÃ´&    èüÿÿÿU‰åW}S‹H‹‹Wÿp‰ØèaÿÿÿYeø[™_]Ã´&    èüÿÿÿU‰åW}S‹‹W‹x…ÿu?…Ût#…Òt‰Ñ‰Ø1Òèüÿÿÿeø¸êÿÿÿºÿÿÿÿ[_]Ãv eø¸êÿÿÿºÿÿÿÿ[_]Ã´&    ‹Hÿp‰ØèëşÿÿYeø[™_]ÃfèüÿÿÿUWV‰ÆS‰Óƒì‹º¤   d¡    ‰D$1À‹‚    ·’”   ‰$…ö„6  ‰Í¶N¸   Óà%¿ïÿÿ„û   ·Fƒà÷fƒø…  ‹K$)×‹†ä  ‰s‰L$‹KT‹s8‰“¤   ù‰KT+KXÊ‰4$‰S8ƒı„ó   ‹„¨Ø  ÇD$    ‰D$èüÿÿÿèüÿÿÿ‹D$L$d‹    h‹²p  ‰Šp  ‹@…Àt,‰Ú‰l$èüÿÿ…Àu|$ ğÿÿwÇD$ÿÿÿÿ‹EƒÅ…ÀuÕd¡    ‰°p  èüÿÿÿèüÿÿÿ‹D$…ÀuO‹ST‹$)ú‰K8‰ST;SX‚  »¤   ‹|$‰{‹T$d+    …ô   ƒÄ[^_]Ã´&    v 1ÀëÙt& = ğÿÿºòÿÿÿFÂë¢´&    f‹¨Ü  ÇD$    èüÿÿÿèüÿÿÿud¡    ‹ˆp  ‰L$L$‰ˆp  ‹E1í…Àt3f‰Ú‰t$è%ûÿÿ¨u|$ ğÿÿwÇD$ÿÿÿÿÑèƒÆ	Å‹…ÀuÒƒå‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹T$D- …Ò„şşÿÿú ğÿÿ¸òÿÿÿFĞ…í¸   DÂéáşÿÿ´&    èüÿÿÿ´&    fèüÿÿÿUW‰ÏV‰ÖS‰Ãƒì‹@ …Àtèâÿÿÿ‰Å…Àx|‹‹‹‹èüÿÿÿ‰Ã…ÀxS‹)Á‰ëtGº   ¸   ‰$‹.9ÑFÑèüÿÿÿ‹$ƒøv9Á…üÿÿÿP9Á‰èEÊº   èüÿÿÿ…Àx,Ã)ƒÄ‰Ø[^_]Ãt& 1íëˆt& ƒÄ‰Ã‰Ø[^_]Ãt& ƒÄ‰Ã‰Ø[^_]Ãt& èüÿÿÿU‰åV‰ÆSƒì‹M‹Ud¡    ‰D$1À‹E‰L$‰$…ÀtYƒât,1Û‹V‹èüÿÿÿÃ…ÀIÃ™‹L$d+    u?ƒÄ[^]Ãt& ‹…Àt"‹@ ‰âL$èÌşÿÿ‰Ã™…ÀxÍ‹L$‹$ë²t& ¸êÿÿÿºÿÿÿÿë´èüÿÿÿ´&    èüÿÿÿ…ÀxOƒùtJV‰ÖS‰Ãƒøw4‰Ø™÷ş…Òu+ƒû6‹D$Ç@   ƒşwVÿ¸   …òt´&    v 1À[^Ãv 1ÀÃt& ƒş[^”ÀÃ´&    èüÿÿÿ…Àx?W‰Ï1ÉV‰ÖS‰Ãƒøw!‰Ø™÷ş…Òuƒû~&ƒëƒûwƒş”Áƒÿt3[‰È^_Ãt& 1ÀÃt& ƒÿuè‹D$Ç@   ƒşwØë´&    f‹D$1ÉÇ@   ƒşw»Fÿ…ğ”Áë±t& èüÿÿÿƒø'wVV‰Ö™S‰Ã÷ş1À…Òu"ƒùtrƒû„¡   …Ût;ƒûuNƒş„­   [^Ãfƒû$u;1Àƒşuï‹D$ƒx”Àëâ´&    1ÀÃt& ƒşuÎ‹D$Ç    ¸   ë½fƒş”Àë³´&    ƒû të!ƒãûƒûuœƒşu—‹D$ƒx”ÀëŠ´&    ƒû$tˆéyÿÿÿ¶    ƒş…jÿÿÿ‹D$Ç 	   ¸   éVÿÿÿv ‹D$Ç    ¸   é?ÿÿÿt& èüÿÿÿƒøh„²   w ƒøf„—   º   ƒøguE‰ĞÃ´&    v ƒøjtkº    =    tâº    ƒøitØ=º   t~=»   u9ºà  ‰ĞÃ´&    º€  ƒøet±ƒøQtywÒƒøtbº    ƒøPt›º    ƒøt‘éüÿÿÿ´&    fde/linux/objtool.h \
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