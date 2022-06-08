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
  include/linux/siphash.h G�A���p]�Zs��*䞔/A�P3�n��*v��
+�;�X����ol4�ʁ:V�*���(�wz��Q�)�X9<��
[:2�I���W�nrVK���R�	��Λ�ۘ�+�P�Iޫ���|��?�� d��������(�,7�SC�L7hԧ�w�;��%��#4����y��]�d�T9\)�+.���nK����M�O�4��$�2TɤyBK�6�@����;�[�����~.��m��5ݮ��p����ߺ��!�F���sfʄ;X8�gA�[�~��7��v��+� ����� 9�Ё����f���]kOY0��V�	~�������i�~�_�5闍��*����Mз��,v*f
�R����ݸ�B
k���V�(����=��]��
��pPl�}�і5ex���f���|�\Z\?_�JV`u4��c�q+I��)�8��s8�7y�w��^�nC�Q�rx*%���[���� �?E.#��~r�9hOQg�!fM�����(�	����ޚ�/�A���x��_��+xP�v�o꟝l���R�����ZPկY�t���ۖ~��!������kx2�:1_��5�Ө�|��[{2��Z#���+M88��������-��$ w�ߓIb-$pj�ɡ����yjJo�#p�I,���}�g��<�ͭ�\YfY#Hӽ�&UuDhI��d56��� oi(}OZ	hŦ�Dv��A~� eLM!^6Ca.��V�J)�^D��mق:����=�k.#2׸'h��m/+k:��'yeu���� B葉A�"�)Mb���ݗ|^�T*�˼���} ��SeDF�}�/#�Q�y��Ǵ�+�.�����R5g�����uD�R���f��G�2Ӝ��o���M����d~�'Ϸ�L���4څ>�zܑ�Bj���~9Og�.W�4��e��]w��ϚL�Ȅ���(�>>���0+��d�E���0ަp;��!�w^}E)M5"��-�1Rɏ�w",~E��1��A)��\)Km3�x֓�|�>hf�d�%��ɂfo*�+����y��е��UdNLDn'Q������*��.�n���C��H����7�i&7N��ֆ�X�hV��χF�������N� �+GhE~�7�*E� ԮA�l+3s2�}���n/"Ս��ݦsk=
���Q��@�1�U�UG${��@a�81	<̵)�ݭ9���H7P���y~$��O����0;��a���])�@���g�IA	1Nw_�}?f��:~n�͵br�|!e�M��S�=��Sr-1�)]#@���&%vz;�3o����Z�5��u��ڰ��4�|�"����r���biz9�"T�*�b%m(���s�z wo��]� ��]�j�J�jK�UA����ZL����sA�Ao%�c�n%��L+�"��l	{�?�!�gGg���6o�܆�Ί�O�����Φz����P��_�'�Sߥ��L�e>gb��B�p��N���ZO�CA�sz	�C�S��qb����Ѷ>Z8xpJ��U�h���ɘb^�'3˝�(�SG/ss�)�$�S����P�G��4c�]ʧ+�oi@6��J;�z��g)��y̷oON��67���q��uݩ�>�r�l�;X�Dc!�z����^�
���R��3CYP�����M�0$��-)�%��T_.�������ךP�DY�{���C�V�����8|��P92���A(���1��]ΉbՇ,��|�;>�В�g�sa��0�D'��4�CX��k�\�t�UW��Y�o�_��Y�"'a�V�~�Z�7��0]�>Ə�H�j���,,p����G<C�w��N�����7Z���%�)�0�
��3���H��և�/�(.̷	�Rt8J/Ĭ��)i��?7Gq5���v8��L��p�v��A���qs.�A����s��*�*���چ��R�.���u�&�lN�Ek�{5w�߆|����c!�i���8�,���`�UNv���M�-��̴�>�/_!�E��&�؊�|������_h�`�$�6Y{�@2�E,��+[�;/�x�zUm��dʝ��B�(�j�dcV�kyq�wH\@ꦿ�� q!��S�٧�����D�Hw<iZ���s�b�&�A�5��������Aj���n'�G�{q G��/L�����"���JC5���;'�g��=�\~q$ۜx,gbɟ��~�1��:N/��T6��Z�W�<��\�,�6۲O/�"��\K�������F�|wE)�������Ev4aD�\�����:\d�2L� �mH���o4B�֕�Ub��g]@T^��j�$4�J����H �����J}σ(��ܖ��.��>�HQ4ε�~��q�,��$��$Vv�(PO-�.�Q��5S ��^�.�B�3�t�N�|�X�D`���L�&@�8ϷW������[�c��"(x����<|[5k-����� (Ye�GJT�B�~,�ݔ怪��nN:�k���������Vi������_	h��.=f�W�����5��ƙ��h�<d�k����V�I�}����?>������(R�Uؓs�s����kNX�r�k��Q��慣猷흣��j>�r�I�d���֧`��/��ļg͕�,cyv�EH���l������`�Q�K'-c{����h���:�abOvݶx��uX��@w��a�f�(P5[J I�=�B��J#��O#����G�z�2��5{aocxv���qk�>J3��=T��y��_�t�
�������bS^���H�}i�uKN�_�q}�$��&��[����WG�E�v��K���pV���l/`'�X���W!dR�^k[�ԜR5@���h{�z�9Y�(�(������Z���S�-��Cv,s]xͪ�)}Ȇ��e<1-�ׁ��\O�H�ȉG�%1@{�ێU�r5W��v6+tҘ����6K+p��o��id�/q�DG� ��Ȯkޣ�qY7����4���"�܏&�ĝB��[��DG*��KY��6A������3���O�?��0�ȑu!/о��I_�Ŵ �@k�M�G�Jc��F�;F�(_��oF�$�߼y���`�)����m�y��+��i�X��_�Fǋ�W�r��{�T���JI�O���2�:)��
oq�|�9H,I���Cz�l�h����u��jV��k6^H��1�-d�{�h{����M�af�Wn{��or�*�v�����G�lxo���*�?{p�	�^%��$-"�j(�ɂt�o�����^;�ꤰ,n.���Ъ���#�&M���y{cu�s�`���X�٣\`���� ��<����ɶ�0�]�Pj.�Y�Uo �k}�v��:�DT�2c���r��s�	`�]ʤo�����U8�M�����1T�a��~'�8I�Q7����j�����V:��i������kP��H�=��Uy��	�d�����1<L����iݲ�y$�g����ۼg@I_��5�������:��������2K�	ݚ��j�L���K���������kV/�슾־���tے��Y1��BYP�E�^Z�Ml����)�WШƩL�vp�G'(I�IZ��T�1|
��l�W�$I&��,
�K�@�|),�@=��-��i�Z�F(��C{A��r����zC��A��Nl����p�~q���\?N�~��d��NO�1�,a�ữ��``�K8T�}f�i�E�4���V9�Ls��e����Sm%ɶ?xE_�7�6S_%��P��p��?.]FP�VV�l�O��q��L*Pq�;�b���n�Ҿ�-ڄ�d�A ���<������qo��Z	0h�w0#�ʣ^���Ңُ�㹯xw4��SzP0��f��ƀ������*.޼�l�`�= ��<�u[�|]�i�����!)��{mr�y�+@푎��:V��,wX�J���M�3V&Y��;�ye�)�y�!~q��Gy�˭���d.7� ��F��s�~�1s���>���� de/linux/ptr_ring.h \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   �����\�)M_1~+j:p�Ċ��uJ�q%݉U �`�M�Uj��F�UY�#�41�q�u���o�Ň�a���I�9:���֧Of��P>0�<̢�j�'S�>�7%��䁶x(��b%�}���W�p!�!H�Eb��܉L*�.���S֮�Q^��a���&+�*�@{S���hP�{|�Ye��|&�WF\����ϫ�<b$�y���cxl>�<�ȅ�|�^%ӕ~iO1Cx՞����_Ɉ�Ǻ>���׺_�ڃG�g]���rw�����2\f�ԥ���Ls�?*�',�m(KL���5d:��E�n�����5��	F�+G=t0y����jT�W��d���]�*=wqL*Y�[/mN�|Ě[���!aXd<�ִPr�T�oذ�����|?���0[o=Y�K6�>��}�4n�9�9ڭ�����v����͕�������1+%�]�Μ�(�Pu_7�9�ET4�G���|�A���` ���
B����9>�Ū�CM�O�$(9	�>7��9��[��$1�P�<�Q-�{���2TO�W�7�Z�3�*�3'�@�TZH��wFI����tx�s�t��+J������+��6抎^�-[��c]������@;�a�ljg��>�f�?]���=����h�������
��h~�[BD"�i��uzvb�X>d�B��Z)�mT�b�\�')��X�e'�J{��U��I�%�SZ�����M�e�L��_��s�q��;Ij��2�$����eRQ+��_Z��� #�h�p�����L��8?3�o�����`U@����!����Ho�[�n������F�{RWcrsؔ��(�Z�a�z�#�w�2�8���zeB�� : ��i놓9�")�ɤE�*��<���֮���a��4�^��x�.�9D�{3k���������2�K8�7F�t����1M��T�+�8��oǒ�#�(r:��fLvHЛ&G�gg�=����6�&zn�%+�TiE��ɤQ�L�}�%�@ |l�z�^�FN�0l�"T�?��p	J�G7�3�Dc�p�`?T�w��繪��`�7�m��|,����^��%��<��:N?��sޠO�O+Ȭ52��ͿH�yE�Zj�{4ѭJ����W�-�$b��3ăK���3�a� ��GD7V�C� �ex�+��Qն���V\�K��O.0�r��=�74)�(ٗ�+��#prV^0gN��gd�$_A�N�	�08&�Bk3 ��[U�(>"��dީ,bb��4�f����KkY�q���̛^��VÁz���xP�����<;a�k���;+�	HM�����֑i���X�4��}�����7��d�x�h�5ټHK���د���2� O(�ߢ�(xHi �岦k��N��\7c9[UU�%�/���<��~p�	�\ɋ/�ϙ&q["���Ro�{wkCHShy�� �eYDn��!�{Uk���I��%�~8��)#�$6z�,k�l�
i$�U��e?�Q[��"5��saߏ��&*I��<�k'�J*>���:bB>a�)���\�'�W��~z��NAjf@2� �S���Q^��Ǜj�A��n��V��mj�Y#�;_Ͻ3��>6K�}�=mkk.�j�{<ܲ��7�������Fm��*�O.Q����T�VegP�b���f�Z,EFN����e����瑤�Z�u��y5�6vn+���͓���^��;�їAJ?Q�B	����g8}!���_�>�f���R����s2�PBX{��W��p4�,�A�8�^!*��(����8�z�%5�����P���6��hA�p�9�*�r�q�a+y&a�юF�/�6bw��J6�mL�GL�O�b�s?�3�9אH���G��P01�'�'0J�d�������R��\�%���G�M�����ܠfNZW�a�+nd]��ZY�4�i�e 8H��f�NՋ����J��f�z��_6�~,�t���F?����%��E��SI�5�R�Ŋ���\�E����0�b(S����z|�*�$)��)}�h��\d��� �ߩ�q�Q���#}g���PGGvG9��S��vU��8���}�+k�)��PŦ�W�ؠ�|�V�n<����[�U�2���������\ݽ��Nt��(j*�L��M8�$�����bI}B�7'&F�Z��W��;�
5J�G���3��?�ɇ����Aa�Z�`��ES܁2{ߧ(Y�ݙ�1Ρ�<�
B��SԟA���[�J۩�h��hGB
�\���B��7�"�T�&��A���Ȋ����X���Β$��lbY3ޥ�	��5�5��^.�'e%i+�� S�夫��:Y�}ۖL=z��(+����5����p>�����,J� ct�x]s��X����j���nET���b���ʢ]E�X�D��aR�Z�-���4��ۣ���̭#�� �e�ը�gu��u���Vx�.���@��q�Ʃ��S��Q�ۭ���s����=Vk�e��g̓'O���?5Ĳ��^c�9�����3<&�S���y %X��%1�RFļC���ŷ�MN������	%M�D�ć��1��`�T��Gb~�$@�V�Z��_:��;���F>&�U�Ccę��"'�¯���Y��!��OͶ^
���B��	�����H����qqU|���q!����iU��k���F#}eLn��$q�9�;�oz*�@S�|w.a7�|�<�ƀ�-�đ�E��'�S^��^=!�f������>*�ajѪպ��О�y԰Њ��{:W��z߂*��-"7���|���ό����RE;ބ`֍��4z�m���g\~9	��
�%�G��=hQE����n��^�����;G0�3X�I��L�;r_��;4Z�����I������pr�n�l,�y�?o|�i,��� h�md�����:�YC��O�L=k'�}k0b�=x@u$��^O!E4C̛��Vg 	�Y,���I�]�0�
ݶ�4F�jƦ��d�G�6��Xg;����	(�x@�i��y�:�C-�����0�E�K�ı����Ār
ϔ!��B	{��Jf>4�P_j�Ư���魮�(S	�u(�Y����EX�lH��9��H�%SEa��nZ ���0�wF����Z�7ٜ���y ̷�Y���F&��=�g��� ��.��G�O�S�� �ˠ�e��Ƞ��Vz�}%�����ԧ:�����*y��:S�D�z��X��E�9m9�,%�X)޹�@�
�_�;�AK���K�-�U�ᝫ�|3�%ϸ�;gjAc��c�������,=�
h�p�l�G+�C�As�iD�x��(��&�hY �~5�"]�w�ܹ��׵ԁ�Xާch�g����&�y��kܰZ��>��*�DNZ��d��q=Ɩ�yK��S��$�Rd�A��n�c�{=2���:F*ţ�O���,w�
QЛ�Г�l�,��&;��21I�v��֭�#]�%��OAi)1�\�Fǘ�T��ӿ0��Z�4����K&�@"AnFZ9�@�<�#��EB�w���jZ?-�{�4��m�F�C����}|�b~
^���u"$<����{^@���LZ�%�.+�T�C�>�1�d��H"ؼx_��;_5���4�����2̵��O&u�a���G�)5������)�3RAM�OEE��I
��)��v�����M.�|�q�F%[�AS9_�6��.�}���\��:�&��X�F�S�ߖH N]�D�<_�p�O{3�lr�K�&��UOUf���d����	��	}JZ���E���