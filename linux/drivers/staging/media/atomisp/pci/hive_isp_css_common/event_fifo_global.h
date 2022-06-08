< 0) {
			pr_err("%s() Failed to register dvb on VID_C\n",
			       __func__);
		}
	} else
	if (cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER) {
		if (cx23885_417_register(dev) < 0) {
			pr_err("%s() Failed to register 417 on VID_C\n",
			       __func__);
		}
	}

	cx23885_dev_checkrevision(dev);

	/* disable MSI for NetUP cards, otherwise CI is not working */
	if (cx23885_boards[dev->board].ci_type > 0)
		cx_clear(RDR_RDRCTL1, 1 << 8);

	switch (dev->board) {
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TEVII_S471:
		cx_clear(RDR_RDRCTL1, 1 << 8);
		break;
	}

	return 0;
}

static void cx23885_dev_unregister(struct cx23885_dev *dev)
{
	release_mem_region(pci_resource_start(dev->pci, 0),
			   pci_resource_len(dev->pci, 0));

