00000036);
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
		cx