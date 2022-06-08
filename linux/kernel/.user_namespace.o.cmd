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
  inΊ    ‰ΠΓ΄&    Ί@  ‰ΠΓ΄&    Ίΐ   ‰ΠΓ΄&    Ί   ιIÿÿÿ¶    Ί    ι9ÿÿÿ¶    Ί    ι)ÿÿÿ¶    θόÿÿÿWVS…ÒxT‰Ζϊ   ‰Χ‰Σƒϊ ‰NΡ‰Σ‰N‰Ψ[^_Γ»   Ώ   Ίΐ ‰ψθόÿÿÿ‰F…ΐtψ‰F‰Ψ[^_Γ΄&    »κÿÿÿ‰Ψ[^_Γ»τÿÿÿλ·΄&    ΄&    θόÿÿÿUW‰ΟV‰ΦS‰ΓΈ    ƒμθόÿÿÿ‹C(‰$…ΐ„™  …ÿt	9{…‚  ‹C,ƒψ"‡Ν   Ύ€    ‰D$…ΐΊ   ‹<$„   Ε4  ‹n‰D$‰ϊ‰L$Β‹C9h…   ‹DΟ9Βuι!  f‹ 9Β„  ;Xuρ‰π‡C‹$‰D$1ΐθόÿÿÿ‰Ζ‹D$8  ‰D$…φuιΥ   ¶    ‹$‰πθόÿÿÿ‰Ζ…ΐ„»   †T  θόÿÿÿ„ΐuέ‰υ1Ι…Ιt5‹D$φD…u*‹mh…νuκ»κÿÿÿΈ    θόÿÿÿ‰ΨƒΔ[^_]Γ΄&    ‹D$| ‹D$‹DΕ9Ηuλΐ΄&    v 9Σt ƒΑ‹ 9Ηt©ƒx ‹Puκ…Òtνƒz tη9Σuΰ‹|$‹CIΑβ”ΎΨ  ‰B‹$‰πθόÿÿÿ‰Ζ…ΐ…Eÿÿÿ‹D$1Ϋθόÿÿÿιfÿÿÿ»ώÿÿÿι\ÿÿÿ»ÿÿÿÿιRÿÿÿ»½ÿÿÿιHÿÿÿv θόÿÿÿU‰ΥWVSƒμ‹€δ  d‹    ‰T$1ÒΗD$    ‹Ψ  θόÿÿÿθόÿÿÿ{d΅    ‹°p  ‰t$t$‰°p  ‹[…Ϋt:f‰|$>t& ‹K$S0‰θθόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΘ‹|$d΅    ‰Έp  θόÿÿÿθόÿÿÿ‹D$‹T$d+    unƒΔ[^_]Γt& θόÿÿÿS0‹s$‰D$‰θθόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ‹L$1Ò)ΘCSƒCχ$   „MÿÿÿϋιGÿÿÿθόÿÿÿ΄&    v θόÿÿÿU‰ΕWVSƒμd΅    ‰D$‹…δ  ΗD$    ‹Ψ  θόÿÿÿθόÿÿÿ{d΅    ‹°p  ‰t$t$‰°p  ‹[…Ϋt=t& ‰|$>t& ‹K$S0‰θθόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΘ‹|$d΅    ‰Έp  θόÿÿÿθόÿÿÿ‹D$‹T$d+    unƒΔ[^_]Γt& θόÿÿÿS0‹s$‰D$‰θθόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ‹L$1Ò)ΘCSƒCχ$   „MÿÿÿϋιGÿÿÿθόÿÿÿ΄&    v θόÿÿÿUWV1φS‰Γ‰Π‰Κμ¬   d‹    ‰$¨   1Ι‹$ΐ   ΗD$    ΗD$     ‹¬$Δ   ‰L$$·K‰\$ƒαχ‰D$fƒω…«   …ΐ„.  ‹ƒδ  ΗD$    ‹Ψ  θόÿÿÿθόÿÿÿ{d΅    ‹p  ‰T$T$‰p  ‹s…φtBv ‰|$>t& ‹N$V0D$θόÿÿÿ‰ΓφΓu|$ πÿÿwΗD$ÿÿÿÿƒΗΡλ	] ‹7…φuΑ‹|$d΅    ‰Έp  θόÿÿÿθόÿÿÿ‹t$‹„$¨   d+    …   Δ¬   ‰π[^_]ΓfθόÿÿÿV0‹^$‰D$D$θόÿÿÿ‹v‰Γθόÿÿÿ4…    $ϊƒFƒƒV θόÿÿÿ‹L$1Ò)ΘFVƒFχ$   „=ÿÿÿϋι7ÿÿÿt& t$(Ή    ‰χ‰t$σ«ιΌώÿÿθόÿÿÿ΄&    ¶    θόÿÿÿƒψQtfwƒψtOΊ    ƒψPu%‰ΠΓf=Ί   t)=»   uΊΰ  ‰ΠΓ΄&    v Ί    ƒψtΡιόÿÿÿΊ   λΔ΄&    fΊ    λ΄΄&    fΊ    λ¤΄&    fθόÿÿÿU1νWV²„   S‰Γ‰Π8  ‰ίƒμ‰t$4Υ4  ‰T$‰L$‰4$‰D$‰θ…νt‹t$φD·tD‹$1Ò4‹D$‹DΗ9Ζuλ(ƒΒ‹ 9Ζt‹H…Ιuπ‹H…Ιtμ‹I…Ιuβ‹ 9Ζuεt& D ‰Ε‹h…ÿu¤Ίΐ  θόÿÿÿ‰Ζ…ΐ„‡   ‹D$1Ò8  ‰D$…Ò~$‹D$φDƒu‹[h…Ϋuκ‹D$‰01ΐƒΔ[^_]Γf‹D$‹<$‹DΓ,;9Εu'λΣt& <RƒΒ‰LΎR‹x‰9‹x‰y‹ 9θt®‹H…ΙuΨ‹H…Ιtμ‹I…ΙtελΘΈτÿÿÿλ΄&    fθόÿÿÿUW‰Χ‰ΒV‰Ζ1ΐSθόÿÿÿ…ΐt=‰Γt& ƒT  θόÿÿÿ„ΐu‹P  ‰ϊ‰Ψθ‚ώÿÿ‰Ε…ΐ…   ‰Ψ‰ςθόÿÿÿ‰Γ…ΐuΙ‰ς1ΐΗτ   θόÿÿÿ‰Γ…ΐuλVf‹ƒP  …ΐu,‰Ψ‰ςθόÿÿÿ‰Γ…ΐt;ƒT  θόÿÿÿ„ΐuΨ‹“P  ‹D»‰T»θόÿÿÿ‰Ψ‰ςΗƒP      θόÿÿÿ‰Γ…ΐuΕ1ν[‰θ^_]Γv ‰ς1ΐθόÿÿÿ‰Γ…ΐtη‹ƒP  θόÿÿÿ‰Ψ‰ςΗƒP      θόÿÿÿ‰Γ…ΐuά[‰θ^_]Γt& θόÿÿÿUWV‰Ζ‰ΠSƒμ‹T$(‰$ƒϊ"‡    ‰ΛΎ    …Ι   Ή„   ‘8  ‰|$Αη‰|$|>‰T$‹T–…ΐt…Ϋudƒβuo‹T$½ώÿÿÿ‹DΦ9ΗtS‹\Φϋ πÿÿ‡ό   ‹CΗC    ‰ΚΗC    ‰D$‰πθPώÿÿ‰Ε…ΐtz‹D$‰C‹$‰CƒΔ‰θ[^_]Γv ½κÿÿÿƒΔ‰θ[^_]Γ‹$½κÿÿÿ‰Σ	Γtη‹\$‹\ή9ίuλt& ‹9ίt;Cuυ;S„rÿÿÿ‹9ίuμƒΔ½ώÿÿÿ[‰θ^_]Γ΄&    ‹C‹‰A‰‰ΨΗ   ΗC"  θόÿÿÿ‹D$‹DΖ9Ηt&‹D$…ΐtθόÿÿÿ‹D$ΰϋÿÿθόÿÿÿ‰θƒΔ[^_]Γ‹D$ΗD†    λΜ‰έι@ÿÿÿ΄&    t& θόÿÿÿVS‰ΓΈ    θόÿÿÿ‹C(…ΐtVÿs,1Ò‰ΩθJώÿÿZ…ΐuU‹s(ΗC(    Έ    θόÿÿÿφF,t[^Γ΄&    θόÿÿÿ‹F¨u,dÿ[^ιόÿÿÿ΄&    v [Έ    ^ιόÿÿÿt& λ§t& ‹Fπƒ(uΞ‹VF‹RθόÿÿÿλΎ΄&    ΄&    θόÿÿÿ‹P(…Òtι?ÿÿÿ΄&    Γ΄&    θόÿÿÿ‹P(…Òtθÿÿÿ1ΐΓ΄&    t& θόÿÿÿƒψk„Β   wƒψ9t[Ή    ƒψ`u!‰ΘΓ¶    ƒψlt3ƒψzuVΉ`   ‰ΘΓ¶    ƒψ1u[ƒzΉ    Έ    EΘλΗ΄&    Ή    λΈ΄&    fƒzΉ    Έ    EΘλt& =Ί   tQ=»   u2Ήΰ  λ‚v ƒψQt[wαƒψtDΉ    ƒψP„eÿÿÿΉ    ƒψ„Wÿÿÿιόÿÿÿv Ή    ιEÿÿÿ¶    Ή   ι5ÿÿÿ¶    Ή    ι%ÿÿÿ¶    Ή    ιÿÿÿ¶    θόÿÿÿU½    W‰ΗVSƒμ‰D$@μ‰D$Έ    θόÿÿÿ‰ψΏΨώÿÿ-„  ‰D$΄&    ‹‹39ϋu'ιƒ   v ΗB(    ‰Ψ‰σθόÿÿÿ‰θθόÿÿÿ‹9ώtc‰Ζ‹C‰F‰0‹CΗ   ΗC"  …ΐtθόÿÿÿ‹S…Òtΐ‹J(φA,u°‰L$‰$θόÿÿÿ‹L$‹$‹A¨…Β   dÿ‰$θόÿÿÿ‹$λ‚f‹t$ƒΕƒΗ‹θόÿÿÿ‰πƒΐ‰D$ύΈ   …Jÿÿÿ‹D$‹|$‹@μ‹Xδqδ9D$t‰Ψθόÿÿÿ‰Ψ‰σθόÿÿÿ‹FpδC9ψuγΈ    θόÿÿÿ‹D$‹ϋÿÿ…Ϋtθόÿÿÿ‹ƒT  ¨utdÿθόÿÿÿ‹[h…Ϋuβ‹|$Gψθόÿÿÿφ‡Πϊÿÿt.ƒΔ[^_]Γ‹Aπƒ(…4ÿÿÿA‹I‰$‹Iθόÿÿÿ‹$ιÿÿÿθόÿÿÿ‹D$‹€¬ϊÿÿ¨u4dÿƒΔ[^_]ιόÿÿÿ‹ƒX  πƒ(uƒ‹“X  ƒT  ‹Rθόÿÿÿιjÿÿÿ‹D$‹€°ϊÿÿπƒ(uΏ‹L$‹‘°ϊÿÿ¬ϊÿÿ‹Rθόÿÿÿλ¥΄&    fθόÿÿÿUWVSƒμH‹t$\‰$‰L$‰t$d΅    ‰D$D‹D$dΗD$4    ΗD$8    ΗD$<    ΗD$@    ƒΰ‰D$ƒψ„_  ‹D$dƒΰƒψ„O  ‰Χ…φt	Ρ…A  ‹D$d‹t$Αθƒΰ…φ•Β8Π…&ELF                             4     (               θόÿÿÿ‹@\θόÿÿÿ1ΐΓθόÿÿÿVΉ   S‰ΓƒΓpƒμd΅    ‰D$‹C$jT$θόÿÿÿ‹C$¶t$Ή   jT$
θόÿÿÿ‰π¶πV¶D$PSh    θόÿÿÿƒΔ‹D$d+    uƒΔ1ΐ[^Γθόÿÿÿ΄&    θόÿÿÿUW‰ΟV‰ΦS‰Γƒμ‹«”   d΅    ‰D$1ΐL$‹    T$…Ι   j Ή   ‰θT$θόÿÿÿZƒψ…όÿÿÿ‹D$d+    uƒΔ[^_]Γθόÿÿÿθόÿÿÿ‹    VS‰Γ¶°ΐ   …Ι>   ‰ρ‰Ψ1ÒƒΙ¶Ιθaÿÿÿ‰ρ‰Ψ1Òαο   θPÿÿÿ1ΐ[^Γ΄&    t& θόÿÿÿUWVS‰Γ‹@‹P‹Rθόÿÿÿ%   =   „S   Έϋÿÿÿ[^_]Γ΄&    ¶    θόÿÿÿU‰ΥΑεWƒεΐV4S‰ι‰Γƒζ ‹Ρ   ‰ρ‰ΠΑβ‹Ò   ¶‹ΐ   Αΰƒβ€	ΦƒΠ   ‰Ο“Σ   ƒα?1Òƒη_	Α‰Ψ	χ¶³Θ   ¶Ιθώÿÿ‰ψΊ   ƒζ?¶Θ‰Ψθ„ώÿÿ	ξΊ   ‰π¶Θ‰Ψθqώÿÿ‹    ¶³ΐ   …Ιΰ   ‰ρ‰Ψ1ÒƒΙ¶ΙθKώÿÿ‰ρ‰Ψ1Òαο   θ:ώÿÿ1ΐ[^_]Γ       P                                                                                                                                       P        6%s: Status: SA00=0x%02x SA01=0x%02x
  7%s: write reg: %02X val: %02X
    3%s: I/O error write 0x%02x/0x%02x
    7%s: changed input or channel
 6%s %d-%04x: chip found @ 0x%x (%s)
 ‰ψƒΓp¶ψ‰π¶πWVShL   θόÿÿÿƒΔιβ   ‰ψ¶ΐP¶ΒPCpPh(   θόÿÿÿƒΔιΖ   @pPht   θόÿÿÿXZι  ‹S·C  Q QPÿ²   ‹CTÿ0h”   θόÿÿÿCΉΐ  ΊΤ   θόÿÿÿ‰ΕƒΔ…ΐu
Έτÿÿÿιz  Ή`   ‰Ϊ½ΐ   1ΫθόÿÿÿΎ    Ή   σ¥¶ΐ   ‰Ϊ‰θƒΓθ   ƒϋuη1ΐιz  CpPht   θόÿÿÿXZι0                    ΈHÒζ―  ^ Π €              upd64031a                                                          ΰ       €                                                                                                                                                                                                                               debug θόÿÿÿΊ    Έ    ιόÿÿÿΈ    ιόÿÿÿupd64031a parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=T. Adachi, Takeru KOMORIYA, Hans Verkuil description=uPD64031A driver  P          ¤ÿ      GCC: (GNU) 11.2.0           GNU  ΐ        ΐ                                  ρÿ                                                                          x                  	 3      p     ¤           C       B    	 X      E     n   B       	               ‰   P  3     ™   W       	 ®   `         Ό     Ν     Π   δ       	               ι            ÿ       €           
                   &      0     3     0     F  ΰ   ,     Z  €   P     n           ƒ                     ª  P       Ό  0        Σ  <   0     ι  l                                  -             ?             Y             a             r             ‹                          ­             »             Ο           Ϋ             κ      
     ω      0                   upd64031a.c upd64031a_remove upd64031a_log_status upd64031a_write upd64031a_write.cold upd64031a_s_frequency upd64031a_s_frequency.cold upd64031a_probe upd64031a_probe.cold upd64031a_ops upd64031a_s_routing upd64031a_s_routing.cold upd64031a_driver_init upd64031a_driver upd64031a_driver_exit upd64031a_id upd64031a_core_ops upd64031a_tuner_ops upd64031a_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev __stack_chk_guard i2c_transfer_buffer_flags _printk __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__upd64031a_id_device_table param_ops_int    $  	   %     $  &   &  8   '  P   '  b     g   (  u   &  „   )  ‘   $  ª   &  Ί     Ψ   '  ν   &  ό   )    $      Q  $  e  *  ‘  $  !    Ζ     β         u    0                   h                                                 (  1     6   (  G     L   (  v     {   (     +  ¥     ΄   ,  Ή     Τ     ι     ξ   (       >     S           ΰ     υ     `     d     l     €     θ             $          -     .          0           -     3        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rel.text.unlikely .rel.rodata .rel.init.text .rel.exit.text .rodata.str1.1 .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   ]                    	   @       Δ  Ψ               )                €                   %   	   @                         /                                  8                                  4   	   @       Ό  8               E      2       <  Ί                 X             φ  ω                  T   	   @       τ  ¨      	         k                V                  g   	   @         0               w             V                    s   	   @       Μ                  †             j  
                  ‚   	   @       μ                 ‘      2       t  
                               ~  ‰                  ­                                 ©   	   @       ό                  µ      0                          Ύ              /                     Ξ             0  (                                X  @     $         	              
  ,                                 α                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ƒ|$`"‡  ‹D$`Ύ€    ‰D$…ΐ  „   έ    ‰\$q‰L$$‰σ‰t$‹4$‰υ‹vhέ…φtz8  ‰l$ ‰D$‹D$‹l†‰κƒβuZ‹D$‹D$‹DΖ9Θuλ>΄&    ƒΒ‹ 9Αt‹X…Ϋuπ‹X…Ϋtμ‹[…Ϋuβ‹ 9Αuεt& ƒϊ‡'  „(  ‹vh…φu—‹l$ ‹4$‹\$ή‰D$‹€4  9Εt‹D$9„μ  …  ‹D$‹4  9έ„C  ‰Ψ1Òλt& ƒΒ‹ 9Εt‹p…φuπ‹H…Ιtμ‹I…Ιuβ‹ 9Εuεt& ƒϊ?‡1  φD$d„  ‹L$‹T$‰Ψ΄&    v …ÿt9xu9Ο…·   …Òt	;P„ª   ‹ 9Εuά‹T$‹D$…ÒuιΥ   v ‹9έ„ª  ;Cuρϋ πÿÿ‡Ξ  ‰ω…ÿ„λ  ‹$‰\$‰Λ1φ‹€”   ‹P@‹@<‰D$(‹D$`‰T$,‰D$0‹C(‹„°x  …ΐt1ΙT$(θόÿÿÿ‰D΄4…ΐ„w  ƒώtj‹C(Ύ   ‹„°x  …ΐuΡλζt& Ύκÿÿÿ‹D$Dd+    …>  ƒΔH‰π[^_]ΓφD$d…¬  t& ‹D$‹€4  9Ε…=ÿÿÿ1ΫιBÿÿÿ΄&    ‹\$…Ϋtu‹D$‹k‰{‹L$‹|$‰C‹D$4‰ϊ‰C‹D$8‰C‹$‰Έμ  θόυÿÿ‰Ζ…ΐ…ϊ   …ν„   ‰θθόÿÿÿ‹<$‹L$`1φ‹D$<‰ϊθόÿÿÿ‹L$`‹D$@‰ϊθόÿÿÿι8ÿÿÿ΅   Ή   Ίΐ  θόÿÿÿ‰Γ…ΐ„C  ‹t$‹L$‰+‹†8  ‰8  ‰C‰‹D$‰{‹|$‰C‹D$4‰ϊ‰C‹D$8‰C‹$‰Έμ  θUυÿÿ‰Ζ…ΐ…²   ‹D$$ΰϋÿÿθόÿÿÿιUÿÿÿ΄&    ‰ς‰Ψθόÿÿÿ‰D΄4= πÿÿ‡Ύ   ‰D΄<ιhώÿÿ΄&    ‹D$‹Hι	ώÿÿt& …νt[‰k‹D$<ΗC    θόÿÿÿ‹D$@θόÿÿÿιPώÿÿv ιΨόÿÿ‰θ‹l$ ¨…ΥόÿÿΎÿÿÿÿι.ώÿÿ‹D$Ύώÿÿÿ…ΐ…ώÿÿ1Ϋιύÿÿ‹D$<θόÿÿÿ‹D$@θόÿÿÿ‹C‹‰B‰‰ΨΗ   ΗC"  θόÿÿÿιαύÿÿΎώÿÿÿιΧύÿÿ‹D$<Ύτÿÿÿθόÿÿÿ‹D$@θόÿÿÿι»ύÿÿΎωÿÿÿι±ύÿÿθόÿÿÿ‰ήι¥ύÿÿ΄&    θόÿÿÿS‰Γφ@,tƒT  1Ò[ιόÿÿÿt& θόÿÿÿ‹C¨udÿ θόÿÿÿƒT  1Ò[ιόÿÿÿ¶    ‹Cπƒ λή΄&    ΄&    θόÿÿÿUΉ   ΊΠ  WV‰ΖSƒμdd΅    ‰D$`1ΐl$‰οσ«†T  ‰$hΐ  θόÿÿÿZ‰Η…ΐ…¶   ‹^h…Ϋt"θόÿÿÿ‹ƒT  ¨…μ   dÿ θόÿÿÿ‹[h…Ϋuή†4  μ  ‰ ‰@ƒΐ9Αuτ†H  ‰†H  ‰†L  λƒΗƒÿtxL½ ‰ϊ‰π‰λθΙρÿÿ…ΐtεl$`‹ƒΓθόÿÿÿ9λuς‹^h…Ϋtθόÿÿÿ‹ƒT  ¨u{dÿθόÿÿÿ‹[h…Ϋuβ‹$Ώτÿÿÿθόÿÿÿ‹D$`d+    u{ƒΔd‰ψ[^_]Γ΄&    f1Ϋ¶    ‹L ‹„Ψ  “τ   ƒΓ‰L–θόÿÿÿƒϋuή1ÿλ²f‹ƒX  πƒ ιÿÿÿ‹ƒX  πƒ(…xÿÿÿ‹“X  ƒT  ‹Rθόÿÿÿι_ÿÿÿθόÿÿÿv θόÿÿÿU‰ΥW‰ΟVS‰Γƒμ‹ θόÿÿÿ‰Ζ= πÿÿ‡Κ   ‹S‰ΠƒΰƒψtEΈ    ‹[‰$θόÿÿÿ‹$1Ι‰πR‰ϊSj θ›ψÿÿ‰ΓΈ    θόÿÿÿƒΔφF,thƒΔ‰Ψ[^_]Γt& ‹C‰κ1Ιθόÿÿÿ‰Ε= πÿÿws‹SΈ    ‹[‰$θόÿÿÿ‹$‰ι‰πR‰ϊSj θ>ψÿÿ‰ΓΈ    θόÿÿÿƒΔ…νt‰θθόÿÿÿφF,uθόÿÿÿ‹F¨uRdÿθόÿÿÿ‰ΨƒΔ[^_]ΓƒΔ‰Γ‰Ψ[^_]Γt& φF,t
‰λι]ÿÿÿv θόÿÿÿ‹F¨u-dÿθόÿÿÿ‰λι?ÿÿÿt& ‹Fπƒ(u¨‹VF‹Rθόÿÿÿλ‹Fπƒ(uΝ‹VF‹Rθόÿÿÿλ½΄&    ΄&    θόÿÿÿW‰ΧV‰ΖS‹ θόÿÿÿ‰Γ= πÿÿwt‹F‰ϊ1Ιθόÿÿÿ‹v‰ΗΈ    ÿ πÿÿwfθόÿÿÿ‰ϊ1Ι‰ΨVθ…ρÿÿ‰ΖΈ    θόÿÿÿZ…ÿt‰ψθόÿÿÿφC,t‰π[^_Γθόÿÿÿ‹C¨uDdÿθόÿÿÿ‰π[^_Γ¶    ‰Ζ[‰π^_Γ΄&    θόÿÿÿ1Ι1Ò‰ΨVθρÿÿ‰ΖΈ    θόÿÿÿXλ£‹Cπƒ(u¶‹SC‹Rθόÿÿÿλ¦΄&    θόÿÿÿUWV‰ΖSƒμ‹Nd΅    ‰D$1ΐ…Ι…σ   ‹F‰Υθόÿÿÿ‰Η= πÿÿ‡Τ   ΅   Ή0   Ίΐ θόÿÿÿ‰Γ…ΐ„ί   UΉ\  Ί   θόÿÿÿ‹F‰{(T$‰C,‰Ψθόÿÿÿ‰ΖX…φuS‹s,Έ    θόÿÿÿ1Ι1Ò‰ψjVSθÿυÿÿ‰ΖΈ    θόÿÿÿƒΔ‰ΰ…φuJθόÿÿÿ‰Ζ‹D$d+    uvƒΔ‰π[^_]Γt& ‰ΨθόÿÿÿφG,uΨθόÿÿÿ‹G¨u/dÿθόÿÿÿλΒt& θόÿÿÿλΨ΄&    f‰Ζλ©t& Ύκÿÿÿλ‹Gπƒ(uΛ‹WG‹Rθόÿÿÿλ»Ύτÿÿÿλθόÿÿÿ΄&    t& θόÿÿÿUWV‰ΦS‰Γƒμ d΅    ‰D$‹θόÿÿÿ‰Η= πÿÿ‡ς  Έ    θόÿÿÿ‹CΗ$κÿÿÿƒψ"‡<  Ύ€    …ΐ-  „   ‹”‡μ  ‹k‰L$Η4  ‰L$‹‡Ψ  ‹C‰T$‰L$ƒΰ„D  ‰Θθόÿÿÿ‰D$Ή   FT$θόÿÿÿ…ΐ…¬  FΉ   T$θόÿÿÿ…ΐ…“  ‹C‰ξ…ΐ„Ά   …ν„   ‹L$‰$…Ι„’   9ΘƒC  ‰D$Η$δÿÿÿφC…~  ‹D$1ν‹\Η‹D$9Ψtc‰|$‰Ηλ@t& ‹@(Ή   T$‹@ ‰D$‰πθόÿÿÿ…ΐ…  ƒΕ;l$„Ρ   ‹ƒΖ9ί„Δ   ‹C…ΐu½‹C…ΐt¶‹@λ±Η$    Έ    θόÿÿÿφG,t!‹D$d+    …ÿ   ‹$ƒΔ [^_]Γt& θόÿÿÿ‹G¨…¨   dÿθόÿÿÿλΕ¶    ‹L$‹TΟ‹L$9Ρth‰$‰Λλ
ƒΐ‹9Σt‹J…Ιuπ‹J…Ιtμ‹I…Ιuβλγ‹$ιƒώÿÿ΄&    ‰$ιoÿÿÿ΄&    ‹|$ιNÿÿÿ΄&    Η$    ιΌώÿÿt& 1ΐιDώÿÿ΄&    f‹|$Η$ςÿÿÿιÿÿÿ‹Gπƒ(…Nÿÿÿ‹WG‹Rθόÿÿÿι;ÿÿÿ‹L$‹D$‰κθόÿÿÿ‰$ιΰώÿÿθόÿÿÿt& θόÿÿÿUWVS‰ΓΏΫƒμ d΅    ‰D$1ΐΏD$4‰L$‰T$Αΰ	Ψ‰D$d‹    θόÿÿÿ‹ƒΘ  ‹T$8ΗD$    ‹@8‹΄Ψ  θόÿÿÿθόÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ΫtB~t& ‰|$>t& ‹K$S0D$θόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹\$θόÿÿÿ‹D$d+    ukƒΔ ‰Ψ[^_]Γt& θόÿÿÿS0‹s$‰ΕD$θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „KÿÿÿϋιEÿÿÿθόÿÿÿ΄&    ΄&    θόÿÿÿU‰ΕWV‰ΞΉ	   S‰ΣƒμP‹D$d|$(‰D$‹D$h‰D$d΅    ‰D$L1ΐ‰T$$Ίΐ  ‰l$ ΗD$    ΗD$    σ«‹D$lΗD$,   ‰t$<‹L$`‰D$@Έ   θόÿÿÿ‰D$(…ΐ„4  ‰ΑD$1ÒPD$0P‰Ψ‹{θόÿÿÿ[_…ΐ…  …φt‹D$‹…Ιt‹D$‹…Ò…  θόÿÿÿ‹T$pd‹    ‹ƒΘ  ΗD$    ‹@8‹΄Ψ  θόÿÿÿθόÿÿÿ‹ƒp  ~‰D$D$‰ƒp  ‹^…Ϋt@¶    ‰|$>t& ‹K$S0D$ θόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹\$θόÿÿÿ‹D$(θόÿÿÿƒϋu‹D$8…ΐ…™   ‹D$0θόÿÿÿ‹D$Ld+    …7  ƒΔP‰Ψ[^_]Γt& ΗD$,    ιαώÿÿv θόÿÿÿS0‹s$‰ΕD$ θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „ÿÿÿϋιÿÿÿv ‹|$‹θόÿÿÿ‹D$0‹L$‰‹D$4‰ιPÿÿÿ΄&    ‹L$`Ίΐ  Έ   θόÿÿÿΉ   ‰D$0‰Β‹D$‹ 9ΘGΑ‰D$4…ÒtR‹L$‹1ƒψs$…ΐ„.ώÿÿ¶
¨„!ώÿÿ·Lώf‰Lώιώÿÿ‹zƒηό‰
‹Lό‰Lό)ϊ)ΦΑισ¥ιούÿÿΗD$4    ιβύÿÿθόÿÿÿ΄&    t& θόÿÿÿUWV‰ΖSƒμp‹¶δ  ‰D$‹„$   ‰T$‹$„   1Ò‰D$‹„$   ‰L$‰D$d΅    ‰D$l1ΐ‰TLƒΐƒψ rτ1Ò1ΐ‰T,ƒΐƒψ rτ‹D$‹|$L$L‰D$(‹D$‹‹ ‰D$4‹D$‹ ‰D$8Έ   9ΒLΠD$(θήÿÿ‰D$…ΐxN‹?‹l$,9Η‰|$<Oψ…ÿρ  ‰ϊ1Ι‰θθόÿÿÿ‰ω‰Ϊ‰θθόÿÿÿ‰Η…ÿt=‹l$,ΗD$ςÿÿÿD$L9Εt‰θθόÿÿÿ‹D$ld+    …ν  ‹D$ƒΔp[^_]Γt& ‹D$1Òθόÿÿÿ‹  ΗD$$    θόÿÿÿ{θόÿÿÿd΅    ‹p  ‰L$L$ ‰p  ‹[…Ϋt;‰|$ >t& ‹K$S0D$(θόÿÿÿ‰Ζ…φu|$$ πÿÿwΗD$$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹\$$‹D$θόÿÿÿ…Ϋ…Η   ‹D$<‹l$,ƒψÿ„Ζ   ÿώÿÿ;D$υώÿÿ‹T$4‹L$‰‹T$8‹L$‰…ΐ„±   ‹L$‰L$L9Ν„¬   ‹D$ΗD$    ‰(ιΛώÿÿt& θόÿÿÿS0‹s$‰ΕD$(θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „σώÿÿϋινώÿÿv ‰\$‹l$,ιJώÿÿv ΗD$   ι:ώÿÿv ι ώÿÿΗD$    ι#ώÿÿΊΐ θόÿÿÿ‰Γ…ΐt%‹L$<‹T$,θόÿÿÿ‹D$ΗD$    ‰ιώÿÿθόÿÿÿΗD$τÿÿÿ‹l$,ιέύÿÿ¶    θόÿÿÿUWV‰ΖSƒμh‹δ  1φ‰D$‹D$|‹Ό$   ‰D$‹„$€   ‰D$d΅    ‰D$d1ΐ‰tDƒΐƒψ rτ1φ1ΐ‰t$ƒΐƒψ rτ‹D$‰T$,‰L$0L$D‰D$ d΅    ‰D$8‹„$„   ‰D$4‰ΒD$ θΫÿÿ‰D$…ΐX  …ÿuL‹D$θόÿÿÿ‰T$4…Òc  …ΐ…[  ‹D$‰Φ‹l$$9ΒOπ1Ι‰θ‰ςθόÿÿÿ‹T$‰ρ‰θθόÿÿÿ…ΐ…,  ‹D$1Òθόÿÿÿ‹›  ‰|$θόÿÿÿ{θόÿÿÿd΅    ‹p  ‰L$L$‰p  ‹[…Ϋt;‰|$>t& ‹K$S0D$ θόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹t$‹D$θόÿÿÿ…φχ   ‹\$4‹D$$;\$u…Ϋxq‰t$t6Ή   ‰Ϊ‰$θόÿÿÿ‹$‹D$‰Ωθόÿÿÿ…ΐuF‹D$4‹L$θόÿÿÿ…Ιu5‹D$$L$D9Θtθόÿÿÿ‹D$dd+    …”   ‹D$ƒΔh[^_]Γ΄&    ‹D$$ΗD$ςÿÿÿλΑ΄&    v θόÿÿÿS0‹s$‰ΕD$ θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „Γώÿÿϋι½ώÿÿv ‰t$ιJÿÿÿθόÿÿÿfθόÿÿÿUWV‰ΦS‰Γƒμ<‹|$T‹D$P‰|$d‹    ‰T$8‹“δ  ‰\$‹‰D$Ψ‰t$ ‰D$‹D$X‰L$$‰D$‹²  ‰\$(ΗD$0    d‹    ΗD$4    ‰\$,θόÿÿÿθόÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ΫtC~¶    ‰|$>t& ‹K$S0D$θόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹D$…ΐx‹L$‹T$(;€   …Òux‹T$8d+    uyƒΔ<[^_]Γv θόÿÿÿS0‹s$‰ΕD$θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „;ÿÿÿϋι5ÿÿÿv ‰λ„Έςÿÿÿιzÿÿÿθόÿÿÿ     cgroup_bpf_enabled_key  __cgroup_bpf_run_filter_skb  __cgroup_bpf_run_filter_sk  __cgroup_bpf_run_filter_sock_addr  __cgroup_bpf_run_filter_sock_ops                                                                        @   π  0  P  `  p  Π     °  @  ΐ  π  `	  ΐ  €                   p  °  π  °  0  p  €    ΐ  ΰ       0  P$  °$  @&  °'  (  ΰ)  ,   .  π0   4   6  cgroup_id:	%llu
attach_type:	%d
      ΐ      Τ  @        €      ¤         -  -      /  Θ/      T2   3      45  06      D7  Θ7      include/linux/skbuff.h / kernel/bpf/cgroup.c include/linux/thread_info.h °      φ	  ­     @       ƒ	 #     Υ 	 3  -   ι 	Έ$   θόÿÿÿ ›  “  ή    ›$  &  &  s'  '  s(  ³)  S,   ÿÿÿ	
ÿÿÿÿÿÿÿÿÿ strnlen           P      0                                          	                                                               @                           	                                                            	                                                             	                                                             	         
                                   °                                             
                                                                                                               ΐ  `  ΰ        p                       GCC: (GNU) 11.2.0           GNU  ΐ       ΐ                                  ρÿ              
              +             N              o             •   4          ½              γ   5            P          /            
 T  Q          €  s          ®             Ϊ  t            •          2             ]  @   ª    w  0      •  P       ­  `  
     Ε  p  V     δ  Π  /     ϊ                           V     )  `  E    @  @  y                 cmd_drivers/media/i2c/upd64031a.o := gcc -Wp,-MMD,drivers/media/i2c/.upd64031a.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"upd64031a"' -DKBUILD_MODNAME='"upd64031a"' -D__KBUILD_MODNAME=kmod_upd64031a -c -o drivers/media/i2c/upd64031a.o drivers/media/i2c/upd64031a.c 

source_drivers/media/i2c/upd64031a.o := drivers/media/i2c/upd64031a.c

deps_drivers/media/i2c/upd64031a.o := \
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
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
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
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
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
  include/asm