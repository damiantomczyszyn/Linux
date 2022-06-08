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
«ß)ÑËv—äú·M.Ü|óŸq›F%[ÂAS9_ã6‰ß.†}›à \‰:ô&“´X¤FS«ß–H N]ôDº<_¬pªO{3ˆlrÏK¨&íÆUOUf’ùùdú‚£Ò	şà°	}JZˆ”âEñÄ˜