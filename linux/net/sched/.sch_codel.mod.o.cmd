us & VID_BC_MSK_SYNC) ||
		(status & VID_BC_MSK_OF)) {

		if (status & VID_BC_MSK_OPC_ERR)
			dprintk(7, " (VID_BC_MSK_OPC_ERR 0x%08x)\n",
				VID_BC_MSK_OPC_ERR);

		if (status & VID_BC_MSK_BAD_PKT)
			dprintk(7, " (VID_BC_MSK_BAD_PKT 0x%08x)\n",
				VID_BC_MSK_BAD_PKT);

		if (status & VID_BC_MSK_SYNC)
			dprintk(7, " (VID_BC_MSK_SYNC    0x%08x)\n",
				VID_BC_MSK_SYNC);

		if (status & VID_BC_MSK_OF)
			dprintk(7, " (VID_BC_MSK_OF      0x%08x)\n",
				VID_BC_MSK_OF);

		pr_err("%s: mpeg risc op code error\n", dev->name);

		cx_clear(port->reg_dma_ctl, port->dma_ctl_val);
		cx23885_sram_channel_dump(dev,
			&dev->sram_channels[port->sram_chno]);

	} else if (status & VID_BC_MSK_RISCI1) {

		dprintk(7, " (RISCI1            0x%08x)\n", VID_BC_MSK_RISCI1);

		spin_lock(&port->slock);
		count = cx_read(port->reg_gpcnt);
		cx23885_wakeup(port, &port->mpegq, count);
		spin_unlock(&port->slock);

	}
	if (status) {
		cx_write(port->reg_ts_int_stat, status);
		handled = 1;
	}

	return handled;
}

static irqreturn_t cx23885_irq(int irq, void *dev_id)
{
	struct cx23885_dev *dev = dev_id;
	struct cx23885_tsport *ts1 = &dev->ts1;
	struct cx23885_tsport *ts2 = &dev->ts2;
	u32 pci_status, pci_mask;
	u32 vida_status, vida_mask;
	u32 audint_status, audint_mask;
	u32 ts1_status, ts1_mask;
	u32 ts2_status, ts2_mask;
	int vida_count = 0, ts1_count = 0, ts2_count = 0, handled = 0;
	int audint_count = 0;
	bool subdev_handled;

	pci_status = cx_read(PCI_INT_STAT);
	pci_mask = cx23885_irq_get_mask(dev);
	if ((pci_status & pci_mask) == 0) {
		dprintk(7, "pci_status: 0x%08x  pci_mask: 0x%08x\n",
			pci_status, pci_mask);
		goto out;
	}

	vida_status = cx_read(VID_A_INT_STAT);
	vida_mask = cx_read(VID_A_INT_MSK);
	audint_status = cx_read(AUDIO_INT_INT_STAT);
	audint_mask = cx_read(AUDIO_INT_INT_MSK);
	ts1_status = cx_read(VID_B_INT_STAT);
	ts1_mask = cx_read(VID_B_INT_MSK);
	ts2_status = cx_read(VID_C_INT_STAT);
	ts2_mask = cx_read(VID_C_INT_MSK);

	if (((pci_status & pci_mask) == 0) &&
		((ts2_status & ts2_mask) == 0) &&
		((ts1_status & ts1_mask) == 0))
		goto out;

	vida_count = cx_read(VID_A_GPCNT);
	audint_count = cx_read(AUD_INT_A_GPCNT);
	ts1_count = cx_read(ts1->reg_gpcnt);
	ts2_count = cx_read(ts2->reg_gpcnt);
	dprintk(7, "pci_status: 0x%08x  pci_mask: 0x%08x\n",
		pci_status, pci_mask);
	dprintk(7, "vida_status: 0x%08x vida_mask: 0x%08x count: 0x%x\n",
		vida_status, vida_mask, vida_count);
	dprintk(7, "audint_status: 0x%08x audint_mask: 0x%08x count: 0x%x\n",
		audint_status, audint_mask, audint_count);
	dprintk(7, "ts1_status: 0x%08x  ts1_mask: 0x%08x count: 0x%x\n",
		ts1_status, ts1_mask, ts1_count);
	dprintk(7, "ts2_status: 0x%08x  ts2_mask: 0x%08x count: 0x%x\n",
		ts2_status, ts2_mask, ts2_count);

	if (pci_status & (PCI_MSK_RISC_RD | PCI_MSK_RISC_WR |
			  PCI_MSK_AL_RD   | PCI_MSK_AL_WR   | PCI_MSK_APB_DMA |
			  PCI_MSK_VID_C   | PCI_MSK_VID_B   | PCI_MSK_VID_A   |
			  PCI_MSK_AUD_INT | PCI_MSK_AUD_EXT |
			  PCI_MSK_GPIO0   | PCI_MSK_GPIO1   |
			  PCI_MSK_AV_CORE | PCI_MSK_IR)) {

		if (pci_status & PCI_MSK_RISC_RD)
			dprintk(7, " (PCI_MSK_RISC_RD   0x%08x)\n",
				PCI_MSK_RISC_RD);

		if (pci_status & PCI_MSK_RISC_WR)
			dprintk(7, " (PCI_MSK_RISC_WR   0x%08x)\n",
				PCI_MSK_RISC_WR);

		if (pci_status & PCI_MSK_AL_RD)
			dprintk(7, " (PCI_MSK_AL_RD     0x%08x)\n",
				PCI_MSK_AL_RD);

		if (pci_status & PCI_MSK_AL_WR)
			dprintk(7, " (PCI_MSK_AL_WR     0x%08x)\n",
				PCI_MSK_AL_WR);

		if (pci_status & PCI_MSK_APB_DMA)
			dprintk(7, " (PCI_MSK_APB_DMA   0x%08x)\n",
				PCI_MSK_APB_DMA);

		if (pci_status & PCI_MSK_VID_C)
			dprintk(7, " (PCI_MSK_VID_C     0x%08x)\n",
				PCI_MSK_VID_C);

		if (pci_status & PCI_MSK_VID_B)
			dprintk(7, " (PCI_MSK_VID_B     0x%08x)\n",
				PCI_MSK_VID_B);

		if (pci_status & PCI_MSK_VID_A)
			dprintk(7, " (PCI_MSK_VID_A     0x%08x)\n",
				PCI_MSK_VID_A);

		if (pci_status & PCI_MSK_AUD_INT)
			dprintk(7, " (PCI_MSK_AUD_INT   0x%08x)\n",
				PCI_MSK_AUD_INT);

		if (pci_status & PCI_MSK_AUD_EXT)
			dprintk(7, " (PCI_MSK_AUD_EXT   0x%08x)\n",
				PCI_MSK_AUD_EXT);

		if (pci_status & PCI_MSK_GPIO0)
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
		util += 