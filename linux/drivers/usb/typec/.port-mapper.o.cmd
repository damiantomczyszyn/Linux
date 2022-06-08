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
	dprintk(1, "%s() radio_type = 0x%x radio_addr = 0x%x\n",
		__func__, dev->radio_type, dev->radio_addr);

	/* The cx23417 encoder has GPIO's that need to be initialised
	 * before DVB, so that demodulators and tuners are out of
	 * reset before DVB uses them.
	 */
	if ((cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) ||
		(cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER))
			cx23885_mc417_init(dev);

	/* init hardware */
	cx23885_reset(dev);

	cx23885_i2c_register(&dev->i2c_bus[0]);
	cx23885_i2c_register(&dev->i2c_bus[1]);
	cx23885_i2c_register(&dev->i2c_bus[2]);
	cx23885_card_setup(dev);
	call_all(dev, tuner, standby);
	cx23885_ir_init(dev);

	if (dev->board == CX23885_BOARD_VIEWCAST_460E) {
		/*
		 * GPIOs 9/8 are input detection bits for the breakout video
		 * (gpio 8) and audio (gpio 9) cables. When they're attached,
		 * this gpios are pulled high. Make sure these GPIOs are marked
		 * as inputs.
		 */
		cx23885_gpio_enable(dev, 0x300, 0);
	}

	if (cx23885_boards[dev->board].porta == CX23885_ANALOG_VIDEO) {
		if (cx23885_video_register(dev) < 0) {
			pr_err("%s() Failed to register analog video adapters on VID_A\n",
			       __func__);
		}
	}

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_DVB) {
		if (cx23885_boards[dev->board].num_fds_portb)
			dev->ts1.num_frontends =
				cx23885_boards[dev->board].num_fds_portb;
		if (cx23885_dvb_register(&dev->ts1) < 0) {
			pr_err("%s() Failed to register dvb adapters on VID_B\n",
			       __func__);
		}
	} else
	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) {
		if (cx23885_417_register(dev) < 0) {
			pr_err("%s() Failed to register 417 on VID_B\n",
			       __func__);
		}
	}

	if (cx23885_boards[dev->board].portc == CX23885_MPEG_DVB) {
		if (cx23885_boards[dev->board].num_fds_portc)
			dev->ts2.num_frontends =
				cx23885_boards[dev->board].num_fds_portc;
		if (cx23885_dvb_register(&dev->ts2) < 0) {
			pr_err("%s() Failed to register dvb on VID_C\n",
			       __func__);
		}
	} else
	if (cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER) {
		if (cx23885_417_register(dev) < 0) {
			pr_err("%s() Failed to register 417 on VID_C\n",
			       __func__);
		}
	}

	cx23885_dev_checkrevision(dev);

	/* disable MSI for NetUP cards, otherwise CI is not working */
	if (cx23885_boards[dev->board].ci_type > 0)
		cx_clear(RDR_RDRCTL1, 1 << 8);

	switch (dev->board) {
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TEVII_S471:
		cx_clear(RDR_RDRCTL1, 1 << 8);
		break;
	}

	return 0;
}

static void cx23885_dev_unregister(struct cx23885_dev *dev)
{
	release_mem_region(pci_resource_start(dev->pci, 0),
			   pci_resource_len(dev->pci, 0));

	if (!atomic_dec_and_test(&dev->refcount))
		return;

	if (cx23885_boards[dev->board].porta == CX23885_ANALOG_VIDEO)
		cx23885_video_unregister(dev);

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_DVB)
		cx23885_dvb_unregister(&dev->ts1);

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
		cx23885_417_unregister(dev);

	if (cx23885_boards[dev->board].portc == CX23885_MPEG_DVB)
		cx23885_dvb_unregister(&dev->ts2);

	if (cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER)
		cx23885_417_unregister(dev);

	cx23885_i2c_unregister(&dev->i2c_bus[2]);
	cx23885_i2c_unregister(&dev->i2c_bus[1]);
	cx23885_i2c_unregister(&dev->i2c_bus[0]);

	iounmap(dev->lmmio);
}

static __le32 *cx23885_risc_field(__le32 *rp, struct scatterlist *sglist,
			       unsigned int offset, u32 sync_line,
			       unsigned int bpl, unsigned int padding,
			       unsigned int lines,  unsigned int lpi, bool jump)
{
	struct scatterlist *sg;
	unsigned int line, todo, sol;


	if (jump) {
		*(rp++) = cpu_to_le32(RISC_JUMP);
		*(rp++) = cpu_to_le32(0);
		*(rp++) = cpu_to_le32(0); /* bits 63-32 */
	}

	/* sync instruction */
	if (sync_line != NO_SYNC_LINE)
		*(rp++) = cpu_to_le32(RISC_RESYNC | sync_line);

	/* scan lines */
	sg = sglist;
	for (line = 0; line < lines; line++) {
		while (offset && offset >= sg_dma_len(sg)) {
			offset -= sg_dma_len(sg);
			sg = sg_next(sg);
		}

		if (lpi && line > 0 && !(line % lpi))
			sol = RISC_SOL | RISC_IRQ1 | RISC_CNT_INC;
		else
			sol = RISC_SOL;

		if (bpl <= sg_dma_len(sg)-offset) {
			/* fits into current chunk */
			*(rp++) = cpu_to_le32(RISC_WRITE|sol|RISC_EOL|bpl);
			*(rp++) = cpu_to_le32(sg_dma_address(sg)+offset);
			*(rp++) = cpu_to_le32(0); /* bits 63-32 */
			offset += bpl;
		} else {
			/* scanline needs to be split */
			todo = bpl;
			*(rp++) = cpu_to_le32(RISC_WRITE|sol|
					    (sg_dma_len(sg)-offset));
			*(rp++) = cpu_to_le32(sg_dma_address(sg)+offset);
			*(rp++) = cpu_to_le32(0); /* bits 63-32 */
			todo -= (sg_dma_len(sg)-offset);
			offset = 0;
			sg = sg_next(sg);
			while (todo > sg_dma_len(sg)) {
				*(rp++) = cpu_to_le32(RISC_WRITE|
						    sg_dma_len(sg));
				*(rp++) = cpu_to_le32(sg_dma_address(sg));
				*(rp++) = cpu_to_le32(0); /* bits 63-32 */
				todo -= sg_dma_len(sg);
				sg = sg_next(sg);
			}
			*(rp++) = cpu_to_le32(RISC_WRITE|RISC_EOL|todo);
			*(rp++) = cpu_to_le32(sg_dma_address(sg));
			*(rp++) = cpu_to_le32(0); /* bits 63-32 */
			offset += todo;
		}
		offset += padding;
	}

	return rp;
}

int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
			struct scatterlist *sglist, unsigned int top_offset,
			unsigned int bottom_offset, unsigned int bpl,
			unsigned int padding, unsigned int lines)
{
	u32 instructions, fields;
	__le32 *rp;

	fields = 0;
	if (UNSET != top_offset)
		fields++;
	if (UNSET != bottom_offset)
		fields++;

	/* estimate risc mem: worst case is one write per page border +
	   one write per scan line + syncs + jump (all 2 dwords).  Padding
	   can cause next bpl to start close to a page border.  First DMA
	   region may be smaller than PAGE_SIZE */
	/* write and jump need and extra dword */
	instructions  = fields * (1 + ((bpl + padding) * lines)
		/ PAGE_SIZE + lines);
	instructions += 5;
	risc->size = instructions * 12;
	risc->cpu = dma_alloc_coherent(&pci->dev, risc->size, &risc->dma,
				       GFP_KERNEL);
	if (risc->cpu == NULL)
		return -ENOMEM;

	/* write risc instructions */
	rp = risc->cpu;
	if (UNSET != top_offset)
		rp = cx23885_risc_field(rp, sglist, top_offset, 0,
					bpl, padding, lines, 0, true);
	if (UNSET != bottom_offset)
		rp = cx23885_risc_field(rp, sglist, bottom_offset, 0x200,
					bpl, padding, lines, 0, UNSET == top_offset);

	/* save pointer to jmp instruction address */
	risc->jmp = rp;
	BUG_ON((risc->jmp - risc->cpu + 2) * sizeof(*risc->cpu) > risc->size);
	return 0;
}

int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines, unsigned int lpi)
{
	u32 instructions;
	__le32 *rp;

	/* estimate risc mem: worst case is one write per page border +
	   one write per scan line + syncs + jump (all 2 dwords).  Here
	   there is no padding and no sync.  First DMA region may be smaller
	   than PAGE_SIZE */
	/* Jump and write need an extra dword */
	instructions  = 1 + (bpl * lines) / PAGE_SIZE + lines;
	instructions += 4;

	risc->size = instructions * 12;
	risc->cpu = dma_alloc_coherent(&pci->dev, risc->size, &risc->dma,
				       GFP_KERNEL);
	if (risc->cpu == NULL)
		return -ENOMEM;

	/* write risc instructions */
	rp = risc->cpu;
	rp = cx23885_risc_field(rp, sglist, 0, NO_SYNC_LINE,
				bpl, 0, lines, lpi, lpi == 0);

	/* save pointer to jmp instruction address */
	risc->jmp = rp;
	BUG_ON((risc->jmp - risc->cpu + 2) * sizeof(*risc->cpu) > risc->size);
	return 0;
}

int cx23885_risc_vbibuffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
			struct scatterlist *sglist, unsigned int top_offset,
			unsigned int bottom_offset, unsigned int bpl,
			unsigned int padding, unsigned int lines)
{
	u32 instructions, fields;
	__le32 *rp;

	fields = 0;
	if (UNSET != top_offset)
		fields++;
	if (UNSET != bottom_offset)
		fields++;

	/* estimate risc mem: worst case is one write per page border +
	   one write per scan line + syncs + jump (all 2 dwords).  Padding
	   can cause next bpl to start close to a page border.  First DMA
	   region may be smaller than PAGE_SIZE */
	/* write and jump need and extra dword */
	instructions  = fields * (1 + ((bpl + padding) * lines)
		/ PAGE_SIZE + lines);
	instructions += 5;
	risc->size = instructions * 12;
	risc->cpu = dma_alloc_coherent(&pci->dev, risc->size, &risc->dma,
				       GFP_KERNEL);
	if (risc->cpu == NULL)
		return -ENOMEM;
	/* write risc instructions */
	rp = risc->cpu;

	/* Sync to line 6, so US CC line 21 will appear in line '12'
	 * in the userland vbi payload */
	if (UNSET != top_offset)
		rp = cx23885_risc_field(rp, sglist, top_offset, 0,
					bpl, padding, lines, 0, true);

	if (UNSET != bottom_offset)
		rp = cx23885_risc_field(rp, sglist, bottom_offset, 0x200,
					bpl, padding, lines, 0, UNSET == top_offset);



	/* save pointer to jmp instruction address */
	risc->jmp = rp;
	BUG_ON((risc->jmp - risc->cpu + 2) * sizeof(*risc->cpu) > risc->size);
	return 0;
}


void cx23885_free_buffer(struct cx23885_dev *dev, struct cx23885_buffer *buf)
{
	struct cx23885_riscmem *risc = &buf->risc;

	dma_free_coherent(&dev->pci->dev, risc->size, risc->cpu, risc->dma);
}

static void cx23885_tsport_reg_dump(struct cx23885_tsport *port)
{
	struct cx23885_dev *dev = port->dev;

	dprintk(1, "%s() Register Dump\n", __func__);
	dprintk(1, "%s() DEV_CNTRL2               0x%08X\n", __func__,
		cx_read(DEV_CNTRL2));
	dprintk(1, "%s() PCI_INT_MSK              0x%08X\n", __func__,
		cx23885_irq_get_mask(dev));
	dprintk(1, "%s() AUD_INT_INT_MSK          0x%08X\n", __func__,
		cx_read(AUDIO_INT_INT_MSK));
	dprintk(1, "%s() AUD_INT_DMA_CTL          0x%08X\n", __func__,
		cx_read(AUD_INT_DMA_CTL));
	dprintk(1, "%s() AUD_EXT_INT_MSK          0x%08X\n", __func__,
		cx_read(AUDIO_EXT_INT_MSK));
	dprintk(1, "%s() AUD_EXT_DMA_CTL          0x%08X\n", __func__,
		cx_read(AUD_EXT_DMA_CTL));
	dprintk(1, "%s() PAD_CTRL                 0x%08X\n", __func__,
		cx_read(PAD_CTRL));
	dprintk(1, "%s() ALT_PIN_OUT_SEL          0x%08X\n", __func__,
		cx_read(ALT_PIN_OUT_SEL));
	dprintk(1, "%s() GPIO2                    0x%08X\n", __func__,
		cx_read(GPIO2));
	dprintk(1, "%s() gpcnt(0x%08X)          0x%08X\n", __func__,
		port->reg_gpcnt, cx_read(port->reg_gpcnt));
	dprintk(1, "%s() gpcnt_ctl(0x%08X)      0x%08x\n", __func__,
		port->reg_gpcnt_ctl, cx_read(port->reg_gpcnt_ctl));
	dprintk(1, "%s() dma_ctl(0x%08X)        0x%08x\n", __func__,
		port->reg_dma_ctl, cx_read(port->reg_dma_ctl));
	if (port->reg_src_sel)
		dprintk(1, "%s() src_sel(0x%08X)        0x%08x\n", __func__,
			port->reg_src_sel, cx_read(port->reg_src_sel));
	dprintk(1, "%s() lngth(0x%08X)          0x%08x\n", __func__,
		port->reg_lngth, cx_read(port->reg_lngth));
	dprintk(1, "%s() hw_sop_ctrl(0x%08X)    0x%08x\n", __func__,
		port->reg_hw_sop_ctrl, cx_read(port->reg_hw_sop_ctrl));
	dprintk(1, "%s() gen_ctrl(0x%08X)       0x%08x\n", __func__,
		port->reg_gen_ctrl, cx_read(port->reg_gen_ctrl));
	dprintk(1, "%s() bd_pkt_status(0x%08X)  0x%08x\n", __func__,
		port->reg_bd_pkt_status, cx_read(port->reg_bd_pkt_status));
	dprintk(1, "%s() sop_status(0x%08X)     0x%08x\n", __func__,
		port->reg_sop_status, cx_read(port->reg_sop_status));
	dprintk(1, "%s() fifo_ovfl_stat(0x%08X) 0x%08x\n", __func__,
		port->reg_fifo_ovfl_stat, cx_read(port->reg_fifo_ovfl_stat));
	dprintk(1, "%s() vld_misc(0x%08X)       0x%08x\n", __func__,
		port->reg_vld_misc, cx_read(port->reg_vld_misc));
	dprintk(1, "%s() ts_clk_en(0x%08X)      0x%08x\n", __func__,
		port->reg_ts_clk_en, cx_read(port->reg_ts_clk_en));
	dprintk(1, "%s() ts_int_msk(0x%08X)     0x%08x\n", __func__,
		port->reg_ts_int_msk, cx_read(port->reg_ts_int_msk));
	dprintk(1, "%s() ts_int_status(0x%08X)  0x%08x\n", __func__,
		port->reg_ts_int_stat, cx_read(port->reg_ts_int_stat));
	dprintk(1, "%s() PCI_INT_STAT           0x%08X\n", __func__,
		cx_read(PCI_INT_STAT));
	dprintk(1, "%s() VID_B_INT_MSTAT        0x%08X\n", __func__,
		cx_read(VID_B_INT_MSTAT));
	dprintk(1, "%s() VID_B_INT_SSTAT        0x%08X\n", __func__,
		cx_read(VID_B_INT_SSTAT));
	dprintk(1, "%s() VID_C_INT_MSTAT        0x%08X\n", __func__,
		cx_read(VID_C_INT_MSTAT));
	dprintk(1, "%s() VID_C_INT_SSTAT        0x%08X\n", __func__,
		cx_read(VID_C_INT_SSTAT));
}

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf)
{
	struct cx23885_dev *dev = port->dev;
	u32 reg;

	dprintk(1, "%s() w: %d, h: %d, f: %d\n", __func__,
		dev->width, dev->height, dev->field);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);

	/* Stop the fifo and risc engine for this port */
	cx_clear(port->reg_dma_ctl, port->dma_ctl_val);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev,
				   &dev->sram_channels[port->sram_chno],
				   port->ts_packet_size, buf->risc.dma);
	if (debug > 5) {
		cx23885_sram_channel_dump(dev,
			&dev->sram_channels[port->sram_chno]);
		cx23885_risc_disasm(port, &buf->risc);
	}

	/* write TS length to chip */
	cx_write(port->reg_lngth, port->ts_packet_size);

	if ((!(cx23885_boards[dev->board].portb & CX23885_MPEG_DVB)) &&
		(!(cx23885_boards[dev->board].portc & CX23885_MPEG_DVB))) {
		pr_err("%s() Unsupported .portb/c (0x%08x)/(0x%08x)\n",
			__func__,
			cx23885_boards[dev->board].portb,
			cx23885_boards[dev->board].portc);
		return -EINVAL;
	}

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
		cx23885_av_clk(dev, 0);

	udelay(100);

	/* If the port supports SRC SELECT, configure it */
	if (port->reg_src_sel)
		cx_write(port->reg_src_sel, port->src_sel_val);

	cx_write(port->reg_hw_sop_ctrl, port->hw_sop_ctrl_val);
	cx_write(port->reg_ts_clk_en, port->ts_clk_en_val);
	cx_write(port->reg_vld_misc, port->vld_misc_val);
	cx_write(port->reg_gen_ctrl, port->gen_ctrl_val);
	udelay(100);

	/* NOTE: this is 2 (reserved) for portb, does it matter? */
	/* reset counter to zero */
	cx_write(port->reg_gpcnt_ctl, 3);
	q->count = 0;

	/* Set VIDB pins to input */
	if (cx23885_boards[dev->board].portb == CX23885_MPEG_DVB) {
		reg = cx_read(PAD_CTRL);
		reg &= ~0x3; /* Clear TS1_OE & TS1_SOP_OE */
		cx_write(PAD_CTRL, reg);
	}

	/* Set VIDC pins to input */
	if (cx23885_boards[dev->board].portc == CX23885_MPEG_DVB) {
		reg = cx_read(PAD_CTRL);
		reg &= ~0x4; /* Clear TS2_SOP_OE */
		cx_write(PAD_CTRL, reg);
	}

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) {

		reg = cx_read(PAD_CTRL);
		reg = reg & ~0x1;    /* Clear TS1_OE */

		/* FIXME, bit 2 writing here is questionable */
		/* set TS1_SOP_OE and TS1_OE_HI */
		reg = reg | 0xa;
		cx_write(PAD_CTRL, reg);

		/* Sets MOE_CLK_DIS to disable MoE clock */
		/* sets MCLK_DLY_SEL/BCLK_DLY_SEL to 1 buffer delay each */
		cx_write(CLK_DELAY, cx_read(CLK_DELAY) | 0x80000011);

		/* ALT_GPIO_ALT_SET: GPIO[0]
		 * IR_ALT_TX_SEL: GPIO[1]
		 * GPIO1_ALT_SEL: VIP_656_DATA[0]
		 * GPIO0_ALT_SEL: VIP_656_CLK
		 */
		cx_write(ALT_PIN_OUT_SEL, 0x10100045);
	}

	switch (dev->bridge) {
	case CX23885_BRIDGE_885:
	case CX23885_BRIDGE_887:
	case CX23885_BRIDGE_888:
		/* enable irqs */
		dprintk(1, "%s() enabling TS int's and DMA\n", __func__);
		/* clear dma in progress */
		cx23885_clear_bridge_error(dev);
		cx_set(port->reg_ts_int_msk,  port->ts_int_msk_val);
		cx_set(port->reg_dma_ctl, port->dma_ctl_val);

		/* clear dma in progress */
		cx23885_clear_bridge_error(dev);
		cx23885_irq_add(dev, port->pci_irqmask);
		cx23885_irq_enable_all(dev);

		/* clear dma in progress */
		cx23885_clear_bridge_error(dev);
		break;
	default:
		BUG();
	}

	cx_set(DEV_CNTRL2, (1<<5)); /* Enable RISC controller */
	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
		cx23885_av_clk(dev, 1);

	if (debug > 4)
		cx23885_tsport_reg_dump(port);

	cx23885_irq_get_mask(dev);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);

	return 0;
}

static int cx23885_stop_dma(struct cx23885_tsport *port)
{
	struct cx23885_dev *dev = port->dev;
	u32 reg;
	int delay = 0;
	uint32_t reg1_val;
	uint32_t reg2_val;

	dprintk(1, "%s()\n", __func__);

	/* Stop interrupts and DMA */
	cx_clear(port->reg_ts_int_msk, port->ts_int_msk_val);
	cx_clear(port->reg_dma_ctl, port->dma_ctl_val);
	/* just in case wait for any dma to complete before allowing dealloc */
	mdelay(20);
	for (delay = 0; delay < 100; delay++) {
		reg1_val = cx_read(TC_REQ);
		reg2_val = cx_read(TC_REQ_SET);
		if (reg1_val == 0 || reg2_val == 0)
			break;
		mdelay(1);
	}
	dev_dbg(&dev->pci->dev, "delay=%d reg1=0x%08x reg2=0x%08x\n",
		delay, reg1_val, reg2_val);

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) {
		reg = cx_read(PAD_CTRL);

		/* Set TS1_OE */
		reg = reg | 0x1;

		/* clear TS1_SOP_OE and TS1_OE_HI */
		reg = reg & ~0xa;
		cx_write(PAD_CTRL, reg);
		cx_write(port->reg_src_sel, 0);
		cx_write(port->reg_gen_ctrl, 8);
	}

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
		cx23885_av_clk(dev, 0);

	return 0;
}

/* ------------------------------------------------------------------ */

int cx23885_buf_prepare(struct cx23885_buffer *buf, struct cx23885_tsport *port)
{
	struct cx23885_dev *dev = port->dev;
	int size = port->ts_packet_size * port->ts_packet_count;
	struct sg_table *sgt = vb2_dma_sg_plane_desc(&buf->vb.vb2_buf, 0);

	dprintk(1, "%s: %p\n", __func__, buf);
	if (vb2_plane_size(&buf->vb.vb2_buf, 0) < size)
		return -EINVAL;
	vb2_set_plane_payload(&buf->vb.vb2_buf, 0, size);

	cx23885_risc_databuffer(dev->pci, &buf->risc,
				sgt->sgl,
				port->ts_packet_size, port->ts_packet_count, 0);
	return 0;
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back to the start + 12 (skipping
 * the initial JUMP).
 *
 * This is the risc program of the first buffer to be queued if the active list
 * is empty and it just keeps DMAing this buffer without generating any
 * interrupts.
 *
 * If a new buffer is added then the initial JUMP in the code for that buffer
 * will generate an interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
void cx23885_buf_queue(struct cx23885_tsport *port, struct cx23885_buffer *buf)
{
	struct cx23885_buffer    *prev;
	struct cx23885_dev *dev = port->dev;
	struct cx23885_dmaqueue  *cx88q = &port->mpegq;
	unsigned long flags;

	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	spin_lock_irqsave(&dev->slock, flags);
	if (list_empty(&cx88q->active)) {
		list_add_tail(&buf->queue, &cx88q->active);
		dprintk(1, "[%p/%d] %s - first active\n",
			buf, buf->vb.vb2_buf.index, __func__);
	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(cx88q->active.prev, struct cx23885_buffer,
				  queue);
		list_add_tail(&buf->queue, &cx88q->active);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(1, "[%p/%d] %s - append to active\n",
			 buf, buf->vb.vb2_buf.index, __func__);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

/* ----------------------------------------------------------- */

static void do_cancel_buffers(struct cx23885_tsport *port, char *reason)
{
	struct cx23885_dmaqueue *q = &port->mpegq;
	struct cx23885_buffer *buf;
	unsigned long flags;

	spin_lock_irqsave(&port->slock, flags);
	while (!list_empty(&q->active)) {
		buf = list_entry(q->active.next, struct cx23885_buffer,
				 queue);
		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
		dprintk(1, "[%p/%d] %s - dma=0x%08lx\n",
			buf, buf->vb.vb2_buf.index, reason,
			(unsigned long)buf->risc.dma);
	}
	spin_unlock_irqrestore(&port->slock, flags);
}

void cx23885_cancel_buffers(struct cx23885_tsport *port)
{
	dprintk(1, "%s()\n", __func__);
	cx23885_stop_dma(port);
	do_cancel_buffers(port, "cancel");
}

int cx23885_irq_417(struct cx23885_dev *dev, u32 status)
{
	/* FIXME: port1 assumption here. */
	struct cx23885_tsport *port = &dev->ts1;
	int count = 0;
	int handled = 0;

	if (status == 0)
		return handled;

	count = cx_read(port->reg_gpcnt);
	dprintk(7, "status: 0x%08x  mask: 0x%08x count: 0x%x\n",
		status, cx_read(port->reg_ts_int_msk), count);

	if ((status & VID_B_MSK_BAD_PKT)         ||
		(status & VID_B_MSK_OPC_ERR)     ||
		(status & VID_B_MSK_VBI_OPC_ERR) ||
		(status & VID_B_MSK_SYNC)        ||
		(status & VID_B_MSK_VBI_SYNC)    ||
		(status & VID_B_MSK_OF)          ||
		(status & VID_B_MSK_VBI_OF)) {
		pr_err("%s: V4L mpeg risc op code error, status = 0x%x\n",
		       dev->name, status);
		if (status & VID_B_MSK_BAD_PKT)
			dprintk(1, "        VID_B_MSK_BAD_PKT\n");
		if (status & VID_B_MSK_OPC_ERR)
			dprintk(1, "        VID_B_MSK_OPC_ERR\n");
		if (status & VID_B_MSK_VBI_OPC_ERR)
			dprintk(1, "        VID_B_MSK_VBI_OPC_ERR\n");
		if (status & VID_B_MSK_SYNC)
			dprintk(1, "        VID_B_MSK_SYNC\n");
		if (status & VID_B_MSK_VBI_SYNC)
			dprintk(1, "        VID_B_MSK_VBI_SYNC\n");
		if (status & VID_B_MSK_OF)
			dprintk(1, "        VID_B_MSK_OF\n");
		if (status & VID_B_MSK_VBI_OF)
			dprintk(1, "        VID_B_MSK_VBI_OF\n");

		cx_clear(port->reg_dma_ctl, port->dma_ctl_val);
		cx23885_sram_channel_dump(dev,
			&dev->sram_channels[port->sram_chno]);
		cx23885_417_check_encoder(dev);
	} else if (status & VID_B_MSK_RISCI1) {
		dprintk(7, "        VID_B_MSK_RISCI1\n");
		spin_lock(&port->slock);
		cx23885_wakeup(port, &port->mpegq, count);
		spin_unlock(&port->slock);
	}
	if (status) {
		cx_write(port->reg_ts_int_stat, status);
		handled = 1;
	}

	return handled;
}

static int cx23885_irq_ts(struct cx23885_tsport *port, u32 status)
{
	struct cx23885_dev *dev = port->dev;
	int handled = 0;
	u32 count;

	if ((status & VID_BC_MSK_OPC_ERR) ||
		(status & VID_BC_MSK_BAD_PKT) ||
		(status & VID_BC_MSK_SYNC) ||
		(status & VID_BC_MSK_OF)) {

		if (status & VID_BC_MSK_OPC_ERR)
			dprintk(7, " (VID_BC_MSK_OPC_ERR 0x%08x)\n",
				VID_BC_MSK_OPC_ERR);

		if (status & VID_BC_MSK_BAD_PKT)
			dprintk(7, " (VID_BC_MSK_BAD_PKT 0x%08x)\n",
				VID_BC_MSK_BAD_PKT);

		if (status & VID_BC_MSK_SYNC)
			dprintk(7, " (VID_BC_MSK_SYNC    0x%08x)\n",
				VID_BC_MSK_SYNC);

		if (status & VID_BC_MSK_OF)
			dprintk(7, " (VID_BC_MSK_OF      0x%08x)\n",
				VID_BC_MSK_OF);

		pr_err("%s: mpeg risc op code error\n", dev->name);

		cx_clear(port->reg_dma_ctl, port->dma_ctl_val);
		cx23885_sram_channel_dump(dev,
			&dev->sram_channels[port->sram_chno]);

	} else if (status & VID_BC_MSK_RISCI1) {

		dprintk(7, " (RISCI1            0x%08x)\n", VID_BC_MSK_RISCI1);

		spin_lock(&port->slock);
		count = cx_read(port->reg_gpcnt);
		cx23885_wakeup(port, &port->mpegq, count);
		spin_unlock(&port->slock);

	}
	if (status) {
		cx_write(port->reg_ts_int_stat, status);
		handled = 1;
	}

	return handled;
}

static irqreturn_t cx23885_irq(int irq, void *dev_id)
{
	struct cx23885_dev *dev = dev_id;
	struct cx23885_tsport *ts1 = &dev->ts1;
	struct cx23885_tsport *ts2 = &dev->ts2;
	u32 pci_status, pci_mask;
	u32 vida_status, vida_mask;
	u32 audint_status, audint_mask;
	u32 ts1_status, ts1_mask;
	u32 ts2_status, ts2_mask;
	int vida_count = 0, ts1_count = 0, ts2_count = 0, handled = 0;
	int audint_count = 0;
	bool subdev_handled;

	pci_status = cx_read(PCI_INT_STAT);
	pci_mask = cx23885_irq_get_mask(dev);
	if ((pci_status & pci_mask) == 0) {
		dprintk(7, "pci_status: 0x%08x  pci_mask: 0x%08x\n",
			pci_status, pci_mask);
		goto out;
	}

	vida_status = cx_read(VID_A_INT_STAT);
	vida_mask = cx_read(VID_A_INT_MSK);
	audint_status = cx_read(AUDIO_INT_INT_STAT);
	audint_mask = cx_read(AUDIO_INT_INT_MSK);
	ts1_status = cx_read(VID_B_INT_STAT);
	ts1_mask = cx_read(VID_B_INT_MSK);
	ts2_status = cx_read(VID_C_INT_STAT);
	ts2_mask = cx_read(VID_C_INT_MSK);

	if (((pci_status & pci_mask) == 0) &&
		((ts2_status & ts2_mask) == 0) &&
		((ts1_status & ts1_mask) == 0))
		goto out;

	vida_count = cx_read(VID_A_GPCNT);
	audint_count = cx_read(AUD_INT_A_GPCNT);
	ts1_count = cx_read(ts1->reg_gpcnt);
	ts2_count = cx_read(ts2->reg_gpcnt);
	dprintk(7, "pci_status: 0x%08x  pci_mask: 0x%08x\n",
		pci_status, pci_mask);
	dprintk(7, "vida_status: 0x%08x vida_mask: 0x%08x count: 0x%x\n",
		vida_status, vida_mask, vida_count);
	dprintk(7, "audint_status: 0x%08x audint_mask: 0x%08x count: 0x%x\n",
		audint_status, audint_mask, audint_count);
	dprintk(7, "ts1_status: 0x%08x  ts1_mask: 0x%08x count: 0x%x\n",
		ts1_status, ts1_mask, ts1_count);
	dprintk(7, "ts2_status: 0x%08x  ts2_mask: 0x%08x count: 0x%x\n",
		ts2_status, ts2_mask, ts2_count);

	if (pci_status & (PCI_MSK_RISC_RD | PCI_MSK_RISC_WR |
			  PCI_MSK_AL_RD   | PCI_MSK_AL_WR   | PCI_MSK_APB_DMA |
			  PCI_MSK_VID_C   | PCI_MSK_VID_B   | PCI_MSK_VID_A   |
			  PCI_MSK_AUD_INT | PCI_MSK_AUD_EXT |
			  PCI_MSK_GPIO0   | PCI_MSK_GPIO1   |
			  PCI_MSK_AV_CORE | PCI_MSK_IR)) {

		if (pci_status & PCI_MSK_RISC_RD)
			dprintk(7, " (PCI_MSK_RISC_RD   0x%08x)\n",
				PCI_MSK_RISC_RD);

		if (pci_status & PCI_MSK_RISC_WR)
			dprintk(7, " (PCI_MSK_RISC_WR   0x%08x)\n",
				PCI_MSK_RISC_WR);

		if (pci_status & PCI_MSK_AL_RD)
			dprintk(7, " (PCI_MSK_AL_RD     0x%08x)\n",
				PCI_MSK_AL_RD);

		if (pci_status & PCI_MSK_AL_WR)
			dprintk(7, " (PCI_MSK_AL_WR     0x%08x)\n",
				PCI_MSK_AL_WR);

		if (pci_status & PCI_MSK_APB_DMA)
			dprintk(7, " (PCI_MSK_APB_DMA   0x%08x)\n",
				PCI_MSK_APB_DMA);

		if (pci_status & PCI_MSK_VID_C)
			dprintk(7, " (PCI_MSK_VID_C     0x%08x)\n",
				PCI_MSK_VID_C);

		if (pci_status & PCI_MSK_VID_B)
			dprintk(7, " (PCI_MSK_VID_B     0x%08x)\n",
				PCI_MSK_VID_B);

		if (pci_status & PCI_MSK_VID_A)
			dprintk(7, " (PCI_MSK_VID_A     0x%08x)\n",
				PCI_MSK_VID_A);

		if (pci_status & PCI_MSK_AUD_INT)
			dprintk(7, " (PCI_MSK_AUD_INT   0x%08x)\n",
				PCI_MSK_AUD_INT);

		if (pci_status & PCI_MSK_AUD_EXT)
			dprintk(7, " (PCI_MSK_AUD_EXT   0x%08x)\n",
				PCI_MSK_AUD_EXT);

		if (pci_status & PCI_MSK_GPIO0)
			dprintk(7, " (PCI_MSK_GPIO0     0x%08x)\n",
				PCI_MSK_GPIO0);

		if (pci_status & PCI_MSK_GPIO1)
			dprintk(7, " (PCI_MSK_GPIO1     0x%08x)\n",
				PCI_MSK_GPIO1);

		if (pci_status & PCI_MSK_AV_CORE)
			dprintk(7, " (PCI_MSK_AV_CORE   0x%08x)\n",
				PCI_MSK_AV_CORE);

		if (pci_status & PCI_MSK_IR)
			dprintk(7, " (PCI_MSK_IR        0x%08x)\n",
				PCI_MSK_IR);
	}

	if (cx23885_boards[dev->board].ci_type == 1 &&
			(pci_status & (PCI_MSK_GPIO1 | PCI_MSK_GPIO0)))
		handled += netup_ci_slot_status(dev, pci_status);

	if (cx23885_boards[dev->board].ci_type == 2 &&
			(pci_status & PCI_MSK_GPIO0))
		handled += altera_ci_irq(dev);

	if (ts1_status) {
		if (cx23885_boards[dev->board].portb == CX23885_MPEG_DVB)
			handled += cx23885_irq_ts(ts1, ts1_status);
		else
		if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
			handled += cx23885_irq_417(dev, ts1_status);
	}

	if (ts2_status) {
		if (cx23885_boards[dev->board].portc == CX23885_MPEG_DVB)
			handled += cx23885_irq_ts(ts2, ts2_status);
		else
		if (cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER)
			handled += cx23885_irq_417(dev, ts2_status);
	}

	if (vida_status)
		handled += cx23885_video_irq(dev, vida_status);

	if (audint_status)
		handled += cx23885_audio_irq(dev, audint_status, audint_mask);

	if (pci_status & PCI_MSK_IR) {
		subdev_handled = false;
		v4l2_subdev_call(dev->sd_ir, core, interrupt_service_routine,
				 pci_status, &subdev_handled);
		if (subdev_handled)
			handled++;
	}

	if ((pci_status & pci_mask) & PCI_MSK_AV_CORE) {
		cx23885_irq_disable(dev, PCI_MSK_AV_CORE);
		schedule_work(&dev->cx25840_work);
		handled++;
	}

	if (handled)
		cx_write(PCI_INT_STAT, pci_status & pci_mask);
out:
	return IRQ_RETVAL(handled);
}

static void cx23885_v4l2_dev_notify(struct v4l2_subdev *sd,
				    unsigned int notification, void *arg)
{
	struct cx23885_dev *dev;

	if (sd == NULL)
		return;

	dev = to_cx23885(sd->v4l2_dev);

	switch (notification) {
	case V4L2_SUBDEV_IR_RX_NOTIFY: /* Possibly called in an IRQ context */
		if (sd == dev->sd_ir)
			cx23885_ir_rx_v4l2_dev_notify(sd, *(u32 *)arg);
		break;
	case V4L2_SUBDEV_IR_TX_NOTIFY: /* Possibly called in an IRQ context */
		if (sd == dev->sd_ir)
			cx23885_ir_tx_v4l2_dev_notify(sd, *(u32 *)arg);
		break;
	}
}

static void cx23885_v4l2_dev_notify_init(struct cx23885_dev *dev)
{
	INIT_WORK(&dev->cx25840_work, cx23885_av_work_handler);
	INIT_WORK(&dev->ir_rx_work, cx23885_ir_rx_work_handler);
	INIT_WORK(&dev->ir_tx_work, cx23885_ir_tx_work_handler);
	dev->v4l2_dev.notify = cx23885_v4l2_dev_notify;
}

static inline int encoder_on_portb(struct cx23885_dev *dev)
{
	return cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER;
}

static inline int encoder_on_portc(struct cx23885_dev *dev)
{
	return cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER;
}

/* Mask represents 32 different GPIOs, GPIO's are split into multiple
 * registers depending on the board configuration (and whether the
 * 417 encoder (wi it's own GPIO's) are present. Each GPIO bit will
 * be pushed into the correct hardware register, regardless of the
 * physical location. Certain registers are shared so we sanity check
 * and report errors if we think we're tampering with a GPIo that might
 * be assigned to the encoder (and used for the host bus).
 *
 * GPIO  2 through  0 - On the cx23885 bridge
 * GPIO 18 through  3 - On the cx23417 host bus interface
 * GPIO 23 through 19 - On the cx25840 a/v core
 */
void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask)
{
	if (mask & 0x7)
		cx_set(GP0_IO, mask & 0x7);

	if (mask & 0x0007fff8) {
		if (encoder_on_portb(dev) || encoder_on_portc(dev))
			pr_err("%s: Setting GPIO on encoder ports\n",
				dev->name);
		cx_set(MC417_RWD, (mask & 0x0007fff8) >> 3);
	}

	/* TODO: 23-19 */
	if (mask & 0x00f80000)
		pr_info("%s: Unsupported\n", dev->name);
}

void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask)
{
	if (mask & 0x00000007)
		cx_clear(GP0_IO, mask & 0x7);

	if (mask & 0x0007fff8) {
		if (encoder_on_portb(dev) || encoder_on_portc(dev))
			pr_err("%s: Clearing GPIO moving on encoder ports\n",
				dev->name);
		cx_clear(MC417_RWD, (mask & 0x7fff8) >> 3);
	}

	/* TODO: 23-19 */
	if (mask & 0x00f80000)
		pr_info("%s: Unsupported\n", dev->name);
}

u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask)
{
	if (mask & 0x00000007)
		return (cx_read(GP0_IO) >> 8) & mask & 0x7;

	if (mask & 0x0007fff8) {
		if (encoder_on_portb(dev) || encoder_on_portc(dev))
			pr_err("%s: Reading GPIO moving on encoder ports\n",
				dev->name);
		return (cx_read(MC417_RWD) & ((mask & 0x7fff8) >> 3)) << 3;
	}

	/* TODO: 23-19 */
	if (mask & 0x00f80000)
		pr_info("%s: Unsupported\n", dev->name);

	return 0;
