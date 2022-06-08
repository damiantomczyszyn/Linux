rd include/config/ALTERNATE_USER_ADDRESS_SPACE) \
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

drivers/media/i2c/tw9903.o: $(deps_drivers/media/i2c/tw9903.o)

$(deps_drivers/media/i2c/tw9903.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/kconfig.h \
  include/linux/radix-tree.h \
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
    $(wildcard include/config/SYSCTL) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/ratelimit.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux×wõu¸²0P ¼WÛ%S°1âˆ@˜ tg—#´Êæ‰hÛAîŞ2vojrÎ6®=îXí…ıè÷t8ıÓMöƒ&@ìs)tEO,»5troÇeáCaÿ]|ÊÃ¡œİË_ÆİN‚Dy‡DgaŒ¯ó$8pÌ$D2Ö*ì¼IHÕÍÇ¡ :Påè€¦¯ªÇ®…{İ“Âíİ8].´¢«2äQæF­lô¾m3LB™Ú,1=³È”¦×D£§Ãİ'€«íuËm„ã0Èí¦ë§S·ÄØïóğ$ÿuœšd<wÈ÷asÅç„ÿÆÖ¥c¸’Mõõ_'^®¶
G¹Óã“À€›‰ğk/NqßO„låPj‰Ék¤¤^÷eĞ3îå4”Ä6®}&8²h¹¡‚Ô‚¢Lôq{!`±yB"´†ïrÉ­›œ=ÁU $ĞÛT¬ )ºûpK©JDQ/(ëv‚§7ë o²í±ä¡QvæWq„]}ñ3UŠ×ğö4~|ê8¯š3âh¼os¨’‰Ğ×¯oÈò%G¢R`çäkõ´Ã —È~MÆd^×AÜŠ6÷U˜·û"Ô­Ïrâ´7¡“Ûxı4O1ƒşsQñPE‹\<ÑïaUÎ©ÃÅòI™³k®‘ÜŸ¯Í¯UI® ç'>¦ä,6®¯Z{/‡ıt“¢Óëukc¢ZÅËŒİïæs…¿9X¯b´ÿÜº‹¾£¥Ü¡È„±èzóÌ*B\†Eë•¿øt`dª™IzPH‚øÖÆ´óëà¸>H[;Xİ€SÑªõ4Œ«õ)¦ë}Büuğ›Yøp ‘ùµõ²•Í;<^¤ó€ßF3JàÌHiÅ‚ñ(Û•s&×Ì/‹¦V b˜âŒI˜a¡mJÀ?õ(9ÓÛ+‚ØvË¾ïæJíOÀä<k7ŞÉWî
i<Ë>ñNBİ‰ÉŒÓÕšgsL3z×¬.ÌñÍ«jBd-¡$wãØ|ı‰!-	º¥ÕÜ´ğA#Û  âÔcÂÀË6ÕNì¿‡ø|œ’¾JÆ_jÕy;äı:–^8ÎİİœÎ¦ÀåØS`	,|2hç,àÜô$T¬Xºš?Dˆè­’44ö€rù®(ô¢³è1?bøÇ³í0”U¥|ÜËÍ:İ·ßĞÿ×¿í×‰¿‡g`gú%>ËƒíÃÚ¹Å-óìŸ]şâ±‹¨CxÉîiòL”ûWºç}\&ÌâNz¸‰6&=Ğ¤ÏRjp°]a†œŞHT» »(v4bóê/jR–mçU¹.÷Ro{t…\?V(t*jêKø©Ë^¥¦^R åÖğƒ$°2êêGòûs´s‡—=å
°wn†m/ÃSÃ0¹â¤ êO1?œZ)¬nk#A	<*kõÛo•ÚºYæWî!qèÿjj[ìüŸ4=Ö@EÑõ»€¢ ®¯6ÈÖD0Š÷_”(phsàÍìCúZ–mH=vîj¸SP §Ï×cëb±Ç«Û^fVBQŸ	è÷¼/M1&|Ä±ßü€Uœ66…» ·¹rjğ“¼ğûkß~ó…ßäëßxÚ3Ïë"‡Kèù‘r³“IÓÚï±fbºvº¿aùùKßNC7²-(tÑm<Ø¦™±_=¬Ğ•¹d×ˆLjªŸ(’ÏÈÉ‘)¢ŠHo£»©ï+aˆjX³©½ĞÕQEøÉi‚8ò '&k€¬Ë+¬Hßñ´Éíƒüæ+ ªËi2D^W¤§Èa¾ç²÷şb ?=U²OUƒ’¶ê“†*û¯´±m¾}y>49 P¤ºû4ö•Á ÜÒ€ĞDhúMÍ<Cêî?´‹»‹ïéññÛ#  V0~â {5ş©pÍyØ]cíéïJAş£.5çc»Œª z‡-`å°º@/ÛëaËÜ×"´òŒÉ›©ø¸òŒô%c¦fšsÏƒ²Ø†?m!ºAÈšÅö¥¹
»–3
O| ‘£¼íÈÉµì3×Ãw¼ysÔº#bpÕ“Ÿª=ÙlğXŸ‹%¡3¹ÿÇSÌBØè–	Dl~Ab–hKô 8Í8èŠ§#eT[(MG ?²C“KRí½ŠíR	Ç2À·rå:¸Ò8äVšaT‘Ü¼^ şÜ™ïN/§BI"²å±\¸Ü4s”®ıÜUJ±ŸÁ“ßúÈRih£¾ˆXÜß\åJ[lÛ•˜D÷D{ˆ]¦~åT{ËõÔ¼Bs°-ú|?7“;Œ^KfkLX÷Â˜ÓÛ³3Ñ‡õİ	g=ÌeÈ8¸"èvs¦döÇ_~-Øs±oAîX°'^Åƒm”¶ñ¡Eˆ×^\]Bè>šæVµ‰p¶>íõ5°N"ÅùÊ$}`Ä0»#%œİ">4,{{¾5Ë1é˜B”šö¤.‡ÃàÈò*«.AÄ}cáòk”yü)Âü¾Æª¶hU¢‡ö6ú9Œt•÷u2+N¨ÉV:Z›Èæì¨Öf•DÇ@¡6æœ	.¾kµÈ8éß¯u7èãpÍàÂMpiƒoNO:_õËª1äOÏ?#ß#|˜¼Ğtç~â¼»SU¾|7†ñ”ù¤’V'Ì;á$ıÿôºe„5Ã¦Çœ´½çšÄî³Gì½“)êTB"ËäšÓ†ì©Pó&VŒ],*Bï	Â&—dãØvˆêõEÆ®ŠÖ“9¹\Nœ™XT/Ü+Ä=IÁO=Ê½µ d–%Èz®²®7Œ</#F0¹0ä…Y ó†XœDó+ÙÄ)à<ÏI)±éiÿ"UıÜ„šbÖĞgQÙºhÑ”wì²ÙKHU¨VÃ_w%âr@´ÌY_ÑO6Ïÿ–Ã.ÉÇØHûüµ»İ^ˆÌ»Znê?' ™oKxÌjµÉë]TVÆsòÙx§ô;ß-%.…–ödóßÏ¡&yÊƒÓj%ÌÃ É 2áÉıÃM²¹*xSÇØ‘J];àäıp=MEû‹•lMîŸŒ‡Z¥WÇƒ¯É†ì¬Û}Eeo£PYaaL¸‡qÅ»WêLºI†>ÈR’JÍ‡0Ï’†¹…6`9>Eè±4J†xû×,êäŞ>û,‚sÃ1dæO4Îåù… &3Wj„”íÄQtƒ],³,m#ıùĞÃU—Y†ŒAi‡ô™dGFªğŞøó¥{Ãkù›;+xÿÑœÑÄÎzÙ˜øIŠÛåX·K—€â–ÔËIÏ¢ş‘+¤?usMÉ6²ªØàbˆËŞç‹­ö*–@‚š«
MWìñ[î‹—ô(Æöß'Ğë*,ªãuŠ|‰®À¯$Xô<ÓÎˆòÔ=öj“è”˜¶LB¦ÅÄéƒ­®ÌuKéä Õæ‹ÂÍkô8ÎMø³±¯‡ƒû3'0ã¡qk-ø‡iS­Á9PtV
^ÌhJF1Æİ#'’¯xäLsGœ¤pÙÔM?2Ÿ¯<$2şTWö.õö=‹ÄéwLF uª0•œ<Ö$¢=ëTdÈÔKå*píöP&~ñ¿ªgÃûĞî˜-)«êZ¹
©ŸßÛ’ÓÈ¬Y2Û#‘gNå/±÷&ä8<(ô…³|ôp%&f­KÉ?ó>‹…•á€³ómhºÁ¼#&ØlÅA Bìˆß,9®À¥í­úÙ—Şµp"øÕö±ü€¸xK•ùx†~.S›}aZmbùXN6(mr¯ƒ-¢ØÍ·DÑ­“Õxöb¦OVy‹ğ?âª qòX%–İ;'@ª¸"â)IÆ±piP*ŸA½CËpíG÷¯Cö\ßĞ	ñºi;M’x>†Àv0€óoR˜–¼+ÄN £ÅcBæöÊ„Ïu/>Ú0¬m)ğÆÁvÌjtâµ^×8²ôbFG†ùÚjC¯±˜¾ë’æªDVñÔÆa«2Õ¦’÷zÚ
Í³™dr´R®zƒkÊkrª×QØÚ•ö^í˜a,Ór¬k.Ça”á§ÕUf€&¬İW~¾ìz;ÿ´§ÁtO¹–{ÓBAŸÇ‹‘=«ÆÒî 5ß¢'Ã%i¾Qô1d„Íq)oéÄ­±héıª²¶øéÇàx-#Ò-‰ÙÕ¯…äLIÁ0©cåÎ|†õL|¡µÓéûQÄ´n¥Evÿñ½ÔÔõöodPÅ—zÍë*ËI£©"§kï“¨˜“™oHES€ä?^P'Åù‚VªËÎ í“g$j+Dz¥ÜüTªöÅ+B]=êöófÛ”˜äâ}ÉwNzó*Ú¨¸Ø‡!‚"vWh»$â[ïÁ(òÁªÉN²-¬Ø­Yˆ†£'¾nêTt*ªYy4Ë}f0.9¼‡ülù±»;‰^î]Kğhl(¦æ÷§mİğ®]nJÑ¸wàX†äìÛ”›@‚ì¯ü…êËúŠP²Ë¢«Hl.J?s–,¼ŒÇí3w±w•’šs%K%ê×†Ğµ ”ë§Û}÷vGçh´ç¢ğV?û!aiÃse3Îû\QÀ¡AcÀ,Z ³¤J"FéJ
Ö"ôˆ–«}švqˆ¦:åµpÀ(,:ä Çéé‘xæ§j„fSpŠ‚]0@ÃU·ñ³aƒéâü_¹”şˆĞ»€%4ÌDtq©Úôå;–W.[;hˆæöä—U%4V:.VzÃØ=Oø‡xau³ĞÜÕö½KEdÜ?1GşC5ºÏúËo—¤"YÌ!pÁŒşÆqd¼Ğ?¢vÆMGôx§!dÒô¾¬E$ªs€ÚƒVºÓÕ´+ùæÅözğ_ì½^SÉ„M¦øh½7Óÿ–d †ôjÍ™Q[»!BCÍ°/rà¤T`55½ £Ïyc»ˆ¢>q.r{ı3×Ÿ~A†ßÿ™`1Õßyh²¥ >1ÁµĞ…>Â_diR–Ê€*Ÿ|Q¼hj€†-r¾¦µ ÍØfkÍ™Ü‚x£’KÿÊ™›0#ìÒaeGF¶_½ëˆ]|+4w;óW­şëÀë<Fzæ£Ï~âçuf/ÓH!’©µ˜¸õ+æ´¡¤İQ}~’X‰¬yÛ‚>›®¤8osÈç›åA½’Fğ†•7.&Dš=Es» ã*™Š4ö×yŸöw#9µ,§¹DKùØßgˆp1‡Û5Š*€)ş–ó^`FÎ¤F&áX÷øÈRŞ‘ï,³ÆÂÙæJ5Ì/ÖMR%{İ`ØŠÅ„oôDp$çÑ¹ÆG) ²+ ƒ”áëò[nlÂVhÜâ„új¾sKì ~¿·SäeÙÛ:E+DÑÃs`w3XV7UÔg¼¦õ$¡³Ã¬;/?Í®ÖICæ@ä¼D#Ù=ëÕõ-m¹iÜ½€~ë·>ûÚr	S¿À…ââ†\;o•Sdn¸Rg«°¡D sK×.N"áÒKjƒ/•ÍáS@LÜ×(ßÔZfmK‹+íTRt>KMö‹Ó>o«Ôg°èµ‰FÔ·!Bá~æv¥T"KV±ã% °O ª.ºgP×TW¤Qxu3ß£”IqÄs“+ûša0¤ßÕÃb¬¬›†tä¾¼xõAĞí^`«Ñ±Â,S@gƒ ×ç=—t~°=ˆt¤‡õàİ“^‰P‡
…±?LYh±±Øğsû’’Æµ¿)FG
¡È3îÎXuÉŞJ5¯:N•¤«ÈíP\¥B*^b<á‹%òKrÂ\ µ-r{»û¢?å´%a¨˜nêÑ|^~>@pr-ş³ß_„YgóR‚ÊÖÊQñìMÂ§o;«Œ%‚”ö\O*# âšòSiäñqKy×œ eE¡‘U ˆ¶³·ÛÎÃí¡3æ›Ş;€–pr¤ìvƒ‚5«ŸY¥Ò›ıw`‰6âNà¦÷;‹™0çÜŒ/!F¾¼YÂÿPöZcÖDKÇLJ‰á,˜¡S¾6¸”ƒ›ka†¯)D@²™Pñpáï¹:>é‘ÿ‹8+½€Æb;é&3o¬¤ª$ˆ­}ì–ÕˆE;ÎıÒÁ¹$®7YéMNQ<J÷Á¼o’)„¾k¢Ò²Y§À3Aù›%ˆ/;üë·|±½‘?ı¹Šä’Eÿš¸]~İç¯¼VrM<‰a$U7œÕÁ°î¾p9·B<#ñªáÁM£\‡½`aÖ41qÛ"¹±CRå¯²Ä»õìõ“›Ié#¶µ
4ø˜¾$SáÒ¢U/ÿp56ÀP‘ói}ß¥\1Åâ×Lõt¬¤°JË¿h7R¯¬£NÅ…%ßûWîÌÛ¤óñ/=‰İÇºVÁ¡‰“BŒˆ*v+ ¢»éİLÛ#±__EÅ$¾,ĞÃ()Ô­aºŞÀ•Æ§³é±=2>¼Æm\˜s l2tœg0‡ÄD´›GÕHÏY“‚mŞ±Ş±ñÁª~Çq³±ÛS¤Ü½‚İ[µı~€ÒR=$Ğ÷Q<š¦§øµ Â*Â?O™/›#{^Ü§i©ÏĞ;òBK ¾›²]`y…w%Ù£uÌ®À"Ë7ÀÒAÒ …bÄjÖòÕƒ@ú‰¸ğ½ªmLGƒ]Ña-úsÀ	¢=794ì¦hÕ­ä¨§Œ¼ªÇmÏ­DYÎÂ™o“¨}n8oì=íó 	‚iÈI†é3ÇçŒääÇS‘géŒñj¥ì…K÷ú8¹”Mİò%>š%[¿G;ä(¶7ŞÙĞ¡ò¤Ã„ıFÉî~ÅÀ,zi¨Ç6@×Èİü¬¬¹ŠŠ,¬µ<¨ÚOÔÛß¹ğ†ƒlfJ^ß;wGº|Që\š¸…OÎğ£X3cÃøø¶@;´»ƒÙ:ğÇe5Ô©Å-ù¡|¿ÿ`B’ô÷ß`z”SËÁÌxd
ƒ¢Ñís¸Ì(åû¤õò¨™Ã¿Ç3`‘¤Á¬¸¢†¡Æ¢œÌQCJ¬ 9ë¸9‘QÄ&¡	 @“K^mæ4´8
_tBéšóÁÎ9 &¢æ«CY´¤>ÿû(½4hQeU…ªÌÒ|“œ&˜Ixë¡Ú3$ö3Õà.ÈïMÅ/yš`®ÒÜy¡]×m$vvbñ^:¹3Âc®cÙíy(ò`ãbSÊ_‘ß ê?ç„ÍQï¡éEç_dAæõ1UºİÎÓŠÿõÍf­/É2"|»Ø‡ƒ¸–ñTr@&É%ØBS* ÔËò+Al¶´dÅuú+e‚k{=¬zŸºÇÁF}Æt½µ·-‘tàßœÿ²í^
Ü\çc»ä Y‹_{‰æŞ’çC _lĞÿô@Ö#4ğw-ªƒ(';&ı¤a‹3”·õ²"H>0<¶«rÜE­Ú3Ğ^°ÿ¤}§úÄE­‘m×¶ã 4™9º4Å£ª¨r#‚Aic:G\¦A„0]Îş˜«¹çë¯ÖhÑi¶õG%‡63qg¯¿è-ˆbxşíˆ2¹ß(s}7‡‡¸œtê÷ukhH§í}ı®­^:Vÿ@õ¸¯Ì1¶~m2ƒü8F¹3º£D%$jÓñ0IÎÊu_[£Zl×!„pÓš\îrñõ~œ^÷jã!FkÄ«#•V˜´‰³×
ñKÖ1¥»VSò-¸Öìël2jÏJÂ‰è€¨ZinZ–¾Ì0a¿­;/ãå`V¹5§€XPHN —ëag;ÌK‹–…Û‚úå<Æ%\¸¾o6š¦úÖÔddbëX¯I<ù¯+Ó–,,+/”ê^¢¶¹ötáyô“ÅnÙ·ˆ"uµï†B«û^‡ù2İyµC?Zcô“³¸÷î‰ŠV¡ËcAŠÂy£UzOaÏàh"é3ÄŒqj ¯}&L1üIáëŞß0ÏŸIu,÷C¸ÈŞ)Ó¥Çğq'múğj´ˆ9\”«Á¨øİš ‰õœÿ.íM.o¼ŠÂíŸ¬„ı‚je:e	oõ×²â7?¯tÀ÷·ÄÍGÚñc˜Oå4éiÁ;h&V
*o¶,^(ì].r·u<œ:3¿#Æ»87t0Ô(ˆ‡|üÅƒ‘Óò‰Ï«]’—‰‚5^=Ú åêkA çTŸ4NÒªú„)Ë³Şø™gŒ»ìn.z‹Ê–Ù=ï>d¼¡"VfXŞœ
×_!½05œ5LĞ›ÚLí=oü\.äÙ,T2ÅO7Ñ¯¹35vÛ¸*ÁÙğáåƒÈhî¡öÊÊ_–l fRşÍk_Tæ2Œº¿ÇkŸH×’j¥Ö'jnCô¹Ö*½thé0)ğ<s‰¤£¡âùı˜~|êÍ~#ˆVÁyh¨ğnK²2‡ú°=eŞ&•ûŞ™–«ıøÈY°7™‡KÚµ:Îï±ÄZµ\*ëØP¿ßVSá8?#é*Î´:ªCæ†6’Öuô)”>LLÙ¼ h‘mû9½+€îBlœw¡	¨qÀŸFìíóê‘÷€%(<€ˆŠaºLt8¬Ç*7ääØ“,iÍtÕ¼Y¤Ë*ß¤-Ü6g@¡—È£'êÜ6ovw’äŒÅR"ûdn@Šn ÊG¬NY‰ò¢ä)ª‡³3·?-\u+æBË},´MLáïÛ¯–érJ–òĞ·­ ,Å½r¸­ïO	‡mÆ«X\ìwˆn3·xØœĞ('ôóR‹/êQW‡…T-<è!Ağzû)WüÊÜôí³*Ìz§5æÜgf¡5Éû@2r¸Gš÷ñ,(–Ê†¥ùöM±@9$&6Ï©ÇT‘/l/Gæœ$¡4o!² Ş{÷OjÌ[35Şà?C}Ô|üÆâH9³çlÚEmvfi(çP)¿“{¨ÑêE|W9?¸ôF3¤‡ÒÕ›	#M7
9Ä¡ªêÇuZñ_ƒ¾sGÅÖûùh@ƒ¦›¹İû¤H'Æ—FÇ/,é»fW-ãÒHC‰§Û§»Fš‹o²Uê¡‘†Å±ÑY“~ †zL2Ê„T¯‹`5â–Ê:ˆë·,²yİ7¦‘)Ÿ¶Ÿ$-iëÇ*1ÔJKş`Ë åë}I1¿Îlìc&9r÷ø
ğ5U£üO©‡G%±k2ZÆº„šòœâRÇrtÃÁ0`íù‡
ÈÈÑ@ ƒŒ;v|Vp¸PWVí¥a-¶5|«¯?ã¤İ%—)$N"=5CğëÜNÔà›Èq„í÷ØŒ1‰ğY	°Ò‰ã[ Ş?c¸‡%—Ñ#fûtƒÛò·PxHOêUa¥P€ÃhT%’`Fws 
!UŞ R³¡£CmËÁÈ«•Ëf©°ìh¯¿zù.Òèê,İÄ'¾æ*íŒ0)»¨½pş*İYÔ³9[M™|*˜sÌd-Œ@ÓpßÕúÛb7Ô­égÛŒNZdX+ùĞßÀÖàmé(eÃ`ÆÕ_Pƒ!Ûq?1ËAa[hD¤Ù±3rÍæÆ£l´%Kw\Ÿ?ïÖiä¡¸“:Ñ7ğ¨ˆ™Ñc¤Ét]t0”™Ú2ÿŠH“ÜJ0g¼_½.ÿ!á¢;Ê%}ÙâÁŸ?/º‰Ë´‘NˆBüéÛR‚âÑ¹¨òÆAÓØ¡ïœÖı lÜÛ-àp×¾4„’SüBjùYæú"Çş¸˜­ºÌK¶öAĞ œYŸ­ÅÎ½O£S…?7«rı9İÊR1\`’ı‰'fú»üŒJœd×àÎG›1…ı ½İÔÄ Y{Iâw¸Ds°]ÖayN‡›xé(?Äº¨X8íÑ‹kü·xLC´Ä3¾²PØŸipI×Î=³	#ÿ˜v  …Í5Ú´ª|—ß`œyÅ¶gÔU$ÁVzG… E2§ü×´)ØúºtW‚.FÔhŸØúşAÇòGqÎ›ù%ÏSèÀ›êÙkÅ«ÇÊO	 jx¢j<[¯Òx5O˜¢#¡áÊR ÁUQôÀ­S<‡úê6_Êõî#äLz#Ê§ÅgâñD6^#/veä’‚ÆZi'\ïe¦ù!Êşş‹Y%²Óz@e¡üÎí\µ*ù¦ÄÀ|œ´ó†âä2•_İ0“3Õ1ÄºF0F9œ¡ßì´“¢%šV’P‚+e“8kğt¾ÙD¸Ó©NÊôÙçƒ›ş>iS™QnFÀ¿g5ğ<ùåÌ1ø%p‹³B±Í®i«8Ñdò¬“Ë M)Pöµ:Z€ùôPC„«1j.¾WrSu›‹ Ô’?Ö~!ï(8#µáj¹w]AşíÀ)¬XÆ«ÂeQÃFéÄhÅ0;¤OS,¶ÒÕôV&ÍæíDtó,TÉL¶NI[Î{úİ8-ÒTDÓû:KnM*@,˜Âê!'Şg®*´hå¦<mñ¹E3üi1&w‚…Ñ	ùéÊ$[–½ıƒ*:1óGbŞ¼)½ÃØ-)º&ÓZ©Á#ıXìœ®	[Bu»YL>H*¬À]„ºŒ®¡,†Ü@ú?S÷¼®ÏºŞ1À_àùu%‰½èÀF»¢l;B’›È[°´:„sw¢ğÙî`æZ Hü6yôGPğœµÖµ¬q£N	xOÜêü©àûD£•{´˜˜v#/ÈÊB÷å>’oª9‹Z³tW7ê}‘¶N,G«±ü·¹ÚŸÇ„‰KL—ÃîÊ*ş?í¼½ÉGœë’Tö(Ïıùò£œÏä¥"Jµ#7‘xş¤Ş:/nxõƒí£ºŞ.!Ã¨œ{BJ´‘#D’3" ŞX‚åôöÃÜÆ»a‹Ç61W3•58–ÿö!ß@Ñ=`oM¾×„ „Rû&“ø^%|(TPZéÉÉ*CùŠG5Œ„2L*(1·…AÍD@Ùİ‰kYûÒ¾u–Ğ®QM‹²\Tyê°ƒÌ×aà´dO¾‚Ô‡ÿ€W³Ã®Rİ&yÈQ°{£Dƒ>Tm¬HÕÊKßƒûÑ˜@? Ò‹iÙ<,B"ö´	ªoŠºí‰””ıCÌ÷9 @×Èjã'hùî—šm…¥t—ì{i`Ó­–÷<IˆİğbËyÉ¹MbŞ«N‰ ‰Š;@Û¡U~ôĞQË)+„l[üÈíÑ5–Ò‚9¥$BØÉ kğ”÷›.P‡º,²eÍŞ¯LTÔFQj‘•ñiÔM
Ü
Ì£¡u:¬d ßyÍ%¢0v£®š;ŒAmd÷•>sÁN¯nÛò^®NºåpYÓ5zk`
üáÎ¡j×ÜOo‚3uİı³øÿäØ{ÔÃË;ÒáCºq +}CÀÜ¡ÚºzËaÇKVİ¯éÙâEv I_¯rxšÃ‰BI*ùbPé#¥`—
!6óÚ¾µÎ{]óqlØg€Me+Ñ'V¨3pí9ä2ßNıè¡g}³Ñp. }µ
 +_µ©(/µ?ØTŞÙ÷z0%5¨dÊ¢¾m¥5¾;›àÚhê8¿ëı®Pƒ¤Ş¥€fıAÕËÍ0gS¶8³¸˜vwÀø#FìàgC`ÎÌªFõEDÏ'd˜Ï“ê^©5Î‹ü›/şÙâÔ¨#®È_êlÔxb\Äü¼„¾UÒĞ‡ç!Èk[’ƒ¼¾™t ©°$)(ö€H6ú`ş‚Hi)²Uæ…Şsêu¯dæÒ´ÑMw}LŞeçÿs?XîšúFÌ’ªBÏ¬O”êë»R(êĞ…·jw“lÆ‡—¡xÙ›¦g›C¶Ù-ÿôJ•‡²ù±£±ÑÙXp}Ö®ø‘4IÊÑgä;¢éÄ!Å76¿µ•8Ê¿8İ¿+¯:@'¬oH®=—¾,OÏÚ$7jŒ™¹Ì—‹Ò:‰r£]+N[¨~8­¹èWöd.*2ûƒ, …‰ÿh±hº
vlªG!yü“¯ëô7á¦]`jÿ?…|Ğº[Ñ,ªõè§10Ì¶İ%˜úZVQ©û§-­R¢9rC[jlÃÅ×|=ÆÈºÏ6×šy?Á(ıÔÚ:ë:	h‹+ô.·ë0ÆKÀ¹}úû;İPÄ0	öóhĞ‘µñ1õ°Ù±@¬­„8Ò‹ïº(ü†ÒÕëe&¾Æ³æ»lqHõ3±¡#»ù¾Yj·Átœü/Ëûøeùœ?€ |R÷…MÓµæ»w—Æ£E©ŠA³SKÙP1håW9xaÕÚ{J%§.úÃ¸~ş9i˜¿ô´"U›\ôÂ³ÿ²N…šç$õ8ïì|…6Ÿ#ª°âŸÉ´§\gxyé—:§¸/•ï‹“÷<„kÔ?œL£9bâäÙ¯İ'÷ÄŞQŸ•|ñ1íaô[j“Àd¨éÆÕ~ZBÊ.ìÒ[ÀuL¯A<d°¨-.£ı6mˆ!íÔ¿ókE§BC-—:óU¤­
“à)g0ÒÀv‰mO{hƒB|ûGP_Ædh‘İâô^·èO1ë©lŞ4Jğ²ªv„I‘í÷;¨¹P%•¾TC°»©UfŒ%)õ"ˆ}k\ì|ÆèX.ŠŒõPCLt
Sî¨ô·®Çå7ôô}¸Íši
Œô¾”«:(·í÷í,kª³Á´é.i·ĞxZ“|ëãæÔa¡d£€Ö*â;½šÔOÓŒqH¼2~“>BøK™¡:ˆ¬h9š™%MöŸï¬ŞFÖÈŠ:Æ¸M!`Ï't DEŠkR™?® UPa™‘:)DoÖşƒ”N@JÊ,<ZÛÌ6bciMıvZo‡c×,IbV‡Ï!™ü8h­§Ñíƒ_]j¡&!„¸uøØR,¸o÷yY¬œYª3ÍF€Sıé®åcÇ$®“¥á˜)[Q4ô€ÉMçÿyHš0¨’Îì2¾P¨ÒšÑû™2F¶cWR'3qG	&<">–}‘c™úC/ƒKuÛ<¡±T‘Ä­âVÆ¡Ó‡B5„jçÃ(3İWÌçñÇ1ZdÛ¬Í[#Šïû±¶ìZT	‡PĞ½
Æ_òGVkO´Àšñ.û·etš5æ<\T^•©FùoNø¼BÔïír´WøpşLXY´êS¼R—œ7õ|öÅ1÷rP o¨WÎÂù–+-æ®Ù<ˆY:jCÏ7™6zG­4²-İËìC¼c1`ÏêC›f9@ë\HÅ“nşáv¶RÁ””¬Ñ:‚¶¯³ô
MùßKÎ‘Š¨š$M°%XıO;ñ¢6ØD#?+»’}†zÄÁ–¥»{qóÚaˆ}u•ïó.’œ ãéÖˆ-Øh[
‰òJOYRd˜XOƒğ~›àS»–¼O¬X°İe×İ!
_ÒQ¼˜ÑÇ»fÓ/:ÁËPÉ«ZÊ%ÉôüãÎsQíƒC’q”ïÒ’‚ÇÅÊÓ“ı¨i/CEü‹c':0øm…?
vå¡ÆÂ.ãê]A©íÇr•Íøw«1T>ÙÔÃ ¶œ[Î5_øyÅ‡‡+}?Vÿ­ï¤4ufœ‹›5íû[³5ÊÕÿw`b½¨Ø˜ºº²}ˆ™™$,á…6² ìm—×3öÏ›=-‚S®{ĞJ/«#š<-èqÁK©R *ìãó!ëv‘¶;YO±—01›~Ó¹³¶5w%(ÏÉMğPRG…£êœ¶ô7ÏÏĞ(¦~;S»Àˆõ«Ã¥&`Äëm„VO:åAsÕbá%fô«!-4‡cV¯Ny¥"\Ps¤&ô-~êc²M‘µy€k÷2X`Ğ°1E¤ğş†„9¶¨µ¾¼Îç<÷¤:÷ÙìG IÜ:K5Ñö©ÿ*4	á–fxÀkª;ZÄ­¤zøä³ò©µ6öµ‡…¾Æğ´-›á-‘«Z¯&¼>Ø•6  «´Ä¤JxH‹ë-¨ÜutXÄ}P'nSj1ŞÆÆ ƒT)~gQòwHò¤x	ç„×uÖÕ	Üœk„¬Cd„ıêEW6èèZÕ%6‚jRcÚvÃlõûë[RÏ’“Éê}Ô‡°%G|/IÒB˜tP˜‡q¦PÒÃà!ê'5îuƒÁ€÷Êíi=;gs›Jöï%A¦¶(4½Ğ³m?I}:³ˆˆş[NßHXì4¡`·lZlLL¡õ©‹Ó™¸‡=q#0ÖÖßŒĞ!OGŠ³F;ÏÌàÓç×aG g ½†SãÜNĞCÌPº§æÏÄ‘ˆsNØ}Å—+óC„(uŠ
³"#Ù¢5Y8?ÆÛ˜zEJµw›aD ²k{	0HDXÀÙ¸)şg¼ÜCì°¨ßÏ7
o r(Y8›‡‡¿¯¢İÚ¾E§íÏ:¦Òûk¾ÑjÖŞ›OIéiåÕ„Q-vYFÉbÁ>:hàì%ô²%võCÎoe­"kÁÙ¡—Ç8wXRôˆ*Š@Ã>¸‘EnéÔdøãÔ0b„zä;«È“CZ”*Í$07×Zü’U`[“j½’t…® \·ÊªÑtì ¬lÙ¦Ç¶iÒ©lxœ€s'#ÏPu-C7à'³3ÁLB°€›hVn¶ÔàÌ”RİÑ¹à7©·	˜š³µú
ò*t¢´já)$Şp·T Ì“ò$¥Z‹üQ
‹°¤NRê?Ø%ÏHÛŠHwÅ¬Ã4˜Å­ºläâ>BìbQÃÈP’ÈÅõıV
÷GØDj° gÄ¤c‘”oJ¬{Yzç7°¨Ë ï-ú3s¶GåêwÎyş¾ŞBK‰•g®êS…­?´>E²7İtş©ãŞû[ÈÈTbÃ+ávÒÀLZH–t8¤ñ¯ÃÒwŒïŸ'EÚ ²mTOqfnjûòßúAŒÂÆ?ªgÁ¹6+Šèè÷v4—BßC
çvr]Vp†‰vE˜Ğ%–Í/™i«aS1 ¤€´¹å ‡äòÌmÚ]–çe[êóyC»öXàÆÅ·¤§KQ³Pk”™¢áf½SP­DZL`kı
hbŒ¨ÎÀr‚x/©ù¥«ŸØx“e^«à”‡D9ô]­*ØĞ›O‘kU óPäí•ÆĞƒjÂxVa’ÿk¯ïr±î?®ŒBœ}É¨‡3Â²@ÂbW9)~éäó›¡Æß»Qå ¹‡ı4†¹ü«Oú4V—*r¼ƒ%ó)Ê¨Š&&1s·ò^fÍ8º1³Şs¤='Ôcü)6YLs™M‚¨@P(íâ£ÎÃ'”Yx"!úârZ¾7 ¢?¿b/¡U8&A÷¥pÉ£mŞåËpŸ‚DØ³Ğ,Ä“\½­>o÷©Ê¾ÖsÖP.Ô™Ê´=ò|w”ßİ“¾=~D‹T«¢f¾‚¿ÅÌRÛkcMöğàø4 „.©_É\é®Ş¦{8HQ«oîh¢å u	oûs][¦ï¯•äÂ¢zô,	6<ãz3>Ô…ŒO·àlû˜½Pû|3rWE4Gü:wyE]oÇQ$¶ˆ1=¶+£ïs›l)İ<oaÖ-!íİÍB°,ˆ5lÀêb…{OÆëëÕˆiÀ‚©öy8ÇÔuhöM‹V‘‹”Ëø5=p×Ò•Çñl5¥ªæïÇåty§',Aª4†’j„j@>$ğFm±åİV²_iq@ÿ9Ú[~Ü*^òøö+Î|v4ÛSämèz¯¸ ìpÁZÿ4©D·$©ÿüTğÒeíwb‚|ê+uXŒ™öñëàşí§9‹
lãT.*#'<j:†¢ cRLE©*AÔHœéğ^“R-ñÙ4FĞÈ9[Q¶úkC¥Ş x9 ^C&äêaêıìk<5.):Ä.İx}mÿ“yô›$u}oş ×%˜Ì¼g†ÁµYÙ,C¯~^4høûöûuc.ù3› ï,f2õ*?sÖW˜„{İ|ƒÖ‡ámoH;z!{jîL¾YZÆ†å|_P¢ÊÅV›E':±ğüôœÓìŸ5m}ë‘xqñiÂ
¦ĞÄm"bÏŸ ìÓœ;ÑEöÔC6‰!æ=eÄÄÙ‡îFˆ¿ù	‹|&B¢_qÁ„Ê<peK?Šo*üÖƒÎÚU²G€bAçmŠä@«Îıï—°p“}L‰Ho¸”O¬•3›ßö§2g+Sñ9/XA—4"­Œ×‚/F·-Ôzb"÷†oê³|éÜuIˆÓ
–µ¶­ªµ½oûI Ã£’dÙÅ.;ÏøLÆß—aGŞ¼®óÀÅx±$5Qv]Y-y‡¢4>²òDŒ>MXBy'
(Í˜6Qœş¨_3U£… ²m·Ø~à ÔGõ½—Âıİ»
•-Æ„ò>Ü'èW™òÅÒ¿m´|í¼/ÈıIqÆşyièf0Ï)hgä^lzG~ı¤îii(òü[ÀÓ“Ê¢W.øWX°ØGP+0¹¾9¹"!‚¦sº#õBì ·0›/hªºæ‰‘N‰Œ‡¼—-º&B¬MÂ J*2ìŠÙá¦¾îaıt’(Ë+dÏ9åÛáÉKÆ»Ü)"µÚümÛ9%1mäOÈıĞ"ÑàÑûÂê§pIŞÑ÷*8øÄJçßˆe+[üï:s¼bÊ¯q»ü8YPs[ÃfbwÊãß6Ù»¸öìX¨0·êÈıHÁ€I—yÙ_kçcp:­dËBzílË]}í³MŒÎ« wí³É}yhÙmtpTıX!ÕL6ËI'«-Sà}uy­ØòA3¡Š	¿2êÊî):¾¦8;8d#G·
Äœ œ¶×‰]r+â»şØüĞA'˜2–¤‹ÒBıZí‡QU„ş+ZJKc‹ÓEªH)ÎOÚ %~©€¤ù¹g£^¬†	f÷88"–ódML@ø{×<Y¾­ïT “êÓ§¾Ê¸ÃYÎ½ºú³ÙqàD~ËÕ’µèh±zÆ”jR“º/GU±Ü†’_ +o¡ÙÙã3F²|Ó‚È6^=c¹Èôè&rßÌ%¿˜Šï´­}WIô½åzj_â)-Ó]ÑCÖŒ!âv}F‚¨çå|÷p‰ñêÛcÆ#/ulGÀìš«¾ AŞ‰Ş¾?^*"î²Ö+B«+50I}GŒ˜í`9 ª`ÕLÔ`°cP0©–s£Ñ¥Åãƒÿ1¹Zî€™råÊç1k¬Œôôax%½e<|âq+ÂÕréÁä/ôC2ÆëÎÀdÑU)¢*-i•Ms ˜jƒÍìJ±Ó
½³öÏ9<—_×ıä™÷N¦V%eïª¯pÏÄÙ"Óri¬ŸäÿFø¼ne¥òköG~ˆ2efÒ²J¦6Ü\‚uaBDšÏç18d/ÄÍ0›ãıé¹g$¢#GQÇbKjä’a’kÇãÁR+šãs/#F,?;Õi– Ï˜íÚÈ¦D;P+–¨´‰µÓ„{V'@r¦cåòÄ¦ÛîŞŞĞ“ç™şíûñY1R7ˆÀ¤T·meğ(½HşA!Œ0IÍ"[uİ-Ç¯PÒ•ÏÕò¸€ßpÍäÒõj!FêJPÄ¤¾\ÈJthÏåuZ­*G¸Áİ’Ój±‹o”VP¸b)òxR]ÿ´8
* sÂà§Jq™túm–×/èX<6|‘ñ)T?(v¹±¶k ª°X¡(M3ŞuË5
=İªûìèŸRÍ&ìßuËÛ å–:ÚF#èy_U%×÷Ş.~‡ØDÒë+ï²KØãmÏÒs:»¸ÜfWê½Q‹@Æ¾ÑKõ‹D¹áåSê¤‘ì5~0èó*!½ml'Í¼¨X§$Áø‡P"(|U×ÄusŸWÃd›çêœ¦ÙÔ:"íÇ.Ø¤L‰·c%åä¢±ÊGŞv*ÌÀ÷u"±âúV¢†Û†ºú’ZïiÒ”LlC·-
¡7:§Àßí
{‡©z P¥.[Í=Ğ¨fÍÏ)°©‘Õnì)œçÒÑNÀšéä8ÔÏ,mFR¯ÑÍÚtXÖ€eÖx³m	áÌßjtÜ˜ù”§'X¡\Dâgİ+3j5®o±ú¡ØQEñ¬ñéÛJõ³ÚfcÔğ¿#$ŞueRtÕ-QDş“l ½Ä˜8ÀİÌ«n¼ş©úË†åU$NÚğJhKd¿˜*§¹èÆ*İñEëå©w¹Í…ºÅ‰ }9‚hñƒGÈ™3Nmİ3Ò„x:Ï]AÒšÍC®‡­²äÈEûÓ¥©;Ÿ5Ù©Ë”w»“@\Ù¡Ø(åò>mã\zòJ@¤˜EYHEÙF¥„lÍI•h,€ ÕÁ3T*ÁbÔ™	Ê*èè-¼Í‡Ê7à‘¸ìÏwuı£bµ+ı^ŞcaÚ¸Ÿ0DÈX¦MTñy &	x±G¡íÇê8”˜¦H¢*ü¦èB‹qõñ!=§âlŠh|Ñ¥9Dæª4:›
õHÒİù~/C~D†\F^dzn`9ÿæ£ÎD¾0€¤Dœ©dáî ïµYLìä¯Y =lgÙÛ·5àçØWz9É{!l*Êl"é p»áábQ¹>àú¬ğ¾¨=<g
ğı%•]I«'ĞÔö”Ò•m±à$ZÙ˜‹tPº…!¢Ğ	iæ&œJúJ“Vëô‚§^’ Ê~ığ¿ğµ
C˜ò#ä‘ç B¦ïÁ$q/fEjªæY©­ë¯èù¹ùâ}šÁÈN›SØ´ş+ê8Ò4ãC©Ñ^©hşÀİVˆ‰9+G±Qt!£»ìõ7'ıış–¦!·wûch'ã·ì ı¨Šî¥ÑÃ4ëJ´½šŸÙ9¼ÒşÙã†0hÄ|šz_¹L¦EOá¡|¹K¸¯Å AôkŞ”ê!ÛCmêZrF3os‡—ıj ÅúfÆóù$“%–2OœÏrÙ2Ş÷°5m­zwúK:oÊ¦˜‰úl÷NÅô6³ô Ã¦;ErÒ<"©‹øßÊ™_	ƒ’-ŠÖù‘‹|çÁ)WŠxrhâºˆµ˜ ³ôk¹d¾kû;¤šX(T=äå+´xù´Öİ‹æI×“¯Ü¨ã)eú^C¼åÃ£‰ïÂàËX¦&ÿÅrWpE|_ìQ\*AEä_µºÊU8¶Ö°m.¨ã_ŞmÓ¬ÇÅ6j@2¡A¯­-…2™à^ÄA­(–#úÆŒ×c®Ìwõ¯,C>F«ìË%‚'í‹+çÉ³Òˆ¥”MPr‘ñÖ¹E‰ş›}¸2¨…Ôkk'É‡Œ½Zz×JĞ’Òz“ljUĞó Òúä™‰Ä¹ıxx¬k‘NÿEµJ90)ÁşvU3Y ÷)³o(_[ÿ~úF0Lˆß‰É­éÏOø[¦\C´>|ÏZÛş^æjV
‹å8sZV†àçDêkjÔ–—x“.4„Vfj‰‡¸¥g{}ó\r¼’ÂôI(\˜™¤26rí*“;Y=#y‘}Ë‰ÓlùwU*Ø{‡ªÕ1-khæjs%¯¦,—û¬É˜2ÆÓ²·„‘E Á7ìs¨NQksq‚ê&p'İ)£n;…Zu`çÄ_o±ÍêŒtå ¬*ºÀ÷/&¨Í’ =vN/ê
¤Ës8dİ¶1F~OƒiF`‘.íoÒ`‘JA[8òÙçp’üÚÆÙRİÔ)j¾éEe–*bë—_XŸ?"Ì¯’mÍµye4a‰,°€‘täÓµÔ+föv¦¶\Bk÷‚2…ø³Šé:¬½ü™ŒJ$5ÆÚicms¸İH&ÌÙWäÀÓÀ`¨Gæ^I-Cá¾àYP‡!3%ñüG³ÒQé‹½Q»F@ÆË	çË]¼XSlpGƒw/;ÄÄ‚Å–ğÂñ^éÓÌö,ùÈ
Hï}Œ¾gB%*<•\ÀcÛ°œX™¨æÒm÷”Ôäa‡g‰g‡3?˜HwLßò\uìon€šÖ/{1ß±×µñÅ²ğÀ,ımW^åªïj®‘O»»ÑEº-•¢¦úªµ ÔŸ*<½Bä V®cm‰ŞIr®õê-@Ö'[¿ü*ô»A¯úŸ~qÕe–şÖú*À¢î ?€!õ¾¶wÆB-Å|V_d	Ì‚Ó‚ç{´é›Pfº×ÆL¤ª<,$.¹%×pL…z€,»EÖÀË¡<V™Ï~~îbMë‰ËLˆ<éË-÷Bå@ræJé$_K”W¦ÑÙú$ô(s_®{G®M"›Ú.lNê»	ÄÉ²¡dg5TT.IŒ ({ÑK]ˆÈÚ Ö9ş'y‘¶.ª.ÕL¿J5‚6±¤ï&xg¢9A(H“µ‚°­¹dLÔoÄJu›¼Œ‹v¡G‹8EÕÆˆı0˜í¸Òzr(a‹0NBu€çfÕ#KŒ0á:Ïï>Ó Î»ÿ;$ÃXKıèW9@IÅDe[]v]rê`ğŒ[	a;ç=µ=r}¨øSqÆ”Ÿæã~Ü]ûƒÙµG%„ÃæØ½ˆBşŞ™ù)˜OÇCBúWßş:.²¡ŸAº-üp#FÃ'¤‘ou{4ÇÊÅ,×tÕøøÄ†€İüóO'˜"ö;ÓìU€ÏnUî¦ ÛPšÌÙöÆÜ_úYŞæF·8ÿsÖÉ£=f;ı•$xİ6£q›lúèáv`·¹¡QàE¤ 0[e2$ü£·pò.R¶¦—(‘¼Aí+Œ0=À-‡N M;¨2ˆœ°mßB¹ÅÁ°×oµªiÂQˆãÔı–EUÜ–ë÷oÇ°Yn}3ÄH-»î|şi(¹ëT'‰#tèwGé+kÛqÇhªï†õv…³ÙĞä.ÀöïŸ	`§¹¸y	ÇÍ#İ]U^÷èØ©ê
ï,¥—›ÒW‡kgÍı¶Ô2×“³}™ÉãaGçÁ2:ï#ÄM¢mö@GÓp³T¾³T?ˆ·gÉ4.7[=‹2õœf_2*qN.9~ƒö4Xˆ+y‡Î—qÀíÏˆç.¼ä²ÏÆÎfrÈ<à'	§¦ã¥	O¥b„Šà\Ôp—ß€!]¤y&Ö>ÊC?VDóó:ÌòË„ÛJ§`Ev’Ú.™*u­c‹„ıÎ‡vÌ„¶k›'ã†´Â‰0>©+n™ÛW#§Sû³xç`€¦MŠ£^ ÷DpªÒpá‚DâçíàÓ1	µaV}&ë“ùŸc\9ù¼0ì½aÜRæ)‹A±ş2%òÿu¨xBEûÌFL  Jÿœ»–»„í5.a‚ãl­W˜ÀrÛg;
CWÔéC‘ãQæšÖÅÕæ¡«qç0ÕÀa~’uw•Á ´R%ß2Îÿ©"±ì¾6_ä7Iw,NùBßÕ¦¬!Î ¨&7qõİGœ=ÅyÌ!Vû-øj´$~ôºSçŸŒst_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;
	int i, j;

	/*
	 * The caller must hold the graph lock, ensure we've got IRQs
	 * disabled to make this an IRQ-safe lock.. for recursion reasons
	 * lockdep won't complain about its own locking errors.
	 */
	if (lockdep_assert_locked())
		return 0;

	chain = alloc_lock_chain();
	if (!chain) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAINS too low!");
		dump_stack();
		return 0;
	}
	chain->chain_key = chain_key;
	chain->irq_context = hlock->irq_context;
	i = get_first_held_lock(curr, hlock);
	chain->depth = curr->lockdep_depth + 1 - i;

	BUILD_BUG_ON((1UL << 24) <= ARRAY_SIZE(chain_hlocks));
	BUILD_BUG_ON((1UL << 6)  <= ARRAY_SIZE(curr->held_locks));
	BUILD_BUG_ON((1UL << 8*sizeof(chain_hlocks[0])) <= ARRAY_SIZE(lock_classes));

	j = alloc_chain_hlocks(chain->depth);
	if (j < 0) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAIN_HLOCKS too low!");
		dump_stack();
		return 0;
	}

	chain->base = j;
	for (j = 0; j < chain->depth - 1; j++, i++) {
		int lock_id = hlock_id(curr->held_locks + i);

		chain_hlocks[chain->base + j] = lock_id;
	}
	chain_hlocks[chain->base + j] = hlock_id(hlock);
	hlist_add_head_rcu(&chain->entry, hash_head);
	debug_atomic_inc(chain_lookup_misses);
	inc_chains(chain->irq_context);

	return 1;
}

/*
 * Look up a dependency chain. Must be called with either the graph lock or
 * the RCU read lock held.
 */
static inline struct lock_chain *lookup_chain_cache(u64 chain_key)
{
	struct hlist_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;

	hlist_for_each_entry_rcu(chain, hash_head, entry) {
		if (READ_ONCE(chain->chain_key) == chain_key) {
			debug_atomic_inc(chain_lookup_hits);
			return chain;
		}
	}
	return NULL;
}

/*
 * If the key is not present yet in dependency chain cache then
 * add it and return 1 - in this case the new dependency chain is
 * validated. If the key is already hashed, return 0.
 * (On return with 1 graph_lock is held.)
 */
static inline int lookup_chain_cache_add(struct task_struct *curr,
					 struct held_lock *hlock,
					 u64 chain_key)
{
	struct lock_class *class = hlock_class(hlock);
	struct lock_chain *chain = lookup_chain_cache(chain_key);

	if (chain) {
cache_hit:
		if (!check_no_collision(curr, hlock, chain))
			return 0;

		if (very_verbose(class)) {
			printk("\nhash chain already cached, key: "
					"%016Lx tail class: [%px] %s\n",
					(unsigned long long)chain_key,
					class->key, class->name);
		}

		return 0;
	}

	if (very_verbose(class)) {
		printk("\nnew hash chain, key: %016Lx tail class: [%px] %s\n",
			(unsigned long long)chain_key, class->key, class->name);
	}

	if (!graph_lock())
		return 0;

	/*
	 * We have to walk the chain again locked - to avoid duplicates:
	 */
	chain = lookup_chain_cache(chain_key);
	if (chain) {
		graph_unlock();
		goto cache_hit;
	}

	if (!add_chain_cache(curr, hlock, chain_key))
		return 0;

	return 1;
}

static int validate_chain(struct task_struct *curr,
			  struct held_lock *hlock,
			  int chain_head, u64 chain_key)
{
	/*
	 * Trylock needs to maintain the stack of held locks, but it
	 * does not add new dependencies, because trylock can be done
	 * in any order.
	 *
	 * We look up the chain_key and do the O(N^2) check and update of
	 * the dependencies only if this is a new dependency chain.
	 * (If lookup_chain_cache_add() return with 1 it acquires
	 * graph_lock for us)
	 */
	if (!hlock->trylock && hlock->check &&
	    lookup_chain_cache_add(curr, hlock, chain_key)) {
		/*
		 * Check whether last held lock:
		 *
		 * - is irq-safe, if this lock is irq-unsafe
		 * - is softirq-safe, if this lock is hardirq-unsafe
		 *
		 * And check whether the new lock's dependency graph
		 * could lead back to the previous lock:
		 *
		 * - within the current held-lock stack
		 * - across our accumulated lock dependency records
		 *
		 * any of these scenarios could lead to a deadlock.
		 */
		/*
		 * The simple case: does the current hold the same lock
		 * already?
		 */
		int ret = check_deadlock(curr, hlock);

		if (!ret)
			return 0;
		/*
		 * Add dependency only if this lock is not the head
		 * of the chain, and if the new lock introduces no more
		 * lock dependency (because we already hold a lock with the
		 * same lock class) nor deadlock (because the nest_lock
		 * serializes nesting locks), see the comments for
		 * check_deadlock().
		 */
		if (!chain_head && ret != 2) {
			if (!check_prevs_add(curr, hlock))
				return 0;
		}

		graph_unlock();
	} else {
		/* after lookup_chain_cache_add(): */
		if (unlikely(!debug_locks))
			return 0;
	}

	return 1;
}
#else
static inline int validate_chain(struct task_struct *curr,
				 struct held_lock *hlock,
				 int chain_head, u64 chain_key)
{
	return 1;
}

static void init_chain_block_buckets(void)	{ }
#endif /* CONFIG_PROVE_LOCKING */

/*
 * We are building curr_chain_key incrementally, so double-check
 * it from scratch, to make sure that it's done correctly:
 */
static void check_chain_key(struct task_struct *curr)
{
#ifdef CONFIG_DEBUG_LOCKDEP
	struct held_lock *hlock, *prev_hlock = NULL;
	unsigned int i;
	u64 chain_key = INITIAL_CHAIN_KEY;

	for (i = 0; i < curr->lockdep_depth; i++) {
		hlock = curr->held_locks + i;
		if (chain_key != hlock->prev_chain_key) {
			debug_locks_off();
			/*
			 * We got mighty confused, our chain keys don't match
			 * with what we expect, someone trample on our task state?
			 */
			WARN(1, "hm#1, depth: %u [%u], %016Lx != %016Lx\n",
				curr->lockdep_depth, i,
				(unsigned long long)chain_key,
				(unsigned long long)hlock->prev_chain_key);
			return;
		}

		/*
		 * hlock->class_idx can't go beyond MAX_LOCKDEP_KEYS, but is
		 * it registered lock class index?
		 */
		if (DEBUG_LOCKS_WARN_ON(!test_bit(hlock->class_idx, lock_classes_in_use)))
			return;

		if (prev_hlock && (prev_hlock->irq_context !=
							hlock->irq_context))
			chain_key = INITIAL_CHAIN_KEY;
		chain_key = iterate_chain_key(chain_key, hlock_id(hlock));
		prev_hlock = hlock;
	}
	if (chain_key != curr->curr_chain_key) {
		debug_locks_off();
		/*
		 * More smoking hash instead of calculating it, damn see these
		 * numbers float.. I bet that a pink elephant stepped on my memory.
		 */
		WARN(1, "hm#2, depth: %u [%u], %016Lx != %016Lx\n",
			curr->lockdep_depth, i,
			(unsigned long long)chain_key,
			(unsigned long long)curr->curr_chain_key);
	}
#endif
}

#ifdef CONFIG_PROVE_LOCKING
static int mark_lock(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit new_bit);

static void print_usage_bug_scenario(struct held_lock *lock)
{
	struct lock_class *class = hlock_class(lock);

	printk(" Possible unsafe locking scenario:\n\n");
	printk("       CPU0\n");
	printk("       ----\n");
	printk("  lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_usage_bug(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit prev_bit, enum lock_usage_bit new_bit)
{
	if (!debug_locks_off() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("================================\n");
	pr_warn("WARNING: inconsistent lock state\n");
	print_kernel_ident();
	pr_warn("--------------------------------\n");

	pr_warn("inconsistent {%s} -> {%s} usage.\n",
		usage_str[prev_bit], usage_str[new_bit]);

	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] takes:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		lockdep_softirq_context(curr), softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		lockdep_softirqs_enabled(curr));
	print_lock(this);

	pr_warn("{%s} state was registered at:\n", usage_str[prev_bit]);
	print_lock_trace(hlock_class(this)->usage_traces[prev_bit], 1);

	print_irqtrace_events(curr);
	pr_warn("\nother info that might help us debug this:\n");
	print_usage_bug_scenario(this);

	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Print out an error if an invalid bit is set:
 */
static inline int
valid_state(struct task_struct *curr, struct held_lock *this,
	    enum lock_usage_bit new_bit, enum lock_usage_bit bad_bit)
{
	if (unlikely(hlock_class(this)->usage_mask & (1 << bad_bit))) {
		graph_unlock();
		print_usage_bug(curr, this, bad_bit, new_bit);
		return 0;
	}
	return 1;
}


/*
 * print irq inversion bug:
 */
static void
print_irq_inversion_bug(struct task_struct *curr,
			struct lock_list *root, struct lock_list *other,
			struct held_lock *this, int forwards,
			const char *irqclass)
{
	struct lock_list *entry = other;
	struct lock_list *middle = NULL;
	int depth;

	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("========================================================\n");
	pr_warn("WARNING: possible irq lock inversion dependency detected\n");
	print_kernel_ident();
	pr_warn("--------------------------------------------------------\n");
	pr_warn("%s/%d just changed the state of lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(this);
	if (forwards)
		pr_warn("but this lock took another, %s-unsafe lock in the past:\n", irqclass);
	else
		pr_warn("but this lock was taken by another, %s-safe lock in the past:\n", irqclass);
	print_lock_name(other->class);
	pr_warn("\n\nand interrupts could create inverse lock ordering between them.\n\n");

	pr_warn("\nother info that might help us debug this:\n");

	/* Find a middle lock (if one exists) */
	depth = get_lock_depth(other);
	do {
		if (depth == 0 && (entry != root)) {
			pr_warn("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}
		middle = entry;
		entry = get_lock_parent(entry);
		depth--;
	} while (entry && entry != root && (depth >= 0));
	if (forwards)
		print_irq_lock_scenario(root, other,
			middle ? middle->class : root->class, other->class);
	else
		print_irq_lock_scenario(other, root,
			middle ? middle->class : other->class, root->class);

	lockdep_print_held_locks(curr);

	pr_warn("\nthe shortest dependencies between 2nd lock and 1st lock:\n");
	root->trace = save_trace();
	if (!root->trace)
		return;
	print_shortest_lock_dependencies(other, root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Prove that in the forwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_forwards(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_root(&root, this);
	ret = find_usage_forwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(read_bit));
	}

	return 0;
}

/*
 * Prove that in the backwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_backwards(struct task_struct *curr, struct held_lock *this,
		      enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_rootb(&root, this);
	ret = find_usage_backwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(read_bit));
	}

	return 0;
}

void print_irqtrace_events(struct task_struct *curr)
{
	const struct irqtrace_events *trace = &curr->irqtrace;

	printk("irq event stamp: %u\n", trace->irq_events);
	printk("hardirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->hardirq_enable_event, (void *)trace->hardirq_enable_ip,
		(void *)trace->hardirq_enable_ip);
	printk("hardirqs last disabled at (%u): [<%px>] %pS\n",
		trace->hardirq_disable_event, (void *)trace->hardirq_disable_ip,
		(void *)trace->hardirq_disable_ip);
	printk("softirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->softirq_enable_event, (void *)trace->softirq_enable_ip,
		(void *)trace->softirq_enable_ip);
	printk("softirqs last disabled at (%u): [<%px>] %pS\n",
		trace->softirq_disable_event, (void *)trace->softirq_disable_ip,
		(void *)trace->softirq_disable_ip);
}

static int HARDIRQ_verbose(struct lock_class *class)
{
#if HARDIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int SOFTIRQ_verbose(struct lock_class *class)
{
#if SOFTIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int (*state_verbose_f[])(struct lock_class *class) = {
#define LOCKDEP_STATE(__STATE) \
	__STATE##_verbose,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline int state_verbose(enum lock_usage_bit bit,
				struct lock_class *class)
{
	return state_verbose_f[bit >> LOCK_USAGE_DIR_MASK](class);
}

typedef int (*check_usage_f)(struct task_struct *, struct held_lock *,
			     enum lock_usage_bit bit, const char *name);

static int
mark_lock_irq(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit new_bit)
{
	int excl_bit = exclusive_bit(new_bit);
	int read = new_bit & LOCK_USAGE_READ_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * Validate that this particular lock does not have conflicting
	 * usage states.
	 */
	if (!valid_state(curr, this, new_bit, excl_bit))
		return 0;

	/*
	 * Check for read in write conflicts
	 */
	if (!read && !valid_state(curr, this, new_bit,
				  excl_bit + LOCK_USAGE_READ_MASK))
		return 0;


	/*
	 * Validate that the lock dependencies don't have conflicting usage
	 * states.
	 */
	if (dir) {
		/*
		 * mark ENABLED has to look backwards -- to ensure no dependee
		 * has USED_IN state, which, again, would allow  recursion deadlocks.
		 */
		if (!check_usage_backwards(curr, this, excl_bit))
			return 0;
	} else {
		/*
		 * mark USED_IN has to look forwards -- to ensure no dependency
		 * has ENABLED state, which would allow recursion deadlocks.
		 */
		if (!check_usage_forwards(curr, this, excl_bit))
			return 0;
	}

	if (state_verbose(new_bit, hlock_class(this)))
		return 2;

	return 1;
}

/*
 * Mark all held locks with a usage bit:
 */
static int
mark_held_locks(struct task_struct *curr, enum lock_usage_bit base_bit)
{
	struct held_lock *hlock;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		enum lock_usage_bit hlock_bit = base_bit;
		hlock = curr->held_locks + i;

		if (hlock->read)
			hlock_bit += LOCK_USAGE_READ_MASK;

		BUG_ON(hlock_bit >= LOCK_USAGE_STATES);

		if (!hlock->check)
			continue;

		if (!mark_lock(curr, hlock, hlock_bit))
			return 0;
	}

	return 1;
}

/*
 * Hardirqs will be enabled:
 */
static void __trace_hardirqs_on_caller(void)
{
	struct task_struct *curr = current;

	/*
	 * We are going to turn hardirqs on, so set the
	 * usage bit for all held locks:
	 */
	if (!mark_held_locks(curr, LOCK_ENABLED_HARDIRQ))
		return;
	/*
	 * If we have softirqs enabled, then set the usage
	 * bit for all held locks. (disabled hardirqs prevented
	 * this bit from being set before)
	 */
	if (curr->softirqs_enabled)
		mark_held_locks(curr, LOCK_ENABLED_SOFTIRQ);
}

/**
 * lockdep_hardirqs_on_prepare - Prepare for enabling interrupts
 * @ip:		Caller address
 *
 * Invoked before a possible transition to RCU idle from exit to user or
 * guest mode. This ensures that all RCU operations are done before RCU
 * stops watching. After the RCU transition lockdep_hardirqs_on() has to be
 * invoked to set the final state.
 */
void lockdep_hardirqs_on_prepare(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs do not (and cannot) track lock dependencies, nothing to do.
	 */
	if (unlikely(in_nmi()))
		return;

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (unlikely(lockdep_hardirqs_enabled())) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * See the fine text that goes along with this variable definition.
	 */
	if (DEBUG_LOCKS_WARN_ON(early_boot_irqs_disabled))
		return;

	/*
	 * Can't allow enabling interrupts while in an interrupt handler,
	 * that's general bad form and such. Recursion, limited stack etc..
	 */
	if (DEBUG_LOCKS_WARN_ON(lockdep_hardirq_context()))
		return;

	current->hardirq_chain_key = current->curr_chain_key;

	lockdep_recursion_inc();
	__trace_hardirqs_on_caller();
	lockdep_recursion_finish();
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on_prepare);

void noinstr lockdep_hardirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs can happen in the middle of local_irq_{en,dis}able() where the
	 * tracking state and hardware state are out of sync.
	 *
	 * NMIs must save lockdep_hardirqs_enabled() to restore IRQ state from,
	 * and not rely on hardware state like normal interrupts.
	 */
	if (unlikely(in_nmi())) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;

		/*
		 * Skip:
		 *  - recursion check, because NMI can hit lockdep;
		 *  - hardware state check, because above;
		 *  - chain_key check, see lockdep_hardirqs_on_prepare().
		 */
		goto skip_checks;
	}

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (lockdep_hardirqs_enabled()) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * Ensure the lock stack remained unchanged between
	 * lockdep_hardirqs_on_prepare() and lockdep_hardirqs_on().
	 */
	DEBUG_LOCKS_WARN_ON(current->hardirq_chain_key !=
			    current->curr_chain_key);

skip_checks:
	/* we'll do an OFF -> ON transition: */
	__this_cpu_write(hardirqs_enabled, 1);
	trace->hardirq_enable_ip = ip;
	trace->hardirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(hardirqs_on_events);
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on);

/*
 * Hardirqs were disabled:
 */
void noinstr lockdep_hardirqs_off(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * Matching lockdep_hardirqs_on(), allow NMIs in the middle of lockdep;
	 * they will restore the software state. This ensures the software
	 * state is consistent inside NMIs as well.
	 */
	if (in_nmi()) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;
	} else if (__this_cpu_read(lockdep_recursion))
		return;

	/*
	 * So we're supposed to get called after you mask local IRQs, but for
	 * some reason the hardware doesn't quite think you did a proper job.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (lockdep_hardirqs_enabled()) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		__this_cpu_write(hardirqs_enabled, 0);
		trace->hardirq_disable_ip = ip;
		trace->hardirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(hardirqs_off_events);
	} else {
		debug_atomic_inc(redundant_hardirqs_off);
	}
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_off);

/*
 * Softirqs will be enabled:
 */
void lockdep_softirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c, avoids
	 * funny state and nesting things.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		debug_atomic_inc(redundant_softirqs_on);
		return;
	}

	lockdep_recursion_inc();
	/*
	 * We'll do an OFF -> ON transition:
	 */
	current->softirqs_enabled = 1;
	trace->softirq_enable_ip = ip;
	trace->softirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(softirqs_on_events);
	/*
	 * We are going to turn softirqs on, so set the
	 * usage bit for all held locks, if hardirqs are
	 * enabled too:
	 */
	if (lockdep_hardirqs_enabled())
		mark_held_locks(current, LOCK_ENABLED_SOFTIRQ);
	lockdep_recursion_finish();
}

/*
 * Softirqs were disabled:
 */
void lockdep_softirqs_off(unsigned long ip)
{
	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		current->softirqs_enabled = 0;
		trace->softirq_disable_ip = ip;
		trace->softirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(softirqs_off_events);
		/*
		 * Whoops, we wanted softirqs off, so why aren't they?
		 */
		DEBUG_LOCKS_WARN_ON(!softirq_count());
	} else
		debug_atomic_inc(redundant_softirqs_off);
}

static int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	if (!check)
		goto lock_used;

	/*
	 * If non-trylock use in a hardirq or softirq context, then
	 * mark the lock as used in these contexts:
	 */
	if (!hlock->trylock) {
		if (hlock->read) {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_HARDIRQ_READ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_SOFTIRQ_READ))
					return 0;
		} else {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock, LOCK_USED_IN_HARDIRQ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock, LOCK_USED_IN_SOFTIRQ))
					return 0;
		}
	}
	if (!hlock->hardirqs_off) {
		if (hlock->read) {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ_READ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ_READ))
					return 0;
		} else {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ))
					return 0;
		}
	}

lock_used:
	/* mark it as used: */
	if (!mark_lock(curr, hlock, LOCK_USED))
		return 0;

	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return LOCK_CHAIN_HARDIRQ_CONTEXT * !!lockdep_hardirq_context() +
	       LOCK_CHAIN_SOFTIRQ_CONTEXT * !!task->softirq_context;
}

static int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	unsigned int depth = curr->lockdep_depth;

	/*
	 * Keep track of points where we cross into an interrupt context:
	 */
	if (depth) {
		struct held_lock *prev_hlock;

		prev_hlock = curr->held_locks + depth-1;
		/*
		 * If we cross into another context, reset the
		 * hash key (this also prevents the checking and the
		 * adding of the dependency to 'prev'):
		 */
		if (prev_hlock->irq_context != hlock->irq_context)
			return 1;
	}
	return 0;
}

/*
 * Mark a lock with a usage bit, and validate the state transition:
 */
static int mark_lock(struct task_struct *curr, struct held_lock *this,
			     enum lock_usage_bit new_bit)
{
	unsigned int new_mask, ret = 1;

	if (new_bit >= LOCK_USAGE_STATES) {
		DEBUG_LOCKS_WARN_ON(1);
		return 0;
	}

	if (new_bit == LOCK_USED && this->read)
		new_bit = LOCK_USED_READ;

	new_mask = 1 << new_bit;

	/*
	 * If already set then do not dirty the cacheline,
	 * nor do any checks:
	 */
	if (likely(hlock_class(this)->usage_mask & new_mask))
		return 1;

	if (!graph_lock())
		return 0;
	/*
	 * Make sure we didn't race:
	 */
	if (unlikely(hlock_class(this)->usage_mask & new_mask))
		goto unlock;

	if (!hlock_class(this)->usage_mask)
		debug_atomic_dec(nr_unused_locks);

	hlock_class(this)->usage_mask |= new_mask;

	if (new_bit < LOCK_TRACE_STATES) {
		if (!(hlock_class(this)->usage_traces[new_bit] = save_trace()))
			return 0;
	}

	if (new_bit < LOCK_USED) {
		ret = mark_lock_irq(curr, this, new_bit);
		if (!ret)
			return 0;
	}

unlock:
	graph_unlock();

	/*
	 * We must printk outside of the graph_lock:
	 */
	if (ret == 2) {
		printk("\nmarked lock as {%s}:\n", usage_str[new_bit]);
		print_lock(this);
		print_irqtrace_events(curr);
		dump_stack();
	}

	return ret;
}

static inline short task_wait_context(struct task_struct *curr)
{
	/*
	 * Set appropriate wait type for the context; for IRQs we have to take
	 * into account force_irqthread as that is implied by PREEMPT_RT.
	 */
	if (lockdep_hardirq_context()) {
		/*
		 * Check if force_irqthreads will run us threaded.
		 */
		if (curr->hardirq_threaded || curr->irq_config)
			return LD_WAIT_CONFIG;

		return LD_WAIT_SPIN;
	} else if (curr->softirq_context) {
		/*
		 * Softirqs are always threaded.
		 */
		return LD_WAIT_CONFIG;
	}

	return LD_WAIT_MAX;
}

static int
print_lock_invalid_wait_context(struct task_struct *curr,
				struct held_lock *hlock)
{
	short curr_inner;

	if (!debug_locks_off())
		return 0;
	if (debug_locks_silent)
		return 0;

	pr_warn("\n");
	pr_warn("=============================\n");
	pr_warn("[ BUG: Invalid wait context ]\n");
	print_kernel_ident();
	pr_warn("-----------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("other info that might help us debug this:\n");

	curr_inner = task_wait_context(curr);
	pr_warn("context-{%d:%d}\n", curr_inner, curr_inner);

	lockdep_print_held_locks(curr);

	pr_warn("stack backtrace:\n");
	dump_stack();

	return 0;
}

/*
 * Verify the wait_type context.
 *
 * This check validates we take locks in the right wait-type order; that is it
 * ensures that we do not take mutexes inside spinlocks and do not attempt to
 * acquire spinlocks inside raw_spinlocks and the sort.
 *
 * The entire thing is slightly more complex because of RCU, RCU is a lock that
 * can be taken from (pretty much) any context but also has constraints.
 * However when taken in a stricter environment the RCU lock does not loosen
 * the constraints.
 *
 * Therefore we must look for the strictest environment in the lock stack and
 * compare that to the lock we're trying to acquire.
 */
static int check_wait_context(struct task_struct *curr, struct held_lock *next)
{
	u8 next_inner = hlock_class(next)->wait_type_inner;
	u8 next_outer = hlock_class(next)->wait_type_outer;
	u8 curr_inner;
	int depth;

	if (!next_inner || next->trylock)
		return 0;

	if (!next_outer)
		next_outer = next_inner;

	/*
	 * Find start of current irq_context..
	 */
	for (depth = curr->lockdep_depth - 1; depth >= 0; depth--) {
		struct held_lock *prev = curr->held_locks + depth;
		if (prev->irq_context != next->irq_context)
			break;
	}
	depth++;

	curr_inner = task_wait_context(curr);

	for (; depth < curr->lockdep_depth; depth++) {
		struct held_lock *prev = curr->held_locks + depth;
		u8 prev_inner = hlock_class(prev)->wait_type_inner;

		if (prev_inner) {
			/*
			 * We can have a bigger inner than a previous one
			 * when outer is smaller than inner, as with RCU.
			 *
			 * Also due to trylocks.
			 */
			curr_inner = min(curr_inner, prev_inner);
		}
	}

	if (next_outer > curr_inner)
		return print_lock_invalid_wait_context(curr, next);

	return 0;
}

#else /* CONFIG_PROVE_LOCKING */

static inline int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return 0;
}

static inline int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	return 0;
}

static inline int check_wait_context(struct task_struct *curr,
				     struct held_lock *next)
{
	return 0;
}

#endif /* CONFIG_PROVE_LOCKING */

/*
 * Initialize a lock instance's lock-class mapping info:
 */
void lockdep_init_map_type(struct lockdep_map *lock, const char *name,
			    struct lock_class_key *key, int subclass,
			    u8 inner, u8 outer, u8 lock_type)
{
	int i;

	for (i = 0; i < NR_LOCKDEP_CACHING_CLASSES; i++)
		lock->class_cache[i] 