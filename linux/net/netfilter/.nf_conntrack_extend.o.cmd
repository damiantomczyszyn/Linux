		sn);
}

static void hauppauge_eeprom(struct cx23885_dev *dev, u8 *eeprom_data)
{
	struct tveeprom tv;

	tveeprom_hauppauge_analog(&tv, eeprom_data);

	/* Make sure we support the board model */
	switch (tv.model) {
	case 22001:
		/* WinTV-HVR1270 (PCIe, Retail, half height)
		 * ATSC/QAM and basic analog, IR Blast */
	case 22009:
		/* WinTV-HVR1210 (PCIe, Retail, half height)
		 * DVB-T and basic analog, IR Blast */
	case 22011:
		/* WinTV-HVR1270 (PCIe, Retail, half height)
		 * ATSC/QAM and basic analog, IR Recv */
	case 22019:
		/* WinTV-HVR1210 (PCIe, Retail, half height)
		 * DVB-T and basic analog, IR Recv */
	case 22021:
		/* WinTV-HVR1275 (PCIe, Retail, half height)
		 * ATSC/QAM and basic analog, IR Recv */
	case 22029:
		/* WinTV-HVR1210 (PCIe, Retail, half height)
		 * DVB-T and basic analog, IR Recv */
	case 22101:
		/* WinTV-HVR1270 (PCIe, Retail, full height)
		 * ATSC/QAM and basic analog, IR Blast */
	case 22109:
		/* WinTV-HVR1210 (PCIe, Retail, full height)
		 * DVB-T and basic analog, IR Blast */
	case 22111:
		/* WinTV-HVR1270 (PCIe, Retail, full height)
		 * ATSC/QAM and basic analog, IR Recv */
	case 22119:
		/* WinTV-HVR1210 (PCIe, Retail, full height)
		 * DVB-T and basic analog, IR Recv */
	case 22121:
		/* WinTV-HVR1275 (PCIe, Retail, full height)
		 * ATSC/QAM and basic analog, IR Recv */
	case 22129:
		/* WinTV-HVR1210 (PCIe, Retail, full height)
		 * DVB-T and basic analog, IR Recv */
	case 71009:
		/* WinTV-HVR1200 (PCIe, Retail, full height)
		 * DVB-T and basic analog */
	case 71100:
		/* WinTV-ImpactVCB-e (PCIe, Retail, half height)
		 * Basic analog */
	case 71359:
		/* WinTV-HVR1200 (PCIe, OEM, half height)
		 * DVB-T and basic analog */
	case 71439:
		/* WinTV-HVR1200 (PCIe, OEM, half height)
		 * DVB-T and basic analog */
	case 71449:
		/* WinTV-HVR1200 (PCIe, OEM, full height)
		 * DVB-T and basic analog */
	case 71939:
		/* WinTV-HVR1200 (PCIe, OEM, half height)
		 * DVB-T and basic analog */
	case 71949:
		/* WinTV-HVR1200 (PCIe, OEM, full height)
		 * DVB-T and basic analog */
	case 71959:
		/* WinTV-HVR1200 (PCIe, OEM, full height)
		 * DVB-T and basic analog */
	case 71979:
		/* WinTV-HVR1200 (PCIe, OEM, half height)
		 * DVB-T and basic analog */
	case 71999:
		/* WinTV-HVR1200 (PCIe, OEM, full height)
		 * DVB-T and basic analog */
	case 76601:
		/* WinTV-HVR1800lp (PCIe, Retail, No IR, Dual
			channel ATSC and MPEG2 HW Encoder */
	case 77001:
		/* WinTV-HVR1500 (Express Card, OEM, No IR, ATSC
			and Basic analog */
	case 77011:
		/* WinTV-HVR1500 (Express Card, Retail, No IR, ATSC
			and Basic analog */
	case 77041:
		/* WinTV-HVR1500Q (Express Card, OEM, No IR, ATSC/QAM
			and Basic analog */
	case 77051:
		/* WinTV-HVR1500Q (Express Card, Retail, No IR, ATSC/QAM
			and Basic analog */
	case 78011:
		/* WinTV-HVR1800 (PCIe, Retail, 3.5mm in, IR, No FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 78501:
		/* WinTV-HVR1800 (PCIe, OEM, RCA in, No IR, FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 78521:
		/* WinTV-HVR1800 (PCIe, OEM, RCA in, No IR, FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 78531:
		/* WinTV-HVR1800 (PCIe, OEM, RCA in, No IR, No FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 78631:
		/* WinTV-HVR1800 (PCIe, OEM, No IR, No FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 79001:
		/* WinTV-HVR1250 (PCIe, Retail, IR, full height,
			ATSC and Basic analog */
	case 79101:
		/* WinTV-HVR1250 (PCIe, Retail, IR, half height,
			ATSC and Basic analog */
	case 79501:
		/* WinTV-HVR1250 (PCIe, No IR, half height,
			ATSC [at least] and Basic analog) */
	case 79561:
		/* WinTV-HVR1250 (PCIe, OEM, No IR, half height,
			ATSC and Basic analog */
	case 79571:
		/* WinTV-HVR1250 (PCIe, OEM, No IR, full height,
		 ATSC and Basic analog */
	case 79671:
		/* WinTV-HVR1250 (PCIe, OEM, No IR, half height,
			ATSC and Basic analog */
	case 80019:
		/* WinTV-HVR1400 (Express Card, Retail, IR,
		 * DVB-T and Basic analog */
	case 81509:
		/* WinTV-HVR1700 (PCIe, OEM, No IR, half height)
		 * DVB-T and MPEG2 HW Encoder */
	case 81519:
		/* WinTV-HVR1700 (PCIe, OEM, No IR, full height)
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

		/* Put the demod into reset and protect the eeprom */
		mc417_gpio_clear(dev, GPIO_15 | GPIO_14);
		msleep(100);

		/* Bring the demod and blaster out of reset */
		mc417_gpio_set(dev, GPIO_15 | GPIO_14);
		msleep(100);

		/* Force the TDA8295A into reset and back */
		cx23885_gpio_enable(dev, GPIO_2, 1);
		cx23885_gpio_set(dev, GPIO_2);
		msleep(20);
		cx23885_gpio_clear(dev, GPIO_2);
		msleep(20);
		cx23885_gpio_set(dev, GPIO_2);
		msleep(20);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
		/* GPIO-0 tda10048 demodulator reset */
		/* GPIO-2 tda18271 tuner reset */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x00050000);
		msleep(20);
		cx_clear(GP0_IO, 0x00000005);
		msleep(20);
		cx_set(GP0_IO, 0x00050005);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
		/* GPIO-0 TDA10048 demodulator reset */
		/* GPIO-2 TDA8295A Reset */
		/* GPIO-3-10 cx23417 data0-7 */
		/* GPIO-11-14 cx23417 addr0-3 */
		/* GPIO-15-18 cx23417 READY, CS, RD, WR */

		/* The following GPIO's are on the interna AVCore (cx25840) */
		/* GPIO-19 IR_RX */
		/* GPIO-20 IR_TX 416/DVBT Select */
		/* GPIO-21 IIS DAT */
		/* GPIO-22 IIS WCLK */
		/* GPIO-23 IIS BCLK */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x00050000);
		msleep(20);
		cx_clear(GP0_IO, 0x00000005);
		msleep(20);
		cx_set(GP0_IO, 0x00050005);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
		/* GPIO-0  Dibcom7000p demodulator reset */
		/* GPIO-2  xc3028L tuner reset */
		/* GPIO-13 LED */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x00050000);
		msleep(20);
		cx_clear(GP0_IO, 0x00000005);
		msleep(20);
		cx_set(GP0_IO, 0x00050005);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP:
		/* GPIO-0 xc5000 tuner reset i2c bus 0 */
		/* GPIO-1 s5h1409 demod reset i2c bus 0 */
		/* GPIO-2 xc5000 tuner reset i2c bus 1 */
		/* GPIO-3 s5h1409 demod reset i2c bus 0 */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x000f0000);
		msleep(20);
		cx_clear(GP0_IO, 0x0000000f);
		msleep(20);
		cx_set(GP0_IO, 0x000f000f);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		/* GPIO-0 portb xc3028 reset */
		/* GPIO-1 portb zl10353 reset */
		/* GPIO-2 portc xc3028 reset */
		/* GPIO-3 portc zl10353 reset */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x000f0000);
		msleep(20);
		cx_clear(GP0_IO, 0x0000000f);
		msleep(20);
		cx_set(GP0_IO, 0x000f000f);
		break;
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_LEADTEK_WINFAST_PXTV1200:
		/* GPIO-2  xc3028 tuner reset */

		/* The following GPIO's are on the internal AVCore (cx25840) */
		/* GPIO-?  zl10353 demod reset */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x00040000);
		msleep(20);
		cx_clear(GP0_IO, 0x00000004);
		msleep(20);
		cx_set(GP0_IO, 0x00040004);
		break;
	case CX23885_BOARD_TBS_6920:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_PROF_8000:
		cx_write(MC417_CTL, 0x00000036);
		cx_write(MC417_OEN, 0x00001000);
		cx_set(MC417_RWD, 0x00000002);
		msleep(200);
		cx_clear(MC417_RWD, 0x00000800);
		msleep(200);
		cx_set(MC417_RWD, 0x00000800);
		msleep(200);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		/* GPIO-0 INTA from CiMax1
		   GPIO-1 INTB from CiMax2
		   GPIO-2 reset chips
		   GPIO-3 to GPIO-10 data/addr for CA
		   GPIO-11 ~CS0 to CiMax1
		   GPIO-12 ~CS1 to CiMax2
		   GPIO-13 ADL0 load LSB addr
		   GPIO-14 ADL1 load MSB addr
		   GPIO-15 ~RDY from CiMax
		   GPIO-17 ~RD to CiMax
		   GPIO-18 ~WR to CiMax
		 */
		cx_set(GP0_IO, 0x00040000); /* GPIO as out */
		/* GPIO1 and GPIO2 as INTA and INTB from CiMaxes, reset low */
		cx_clear(GP0_IO, 0x00030004);
		msleep(100);/* reset delay */
		cx_set(GP0_IO, 0x00040004); /* GPIO as out, reset high */
		cx_write(MC417_CTL, 0x00000037);/* enable GPIO3-18 pins */
		/* GPIO-15 IN as ~ACK, rest as OUT */
		cx_write(MC417_OEN, 0x00001000);
		/* ~RD, ~WR high; ADL0, ADL1 low; ~CS0, ~CS1 high */
		cx_write(MC417_RWD, 0x0000c300);
		/* enable irq */
		cx_write(GPIO_ISM, 0x00000000);/* INTERRUPTS active low*/
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
		/* GPIO-5 RF Control: 0 = RF1 Terrestrial, 1 = RF2 Cable */
		/* GPIO-6 I2C Gate which can isolate the demod from the bus */
		/* GPIO-9 Demod reset */

		/* Put the parts into reset and back */
		cx23885_gpio_enable(dev, GPIO_9 | GPIO_6 | GPIO_5, 1);
		cx23885_gpio_set(dev, GPIO_9 | GPIO_6 | GPIO_5);
		cx23885_gpio_clear(dev, GPIO_9);
		msleep(20);
		cx23885_gpio_set(dev, GPIO_9);
		break;
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
	case CX23885_BOARD_MYGICA_X8507:
		/* GPIO-0 (0)Analog / (1)Digital TV */
		/* GPIO-1 reset XC5000 */
		/* GPIO-2 demod reset */
		cx23885_gpio_enable(dev, GPIO_0 | GPIO_1 | GPIO_2, 1);
		cx23885_gpio_clear(dev, GPIO_1 | GPIO_2);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_0 | GPIO_1 | GPIO_2);
		msleep(100);
		break;
	case CX23885_BOARD_MYGICA_X8558PRO:
		/* GPIO-0 reset first ATBM8830 */
		/* GPIO-1 reset second ATBM8830 */
		cx23885_gpio_enable(dev, GPIO_0 | GPIO_1, 1);
		cx23885_gpio_clear(dev, GPIO_0 | GPIO_1);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_0 | GPIO_1);
		msleep(100);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		/* GPIO-0 656_CLK */
		/* GPIO-1 656_D0 */
		/* GPIO-2 Wake# */
		/* GPIO-3-10 cx23417 data0-7 */
		/* GPIO-11-14 cx23417 addr0-3 */
		/* GPIO-15-18 cx23417 READY, CS, RD, WR */
		/* GPIO-19 IR_RX */
		/* GPIO-20 C_IR_TX */
		/* GPIO-21 I2S DAT */
		/* GPIO-22 I2S WCLK */
		/* GPIO-23 I2S BCLK */
		/* ALT GPIO: EXP GPIO LATCH */

		/* CX23417 GPIO's */
		/* GPIO-14 S5H1411/CX24228 Reset */
		/* GPIO-13 EEPROM write protect */
		mc417_gpio_enable(dev, GPIO_14 | GPIO_13, 1);

		/* Put the demod into reset and protect the eeprom */
		mc417_gpio_clear(dev, GPIO_14 | GPIO_13);
		msleep(100);

		/* Bring the demod out of reset */
		mc417_gpio_set(dev, GPIO_14);
		msleep(100);

		/* CX24228 GPIO */
		/* Connected to IF / Mux */
		break;
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
		cx_set(GP0_IO, 0x00010001); /* Bring the part out of reset */
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		/* GPIO-0 ~INT in
		   GPIO-1 TMS out
		   GPIO-2 ~reset chips out
		   GPIO-3 to GPIO-10 data/addr for CA in/out
		   GPIO-11 ~CS out
		   GPIO-12 ADDR out
		   GPIO-13 ~WR out
		   GPIO-14 ~RD out
		   GPIO-15 ~RDY in
		   GPIO-16 TCK out
		   GPIO-17 TDO in
		   GPIO-18 TDI out
		 */
		cx_set(GP0_IO, 0x00060000); /* GPIO-1,2 as out */
		/* GPIO-0 as INT, reset & TMS low */
		cx_clear(GP0_IO, 0x00010006);
		msleep(100);/* reset delay */
		cx_set(GP0_IO, 0x00000004); /* reset high */
		cx_write(MC417_CTL, 0x00000037);/* enable GPIO-3..18 pins */
		/* GPIO-17 is TDO in, GPIO-15 is ~RDY in, rest is out */
		cx_write(MC417_OEN, 0x00005000);
		/* ~RD, ~WR high; ADDR low; ~CS high */
		cx_write(MC417_RWD, 0x00000d00);
		/* enable irq */
		cx_write(GPIO_ISM, 0x00000000);/* INTERRUPTS active low*/
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR4400:
	case CX23885_BOARD_HAUPPAUGE_STARBURST:
		/* GPIO-8 tda10071 demod reset */
		/* GPIO-9 si2165 demod reset (only HVR4400/HVR5500)*/

		/* Put the parts into reset and back */
		cx23885_gpio_enable(dev, GPIO_8 | GPIO_9, 1);

		cx23885_gpio_clear(dev, GPIO_8 | GPIO_9);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_8 | GPIO_9);
		msleep(100);

		break;
	case CX23885_BOARD_AVERMEDIA_HC81R:
		cx_clear(MC417_CTL, 1);
		/* GPIO-0,1,2 setup direction as output */
		cx_set(GP0_IO, 0x00070000);
		usleep_range(10000, 11000);
		/* AF9013 demod reset */
		cx_set(GP0_IO, 0x00010001);
		usleep_range(10000, 11000);
		cx_clear(GP0_IO, 0x00010001);
		usleep_range(10000, 11000);
		cx_set(GP0_IO, 0x00010001);
		usleep_range(10000, 11000);
		/* demod tune? */
		cx_clear(GP0_IO, 0x00030003);
		usleep_range(10000, 11000);
		cx_set(GP0_IO, 0x00020002);
		usleep_range(10000, 11000);
		cx_set(GP0_IO, 0x00010001);
		usleep_range(10000, 11000);
		cx_clear(GP0_IO, 0x00020002);
		/* XC3028L tuner reset */
		cx_set(GP0_IO, 0x00040004);
		cx_clear(GP0_IO, 0x00040004);
		cx_set(GP0_IO, 0x00040004);
		msleep(60);
		break;
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		/* enable GPIO3-18 pins */
		cx_write(MC417_CTL, 0x00000037);
		cx23885_gpio_enable(dev, GPIO_2 | GPIO_11, 1);
		cx23885_gpio_clear(dev, GPIO_2 | GPIO_11);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_2 | GPIO_11);
		break;
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
		/*
		 * GPIO-0 INTA from CiMax, input
		 * GPIO-1 reset CiMax, output, high active
		 * GPIO-2 reset demod, output, low active
		 * GPIO-3 to GPIO-10 data/addr for CAM
		 * GPIO-11 ~CS0 to CiMax1
		 * GPIO-12 ~CS1 to CiMax2
		 * GPIO-13 ADL0 load LSB addr
		 * GPIO-14 ADL1 load MSB addr
		 * GPIO-15 ~RDY from CiMax
		 * GPIO-17 ~RD to CiMax
		 * GPIO-18 ~WR to CiMax
		 */

		cx_set(GP0_IO, 0x00060002); /* GPIO 1/2 as output */
		cx_clear(GP0_IO, 0x00010004); /* GPIO 0 as input */
		msleep(100); /* reset delay */
		cx_set(GP0_IO, 0x00060004); /* GPIO as out, reset high */
		cx_clear(GP0_IO, 0x00010002);
		cx_write(MC417_CTL, 0x00000037); /* enable GPIO3-18 pins */

		/* GPIO-15 IN as ~ACK, rest as OUT */
		cx_write(MC417_OEN, 0x00001000);

		/* ~RD, ~WR high; ADL0, ADL1 low; ~CS0, ~CS1 high */
		cx_write(MC417_RWD, 0x0000c300);

		/* enable irq */
		cx_write(GPIO_ISM, 0x00000000); /* INTERRUPTS active low */
		break;
	case CX23885_BOARD_DVBSKY_S950:
		cx23885_gpio_enable(dev, GPIO_2, 1);
		cx23885_gpio_clear(dev, GPIO_2);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_2);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_STARBURST2:
		/*
		 * HVR5525 GPIO Details:
		 *  GPIO-00 IR_WIDE
		 *  GPIO-02 wake#
		 *  GPIO-03 VAUX Pres.
		 *  GPIO-07 PROG#
		 *  GPIO-08 SAT_RESN
		 *  GPIO-09 TER_RESN
		 *  GPIO-10 B2_SENSE
		 *  GPIO-11 B1_SENSE
		 *  GPIO-15 IR_LED_STATUS
		 *  GPIO-19 IR_NARROW
		 *  GPIO-20 Blauster1
		 *  ALTGPIO VAUX_SWITCH
		 *  AUX_PLL_CLK : Blaster2
		 */
		/* Put the parts into reset and back */
		cx23885_gpio_enable(dev, GPIO_8 | GPIO_9, 1);
		cx23885_gpio_clear(dev, GPIO_8 | GPIO_9);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_8 | GPIO_9);
		msleep(100);
		break;
	case CX23885_BOARD_VIEWCAST_260E:
	case CX23885_BOARD_VIEWCAST_460E:
		/* For documentation purposes, it's worth noting that this
		 * card does not have any GPIO's connected to subcomponents.
		 */
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885:
		/*
		 * GPIO-08 TER1_RESN
		 * GPIO-09 TER2_RESN
		 */
		/* Put the parts into reset and back */
		cx23885_gpio_enable(dev, GPIO_8 | GPIO_9, 1);
		cx23885_gpio_clear(dev, GPIO_8 | GPIO_9);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_8 | GPIO_9);
		msleep(100);
		break;
	}
}

int cx23885_ir_init(struct cx23885_dev *dev)
{
	static struct v4l2_subdev_io_pin_config ir_rxtx_pin_cfg[] = {
		{
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_INPUT),
			.pin	  = CX23885_PIN_IR_RX_GPIO19,
			.function = CX23885_PAD_IR_RX,
			.value	  = 0,
			.strength = CX25840_PIN_DRIVE_MEDIUM,
		}, {
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_OUTPUT),
			.pin	  = CX23885_PIN_IR_TX_GPIO20,
			.function = CX23885_PAD_IR_TX,
			.value	  = 0,
			.strength = CX25840_PIN_DRIVE_MEDIUM,
		}
	};
	const size_t ir_rxtx_pin_cfg_count = ARRAY_SIZE(ir_rxtx_pin_cfg);

	static struct v4l2_subdev_io_pin_config ir_rx_pin_cfg[] = {
		{
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_INPUT),
			.pin	  = CX23885_PIN_IR_RX_GPIO19,
			.function = CX23885_PAD_IR_RX,
			.value	  = 0,
			.strength = CX25840_PIN_DRIVE_MEDIUM,
		}
	};
	const size_t ir_rx_pin_cfg_count = ARRAY_SIZE(ir_rx_pin_cfg);

	struct v4l2_subdev_ir_parameters params;
	int ret = 0;
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		/* FIXME: Implement me */
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
		ret = cx23888_ir_probe(dev);
		if (ret)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_888_IR);
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rx_pin_cfg_count, ir_rx_pin_cfg);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		ret = cx23888_ir_probe(dev);
		if (ret)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_888_IR);
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rxtx_pin_cfg_count, ir_rxtx_pin_cfg);
		/*
		 * For these boards we need to invert the Tx output via the
		 * IR controller to have the LED off while idle
		 */
		v4l2_subdev_call(dev->sd_ir, ir, tx_g_parameters, &params);
		params.enable = false;
		params.shutdown = false;
		params.invert_level = true;
		v4l2_subdev_call(dev->sd_ir, ir, tx_s_parameters, &params);
		params.shutdown = true;
		v4l2_subdev_call(dev->sd_ir, ir, tx_s_parameters, &params);
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		if (!enable_885_ir)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_AV_CORE);
		if (dev->sd_ir == NULL) {
			ret = -ENODEV;
			break;
		}
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rx_pin_cfg_count, ir_rx_pin_cfg);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		if (!enable_885_ir)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_AV_CORE);
		if (dev->sd_ir == NULL) {
			ret = -ENODEV;
			break;
		}
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rxtx_pin_cfg_count, ir_rxtx_pin_cfg);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		request_module("ir-kbd-i2c");
		break;
	}

	return ret;
}

void cx23885_ir_fini(struct cx23885_dev *dev)
{
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		cx23885_irq_remove(dev, PCI_MSK_IR);
		cx23888_ir_remove(dev);
		dev->sd_ir = NULL;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
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

static int netup_jtag_io(void *device, int tms, int tdi, int read_tdo)
{
	int data;
	int tdo = 0;
	struct cx23885_dev *dev = (struct cx23885_dev *)device;
	/*TMS*/
	data = ((cx_read(GP0_IO)) & (~0x00000002));
	data |= (tms ? 0x00020002 : 0x00020000);
	cx_write(GP0_IO, data);

	/*TDI*/
	data = ((cx_read(MC417_RWD)) & (~0x0000a000));
	data |= (tdi ? 0x00008000 : 0);
	cx_write(MC417_RWD, data);
	if (read_tdo)
		tdo = (data & 0x00004000) ? 1 : 0; /*TDO*/

	cx_write(MC417_RWD, data | 0x00002000);
	udelay(1);
	/*TCK*/
	cx_write(MC417_RWD, data);

	return tdo;
}

void cx23885_ir_pci_int_enable(struct cx23885_dev *dev)
{
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		if (dev->sd_ir)
			cx23885_irq_add_enable(dev, PCI_MSK_IR);
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		if (dev->sd_ir)
			cx23885_irq_add_enable(dev, PCI_MSK_AV_CORE);
		break;
	}
}

void cx23885_card_setup(struct cx23885_dev *dev)
{
	struct cx23885_tsport *ts1 = &dev->ts1;
	struct cx23885_tsport *ts2 = &dev->ts2;

	static u8 eeprom[256];

	if (dev->i2c_bus[0].i2c_rc == 0) {
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client,
			      eeprom, sizeof(eeprom));
	}

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		if (dev->i2c_bus[0].i2c_rc == 0) {
			if (eeprom[0x80] != 0x84)
				hauppauge_eeprom(dev, eeprom+0xc0);
			else
				hauppauge_eeprom(dev, eeprom+0x80);
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
		if (dev->i2c_bus[0].i2c_rc == 0)
			hauppauge_eeprom(dev, eeprom+0x80);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR4400:
	case CX23885_BOARD_HAUPPAUGE_STARBURST:
	case CX23885_BOARD_HAUPPAUGE_IMPACTVCBE:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_STARBURST2:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885:
		if (dev->i2c_bus[0].i2c_rc == 0)
			hauppauge_eeprom(dev, eeprom+0xc0);
		break;
	case CX23885_BOARD_VIEWCAST_260E:
	case CX23885_BOARD_VIEWCAST_460E:
		dev->i2c_bus[1].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[1].i2c_client,
			      eeprom, sizeof(eeprom));
		if (dev->i2c_bus[0].i2c_rc == 0)
			viewcast_eeprom(dev, eeprom);
		break;
	}

	switch (dev->board) {
	case CX23885_BOARD_AVERMEDIA_HC81R:
		/* Defaults for VID B */
		ts1->gen_ctrl_val  = 0x4; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		/* Defaults for VID C */
		/* DREQ_POL, SMODE, PUNC_CLK, MCLK_POL Serial bus + punc clk */
		ts2->gen_ctrl_val  = 0x10e;
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2������'A�
��zC���M�tIḠ'���F򞈌�+�̤���ھ%N�K�9d��"�"�Hx+�j#�Hn�~���9��_Gz�����EU��Q ��	��!"_U��E��mC�:G��$K��C7�k�#9/j��p�0��S���~�2�ׇ��+�X��Ŭ���!�碀[��-���x�td.������S:���.�R%��M`W=:d�uUT�����u���D���*�|P&Bf�y#+6_}x�Dy�SE�h�M����D<}�O�R����	��a"=��Q鹤��=��:Z8�(����lԱX��.���c% <�}rJה
~�fXm��9�n�h�'��4r�-"$�V=���:�OY�J�#a�`Y�B���65��������E��d�q.�����{� K���4.I��֡�6a4�9�Z�_4\!'|k����c�
<&	q6ÊH-���o��6���p_��F��c��E��~F�]���ք��P-�p�@�	.���? ��s������{]�TPE�	��EKz\��^1�]N*Bq2$H���#94hxK�ٯ�ƂɉG��З��M��@�ĵJ�����"��[~WdV�K����a]o�2kx&��j����"g�g�-�4�Z%i9eNV�l�(3'�ԍe��T�'�p�6%aA�1���P�m�=������Kǖ�`�5��5���U��+ܞ��H��/
U��	Χ-�|�[9b��M�4Y�"��B�Ӱ��\WU#T�v�$~��_�	�eۆ�@\��0�-��	s����Ȁ�T�̔�2���|Q�d�;�	^ ���9k�%���1��8�8Ah���-ܒY�_0�?/��K`�j ���0��tRFmAY���>����$�j�ga�է{�����=��9���6i��Χ��?�+�Q�H�=�5�/`�쬨P(�������)�X�6�/q<��k�U�2U��Íp��J-�fF:s��s;\,?i{.Y�5�m��9��N�������9_��j�,��Dt��q�I}úv2~9l�B�� �Mə�r��h԰��gT2Z��&r����>}����|�:�HTþO� �ER�" @5`=�޿ dY��t#n��^<&�Rx�qr�ޣ���>�����@⏭˸���zl�npkX�XĻ_`���|!�R���?o�1e@�3vȦ����LX�@Q���v��og}}�0G�4�W1�?��/�=:^�I�	΢�ոZw��U�"	K���>�I�ϙ��\qr}PSG��Ab<�-��f]��sH@{&Op�H.�&do��5���z�\ȕ�)Q�_��:���U̠��R�Zƽ��A�#v��B@��v_L���\~Y�&~���|�C��\Υ~
y�ilQcQ�0g���`-y�E�h#���u�����Լl,�W��O�D,[�#�_��$���W��
!�ة�����	��d���W C�ǲ�6�� n���^��������6hh���zǬD]^�ޒ�Q=i+c�LQ�,��8�v����AV��)�s�N�)WJ��k����k)f4V��e��י�P��T���� ^
Vsg�٫��`W���:���
F�]c WUV>%�������Я��N�ta�KE����+�˨������=� �*ӿ�U?��x6"g�z�o��2�O�*�tE�� �!��%e}�Ԃ1{� n�B�w��������p a��:����S��<�u<�z^|�x �KZ
ٙ���`,2"`����O�����3b�|��[�H�\�$5�#%��p�s������H�c����@��ߞC����^�`GRL����M�_')�N��V�y��"G�����[I��[�Sq�r)�� �'0�\�?�[>;(�!�)dB�#rʢs&5C���V��Z�
/�f͝<tf�&>��C�d��㏜v# �{e������w�g*�������8`�!�J���R����E[yPQ�W!&%�M�n�DO5�Q��l?,
P���8t���28�a�Tė"/d�����O͢�8z���-X%�l��%� B�� �^�����/�!J�2Է�,)	�}�����M��|r�_gLwCy����1�EA���b��/�a�q���Y�s��iR2vL�fa�{���MÂ�J�޹<��/j�Qh�i�[^;�<�y��..j�[0��J�;J�?̷���,u�֤"o�l��ֹc��h����5���zR��z?����G�"��(ta[k�o��Bh��4m-Du+��z�+�mܣ�H���f����E��V���=�Pp���>
e���K#�P2�̫��
��8rA]oRA�S�Mn%Η,�_�^���+W2
�q��:�٪�����$`��1a��ga+��ֱ\��Ђ}6�\��'�MWRB���Nq� �)����'�1�=`��NU��2s�sׄ���$���р<ֈ�& \��N���/#N�����LǊ�.r*��gv�y		Ff��4ɮ��X�pi@τ�� 1�i:�:��ˠl��4("�<P8�p�u�A�t8	�g<��a�	�{��C�N�$+�#	b�4p�0q�g����pB�'l��α�W+�H�.�:��۞���3!�p�A�ա�3�������EQ�j�]XoLM�L�F��CY�Q��D@T��Al������a*�j��A�*׺4J 6�-��d�꼾��<�f%O�_eϽR�C��F��[U�gxr s��s1��u�n�3�������m�����e%����c�8��7�~0�=%۵�
��¤�Q�+T�Gԡ_P�,l�����?�o��Fr"b)��.��}4�2=x�g�p1��ў0)F�G���[_7��K���}��T�)��/��F��ဏ@2���.��7�6���Y�
f�L��p��b���=r=�F9xw$T�����tiTn��dE��8ٔ����v�r=�O�P�a�B �L0Ph�F���=�����5���'��7v�O�=b��%`��!�39\���o�O�ta���`o�c�׋��|B��=y&*���G�"�bG���\i�����Ig��:�L~� ��/3�*�R�a�ё݇ݩ%H���H�jj_��%O؈X�.u���x�`$\}����-@�bxO�Y	A�«��t[T���|�Ȗ
g���&����)n$�e�cw��D�Bod"��Aq嚌+_�
|��&�U��8�b�mo�@(d���;�v5s*:#C!`	.c6�V�`x�F���Q�H;Q�e.��c���tć7$�rB��n:��q�f�x��(j��_WK2�C���+��*vy���<W�FI����;�n�}��J)wx��F�����g�����r}���h��r��
�;M~6%�J����V�Ľ\�GV!x��Sv�Ql��H�U(!����ҕ���rOT�&�g�A���A6BJxz8�	�|��,k�	tZ�jYb���5Skue��4����0��a���h��:6R��0�����\�Ͼ����������eaN��J�O8w��P���KPo���(�f9m�
%��L�񏣻40�M�,��_�G/0J�#���$�!�8�/'$!N���j���cz�`\`wB:F�?�&~�1�i�t����,��n��zyh���d%*����(�d�����][/�����I�(�鱍�;>F�R�(\����՛#�<�/��<�i�D���!�>x����<�l.n��*A%�6m�e�*&����Q�ɕͱ��Ώ��@�w���8��H�2� �`�r��㿟W���V��x���j�ܤ��W�ʰ����J&�9�Z쨜�B$g�w�+��e��v��
fr�����/� %(ؙd��=��� �2�T?-?��.�Y�%��h���+?�GAD~�G��u�0@�%�?���#Oҍ��������Кpp�m*�kysO �p�7�o���������ܵ�	�]G*|�v@qYI�����]��p;��>�
��I`�ā�n��v��؎�IDEO;
		break;
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T982:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0x8; /* Serial bus */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_DVBSKY_S952:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xe; /* Serial bus */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_STARBURST2:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
	case CX23885_BOARD_HAUPPAUGE_IMPACTVCBE:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
	default:
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
	}

	/* Certain boards support analog, or require the avcore to be
	 * loaded, ensure this happens.
	 */
	switch (dev->board) {
	case CX23885_BOARD_TEVII_S470:
		/* Currently only enabled for the integrated IR controller */
		if (!enable_885_ir)
			break;
		fallthrough;
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
	case CX23885_BOARD_HAUPPAUGE_IMPACTVCBE:
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_LEADTEK_WINFAST_PXTV1200:
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
	case CX23885_BOARD_HAUPPA����E�M��Aئ^7J�v���A�=�s���q|J�5�����&1JM1��z~G�����C���#��7�k��GZ���>��O�����.%��fS�z�j=�O�`�ݡ�VgV�ɩ*��YCYEiq�ܢ�/`��A��I0<YJCtʍa�NH��#�Y�JL�u��SO�V��\̯��s�^�ꢼ�#�(yq�����>�wP ݉������JK�n����U(5�g��ck���~���US���n�fn�}J5�%��8*����Z�{?��,�B����86�ǷK��	�CB�i��͉^%�ε/vC����[(�!�"[05gf���K
>��o��[s�6Є�����w�ƫ-l^�C?<zb����/��I1�։@!�9��<H�s�;�p&��χ���]�n%�T-T��7��[n�U�_]�z�t�O>�I�����2.	c��l\��.���w���vgno����?�Ƨ������!T��.�xoF�s2��(��v���h6�g��g"���L�5K[�Cq�n��4��GgsB��ꚱw.�/�P�\~0L�C�Òn4�q!�`�����"�$V+$.� �U��2hJ�z�;�g�"�:���R!�1%��yeb�� [COa��+,�A��Z^��U;�EjT�f~����"��0� �`Hd�#o�e�+��m4!���ơA����in �7t�&������kUq&�Nhx8��!�k�%�BE#��\��<�@'�KJ%��}�_HIi�Q��ô�	"2;��킴amt�+��c���"��b�hH7J��C��ݷ���T�+�r ���h�"�> �h�]��(n�Kȡ��s���dx4c˙"�ٽ�+�x?mȦ9DF�a�7'���`x�QDғ�D�-L.��ID�G��6#}��Y��q�]��o����?x�ot6�����%= �P�Z�iD�[R�������F5�v;�e��ēn��|��!
�1����]��O6�B"�(����-�]C�HcB�&���M�isEՓ��6n��ֿ��	J���N��A͒6[�z�<8I�{f���`T��1���ѿIvL��*�0w��y'�'�6W`]-�>�w-���C��͌��;TAd�/�Hb���w�ZM=��Ƣq��.1��Lqj�lgdC�W�pG%?<+4̈́�30'FK��<F�D��V��u�g�U�AR2���Q����?��Y����.4�yOH�Y��]1Kf�����)�͝��u�`��=�(��u|+� ���@� r�MC�T�}|ޒ|�ʢ ڛ�us�^CO?LO����RÌ��V���/"I�k�������������BKF���*,��?������( ���jq�!��l�^�`�s��U�A�7c�P>R��}����]Lb�mB3Z�m����N�.8Ȃ/ۄrJ�-�Tz��+�c<�}�9�,U�t�G���*�[J�X���\�7@��n�"�_�f�肠%cF�[�b��6�1�v<Y��;�Q����^ nc�m{�;�#N,�7PA� W�7x�}��9�	g��bt@T��$'p�I^9W*������J�Amn�m��
b:Yw`0�I��C��H9�'tiS ��L�7ÿ�T*Er5�J�	�mZT�w?'�S�E����IG�����OOӃ:�V�>-��۝�CH�!�v<4��S�j#-)c��<ڗ���t�o�.Ɓ��3�9l=��ug����]�
:����9�<,l7� @����Eą��{� �����s�}vwB��?l~8��!��ީ�H9s.wq��ٚT�<��*Ѷ���%]M��c�����O�b|�u���j�+Z�I#P	S[��b��2@�t�D��ר�Vq���z�4�u�0� M+�T*:w��I�R=�C�1�9Om��Z����Ha��;2�+Z�}/ǹ������`hNU4���x���J�}Oo�RF��C3��%n/�h�������I'�` /��3\��	�4�'戃W�Zi�i�>I�?���Pܕ����tދ�����C��W��O�	y^�4����)��<����p�r�+@���~�ż������0�,�}{J��2��kθ��l���8���������O�f�ﰨ�1�K��n1M�/�pt��;/�2�L0f}����%9��As��-|1%~�^�����׼�Y��V
$\�����������wyhn@a�r�&�*��y�������Zt���@a��-��*/�f�1){�*,|�����!*��w<,�	[�b�|nwz��V��fvIK�UV)<��0�C���p��X+�uO�@��,\�?i����|���T>O���%��F�E��Pf@�9kt��	��૯M[��s����I�K��i;� \�
�X��M�[Z��VX)4os;��i�%W�Ҷ���-`�����⿷1�O@c)���8ԭ7V�;b9k\�W6����-ƘgPՃm�8��SD���^G�tw�H$�3��맇�۱�-o�cAn
��5�/	A}���Ui�ԷL��
�4���� k��X��ï���o�4����ǃEGQ"X9xB5�49ea?#�2�e' 0�1џ3��"�ːUWY�d�ĩR��h\̌�0XG�c�:����H�a�L�J�I��Ah5mn�=)4���*�5�sti-���I�����ITr��,Z��|�-�Is٩lJ�n�[fB�6_��~do��]�)eWu�`��㋺"G�bN�U����+����p�5�D�E���/�!}����*�3��7*G�@s��I�|x	M����T��c�~�`z~�1ɭr18&3ؼ��b��7!�ѳø�=ٵy��^?�U�%X��4����k<���Tz�ǒ`K�\u�����4�M�oWL&k�>ˮ��Ah��:}
���ػ�x��S�8:�S{�_s������V�j|ߴ��OX<�k�_�cqبW1���~�(��66
�8���4KJ(fSS�mQ���Ē/n��=�_�Ԥ씘]�0��X�E�ͻmFu5#�B���4�%r�R�3E��5C_T�=������h���/�n����d+�����w��n���3�l�仏�-��p�U�O�nI$z�sGD39n)��X5�u��az��ԙ]�B��ߛ�w��z(��~EL$�z��˙\��?��*�,m@ϵ.�y,q�P,�[�-���,�G�dm�`�H�=��v��3�F+����^Rk����w����	��Pv���<9QS�l�''�v�%���FW�g���ȣH{�A= �D�y	�p�m=���d��@4����0�n��ų�������� �At���a���LA	f��Q\��n���I�n�l~����r
3��m�k]������u��[m�>����~�
DS9���*�[��a2)�h��G@q�f@�u�WS���4(���uG��4���,M���3�1������N�oh3�Q�3�xp�}�n�?V�]�@��=��T���JN��x�t��&f������q�����@�e���C	���k������e����
��1�Oon�����huoJ%8�k��"������z]�lo�sV'�����9�@D���J΄Q1���ũ8X ��A������K�2�*߭D������E�����rN�AJ���[�\���!��u�8��?�*(���x��)��@��aݤ�����Y�f	����1�A��������إ�u��2�
�N�鞻����C�Kg%�6rr�}x����z|W�A{@ż$�x|}�G3��� fGg��^�9�GW���1T��.�Ǭ�r`�J�SN�c^��;8ܛ�c~�?"F���h���ג�̖y?��>R� P�����l� �=��vr ��L!�ĭ�_���\�6�8�Y�=��Т��-UU��}���Ȇj����L��[C: ��f5Z\I��_vc$!��%Gc��/]x-"<�T�vԛ@Fd(z�d)}D��$?�~XD��Y��݁�JQfQr��&���"1|87�S��8}�Ǹ!�?.�NhO1�J��OMh ˷5����NuUvIb,�i�����+���3q�/�LL�G��Ѡ�x����7�No�l&�x��r�g\3�@M����~'�������F��)o��و*�]��^4���w��[�����Nb��=�\��2^�r��e�B��@`����3?���9�F�r�<)�	�of6�� /C0DH���!.2�v��]T��z���~UP�os��;�7,����[�s��N('O�4z���"�50�E
�\�T�y9Ϊ�D�NK�{m�^N���q�������7��c$��b>�z[�0�_to�]�S���o\m/v�0���|I��藭�x"I�G#�*	��њ����<Vt�\ӓ�7˔��r�����S���9�R/"wuL��aT޿~�=�q��#�o��>�x��u(Ш~�~���x�3�)T�znh�3٨����&��P���;�X�~Ʒ;*X�{�}� aJ �`��1��b4��E�I�tȁ_�z�mm+t��3����kd���*�LmeH�	���L��۱��^[5u�h)��ɿ���%U���,��>��?~��x=a��L�H���K���M��ܬ�����͞���O*��ȗ�O{S�U�S��b	��@�}?�9��X�`�k���Q'>v�O`��<Y�/-9
�*����sp,��
�J-���;�?���P&<��d^�{��Yi�k��6l�W��f�"�BMx�>�[�ʕb�x��ԘĚ\{s1~!:(�TC�$�~_�$�c�ϵ`
N�#�9���q�~��2�4�����6金�V\"�=˸�(/;|z/�t�T�;b�)Q��������U��z��.�PQHZFrw�s���N��Y����p��c��#w�����p��PLs��ܯ�8y}��/���w�@��EF5Y�O��\�� ���=F6�g�@?�ܦI�u:�c;�f��V����+��􆺂]�1��GW-B�'~0z����h�X��5�k���.�,9jj�a����K�Tڜ%��vz���.Y�hLI��`���;��E]�������>i�E����u��tN�R1���+��-��J������c氲�E¯�*�����pr�u��<��t,T���H��썤�J^�q�>xd��ǫ��S������2 �N�́�:���:�<~���2�	�*���#/�`!�?� �c�ϛ��#N^..�^֩.�T�;ڝ#O���
<H�f�/H\��@]�R#Wn�2�
[ɩ'�s�f��������٫Z��>��0����m�,tb��\��9��`;�1�mŉ� S
�%�+&�)�3gŀ4$��7��O�i���Ѩ*.�Aƕ٘us�<)�4�C=dO�ٗƆ��Tl5(�j��F�̪!:���%Iݲ�q!%?��t�Ζ���$l��{��
��Ó&�+8�n��(r�Y�Ŕ'7��Jb|��iǭr�l84��)> ��M��z�=��'�	<���1Ѐ�C�ӈ�+���*�M����7C�8�S�%��*`�۟#�5�8�Z�Z�ݮ������Q9�0��������^����V��0`/��� �؇Lcq��(+�9L~0+��H��YL�_"���&O�z�"���wp�k�҄�~�}���u���V�n(�y55�ʃ
m��H�[ཚ�J(�5��fm����5�j�#�^�7���-�G������E+*[�e�e����IgbC�v����j�I�f3�08Æe���g/so���S�x�c���|!���Dw�i��h�in���\u�ިh4��XP�#}7� .���K�W*+v?��{�ַ�D�nkSS~1��60�j�j���bw��i���>D<Dk�=Ў	�M��;� F#�.�PiIF��D���"OAdeU9��{�߂qWN=�j�mXe��0�{"��?~A�Œ��I�)�c����J�˼��q~/+��r��m����������I�d}�3�܍��@��Q?���A�YD��Q R���1J����i<b�9��rvP%�W���贗�|�J%λB�+s��d��:�����=�t9�H݇���Y�#��rg�����:���R)��	��aX�F��I�"8��4Y�}��c2�x�Ҽ��	B��rp)��I���CCb�D`Dȡ���U�UB=J����ǧ�W�a��V/�Dm����,!����E���>;<yS兹�
>�Ɇ��� с��}�G@��h_n{߱����{-t\A��	��ԅF���t�Y�E_�R�%�hk��Ɛ2�:V�q��4��Ĕ6���[��r����A����\�T���!:8X0]�y�	�NU��ɮ7}P ���w<�x���'�i�l��I\))Ncױ6�M������*�8��w�B ''�]���rU��s�:�4�?�����sJIÌ��#�`�}��N�9Pp�)/�d@&!��zx��gT>���@�Йd�@к|
�Z�:]ӣ(w��Z����:]tR���������!�Z�kű~����vΔ-�P{(f?9Up9R%g����:����H`2���w��b�*���EH���P^Pe\!F�G��&�0J��煉�$���/� �- �z����n'i��*Hv����=�R9[�����0Eq|�@������G��4j��/��%�Q%SII�S��zu� �3k�fЫ�_�g��2�ߜ(a�x���Zc�2R$����;#N���n��h�1�!j��D�@�����z������UJ-`U�C�Ow�=k�3����R�^\���<���]z�F����GLE
䝚�9�h�O&������!��ԾNV��_�&o��.^Sy0�1ݭM@�s0i�k�gt��I�*�2��3��V5�$ĀT���4_��=7�^�e&���!:p,q�O��DqNwO�m�JK�ج~�|��k�W		(c��1���d���is�Cc���_Ӡ�'
Ϯ�w���R���-�/.�.��%�J�xB��ao3��E����DѨHc���#I����gS�L
���XԌP�v�<��Q⭯��q^`ɷ2*慌��(p�
I�c�,T�<a�૓R;���A�um�re_}��,������
���3��J�S���~���C�j�[�W�����`|JE�A��-0�>�vO�h���h�	�;3��#h�$�D3c��,�gI4r%� �O|���6_j@π,^�
�\�c9wH�j�*OP&�8Ԉ��ԝbz�e��-��ж���u��h�%&�Դ�[}_DE98]�L�����Q���R�ī��ll��"*/�~��f����N}$���M��+ye-��Ft�]�Jͨx���&��u��0��s��P��ԙ;�`/,Us?
��5�����������~��#�l=BS��������8�5�G2�xeM�|�u����(�")���4�>�tn ���9�A달=�{�dt����2��gHb����#�f�HXI ���	�7��+�t������b�<�}��F�ܯ��X���?���׼�dx`���2�\�q��<B����wc�(K� |��p�c����@'�8�"'����͸�V�Dб� >S�kbէ|l����Z��G�i}hF�(gS�2�����"]b�;<ׄ��?k��xR����	s!+�(D�����(&PA}���;#���|.m�j���̾���T�y�E����.�2n��oYf;�z���y��|�7`�%��[����Z��$C<�H�ܭ�k	�@SF3-�lBz��c�Df���u�%�o"в�J�1�X�@�Ø���;J��ѝ���K>ns�;&��v&�_< �C\[4l�������:�d�k�AȮ����_��D�|y����I~�b.��H)Ws[��V��k;t����Q@����B�H��U�!�{'�T�V���̒�p\,p/pvL����O�K��v7���ԝ�"�a�>�cw�y�ĉ�;���{��kZ�vE��׻h��%�5��j��\�O�صX9)zۘ�i	�Rщ�W��A7�@.��Jl�z�gV$y���d��a���ǗBJ�Fj�j���ʱ�Gt+���%��A�X�>���]�B�|0h0�5y��.IWw��.���nw!�2�V��^��epZ�B��6,V�%KF՞=F�Gx���¼w:o��+|R����f��\w���rr��4�$9�[h�QL����K�r��A�@�́�������@��T!�B��)���#��cD�����K�}�:ڿ�B��y4�~k*���j�{u�T�W�%໬|n�X�}���|B��$ȈTc�~�c� +��b�ݮ��W��7�jC�� �eE*��-b��o�i)T�hL,�p�p�Ѱ�mwZT찵��.�8j/Yx� Q׈�ϰ�x=S�.�n���^�#�vpL{n���J�=s^�G~
u����
��r�����'*��fsQ��4�+�V?�E���5ڌkl;�kSI���8��iX5��I��ur~��(�����T�m��%$бoǮуL����v������{x'?l/����-7rV{�*�^��jA��i�5IoՍR��5D�@37\әt���~�Mqs��\2��a�gJ���z{�a�R���YH���)H�p��nʊ/Cg�pƼ�..�m4�NZ�)8�gE�d9r��R�l�;�!���?������&i��l��4�}I4��
������C��`�J��.qE����c�miw�ݛm��+ݳ�� ��c@�lئ�Rf��䁆�Cj��S�h���jQ���ѷ�ݧo���q	Æ!6��8n�tȖW�w�"*��A��W�I�Uh�M+�z�5zCP����J
������Im{f�Ouc���]8d� ����Z���?�RF��j݅K�=��H(�ro�N$���ީW�<	�NQ��NΤ�u94��ܑ��� �` ]9�Ȥ/�$6��K)�i��aT) ?%㚄�?=aoՂ�i �o_�f��������B� r\�3��I{�v����﮽O !�P��O�c�%�}��1���:��O�BN��P���сV[���Ր~�^?���A:��ʱYd���^\���8@��.�	�QS��&b��Igm�����`��ע���)������o}2�Fb��$�= /�;�ma4��s��bQ~��:��R���`��4��D%{Z=�Ł&r���1�9����w�cT�,��ߎw�mN�K1�U�=�<0Љ���g��0ȭ='F�@j۱e��S`Be�Ś
�wj�&��(��d1�4�ry"��	A�U����Iܓ쐇�0�]?!�5،^��@>�-R@�|/�Cؖ*����A���LC�D����� ���[�ְ_׼�`����4:����1��V%��p@/_���	w���u ��K��4;��+�H�.@�b���a�e���P�O�#�g�$���+O�PpIyv����ڥ)�׈K�e&�Z
r�{�#jea�If��:��Y��#���!������$����_=�S3��ڙ�18�7���'��C��i�����(P�@pk�_%��:J�LL��Z�	/,\�y�[��=Wc��&x]|�֋��O������������"d�g��U�	C}H2���/vQP� ����hr;��yG�Bm@����E��~5��5��k�׌�.��G�7��i;7��S�yKn)l8�Ѵ#�©lO��jDz� _SC��&B�Hbk���?3��
&�fƥ��^�2fg�;��B ����aQ9jJ�i�������b��k��p�����J��8ψXyiy�Ԉ�8��&\�w�@h����?9��K#ex�W��)��> ������7��;{���ݥ��F�	FC$�dx��ÜX2`���k�a�)�G��1m�j�;�k5�tڐ
P���4��W��ͨ�d��(Ԏ�
�h��B�� ��Gc����0�5 ء�e��Ց�?��C��#1�ꗑ߈��h�J�Q),&q�gy�g�ϵ8�.�4 Ծk){����X�D���s��l(�� ��i�Rm=;N�넉1�o�Y�es&⥱I��gt��+CHtf'�wį�/�2b�X�Ac�cڲ�[è(mk���7�o[���uv�j �t}��y(��t�~���Q-��q\�NpE�o�`���
���8���[�?�u1f�Gp�����J�+U�mO�Sqӫu��a�0�[X��f����Xu�6ܚ=\�Eб���¯���5��Cw֤��;80��2��4���n������_�o�|�����y�R��^t��ź(P8�r��qь�2b�N���G^����w�̎��p= ��?©�Gj벧xy��`�}�Wk��!��Zpwe��-N$E�� \W��������ÆR0�k7�<&\D2��k!�/e6�>��Tœs�00Q~\�L �jcD�핽"D��*`JǞ��gE������Z�>��<��������@��V�e0p��|��Ee��g>����������Y�T� ��̦�-�j^�Pv{6������
����W��Ҧls{�@i��x��B�e�q!�d��=pwD.�U�O�>t���$��4�\���bs�C��V�9�<�ԣ��wG���+��,5R�����T~n��H3`��5��;�$��&~q�z�nߤ|1��&er�
��"K��S�UN/r�|3|&/���gg�DzmG�9DRR�|�0- ��~\���x�O=��#�<���������4/�v��E%�}��O�\�wC�U�T� j�S�<���\����qnd����pށ�Xʔ�O��/p/�Ne�Բd�����)v3�q����%�xvi��=��	U���r��Q>U�0���,����&�>��v�s����(ī�v�-��+N-�W�3��ȡÏ�{��EU��"k`ũWخ捵@���s���Y��˸s]�V�g��02M��i�<�/���ȶ��u�L����W�԰4��v�)�,��A\O3B8�6��.�r�R���5��L~�bBhj�>m��_�r��3�΃S�=���s� +���Qs�~�U�|`��r�b��R�~�+z�y`�;ϫB�H�)���G��?��s�6�/&� 릶�� �ɞ�L6��S9{`�T(и�|�&B�+ǻς3)v����N[D�����~c�^�;�h=i�ؙHRr橜��A;<�]SB )���U �.�[���,��eQk��IU�֓���\��tîֆ�I��qg���"�o�-����a:.0Ė1����}�U\YO��.��??y7ǭd]��7���	����������C�*�@}z�0 	V��)�����D��~-� h���	��JfA�L�|�x0�g�0j��R���wk��r�y.�
�n������/�f_�v�A~Ǡ|w�Fƍ30:��t?2-�N��t�5�S�6�}�V!f�k��+ԕz�&�a���uSD7}��Mw,:�w�i"��4J���x/�2��>��ŖS1z�ǒ��(WI�㓌�+PU�jy�bv�r���PR:ǈ����we�?�Cn�xM��7xC��҈T��jy�K��)��%>C�",��%�Fi���du;>���v�FK�0W�h�Oj<��Xs�n�Ӷ<��
�+;���wb�1�]g�ϪɡW،r�lP5�t����H�]&�E�fWu ��m����E��'V���Ƞ#��7�12ZOvdVg>��~Rt���b(B���QnУ�i){�_��	<҇�$�����dQc���I�����y=ˎR��/"�m�
L�� �|���է���
��=M���D=s�'������8H��%�6�BDK�,�ʘ]�t�T�ޜ)W�+O��[�+�����wv;���Ƒ� b��g��Ds5z�T3֛��Ӣfw�����ڃ�(�g��ud���ǫM /�[|(�G��m��p����~՗߹b�g�X�B6�5P��!��]j��jy6&ɘ�=`���>J� �-dI��U��EGg@��k|��4�㺨��0��� ?���`_�A�3������ss�1���д2��tq��92�ڼ]Xnt�4.p�^����X�myQǰ8t{�{�^�cC��5j�d��	�юy"��N�+K����6��i0BNG���GG#4��:wKi�7pܿۢHI �qC��i�8�����/�.���	
@!�!��"3��P,?t�(�\z��q^��R[���++�*"�qF(�f��|
�����?Gr\=�Ha7�'�]b��t�y�x9���S�m��{�[q�)d�O��G�vK�[
�V݄��'S�\�hs����;�6�����,�����1�e`K����\5��4@E~QЦ{�)��fT�s�i�40Ɣݒ�ƫn��a�R��o?�M���՘H�n�f���?�R�����ڑ���K*d"���J���軹 �{dX��އ����_���I1�W�Ǹ��ґ�4���YȦHAr�b�D�;#Ƙَ_
h������sn��M���\CC��(;M��`̨�AQ�mS�(�g	uw��w�	b�W��e���k�Ksf<T e�*-���|>���,��0�D5�߸w[ʣ��#ht�0�@�ٞ:S*��Ee�8=��G7���t����`�Qt�o�!����G���%WJ;�^�=�
��'V����DCu��vB�2S�`�v������i`c#Q}����窾�D+؄�BñI�
� ��#�糀�	w�z#�I��@8��{`"�8!���vu�޼��n2+����%l�/�#y��P�\���ZP��֑�i�f/�oի�޳L�7bʬ�߭��N��t�HT�b���G�k�^�6����Ɉ�;S�][d0��긓K���]Q�в�~���� �x�r"'yn�%j)�����r�[�5t�lG'li`2��������7�T�n��=�{��0OǸ,���=��c<.��̟����db�ڒ3��vF��"ر����Z�<� Xn=?ᐟ�-+�X�X[�Jί쒱���g���	P/R��&z�y���=\����2�ŋ�tSh��ۭ�䳮�l����>8�o%�cV�r���''hZ��T�5 ctD*��&K0\���~�A�%+GV�Nw�$��c����*�%��3|�ՠM*����g���������I}��AY���mO~�7���"~�F��.%w�vO6i��k���b�(l{ԇF�X� ���	�YfQF�5�2�'�MG��jZ$d=ot8η�i#ܢI�`+�I%�z�*�r�R��?}Z�!38�q#�L���u�GU�o�1o�E ��e�-�x3�̷��1����8�sȎ�j���c	�+���C�,��p��;΢������S�/�7SZfPJ�OX����J�����}?zrBT�3�.�H#����t3���ѕ,���H�Y����Rpb�� BO�6����	��}(\�+�.z���T�Y�~^����_����7�W���,џ(���Q��-����z+F����}Z ��ź;㐷Ƅ��*�&����Z^zr�6 �9��S$���b��Q��`x��z<A�7����q2k�s�&��zw�6ΰ�I�Ǡ
�c�����Qou�[I J�O�D%��:�oO�y �<o;a8ْ2_�`t����Cɦ{
.2i�����D�t�R(J�S�+i�b��ǔ�BA�s*NV����O#I�1(4K�;\�:���āӑc�zV�gё�5p������۾q���$��_��
k2�)����lvؖ뺤-e�:�э�v��(����1�<y��`����>k&�(W��aM��A�Y[�MGvu6PU&g�#�j�z�G>���˶�iّ�y�������Y�
+|jW	��
�={� ��7艝�-����|�޼�u��N���!��I�:+���Ǉ�Q�0�m��_�O���ociꇯ�M���Wp��Y`�w�ɢ�g
=�ǈJ��YY<��,t?�o8�>bܦ���;������;�J�8#qy�z 2�T��ń���#�Y��cH��	�Ū��%
q�+���9Aj���X���Sf�,9�~[쁪�e��nKP(���0���l˽����9K��*��r��_�QH��R����2H�>�9Ȯn֎EX�r����EA�˭�.!�eB�"�娮�d��,�;zܩs-9t�H����Hȑ霩�Y�����)`���ӊ0ӱ�����q��	c�w���2�hV�\�W$\86`g�5�[5�%Y-��4©��ME��W��D�N���M�3�z�}� ��;*F<�'��b�|��#��m�R�=�=k<��8�g�뫬W	%]��)��ݍ����
�B>
��EF��,Ǚpi�<j�U���I��dv�x��M.��W5�+�
�m�*���7��dGT[A~�P�N�0$
;��W��/�X�Z�T>�Q�Cο��&X�w~���8�*kG
_HȺ&�/9�*��	�UZ����Ʒ���Qd��	p�b�5���Kcի�����2^�z*ϯl���l���䛺sl�9�t�3��MVR 3%0T���O:{'��U�հok��YҏS�����̢g׻/����.0��;��H �%�e���J#Oy��h��b�v}�S�)Z·={�a�2tg�q*ڴk���T��3kL�-�ʧ�V�C�J8����^wU���
�1q�Ԭ�,���C'���&K>��𨞍�o*/� ��F@�E��u�wG���
kE��lf@A�3q��`�[��n�h�huZ�˼��_}`������O��u��ޘ�x�H�Cb9Ÿ�(��UH���zK�)��ǠXC;�����h	��?�Y_�j�۱Z��8n�4�5�"�DK�Dg�=/��֒d>��*�U�s�N~^@Ti�'�A=����1�x�>���N��־��f��M�r�8i �ߔ��vx���xy��w��C����o�=�8f�Z��b@��O%A�L&|��	�����@w 3)���j��i!׌��r�S�;��n�X�^8�S�WO����(X�I���-�����g�+s*�h�p�Q�4\�*�ͭ��fi7��,(�p�n>���e��D9���:D�,vv9��s|3_?��s��01���>��+-�����L���xY*�n��D�t�'�*WtּM���@��򯴗C_`?V��^��x���w��݁���Řm$P��:�!yM��証��Jv	��zg�B��В�큦i��m?��	�iI�yg]%�Γ7 3���g"�ax���|��#���B�~��k?	c����n\0(�Z���U��:�C��<$��ͨ>�{�r�2&�I���v�\>"���:�*劄�v��f?�hEA5��d'!����1}�66�"b_KTIV��$!�%��
g�.�N�my-�3�.�[������U�!��%ʩ��ܓ�$Z�z�'�}���9`��f�}�*R��i��<L:$�N� �־~I\w�S��[I$��"��.��i��X��{�3Qs!Ro_�
�a�`��M��Ԟ�AT&]Br�u���'=Ê�S�uqe���uiK� ��k��[kW
��qDf���h+`�&|%#q=���Q�*b4�i�O{{E�c���(�~�:9ERƛ6��Ђ�9���O{�r����m�mMO�B����EZ>���K�#�Wu$Ni� �2�nَ7�� ?Ө4h��	ƅG,�C��r����ț�|0l���KB��v�e%���&$!��;�� W�w<�#v4�	���`� ���`�L]v��J�,5z�c`���;�~DsH��kR�����)t0�_�pmfA�s��I�f�)�ʕ�l�QЫZx3$>tbN�~VN�k�'Z\�\5�
kV�B�E�J�Ɋ��7������2�	�Wٶ.�Vf�� ���:#�x,Nֿ�2��Y�G�7�P�ߘ��Epߤ����k��L[��,��t2���赵���N�������~�mG������T`�E���Wڪ�i�g�_��-�a�夓$�i˴׾Ҧ�Z���j��ٛ@�;��Q˪v��E�Pz��M���;�OD{�A���W����/�Za$w��3_��Ha�ڡV�,��
�B�g�G�MN���� �"��Z����h�r*��d�U�Tޢ�n���T�iĥ>g�B��D wf/-�A��! �_!����r�ʤ�u��W�[,��LC��q�"t��+w�Єح�����
[��| �bR��O���wȗ\�vk���Co����MW築l��=��1��m��PD�z����F��#��H���F�8��{�����@�(zzL����'~�e�4��2��yY���?�Bw�m��ˢy	X>��u�8!v>:�������Q�-|�՟�S��3��_E�*�	?�JZc1��4��W���7��*���y*�/I�ˌ�7�"�&3l�Xw�0�,8	7��_-�C��r5)RuM2U�8r�q~Fߐ�Κ��%u����f�ǀ��͑�7&E�}�`���ۃxq�,����e]B�ߡ���<ߔ�����BSH�鉰�4׮#e���6w&�����&6�lm+�[�$Yh �_�H��-� ��R:i'�M�B[*�a��s:]�E��Q��'�quSgw�x3&4�;�=ȋQz^P1�:q
�� q{]f]P��7$��@c��F71��ȶ�N{+�)̞�3��Z�z�S�ڵ�t��@��/��(BTB�7:)ձ�(W�|�ï�R�����!��I�T�Tg�ay���|*�1ݡ���xT�,��f��
�r�TVc*���'�lKB]C��(>���F��B��y���D:1B��-(�dy=!�AT�*6'E}c��n��H#v�{��s
Bj`{�}C6���w�ʗ�j;C�)�{�#R��L9����G�ϧR O�m�]=�38:k3	�6a��O����zI4<�?���ZiH��?I����J���v)��*�>>C�B&�CP�9cp|Uf
�|B�S����!�1Y:����O���$3<?+tM*(݆�?|V�0L���!�|O�>B�Ӷ �l�i��Ci$�>��k���-�\r����ڍ���,��A?��������n�1&́�>B	03E��U��0|VmtcN�H�	*��LBo)�t����[�!�
-Ss\��1�h�l⏯�w�
Y��(�4�1��<5��D���M��o{%�+�ߖ�F��\�qb�0oN��-+K�΢�V����%ui�q]IޔH��(��f�b�d�oꜗFU �	%���&��T���}{� ��@1�q[�"���&����>״^�aS�:
�Z}�z�7�/�Bpv�
M�� ��bv�χ�ѡS�|��k��顆z�n,s����Y �0�/߮-^���*���*n�rm�Q�������k��H$%i�^Ly
?��o���r�=�?l��dXJ,Մ0٫�d$yZQ��� ��lLzR��kNg�	�Q,j(�"�ȷ1m�����j��|v����`|A2vk=�J��K�w8�T����d4���z[���>m�ڬ�DV������n���rT�t�B�P�`G��o? ���[�W<��⏥�HO��n��!e���)�D(z
;��'鿏0r;�,F!����+���MD8���C��j\V�䪖*V���E��:��C�b���7!:w��À+ɾ.��w���s'�mƶ����2����o!j~���-���!��j�l�4���PJߦ�旂װ
��g����UD����fG�����TY��ƯH�|�
�ޒTr�ב�I�J�]�p��hG��(��W�0�(p"yP�N��+^�a�.zQo�bF�>��'�j<F$�$��1��q�V��?uDe���@�����̲Q�Z����A}/{%��^�� ��e����c=�<yϗ�X�������!�^���H�P.�8��WD�!{�t/����3?û
�"���ڌ� �l�������ѯ<�9�!p��2�н�e "փ�5z�{�w�L��~g�l��n�MP�Ԁ���90"Ei��*8��B���������d���x-�)�U�>����%mu��v��5a���H���v�ߨDv�jvf�����m �a���+FtJ���CoL�d���A�-�Ci�A���[��N�� ��:I1O�9]{P�r����U)mL< b�q���LU�s��9�Gq���ۘ5��h�oYa�Ҫ���7VOx��*�{�{��N��~���T�ˀnºL��uk���L/3�:q��p8�;��uF�/�ف�ig����|ؑG,��}a�i=*�B��-�Y$��y3��,Iӿ�ۦ�ܓ���ޕĨ<�ӵ��L�Q�	��ゾ�o�Y�		���+�U�f!���O�4뱇��>����A��W�[�j�7�v'U�W�K�wl-Ǝ�Fv&8,=P�ֳ���ׄ:;�TJm�����R��o���M�j�������'�!��Lm�˯��\9��������_��t�X*t����9�*p��L���t�ʨ}�Ƅ�'�R+�҉f��X�t�0��(�liZ��C��t��CY��R�[��F	�y�
Ѧ�VW�����~ÚY���w��Yn���P�F�у�^X���(0a7�9}��� mvSMN����$^_1������(�4�D�z�JJ��lcڕ}ᰉ	���Æ0�An�9�2K��E˺�>4l)_j���b����2+�gKP��&����i��h��[��.Ha�����\-�����;6&<�x�Z2���D#B��u7�i#lc�z}]��0��LW�Jh>3���g6YyzW�V�%~�k(�H�I_62/Y��^!ڦ��̲[������X��|rl����P��9UDj#q�B�ք�� �3�� T�|<p7㪼/�~�'��?6X�.Ej��>�i>4�]M땅Ocis:j,t� �2A*��ޓ�f]���׎�vK3>�21irԸ����bN���/s8y���X~�ah8t�!��}I&2���^L�j%�n�މ�TN�G�E�ԛ@��s����:�6Rb��Ae��܎����D�+���i�!�e�[���'�Z�B/t=���G�{�q�b�]�ґ�б�w��U[Ǒ��ҐME�kW{�y���	�����M��l��k��,�O�-�ajZ��Zy���O���j�r��O���Q?�8/�5׶��0�ґ����O	;�`�	*�pY*m��N�����v����T��Ҟp%L˻^'�̗<�	m��ߜ(��]e`D�:���+7![�麞>�*��X7~�^�r?�h�Θ~V�R\���q6��Ty~q9C��*��8ԧ��Q�,�DX��
���3�
�G��X7�g�L��+F�R�k�u��ȶ	�7�+W�|>o��D��{��� �� �R�	�i��[�6�r(���"��` �#7g�����Nkq��퓴/�f����� �mLR�C�
o��h�hwb`m�(�%ᇓ���3l�{b#�Y��E0vۃڏ���Ȅ��QVڅ���n�߭�M��e���QG��5s�w6ޗғ̲�ϳ�A+.+N(���� 퀇�++p+�:�f�(����U�9F}�݂��v�n z~�m�����o��0�ג�P�T��ݔ���F���-Ҳ���IE9L{�%����ԧZL�{X:=W��y��~m�����b�h��r�j����e�\`w��ȧTx�']T"�ǖF��y��#ݬ�A����*5������
��0�В�"��xw�=�����Ρ=ߊV���}��=��_����}Y8�o$�33pr#�����!l�=�i��ŸC���ƭ�����=j�*��[�-W*	(�. �7��_���ώr	��:��oW�d����#�W��=S�-G�Ή��(j;r�9cޱA�.��D^��f (should_resched(0)) {
		preempt_schedule_common();
		return 1;
	}
	/*
	 * In preemptible kernels, ->rcu_read_lock_nesting tells the tick
	 * whether the current CPU is in an RCU read-side critical section,
	 * so the tick can report quiescent states even for CPUs looping
	 * in kernel context.  In contrast, in non-preemptible kernels,
	 * RCU readers leave no in-memory hints, which means that CPU-bound
	 * processes executing in kernel context might never report an
	 * RCU quiescent state.  Therefore, the following code causes
	 * cond_resched() to report a quiescent state, but only when RCU
	 * is in urgent need of one.
	 */
#ifndef CONFIG_PREEMPT_RCU
	rcu_all_qs();
#endif
	return 0;
}
EXPORT_SYMBOL(__cond_resched);
#endif

#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#define cond_resched_dynamic_enabled	__cond_resched
#define cond_resched_dynamic_disabled	((void *)&__static_call_return0)
DEFINE_STATIC_CALL_RET0(cond_resched, __cond_resched);
EXPORT_STATIC_CALL_TRAMP(cond_resched);

#define might_resched_dynamic_enabled	__cond_resched
#define might_resched_dynamic_disabled	((void *)&__static_call_return0)
DEFINE_STATIC_CALL_RET0(might_resched, __cond_resched);
EXPORT_STATIC_CALL_TRAMP(might_resched);
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
static DEFINE_STATIC_KEY_FALSE(sk_dynamic_cond_resched);
int __sched dynamic_cond_resched(void)
{
	if (!static_branch_unlikely(&sk_dynamic_cond_resched))
		return 0;
	return __cond_resched();
}
EXPORT_SYMBOL(dynamic_cond_resched);

static DEFINE_STATIC_KEY_FALSE(sk_dynamic_might_resched);
int __sched dynamic_might_resched(void)
{
	if (!static_branch_unlikely(&sk_dynamic_might_resched))
		return 0;
	return __cond_resched();
}
EXPORT_SYMBOL(dynamic_might_resched);
#endif
#endif

/*
 * __cond_resched_lock() - if a reschedule is pending, drop the given lock,
 * call schedule, and on return reacquire the lock.
 *
 * This works OK both with and without CONFIG_PREEMPTION. We do strange low-level
 * operations here to prevent schedule() from being called twice (once via
 * spin_unlock(), once by hand).
 */
int __cond_resched_lock(spinlock_t *lock)
{
	int resched = should_resched(PREEMPT_LOCK_OFFSET);
	int ret = 0;

	lockdep_assert_held(lock);

	if (spin_needbreak(lock) || resched) {
		spin_unlock(lock);
		if (!_cond_resched())
			cpu_relax();
		ret = 1;
		spin_lock(lock);
	}
	return ret;
}
EXPORT_SYMBOL(__cond_resched_lock);

int __cond_resched_rwlock_read(rwlock_t *lock)
{
	int resched = should_resched(PREEMPT_LOCK_OFFSET);
	int ret = 0;

	lockdep_assert_held_read(lock);

	if (rwlock_needbreak(lock) || resched) {
		read_unlock(lock);
		if (!_cond_resched())
			cpu_relax();
		ret = 1;
		read_lock(lock);
	}
	return ret;
}
EXPORT_SYMBOL(__cond_resched_rwlock_read);

int __cond_resched_rwlock_write(rwlock_t *lock)
{
	int resched = should_resched(PREEMPT_LOCK_OFFSET);
	int ret = 0;

	lockdep_assert_held_write(lock);

	if (rwlock_needbreak(lock) || resched) {
		write_unlock(lock);
		if (!_cond_resched())
			cpu_relax();
		ret = 1;
		write_lock(lock);
	}
	return ret;
}
EXPORT_SYMBOL(__cond_resched_rwlock_write);

#ifdef CONFIG_PREEMPT_DYNAMIC

#ifdef CONFIG_GENERIC_ENTRY
#include <linux/entry-common.h>
#endif

/*
 * SC:cond_resched
 * SC:might_resched
 * SC:preempt_schedule
 * SC:preempt_schedule_notrace
 * SC:irqentry_exit_cond_resched
 *
 *
 * NONE:
 *   cond_resched               <- __cond_resched
 *   might_resched              <- RET0
 *   preempt_schedule           <- NOP
 *   preempt_schedule_notrace   <- NOP
 *   irqentry_exit_cond_resched <- NOP
 *
 * VOLUNTARY:
 *   cond_resched               <- __cond_resched
 *   might_resched              <- __cond_resched
 *   preempt_schedule           <- NOP
 *   preempt_schedule_notrace   <- NOP
 *   irqentry_exit_cond_resched <- NOP
 *
 * FULL:
 *   cond_resched               <- RET0
 *   might_resched              <- RET0
 *   preempt_schedule           <- preempt_schedule
 *   preempt_schedule_notrace   <- preempt_schedule_notrace
 *   irqentry_exit_cond_resched <- irqentry_exit_cond_resched
 */

enum {
	preempt_dynamic_undefined = -1,
	preempt_dynamic_none,
	preempt_dynamic_voluntary,
	preempt_dynamic_full,
};

int preempt_dynamic_mode = preempt_dynamic_undefined;

int sched_dynamic_mode(const char *str)
{
	if (!strcmp(str, "none"))
		return preempt_dynamic_none;

	if (!strcmp(str, "voluntary"))
		return preempt_dynamic_voluntary;

	if (!strcmp(str, "full"))
		return preempt_dynamic_full;

	return -EINVAL;
}

#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#define preempt_dynamic_enable(f)	static_call_update(f, f##_dynamic_enabled)
#define preempt_dynamic_disable(f)	static_call_update(f, f##_dynamic_disabled)
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
#define preempt_dynamic_enable(f)	static_key_enable(&sk_dynamic_##f.key)
#define preempt_dynamic_disable(f)	static_key_disable(&sk_dynamic_##f.key)
#else
#error "Unsupported PREEMPT_DYNAMIC mechanism"
#endif

void sched_dynamic_update(int mode)
{
	/*
	 * Avoid {NONE,VOLUNTARY} -> FULL transitions from ever endin