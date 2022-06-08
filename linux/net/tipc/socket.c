ze, &risc->dma,
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
		return max;
	}

	/*
	 * Early check to see if IRQ/steal time saturates the CPU, can be
	 * because of inaccuracies in how we track these -- see
	 * update_irq_load_avg().
	 */
	irq = cpu_util_irq(rq);
	if (unlikely(irq >= max))
		return max;

	/*
	 * Because the time spend on RT/DL tasks is visible as 'lost' time to
	 * CFS tasks and we use the same metric to track the effective
	 * utilization (PELT windows are synchronized) we can directly add them
	 * to obtain the CPU's actual utilization.
	 *
	 * CFS and RT utilization can be boosted or capped, depending on
	 * utilization clamp constraints requested by currently RUNNABLE
	 * tasks.
	 * When there are no CFS RUNNABLE tasks, clamps are released and
	 * frequency will be gracefully reduced with the utilization decay.
	 */
	util = util_cfs + cpu_util_rt(rq);
	if (type == FREQUENCY_UTIL)
		util = uclamp_rq_util_with(rq, util, p);

	dl_util = cpu_util_dl(rq);

	/*
	 * For frequency selection we do not make cpu_util_dl() a permanent part
	 * of this sum because we want to use cpu_bw_dl() later on, but we need
	 * to check if the CFS+RT+DL sum is saturated (ie. no idle time) such
	 * that we select f_max when there is no idle time.
	 *
	 * NOTE: numerical errors or stop class might cause us to not quite hit
	 * saturation when we should -- something for later.
	 */
	if (util + dl_util >= max)
		return max;

	/*
	 * OTOH, for energy computation we need the estimated running time, so
	 * include util_dl and ignore dl_bw.
	 */
	if (type == ENERGY_UTIL)
		util += dl_util;

	/*
	 * There is still idle time; further improve the number by using the
	 * irq metric. Because IRQ/steal time is hidden from the task clock we
	 * need to scale the task numbers:
	 *
	 *              max - irq
	 *   U' = irq + --------- * U
	 *                 max
	 */
	util = scale_irq_capacity(util, irq, max);
	util += irq;

	/*
	 * Bandwidth required by DEADLINE must always be granted while, for
	 * FAIR and RT, we use blocked utilization of IDLE CPUs as a mechanism
	 * to gracefully reduce the frequency when no tasks show up for longer
	 * periods of time.
	 *
	 * Ideally we would like to set bw_dl as min/guaranteed freq and util +
	 * bw_dl as requested freq. However, cpufreq is not yet ready for such
	 * an interface. So, we only do the latter for now.
	 */
	if (type == FREQUENCY_UTIL)
		util += cpu_bw_dl(rq);

	return min(max, util);
}

unsigned long sched_cpu_util(int cpu, unsigned long max)
{
	return effective_cpu_util(cpu, cpu_util_cfs(cpu), max,
				  ENERGY_UTIL, NULL);
}
#endif /* CONFIG_SMP */

/**
 * find_process_by_pid - find a process with a matching PID value.
 * @pid: the pid in question.
 *
 * The task of @pid, if found. %NULL otherwise.
 */
static struct task_struct *find_process_by_pid(pid_t pid)
{
	return pid ? find_task_by_vpid(pid) : current;
}

/*
 * sched_setparam() passes in -1 for its policy, to let the functions
 * it calls know not to change it.
 */
#define SETPARAM_POLICY	-1

static void __setscheduler_params(struct task_struct *p,
		const struct sched_attr *attr)
{
	int policy = attr->sched_policy;

	if (policy == SETPARAM_POLICY)
		policy = p->policy;

	p->policy = policy;

	if (dl_policy(policy))
		__setparam_dl(p, attr);
	else if (fair_policy(policy))
		p->static_prio = NICE_TO_PRIO(attr->sched_nice);

	/*
	 * __sched_setscheduler() ensures attr->sched_priority == 0 when
	 * !rt_policy. Always setting this ensures that things like
	 * getparam()/getattr() don't report silly values for !rt tasks.
	 */
	p->rt_priority = attr->sched_priority;
	p->normal_prio = normal_prio(p);
	set_load_weight(p, true);
}

/*
 * Check the target process has a UID that matches the current process's:
 */
static bool check_same_owner(struct task_struct *p)
{
	const struct cred *cred = current_cred(), *pcred;
	bool match;

	rcu_read_lock();
	pcred = __task_cred(p);
	match = (uid_eq(cred->euid, pcred->euid) ||
		 uid_eq(cred->euid, pcred->uid));
	rcu_read_unlock();
	return match;
}

static int __sched_setscheduler(struct task_struct *p,
				const struct sched_attr *attr,
				bool user, bool pi)
{
	int oldpolicy = -1, policy = attr->sched_policy;
	int retval, oldprio, newprio, queued, running;
	const struct sched_class *prev_class;
	struct callback_head *head;
	struct rq_flags rf;
	int reset_on_fork;
	int queue_flags = DEQUEUE_SAVE | DEQUEUE_MOVE | DEQUEUE_NOCLOCK;
	struct rq *rq;

	/* The pi code expects interrupts enabled */
	BUG_ON(pi && in_interrupt());
recheck:
	/* Double check policy once rq lock held: */
	if (policy < 0) {
		reset_on_fork = p->sched_reset_on_fork;
		policy = oldpolicy = p->policy;
	} else {
		reset_on_fork = !!(attr->sched_flags & SCHED_FLAG_RESET_ON_FORK);

		if (!valid_policy(policy))
			return -EINVAL;
	}

	if (attr->sched_flags & ~(SCHED_FLAG_ALL | SCHED_FLAG_SUGOV))
		return -EINVAL;

	/*
	 * Valid priorities for SCHED_FIFO and SCHED_RR are
	 * 1..MAX_RT_PRIO-1, valid priority for SCHED_NORMAL,
	 * SCHED_BATCH and SCHED_IDLE is 0.
	 */
	if (attr->sched_priority > MAX_RT_PRIO-1)
		return -EINVAL;
	if ((dl_policy(policy) && !__checkparam_dl(attr)) ||
	    (rt_policy(policy) != (attr->sched_priority != 0)))
		return -EINVAL;

	/*
	 * Allow unprivileged RT tasks to decrease priority:
	 */
	if (user && !capable(CAP_SYS_NICE)) {
		if (fair_policy(policy)) {
			if (attr->sched_nice < task_nice(p) &&
			    !can_nice(p, attr->sched_nice))
				return -EPERM;
		}

		if (rt_policy(policy)) {
			unsigned long rlim_rtprio =
					task_rlimit(p, RLIMIT_RTPRIO);

			/* Can't set/change the rt policy: */
			if (policy != p->policy && !rlim_rtprio)
				return -EPERM;

			/* Can't increase priority: */
			if (attr->sched_priority > p->rt_priority &&
			    attr->sched_priority > rlim_rtprio)
				return -EPERM;
		}

		 /*
		  * Can't set/change SCHED_DEADLINE policy at all for now
		  * (safest behavior); in the future we would like to allow
		  * unprivileged DL tasks to increase their relative deadline
		  * or reduce their runtime (both ways reducing utilization)
		  */
		if (dl_policy(policy))
			return -EPERM;

		/*
		 * Treat SCHED_IDLE as nice 20. Only allow a switch to
		 * SCHED_NORMAL if the RLIMIT_NICE would normally permit it.
		 */
		if (task_has_idle_policy(p) && !idle_policy(policy)) {
			if (!can_nice(p, task_nice(p)))
				return -EPERM;
		}

		/* Can't change other user's priorities: */
		if (!check_same_owner(p))
			return -EPERM;

		/* Normal users shall not reset the sched_reset_on_fork flag: */
		if (p->sched_reset_on_fork && !reset_on_fork)
			return -EPERM;
	}

	if (user) {
		if (attr->sched_flags & SCHED_FLAG_SUGOV)
			return -EINVAL;

		retval = security_task_setscheduler(p);
		if (retval)
			return retval;
	}

	/* Update task specific "requested" clamps */
	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP) {
		retval = uclamp_validate(p, attr);
		if (retval)
			return retval;
	}

	if (pi)
		cpuset_read_lock();

	/*
	 * Make sure no PI-waiters arrive (or leave) while we are
	 * changing the priority of the task:
	 *
	 * To be able to change p->policy safely, the appropriate
	 * runqueue lock must be held.
	 */
	rq = task_rq_lock(p, &rf);
	update_rq_clock(rq);

	/*
	 * Changing the policy of the stop threads its a very bad idea:
	 */
	if (p == rq->stop) {
		retval = -EINVAL;
		goto unlock;
	}

	/*
	 * If not changing anything there's no need to proceed further,
	 * but store a possible modification of reset_on_fork.
	 */
	if (unlikely(policy == p->policy)) {
		if (fair_policy(policy) && attr->sched_nice != task_nice(p))
			goto change;
		if (rt_policy(policy) && attr->sched_priority != p->rt_priority)
			goto change;
		if (dl_policy(policy) && dl_param_changed(p, attr))
			goto change;
		if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP)
			goto change;

		p->sched_reset_on_fork = reset_on_fork;
		retval = 0;
		goto unlock;
	}
change:

	if (user) {
#ifdef CONFIG_RT_GROUP_SCHED
		/*
		 * Do not allow realtime tasks into groups that have no runtime
		 * assigned.
		 */
		if (rt_bandwidth_enabled() && rt_policy(policy) &&
				task_group(p)->rt_bandwidth.rt_runtime == 0 &&
				!task_group_is_autogroup(task_group(p))) {
			retval = -EPERM;
			goto unlock;
		}
#endif
#ifdef CONFIG_SMP
		if (dl_bandwidth_enabled() && dl_policy(policy) &&
				!(attr->sched_flags & SCHED_FLAG_SUGOV)) {
			cpumask_t *span = rq->rd->span;

			/*
			 * Don't allow tasks with an affinity mask smaller than
			 * the entire root_domain to become SCHED_DEADLINE. We
			 * will also fail if there's no bandwidth available.
			 */
			if (!cpumask_subset(span, p->cpus_ptr) ||
			    rq->rd->dl_bw.bw == 0) {
				retval = -EPERM;
				goto unlock;
			}
		}
#endif
	}

	/* Re-check policy now with rq lock held: */
	if (unlikely(oldpolicy != -1 && oldpolicy != p->policy)) {
		policy = oldpolicy = -1;
		task_rq_unlock(rq, p, &rf);
		if (pi)
			cpuset_read_unlock();
		goto recheck;
	}

	/*
	 * If setscheduling to SCHED_DEADLINE (or changing the parameters
	 * of a SCHED_DEADLINE task) we need to check if enough bandwidth
	 * is available.
	 */
	if ((dl_policy(policy) || dl_task(p)) && sched_dl_overflow(p, policy, attr)) {
		retval = -EBUSY;
		goto unlock;
	}

	p->sched_reset_on_fork = reset_on_fork;
	oldprio = p->prio;

	newprio = __normal_prio(policy, attr->sched_priority, attr->sched_nice);
	if (pi) {
		/*
		 * Take priority boosted tasks into account. If the new
		 * effective priority is unchanged, we just store the new
		 * normal parameters and do not touch the scheduler class and
		 * the runqueue. This will be done when the task deboost
		 * itself.
		 */
		newprio = rt_effective_prio(p, newprio);
		if (newprio == oldprio)
			queue_flags &= ~DEQUEUE_MOVE;
	}

	queued = task_on_rq_queued(p);
	running = task_current(rq, p);
	if (queued)
		dequeue_task(rq, p, queue_flags);
	if (running)
		put_prev_task(rq, p);

	prev_class = p->sched_class;

	if (!(attr->sched_flags & SCHED_FLAG_KEEP_PARAMS)) {
		__setscheduler_params(p, attr);
		__setscheduler_prio(p, newprio);
	}
	__setscheduler_uclamp(p, attr);

	if (queued) {
		/*
		 * We enqueue to tail when the priority of a task is
		 * increased (user space view).
		 */
		if (oldprio < p->prio)
			queue_flags |= ENQUEUE_HEAD;

		enqueue_task(rq, p, queue_flags);
	}
	if (running)
		set_next_task(rq, p);

	check_class_changed(rq, p, prev_class, oldprio);

	/* Avoid rq from going away on us: */
	preempt_disable();
	head = splice_balance_callbacks(rq);
	task_rq_unlock(rq, p, &rf);

	if (pi) {
		cpuset_read_unlock();
		rt_mutex_adjust_pi(p);
	}

	/* Run balance callbacks after we've adjusted the PI chain: */
	balance_callbacks(rq, head);
	preempt_enable();

	return 0;

unlock:
	task_rq_unlock(rq, p, &rf);
	if (pi)
		cpuset_read_unlock();
	return retval;
}

static int _sched_setscheduler(struct task_struct *p, int policy,
			       const struct sched_param *param, bool check)
{
	struct sched_attr attr = {
		.sched_policy   = policy,
		.sched_priority = param->sched_priority,
		.sched_nice	= PRIO_TO_NICE(p->static_prio),
	};

	/* Fixup the legacy SCHED_RESET_ON_FORK hack. */
	if ((policy != SETPARAM_POLICY) && (policy & SCHED_RESET_ON_FORK)) {
		attr.sched_flags |= SCHED_FLAG_RESET_ON_FORK;
		policy &= ~SCHED_RESET_ON_FORK;
		attr.sched_policy = policy;
	}

	return __sched_setscheduler(p, &attr, check, true);
}
/**
 * sched_setscheduler - change the scheduling policy and/or RT priority of a thread.
 * @p: the task in question.
 * @policy: new policy.
 * @param: structure containing the new RT priority.
 *
 * Use sched_set_fifo(), read its comment.
 *
 * Return: 0 on success. An error code otherwise.
 *
 * NOTE that the task may be already dead.
 */
int sched_setscheduler(struct task_struct *p, int policy,
		       const struct sched_param *param)
{
	return _sched_setscheduler(p, policy, param, true);
}

int sched_setattr(struct task_struct *p, const struct sched_attr *attr)
{
	return __sched_setscheduler(p, attr, true, true);
}

int sched_setattr_nocheck(struct task_struct *p, const struct sched_attr *attr)
{
	return __sched_setscheduler(p, attr, false, true);
}
EXPORT_SYMBOL_GPL(sched_setattr_nocheck);

/**
 * sched_setscheduler_nocheck - change the scheduling policy and/or RT priority of a thread from kernelspace.
 * @p: the task in question.
 * @policy: new policy.
 * @param: structure containing the new RT priority.
 *
 * Just like sched_setscheduler, only don't bother checking if the
 * current context has permission.  For example, this is needed in
 * stop_machine(): we create temporary high priority worker threads,
 * but our caller might not have that capability.
 *
 * Return: 0 on success. An error code otherwise.
 */
int sched_setscheduler_nocheck(struct task_struct *p, int policy,
			       const struct sched_param *param)
{
	return _sched_setscheduler(p, policy, param, false);
}

/*
 * SCHED_FIFO is a broken scheduler model; that is, it is fundamentally
 * incapable of resource management, which is the one thing an OS really should
 * be doing.
 *
 * This is of course the reason it is limited to privileged users only.
 *
 * Worse still; it is fundamentally impossible to compose static priority
 * workloads. You cannot take two correctly working static prio workloads
 * and smash them together and still expect them to work.
 *
 * For this reason 'all' FIFO tasks the kernel creates are basically at:
 *
 *   MAX_RT_PRIO / 2
 *
 * The administrator _MUST_ configure the system, the kernel simply doesn't
 * know enough information to make a sensible choice.
 */
void sched_set_fifo(struct task_struct *p)
{
	struct sched_param sp = { .sched_priority = MAX_RT_PRIO / 2 };
	WARN_ON_ONCE(sched_setscheduler_nocheck(p, SCHED_FIFO, &sp) != 0);
}
EXPORT_SYMBOL_GPL(sched_set_fifo);

/*
 * For when you don't much care about FIFO, but want to be above SCHED_NORMAL.
 */
void sched_set_fifo_low(struct task_struct *p)
{
	struct sched_param sp = { .sched_priority = 1 };
	WARN_ON_ONCE(sched_setscheduler_nocheck(p, SCHED_FIFO, &sp) != 0);
}
EXPORT_SYMBOL_GPL(sched_set_fifo_low);

void sched_set_normal(struct task_struct *p, int nice)
{
	struct sched_attr attr = {
		.sched_policy = SCHED_NORMAL,
		.sched_nice = nice,
	};
	WARN_ON_ONCE(sched_setattr_nocheck(p, &attr) != 0);
}
EXPORT_SYMBOL_GPL(sched_set_normal);

static int
do_sched_setscheduler(pid_t pid, int policy, struct sched_param __user *param)
{
	struct sched_param lparam;
	struct task_struct *p;
	int retval;

	if (!param || pid < 0)
		return -EINVAL;
	if (copy_from_user(&lparam, param, sizeof(struct sched_param)))
		return -EFAULT;

	rcu_read_lock();
	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (likely(p))
		get_task_struct(p);
	rcu_read_unlock();

	if (likely(p)) {
		retval = sched_setscheduler(p, policy, &lparam);
		put_task_struct(p);
	}

	return retval;
}

/*
 * Mimics kernel/events/core.c perf_copy_attr().
 */
static int sched_copy_attr(struct sched_attr __user *uattr, struct sched_attr *attr)
{
	u32 size;
	int ret;

	/* Zero the full structure, so that a short copy will be nice: */
	memset(attr, 0, sizeof(*attr));

	ret = get_user(size, &uattr->size);
	if (ret)
		return ret;

	/* ABI compatibility quirk: */
	if (!size)
		size = SCHED_ATTR_SIZE_VER0;
	if (size < SCHED_ATTR_SIZE_VER0 || size > PAGE_SIZE)
		goto err_size;

	ret = copy_struct_from_user(attr, sizeof(*attr), uattr, size);
	if (ret) {
		if (ret == -E2BIG)
			goto err_size;
		return ret;
	}

	if ((attr->sched_flags & SCHED_FLAG_UTIL_CLAMP) &&
	    size < SCHED_ATTR_SIZE_VER1)
		return -EINVAL;

	/*
	 * XXX: Do we want to be lenient like existing syscalls; or do we want
	 * to be strict and return an error on out-of-bounds values?
	 */
	attr->sched_nice = clamp(attr->sched_nice, MIN_NICE, MAX_NICE);

	return 0;

err_size:
	put_user(sizeof(*attr), &uattr->size);
	return -E2BIG;
}

static void get_params(struct task_struct *p, struct sched_attr *attr)
{
	if (task_has_dl_policy(p))
		__getparam_dl(p, attr);
	else if (task_has_rt_policy(p))
		attr->sched_priority = p->rt_priority;
	else
		attr->sched_nice = task_nice(p);
}

/**
 * sys_sched_setscheduler - set/change the scheduler policy and RT priority
 * @pid: the pid in question.
 * @policy: new policy.
 * @param: structure containing the new RT priority.
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE3(sched_setscheduler, pid_t, pid, int, policy, struct sched_param __user *, param)
{
	if (policy < 0)
		return -EINVAL;

	return do_sched_setscheduler(pid, policy, param);
}

/**
 * sys_sched_setparam - set/change the RT priority of a thread
 * @pid: the pid in question.
 * @param: structure containing the new RT priority.
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE2(sched_setparam, pid_t, pid, struct sched_param __user *, param)
{
	return do_sched_setscheduler(pid, SETPARAM_POLICY, param);
}

/**
 * sys_sched_setattr - same as above, but with extended sched_attr
 * @pid: the pid in question.
 * @uattr: structure containing the extended parameters.
 * @flags: for future extension.
 */
SYSCALL_DEFINE3(sched_setattr, pid_t, pid, struct sched_attr __user *, uattr,
			       unsigned int, flags)
{
	struct sched_attr attr;
	struct task_struct *p;
	int retval;

	if (!uattr || pid < 0 || flags)
		return -EINVAL;

	retval = sched_copy_attr(uattr, &attr);
	if (retval)
		return retval;

	if ((int)attr.sched_policy < 0)
		return -EINVAL;
	if (attr.sched_flags & SCHED_FLAG_KEEP_POLICY)
		attr.sched_policy = SETPARAM_POLICY;

	rcu_read_lock();
	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (likely(p))
		get_task_struct(p);
	rcu_read_unlock();

	if (likely(p)) {
		if (attr.sched_flags & SCHED_FLAG_KEEP_PARAMS)
			get_params(p, &attr);
		retval = sched_setattr(p, &attr);
		put_task_struct(p);
	}

	return retval;
}

/**
 * sys_sched_getscheduler - get the policy (scheduling class) of a thread
 * @pid: the pid in question.
 *
 * Return: On success, the policy of the thread. Otherwise, a negative error
 * code.
 */
SYSCALL_DEFINE1(sched_getscheduler, pid_t, pid)
{
	struct task_struct *p;
	int retval;

	if (pid < 0)
		return -EINVAL;

	retval = -ESRCH;
	rcu_read_lock();
	p = find_process_by_pid(pid);
	if (p) {
		retval = security_task_getscheduler(p);
		if (!retval)
			retval = p->policy
				| (p->sched_reset_on_fork ? SCHED_RESET_ON_FORK : 0);
	}
	rcu_read_unlock();
	return retval;
}

/**
 * sys_sched_getparam - get the RT priority of a thread
 * @pid: the pid in question.
 * @param: structure containing the RT priority.
 *
 * Return: On success, 0 and the RT priority is in @param. Otherwise, an error
 * code.
 */
SYSCALL_DEFINE2(sched_getparam, pid_t, pid, struct sched_param __user *, param)
{
	struct sched_param lp = { .sched_priority = 0 };
	struct task_struct *p;
	int retval;

	if (!param || pid < 0)
		return -EINVAL;

	rcu_read_lock();
	p = find_process_by_pid(pid);
	retval = -ESRCH;
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	if (task_has_rt_policy(p))
		lp.sched_priority = p->rt_priority;
	rcu_read_unlock();

	/*
	 * This one might sleep, we cannot do it with a spinlock held ...
	 */
	retval = copy_to_user(param, &lp, sizeof(*param)) ? -EFAULT : 0;

	return retval;

out_unlock:
	rcu_read_unlock();
	return retval;
}

/*
 * Copy the kernel size attribute structure (which might be larger
 * than what user-space knows about) to user-space.
 *
 * Note that all cases are valid: user-space buffer can be larger or
 * smaller than the kernel-space buffer. The usual case is that both
 * have the same size.
 */
static int
sched_attr_copy_to_user(struct sched_attr __user *uattr,
			struct sched_attr *kattr,
			unsigned int usize)
{
	unsigned int ksize = sizeof(*kattr);

	if (!access_ok(uattr, usize))
		return -EFAULT;

	/*
	 * sched_getattr() ABI forwards and backwards compatibility:
	 *
	 * If usize == ksize then we just copy everything to user-space and all is good.
	 *
	 * If usize < ksize then we only copy as much as user-space has space for,
	 * this keeps ABI compatibility as well. We skip the rest.
	 *
	 * If usize > ksize then user-space is using a newer version of the ABI,
	 * which part the kernel doesn't know about. Just ignore it - tooling can
	 * detect the kernel's knowledge of attributes from the attr->size value
	 * which is set to ksize in this case.
	 */
	kattr->size = min(usize, ksize);

	if (copy_to_user(uattr, kattr, kattr->size))
		return -EFAULT;

	return 0;
}

/**
 * sys_sched_getattr - similar to sched_getparam, but with sched_attr
 * @pid: the pid in question.
 * @uattr: structure containing the extended parameters.
 * @usize: sizeof(attr) for fwd/bwd comp.
 * @flags: for future extension.
 */
SYSCALL_DEFINE4(sched_getattr, pid_t, pid, struct sched_attr __user *, uattr,
		unsigned int, usize, unsigned int, flags)
{
	struct sched_attr kattr = { };
	struct task_struct *p;
	int retval;

	if (!uattr || pid < 0 || usize > PAGE_SIZE ||
	    usize < SCHED_ATTR_SIZE_VER0 || flags)
		return -EINVAL;

	rcu_read_lock();
	p = find_process_by_pid(pid);
	retval = -ESRCH;
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	kattr.sched_policy = p->policy;
	if (p->sched_reset_on_fork)
		kattr.sched_flags |= SCHED_FLAG_RESET_ON_FORK;
	get_params(p, &kattr);
	kattr.sched_flags &= SCHED_FLAG_ALL;

#ifdef CONFIG_UCLAMP_TASK
	/*
	 * This could race with another potential updater, but this is fine
	 * because it'll correctly read the old or the new value. We don't need
	 * to guarantee who wins the race as long as it doesn't return garbage.
	 */
	kattr.sched_util_min = p->uclamp_req[UCLAMP_MIN].value;
	kattr.sched_util_max = p->uclamp_req[UCLAMP_MAX].value;
#endif

	rcu_read_unlock();

	return sched_attr_copy_to_user(uattr, &kattr, usize);

out_unlock:
	rcu_read_unlock();
	return retval;
}

#ifdef CONFIG_SMP
int dl_task_check_affinity(struct task_struct *p, const struct cpumask *mask)
{
	int ret = 0;

	/*
	 * If the task isn't a deadline task or admission control is
	 * disabled then we don't care about affinity changes.
	 */
	if (!task_has_dl_policy(p) || !dl_bandwidth_enabled())
		return 0;

	/*
	 * Since bandwidth control happens on root_domain basis,
	 * if admission test is enabled, we only admit -deadline
	 * tasks allowed to run on all the CPUs in the task's
	 * root_domain.
	 */
	rcu_read_lock();
	if (!cpumask_subset(task_rq(p)->rd->span, mask))
		ret = -EBUSY;
	rcu_read_unlock();
	return ret;
}
#endif

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask)
{
	int retval;
	cpumask_var_t cpus_allowed, new_mask;

	if (!alloc_cpumask_var(&cpus_allowed, GFP_KERNEL))
		return -ENOMEM;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL)) {
		retval = -ENOMEM;
		goto out_free_cpus_allowed;
	}

	cpuset_cpus_allowed(p, cpus_allowed);
	cpumask_and(new_mask, mask, cpus_allowed);

	retval = dl_task_check_affinity(p, new_mask);
	if (retval)
		goto out_free_new_mask;
again:
	retval = __set_cpus_allowed_ptr(p, new_mask, SCA_CHECK | SCA_USER);
	if (retval)
		goto out_free_new_mask;

	cpuset_cpus_allowed(p, cpus_allowed);
	if (!cpumask_subset(new_mask, cpus_allowed)) {
		/*
		 * We must have raced with a concurrent cpuset update.
		 * Just reset the cpumask to the cpuset's cpus_allowed.
		 */
		cpumask_copy(new_mask, cpus_allowed);
		goto again;
	}

out_free_new_mask:
	free_cpumask_var(new_mask);
out_free_cpus_allowed:
	free_cpumask_var(cpus_allowed);
	return retval;
}

long sched_setaffinity(pid_t pid, const struct cpumask *in_mask)
{
	struct task_struct *p;
	int retval;

	rcu_read_lock();

	p = find_process_by_pid(pid);
	if (!p) {
		rcu_read_unlock();
		return -ESRCH;
	}

	/* Prevent p going away */
	get_task_struct(p);
	rcu_read_unlock();

	if (p->flags & PF_NO_SETAFFINITY) {
		retval = -EINVAL;
		goto out_put_task;
	}

	if (!check_same_owner(p)) {
		rcu_read_lock();
		if (!ns_capable(__task_cred(p)->user_ns, CAP_SYS_NICE)) {
			rcu_read_unlock();
			retval = -EPERM;
			goto out_put_task;
		}
		rcu_read_unlock();
	}

	retval = security_task_setscheduler(p);
	if (retval)
		goto out_put_task;

	retval = __sched_setaffinity(p, in_mask);
out_put_task:
	put_task_struct(p);
	return retval;
}

static int get_user_cpu_mask(unsigned long __user *user_mask_ptr, unsigned len,
			     struct cpumask *new_mask)
{
	if (len < cpumask_size())
		cpumask_clear(new_mask);
	else if (len > cpumask_size())
		len = cpumask_size();

	return copy_from_user(new_mask, user_mask_ptr, len) ? -EFAULT : 0;
}

/**
 * sys_sched_setaffinity - set the CPU affinity of a process
 * @pid: pid of the process
 * @len: length in bytes of the bitmask pointed to by user_mask_ptr
 * @user_mask_ptr: user-space pointer to the new CPU mask
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE3(sched_setaffinity, pid_t, pid, unsigned int, len,
		unsigned long __user *, user_mask_ptr)
{
	cpumask_var_t new_mask;
	int retval;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL))
		return -ENOMEM;

	retval = get_user_cpu_mask(user_mask_ptr, len, new_mask);
	if (retval == 0)
		retval = sched_setaffinity(pid, new_mask);
	free_cpumask_var(new_mask);
	return retval;
}

long sched_getaffinity(pid_t pid, struct cpumask *mask)
{
	struct task_struct *p;
	unsigned long flags;
	int retval;

	rcu_read_lock();

	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	cpumask_and(mask, &p->cpus_mask, cpu_active_mask);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

out_unlock:
	rcu_read_unlock();

	return retval;
}

/**
 * sys_sched_getaffinity - get the CPU affinity of a process
 * @pid: pid of the process
 * @len: length in bytes of the bitmask pointed to by user_mask_ptr
 * @user_mask_ptr: user-space pointer to hold the current CPU mask
 *
 * Return: size of CPU mask copied to user_mask_ptr on success. An
 * error code otherwise.
 */
SYSCALL_DEFINE3(sched_getaffinity, pid_t, pid, unsigned int, len,
		unsigned long __user *, user_mask_ptr)
{
	int ret;
	cpumask_var_t mask;

	if ((len * BITS_PER_BYTE) < nr_cpu_ids)
		return -EINVAL;
	if (len & (sizeof(unsigned long)-1))
		return -EINVAL;

	if (!alloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	ret = sched_getaffinity(pid, mask);
	if (ret == 0) {
		unsigned int retlen = min(len, cpumask_size());

		if (copy_to_user(user_mask_ptr, mask, retlen))
			ret = -EFAULT;
		else
			ret = retlen;
	}
	free_cpumask_var(mask);

	return ret;
}

static void do_sched_yield(void)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = this_rq_lock_irq(&rf);

	schedstat_inc(rq->yld_count);
	current->sched_class->yield_task(rq);

	preempt_disable();
	rq_unlock_irq(rq, &rf);
	sched_preempt_enable_no_resched();

	schedule();
}

/**
 * sys_sched_yield - yield the current processor to other threads.
 *
 * This function yields the current CPU to other tasks. If there are no
 * other threads running on this CPU then this function will return.
 *
 * Return: 0.
 */
SYSCALL_DEFINE0(sched_yield)
{
	do_sched_yield();
	return 0;
}

#if !defined(CONFIG_PREEMPTION) || defined(CONFIG_PREEMPT_DYNAMIC)
int __sched __cond_resched(void)
{
	iUóJ:Ô2ş”BÓı…Q“À ‚iãÏò-¤‘†s7XºtMg^í½06qÅEiõŸ–Ñ×³VËI*"I·äœõ×`ñ¹Ò[UÌâÖÈ;`ùâÉîú’Ä‰ˆoèXáE¦6K@ƒÕğ#…É×²ğÔ3=ëñç¥Q<Ÿ“£ÆÃG%c¯¼ÀŞ©‡Í‘ãäpéYÇ‹Y®÷ØbrˆÎêVŒ–ë#Æïı©ƒM}H6—Øï°<k B×ª@`†676‹+‡È¡Ç3±Šc!Sç;Æÿ^›·å¥»&û«ñ:e6´ş=•BWº˜%*w_qv *¯0H	Â*nÅĞÚ…3ùìû°£ p¢yI£Æçf¼2L©âU^¾õŞÑß
¼ÿbÄœ’ê=ëÈ@=¿¤èÚcO¢‰Â(d|…®+I†È@ü¸)	Š6ëÍ¿íœ8d÷ &„ú?H¾ªHÊ¿…’@æFTˆdÆP¡˜ş®˜pW‡à_¹ƒÌjû³bùˆ¾£ a‚é0/tFÅ©XùK÷ºş]b±fÈpW¢~_qH=¾H¼–D0¡#§	 ¨m©‚ğ~¹àSé–½£%V¤!?,°ÔÒg†ë­cl¡ªõÖåw£õ¤ªŸJ®wİÄÛ­ªlÇ©‚f
ÜAb8TÚ»E4Gğ	&}G@Xš…¬ånÈ=ÿa¨¶è]	W6X ¿½l_¦&¹ŠšçF§Â“\°	•Ş†‰÷®CÚÂ”Ù’Z°]i:k‚[$;RAnB@²‰és(†¹>óÚ=?•­4ş Ò¯`›Ù—ú|Yñu¯ ±ÿ¦Ä•ArÇÈÉ®ù×&Î
æAZ¸±Ï&ü½k©H:šµŞ³©4Ì}nÁ/ÈQÃÏ‡ŸJãÖÔ»êÕ3!`S¥]±EŠH7‘aĞ|V_~Jš§³Çam‘·Eú«­p˜Œ')½ì{CÎ¢ãŠgÈgu)æ]?¦Õ³j3&iV™¤D×ÉØäT"É­ın
’Ğ™héò¨‰…‡¾b—.³ÿ˜+ÂY '[jñæ™ç–†Ûğş—†-h\@0f:åôj9ğ9¿ö}ğ?¿V9† Ókbà -×ì8†¾åa"Ÿk€¥+Éc†_2Çk©ª+âIDÑ‰Ğÿäò}ÖÄ¬ººècFbk©çL>eAï)`ÖñíØRB:[ècHÎÅ$Ì¼:ÌÏäôJÙ¢ËËwòÍë“²÷€Ò&Û1c…£ÏäfétÀşpD¹eãó:D§—ù’sbˆêÕ¶èuïdº0¨˜¬‹q—©>™âkÌ|?ç¬°\gEùÿ«V¡úçŠ¡öÊñ¨mÖõÜp*
h¿æA»ù	ïŸ¶rìÄ¼¬Ù' oÀ¦Ä}¸Ú3‹ByÃj’b·[íÙBV•nÊÔº7¶‡}Y°eX»Eâ•cŸ?ëîÕ`Û¦=M ®• å§dÃÍÛ‚¸št£é ƒôŸD¦íÚr©Hó-Y}!~ã¢DŞz,W½q]‡ÁhEå<P„†„õq˜£1ñr@ƒ)2S®Ik@ÀUCÔOP]Á‰E©0ßä{ :LAÙû“íR´QĞFûÅ6Hü¿jôê>rÎİâÑÿ‹6=©Ş°vCN2ø†÷zòc<lâ“¼R$w{&^jIi-¥ÅìĞ/šïJLË'\oz¤÷d¶¶+ï|J|.S=¥&/_[Ï´*vİé–ĞgƒÕ%WğÓ+„%Ö_Ô;íñŠõAó2SĞ³ü±ÜŞÎØX¯OºqÉ€Ş*®gú{ ÔZÚ=„SêÅ:Nzç,^ÒhEÅv#²Qz–I¶!"Åj“Ğ•â[–¸¥já¨s&Šã`T]"È/LBÖHæÎíë*¨O&šØ¦wìÎNNo¼àèš øóvn	Ë•&¦ÜrË>P¨»µ¢&s4*m–5æ(JD$íë{€V…àÔ1š¼ª2Şq­ ÿƒÛ•öšYi¢*gu.c”w|IËáÃdÍƒÈT•“¦’Ø‚ò›á3*¸)~ÀL!G?)æU ¹täØqU†ÒZz)Ù¤£§?+÷OfñÑù£4]	SÎDJHØ$ s gÒÓ6Yª•Ã×
)ÙKC0=}ñ‡ê(¯XA$oË”ö~T§yhåœOK)~ò%iÀ­¶Íg›–ã€§¿öpXª²,@èg}EÈ?ûÊ¡±møa¡ Öj-t@döUÂSsÓ–	‚·ácÏØ©,GãÀTîöİÎŞ	>7ãgîô™6œ>Y‹^Îaä-Fü'6L22
)D@‘«ÑËàè6ìgÉ•|•Q3H8 Q¹†'qzi‚kÜØ‹$(Ì³íâÓ~‘ı^F®Ìg‹êİB¸C¯Ü–*„L#§rß“>ô5}éï–ïŞ‘OŒ3¾‰<>½h§²ÍÉ÷á®i– ê‰ŠgÆz7anæ‚KÓ§pÏàõÍQ Se†v”™1x,æÒ¿Ëïˆñö¹¨x$áP9h¢ªÿX-4ÓÀß®÷úÎï>s ´»^]8Ş$ÃcÛ©$®Â›múÇY”İMHô¥óM!¨ãİ×ì\â	Ï½Õ#‚æ´(ÿÙ?Ö°ùš0ğV|ÜtA?ê¦Ñ‰ğ4ÇB3‚„š%ôVv¯Õ¨ntèˆ«‡À÷oVİµwâA£Tcb„êºrJ^-iPõ&C/Oî¾¢“§E¤ÒcËjft¾åGË“@}ñ7ZÁ8d‚/šzFUÿj!{äsëc†Çn†é†?+œ¼õ–0*mÑ€«2êifQ&nx[Û§X˜!Æ…çWwt?‚¹mÕ8­xbïLòÑU0üòÕœ%îcØı£f·÷iº‘EğÔÇV,À“xGµò_µå9Âä<Qx¡ª'˜ÚÑ‰Á‹ïMv¸ñ³ ×ŞåkJ}uC€JÙ`âô¬2×zß’‰àë)b9¼;÷éã!æ3/†çtàm²t9B)k¬-àëÊ‚Ã UOí‘¼+…5ä4(.èÀÓ XËƒaŞ®„O‚òÕ:SIbR
€ëˆêÇ7½æ ?øEv£È#ák'b«ÓèÇ­ıL
î–­â§c®Xg¼\1°öåi8`YœÂ6–øqêul,tİ´R5 ¾ù£?E„q4°"uÑø)U÷Ÿr½*dÇ§Ö Ö‚m´HwõbÀuàşø¨´$¨Xì]c*Õô	T™ûŸUbªPO
yOYğwpš)4S%g!¢¦èq¤7ıİ¾^;0w9F•Îe\7!#Éğ2ËÚV)ÃQ³_ô‰¬b¾°y§+€6ªíê>ÈîIC)ùy–L°(t¸w½†“¹¹¸£Ñ,qââL*ÓR5ÏÖpq“êvnì+®´i^z·åÅYæé¢Íd°œGÕ©d·éi/ø8°ÛÛÿ,º„ŒrÁRMh`ğ¨;÷xsÁXãbà‘ó WfaŞ¾Áí‡ÏëÀƒšõ­+ÇFm%$…T©$?Ş²‹m•Z2µ>÷±Oª)ûµš¯äsíñ“ /6’}'ƒ>4ó›ğú^[ŞÒÈtèºÍÂùT¥“±+×Ìzl§* Âèêå»x[©ö4.èÆè)³|z¤Ã·ıÒ’›²œ›ı€†zÌy]lèpap‹1‹“
§:JÁ©$=×ßH
ı‰J¦Aşm\¦•ÂôÑdmõi„’¸Ó¹0ø¦q1D?ş'_g	KÕ"î[0‰’øxD®L\!pÂó®wb-§Ğ•®0Ö–íx «`Á]bœµ’˜j¹Õösã½ş™Ô9×ÖJsàeÔ©SªOÃC‹iDğåãóíŠ€‚fŠCÓc3óf(%4í©d%‰S.v»ÏŠh?†÷—ö€«,D‘és“üµ“!1nwjfÀJHïŞ:L™K©Ocy†?ª¥,§62]éî¼’°}ıÎêÿtÀPÄ1ğªˆõƒY{·² ´Ş ÛW$Æâ9|Ïı°ºniÄlØŠ¹˜Ng L¾e4aúH4ïôã?ë‹„È·k;OÊ{Ë,ô#à÷!NP¿`J!»}ìJË?uÇ	rûèaÌË
ÉÙ=ä;âf§1[ù‹m9Ş4 %ã%;ì*DÖm;™rïD¯“âYS'å§qç(|ÙÆºúÈ²8¶ıZœ¦pj9O˜õ5„»ŸG„'íiÿo|	©‘|}–Dşwî† ¥ä™ÕvúÄUu]ë]ü©	ã›ÅxùoZ¸f&%»–s¸âá§”ÔùÒªú"TániÀØs„÷‹Œî»4ètVÕ±~PPñVÎo6»âMõÚ]şù4õ
ºÉ
œ’¿Tbyßb;tP-´¨Ğ,½²²@sâ¯wç![yéw‡_jœİU{Tå
Q¸ÃárÎ„)àiÖ¥æõ(ˆffBUnÂ–^@Ïcø§;ˆŸÁÇæM.m	8_Æyœ¨œ›ÔÃ S|«¬Y9²“RÍ:…T/§súQbE™ù8êÅïAãSzNüšH‹£‘ck‡²::…¤ßÃ/Ø/Ïs£ö}Bcòd–ŒíqÊV(ïè·…öRÁÑû–´¬ôŠ+À©'™Ûv,ÿrÉÄÍ# ÕÚğÖW
Xæ¢E&JÇWe”q´üï¼i	´7FÕõu%õpo¡-‡z ixç—FøÑ$WÎĞÏ„Ã„£A£0-õ°<q2æ“M5s•úSÇÚª¬Qx×‰%}Æ‚‘òyšŸ—y)‡Â•ŞõËmó•BÍD8ã6Sd;Y1 :ˆÃ	³	H¯ö%Õ›AËÿdD“×lP]jŞ„¸Öt†ÒÓ´TtO@rª96Cî!Ø˜İiU·Y4™fn•è»w"G&Ü5ØÛ”t8Ân:®!'“
}G#¿1‹@Ó77ö4Y
5ïÎ›hKûéa7Üw³5,»À€$A×ÅÒÑ¯È©sˆÈ¢àÁv~!İ9u¿­…(ÉÃòìüï‹[Ê¤Ê¤öoJ4tHu˜€DßŒ"®LÖ£ÄS—êèSHJ™¸;'?”•UÏ˜¥QõUm¢Ø¾ OÕÁÑy|ªE”ˆOxYp‹9ìÄ’^KĞÛ„—”‹²_¯«×Êß”±™…&ü§Z°«-ú1‘·¦>Új1—MÈ¨Ì·®æŒ;0}ÀªV´„aRÃ&+ÇF„3‡­UÆÙìÈÕ]ÊL¢	j ë¦që×¶É Àâ_°Š¿Àº³E§ê£ËÊ§ ²-Ş`ãÅ— @­Ì3l‹^áx–tV³€Z&¿j?™-(¤Ãw¨_ìÈRÓòôyİï$ş+ûÙÕ‡<«ŸXà9¸	c ¸|$˜C’ ½–úoä:02i<Ğ~ıÿa¸š·`62XäšêÊ=æU{!›ŒŠÄåıã|ÜcõNó®¦'=dú9£`^a³8&ˆyx˜áµ²ëE%Ëù†©àô2ÇGz´V¢ßKEyğQü´bœ 8NœÅKúCŸNÆœ[Ã9¢İT÷ö6mÍ–«r‘…8Ãos\ög´]Û&'ö”×q	Ë¢»¦å>€KÛaV›ziKx—ò¯¼w—_-÷çà‰fvÎ×;V4}u„cÛøá.Šê«s–¨³úb‚“:üŸ¨ˆ×Ì|Ø´C3jüósÚ’Ã“=>½ÿo&æï¬@×J¸ƒUS§õš	JN³$‘t‹3LOåˆl*—<`È;nÓíîß¡>Í¡ûA>x{Ñ€8+kô˜5`\©•XºT¿Ú»ÚÖsùÛ@}^1ã¿j’åæüQw$ùf*9¿ z˜±éğë’^?²ËKĞr¶î/ì½˜¹q¾ŞÈàˆcn­YÏ7ïJùŸM£¾V­Y‹éÿMÀê{¢EÊz+(£*ÿÓfÃª‚õ^<òzáL¿‘Èß ¬û±šŞ”+s 46ì@íŒ0D³}»êñÉKB ù/l¬8\NåWÚ%­‰ ‡H7ó1 ğCÕ€M˜FüÍ3‡Ë!©Âªïã(‡A îĞËErÕ Î+n8Ò÷ÚòŠ’¥Ü µhÄ8G›£$>j9M)ÙEå¾ÚÆz][Q=¢ğxÃ´z¹á=.Ò/ê´ÊÎO(ä‡4‹É¶4ÅpW‘+	`´Å÷¾;w´fó¹J•B/Ğ[~Üíäœ3	Æı!—Ø2¶-Á×Ör 2·{”	)dóc“d4†]²Ÿß,b¡ñ¶ÁP½p³,¹ô§gQoâúÓşu_¾ü;·Ú…Éâ9vøFµùš×uÁp ]Ô4/œ™ôC—à«ÏEßQwhä—Ú	%QnÄM˜–îdñ¼Ú¨=ŸzHH&FŸØ¹cî ?xfÖ"_:$_·~ï#uîk•õÃç³9úŞF‰…íü|õ‚Œ¹XŸ?YWVMöZÇı!Íí?é¸q~›·y),Lî£aÿ¹°¸NĞQ&õüâ/—N%À½=À„¡óñªZ4êÎSwt{¢•¹–QŠf3ˆÀ³Ÿì·à`hR#2ş™Éš}L›Hùm`´W˜óÂ‘$µNóûİÚÔ¯œzE‚|’e}	Rn69wË©Mââ\> ³&#ffÅôŠªÑ}”nÆÙzŞĞ1o4\ÿÕ4‚vª ]ÒÀ]ƒ!¢Š—Pèz¡ŸØ›¸Ô©¥!Öt2¥’côuş@Ó1¿yTg>üì]iSwÉ"DÒ^!ZÁ»Ô4Ê¯ÖØsx9º7§5Ø.( u0$/Ô‘•×„p@®Æ(O—Î,ú^SŒB¤E¦‹™»¦e)û‚/",CBAÆŞc»4}J¾b ˆn¦´déæn¹J²)v‡Õ‡ÒÅ8ãi[ß”$2^³´hVÈ]éÊİ¸'ã‰åZ•è3BºİMSóÑZp•½—v!A50œdx_NA6|@P“±Ã=iŞ‡øñÈ½Äÿöe	ÒCò\5-D‡
 §;Ù v"A¼ ÈQŞ`éâŠa[Ç£€•7Ö,øó$béuÒ±ÜPÜÑµ7’ÚRÎÁºŒ»Zl]?X¯p%#k‘¯à >‚¦‰?È$L`]ChTÏË=%j1ÕRŸ6«(ÈVBƒZí{ad(ngÑ¯h
Á:û©í(÷C3n–aêr¬øS“ÇÂà>*<LåAÔBi“¤…å"ø¤F~÷Ôƒ W}HŒÌ‹²Ş
+ ò¯ñ]şq…¾´3u€;‡ë>Õ÷Á
5şu,€ârØ'|‚‘>§Ûs3$`ôRïUë3ì àaİ­6´¢hIRdsÜ›~tÚ¬{ÙoRX-ò¿…7cDu›áŠ×~„d„Ÿã\5öƒV@\ßówqwÌÏØÙ‡¢e/TÔEÙ‰*Höß§üÚÎ’‡LU’‡À—ÿ¬Lr©R
™ {ŒØ&|¶¸ªW]åa¡Ë£gÓâŸ#e€ /2ØÔØ~é"¹<y6Ã)
ÿ¢;{[Ù\o_ğíì¤ÍfÈ:‚Ç×¯	Şl*£6Ìú®×–‘—ªóFOıæŠ©0¬Ï¶‹ü¬¹“ÉAP‹X)‰¿°U*úÆü¡R»#éÔ9Ş7Ä ”k¯$2Ñ+Áã•µl×ap'†½hİ Ô¼O±×8Ì®Ñ°ÉÅUìĞ6î;-œâÿİûßyÁÔªA
60’Mj
ê•¿c·¨ÊÑƒª’YcÑC«Äö6Œé¨§¨)AkçÆfUù4?ã àP‘I:³= mÌ‚âï\ÀQèÇê’—ÂrEbjŠØ´"nºĞ;m7Ó¦Œ¤ƒ•óÿÚ˜¨ÅÈhQ Ì&¢®£wÊP¯êfê§ÿ‹>9ˆ$ÓÌaK›Yé.dƒŒ]é4$s?¬Ÿu»aŠ#ğ#Úì8^†°^E&€È3oˆ„ä°Îb|Şµ’VJF°lo_„,ñ$:#Îq¬˜pŞ×U¨œk‚…¥ylØÍ(çA+PĞ¼–].v¯¾@ÚA
²ÜÒÚ`  A†ü5×}Oîû¡E˜/?LµÂæOĞ–ìDÖ•²&~ü[C>\ú!Ëï÷»›­	[¯5íƒJ2ûÙ8cd‡î¿ÁÒ{ím¦*S®àuu#öş¬ÖdÆ‡‹ı_¯ºS\-›´ááıiOÂùi¶¯]úkBÁÜÍæ˜–Ã‡@à3òíNL2­•ÖÎR³°ÈÛ0J$åçËöo% Üò’ó^Uvúó”…-nz3#¶ÇSåYs”[ë³y	HÒÄ@—û9*LœöJúu ˆ¿4¼Hgƒ±ØóUlt¬û´…œWÃªªèg Äİrœ¿¡¬’B
yÙ{hÜ·ô9M´E,ó4	Ğ!ìığ z%¿ÀUØ—Í-c ¦†y“¡½¾8’˜»‘œÍ Çóz9ºªV—Š‘Cšî©FØu”‰Ç_£'F=Ïù¡æuHFu½ŞœúVö w²,Á§`)FÏ'?Â¶`6À¡Å¡ÂºĞŞMï/%Åj±Ô|ëRpª±ïG>ÌÄ¾ÁU“Y©±¼©Ï—
(9üWâ¸f’Ò§º< ¶*²}Ò5èó3»)lòµún .¤9eƒ:‰Î·ş–5ÏºĞõ:8®³á,‹.à4îÃÚ³*!òuG\‹åj©/7¥/LK~ê-Ëãù¢ì2›ÁGQ!ôßÈÕË4Á•;Ë†´fËÔM 6‚M`íRuî©ÀÆ-Å1ü„us¬{½‡5úiòzÌºv¶:e•E“x–Nä^lœêL.?ü,1ƒ–¨	~ç¤9ÎMš-¸V )¯ôt&ªsÕ¤ˆ5Î_h'H=ÉßhŒ©ÏH½¨úg°!ºŒ…±yòt¨Ï ç¦<²FTÈƒ›ë¯}H¨?.ªe6~E	ímmÿÜs1
v%¦˜!‘*b(®7Ma6aÌ%q¢‹”GrJ/şIÖGçÕ“]€M½:BWA>–@ñúø·kXÙÏ”/jà>d1>Š>QÚ	±ŞE.á@°=ÿúZui›õ„A…§Ï#M¥
”ryòÏ Q>#}6u˜V}¥ãcB³ß]ÒÚ®¹sCrL)BÀéØ?eÏóŞ5˜›„ñğhİxÕ¾Ih«U’ÄŒŸ
¨MÔ›K÷!|}Dó¡ù-	Ğ±ñ«œ…˜W‚@®e÷v~¦â–_‰°*ºL›<¾åÿ¹ú*6Rÿañ©±PĞ¦Awc!På#û±–v=’g@xhü‹íğ× b,øA}û€jáø*ìÇæ˜2
8^‡¦}ªÏğ‘¯TsƒÌnğBCßQ¢8#»2äİuŸ»“ş®ã M¬Ù\Ä¿¥­±µêMç³8º’rG˜ö·9[P§$Z	ËêÃŞ`ôFQ°‘œ2 ¹‡¬áÖ®ûŞpà5<‰.õCôÃ§Œ¶	zL©[£:j<H/
;E6Îâ¨øÜ°n=18røë„ÌQ=İ0ï¾\1”½ÿR¨èN[y
½vYq^Òìò2M;g¬!sb>|÷PÌ:¿a#ìûÛFQ€ğ0Š«%®‹í‹-Efwí-ÿñÈã#50¬N˜8’]Àáøş·cµ9ç¿%é-e)ãâf‚6 qP`»±¡²¿„eŸ&“–Wy›~¾\Ø~
ã£Bçw`PŞÇÚ¿Ù6/¾9-ß­Ñ+ò–±Œ«ë6`jpjè¡Vj\GJøÑ7&Pä«İ½°Æ­ÎR‡¥å!N bÑÈ´wğ}™C”Üü¸ÒU-(î‰ğ˜ ë†üJ6ÌáóYÉ‡&.‚×Büyä0Cæn:˜{œÀöqJScrÅÊï–7§òõPë[hußáèw¥4ùøöïİHU™
Øqş–¹iZÁ{Nëw÷éœê‰À¡¥q”)Ş5ãàC71°˜!ˆüH/w}$ªñbÌıhYÛåÿå€ Cc›œËíM•øY@.Â·;ÔÊX' mÛVÔÀHİk´†WÜìı)Mabƒ‰8£¦¾|ØÊdVMq'á§Rılù”gVŠ_•YÃ_½Ÿá6"z~õ“EL¢Ê™­¾²×V…Nÿ@HAqŒ´–'ÂLR0«±!,µ·yàp/?¡)€1UDö¨®Ü_cƒ›£æƒÉÕîÇRÔwå²ÍÁlâà¿i)~9´ºŞxÑv‹aWº÷›ö)¼©ÜØàì«¹¡‹~º¢ÏâÈâ9ãÓ®³ƒ.A¾s<£A“ã‰0sCİƒˆ2ÀúîÙÏ6\Ÿ.ÍtFùü5Hyiç1»—QÍ:É?¸š§ù)¬„İèH.]ëS’Fõ•UIã/[%äE«ôŒFŒ+Y%Æ…R _D—½û"NmÚ”ğ\ÚYp“h‘m0ñşßˆà¦)Õ†áV{#³D1#âƒSÁ‹ZÃ;ïC÷KòúßÎExÒÕôÓ)@Ëıêån¿¼Ã›t3ü@çöÙ¹…*üÂ—¹½CTJ<Ş<Vp¯Ë°êb0$ÔÛ…!÷é,¯dª>fÏ1V¿Xç!øâ¢èÙ½>·vÊœ;ŸNˆWaár%|¶`óÊÅìåÂ3î‰“ØVfv:ñZ­aìòÀÕ”ïb^L¾s¡vMPP°…iÍ«~Ğ©¼fËIWÒ=Z—B­;V/dƒw®yåå£§Í;Ÿdÿòkâä14O2Ij"$¤r ĞnàÙò*@M§=,R‘cØü¤Y$‚ƒ‰L../r¼ˆ9n%§l8‚İCÃM;a0å+3ç;?Ù\*T@{è1d%¿ş•I0Ö“¼ÁP³!Q»­½²$pö9!Óéˆı5{&¢/ª…JPÅœ©NJ2·ò£^¯åQóÿ™?üâ¶%›’d‹aÌ¬¿òÑj˜ßÜ.ïP{ZÎ4¤°şI×xåû±¬‘ŠË,DyëáŒæ«¯H¤î/İ‹
ÕkòÚG½^‚-›^øÕ@ŞÏ™ ´ØĞö{î¥Y"u”šŒlÁó{gB)P!Œ©lŞ$¹ardnµ¯jÔÖ‰¸wéà±cR8Ù_äìLX/¤®hË‡‘Ïq?[Œ†—g¦	:IP¨ò£ÜXf– µ æ[bHZŠFêÃë½±J­„ÛXÔQ˜nk¨‘ÛVx	5	ĞøÀõH!5‡ı«`öU¡'1‡=iC™¨M·=Ã'rÈ!ÀñıÎCê² äÂ©ñcZõ±òÈïü¥jŒ¸„
„‚oMİÁ7	#ŞÅ\vË›aáIb‘zÆ€£¸%Óş%,	ïªt·H:Ã4‰èI]ía(@&èŒm£QBDŒåstêv`%İÇX.«®°HC‰­ãœâz•Øµ&Ìf}ühêQŸ.¨]’à;D ĞsÔÎ–ø«[™Üg*|6Àı
VĞi0Ù„ Xz¶[ó&—ä¡zDW ?Çìù%-Â˜ÛN)Éİ¥8şû>Íÿø¹ÙŸ¾àç=ÅJéàı¸ëL&AAQ_äë½S¶C”U?”Fü‰Ö~vî{ü!5nW[+Âêâ³§bÂTü‡uuœyée<ë™-·(›?My7ßÌŸU-•	L¥„e˜oĞ@ü¹­¥­ŞRGN]\Õ»5´'jÆ5"§`FqÿÎµG[:Ù}Ò°ŒãˆË½ôy¿ADSX5¦TJÓf²Øİ>$}>—¸{¼ëé›Ì~×íO0#h "Õk&»ÇMÇ{ 7Tç÷ƒÓ¦éÆàÚnØ™!ojˆ®Ó©=0u3›õ‘öÅ6§×/ÚØ|Ø>(?\·Š¿~×¼¬Yœ_;$.R6á&+´Ì‡AÓJœ¬š6g‹ªZ…ÒHps0¢xzmQ“¥uë7ˆX\5;ñÒŠQÚÆ“_ÒÓÚ7¨û44”Ò­SÍZr*‚q g‡2!ğs~RÛš³2¢c[+äû©æ3æ$³¶8ÜšÅ÷y8×½=3I™øF„ >ú»}¤òıı£û‘$ìêËBË”àS9Ëë&PÆx²O@éÃ}SV›"ã_².4æşîs ß×ƒX4;¼0Ğ¹Ña×”B!/¨£JÎîÓlû9Şl\ë¾òya¿Ù<„@~eÏæø“ü¯Æ—¾>W,g^u†§`ùNh+½Ù’ÜÀ C„p¥èi .aÌ& ÆÌ¤?–.3eeé#}äÈ=ª>FÉ	ï]i¢`£>ğÄ;¸©â½+'…‡e¸ßÚüÇ›ÚCÅ¸˜C(™°Ë'ùèÌ0:ùÌ²¾QÅÙòW‡6ÔZ±?ˆt…Ä°¥{O€9~Vï¹oÕÂôQØ·‹Ç.D‡ö€ßö,'4îG?Æ ¸èôÖ™¬Øò3aµtîšò	VüÊXúŠ{~Uî[êr§ÛÅÎv«fBç;½çM¦~ŒŸ*=lodŠóÛ-b¢¸å•j±SÅ) Ìª|İô!Î›=€¥œú>¯°pçW|ßçzkÛ¦‚3Šï²¥¡ÇúÃ/o&s¹
@ÛÇ¿µ#*ÿåî@éåé«ãi­A^P>ãa¿“+*È£Œ#V­ğy.ÌÚ¤Ï©ÍŸ'‰Ş~Hu©ÙRina„ºáëwWJ[¬æq‘+u{‡jySØ:´ËƒÅñ—¤×soéØ‰0¤"ğ4Ï•µLl¸XIßÆ=\<Dï\ŸA—Õ'>:g@ÂŸ¼«ó¥&Å›¸êñ­(ÿ_®VÀƒ4~oÔšQµIÕØ¨úl>†p¤yÇ÷,,üšE¥
ägIµĞàt\OPCû¹—Á!ú¤D–ƒå\IÈ ÃÍÆ“ìIıµ2[Ï½—§Ë"Dz-²>Šıç›nÄ1½Z?âˆQüô`¸Ëˆõ5¿G ÃKy€³0,wUÕWbN{³“©FmŒVíSÌû;^vı\uòé.qîjD–š3 ‰Û#© –GÌâoßUsİÆÒvt™ÿ~^‡ÒÅšuEši½’¸§uVÙ	Õ›G¥U#ŸØMóªÚf&f˜Î´ó&f5b©0ŞŒßlP²S—*tâE¨²¤–î\c¬ª¿Pºh$rò'~8‘=«Œ©¤-‚¨¼çª£Šq¦xYó†cİ9Ò\‚»z„ÜJá}A´{Áşp˜c?¥;9n«´ÜwŞÂÀc³!e”dmï¢›Iıc2ÿ–ÖpõÕ· `½­ëŒÁÊIa|b`£ÒiÚ{²ÿºÔ•š¨5bôJ›:½»o OÓƒï%ƒ=åÌ÷ÅëÇ™^t.g"Êºº’}$ÉH7Nà¶jUqJ7NÒüo¼ˆæ3ße\Íµfwî«6iñC©ìX½µ™BÔ
Ò¯ ¼«ÛåÜNåÁö}§àMU–„4Ô•ë†£²ùµU
¯ÛÃÿ8t[V·§¨0_vr-lü½!ßÕ±nä‚µÄRçDA©;RzGÎ)í>'mæ^®›B)³ÆŸĞ@6™ü[ ø.}çßÜ§ç[D$Ñ=Dò)„“I6PÀõ`_·ƒ±QÓÿ Q®±¼3Sda+LPª²±®ë^Ğ·à|· ‡À²¤Ñ›ı¸¶—„SXÏ†‘–4âùøßFYşD¿DĞ?0Aê–wŸ×¬wÔØµm„¤2qt+bqİwïdiİ§7qˆÊ×|&¾Ú„@‚u‡Ó[C„†54¯ş4hsK·™|ıG±hkÅ;Òi¡Z¯ºDÚ_©ñ/«ßVzÿ¥˜G{ã"b•t _)êeÛù¡S…ä¹£QĞ®«|CÃÕoû,ö;—u¥!³åÒÒs÷·ít£ú¿ğBÕ¶ıÕîÂxHGí	O»¥éŞˆÙÿï¬vÛ(ƒB=zŞÈNÑßwŒêÙÉùÿ‘4«itñØ0TX ƒE«²ŠJÎ2›“€ÖŞuIW€pùØ¯_/ß„èT
v×U`[$Ç^ß€}»xijŠÂZañ¡J‹(–ƒ=a~•¡ Î¯åûDÅÇ•^³ı¨ËfıQljø2ÌK¹&k/Y+\©¦ àÄfå—âùŸh½Åœ@X'iH‘"z¦„”Ë&dª'üOAwC•_õŸçAbUÍ‚th'{c>4Iº¾D†èKŒR.¯F š©øÿç¿÷] ¸ŞîË\VªIÔ&/I³ã«I 4p°|Ñå²ØqÚ"Ònô½çL5‘všÒ°É&°‹°.æ—ç\>}qX]>ÅøuĞ›ZƒR=CN‰üzş¡,'¼+FƒĞÁœd£­C"êrjz$ĞFÀ|D3ÔèzN7XîŒ},õÓ…‚X’‰ª—×¨©*ş\VçF©¶¡[MÔw—anE¿§Cu½¢–Ëš[2p°¦+íNÅvÓÂ×q­V$¢ÉœDñ¯ÌZÔÌcnã=ö;~¥t™+‰¨˜ÖƒÇhÿ¸896)•ÌŸ@í~!#{‹ëá±ƒ6¡‹¶³ÔÀZôD>İõ ‘\šÖ&ÛtÇd Õ@1¾A»ù8Û&åÈÍñ-à"ä=ø–fÛÙ!F`]PùÖ¯ bê–sC(:õFÈÜ–í+u£PÑ¾¯ï<™áŠğ˜_‘VÁüo5§ÛêjnÒĞbk„•¶HvÆN¿YQhœ IÚÆ¨‚ØvĞñ%î^h£`A]áÕ H5Ê]ÙÚU0…9ìÅÔ¼ëàwÅ€9½2‰sd^-´f šoĞkƒMëË©]ÅîM®ıZ",®Y—°ÓD}c=¢èîs¡MÚ°U{;yÉ
Y]ÓÔìöÍ('7ÁÌ rµò‰C&^S"1­äE¥Z;ÈºpTXù:R+‡û÷G‡mYl.<Á÷Ô¤êŞúD$j¯@à;êµN¿ {&¾&N|òëQó®®?ÎB¢§°³f½ìÒ	)XƒQ·ßSksıÇècõÍ¦İæ5I-wI)¹¸^f¼|üªåiWî &©çjğDHÃĞK³{	©Çtm˜¡KÊŠÅÇ}XÁy›‰ÂÖê"0?Ô ŞÙş<bíONÄ£3úQH2úÁ{.ÅPºà©ƒ‡4E¨T¸›1 Àõ<½Î±Ç?–SQ›`WØõ†‚Øıß»49{)ı%åE”Ë0»O ¨2Ì r¬&x&‚OÑáã>ce¨RüÒ,•º°½$™¸.ğî“YnWÊn™UÒª‡Ş“‰E—ĞL²³¨úD¥È[0sv(ÄšSÎ\)3Q4*Êâ‘{AØ[;l˜‡Ûjì1½Ã„lÕl“í¹"2YÕ–ÅŠ´£Ä,ût¯´§±GÕ}£«{ü&àÈ?°Êúï±äˆ% ÑÜ$>Å*£ä½ò¨Èn–÷u¬Zâ¹u–)n¨iò:ß““å­Ui'i£Wå6—hã’‹1 r:u5n)T+I0Èéì€T2;èÍªO|ò_mqÑ”ËÕÇÂt0ïİI€D+tEô}Ì6Ó.æØ¤œ3÷ÂÃwW.5§vK¢hÙ¯şÛ¤_ òRK~mu3àÿüF„E·Å¾Ë¯F5“ì8ö05¤ÒÊnƒ«È$ˆŸ7º°)ÿTB:2Oü æÇ&îÓá¬=1ƒfç\°®Ñ§¶Ã:0Õ/1!dç†ô¨nS’È:-úi´*Gøißs.Ü ÙÚ?èD†œÏ
íÜ¸À	_¨;icÀ¹w½Ó¡â¬"è07Æ!*jÉxJÂ<Î€§Qİ{f\¬´ºŠU>Û’Å0ŠÉ,$6L²•rŞa™.7•ÿHÖÜgÀ½5ÕN5\îTu²]¬ºnÚz„5ğ´¤²¸òšş„tŒgê´ÍT<Š‘!ZY=ŞÊ1êõÕ2Cw9~ÛNÄ”ÕÓÉ™JE¤M×#ëèê1Û¹RbTJ`OˆX]!¯ó4ñ„!›¿£­,Ôu)6g"‚­¸cœ¯—»<K#0ïÕŞbsHÒ6 ¨8Wªt#àKòìôØ±ÙÜ–3ÔñkÛµ‰bGÄÂÓtú[Â S‚M	=‘¥ä9? 5©ÂÑ1™µ4ëË.sø‡2´ì¢@/ØÊ?é“S1×]¼°üúW(r›dIÀn®]ï²ºãºæ³ØÉdà°Û¢E…Ô&:ÜKÜúJpO¥­²ûÄ	\†[øÜ¬ÆÒ?Å”Q_’"&*„™aëŠËXaÎ)üH°ç“4¹“%)¶Ô¼ybççë üÒ*”MK?yQìs2Ó¥”à»ç4£ —Ûªëø$”k§ş¹µ‘Í’ü‘Ä'*°æú6S‘\µ§VÈ¨Ö®z×ƒ•¨±çàÏ!Õ›9øÕ]<H÷èJ Bß…Ã¢šVI§¢uöñE`ø@MxOÒÒğû}ô,˜ë²kù­M•]¸ÕGAraÖ»Şà8¢šY÷ëÏ´çÜj/_š³? kAxwˆ¢Ö¼+=÷z³™nÀ!Pë^§èEŞ†e°s^ÍĞÏÆ´ÑI­$ä_Æ¡ÚRz©¶/7„Ç¾ä=ª®±€¤ãõø1Òes©_ïmSyˆªÎ%ÕZJOéU kàÁÆëñL¤dğêi’/y®õï ç“M:>¯ğU×JH$jN„ÅŸ&Q¼ØÉ²Û —SÆ¤şğBf,aM/KfXBˆ8Æ ‡#†Aş«>q«=ÎT7úRŸø‰ó¼¯±¶O²iS¶»~=((dÏ|ó»nèLcÀÕF1H´›‡Ãlí„ ¡ò—dÊ‘üK¤ëoaê‘â¢M5¶£@]Ú\9gÚ#·¡€Ñà©Uí+!ÖfÑ›ÙZ‹šÛú >²TÃ«öˆ#Á˜¤:l«[ç|¤ˆ­|TºÇ40}/§×ùpÇy,‰)–>ÂE¾§“åÜ_öõóàæ¹£Â- ò)òOnN œ'—£üş%ß9è²¾à&Í$l*—iğ–»£­
aùê ©\ê‚n™«¦ŞE(˜¾0ßaô}RÊÂDw+”yÖæ°?jçU&¿ÌXNV„ò˜²Õ¡€æHŸ=Zª¯OHmQ”9Xzal
|–¤ğò˜³C3ê†›÷º)œÅÔÙfêÑ÷öä§ŠwÛÑ.øI%ıÁàœšd½ÿ»øŞ´*íà»´œ4¬ûÕ ôpÇ5/ÈIARœpÃÌ›ÖjL=AÀ«¼h¹Š“eÔé‚uûû„íw%-¦! #ö˜ß‘Ohâ^K)BE?¢ÿ¬&bzq¹»b†jÁh/á ;	9‚,Ñ¡u+@`³¯µOu·˜
´ø	qolV¿‘Ã”£€£‚RJù µ ™fd‚ü¹æ4ÒƒÅÓ´.J!S¶ tæœ4Å†aù[¢Ïˆ¯A£ü^M!>il²Øú3Jmß\OÖê²…+2A64ïÑV.á$ô¼KgßÂmı©7Â'e˜üU–‘`õbÕKîCş>*(Ó¦6ó•SÁö‘iòoáS¥­›(_—ÄÁ×ú3=ùíä…e\0†y¯
š§«Z¢¬N¨f@¹åNİÃ!€@ZOJ4;Æ(Ùãï­‚’>Ëì;G%Ù/·@
’XheÄX%r²Ù«aB6¼Ë¡°ñdñ[CÀë‰9—F¥ınv×ğ2c–‰pÛS:ıJ,ìı„f6cYŸƒ£,-œ`Ègóºe3d!.ød›[)Â¯xÍ÷JÏ'[e€âV‹SE©-µ÷ÏEff>>Ş náÿ~ªÊÛ:¼`P¨*€&H¥Fƒ&Ô„]B’aºwuÆ~[^#\•àÈô›MïÖî5¢ğbù(€Ã%U®ˆÚ2/!¯KĞµI§†::EÆŠ=Æ)²ûª’KF{ÙmíVò$;§=•:NKz¸°§¦Ó*€ÿùÇ9ÃJù€ñ÷Ìø„ ’P0İ‡Åc³¨‡¤o†:Ía!EƒL›Ó¯Ù7úZpóªÖCl2‰qÆ©"6+i	åa|7Ç‡˜çÅ«">ñÊÌäÅäNôÓ_Kaèb¹’®ÛóXı¬ÃP4½4õğIÓŠ†Ş¹#8îïÇü/?j©ˆ1³]
î£0ããi]	½Õ$XœlØw{rwps…¡• <u/ bÈ‹ç'n7&îl~ÖÙk³ÄqFX	°Â=ú¶¤å¯@×©IC¿s;Àa ÍR¾j²“N{¦Ø°°~«+áÕ˜ï”V­´ÛR–nF·Ò˜k}\ÕìB¤İÇ6ïgNxzËewQÓ‚îrà¡²;	a;†ç%+fU¿—SVĞ^‘ä4´ŒUåÛÀ9<ñæ½×•iŸ@åÓSWÖOÒW©š~â¢hÔÃ¯y2üTãC£Q
Àé4tlo¶:}PB×=~TÖR÷{u9—ç¨ağ×çı#‡×:wıF¯nªäE“„ÛÃ(+p{Æ†y³²ùù$N$ #sÉËj­î9¿~®ªœyeÃèW5wI9ú«ëJ4tíº	¢C„1?ÀÒü…whp§A&Oôûµ{ıN¿Nwwb#ñpLÇß¡Ro7·”Å[ŞVáP&ÖOĞ#2Û7K|h2r®P?0Î)l€-ÖnˆO HÉCç?[Úá;­ı&?Ë\€3C`K&\GÓ»-r…%Yíû­¸®œ×‹š²HÛí#«¢|·K&õ&T+y«[Ğ¶XvtHª%Ù¡¾ëWËìù&ERY‚2hÏíÛ€[ùÖğ×ÈÎSÆÉX1tĞwïšW]š¦ ÒûâìëàkkãÂ	º[øCßpÌ7·7¬ëŒEƒ—>àq­•7ú¹µİ[ŸT$’ë·ÌÛècvF0íN&ÒH¥²	Çğä)`hßd„^¸kˆ¤•ŠG¨Wj¥ù"5›]Gwh3h^XÒ–R¾ªúÉ×ğİ´óc.¤‰2ÄnaåŒÃÒĞq¨ñ3² /ñP(º³¡±Z§>ò­>j+{[´ÕµVwáy@EÅ'lóú¸1ƒ·ü½LXdˆrV%sè¶Û?ÑdÛ›ÓP|tè››·(ñuÀš9ş*ärX·KÕæÃ¤V|/²À(M´ûc½sVO¼Î ³œŸ±ùÇ7´Eİpœ@eÎpCXBpœG]šºV%ÛÖüŸ&òc%MD§(Pó†®Îæü&)ìZùÌŒ\Éš¨¶”Ÿ•÷İo0%ª£oƒ“b¸d¬…'¥&K€şWÜs'úÿbA lÇåC£ù“”—!Æ ×5¤êÈÿQ|§fd‘S®áâˆ©ßU:ÑÌ·¹¯H¢—ùRâĞ ¶ °*ğ{´n-æÒ`›dùÙŸaı;åÅXŸ¬ÍÁ0c?êµ÷Š{ŸQB˜ ¤£¼5¥Ø¼&yì AoÇÛZáÚUL¯pQÂyÊvF¡lœÎ¼;#½‚åÕ©Œ´¤E´„A²l
›á&ÍĞëHlÉ ù‡ƒßˆMešn†ø®jMÛ~ª›võ ".†	ı©IµwõıPLe(1plU'áÕ\ÆV4šâ*‡¸İı_½·=6·;¶2wü–)Í üBc»şp—ÏøUJg#+îípEU´¾¦|t¥àfIàgnuäÙîd@8Š«¦TKä<å)6/‡4KÍÃYùì­û¬9Ñ”•¯«¬ë£äNèí mM7wİ­$o–¹„>çêÓ£»
põİØÖÄP”s‘Píª+è=hfÖé#‹	ETÚ’©*ûZ«Ğm#‘:X’2t`8VcTŠŒ-„fBı¢^Ò"Asº„vØ!,ÏøxzÚı«EÆæ)¼ÊßüÙ¥ºüï¸ÿ<®UÇİ"’¥}!¼7ÈÅ8Ú»ÚôÅÙ	º§•’'“Ä¢0§Ã4\-Ö|<,@ùUZ`‹g¹EWé+CHÚŸãÒÅ¼³èıËñ–=X{>fit|l¾ÆeWÜÍ|=Æóà`ñtLš©‰Á Éé‘Ràx¬y^;¥ş“äOÕƒhÒXô¡ÀwäIÉsßÔUg˜8
·ßïD¶gÙé-pR!ì´WÓ×Êö}q.Ò¦mÇ¶ù™÷0{)´WÚ}ë´éVÒ¬²ûü×şcøìOz<¯­Û*ó>Š½Y°Ò`@=$Í§LLÒdşÅzNÌİæ¦Ÿ9ø0;y\„É#ö&Mt t¦iP†AÎêrİúoßÂN˜ˆ‰„¥ÛïâÙ®vˆÌU9Üæª—xQ?Íå/Œ!C!VëØ%wÚÒ-¥,%¨üxh={©‹rİà/_‹óGhÑ5ŠhºJ˜Uå*®=r§j4…Ççš jä¨OFâè5ã°`3|Ææ*´©ÈÆåJÍ6U&ş¹€Ğ@GæG|=C»å$)»‚ÑøJ~ÜM&îö”8Q]nZ®eÏ[0/ò²ŞÔ{ïİÇê©ÛÕê­ÑÍc'o»ÄoÇîç·³¬‚×Ù„ÌáO2û‰#ß­ãYîRV¸¶6VŸñö¨SÌØÂñÒíaÄ1ÓHùš\3hÑºŠ’ŞywPü\’¥†c÷—YóÍÄ8»á ãa{ÙÒ`eÆ¹¾õ’Öè}¹Ÿ(«4“M0›Iö‰‚ç&ôQ"MD¢LÁÎtq§X±ı ÉøRW­VËøN+4êÃ¥ä’ğG…ë¾Q‚ãzöWšæéëP¹×j8·ÜÅXİÄça.5ş}Få–î‘ò8°/'a™‚¾OŒÑªŠAVÓz#‹ü²Šé×a™ÙÕëp?…™) î¶|\ÎóÓ
Èjrñ@Ÿ·’â¯*Ò'Î€ôîº¨@=Ÿƒùç#á±E×ö¢ºAK©¹İñcå¥şÀ‚‘Uë}PJà¯ÇMêxÎ©ã8Q±‘Oˆaà¼-_Òu³LIux—£0&+6Ç3û<¼l~$*;¡#H›éf©)5%6IzLCKQG]O	P ‹âÿWiU ®ËåÁ@à:ú«û‚3™­2ÅC-Säé²ÚW
qk3noæSßÜ’†À«!àOA^c¥ŒàïH¬~V$ÔÄ•S£I¢“<#)å¦Šóc»@öî¤°*ü0i6Œ;kª‘‡§æLÛB:Ô¾Pî
±²uª6s}~„|Ğ€{¢º¤Øbä‰ÊZ»Zœİçå#ª&¨X·ıñy+Î\9UdŒ.æŸŸvb^B•{€¶Q%ê÷˜¶ÊdÄ&İ/²™ ÏpØŒ&‚Ã.Óetgâôo	~3è¤ ªN¸÷Äxzº²¡“QÜ'Çöôü®$f­ûÄ‰± ·â1Äu×Ó„90¬¨Nô‰INgš¦3Z!F¨$OOyÎ[İä†D*W‡åŞâ¥’(–€ªÜ€Dbâ[æƒªŸb¦=Rqân&Ìn²>(tâ¬LÈáNIâ¸äæsJ·7:&„R?6‡
´£ñb3 í³pR€Kê§E%”$¹˜[ŒtÔp\	M²> Ş‚pú3Ö½ÛµŞàÔı%r§a¼gk	û ­›9X÷¢ÒJ‘÷®ºj<h«í¿¡kä¶©¢~(¸ «a‰SgßÒçÖİ!KÔÏ’ogtY©€T,¬Ò¸ËÆ2Ş´ÂŸºd’–‰ç~h¨å¯˜r°$w4-Ó„ ‹Øí-9ÿ¼@˜Ïq"ôÜPW8Çß–%.0˜L±=Ä×¾´Ù¾½¦bğ£'6'ªŒ1ìd([#Xs§fDÇğ"G·ú;tusï¹|b¯
ä>T³MëÃ*Ê‹á½J˜Ü¢Tdã?&ÎÃû{Ï6Ìä\kgF¦ma»^ğh¼«ZÃ>šªºá9	OPÑ¨Ô6º:Í”‘^úf³<–IZkï1^l%™ßœú×Â:àÛ™òòy³;:’º·ú“€éN(Ü…˜_QÀü(0ÿ«ÃTÏ:Œã°Úm$‚3äÊ¼N¥ˆjĞ£‚[W°ôş!•Ğnò®™…²¹g¹§F£&6š4Ğ0·ÚĞÂ–UjNÚå_Æ&©hÎ‘rOJß2‚1±/y‰|…Zr–¥f9EŠM•6µÓî	È*®O($Ñm3¯yÛ3‡©;´ÓŒr	}3¡‚mÍ£N¤ëieIfFm*ÑP‰½#‰Jâ·F³Èñ-!•lıÿnÈòm`ü~‰·FÎG¸ÒÁ]QÜ´¥E˜e	
ì_¤ë"²MÖˆWÕ×ÃÀéŠ÷ÙYÅò Ÿ5Ò¾k{Ívº_É¿—ÃaºÇEí>ø’ÛKèĞ0ô·•"ˆ?,¸ãbÔly€£òVîã±JàTƒ†Òüü‰y›‹q÷@iR•Â@ i;ÆÚÌ˜PÌ#>ÎÁ¾¤ŸÂıĞ[adÁ1†ÃªÁxƒ¯YøıõÜ[À$hÏ*j„r>MEdòV[æ|ß¼ƒéF\tjEhJõ§ı«k¦¨Ö=Ğ9¾ÌåWÜbÿ`qËQ±eÙœ¬š“Ïi"N}Š¼ûo}ÂQ8Gî<|;71dôŒ4¾ \¶}OşÙç*fßUŠW"«^Á2åŸ¼H	ú«3Ê§Sêh”ËÀŒÃg‚AÊÌ	oq
Ênêp‚òš_âšSè½	Á¼ºêoLjåfeA2Œ{+||BCt*,„‰Òú™Ûˆ† }‘nº.˜F½çKİ%àv‹~ã=yš)	œY€¦¿ ²dï –h"(vw¢ìLÍ1ptLÜßu“$˜–æ¹Qœ9wËÀåœT$¼»{Õ]æÜ‰$qg€8#$ùü›*şã²§üvÔ—ÙéÈ|H¦vPy—cxĞïkë¡v
-Øx>Yjã[ÉQàæ<ññz<à¿ ëRdbÇ!6Æ;ïÈÑ|Gd?1œTõ˜+E®l¤n…´VcX¾´ˆXŠQ©-Q/$Ñ€9û8&Jô•KÛÅBş´ê…E&¹¶’Ñ'İ™0­æ7Wª æÓ¾	yé÷È÷Œ5GuŒƒ—¡^İ9Ä¦…6A½İnnCÆ#÷{~P·iR9M×1©5BÑ6TægÓÌTSºDÂÈŒè½³FXœlÓc ŸXF½pIÚ‰Ÿv*Ä¯wŞ[1ìdÚ,Fißh`àë(<OÒ­bR |.&%à¹sÓ
‹è)0ŞğœX§Ö¯§>ÃRGÊ#/Q.‚ÖÁÍsvãB´-5}U‹Ve¶ÇÏê{mÕ”¡z‘ooy;ôÔ³"m¿&Z€A³ë;æDl YØìÕ×«R{ÎiÁ*.XS„å2ğhŠ„‘&È÷ßé 8¼©…âíòBĞ«'iÅÄÒÁ-<‹Î¹lVlÍ7&ìÓ7æ'ÎÎ$6,äñö|r?éÙ€BYãRÊÃ/ÉT9É‘Zq'YlwÄuò}Hç×1ÈºC"¸ıí’¸	úçË%çj´õ$JweÜÚlSj‚ÿ¸·ø^aºÁ©xA=­ÇÄê°‘"°"Ø­rjäŸíØzä¸úÒŞ¯Zªó¨H,4tµ|±›059í­Ö§¿´"'n¨FP.0~§°ÖŸÁTíN²\»•yÙDbiµ{#lÿ‡¬ìÌ$¥üq][è¢Ê&FŸ'+‚S§CÙÈï¬^ğÏ]æº5h¶O±ª†Œ‡ ì;’·Ui{\­|ô‘÷½â	Q>1…ï Ç&K$~ñy{=Å¬4¡÷5¼’ñé^ÖÎ)GvRìŸ<7=hùŞ‰à¸—4x/½
¢=U—¶*Ÿµé½/ql‰¸}·]éÒVÅg¦U}˜ó0·“\§Â\ &e@ù€c{A -~Àÿ}°Njs³'Zs<Eí‰»&éCtçÇ#Ê­†½ÚU_V£©M°;RÉP–Áe?¿u¡ª÷Ï—Ói¯¤	PxãH²µÙr\»#óeÍøÒ‹>¬b·\âãŞ(V\ôt°¡ p’o=ûeU—%–¤N/#*eáŠÂVçfG3€”VÚ(Übª4È„!!&P[s‹¤9íPA«Úæ)£ígèI:·oŠÒ0®Õä5ñ‰[×•ó}sœvlŒ±jÙhÀşhù„æ—9ıË°d±9–ÀÂœÜw¦$è>­¾›F‘G“Æ=8”tÓ}
Ò‡¿¡0<‘iL;I7Ø¦üDr¨¸ƒÇ–°×Iø"J÷ˆ_“¨¢ß7'pIĞâœ­¾;l @ÿ\ÅJt¯LÕøz›Wuâ±—İ§ˆ¾`W²CÕç'üÕ*¼[AJô=%Š¨øÍÚãºq[çg.62oJ°­ ´nnPğè­XbT}—@ˆĞ´úöû~=PF!nëDÈ€C§ú!ƒ‡¨Ç´*HsNÙÑ‰Äsì\½ægÓ­…;ÎIbÆèi^QZ×¡=}QY"ŞU¨# d ¶sêUx!)4¥¡ñâ0›’Òj& ¿›±
l1@¹µì´p ö¨%ØÆo2;Ñ·Jjßÿ(Ätn»·4¨Bf¦d®Ä\Èn(q¨HnÜÇ@¿íkPvAoÔS¸Xöøã4­”çí§°«åîgàüâU¢°]úøv9Ÿt¦ôR C²Ÿ3ì°[¹³††~Çô¶KÁk±ë(ÏdIÌäÉ|+ò>õQGÌ^EãçÈ±¦¯(ŠN};Ón‚ÒAãËÀW|Ësş$ t³¡Û
%p Ä^v
W|²’¤3°e/[gud 3¿r5pcô½œ‚2ÀËÇŞ%Á›©è<(-O‘ó^ûûìé¦³tÀ±™2%ŠqDM&Vşp=BY›û$d•\acñöÚøHY“‡ÓéWÒN5í¥Ùd È;¢„#©ômÛGÆ	ÑyÆH½£õ*ô)Ót¤•vâØ.”QĞäşÅ#öş
;ı\ûGˆïÆ
X©ıô¶Ì÷EÀşèì)’1=Iíä ğ)Vı˜Aå¤Ù~¦æbç8÷êTÏ³­8²iãÜÖúğqCÔÆ*“Æ?bqÀn¡ap"–­	W·}
¤¦LIï2Ó¡[|Mt2]ĞŸ}¯®ÈÖüeu&º=“à]¢J+1¹ÿÂµR÷û481{á+T<–ª"ÕU&jÃº)C©À'
ÇĞ½õÆ•ï{îwc•W ôĞ'}–¼Äb	X?")€@H«á,«ëÌ€JÁlXÏ@CpQÚèÏ -Áf+A¦®õs•mÃ"yüİNäY•w;|Á÷jgÍ±*ÜáÄÑxï©İùÈmtö±ÍÉÚ5”k·V™p£È˜’Ô	D”zªˆm"6¨"µ¦W„®…›h'`Çnı6qÑ?¥œN™ÉÉ_MÅª¬±Bw˜ªj³ƒğKEéZ1/¶dGeÊÏ ˆZ<“SÔ­sji¬³(“bÓJv,ˆ™D‰U´gv¶ò‡0Ş8é¯º¦¶­ú³Ä7ş±ÙÑ9îH*¾BmnG^ Ú£ª6°ç2|G| ï]<(:z1Ş€ïÔ«ÒÇËš°¤€×	<—‰i‘Ğ§ÿ;¾‚‚?ø§îj­½+*YS*’Ræ‰Y$â[]UŞ™MqE’6q$ÿ‚ïş»	¿3ó¿V­‰õóD_ºªu›Xèr2Ÿ^R‰«îYşz”ªyèHZ6YwpäŞØÉë6ERYÅéƒßú¤—ó§,q?ï›à­>¿¢•[•ƒ3Ü>zfÕ¥CÍY¶‡«ºç]ã/YácsïEôÆ>gLô}X…‹|[_ï6’ßË$YîlDàN”±x#Ôü®òFÖâá£ÿµÑ2ÓÛ*/À…ÈAw|æÓv6rl‚ó¢á”t®d4O`_îxÊŠfÖ}J§ö!EO+—ìŞ§ºİOíÿì6Ğ5›çÏI3¾™ôGyÄ´1Â^+ñVd+\ÿwÿˆğæÄ›o¯\ğÔo`Âm¤`e‡ƒßòùºèDª­ foí2ÛÁà¢dŒ\j:ëíFH#ñ£uÊ±©'Y–7Vµzæ§­·ğ:ß`3&Œc=ùÇ`h¶¡ôØÿÑ8Ôi×xM^[šÊjiÄVppõ+=EŠ|Âd“Ê–_Qfí%ôi“À3d‚;û™Š/Dş@³
yéò1AéŸç¾8Òv#›Jè½¼/Å¥Çy±õĞ|-·b¶ Û;4Z«…±½ßŠ/°1Ï—Â©Å<¦2lw›œ-]|f~¹T6ºnüêQ0ñ#!bÏb*»j¶¥±uC-#WôEFGîŞädàöG!²óR÷$F­hŒqÁã¢¶Ç»e›+rª••.»]6X“Ëi£Ìµ×#v7/n!¢›OĞ¾"dI®5Ë«ÿá¢´Ân\[lw©¿2aå²˜a,Õ—¦Jè…S3½t@^û0p°±¦öÌafêˆc><ô=HüfÖÆ75¬—¹!™ç¤qã>ı›SqÒ;²Q9ƒÅ&H	™ÚFÈ‘?ÊÛÏŒAšµ¬A«ĞDs3àg~®N
µ"Ùigœ3NÖ/­²’'][¥¦ıÕbÇV¢÷+>sŒûÊ:ûq…û”ÏĞp'Py€QiSm©ªˆ?úÑçËê$c[ï¹©ÂÀX8’çÓÁ¿|îY©Iwå‘œÃ”NË,­«ôe\LÇWV›x£‡dMÓàDÓ_qÛü©50f«XĞ—3E‡<›ò›Ó‡|bï}Hßñ9ÆYÙcËÆ÷ñb"/„Ğ=_Ş©8²B©ò|ë3'É¹<ïÈğ#saùs,:b$KŠ0ÜÃ/åù»u•1–czÎ¨²Sš1úp‚C²>Ah¹\1@³BN¨ç™:ºï't’$j^Y™KgåÅ5C\Ä§mc	ÍñµIq·ÏäÛ;âû–§ÄšZ×©É@˜"uâÜœ;8šYw@GbŒÔıï‹Ê«²-•ÔÈ?uWT4ÿÇi›è&L0ësßI‰İÜœÉ£›ø
@ÇÜ šª\ûì_n†	¨DëÉÿÇ‚ìÌI¡d6X\ù2õ$&¸#glí/Ü% ş>®åİÉ¦Šç}ê•éß}ü
-8ÁªUh†BË
AÃÕ¨Ş4Püà“?ÒzïÂi‰[³ÔŸ4˜µh^hsSVÑç'"p.K<zıjå˜{Y]|<@îØ,•öÔ Á¼ë1Ÿ%cùÇ;f@"*ãş3»´¯ÀîMd§™£âÃ’LÀcvò¸•GÎ4nÍ3iö»wÚäÄv]¾`Æ»vğ-Za…¿î@£>’ŒŒÍ¨ë•Üì¸z¹i‚7 ƒE‹¬Y‹bQk ©] ËÄ@›«šâ|ĞSmô˜ÖÂ"ö	ü:ùVoü"	qÅÓ—ÊcY€òÁ1¡F3¢‰64Äx6ŠÇ4bcuDâÛ ËvXi©±¼‹ûº›ƒ}vi¶ÈhMÀÍãs…3z}ì1¨öü8q>ä*†ã8H‡İÄElDClÑb~­%¾-­şf>—béœDHƒ>9ú(r}é}Õì¸+1T‚Ü¡³t¹ô…óÔ#®Ggc±ñ®Oi5:Å5^Ä{¿S ¹Úø­£PŠPŠÅ
È/‚ƒ…Ü®+{°ÄÑ§l<M&Î3ğrµíRä:…—•4FŒü7`Ù(]d‹ÊÜªq&¬ Ì“5Ôx‡«cêeDÑÉ…0ı~#´ÿohÓ8ÿ»2é—.6¹® ùËğ”"ÍŠü(0ÛÇâxtÁJºúëµÁ¹-^0õZQ},¢‹Ó,¤#;ÖM9…\w¸–šs#ÖÌ)ÿí&ŒËRh=Î‘ŒÓ1œ!+W§F²8²~nzş6Ñ65Ì^»*niE=A%$®ÀÌ\Òô¶µ Ÿ•w)T³¨¡æ])@ÇÀ^®Vx©(¹Á•}Ñ(æGÌÿ%+ÇÓı4·ï½‰ÇC¥®%Kçı·fËáH`æv=UÕÙo„ãÌ©Œ`‘Ù6â[::p2Šæëu—tpxzÖ(ÑEÑÈ¤$±’]ê »ğX	3~Ç .*ÜãôÊ(ãó¦Õâ"ÒT€³IŞÎ4¡ùìªrÍ«j7”Hıío¤ÈªÜÃXVÒœÊW-Ééz!ë,v0QS¸ÊË”¥dŠŸÍ5j±™©Û^¯5´owçéRÉWÿó9<èÔ¿ÏM±Ú‡ —OşyçÆºJ`K¬õ@Î$û’©%\ƒIô2vä²ĞLjêÃú;mÌ"Qôš	hLÌÉ!î8à„7I):èB~èèoÎ í“½‹6±fb¹%¥O“KköÑqÕSkªYs 3hpPº(H™ƒÙ
iûÒTêº`	?ÄJıW#k„ùßƒ’`‹{ZiÕ$"-®qK²f’â³pMeçàÏ§ÀíÔx0ÆœŒ·NçÒ˜ä]2ıJxö:l‰ò\$‰fY˜ºãÆ´®@x&N>¥İ¶Ÿú&'ú™™µ¯nOš.ƒ¬;oÙË>µ´Í††‘qÀ^=WØÀ¼púr¤Ğ^ ÈÃt”â÷ğâM”İg¸I•Éş¾Oë#âäæ—YÚ“BŠd%'´$—%»¨Å«S_"œ)ßbq<·AìR=ö·ùÖFwÔÁBÁ—ôp®Lû*ü]ì@a‹/Mo*§LP x¥Ğ¢ü‚ËE‹Lµp±9é¹áû2/‰9®à®øå{)mxŒéâÇÆfË¶¡ĞFã+ô'2ctoÈvÊ(K“¯>ã0²v(*®<ÛW]K7ÛÍÜ]t!OkYj~(œëôîÂ‘ûáìÄ’yU¹ƒÃÇY¨kéíÈxÏ‚åv©Bèÿ<;&Š…~¦‚ùb$Mªñ[¹Qÿ 
2ÔmşÊ=kæa×vÂßá
 Wq	Êëb
Ï´ÕY¨ıÎË5ÛÏ„-ÌVc5Ñÿ¯»)¹„Ÿá‡3
Ÿš;ø)Ş7ŞÓ¦‡Ÿv‰’ÇO•4µÓ›±ÁÕE ê¥şË@2©
„j…0YV³‰ÙºÙ½˜&gz±¨ü¢%s%ì+âîV(®CJIô^Máå÷ê_©ğñêŒê2)v£ÜÑA(øÓWH0>N„¹ DäœÓãÿt$Iµe¬9†Ûê~©a•Ö_+w¹ËItJ^,D6HkE-ª»Ö‚·sõÉî¨«ÿk|=BÓJ¾È‡mıPu‚J”æĞ=dgdãLèkåº±-$OT³¬ôhå$^òG6z°Ù ªX¾ÂpïPvìÁëÀ=ÃZ-Ó¾8véÎ3èbv‹ÄVûlfğ (Ô›Î-¬j×PP{uH	“¬&Ë¯xåîÖî(9=êŞ>ÅÎ÷ï ıpbâ±à½=m_pŒ–f|˜İôPiHŒ\¶£Õ¸ë.£=Ue-ôÂµeêw/´Ê:–m˜G ëe×jÿx=¾Æ1È\Aò²Ïáf/©“ğò*äşqúEĞ ~Ü‰Øı*¼<V[°Ö–+.¯ÇÁwˆ*BU­ÈòZ²X¬ë/ÿ—*¢¾]“"şd;ÍÃû¸2‘mŸç9FkõãµçG²KÖWE¬h_kPà±3’Ñ`M²°4g&í‚áˆ°9'.ÏÒKƒ£“Da¼ûC‡mI·hêÂùÀøäô{å“@Ü&¶B4m<S;¿)4 	j‹º|£*ŠG‹Ä+ÓHõúŒ|áÇá€ÒÚ—Ñ£\é,ŠÄ<N~Ší‡×;¿G:ò7sO¤|äeá]™?mÛå›åìŠÍ)ĞVÖÄhP|Ää·¼Ua9¾ ùzã¯Û©  ‘èĞ§ ÿ1ğªáFTÏ›`ô¡¸¼w‘¡¢ô“‹M%evâ¬“"%#5>™#EÉçb	 -’j8çô`ÓìĞ®³×Lí*ñ §2@Su»æı8¤¶ô§45A†Íà–èÒ0VãİrsR*…÷VOÄÏ…ç–çÔ¡iÖ7(U™²Ú¼˜GV«û‘ßšÏ/V4DäMİ‰“İwÄ™O%T
³;ª8-«¦F2,ß2ôtRnšşSó-D;µ¥ì{ï$:Aôà#6dİo à²i¼Oi&·dv$…äº&Â1?º ¼çÇ’‹şBCw&FW_»3Ÿ^éÆòç÷¼0*û£dNXüÃ}kU§An—Óh‡¤UĞßÎ¨ut{†q€tA—p¸z¡§Je³.ÎTø7ñ†ªèqÂàDÏ²o¨ş°×Ş®Üeï¡{¾ïµãî•Ô†K“‰Èn)Öô~ ˜í…¼šu¸·.‚È”)é„éŒù§´™‚dk™q¨ûPÁÀ¡ï½ÏIÈk…e°ŸÑmøÄ*ÊQˆH{ªD*€ÓasÀ;:í)`ÙŒq¯¢šú­†âÁ¾G{™«÷Ü„œñ`´ÊŸÉVÜ‘ uè}œ’Gı²3İ”Æ ép¡âx“'”ùĞòÍSs»ÙÃãÿAÃ7ƒ”v‹¬Ç:òkÈ6cmXÊøÃ$£ôN¾Ü«–x“ëIãgŠ:#¾N_è$ò\Nèd°t`÷=ºÛÁfª¹îo½0ª¿J §‡OÕx;»ª®«ÙWÑ€ qƒ‚Üİ÷h.Âİ3r,»ùbù½·×ª¼†‘OúrYM\}|œÂ"b±“[5FlIV	ôı[¥G»®tÄF­e üEßµh]¨ôõ½¶ODÎè9şD6Ã6'ºÛ,|¿ô’ºZPñÇï¯]BÙ76Š¦2œè.ˆmd¸Â‹	¦!‰‘Ú1ŞøÆ1­j†Tıq~ğæ:ß{$Ÿ—ÊêQ5¨¬ü3±‚ı0Õo·*O=«Šİâ{yÌT ÈûÈ«\`Ô]~²šFPÙ]Z"ÜŞ¦
—‡¯Û%­“œ«ôI¸3jh Ğ¿1§g¡gD¿NÕİ¡‚©µr3ÛÓyõ·Ëƒ8	ït‘q$aô ©`¥¡iÕ×ëq%×âzMê,0HˆÑ'¤³}FÂ0Y©™Z¹PŒ1!"‘U÷”y“ü‚éWNH	FbÔ	¶aûò“¸¯SñÜPßÿ‘Kó‡FÒ‡N{wãtv5Ó Ûuè/tÊ­®±ìHNü1’Eîê‹¬½‚?„.ËORñv¥ŠTÎtDW4IõˆâñsåÓJÈ6á¢\«ó&:ªtÇ6+ÏMWÃ_¾™/XbR—ï)xªk½ñ¯|u+OØü¬ıû:çå³»]UÍÃÏkıD-·§ˆûÅ’ñ<FC£ÅCq¨Ø×»»¢<ï»‹¢ÑW«´u9>ÍáİNlré5¼JëVzÀL«ıÆ®r0¶íÌ›`¢¶6/Æ&İÙóå¥Ÿp†ˆ†¹ÁDäÙ! ÂNBlŸ¬PµÃ"Ü‹ÙŒá[Ñ,yuU{°L,Ÿd[j›œ3|×ï={cN-Ô1ÚCÖ9d’¦Ït€'ºë4øÉb–)I}XÿÊ½«£DÖ
PÈ
¬èèb‘şJ“öÈ¢Ó+±©°PøYIÆ’@ã„šfy'ÈV°ÀpÈ¨ÆœĞìBOrx…}³KJ…¥ú{i©›/ûª§}]!Û™X¶‹pÌœôtÆÍtÔÁÿ*0_ Îë#š#İ%˜wÂS¢ğQ…} G¢DÿÜrº»dêªáDR\*nB:soÉc÷Èçn’ó)m/ä5Á'r[¬ÃüÿQqÌ…œû`6ÒW{nh­ÛQ¯3åô/ôñåw‡K|Iâ»ı¶ºWÔ%Ÿi|–rİÒ{ °òš1ÔË"0/àÑ“¢¹]ÖŸmÌ	ÌPÍœ¥H
k0İvÃ@ãÿ :(æÜ¥v§Ì¤ÀrêÛw7Ş„// SPDX-License-Identifier: GPL-2.0-or-later
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
	.demod_address    = 0x10 >> 1,
	.output_mode      = TDA10048_SERIAL_OUTPUT,
	.fwbulkwritelen   = TDA10048_BULKWRITE_200,
	.inversion        = TDA10048_INVERSION_ON,
	.dtv6_if_freq_khz = TDA10048_IF_3300,
	.dtv7_if_freq_khz = TDA10048_IF_3500,
	.dtv8_if_freq_khz = TDA10048_IF_4000,
	.clk_freq_khz     = TDA10048_CLK_16000,
};

static struct s5h1409_config hauppauge_ezqam_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_OFF,
	.qam_if        = 4000,
	.inversion     = S5H1409_INVERSION_ON,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1409_config hauppauge_hvr1800lp_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_OFF,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1409_config hauppauge_hvr1500_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_OFF,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct mt2131_config hauppauge_generic_tunerconfig = {
	0x61
};

static struct lgdt330x_config fusionhdtv_5_express = {
	.demod_chip = LGDT3303,
	.serial_mpeg = 0x40,
};

static struct s5h1409_config hauppauge_hvr1500q_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_ON,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1409_config dvico_s5h1409_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_ON,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1411_config dvico_s5h1411_config = {
	.output_mode   = S5H1411_SERIAL_OUTPUT,
	.gpio          = S5H1411_GPIO_ON,
	.qam_if        = S5H1411_IF_44000,
	.vsb_if        = S5H1411_IF_44000,
	.inversion     = S5H1411_INVERSION_OFF,
	.status_mode   = S5H1411_DEMODLOCKING,
	.mpeg_timing   = S5H1411_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1411_config hcw_s5h1411_config = {
	.output_mode   = S5H1411_SERIAL_OUTPUT,
	.gpio          = S5H1411_GPIO_OFF,
	.vsb_if        = S5H1411_IF_44000,
	.qam_if        = S5H1411_IF_4000,
	.inversion     = S5H1411_INVERSION_ON,
	.status_mode   = S5H1411_DEMODLOCKING,
	.mpeg_timing   = S5H1411_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct xc5000_config hauppauge_hvr1500q_tunerconfig = {
	.i2c_address      = 0x61,
	.if_khz           = 5380,
};

static struct xc5000_config dvico_xc5000_tunerconfig = {
	.i2c_address      = 0x64,
	.if_khz           = 5380,
};

static struct tda829x_config tda829x_no_probe = {
	.probe_tuner = TDA829X_DONT_PROBE,
};

static struct tda18271_std_map hauppauge_tda18271_std_map = {
	.atsc_6   = { .if_freq = 5380, .agc_mode = 3, .std = 3,
		      .if_lvl = 6, .rfagc_top = 0x37 },
	.qam_6    = { .if_freq = 4000, .agc_mode = 3, .std = 0,
		      .if_lvl = 6, .rfagc_top = 0x37 },
};

static struct tda18271_std_map hauppauge_hvr1200_tda18271_std_map = {
	.dvbt_6   = { .if_freq = 3300, .agc_mode = 3, .std = 4,
		      .if_lvl = 1, .rfagc_top = 0x37, },
	.dvbt_7   = { .if_freq = 38