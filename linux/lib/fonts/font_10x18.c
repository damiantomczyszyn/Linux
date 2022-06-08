_MSK_GPIO0)
			dprintk(7, " (PCI_MSK_GPIO0     0x%08x)\n",
				PCI_MSK_GPIO0);

		if (pci_status & PCI_MSK_GPIO1)
			dprintk(7, " (PCI_MSK_GPIO1     0x%08x)\n",
				PCI_MSK_GPIO1);

		if (pci_status & PCI_MSK_AV_CORE)
			dprintk(7, " (PCI_MSK_AV_CORE   0x%08x)\n",
				PCI_MSK_AV_CORE);

		if (pci_status & PCI_MSK_IR)
			dprintk(7, " (PCI_MSK_IR        0x%08x)\n",
				PCI_MSK_IR);
	}

	if (cx23885_boards[dev->board].ci_type == 1 &&
			(pci_status & (PCI_MSK_GPIO1 | PCI_MSK_GPIO0)))
		handled += netup_ci_slot_status(dev, pci_status);

	if (cx23885_boards[dev->board].ci_type == 2 &&
			(pci_status & PCI_MSK_GPIO0))
		handled += altera_ci_irq(dev);

	if (ts1_status) {
		if (cx23885_boards[dev->board].portb == CX23885_MPEG_DVB)
			handled += cx23885_irq_ts(ts1, ts1_status);
		else
		if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
			handled += cx23885_irq_417(dev, ts1_status);
	}

	if (ts2_status) {
		if (cx23885_boards[dev->board].portc == CX23885_MPEG_DVB)
			handled += cx23885_irq_ts(ts2, ts2_status);
		else
		if (cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER)
			handled += cx23885_irq_417(dev, ts2_status);
	}

	if (vida_status)
		handled += cx23885_video_irq(dev, vida_status);

	if (audint_status)
		handled += cx23885_audio_irq(dev, audint_status, audint_mask);

	if (pci_status & PCI_MSK_IR) {
		subdev_handled = false;
		v4l2_subdev_call(dev->sd_ir, core, interrupt_service_routine,
				 pci_status, &subdev_handled);
		if (subdev_handled)
			handled++;
	}

	if ((pci_status & pci_mask) & PCI_MSK_AV_CORE) {
		cx23885_irq_disable(dev, PCI_MSK_AV_CORE);
		schedule_work(&dev->cx25840_work);
		handled++;
	}

	if (handled)
		cx_write(PCI_INT_STAT, pci_status & pci_mask);
out:
	return IRQ_RETVAL(handled);
}

static void cx23885_v4l2_dev_notify(struct v4l2_subdev *sd,
				    unsigned int notification, void *arg)
{
	struct cx23885_dev *dev;

	if (sd == NULL)
		return;

	dev = to_cx23885(sd->v4l2_dev);

	switch (notification) {
	case V4L2_SUBDEV_IR_RX_NOTIFY: /* Possibly called in an IRQ context */
		if (sd == dev->sd_ir)
			cx23885_ir_rx_v4l2_dev_notify(sd, *(u32 *)arg);
		break;
	case V4L2_SUBDEV_IR_TX_NOTIFY: /* Possibly called in an IRQ context */
		if (sd == dev->sd_ir)
			cx23885_ir_tx_v4l2_dev_notify(sd, *(u32 *)arg);
		break;
	}
}

static void cx23885_v4l2_dev_notify_init(struct cx23885_dev *dev)
{
	INIT_WORK(&dev->cx25840_work, cx23885_av_work_handler);
	INIT_WORK(&dev->ir_rx_work, cx23885_ir_rx_work_handler);
	INIT_WORK(&dev->ir_tx_work, cx23885_ir_tx_work_handler);
	dev->v4l2_dev.notify = cx23885_v4l2_dev_notify;
}

static inline int encoder_on_portb(struct cx23885_dev *dev)
{
	return cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER;
}

static inline int encoder_on_portc(struct cx23885_dev *dev)
{
	return cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER;
}

/* Mask represents 32 different GPIOs, GPIO's are split into multiple
 * registers depending on the board configuration (and whether the
 * 417 encoder (wi it's own GPIO's) are present. Each GPIO bit will
 * be pushed into the correct hardware register, regardless of the
 * physical location. Certain registers are shared so we sanity check
 * and report errors if we think we're tampering with a GPIo that might
 * be assigned to the encoder (and used for the host bus).
 *
 * GPIO  2 through  0 - On the cx23885 bridge
 * GPIO 18 through  3 - On the cx23417 host bus interface
 * GPIO 23 through 19 - On the cx25840 a/v core
 */
void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask)
{
	if (mask & 0x7)
		cx_set(GP0_IO, mask & 0x7);

	if (mask & 0x0007fff8) {
		if (encoder_on_portb(dev) || encoder_on_portc(dev))
			pr_err("%s: Setting GPIO on encoder ports\n",
				dev->name);
		cx_set(MC417_RWD, (mask & 0x0007fff8) >> 3);
	}

	/* TODO: 23-19 */
	if (mask & 0x00f80000)
		pr_info("%s: Unsupported\n", dev->name);
}

void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask)
{
	if (mask & 0x00000007)
		cx_clear(GP0_IO, mask & 0x7);

	if (mask & 0x0007fff8) {
		if (encoder_on_portb(dev) || encoder_on_portc(dev))
			pr_err("%s: Clearing GPIO moving on encoder ports\n",
				dev->name);
		cx_clear(MC417_RWD, (mask & 0x7fff8) >> 3);
	}

	/* TODO: 23-19 */
	if (mask & 0x00f80000)
		pr_info("%s: Unsupported\n", dev->name);
}

u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask)
{
	if (mask & 0x00000007)
		return (cx_read(GP0_IO) >> 8) & mask & 0x7;

	if (mask & 0x0007fff8) {
		if (encoder_on_portb(dev) || encoder_on_portc(dev))
			pr_err("%s: Reading GPIO moving on encoder ports\n",
				dev->name);
		return (cx_read(MC417_RWD) & ((mask & 0x7fff8) >> 3)) << 3;
	}

	/* TODO: 23-19 */
	if (mask & 0x00f80000)
		pr_info("%s: Unsupported\n", dev->name);

	return 0;
}

void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput)
{
	if ((mask & 0x00000007) && asoutput)
		cx_set(GP0_IO, (mask & 0x7) << 16);
	else if ((mask & 0x00000007) && !asoutput)
		cx_clear(GP0_IO, (mask & 0x7) << 16);

	if (mask & 0x0007fff8) {
		if (encoder_on_portb(dev) || encoder_on_portc(dev))
			pr_err("%s: Enabling GPIO on encoder ports\n",
				dev->name);
	}

	/* MC417_OEN is active low for output, write 1 for an input */
	if ((mask & 0x0007fff8) && asoutput)
		cx_clear(MC417_OEN, (mask & 0x7fff8) >> 3);

	else if ((mask & 0x0007fff8) && !asoutput)
		cx_set(MC417_OEN, (mask & 0x7fff8) >> 3);

	/* TODO: 23-19 */
}

static struct {
	int vendor, dev;
} const broken_dev_id[] = {
	/* According with
	 * https://openbenchmarking.org/system/1703021-RI-AMDZEN08075/Ryzen%207%201800X/lspci,
	 * 0x1451 is PCI ID for the IOMMU found on Ryzen
	 */
	{ PCI_VENDOR_ID_AMD, 0x1451 },
	/* According to sudo lspci -nn,
	 * 0x1423 is the PCI ID for the IOMMU found on Kaveri
	 */
	{ PCI_VENDOR_ID_AMD, 0x1423 },
	/* 0x1481 is the PCI ID for the IOMMU found on Starship/Matisse
	 */
	{ PCI_VENDOR_ID_AMD, 0x1481 },
	/* 0x1419 is the PCI ID for the IOMMU found on 15h (Models 10h-1fh) family
	 */
	{ PCI_VENDOR_ID_AMD, 0x1419 },
	/* 0x5a23 is the PCI ID for the IOMMU found on RD890S/RD990
	 */
	{ PCI_VENDOR_ID_ATI, 0x5a23 },
};

static bool cx23885_does_need_dma_reset(void)
{
	int i;
	struct pci_dev *pdev = NULL;

	if (dma_reset_workaround == 0)
		return false;
	else if (dma_reset_workaround == 2)
		return true;

	for (i = 0; i < ARRAY_SIZE(broken_dev_id); i++) {
		pdev = pci_get_device(broken_dev_id[i].vendor,
				      broken_dev_id[i].dev, NULL);
		if (pdev) {
			pci_dev_put(pdev);
			return true;
		}
	}
	return false;
}

static int cx23885_initdev(struct pci_dev *pci_dev,
			   const struct pci_device_id *pci_id)
{
	struct cx23885_dev *dev;
	struct v4l2_ctrl_handler *hdl;
	int err;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (NULL == dev)
		return -ENOMEM;

	dev->need_dma_reset = cx23885_does_need_dma_reset();

	err = v4l2_device_register(&pci_dev->dev, &dev->v4l2_dev);
	if (err < 0)
		goto fail_free;

	hdl = &dev->ctrl_handler;
	v4l2_ctrl_handler_init(hdl, 6);
	if (hdl->error) {
		err = hdl->error;
		goto fail_ctrl;
	}
	dev->v4l2_dev.ctrl_handler = hdl;

	/* Prepare to handle notifications from subdevices */
	cx23885_v4l2_dev_notify_init(dev);

	/* pci init */
	dev->pci = pci_dev;
	if (pci_enable_device(pci_dev)) {
		err = -EIO;
		goto fail_ctrl;
	}

	if (cx23885_dev_setup(dev) < 0) {
		err = -EINVAL;
		goto fail_ctrl;
	}

	/* print pci info */
	dev->pci_rev = pci_dev->revision;
	pci_read_config_byte(pci_dev, PCI_LATENCY_TIMER,  &dev->pci_lat);
	pr_info("%s/0: found at %s, rev: %d, irq: %d, latency: %d, mmio: 0x%llx\n",
	       dev->name,
	       pci_name(pci_dev), dev->pci_rev, pci_dev->irq,
	       dev->pci_lat,
		(unsigned long long)pci_resource_start(pci_dev, 0));

	pci_set_master(pci_dev);
	err = dma_set_mask(&pci_dev->dev, 0xffffffff);
	if (err) {
		pr_err("%s/0: Oops: no 32bit PCI DMA ???\n", dev->name);
		goto fail_ctrl;
	}

	err = request_irq(pci_dev->irq, cx23885_irq,
			  IRQF_SHARED, dev->name, dev);
	if (err < 0) {
		pr_err("%s: can't get IRQ %d\n",
		       dev->name, pci_dev->irq);
		goto fail_irq;
	}

	switch (dev->board) {
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		cx23885_irq_add_enable(dev, PCI_MSK_GPIO1 | PCI_MSK_GPIO0);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		[�vz�t��e��쥨Jj��/��V�����h(��r/:*�WԻ�%1b�2�9u	a��8���J�D�#1<W�(ޯ�_Xڧ�����T�Ŝ9/���<Z���E��L�R)D���S�P���nד�~�ν�Y�<�)E2,\5u�'�u����ʆ� �����\�|#sg�����)y�H�C*��$|�u�~Z��bV��|�t�B��J���HX�-p�e�7W��R�Gt*Jy�EA� ��Ƥ�5Ԅ nz��W�&�\���dr[(񰳠��Մ0�j�0s�n�/M��n�U�*K�����4�!��@R���b���vP�,D�F�H�ؘ(���v�_��4d5��Ҷ�=IXw�c֩���q��2�&k�gt�ёi%y�~9eh��{��Z��j���b�
s(��E~��T��t+5����Er鬐�7Ob���5Ď0�����Ѯ��6d������vI�7.�p������)�󦐑N>c��B��$�	�*ImR���M�=�<P�|��Ӝ����ڿ)-�΄�]����y�[���˸��3���X�@���K�zj���;���֞����:��D�G�&�48S�
"2�8�,,9�������2�,s�m�<�A)عP���j�|؊I�U�ņ�f�W�)�����ki��ʸ�Q��R�Q�꓍=?+�bN���J�|�2� ���w���!�����ڙwK�5r���_t�@8u��,d�:���<�͸Z<�A�)?�U���{ZA�C0l3"j��	ū���B�/�0AQ��z���tJ�l3^@��t|�X����Z+x.���{
��x&GI	���s�!��F3�����Aj�\|�^��*�?@��k�AU?��T,Mϼv(q+3��B�h�Y7wξ�A�Fr"�8SX��'��&��f*3��"ڽ�3ȁ���#+<�K*�� 0��bCo�{f<(�$���<]D��\���_�&�>���E��/�B(G+*֢Q�3��Ȇ�I]�~jQiNq���4�f
80v7��$=��zىf�!��nH���^�qkʂPFl��'�+�u}���Q�C��<<��R|t��=�pj��
��`U�J�ɹ/w�ޭ��d͹�c�?�$ ԦK�OF [�q�����E������r�i;L篯-)9c�
DG��jM,6�4����>�H]g�hs'�u���v����,����v��%��N>e?d�{��kA��ȼ�d��g��S��+�F���-
��X���/_�������2|��g��9���̆��&�C��)�4�LV3������v9��s��q#ll!�-�c�ˁ��
鷺��cD%��?D��ς��h�?C��+�/M2�q�$�5A�����`{3�8�
�s/m�Lُë�a�0̑�®ŷ9�Оs��.�E�D$�͎#HV�,i�8�� �֦� q�����濩B�p`�r���� �u�6�U҃ś�T��0�yB�4J�d�&`�jɯ)�z�����y).M{lg`���]��[O��-�����dڋ8�@͑jW����T��l�$2��.�u��n6�Vsk��A2�����LgaP�R���O�}e\^�`=�OᅶCF~@ jA����;�XYT�x���I6(re{��^b� M/�;f��w"�  ���h,���`���P{)?&6J��t"f \}�R�*|(
�c��0h'	��&<GG�M��8�ޒ�S��f�Ӝ���x��{�v	2�~:U�s��7D�4ew�xu��D�c�fg�3���o#ǜF�D�#CXP�pmOk���	U�/&��� �k��?y)'Є�)�5��+5�7T(Lf&�7htО�R;�!6������'*�j�mi&���tN�3�',�)��/"�q�����Y��U�[*P���ab�WI�{�o��0(��S�q�JH����d�[L�	�˛61�*��R�1����N��ť2t���,��ୡ�����t��z����:O�vӞ^8�n�J�1f)	DӍ	G�D�����s+LZ+�n�A��a��r@�6�����Ɍ���Kڀ�v\X�Nd�⺑"vǒP�Y�P�wl���߆e!�q�8ڙ޿ԅ�!����5`�@���YTó83=FWS����<�����|�<�9��\ϊ���i�Vv�n��!VI�CU��y֩��Ik�qinn�<||0�%��@��E��+]=>Z�P(o��/\���u��I�AzP!ǒTs�RK���D�k��0�j/�f���ưYuo�*Ϭ��)�o� 6�s=\�q�Cd����J�C�V����@�V����!���}��7���$a�;��[�"~,::��ݬ{U�DҾ~1(��"��J�-����T��>����'f>���O�:�.�/�G�oS������_c�Q:�)V,�ȷڛF�<�y���h׊7���s����Z���d��|eU7B:�lsC	��8'��(2��[��쀍�p��l]��բ)ʾ�K��:|L�?�"J��Ne�.3(�`#mPh�V�^e{S����!�50��3��Գg��D���8x�rҹ����L�����������O��)�]i=�M8�g��wc��%6��B@����ɦ'o����rt������6�����
��P�:H�CX�����n���*d�롳b#�}��:N�GS�>8�C�b�2�n�٭GhҦ#s��Q�-vf��3M�k��Bic���4�z?����X0�{�����L�1�G��)�=�֢����jn��_�2��&X"�����*`p�*�ق����P�ғG	?U!����q��k����熸r�T���v���08O`�0,��휪�9x�4���X�ƍ�|����5n�0�_H_sDU7(�.�����RDqV/1��(�x~��?m�9@�q q�#Ѽ��Ak��b���̽�e�ۏk;}�OU���OGY���Ֆh���i���!H��F#	I�q��o�D�"���4�+c
t3��/�����2�TK��c����^J�7�d-Dg�=T��,�h�Z�M3&��4^U�&׾�����	 �2@���w�F�k8}v����������kݺ�N!a�D��A�G5Hm�ੀ�ÿ�[FN�l�
O�R�ğj+uA���f)֮|�}��Ĳn
.���Kv��AMlG�~=	yK-�������7�x���Fƣ�Z?�u0�7� �M���q�.��u����ND�Y@�=Z��}M0l"b$�����Oa��qƔO��゜Oo�_���V���a�R�r�2J��B�+�8۷�5�4�\=�D2.9[�v�Wip�|[��3r�^�2	�A��Bk���vě�i$c"�ݴ�a.���~o���
0ၾ*�q.���2+v�9|C�&0/���!TVk�s{��[u�<n:����z��ip�d�#
�OnuE�./�O��*J-;�
�1�� k��yQ�]B�g�O�����v~�B�o5ߨ4}ڂB ����:�2���p�j���:\��X�'f/*|����#���V7{
5�ܭ
]�&+��Κ�� bx�1�hm\��� 6��`d�v^
�%���:�w�d�'��;�l�9��ьܩ@�`zST�K�*�*�'���/f@�:j�O�ɦĳ�lb7xN��$鹖�6��Z��<m&Rʐ@��� ���톘d�Joh��8b��1,�O��\_ާG^�R���HpMX����КfR���~gi�/+J(U� �j�֩�(���&�\ogر.�fɟ����	*���6�Yl������)F������R�a��C��iT{{�u��l��M}΅���!^�vl���y��A���6���{��isS�3#����I����^�A &]zh�"*�A;@~t^��#Ұ+?����E�kp�툪���Q|`~I��Ϝ2�P%�di��1���3Y$���?��1��E��ޟ؅�|�S�!X��zÊ�54�ԏ���5#��~�2�5y�#�r���`3U�Ŋ��zqbL�w������o+�*e��G�m�j��O����4�3Ӓ���}��e�;�\"�y) �<+*�`�œ[�4yQ��2�UVy!st�DJ��IԴ�v0�>��̄�6����!��:2�og֮̔P�m�Cw-)��N�)��Wi݃�j��A}���zv�Ea����� b�A�@<����1x�{��և�����Iioc�u>��5��޺E��Ė
mi{�K-�/�J^i�7��h�D^Df�)����7��#���(u����sZw�Ézզ�>�\d�Qd5چ����ݶݝl�� 2s)خ�׃����IόP��"�BqՊ7��*�����u�;vFO߻�ƾ�E�C���P	�sA�X�B��"�$JFl�,����ZӦ%���iꂤ�0}	� (_�pрDSfθ�!q�9<����в���c�y)��|��d#���XA�X|��9`m/�[�*�,a弛�l#x0�ǐ�����dԻ���4.��Z_|�镋Wv�mc\����f��"�L�mGj~W�L�;I�:�ř603>�	��%Mq��0D�h�v��$�o����4���GUI�1\X&wD��;���ֆVcp�$�67S�Ž�Z��$�Ҿ"T3��:Ÿ,��f}�Ǿz���	�i}=��M��A��	�?ukD�l=D�X��)���h#���`tþ�ί,���g⪬uв/A��T)[I����ْ^�=�>��ua�u�P���<;�8+���ɥ,:��m'���	ᓫ��Or��d\��T,=h�a�v��U	���Y]|G���-��B�H-U+�"��Z���I��TB��� �Յ�U��p��9��63U2Ҳ�C�˂9�2JМ�ϑW���nz��>9���֨�!0a82��/���o�q,�k�!�ݶJ�9⪃q�mU�x^�D�*`��Ů�D] ��`hZ_w1�A�����@S�@t!� ��WН��І��+�HT�))R	s+ՂyQ�5Vi�t���z�Bnf�[�D��kq����QNi�����4�����d�K�٦ʐT���l�{["5;��#WM����0-�@;�j���
��2�ء����Y~�$Ȍ��穜~X��mHQXI|/i&�Hq;�{���#�J*�
�5]���5���O��}��8^���Y��'��JX���v�W���x3>��F@O�;],�͕�i���g|��"��"*�t�6��A��������`DG����/��/�x=���L������*�]� �=�i {>���ͼ�V`�0J,�F\��nʒ��x���*�4������4s��bMb�U���C?4�≼�
�`�U� �`��FY,��ǲr�Y�0� P��f��+Zߦln�bA�\�ⳓ���8��)ќ�<=�_����x���{���?.B}�v���@c����c�C���(������yp�h0�l����T��5k��D���g�}p�����=�@A���aC�݀��B�i��1��T����G�´��/d��8�q;r1���PH�EۤFF�|���6's����|L�6_���h�p�uO�=���b�H�'Z�<�K:7&S,*$b9Ԇ�ӗ�#�]�	�-4Øtn��o��2�'�/	�c_A@�]���ق�v2�O-�B��x�� RsK�G=�є��G��Y�� Ծ5�g9bo7�֗�-���'�I� y�7O�B���1��:u��i��� 	} ����v�<��f	iW"5�B�su�Ν6�������	b�K�"5��c� ��?�>���2~��g4:Ay�;��n��H_�l>��[��ö{�W� x��m���T�(l<K�t�,��N:cx��`�3��*��0Z��Iѵ������|}�,���X��N4�a���3o�<�\�2�g}4�c�.�E���'Y,j *�2l)xH[�M̵�	,ĩ&�*"��{=>d�[+�,�+/���B�$���~9�=�M���4)#N'"������kR�	!E�=��S���s��H�?�.��?�����v�i�`H�a���C�.8W��DJ~���u�O��6�y�2�^��0��l$�Sk�������e	���w�<���D�*������vQ�K?�9/�<��N7�<��he�W��Г�k��g�zq/ڇ8��I&)
�ґN� b^��O�o�z�ß��l�h�;wVBG�$"�)!���1ꊔv���4 ܁Y&��^�MT�}�.��'��}�r�i���Tϣ�
!5�;n���\T&Y�����xO��FR��=y�-��#��,�9i�(�E&����(T:�����z�Yt�����8��D?yⶆ���hC�ۺ �|�cG�n����&����㹐�A.�`�~I�w�+�MxJ���k���e!'E�0,��}%RԳP�B+Qn�Iғ�a	���:�Z�t�f5����$�ҡ0P��ݪ1|�g�=���gR]8��3�/P�@O������8�
�Hj�cb"B�ŵ����x�L��g�����M��6�O�b�������y�1sj<���Ӿ\�tHK��͆q�����M��bwڟ_��ОR��]�;=�NB�$�-;���]]����9���1���Cv�*!E����/�d��cQ�6ɓe�ȭkY�����@H�&+ٞ+ЅSBz��{b��7I3pP+�zfֳ��q�~�tc�*��6��0�n�$,����xH���d��� �;�\A�k�G*d�p�4�)boMx=�0��C2V�흈;���"�?�h+�?^��1u�6�
�8��3}Ph���v�s�̯�pg/I[���@Y\�h�[��o*PWX���Z�X���,oi�� ��<h�> CJ���-��63:��>�ma�0��r��3_ڭ�E\��@j�G�+����u���y��;pC~-X��=���(���4#1x�,��w���H��jR��K�����ˉL�ẟ9�U��5;�����mg��3�|�Es�_zL(8Q]��(�ڊ�.�����O2�o�Y��?V�\�n�S[E��M4��N��3a��W�x`�^�(=��p��5 GnT�������9�+���6�T�0��P|�BR0R�� ��8��sD~�EfÞ��uZ=�GQ�D�a�F�zD�Hr�K*9��-��2� �q�oQyz%,��m��D�z٪_8�]Y�p!�N(Zn:�u,������ٚ��>���d
����= �x�U��=g'5,h*���p�A2��_ܚ����?���b�떭�c�Ԁ��,����sw{�a���T��3���b��m��H2	Ҳ�/�U��p�f;�sB>��HP���%gV��e@��a#�uc������dT����rr����]��$s�.�x�"X��w��|n��,3��	�&�����Omo��7�7FYg�.y�p�rje$�%s	�m�3�Q��@�bʝ�ph �1R���(�o�3�2�as����dL��s|�հ��d��n<Z�-^�y��ɂt�H*�&�[/��֥��/����_�{8�c%���$��l���Ǝe�W��5����43�4$������y������{z�pt�׏_����嶺�,����)#����Iu�i�b�p�1��<�'�H��*���������%ʽ���_������̘�h�	�i�㤸W�G^'n0F�D-�p��ӗ�JD�*��}���$,�����c�~���a�ax��c%@Pe�.�唣;��z  4�Q�U'L�^G���(��Fi�:�ԗAݿ=on�v����3��������)�d"�:��ӻ�����`�j2j��+�vS�6�J����A{//|���̀n��l-Gm�f������1m9��j��r�y��u@�-��*yyjʬ$�w��+���|a�o��$�4K̡������/3���U BC؜�Pc1贓����w��_b}
���Oë�{)m�����T��S���^��u,\��Uq>�H��߮H�kze^eJ��^tn����ɐs�n�_:�N�w~\� ���(q����h<�X.�3�x��1� 7������
���+�����$��d��=���;Z��s&wN��D?�AV�FL�f��Tv�<\�@+tl�Ú1�J�dı��C��������	�+�P�F�_�ߙY��H����D~c�K9M�b�c��rM֐�³�	{��lE92���g�Ѝ3�$q� D�D�Bt���2�Sj}���f��WS&�+�&T�*�E.��Ѥ:<L���k	72��B��[qf�K,�a��8y��N�-���ޕ3�v�/�R���ʗ�� $��kW��XD�EoX�7c@���f�������7���	�	mC���!����쿪ޏY?o;.pR��?*�Sq_�&�K<Bړb}A"y󝪩4��*V�(ʳȳ۾ ;t�$�zi�>.��ݯF���(]��vptpr��/�蒘[e�+�/ v�M'�q�c	2�t�p	'�(ԍ��7�?y����gi��/�a�o+��?Q�׬ýP�x�@� �@��o�+T��U˯Hh��}�><��:K�ϐ��x !���:����9�oD����b{��6��v4������c��MVw��E���	�Gl�!���"�E
�#e0�/��5�Y���͗|k���<5u�iA.���GU��JqF8�lk�8]��ѓ��|�d�8�*5=[x6��V�ݘ�TH�nU���P<��
F�ئV��!o���a��C
�?�<F�c$�䛉T==ĳQE:Vפ�4�rxW��;���Y.��m��"A��`g,Sw��FG&�Y��C{�E٤�pP��Ǥtzig.v��:�c�Eً mF��<m��9_\���.���M*R����W?6\R���1N���?��<I=�6 H����H�Ť�vU���{��t䗂�f.��ӑ�����W����NzTD1j+m�rk�#S2�=�֑FF`�PK�Q�`6���,��_�)|~$�Z�����VAb�IT3ϼ��!�A���tN+ZZ�p��;ս�mLOi��kx�.�����֬�5*�'�o1�����Y_�}��B�6}%ڶ�!��{�P3����I��Dq9�c/�J7���Y��B��>?��O�����;|]�vyjq^:ӊ�A`8���ja�P��f�fʳ�Gb�G�$��� �~\�0��M���Fާf�ܸB��s���")V<��d�6��M_7n�p�jo����-pt2���fȩ�2��UǥiTG���H�N�Ϗ�L�X�K��'!�f�f�b*X��n�VF����<mg�X)�,-_VK��uF�Qq�?ml��:�5Z�-?srO�<�� mk�zO���WJ��륻˟�)��GA�:��M�pJ'F ������@b@�!� s��iu	C=�JUXz-%{����;�lQECW*[�J$���������!>[�L�`��%O�I�����
��E�h���������HT���_t�^?u���2�&�;�z��W�(h��7�����#)M�躅B�D%	��`Z�6X��7���<އ;Q�մ�\`Eo�}������m�]��3��r0�u�j�T7<'ތC׵�2��m'���<�<ī���:�l�'�=i8��*�Fy8?8fOp��X�W��UN���2���hT�3+�LȏD�#z�����D�OGZ�|1}��e'jG����]��-7�wm�
w�]|�X�4쮿o�L��n����@Y���	<Κ��zv��(�%W	��@��O����(�ր��RE��$�x�����`o'�y���ܡf���v�C��a�[f��;��7�՘v퐹2�DNx�rs��|)!����mfvBǉ6��ri�p��P&���7OÈ[#m�+R �����Þ� &�T�v���)=\/ݜ���%�)#� #My^uk�3W�#��T�t�<�s�I=O�n��m`����
�,]r�omo�}��ا\����ז!T>>����Qb#���QJ�gS�OH��^=S�� �I�n!?%k�*�;,�}�V/���ha^��\�p�'Lz����V��Q
[qE�b���W,^|����Ţ�O��w&|��@�4N�|-�#�|I�Y�_��/��(�������$MH�2�Έ�~bzr�MF<���7�]��6�L����3%�G��c��b�>�6di����ӳ߮'z���V"D��x��tyR�.ryT#��
��jl��tݓf�,�*�w �&��K�S�"FPݪ��|��s�𭻢7����J	��TV�G�*��&�%��Խدu��ŐJ�����DA澑(�v�%�{n��"q�9��ǫ�
��=4i�%�@��z��6v�>k�8�!�>̠�,7ʉ����G�z���5�!s
n��ԝ�H$ ��S��/�6�ῦ���:Ƿ)LX2p�l�Fc�>*�O��x���ր��n�,E��0�5n��g����I}|��&�D.���8���DpvC$�_�c��]��=�zGc)
]
��%�,��x�`���9$��2��i1(���/z!h�;�ut�j���ʝ�e6!7[,1�V@~�,'�U0���A�Ө�Z���F�5�H6Y��-F����1ѯګ�hM!�H�r�_���l�P�WF]�I\�:,���@�kTN��KCi�6̅oI�"z���:Q<C��VA�^b&�����.��0$�|�!�/�kB)�|�\�Ǟ���οb�`|9O(4������E�t��^�ǪG�,��" _z&��	���K��W�Ṋ�@L{=����4�Z�?�r��з����S�h-#�VZ�6ף�ѷ4�w"�"S�*[=1D-���_QM<|k�I���H<��ۈge8q�>.K�S��$�f��T�%|�JR��q��0��0bb�+'J"��Ɂ{Y?�	z�ڂl��&̽u(��q|���\�2��0R|�w��f�fi-�ZѲxG�b�5۴\Z!�~%�ޓ�:#�Wl,V�թ���kJRe9��cƏR+;�à��H2���F�;,�B�c� ��O
r��d��Pϋ)l��M}��(gc3��ۣx�*�(��nq��08�៘��}�^?�jM��\�S��'��X������ք!� �M(�C����q!�$��a���e�з�!�@c������dN�Mo��f�b�|�?0������lݹ]1o�k���gm���=����H����������.�2�o�p)E�j��R[>Ƿ�S����#�<v��MG"�ZX����=XR��b�jMa����[Ǌx�%�a��5�ŏ}��s��H����~-̀i�Ǻ��+(0&����R�'���� '`�/G�:���;�,J�~��,v!P�늲l��3�rE���)#R.V\���mן�'��)��7��C��KW���H�fPX�.��@0�����SaG��D4��d�Ĺ���ͽs/��[�&���l����cV쇆b�Ē@F&A
��p|�׻C���"$I��������b�!�[��6�N�v���o'�����Q��g��Gq�g�J��ig���3G�.w�ǂ�C��G$2�dvA���"�1�c����0S$����ġ� ,�߷��3����[s;�:ީM��%nǀ�TƗ����yI3	��Wf�t ��'r�Cc��ڼ]���f����7��m����['F �oCӁ2��"o��9�A ��MW��4�.v9�<���+ԒeT���h^Eg�fBHte 8p�D�i �_�@��\��1�H�҂�^[�t�K�֗QH��%��E�"+ؐD��=�d=i����"��S���Ge�i���]'��~��K�7p�bk&��v���{q0��f��K�����S�'zp3�H����(c
�F�,�S���JN���y�`��"��|l��}T�`�V����o%��)��;ܦ��rwZ�@	f{
		if (dl_prio(oldprio))
			p->dl.pi_se = &p->dl;
		if (oldprio < prio)
			queue_flag |= ENQUEUE_HEAD;
	} else {
		if (dl_prio(oldprio))
			p->dl.pi_se = &p->dl;
		if (rt_prio(oldprio))
			p->rt.timeout = 0;
	}

	__setscheduler_prio(p, prio);

	if (queued)
		enqueue_task(rq, p, queue_flag);
	if (running)
		set_next_task(rq, p);

	check_class_changed(rq, p, prev_class, oldprio);
out_unlock:
	/* Avoid rq from going away on us: */
	preempt_disable();

	rq_unpin_lock(rq, &rf);
	__balance_callbacks(rq);
	raw_spin_rq_unlock(rq);

	preempt_enable();
}
#else
static inline int rt_effective_prio(struct task_struct *p, int prio)
{
	return prio;
}
#endif

void set_user_nice(struct task_struct *p, long nice)
{
	bool queued, running;
	int old_prio;
	struct rq_flags rf;
	struct rq *rq;

	if (task_nice(p) == nice || nice < MIN_NICE || nice > MAX_NICE)
		return;
	/*
	 * We have to be careful, if called from sys_setpriority(),
	 * the task might be in the middle of scheduling on another CPU.
	 */
	rq = task_rq_lock(p, &rf);
	update_rq_clock(rq);

	/*
	 * The RT priorities are set via sched_setscheduler(), but we still
	 * allow the 'normal' nice value to be set - but as expected
	 * it won't have any effect on scheduling until the task is
	 * SCHED_DEADLINE, SCHED_FIFO or SCHED_RR:
	 */
	if (task_has_dl_policy(p) || task_has_rt_policy(p)) {
		p->static_prio = NICE_TO_PRIO(nice);
		goto out_unlock;
	}
	queued = task_on_rq_queued(p);
	running = task_current(rq, p);
	if (queued)
		dequeue_task(rq, p, DEQUEUE_SAVE | DEQUEUE_NOCLOCK);
	if (running)
		put_prev_task(rq, p);

	p->static_prio = NICE_TO_PRIO(nice);
	set_load_weight(p, true);
	old_prio = p->prio;
	p->prio = effective_prio(p);

	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE | ENQUEUE_NOCLOCK);
	if (running)
		set_next_task(rq, p);

	/*
	 * If the task increased its priority or is running and
	 * lowered its priority, then reschedule its CPU:
	 */
	p->sched_class->prio_changed(rq, p, old_prio);

out_unlock:
	task_rq_unlock(rq, p, &rf);
}
EXPORT_SYMBOL(set_user_nice);

/*
 * can_nice - check if a task can reduce its nice value
 * @p: task
 * @nice: nice value
 */
int can_nice(const struct task_struct *p, const int nice)
{
	/* Convert nice value [19,-20] to rlimit style value [1,40]: */
	int nice_rlim = nice_to_rlimit(nice);

	return (nice_rlim <= task_rlimit(p, RLIMIT_NICE) ||
		capable(CAP_SYS_NICE));
}

#ifdef __ARCH_WANT_SYS_NICE

/*
 * sys_nice - change the priority of the current process.
 * @increment: priority increment
 *
 * sys_setpriority is a more generic, but much slower function that
 * does similar things.
 */
SYSCALL_DEFINE1(nice, int, increment)
{
	long nice, retval;

	/*
	 * Setpriority might change our priority at the same moment.
	 * We don't have to worry. Conceptually one call occurs first
	 * and we have a single winner.
	 */
	increment = clamp(increment, -NICE_WIDTH, NICE_WIDTH);
	nice = task_nice(current) + increment;

	nice = clamp_val(nice, MIN_NICE, MAX_NICE);
	if (increment < 0 && !can_nice(current, nice))
		return -EPERM;

	retval = security_task_setnice(current, nice);
	if (retval)
		return retval;

	set_user_nice(current, nice);
	return 0;
}

#endif

/**
 * task_prio - return the priority value of a given task.
 * @p: the task in question.
 *
 * Return: The priority value as seen by users in /proc.
 *
 * sched policy         return value   kernel prio    user prio/nice
 *
 * normal, batch, idle     [0 ... 39]  [100 ... 139]          0/[-20 ... 19]
 * fifo, rr             [-2 ... -100]     [98 ... 0]  [1 ... 99]
 * deadline                     -101             -1           0
 */
int task_prio(const struct task_struct *p)
{
	return p->prio - MAX_RT_PRIO;
}

/**
 * idle_cpu - is a given CPU idle currently?
 * @cpu: the processor in question.
 *
 * Return: 1 if the CPU is currently idle. 0 otherwise.
 */
int idle_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (rq->curr != rq->idle)
		return 0;

	if (rq->nr_running)
		return 0;

#ifdef CONFIG_SMP
	if (rq->ttwu_pending)
		return 0;
#endif

	return 1;
}

/**
 * available_idle_cpu - is a given CPU idle for enqueuing work.
 * @cpu: the CPU in question.
 *
 * Return: 1 if the CPU is currently idle. 0 otherwise.
 */
int available_idle_cpu(int cpu)
{
	if (!idle_cpu(cpu))
		return 0;

	if (vcpu_is_preempted(cpu))
		return 0;

	return 1;
}

/**
 * idle_task - return the idle task for a given CPU.
 * @cpu: the processor in question.
 *
 * Return: The idle task for the CPU @cpu.
 */
struct task_struct *idle_task(int cpu)
{
	return cpu_rq(cpu)->idle;
}

#ifdef CONFIG_SMP
/*
 * This function computes an effective utilization for the given CPU, to be
 * used for frequency selection given the linear relation: f = u * f_max.
 *
 * The scheduler tracks the following metrics:
 *
 *   cpu_util_{cfs,rt,dl,irq}()
 *   cpu_bw_dl()
 *
 * Where the cfs,rt and dl util numbers are tracked with the same metric and
 * synchronized windows and are thus directly comparable.
 *
 * The cfs,rt,dl utilization are the running times measured with rq->clock_task
 * which excludes things like IRQ and steal-time. These latter are then accrued
 * in the irq utilization.
 *
 * The DL bandwidth number otoh is not a measured metric but a value computed
 * based on the task model parameters and gives the minimal utilization
 * required to meet deadlines.
 */
unsigned long effective_cpu_util(int cpu, unsigned long util_cfs,
				 unsigned long max, enum cpu_util_type type,
				 struct task_struct *p)
{
	unsigned long dl_util, util, irq;
	struct rq *rq = cpu_rq(cpu);

	if (!uclamp_is_used() &&
	    type == FREQUENCY_UTIL && rt_rq_is_runnable(&rq->rt)) {
		return max;
	}

	/*
	 * Early check to see if IRQ/steal time saturates the CPU, can be
	 * because of inaccuracies in how we track these -- see
	 * update_irq_load_avg().
	 */
	irq = cpu_util_irq(rq);
	if (unlikely(irq >= max))
		return max;

	/*
	 * Because the time spend on RT/DL tasks is visible as 'lost' time to
	 * CFS tasks and we use the same metric to track the effective
	 * utilization (PELT windows are synchronized) we can directly add them
	 * to obtain the CPU's actual utilization.
	 *
	 * CFS and RT utilization can be boosted or capped, depending on
	 * utilization clamp constraints requested by currently RUNNABLE
	 * tasks.
	 * When there are no CFS RUNNABLE tasks, clamps are released and
	 * frequency will be gracefully reduced with the utilization decay.
	 */
	util = util_cfs + cpu_util_rt(rq);
	if (type == FREQUENCY_UTIL)
		util = uclamp_rq_util_with(rq, util, p);

	dl_util = cpu_util_dl(rq);

	/*
	 * For frequency selection we do not make cpu_util_dl() a permanent part
	 * of this sum because we want to use cpu_bw_dl() later on, but we need
	 * to check if the CFS+RT+DL sum is saturated (ie. no idle time) such
	 * that we select f_max when there is no idle time.
	 *
	 * NOTE: numerical errors or stop class might cause us to not quite hit
	 * saturation when we should -- something for later.
	 */
	if (util + dl_util >= max)
		return max;

	/*
	 * OTOH, for energy computation we need the estimated running time, so
	 * include util_dl and ignore dl_bw.
	 */
	if (type == ENERGY_UTIL)
		util += dl_util;

	/*
	 * There is still idle time; further improve the number by using the
	 * irq metric. Because IRQ/steal time is hidden from the task clock we
	 * need to scale the task numbers:
	 *
	 *              max - irq
	 *   U' = irq + --------- * U
	 *                 max
	 */
	util = scale_irq_capacity(util, irq, max);
	util += irq;

	/*
	 * Bandwidth required by DEADLINE must always be granted while, for
	 * FAIR and RT, we use blocked utilization of IDLE CPUs as a mechanism
	 * to gracefully reduce the frequency when no tasks show up for longer
	 * periods of time.
	 *
	 * Ideally we would like to set bw_dl as min/guaranteed freq and util +
	 * bw_dl as requested freq. However, cpufreq is not yet ready for such
	 * an interface. So, we only do the latter for now.
	 */
	if (type == FREQUENCY_UTIL)
		util += cpu_bw_dl(rq);

	return min(max, util);
}

unsigned long sched_cpu_util(int cpu, unsigned long max)
{
	return effective_cpu_util(cpu, cpu_util_cfs(cpu), max,
				  ENERGY_UTIL, NULL);
}
#endif /* CONFIG_SMP */

/**
 * find_process_by_pid - find a process with a matching PID value.
 * @pid: the pid in question.
 *
 * The task of @pid, if found. %NULL otherwise.
 */
static struct task_struct *find_process_by_pid(pid_t pid)
{
	return pid ? find_task_by_vpid(pid) : current;
}

/*
 * sched_setparam() passes in -1 for its policy, to let the functions
 * it calls know not to change it.
 */
#define SETPARAM_POLICY	-1

static void __setscheduler_params(struct task_struct *p,
		const struct sched_attr *attr)
{
	int policy = attr->sched_policy;

	if (policy == SETPARAM_POLICY)
		policy = p->policy;

	p->policy = policy;

	if (dl_policy(policy))
		__setparam_dl(p, attr);
	else if (fair_policy(policy))
		p->static_prio = NICE_TO_PRIO(attr->sched_nice);

	/*
	 * __sched_setscheduler() ensures attr->sched_priority == 0 when
	 * !rt_policy. Always setting this ensures that things like
	 * getparam()/getattr() don't report silly values for !rt tasks.
	 */
	p->rt_priority = attr->sched_priority;
	p->normal_prio = normal_prio(p);
	set_load_weight(p, true);
}

/*
 * Check the target process has a UID that matches the current process's:
 */
static bool check_same_owner(struct task_struct *p)
{
	const struct cred *cred = current_cred(), *pcred;
	bool match;

	rcu_read_lock();
	pcred = __task_cred(p);
	match = (uid_eq(cred->euid, pcred->euid) ||
		 uid_eq(cred->euid, pcred->uid));
	rcu_read_unlock();
	return match;
}

static int __sched_setscheduler(struct task_struct *p,
				const struct sched_attr *attr,
				bool user, bool pi)
{
	int oldpolicy = -1, policy = attr->sched_policy;
	int retval, oldprio, newprio, queued, running;
	const struct sched_class *prev_class;
	struct callback_head *head;
	struct rq_flags rf;
	int reset_on_fork;
	int queue_flags = DEQUEUE_SAVE | DEQUEUE_MOVE | DEQUEUE_NOCLOCK;
	struct rq *rq;

	/* The pi code expects interrupts enabled */
	BUG_ON(pi && in_interrupt());
recheck:
	/* Double check policy once rq lock held: */
	if (policy < 0) {
		reset_on_fork = p->sched_reset_on_fork;
		policy = oldpolicy = p->policy;
	} else {
		reset_on_fork = !!(attr->sched_flags & SCHED_FLAG_RESET_ON_FORK);

		if (!valid_policy(policy))
			return -EINVAL;
	}

	if (attr->sched_flags & ~(SCHED_FLAG_ALL | SCHED_FLAG_SUGOV))
		return -EINVAL;

	/*
	 * Valid priorities for SCHED_FIFO and SCHED_RR are
	 * 1..MAX_RT_PRIO-1, valid priority for SCHED_NORMAL,
	 * SCHED_BATCH and SCHED_IDLE is 0.
	 */
	if (attr->sched_priority > MAX_RT_PRIO-1)
		return -EINVAL;
	if ((dl_policy(policy) && !__checkparam_dl(attr)) ||
	    (rt_policy(policy) != (attr->sched_priority != 0)))
		return -EINVAL;

	/*
	 * Allow unprivileged RT tasks to decrease priority:
	 */
	if (user && !capable(CAP_SYS_NICE)) {
		if (fair_policy(policy)) {
			if (attr->sched_nice < task_nice(p) &&
			    !can_nice(p, attr->sched_nice))
				return -EPERM;
		}

		if (rt_policy(policy)) {
			unsigned long rlim_rtprio =
					task_rlimit(p, RLIMIT_RTPRIO);

			/* Can't set/change the rt policy: */
			if (policy != p->policy && !rlim_rtprio)
				return -EPERM;

			/* Can't increase priority: */
			if (attr->sched_priority > p->rt_priority &&
			    attr->sched_priority > rlim_rtprio)
				return -EPERM;
		}

		 /*
		  * Can't set/change SCHED_DEADLINE policy at all for now
		  * (safest behavior); in the future we would like to allow
		  * unprivileged DL tasks to increase their relative deadline
		  * or reduce their runtime (both ways reducing utilization)
		  */
		if (dl_policy(policy))
			return -EPERM;

		/*
		 * Treat SCHED_IDLE as nice 20. Only allow a switch to
		 * SCHED_NORMAL if the RLIMIT_NICE would normally permit it.
		 */
		if (task_has_idle_policy(p) && !idle_policy(policy)) {
			if (!can_nice(p, task_nice(p)))
				return -EPERM;
		}

		/* Can't change other user's priorities: */
		if (!check_same_owner(p))
			return -EPERM;

		/* Normal users shall not reset the sched_reset_on_fork flag: */
		if (p->sched_reset_on_fork && !reset_on_fork)
			return -EPERM;
	}

	if (user) {
		if (attr->sched_flags & SCHED_FLAG_SUGOV)
			return -EINVAL;

		retval = security_task_setscheduler(p);
		if (retval)
			return retval;
	}

	/* Update task specific "requested" clamps */
	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP) {
		retval = uclamp_validate(p, attr);
		if (retval)
			return retval;
	}

	if (pi)
		cpuset_read_lock();

	/*
	 * Make sure no PI-waiters arrive (or leave) while we are
	 * changing the priority of the task:
	 *
	 * To be able to change p->policy safely, the appropriate
	 * runqueue lock must be held.
	 */
	rq = task_rq_lock(p, &rf);
	update_rq_clock(rq);

	/*
	 * Changing the policy of the stop threads its a very bad idea:
	 */
	if (p == rq->stop) {
		retval = -EINVAL;
		goto unlock;
	}

	/*
	 * If not changing anything there's no need to proceed further,
	 * but store a possible modification of reset_on_fork.
	 */
	if (unlikely(policy == p->policy)) {
		if (fair_policy(policy) && attr->sched_nice != task_nice(p))
			goto change;
		if (rt_policy(policy) && attr->sched_priority != p->rt_priority)
			goto change;
		if (dl_policy(policy) && dl_param_changed(p, attr))
			goto change;
		if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP)
			goto change;

		p->sched_reset_on_fork = reset_on_fork;
		retval = 0;
		goto unlock;
	}
change:

	if (user) {
#ifdef CONFIG_RT_GROUP_SCHED
		/*
		 * Do not allow realtime tasks into groups that have no runtime
		 * assigned.
		 */
		if (rt_bandwidth_enabled() && rt_policy(policy) &&
				task_group(p)->rt_bandwidth.rt_runtime == 0 &&
				!task_group_is_autogroup(task_group(p))) {
			retval = -EPERM;
			goto unlock;
		}
#endif
#ifdef CONFIG_SMP
		if (dl_bandwidth_enabled() && dl_policy(policy) &&
				!(attr->sched_flags & SCHED_FLAG_SUGOV)) {
			cpumask_t *span = rq->rd->span;

			/*
			 * Don't allow tasks with an affinity mask smaller than
			 * the entire root_domain to become SCHED_DEADLINE. We
			 * will also fail if there's no bandwidth available.
			 */
			if (!cpumask_subset(span, p->cpus_ptr) ||
			    rq->rd->dl_bw.bw == 0) {
				retval = -EPERM;
				goto unlock;
			}
		}
#endif
	}

	/* Re-check policy now with rq lock held: */
	if (unlikely(oldpolicy != -1 && oldpolicy != p->policy)) {
		policy = oldpolicy = -1;
		task_rq_unlock(rq, p, &rf);
		if (pi)
			cpuset_read_unlock();
		goto recheck;
	}

	/*
	 * If setscheduling to SCHED_DEADLINE (or changing the parameters
	 * of a SCHED_DEADLINE task) we need to check if enough bandwidth
	 * is available.
	 */
	if ((dl_policy(policy) || dl_task(p)) && sched_dl_overflow(p, policy, attr)) {
		retval = -EBUSY;
		goto unlock;
	}

	p->sched_reset_on_fork = reset_on_fork;
	oldprio = p->prio;

	newprio = __normal_prio(policy, attr->sched_priority, attr->sched_nice);
	if (pi) {
		/*
		 * Take priority boosted tasks into account. If the new
		 * effective priority is unchanged, we just store the new
		 * normal parameters and do not touch the scheduler class and
		 * the runqueue. This will be done when the task deboost
		 * itself.
		 */
		newprio = rt_effective_prio(p, newprio);
		if (newprio == oldprio)
			queue_flags &= ~DEQUEUE_MOVE;
	}

	queued = task_on_rq_queued(p);
	running = task_current(rq, p);
	if (queued)
		dequeue_task(rq, p, queue_flags);
	if (running)
		put_prev_task(rq, p);

	prev_class = p->sched_class;

	if (!(attr->sched_flags & SCHED_FLAG_KEEP_PARAMS)) {
		__setscheduler_params(p, attr);
		__setscheduler_prio(p, newprio);
	}
	__setscheduler_uclamp(p, attr);

	if (queued) {
		/*
		 * We enqueue to tail when the priority of a task is
		 * increased (user space view).
		 */
		if (oldprio < p->prio)
			queue_flags |= ENQUEUE_HEAD;

		enqueue_task(rq, p, queue_flags);
	}
	if (running)
		set_next_task(rq, p);

	check_class_changed(rq, p, prev_class, oldprio);

	/* Avoid rq from going away on us: */
	preempt_disable();
	head = splice_balance_callbacks(rq);
	task_rq_unlock(rq, p, &rf);

	if (pi) {
		cpuset_read_unlock();
		rt_mutex_adjust_pi(p);
	}

	/* Run balance callbacks after we've adjusted the PI chain: */
	balance_callbacks(rq, head);
	preempt_enable();

	return 0;

unlock:
	task_rq_unlock(rq, p, &rf);
	if (pi)
		cpuset_read_unlock();
	return retval;
}

static int _sched_setscheduler(struct task_struct *p, int policy,
			       const struct sched_param *param, bool check)
{
	struct sched_attr attr = {
		.sched_policy   = policy,
		.sched_priority = param->sched_priority,
		.sched_nice	= PRIO_TO_NICE(p->static_prio),
	};

	/* Fixup the legacy SCHED_RESET_ON_FORK hack. */
	if ((policy != SETPARAM_POLICY) && (policy & SCHED_RESET_ON_FORK)) {
		attr.sched_flags |= SCHED_FLAG_RESET_ON_FORK;
		policy &= ~SCHED_RESET_ON_FORK;
		attr.sched_policy = policy;
	}

	return __sched_setscheduler(p, &attr, check, true);
}
/**
 * sched_setscheduler - change the scheduling policy and/or RT priority of a thread.
 * @p: the task in question.
 * @policy: new policy.
 * @param: structure containing the new RT priority.
 *
 * Use sched_set_fifo(), read its comment.
 *
 * Return: 0 on success. An error code otherwise.
 *
 * NOTE that the task may be already dead.
 */
int sched_setscheduler(struct task_struct *p, int policy,
		       const struct sched_param *param)
{
	return _sched_setscheduler(p, policy, param, true);
}

int sched_setattr(struct task_struct *p, const struct sched_attr *attr)
{
	return __sched_setscheduler(p, attr, true, true);
}

int sched_setattr_nocheck(struct task_struct *p, const struct sched_attr *attr)
{
	return __sched_setscheduler(p, attr, false, true);
}
EXPORT_SYMBOL_GPL(sched_setattr_nocheck);

/**
 * sched_setscheduler_nocheck - change the scheduling policy and/or RT priority of a thread from kernelspace.
 * @p: the task in question.
 * @policy: new policy.
 * @param: structure containing the new RT priority.
 *
 * Just like sched_setscheduler, only don't bother checking if the
 * current context has permission.  For example, this is needed in
 * stop_machine(): we create temporary high priority worker threads,
 * but our caller might not have that capability.
 *
 * Return: 0 on success. An error code otherwise.
 */
int sched_setscheduler_nocheck(struct task_struct *p, int policy,
			       const struct sched_param *param)
{
	return _sched_setscheduler(p, policy, param, false);
}

/*
 * SCHED_FIFO is a broken scheduler model; that is, it is fundamentally
 * incapable of resource management, which is the one thing an OS really should
 * be doing.
 *
 * This is of course the reason it is limited to privileged users only.
 *
 * Worse still; it is fundamentally impossible to compose static priority
 * workloads. You cannot take two correctly working static prio workloads
 * and smash them together and still expect them to work.
 *
 * For this reason 'all' FIFO tasks the kernel creates are basically at:
 *
 *   MAX_RT_PRIO / 2
 *
 * The administrator _MUST_ configure the system, the kernel simply doesn't
 * know enough information to make a sensible choice.
 */
void sched_set_fifo(struct task_struct *p)
{
	struct sched_param sp = { .sched_priority = MAX_RT_PRIO / 2 };
	WARN_ON_ONCE(sched_setscheduler_nocheck(p, SCHED_FIFO, &sp) != 0);
}
EXPORT_SYMBOL_GPL(sched_set_fifo);

/*
 * For when you don't much care about FIFO, but want to be above SCHED_NORMAL.
 */
void sched_set_fifo_low(struct task_struct *p)
{
	struct sched_param sp = { .sched_priority = 1 };
	WARN_ON_ONCE(sched_setscheduler_nocheck(p, SCHED_FIFO, &sp) != 0);
}
EXPORT_SYMBOL_GPL(sched_set_fifo_low);

void sched_set_normal(struct task_struct *p, int nice)
{
	struct sched_attr attr = {
		.sched_policy = SCHED_NORMAL,
		.sched_nice = nice,
	};
	WARN_ON_ONCE(sched_setattr_nocheck(p, &attr) != 0);
}
EXPORT_SYMBOL_GPL(sched_set_normal);

static int
do_sched_setscheduler(pid_t pid, int policy, struct sched_param __user *param)
{
	struct sched_param lparam;
	struct task_struct *p;
	int retval;

	if (!param || pid < 0)
		return -EINVAL;
	if (copy_from_user(&lparam, param, sizeof(struct sched_param)))
		return -EFAULT;

	rcu_read_lock();
	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (likely(p))
		get_task_struct(p);
	rcu_read_unlock();

	if (likely(p)) {
		retval = sched_setscheduler(p, policy, &lparam);
		put_task_struct(p);
	}

	return retval;
}

/*
 * Mimics kernel/events/core.c perf_copy_attr().
 */
static int sched_copy_attr(struct sched_attr __user *uattr, struct sched_attr *attr)
{
	u32 size;
	int ret;

	/* Zero the full structure, so that a short copy will be nice: */
	memset(attr, 0, sizeof(*attr));

	ret = get_user(size, &uattr->size);
	if (ret)
		return ret;

	/* ABI compatibility quirk: */
	if (!size)
		size = SCHED_ATTR_SIZE_VER0;
	if (size < SCHED_ATTR_SIZE_VER0 || size > PAGE_SIZE)
		goto err_size;

	ret = copy_struct_from_user(attr, sizeof(*attr), uattr, size);
	if (ret) {
		if (ret == -E2BIG)
			goto err_size;
		return ret;
	}

	if ((attr->sched_flags & SCHED_FLAG_UTIL_CLAMP) &&
	    size < SCHED_ATTR_SIZE_VER1)
		return -EINVAL;

	/*
	 * XXX: Do we want to be lenient like existing syscalls; or do we want
	 * to be strict and return an error on out-of-bounds values?
	 */
	attr->sched_nice = clamp(attr->sched_nice, MIN_NICE, MAX_NICE);

	return 0;

err_size:
	put_user(sizeof(*attr), &uattr->size);
	return -E2BIG;
}

static void get_params(struct task_struct *p, struct sched_attr *attr)
{
	if (task_has_dl_policy(p))
		__getparam_dl(p, attr);
	else if (task_has_rt_policy(p))
		attr->sched_priority = p->rt_priority;
	else
		attr->sched_nice = task_nice(p);
}

/**
 * sys_sched_setscheduler - set/change the scheduler policy and RT priority
 * @pid: the pid in question.
 * @policy: new policy.
 * @param: structure containing the new RT priority.
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE3(sched_setscheduler, pid_t, pid, int, policy, struct sched_param __user *, param)
{
	if (policy < 0)
		return -EINVAL;

	return do_sched_setscheduler(pid, policy, param);
}

/**
 * sys_sched_setparam - set/change the RT priority of a thread
 * @pid: the pid in question.
 * @param: structure containing the new RT priority.
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE2(sched_setparam, pid_t, pid, struct sched_param __user *, param)
{
	return do_sched_setscheduler(pid, SETPARAM_POLICY, param);
}

/**
 * sys_sched_setattr - same as above, but with extended sched_attr
 * @pid: the pid in question.
 * @uattr: structure containing the extended parameters.
 * @flags: for future extension.
 */
SYSCALL_DEFINE3(sched_setattr, pid_t, pid, struct sched_attr __user *, uattr,
			       unsigned int, flags)
{
	struct sched_attr attr;
	struct task_struct *p;
	int retval;

	if (!uattr || pid < 0 || flags)
		return -EINVAL;

	retval = sched_copy_attr(uattr, &attr);
	if (retval)
		return retval;

	if ((int)attr.sched_policy < 0)
		return -EINVAL;
	if (attr.sched_flags & SCHED_FLAG_KEEP_POLICY)
		attr.sched_policy = SETPARAM_POLICY;

	rcu_read_lock();
	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (likely(p))
		get_task_struct(p);
	rcu_read_unlock();

	if (likely(p)) {
		if (attr.sched_flags & SCHED_FLAG_KEEP_PARAMS)
			get_params(p, &attr);
		retval = sched_setattr(p, &attr);
		put_task_struct(p);
	}

	return retval;
}

/**
 * sys_sched_getscheduler - get the policy (scheduling class) of a thread
 * @pid: the pid in question.
 *
 * Return: On success, the policy of the thread. Otherwise, a negative error
 * code.
 */
SYSCALL_DEFINE1(sched_getscheduler, pid_t, pid)
{
	struct task_struct *p;
	int retval;

	if (pid < 0)
		return -EINVAL;

	retval = -ESRCH;
	rcu_read_lock();
	p = find_process_by_pid(pid);
	if (p) {
		retval = security_task_getscheduler(p);
		if (!retval)
			retval = p->policy
				| (p->sched_reset_on_fork ? SCHED_RESET_ON_FORK : 0);
	}
	rcu_read_unlock();
	return retval;
}

/**
 * sys_sched_getparam - get the RT priority of a thread
 * @pid: the pid in question.
 * @param: structure containing the RT priority.
 *
 * Return: On success, 0 and the RT priority is in @param. Otherwise, an error
 * code.
 */
SYSCALL_DEFINE2(sched_getparam, pid_t, pid, struct sched_param __user *, param)
{
	struct sched_param lp = { .sched_priority = 0 };
	struct task_struct *p;
	int retval;

	if (!param || pid < 0)
		return -EINVAL;

	rcu_read_lock();
	p = find_process_by_pid(pid);
	retval = -ESRCH;
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	if (task_has_rt_policy(p))
		lp.sched_priority = p->rt_priority;
	rcu_read_unlock();

	/*
	 * This one might sleep, we cannot do it with a spinlock held ...
	 */
	retval = copy_to_user(param, &lp, sizeof(*param)) ? -EFAULT : 0;

	return retval;

out_unlock:
	rcu_read_unlock();
	return retval;
}

/*
 * Copy the kernel size attribute structure (which might be larger
 * than what user-space knows about) to user-space.
 *
 * Note that all cases are valid: user-space buffer can be larger or
 * smaller than the kernel-space buffer. The usual case is that both
 * have the same size.
 */
static int
sched_attr_copy_to_user(struct sched_attr __user *uattr,
			struct sched_attr *kattr,
			unsigned int usize)
{
	unsigned int ksize = sizeof(*kattr);

	if (!access_ok(uattr, usize))
		return -EFAULT;

	/*
	 * sched_getattr() ABI forwards and backwards compatibility:
	 *
	 * If usize == ksize then we just copy everything to user-space and all is good.
	 *
	 * If usize < ksize then we only copy as much as user-space has space for,
	 * this keeps ABI compatibility as well. We skip the rest.
	 *
	 * If usize > ksize then user-space is using a newer version of the ABI,
	 * which part the kernel doesn't know about. Just ignore it - tooling can
	 * detect the kernel's knowledge of attributes from the attr->size value
	 * which is set to ksize in this case.
	 */
	kattr->size = min(usize, ksize);

	if (copy_to_user(uattr, kattr, kattr->size))
		return -EFAULT;

	return 0;
}

/**
 * sys_sched_getattr - similar to sched_getparam, but with sched_attr
 * @pid: the pid in question.
 * @uattr: structure containing the extended parameters.
 * @usize: sizeof(attr) for fwd/bwd comp.
 * @flags: for future extension.
 */
SYSCALL_DEFINE4(sched_getattr, pid_t, pid, struct sched_attr __user *, uattr,
		unsigned int, usize, unsigned int, flags)
{
	struct sched_attr kattr = { };
	struct task_struct *p;
	int retval;

	if (!uattr || pid < 0 || usize > PAGE_SIZE ||
	    usize < SCHED_ATTR_SIZE_VER0 || flags)
		return -EINVAL;

	rcu_read_lock();
	p = find_process_by_pid(pid);
	retval = -ESRCH;
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	kattr.sched_policy = p->policy;
	if (p->sched_reset_on_fork)
		kattr.sched_flags |= SCHED_FLAG_RESET_ON_FORK;
	get_params(p, &kattr);
	kattr.sched_flags &= SCHED_FLAG_ALL;

#ifdef CONFIG_UCLAMP_TASK
	/*
	 * This could race with another potential updater, but this is fine
	 * because it'll correctly read the old or the new value. We don't need
	 * to guarantee who wins the race as long as it doesn't return garbage.
	 */
	kattr.sched_util_min = p->uclamp_req[UCLAMP_MIN].value;
	kattr.sched_util_max = p->uclamp_req[UCLAMP_MAX].value;
#endif

	rcu_read_unlock();

	return sched_attr_copy_to_user(uattr, &kattr, usize);

out_unlock:
	rcu_read_unlock();
	return retval;
}

#ifdef CONFIG_SMP
int dl_task_check_affinity(struct task_struct *p, const struct cpumask *mask)
{
	int ret = 0;

	/*
	 * If the task isn't a deadline task or admission control is
	 * disabled then we don't care about affinity changes.
	 */
	if (!task_has_dl_policy(p) || !dl_bandwidth_enabled())
		return 0;

	/*
	 * Since bandwidth control happens on root_domain basis,
	 * if admission test is enabled, we only admit -deadline
	 * tasks allowed to run on all the CPUs in the task's
	 * root_domain.
	 */
	rcu_read_lock();
	if (!cpumask_subset(task_rq(p)->rd->span, mask))
		ret = -EBUSY;
	rcu_read_unlock();
	return ret;
}
#endif

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask)
{
	int retval;
	cpumask_var_t cpus_allowed, new_mask;

	if (!alloc_cpumask_var(&cpus_allowed, GFP_KERNEL))
		return -ENOMEM;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL)) {
		retval = -ENOMEM;
		goto out_free_cpus_allowed;
	}

	cpuset_cpus_allowed(p, cpus_allowed);
	cpumask_and(new_mask, mask, cpus_allowed);

	retval = dl_task_check_affinity(p, new_mask);
	if (retval)
		goto out_free_new_mask;
again:
	retval = __set_cpus_allowed_ptr(p, new_mask, SCA_CHECK | SCA_USER);
	if (retval)
		goto out_free_new_mask;

	cpuset_cpus_allowed(p, cpus_allowed);
	if (!cpumask_subset(new_mask, cpus_allowed)) {
		/*
		 * We must have raced with a concurrent cpuset update.
		 * Just reset the cpumask to the cpuset's cpus_allowed.
		 */
		cpumask_copy(new_mask, cpus_allowed);
		goto again;
	}

out_free_new_mask:
	free_cpumask_var(new_mask);
out_free_cpus_allowed:
	free_cpumask_var(cpus_allowed);
	return retval;
}

long sched_setaffinity(pid_t pid, const struct cpumask *in_mask)
{
	struct task_struct *p;
	int retval;

	rcu_read_lock();

	p = find_process_by_pid(pid);
	if (!p) {
		rcu_read_unlock();
		return -ESRCH;
	}

	/* Prevent p going away */
	get_task_struct(p);
	rcu_read_unlock();

	if (p->flags & PF_NO_SETAFFINITY) {
		retval = -EINVAL;
		goto out_put_task;
	}

	if (!check_same_owner(p)) {
		rcu_read_lock();
		if (!ns_capable(__task_cred(p)->user_ns, CAP_SYS_NICE)) {
			rcu_read_unlock();
			retval = -EPERM;
			goto out_put_task;
		}
		rcu_read_unlock();
	}

	retval = security_task_setscheduler(p);
	if (retval)
		goto out_put_task;

	retval = __sched_setaffinity(p, in_mask);
out_put_task:
	put_task_struct(p);
	return retval;
}

static int get_user_cpu_mask(unsigned long __user *user_mask_ptr, unsigned len,
			     struct cpumask *new_mask)
{
	if (len < cpumask_size())
		cpumask_clear(new_mask);
	else if (len > cpumask_size())
		len = cpumask_size();

	return copy_from_user(new_mask, user_mask_ptr, len) ? -EFAULT : 0;
}

/**
 * sys_sched_setaffinity - set the CPU affinity of a process
 * @pid: pid of the process
 * @len: length in bytes of the bitmask pointed to by user_mask_ptr
 * @user_mask_ptr: user-space pointer to the new CPU mask
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE3(sched_setaffinity, pid_t, pid, unsigned int, len,
		unsigned long __user *, user_mask_ptr)
{
	cpumask_var_t new_mask;
	int retval;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL))
		return -ENOMEM;

	retval = get_user_cpu_mask(user_mask_ptr, len, new_mask);
	if (retval == 0)
		retval = sched_setaffinity(pid, new_mask);
	free_cpumask_var(new_mask);
	return retval;
}

long sched_getaffinity(pid_t pid, struct cpumask *mask)
{
	struct task_struct *p;
	unsigned long flags;
	int retval;

	rcu_read_lock();

	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	cpumask_and(mask, &p->cpus_mask, cpu_active_mask);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

out_unlock:
	rcu_read_unlock();

	return retval;
}

/**
 * sys_sched_getaffinity - get the CPU affinity of a process
 * @pid: pid of the process
 * @len: length in bytes of the bitmask pointed to by user_mask_ptr
 * @user_mask_ptr: user-space pointer to hold the current CPU mask
 *
 * Return: size of CPU mask copied to user_mask_ptr on success. An
 * error code otherwise.
 */
SYSCALL_DEFINE3(sched_getaffinity, pid_t, pid, unsigned int, len,
		unsigned long __user *, user_mask_ptr)
{
	int ret;
	cpumask_var_t mask;

	if ((len * BITS_PER_BYTE) < nr_cpu_ids)
		return -EINVAL;
	if (len & (sizeof(unsigned long)-1))
		return -EINVAL;

	if (!alloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	ret = sched_getaffinity(pid, mask);
	if (ret == 0) {
		unsigned int retlen = min(len, cpumask_size());

		if (copy_to_user(user_mask_ptr, mask, retlen))
			ret = -EFAULT;
		else
			ret = retlen;
	}
	free_cpumask_var(mask);

	return ret;
}

static void do_sched_yield(void)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = this_rq_lock_irq(&rf);

	schedstat_inc(rq->yld_count);
	current->sched_class->yield_task(rq);

	preempt_disable();
	rq_unlock_irq(rq, &rf);
	sched_preempt_enable_no_resched();

	schedule();
}

/**
 * sys_sched_yield - yield the current processor to other threads.
 *
 * This function yields the current CPU to other tasks. If there are no
 * other threads running on this CPU then this function will return.
 *
 * Return: 0.
 */
SYSCALL_DEFINE0(sched_yield)
{
	do_sched_yield();
	return 0;
}

#if !defined(CONFIG_PREEMPTION) || defined(CONFIG_PREEMPT_DYNAMIC)
int __sched __cond_resched(void)
{
	iU�J:�2��B����Q��� �i���-���s7X�tMg^��06q�Ei����׳V�I*"I�����`���[U����;`�������ĉ�o�X�E�6K@����#��ײ��3=���Q<�����G%c���ީ�͑��p�YǋY���br���V���#�����M}H6���<k Bת@`�676�+����3��c!S�;��^����&���:e6��=�BW��%*w_qv *�0H	�*n��څ3����� p�yI���f�2L��U^�����
��b����=��@=����cO���(d|��+I��@��)	�6�Ϳ�8d��&��?H���Hʿ��@�FT�d�P������pW��_���j��b���� a��0/tFũX�K���]b�fȎpW�~_qH=�H��D0�#�	��m���~��S閽�%V�!?,���g��cl���֐�w�����J�w�����lǍ��f
�Ab8TڻE4G�	&}G@X����n�=�a���]	W6X���l_�&����F�\�	�ކ���C����ْZ�]i:k�[$;RAnB@���s(��>��=?��4� ү`�ٗ�|Y�u�����ĕAr���ɮ��&΁
�AZ���&��k�H:��޳�4�}n�/�Q�χ�J��Ի�Վ3!`S�]�E�H7�a�|V_~J����am��E���p��')��{C����g�gu)�]?�ճj3&iV��D����T"ɭ�n
�Йh�򨉅��b�.���+�Y '[j��疆�����-h\@0f:��j9�9��}�?�V9� �kb� -��8���a"�k���+�c�_2�k��+�IDщ����}�Ĭ���cF�bk��L�>eA�)`����RB:[�cH��$̼:����J٢��w��듲���&�1c����f�t��pD�e��:D����sb��ն�u�d�0����q��>��k�|?笰\gE���V��������m���p*
h��A��	��r�ļ��'�o���}��3�By�j�b�[��BV�n����7��}Y�eX�E�c�?���`ۦ=M �� �d��ۂ��t������D���r�H�-Y}!~�D�z,W�q]��hE�<P����q��1�r@�)2S�Ik@�UC��OP]��E�0��{�:LA����R�Q�F��6H��j��>r������6=�ްvCN2���z�c<l⓼R$w{&^jIi-����/��JL�'\oz��d��+�|J|.S=�&/_[��*v��Ўg��%W��+�%�_�;���A�2Sг�������X�O�qɀ�*�g�{��Z�=�S���:Nz�,^�hE�v#�Qz�I�!"�j�Е�[���j��s&��`T]"�/LB�H����*�O&�ئw��NNo��蚠��vn	��&��r�>P����&s4*m�5�(�JD$��{�V���1���2�q� ��ە��Yi�*gu.c�w|I���d̓�T����؂���3*�)~�L!G?)�U �t��qU��Zz)����?+�Of����4]	S�DJH�$ �s g��6Y����
�)ٍKC0=}��(�XA$o˔�~T�yh�OK)~�%i����g��〧���pX��,@�g}Eȏ?�ʡ�m�a����j-t@d�U�SsӖ	���c�ة,G��T������	>7�g���6�>Y�^�a�-F�'6L22
)D@������6�g��|�Q3H8 Q��'qzi�k�؋$(�����~��^F��g���B�C�ܖ*�L#�rߓ>�5}���ޑO�3���<>�h������i� ꉊg�z7an�Kӧp����Q� Se�v��1x,�ҿ�������x$�P9h���X-4��߮����>s ��^]8�$�c۩$��m��Y��MH���M!�����\��	Ͻ՝#��(��?ְ��0�V|�tA?��щ�4�B3���%�Vv�ըnt�����oVݵw�A�Tcb��rJ^-iP��&C/O������E��c�jft��G˓@}�7Z�8d��/�zFU�j!{�s�c��n��?+����0�*�mр�2�ifQ&nx[��X�!ƅ�Wwt?��m�8�xb�L��U0����%�c���f��i��E���V,��xG��_��9��<Qx��'���ю����Mv�� ���kJ}uC�J�`���2�zߒ���)b9�;���!�3/��t�m�t9B)k�-���ʂ� UO푼+�5�4(.��� X˃aޮ�O���:SIbR
����7��?�Ev��#�k'b���ǭ�L
���c�Xg�\1���i8`Y��6��q�u�l,tݴR5����?E�q4�"u��)U��r�*dǧ�֠ւm�Hw�b�u�����$�X�]c*��	T���Ub�PO
yOY�wp�)4S%�g!���q�7�ݾ^;0w9F��e\7!#��2��V)�Q�_�b���y�+�6���>��IC)�y�L�(t�w��������,q��L*�R5��p�q��vn�+��i^z���Y����d��Gթd��i/�8����,���r�RMh`�;�xs�X�b���Wfa޾����������+�Fm%$�T�$?޲�m�Z2�>��O�)�����s��/6�}'�>4���^[���t����T���+��zl�* �����x[��4�.���)�|z�Ð���Ғ�������z�y]l�pa�p�1��
�:J��$=��H
��J�A�m\�����dm�i���ӹ0��q1�D?�'_g	K�"�[0���xD�L\!p�wb-��Е�0���x��`�]b����j�՝�s���ԝ9��Js�eԩS�O�C�iD��������f�C�c�3�f(%4�d%�S.v�ϊh?�������,D��s����!1nwjf�JH��:L�K�Ocy�?��,�62]��}������t��P�1����Y{�� �ޞ� �W$��9|����ni�l؊��Ng L�e4a�H4���?닄ȷk;O�{�,�#��!NP�`J!��}�J�?u�	r���a��
��=��;�f�1[��m9�4 %�%;�*D�m;��r�D���YS'�q�(|�ƺ�Ȳ8��Z��pj9O��5����G�'�i�o|	���|}�D�w ���v��Uu]�]��	��x�oZ�f&%��s��᧔��Ҫ�"T�ni��s�����4�tVձ~PP�V�o6��M��]��4�
��
���Tby�b;tP-����,���@s�w�![y�w�_j��U{T��
Q���r΄)�i֥��(�ff�BUn^@�c��;�����M.m	8_�y�����àS|��Y9��R�:�T/�s�QbE��8���A�SzN��H���ck��::����/�/�s��}Bc�d���q�V�(�跅�R�������+��'��v,�r���# �����W
X�E&J�We�q���i	�7F��u%�po�-�z ix痍F��$W����Ä�A�0-��<q2�M5s��S�ڪ�Qx׉%}Ƃ��y���y)����m�B�D8�6Sd�;Y1 �:��	�	H��%՛A��dD���lP]�jގ���t���ӴTtO@r�96C�!ؘݎiU�Y�4�f�n��w"G&�5���t8�n:�!'�
}G#�1�@�77�4Y
5�ΛhK��a7�w�5,���$A���ѯȩs�Ȣ��v~!�9u����(������[ʤʤ�oJ4t�Hu��Dߌ"��L֣�S���SHJ���;'?��UϘ�Q�Um�ؾ O���y|�E��OxYp�9�Ē^K�ۄ����_����ߔ���&��Z��-�1���>�j1�MȨ̷��;0}���V��aR�&+�F�3��U�����]�L�	j ��q�׶� ��_������E���ʧ �-�`�ŗ�@��3l�^�x��tV��Z&�j?�-(��w�_��R���y��$�+��Շ<��X�9�	c �|$�C� ���o�:02i<��~��a���`62X���=�U�{!�������|�c��N�'�=d�9�`^a�8&�y�x�ᵲ�E%ˁ�����2�Gz��V�ߞKEy�Q��b� 8N��K�C�NƜ[�9��T���6m͖�r��8�os\�g�]�&'���׏q	ˢ���>�K�aV�ziKx���w�_-�����fv��;V4}u�c���.���s����b��:������|شC3j��sڒÓ=>��o&��@�J��US���	JN�$�t�3LO�l*�<`�;n���ߡ>͡�A>x{р8+k��5`\��X�T�ڻ��s��@}^1�j����Qw$�f*9��z�������^?��K�r��/콘�q�����cn�Y�7�J��M���V�Y���M��{�E�z+�(�*���fê��^<�z፞L���� ����ޔ+s 46��@�0D�}����KB �/l�8\N�W�%��� �H7�1��CՀM�F��3��!�����(�A����Er� �+n8���򊒥ܝ���h�8G��$>j9M)ٝE���z][Q=��xôz��=.�/���O(�4�ɶ4�pW�+�	`����;�w�f�J�B/�[~���3	��!��2�-���r 2�{�	)d�c�d4�]���,b����P�p�,���gQo����u_��;�څ��9v�F����u�p ]�4/���C���ύE�Qw�h��	%Qn�M���d�ڨ=�zHH&F�عc� ?xf�"_:$_�~�#u�k����9��F����|����X�?YWVM�Z��!��?�q~��y),L�a����N�Q&���/�N%��=�����Z4��S��wt{����Q�f3������`hR#�2��ɚ}L�H�m`�W��$�N����ԯ�zE�|�e}	Rn69w˩M��\>��&#ff�􊪝�}�n��z��1o4\��4��v��]��]��!����P�z��؍���ԩ�!�t2��c�u�@�1�yTg>��]iSw�"D�^!Z���4ʯ��sx9�7�5��.( u0�$/ԑ�ׄp@��(O��,�^S�B�E�����e)��/",CBA��c�4}J�b��n��d��n�J�)v�Շ��8�i[ߔ$2^��hV�]��ݸ'��Z���3B��MS��Zp���v!A50�dx�_NA6|@P���=iގ���Ƚ���e	�C�\5-D�
��;� v"A� �Q�`��a[ǣ��7�,��$b�uұ�P��ѵ7��R�����Zl]?X�p%#k��� >����?�$L`]ChT��=%j1�R�6�(��VB�Z�{a�d(ngѯh
�:���(�C3n�a�r��S����>*<L�A�Bi����"��F~�ԃ W}�H��̋��
+ ��]�q����3u�;��>���
5�u,��r�'|��>��s3$`�R�U�3� �a��6��hIRdsܛ~tڬ{�oRX-���7cDu���~�d���\5��V@\��wqw������e/T�Eى*H�ߧ��Β�LU������Lr�R
��{��&|���W]�a�ˣg��#e� /2���~�"�<y6�)
���;{[�\o_����f�:��ׯ	�l*�6̎��ז����FO�抩0����������AP�X)���U*����R�#��9�7� �k�$2�+����l�ap'��h� ԼO�׍8̮Ѱ��U��6�;-������y�ԪA
60�Mj
���c���у��Yc�C���6�騧�)Ak��fU�4?� �P�I:�= m̂��\�Q��꒗�rEbj�ش"n��;m7Ӧ����������ȝhQ �&���w�P��f���>9�$��aK�Y�.d��]�4$s?��u�a�#�#��8^���^�E&��3o����b|����VJF�lo_�,��$:#�q��p��U��k���yl��(�A+Pм�].v��@�A
����`  A��5�}O���E�/?L���OЖ�D֕�&~�[C>\�!������	[�5�J2��8cd����{�m�*S��uu#����dƇ��_��S\-������iO��i��]�kB��͞���Ç@�3��NL2��֐΁R����0J$����o%����^Uv��-nz�3#��S�Ys�[�y	Hҝ�@���9�*L���J�u ��4�Hg�����Ult������Wê��g ��r�����B
�y�{h���9�M�E,�4	�!��� z%��Uؗ�-c ��y�����8������ ��z9����V���C��F��u���_�'F=����uHFu�����V� w�,��`)F�'?�`6��šº��M�/%�j��|�Rp���G>�ľ�U�Y������
(�9�W�f�ҧ�< �*�}�5��3�)l��n .�9e�:�η���5Ϻ��:8���,�.�4��ڳ*!�uG\��j�/7�/LK~�-�����2��GQ!�����4��;ˍ��f��M�6�M`�Ru���-�1��us�{��5�i�z̺v�:e��E�x�N�^l��L�.?�,��1���	~�9�M�-�V� )��t�&�s���5�_h'H=��h���H���g�!����y�t�Ϡ�<�FTȃ��}H�?.�e6~E	�mm��s1
v%��!�*b(�7Ma6a̎%q���GrJ/�I�G�Փ]�M�:BWA>�@����kX�ϔ/j�>d1>�>Q��	��E.��@�=��Zui���A���#M�
�ry�ϠQ>#}6u�V}��cB��]�ڮ�sCrL)B���?e���5�����h�xվIh�U����
�M��K�!|}D��-	б񫜅�W��@�e�v~��_��*�L�<�����*6R�a�PЦAwc!P�#���v=�g@xh����� b,�A}��j���*���2
8^��}���Ts��n��BC�Q�8#�2�݁u������ M��\Ŀ�����M�8��rG���9[��P�$Z	����`�FQ����2 ����֮���p�5<�.�C�ç��	zL�[�:j<H/
;�E6����ܰn=18r���Q�=�0�\1���R��N[y
�vYq^���2M;g�!sb>|�P�:�a#���FQ��0��%���-Efw�-����#50�N�8�]������c�9�%�-e)��f�6 qP`������e�&��Wy�~�\�~
�B�w`P��ڿ�6/�9-߭�+򖱌��6`jpj�Vj\GJ��7&P�ݽ�����R���!N b��ȴw�}�C�����U-(�� ��J6����Y�ɇ&.��B�y�0C�n:�{���qJScr����7���P�[hu���w�4�����H�U�
�q���iZ�{N�w������q�)�5��C71��!��H/w}$��b��hY���� Cc����M��Y@.��;��X' m�V��H�k��W���)Mab��8���|؝�dVMq'�R�l��gV�_�Y�_���6"z~��EL�ʙ����V�N�@HAq����'�LR0��!,��y�p/?�)�1UD����_�c��������R�w���l��i)~9���x�v��aW����)�����쫹��~������9�Ӯ��.A�s<�A��0sC݃��2�����6\�.�tF��5Hyi�1��Q�:�?����)����H.]�S�F��UI�/[%�E��F�+Y%ƅR�_D���"Nmڔ�\�Y�p�h�m0�����)Ն�V{#��D1#��S��Z�;�C�K����Ex����)@����n��Ût3�@��ٹ�*���CTJ<�<Vp��˰�b0$���!��,��d�>�f�1V�X�!���ٽ>�vʜ;�N�Wa�r%|�`������3���Vfv:�Z�a���Ք�b^L�s��vMPP��i�ͫ~Щ�f�IW�=Z�B�;V/d�w�y�壧�;�d��k��14O2Ij"$�r��n���*@M�=,R�c؝��Y$���L../r��9�n%�l8��C�M;a0�+3�;?�\*T�@{�1d%���I0֓��P�!Q����$p�9!���5{&�/��JP���NJ2���^��Q���?��%��d�a̬���j���.�P{Z�4���I�x�������,Dy��櫯H��/݋
�k��G�^�-�^��@�ϙ�����{�Y"u���l��{gB)P!��l�$�ardn��j����w��cR8�_��LX/��hˇ��q?[���g�	:IP���Xf��� �[bHZ�F��뽱J���X�Q�nk���Vx	5	�����H!5����`�U�'1�=iC����M�=�'rȞ!����C��������cZ�������j���
��oM��7	#��\v˛a�Ib�zƀ��%��%,	�t�H:�4��I]�a(@&�m��QBD��st�v`%��X.���HC�����z�ص&�f}�h�Q�.�]��;�D �sԁΖ��[���g*|6��
V�i0ل X�z�[��&��zDW�?���%-�N)�ݥ8��>����ٟ����=�J�����L&AAQ_��S�C�U?�F����~v�{�!5nW[+��⳧b��T��uu�y�e<�-��(�?My7�̟U-�	L��e��o�@������RGN]\ջ5�'j�5"�`Fq�ε�G[:�}Ұ��˽�y�ADSX5�TJ�f���>$}>��{����~��O0#h "�k&���M�{ 7T���Ӧ����nؙ!oj��ө=0u�3�����6��/��|�>(?\���~׼�Y�_;$.R6�&+�̇A�J���6g��Z��H�ps0�xzmQ��u�7�X\5;�ҊQ���_���7��44�ҭS�Zr*�q g�2!�s�~Rۚ�2�c[+����3�$��8ܚ��y8׽=3I��F� >��}�������$����B˔�S9��&P�x�O@��}SV�"�_�.4���s �׃X4�;�0й�a��B!/��J���l�9�l\��ya��<�@~�e������Ɨ�>W,g^u��`�Nh+��ف��� C�p��i�.a�&��̤?�.3ee�#}��=�>F�	�]�i�`�>��;���+'��e����Ǜ�CŸ�C(���'���0:�̲�Q���W�6�Z�?�t�İ�{�O�9~V�o���Q���ǎ.D�����,�'4�G?� ���֙���3a�t��	V���X��{~U�[�r����v�fB�;��M�~��*=lod���-b���j�S�) ̪|݁�!Λ=����>��p�W|��zk���3�ﲥ����/o&s�
@��ǿ�#*���@��鍫�i��A^P>�a��+*ȣ�#V���y.�ڤϩ͟'��~Hu��Rina����wWJ[��q�+u{�jyS�:�������so�؉0��"�4ϕ�Ll�XI��=\<D�\�A��'>:g@���&ś���(�_�V��4~oԚQ�I�ب�l>�p�y��,,��E�
�gI���t\OPC����!��D���\IȠ��Ɠ�I��2[�����"Dz-�>���n�1��Z?�Q��`�ˈ�5�G �Ky��0,wU�WbN{���Fm�V�S���;^v�\u��.q�jD��3���#���G��o�Us���vt��~^��Ś�uE�i����uV�	՛G�U#��M��f&f�δ�&f5b�0ތ�lP�S�*t�E�����\c���P�h$r�'~8�=����-���窣�q�xY��c�9�\��z��J�}A�{��p�c?�;9n���w���c�!e�dmI�c2��֏p��� `�����Ia|�b`��i�{���ԍ���5b�J�:��o OӃ�%�=�����Ǚ^t.g"ʺ��}$�H7N��jUqJ7N��o����3�e\͵fw�6i�C��X���B�
ҏ� �����N���}��MU��4ԕ놣���U
����8t[V���0_vr�-l��!�ձn䂁��R�DA�;RzG�)�>'m�^��B)�Ɵ�@6��[��.}��ܧ�[D$�=�D�)��I6P��`_���Q���� Q���3Sda+LP�����^з�|� �����ћ������SXφ��4����FY�D�D�?0A��w�׬w�ؐ�m��2qt+bq�w�diݧ7q���|&�ڄ@�u��[C��54��4hsK��|�G�hk�;�i�Z���D�_��/��Vz�����G{�"�b�t _)�e���S����Q���|C��o�,�;��u�!����s���t����Bն����xHG�	O���ވ���v�(��B=z��N��w�������4�it��0TX��E���J�2�����uIW�p�د_/߄�T
v�U`[$�^߀}�x�i�j��Za�J�(��=a~�� ί��D���^����f�Qlj�2�K�&k/Y+\�� ��f�����h�Ŝ@X'iH�"z����&d�'�OAwC�_���AbU͂�th'{c>4I��D��K�R.�F ������]� ����\V�I�&/I�㞫I�4p�|���q�"�n���L5��v�����&���.��\>}qX]>��uЛZ�R=CN���z��,�'�+F����d��C"�rjz$�F�|D3��zN��7X�},�Ӆ�X����ר��*�\V�F���[M�w�anE��Cu���˚[2p��+�N��v���q�V$�ɜD���Z��cn�=�;~�t�+����փ�h��896)�̟@�~!#{��᱃6������Z�D>�� ��\��&�t�d �@1�A��8�&����-�"�=��f��!�F`]P�֯ b�sC(:�F�ܖ�+u�PѾ��<������_�V��o5���jn��bk���Hv�N�YQh� I�ƨ��v��%�^h�`A]��ՠH5�]���U0�9������wŀ9�2�sd^-�f��o�k�M�˩]��M��Z",��Y���D}c=���s�MڰU{;y�
Y]�����('7�� r��C&^S"1��E�Z;ȺpTX�:R�+���G�mYl.<�������D$j�@�;�N��{&�&N|��Q�?�B����f���	)X�Q�ߎSks���c�ͦ��5I-wI)��^f�|���iW� &��j�DH��K�{	���tm��K����}X�y������"�0?� ���<b�ONģ3�QH2���{.�P����4E�T��1���<����?��SQ�`W������߻49{)�%�E��0�O �2̞ r�&x&�O���>ce�R��,�����$��.��YnW��n�UҪ�ޓ�E��L����D��[0sv(ĚS�\)3Q4*��{A�[;l���j�1�Äl�l��"2YՖŊ����,�t����G�}��{�&��?�����% ��$>�*����n��u�Z➹u�)n�i�:ߓ���Ui'i�W�6�h㞒�1 r:u5n)T+I0���T2;�ͪO|�_mqє����t0��I�D+tE�}�6�.�ؤ�3���wW.5�vK��hٯ�ۤ_ �RK~mu3���F�E�ž˯F5��8�05���n���$��7��)�TB:2O� ��&���=1�f�\��ѧ��:0�/1!d���nS��:-�i�*G�i�s.ܠ��?�D���
�ܸ�	_�;ic��w���⁬"�07�!*j�xJ�<΀�Q�{f\����U>ے�0��,$6L��r�a�.7��H��g��5�N5\�Tu�]��n�z�5𴤲�����t�g��T<��!ZY=��1���2Cw9~�NĔ��Ɏ�JE�M�#���1۹RbTJ`O�X]!��4�!����,��u)6g"���c����<K#0���bsH�6��8W�t#�K���ر�ܖ3��k۵�bG���t��[�S�M	=���9? 5���1��4���.s��2��@/��?�S1�]����W(r�dI�n�]ﲺ����d���E��&:�K��JpO�����	\�[�ܬ��?ŔQ�_��"&*��a��Xa�)�H��4��%)����yb��� ��*�MK�?yQ�s2ӥ���4���۪��$�k����������'*���6S�\��VȨ֮z׃������!՛9��]<H��J B߅â�VI��u��E`���@MxO����}�,��k��M��]��GAraֻ��8��Y�����܁j/_��? kAxw���ּ+=�z��n�!P�^��Eކe�s^���ƴѝI�$�_ơ�Rz��/7�Ǿ�=��������1�es�_�mSy���%�ZJO�U k�����L�d��i�/y��� �M:>���U�JH$jN�ş&Q��ɏ�� �SƤ��Bf,aM/KfXB�8� �#�A��>q�=�T7�R��������O�iS��~=((d�|�n�Lc��F1H����l� ��d���K��oa��M5��@]�\9�g�#�����U�+!�fћ�Z������>�T�����#����:l�[�|���|�T��40}/���p�y,�)�>�E�����_����湣�- ��)�OnN �'����%�9����&�$l*�i𖻣�
a���\��n����E(��0�a�}R��Dw+�y�氞?j�U&��XNV�򁘲ա��H�=Z��OHmQ�9X�zal
|����C3�����)����f����䧊�w��.�I%������d����޴*�໴�4��� �p�5/��IAR�p�̛�jL=A���h���e��u����w%-�! #���ߑOh�^K)BE?����&bzq��b��j�h/�;	9�,ѡu+@`���Ou��
��	qolV��Ô�����RJ� ���fd����4���Ӵ.J!S���t��4ņa�[�ψ��A��^M!>il���3Jm�\O��겅+2A64��V.�$��Kg��m��7�'e��U��`�b�K�C�>*(Ӧ6�S����i�o�S����(_�����3=���e\0�y�
���Z��N��f@��N��!�@ZOJ4;��(����>��;G%��/�@�
�Xhe�X%r�٫aB6�ˡ��d�[C��9�F��nv��2c��p�S:�J,���f6cY���,-�`�g�e3�d!.�d�[)¯x��J�'[e��V�SE�-���Eff>>� n��~���:�`P�*�&H�F�&��]B�a�wu�~[^#\�����M���5��b�(��%U���2/!�K��I��::EƊ=�)����KF{�m�V�$;�=�:NKz�����*����9�J������� �P0݇��c�����o�:�a!E�L�ӯ�7�Zp��Cl2�qƩ"6+i	�a|7����ū">�������N��_Ka�b�����X���P�4�4��Iӊ�޹#8����/?j��1�]�
��0��i]	��$X�l�w{rwps��� <u/ bȝ��'n7&�l~��k��qFX	��=����@שIC�s;�a���R�j���N{�ذ�~�+�՘��V���R�nF�Ҙk}\��B���6�gNxz˞ewQӂ�rࡲ;	a;��%+fU��SV�^��4��U���9<��וi�@��SW�O�W��~�h�ïy�2��T�C�Q
��4tlo�:}PB�=~T�R�{u9��a����#��:w�F�n��E����(+p{Ɔy����$N$ #s��j��9�~���yeÐ�W5wI9���J4t��	�C�1?����w�hp�A&O���{�N�Nwwb#�p�L�ߡRo�7��ŏ[ލV�P&�O�#2�7K|h2r�P?0�)l�-�n�O H�C�?[��;��&?�\�3C`K&\G�ӻ-r�%Y������׋��H���#���|�K&�&T+y�[жXvtH�%١��W���&ERY�2h��ۀ[������S��X1t�w�W�]���������kk��	�[�C�p�7�7��E��>�q��7����[�T$�����cvF0�N&�H���	���)`h�d��^�k����G��Wj��"5�]Gwh3h^XҎ�R������ݴ�c.���2�na����q��3��/�P(����Z�>�>j+{[�յVw�y@E�'l���1����LXd��r�V%s���?�dۛ�P|�t����(�u��9�*�rX�K��Í��V|/��(M��c�sVO�� ������7�E�p��@e�pCXBp�G]��V%����&�c%MD�(P������&)�Z�̌\ɚ��������o0%��o��b�d��'�&K��W�s'���bA l��C�����!�� �5����Q|�fd�S��∩�U:ѝ̷��H���R�� ���*�{�n-��`�d���a�;��X����0c?���{�QB� ���5�ؼ&y� Ao��Z��UL��p�Q�y�vF�l�μ;#���թ���E��A�l
��&���Hl� �����Me�n���jM�~���v��".�	��I�w��PLe(1plU'��\�V4��*����_��=6��;�2w���)͠�Bc��p���UJg#+��pEU���|t��fI�gnu���d@8���TK�<�)6��/�4K͎�Y����9є�������N�� mM7wݭ$o���>��ӣ�
p�����P�s�P��+�=hf���#�	ETڒ�*�Z��m#�:X�2t`8VcT��-�fB��^�"As��v�!,��xz���E��)��ߏ�٥��︎�<�U��"��}!��7��8������	����'���0��4\-�|<,@�UZ`�g�EW�+CHڟ��ż�������=X{>fit|l��eW��|=���`�tL���� ��R�x�y^;����OՃh�X���w�I�s��Ug�8�
���D�g��-pR!�W����}q�.��mǐ����0{)�W�}��VҬ�����c��Oz<���*�>��Y��`@=$ͧLL�d��zN����9�0;y\��#�&Mt t�iP�A��r��o��N���������ٮv��U9�檗xQ?��/�!C!V��%w��-�,%��xh={��r��/_��Gh�5�h�J�U�*�=r�j4��皠j�OF��5�`3|��*�����J�6U&����@G�G|=C��$�)����J~�M&���8Q]nZ�e�[0/���{��������́c'o��o��緳���ل��O2��#߭�Y�RV��6�V�����S������a�1�H��\3hѺ���ywP�\���c��Y���8����a{��`eƹ�����}��(�4�M0�I����&�Q"MD�L��tq�X�� ���RW�V��N+4�å��G��Q��z�W����P��j8����X���a.5�}F��8�/'a����O�Ѫ�AV�z#������a����p?��) �|\���
�jr�@�����*�'���@=����#�E����AK����c�����U�}PJ��M�xΩ�8Q���O�a���-_��u��LIux��0&+6�3�<�l~$*;�#H��f�)5%6IzLCKQG]O	P ���WiU�����@�:����3��2�C-S���W
qk3no�S�ܒ���!�OA^c����H�~V$�ĕS�I��<#)妊�c�@�*�0i6�;k�����L�B:ԾP�
��u��6s}~�|Ѐ{�����b��Z��Z����#�&�X���y+�\9Ud�.柟vb^B�{��Q%�����d�&�/�� �p،&��.�etg��o	~3� �N���xz����Q�'�������$f��ĉ� ��1�u�ӄ90��N�INg��3Z!F�$OOy�[��D*W���⥒(���܀D�b�[���b�=Rq�n&�n��>(t�L��NI���sJ�7:&�R?6�
���b3 �pR�K�E%�$��[�t�p\	�M�> ނp�3ֽ������%r�a�gk	����9X���J����j<h���k����~(� �a�Sg�����!K�ϒogtY��T,�Ҹ��2޴�d����~h���r�$w4-ӄ �؝�-9��@��q"��PW8�ߖ%.0�L�=�����پ��b�'6'��1�d([#Xs�f�D��"G���;tus�|b�
�>T�M��*ʋ�J�ܢTd�?&Ξ��{�6��\kgF�ma�^��h��Z�>����9	OPѨ�6�:͔�^�f�<�IZk�1^l%�ߜ���:�ۙ��y�;:�������N(܅�_Q��(0���T�:���m$�3�ʼN��j���[W���!��n򮙅��g��F�&6�4�0���UjN��_�&�hΑrOJ�2�1�/y�|��Zr��f9E��M�6���	�*�O($�m3�y�3��;�ӌr	}3��mͣN��ieIfFm*�P��#�J�F���-!�l��n��m`�~��F�G���]Q�ܴ��E�e	
�_��"���MֈW�������Y��5Ҿk{�v�_ɿ��a�ǐE�>��ێK��0���"�?,��b�ly���V��J�T������y��q�@iR��@�i;��̘P�#>��������[ad�1�ê�x��Y����[�$h�*j�r>MEd�V[�|�߼��F\tjEhJ����k���=�9���W�b��`q�Q�eٜ���ρi"N}���o}�Q8G�<|;71d�4��\�}O����*f�U�W"�^�2埼H	��3ʧS�h�����g�A��	oq
�n�p��_�S�	����oLj�feA�2�{+||BCt*,������ۈ� }�n�.�F��K�%�v�~�=y�)	�Y��� �dh"(vw��L�1ptL��u�$���Q�9w���T$��{�]���$qg�8#�$���*�㲧�vԗ���|H�v�Py��cx��k�v
-�x>Yj�[�Q��<��z<࿠�R�db�!6�;��ў|Gd?1��T��+E�l�n��VcX����X�Q�-Q/$��9�8&J��K��B����E&����'��0��7W� �Ӿ	y�����5Gu����^�9���6A��nnC�#��{~P�iR9M�1�5B��6T�gӝ�TS�D���轳FX��l�c��XF�pIډ�v*įw�[1�dڐ,Fi�h`���(<OҭbR |.�&%�s�
��)0��X�֞��>ÞRG�#/Q.����s�v�B�-5}U�Ve����{mՔ�z�ooy;���"m�&Z�A��;�Dl Y���׫R{�i�*.X�S��2�h���&���� 8������BЫ'i����-<�ιlVl�7&��7�'��$6,���|r?�ـ�BY�R��/�T9ɑZq'Ylw�u�}H��1Ȏ�C"��풸	���%�j��$Jwe��lSj�����^a����xA=������"�"حrj���z���ޯZ��H,4t�|���059�֧��"'n�FP.0~��֟�T�N�\��y�Dbi�{#l�����$��q][��&F�'+�S�C���^��]�5h�O����� �;��Ui{\�|������	Q>1���&K$~�y{=Ŭ4��5�����^��)�Gv�R�<7=h���ท4x/�
�=U��*���/ql���}�]��V�g�U}��0��\��\ &�e@��c{A -~��}�Njs�'Zs<E퉻&�Ct��#ʭ���U_V��M�;R�P��e?�u���ϗ�i��	Px�H��ِr\�#�e����>�b��\���(V\�t�� p�o=�eU�%��N/#*e��V�fG3��V�(�b�4Ȅ!!&P[s��9�PA���)��g�I:�o��0���5�[ו�}s�vl��j�h���h���9�˰d�9���w�$�>���F�G��=8�t�}
҇��0<�iL;I7ئ�Dr��������I�"J��_����7'pI����;l @�\�Jt�L��z�Wu��ݧ��`W�C��'��*�[AJ�=%�������q[�g.62oJ����nnP��XbT}�@�д���~=P�F!n�D��C��!���Ǵ*HsN�щ�s�\��gӭ�;�Ib��i^QZס=}QY"�U�# d �s�Ux!)4����0���j& �����
l1@���p ��%��o2;ѷJj��(��tn��4�Bf�d��\�n(q�Hn��@��kP�vAo�S�X���4���큧����g���U��]��v9�t���R C��3�[����~���K�k��(�dI���|�+�>�QG�^E��ȱ��(�N};�n��A���W|�s�$ t���
%p �^v
W|���3�e/[gud 3�r5p�c����2����%����<(-O��^���馳t���2%�qDM&V�p=BY��$d��\ac����HY����W�N5��d �;��#��m�G�	�y�H���*�)�t��v��.�Q����#��
;�\�G���
X������E����)�1�=I�� �)V��A�ُ~��b�8��T����8�i�����qC��*��?bq�n�ap"��	W�}
��LI�2ӡ[|Mt2]П}�����eu&�=��]�J+1��µR��481{�+T<��"�U&jú)C��'
�����ƕ�{�wc�W���'}���b	X?")��@H��,��̀J��lX�@CpQ��� -�f+A���s�m��"y��N�Y�w;|��jgͱ*����x����mt������5�k�V�p�Ș��	D�z���m"6�"��W����h'`�n�6q�?��N���_MŪ��Bw��j���KE�Z1/�dGe�� �Z<�Sԭsji��(�b�Jv,��D�U��gv��0�8鯎�������7����9�H�*�BmnG^�ڣ�6��2|G|� �]<(:z1ހ�ԫ��˚����	<��i�Ч�;����?���j���+*YS*�R�Y$�[]UޙMqE�6q$������	�3�V����D_��u��X�r2�^R���Y�z����y�HZ6Ywp�����6�ERY�������,q?��>���[��3�>zfեC�Y������]�/Y�cs�E�Ɓ>gL�}X��|�[_�6���$Y�lD�N��x#����F������2��*/����Aw|��v6�rl���t�d4O`_�x���f�}J��!EO+��ޝ���O���6�5���I3���GyĴ1�^+�Vd+\�w����ěo�\��o`�m�`e�������D�� fo�2���d�\j:��FH#�uʱ�'Y�7V�z槭��:�`3&�c=��`h������8�i�xM^[��ji�Vp�p�+=E�|�d�ʖ_Qf�%�i��3d�;���/D�@�
y��1A��8�v#�J轼/ť�y���|-�b���;4Z������/�1ϗ©�<�2lw��-]|f~�T6�n��Q0�#!b�b*�j���uC-#W�EFG���d��G!��R�$�F�h�q�㢶ǻe�+r���.�]6X��i�̵�#v7/n!��O�о"dI�5˫�ᢴ�n\[lw��2a岘�a,Ս��J�S3�t@^�0p�����af���c><�=H�f��75���!��q�>��Sq�;�Q9��&H	��Fȑ?��όA���A��Ds3�g~�N
�"�ig�3N�/���'][���՞b�V��+>s����:�q�����p'Py�QiSm���?�����$c[﹩��X8�����|�Y�Iw���ÔN�,���e\L�WV��x��dM��D�_q���50f�XЗ3�E�<��Ӈ|b�}H��9�Y�c�˞���b"�/��=�_ީ�8�B��|�3'ɹ<���#sa��s,:b$K�0��/����u�1�czΨ�S�1�p�C�>Ah�\1@�BN��:��'t�$j^Y�Kg��5C\��mc	��Iq����;����ĚZ���@�"u�ܜ;8�Yw@G�b����ʫ�-���?uW�T4��i��&L0�s�I��ܜɣ��
@�ܠ��\��_n�	�D���ǂ��I�d6X\��2�$&�#gl�/�% �>���ɦ���}����}�
-8���Uh�B�
A�ը�4P����?�z��i�[�ԟ4��h^h�sSV��'"p.K<z�j�{Y�]|<@��,��� ���1�%c��;f@"*��3�����Md�����ÒL�cv��G�4n�3i��w���v]�`ƻv�-Za���@�>���ͨ���z�i�7���E���Y�bQk��] ��@����|�Sm����"�	�:�Vo�"	q�ӗ��cY���1�F3��64�x6��4bcuD�� �vXi����������}vi��hM���s�3z}�1���8q>�*��8H���ElDCl�b~�%��-����f>�b�DH�>9�(r�}�}���+1T�ܡ�t����#�Ggc����Oi5:�5^��{�S ������P�P��
�/���ܮ+{��ѧl<M&�3�r��R�:���4F��7`�(]d��ܪq&� ̓5ԍx��c�eD�Ʌ0�~#��oh�8���2�.6�� ���"͊�(0���xt�J�����-^0�ZQ},���,�#;�M9��\w���s#��)��&��Rh=Α��1�!+W�F�8�~nz�6�65�^�*niE=A%$���\���� ��w)�T����])@��^�Vx�(����}�(��G��%+���4��ｉ�C��%K���f��H`�v=U��o�����`��6�[::p2���u�tpxz�(�E���$��]���X	3~� .*����(����"�T��I��4���rͫj7�H��o�Ȫ��XVҎ��W-��z!�,v0�QS��˔�d���5j����^�5�ow��R�W��9<�Կ�M�ڇ��O�y�ƺJ`K��@�$���%\�I�2�v��Lj���;m�"Q��	hL��!�8��7I):�B~���o� 퓽�6�fb�%�O�Kk��q�Sk�Ys 3hpP��(H���
i��T�`	?�J�W#k��ߍ��`�{Zi�$"-�qK�f��pMe��ϧ���x0Ɯ��N�Ҙ�]2�Jx�:l��\$�fY���ƴ�@x&N>�ݶ��&'�����nO�.��;o��>��͆��q�^=W���p�r��^���t�����M��g�I����O�#���YړB�d%'�$�%��ūS_"�)�bq<�A�R=����Fw��B���p�L�*�]�@a��/Mo*�LP x�Т����E�L��p�9���2/�9����{)mx�����f˶���F�+�'2cto�v�(K��>�0�v(*�<�W]K7���]t�!OkYj~(�������ĒyU�����Y�k��ȁ�xς�v�B��<;&��~���b$M��[�Q� 
2�m��=k�a�v���
�Wq	��b
���Y���˝5�τ-�Vc5�����)����3
��;�)�7�Ӧ��v���O�4�ӛ���E�����@2��
�j�0YV��ٺٽ�&gz����%s%�+��V(�CJI�^M����_������2)v���A(��WH0>N�� D����t$I�e�9���~�a��_+w���ItJ^,D6HkE-��ւ�s���k|=B�J�ȇm��Pu�J����=dgd�L�k庱-$OT���h�$^�G6z��� �X��p�Pv����=�Z-Ӿ8v��3�bv��V�lf�(ԛ�-�j�PP{uH	���&�˯x����(9=��>�����pb��=m_�p��f|���PiH�\��ո�.�=Ue-�µe�w/��:�m�G �e�j�x=��1�\A����f/����*��q�E��~܉��*�<V[��֖+.���w�*B�U����Z��X��/��*��]�"�d;����2�m��9Fk���G�K�WE�h_kP�3��`M��4g&���9'.��K���Da��C�mI�h�������{�@�&�B4m<S;�)4�	j��|�*�G��+�H���|����ڗѣ\�,��<N~����;�G:�7sO�|�e�]�?m�������)�V��hP|����Ua9� �z�۩� ���� �1��FTϛ`����w�������M%ev⬓"%#5>��#E��b	�-�j8��`��Ю��L�*� �2@Su���8����45A������0V��rsR*��VO�υ��ԡi�7(U�����GV����ߚ�/V4D�M݉��węO%T
�;�8-��F2,�2�tRn���S�-D;���{�$:A��#6d�o �i�Oi&�dv$���&�1?� ��ǒ��BCw&FW_�3�^������0*��dNX��}kU�An��h��U��Ψut{�q�tA�p�z��Je�.�T�7��q��Dϲo�������e�{�����ԆK���n)��~ �텼�u��.�Ȕ)�������dk�q��P���｝�I�k�e���m��*�Q�H{�D*��as��;:�)`ٌq����������G�{���܏���`�ʟ�Vܑ u�}��G��3ݔ� �p��x�'�����Ss�����A�7��v���:�k�6c�mX���$��N�ܫ�x��I�g�:#�N_�$�\N�d��t`�=���f���o�0��J ��O�x;�����Wр� q�����h.��3r,��b���ת���O�rYM\}|��"b��[5FlIV	��[��G��t�F�e��Eߵh]�����OD��9�D6�6'��,|����ZP��]�B�76��2��.�m�d�	�!���1���1�j�T�q~��:�{$����Q5���3���0�o�*O=����{y�T ��ȫ\�`�]~��FP�]Z"�ަ��
�����%�����I�3jh п1�g�gD�N�������r3��y��˃8	�t�q$a� �`��i���q%��zM�,0H��'��}F�0Y���Z�P�1!"�U��y����WNH	Fb��	��a�򓸯S��P���K�F҇N{w�tv5Ӡ�u�/�tʭ���HN�1�E�ꋬ��?�.�OR�v��T�tDW4I����s��J�6��\��&�:�t�6+�MW�_��/XbR��)x�k��|u+O�����:�垳�]�U���k�D-�������Œ�<FC���Cq���׻���<ﻋ��W��u9>���Nlr�5�J�Vz�L����r0��̛`��6/�&������p�����D��!�NBl��P��"܋ٌ�[�,yuU{�L,�d[j��3|��={cN-�1�C�9d���t�'��4��b�)I}X�ʽ��D�
P�
���b����J��Ȣ�+���P�YIƒ@ㄚfy'�V���p��Ɯ��BOrx�}�KJ���{i��/���}]�!۞�X���p̜�t��t���*0_ ��#�#�%�w�S��Q�} G�D��r��dꪝ��DR\*nB:so�c���n���)m/�5�'r[����Qq����`6�W{nh��Q�3��/���w�K|I����W�%�i|�r��{ ��1��"0/�ѓ��]֟m�	�P͜�H
k0�v�@�� :(�ܥv�̤�r��w7ބ// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/file.h>
#include <linux/suspend.h>

#include <media/v4l2-common.h>

#include <media/dvb_ca_en50221.h>
#include "s5h1409.h"
#include "s5h1411.h"
#include "mt2131.h"
#include "tda8290.h"
#include "tda18271.h"
#include "lgdt330x.h"
#include "xc4000.h"
#include "xc5000.h"
#include "max2165.h"
#include "tda10048.h"
#include "xc2028.h"
#include "tuner-simple.h"
#include "dib7000p.h"
#include "dib0070.h"
#include "dibx000_common.h"
#include "zl10353.h"
#include "stv0900.h"
#include "stv0900_reg.h"
#include "stv6110.h"
#include "lnbh24.h"
#include "cx24116.h"
#include "cx24117.h"
#include "cimax2.h"
#include "lgs8gxx.h"
#include "netup-eeprom.h"
#include "netup-init.h"
#include "lgdt3305.h"
#include "atbm8830.h"
#include "ts2020.h"
#include "ds3000.h"
#include "cx23885-f300.h"
#include "altera-ci.h"
#include "stv0367.h"
#include "drxk.h"
#include "mt2063.h"
#include "stv090x.h"
#include "stb6100.h"
#include "stb6100_cfg.h"
#include "tda10071.h"
#include "a8293.h"
#include "mb86a20s.h"
#include "si2165.h"
#include "si2168.h"
#include "si2157.h"
#include "sp2.h"
#include "m88ds3103.h"
#include "m88rs6000t.h"
#include "lgdt3306a.h"

static unsigned int debug;

#define dprintk(level, fmt, arg...)\
	do { if (debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s dvb: " fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

static unsigned int alt_tuner;
module_param(alt_tuner, int, 0644);
MODULE_PARM_DESC(alt_tuner, "Enable alternate tuner configuration");

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_tsport *port = q->drv_priv;

	port->ts_packet_size  = 188 * 4;
	port->ts_packet_count = 32;
	*num_planes = 1;
	sizes[0] = port->ts_packet_size * port->ts_packet_count;
	*num_buffers = 32;
	return 0;
}


static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);

	return cx23885_buf_prepare(buf, port);
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_dev *dev = port->dev;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(dev, buf);
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_buf_queue(port, buf);
}

static void cx23885_dvb_gate_ctrl(struct cx23885_tsport  *port, int open)
{
	struct vb2_dvb_frontends *f;
	struct vb2_dvb_frontend *fe;

	f = &port->frontends;

	if (f->gate <= 1) /* undefined or fe0 */
		fe = vb2_dvb_get_frontend(f, 1);
	else
		fe = vb2_dvb_get_frontend(f, f->gate);

	if (fe && fe->dvb.frontend && fe->dvb.frontend->ops.i2c_gate_ctrl)
		fe->dvb.frontend->ops.i2c_gate_ctrl(fe->dvb.frontend, open);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_tsport *port = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &port->mpegq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_dma(port, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_tsport *port = q->drv_priv;

	cx23885_cancel_buffers(port);
}

static const struct vb2_ops dvb_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

static struct s5h1409_config hauppauge_generic_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_ON,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct tda10048_config hauppauge_hvr1200_config = {
	.demod_address    = 0x10 >> 1,
	.output_mode      = TDA10048_SERIAL_OUTPUT,
	.fwbulkwritelen   = TDA10048_BULKWRITE_200,
	.inversion        = TDA10048_INVERSION_ON,
	.dtv6_if_freq_khz = TDA10048_IF_3300,
	.dtv7_if_freq_khz = TDA10048_IF_3800,
	.dtv8_if_freq_khz = TDA10048_IF_4300,
	.clk_freq_khz     = TDA10048_CLK_16000,
};

static struct tda10048_config hauppauge_hvr1210_config = {
	.demod_address    = 0x10 >> 1,
	.output_mode      = TDA10048_SERIAL_OUTPUT,
	.fwbulkwritelen   = TDA10048_BULKWRITE_200,
	.inversion        = TDA10048_INVERSION_ON,
	.dtv6_if_freq_khz = TDA10048_IF_3300,
	.dtv7_if_freq_khz = TDA10048_IF_3500,
	.dtv8_if_freq_khz = TDA10048_IF_4000,
	.clk_freq_khz     = TDA10048_CLK_16000,
};

static struct s5h1409_config hauppauge_ezqam_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_OFF,
	.qam_if        = 4000,
	.inversion     = S5H1409_INVERSION_ON,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1409_config hauppauge_hvr1800lp_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_OFF,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1409_config hauppauge_hvr1500_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_OFF,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct mt2131_config hauppauge_generic_tunerconfig = {
	0x61
};

static struct lgdt330x_config fusionhdtv_5_express = {
	.demod_chip = LGDT3303,
	.serial_mpeg = 0x40,
};

static struct s5h1409_config hauppauge_hvr1500q_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_ON,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1409_config dvico_s5h1409_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_ON,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1411_config dvico_s5h1411_config = {
	.output_mode   = S5H1411_SERIAL_OUTPUT,
	.gpio          = S5H1411_GPIO_ON,
	.qam_if        = S5H1411_IF_44000,
	.vsb_if        = S5H1411_IF_44000,
	.inversion     = S5H1411_INVERSION_OFF,
	.status_mode   = S5H1411_DEMODLOCKING,
	.mpeg_timing   = S5H1411_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1411_config hcw_s5h1411_config = {
	.output_mode   = S5H1411_SERIAL_OUTPUT,
	.gpio          = S5H1411_GPIO_OFF,
	.vsb_if        = S5H1411_IF_44000,
	.qam_if        = S5H1411_IF_4000,
	.inversion     = S5H1411_INVERSION_ON,
	.status_mode   = S5H1411_DEMODLOCKING,
	.mpeg_timing   = S5H1411_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct xc5000_config hauppauge_hvr1500q_tunerconfig = {
	.i2c_address      = 0x61,
	.if_khz           = 5380,
};

static struct xc5000_config dvico_xc5000_tunerconfig = {
	.i2c_address      = 0x64,
	.if_khz           = 5380,
};

static struct tda829x_config tda829x_no_probe = {
	.probe_tuner = TDA829X_DONT_PROBE,
};

static struct tda18271_std_map hauppauge_tda18271_std_map = {
	.atsc_6   = { .if_freq = 5380, .agc_mode = 3, .std = 3,
		      .if_lvl = 6, .rfagc_top = 0x37 },
	.qam_6    = { .if_freq = 4000, .agc_mode = 3, .std = 0,
		      .if_lvl = 6, .rfagc_top = 0x37 },
};

static struct tda18271_std_map hauppauge_hvr1200_tda18271_std_map = {
	.dvbt_6   = { .if_freq = 3300, .agc_mode = 3, .std = 4,
		      .if_lvl = 1, .rfagc_top = 0x37, },
	.dvbt_7   = { .if_freq = 3800, .agc_mode = 3, .std = 5,
		      .if_lvl = 1, .rfagc_top = 0x37, },
	.dvbt_8   = { .if_freq = 4300, .agc_mode = 3, .std = 6,
		      .if_lvl = 1, .rfagc_top = 0x37, },
};

static struct tda18271_config hauppauge_tda18271_config = {
	.std_map = &hauppauge_tda18271_std_map,
	.gate    = TDA18271_GATE_ANALOG,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda18271_config hauppauge_hvr1200_tuner_config = {
	.std_map = &hauppauge_hvr1200_tda18271_std_map,
	.gate    = TDA18271_GATE_ANALOG,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda18271_config hauppauge_hvr1210_tuner_config = {
	.gate    = TDA18271_GATE_DIGITAL,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda18271_config hauppauge_hvr4400_tuner_config = {
	.gate    = TDA18271_GATE_DIGITAL,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda18271_std_map hauppauge_hvr127x_std_map = {
	.atsc_6   = { .if_freq = 3250, .agc_mode = 3, .std = 4,
		      .if_lvl = 1, .rfagc_top = 0x58 },
	.qam_6    = { .if_freq = 4000, .agc_mode = 3, .std = 5,
		      .if_lvl = 1, .rfagc_top = 0x58 },
};

static struct tda18271_config hauppauge_hvr127x_config = {
	.std_map = &hauppauge_hvr127x_std_map,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct lgdt3305_config hauppauge_lgdt3305_config = {
	.i2c_addr           = 0x0e,
	.mpeg_mode          = LGDT3305_MPEG_SERIAL,
	.tpclk_edge         = LGDT3305_TPCLK_FALLING_EDGE,
	.tpvalid_polarity   = LGDT3305_TP_VALID_HIGH,
	.deny_i2c_rptr      = 1,
	.spectral_inversion = 1,
	.qam_if_khz         = 4000,
	.vsb_if_khz         = 3250,
};

static struct dibx000_agc_config xc3028_agc_config = {
	BAND_VHF | BAND_UHF,	/* band_caps */

	/* P_agc_use_sd_mod1=0, P_agc_use_sd_mod2=0, P_agc_freq_pwm_div=0,
	 * P_agc_inv_pwm1=0, P_agc_inv_pwm2=0,
	 * P_agc_inh_dc_rv_est=0, P_agc_time_est=3, P_agc_freeze=0,
	 * P_agc_nb_est=2, P_agc_write=0
	 */
	(0 << 15) | (0 << 14) | (0 << 11) | (0 << 10) | (0 << 9) | (0 << 8) |
		(3 << 5) | (0 << 4) | (2 << 1) | (0 << 0), /* setup */

	712,	/* inv_gain */
	21,	/* time_stabiliz */

	0,	/* alpha_level */
	118,	/* thlock */

	0,	/* wbd_inv */
	2867,	/* wbd_ref */
	0,	/* wbd_sel */
	2,	/* wbd_alpha */

	0,	/* agc1_max */
	0,	/* agc1_min */
	39718,	/* agc2_max */
	9930,	/* agc2_min */
	0,	/* agc1_pt1 */
	0,	/* agc1_pt2 */
	0,	/* agc1_pt3 */
	0,	/* agc1_slope1 */
	0,	/* agc1_slope2 */
	0,	/* agc2_pt1 */
	128,	/* agc2_pt2 */
	29,	/* agc2_slope1 */
	29,	/* agc2_slope2 */

	17,	/* alpha_mant */
	27,	/* alpha_exp */
	23,	/* beta_mant */
	51,	/* beta_exp */

	1,	/* perform_agc_softsplit */
};

/* PLL Configuration for COFDM BW_MHz = 8.000000
 * With external clock = 30.000000 */
static struct dibx000_bandwidth_config xc3028_bw_config = {
	60000,	/* internal */
	30000,	/* sampling */
	1,	/* pll_cfg: prediv */
	8,	/* pll_cfg: ratio */
	3,	/* pll_cfg: range */
	1,	/* pll_cfg: reset */
	0,	/* pll_cfg: bypass */
	0,	/* misc: refdiv */
	0,	/* misc: bypclk_div */
	1,	/* misc: IO_CLK_en_core */
	1,	/* misc: ADClkSrc */
	0,	/* misc: modulo */
	(3 << 14) | (1 << 12) | (524 << 0), /* sad_cfg: refsel, sel, freq_15k */
	(1 << 25) | 5816102, /* ifreq = 5.200000 MHz */
	20452225, /* timf */
	30000000  /* xtal_hz */
};

static struct dib7000p_config hauppauge_hvr1400_dib7000_config = {
	.output_mpeg2_in_188_bytes = 1,
	.hostbus_diversity = 1,
	.tuner_is_baseband = 0,
	.update_lna  = NULL,

	.agc_config_count = 1,
	.agc = &xc3028_agc_config,
	.bw  = &xc3028_bw_config,

	.gpio_dir = DIB7000P_GPIO_DEFAULT_DIRECTIONS,
	.gpio_val = DIB7000P_GPIO_DEFAULT_VALUES,
	.gpio_pwm_pos = DIB7000P_GPIO_DEFAULT_PWM_POS,

	.pwm_freq_div = 0,
	.agc_control  = NULL,
	.spur_protect = 0,

	.output_mode = OUTMODE_MPEG2_SERIAL,
};

static struct zl10353_config dvico_fusionhdtv_xc3028 = {
	.demod_address = 0x0f,
	.if2           = 45600,
	.no_tuner      = 1,
	.disable_i2c_gate_ctrl = 1,
};

static struct stv0900_reg stv0900_ts_regs[] = {
	{ R0900_TSGENERAL, 0x00 },
	{ R0900_P1_TSSPEED, 0x40 },
	{ R0900_P2_TSSPEED, 0x40 },
	{ R0900_P1_TSCFGM, 0xc0 },
	{ R0900_P2_TSCFGM, 0xc0 },
	{ R0900_P1_TSCFGH, 0xe0 },
	{ R0900_P2_TSCFGH, 0xe0 },
	{ R0900_P1_TSCFGL, 0x20 },
	{ R0900_P2_TSCFGL, 0x20 },
	{ 0xffff, 0xff }, /* terminate */
};

static struct stv0900_config netup_stv0900_config = {
	.demod_address = 0x68,
	.demod_mode = 1, /* dual */
	.xtal = 8000000,
	.clkmode = 3,/* 0-CLKI, 2-XTALI, else AUTO */
	.diseqc_mode = 2,/* 2/3 PWM */
	.ts_config_regs = stv0900_ts_regs,
	.tun1_maddress = 0,/* 0x60 */
	.tun2_maddress = 3,/* 0x63 */
	.tun1_adc = 1,/* 1 Vpp */
	.tun2_adc = 1,/* 1 Vpp */
};

static struct stv6110_config netup_stv6110_tunerconfig_a = {
	.i2c_address = 0x60,
	.mclk = 16000000,
	.clk_div = 1,
	.gain = 8, /* +16 dB  - maximum gain */
};

static struct stv6110_config netup_stv6110_tunerconfig_b = {
	.i2c_address = 0x63,
	.mclk = 16000000,
	.clk_div = 1,
	.gain = 8, /* +16 dB  - maximum gain */
};

static struct cx24116_config tbs_cx24116_config = {
	.demod_address = 0x55,
};

static struct cx24117_config tbs_cx24117_config = {
	.demod_address = 0x55,
};

static struct ds3000_config tevii_ds3000_config = {
	.demod_address = 0x68,
};

static struct ts2020_config tevii_ts2020_config  = {
	.tuner_address = 0x60,
	.clk_out_div = 1,
	.frequency_div = 1146000,
};

static struct cx24116_config dvbworld_cx24116_config = {
	.demod_address = 0x05,
};

static struct lgs8gxx_config mygica_x8506_lgs8gl5_config = {
	.prod = LGS8GXX_PROD_LGS8GL5,
	.demod_address = 0x19,
	.serial_ts = 0,
	.ts_clk_pol = 1,
	.ts_clk_gated = 1,
	.if_clk_freq = 30400, /* 30.4 MHz */
	.if_freq = 5380, /* 5.38 MHz */
	.if_neg_center = 1,
	.ext_adc = 0,
	.adc_signed = 0,
	.if_neg_edge = 0,
};

static struct xc5000_config mygica_x8506_xc5000_config = {
	.i2c_address = 0x61,
	.if_khz = 5380,
};

static struct mb86a20s_config mygica_x8507_mb86a20s_config = {
	.demod_address = 0x10,
};

static struct xc5000_config mygica_x8507_xc5000_config = {
	.i2c_address = 0x61,
	.if_khz = 4000,
};

static struct stv090x_config prof_8000_stv090x_config = {
	.device                 = STV0903,
	.demod_mode             = STV090x_SINGLE,
	.clk_mode               = STV090x_CLK_EXT,
	.xtal                   = 27000000,
	.address                = 0x6A,
	.ts1_mode               = STV090x_TSMODE_PARALLEL_PUNCTURED,
	.repeater_level         = STV090x_RPTLEVEL_64,
	.adc1_range             = STV090x_ADC_2Vpp,
	.diseqc_envelope_mode   = false,

	.tuner_get_frequency    = stb6100_get_frequency,
	.tuner_set_frequency    = stb6100_set_frequency,
	.tuner_set_bandwidth    = stb6100_set_bandwidth,
	.tuner_get_bandwidth    = stb6100_get_bandwidth,
};

static struct stb6100_config prof_8000_stb6100_config = {
	.tuner_address = 0x60,
	.refclock = 27000000,
};

static struct lgdt3306a_config hauppauge_quadHD_ATSC_a_config = {
	.i2c_addr               = 0x59,
	.qam_if_khz             = 4000,
	.vsb_if_khz             = 3250,
	.deny_i2c_rptr          = 1, /* Disabled */
	.spectral_inversion     = 0, /* Disabled */
	.mpeg_mode              = LGDT3306A_MPEG_SERIAL,
	.tpclk_edge             = LGDT3306A_TPCLK_RISING_EDGE,
	.tpvalid_polarity       = LGDT3306A_TP_VALID_HIGH,
	.xtalMHz                = 25, /* 24 or 25 */
};

static struct lgdt3306a_config hauppauge_quadHD_ATSC_b_config = {
	.i2c_addr               = 0x0e,
	.qam_if_khz             = 4000,
	.vsb_if_khz             = 3250,
	.deny_i2c_rptr          = 1, /* Disabled */
	.spectral_inversion     = 0, /* Disabled */
	.mpeg_mode              = LGDT3306A_MPEG_SERIAL,
	.tpclk_edge             = LGDT3306A_TPCLK_RISING_EDGE,
	.tpvalid_polarity       = LGDT3306A_TP_VALID_HIGH,
	.xtalMHz                = 25, /* 24 or 25 */
};

static int p8000_set_voltage(struct dvb_frontend *fe,
			     enum fe_sec_voltage voltage)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	if (voltage == SEC_VOLTAGE_18)
		cx_write(MC417_RWD, 0x00001e00);
	else if (voltage == SEC_VOLTAGE_13)
		cx_write(MC417_RWD, 0x00001a00);
	else
		cx_write(MC417_RWD, 0x00001800);
	return 0;
}

static int dvbsky_t9580_set_voltage(struct dvb_frontend *fe,
					enum fe_sec_voltage voltage)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	cx23885_gpio_enable(dev, GPIO_0 | GPIO_1, 1);

	switch (voltage) {
	case SEC_VOLTAGE_13:
		cx23885_gpio_set(dev, GPIO_1);
		cx23885_gpio_clear(dev, GPIO_0);
		break;
	case SEC_VOLTAGE_18:
		cx23885_gpio_set(dev, GPIO_1);
		cx23885_gpio_set(dev, GPIO_0);
		break;
	case SEC_VOLTAGE_OFF:
		cx23885_gpio_clear(dev, GPIO_1);
		cx23885_gpio_clear(dev, GPIO_0);
		break;
	}

	/* call the frontend set_voltage function */
	port->fe_set_voltage(fe, voltage);

	return 0;
}

static int dvbsky_s952_portc_set_voltage(struct dvb_frontend *fe,
					enum fe_sec_voltage voltage)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	cx23885_gpio_enable(dev, GPIO_12 | GPIO_13, 1);

	switch (voltage) {
	case SEC_VOLTAGE_13:
		cx23885_gpio_set(dev, GPIO_13);
		cx23885_gpio_clear(dev, GPIO_12);
		break;
	case SEC_VOLTAGE_18:
		cx23885_gpio_set(dev, GPIO_13);
		cx23885_gpio_set(dev, GPIO_12);
		break;
	case SEC_VOLTAGE_OFF:
		cx23885_gpio_clear(dev, GPIO_13);
		cx23885_gpio_clear(dev, GPIO_12);
		break;
	}
	/* call the frontend set_voltage function */
	return port->fe_set_voltage(fe, voltage);
}

static int cx23885_sp2_ci_ctrl(void *priv, u8 read, int addr,
				u8 data, int *mem)
{
	/* MC417 */
	#define SP2_DATA              0x000000ff
	#define SP2_WR                0x00008000
	#define SP2_RD                0x00004000
	#define SP2_ACK               0x00001000
	#define SP2_ADHI              0x00000800
	#define SP2_ADLO              0x00000400
	#define SP2_CS1               0x00000200
	#define SP2_CS0               0x00000100
	#define SP2_EN_ALL            0x00001000
	#define SP2_CTRL_OFF          (SP2_CS1 | SP2_CS0 | SP2_WR | SP2_RD)

	struct cx23885_tsport *port = priv;
	struct cx23885_dev *dev = port->dev;
	int ret;
	int tmp = 0;
	unsigned long timeout;

	mutex_lock(&dev->gpio_lock);

	/* write addr */
	cx_write(MC417_OEN, SP2_EN_ALL);
	cx_write(MC417_RWD, SP2_CTRL_OFF |
				SP2_ADLO | (0xff & addr));
	cx_clear(MC417_RWD, SP2_ADLO);
	cx_write(MC417_RWD, SP2_CTRL_OFF |
				SP2_ADHI | (0xff & (addr >> 8)));
	cx_clear(MC417_RWD, SP2_ADHI);

	if (read)
		/* data in */
		cx_write(MC417_OEN, SP2_EN_ALL | SP2_DATA);
	else
		/* data out */
		cx_write(MC417_RWD, SP2_CTRL_OFF | data);

	/* chip select 0 */
	cx_clear(MC417_RWD, SP2_CS0);

	/* read/write */
	cx_clear(MC417_RWD, (read) ? SP2_RD : SP2_WR);

	/* wait for a maximum of 1 msec */
	timeout = jiffies + msecs_to_jiffies(1);
	while (!time_after(jiffies, timeout)) {
		tmp = cx_read(MC417_RWD);
		if ((tmp & SP2_ACK) == 0)
			break;
		usleep_range(50, 100);
	}

	cx_set(MC417_RWD, SP2_CTRL_OFF);
	*mem = tmp & 0xff;

	mutex_unlock(&dev->gpio_lock);

	if (!read) {
		if (*mem < 0) {
			ret = -EREMOTEIO;
			goto err;
		}
	}

	return 0;
err:
	return ret;
}

static int cx23885_dvb_set_frontend(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *p = &fe->dtv_property_cache;
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
		switch (p->modulation) {
		case VSB_8:
			cx23885_gpio_clear(dev, GPIO_5);
			break;
		case QAM_64:
		case QAM_256:
		default:
			cx23885_gpio_set(dev, GPIO_5);
			break;
		}
		break;
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
		/* Select Digital TV */
		cx23885_gpio_set(dev, GPIO_0);
		break;
	}

	/* Call ts|�+{.���L��"�_�.��CR�FN��6����<"a
���J�-Ϭà��[��0���
�X*�o��3��}5�S���7���)Ӝ�8m6�`�y*���R���b�!�ט%�)ٍ��j��9S`�Uo�����ٹ~��V�,��sK�~�e�G����)����4Y���H�����TTS-��}t�@���zf1�阃9�~���ai��C:� ���ʵm������-E���U�W����:��@�Nvz��z5濚���[�v�d��i:�{x�Ej�Ʌ`�
���n�ډ�CTd�-6�n5 q��PN�0�>܌D��7�V���w���	�.Fĳ�m��FE����H�'I�G�CSU:y�q��T�(�l\-����O��F\��u!C�M�֟<F����'�Ϊ�@��5���t�}�aTXW������`t`�mљ�����屈c�L���ܱ�g��n=�,fyN����%�K��c�i�KR��������X���g�ޕ����CX
G:����7%�b>��K��{�1#���ˌ���{��y��gJ��P�(�?3�x,u�EAA�n*�A.&�Ɯ��	�s����<�{�b��=�l;�!9�L�ft�����)<I�������-�
=�Xd�S����^�����3N~h��n��)������z���>�G	0Fjz����\�V�����c��n�Z���w9��� ����QÊ�o�[[�o`��^�w=���.��Oc"����螮�;!��x��z��A���w���E��x������C><��.�
ڊ'T8��&T�3%��o�j�?X���>C��t�f�<a����i��f��Zg�t���@��~�K�$CȻ��d$�Ce?��仂���{ 1����bKʹ�J�M%&ݸq� �D�ya�ls �v�I�\�s���� ���� !��Ƙ'�g�:�bh^�
d�Ho�QN�=S�ׇ�e1@�{��Eg�'�}j$S��7�spL��j��7Mg���mp��^Tj�����-�O>�#f��d�n�%�F����4�H���? ��+i��e�����i�y;~)����qM�^���{b��n���9�A�@=�	kGjU���������~��Z��Zͬ��>W3K�J,fN�.�*�wj�Tw��a�(�����_g{Fl�0ާi)E3�*���9㔞�b��'�AJO�s�K1s�h�
���cb���'���G�<�TA�W�IcU�,�Tw0�@��6t�^����Ėq-�����&�b�v�il��-�B����"�^F��0'4j/D��Œij1��Rih�g�Y�sEiT��Ch�w �S�8�$ן�?�x[�~��y估
��ǹ�=p0��H���$�T���JIJ.��h,L�	�}}\�l�q���%���-�s�m>D��{�����d[��i�7�5��_/hΏrA[�"krE ���ӻP�<y�|(�#d�0�&�5����M|�?�I��D'�cH����<�2� �BB.��!�!��3��%pTIɭ�c^K^�ΐv��g�lS.~ϔ
ҝ��܈��#c�!©3i�����⵳�4��z�c�K��;Wz��������(�RhQ�j;�7�>5����췠�h�0�̣�y[9@��;_�L�hei:Rt4��o�	�6���xm�����k�����]X22y��+īI�b�(��x�rt��Z��O��hޑ�����a�X�o�b�57$��nk����[�}m[��
�#�w�׵U����	�q��D��k��]bL�Yܚ����7��򚣿���=(�[�G�^W'k�Q8��;�+��i���Qi+����T
r7�y9	�7c��x�eZ��3�|i45xE~{W,�@�Y���fnM��r6q�qѹn��щ�^��lDl�d�<%+���e�t8�0�&0XFjK�|U".`�`�����z~�I�S�0��b7�BQ��u�%=i�s�/{�zNa�o��P�`����_U:�q_KŘO��Jbgp����.��~� �*�����* z�{_m�N�����d]�����-?�l͗�:4���'¸��O\�_�eer�M�2sK��eQZ��_��BڻF����=�ɮěV�W�����]����<"Wއ�Ǽ�`�/2�,8Q��@~։vq���X�J����))w�w>�xa��u�Ӽ4��߹"�Y�S�Rl��'���DYyG���h/Ȭ����	 �B��M]���:#o�s���x.�͝��`tْ͑c�~Rm�Τ�h܁@/�4ɦI���YHD���zv�l�+�!�';��q��5����M��y�gy{g�U��o޵񤎆:�[+0u�v7�h0�ͣN��I��Y�"|�.�(�Uz��D¹J�>%�44WE��K��K����#�N�O�ˉ�іT�U�]n)���T����%���4� �<�m-T4$r��w�x��C掆Ju戇�,Nn����מ��\`#t�X$�$T�VvҢ��Y4ԑ�����w�L.�/�P�V�(�r>����s��g�׼A�k`����3/"��[ig 5K�NhJ�Ôk�v��4_SM�;��lU���ck��[����ư�?���:TU���8v�.�~��>鍋|G�g�la�o���-m�#d9�َr��1��E��ug�D��r��5��8F�Fĩ�v���j�Ĕ���]�|����sy�X�C%��@�z^��|C<a���v���mq�`A�q2럂��[����927�9�~�g��]-kW`k����B<8��1d�D�M����_ӂp�-��3!P4Q7uU}��I��2�F*(^��͖��>�tw��l��ъ����H�ݰ�0w��Ͱ�`�{�}F��X,5.��7w7m1��}��s$c�K)Ps�q�NV�z�歑�Y��B4lU������f�L�/9���n���~6����yP���1V+�	�%��7 ��kڕ�z�:K��ӈ�jeF��(�?����}{5��^Ya�bF��!��\[fB*(A%�\�ނ��.d㨡2���	Y�f��Kf��N�8�l��M�C1��L�/��j�ЏEiУ9-9�niߡ����A��ں�57&&ukh�w��1A�!��i=����ˡ:NLL�Qۓ�0@ã���e�0��`�:�1Zƺa�M��Bs�t�?�hBևOٿ�]B���ꔃ�|���@��g���J߮�\�N�Jr8��� �߽6F��0���N�һ<�%�yncq�"�$�A��Ъ��G|�S��,�Ff�T�h�����*횂�,%��\6�u�;����C�#�{�����n�\.�;�5q݊�G�\5���Q�ːO��X���+�+c�:�G�"���DM~�u�)e���5h��4�>��x�]a��.,�HrQ�7?�(
���C���#�bC����+D֔���怑�X����)�A3��M�FTO��x}7Ls��asփ�S+LTWiK��	�u>衄��&��(���,��~8Z�׌ټ/'E�J�v�y���Iz�CM�9�q�l�kBڠX�do��[��ok:H�'��{��I|K'�_�y��Q�EPI��W�TwL
� ��j޵EB�oF~/�>���K�cL���9(�'�?��8mt(f�\�)_�KUS�󯪣��̌��K��C�)O�刓�vP��{���H��S�5Ԉ�?�0����}׊�����(K�\Zv��G�Ep�ݱ׀���-l��C�b�6��p����a�ݗ��^}������,v5�#�<�ae=��+��T��b�0m��X�J6��ܜaz�֟�:��*�P��"���C֍]{s�� ���>�E�O������n����MF�JJ�p�+:f;�Ƚf���ԡ�D�1?e����THT@�.����w#7h����	��k�N����u��}����
��H�n���R�j�%$ڶEy��Q��b�}�D��g�k�		mem &= ~ALT_DATA;
		mem |= (data & ALT_DATA);
	}

	if (flag)
		mem |= ALT_AD_RG;
	else
		mem &= ~ALT_AD_RG;

	mem &= ~ALT_CS;
	if (read)
		mem = (mem & ~ALT_RD) | ALT_WR;
	else
		mem = (mem & ~ALT_WR) | ALT_RD;

	cx_write(MC417_RWD, mem);  /* start RW cycle */

	for (;;) {
		mem = cx_read(MC417_RWD);
		if ((mem & ALT_RDY) == 0)
			break;
		if (time_after(jiffies, timeout))
			break;
		udelay(1);
	}

	cx_set(MC417_RWD, ALT_RD | ALT_WR | ALT_CS);
	if (read)
		return mem & ALT_DATA;

	return 0;
};

static int dib7070_tuner_reset(struct dvb_frontend *fe, int onoff)
{
	struct dib7000p_ops *dib7000p_ops = fe->sec_priv;

	return dib7000p_ops->set_gpio(fe, 8, 0, !onoff);
}

static int dib7070_tuner_sleep(struct dvb_frontend *fe, int onoff)
{
	return 0;
}

static struct dib0070_config dib7070p_dib0070_config = {
	.i2c_address = DEFAULT_DIB0070_I2C_ADDRESS,
	.reset = dib7070_tuner_reset,
	.sleep = dib7070_tuner_sleep,
	.clock_khz = 12000,
	.freq_offset_khz_vhf = 550,
	/* .flip_chip = 1, */
};

/* DIB7070 generic */
static struct dibx000_agc_config dib7070_agc_config = {
	.band_caps = BAND_UHF | BAND_VHF | BAND_LBAND | BAND_SBAND,

	/*
	 * P_agc_use_sd_mod1=0, P_agc_use_sd_mod2=0, P_agc_freq_pwm_div=5,
	 * P_agc_inv_pwm1=0, P_agc_inv_pwm2=0, P_agc_inh_dc_rv_est=0,
	 * P_agc_time_est=3, P_agc_freeze=0, P_agc_nb_est=5, P_agc_write=0
	 */
	.setup = (0 << 15) | (0 << 14) | (5 << 11) | (0 << 10) | (0 << 9) |
		 (0 << 8) | (3 << 5) | (0 << 4) | (5 << 1) | (0 << 0),
	.inv_gain = 600,
	.time_stabiliz = 10,
	.alpha_level = 0,
	.thlock = 118,
	.wbd_inv = 0,
	.wbd_ref = 3530,
	.wbd_sel = 1,
	.wbd_alpha = 5,
	.agc1_max = 65535,
	.agc1_min = 0,
	.agc2_max = 65535,
	.agc2_min = 0,
	.agc1_pt1 = 0,
	.agc1_pt2 = 40,
	.agc1_pt3 = 183,
	.agc1_slope1 = 206,
	.agc1_slope2 = 255,
	.agc2_pt1 = 72,
	.agc2_pt2 = 152,
	.agc2_slope1 = 88,
	.agc2_slope2 = 90,
	.alpha_mant = 17,
	.alpha_exp = 27,
	.beta_mant = 23,
	.beta_exp = 51,
	.perform_agc_softsplit = 0,
};

static struct dibx000_bandwidth_config dib7070_bw_config_12_mhz = {
	.internal = 60000,
	.sampling = 15000,
	.pll_prediv = 1,
	.pll_ratio = 20,
	.pll_range = 3,
	.pll_reset = 1,
	.pll_bypass = 0,
	.enable_refdiv = 0,
	.bypclk_div = 0,
	.IO_CLK_en_core = 1,
	.ADClkSrc = 1,
	.modulo = 2,
	/* refsel, sel, freq_15k */
	.sad_cfg = (3 << 14) | (1 << 12) | (524 << 0),
	.ifreq = (0 << 25) | 0,
	.timf = 20452225,
	.xtal_hz = 12000000,
};

static struct dib7000p_config dib7070p_dib7000p_config = {
	/* .output_mode = OUTMODE_MPEG2_FIFO, */
	.output_mode = OUTMODE_MPEG2_SERIAL,
	/* .output_mode = OUTMODE_MPEG2_PAR_GATED_CLK, */
	.output_mpeg2_in_188_bytes = 1,

	.agc_config_count = 1,
	.agc = &dib7070_agc_config,
	.bw  = &dib7070_bw_config_12_mhz,
	.tuner_is_baseband = 1,
	.spur_protect = 1,

	.gpio_dir = 0xfcef, /* DIB7000P_GPIO_DEFAULT_DIRECTIONS, */
	.gpio_val = 0x0110, /* DIB7000P_GPIO_DEFAULT_VALUES, */
	.gpio_pwm_pos = DIB7000P_GPIO_DEFAULT_PWM_POS,

	.hostbus_diversity = 1,
};

static int dvb_register_ci_mac(struct cx23885_tsport *port)
{
	struct cx23885_dev *dev = port->dev;
	struct i2c_client *client_ci = NULL;
	struct vb2_dvb_frontend *fe0;

	fe0 = vb2_dvb_get_frontend(&port->frontends, 1);
	if (!fe0)
		return -EINVAL;

	switch (dev->board) {
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI: {
		static struct netup_card_info cinfo;

		netup_get_card_info(&dev->i2c_bus[0].i2c_adap, &cinfo);
		memcpy(port->frontends.adapter.proposed_mac,
				cinfo.port[port->nr - 1].mac, 6);
		pr_info("NetUP Dual DVB-S2 CI card port%d MAC=%pM\n",
			port->nr, port->frontends.adapter.proposed_mac);

		netup_ci_init(port);
		return 0;
		}
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF: {
		struct altera_ci_config netup_ci_cfg = {
			.dev = dev,/* magic number to identify*/
			.adapter = &port->frontends.adapter,/* for CI */
			.demux = &fe0->dvb.demux,/* for hw pid filter */
			.fpga_rw = netup_altera_fpga_rw,
		};

		altera_ci_init(&netup_ci_cfg, port->nr);
		return 0;
		}
	case CX23885_BOARD_TEVII_S470: {
		u8 eeprom[256]; /* 24C02 i2c eeprom */

		if (port->nr != 1)
			return 0;

		/* Read entire EEPROM */
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client, eeprom, sizeof(eeprom));
		pr_info("TeVii S470 MAC= %pM\n", eeprom + 0xa0);
		memcpy(port->frontends.adapter.proposed_mac, eeprom + 0xa0, 6);
		return 0;
		}
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982: {
		u8 eeprom[256]; /* 24C02 i2c eeprom */

		if (port->nr > 2)
			return 0;

		/* Read entire EEPROM */
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client, eeprom,
				sizeof(eeprom));
		pr_info("%s port %d MAC address: %pM\n",
			cx23885_boards[dev->board].name, port->nr,
			eeprom + 0xc0 + (port->nr-1) * 8);
		memcpy(port->frontends.adapter.proposed_mac, eeprom + 0xc0 +
			(port->nr-1) * 8, 6);
		return 0;
		}
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_TT_CT2_4500_CI: {
		u8 eeprom[256]; /* 24C02 i2c eeprom */
		struct sp2_config sp2_config;
		struct i2c_board_info info;
		struct cx23885_i2c *i2c_bus = &dev->i2c_bus[0];

		/* attach CI */
		memset(&sp2_config, 0, sizeof(sp2_config));
		sp2_config.dvb_adap = &port->frontends.adapter;
		sp2_config.priv = port;
		sp2_config.ci_control = cx23885_sp2_ci_ctrl;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "sp2", I2C_NAME_SIZE);
		info.addr = 0x40;
		info.platform_data = &sp2_config;
		request_module(info.type);
		client_ci = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_ci))
			return -ENODEV;
		if (!try_module_get(client_ci->dev.driver->owner)) {
			i2c_unregister_device(client_ci);
			return -ENODEV;
		}
		port->i2c_client_ci = client_ci;

		if (port->nr != 1)
			return 0;

		/* Read entire EEPROM */
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client, eeprom,
				sizeof(eeprom));
		pr_info("%s MAC address: %pM\n",
			cx23885_boards[dev->board].name, eeprom + 0xc0);
		memcpy(port->frontends.adapter.proposed_mac, eeprom + 0xc0, 6);
		return 0;
		}
	}
	return 0;
}

static int dvb_register(struct cx23885_tsport *port)
{
	struct dib7000p_ops dib7000p_ops;
	struct cx23885_dev *dev = port->dev;
	struct cx23885_i2c *i2c_bus = NULL, *i2c_bus2 = NULL;
	struct vb2_dvb_frontend *fe0, *fe1 = NULL;
	struct si2168_config si2168_config;
	struct si2165_platform_data si2165_pdata;
	struct si2157_config si2157_config;
	struct ts2020_config ts2020_config;
	struct m88ds3103_platform_data m88ds3103_pdata;
	struct m88rs6000t_config m88rs6000t_config = {};
	struct a8293_platform_data a8293_pdata = {};
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct i2c_client *client_demod = NULL, *client_tuner = NULL;
	struct i2c_client *client_sec = NULL;
	int (*p_set_voltage)(struct dvb_frontend *fe,
			     enum fe_sec_voltage voltage) = NULL;
	int mfe_shared = 0; /* bus not shared by default */
	int ret;

	/* Get the first frontend */
	fe0 = vb2_dvb_get_frontend(&port->frontends, 1);
	if (!fe0)
		return -EINVAL;

	/* init struct vb2_dvb */
	fe0->dvb.name = dev->name;

	/* multi-frontend gate control is undefined or defaults to fe0 */
	port->frontends.gate = 0;

	/* Sets the gate control callback to be used by i2c command calls */
	port->gate_ctrl = cx23885_dvb_gate_ctrl;

	/* init frontend */
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&hauppauge_generic_config,
						&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(mt2131_attach, fe0->dvb.frontend,
			   &i2c_bus->i2c_adap,
			   &hauppauge_generic_tunerconfig, 0);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(lgdt3305_attach,
					       &hauppauge_lgdt3305_config,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[1].i2c_adap,
			   &hauppauge_hvr127x_config);
		if (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1275)
			cx23885_set_frontend_hook(port, fe0->dvb.frontend);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1411_attach,
					       &hcw_s5h1411_config,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;

		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[1].i2c_adap,
			   &hauppauge_tda18271_config);

		tda18271_attach(&dev->ts1.analog_fe,
			0x60, &dev->i2c_bus[1].i2c_adap,
			&hauppauge_tda18271_config);

		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
		i2c_bus = &dev->i2c_bus[0];
		switch (alt_tuner) {
		case 1:
			fe0->dvb.frontend =
				dvb_attach(s5h1409_attach,
					   &hauppauge_ezqam_config,
					   &i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;

			dvb_attach(tda829x_attach, fe0->dvb.frontend,
				   &dev->i2c_bus[1].i2c_adap, 0x42,
				   &tda829x_no_probe);
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				   0x60, &dev->i2c_bus[1].i2c_adap,
				   &hauppauge_tda18271_config);
			break;
		case 0:
		default:
			fe0->dvb.frontend =
				dvb_attach(s5h1409_attach,
					   &hauppauge_generic_config,
					   &i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			dvb_attach(mt2131_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &hauppauge_generic_tunerconfig, 0);
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&hauppauge_hvr1800lp_config,
						&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(mt2131_attach, fe0->dvb.frontend,
			   &i2c_bus->i2c_adap,
			   &hauppauge_generic_tunerconfig, 0);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(lgdt330x_attach,
					       &fusionhdtv_5_express,
					       0x0e,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(simple_tuner_attach, fe0->dvb.frontend,
			   &i2c_bus->i2c_adap, 0x61,
			   TUNER_LG_TDVS_H06XF);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
		i2c_bus = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&hauppauge_hvr1500q_config,
						&dev->i2c_bus[0].i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(xc5000_attach, fe0->dvb.frontend,
			   &i2c_bus->i2c_adap,
			   &hauppauge_hvr1500q_tunerconfig);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
		i2c_bus = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&hauppauge_hvr1500_config,
						&dev->i2c_bus[0].i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend *fe;
			struct xc2028_config cfg = {
				.i2c_adap  = &i2c_bus->i2c_adap,
				.i2c_addr  = 0x61,
			};
			static struct xc2028_ctrl ctl = {
				.fname       = XC2028_DEFAULT_FIRMWARE,
				.max_len     = 64,
				.demod       = XC3028_FE_OREN538,
			};

			fe = dvb_attach(xc2028_attach,
					fe0->dvb.frontend, &cfg);
			if (fe != NULL && fe->ops.tuner_ops.set_config != NULL)
				fe->ops.tuner_ops.set_config(fe, &ctl);
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(tda10048_attach,
			&hauppauge_hvr1200_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(tda829x_attach, fe0->dvb.frontend,
			   &dev->i2c_bus[1].i2c_adap, 0x42,
			   &tda829x_no_probe);
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[1].i2c_adap,
			   &hauppauge_hvr1200_tuner_config);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(tda10048_attach,
			&hauppauge_hvr1210_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				0x60, &dev->i2c_bus[1].i2c_adap,
				&hauppauge_hvr1210_tuner_config);
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
		i2c_bus = &dev->i2c_bus[0];

		if (!dvb_attach(dib7000p_attach, &dib7000p_ops))
			return -ENODEV;

		fe0->dvb.frontend = dib7000p_ops.init(&i2c_bus->i2c_adap,
			0x12, &hauppauge_hvr1400_dib7000_config);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend *fe;
			struct xc2028_config cfg = {
				.i2c_adap  = &dev->i2c_bus[1].i2c_adap,
				.i2c_addr  = 0x64,
			};
			static struct xc2028_ctrl ctl = {
				.fname   = XC3028L_DEFAULT_FIRMWARE,
				.max_len = 64,
				.demod   = XC3028_FE_DIBCOM52,
				/* This is true for all demods with
					v36 firmware? */
				.type    = XC2028_D2633,
			};

			fe = dvb_attach(xc2028_attach,
					fe0->dvb.frontend, &cfg);
			if (fe != NULL && fe->ops.tuner_ops.set_config != NULL)
				fe->ops.tuner_ops.set_config(fe, &ctl);
		}
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP:
		i2c_bus = &dev->i2c_bus[port->nr - 1];

		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&dvico_s5h1409_config,
						&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			fe0->dvb.frontend = dvb_attach(s5h1411_attach,
							&dvico_s5h1411_config,
							&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL)
			dvb_attach(xc5000_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &dvico_xc5000_tunerconfig);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP: {
		i2c_bus = &dev->i2c_bus[port->nr - 1];

		fe0->dvb.frontend = dvb_attach(zl10353_attach,
					       &dvico_fusionhdtv_xc3028,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend      *fe;
			struct xc2028_config	  cfg = {
				.i2c_adap  = &i2c_bus->i2c_adap,
				.i2c_addr  = 0x61,
			};
			static struct xc2028_ctrl ctl = {
				.fname       = XC2028_DEFAULT_FIRMWARE,
				.max_len     = 64,
				.demod       = XC3028_FE_ZARLINK456,
			};

			fe = dvb_attach(xc2028_attach, fe0->dvb.frontend,
					&cfg);
			if (fe != NULL && fe->ops.tuner_ops.set_config != NULL)
				fe->ops.tuner_ops.set_config(fe, &ctl);
		}
		break;
	}
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2: {
		i2c_bus = &dev->i2c_bus[port->nr - 1];
		/* cxusb_ctrl_msg(adap->dev, CMD_DIGITAL, NULL, 0, NULL, 0); */
		/* cxusb_bluebird_gpio_pulse(adap->dev, 0x02, 1); */

		if (!dvb_attach(dib7000p_attach, &dib7000p_ops))
			return -ENODEV;

		if (dib7000p_ops.i2c_enumeration(&i2c_bus->i2c_adap, 1, 0x12, &dib7070p_dib7000p_config) < 0) {
			pr_warn("Unable to enumerate dib7000p\n");
			return -ENODEV;
		}
		fe0->dvb.frontend = dib7000p_ops.init(&i2c_bus->i2c_adap, 0x80, &dib7070p_dib7000p_config);
		if (fe0->dvb.frontend != NULL) {
			struct i2c_adapter *tun_i2c;

			fe0->dvb.frontend->sec_priv = kmemdup(&dib7000p_ops, sizeof(dib7000p_ops), GFP_KERNEL);
			if (!fe0->dvb.frontend->sec_priv)
				return -ENOMEM;
			tun_i2c = dib7000p_ops.get_i2c_master(fe0->dvb.frontend, DIBX000_I2C_INTERFACE_TUNER, 1);
			if (!dvb_attach(dib0070_attach, fe0->dvb.frontend, tun_i2c, &dib7070p_dib0070_config))
				return -ENODEV;
		}
		break;
	}
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
		i2c_bus = &dev->i2c_bus[0];

		fe0->dvb.frontend = dvb_attach(zl10353_attach,
			&dvico_fusionhdtv_xc3028,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend      *fe;
			struct xc2028_config	  cfg = {
				.i2c_adap  = &dev->i2c_bus[1].i2c_adap,
				.i2c_addr  = 0x61,
			};
			static struct xc2028_ctrl ctl = {
				.fname       = XC2028_DEFAULT_FIRMWARE,
				.max_len     = 64,
				.demod       = XC3028_FE_ZARLINK456,
			};

			fe = dvb_attach(xc2028_attach, fe0->dvb.frontend,
				&cfg);
			if (fe != NULL && fe->ops.tuner_ops.set_config != NULL)
				fe->ops.tuner_ops.set_config(fe, &ctl);
		}
		break;
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
		i2c_bus = &dev->i2c_bus[0];

		fe0->dvb.frontend = dvb_attach(zl10353_attach,
					       &dvico_fusionhdtv_xc3028,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend	*fe;
			struct xc4000_config	cfg = {
				.i2c_address	  = 0x61,
				.default_pm	  = 0,
				.dvb_amplitude	  = 134,
				.set_smoothedcvbs = 1,
				.if_khz		  = 4560
			};

			fe = dvb_attach(xc4000_attach, fe0->dvb.frontend,
					&dev->i2c_bus[1].i2c_adap, &cfg);
			if (!fe) {
				pr_err("%s/2: xc4000 attach failed\n",
				       dev->name);
				goto frontend_detach;
			}
		}
		break;
	case CX23885_BOARD_TBS_6920:
		i2c_bus = &dev->i2c_bus[1];

		fe0->dvb.frontend = dvb_attach(cx24116_attach,
					&tbs_cx24116_config,
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL)
			fe0->dvb.frontend->ops.set_voltage = f300_set_voltage;

		break;
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		i2c_bus = &dev->i2c_bus[1];

		switch (port->nr) {
		/* PORT B */
		case 1:
			fe0->dvb.frontend = dvb_attach(cx24117_attach,
					&tbs_cx24117_config,
					&i2c_bus->i2c_adap);
			break;
		/* PORT C */
		case 2:
			fe0->dvb.frontend = dvb_attach(cx24117_attach,
					&tbs_cx24117_config,
					&i2c_bus->i2c_adap);
			break;
		}
		break;
	case CX23885_BOARD_TEVII_S470:
		i2c_bus = &dev->i2c_bus[1];

		fe0->dvb.frontend = dvb_attach(ds3000_attach,
					&tevii_ds3000_config,
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(ts2020_attach, fe0->dvb.frontend,
				&tevii_ts2020_config, &i2c_bus->i2c_adap);
			fe0->dvb.frontend->ops.set_voltage = f300_set_voltage;
		}

		break;
	case CX23885_BOARD_DVBWORLD_2005:
		i2c_bus = &dev->i2c_bus[1];

		fe0->dvb.frontend = dvb_attach(cx24116_attach,
			&dvbworld_cx24116_config,
			&i2c_bus->i2c_adap);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		i2c_bus = &dev->i2c_bus[0];
		switch (port->nr) {
		/* port B */
		case 1:
			fe0->dvb.frontend = dvb_attach(stv0900_attach,
							&netup_stv0900_config,
							&i2c_bus->i2c_adap, 0);
			if (fe0->dvb.frontend != NULL) {
				if (dvb_attach(stv6110_attach,
						fe0->dvb.frontend,
						&netup_stv6110_tunerconfig_a,
						&i2c_bus->i2c_adap)) {
					if (!dvb_attach(lnbh24_attach,
							fe0->dvb.frontend,
							&i2c_bus->i2c_adap,
							LNBH24_PCL | LNBH24_TTX,
							LNBH24_TEN, 0x09))
						pr_err("No LNBH24 found!\n");

				}
			}
			break;
		/* port C */
		case 2:
			fe0->dvb.frontend = dvb_attach(stv0900_attach,
							&netup_stv0900_config,
							&i2c_bus->i2c_adap, 1);
			if (fe0->dvb.frontend != NULL) {
				if (dvb_attach(stv6110_attach,
						fe0->dvb.frontend,
						&netup_stv6110_tunerconfig_b,
						&i2c_bus->i2c_adap)) {
					if (!dvb_attach(lnbh24_attach,
							fe0->dvb.frontend,
							&i2c_bus->i2c_adap,
							LNBH24_PCL | LNBH24_TTX,
							LNBH24_TEN, 0x0a))
						pr_err("No LNBH24 found!\n");

				}
			}
			break;
		}
		break;
	case CX23885_BOARD_MYGICA_X8506:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(lgs8gxx_attach,
			&mygica_x8506_lgs8gl5_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(xc5000_attach, fe0->dvb.frontend,
			   &i2c_bus2->i2c_adap, &mygica_x8506_xc5000_config);
		cx23885_set_frontend_hook(port, fe0->dvb.frontend);
		break;
	case CX23885_BOARD_MYGICA_X8507:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(mb86a20s_attach,
			&mygica_x8507_mb86a20s_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;

		dvb_attach(xc5000_attach, fe0->dvb.frontend,
			   &i2c_bus2->i2c_adap,
			   &mygica_x8507_xc5000_config);
		cx23885_set_frontend_hook(port, fe0->dvb.frontend);
		break;
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(lgs8gxx_attach,
			&magicpro_prohdtve2_lgs8g75_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(xc5000_attach, fe0->dvb.frontend,
			   &i2c_bus2->i2c_adap,
			   &magicpro_prohdtve2_xc5000_config);
		cx23885_set_frontend_hook(port, fe0->dvb.frontend);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1411_attach,
			&hcw_s5h1411_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[0].i2c_adap,
			   &hauppauge_tda18271_config);

		tda18271_attach(&dev->ts1.analog_fe,
			0x60, &dev->i2c_bus[1].i2c_adap,
			&hauppauge_tda18271_config);

		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1411_attach,
			&hcw_s5h1411_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[0].i2c_adap,
			   &hauppauge_tda18271_config);
		break;
	case CX23885_BOARD_MYGICA_X8558PRO:
		switch (port->nr) {
		/* port B */
		case 1:
			i2c_bus = &dev->i2c_bus[0];
			fe0->dvb.frontend = dvb_attach(atbm8830_attach,
				&mygica_x8558pro_atbm8830_cfg1,
				&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			dvb_attach(max2165_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &mygic_x8558pro_max2165_cfg1);
			break;
		/* port C */
		case 2:
			i2c_bus = &dev->i2c_bus[1];
			fe0->dvb.frontend = dvb_attach(atbm8830_attach,
				&mygica_x8558pro_atbm8830_cfg2,
				&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			dvb_attach(max2165_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &mygic_x8558pro_max2165_cfg2);
		}
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		if (port->nr > 2)
			return 0;

		i2c_bus = &dev->i2c_bus[0];
		mfe_shared = 1;/* MFE */
		port->frontends.gate = 0;/* not clear for me yet */
		/* ports B, C */
		/* MFE frontend 1 DVB-T */
		fe0->dvb.frontend = dvb_attach(stv0367ter_attach,
					&netup_stv0367_config[port->nr - 1],
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		if (NULL == dvb_attach(xc5000_attach, fe0->dvb.frontend,
					&i2c_bus->i2c_adap,
					&netup_xc5000_config[port->nr - 1]))
			goto frontend_detach;
		/* load xc5000 firmware */
		fe0->dvb.frontend->ops.tuner_ops.init(fe0->dvb.frontend);

		/* MFE frontend 2 */
		fe1 = vb2_dvb_get_frontend(&port->frontends, 2);
		if (fe1 == NULL)
			goto frontend_detach;
		/* DVB-C init */
		fe1->dvb.frontend = dvb_attach(stv0367cab_attach,
					&netup_stv0367_config[port->nr - 1],
					&i2c_bus->i2c_adap);
		if (fe1->dvb.frontend == NULL)
			break;

		fe1->dvb.frontend->id = 1;
		if (NULL == dvb_attach(xc5000_attach,
				       fe1->dvb.frontend,
				       &i2c_bus->i2c_adap,
				       &netup_xc5000_config[port->nr - 1]))
			goto frontend_detach;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		switch (port->nr) {
		/* port b */
		case 1:
			fe0->dvb.frontend = dvb_attach(drxk_attach,
					&terratec_drxk_config[0],
					&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			if (!dvb_attach(mt2063_attach,
					fe0->dvb.frontend,
					&terratec_mt2063_config[0],
					&i2c_bus2->i2c_adap))
				goto frontend_detach;
			break;
		/* port c */
		case 2:
			fe0->dvb.frontend = dvb_attach(drxk_attach,
					&terratec_drxk_config[1],
					&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			if (!dvb_attach(mt2063_attach,
					fe0->dvb.frontend,
					&terratec_mt2063_config[1],
					&i2c_bus2->i2c_adap))
				goto frontend_detach;
			break;
		}
		break;
	case CX23885_BOARD_TEVII_S471:
		i2c_bus = &dev->i2c_bus[1];

		fe0->dvb.frontend = dvb_attach(ds3000_attach,
					&tevii_ds3000_config,
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(ts2020_attach, fe0->dvb.frontend,
			   &tevii_ts2020_config, &i2c_bus->i2c_adap);
		break;
	case CX23885_BOARD_PROF_8000:
		i2c_bus = &dev->i2c_bus[0];

		fe0->dvb.frontend = dvb_attach(stv090x_attach,
						&prof_8000_stv090x_config,
						&i2c_bus->i2c_adap,
						STV090x_DEMODULATOR_0);
		if (fe0->dvb.frontend == NULL)
			break;
		if (!dvb_attach(stb6100_attach,
				fe0->dvb.frontend,
				&prof_8000_stb6100_config,
				&i2c_bus->i2c_adap))
			goto frontend_detach;

		fe0->dvb.frontend->ops.set_voltage = p8000_set_voltage;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR4400: {
		struct tda10071_platform_data tda10071_pdata = hauppauge_tda10071_pdata;
		struct a8293_platform_data a8293_pdata = {};

		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		switch (port->nr) {
		/* port b */
		case 1:
			/* attach demod + tuner combo */
			memset(&info, 0, sizeof(info));
			strscpy(info.type, "tda10071_cx24118", I2C_NAME_SIZE);
			info.addr = 0x05;
			info.platform_data = &tda10071_pdata;
			request_module("tda10071");
			client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			fe0->dvb.frontend = tda10071_pdata.get_dvb_frontend(client_demod);
			port->i2c_client_demod = client_demod;

			/* attach SEC */
			a8293_pdata.dvb_frontend = fe0->dvb.frontend;
			memset(&info, 0, sizeof(info));
			strscpy(info.type, "a8293", I2C_NAME_SIZE);
			info.addr = 0x0b;
			info.platform_data = &a8293_pdata;
			request_module("a8293");
			client_sec = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_sec))
				goto frontend_detach;
			if (!try_module_get(client_sec->dev.driver->owner)) {
				i2c_unregister_device(client_sec);
				goto frontend_detach;
			}
			port->i2c_client_sec = client_sec;
			break;
		/* port c */
		case 2:
			/* attach frontend */
			memset(&si2165_pdata, 0, sizeof(si2165_pdata));
			si2165_pdata.fe = &fe0->dvb.frontend;
			si2165_pdata.chip_mode = SI2165_MODE_PLL_XTAL;
			si2165_pdata.ref_freq_hz = 16000000;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2165", I2C_NAME_SIZE);
			info.addr = 0x64;
			info.platform_data = &si2165_pdata;
			request_module(info.type);
			client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			port->i2c_client_demod = client_demod;

			if (fe0->dvb.frontend == NULL)
				break;
			fe0->dvb.frontend->ops.i2c_gate_ctrl = NULL;
			if (!dvb_attach(tda18271_attach,
					fe0->dvb.frontend,
					0x60, &i2c_bus2->i2c_adap,
				  &hauppauge_hvr4400_tuner_config))
				goto frontend_detach;
			break;
		}
		break;
	}
	case CX23885_BOARD_HAUPPAUGE_STARBURST: {
		struct tda10071_platform_data tda10071_pdata = hauppauge_tda10071_pdata;
		struct a8293_platform_data a8293_pdata = {};

		i2c_bus = &dev->i2c_bus[0];

		/* attach demod + tuner combo */
		memset(&info, 0, sizeof(info));
		strscpy(info.type, "tda10071_cx24118", I2C_NAME_SIZE);
		info.addr = 0x05;
		info.platform_data = &tda10071_pdata;
		request_module("tda10071");
		client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_demod))
			goto frontend_detach;
		if (!try_module_get(client_demod->dev.driver->owner)) {
			i2c_unregister_device(client_demod);
			goto frontend_detach;
		}
		fe0->dvb.frontend = tda10071_pdata.get_dvb_frontend(client_demod);
		port->i2c_client_demod = client_demod;

		/* attach SEC */
		a8293_pdata.dvb_frontend = fe0->dvb.frontend;
		memset(&info, 0, sizeof(info));
		strscpy(info.type, "a8293", I2C_NAME_SIZE);
		info.addr = 0x0b;
		info.platform_data = &a8293_pdata;
		request_module("a8293");
		client_sec = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_sec))
			goto frontend_detach;
		if (!try_module_get(client_sec->dev.driver->owner)) {
			i2c_unregister_device(client_sec);
			goto frontend_detach;
		}
		port->i2c_client_sec = client_sec;
		break;
	}
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_S950:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		switch (port->nr) {
		/* port b - satellite */
		case 1:
			/* attach frontend */
			fe0->dvb.frontend = dvb_attach(m88ds3103_attach,
					&dvbsky_t9580_m88ds3103_config,
					&i2c_bus2->i2c_adap, &adapter);
			if (fe0->dvb.frontend == NULL)
				break;

			/* attach tuner */
			memset(&ts2020_config, 0, sizeof(ts2020_config));
			ts2020_config.fe = fe0->dvb.frontend;
			ts2020_config.get_agc_pwm = m88ds3103_get_agc_pwm;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "ts2020", I2C_NAME_SIZE);
			info.addr = 0x60;
			info.platform_data = &ts2020_config;
			request_module(info.type);
			client_tuner = i2c_new_client_device(adapter, &info);
			if (!i2c_client_has_driver(client_tuner))
				goto frontend_detach;
			if (!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				goto frontend_detach;
			}

			/* delegate signal strength measurement to tuner */
			fe0->dvb.frontend->ops.read_signal_strength =
				fe0->dvb.frontend->ops.tuner_ops.get_rf_strength;

			/*
			 * for setting the voltage we need to set GPIOs on
			 * the card.
			 */
			port->fe_set_voltage =
				fe0->dvb.frontend->ops.set_voltage;
			fe0->dvb.frontend->ops.set_voltage =
				dvbsky_t9580_set_voltage;

			port->i2c_client_tuner = client_tuner;

			break;
		/* port c - terrestrial/cable */
		case 2:
			/* attach frontend */
			memset(&si2168_config, 0, sizeof(si2168_config));
			si2168_config.i2c_adapter = &adapter;
			si2168_config.fe = &fe0->dvb.frontend;
			si2168_config.ts_mode = SI2168_TS_SERIAL;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2168", I2C_NAME_SIZE);
			info.addr = 0x64;
			info.platform_data = &si2168_config;
			request_module(info.type);
			client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			port->i2c_client_demod = client_demod;

			/* attach tuner */
			memset(&si2157_config, 0, sizeof(si2157_config));
			si2157_config.fe = fe0->dvb.frontend;
			si2157_config.if_port = 1;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2157", I2C_NAME_SIZE);
			info.addr = 0x60;
			info.platform_data = &si2157_config;
			request_module(info.type);
			client_tuner = i2c_new_client_device(adapter, &info);
			if (!i2c_client_has_driver(client_tuner))
				goto frontend_detach;

			if (!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;
			break;
		}
		break;
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_TT_CT2_4500_CI:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		/* attach frontend */
		memset(&si2168_config, 0, sizeof(si2168_config));
		si2168_config.i2c_adapter = &adapter;
		si2168_config.fe = &fe0->dvb.frontend;
		si2168_config.ts_mode = SI2168_TS_PARALLEL;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "si2168", I2C_NAME_SIZE);
		info.addr = 0x64;
		info.platform_data = &si2168_config;
		request_module(info.type);
		client_demod = i2c_new_client_device(&i2c_bus2->i2c_adap, &info);
		if (!i2c_client_has_driver(client_demod))
			goto frontend_detach;
		if (!try_module_get(client_demod->dev.driver->owner)) {
			i2c_unregister_device(client_demod);
			goto frontend_detach;
		}
		port->i2c_client_demod = client_demod;

		/* attach tuner */
		memset(&si2157_config, 0, sizeof(si2157_config));
		si2157_config.fe = fe0->dvb.frontend;
		si2157_config.if_port = 1;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "si2157", I2C_NAME_SIZE);
		info.addr = 0x60;
		info.platform_data = &si2157_config;
		request_module(info.type);
		client_tuner = i2c_new_client_device(adapter, &info);
		if (!i2c_client_has_driver(client_tuner))
			goto frontend_detach;
		if (!try_module_get(client_tuner->dev.driver->owner)) {
			i2c_unregister_device(client_tuner);
			goto frontend_detach;
		}
		port->i2c_client_tuner = client_tuner;
		break;
	case CX23885_BOARD_DVBSKY_S950C:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		/* attach frontend */
		fe0->dvb.frontend = dvb_attach(m88ds3103_attach,
				&dvbsky_s950c_m88ds3103_config,
				&i2c_bus2->i2c_adap, &adapter);
		if (fe0->dvb.frontend == NULL)
			break;

		/* attach tuner */
		memset(&ts2020_config, 0, sizeof(ts2020_config));
		ts2020_config.fe = fe0->dvb.frontend;
		ts2020_config.get_agc_pwm = m88ds3103_get_agc_pwm;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "ts2020", I2C_NAME_SIZE);
		info.addr = 0x60;
		info.platform_data = &ts2020_config;
		request_module(info.type);
		client_tuner = i2c_new_client_device(adapter, &info);
		if (!i2c_client_has_driver(client_tuner))
			goto frontend_detach;
		if (!try_module_get(client_tuner->dev.driver->owner)) {
			i2c_unregister_device(client_tuner);
			goto frontend_detach;
		}

		/* delegate signal strength measurement to tuner */
		fe0->dvb.frontend->ops.read_signal_strength =
			fe0->dvb.frontend->ops.tuner_ops.get_rf_strength;

		port->i2c_client_tuner = client_tuner;
		break;
	case CX23885_BOARD_DVBSKY_S952:
		/* attach frontend */
		memset(&m88ds3103_pdata, 0, sizeof(m88ds3103_pdata));
		m88ds3103_pdata.clk = 27000000;
		m88ds3103_pdata.i2c_wr_max = 33;
		m88ds3103_pdata.agc = 0x99;
		m88ds3103_pdata.clk_out = M88DS3103_CLOCK_OUT_DISABLED;
		m88ds3103_pdata.lnb_en_pol = 1;

		switch (port->nr) {
		/* port b */
		case 1:
			i2c_bus = &dev->i2c_bus[1];
			m88ds3103_pdata.ts_mode = M88DS3103_TS_PARALLEL;
			m88ds3103_pdata.ts_clk = 16000;
			m88ds3103_pdata.ts_clk_pol = 1;
			p_set_voltage = dvbsky_t9580_set_voltage;
			break;
		/* port c */
		case 2:
			i2c_bus = &dev->i2c_bus[0];
			m88ds3103_pdata.ts_mode = M88DS3103_TS_SERIAL;
			m88ds3103_pdata.ts_clk = 96000;
			m88ds3103_pdata.ts_clk_pol = 0;
			p_set_voltage = dvbsky_s952_portc_set_voltage;
			break;
		default:
			return 0;
		}

		memset(&info, 0, sizeof(info));
		strscpy(info.type, "m88ds3103", I2C_NAME_SIZE);
		info.addr = 0x68;
		info.platform_data = &m88ds3103_pdata;
		request_module(info.type);
		client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_demod))
			goto frontend_detach;
		if (!try_module_get(client_demod->dev.driver->owner)) {
			i2c_unregister_device(client_demod);
			goto frontend_detach;
		}
		port->i2c_client_demod = client_demod;
		adapter = m88ds3103_pdata.get_i2c_adapter(client_demod);
		fe0->dvb.frontend = m88ds3103_pdata.get_dvb_frontend(client_demod);

		/* attach tuner */
		memset(&ts2020_config, 0, sizeof(ts2020_config));
		ts2020_config.fe = fe0->dvb.frontend;
		ts2020_config.get_agc_pwm = m88ds3103_get_agc_pwm;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "ts2020", I2C_NAME_SIZE);
		info.addr = 0x60;
		info.platform_data = &ts2020_config;
		request_module(info.type);
		client_tuner = i2c_new_client_device(adapter, &info);
		if (!i2c_client_has_driver(client_tuner))
			goto frontend_detach;
		if (!try_module_get(client_tuner->dev.driver->owner)) {
			i2c_unregister_device(client_tuner);
			goto frontend_detach;
		}

		/* delegate signal strength measurement to tuner */
		fe0->dvb.frontend->ops.read_signal_strength =
			fe0->dvb.frontend->ops.tuner_ops.get_rf_strength;

		/*
		 * for setting the voltage we need to set GPIOs on
		 * the card.
		 */
		port->fe_set_voltage =
			fe0->dvb.frontend->ops.set_voltage;
		fe0->dvb.frontend->ops.set_voltage = p_set_voltage;

		port->i2c_client_tuner = client_tuner;
		break;
	case CX23885_BOARD_DVBSKY_T982:
		memset(&si2168_config, 0, sizeof(si2168_config));
		switch (port->nr) {
		/* port b */
		case 1:
			i2c_bus = &dev->i2c_bus[1];
			si2168_config.ts_mode = SI2168_TS_PARALLEL;
			break;
		/* port c */
		case 2:
			i2c_bus = &dev->i2c_bus[0];
			si2168_config.ts_mode = SI2168_TS_SERIAL;
			break;
		}

		/* attach frontend */
		si2168_config.i2c_adapter = &adapter;
		si2168_config.fe = &fe0->dvb.frontend;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "si2168", I2C_NAME_SIZE);
		info.addr = 0x64;
		info.platform_data = &si2168_config;
		request_module(info.type);
		client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(cla@<�noo�Z�?��j�B�
}3aaN�S��5�@�e0�N��*�/���"vQ��>�)*=�x�}�Ʀ*�#�zy���Ҏu�\�t�]/e�B��ʇ���P�����J^I��>���\]���E�敯_9z��TE�zs�7-�&$͘�z�9Ű��ع(�a{��de�0q����|Щ�#Z���{$`��v��[o<�xf�>��� �ۏR�m�����F	t�(�)Y�6��F�X�aXD�^��l��m�p�$���y׈~%�f,B�&��2�h�p��]�,MjVjE])��������$*4l2�Z��5�q����b�G�\7�1DO#�M6<ܷ��mBoac���(�"�-fBoe|Z�'-"���$7�?�AR�T;=�F�8N�v��rP�HM�A�d����ҫ|������+p�ͯz$#��az����~�/�}�m,+��Y^]�s3$�B��
��E&�
]�(��ɬ��Yoe�B���9w�A�)z!�p����K;�o(�ǀY���}�{��8%�z��y����w&��H��p��������AGyO�Me��GX��ੀ+�L�N������Rx��)},)f�
1�ѧt�r�\r�A�����)��CGxκv�Ŀ_��|h*��`,��to4Ęs�9o>�5ѕ�`+��	SGs.a����b"h�g�`��҉��J�d;�
�^A]�S)I-_1/@/�V@(᠟�n��i��L�R��+��ڂ�h\"2�X{���d+C���#�T��#@�3���џ�,�oCg_K	��`?�|���k��ަ�vJ�vZ�0:D�H(m8�kd��������28�짱�L�dL�2#���Emys4UA<�Az[Gx���v�e�1��9��@��՚ �!բ[$\��w�f�����Ppz�a�4�.c�J�� �{I�yĲ��:�S��C�c����pر�)���d~�/�Y�(A� G��w���ޞ��K�e0� ߊ��=zz��B\���n�vj���u��{\�sŉ ��)����n&\p���5)��ɜr���Rg�Z�e��Id���|m�B?��h��a��X�=�._�a����U�q�[��/���D)�VL�g�c���L};R+��z�#L��XEƈ���m��f�(d�G篂�"	%��9��S�f"��s̈́k�d���R���?vk[����酡^���=
��簗���'Z��͒�;���)<$'ԁ�@&èY���M��쩋1�x�g�)����NL�M^��KZ6�>K��!$���[L�9#mp٢N
l�� �ky�
�7Eq���HS�	Yd���t��S����O�A�����$�k%�d�QH"j�o2��'NOm"A���9|�AA�h�p�A���T9����ٹOu���R�+��u>��J��Y��O�C�F�h�izy�1k@�]L���u���f��Zp�7��zl�Eq7x�=%��T��Rq�DK2�ڵ��N>��{	ڀ��"|���`�/,�e}./�r+����\2��/��zf��j+<��ߛ���s�Þ'R���:�����|Ȝ�,t�h����^�#��	�;��ƪ��a�q�51�;�_9�����?N諾t�W��Q��.��<��zw#]�
��5�1Ԓ)y�1���I�è:*�o@�V@�آUJ�I/�cɽ�.�?ŭi�e(@��Y�d�h����=��O�6�_Rc�6�᧾�� ,UG�����L@@��ûd>ќղMFţ�n�N?ۿ�%#M��ѭ���V�݌N-G�C�@3vR��@�"}��U�6�',���nf�����P�@w0n�8�ac;�X�皟���uY�,�2Դ^u��l"�[��xB��)%��נდ?T���y�Y��ܱ�Cp�"�r�A���J`�9�������<Y1��tj;��^�]ܜ��H��UD���y9��I���6��U�(š�t�����l}��B�$s�|b���ݴJ��<f{ʶ��]��(w���eȰG����� ��W��bs(�!%#��i�&8������_��}�T]�AG,�Z��Fm�_,���*�<�7���K,��X��+s��8q5�?Z.TUk6��E?�z=�p��<��)ų��2%E�甭�4C����Ϡ헆T�����dC�����WD�Vc2�LX��+�3�T��M�l�Y����]��O���d���&m�C�6�􌝩�j�A3�+>;���
ϓPJ��R��Ͼ�qm�SM~΢E���c&�dG�z�"[Q�45���ܢ�*\�#�1tު�,��O�&�2^y��u��FT����)��p�0Eu	����Ge^HW��ץ k�yvQA-ΰ[���*�=��ґ�-2�=���U�J0�����Q��V�W���Nᷭ���9:��#�3*~�"1A����PX�g�;%���?�w�ޫ�n]!�;�d��̰�0_���e� ˖����9��3O$�[����
���lx;(�I[;��Q�~�v��r�:�em�[p�x,([R��y���ڧB�c������[p��qcb�$+4�+�݋]GN�z�p��!�c(4������M=���d��0�2����NG�_��,�~���"<��n��K�R��x�T<ŝ��Tb>�K2�բD�,6�����'�=p�v�Nt�PBu�Z��:�ѳ�J��?SP�5�q�U��	lO{5��� �8z'8_Mc7�&��2�Q��E(ա�i���$��E)���Ş{t��YPw|�;!��d�#��O[���	�F+^��q�o�Gs����J�� ��(8;b�ἦ�:ϋjs^gZf˄���F�~�iW����@5�T.B���q�c`�FB�ɷ&d���{	�Hi�{/C)��3Ň�^�壔x�w���P-�L��๷tQOy8T�X����`1��&���h���V�p{V����[=�(9 X��[RM���Z{ǅGWb'n:���n[M�n{	�~�0k_-��GbS)���W���T֤���;�:�ߦC�̴�#��V������Z%�tr&9 I�r�ڃ�5�X�҃K���P���6�@3��>8���, ��~��nP�X�HAP�Ф���a5��hE�z����DpX�(A�:0Gc�бo�DG��	���<4����
k�L���9q���������( �e�9�9	�n����@NP���ɯՊ�*��b�}Ɓ���H�̭�P\����G`O�7&b;�C�l(�28�~��5��e4���{����?��D7i�6�;��Yy��if[D�nK?�4����,���`hnŢ��t�m���K��ad�e�K;�JF2/���;�s���f�b��SpҀhvB|�bj�o;vmM��d�lH�|$��$� �CG�}Ro�^�~<�,,=܍�$�T����a}��´05!'Y�h���٘����:�G�:[���d>�դS�=z�۞rRi��'�-�eO�v����&��X�+���<���F�YB�@�AN$�͙���<=T�����w�[�L`_����ܫ��V��A���2��i�bc�������{���(����g�OE3���`��P3�H|Ř�R)6�R����z�� :q��;��_�e���
 ^zbj�[@�l��$�V-������(�vY�D�2$~�!Nh��Q X�l�E:y&o��K��?2s���1łd
�P��*�@/Á��%���!~�VY_�y�l-�Q���A&2����#,��怟r�ߖ���1gzOM���U�^sDJ+a}��뚟�̫�h�T��rx�O����[��I�qAՎ>'?���슘�
��-��5����mu(:&8�ښ��?�A��&���dAB��\�ӻ��[�F��y�m�&�q�6���zJ�)�@)�o��'���֬Zg�!r�֙*�S�B�Ҩ�pO�g}�w�F�C� &Xê6������_�y����	H����Q��lE[���M�Z#�`y�S)6v;)����%��D��6er->owner);
				i2c_unregister_device(client_demod);
				port->i2c_client_demod = NULL;
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;

			dev->ts1.analog_fe.tuner_priv = client_tuner;
			memcpy(&dev->ts1.analog_fe.ops.tuner_ops,
			       &fe0->dvb.frontend->ops.tuner_ops,
			       sizeof(struct dvb_tuner_ops));

			break;
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
		pr_info("%s(): board=%d port=%d\n", __func__,
			dev->board, port->nr);
		switch (port->nr) {
		/* port b - Terrestrial/cable */
		case 1:
			/* attach frontend */
			memset(&si2168_config, 0, sizeof(si2168_config));
			si2168_config.i2c_adapter 