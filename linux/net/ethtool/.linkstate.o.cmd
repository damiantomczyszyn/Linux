ACPI_DOCK) \
  include/acpi/acpi_numa.h \
    $(wildcard include/config/ACPI_HMAT) \
  include/acpi/acpi_io.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ELF                      §k      4     ( & %             Ë¸ˇˇˇUd°    ãÄp  âÂ]ã@ô√ç∂    Ë¸ˇˇˇUâ¬d°    ãÄp  âÂâP1¿1“]√êË¸ˇˇˇWVSâ”â ∑KfÖ…Ñá   fÉ˘uq∂KâŒ¿ÈÉÊÉ¯Ñ§   ¡·∏    ∆a	ÒfâBçBàJ«B    ∂K∂«B    É„ÉÀaàZâÀÉ·¡„	ŸàJ	1…fâJ
ã|$)–¡¯«   [^_√ê[1¿^_√ç¥&    çv ∂ã|$∂KÉ‡«   [É»a^_àJà∏   fâB∏   «B    √çv Ä˘	Ñø   â<	Ñù   π	   â∂¯çAˇÉ‡9œD»â»∆{¡‡«B    	æ$   fâr∂ÒàB∂C¡Êâ˜«B    ÅŒy $ É‡Åœa   ¡‡	¯1ˇâB∂C∆BcÉ‡fâz	»«B    àB∂KçB «B    É·¡·	ÒâJÈ˙˛ˇˇç∂    Ä˘ï¡É¡Èjˇˇˇç¥&    çv π   ÈFˇˇˇç∂    Ë¸ˇˇˇUâÂÖ¿tã ]ã@$ãê  ãÄ   √ê∏    ]ãê  ãÄ   √ç¥&    ç∂    Ë¸ˇˇˇUWVâ∆Sâ”â ∑CfÉ¯Ñì  ifÉ¯Ñ«  fÉ¯u)∂C∆a«A    àA∏   fâA∏   [^_]√çt& êfÖ¿Öˇ   ∂C∆aàA1…[∏   ^_fâJ«B    ]√ç∂    fÉ¯ Ñﬁ   fÉ¯$ÖÑ   ∂Câ¡¿ËÉ·É˛Ño  ¡‡æ   ∆aøp  	»fârΩ   àB«B    ∂C∆Baâ¡É‡fâz
¡·«B    	»àB	∂C∆Baâ¡É‡fâj¡·«B    	»àB∏   È.ˇˇˇfÉ¯u:∂Câ¡¿ËÉ·É˛Ñ›   ∆a¡‡[^«B    	¡∏   _]fâB∏   àJ√[1¿^_]√ç¥&    fê∂Câ¡¿ËÉ·É˛Ñk  ∆a¡‡[^«B    	¡∏   _]fâB∏   àJ√ç∂    ∂Câ¡¿ËÉ·É˛Ñ#  ∆a¡‡[^«B    	¡∏   _]fâB∏   àJ√ç∂    ∂C∆aàAπ   [∏   ^_fâJ«B    ]√çt& ∆cÈˇˇˇ<	Ñÿ   æ	   Ä˘	Ñ‘   çFˇ∂˘É‡9˜Dâ∆{¡‡«B    	»âÒàB∂˘∏   fâBâ˘∂C¡Á¡·«B    âÕÉ‡«B    ¡‡ÅÕa  	ËâBâ»Å…y  a p	¯âB∂C∆BcÉ‡«B    	àB∏   fâB∂C«B$    É‡¡‡	»âB ∏   Èi˝ˇˇçv ∆cÈÿ˛ˇˇ∆cÈê˛ˇˇç¥&    êæ   È,ˇˇˇ<ï¿çpÈ-ˇˇˇç¥&    fêË¸ˇˇˇ1¿√ç¥&    êË¸ˇˇˇÈ¸ˇˇˇç∂    Ë¸ˇˇˇUWVâ∆∏    Sâ”Ë¸ˇˇˇãF(Ö¿t2ãÄî   ãx<ãh@∏    Ë¸ˇˇˇâ{âkãF,âC[1¿^_]√ç¥&    fê1ˇ1ÌÎ‘ç¥&    çv Ë¸ˇˇˇçP«@‡ˇˇˇçHâPâPã    «@  ∏    È¸ˇˇˇêË¸ˇˇˇUWVâ÷Sâ√∏    Ë¸ˇˇˇãC(Ö¿t2ãÄî   ãx<ãh@∏    Ë¸ˇˇˇˇs,UWh    VË¸ˇˇˇÉƒ[^_]√çt& 1ˇ1ÌÎ‘ç¥&    çv UWâ◊VSâ√ÉÏ d°    âD$1¿ˆCÖß   >çt& ãK$çS0â¯Ë¸ˇˇˇâ≈ˆCÖŸ   ãD$d+    Ö   Éƒ âË[^_]√ç∂    Ë¸ˇˇˇãs$çS0âD$â¯Ë¸ˇˇˇãsâ≈Ë¸ˇˇˇ4Ö    úè$˙ÉFÉÉV Ë¸ˇˇˇãL$1“)»FVÉF˜$   Ñzˇˇˇ˚Ètˇˇˇç¥&    ãB «B     âD$ãB$«B$    âD$ãB(«B(    âD$ãB,«B,    âD$ãB0«B0    âD$Èˇˇˇçt& êãD$âG ãD$âG$ãD$âG(ãD$âG,ãD$âG0Èˇ˛ˇˇË¸ˇˇˇç¥&    çt& Ë¸ˇˇˇUâÂWVSÉÏãPãMãuÖ“t]ãxâ√∫ˇˇˇˇ∏ÍˇˇˇÖˇt=É{ t7â$Ö…t=Öˆt9∏˘ˇˇˇ∫ˇˇˇˇÅ˛ˇ  wã$âÒâ¯Ë¸ˇˇˇâs1¿1“«C   Éƒ[^_]√çt& êÉƒ∏Íˇˇˇ∫ˇˇˇˇ[^_]√ç∂    Ë¸ˇˇˇWVSãt$Ö¿tbâ”Ö“tBâ«â Ö…tBÖˆt>9ÛâÒFÀË¸ˇˇˇ9Ûw∆Dˇ ∏˘ˇˇˇ[^_√)Ûç71“âŸË¸ˇˇˇâ[^_√çt& ∏˘ˇˇˇÎ›êâŸ1“â¯Ë¸ˇˇˇ∏ÍˇˇˇÎ ∏ÍˇˇˇÎ√ç¥&    Ë¸ˇˇˇUâÂWç}SãHããWˇpâÿËaˇˇˇYçe¯[ô_]√ç¥&    êË¸ˇˇˇUâÂWç}SããWãxÖˇu?Ö€t#Ö“tâ—âÿ1“Ë¸ˇˇˇçe¯∏Íˇˇˇ∫ˇˇˇˇ[_]√çv çe¯∏Íˇˇˇ∫ˇˇˇˇ[_]√ç¥&    ãHˇpâÿËÎ˛ˇˇYçe¯[ô_]√fêË¸ˇˇˇUWVâ∆Sâ”ÉÏã∫§   d°    âD$1¿ãÇ†   ∑íî   â$ÖˆÑ6  âÕ∂N∏   ”‡%øÔˇˇÑ˚   ∑FÉ‡˜fÉ¯Ö  ãK$)◊ãÜ‰  âsâL$ãKTãs8âì§   ˘âKT+KX â4$âS8É˝ÑÛ   ãÑ®ÿ  «D$    âD$Ë¸ˇˇˇË¸ˇˇˇãD$çL$dã    çhã≤p  âäp  ã@Ö¿t,êâ⁄âl$Ë¸ˇˇÖ¿uÅ|$ ˇˇw«D$ˇˇˇˇãEÉ≈Ö¿u’d°    â∞p  Ë¸ˇˇˇË¸ˇˇˇãD$Ö¿uOãSTã$)˙âK8âST;SXÇ  ª§   ã|$â{ãT$d+    ÖÙ   Éƒ[^_]√ç¥&    çv 1¿ÎŸçt& = ˇˇ∫ÚˇˇˇF¬Î¢ç¥&    fêã®‹  «D$    Ë¸ˇˇˇË¸ˇˇˇçud°    ãàp  âL$çL$âàp  ãE1ÌÖ¿t3fêâ⁄ât$Ë%˚ˇˇ®uÅ|$ ˇˇw«D$ˇˇˇˇ—ËÉ∆	≈ãÖ¿u“ÉÂãL$d°    âàp  Ë¸ˇˇˇË¸ˇˇˇãT$çD- Ö“Ñ˛˛ˇˇÅ˙ ˇˇ∏ÚˇˇˇF–ÖÌ∏   D¬È·˛ˇˇç¥&    êË¸ˇˇˇç¥&    fêË¸ˇˇˇUWâœVâ÷Sâ√ÉÏã@ Ö¿tË‚ˇˇˇâ≈Ö¿x|ããããË¸ˇˇˇâ√Ö¿xSã)¡âÎtG∫   ∏   â$ã.9—F—Ë¸ˇˇˇã$É¯v9¡Ö¸ˇˇˇçP9¡âËE ∫   Ë¸ˇˇˇÖ¿x,√)Éƒâÿ[^_]√çt& 1ÌÎàçt& Éƒâ√âÿ[^_]√çt& Éƒâ√âÿ[^_]√çt& Ë¸ˇˇˇUâÂVâ∆SÉÏãMãUd°    âD$1¿ãEâL$â$Ö¿tYÉ‚t,1€ãVãË¸ˇˇˇ√Ö¿I√ôãL$d+    u?Éƒ[^]√çt& ãÖ¿t"ã@ â‚çL$ËÃ˛ˇˇâ√ôÖ¿xÕãL$ã$Î≤çt& ∏Íˇˇˇ∫ˇˇˇˇÎ¥Ë¸ˇˇˇç¥&    Ë¸ˇˇˇÖ¿xOÉ˘tJVâ÷Sâ√çÉ¯w4âÿô˜˛Ö“u+É˚6ãD$«@   É˛wçVˇ∏   ÖÚtç¥&    çv 1¿[^√çv 1¿√çt& êÉ˛[^î¿√ç¥&    Ë¸ˇˇˇÖ¿x?Wâœ1…Vâ÷Sâ√çÉ¯w!âÿô˜˛Ö“uÉ˚~&ÉÎÉ˚wÉ˛î¡Éˇt3[â»^_√çt& ê1¿√çt& êÉˇuËãD$«@   É˛wÿÎç¥&    fêãD$1…«@   É˛wªçFˇÖî¡Î±çt& Ë¸ˇˇˇÉ¯'wVVâ÷ôSâ√˜˛1¿Ö“u"É˘trÉ˚Ñ°   Ö€t;É˚uNÉ˛Ñ≠   [^√fêÉ˚$u;1¿É˛uÔãD$Éxî¿Î‚ç¥&    1¿√çt& êÉ˛uŒãD$«    ∏   ÎΩfêÉ˛î¿Î≥ç¥&    êÉ˚ tÎ!É„˚É˚uúÉ˛uóãD$Éxî¿Îäç¥&    É˚$tàÈyˇˇˇç∂    É˛ÖjˇˇˇãD$« 	   ∏   ÈVˇˇˇçv ãD$«    ∏   È?ˇˇˇçt& Ë¸ˇˇˇÉ¯hÑ≤   w É¯fÑó   ∫   É¯guEâ–√ç¥&    çv É¯jtk∫    =†   t‚∫    É¯itÿ=∫   t~=ª   u9∫‡  â–√ç¥&    ∫Ä  É¯et±É¯Qtyw“É¯tb∫    É¯Ptõ∫    É¯tëÈ¸ˇˇˇç¥&    fêde/linux/objtool.h \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /* SPDX-License-Identifier: GPL-2.0 */
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
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef __LOCKING_LOCK_EVENTS_H
#define __LOCKING_LOCK_EVENTS_H

enum lock_events {

#include "lock_events_list.h"

	lockevent_num,	/* Total number of lock event counts */
	LOCKEVENT_reset_cnts = lockevent_num,
};

#ifdef CONFIG_LOCK_EVENT_COUNTS
/*
 * Per-cpu counters
 */
DECLARE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * Increment the statistical counters. use raw_cpu_inc() because of lower
 * overhead and we don't care if we loose the occasional update.
 */
static inline void __lockevent_inc(enum lock_events event, bool cond)
{
	if (cond)
		raw_cpu_inc(lockevents[event]);
}

#define lockevent_inc(ev)	  __lockevent_inc(LOCKEVENT_ ##ev, true)
#define lockevent_cond_inc(ev, c) __lockevent_inc(LOCKEVENT_ ##ev, c)

static inline void __lockevent_add(enum lock_events event, int inc)
{
	raw_cpu_add(lockevents[event], inc);
}

#define lockevent_add(ev, c)	__lockevent_add(LOCKEVENT_ ##ev, c)

#else  /* CONFIG_LOCK_EVENT_COUNTS */

#define lockevent_inc(ev)
#define lockevent_add(ev, c)
#define lockevent_cond_inc(ev, c)

#endif /* CONFIG_LOCK_EVENT_COUNTS */
#endif /* __LOCKING_LOCK_EVENTS_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0 */
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
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef __LOCKING_LOCK_EVENTS_H
#define __LOCKING_LOCK_EVENTS_H

enum lock_events {

#include "lock_events_list.h"

	lockevent_num,	/* Total number of lock event counts */
	LOCKEVENT_reset_cnts = lockevent_num,
};

#ifdef CONFIG_LOCK_EVENT_COUNTS
/*
 * Per-cpu counters
 */
DECLARE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * Increment the statistical counters. use raw_cpu_inc() because of lower
 * overhead and we don't care if we loose the occasional update.
 */
static inline void __lockevent_inc(enum lock_events event, bool cond)
{
	if (cond)
		raw_cpu_inc(lockevents[event]);
}

#define lockevent_inc(ev)	  __lockevent_inc(LOCKEVENT_ ##ev, true)
#define lockevent_cond_inc(ev, c) __lockevent_inc(LOCKEVENT_ ##ev, c)

static inline void __lockevent_add(enum lock_events event, int inc)
{
	raw_cpu_add(lockevents[event], inc);
}

#define lockevent_add(ev, c)	__lockevent_add(LOCKEVENT_ ##ev, c)

#else  /* CONFIG_LOCK_EVENT_COUNTS */

#define lockevent_inc(ev)
#define lockevent_add(ev, c)
#define lockevent_cond_inc(ev, c)

#endif /* CONFIG_LOCK_EVENT_COUNTS */
#endif /* __LOCKING_LOCK_EVENTS_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0 */
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
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef LOCK_EVENT
#define LOCK_EVENT(name)	LOCKEVENT_ ## name,
#endif

#ifdef CONFIG_QUEUED_SPINLOCKS
#ifdef CONFIG_PARAVIRT_SPINLOCKS
/*
 * Locking events for PV qspinlock.
 */
LOCK_EVENT(pv_hash_hops)	/* Average # of hops per hashing operation */
LOCK_EVENT(pv_kick_unlock)	/* # of vCPU kicks issued at unlock time   */
LOCK_EVENT(pv_kick_wake)	/* # of vCPU kicks for pv_latency_wake	   */
LOCK_EVENT(pv_latency_kick)	/* Average latency (ns) of vCPU kick	   */
LOCK_EVENT(pv_latency_wake)	/* Average latency (ns) of kick-to-wakeup  */
LOCK_EVENT(pv_lock_stealing)	/* # of lock stealing operations	   */
LOCK_EVENT(pv_spurious_wakeup)	/* # of spurious wakeups in non-head vCPUs */
LOCK_EVENT(pv_wait_again)	/* # of wait's after queue head vCPU kick  */
LOCK_EVENT(pv_wait_early)	/* # of early vCPU wait's		   */
LOCK_EVENT(pv_wait_head)	/* # of vCPU wait's at the queue head	   */
LOCK_EVENT(pv_wait_node)	/* # of vCPU wait's at non-head queue node */
#endif /* CONFIG_PARAVIRT_SPINLOCKS */

/*
 * Locking events for qspinlock
 *
 * Subtracting lock_use_node[234] from lock_slowpath will give you
 * lock_use_node1.
 */
LOCK_EVENT(lock_pending)	/* # of locking ops via pending code	     */
LOCK_EVENT(lock_slowpath)	/* # of locking ops via MCS lock queue	     */
LOCK_EVENT(lock_use_node2)	/* # of locking ops that use 2nd percpu node */
LOCK_EVENT(lock_use_node3)	/* # of locking ops that use 3rd percpu node */
LOCK_EVENT(lock_use_node4)	/* # of locking ops that use 4th percpu node */
LOCK_EVENT(lock_no_node)	/* # of locking ops w/o using percpu node    */
#endif /* CONFIG_QUEUED_SPINLOCKS */

/*
 * Locking events for rwsem
 */
LOCK_EVENT(rwsem_sleep_reader)	/* # of reader sleeps			*/
LOCK_EVENT(rwsem_sleep_writer)	/* # of writer sleeps			*/
LOCK_EVENT(rwsem_wake_reader)	/* # of reader wakeups			*/
LOCK_EVENT(rwsem_wake_writer)	/* # of writer wakeups			*/
LOCK_EVENT(rwsem_opt_lock)	/* # of opt-acquired write locks	*/
LOCK_EVENT(rwsem_opt_fail)	/* # of failed optspins			*/
LOCK_EVENT(rwsem_opt_nospin)	/* # of disabled optspins		*/
LOCK_EVENT(rwsem_rlock)		/* # of read locks acquired		*/
LOCK_EVENT(rwsem_rlock_steal)	/* # of read locks by lock stealing	*/
LOCK_EVENT(rwsem_rlock_fast)	/* # of fast read locks acquired	*/
LOCK_EVENT(rwsem_rlock_fail)	/* # of failed read lock acquisitions	*/
LOCK_EVENT(rwsem_rlock_handoff)	/* # of read lock handoffs		*/
LOCK_EVENT(rwsem_wlock)		/* # of write locks acquired		*/
LOCK_EVENT(rwsem_wlock_fail)	/* # of failed write lock acquisitions	*/
LOCK_EVENT(rwsem_wlock_handoff)	/* # of write lock handoffs		*/
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         /* SPDX-License-Identifier: GPL-2.0 */
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
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef LOCK_EVENT
#define LOCK_EVENT(name)	LOCKEVENT_ ## name,
#endif

#ifdef CONFIG_QUEUED_SPINLOCKS
#ifdef CONFIG_PARAVIRT_SPINLOCKS
/*
 * Locking events for PV qspinlock.
 */
LOCK_EVENT(pv_hash_hops)	/* Average # of hops per hashing operation */
LOCK_EVENT(pv_kick_unlock)	/* # of vCPU kicks issued at unlock time   */
LOCK_EVENT(pv_kick_wake)	/* # of vCPU kicks for pv_latency_wake	   */
LOCK_EVENT(pv_latency_kick)	/* Average latency (ns) of vCPU kick	   */
LOCK_EVENT(pv_latency_wake)	/* Average latency (ns) of kick-to-wakeup  */
LOCK_EVENT(pv_lock_stealing)	/* # of lock stealing operations	   */
LOCK_EVENT(pv_spurious_wakeup)	/* # of spurious wakeups in non-head vCPUs */
LOCK_EVENT(pv_wait_again)	/* # of wait's after queue head vCPU kick  */
LOCK_EVENT(pv_wait_early)	/* # of early vCPU wait's		   */
LOCK_EVENT(pv_wait_head)	/* # of vCPU wait's at the queue head	   */
LOCK_EVENT(pv_wait_node)	/* # of vCPU wait's at non-head queue node */
#endif /* CONFIG_PARAVIRT_SPINLOCKS */

/*
 * Locking events for qspinlock
 *
 * Subtracting lock_use_node[234] from lock_slowpath will give you
 * lock_use_node1.
 */
LOCK_EVENT(lock_pending)	/* # of locking ops via pending code	     */
LOCK_EVENT(lock_slowpath)	/* # of locking ops via MCS lock queue	     */
LOCK_EVENT(lock_use_node2)	/* # of locking ops that use 2nd percpu node */
LOCK_EVENT(lock_use_node3)	/* # of locking ops that use 3rd percpu node */
LOCK_EVENT(lock_use_node4)	/* # of locking ops that use 4th percpu node */
LOCK_EVENT(lock_no_node)	/* # of locking ops w/o using percpu node    */
#endif /* CONFIG_QUEUED_SPINLOCKS */

/*
 * Locking events for rwsem
 */
LOCK_EVENT(rwsem_sleep_reader)	/* # of reader sleeps			*/
LOCK_EVENT(rwsem_sleep_writer)	/* # of writer sleeps			*/
LOCK_EVENT(rwsem_wake_reader)	/* # of reader wakeups			*/
LOCK_EVENT(rwsem_wake_writer)	/* # of writer wakeups			*/
LOCK_EVENT(rwsem_opt_lock)	/* # of opt-acquired write locks	*/
LOCK_EVENT(rwsem_opt_fail)	/* # of failed optspins			*/
LOCK_EVENT(rwsem_opt_nospin)	/* # of disabled optspins		*/
LOCK_EVENT(rwsem_rlock)		/* # of read locks acquired		*/
LOCK_EVENT(rwsem_rlock_steal)	/* # of read locks by lock stealing	*/
LOCK_EVENT(rwsem_rlock_fast)	/* # of fast read locks acquired	*/
LOCK_EVENT(rwsem_rlock_fail)	/* # of failed read lock acquisitions	*/
LOCK_EVENT(rwsem_rlock_handoff)	/* # of read lock handoffs		*/
LOCK_EVENT(rwsem_wlock)		/* # of write locks acquired		*/
LOCK_EVENT(rwsem_wlock_fail)	/* # of failed write lock acquisitions	*/
LOCK_EVENT(rwsem_wlock_handoff)	/* # of write lock handoffs		*/
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/lockdep.c
 *
 * Runtime locking correctness validator
 *
 * Started by Ingo Molnar:
 *
 *  Copyright (C) 2006,2007 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2007 Red Hat, Inc., Peter Zijlstra
 *
 * this code maps all the lock dependencies as they occur in a live kernel
 * and will warn about the following classes of locking bugs:
 *
 * - lock inversion scenarios
 * - circular lock dependencies
 * - hardirq/softirq safe/unsafe locking bugs
 *
 * Bugs are reported even if the current locking scenario does not cause
 * any deadlock at this point.
 *
 * I.e. if anytime in the past two locks were taken in a different order,
 * even if it happened for another task, even if those were different
 * locks (but of the same class as this lock), this code will detect it.
 *
 * Thanks to Arjan van de Ven for coming up with the initial idea of
 * mapping lock dependencies runtime.
 */
#define DISABLE_BRANCH_PROFILING
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/interrupt.h>
#include <linux/stacktrace.h>
#include <linux/debug_locks.h>
#include <linux/irqflags.h>
#include <linux/utsname.h>
#include <linux/hash.h>
#include <linux/ftrace.h>
#include <linux/stringify.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/gfp.h>
#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/nmi.h>
#include <linux/rcupdate.h>
#include <linux/kprobes.h>
#include <linux/lockdep.h>

#include <asm/sections.h>

#include "lockdep_internals.h"

#define CREATE_TRACE_POINTS
#include <trace/events/lock.h>

#ifdef CONFIG_PROVE_LOCKING
int prove_locking = 1;
module_param(prove_locking, int, 0644);
#else
#define prove_locking 0
#endif

#ifdef CONFIG_LOCK_STAT
int lock_stat = 1;
module_param(lock_stat, int, 0644);
#else
#define lock_stat 0
#endif

DEFINE_PER_CPU(unsigned int, lockdep_recursion);
EXPORT_PER_CPU_SYMBOL_GPL(lockdep_recursion);

static __always_inline bool lockdep_enabled(void)
{
	if (!debug_locks)
		return false;

	if (this_cpu_read(lockdep_recursion))
		return false;

	if (current->lockdep_recursion)
		return false;

	return true;
}

/*
 * lockdep_lock: protects the lockdep graph, the hashes and the
 *               class/list/hash allocators.
 *
 * This is one of the rare exceptions where it's justified
 * to use a raw spinlock - we really dont want the spinlock
 * code to recurse back into the lockdep code...
 */
static arch_spinlock_t __lock = (arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED;
static struct task_struct *__owner;

static inline void lockdep_lock(void)
{
	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	__this_cpu_inc(lockdep_recursion);
	arch_spin_lock(&__lock);
	__owner = current;
}

static inline void lockdep_unlock(void)
{
	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	if (debug_locks && DEBUG_LOCKS_WARN_ON(__owner != current))
		return;

	__owner = NULL;
	arch_spin_unlock(&__lock);
	__this_cpu_dec(lockdep_recursion);
}

static inline bool lockdep_assert_locked(void)
{
	return DEBUG_LOCKS_WARN_ON(__owner != current);
}

static struct task_struct *lockdep_selftest_task_struct;


static int graph_lock(void)
{
	lockdep_lock();
	/*
	 * Make sure that if another CPU detected a bug while
	 * walking the graph we dont change it (while the other
	 * CPU is busy printing out stuff with the graph lock
	 * dropped already)
	 */
	if (!debug_locks) {
		lockdep_unlock();
		return 0;
	}
	return 1;
}

static inline void graph_unlock(void)
{
	lockdep_unlock();
}

/*
 * Turn lock debugging off and return with 0 if it was off already,
 * and also release the graph lock:
 */
static inline int debug_locks_off_graph_unlock(void)
{
	int ret = debug_locks_off();

	lockdep_unlock();

	return ret;
}

unsigned long nr_list_entries;
static struct lock_list list_entries[MAX_LOCKDEP_ENTRIES];
static DECLARE_BITMAP(list_entries_in_use, MAX_LOCKDEP_ENTRIES);

/*
 * All data structures here are protected by the global debug_lock.
 *
 * nr_lock_classes is the number of elements of lock_classes[] that is
 * in use.
 */
#define KEYHASH_BITS		(MAX_LOCKDEP_KEYS_BITS - 1)
#define KEYHASH_SIZE		(1UL << KEYHASH_BITS)
static struct hlist_head lock_keys_hash[KEYHASH_SIZE];
unsigned long nr_lock_classes;
unsigned long nr_zapped_classes;
unsigned long max_lock_class_idx;
struct lock_class lock_classes[MAX_LOCKDEP_KEYS];
DECLARE_BITMAP(lock_classes_in_use, MAX_LOCKDEP_KEYS);

static inline struct lock_class *hlock_class(struct held_lock *hlock)
{
	unsigned int class_idx = hlock->class_idx;

	/* Don't re-read hlock->class_idx, can't use READ_ONCE() on bitfield */
	barrier();

	if (!test_bit(class_idx, lock_classes_in_use)) {
		/*
		 * Someone passed in garbage, we give up.
		 */
		DEBUG_LOCKS_WARN_ON(1);
		return NULL;
	}

	/*
	 * At this point, if the passed hlock->class_idx is still garbage,
	 * we just have to live with it
	 */
	return lock_classes + class_idx;
}

#ifdef CONFIG_LOCK_STAT
static DEFINE_PER_CPU(struct lock_class_stats[MAX_LOCKDEP_KEYS], cpu_lock_stats);

static inline u64 lockstat_clock(void)
{
	return local_clock();
}

static int lock_point(unsigned long points[], unsigned long ip)
{
	int i;

	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		if (points[i] == 0) {
			points[i] = ip;
			break;
		}
		if (points[i] == ip)
			break;
	}

	return i;
}

static void lock_time_inc(struct lock_time *lt, u64 time)
{
	if (time > lt->max)
		lt->max = time;

	if (time < lt->min || !lt->nr)
		lt->min = time;

	lt->total += time;
	lt->nr++;
}

static inline void lock_time_add(struct lock_time *src, struct lock_time *dst)
{
	if (!src->nr)
		return;

	if (src->max > dst->max)
		dst->max = src->max