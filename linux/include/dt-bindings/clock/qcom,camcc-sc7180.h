00d00);
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
		/* Put the parts into