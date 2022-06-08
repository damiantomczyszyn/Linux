lude/config/SHMEM) \
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
  include/linu/x86/include/uapi/asm/bitsperlong.h \
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
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
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
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
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
  include/linux/kstrtox.h \
  include/ELF                       "      4     (               Ë¸ˇˇˇSãÄî   ã@\ãòÿ   ãà‘   1¿âZ[â
√ç¥&    çt& êË¸ˇˇˇãÄî   É9ã@\uMÉywG«A    «A    ˜Ä‘    ∞  t«AÄ  1¿«A‡  √çt& «A   1¿«A@  √ç¥&    ∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇãÖ“uãAÖ¿u«A   1¿√çt& ∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇ«ˇ∞  1¿«B    √ç¥&    çt& Ë¸ˇˇˇSãX\ãÉ»   Ö¿tË¸ˇˇˇãÉ¿   Ë¸ˇˇˇâÿË¸ˇˇˇ1¿[√fêË¸ˇˇˇUWVSã®¿   â√ã∏î   ÖÌt&âËË¸ˇˇˇâ∆Ö¿t[â^_]√êâËË¸ˇˇˇâ∆Ö¿ÖØ   ãÉ»   Ö¿t1“Ë¸ˇˇˇπ   ∫Ë  ∏Ù  Ë¸ˇˇˇÉ«π   ∫    â¯Ë¸ˇˇˇâÉÃ   = ˇˇá¸ˇˇˇ1ˆÖ¿tó∫   Ë¸ˇˇˇ∏Ù  π   ∫Ë  Ë¸ˇˇˇãÉÃ   Ö¿t!1“Ë¸ˇˇˇ∏Ù  π   ∫Ë  Ë¸ˇˇˇãÉÃ   Ë¸ˇˇˇ1ˆ[â^_]√ç¥&    êâËË¸ˇˇˇÈ,ˇˇˇçt& Ë¸ˇˇˇVSãÄî   ãX\Ö“tâÿ[^È„˛ˇˇçv ã≥¿   âË¸ˇˇˇâË¸ˇˇˇãÉ»   Ö¿t∫   Ë¸ˇˇˇπ   ∫Ë  ∏Ù  Ë¸ˇˇˇ1¿[^√ç¥&    ç¥&    êË¸ˇˇˇUWVâ∆Sç~ÉÏãnXã@ÖÌÑg   ãPãRË¸ˇˇˇ%   =   ÖP   π¿  ∫‡   â¯Ë¸ˇˇˇâ√Ö¿Ñ˚   â®ƒ   π@   âÚË¸ˇˇˇ∫   â¯Ë¸ˇˇˇâÉ¿   É¯˛ÑÜ   = ˇˇáÂ   π   ∫<   â¯Ë¸ˇˇˇâÉ»   = ˇˇá  ãn\ãÖƒ   ã ÉËÉ‡˜Ö˝   ãÖî   ã@\Ëº˝ˇˇâ¬Ö¿xF1“âË¸ˇˇˇâ¬â∆¡˙ÉÊÉ‚âµ‹   É˙ÖÜ   È~   çt& «É¿       Èvˇˇˇç¥&    fêãÉ»   Ö¿tâ$Ë¸ˇˇˇã$ãÉ¿   â$Ë¸ˇˇˇã$Éƒâ–[^_]√∫ÙˇˇˇÎÔç¥&    fêË¸ˇˇˇUWVSÉÏãêî   ãiãr\ÖÌÖ¿   ãæ–   â»Öˇt4∑WâPãñ–   ∑R«@   «@   âP«@	   1¿Éƒ[^_]√êãñ‘   ˆ∆∞ÖÅ   Ñ“tmΩ‡   ø@  ç_HâD$çOâ4$âÿÎ3fê∑QªÄ  æ‡  )”â⁄˜⁄H”∑Y)ﬁâÛ˜€Hﬁ⁄9Ísâ’âœÉ¡9»uÀã4$ãD$âæ–   ÖˇÖRˇˇˇç∂    ∏ÍˇˇˇÈmˇˇˇç∂    ø†  ÎÜç¥&    fêË¸ˇˇˇWVSãòî   ãC\˜¬ˇ∞  Ñ√   âê‘   ÄÊ˘∫U   âàÿ   âÿusË¸ˇˇˇÖ¿àì   %Ô   ∫U   ø   æ   â¡âÿË¸ˇˇˇÖ¿urâÒ∫   âÿ¡ÈÉ·É…Ë¸ˇˇˇÖ¿uWâ˘∫   âÿË¸ˇˇˇÖ¿uEâ∫	   ∂»âÿ[^_È¸ˇˇˇçt& Ë¸ˇˇˇÖ¿x$%Ô   ∫U   ø   æ   É»â¡âÿË¸ˇˇˇÖ¿té[^_√ç¥&    ∏ÍˇˇˇÎÓç¥&    fêË¸ˇˇˇWVSã∞î   â”ãF\Ö“Ö≈   ãÄ‹   ø   Ö¿Öö   ∫   âË¸ˇˇˇÖ¿à~   %¯   ∫   	¯â¡âË¸ˇˇˇÖ¿xeÉ˚∫   âˇÉÁÉ˚€É„@Ë¸ˇˇˇÖ¿xE%Ò   ∫   	¯â¡âË¸ˇˇˇÖ¿x,∫   âË¸ˇˇˇÖ¿x∂€%ø   ∫   	ÿ[â¡â^_È¸ˇˇˇê[^_√çt& É¯Ö¿  ø   ÈSˇˇˇçt& êãà–   Ö…Ñ⁄  1ˇÈ9ˇˇˇç¥&    çt& Ë¸ˇˇˇUWVSâÀÉÏãàî   âD$ãCâT$ãQ\Ö¿Öû  ãCÖ¿Ñ  É¯	ÖÚ  ãCãK«C   «C   â$ãÇ‘   ˆƒ∞Ö¸   Ñ¿Ñ\  æ@  ∏   ∫@  çzHâ\$1ÌâÀâ|$øˇˇˇˇÎç¥&    ∑B∑rã$)¡â»˜ÿH¡∑Œâﬁ)ŒâÒ˜ŸHŒ»9¯sâ’â«É¬9T$uÀã\$ÖÌÑÔ   ∑EÉ;âC∑uâsÑà   ã|$1ˆãâãCâBãCâBãCâBãCâBãCâBãC âBãC$âBãC(âB ãC,âB$ãC0âB(ãC4âB,Éƒâ[^_]√ç¥&    çv «C	   È‚˛ˇˇçt& æ‡  ∏Ä  ∫†  Èˇˇˇçt& ãSÖ“t	É˙	ÖÁ  ã|$ãøî   â|$ã\ãó‘   â|$ˆ∆∞u*Ñ“ÑÆ  «D$@  ∫   π@  Î çt& æÍˇˇˇÈjˇˇˇ«D$†  ∫Ä  π‡  â˜â\$)œâ˘˜ŸHœâ«)◊â˙˜⁄H◊ã|$ç,çOÉ«Hâ<$Î-∑Qâ√â˜)”â⁄˜⁄H”∑Y)ﬂâ˚˜€Hﬂ⁄9ÍsâL$â’É¡9$uŒã|$ãD$ã\$âá–   Ö¿Ñ¶  ã|$∫   â¯Ë¸ˇˇˇÖ¿xÉ‡∫   Äâ¡â¯Ë¸ˇˇˇπ   ∫à  ∏Ë  Ë¸ˇˇˇãD$ãl$∫   ãÄƒ   ã8âËË¸ˇˇˇâ∆Ö¿àS  1¿Éˇ∫   î¿ÅÊø   ¡‡âÒ	¡âËË¸ˇˇˇâ∆Ö¿à(  ãD$∫   ãÄƒ   ãxçGˇÉ¯∏    G¯âËË¸ˇˇˇâ∆Ö¿à˜   ÉÊ¯∫   âËâÒ	˘∂…Ë¸ˇˇˇâ∆Ö¿à◊   ãD$∫   ã∏–   ∑O
∂G	¡˘É‡É·	¡âË∂…Ë¸ˇˇˇâ∆Ö¿à°   ∂O∫   âËË¸ˇˇˇâ∆Ö¿àá   ∂O
∫   âËË¸ˇˇˇâ∆Ö¿xqπL   ∫k   âËã}\Ë¸ˇˇˇâ∆Ö¿xWπ`   ∫l   âËË¸ˇˇˇâ∆Ö¿x@Éø‹   Ñô   ãD$ãÄ–   ∑P∑@ÖˆÖE˝ˇˇâSâCÈ:˝ˇˇæÍˇˇˇç¥&    fêã|$∫   â¯Ë¸ˇˇˇÖ¿xÉ‡∫   Äâ¡â¯Ë¸ˇˇˇ∏Ë  π   ∫à  Ë¸ˇˇˇãD$«Ä–       Èﬁ¸ˇˇ«á–       æÍˇˇˇÎ°È˝ˇˇã|$∫N   â¯Ë¸ˇˇˇâ∆Ö¿xÑÅÊà   ∫N   â¯âÒË¸ˇˇˇâ∆Ö¿â3ˇˇˇÈaˇˇˇ                     ê                     ˘                                                                                                       0   †   –            ê      ‡  ‡      rstb Unable to get GPIO "rstb" xti Unable to get xti clock
 pdn Unable to get GPIO "pdn" bus width error
 Product ID error %x:%x
 tw9910 Product ID %0x:%0x
 un-supported revision
 norm select error
 Field type %d invalid
 drivers/media/i2c/tw9910.c tw9910 PAL SQ PAL CCIR601 PAL SQ (CIF) PAL CCIR601 (CIF) PAL SQ (QCIF) PAL CCIR601 (QCIF) NTSC SQ NTSC CCIR601 NTSC SQ (CIF) NTSC CCIR601 (CIF) NTSC SQ (QCIF) NTSC CCIR601 (QCIF) h   WË¸ˇˇˇã≥¿   âË¸ˇˇˇâË¸ˇˇˇãÉ»   ZYÖ¿t∫   Ë¸ˇˇˇπ   ∫Ë  ∏Ù  Ë¸ˇˇˇã≥Ã   ÈD  h    WË¸ˇˇˇXZ∫˚ˇˇˇÈ“  h    WË¸ˇˇˇ∫ÍˇˇˇY[È“  ®Ñ∞   VRhj   WË¸ˇˇˇãÖî   ãp\ãæ¿   â¯Ë¸ˇˇˇâ¯Ë¸ˇˇˇãÜ»   ÉƒÖ¿to∫   Ë¸ˇˇˇ∫Ë  π   ∏Ù  Ë¸ˇˇˇ∫ÌˇˇˇÈ¨  h#   WË¸ˇˇˇãì¿   _]È“  hY   WË¸ˇˇˇXZ∫ÌˇˇˇÈ¨  h@   WË¸ˇˇˇãì»   Y^È¡  ∫ÌˇˇˇÈ¨  VjhÇ   WË¸ˇˇˇãÖî   «Ö‘    ∞  «Öÿ       «Ö–   †  ãp\ãæ¿   â¯Ë¸ˇˇˇâ¯Ë¸ˇˇˇãÜ»   ÉƒÖ¿t∫   Ë¸ˇˇˇπ   ∫Ë  ∏Ù  Ë¸ˇˇˇâÿË¸ˇˇˇâ¬Ö¿Ñ“  È¨  É∆hù   VË¸ˇˇˇ∏Íˇˇˇ[^Èú  É∆h¥   VË¸ˇˇˇXÉ»ˇZÈú  É¡PæÍˇˇˇh«   QË¸ˇˇˇÉƒÈ    TW9910: missing platform data!
 I2C-Adapter doesn't support I2C_FUNC_SMBUS_BYTE_DATA
 O  ﬁ   	Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇlicense=GPL v2 author=Kuninori Morimoto description=V4L2 driver for TW9910 video decoder                    tw9910                                                                     †               `       †             ‡  0                                                                 –           ‡                                                                                                                         @    –@    Ä      h   2  ¿ ê   @  ¥ ê                           S  Ä‡  [  –‡  h  @   v  h   â  † x   ò  ¥ x    GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                            
       $        0   f     ,   †   &     B   –        S      .                                	 a      ¸     q       T    	 Ü      a                   ï   ê  W    ¢   @         ¥   T   p   	 ∆   †  H     Ÿ     ˜     Ë   @  H     ˙     Á       ‡  ı       ƒ  2    	 ,  ‡  Ø    ;  ˆ      	               O           f      Ä     x      
     è           ¶          º  (   1     ◊      0                   ·     0     ¯  †   P       `   @     &             1             ;             C             `             l             w             á             ö             ≠             ª             ≈             —             Í             ˜                                       -             6             Q             k             y             ç           ô             ®      
     ∑      0      tw9910.c tw9910_g_std tw9910_get_selection tw9910_enum_mbus_code tw9910_g_tvnorms tw9910_remove tw9910_power_on tw9910_power_on.cold tw9910_s_power tw9910_probe tw9910_subdev_ops tw9910_probe.cold tw9910_ntsc_scales tw9910_get_fmt tw9910_pal_scales tw9910_s_std tw9910_s_stream tw9910_s_stream.cold tw9910_set_fmt tw9910_set_fmt.cold tw9910_i2c_driver_init tw9910_i2c_driver tw9910_i2c_driver_exit __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 tw9910_id tw9910_subdev_core_ops tw9910_subdev_video_ops tw9910_subdev_pad_ops __fentry__ gpiod_put clk_put v4l2_async_unregister_subdev clk_prepare clk_enable gpiod_set_value usleep_range_state gpiod_get_optional clk_unprepare _dev_info clk_disable __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init clk_get i2c_smbus_read_byte_data _dev_err v4l2_async_register_subdev i2c_smbus_write_byte_data __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw9910_id_device_table       &  1   &  °   &  —   &  Ò   &    '    (    )  !  &  >  *  S  +  n  ,  Ç  -  è  	  ñ  .  ∑  ,  À  -  ‹  ,    -  ˚  '    /  !  &  I  1  P  /  d  ,  x  -  ë  &  ∂  2  ◊  3  Ï  "  Û  4  ¯  	  ˇ  5  #  	  *  .  o  6  æ  '  œ  (  Ò  &  i  "  ·  "  Ò  &  &  6  K  9  f  9  x  9  ô  6  Ω  9  ·  &    6  3  9  S  6  l  9  |  6  ·  &  W  "  K  "  î  "  ∂  "  H	  6  _	  9  s	  -  è	  6  ∫	  9  Î	  6  
  9  A
  9  [
  9  u
  9  è
  9  ¶
  9  ¸
  6    9  '  -  b  6  |  9  ß  
  ´  
  ∆  
    
  ;  
  R  
  ã  
  ê  
  ê  9  õ  9  ≠  
     
    
               	  h   "                                                 $     (     ,     0        	     0     1     /  1   ,  E   -  U     [   7  l     r   7  ç   	  ì   7  ©   1  ∞   /  «   ,  €   -  Í   	     7    	    7    	    0  >  	  D  0  h  "  x  1    /  ñ  ,  ™  -  ±  8  »  	  Œ  7  ‚  	  Ë  7     	    7  P     g     ~     Â     ˝         ,    6    ª    ¿    ⁄    Ú                 	     &          :     ;          =  @   "  L   "  \   "  d     p     t     x     ®     ¨     º     »          @  	  L  	  X  	  d  	  p  	  |  	  †  	  ¨  	  ∏  	  ƒ  	  –  	  ‹  	   .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel__bug_table .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   è                    	   @       @  »              )             ‡  Ä                   %   	   @                         /             `                     8             `  4                  4   	   @       (  h               E      2       î  ¨                X             @                   T   	   @       ê  à     	         g      2       T  V                 z             ™                    v   	   @                         ä             ∂                    Ü   	   @       (                   ô                
                  ï   	   @       H                  §             ‘  Y                  ±             @  Ë                  ≠   	   @       X   ¿               π      0       (                   ¬              ;                     “             <  (                                d        &         	              d  Ÿ                               !  Â                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                       "      4     (               Ë¸ˇˇˇSãÄî   ã@\ãòÿ   ãà‘   1¿âZ[â
√ç¥&    çt& êË¸ˇˇˇãÄî   É9ã@\uMÉywG«A    «A    ˜Ä‘    ∞  t«AÄ  1¿«A‡  √çt& «A   1¿«A@  √ç¥&    ∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇãÖ“uãAÖ¿u«A   1¿√çt& ∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇ«ˇ∞  1¿«B    √ç¥&    çt& Ë¸ˇˇˇSãX\ãÉ»   Ö¿tË¸ˇˇˇãÉ¿   Ë¸ˇˇˇâÿË¸ˇˇˇ1¿[√fêË¸ˇˇˇUWVSã®¿   â√ã∏î   ÖÌt&âËË¸ˇˇˇâ∆Ö¿t[â^_]√êâËË¸ˇˇˇâ∆Ö¿ÖØ   ãÉ»   Ö¿t1“Ë¸ˇˇˇπ   ∫Ë  ∏Ù  Ë¸ˇˇˇÉ«π   ∫    â¯Ë¸ˇˇˇâÉÃ   = ˇˇá¸ˇˇˇ1ˆÖ¿tó∫   Ë¸ˇˇˇ∏Ù  π   ∫Ë  Ë¸ˇˇˇãÉÃ   Ö¿t!1“Ë¸ˇˇˇ∏Ù  π   ∫Ë  Ë¸ˇˇˇãÉÃ   Ë¸ˇˇˇ1ˆ[â^_]√ç¥&    êâËË¸ˇˇˇÈ,ˇˇˇçt& Ë¸ˇˇˇVSãÄî   ãX\Ö“tâÿ[^È„˛ˇˇçv ã≥¿   âË¸ˇˇˇâË¸ˇˇˇãÉ»   Ö¿t∫   Ë¸ˇˇˇπ   ∫Ë  ∏Ù  Ë¸ˇˇˇ1¿[^√ç¥&    ç¥&    êË¸ˇˇˇUWVâ∆Sç~ÉÏãnXã@ÖÌÑg   ãPãRË¸ˇˇˇ%   =   ÖP   π¿  ∫‡   â¯Ë¸ˇˇˇâ√Ö¿Ñ˚   â®ƒ   π@   âÚË¸ˇˇˇ∫   â¯Ë¸ˇˇˇâÉ¿   É¯˛ÑÜ   = ˇˇáÂ   π   ∫<   â¯Ë¸ˇˇˇâÉ»   = ˇˇá  ãn\ãÖƒ   ã ÉËÉ‡˜Ö˝   ãÖî   ã@\Ëº˝ˇˇâ¬Ö¿xF1“âË¸ˇˇˇâ¬â∆¡˙ÉÊÉ‚âµ‹   É˙ÖÜ   È~   çt& «É¿       Èvˇˇˇç¥&    fêãÉ»   Ö¿tâ$Ë¸ˇˇˇã$ãÉ¿   â$Ë¸ˇˇˇã$Éƒâ–[^_]√∫ÙˇˇˇÎÔç¥&    fêË¸ˇˇˇUWVSÉÏãêî   ãiãr\ÖÌÖ¿   ãæ–   â»Öˇt4∑WâPãñ–   ∑R«@   «@   âP«@	   1¿Éƒ[^_]√êãñ‘   ˆ∆∞ÖÅ   Ñ“tmΩ‡   ø@  ç_HâD$çOâ4$âÿÎ3fê∑QªÄ  æ‡  )”â⁄˜⁄H”∑Y)ﬁâÛ˜€Hﬁ⁄9Ísâ’âœÉ¡9»uÀã4$ãD$âæ–   ÖˇÖRˇˇˇç∂    ∏ÍˇˇˇÈmˇˇˇç∂    ø†  ÎÜç¥&    fêË¸ˇˇˇWVSãòî   ãC\˜¬ˇ∞  Ñ√   âê‘   ÄÊ˘∫U   âàÿ   âÿusË¸ˇˇˇÖ¿àì   %Ô   ∫U   ø   æ   â¡âÿË¸ˇˇˇÖ¿urâÒ∫   âÿ¡ÈÉ·É…Ë¸ˇˇˇÖ¿uWâ˘∫   âÿË¸ˇˇˇÖ¿uEâ∫	   ∂»âÿ[^_È¸ˇˇˇçt& Ë¸ˇˇˇÖ¿x$%Ô   ∫U   ø   æ   É»â¡âÿË¸ˇˇˇÖ¿té[^_√ç¥&    ∏ÍˇˇˇÎÓç¥&    fêË¸ˇˇˇWVSã∞î   â”ãF\Ö“Ö≈   ãÄ‹   ø   Ö¿Öö   ∫   âË¸ˇˇˇÖ¿à~   %¯   ∫   	¯â¡âË¸ˇˇˇÖ¿xeÉ˚∫   âˇÉÁÉ˚€É„@Ë¸ˇˇˇÖ¿xE%Ò   ∫   	¯â¡âË¸ˇˇˇÖ¿x,∫   âË¸ˇˇˇÖ¿x∂€%ø   ∫   	ÿ[â¡â^_È¸ˇˇˇê[^_√çt& É¯Ö¿  ø   ÈSˇˇˇçt& êãà–   Ö…Ñ⁄  1ˇÈ9ˇˇˇç¥&    çt& Ë¸ˇˇˇUWVSâÀÉÏãàî   âD$ãCâT$ãQ\Ö¿Öû  ãCÖ¿Ñ  É¯	ÖÚ  ãCãK«C   «C   â$ãÇ‘   ˆƒ∞Ö¸   Ñ¿Ñ\  æ@  ∏   ∫@  çzHâ\$1ÌâÀâ|$øˇˇˇˇÎç¥&    ∑B∑rã$)¡â»˜ÿH¡∑Œâﬁ)ŒâÒ˜ŸHŒ»9¯sâ’â«É¬9T$uÀã\$ÖÌÑÔ   ∑EÉ;âC∑uâsÑà   ã|$1ˆãâãCâBãCâBãCâBãCâBãCâBãC âBãC$âBãC(âB ãC,âB$ãC0âB(ãC4âB,Éƒâ[^_]√ç¥&    çv «C	   È‚˛ˇˇçt& æ‡  ∏Ä  ∫†  Èˇˇˇçt& ãSÖ“t	É˙	ÖÁ  ã|$ãøî   â|$ã\ãó‘   â|$ˆ∆∞u*Ñ“ÑÆ  «D$@  ∫   π@  Î çt& æÍˇˇˇÈjˇˇˇ«D$†  ∫Ä  π‡  â˜â\$)œâ˘˜ŸHœâ«)◊â˙˜⁄H◊ã|$ç,çOÉ«Hâ<$Î-∑Qâ√â˜)”â⁄˜⁄H”∑Y)ﬂâ˚˜€Hﬂ⁄9ÍsâL$â’É¡9$uŒã|$ãD$ã\$âá–   Ö¿Ñ¶  ã|$∫   â¯Ë¸ˇˇˇÖ¿xÉ‡∫   Äâ¡â¯Ë¸ˇˇˇπ   ∫à  ∏Ë  Ë¸ˇˇˇãD$ãl$∫   ãÄƒ   ã8âËË¸ˇˇˇâ∆Ö¿àS  1¿Éˇ∫   î¿ÅÊø   ¡‡âÒ	¡âËË¸ˇˇˇâ∆Ö¿à(  ãD$∫   ãÄƒ   ãxçGˇÉ¯∏    G¯âËË¸ˇˇˇâ∆Ö¿à˜   ÉÊ¯∫   âËâÒ	˘∂…Ë¸ˇˇˇâ∆Ö¿à◊   ãD$∫   ã∏–   ∑O
∂G	¡˘É‡É·	¡âË∂…Ë¸ˇˇˇâ∆Ö¿à°   ∂O∫   âËË¸ˇˇˇâ∆Ö¿àá   ∂O
∫   âËË¸ˇˇˇâ∆Ö¿xqπL   ∫k   âËã}\Ë¸ˇˇˇâ∆Ö¿xWπ`   ∫l   âËË¸ˇˇˇâ∆Ö¿x@Éø‹   Ñô   ãD$ãÄ–   ∑P∑@ÖˆÖE˝ˇˇâSâCÈ:˝ˇˇæÍˇˇˇç¥&    fêã|$∫   â¯Ë¸ˇˇˇÖ¿xÉ‡∫   Äâ¡â¯Ë¸ˇˇˇ∏Ë  π   ∫à  Ë¸ˇˇˇãD$«Ä–       Èﬁ¸ˇˇ«á–       æÍˇˇˇÎ°È˝ˇˇã|$∫N   â¯Ë¸ˇˇˇâ∆Ö¿xÑÅÊà   ∫N   â¯âÒË¸ˇˇˇâ∆Ö¿â3ˇˇˇÈaˇˇˇ                     ê                     ˘                                                                                                       0   †   –            ê      ‡  ‡      rstb Unable to get GPIO "rstb" xti Unable to get xti clock
 pdn Unable to get GPIO "pdn" bus width error
 Product ID error %x:%x
 tw9910 Product ID %0x:%0x
 un-supported revision
 norm select error
 Field type %d invalid
 drivers/media/i2c/tw9910.c tw9910 PAL SQ PAL CCIR601 PAL SQ (CIF) PAL CCIR601 (CIF) PAL SQ (QCIF) PAL CCIR601 (QCIF) NTSC SQ NTSC CCIR601 NTSC SQ (CIF) NTSC CCIR601 (CIF) NTSC SQ (QCIF) NTSC CCIR601 (QCIF) h   WË¸ˇˇˇã≥¿   âË¸ˇˇˇâË¸ˇˇˇãÉ»   ZYÖ¿t∫   Ë¸ˇˇˇπ   ∫Ë  ∏Ù  Ë¸ˇˇˇã≥Ã   ÈD  h    WË¸ˇˇˇXZ∫˚ˇˇˇÈ“  h    WË¸ˇˇˇ∫ÍˇˇˇY[È“  ®Ñ∞   VRhj   WË¸ˇˇˇãÖî   ãp\ãæ¿   â¯Ë¸ˇˇˇâ¯Ë¸ˇˇˇãÜ»   ÉƒÖ¿to∫   Ë¸ˇˇˇ∫Ë  π   ∏Ù  Ë¸ˇˇˇ∫ÌˇˇˇÈ¨  h#   WË¸ˇˇˇãì¿   _]È“  hY   WË¸ˇˇˇXZ∫ÌˇˇˇÈ¨  h@   WË¸ˇˇˇãì»   Y^È¡  ∫ÌˇˇˇÈ¨  VjhÇ   WË¸ˇˇˇãÖî   «Ö‘    ∞  «Öÿ       «Ö–   †  ãp\ãæ¿   â¯Ë¸ˇˇˇâ¯Ë¸ˇˇˇãÜ»   ÉƒÖ¿t∫   Ë¸ˇˇˇπ   ∫Ë  ∏Ù  Ë¸ˇˇˇâÿË¸ˇˇˇâ¬Ö¿Ñ“  È¨  É∆hù   VË¸ˇˇˇ∏Íˇˇˇ[^Èú  É∆h¥   VË¸ˇˇˇXÉ»ˇZÈú  É¡PæÍˇˇˇh«   QË¸ˇˇˇÉƒÈ    TW9910: missing platform data!
 I2C-Adapter doesn't support I2C_FUNC_SMBUS_BYTE_DATA
 O  ﬁ   	Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇlicense=GPL v2 author=Kuninori Morimoto description=V4L2 driver for TW9910 video decoder                    tw9910                                                                     †               `       †             ‡  0                                                                 –           ‡                                                                                                                         @    –@    Ä      h   2  ¿ ê   @  ¥ ê                           S  Ä‡  [  –‡  h  @   v  h   â  † x   ò  ¥ x    GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                            
       $        0   f     ,   †   &     B   –        S      .                                	 a      ¸     q       T    	 Ü      a                   ï   ê  W    ¢   @         ¥   T   p   	 ∆   †  H     Ÿ     ˜     Ë   @  H     ˙     Á       ‡  ı       ƒ  2    	 ,  ‡  Ø    ;  ˆ      	               O           f      Ä     x      
     è           ¶          º  (   1     ◊      0                   ·     0     ¯  †   P       `   @     &             1             ;             C             `             l             w             á             ö             ≠             ª             ≈             —             Í             ˜                                       -             6             Q             k             y             ç           ô             ®      
     ∑      0      tw9910.c tw9910_g_std tw9910_get_selection tw9910_enum_mbus_code tw9910_g_tvnorms tw9910_remove tw9910_power_on tw9910_power_on.cold tw9910_s_power tw9910_probe tw9910_subdev_ops tw9910_probe.cold tw9910_ntsc_scales tw9910_get_fmt tw9910_pal_scales tw9910_s_std tw9910_s_stream tw9910_s_stream.cold tw9910_set_fmt tw9910_set_fmt.cold tw9910_i2c_driver_init tw9910_i2c_driver tw9910_i2c_driver_exit __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 tw9910_id tw9910_subdev_core_ops tw9910_subdev_video_ops tw9910_subdev_pad_ops __fentry__ gpiod_put clk_put v4l2_async_unregister_subdev clk_prepare clk_enable gpiod_set_value usleep_range_state gpiod_get_optional clk_unprepare _dev_info clk_disable __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init clk_get i2c_smbus_read_byte_data _dev_err v4l2_async_register_subdev i2c_smbus_write_byte_data __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw9910_id_device_table       &  1   &  °   &  —   &  Ò   &    '    (    )  !  &  >  *  S  +  n  ,  Ç  -  è  	  ñ  .  ∑  ,  À  -  ‹  ,    -  ˚  '    /  !  &  I  1  P  /  d  ,  x  -  ë  &  ∂  2  ◊  3  Ï  "  Û  4  ¯  	  ˇ  5  #  	  *  .  o  6  æ  '  œ  (  Ò  &  i  "  ·  "  Ò  &  &  6  K  9  f  9  x  9  ô  6  Ω  9  ·  &    6  3  9  S  6  l  9  |  6  ·  &  W  "  K  "  î  "  ∂  "  H	  6  _	  9  s	  -  è	  6  ∫	  9  Î	  6  
  9  A
  9  [
  9  u
  9  è
  9  ¶
  9  ¸
  6    9  '  -  b  6  |  9  ß  
  ´  
  ∆  
    
  ;  
  R  
  ã  
  ê  
  ê  9  õ  9  ≠  
     
    
               	  h   "                                                 $     (     ,     0        	     0     1     /  1   ,  E   -  U     [   7  l     r   7  ç   	  ì   7  ©   1  ∞   /  «   ,  €   -  Í   	     7    	    7    	    0  >  	  D  0  h  "  x  1    /  ñ  ,  ™  -  ±  8  »  	  Œ  7  ‚  	  Ë  7     	    7  P     g     ~     Â     ˝         ,    6    ª    ¿    linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
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
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  include/linux/instruction_pointer.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/linux/sched.h \
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
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/IOMMU_SVA) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcarcmd_drivers/media/i2c/tw9910.o := gcc -Wp,-MMD,drivers/media/i2c/.tw9910.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"tw9910"' -DKBUILD_MODNAME='"tw9910"' -D__KBUILD_MODNAME=kmod_tw9910 -c -o drivers/media/i2c/tw9910.o drivers/media/i2c/tw9910.c 

source_drivers/media/i2c/tw9910.o := drivers/media/i2c/tw9910.c

deps_drivers/media/i2c/tw9910.o := \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
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
  include/linux/clk.h \
    $(wildcard include/config/COMMON_CLK) \
    $(wildcard include/config/HAVE_CLK_PREPARE) \
    $(wildcard include/config/HAVE_CLK) \
    $(wildcard include/config/OF) \
  include/linux/err.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
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
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
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
  include/asm-generic/bitops/