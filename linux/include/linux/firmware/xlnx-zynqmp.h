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

drivers/media/i2c/tw9906.o: $(deps_drivers/media/i2c/tw9906.o)

$(deps_drivers/media/i2c/tw9906.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ELF                       "      4     (               èüÿÿÿS‹€”   ‹@\‹˜Ø   ‹ˆÔ   1À‰Z[‰
Ã´&    t& èüÿÿÿ‹€”   ƒ9‹@\uMƒywGÇA    ÇA    ÷€Ô    °  tÇA€  1ÀÇAà  Ãt& ÇA   1ÀÇA@  Ã´&    ¸êÿÿÿÃ´&    v èüÿÿÿ‹…Òu‹A…ÀuÇA   1ÀÃt& ¸êÿÿÿÃ´&    v èüÿÿÿÇÿ°  1ÀÇB    Ã´&    t& èüÿÿÿS‹X\‹ƒÈ   …Àtèüÿÿÿ‹ƒÀ   èüÿÿÿ‰Øèüÿÿÿ1À[ÃfèüÿÿÿUWVS‹¨À   ‰Ã‹¸”   …ít&‰èèüÿÿÿ‰Æ…Àt[‰ğ^_]Ã‰èèüÿÿÿ‰Æ…À…¯   ‹ƒÈ   …Àt1Òèüÿÿÿ¹   ºè  ¸ô  èüÿÿÿƒÇ¹   º    ‰øèüÿÿÿ‰ƒÌ   = ğÿÿ‡üÿÿÿ1ö…Àt—º   èüÿÿÿ¸ô  ¹   ºè  èüÿÿÿ‹ƒÌ   …Àt!1Òèüÿÿÿ¸ô  ¹   ºè  èüÿÿÿ‹ƒÌ   èüÿÿÿ1ö[‰ğ^_]Ã´&    ‰èèüÿÿÿé,ÿÿÿt& èüÿÿÿVS‹€”   ‹X\…Òt‰Ø[^éãşÿÿv ‹³À   ‰ğèüÿÿÿ‰ğèüÿÿÿ‹ƒÈ   …Àtº   èüÿÿÿ¹   ºè  ¸ô  èüÿÿÿ1À[^Ã´&    ´&    èüÿÿÿUWV‰ÆS~ƒì‹nX‹@…í„g   ‹P‹Rèüÿÿÿ%   =   …P   ¹À  ºà   ‰øèüÿÿÿ‰Ã…À„û   ‰¨Ä   ¹@   ‰òèüÿÿÿº   ‰øèüÿÿÿ‰ƒÀ   ƒøş„†   = ğÿÿ‡å   ¹   º<   ‰øèüÿÿÿ‰ƒÈ   = ğÿÿ‡  ‹n\‹…Ä   ‹ ƒèƒà÷…ı   ‹…”   ‹@\è¼ıÿÿ‰Â…ÀxF1Ò‰ğèüÿÿÿ‰Â‰ÆÁúƒæƒâ‰µÜ   ƒú…†   é~   t& ÇƒÀ       évÿÿÿ´&    f‹ƒÈ   …Àt‰$èüÿÿÿ‹$‹ƒÀ   ‰$èüÿÿÿ‹$ƒÄ‰Ğ[^_]Ãºôÿÿÿëï´&    fèüÿÿÿUWVSƒì‹”   ‹i‹r\…í…À   ‹¾Ğ   ‰È…ÿt4·W‰P‹–Ğ   ·RÇ@   Ç@   ‰PÇ@	   1ÀƒÄ[^_]Ã‹–Ô   öÆ°…   „Òtm½à   ¿@  _H‰D$O‰4$‰Øë3f·Q»€  ¾à  )Ó‰Ú÷ÚHÓ·Y)Ş‰ó÷ÛHŞÚ9ês‰Õ‰ÏƒÁ9ÈuË‹4$‹D$‰¾Ğ   …ÿ…Rÿÿÿ¶    ¸êÿÿÿémÿÿÿ¶    ¿   ë†´&    fèüÿÿÿWVS‹˜”   ‹C\÷Âÿ°  „Ã   ‰Ô   €æùºU   ‰ˆØ   ‰Øusèüÿÿÿ…Àˆ“   %ï   ºU   ¿   ¾   ‰Á‰Øèüÿÿÿ…Àur‰ñº   ‰ØÁéƒáƒÉèüÿÿÿ…ÀuW‰ùº   ‰Øèüÿÿÿ…ÀuE‰ğº	   ¶È‰Ø[^_éüÿÿÿt& èüÿÿÿ…Àx$%ï   ºU   ¿   ¾ğ   ƒÈ‰Á‰Øèüÿÿÿ…Àt[^_Ã´&    ¸êÿÿÿëî´&    fèüÿÿÿWVS‹°”   ‰Ó‹F\…Ò…Å   ‹€Ü   ¿   …À…š   º   ‰ğèüÿÿÿ…Àˆ~   %ø   º   	ø‰Á‰ğèüÿÿÿ…Àxeƒûº   ‰ğÿƒçƒûÛƒã@èüÿÿÿ…ÀxE%ñ   º   	ø‰Á‰ğèüÿÿÿ…Àx,º   ‰ğèüÿÿÿ…Àx¶Û%¿   º   	Ø[‰Á‰ğ^_éüÿÿÿ[^_Ãt& ƒø…À  ¿   éSÿÿÿt& ‹ˆĞ   …É„Ú  1ÿé9ÿÿÿ´&    t& èüÿÿÿUWVS‰Ëƒì‹ˆ”   ‰D$‹C‰T$‹Q\…À…  ‹C…À„  ƒø	…ò  ‹C‹KÇC   ÇC   ‰$‹‚Ô   öÄ°…ü   „À„\  ¾@  ¸   º@  zH‰\$1í‰Ë‰|$¿ÿÿÿÿë´&    ·B·r‹$)Á‰È÷ØHÁ·Î‰Ş)Î‰ñ÷ÙHÎÈ9øs‰Õ‰ÇƒÂ9T$uË‹\$…í„ï   ·Eƒ;‰C·u‰s„ˆ   ‹|$1ö‹‰‹C‰B‹C‰B‹C‰B‹C‰B‹C‰B‹C ‰B‹C$‰B‹C(‰B ‹C,‰B$‹C0‰B(‹C4‰B,ƒÄ‰ğ[^_]Ã´&    v ÇC	   éâşÿÿt& ¾à  ¸€  º   éÿÿÿt& ‹S…Òt	ƒú	…ç  ‹|$‹¿”   ‰|$‹\‹—Ô   ‰|$öÆ°u*„Ò„®  ÇD$@  º   ¹@  ë t& ¾êÿÿÿéjÿÿÿÇD$   º€  ¹à  ‰÷‰\$)Ï‰ù÷ÙHÏ‰Ç)×‰ú÷ÚH×‹|$,OƒÇH‰<$ë-·Q‰Ã‰÷)Ó‰Ú÷ÚHÓ·Y)ß‰û÷ÛHßÚ9ês‰L$‰ÕƒÁ9$uÎ‹|$‹D$‹\$‰‡Ğ   …À„¦  ‹|$º   ‰øèüÿÿÿ…Àxƒàº   €‰Á‰øèüÿÿÿ¹   ºˆ  ¸è  èüÿÿÿ‹D$‹l$º   ‹€Ä   ‹8‰èèüÿÿÿ‰Æ…ÀˆS  1Àƒÿº   ”Àæ¿   Áà‰ñ	Á‰èèüÿÿÿ‰Æ…Àˆ(  ‹D$º   ‹€Ä   ‹xGÿƒø¸    Gø‰èèüÿÿÿ‰Æ…Àˆ÷   ƒæøº   ‰è‰ñ	ù¶Éèüÿÿÿ‰Æ…Àˆ×   ‹D$º   ‹¸Ğ   ·O
¶G	Áùƒàƒáğ	Á‰è¶Éèüÿÿÿ‰Æ…Àˆ¡   ¶Oº   ‰èèüÿÿÿ‰Æ…Àˆ‡   ¶O
º   ‰èèüÿÿÿ‰Æ…Àxq¹L   ºk   ‰è‹}\èüÿÿÿ‰Æ…ÀxW¹`   ºl   ‰èèüÿÿÿ‰Æ…Àx@ƒ¿Ü   „™   ‹D$‹€Ğ   ·P·@…ö…Eıÿÿ‰S‰Cé:ıÿÿ¾êÿÿÿ´&    f‹|$º   ‰øèüÿÿÿ…Àxƒàº   €‰Á‰øèüÿÿÿ¸è  ¹   ºˆ  èüÿÿÿ‹D$Ç€Ğ       éŞüÿÿÇ‡Ğ       ¾êÿÿÿë¡éıÿÿ‹|$ºN   ‰øèüÿÿÿ‰Æ…Àx„æˆ   ºN   ‰ø‰ñèüÿÿÿ‰Æ…À‰3ÿÿÿéaÿÿÿ                       ğ                   ù                                                                                                       0       Ğ   ğ           ğ  ğ  à  à      rstb Unable to get GPIO "rstb" xti Unable to get xti clock
 pdn Unable to get GPIO "pdn" bus width error
 Product ID error %x:%x
 tw9910 Product ID %0x:%0x
 un-supported revision
 norm select error
 Field type %d invalid
 drivers/media/i2c/tw9910.c tw9910 PAL SQ PAL CCIR601 PAL SQ (CIF) PAL CCIR601 (CIF) PAL SQ (QCIF) PAL CCIR601 (QCIF) NTSC SQ NTSC CCIR601 NTSC SQ (CIF) NTSC CCIR601 (CIF) NTSC SQ (QCIF) NTSC CCIR601 (QCIF) h   Wèüÿÿÿ‹³À   ‰ğèüÿÿÿ‰ğèüÿÿÿ‹ƒÈ   ZY…Àtº   èüÿÿÿ¹   ºè  ¸ô  èüÿÿÿ‹³Ì   éD  h    WèüÿÿÿXZºûÿÿÿéÒ  h    WèüÿÿÿºêÿÿÿY[éÒ  ¨„°   VRhj   Wèüÿÿÿ‹…”   ‹p\‹¾À   ‰øèüÿÿÿ‰øèüÿÿÿ‹†È   ƒÄ…Àtoº   èüÿÿÿºè  ¹   ¸ô  èüÿÿÿºíÿÿÿé¬  h#   Wèüÿÿÿ‹“À   _]éÒ  hY   WèüÿÿÿXZºíÿÿÿé¬  h@   Wèüÿÿÿ‹“È   Y^éÁ  ºíÿÿÿé¬  Vjh‚   Wèüÿÿÿ‹…”   Ç…Ô    °  Ç…Ø       Ç…Ğ      ‹p\‹¾À   ‰øèüÿÿÿ‰øèüÿÿÿ‹†È   ƒÄ…Àtº   èüÿÿÿ¹   ºè  ¸ô  èüÿÿÿ‰Øèüÿÿÿ‰Â…À„Ò  é¬  ƒÆh   Vèüÿÿÿ¸êÿÿÿ[^éœ  ƒÆh´   VèüÿÿÿXƒÈÿZéœ  ƒÁP¾êÿÿÿhÇ   QèüÿÿÿƒÄé    TW9910: missing platform data!
 I2C-Adapter doesn't support I2C_FUNC_SMBUS_BYTE_DATA
 O  Ş   	èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿlicense=GPL v2 author=Kuninori Morimoto description=V4L2 driver for TW9910 video decoder                    tw9910                                                                                     `                   ğ  à  0                                                   ğ              Ğ           à                                                                                                                         @    Ğ@    €      h   2  À    @  ´                            S  €à  [  Ğà  h  @ğ   v  hğ   ‰    x   ˜  ´ x    GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                            
       $        0   f     ,       &     B   Ğ        S   ğ   .                                	 a      ü     q       T    	 †      a                   •     W    ¢   @         ´   T   p   	 Æ      H     Ù   ğ  ÷     è   @  H     ú   ğ  ç       à  õ       Ä  2    	 ,  à  ¯    ;  ö      	               O           f      €     x      
                ¦          ¼  (   1     ×      0                   á     0     ø      P       `   @     &             1             ;             C             `             l             w             ‡             š             ­             »             Å             Ñ             ê             ÷                                       -             6             Q             k             y                        ™             ¨      
     ·      0      tw9910.c tw9910_g_std tw9910_get_selection tw9910_enum_mbus_code tw9910_g_tvnorms tw9910_remove tw9910_power_on tw9910_power_on.cold tw9910_s_power tw9910_probe tw9910_subdev_ops tw9910_probe.cold tw9910_ntsc_scales tw9910_get_fmt tw9910_pal_scales tw9910_s_std tw9910_s_stream tw9910_s_stream.cold tw9910_set_fmt tw9910_set_fmt.cold tw9910_i2c_driver_init tw9910_i2c_driver tw9910_i2c_driver_exit __UNIQUE_ID_license268 __UNIQUE