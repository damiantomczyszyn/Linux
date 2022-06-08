// SPDX-License-Identifier: GPL-2.0-or-later
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
	cx_write(ch->cmds_start + 12, (lines*16) >> 3);
	cx_write(ch->cmds_start + 16, ch->ctrl_start);
	if (ch->jumponly)
		cx_write(ch->cmds_start + 20, 0x80000000 | (64 >> 2));
	else
		cx_write(ch->cmds_start + 20, 64 >> 2);
	for (i = 24; i < 80; i += 4)
		cx_write(ch->cmds_start + i, 0);

	/* fill registers */
	cx_write(ch->ptr1_reg, ch->fifo_start);
	cx_write(ch->ptr2_reg, cdt);
	cx_write(ch->cnt2_reg, (lines*16) >> 3);
	cx_write(ch->cnt1_reg, (bpl >> 3) - 1);

	dprintk(2, "[bridge %d] sram setup %s: bpl=%d lines=%d\n",
		dev->bridge,
		ch->name,
		bpl,
		lines);

	return 0;
}

void cx23885_sram_channel_dump(struct cx23885_dev *dev,
				      struct sram_channel *ch)
{
	static char *name[] = {
		"init risc lo",
		"init risc hi",
		"cdt base",
		"cdt size",
		"iq base",
		"iq size",
		"risc pc lo",
		"risc pc hi",
		"iq wr ptr",
		"iq rd ptr",
		"cdt current",
		"pci target lo",
		"pci target hi",
		"line / byte",
	};
	u32 risc;
	unsigned int i, j, n;

	pr_warn("%s: %s - dma channel status dump\n",
		dev->name, ch->name);
	for (i = 0; i < ARRAY_SIZE(name); i++)
		pr_warn("%s:   cmds: %-15s: 0x%08x\n",
			dev->name, name[i],
			cx_read(ch->cmds_start + 4*i));

	for (i = 0; i < 4; i++) {
		risc = cx_read(ch->cmds_start + 4 * (i + 14));
		pr_warn("%s:   risc%d: ", dev->name, i);
		cx23885_risc_decode(risc);
	}
	for (i = 0; i < (64 >> 2); i += n) {
		risc = cx_read(ch->ctrl_start + 4 * i);
		/* No consideration for bits 63-32 */

		pr_warn("%s:   (0x%08x) iq %x: ", dev->name,
			ch->ctrl_start + 4 * i, i);
		n = cx23885_risc_decode(risc);
		for (j = 1; j < n; j++) {
			risc = cx_read(ch->ctrl_start + 4 * (i + j));
			pr_warn("%s:   iq %x: 0x%08x [ arg #%d ]\n",
				dev->name, i+j, risc, j);
		}
	}

	pr_warn("%s: fifo: 0x%08x -> 0x%x\n",
		dev->name, ch->fifo_start, ch->fifo_start+ch->fifo_size);
	pr_warn("%s: ctrl: 0x%08x -> 0x%x\n",
		dev->name, ch->ctrl_start, ch->ctrl_start + 6*16);
	pr_warn("%s:   ptr1_reg: 0x%08x\n",
		dev->name, cx_read(ch->ptr1_reg));
	pr_warn("%s:   ptr2_reg: 0x%08x\n",
		dev->name, cx_read(ch->ptr2_reg));
	pr_warn("%s:   cnt1_reg: 0x%08x\n",
		dev->name, cx_read(ch->cnt1_reg));
	pr_warn("%s:   cnt2_reg: 0x%08x\n",
		dev->name, cx_read(ch->cnt2_reg));
}

static void cx23885_risc_disasm(struct cx23885_tsport *port,
				struct cx23885_riscmem *risc)
{
	struct cx23885_dev *dev = port->dev;
	unsigned int i, j, n;

	pr_info("%s: risc disasm: %p [dma=0x%08lx]\n",
	       dev->name, risc->cpu, (unsigned long)risc->dma);
	for (i = 0; i < (risc->size >> 2); i += n) {
		pr_info("%s:   %04d: ", dev->name, i);
		n = cx23885_risc_decode(le32_to_cpu(risc->cpu[i]));
		for (j = 1; j < n; j++)
			pr_info("%s:   %04d: 0x%08x [ arg #%d ]\n",
				dev->name, i + j, risc->cpu[i + j], j);
		if (risc->cpu[i] == cpu_to_le32(RISC_JUMP))
			break;
	}
}

static void cx23885_clear_bridge_error(struct 