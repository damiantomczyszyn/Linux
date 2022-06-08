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
�y�{h���9�M�E,�4	�!��� z%��Uؗ�-c ��y�����8������ ��z9����V���C��F��u���_�'F=����uHFu�����V� w�,��`)F�'?�`6��šº��M�/%�j��|�Rp���G>�ľ