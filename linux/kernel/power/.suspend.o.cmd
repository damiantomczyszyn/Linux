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
		cx_write(PAD_CUóJ:Ô2þ”BŽÓý…QŽ“À ‚iãÏò-¤‘†s7XºtMg^í½06qÅEiõŸ–Ñ×³VËI*"I·äœõ×`ñ¹Ò[UÌâÖÈ;`ùâÉîú’Ä‰ˆoèXáE¦6K@ƒÕð#…É×²ðÔ3=ëñç¥Q<Ÿ“£ÆÃG%c¯¼ÀÞ©‡Í‘ãäpéYÇ‹Y®÷ØbrˆÎêVŒ–ë#Æïý©ƒM}H6—Øï°<k B×ª@`†676‹+‡È¡Ç3±Šc!Sç;Æÿ^›·å¥»&û«ñ:e6´þ=•BWº˜%*w_qv *¯0H	Â*nÅÐÚ…3ùìû°£ p¢yI£Æçf¼2L©âU^¾õÞÑß
¼ÿbÄœ’ê=ëÈ@=¿¤èÚcO¢‰Â(d|…®+I†È@ü¸)	Š6ëÍ¿íœ8d÷ &„ú?H¾ªHÊ¿…’@æFTˆdÆP¡˜þ®˜pW‡à_¹ƒÌjû³bùˆ¾£ a‚é0/tFÅ©XùK÷ºþ]b±fÈŽpW¢~_qH=¾H¼–D0¡#§	 ¨m©‚ð~¹àSé–½£%V¤!?,°ÔÒg†ë­cl¡ªõÖåw£õ¤ªŸJ®wÝÄÛ­ªlÇ©‚f
ÜAb8TÚ»E4Gð	&}G@Xš…¬ånÈ=ÿa¨¶è]	W6X ¿½l_¦&¹ŠšçF§Â“\°	•Þ†‰÷®CÚÂŽ”Ù’Z°]i:k‚[$;RAnB@²‰és(†¹>óÚ=?•­4þ Ò¯`›Ù—ú|Yñu¯ ±ÿ¦Ä•ArÇÈÉ®ù×&Î
æAZ¸±Ï&ü½k©H:šµÞ³©4Ì}nÁ/ÈQÃÏ‡ŸJãÖÔ»êÕŽ3!`S¥]±EŠH7‘aÐ|V_~Jš§³Çam‘·Eú«­p˜Œ')½ì{CÎ¢ãŠgÈgu)æ]?¦Õ³j3&iV™¤D×ÉØäT"É­ýn
’Ð™héò¨‰…‡¾b—.³ÿ˜+ÂY '[jñæ™ç–†Ûðþ—†-h\@0f:åôj9ð9¿ö}ð?¿V9† Ókbà -×ì8†¾åa"Ÿk€¥+Éc†_2Çk©ª+âIDÑ‰Ðÿäò}ÖÄ¬ººècFbk©çL>eAï)`ÖñíØRB:[ècHÎÅ$Ì¼:ÌÏäôJÙ¢ËËwòÍë“²÷€Ò&Û1c…£ÏäfétÀþpD¹eãó:D§—ù’sbˆêÕ¶èuïdº0¨˜¬‹q—©>™âkÌ|?ç¬°\gEùÿ«V¡úçŠ¡öÊñ¨mÖõÜp*
h¿æA»ù	ïŸ¶rìÄ¼¬Ù' oÀ¦Ä}¸Ú3‹ByÃj’b·[íÙBV•nÊÔº7¶‡}Y°eX»Eâ•cŸ?ëîÕ`Û¦=M ®• å§dÃÍÛ‚¸št£é ƒôŸD¦íÚr©Hó-Y}!~ã¢DÞz,W½q]‡ÁhEå<P„†„õq˜£1ñr@ƒ)2S®Ik@ÀUCÔOP]Á‰E©0ßä{ :LAÙû“íR´QÐFûÅ6Hü¿jôê>rÎÝâÑÿ‹6=©Þ°vCN2ø†÷zòc<lâ“¼R$w{&^jIi-¥ÅìÐ/šïJLË'\oz¤÷d¶¶+ï|J|.S=¥&/_[Ï´*vÝé–ÐŽgƒÕ%WðÓ+„%Ö_Ô;íñŠõAó2SÐ³ü±ÜÞÎØX¯OºqÉ€Þ*®gú{ ÔZÚ=„SêÅ:Nzç,^ÒhEÅv#²Qz–I¶!"Åj“Ð•â[–¸¥jŽá¨s&Šã`T]"È/LBÖHæÎíë*¨O&šØ¦wìÎNNo¼àèš øóvn	Ë•&¦ÜrË>P¨»µ¢&s4*m–5æ(JD$íë{€V…àÔ1š¼ª2Þq­ ÿƒÛ•öšYi¢*gu.c”w|IËáÃdÍƒÈT•“¦’Ø‚ò›á3*¸)~ÀL!G?)æU ¹täØqU†ÒZz)Ù¤£§?+÷OfñÑù£4]	SÎDJHØ$ s gÒÓ6Yª•Ã×
)ÙKC0=}ñ‡ê(¯XA$oË”ö~T§yhåœOK)~ò%iÀ­¶Íg›–ã€§¿öpXª²,@èg}EÈ?ûÊ¡±møa¡ Öj-t@döUÂSsÓ–	‚·ácÏØ©,GãÀTŽîöÝÎÞ	>7ãgîô™6œ>Y‹^Îaä-Fü'6L22
)D@‘«ÑËàè6ìgÉ•|•Q3H8 Q¹†'qzi‚kÜØ‹$(Ì³íâÓ~‘ý^F®Ìg‹êÝB¸C¯Ü–*„L#§rß“>ô5}éï–ïÞ‘OŒ3¾‰<>½h§²ÍÉ÷á®i– ê‰ŠgÆz7anæ‚KÓ§pÏàõÍQž Se†v”™1x,æÒ¿Ëïˆñö¹¨x$áP9h¢ªÿX-4ÓÀß®÷úÎï>s ´»^]8Þ$ÃcÛ©$®Â›múÇY”ÝMHô¥óM!¨ãÝ×ì\â	Ï½Õ#‚æ´(ÿÙ?Ö°ùš0ðV|ÜtA?ê¦Ñ‰ð4ÇB3‚„š%ôVv¯Õ¨ntèˆ«‡À÷oVÝµwâA£Tcb„êºrJ^-iPõ&C/Oî¾¢“§E¤ÒcËjft¾åGË“@}ñ7ZÁ8d‚Ž/šzFUÿj!{äsëc†Çn†é†?+œ¼õ–0Ž*mÑ€«2êifQ&nx[Û§X˜!Æ…çWwt?‚¹mÕ8­xbïLòÑU0üòÕœ%îcØý£f·÷iº‘EðÔÇV,À“xGµò_µå9Âä<Qx¡ª'˜ÚÑŽ‰Á‹ïMv¸ñ³ ×ÞåkJ}uC€JÙ`âô¬2×zß’‰àë)b9¼;÷éã!æ3/†çtàm²t9B)k¬-àžëÊ‚Ã UOí‘¼+…5ä4(.èÀÓ XËƒaÞ®„O‚òÕ:SIbR
€ëˆêÇ7½æ ?øEv£È#ák'b«ÓèÇ­ýL
î–­â§c®Xg¼\1°öåi8`YœÂ6–øqêužl,tÝ´R5 ¾ù£?E„q4°"uÑø)U÷Ÿr½*dÇ§Ö Ö‚m´HwõbÀuàþø¨´$¨Xì]c*Õô	T™ûŸUbªPO
yOYðwpš)4S%Žg!¢¦èq¤7ýÝ¾^;0w9F•Îe\7!#Éð2ËÚV)ÃQ³_ô‰¬b¾°y§+€6ªíê>ÈîIC)ùy–L°(t¸w½†“¹¹¸£Ñ,qââL*ÓR5ÏÖpŽq“êvnì+®´i^z·åÅYæé¢Íd°œGÕ©d·éi/ø8°ÛÛÿ,º„ŒrÁRMh`ð¨;÷xsÁXãbà‘ó WfaÞ¾ÁŽí‡ÏëÀƒšõ­+ÇFm%$…T©$?Þ²‹m•Z2µ>÷±Oª)ûµš¯äsíñ“ /6’}'ƒ>4ó›ðú^[ÞÒÈtèºÍÂùT¥“±+×Ìzl§* Âèêå»x[©ö4.èÆè)³|z¤Ã·ýÒ’›²œ›ý€†zÌy]lèpap‹1‹“
§:JÁ©$=×ßH
ý‰J¦Aþm\¦•ÂôÑdmõi„’¸Ó¹0ø¦q1D?þ'_g	KÕ"î[0‰’øxD®L\!pÂŽó®wb-§Ð•®0Ö–íx «`Á]bœµ’˜j¹Õösã½þ™Ô9×ÖJsàeÔ©SªOÃC‹iDðåãóíŠ€‚fŠCÓc3óf(%4í©d%‰S.v»ÏŠh?†÷—ö€«,D‘és“üµ“!1nwjfÀJHïÞ:L™K©Ocy†?ª¥,§62]éî¼’°}žýÎêÿtÀPÄ1ðªˆõƒY{·² ´Þž ÛW$Æâ9|Ïý°ºniÄlØŠ¹˜Ng L¾e4aúH4ïôã?ë‹„È·k;OÊ{Ë,ô#à÷!NP¿`J!»}ìJË?uÇ	rûèaÌË
ÉÙ=ä;âf§1[ù‹m9Þ4 %ã%;ì*DÖm;ž™rïD¯“âYS'å§qç(|ÙÆºúÈ²8¶ýZœ¦pj9O˜õ5„»ŸG„'íiÿo|	©‘|}–Dþwî† ¥ä™ÕvúÄUu]ë]ü©	ã›ÅxùoZ¸f&%»–s¸âá§”ÔùÒªú"TániÀØs„÷‹Œî»4ètVÕ±~PPñVÎo6»âMõÚ]þù4õ
ºÉ
œ’¿Tbyßb;tP-´¨žÐ,½²²@sâ¯wç![yéw‡_jœÝU{TŽå
Q¸ÃárÎ„)àiÖ¥æõ(ˆffžBUnÂ–^@Ïcø§;ˆŸÁÇæM.m	8_Æyœ¨œ›ÔÃ S|«¬Y9²“RÍ:…T/§súQbE™ù8êÅïAãSzNüšH‹£‘ck‡²::…¤ßÃ/Ø/Ïs£ö}Bcòd–ŒíqÊVž(ïè·…öRÁÑû–´¬ôŠ+À©'™Ûv,ÿrÉÄÍ# ÕÚðÖW
Xæ¢E&JÇWe”q´üï¼i	´7FÕõu%õpo¡-‡z ixç—FøÑ$WÎÐÏ„Ã„£A£0-õ°<q2æ“M5s•úSÇÚª¬Qx×‰%}Æ‚‘òyšŸ—y)‡Â•ÞõËmó•BÍD8ã6SdŽ;Y1 :ˆÃ	³	H¯ö%Õ›AËÿdD“×lP]jÞŽ„¸ÖtŽ†ÒÓ´TtO@rª96Cî!Ø˜ÝŽiU·Y4™fn•è»w"G&Ü5ØÛ”t8Ân:®!'“
}G#¿1‹@Ó77ö4Y
5ïÎ›hKûéa7Üw³5,»À€$A×ÅÒÑ¯È©sˆÈ¢àÁv~!Ý9u¿­…(ÉÃòìüï‹[Ê¤Ê¤öoJ4tHu˜€DßŒ"®LÖ£ÄS—êèSHJ™¸;'?”•UÏ˜¥QõUm¢Ø¾ OÕÁÑy|ªE”ˆOxYp‹9ìÄ’^KÐÛ„—”‹²_¯«×Êß”±™…&ü§Z°«-ú1‘·¦>Új1—MÈ¨Ì·®æŒ;0}ÀžªV´„aRÃ&+ÇF„3‡­UÆÙìÈÕ]ÊL¢	j ë¦që×¶É Àâ_°Š¿Àº³E§ê£ËÊ§ ²-Þ`ãÅ— @­Ì3l‹^áx–tV³€Z&¿j?™-(¤Ãw¨_ìÈRÓòôyÝï$þ+ûÙÕ‡<«ŸXà9¸	c ¸|$˜C’ ½–úoä:02i<ŽÐ~ýÿa¸š·`62XäšêÊ=æU{!›ŒŠÄåýã|ÜcõNó®¦'Ž=dú9£`^a³8&ˆyx˜áµ²ëE%Ëù†©àô2ÇGz´V¢ßžKEyðQü´bœ 8NœÅKúCŸNÆœ[Ã9¢ÝT÷ö6mÍ–«r‘…8Ãos\ög´]Û&'ö”×q	Ë¢»¦å>€KÛaV›ziKx—ò¯¼w—_-Ž÷çà‰fvÎ×;V4}u„cÛøá.Šê«s–¨³úb‚“:üŸ¨ˆ×Ì|Ø´C3jüósÚ’Ã“=>½ÿo&æï¬@×J¸ƒUS§õš	JN³$‘t‹3LOåˆl*—<`È;nÓíîß¡>Í¡ûA>x{Ñ€8+kô˜5`\©•XºT¿Ú»ÚÖsùÛ@}^1ã¿j’åæüQw$ùf*9¿ z˜ž±éðë’^?²ËKÐr¶î/ì½˜¹q¾ÞÈàˆcn­YÏ7ïJùŸM£¾V­Y‹éÿMÀêŽ{¢EÊz+ž(£*ÿÓfÃª‚õ^<òzážL¿‘Èß ¬û±šÞ”+s 46ì@íŒ0D³}»êñÉKB ù/l¬8\NåWÚ%­‰ ‡H7ó1 ðCÕ€M˜FüÍ3‡Ë!©Âªïã(‡A îÐËErÕ Î+n8Ò÷ÚòŠ’¥Ü žµhÄ8G›£$>j9M)ÙEå¾ÚÆz][Q=¢ðxÃ´z¹á=.Ò/ê´ÊÎO(ä‡4‹É¶4ÅpW‘+ž	`´Å÷¾;žw´fó¹ŽJ•B/Ð[~Üíäœ3	Æý!—Ø2¶-Á×Ör 2·{”	)dóc“d4†]²Ÿß,b¡ñ¶ÁP½p³,¹ô§gQoâúÓþu_¾ü;·Ú…Éâ9vøFµùš×uÁp ]Ô4/œ™ôC—à«ÏEßQwžhä—Ú	%QnÄM˜–îdñ¼Ú¨=ŸzHH&FŸØ¹cî ?xfÖ"_:$_·~ï#uîk•õÃç³9úÞF‰…íü|õ‚Œ¹XŸ?YWVMöZÇý!Íí?é¸q~›·y),Lî£aÿ¹°¸NÐQ&õüâ/—N%À½=À„¡óñªZ4êÎSwt{¢•¹–QŠf3ˆÀ³Ÿì·à`hR#2þ™Éš}L›Hùm`´W˜óÂ‘$µNóûÝÚÔ¯œzE‚|’e}	Rn69wË©Mââ\> ³&#ffÅôŠªÑ}”nÆÙzÞÐ1o4\ÿÕ4‚vª ]ÒÀ]ƒŽ!¢Š—Pèz¡ŸØ›¸Ô©¥!Öt2¥’côuþ@Ó1¿yTg>üì]iSwÉ"DÒ^!ZÁ»Ô4Ê¯ÖØsx9º7§5ŽØ.( u0$/Ô‘•×„p@®Æ(O—Î,ú^SŒB¤E¦‹™»¦e)û‚/",CBAÆÞc»4}J¾b ˆn¦´déæn¹J²)v‡Õ‡ÒÅ8ãi[ß”$2^³´hVÈ]éÊÝ¸'ã‰åZ•è3BºÝMSóÑZp•½—v!A50œdxž_NA6|@P“±Ã=iÞŽ‡øñÈ½Äÿöe	ÒCò\5-D‡
 §;Ù v"A¼ ÈQÞ`éâŠa[Ç£€•7Ö,øó$béuÒ±ÜPŽÜÑµ7’ÚRÎÁºŒ»Zl]?X¯p%#k‘¯à >‚¦‰?È$L`]ChTÏË=%j1ÕRŸ6«(ÈVBƒZí{ažd(ngÑ¯h
Á:û©í(÷C3n–aêr¬øS“ÇÂà>*<LåAÔBi“¤…å"ø¤F~÷Ôƒ W}ŽHŒÌ‹²Þ
+ ò¯ñ]þq…¾´3u€;‡ë>Õ÷Á
5þu,€ârØ'|‚‘>§Ûs3$`ôRïUë3ì àaÝ­6´¢hIRdsÜ›~tÚ¬{ÙoRX-òŽ¿…7cDu›áŠ×~„d„Ÿã\5öƒV@\ßówqwÌÏØÙ‡¢e/TÔEÙ‰*Höß§üÚÎ’‡LU’‡À—ÿ¬Lr©R
™ {ŒØ&|¶¸ªW]åa¡Ë£gÓâŸ#e€ /2ØÔØ~é"¹<y6Ã)
ÿ¢;{[Ù\o_ðíì¤ÍfÈ:‚Ç×¯	Þl*£6ÌŽú®×–‘—ªóFOýæŠ©0¬Ï¶‹ü¬¹“ÉAP‹X)‰¿°U*úÆü¡R»#éÔ9Þ7Ä ”k¯$2Ñ+Áã•µl×ap'†½hÝ Ô¼O±×8Ì®Ñ°ÉÅUìÐ6î;-œâÿÝûßyÁÔªA
60’Mj
ê•¿c·¨ÊÑƒª’YcÑC«Äö6Œé¨§¨)AkçÆfUù4?ã àP‘I:³= mÌ‚âï\ÀQèÇê’—ÂrEbjŠØ´"nºÐ;m7Ó¦Œ¤ƒ•óÿÚ˜¨ÅÈhQ Ì&¢®£wÊP¯êfê§ÿ‹>9ˆ$ÓÌaK›Yé.dƒŒ]é4$s?¬Ÿu»aŠ#ð#Úì8^†°^žE&€È3oˆ„ä°Îb|Þµ’VJF°lo_„,ñ$:#Îq¬˜pÞ×U¨œk‚…¥ylØÍ(çA+PÐ¼–].v¯¾@ÚA
²ÜÒÚ`  A†ü5×}Oîû¡E˜/?LµÂæOÐ–ìDÖ•²&~ü[C>\ú!Ëï÷»›­	[¯5íƒJ2ûÙ8cd‡î¿ÁÒ{ím¦*S®àuu#öþ¬ÖdÆ‡‹ý_¯ºS\-›´ááýiOÂùi¶¯]úkBÁÜÍžæ˜–Ã‡@à3òíNL2­•ÖÎR³°ÈÛ0J$åçËöo% Üò’ó^Uvúó”…-nz3#¶ÇSåYs”[ë³y	HÒÄ@—û9ž*LœöJúu ˆ¿4¼Hgƒ±ØóUlt¬û´…œWÃªªèg ÄÝrœ¿¡¬’B
yÙ{hÜ·ô9M´E,ó4	Ð!ìýð z%¿ÀUØ—Í-c ¦†y“¡Ž½¾8’˜»‘œÍ Çóz9ºªV—Š‘Cšî©FØu”‰Ç_£'F=Ïù¡æuHFu½ÞœúVö w²,Á§`)FÏ'?Â¶`6À¡Å¡ÂºÐÞMï/%Åj±Ô|ëRpª±ïG>ÌÄ¾ÁU“Y©±¼©Ï—
(9üWâ¸f’Ò§º< ¶*²}Ò5èó3»)lòµún .¤9eƒ:‰Î·þ–5ÏºÐõ:8®³á,‹.à4îÃÚ³*!òuG\‹åj©/7¥/LK~ê-Ëãù¢ì2›ÁGQ!ôßÈÕË4Á•;Ë†´fËÔM 6‚M`íRuî©ÀÆ-Å1ü„us¬{½‡5úiòzÌºv¶:e•E“x–Nä^lœêL.?ü,Ž1ƒ–¨	~ç¤9ÎMš-¸V )¯ôtž&ªsÕ¤ˆ5Î_h'H=ÉßhŒ©ÏH½¨úg°!ºŒ…±yòt¨Ï ç¦<²FTÈƒ›ë¯}H¨?.ªe6~E	ímmÿÜs1
v%¦˜!‘*b(®7Ma6aÌŽ%q¢‹”GrJ/þIÖGçÕ“]€M½:BWA>–@ñúø·kXÙÏ”/jà>d1>Š>QÚ	±ÞE.á@°=ÿúZui›õ„A…§Ï#M¥
”ryòÏ Q>#}6u˜V}¥ãcB³ß]ÒÚ®¹sCrL)BÀéØ?eÏóÞ5˜›„ñðhÝxÕ¾Ih«U’ÄŒŸ
¨MÔ›K÷!|}Dó¡ù-	Ð±ñ«œ…˜W‚@®e÷v~¦â–_‰°*ºL›<¾åÿ¹ú*6Rÿañ©±PÐ¦Awc!På#û±–v=’g@xhü‹íð× b,øA}û€jáø*ìÇæ˜2
8^‡¦}ªÏð‘¯TsƒÌnðBCßQ¢8#»2äÝuŸ»“þ®ã M¬Ù\Ä¿¥­±µêMç³8º’rG˜ö·9[P§$Z	ËêžÃÞ`ôFQ°‘œ2 ¹‡¬áÖ®ûÞpà5<‰.õCôÃ§Œ¶	zL©[£:j<H/
;ŽE6Îâ¨øÜ°n=18røë„ÌQ=Ý0ï¾\1”½ÿR¨èN[y
½vYq^Òìò2M;g¬!sb>|÷PÌ:¿a#ìûÛFQ€ð0Š«%®‹í‹-Efwí-ÿñÈã#50¬N˜8’]Àáøþ·cµ9ç¿%é-e)ãâf‚6 qP`»±¡²¿„eŸ&“–Wy›~¾\Ø~
ã£Bçw`PÞÇÚ¿Ù6/¾9-ß­Ñ+ò–±Œ«ë6`jpjè¡Vj\GJøÑ7&Pä«Ý½°Æ­ÎR‡¥å!N bŽÑÈ´wð}™C”Üü¸ÒU-(î‰ð˜ ë†üJ6ÌáóYÉ‡&.‚×Büyä0Cæn:˜{œÀöqJScrÅÊï–7§òõPë[hußáèw¥4ùøöïÝHU™
Øqþ–¹iZÁ{Nëw÷éœê‰À¡¥q”)Þ5ãàC71°˜!ˆüH/w}$ªñbÌýhYÛåÿå€ Cc›œËíM•øY@.Â·;ÔÊX' mÛVÔÀHÝk´†WÜìý)Mabƒ‰8£¦¾|ØÊdVMq'á§Rýlù”gVŠ_•YÃ_½Ÿá6"z~õ“EL¢Ê™­¾²×V…Nÿ@HAqŒž´–'ÂLR0«±!,µ·yàp/?¡)€1UDö¨®Ü_žcƒ›£æƒÉÕîÇRÔwå²ÍÁlâà¿i)~9´ºÞxÑv‹aWº÷›ö)¼©ÜØàì«¹¡‹~º¢ÏâÈâ9ãÓ®³ƒ.A¾s<£A“ã‰0sCÝƒˆŽ2ÀúîÙÏ6\Ÿ.ÍtFùü5Hyiç1»—QÍ:É?¸š§ù)¬„ÝèH.]ëS’Fõ•UIã/[%äE«ôŒFŒ+Y%Æ…R _D—½û"NmÚ”ð\ÚYŽp“h‘m0ñþßˆà¦)Õ†áV{#³D1#âƒSÁ‹ZÃ;ïC÷KòúßÎExÒÕôÓ)@Ëýêån¿¼Ã›t3ü@çöÙ¹…*üÂ—¹½CTJ<Þ<Vp¯Ë°êb0$ÔÛ…!÷é,¯dª>fÏ1V¿Xç!øâ¢èÙ½>·vÊœ;ŸNˆWaár%|¶`óÊÅìåÂ3î‰“ØVfv:ñZ­aìòÀÕ”ïb^L¾s¡vMPP°…ižÍ«~Ð©¼fËIWÒ=Z—B­;V/dƒw®yåå£§Í;Ÿdÿòkâä14O2Ij"$¤r ÐnàÙò*@M§=,R‘cØü¤Y$‚ƒ‰L../r¼ˆ9Žn%§l8‚ÝCÃM;a0å+3ç;?Ù\*T@{è1d%¿þ•I0Ö“¼ÁP³!Q»­½²$pö9!Óéˆý5{&¢/ª…JPÅœ©NJ2·ò£^¯åQóÿ™?üâ¶%›’d‹aÌ¬¿òÑj˜ßÜ.ïP{ZÎ4¤°þI×xåû±¬‘ŠË,DyëáŒæ«¯H¤î/Ý‹
ÕkòÚG½^‚-›^øÕ@ÞÏ™ ´ØÐö{î¥Y"u”šŒlÁó{gB)P!Œ©lÞ$¹ardnµ¯jÔÖ‰¸wéà±cR8Ù_äìLX/¤®hË‡‘Ïq?[Œ†—g¦	:IP¨ò£ÜXf– µ æŽ[bHZŠFêÃë½±J­„ÛXÔQ˜nk¨‘ÛVx	5	ÐøÀõH!5‡ý«`öU¡'1‡=iC™¨žM·=Ã'rÈž!ÀñýÎCê² äÂ©ñcZõ±òÈïü¥jŒ¸„
„‚oMÝÁ7	#ÞÅ\vË›aáIb‘zÆ€£¸%Óþ%,	ïªt·H:Ã4‰èI]ía(@&èŒm£QBDŒåstêv`%ÝÇX.«®°HC‰­ãœâz•Øµ&Ìf}ühêQŸ.¨]’à;žD ÐsÔÎ–ø«[™Üg*|6Àý
VÐi0Ù„ Xz¶[Žó&—ä¡zDW ?Çìù%-Â˜ÛN)ÉÝ¥8þû>Íÿø¹ÙŸ¾àç=ÅJéàý¸ëL&AAQ_äë½S¶C”U?”FüŽ‰Ö~vî{ü!5nW[+Âêâ³§bÂTü‡uuœyée<ë™-·(›?My7ßÌŸU-•	L¥„e˜ŽoÐ@ü¹­¥­ÞRGN]\Õ»5´'jÆ5"§`FqÿÎµG[:Ù}Ò°ŒãˆË½ôy¿ADSX5¦TJÓf²ØÝ>$}>—¸{¼ëé›Ì~×íO0#h "Õk&»ÇMÇ{ 7Tç÷ƒÓ¦éÆàÚnØ™!ojˆ®Ó©=0u3›õ‘öÅ6§×/ÚØ|Ø>(?\·Š¿~×¼¬Yœ_;$.R6á&+´Ì‡AÓJœ¬š6g‹ªZ…ÒHŽps0¢xzmQ“¥uë7ˆX\5;ñÒŠQÚÆ“_ÒÓÚ7¨û44”Ò­SÍZr*‚q g‡2!ðs~RÛš³2¢c[+äû©æ3æ$³¶8ÜšÅ÷y8×½=3I™øF„ >ú»}¤òýý£û‘$žìêËBË”àS9Ëë&PÆx²O@éÃ}SV›"ã_².4æþîs ß×ƒX4;¼0Ð¹Ña×”B!/¨£JÎîÓlû9Þl\ë¾òya¿Ù<„@~ŽeÏæø“ü¯Æ—¾>W,g^u†§`ùNh+½Ù’ÜÀ C„p¥èi .aÌ& ÆÌ¤?–.3eeé#}äÈ=ª>FÉ	ï]Ži¢`£>ðÄ;¸©â½+'…‡e¸ßÚüÇ›ÚCÅ¸˜C(™°Ë'ùèÌ0:ùÌ²¾QÅÙòW‡6ÔZ±?ˆt…Ä°¥{O€9~Vï¹oÕÂôQØ·‹ÇŽ.D‡ö€ßö,'4îG?Æ ¸èôÖ™¬Øò3aµtîšò	VüÊXúŠ{~Uî[êr§ÛÅÎv«fBç;½çM¦~ŒŸ*=lodŠóÛ-b¢¸å•j±SÅ) Ìª|Ýô!Î›=€¥œú>¯°pçW|ßçzkÛ¦‚3Šï²¥¡ÇúÃ/o&s¹
@ÛÇ¿µ#*ÿåî@éåé«ãi­A^P>ãa¿“+*È£Œ#V­ðy.ÌÚ¤Ï©ÍŸ'‰Þ~Hu©ÙRina„ºáëwWJ[¬æq‘+u{‡jySØ:´ËƒÅñ—¤×soéØ‰0¤"ð4Ï•µLl¸XIßÆ=\<Dï\ŸA—Õ'>:g@ÂŸ¼«ó¥&Å›¸êñ­(ÿ_®VÀƒ4~oÔšQµIÕØ¨úl>†p¤yÇ÷,,üšE¥
ägIµÐàt\OPCû¹—Á!ú¤D–ƒå\IÈ ÃÍÆ“ìIýµ2[Ï½—§Ë"Dz-²>Šýç›nÄ1½Z?âˆQüô`¸Ëˆõ5¿G ÃKy€³0,wUÕWbN{³“©FmŒVíSÌû;^vý\uòé.qîjD–š3 ‰Û#© –GÌâoßUsÝÆÒvt™ÿ~^‡ÒÅšuEši½’¸§uVÙ	Õ›G¥U#ŸØMóªÚf&f˜Î´ó&f5b©0ÞŒßlP²S—*tâE¨²¤–î\c¬ª¿Pºh$rò'~8‘=«Œ©¤-‚¨¼çª£Šq¦xYó†cÝ9Ò\‚»z„ÜJá}A´{Áþp˜c?¥;9n«´ÜwÞÂÀc³!e”dmï¢›Iýc2ÿ–ÖpõÕ· `½­ëŒÁÊIa|b`£ÒiÚ{²ÿºÔ•š¨5bôJ›:½»o OÓƒï%ƒ=åÌ÷ÅëÇ™^t.g"Êºº’}$ÉH7Nà¶jUqJ7NÒüo¼ˆæŽ3ße\Íµfwî«6iñC©ìX½µ™BÔ
Ò¯ ¼«ÛåÜNåÁö}§àMU–„4Ô•ë†£²ùµU
¯ÛÃÿ8t[V·§¨0_vr-lü½!ßÕ±nä‚µÄRçDA©;RzGÎ)í>'mæ^®›B)³ÆŸÐ@6™ü[ ø.}çßÜ§ç[D$Ñ=Dò)„“I6PÀõ`_·ƒ±QÓÿ Q®±¼3Sda+LPª²±®ë^Ð·à|· ‡À²¤Ñ›žý¸¶—„SXÏ†‘–4âùøßFYþD¿DÐ?0Aê–wŸ×¬wÔØµm„¤2qt+bqÝwïdiÝ§7qˆÊ×|&¾Ú„@‚u‡Ó[C„†54¯þ4hsK·™|ýG±hkÅ;Òi¡Z¯ºDÚ_©ñ/«ßVzžžÿ¥˜G{ã"b•t _)êeÛù¡S…ä¹£QÐ®«|CÃÕoû,ö;—u¥!³åÒÒs÷·ít£ú¿ðBÕ¶ýÕîÂxHGí	O»¥éÞˆÙÿï¬vÛ(žƒB=zÞÈNÑßwŒêÙÉùÿ‘4«itñØ0TX ƒE«²ŠJÎ2›“€ÖÞuIW€pùØ¯_/ß„èT
v×U`[$Ç^ß€}»xiŽjŠÂZañ¡J‹(–ƒ=a~•¡ Î¯åûDÅÇ•^³ý¨ËfýQljø2ÌK¹&k/Y+\©¦ àÄfå—âùŸh½Åœ@X'iH‘"z¦„”Ë&dª'üOAwC•_õŸçAbUÍ‚th'{c>4Iº¾D†èKŒR.¯F š©øÿç¿÷] ¸ÞîË\VªIÔ&/I³ãž«I 4p°|Ñå²ØqÚ"Ònô½çL5ž‘všÒ°É&°‹°.æ—ç\>}qX]>ÅøuÐ›ZƒR=CN‰üzþ¡,'¼+FƒÐÁœd£­C"êrjz$ÐFÀ|D3ÔèzN7XîŒ},õÓ…‚X’‰ª—×¨©*þ\VçF©¶¡[MÔw—anE¿§Cu½¢–Ëš[2p°¦+íNÅžvÓÂ×q­V$¢ÉœDñ¯ÌZÔÌcnã=ö;~¥t™+‰¨Ž˜ÖƒÇhÿ¸896)•ÌŸ@í~!#{‹ëá±ƒ6¡‹¶³ÔÀZôD>Ýõ ‘ž\šÖ&ÛtÇd Õ@1¾A»ù8Û&åÈÍñ-à"ä=ø–fÛÙ!F`]PùÖ¯ bê–sC(:õFÈÜ–í+u£PÑ¾¯ï<™áŠð˜_‘VÁüo5§ÛêjnÒÐbk„•¶HvÆN¿YQhœ IÚÆ¨‚ØvÐñ%î^h£`A]áÕ H5Ê]ÙÚU0…9ìÅÔ¼ëàwÅ€9½2‰sd^-´f šoÐkƒMëË©]ÅîM®ýZ",Ž®Y—°ÓD}c=¢èîs¡MÚ°U{;yÉ
Y]ÓÔìöÍ('7ÁÌ rµò‰C&^S"1­äE¥Z;ÈºpTXù:R+‡û÷G‡mYl.<Á÷Ô¤êÞúD$j¯@à;êµN¿ {&¾&N|òëQó®®?ÎB¢§°³f½ìÒ	)XƒQ·ßŽSksýÇècõÍ¦Ýæ5I-wI)¹¸^f¼|üªåiWî &©çjðDHÃÐK³{	©Çtm˜¡KÊŠÅÇ}XÁy›‰ŽÂÖê"0?Ô ÞÙþ<bíONÄ£3úQH2úÁ{.ÅPºà©ƒ‡4E¨T¸›1 Àõ<½Î±Ç?–SQ›`WØõ†‚Øýß»49{)ý%åE”Ë0»O ¨2Ìž r¬&x&‚OÑáã>ce¨RüÒ,•º°½$™¸.ðî“YnWÊn™UÒª‡Þ“‰E—ÐL²³¨úD¥È[0sv(ÄšSÎ\)3Q4*Êâ‘{AØ[;l˜‡ÛjìŽ1½Ã„lÕl“í¹"2YÕ–ÅŠ´Ž£Ä,ût¯´§±GÕ}£«{ü&àÈ?°Êúï±äˆ% ÑÜ$>Å*£ä½ò¨Èn–÷u¬Zâž¹u–)n¨iò:ß““å­Ui'i£Wå6—hãž’‹1 r:u5n)T+I0Èéì€T2;èÍªO|ò_mqÑ”ËÕÇÂt0ïÝI€D+tEô}Ì6Ó.æØ¤œ3÷ÂÃwW.5§vK¢hÙ¯þÛ¤_ òRK~mu3àÿüF„E·Å¾Ë¯F5“ì8ö05¤ÒÊnƒ«È$ˆŸ7º°)ÿTB:2Oü æÇ&îÓá¬=1ƒfç\°®Ñ§¶Ã:0Õ/1!dç†ô¨nS’È:-úi´*Gøißs.Ü ÙÚ?èD†œÏ
íÜ¸À	_¨;icÀ¹w½Ó¡â¬"è07Æ!*jÉxJÂ<Î€§QÝ{f\¬´ºŠU>Û’Å0ŠÉ,$6L²•rÞa™.7•ÿHÖÜgÀ½5ÕN5\îTu²]¬ºnÚz„5ð´¤²¸òšþ„tŒgê´ÍT<Š‘!ZY=ÞÊ1êõÕ2Cw9~ÛNÄ”ÕÓÉŽ™JE¤M×#ëèê1Û¹RbTJ`OˆX]!¯ó4ñ„!›¿£­,Ôu)6g"‚­¸cœ¯—»<K#0ïÕÞbsHÒ6 ¨8Wªt#àKòìôØ±ÙÜ–3ÔñkÛµ‰bGÄÂÓtú[Â S‚M	=‘¥ä9? 5©ÂÑ1™µ4ëË.sø‡2´ì¢@/ØÊ?é“S1×]¼°üúW(r›dIÀn®]ï²ºãºæ³ØÉdà°Û¢E…Ô&:ÜKÜúJpO¥­²ûÄ	\†[øÜ¬ÆÒ?Å”Q_’"&*„™aëŠËXaÎ)üH°ç“4¹“%)Ž¶Ô¼ybççë üÒ*”MKŽ?yQìs2Ó¥”à»ç4£ —Ûªëø$”k§þ¹µ‘Í’ü‘Ä'*°æú6S‘\µ§VÈ¨Ö®z×ƒ•¨±çàÏ!Õ›9øÕ]<H÷èJ Bß…Ã¢šVI§¢uöñE`žø@MxOÒÒðû}ô,˜ë²kù­M•]¸ÕGAraÖ»Þà8¢šY÷ëÏ´çÜj/_š³? kAxwˆ¢žÖ¼+=÷z³™nÀ!Pë^§èEÞ†e°s^ÍÐÏÆ´ÑI­$ä_Æ¡ÚRz©¶/7„Ç¾ä=ª®±€¤ãõø1Òes©_ïmSyˆªÎ%ÕZJOéU kàÁÆëñL¤dðêi’/y®õï ç“M:>¯ðU×JH$jN„ÅŸ&Q¼ØÉ²Û —SÆ¤þðBf,aM/KfXBˆ8Æ ‡#†Aþ«>q«=ÎT7úRŸø‰ó¼¯±¶O²iS¶»~=((dÏ|ó»nèLcÀÕF1H´›‡Ãlí„ ¡ò—dÊ‘üK¤ëoaê‘â¢M5¶£@]Ú\9gÚ#·¡€Ñà©Uí+!ÖfÑ›ÙZ‹šÛú >²TÃ«öˆ#žÁ˜¤:l«[ç|¤ˆ­|TºÇ40}/§×ùpÇy,‰)–>ÂE¾§“åÜ_öõóàæ¹£Â- ò)òOnN œ'—£üþ%ß9è²¾à&Í$l*—ið–»£­
aùê ©\ê‚n™«¦ÞE(˜¾0ßaô}RÊÂDw+”yÖæ°ž?jçU&¿ÌXNV„ò˜²Õ¡€æHŸ=Zª¯OHmQ”9XŽzal
|–¤ðò˜³C3ê†›÷º)œÅÔÙfêÑ÷öä§ŠŽwÛÑ.øI%žýÁàœšd½ÿ»øÞ´*íà»´œ4¬ûÕ ôpÇ5/ÈIARœpÃÌ›ÖjL=AÀ«¼h¹Š“eÔé‚uûû„íw%-¦! #Žö˜ß‘Ohâ^K)BE?¢ÿ¬&bzq¹»b†jÁh/á ;	9‚,Ñ¡u+@`³¯µOu·˜
´ø	qolV¿‘Ã”£€£‚RJù µ ™fd‚ü¹æ4ÒƒÅÓ´.J!S¶ tæœ4Å†aù[¢Ïˆž¯A£ü^M!>il²Øú3Jmß\OÖê²…+2A64ïÑV.á$ô¼KgßÂmý©7Â'e˜üU–‘`õbÕKîCþ>*(Ó¦6ó•SÁö‘iòoáS¥­Ž›(_—ÄÁ×ú3=ùíä…e\0†y¯
š§«Z¢¬N¨f@¹åNÝÃ!€@ZOJ4;Æ(Ùãï­‚’>Ëì;G%ŽÙ/·@
’XheÄX%r²Ù«aB6¼Ë¡°ñdñ[CÀë‰9—F¥ýnv×ð2c–‰pÛS:ýJ,ìý„f6cYŸƒ£,-œ`Ègóºe3d!.ød›[)Â¯xÍ÷JÏ'[e€âV‹SE©-µ÷ÏEff>>Þ náÿ~ªÊÛ:¼`P¨*€&H¥Fƒ&Ô„]B’aºwuÆ~[^#\•àÈô›MïÖî5¢ðžbù(€Ã%U®ˆÚ2/!¯KÐµI§†::EÆŠ=Æ)²ûª’KF{ÙmíVò$;§=•:NKz¸°§¦Ó*€ÿùÇ9ÃJù€ñ÷Ìø„ ’P0Ý‡Åc³¨‡ž¤o†:Ía!EƒL›Ó¯Ù7úZpóªÖCl2‰qÆ©"6+i	åa|7Ç‡˜çÅ«">ñÊÌäÅäNôÓ_Kaèb¹’®ÛóXý¬ÃPŽ4½4õðIÓŠ†Þ¹#8îïÇü/?j©ˆ1³]
î£0ããi]	½Õ$XœlØw{rwps…¡• <u/ bÈ‹ç'n7&îl~ÖÙk³ÄqFX	°Â=ú¶¤å¯@×©IC¿s;Àa ÍR¾j²“N{¦Ø°°~«+áÕ˜ï”V­´ÛR–nF·Ò˜k}\ÕìB¤ÝÇ6ïgNxzËžewQÓ‚îrà¡²;	a;†ç%+fU¿—SVÐ^‘ä4´ŒUåÛÀ9<ñæ½×•iŸ@åÓSWÖOÒW©š~â¢hÔÃ¯y2žüTãC£Q
Àé4tlo¶:}PB×=~TÖR÷{u9—ç¨að×çý#‡×:wýF¯nªäE“„ÛÃ(+p{Æ†y³²ùù$N$ #sÉËj­î9¿~®ªœyeÃèW5wI9ú«ëJ4tíº	¢C„1?ÀÒü…wžhp§A&Oôûµ{ýN¿Nwwb#ñpLÇß¡Ro7·”Å[ÞVáP&ÖOÐ#2Û7K|h2r®P?0Î)l€-ÖnˆO HÉCç?[Úá;­ý&?Ë\€3C`K&\GÓ»-r…%Yíû­¸®œ×‹š²HÛí#Ž«¢|·K&õ&T+y«[Ð¶XvtHª%Ù¡¾ëWËìù&ERY‚2hÏíÛ€[ùÖð×ÈÎSÆÉX1tÐwïšW]š¦ ÒûâìëàkkãÂ	º[øCßpÌ7·7¬ëŒEƒ—>àq­•7ú¹µÝ[ŸT$’ë·ÌÛècvF0íN&ÒH¥²	Çðä)`hßd„ž^¸kˆ¤•ŠG¨Wj¥ù"5›]Gwh3h^XÒŽ–R¾ªúÉ×ðÝ´óc.¤‰2ÄnaåŒÃÒÐq¨ñ3² /ñP(º³¡±Z§>ò­>j+{[´ÕµVwáy@EÅ'lóú¸1ƒ·ü½LXdˆrV%sè¶Û?ÑdÛ›ÓP|tè››·(ñuÀš9þ*ärX·KÕæÃž¤V|/²À(M´ûc½sVO¼Î ³œŸ±ùÇ7´EÝpœ@eÎpCXBpœG]šºV%ÛÖüŸ&òc%MD§(Pó†®Îæü&)ìZùÌŒ\Éš¨¶”Ÿ•÷Ýo0%ª£oƒ“b¸d¬…'¥&K€þWÜs'úÿbA lÇåC£ù“”—!Æ ×5¤êÈÿQ|§fd‘S®áâˆ©ßU:ÑÌ·¹¯H¢—ùRâÐ ¶ °*ð{´n-æÒ`›dùÙŸaý;åÅXŸ¬ÍÁ0c?êµ÷Š{ŸQB˜ ¤£¼5¥Ø¼&yì AoÇÛZáÚUL¯pŽQÂyÊvF¡lœÎ¼;#½‚åÕ©Œ´¤E´„A²l
›á&ÍÐëHlÉ ù‡ƒßˆMešn†ø®jMÛ~ª›võ ".†	ý©IµwõýPLe(1plU'áÕ\ÆV4šâ*‡¸Ýý_½·=6·ž;¶2wü–)Í üBc»þp—ÏøUJg#+îípEU´¾¦|t¥àfIàgnuäÙîd@8Š«¦TKä<å)6/‡4KÍŽÃYùì­û¬9Ñ”•¯«¬ë£äNèí mM7wÝ­$o–¹„>çêÓ£»
põÝØÖÄP”s‘Píª+è=hfÖé#‹	ETÚ’©*ûZ«Ðm#‘:X’2t`8VcTŠŒ-„fBý¢^Ò"Asº„vØ!,ÏøxzÚý«EÆæ)¼ÊßüÙ¥ºüï¸Žÿ<®UÇÝ"’¥}!¼Ž7ÈÅ8Ú»ÚôÅÙ	º§•’'“Ä¢0§Ã4\-Ö|<,@ùUZ`‹g¹EWé+CHÚŸãÒÅ¼Ž³èýËñ–=X{>fit|l¾ÆeWÜÍ|=Æóà`ñtLš©‰Á Éé‘Ràx¬y^;¥þ“äOÕƒhÒXô¡ÀwäIÉsßÔUg˜8
·ßïD¶gÙé-pR!ì´WÓ×Êö}q.Ò¦mÇ¶ù™÷0{)´WÚ}ë´éVÒ¬²ûü×þcøìOz<¯­Û*ó>Š½Y°Ò`@=$Í§LLÒdþÅzNÌÝæ¦Ÿ9ø0;y\„É#ö&Mt t¦iP†AÎêrÝúoßÂN˜ˆ‰„¥ÛïâÙ®vˆÌU9Üæª—xQ?Íå/Œ!C!VëØ%wÚÒ-¥,%¨üxh={©‹rÝà/_‹óGhÑ5ŠhºJ˜Uå*®=r§j4…Ççš jä¨OFâè5ã°`3|Ææ*´©ÈÆåJÍ6U&þ¹€Ð@GæG|=C»å$)»‚ÑøJ~ÜM&îö”8Q]nZ®eÏ[0/ò²ÞÔ{ïÝÇê©ÛÕê­ÑÍc'o»ÄoÇîç·³¬‚×Ù„ÌáO2û‰#ß­ãYîRV¸¶6VŸñö¨SÌØÂñÒíaÄ1ÓHùš\3hÑºŠ’ÞywPü\’¥†c÷—YóÍÄ8»á ãa{ÙÒ`eÆ¹¾õ’Öè}¹Ÿ(«4“M0›Iö‰‚ç&ôQ"MD¢LÁÎtq§X±ý ÉøRW­VËøN+4êÃ¥ä’ðG…ë¾Q‚ãzöWšæŽéëP¹×j8·ÜÅXÝÄça.5þ}Få–î‘ò8°/'aŽ™‚¾OŒÑªŠAVÓz#‹ü²Šéž×a™ÙÕëp?…™) î¶|\ÎóÓ
Èjrñ@Ÿ·’â¯*Ò'Î€ôîº¨@=Ÿƒùç#á±E×ö¢ºAK©¹Ýñcå¥þÀ‚‘Uë}PJà¯ÇMêxÎ©ã8Q±ž‘Oˆaà¼-_ŽÒu³LIux—£0&+6Ç3û<¼l~$*;¡#H›éf©)5%6IzLCKQG]O	P ‹âÿWiU ®ËåÁ@à:ú«û‚3™­2ÅC-Säé²ÚW
qk3noæSßÜ’†À«!àOA^c¥ŒàïH¬~V$ÔÄ•S£I¢“<#)å¦Šóc»@öî¤°*ü0i6Œ;kª‘‡§æLÛB:Ô¾Pî
±²uª6s}~„|Ð€{¢ºž¤Øbä‰ÊZ»ZœÝçå#ª&¨X·ýñy+Î\9UdŒ.æŸŸvb^B•{€¶Q%ê÷˜¶ÊdÄ&Ý/²™ ÏpØŒ&‚Ã.Óetgâôo	~3è¤ ªN¸÷Äxzº²¡“QÜ'ÇöôüŽ®ž$f­ûÄ‰± ·â1Äu×Ó„90¬¨Nô‰INgš¦3Z!F¨$OOyÎ[Ýä†D*W‡åÞâ¥’(–€ªÜ€Dbâ[æƒªŸb¦=Rqân&Ìn²>(tâ¬LÈáNIâ¸äæsJ·7:&„R?6‡
´£ñb3 í³pR€Kê§E%”$¹˜[ŒtÔp\	M²> Þ‚pú3Ö½ÛµÞàÔý%r§a¼gk	û ­›9X÷¢ÒJ‘÷®ºj<h«í¿¡kä¶©¢~(¸ «a‰SgßÒçÖÝ!KÔÏ’ogtY©€T,¬Ò¸ËÆ2Þ´ÂŸºd’–‰ç~h¨å¯˜r°$w4-Ó„ ‹Øí-9ÿ¼@˜Ïq"ôÜPW8Çß–%.0˜L±=Ä×ž¾´Ù¾½¦bð£Ž'6'ªŒ1ìd([#Xs§fžDÇð"G·úŽ;tusï¹|b¯
ä>T³MëÃ*Ê‹á½J˜Ü¢Tdã?&ÎžÃû{Ï6Ìä\kgF¦ma»^ðh¼«ZÃ>šªºá9	OPÑ¨Ô6º:Í”‘^úf³<–IZkï1^l%™ßœú×Â:àÛ™òòy³;:’º·ú“€éN(Ü…˜_QÀü(0ÿ«ÃTÏ:Œã°Úm$‚3äÊ¼N¥ˆjÐ£‚[W°ôþ!•Ðnò®™…²¹g¹§F£&6š4Ð0·ÚÐÂ–UjNÚå_Æ&©hÎ‘rOJß2‚1±/y‰|…ŽZr–¥f9EŠM•6µÓî	È*®O($Ñm3¯yÛ3‡©;´ÓŒr	}3¡‚mÍ£N¤ëieIfFm*ÑP‰½#‰Jâ·F³Èñ-!•lýÿnÈòm`ü~‰·FÎG¸ÒÁ]QŽÜ´¥E˜e	
ì_¤ë"²žMÖˆWÕ×ÃÀéŠ÷ÙYÅò Ÿ5Ò¾k{Ívº_É¿—ÃaºÇEí>ø’ÛŽKèÐ0ô·•"ˆ?,¸ãbÔly€£òVîã±JàTƒ†Òüü‰y›‹q÷@iR•Â@ i;ÆÚÌ˜PÌ#>ÎÁ¾¤ŸÂýÐ[adÁ1†ÃªÁxƒ¯YøýõÜ[À$hÏ*j„r>MEdòV[æ|ß¼ƒéF\tjEhJõ§ý«k¦¨Ö=Ð9¾ÌåWÜbÿ`qËQ±eÙœ¬š“Ïi"N}Š¼ûo}ÂQ8Gî<|;71dôŒ4¾ \¶}OþÙç*fßUŠW"«^Á2åŸ¼H	ú«3Ê§Sêh”ËÀŒÃg‚AÊÌ	oq
Ênêp‚òš_âšSè½	Á¼ºêoLjåfeA2Œ{+||BCt*,„‰Òú™Ûˆ† }‘nº.˜F½çKÝ%àv‹~ã=yš)	œY€¦¿ ²dï –h"(vw¢ìLÍ1ptLÜßu“$˜–æ¹Qœ9wËÀåœT$¼»{Õ]æÜ‰$qg€8#$ùü›*þã²§üvÔ—ÙéÈ|H¦vPy—cxÐïkë¡v
-Øx>Yjã[ÉQàæ<ññz<à¿ ëRdbÇ!6Æ;ïÈÑž|Gd?1œTõ˜+E®l¤n…´VcX¾´ˆXŠQ©-Q/$Ñ€9û8&Jô•KÛÅBþ´ê…E&¹¶’Ñ'Ý™0­æ7Wª æÓ¾	yé÷È÷Œ5GuŒƒ—¡^Ý9Ä¦…6A½ÝnnCÆ#÷{~P·iR9M×1©5BÑ6TægÓÌTSºDÂÈŒè½³FXœlÓc ŸXF½pIÚ‰Ÿv*Ä¯wÞ[1ìdÚ,Fißh`àë(<OÒ­bR |.&%à¹sÓ
‹è)0ÞðœX§Öž¯§>ÃžRGÊ#/Q.‚ÖÁÍsvãB´-5}U‹Ve¶ÇÏê{mÕ”¡z‘ooy;ôÔ³"m¿&Z€A³ë;æDl YØìÕ×«R{ÎiÁ*.XS„å2ðhŠ„‘&È÷ßé 8¼©…âíòBÐ«'iÅÄÒÁ-<‹Î¹lVlÍ7&ìÓ7æ'ÎÎ$6,äñö|r?éÙ€BYãRÊÃ/ÉT9É‘Zq'YlwÄuò}Hç×1ÈŽºC"¸ýí’¸	úçË%çj´õ$JweÜÚlSj‚ÿ¸·ø^aºÁ©xA=­ÇÄê°‘"°"Ø­rjäŸíØzä¸úÒÞ¯Zªó¨H,4tµ|Ž±›059í­Ö§¿´"'n¨FP.0~§°ÖŸÁTíN²\»•yÙDbiµ{#lÿ‡¬ìÌ$¥üq][è¢Ê&FŸ'+‚S§CÙÈï¬^ðÏ]æº5h¶O±ª†Œ‡ ì;’·Ui{\­|ô‘÷½žâ	Q>1…ï Ç&K$~ñy{=Å¬4¡÷5¼’ñé^ÖÎ)ŽGvžRìŸ<7=hùÞ‰à¸—4x/½
¢=U—¶*Ÿµé½/ql‰¸}·]éÒVÅg¦U}˜ó0·“\§Â\ &Že@ù€c{A -~Àÿ}°Njs³'Zs<Eí‰»&éCtçÇ#Ê­†½ÚU_V£©M°;RÉP–Áe?¿u¡ª÷Ï—Ói¯¤	PxãH²µÙr\»#óeÍøÒ‹>¬b·\âãÞ(V\ôt°¡ p’o=ûeU—%–¤N/#*eáŠÂVçfG3€”VÚ(Übª4È„!!&P[s‹¤9íPA«Úæ)£ížgèI:·oŠÒ0®Õä5ñ‰[×•ó}sœvlŒ±jÙhÀžþhù„æ—9ýË°d±9–ÀÂœÜw¦$è>­¾›F‘G“Æ=8”tÓ}
Ò‡¿¡0<‘iL;I7Ø¦üDr¨¸ƒÇ–°×Iø"J÷ˆ_“¨¢ß7'pIÐâœ­¾;l @ÿ\ÅJt¯LÕøz›Wuâ±—Ý§ˆ¾`W²CÕç'üÕ*¼[AJô=%Š¨øÍÚãºq[çg.62oJ°­ ´nnPðè­XbT}—@ˆÐ´úöû~=PF!nëDÈ€C§ú!ƒ‡¨Ç´*HsNÙÑ‰Äsì\½ægÓ­…;ÎIbÆèi^QZ×¡=}QY"ÞU¨# d ¶sêUx!)4¥¡ñâŽ0›’Òj& ¿›±
l1@¹µì´p ö¨%ØÆo2;Ñ·Jjßÿ(Ätn»·4¨Bf¦d®Ä\Èn(q¨HnÜÇ@¿íkPvAoÔS¸Xöøã4­”çí§°«åîgàüâU¢°]úøv9Ÿt¦ôR C²Ÿ3ì°[¹³††~Çô¶KÁk±ë(ÏdIÌäÉ|+ò>õQGÌ^EãçÈ±¦¯(ŠN};Ón‚ÒAãËÀW|Ësþ$ t³¡Û
%p Ä^v
W|²’¤3°e/[gud 3¿r5pŽcô½œ‚2ÀËÇÞ%Á›©è<(-O‘ó^ûûìé¦³tÀ±™2%ŠqDM&Vþp=BY›û$d•\acñöÚøHY“‡ÓéWÒN5í¥Ùd È;¢„#©ômÛGÆ	ÑyÆH½£õ*ô)Ót¤•vâØ.”QÐäþÅ#öþ
;ý\ûGˆïÆ
X©ýô¶Ì÷EÀþèì)’1=Iíä ð)Vý˜Aå¤Ù~¦æbç8÷êTÏ³­8²iãÜÖúðqCÔÆ*“Æ?bqÀn¡ap"–­	W·}
¤¦LIï2Ó¡[|Mt2]ÐŸ}¯®ÈÖüeu&º=“à]¢J+1¹ÿÂµR÷û481{á+T<–ª"ÕU&jÃº)C©À'
ÇÐ½õÆ•ï{îwc•W ôÐ'}–¼Äb	X?")€@H«á,«ëÌ€JÁlXÏ@CpQÚèÏ -Áf+A¦®õs•mžÃ"yüÝNäY•w;|Á÷jgÍ±*ÜáÄÑxï©ÝùÈmtöž±ÍÉÚ5”k·V™p£È˜’Ô	D”zŽªˆm"6¨"µ¦W„®…›h'`Çný6qÑ?¥œN™ÉÉ_MÅª¬±Bw˜ªj³ƒðKEéZ1/¶dGeÊÏ ˆZ<“SÔ­sji¬³(“bÓJv,ˆ™D‰Už´gv¶ò‡0Þ8é¯Žº¦¶­ú³Ä7þ±ÙÑ9îH*¾BmnG^ Ú£ª6°ç2|G| ï]<(:z1Þ€ïÔ«ÒÇËš°¤€×	<—‰i‘Ð§ÿ;¾‚‚?ø§îj­Ž½+*YS*’Ræ‰Y$â[]UÞ™MqE’6q$žÿ‚ïþ»	¿3ó¿V­‰õóD_ºªu›Xèr2Ÿ^R‰«îYþz”ªyèHZ6YwpäÞØÉë6ERYÅéƒßú¤—ó§,q?ï›à­>¿¢•[•ƒ3Ü>zfÕ¥CÍY¶‡«ºŽç]ã/YácsïEôÆ>gLô}X…‹|ž[_ï6’ßË$YîlDàN”±x#Ôü®òFÖâá£ÿµÑ2ÓÛ*/À…ÈAw|æÓv6rl‚ó¢á”t®d4O`_îxÊŠfÖ}J§ö!EO+—ìÞ§ºÝOíÿì6Ð5›çÏI3¾™ôGyÄ´1Â^+ñVd+\ÿwÿˆðæÄ›o¯\ðÔo`Âm¤`e‡ƒßòùºèDª­ foí2ÛÁà¢dŒ\j:ëíFH#ñ£uÊ±©'Y–7Vµzæ§­·ð:ß`3&Œc=ùÇ`h¶¡ôØÿÑ8Ôi×xM^[šÊjiÄVppõ+=EŠ|Âd“Ê–_Qfí%ôi“À3d‚;û™Š/Dþ@³
yéò1AéŸç¾8Òv#›Jè½¼/Å¥Çy±õÐ|-·b¶ Û;4Z«…±½ßŠ/°1Ï—Â©Å<¦2lw›œ-]|f~¹T6ºnüêQ0ñ#!bÏb*»j¶¥±uC-#WôEFGîÞädàöG!²óR÷$F­hŒqÁã¢¶Ç»e›+rª••.»]6X“Ëi£Ìµ×#v7/n!¢›OŽÐ¾"dI®5Ë«ÿá¢´Ân\[lw©¿2aå²˜a,Õ—¦Jè…S3½t@^û0p°±¦öÌafêˆc><ô=HüfÖÆ75¬—¹!™ç¤qã>ý›SqÒ;²Q9ƒÅ&H	™ÚFÈ‘?ÊÛÏŒAšµ¬A«ÐDs3àg~®N
µ"Ùigœ3NÖ/­²’'][¥¦ýÕžbÇV¢÷+>sŒûÊ:ûq…û”ÏÐp'Py€QiSm©ªˆ?úÑçËê$c[ï¹©ÂÀX8’çÓÁ¿|îY©Iwå‘œÃ”NË,­«ôe\LÇWV›x£‡dMÓàDÓ_qÛü©50f«XÐ—3E‡<›ò›Ó‡|bï}Hßñ9ÆYÙcËžÆ÷ñb"Ž/„Ð=_Þ©Ž8²B©ò|ë3'É¹<ïÈð#saùs,:b$KŠ0ÜÃ/åù»u•1–czÎ¨²Sš1úp‚C²>Ah¹\1@³BN¨ç™:ºï't’$j^Y™KgåÅ5C\Ä§mc	ÍñµIq·ÏäÛ;âû–§ÄšZ×©É@˜"uâÜœ;8šYw@GbŒÔýï‹Ê«²-•ÔÈ?uWŽT4ÿÇi›è&L0ësßI‰ÝÜœÉ£›ø
@ÇÜ šª\ûì_n†	¨DëÉÿÇ‚ìÌI¡d6X\ùž2õ$&¸#glí/Ü% þ>®åÝÉ¦Šç}ê•éß}ü
-8ŽÁªUh†BË
AÃÕ¨Þ4Püà“Ž?ÒzïÂi‰[³ÔŸ4˜µh^hsSVÑç'"p.K<zýjå˜{Yž]|<@îØ,•öÔ Á¼ë1Ÿ%cùÇ;f@"*ãþ3»´¯ÀîMd§™£âžÃ’LÀcvò¸•GÎ4nÍ3iö»wÚäÄv]¾`Æ»vð-Za…¿î@£>’ŒŒÍ¨ë•Üì¸z¹i‚7 ŽƒE‹¬Y‹bQk ©] ËÄ@›«šâ|ÐSmô˜ÖÂ"ö	ü:ùVoü"	qÅÓ—ÊcY€òÁ1¡F3¢‰64Äx6ŠÇ4bcuDâÛ ËvXiŽ©±¼‹ûº›ƒ}vi¶ÈhMÀÍãs…3z}ì1¨öü8q>ä*†ã8H‡ÝÄElDClÑb~­%¾-­þžf>—béœDHƒ>9ú(r}é}Õì¸+1T‚Ü¡³t¹ô…óÔ#®Ggc±ñ®Oi5:Å5^ŽÄ{¿S ¹ÚŽø­£PŠPŠÅ
È/‚ƒ…Ü®+{°ÄÑ§l<M&Î3ðrµíRä:…—•4FŒü7`Ù(]d‹ÊÜªq&¬ Ì“5Ôx‡«cêeDÑÉ…0ý~#´ÿohÓ8ÿ»2é—.6¹® ùËð”"ÍŠü(0ÛÇâxtÁJºúëµÁ¹-^0õZQ},¢‹Ó,¤#;ÖM9…Ž\w¸–šs#ÖÌ)ÿí&ŒËRh=Î‘ŒÓ1œ!+W§F²8²~nzþ6Ñ65Ì^»*niE=A%$®ÀÌ\Òô¶µ Ÿ•w)T³¨¡æ])@ÇÀ^®Vx©(¹Á•}Ñ(žæGÌÿ%+ÇÓý4·ï½‰ÇC¥®%Kçý·fËáH`æv=UÕÙo„ãÌ©Œ`‘Ù6â[::p2Šæëu—tpxzÖ(ÑEÑÈ¤$±’]ê »ðX	3~Ç .*ÜãôÊ(ãó¦Õâ"ÒT€³IÞÎ4¡ùìªrÍ«j7”Hýío¤ÈªÜÃXVÒŽœÊW-Ééz!ë,v0žQS¸ÊË”¥dŠŸÍ5j±™©Û^¯5´owçéRÉWÿó9<èÔ¿ÏM±Ú‡ —OþyçÆºJ`K¬õ@Î$û’©%\ƒIô2žvä²ÐLjêÃú;mÌ"Qôš	hLÌÉ!î8à„7I):èB~žèèoÎ í“½‹6±fb¹%¥O“KköÑqÕSkªYs 3hpPºž(H™ƒÙ
iûÒTêº`	?ÄJýW#k„ùßƒ’`‹{ZiÕ$"-®qK²f’â³pMeçàÏ§ÀíÔx0ÆœŒ·NçÒ˜ä]2ýJxö:l‰ò\$‰fY˜ºãŽÆ´®@x&N>¥Ý¶Ÿú&'ú™™µ¯nOš.ƒ¬;oÙË>µ´Í††‘qÀ^=WØÀ¼púr¤Ð^ ÈÃt”â÷ðâM”Ýg¸I•Éþ¾Oë#âäæ—YÚ“BŠd%'´$—%»¨Å«S_"œ)ßbq<·AìR=ö·ùÖFwÔÁBÁ—ôp®Lû*ü]ì@a‹/Mo*§LP x¥Ð¢ü‚žËE‹Lµp±9é¹áû2/‰9®à®øå{)mxŒéâÇÆfË¶¡ŽÐFã+ô'2ctoÈvÊ(K“¯>ã0²v(*®<ÛW]K7ÛÍÜ]t!OkYj~(œëôîÂ‘ûáìÄ’yU¹ƒÃžÇY¨kéíÈžxÏ‚åv©Bèÿ<;&Š…~¦‚ùb$Mªñ[¹Qÿ 
2ÔmþÊ=kæa×vÂßá
 Wq	Êëb
Ï´ÕY¨ýÎË5ÛÏ„-ÌVc5Ñÿ¯»)¹„Ÿá‡3
Ÿš;ø)Þ7ÞÓ¦‡Ÿv‰’ÇO•4µÓ›±ÁÕE ê¥þË@2Ž©
„j…0YV³‰ÙºÙ½˜&gz±¨ü¢%s%ì+âîV(®CJIô^Máå÷ê_Ž©ðñêŒê2)v£ÜÑA(øÓWH0>N„¹ DäœÓãÿt$Iµe¬9†Ûê~©a•Ö_+w¹ËItJ^,D6HkE-ª»Ö‚·sõÉî¨«ÿk|=BÓJ¾È‡mýPu‚J”žæÐ=dgdãLèkåº±-$OT³¬ôhå$^òG6z°žÙ ªX¾ÂpïPvìÁëÀ=ÃZ-Ó¾8véÎ3èbv‹ÄVûlfð (Ô›Î-¬j×PP{uH	“¬Ž&Ë¯xåîÖî(9=êÞ>ÅÎ÷ï ýpbâ±à½=m_žpŒ–f|˜ÝôPiHŒ\¶£Õ¸ë.£=Ue-ôÂµeêw/´Ê:–m˜G ëže×jÿx=¾Æ1È\Aò²Ïáf/©“ðò*äþqúEÐ ~Ü‰Øý*¼<V[°Ö–+.¯ÇÁwˆ*BU­ÈòZŽ²X¬ë/ÿ—*¢¾]“"þd;ÍÃû¸2‘mŸç9FkõãµçG²KÖWE¬h_kPà±3’Ñ`M²°4g&í‚áˆ°9'.ÏÒKƒ£“Da¼ûC‡mI·hêÂùÀøäô{å“@Ü&¶B4m<S;¿)4 	j‹º|£*ŠG‹Ä+ÓHõúŒ|áÇá€ÒÚ—Ñ£\é,ŠÄ<N~Ší‡×;¿G:ò7sO¤|äeá]™?mÛå›åìŠÍ)ÐVÖÄhP|Ää·¼Ua9¾ ùzã¯Û©  ‘èÐ§ ÿ1ðªáFTÏ›`ô¡¸¼w‘¡¢žô“‹M%evâ¬“"%#5>™#EÉçb	 -’j8çô`ÓìÐ®³×Lí*ñ §2@Su»æý8¤¶ô§45A†Íà–èÒ0VãÝrsR*…÷VOÄÏ…ç–çÔ¡iÖ7(U™²Ú¼˜GV«û‘ßšÏ/V4DäMÝ‰“ÝwÄ™O%T
³;ª8-«¦F2,ß2ôtRnžšþSó-D;µ¥ì{ï$:Aôà#6dÝo à²i¼Oi&·dv$…äº&Â1?º ¼çÇ’‹þBCw&FW_»3Ÿ^éÆòç÷¼0*û£dNXüÃ}kU§An—Óh‡¤UÐßÎ¨ut{†q€tA—p¸z¡§Je³.ÎTø7ñ†ªèqÂàDÏ²o¨þ°×Þ®Üeï¡{¾ïµãî•Ô†K“‰Èn)Öô~ ˜í…¼šu¸·.‚È”)é„éŒù§´™‚dk™q¨ûPÁÀ¡ï½ÏIÈk…e°ŸÑmøÄ*ÊQˆH{ªD*€ÓasÀ;:í)`ÙŒq¯Ž¢šú­†âÁ¾G{™«÷Ü„œñ`´ÊŸÉVÜ‘ uè}œ’Gý²3Ý”Æ ép¡âx“'”ùÐòÍSs»ÙÃãÿAÃ7ƒ”v‹¬Ç:òkÈ6cmXÊøÃ$£ôN¾Ü«–x“ëIãgŠ:#¾N_è$ò\Nèd°t`÷=ºÛÁfª¹îo½0ª¿J §‡OÕx;»ª®«ÙWÑ€ qƒ‚ÜÝ÷h.ÂÝ3r,»ùbù½·×ª¼†‘OúrYM\}|œÂ"b±“[5FlIV	ôý[¥G»®tÄF­e üEßµh]¨ôõ½¶ODÎè9þD6Ã6'ºÛ,|¿ô’ºZPñÇï¯]BÙ76Š¦2œè.ˆmd¸Â‹	¦!‰‘Ú1ÞøÆ1­j†Týq~ðæ:ß{$Ÿ—ÊêQ5¨¬ü3±‚ý0Õo·*O=«ŠÝâ{yÌT ÈûÈ«\Ž`Ô]~²šFPÙ]Z"ÜÞ¦
—‡¯Û%­“œ«ôI¸3jh Ð¿1§g¡gD¿NÕÝ¡‚©µr3ÛÓyõ·Ëƒ8	ït‘q$aô ©`¥¡iÕ×ëq%×âzMê,0HˆÑ'¤³}FÂ0Y©™Z¹PŒ1!"‘U÷”y“ü‚éWNH	FbŽÔ	ž¶aûò“¸¯SñÜPßÿ‘Kó‡FÒ‡N{wãtv5Ó Ûuè/tÊ­®±ìHNü1’Eîê‹¬½‚?„.ËORñv¥ŠTÎtDW4IõˆâñsåÓJÈ6á¢\«ó&Ž:ªtÇ6+ÏMWÃ_¾™/XbR—ï)xªk½ñ¯|u+OØü¬ýû:çåž³»]UÍÃÏkýD-·§ˆûÅ’ñ<FCž£ÅCq¨Ø×»»¢<ï»‹¢ÑW«´u9>ÍáÝNlré5¼JëVzÀL«ýÆ®r0¶íÌ›`¢¶6/Æ&ÝÙóå¥Ÿp†ˆ†¹ÁDäÙ! ÂNBlŸ¬PµÃ"Ü‹ÙŒá[Ñ,yuU{°L,Ÿd[j›œ3|×ï={cN-Ô1ÚCÖ9d’¦Ït€'ºë4øÉb–)I}XÿÊ½«£DÖ
PÈ
¬èèb‘þŽJ“öÈ¢Ó+±©°PøYIÆ’@ã„šfy'ÈV°ÀpÈ¨ÆœÐìBOrx…}³KJ…¥ú{i©›/ûª§}]!Ûž™X¶‹pÌœôtÆÍtÔÁÿ*0_ Îë#š#Ý%˜wÂS¢ðQ…} G¢DÿÜrº»dêªáDR\*nB:soÉc÷Èçn’ó)m/ä5Á'r[¬ÃüÿQqÌ…œû`6ÒW{nh­ÛQ¯3åô/ôñåw‡K|Iâ»ý¶ºWÔ%Ÿi|–rÝÒ{ °òš1ÔË"0/àÑ“¢¹]ÖŸmÌ	ÌPÍœ¥H
k0ÝvÃ@ãÿ :(æÜ¥v§Ì¤ÀrêÛw7Þ„// SPDX-License-Identifier: GPL-2.0-or-later
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