) \
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

drivers/media/i2c/vp27smpx.o: $(deps_drivers/media/i2c/vp27smpx.o)

$(deps_drivers/media/i2c/vp27smpx.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ELF                      "      4     (               èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[ÃèüÿÿÿWV‰ÆS»
   d‹=    ´&    v ‹†”   º)   èüÿÿÿƒàt¸Æ§  èüÿÿÿ‹¨uƒëuÖ¸ÿÿÿÿ[^_Ãt& èüÿÿÿëæèüÿÿÿVfÁÂS‹°”   ‰Ã·Êº&   ‰ğèüÿÿÿƒøÿt)‰Øèrÿÿÿ…Àx+º(   ‰ğèüÿÿÿfƒøÿtfÁÀ·À[^Ãf¡    …Àüÿÿÿ¸ÿÿÿÿëç´&    t& èüÿÿÿUW‰ÇV‰ÖS‰Ëƒì…É„–   ‹A‹)‰$ºó   ‰øèbÿÿÿ‰Á¡    …À   …Éx)‰Èƒà t0Ç$    1í¸   …Ût‹<$‰+‰{1É…öt‰ƒÄ‰È[^_]Ãt& ƒáº   Óâ÷Â uVâ  u6ƒùu¿Ç$    å  ÿ ë°´&    v Ç$    ½ÿÿÿ éaÿÿÿ´&    Ç$    å °  é|ÿÿÿ¶    ‰êÇ$    ¶êéeÿÿÿ´&    ´&    èüÿÿÿS‰Ãƒì‹    …É9   ƒÄ‰Ø1É[éÚşÿÿ´&    v èüÿÿÿS‰Ã‰Ñƒì¡    …ÀT   ƒÄ‰Ø1Ò[é©şÿÿ´&    fèüÿÿÿWfÁÂV‰ÆS‹¸”   ‰Ë‰Ñá ÿ  º'   ‰øèüÿÿÿƒøÿt1‰ğèšıÿÿ…ÀxfÁÃº(   ‰ø·Ëèüÿÿÿƒøÿt[^_Ã´&    v ¡    …Ào   ëãèüÿÿÿV‰Æ‰ÑSƒì¡    …ÀŠ   1Û…Ét
»   ¹   ‹†”   ‹P\ˆš"  ºò   èüÿÿÿ1ÀƒÄ[^Ãt& èüÿÿÿƒúwvV‰ÖS‰Ã¡    …À·   ‹ƒ”   ‹õ    ‹P\ˆŠc  º3   ¶Éèüÿÿÿºò   ‰Øè$ıÿÿƒàßx*‹õ   ºò   Áá	Á‰ØƒÉ·ÉèÑşÿÿ¸Æ§  èüÿÿÿ1À[^Ãf¸êÿÿÿÃ´&    v èüÿÿÿVS‹X‹P(³@ÿÿÿú	˜ „‰   w7ú 	˜ tWú	˜ uG‹H|‹CÔ‹P\ƒé@ˆŠ  ¶Éºç   èüÿÿÿ1À[^Ãv ú	˜ u·H|º   ‰ğè@şÿÿ1À[^Ãv ¸êÿÿÿ[^Ã‹H|‹CÔ‹P\ˆŠ  ¶Éºæ   èüÿÿÿ1À[^Ã´&    ·H|º    ‰ğèøıÿÿ1À[^Ãv èüÿÿÿU‰ÍW‰×ºò   V‰ÆSƒìèüÿÿ‹    ‰$…Û  ‰ø% °  …Ş   ‰ø„À…Œ   ‰ø%  ÿ „  ¸   ºˆ   »à  ·Èë
v ··KƒÃ¶Ò‰ğètıÿÿû  uä‹    …ÉÔ   ‰¾0  ·<$ºò   ‰ğ‰®4  ƒÏ·Ïè<ıÿÿ¸Æ§  èüÿÿÿ1ÀƒÄ[^_]Ã´&    ¸   ºˆ   »   ·Èët& ··KƒÃ¶Ò‰ğèôüÿÿûH  uä¡    …À~…éé   ¶    1Éº   »`  ë´&    v ··KƒÃ¶Ò‰ğè¬üÿÿûŒ  uä‹    …É8ÿÿÿéş   ¸êÿÿÿéVÿÿÿ¶    èüÿÿÿW1Éºò   V‰ÆS»   ë´&    ¶¶K‹†”   ƒÃ‹x\ˆŒ0  èüÿÿÿ…ÀxûÄ  uÕ1ÉºY   »`  ë´&    ··KƒÃ¶Ò‰ğèüÿÿûˆ  uä‹†0  öÄ°…}   ¹   ºˆ   „Àu?©  ÿ „¤   »à  ët& ··KƒÃ¶Ò‰ğè¼ûÿÿû  uä[1À^_Ã¶    »   ë··KƒÃ¶Ò‰ğèŒûÿÿûH  uä[1À^_Ã¶    1Éº   »`  ë´&    v ··KƒÃ¶Ò‰ğèLûÿÿûŒ  uä[1À^_Ã¶    »   ë··KƒÃ¶Ò‰ğèûÿÿûH  uä[1À^_Ã´&    ´&    èüÿÿÿUWV‰ÆSƒì‹@‹P‹Rèüÿÿÿ%  x =  x …d  F¹À  º@  èüÿÿÿ‰Ã…À„O  ¹    ‰ò»À   èüÿÿÿ1Éº   ‰øj Çƒ0  ÿ   Çƒ4      Çƒ8      Çƒ<     èüÿÿÿ¹ 	˜ ºP  ‰øj j j jj jjÿj€èüÿÿÿ¹	˜ ºP  ‰øƒÄ$j j j jj j?j j èüÿÿÿ¹	˜ ºP  ‰øƒÄ j h   j jj hÿ  j j èüÿÿÿ¹	˜ ºP  ‰øƒÄ j j j jj hÿ  jÿh şÿÿèüÿÿÿ‹«ü   ‰{l‰øƒÄ …íuCèüÿÿÿ1Ò‰ğèüÿÿÿº   ‰$‰ğèüÿÿÿº   ‰Ç‰ğèüÿÿÿÁç‰Â‰øĞ€<$ì…P  é+  èüÿÿÿƒÄ‰è[^_]Ã½íÿÿÿëî½ôÿÿÿëç                      P                      $                                                                          @                               €   ğ   ğ     P  À       `  Ğ  P      7%s: %s: failed
 7%s: status: 0x%04x
 7%s: g_input_status
 7%s: querystd
 on off 7%s: s_stream %s
 7%s: input switched to %s
 7%s: s_std %llx
 7%s: norm switched to NTSC
 7%s: norm switched to PAL
 7%s: norm switched to SECAM
 vpx3214c vpx3216b vpx3220a 6%s: %s found @ 0x%x (%s)
 vpx3220 internal composite svideo ƒÃph   Sh    èüÿÿÿƒÈÿƒÄéÇ   QƒÇp‰L$Wh   èüÿÿÿ‹L$ƒÄé)  @p‰$Ph(   èüÿÿÿXZ‹$é  Cp‰$Ph>   èüÿÿÿXZ‹$é6  ƒÆph(   Vh    èüÿÿÿƒÄé  ‰$…Ò¸N   ºQ   DÂPFpPhU   èüÿÿÿ‹L$ƒÄé×  Cpÿ4•Œ  Phh   èüÿÿÿƒÄé)  FpPhÏ   èüÿÿÿXZéî  FpPh³   èüÿÿÿ[Xéî  FpPh–   èüÿÿÿXZéî  FpUWPh„   èüÿÿÿƒÄé‰  f=€F„³   f=`B„â   ºí   f=€B„   ‹N·À‘  R·V1öÒRP¶|$CpWPh    èüÿÿÿƒÄ‹ƒ”   ¶¡  ¶–   ‹x\ˆŒ0  èüÿÿÿ…Àyq1ö·b  ¶–`  ‰ØƒÆèL  ƒş(uã1ö·"  ¶–   ‰ØƒÆèL  ƒş(uãéÑ  ºÿ   ‹F  P·F1öÀPCpRPh  èüÿÿÿƒÄéhÿÿÿƒÆƒş$t‡é[ÿÿÿºö   ëÃ6%s: chip (%02x:%04x) found @ 0x%x (%s)
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                                vpx3220_fp_read vpx3220_fp_write        vpx3220a                vpx3216b                vpx3214c                                                   À                             `                     ğ  À                                                              Ğ                                                     Y     £   ¤   ¨  ²  ¾  X   &   K ˜                        ò 3Ø¨ àÿá âã€äå€æ çàèøêğŠñøù$                            ˆ  ‰  Š  ‹  Œ     ğ w ò Õç A                        ˆ  ‰  Š  ‹  Œ     ğ w ò Ñç A                           ˆ  ‰ ğ Š ğ ‹   Œ € €  ğ s ò  ç á,  5  ?  debug parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=Laurent Pinchart description=vpx3220a/vpx3216b/vpx3214c video decoder driver   ˜            ÿ      GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                                                             _                                	 ,   €   d     Å           <           	 Q           \   ğ   ñ     k          	    ğ  &     –   =       	 ²      '     Ã   X       	 Ù   P  o     ñ   s       	   (          À  L     *     -    	 @    †     R           _  »       	 v  Œ       }     ½     Œ  `  j    š  à  (     ¥     (     ®  `  ,     ¸  Ø   W    	 Ë  Ğ  r    Ø     $     ä  `  (                   ì  P      ú              P         /  ÿ    	               *           >      €     M      
                   a  @   `     l     0     }  À   P                ¤          ½           Ë  ˜       İ  0        ô  <        
  T   <     %             0             N             e             r             ‹             š             ®             È             á             é                                       )             >             [             m             …             “             §           ³             Â      
     Ñ  @   `     ô              vpx3220.c vpx3220_remove vpx3220_fp_status vpx3220_fp_read vpx3220_fp_read.cold __func__.2 vpx3220_status vpx3220_status.cold vpx3220_g_input_status vpx3220_g_input_status.cold vpx3220_querystd vpx3220_querystd.cold vpx3220_fp_write.isra.0 vpx3220_fp_write.isra.0.cold __func__.1 vpx3220_s_stream vpx3220_s_stream.cold vpx3220_s_routing input_vals.3 vpx3220_s_routing.cold inputs vpx3220_s_ctrl vpx3220_s_std init_secam init_pal init_ntsc vpx3220_s_std.cold vpx3220_init init_common init_fp vpx3220_probe vpx3220_ops vpx3220_ctrl_ops vpx3220_probe.cold vpx3220_driver_init vpx3220_driver vpx3220_driver_exit vpx3220_id vpx3220_core_ops vpx3220_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free current_task i2c_smbus_read_byte_data __const_udelay __SCT__cond_resched i2c_smbus_write_word_data i2c_smbus_read_word_data _printk i2c_smbus_write_byte_data __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vpx3220_id_device_table param_ops_int      9     :     ;  !   9  2   <  L   =  [   >  y   ?     9     @  º   A  Ñ     ñ   9  !    ñ  9  ı    !  9  .    Q  9  v  @  ™  @  ±    Á  9  Ï       C    9  !    6  .  L  C  d  .  …  >  ¡  9  ì  C  8  C  a  9  ~    ¼  .  Ş  .  æ      >  ;  .  ^  .  e    €  .  ¦  .  ®    Ñ  9  â  .    C    .  #  .  F  .  u  .  –  .  ©  .  Æ  .  à  .    .    .  6  .  Q  9  h  D  Š  E  ™  .  ¦  F  Ş  G  è  .  ÿ  H  	  .  #  H  -  .  M  H  W  .  w  H    I  —  =  ¦  =  ´  =  Ñ  ;  Ù     )        6    ¹    ×    )    ‰    î    n    »    Ç    Ì                   h   .                                                 $     (     ,     0     4   *     .  
        B  (     -   B  E     J   B  `     e   B  w   .  }     ‚   B  ”     ™     ¦     «   B  Á   .  Ç     Ì   B  İ     â   B  ò     ÷   B        B      #  B  F    v  %  {  B  ‹  .  ’  .  ¡  C  ®  .  µ  .  ¿    Í  .  Ô  .  Ş    í    	      B  (         9     T     o     Š     ·     Ô     é     ş         +    è       9          J     K          M      .  ¬   .  À     Ì     Ø     ä     è     (    X    Œ        ”        .     J     P        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   î                    	   @       T  à              )             @	  €                   %   	   @       4                  /             À	                    8             À	  8                  4   	   @       T  p               E      2       ø	  F                X             >  .                 T   	   @       Ä  ¸     	         g      2       l  *                 z             –                    v   	   @       |                   ‰             ª  
                  …   	   @       œ                  ˜             À                    ”   	   @       ¬   `                             ^                    ­             ğ                    ©   	   @       !                  µ      0                          ¾                                   Î               (                                @       9         	              P                                 ,!  á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      "      4     (               èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[ÃèüÿÿÿWV‰ÆS»
   d‹=    ´&    v ‹†”   º)   èüÿÿÿƒàt¸Æ§  èüÿÿÿ‹¨uƒëuÖ¸ÿÿÿÿ[^_Ãt& èüÿÿÿëæèüÿÿÿVfÁÂS‹°”   ‰Ã·Êº&   ‰ğèüÿÿÿƒøÿt)‰Øèrÿÿÿ…Àx+º(   ‰ğèüÿÿÿfƒøÿtfÁÀ·À[^Ãf¡    …Àüÿÿÿ¸ÿÿÿÿëç´&    t& èüÿÿÿUW‰ÇV‰ÖS‰Ëƒì…É„–   ‹A‹)‰$ºó   ‰øèbÿÿÿ‰Á¡    …À   …Éx)‰Èƒà t0Ç$    1í¸   …Ût‹<$‰+‰{1É…öt‰ƒÄ‰È[^_]Ãt& ƒáº   Óâ÷Â uVâ  u6ƒùu¿Ç$    å  ÿ ë°´&    v Ç$    ½ÿÿÿ éaÿÿÿ´&    Ç$    å °  é|ÿÿÿ¶    ‰êÇ$    ¶êéeÿÿÿ´&    ´&    èüÿÿÿS‰Ãƒì‹    …É9   ƒÄ‰Ø1É[éÚşÿÿ´&    v èüÿÿÿS‰Ã‰Ñƒì¡    …ÀT   ƒÄ‰Ø1Ò[é©şÿÿ´&    fèüÿÿÿWfÁÂV‰ÆS‹¸”   ‰Ë‰Ñá ÿ  º'   ‰øèüÿÿÿƒøÿt1‰ğèšıÿÿ…ÀxfÁÃº(   ‰ø·Ëèüÿÿÿƒøÿt[^_Ã´&    v ¡    …Ào   ëãèüÿÿÿV‰Æ‰ÑSƒì¡    …ÀŠ   1Û…Ét
»   ¹   ‹†”   ‹P\ˆš"  ºò   èüÿÿÿ1ÀƒÄ[^Ãt& èüÿÿÿƒúwvV‰ÖS‰Ã¡    …À·   ‹ƒ”   ‹õ    ‹P\ˆŠc  º3   ¶Éèüÿÿÿºò   ‰Øè$ıÿÿƒàßx*‹õ   ºò   Áá	Á‰ØƒÉ·ÉèÑşÿÿ¸Æ§  èüÿÿÿ1À[^Ãf¸êÿÿÿÃ´&    v èüÿÿÿVS‹X‹P(³@ÿÿÿú	˜ „‰   w7ú 	˜ tWú	˜ uG‹H|‹CÔ‹P\ƒé@ˆŠ  ¶Éºç   èüÿÿÿ1À[^Ãv ú	˜ u·H|º   ‰ğè@şÿÿ1À[^Ãv ¸êÿÿÿ[^Ã‹H|‹CÔ‹P\ˆŠ  ¶Éºæ   èüÿÿÿ1À[^Ã´&    ·H|º    ‰ğèøıÿÿ1À[^Ãv èüÿÿÿU‰ÍW‰×ºò   V‰ÆSƒìèüÿÿ‹    ‰$…Û  ‰ø% °  …Ş   ‰ø„À…Œ   ‰ø%  ÿ „  ¸   ºˆ   »à  ·Èë
v ··KƒÃ¶Ò‰ğètıÿÿû  uä‹    …ÉÔ   ‰¾0  ·<$ºò   ‰ğ‰®4  ƒÏ·Ïè<ıÿÿ¸Æ§  èüÿÿÿ1ÀƒÄ[^_]Ã´&    ¸   ºˆ   »   ·Èët& ··KƒÃ¶Ò‰ğèôüÿÿûH  uä¡    …À~…éé   ¶    1Éº   »`  ë´&    v ··KƒÃ¶Ò‰ğè¬üÿÿûŒ  uä‹    …É8ÿÿÿéş   ¸êÿÿÿéVÿÿÿ¶    èüÿÿÿW1Éºò   V‰ÆS»   ë´&    ¶¶K‹†”   ƒÃ‹x\ˆŒ0  èüÿÿÿ…ÀxûÄ  uÕ1ÉºY   »`  ë´&    ··KƒÃ¶Ò‰ğèüÿÿûˆ  uä‹†0  öÄ°…}   ¹   ºˆ   „Àu?©  ÿ „¤   »à  ët& ··KƒÃ¶Ò‰ğè¼ûÿÿû  uä[1À^_Ã¶    »   ë··KƒÃ¶Ò‰ğèŒûÿÿûH  uä[1À^_Ã¶    1Éº   »`  ë´&    v ··KƒÃ¶Ò‰ğèLûÿÿûŒ  uä[1À^_Ã¶    »   ë··KƒÃ¶Ò‰ğèûÿÿûH  uä[1À^_Ã´&    ´&    èüÿÿÿUWV‰ÆSƒì‹@‹P‹Rèüÿÿÿ%  x =  x …d  F¹À  º@  èüÿÿÿ‰Ã…À„O  ¹    ‰ò»À   èüÿÿÿ1Éº   ‰øj Çƒ0  ÿ   Çƒ4      Çƒ8      Çƒ<     èüÿÿÿ¹ 	˜ ºP  ‰øj j j jj jjÿj€èüÿÿÿ¹	˜ ºP  ‰øƒÄ$j j j jj j?j j èüÿÿÿ¹	˜ ºP  ‰øƒÄ j h   j jj hÿ  j j èüÿÿÿ¹	˜ ºP  ‰øƒÄ j j j jj hÿ  jÿh şÿÿèüÿÿÿ‹«ü   ‰{l‰øƒÄ …íuCèüÿÿÿ1Ò‰ğèüÿÿÿº   ‰$‰ğèüÿÿÿº   ‰Ç‰ğèüÿÿÿÁç‰Â‰øĞ€<$ì…P  é+  èüÿÿÿƒÄ‰è[^_]Ã½íÿÿÿëî½ôÿÿÿëç                      P                      $                                                                          @                               €   ğ   ğ     P  À       `  Ğ  P      7%s: %s: failed
 7%s: status: 0x%04x
 7%s: g_input_status
 7%s: querystd
 on off 7%s: s_stream %s
 7%s: input switched to %s
 7%s: s_std %llx
 7%s: norm switched to NTSC
 7%s: norm switched to PAL
 7%s: norm switched to SECAM
 vpx3214c vpx3216b vpx3220a 6%s: %s found @ 0x%x (%s)
 vpx3220 internal composite svideo ƒÃph   Sh    èüÿÿÿƒÈÿƒÄéÇ   QƒÇp‰L$Wh   èüÿÿÿ‹L$ƒÄé)  @p‰$Ph(   èüÿÿÿXZ‹$é  Cp‰$Ph>   èüÿÿÿXZ‹$é6  ƒÆph(   Vh    èüÿÿÿƒÄé  ‰$…Ò¸N   ºQ   DÂPFpPhU   èüÿÿÿ‹L$ƒÄé×  Cpÿ4•Œ  Phh   èüÿÿÿƒÄé)  FpPhÏ   èüÿÿÿXZéî  FpPh³   èüÿÿÿ[Xéî  FpPh–   èüÿÿÿXZéî  FpUWPh„   èüÿÿÿƒÄé‰  f=€F„³   f=`B„â   ºí   f=€B„   ‹N·À‘  R·V1öÒRP¶|$CpWPh    èüÿÿÿƒÄ‹ƒ”   ¶¡  ¶–   ‹x\ˆŒ0  èüÿÿÿ…Àyq1ö·b  ¶–`  ‰ØƒÆèL  ƒş(uã1ö·"  ¶–   ‰ØƒÆèL  ƒş(uãéÑ  ºÿ   ‹F  P·F1öÀPCpRPh  èüÿÿÿƒÄéhÿÿÿƒÆƒş$t‡é[ÿÿÿºö   ëÃ6%s: chip (%02x:%04x) found @ 0x%x (%s)
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                                vpx3220_fp_read vpx3220_fp_write        vpx3220a                vpx3216b                vpx3214c                                                   À                             `                     ğ  À                                                              Ğ                                                     Y     £   ¤   ¨  ²  ¾  X   &   K ˜                        ò 3Ø¨ àÿá âã€äå€æ çàèøêğŠñøù$                            ˆ  ‰  Š  ‹  Œ     ğ w ò Õç A                        ˆ  ‰  Š  ‹  Œ     ğ w ò Ñç A                           ˆ  ‰ ğ Š ğ ‹   Œ € €  ğ s ò  ç á,  5  ?  debug parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=Laurent Pinchart description=vpx3220a/vpx3216b/vpx3214c video decoder driver   ˜            ÿ      GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                                                             _                                	 ,   €   d     Å           <           	 Q           \   ğ   ñ     k          	    ğ  &     –   =       	 ²      '     Ã   X       	 Ù   P  o     ñ   s       	   (          À  L     *     -    	 @    †     R           _  »       	 v  Œ       }     ½     Œ  `  j    š  à  (     ¥     (     ®  `  ,     ¸  Ø   W    	 Ë  Ğ  r    Ø     $     ä  `  (                   ì  P      ú              P         /  ÿ    	               *           >      €     M      
                   a  @   `     l     0     }  À   P                ¤          ½           Ë  ˜       İ  0        ô  <        
  T   <     %             0             N             e             r             ‹             š             ®             È             á             é                                       )             >             [             m             …             “             §           ³             Â      
     Ñ  @   `     ô              vpx3220.c vpx3220_remove vpx3220_fp_status vpx3220_fp_read vpx3220_fp_read.cold __func__.2 vpx3220_status vpx3220_status.cold vpx3220_g_input_status vpx3220_g_input_status.cold vpx3220_querystd vpx3220_querystd.cold vpx3220_fp_write.isra.0 vpx3220_fp_write.isra.0.cold __func__.1 vpx3220_s_stream vpx3220_s_stream.cold vpx3220_s_routing input_vals.3 vpx3220_s_routing.cold inputs vpx3220_s_ctrl vpx3220_s_std init_secam init_pal init_ntsc vpx3220_s_std.cold vpx3220_init init_common init_fp vpx3220_probe vpx3220_ops vpx3220_ctrl_ops vpx3220_probe.cold vpx3220_driver_init vpx3220_driver vpx3220_driver_exit vpx3220_id vpx3220_core_ops vpx3220_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free current_task i2c_smbus_read_byte_data __const_udelay __SCT__cond_resched i2c_smbus_write_word_data i2c_smbus_read_word_data _printk i2c_smbus_write_byte_data __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vpx3220_id_device_table param_ops_int      9     :     ;  !   9  2   <  L   =  [   >  y   ?     9     @  º   A  Ñ     ñ   9  !    ñ  9  ı    !  9  .    Q  9  v  @  ™  @  ±    Á  9  Ï       C    9  !    6  .  L  C  d  .  …  >  ¡  9  ì  C  8  C  a  9  ~    ¼  .  Ş  .  æ      >  ;  .  ^  .  e    €  .  ¦  .  ®    Ñ  9  â  .    C    .  #  .  F  .  u  .  –  .  ©  .  Æ  .  à  .    .    .  6  .  Q  9  h  D  Š  E  ™  .  ¦  F  Ş  G  è  .  ÿ  H  	  .  #  H  -  .  M  H  W  .  w  H    I  —  =  ¦  =  ´  =  Ñ  ;  Ù     )        6    ¹    ×    )    ‰    î    n    »    Ç    Ì                   h   .                                                 $     (     ,     0     4   *     .  
        B  (     -   B  E     J   B  `     e   B  w   .  }     ‚   B  ”     ™     ¦     «   B  Á   .  Ç     Ì   B  İ     â   B  ò     ÷   B        B      #  B  F    v  %  {  B  ‹  .  ’  .  ¡  C  ®  .  µ  .  ¿    Í  .  Ô  .  Ş    í    	      B  (         9     T     o     Š     ·     Ô     é     ş         +    è       9          J     K          M      .  ¬   .  À     Ì     Ø     ä     è     (    X    Œ        ”        .     J     P        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   î                    	   @       T  à              )             @	  €                   %   	   @       4                  /             À	                    8             À	  8                  4   	   @       T  p               E      2       ø	  F                X             >  .                 T   	   @       Ä  ¸     	         g      2       l  *                 z             –                    v   	   @       |                   ‰             ª  
                  …   	   @       œ                  ˜             À                    ”   	   @       ¬   `                             ^                    ­             ğ                    ©   	   @       !                  µ      0                          ¾                                   Î               (                                @       9         	              P                                 ,!  á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             