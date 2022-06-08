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
  include/linux/siphash.h GüAŸ´p]óZs×Ğ*ä”/AØP3õnÛ±*vû¦
+Ô;úXÓü–·ol4ÃÊ:VÙ*€¹(Šwzè§şQá)‹X9<³ÿ
[:2ÓIÔàWÑnrVK¸ÒR£	«ÅÎ›±Û˜‚+äP•IŞ«®û‰|»Õ?Ÿ d·ÁÇÇöŸÊÍ(ü,7ÒSC¶L7hÔ§³w­;¼«%õÚ#4‡¬»…y¿½]ÚdëT9\)Ş+.öó§nK¦ÏËèM‹Oí4 ©$‚2TÉ¤yBKŒ6˜@Åá²€;Ø[¤¢ïÃØ~.Ì÷m¡õ5İ®ÎÇp×û’ãßº‡Ö!ñŒF•±ìsfÊ„;X8ìgA¡[ˆ~±µ7øvµó+ Š¿›ïÿ 9¼ĞãğÆçf ¬ñ]kOY0íÊV¹	~£±·¸Ê§ÚiÎ~ß_£5é—ü¿*ËöúÙMĞ·€¯,v*f
úRÎ¡ı±İ¸¥B
k±šÌVö(äÕ„å=º¸]ÖÍ
ñåˆpPlé}ºÑ–5ex·ïf‡³|ª\Z\?_¸JV`u4Èäc·q+I‹‹)à8æŸÍs8‘7y²wçÛ^ònCìQrx*%ş­’[üåå  ³?E.#û~r‰9hOQg¡!fMÉğÔÃ(Ø	µÇÂâŞšú/÷A§¤™x¢ª_ıïŠ+xPŸvoêŸl©íãRûàÖÿŒZPÕ¯Y’tª«ßÛ–~º½!¥¨ÿòÀkx2ğ:1_5øÓ¨ö|Ãİ[{2ÒZ#ş—ÿ+M88âş€’àŠ”¾-Ôµ$ wÆß“Ib-$pjìÉ¡Á…ŸœyjJoÛ#pI,‘óÆ}ág<„Í­ê\YfY#HÓ½ì&UuDhI†éd56›ì€Â–¸ oi(}OZ	hÅ¦¢Dvı¤A~à eLM!^6Ca.˜ûV²J)Æ^D •mÙ‚:ËçãÕ=à¡k.#2×¸'hçÌm/+k:İ'yeuõ“íÓ Bè‘‰Aé"µ)Mbô§·İ—|^ÄT*ÖË¼åÿÀ} ˆœSeDFÒ}µ/#îQêy—³Ç´©+ì½.¯©—«ûR5g¼ü£‘„uDÀRÈ÷ƒf¬·Gà2Óœ©oõ¸MÀãd~À'Ï·™L«¤ç4Ú…>ÿzÜ‘úBjş Ğ~9Og•.WÃ4˜£e©¥]w›¯ÏšL³È„‡‘ç(ª>>²Æè™0+èí”d¯E†®Ò0Ş¦p;†Å!Ğw^}E)M5"ÃÌ-Ù1RÉ¿w",~EîÎ1ÜÏA)¶ã\)Km3ğxÖ“È|ê>hf¯d¼%ÁíÉ‚fo*ğ+ë¹şóşyÕÕĞµÆÂUdNLDn'Q‰Âèú†·*®¥.»næÁ›C‰•Hˆ¬öç7‰i&7N¯ãÖ†œX«hVˆÏ‡F¥‰‡àßÖÛN± ¤+GhE~•7Ë*EÁ Ô®Aól+3s2Ğ}ƒøn/"Õîšİ¦sk=
‹µÌQÙã@Ô1´UÑUG${¦›@a¨81	<Ìµ)¯İ­9âÙùH7Pî÷ñy~$ëÏO¶şœƒ0;¸şaìïò®])êƒ@€²µgÂIA	1Nw_æ}?fö‘:~n‘Íµbrö|!e™MS»=®“Sr-1)]#@¤¡³&%vz;Ä3oû­Á­ZÌ5ìñuıäÚ°½É4çƒ|Ö"®±Õár²…§biz9ç"T±*ßb%m(¹‚ÏsÒz woêò]ö ¥²]‚jÓJ“jKÅUA·å­çZLÊï­¡sA¦Ao%•cî n%À¯L+°"‰ºl	{Ö?™!½gGg›ÇÑ6oÓÜ†‚ÎŠ¼OóŸÑÍÁÎ¦z¹ú·ÊP¸˜_Á'šSß¥Áí²LÊe>gbÿ‰BÃp´¡N€ÙıZOÛCAÂsz	ÆCÅSšêqb³—˜ßÑ¶>Z8xpJÚŞU†h‚–ÂÉ˜b^Ä'3Ë¿(ßSG/ssÅ)Œ$ÄSƒãÉıPÍG¥ä4c‘]Ê§+ìoi@6‚ÌJ;Øz ©g)ñÖyÌ·oON×È67—®qæöuİ©½>är²l„;XŠDc!ÕzÒÌ¶ú^ü
¼ ïRªµ3CYPÎÓúŸ¦M¥0$çºõ-)Ø%ÁêT_.“«óñ°‚î×šP’DYë{¸™íCğVŒóÔğÂ8|ğŠP92¯¬±A(¸ù1ÌÍ]Î‰bÕ‡,¸|Ú;>Ğ’íŠgØsa¶³0D'Òì4»CX÷¯k\¢tUWœûYÜoô_ÁáY¶"'a Vº~ÌZÑ7‘Î0]“>ÆíHÅj¡½ë,,p‘ı²ÅG<Cäwï¨ËNŠôÜ÷·7Z´¤%ğ)›0«
ôÊ3ÃôHá…Ö‡œ/Ñ(.Ì·	Rt8J/Ä¬ëÖ)i¾·?7Gq5¡ıv8½ÅL“×p•v°İA»®Ôqs.‰AŠçÊÁsöª*Æ*¹ÜúÚ†…âR”.ıÈİuˆ&µlNõEkŠ{5wÁß†|»âìñc!¦i¸êÊ8—,•çšı`ªUNvëİÌMÌ-íçÌ´ƒ>á³/_!—EÖÃ&ã¥ØŠø|ÿ‘²âğ¨è_hŒ`ƒ$Ú6Y{¬@2‰E,Á¯+[ï;/“x©zUm¨½dÊÚóB‰(²jì—dcVkyqƒwH\@ê¦¿éÍ q!ş£Sñ®¾Ù§ˆäİ×DİHw<iZî¿ÒÀsåb²&ùAÒ5ÖÿÑò×‚ÛÊAj÷‰±n'G¤{q G‘ş/L™©©Œ"ş¸¬JC5Í“;'¯g¹×=\~q$Ûœx,gbÉŸş£~ÿ1½€:N/ßT6š™Z¶WÎ<±¥\è,²6Û²O/«"ğ\KŸÁ§±˜÷ƒFŠ|wE)¦…£âééÓEv4aDŠ\ô§¤‚á:\dÇ2LÏ ømHû©şo4B¡Ö•¿UbãÊg]@T^¤Òj$4¦Jå‘ÌÛŞH ó·œµ™J}Ïƒ(¿°Ü–ì³à.óŞ>ŠHQ4Îµî~ºÄq,ÖÖ$ÿ¡$Vv”(PO-Ü.¹Qé¾ï5S áˆ^Â.·Bœ3òtãN‰|¢X D`ûü¿Lÿ&@²8Ï·WåŒµØÃÄ[ècòì²"(xŞÆÃç<|[5k-Éòïô¶ (YeÑGJT Bª~,÷İ”æ€ª˜ŠnN:­k·¬Ë¼¡•¢ÏViçìÂ÷û_	h‡Í.=f’W†ÅÒ5õÍÆ™şÏhµ<d°kéÀÇÏVç­Ió}š„ü×?>ü¿¦ô¹À(R·UØ“súsâÿÉkNXér§kûÕQ”¥æ…£çŒ·í£‘¢j>Çrè‘IÃd×ÿäÖ§`ò€/ôÄ¼gÍ•¿,cyvEHªëï löÖêüıÓ`ÈQÂK'-c{ÓÖõ¡h°ÓÀ:ÜabOvİ¶xğìuX©ë@wãËa×f÷(P5[J IÆ=ºBûJ#®ºO#¥çç¦îGñz2‹ğ¾5{aocxvƒüÚqkô>J3ş=Tëãy™¸_…tÌ
¸Ô²âøÔóbS^¹ªîHË}i÷uKNü_âq}£$è&›½[ÓÜâúWGâEëv„İK ö†pVì·öl/`'ÚXóÛW!dR‹^k[¿ÔœR5@÷‡ãh{õz 9YĞ(ñ(«ªÿ‘¬òZ®ÔïSË-±œCv,s]xÍªò)}È†«¼e<1-ñ×ıù\O€HñÈ‰G©%1@{¹ÛUÈr5W¿“v6+tÒ˜øº’¯6K+p°¡o‚˜id½/qĞDGÈ ‹ÓÈ®kŞ£ÅqY7ìóÀ¹4¬ÏÒ"àÜ&ˆÄB…¨[¾åˆDG*áùKY‚à6Aî¼‰è§Ğò•Ğ¼’3†£ÄO¤?Áß0¹È‘u!/Ğ¾˜ÔI_¬Å´ ¿@kàM•GÙJcĞĞFû;F(_ÅoFß$Úß¼yÀâÅ`Ê)ğ‹ºÊm´yüÏ+€ÅiˆX ¢_ÓFÇ‹˜Wær…Ï{ºT¥·ÀJI§OÕĞÌ2ó:)£×
oq½|Û9H,IïÖìCzƒlÎh±‘§uŞÄjVûÏk6^H¯„1ˆ-dœ{µh{»»à˜MœafµWn{éëoré*­vï’£˜©Gôlxo®ªì*î?{pé	ƒ^%›$-"Îj(ìªÉ‚táo©™ŒÚê^;Àê¤°,n.ÉğîĞª™‹Ë#&MÊà¾ßy{cuƒs`ÁŸ°XâÙ£\`¯áú“ İî<»âÜËÉ¶ô„0–]ÀPj.¯YñUo şk}ŸvŸ:áDT­2cÜäÊrš€sô	`İ]Ê¤oˆıßì¾U8½M¬ŸÍÃÔ1TŒaˆé~'ø8Ií’Q7­’Á‡j—¼ØáV:æìi‰›Á‰¥ÎkPšñHù=ŞşUy Ü	ºd™¹À½¤1<LŒ°³°iİ²°y$Ëg´àø½Û¼g@I_²ò5õÔÀš”Ù:µ£À·ƒóìò2K«	İš£Áj“L’¯–Kô½£³¢ãÿ…kV/…ìŠ¾Ö¾ŒÁ„tÛ’ÿÛY1âĞBYPîEÀ^Z±MlíÒñ)ğ½WĞ¨Æ©L¯vpìˆG'(IúIZİòTà1|
®ÊlÀWÈ$I&µ¾,
ÈKø@‹|),”@=½ã«-…ßiÕZ²F(ÕC{AÂör­¬ÏzCñæ®A§†Nlúä•¸pø~q©º›\?N¢~ëÛdúÂNOã1ìµ,aüÅ©Ì›÷’``¸K8Tï}fñişEà4Ç« V9½Lsòğeçö¦Sm%É¶?xE_Â7Á6S_%¼¬P®p›?.]FPêVV›lŸOõŒqûšL*Pqâ;¼b¡‘ÈníÒ¾“-Ú„ªdÇA æÇ¹<×ñÒëøÙqoåÍZ	0hÌw0#ŠÊ£^àñåÒ¢Ù½ã¹¯xw4ŞúSzP0ø‰f»Æ€¾¤„¥¹‡*.Ş¼lˆ`¹= ©·<›u[|]µiéÛëÈ!)»º{mrËy«+@í‘‡Ø:Vÿõ,wXÓJæ½€Mª3V&Yï;ïye«)˜y‘!~qÈìGyÕË­’ÓËd.7â »İFÁÀsı~1s—°š>«áæ de/linux/ptr_ring.h \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   İèº›õ\á)M_1~+j:pÁÄŠ—ÇuJ§q%İ‰U é`¢MìUjóäF¯UYòŠ†#»41¸q¤u²„íoãÅ‡°aÑæüI¾9:Àù×Ö§OfÊıP>0Ë<Ì¢îjÍ'SĞ>ù7%÷›ä¶x(‚Øb%Ÿ}¥øòWıp!ó!H¿Eb–õÜ‰L*‚.ë‰«‘SÖ®Q^¶îaÀ™õ&+î*@{S·ú¼hPÅ{|Ye¢¥|&ŞWF\èÅ÷òÏ«Ä<b$ıy¢şcxl>“<ÆÈ…‘|ï´^%Ó•~iO1CxÕÌ÷’_ÉˆåÇº>ŠÍØ×º_…ÚƒGóg]ÿörw²ˆãÔÎ2\f“Ô¥åÈLsŸ?*',ém(KL†”™5d:³ŞE®nÿïÚõØ5²§	FÂ+G=t0y·û¹ÃjTìW¾õd±ªû]Ÿ*=wqL*YÔ[/mNë|Äš[™µ¥!aXd<ªÖ´PráToØ°çüÜı¿|?ĞØÙ0[o=Y K6æ>ÁÇ}¤4nù9µ9Ú­íë«í‹Évõã‹ªúÍ•´¡‡¡Á»Õ1+%ï]´Îœ—(Pu_7Ş9¥ET4‘Gä‡ğ—|¢A²ûŸ` ¡›Ö
B€“Š°9>µÅªöCM­OÂ$(9	¯>7†ú9çŒ[û$1ŸP‡<„Q-²{´§2TOøW±7‰Zç3µ*²3'ä¬@ºTZH¬“wFI¹€¾¸txès¿t»¼+JÄğÆÿ»ç+“Ã6æŠ^¶-[Øác]È…‘àşÁ@;»aô„ljgíú>èf–?]’­İ=àÀ°ĞhÂ×üñïìî
—¤h~ø[BD"ßiğˆuzvb×X>d˜BñşZ)’mTÄbø\ñ')¨èºXúe'ŸJ{°ÃUİ»IØ%¸SZ’€ÕÓÂM¦e¼LçÍ_Ös˜q»£;Ijƒß2¤$…–°êeRQ+Õè_ZôéŸ #•hæpş†˜àËL•î8?3Îo±û¿îÂ`U@èõŒ¿!Ö°ì¡ÓHoñ[únº˜™ÂïĞF„{RWcrsØ”Öò(å·ZÜaôzº#ÀwÜ2İ8ÊøõzeBÁè : ¿„ië†“9¡")ˆÉ¤E¦*±ß<ÀêÜÖ®Ü×óaÊÂ4ß^ÑÑx¸.Á9Dó{3k¼æô­àÛ¿‚å2K8ê7F£t‘—šá¢1MÉ×TÒ+ 8·oÇ’¹#Â(r:ô’fLvHĞ›&GÇggß=ôİõî6&znå¤%+ÎTiE„²É¤QëL‰}ß%­@ |l…zÜ^ëFN¤0l¥"T?óÁp	JƒG7»3òƒDcŸpÜ`?T”w”Ùç¹ªø•`‰7m²|,Ñçúä^€%À <ÏÛ:N?éÑsŞ O¶O+È¬52ŠïÍ¿HÀyEÑZj©{4Ñ­J‰•ËõWĞ-ë$bÛğ3ÄƒKûµâ3ça‘ êÂGD7VšC€ ÊexÎ+’œQÕ¶š…—V\¤KÎğO.0¨rèá=è74)ì€(Ù—Ç+„ˆ#prV^0gN„ógd†$_A÷NÁ	Ò08&‹Bk3 İü[Uû(>"ãÛdŞ©,bb§í4®fäÔäÿKkY”q´óúÌ›^ÊğVÃz¢úÙxPÄÓ÷Ûæ<;aÂk³ÌÍ;+Œ	HM¬¶—äïÖ‘i¼¡£Xä4 Î}€¾€ğì7ÓËdÓxÇh¶5Ù¼HK¾™¾Ø¯¡¸ø2ã O(‘ß¢¡(xHi í’å²¦kŞíN“¤\7c9[UUÃ%ø/¤«¥<¬Ü~pã§	\É‹/ØÏ™&q["¶çíRo•{wkCHShy‰ã ‰eYDnïÒ!‘{Ukƒ”ÄIµ¢%Æ~8ÕÎ)#×$6zÔ,kÇlş
i$éUçËe?çQ[Ÿ±"5ŠÏsaß™î&*I¯î<­k'ßJ*>†‡ö:bB>ağ)ëÙø\­'êWú¦~zÍáNAjf@2Ë şSö—¦Q^’ŒÇ›jóA¾ánîİVš¡mjÈY#;_Ï½3›Ş>6K˜}é=mkk.ò­j½{<Ü²¹ã‘7¤›ÑşîÎFmŸı*ÄO.QÖĞïìTï¼VegP´bãîêfûZ,EFN½À·àe›‘şêç‘¤¿ZÓu¾y5Ñ6vn+ãŠ­ÏÍ“½“^—…;·Ñ—AJ?QøB	À³àÄg8}!€‘_â>´fÔĞÛR…”½²s2PBX{¦ÜW¡Øp4‹,˜A÷8Á^!*(ùâ­Á8¡z¿%5§á¯€õPü€Ÿ6ÕàhA”pÇ9’*ìrºqæa+y&aëÑFŠ/ö6bw•†J6ämLïGLÚOœbÚs?«3Î9×HÀ€›G‘úP01å'œ'0Jêd¼¨À£ÁàƒR¡È\’%±ğG¸M‹‡éÎóÜ fNZW‚a¥+nd]µİZYå4´iåe 8H…ÍfÃNÕ‹¨œÜùJİñf€züå_6~,Ôt÷ş«F?®¶æø%’ÖE°ÉSI’5±R²ÅŠËåß\µE¤éÈÛ0‘b(S‰ªõ¸z|™*¥$)’)}ºh•ì\düò  •ß©¥qÎQ°ä‚#}géØóPGGvG9ƒ‘SËıvU£–8«”}Š+kï)ƒŠPÅ¦WÇØ Æ|¿V¹n<Æéàß[›Uá2†œ†¼ÜÍÿ­\İ½ƒ‡Ntû¸(j*ï›Lª—M8´$èù¼ïÕbI}Bû7'&F¿Zğñ¹WöÇ;â
5JGâ›Ğô3Õè? É‡ğ™ÉûØAaÂZ¥`àíESÜ2{ß§(Yüİ™ò“ª1Î¡æ<…
B• SÔŸAºœı[ıJÛ©´hµühGB
è\äÓB»¸7â"øTÌ&éÍAôğÂÈŠ÷›­—X­–—Î’$¡ªlbY3Ş¥í	Âü5é5¡Î^.Ó'e%i+†« SŠå¤«ã×:Y}Û–L=z§(+¸¬¶¿5ø¿Š‹p>À¾ÓÉõ,J½ ctó—¢x]sŒæXÂ™ıú³¼j¼…·nETì‰ÛbØÇìÊ¢]EÒXØD¡çaR˜Zª-¿‰Ü4ÃßÛ£ÁÉÌ­#±å îe×Õ¨ÂguÈæuı«ıVxõ.º‹¿@“©qÇÆ©§ŸSîQçÛ­ÁÀÒs«é’¡=VkçeØgÌ“'O†ş–?5Ä²À^cŸ9ğÀ¤ÚÛ3<&óSøšy %XñÌ%1RFÄ¼Cœû—Å·óMNŞùØŞíî	%M¸DçÄ‡î½æ1¿Æ`›T÷ò´Gb~¡$@ÒVÍZÿ_:µĞ;èíİF>&ıU¶CcÄ™’¼"'¸Â¯£¸â“Yà×!Ä‚OÍ¶^
‡ú‹BğÙ	¸¥âHûƒûæqqU|Šáq!À•¬ÓiU—ˆkñ‡ôùF#}eLnÉè$qä9¯;²oz*á¾@S|w.a7 |ì<™Æ€Ã-ĞÄ‘ĞE½‡'†S^ÆØ^=!¬f†•õş™†>*å¼ajÑªÕº¬èĞ·yÔ°ĞŠÈ…{:WÿÂzß‚*×Á-"7†Œ£|ü¡¤ÏŒØîùç‰RE;Ş„`Öğü4zÙmòŞÁg\~9	ÛÄ
%ÿGËÖ=hQE»ÿ™ïnÙË^¨ğÊùÓ;G0Â3X¹I¤ÀLˆ;r_¨ÿ;4Z Öãü¸I¬¶àÀóäprÆnl,íyê?o|£i,ÕÄœ h mdŠ«Óÿ°:ªYCúïOãL=k'ó}k0b=x@u$ŞÛ^O!E4CÌ›©Vg 	‰Y,¸ˆ¾IÑ]ó0á
İ¶š4F…jÆ¦º«dÕGí6ØXg;®¸²	(›x@ïi…ùyˆ:ˆC-ÃáÆŞ÷0”EĞKöÄ±£‰³‘Ä€r
Ï”!ÿªB	{¦´Jf>4ÎP_jÉÆ¯¾—Ìé­®”(S	Óu(±YùÉ±¥EXÖlH©Ê9úœH%SEaÀánZ ïÄñ0ï¨wFùÖóZ‹7ÙœãØêy Ì·á§YÙö“F&³‰=ñg‚€­ Âö.óœˆGáOïSÖâ İË ó³eƒèÈ ³ÙVzê–}%Áùæô­Ô§:çÆ‚£*yˆø:SíDÁzšîXµûE„9m9º,%ÛX)Ş¹ê@Ô
Ö_í;ò AKš¾»Kå-‚Uöá«Ê|3Ü%Ï¸´;gjAcó´Ëc€û‹¿óÛ,=Ñ
hÁpµlŠG+«CÔAsñiD™x±ı(¢–&¼hY é~5¶"]ìw×Ü¹¿Ò×µÔÑXŞ§chÁg›³äù&Şy¡ÊkÜ°Zù½>¶ı*¶DNZâÚdëĞq=Æ–±yK˜äSµŒ$øRdÇAöênàcÎ{=2Šì:F*Å£¢OÆÂ±,wõ
QĞ›ÛĞ“ä¼l–,ıí&;˜½21IÇvªÙÖ­ë#]â%üOAi)1ú\ŠFÇ˜ˆT‰âÓ¿0éòZÊ4„¸³¨K&à@"AnFZ9Ÿ@¾<ˆ#”òEBÖw¯À½jZ?-ã•{ø4µ°mèF×CçˆÖÔã}|™b~
^”ÉÀu"$<³º¦©{^@‘±úLZÎ%ğ.+­TåC•>Õ1ÿd–H"Ø¼x_íô;_5²Úı4àŸØüì2Ìµ†ğO&uãaÍô‚çG•)5¬´Âù´À)¦3RAMOEE›¸I
«ß)ÑËv—äú·M.Ü|óŸq›F%[ÂAS9_ã6‰ß.†}›à \‰:ô&“´X¤FS«ß–H N]ôDº<_¬pªO{3ˆlrÏK¨&íÆUOUf’ùùdú‚£Ò	şà°	}JZˆ”âEñÄ˜{Ñü_¿À}*lÌ¶)V©L.•Y?¶LC[z²||”®W’Ş°~ã^’Ñ÷eš­$xe=¹İ<|´A—N¢¨_–…Øßx£NÚ(n@Ù4½ò!¼uÎíh2ûø4jR.™×*(€ÛOc$÷Ş˜<Ú’… i!Q•ıb©ßm}ì
ö@#ú¥¶zªŞİ{Bğ§7Bûğïª=g·"lºâˆõ.Ö)]©}V%ÜĞ§xFAÄCúŸ de~Âa˜Â'»'›;†”<zà³ ed´‹ç0Dø¨¾?¶Ã•r¦×¼hâ	¢ÒªhÎÇ>ƒ˜§v_;ØX!œbIKHØ^8|/ ±{ßçK<r çv¨WPjéêBæ …¬4¢‹í>ìòˆyúşO”eå•%¯»é!ÿê¸`¥ïrà]`XÉLò[ç2Öœ_W<qY	¡ıbk»&Ÿ¶FÔcSPO.Ç	&¢ÇÏaÌî ëïœŒVåÇûo¶F}`÷ÕşÙ+p¶„±"ŸÖ®Ğ§§ÑRØI!Ú
MJ¤Nu “Ş°(péÆŞÈÑ¼•“o,Ÿq–±%{„ ÇÅ@œ×‡)ÙkoÑÁâÚ °s`ã\Ô¢2¥oz¶$&Í0vRo—õ¸Ê· AI&ã¸´@V×C?Åõ—ÜT¸yŸÆ^€pbCRä$×[f@\úgz-bGŸ÷KÛ§Ê+í2ŸT°HÅb“úKIÁÖÎí4—ş•‡âWJMî‰’ƒuˆ¤$qğg#N(6ø}õ»ÍÅ¸*såBÍpšş:Âd‰•óhö£€Ö!œ=)?¶t»ŒÅömŞud	¶Zğ–%|*ëCóœàüÆ¯.‚ƒÿ’Ì-òDÏÖÅs¸i½mMÓêbiäòÚ½Á“3"ìÍ/ÌıÚ%ï£„£éÏ.Á×ÉÿSy8
Ü0zË8£B€=rî×Ú„ |¯¸KøõeMöÖC†æ´C9³ºöK^ñ±OVD±Y¼5Bb˜=Íş/Àğï³y¨7×)aU3TÊ@Î,UXXgòd2.½°şí*×K¹íåùA¾äCè8İ2Â*İàTpÀ®RŞ»Re=¸ÜCK±âPƒìoóQ'ÆlÿÆÎĞ‚›5
™33·õåÃ™IĞÂÌ
®’·h#EË'/Šªå
qôäĞàDL‹J"×Üı@cÄôÎµÇpvÃ	øÃ)U¬éşÚÂ×§ƒ¹ZDkë.â•U1!¯Iå¥([NÇÜ» "³N|¿ªG…nSu¡DnÖnàV1êÔsüÆã2àƒ4›ù±±(ÿ{èòåÏå˜¸Ä4‘m=á2íLëËóu¦ÆY5İÏDê#AÏnÂ]«[? ulã mØè,êRlmÈµâ,£3Jª‘|
çÖÈ,z/G†é;VÍ6±=lc¦ù¡Ñé¹U £Z÷3În&sE)•ëûÖ­UDºGß”°³?RRô(…¿Óz ´H<bj§5o+Ö„Yjá7B8ù*ä²]Ô%.¹‡¨9¢Ÿ¯ĞCĞ¥®<E©q¦B"¢xÇOoÿóøL€0Æ!íüø‚|%¶.uõ²è’Yù*¨¥.üò[Ì
.ş…ÒÅY()p“ŸñI³‘È~›ÛPQˆ[–_¶cMU®9¶9IÀo,¯ÙÁàQå½¿SÆ­‰c!{ì	ğÑùMß÷«NŠ¦3_Ù6èÁzp3Âª÷¼æ€+”HöQh£GùàŸØ,µ5ÛL¶ÏÈEÑÈ)F¸ok–÷‡š%ÏJ‚™…:Æ²{‚±àëBihİ&x®N$ıSt)¯t£óóÉ“]µGßûÓ0ƒ”t¦ñëÉö£éÜ¥tø{Ò)úL™ÙĞk‰(C‹ñD©‹zzºÌJ^ÿ¿Ïkútæ³2ÏœñÒ]ÃëŠ#xLv8ˆeÿ»àTie2¹Ö5ïYË€p»ğ$wUIá&ÊzXöû™ç­˜…ô±.ºPf°³mÆä¸ªè>i|E)’¦ä‡ƒ½OmÊÛ‚éj½Âx›2y#‘©‡“IÓiñ`çèq=”mîüêeèáä@ŠU)Ç¸¾Ì:RvƒVÔ¦³Ği;üƒBÄ‘¼ú.­ñ:“¬æEcc—L³‘vÃT-ßDb2šcÌG_öM'[&û"¤‡c"\w×bP€÷RëÅÿ˜q['¸\,>ìoq+:ÒÏ¬¡œ¥]·©ä÷õ‘âûl€ş$À¡¡VúèŞ~DEÚéĞ;´v5bñ¨Rj¤«¨;c`[ã‡äÓö¿“r€d\ÒãÑ–Ìè³‡¶/töÌybn×™¼¯Z)FàõÿUN½A8b#ã<iŠ;šÆq³Æ%&¾Ê‹h¡cav-!¨‘M©í1/ûHAcºànBñ)‚F†8œ™™ƒæiö>ÙµS"¡R$£ŸLşœU­àòFkYØ\Ä!ÒJÇ‚ÈÔœUÖ¬ĞóÙ¬J„2ß“"Â"¹?Î:;MmR¢­Õ‹Übì}ÄQp}îõèâ÷ÊcòUD%ø¢S8‡|¢½×ŠšÈ}Är[IVŠÌV‡3L‰ÉCãÕ”9T©Fô:*²º9WÉölIK%Ö!dõm)8Ç²µQ¦úèÊ¯ŠSi|á’6\Ç!èÊ†i°½Õu×WÜ$Ÿ!	µÒ2`\“Q”ºO±Ùd'Ëz.8˜H‘=7ÿê°%á@ë}´õë´l2¡ÖÅ ×¦ŒW”ì´[Ú[ûïÇpd›[`Ç#ïpù!~‰ºñˆ‰h¼¦…Na×>8=\˜É`Gˆuõ%şDWÓ¤¸ÿİG­KJy§)µñ÷‡{ßVÅ“#N&Ö¦Å[ßÃù¶<E'è€‰—ú|ÇŞ0ê]?6G„w`ªìB†UğË0àßÉ`Iˆ`ÓZm¨Ÿ –Òá¿Méßö‚·øãÂ¯ö¬¥Ñ–‚«7á¿ÑÑN ÂyÜV·]…ÖJ#k9ë¼Ç }ùW‚Å#•šQâ4&*”´¬`½e|f'IxtŒ}¯f^Õ<Fè£vìpä‹N4›‰Åa”©šàïEà¡ï™wU]&‰Â×¾9÷«ú†9©¯V“vú1\ƒÏ=nâ*ÎÄ?ÔÜ§q¯ßtŒpé™D’¿rkzP1ü¼¯!WØ/ùk"“áJ¬¢~ı®­>äM¡ó9pfùÕI…'·j‡ˆÈ¡uw‹*o	ÇÃ§£à'AA½ÃË¸©Gç	/"Ûƒ9á*DŒ8HUN˜øfC1áÉ¿M1ğ5®Af²VwÜÑÌ¿“Â6†”åhbáXÄ|5<ÎƒQƒª åÑ`>MD Ñ?DPigŠ˜r.q~›”I²ªë/öë9ras"»2¡W³²î	àx¤C$0ÕíP$J†+’3SôYúVœ³ÜAÓÇJ1‹ÒÃ¯ì—€ØDrdÄ 3ô}f
H“:e¬Ê%¸˜ÏéÚâ.ãni —‹Ò±BXê¡ş¢…ë÷hÉsÄ«õØ«É‰mºŸ|¹â?Ÿ Raˆ¢Û¡zåÅ‹ÛòÚÙdVÕ6st½Æ[û[qäIIŸ5<ŒÊLos,Ş<™T!õH¶0GzéÿÏ‚²"SIë‹M¿¦Z‰'…~ö°>7ß"ÜÜF¼aÀ7¾³?Ø˜÷ûõƒ`şZõ?Ø‚\	¥€ô¿PQ-Ö—Ç%ŞšÚ%Æ|OòNLk®åƒ`ÑÂ^ƒD¹šeOwğ"%qC8à±zî9¸–1ö—[û×$Ú«yğ¢D›‹>=Ê|^ŸF¶ €n~Bøcê¢¦¦¤Ç@MñÒ¤}lAÇ0g>´Ë^Œ‹KIšrOûàû†Àå\ÈjQ) ³øÖbX<èİw™§QıGä\è9"uÜ6PSC?üõˆ¦ÄXq˜²n|ğ€´(şy¶Â¡JêÂlêè/éÍ{RIæüTKVyÓ•iX
¹Şƒ:ÿå=p¹ìñNÚäÛªC	\
ÓÅ@[ÿßŸšoÊ¥š1Ù»ğ·€O:IY"{2­÷Èm)¤®ër£–,Â*ĞÊíÃâïQm40`gå$™#ßvq‡é§%æ|™·ë¢¯)5°}±ÿçédùªT^õ8$ÿkXÆÄ²?ËvKI’rÇºs4æUsé»n<l˜´ƒ}­qã¼îF¼æ¥>ÙR¯#·§0'›LûòIÏâ‚æsXh Æ©’“NDµ·C;ÒØ2oÀŒó‘¯î0Á]ıÕ’I;7Ù
ÏŒ‘GQyòFªYdÍy²[k"ƒ¤hgóæ¶FÚÚ²ÀtÉFÉ£%MËû¬f*Ğ-ä3¡ÆÃCw	‰¥-6¥ÑïÚ/y ¿ìÉñêä¦È7mäüqë!ú)xéuTµÁ×Ô]¼t‘|DÓÉ6YšÈ3Œ´†bôzŞ|hëP‚€:¶XÔWg+Ù­2û"ÆpüÖkR:…RWô2/ûM–E.ìOÛƒVr3æ<ûò¼ìó•l™áì7ß{œ•‡À‘Pj£’9‹&<3 r[<Yz_çr³Î_mS/ˆ÷	mÿà‘$*.Q÷Ã«›µXçMŠ“¼ÜlBè½jˆC{>„¬‡ÓàÃCÅr²xÚÒ×1Raş÷•v$ =´SÍ!h÷kÚÖvêÆÃàj’ÇÄø!Œ&	DÜşkÊìCúMÙŸEaëÿÕkVDÂŒxÆô•Î¸æğ8®æ•¤¥8k2ü÷†P~Vàè/µïäa´Nùê¹k…µ™ENŠŒahô†áù®¡Ú-±ü, M3­#šoXN2ÁÊ‡AµÙÁŞ´;¯G>ú ë·õÃ%..9WaQµ¸80“Hš½8/t½S·¯ÒZš–nR3ŸEğC‹ â8W„"Ì„±ÄqğĞg?³¶±º’'©0ĞÁ)äæJ ?qî]{!ñ9êoş›ÑÇûØ|†®ğK™PŸ³ÆŒX¯ËØ†_¥“+÷Ñc‚ˆ¿[ÈĞşùõaÔDÁ‚‘}÷’›Á‚0-Yş‡l3|à›ÔŸ•Ÿe¡qUí{
êœÓ©Q-A¦©˜êZîàîE¢1fŞØ•5¹öÍƒR†>DúêepÅŸáÕ7ít5hSŸ«÷äc¢Ö$J	úÔáûØ¬ßªG`,¥˜N[Ê?ze„MªD)N(ßîkºaq¬£1/Œ!^Dm-—)=–ejUx´¦´Õõ«!‚•Yå†%ÜvfÎç=x}uLİ5J›m5×yªÃÍ ³‘.şWy0}¦`aÍ²(ZQ»F![÷t6 u°á"WWú/ò#Fö?iO „­ï®péÇ\ãI“/>K‘ë4­õ& *¿ŠôLß']rÒYf|æj&V³úh¦;“–gå¸iĞü(L"*– ¢À–J´\ëçrbŠù›aWó9 ¸V¿±Á›¿B‹H*AütPgXM´ûÊíæd¬İ 0OF`V`ë@£¿røª¡0$Œß¼7èğèöp´jß<7èw“Ş«oßÕ|#`3oô¶˜^ŞFÃò-SÜ˜zĞ¸³sÆ8ÌP&‡Kª"•|EÕşgpåÜ	Óím¶uà-TD¡O¸„$ˆ°=şÅ’*øëI©-¢(ga	$!ĞW”£yÇŠh‚cg‚xÀ²k`2ôÔ>®ø'²M+·÷)=Áõ*n"ı>Ú_Áı]DJ8xeô88:ê¯jn~\ãT‘Ú s„¯ëĞÙPa”jr¸W1o]^Ê'¸’›ÒÜk3c×—U­¬k<:Ãd‚Ì‘Møj™ğ0}½â‘<š¨…g¶ÃÙ6V¶’şµÄ”ˆT6ÿ%ÏÒbb’XA-åÚôc;Â†ì´Ş<´Í|¹¯?×U5§Ş]ûĞÕùt[WK#tÎ£9SÚÚô´2–t(ç}DÈê³ò›£ òVıòƒÇÚU:Ä±$µ²U#±ºìÊ„ @CÉ¬û]^qtïbÓ~ğ}¾LqWXa>RßçYÀßÊkBe­æ¾‡jŸ¯«>ĞcÂÏ£ücµîÄ¡<ZËTIƒÅˆÉAƒBtî"w³‡ïW´ô‚DÅfÓÜx¢jì37*u¯â]a%Ï%(QmRêäQ$œ°ÄJsF8x”Ó¡Rñ†UŒÒéh© à·2P„¢\$LË^7­ápêš;ååI’.ÀRŞÂÚjù_ÏØşwØ‹%Ş;csYèx2mÒÊı!ü İÖÃ]p·Ä²gÑbÿî#¤XHğîí¢SæU!¯cìñˆAúH;¤~sU@[¢>°Ñû g 8—Ò
 Ye¿ŞåZU1‰g$ï"6=BÿŞ¤Jñ²‘u+É‡-N€ŞaO÷Îê‡Ñ£~À÷‹„îTä+Jõò¨eRCKîJÒ›e@ÙWçódğƒH­U¶€ªê|á’cÖ–Ï=¶ÿ}±úæælwñl“ _síÃ^“¼?jıõ<@L™7e~d›|!÷m«a‘óîÅ±f•ÚTxğcÈŸ£B¡Ä	ågtc¼&¿GõÒúé= I'¶I9DÏMìq¹»çëÌ½4¼aÛMcâ¯my'¦ò"Š%yKØ¸I: Š2J9?¿D¥¨)›Òğg;$2 ÓH¶Ş®Ow[U]úhD¢ „›iùY˜0âÿ·IjÇŠºöu‚©K†&"¿©uÓ:İ63nı44ÔuŞòíÏšS÷¦ÒciãKÈ{ª<ù½î·ëyæıOûò÷*¹ä§(Ü­àè'ó8ªª²ŒğCÈÿ.÷¹öF/À3:B¯û!­1!²— ÛFfTtŒ75Rd¯?Œ(£G

ÜÁ×ÇÇA–·ùöğu£É÷nM!iyÙÒ~CO¶äÚ"|°I«D‚¯Õ.Eî©®‹cF½[˜c|.S©:·b¾ÆPB ²×ÃwI5o÷]Oó×ú;ë7w‚‘F'“ »¾läUkS­µ?­ä)}Kz­¡†D›4|{èeñæÛàh|¢7Æa×³—Ûæ…W!î<æ´×î¹GÉ¬®?ê!<ü@)Y˜ÉÇë^°œè,g@,‚ ;Q	.~ŒÈ²Ïæöo`Ç€©§€öGÎ.ôˆ	£„ÈÁ§…j$Ëéx½"ñŞãT=
B^ÓËØ¶å˜âQ»ñ›JRm¬.vè¦çLîÚ|yÏ@GÃ)ñ\_y[?Ş»¿ó¨«=ó@~àİLœ;k~ƒ—oeKîô¹Qwb¿œS6™<+i^‘Q|„véKŞ˜Ú]U?:´ëà0%¢KŠE))”xø(í.mqî‚÷bmié:îJÁ•“¥í–†SÍCã™“#ÄÖÂ~«GFÄ[äo˜¹Š:†öPÈF´¯)ñ$Ó÷®·
Çq±ç?‡PrÏ3èù`ŠÑ"ŸDjJ.§×ù¬N…:÷úvØ£^ÑbE!ƒŸĞ,*:ü)ñ·ÅtZÂ-)¨g¥øñ|®ƒ_%Ìƒ0yUŒºuÙp÷¸ÃùzD0lFç\;ET¬ 8ÚÊ^xÛ²(\nA´ş¥gJL¡“	ÕæJ©X¤îT"¹G7?€ü¬Q&°÷y¯IS•ÿ¾ ÙŞRGú·aüÎZ±@²ût-SÄ ÓaOë'ÊÆ4¥lúğ-:eÃØ6c2¢o´#bÆ"s‹D×ïZ¨.˜ş¢•„Ïˆ\`iDöHÒE‡lÇB!©7/k ?³nyÒ&/‡Z¯¡Ş1BÙŞÑıŠşØù«çöØbé{öj@‘†4ÎĞü_nò°ğFÓë $Ä–,–ß;ÜÒO!¡Úuµß<>ĞÒaêóÎ?ä( u;‰*{ßLª‹øöôiï™İN”y¼kYñÊœ=0SúÃöı¼c5/7Z›åVşÕ`/µ¡7ğ€³¨Ñ·Ø´´çÑ‚ãÜ5:E…â6ß¶ÃCÃ"°Ø¤NŞı&PŸ' oPçåÑ¸Õ¥"çB9ïT ¯Çu¢˜ÆEàNMÔ<‚ª@åêV6”âì~¬¿ë|[¥ŸïR¢ÃLµC“štCJûTÀë‘üÕ(ÀäT`!—,tr[§OÅdêAú¹t÷)•ÉÎ\4È ±Ç -´.¥¶ŒÖm5g¹Æ…}„•sº:Jé/Ea¹—…ù˜àëÆuÜîÂ]>¯êËEœ¤ÜOÃiû•JÔÕ§öO>2}æ¤û;#Rû#©x/JJ¿ÂÈPµ^øğ!ËJÄgzjİ7ÕZKL!ÙÕ³Å©KzY-¿ß~¤ü>æ"ñ÷ˆ‹Y%a‰½êL¶o‚@'½¾üé"ı­Š/3­ºª‚€2Oõ	ÄOIzüÊÃ¼6á_0—zıyÕˆôêZß«äu/ıÚƒ0ÎÅ
ô‰kŞÄÿSìSÖÖƒê-­g#«mØ:ı*Üä–
b"•Mxò¨=£nYàtÌ)©7ë7ßs8N^7ä £Á©_p`Ï£ØÂßÇ;xüƒİ¡ç‰ÁÖ‚;2é35D|¦*´ÓÅ[ 'ãNt¨ „ò‹ºüwLrê‡}µ8ì3ñ/‡ Š’v]äØ`ë^è—°*…Š÷½•~×?ÏŠ¹B·1ö`\ê_mnL]}!Òy€~X‰H92/ğÕÑ"Xÿ³v‡€ëØ²\ú›×`Úí âG-#QúêİĞ:aWñ¶.¦Ó®)×oËÂ©â6\US65	'ÖrµËÚ³IÁD‡ıŞ}ê³‡z!Hsbãî.W<ñ„AVÕŞâec¾h5Õtp™âÓ@îm•æCšdAc@ÿ:¤Yúw1Ç=|Ôù²—Êó¥Ašï’»Y;ÄÁ#òÕ‘ñœi:‹´/@Z…æltÑJù)M5¿Öêö4"[;=lÅE@Ã=ƒÍ¿ªoÎÛ¶‘€ü;A.¨}ùÇ°ĞJ*IÒÅa¶ûQ@×oÓnO¥éÁ¸qË¨­¨®äĞDèÍP’¿ÇÜ ·ËfÔò"©C%1ä^vEËçşˆHLó@UíèË“»/ûV6fÒ÷ ¨rXpÍºşÃ®§è:xı[€%°œ_€.<‹"ÁršVZXÒ’`oCÂrsñCn«bn¹*ŠŞ‡0êÕ›¨(zHÑKvú·ôŞk"†µkAÂ{¸AU‹è2–Xœ¦¨R¥ëaLÂ:Ås-o†¾âßÌ‚[»¸”(ŞÓ&d›ãâ¢'%]ó‡…08u[ÚòÌÌP™ö­ÑºîJ<?Ïß¶ïÃ·x£~ğ.aÊß4K Ş+¡QŠéæâB%öÓy[»üF'??¾qš¦2íÄXAÿÇú\ËwgÛ9¯Õ±ç)J.#67§©2IX€²äÂŒÍ<7T6ìZ—ªb•í Ş©­Ø²h7ÂÂŞ7èzéŞËøIÍ|ƒ'8çÃ@iÔ6ø¡­êÑÒşUkPB¼‘ĞÁ‡ğ˜@/³ë«zæ¾,‹l>™a¾Ã$ ¤à;Ğ(	k§RˆÓ]KŒ4—HæµPGĞzRƒ{‰Z
çŒ-“¥I³5D—æ/¦ U‹Ø{1¶ü%İ°b¶÷	H}ó­3Ï„wmHìrJÙÇ¸TÿÀáR£Ê–BbIvfj›àÚ¹¨k2í—ZÇÄÿv¾—¹ÕÁ™\ÛáH}4bØ©€æYP/¥³Ø£Ïd–çê<%DN¦ğ9†÷qMÃüË$´Õ»î º¹n¥½À©yl³ÓÀ¯¡VÈ²¨@uâµÌ¸nÔæäŠå#P
Ä·©ÛKŸsö¾Âş¼å^’6Ë\µnÖV¶>Ö“t€òˆK&˜ò©•¤)†ºT[€BEùG4~ÂÈØGúP!! ÒP»^èÂµUÅ·ø‰…0R/e"ÜTWÆ¨´ rãº†á»Õeßš³ªy¬>¡Œ?«_W]l:‹
!]T¥ø­±7ıÙhsŠ%âö*ó¡ 3ZÊ…ÿìÏZ…Q'±Áä….›]
RXVÄ~÷;dŞßÉç Æ/r´%–È.ì1ÈUÀ€;]@8hkÃûî-ÑGÆª‰©úâäæn7P+p2í;¡¾T=µ@ú5¦†D®.¿»¯j{W‰ê!V–©8d˜C’šrKDı(Kx h5û”ÜŞDmæ›è±gõÇÒÑ7·¨˜±ƒ×²Ìı”âº
alKÿp 6?’½MÌdO›Vë×²Ã@lTWsá,IÕ¿ª4{x0zÅûXçùXÊËUÛ1FCäö–ñgö?	q½R7
Ò­ÖV|úRÀÔ¥Î~ƒbâæé8ÿ»ˆvo!½@è­¾0WfHØYä¾>Í­ŸÊ÷WçÙ÷–'+`àL4z¯®Ë6’.º£Qæ) jy%•;]ş³jXò¾ qs?ƒä3¡,/°2š.¥ÿS¸I<®íøö$ß–á!B‡Üg„öÇ¥øœ»¥~Úè%Za´ì“:ò¢Ü`Ş5d²}Ö(¯Lé´ïaGAcs3è&öX¢-Ã·V Z$gsLá|U\iï‰’'ŒÁÎh%½¥Q‡Ìk«¦6Ä±³*IĞDtO‰^óm B °Ó%áq¾Ú™FÜ™õn^4ê;Û«œ1†WZÕVY^Ç½Ù6º§?}z^Òü€¸ËÓí¾7XìI*aÀ†W£ú-Ûõ+?rVµ2êÂíÉ*èÏY•µÃoÏ¹.ÃeizTó
ÊŒÔç¯…’RÉ’v•QŸ©*V²]ù,)¾™ãBšô<æj†¨¼¦šf4y!/Obc”¡ÌV¶¾zò\;„èH,zP(duo~3oè"à–Uğ×p3xp’ìƒ/G•êj'[êÖåøô¿‡—Ié-„†sÜ—c¢n¬ˆRÆ>2ç¥eĞßä \Ş§í_SV$§dhƒ‘ªs);

/**
 * struct pending_free - information about data structures about to be freed
 * @zapped: Head of a list with struct lock_class elements.
 * @lock_chains_being_freed: Bitmap that indicates which lock_chains[] elements
 *	are about to be freed.
 */
struct pending_free {
	struct list_head zapped;
	DECLARE_BITMAP(lock_chains_being_freed, MAX_LOCKDEP_CHAINS);
};

/**
 * struct delayed_free - data structures used for delayed freeing
 *
 * A data structure for delayed freeing of data structures that may be
 * accessed by RCU readers at the time these were freed.
 *
 * @rcu_head:  Used to schedule an RCU callback for freeing data structures.
 * @index:     Index of @pf to which freed data structures are added.
 * @scheduled: Whether or not an RCU callback has been scheduled.
 * @pf:        Array with information about data structures about to be freed.
 */
static struct delayed_free {
	struct rcu_head		rcu_head;
	int			index;
	int			scheduled;
	struct pending_free	pf[2];
} delayed_free;

/*
 * The lockdep classes are in a hash-table as well, for fast lookup:
 */
#define CLASSHASH_BITS		(MAX_LOCKDEP_KEYS_BITS - 1)
#define CLASSHASH_SIZE		(1UL << CLASSHASH_BITS)
#define __classhashfn(key)	hash_long((unsigned long)key, CLASSHASH_BITS)
#define classhashentry(key)	(classhash_table + __classhashfn((key)))

static struct hlist_head classhash_table[CLASSHASH_SIZE];

/*
 * We put the lock dependency chains into a hash-table as well, to cache
 * their existence:
 */
#define CHAINHASH_BITS		(MAX_LOCKDEP_CHAINS_BITS-1)
#define CHAINHASH_SIZE		(1UL << CHAINHASH_BITS)
#define __chainhashfn(chain)	hash_long(chain, CHAINHASH_BITS)
#define chainhashentry(chain)	(chainhash_table + __chainhashfn((chain)))

static struct hlist_head chainhash_table[CHAINHASH_SIZE];

/*
 * the id of held_lock
 */
static inline u16 hlock_id(struct held_lock *hlock)
{
	BUILD_BUG_ON(MAX_LOCKDEP_KEYS_BITS + 2 > 16);

	return (hlock->class_idx | (hlock->read << MAX_LOCKDEP_KEYS_BITS));
}

static inline unsigned int chain_hlock_class_idx(u16 hlock_id)
{
	return hlock_id & (MAX_LOCKDEP_KEYS - 1);
}

/*
 * The hash key of the lock dependency chains is a hash itself too:
 * it's a hash of all locks taken up to that lock, including that lock.
 * It's a 64-bit hash, because it's important for the keys to be
 * unique.
 */
static inline u64 iterate_chain_key(u64 key, u32 idx)
{
	u32 k0 = key, k1 = key >> 32;

	__jhash_mix(idx, k0, k1); /* Macro that modifies arguments! */

	return k0 | (u64)k1 << 32;
}

void lockdep_init_task(struct task_struct *task)
{
	task->lockdep_depth = 0; /* no locks held yet */
	task->curr_chain_key = INITIAL_CHAIN_KEY;
	task->lockdep_recursion = 0;
}

static __always_inline void lockdep_recursion_inc(void)
{
	__this_cpu_inc(lockdep_recursion);
}

static __always_inline void lockdep_recursion_finish(void)
{
	if (WARN_ON_ONCE(__this_cpu_dec_return(lockdep_recursion)))
		__this_cpu_write(lockdep_recursion, 0);
}

void lockdep_set_selftest_task(struct task_struct *task)
{
	lockdep_selftest_task_struct = task;
}

/*
 * Debugging switches:
 */

#define VERBOSE			0
#define VERY_VERBOSE		0

#if VERBOSE
# define HARDIRQ_VERBOSE	1
# define SOFTIRQ_VERBOSE	1
#else
# define HARDIRQ_VERBOSE	0
# define SOFTIRQ_VERBOSE	0
#endif

#if VERBOSE || HARDIRQ_VERBOSE || SOFTIRQ_VERBOSE
/*
 * Quick filtering for interesting events:
 */
static int class_filter(struct lock_class *class)
{
#if 0
	/* Example */
	if (class->name_version == 1 &&
			!strcmp(class->name, "lockname"))
		return 1;
	if (class->name_version == 1 &&
			!strcmp(class->name, "&struct->lockfield"))
		return 1;
#endif
	/* Filter everything else. 1 would be to allow everything else */
	return 0;
}
#endif

static int verbose(struct lock_class *class)
{
#if VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static void print_lockdep_off(const char *bug_msg)
{
	printk(KERN_DEBUG "%s\n", bug_msg);
	printk(KERN_DEBUG "turning off the locking correctness validator.\n");
#ifdef CONFIG_LOCK_STAT
	printk(KERN_DEBUG "Please attach the output of /proc/lock_stat to the bug report\n");
#endif
}

unsigned long nr_stack_trace_entries;

#ifdef CONFIG_PROVE_LOCKING
/**
 * struct lock_trace - single stack backtrace
 * @hash_entry:	Entry in a stack_trace_hash[] list.
 * @hash:	jhash() of @entries.
 * @nr_entries:	Number of entries in @entries.
 * @entries:	Actual stack backtrace.
 */
struct lock_trace {
	struct hlist_node	hash_entry;
	u32			hash;
	u32			nr_entries;
	unsigned long		entries[] __aligned(sizeof(unsigned long));
};
#define LOCK_TRACE_SIZE_IN_LONGS				\
	(sizeof(struct lock_trace) / sizeof(unsigned long))
/*
 * Stack-trace: sequence of lock_trace structures. Protected by the graph_lock.
 */
static unsigned long stack_trace[MAX_STACK_TRACE_ENTRIES];
static struct hlist_head stack_trace_hash[STACK_TRACE_HASH_SIZE];

static bool traces_identical(struct lock_trace *t1, struct lock_trace *t2)
{
	return t1->hash == t2->hash && t1->nr_entries == t2->nr_entries &&
		memcmp(t1->entries, t2->entries,
		       t1->nr_entries * sizeof(t1->entries[0])) == 0;
}

static struct lock_trace *save_trace(void)
{
	struct lock_trace *trace, *t2;
	struct hlist_head *hash_head;
	u32 hash;
	int max_entries;

	BUILD_BUG_ON_NOT_POWER_OF_2(STACK_TRACE_HASH_SIZE);
	BUILD_BUG_ON(LOCK_TRACE_SIZE_IN_LONGS >= MAX_STACK_TRACE_ENTRIES);

	trace = (struct lock_trace *)(stack_trace + nr_stack_trace_entries);
	max_entries = MAX_STACK_TRACE_ENTRIES - nr_stack_trace_entries -
		LOCK_TRACE_SIZE_IN_LONGS;

	if (max_entries <= 0) {
		if (!debug_locks_off_graph_unlock())
			return NULL;

		print_lockdep_off("BUG: MAX_STACK_TRACE_ENTRIES too low!");
		dump_stack();

		return NULL;
	}
	trace->nr_entries = stack_trace_save(trace->entries, max_entries, 3);

	hash = jhash(trace->entries, trace->nr_entries *
		     sizeof(trace->entries[0]), 0);
	trace->hash = hash;
	hash_head = stack_trace_hash + (hash & (STACK_TRACE_HASH_SIZE - 1));
	hlist_for_each_entry(t2, hash_head, hash_entry) {
		if (traces_identical(trace, t2))
			return t2;
	}
	nr_stack_trace_entries += LOCK_TRACE_SIZE_IN_LONGS + trace->nr_entries;
	hlist_add_head(&trace->hash_entry, hash_head);

	return trace;
}

/* Return the number of stack traces in the stack_trace[] array. */
u64 lockdep_stack_trace_count(void)
{
	struct lock_trace *trace;
	u64 c = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(stack_trace_hash); i++) {
		hlist_for_each_entry(trace, &stack_trace_hash[i], hash_entry) {
			c++;
		}
	}

	return c;
}

/* Return the number of stack hash chains that have at least one stack trace. */
u64 lockdep_stack_hash_count(void)
{
	u64 c = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(stack_trace_hash); i++)
		if (!hlist_empty(&stack_trace_hash[i]))
			c++;

	return c;
}
#endif

unsigned int nr_hardirq_chains;
unsigned int nr_softirq_chains;
unsigned int nr_process_chains;
unsigned int max_lockdep_depth;

#ifdef CONFIG_DEBUG_LOCKDEP
/*
 * Various lockdep statistics:
 */
DEFINE_PER_CPU(struct lockdep_stats, lockdep_stats);
#endif

#ifdef CONFIG_PROVE_LOCKING
/*
 * Locking printouts:
 */

#define __USAGE(__STATE)						\
	[LOCK_USED_IN_##__STATE] = "IN-"__stringify(__STATE)"-W",	\
	[LOCK_ENABLED_##__STATE] = __stringify(__STATE)"-ON-W",		\
	[LOCK_USED_IN_##__STATE##_READ] = "IN-"__stringify(__STATE)"-R",\
	[LOCK_ENABLED_##__STATE##_READ] = __stringify(__STATE)"-ON-R",

static const char *usage_str[] =
{
#define LOCKDEP_STATE(__STATE) __USAGE(__STATE)
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	[LOCK_USED] = "INITIAL USE",
	[LOCK_USED_READ] = "INITIAL READ USE",
	/* abused as string storage for verify_lock_unused() */
	[LOCK_USAGE_STATES] = "IN-NMI",
};
#endif

const char *__get_key_name(const struct lockdep_subclass_key *key, char *str)
{
	return kallsyms_lookup((unsigned long)key, NULL, NULL, NULL, str);
}

static inline unsigned long lock_flag(enum lock_usage_bit bit)
{
	return 1UL << bit;
}

static char get_usage_char(struct lock_class *class, enum lock_usage_bit bit)
{
	/*
	 * The usage character defaults to '.' (i.e., irqs disabled and not in
	 * irq context), which is the safest usage category.
	 */
	char c = '.';

	/*
	 * The order of the following usage checks matters, which will
	 * result in the outcome character as follows:
	 *
	 * - '+': irq is enabled and not in irq context
	 * - '-': in irq context and irq is disabled
	 * - '?': in irq context and irq is enabled
	 */
	if (class->usage_mask & lock_flag(bit + LOCK_USAGE_DIR_MASK)) {
		c = '+';
		if (class->usage_mask & lock_flag(bit))
			c = '?';
	} else if (class->usage_mask & lock_flag(bit))
		c = '-';

	return c;
}

void get_usage_chars(struct lock_class *class, char usage[LOCK_USAGE_CHARS])
{
	int i = 0;

#define LOCKDEP_STATE(__STATE) 						\
	usage[i++] = get_usage_char(class, LOCK_USED_IN_##__STATE);	\
	usage[i++] = get_usage_char(class, LOCK_USED_IN_##__STATE##_READ);
#include "lockdep_states.h"
#undef LOCKDEP_STATE

	usage[i] = '\0';
}

static void __print_lock_name(struct lock_class *class)
{
	char str[KSYM_NAME_LEN];
	const char *name;

	name = class->name;
	if (!name) {
		name = __get_key_name(class->key, str);
		printk(KERN_CONT "%s", name);
	} else {
		printk(KERN_CONT "%s", name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		if (class->subclass)
			printk(KERN_CONT "/%d", class->subclass);
	}
}

static void print_lock_name(struct lock_class *class)
{
	char usage[LOCK_USAGE_CHARS];

	get_usage_chars(class, usage);

	printk(KERN_CONT " (");
	__print_lock_name(class);
	printk(KERN_CONT "){%s}-{%d:%d}", usage,
			class->wait_type_outer ?: class->wait_type_inner,
			class->wait_type_inner);
}

static void print_lockdep_cache(struct lockdep_map *lock)
{
	const char *name;
	char str[KSYM_NAME_LEN];

	name = lock->name;
	if (!name)
		name = __get_key_name(lock->key->subkeys, str);

	printk(KERN_CONT "%s", name);
}

static void print_lock(struct held_lock *hlock)
{
	/*
	 * We can be called locklessly through debug_show_all_locks() so be
	 * extra careful, the hlock might have been released and cleared.
	 *
	 * If this indeed happens, lets pretend it does not hurt to continue
	 * to print the lock unless the hlock class_idx does not point to a
	 * registered class. The rationale here is: since we don't attempt
	 * to distinguish whether we are in this situation, if it just
	 * happened we can't count on class_idx to tell either.
	 */
	struct lock_class *lock = hlock_class(hlock);

	if (!lock) {
		printk(KERN_CONT "<RELEASED>\n");
		return;
	}

	printk(KERN_CONT "%px", hlock->instance);
	print_lock_name(lock);
	printk(KERN_CONT ", at: %pS\n", (void *)hlock->acquire_ip);
}

static void lockdep_print_held_locks(struct task_struct *p)
{
	int i, depth = READ_ONCE(p->lockdep_depth);

	if (!depth)
		printk("no locks held by %s/%d.\n", p->comm, task_pid_nr(p));
	else
		printk("%d lock%s held by %s/%d:\n", depth,
		       depth > 1 ? "s" : "", p->comm, task_pid_nr(p));
	/*
	 * It's not reliable to print a task's held locks if it's not sleeping
	 * and it's not the current task.
	 */
	if (p != current && task_is_running(p))
		return;
	for (i = 0; i < depth; i++) {
		printk(" #%d: ", i);
		print_lock(p->held_locks + i);
	}
}

static void print_kernel_ident(void)
{
	printk("%s %.*s %s\n", init_utsname()->release,
		(int)strcspn(init_utsname()->version, " "),
		init_utsname()->version,
		print_tainted());
}

static int very_verbose(struct lock_class *class)
{
#if VERY_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

/*
 * Is this the address of a static object:
 */
#ifdef __KERNEL__
/*
 * Check if an address is part of freed initmem. After initmem is freed,
 * memory can be allocated from it, and such allocations would then have
 * addresses within the range [_stext, _end].
 */
#ifndef arch_is_kernel_initmem_freed
static int arch_is_kernel_initmem_freed(unsigned long addr)
{
	if (system_state < SYSTEM_FREEING_INITMEM)
		return 0;

	return init_section_contains((void *)addr, 1);
}
#endif

static int static_obj(const void *obj)
{
	unsigned long start = (unsigned long) &_stext,
		      end   = (unsigned long) &_end,
		      addr  = (unsigned long) obj;

	if (arch_is_kernel_initmem_freed(addr))
		return 0;

	/*
	 * static variable?
	 */
	if ((addr >= start) && (addr < end))
		return 1;

	/*
	 * in-kernel percpu var?
	 */
	if (is_kernel_percpu_address(addr))
		return 1;

	/*
	 * module static or percpu var?
	 */
	return is_module_address(addr) || is_module_percpu_address(addr);
}
#endif

/*
 * To make lock name printouts unique, we calculate a unique
 * class->name_version generation counter. The caller must hold the graph
 * lock.
 */
static int count_matching_names(struct lock_class *new_class)
{
	struct lock_class *class;
	int count = 0;

	if (!new_class->name)
		return 0;

	list_for_each_entry(class, &all_lock_classes, lock_entry) {
		if (new_class->key - new_class->subclass == class->key)
			return class->name_version;
		if (class->name && !strcmp(class->name, new_class->name))
			count = max(count, class->name_version);
	}

	return count + 1;
}

/* used from NMI context -- must be lockless */
static noinstr struct lock_class *
look_up_lock_class(const struct lockdep_map *lock, unsigned int subclass)
{
	struct lockdep_subclass_key *key;
	struct hlist_head *hash_head;
	struct lock_class *class;

	if (unlikely(subclass >= MAX_LOCKDEP_SUBCLASSES)) {
		instrumentation_begin();
		debug_locks_off();
		printk(KERN_ERR
			"BUG: looking up invalid subclass: %u\n", subclass);
		printk(KERN_ERR
			"turning off the locking correctness validator.\n");
		dump_stack();
		instrumentation_end();
		return NULL;
	}

	/*
	 * If it is not initialised then it has never been locked,
	 * so it won't be present in the hash table.
	 */
	if (unlikely(!lock->key))
		return NULL;

	/*
	 * NOTE: the class-key must be unique. For dynamic locks, a static
	 * lock_class_key variable is passed in through the mutex_init()
	 * (or spin_lock_init()) call - which acts as the key. For static
	 * locks we use the lock object itself as the key.
	 */
	BUILD_BUG_ON(sizeof(struct lock_class_key) >
			sizeof(struct lockdep_map));

	key = lock->key->subkeys + subclass;

	hash_head = classhashentry(key);

	/*
	 * We do an RCU walk of the hash, see lockdep_free_key_range().
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return NULL;

	hlist_for_each_entry_rcu_notrace(class, hash_head, hash_entry) {
		if (class->key == key) {
			/*
			 * Huh! same key, different name? Did someone trample
			 * on some memory? We're most confused.
			 */
			WARN_ON_ONCE(class->name != lock->name &&
				     lock->key != &__lockdep_no_validate__);
			return class;
		}
	}

	return NULL;
}

/*
 * Static locks do not have their class-keys yet - for them the key is
 * the lock object itself. If the lock is in the per cpu area, the
 * canonical address of the lock (per cpu offset removed) is used.
 */
static bool assign_lock_key(struct lockdep_map *lock)
{
	unsigned long can_addr, addr = (unsigned long)lock;

#ifdef __KERNEL__
	/*
	 * lockdep_free_key_range() assumes that struct lock_class_key
	 * objects do not overlap. Since we use the address of