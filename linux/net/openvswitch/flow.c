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
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/COMPAT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
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
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
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
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/TRACEPOINTS) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wil/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <waiman.long@hpe.com>
 */

/*
 * Collect locking event counts
 */
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/fs.h>

#include "lock_events.h"

#undef  LOCK_EVENT
#define LOCK_EVENT(name)	[LOCKEVENT_ ## name] = #name,

#define LOCK_EVENTS_DIR		"lock_event_counts"

/*
 * When CONFIG_LOCK_EVENT_COUNTS is enabled, event counts of different
 * types of locks will be reported under the <debugfs>/lock_event_counts/
 * directory. See lock_events_list.h for the list of available locking
 * events.
 *
 * Writing to the special ".reset_counts" file will reset all the above
 * locking event counts. This is a very slow operation and so should not
 * be done frequently.
 *
 * These event counts are implemented as per-cpu variables which are
 * summed and computed whenever the corresponding debugfs files are read. This
 * minimizes added overhead making the counts usable even in a production
 * environment.
 */
static const char * const lockevent_names[lockevent_num + 1] = {

#include "lock_events_list.h"

	[LOCKEVENT_reset_cnts] = ".reset_counts",
};

/*
 * Per-cpu counts
 */
DEFINE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * The lockevent_read() function can be overridden.
 */
ssize_t __weak lockevent_read(struct file *file, char __user *user_buf,
			      size_t count, loff_t *ppos)
{
	char buf[64];
	int cpu, id, len;
	u64 sum = 0;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	id = (long)file_inode(file)->i_private;

	if (id >= lockevent_num)
		return -EBADF;

	for_each_possible_cpu(cpu)
		sum += per_cpu(lockevents[id], cpu);
	len = snprintf(buf, sizeof(buf) - 1, "%llu\n", sum);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

/*
 * Function to handle write request
 *
 * When idx = reset_cnts, reset all the counts.
 */
static ssize_t lockevent_write(struct file *file, const char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	int cpu;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	if ((long)file_inode(file)->i_private != LOCKEVENT_reset_cnts)
		return count;

	for_each_possible_cpu(cpu) {
		int i;
		unsigned long *ptr = per_cpu_ptr(lockevents, cpu);

		for (i = 0 ; i < lockevent_num; i++)
			WRITE_ONCE(ptr[i], 0);
	}
	return count;
}

/*
 * Debugfs data structures
 */
static const struct file_operations fops_lockevent = {
	.read = lockevent_read,
	.write = lockevent_write,
	.llseek = default_llseek,
};

#ifdef CONFIG_PARAVIRT_SPINLOCKS
#include <asm/paravirt.h>

static bool __init skip_lockevent(const char *name)
{
	static int pv_on __initdata = -1;

	if (pv_on < 0)
		pv_on = !pv_is_native_spin_unlock();
	/*
	 * Skip PV qspinlock events on bare metal.
	 */
	if (!pv_on && !memcmp(name, "pv_", 3))
		return true;
	return false;
}
#else
static inline bool skip_lockevent(const char *name)
{
	return false;
}
#endif

/*
 * Initialize debugfs for the locking event counts.
 */
static int __init init_lockevent_counts(void)
{
	struct dentry *d_counts = debugfs_create_dir(LOCK_EVENTS_DIR, NULL);
	int i;

	if (!d_counts)
		goto out;

	/*
	 * Create the debugfs files
	 *
	 * As reading from and writing to the stat files can be slow, only
	 * root is allowed to do the read/write to limit impact to system
	 * performance.
	 */
	for (i = 0; i < lockevent_num; i++) {
		if (skip_lockevent(lockevent_names[i]))
			continue;
		if (!debugfs_create_file(lockevent_names[i], 0400, d_counts,
					 (void *)(long)i, &fops_lockevent))
			goto fail_undo;
	}

	if (!debugfs_create_file(lockevent_names[LOCKEVENT_reset_cnts], 0200,
				 d_counts, (void *)(long)LOCKEVENT_reset_cnts,
				 &fops_lockevent))
		goto fail_undo;

	return 0;
fail_undo:
	debugfs_remove_recursive(d_counts);
out:
	pr_warn("Could not create '%s' debugfs entries\n", LOCK_EVENTS_DIR);
	return -ENOMEM;
}
fs_initcall(init_lockevent_counts);
                                          