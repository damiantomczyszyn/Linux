dio",
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
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask |= mask;

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask |= mask;
	cx_set(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask)
{
	u32 v;
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	v = mask & dev->pci_irqmask;
	if (v)
		cx_set(PCI_INT_MSK, v);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static inline void cx23885_irq_enable_all(struct cx23885_dev *dev)
{
	cx23885_irq_enable(dev, 0xffffffff);
}

void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	cx_clear(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static inline void cx23885_irq_disable_all(struct cx23885_dev *dev)
{
	cx23885_irq_disable(dev, 0xffffffff);
}

void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask &= ~mask;
	cx_clear(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static u32 cx23885_irq_get_mask(struct cx23885_dev *dev)
{
	u32 v;
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	v = cx_read(PCI_INT_MSK);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
	return v;
}

static int cx23885_risc_decode(u32 risc)
{
	static char *instr[16] = {
		[RISC_SYNC    >> 28] = "sync",
		[RISC_WRITE   >> 28] = "write",
		[RISC_WRITEC  >> 28] = "writec",
		[RISC_READ    >> 28] = "read",
		[RISC_READC   >> 28] = "readc",
		[RISC_JUMP    >> 28] = "jump",
		[RISC_SKIP    >> 28] = "skip",
		[RISC_WRITERM >> 28] = "writerm",
		[RISC_WRITECM >> 28] = "writecm",
		[RISC_WRITECR >> 28] = "writecr",
	};
	static int incr[16] = {
		[RISC_WRITE   >> 28] = 3,
		[RISC_JUMP    >> 28] = 3,
		[RISC_SKIP    >> 28] = 1,
		[RISC_SYNC    >> 28] = 1,
		[RISC_WRITERM >> 28] = 3,
		[RISC_WRITECM >> 28] = 3,
		[RISC_WRITECR >> 28] = 4,
	};
	static char *bits[] = {
		"12",   "13",   "14",   "resync",
		"cnt0", "cnt1", "18",   "19",
		"20",   "21",   "22",   "23",
		"irq1", "irq2", "eol",  "sol",
	};
	int i;

	printk(KERN_DEBUG "0x%08x [ %s", risc,
	       instr[risc >> 28] ? instr[risc >> 28] : "INVALID");
	for (i = ARRAY_SIZE(bits) - 1; i >= 0; i--)
		if (risc & (1 << (i + 12)))
			pr_cont(" %s", bits[i]);
	pr_cont(" count=%d ]\n", risc & 0xfff);
	return incr[risc >> 28] ? incr[risc >> 28] : 1;
}

static void cx23885_wakeup(struct cx23885_tsport *port,
			   struct cx23885_dmaqueue *q, u32 count)
{
	struct cx23885_buffer *buf;
	int count_delta;
	int max_buf_done = 5; /* service maximum five buffers */

	do {
		if (list_empty(&q->active))
			return;
		buf = list_entry(q->active.next,
				 struct cx23885_buffer, queue);

		buf->vb.vb2_buf.timestamp = ktime_get_ns();
		buf->vb.sequence = q->count++;
		if (count != (q->count % 65536)) {
			dprintk(1, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
				buf->vb.vb2_buf.index, count, q->count);
		} else {
			dprintk(7, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
				buf->vb.vb2_buf.index, count, q->count);
		}
		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
		max_buf_done--;
		/* count register is 16 bits so apply modulo appropriately */
		count_delta = ((int)count - (int)(q->count % 65536));
	} while ((count_delta > 0) && (max_buf_done > 0));
}

int cx23885_sram_channel_setup(struct cx23885_dev *dev,
				      struct sram_channel *ch,
				      unsigned int bpl, u32 risc)
{
	unsigned int i, lines;
	u32 cdt;

	if (ch->cmds_start == 0) {
		dprintk(1, "%s() Erasing channel [%s]\n", __func__,
			ch->name);
		cx_write(ch->ptr1_reg, 0);
		cx_write(ch->ptr2_reg, 0);
		cx_write(ch->cnt2_reg, 0);
		cx_write(ch->cnt1_reg, 0);
		return 0;
	} else {
		dprintk(1, "%s() Configuring channel [%s]\n", __func__,
			ch->name);
	}

	bpl   = (bpl + 7) & ~7; /* alignment */
	cdt   = ch->cdt;
	lines = ch->fifo_size / bpl;
	if (lines > 6)
		lines = 6;
	BUG_ON(lines < 2);

	cx_write(8 + 0, RISC_JUMP | RISC_CNT_RESET);
	cx_write(8 + 4, 12);
	cx_write(8 + 8, 0);

	/* write CDT */
	for (i = 0; i < lines; i++) {
		dprintk(2, "%s() 0x%08x <- 0x%08x\n", __func__, cdt + 16*i,
			ch->fifo_start + bpl*i);
		cx_write(cdt + 16*i, ch->fifo_start + bpl*i);
		cx_write(cdt + 16*i +  4, 0);
		cx_write(cdt + 16*i +  8, 0);
		cx_write(cdt + 16*i + 12, 0);
	}

	/* write CMDS */
	if (ch->jumponly)
		cx_write(ch->cmds_start + 0, 8);
	else
		cx_write(ch->cmds_start + 0, risc);
	cx_write(ch->cmds_start +  4, 0); /* 64 bits 63-32 */
	cx_write(ch->cmds_start +  8, cdt);