// SPDX-License-Identifier: GPL-2.0-or-later
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

int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm)
{
	struct v4l2_subdev_format format = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
		.format.code = MEDIA_BUS_FMT_FIXED,
	};

	dprintk(1, "%s(norm = 0x%08x) name: [%s]\n",
		__func__,
		(unsigned int)norm,
		v4l2_norm_to_name(norm));

	if (dev->tvnorm == norm)
		return 0;

	if (dev->tvnorm != norm) {
		if (vb2_is_busy(&dev->vb2_vidq) || vb2_is_busy(&dev->vb2_vbiq) ||
		    vb2_is_busy(&dev->vb2_mpegq))
			return -EBUSY;
	}

	dev->tvnorm = norm;
	dev->width = 720;
	dev->height = norm_maxh(norm);
	dev->field = V4L2_FIELD_INTERLACED;

	call_all(dev, video, s_std, norm);

	format.format.width = dev->width;
	format.format.height = dev->height;
	format.format.field = dev->field;
	call_all(dev, pad, set_fmt, NULL, &format);

	return 0;
}

static struct video_device *cx23885_vdev_init(struct cx23885_dev *dev,
				    struct pci_dev *pci,
				    struct video_device *template,
				    char *type)
{
	struct video_device *vfd;
	dprintk(1, "%s()\n", __func__);

	vfd = video_device_alloc();
	if (NULL == vfd)
		return NULL;
	*vfd = *template;
	vfd->v4l2_dev = &dev->v4l2_dev;
	vfd->release = video_device_release;
	vfd->lock = &dev->lock;
	snprintf(vfd->name, sizeof(vfd->name), "%s (%s)",
		 cx23885_boards[dev->board].name, type);
	video_set_drvdata(vfd, dev);
	return vfd;
}

int cx23885_flatiron_write(struct cx23885_dev *dev, u8 reg, u8 data)
{
	/* 8 bit registers, 8 bit values */
	u8 buf[] = { reg, data };

	struct i2c_msg msg = { .addr = 0x98 >> 1,
		.flags = 0, .buf = buf, .len = 2 };

	return i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}

u8 cx23885_flatiron_read(struct cx23885_dev *dev, u8 reg)
{
	/* 8 bit registers, 8 bit values */
	int ret;
	u8 b0[] = { reg };
	u8 b1[] = { 0 };

	struct i2c_msg msg[] = {
		{ .addr = 0x98 >> 1, .flags = 0, .buf = b0, .len = 1 },
		{ .addr = 0x98 >> 1, .flags = I2C_M_RD, .buf = b1, .len = 1 }
	};

	ret = i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg[0], 2);
	if (ret != 2)
		pr_err("%s() error\n", __func__);

	return b1[0];
}

static void cx23885_flatiron_dump(struct cx23885_dev *dev)
{
	int i;
	dprintk(1, "Flatiron dump\n");
	for (i = 0; i < 0x24; i++) {
		dprintk(1, "FI[%02x] = %02x\n", i,
			cx23885_flatiron_read(dev, i));
	}
}

static int cx23885_flatiron_mux(struct cx23885_dev *dev, int input)
{
	u8 val;
	dprintk(1, "%s(input = %d)\n", __func__, input);

	if (input == 1)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) & ~FLD_CH_SEL;
	else if (input == 2)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) | FLD_CH_SEL;
	else
		return -EINVAL;

	val |= 0x20; /* Enable clock to delta-sigma and dec filter */

	cx23885_flatiron_write(dev, CH_PWR_CTRL1, val);

	/* Wake up */
	cx23885_flatiron_write(dev, CH_PWR_CTRL2, 0);

	if (video_debug)
		cx23885_flatiron_dump(dev);

	return 0;
}

static int cx23885_video_mux(struct cx23885_dev *dev, unsigned int input)
{
	dprintk(1, "%s() video_mux: %d [vmux=%d, gpio=0x%x,0x%x,0x%x,0x%x]\n",
		__func__,
		input, INPUT(input)->vmux,
		INPUT(input)->gpio0, INPUT(input)->gpio1,
		INPUT(input)->gpio2, INPUT(input)->gpio3);
	dev->input = input;

	if (dev->board == CX23885_BOARD_MYGICA_X8506 ||
		dev->board == CX23885_BOARD_MAGICPRO_PROHDTVE2 ||
		dev->board == CX23885_BOARD_MYGICA_X8507) {
		/* Select Analog TV */
		if (INPUT(input)->type == CX23885_VMUX_TELEVISION)
			cx23885_gpio_clear(dev, GPIO_0);
	}

	/* Tell the internal A/V decoder */
	v4l2_subdev_call(dev->sd_cx25840, video, s_routing,
			INPUT(input)->vmux, 0, 0);

	if ((dev->board == CX23885_BOARD_HAUPPAUGE_HVR1800) ||
		(dev->board == CX23885_BOARD_MPX885) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1250) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_IMPACTVCBE) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255_22111) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1265_K4) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1850) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR5525) ||
		(dev->board == CX23885_BOARD_MYGICA_X8507) ||
		(dev->board == CX23885_BOARD_AVERMEDIA_HC81R) ||
		(dev->board == CX23885_BOARD_VIEWCAST_260E) ||
		(dev->board == CX23885_BOARD_VIEWCAST_460E) ||
		(dev->board == CX23885_BOARD_AVERMEDIA_CE310B)) {
		/* Configure audio routing */
		v4l2_subdev_call(dev->sd_cx25840, audio, s_routing,
			INPUT(input)->amux, 0, 0);

		if (INPUT(input)->amux == CX25840_AUDIO7)
			cx23885_flatiron_mux(dev, 1);
		else if (INPUT(input)->amux == CX25840_AUDIO6)
			cx23885_flatiron_mux(dev, 2);
	}

	return 0;
}

static int cx23885_audio_mux(struct cx23885_dev *dev, unsigned int input)
{
	dprintk(1, "%s(input=%d)\n", __func__, input);

	/* The baseband video core of the cx23885 has two audio inputs.
	 * LR1 and LR2. In almost every single case so far only HVR1xxx
	 * cards we've only ever supported LR1. Time to support LR2,
	 * which is available via the optional white breakout header on
	 * the board.
	 * We'll use a could of existing enums in the card struct to allow
	 * devs to specify which baseband input they need, or just default
	 * to what we've always used.
	 */
	if (INPUT(input)->amux == CX25840_AUDIO7)
		cx23885_flatiron_mux(dev, 1);
	else if (INPUT(input)->amux == CX25840_AUDIO6)
		cx23885_flatiron_mux(dev, 2);
	else {
		/* Not specifically defined, assume the default. */
		cx23885_flatiron_mux(dev, 1);
	}

	return 0;
}

/* ------------------------------------------------------------------ */
static int cx23885_start_video_dma(struct cx23885_dev *dev,
			   struct cx23885_dmaqueue *q,
			   struct cx23885_buffer *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* Stop the dma/fifo before we tamper with it's risc programs */
	cx_clear(VID_A_DMA_CTL, 0x11);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH01],
				buf->bpl, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000011);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x11); /* FIFO and RISC enable */

	return 0;
}

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;

	*num_planes = 1;
	sizes[0] = (dev->fmt->depth * dev->width * dev->height) >> 3;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);
	u32 line0_offset, line1_offset;
	struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, 0);
	int field_tff;

	buf->bpl = (dev->width * dev->fmt->depth) >> 3;

	if (vb2_plane_size(vb, 0) < dev->height * buf->bpl)
		return -EINVAL;
	vb2_set_plane_payload(vb, 0, dev->height * buf->bpl);

	switch (dev->field) {
	case V4L2_FIELD_TOP:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, 0, UNSET,
				buf->bpl, 0, dev->height);
		break;
	case V4L2_FIELD_BOTTOM:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, UNSET, 0,
				buf->bpl, 0, dev->height);
		break;
	case V4L2_FIELD_INTERLACED:
		if (dev->tvnorm & V4L2_STD_525_60)
			/* NTSC or  */
			field_tff = 1;
		else
			field_tff = 0;

		if (cx23885_boards[dev->board].force_bff)
			/* PAL / SECAM OR 888 in NTSC MODE */
			field_tff = 0;

		if (field_tff) {
			/* cx25840 transmits NTSC bottom field first */
			dprintk(1, "%s() Creating TFF/NTSC risc\n",
					__func__);
			line0_offset = buf->bpl;
			line1_offset = 0;
		} else {
			/* All other formats are top field first */
			dprintk(1, "%s() Creating BFF/PAL/SECAM risc\n",
					__func__);
			line0_offset = 0;
			line1_offset = buf->bpl;
		}
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, line0_offset,
				line1_offset,
				buf->bpl, buf->bpl,
				dev->height >> 1);
		break;
	case V4L2_FIELD_SEQ_TB:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl,
				0, buf->bpl * (dev->height >> 1),
				buf->bpl, 0,
				dev->height >> 1);
		break;
	case V4L2_FIELD_SEQ_BT:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl,
				buf->bpl * (dev->height >> 1), 0,
				buf->bpl, 0,
				dev->height >> 1);
		break;
	default:
		BUG();
	}
	dprintk(2, "[%p/%d] buffer_init - %dx%d %dbpp 0x%08x - dma=0x%08lx\n",
		buf, buf->vb.vb2_buf.index,
		dev->width, dev->height, dev->fmt->depth, dev->fmt->fourcc,
		(unsigned long)buf->risc.dma);
	return 0;
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
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
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct cx23885_buffer   *prev;
	struct cx23885_dmaqueue *q    = &dev->vidq;
	unsigned long flags;

	/* add jump to start */
	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	spin_lock_irqsave(&dev->slock, flags);
	if (list_empty(&q->active)) {
		list_add_tail(&buf->queue, &q->active);
		dprintk(2, "[%p/%d] buffer_queue - first active\n",
			buf, buf->vb.vb2_buf.index);
	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		list_add_tail(&buf->queue, &q->active);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
				buf, buf->vb.vb2_buf.index);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_video_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x11);
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static const struct vb2_ops cx23885_video_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

/* ------------------------------------------------------------------ */
/* VIDEO IOCTLS                                                       */

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.width        = dev->width;
	f->fmt.pix.height       = dev->height;
	f->fmt.pix.field        = dev->field;
	f->fmt.pix.pixelformat  = dev->fmt->fourcc;
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * dev->fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct cx23885_fmt *fmt;
	enum v4l2_field   field;
	unsigned int      maxw, maxh;

	fmt = format_by_fourcc(f->fmt.pix.pixelformat);
	if (NULL == fmt)
		return -EINVAL;

	field = f->fmt.pix.field;
	maxw  = 720;
	maxh  = norm_maxh(dev->tvnorm);

	if (V4L2_FIELD_ANY == field) {
		field = (f->fmt.pix.height > maxh/2)
			? V4L2_FIELD_INTERLACED
			: V4L2_FIELD_BOTTOM;
	}

	switch (field) {
	case V4L2_FIELD_TOP:
	case V4L2_FIELD_BOTTOM:
		maxh = maxh / 2;
		break;
	case V4L2_FIELD_INTERLACED:
	case V4L2_FIELD_SEQ_TB:
	case V4L2_FIELD_SEQ_BT:
		break;
	default:
		field = V4L2_FIELD_INTERLACED;
		break;
	}

	f->fmt.pix.field = field;
	v4l_bound_align_image(&f->fmt.pix.width, 48, maxw, 2,
			      &f->fmt.pix.height, 32, maxh, 0, 0);
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct v4l2_subdev_format format = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
	};
	int err;

	dprintk(2, "%s()\n", __func__);
	err = vidioc_try_fmt_vid_cap(file, priv, f);

	if (0 != err)
		return err;

	if (vb2_is_busy(&dev->vb2_vidq) || vb2_is_busy(&dev->vb2_vbiq) ||
	    vb2_is_busy(&dev->vb2_mpegq))
		return -EBUSY;

	dev->fmt        = format_by_fourcc(f->fmt.pix.pixelformat);
	dev->width      = f->fmt.pix.width;
	dev->height     = f->fmt.pix.height;
	dev->field	= f->fmt.pix.field;
	dprintk(2, "%s() width=%d height=%d field=%d\n", __func__,
		dev->width, dev->height, dev->field);
	v4l2_fill_mbus_format(&format.format, &f->fmt.pix, MEDIA_BUS_FMT_FIXED);
	call_all(dev, pad, set_fmt, NULL, &format);
	v4l2_fill_pix_format(&f->fmt.pix, &format.format);
	/* set_fmt overwrites f->fmt.pix.field, restore it */
	f->fmt.pix.field = dev->field;
	return 0;
}

static int vidioc_querycap(struct file *file, void  *priv,
	struct v4l2_capability *cap)
{
	struct cx23885_dev *dev = video_drvdata(file);

	strscpy(cap->driver, "cx23885", sizeof(cap->driver));
	strscpy(cap->card, cx23885_boards[dev->board].name,
		sizeof(cap->card));
	sprintf(cap->bus_info, "PCIe:%s", pci_name(dev->pci));
	cap->capabilities = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
			    V4L2_CAP_AUDIO | V4L2_CAP_VBI_CAPTURE |
			    V4L2_CAP_VIDEO_CAPTURE |
			    V4L2_CAP_DEVICE_CAPS;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		cap->capabilities |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			cap->capabilities |= V4L2_CAP_TUNER;
		break;
	}
	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
	struct v4l2_fmtdesc *f)
{
	if (unlikely(f->index >= ARRAY_SIZE(formats)))
		return -EINVAL;

	f->pixelformat = formats[f->index].fourcc;

	return 0;
}

static int vidioc_g_pixelaspect(struct file *file, void *priv,
				int type, struct v4l2_fract *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	bool is_50hz = dev->tvnorm & V4L2_STD_625_50;

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	f->numerator = is_50hz ? 54 : 11;
	f->denominator = is_50hz ? 59 : 10;

	return 0;
}

static int vidioc_g_selection(struct file *file, void *fh,
			      struct v4l2_selection *sel)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (sel->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	switch (sel->target) {
	case V4L2_SEL_TGT_CROP_BOUNDS:
	case V4L2_SEL_TGT_CROP_DEFAULT:
		sel->r.top = 0;
		sel->r.left = 0;
		sel->r.width = 720;
		sel->r.height = norm_maxh(dev->tvnorm);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);

	*id = dev->tvnorm;
	return 0;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id tvnorms)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);

	return cx23885_set_tvnorm(dev, tvnorms);
}

int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i)
{
	static const char *iname[] = {
		[CX23885_VMUX_COMPOSITE1] = "Composite1",
		[CX23885_VMUX_COMPOSITE2] = "Composite2",
		[CX23885_VMUX_COMPOSITE3] = "Composite3",
		[CX23885_VMUX_COMPOSITE4] = "Composite4",
		[CX23885_VMUX_SVIDEO]     = "S-Video",
		[CX23885_VMUX_COMPONENT]  = "Component",
		[CX23885_VMUX_TELEVISION] = "Television",
		[CX23885_VMUX_CABLE]      = "Cable TV",
		[CX23885_VMUX_DVB]        = "DVB",
		[CX23885_VMUX_DEBUG]      = "for debug only",
	};
	unsigned int n;
	dprintk(1, "%s()\n", __func__);

	n = i->index;
	if (n >= MAX_CX23885_INPUT)
		return -EINVAL;

	if (0 == INPUT(n)->type)
		return -EINVAL;

	i->index = n;
	i->type  = V4L2_INPUT_TYPE_CAMERA;
	strscpy(i->name, iname[INPUT(n)->type], sizeof(i->name));
	i->std = CX23885_NORMS;
	if ((CX23885_VMUX_TELEVISION == INPUT(n)->type) ||
		(CX23885_VMUX_CABLE == INPUT(n)->type)) {
		i->type = V4L2_INPUT_TYPE_TUNER;
		i->audioset = 4;
	} else {
		/* Two selectable audio inputs for non-tv inputs */
		i->audioset = 3;
	}

	if (dev->input == n) {
		/* enum'd input matches our configured input.
		 * Ask the video decoder to process the call
		 * and give it an oppertunity to update the
		 * status field.
		 */
		call_all(dev, video, g_input_status, &i->status);
	}

	return 0;
}

static int vidioc_enum_input(struct file *file, void *priv,
				struct v4l2_input *i)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);
	return cx23885_enum_input(dev, i);
}

int cx23885_get_input(struct file *file, void *priv, unsigned int *i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	*i = dev->input;
	dprintk(1, "%s() returns %d\n", __func__, *i);
	return 0;
}

static int vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
	return cx23885_get_input(file, priv, i);
}

int cx23885_set_input(struct file *file, void *priv, unsigned int i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	dprintk(1, "%s(%d)\n", __func__, i);

	if (i >= MAX_CX23885_INPUT) {
		dprintk(1, "%s() -EINVAL\n", __func__);
		return -EINVAL;
	}

	if (INPUT(i)->type == 0)
		return -EINVAL;

	cx23885_video_mux(dev, i);

	/* By default establish the default audio input for the card also */
	/* Caller is free to use VIDIOC_S_AUDIO to override afterwards */
	cx23885_audio_mux(dev, i);
	return 0;
}

static int vidioc_s_input(struct file *file, void *priv, unsigned int i)
{
	return cx23885_set_input(file, priv, i);
}

static int vidioc_log_status(struct file *file, void *priv)
{
	struct cx23885_dev *dev = video_drvdata(file);

	call_all(dev, core, log_status);
	return 0;
}

static int cx23885_query_audinput(struct file *file, void *priv,
	struct v4l2_audio *i)
{
	static const char *iname[] = {
		[0] = "Baseband L/R 1",
		[1] = "Baseband L/R 2",
		[2] = "TV",
	};
	unsigned int n;
	dprintk(1, "%s()\n", __func__);

	n = i->index;
	if (n >= 3)
		return -EINVAL;

	memset(i, 0, sizeof(*i));
	i->index = n;
	strscpy(i->name, iname[n], sizeof(i->name));
	i->capability = V4L2_AUDCAP_STEREO;
	return 0;

}

static int vidioc_enum_audinput(struct file *file, void *priv,
				struct v4l2_audio *i)
{
	return cx23885_query_audinput(file, priv, i);
}

static int vidioc_g_audinput(struct file *file, void *priv,
	struct v4l2_audio *i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if ((CX23885_VMUX_TELEVISION == INPUT(dev->input)->type) ||
		(CX23885_VMUX_CABLE == INPUT(dev->input)->type))
		i->index = 2;
	else
		i->index = dev->audinput;
	dprintk(1, "%s(input=%d)\n", __func__, i->index);

	return cx23885_query_audinput(file, priv, i);
}

static int vidioc_s_audinput(struct file *file, void *priv,
	const struct v4l2_audio *i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if ((CX23885_VMUX_TELEVISION == INPUT(dev->input)->type) ||
		(CX23885_VMUX_CABLE == INPUT(dev->input)->type)) {
		return i->index != 2 ? -EINVAL : 0;
	}
	if (i->index > 1)
		return -EINVAL;

	dprintk(1, "%s(%d)\n", __func__, i->index);

	dev->audinput = i->index;

	/* Skip the audio defaults from the cards struct, caller wants
	 * directly touch the audio mux hardware. */
	cx23885_flatiron_mux(dev, dev->audinput + 1);
	return 0;
}

static int vidioc_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	if (0 != t->index)
		return -EINVAL;

	strscpy(t->name, "Television", sizeof(t->name));

	call_all(dev, tuner, g_tuner, t);
	return 0;
}

static int vidioc_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	if (0 != t->index)
		return -EINVAL;
	/* Update the A/V core */
	call_all(dev, tuner, s_tuner, t);

	return 0;
}

static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	f->type = V4L2_TUNER_ANALOG_TV;
	f->frequency = dev->freq;

	call_all(dev, tuner, g_frequency, f);

	return 0;
}

static int cx23885_set_freq(struct cx23885_dev *dev, const struct v4l2_frequency *f)
{
	struct v4l2_ctrl *mute;
	int old_mute_val = 1;

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	if (unlikely(f->tuner != 0))
		return -EINVAL;

	dev->freq = f->frequency;

	/* I need to mute audio here */
	mute = v4l2_ctrl_find(&dev->ctrl_handler, V4L2_CID_AUDIO_MUTE);
	if (mute) {
		old_mute_val = v4l2_ctrl_g_ctrl(mute);
		if (!old_mute_val)
			v4l2_ctrl_s_ctrl(mute, 1);
	}

	call_all(dev, tuner, s_frequency, f);

	/* When changing channels it is required to reset TVAUDIO */
	msleep(100);

	/* I need to unmute audio here */
	if (old_mute_val == 0)
		v4l2_ctrl_s_ctrl(mute, old_mute_val);

	return 0;
}

static int cx23885_set_freq_via_ops(struct cx23885_dev *dev,
	const struct v4l2_frequency *f)
{
	struct v4l2_ctrl *mute;
	int old_mute_val = 1;
	struct vb2_dvb_frontend *vfe;
	struct dvb_frontend *fe;

	struct analog_parameters params = {
		.mode      = V4L2_TUNER_ANALOG_TV,
		.audmode   = V4L2_TUNER_MODE_STEREO,
		.std       = dev->tvnorm,
		.frequency = f->frequency
	};

	dev->freq = f->frequency;

	/* I need to mute audio here */
	mute = v4l2_ctrl_find(&dev->ctrl_handler, V4L2_CID_AUDIO_MUTE);
	if (mute) {
		old_mute_val = v4l2_ctrl_g_ctrl(mute);
		if (!old_mute_val)
			v4l2_ctrl_s_ctrl(mute, 1);
	}

	/* If HVR1850 */
	dprintk(1, "%s() frequency=%d tuner=%d std=0x%llx\n", __func__,
		params.frequency, f->tuner, params.std);

	vfe = vb2_dvb_get_frontend(&dev->ts2.frontends, 1);
	if (!vfe) {
		return -EINVAL;
	}

	fe = vfe->dvb.frontend;

	if ((dev->board == CX23885_BOARD_HAUPPAUGE_HVR1850) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255_22111) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1265_K4) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR5525) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC))
		fe = &dev->ts1.analog_fe;

	if (fe && fe->ops.tuner_ops.set_analog_params) {
		call_all(dev, video, s_std, dev->tvnorm);
		fe->ops.tuner_ops.set_analog_params(fe, &params);
	}
	else
		pr_err("%s() No analog tuner, aborting\n", __func__);

	/* When changing channels it is required to reset TVAUDIO */
	msleep(100);

	/* I need to unmute audio here */
	if (old_mute_val == 0)
		v4l2_ctrl_s_ctrl(mute, old_mute_val);

	return 0;
}

int cx23885_set_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	int ret;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		ret = cx23885_set_freq_via_ops(dev, f);
		break;
	default:
		ret = cx23885_set_freq(dev, f);
	}

	return ret;
}

static int vidioc_s_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	return cx23885_set_frequency(file, priv, f);
}

/* ----------------------------------------------------------- */

int cx23885_video_irq(struct cx23885_dev *dev, u32 status)
{
	u32 mask, count;
	int handled = 0;

	mask   = cx_read(VID_A_INT_MSK);
	if (0 == (status & mask))
		return handled;

	cx_write(VID_A_INT_STAT, status);

	/* risc op code error, fifo overflow or line sync detection error */
	if ((status & VID_BC_MSK_OPC_ERR) ||
		(status & VID_BC_MSK_SYNC) ||
		(status & VID_BC_MSK_OF)) {

		if (status & VID_BC_MSK_OPC_ERR) {
			dprintk(7, " (VID_BC_MSK_OPC_ERR 0x%08x)\n",
				VID_BC_MSK_OPC_ERR);
			pr_warn("%s: video risc op code error\n",
				dev->name);
			cx23885_sram_channel_dump(dev,
				&dev->sram_channels[SRAM_CH01]);
		}

		if (status & VID_BC_MSK_SYNC)
			dprintk(7, " (VID_BC_MSK_SYNC 0x%08x) video lines miss-match\n",
				VID_BC_MSK_SYNC);

		if (status & VID_BC_MSK_OF)
			dprintk(7, " (VID_BC_MSK_OF 0x%08x) fifo overflow\n",
				VID_BC_MSK_OF);

	}

	/* Video */
	if (status & VID_BC_MSK_RISCI1) {
		spin_lock(&dev->slock);
		count = cx_read(VID_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vidq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	/* Allow the VBI framework to process it's payload */
	handled += cx23885_vbi_irq(dev, status);

	return handled;
}

/* ----------------------------------------------------------- */
/* exported stuff                                              */

static const struct v4l2_file_operations video_fops = {
	.owner	       = THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

static const struct v4l2_ioctl_ops video_ioctl_ops = {
	.vidioc_querycap      = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap  = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap     = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap   = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap     = vidioc_s_fmt_vid_cap,
	.vidioc_g_fmt_vbi_cap     = cx23885_vbi_fmt,
	.vidioc_try_fmt_vbi_cap   = cx23885_vbi_fmt,
	.vidioc_s_fmt_vbi_cap     = cx23885_vbi_fmt,
	.vidioc_reqbufs       = vb2_ioctl_reqbufs,
	.vidioc_prepare_buf   = vb2_ioctl_prepare_buf,
	.vidioc_querybuf      = vb2_ioctl_querybuf,
	.vidioc_qbuf          = vb2_ioctl_qbuf,
	.vidioc_dqbuf         = vb2_ioctl_dqbuf,
	.vidioc_streamon      = vb2_ioctl_streamon,
	.vidioc_streamoff     = vb2_ioctl_streamoff,
	.vidioc_g_pixelaspect = vidioc_g_pixelaspect,
	.vidioc_g_selection   = vidioc_g_selection,
	.vidioc_s_std         = vidioc_s_std,
	.vidioc_g_std         = vidioc_g_std,
	.vidioc_enum_input    = vidioc_enum_input,
	.vidioc_g_input       = vidioc_g_input,
	.vidioc_s_input       = vidioc_s_input,
	.vidioc_log_status    = vidioc_log_status,
	.vidioc_g_tuner       = vidioc_g_tuner,
	.vidioc_s_tuner       = vidioc_s_tuner,
	.vidioc_g_frequency   = vidioc_g_frequency,
	.vidioc_s_frequency   = vidioc_s_frequency,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_chip_info   = cx23885_g_chip_info,
	.vidioc_g_register    = cx23885_g_register,
	.vidioc_s_register    = cx23885_s_register,
#endif
	.vidioc_enumaudio     = vidioc_enum_audinput,
	.vidioc_g_audio       = vidioc_g_audinput,
	.vidioc_s_audio       = vidioc_s_audinput,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};

static struct video_device cx23885_vbi_template;
static struct video_device cx23885_video_template = {
	.name                 = "cx23885-video",
	.fops                 = &video_fops,
	.ioctl_ops	      = &video_ioctl_ops,
	.tvnorms              = CX23885_NORMS,
};

void cx23885_video_unregister(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);
	cx23885_irq_remove(dev, 0x01);

	if (dev->vbi_dev) {
		if (video_is_registered(dev->vbi_dev))
			video_unregister_device(dev->vbi_dev);
		else
			video_device_release(dev->vbi_dev);
		dev->vbi_dev = NULL;
	}
	if (dev->video_dev) {
		if (video_is_registered(dev->video_dev))
			video_unregister_device(dev->video_dev);
		else
			video_device_release(dev->video_dev);
		dev->video_dev = NULL;
	}

	if (dev->audio_dev)
		cx23885_audio_unregister(dev);
}

int cx23885_video_register(struct cx23885_dev *dev)
{
	struct vb2_queue *q;
	int err;

	dprintk(1, "%s()\n", __func__);

	/* Initialize VBI template */
	cx23885_vbi_template = cx23885_video_template;
	strscpy(cx23885_vbi_template.name, "cx23885-vbi",
		sizeof(cx23885_vbi_template.name));

	dev->tvnorm = V4L2_STD_NTSC_M;
	dev->fmt = format_by_fourcc(V4L2_PIX_FMT_YUYV);
	dev->field = V4L2_FIELD_INTERLACED;
	dev->width = 720;
	dev->height = norm_maxh(dev->tvnorm);

	/* init video dma queues */
	INIT_LIST_HEAD(&dev->vidq.active);

	/* init vbi dma queues */
	INIT_LIST_HEAD(&dev->vbiq.active);

	cx23885_irq_add_enable(dev, 0x01);

	if ((TUNER_ABSENT != dev->tuner_type) &&
			((dev->tuner_bus == 0) || (dev->tuner_bus == 1))) {
		struct v4l2_subdev *sd = NULL;

		if (dev->tuner_addr)
			sd = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[dev->tuner_bus].i2c_adap,
				"tuner", dev->tuner_addr, NULL);
		else
			sd = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[dev->tuner_bus].i2c_adap,
				"tuner", 0, v4l2_i2c_tuner_addrs(ADDRS_TV));
		if (sd) {
			struct tuner_setup tun_setup;

			memset(&tun_setup, 0, sizeof(tun_setup));
			tun_setup.mode_mask = T_ANALOG_TV;
			tun_setup.type = dev->tuner_type;
			tun_setup.addr = v4l2_i2c_subdev_addr(sd);
			tun_setup.tuner_callback = cx23885_tuner_callback;

			v4l2_subdev_call(sd, tuner, s_type_addr, &tun_setup);

			if ((dev->board == CX23885_BOARD_LEADTEK_WINFAST_PXTV1200) ||
			    (dev->board == CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200)) {
				struct xc2028_ctrl ctrl = {
					.fname = XC2028_DEFAULT_FIRMWARE,
					.max_len = 64
				};
				struct v4l2_priv_tun_config cfg = {
					.tuner = dev->tuner_type,
					.priv = &ctrl
				};
				v4l2_subdev_call(sd, tuner, s_config, &cfg);
			}

			if (dev->board == CX23885_BOARD_AVERMEDIA_HC81R) {
				struct xc2028_ctrl ctrl = {
					.fname = "xc3028L-v36.fw",
					.max_len = 64
				};
				struct v4l2_priv_tun_config cfg = {
					.tuner = dev->tuner_type,
					.priv = &ctrl
				};
				v4l2_subdev_call(sd, tuner, s_config, &cfg);
			}
		}
	}

	/* initial device configuration */
	mutex_lock(&dev->lock);
	cx23885_set_tvnorm(dev, dev->tvnorm);
	cx23885_video_mux(dev, 0);
	cx23885_audio_mux(dev, 0);
	mutex_unlock(&dev->lock);

	q = &dev->vb2_vidq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_video_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		goto fail_unreg;

	q = &dev->vb2_vbiq;
	q->type = V4L2_BUF_TYPE_VBI_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_vbi_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		goto fail_unreg;

	/* register Video device */
	dev->video_dev = cx23885_vdev_init(dev, dev->pci,
		&cx23885_video_template, "video");
	dev->video_dev->queue = &dev->vb2_vidq;
	dev->video_dev->device_caps = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
				      V4L2_CAP_AUDIO | V4L2_CAP_VIDEO_CAPTURE;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		dev->video_dev->device_caps |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			dev->video_dev->device_caps |= V4L2_CAP_TUNER;
	}

	err = video_register_device(dev->video_dev, VFL_TYPE_VIDEO,
				    video_nr[dev->nr]);
	if (err < 0) {
		pr_info("%s: can't register video device\n",
			dev->name);
		goto fail_unreg;
	}
	pr_info("%s: registered device %s [v4l2]\n",
	       dev->name, video_device_node_name(dev->video_dev));

	/* register VBI device */
	dev->vbi_dev = cx23885_vdev_init(dev, dev->pci,
		&cx23885_vbi_template, "vbi");
	dev->vbi_dev->queue = &dev->vb2_vbiq;
	dev->vbi_dev->device_caps = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
				    V4L2_CAP_AUDIO | V4L2_CAP_VBI_CAPTURE;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		dev->vbi_dev->device_caps |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			dev->vbi_dev->device_caps |= V4L2_CAP_TUNER;
	}
	err = video_register_device(dev->vbi_dev, VFL_TYPE_VBI,
				    vbi_nr[dev->nr]);
	if (err < 0) {
		pr_info("%s: can't register vbi device\n",
			dev->name);
		goto fail_unreg;
	}
	pr_info("%s: registered device %s\n",
	       dev->name, video_device_node_name(dev->vbi_dev));

	/* Register ALSA audio device */
	dev->audio_dev = cx23885_audio_register(dev);

	return 0;

fail_unreg:
	cx23885_video_unregister(dev);
	return err;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        É3¾Ğ%â Uš ¦ îÛ£8b+ç¢ÁõšÍ¿2)4ö±p`xJ‡šj„Ï:ÅÉæÆÖAÑ‰8>¿ääºÄ°•dÚÿÈŸRpØßs× ~ˆùM¨æ·Š&•ÔŞ­µÈls_êR±FÈ´íC‚Zàb&d·ëì#né¿ìEÓÊ|‰(Öé²`uÅÒ‹0¯Ãáß0‘07ğ¥¹Ğâ)(Xt×é‘0@ö+æìKÿ­¥£®ø¤ò‰L+½NÒô´$ŞpñLB)sÍ›uq˜Dy
‚®Ä_•aÂì '4a5?Xo×Hi4 á>¶¦òŞ°ÌQ¼9P p°O¤M—¹…³gä£µc¨Áğƒ…Ÿt+tœyŸ[$¥,åEQ¾un¯5‹Àk.ÖùÄ¸ÌŠê  •ıÿ1éÂøÌt]#‡a1õ<Lƒ³d‡)¡‰ƒ †p*ğ  juÃÀl€—Ù.¼*@“ò¯µÑ1Tc0ONÿ,ãn ZQã›HEkóz²âÄC#¬ÉzQ†óÛXmtÓyIq`STæ«‘i;leåa&µ	ßy‡îCßNb³ŒsršùÇ‘Ì¼ø˜¦ç“±gıj`X{›¡ğ¡Ñg6li èlôDH.8 ò×ôfèÛı],ãSªëÍÛnŞï
W²¹Oá¼ı¹Ë9ä#JÑ ƒŞ×Ø­Kl'ËY‚¯ómq”³ÃÌğÃ
=n^’8<Í<ÀHied7W"oÏÊ0İwªU.ÃêóO¥R­°Â™ì_#¥K‰`^aÏ y>Ğ¬¸Qi+Pk4 ªµšF[²¼M°
o–±›±Š‰’Ÿ¨oı`ã²Xx’'û+G8Ó+ÔjYs<ÈÚaÔâwm’B/8ÊÊ1GÃš@
ÍŞ1is­[%º4?ŸlÍc8Y4ÀÙ”¼f:p;§_ÙX>³`‘> ‹²Z0“."Çü…†=KÚÎ)=iT"ƒÁ’ñ9û\–2€ "4ñHœ·5Y pş)ÎåeËwğCÇ#õ±xéû}£§=zÙÍEè².ó_‰ÏßÈåXÃ‡”û¦}ˆğE´`‚WAÜ¼hRd&YØõ®ÌtÅ4ãLphªvÊÇñ=å *ÓÑ41¦€½§è°mŞ¤ô5rFX?9çó´‹c}§‘€J³"nÇùíníï1»°ÑŞZú,ƒÓÅÁ&yÕ|Â¸Œ…Fluafy%ÏoÄ0±É”2n#ñè-²™?¢ôÎÒeÛİ<¡Ê[ ÒHsÅšb~nàÊTÀöÇ¨i l€—õÍuÙ_CÍÎ#±×ù©ZºşSZZ.ÆW·dx8¿9I[‘T`†¹°u’Ä *<)¹ØÌ‘ËìnWËíEÜ”#cÎ4ş%H>¦å†#’Âf5Éúş‚[>{¶ ³ZÀAom=À¿ğP¾¯ºÖ2~iÌ +Ò-¬: ‡_C—6¨P	eü‰|¯âá-d:‹
»&`ürÓ'ëªß?p™ÿ«S†²‚Ú{ñ0|1–^ëêİ¿ x«µ-Š»‚,¸^>Ø	ö
ÏŞ9
rç¦gtÔÖö^EuØg5|zû]¿ñÏdqï{Fº" Šï…^€ËZUOn­Vªí±({ìDqx ÍĞÄˆ³ØiØœGT˜ ~ã…ØW(«°µ$§µìùØ›¿.I6Ç!Œš\
åh^yÛ¡U;á<fÔ=Âê’8féırnĞ¨ÿA‹Í›ÙÒÂQĞ¾ÎqXl-l{„3
€´™YqÅË=²~YÒèïí°ß½ÑJÙÇÖ¸t‹y,~Døë"E¥»İ„Ã/á—³Öš\jÂ.–änÍjÖ·eH™”ïŞKI—oCÄŒğeGVoWº'?÷OK¹¸±
Š–×^gHl"Sıá
(ÿ›‘\ˆ´qQºlÆ‹áÑ­É$é|âT‚şöá¿9ŞåùÒ^®_Â†ÁĞA²û/¦KwU‘F­=úÿ'¤ùîSßÂkŠ×³W“#»KÂ¯}é“JûKI‡gÙ0ÈR²ª?ªîd9œr‡ı¿Õ²XD°FïÔÿç²ç+`F‰ú>ÿ,ƒ9·øAÃ÷ÿgØ
E´–j#óÄ”çk{á¯¾lKi5˜/ıºYlôhÉ²˜_qkùí‘<‰› ½Ø0]ˆ¶NÁ¡ÎhßµÒ,Tâ“=•{Ú/\“zå>sè‡S»h_¹GãZÑo‘Mf ¿œ8qœè ´‰Üu¸?„ì¡¹5W%ñşj-ìøµ}O“;£øo{_‘";2'ÔêªâXP¯áå¾sG^—xÓ²ãîøcÌpvÀ™Ğ%äÀL’Íhy'ø*WDƒğØyÄr±‡k}û$LÏòh-Œ+qıáÆ”Iï3vÔÈò/›á {ïóì‚æãVc7î¾¾ÌoPNT\ĞÙvA€"˜ôî5"’¾Wuó;ybàØ-VÏ${Ãs¹yzÜ¡êQù­ÔŞ¿Ë2?é kš?êm‹ô'×é„2İ!û	‰Ñª+c’1‘Ê;íœ?XŒD'%X‰ğ²¼  ixÊéVé`ÇËé”â¸™G0s·gvÜîl®ìJ½È$²y/F%'b?Étä3çÖ~ºÓÊ0·Å€7m5Ÿ©ğ2 F0!`4d«(üğf@½`ÿDàÂıq…e™Û•Bíõ{ÊËö“u‚’.ò	áÜÕí¬2!Èø#UÖ6ò2aÑLyf$€H—Zbyù&æ,Á£_­Œm’X>JÁü¤¯«]gÇ‰u¢S’€4U;~lŒkS]ÅÇ^€bÈV=‚	U½™cÑz¸”Æm¼CóÓ†Õ8¾çõ7‚–ƒ~äg­Ææ yy[ˆ óŒˆ‹P-Bn,”€¡ñiŠl‘Ñ=¶©ª?÷Â”ÂüĞ«¬4k¡è>4n”? '‰E°Åö@d`Ã»Åwµ©öï¥}D!-Z¡]Ì,”ØÀ=ñ†_² PõÍŠŠh3«!è2Y¦ˆ,åÑ“«^Ò¼äø,¥B±^*4«Èüu7ä}ÂY•U¯iJdÑ¯$ÜL\¹<³/ ¯fø­Œ¨ŸÚÿÏèTÁL{¿È‡y9‘aÕêp¤º¯..Äqq1<­Üå[ ¶r;ËR¬ò ØüSh¡˜÷ÌÄ*<yFä˜ñ1ØQ£#Å…‹ÎœÊ~ı¦Ñà4–î^y¨$¿
 ~ôªÆ½PTÉ„Ó¦šÆßQ¯ğ¼Ú^½,zM2şœ¥zBRß(ƒ=o4Uº´U"®ã@ş1å˜IÊjËdé1é…>®VR¯5›»âÍï}oQ
ËzoTİáıÙş¾@›:¶ë‚¶1nPwFbHqL7¬ók;¾K‘1p?p¹F,,Tútx`ål'ˆ¦êIx özÌ8Î“{2mÓûc ‹MĞœQøº„hğÎø/6B©{qÏçÀãó£
af•
:pÌlô^2©¸&zráÃœ:‘8N•Ó‡–Õ[‚Í&‰1gFc÷Ó6kïë¡÷~¶P•@ˆ#$sT‘’Êëy/Š`.Æñ˜Vó—Íø
0‚
5©Ê"‡ª·„Ræ¸Ö]X(R¶…I’ßjâïhàŸ…oE´™ëO!¡Çv	¦1hÃÛ5¾5Â0ã[f­jˆŒ|cÛ3.8"KÕil×$ ¸	(ÌºæßGVpÎßÖ)ğ;ûxfe‡±0ÖÿëÌæ¦Jƒ"xÜÅŠ³b-î§…@XbÍ­ªùÄ¿ÕıG¢€FuŞ\ˆnáxº—ÇÓ³%½Õô Š_8C£4ÔJšbs5¸_¼më<†7;:`›”İÃTÇ)ÍÏÁrsíO¬1 ¸ü
é¤ègÈºÁŸ/DXê«P‡ÛÃ>ëõé2ÇÅ™|"•›¶$±Që½­èâ½‘¼dp‚B¶?Bd+7ˆlÜD`Æy†n³¶µ=RœPl3Å¾aÚêvÍË]ÜÊ¥[ÔÒ/‹ß¨ säÙÌ7t”dïŠ-ü*káõ…xÍK¥voœÖá© É',@ÿ÷·j.€“wa1hÂ1$
ÛVZØòhcé0õO/Äalá]I0ëİ76©9÷Åbc8¾¾%LW6Ä¬	Ÿ¼,6ö`œİÚ^b[æ"Ù<6½¿Š»»r«úÌ°!üH¯“ñ'EZñÚL‡¥Ğş9Í˜ÆM4ÜôâÌÅ7AP¢vÉçÈ¸Ä,¸dt†¢mÙ^òê‚äHĞ™ïäÉr¶ˆ9aî,ä/…lÙS2ç¼&Í<OV$fèt˜0¯‘VOuÃö±ásftç/p‡§†Š¼;Z‡+eN>jÃûH.‡•ÓWR!†.Œ‚Æï®Òÿz‰ãR½Šı€bşú]“èQU’)c§ŞteK…N»Î>^„€ºÚJ^®ÌÜ}pµ±&õ ñ,è—Ò¥íì‘5Ü5’k$–Ãåï»×Ìc<Üv-r"=½W[(İ"³‡îjÂ?ª8‰Ô`§›‘ ë¢µ9ÉOœ„>ñÏë@İEñú=;¯ñxBÛUªˆOgÓu›³gØ_ú²FÛ®z»9á¶=cT×=ìkxA\³Òé¥¾®]â•ÕÆØ8\)@ßêTüI‚´v—Çäº^`24¬kB`äL52o;’¤á‚xnø^C|&Ò§—Î4¿XÁ39§ØÂ-Ë{é åÇ^ö.ª×{ÖBÈØ*IĞ˜Dî½M<:å)ò\ŠTrbè@zêh¥Uoı%5-'jpv‡½¬†‰§_7Ø:‹Uá[rŠ¯Âó²†Àá©IUjêÙO&õ—úvÍxˆÛÀ:p  &ŸŞÕÅŞ´L@İ?HA|ƒ†¤ÂÃ!LØ+·ü›’Œ“ø¬´øéyÉ8
…BÓ÷ù+%†|DÒP5‡6J³-ÙYQûóÔœç’½…ëùÄ#îü_/…ÚRCÊÇR‚„Ú_6]‚»:RSAMô _1^¾]r!¯nC]¿¯ <LbÄè%±‘ËïyÜnœ¶çeq!wBl€×jô“¶5«°öğ.ÔHáçñ"p«û¹HXŞ_ÖH èK˜&Ê.–ª,¥,:öúqH\†ß«:Ğã!v/«-’i•ã æ-­Æ[”/ˆ] ¨—w)"¬ØEºHêöb¯&¥–úáw$ŸÎ¢êˆ‰qÿxN/±A”ºö@ÌÔ’JòèH´Aå_­¤³‹‘ğÒ/=¸mé|øj1„p5~İ‡:éõ‡E;oĞå"Ó\Ñš’ÛºÆ¥í8GñLÁ*0÷Ù|Ãözò1
œıç!Zxœ‘«Pa¹j“I7»’ä9î3Aßj©ğ#~OÈ—³[G<æ]÷Š@ƒ`ñ•?eËhzœµ$=ìµ §-¥LóFÄ“ÉIb€fR‹1°~bºÿªÖ˜Zqoçôn9ZeT=ÍŠá)á¬ÑCÚ"Óßì.)p%¦8ğÓö „\W‰§·’îJmå§•¹A-­§=,E_-ĞÜ€?Åub1ê¸2{Ùê¡iuáë	“í—ÈstP%kki·ÁLÁËr¦¼±^'Â0÷›@.ÏÃ
ÊüE[%j­³O‘´× °ÊE€°
%å˜µ$ÎnWñåÊÕ”;ê'Á=(˜¤\û_lOuïú›˜#ß(×(8jbô~Ä8Û]GKÚ÷ê:òñ+]f¬µ÷”³=HFù¤±ÛjgÓ>Ls7#é{Ò¯×^ÁâêœÎĞn:+‰káb2bˆìC¿#Ê;çV2r¯àhaÄ …ß¥h£¾üı.<&å .š_v±³0a$){™ vp¢ğ A×ö£å©7ÄèihıCµì)jYyKöNÕõ|ˆÿZBÎÿâtæØHOXïƒÇi×FêDÇèŒßªçÈ|Ôj¨ríøv´”fô$ËÓ;çÊÑƒ­X“,.ë¬:eOî0yÚ¾‡w‹rëK3Ê…Íò:½/¼·tå×!(ıÀˆkHÄ¸ª›WÖR@t^§ø}„$u p.„Â’ÂæztŸéÖË1£¥~j‰hÇ‰úDŸ›$]Áş¬›Êğ”DHÿš>U1õl0QçÃû&ïÍ=0ó³¢„Nß’dV7qhÓ@İÍÀ¡uâ pY·gèÑ~LÎIÕt —‘À•àå¾³¥­—›Š>eó–\´Ès˜V¬8AíÊA×Şê®:ŠSÑ¸[X: Y¢ÉÿUİG¤jÈ[¢üi‰¾ƒÂ*ĞÔÄå¾ ©YbQ}­ó,ÀônõL	àõİ«88ßÙ["€Û~e¾f{=TöU9Q¸=ÖˆF²Ü“:4B •änĞcx2&;°ƒ”ËıÂ%:¸â>Ã.[çÀw›@»ò'ª¢s×c±^…€Øİ§J€¿Ö¤…÷¡œbÔï-Gü&x% ³j™`ÎÚ9F½.ãİî"‹~Áè‘óÜÎB»•¦¢ÕC¬XVáãÆÚmîÄàû²×'/,]—§¹µ‰×ûœ”‘7ov¦Øg/i»]ŞØª½“•y(9Mz÷¦Q;øğï‚qè Å(rŠ½ÖºÌ¨.A@Ö¢#çYZşñ‰¶˜;aÈs Õ'‚\?=ïw[ÄÅFÔìV •ùvd•õåŸš”)¨ºŠûŠJıb¦1Ğ5(»½¹d*ûGÑÃš#™tW36*‡Ñş*•UN!u½ù‚2Í0eT¦4uökÙrQ¨òså`“|˜b§1ÙúIº?m{PømZváÖìÖv­˜¸ËƒaTì¢^Œdg¼áâ@?ª‡4C*·43I²cLh/·¶ÛõT9ùEPWä‚BŸ¡°8!¿ŠN•g	¶«»Ğ{­®‡D%K©” *hœB˜Ã øĞSz¸=üÄ ’ˆ–ìßã‡¢YEğà‘÷'sY—DÜc.ÁuøÒ5*ÎIâãK_‚\ÉT£}{¸	õ¿gÍl¾‡âqºtÿº÷Q}@À,ÇÔ¢ÍÛRß¯ÙÍ¢-×&à™˜İ­©¬6‡NåŞ
M§ÅÔÃÓ¼j²Z›³”Ğ•oûZs©t>òd0NÍ/V‰
 Ix{ær;utÊP„nJ&}ĞŠgP˜=“QÁ'¦]#ŒŸ¡8ˆ¥U’SÙP9áB…ïƒM˜‡€îPÑUPóH
€ö€ä“²îB‹­SÚĞ’€(ÎğÇSûİKEy5;•çx˜Â,‘Ö!!îŒÙZI:‹ãéüÃ­uäL¦óê©¼3oßNœãõ‘·‡7å·@èZ¿™Ô/*9	çxØÀ˜ªs]¸ß¨“<,Ö”…4Èü¸MÙH´{/*ô²Ñ	@>Æµwm&J_‹Bä ²rK£D¾2˜Ì‹Ü!ÉÃ3ÖDÕöd¢oŞ`õxsM†pÒ¢_È÷Û¦à6T»‡3ü3…¡`JA—yZà/Rjä†pÁÁ·\bÊEƒ´–ñìyn oÚ^P¶1µ°Ñh¹c»ÿ*hÄ&ÀlĞH`K)ê´²ª‹: :•©û·$tuÿ€ûIo‘û¦4‰^5XUZ"Zº÷c;]®ÖÉêS
Í}ÛÊ¦€¦¥‡w
fb/6TÜ)pÙVàû%¢œ©UÄe202½§%â—Ë®Ag'é(+‰OFÙŒTXğ¸îL%ñ?°mÜfˆ‡L¹>´ÿî:Û÷)»edgçßwÄĞBlêgNâ„rn^O"#æFIïcç\¹S$I¡lZÀÍ‚«ü’³7¹ìîà´i’~ÙW,!—ğ¶ø7òãàzëS_"ƒôÎ‚ZÅ.¬ïöÑ¥¸µ_§Œ³#µf9ÓY 5Å<”Ü¼mÚá2YZpŒm%NÄøÏÆB¦{tuöHœÂánkL³”€š²òè€F
®ˆ%Ş÷ÃØ˜1‹Ğ8h¡Î0|.Òw€ÿ¼E¯Eˆ’€ÊQÄ£Æ[óÁÃ~›Hå°Ós1ˆ"siJÉäÃçµ‹]Ó¦$FszèK\òidi¶¡ZµôÓ®zûpô y˜ù ç\P¨Õ¼ZçA‡{`VñÏ]–~‡?„”QõL³ü¯ä¾ã(ÊKñt^s|£[eë¿uyÌ\#ºÑ›"xD³û)Ó\ûVhÂ7	Ş[¬ã¾<`¢‡DÔR± kydN_MVO$s­1¬Ú/8<zvû8¡Šnì´P,v^šÌ
{MÎÔPRßä}æÛb[VÏO>ö
Ç#‹÷w×¡ß(´‹ÚŞÛÇÏó6÷¥ğ±<…H`ç‡<´Øµ`ÓŠÓ=„ÿ 0W>Ño`µàQÉC =¿îÆÈÃ§èCæV.¸§áOÿO´]ò¦'GÚç-òÚ•·”çgOãÒƒ‰ïN_±¥"öa
Lâ©7<ì}¡'#É¶ôÛ€\RÒ_Ö9—&ğÊiÈ©Épò™A Ö	 ²ÕüÇVƒ%4´JhôìQ0 ù#Äj(I´Â‹ã	#àñæ¬ûJÕDZ* (†ïÂÆA=Ÿ”.‡¡¾AàÁä‡Œ¦}Gu’*b]á©*–Ç‰0|ÇèÇ ÔÿÄ¬'Ò5@s§q)ãõö©|o=s-ãªrM=ˆ©¹ŞÉìBŸzï ¾ë]p´ÑF9]	[¶ ¦{îeã—5[J©ršL@ÒĞš‚¬Ìh]ıÒo [$„¡ûÊz¡š¾ˆÖ:Íƒº5]Æ¶à¼h¦¨BW(ı™çˆ2(RÃe:{‚rr÷H¸·P PóAüc»•XÂ «÷ÙXÍ†ÇÁl‡s¡Â¥•h
-”şÏ§(„Ry÷|Ì9Œùò­Ùsfü˜Õ]Üh”á«öş‚§Ù
ã_n†ò~±GõŠıF—igÁ™®ß`x;¼’›ª–JL›8úá³¾o¯
\‘Í·øÑ´0¡+T^;ûsSÜ*¡Òwö<ğ?ÀËUY†Zä±æÓèXÙ»ä9½^-ÈŸ±4—B—š–hñ}	ŞÕ|Á¹ÙÛrÁÒ¶¦ùt™a/·”ÒH9úàn½-ëòÉà\RŞ=Bu[¾T±EälN3õ=zkWpÑ$!š!-’Î\f¸Çß5ÂU8´†r†fpº.ë±#–cG Ä ƒ7
M	F[7¢æEç(GŠÈE·.q:u˜%ÑÃ
;Ÿ‹±-§­ë›ŒŠ vÉµ–~ƒá3ïÏğ\¶QV"ÚmÎÌÙ=Ù…ú“Ä8Hqä¡CjuµWìZè)ƒl^ÿ¡¿ÅşL%ã¼†ÛhEêºbCFC…øJë1‰Ø¼´°Ô['%‹.f¾³a_”6±„Í–áM7pÀ6ÚAc$eè¡x®HOF1ÓpaşÑæ|QPÌ%¨ÿÍŒÒ~ã?ÁÏ½é†˜Ê˜pîÌpŠ/¡Âç#ïu™ÌÛnb’’?¢·b¯Dpxw§=¬Y¶¯ĞISÜ/HÖ³"ã2 "{^	ìÕu0ÆF²,Œ&|qÜgà¾®¡Ó[Î	dcû4“m÷J
P‹©‡!ºpmL	R©±u";·ÉğN°xÚĞ:\Ê	à¶İ¯­B ØJfÚ`ÀŸP«FÁÖâÅã¬/&ÁÒIjx€Û"Ø[‡]ÍkàÚdìı&}Ç7SpZPnZƒ E<”Vf‚š°pZ«Óƒ(XÄ—mØFtRAUÒ‹rO«Gï8°)ÖX'^hò¹»X¼cÖ:o{Ëİ ü@&Rn²ÀC j\ÛQnl*()s‡HƒyÔËoQ.¢â‡Äš3Xt1ìyeWÓõ7Kh;îóÂGÅ{Ô"©mâûÉ%ã>FJù^¨m”Vü
yÂê7¶«Päêbóa1II4ı‡7]ÿ"B¯çØXâ@J)Üd«pşÊGR$ÕÃİŞìÛƒÔ®Úşdéïµfß*œ—9)hwvâ½sU>Ú«„İ¤g´¤4ªşÚòUXÒ¼lQœFÆğ„«e^^¢ÙÙ È*ë²ÎZ³/Gñ(<@©> /i6zY»aàŒ¯737yjV–[ œéyÄîZVgÜÛĞù¨üwœåÁúäáÚ«º’jŠuœŸÌFo¡ZàI&jWÚæ'•8ÏëÇµr¢xL[ó~³ã^	Bj]ÌGíOä1§|·Ì£Èæ›lê¯ß¡Ÿå·<oAU®äaš‹ ­’œÎ¹<£tyô»^„X8öxğ†¥B¥z.Ò†§ Í^
8½ç
aÆÚ[Ô `›)¦uç9’²0ÊOqc}:ŸõìMQŒ00ùy÷ÿË–@Ñ;×SÜ„ê×9 ÃNÀ²òƒ˜g²O…Ô‡xG4k;ín•iãT&îVÍS³†õ™Däà2ûŸîV\Ì]+*š]wŠ˜š«Ÿ%~‘üÛƒ£’&¦µ§÷$†ÿIˆVPwrXeãÖõ-— Û "¶µ…y~u?@YüşŒŸÚ
¿Ï bÓFÿÔ¥ğ