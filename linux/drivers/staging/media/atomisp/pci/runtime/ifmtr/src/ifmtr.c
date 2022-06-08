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
	u32 pci