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
		ts2†”ºèÏï¶'AÍ
µzCİçŒŒM†tIá¸ 'ì±ĞÄFòˆŒà+ÑÌ¤àÀàÚ¾%NÔK¿9d„İ"ó"áHx+¨j#åHn÷~„‚Ò9Œ®_Gzø¦·ªóEU›Q Äª	şÚ!"_UúšEË×mCİ:GÖÊ$Kî®ÕC7‘k²#9/j°ó¼pé0‡äS¨ø–~ó2Õ×‡šî+…XùÅ¬ÏûÍ!¢ç¢€[Àù-“ıİxètd.äºàêì·àS:‰Ê.ªR%“ÆM`W=:dşuUT•Ï©÷u»´«Dˆ…¨*…|P&Bf¾y#+6_}xŞDy SEÀhùM­¼£ÅD<}ÍOÁR¥è¼Íô	Šña"=ËÓQé¹¤á’=Úè:Z8Ä(â«¿ßÿlÔ±X©ã.œµ°c% <ù}rJ×”
~´fXmÅŞ9¥nÕhù'™†4rí-"$ÈV=Šš“:šOYÈJ÷#aì`Y Bõúú65ÎÀÛè’É©‹Eçádî™q.Ôúÿ‹{ü KŞÕË4.IığœÖ¡ø6a4ä„9µZ_4\!'|kÒÿÿ·cÙ
<&	q6ÃŠH-óÕîoª±6ñ¥öãp_äÃFëücô¾Eµ†~Fª]®‰¾Ö„ëİP-špş@Ü	.ù‰Ø?  ¸sêÇî³Ğ{]ŒTPEÃ	¼ëEKz\š¸^1´]N*Bq2$H…¡Å#94hxK´Ù¯—Æ‚É‰GµÊĞ—‚„Möª@¡ÄµJÄş€‚ê¨"„‘[~WdV‹KıƒšÓa]o”2kx&‡•jÖÛÏ"ggİ-‘4ÄZ%i9eNV‡lĞ(3'¡Ôe›“T½'épè6%aA™1ğñ®éP¤mö=ò×ËKÇ–Ÿ`”5ÖÜ5†÷½U÷¸+ÜƒÒHÛÊ/
U¨Ü	Î§-ƒ|ˆ[9bŒÌM‹4Y“"˜ôB¨Ó°Š\WU#T¾vå$~ƒÇ_¦	eÛ†@\Óí0Â-–—	sŠíõÈ€ÈTöÌ”ä2ÎóÊ|Q„dğµ;‚	^ µ’™9k®%Š‰ö1ƒá8ã–8Ah…ØÎ-Ü’Y¾_0å?/­¼K`şj œı0¼átRFmAYö¸ƒ>‰•¬¸$¦jága¹Õ§{ìù—·¢=èô9’¸‘6i£Î§¤î?ò+šQ¤Hé¨=‘5Á/`»ì¬¨P(ÏÀÀêÓõù)ÆX™6œ/q<ü·këU¬2U¹ŸÃp‹¢J-£fF:s±òs;\,?i{.YÍ5á‰mí9…ŒN¯‘°¦›Ùø9_‚ìj­,¼ŒDt°Éq‡I}Ãºv2~9lµB¹… ŸMÉ™ärøÆhÔ°ÿò«gT2Zˆö&rŸÕëÌ>}ˆûêÖ|á·:ªHTÃ¾O˜ ™ERÇ" @5`= Ş¿ dY¨üt#nûæ^<&ËRxÎqró•Ş£±¹>ëÇéÃó@â­Ë¸ˆãñšzlænpkX‹XÄ»_`òÖæ|!R¢‡ê?o÷1e@˜3vÈ¦²ÒîæLXË@Q… ÿv½æog}}¸0G´4W1Ú? •/ö=:^ÒI¿	Î¢ñÕ¸ZwùÿU¡"	K±¡>ŸI¤Ï™×Ä\qr}PSG±ãAb<Ç-™Æf]¨­sH@{&OpúH.é&doÂÀ5º‹ÎzĞ\È•õ)Q€_Üø:‘ËÔUÌ ¸ìRŞZÆ½”ÌAó#v¯«B@æÀv_LŒ—å\~Yã&~¨‘İ|ÆC„¡\Î¥~
yàilQcQç0g·°Ä`-yíEï·h#îğ÷uÇƒ‰ãÔ¼l,ïWÈèOĞD,[³#¡_ù¨$àˆ„WèºÒ
!´Ø©õÀ“°¦	£öd’„×W CÇÇ²Õ6Äã« n¬†Ö^·ü•¬ªâË6hhƒñ«ùzÇ¬D]^ÎŞ’Q=i+cÄLQ©,€Ç8óvš‚°ËAV¤)°s¨Nÿ)WJåËk¡ªÿ±k)f4Víàe ×™ØP©òT€í… ^
Vsg·Ù«ÿ®`W¦¢:¥˜£
FŠ]c WUV>%‰’…ò£úßĞ¯ù»NñtaœKEŒº³’+¤Ë¨„ï¹û«‹¾=÷ ™*Ó¿¹U?°øx6"gâzŸoéç2ÖO*ˆtE¾¨ á!™³%e}ÎÔ‚1{€ n„BŞw†‡«¦Èê©†p aíæ:…¹ØÅS¥µ<êu<­z^|œx äKZ
Ù™ª`,2"`£µı·OŒ˜­’‘3b×|éû[àH–\ä$5š#%Òıpºs†øáÜüHãc­ÿ‡¼@äßCÿôÀÃ^·`GRL—àÎM†_')¡NŒØVÿy§"G”íö¢ [Iˆª[Sq±r)ûó ®'0ƒ\ñ?‰[>;(Â…ª!É)dBÊ#rÊ¢s&5C±»¤V…’Z©
/ÍfÍ<tfÏ&>¬èCãdÕããœv# È{eô¢•òèçw–g*×ş‰Á÷8`ş!òJÀÓïR¹Á¡E[yPQÉW!&%ÜM—n»DO5ºQÇ½l?,
Pì²û8tË–´28²aşTÄ—"/d·İÇ±áOÍ¢Ù8zµ¦·-X%¢l·ú%„ BÏÌ Ì^ÏÌõ¯/!JÁ2Ô·ª,)	û}¬¤¤ğÎM¹Ë|rš_gLwCy±Ï«ë1®EAêÅÜbö‹/…a›qëèşYÛsÌéiR2vL†fa¥{µûÅMÃ‚›JŞ¹<µ¨/jÅQhÚi›[^;Œ<×yÌâ‰..jé[0æ°ì­Jÿ;JÊ?Ì·ôğ’,uİÖ¤"oşl·âÖ¹càĞhï„ã‹5ùôŞzR­êz?öôGé"Ëò(ta[kúoøÉBh¨æ4m-Du+Ÿ‰z§+ÓmÜ£èHÏÜ×fïè¡ÊE¼„Vô²ç=¥Pp±úÏ>
eÕÿ•K#‹P2‹Ì««Â
Š¸8rA]oRA’S½Mn%Î—,à_¶^ÅÚ+W2
Êqñæ:¸ÙªÈà€Äí$`»î1aåÏga+¯ĞÖ±\“‹Ğ‚}6”\ı¯'ŠMWRBû‡ÜNqŞ Ò)÷½Ñª'Ù1´ï=`°ñ˜NUùø2sÎs×„¢Çê$¨¶«Ñ€<Öˆ¬& \òöN†±/#N’›²­şLÇŠ¨.r*½ÛgvŸy		FfÔû4É®ÅøXÍpi@Ï„œ™ 1i:©:şË lİÂ4("ñ<P8¶pİuèAşt8	”g<ªÑaĞ	î­ ™{óCúNˆ$+Æ#	bï4pæ—0qµgêÜÄpBˆ'l¬»Î±šW+œHğ.¸:˜¢Û²ŞÆ3!Ÿp¸AËÕ¡·3¹²²¤€¶EQ†j…]XoLMÔLãF†ïCYÍQóÌD@TÂAlæ¼¦­•‹™a*ĞjÏØAÓ*×º4J 6ø-³dáê¼¾İû<°f%Oÿ_eÏ½RCùçFŠÿ[Uúgxr s¡ƒs1Àçuñn¼3‡ÆÙüˆ„Ôm·ÔÙÓe%²„£Çcœ8ÃÌ7ƒ~0«=%ÛµÓ
şÊÂ¤³QÜ+T¹GÔ¡_Pç,l®„¨Ìİ?oı¿Fr"b)ÊÛ.Õı}4³2=x€gÀp1¸­Ñ0)FÑGşÍş[_7éĞK¯Ÿü}¨ºTè)°¿/¦õF‰¿á€@2 Óç.ŸŞ7¥6ÜŸ¯Y¯
f°L©öp‹bÏÂÈ=r=F9xw$T‰”³“ëtiTnÿ­dEŠ¢8Ù”‡•©»vôr=ÚOğPaïB L0Ph×FŒù¥=¹ÙÛøŞ5²ÉÕ'ú¸7v¡Oâ=b–Ã%`¶š!›39\ïÀ¬o®OætaÂæ»Ç`oå†c×‹«Ë|B³=y&*ş£ôG"ÉbG£‚õ\i‘üéæêIgûñ:¼L~ …½/3ë*íRİaãÑ‘İ‡İ©%H±™‹Hïjj_¾%OØˆXİ.u½Óêxº`$\}óÁ±„-@ bxO²Y	AÇÂ«”Öt[TĞô¶|²È–
gÙßĞ&€´Å)n$çe¿cw¹DáBod"Œ®AqåšŒ+_×
|ÔŸ&çU«Ñ8íb‘moù@(díÔğ;–v5s*:#C!`	.c6İV³`xşF°¥“Q€H;Q°e.ÚcñôñtÄ‡7$ürBµÿn:ùãqÙfšxõã(j¾ö_WK2òCµÀ½+›©*vyˆë£ı<WãFI‰ÿü±;ÜnÚ}º‹J)wx¨±FƒÙÜİègüİïÁ¯r}ŸœÀh¼Ãr§Ò
–;M~6%ˆJó„ÒÿÜVÙÄ½\†GV!x¤ÿSv¶Qlµ¨H–U(!”’’ Ò•¦¯ÚrOTœ&g¬AøñÙA6BJxz8À	µ|ô,k‘	tZ„jYbÃØô5Skue„¢4Øûº¬0®ôa’Äòh‹‘:6R³À0œÑù»›\£Ï¾•íšìÅÜù³Ïïœà¯eaNÀ½JÛO8wÿ÷P®ûÑKPoÎÙ¬(êf9mö
%±L×ñ£»40©M‡,›Á_´G/0Jõ#Áú$„!“8Ó/'$!N¡Îúj¾‡cz¨`\`wB:FË?Í&~Ù1i»t¥¾£Ú,òùn¿‡zyh¥­İd%*ËİŠµ(Çd÷™àôÂ][/Œ˜°ÉÙI…(¬é±ä;>FËRå(\õ¾‘©Õ›#¦<«/¸„<ğ°iÑDæŞâ!ì>x°˜ˆâ¨<¬l.nÌÔ*A%»6mİe¹*&øèÂ÷QÏÉ•Í±À¹ÎÌĞ@ûw›÷ù8¥ÃHÈ2€ ±`ærĞã¿ŸW¨µ VÕÆx”ùÇjÜ¤éÀW¡Ê°üÛö¸J&à9±Zì¨œéB$gÃwå+ô”eÉãv×
frìş¶ıø/í %(Ø™dèÇ=¯Şã Ê2èT?-?ú¹.öYú%ˆõh¾ùé+?—GAD~ Gíğ­uá0@Ÿ%´?û®Š#OÒØùå®ÇÖğòóĞšpp»m*ŸkysO Ìpµ7Úo²¯‚œÍÂˆı²Í¯Üµ§	Ì]G*|‡v@qYIÿ¥¥Ç]µ±p;éö>Í
Ú’I`°Äñ‹nÌÁvØğØ…IDEO;
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
	case CX23885_BOARD_HAUPPA›û˜ÌEÆMğ·AØ¦^7JˆvàİAí´=·s¸éq|J…5º†÷Á&1JM1½êz~G˜»¸¿ğC£Ë#¯Ÿ7ğkö’GZ¿–¡>¾ÛOù×é£´â.%šfS·z½j=ŞOÑ`Éİ¡ëVgVÉ©*¡ªYCYEiq‚Ü¢ß/`™ŞAš½I0<YJCtÊaªNH›ñ#ÇYÄJL¥u¦µSO£Vº°\Ì¯àsÕ^”ê¢¼¯#Ÿ(yq’¸áé›>ÆwP İ‰Îä¢À–»íJKşn¡ô ñU(5Ãgıµckèôé~ü“²US°†§nøfnÀ}J5Ê%ØŞ8*ƒ¢èãZû{?ûã,ÄB™Œ©‹86ıÇ·Kóğ	ÃCBàiõˆÍ‰^%ïÎµ/vCÙ†›Ô[(ï™!‡"[05gfƒÄ©K
>·³o”[s“6Ğ„´ş–¿wÏÆ«-l^™C?<zb¿£Ëñ/–I1ÿÖ‰@!‘9ı§<H²s¸;ç¨p&¢’Ï‡û“¹]n%İT-T†æ7[n«U˜_]­zÍtO>õI¿»áİ†2.	c¦±l\ºâ.¿çà¤wƒÅßvgno£ˆ›?ØÆ§ì€ñ¬øôÕÿ!T›ş.•xoFğšs2ïØ(ÓÑv€¯€h6ÍgŞòg"ÈƒLÎ5K[ŸCq·n™›4—€GgsB¨¥êš±w. /ÑP\~0LÌC¿Ã’n4›q!Ù`¤ö”•ş"¿$V+$.‰ ŠUÄÚ2hJ¾zœ;g£"×:Ã±R!À1%îËyebëú [COaùŞ+,Aª·Z^å¶ØU;¿EjT·f~›¶·"íû0° Â`Hdì#oĞe¢+ö›m4!†•äÆ¡AÄè£Ë×in …7tÖ&ú£û¡ŠõkUq&¶Nhx8¥!¢kî%ğ¸BE#ğı\ÆÍ<·@'şKJ%¸¾}á_HIi¨QØé¨Ã´Ù	"2;´şí‚´amtÙ+®şc”ƒñ"ô“b·hH7JßñC²ßİ·«àTÙ+Šr ×ğİhË"ñ¦> ¤h]ºÛ(nÊKÈ¡ şs÷à¥Édx4cË™"²Ù½•+‚x?mÈ¦9DFÆaá7'©Š`xÂQDÒ“óDà-L.†ªIDáGø6#}£Yß·qâ]Öäœoúõ—?xïot6§š¥£§%= ËP®ZìiDÆ[Rï‚ûéüú§F5¸v;e³òœ¦Ä“n©•|Ôï!
¸1Øî›]¯O6B"(£ûû-ä†]CâHcBò&÷»MŸisEÕ“·6nœ¿Ö¿•Ú	JâöÕN¦íAÍ’6[ z<8I{fÄÇÏ`T©ä1ØóÆÑ¿IvLñÓ*¦0w“ïy'·'å´6W`]-ª>Ãw-·ãøCƒœÍŒÖó°;TAd°/’Hbı…ˆwÇZM=ÖÓÆ¢q–¦.1š¸Lqj¨lgdCÓWƒpG%?<+4Í„å30'FK›ñ<FŠDäğV†òug¬U¢AR2­¸ßQ«˜§£?ÖûYĞõ .4òyOHÂY®…]1Kf…‡°‚Á)ÆÍõu`¹Ö=î€(‡µu|+á ÑÂÉ@— rMCıTï}|Ş’|¬Ê¢ Ú›¡usï^CO?LO…¼²°RÃŒØÊV©ƒ°/"IŞkªŒüì¿ØÄéóªÂ£ÅBKFë÷Ó*, ?šàõ‡ùÉ( šŸÕjq¡!›lÁ^š`®sÃ¤U”A­7cˆP>Rİş}Èû™Ğ]Lb¾mB3Z¯m„©ü¦N‹.8È‚/Û„rJ»-Tz‘æ+êc<ı} 9´,UÙtÍGé¦Öù*½[J‰XÉÁ \İ7@€­nà"¯_ÜfÃè‚ %cFæ¦[Äb¨‰6ô1ä²v<Y‡Ø;ÚQÿ«®¤^ ncûm{£;Å#N,ô7PA¿ Wï7xê}ğÅ9”	gñôbt@TøÀ$'pĞI^9W*í²…µ°‹J®Amnîmï
b:Yw`0»I½±CŞíH9œ'tiS ±‡L7Ã¿³T*Er5ñJÂ	ømZT·w?'†S”EùãÖåIGğö÷ª­OOÓƒ:•Vá²>-ÛÏCH¾!åv<4·êSâ©j#-)cÒÎ<Ú—©Âítıo¸.Æàü3‡9l=ÉÄugõ¡•½]ö
:¤ÊõÒ9ÿ<,l7Â @ÓğüòEÄ…ş{ ‘½ÃÏÍsŸ}vwB×î?l~8ÙÅ!çÑŞ©çH9s.wq¬ûÙšTë<êê*Ñ¶¬„%]Mš¨cŒª§˜ÖOõb|ÆuÄæÙj©+Z§I#P	S[¼Úb˜¼2@³tŒDèõ×¨™VqµÓØzı4éu™0ú M+í‡T*:wõ¢I°R=éœCä1ï9OmœØZÉø“óHa’î;2°+Zé}/Ç¹è©ùäñÅ¿`hNU4¤“ÑxşÊúJò}OoRF²ÆC3çì%n/§h›óë»‹À†I'Ä` /óô3\ÒØ	‘4†'æˆƒWÜZiÓié>Iº?Ç²çPÜ•›îàƒtŞ‹ìñùõ¬CµÔW£ŠOø	y^©4ºš½ƒ)Ëà<›ÔõüpİrÏ+@“§É~òÅ¼Éı¹ØÑ0,‘}{Jçû2ĞşkÎ¸¨¯lóÎ8×Ö»àı©šÿ¢O©fšï°¨ë1†KüÊn1M/Òptšø;/½2¸L0f}õÄøå%9Ÿ‡AsĞ-|1%~Â^¦êŠîïÄ×¼¬Y¹V
$\ø’­«ˆƒîìÀwyhn@aírÔ&ñ*©§yñÀ»¹•‡£ZtİÁÀ@a²“-“”*/¹fó›¡1){ş*,|Šõˆø¹!*õ’w<,ä	[ªbµ|nwzÀ×V÷ôfvIK©UV)<§×0úCîı˜pçàX+ÃuOî@ÔÒ,\ü?i–Èâè|˜õT>Oòïö%«„F¦Eš×Pf@9ktº	ÅËà«¯M[öÑsğèêI—KÕÌi;ª \ò
ôX¢’MÙ[Z¤¬VX)4os;­íiÑ%WâÒ¶ÈåÏ-`„œëùâ¿·1´O@c)Œƒ£8Ô­7V¨;b9k\—W6Ê‰Šû-Æ˜gPÕƒmĞ8¥§SD¿Ä^G¤twÅH$¦3¡ñ¥ë§‡Û±¦-oécAn
ÍÚ5å/	A}éùëUi”Ô·L­
ì4µ÷Ğæ kˆæX…³Ã¯©¸Éo›4ÿÊŞõÇƒEGQ"X9xB5ã49ea?#‚2”e' 0„1ÑŸ3ØÅ"ËUWYšd©Ä©Rİh\ÌŒë0XG±cé:®ªú¨HõaÎL¨J I¯ÙAh5mnß=)4ÎûêŸ*”5¹sti-³òÑIš úÏêITr¦ò,Z®è|Á-ó·IsÙ©lJ™n¨[fBï6_°õ~doê¥]İ)eWuİ`µ¡ã‹º"G˜bNÓUâø‡¹+ ‘Êp¯5ÉD„Eµ’Œ/²!}†²ğë*¤3¾¿7*Gï@s£«I…|x	M—§“Tşğc×~Ğ`z~1É­r18&3Ø¼æõb”£7!ÁÑ³Ã¸Ì=Ùµy»Š^?ÉU¶%Xİ¥4Í•Šk<·ŒìTzöÇ’`Kä\uªˆš¬¡4±M‹oWL&k›>Ë®µÚAhœ¬:}
œ¶’Ø»øx»ôSÃ8:ÄS{»_sÒóù»“ùVÑj|ß´æôOX<‘k«_écqØ¨W1ãœİ~(£î66
Î8“ˆ¡4KJ(fSSó‚mQÜËŞÄ’/nŒø=±_ıÔ¤ì”˜]´0ÎåXE´Í»mFu5#²B”à»¶4Ì%r»R²3EüÌ5C_TÔ=©“ö‚€hœµ­/ÕnÁ¯õì—d+—¼÷Ïæw’ÍnĞıï3¶lÒä»˜-¿üpùUÑO±nI$z®sGD39n)éİX5£u¥azğÉÔ™]ÖB¡ß›Ówş¼z(°À~EL$şz«îË™\ç÷?£¶*ä¥,m@Ïµ.Øy,q×P,™[Ì-‚ğ¿á,ıG„dm›`ÙHª=¢Èv´–3ÚF+Øş¶¿^Rk÷ùÀ®w„’è¶Ä	¨õPvÔ‰Ã<9QSÒl‹'î„µ'¶v·%ê°¦FW³g¦’İÈ£H{¿A= DÒy	¶p¯m=ÜÜ®d“ì@4³ìÍÇ0ãn˜ŞÅ³‰â£ããµï£ğÜÉ ¼AtÁ†ña‡ƒàLA	fµ²Q\÷—nÕğùIÒn‹l~¬Ì÷áªr
3…ÕmÎk]´ĞëìÃuå[m>Ø¬ª¡~
DS9¶äÂ*õ[a2)—hğËG@q‚f@ìu¸WS†ñä4(€‰á´uG€Ã4­ì,MóŠ¡°‹3Ş1•ÿĞŞìïN”oh3ÆQ÷3ôxpï}¥n‰?Và]ø@Ñâ=³ûT€·ôJNÿæxÅtş&f¹‰¨„–ûqõåü‹@”e¹èÃC	ÿûk¥ÍÙŸ·eØ›ÿŠ
‰Â1†Oon´–¸¼ğhuoJ%8ßkúß"øíİö­Ñz]loÒsV'ıÃÙ”Ò9ô@D†úªJÎ„Q1€ÈİÅ©8X ÃúA¦øŸ’ÌúK®2ç*ß­DŸ¥ø‚èE¸­»óërNïAJ¼”ƒ[ïŠ\†Ö!„‡uÆ8‚­?¤*(Âö¥xÆ)õİ@Îœaİ¤êòï×çY­f	‰†¹ô1§A•óšÓí©ıŒ‘„Ø¥©u©2Ã
–NÁé»½ı†ôCèKg%6rrı}x¬µ‘z|WñA{@Å¼$Åx|}³G3­”† fGgµ^‘9ıGWş”Ú1T†.ıÇ¬šr`JäSNŞc^‹;8Ü››c~„?"F ½hñÿÂ×’”Ì–y?•‘>Rù P¦˜†®©l‚ ğ=’Ávr İäL!ıÄ­õ_‡§Â\ˆ6î8»YÃ=ƒ³Ğ¢™—-UU©é}£ï–ÍÈ†j¿·‚LÈì[C: ‘¾f5Z\I¦·_vc$!¶ö%GcØÖ/]x-"<ŠTƒvÔ›@Fd(zÔd)}Dì“$?¦~XD’ YÌîİ¬JQfQr¬€&¦”Ì"1|87S¨£8}ªÇ¸!Ù?.²NhO1ğJíÙOMh Ë·5¸ùNuUvIb,èiÈäÓğø+ªä³3q¬/æLL‚G‡ÃÑ ²x²ºáá7×Noîl&Àxú¡rˆg\3š@M’üĞÜ~'±†²™’ËÆFÜİ)oƒóÙˆ*´]™Ü^4ºøÏwµÖ[¨¶…¿ƒNb“ƒ=æ\šÔ2^ÓrãİeıBòÜ@`ôÕÓÃ3?óùÃ9“F€rô<)—	İof6ÓÒ /C0DHâğ»ß!.2÷v‘š]T˜ÎzÃøè~UPÍos¹é‰;Õ7,°µÔ[äs†òN('OÜ4z¢Øü"—50¯E
‰\ÕTèy9Îª·DíNKš{m®^Nü™Îq¨õª‹ü7ñ©c$Øßb>›z[›0ğ_toß]ÀSë×ìo\m/vÂ0•íÄ|IÊÉè—­šx"IåG#ó*	–µÑšóäÛé<Vtî\Ó“¨7Ë”¡ér§ûÙ“íSçø¡9R/"wuL—ÈaTŞ¿~=Âqëò#ıoÑî>xïûu(Ğ¨~˜~¥¤xÈ3³)T¯znhÜ3Ù¨º—‹Û&¨šP¡¬ò;¬Xç~Æ·;*Xç{Š}ï aJ ±`ğÎ1Äëb4û¶EÆItÈ_çz¶mm+tâá3ÌÉÇĞkdêàÙ*ÁLmeH‹	¶‰Lô”Û±îì^[5u½h)©™É¿ßë…€%U¼ÔÇ,Ú™>¥Î?~’¦x=a™æLH±Ÿ§KîŠÖİMÊÑÜ¬’¶„¬¸Í£šûO*à”È—O{SùU´Sµ©b	ü€@¤}?§9ˆüXá`¡kÀ¤•Q'>vÙO`—Ù<YÎ/-9
*¥ëÚÅsp,±â
×J-ˆ©ë;¡?˜ËP&<”­d^Ä{®ĞYi°k’6l¤WŒ™f…"±BMxõ>–[´Ê•b‹x´Ô˜Äš\{s1~!:(‡TCÂ$¹~_$©cÅÏµ`
Nñ#†9ô¢íqæ~øŠ2ì¨4„õŸÔ6ï¤Š¸V\"»=Ë¸Å(/;|z/ätûT”;bĞ)Q‚”––ªÍØÃU¼€züÛ.PQHZFrws·¾Nå¬ï…Yœ‘ú‹päìcÈì¸#wõ¹¸pºµPLsğçÜ¯â8y}º/†¼w×@ö€EF5Y‡OÛŞ\›” ©å°â=F6ñ¦½g³@?ÏÜ¦I½u:Ñc;¼f™èV¥°²+÷èô†º‚]Ù1ŞğGW-B‰'~0z±æºò¼hÔXĞĞ5Âkß¥à.–,9jj—aŞŞâ…K·TÚœ%—Ìvz¤¯À.Y»hLI•÷`‡»Ê;€ÈE]ŸÀ¼™º›>işE§åô¡u‡„tN€R1»ñì+›–-ÒØJ›®Š•°Ûcæ°²øEÂ¯¥*öõú÷pr±u†Ã<½·t,T°…ãHØÖì¤‰J^üqè>xd±õÇ«âåSÎÈ—²µ2 ¨NÒÍ˜:ŠÛÁ:Ä<~¨Ñ2Œ	²*¡ñÎ#/ã`!û?ï· ¬cÿÏ›ÖÅ#N^..¤^Ö©.ÔT;Ú#OÑù…
<Hßfƒ/H\’Ò@]áR#Wn2ä¶
[É©'¡sîfó¨ëÜú›¸ãÀÙ«Zšä>ÅÉ0‹”­Ñm¨,tbÌé\Üí9°`;å1æmÅ‰Ş S
Ë%»+&æ)­3gÅ€4$ËÀ7¥ØOi¬ßÓÑ¨*.AÆ•Ù˜us¦<)‡4ÿC=dO„Ù—Æ†ïTl5(³j÷ÒFÂÌª!:—ùÍ%Iİ²ìq!%?ô­tâÎ–ßï«É$läê{Í
ÌØÃ“&Û+8Ùnå¾Ã(r„Y‚Å”'7‰ğ¦Jb|¢iÇ­rl84ŸÚ)> ‹ƒMÉãz¦=üÕ'±	<¡¦1Ğ€ÔC·Óˆˆ+åÉİ*ÓMÊõİÄ7Cˆ8èS²%ÃÚ*`ïÛŸ#é5¡8ÄZÀZ˜İ®¯¾«ˆÀQ9™0Òà‰ö—îë^¼üV‰º0`/«» ÄØ‡Lcq…õ(+Ö9L~0+½ºHÏäYLù_"—åÀ&OËzû"ˆöùwpâkÅÒ„õ~„}à”×u…ŞóVın(íy55 Êƒ
mœ¯Hˆ[à½šÅJ(5×Àfm¯éŒÑ5¨j÷#½^Ö7ü€­-ÇGÇÔ¡şâ†E+*[‡eeé˜ü¾„IgbC¢v¸¤òjúIşf3á08Ã†e¼ûòg/soæÿ½SãxÆcÑ·—|!Åğ²íDw›iÉÄhİin¾½Õ\uŞ¨h4€ŸXPÑ#}7ã¢ .õ†ïK¼W*+v?óÇ{âÖ·÷DÁnkSS~1—¸60¢jïj…ºÄbw™¶iêÑÀ>D<Dk—=Ğ	„MÁ;é F#Ù.¾PiIF¨DÒà‡"OAdeU9„í{Ûß‚qWN=şjàmXe›š0†{"ƒ?~AòÅ’ƒæI’)„c”ıôûJÉË¼—¾q~/+ÛÀrıõm”®¬ÒÊÔÌüµâI›d}Ş3ùÜéê´@ÙÏQ?ˆÛğAãYDÏÍQ R¸–’1JÌ¬À©i<b‚9…‚rvP%§WöÀëè´—å|÷J%Î»B¬+sŒİd¢÷:¶Œø€ä=›t9ëHİ‡—şøYœ#’…rgŞàı“ :¢ÛÅR)ø²	İé¬aXF—ÏIò"8ãÃ4YÖ}ÁÈc2òx¦Ò¼ìÒ	BˆÆrp)¶òIÊ§ÈCCbÈD`DÈ¡‚åÅUİUB=JãÁîÇ§ÆW‡aV/DmÿÑìôï€,!»‚E¯ñé>;<ySå…¹‡
>É†ï»† ÑïÊ}õG@ö¥h_n{ß±èĞÿì£{-t\A¹İ	¦óÔ…F˜è©tÌYÆE_æRÒ%å‡hk’ÍÆ2Ê:V·q±ø4‰Ä”6³ŠÙ[•ÁrêÄ²¸AÓ‚×\…T÷µµ!:8X0]ßyÙ	²NU´’É®7}P ãô—w<Åx’‡´'ôi‚l–ñ»I\))Nc×±6÷M²±°ú¥*ç8±Ÿw­B ''ñŠ]ñ¡ô•rU…ºsá:€4ò?¯•„õ†sJIÃŒÔë#ç`}÷”N”9Pp)/—d@&!Äzx·ígT>Ğ‹ó´@ç•Ğ™dˆ@Ğº|
ßZœ:]Ó£(w©–ZôñÒÜ:]tRÙëîÂòŠ¿ªá!ÙZkÅ±~¯õ’vÎ”-ØP{(f?9Up9R%g¢ù¿É:ŒöçH`2ªÜów˜…bğ*ñşEH¾°ïP^Pe\!FÔG¿&š0J“ïï¦“Š$£†³/ö ±- õzúï“ÑËn'iğÌ*Hv¬ñõ·=÷R9[Ÿº¤â0Eq|è@ÇÄù²•ºGÃ’4jƒÆ/ç¼ã%ÜQ%SII‘S”ìzuÊ Ğ3k˜fĞ«­_¼gºÒ2üßœ(aìx‚õÑZcÓ2R$‘•ù›;#N¤¬æ£nâßhÊ1ï!jæ°İDÓ@èú±¥éz¢Œ½òÇèUJ-`UÇC„Owç=kÜ3èûşòRÀ^\Òşì<›·]zÀF©öÆúGLE
äšŸ9óhŠO&¡Èøˆ§×!—ÿÔ¾NVÂë_ó&oåİ.^Sy0£1İ­M@ús0i¥k¬gtñãIô*œ2‘3ÃùV5œ$Ä€T‡ÕÑ4_÷é=7Õ^Èe&‘«±!:p,qïOº®DqNwOmíJKîØ¬~¯|âĞk½W		(c¬á1±¢ï”dü¾êŸisàCc¹úƒ_Ó €'
Ï®‚w’¨ÏR’£½-à/.Î.Ø¢%ùJ‡xB‡éao3õãEÎèòÛDÑ¨Hc€­Ş#I½ƒÍÆgSŒL
Ûí àXÔŒP vç<½Qâ­¯ï½Õq^`É·2*æ…Œ†(pæ
IÖc©,T”<aèà«“R;à…”AèumÂre_}«®,–íı®¡”
€”Ş3®€JáSŠ½‘~ÕÓóC‡j×[ÆW€‡ºÖÀ`|JE€Ağˆ-0‚>á™vOÈh¸Ïæh	Å;3»İ#h•$ÂD3c©Ş,¶gI4r%ë ÔO|ù°©6_j@Ï€,^Ñ
ç\òc9wHæj˜*OP&™8Ôˆ‰°Ôbzñe´-Ö²Ğ¶¹…¾uª£hı%&ÉÔ´©[}_DE98]‰LÌäé½äQËæR€Ä««°llÍ"*/Á~Àïf¼±ü«N}$š¢–Môû+ye-îÖFtĞ]®JÍ¨xªáó‘&¼¬uç»½0ÊÜsúÇP²‰Ô™;¡`/,Us?
´œ5 ¦†“™“Š¨­ä~óÿ#¢l=BSş£«£’•«Ô85éG2ıxeMÕ|ò‹uş¦‰ï(ç")Œ’4Ê>‘tn –®Ã9€Aë‹¬=Ü{ódt¯¨åÆ2‡†gHbòåïÚ#øfµHXI £¢ò	¡7×í¶+ít¢¼™èş¿bÊ<¡}FŞÜ¯¡³Xä·Ï?­‡À×¼şdx`¢ÿ¯2¸\«q²Ê<B’¸ØñŒwcè(KÔ |Õ×pÔcÓàá@'Ö8ú"'Ğ°‚«Í¸üVâDĞ±“ >SîkbÕ§|l„€úçZ½œG¹i}hFˆ(gS›2ø‚À¨Ë"]b°;<×„êñ‰?k²²xR‹›àìµ	s!+(D¯˜­¡á(&PA}º¢•;#”ô„|.m©jÓùÀÌ¾çÉøTĞyÊE¼–¥ş.Ñ2nÖóoYf;µzÀ„åyÿ»|×7`†%¤[¤ÏãôZ½½$C<åH¶Ü­Ùk	§@SF3-›lBz†ôcÕDf¿Åèuî%Şo"Ğ²¦J¹1ÆX™@‚Ã˜Á¦Î;JËĞÑ ™K>nsœ;&ª÷v&´_< öC\[4lƒ›½ÅÀ»«:£dâªk¹AÈ®’”¿¶_¿øDñ|y“¤·ËI~şb.¹ÓH)Ws[‘VÀ·k;t¶‰ıùQ@¹æµ÷BŸHö’U–!î—{'‰TVŒ¸¢Ì’–p\,p/pvLìæÏOÓK™ßv7ïµÎÔ€"Üaó>ÒcwüyÅÄ‰¾;™·{¬ÚkZñvE€¡×»h÷‰%‰5øÁj“Î\ÛO¬ØµX9)zÛ˜·i	ÅRÑ‰™W²êA7ô@.…ãJlzËgV$y¡­d“öa¼¾şÇ—BJ Fj’j¥ªõÊ±ËGt+‹Âß%”øAéX†>»Ç­]ÖB™|0h0ä»5y»Ã.IWw¦.†ÍÂnw!Ş2ÔV‰ª^ùÄepZ›B©6,VÆ%KFÕ=FŸGxùÌ˜Â¼w:o²Á+|Rš‚ñõf•²\w°©ãrrşñ4É$9›[h£QLâÑÁ…K–rÎÎAï@æÍ‰˜„ÿ™†@ëïT!èBóÉ)ŠÄÿ#İêcD¸ÿÇôK£}:Ú¿ëB·Ğy4Æ~k*‚âÂjÒ{uåTèW¸%à»¬|nòXé“}ğÙ|B£ë$ÈˆTcß~„cÖ +ÎÅbÂİ®ƒ¢W§¦7İjCúÖ âeE*Öâ-bÂÎoi)T•hL,ÓpüpöÑ°ÖmwZTì°µ¶ÿ.µ8j/Yx” Q×ˆ­Ï°Îx=S–.Ánù¦§^¥#™vpL{n šİJò=s^ÂG~
uåøõ
ÈÙrš÷™¼'*øÄfsQ¡ä4É+èV?šE¹×ã€5ÚŒkl;ëkSI¾°î8«iX5Çê”I–Îur~ÛÓ(àÔİ–ïTÆmù%$Ğ±oÇ®ÑƒLñÀíìv‰ü³º¨Ê{x'?l/¬“à•-7rV{¡*œ^åıjAŠµiÙ5IoÕR„µ5Dš@37\Ó™t¦¶Š~°Mqs¥é\2µóa¹gJ¬ìÃz{ÇaÜR‚—ÈYHÜèÇ)H¬pÌànÊŠ/Cg‰pÆ¼è..¾m4„NZÃ)8ÔgEëd9r–Röl÷;ô!¤ãû?–Šù˜•—&i½l¤ß4¤}I4Éß
Œ´©…ÊñC·¢`üJ—ı.qEóªı„cÂmiwöİ›m†å+İ³Ï“ ¼ì‰c@ÈlØ¦¸Rföä††CjÂÉS½hˆjQ†„úÑ·õİ§oÜä®Êq	Ã†!6¤ˆ8n·tÈ–WğwÒ"*äÛA¢ŸWöIŒUh¤M+åz–5zCP›Öù J
úòĞÉíæIm{f•OucŒ›¥]8d½ ¤ÈëZ¶”¼?•RFš™jİ…K¯=Òâ¸H(ªro£N$‹ÍŞ©WË<	ËNQúŞNÎ¤·u94›ÀÜ‘÷‹Ê ` ]9±È¤/ñ$6ëıK)Ái†¼aT) ?%ãš„é”?=aoÕ‚i ¾o_÷fóÀÀ¡úš¿òB§ r\‘3ÆÔI{²v–õ®€ï®½O !£PÛşO£c€%œ}ÒË1øé¾Ì:‰æOªBNé÷PÜğ ÑV[ÇÄÖÕ~Ä^?µ±A:ÏÓÊ±Yd‰„É^\Ú×íŒ8@…. 	QS„Š&b´é·Igm–· §ó`×¢ÄÖï)ˆËï ğì‘o}2ŞFb•˜$Ü= /’;ğma4ŸısĞìbQ~±Ø:ƒ‡R¯¦§`¿4ÄÕD%{Z=ÌÅ&r•Ğî1¸9Ò¢¿ìwæcTÊ,¯ÊßwŠmN¼K1ÕU”=º<0Ğ‰´ı¼gÔ•0È­='Få@jÛ±e¶S`Be Åš
Êwjµ&÷û(ä×d1„4¾ry"´È	A‚U¥±‘‘IÜ“ì‡ñ0õ]?!õ5ØŒ^ÆÌ@>‹-R@Ï|/£CØ–*åÉá—ÁA—ÓÆLC‡Dûş®ß ·ÿóº[ñÖ°_×¼­`´—„¾4:‰€¶©1™V%ºøp@/_ÏÀâ	w«êu ¹‹Kø»4;æÖ+ŞHñ„.@ùbúšúaüeâøÍPËOå#ºg’$İËï+OÛPpIyv¬Òô‰Ú¥)¿×ˆKœe& Z
ré¤{’#jïˆ¼eaƒIfùÎ:ªY¡Ğ#æù€!úËÇôá×$­´ƒ‘_=¢S3÷ÍÚ™‚18Ş7‹‘À'åCÖÈi˜ ¸šÀ(P¦@pk€_%æë:JëLL¿«Z	/,\éy«[œù=WcŸÊ&x]|šÖ‹çO‹’Ô„ãÇá¬•ÉÁ"d¶g¯½U–	C}H2£ã/vQPÚ ÊåŠõ¬hr;¼âyG÷Bm@¬‘øôE‰ß~5ø5Šk¶×ŒÓ. ÕG7Òôi;7„SØyKn)l8ÍÑ´#ÃÂ©lO¶ØjDzÎ _SCÙÂ&BµHbkËËö?3™¥
&ÄfÆ¥úõ^ƒ2fgÿ;£ûB õ¯îîaQ9jJâiŸêçÈÁà­b§ê¹kªãpÙò‚ş®åJõÏ8ÏˆXyiyÒÔˆ•8Š“&\÷w†@hù÷ĞĞ?9²œK#ex—W¸Ë)“›> üšñÍÙÂ7·°;{—¹£İ¥–öFß	FC$„dx¾íÃœX2`úàökÿa)¸G©À1mùjÁ;Çk5êtÚ
P˜æ¦4ÎÀWÆÂÍ¨Ìd ğ(Ôá¯
ÈhŸáB§´ ç×Gc•³ˆ€0ş5 Ø¡æe³ÃÕ‘¼?şçC¸#1şê—‘ßˆ¿–h¹JùQ),&qëgyg°Ïµ8¨.4 Ô¾k){ ¡„»X¡D¦–ìsºl(°ß éïi×Rm=;Nšë„‰1ÏoÛYões&â¥±Iõªgtş…+CHtf'õwÄ¯İ/§2bÿXÈAc¦cÚ² [Ã¨(mk˜Ìå7Åo[¿åĞuvëj §t}•»y(ƒ›t‰~Œ›®Q-£…q\‘NpEºo”`™Ÿ±
…šİ8êÌò[·?í£u1f³Gp¥¹ûÁ‘J+UàmOÉSqÓ«u‘¼a¤0“[X™³fÜ„ÍàXu·6Üš=\ôEĞ±²çèÂ¯€¸û5†ÏCwÖ¤½ë;80ƒŞ2¸¡4ÍîÂnÌÙ¬ÿ÷ë_ƒoí|€§½ñ¨yÔR§æ^tœ¹Åº(P8Úr€üqÑŒÙ2bëNÇûşG^²–ößwİÌ âˆp= “ë?Â©èGjë²§xyˆ´`Ó}îWkš­!ŒƒZpweÏé-N$E ™ \Wİö¯õ¥ÖîÃ†R0Åk7Ÿ<&\D2Ìšk!ñ/e6œ>¢šTÅ“s‹00Q~\áL ïjcDí•½"D¥­*`JÇõgEÍãşäÌZ‡>öø<Ÿ¤‡´ÕÍ½@òàV¬e0pø£|§ÉEe g>¸¾±…¬¼ü®ÒıYÙT© ø˜Ì¦ï-£j^„Pv{6 Ü¨½Ÿ¯
şŞW÷ÏÒ¦ls{í@i™Êxø»Bãe„q!Œd„Ë=pwD.ˆUÈOŞ>t…ØÒ$èâ4ò\ÜòÙbs²CÛúV¹9ô<ŞÔ£öÌwG·´¥+Îş,5Rï½øŸ‚ÿT~n©ÚH3`óè5”û;±$ÒÇ&~q°zŞnß¤|1™Ï&erğ
×"K½‰SôUN/rÊ|3|&/ıóÁggıDzmGç9DRR™|š0- Ÿ~\¬”òxê°O=‹#ç<—°ı©¸–‘‹4/ßvïĞE%Ù}äŞOù\÷wCºU¥Tç jƒS<öô£\ÉÓòßqnd–¸†ĞpŞ”XÊ”˜O±ª/p/¯Ne×Ô²d›ìô™ò©)v3¼q¢ŞéÊ%ìxvi¤Œ=ŒÊ	U×úörÎQ>UÊ0…¿ëƒ,‘‘ş…&¥>ö­vÜsçÇíù(Ä«õv…-ÕÌ+N-­WÍ3ûê©È¡Ã{ñÓEU¸"k`Å©WØ®æµ@õ¬ê®s¯ãÓYĞÈË¸s]„V¯g™Ü02M¶¯iñ</æãÅÈ¶ùšu½L½òÔÏWÎÔ°4óœñvš)¡,úÔA\O3B8æ6´âˆ.Œr‡RÉºª5šÃL~ÒbBhjş>m°Ø_½r”…3¾ÎƒS“=‚Íßsƒ +ıŞÑQsÕ~¹Uº|`ãºrÒbÕØR„~°+zêy`ñ‹;Ï«BƒH)”êÏG¦¸?»°s£6µ/&Œ ë¦¶·Å ŒÉáL6˜ÜS9{`˜T(Ğ¸êœ|õ&Bé+Ç»Ï‚3)v»ˆ‡N[D§äú­”~c¥^Ï;æ h=ióØ™HRræ©œ‡§A;<€]SB )÷®ÃU ğ.©[¨ğ´è,¾–eQkôğIUéÖ“øãŞ\åÚtÃ®Ö†ªI»¹qgöë"¼oğ-³Ãù±a:.0Ä–1¹·«ù}¹U\YO—ˆ.ˆ¸??y7Ç­d]‚7—ŒÀ	õ…‹„ø®Á™ğCá*Ä@}zÎ0 	VÖú)÷ÅÇæçD·Ô~-ë h…Ÿ¦	êıJfAÒLÔ|éx0Ôgü0j´şRïÔèwk†àrôy.°
èn–«Øõ”ı/÷f_œvæA~Ç |w¥FÆ30:“ït?2-ğNÑöt–5³Sµ6ƒ}V!f€k®±+Ô•zØ&aÕÃüuSD7}ªïMw,:™w´i"‡ë4J¦x/è2ü®>ñê¢Å–S1zÆÇ’òø(WIÏã“Œ“+PUìjy¥bv«røíĞPR:Çˆ—Äü²weë´?ñCnÔxM¾ä§7xC¸§ÒˆT¸«jyõKí¨‘û)›œ%>Cí±",îş%‹Fiœàdu;>èƒ™v FK¬0WÇh¹Oj<»àXs­n–Ó¶<¼Ã
Ş+;ĞÑÕwbš1ìª]g¯ÏªÉ¡WØŒrŞlP5åtÙÔÚèHİ]&ÂEÊfWu ÿÂm¥‡ÍÏE¼ÿ'Vëä”ôÈ #´ç7»12ZOvdVg>Î~RtÅÃë–b(B…±QnĞ£õi){…_Úş	<Ò‡´$­ı©ìdQcü‰ÙIŠÚâçÍy=ËR·ë/"“m·
L¥Ğ Š|”„¨Õ§á÷»
âó=M² …D=sœ'´¿–ú«Ç8H€ì˜%¡6ÉBDKˆ,»Ê˜]ÀtûTÀŞœ)WÙ+O†Ê[ü+§ŸòÒwv;ú·ÍÆ‘Ø bËµg¿™Ds5zùT3Ö›½¢Ó¢fw†–¶¯õÚƒä(·gôñud³äìÇ«M /—[|(øGº¡m„˜pÓÛ³~Õ—ß¹b™gÑXíB6—5Pù‚!±—]jªÕjy6&É˜¦=`‚½>Jñ Û-dIã€ÀUşEGg@›Çk|Ñò4…ãº¨ó Û0„·“ ?¤‘Ë`_¿Ağ3á°ç÷×üºssÎ1ÿ…šĞ´2ÙÕtq£‰92›Ú¼]Xntÿ4.p¢^ÿëò¨äXŠmyQÇ°8t{¯{Í^ŒcCÀê5jëdğè	ßÑy"ŒöNÄ+KƒŠû‡6‹äi0BNGõ–æGG#4”:wKià7pÜ¿Û¢HI qC‡åiÀ8åñÃõş/‰.©µÚ	
@!³!ğÅ"3¿åP,?t¾(­\züÄq^­«R[Ÿ»¹++Æ*"ŠqF(Úfôú|
ŸÕå å˜?Gr\=şHa7¯'¥]bÈÈt£y·x9’ˆ¼Smµú{á[q‰)dªO²—GŞvKÅ[
ƒVİ„ë¾Ç'S´\ºhsÁµ¹œ;ã§6ÄÁ€ğğ,·ğìÄ1–e`K‘ˆ¸Œ\5œÅ4@E~QĞ¦{¢)À”fTèsÑi›40Æ”İ’åÆ«n›è¯a­Rúšo?–M‡‰›Õ˜H¼n”f…Ğú?ğRÉ°òô¥Ú‘»ÀK*d"‹±J¼¿İè»¹ é‘{dX§ÃŞ‡íôöô_“ö¸I1’W¤Ç¸úÒ‘»4ÙõYÈ¦HArËbŠDÓ;#Æ˜Ù_
hĞ×ÿèèÖsn¨œMÃÚ\CC¢ƒ(;Mçã`Ì¨’AQã—mSÖ(èg	uwò´ˆãw¤	båWš¿e¦²•k’Ksf<T eÿ*-¨˜¾|>„,‚Ã0†D5Ñß¸w[Ê£º—#hté0Ò@¶Ù:S*†êEe8=ŸÀG7ÁËítáô¸Ä`›Qt©o”!£Œ¬G°Éı%WJ;¢^É=à
º¡'V‡øö¢DCu›™vB¼2SË`“vàıÿÿô”i`c#Q}’Šçª¾˜D+Ø„¹BÃ±IÏ
¿ £å#í‚ç³€í¨	wüz#ÈI‰¨@8Àÿ{`"Ä8!ƒÌvuïŞ¼ëÎn2+Œ…¶œ%lÚ/#y­ÀP÷\ö“´ZPáÖ‘œiˆf/İoÕ«œŞ³L7bÊ¬ªß­ÛµN‚t½HT½b—–ŞGók»^œ6 ©˜ğÉˆ;Sâ][d0Ùùê¸“K¶¾ó]QŸĞ²ş~ãšü·… ²xër"'yn×%j)ÌéÆç…ÀrÉ[ó5t£lG'li`2¿÷ûœº•øÅ7ÍTŒn…ğ=ú{ÜÏ0OÇ¸,š—–=Şc<.ã²ÔÌŸ÷ã¶ÖdbüÚ’3…ã•vF’Š"Ø±âÍáŞZ‹<Â Xn=?áŸÏ-+ªXê¯X[ÜJÎ¯ì’±°£¶g¾ˆ¶	P/R°¯&zÛyë’í€ø=\óü’á€2àÅ‹§tSh¯Û­ä³®ë˜lùø¦‘>8šo%ëcVŒr¶ÊØ''hZ†ŸTç5 ctD*ÀË&K0\¨•ö~İAµ%+GV¢Nwú$õˆcùÜáØ*—%£›3|õÕ M*«íùûg³„ÍÔ´’Œ„±I}²ĞAYÄç»ëmO~“7‚¡£"~–FÏã.%wªvO6iòük‚Şıbä(l{Ô‡FûXù ˜	ãYfQFš5ğ2Î'ğMG§jZ$d=ot8Î·üi#Ü¢IÇ`+ã»I%Æz¢*–räR‹Ø?}Z…!38”q#œL²u‰GUÌo¥1o™E “¾eª-öx3ŸÌ·—æ1º½¢ 8ÉsÈÑj‘¼âc	™+ÛîæC—,Àõp¥;Î¢ğ„ê¿Í§©S¹/…7SZfPJ€OXìÊÑØJ ùÁğÓ}?zrBTÔ3¶.‰H#ÚÓìt3Ÿ¿àÑ•,ÙÉêHõY‹Ê€ÁRpbè° BOï6ãóëÍ	±“}(\û+ß.zõ§ÁTÑY´~^í×Ö_Ÿ•ˆ¼7‘W¶ƒî,ÑŸ(´õãQ•€-´Íîïz+FÜü­ÿ}Z ÷İÅº;ã·Æ„’€*»&ûû„…Z^zrä6 ¹9‡‘S$°Úæb×ìQ¦¢`x€çz<Aã7ïøËÜq2k‘så&¬zw˜6Î°IüÇ 
Ècô¾ÄÑÎQou®[I JéOˆD%Ìå:òoOîy ø<o;a8Ù’2_œ`tô¡‚‰CÉ¦{
.2iš‰… ¯DìtíR(JĞSÂ+iõbæóÇ”½BA·s*NV¡†“¡O#Iæ1(4K€;\Ö:¥ô·ÄÓ‘cÍzVŸgÑ‘»5pãü’êÛ¾q„Çã$¡¿_À•
k2Ñ)ùÔõ¹lvØ–ëº¤-eˆ:öÑÁvô(âº×Á1Ø<yë`á„ìñë>k&(W±½aM¸²Aã¢Y[›MGvu6PU&gú#”jà¢zûG>ñÀË¶ØiÙ‘¡yÒÌùµè¯ÃğY¥
+|jW	«Ò
óœ={ñ ¢•7è‰³-šÁ‚¥|¬Ş¼ªuÃØN˜·ö!¤ğIø:+Œƒ¿Ç‡øQ€0môÄ_›O´¨µociê‡¯ÒM¸ªWpØİY`ÕwÚÉ¢ g
=ÏÇˆJ¸½YY<ğò,t?Ìo8ê>bÜ¦ŞÙü; ÛÙäÖà;óJ²8#qyáz 2¹Tƒ•Å„ö÷ö#ıYŞécH¥ø	‘Åª¨Ï%
q+²Àá9Aj¼ÌX ‚áSf¤,9î~[ìªôe·nKP(Åáø0©„¹lË½Œƒ·®9K¢é*ŠÜrÁ_ïQHîæRäââ2H‡>ÂŒé9È®nÖEXÚr¹¾œ»EAéË­„.!’eB¸"Àå¨®½dÇÉ,ƒ;zÜ©s-9t»H°‘•–HÈ‘éœ©¸Y‚üşõ)`ÜäÏÓŠ0Ó±ĞğğçÙqÂ	c”wËÔØ2ËhV¼\ÛW$\86`gË5£[5‚%Y-÷«4Â©€ÆMEìÚWÍÑDĞNÚØ˜M£3×zÓ}Û “µ;*F<™'ÛÁb¬|¦Ó#şŞm§RÛ=›=k<ş‚8§gíë«¬W	%]ı‡)æÏİ³ ¨ú
¿B>
€ÂEFÜı,Ç™pi»<jU—»¸I‰´dvøx’ãM.ñÎW5Ä+ò
ïmï*ÇÍë7ÀÀdGT[A~ŸPßNê0$
;±µW°œ/áXäZ‡T>ÛQİCÎ¿İ&X‘w~®Àú8³*kG
_HÈº&³/9ò*ù”	¨UZ±­‡ÔÆ·­ïËQdËù	pÌb³5šäêKcÕ«š£ìäÉ2^’z*Ï¯l»¿¼lµ¨Çä›ºslÌ9²t”3½ÔMVR 3%0TûèO:{'÷ÃUºÕ°ok÷ÆYÒS‰„ïÒêÌ¢g×»/½İş.0óÒ;²òH ¨%Ïe¤¤¤J#OyØ÷h¶üb¿v}S½)ZÎ‡={æœa§2tgïq*Ú´kªÔT¤È3kLß-ôÊ§´V¯CJ8¡úŒä^wU ƒ«
¢1qØÔ¬Ò,®ªçC'’É€&K>÷æğ¨Ûo*/ ŠF@©E‚¡uéwG‘óî‹
kE lf@Aº3qÇ`‚[ Ën‚hhuZâ£Ë¼ã£_}`Æ÷ƒáŠèøO†¹uíæŞ˜ßxáH‚Cbï¬9Å¸Õ(¬UH£›ÒzKÅ)öÑÇ XC;Ÿ®òèæh	×Ï?îY_¾jÆÛ±ZãÏ8n×4ê5è¡"DKäDgº=/©ÓÖ’d>ÚÜ*U¯s·N~^@Tiå'¶A=ä˜ÿµ1ÂxŞ>ÓğÔNÆãÖ¾ãï…¼Âf¤„Mí€r£8i Şß”ŒÃvxôÅxy˜šwÀ‡C£–Ëÿo»=é8f¾ZüÁb@¥©O%AØL&|˜Õ	«»ÏÂĞ@w 3)†Åğj¦°i!×Œ›úrç”Sù;‰÷nëX¢^8æSÑWO†¢ÿß(X—I§¼´-éûàÌg²+s*ôhÆp¯Q¨4\ı*Í­ğÈfi7œ¬,(Šp©n>¥ã±Ûe°ïD9¸’‚:D¥,vv9§és|3_?‹ÎsÇğ01Ùş›>ê…Ë+-‹šÓ™èL¾ßì£xY*¿néå¿Dšt©'™*WtÖ¼Mï»êé@ş—ò¯´—C_`?VÈÇ^˜‚x”ûÌw»…İø½ÙÅ˜m$P ­:Ì!yMµØè¨¼ÕÁJv	½çzgúB™öĞ’äí¦i³—m?‚í	‰iIàyg]%äÎ“7 3„ƒäg"±ax··£|ŠÁ#¾üìB»~•ök?	cŒ›ôn\0(ïZË‹ïUª:ÎCĞ³<$‡Í¨>ï¤{ÚrÚ2&úIÿñvş\>"”ÌÇ:“*åŠ„—v™íf?î¾hEA5ß×d'!÷š¾´1}Ú66"b_KTIV±²$!¹%°Û
gú.ùNåmy-Ó3Ä.¦[ÃÇ¤ø‡ôU„!¤¥%Ê©îÜ“í$Zàz½'°}ş‹˜9`®ïf¸}…*Rù±i·<L:$ùN• ¿Ö¾~I\w¬Sø[I$™Ü"ğÅ.¿éi¢ŒX¨„{ú3Qs!Ro_•
ìa°`´M¢úÔñ‘AT&]Bró“u­ĞÃ'=ÃŠÅSÚuqeö®àuiKÄ Çğkôº[kW
â€qDf˜Úåh+`¬&|%#q=ßûËQô*b4áiÑO{{EñcÊî—(İ~Ñ:9ERÆ›6ŒĞ‚Æ9ÁåÀO{årÛßÁÉmÖmMO‘B¼§EZ>šŒùKÔ#ó¦Wu$Niõ é2änÙ7ş¤ ?Ó¨4h¯ÿ	Æ…G,ÏCÖä½rìóÌûÈ›®|0l¨çéKB¬¹vÃe%¶‹û&$!‚š;Ø WŸw<¼#v4Ô	Çô`Œ ›êÎ`é‰L]v¿é„Jô,5z‰c`ìàò;Ô~DsHôkRéö”å÷)t0¼_ÕpmfA²sñäI°f¥)íÊ•œlÙQĞ«Zx3$>tbNü~VNÕkÅ'Z\½\5»
kVùBüEôJóÉŠ·Ú7ùÀÕ •Š2‘	WÙ¶.¾Vf¸ı ü¹†:#ï«x,NÖ¿ëŸ2œ„Y‚Gô7·Püß˜ˆ˜Epß¤÷”Ùëk¦L[ü¯,şĞt2ÜµÆèµµı²âNíôèôõ‹š~ÉmGºöøêT`úE’¤ÒWÚª«iågŞ_®Æ-ùa›å¤“$æiË´×¾Ò¦ºZ¬òÈjí­êÙ›@şÍ¾íáQËªv“‹EòPz¶†Mğ‰¶;ÉOD{®AòäİWº®/áZa$wœğ3_¨…Ha¾Ú¡VÁ,ßÓ
šB÷g”Gì¯MN¥”í ¤"€¦Zìõ¹½hõr*š—dôU’TŞ¢»nÛµœT˜iÄ¥>g™BŸ©D wf/-ÓA¾Ì! à_!½´ürÖÊ¤äuˆWŸ[,¨’LCåèqœ"tøÛ+wÃĞ„Ø­å¤ş™¡
[öù| ÇbRÑãO™Ÿ¡wÈ—\—vkÆëºçCo­Ûğó”MWç¯‰l‰Õ=¯â1–¸m¾ŞPDÇz‚‘ŒöF‡Ş#èáHŸ¿Fÿ8‹ñ{€ÿ½§Ë@è(zzLŒçÄÄ'~Öe¤4¸¯2’¤yYÓÀş?ŸBwÔm×ıË¢y	X>ÂÆu¼8!v>:•´ÌæïŠòQ‰-|ê¸ÕŸšSÈâ3íÉ_E§*Ò	?ˆJZc1Èï4”úW¤ñ›7´§*úŠŞy*†/IıËŒ7™"Š&3lƒXwš0”,8	7Ÿş_-°C¡°r5)RuM2U÷8r­q~FßÉÎš•Ó%uûúæÂƒfìˆÇ€ ¥Í‘ƒ7&Eó}Å`Š‚²Ûƒxq¹,Ô»¨Òe]B×ß¡Æöç‘<ß” ø­›ÎBSHÊé‰°4×®#e¦Ü6w&üõçãÄ&6Ålm+Ÿ[•$Yh ™_şHı-Å çÓR:i'†MÜB[*øa«ßs:]öE©ıQ€æ¤'ƒquSgw—x3&4Ò;Ï=È‹Qz^P1:q
©¹ q{]f]P¡‚7$ıÇ@c¬¼F71’ã¨È¶ÛN{+¾)Ì•3úíZæz‘SµÚµët“ã@æã/°«(BTB7:)Õ±Ÿ(Wà|½Ã¯ÙR£ûîû†!ËIàT•Tgåay²ûª|*§1İ¡¿³xT•,²f¶ı
ärôTVc*¤¸Æ'”lKB]CÏî(> ìİFÙªBòã±yøşøD:1BÆË-(«dy=!æ¯AT›*6'E}c­øn…®H#vè{¾¹s
Bj`{”}C6º”Ëw‡Ê—”j;Cù)“{Ş#RñïL9Ÿ¿Š€GÀÏ§R O†mŒ]=¼38:k3	—6aÕÕO‚œš×zI4<¢?¬Ø­ZiH·¶?IŸ™öÅJ‡àv)ª¢*ó>>C³B&²CPÂ9cp|Uf
|B•S¦öØ!£1Y:¡ƒ™îO‡šÜ$3<?+tM*(İ†Ü?|Vâ0LñïË!İ|O§>BµÓ¶ ¤léiCi$ı>ŒÒkŸ‡Ô-Ä\rïÃ¤ØÚÁ×Ë,¾ÂA?¹øˆ¬ÇåÖŞnÀ1&Íş>B	03E•†UˆØ0|VmtcNÔHŸ	*íëLBo)×t¼åÃ÷[õ!Û
-Ss\¡²1£hšlâ¯îwö
Y°ã(Ä4‰1’ÿ<5ßÁD‚êå¥M¿¶o{%ê+îß–ËFÀú\õqbì0oN¬Ø-+KÎ¢ÀVÁ‡ÛÂ%uiíq]IŞ”HÚÊ(ƒ´fîbödŸoêœ—FU ‰	%Ãúğ&ø§TãÇ}{Ÿ ¢@1„q[ı"‘¤·&§Á»£>×´^ÀaS§:
¦Z}¼zÎ7Ñ/ØBpvÆ
Mİ î¬ÏbvÀÏ‡èÑ¡S€|ÍÙk¯˜é¡†zƒn,sâ—„áY Û0Î/ß®-^€õé*ú€‰*n‹rmÊQ…Ãñû»‘“kÑô‹H$%i¿^Ly
?Éî‡o˜·r­=ù?l±ä¤dXJ,Õ„0Ù«½d$yZQš®‘ ÛìlLzR±ÌkNg½	ÄQ,j(ı"ĞÈ·1mÍÓÎúßjˆú|v˜àÀ`|A2vk=¹J›Køw8˜T¤ ùÙd4»«æz[¯ôæ>mâÚ¬øDVƒ²µ¦û‰n‰ú¸rTÅtªBíPí`G¡©o? ìúĞ[ıW<Îóâ¥©HO Àn›Š!e±¯º)«D(z
;­ 'é¿0r;Ù,F!â×Šö+Øûå¢MD8Ü¾ÕCĞÄj\V÷äª–*V‘ÏE±ı:·æCÙb£Šû7!:w™ŒÃ€+É¾.ÍÀwö“s'§mÆ¶¸‘ÿ2©£™•o!j~š–è-°ÎÍ!¯ò½j¥lá4˜Ç‰PJß¦æ—‚×°
—ögıØöÇUD­ï™ÀfGÁó•øˆTYºãšÆ¯HÙ|á
ñŞ’Tr‚×‘šIúJå·]¿pÀîhG…š(ÖÎW™0±(p"yP£N‹Ù+^Âa©.zQo—bFø>ÖÄ'§j<F$«$µÀ1°§qV˜?uDe½û•@¥ÖÀÙÇÌ²Q±Zƒ¿ªA}/{%ÔÆ^…Ö á±âe–Ì³æc=©<yÏ—³X³İŞâí³!É^šãşHàP.¢8œøWD!{…t/¹®´÷3?Ã»
½"ïè©ßÚŒÌ ôl“ôáËüĞæÑ¯<©9‚!p‚2°Ğ½àe "Öƒ”5zÇ{¾wL‘~gñlÚènMP€Ô€«œõ90"Ei Ø*8Å»BËÅÙûùİÒªŠd°¼¤x-«)üUš>àÈöÇ%muãív³„5aµŠ¸H§ÿ¦vìß¨DvÈjvf™éÖ®×m Ãa²˜ò+FtJÙ¿ºCoL¨d˜»äAõ-°Ci®A­‰[¸×N¨ê éÈ:I1O9]{P‰r­¦…U)mL< bq…ƒŸLUŸsÆŞ9°Gq´†£Û˜5²¯hçoYa¼ÒªàšÆ7VOxúø*Á{‘{äÍÂŸNÕò~¦«íTëË€nÂºLÖêukø†çL/3:qĞßp8ö;ÿ¯uFı/îÙ¯igÂÿˆ®|Ø‘G,°¢}aüi=*ÿB‡¨-»Y$¥éy3ıœ,IÓ¿ï®Û¦ƒÜ“´ÅÛŞ•Ä¨<êÓµ–†LÎQÁ	”’ã‚¾ûo¥Y¤		şÆá+üU©f!½¹OŠ4ë±‡Şî>™š€ÓAŠÛW[ÄjÀ7šv'UÛWKÃwl-ÆùFv&8,=Pà¹Ö³‚ó×„:;¡TJmˆœúÊRÄoœœ¸MójîÅüŒ®˜'¼!ø¥Lm©Ë¯øÅ\9İğ»í°÷§ÀŒœ_®°tÎX*t€ÁÓÑ9ª*pÅÇL™Ãï…tÂÊ¨}ôÆ„­'ôR+êÒ‰føúX¶tå0¸(ÂliZÙÕC¸ùtÃCYå²ØR»[•³F	ÙyÍ
Ñ¦³VWšú•Åä~ÃšY–’äwêöYn‹ŸPàF‘Ñƒ‹^X—Ğë(0a7¡9}ƒ”Œ mvSMN—¡ £$^_1¡®ú—Ÿ”(‹4ÚDézÀJJô¬lcÚ•}á°‰	€ô‚å­Ã†0¯An9Õ2KÛEËºóµ>4l)_j±‡«bçŒİğ·Æ2+‚gKPİî&Š²ĞiéÔh©İ[ƒÕ.Ha¸ÿøÿ¼\-‘£ÂÀÃ;6&<îx·Z2ØÉçD#B—Ïu7ôi#lc’z}]ı£0åïLWíJh>3¾ÛØg6YyzW«V—%~¼k(¬H†I_62/Y€à^!Ú¦¯ŒÌ²[ƒî³º¥µXÓş|rlì¡…P¼È9UDj#q‹BãÖ„¶¤ ƒ3¥Â Tø|<p7ãª¼/°~Ï'İÔ?6XÁ.Ejûí>ïi>4Ë]Më•…Ocis:j,t¢ ‰2A*ˆŞ“ûf]ô®×ávK3> 21irÔ¸‡öÅïbN ü»/s8yùÍûX~§ah8tä!‹Å}I&2ÔÌ×^L»j%înßŞ‰ŸTN°GÊE€Ô›@ØÄs¾€Ç:¿6RbÁÄAeñÚÜ­íDÕ+¦¶Áiá!ºeò[“š¦'ìZ®B/t=±áÛGÿ{èºqıb…]¢Ò‘ºĞ±ªwªìU[Ç‘º•ÒMEåkW{Æy•ÿ“	‡ùÚÄÆMŠÍlŒÄk·,ÁO¤-ÆajZõüZy– ×OåÓøjær±ÀO™àéQ?—8/Ò5×¶â£É0×Ò‘ÙÍÆœO	;é`È	*³pY*môÎNêøïÓÙv›‡ŠÊTÁÒp%LË»^'ØÌ—<Ÿ	m³±ßœ(ÿã]e`Dñ:Ìò+7![®éº>Â*îÌX7~µ^ r?îh·Î˜~VR\à¶q6¹éTy~q9CÃğ¨*ÕÙ8Ô§äÒQŒ,œDXıˆ
ÈìË3ë‹
úGÄ‘X7…gñL‚õ+FÙR“k’u·¬È¶	„7ì+Wä|>oó’ìDÎã{¨‰× éê ˆRÕ	çi¿«[š6Âr(ñ¦ä"ù¦` ó#7gÛöòşëNkqˆí“´/öf™¦‘§ mLR†C³
o­½î€·h„hwb`mà(Õ%á‡“œ¦3lÜ{b#åY±°E0vÛƒÚÖËÈ„ŠİQVÚ…‡‚õn¹ß­¥Mùçe’‚ÔQGù±5s×w6Ş—Ò“Ì²»Ï³·A+.+N(åÙİ í€‡Ï++p+‹:ÿf“( ˜š·U÷9F}åİ‚ôávğn z~—m‘˜ê½Üíoœù0ò×’â‚P¿Tüıİ”ÆÃöFÂüœ-Ò²œšƒIE9L{å%ç£øìÓÔ§ZLÚ{X:=WÏÒyâá~mÂÿª¤†bğh¤èr»jáÏÈ“eÄ\`w¢ìÈ§TxŞ']T"Ç–F·ÇyÑÔ#İ¬ïAŸ²Íê*5µĞ÷Äö«
¼á0öĞ’"Éıxwî=Œ´¿’‡Î¡=ßŠV³©Ö}†«= ´_å€°²}Y8ıo$33pr#É´¸œ!lÿ=‹iù¤Å¸CÛíÜÆ­Á‰ù¶=jÊ*íş[°-W*	(˜. ‚7ù©_˜ÿ›Ïr	ñ‡:¹£oWïd³ßûÎ#¥Wì±=Sá-G»Î‰£¿(j;rİ9cŞ±AĞ.”ŸD^äàf (should_resched(0)) {
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