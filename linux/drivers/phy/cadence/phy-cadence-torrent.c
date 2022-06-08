mempolicy.h \
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
  include/linux/siphash.h \
    $(wildcard include/config/HAVE_EFFICIENT_UNALIGNED_ACCESS) \
  include/uapi/linux/if_ether.h \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/XFRM_STATISTICS) \
    $(wildcard include/config/TLS) \
    $(wildcard include/config/MPTCP) \
  include/net/snmp.h \
  include/uapi/linux/snmp.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/IP_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_CLASSID) \
    $(wildcard include/config/IP_MROUTE) \
    $(wildcard include/config/IP_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_MULTIPATH) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/IPV6_MULTIPLE_TABLES) \
    $(wildcard include/config/IPV6_SUBTREES) \
    $(wildcard include/config/IPV6_MROUTE) \
    $(wildcard include/config/IPV6_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/NF_DEFRAG_IPV6) \
  include/net/dst_ops.h \
  include/uapi/linux/icmpv6.h \
  include/net/netns/nexthop.h \
  include/net/netns/ieee802154_6lowpan.h \
  include/net/netns/sctp.h \
  include/net/netns/netfilter.h \
    $(wildcard include/config/NETFILTER_FAMILY_ARP) \
    $(wildcard include/config/NETFILTER_FAMILY_BRIDGE) \
    $(wildcard include/config/NF_DEFRAG_IPV4) \
  include/linux/netfilter_defs.h \
  include/uapi/linux/netfilter.h \
  include/linux/in.h \
  include/uapi/linux/in.h \
  include/net/netns/conntrack.h \
    $(wildcard include/config/NF_CT_PROTO_DCCP) \
    $(wildcard include/config/NF_CT_PROTO_SCTP) \
    $(wildcard include/config/NF_FLOW_TABLE) \
    $(wildcard include/config/NF_CT_PROTO_GRE) \
    $(wildcard include/config/NF_CONNTRACK_EVENTS) \
    $(wildcard include/config/NF_CONNTRACK_LABELS) \
  include/linux/list_nulls.h \
  include/linux/netfilter/nf_conntrack_tcp.h \
  include/uapi/linux/netfilter/nf_conntrack_tcp.h \
  include/linux/netfilter/nf_conntrack_dccp.h \
  include/uapi/linux/netfilter/nf_conntrack_tuple_common.h \
  include/linux/netfilter/nf_conntrack_common.h \
  include/uapi/linux/netfilter/nf_conntrack_common.h \
  include/linux/netfilter/nf_conntrack_sctp.h \
  include/uapi/linux/netfilter/nf_conntrack_sctp.h \
  include/net/netns/nftables.h \
  include/net/netns/xfrm.h \
  include/uapi/linux/xfrm.h \
  include/net/netns/mpls.h \
  include/net/netns/can.h \
  include/net/netns/xdp.h \
  include/net/netns/smc.h \
  include/net/netns/bpf.h \
  include/net/netns/mctp.h \
  include/net/net_trackers.h \
  include/linux/ref_tracker.h \
    $(wildcard include/config/REF_TRACKER) \
  include/linux/skbuff.h \
    $(wildcard include/config/BRIDGE_NETFILTER) \
    $(wildcard include/config/NET_TC_SKB_EXT) \
    $(wildcard include/config/NET_SOCK_MSG) \
    $(wildcard include/config/SKB_EXTENSIONS) \
    $(wildcard include/config/IPV6_NDISC_NODETYPE) \
    $(wildcard include/config/NET_SWITCHDEV) \
    $(wildcard include/config/NET_REDIRECT) \
    $(wildcard include/config/NETFILTER_SKIP_EGRESS) \
    $(wildcard include/config/NET_RX_BUSY_POLL) \
    $(wildcard include/config/NETWORK_SECMARK) \
    $(wildcard include/config/PAGE_POOL) \
    $(wildcard include/config/NETWORK_PHY_TIMESTAMPING) \
    $(wildcard include/config/MCTP_FLOWS) \
    $(wildcard include/config/NETFILTER_XT_TARGET_TRACE) \
  include/linux/net.h \
  include/linux/random.h \
    $(wildcard include/config/VMGENID) \
    $(wildcard include/config/ARCH_RANDOM) \
  include/linux/once.h \
  include/uapi/linux/random.h \
  include/linux/prandom.h \
  arch/x86/include/asm/archrandom.h \
  include/linux/sockptr.h \
  include/uapi/linux/net.h \
  include/linux/textsearch.h \
  include/net/checksum.h \
  arch/x86/include/asm/checksum.h \
  arch/x86/include/asm/checksum_32.h \
  include/linux/netdev_features.h \
  include/linux/sched/clock.h \
    $(wildcard include/config/HAVE_UNSTABLE_SCHED_CLOCK) \
  include/linux/splice.h \
  include/linux/pipe_fs_i.h \
  include/uapi/linux/if_packet.h \
  include/net/page_pool.h \
    $(wildcard include/config/PAGE_POOL_STATS) \
  include/linux/ptr_ring.h \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   cmd_drivers/media/pci/cx18/cx18.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18.o drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   cmd_drivers/media/pci/cx18/cx18.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18.o drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ELF                      §"      4     (            GNU  ¿       ¿          Ë¸ˇˇˇÖ¿t?Sâ√ãÄà  Ö¿tãÖ“t
«Ç$&      Ë¸ˇˇˇ«Éà      «Éå      [√ç¥&    √ç¥&    Ë¸ˇˇˇUWVâ∆Sâ˜ÉÏd°    âD$1¿É«ÑŒ   ãñ  Ö“Ñ˝   ãÜ,&  Ö¿Öç   ãF«$    1…∫ˇˇˇˇTj à   h    Ë¸ˇˇˇÉƒÖ¿Öt   °   π   ∫¿  ã,$Ë¸ˇˇˇâ√Ö¿ÑZ   â8π   ∫
   âhâÖà  «Öå      ãxã(çGË¸ˇˇˇçG$ˇu¯É«Dh   j PË¸ˇˇˇˇudˇu¯h@  jPWË¸ˇˇˇâÿÉƒ$Ë¸ˇˇˇÖ¿Ö   âû,&  ã$Ë¸ˇˇˇÖ¿Ö®   ˆ    u-ãD$d+    uÉƒ1¿[^_]√çv ˆ    t›È¸ˇˇˇË¸ˇˇˇÈ‰   fêfêfêfêË¸ˇˇˇã@«@    «@    1¿√ç¥&    Ë¸ˇˇˇ1¿√ç¥&    êË¸ˇˇˇVSãpç^âÿË¸ˇˇˇãvâ¬âÿË¸ˇˇˇâ[^√ç¥&    çt& Ë¸ˇˇˇUWVSâ√ÉÏã5   â$â ÖˆÖ2  ãhÖÌÑ¬   ã}|ÖˇÑ∑   ãw`¡ÓÑù   9ÚÇ  ãè4  Ö…Ñ7  â–1“˜ˆãSâD$ãGTâD$â–Ø∆»â—âD$ãD$–â¬ãD$9¬svØt$ã$ãD$âÒË¸ˇˇˇâËË¸ˇˇˇãCãt$âCãOTâ¬) 9»C¬sâsâÒâCãGL9∆Éû   ÉƒâË[^_]È¸ˇˇˇã-   ÖÌÖ  Éƒ[^_]√ç¥&    çv â¬âL$ãD$) â—ã$ØŒâL$Ë¸ˇˇˇãD$ãL$+L$ã$»ãL$Ø∆ â¡ãá4  Ë¸ˇˇˇÈTˇˇˇç¥&    ã   Ö€Ö   Éƒ[^_]√ç¥&    çv )¡âËâKË¸ˇˇˇâËÉƒ[^_]È¸ˇˇˇç∂    ã   Ö…ÑPˇˇˇÈá  ç¥&    ç∂    Ë¸ˇˇˇWVSãpãçÉ   çªT  Ë¸ˇˇˇ1“â¯Ë¸ˇˇˇÄ£®  Ôâ¯Ë¸ˇˇˇ«É(&      ã   Ë¸ˇˇˇ1¿[^_√ç¥&    ç¥&    Ë¸ˇˇˇUâ≈WVSÉÏlãu|d°    âD$hãEãâ$çC¯âD$â«çÉ   Ë¸ˇˇˇãÉ<&  ãìî  â|$dçHâD$\çD$âã<&  âT$`Ë¸ˇˇˇÖ¿Ö"  â¬ãÉ®  Ñ¿xçÉT  âD$∫´®  s6ã$âT$ã    Ë¸ˇˇˇãT$ãD$hd+    ÖÚ   Éƒlâ–[^_]√çt& â–çæ¯   π   ∫   Û´â«Ü¯    «Ü¸      «Ü       «Ü  Ä   «Ü  Äª  «Ü  Äª  «Ü     «Ü     «Ü  ® @«Ü!  1  «Ü(  b   Ë¸ˇˇˇã<$ãL$âoâé   «É(&      Äã®  ãD$Ë¸ˇˇˇâ$ã   Ë¸ˇˇˇã$Èˇˇˇã$ã    Ë¸ˇˇˇ∫ˇˇˇÈ˝˛ˇˇË¸ˇˇˇç¥&    Ë¸ˇˇˇU1…∫   Wâ«VSÉÏã/d°    âD$1¿ãGTjj Ë¸ˇˇˇâ∆ÉƒÖ¿Ö‘  «G    ã$πÄ   ∫   Ë¸ˇˇˇ1…∫   j j ãD$Ë¸ˇˇˇã\$∫P   «C    É√Xâª`  ã}dâ¯Ë¸ˇˇˇâ¡XZÉ˘ˇÑ   É˘PtÉ¡É˘Pá¿  â˙âÿË¸ˇˇˇãD$d+    u
Éƒâ[^_]√Ë¸ˇˇˇSã@@Ö¿uh    h    Ë¸ˇˇˇXZÎ6ãê$&  Ö“uÉ¿h    Ph0   Ë¸ˇˇˇÉƒÎããBË¸ˇˇˇ«É$&      1¿[√hh  Ë¸ˇˇˇ∫    ∏   Ë¸ˇˇˇ1…1“h    Ë¸ˇˇˇ«        hå  Ë¸ˇˇˇÉƒ√h(   h    h`   Ë¸ˇˇˇÉƒÈb  É∆Ph   Vhl  Ë¸ˇˇˇÉƒã$Ö¿tË¸ˇˇˇâÿË¸ˇˇˇh(   Vh  Ë¸ˇˇˇÉƒÈb  É∆jÙh   Vh  Ë¸ˇˇˇÉƒÎªÉ∆Ph   Vh–   Ë¸ˇˇˇÉƒÎµÉ∆h(   Vhò   Ë¸ˇˇˇÉƒÈb  «Ü,&      É∆Ph   Vh®  Ë¸ˇˇˇÉƒÈaˇˇˇh(   h    Ë¸ˇˇˇY[Èb  h(   h    h‡  Ë¸ˇˇˇÉƒÈb  hd   hd   hp  Ë¸ˇˇˇÉƒÈÍ  hd   hL  Ë¸ˇˇˇ^_ÈÍ  QâL$ˇt$Phd   h¥  Ë¸ˇˇˇãkãT$ÉƒÖÌtDã}|ÖˇÖ3  É=    ÑÍ  hd   h$  Ë¸ˇˇˇXZÈÍ  hd   hú  Ë¸ˇˇˇXZÈÍ  É=    ÑÍ  hd   h¸  Ë¸ˇˇˇY[ÈÍ  ∏H   Ë¸ˇˇˇ∏@   Ë¸ˇˇˇÉ≈PhP   Uh–  Ë¸ˇˇˇÉƒÈp  Ë¸ˇˇˇhH  Ë¸ˇˇˇX1¿«    P   √           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   ∞              †  ¿      –                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P       †  ¿  –     ∞    ¿  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           §ˇ     ¿           §ˇ    ◊  è  s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         Òˇ       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 â            ò   U   ?     ß            æ           ’      #        =          O   Y       ®        4           B  8       
 T           Òˇd  †       y  ¿       é  –  %     ä          £    ¿     s   d       
 ƒ  ∞  R     ﬂ    ©    ˘  Ä   @    
   ƒ  0     +  H       
 6  @       
 A  P       
 L  ª   -     e  Ë        Ç          î  ¿   
    
 ™             ∂             ƒ             –             Ÿ             Î             ˝               U   ?                  !             '             >             U             c             n           z             ò             †             ∂             «             œ             ‹             Á             ˜             	                          9             X             u             ì             õ             Ø             Ω                           ·           Ò     £                 $             2  ¿  –     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q     ç     ´   6  ∞   I  ª     ¿   ]  “   @  ‹     Ë   	  ˚       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  Ç  W  ä    è  G  î    °  B  ¡  B  —  B  ‡  ?  Ï  N    B          ô  =  †  S  ‹  F  ‚    Í      =  @  =  R    Z    x  F  Ü  Y  í    ü    ±  B     J  ”  P  ‚  M  ¯  U    B  #  9  C  J  m  L  ©  U  ∏  9  U  Q  o  X  Ä  D  è  U  ¶  U  µ  G  ¡  B  …  	  Ÿ  9  Ï  7  ˘          K  &  O  I  H  V    g    p  R  {  9  å  G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  á     å   E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     É     à   E  ï     õ     †   E  ®     ª     ¡     ∆   E  ”     ÿ     ›   E  ‰     È     Ó   	  Û     ¯   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    Ä  E  á    å    ë    ñ  E  ù    £    ™    Ø    ¥    π  E  ¿    ≈       A  œ    ‘  A  ›    „    Ë  E         B          E     T       Ä     Ñ     î     ò     †                                                    $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `   ê                 .   	   @       h  à              <               î                  8   	   @         à               K             Ñ  Ù                 G   	   @       x  H              ^             x	                    Z   	   @       ¿   (               m             †	                      i   	   @       Ë   (      
         u      2       l
  	                à             u  (                  Ñ   	   @       !  P               ï      2       ù  +                 §             »  ˇ                  ±             »  (                  ≠   	   @       `!  @               Ω                                 π   	   @       †!                 »             ¸                     Œ             ¸                    ”      0       ¸  &                 ‹              "                                   $  ‡     5         	                b                               ∏!  Ï                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ELF                      §"      4     (            GNU  ¿       ¿          Ë¸ˇˇˇÖ¿t?Sâ√ãÄà  Ö¿tãÖ“t
«Ç$&      Ë¸ˇˇˇ«Éà      «Éå      [√ç¥&    √ç¥&    Ë¸ˇˇˇUWVâ∆Sâ˜ÉÏd°    âD$1¿É«ÑŒ   ãñ  Ö“Ñ˝   ãÜ,&  Ö¿Öç   ãF«$    1…∫ˇˇˇˇTj à   h    Ë¸ˇˇˇÉƒÖ¿Öt   °   π   ∫¿  ã,$Ë¸ˇˇˇâ√Ö¿ÑZ   â8π   ∫
   âhâÖà  «Öå      ãxã(çGË¸ˇˇˇçG$ˇu¯É«Dh   j PË¸ˇˇˇˇudˇu¯h@  jPWË¸ˇˇˇâÿÉƒ$Ë¸ˇˇˇÖ¿Ö   âû,&  ã$Ë¸ˇˇˇÖ¿Ö®   ˆ    u-ãD$d+    uÉƒ1¿[^_]√çv ˆ    t›È¸ˇˇˇË¸ˇˇˇÈ‰   fêfêfêfêË¸ˇˇˇã@«@    «@    1¿√ç¥&    Ë¸ˇˇˇ1¿√ç¥&    êË¸ˇˇˇVSãpç^âÿË¸ˇˇˇãvâ¬âÿË¸ˇˇˇâ[^√ç¥&    çt& Ë¸ˇˇˇUWVSâ√ÉÏã5   â$â ÖˆÖ2  ãhÖÌÑ¬   ã}|ÖˇÑ∑   ãw`¡ÓÑù   9ÚÇ  ãè4  Ö…Ñ7  â–1“˜ˆãSâD$ãGTâD$â–Ø∆»â—âD$ãD$–â¬ãD$9¬svØt$ã$ãD$âÒË¸ˇˇˇâËË¸ˇˇˇãCãt$âCãOTâ¬) 9»C¬sâsâÒâCãGL9∆Éû   ÉƒâË[^_]È¸ˇˇˇã-   ÖÌÖ  Éƒ[^_]√ç¥&    çv â¬âL$ãD$) â—ã$ØŒâL$Ë¸ˇˇˇãD$ãL$+L$ã$»ãL$Ø∆ â¡ãá4  Ë¸ˇˇˇÈTˇˇˇç¥&    ã   Ö€Ö   Éƒ[^_]√ç¥&    çv )¡âËâKË¸ˇˇˇâËÉƒ[^_]È¸ˇˇˇç∂    ã   Ö…ÑPˇˇˇÈá  ç¥&    ç∂    Ë¸ˇˇˇWVSãpãçÉ   çªT  Ë¸ˇˇˇ1“â¯Ë¸ˇˇˇÄ£®  Ôâ¯Ë¸ˇˇˇ«É(&      ã   Ë¸ˇˇˇ1¿[^_√ç¥&    ç¥&    Ë¸ˇˇˇUâ≈WVSÉÏlãu|d°    âD$hãEãâ$çC¯âD$â«çÉ   Ë¸ˇˇˇãÉ<&  ãìî  â|$dçHâD$\çD$âã<&  âT$`Ë¸ˇˇˇÖ¿Ö"  â¬ãÉ®  Ñ¿xçÉT  âD$∫´®  s6ã$âT$ã    Ë¸ˇˇˇãT$ãD$hd+    ÖÚ   Éƒlâ–[^_]√çt& â–çæ¯   π   ∫   Û´â«Ü¯    «Ü¸      «Ü       «Ü  Ä   «Ü  Äª  «Ü  Äª  «Ü     «Ü     «Ü  ® @«Ü!  1  «Ü(  b   Ë¸ˇˇˇã<$ãL$âoâé   «É(&      Äã®  ãD$Ë¸ˇˇˇâ$ã   Ë¸ˇˇˇã$Èˇˇˇã$ã    Ë¸ˇˇˇ∫ˇˇˇÈ˝˛ˇˇË¸ˇˇˇç¥&    Ë¸ˇˇˇU1…∫   Wâ«VSÉÏã/d°    âD$1¿ãGTjj Ë¸ˇˇˇâ∆ÉƒÖ¿Ö‘  «G    ã$πÄ   ∫   Ë¸ˇˇˇ1…∫   j j ãD$Ë¸ˇˇˇã\$∫P   «C    É√Xâª`  ã}dâ¯Ë¸ˇˇˇâ¡XZÉ˘ˇÑ   É˘PtÉ¡É˘Pá¿  â˙âÿË¸ˇˇˇãD$d+    u
Éƒâ[^_]√Ë¸ˇˇˇSã@@Ö¿uh    h    Ë¸ˇˇˇXZÎ6ãê$&  Ö“uÉ¿h    Ph0   Ë¸ˇˇˇÉƒÎããBË¸ˇˇˇ«É$&      1¿[√hh  Ë¸ˇˇˇ∫    ∏   Ë¸ˇˇˇ1…1“h    Ë¸ˇˇˇ«        hå  Ë¸ˇˇˇÉƒ√h(   h    h`   Ë¸ˇˇˇÉƒÈb  É∆Ph   Vhl  Ë¸ˇˇˇÉƒã$Ö¿tË¸ˇˇˇâÿË¸ˇˇˇh(   Vh  Ë¸ˇˇˇÉƒÈb  É∆jÙh   Vh  Ë¸ˇˇˇÉƒÎªÉ∆Ph   Vh–   Ë¸ˇˇˇÉƒÎµÉ∆h(   Vhò   Ë¸ˇˇˇÉƒÈb  «Ü,&      É∆Ph   Vh®  Ë¸ˇˇˇÉƒÈaˇˇˇh(   h    Ë¸ˇˇˇY[Èb  h(   h    h‡  Ë¸ˇˇˇÉƒÈb  hd   hd   hp  Ë¸ˇˇˇÉƒÈÍ  hd   hL  Ë¸ˇˇˇ^_ÈÍ  QâL$ˇt$Phd   h¥  Ë¸ˇˇˇãkãT$ÉƒÖÌtDã}|ÖˇÖ3  É=    ÑÍ  hd   h$  Ë¸ˇˇˇXZÈÍ  hd   hú  Ë¸ˇˇˇXZÈÍ  É=    ÑÍ  hd   h¸  Ë¸ˇˇˇY[ÈÍ  ∏H   Ë¸ˇˇˇ∏@   Ë¸ˇˇˇÉ≈PhP   Uh–  Ë¸ˇˇˇÉƒÈp  Ë¸ˇˇˇhH  Ë¸ˇˇˇX1¿«    P   √           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   ∞              †  ¿      –                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P       †  ¿  –     ∞    ¿  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           §ˇ     ¿           §ˇ    ◊  è  s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         Òˇ       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 â            ò   U   ?     ß            æ           ’      #        =          O   Y       ®        4           B  8       
 T           Òˇd  †       y  ¿       é  –  %     ä          £    ¿     s   d       
 ƒ  ∞  R     ﬂ    ©    ˘  Ä   @    
   ƒ  0     +  H       
 6  @       
 A  P       
 L  ª   -     e  Ë        Ç          î  ¿   
    
 ™             ∂             ƒ             –             Ÿ             Î             ˝               U   ?                  !             '             >             U             c             n           z             ò             †             ∂             «             œ             ‹             Á             ˜             	                          9             X             u             ì             õ             Ø             Ω                           ·           Ò     £                 $             2  ¿  –     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q     ç     ´   6  ∞   I  ª     ¿   ]  “   @  ‹     Ë   	  ˚       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  Ç  W  ä    è  G  î    °  B  ¡  B  —  B  ‡  ?  Ï  N    B          ô  =  †  S  ‹  F  ‚    Í      =  @  =  R    Z    x  F  Ü  Y  í    ü    ±  B     J  ”  P  ‚  M  ¯  U    B  #  9  C  J  m  L  ©  U  ∏  9  U  Q  o  X  Ä  D  è  U  ¶  U  µ  G  ¡  B  …  	  Ÿ  9  Ï  7  ˘          K  &  O  I  H  V    g    p  R  {  9  å  G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  á     å   E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     É     à   E  ï     õ     †   E  ®     ª     ¡     ∆   E  ”     ÿ     ›   E  ‰     È     Ó   	  Û     ¯   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    Ä  E  á    å    ë    ñ  E  ù    £    ™    Ø    ¥    π  E  ¿    ≈       A  œ    ‘  A  ›    „    Ë  E         B          E     T       Ä     Ñ     î     ò     †                                                    $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `   ê                 .   	   @       h  à              <               î                  8   	   @         à               K             Ñ  Ù                 G   	   @       x  H              ^             x	                    Z   	   @       ¿   (               m             †	                      i   	   @       Ë   (      
         u      2       l
  	                à             u  (                  Ñ   	   @       !  P               ï      2       ù  +                 §             »  ˇ                  ±             »  (                  ≠   	   @       `!  @               Ω                                 π   	   @       †!                 »             ¸                     Œ             ¸                    ”      0       ¸  &                 ‹              "                                   $  ‡     5         	                b                               ∏!  Ï                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              cmd_drivers/media/pci/cx18/cx18-alsa.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_drivers/media/pci/cx18/cx18-alsa.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            cmd_drivers/media/pci/cx18/cx18.mod := { echo  drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o;  echo; } > drivers/media/pci/cx18/cx18.mod
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  cmd_drivers/media/pci/cx18/cx18.mod := { echo  drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o;  echo; } > drivers/media/pci/cx18/cx18.mod
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                cmd_drivers/media/pci/cx18/cx18-alsa.mod := { echo  drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o;  echo; } > drivers/media/pci/cx18/cx18-alsa.mod
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            cmd_drivers/media/pci/cx18/cx18-alsa.mod := { echo  drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o;  echo; } > drivers/media/pci/cx18/cx18-alsa.mod
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            drivers/media/pci/cx18/cx18.ko
drivers/media/pci/cx18/cx18-alsa.ko
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             drivers/media/pci/cx18/cx18.ko
drivers/media/pci/cx18/cx18-alsa.ko
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_drivers/media/pci/cx18/modules.order := {   echo drivers/media/pci/cx18/cx18.ko;   echo drivers/media/pci/cx18/cx18-alsa.ko; :; } | awk '!x[$$0]++' - > drivers/media/pci/cx18/modules.order
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               cmd_drivers/media/pci/cx18/modules.order := {   echo drivers/media/pci/cx18/cx18.ko;   echo drivers/media/pci/cx18/cx18-alsa.ko; :; } | awk '!x[$$0]++' - > drivers/media/pci/cx18/modules.order
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               #include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

MODULE_INFO(depends, "snd-pcm,cx18,snd");


MODULE_INFO(srcversion, "0A31E57A452F6B591496F85");
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           #include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

MODULE_INFO(depends, "snd-pcm,cx18,snd");


MODULE_INFO(srcversion, "0A31E57A452F6B591496F85");
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           #include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

MODULE_INFO(depends, "videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit");

MODULE_ALIAS("pci:v000014F1d00005B7Asv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "1CE209474DA443168BA26F2");
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              #include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

MODULE_INFO(depends, "videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit");

MODULE_ALIAS("pci:v000014F1d00005B7Asv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "1CE209474DA443168BA26F2");
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ELF                      ®      4     (   srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                                                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   Linux                Linux        GCC: (GNU) 11.2.0           GNU  ¿        ¿                                  Òˇ       #     *   #        @   <        X   H   	     m   Q        Ä   `   7     ó            ü           ß            µ              ¡               cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module init_module cleanup_module Ã     Ù     .symtab .strtab .shstrtab .text .data .bss .modinfo .rel.gnu.linkonce.this_module .note.Linux .comment .note.GNU-stack .note.gnu.property                                                       4                      !             4                      '             4                      ,             4   ó                  9                           @       5   	   @                        S                0                  _      0       0                   h              C                     x             D  (                                l  –      
         	              <  –                                  ã                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      ®      4     (   srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                                                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   Linux                Linux        GCC: (GNU) 11.2.0           GNU  ¿        ¿                                  Òˇ       #     *   #        @   <        X   H   	     m   Q        Ä   `   7     ó            ü           ß            µ              ¡               cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module init_module cleanup_module Ã     Ù     .symtab .strtab .shstrtab .text .data .bss .modinfo .rel.gnu.linkonce.this_module .note.Linux .comment .note.GNU-stack .note.gnu.property                                                       4                      !             4                      '             4                      ,             4   ó                  9                           @       5   	   @                        S                0                  _      0       0                   h              C                     x             D  (                                l  –      
         	              <  –                                  ã                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          cmd_drivers/media/pci/cx18/cx18-alsa.mod.o := gcc -Wp,-MMD,drivers/media/pci/cx18/.cx18-alsa.mod.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -DMODULE -DKBUILD_BASENAME='"cx18_alsa.mod"' -DKBUILD_MODNAME='"cx18_alsa"' -D__KBUILD_MODNAME=kmod_cx18_alsa -c -o drivers/media/pci/cx18/cx18-alsa.mod.o drivers/media/pci/cx18/cx18-alsa.mod.c

source_drivers/media/pci/cx18/cx18-alsa.mod.o := drivers/media/pci/cx18/cx18-alsa.mod.c

deps_drivers/media/pci/cx18/cx18-alsa.mod.o := \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/RETPOLINE) \
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
    $(wi