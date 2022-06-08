i/cx18/cx18-av-core.h \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ELF                      ¤"      4     (            GNU  ΐ       ΐ          θόÿÿÿ…ΐt?S‰Γ‹€  …ΐt‹…Òt
Η‚$&      θόÿÿÿΗƒ      Ηƒ      [Γ΄&    Γ΄&    θόÿÿÿUWV‰ΖS‰χƒμd΅    ‰D$1ΐƒΗ„Ξ   ‹–  …Ò„ύ   ‹†,&  …ΐ…   ‹FΗ$    1ΙΊÿÿÿÿTj    h    θόÿÿÿƒΔ…ΐ…t   ΅   Ή   Ίΐ  ‹,$θόÿÿÿ‰Γ…ΐ„Z   ‰8Ή   Ί
   ‰h‰…  Η…      ‹x‹(GθόÿÿÿG$ÿuψƒΗDh   j Pθόÿÿÿÿudÿuψh@  jPWθόÿÿÿ‰ΨƒΔ$θόÿÿÿ…ΐ…   ‰,&  ‹$θόÿÿÿ…ΐ…¨   φ    u-‹D$d+    uƒΔ1ΐ[^_]Γv φ    tέιόÿÿÿθόÿÿÿιδ   ffffθόÿÿÿ‹@Η@    Η@    1ΐΓ΄&    θόÿÿÿ1ΐΓ΄&    θόÿÿÿVS‹p^‰Ψθόÿÿÿ‹v‰Β‰Ψθόÿÿÿ‰π[^Γ΄&    t& θόÿÿÿUWVS‰Γƒμ‹5   ‰$‰Κ…φ…2  ‹h…ν„Β   ‹}|…ÿ„·   ‹w`Αξ„   9ς‚  ‹4  …Ι„7  ‰Π1Òχφ‹S‰D$‹GT‰D$‰Π―ΖΘ‰Ρ‰D$‹D$Π‰Β‹D$9Βsv―t$‹$‹D$‰ρθόÿÿÿ‰θθόÿÿÿ‹C‹t$π‰C‹OT‰Β)Κ9ΘCΒs‰s‰ρ‰C‹GL9Ζƒ   ƒΔ‰θ[^_]ιόÿÿÿ‹-   …ν…  ƒΔ[^_]Γ΄&    v ‰Β‰L$‹D$)Κ‰Ρ‹$―Ξ‰L$θόÿÿÿ‹D$‹L$+L$‹$Θ‹L$―ΖΚ‰Α‹‡4  θόÿÿÿιTÿÿÿ΄&    ‹   …Ϋ…   ƒΔ[^_]Γ΄&    v )Α‰θ‰Kθόÿÿÿ‰θƒΔ[^_]ιόÿÿÿ¶    ‹   …Ι„Pÿÿÿι‡  ΄&    ¶    θόÿÿÿWVS‹p‹ƒ   »T  θόÿÿÿ1Ò‰ψθόÿÿÿπ€£¨  ο‰ψθόÿÿÿΗƒ(&      ‹   θόÿÿÿ1ΐ[^_Γ΄&    ΄&    θόÿÿÿU‰ΕWVSƒμl‹u|d΅    ‰D$h‹E‹‰$Cψ‰D$‰Ηƒ   θόÿÿÿ‹ƒ<&  ‹“”  ‰|$dH‰D$\D$‰‹<&  ‰T$`θόÿÿÿ…ΐ…"  ‰Β‹ƒ¨  „ΐxƒT  ‰D$πΊ«¨  s6‹$‰T$‹    θόÿÿÿ‹T$‹D$hd+    …ς   ƒΔl‰Π[^_]Γt& ‰ΠΎψ   Ή   Ί   σ«‰πΗ†ψ    Η†ό      Η†       Η†  €   Η†  €»  Η†  €»  Η†     Η†     Η†  ¨ @Η†!  1  Η†(  b   θόÿÿÿ‹<$‹L$‰o‰π   Ηƒ(&      π€‹¨  ‹D$θόÿÿÿ‰$‹   θόÿÿÿ‹$ιÿÿÿ‹$‹    θόÿÿÿΊπÿÿÿιύώÿÿθόÿÿÿ΄&    θόÿÿÿU1ΙΊ   W‰ΗVSƒμ‹/d΅    ‰D$1ΐ‹GTjj θόÿÿÿ‰ΖƒΔ…ΐ…Τ  ΗG    ‹$Ή€   Ί   θόÿÿÿ1ΙΊ   j j ‹D$θόÿÿÿ‹\$ΊP   ΗC    ƒΓX‰»`  ‹}d‰ψθόÿÿÿ‰ΑXZƒωÿ„Κ  ƒωPtƒΑƒωP‡ΐ  ‰ϊ‰Ψθόÿÿÿ‹D$d+    u
ƒΔ‰π[^_]ΓθόÿÿÿS‹@@…ΐuh    h    θόÿÿÿXZλ6‹$&  …Òuƒΐh    Ph0   θόÿÿÿƒΔλ‹‹BθόÿÿÿΗƒ$&      1ΐ[Γhh  θόÿÿÿΊ    Έ   θόÿÿÿ1Ι1Òh    θόÿÿÿΗ        h  θόÿÿÿƒΔΓh(   h    h`   θόÿÿÿƒΔιb  ƒΖPh   Vhl  θόÿÿÿƒΔ‹$…ΐtθόÿÿÿ‰Ψθόÿÿÿh(   Vh  θόÿÿÿƒΔιb  ƒΖjτh   Vh  θόÿÿÿƒΔλ»ƒΖPh   VhΠ   θόÿÿÿƒΔλµƒΖh(   Vh   θόÿÿÿƒΔιb  Η†,&      ƒΖPh   Vh¨  θόÿÿÿƒΔιaÿÿÿh(   h    θόÿÿÿY[ιb  h(   h    hΰ  θόÿÿÿƒΔιb  hd   hd   hp  θόÿÿÿƒΔικ  hd   hL  θόÿÿÿ^_ικ  Q‰L$ÿt$Phd   h΄  θόÿÿÿ‹k‹T$ƒΔ…νtD‹}|…ÿ…3  ƒ=    „κ  hd   h$  θόÿÿÿXZικ  hd   h  θόÿÿÿXZικ  ƒ=    „κ  hd   hό  θόÿÿÿY[ικ  ΈH   θόÿÿÿΈ@   θόÿÿÿƒΕPhP   UhΠ  θόÿÿÿƒΔιp  θόÿÿÿhH  θόÿÿÿX1ΐΗ    P   Γ           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   °                 ΐ      Π                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
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
     P          ΐ  Π     °    ΐ  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           ¤ÿ     ΐ           ¤ÿ    Χ    s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ρÿ       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 ‰               U   ?     §            Ύ           Υ      #     π   =          O   Y       ¨        4           B  8       
 T           ρÿd          y  ΐ         Π  %               £    ΐ     s   d       
 Δ  °  R     ί    ©    ω  €   @    
   Δ  0     +  H       
 6  @       
 A  P       
 L  »   -     e  θ        ‚          ”  ΐ   
    
 ª             ¶             Δ             Π             Ω             λ             ύ               U   ?                  !             '             >             U             c             n           z                                        ¶             Η             Ο             ά             η             χ             	                          9             X             u             “             ›             ―             ½             Κ             α           ρ     £                 $             2  ΐ  Π     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q          «   6  °   I  »     ΐ   ]  Ò   @  ά     θ   	  ϋ       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  ‚  W        G  ”    ΅  B  Α  B  Ρ  B  ΰ  ?  μ  N    B          ™  =     S  ά  F  β    κ      =  @  =  R    Z    x  F  †  Y  ’        ±  B  Κ  J  Σ  P  β  M  ψ  U    B  #  9  C  J  m  L  ©  U  Έ  9  U  Q  o  X  €  D    U  ¦  U  µ  G  Α  B  Ι  	  Ω  9  μ  7  ω          K  &  O  I  H  V    g    p  R  {  9    G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  ‡        E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     ƒ        E  •     ›         E  ¨     »     Α     Ζ   E  Σ     Ψ     έ   E  δ     ι     ξ   	  σ     ψ   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    €  E  ‡        ‘    –  E      £    ª    ―    ΄    Ή  E  ΐ    Ε    Κ  A  Ο    Τ  A  έ    γ    θ  E  π       B          E     T       €     „     ”                                                               $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `                    .   	   @       h                <             π  ”                  8   	   @       π                 K             „  τ                 G   	   @       x  H              ^             x	                    Z   	   @       ΐ   (               m              	  Κ                   i   	   @       θ   (      
         u      2       l
  	                             u  (                  „   	   @       !  P               •      2         +                 ¤             Θ  ÿ                  ±             Θ  (                  ­   	   @       `!  @               ½             π                    Ή   	   @        !                 Θ             ό                     Ξ             ό                    Σ      0       ό  &                 ά              "                                   $  ΰ     5         	                b                               Έ!  μ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ELF                      ¤"      4     (            GNU  ΐ       ΐ          θόÿÿÿ…ΐt?S‰Γ‹€  …ΐt‹…Òt
Η‚$&      θόÿÿÿΗƒ      Ηƒ      [Γ΄&    Γ΄&    θόÿÿÿUWV‰ΖS‰χƒμd΅    ‰D$1ΐƒΗ„Ξ   ‹–  …Ò„ύ   ‹†,&  …ΐ…   ‹FΗ$    1ΙΊÿÿÿÿTj    h    θόÿÿÿƒΔ…ΐ…t   ΅   Ή   Ίΐ  ‹,$θόÿÿÿ‰Γ…ΐ„Z   ‰8Ή   Ί
   ‰h‰…  Η…      ‹x‹(GθόÿÿÿG$ÿuψƒΗDh   j Pθόÿÿÿÿudÿuψh@  jPWθόÿÿÿ‰ΨƒΔ$θόÿÿÿ…ΐ…   ‰,&  ‹$θόÿÿÿ…ΐ…¨   φ    u-‹D$d+    uƒΔ1ΐ[^_]Γv φ    tέιόÿÿÿθόÿÿÿιδ   ffffθόÿÿÿ‹@Η@    Η@    1ΐΓ΄&    θόÿÿÿ1ΐΓ΄&    θόÿÿÿVS‹p^‰Ψθόÿÿÿ‹v‰Β‰Ψθόÿÿÿ‰π[^Γ΄&    t& θόÿÿÿUWVS‰Γƒμ‹5   ‰$‰Κ…φ…2  ‹h…ν„Β   ‹}|…ÿ„·   ‹w`Αξ„   9ς‚  ‹4  …Ι„7  ‰Π1Òχφ‹S‰D$‹GT‰D$‰Π―ΖΘ‰Ρ‰D$‹D$Π‰Β‹D$9Βsv―t$‹$‹D$‰ρθόÿÿÿ‰θθόÿÿÿ‹C‹t$π‰C‹OT‰Β)Κ9ΘCΒs‰s‰ρ‰C‹GL9Ζƒ   ƒΔ‰θ[^_]ιόÿÿÿ‹-   …ν…  ƒΔ[^_]Γ΄&    v ‰Β‰L$‹D$)Κ‰Ρ‹$―Ξ‰L$θόÿÿÿ‹D$‹L$+L$‹$Θ‹L$―ΖΚ‰Α‹‡4  θόÿÿÿιTÿÿÿ΄&    ‹   …Ϋ…   ƒΔ[^_]Γ΄&    v )Α‰θ‰Kθόÿÿÿ‰θƒΔ[^_]ιόÿÿÿ¶    ‹   …Ι„Pÿÿÿι‡  ΄&    ¶    θόÿÿÿWVS‹p‹ƒ   »T  θόÿÿÿ1Ò‰ψθόÿÿÿπ€£¨  ο‰ψθόÿÿÿΗƒ(&      ‹   θόÿÿÿ1ΐ[^_Γ΄&    ΄&    θόÿÿÿU‰ΕWVSƒμl‹u|d΅    ‰D$h‹E‹‰$Cψ‰D$‰Ηƒ   θόÿÿÿ‹ƒ<&  ‹“”  ‰|$dH‰D$\D$‰‹<&  ‰T$`θόÿÿÿ…ΐ…"  ‰Β‹ƒ¨  „ΐxƒT  ‰D$πΊ«¨  s6‹$‰T$‹    θόÿÿÿ‹T$‹D$hd+    …ς   ƒΔl‰Π[^_]Γt& ‰ΠΎψ   Ή   Ί   σ«‰πΗ†ψ    Η†ό      Η†       Η†  €   Η†  €»  Η†  €»  Η†     Η†     Η†  ¨ @Η†!  1  Η†(  b   θόÿÿÿ‹<$‹L$‰o‰π   Ηƒ(&      π€‹¨  ‹D$θόÿÿÿ‰$‹   θόÿÿÿ‹$ιÿÿÿ‹$‹    θόÿÿÿΊπÿÿÿιύώÿÿθόÿÿÿ΄&    θόÿÿÿU1ΙΊ   W‰ΗVSƒμ‹/d΅    ‰D$1ΐ‹GTjj θόÿÿÿ‰ΖƒΔ…ΐ…Τ  ΗG    ‹$Ή€   Ί   θόÿÿÿ1ΙΊ   j j ‹D$θόÿÿÿ‹\$ΊP   ΗC    ƒΓX‰»`  ‹}d‰ψθόÿÿÿ‰ΑXZƒωÿ„Κ  ƒωPtƒΑƒωP‡ΐ  ‰ϊ‰Ψθόÿÿÿ‹D$d+    u
ƒΔ‰π[^_]ΓθόÿÿÿS‹@@…ΐuh    h    θόÿÿÿXZλ6‹$&  …Òuƒΐh    Ph0   θόÿÿÿƒΔλ‹‹BθόÿÿÿΗƒ$&      1ΐ[Γhh  θόÿÿÿΊ    Έ   θόÿÿÿ1Ι1Òh    θόÿÿÿΗ        h  θόÿÿÿƒΔΓh(   h    h`   θόÿÿÿƒΔιb  ƒΖPh   Vhl  θόÿÿÿƒΔ‹$…ΐtθόÿÿÿ‰Ψθόÿÿÿh(   Vh  θόÿÿÿƒΔιb  ƒΖjτh   Vh  θόÿÿÿƒΔλ»ƒΖPh   VhΠ   θόÿÿÿƒΔλµƒΖh(   Vh   θόÿÿÿƒΔιb  Η†,&      ƒΖPh   Vh¨  θόÿÿÿƒΔιaÿÿÿh(   h    θόÿÿÿY[ιb  h(   h    hΰ  θόÿÿÿƒΔιb  hd   hd   hp  θόÿÿÿƒΔικ  hd   hL  θόÿÿÿ^_ικ  Q‰L$ÿt$Phd   h΄  θόÿÿÿ‹k‹T$ƒΔ…νtD‹}|…ÿ…3  ƒ=    „κ  hd   h$  θόÿÿÿXZικ  hd   h  θόÿÿÿXZικ  ƒ=    „κ  hd   hό  θόÿÿÿY[ικ  ΈH   θόÿÿÿΈ@   θόÿÿÿƒΕPhP   UhΠ  θόÿÿÿƒΔιp  θόÿÿÿhH  θόÿÿÿX1ΐΗ    P   Γ           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   °                 ΐ      Π                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
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
     P          ΐ  Π     °    ΐ  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           ¤ÿ     ΐ           ¤ÿ    Χ    s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ρÿ       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 ‰               U   ?     §            Ύ           Υ      #     π   =          O   Y       ¨        4           B  8       
 T           ρÿd          y  ΐ         Π  %               £    ΐ     s   d       
 Δ  °  R     ί    ©    ω  €   @    
   Δ  0     +  H       
 6  @       
 A  P       
 L  »   -     e  θ        ‚          ”  ΐ   
    
 ª             ¶             Δ             Π             Ω             λ             ύ               U   ?                  !             '             >             U             c             n           z                                        ¶             Η             Ο             ά             η             χ             	                          9             X             u             “             ›             ―             ½             Κ             α           ρ     £                 $             2  ΐ  Π     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q          «   6  °   I  »     ΐ   ]  Ò   @  ά     θ   	  ϋ       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  ‚  W        G  ”    ΅  B  Α  B  Ρ  B  ΰ  ?  μ  N    B          ™  =     S  ά  F  β    κ      =  @  =  R    Z    x  F  †  Y  ’        ±  B  Κ  J  Σ  P  β  M  ψ  U    B  #  9  C  J  m  L  ©  U  Έ  9  U  Q  o  X  €  D    U  ¦  U  µ  G  Α  B  Ι  	  Ω  9  μ  7  ω          K  &  O  I  H  V    g    p  R  {  9    G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  ‡        E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     ƒ        E  •     ›         E  ¨     »     Α     Ζ   E  Σ     Ψ     έ   E  δ     ι     ξ   	  σ     ψ   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    €  E  ‡        ‘    –  E      £    ª    ―    ΄    Ή  E  ΐ    Ε    Κ  A  Ο    Τ  A  έ    γ    θ  E  π       B          E     T       €     „     ”                                                               $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `                    .   	   @       h                <             π  ”                  8   	   @       π                 K             „  τ                 G   	   @       x  H              ^             x	                    Z   	   @       ΐ   (               m              	  Κ                   i   	   @       θ   (      
         u      2       l
  	                             u  (                  „   	   @       !  P               •      2         +                 ¤             Θ  ÿ                  ±             Θ  (                  ­   	   @       `!  @               ½             π                    Ή   	   @        !                 Θ             ό                     Ξ             ό                    Σ      0       ό  &                 ά              "                                   $  ΰ     5         	                b                               Έ!  μ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              cmd_drivers/media/pci/cx18/cx18-alsa.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_drivers/media/pci/cx18/cx18-alsa.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 