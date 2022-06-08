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
