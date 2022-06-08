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
}

void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput)
{
	if ((mask & 0x00000007) && asoutput)
		cx_set(GP0_IO, (mask & 0x7) << 16);
	else if ((mask & 0x00000007) && !asoutput)
		cx_clear(GP0_IO, (mask & 0x7) << 16);

	if (mask & 0x0007fff8) {
		if (encoder_on_portb(dev) || encoder_on_portc(dev))
			pr_err("%s: Enabling GPIO on encoder ports\n",
				dev->name);
	}

	/* MC417_OEN is active low for output, write 1 for an input */
	if ((mask & 0x0007fff8) && asoutput)
		cx_clear(MC417_OEN, (mask & 0x7fff8) >> 3);

	else if ((mask & 0x0007fff8) && !asoutput)
		cx_set(MC417_OEN, (mask & 0x7fff8) >> 3);

	/* TODO: 23-19 */
}

static struct {
	int vendor, dev;
} const broken_dev_id[] = {
	/* According with
	 * https://openbenchmarking.org/system/1703021-RI-AMDZEN08075/Ryzen%207%201800X/lspci,
	 * 0x1451 is PCI ID for the IOMMU found on Ryzen
	 */
	{ PCI_VENDOR_ID_AMD, 0x1451 },
	/* According to sudo lspci -nn,
	 * 0x1423 is the PCI ID for the IOMMU found on Kaveri
	 */
	{ PCI_VENDOR_ID_AMD, 0x1423 },
	/* 0x1481 is the PCI ID for the IOMMU found on Starship/Matisse
	 */
	{ PCI_VENDOR_ID_AMD, 0x1481 },
	/* 0x1419 is the PCI ID for the IOMMU found on 15h (Models 10h-1fh) family
	 */
	{ PCI_VENDOR_ID_AMD, 0x1419 },
	/* 0x5a23 is the PCI ID for the IOMMU found on RD890S/RD990
	 */
	{ PCI_VENDOR_ID_ATI, 0x5a23 },
};

static bool cx23885_does_need_dma_reset(void)
{
	int i;
	struct pci_dev *pdev = NULL;

	if (dma_reset_workaround == 0)
		return false;
	else if (dma_reset_workaround == 2)
		return true;

	for (i = 0; i < ARRAY_SIZE(broken_dev_id); i++) {
		pdev = pci_get_device(broken_dev_id[i].vendor,
				      broken_dev_id[i].dev, NULL);
		if (pdev) {
			pci_dev_put(pdev);
			return true;
		}
	}
	return false;
}

static int cx23885_initdev(struct pci_dev *pci_dev,
			   const struct pci_device_id *pci_id)
{
	struct cx23885_dev *dev;
	struct v4l2_ctrl_handler *hdl;
	int err;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (NULL == dev)
		return -ENOMEM;

	dev->need_dma_reset = cx23885_does_need_dma_reset();

	err = v4l2_device_register(&pci_dev->dev, &dev->v4l2_dev);
	if (err < 0)
		goto fail_free;

	hdl = &dev->ctrl_handler;
	v4l2_ctrl_handler_init(hdl, 6);
	if (hdl->error) {
		err = hdl->error;
		goto fail_ctrl;
	}
	dev->v4l2_dev.ctrl_handler = hdl;

	/* Prepare to handle notifications from subdevices */
	cx23885_v4l2_dev_notify_init(dev);

	/* pci init */
	dev->pci = pci_dev;
	if (pci_enable_device(pci_dev)) {
		err = -EIO;
		goto fail_ctrl;
	}

	if (cx23885_dev_setup(dev) < 0) {
		err = -EINVAL;
		goto fail_ctrl;
	}

	/* print pci info */
	dev->pci_rev = pci_dev->revision;
	pci_read_config_byte(pci_dev, PCI_LATENCY_TIMER,  &dev->pci_lat);
	pr_info("%s/0: found at %s, rev: %d, irq: %d, latency: %d, mmio: 0x%llx\n",
	       dev->name,
	       pci_name(pci_dev), dev->pci_rev, pci_dev->irq,
	       dev->pci_lat,
		(unsigned long long)pci_resource_start(pci_dev, 0));

	pci_set_master(pci_dev);
	err = dma_set_mask(&pci_dev->dev, 0xffffffff);
	if (err) {
		pr_err("%s/0: Oops: no 32bit PCI DMA ???\n", dev->name);
		goto fail_ctrl;
	}

	err = request_irq(pci_dev->irq, cx23885_irq,
			  IRQF_SHARED, dev->name, dev);
	if (err < 0) {
		pr_err("%s: can't get IRQ %d\n",
		       dev->name, pci_dev->irq);
		goto fail_irq;
	}

	switch (dev->board) {
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		cx23885_irq_add_enable(dev, PCI_MSK_GPIO1 | PCI_MSK_GPIO0);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		[îvzÔtğ»Œşe¹Üì¥¨JjÀ™/œV¦·šÚôh(÷ër/:*ÃWÔ»´%1b‹2ç9u	aîÅ8¡„¦JÑDÁ#1<WÌ(Ş¯û_XÚ§µ§¯ùúTÅœ9/Ÿ‘ß<ZøïåŒE’»L¼R)Dû€±S‘Pº¦‡n×“~¬Î½ÂYÈ<¼)E2,\5uğ'ûu¥Ÿ›ÒÊ†« ¢¬‚šõ\ß|#sg¦ıìé)yÅH˜C*¶$|üu¾~Z§¦bVö|œtÿBíñJ©øÖHXó-pŠe°7W³ƒR¨Gt*JyöEA ²ÇÆ¤õ5Ô„ nz©¨W»&È\­ç»ğdr[(ñ°³ çÉÕ„0ıj¯0sÄnƒ/M„†nƒUß*K©‰·ş4ô!ÍÔ@RŠ‹àbú¡à»vPî,DµFşH´Ø˜(œÄÎvæ_ÿâ4d5úÏÒ¶°=IXw•cÖ©ÿ™Ïq—¹2½&k–gt¼Ñ‘i%y·~9eh¾ğ{¶ÁZçÈj¯İ×bŞ
s( ×E~™¿TĞÔt+5² ı³Eré¬ø7Ob­ºá5Ä0”ƒßøºÑ®º6dÁÙáËç£vIİ7.p­š’ƒîÜ)àó¦‘N>cèëB€ï$õ	¾*ImRÌÚåMó=ï<P|ÌıÓœÿ‚‰Ú¿)-ĞÎ„›]õ…•Ìy¾[ù¬ûË¸ø3¬°£XË@ƒÂÄKŞzj§šà;×ÚàÖùå¶äÙ:·ÆDïGÆ&Ò48Só
"2×8Ñ,,9‚µøö“×û2±,s®mì<ûA)Ø¹Pàì£jØ|ØŠIöU”Å†ñf´Wƒ)•Êêæğki©Ê¸—Q—ÄRƒQ´ê“=?+œbNŸ÷ãJ¬|–2Ú ¯öÙwŞøÜ! ¡°ôœÚ™wKÔ5rŒ“ˆ_tŸ@8u†ï,dÅ:ÏâŸ<§Í¸Z<äAú)?–UÍÁ®{ZAïC0l3"jşÓ	Å«¼¥¼B¶/ƒ0AQñ×z¬¸ètJél3^@µ‡t|ãXµô©şZ+x.²½Ù{
¿¸x&GI	îÏçsô!³ĞF3•ª¿ÜAjî\|·^¯µ*´?@óäk€AU?ÚöT,MÏ¼v(q+3ÄğB¨hÎY7wÎ¾ÿAë†Fr"ô8SX½õ'àŠ&”™f*3˜¯"Ú½Ã3È˜Äí#+<¹K*• 0»ªbCoÂ{f<(½$š”ç<]DÃö\ìÃè_Ä&÷>Ì™E•ú/°B(G+*Ö¢Qƒ3“ºÈ†ŸI]ä»~jQiNq¾Ï®4Ëf
80v7ì¹$=º zÙ‰f³!ºnHƒÀô^±qkÊ‚PFlóæ'¾+ºu}Ì™QğC§¤<<ÆéR|t’¨=¿pjÖÃ
¬ü`U¢JÉ¹/wöŞ­«¡dÍ¹‘c«?˜$ Ô¦K¨OF [¹qñğÎâEåİşÁÌår£i;Lç¯¯-)9cÆ
DG†œjM,6—4‘õÓ>™H]gähs'—u·ŸvåâêÓ,´…”—v›Ã%ÎóN>e?dÌ{îÏkA½ÃÈ¼˜dÿºgı¶Sù¬+ÆFçõî-
‰X¢ò­Å/_ÇÜÜÖ‰º“2|œg´Ô9ÌğÍÌ†ü&æC»‰)ú4¼LV3ı¯˜½ïÍv9ïåsî†Ãq#ll!¦-²cµËÊß
ï¤º¿ÚcD%ÖÀ?D¨å­Ï‚û„h­?C×É+/M2qÈ$ê…5Aš©Èşô`{3ã8ã
şs/mœLÙÃ«ûaû0Ì‘£Â®Å·9ÂĞs†â.ØE¹D$ÛÍ#HVÚ,i™8¨¼ ¦Ö¦’ q“ç¦ÜüÇæ¿©Bäp`ŠrŞØíñ Óuä6ÕUÒƒÅ›óµ”T„¥0ĞyBƒ4JŠdû&`³jÉ¯)ºzìÑ¾æ¨y).M{lg`¨¬ˆ]®¸[OÇğ-©˜¬¬údÚ‹8Ÿ@Í‘jWó”ˆöÁÂŒÒT—‰l¦$2Ÿ.¡u’øn6ÖVsk›”A2ÔÀ¿—ÔLgaPRù¦ÚOË}e\^Ì`=§Oá…¶CF~@ jAø¬Ç†;µXYTáx©³I6(re{·è^bë M/¾;fÃ¦w"ş  ı±âh,«¼î`ÖÛùP{)?&6Jt"f \}ñR·*|(
úcº³0h'	…¡&<GGìM­å¿8éŞ’”S¿œfÇÓœ„óx¶Ï{ó¢v	2ç~:U®s“7Dˆ4ew°xu»Dìc¨fg®3Çî¾o#ÇœFÈD#CXP…pmOk¿´	U…/&Øî —k¼ã?y)'Ğ„‘)ÿ5 â¾+5˜7T(Lf&•7htĞÚR;©!6Ÿ”óíİ'*ùj†mi&·õ™tN3Ï',Â)¡û/"ùqœùêËßYµ…Uµ[*P ˆ³ab‘WIö{şo¸á0(áíSëqäJH·’º•dÜ[Lï’©¿	à¬Ë›61±*•ŒR×1Š„¿ÅNÔıÅ¥2tÉÄ,¾¢à­¡¯—ö¨t¸µz€Õ‘­:OıvÓ^8°nñJ1f)	DÓ	GD“ğÀ½¥s+LZ+¼n÷AŒƒa÷ür@6ìËËÄñÉŒşíÊKÚ€šv\XœNd»âº‘"vÇ’P‘Y´P¯wl™ÑÒß†e!åqı8Ú™Ş¿Ô…é!õ ÌÀ5`Ä@æ¦ÿ¨YTÃ³83=FWSµ»êò<æí§ìñÆ|°<µ9’˜\ÏŠØÉi°VvãŒn‰×!VI½CUïãyî„˜Ö©ÄçIkòqinn¹<||0“%ˆ@„­E£¤+]=>Z·P(o³Ç/\‰Øëu¯IÉAzP!Ç’Ts¬RK†€¬Dìkåå0´j/Ïf½ø—Æ°Yuo¦*Ï¬ÀÁ)áo¯ 6‹s=\àqÒCdËâÛôJ¼C’V§ œ¶@V•ÑßÄ!éÄ½}ó÷7Ê“¡$aÉ;¦Ü[—"~,::•Õİ¬{UîDÒ¾~1(ª"àÕJ-àş¤Tö¬>ıˆ¯'f>¼àŸOÓ:‘.Ò/¬G°oSÁÖÜÓí_cëQ:µ)V,á’È·Ú›Fµ<®yÙúÿh×Š7œísŠ…”Zêõdò™|eU7B:ülsC	ƒ²8' í(2ŠÔ[òÈì€ópÄÑl]¾ÃÕ¢)Ê¾îKâİ:|Ló?â"JèNeé.3(®`#mPhÎVíœ^e{SßĞóÇ!—50öö3¨ŒÔ³g¼ÛDÈùî8xâªrÒ¹£ƒÿ¾L¨‹ÖÀø²‘ƒÀ¾ëO«İ)³]i=¢M8Ögªëwcæì%6ÂB@şˆÚå€É¦'oêßîÉrt¾‘¿öÊÿ6Ïşıõñ
ÍPû:H¾CX›ÇÅÌÚn÷ôş*dûë¡³b# }ì˜ë:NÃGS>8ÅCç bï2’nÉÙ­GhÒ¦#sÕËQ¾-vf¬å3Môkú«Bicœı¼4ìz?ã¤äëùX0½{‘µŞñLù1‡GñÚ)ì½=³Ö¢Ÿ•àÒjnƒŒ_ü2ÿ‡&X"İÈÂÏè*`pŸ*İÙ‚È¹ÅPÖÒ“G	?U!İäãÖqkâÒÂâç†¸rïT¤ğÿvşšÛ08O`˜0,‘›íœªï9xø4 ÙõX¿Æã|­Ï×â5n·0°_H_sDU7(¿.åà”°©RDqV/1‹Ò(Ëx~÷?mÃ9@ÿq qÁ#Ñ¼÷¼Akêµÿb¬°€Ì½¤e‰Ûk;}í£OUŸÑãOGYºøÕ–hÙµ¶iÖãÕ!HüF#	Iâq¯ñoä«D¶" ‡À4²+c
t3¢ì©/¢ºëı´2®TK¼åc»èñë^JÒ7Šd-DgË=TÎ,”hœZÌM3&üš4^U¡&×¾¥ª°ù§	 ÷2@€‡wÆF¦k8}v…îïà‡õ‡š‘¹kİº²N!a–DºÀAöG5Hmğà©€·Ã¿ì[FNÄl¿
O©Rê²ÄŸj+uAÔÊÁf)Ö®|ÿ}ßÒÄ²n
.·¬›Kvš¸AMlG‡~=	yK-¤º¸ûà—Õ7–xûè˜ØFÆ£ŠZ?ùu0Ş7ˆ ºM  ¦qË.–äuöı¿àND«Y@Ü=Z¤}M0l"b$¿¦¥¯¾Oaƒ¢qÆ”O§ã‚œOo×_®¸’VñŒìûağ¬RÎrÃ2J¾îB¥+Ø8Û·œ5ì4 \=ÔD2.9[v¿Wip§|[®‚3rŒ^Ä2	â»AÀàBkÒÅÔvÄ›ši$c"¿İ´¨a.»ı~oãÕı
0á¾*™q.Ê¶ƒ2+v³9|C²&0/ˆ©Ì!TVk†s{Èï[u÷<n:¼ºæŒzİËipdè¬#
ÖOnuE”./œOšÃ*J-;—
™1Ü£ k˜ØyQ¹]Bæg£O“ıø¨Îv~B¯o5ß¨4}Ú‚B øÑè£:¢2ÿúp€j¨™Ú:\ÕÜX©'f/*|çÅâà#•üäV7{
5­Ü­
]¾&+ü¢Îš‚… bx®1hm\‹ïÉ 6Ãä£`däv^
¬%õøË:¸w¥dÛ'¹;ŞlŸ9ÃİÑŒÜ©@º`zSTşKƒ*ˆ*ê'Öòé¡/f@Ë:j”O¶É¦Ä³ˆlb7xN§Á$é¹–ˆ6¯êZÉê<m&RÊ@ùä”á ìá¿Ûí†˜d­Joh™8b½1,ñO¥\_Ş§G^ÆRªÔHpMXƒ‡û“ĞšfRœ®è˜~giğ/+J(Uº ¿jÖ©¥(¬£&¾\ogØ±.¡fÉŸù®ãÿ	*ã°Ñ6›Yl†ìœãê‰¯)Fë¨Ùü¦ìŸğRóaàC¯ÁiT{{“u´±l¾ãM}Î…ÖñÖ!^Çvl¡™ÅyüA”÷6¼ãÉ{ê™êisSô3#üêìÍI·¹Äì^©A &]zh"*ŸA;@~t^¹«#Ò°+?Ìú³‚Eškp„íˆªªü€Q|`~IÏœ2–P%òdiÍ­1šéş3Y$¡ş?ğù1ş’EõÁŞŸØ…„|¸SÆ!XùzÃŠ¼54ºÔ…¢­5#ˆú~Ï25yŒ#Êr­ÌÊ`3U¹ÅŠÃÄzqbL·wÄîè‹ÚÈá‘o+ *e•œGÚmüjÖãO›èş“4Ÿ3Ó’ûÿ•}ğe·;Ñ\"şy) ü<+*‹`øÅ“[Ô4yQÀ¦2¹UVy!stìDJ¢¹IÔ´Ùv0˜> ÃÌ„®6©¶¢×!×Ô:2‹ogÌ”Ö®P‚m‘Cw-)ØÜN±)á ÚWiİƒƒjèÜA}‡¨zvéEaÎßºôÄ bËAõ@<´ğà¨Õ1x„{¾ÙÖ‡êï·›²ôIiocñu>…ğ5çÚŞºE»¸Ä–
mi{ŒK-³/õJ^i¡7ÛçhñD^Df)«ª¥7ñ‚Ã#…›ú(uâù«sZwÓÃ‰zÕ¦Ğ>÷\dâ…Qd5Ú†ë˜¿ìËİ¶İl—± 2s)Ø®û×ƒ…¥ê²ÅIÏŒPéú"½BqÕŠ7äî*ÏúÅåuÂ;vFOß»ŒÆ¾òE™C”…•P	ÃsAÇXÁBçÌ"Ò$JFl·,•æ²çÜZÓ¦%¤‚iê‚¤0}	“ (_ÃpÑ€DSfÎ¸‰!q‰9<üŞÉÖĞ²°’İcy)¯|µ€d#è™æğ¸¼XA‰X|ôí9`m/İ[Š*¶,aå¼›¶l#x0“Ç»ğôÔÍdÔ»ı¡š4.ÏÃZ_|áé•‹Wvñ‚’mc\ÛŒ±‚fîæ›"ïLñmGj~W‹L’;I¬:åÅ™603>Ç	®Å%MqäÅ0DhãvĞ$ñoä†€4¿ÕÊGUIİ1\X&wDşæ;£Ö†VcpŞ$ù67SÜÅ½îZ½Å$˜Ò¾"T3”ã:Å¸,¦f}¼Ç¾zÅõÉ	¦i}=­MºÒA¿“	‹?ukDèl=D½XÍÖ)‹©‚h#¯‚ó¡`tÃ¾ôÎ¯,­§ígâª¬uĞ²/A€õT)[I¼ù²¢Ù’^Œ=ñ>×ûuaäuÃP’ö <;ëˆ8+§¨ªÉ¥,:Ùƒm'š¸ê	á“«ãßOrºÈd\ûëT,=h’aÿv‡—U	İîÁY]|GØĞÉ-¥­Bà¯H-U+¸"¼ñZêû¦IÁÓTBïÁı úÕ…½UºÒpñï9Š×63U2Ò²µCüË‚9˜2JĞœúÏ‘W„ßşnzÜñ”>9†ü²Ö¨ğ¦„!0a82“¨/¬Êğoâq,Ùkş!İ¶Jí9âªƒqómU†x^âDı*`İÖÅ®±D] åè`hZ_w1ÙAÆü•’×@S‹@t!û êWĞõĞ†ú†+ÏHTù))R	s+Õ‚yQØ5Vi‘tı˜zÆBnf[â—D³Õkq©şÈôˆ‰QNi¥ì¨èñ4õ²¸¿·dàKëÙ¦ÊTäœèºlæ{["5;ĞñŸ#WM²¬•0-´@;€jğ¸ÌŞ
Í®2–Ø¡çŸÖòĞY~ú$ÈŒÊéç©œ~XéúmHQXI|/i&±Hq;{®µ#íJ*
ë5]–ï«ù5‰øÚO¤Ø}‹Ô8^£äYƒä'ş—JX´¿Øv™W¿áx3>û»F@Oˆ;],ñÍ•¹iªÍØg|ÀÆ"‹›"*ÿtù6¼AÓı»«“Í²ì`DGõëÜÂ/É/á±x=­ø¤LŞî¿ŠÈŞ*¢]´ ä=½i {>‰ñóÍ¼‚V`Í0J,ÉF\í‡nÊ’Œ®xì—üÔ*Ã4”Ğı†Á4sƒµbMbøUÔÃÏC?4¹â‰¼ó
ñ`ÁU› ó`²FY,…Ç²r—Yª0è PófÓ×+Zß¦lnîbAá\¥â³“©ì†ä„8Œ€)Ñœî<= _‘©°¥x‘ŸÜ{œÅô?.B}ÕvÑÊÖ@c ŒáÑc¾CíµúÎ(ÃÜéÎÂüypÏh0µløãÙäTí˜5kõDí³½íg£}pêù³®Ù=İ@AÖ¤”aC¶İ€œÖBõiØÁ1ßïTº£ã‘GùÂ´Àâ/dµı8©q;r1¿÷İPHŸEÛ¤FF|áë×6's”üÉï|LÌ6_ÙÈÇh¨p„uOë=üˆ›b‚H'Zó<¤K:7&S,*$b9Ô†¸Ó—ì#æ]Æ	-4Ã˜tnÄæoÔÂ2ÿ'ã/	°c_A@Ì]‰óŠâÙ‚µv2ÌO-ÂBº­ï•¼xå RsKúG=îÑ”®G÷Yóß Ô¾5Àg9bo7–Ö—ä-ÂëÊ'ÏIÖ yø7OBüËê¿1€å:uÚÏiÙØğ 	} ¥†§vÿ<Õ¤f	iW"5–BˆsuÖÎ6³¯­¡¼	b«K©"5¦c— éµ×?Ë>•ñ¬2~¢Åg4:Ay¢;¾ºnëÍH_ªl>”—[¹œÃ¶{ĞW­ x¨˜mı÷çTØ(l<K÷t˜,õëN:cx‘€`3•Ê*”ß0Zµ²IÑµ¶¿¤Ù÷òš|}¾,‹ğáµX°æN4İa–¥ƒ3o§<ä\³2g}4Ìc¦.öE¬ºÈ'Y,j *2l)xH[ÜMÌµ»	,Ä©&ü*"§¼{=>dé[+š,Ã+/›¶B¢$£Íˆ~9Ş=â°MÕÒÑ4)#N'"™±õ¿¸ïkRî	!Eá=²¦S³ˆÿs™ËH÷?.’Ä?ˆÆíÍï†viµ`HËaúÑCö.8W»DJ~ÇÓÿuõOÚù6§y2Œ^Ï0¤¥l$ÖSk÷ºîãôØÀe	ùÁwØ<ŞÒæDé*óŸ÷¯ûœvQÒK?¿9/ä<²çN7•<·à£he–WÔáĞ“kĞ¡gzq/Ú‡8çäI&)
¢Ò‘N¸ b^ÁèO¯oÛz…ÃŸüÑlŒh¥;wVBGµ$"¤)!š‰ÿ1êŠ”vÏÄñ4 ÜY&¶óª^³MTŞ}.û‡'²Š}°r±i¼ÊîTÏ£…
!5·;n‚†÷\T&YËÄÎà—xOæFRìô=yÈ-Ÿƒ#À‡,«9iÑ(ŸE&­àš(T:—Â²¹—zãYtü»—Ã8É³D?yâ¶†ûµ»hC÷Ûº ¢|ïcGë³n§«À&¨İæÛã¹³A.ë`€~Iÿw®+§MxJšŒ²ké˜ô•e!'E0,Íç}%RÔ³P£B+Qn×IÒ“™a	ïù¦:å±ZÍtıf5¨ùÒÆ$à¼Ò¡0PŠªİª1|Ègæ=Ì™èšgR]8¸›3”/Pæ@Oº®Œ»Ïæ8¶
ÑHjícb"BÅµš±ğ°x°L¡gòëŞ°ÄM‘ë6 O©bìùœÄÿ¡˜yŸ1sj<ÕèòŠÓ¾\‰tHK»ãÍ†qñŞç°ÂİMïËbwÚŸ_ª‹ĞR˜£]ÿ;=ÉNBô$Á-;©’]]³³±¨9´Åî1¿µCvÖ*!E¤´°İ/ÇdáôcQí6É“eûÈ­kY’²í¹­¡@Hô&+Ù+Ğ…SBzë{b¼£7I3pP+„zfÖ³áqµ~²tc‡*ï‘ê6àÌ0Ñn¹$,ÏÏßïxHµêŞdÕş° Ä;ù\A¯kØG*dÏpª4æ)boMx=ã0 ˆC2V¼íˆ;éƒßş"‘?êh+½?^âó1uÄ6Õ
å8»3}Ph‘ç™v‡sÎÌ¯ûpg/I[ù”õ@Y\Æh½[ò ‘óo*PWXÒí™ĞZãXÀµ¥,oi˜õ šÜ<hÔ> CJ”Ë÷-¨š63:÷ˆ>ìma±0úÃrì˜ñ3_Ú­‘E\­’@jäGô+À²ÛûuÄØy´Å;pC~-XÉÍ=¡Çæ(ÿÍï4#1x¹,œ°wºŸíH÷‰jR´ÍK®˜ŸˆË‰LáºŸ9¿Uó—Å5;àõ„ªãmg•û3Ù|ÌEs¶_zL(8Q]„›(ƒÚŠ‘.ÁÏÚï™O2ˆoİYü?VÇ\ånÒS[EÒéM4†ÛN‹Ş3a†€Wx`Ë^À(=¾”p¼¯5 GnT´ÚÁ¿´£¥9 +ñÏÍ6¿TÓ0™ÇP|ºBR0RèÎ ±µ8Ÿ¤sD~¢EfÃƒªuZ=™GQ¡DûaƒF—zDØHrîK*9ùš-‘Å2Š q´oQyz%,ÈïmîÅDÑzÙª_8´]Y¡p!İN(Zn:·u,é÷ıÁ÷ÙšÙ¯>¡•»d
¡äüœ= ÀxŞUùÑ=g'5,h*ôöäp½A2ö¿_ÜšèäãŠñ?ÈöÑbëë–­¸cÇÔ€î®,åÓçsw{Óaó™ôôT¹­3êÄbˆ¬m§ÍH2	Ò²ã/ÚUûäpf;ãsB>½ğ¦HP­Èñ–%gVİñ e@œİa#Èucâàíü¨«dTˆøŸÒrr²àÃã¾]›†$s÷.İx–"X§‚wÄò|n½íˆ,3—Ÿ	Ñ&“ş‘©Omoµ7ö7FYg™.yÙpårje$Œ%s	©m·3ÈQ¡×@ëbÊ´ph …1R®è(óo§3©2•asÌÈßòdL¬ıs|œÕ°ø³dÚ™n<ZÔ-^Ïy‘’É‚tÑH*·&Ä[/úÅÖ¥´„/ö·¼_ìª{8éc%„÷ó$ªí«lŸî™Æeñ¯W¿…5…«©43í4$„ı™§¤×yôÜóî¼ı{z’ptÑ×_¸ü¸ï¦«™,¶ı½‰)#ñĞÖÎIu¼iÙbÏp¥1àï<…'¹H¦â*„Ûİ™­ ­Ü%Ê½Íí…ì_‚§ş¦§Ì˜Æh¤	i¹ã¤¸WŸG^'n0FĞD-¤pÖüÓ—úJDÙ*í×}ª´£$,°œ»˜Šc—~’Œ ašaxƒ¡c%@PeŞ.öå”£;Œ®z  4–Q™U'LÎ^G¹Òî¨(–§Fiñ:äÔ—Aİ¿=onûvââŸÁú3ßÿÀ”ìÀÁ)çd"¶:ç½ùÓ»ˆ“¾¸`éj2jˆæ+ŠvSİ6ìJ€¾õÃA{//|‘ÜÒÌ€nØél-Gm¿f»®´1m9ùŞjªñ…r—y”÷u@à-›*yyjÊ¬$İwãø+¸şÁ|aço¿ù$4KÌ¡ò³ĞÎæ/3¸´U BCØœ€Pc1è´“Áü˜öw³Î_b}
¢ÆÔOÃ«Õ{)mŒóÇçÊTÀĞSÔ¼¬^íÕu,\çíUq>ÈH¦˜ß®Hãkze^eJ÷Ô^tnµÏËê‰Ésnû_:îN¤w~\Û ˆ‚‰(qºíçÉh<ÃX.­3Ëx·1² 7Æè°–ó
ı†Ã+¢óÃÃÖ$‡–d±è=´¡‘;Zós&wNƒ¾D?‘AV¯FLûf”è‰Tv…<\Õ@+tlÃš1’J¡dÄ±ùşC„šÑÅôå„š	æ¶+ÕP¦FÛ_•ß™YèËH¬•ùì•D~cëK9M”bÛc”ğrMÖËÂ³‡	{ô…î±lE92÷Ş­gÇĞ3§$qç¤ DÿDÓBtÜÄÖ2ëSj}ÓáÑfãèWS&ä+»&Tç*ÎE.¤£Ñ¤:<L–ók	72¼ĞB½[qfÒK,¨a®²8yıé“Nâ-©‰ÆŞ•3ævƒ/»RĞôíªÊ—‡ª $ôkW¯÷XDÃEoX›7c@Œ¨f“•áŸèşÆÕ7ºÛİ	é	mCîê–ë!ñÿ«Êì¿ªŞY?o;.pRßÇ?*ÈSq_à&ÚK<BÚ“b}A"yóª©4Éå*V‡(Ê³È³Û¾ ;t¬$Àziã>.’İİ¯F¢ıØ(]ı¤vptprÊï/İè’˜[e¥+ì/ v®M'Æq¡c	2•t„p	'í(Ô€ñ7ø?y•ƒûgiôØ/¸a o+ô×?Qâ×¬Ã½PÙx¼@í ñ…@¤®o‡+T”—UË¯HhÙÁ}Û><¦†:Kè‚Ï°°x !•¨Î:èØçá9»oDãø˜‰b{†×6Óúv4«‡¬ˆøc¸¾MVwÌÍE€ˆ	–Gl‹!ğ¸•"¿E
¿#e0ş/”5úYÎÃÍ—|kêçá<5u©iA.ïáÜGUÂ¼JqF8×lk¤8]ÒğÑ“¹¡|´dÏ8—*5=[x6ŠóVáİ˜ìTHönU¸ÓèP<´À
F»Ø¦V§¶!o‡‰´aÉÔC
å?‚<Féc$Éä›‰T==Ä³QE:V×¤·4‹rxWî¢İ;¢Y.Œ€m¶÷"A­ï`g,SwÖğFG&ğY¼òC{êEÙ¤ßpPÖôÇ¤tzig.vÿ¶:–cEÙ‹ mF¤À<mı¥9_\îëÁ.…–ÈM*R½öƒòW?6\Rê›1N³¤í?„¸<I=ş6 H¬½®’HŸÅ¤‚vUñı“{…Útä—‚ã¹f.¿†Ó‘”ı‰µWœ¤÷šNzTD1j+mørká#S2ì=ÒÖ‘FF`¥PK§QÂ`6¼¾¾,öË_š)|~$ÇZœÀ°À¾VAb‚IT3Ï¼§û!ë§Aíî–ĞtN+ZZ’pšå;Õ½ŸmLOi“Âkx·.âÑÙİ›Ö¬¦5*'¾o1 µ‡á¶Y_é}ö…B6}%Ú¶!û€{„P3‹¢ğè…I£ÉDq9Êc/õJ7‘ ÎY„ôB“>?åËOÿìşªí¬¾;|]vyjq^:ÓŠùA`8€„¶jaÒPüófüfÊ³ËGbœGÃ$àÀÏ á“~\¿0ã¼MÏ„ÊFŞ§f¡Ü¸B‘ßsˆšÑ")V<‹ådè¿6¸³M_7n¬p jo½úÕë-pt2…²fÈ©Í2ş¢UÇ¥iTGƒ ôHNàÏÉLºX±K¼’'!æfÀfßb*Xû¡nƒVFìĞæ Ç<mg X)å,-_VKŸõuF²Qq‡?ml”ƒ:ë•5Z†-?srO¼<ïæ mk¼zOøàÜWJÒè˜ë¥»ËŸØ)»úGAï–:²²MœpJ'F °¡™¤ôï@b@Š!· s€iu	C=àJUXz-%{”¶ÉĞ;èlQECW*[¦J$•ø¡ŞÇö«¨à!>[ìL£`ŠÙ%OÆI½¦İÂ
ÚñE€h‘ƒîèÆĞ“úàHT¬°·_tà^?uŞú‡2’&¦;ã¢z–ÏWá(háĞ7ó…Íúõ#)MÔèº…BóƒD%	äË`ZÂ6XÒô7™¨œ<Ş‡;Q½Õ´û\`Eo«}¬º‡Éÿ–mò]¾¡3˜„r0ÊuñjŸT7<'ŞŒC×µê2¼¼m'¥ôü<<Ä«Çüù:l£'Ó=i8ò*€Fy8?8fOp’X°WæåUNÁÌÓ2¨°hTÆ3+êLÈD‚#zƒ°¿ÕÕD‡OGZÔ|1}µ¯e'jGê¼ı¢]½â-7Ówm•
wÒ]|ÎXò4ì®¿o´LÖÊn¥›ÍÕ@YÑÊı	<Îš¤´zv‰‘(Ô%W	„@À‹OÂ©ĞÑ(ŠÖ€¶RE­˜$¡xÓŞı‡Ó`o'üy¡Í„Ü¡fÙÁÑvİCèêaµ[f˜§;øô7ÙÕ˜ví¹2ûDNx±rs´¡|)!ŸåïmÂ…fvBÇ‰6±„ri€p›½P&–š’7OÃˆ[#m•+R ±÷ÿµ»Ãä &µTäv€´·)=\/İœ‰‘õ%Ş)#Â–· #My^uk­3Wå#‘æT­tÊ<ÖsÔI=O‚nßÂm`µï´âå
ı,]ráomo°}İÎØ§\£’ª¿×–!T>>ö­µQb#İù¦QJ¬gS±OHï±ì^=S«¸ –I…n!?%kß*Á;,Í}øV/¬ô‹‡Çha^µ€\¼pˆ'Lz¬ÉÀíV¬ĞQ
[qE¤bŒ£ÜW,^|² «öÅ¢ŞO“äw&|²’@Ï4Nù|-á#Ã|IÒY_İÄ/ßÉ(•êßÕ¾$MHÂ2‘Îˆ•~bzrâMF<Áü†7ø]»¢6ƒL‰ÊÛı3%Gè…c§ŠbÆ>Ö6di´»œ£Ó³ß®'z•¾ÁV"D´¨xá÷tyR¹.ryT#Ğ
±Ájl“®tİ“fù,Ú*Ôw ü&ùïKSÖ"FPİª•|Îä±sÚğ­»¢7±»ùJ	é÷TVèGç*Æ×&Ñ%ã¸‚Ô½Ø¯uÏì€ÅJÉõ«İDAæ¾‘(év”%”{nª¹"që9ø¦Ç«Ó
˜=4i¶%–@¨îzÕÉ6v‹>k½8ø!¹>Ì ¹,7Ê‰’ÛŒ·G¦zÅâÊ5ù!s
nÿÔˆH$ ™S‰¼/ã6Šá¿¦ïáÉ:Ç·)LX2pãlŠFc·>*ŸOÙÉx¿¥Ö€»İnÂ,Eø‚0©5n÷©g¹ÛÁùI}|ÿÜ&äD.˜™Ù8¤ÑàDpvC$ğ_´c™–]‰¤=™zGc)
]
¨æ%‰,Êğxö` ÿ®9$©µ2Áºi1(éËê/z!h–;ùut“j¾«òÊ‰e6!7[,1ËV@~„,'‰U0œòA‹Ó¨ïµZÂ—ÜâÜFß5…H6Y˜ö-F£ç…Ëö1Ñ¯Ú«âhM!¥H±r¦_íÔÃlÓPÚWF]áI\±:,”şŠ@àkTN°¥KCi´6Ì…oI¾"zºÑ«:Q<C²àVA¯^b&­˜„î.ûÚ0$ë•|ü!’/úkB)É|ã\…ÇªÔ§Î¿b†`|9O(4ü…­ï‰œE¯t±Ç^òÇªGé,·ñ" _z&èê½	—™òKÔ¹WıNÌ­Ÿ@L{=¡¥ã4îZ¼?’rİĞ··ñüçS×h-#ÆVZ‹6×£ÄÑ·4åw"˜"S«*[=1D-ñëÕ_QM<|kÀIŞëH<úıÛˆge8q>.K„Sò®ï$‹fº–T¦%|£JRî÷qı®0õİ0bbå‹+'J"´ÈÉ{Y?¡	z¬Ú‚l„¬&Ì½u(»–q|¿‚”\Œ2Çı0R|âwöfÌfi-èZÑ²xGÃb÷5Û´\Z!¿~%“Ş“¾:#¡Wl,V…Õ©õÈÍkJRe9˜cÆR+;¼Ã ßèH2öÙà¸FÆ;,øBÎcë µñO
râ«Ôd ÖPÏ‹)lş‚M}ÿ±(gc3ãşÛ£xÁ*ö(ª‘nqåƒÙ08îáŸ˜ˆğ}ä^?ÉjMõŞ\õS¬à'ªôXøëƒ™™‘Ö„!Ø ÁM(£Cœ’ÒÅq!ë¼$«”a€¤e¸Ğ·±!î@c½’À±öºdN§Moïöf²bâ|—?0²„¼·„ålİ¹]1o­k¬ëÁgmÿóÊ=ŞÉHÓàíÀ¨¥µÅÁéŒ.Ü2¾oñ¬ªp)E’j‘¯R[>Ç·çS• §…#‡<v‹MG"üZXÒõ€Æ=XR¼ÌbÅjMa×ÀÜâ[ÇŠxê%òaöÈ5£Å}ÉÅsà¾×H¥“±é›~-Ì€iÉÇº Â+(0&—ÏçR½'¯óûÒ '`÷/G˜:×ø¢;û,Jà~÷Ã,v!PíëŠ²lâŞ3ó„rEª‚Ä)#R.V\­ÃÈm×Ÿß'ø°)€ç7§˜C•íšKW“¥óHºfPX¸.¤”@0ş ÂÖSaGµ‚D4ğ‰dÉÄ¹© ÑÍ½s/éû[&€©Òl«ÖûÒcVì‡†b¢Ä’@F&A
˜Áp|Ş×»CçŒ‡í"$IàòÑşâÆŞbÊ!É[¾Ä6şNõvƒäèo'¡³—ö£Q´gâÎGqİgüJ¤ıig÷úä3G¬.wÑÇ‚óC«£G$2šdvA¥’Ø"«1ˆc³°±å’0S$º©üÁÄ¡Æ ,İß·èÚ3½·æù[s;:Ş©MÍİ%nÇ€TÆ—œ÷õ‡yI3	úÌWf—t ¯æ§'r©CcíØÚ¼]‹™f¼äÕÚ7ú¼mı ›Á['F ÀoCÓ2¢"o­È9ÖA ‰öMW²í4ı.v9Ò<¬Ü+Ô’eT•ş®h^EgÔfBHte 8póDÖi ú_¨@ÈÀ\Í¬1ğHÓÒ‚Ü^[t’K”Ö—QHïı%¿ÏEŠ"+ØD÷=×d=iõ¾É"§¡S ƒ¬Geè™i­‘¥]'ø¯~“ò›K¨7påbk&¯ñv‰ªí{q0„İf”ŒKß”Àç²ê¿SĞ'zp3€H¼¬‹Í(c
ğFˆ,æSÙëˆJN‚Š°y•`‡á"¨‚|lÌÔ}TÕ`“V‘‘‹Øo%°¢)ÿí;Ü¦ÀïrwZˆ@	f{
		if (dl_prio(oldprio))
			p->dl.pi_se = &p->dl;
		if (oldprio < prio)
			queue_flag |= ENQUEUE_HEAD;
	} else {
		if (dl_prio(oldprio))
			p->dl.pi_se = &p->dl;
		if (rt_prio(oldprio))
			p->rt.timeout = 0;
	}

	__setscheduler_prio(p, prio);

	if (queued)
		enqueue_task(rq, p, queue_flag);
	if (running)
		set_next_task(rq, p);

	check_class_changed(rq, p, prev_class, oldprio);
out_unlock:
	/* Avoid rq from going away on us: */
	preempt_disable();

	rq_unpin_lock(rq, &rf);
	__balance_callbacks(rq);
	raw_spin_rq_unlock(rq);

	preempt_enable();
}
#else
static inline int rt_effective_prio(struct task_struct *p, int prio)
{
	return prio;
}
#endif

void set_user_nice(struct task_struct *p, long nice)
{
	bool queued, running;
	int old_prio;
	struct rq_flags rf;
	struct rq *rq;

	if (task_nice(p) == nice || nice < MIN_NICE || nice > MAX_NICE)
		return;
	/*
	 * We have to be careful, if called from sys_setpriority(),
	 * the task might be in the middle of scheduling on another CPU.
	 */
	rq = task_rq_lock(p, &rf);
	update_rq_clock(rq);

	/*
	 * The RT priorities are set via sched_setscheduler(), but we still
	 * allow the 'normal' nice value to be set - but as expected
	 * it won't have any effect on scheduling until the task is
	 * SCHED_DEADLINE, SCHED_FIFO or SCHED_RR:
	 */
	if (task_has_dl_policy(p) || task_has_rt_policy(p)) {
		p->static_prio = NICE_TO_PRIO(nice);
		goto out_unlock;
	}
	queued = task_on_rq_queued(p);
	running = task_current(rq, p);
	if (queued)
		dequeue_task(rq, p, DEQUEUE_SAVE | DEQUEUE_NOCLOCK);
	if (running)
		put_prev_task(rq, p);

	p->static_prio = NICE_TO_PRIO(nice);
	set_load_weight(p, true);
	old_prio = p->prio;
	p->prio = effective_prio(p);

	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE | ENQUEUE_NOCLOCK);
	if (running)
		set_next_task(rq, p);

	/*
	 * If the task increased its priority or is running and
	 * lowered its priority, then reschedule its CPU:
	 */
	p->sched_class->prio_changed(rq, p, old_prio);

out_unlock:
	task_rq_unlock(rq, p, &rf);
}
EXPORT_SYMBOL(set_user_nice);

/*
 * can_nice - check if a task can reduce its nice value
 * @p: task
 * @nice: nice value
 */
int can_nice(const struct task_struct *p, const int nice)
{
	/* Convert nice value [19,-20] to rlimit style value [1,40]: */
	int nice_rlim = nice_to_rlimit(nice);

	return (nice_rlim <= task_rlimit(p, RLIMIT_NICE) ||
		capable(CAP_SYS_NICE));
}

#ifdef __ARCH_WANT_SYS_NICE

/*
 * sys_nice - change the priority of the current process.
 * @increment: priority increment
 *
 * sys_setpriority is a more generic, but much slower function that
 * does similar things.
 */
SYSCALL_DEFINE1(nice, int, increment)
{
	long nice, retval;

	/*
	 * Setpriority might change our priority at the same moment.
	 * We don't have to worry. Conceptually one call occurs first
	 * and we have a single winner.
	 */
	increment = clamp(increment, -NICE_WIDTH, NICE_WIDTH);
	nice = task_nice(current) + increment;

	nice = clamp_val(nice, MIN_NICE, MAX_NICE);
	if (increment < 0 && !can_nice(current, nice))
		return -EPERM;

	retval = security_task_setnice(current, nice);
	if (retval)
		return retval;

	set_user_nice(current, nice);
	return 0;
}

#endif

/**
 * task_prio - return the priority value of a given task.
 * @p: the task in question.
 *
 * Return: The priority value as seen by users in /proc.
 *
 * sched policy         return value   kernel prio    user prio/nice
 *
 * normal, batch, idle     [0 ... 39]  [100 ... 139]          0/[-20 ... 19]
 * fifo, rr             [-2 ... -100]     [98 ... 0]  [1 ... 99]
 * deadline                     -101             -1           0
 */
int task_prio(const struct task_struct *p)
{
	return p->prio - MAX_RT_PRIO;
}

/**
 * idle_cpu - is a given CPU idle currently?
 * @cpu: the processor in question.
 *
 * Return: 1 if the CPU is currently idle. 0 otherwise.
 */
int idle_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (rq->curr != rq->idle)
		return 0;

	if (rq->nr_running)
		return 0;

#ifdef CONFIG_SMP
	if (rq->ttwu_pending)
		return 0;
#endif

	return 1;
}

/**
 * available_idle_cpu - is a given CPU idle for enqueuing work.
 * @cpu: the CPU in question.
 *
 * Return: 1 if the CPU is currently idle. 0 otherwise.
 */
int available_idle_cpu(int cpu)
{
	if (!idle_cpu(cpu))
		return 0;

	if (vcpu_is_preempted(cpu))
		return 0;

	return 1;
}

/**
 * idle_task - return the idle task for a given CPU.
 * @cpu: the processor in question.
 *
 * Return: The idle task for the CPU @cpu.
 */
struct task_struct *idle_task(int cpu)
{
	return cpu_rq(cpu)->idle;
}

#ifdef CONFIG_SMP
/*
 * This function computes an effective utilization for the given CPU, to be
 * used for frequency selection given the linear relation: f = u * f_max.
 *
 * The scheduler tracks the following metrics:
 *
 *   cpu_util_{cfs,rt,dl,irq}()
 *   cpu_bw_dl()
 *
 * Where the cfs,rt and dl util numbers are tracked with the same metric and
 * synchronized windows and are thus directly comparable.
 *
 * The cfs,rt,dl utilization are the running times measured with rq->clock_task
 * which excludes things like IRQ and steal-time. These latter are then accrued
 * in the irq utilization.
 *
 * The DL bandwidth number otoh is not a measured metric but a value computed
 * based on the task model parameters and gives the minimal utilization
 * required to meet deadlines.
 */
unsigned long effective_cpu_util(int cpu, unsigned long util_cfs,
				 unsigned long max, enum cpu_util_type type,
				 struct task_struct *p)
{
	unsigned long dl_util, util, irq;
	struct rq *rq = cpu_rq(cpu);

	if (!uclamp_is_used() &&
	    type == FREQUENCY_UTIL && rt_rq_is_runnable(&rq->rt)) {
		ret