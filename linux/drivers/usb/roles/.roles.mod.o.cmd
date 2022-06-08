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
		ts2->src_sel_val     = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		fallthrough;
	case CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
		/* Defaults for VID B - Analog encoder */
		/* DREQ_POL, SMODE, PUNC_CLK, MCLK_POL Serial bus + punc clk */
		ts1->gen_ctrl_val    = 0x10e;
		ts1->ts_clk_en_val   = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val     = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;

		/* APB_TSVALERR_POL (active low)*/
		ts1->vld_misc_val    = 0x2000;
		ts1->hw_sop_ctrl_val = (0x47 << 16 | 188 << 4 | 0xc);
		cx_write(0x130184, 0xc);

		/* Defaults for VID C */
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_TBS_6920:
		ts1->gen_ctrl_val  = 0x4; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TEVII_S471:
	case CX23885_BOARD_DVBWORLD_2005:
	case CX23885_BOARD_PROF_8000:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		tbs_card_init(dev);
		break;
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
	case CX23885_BOARD_MYGICA_X8507:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_MYGICA_X8558PRO:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR4400:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_STARBURST:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
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
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_MPX885:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_AVERMEDIA_HC81R:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_VIEWCAST_260E:
	case CX23885_BOARD_VIEWCAST_460E:
	case CX23885_BOARD_AVERMEDIA_CE310B:
		dev->sd_cx25840 = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[2].i2c_adap,
				"cx25840", 0x88 >> 1, NULL);
		if (dev->sd_cx25840) {
			/* set host data for clk_freq configuration */
			v4l2_set_subdev_hostdata(dev->sd_cx25840,
						&dev->clk_freq);

			dev->sd_cx25840->grp_id = CX23885_HW_AV_CORE;
			v4l2_subdev_call(dev->sd_cx25840, core, load_fw);
		}
		break;
	}

	switch (dev->board) {
	case CX23885_BOARD_VIEWCAST_260E:
		v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[0].i2c_adap,
				"cs3308", 0x82 >> 1, NULL);
		break;
	case CX23885_BOARD_VIEWCAST_460E:
		/* This cs3308 controls the audio from the breakout cable */
		v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[0].i2c_adap,
				"cs3308", 0x80 >> 1, NULL);
		/* This cs3308 controls the audio from the onboard header */
		v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[0].i2c_adap,
				"cs3308", 0x82 >> 1, NULL);
		break;
	}

	/* AUX-PLL 27MHz CLK */
	switch (dev->board) {
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		netup_initialize(dev);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF: {
		int ret;
		const struct firmware *fw;
		const char *filename = "dvb-netup-altera-01.fw";
		char *action = "configure";
		static struct netup_card_info cinfo;
		struct altera_config netup_config = {
			.dev = dev,
			.action = action,
			.jtag_io = netup_jtag_io,
		};

		netup_initialize(dev);

		netup_get_card_info(&dev->i2c_bus[0].i2c_adap, &cinfo);
		if (netup_card_rev)
			cinfo.rev = netup_card_rev;

		switch (cinfo.rev) {
		case 0x4:
			filename = "dvb-netup-altera-04.fw";
			break;
		default:
			filename = "dvb-netup-altera-01.fw";
			break;
		}
		pr_info("NetUP card rev=0x%x fw_filename=%s\n",
			cinfo.rev, filename);

		ret = request_firmware(&fw, filename, &dev->pci->dev);
		if (ret != 0)
			pr_err("did not find the firmware file '%s'. You can use <kernel_dir>/scripts/get_dvb_firmware to get the firmware.",
			       filename);
		else
			altera_init(&netup_config, fw);

		release_firmware(fw);
		break;
	}
	}
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kmod.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/div64.h>
#include <linux/firmware.h>

#include "cimax2.h"
#include "altera-ci.h"
#include "cx23888-ir.h"
#include "cx23885-ir.h"
#include "cx23885-av.h"
#include "cx23885-input.h"

MODULE_DESCRIPTION("Driver for cx23885 based TV cards");
MODULE_AUTHOR("Steven Toth <stoth@linuxtv.org>");
MODULE_LICENSE("GPL");
MODULE_VERSION(CX23885_VERSION);

/*
 * Some platforms have been found to require periodic resetting of the DMA
 * engine. Ryzen and XEON platforms are known to be affected. The symptom
 * encountered is "mpeg risc op code error". Only Ryzen platforms employ
 * this workaround if the option equals 1. The workaround can be explicitly
 * disabled for all platforms by setting to 0, the workaround can be forced
 * on for any platform by setting to 2.
 */
static unsigned int dma_reset_workaround = 1;
module_param(dma_reset_workaround, int, 0644);
MODULE_PARM_DESC(dma_reset_workaround, "periodic RiSC dma engine reset; 0-force disable, 1-driver detect (default), 2-force enable");

static unsigned int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "enable debug messages");

static unsigned int card[]  = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };
module_param_array(card,  int, NULL, 0444);
MODULE_PARM_DESC(card, "card type");

#define dprintk(level, fmt, arg...)\
	do { if (debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: " fmt), \
		       __func__, ##arg); \
	} while (0)

static unsigned int cx23885_devcount;

#define NO_SYNC_LINE (-1U)

/* FIXME, these allocations will change when
 * analog arrives. The be reviewed.
 * CX23887 Assumptions
 * 1 line = 16 bytes of CDT
 * cmds size = 80
 * cdt size = 16 * linesize
 * iqsize = 64
 * maxlines = 6
 *
 * Address Space:
 * 0x00000000 0x00008fff FIFO clusters
 * 0x00010000 0x000104af Channel Management Data Structures
 * 0x000104b0 0x000104ff Free
 * 0x00010500 0x000108bf 15 channels * iqsize
 * 0x000108c0 0x000108ff Free
 * 0x00010900 0x00010e9f IQ's + Cluster Descriptor Tables
 *                       15 channels * (iqsize + (maxlines * linesize))
 * 0x00010ea0 0x00010xxx Free
 */

static struct sram_channel cx23885_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x10380,
		.cdt		= 0x104c0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "ch2",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10400,
		.cdt		= 0x10580,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH06] = {
		.name		= "TS2 C",
		.cmds_start	= 0x10140,
		.ctrl_start	= 0x10440,
		.cdt		= 0x105e0,
		.fifo_start	= 0x6000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH07] = {
		.name		= "TV Audio",
		.cmds_start	= 0x10190,
		.ctrl_start	= 0x10480,
		.cdt		= 0x10a00,
		.fifo_start	= 0x7000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA6_PTR1,
		.ptr2_reg	= DMA6_PTR2,
		.cnt1_reg	= DMA6_CNT1,
		.cnt2_reg	= DMA6_CNT2,
	},
	[SRAM_CH08] = {
		.name		= "ch8",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA7_PTR1,
		.ptr2_reg	= DMA7_PTR2,
		.cnt1_reg	= DMA7_CNT1,
		.cnt2_reg	= DMA7_CNT2,
	},
	[SRAM_CH09] = {
		.name		= "ch9",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA8_PTR1,
		.ptr2_reg	= DMA8_PTR2,
		.cnt1_reg	= DMA8_CNT1,
		.cnt2_reg	= DMA8_CNT2,
	},
};

static struct sram_channel cx23887_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x105b0,
		.cdt		= 0x107b0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "VID A (VBI)",
		.cmds_start	= 0x10050,
		.ctrl_start	= 0x105F0,
		.cdt		= 0x10810,
		.fifo_start	= 0x3000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10630,
		.cdt		= 0x10870,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH06] = {
		.name		= "TS2 C",
		.cmds_start	= 0x10140,
		.ctrl_start	= 0x10670,
		.cdt		= 0x108d0,
		.fifo_start	= 0x6000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH07] = {
		.name		= "TV Audio",
		.cmds_start	= 0x10190,
		.ctrl_start	= 0x106B0,
		.cdt		= 0x10930,
		.fifo_start	= 0x7000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA6_PTR1,
		.ptr2_reg	= DMA6_PTR2,
		.cnt1_reg	= DMA6_CNT1,
		.cnt2_reg	= DMA6_CNT2,
	},
	[SRAM_CH08] = {
		.name		= "ch8",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA7_PTR1,
		.ptr2_reg	= DMA7_PTR2,
		.cnt1_reg	= DMA7_CNT1,
		.cnt2_reg	= DMA7_CNT2,
	},
	[SRAM_CH09] = {
		.name		= "ch9",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA8_PTR1,
		.ptr2_reg	= DMA8_PTR2,
		.cnt1_reg	= DMA8_CNT1,
		.cnt2_reg	= DMA8_CNT2,
	},
};

static void cx23885_irq_add(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_loc