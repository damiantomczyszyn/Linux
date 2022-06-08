card include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
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
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
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
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/IOMMU_SVA) \
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
    $(wildcard include/config/DEBUG_MUTEXES) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPUCOE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/NET_L3_MASTER_DEV) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/DECNET) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/MCTP) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/net_namespace.h \
    $(wildcard include/config/IEEE802154_6LOWPAN) \
    $(wildcard include/config/IP_SCTP) \
    $(wildcard include/config/NETFILTER) \
    $(wildcard include/config/WEXT_CORE) \
    $(wildcard include/config/MPLS) \
    $(wildcard include/config/CAN) \
    $(wildcard include/config/CRYPTO_USER) \
    $(wildcard include/config/SMC) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/NET_NS_REFCNT_TRACKER) \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/XFRM_STATISTICS) \
    $(wildcard include/config/TLS) \
  include/net/snmp.h \
  include/uapi/linux/snmp.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/IP_MULTIPLE_TABLES) \
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
  include/linux/seq_file_net.h \
  include/net/netprio_cgroup.h \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/SECURITY_NELF                      X)      4     (               Ë¸ˇˇˇWçzVã∞<  ÉÁ¸ãâãÜÄ   âÇÄ   )˙1¿çäÑ   )÷¡ÈÛ•^_√ç¥&    fêË¸ˇˇˇãBÖ¿u$WVã2É˛w+iˆò   çzπ!   1¿Å∆@  Û•^_√∏Íˇˇˇ√ç¥&    çv ∏ÍˇˇˇÎÊç¥&    fêË¸ˇˇˇãAÖ¿u«A   1¿√ç¥&    çv ∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇâ¬«A   â»ãä<  ãIâHãä<  ãIâHãä<  ãâå   âHãí<  ãíà   âP1¿√ç¥&    Ë¸ˇˇˇU∂ÈWV∂ÚSâ√É√pÉÏã∏î   «$    âÈâÚâ¯Ë¸ˇˇˇÖ¿à¸ˇˇˇ1“Éƒâ–[^_]√ç¥&    ç∂    Ë¸ˇˇˇVSâ√ã@Å{(	ò uKç∞@ˇˇˇ∂K|∫
   âËÖˇˇˇÖ¿t	[^√ç∂    ∂K|∫	   âËhˇˇˇ∂K|Ö¿uﬂâ[∫   ^ÈRˇˇˇfê∏ÍˇˇˇÎ»ç¥&    fêË¸ˇˇˇÄ=     SãX\ÖM   âÿË¸ˇˇˇçÉ¿   Ë¸ˇˇˇ1¿[√çt& Ë¸ˇˇˇâ¬«A   â»ãä<  ãIâHãä<  ãIâHãä<  ãâå   âHãí<  ãíà   âP1¿√ç¥&    Ë¸ˇˇˇUWVSãÖ€u5â≈çz1ˆç¥&    çv çìD  π   â¯Ë¸ˇˇˇÖ¿tÉ∆Å√ò   É˛u‹∏Íˇˇˇ[^_]√fêiˆò   Å√@  âù<  ãûƒ  ∂Ä˙\uÎ◊ç∂    ∂SÉ√Ä˙\t≈Ä{uÓÖ¿uÍ∂KâËË'˛ˇˇÎ›çt& êË¸ˇˇˇUWVSâ√ÅÏà   ãsXd°    âÑ$Ñ   1¿ÖˆÑƒ  ãCãPãRË¸ˇˇˇ%   =   Öû   çCπ¿  ∫`  Ë¸ˇˇˇâ≈Ö¿Ñ§   â∞0  π@   â⁄æ   «Ä<  @  Ë¸ˇˇˇãC∑Sçà  QRRˇ∞   ãCTˇ0h0   Ë¸ˇˇˇãΩî   ÉƒÎç¥&    çv ∏
   Ë¸ˇˇˇÉÓÑﬁ  1“â¯Ë¸ˇˇˇâ√Ö¿xﬁÈh   ª˚ˇˇˇãÑ$Ñ   d+    uÅƒà   âÿ[^_]√ªÙˇˇˇÎ‹Ë¸ˇˇˇç¥&    çt& Ë¸ˇˇˇ9ê8  tSUWVâ÷Sâ√ç@pÉÏÉ˙ãx$«$    ÌâD$ÉÂâÈ∫   â¯Ë¸ˇˇˇÖ¿à  â≥8  1“Éƒâ–[^_]√ç¥&    1¿√ç¥&    ç∂    Ë¸ˇˇˇUWø   Vâ÷SÉÏ«   ã®î   â$Îçt& ∏
   Ë¸ˇˇˇÉÔÑÖ  ∫7   âËË¸ˇˇˇâ√Ö¿x€ã$â\$ø   ã®î   Îç¥&    ∏
   Ë¸ˇˇˇÉÔÑ¡  ∫8   âËË¸ˇˇˇâ√Ö¿x€âD$ã\$ø   ã$â\$ã®î   Îç¥&    ∏
   Ë¸ˇˇˇÉÔÑ‹  ∫9   âËË¸ˇˇˇâ√Ö¿x€âD$ã$ã∏î   ∏   â˝â«Îçv ∏
   Ë¸ˇˇˇÉÔÑ˜  ∫:   âËË¸ˇˇˇâ√Ö¿x€∂|$â≈ã\$∫Ó  ¡Â«    1…â¯∂€ÅÂ   ¡ˇ¡‡ÉÁ%   	√∂D$	≈∏@  Îç¥&    É¡ò   â=h  tW∑êí   9⁄u‰f;∏ê   u€∑êî   fÉ˙ˇt9Íw ∑êñ   9Írøç¥&    Ä=     Ö  1€Éƒâÿ[^_]√ç¥&    ∂    É˘u‘Ñ¿Ö†  ªΩˇˇˇÎ‘ç¥&    ç∂    Ë¸ˇˇˇWVSâ”ÉÏdã    âT$â‚ËÒ˝ˇˇÖ¿u6i$ò   ç{ÉÁ¸ç≤@  ãí@  âãñÄ   âìÄ   )˚çãÑ   )ﬁ¡ÈÛ•ãT$d+    uÉƒ[^_√Ë¸ˇˇˇç¥&    ç¥&    Ë¸ˇˇˇVSâ√çspÉÏd°    âD$1¿â‚âÿËl˝ˇˇãÉ<  ˇpˇpVh¨   Ë¸ˇˇˇãD$ÉƒÉ¯Ö/  Èî          –                ‡                                                                                                       @   ê   ¿     `  –     P      Ä  p        4%s: Write: retry ... %d
 3%s: TVP7002 write error %d
 No platform data
 6%s: Rev. %02x detected.
 3%s: TVP7002 read error %d
 7%s: Fail to set streaming
 7%