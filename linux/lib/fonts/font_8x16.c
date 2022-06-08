TATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/ioport.h \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/irqhandler.h \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/fwnode.h \
  include/linux/resource_ext.h \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  include/linux/ctype.h \
  arch/x86/include/asm/acenv.h \
  include/acpi/acnames.h \
  include/acpi/actypes.h \
  include/acpi/acexcep.h \
  include/acpi/actbl.h \
  include/acpi/actbl1.h \
  include/acpi/actbl2.h \
  include/acpi/actbl3.h \
  include/acpi/acrestyp.h \
  include/acpi/platform/acenvex.h \
  include/acpi/platform/aclinuxex.h \
  include/acpi/platform/acgccex.h \
  include/acpi/acoutput.h \
  include/acpi/acpiosxf.h \
  include/acpi/acpixf.h \
  include/acpi/acconfig.h \
  include/acpi/acbuffer.h \
  include/linux/dynamic_debug.h \
  include/acpi/acpi_bus.h \
    $(wildcard include/config/X86_ANDROID_TABLETS) \
    $(wildcard include/config/ACPI_SYSTEM_POWER_STATES_SUPPORT) \
    $(wildcard include/config/ACPI_SLEEP) \
  include/acpi/acpi_drivers.h \
    $(wildcard include/config/ACPI_DOCK) \
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
		dst->max = src->max;

	if (src->min < dst->min || !dst->nr)
		dst->min = src->min;

	dst->total += src->total;
	dst->nr += src->nr;
}

struct lock_class_stats lock_stats(struct lock_class *class)
{
	struct lock_class_stats stats;
	int cpu, i;

	memset(&stats, 0, sizeof(struct lock_class_stats));
	for_each_possible_cpu(cpu) {
		struct lock_class_stats *pcs =
			&per_cpu(cpu_lock_stats, cpu)[class - lock_classes];

		for (i = 0; i < ARRAY_SIZE(stats.contention_point); i++)
			stats.contention_point[i] += pcs->contention_point[i];

		for (i = 0; i < ARRAY_SIZE(stats.contending_point); i++)
			stats.contending_point[i] += pcs->contending_point[i];

		lock_time_add(&pcs->read_waittime, &stats.read_waittime);
		lock_time_add(&pcs->write_waittime, &stats.write_waittime);

		lock_time_add(&pcs->read_holdtime, &stats.read_holdtime);
		lock_time_add(&pcs->write_holdtime, &stats.write_holdtime);

		for (i = 0; i < ARRAY_SIZE(stats.bounces); i++)
			stats.bounces[i] += pcs->bounces[i];
	}

	return stats;
}

void clear_lock_stats(struct lock_class *class)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct lock_class_stats *cpu_stats =
			&per_cpu(cpu_lock_stats, cpu)[class - lock_classes];

		memset(cpu_stats, 0, sizeof(struct lock_class_stats));
	}
	memset(class->contention_point, 0, sizeof(class->contention_point));
	memset(class->contending_point, 0, sizeof(class->contending_point));
}

static struct lock_class_stats *get_lock_stats(struct lock_class *class)
{
	return &this_cpu_ptr(cpu_lock_stats)[class - lock_classes];
}

static void lock_release_holdtime(struct held_lock *hlock)
{
	struct lock_class_stats *stats;
	u64 holdtime;

	if (!lock_stat)
		return;

	holdtime = lockstat_clock() - hlock->holdtime_stamp;

	stats = get_lock_stats(hlock_class(hlock));
	if (hlock->read)
		lock_time_inc(&stats->read_holdtime, holdtime);
	else
		lock_time_inc(&stats->write_holdtime, holdtime);
}
#else
static inline void lock_release_holdtime(struct held_lock *hlock)
{
}
#endif

/*
 * We keep a global list of all lock classes. The list is only accessed with
 * the lockdep spinlock lock held. free_lock_classes is a list with free
 * elements. These elements are linked together by the lock_entry member in
 * struct lock_class.
 */
static LIST_HEAD(all_lock_classes);
static LIST_HEAD(free_lock_classedcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/irqhandler.h \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/CMA) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/linux/seqlock.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/osq_lock.h \
  include/linux/debug_locks.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  include/linux/rbtree_types.h \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/generated/bounds.h \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/tsc.h \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
    $(wildcard include/config/PCI) \
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
  include/linux/fwnode.h \
  include/linux/vmalloc.h \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  include/linux/overflow.h \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  arch/x8
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
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/socket.h \
  arch/x86/include/generated/uapi/asm/socket.h \
  include/uapi/asm-generic/socket.h \
  arch/x86/include/generated/uapi/asm/sockios.h \
  include/uapi/asm-generic/sockios.h \
  include/uapi/linux/sockios.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/uapi/linux/socket.h \
  include/linux/net.h \
  include/linux/random.h \
    $(wildcard include/config/VMGENID) \
    $(wildcard include/config/ARCH_RANDOM) \
  include/linux/once.h \
  include/uapi/linux/random.h \
  include/linux/prandom.h \
  include/linux/siphash.h \
  arch/x86/include/asm/archrandom.h \
  include/uapi/linux/net.h \
  include/linux/textsearch.h \
  include/net/checksum.h \
  arch/x86/include/asm/checksum.h \
  arch/x86/include/asm/checksum_32.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/uapi/linux/libc-compat.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/DMA_API_DEBUG) \
    $(wildcard include/config/HAS_DMA) \
    $(wildcard include/config/NEED_DMA_MAP_STATE) \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/dma-direction.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  arch/x86/include/asm/io.h \
    $(wildcard includeELF                      L"      4     (               Ë¸ˇˇˇãÖ¿uãAÉ¯wã≈¨  âA1¿√∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇâ¬â»ãIÖ…upãä8  âHãä<  âHãä@  âHãäD  âHãäH  âHãäL  âHãäP  âH ãäT  âH$ãäX  âH(ãä\  âH,ãä`  âH0ãíd  âP41¿√ê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇãà4  âJãÄ0  âB1¿√ç∂    Ë¸ˇˇˇSãX\âÿË¸ˇˇˇãClË¸ˇˇˇ1¿[√çt& Ë¸ˇˇˇÉÏãÄî   dã    âL$1…πÄˇˇfâL$Ö“t7∆D$j π   çT$Ë¸ˇˇˇX∏ºç Ë¸ˇˇˇãD$d+    u1¿Éƒ√çt& ∆D$Î«Ë¸ˇˇˇçt& Ë¸ˇˇˇSâ√ÉÏãJd°    âD$1¿Ö…tãBÖ¿Öﬂ   «B   ∏   «B   âÉ0  ãB∫Äˇˇπ   fâT$âÉ4  ãÉî   ∆D$ j çT$Ë¸ˇˇˇãì0  πÅˇˇãÉî   fâL$π   àt$j çT$	Ë¸ˇˇˇãì0  πÇˇˇãÉî   fâL$	π   àT$j çT$Ë¸ˇˇˇãì4  ãÉî   ªÑˇˇfâ\$π   àT$j çT$Ë¸ˇˇˇÉƒãD$d+    uÉƒ1¿[√çv k…9»Ü)ˇˇˇÈˇˇˇË¸ˇˇˇçv Ë¸ˇˇˇUâ≈WVSÉÏd°    âD$ãEãPãRË¸ˇˇˇ®Ñ)  ãEXâ$Ö¿Ñ"  h    çuãπ   âË¸ˇˇˇâ√XÖ€Ö¸ˇˇˇ∏d   Ë¸ˇˇˇπ¿  ∫l  âË¸ˇˇˇâ√Ö¿Ñ„  π@   âÍæ†  Ë¸ˇˇˇπ   ∫ÅˇˇÎê∑Nf¡¬àL$ãÉî   É∆fâT$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÕ∫¿  ∆D$ãÉî   π   fâT$j çT$	Ë¸ˇˇˇπ¿D  ãÉî   fâL$	π   ∆D$j çT$Ë¸ˇˇˇπ   ∫¯*  ∏'  Ë¸ˇˇˇπ   ^∫oÄˇˇ_æ|  Îçt& ∑Nf¡¬àL$ãÉî   É∆fâT$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÕ∫Ñ   æ¥  ø@  Îçt& ∑Wf¡∆àT$É«π   fât$ãÉî   j çT$	Ë¸ˇˇˇ∑7XfÖˆuÕ∫%Üˇˇ∆D$ãÉî   π   fâT$ø‡  j çT$	Ë¸ˇˇˇ∫  YÎçv ∑wf¡¬âÒãÉî   É«fâT$àL$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÀ∫Äˇˇ∆D$ π   ãÉî   fâT$æÇˇˇøÑˇˇ«É0     «É4     j çT$	Ë¸ˇˇˇãì0  πÅˇˇãÉî   fâL$	π   àt$j çT$Ë¸ˇˇˇãì0  ãÉî   π   fât$ç≥¿   àT$j çT$Ë¸ˇˇˇãì4  ãÉî   π   fâ|$çª8  àT$j çT$Ë¸ˇˇˇ1¿π   ∑UÛ´ãD$«É8  Ä  «É<  ‡  «É@     «ÉD     «ÉH     ã âÉh  ãEçà  QçQRˇ∞   ãETˇ0h,   Ë¸ˇˇˇ1…∫   âÉƒ(j Ë¸ˇˇˇπ	ò ∫0  âj há   j jj hˇ   j j Ë¸ˇˇˇπ	ò ∫0  âÉƒ$j jxj jj hˇ   j j Ë¸ˇˇˇπ	ò ∫0  âÉƒ j j j jj jj j Ë¸ˇˇˇπ	ò ∫0  âÉƒ j j j jj jj j Ë¸ˇˇˇâslãõ¸   âÉƒ Ö€u6Ë¸ˇˇˇâ√Ö¿uãD$d+    u:Éƒâÿ[^_]√çt& âË¸ˇˇˇÎ‹ç¥&    Ë¸ˇˇˇÎŒª˚ˇˇˇÎ«ªÍˇˇˇÎ¿ªÙˇˇˇÎπË¸ˇˇˇç¥&    ç¥&    êË¸ˇˇˇSÉÏãHdã    âT$1“ãP(Å˙	ò Ñê   wNÅ˙	ò tnÅ˙	ò u^∫∂ˇˇãY‘ã@|fâT$àD$π   âÿj çT$Ë¸ˇˇˇX1¿ãT$d+    uZÉƒ[√çv Å˙	ò u∫ºˇˇãY‘ã@|fâT$Î∏ç∂    ∏ÍˇˇˇÎƒêãY‘π¥ˇˇã@|fâL$Îòç∂    ãY‘π∫ˇˇã@|fâL$ÎÄË¸ˇˇˇêË¸ˇˇˇUWVâ∆SÉÏd°    âD$1¿ãAÖ¿Öé   ãAâ◊âÀçê˙ﬂˇˇÉ‚˝Ñ%  =  Ñ*  «A   π   ãCãk«C   âKâ¬É‚¸=Å  ∏Ä  C–âËÉ‡¸Å˝·  Ω‡  C≈ã+âSâCÖÌtEãC=   Ñó  =   ÑD  =  tu∏ÍˇˇˇãT$d+    Ö¬  Éƒ[^_]√ç¥&    ãâãSâPãSâPãSâPãSâPãSâPãS âPãS$âPãS(âP ãS,âP$ãS0âP(ãS4âP,1¿Îñç∂    ∫∞ˇˇç|$∆D$ãÜî   fâT$π   â˙j Ë¸ˇˇˇπ%îˇˇãÜî   fâL$∆D$ j π   â˙Ë¸ˇˇˇãCY]ãÆî   =Ä  Ñ∏  =@  ÖÌ  Å{   Ñ⁄  ∏Äˇˇ∆D$π   â˙fâD$âËΩÑˇˇj Ë¸ˇˇˇãSπÉˇˇãÜî   fâL$π   àt$â˙j Ë¸ˇˇˇãSãÜî   π   fâl$	ΩàˇˇàT$â˙j Ë¸ˇˇˇãSπáˇˇãÜî   fâL$π   àt$â˙j Ë¸ˇˇˇãSãÜî   π   fâl$àT$â˙j Ë¸ˇˇˇ∫ûˇˇãÜî   π   fâT$â˙∆D$j Ë¸ˇˇˇÉƒãCâÜ8  ãCâÜ<  ãCâÜ@  ãCâÜD  ãCâÜH  ãCâÜL  ãC âÜP  ãC$âÜT  ãC(âÜX  ãC,âÜ\  ãC0âÜ`  ãC4âÜd  1¿ÈÃ˝ˇˇçt& ø∞ˇˇ∆D$ ãÜî   π   fâ|$ç|$Ω%ñˇˇj â˙Ë¸ˇˇˇãÜî   fâl$∆D$È3˛ˇˇç¥&    ∫∞ˇˇç|$∆D$ ãÜî   fâT$π   â˙j Ë¸ˇˇˇπ%ñˇˇãÜî   fâL$∆D$ÈÎ˝ˇˇç¥&    =†   u!É{xÖ˛ˇˇ∏Äˇˇ∆D$fâD$Î~ç∂    =`  u!Å{   Ö‰˝ˇˇ∏Äˇˇ∆D$fâD$ÎSçv =∞   u!Å{ê   Öº˝ˇˇ∏Äˇˇ∆D$fâD$Î+çv É¯XÖü˝ˇˇÉ{HÖï˝ˇˇπÄˇˇ∆D$fâL$ç∂    j π   â˙âËË¸ˇˇˇXÈC˛ˇˇç¥&    çv Å{‡  ÖS˝ˇˇ∫Äˇˇ∆D$fâT$Î¬fêπ   ÈË˚ˇˇç∂    π   Èÿ˚ˇˇ∏Äˇˇ∆D$fâD$ÎïË¸ˇˇˇ                    ê  ‡                                                                                                                          0   ¿   ‡      p  ê    ‡      VS6624 Chip Enable vs6624   3%s %d-%04x: failed to request GPIO %d
    6%s %d-%04x: chip found @ 0x%02x (%s)
 ã$ˇ0∑EPãEˇ∞   ãETˇ0h    Ë¸ˇˇˇÉƒÈ±  Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇlicense=GPL v2 author=Scott Jiang <Scott.Jiang.Linux@gmail.com> description=VS6624 sensor driver                                vs6624                                                                     †               `                   0   ‡                                                                                         ¿   p                                                                                                        ¥Ñ ∂u ∏ 4Ñ 6u 8 ÇÄ ÑÄ ÜÄ Ä ê˛ Ç  Äd     ( ∫  º  ã  å è  ê ï  ñ ô  ö  Ñ%! Ä  Ñ Ç Ç  ∞  ñ% Ä                            <  f e — f b      &  & &c &— &h &› &: &  Ä$  ä0 ëb íJ ïe ñ °: ¢∏  

 :  ; Ö 	; 
Ö :  0 è      oÄ å                             Å Å  … …G … …Ä …: :ê ;êG <ê …1 	…‹ 
…Ä …D Dê Eê1 Fê‚ … …‡ …Ä …G Gêê HêÉ IêÅ Jê‡ Kê` Lê Mêê Nê¿ OêC Pêt Qê Rê SêÄ Tê Uê‰ Vêê Wê¿ XêC Yê Zê [ê \êÏ …] …  …Ä …] ]ê£ ^ê _ê `ê£ aê bê cê" …r …í …Ä …d dêt eê fê gêr hêï …G …Ú …Å …i iët jë kë lëÏ më¥ në oë
 pëê qëÄ rë së‡ tëp uë vëê wë” xëƒ yë zë" …
 …æ …Ä …s sê¸ tê£ uê‡ vêı wêÇ xêå yêÉ zê£ {ê£ |ê‡ }ê¸ ~ê£ ê‡ Äê√ Åêü Çêˇ ÉêÏ Ñêû Öê˛ Üê áê
 àêÍ  …G !…8 "…Ä #…â âêÏ äê” ãêî åê  çê@ éê èê êêê ëê” íê‘ ìêÏ îê ïê ñêG óê= $…E %…  &…Ä '…ò òê ôêw öê÷ õê úêE ùêÕ (…  )…’ *…Ä +…û ûêê üêÇ †ê °ê‡ ¢ê¥ £ê §ê •êê ¶êÉ ßêø ®ê‡ ©ê` ™ê ´êê ¨êÅ ≠ê¸ Æê‡ Øêˇ ∞ê√ ±ê ≤ê ≥êê ¥êÅ µê¸ ∂ê‡ ∑êˇ ∏ê πê  ∫ê⁄ ,…p -…º .…Ä /…ª ªêê ºêÇ Ωê æê‡ øê¥ ¿ê ¡ê ¬êê √ê¡ ƒê ≈êt ∆ê «ê »êê …ê”  ê† Àê Ãêp Õêø 0…r 1…! 2…Å 3…; ;ë} <ë =ë >ë{ ?ë @ër Aë% 4…( 5…Æ 6…Ä 7…“ “ê ”êê ‘ê“ ’ê
 ÷ê ◊ê( ÿê¥ 8…( 9…± :…Ä ;…Ÿ Ÿêê ⁄êÉ €ê∫ ‹ê‡ ›êˇ ﬁêê ﬂê“ ‡ê ·ê‡ ‚ê‰ „êÔ ‰ê Âê£ Êê‡ Áêt Ëêˇ Èê Íêê Îê“ Ïê
 Ìê Óê( Ôê¥ <…) =…y >…Ä ?… ê Òêê Úê“ Ûê Ùê ıê) ˆê @…) A…| B…Ä C…˜ ˜êê ¯êÉ ˘ê∫ ˙ê‡ ˚êˇ ¸êê ˝ê“ ˛ê ˇê‡  ë‰ ëÔ ë ë£ ë‡ ët ëˇ ë ëê 	ë“ 
ë ë ë) ë D…* E…B F…Å G… ë ëê ë“ ë ë ë* ëH H…* I…E J…Å K… ëê ëÉ ë∫ ë‡ ëˇ ëê ë“ ë ë‡ ë‰ ëÔ  ë !ë£ "ë‡ #ët $ëˇ %ë &ëê 'ë“ (ë )ë *ë* +ëH  …                       GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                            
       &         ¨       /   0   Ü     >   ¿        V   ‡        d      l     t   p                                 	              
 å   ê  q    ô   @         §   †      Æ   |       ∏   @  å     «   ‡  ú     ÿ   0       Ë       *    
 ˙     œ       ‡  ê                             *      Ä     8      
     K           b     1     x  @   !     ì      0                   ù     0     ≠  †   P     æ  `   @     Õ             ÿ             ˆ                                       9             H             Y             r             à             è             ú             ±             ƒ             Ã             È             ˚                          !             5           A             P      
     _      0      vs6624.c vs6624_enum_mbus_code vs6624_formats vs6624_get_fmt vs6624_g_frame_interval vs6624_remove vs6624_s_stream vs6624_s_frame_interval vs6624_probe vs6624_ops vs6624_p1 vs6624_p2 vs6624_default vs6624_run_setup vs6624_ctrl_ops vs6624_probe.cold vs6624_s_ctrl vs6624_set_fmt vs6624_driver_init vs6624_driver vs6624_driver_exit __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 vs6624_id vs6624_core_ops vs6624_video_ops vs6624_pad_ops __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free __stack_chk_guard i2c_transfer_buffer_flags __const_udelay __stack_chk_fail __x86_indirect_thunk_edx devm_gpio_request_one msleep devm_kmalloc v4l2_i2c_subdev_init usleep_range_state _printk v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vs6624_id_device_table       $        1   $  ¡   $  ·   $  Ï   %  Ù   &    $    '  :  (  E  )  P  '  h  *  q  $  Ä  '  ‹  (    (  0  (  Z  (  h  '  â  *  ë  $  †  '  ≤  +  Õ    ﬁ  ,  Û  -    .              /  V  (  Ñ  (  ©  (  Ω  0  Œ     ˛  (       F  (  n     y  (  ∞  (  ¸  (  &  (  Q  (  |  (  Ê    Î  1  ˛  2       %  3  /     L  3  V     p  3  z     î  3  ´  4  º  '  ”  &  ·  &  ˝  *    $    '  l  (  z  '  €  *  ·  $    '  ü  '  +	  (  N	  (  †	  (  ≈	  (  Í	  (  
  (  /
  (  R
  (  ¯
  (  ;  (    (  l  *  È                   h                                                     $             1  &        $          5     6          8  @      L      \      d     p     t     »     –     ‘     8     .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rodata.str1.4 .rel.text.unlikely .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   p                    	   @       ‘  ò              )             ¿  Ä                   %   	   @       l                   /             @                     8             @  (                  4   	   @       å   P               E      2       h                   T      2       Ñ  T                 g             ÿ  *                  c   	   @       ‹         
         z                                 v   	   @       Ù                   â               
                  Ö   	   @       !                 î                a                  °             †  ƒ                  ù   	   @       $!  P               ©      0       d                   ≤              w                     ¬             x  (                                †  ∞     $         	              P  Å                               t!  ’                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              6/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/gfp.h \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  arch/x86/include/asm/irq.h \
    $(wildcard include/config/HAVE_KVM) \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/generated/asm/irq_regs.h \
  include/asm-generic/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/SPARSE_IRQ) \
    $(wildcard include/config/IRQ_DOMAIN) \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/kobject_ns.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  arch/x86/include/asm/hw_irq.h \
  include/linux/profile.h \
    $(wildcard include/config/PROFILING) \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/BPF_JIT) \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/sched.h \
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
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/rhashtable-types.h \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RE