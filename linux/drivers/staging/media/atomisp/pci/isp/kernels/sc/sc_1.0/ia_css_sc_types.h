 of reset */
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
		ms