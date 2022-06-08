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
		ts2->src_sel