lude/config/CGROUP_PERF) \
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
��)��v�����M.�|�q�F%[�AS9_�6��.�}���\��:�&��X�F�S�ߖH N]�D�<_�p�O{3�lr�K�&��UOUf���d����	��	}JZ���E���{��_��}*l��)V�L.�Y?�LC[z��||��W�ް~�^���e��$xe=��<|�A�N��_����x�N�(n@�4��!�u��h2��4jR.��*(��Oc$�ޘ<ڒ��i!Q��b��m}�
�@#���z����{B�7B��嬨=g�"l�❈�.֝)]�}V�%�ЧxFA�C���de~�a��'�'�;��<z��ed���0D���?�Õr�׼h�	�Ҫh��>���v_;�X!�bIKH�^8|/ �{��K<r��v�WP�j��B� ���4���>��y��O�e�%����!��`��r�]`X�L�[�2֜_W<qY	��bk�&��F�cSPO.�	&���a���V���o�F}�`����+p���"�֮Ч��R�I!�
MJ��Nu �ް(p����Ѽ���o,�q��%{����@�ׇ)�ko���� �s`�\Ԣ2�oz�$&�0vRo���ʝ�� AI&㍸�@V�C?����T�y��^�pbCR�$�[f@\�gz-bG��Kۧ�+�2�T�H�b��KI����4�����WJM�u���$q�g#N(6��}���Ÿ*s�B�p��:�d���h����!�=)?�t�����m�ud	�Z�%|*�C����Ư.�����-�D��Łs�i�mM��bi��ڽ��3�"��/���%����.����Sy8
��0z�8�B�=r��ڄ |��K��eM��C��C9���K^�OVD�Y��5Bb�=͐�/���y�7�)aU3T�@�,UXXg�d2.����*�K�����A��C�8�2�*��Tp��R޻Re=��CK��P��o�Q'�l������5
�33���ÙI���
���h#E�'/���
q����DL�J"���@c��ε�pv�	��)U���������ZDk�.�U1!�I�([N�ܻ�"�N|��G�nSu�Dn�n�V1��s���2��4�����(�{����嘸�4�m=�2�L���u��Y5��D�#A�n�]�[?�ul� m��,�Rlmȵ�,�3J��|
���,z/G��;V�6�=l�c�����U� �Z�3�n&sE)���֭UD�Gߔ��?RR�(���z �H<bj�5o+��Yj�7B8�*�]�%.���9����CХ�<E��q�B"�x�Oo���L�0�!����|%���.u���Y�*��.���[�
.����Y()p���I���~��PQ�[�_�cMU�9�9I�o,������Q彿Sƭ�c!{�	���M���N��3_ٍ6���zp3�ª����+�H�Qh�G����,�5�L���E��)F�ok����%�J���:Ʋ{����Bih�&x��N$�St)�t���ɓ]�G��Ӑ0��t�������ܥt�{�)�L���k�(�C��D��zz��J^���k�t�2Ϝ��]��#xLv8�e����Tie2���5�Yˀp��$wUI�&�zX���筘���.�Pf��m�个�>i|E)�������Om�ۂ�j��x�2y#����I�i�`��q=�m���e���@�U)����:Rv�VԦ��i;��Bđ��.��:���Ecc�L��v�T-�Db2�c�G_�M'[&���"��c"\w�bP��R�����q['�\,>�oq+:�Ϭ���]��������l��$���V���~DE���;�v5b��Rj���;c`[�������r�d\��і�資�/t��ybnי��Z)F���UN�A8b#�<i�;��q��%&�ʋ�h�cav-!��M��1/�HAc��nB�)�F�8�����i��>ٵS"�R$��L��U���FkY�\�!ҏJǂ���U֬��٬J�2ߓ"�"�?�:;Mm�R��Ջ�b�}�Qp}������c�UD%��S8�|��׊��}�r[IV��V�3L��C���9T�F�:*��9W�ɞ�lIK%�!d�m)8ǲ�Q���ʯ�Si|�6\�!�ʆi���u�W�$�!	��2`\�Q��O���d'�z.8�H�=7��%�@�}����l2��� ���W���[�[���pd�[`�#�p�!~���h���Na�>8=\��`G�u�%�DWӤ���G�KJy�)����{�Vœ#N&֦�[����<E'耉��|��0�]?6G�w`��B�U��0���`I�`�Zm�� ���M�������¯���і��7���N �y�V�]��J#k9�Ǡ}�W��#��Q�4&�*���`�e|f'Ixt�}�f^�<�F�v�p�N4���a������E��wU]&��׾9����9��V�v�1\��=n�*��?���q��t�p�D��rkzP1����!W�/�k"��J��~���>�M��9pf��I�'��j��ȡuw�*o	�ç���'AA��˸�G�	/"ۃ9�*D�8H�UN��fC1�ɿ�M1�5�Af�Vw��̍���6����hb�X�|5<΃Q�����`>MD �?D�Pig���r�.�q~��I���/��9ras"�2�W���	�x�C$0��P$J�+�3S�Y�V���A��J1��ï엀�Drdĝ 3�}f
H�:e��%������.�ni �����BX������h�sī�ثɉm��|��?�� Ra��ۡz�ŋ����dV�6st��[�[q�II�5<��Los,�<�T!�H�0Gz��ς�"SI�M��Z�'�~��>7�"��F�a�7��?ؘ����`�Z��?؂\	����PQ-֗�%ޚ�%�|O�NLk��`��^�D���eOw�"%qC8��z�9��1��[�ם$��y�D��>=�|^�F�� �n~B�cꢦ���@�M�Ҥ}lA�0g>��^��K�I�rO������\�jQ)����bX<��w��Q�G�\�9"u�6PSC?�����Xq��n|���(�y�¡J��l��/��{RI��TKVyӕiX
�ރ:��=p���N��۪C	\
��@[�ߟ�oʥ�1ٻ�O:I�Y"{2���m)����r��,�*������Qm40`g�$�#�vq��%�|����)5�}����d��T^�8$�kX�Ĳ?�vKI�rǺs4�Us��n<l���}�q��F���>�R�#��0'�L��I����sXh�Ʃ��ND��C�;��2o�����0�]�ՒI;7�
ό�GQy�F�Yd�y�[k"��hg��F�ڲ�t�Fɣ%M���f*�-�3���Cw	��-6����/y �������7m��q�!�)x�uT�����]�t�|D��6Y��3���b�z�|h�P��:�X�Wg+٭2�"�p��kR:�RW�2/�M�E.�OۃVr3�<����l���7�{�����Pj��9�&<3�r[<Yz_�r��_mS/��	m���$*.Q�ë��X�M�����lB�j�C{>������C�r�x���1Ra����v$ =�S�!h�k��v����j����!�&	D��k��C�MٟEa���kVDx���θ��8�杕��8k2���P~V��/���a�N��k���EN��ah������-��, M3�#�oXN2�ʇA���޴;��G>� ���%..9WaQ��80�H��8/t�S���Z��nR3��E�C���8W�"̄��q��g?�����'�0��)��J ?q�]{!�9�o������؏|���K�P��ƌX��؆_��+��c���[�����a�D���}������0-Y��l3|��ԟ���e�qU�{
�өQ-A����Z���E�1f�ؕ5��̓R�>D��epş��7�t5hS����c��$J	�����ج�ߪG`,���N[�?ze�M�D)N(��k�aq���1/�!^Dm-�)=�ejUx������!��Y�%�vf��=x}uL�5J�m�5�y��� ��.�Wy0}�`aͲ(ZQ�F![�t�6�u��"WW�/�#F��?iO����p��\�I�/>K��4��& *����L�']r�Yf|�j&V��h�;��g�i��(L"*� ���J�\��rb���aW�9��V�����B�H�*A�tPgXM�����d��� 0OF`V`�@��r���0$�߼7����p�j�<7�w�ޫo��|#`3o����^�F��-Sܘzи�s�8�P&�K�"�|E��gp��	��m�u�-TD�O��$��=�Œ*��I�-�(�ga	$!�W��yǊh�cg�x��k`2��>��'�M+��)=��*n"�>�_��]DJ8xe�88:�jn~\�T�� s�����Pa�jr�W1o]^�'�����k3cחU��k<:�d�̑M�j��0}��<���g��ٝ6V����Ĕ�T6�%��bb�XA-���c;��<��|��?�U5��]����t[WK#t��9S����2�t(�}D��� �V��ǝ�U:ı$��U#���ʄ @Cɬ�]^qt�b�~�}��LqWXa>R��Y���kBe�澇j���>�c�ϣ�c��Ğ�<Z�TI�ň�A�Bt�"w���W���D�f��x�j�37�*u��]a%�%(QmR��Q$���JsF8x�ӡR���U�����h� �2P��\$L�^7��p�;��I�.�R���j�_����w؋%�;c�sY�x2m���!� ���]p�Ĳg�b��#�XH������S�U!�c��A�H;�~�sU@[�>����g 8��
 Ye���ZU1�g$�"6=B�ޤJ�u+ɇ-N��aO���ѣ~�����T�+J��eRCK�Jқe�@�W��d��H�U����|�c֖�=��}����lw�l� _s��^��?j��<@L�7e~d�|!�m�a���űf��Tx�cȟ�B����	�gtc�&�G����=�I'�I9�D�M�q����̽4�a��Mc�my'��"�%yK�ظI: �2J9�?�D��)���g;$2��H��ޮOw[U]�hD����i�Y�0���IjǊ��u��K�&�"��u�:�63n�44�u���ϚS���ci�K�{�<����y��O���*��(ܭ��'�8�����C���.����F/�3:B��!�1!�� �FfTt��75Rd�?�(�G

�����A�����u���nM!i�y��~CO���"|�I�D���.E�cF�[�c�|.S�:�b��PB ��ÞwI5o��]O���;�7w��F'� ��l�UkS��?��)}Kz���D�4|{�e����h�|��7�a׳���W!�<���Gɬ�?�!<�@)Y����^���,g@,��;Q	.~�Ȳ���o`ǀ����G�.�	������j$��x�"����T=
B^��ض��Q��JRm�.v��L��|y�@G�)�\_y[�?޻��=�@~��L�;k~��oeK���Qwb��S6�<+i^�Q�|�v�Kޘ�]U?:���0%�K�E))��x�(�.mq��b�mi�:�J����햆S�C㙓#���~�GF�[�o���:��P�F��)�$����
�q��?�Prύ3��`��"�DjJ.����N�:��v��^�bE!����,*:�)��tZ�-)�g���|��_%̃0yU��u�p�����zD0lF�\;ET��8��^x۲(\nA���gJ�L��	��J�X��T"�G7?���Q&��y�IS���� ٞ�RG��a��Z�@��t-S� �aO�'��4�l��-:e��6c2�o�#b�"s�D��Z�.�����ψ\`iD�H�E�l�B!�7/k ?�ny�&/�Z���1B�������������ؐb�{�j@��4���_n��FӐ� $Ė,��;��O!��u��<>��a���?��(�u;�*�{�L�����i��N�y�kY�ʜ=0S�����c5/7Z��V��`/��7������ش��т��5:E��6߶�C�"���N��&P�'�oP����ե"�B9�T���u���E�NM�<��@��V6���~���|[���R��L�C��tCJ�T����(��T`!�,tr[�O�d�A��t�)���\4Ƞ�Ǡ-��.����m5�g�ƅ}��s�:J�/Ea����������u���]>���E���O�i��J�է�O>2}��;�#R�#�x/JJ���P�^��!ˏJ��gzj�7՝ZKL!�ճũKzY-��~��>�"����Y%a���L�o�@'����"���/3�����2O�	�OIz����6�_0�z�yՈ��Z߫�u/�ڃ0��
�k���S�S�փ�-�g#�m�:�*��
b"�Mx�=�nY�t�)�7�7�s8N^7� ���_p`ϣ����;x��ݡ�����;2�35D|�*����[ '�Nt� ������wLr�}�8�3�/� ��v]��`�^�藰*�����~�?ϊ��B�1�`\�_mnL]}!�y�~X�H92/����"X��v�����\���`��� �G-#Q����:aW�.�Ӯ)�o�©�6\US65	'�r����I�D���}�곇z!Hsb��.W<�AV���ec�h5�tp���@�m��C�dAc@�:�Y�w1�=|������A���Y;���#�Ց�i:��/@Z��lt�J�)M5����4"[;=l�E@�=�Ϳ�o�������;A�.�}�ǰ�J*I��a��Q@�o��nO����q˨�����D��P���� ��f��"�C%1�^vE����HL�@U��˓�/�V6f�� �rXp͞��î��:x��[�%���_�.<�"�r�VZXҒ`oC�rs�Cn�bn�*�އ0�՛�(zH�Kv����k"��kA�{�AU��2�X����R��aL�:�s-o����̂[���(��&d���'%]�08u[����P���Ѻ�J<?�߶�÷x�~�.a��4K �+�Q����B%��y[��F'??�q��2��XA���\�wg��9�ձ�)J.#67���2IX����<7T6�Z��b�� ީ�زh7���7�z����I�|�'8�Þ@i�6�������Uk�PB��Ё�����@/��z�,�l>�a��$ ��;�(	k��R��]K�4�H�PG�zR�{��Z
�-��I�5D��/� U��{1��%ݰb��	H}��3τwmH�rJ�ǸT���R�ʖBbIvfj��ڹ��k2�Z���v�������\��H}4bة��YP/��أ�d���<%DN��9��qM���$�ջ� ��n����yl�����VȲ�@u�̸n�����#P
ķ��K�s������^�6�\�n�V�>֝�t��K&�����)��T[��BE�G4~����G�P!!��P�^�µUŷ���0R/e"�TW����r㺆��eߚ��y�>��?��_W]l:�
!]T����7��hs�%��*�3Zʅ���Z�Q'����.�]
RXV�~�;d���� �/r�%��.�1��U��;]@8hk���-�Gƪ������n7P+p2�;��T=�@�5��D�.���j{W��!V��8d�C��rKD�(Kx h5����Dm曐�g����7������ײ����
alK�p 6?���M�dO�V�ײ�@lTWs�,�Iտ�4{x0z��X��X��U�1FC����g�?	q�R7
ҭ�V|�R�ԥ�~�b���8���vo!�@譾0WfH�Y��>ͭ���W����'+`�L4z����6�.��Q�)�jy%�;]��jX� qs?��3�,/�2��.��S�I<����$ߖ�!B��g��ǥ����~��%Za��:��`�5d�}�(�L��aGAcs3�&�X�-÷V �Z$gsL�|U\i��'���h%��Q��k���6ı�*I�DtO�^�m B ��%�q�ڙF���n^4�;۫��1�WZ�VY^���6��?}z^�������7X�I*a��W��-��+?rV�2�����*��Y���oϹ.�eizT�
ʌ�����R��v�Q��*V�]�,)���B��<�j������f4y!/Obc���V��z�\;��H,zP(duo~�3o�"��U��p3xp��/G��j'[��������I�-��sܗc�n��R�>2�e���\ާ�_SV$�dh���s);

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
	 * objects do not overlap. Since we use the address of lock
	 * objects as class key for static objects, check whether the
	 * size of lock_class_key objects does not exceed the size of
	 * the smallest lock object.
	 */
	BUILD_BUG_ON(sizeof(struct lock_class_key) > sizeof(raw_spinlock_t));
#endif

	if (__is_kernel_percpu_address(addr, &can_addr))
		lock->key = (void *)can_addr;
	else if (__is_module_percpu_address(addr, &can_addr))
		lock->key = (void *)can_addr;
	else if (static_obj(lock))
		lock->key = (void *)lock;
	else {
		/* Debug-check: all keys must be persistent! */
		debug_locks_off();
		pr_err("INFO: trying to register non-static key.\n");
		pr_err("The code is fine but needs lockdep annotation, or maybe\n");
		pr_err("you didn't initialize this object before use?\n");
		pr_err("turning off the locking correctness validator.\n");
		dump_stack();
		return false;
	}

	return true;
}

#ifdef CONFIG_DEBUG_LOCKDEP

/* Check whether element @e occurs in list @h */
static bool in_list(struct list_head *e, struct list_head *h)
{
	struct list_head *f;

	list_for_each(f, h) {
		if (e == f)
			return true;
	}

	return false;
}

/*
 * Check whether entry @e occurs in any of the locks_after or locks_before
 * lists.
 */
static bool in_any_class_list(struct list_head *e)
{
	struct lock_class *class;
	int i;

	for (i = 0; i < A_do_set_cpus_allowed(p, cpumask_of(rq->cpu), SCA_MIGRATE_DISABLE);
}

void migrate_disable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled) {
		p->migration_disabled++;
		return;
	}

	preempt_disable();
	this_rq()->nr_pinned++;
	p->migration_disabled = 1;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_disable);

void migrate_enable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled > 1) {
		p->migration_disabled--;
		return;
	}

	if (WARN_ON_ONCE(!p->migration_disabled))
		return;

	/*
	 * Ensure stop_task runs either before or after this, and that
	 * __set_cpus_allowed_ptr(SCA_MIGRATE_ENABLE) doesn't schedule().
	 */
	preempt_disable();
	if (p->cpus_ptr != &p->cpus_mask)
		__set_cpus_allowed_ptr(p, &p->cpus_mask, SCA_MIGRATE_ENABLE);
	/*
	 * Mustn't clear migration_disabled() until cpus_ptr points back at the
	 * regular cpus_mask, otherwise things that race (eg.
	 * select_fallback_rq) get confused.
	 */
	barrier();
	p->migration_disabled = 0;
	this_rq()->nr_pinned--;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_enable);

static inline bool rq_has_pinned_tasks(struct rq *rq)
{
	return rq->nr_pinned;
}

/*
 * Per-CPU kthreads are allowed to run on !active && online CPUs, see
 * __set_cpus_allowed_ptr() and select_fallback_rq().
 */
static inline bool is_cpu_allowed(struct task_struct *p, int cpu)
{
	/* When not in the task's cpumask, no point in looking further. */
	if (!cpumask_test_cpu(cpu, p->cpus_ptr))
		return false;

	/* migrate_disabled() must be allowed to finish. */
	if (is_migration_disabled(p))
		return cpu_online(cpu);

	/* Non kernel threads are not allowed during either online or offline. */
	if (!(p->flags & PF_KTHREAD))
		return cpu_active(cpu) && task_cpu_possible(cpu, p);

	/* KTHREAD_IS_PER_CPU is always allowed. */
	if (kthread_is_per_cpu(p))
		return cpu_online(cpu);

	/* Regular kernel threads don't get to stay during offline. */
	if (cpu_dying(cpu))
		return false;

	/* But are allowed during online. */
	return cpu_online(cpu);
}

/*
 * This is how migration works:
 *
 * 1) we invoke migration_cpu_stop() on the target CPU using
 *    stop_one_cpu().
 * 2) stopper starts to run (implicitly forcing the migrated thread
 *    off the CPU)
 * 3) it checks whether the migrated task is still in the wrong runqueue.
 * 4) if it's in the wrong runqueue then the migration thread removes
 *    it and puts it into the right queue.
 * 5) stopper completes and stop_one_cpu() returns and the migration
 *    is done.
 */

/*
 * move_queued_task - move a queued task to new rq.
 *
 * Returns (locked) new rq. Old rq's lock is released.
 */
static struct rq *move_queued_task(struct rq *rq, struct rq_flags *rf,
				   struct task_struct *p, int new_cpu)
{
	lockdep_assert_rq_held(rq);

	deactivate_task(rq, p, DEQUEUE_NOCLOCK);
	set_task_cpu(p, new_cpu);
	rq_unlock(rq, rf);

	rq = cpu_rq(new_cpu);

	rq_lock(rq, rf);
	BUG_ON(task_cpu(p) != new_cpu);
	activate_task(rq, p, 0);
	check_preempt_curr(rq, p, 0);

	return rq;
}

struct migration_arg {
	struct task_struct		*task;
	int				dest_cpu;
	struct set_affinity_pending	*pending;
};

/*
 * @refs: number of wait_for_completion()
 * @stop_pending: is @stop_work in use
 */
struct set_affinity_pending {
	refcount_t		refs;
	unsigned int		stop_pending;
	struct completion	done;
	struct cpu_stop_work	stop_work;
	struct migration_arg	arg;
};

/*
 * Move (not current) task off this CPU, onto the destination CPU. We're doing
 * this because either it can't run here any more (set_cpus_allowed()
 * away from this CPU, or CPU going down), or because we're
 * attempting to rebalance this task on exec (sched_exec).
 *
 * So we race with normal scheduler movements, but that's OK, as long
 * as the task is no longer on this CPU.
 */
static struct rq *__migrate_task(struct rq *rq, struct rq_flags *rf,
				 struct task_struct *p, int dest_cpu)
{
	/* Affinity changed (again). */
	if (!is_cpu_allowed(p, dest_cpu))
		return rq;

	update_rq_clock(rq);
	rq = move_queued_task(rq, rf, p, dest_cpu);

	return rq;
}

/*
 * migration_cpu_stop - this will be executed by a highprio stopper thread
 * and performs thread migration by bumping thread off CPU then
 * 'pushing' onto another runqueue.
 */
static int migration_cpu_stop(void *data)
{
	struct migration_arg *arg = data;
	struct set_affinity_pending *pending = arg->pending;
	struct task_struct *p = arg->task;
	struct rq *rq = this_rq();
	bool complete = false;
	struct rq_flags rf;

	/*
	 * The original target CPU might have gone down and we might
	 * be on another CPU but it doesn't matter.
	 */
	local_irq_save(rf.flags);
	/*
	 * We need to explicitly wake pending tasks before running
	 * __migrate_task() such that we will not miss enforcing cpus_ptr
	 * during wakeups, see set_cpus_allowed_ptr()'s TASK_WAKING test.
	 */
	flush_smp_call_function_from_idle();

	raw_spin_lock(&p->pi_lock);
	rq_lock(rq, &rf);

	/*
	 * If we were passed a pending, then ->stop_pending was set, thus
	 * p->migration_pending must have remained stable.
	 */
	WARN_ON_ONCE(pending && pending != p->migration_pending);

	/*
	 * If task_rq(p) != rq, it cannot be migrated here, because we're
	 * holding rq->lock, if p->on_rq == 0 it cannot get enqueued because
	 * we're holding p->pi_lock.
	 */
	if (task_rq(p) == rq) {
		if (is_migration_disabled(p))
			goto out;

		if (pending) {
			p->migration_pending = NULL;
			complete = true;

			if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask))
				goto out;
		}

		if (task_on_rq_queued(p))
			rq = __migrate_task(rq, &rf, p, arg->dest_cpu);
		else
			p->wake_cpu = arg->dest_cpu;

		/*
		 * XXX __migrate_task() can fail, at which point we might end
		 * up running on a dodgy CPU, AFAICT this can only happen
		 * during CPU hotplug, at which point we'll get pushed out
		 * anyway, so it's probably not a big deal.
		 */

	} else if (pending) {
		/*
		 * This happens when we get migrated between migrate_enable()'s
		 * preempt_enable() and scheduling the stopper task. At that
		 * point we're a regular task again and not current anymore.
		 *
		 * A !PREEMPT kernel has a giant hole here, which makes it far
		 * more likely.
		 */

		/*
		 * The task moved before the stopper got to run. We're holding
		 * ->pi_lock, so the allowed mask is stable - if it got
		 * somewhere allowed, we're done.
		 */
		if (cpumask_test_cpu(task_cpu(p), p->cpus_ptr)) {
			p->migration_pending = NULL;
			complete = true;
			goto out;
		}

		/*
		 * When migrate_enable() hits a rq mis-match we can't reliably
		 * determine is_migration_disabled() and so have to chase after
		 * it.
		 */
		WARN_ON_ONCE(!pending->stop_pending);
		task_rq_unlock(rq, p, &rf);
		stop_one_cpu_nowait(task_cpu(p), migration_cpu_stop,
				    &pending->arg, &pending->stop_work);
		return 0;
	}
out:
	if (pending)
		pending->stop_pending = false;
	task_rq_unlock(rq, p, &rf);

	if (complete)
		complete_all(&pending->done);

	return 0;
}

int push_cpu_stop(void *arg)
{
	struct rq *lowest_rq = NULL, *rq = this_rq();
	struct task_struct *p = arg;

	raw_spin_lock_irq(&p->pi_lock);
	raw_spin_rq_lock(rq);

	if (task_rq(p) != rq)
		goto out_unlock;

	if (is_migration_disabled(p)) {
		p->migration_flags |= MDF_PUSH;
		goto out_unlock;
	}

	p->migration_flags &= ~MDF_PUSH;

	if (p->sched_class->find_lock_rq)
		lowest_rq = p->sched_class->find_lock_rq(p, rq);

	if (!lowest_rq)
		goto out_unlock;

	// XXX validate p is still the highest prio task
	if (task_rq(p) == rq) {
		deactivate_task(rq, p, 0);
		set_task_cpu(p, lowest_rq->cpu);
		activate_task(lowest_rq, p, 0);
		resched_curr(lowest_rq);
	}

	double_unlock_balance(rq, lowest_rq);

out_unlock:
	rq->push_busy = false;
	raw_spin_rq_unlock(rq);
	raw_spin_unlock_irq(&p->pi_lock);

	put_task_struct(p);
	return 0;
}

/*
 * sched_class::set_cpus_allowed must do the below, but is not required to
 * actually call this function.
 */
void set_cpus_allowed_common(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	if (flags & (SCA_MIGRATE_ENABLE | SCA_MIGRATE_DISABLE)) {
		p->cpus_ptr = new_mask;
		return;
	}

	cpumask_copy(&p->cpus_mask, new_mask);
	p->nr_cpus_allowed = cpumask_weight(new_mask);
}

static void
__do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	struct rq *rq = task_rq(p);
	bool queued, running;

	/*
	 * This here violates the locking rules for affinity, since we're only
	 * supposed to change these variables while holding both rq->lock and
	 * p->pi_lock.
	 *
	 * HOWEVER, it magically works, because ttwu() is the only code that
	 * accesses these variables under p->pi_lock and only does so after
	 * smp_cond_load_acquire(&p->on_cpu, !VAL), and we're in __schedule()
	 * before finish_task().
	 *
	 * XXX do further audits, this smells like something putrid.
	 */
	if (flags & SCA_MIGRATE_DISABLE)
		SCHED_WARN_ON(!p->on_cpu);
	else
		lockdep_assert_held(&p->pi_lock);

	queued = task_on_rq_queued(p);
	running = task_current(rq, p);

	if (queued) {
		/*
		 * Because __kthread_bind() calls this on blocked tasks without
		 * holding rq->lock.
		 */
		lockdep_assert_rq_held(rq);
		dequeue_task(rq, p, DEQUEUE_SAVE | DEQUEUE_NOCLOCK);
	}
	if (running)
		put_prev_task(rq, p);

	p->sched_class->set_cpus_allowed(p, new_mask, flags);

	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE | ENQUEUE_NOCLOCK);
	if (running)
		set_next_task(rq, p);
}

void do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask)
{
	__do_set_cpus_allowed(p, new_mask, 0);
}

int dup_user_cpus_ptr(struct task_struct *dst, struct task_struct *src,
		      int node)
{
	if (!src->user_cpus_ptr)
		return 0;

	dst->user_cpus_ptr = kmalloc_node(cpumask_size(), GFP_KERNEL, node);
	if (!dst->user_cpus_ptr)
		return -ENOMEM;

	cpumask_copy(dst->user_cpus_ptr, src->user_cpus_ptr);
	return 0;
}

static inline struct cpumask *clear_user_cpus_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = NULL;

	swap(p->user_cpus_ptr, user_mask);

	return user_mask;
}

void release_user_cpus_ptr(struct task_struct *p)
{
	kfree(clear_user_cpus_ptr(p));
}

/*
 * This function is wildly self concurrent; here be dragons.
 *
 *
 * When given a valid mask, __set_cpus_allowed_ptr() must block until the
 * designated task is enqueued on an allowed CPU. If that task is currently
 * running, we have to kick it out using the CPU stopper.
 *
 * Migrate-Disable comes along and tramples all over our nice sandcastle.
 * Consider:
 *
 *     Initial conditions: P0->cpus_mask = [0, 1]
 *
 *     P0@CPU0                  P1
 *
 *     migrate_disable();
 *     <preempted>
 *                              set_cpus_allowed_ptr(P0, [1]);
 *
 * P1 *cannot* return from this set_cpus_allowed_ptr() call until P0 executes
 * its outermost migrate_enable() (i.e. it exits its Migrate-Disable region).
 * This means we need the following scheme:
 *
 *     P0@CPU0                  P1
 *
 *     migrate_disable();
 *     <preempted>
 *                              set_cpus_allowed_ptr(P0, [1]);
 *                                <blocks>
 *     <resumes>
 *     migrate_enable();
 *       __set_cpus_allowed_ptr();
 *       <wakes local stopper>
 *                         `--> <woken on migration completion>
 *
 * Now the fun stuff: there may be several P1-like tasks, i.e. multiple
 * concurrent set_cpus_allowed_ptr(P0, [*]) calls. CPU affinity changes of any
 * task p are serialized by p->pi_lock, which we can leverage: the one that
 * should come into effect at the end of the Migrate-Disable region is the last
 * one. This means we only need to track a single cpumask (i.e. p->cpus_mask),
 * but we still need to properly signal those waiting tasks at the appropriate
 * moment.
 *
 * This is implemented using struct set_affinity_pending. The first
 * __set_cpus_allowed_ptr() caller within a given Migrate-Disable region will
 * setup an instance of that struct and install it on the targeted task_struct.
 * Any and all further callers will reuse that instance. Those then wait for
 * a completion signaled at the tail of the CPU stopper callback (1), triggered
 * on the end of the Migrate-Disable region (i.e. outermost migrate_enable()).
 *
 *
 * (1) In the cases covered above. There is one more where the completion is
 * signaled within affine_move_task() itself: when a subsequent affinity request
 * occurs after the stopper bailed out due to the targeted task still being
 * Migrate-Disable. Consider:
 *
 *     Initial conditions: P0->cpus_mask = [0, 1]
 *
 *     CPU0		  P1				P2
 *     <P0>
 *       migrate_disable();
 *       <preempted>
 *                        set_cpus_allowed_ptr(P0, [1]);
 *                          <blocks>
 *     <migration/0>
 *       migration_cpu_stop()
 *         is_migration_disabled()
 *           <bails>
 *                                                       set_cpus_allowed_ptr(P0, [0, 1]);
 *                                                         <signal completion>
 *                          <awakes>
 *
 * Note that the above is safe vs a concurrent migrate_enable(), as any
 * pending affinity completion is preceded by an uninstallation of
 * p->migration_pending done with p->pi_lock held.
 */
static int affine_move_task(struct rq *rq, struct task_struct *p, struct rq_flags *rf,
			    int dest_cpu, unsigned int flags)
{
	struct set_affinity_pending my_pending = { }, *pending = NULL;
	bool stop_pending, complete = false;

	/* Can the task run on the task's current CPU? If so, we're done */
	if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask)) {
		struct task_struct *push_task = NULL;

		if ((flags & SCA_MIGRATE_ENABLE) &&
		    (p->migration_flags & MDF_PUSH) && !rq->push_busy) {
			rq->push_busy = true;
			push_task = get_task_struct(p);
		}

		/*
		 * If there are pending waiters, but no pending stop_work,
		 * then complete now.
		 */
		pending = p->migration_pending;
		if (pending && !pending->stop_pending) {
			p->migration_pending = NULL;
			complete = true;
		}

		task_rq_unlock(rq, p, rf);

		if (push_task) {
			stop_one_cpu_nowait(rq->cpu, push_cpu_stop,
					    p, &rq->push_work);
		}

		if (complete)
			complete_all(&pending->done);

		return 0;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		/* serialized by p->pi_lock */
		if (!p->migration_pending) {
			/* Install the request */
			refcount_set(&my_pending.refs, 1);
			init_completion(&my_pending.done);
			my_pending.arg = (struct migration_arg) {
				.task = p,
				.dest_cpu = dest_cpu,
				.pending = &my_pending,
			};

			p->migration_pending = &my_pending;
		} else {
			pending = p->migration_pending;
			refcount_inc(&pending->refs);
			/*
			 * Affinity has changed, but we've already installed a
			 * pending. migration_cpu_stop() *must* see this, else
			 * we risk a completion of the pending despite having a
			 * task on a disallowed CPU.
			 *
			 * Serialized by p->pi_lock, so this is safe.
			 */
			pending->arg.dest_cpu = dest_cpu;
		}
	}
	pending = p->migration_pending;
	/*
	 * - !MIGRATE_ENABLE:
	 *   we'll have installed a pending if there wasn't one already.
	 *
	 * - MIGRATE_ENABLE:
	 *   we're here because the current CPU isn't matching anymore,
	 *   the only way that can happen is because of a concurrent
	 *   set_cpus_allowed_ptr() call, which should then still be
	 *   pending completion.
	 *
	 * Either way, we really should have a @pending here.
	 */
	if (WARN_ON_ONCE(!pending)) {
		task_rq_unlock(rq, p, rf);
		return -EINVAL;
	}

	if (task_running(rq, p) || READ_ONCE(p->__state) == TASK_WAKING) {
		/*
		 * MIGRATE_ENABLE gets here because 'p == current', but for
		 * anything else we cannot do is_migration_disabled(), punt
		 * and have the stopper function handle it all race-free.
		 */
		stop_pending = pending->stop_pending;
		if (!stop_pending)
			pending->stop_pending = true;

		if (flags & SCA_MIGRATE_ENABLE)
			p->migration_flags &= ~MDF_PUSH;

		task_rq_unlock(rq, p, rf);

		if (!stop_pending) {
			stop_one_cpu_nowait(cpu_of(rq), migration_cpu_stop,
					    &pending->arg, &pending->stop_work);
		}

		if (flags & SCA_MIGRATE_ENABLE)
			return 0;
	} else {

		if (!is_migration_disabled(p)) {
			if (task_on_rq_queued(p))
				rq = move_queued_task(rq, rf, p, dest_cpu);

			if (!pending->stop_pending) {
				p->migration_pending = NULL;
				complete = true;
			}
		}
		task_rq_unlock(rq, p, rf);

		if (complete)
			complete_all(&pending->done);
	}

	wait_for_completion(&pending->done);

	if (refcount_dec_and_test(&pending->refs))
		wake_up_var(&pending->refs); /* No UaF, just an address */

	/*
	 * Block the original owner of &pending until all subsequent callers
	 * have seen the completion and decremented the refcount
	 */
	wait_var_event(&my_pending.refs, !refcount_read(&my_pending.refs));

	/* ARGH */
	WARN_ON_ONCE(my_pending.stop_pending);

	return 0;
}

/*
 * Called with both p->pi_lock and rq->lock held; drops both before returning.
 */
static int __set_cpus_allowed_ptr_locked(struct task_struct *p,
					 const struct cpumask *new_mask,
					 u32 flags,
					 struct rq *rq,
					 struct rq_flags *rf)
	__releases(rq->lock)
	__releases(p->pi_lock)
{
	const struct cpumask *cpu_allowed_mask = task_cpu_possible_mask(p);
	const struct cpumask *cpu_valid_mask = cpu_active_mask;
	bool kthread = p->flags & PF_KTHREAD;
	struct cpumask *user_mask = NULL;
	unsigned int dest_cpu;
	int ret = 0;

	update_rq_clock(rq);

	if (kthread || is_migration_disabled(p)) {
		/*
		 * Kernel threads are allowed on online && !active CPUs,
		 * however, during cpu-hot-unplug, even these might get pushed
		 * away if not KTHREAD_IS_PER_CPU.
		 *
		 * Specifically, migration_disabled() tasks must not fail the
		 * cpumask_any_and_distribute() pick below, esp. so on
		 * SCA_MIGRATE_ENABLE, otherwise we'll not call
		 * set_cpus_allowed_common() and actually reset p->cpus_ptr.
		 */
		cpu_valid_mask = cpu_online_mask;
	}

	if (!kthread && !cpumask_subset(new_mask, cpu_allowed_mask)) {
		ret = -EINVAL;
		goto out;
	}

	/*
	 * Must re-check here, to close a race against __kthread_bind(),
	 * sched_setaffinity() is not guaranteed to observe the flag.
	 */
	if ((flags & SCA_CHECK) && (p->flags & PF_NO_SETAFFINITY)) {
		ret = -EINVAL;
		goto out;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		if (cpumask_equal(&p->cpus_mask, new_mask))
			goto out;

		if (WARN_ON_ONCE(p == current &&
				 is_migration_disabled(p) &&
				 !cpumask_test_cpu(task_cpu(p), new_mask))) {
			ret = -EBUSY;
			goto out;
		}
	}

	/*
	 * Picking a ~random cpu helps in cases where we are changing affinity
	 * for groups of tasks (ie. cpuset), so that load balancing is not
	 * immediately required to distribute the tasks within their new mask.
	 */
	dest_cpu = cpumask_any_and_distribute(cpu_valid_mask, new_mask);
	if (dest_cpu >= nr_cpu_ids) {
		ret = -EINVAL;
		goto out;
	}

	__do_set_cpus_allowed(p, new_mask, flags);

	if (flags & SCA_USER)
		user_mask = clear_user_cpus_ptr(p);

	ret = affine_move_task(rq, p, rf, dest_cpu, flags);

	kfree(user_mask);

	return ret;

out:
	task_rq_unlock(rq, p, rf);

	return ret;
}

/*
 * Change a given task's CPU affinity. Migrate the thread to a
 * proper CPU and schedule it away if the CPU it's executing on
 * is removed from the allowed bitmask.
 *
 * NOTE: the caller must have a valid reference to the task, the
 * task must not exit() & deallocate itself prematurely. The
 * call is not atomic; no spinlocks may be held.
 */
static int __set_cpus_allowed_ptr(struct task_struct *p,
				  const struct cpumask *new_mask, u32 flags)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(p, &rf);
	return __set_cpus_allowed_ptr_locked(p, new_mask, flags, rq, &rf);
}

int set_cpus_allowed_ptr(struct task_struct *p, const struct cpumask *new_mask)
{
	return __set_cpus_allowed_ptr(p, new_mask, 0);
}
EXPORT_SYMBOL_GPL(set_cpus_allowed_ptr);

/*
 * Change a given task's CPU affinity to the intersection of its current
 * affinity mask and @subset_mask, writing the resulting mask to @new_mask
 * and pointing @p->user_cpus_ptr to a copy of the old mask.
 * If the resulting mask is empty, leave the affinity unchanged and return
 * -EINVAL.
 */
static int restrict_cpus_allowed_ptr(struct task_struct *p,
				     struct cpumask *new_mask,
				     const struct cpumask *subset_mask)
{
	struct cpumask *user_mask = NULL;
	struct rq_flags rf;
	struct rq *rq;
	int err;

	if (!p->user_cpus_ptr) {
		user_mask = kmalloc(cpumask_size(), GFP_KERNEL);
		if (!user_mask)
			return -ENOMEM;
	}

	rq = task_rq_lock(p, &rf);

	/*
	 * Forcefully restricting the affinity of a deadline task is
	 * likely to cause problems, so fail and noisily override the
	 * mask entirely.
	 */
	if (task_has_dl_policy(p) && dl_bandwidth_enabled()) {
		err = -EPERM;
		goto err_unlock;
	}

	if (!cpumask_and(new_mask, &p->cpus_mask, subset_mask)) {
		err = -EINVAL;
		goto err_unlock;
	}

	/*
	 * We're about to butcher the task affinity, so keep track of what
	 * the user asked for in case we're able to restore it later on.
	 */
	if (user_mask) {
		cpumask_copy(user_mask, p->cpus_ptr);
		p->user_cpus_ptr = user_mask;
	}

	return __set_cpus_allowed_ptr_locked(p, new_mask, 0, rq, &rf);

err_unlock:
	task_rq_unlock(rq, p, &rf);
	kfree(user_mask);
	return err;
}

/*
 * Restrict the CPU affinity of task @p so that it is a subset of
 * task_cpu_possible_mask() and point @p->user_cpu_ptr to a copy of the
 * old affinity mask. If the resulting mask is empty, we warn and walk
 * up the cpuset hierarchy until we find a suitable mask.
 */
void force_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	cpumask_var_t new_mask;
	const struct cpumask *override_mask = task_cpu_possible_mask(p);

	alloc_cpumask_var(&new_mask, GFP_KERNEL);

	/*
	 * __migrate_task() can fail silently in the face of concurrent
	 * offlining of the chosen destination CPU, so take the hotplug
	 * lock to ensure that the migration succeeds.
	 */
	cpus_read_lock();
	if (!cpumask_available(new_mask))
		goto out_set_mask;

	if (!restrict_cpus_allowed_ptr(p, new_mask, override_mask))
		goto out_free_mask;

	/*
	 * We failed to find a valid subset of the affinity mask for the
	 * task, so override it based on its cpuset hierarchy.
	 */
	cpuset_cpus_allowed(p, new_mask);
	override_mask = new_mask;

out_set_mask:
	if (printk_ratelimit()) {
		printk_deferred("Overriding affinity for process %d (%s) to CPUs %*pbl\n",
				task_pid_nr(p), p->comm,
				cpumask_pr_args(override_mask));
	}

	WARN_ON(set_cpus_allowed_ptr(p, override_mask));
out_free_mask:
	cpus_read_unlock();
	free_cpumask_var(new_mask);
}

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask);

/*
 * Restore the affinity of a task @p which was previously restricted by a
 * call to force_compatible_cpus_allowed_ptr(). This will clear (and free)
 * @p->user_cpus_ptr.
 *
 * It is the caller's responsibility to serialise this with any calls to
 * force_compatible_cpus_allowed_ptr(@p).
 */
void relax_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = p->user_cpus_ptr;
	unsigned long flags;

	/*
	 * Try to restore the old affinity mask. If this fails, then
	 * we free the mask explicitly to avoid it being inherited across
	 * a subsequent fork().
	 */
	if (!user_mask || !__sched_setaffinity(p, user_mask))
		return;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	user_mask = clear_user_cpus_ptr(p);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

	kfree(user_mask);
}

void set_task_cpu(struct task_struct *p, unsigned int new_cpu)
{
#ifdef CONFIG_SCHED_DEBUG
	unsigned int state = READ_ONCE(p->__state);

	/*
	 * We should never call set_task_cpu() on a blocked task,
	 * ttwu() will sort out the placement.
	 */
	WARN_ON_ONCE(state != TASK_RUNNING && state != TASK_WAKING && !p->on_rq);

	/*
	 * Migrating fair class task must have p->on_rq = TASK_ON_RQ_MIGRATING,
	 * because schedstat_wait_{start,end} rebase migrating task's wait_start
	 * time relying on p->on_rq.
	 */
	WARN_ON_ONCE(state == TASK_RUNNING &&
		     p->sched_class == &fair_sched_class &&
		     (p->on_rq && !task_on_rq_migrating(p)));

#ifdef CONFIG_LOCKDEP
	/*
	 * The caller should hold either p->pi_lock or rq->lock, when changing
	 * a task's CPU. ->pi_lock for waking tasks, rq->lock for runnable tasks.
	 *
	 * sched_move_task() holds both and thus holding either pins the cgroup,
	 * see task_group().
	 *
	 * Furthermore, all task_rq users should acquire both locks, see
	 * task_rq_lock().
	 */
	WARN_ON_ONCE(debug_locks && !(lockdep_is_held(&p->pi_lock) ||
				      lockdep_is_held(__rq_lockp(task_rq(p)))));
#endif
	/*
	 * Clearly, migrating tasks to offline CPUs is a fairly daft thing.
	 */
	WARN_ON_ONCE(!cpu_online(new_cpu));

	WARN_ON_ONCE(is_migration_disabled(p));
#endif

	trace_sched_migrate_task(p, new_cpu);

	if (task_cpu(p) != new_cpu) {
		if (p->sched_class->migrate_task_rq)
			p->sched_class->migrate_task_rq(p, new_cpu);
		p->se.nr_migrations++;
		rseq_migrate(p);
		perf_event_task_migrate(p);
	}

	__set_task_cpu(p, new_cpu);
}

#ifdef CONFIG_NUMA_BALANCING
static void __migrate_swap_task(struct task_struct *p, int cpu)
{
	if (task_on_rq_queued(p)) {
		struct rq *src_rq, *dst_rq;
		struct rq_flags srf, drf;

		src_rq = task_rq(p);
		dst_rq = cpu_rq(cpu);

		rq_pin_lock(src_rq, &srf);
		rq_pin_lock(dst_rq, &drf);

		deactivate_task(src_rq, p, 0);
		set_task_cpu(p, cpu);
		activate_task(dst_rq, p, 0);
		check_preempt_curr(dst_rq, p, 0);

		rq_unpin_lock(dst_rq, &drf);
		rq_unpin_lock(src_rq, &srf);

	} else {
		/*
		 * Task isn't running anymore; make it appear like we migrated
		 * it before it went to sleep. This means on wakeup we make the
		 * previous CPU our target instead of where it really is.
		 */
		p->wake_cpu = cpu;
	}
}

struct migration_swap_arg {
	struct task_struct *src_task, *dst_task;
	int src_cpu, dst_cpu;
};

static int migrate_swap_stop(void *data)
{
	struct migration_swap_arg *arg = data;
	struct rq *src_rq, *dst_rq;
	int ret = -EAGAIN;

	if (!cpu_active(arg->src_cpu) || !cpu_active(arg->dst_cpu))
		return -EAGAIN;

	src_rq = cpu_rq(arg->src_cpu);
	dst_rq = cpu_rq(arg->dst_cpu);

	double_raw_lock(&arg->src_task->pi_lock,
			&arg->dst_task->pi_lock);
	double_rq_lock(src_rq, dst_rq);

	if (task_cpu(arg->dst_task) != arg->dst_cpu)
		goto unlock;

	if (task_cpu(arg->src_task) != arg->src_cpu)
		goto unlock;

	if (!cpumask_test_cpu(arg->dst_cpu, arg->src_task->cpus_ptr))
		goto unlock;

	if (!cpumask_test_cpu(arg->src_cpu, arg->dst_task->cpus_ptr))
		goto unlock;

	__migrate_swap_task(arg->src_task, arg->dst_cpu);
	__migrate_swap_task(arg->dst_task, arg->src_cpu);

	ret = 0;

unlock:
	double_rq_unlock(src_rq, dst_rq);
	raw_spin_unlock(&arg->dst_task->pi_lock);
	raw_spin_unlock(&arg->src_task->pi_lock);

	return ret;
}

/*
 * Cross migrate two tasks
 */
int migrate_swap(struct task_struct *cur, struct task_struct *p,
		int target_cpu, int curr_cpu)
{
	struct migration_swap_arg arg;
	int ret = -EINVAL;

	arg = (struct migration_swap_arg){
		.src_task = cur,
		.src_cpu = curr_cpu,
		.dst_task = p,
		.dst_cpu = target_cpu,
	};

	if (arg.src_cpu == arg.dst_cpu)
		goto out;

	/*
	 * These three tests are all lockless; this is OK since all of them
	 * will be re-checked with proper locks held further down the line.
	 */
	if (!cpu_active(arg.src_cpu) || !cpu_active(arg.dst_cpu))
		goto out;

	if (!cpumask_test_cpu(arg.dst_cpu, arg.src_task->cpus_ptr))
		goto out;

	if (!cpumask_test_cpu(arg.src_cpu, arg.dst_task->cpus_ptr))
		goto out;

	trace_sched_swap_numa(cur, arg.src_cpu, p, arg.dst_cpu);
	ret = stop_two_cpus(arg.dst_cpu, arg.src_cpu, migrate_swap_stop, &arg);

out:
	return ret;
}
#endif /* CONFIG_NUMA_BALANCING */

/*
 * wait_task_inactive - wait for a thread to unschedule.
 *
 * If @match_state is nonzero, it's the @p->state value just checked and
 * not expected to change.  If it changes, i.e. @p might have woken up,
 * then return zero.  When we succeed in waiting for @p to be off its CPU,
 * we return a positive number (its total switch count).  If a second call
 * a short while later returns the same number, the caller can be sure that
 * @p has remained unscheduled the whole time.
 *
 * The caller must ensure that the task *will* unschedule sometime soon,
 * else this function might spin for a *long* time. This function can't
 * be called with interrupts off, or it may introduce deadlock with
 * smp_call_function() if an IPI is sent by the same process we are
 * waiting to become inactive.
 */
unsigned long wait_task_inactive(struct task_struct *p, unsigned int match_state)
{
	int running, queued;
	struct rq_flags rf;
	unsigned long ncsw;
	struct rq *rq;

	for (;;) {
		/*
		 * We do the initial early heuristics without holding
		 * any task-queue locks at all. We'll only try to get
		 * the runqueue lock when things look like they will
		 * work out!
		 */
		rq = task_rq(p);

		/*
		 * If the task is actively running on another CPU
		 * still, just relax and busy-wait without holding
		 * any locks.
		 *
		 * NOTE! Since we don't hold any locks, it's not
		 * even sure that "rq" stays as the right runqueue!
		 * But we don't care, since "task_running()" will
		 * return false if the runqueue has changed and p
		 * is actually now running somewhere else!
		 */
		while (task_running(rq, p)) {
			if (match_state && unlikely(READ_ONCE(p->__state) != match_state))
				return 0;
			cpu_relax();
		}

		/*
		 * Ok, time to look more closely! We need the rq
		 * lock now, to be *sure*. If we're wrong, we'll
		 * just go back and repeat.
		 */
		rq = task_rq_lock(p, &rf);
		trace_sched_wait_task(p);
		running = task_running(rq, p);
		queued = task_on_rq_queued(p);
		ncsw = 0;
		if (!match_state || READ_ONCE(p->__state) == match_state)
			ncsw = p->nvcsw | LONG_MIN; /* sets MSB */
		task_rq_unlock(rq, p, &rf);

		/*
		 * If it changed from the expected state, bail out now.
		 */
		if (unlikely(!ncsw))
			break;

		/*
		 * Was it really running after all now that we
		 * checked with the proper locks actually held?
		 *
		 * Oops. Go back and try again..
		 */
		if (unlikely(running)) {
			cpu_relax();
			continue;
		}

		/*
		 * It's not enough that it's not actively running,
		 * it must be off the runqueue _entirely_, and not
		 * preempted!
		 *
		 * So if it was still runnable (but just not actively
		 * running right now), it's preempted, and we should
		 * yield - it could be a while.
		 */
		if (unlikely(queued)) {
			ktime_t to = NSEC_PER_SEC / HZ;

			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_hrtimeout(&to, HRTIMER_MODE_REL_HARD);
			continue;
		}

		/*
		 * Ahh, all good. It wasn't running, and it wasn't
		 * runnable, which means that it will never become
		 * running in the future either. We're all done!
		 */
		break;
	}

	return ncsw;
}

/***
 * kick_process - kick a running thread to enter/exit the kernel
 * @p: the to-be-kicked thread
 *
 * Cause a process which is running on another CPU to enter
 * kernel-mode, without any delay. (to get signals handled.)
 *
 * NOTE: this function doesn't have to take the runqueue lock,
 * because all it wants to ensure is that the remote task enters
 * the kernel. If the IPI races and the task has been migrated
 * to another CPU then no harm is done and the purpose has been
 * achieved as well.
 */
void kick_process(struct task_struct *p)
{
	int cpu;

	preempt_disable();
	cpu = task_cpu(p);
	if ((cpu != smp_processor_id()) && task_curr(p))
		smp_send_reschedule(cpu);
	preempt_enable();
}
EXPORT_SYMBOL_GPL(kick_process);

/*
 * ->cpus_ptr is protected by both rq->lock and p->pi_lock
 *
 * A few notes on cpu_active vs cpu_online:
 *
 *  - cpu_active must be a subset of cpu_online
 *
 *  - on CPU-up we allow per-CPU kthreads on the online && !active CPU,
 *    see __set_cpus_allowed_ptr(). At this point the newly online
 *    CPU isn't yet part of the sched domains, and balancing will not
 *    see it.
 *
 *  - on CPU-down we clear cpu_active() to mask the sched domains and
 *    avoid the load balancer to place new tasks on the to be removed
 *    CPU. Existing tasks will remain running there and will be taken
 *    off.
 *
 * This means that fallback selection must not select !active CPUs.
 * And can assume that any active CPU must be online. Conversely
 * select_task_rq() below may allow selection of !active CPUs in order
 * to satisfy the above rules.
 */
static int select_fallback_rq(int cpu, struct task_struct *p)
{
	int nid = cpu_to_node(cpu);
	const struct cpumask *nodemask = NULL;
	enum { cpuset, possible, fail } state = cpuset;
	int dest_cpu;

	/*
	 * If the node that the CPU is on has been offlined, cpu_to_node()
	 * will return -1. There is no CPU on the node, and we should
	 * select the CPU on the other node.
	 */
	if (nid != -1) {
		nodemask = cpumask_of_node(nid);

		/* Look foRRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (in_list(e, &class->locks_after) ||
		    in_list(e, &class->locks_before))
			return true;
	}
	return false;
}

static bool class_lock_list_valid(struct lock_class *c, struct list_head *h)
{
	struct lock_list *e;

	list_for_each_entry(e, h, entry) {
		if (e->links_to != c) {
			printk(KERN_INFO "class %s: mismatch for lock entry %ld; class %s <> %s",
			       c->name ? : "(?)",
			       (unsigned long)(e - list_entries),
			       e->links_to && e->links_to->name ?
			       e->links_to->name : "(?)",
			       e->class && e->class->name ? e->class->name :
			       "(?)");
			return false;
		}
	}
	return true;
}

#ifdef CONFIG_PROVE_LOCKING
static u16 chain_hlocks[MAX_LOCKDEP_CHAIN_HLOCKS];
#endif

static bool check_lock_chain_key(struct lock_chain *chain)
{
#ifdef CONFIG_PROVE_LOCKING
	u64 chain_key = INITIAL_CHAIN_KEY;
	int i;

	for (i = chain->base; i < chain->base + chain->depth; i++)
		chain_key = iterate_chain_key(chain_key, chain_hlocks[i]);
	/*
	 * The 'unsigned long long' casts avoid that a compiler warning
	 * is reported when building tools/lib/lockdep.
	 */
	if (chain->chain_key != chain_key) {
		printk(KERN_INFO "chain %lld: key %#llx <> %#llx\n",
		       (unsigned long long)(chain - lock_chains),
		       (unsigned long long)chain->chain_key,
		       (unsigned long long)chain_key);
		return false;
	}
#endif
	return true;
}

static bool in_any_zapped_class_list(struct lock_class *class)
{
	struct pending_free *pf;
	int i;

	for (i = 0, pf = delayed_free.pf; i < ARRAY_SIZE(delayed_free.pf); i++, pf++) {
		if (in_list(&class->lock_entry, &pf->zapped))
			return true;
	}

	return false;
}

static bool __check_data_structures(void)
{
	struct lock_class *class;
	struct lock_chain *chain;
	struct hlist_head *head;
	struct lock_list *e;
	int i;

	/* Check whether all classes occur in a lock list. */
	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (!in_list(&class->lock_entry, &all_lock_classes) &&
		    !in_list(&class->lock_entry, &free_lock_classes) &&
		    !in_any_zapped_class_list(class)) {
			printk(KERN_INFO "class %px/%s is not in any class list\n",
			       class, class->name ? : "(?)");
			return false;
		}
	}

	/* Check whether all classes have valid lock lists. */
	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (!class_lock_list_valid(class, &class->locks_before))
			return false;
		if (!class_lock_list_valid(class, &class->locks_after))
			return false;
	}

	/* Check the chain_key of all lock chains. */
	for (i = 0; i < ARRAY_SIZE(chainhash_table); i++) {
		head = chainhash_table + i;
		hlist_for_each_entry_rcu(chain, head, entry) {
			if (!check_lock_chain_key(chain))
				return false;
		}
	}

	/*
	 * Check whether all list entries that are in use occur in a class
	 * lock list.
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		e = list_entries + i;
		if (!in_any_class_list(&e->entry)) {
			printk(KERN_INFO "list entry %d is not in any class list; class %s <> %s\n",
			       (unsigned int)(e - list_entries),
			       e->class->name ? : "(?)",
			       e->links_to->name ? : "(?)");
			return false;
		}
	}

	/*
	 * Check whether all list entries that are not in use do not occur in
	 * a class lock list.
	 */
	for_each_clear_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		e = list_entries + i;
		if (in_any_class_list(&e->entry)) {
			printk(KERN_INFO "list entry %d occurs in a class list; class %s <> %s\n",
			       (unsigned int)(e - list_entries),
			       e->class && e->class->name ? e->class->name :
			       "(?)",
			       e->links_to && e->links_to->name ?
			       e->links_to->name : "(?)");
			return false;
		}
	}

	return true;
}

int check_consistency = 0;
module_param(check_consistency, int, 0644);

static void check_data_structures(void)
{
	static bool once = false;

	if (check_consistency && !once) {
		if (!__check_data_structures()) {
			once = true;
			WARN_ON(once);
		}
	}
}

#else /* CONFIG_DEBUG_LOCKDEP */

static inline void check_data_structures(void) { }

#endif /* CONFIG_DEBUG_LOCKDEP */

static void init_chain_block_buckets(void);

/*
 * Initialize the lock_classes[] array elements, the free_lock_classes list
 * and also the delayed_free structure.
 */
static void init_data_structures_once(void)
{
	static bool __read_mostly ds_initialized, rcu_head_initialized;
	int i;

	if (likely(rcu_head_initialized))
		return;

	if (system_state >= SYSTEM_SCHEDULING) {
		init_rcu_head(&delayed_free.rcu_head);
		rcu_head_initialized = true;
	}

	if (ds_initialized)
		return;

	ds_initialized = true;

	INIT_LIST_HEAD(&delayed_free.pf[0].zapped);
	INIT_LIST_HEAD(&delayed_free.pf[1].zapped);

	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		list_add_tail(&lock_classes[i].lock_entry, &free_lock_classes);
		INIT_LIST_HEAD(&lock_classes[i].locks_after);
		INIT_LIST_HEAD(&lock_classes[i].locks_before);
	}
	init_chain_block_buckets();
}

static inline struct hlist_head *keyhashentry(const struct lock_class_key *key)
{
	unsigned long hash = hash_long((uintptr_t)key, KEYHASH_BITS);

	return lock_keys_hash + hash;
}

/* Register a dynamically allocated key. */
void lockdep_register_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head;
	struct lock_class_key *k;
	unsigned long flags;

	if (WARN_ON_ONCE(static_obj(key)))
		return;
	hash_head = keyhashentry(key);

	raw_local_irq_save(flags);
	if (!graph_lock())
		goto restore_irqs;
	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (WARN_ON_ONCE(k == key))
			goto out_unlock;
	}
	hlist_add_head_rcu(&key->hash_entry, hash_head);
out_unlock:
	graph_unlock();
restore_irqs:
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lockdep_register_key);

/* Check whether a key has been registered as a dynamic key. */
static bool is_dynamic_key(const struct lock_class_key *key)
{
	struct hlist_head *hash_head;
	struct lock_class_key *k;
	bool found = false;

	if (WARN_ON_ONCE(static_obj(key)))
		return false;

	/*
	 * If lock debugging is disabled lock_keys_hash[] may contain
	 * pointers to memory that has already been freed. Avoid triggering
	 * a use-after-free in that case by returning early.
	 */
	if (!debug_locks)
		return true;

	hash_head = keyhashentry(key);

	rcu_read_lock();
	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (k == key) {
			found = true;
			break;
		}
	}
	rcu_read_unlock();

	return found;
}

/*
 * Register a lock's class in the hash-table, if the class is not present
 * yet. Otherwise we look it up. We cache the result in the lock object
 * itself, so actual lookup of the hash should be once per lock object.
 */
static struct lock_class *
register_lock_class(struct lockdep_map *lock, unsigned int subclass, int force)
{
	struct lockdep_subclass_key *key;
	struct hlist_head *hash_head;
	struct lock_class *class;
	int idx;

	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	class = look_up_lock_class(lock, subclass);
	if (likely(class))
		goto out_set_class_cache;

	if (!lock->key) {
		if (!assign_lock_key(lock))
			return NULL;
	} else if (!static_obj(lock->key) && !is_dynamic_key(lock->key)) {
		return NULL;
	}

	key = lock->key->subkeys + subclass;
	hash_head = classhashentry(key);

	if (!graph_lock()) {
		return NULL;
	}
	/*
	 * We have to do the hash-walk again, to avoid races
	 * with another CPU:
	 */
	hlist_for_each_entry_rcu(class, hash_head, hash_entry) {
		if (class->key == key)
			goto out_unlock_set;
	}

	init_data_structures_once();

	/* Allocate a new lock class and add it to the hash. */
	class = list_first_entry_or_null(&free_lock_classes, typeof(*class),
					 lock_entry);
	if (!class) {
		if (!debug_locks_off_graph_unlock()) {
			return NULL;
		}

		print_lockdep_off("BUG: MAX_LOCKDEP_KEYS too low!");
		dump_stack();
		return NULL;
	}
	nr_lock_classes++;
	__set_bit(class - lock_classes, lock_classes_in_use);
	debug_atomic_inc(nr_unused_locks);
	class->key = key;
	class->name = lock->name;
	class->subclass = subclass;
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	class->name_version = count_matching_names(class);
	class->wait_type_inner = lock->wait_type_inner;
	class->wait_type_outer = lock->wait_type_outer;
	class->lock_type = lock->lock_type;
	/*
	 * We use RCU's safe list-add method to make
	 * parallel walking of the hash-list safe:
	 */
	hlist_add_head_rcu(&class->hash_entry, hash_head);
	/*
	 * Remove the class from the free list and add it to the global list
	 * of classes.
	 */
	list_move_tail(&class->lock_entry, &all_lock_classes);
	idx = class - lock_classes;
	if (idx > max_lock_class_idx)
		max_lock_class_idx = idx;

	if (verbose(class)) {
		graph_unlock();

		printk("\nnew class %px: %s", class->key, class->name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		printk(KERN_CONT "\n");
		dump_stack();

		if (!graph_lock()) {
			return NULL;
		}
	}
out_unlock_set:
	graph_unlock();

out_set_class_cache:
	if (!subclass || force)
		lock->class_cache[0] = class;
	else if (subclass < NR_LOCKDEP_CACHING_CLASSES)
		lock->class_cache[subclass] = class;

	/*
	 * Hash collision, did we smoke some? We found a class with a matching
	 * hash but the subclass -- which is hashed in -- didn't match.
	 */
	if (DEBUG_LOCKS_WARN_ON(class->subclass != subclass))
		return NULL;

	return class;
}

#ifdef CONFIG_PROVE_LOCKING
/*
 * Allocate a lockdep entry. (assumes the graph_lock held, returns
 * with NULL on failure)
 */
static struct lock_list *alloc_list_entry(void)
{
	int idx = find_first_zero_bit(list_entries_in_use,
				      ARRAY_SIZE(list_entries));

	if (idx >= ARRAY_SIZE(list_entries)) {
		if (!debug_locks_off_graph_unlock())
			return NULL;

		print_lockdep_off("BUG: MAX_LOCKDEP_ENTRIES too low!");
		dump_stack();
		return NULL;
	}
	nr_list_entries++;
	__set_bit(idx, list_entries_in_use);
	return list_entries + idx;
}

/*
 * Add a new dependency to the head of the list:
 */
static int add_lock_to_list(struct lock_class *this,
			    struct lock_class *links_to, struct list_head *head,
			    unsigned long ip, u16 distance, u8 dep,
			    const struct lock_trace *trace)
{
	struct lock_list *entry;
	/*
	 * Lock not present yet - get a new dependency struct and
	 * add it to the list:
	 */
	entry = alloc_list_entry();
	if (!entry)
		return 0;

	entry->class = this;
	entry->links_to = links_to;
	entry->dep = dep;
	entry->distance = distance;
	entry->trace = trace;
	/*
	 * Both allocation and removal are done under the graph lock; but
	 * iteration is under RCU-sched; see look_up_lock_class() and
	 * lockdep_free_key_range().
	 */
	list_add_tail_rcu(&entry->entry, head);

	return 1;
}

/*
 * For good efficiency of modular, we use power of 2
 */
#define MAX_CIRCULAR_QUEUE_SIZE		(1UL << CONFIG_LOCKDEP_CIRCULAR_QUEUE_BITS)
#define CQ_MASK				(MAX_CIRCULAR_QUEUE_SIZE-1)

/*
 * The circular_queue and helpers are used to implement graph
 * breadth-first search (BFS) algorithm, by which we can determine
 * whether there is a path from a lock to another. In deadlock checks,
 * a path from the next lock to be acquired to a previous held lock
 * indicates that adding the <prev> -> <next> lock dependency will
 * produce a circle in the graph. Breadth-first search instead of
 * depth-first search is used in order to find the shortest (circular)
 * path.
 */
struct circular_queue {
	struct lock_list *element[MAX_CIRCULAR_QUEUE_SIZE];
	unsigned int  front, rear;
};

static struct circular_queue lock_cq;

unsigned int max_bfs_queue_depth;

static unsigned int lockdep_dependency_gen_id;

static inline void __cq_init(struct circular_queue *cq)
{
	cq->front = cq->rear = 0;
	lockdep_dependency_gen_id++;
}

static inline int __cq_empty(struct circular_queue *cq)
{
	return (cq->front == cq->rear);
}

static inline int __cq_full(struct circular_queue *cq)
{
	return ((cq->rear + 1) & CQ_MASK) == cq->front;
}

static inline int __cq_enqueue(struct circular_queue *cq, struct lock_list *elem)
{
	if (__cq_full(cq))
		return -1;

	cq->element[cq->rear] = elem;
	cq->rear = (cq->rear + 1) & CQ_MASK;
	return 0;
}

/*
 * Dequeue an element from the circular_queue, return a lock_list if
 * the queue is not empty, or NULL if otherwise.
 */
static inline struct lock_list * __cq_dequeue(struct circular_queue *cq)
{
	struct lock_list * lock;

	if (__cq_empty(cq))
		return NULL;

	lock = cq->element[cq->front];
	cq->front = (cq->front + 1) & CQ_MASK;

	return lock;
}

static inline unsigned int  __cq_get_elem_count(struct circular_queue *cq)
{
	return (cq->rear - cq->front) & CQ_MASK;
}

static inline void mark_lock_accessed(struct lock_list *lock)
{
	lock->class->dep_gen_id = lockdep_dependency_gen_id;
}

static inline void visit_lock_entry(struct lock_list *lock,
				    struct lock_list *parent)
{
	lock->parent = parent;
}

static inline unsigned long lock_accessed(struct lock_list *lock)
{
	return lock->class->dep_gen_id == lockdep_dependency_gen_id;
}

static inline struct lock_list *get_lock_parent(struct lock_list *child)
{
	return child->parent;
}

static inline int get_lock_depth(struct lock_list *child)
{
	int depth = 0;
	struct lock_list *parent;

	while ((parent = get_lock_parent(child))) {
		child = parent;
		depth++;
	}
	return depth;
}

/*
 * Return the forward or backward dependency list.
 *
 * @lock:   the lock_list to get its class's dependency list
 * @offset: the offset to struct lock_class to determine whether it is
 *          locks_after or locks_before
 */
static inline struct list_head *get_dep_list(struct lock_list *lock, int offset)
{
	void *lock_class = lock->class;

	return lock_class + offset;
}
/*
 * Return values of a bfs search:
 *
 * BFS_E* indicates an error
 * BFS_R* indicates a result (match or not)
 *
 * BFS_EINVALIDNODE: Find a invalid node in the graph.
 *
 * BFS_EQUEUEFULL: The queue is full while doing the bfs.
 *
 * BFS_RMATCH: Find the matched node in the graph, and put that node into
 *             *@target_entry.
 *
 * BFS_RNOMATCH: Haven't found the matched node and keep *@target_entry
 *               _unchanged_.
 */
enum bfs_result {
	BFS_EINVALIDNODE = -2,
	BFS_EQUEUEFULL = -1,
	BFS_RMATCH = 0,
	BFS_RNOMATCH = 1,
};

/*
 * bfs_result < 0 means error
 */
static inline bool bfs_error(enum bfs_result res)
{
	return res < 0;
}

/*
 * DEP_*_BIT in lock_list::dep
 *
 * For dependency @prev -> @next:
 *
 *   SR: @prev is shared reader (->read != 0) and @next is recursive reader
 *       (->read == 2)
 *   ER: @prev is exclusive locker (->read == 0) and @next is recursive reader
 *   SN: @prev is shared reader and @next is non-recursive locker (->read != 2)
 *   EN: @prev is exclusive locker and @next is non-recursive locker
 *
 * Note that we define the value of DEP_*_BITs so that:
 *   bit0 is prev->read == 0
 *   bit1 is next->read != 2
 */
#define DEP_SR_BIT (0 + (0 << 1)) /* 0 */
#define DEP_ER_BIT (1 + (0 << 1)) /* 1 */
#define DEP_SN_BIT (0 + (1 << 1)) /* 2 */
#define DEP_EN_BIT (1 + (1 << 1)) /* 3 */

#define DEP_SR_MASK (1U << (DEP_SR_BIT))
#define DEP_ER_MASK (1U << (DEP_ER_BIT))
#define DEP_SN_MASK (1U << (DEP_SN_BIT))
#define DEP_EN_MASK (1U << (DEP_EN_BIT))

static inline unsigned int
__calc_dep_bit(struct held_lock *prev, struct held_lock *next)
{
	return (prev->read == 0) + ((next->read != 2) << 1);
}

static inline u8 calc_dep(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bit(prev, next);
}

/*
 * calculate the dep_bit for backwards edges. We care about whether @prev is
 * shared and whether @next is recursive.
 */
static inline unsigned int
__calc_dep_bitb(struct held_lock *prev, struct held_lock *next)
{
	return (next->read != 2) + ((prev->read == 0) << 1);
}

static inline u8 calc_depb(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bitb(prev, next);
}

/*
 * Initialize a lock_list entry @lock belonging to @class as the root for a BFS
 * search.
 */
static inline void __bfs_init_root(struct lock_list *lock,
				   struct lock_class *class)
{
	lock->class = class;
	lock->parent = NULL;
	lock->only_xr = 0;
}

/*
 * Initialize a lock_list entry @lock based on a lock acquisition @hlock as the
 * root for a BFS search.
 *
 * ->only_xr of the initial lock node is set to @hlock->read == 2, to make sure
 * that <prev>ELF                      �"      4     (            GNU  �       �          �������t?S�Ë��  ��t���t
ǂ$&      �����ǃ�      ǃ�      [Í�&    Í�&    �����UWV��S����d�    �D$1�����   ��  ����   ��,&  ����   �F�$    1ɺ����Tj �   h    ����������t   �   �   ��  �,$������Å��Z   �8�   �
   �h���  ǅ�      �x�(�G������G$�u���Dh   j P������ud�u�h@  jPW������؃�$��������   ��,&  �$���������   �    u-�D$d+    u��1�[^_]Ív �    t�������������   f�f�f�f�������@�@    �@    1�Í�&    �����1�Í�&    ������VS�p�^��������v���������[^Í�&    �t& �����UWVS�Ã��5   �$�ʅ��2  �h����   �}|����   �w`����   9��  ��4  ���7  ��1����S�D$�GT�D$����ȉщD$�D$ЉD$9�sv�t$�$�D$���������������C�t$��C�OT��)�9�C�s�s��C�GL9���   ����[^_]������-   ���  ��[^_]Í�&    �v �L$�D$)ʉы$�ΉL$������D$�L$+L$�$ȋL$��ʉ���4  ������T�����&    �   ���   ��[^_]Í�&    �v )���K��������[^_]�������    �   ���P����  ��&    ��    �����WVS�p���   ��T  �����1҉����������  �������ǃ(&      �   �����1�[^_Í�&    ��&    �����U��WVS��l�u|d�    �D$h�E��$�C��D$�Ǎ�   �������<&  ���  �|$d�H�D$\�D$��<&  �T$`��������"  ���  ��x��T  �D$����  s6�$�T$�    ������T$�D$hd+    ��   ��l��[^_]Ít& �Ѝ��   �   �   ��ǆ�    ǆ�      ǆ       ǆ  �   ǆ  ��  ǆ  ��  ǆ     ǆ     ǆ  � @ǆ!  1  ǆ(  b   ������<$�L$�o���   ǃ(&      ����  �D$������$�   ������$�����$�    ����������������������&    �����U1ɺ   W��VS���/d�    �D$1��GTjj ������ƃ�����  �G    �$��   �   �����1ɺ   j j �D$������\$�P   �C    ��X��`  �}d���������XZ�����  ��Pt����P��  ����������D$d+    u
����[^_]������S�@@��uh    h    �����XZ�6��$&  ��u��h    Ph0   ����������B�����ǃ$&      1�[�hh  ������    �   �����1�1�h    ������        h�  ��������h(   h    h`   ��������b  ��Ph   Vhl  ��������$��t������������h(   Vh  ��������b  ��j�h   Vh  �������뻃�Ph   Vh�   �������뵃�h(   Vh�   ��������b  ǆ,&      ��Ph   Vh�  ��������a���h(   h    �����Y[�b  h(   h    h�  ��������b  hd   hd   hp  ���������  hd   hL  �����^_��  Q�L$�t$Phd   h�  ������k�T$����tD�}|���3  �=    ��  hd   h$  �����XZ��  hd   h�  �����XZ��  �=    ��  hd   h�  �����Y[��  �H   ������@   �������PhP   Uh�  ��������p  �����hH  �����X1��    P   �           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   �              �  �      �                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
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
     P       �  �  �     �    �  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           ��     �           ��    �  �  s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ��       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 �            �   U   ?     �            �           �      #     �   =          O   Y       �        4           B  8       
 T           ��d  �       y  �       �  �  %     �          �    �     s   d       
 �  �  R     �    �    �  �   @    
   �  0     +  H       
 6  @       
 A  P       
 L  �   -     e  �        �          �  �   
    
 �             �             �             �             �             �             �               U   ?                  !             '             >             U             c             n           z             �             �             �             �             �             �             �             �             	                          9             X             u             �             �             �             �             �             �           �     �                 $             2  �  �     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q     �     �   6  �   I  �     �   ]  �   @  �     �   	  �       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  �  W  �    �  G  �    �  B  �  B  �  B  �  ?  �  N    B          �  =  �  S  �  F  �    �      =  @  =  R    Z    x  F  �  Y  �    �    �  B  �  J  �  P  �  M  �  U    B  #  9  C  J  m  L  �  U  �  9  U  Q  o  X  �  D  �  U  �  U  �  G  �  B  �  	  �  9  �  7  �          K  &  O  I  H  V    g    p  R  {  9  �  G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  �     �   E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     �     �   E  �     �     �   E  �     �     �     �   E  �     �     �   E  �     �     �   	  �     �   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    �  E  �    �    �    �  E  �    �    �    �    �    �  E  �    �    �  A  �    �  A  �    �    �  E  �       B          E     T       �     �     �     �     �                                                    $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `   �                 .   	   @       h  �              <             �  �                  8   	   @       �  �               K             �  �                 G   	   @       x  H              ^             x	                    Z   	   @       �   (               m             �	  �                   i   	   @       �   (      
         u      2       l
  	                �             u  (                  �   	   @       !  P               �      2       �  +                 �             �  �                  �             �  (                  �   	   @       `!  @               �             �                    �   	   @       �!                 �             �                     �             �                    �      0       �  &                 �              "                                   $  �     5         	                b                               �!  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ��ė�I�A�92=��nxsw(ğuz����*���r�?/W�
��c^|��B�����0�b�6[���Gv�{z�=���Q�5��(х����ݚ�J���?�8���߆��Vr:9x�Ss�?�N3�)�Cs���/#�����#��"U 4�F?�H��5�V�5��c���������A�6��B-\�+��J'�1�rx:�x���ꁙ!��O	l�s�C���c�	߹Ve��.��9��C��ֹQr7��'Đap�=[�m���Ӈ��unR�^ަ3��[�-��qŲ�=�Ll:0�B�wZiTX���\�!|*"򀛹�����*�o���A'�G~G��"���`g	������H��eQ|k
�m�UPA��!�"�p����+D(=Pi�0�:8:��ͮ����&Ebm�D>���ޮ����2�e�o��y��T>w,���l.V�u{/v��M�"�-ˠ�ԫ�&[G�Si@?���[Bc+BU�5���{����7��!PĦ�_D�-�_\�. =w��Jf`���
�Y?[+�G�E�C �, �͡�cW	pD4�Vt��*��n�. Q��m,s�8��J���`u�L��[&�h�Q�g�JE��J���!�W���x)�����6�����8kԱ�Ӿ����ObGG�y�P��A��߮��x��0G$���O �S���P4w\@n ���o�EQ�N1"z��|���'z��i���#yQ�ф�8��W�l������Xm��N��d�1�~���^��м��n }��L�
�8b*�t�Wg� X�6�n�����4yR���ޘ��E��NLkH�Ȩ��(��A����)�/W�^Q��ŵu�=6�"wK�j��a7�����oF#��-��2Vu}E|����u��,��hY/��%|�TMx���ٗ�q�:�=����{Rt�j�S��C���1��5��2���<L����p���K�S����� K���ݞR�#�'��k�Fϔ�F����HJ�/!�I��C;���sB�L_
����i]*ǜc���wm����1��wP�U����+4��ْ�/J���,|����� �[ ��#m���#�$ƾ�<�&��I'M�T��Y!$_�?���B�-AY^W��:�W�c��N&���`��Fݠ�RV��<�v��#t����&��@��C+��}:��=B�(�ò(f+Oނ��43��%��}c���G"���i�T4<v��G�֪�X� �
V;
ǅ�q"�^ũ�kQ�� $U��8��}�;��3�~,ƦwW��຿�1��t�p9��-���� �"3�*u�\��,B55V��QÀ��?6���F7�uk�0�Os,��ǴK ��g�[W�/T�5�K�(jPkK�i�%�)� J_�j������ě�1���HsS��?9g-��ّBT,Y����#��J�� xA��[��V/����zŖ�Uj��6oLc�Ӄ<[zq�v��s�k��9�k��g��C�a��Ql\G��*��"]Hw����xb��]��s8_|S5�����Z��O�!F�����JvG����,Wv���ҡ�h�e+ߙ�W����ɸ�	w?*�e�)\����K�d�J�No5H� �׼i�)��y�!��(��K �P�6�cU�3$31gJ�T�y�D�i�.�C���+ԁ�^��c�����8A�k[�[�L�\�&H�ĝ�6����4?�&6,����aZ���\��!�xp���S�Z��{�B��7&�؋���T��j�ym"G��ڼ�'�ɂBc߃Gi� �;����c�S({�z�,�@S�}Ee�j���]p���_�:b���ɩ�����8}��ղ���� Dz���C0��!�9f_����_���B2,�*en�̉6���c�e=������]DL���r��R��4��_"��7S�� ��+f��>�"�}���~I��i����r��4��UXs�g|C�I�O��(�_��XN~�j�UN+�"&u�<-����>�N�+������ܵ�H^�����'�EjC��8M��Z�,��m����m��P��"�**�CT��µ)d��_F����=�DwY�6����b�d��,�q��U���xC>�5��X[�����V΅7ф���F%�H�Sp���0�ڱ�q���E��������K�M-�OE�@2��w!Rϩ�R���	k9�QP(実�`��-��������`������qFI$e�?��d��Ō_���S�S��d��v�*���w���#re]�#u�hta{��vh����*��@�U�Fq-&��@���[g���gӋ�5��Y�]��b�$�+�A�JX-#t��,���u���e�XPA�*ݝ%P��n?��C�ү�fZ7��JTRے���<@
��
4KƼ��1H�g�3�����6w��w�ɹ1J�a������1�%��5��`J����B/pz3��0��
`H�����5yx�v�?O��'���:Bb��dL�����.��G� 9���i��B?�������¯���k�QG"�i�;Z1�TT2V����֥�֣d!;�b�υ�շ?���E�̳�M65�������mct���tgN{�MeZD�{UTT��^	�XNC�ɗA�ˆ�����L�����G�2K����fA�x�O��� �͹Ƣu�m�uvn/��H��p���խ>�T8dj�~U�#:Μ(�c��%��y8Gi���X���ߜo�iݕ1��H������B�'U\�70��{w�,����'������+I5�6�1�JU��d��./�z�At(���Q�Ω��%��"f���aǻc+Ӡ����pP�~ 
(S�:��7�����O2IR�_�N�N�yV8�}��AA�fߊQL�ޑ�ӂ�s`��le�9E�JZ�H�V"X���rk�o�V�:���VN��50��a����V�#��\�q@����9��ݓk��Pv���m��N^��̓7L~΄'���^��9h��� �#D��qy�I�OYq�1�(������	������P8��c��:?nPGN/�A5�!�u"����~�1��0��ǅ�1�:#�����<��d�_�� p8�V�F�k��/�� ��Yn�T��ҹ�Q1��Q��?��>�
��g�B�XG�\ڼ~X���1��y2�~lpQP�^�x� ����^�g�Zi�2\��Y�8l����G��+�q��u�%��P�Y��D�U�ۈ6o�I�"u�9xq���U
4����\t�7l'/��9)�a#Y;I�IC�V?�D�����g�uO{
����<��I�Lv%V/���B���(�����W�=��N�֏Z� 쩋���I�pH�Z"��G��3z��o �v��Ӡs��{8@�&?\����*�q��x�=v���A�����6��w�R�=kk�j�<@���!%�Hڷ����i�D��&"k�V��Ef8�XAHx�\�ǽ��#1Qk_8�dny��r3U�Խ���wbmf ����?&�Y7�ڽK&�dὫ|߭b^,�E���h���6k*x�ɭ��1�_%"]7�m����.�,lZ��C��o��8���/�ػ0V�Ъ��5���ѿ�+JE��Yi�7~��Oc��kr��`V'|K�3*hA<�}���L��sň��=錁�%��Z@���Bp��v8��R�|+v̽]���o��2�}y�ـX�0���-�.P�<Ǯ\0)	}��E�NG<Tl(���?�[��ڙǨT�=7T6$�y��n�B,��h�p����$a+E��Dd�R�I��:<`���]�k��>�-���_xw�����6�G�w��P>��I�<�t�~~Qh�ԧr��Vi/G��ϔ�z���T�	T����^{��`x�cP��{�G�Z��S�;��N���C;�bu��,T@E���3��N�X��k�(D0M�~�]i�-'NKݤ@�U>�|;d$����AB�H�i`�d��v~�DЃ��?   �t$�D$$    )Ћu 9؉�Nء u �T$�\$�\$�D$9����  ��&    �v �\$�D$1��T$�|$�ًl$������ E�EƉ�9���B  )ǋ5u �ى|$�=u Չt$Lt �|$ Pt �t � ʚ;�t ��1����� �Xt �l$E�E�5\t =`t 1�1�5\t ������ ��E�Eŉ=`t 9Ɖ$щT$��   1퍴&    �v ��t ��t +4$�8u |$���5\t �� �=`t ��t ��t ����  ������Å���  �5\t �=`t �$�T$��9��s��u �u 	l$$�D$� u �T$ �u �D$�T$�L$�u 1ۋu ������ ��t E�E��t �t �Ɖף�t ��t � ʚ;1�1������� E�E�9Ɖ������rQ��t ��t ���� ��&    f�)Ή$߃��T$���� 9��s�$�T$�5�t �=�t ��t ��t �\$� u 1��$u �t$�������� E�E�5u �|$ =u 0u ��1������� E�E�)��(u ,u �\$�D$1��T$�|$�ًl$������ E�E�9ǉ��s���\$�D$�|$�T$�l$9�����;����5 u �$u �����1�1�	���  �Tt ��+4u �=,u ��;=(u ��,u �����4u ؉Ɖ$1�)����D$����   �|$�l$� u �u ����'  ��tE�������։D$�D$��ЉD$�ȋ\$����D$Ӊ�����ЉD$���l$���Յ�~�t$����  �$u u )=\t -`t �Tt �@t �B���A  �Xt � ʚ;1�1ۋ=`t �5\t ������ E�EÉT$�$����  �$�T$��9��rq1�t& ��t ��t +4$�8u |$���5\t �� �=`t ��t ��t ���+  ������Å��4  �5\t �=`t �$�T$��9��s�	l$$�@u �\$$�@t �@t ���	����Du �?   ���@u �����]�����&    �v �8u     �I�����ǋ5�t �   ��t ���t ��t =�t )�������щ�W� ʚ;V�D$L������D$L�T$P�L$T�D$4�T$8�L$<QRP�@t �W�����t �5\t )أ�t ���t �t ��t ����t �=`t ������    �8u     �������&    f��ǋ5�t �   ��t ���t ��t =�t )�������щ�W� ʚ;V�D$L������D$L�T$P�L$T�D$@�T$D�L$HQRP�@t ������t �5\t )أ�t ���t �t ��t ����t �=`t �6����������Dу�!�b����������������    �S����ك� ���߃� ��� ����������D���!�����������0u �5u 1��5 u � u �Љ$u ���� E�E�+u u ������Tt ]��X�	����8u    �׉5\t ��t ��=`t ��t �������5Tt �z��)�����H�9�������=�    �����陾  �D$��������������UWVS��$�D$ �-�� ���k�4���� ���� ���� ���� ���� �T$���� �L$���� �T$���� �L$���� ���� �$�L$������+$T$��!�!�����$���|$�L$�Ћ<$�Ƹ    ��!�!�	���sD�EЋC�{ �$�K�D$��� 9��;����$�������L$��׋T$ 1������ E�E���tF�D$�T$�\$ ����S�\$ t$|$�s�{�u �u �C�S��$[^_]Ít& ��    �   ��&    �t& �������d�    �D$�    ��u�D$d+    u0��Ít& k�<�D$    �       �$���D$���������������v �����9Du t+S1ɉÉ¸P� ����������1�9Du [����Í�    1�Ít& �������@u ��u �Du �@D�@u 9�u�� Í�&    �v ��э�&    �t& ������U��WV�@u ��u#�Du �p�x�@u 9�u�����^_]Í�    ��΍t& ������     �@    �@    Í�    �����W�|$����w�U��WVS��   �Du d�    �E�1��E�����������������    ������@u ��t����Du ��������x����щ�|������������	��U  �E��U��5 t �=$t ��9����  �M���+(t 9��|����  )������SQ�щE�������E؋U�^_�E��E��E��U��UĉEȋu��]������  �}ȉ�p������ɚ;���  �}�%9ظ   ����  �\u ���u 5�u ��]������u��5�u 1��� E�EƉ�u �Ɖ�5`u � ʚ;=du 1�1ۉ5`u ������ �=du E�EÉ�9Ɖ����rX�E��U����� �E��U��t& ��E��U�)�߉E����U��� 9ΉE���؉U�s܋E��U��5`u �=du ��u ��u � ʚ;1�1ۋ5�u ��u �=�u ������ E�E�9Ɖ������rZ��u ��u ���� �E��U�f��E��U�)�߉E����U��� 9ΉE���؉U�s܋E��U��5�u �=�u ��u ��u ��u +E��   ��u ��u U�+�p�������SQ�щE�������E؋U܋M��E�QRP�Du �UЉM�������E��Mĺ}�%���]�9���   � ʚ;i� ʚ;��ʉ�1����u �u ��u �щ�u �E�������E���u �E���u �E�� v �E��������x�����|����,v     �0v     �Pu �Tu ��u �Du ��u �   �        �'�����t����    �@u ����������������E�d+    uY�e�[^_]�g�_Í�    �щE�������E��E��E��EċE��UĉEȋE��U��E��������&    f����������������������&    �t& ������UWVS��d�    �D$� t ������$t  t �  �(t ����  �    ������@u �������Pu �Tu �       �Du ������=,t  ��  �   �Du ������Du ��s �Hu ��s �Lu ��s �Pu ��s �Tu ��s �Xu ��s �\u ��s �`u ��s �du � t �hu �t �lu �t �pu �t �tu �t �Du ��������s @� �8t ��u �<t hu ��u lu �t ��s �t ��� �Z����|u �@� ��s @� ��s ��u ��s ��u ��s ��u ��s ��u ��s ��u ��s ��u ��s ��u � t ��u �t ��u �t ��u �t ��u �t ��s ������    �ڃ@u ���������������������D$d+    �S  ��1�[^_]Ít& �,t ������t& �\u �du �`u �5�u �=�u +5 t =$t ������ ��E�+(t �R��P�D$������D$�T$�l$�Ɖ�+5t �D$=t ��+t �T$�șR��Q���|$��������D$�|$$�T$ �|$���߉у� ���   ��H�Hʃ�9��    �}(�4$�|$�-t �5t �=t �]�����&    �v  t �t$$t 5(t ���щ���WV�D$������D$�T$� t �D$�$t �(t XZ�����������&    ��&    �����1�������uÍ�&    ������v �����1�       �������&    ������U��WVS������D$�T$�$�@u ���%  �hu �lu �D$�Du �T$������+Pu Tu �׋Hu �5Hu !¡Lu �T$�T$!��=Lu ������ֿ    ��!�!�	���Xu D�Eǋ|$�ȉ��%Xu ʋ\u `u du 1������� E�E�D$T$�Ɖѡ�u 9t<��|$��u ��u ��W�<$��u ��u ��W�}��u ��u ��W;5�u ���u },�@u 9�������e����[^_]Ív ��������&    f���u ��u �|$ 6eă����W�f������UWVS�Á�   d�    ��$�   ��� �  ��   ���  �� ��  ���   �   ���������  �KT�CP����  ��ց�    ��  �?B 9���q  ����  i��  �sH�{L�l$�D$��t$�|$������ƅ��e  �D$�T$�D$�D$�T$�D$D$u��t=d�    ���  ��t-���u'�D$�T$�L$��������&    ��l$����  1��|$ �   ��������    ������@u �$�=�u �D$ ��1�|$P�؍L$������ƋD$Z9�t7� ʚ;��u �   ���u �u ��u �Du ��u �   ����������ѣ�u ���Љω�u �����	�t�Љ�+�u �u ��u ��u �$�    �@u �����d�    ���  ��t� ��tu�@  u~�����   �������$�   d+    ��   �Ą   ��[^_]Ít& ��l$�������&    ��ɚ;9ƾ    ���   ��t׋sH�{L� ����t& �D$ ������@  t��   �5���	ŉ���v���1�������j����t& ��   �$�������tT���@�k����sX�{\��  �$���������9���J����v ������ �����    �   �������t��"������������������f�f������i    �� ��v WVS��v ��1��v �v �Ӊށ��  vd�غ�Mb�����i��  )Ɖ���    �ȹ�  ������v [�v +�v ��v �v ^�=�v ��v _��v Ít& �1�1�뮍�&    �v ������    ��� �    �����1�Ív �������� @��v     ���  $� ���  $� ������v ��v ��v     ��v     ��v ��v ��� ������� ����f��������v ��v U��]Í�&    �t& ������=�v ���������tÍ�&    ��� t�U��� ��V�5�� S�}�%9�   �|� ʚ;i� ʚ;���[^]Ív �����W��V��S����v d�    �D$1�����  ��   ���  ��u��� ��0��  1ۡ�� �  = $� �  ��� ��v ��v �5�v �=�v �$��� �T$���H������   ������ t����)Ɖ5�v �5�v �$T$�=�v ��v ��v ��t5���  ��   �������  ��v     ��1�����v ��v �D$d+    �  ����[^_Ív ���8  ��������� ��   ��� ����1���� �����v     ������t& ���� @���  $� �������&    �v �؃� �������� t�����؃� ���������    ���  ���  ��v �5�v ��v �1�����    ��v     �`������� ��� 1�1�	��F������  �t& ���  �)������ ��� 1�1�	������ɾ  ��&    ��� ����1���� �����v    ������v ��� ���   �� ��������L$����v    Q����Q �� ������L$��������)�Y��Q �� ��� 1ۉ�� �����v ���  ���  ��v �5�v ��v ������    �D$��Q ��v    P��������T$����)�ρƀQ �� �5�� ^�=�� �����������������������Í�&    ��&    ������WVS��d�    �D$1���� @u��v �t�D$d+    �`  ��[^_Ít& ���������$�\$�D$������ e��R�ʉ�P�D$������L$�D$�T$[^��?KL v]���� ���~N;wO������}�%9��   ���   i� ʚ;� ʚ;��� /hY�� j �щ¸`v j j ��������F����v �    ��tk    <����)�ډT$�T$1ɉD$�D$    ������Ã������������Ƹ}�%9�   �|Q� ʚ;i� ʚ;��׉�ω��� cC��ҙ   ���`������� /hY�� �O�����    ��.hY�   ��:���������������������&    ��&    ��������� @u��v �tÍ�    �    ��� �    �������&    �t& �����U��WVS�Ã�,��L$�|$@d�    �D$(1���� �$���ƀ�  ��v �D$���D$�� ��  �D$�T$��    �t$�C��v �S��v �D$�T$�=�v �$@  �   �T$Dǉ|$ ��I���D$����i�I���׉�������y�؃� �ډ����؃� ���ډס�� �s�{�C���C��� �C8   �C ���C$�$�C<    �C(��� �C@  ��C0���C4�    �CD    �CX�D$�C\    �8�C`    �Cd    ���   �Ch    �Cl    �Cp    �Cx    �C|    ǃ�       ǃ�       ǃ�       ǃ�       ǃ�       ǃ�       ǃ�       ǃ�       �E �U�D$�CH�SL�E�T$�CP���CT�D$��u�u��Mb��������)�SP���ST�U �E�5�� �-�� �T$9�����  �D$(d+    �C  ��,��[^_]Í�    ��v ��v �L$ ����   �$�5�v �=�v ��    �t$�L$�|$��  iL$ �  ��ʉ׉�����y�؃� �ډ����؃� ���ډ׋D$��t�s�{����f���Mb���C    �����S�����t& ��C��v �D$������D$�T$�GP�D$�WT�GX���G\�1�����&    ��D$ ��v �w �G��v �W�G�����G$�D$� �GD    �G0���G4�    �G@�����  �� t�$    �$��� ����  �$%    �D$��t^�  ��cis  ��D$� � �T$1�t$;T$��t$MT$��^��MD$�T$�� ���    MT$LƉ�v ��v ��t�C��� ��t�C ��� �� t%�C0�t$�P��Dº
   9�O�1҅�H£�� ���t�S0��� 9и    C4��  ����   �5�v ��v �t$ ��@t�SX�    ���@  ��  �L$ ��v �G�O��v �T$�O�L$�$�W�O(���O,�L$��    �GL    �W8���W<�OH�����t& ��|$ ��  �T$ ����  �$ �����G��T$�   ���   �D$���� �CH�SL�������&    �$$�����$�D$    ��� �C����t& �$�����T$�C��u�����9�Lº@B 9�O�i��  � �2�9Љ�M� e�9�O������+�v ��$���t$�T$��� ��x  ������$��v ��� ��v �Ѐ俣�� ���   �,  ��u��   �  �L$$��@�T$�L$ ��� �D$Q�L$����������D$�T$ X��� �H�   ���$9�O��   +�� ���1������� E�EƉT$�T$�$�D$�v �v �щ¸ � $L$1�9։�ξ    M�M���^���� ʺ    M�L���  ��v �T$�t$ ��v 1�������5�� ��T$$�t$��    ��v �t$Z��v �g����t& �C(����   ��   ��u&��v     �@   ��� ������� ����4$�$0�� �  	$��� ������&    �D$�o����D$������    ��������T$1�1�	и   D�������t���������v ��� ��v �$�C(������&    �v �$�E����G�T$�   ���   �D$�������CH�SL������&    �D$���>����v �D$$�D$    �D$    �D$ �����������%�� ������v �D$$��v �$    �D$    �D$    �D$ ���������f�f�f�f��������������v ��v Í�&    ������U�    W��VS���t$4�D$�ȉT$���1҉���������   ��1һ    �D$    ��T$�D$�D$0�$�D$�	�v �,$tV�$�D$1ۋT$������ E�E�D$T$1ۉ���t��1����Éȉ����ډ։�����1��� E�E��	�u��|$�$��D$���[^_]Ít& ��������	��J����������	�u��6���������UWV��S������(w ����   �š�� =�� ��   ��� �x�=�� ��   �dw ���t9���   ��t
�GD ��   ��tx�    �@w ������� ��   ��  ����t=��� �X�=�� tZ����t9dw t?�C,�@w �������u.�CD� u���#�  9dw t����������E�  [^_]Ív �C0�X�=�� u����Ѝ�&    �W0�zЁ��� �6������_�  ��&    �����h6�  j�j h�I �������= ���w�������&    f�Í�&    ��&    ������U��W�Ǹ w VS��������5w �É���$  ��� �$    �w     �BЁ��� �&  ���$��1�Ӎv �@Du9�u��u��t�Q89P8~���   �P0�BЁ��� uщ������   �w ����   9�t%�x� �B���x� tf��P`�`D�B���x� u�5�v ��u\��tX�x� =x� tLj 1ɺB �w j ������    Z�    Y�  �w �    ��t��иw �������v    ���ڸ w [^_]������t& ���� �BЁ��� t�4$���������_����t& ��5w ���M����v �$���������U��� W��VS�������� =�� t�X�1��   �t& ��������t�CD t#��    �s,)�hN�  H�P�7P�����ƃ��C0�X�=�� u��   �    )�H����� �����hR�  SW��������[^_]û   1��׍�&    �����S��� ��������dw �p,hT�  h   S������ø�� ������؃�[Ív �����U��WVS��(��v     ��v     ������   ������5`� ���Q  ��v     �    ��   �    ����   �    ���G  �    �    �����9�u���    ������ǉ�;    �2  ��=�v �    �   9�G����bf���Љ�9�vF���v ��9�tG������    1���B��    �����9�wѡ    ��u��    �    9�w���9�u���&    ��������v ��v ��������!  ������D$$h�v �5    P�u,hD ������    �D$�������D$�����D$����D$    �D$    ��    �$��v ������$9��p  �<$9|$$tߋU ���������D$���T$��6 j�|$������U ��������}�u�T$$��v �D$ ��v ��;D$\$Y����   �L$�\$ 9�Ӆ���   �L$�\$ +L$\$�ȉډ������!�!��Ήٿ    �\$����!�!�	�t$E�E�1��U���eʋM������ E�E׉��t$9�׉ߋ\$L��|$�|$L�9؉|$����ߋ    L���MD$�|$�D$�������&    �<$��=�v ��v �L$��v �\$ 9��}�uӅ��!����$���v �u�}����f��   �����d�    ����   �������v ���}�  ��v �����  ��([^_]ø   �����d�    ��tj������u,h�C �����[^��([^_]á    ��v ��������v ��v ������&    ��ۉ������t& �������f�����v �T��������돍�&    �t& ������`� ��u��������&    �t& �����V��S���5`w ��t
�`w     ��� �X�=�� u�\��&    �C0�X�=�� tH9�u��u��CD�t�CP����  �KT����  �`w ��t�@89C8~��C0�`w �X�=�� u���[^Í�&    ��    �����WVS�Ã�;w ��   9dw ��   ;`w ��   � w ������¡w 9�t�CDu,�s0�K4�C0�N�1�C0�C4� w �����1���[^_Ít& ��{`�sd�K`�w�>�K`�Kd��v ��t���t�x� =x� u��w �$������$��v     늍�    �   �����;w �;��������뇍v �   �~���9dw t�;`w �+�����&    �v �   �F�������������S�ø�� ������S0�C09�t�������ø�� �������[Ít& �1۸�� �������[Í�&    �������PD�H0��σ�@�PD�P09�t5�PX��t������(w ��uÍ�&    �    �h� �    ������v �@8    Í�&    ������U� w WVS��X������-�v ���{  ��v �D$L�x� �h�=x� �*  �l$@�1��&    �v �5(w ����  �D$@�@`�x��|$@=x� ��  �D$@�@D��@�$uʍ�&    �v ��w �������D$$�D$@�T$(�������D$D�w �T$H�������D$�w �T$�������D$,�T$0��-w �D$+D$$�T$�u�}��T$(��!����L$�щ�����!��D$�Ӊ\$��#D$�Ӊ\$4!�	��C  �$����  9    ��  �D$@�@D�$��Q  ��v ���C  �\$@�=w �Cp�w�|$�St��D$�D$$+D$�T$ �ŋT$(T$ !����l$�Չ�t$!�����Љ��|$��!�!�	W�D$DE�E�T$4�W�t$�{�sl�l$8�k�[h�T$�T$H�t$T)؉\$P���Ӊ�!Ɖ�����!�T$0�T$0�Љ�!���!ڋ\$@	T$D�D$$E�E��K�k�Sh�T$(�L$,�L$H�Cp��v �Kl�St��������L$8�D$�L$�d$��L$4������ t��1҉D$�����L$,�T$��1���|$������ E�E�1�D$�T$�W ��S �\$+\$�t$��t$���ك� ��H�H�9����  �D$@9dw ��  �$��!���J����w �@D�;����4$�|$@���� �GD�(w ������9=dw ��  ��   �GD�v �    �h� �    ������������    �U�E�M�T$1҉D$8�D$�T$<�e��L$������ t��1ҹ�� �D$9��T$��r����L$,�\$0+L$\$�ډȋ\$4!��|$!�!�!�	�u<�t$8�L$<�������1���L$������ E�E։ƸP�  ��9���'  �$�$;    �V����],������4$�t$�t$SPh�F ������D$X�>�����������|$L��t���v d�    �    �����;    sM�w ����   ��X� w [^_]������$�|$@�T$(���GD�D$$�Wt�T$H�Gp�D$D�Wl�Gh�����    �    ��t���w ��u��,�P\�ǅ������������GD�$�$��!���:���������w �  �¸w ������[�������������],�����WVSPh�E ������t$$�t$$�D$\�p,hPF �������$������|$@�_,�����SPhG ������w �p�p�t$4�t$4�t$D�t$D�t$<�t$<�p,h�G �������4�w�w�t$\�t$\�t$X�t$X�t$(�t$(�w,h�G ������dw ��(9��*�  �`�  �],������4$SPh�E ������������������dw UWVS��t�PD��@  ��@  �7  � w 1�������x� �Ƌ�J��x���x� u.��   ��t����   �AD�G`�W`����`��� ��   �ǋAD�@tЋY`�Ad��� �C��A4�Q`�Qd�Q0�B���� �A0   �A4"  �PЉ��A8    =�� u�"��&    �B0�ݍP�=�� t	�ËB8��y�E �Q0�   �P�A0�i4�U �AD�I�����&    f���v ��t�w ��t%�x� =x� t�� w �������[^_]Í�    �w �������v     �ы`� ������������������&    �v �������� ������l�����u��� �����1�Ív 1��������� �����1�Ít& �����UWV�Ƹ�� S�������� w ������V0�ŋF4�B���� �F0   �F4"  �^8=�� tj�PЉ���� ���&    f��B0�ύP�=�� t��;Z8~��V0�P�F0� w �~4��������1�����1��p���1��������� [^_]�������� 뼍t& �����VS�ø w ��������CD@u�S0�C09�t
�S`�C`9�t��������[� w ^������t& �x� �x� �B�S`�Cdx� �͍�&    ��&    �����S�á`w ��t+9�t;�PH��t�������u�`w ������� w �$w [�f��=�    �u�  [Ð� w [�$w �f������U��WVS�Ã��D$�Љʋ`w ���  9���   ��������� w �$w 9����r5�`w 1�1��D$�D$9D$t�PL��t�����������[^_]Í�    �=`w �|$)ȋ|$ڋ\$�w��K�[!�!�����$��L$���҉\$�$��!�!�	�u~�L$�d$�Ɖ�1������� E�E��؅��g����    +L$1��d$������ E�E����A�����&    f�� w �$w 9����r<��1�1�[����^_]Ít& �L$1��d$�Ɖ�1����փ� E�E�������t& �|$�|$�����v �������� =�� t'S�XЍv �SP��t��������C4�X�=�� u�[Ív Í�&    �������� S=�� t!�XЍv �ST��t��������C0�X�=�� u����v [Ít& ��������v Ív �����U��WVS������u�}�D$������T$������D$    �$��t
��1���D$�$����T$9��C�C��D$��)�1�������L$����� �ME�EӅ�t�]�3�{����[��^_]Í�&    �����U��WV��S�����  �@ ����  �F P���^�   1���$�х�t���Q����Љ�����k�d)��ʋ$�d   ����F������  �n�Q��&    f�����   1���^�n�щ$��t���Q����Љ�����k�d)��ʋ$�d   ����F�����r�9�r��F�V�$�F$�T$��P���t$�t$�����R�FP�V�v(�v$�t$ �t$ �v,h�I �������4[^_]Ð�@�׋V1ۉ$�T$����t��1����É�����1ۉ�����   �����ډӉщ�	�t/�|$ ��   �X  9º    ���   i�X  ��&    �v �F�^��$� ʚ;R1���P�T$��������F Y[���]�����u&��1Ҹ ʚ;���@ =? FF �G����t& �=? �7����0������r*�n�������&    ���1������4����v �ǉ��f����=�    u��v,hXI ��   �����X�N�n�^Z�y�����&    �����UW��V��S��������{<�  �C@�����  ������������� ������ w ������š�� �PЉ�=�� ��   �{8��� ���    �B0�΍P�=�� t��9z8}��S0�P�C0�C`�s4��SD�C`�Cd���}   �x� �x� �B�cD�S`�Cdx� �� w �����1��6���1������CD�t-�sP�����  �KT�����  �`w ��t�@89C8~�`w ��� �����1�[^_]Ív ��t��� �SD뒍v �C<    �������� �8��������UWVS�Y�������   �ƉȀ|�
tu�˃�rN��j�׃���
�L�)�L���)�߃����r�$���1��1�D5 ��9�r�$�� ��[^_]Ít& ��t���
��t��L�f�L��Ս�    ��t��f�������ō�&    f������V��S��$d�    �T$ 1ҋt$0���������Å�xD��� �������� �X�=�� t�C,���������t5�C0�X�=�� u帀� ������������D$ d+    u%��$��[^Ív �������ø�� �������D����������&    f������S��� ��������L$�غ@w ������Å�x1��1������ �������[�f�������U�    1҉�]Í�&    ��&    ������U��WV��t& �    ��u�5    �=   �    9�u߉���^_]Ít& ������U�   ��MbW��w V��� S�Ǎ��  ��������w ��  ��������w ��Љ�����)�)˙��1�����t��1���Ɖ�������1�  ʚ��;1�ƅ�t��1���ŉ����1����   ��w ��w �����1�[^_]�f�f�f�f������Í�&    �v �����WV��S�XD�Bt\������C �~�6��	�t9��������    ������;    r�{ u4������C��������	�uΉ�[^_Í�&    �������C�S뗍v 1ۉ�[^_Í�&    �������D$�T$��t�L$�����Ít& ��D$PR�����XZ�f������U�ŉ�WVS��<�D$P�T$8�T$T�D$,�    �    �T$0�D$4��Qh��  U��������8�D$���D$(    �,$��&    �t$(h��  �|$W�]����t$Vh��  W�M����vh��  W�?�����$�5    h�  W�+����vh(�  W�����v$�v h<�  W������(hV�  W������D$4F �D$��T$8V$1�1ۉT$������N�D$X��Z�|$�����   �t& �V�N�~�T$�V�L$ �N�T$�T$�L$�N$�L$'�����WVShf�  �\$S�~����|$;Whw�  S�m����� h��  �\$S�[����L$$�\$(�ȉ�+D$T$RP�t$$�|$(��+D$��T$ ��RPS1�QWVhDJ �t$,�����|$4��0�������w�D$���)������!��������������t9�r��|$�Ƌ�������T$������D$(�D$(�D$(���@����,$�t$4�v$�v h��  h��  U�����F1�R��Ph��  h��  U�x����F1҃�(RPh��  h��  U�^����F1�RPh��  h��  U�F����F1҃�(RPh��  h��  U�+����F1�RPh��  h��  U�����D$`��(�����1҉Ë@,RPh��  h��  U������s8�s4h��  h��  U������C01҃�(��R��Ph��  h��  U�����CD1�RPh�  h��  U�����CH1҃�(RPh�  h��  U�����CL1�RPh$�  h��  U�q�����(�sT�sPh0�  h��  U�X����s\�sXh?�  h��  U�B�����(�sd�s`hM�  h��  U�)����sl�shh[�  h��  U������(�st�sphj�  h��  U������Cx1�RPh{�  h��  U�������(���   ���   h��  h��  U��������   ���   h��  h��  U�����    1҃�(RPh��  U�����l$`�D$d��  ��L[^_]�y�����&    f������WV��S�|$��Qh��  P�T���������  Wh��  S�=�����h�  S�/���XZ���u  �v`h�  S�����v�vh"�  S�����v �vh9�  S�������,�v$hP�  S������v(he�  S������v,hz�  S�������$�v�vh��  S�����vh��  S�����FD����tPh��  S�������F8��tPh��  S�z������F<��tPh��  S�d������F@��tPh�  S�N������FH��tPh�  S�8������6h0�  S�(���h��  S�����v4hF�  S������ ��yh��  S�����XZ[^_Í�&    ����������  ��t�P`Rh\�  S��������čt& h��  S����YX�|�����    h�  S����Y[[^_Í�&    ��&    ������S��������Hj�������������P�5    ho�  S�^��������P�5    hxJ S�G�����$h��  S�9���XZ[Ít& ������WVS�2���B���t;��t��������HV�������X1�[^_Ít& �r���r������1�ZY[^_Ð��t���E���1�[^_Í�&    �r�zh��  S����jh��  S����WVh��  S������$h��  S����1�[^[^_Í�&    �����S�XD�    ��Q �������;    r�{ u������C��[�1���f������WVS�����h��  �Ɖ�j �#���jh��  j ����WVh��  j ������$h��  j �����[�����X���&    W��1�V����ZY�غ    �������;    r�1�������)������&    ��������HS�1��,���X�غ    �������;    r�[^_Ð�����UW��VS��(d�    �L$$1ɍt$�\$<��Q V�����Y|$�Ɖ���  �v �����Q x��Ⱥų���   �����Si�  )Ѻ�����������Sk�<)Ѻ�$I���N����������)�    )ЉʍL$)BHЉ��S�����b ��   ��D$�T$ Qj h�: ��������֋D$(�T$,k�d���a�, �D� 1���l$�ŉT$�׺��O[����d   ���T$��D$�D$�T$�T$t$`�A^��1����D$�T$������   ���D$iD$]  1� ����������x�����t$4$i�qz  ��������x�vm���������l  ��L$�S�s�C�K�D$$d+    uI��([^_]Í�&    ��Q ����Q ��e�����&    �v ���D$�Y����v �L$��;������������V��S�É���������C�D$�S�T$�C1��S�N1��C    �� �C     ��������������C�S[^Ít& ������U��WVS�Ã����� �������3�{�$+C�T$S��#F#N�f�NʋKCS !�!��{ �|$�K�N1��{������ E�E֋4$CS�C�s�S�e�[^_]Í�&    �v �����U��WVS�ÉЉʃ����0��s�{�$�t$ �q�|$$�y�t$(�t$ �|$,�|$$)��T$�ƋD$(��{#T$,!��s�|$�t$�s �[�t$�\$�Y�I�މL$�L$(�\$,����9��s`�D$(�T$ +$�L$$L$!ЋT$,!��L$�������+D$T$1������� E�E։ƋD$�׋T$)���e�[^_]Í�&    ���L$�����D$T$1������ E�E�D$T$�e�[^_]�f�f�f�f�f������UWVS�    ��t)�։ύ�&    f��C�+���t$���������X��u�[1�^_]Í�&    ��    �����UWVS�    �t$�|$��t�Սv �C���W��V������XZ��u�[1�^_]������UWVS�    �t$�|$��t�Սv �C���W��V������XZ��u�[1�^_]������UWVS�    �t$�|$��t�Սv �C���W��V������XZ��u�[1�^_]������S� x ������x �¸ x �������[Í�&    �v �����U��WV�p�x�@@�@�Ōx �����)�׉�^��_]�f������U��S�ˉыP`�@\)�ڋ]��Ít& ������Í�&    �U��W��V��S�Ã��S@d�    �E�1��U�������U��    �CD�U��to�M�U�   �������tC�M�U�J0�`   f�J4�M�j<�B8    �H�M�p�xj �u�R�   j jS��������E�d+    u�e�[^_]Ít& ���u����������&    �t& U��W��V��S���^@d�    �E�1�������    �FD�]����   �M�U�$   �������tX�M�U�J0�`   �j<f�J4�B8    �x�O@�H�O�_�H�M�X�]�H�M�Xj �u�R�$   j jV��������E�d+    u�e�[^_]Ív ����s������������&    W��V��S�Ã�d�    �D$�C$��  u@�   �ډ��������t�T$,�p�x�P��������D$d+    u��[^_Í�    ��u����������t����������&    f�V��S�Ӄ�d�    �D$�F$��  uS�    ����������t*�X�S@�P�K�[�H�L$(�X�\$,�H�X��������D$d+    u��[^Ít& ��u����������t����������&    f�U��WVS�Ë�x   �������t[^_]Ív �M�u���  �}h �  �����C@�����WVPh�J S�������`  ��[^_]�����U��WVS�Ã���x   �������t��[^_]Ít& ��M�U�u�}�L$�M�$���  h�  �����C@�����WV�t$�t$P�uh�J S�������`  ��,[^_]�����W1�V�t$WV�����XZ^_Í�&    �t& �1��������&    f������W��1�V�֍RS�Քx �Í@������{<�s@��C0 l �CD    [^_Ít& �����W�|$����w�U��WV��S���E�ЉʉE؉���ӉU܋W�߉E�F�U�V��)�ׅ�x?��;u�}��    �   }A�u��u�������]�C�SXZ�e����[^_]�g�_Ív �e�1�1�[����^_]�g�_Ít& �u��xQ�u�ȉ��u�������Ɖ׋E�U����֍�E���ڋ]�CS�C�SY�M�[�]�9��|����� �g�����    �����U��W��V��S�Ë@@�@�Ōx �����W�щ�V�����Y[�e�[^_]Í�    �����U��P��WV�p�x�QR��������Y^�e�^_]Í�&    f�������PH�@H    ��t�������1�Ív U�   ��]�����f������W�|$����w�U��WVS�Ã�,d�    �E�   ������щE�������E�d�    ��E̋3�C�EЋ{�C���  �Rr$z(B,���R��P�E�������E؋U܉�E��S�CXZ�E�d+    u�e�[^_]�g�_��������&    �U1���]������t& ������UWVS�Ã�8�x d�    �D$41�����   ���  ���   �C$������@xtp���   ��tf�|$1��   �\$�D$�D$��  �D$�����������= ���w�   �F������ x ������¡x ��t-� x �������������D$4d+    uf��8��[^_]Ít& ����  �$������$��t��؉x 1�1�������$먍t& �� x ��������f������띍�&    f�������������t& �����UWVS�Ã��x@������x �T$��������$�C������ƅ�yw�$���������Ōx �����>�t& ����[^_]Í�&    d�    �    s�d�    �    ��t�IR��P�������XZd�    u������뮍�&    �CDt��T$���x ��������cD��i��������S�����    ���������x�[Ð�������P�����v �����UWVS�Ã����s,�$�v�����x �D$����������C0t�D$�$�x ������c0����������k(����   �v�Ōx ������щ$������ŉ����������tA�C��S��l$�C�S�ńx �C�S�C0��   �$��L$�   ������K0�L$����������v�Ōx �����>�t& ����[^_]Í�&    d�    �    s�d�    �    ��t�IRP�T$�������XZd�    u������묍t& �����������|���f��$��L$������L$�H�����&    f������UWVS�Ë@@�4@�����x �Ƅx ��������S�Kj �ǍCj �S j �K$�S(�K,��������CDu�ډ�������KD��[��^_]������t& �ډ�������׍t& ������UW��V��S�Ã��h@�Tm �����x �T$�$������T$�s�D$�{���x �CD��   �ډ��s������KD�C�Sj j j �щ�������T$�D$������Dm �Ōx �������>�t& ��[^_]�d�    �    s�d�    �    ��t�IR��P�������XZd�    u������뷍�&    �ډ�������V�����&    �v �����W��V��S�Ë@@�@�Ōx �����R��P�������Y^�щ�[^_������t& ������UW��V��S�|$ �Ël$u&���   �@�Ōx �����R��P��������Ɖ�XZ���t�s\�{`[^_]Í�&    �CP��[��^_]�X�����&    ������W�|$����w�U��WVS��d�5    ��<�U��?�M�d�    �E�1��sH��&    ��   �F�U��Mĉ�������CH��t�������t& ����������x�CH��t��   u��t��t& �d�    �@    �[H����   �    ����   d�5    ��D  ����   ��Ōx ������M��]�)��1�9�ظ    |�U�d+    �   �e�[^_]�g�_ÉʍEȉ�������EȍU؉E؋ẺE܋EЉE���8  ������1����������j�������   ��t9�=�    �P���Wh��  ��   �����ZY�5�����&    ������_�����x �   �@������M��]�)��Hx ӉM��]�������M��Px �áTx 9U��|	�u(�E��U��5`x �Px �E��Tx �UģXx �\x �ڸHx �����������x �   �{����������&    �v �����S� x ������x �¸ x �������t� x �������1�[Í�&    �v �����U�hPWVS�Ëp<�x@���   �@�Ōx �����W�щ�V�����C$XS(�C\�K`Z��[��^_]������&    ��&    ������V� x S��������5x �¸ x �������t�    �    �C    �C1�[^ø��������    �����WV��S1ۃ�t1ۃ�	�Í\�� x ������=x �¸ x �������t�[�Őx �������1�[^_ø�������t& ������U��VS1ۃ�t1ۃ�	�Í\�� x ������5x �¸ x �������t�[[^�Ōx ]�����[����������^]Ív �����WVS�ø x ������5x �¸ x �������tu�#   �������t`�C1���t��	�����t ��v�Cd1ɋՔx �{P������{P1�ǃ�    l ǃ�   �x ���   ǃ�       [^_Ít& ������������鍴&    �v �����U��W��V��S1ۃ�X�D$d�    �D$T1���t1ۃ�	�Í\�d�    �$� x ������=x �¸ x ���������  �������B  �#   ��������@  �[1ɋŔx �D$������N�}�%�D$�D$8 l �D$��D$D i �v9º   �\$H��D$L    ��   � ʚ;i� ʚ;1�����׃�ty�[�Ōx �����WV�����S�׉Ɖ��D$�Y�����=����t�T$Td+    ��   ��X[^_]Í�    �$��@  ��L  ��P  ǁ<  �]  ����&    d�    ���  �@=    uRS����D$�����Z�����=����D�녍t& ����������;����������g�����    ������W�����    ������P�D$�����Y�Ɖ�떸�����/����������&    f������U��x WVS��H�D$d�    �D$D�Hx ������Px �Tx �Px     �Tx     �¡`x �$�Xx �\$�\x �D$�Hx �L$�\$������ x ������=x �¸ x �|$������D$    ����  ��������]���������tD�s�{�E�����)Ƌ$׋T$��	�t9Ɖ��}�C�S�4$�|$�D$�D$�T$�D$���|$��   �l$�<$��	��  ���5w9��    ��  >�t& �t$� x ����������T$ ������t$@�t$@�t$@�t$@�t$@�t$@�t$@�t$@�t$@�������$j j D$T$R� x P��������Ã�����   �D$Dd+    ��   ��H��[^_]Ív �D$   ������v d�    �    �Q���d�    �    ��t�@�t$�T$�L$�����Xd�    ���������������&    f�1��s�����&    f��D$1ɺ�  ������U����D$1ɺ�  �����������;����������&    �t& ������U�h�W�x�VS�É���������C�    �ƋC�C�tD�C�P�S��1��������t�S��C�щ$	��T$u#������������[^_]Í�&    �1����t& �C@�   �@�Ōx ������t$�t$�щ�������C�   C�XS�Z�C��f�f�f�f��������    �    �B    �B1�Ív �����U��S�ˉыP`�@\)�ڋ]��Ít& ������Í�&    ������S�Ë@�C<    �C@    �P4���������x�C     1�[Í�    �   [Ð������P1Ƀ�P�����1�Í�&    �t& �����U��WS�XP�p@�p<�������Y[�e�[_]Í�&    ��    �������P������v �����U�պ}�%WVS�Ë�y�q9º   �|+� ʚ;i� ʚ;1������S���у������Z[^_]Ív ����������ލt& �����U�   ��]�������&    ��    �����U�   ��]�������&    ��    �����U1���]����������U��S�Ӄ���� d�    �D$�   ������щ�������$��D$�C�D$�C�D$d+    u�]�1���������t& �����U1ɉ�S�Ӻ@B ����� d�    �D$1���������$��D$�C�D$�C�D$d+    u�]�1����������&    f������U��WVS�Ã����0�K@�s�T$�S<d�    �D$,1��C8�D$��	�uw�C ��u�|$uG�C�V������$�T$�L$�v0�؋$�������1ҍ|$9����   �|$t�D$�@   �D$,d+    ��   �e�[^_]Í�&    f��D$��������D$�L$��D$�A�D$�A�C�V������$�T$�C4u�|$u�$�L$�؋~,�����C$S(�K���f��L$�v0�؋$�������1�9���T������������D$�|$�G�D$�G�D$�G�7���������t& �����UWV��S�Ã��h�$�T$ ��t������C<    ���C@    �U4������¸   ����   �C4�C     �C,    ���C0    ����C4�FF��   ��}�%�N�~9и   ���   � ʚ;i� ʚ;�|$���D$    �D$T$�S@�}�%�C<�F�N�v9º   ���   � ʚ;i� ʚ;1�������<$��uR�s81Ƀ���Q�щ�W�}8�����X1�����Z�C 1���[^_]Í�&    ��F���<�����[^_]Ít& �d�    ���  �I��    t����sQ�щ������Y끍v ����������/�����&    f�����������O����������������1�Ð�����U��]�����f������WVS�Ӄ�d�    �D$�������d�    ���  �@�p�x3�@ {C���R��P�D$������D$�T$��D$�S�CXZ�D$d+    u	��1�[^_��������������������t& ������������1�Ð�������1������f��������d������ �������&    �t& �����S�Å�u$�CH������CL��������   �� [������v ��x ������K�C���t�H�C"  ��x ������f������U��W��VS�Ã��S�t$ �D$$��D$�CP���$���D$��u��`�  �����  D��C�$�L$��������Cl�� ��u�Cp�@�����R��P��������ǉ�XZ�|$ �{\�k`�{d�kht��[^_]Ít& �j ����j j �D$���������[^_]�f������WVS1ۅ�xw�ǉ������d�    ���  ��1�i�G��a�����x ��u�A�v �C��t7����t0;PDu�;xu�x����������d�    ���  9CDt������������1ۉ�[^_Ít& ��������[^_Ít& ������UWV��S�Ëh�x�������C������U<��t��������������[��^_]������    ��t& �����V��S��d�    �T$���������tW��1�1������r��C��t6�H$��t/���1�������$�C������D$d+    u����[^Ít& ������Ӿ�������������&    �t& �����U��W��VS�Ã�d�    �D$1��L$ ��t1҉����r��؍T$�$�?����$����u4�d�t& �p��t.Q������������X��u!�؍T$����1ɉÅ�t2�C��u�������T$�C������D$d+    u����[^_]Ð�������������t& �����W�|$����w�U��WVS�Ӄ�,d�    �E�   ������щE�������E�d�    ��E̋3�C�EЋ{�C���  �Rr$z(B,���R��P�E�������E؋U܉�E��S�CXZ�E�d+    u�e�1�[^_]�g�_��������    �����U�hPWVS�Ëp<�x@�@p�@�����W�щ�V�����C$�CdS(�Shj �K\�[`)��SQ�щ��������[^_]Í�&    �����U�պ}�%WVS�Ë�y�q9º   �|[� ʚ;i� ʚ;1��������uS��������Z[^_]�d�    ���  �I��    t׃�Q�щ������Y�ō�    ���������뮍t& �����UW��VS��$�D$�T$�L$d�    �D$ 1�����  �|$����  ���!����  �D$�D$����  �P���  ��� ��  ������Ņ��  ������EL����  �@    �P��1��@    ���)J 1҃�����9�r��E    d�    ���  �l$�CT�$�E�D$f���x ������sT��1�i�G��a���<��x �,��x ���  ����t& �@����  ����  ;XDu�P9�u��������    HЉST9$t��x ��������t��l$��v �l$��x �����  ������t$���  �D$�|$�u�E$�����E�D$�E(�����E����  �����d�5    ���  ��  �G���(  ��   ���  ���'  ���  �   �����_  �P	��!  �]H������EH����  �|$�G�W�E8�EL�P�EL��P �EL�U�   �P�EL�T$�@�����D$���������  �D$�P��������   �ƅ���   d�    ���  ��������  �ED���  �PX�j�U �PX�U�hX���  ������   1ۃ��:����D$�@�����1҉��������t$���  9��  u�D$�@�$����?�����1��������D$�x�h�D$���x ��t�D$�G�sT�����������������1��D$�����������D$ d+    �  ��$��[^_]ËEL�E8    �@   �EL�@     �EL�U�P d�    ���  ��  ��t�   ������   �P	�xu�]H�i������    �����    DD$�U����   ������W���������T������ ������������>����   ������(����   �������������   ��������z����   ��������i����   �������������������������������1�������������    �����WVS�Ӄ�d�    �D$�������d�    ���  �@�p�x3�@ {C���R��P�D$������D$�T$��D$�S�CXZ�D$d+    u	��1�[^_������������WVS�Ӄ�d�    �D$�������d�    ���  �@�p�x3�@ {C���R��P�D$������D$�T$��D$�S�CXZ�D$d+    u	��1�[^_������������UW�x�VS�É���������S�S��C�    ����   �C���C�S�1ɉB$�C��S��C���1��������~`�C�C�tX�C �@������?B �$�C�T$�S�9��|�$@B �T$ R�   P�T$�؋L$�����C�XS�Z�C��C�   ������������[^_]Í�&    1��_�����&    f�V��   ��S�Á��   d�    ��$�   1����������um��xq����   ���!؋��  ��tv�H��tv�؉�������Å�y ��$�   d+    u^���   ��[^Ít& ���   ����������t΍�    ���������غ    �����    E�돍�&    f������뙻������������&    ��    W��V��S���   d�    ��$�   ��������Å�t)��$�   d+    ��   ���   ��[^_Í�&    f���xL��g���!����  ��tU�H��tX���������Å�x����������������E�둍�&    �v ��    �����    E�밍�&    f�������]���������S����������&    �����UWVS�Ã�d�    �D$�C��������t�ŋ@@E<u �$�E������D$d+    ua��[^_]ËC9E4u؋E�P(��������E$�U(�E    �E4�E,�U0�E$�����E(�����s����Ƹ����1�9��L��s������f�������HL�Q$1ɋPH�@8�@L�������������Í�&    ������VS��D��pd�    �T$@�P��t?�@   ��������¸������t�T$@d+    u8��D[^Ít& ����������ۍv �D$@d+    u��D���1�[^�c����������&    ��&    �����S���X� d�    �T$1҉��>�����t�T$d+    u!��[Í�&    ��ډ��������tٸ�������������&    �����S���X� d�    �T$1҉��������t�T$d+    u!��[Í�&    ��ډ��������tٸ�������������&    �����S��� d�    �T$��������t3�P,����1�9�X0Mʋ$����������D$d+    u����[û����������������UWVS��8�X�hd�    �T$41ҋ8�P�t$1���EƉ$��tZ�D$�������uD�|$�ɚ;�D$w?D$x9�|$�ɚ;w/�4$����L$�_���Z��u��t�ډ��������t������f�������T$4d+    u��8[^_]������������UWVS��8�X�hd�    �T$41ҋ8�P�t$1���EƉ$��tZ�D$�������uD�|$�ɚ;�D$w?D$x9�|$�ɚ;w/�4$����L$����Z��u��t�ډ��������t������f�������T$4d+    u��8[^_]������������VS��� d�    �T$��������Å���   �C���}   �P ��tv���������trd�5    ���  �������C�B��   �C"  ���  ������CD    �$�C������غ   �8���1��T$d+    u0��[^Ív 뒍t& �؉�������Å��]����t& ��������������t& �����U�hXW��VS�@X9�tf�v �_X�s��P ��t!���������u���������������C��u���C�B����   �C"  ������غ   �|����GX9�u�[^_]������VS���d�    �T$1ҋP��xP��wk���!؋4��  ��tY�F��tR���������uW�N���������T$d+    uE��[^Í�&    ��ؾ    �����    E�묍�&    f������뾍�&    f�������������t& �����S���X� d�    �T$1҅�xY��wl���!��  ��tZ�J���������t�T$d+    uF��[Í�&    �v �ډ��������t׸�����Ѝt& �¹    �����    E�룐������������t& �����S�Ӆ�x,��?���!��  ��t-�J��t-��[�������    �¹    �����    E��А�����[ø����[�f�������P� �1���������S���X� d�    �T$1҅�xY��wl���!��  ��tZ�
���������u��u�T$d+    uC��[Í�&    �ډ��������tڸ�����Ӎt& �¹    �����    E�룐������������t& �����VS���d�    �T$1ҋP��xP��wk���!؋4��  ��tY�F��tR���������uW�N���������T$d+    uE��[^Í�&    ��ؾ    �����    E�묍�&    f������뾍�&    f�������������t& �����S���X� d�    �T$1҅�xY��wl���!��  ��tZ�J���������t�T$d+    uF��[Í�&    �v �ډ��������t׸�����Ѝt& �¹    �����    E�룐������������t& ������P� ����������S���X� d�    �T$1҅�xY��wl���!��  ��tZ�
���������u��u�T$d+    uC��[Í�&    �ډ��������tڸ�����Ӎt& �¹    �����    E�룐������������t& �����UWVS����pd�    �T$1ҋx�P����   ��wo���!؋,��  ��t]�M����   �����������   �D$��x;�|$�ɚ;w1��   ui1҅���d�    �ቐD  ����H  �؋u������������T$d+    uL��[^_]Í�    �ؽ    �����    E��e�����    1�1�똍�&    �v �����뮸������������&    ��    �����UWVS����pd�    �T$1ҋx�P����   ��ww���!؋,��  ��te�M����   �����������   �D$��xC�|$�ɚ;w9��tu��   um�   d�    �ቐD  ����H  �؋u��������    ������T$d+    uD��[^_]Í�    �ؽ    �����    E��]�����    1�1��f������붸����������f�f�f�f�f�f�������U��WVS�ÉЉʃ���� �K<�s\�{`�L$�K@9��|$�L$��L$�t$�|$�t$��   �L$��|$��   �|$ȉ��+D$T$�D$)ȉT$�9���ѹ    s4�\$��&    ��D$���T$�����)��9��r�\$��xs�v �D$�T$�D$�D$�T$�T$9����rE�D$�T$���D$�T$�D$�T$�C\�   �S`1������� t��1�C$S()t$|$�����s��D$�T$�e�[^_]Í�&    ������UWVS�Ӄ��$��R��	�tD�|$9D$���rF�k�{��	�t�Չ;�k>�t& �$�D$ �   ������S���	�u��[^_]Í�&    �;��^s��V��[^_]Í�&    �v �$���  ��  d�    �    s�d�    �    ��t�T$ �@�t$�t$�������XZd�    �`���������V���f������WVS�X�ك�������
$  u���  ��
�   �O�p\�X`�PP�Hd���������t;7��Gs�7�_[^_Í�&    ��������W��V����S����tP�Ѕ�u d�    ��tN���  ��[^_Í�&    �v ��������ǅ�t��u?��td�    ;��  t�G��u�1���[^_Í�    ���  [^��  ��_Í�    1��������t�d�    ���  9��  t�1�븍�&    �t& ������V��S�������1҉��'�����t7��������    �F    ��t�F@B 1�[^Ít& ��F   1�[^������������������V��S������F1��������tR�ÍFP�F    �FP�   ����t(�P	�x�^h�����1�[^Ít& �   ���������f��   ��������������������ύ�    ������@�����o�����&    ��&    �������@�����O�����&    ��&    ������U��W��VS���T$�D$    �D$�t$�_���D��$�D$����   �C�t$�S9$�rR�   �C�C$   ��t��������C    �E�S�U�k�C ��_��tA���S�C��t�L$9$�s��G���D$�D$�W����a�����[^_]Ít& ������������̍�&    �t& ������S�������1҉��*�����t����������[�����������[Í�&    ��&    �����VS�Ã�d�    �D$1������1ҋCh�C���������tc������������   �Ct��t6