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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ELF                      L"      4     (               Ë¸ˇˇˇãÖ¿uãAÉ¯wã≈¨  âA1¿√∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇâ¬â»ãIÖ…upãä8  âHãä<  âHãä@  âHãäD  âHãäH  âHãäL  âHãäP  âH ãäT  âH$ãäX  âH(ãä\  âH,ãä`  âH0ãíd  âP41¿√ê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇãà4  âJãÄ0  âB1¿√ç∂    Ë¸ˇˇˇSãX\âÿË¸ˇˇˇãClË¸ˇˇˇ1¿[√çt& Ë¸ˇˇˇÉÏãÄî   dã    âL$1…πÄˇˇfâL$Ö“t7∆D$j π   çT$Ë¸ˇˇˇX∏ºç Ë¸ˇˇˇãD$d+    u1¿Éƒ√çt& ∆D$Î«Ë¸ˇˇˇçt& Ë¸ˇˇˇSâ√ÉÏãJd°    âD$1¿Ö…tãBÖ¿Öﬂ   «B   ∏   «B   âÉ0  ãB∫Äˇˇπ   fâT$âÉ4  ãÉî   ∆D$ j çT$Ë¸ˇˇˇãì0  πÅˇˇãÉî   fâL$π   àt$j çT$	Ë¸ˇˇˇãì0  πÇˇˇãÉî   fâL$	π   àT$j çT$Ë¸ˇˇˇãì4  ãÉî   ªÑˇˇfâ\$π   àT$j çT$Ë¸ˇˇˇÉƒãD$d+    uÉƒ1¿[√çv k…9»Ü)ˇˇˇÈˇˇˇË¸ˇˇˇçv Ë¸ˇˇˇUâ≈WVSÉÏd°    âD$ãEãPãRË¸ˇˇˇ®Ñ)  ãEXâ$Ö¿Ñ"  h    çuãπ   âË¸ˇˇˇâ√XÖ€Ö¸ˇˇˇ∏d   Ë¸ˇˇˇπ¿  ∫l  âË¸ˇˇˇâ√Ö¿Ñ„  π@   âÍæ†  Ë¸ˇˇˇπ   ∫ÅˇˇÎê∑Nf¡¬àL$ãÉî   É∆fâT$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÕ∫¿  ∆D$ãÉî   π   fâT$j çT$	Ë¸ˇˇˇπ¿D  ãÉî   fâL$	π   ∆D$j çT$Ë¸ˇˇˇπ   ∫¯*  ∏'  Ë¸ˇˇˇπ   ^∫oÄˇˇ_æ|  Îçt& ∑Nf¡¬àL$ãÉî   É∆fâT$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÕ∫Ñ   æ¥  ø@  Îçt& ∑Wf¡∆àT$É«π   fât$ãÉî   j çT$	Ë¸ˇˇˇ∑7XfÖˆuÕ∫%Üˇˇ∆D$ãÉî   π   fâT$ø‡  j çT$	Ë¸ˇˇˇ∫  YÎçv ∑wf¡¬âÒãÉî   É«fâT$àL$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÀ∫Äˇˇ∆D$ π   ãÉî   fâT$æÇˇˇøÑˇˇ«É0     «É4     j çT$	Ë¸ˇˇˇãì0  πÅˇˇãÉî   fâL$	π   àt$j çT$Ë¸ˇˇˇãì0  ãÉî   π   fât$ç≥¿   àT$j çT$Ë¸ˇˇˇãì4  ãÉî   π   fâ|$çª8  àT$j çT$Ë¸ˇˇˇ1¿π   ∑UÛ´ãD$«É8  Ä  «É<  ‡  «É@     «ÉD     «ÉH     ã âÉh  ãEçà  QçQRˇ∞   ãETˇ0h,   Ë¸ˇˇˇ1…∫   âÉƒ(j Ë¸ˇˇˇπ	ò ∫0  âj há   j jj hˇ   j j Ë¸ˇˇˇπ	ò ∫0  âÉƒ$j jxj jj hˇ   j j Ë¸ˇˇˇπ	ò ∫0  âÉƒ j j j jj jj j Ë¸ˇˇˇπ	ò ∫0  âÉƒ j j j jj jj j Ë¸ˇˇˇâslãõ¸   âÉƒ Ö€u6Ë¸ˇˇˇâ√Ö¿uãD$d+    u:Éƒâÿ[^_]√çt& âË¸ˇˇˇÎ‹ç¥&    Ë¸ˇˇˇÎŒª˚ˇˇˇÎ«ªÍˇˇˇÎ¿ªÙˇˇˇÎπË¸ˇˇˇç¥&    ç¥&    êË¸ˇˇˇSÉÏãHdã    âT$1“ãP(Å˙	ò Ñê   wNÅ˙	ò tnÅ˙	ò u^∫∂ˇˇãY‘ã@|fâT$àD$π   âÿj çT$Ë¸ˇˇˇX1¿ãT$d+    uZÉƒ[√çv Å˙	ò u∫ºˇˇãY‘ã@|fâT$Î∏ç∂    ∏ÍˇˇˇÎƒêãY‘π¥ˇˇã@|fâL$Îòç∂    ãY‘π∫ˇˇã@|fâL$ÎÄË¸ˇˇˇêË¸ˇˇˇUWVâ∆SÉÏd°    âD$1¿ãAÖ¿Öé   ãAâ◊âÀçê˙ﬂˇˇÉ‚˝Ñ%  =  Ñ*  «A   π   ãCãk«C   âKâ¬É‚¸=Å  ∏Ä  C–âËÉ‡¸Å˝·  Ω‡  C≈ã+âSâCÖÌtEãC=   Ñó  =   ÑD  =  tu∏ÍˇˇˇãT$d+    Ö¬  Éƒ[^_]√ç¥&    ãâãSâPãSâPãSâPãSâPãSâPãS âPãS$âPãS(âP ãS,âP$ãS0âP(ãS4âP,1¿Îñç∂    ∫∞ˇˇç|$∆D$ãÜî   fâT$π   â˙j Ë¸ˇˇˇπ%îˇˇãÜî   fâL$∆D$ j π   â˙Ë¸ˇˇˇãCY]ãÆî   =Ä  Ñ∏  =@  ÖÌ  Å{   Ñ⁄  ∏Äˇˇ∆D$π   â˙fâD$âËΩÑˇˇj Ë¸ˇˇˇãSπÉˇˇãÜî   fâL$π   àt$â˙j Ë¸ˇˇˇãSãÜî   π   fâl$	ΩàˇˇàT$â˙j Ë¸ˇˇˇãSπáˇˇãÜî   fâL$π   àt$â˙j Ë¸ˇˇˇãSãÜî   π   fâl$àT$â˙j Ë¸ˇˇˇ∫ûˇˇãÜî   π   fâT$â˙∆D$j Ë¸ˇˇˇÉƒãCâÜ8  ãCâÜ<  ãCâÜ@  ãCâÜD  ãCâÜH  ãCâÜL  ãC âÜP  ãC$âÜT  ãC(âÜX  ãC,âÜ\  ãC0âÜ`  ãC4âÜd  1¿ÈÃ˝ˇˇçt& ø∞ˇˇ∆D$ ãÜî   π   fâ|$ç|$Ω%ñˇˇj â˙Ë¸ˇˇˇãÜî   fâl$∆D$È3˛ˇˇç¥&    ∫∞ˇˇç|$∆D$ ãÜî   fâT$π   â˙j Ë¸ˇˇˇπ%ñˇˇãÜî   fâL$∆D$ÈÎ˝ˇˇç¥&    =†   u!É{xÖ˛ˇˇ∏Äˇˇ∆D$fâD$Î~ç∂    =`  u!Å{   Ö‰˝ˇˇ∏Äˇˇ∆D$fâD$ÎSçv =∞   u!Å{ê   Öº˝ˇˇ∏Äˇˇ∆D$fâD$Î+çv É¯XÖü˝ˇˇÉ{HÖï˝ˇˇπÄˇˇ∆D$fâL$ç∂    j π   â˙âËË¸ˇˇˇXÈC˛ˇˇç¥&    çv Å{‡  ÖS˝ˇˇ∫Äˇˇ∆D$fâT$Î¬fêπ   ÈË˚ˇˇç∂    π   Èÿ˚ˇˇ∏Äˇˇ∆D$fâD$ÎïË¸ˇˇˇ                    ê  ‡                                                                                                                          0   ¿   ‡      p  ê    ‡      VS6624 Chip Enable vs6624   3%s %d-%04x: failed to 