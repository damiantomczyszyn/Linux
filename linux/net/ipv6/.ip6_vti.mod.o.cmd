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
䝚�9�h�O&������!��ԾNV��_�&o��.^Sy0�1ݭM@�s0i�k�gt��I�*�2��3��V5�$ĀT���4_��=7�^�e&���