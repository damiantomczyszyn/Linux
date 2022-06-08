TV-HVR1700 (PCIe, OEM, No IR, full height)
		 * DVB-T and MPEG2 HW Encoder */
		break;
	case 85021:
		/* WinTV-HVR1850 (PCIe, Retail, 3.5mm in, IR, FM,
			Dual channel ATSC and MPEG2 HW Encoder */
		break;
	case 85721:
		/* WinTV-HVR1290 (PCIe, OEM, RCA in, IR,
			Dual channel ATSC and Basic analog */
	case 121019:
		/* WinTV-HVR4400 (PCIe, DVB-S2, DVB-C/T) */
		break;
	case 121029:
		/* WinTV-HVR5500 (PCIe, DVB-S2, DVB-C/T) */
		break;
	case 150329:
		/* WinTV-HVR5525 (PCIe, DVB-S/S2, DVB-T/T2/C) */
		break;
	case 161111:
		/* WinTV-HVR-1265 K4 (PCIe, Analog/ATSC/QAM-B) */
		break;
	case 166100: /* 888 version, hybrid */
	case 166200: /* 885 version, DVB only */
		/* WinTV-QuadHD (DVB) Tuner Pair 1 (PCIe, IR, half height,
		   DVB-T/T2/C, DVB-T/T2/C */
		break;
	case 166101: /* 888 version, hybrid */
	case 166201: /* 885 version, DVB only */
		/* WinTV-QuadHD (DVB) Tuner Pair 2 (PCIe, IR, half height,
		   DVB-T/T2/C, DVB-T/T2/C */
		break;
	case 165100: /* 888 version, hybrid */
	case 165200: /* 885 version, digital only */
		/* WinTV-QuadHD (ATSC) Tuner Pair 1 (PCIe, IR, half height,
		 * ATSC/QAM-B, ATSC/QAM-B */
		break;
	case 165101: /* 888 version, hybrid */
	case 165201: /* 885 version, digital only */
		/* WinTV-QuadHD (ATSC) Tuner Pair 2 (PCIe, IR, half height,
		 * ATSC/QAM-B, ATSC/QAM-B */
		break;
	default:
		pr_warn("%s: warning: unknown hauppauge model #%d\n",
			dev->name, tv.model);
		break;
	}

	pr_info("%s: hauppauge eeprom: model=%d\n",
		dev->name, tv.model);
}

/* Some TBS cards require initing a chip using a bitbanged SPI attached
   to the cx23885 gpio's. If this chip doesn't get init'ed the demod
   doesn't respond to any command. */
static void tbs_card_init(struct cx23885_dev *dev)
{
	int i;
	static const u8 buf[] = {
		0xe0, 0x06, 0x66, 0x33, 0x65,
		0x01, 0x17, 0x06, 0xde};

	switch (dev->board) {
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		cx_set(GP0_IO, 0x00070007);
		usleep_range(1000, 10000);
		cx_clear(GP0_IO, 2);
		usleep_range(1000, 10000);
		for (i = 0; i < 9 * 8; i++) {
			cx_clear(GP0_IO, 7);
			usleep_range(1000, 10000);
			cx_set(GP0_IO,
				((buf[i >> 3] >> (7 - (i & 7))) & 1) | 4);
			usleep_range(1000, 10000);
		}
		cx_set(GP0_IO, 7);
		break;
	}
}

int cx23885_tuner_callback(void *priv, int component, int command, int arg)
{
	struct cx23885_tsport *port = priv;
	struct cx23885_dev *dev = port->dev;
	u32 bitmask = 0;

	if ((command == XC2028_RESET_CLK) || (command == XC2028_I2C_FLUSH))
		return 0;

	if (comma