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
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
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
  include/sound/core.h \
    $(wildcard include/config/SND_DYNAMIC_MINORS) \
    $(wildcard include/config/SND_MAX_CARDS) \
    $(wildcard include/config/SND_MAJOR) \
    $(wildcard include/config/SND_DEBUG) \
    $(wildcard include/config/SND_MIXER_OSS) \
    $(wildcard include/config/SND_OSSEMUL) \
    $(wildcard include/config/ISA_DMA_API) \
    $(wildcard include/config/SND_VERBOSE_PRINTK) \
    $(wildcard include/config/SND_DEBUG_VERBOSE) \
    $(wildcard include/config/GAMEPORT) \
  include/sound/pcm.h \
    $(wildcard include/config/SND_PCM_OSS) \
    $(wildcard include/config/SND_VERBOSE_PROCFS) \
    $(wildcard include/config/SND_PCM_XRUN_DEBUG) \
  include/sound/asound.h \
  include/uapi/sound/asound.h \
  include/sound/memalloc.h \
    $(wildcard include/config/GENERIC_ALLOCATOR) \
    $(wildcard include/config/SND_DMA_SGBUF) \
    $(wildcard include/config/HAS_DMA) \
  include/linux/dma-direction.h \
  include/sound/minors.h \
  include/linux/pm_qos.h \
    $(wildcard include/config/CPU_IDLE) \
  include/sound/pcm_oss.h \
    $(wildcard include/config/SND_PCM_OSS_PLUGINS) \
  drivers/media/pci/cx18/cx18-driver.h \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/pci.h \
    $(wildcard include/config/PCI_IOV) \
    $(wildcard include/config/PCIEAER) \
    $(wildcard include/config/PCIEPORTBUS) \
    $(wildcard include/config/PCIEASPM) \
    $(wildcard include/config/HOTPLUG_PCI_PCIE) \
    $(wildcard include/config/PCIE_PTM) \
    $(wildcard include/config/PCIE_DPC) \
    $(wildcard include/config/PCI_ATS) \
    $(wildcard include/config/PCI_PRI) \
    $(wildcard include/config/PCI_PASID) \
    $(wildcard include/config/PCI_P2PDMA) \
    $(wildcard include/config/PCI_DOMAINS_GENERIC) \
    $(wildcard include/config/PCI_DOMAINS) \
    $(wildcard include/config/PCI_QUIRKS) \
    $(wildcard include/config/ACPI_MCFG) \
    $(wildcard include/config/HOTPLUG_PCI) \
    $(wildcard include/config/EEH) \
  include/uapi/linux/pci.h \
  include/uapi/linux/pci_regs.h \
  include/linux/pci_ids.h \
  include/linux/dmapool.h \
  arch/x86/include/asm/pci.h \
    $(wildcard include/config/PCI_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/VMD) \
  arch/x86/include/asm/memtype.h \
  include/asm-generic/pci.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/DMA_API_DEBUG) \
    $(wildcard include/config/NEED_DMA_MAP_STATE) \
  include/linux/i2c-algo-bit.h \
  include/media/v4l2-ioctl.h \
  include/media/tuner.h \
  include/media/v4l2-mc.h \
  include/media/i2c/ir-kbd-i2c.h \
  include/media/rc-core.h \
    $(wildcard include/config/LIRC) \
  include/linux/kfifo.h \
  include/media/rc-map.h \
    $(wildcard include/config/IR_RC5_DECODER) \
    $(wildcard include/config/IR_JVC_DECODER) \
    $(wildcard include/config/IR_SONY_DECODER) \
    $(wildcard include/config/IR_NEC_DECODER) \
    $(wildcard include/config/IR_SANYO_DECODER) \
    $(wildcard include/config/IR_MCE_KBD_DECODER) \
    $(wildcard include/config/IR_RC6_DECODER) \
    $(wildcard include/config/IR_SHARP_DECODER) \
    $(wildcard include/config/IR_XMP_DECODER) \
    $(wildcard include/config/IR_IMON_DECODER) \
    $(wildcard include/config/IR_RCMM_DECODER) \
    $(wildcard include/config/MEDIA_CEC_RC) \
  include/linux/input.h \
  include/uapi/linux/input.h \
  include/uapi/linux/input-event-codes.h \
  include/uapi/linux/lirc.h \
  drivers/media/pci/cx18/cx18-mailbox.h \
  drivers/media/pci/cx18/cx18-av-core.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \
  drivers/media/pci/cx18/cx23418.h \
  include/media/drv-intf/cx2341x.h \
  include/media/demux.h \
  include/uapi/linux/dvb/dmx.h \
  include/media/dmxdev.h \
  include/media/dvbdev.h \
    $(wildcard include/config/DVB_MAX_ADAPTERS) \
    $(wildcard include/config/MEDIA_CONTROLLER_DVB) \
    $(wildcard include/config/MEDIA_ATTACH) \
  include/media/dvb_ringbuffer.h \
  include/media/dvb_vb2.h \
    $(wildcard include/config/DVB_MMAP) \
  include/media/videobuf2-core.h \
  include/linux/dma-buf.h \
    $(wildcard include/config/DMABUF_SYSFS_STATS) \
  include/linux/iosys-map.h \
  include/linux/file.h \
  include/linux/dma-fence.h \
  include/media/frame_vector.h \
  include/media/videobuf2-dma-contig.h \
  include/media/videobuf2-v4l2.h \
  include/media/videobuf2-vmalloc.h \
  include/media/dvb_demux.h \
  include/media/dvb_frontend.h \
  include/uapi/linux/dvb/frontend.h \
  include/media/dvb_net.h \
    $(wildcard include/config/DVB_NET) \
  include/linux/netdevice.h \
    $(wildcard include/config/DCB) \
    $(wildcard include/config/HYPERV_NET) \
    $(wildcard include/config/WLAN) \
    $(wildcard include/config/AX25) \
    $(wildcard include/config/MAC80211_MESH) \
    $(wildcard include/config/NET_IPIP) \
    $(wildcard include/config/NET_IPGRE) \
    $(wildcard include/config/IPV6_SIT) \
    $(wildcard include/config/IPV6_TUNNEL) \
    $(wildcard include/config/RPS) \
    $(wildcard include/config/NETPOLL) \
    $(wildcard include/config/XPS) \
    $(wildcard include/config/XDP_SOCKETS) \
    $(wildcard include/config/BQL) \
    $(wildcard include/config/RFS_ACCEL) \
    $(wildcard include/config/FCOE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/NET_POLL_CONTROLLER) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/NET_L3_MASTER_DEV) \
    $(wildcard include/config/IPV6) \
    $(wildcard include/config/TLS_DEVICE) \
    $(wildcard include/config/VLAN_8021Q) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/DECNET) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/MCTP) \
    $(wildcard include/config/NET_CLS_ACT) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/NET_SCHED) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/net_namespace.h \
    $(wildcard include/config/NF_CONNTRACK) \
    $(wildcard include/config/IEEE802154_6LOWPAN) \
    $(wildcard include/config/IP_SCTP) \
    $(wildcard include/config/NETFILTER) \
    $(wildcard include/config/NF_TABLES) \
    $(wildcard include/config/WEXT_CORE) \
    $(wildcard include/config/XFRM) \
    $(wildcard include/config/IP_VS) \
    $(wildcard include/config/MPLS) \
    $(wildcard include/config/CAN) \
    $(wildcard include/config/CRYPTO_USER) \
    $(wildcard include/config/SMC) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/NET_NS_REFCNT_TRACKER) \
  include/net/flow.h \
  include/linux/socket.h \
  arch/x86/include/generated/uapi/asm/socket.h \
  include/uapi/asm-generic/socket.h \
  arch/x86/include/generated/uapi/asm/sockios.h \
  include/uapi/asm-generic/sockios.h \
  include/uapi/linux/sockios.h \
  include/uapi/linux/socket.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/uapi/linux/libc-compat.h \
  include/net/flow_dissector.h \
  include/linux/siphash.h G¸Aêü¥p]ÛZs◊–*‰ûî/AÿP3ın€±*v˚¶
+‘;˙X”¸ñ∑ol4√ Å:VŸ*ÄÅπ(äwzËß˛Q·)ãX9<≥ˇ
[:2”Iç‘‡W—nrVKç∏“R£	´≈Œõ±€òÇ+‰PïIﬁ´Æ˚â|ª’?éü d∑¡««ˆü Õ(¸,7“SC∂L7h‘ß≥w≠;º´%ı⁄#4á¨ªÖyøΩ]⁄dÎT9\)ﬁ+.ùˆÛßnK¶œÀËMãOÌ4†©$Ç2T…§yBKå6ò@≈·≤éÄ;ÿ[§¢Ô√ÿ~.Ã˜m°ı5›ÆŒ«p◊˚í„ﬂ∫á÷!ÒåFï±Ïsf Ñ;X8ÏgA°[à~±µ7¯ûvµÛ+û äøõÔˇ 9º–Å„∆Áf†¨Ò]kOY0Ì Vπ	~£±∑∏ ß⁄iŒ~ﬂ_£5Èóç¸ø*Àˆ˙ŸM–∑ÄØ,v*f
˙RŒ°˝±›∏•B
k±öÃVˆ(‰’ÑÂ=∫∏]÷Õ
ÒÂàpPlÈ}∫—ñ5exû∑Ôfá≥é|™\Z\?_∏JV`u4»‰c∑q+Iãã)‡8ÊüÕs8ë7y≤wÁ€^ÚnCÏQùrx*%˛≠í[¸ÂÂ† ≥?E.#˚é~râ9hOQg°!fMû…‘√(ÿ	µ«¬‚ﬁö˙/˜Aß§ôx¢™_˝Ôä+xPüvûoÍüùl©Ì„R˚‡÷ˇåZP’ØYít™´ﬂ€ñ~∫Ω!•®ˇÅÚ¿kx2:1_éû5¯”®ˆ|√›[{2é“Z#˛óˇ+M88‚˛Äí‡äîæ-‘µ$ w∆ﬂìIb-$pjÏ…°¡ÖüúyjJo€#pûI,ëÛ∆}·gûù<ÑÕ≠Í\YfY#H”ΩÏ&UuDhIÜÈd56õÏÄ¬ñ∏ oi(}OZ	h≈¶¢Dv˝§A~‡ eLM!^6Ca.ò˚V≤J)∆^D†ïmŸÇ:ÀÁ„’=‡°k.#2◊∏'hÁÃm/+k:ê›'yeuıìÌ” BËëâAÈ"µ)MbÙß∑›ó|^ƒT*÷ÀºÂˇ¿} àúSeDF“}µ/#ÓQÍyó≥«¥©+ÏΩ.Ø©ó´˚R5gº¸£ëÑuD¿R»˜Éf¨∑G‡2”úç©oıç∏M¿éè„d~¿'œ∑ôL´§Á4⁄Ö>ˇz‹ë˙Bj˛†–~9Ogï.W√4ò£e©•]wõØœöL≥»ÑáëÁ(™>>≤∆Ëô0+ËÌîdØEÜÆ“0ﬁ¶p;Ü≈!–w^}E)M5"√Ã-Ÿ1R…èøw",~EÓŒ1‹œA)∂„\)Km3x÷ì»|Í>hfØdº%¡Ì…Çfo*+Îπ˛Û˛y’’–µ∆¬UdNLDn'Qâ¬Ë˙Ü∑*Æ•.ªnÊ¡õCâïHà¨ˆÁ7âi&7NØ„÷ÜúX´hVàùœáF•âá‡ﬂ÷€N± §+GhE~ï7À*E¡ ‘ÆAÛl+3s2–}èÉ¯n/"’çÓö›¶sk=
ãµÃQŸ„@‘1¥U—UG${¶õ@a®81	<Ãµ)Ø›≠9‚Ÿ˘H7PÓ˜Òy~$ÎœO∂˛úÉ0;∏˛aÏÔÚÆ])ÍÉ@Ä≤µg¬IA	1Nw_Ê}?fˆë:~nëÕµbrˆ|!eôMêûSª=ÆìSr-1è)]#@§°≥&%vz;ƒ3o˚≠¡≠ZÃ5ÏÒu˝‰⁄∞Ω…4ÁÉ|÷"Æ±’·r≤Ößbiz9Á"T±*ﬂb%m(πÇœs“z woÍÚ]ˆ •≤]Çj”JìjK≈UA∑Â≠ÁZL Ô≠û°sA¶Ao%ïcÓ†n%¿ØL+∞"â∫l	{÷?ô!ΩgGgõ«—6o”‹ÜÇŒäºOûÛü—Õ¡Œ¶zπ˙∑ P∏ò_¡'öSﬂ•¡Ì≤L e>gbˇâB√p¥°NÄŸ˝ZO€CA¬sz	∆C≈SöÍqb≥óòﬂ—∂>Z8xpJ⁄ﬁUÜhÇñ¬…òb^ƒ'3Àùø(ﬂSG/ss≈)å$ƒSÉ„…˝PÕG•‰4cë] ß+Ïoi@6ÇÃJ;ÿz†©g)Ò÷yÃ∑oON◊»67èóÆqÊˆu›©Ω>‰r≤lÑ;XäDc!’z“Ã∂˙^¸
º†ÔR™µ3CYPŒ”˙ü¶M•0$Á∫ı-)ÿ%¡ÍT_.ì´ÛÒ∞ÇÓ◊öPíDYÎ{∏ôÌCVåÛ‘¬8|äP92Ø¨±A(é∏˘1ÃÕ]Œâb’á,ç∏|⁄;>è–íÌägÿsa∂≥0êD'“Ï4ªCX˜Økû\¢tûUWú˚Y‹oÙ_¡·Y∂"'a†V∫~ÃZ—7ëŒ0]ì>∆èÌH≈j°ΩÎ,,pë˝≤≈G<C‰wÔ®ÀNäÙ‹˜∑7Z¥§Å%)õ0´
Ù 3û√ÙH·Ö÷áú/—(.Ã∑	êRt8J/ƒ¨Î÷)iæ∑?7Gq5°˝Åv8Ω≈Lì◊pïv∞›AªÆ‘qs.âAäÁ ¡sˆ™*∆*π‹˙⁄ÜÖ‚Rî.˝»›uà&µlNıEkä{5w¡ﬂÜ|ª‚ÏÒc!¶i∏Í 8ó,ïÁö˝`™UNvÎ›ÃMÃ-ÌÁÃ¥É>·≥/_!óE÷√&„•ÿä¯|ˇë≤‚®Ë_hå`É$⁄6Y{¨@2âE,¡Ø+[Ô;/ìx©zUm®Ωd ù⁄ÛBâ(≤jÏódcVÅkyqÉwH\@Í¶øÈÕ q!˛£SÒÆæŸßàç‰›◊D›Hw<iZÓø“¿sÂùb≤&˘A“5÷ˇ—Ú◊Ç€ Aj˜â±n'çG§{q Gë˛/Lô©©éå"˛∏¨JC5çÕì;'Øgπ◊=û\~q$€úx,gb…ü˛£~ˇ1ΩÄ:N/ûﬂT6öôZ∂WŒ<±•\Ë,≤6€≤O/´"ê\Kü¡ß±ò˜ÉFä|wE)¶Ö£‚ÈÈ”Ev4aDä\Ùß§Ç·:\d«2Lœ ¯mH˚©˛o4B°÷ïøUb„ g]@T^§“jé$4¶JÂëÃ€ﬁH Û∑úµôéJ}œÉ(ø∞‹ñÏ≥‡.Ûﬁ>äHQ4ŒµÓ~∫ƒqù,÷÷$ˇ°$Vvî(PO-‹.πQÈæÔ5S è·à^¬.∑Bú3Út„Nâ|¢X†D`˚¸øLˇ&@≤8œ∑WÂåµÿ√ƒ[ËcÚÏ≤"(xﬁ∆√Á<|[5k-…ÚÔÙ∂ (Ye—GJT†B™~,˜›îÊÄ™òänN:≠k∑ç¨Àº°ï¢œViÁÏ¬˜˚û_	háÕ.=fíWçèÜ≈“5ıÕ∆ô˛œhµ<d∞kÈ¿«œVÁ≠IÛ}öÑ¸◊?>¸ø¶Ùπ¿(R∑Uÿìs˙sè‚ˇ…kNXÈrßk˚’Qî•ÊÖ£Áå∑Ìù£ë¢j>«rËëI√d◊ˇ‰÷ß`ÚÄ/êÙƒºgÕïø,cyvèEH™ÎÔ†lˆ÷Í¸˝”`»Q¬K'-c{”÷ı°h∞”¿:‹abOv›∂xÏuX©Î@w„Àa◊f˜(P5[J I∆=∫B˚çJ#Æ∫O#•ÁÁ¶ÓGÒèzè2ãæ5{aocxvÉ¸⁄qkÙ>J3ç˛=TÎ„yô∏_ÖtÃ
∏‘≤‚¯‘ÛbS^π™ÓHÀ}i˜uKN¸_‚q}£$ÅË&õΩ[”‹‚˙WG‚EÎvÑ›K†ˆÜpVùÏ∑ˆl/`'⁄XêÛ€W!dRã^k[ø‘úR5@˜á„h{ız†9Y–(Ò(´™ˇë¨ÚZÆ‘ÔSÀ-±úCv,s]xÕ™Ú)}»Ü´ºe<1-Ò◊Å˝˘\OÄHÒ»âG©%1@{π€éU»r5Wøìv6+t“ò¯∫íØ6K+p∞°oÇòidΩ/q–DG» ã”»Ækﬁ£≈qY7ÏÛ¿π4¨œ“"‡‹è&àƒùBÖ®[æÂàDG*·˘KYÇ‡6AÓºâËß–Úï–ºí3Ü£ƒO§?¡ﬂ0π»ëu!/–æò‘I_¨≈¥ ø@k‡MïGŸJc––F˚;Fù(_≈éoFﬂ$⁄ﬂºy¿‚≈` )ã∫ m¥y¸œ+Ä≈iàX†¢_”F«ãòWÊrÖœ{∫T•∑¿JIßO’–Ã2Û:)£◊
oqΩ|€9H,IÔ÷ÏCzÉlŒhê±ëßuﬁƒjV˚œk6^HØÑ1à-dú{µh{ªª‡òMúafµWn{ÈçÎorÈ*≠vÔí£ò©GÙlxoÆ™Ï*Ó?{pÈ	É^%õê$-"Œj(Ï™…Çt·o©ôå⁄Í^;¿Í§∞,n.…Ó–™ôãÀ#ê&M ‡æﬂy{cuÉsù`¡ü∞X‚Ÿ£\`Ø·˙ì ›Ó<ª‚‹À…∂ÙÑ0ñ]¿Pj.ØYÒUo ˛k}üvüé:·DT≠2c‹‰ röÄsÙ	`›] §oà˝ﬂÏæU8ΩM¨üÕ√‘1TåaàÈ~'¯8IÌíQ7≠í¡ájóºùÿ·V:ÊÏiâõ¡â•ŒkPöÒH˘=ﬁ˛Uy†‹	∫dôπ¿Ω§1<Lå∞≥∞i›≤∞y$Àg¥‡¯Ω€ºg@I_≤Ú5ıè‘¿öîŸ:µ£¿∑ÉÛÏÚ2K´	›ö£¡jìLíØñKÙΩ£≥¢„ˇùÖkV/ÖÏäæ÷æå¡Ñt€íˇ€Y1‚–BYPÓE¿^Z±MlèÌ“Ò)ΩW–®∆©LØvpÏàG'(I˙IZ›ÚT‡1|
Æ l¿W»$I&µæ,
»K¯@ã|),î@=Ω„´-Öﬂi’Z≤F(ç’C{A¬ˆr≠ê¨œzCÒÊÆAßÜNl˙‰ï∏p¯~q©∫õ\?N¢~Î€d˙¬NO„1Ïµ,a¸≈©Ãõ˜í``∏K8TÔ}fÒi˛E‡4«´†V9ΩLsÚeèÁˆ¶Sm%…∂?xE_¬7¡6S_%º¨PÆùpõç?.]FPÍVVõlüOıåq˚öL*Pq‚;ºb°ë»nÌ“æì-⁄Ñ™d«A Ê«π<◊Ò“Î¯ŸqoÂÕZ	0hÃw0#ä £^‡ÒÂ“¢ŸèΩ„πØxw4ﬁ˙SzP0¯âféª∆Äæ§Ñ•πá*.ﬁºêlà`π= ©∑<õu[è|]µiÈû€Î»!)ª∫{mrÀy´+@Ìëéáÿ:Vˇı,wX”JÊΩÄM™3V&YêÔ;Ôye´)òyë!~q»ÏGy’À≠í”Àd.7‚ ª›F¡¿s˝~ù1só∞ö>´ç·Ê de/linux/ptr_ring.h \
  include/linux/seq_file_net.h \
  include/net/netprio_cgroup.h \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/SECURITY_NETWORK) \
  include/linux/security.h \
    $(wildcard include/config/SECURITY_INFINIBAND) \
    $(wildcard include/config/SECURITY_NETWORK_XFRM) \
    $(wildcard include/config/SECURITY_PATH) \
    $(wildcard include/config/SECURITYFS) \
  include/linux/kernel_read_file.h \
  include/uapi/linux/netlink.h \
  include/uapi/linux/netdevice.h \
  include/uapi/linux/if.h \
  include/uapi/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
  include/uapi/linux/if_bonding.h \
  include/uapi/linux/pkt_cls.h \
  include/uapi/linux/pkt_sched.h \
  include/linux/hashtable.h \
  include/linux/inetdevice.h \
    $(wildcard include/config/INET) \
  include/linux/ip.h \
  include/uapi/linux/ip.h \
  include/linux/rtnetlink.h \
    $(wildcard include/config/NET_INGRESS) \
    $(wildcard include/config/NET_EGRESS) \
  include/uapi/linux/rtnetlink.h \
  include/uapi/linux/if_addr.h \
  include/linux/etherdevice.h \
  include/linux/crc32.h \
  include/linux/bitrev.h \
    $(wildcard include/config/HAVE_ARCH_BITREVERSE) \
  arch/x86/include/generated/asm/unaligned.h \
  include/asm-generic/unaligned.h \
  include/linux/unaligned/packed_struct.h \
  include/media/videobuf-core.h \
  include/media/videobuf-vmalloc.h \
  drivers/media/pci/cx18/cx18-queue.h \
  drivers/media/pci/cx18/cx18-streams.h \
  drivers/media/pci/cx18/cx18-fileops.h \
  drivers/media/pci/cx18/cx18-alsa.h \
  drivers/media/pci/cx18/cx18-alsa-pcm.h \

drivers/media/pci/cx18/cx18-alsa-pcm.o: $(deps_drivers/media/pci/cx18/cx18-alsa-pcm.o)

$(deps_drivers/media/pci/cx18/cx18-alsa-pcm.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ›Ë∫õı\·)M_1~+j:p¡ƒäó«uJßq%›âU È`¢MÏUjÛ‰FØUYÚäÜ#ª41∏q§u≤ÑÌo„≈á∞a—Ê¸Iæ9:¿˘◊÷ßOf ˝P>0À<Ã¢ÓjÕ'S–>˘7%˜õ‰Å∂x(Çÿb%ü}•¯ÚW˝p!Û!HøEbñı‹âL*Ç.Îâ´ëS÷ÆçQ^∂Óa¿ôı&+Ó*Å@{S∑˙ºhP≈{|ÅYe¢•|&ﬁWF\Ë≈˜Úœ´ƒ<b$˝y¢ç˛cxl>ì<∆»Öë|Ô¥^%”ï~iO1Cx’ûéÃ˜í_…àÂ«∫>äÕÿ◊∫_Ö⁄ÉGÛg]ˇˆèrw≤à„‘Œ2\fì‘•éÂ»Lsü?*ç',Èm(KLÜîô5d:≥ﬁEÆnˇÔ⁄ıÿ5≤ß	F¬+G=t0y∑˚π√jTÏWæıd±™˚]ü*=wqL*Y‘[/mNÎ|ƒö[ôµ•!aXd<™÷¥Pr·Tèoÿ∞Á¸‹˝ø|?–ÿŸ0[o=Y†K6Ê>¡«}§4n˘9µ9⁄≠ÌÎ´Ìã…vı„ã™˙Õï¥°á°¡ª’1+%Ô]¥Œúó(êPu_7ﬁ9•ET4ëG‰áó|¢A≤˚ü` °õ÷
BÄìä∞9>µ≈™ˆCM≠O¬$(9	Ø>7Ü˙9ûÁå[û˚$1üPá<ÑQ-≤{ç¥ß2TO¯W±7âZÁ3µ*≤3'‰¨@∫TZH¨ìwFIπÄæ∏txËsøtªº+Jƒ∆ˇªÁ+ì√6Êäé^∂-[ÿ·c]»Öë‡˛¡@;ªaÙÑljgÌ˙>Ëfñ?]í≠›=‡¿∞–h¬◊¸ÒÔÏÓ
ó§h~¯[BD"ﬂiàuzvb◊X>dòBÒ˛Z)ímTƒb¯\Ò')®Ë∫X˙e'üJ{∞√U›ªIÿ%∏SZíÄ’”¬M¶eºLÁÕ_ê÷sòqª£;IjÉﬂ2§$Öñ∞