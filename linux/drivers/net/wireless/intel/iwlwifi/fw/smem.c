IO moving on encoder ports\n",
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
	pr_info("%s/0: 