ffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(vb->vb2_queue->drv_priv, buf);
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
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
			struct cx23885_buffer, vb);
	struct cx23885_buffer *prev;
	struct cx23885_dmaqueue *q = &dev->vbiq;
	unsigned long flags;

	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	if (list_empty(&q->active)) {
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		dprintk(2, "[%p/%d] vbi_queue - first active\n",
			buf, buf->vb.vb2_buf.index);

	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
			buf, buf->vb.vb2_buf.index);
	}
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_vbi_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}


const struct vb2_ops cx23885_vbi_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"
#include "cx23885-video.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kmod.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/div64.h>

#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-event.h>
#include "cx23885-ioctl.h"
#include "xc2028.h"

#include <media/drv-intf/cx25840.h>

MODULE_DESCRIPTION("v4l2 driver module for cx23885 based TV cards");
MODULE_AUTHOR("Steven Toth <stoth@linuxtv.org>");
MODULE_LICENSE("GPL");

/* ------------------------------------------------------------------ */

static unsigned int video_nr[] = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };
static unsigned int vbi_nr[]   = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };

module_param_array(video_nr, int, NULL, 0444);
module_param_array(vbi_nr,   int, NULL, 0444);

MODULE_PARM_DESC(video_nr, "video device numbers");
MODULE_PARM_DESC(vbi_nr, "vbi device numbers");

static unsigned int video_debug;
module_param(video_debug, int, 0644);
MODULE_PARM_DESC(video_debug, "enable debug messages [video]");

static unsigned int irq_debug;
module_param(irq_debug, int, 0644);
MODULE_PARM_DESC(irq_debug, "enable debug messages [IRQ handler]");

static unsigned int vid_limit = 16;
module_param(vid_limit, int, 0644);
MODULE_PARM_DESC(vid_limit, "capture memory limit in megabytes");

#define dprintk(level, fmt, arg...)\
	do { if (video_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: video:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------- */
/* static data                                                         */

#define FORMAT_FLAGS_PACKED       0x01
static struct cx23885_fmt formats[] = {
	{
		.fourcc   = V4L2_PIX_FMT_YUYV,
		.depth    = 16,
		.flags    = FORMAT_FLAGS_PACKED,
	}
};

static struct cx23885_fmt *format_by_fourcc(unsigned int fourcc)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(formats); i++)
		if (formats[i].fourcc == fourcc)
			return formats+i;
	return NULL;
}

/* ------------------------------------------------------------------- */

void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count)
{
	struct cx23885_buffer *buf;

	if (list_empty(&q->active))
		return;
	buf = list_entry(q->active.next,
			struct cx23885_buffer, queue);

	buf->vb.sequence = q->count++;
	buf->vb.vb2_buf.timestamp = ktime_get_ns();
	dprintk(2, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
			buf->vb.vb2_buf.index, count, q->count);
	list_del(&buf->queue);
	vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
}

int cx23885_set_tvnorm(struct cx23885_dev *