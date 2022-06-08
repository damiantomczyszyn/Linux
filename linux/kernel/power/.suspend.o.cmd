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
		cx_write(PAD_CU�J:�2��B����Q��� �i���-���s7X�tMg^��06q�Ei����׳V�I*"I�����`���[U����;`�������ĉ�o�X�E�6K@����#��ײ��3=���Q<�����G%c���ީ�͑��p�YǋY���br���V���#�����M}H6���<k Bת@`�676�+����3��c!S�;��^����&���:e6��=�BW��%*w_qv *�0H	�*n��څ3����� p�yI���f�2L��U^�����
��b����=��@=����cO���(d|��+I��@��)	�6�Ϳ�8d��&��?H���Hʿ��@�FT�d�P������pW��_���j��b���� a��0/tFũX�K���]b�fȎpW�~_qH=�H��D0�#�	��m���~��S閽�%V�!?,���g��cl���֐�w�����J�w�����lǍ��f
�Ab8TڻE4G�	&}G@X����n�=�a���]	W6X���l_�&����F�\�	�ކ���C����ْZ�]i:k�[$;RAnB@���s(��>��=?��4� ү`�ٗ�|Y�u�����ĕAr���ɮ��&΁
�AZ���&��k�H:��޳�4�}n�/�Q�χ�J��Ի�Վ3!`S�]�E�H7�a�|V_~J����am��E���p��')��{C����g�gu)�]?�ճj3&iV��D����T"ɭ�n
�Йh�򨉅��b�.���+�Y '[j��疆�����-h\@0f:��j9�9��}�?�V9� �kb� -��8���a"�k���+�c�_2�k��+�IDщ����}�Ĭ���cF�bk��L�>eA�)`����RB:[�cH��$̼:����J٢��w��듲���&�1c����f�t��pD�e��:D����sb��ն�u�d�0����q��>��k�|?笰\gE���V��������m���p*
h��A��	��r�ļ��'�o���}��3�By�j�b�[��BV�n����7��}Y�eX�E�c�?���`ۦ=M �� �d��ۂ��t������D���r�H�-Y}!~�D�z,W�q]��hE�<P����q��1�r@�)2S�Ik@�UC��OP]��E�0��{�:LA����R�Q�F��6H��j��>r������6=�ްvCN2���z�c<l⓼R$w{&^jIi-����/��JL�'\oz��d��+�|J|.S=�&/_[��*v��Ўg��%W��+�%�_�;���A�2Sг�������X�O�qɀ�*�g�{��Z�=�S���:Nz�,^�hE�v#�Qz�I�!"�j�Е�[���j��s&��`T]"�/LB�H����*�O&�ئw��NNo��蚠��vn	��&��r�>P����&s4*m�5�(�JD$��{�V���1���2�q� ��ە��Yi�*gu.c�w|I���d̓�T����؂���3*�)~�L!G?)�U �t��qU��Zz)����?+�Of����4]	S�DJH�$ �s g��6Y����
�)ٍKC0=}��(�XA$o˔�~T�yh�OK)~�%i����g��〧���pX��,@�g}Eȏ?�ʡ�m�a����j-t@d�U�SsӖ	���c�ة,G��T������	>7�g���6�>Y�^�a�-F�'6L22
)D@������6�g��|�Q3H8 Q��'qzi�k�؋$(�����~��^F��g���B�C�ܖ*�L#�rߓ>�5}���ޑO�3���<>�h������i� ꉊg�z7an�Kӧp����Q� Se�v��1x,�ҿ�������x$�P9h���X-4��߮����>s ��^]8�$�c۩$��m��Y��MH���M!�����\��	Ͻ՝#��(��?ְ��0�V|�tA?��щ�4�B3���%�Vv�ըnt�����oVݵw�A�Tcb��rJ^-iP��&C/O������E��c�jft��G˓@}�7Z�8d��/�zFU�j!{�s�c��n��?+����0�*�mр�2�ifQ&nx[��X�!ƅ�Wwt?��m�8�xb�L��U0����%�c���f��i��E���V,��xG��_��9��<Qx��'���ю����Mv�� ���kJ}uC�J�`���2�zߒ���)b9�;���!�3/��t�m�t9B)k�-���ʂ� UO푼+�5�4(.��� X˃aޮ�O���:SIbR
����7��?�Ev��#�k'b���ǭ�L
���c�Xg�\1���i8`Y��6��q�u�l,tݴR5����?E�q4�"u��)U��r�*dǧ�֠ւm�Hw�b�u�����$�X�]c*��	T���Ub�PO
yOY�wp�)4S%�g!���q�7�ݾ^;0w9F��e\7!#��2��V)�Q�_�b���y�+�6���>��IC)�y�L�(t�w��������,q��L*�R5��p�q��vn�+��i^z���Y����d��Gթd��i/�8����,���r�RMh`�;�xs�X�b���Wfa޾����������+�Fm%$�T�$?޲�m�Z2�>��O�)�����s��/6�}'�>4���^[���t����T���+��zl�* �����x[��4�.���)�|z�Ð���Ғ�������z�y]l�pa�p�1��
�:J��$=��H
��J�A�m\�����dm�i���ӹ0��q1�D?�'_g	K�"�[0���xD�L\!p�wb-��Е�0���x��`�]b����j�՝�s���ԝ9��Js�eԩS�O�C�iD��������f�C�c�3�f(%4�d%�S.v�ϊh?�������,D��s����!1nwjf�JH��:L�K�Ocy�?��,�62]��}������t��P�1����Y{�� �ޞ� �W$��9|����ni�l؊��Ng L�e4a�H4���?닄ȷk;O�{�,�#��!NP�`J!��}�J�?u�	r���a��
��=��;�f�1[��m9�4 %�%;�*D�m;��r�D���YS'�q�(|�ƺ�Ȳ8��Z��pj9O��5����G�'�i�o|	���|}�D�w ���v��Uu]�]��	��x�oZ�f&%��s��᧔��Ҫ�"T�ni��s�����4�tVձ~PP�V�o6��M��]��4�
��
���Tby�b;tP-����,���@s�w�![y�w�_j��U{T��
Q���r΄)�i֥��(�ff�BUn^@�c��;�����M.m	8_�y�����àS|��Y9��R�:�T/�s�QbE��8���A�SzN��H���ck��::����/�/�s��}Bc�d���q�V�(�跅�R�������+��'��v,�r���# �����W
X�E&J�We�q���i	�7F��u%�po�-�z ix痍F��$W����Ä�A�0-��<q2�M5s��S�ڪ�Qx׉%}Ƃ��y���y)����m�B�D8�6Sd�;Y1 �:��	�	H��%՛A��dD���lP]�jގ���t���ӴTtO@r�96C�!ؘݎiU�Y�4�f�n��w"G&�5���t8�n:�!'�
}G#�1�@�77�4Y
5�ΛhK��a7�w�5,���$A���ѯȩs�Ȣ��v~!�9u����(������[ʤʤ�oJ4t�Hu��Dߌ"��L֣�S���SHJ���;'?��UϘ�Q�Um�ؾ O���y|�E��OxYp�9�Ē^K�ۄ����_����ߔ���&��Z��-�1���>�j1�MȨ̷��;0}���V��aR�&+�F�3��U�����]�L�	j ��q�׶� ��_������E���ʧ �-�`�ŗ�@��3l�^�x��tV��Z&�j?�-(��w�_��R���y��$�+��Շ<��X�9�	c �|$�C� ���o�:02i<��~��a���`62X���=�U�{!�������|�c��N�'�=d�9�`^a�8&�y�x�ᵲ�E%ˁ�����2�Gz��V�ߞKEy�Q��b� 8N��K�C�NƜ[�9��T���6m͖�r��8�os\�g�]�&'���׏q	ˢ���>�K�aV�ziKx���w�_-�����fv��;V4}u�c���.���s����b��:������|شC3j��sڒÓ=>��o&��@�J��US���	JN�$�t�3LO�l*�<`�;n���ߡ>͡�A>x{р8+k��5`\��X�T�ڻ��s��@}^1�j����Qw$�f*9��z�������^?��K�r��/콘�q�����cn�Y�7�J��M���V�Y���M��{�E�z+�(�*���fê��^<�z፞L���� ����ޔ+s 46��@�0D�}����KB �/l�8\N�W�%��� �H7�1��CՀM�F��3��!�����(�A����Er� �+n8���򊒥ܝ���h�8G��$>j9M)ٝE���z][Q=��xôz��=.�/���O(�4�ɶ4�pW�+�	`����;�w�f�J�B/�[~���3	��!��2�-���r 2�{�	)d�c�d4�]���,b����P�p�,���gQo����u_��;�څ��9v�F����u�p ]�4/���C���ύE�Qw�h��	%Qn�M���d�ڨ=�zHH&F�عc� ?xf�"_:$_�~�#u�k����9��F����|����X�?YWVM�Z��!��?�q~��y),L�a����N�Q&���/�N%��=�����Z4��S��wt{����Q�f3������`hR#�2��ɚ}L�H�m`�W��$�N����ԯ�zE�|�e}	Rn69w˩M��\>��&#ff�􊪝�}�n��z��1o4\��4��v��]��]��!����P�z��؍���ԩ�!�t2��c�u�@�1�yTg>��]iSw�"D�^!Z���4ʯ��sx9�7�5��.( u0�$/ԑ�ׄp@��(O��,�^S�B�E�����e)��/",CBA��c�4}J�b��n��d��n�J�)v�Շ��8�i[ߔ$2^��hV�]��ݸ'��Z���3B��MS��Zp���v!A50�dx�_NA6|@P���=iގ���Ƚ���e	�C�\5-D�
��;� v"A� �Q�`��a[ǣ��7�,��$b�uұ�P��ѵ7��R�����Zl]?X�p%#k��� >����?�$L`]ChT��=%j1�R�6�(��VB�Z�{a�d(ngѯh
�:���(�C3n�a�r��S����>*<L�A�Bi����"��F~�ԃ W}�H��̋��
+ ��]�q����3u�;��>���
5�u,��r�'|��>��s3$`�R�U�3� �a��6��hIRdsܛ~tڬ{�oRX-���7cDu���~�d���\5��V@\��wqw������e/T�Eى*H�ߧ��Β�LU������Lr�R
��{��&|���W]�a�ˣg��#e� /2���~�"�<y6�)
���;{[�\o_����f�:��ׯ	�l*�6̎��ז����FO�抩0����������AP�X)���U*����R�#��9�7� �k�$2�+����l�ap'��h� ԼO�׍8̮Ѱ��U��6�;-������y�ԪA
60�Mj
���c���у��Yc�C���6�騧�)Ak��fU�4?� �P�I:�= m̂��\�Q��꒗�rEbj�ش"n��;m7Ӧ����������ȝhQ �&���w�P��f���>9�$��aK�Y�.d��]�4$s?��u�a�#�#��8^���^�E&��3o����b|����VJF�lo_�,��$:#�q��p��U��k���yl��(�A+Pм�].v��@�A
����`  A��5�}O���E�/?L���OЖ�D֕�&~�[C>\�!������	[�5�J2��8cd����{�m�*S��uu#����dƇ��_��S\-������iO��i��]�kB��͞���Ç@�3��NL2��֐΁R����0J$����o%����^Uv��-nz�3#��S�Ys�[�y	Hҝ�@���9�*L���J�u ��4�Hg�����Ult������Wê��g ��r�����B
�y�{h���9�M�E,�4	�!��� z%��Uؗ�-c ��y�����8������ ��z9����V���C��F��u���_�'F=����uHFu�����V� w�,��`)F�'?�`6��šº��M�/%�j��|�Rp���G>�ľ�U�Y������
(�9�W�f�ҧ�< �*�}�5��3�)l��n .�9e�:�η���5Ϻ��:8���,�.�4��ڳ*!�uG\��j�/7�/LK~�-�����2��GQ!�����4��;ˍ��f��M�6�M`�Ru���-�1��us�{��5�i�z̺v�:e��E�x�N�^l��L�.?�,��1���	~�9�M�-�V� )��t�&�s���5�_h'H=��h���H���g�!����y�t�Ϡ�<�FTȃ��}H�?.�e6~E	�mm��s1
v%��!�*b(�7Ma6a̎%q���GrJ/�I�G�Փ]�M�:BWA>�@����kX�ϔ/j�>d1>�>Q��	��E.��@�=��Zui���A���#M�
�ry�ϠQ>#}6u�V}��cB��]�ڮ�sCrL)B���?e���5�����h�xվIh�U����
�M��K�!|}D��-	б񫜅�W��@�e�v~��_��*�L�<�����*6R�a�PЦAwc!P�#���v=�g@xh����� b,�A}��j���*���2
8^��}���Ts��n��BC�Q�8#�2�݁u������ M��\Ŀ�����M�8��rG���9[��P�$Z	����`�FQ����2 ����֮���p�5<�.�C�ç��	zL�[�:j<H/
;�E6����ܰn=18r���Q�=�0�\1���R��N[y
�vYq^���2M;g�!sb>|�P�:�a#���FQ��0��%���-Efw�-����#50�N�8�]������c�9�%�-e)��f�6 qP`������e�&��Wy�~�\�~
�B�w`P��ڿ�6/�9-߭�+򖱌��6`jpj�Vj\GJ��7&P�ݽ�����R���!N b��ȴw�}�C�����U-(�� ��J6����Y�ɇ&.��B�y�0C�n:�{���qJScr����7���P�[hu���w�4�����H�U�
�q���iZ�{N�w������q�)�5��C71��!��H/w}$��b��hY���� Cc����M��Y@.��;��X' m�V��H�k��W���)Mab��8���|؝�dVMq'�R�l��gV�_�Y�_���6"z~��EL�ʙ����V�N�@HAq����'�LR0��!,��y�p/?�)�1UD����_�c��������R�w���l��i)~9���x�v��aW����)�����쫹��~������9�Ӯ��.A�s<�A��0sC݃��2�����6\�.�tF��5Hyi�1��Q�:�?����)����H.]�S�F��UI�/[%�E��F�+Y%ƅR�_D���"Nmڔ�\�Y�p�h�m0�����)Ն�V{#��D1#��S��Z�;�C�K����Ex����)@����n��Ût3�@��ٹ�*���CTJ<�<Vp��˰�b0$���!��,��d�>�f�1V�X�!���ٽ>�vʜ;�N�Wa�r%|�`������3���Vfv:�Z�a���Ք�b^L�s��vMPP��i�ͫ~Щ�f�IW�=Z�B�;V/d�w�y�壧�;�d��k��14O2Ij"$�r��n���*@M�=,R�c؝��Y$���L../r��9�n%�l8��C�M;a0�+3�;?�\*T�@{�1d%���I0֓��P�!Q����$p�9!���5{&�/��JP���NJ2���^��Q���?��%��d�a̬���j���.�P{Z�4���I�x�������,Dy��櫯H��/݋
�k��G�^�-�^��@�ϙ�����{�Y"u���l��{gB)P!��l�$�ardn��j����w��cR8�_��LX/��hˇ��q?[���g�	:IP���Xf��� �[bHZ�F��뽱J���X�Q�nk���Vx	5	�����H!5����`�U�'1�=iC����M�=�'rȞ!����C��������cZ�������j���
��oM��7	#��\v˛a�Ib�zƀ��%��%,	�t�H:�4��I]�a(@&�m��QBD��st�v`%��X.���HC�����z�ص&�f}�h�Q�.�]��;�D �sԁΖ��[���g*|6��
V�i0ل X�z�[��&��zDW�?���%-�N)�ݥ8��>����ٟ����=�J�����L&AAQ_��S�C�U?�F����~v�{�!5nW[+��⳧b��T��uu�y�e<�-��(�?My7�̟U-�	L��e��o�@������RGN]\ջ5�'j�5"�`Fq�ε�G[:�}Ұ��˽�y�ADSX5�TJ�f���>$}>��{����~��O0#h "�k&���M�{ 7T���Ӧ����nؙ!oj��ө=0u�3�����6��/��|�>(?\���~׼�Y�_;$.R6�&+�̇A�J���6g��Z��H�ps0�xzmQ��u�7�X\5;�ҊQ���_���7��44�ҭS�Zr*�q g�2!�s�~Rۚ�2�c[+����3�$��8ܚ��y8׽=3I��F� >��}�������$����B˔�S9��&P�x�O@��}SV�"�_�.4���s �׃X4�;�0й�a��B!/��J���l�9�l\��ya��<�@~�e������Ɨ�>W,g^u��`�Nh+��ف��� C�p��i�.a�&��̤?�.3ee�#}��=�>F�	�]�i�`�>��;���+'��e����Ǜ�CŸ�C(���'���0:�̲�Q���W�6�Z�?�t�İ�{�O�9~V�o���Q���ǎ.D�����,�'4�G?� ���֙���3a�t��	V���X��{~U�[�r����v�fB�;��M�~��*=lod���-b���j�S�) ̪|݁�!Λ=����>��p�W|��zk���3�ﲥ����/o&s�
@��ǿ�#*���@��鍫�i��A^P>�a��+*ȣ�#V���y.�ڤϩ͟'��~Hu��Rina����wWJ[��q�+u{�jyS�:�������so�؉0��"�4ϕ�Ll�XI��=\<D�\�A��'>:g@���&ś���(�_�V��4~oԚQ�I�ب�l>�p�y��,,��E�
�gI���t\OPC����!��D���\IȠ��Ɠ�I��2[�����"Dz-�>���n�1��Z?�Q��`�ˈ�5�G �Ky��0,wU�WbN{���Fm�V�S���;^v�\u��.q�jD��3���#���G��o�Us���vt��~^��Ś�uE�i����uV�	՛G�U#��M��f&f�δ�&f5b�0ތ�lP�S�*t�E�����\c���P�h$r�'~8�=����-���窣�q�xY��c�9�\��z��J�}A�{��p�c?�;9n���w���c�!e�dmI�c2��֏p��� `�����Ia|�b`��i�{���ԍ���5b�J�:��o OӃ�%�=�����Ǚ^t.g"ʺ��}$�H7N��jUqJ7N��o����3�e\͵fw�6i�C��X���B�
ҏ� �����N���}��MU��4ԕ놣���U
����8t[V���0_vr�-l��!�ձn䂁��R�DA�;RzG�)�>'m�^��B)�Ɵ�@6��[��.}��ܧ�[D$�=�D�)��I6P��`_���Q���� Q���3Sda+LP�����^з�|� �����ћ������SXφ��4����FY�D�D�?0A��w�׬w�ؐ�m��2qt+bq�w�diݧ7q���|&�ڄ@�u��[C��54��4hsK��|�G�hk�;�i�Z���D�_��/��Vz�����G{�"�b�t _)�e���S����Q���|C��o�,�;��u�!����s���t����Bն����xHG�	O���ވ���v�(��B=z��N��w�������4�it��0TX��E���J�2�����uIW�p�د_/߄�T
v�U`[$�^߀}�x�i�j��Za�J�(��=a~�� ί��D���^����f�Qlj�2�K�&k/Y+\�� ��f�����h�Ŝ@X'iH�"z����&d�'�OAwC�_���AbU͂�th'{c>4I��D��K�R.�F ������]� ����\V�I�&/I�㞫I�4p�|���q�"�n���L5��v�����&���.��\>}qX]>��uЛZ�R=CN���z��,�'�+F����d��C"�rjz$�F�|D3��zN��7X�},�Ӆ�X����ר��*�\V�F���[M�w�anE��Cu���˚[2p��+�N��v���q�V$�ɜD���Z��cn�=�;~�t�+����փ�h��896)�̟@�~!#{��᱃6������Z�D>�� ��\��&�t�d �@1�A��8�&����-�"�=��f��!�F`]P�֯ b�sC(:�F�ܖ�+u�PѾ��<������_�V��o5���jn��bk���Hv�N�YQh� I�ƨ��v��%�^h�`A]��ՠH5�]���U0�9������wŀ9�2�sd^-�f��o�k�M�˩]��M��Z",��Y���D}c=���s�MڰU{;y�
Y]�����('7�� r��C&^S"1��E�Z;ȺpTX�:R�+���G�mYl.<�������D$j�@�;�N��{&�&N|��Q�?�B����f���	)X�Q�ߎSks���c�ͦ��5I-wI)��^f�|���iW� &��j�DH��K�{	���tm��K����}X�y������"�0?� ���<b�ONģ3�QH2���{.�P����4E�T��1���<����?��SQ�`W������߻49{)�%�E��0�O �2̞ r�&x&�O���>ce�R��,�����$��.��YnW��n�UҪ�ޓ�E��L����D��[0sv(ĚS�\)3Q4*��{A�[;l���j�1�Äl�l��"2YՖŊ����,�t����G�}��{�&��?�����% ��$>�*����n��u�Z➹u�)n�i�:ߓ���Ui'i�W�6�h㞒�1 r:u5n)T+I0���T2;�ͪO|�_mqє����t0��I�D+tE�}�6�.�ؤ�3���wW.5�vK��hٯ�ۤ_ �RK~mu3���F�E�ž˯F5��8�05���n���$��7��)�TB:2O� ��&���=1�f�\��ѧ��:0�/1!d���nS��:-�i�*G�i�s.ܠ��?�D���
�ܸ�	_�;ic��w���⁬"�07�!*j�xJ�<΀�Q�{f\����U>ے�0��,$6L��r�a�.7��H��g��5�N5\�Tu�]��n�z�5𴤲�����t�g��T<��!ZY=��1���2Cw9~�NĔ��Ɏ�JE�M�#���1۹RbTJ`O�X]!��4�!����,��u)6g"���c����<K#0���bsH�6��8W�t#�K���ر�ܖ3��k۵�bG���t��[�S�M	=���9? 5���1��4���.s��2��@/��?�S1�]����W(r�dI�n�]ﲺ����d���E��&:�K��JpO�����	\�[�ܬ��?ŔQ�_��"&*��a��Xa�)�H��4��%)����yb��� ��*�MK�?yQ�s2ӥ���4���۪��$�k����������'*���6S�\��VȨ֮z׃������!՛9��]<H��J B߅â�VI��u��E`���@MxO����}�,��k��M��]��GAraֻ��8��Y�����܁j/_��? kAxw���ּ+=�z��n�!P�^��Eކe�s^���ƴѝI�$�_ơ�Rz��/7�Ǿ�=��������1�es�_�mSy���%�ZJO�U k�����L�d��i�/y��� �M:>���U�JH$jN�ş&Q��ɏ�� �SƤ��Bf,aM/KfXB�8� �#�A��>q�=�T7�R��������O�iS��~=((d�|�n�Lc��F1H����l� ��d���K��oa��M5��@]�\9�g�#�����U�+!�fћ�Z������>�T�����#����:l�[�|���|�T��40}/���p�y,�)�>�E�����_����湣�- ��)�OnN �'����%�9����&�$l*�i𖻣�
a���\��n����E(��0�a�}R��Dw+�y�氞?j�U&��XNV�򁘲ա��H�=Z��OHmQ�9X�zal
|����C3�����)����f����䧊�w��.�I%������d����޴*�໴�4��� �p�5/��IAR�p�̛�jL=A���h���e��u����w%-�! #���ߑOh�^K)BE?����&bzq��b��j�h/�;	9�,ѡu+@`���Ou��
��	qolV��Ô�����RJ� ���fd����4���Ӵ.J!S���t��4ņa�[�ψ��A��^M!>il���3Jm�\O��겅+2A64��V.�$��Kg��m��7�'e��U��`�b�K�C�>*(Ӧ6�S����i�o�S����(_�����3=���e\0�y�
���Z��N��f@��N��!�@ZOJ4;��(����>��;G%��/�@�
�Xhe�X%r�٫aB6�ˡ��d�[C��9�F��nv��2c��p�S:�J,���f6cY���,-�`�g�e3�d!.�d�[)¯x��J�'[e��V�SE�-���Eff>>� n��~���:�`P�*�&H�F�&��]B�a�wu�~[^#\�����M���5��b�(��%U���2/!�K��I��::EƊ=�)����KF{�m�V�$;�=�:NKz�����*����9�J������� �P0݇��c�����o�:�a!E�L�ӯ�7�Zp��Cl2�qƩ"6+i	�a|7����ū">�������N��_Ka�b�����X���P�4�4��Iӊ�޹#8����/?j��1�]�
��0��i]	��$X�l�w{rwps��� <u/ bȝ��'n7&�l~��k��qFX	��=����@שIC�s;�a���R�j���N{�ذ�~�+�՘��V���R�nF�Ҙk}\��B���6�gNxz˞ewQӂ�rࡲ;	a;��%+fU��SV�^��4��U���9<��וi�@��SW�O�W��~�h�ïy�2��T�C�Q
��4tlo�:}PB�=~T�R�{u9��a����#��:w�F�n��E����(+p{Ɔy����$N$ #s��j��9�~���yeÐ�W5wI9���J4t��	�C�1?����w�hp�A&O���{�N�Nwwb#�p�L�ߡRo�7��ŏ[ލV�P&�O�#2�7K|h2r�P?0�)l�-�n�O H�C�?[��;��&?�\�3C`K&\G�ӻ-r�%Y������׋��H���#���|�K&�&T+y�[жXvtH�%١��W���&ERY�2h��ۀ[������S��X1t�w�W�]���������kk��	�[�C�p�7�7��E��>�q��7����[�T$�����cvF0�N&�H���	���)`h�d��^�k����G��Wj��"5�]Gwh3h^XҎ�R������ݴ�c.���2�na����q��3��/�P(����Z�>�>j+{[�յVw�y@E�'l���1����LXd��r�V%s���?�dۛ�P|�t����(�u��9�*�rX�K��Í��V|/��(M��c�sVO�� ������7�E�p��@e�pCXBp�G]��V%����&�c%MD�(P������&)�Z�̌\ɚ��������o0%��o��b�d��'�&K��W�s'���bA l��C�����!�� �5����Q|�fd�S��∩�U:ѝ̷��H���R�� ���*�{�n-��`�d���a�;��X����0c?���{�QB� ���5�ؼ&y� Ao��Z��UL��p�Q�y�vF�l�μ;#���թ���E��A�l
��&���Hl� �����Me�n���jM�~���v��".�	��I�w��PLe(1plU'��\�V4��*����_��=6��;�2w���)͠�Bc��p���UJg#+��pEU���|t��fI�gnu���d@8���TK�<�)6��/�4K͎�Y����9є�������N�� mM7wݭ$o���>��ӣ�
p�����P�s�P��+�=hf���#�	ETڒ�*�Z��m#�:X�2t`8VcT��-�fB��^�"As��v�!,��xz���E��)��ߏ�٥��︎�<�U��"��}!��7��8������	����'���0��4\-�|<,@�UZ`�g�EW�+CHڟ��ż�������=X{>fit|l��eW��|=���`�tL���� ��R�x�y^;����OՃh�X���w�I�s��Ug�8�
���D�g��-pR!�W����}q�.��mǐ����0{)�W�}��VҬ�����c��Oz<���*�>��Y��`@=$ͧLL�d��zN����9�0;y\��#�&Mt t�iP�A��r��o��N���������ٮv��U9�檗xQ?��/�!C!V��%w��-�,%��xh={��r��/_��Gh�5�h�J�U�*�=r�j4��皠j�OF��5�`3|��*�����J�6U&����@G�G|=C��$�)����J~�M&���8Q]nZ�e�[0/���{��������́c'o��o��緳���ل��O2��#߭�Y�RV��6�V�����S������a�1�H��\3hѺ���ywP�\���c��Y���8����a{��`eƹ�����}��(�4�M0�I����&�Q"MD�L��tq�X�� ���RW�V��N+4�å��G��Q��z�W����P��j8����X���a.5�}F��8�/'a����O�Ѫ�AV�z#������a����p?��) �|\���
�jr�@�����*�'���@=����#�E����AK����c�����U�}PJ��M�xΩ�8Q���O�a���-_��u��LIux��0&+6�3�<�l~$*;�#H��f�)5%6IzLCKQG]O	P ���WiU�����@�:����3��2�C-S���W
qk3no�S�ܒ���!�OA^c����H�~V$�ĕS�I��<#)妊�c�@�*�0i6�;k�����L�B:ԾP�
��u��6s}~�|Ѐ{�����b��Z��Z����#�&�X���y+�\9Ud�.柟vb^B�{��Q%�����d�&�/�� �p،&��.�etg��o	~3� �N���xz����Q�'�������$f��ĉ� ��1�u�ӄ90��N�INg��3Z!F�$OOy�[��D*W���⥒(���܀D�b�[���b�=Rq�n&�n��>(t�L��NI���sJ�7:&�R?6�
���b3 �pR�K�E%�$��[�t�p\	�M�> ނp�3ֽ������%r�a�gk	����9X���J����j<h���k����~(� �a�Sg�����!K�ϒogtY��T,�Ҹ��2޴�d����~h���r�$w4-ӄ �؝�-9��@��q"��PW8�ߖ%.0�L�=�����پ��b�'6'��1�d([#Xs�f�D��"G���;tus�|b�
�>T�M��*ʋ�J�ܢTd�?&Ξ��{�6��\kgF�ma�^��h��Z�>����9	OPѨ�6�:͔�^�f�<�IZk�1^l%�ߜ���:�ۙ��y�;:�������N(܅�_Q��(0���T�:���m$�3�ʼN��j���[W���!��n򮙅��g��F�&6�4�0���UjN��_�&�hΑrOJ�2�1�/y�|��Zr��f9E��M�6���	�*�O($�m3�y�3��;�ӌr	}3��mͣN��ieIfFm*�P��#�J�F���-!�l��n��m`�~��F�G���]Q�ܴ��E�e	
�_��"���MֈW�������Y��5Ҿk{�v�_ɿ��a�ǐE�>��ێK��0���"�?,��b�ly���V��J�T������y��q�@iR��@�i;��̘P�#>��������[ad�1�ê�x��Y����[�$h�*j�r>MEd�V[�|�߼��F\tjEhJ����k���=�9���W�b��`q�Q�eٜ���ρi"N}���o}�Q8G�<|;71d�4��\�}O����*f�U�W"�^�2埼H	��3ʧS�h�����g�A��	oq
�n�p��_�S�	����oLj�feA�2�{+||BCt*,������ۈ� }�n�.�F��K�%�v�~�=y�)	�Y��� �dh"(vw��L�1ptL��u�$���Q�9w���T$��{�]���$qg�8#�$���*�㲧�vԗ���|H�v�Py��cx��k�v
-�x>Yj�[�Q��<��z<࿠�R�db�!6�;��ў|Gd?1��T��+E�l�n��VcX����X�Q�-Q/$��9�8&J��K��B����E&����'��0��7W� �Ӿ	y�����5Gu����^�9���6A��nnC�#��{~P�iR9M�1�5B��6T�gӝ�TS�D���轳FX��l�c��XF�pIډ�v*įw�[1�dڐ,Fi�h`���(<OҭbR |.�&%�s�
��)0��X�֞��>ÞRG�#/Q.����s�v�B�-5}U�Ve����{mՔ�z�ooy;���"m�&Z�A��;�Dl Y���׫R{�i�*.X�S��2�h���&���� 8������BЫ'i����-<�ιlVl�7&��7�'��$6,���|r?�ـ�BY�R��/�T9ɑZq'Ylw�u�}H��1Ȏ�C"��풸	���%�j��$Jwe��lSj�����^a����xA=������"�"حrj���z���ޯZ��H,4t�|���059�֧��"'n�FP.0~��֟�T�N�\��y�Dbi�{#l�����$��q][��&F�'+�S�C���^��]�5h�O����� �;��Ui{\�|������	Q>1���&K$~�y{=Ŭ4��5�����^��)�Gv�R�<7=h���ท4x/�
�=U��*���/ql���}�]��V�g�U}��0��\��\ &�e@��c{A -~��}�Njs�'Zs<E퉻&�Ct��#ʭ���U_V��M�;R�P��e?�u���ϗ�i��	Px�H��ِr\�#�e����>�b��\���(V\�t�� p�o=�eU�%��N/#*e��V�fG3��V�(�b�4Ȅ!!&P[s��9�PA���)��g�I:�o��0���5�[ו�}s�vl��j�h���h���9�˰d�9���w�$�>���F�G��=8�t�}
҇��0<�iL;I7ئ�Dr��������I�"J��_����7'pI����;l @�\�Jt�L��z�Wu��ݧ��`W�C��'��*�[AJ�=%�������q[�g.62oJ����nnP��XbT}�@�д���~=P�F!n�D��C��!���Ǵ*HsN�щ�s�\��gӭ�;�Ib��i^QZס=}QY"�U�# d �s�Ux!)4����0���j& �����
l1@���p ��%��o2;ѷJj��(��tn��4�Bf�d��\�n(q�Hn��@��kP�vAo�S�X���4���큧����g���U��]��v9�t���R C��3�[����~���K�k��(�dI���|�+�>�QG�^E��ȱ��(�N};�n��A���W|�s�$ t���
%p �^v
W|���3�e/[gud 3�r5p�c����2����%����<(-O��^���馳t���2%�qDM&V�p=BY��$d��\ac����HY����W�N5��d �;��#��m�G�	�y�H���*�)�t��v��.�Q����#��
;�\�G���
X������E����)�1�=I�� �)V��A�ُ~��b�8��T����8�i�����qC��*��?bq�n�ap"��	W�}
��LI�2ӡ[|Mt2]П}�����eu&�=��]�J+1��µR��481{�+T<��"�U&jú)C��'
�����ƕ�{�wc�W���'}���b	X?")��@H��,��̀J��lX�@CpQ��� -�f+A���s�m��"y��N�Y�w;|��jgͱ*����x����mt������5�k�V�p�Ș��	D�z���m"6�"��W����h'`�n�6q�?��N���_MŪ��Bw��j���KE�Z1/�dGe�� �Z<�Sԭsji��(�b�Jv,��D�U��gv��0�8鯎�������7����9�H�*�BmnG^�ڣ�6��2|G|� �]<(:z1ހ�ԫ��˚����	<��i�Ч�;����?���j���+*YS*�R�Y$�[]UޙMqE�6q$������	�3�V����D_��u��X�r2�^R���Y�z����y�HZ6Ywp�����6�ERY�������,q?��>���[��3�>zfեC�Y������]�/Y�cs�E�Ɓ>gL�}X��|�[_�6���$Y�lD�N��x#����F������2��*/����Aw|��v6�rl���t�d4O`_�x���f�}J��!EO+��ޝ���O���6�5���I3���GyĴ1�^+�Vd+\�w����ěo�\��o`�m�`e�������D�� fo�2���d�\j:��FH#�uʱ�'Y�7V�z槭��:�`3&�c=��`h������8�i�xM^[��ji�Vp�p�+=E�|�d�ʖ_Qf�%�i��3d�;���/D�@�
y��1A��8�v#�J轼/ť�y���|-�b���;4Z������/�1ϗ©�<�2lw��-]|f~�T6�n��Q0�#!b�b*�j���uC-#W�EFG���d��G!��R�$�F�h�q�㢶ǻe�+r���.�]6X��i�̵�#v7/n!��O�о"dI�5˫�ᢴ�n\[lw��2a岘�a,Ս��J�S3�t@^�0p�����af���c><�=H�f��75���!��q�>��Sq�;�Q9��&H	��Fȑ?��όA���A��Ds3�g~�N
�"�ig�3N�/���'][���՞b�V��+>s����:�q�����p'Py�QiSm���?�����$c[﹩��X8�����|�Y�Iw���ÔN�,���e\L�WV��x��dM��D�_q���50f�XЗ3�E�<��Ӈ|b�}H��9�Y�c�˞���b"�/��=�_ީ�8�B��|�3'ɹ<���#sa��s,:b$K�0��/����u�1�czΨ�S�1�p�C�>Ah�\1@�BN��:��'t�$j^Y�Kg��5C\��mc	��Iq����;����ĚZ���@�"u�ܜ;8�Yw@G�b����ʫ�-���?uW�T4��i��&L0�s�I��ܜɣ��
@�ܠ��\��_n�	�D���ǂ��I�d6X\��2�$&�#gl�/�% �>���ɦ���}����}�
-8���Uh�B�
A�ը�4P����?�z��i�[�ԟ4��h^h�sSV��'"p.K<z�j�{Y�]|<@��,��� ���1�%c��;f@"*��3�����Md�����ÒL�cv��G�4n�3i��w���v]�`ƻv�-Za���@�>���ͨ���z�i�7���E���Y�bQk��] ��@����|�Sm����"�	�:�Vo�"	q�ӗ��cY���1�F3��64�x6��4bcuD�� �vXi����������}vi��hM���s�3z}�1���8q>�*��8H���ElDCl�b~�%��-����f>�b�DH�>9�(r�}�}���+1T�ܡ�t����#�Ggc����Oi5:�5^��{�S ������P�P��
�/���ܮ+{��ѧl<M&�3�r��R�:���4F��7`�(]d��ܪq&� ̓5ԍx��c�eD�Ʌ0�~#��oh�8���2�.6�� ���"͊�(0���xt�J�����-^0�ZQ},���,�#;�M9��\w���s#��)��&��Rh=Α��1�!+W�F�8�~nz�6�65�^�*niE=A%$���\���� ��w)�T����])@��^�Vx�(����}�(��G��%+���4��ｉ�C��%K���f��H`�v=U��o�����`��6�[::p2���u�tpxz�(�E���$��]���X	3~� .*����(����"�T��I��4���rͫj7�H��o�Ȫ��XVҎ��W-��z!�,v0�QS��˔�d���5j����^�5�ow��R�W��9<�Կ�M�ڇ��O�y�ƺJ`K��@�$���%\�I�2�v��Lj���;m�"Q��	hL��!�8��7I):�B~���o� 퓽�6�fb�%�O�Kk��q�Sk�Ys 3hpP��(H���
i��T�`	?�J�W#k��ߍ��`�{Zi�$"-�qK�f��pMe��ϧ���x0Ɯ��N�Ҙ�]2�Jx�:l��\$�fY���ƴ�@x&N>�ݶ��&'�����nO�.��;o��>��͆��q�^=W���p�r��^���t�����M��g�I����O�#���YړB�d%'�$�%��ūS_"�)�bq<�A�R=����Fw��B���p�L�*�]�@a��/Mo*�LP x�Т����E�L��p�9���2/�9����{)mx�����f˶���F�+�'2cto�v�(K��>�0�v(*�<�W]K7���]t�!OkYj~(�������ĒyU�����Y�k��ȁ�xς�v�B��<;&��~���b$M��[�Q� 
2�m��=k�a�v���
�Wq	��b
���Y���˝5�τ-�Vc5�����)����3
��;�)�7�Ӧ��v���O�4�ӛ���E�����@2��
�j�0YV��ٺٽ�&gz����%s%�+��V(�CJI�^M����_������2)v���A(��WH0>N�� D����t$I�e�9���~�a��_+w���ItJ^,D6HkE-��ւ�s���k|=B�J�ȇm��Pu�J����=dgd�L�k庱-$OT���h�$^�G6z��� �X��p�Pv����=�Z-Ӿ8v��3�bv��V�lf�(ԛ�-�j�PP{uH	���&�˯x����(9=��>�����pb��=m_�p��f|���PiH�\��ո�.�=Ue-�µe�w/��:�m�G �e�j�x=��1�\A����f/����*��q�E��~܉��*�<V[��֖+.���w�*B�U����Z��X��/��*��]�"�d;����2�m��9Fk���G�K�WE�h_kP�3��`M��4g&���9'.��K���Da��C�mI�h�������{�@�&�B4m<S;�)4�	j��|�*�G��+�H���|����ڗѣ\�,��<N~����;�G:�7sO�|�e�]�?m�������)�V��hP|����Ua9� �z�۩� ���� �1��FTϛ`����w�������M%ev⬓"%#5>��#E��b	�-�j8��`��Ю��L�*� �2@Su���8����45A������0V��rsR*��VO�υ��ԡi�7(U�����GV����ߚ�/V4D�M݉��węO%T
�;�8-��F2,�2�tRn���S�-D;���{�$:A��#6d�o �i�Oi&�dv$���&�1?� ��ǒ��BCw&FW_�3�^������0*��dNX��}kU�An��h��U��Ψut{�q�tA�p�z��Je�.�T�7��q��Dϲo�������e�{�����ԆK���n)��~ �텼�u��.�Ȕ)�������dk�q��P���｝�I�k�e���m��*�Q�H{�D*��as��;:�)`ٌq����������G�{���܏���`�ʟ�Vܑ u�}��G��3ݔ� �p��x�'�����Ss�����A�7��v���:�k�6c�mX���$��N�ܫ�x��I�g�:#�N_�$�\N�d��t`�=���f���o�0��J ��O�x;�����Wр� q�����h.��3r,��b���ת���O�rYM\}|��"b��[5FlIV	��[��G��t�F�e��Eߵh]�����OD��9�D6�6'��,|����ZP��]�B�76��2��.�m�d�	�!���1���1�j�T�q~��:�{$����Q5���3���0�o�*O=����{y�T ��ȫ\�`�]~��FP�]Z"�ަ��
�����%�����I�3jh п1�g�gD�N�������r3��y��˃8	�t�q$a� �`��i���q%��zM�,0H��'��}F�0Y���Z�P�1!"�U��y����WNH	Fb��	��a�򓸯S��P���K�F҇N{w�tv5Ӡ�u�/�tʭ���HN�1�E�ꋬ��?�.�OR�v��T�tDW4I����s��J�6��\��&�:�t�6+�MW�_��/XbR��)x�k��|u+O�����:�垳�]�U���k�D-�������Œ�<FC���Cq���׻���<ﻋ��W��u9>���Nlr�5�J�Vz�L����r0��̛`��6/�&������p�����D��!�NBl��P��"܋ٌ�[�,yuU{�L,�d[j��3|��={cN-�1�C�9d���t�'��4��b�)I}X�ʽ��D�
P�
���b����J��Ȣ�+���P�YIƒ@ㄚfy'�V���p��Ɯ��BOrx�}�KJ���{i��/���}]�!۞�X���p̜�t��t���*0_ ��#�#�%�w�S��Q�} G�D��r��dꪝ��DR\*nB:so�c���n���)m/�5�'r[����Qq����`6�W{nh��Q�3��/���w�K|I����W�%�i|�r��{ ��1��"0/�ѓ��]֟m�	�P͜�H
k0�v�@�� :(�ܥv�̤�r��w7ބ// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/file.h>
#include <linux/suspend.h>

#include <media/v4l2-common.h>

#include <media/dvb_ca_en50221.h>
#include "s5h1409.h"
#include "s5h1411.h"
#include "mt2131.h"
#include "tda8290.h"
#include "tda18271.h"
#include "lgdt330x.h"
#include "xc4000.h"
#include "xc5000.h"
#include "max2165.h"
#include "tda10048.h"
#include "xc2028.h"
#include "tuner-simple.h"
#include "dib7000p.h"
#include "dib0070.h"
#include "dibx000_common.h"
#include "zl10353.h"
#include "stv0900.h"
#include "stv0900_reg.h"
#include "stv6110.h"
#include "lnbh24.h"
#include "cx24116.h"
#include "cx24117.h"
#include "cimax2.h"
#include "lgs8gxx.h"
#include "netup-eeprom.h"
#include "netup-init.h"
#include "lgdt3305.h"
#include "atbm8830.h"
#include "ts2020.h"
#include "ds3000.h"
#include "cx23885-f300.h"
#include "altera-ci.h"
#include "stv0367.h"
#include "drxk.h"
#include "mt2063.h"
#include "stv090x.h"
#include "stb6100.h"
#include "stb6100_cfg.h"
#include "tda10071.h"
#include "a8293.h"
#include "mb86a20s.h"
#include "si2165.h"
#include "si2168.h"
#include "si2157.h"
#include "sp2.h"
#include "m88ds3103.h"
#include "m88rs6000t.h"
#include "lgdt3306a.h"

static unsigned int debug;

#define dprintk(level, fmt, arg...)\
	do { if (debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s dvb: " fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

static unsigned int alt_tuner;
module_param(alt_tuner, int, 0644);
MODULE_PARM_DESC(alt_tuner, "Enable alternate tuner configuration");

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_tsport *port = q->drv_priv;

	port->ts_packet_size  = 188 * 4;
	port->ts_packet_count = 32;
	*num_planes = 1;
	sizes[0] = port->ts_packet_size * port->ts_packet_count;
	*num_buffers = 32;
	return 0;
}


static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);

	return cx23885_buf_prepare(buf, port);
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_dev *dev = port->dev;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(dev, buf);
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_buf_queue(port, buf);
}

static void cx23885_dvb_gate_ctrl(struct cx23885_tsport  *port, int open)
{
	struct vb2_dvb_frontends *f;
	struct vb2_dvb_frontend *fe;

	f = &port->frontends;

	if (f->gate <= 1) /* undefined or fe0 */
		fe = vb2_dvb_get_frontend(f, 1);
	else
		fe = vb2_dvb_get_frontend(f, f->gate);

	if (fe && fe->dvb.frontend && fe->dvb.frontend->ops.i2c_gate_ctrl)
		fe->dvb.frontend->ops.i2c_gate_ctrl(fe->dvb.frontend, open);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_tsport *port = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &port->mpegq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_dma(port, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_tsport *port = q->drv_priv;

	cx23885_cancel_buffers(port);
}

static const struct vb2_ops dvb_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

static struct s5h1409_config hauppauge_generic_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_ON,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct tda10048_config hauppauge_hvr1200_config = {
	.demod_address    = 0x10 >> 1,
	.output_mode      = TDA10048_SERIAL_OUTPUT,
	.fwbulkwritelen   = TDA10048_BULKWRITE_200,
	.inversion        = TDA10048_INVERSION_ON,
	.dtv6_if_freq_khz = TDA10048_IF_3300,
	.dtv7_if_freq_khz = TDA10048_IF_3800,
	.dtv8_if_freq_khz = TDA10048_IF_4300,
	.clk_freq_khz     = TDA10048_CLK_16000,
};

static struct tda10048_config hauppauge_hvr1210_config = {
	.demod_address    =