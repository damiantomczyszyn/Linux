3885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		cx23885_irq_remove(dev, PCI_MSK_AV_CORE);
		/* sd_ir is a duplicate pointer to the AV Core, just clear it */
		dev->sd_ir = NULL;
		break;
	}
}

static int netup