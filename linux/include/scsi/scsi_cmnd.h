00000);
	cx_write(PAD_CTRL, 0x00500300);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
	msleep(100);

	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH01],
		720*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH03],
		188*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH04], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH05], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH06],
		188*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH07], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH08], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH09], 128, 0);

	cx23885_gpio_setup(dev);

	cx23885_irq_get_mask(dev);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
}


static int cx23885_pci_quirks(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	/* The cx23885 bridge has a weird bug which causes NMI to be asserted
	 * when DMA begins if RDR_TLCTL0 bit4 is not cleared. It does not
	 * occur on the cx23887 bridge.
	 */
	if (dev->bridge == CX23885_BRIDGE_885)
		cx_clear(RDR_TLCTL0, 1 << 4);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
	return 0;
}

static int get_resources(struct cx23885_dev *dev)
{
	if (request_mem_region(pci_resource_start(dev->pci, 0),
			       pci_resource_len(dev->pci, 0),
			       dev->name))
		return 0;

	pr_err("%s: can't get MMIO memory @ 0x%llx\n",
	       dev->name, (unsigned long long)pci_resource_start(dev->pci, 0));

	return -EBUSY;
}

static int cx23885_init_tsport(struct cx23885_dev *dev,
	struct cx23885_tsport *port, int portno)
{
	dprintk(1, "%s(portno=%d)\n", __func__, portno);

	/* Transport bus init dma queue  - Common settings */
	port->dma_ctl_val        = 0x11; /* Enable RISC controller and Fifo */
	port->ts_int_msk_val     = 0x1111; /* TS port bits for RISC */
	port->vld_misc_val       = 0x0;
	port->hw_sop_ctrl_val    = (0x47 << 16 | 188 << 4);

	spin_lock_init(&port->slock);
	port->dev = dev;
	port->nr = portno;

	INIT_LIST_HEAD(&port->mpegq.active);
	mutex_init(&port->frontends.lock);
	INIT_LIST_HEAD(&port->frontends.felist);
	port->frontends.active_fe_id = 0;

	/* This should be hardcoded allow a single frontend
	 * attachment to this tsport, keeping the -dvb.c
	 * code clean and safe.
	 */
	if (!port->num_frontends)
		port->num_frontends = 1;

	switch (portno) {
	case 1:
		port->reg_gpcnt          = VID_B_GPCNT;
		port->reg_gpcnt_ctl      = VID_B_GPCNT_CTL;
		port->reg_dma_ctl        = VID_B_DMA_CTL;
		port->reg_lngth          = VID_B_LNGTH;
		port->reg_hw_sop_ctrl    = VID_B_HW_SOP_CTL;
		port->reg_gen_ctrl       = VID_B_GEN_CTL;
		port->reg_bd_pkt_status  = VID_B_BD_PKT_STATUS;
		port->reg_sop_status     = VID_B_SOP_STATUS;
		port->reg_fifo_ovfl_stat = VID_B_FIFO_OVFL_STAT;
		port->reg_vld_misc       = VID_B_VLD_MISC;
		port->reg_ts_clk_en      = VID_B_TS_CLK_EN;
		port->reg_src_sel        = VID_B_SRC_SEL;
		port->reg_ts_int_msk     = VID_B_INT_MSK;
		port->reg_ts_int_stat    = VID_B_INT_STAT;
		port->sram_chno          = SRAM_CH03; /* VID_B */
		port->pci_irqmask        = 0x02; /* VID_B bit1 */
		break;
	case 2:
		port->reg_gpcnt          = VID_C_GPCNT;
		port->reg_gpcnt_ctl      = VID_C_GPCNT_CTL;
		port->reg_dma_ctl        = VID_C_DMA_CTL;
		port->reg_lngth          = VID_C_LNGTH;
		port->reg_hw_sop_ctrl    = VID_C_HW_SOP_CTL;
		port->reg_gen_ctrl       = VID_C_GEN_CTL;
		port->reg_bd_pkt_status  = VID_C_BD_PKT_STATUS;
		port->reg_sop_status     = VID_C_SOP_STATUS;
		port->reg_fifo_ovfl_stat = VID_C_FIFO_OVFL_STAT;
		port->reg_vld_misc       = VID_C_VLD_MISC;
		port->reg_ts_clk_en      = VID_C_TS_CLK_EN;
		port->reg_src_sel        = 0;
		port->reg_ts_int_msk     = VID_C_INT_MSK;
		port->reg_ts_int_stat    = VID_C_INT_STAT;
		port->sram_chno          = SRAM_CH06; /* VID_C */
		port->pci_irqmask        = 0x04; /* VID_C bit2 */
		break;
	default:
		BUG();
	}

	return 0;
}

static void cx23885_dev_checkrevision(struct cx23885_dev *dev)
{
	switch (cx_read(RDR_CFG2) & 0xff) {
	case 0x00:
		/* cx23885 */
		dev->hwrevision = 0xa0;
		break;
	case 0x01:
		/* CX23885-12Z */
		dev->hwrevision = 0xa1;
		break;
	case 0x02:
		/* CX23885-13Z/14Z */
		dev->hwrevision = 0xb0;
		break;
	case 0x03:
		if (dev->pci->device == 0x8880) {
			/* CX23888-21Z/22Z */
			dev->hwrevision = 0xc0;
		} else {
			/* CX23885-14Z */
			dev->hwrevision = 0xa4;
		}
		break;
	case 0x04:
		if (dev->pci->device == 0x8880) {
			/* CX23888-31Z */
			dev->hwrevision = 0xd0;
		} else {
			/* CX23885-15Z, CX23888-31Z */
			dev->hwrevision = 0xa5;
		}
		break;
	case 0x0e:
		/* CX23887-15Z */
		dev->hwrevision = 0xc0;
		break;
	case 0x0f:
		/* CX23887-14Z */
		dev->hwrevision = 0xb1;
		break;
	default:
		pr_err("%s() New hardware revision found 0x%x\n",
		       __func__, dev->hwrevision);
	}
	if (dev->hwrevision)
		pr_info("%s() Hardware revision = 0x%02x\n",
			__func__, dev->hwrevision);
	else
		pr_err("%s() Hardware revision unknown 0x%x\n",
		       __func__, dev->hwrevision);
}

/* Find the first v4l2_subdev member of the group id in hw */
struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw)
{
	struct v4l2_subdev *result = NULL;
	struct v4l2_subdev *sd;

	spin_lock(&dev->v4l2_dev.lock);
	v4l2_device_for_each_subdev(sd, &dev->v4l2_dev) {
		if (sd->grp_id == hw) {
			result = sd;
			break;
		}
	}
	spin_unlock(&dev->v4l2_dev.lock);
	return result;
}

static int cx23885_dev_setup(struct cx23885_dev *dev)
{
	int i;

	spin_lock_init(&dev->pci_irqmask_lock);
	spin_lock_init(&dev->slock);

	mutex_init(&dev->lock);
	mutex_init(&dev->gpio_lock);

	atomic_inc(&dev->refcount);

	dev->nr = cx23885_devcount++;
	sprintf(dev->name, "cx23885[%d]", dev->nr);

	/* Configure the internal memory */
	if (dev->pci->device == 0x8880) {
		/* Could be 887 or 888, assume an 888 default */
		dev->bridge = CX23885_BRIDGE_888;
		/* Apply a sensible clock frequency for the PCIe bridge */
		dev->clk_freq = 50000000;
		dev->sram_channels = cx23887_sram_channels;
	} else
	if (dev->pci->device == 0x8852) {
		dev->bridge = CX23885_BRIDGE_885;
		/* Apply a sensible clock frequency for the PCIe bridge */
		dev->clk_freq = 28000000;
		dev->sram_channels = cx23885_sram_channels;
	} else
		BUG();

	dprintk(1, "%s() Memory configured for PCIe bridge type %d\n",
		__func__, dev->bridge);

	/* board config */
	dev->board = UNSET;
	if (card[dev->nr] < cx23885_bcount)
		dev->board = card[dev->nr];
	for (i = 0; UNSET == dev->board  &&  i < cx23885_idcount; i++)
		if (dev->pci->subsystem_vendor == cx23885_subids[i].subvendor &&
		    dev->pci->subsystem_device == cx23885_subids[i].subdevice)
			dev->board = cx23885_subids[i].card;
	if (UNSET == dev->board) {
		dev->board = CX23885_BOARD_UNKNOWN;
		cx23885_card_list(dev);
	}

	if (dev->pci->device == 0x8852) {
		/* no DIF on cx23885, so no analog tuner support possible */
		if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC)
			dev->board = CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885;
		else if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB)
			dev->board = CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885;
	}

	/* If the user specific a clk freq override, apply it */
	if (cx23885_boards[dev->board].clk_freq > 0)
		dev->clk_freq = cx23885_boards[dev->board].clk_freq;

	if (dev->board == CX23885_BOARD_HAUPPAUGE_IMPACTVCBE &&
		dev->pci->subsystem_device == 0x7137) {
		/* Hauppauge ImpactVCBe device ID 0x7137 is populated
		 * with an 888, and a 25Mhz crystal, instead of the
		 * usual third overtone 50Mhz. The default clock rate must
		 * be overridden so the cx25840 is properly configured
		 */
		dev->clk_freq = 25000000;
	}

	dev->pci_bus  = dev->pci->bus->number;
	dev->pci_slot = PCI_SLOT(dev->pci->devfn);
	cx23885_irq_add(dev, 0x001f00);

	/* External Master 1 Bus */
	dev->i2c_bus[0].nr = 0;
	dev->i2c_bus[0].dev = dev;
	dev->i2c_bus[0].reg_stat  = I2C1_STAT;
	dev->i2c_bus[0].reg_ctrl  = I2C1_CTRL;
	dev->i2c_bus[0].reg_addr  = I2C1_ADDR;
	dev->i2c_bus[0].reg_rdata = I2C1_RDATA;
	dev->i2c_bus[0].reg_wdata = I2C1_WDATA;
	dev->i2c_bus[0].i2c_period = (0x9d << 24); /* 100kHz */

	/* External Master 2 Bus */
	dev->i2c_bus[1].nr = 1;
	dev->i2c_bus[1].dev = dev;
	dev->i2c_bus[1].reg_stat  = I2C2_STAT;
	dev->i2c_bus[1].reg_ctrl  = I2C2_CTRL;
	dev->i2c_bus[1].reg_addr  = I2C2_ADDR;
	dev->i2c_bus[1].reg_rdata = I2C2_RDATA;
	dev->i2c_bus[1].reg_wdata = I2C2_WDATA;
	dev->i2c_bus[1].i2c_period = (0x9d << 24); /* 100kHz */

	/* Internal Master 3 Bus */
	dev->i2c_bus[2].nr = 2;
	dev->i2c_bus[2].dev = dev;
	dev->i2c_bus[2].reg_stat  = I2C3_STAT;
	dev->i2c_bus[2].reg_ctrl  = I2C3_CTRL;
	dev->i2c_bus[2].reg_addr  = I2C3_ADDR;
	dev->i2c_bus[2].reg_rdata = I2C3_RDATA;
	dev->i2c_bus[2].reg_wdata = I2C3_WDATA;
	dev->i2c_bus[2].i2c_period = (0x07 << 24); /* 1.95MHz */

	if ((cx23885_boards[dev->board].portb == CX23885_MPEG_DVB) ||
		(cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER))
		cx23885_init_tsport(dev, &dev->ts1, 1);

	if ((cx23885_boards[dev->board].portc == CX23885_MPEG_DVB) ||
		(cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER))
		cx23885_init_tsport(dev, &dev->ts2, 2);

	if (get_resources(dev) < 0) {
		pr_err("CORE %s No more PCIe resources for subsystem: %04x:%04x\n",
		       dev->name, dev->pci->subsystem_vendor,
		       dev->pci->subsystem_device);

		cx23885_devcount--;
		return -ENODEV;
	}

	/* PCIe stuff */
	dev->lmmio = ioremap(pci_resource_start(dev->pci, 0),
			     pci_resource_len(dev->pci, 0));

	dev->bmmio = (u8 __iomem *)dev->lmmio;

	pr_info("CORE %s: subsystem: %04x:%04x, board: %s [card=%d,%s]\n",
		dev->name, dev->pci->subsystem_vendor,
		dev->pci->subsystem_device, cx23885_boards[dev->board].name,
		dev->board, card[dev->nr] == dev->board ?
		"insmod option" : "autodetected");

	cx23885_pci_quirks(dev);

	/* Assume some sensible defaults */
	dev->tuner_type = cx23885_boards[dev->board].tuner_type;
	dev->tuner_addr = cx23885_boards[dev->board].tuner_addr;
	dev->tuner_bus = cx23885_boards[dev->board].tuner_bus;
	dev->radio_type = cx23885_boards[dev->board].radio_type;
	dev->radio_addr = cx23885_boards[dev->board].radio_addr;

	dprintk(1, "%s() tuner_type = 0x%x tuner_addr = 0x%x tuner_bus = %d\n",
		__func__, dev->tuner_type, dev->tuner_addr, dev->tuner_bus);
	dprintk(1, 