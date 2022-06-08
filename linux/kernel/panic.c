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
  include/linux/of_graph.h \
  include/uapi/linux/v4l2-dv-timings.h \
  include/media/i2c/tvp7002.h \
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
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/v4l2-common./x86/include/uapi/asm/bitsperlong.h \
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
  include/ELF                      0      4     (               θόÿÿÿS‹X\‰Ψθόÿÿÿƒΐ   θόÿÿÿ1ΐ[ΓθόÿÿÿS‰Γ‹@‹HΤ‹C(=	 tHw&=	 tW=	 uΊ>   ‰Θθόÿÿÿ‰C|1ΐ[Γt& =$	 uπΊ=   ‰Θθόÿÿÿ‰C|1ΐ[Γ¶    Ί<   ‰Θθόÿÿÿ‰C|1ΐ[Γt& Ί?   ‰Θθόÿÿÿ‰C|1ΐ[Γt& θόÿÿÿWVS‰Γ‹p‹@(‹~Τ=	 „R  wp=	 „U  v3=	 „`  =	 …   ¶K|Ί?   [‰ψ^_ιόÿÿÿ΄&    v = 	 ty=	 ub¶Vp¶K|ΑβƒΚ[‰ψ^¶Ò_ιόÿÿÿ΄&    =	 tYv=$	 u0¶K|Ί=   λ΅t& =	 ty=	 u¶K|Ί<   λƒ΄&    Έκÿÿÿ[^_Γ΄&    ¶Vp¶K|ΑβƒΚλ¶Vp‰ψΑβƒΚ¶Òθόÿÿÿ…ΐxΝƒ{|Ί   ƒΪÿ%ό   	Π¶Vp‰ΑΑβƒΚιPÿÿÿt& ¶Vp‰ψΑβƒΚ¶Òθόÿÿÿ…ΐx‹S|‰Α€Ι€…Òu$‰Α¶Vp¶ΙΑβƒΚιÿÿÿ¶    ¶K|Ί>   ιΞώÿÿf¶Vp¶K|ΑβƒΚιιώÿÿt& ¶Vp¶K|ΑβƒΚιΡώÿÿt& θόÿÿÿW‰ΗVS‹@‹P‹Rθόÿÿÿ%   =   …0  GΉΐ  Ί8  θόÿÿÿ‰Ζ…ΐ„  Ή@   ‰ϊΐ   θόÿÿÿ€0  1Ι‰ΨΗ†4   °  Ί
   j θόÿÿÿΉ 	 Ίπ   ‰Ψj h€   j jj hÿ   j j θόÿÿÿΉ	 Ίπ   ‰ΨƒΔ$j h€   j jj hÿ   j j θόÿÿÿΉ	 Ίπ   ‰ΨƒΔ j h€   j jj hÿ   j j θόÿÿÿΉ	 Ίπ   ‰ΨƒΔ j h€   j jj hÿ   j j θόÿÿÿΉ	 Ίπ   ‰ΨƒΔ j j j jj jj j θόÿÿÿΉ	 Ίπ   ‰ΨƒΔ j j j jj jj j θόÿÿÿΉ	 Ίπ   ‰ΨƒΔ j h€   j jj hÿ   j j θόÿÿÿƒΔ …ΐtHt€   j Ή$	 Ίπ   ‰Ψh€   j jj hÿ   j j θόÿÿÿƒΔ …ΐtHt€   j Ή	 Ίπ   ‰Ψjzj jj hÿ   j j θόÿÿÿƒΔ …ΐtHt€   j Ή	 Ίπ   ‰Ψjzj jj hÿ   j j θόÿÿÿƒΔ …ΐtHt€   ‰^l‹¶ό   …φ„d   ‰Ψθόÿÿÿ‰π[^_Γt& ΎνÿÿÿλξΎτÿÿÿλη΄&    v θόÿÿÿUWV‰ΖSƒμ(‰T$d΅    ‰D$$‹†”   ΖD$ΖD$	ΖD$
ΖD$ΖD$ΖD$‰$‰Π% ω  ΖD$ƒψΖD$νΖD$ ƒεΐΖD$ !ƒν<ƒψΙƒαύƒΑƒψÒL$ƒβ0L$ƒκƒψÿT$ƒη
T$ƒΗ@ƒψΫƒγ@ƒψΐ\$‰λ¶L$ƒΰ\$‰ϋ|$ƒΐ\$D$D$!ΈÿÿÿÿL$L$f‰D$"‹$Έ   ¶®0  Αελt& ¶GƒΗ<ÿt	θ¶¶Π‰Ψθόÿÿÿ…ΐyβ‹D$‰†4  ‹D$$d+    u
ƒΔ(1ΐ[^_]Γθόÿÿÿt& θόÿÿÿUW‰ΗV‰ΦSƒμ‹―”   ‹D$…ΐtz¶—0  Xÿ‰Ρƒα9Λtgƒψ‡®   ƒθƒβόƒΰ	Β—0  ƒβ…Φ   1ΫΊ9   λv ¶“r  ƒΓ€ϊÿ„­   ¶‹q  ‰θθόÿÿÿ…ΐyΫι”   ¶    ‹4$t& ƒώ‡Ν   ¶—0  1ΐ‰Ρΐιƒα9ρt[Αβ‰θƒΚ"¶Òθόÿÿÿ…ΐxG‰ΒƒΘƒβϋ…φ‰Ρ¶—0  EΘ‰θΑβƒΚ"¶Ι¶Òθόÿÿÿ…ΐx¶‡0  ƒζΑζƒΰϋ	π‡0  1ΐƒΔ[^_]Γ΄&    ¶‡0  ƒΰ‰Α‰4$1ΫΈ   Αα‰Ξλv ¶ƒ  ƒΓ<ÿ„>ÿÿÿ	π¶‹  ¶Π‰θθόÿÿÿ…ΐyΧιΚ   Έκÿÿÿλ                               `                      D                                                                                                           °       `  Π        60 Hz 50 Hz 6%s: Standard: %s
 6%s: Channel: %d
 6%s: Input: %d
 tw2804 θόÿÿÿVΊ   S‰Γχ€4   ω  Έ    DΒspPVh   θόÿÿÿƒ0  ƒΰPVh    θόÿÿÿƒ0  ΐθƒΰPVh3   θόÿÿÿ‰ΨƒΔ$[^ιόÿÿÿ‹W·G  Q QP‹GTÿ²   ÿ0h    θόÿÿÿƒΔι©  ƒΕht   UθόÿÿÿXΈϋÿÿÿZι  ƒΕPh(   UθόÿÿÿΈκÿÿÿƒΔι  ¶‡0  ƒΕƒΰPhL   UθόÿÿÿΈϋÿÿÿƒΔι   6%s %d-%04x: chip found @ 0x%02x (%s)
 channel %d is not between 1 and 4!
 error initializing TW2804 channel %d
   error initializing TW2804 global registers
 θόÿÿÿΊ    Έ    ιόÿÿÿΈ    ιόÿÿÿauthor=Micronas USA Inc description=TW2804/TW2802 V4L2 i2c driver license=GPL v2                  tw2804                                                          `                                                                                                         Π                                                                          °       Δ¥ Π Π 	
ππ@Ò€€€€    ÿÿÿÿÿÿÿÿ !" #‘$Q%& ' ( ) * + , - . / 0 1 2 3 4 5 6 7 ÿÿ9 :ÿ;„<€=€>‚?‚x ÿÿ GCC: (GNU) 11.2.0             GNU  ΐ       ΐ                                  ρÿ                            
                   ‹     /   °   «                               	 =       h    	               O   `  f    \   @         g   π        w   h   0    	 ‰   Π  <    –     u    ­   p       Ύ      p     Π      \    	               μ            ÿ       €           
                 6     *     Q  B        h      0                   r  `   0     ‚      P     “                          Ό             Σ             μ                                       *             C             P             e             ‚             ”             ¦             ·             ΐ             Ξ             β           ξ             ύ      
           0      tw2804.c tw2804_remove tw2804_g_volatile_ctrl tw2804_s_ctrl tw2804_log_status tw2804_probe tw2804_ops tw2804_ctrl_ops tw2804_probe.cold tw2804_s_std tw2804_s_video_routing global_registers channel_registers tw2804_s_video_routing.cold tw2804_driver_init tw2804_driver tw2804_driver_exit __UNIQUE_ID_author268 __UNIQUE_ID_description267 __UNIQUE_ID_license266 tw2804_id tw2804_core_ops tw2804_video_ops __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free i2c_smbus_read_byte_data i2c_smbus_write_byte_data _printk v4l2_ctrl_subdev_log_status __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std __stack_chk_guard __stack_chk_fail _dev_err __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw2804_id_device_table                 !  !     P   "  o   "     "      "  ±     °  "  π  "  a    t  &  –  '  ¥    ²  (  Σ  )  έ    ϊ  *      $  *  .    N  *  X    x  *  ‚      *  ¦    ΐ  *  Κ    κ  *        *  9    Q  *  k    ƒ  *  ©  !  Ρ    δ  +  ή  #  χ  +    ,      s    †      #  Ò  "  ÿ  #  S    g    q  #    #  5  #  Ά    F    –    z                   h                                                             &     +   $  ;     @   $  S     X   $  ‡   
     $     
  Ά   -  ·   
  ½   -  έ   
  γ   -  d   %  ”     ®     Κ     π                  .     /          1  @     L     `           ¬     π     ψ      .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   …                    	   @         Π              )             ΰ  €                   %   	   @       ΰ                  /             `                     8             `                     4   	   @          @               E      2       €  K                 X             Λ  τ                  T   	   @       @  °      	         g      2       ΐ	                    z             `
                    v   	   @       π                  ‰             t
  
                  …   	   @                        ”             ~
  Q                  ΅             ΰ
  ‚                     	   @          8               ©      0       b                   ²              u                     Β             x  (                                   @              	              ΰ  .                               X  Υ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          linux/log2.h \
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
    $(wildcard incl