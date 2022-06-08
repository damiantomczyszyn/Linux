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

	if (command != 0) {
		pr_err("%s(): Unknown command 0x%x.\n",
		       __func__, command);
		return -EINVAL;
	}

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_LEADTEK_WINFAST_PXTV1200:
		/* Tuner Reset Command */
		bitmask = 0x04;
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		/* Two identical tuners on two different i2c buses,
		 * we need to reset the correct gpio. */
		if (port->nr == 1)
			bitmask = 0x01;
		else if (port->nr == 2)
			bitmask = 0x04;
		break;
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
		/* Tuner Reset Command */
		bitmask = 0x02;
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		altera_ci_tuner_reset(dev, port->nr);
		break;
	case CX23885_BOARD_AVERMEDIA_HC81R:
		/* XC3028L Reset Command */
		bitmask = 1 << 2;
		break;
	}

	if (bitmask) {
		/* Drive the tuner into reset and back out */
		cx_clear(GP0_IO, bitmask);
		mdelay(200);
		cx_set(GP0_IO, bitmask);
	}

	return 0;
}

void cx23885_gpio_setup(struct cx23885_dev *dev)
{
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		/* GPIO-0 cx24227 demodulator reset */
		cx_set(GP0_IO, 0x00010001); /* Bring the part out of reset */
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
		/* GPIO-0 cx24227 demodulator */
		/* GPIO-2 xc3028 tuner */

		/* Put the parts into reset */
		cx_set(GP0_IO, 0x00050000);
		cx_clear(GP0_IO, 0x00000005);
		msleep(5);

		/* Bring the parts out of reset */
		cx_set(GP0_IO, 0x00050005);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
		/* GPIO-0 cx24227 demodulator reset */
		/* GPIO-2 xc5000 tuner reset */
		cx_set(GP0_IO, 0x00050005); /* Bring the part out of reset */
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
		/* GPIO-0 656_CLK */
		/* GPIO-1 656_D0 */
		/* GPIO-2 8295A Reset */
		/* GPIO-3-10 cx23417 data0-7 */
		/* GPIO-11-14 cx23417 addr0-3 */
		/* GPIO-15-18 cx23417 READY, CS, RD, WR */
		/* GPIO-19 IR_RX */

		/* CX23417 GPIO's */
		/* EIO15 Zilog Reset */
		/* EIO14 S5H1409/CX24227 Reset */
		mc417_gpio_enable(dev, GPIO_15 | GPIO_14, 1);

		/* Put the demod into reset and prote