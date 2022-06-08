card include/config/BPF_JIT) \
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
  include/media/i2c/upd64031a.h \

drivers/media/i2c/upd64031a.o: $(deps_drivers/media/i2c/upd64031a.o)

$(deps_drivers/media/i2c/upd64031a.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      R  À  ¼     a      
     u  $        €     i     ›    Œ     ²     ü     Í     ú     ß     <     ş  à  <       €  <     -  @  <     P  À   <     o     <     „     r     –     Í    ©      #     ´  °  w     Ê  0  7    â  p      ù  €  …        ¡     ,  À       D  à       [     
    q  `   <     ”  0      ¨  \                     ½           û          :          r          «          X       ¸     Ç   `	  W    9  °  6    ¸  ğ  ³    <  p  6    ß           î             ù                          ğ  3     2             ;             A             N             Y             f             p             ~             ‰             ›             ±             Ê             Ö             ï                                       (  °  Š     A             H             O  À  (     l  ğ  n     …             •             ¥             ·             Æ             Î             Ö             ä  €  ™     ø             	             '	             G	             e	             y	             ‹	             §	             Ã	             Í	             å	             ø	             
             
             3
             G
             [
             t
             ‡
             Ÿ
             »
             Ó
             ì
                                       .             D             \             k             ‚             –             ¯  P$  Q     Â             Ş  °$      ñ               @&  b                 +             A  °'  Ù     X  (  E    o             }             Œ             œ             ­  à)  ¬    Ã             ç             Ù             õ  ,  b       .  å    6             M             f  ğ0  
    ‰                          ­             ¾             Ë   4      î             û                6  ›    0  ,        D  0        \  œ        o           †  ¼       š  Ô        cgroup.c __kstrtab_cgroup_bpf_enabled_key __kstrtabns_cgroup_bpf_enabled_key __ksymtab_cgroup_bpf_enabled_key __kstrtab___cgroup_bpf_run_filter_skb __kstrtabns___cgroup_bpf_run_filter_skb __ksymtab___cgroup_bpf_run_filter_skb __kstrtab___cgroup_bpf_run_filter_sk __kstrtabns___cgroup_bpf_run_filter_sk __ksymtab___cgroup_bpf_run_filter_sk __kstrtab___cgroup_bpf_run_filter_sock_addr __kstrtabns___cgroup_bpf_run_filter_sock_addr __ksymtab___cgroup_bpf_run_filter_sock_addr __kstrtab___cgroup_bpf_run_filter_sock_ops __kstrtabns___cgroup_bpf_run_filter_sock_ops __ksymtab___cgroup_bpf_run_filter_sock_ops sysctl_convert_ctx_access cg_sockopt_convert_ctx_access cg_sockopt_get_prologue bpf_cgroup_link_dealloc bpf_cgroup_link_fill_link_info cgroup_bpf_release_fn cgroup_bpf_release bpf_cgroup_link_show_fdinfo __bpf_prog_run_save_cb copy_sysctl_value sysctl_cpy_dir sysctl_cpy_dir.cold __func__.0 cgroup_dev_is_valid_access sysctl_is_valid_access cg_sockopt_is_valid_access sysctl_func_proto bpf_sysctl_get_new_value_proto bpf_set_retval_proto bpf_sysctl_get_name_proto bpf_sysctl_get_current_value_proto bpf_sysctl_set_new_value_proto bpf_get_retval_proto sockopt_alloc_buf cgroup_bpf_replace CSWTCH.197 cgroup_dev_func_proto compute_effective_progs update_effective_progs __cgroup_bpf_detach bpf_cgroup_link_release.part.0 bpf_cgroup_link_release bpf_cgroup_link_detach cg_sockopt_func_proto bpf_get_netns_cookie_sockopt_proto __cgroup_bpf_attach bpf_cgroup_link_lops __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_ops491 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_addr490 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sk489 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_skb488 __UNIQUE_ID___addressable_cgroup_bpf_enabled_key483 bpf_get_retval __fentry__ current_task bpf_set_retval bpf_get_netns_cookie_sockopt init_net kµdNİ7Ò=Œh#9ÃsıŠ½ÊéyOT±,[9!µ&{¬S_1$´Ô >¾¦Ïwú‘*-'HŒĞ(Š¥âNÄĞlVÁa!JĞ{zí÷çh–X¾Cãm'M’â‚Zìˆ©õšŞØÅInpL27ä&87^Vc%ù@Ë¤÷BZÑ–“8.nÆw5!}‡Gİ	sÊ×õ¥R!‹y3‡"¶@Æ²Ñé@£*V”Er”éTÜZOĞS·G®ÅìŒ»àµ»ı‘SõjìYqÏÈ…§ÀÂú&ìe6MËEÄ¤®hû!íQkÕ¤\KÈzoO<ñÉı@Uâ•x¸aT¢¼Ş"›”Es=™®@Üş«nö$_¶<)¹	ªh”ßj(Ì®¦"9²|“ª¹ÇŞ¢x'ícŸ=âüÛE•Y­rBàÔl3*`Ÿ2ZŒ&Dİ<H¸Ôı £5ó2H[ó&VaâVÿT~®kısêx¢5Ú—^2ÚrŸ§‡';DÙ1ÿÇFíâŸ§;Ë*?>±ÅEÆG.¥$_åiXÖ›ÀÑ%Eñîü@ôîWF–ê+O`C˜ä<K{B™gïsÄ	*
J&¹iûgÒ‚ê…N:ºdİ²Âx°¸SàPá\9
°-µ‡jp…È…Ç±òÚ€Ql¡^Ä|@ŒA"[ŠèˆvFŸhnpà$±2ß7 RµÛíÙŒrÛ1POÈ	Ëø©Ä<Øœa)0Æ;»WæB‰[‚t$	™¶İ›
KØ³¥Y™Dg(‘Øøoµ2×mÈKà5«Î‚*—´Š­Òµï9ˆô‹”ÇH;¯ûGÙù?ï‘ô–ĞóîyëÈåŒ0_·SxğK»fÿ÷¹óùÿ¼´[ËÇ%ıs×†L±@M8¶3†zµ“æ«*míÔÛáÛt3ºç‰û@	/¼=vØŒ+[ş?r’ì£»Ón÷Æ‘¨³Îi«„8‘ ó’İÕ§ù‡"cÜ@‰7ƒmX÷|T —/İZÛÕ¼Ë‘^ÎğN"ñì+ç­”=&	,-¨¶´xèÀ¹ÿÄÒ,~7\µCúë}jP08¢ëı¯š2í&+–&'Á>›;œü½/pî7#JòdNÃeù5€¨Lfãåæêˆaš¼,ø½ó~?ü|²">•Dk©ùRù×òy.Ú\Uà'FÁe½òÊmFgà½cF78puâDKtAÿl|u2nÃƒçÌbÌ‚ºì³¹Ğô&iU”wfK,OQšBğrä"ß”Diê¢óKœ™ i=šì×[¹ò[k
 EE§älZ+~†Ğ“ˆİ¿Ì‹ªoÉg7².¸…Û•ù¯IÇ9¨”MbûÖú­ñ©Tnü&êbˆ­O‹ĞBDTd‰6Ù‡Œ†ªsşî+H›ä>ÕİØT^şñ%Çk YšÚ‘±È;h¼Øa‰YwG¶~Æí¸£äXNÔ—K_]n²t=©ÅâëSzhÆ~×>«‡}crqÇmqf:ø0Ö@¥ÆBl5%!¬ÃÜåò·“›µjH0Cäš?ÄjÂ8B†Z¥ãóè™4B¬PÑ{0(c:”Hn5³•‡/OJ˜TuÍ¤ 7ôø‹±ùAöãBæ^ºÖPTuØbÇfˆÕ†`óiwJŞÀs¶¿æ|¦¦Î‰òì=*¸@sà·¹0›¾JæõÁ¡æp†àùèƒ¢{QMÈ´°’u°ÛÈi;6Ò×Bœ%SçïĞü‰¨¨¡£î“H)- Ó R+ºŞZ2a1Ê{ƒ—V>¼0%÷™²É$XìäğFU?‘÷uÔœsŒ,t¢î+ ŞıA†?dğ‘†ÀŸ‡o“À×¨ˆÎc7¾µ¸ë	‰AFàËSq¦êŞ“ê]]`²—Íéu8
ø˜a‚9¹h|AQXŠGU¡	Ÿ¢úV-&:¿.¤¹\Âˆà5â¯óİ3‚¯û[™_¤ \]é]¹ùóÓ-ƒ~÷w› rõâˆ».Ük=¡UD=‡@Š{½¦k%Ï¼š³…H[i|Qj›ãTÉÛÌİQ×–’¸şµÇIß]™ÖJ®ÿéùaşè£G‘ø…ÅË_O²ÔGŒ±X÷Ï›mLn»%¼c4I#ò<¿Kã	Üq¸¯Ò4V½Œrëñ,ã‰)ÕÈÊ,£ëµ&¦+§Ğ¹ÃêJo	¿°%†×´*˜7è	ÍæKö’¥Ôv¨ÄZn÷ÆµÓ¿\ò*LÔ½ûÀ™}Zò{ü¨qTÂ_Ñ2Gˆ~?’Â“–TcG¨V8Ø´öPÿìètÀû”ï2¶×ê¹7KÖ.5aŞİ‡0Ã,ƒ’tec®¹åÛ†lŒu"YöàFá¯§?r‚/³Q,
ş˜ã‹0š’}µjXš”™ì&ZûÅ;€–ã ±Q’æôj<,aÈ„¢Jù%*¯”1›ú‰Q2›²º.00-ÊánŸ»1Ôú`º çbKõ€! Á"¬½~RÈŒËEÇjø£Àq¢x¥F¶€ZÓGü!cUyØ|ÂqĞR¡…”ãZê¦Ùg¥~ÇŠO=¨IÀ±êÌ‹r‹İNõBœ5•XyÉmrüœ7Å!İÎø‹?`CØ°—>Î*  û¸òİÎVî€Èq
°Ó0š5—‹ªNY‰ÍrÊan¯0ÌKÜàùA¿{
ãz°QúÑÓ èŸvÜwmƒQ¦W€©	Y¸_Ë™\ä]–7Nqğ,y(÷.÷ò¤%¢
Êû<Ğ[ü’VĞ|et[¾RÆÉÒËÊ}­†U-ÑCË³iİŸ·E'á’¡@f$Í*’IMGrI~løa¶ÙîÚğ}ÃšmCdF9ø/µ€±ú©ÅêÜ¨J"P‰¨j`5iÏNMµb3à{nrÖ£ü!™œ÷‹’Ï’»R•êhUyÃjÇ1¯TÍT‘ùîLÁ7Ò
rQµ#šy£‚[zq"×Ã/$vöÚR.ªG“©$¢I˜ØEñIÚpU[…!òEtQhÀ€f
‹3½m_l?	Q†ô'VšÆ:“ğò¿BAœ€4áƒ~ÿ‰ı.ˆ(\·?ãÖŠˆµõ@?zT8¼[ÏûŒPì)0«â`ä¸¸Ÿ’ìâ[u×‚É’Ûù}†ƒ"]™o/óKß°tên*åÊ{hP–4WX-º„£?}àŞ¤^¬[WFĞñé×KÔq¨Îq’¦&ÁgƒRXŒ¤mÙ¾˜©(dM6(L;Š_æ>iŸéš†?GÁ…­š`pˆ#^©³q4t$m¶˜›´Ô·¤rô±)&%‚ÿ «íE@•Ûš4È Lõ¾&ëıŠX2ÒZ×áo:Z[ü#¾³İ…ªº‘“7<Ëå	ŞĞe<|P\nØä®™_W¥™ ˜G÷MhHXĞx=¤é+2zäÀæĞu@jíØ-aA‰Œ‚x‚(4OIËº+6I„&•ùe`×ÅÔ£R,ÂãDğ“Ô¼ÒÏD'¤¿X‹ŠmÆÊ»³À)—ã/”†=>¼ehnÉËó,µ3ÇÇ‡Ğ†YìH–#2¿jêa°ÇœÙÊ1†MÁ«QÓ¬ÚæÂiuFĞ6[26}Ãb.ã4pñc’|ªâal¼u¢†÷z1Ê™¶ª4zuóÈu'kÈ	—-1H!ò?>2âÙKËóñ]¨ğEîOT—¸‘g{»–2Ò9@Û¨pê½%]m£+›l <¢@HtŞ*
ØX#¼ë²{ğ»|b 4!˜”—7ŞùºBÎoc* b_wRäGãS7aò¯U–J†Ûnõï9‘1Ô3°$–SáXI\‘™G}z·CtQOÓ»PÇ©7‚)äø":?w¸@º×XÇOhôİ„¯xÇLÛ° Fµ}	K÷‚¨›ñ6iM^ôJS¶@ƒ&œÜöt~}hĞch||fIG)ÉÈLv'eh¡Æ¬çÇL)`§¡”ê˜¥‡B)\ä~şÉ$}ÿ©V6t¶Kß/Iµg¯ÍñMqŸÅSàïÇu%?‚ğK
\mœô‡ìÉ+·k
şO$ÂÙ<›úBbxŠ¿(5ÇÿÛ=·9UÊºµ!ûâ1¥/]ñ-0A¶Ò»P8#Dº4sÁŞRÕ½B¹ä…¥Ÿ^„ÈKãÖ×¿(á|O,h"¥:ÒÆ‡L¸tøÍnt§€€=VÃæ';Õú*õ«»7öEı:²º`|ïïÀ(átB¹A»SPtI](ŠÊJlGë@ÁĞ†Œóå-4™EÖ‡ĞHê £Lcktkqc~ºòWT4‘¹(Iîšè†‡½ƒš§d]»Áy"kº'N‡É¬¾÷f<”óP}Ã~ŠüW¹”Š‰‘@­º‚…å =èÂ&,óNY«OÄÛê´H@S>¨ÈÌÒÆñö»´¢¬ú‰³j{Ù)Y%!ÚA'9‰ÈrÏİé3ˆ°‰¥Ä£òÇ%÷ŞJàé¼«§éµè–ñ($^ã„vbÏ¶ü8¯-9hôÕ—ò™éˆ-Ô[MT
¦šÅÛU_ó†7dö‰¾¤§,ÎH÷íbn'`»¬ÕsB%Õì³|¼™Z=.j¶¶©‹blÃ¬\Â“¨ƒÖ$‰ÖXÜWÿÿ}?&ˆµ½çâ¡ÆAüãPúùA2’=ÑÉ|ó¦¹9°…®Z$…kJÁÑ£2Aä¡ààùI»mÿÙä-Ô)ÏÍN“UyÖÕ
Z!VÿƒR(ˆ]ŠQT%ì·W2fÓ¦	Š’^LÜÿ¤oB@ºò2o_$ßÂÿrn¶â	:ÜÂÖÂopÑ#õˆZã4nôNşm'œ†$óMŸ
ØÕ#–jµPóümöa b7iø:™&–õRd4Û~Ê×¤>Ÿ"AÂ3k!ŞéNhB’f¢B;…P7MmÂ„’ÀVë¬€E\D`3á¯’—làƒ#Hh>:=]o™˜‚#@¤m’ ®Ñá1C4Åtd9|ñæÑl³x±Ô&<úvóÔxWÀñDBÙ çŸså4pCë™Õ8˜{yLÖã—êííãÛDcÈu”újQ.¨8ÚÇLÕ¯o'÷ùâ@8TŠ„ŸËG¦á5›QŸeFjÔ…|Œ.È	õGa(æYŒ	`9V~Eğe„Óú¢øçüö²¯@kN,c±lù,6¸hÙJggù*>:ü‡»Ek5¯O™SMş$È7¦ìowÈÀÒ…¿øEÄÃ©›ÎŞj¤	ˆ?®gHÇĞ…wÏ>Vï-EÕÏƒ*º$‚ùeV0Z=Ë¡bĞjÖp–î’2=ôË,Sk"àW=2²ãT"b~Eo.—;®6Qä"ù9,^î£ÚG¡ıYPœŒ·Qz•Ã;0P_<2§,|Ô0Ã–F}Î¼C¬±ÀÖˆ™jôÅqOÄ•‘´;bM\”]8†KFø¢šV,¤|xÏ@èpì
×—LyŒö¿ƒaóğÁùPşKÖvßd³
‚º2Œ4eY§ tœÈc4ãÆ9Nëà®-Ô.Á{ó¿-3u q{ƒ¥¿ÎîcÔu‹Uö÷XÏï	Ù½·@Aç
AIc`İo•ÀK‹Ç>SG"c#on’,…©ÿ±1sùü‡mØn†(!~òŠ®SØÎªÀ²êÔ#V l¯)Ê¬ŠáOV[ØÄüÈRU•l¯Şnƒ›e6A£l}ç'u<t
?¢Êh)Y¿"ûäªuàéy,Àó]ÉiFspÎªxJ1tçÎ6ç/äadIxKÒ–ëu	‡´ÃíS=Ñô#4®À:ö]~j:
(«Õë¢×—Wş’Ãp#Ut\‚å|)•áİ cR§‘©¦ê"ï·ÿU2ÖÜãúCæls+q1UÄ.e^Î£bí}n5íU/¢«Y½‰ã™DH:5C›{#Â
¾¦yôùÔ Bh	ÅèôÀW´ú=5œûŠkÖPÚï<Ê5“xa	¥ƒYSm'TòŒ@LÄ“Ë	‡½¬™•iK»º%8ù ³Á{•ƒ*ˆî>÷<w‘l•ŞZY¾¼OåD˜ÔŠqUu:^ËéÌœVEæQ£%“†Ám1ÇŸ…qø {:z#cÏTiUÏ}) ç<M”.B_ 3JÖBÿ½xµËŞø˜¶i¯PlÀCM½;²pé¢XH‚JéøF&ı^eíXS1)ú±Î–³a¹nÀ[y\´¤ÇlNÃ¼.¸)dqĞu¦1Ş`¨ÄAÃ¿ÛÛ<0‚@åP/€:·Ïp¶¯Ş­E«ıİ æiı„33ƒwk¸8¶Õ"Š]XÕ¢ëŞ$¹Ì ßĞ}d 7€e¡aÊœ°alº¯Ÿ¼ÆO´¢ªók×«go32jn ¾ØA‰¬s—z(¥[a”7Ñ°•cW6ÓYÏì]ˆ¯Y˜},:u÷RN·³)ü·p²B	ÎW›‚*g©YÉ˜µéW€D¦²RÔØÕ]D,nŠJÊÀq—5–¹ÙOåÉ×UB`#Ç&Ibm<l8ÍûZ×ÜJ/L)g;Í§Zë¦/MÆ+N·K%qb?×-Rn»¡k‘\#<”E®ìhõ±åùeŸà=â‘óTf-€vÃª~C=m2ªn)Ğc¤_ÜÎY K¥ršomÀU»£:±Fû®ñ–ıÌÔ«+¤àí™A[=¹ö/×<# D9"µDtÒU+/ÈÒ7Øjk’[×9Xàƒ|Ô[<ßôëiëÇ	™ñØ9Ò	o€
j]•fzÚ©FÄv¾*ú IQˆÅÜ)\ÃœGD!Šãê;4ôaA«Äe©ù³¾YóX[°÷ÚÜåR5†sVÍãéºÒV%­›[o|yñ¾š–gø^jİµ$|™Ğó¼Ä³*¬¿y2¢ƒ¤3^SÀ¸fÍIˆ¿$•üà×›¢*e„ÄèzÚŸK^ĞTÚÏ²Z¦ÿú–0÷üP¦—ú¨ÛÿbLJõE"P'šcç®«¦<Æë´û;t*ØÆïÌfïÍö6‚â—È†ívÏ”öÄ³0øs³ÿoŞWáóP³³‡a0ğöîŸ+[ˆ`VmO‰ö¾F‘IÆğvğb%;¯T·fg†®a~Â¯»ÑÌòƒ¬®pï@wÃ¨7$¸èz–#Å‰zqcômL}üğ"æ[£Ôs›’qæßm˜H±ğ±ø“é¶g)G9®¾Û)¾ş²Á˜ÑˆÉÙ€äCñp¸©cêñfØ?!Â‰¹M]ÿ\¥Ê¼0aœ¸˜p³š§W¾oâ¸ÿrØZ£2$<öá%mË¿òê„w# {…DÅ£Ê2ooCºW¾¦k”±)/—­I:¶Ê¤¢\Æ’’Âlr¶mÒ”‰ÙAyvŒ(f²î@èÁİü #%mu,^*ß=2(…ôõòOÎËhŸ•¡ü" Çúå^Kßïø>xq=ûßãhd8c€õA@ÅöŸ°v¸­­9\ğñê(mı5;[€röÛn!Ío3`àÕè1R×Ï«)Q3!…3Ó»EU„ÜOaÒCë¿)FÈAëC5ì_@S 1–7eDMî¦Â~)ĞU¶ŸHƒ™H:®R¦7#¬Áğgcêé›ïå…˜¸ßÄ¢k¥ù‡Vñ¬0w>¶]VãsêyÉ*ßŞÃ}1>A$š@1ÌzºÉD']¦ë³’k·-hK§çÂI`‹ĞYM/]]
hÚäÒL¹=lƒ-–â÷İ@ñj¼ñ½ù8ªÇôíë4qnwçé#9Ë4É7Ó:ÿâ¬ê#}‘Ö%IÚgˆ`¯ Så´G"°M­•X¦ÃƒFvgK^ò³š„<rWRúÿhIÒ7¸Ö½–[ªJ6WÂÂ=—ÏÖÛ¦Ì™)ÆŠ3Ãó’+#¸c!¾ÒÛSSŸ¼òHñ²I73œjqÌú>¾^«¢_·åùb)"=ƒO¿umğµ*ï
OÙşXXâ¢áuÑ¹ÏyKvğkí9ğ¼ê®£¥5ĞGózújfRÿîÂsI™‰ı"Ş:¯»,ÙŒñ¿äÇ ÄA!B—ÁøáK|ÈU6àÙ¡œ£G#˜Ëj¢ ‘L$ŠÍ­‚24hûÅÚÁB3¯›ªÓÖé…)4İ»>Í':ÈóÖú"£4¾ G­flwú…©Ÿ6 ¦ú*ékµ§sŠºpÿû+èfoû`g¡o5caxd–gZ$Ş;ŸgDêÁH3cbÓËºŞÑş·„ÊüÀÊ¸i–ÑN6%IfˆšdŒ	ls;.;ğ<!@¦*µAß‰ÀĞÌ±w¡ïĞ6)?w‰Æ2îIwPôYAÌŞ°µ®V8¤õá-5¾è–Ş*í!‡V˜¶–Ù •p$[‚!b¥m%”„© èOZ’	|¥lŞ­û’ÙzıÜ}Ï…» $0ÉÅ}k‹ôqxp¨¨TÏfä»Ïõ6‡-BN›(sÁáââ%°†ùùĞ¸G%p;9¥hMâ((ÍF(¨,Ÿ³…+|,BkDÿ Ï7ì¬ê"aÂûjIîÿï4Ê «¤>{c´ŠE¦÷R=iÙôb¸ŒÜ…[Ş/È@°P_Ÿ}¢°J{h†i1x¤xÎ¬
]CLõó°±®³V/l,Le¹™µ¬ÏH:ØšÏsŸR¾m-a1; ËC¸º+zô½{’XCö_®ğcŞ©™o®¼%®|§«4mvş‹jğoÇ¥“ÜåœìiHÚšz²•¬•zº`6˜yqR/
ğ<¼¬Z·™Ä¦¡3£gğœTmº­ÃFfªÈÏÏÈl@(FxcQÑ»±n‡°qç
IÇ•ÿ‚‚¼)–f×§‡GÜÏ‡U± ÎG¸=éT)e˜&èÑÿ…Åmg[5„°¿'-º7&yVë)Ù3qm°>–úŸ û]ÒI·òj7á€7²‚”ètSôÛ¿Q’IÑ¯åî"B·E†Ü„½I®,,#E‡0Ï‰¤Ìò,²Yá¼OııvÅ=ÄµÓë†© oúKğüŒB:k¬”Ù(wt¾b“€*€c
›Ûºy@]I¡î4#ÇKZ¶Õ—±ÍğÑ›IChî…EL]Øğòn‡f5kÎıÃ]Çœi¯dsb&Rÿ¥òDìñ@‰¤å$tQıæ«X×£BÁ	â¼Î‡s<\?™Ö(»c U4CØmm&Ëû§»½òV{,5ÍïEalÆqYVÓ²¥‹ÌƒË_ïØÜ»Ô†•ñÇõ2JÇ_ë$Ù_ÉwğØmD0U/›aÑ“IÍ
Gƒ‚\ÂEKb´K«¯•Ø<nîŞÊU4íFuM Éù§:ÿ›…4ş4SRç¯ô€=ó?¸ERÏ>º´ÑçöÃm™İ0ÿF„—+EZÙ¥`Mr¹…+„<(ºœ¢†±Qszÿk“\Àó‘é˜Ù</~9*?i:÷-òln\yIìÜyI¡@¯Óà¹ùT&4=R(Å~aÈÓL–ò­A«](©#©¦ÒÜøÑûjCP£2lXùĞÉ=tíg’ĞùëáEåEË0½L1L%­Ü‹É‰u=£-íF1UyÒ¨;¦®Î.À«&?N	ñö®®.Ç3W~X”nL:[eÅ?ßæY@ß3R¬hƒ¿ q}8g oò.(æ_\ öå‚_*šßhFœ* :©8’"Ë7&ÏjÖaD¢†ö)e05­ÂÚğ÷ü?¯cIÿ1Ü7n‘'vaN_' 
êz+u„"=r°ît˜•'ÎñW$†â	üáUºPq+õÏ€Õê©W¨S€¦U@­/…ÇAúòªd3¢Šø Zéxì@Ë½ñØ{7b¬{¥DKÿ¹ïı®]–c\g8a½#‚ƒ«ŸŒ;#ÃàìÚğ&{Ç‰Á˜Nù‘’\¤]+€[a¶wÓWşN‘!8WÈôıÚì{B;ìF¿[¤5>¼ã5çšüˆˆ-¬Æ’ «´¿ÕéÌ•s\±[_wİ;Ô8²iB"ğÑ——¯bvÕAœ?İÃİµ‚Gò
Ø >J¥”ê¬ÙµwxíDšVd,ÖóI˜=°±´òpŠ­Ä—x%şGÓ/X7U7¥¥UÌÅlz4¯¡"fÃ%ÿ%£jLÄ€×ÍeUf¹FR5‹åVÑ/5o7¿©@ØEØÖ±‚§)œ~ëv‚R”…6Jé
öo[-¹bÎP;.¨ÑPöt¦|<k¢	ñLo³RÃ\m>ÕÔTÄ8ßŞöL“¦	àÑŒø_eÑÈ˜[S¸ª³^Fsû“ÏĞ×1Ğ1zFLk3Ş®¡U™j8/Ykååàë“LQûJ\¢]U#ÓS>i’¡aI» õ·Ì_hü!1ZÉ’@Wj¯Õ"<Ã`E0Ó›$°õ9›g„jôr ÁO­æfÅ§t¯Ô(HSÀõ¸n8ˆ*óE¨hu :Â¡7©cÚú4€¥H+Ö‡Á"Æ¡6p©
Úe ƒÀv3‹øi6÷ô’¢!¹Îî¢ç^ö§eŠN8‘>ÙJî´~d©E4o‘Ã­Ÿ¨/.úƒåüÒ)°8õıD7ÕüD`HÆúGç«®Ø'ùcêS¦üØ<ĞåèÖ¾o»²5*•Ø.J¾R©hórPÕ%¬¿¹Rk¤|îhƒë{ëê›õúÚ,/øI {Lªc‰/b·gwIÃßµ\¶Ešn8:§·vÙ0îŠ?/î›ğ¦ğ€Qld7øÔ„…§v“›£¡Oû56'ƒ9x1B•.†ĞÑ/¦†eBHeëiÌÎb+Pà—½ÿ<Ú‘$ŞÁû_2xY)¤µcÂ½êİ«3ØÒ„GYL%ÓYrŠU‡„Ñğe-Ìó'6¥ª']sÇI_˜õ+:øÍ¥>Ï¢7†ıÖàËGD6ú=Ö”w‡{OA§½ïüª{ÃÒ™€‰s5¯ß…Øä$3û§t&‚Ò7Ù
ÅBö`ZœÁr>‚EïMQİ3Ç›ñÖô5c‰–ÿ&v?Ä=`ò>tÑÍFO®gì–ó¹?¸DÆZ¿r»UYäé­ z!Ğ%	-l«Ês•÷Ş1¦ã
ûyä—òŒ‚şs¥$L¢§à¼áïĞ4Ú±ù¶è)œ÷£¸¤êÏŸÖcü^ÜşÓİFw‡Ãg¯XhU©>“¼	*Q$‹&¨1l†  4Ó])Û—nå|KwÿÅ	+¾ÂÔ,Ô/Ã\^1ˆõ£ë?dƒ.Æş2au*C¤ğ:ëó”KWÛËcCeçø‚ÅÖo_ÚDlrğ>RŸqE¾–>Õ¥Fd0ÉÙE‹Ã—U²uˆ^’®XKœ—µí_-Ü>ÉùĞs7l¼‚áŞ‘î9d“Ë3`\+qğÑ¼¿r€0zõú ’í”Læ$&ĞçRŸ¾Rsc•¬ÎôÇÇöíVxo†LÌ|  Ì <->ªÕÊ Ìâëé„ôÃsø
Çô2¼öhb¡ ¾•ÄÄä]šiR,h?aõLíó’‹ZVQØ8Ã¸ûgJÉ¢é&Iãfƒƒ½Yàˆ©Ò;4 ”¥Œ–V¡
ôQ†á¢”@¡Ö½èŒìäÜáå0sÙ),d»91;}7µx^zÏjÇ"v«îƒíèi«LÇ»cñÆ™fşåc1_èîK4¿vOÔv§şÕ¥Ùµû8s«4zzUáiè«vÀàÈÂò¥GÏß¡úÅ6Íux4ÓCŒûH"?\Æf•(IÔKüôW`Â‡Üi·"+CÇxïw×ÃÖ²µ"ÔÂ¸û|öü!°ÌCúëáw'tC9s¯çÒì‚âS±¤—§ Ç'îË ã(›ùôÉt‚+zó>Í N`·ˆ”ä3fê9ušÔ=c—f_ÏÔøÃ;,døú”uE<ç­¿x-2‰ê§TÔ†ÕW
ğœˆB¶¦­,èn%S!:jiàaÊö`ç™WÀÓÏ"GA^)u±¡Ğ0ëk¯‡]u­gbƒñ°ÿ LêÌÔ:Z~è¸Nû[=éêäO°˜´"³Z0U²	Ò.ÿX|¾¹æ»&ˆ8d©9bÚ´œøÈJc|Ÿ‡PL0Ø4&V6‡?f	×¹üÇõuÌ¢ú¯×#ˆ1²„DÚÁöÌŸ«5±/XEöbvüÂBëZ˜BLÀ·ákp¶1ˆìÆà«PTş-Õóàƒi°kU|©€Á‹fÇõæ2v…öñêô!ÃhåáØ†éA
T6ÈÜRÑ¶êrá~hÛu3Haïœ_ğÌËêõ†wG{2.(M­ofv”íÜhM¼%(h$1¢á– ş5VéÁd35JÿZÃ<Ş\»¾jğü¦ã—tü©’	¶­‚¡LêÇKÌÍbXQ@ú„‰ÅÛŸ¸\íu@š¸…Ç¬÷Bôàú!
K•oşågºÄBFAgÙ\W0Tz…H›óŒ6çk³mÜ\¥Ñ×ÖGò8ù„»ò—nì7eı½ZràÉk`Ç„É¡hÇjÁARğı»~åmëöı¥ŒSCüCÑıœ‰Kd5ªŒ˜H³Õ™Æ‹Íù‹è/úadğæû²0Nb2İİ9„…®z«Ë7v[šá{ïƒÏYx¡ÕÆ*Gâu£øwo<yúÆ~×{›Cr”ôãqÑheËü2û{H$
3(oAE†í}Üù¿¸Sõ/95Å|›Y0.¾ªe{Gr:O÷;O"‚™ºsÒ!‰Gc¥’¬#;[ÿÖP†%ÿ‰“®‰ÿg<ã›|ä!ÅÄ»§Íüò†êƒ+ÈV†n½Z"•×ÿr;=õkA*á2Tü»ĞIqíôçÙ¾m²¾ø¸Îj¡D=ç1‘Qz'LR8hZöJ°%‡>Ø€0o“öïaB'¨G`¡.?Q”…˜À©’H5ÂJ#àÆˆ 5ã£ˆSÂn„U àüVV˜‹ˆ
M5ÖUO¤öå²[†}™;Œëãë ŒZ!Â?²Ğc-Xïÿ!tÊ‘…O¿H¤#Õt+î÷‘FH“­næÇ&ô_Ø°&Şc$¢Ìv:ÎL+ıƒ>ô„(Jo56İâÿÆÆ¡Æl/åà–|`¬†L¾KŸqÉ÷Åæç˜Ø€¨û€å+vt5]1ÈjØ>·ÂQní’÷#ùƒÑ,K'£÷”_7‘‡ÓRFŒF¥œXRÃØ@åÖš¯4©š>©Ğ•ÒC\²Zv©]ø²P¶%ïN9í÷ï„ĞÓ€8ÇÂ—UĞJì®‘ÜÕ0ü»¤4µR•Mü`—ÍGHøù4ÔA“£’Ò1jbOX(±!Ù)¦R¬‚©''ÖÎìù¾úº®bzK§-&TÉÛª~ÃA$8l:#PÏòoÕŒ/ÚrÔÛˆºî:aŸªMÂN!¦TV(‘CË†4·E'áìç%ñ½ñIü-:+íáz•f Ò7gF»Â¡<ÍJhO\Ö“É ò%ÚB¸¾*	O»Æêfå}oõbóP„|æİĞê…E. tîzrºâ î¿ĞbGÖ V8£…¶hL<>ÈNşâµÏ&¨×¸¬4fğ%›Ôëåô'`ƒÅ¸»ÿc¦ •Yrèşy)˜şÍ+ùÀ–1G«7£%8ğàNgÔ«°çìØ5HŠE
‡·Í¦¶\úÅ dÏÅÔ‰¢ïÌŒr¬VéÃxUôŞ«Q£~ôšED™‘XePÏ`¹k òAÊfàÆï5”¬ò1»“,ô>Í£<¨º­f6á£Ÿÿz¿JëÉFÏrÚÔKA1@n	›d…Äuİª#d.ÛW»2€"N•*<ğÈú•Ü”Ğ6ô­‡¶çŞÄ©YÏ<™Ëú2¶ãjøqà=0B2¶í®²ÁáUìpuâQ/@Ó»°8œïº<vÅ^íd&Õv²ğƒ~U±¶k¥ÀêW0Ğ…2‰½©i#Œ!I?íÃ/™‹‚T-1Á£ˆ—m«²ø‹6 ›¯ÿÆq‰ß°cégƒ±ZØr›&jÊâ¨Uj—`,Áê<m/£µ“í¨sN‹Lòä¥Ø×¼EÌT¥.lşWöfB¼¾ßoğ~¶*vÅLÏß0¥%(À ®Z–¨ë3Q¡œË4Ãw>½ƒ¸¡Në)×bä_YJ§ªÈ’	lBz±ßò^‹Úç…ÉàQ_*|éêàº'eèÆêõĞ}uõtÂ%ø±kLÜ"ú:.mënõ‰Ã§Åm©0µö™«ì»ãÇùcÊ+³ gªª;¿¨@~Ù<´f…iÂÍˆè]ğ#TrÏd$7ìÚ“Y½’&†.ê¨$áŠpSÃCÊı)õ¾¸´ /Îµ´ıÜ%p0_Î÷záØåtÌ¤å˜ıg,ñQ¹Uª2'j2Gzµy{·Ş?(¾Èk§RPÒî“öÜ¬ÖÇW{l’¢xS&|RÛ“*_h ëøcMƒqÇMfzŸ­5ß|`–ò÷‰úâ×U–lhHÿ‡‹x€2rd']c§ÑtH*µgÅ~Eö”§Q[İ¨DC/vÌ(O_‘ß¼ wkz6ªùtf;'·Gqã-¤fŒ«Fn/ƒ*p_&GáO¬D}ªÆs(’ĞñÂW€Œª€v×ó^“FÔ;F	«è°ƒû ü˜lÎèÌàt­ƒ¼UÇ/WÂ˜@ÎÈ› ­g*¬å¨8øvâ“'Î(î65Èmó^t«¡N.Ñ©aÇjÚ´,±|½€­-!ğnòÃùZn”?mËTéQ€.Pß$ò&ÑVÅ7ı=†QGoÊñ1xİH‰®EØøú3—ØAäi/Úø”EâE~tB¨Ç¼Oôó°ÇD(©µ²D>$YöåÉ–õˆ£Ğf[´Åˆü7K«úFÌ¬ª²Z[s‘ÏåÙ·¶cNRpù Ïqwm?äÏ™'nmZ	t¸Ñ¡ô¾/‘b›M'ıš¤Ô-÷&ê9‹´k°Ç5‰. ‹C‰³¢\ñTêw†¦‰]¡¯x	
I d|5NHÄªµãn ­ÜÕ(JK(„¦§V•…DŒ4ÌÅXÀV,±>™æ–Â(ı×[¥Şí©ÅGgß
å–n¼8°q/5‘ãv=á}¾w^Íd³ÎÕ=§æÉ˜Ø×eÌn¢Û‡Gğ'mcE™®d¢¯÷5Zy)9Òxa;A\v+‘…òRCRRá&9‹yjH‹íB\h:¨aófª‘zó0İ)åesäögÁ[—P£JZ•0ê]j"¹CS<ÑÓâÛ€”æ3ÍÖë(Î},pÃ²%£¢=¹&ûx»!ÁŞ£,zòNdÜÚï³ÿ¨Xÿ–v€UÓµİv¸ğh ÙÑ´âçBœ}^=*rø@Ş|²]z!EÃİ¢î4+Uoı²ââH|c±—ïU|+é¿•år‘J…Jşh1«ÿ«Æ^R¥6,“˜ı˜}’ıK¦İHvíq,ôeÅÒlÃM€s´$:~jÀkMû&ĞSM9xœOŠ‚îı p38›Ip?üøíZ†ªbÇk‰È²¾$ğÖ‹¡S!ÎJÛ ğ±™ÛÙ¬®Wj4–K‡'ÆºJ‘˜ğ…dQBÈæ;Y {NeÓ»‹ˆ"Y`7ËâÛM‰© r0S6¸)h®3U=•B´â]b›Gêæ4{LdïÛÎí’*¥™F«vôÊ_QÆP¦®óÌ·Àœ‰ÒIõœ.g¦PAØ°^Ùk"ŠğŞšëÊÌ¦”<ñ\ôB^<ZÁå9õ6	IùŠåKÙø®ğQüÚl›û£šµß±ÊÅ÷^±E	I9’Uh §H„GRú–-qùÉë•pØ•kÇ¦¹ÛV	 ÏñP¨Wi¯Eíá2ó}YJüÛNÒƒ–—cª‘gL¨:Ë‹n†ÀN:º»³¿áŒ¬÷¡SíŒ8ÒR3ÈúõF¦ôär9åOoó)ø‘¤ş­ UŒ=¶ôxg%yºğ«²Úşf††½O&á²˜-{»XCù'+s"šşv‘›(àõ…8³$n¤~#Q‰hîÅå› J_üc^/IF›ÀHi½‰ºyz³ü9šËy	´H_DôÓ¶§ ?ßåŞßnf­‰.’„VİkWnDòäÇ.EvhN¿2íîg»¹”`µå>ÛRÉàù…#a´ÒËÀò4&
©ízÙ™K‡8PH;ûŸŠøQĞ€uS¯nä
y—¹kN}¾:“oN””@úŠŠÚìÕ^A™“èvÀá™7ØDªóq˜ÕŞiSVqLb0u³D0á2[	º<·qÉŸæ,‡Ã\³#…\0O:ßàšœx!ğò¥+y\~Ó™õD’sî‡¶œğâz/ºINğñYô@6%"2¸N¬.¹Ÿ­Y×G…şNF¸a9Ø6|w<“£[ ¤±_	
ƒ[-âEœö»Œc,ìß8ö%´É¤yw¥³nUøo!]Å_¬çi’è•:U¢fAJˆYÉrÅy3JRİfğ®ãİ+Ùd$‹ráş$ò„øVaÀÊ?èjõU £?Í0ˆDçcdéêƒc3­9WÉ
å^qÊ·77&Tcºú‡_2~+Wğ!ª±jéKÔ:Ô…sÏø¶¢Åß45mbßl´â4&hZ®x¤|Œ¤CuyGŒ†<®«`TNÊ<'\'ßä!ó"9²YÿgJX&,;½`Û¾ø‡„Ê1šßÁ´(#â×Ahs€İ¾&úeYÕ4ı¨vì2$£ñú¸é\pv,ë^ÆôÆyğ	Ÿ‰/uÍ{›úí‚ÁÎUß%»«"NÇË»³Æ§Ì	vllŞÕÏ˜0H¤.8ä+vİÚS‚ ù§¹0Ğ'¢Ì¢8A'´âêv¢·jeá»®ĞÔ—|£¦¨“Ş˜_N1£‹sIåÂvÈˆnÛuà²LE¢Ì->úpXµ‘ğH	„H(ÄEu`6Ï°Ã…¦|òæÑç"¿ T	ÌD.Ğ5|Ì²©(óã€g+ëó¦ut§løv
şPı5„z°“Ì¹³šš3ı5dë ñÒ¨=8ŠÃ¾—úªlJMSJÌÙA6È? p™xV#zaõÒ{ïòP¶n-è‡¢˜<Õ³[‹Åà±g¬jç•Uóù¬$Áá g¯ÃnX¸¡‰Œsl•ğßÄÄYâ’)D»ñ"[ÏìFG¥IÀ×{z·T× Í×·ùßUOyzr¸!ÕEˆn-9]/ÉOwÎïzI>íDF™:³qmÀBû t(ğğ½	îùã~ÛÉj—gÊ%K’iXNÅ°‚ NZÈÈÉIÃ­k“»vÖYsI»’)?>ıIÂ]ä•g\¬áUl`
ÃŠ¾‘ØŸ´Ò@_( õo™ÂìË-©$ùPNT3¿;™ê‹k*K«e-Î³KäÇ>Z7Ò v²Ä¨mœğ+şÉ–z±õ“Wİà€Û¨ş]®Q0WFj/Êš‘Ù?ÎPD×2UCo0fÊq²NõPˆï€aÃ×_ÜÄ%ºMT
—(VÕQc»º(›ìÎÇìp1àV…\ÍÖ›õhÒ°koö®n‰ååßwÑ1}!Ò_ÉÁ<[5sğ™€ø*ó‰úˆŞ.|èÇY8§âÕ‚ ®ºU˜	ä‘ à¦QãWŠñn"ìB‚¡çËlZr„š¤‘º:î^Ñ®ùÓ%EÔ“ÅhŸ·;vE›\ˆ¦sZÛuÙ÷ÔH<İ@á1R.å’TÊdJğRf_M(ÏéÉOUf¤±7Ö7 Sf„<`(1p¡˜ÿ(Ùç¿¦äcßXë©T™Ç¤ÂcdÜ‰òš<]¯¸#0òÂùä²bÅ³"…"ÈáÁûyÕ»)mş«•hIl*pï½sÇÓ›•®·™5cj—DOç˜k7)÷şw[Ë¸ÄCï+ä˜~Üf&%AœSX
ı¨¶Ó=ÕÃµ§YE­Ëa/±µ¡NK“ş¦è›¯5–VvŸµ¸Às£H3°ãx(²*ÔP+İÒf&¢KhP{Ğ8¹­ü w•æQ…Ğ8#–­±FHì¬;¬’ÿ&KR“·$Î+D~‡|ù¡¨`PI‘c6²¼vpşj—Bv&ù„_¡Â3U•§aÏ~ï§>J·öŞ€FŠı$º5âxòŠaYöÈ½}gä’	»Àg[Ÿe<|kQ>Ä¹šª›J¬?¯è"ğÕe$é¸ú´Gbc5²MrQ†Pƒ—‘?~ÑUÃ¤Õr:K[q‡¨ÎA(K¬s2]§Í‚ÀùJÏèç+ÚpJZçaŒëü™àï†–-5Q3UANkñ—¹ZŠ’!Tı[OK°ó n4v6ú˜/ªÈCê¸õAâ/{’ å~…vV¡70!*@Ì¹ğrŒ¸ˆâzÅj?/÷æ:šÌOÒèŠÅ.ªÅÙ˜ÇÖí¨ÏGÙ5c6E¾£2Q•”Ä¤-+À+t^Pd“Ö³3/#Ãc¢©†{¿Øbµ¡7{_“ê×$â›E«—(´(áÇ´JP¬&&\%g›7(ºÙBóáõ(ËuR^~şHéöÓ£öÂ1£F©8]wÒÉ‰3©êFÌ°Ç„ ›ı7-xhÕ5ÖğçúóoaFŒØ–Väb0ä=Ó±º‘Ú€‹ÂNZÂ
­6ÇÖ’‡º«Ç™ş/$Æ«W¢:œ´wör’7Î­…ˆ¤ºÕK†Û%Ü¶Ş9÷¹rÛ…¾è?ÇH4{à AyTlf]U`ÇÏÛÀCX±^„Ç¯â³ÛÚÏ³øäƒz\±ˆfÊ‹§[ÏE#Æÿ˜ï¹½é„D3îê$¸_ıjÊczè4é‹«Ãù»Ñö`ş2ÆŠóEÔw[9`èçd‰ô/ŒÛUF†¿óŞ_‰õèug9n¢ù¥WÌxú5ˆi¸³vúÜóó®À†Ğù¡‡rÕ
iE½§Uì:¸”Í@Mï	€¶z™ ²UI²/uµ gÎ©÷zUqI¨çdjEf¸(1KÀÖ¨rĞì+rüxRHÈ°û4‚-nå˜úÜšqÈÖ’xlÁ7Ÿã­Ë˜7üZê('ƒËÉKs#¥ìæª÷Ù5;+'ÃG—Š×Îlø"£:"¥¼…zá•Å_6ÚKÊù³X\ğ±ÁÔPƒ	A‹ÂÜÁàeªœ—şï*5\k…ˆÏ>¥ +Ša~Ì,;-ÃxX&IÊs~;Ùñ{tÜ_ö’l‡j†G€.²³ÆİOãù—a"ab.ÌiËQÈ. Ş”]=MYF–•ªÌ†Š§jBi@B¸—)Ğ½e*`„ÿ†p‰z=^®$1H5:µv6>U ç‚)}%‡ä½³Ê4;dÚÙ˜÷hp#ÔMÄ(#:9«ÚNÅ­qÌæaè¦OKHï§²HEpi¨œ+½´òlÑÂîó¯,*•PÉ9‰dÊ+è-Ë„25V,¶'åk•rïZ>vèü¬OêêZ¯[{'ÎÁ_j‚lMujvÚqNOê2ÚM­	v[Hı
<Ã<µW©³^©5>Gg4¿2 *[ªèÇ Ü:”›h«Uã}Ö³)o!ÊIyJ”Ê6&¸zPv¿õ¼@9Éøé”Åñ óß	»ˆZè”¼ÆÉgn!.—ÎÂ›»_¬|¬H5ñˆÂ³‡ÁÛ…™•^?·÷™çŞ§%¶ñ%Ä±u~,¸xÄ	ßm›µö½şš|.›õ†?b@Òûãšƒ‘i6ğ%6"bB¢“êübtp–,àM/¶—cQãÙ¿¢ë÷!$·Í’2=œğ'ğK«8!sÅi™ZÁÅ	bdÒ±+úª:ĞãK«q'®é”ø7Ó›bÃƒ‹Lfve—FhÈvôØ2«_ÿ1ğ;Øsï /‚oK¥£*l~²¤e´Ïó—»8‡
‡Q‡â:Ùı–+èå:ew(YõæC„æ‰N
¥ş3Kº¬“VEéDr—ûF`È>5'~Í×2İN¹½-zåù·êtÙ<ÁnWT]53í!Å~<àLÖ	E^ø*
Í}@H‚ËSÆó¢­¬Åµ˜„2Tı©wJ‡TqšŞóò¢ü‰(B¢æ£^/}À¡F¿#CaìåJƒ³¼Dœı"¹h	¿¯yØYo)¡…ÉêcØ&­À©ÌOOR
g€’wß¾‹ãA§ÖQ}íW	½€ÓYpÉÊ@xöÕÒSnáO.YPßt)óÄ£»h©ĞôÄºôp¢ÎÑ4,3îğE•—4öPt\É s¼¦ªÄm#ê8QàÄ9ø65–¢™„ø89hTîä•ÏKXf¿c½W ¾³[átZ¹zò½¤DP)°ùí6yšævêĞµúÉ?èk™së²®ĞŸz#>ó˜:àçe§~‡`rD¶6Ë‹†¶%oYi“´íI§ÀñX¤G#en@C“‰³Q4ôûÍúêdì >öËuwK‡[o]'Ç=^ÖÀ1³	M/ºn®J^ıaûe^Æ  ›±öR¦\‰;ÚÄÜ7ê-Y`U+eìÂÊ'­3¾ºÎB÷WÎ­)ÌÍgÓOáÇÑ¯x–W›á0V.ÃJW~@œà>2"	YXÜ.°ñ¶¦Ê—éTjpÉÌZÆÄò2°Hc”ÖN‚ÂáÜ™Ÿ•	pû•	úÏ-òµŸhT_úŸ-²ÁˆSÕ¢ÿÄq#›¼·ıªµKâÎp'4:ƒæm‹mú¾w¨ÎU^áœ¨ªë÷‹:ì¥ÚDö¼OËçs&+OÅêºÍåˆYŸÁë
º$('Ñ;
NuÓ"U÷<7Òê‚b]iûüĞoú¬t¹U„.P¤Öùßá®{PO6Şl¯µp/7 Í(øîr3´ú¼ QUŞ™\4Iÿô˜›ÑRÿĞ$Ş§ïr‘ñ;”ÑszŠc	£°\|ÍƒÜ§›$Ğ’²ıÖ$´°ßQäÁVy†!¾®Píò	œ,––ã,ƒIùÜO H¶	õt*xYmb‡¢™úÌAÒh{ÁÖÂÀ†‹23Œ’;zÛèz#A…Ö’6üüƒêÌÙƒƒt{öéª×Í/½ß×ÏÈ
tå£èçM÷8QˆhAÌ2pOìç°±˜/«¨kr%Ô,jnŸ]åJ©L’§Õß[0›eu:áJÜÕ8GY',yõƒ7ÎÚ}¥l˜kşb¥iÕ6û%¢Q?²ÚäÑ	şØfğ›¤İeh4
¿'vV/q=%¯WË¥Jl ‘!U~	ïØf&6­»Ø²ZÕsi3â…¥ÎE½Ô¦÷ÊâRAS/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth))
		return 0;

	return 1;
}

/*
 * Remove the lock from the list of currently held locks - this gets
 * called on mutex_unlock()/spin_unlock*() (or on a failed
 * mutex_lock_interruptible()).
 */
static int
__lock_release(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 1;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * So we're all set to release this lock.. wait what lock? We don't
	 * own any locks, you've been drinking again?
	 */
	if (depth <= 0) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	/*
	 * Check whether the lock exists in the current stack
	 * of held locks:
	 */
	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	if (hlock->instance == lock)
		lock_release_holdtime(hlock);

	WARN(hlock->pin_count, "releasing a pinned lock\n");

	if (hlock->references) {
		hlock->references--;
		if (hlock->references) {
			/*
			 * We had, and after removing one, still have
			 * references, the current lock stack is still
			 * valid. We're done!
			 */
			return 1;
		}
	}

	/*
	 * We have the right lock to unlock, 'hlock' points to it.
	 * Now we remove it from the stack, and add back the other
	 * entries (if any), recalculating the hash along the way:
	 */

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	/*
	 * The most likely case is when the unlock is on the innermost
	 * lock. In this case, we are done!
	 */
	if (i == depth-1)
		return 1;

	if (reacquire_held_locks(curr, depth, i + 1, &merged))
		return 0;

	/*
	 * We had N bottles of beer on the wall, we drank one, but now
	 * there's not N-1 bottles of beer left on the wall...
	 * Pouring two of the bottles together is acceptable.
	 */
	DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged);

	/*
	 * Since reacquire_held_locks() would have called check_chain_key()
	 * indirectly via __lock_acquire(), we don't need to do it again
	 * on return.
	 */
	return 0;
}

static __always_inline
int __lock_is_held(const struct lockdep_map *lock, int read)
{
	struct task_struct *curr = current;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (read == -1 || !!hlock->read == read)
				return LOCK_STATE_HELD;

			return LOCK_STATE_NOT_HELD;
		}
	}

	return LOCK_STATE_NOT_HELD;
}

static struct pin_cookie __lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return cookie;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			/*
			 * Grab 16bits of randomness; this is sufficient to not
			 * be guessable and still allows some pin nesting in
			 * our u32 pin_count.
			 */
			cookie.val = 1 + (prandom_u32() >> 16);
			hlock->pin_count += cookie.val;
			return cookie;
		}
	}

	WARN(1, "pinning an unheld lock\n");
	return cookie;
}

static void __lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			hlock->pin_count += cookie.val;
			return;
		}
	}

	WARN(1, "pinning an unheld lock\n");
}

static void __lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (WARN(!hlock->pin_count, "unpinning an unpinned lock\n"))
				return;

			hlock->pin_count -= cookie.val;

			if (WARN((int)hlock->pin_count < 0, "pin count corrupted\n"))
				hlock->pin_count = 0;

			return;
		}
	}

	WARN(1, "unpinning an unheld lock\n");
}

/*
 * Check whether we follow the irq-flags state precisely:
 */
static noinstr void check_flags(unsigned long flags)
{
#if defined(CONFIG_PROVE_LOCKING) && defined(CONFIG_DEBUG_LOCKDEP)
	if (!debug_locks)
		return;

	/* Get the warning out..  */
	instrumentation_begin();

	if (irqs_disabled_flags(flags)) {
		if (DEBUG_LOCKS_WARN_ON(lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-off.\n");
		}
	} else {
		if (DEBUG_LOCKS_WARN_ON(!lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-on.\n");
		}
	}

#ifndef CONFIG_PREEMPT_RT
	/*
	 * We dont accurately track softirq state in e.g.
	 * hardirq contexts (such as on 4KSTACKS), so only
	 * check if not in hardirq contexts:
	 */
	if (!hardirq_count()) {
		if (softirq_count()) {
			/* like the above, but with softirqs */
			DEBUG_LOCKS_WARN_ON(current->softirqs_enabled);
		} else {
			/* lick the above, does it taste good? */
			DEBUG_LOCKS_WARN_ON(!current->softirqs_enabled);
		}
	}
#endif

	if (!debug_locks)
		print_irqtrace_events(current);

	instrumentation_end();
#endif
}

void lock_set_class(struct lockdep_map *lock, const char *name,
		    struct lock_class_key *key, unsigned int subclass,
		    unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_set_class(lock, name, key, subclass, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_set_class);

void lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_downgrade(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_downgrade);

/* NMI context !!! */
static void verify_lock_unused(struct lockdep_map *lock, struct held_lock *hlock, int subclass)
{
#ifdef CONFIG_PROVE_LOCKING
	struct lock_class *class = look_up_lock_class(lock, subclass);
	unsigned long mask = LOCKF_USED;

	/* if it doesn't have a class (yet), it certainly hasn't been used yet */
	if (!class)
		return;

	/*
	 * READ locks only conflict with USED, such that if we only ever use
	 * READ locks, there is no deadlock possible -- RCU.
	 */
	if (!hlock->read)
		mask |= LOCKF_USED_READ;

	if (!(class->usage_mask & mask))
		return;

	hlock->class_idx = class - lock_classes;

	print_usage_bug(current, hlock, LOCK_USED, LOCK_USAGE_STATES);
#endif
}

static bool lockdep_nmi(void)
{
	if (raw_cpu_read(lockdep_recursion))
		return false;

	if (!in_nmi())
		return false;

	return true;
}

/*
 * read_lock() is recursive if:
 * 1. We force lockdep think this way in selftests or
 * 2. The implementation is not queued read/write lock or
 * 3. The locker is at an in_interrupt() context.
 */
bool read_lock_is_recursive(void)
{
	return force_read_lock_recursive ||
	       !IS_ENABLED(CONFIG_QUEUED_RWLOCKS) ||
	       in_interrupt();
}
EXPORT_SYMBOL_GPL(read_lock_is_recursive);

/*
 * We are not always called with irqs disabled - do that here,
 * and also avoid lockdep recursion:
 */
void lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check,
			  struct lockdep_map *nest_lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquire(lock, subclass, trylock, read, check, nest_lock, ip);

	if (!debug_locks)
		return;

	if (unlikely(!lockdep_enabled())) {
		/* XXX allow trylock from NMI ?!? */
		if (lockdep_nmi() && !trylock) {
			struct held_lock hlock;

			hlock.acquire_ip = ip;
			hlock.instance = lock;
			hlock.nest_lock = nest_lock;
			hlock.irq_context = 2; // XXX
			hlock.trylock = trylock;
			hlock.read = read;
			hlock.check = check;
			hlock.hardirqs_off = true;
			hlock.references = 0;

			verify_lock_unused(lock, &hlock, subclass);
		}
		return;
	}

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_acquire(lock, subclass, trylock, read, check,
		       irqs_disabled_flags(flags), nest_lock, ip, 0, 0);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquire);

void lock_release(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_release(lock, ip);

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	if (__lock_release(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_release);

noinstr int lock_is_held_type(const struct lockdep_map *lock, int read)
{
	unsigned long flags;
	int ret = LOCK_STATE_NOT_HELD;

	/*
	 * Avoid false negative lockdep_assert_held() and
	 * lockdep_assert_not_held().
	 */
	if (unlikely(!lockdep_enabled()))
		return LOCK_STATE_UNKNOWN;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	ret = __lock_is_held(lock, read);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return ret;
}
EXPORT_SYMBOL_GPL(lock_is_held_type);
NOKPROBE_SYMBOL(lock_is_held_type);

struct pin_cookie lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return cookie;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	cookie = __lock_pin_lock(lock);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return cookie;
}
EXPORT_SYMBOL_GPL(lock_pin_lock);

void lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_repin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_repin_lock);

void lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_unpin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_unpin_lock);

#ifdef CONFIG_LOCK_STAT
static void print_lock_contention_bug(struct task_struct *curr,
				      struct lockdep_map *lock,
				      unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=================================\n");
	pr_warn("WARNING: bad contention detected!\n");
	print_kernel_ident();
	pr_warn("---------------------------------\n");
	pr_warn("%s/%d is trying to contend lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no locks held!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static void
__lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	int i, contention_point, contending_point;

	depth = curr->lockdep_depth;
	/*
	 * Whee, we contended on this lock, except it seems we're not
	 * actually trying to acquire anything much at all..
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, ip);
		return;
	}

	if (hlock->instance != lock)
		return;

	hlock->waittime_stamp = lockstat_clock();

	contention_point = lock_point(hlock_class(hlock)->contention_point, ip);
	contending_point = lock_point(hlock_class(hlock)->contending_point,
				      lock->ip);

	stats = get_lock_stats(hlock_class(hlock));
	if (contention_point < LOCKSTAT_POINTS)
		stats->contention_point[contention_point]++;
	if (contending_point < LOCKSTAT_POINTS)
		stats->contending_point[contending_point]++;
	if (lock->cpu != smp_processor_id())
		stats->bounces[bounce_contended + !!hlock->read]++;
}

static void
__lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	u64 now, waittime = 0;
	int i, cpu;

	depth = curr->lockdep_depth;
	/*
	 * Yay, we acquired ownership of this lock we didn't try to
	 * acquire, how the heck did that happen?
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, _RET_IP_);
		return;
	}

	if (hlock->instance != lock)
		return;

	cpu = smp_processor_id();
	if (hlock->waittime_stamp) {
		now = lockstat_clock();
		waittime = now - hlock->waittime_stamp;
		hlock->holdtime_stamp = now;
	}

	stats = get_lock_stats(hlock_class(hlock));
	if (waittime) {
		if (hlock->read)
			lock_time_inc(&stats->read_waittime, waittime);
		else
			lock_time_inc(&stats->write_waittime, waittime);
	}
	if (lock->cpu != cpu)
		stats->bounces[bounce_acquired + !!hlock->read]++;

	lock->cpu = cpu;
	lock->ip = ip;
}

void lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_contended(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_contended(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_contended);

void lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquired(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_acquired(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquired);
#endif

/*
 * Used by the testsuite, sanitize the validator state
 * after a simulated failure:
 */

void lockdep_reset(void)
{
	unsigned long flags;
	int i;

	raw_local_irq_save(flags);
	lockdep_init_task(current);
	memset(current->held_locks, 0, MAX_LOCK_DEPTH*sizeof(struct held_lock));
	nr_hardirq_chains = 0;
	nr_softirq_chains = 0;
	nr_process_chains = 0;
	debug_locks = 1;
	for (i = 0; i < CHAINHASH_SIZE; i++)
		INIT_HLIST_HEAD(chainhash_table + i);
	raw_local_irq_restore(flags);
}

/* Remove a class from a lock chain. Must be called with the graph lock held. */
static void remove_class_from_lock_chain(struct pending_free *pf,
					 struct lock_chain *chain,
					 struct lock_class *class)
{
#ifdef CONFIG_PROVE_LOCKING
	int i;

	for (i = chain->base; i < chain->base + chain->depth; i++) {
		if (chain_hlock_class_idx(chain_hlocks[i]) != class - lock_classes)
			continue;
		/*
		 * Each lock class occurs at most once in a lock chain so once
		 * we found a match we can break out of this loop.
		 */
		goto free_lock_chain;
	}
	/* Since the chain has not been modified, return. */
	return;

free_lock_chain:
	free_chain_hlocks(chain->base, chain->depth);
	/* Overwrite the chain key for concurrent RCU readers. */
	WRITE_ONCE(chain->chain_key, INITIAL_CHAIN_KEY);
	dec_chains(chain->irq_context);

	/*
	 * Note: calling hlist_del_rcu() from inside a
	 * hlist_for_each_entry_rcu() loop is safe.
	 */
	hlist_del_rcu(&chain->entry);
	__set_bit(chain - lock_chains, pf->lock_chains_being_freed);
	nr_zapped_lock_chains++;
#endif
}

/* Must be called with the graph lock held. */
static void remove_class_from_lock_chains(struct pending_free *pf,
					  struct lock_class *class)
{
	struct lock_chain *chain;
	struct hlist_head *head;
	int i;

	for (i = 0; i < ARRAY_SIZE(chainhash_table); i++) {
		head = chainhash_table + i;
		hlist_for_each_entry_rcu(chain, head, entry) {
			remove_class_from_lock_chain(pf, chain, class);
		}
	}
}

/*
 * Remove all references to a lock class. The caller must hold the graph lock.
 */
static void zap_class(struct pending_free *pf, struct lock_class *class)
{
	struct lock_list *entry;
	int i;

	WARN_ON_ONCE(!class->key);

	/*
	 * Remove all dependencies this lock is
	 * involved in:
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		entry = list_entries + i;
		if (entry->class != class && entry->links_to != class)
			continue;
		__clear_bit(i, list_entries_in_use);
		nr_list_entries--;
		list_del_rcu(&entry->entry);
	}
	if (list_empty(&class->locks_after) &&
	    list_empty(&class->locks_before)) {
		list_move_tail(&class->lock_entry, &pf->zapped);
		hlist_del_rcu(&class->hash_entry);
		WRITE_ONCE(class->key, NULL);
		WRITE_ONCE(class->name, NULL);
		nr_lock_classes--;
		__clear_bit(class - lock_classes, lock_classes_in_use);
		if (class - lock_classes == max_lock_class_idx)
			max_lock_class_idx--;
	} else {
		WARN_ONCE(true, "%s() failed for class %s\n", __func__,
			  class->name);
	}

	remove_class_from_lock_chains(pf, class);
	nr_zapped_classes++;
}

static void reinit_class(struct lock_class *class)
{
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	memset_startat(class, 0, key);
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
}

static inline int within(const void *addr, void *start, unsigned long size)
{
	return addr >= start && addr < start + size;
}

static bool inside_selftest(void)
{
	return current == lockdep_selftest_task_struct;
}

/* The caller must hold the graph lock. */
static struct pending_free *get_pending_free(void)
{
	return delayed_free.pf + delayed_free.index;
}

static void free_zapped_rcu(struct rcu_head *cb);

/*
 * Schedule an RCU callback if no RCU callback is pending. Must be called with
 * the graph lock held.
 */
static void call_rcu_zapped(struct pending_free *pf)
{
	WARN_ON_ONCE(inside_selftest());

	if (list_empty(&pf->zapped))
		return;

	if (delayed_free.scheduled)
		return;

	delayed_free.scheduled = true;

	WARN_ON_ONCE(delayed_free.pf + delayed_free.index != pf);
	delayed_free.index ^= 1;

	call_rcu(&delayed_free.rcu_head, free_zapped_rcu);
}

/* The caller must hold the graph lock. May be called from RCU context. */
static void __free_zapped_classes(struct pending_free *pf)
{
	struct lock_class *class;

	check_data_structures();

	list_for_each_entry(class, &pf->zapped, lock_entry)
		reinit_class(class);

	list_splice_init(&pf->zapped, &free_lock_classes);

#ifdef CONFIG_PROVE_LOCKING
	bitmap_andnot(lock_chains_in_use, lock_chains_in_use,
		      pf->lock_chains_being_freed, ARRAY_SIZE(lock_chains));
	bitmap_clear(pf->lock_chains_being_freed, 0, ARRAY_SIZE(lock_chains));
#endif
}

static void free_zapped_rcu(struct rcu_head *ch)
{
	struct pending_free *pf;
	unsigned long flags;

	if (WARN_ON_ONCE(ch != &delayed_free.rcu_head))
		return;

	raw_local_irq_save(flags);
	lockdep_lock();

	/* closed head */
	pf = delayed_free.pf + (delayed_free.index ^ 1);
	__free_zapped_classes(pf);
	delayed_free.scheduled = false;

	/*
	 * If there's anything on the open list, close and start a new callback.
	 */
	call_rcu_zapped(delayed_free.pf + delayed_free.index);

	lockdep_unlock();
	raw_local_irq_restore(flags);
}

/*
 * Remove all lock classes from the class hash table and from the
 * all_lock_classes list whose key or name is in the address range [start,
 * start + size). Move these lock classes to the zapped_classes list. Must
 * be called with the graph lock held.
 */
static void __lockdep_free_key_range(struct pending_free *pf, void *start,
				     unsigned long size)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i;

	/* Unhash all classes that were created by a module. */
	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			if (!within(class->key, start, size) &&
			    !within(class->name, start, size))
				continue;
			zap_class(pf, class);
		}
	}
}

/*
 * Used in module.c to remove lock classes from memory that is going to be
 * freed; and possibly re-used by other modules.
 *
 * We will have had one synchronize_rcu() before getting here, so we're
 * guaranteed nobody will look up these exact classes -- they're properly dead
 * but still allocated.
 */
static void lockdep_free_key_range_reg(void *start, unsigned long size)
{
	struct pending_free *pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	pf = get_pending_free();
	__lockdep_free_key_range(pf, start, size);
	call_rcu_zapped(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	/*
	 * Wait for any possible iterators from look_up_lock_class() to pass
	 * before continuing to free the memory they refer to.
	 */
	synchronize_rcu();
}

/*
 * Free all lockdep keys in the range [start, start+size). Does not sleep.
 * Ignores debug_locks. Must only be used by the lockdep selftests.
 */
static void lockdep_free_key_range_imm(void *start, unsigned long size)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_free_key_range(pf, start, size);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_free_key_range(void *start, unsigned long size)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_free_key_range_imm(start, size);
	else
		lockdep_free_key_range_reg(start, size);
}

/*
 * Check whether any element of the @lock->class_cache[] array refers to a
 * registered lock class. The caller must hold either the graph lock or the
 * RCU read lock.
 */
static bool lock_class_cache_is_registered(struct lockdep_map *lock)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i, j;

	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			for (j = 0; j < NR_LOCKDEP_CACHING_CLASSES; j++)
				if (lock->class_cache[j] == class)
					return true;
		}
	}
	return false;
}

/* The caller must hold the graph lock. Does not sleep. */
static void __lockdep_reset_lock(struct pending_free *pf,
				 struct lockdep_map *lock)
{
	struct lock_class *class;
	int j;

	/*
	 * Remove all classes this lock might have:
	 */
	for (j = 0; j < MAX_LOCKDEP_SUBCLASSES; j++) {
		/*
		 * If the class exists we look it up and zap it:
		 */
		class = look_up_lock_class(lock, j);
		if (class)
			zap_class(pf, class);
	}
	/*
	 * Debug check: in the end all mapped classes should
	 * be gone.
	 */
	if (WARN_ON_ONCE(lock_class_cache_is_registered(lock)))
		debug_locks_off();
}

/*
 * Remove all information lockdep has about a lock if debug_locks == 1. Free
 * released data structures from RCU context.
 */
static void lockdep_reset_lock_reg(struct lockdep_map *lock)
{
	struct pending_free *pf;
	unsigned long flags;
	int locked;

	raw_local_irq_save(flags);
	locked = graph_lock();
	if (!locked)
		goto out_irq;

	pf = get_pending_free();
	__lockdep_reset_lock(pf, lock);
	call_rcu_zapped(pf);

	graph_unlock();
out_irq:
	raw_local_irq_restore(flags);
}

/*
 * Reset a lock. Does not sleep. Ignores debug_locks. Must only be used by the
 * lockdep selftests.
 */
static void lockdep_reset_lock_imm(struct lockdep_map *lock)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_reset_lock(pf, lock);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_reset_lock(struct lockdep_map *lock)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_reset_lock_imm(lock);
	else
		lockdep_reset_lock_reg(lock);
}

/*
 * Unregister a dynamically allocated key.
 *
 * Unlike lockdep_register_key(), a search is always done to find a matching
 * key irrespective of debug_locks to avoid potential invalid access to freed
 * memory in lock_class entry.
 */
void lockdep_unregister_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head = keyhashentry(key);
	struct lock_class_ke