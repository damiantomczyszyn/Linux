>drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(dev, buf);
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_buf_queue(&dev->ts1, buf);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->ts1.mpegq;
	unsigned long flags;
	int ret;

	ret = cx23885_initialize_codec(dev, 1);
	if (ret == 0) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		cx23885_start_dma(&dev->ts1, dmaq, buf);
		return 0;
	}
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_QUEUED);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
	return ret;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;

	/* stop mpeg capture */
	cx23885_api_cmd(dev, CX2341X_ENC_STOP_CAPTURE, 3, 0,
			CX23885_END_NOW, CX23885_MPEG_CAPTURE,
			CX23885_RAW_BITS_NONE);

	msleep(500);
	cx23885_417_check_encoder(dev);
	cx23885_cancel_buffers(&dev->ts1);
}

static const struct vb2_ops cx23885_qops = {
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

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct cx23885_dev *dev = video_drvdata(file);

	*id = dev->tvnorm;
	return 0;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id id)
{
	struct cx23885_dev *dev = video_drvdata(file);
	unsigned int i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(cx23885_tvnorms); i++)
		if (id & cx23885_tvnorms[i].id)
			break;
	if (i == ARRAY_SIZE(cx23885_tvnorms))
		return -EINVAL;

	ret = cx23885_set_tvnorm(dev, id);
	if (!ret)
		dev->encodernorm = cx23885_tvnorms[i];
	return ret;
}

static int vidioc_enum_input(struct file *file, void *priv,
	struct v4l2_input *i)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);
	return cx23885_enum_input(dev, i);
}

static int vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
	return cx23885_get_input(file, priv, i);
}

static int vidioc_s_input(struct file *file, void *priv, unsigned int i)
{
	return cx23885_set_input(file, priv, i);
}

static int vidioc_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;
	if (0 != t->index)
		return -EINVAL;
	strscpy(t->name, "Television", sizeof(t->name));
	call_all(dev, tuner, g_tuner, t);

	dprintk(1, "VIDIOC_G_TUNER: tuner type %d\n", t->type);

	return 0;
}

static int vidioc_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;

	/* Update the A/V core */
	call_all(dev, tuner, s_tuner, t);

	return 0;
}

static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;
	f->type = V4L2_TUNER_ANALOG_TV;
	f->frequency = dev->freq;

	call_all(dev, tuner, g_frequency, f);

	return 0;
}

static int vidioc_s_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	return cx23885_set_frequency(file, priv, f);
}

static int vidioc_querycap(struct file *file, void  *priv,
				struct v4l2_capability *cap)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct cx23885_tsport  *tsport = &dev->ts1;

	strscpy(cap->driver, dev->name, sizeof(cap->driver));
	strscpy(cap->card, cx23885_boards[tsport->dev->board].name,
		sizeof(cap->card));
	sprintf(cap->bus_info, "PCIe:%s", pci_name(dev->pci));
	cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE |
			    V4L2_CAP_STREAMING | V4L2_CAP_VBI_CAPTURE |
			    V4L2_CAP_AUDIO | V4L2_CAP_DEVICE_CAPS;
	if (dev->tuner_type != TUNER_ABSENT)
		cap->capabilities |= V4L2_CAP_TUNER;

	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	if (f->index != 0)
		return -EINVAL;

	f->pixelformat = V4L2_PIX_FMT_MPEG;

	return 0;
}

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.width        = dev->ts1.width;
	f->fmt.pix.height       = dev->ts1.height;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_G_FMT: w: %d, h: %d\n",
		dev->ts1.width, dev->ts1.height);
	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_TRY_FMT: w: %d, h: %d\n",
		dev->ts1.width, dev->ts1.height);
	return 0;
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_S_FMT: w: %d, h: %d, f: %d\n",
		f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.field);
	return 0;
}

static int vidioc_log_status(struct file *file, void *priv)
{
	struct cx23885_dev *dev = video_drvdata(file);
	char name[32 + 2];

	snprintf(name, sizeof(name), "%s/2", dev->name);
	call_all(dev, core, log_status);
	v4l2_ctrl_handler_log_status(&dev->cxhdl.hdl, name);
	return 0;
}

static const struct v4l2_file_operations mpeg_fops = {
	.owner	       = THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

static const struct v4l2_ioctl_ops mpeg_ioctl_ops = {
	.vidioc_g_std		 = vidioc_g_std,
	.vidioc_s_std		 = vidioc_s_std,
	.vidioc_enum_input	 = vidioc_enum_input,
	.vidioc_g_input		 = vidioc_g_input,
	.vidioc_s_input		 = vidioc_s_input,
	.vidioc_g_tuner		 = vidioc_g_tuner,
	.vidioc_s_tuner		 = vidioc_s_tuner,
	.vidioc_g_frequency	 = vidioc_g_frequency,
	.vidioc_s_frequency	 = vidioc_s_frequency,
	.vidioc_querycap	 = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap	 = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap	 = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap	 = vidioc_s_fmt_vid_cap,
	.vidioc_reqbufs       = vb2_ioctl_reqbufs,
	.vidioc_prepare_buf   = vb2_ioctl_prepare_buf,
	.vidioc_querybuf      = vb2_ioctl_querybuf,
	.vidioc_qbuf          = vb2_ioctl_qbuf,
	.vidioc_dqbuf         = vb2_ioctl_dqbuf,
	.vidioc_streamon      = vb2_ioctl_streamon,
	.vidioc_streamoff     = vb2_ioctl_streamoff,
	.vidioc_log_status	 = vidioc_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_chip_info	 = cx23885_g_chip_info,
	.vidioc_g_register	 = cx23885_g_register,
	.vidioc_s_register	 = cx23885_s_register,
#endif
};

static struct video_device cx23885_mpeg_template = {
	.name          = "cx23885",
	.fops          = &mpeg_fops,
	.ioctl_ops     = &mpeg_ioctl_ops,
	.tvnorms       = CX23885_NORMS,
};

void cx23885_417_unregister(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	if (dev->v4l_device) {
		if (video_is_registered(dev->v4l_device))
			video_unregister_device(dev->v4l_device);
		else
			video_device_release(dev->v4l_device);
		v4l2_ctrl_handler_free(&dev->cxhdl.hdl);
		dev->v4l_device = NULL;
	}
}

static struct video_device *cx23885_video_dev_alloc(
	struct cx23885_tsport *tsport,
	struct pci_dev *pci,
	struct video_device *template,
	char *type)
{
	struct video_device *vfd;
	struct cx23885_dev *dev = tsport->dev;

	dprintk(1, "%s()\n", __func__);

	vfd = video_device_alloc();
	if (NULL == vfd)
		return NULL;
	*vfd = *template;
	snprintf(vfd->name, sizeof(vfd->name), "%s (%s)",
		cx23885_boards[tsport->dev->board].name, type);
	vfd->v4l2_dev = &dev->v4l2_dev;
	vfd->release = video_device_release;
	return vfd;
}

int cx23885_417_register(struct cx23885_dev *dev)
{
	/* FIXME: Port1 hardcoded here */
	int err = -ENODEV;
	struct cx23885_tsport *tsport = &dev->ts1;
	struct vb2_queue *q;

	dprintk(1, "%s()\n", __func__);

	if (cx23885_boards[dev->board].portb != CX23885_MPEG_ENCODER)
		return err;

	/* Set default TV standard */
	dev->encodernorm = cx23885_tvnorms[0];

	if (dev->encodernorm.id & V4L2_STD_525_60)
		tsport->height = 480;
	else
		tsport->height = 576;

	tsport->width = 720;
	dev->cxhdl.port = CX2341X_PORT_SERIAL;
	err = cx2341x_handler_init(&dev->cxhdl, 50);
	if (err)
		return err;
	dev->cxhdl.priv = dev;
	dev->cxhdl.func = cx23885_api_func;
	cx2341x_handler_set_50hz(&dev->cxhdl, tsport->height == 576);
	v4l2_ctrl_add_handler(&dev->ctrl_handler, &dev->cxhdl.hdl, NULL, false);

	/* Allocate and initialize V4L video device */
	dev->v4l_device = cx23885_video_dev_alloc(tsport,
		dev->pci, &cx23885_mpeg_template, "mpeg");
	q = &dev->vb2_mpegq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		return err;
	video_set_drvdata(dev->v4l_device, dev);
	dev->v4l_device->lock = &dev->lock;
	dev->v4l_device->queue = q;
	dev->v4l_device->device_caps = V4L2_CAP_VIDEO_CAPTURE |
				       V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
	if (dev->tuner_type != TUNER_ABSENT)
		dev->v4l_device->device_caps |= V4L2_CAP_TUNER;
	err = video_register_device(dev->v4l_device,
		VFL_TYPE_VIDEO, -1);
	if (err < 0) {
		pr_info("%s: can't register mpeg device\n", dev->name);
		return err;
	}

	pr_info("%s: registered device %s [mpeg]\n",
	       dev->name, video_device_node_name(dev->v4l_device));

	/* ST: Configure the encoder parameters, but don't begin
	 * encoding, this resolves an issue where the first time the
	 * encoder is started video can be choppy.
	 */
	cx23885_initialize_codec(dev, 0);

	return 0;
}

MODULE_FIRMWARE(CX23885_FIRM_IMAGE_NAME);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *  Support for a cx23417 mpeg encoder via cx23885 host port.
 *
 *    (c) 2004 Jelle Foks <jelle@foks.us>
 *    (c) 2004 Gerd Knorr <kraxel@bytesex.org>
 *    (c) 2008 Steven Toth <stoth@linuxtv.org>
 *      - CX23885/7/8 support
 *
 *  Includes parts from the ivtv driver <http://sourceforge.net/projects/ivtv/>
 */

#include "cx23885.h"
#include "cx23885-ioctl.h"

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/slab.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/drv-intf/cx2341x.h>

#define CX23885_FIRM_IMAGE_SIZE 376836
#define CX23885_FIRM_IMAGE_NAME "v4l-cx23885-enc.fw"

static unsigned int mpegbufs = 32;
module_param(mpegbufs, int, 0644);
MODULE_PARM_DESC(mpegbufs, "number of mpeg buffers, range 2-32");
static unsigned int mpeglines = 32;
module_param(mpeglines, int, 0644);
MODULE_PARM_DESC(mpeglines, "number of lines in an MPEG buffer, range 2-32");
static unsigned int mpeglinesize = 512;
module_param(mpeglinesize, int, 0644);
MODULE_PARM_DESC(mpeglinesize,
	"number of bytes in each line of an MPEG buffer, range 512-1024");

static unsigned int v4l_debug;
module_param(v4l_debug, int, 0644);
MODULE_PARM_DESC(v4l_debug, "enable V4L debug messages");

#define dprintk(level, fmt, arg...)\
	do { if (v4l_debug >= level) \
		printk(KERN_DEBUG pr_fmt("%s: 417:" fmt), \
			__func__, ##arg); \
	} while (0)

static struct cx23885_tvnorm cx23885_tvnorms[] = {
	{
		.name      = "NTSC-M",
		.id        = V4L2_STD_NTSC_M,
	}, {
		.name      = "NTSC-JP",
		.id        = V4L2_STD_NTSC_M_JP,
	}, {
		.name      = "PAL-BG",
		.id        = V4L2_STD_PAL_BG,
	}, {
		.name      = "PAL-DK",
		.id        = V4L2_STD_PAL_DK,
	}, {
		.name      = "PAL-I",
		.id        = V4L2_STD_PAL_I,
	}, {
		.name      = "PAL-M",
		.id        = V4L2_STD_PAL_M,
	}, {
		.name      = "PAL-N",
		.id        = V4L2_STD_PAL_N,
	}, {
		.name      = "PAL-Nc",
		.id        = V4L2_STD_PAL_Nc,
	}, {
		.name      = "PAL-60",
		.id        = V4L2_STD_PAL_60,
	}, {
		.name      = "SECAM-L",
		.id        = V4L2_STD_SECAM_L,
	}, {
		.name      = "SECAM-DK",
		.id        = V4L2_STD_SECAM_DK,
	}
};

/* ------------------------------------------------------------------ */
enum cx23885_capture_type {
	CX23885_MPEG_CAPTURE,
	CX23885_RAW_CAPTURE,
	CX23885_RAW_PASSTHRU_CAPTURE
};
enum cx23885_capture_bits {
	CX23885_RAW_BITS_NONE             = 0x00,
	CX23885_RAW_BITS_YUV_CAPTURE      = 0x01,
	CX23885_RAW_BITS_PCM_CAPTURE      = 0x02,
	CX23885_RAW_BITS_VBI_CAPTURE      = 0x04,
	CX23885_RAW_BITS_PASSTHRU_CAPTURE = 0x08,
	CX23885_RAW_BITS_TO_HOST_CAPTURE  = 0x10
};
enum cx23885_capture_end {
	CX23885_END_AT_GOP, /* stop at the end of gop, generate irq */
	CX23885_END_NOW, /* stop immediately, no irq */
};
enum cx23885_framerate {
	CX23885_FRAMERATE_NTSC_30, /* NTSC: 30fps */
	CX23885_FRAMERATE_PAL_25   /* PAL: 25fps */
};
enum cx23885_stream_port {
	CX23885_OUTPUT_PORT_MEMORY,
	CX23885_OUTPUT_PORT_STREAMING,
	CX23885_OUTPUT_PORT_SERIAL
};
enum cx23885_data_xfer_status {
	CX23885_MORE_BUFFERS_FOLLOW,
	CX23885_LAST_BUFFER,
};
enum cx23885_picture_mask {
	CX23885_PICTURE_MASK_NONE,
	CX23885_PICTURE_MASK_I_FRAMES,
	CX23885_PICTURE_MASK_I_P_FRAMES = 0x3,
	CX23885_PICTURE_MASK_ALL_FRAMES = 0x7,
};
enum cx23885_vbi_mode_bits {
	CX23885_VBI_BITS_SLICED,
	CX23885_VBI_BITS_RAW,
};
enum cx23885_vbi_insertion_bits {
	CX23885_VBI_BITS_INSERT_IN_XTENSION_USR_DATA,
	CX23885_VBI_BITS_INSERT_IN_PRIVATE_PACKETS = 0x1 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM = 0x2 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_USR_DATA = 0x4 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_PRV_DATA = 0x5 << 1,
};
enum cx23885_dma_unit {
	CX23885_DMA_BYTES,
	CX23885_DMA_FRAMES,
};
enum cx23885_dma_transfer_status_bits {
	CX23885_DMA_TRANSFER_BITS_DONE = 0x01,
	CX23885_DMA_TRANSFER_BITS_ERROR = 0x04,
	CX23885_DMA_TRANSFER_BITS_LL_ERROR = 0x10,
};
enum cx23885_pause {
	CX23885_PAUSE_ENCODING,
	CX23885_RESUME_ENCODING,
};
enum cx23885_copyright {
	CX23885_COPYRIGHT_OFF,
	CX23885_COPYRIGHT_ON,
};
enum cx23885_notification_type {
	CX23885_NOTIFICATION_REFRESH,
};
enum cx23885_notification_status {
	CX23885_NOTIFICATION_OFF,
	CX23885_NOTIFICATION_ON,
};
enum cx23885_notification_mailbox {
	CX23885_NOTIFICATION_NO_MAILBOX = -1,
};
enum cx23885_field1_lines {
	CX23885_FIELD1_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD1_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD1_MICRONAS = 0x0105, /* 261 */
};
enum cx23885_field2_lines {
	CX23885_FIELD2_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD2_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD2_MICRONAS = 0x0106, /* 262 */
};
enum cx23885_custom_data_type {
	CX23885_CUSTOM_EXTENSION_USR_DATA,
	CX23885_CUSTOM_PRIVATE_PACKET,
};
enum cx23885_mute {
	CX23885_UNMUTE,
	CX23885_MUTE,
};
enum cx23885_mute_video_mask {
	CX23885_MUTE_VIDEO_V_MASK = 0x0000FF00,
	CX23885_MUTE_VIDEO_U_MASK = 0x00FF0000,
	CX23885_MUTE_VIDEO_Y_MASK = 0xFF000000,
};
enum cx23885_mute_video_shift {
	CX23885_MUTE_VIDEO_V_SHIFT = 8,
	CX23885_MUTE_VIDEO_U_SHIFT = 16,
	CX23885_MUTE_VIDEO_Y_SHIFT = 24,
};

/* defines below are from ivtv-driver.h */
#define IVTV_CMD_HW_BLOCKS_RST 0xFFFFFFFF

/* Firmware API commands */
#define IVTV_API_STD_TIMEOUT 500

/* Registers */
/* IVTV_REG_OFFSET */
#define IVTV_REG_ENC_SDRAM_REFRESH (0x07F8)
#define IVTV_REG_ENC_SDRAM_PRECHARGE (0x07FC)
#define IVTV_REG_SPU (0x9050)
#define IVTV_REG_HW_BLOCKS (0x9054)
#define IVTV_REG_VPU (0x9058)
#define IVTV_REG_APU (0xA064)

/**** Bit definitions for MC417_RWD and MC417_OEN registers  ***
  bits 31-16
+-----------+
| Reserved  |
+-----------+
  bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
+-------+-------+-------+-------+-------+-------+-------+-------+
| MIWR# | MIRD# | MICS# |MIRDY# |MIADDR3|MIADDR2|MIADDR1|MIADDR0|
+-------+-------+-------+-------+-------+-------+-------+-------+
 bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
+-------+-------+-------+-------+-------+-------+-------+-------+
|MIDATA7|MIDATA6|MIDATA5|MIDATA4|MIDATA3|MIDATA2|MIDATA1|MIDATA0|
+-------+-------+-------+-------+-------+-------+-------+-------+
***/
#define MC417_MIWR	0x8000
#define MC417_MIRD	0x4000
#define MC417_MICS	0x2000
#define MC417_MIRDY	0x1000
#define MC417_MIADDR	0x0F00
#define MC417_MIDATA	0x00FF

/* MIADDR* nibble definitions */
#define  MCI_MEMORY_DATA_BYTE0          0x000
#define  MCI_MEMORY_DATA_BYTE1          0x100
#define  MCI_MEMORY_DATA_BYTE2          0x200
#define  MCI_MEMORY_DATA_BYTE3          0x300
#define  MCI_MEMORY_ADDRESS_BYTE2       0x400
#define  MCI_MEMORY_ADDRESS_BYTE1       0x500
#define  MCI_MEMORY_ADDRESS_BYTE0       0x600
#define  MCI_REGISTER_DATA_BYTE0        0x800
#define  MCI_REGISTER_DATA_BYTE1        0x900
#define  MCI_REGISTER_DATA_BYTE2        0xA00
#define  MCI_REGISTER_DATA_BYTE3        0xB00
#define  MCI_REGISTER_ADDRESS_BYTE0     0xC00
#define  MCI_REGISTER_ADDRESS_BYTE1     0xD00
#define  MCI_REGISTER_MODE              0xE00

/* Read and write modes */
#define  MCI_MODE_REGISTER_READ         0
#define  MCI_MODE_REGISTER_WRITE        1
#define  MCI_MODE_MEMORY_READ           0
#define  MCI_MODE_MEMORY_WRITE          0x40

/*** Bit definitions for MC417_CTL register ****
 bits 31-6   bits 5-4   bit 3    bits 2-1       Bit 0
+--------+-------------+--------+--------------+------------+
|Reserved|MC417_SPD_CTL|Reserved|MC417_GPIO_SEL|UART_GPIO_EN|
+--------+-------------+--------+--------------+------------+
***/
#define MC417_SPD_CTL(x)	(((x) << 4) & 0x00000030)
#define MC417_GPIO_SEL(x)	(((x) << 1) & 0x00000006)
#define MC417_UART_GPIO_EN	0x00000001

/* Values for speed control */
#define MC417_SPD_CTL_SLOW	0x1
#define MC417_SPD_CTL_MEDIUM	0x0
#define MC417_SPD_CTL_FAST	0x3     /* b'1x, but we use b'11 */

/* Values for GPIO select */
#define MC417_GPIO_SEL_GPIO3	0x3
#define MC417_GPIO_SEL_GPIO2	0x2
#define MC417_GPIO_SEL_GPIO1	0x1
#define MC417_GPIO_SEL_GPIO0	0x0

void cx23885_mc417_init(struct cx23885_dev *dev)
{
	u32 regval;

	dprintk(2, "%s()\n", __func__);

	/* Configure MC417_CTL register to defaults. */
	regval = MC417_SPD_CTL(MC417_SPD_CTL_FAST)	|
		 MC417_GPIO_SEL(MC417_GPIO_SEL_GPIO3)	|
		 MC417_UART_GPIO_EN;
	cx_write(MC417_CTL, regval);

	/* Configure MC417_OEN to defaults. */
	regval = MC417_MIRDY;
	cx_write(MC417_OEN, regval);

	/* Configure MC417_RWD to defaults. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS;
	cx_write(MC417_RWD, regval);
}

static int mc417_wait_ready(struct cx23885_dev *dev)
{
	u32 mi_ready;
	unsigned long timeout = jiffies + msecs_to_jiffies(1);

	for (;;) {
		mi_ready = cx_read(MC417_RWD) & MC417_MIRDY;
		if (mi_ready != 0)
			return 0;
		if (time_after(jiffies, timeout))
			return -1;
		udelay(1);
	}
}

int mc417_register_write(struct cx23885_dev *dev, u16 address, u32 value)
{
	u32 regval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0 |
		(value & 0x000000FF);
	cx_write(MC417_RWD, regval);

	/* Transition CS/WR to effect write transaction across bus. */
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1 |
		((value >> 8) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2 |
		((value >> 16) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3 |
		((value >> 24) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Indicate that this is a write. */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_MODE |
		MCI_MODE_REGISTER_WRITE;
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	return mc417_wait_ready(dev);
}

int mc417_register_read(struct cx23885_dev *dev, u16 address, u32 *value)
{
	int retval;
	u32 regval;
	u32 tempval;
	u32 dataval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE0 |
		((address & 0x00FF));
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Indicate that this is a register read. */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_MODE |
		MCI_MODE_REGISTER_READ;
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	retval = mc417_wait_ready(dev);

	/* switch the DAT0-7 GPIO[10:3] to input mode */
	cx_write(MC417_OEN, MC417_MIRDY | MC417_MIDATA);

	/* Read data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0;
	cx_write(MC417_RWD, regval);

	/* Transition RD to effect read transaction across bus.
	 * Transition 0x5000 -> 0x9000 correct (RD/RDY -> WR/RDY)?
	 * Should it be 0x9000 -> 0xF000 (also why is RDY being set, its
	 * input only...)
	 */
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0;
	cx_write(MC417_RWD, regval);

	/* Collect byte */
	tempval = cx_read(MC417_RWD);
	dataval = tempval & 0x000000FF;

	/* Bring CS and RD high. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 8);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 16);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 24);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	*value  = dataval;

	return retval;
}

int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value)
{
	u32 regval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0 |
		(value & 0x000000FF);
	cx_write(MC417_RWD, regval);

	/* Transition CS/WR to effect write transaction across bus. */
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1 |
		((value >> 8) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2 |
		((value >> 16) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3 |
		((value >> 24) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE2 |
		MCI_MODE_MEMORY_WRITE | ((address >> 16) & 0x3F);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	return mc417_wait_ready(dev);
}

int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value)
{
	int retval;
	u32 regval;
	u32 tempval;
	u32 dataval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write address byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE2 |
		MCI_MODE_MEMORY_READ | ((address >> 16) & 0x3F);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	retval = mc417_wait_ready(dev);

	/* switch the DAT0-7 GPIO[10:3] to input mode */
	cx_write(MC417_OEN, MC417_MIRDY | MC417_MIDATA);

	/* Read data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3;
	cx_write(MC417_RWD, regval);

	/* Transition RD to effect read transaction across bus. */
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3;
	cx_write(MC417_RWD, regval);

	/* Collect byte */
	tempval = cx_read(MC417_RWD);
	dataval = ((tempval & 0x000000FF) << 24);

	/* Bring CS and RD high. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 16);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 8);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= (tempval & 0x000000FF);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	*value  = dataval;

	return retval;
}

void mc417_gpio_set(struct cx23885_dev *dev, u32 mask)
{
	u32 val;

	/* Set the gpio value */
	mc417_register_read(dev, 0x900C, &val);
	val |= (mask & 0x000ffff);
	mc417_register_write(dev, 0x900C, val);
}

void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask)
{
	u32 val;

	/* Clear the gpio value */
	mc417_register_read(dev, 0x900C, &val);
	val &= ~(mask & 0x0000ffff);
	mc417_register_write(dev, 0x900C, val);
}

void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput)
{
	u32 val;

	/* Enable GPIO direction bits */
	mc417_register_read(dev, 0x9020, &val);
	if (asoutput)
		val |= (mask & 0x0000ffff);
	else
		val &= ~(mask & 0x0000ffff);

	mc417_register_write(dev, 0x9020, val);
}
/* ------------------------------------------------------------------ */

/* MPEG encoder API */
static char *cmd_to_str(int cmd)
{
	switch (cmd) {
	case CX2341X_ENC_PING_FW:
		return  "PING_FW";
	case CX2341X_ENC_START_CAPTURE:
		return  "START_CAPTURE";
	case CX2341X_ENC_STOP_CAPTURE:
		return  "STOP_CAPTURE";
	case CX2341X_ENC_SET_AUDIO_ID:
		return  "SET_AUDIO_ID";
	case CX2341X_ENC_SET_VIDEO_ID:
		return  "SET_VIDEO_ID";
	case CX2341X_ENC_SET_PCR_ID:
		return  "SET_PCR_ID";
	case CX2341X_ENC_SET_FRAME_RATE:
		return  "SET_FRAME_RATE";
	case CX2341X_ENC_SET_FRAME_SIZE:
		return  "SET_FRAME_SIZE";
	case CX2341X_ENC_SET_BIT_RATE:
		return  "SET_BIT_RATE";
	case CX2341X_ENC_SET_GOP_PROPERTIES:
		return  "SET_GOP_PROPERTIES";
	case CX2341X_ENC_SET_ASPECT_RATIO:
		return  "SET_ASPECT_RATIO";
	case CX2341X_ENC_SET_DNR_FILTER_MODE:
		return  "SET_DNR_FILTER_MODE";
	case CX2341X_ENC_SET_DNR_FILTER_PROPS:
		return  "SET_DNR_FILTER_PROPS";
	case CX2341X_ENC_SET_CORING_LEVELS:
		return  "SET_CORING_LEVELS";
	case CX2341X_ENC_SET_SPATIAL_FILTER_TYPE:
		return  "SET_SPATIAL_FILTER_TYPE";
	case CX2341X_ENC_SET_VBI_LINE:
		return  "SET_VBI_LINE";
	case CX2341X_ENC_SET_STREAM_TYPE:
		return  "SET_STREAM_TYPE";
	case CX2341X_ENC_SET_OUTPUT_PORT:
		return  "SET_OUTPUT_PORT";
	case CX2341X_ENC_SET_AUDIO_PROPERTIES:
		return  "SET_AUDIO_PROPERTIES";
	case CX2341X_ENC_HALT_FW:
		return  "HALT_FW";
	case CX2341X_ENC_GET_VERSION:
		return  "GET_VERSION";
	case CX2341X_ENC_SET_GOP_CLOSURE:
		return  "SET_GOP_CLOSURE";
	case CX2341X_ENC_GET_SEQ_END:
		return  "GET_SEQ_END";
	case CX2341X_ENC_SET_PGM_INDEX_INFO:
		return  "SET_PGM_INDEX_INFO";
	case CX2341X_ENC_SET_VBI_CONFIG:
		return  "SET_VBI_CONFIG";
	case CX2341X_ENC_SET_DMA_BLOCK_SIZE:
		return  "SET_DMA_BLOCK_SIZE";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_10:
		return  "GET_PREV_DMA_INFO_MB_10";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_9:
		return  "GET_PREV_DMA_INFO_MB_9";
	case CX2341X_ENC_SCHED_DMA_TO_HOST:
		return  "SCHED_DMA_TO_HOST";
	case CX2341X_ENC_INITIALIZE_INPUT:
		return  "INITIALIZE_INPUT";
	case CX2341X_ENC_SET_FRAME_DROP_RATE:
		return  "SET_FRAME_DROP_RATE";
	case CX2341X_ENC_PAUSE_ENCODER:
		return  "PAUSE_ENCODER";
	case CX2341X_ENC_REFRESH_INPUT:
		return  "REFRESH_INPUT";
	case CX2341X_ENC_SET_COPYRIGHT:
		return  "SET_COPYRIGHT";
	case CX2341X_ENC_SET_EVENT_NOTIFICATION:
		return  "SET_EVENT_NOTIFICATION";
	case CX2341X_ENC_SET_NUM_VSYNC_LINES:
		return  "SET_NUM_VSYNC_LINES";
	case CX2341X_ENC_SET_PLACEHOLDER:
		return  "SET_PLACEHOLDER";
	case CX2341X_ENC_MUTE_VIDEO:
		return  "MUTE_VIDEO";
	case CX2341X_ENC_MUTE_AUDIO:
		return  "MUTE_AUDIO";
	case CX2341X_ENC_MISC:
		return  "MISC";
	default:
		return "UNKNOWN";
	}
}

static int cx23885_mbox_func(void *priv,
			     u32 command,
			     int in,
			     int out,
			     u32 data[CX2341X_MBOX_MAX_DATA])
{
	struct cx23885_dev *dev = priv;
	unsigned long timeout;
	u32 value, flag, retval = 0;
	int i;

	dprintk(3, "%s: command(0x%X) = %s\n", __func__, command,
		cmd_to_str(command));

	/* this may not be 100% safe if we can't read any memory location
	   without side effects */
	mc417_memory_read(dev, dev->cx23417_mailbox - 4, &value);
	if (value != 0x12345678) {
		pr_err("Firmware and/or mailbox pointer not initialized or corrupted, signature = 0x%x, cmd = %s\n",
			value, cmd_to_str(command));
		return -1;
	}

	/* This read looks at 32 bits, but flag is only 8 bits.
	 * Seems we also bail if CMD or TIMEOUT bytes are set???
	 */
	mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
	if (flag) {
		pr_err("ERROR: Mailbox appears to be in use (%x), cmd = %s\n",
		       flag, cmd_to_str(command));
		return -1;
	}

	flag |= 1; /* tell 'em we're working on it */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* write command + args + fill remaining with zeros */
	/* command code */
	mc417_memory_write(dev, dev->cx23417_mailbox + 1, command);
	mc417_memory_write(dev, dev->cx23417_mailbox + 3,
		IVTV_API_STD_TIMEOUT); /* timeout */
	for (i = 0; i < in; i++) {
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, data[i]);
		dprintk(3, "API Input %d = %d\n", i, data[i]);
	}
	for (; i < CX2341X_MBOX_MAX_DATA; i++)
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, 0);

	flag |= 3; /* tell 'em we're done writing */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* wait for firmware to handle the API command */
	timeout = jiffies + msecs_to_jiffies(10);
	for (;;) {
		mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
		if (0 != (flag & 4))
			break;
		if (time_after(jiffies, timeout)) {
			pr_err("ERROR: API Mailbox timeout\n");
			return -1;
		}
		udelay(10);
	}

	/* read output values */
	for (i = 0; i < out; i++) {
		mc417_memory_read(dev, dev->cx23417_mailbox + 4 + i, data + i);
		dprintk(3, "API Output %d = %d\n", i, data[i]);
	}

	mc417_memory_read(dev, dev->cx23417_mailbox + 2, &retval);
	dprintk(3, "API result = %d\n", retval);

	flag = 0;
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	return retval;
}

/* We don't need to call the API often, so using just one
 * mailbox will probably suffice
 */
static int cx23885_api_cmd(struct cx23885_dev *dev,
			   u32 command,
			   u32 inputcnt,
			   u32 outputcnt,
			   ...)
{
	u32 data[CX2341X_MBOX_MAX_DATA];
	va_list vargs;
	int i, err;

	dprintk(3, "%s() cmds = 0x%08x\n", __func__, command);

	va_start(vargs, outputcnt);
	for (i = 0; i < inputcnt; i++)
		data[i] = va_arg(vargs, int);

	err = cx23885_mbox_func(dev, command, inputcnt, outputcnt, data);
	for (i = 0; i < outputcnt; i++) {
		int *vptr = va_arg(vargs, int *);
		*vptr = data[i];
	}
	va_end(vargs);

	return err;
}

static int cx23885_api_func(void *priv, u32 cmd, int in, int out, u32 data[CX2341X_MBOX_MAX_DATA])
{
	return cx23885_mbox_func(priv, cmd, in, out, data);
}

static int cx23885_find_mailbox(struct cx23885_dev *dev)
{
	u32 signature[4] = {
		0x12345678, 0x34567812, 0x56781234, 0x78123456
	};
	int signaturecnt = 0;
	u32 value;
	int i;

	dprintk(2, "%s()\n", __func__);

	for (i = 0; i < CX23885_FIRM_IMAGE_SIZE; i++) {
		mc417_memory_read(dev, i, &value);
		if (value == signature[signaturecnt])
			signaturecnt++;
		else
			signaturecnt = 0;
		if (4 == signaturecnt) {
			dprintk(1, "Mailbox signature found at 0x%x\n", i+1);
			return i+1;
		}
	}
	pr_err("Mailbox signature values not found!\n");
	return -1;
}

static int cx23885_load_firmware(struct cx23885_dev *dev)
{
	static const unsigned char magic[8] = {
		0xa7, 0x0d, 0x00, 0x00, 0x66, 0xbb, 0x55, 0xaa
	};
	const struct firmware *firmware;
	int i, retval = 0;
	u32 value = 0;
	u32 gpio_output = 0;
	u32 gpio_value;
	u32 checksum = 0;
	u32 *dataptr;

	dprintk(2, "%s()\n", __func__);

	/* Save GPIO settings before reset of APU */
	retval |= mc417_memory_read(dev, 0x9020, &gpio_output);
	retval |= mc417_memory_read(dev, 0x900C, &gpio_value);

	retval  = mc417_register_write(dev,
		IVTV_REG_VPU, 0xFFFFFFED);
	retval |= mc417_register_write(dev,
		IVTV_REG_HW_BLOCKS, IVTV_CMD_HW_BLOCKS_RST);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_REFRESH, 0x80000800);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_PRECHARGE, 0x1A);
	retval |= mc417_register_write(dev,
		IVTV_REG_APU, 0);

	if (retval != 0) {
		pr_err("%s: Error with mc417_register_write\n",
			__func__);
		return -1;
	}

	retval = request_firmware(&firmware, CX23885_FIRM_IMAGE_NAME,
				  &dev->pci->dev);

	if (retval != 0) {
		pr_err("ERROR: Hotplug firmware request failed (%s).\n",
		       CX23885_FIRM_IMAGE_NAME);
		pr_err("Please fix your hotplug setup, the board will not work without firmware loaded!\n");
		return -1;
	}

	if (firmware->size != CX23885_FIRM_IMAGE_SIZE) {
		pr_err("ERROR: Firmware size mismatch (have %zu, expected %d)\n",
		       firmware->size, CX23885_FIRM_IMAGE_SIZE);
		release_firmware(firmware);
		return -1;
	}

	if (0 != memcmp(firmware->data, magic, 8)) {
		pr_err("ERROR: Firmware magic mismatch, wrong file?\n");
		release_firmware(firmware);
		return -1;
	}

	/* transfer to the chip */
	dprintk(2, "Loading firmware ...\n");
	dataptr = (u32 *)firmware->data;
	for (i = 0; i < (firmware->size >> 2); i++) {
		value = *dataptr;
		checksum += ~value;
		if (mc417_memory_write(dev, i, value) != 0) {
			pr_err("ERROR: Loading firmware failed!\n");
			release_firmware(firmware);
			return -1;
		}
		dataptr++;
	}

	/* read back to verify with the checksum */
	dprintk(1, "Verifying firmware ...\n");
	for (i--; i >= 0; i--) {
		if (mc417_memory_read(dev, i, &value) != 0) {
			pr_err("ERROR: Reading firmware failed!\n");
			release_firmware(firmware);
			return -1;
		}
		checksum -= ~value;
	}
	if (checksum) {
		pr_err("ERROR: Firmware load failed (checksum mismatch).\n");
		release_firmware(firmware);
		return -1;
	}
	release_firmware(firmware);
	dprintk(1, "Firmware upload successful.\n");

	retval |= mc417_register_write(dev, IVTV_REG_HW_BLOCKS,
		IVTV_CMD_HW_BLOCKS_RST);

	/* F/W power up disturbs the GPIOs, restore state */
	retval |= mc417_register_write(dev, 0x9020, gpio_output);
	retval |= mc417_register_write(dev, 0x900C, gpio_value);

	retval |= mc417_register_read(dev, IVTV_REG_VPU, &value);
	retval |= mc417_register_write(dev, IVTV_REG_VPU, value & 0xFFFFFFE8);

	/* Hardcoded GPIO's here */
	retval |= mc417_register_write(dev, 0x9020, 0x4000);
	retval |= mc417_register_write(dev, 0x900C, 0x4000);

	mc417_register_read(dev, 0x9020, &gpio_output);
	mc417_register_read(dev, 0x900C, &gpio_value);

	if (retval < 0)
		pr_err("%s: Error with mc417_register_write\n",
			__func__);
	return 0;
}

void cx23885_417_check_encoder(struct cx23885_dev *dev)
{
	u32 status, seq;

	status = seq = 0;
	cx23885_api_cmd(dev, CX2341X_ENC_GET_SEQ_END, 0, 2, &status, &seq);
	dprintk(1, "%s() status = %d, seq = %d\n", __func__, status, seq);
}

static void cx23885_codec_settings(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	/* Dynamically change the height based on video standard */
	if (dev->encodernorm.id & V4L2_STD_525_60)
		dev->ts1.height = 480;
	else
		dev->ts1.height = 576;

	/* assign frame size */
	cx23885_api_cmd(dev, CX2341X_ENC_SET_FRAME_SIZE, 2, 0,
				dev->ts1.height, dev->ts1.width);

	dev->cxhdl.width = dev->ts1.width;
	dev->cxhdl.height = dev->ts1.height;
	dev->cxhdl.is_50hz =
		(dev->encodernorm.id & V4L2_STD_625_50) != 0;

	cx2341x_handler_setup(&dev->cxhdl);

	cx23885_api_cmd(dev, CX2341X_ENC_MISC, 2, 0, 3, 1);
	cx23885_api_cmd(dev, CX2341X_ENC_MISC, 2, 0, 4, 1);
}

static int cx23885_initialize_codec(struct cx23885_dev *dev, int startencoder)
{
	int version;
	int retval;
	u32 i, data[7];

	dprintk(1, "%s()\n", __func__);

	retval = cx23885_api_cmd(dev, CX2341X_ENC_PING_FW, 0, 0); /* ping */
	if (retval < 0) {
		dprintk(2, "%s() PING OK\n", __func__);
		retval = cx23885_load_firmware(dev);
		if (retval < 0) {
			pr_err("%s() f/w load failed\n", __func__);
			return retval;
		}
		retval = cx23885_find_mailbox(dev);
		if (retval < 0) {
			pr_err("%s() mailbox < 0, error\n",
				__func__);
			return -1;
		}
		dev->cx23417_mailbox = retval;
		retval = cx23885_api_cmd(dev, CX2341X_ENC_PING_FW, 0, 0);
		if (retval < 0) {
			pr_err("ERROR: cx23417 firmware ping failed!\n");
			return -1;
		}
		retval = cx23885_api_cmd(dev, CX2341X_ENC_GET_VERSION, 0, 1,
			&version);
		if (retval < 0) {
			pr_err("ERROR: cx23417 firmware get encoder :version failed!\n");
			return -1;
		}
		dprintk(1, "cx23417 firmware version is 0x%08x\n", version);
		msleep(200);
	}

	cx23885_codec_settings(dev);
	msleep(60);

	cx23885_api_cmd(dev, CX2341X_ENC_SET_NUM_VSYNC_LINES, 2, 0,
		CX23885_FIELD1_SAA7115, CX23885_FIELD2_SAA7115);
	cx23885_api_cmd(dev, CX2341X_ENC_SET_PLACEHOLDER, 12, 0,
		CX23885_CUSTOM_EXTENSION_USR_DATA, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0);

	/* Setup to capture VBI */
	data[0] = 0x0001BD00;
	data[1] = 1;          /* frames per interrupt */
	data[2] = 4;          /* total bufs */
	data[3] = 0x91559155; /* start codes */
	data[4] = 0x206080C0; /* stop codes */
	data[5] = 6;          /* lines */
	data[6] = 64;         /* BPL */

	cx23885_api_cmd(dev, CX2341X_ENC_SET_VBI_CONFIG, 7, 0, data[0], data[1],
		data[2], data[3], data[4], data[5], data[6]);

	for (i = 2; i <= 24; i++) {
		int valid;

		valid = ((i >= 19) && (i <= 21));
		cx23885_api_cmd(dev, CX2341X_ENC_SET_VBI_LINE, 5, 0, i,
				valid, 0 , 0, 0);
		cx23885_api_cmd(dev, CX2341X_ENC_SET_VBI_LINE, 5, 0,
				i | 0x80000000, valid, 0, 0, 0);
	}

	cx23885_api_cmd(dev, CX2341X_ENC_MUTE_AUDIO, 1, 0, CX23885_UNMUTE);
	msleep(60);

	/* initialize the video input */
	cx23885_api_cmd(dev, CX2341X_ENC_INITIALIZE_INPUT, 0, 0);
	msleep(60);

	/* Enable VIP style pixel invalidation so we work with scaled mode */
	mc417_memory_write(dev, 2120, 0x00000080);

	/* start capturing to the host interface */
	if (startencoder) {
		cx23885_api_cmd(dev, CX2341X_ENC_START_CAPTURE, 2, 0,
			CX23885_MPEG_CAPTURE, CX23885_RAW_BITS_NONE);
		msleep(10);
	}

	return 0;
}

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;

	dev->ts1.ts_packet_size  = mpeglinesize;
	dev->ts1.ts_packet_count = mpeglines;
	*num_planes = 1;
	sizes[0] = mpeglinesize * mpeglines;
	*num_buffers = mpegbufs;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);

	return cx23885_buf_prepare(buf, &dev->ts1);
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(dev, buf);
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_buf_queue(&dev->ts1, buf);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->ts1.mpegq;
	unsigned long flags;
	int ret;

	ret = cx23885_initialize_codec(dev, 1);
	if (ret == 0) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		cx23885_start_dma(&dev->ts1, dmaq, buf);
		return 0;
	}
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_QUEUED);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
	return ret;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;

	/* stop mpeg capture */
	cx23885_api_cmd(dev, CX2341X_ENC_STOP_CAPTURE, 3, 0,
			CX23885_END_NOW, CX23885_MPEG_CAPTURE,
			CX23885_RAW_BITS_NONE);

	msleep(500);
	cx23885_417_check_encoder(dev);
	cx23885_cancel_buffers(&dev->ts1);
}

static const struct vb2_ops cx23885_qops = {
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

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct cx23885_dev *dev = video_drvdata(file);

	*id = dev->tvnorm;
	return 0;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id id)
{
	struct cx23885_dev *dev = video_drvdata(file);
	unsigned int i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(cx23885_tvnorms); i++)
		if (id & cx23885_tvnorms[i].id)
			break;
	if (i == ARRAY_SIZE(cx23885_tvnorms))
		return -EINVAL;

	ret = cx23885_set_tvnorm(dev, id);
	if (!ret)
		dev->encodernorm = cx23885_tvnorms[i];
	return ret;
}

static int vidioc_enum_input(struct file *file, void *priv,
	struct v4l2_input *i)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);
	return cx23885_enum_input(dev, i);
}

static int vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
	return cx23885_get_input(file, priv, i);
}

static int vidioc_s_input(struct file *file, void *priv, unsigned int i)
{
	return cx23885_set_input(file, priv, i);
}

static int vidioc_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;
	if (0 != t->index)
		return -EINVAL;
	strscpy(t->name, "Television", sizeof(t->name));
	call_all(dev, tuner, g_tuner, t);

	dprintk(1, "VIDIOC_G_TUNER: tuner type %d\n", t->type);

	return 0;
}

static int vidioc_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;

	/* Update the A/V core */
	call_all(dev, tuner, s_tuner, t);

	return 0;
}

static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;
	f->type = V4L2_TUNER_ANALOG_TV;
	f->frequency = dev->freq;

	call_all(dev, tuner, g_frequency, f);

	return 0;
}

static int vidioc_s_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	return cx23885_set_frequency(file, priv, f);
}

static int vidioc_querycap(struct file *file, void  *priv,
				struct v4l2_capability *cap)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct cx23885_tsport  *tsport = &dev->ts1;

	strscpy(cap->driver, dev->name, sizeof(cap->driver));
	strscpy(cap->card, cx23885_boards[tsport->dev->board].name,
		sizeof(cap->card));
	sprintf(cap->bus_info, "PCIe:%s", pci_name(dev->pci));
	cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE |
			    V4L2_CAP_STREAMING | V4L2_CAP_VBI_CAPTURE |
			    V4L2_CAP_AUDIO | V4L2_CAP_DEVICE_CAPS;
	if (dev->tuner_type != TUNER_ABSENT)
		cap->capabilities |= V4L2_CAP_TUNER;

	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	if (f->index != 0)
		return -EINVAL;

	f->pixelformat = V4L2_PIX_FMT_MPEG;

	return 0;
}

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.width        = dev->ts1.width;
	f->fmt.pix.height       = dev->ts1.height;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_G_FMT: w: %d, h: %d\n",
		dev->ts1.width, dev->ts1.height);
	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_TRY_FMT: w: %d, h: %d\n",
		dev->ts1.width, dev->ts1.height);
	return 0;
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_S_FMT: w: %d, h: %d, f: %d\n",
		f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.field);
	return 0;
}

static int vidioc_log_status(struct file *file, void *priv)
{
	struct cx23885_dev *dev = video_drvdata(file);
	char name[32 + 2];

	snprintf(name, sizeof(name), "%s/2", dev->name);
	call_all(dev, core, log_status);
	v4l2_ctrl_handler_log_status(&dev->cxhdl.hdl, name);
	return 0;
}

static const struct v4l2_file_operations mpeg_fops = {
	.owner	       = THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

static const struct v4l2_ioctl_ops mpeg_ioctl_ops = {
	.vidioc_g_std		 = vidioc_g_std,
	.vidioc_s_std		 = vidioc_s_std,
	.vidioc_enum_input	 = vidioc_enum_input,
	.vidioc_g_input		 = vidioc_g_input,
	.vidioc_s_input		 = vidioc_s_input,
	.vidioc_g_tuner		 = vidioc_g_tuner,
	.vidioc_s_tuner		 = vidioc_s_tuner,
	.vidioc_g_frequency	 = vidioc_g_frequency,
	.vidioc_s_frequency	 = vidioc_s_frequency,
	.vidioc_querycap	 = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap	 = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap	 = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap	 = vidioc_s_fmt_vid_cap,
	.vidioc_reqbufs       = vb2_ioctl_reqbufs,
	.vidioc_prepare_buf   = vb2_ioctl_prepare_buf,
	.vidioc_querybuf      = vb2_ioctl_querybuf,
	.vidioc_qbuf          = vb2_ioctl_qbuf,
	.vidioc_dqbuf         = vb2_ioctl_dqbuf,
	.vidioc_streamon      = vb2_ioctl_streamon,
	.vidioc_streamoff     = vb2_ioctl_streamoff,
	.vidioc_log_status	 = vidioc_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_chip_info	 = cx23885_g_chip_info,
	.vidioc_g_register	 = cx23885_g_register,
	.vidioc_s_register	 = cx23885_s_register,
#endif
};

static struct video_device cx23885_mpeg_template = {
	.name          = "cx23885",
	.fops          = &mpeg_fops,
	.ioctl_ops     = &mpeg_ioctl_ops,
	.tvnorms       = CX23885_NORMS,
};

void cx23885_417_unregister(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	if (dev->v4l_device) {
		if (video_is_registered(dev->v4l_device))
			video_unregister_device(dev->v4l_device);
		else
			video_device_release(dev->v4l_device);
		v4l2_ctrl_handler_free(&dev->cxhdl.hdl);
		dev->v4l_device = NULL;
	}
}

static struct video_device *cx23885_video_dev_alloc(
	struct cx23885_tsport *tsport,
	struct pci_dev *pci,
	struct video_device *template,
	char *type)
{
	struct video_device *vfd;
	struct cx23885_dev *dev = tsport->dev;

	dprintk(1, "%s()\n", __func__);

	vfd = video_device_alloc();
	if (NULL == vfd)
		return NULL;
	*vfd = *template;
	snprintf(vfd->name, sizeof(vfd->name), "%s (%s)",
		cx23885_boards[tsport->dev->board].name, type);
	vfd->v4l2_dev = &dev->v4l2_dev;
	vfd->release = video_device_release;
	return vfd;
}

int cx23885_417_register(struct cx23885_dev *dev)
{
	/* FIXME: Port1 hardcoded here */
	int err = -ENODEV;
	struct cx23885_tsport *tsport = &dev->ts1;
	struct vb2_queue *q;

	dprintk(1, "%s()\n", __func__);

	if (cx23885_boards[dev->board].portb != CX23885_MPEG_ENCODER)
		return err;

	/* Set default TV standard */
	dev->encodernorm = cx23885_tvnorms[0];

	if (dev->encodernorm.id & V4L2_STD_525_60)
		tsport->height = 480;
	else
		tsport->height = 576;

	tsport->width = 720;
	dev->cxhdl.port = CX2341X_PORT_SERIAL;
	err = cx2341x_handler_init(&dev->cxhdl, 50);
	if (err)
		return err;
	dev->cxhdl.priv = dev;
	dev->cxhdl.func = cx23885_api_func;
	cx2341x_handler_set_50hz(&dev->cxhdl, tsport->height == 576);
	v4l2_ctrl_add_handler(&dev->ctrl_handler, &dev->cxhdl.hdl, NULL, false);

	/* Allocate and initialize V4L video device */
	dev->v4l_device = cx23885_video_dev_alloc(tsport,
		dev->pci, &cx23885_mpeg_template, "mpeg");
	q = &dev->vb2_mpegq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		return err;
	video_set_drvdata(dev->v4l_device, dev);
	dev->v4l_device->lock = &dev->lock;
	dev->v4l_device->queue = q;
	dev->v4l_device->device_caps = V4L2_CAP_VIDEO_CAPTURE |
				       V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
	if (dev->tuner_type != TUNER_ABSENT)
		dev->v4l_device->device_caps |= V4L2_CAP_TUNER;
	err = video_register_device(dev->v4l_device,
		VFL_TYPE_VIDEO, -1);
	if (err < 0) {
		pr_info("%s: can't register mpeg device\n", dev->name);
		return err;
	}

	pr_info("%s: registered device %s [mpeg]\n",
	       dev->name, video_device_node_name(dev->v4l_device));

	/* ST: Configure the encoder parameters, but don't begin
	 * encoding, this resolves an issue where the first time the
	 * encoder is started video can be choppy.
	 */
	cx23885_initialize_codec(dev, 0);

	return 0;
}

MODULE_FIRMWARE(CX23885_FIRM_IMAGE_NAME);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *  Support for CX23885 analog audio capture
 *
 *    (c) 2008 Mijhail Moreyra <mijhail.moreyra@gmail.com>
 *    Adapted from cx88-alsa.c
 *    (c) 2009 Steven Toth <stoth@kernellabs.com>
 */

#include "cx23885.h"
#include "cx23885-reg.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>
#include <linux/pci.h>

#include <asm/delay.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>

#include <sound/tlv.h>

#define AUDIO_SRAM_CHANNEL	SRAM_CH07

#define dprintk(level, fmt, arg...) do {				\
	if (audio_debug + 1 > level)					\
		printk(KERN_DEBUG pr_fmt("%s: alsa: " fmt), \
			chip->dev->name, ##arg); \
} while(0)

/****************************************************************************
			Module global static vars
 ****************************************************************************/

static unsigned int disable_analog_audio;
module_param(disable_analog_audio, int, 0644);
MODULE_PARM_DESC(disable_analog_audio, "disable analog audio ALSA driver");

static unsigned int audio_debug;
module_param(audio_debug, int, 0644);
MODULE_PARM_DESC(audio_debug, "enable debug messages [analog audio]");

/****************************************************************************
			Board specific functions
 ****************************************************************************/

/* Constants taken from cx88-reg.h */
#define AUD_INT_DN_RISCI1       (1 <<  0)
#define AUD_INT_UP_RISCI1       (1 <<  1)
#define AUD_INT_RDS_DN_RISCI1   (1 <<  2)
#define AUD_INT_DN_RISCI2       (1 <<  4) /* yes, 3 is skipped */
#define AUD_INT_UP_RISCI2       (1 <<  5)
#define AUD_INT_RDS_DN_RISCI2   (1 <<  6)
#define AUD_INT_DN_SYNC         (1 << 12)
#define AUD_INT_UP_SYNC         (1 << 13)
#define AUD_INT_RDS_DN_SYNC     (1 << 14)
#define AUD_INT_OPC_ERR         (1 << 16)
#define AUD_INT_BER_IRQ         (1 << 20)
#define AUD_INT_MCHG_IRQ        (1 << 21)
#define GP_COUNT_CONTROL_RESET	0x3

static int cx23885_alsa_dma_init(struct cx23885_audio_dev *chip,
				 unsigned long nr_pages)
{
	struct cx23885_audio_buffer *buf = chip->buf;
	struct page *pg;
	int i;

	buf->vaddr = vmalloc_32(nr_pages << PAGE_SHIFT);
	if (NULL == buf->vaddr) {
		dprintk(1, "vmalloc_32(%lu pages) failed\n", nr_pages);
		return -ENOMEM;
	}

	dprintk(1, "vmalloc is at addr %p, size=%lu\n",
		buf->vaddr, nr_pages << PAGE_SHIFT);

	memset(buf->vaddr, 0, nr_pages << PAGE_SHIFT);
	buf->nr_pages = nr_pages;

	buf->sglist = vzalloc(array_size(sizeof(*buf->sglist), buf->nr_pages));
	if (NULL == buf->sglist)
		goto vzalloc_err;

	sg_init_table(buf->sglist, buf->nr_pages);
	for (i = 0; i < buf->nr_pages; i++) {
		pg = vmalloc_to_page(buf->vaddr + i * PAGE_SIZE);
		if (NULL == pg)
			goto vmalloc_to_page_err;
		sg_set_page(&buf->sglist[i], pg, PAGE_SIZE, 0);
	}
	return 0;

vmalloc_to_page_err:
	vfree(buf->sglist);
	buf->sglist = NULL;
vzalloc_err:
	vfree(buf->vaddr);
	buf->vaddr = NULL;
	return -ENOMEM;
}

static int cx23885_alsa_dma_map(struct cx23885_audio_dev *dev)
{
	struct cx23885_audio_buffer *buf = dev->buf;

	buf->sglen = dma_map_sg(&dev->pci->dev, buf->sglist,
			buf->nr_pages, DMA_FROM_DEVICE);

	if (0 == buf->sglen) {
		pr_warn("%s: cx23885_alsa_map_sg failed\n", __func__);
		return -ENOMEM;
	}
	return 0;
}

static int cx23885_alsa_dma_unmap(struct cx23885_audio_dev *dev)
{
	struct cx23885_audio_buffer *buf = dev->buf;

	if (!buf->sglen)
		return 0;

	dma_unmap_sg(&dev->pci->dev, buf->sglist, buf->nr_pages, DMA_FROM_DEVICE);
	buf->sglen = 0;
	return 0;
}

static int cx23885_alsa_dma_free(struct cx23885_audio_buffer *buf)
{
	vfree(buf->sglist);
	buf->sglist = NULL;
	vfree(buf->vaddr);
	buf->vaddr = NULL;
	return 0;
}

/*
 * BOARD Specific: Sets audio DMA
 */

static int cx23885_start_audio_dma(struct cx23885_audio_dev *chip)
{
	struct cx23885_audio_buffer *buf = chip->buf;
	struct cx23885_dev *dev  = chip->dev;
	struct sram_channel *audio_ch =
		&dev->sram_channels[AUDIO_SRAM_CHANNEL];

	dprintk(1, "%s()\n", __func__);

	/* Make sure RISC/FIFO are off before changing FIFO/RISC settings */
	cx_clear(AUD_INT_DMA_CTL, 0x11);

	/* setup fifo + format - out channel */
	cx23885_sram_channel_setup(chip->dev, audio_ch, buf->bpl,
		buf->risc.dma);

	/* sets bpl size */
	cx_write(AUD_INT_A_LNGTH, buf->bpl);

	/* This is required to get good audio (1 seems to be ok) */
	cx_write(AUD_INT_A_MODE, 1);

	/* reset counter */
	cx_write(AUD_INT_A_GPCNT_CTL, GP_COUNT_CONTROL_RESET);
	atomic_set(&chip->count, 0);

	dprintk(1, "Start audio DMA, %d B/line, %d lines/FIFO, %d periods, %d byte buffer\n",
		buf->bpl, cx_read(audio_ch->cmds_start+12)>>1,
		chip->num_periods, buf->bpl * chip->num_periods);

	/* Enables corresponding bits at AUD_INT_STAT */
	cx_write(AUDIO_INT_INT_MSK, AUD_INT_OPC_ERR | AUD_INT_DN_SYNC |
				    AUD_INT_DN_RISCI1);

	/* Clean any pending interrupt bits already set */
	cx_write(AUDIO_INT_INT_STAT, ~0);

	/* enable audio irqs */
	cx_set(PCI_INT_MSK, chip->dev->pci_irqmask | PCI_MSK_AUD_INT);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5)); /* Enables Risc Processor */
	cx_set(AUD_INT_DMA_CTL, 0x11); /* audio downstream FIFO and
					  RISC enable */
	if (audio_debug)
		cx23885_sram_channel_dump(chip->dev, audio_ch);

	return 0;
}

/*
 * BOARD Specific: Resets audio DMA
 */
static int cx23885_stop_audio_dma(struct cx23885_audio_dev *chip)
{
	struct cx23885_dev *dev = chip->dev;
	dprintk(1, "Stopping audio DMA\n");

	/* stop dma */
	cx_clear(AUD_INT_DMA_CTL, 0x11);

	/* disable irqs */
	cx_clear(PCI_INT_MSK, PCI_MSK_AUD_INT);
	cx_clear(AUDIO_INT_INT_MSK, AUD_INT_OPC_ERR | AUD_INT_DN_SYNC |
				    AUD_INT_DN_RISCI1);

	if (audio_debug)
		cx23885_sram_channel_dump(chip->dev,
			&dev->sram_channels[AUDIO_SRAM_CHANNEL]);

	return 0;
}

/*
 * BOARD Specific: Handles audio IRQ
 */
int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask)
{
	struct cx23885_audio_dev *chip = dev->audio_dev;

	if (0 == (status & mask))
		return 0;

	cx_write(AUDIO_INT_INT_STAT, status);

	/* risc op code error */
	if (status & AUD_INT_OPC_ERR) {
		pr_warn("%s/1: Audio risc op code error\n",
			dev->name);
		cx_clear(AUD_INT_DMA_CTL, 0x11);
		cx23885_sram_channel_dump(dev,
			&dev->sram_channels[AUDIO_SRAM_CHANNEL]);
	}
	if (status & AUD_INT_DN_SYNC) {
		dprintk(1, "Downstream sync error\n");
		cx_write(AUD_INT_A_GPCNT_CTL, GP_COUNT_CONTROL_RESET);
		return 1;
	}
	/* risc1 downstream */
	if (status & AUD_INT_DN_RISCI1) {
		atomic_set(&chip->count, cx_read(AUD_INT_A_GPCNT));
		snd_pcm_period_elapsed(chip->substream);
	}
	/* FIXME: Any other status should deserve a special handling? */

	return 1;
}

static int dsp_buffer_free(struct cx23885_audio_dev *chip)
{
	struct cx23885_riscmem *risc;

	BUG_ON(!chip->dma_size);

	dprintk(2, "Freeing buffer\n");
	cx23885_alsa_dma_unmap(chip);
	cx23885_alsa_dma_free(chip->buf);
	risc = &chip->buf->risc;
	dma_free_coherent(&chip->pci->dev, risc->size, risc->cpu, risc->dma);
	kfree(chip->buf);

	chip->buf = NULL;
	chip->dma_size = 0;

	return 0;
}

/****************************************************************************
				ALSA PCM Interface
 ****************************************************************************/

/*
 * Digital hardware definition
 */
#define DEFAULT_FIFO_SIZE	4096

static const struct snd_pcm_hardware snd_cx23885_digital_hw = {
	.info = SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_BLOCK_TRANSFER |
		SNDRV_PCM_INFO_MMAP_VALID,
	.formats = SNDRV_PCM_FMTBIT_S16_LE,

	.rates =		SNDRV_PCM_RATE_48000,
	.rate_min =		48000,
	.rate_max =		48000,
	.channels_min = 2,
	.channels_max = 2,
	/* Analog audio output will be full of clicks and pops if there
	   are not exactly four lines in the SRAM FIFO buffer.  */
	.period_bytes_min = DEFAULT_FIFO_SIZE/4,
	.period_bytes_max = DEFAULT_FIFO_SIZE/4,
	.periods_min = 1,
	.periods_max = 1024,
	.buffer_bytes_max = (1024*1024),
};

/*
 * audio pcm capture open callback
 */
static int snd_cx23885_pcm_open(struct snd_pcm_substream *substream)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;

	if (!chip) {
		pr_err("BUG: cx23885 can't find device struct. Can't proceed with open\n");
		return -ENODEV;
	}

	err = snd_pcm_hw_constraint_pow2(runtime, 0,
		SNDRV_PCM_HW_PARAM_PERIODS);
	if (err < 0)
		goto _error;

	chip->substream = substream;

	runtime->hw = snd_cx23885_digital_hw;

	if (chip->dev->sram_channels[AUDIO_SRAM_CHANNEL].fifo_size !=
		DEFAULT_FIFO_SIZE) {
		unsigned int bpl = chip->dev->
			sram_channels[AUDIO_SRAM_CHANNEL].fifo_size / 4;
		bpl &= ~7; /* must be multiple of 8 */
		runtime->hw.period_bytes_min = bpl;
		runtime->hw.period_bytes_max = bpl;
	}

	return 0;
_error:
	dprintk(1, "Error opening PCM!\n");
	return err;
}

/*
 * audio close callback
 */
static int snd_cx23885_close(struct snd_pcm_substream *substream)
{
	return 0;
}


/*
 * hw_params callback
 */
static int snd_cx23885_hw_params(struct snd_pcm_substream *substream,
			      struct snd_pcm_hw_params *hw_params)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct cx23885_audio_buffer *buf;
	int ret;

	if (substream->runtime->dma_area) {
		dsp_buffer_free(chip);
		substream->runtime->dma_area = NULL;
	}

	chip->period_size = params_period_bytes(hw_params);
	chip->num_periods = params_periods(hw_params);
	chip->dma_size = chip->period_size * params_periods(hw_params);

	BUG_ON(!chip->dma_size);
	BUG_ON(chip->num_periods & (chip->num_periods-1));

	buf = kzalloc(sizeof(*buf), GFP_KERNEL);
	if (NULL == buf)
		return -ENOMEM;

	buf->bpl = chip->period_size;
	chip->buf = buf;

	ret = cx23885_alsa_dma_init(chip,
			(PAGE_ALIGN(chip->dma_size) >> PAGE_SHIFT));
	if (ret < 0)
		goto error;

	ret = cx23885_alsa_dma_map(chip);
	if (ret < 0)
		goto error;

	ret = cx23885_risc_databuffer(chip->pci, &buf->risc, buf->sglist,
				   chip->period_size, chip->num_periods, 1);
	if (ret < 0)
		goto error;

	/* Loop back to start of program */
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP|RISC_IRQ1|RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	substream->runtime->dma_area = chip->buf->vaddr;
	substream->runtime->dma_bytes = chip->dma_size;
	substream->runtime->dma_addr = 0;

	return 0;

error:
	kfree(buf);
	chip->buf = NULL;
	return ret;
}

/*
 * hw free callback
 */
static int snd_cx23885_hw_free(struct snd_pcm_substream *substream)
{

	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);

	if (substream->runtime->dma_area) {
		dsp_buffer_free(chip);
		substream->runtime->dma_area = NULL;
	}

	return 0;
}

/*
 * prepare callback
 */
static int snd_cx23885_prepare(struct snd_pcm_substream *substream)
{
	return 0;
}

/*
 * trigger callback
 */
static int snd_cx23885_card_trigger(struct snd_pcm_substream *substream,
	int cmd)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	int err;

	/* Local interrupts are already disabled by ALSA */
	spin_lock(&chip->lock);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		err = cx23885_start_audio_dma(chip);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		err = cx23885_stop_audio_dma(chip);
		break;
	default:
		err = -EINVAL;
		break;
	}

	spin_unlock(&chip->lock);

	return err;
}

/*
 * pointer callback
 */
static snd_pcm_uframes_t snd_cx23885_pointer(
	struct snd_pcm_substream *substream)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	u16 count;

	count = atomic_read(&chip->count);

	return runtime->period_size * (count & (runtime->periods-1));
}

/*
 * page callback (needed for mmap)
 */
static struct page *snd_cx23885_page(struct snd_pcm_substream *substream,
				unsigned long offset)
{
	void *pageptr = substream->runtime->dma_area + offset;
	return vmalloc_to_page(pageptr);
}

/*
 * operators
 */
static const struct snd_pcm_ops snd_cx23885_pcm_ops = {
	.open = snd_cx23885_pcm_open,
	.close = snd_cx23885_close,
	.hw_params = snd_cx23885_hw_params,
	.hw_free = snd_cx23885_hw_free,
	.prepare = snd_cx23885_prepare,
	.trigger = snd_cx23885_card_trigger,
	.pointer = snd_cx23885_pointer,
	.page = snd_cx23885_page,
};

/*
 * create a PCM device
 */
static int snd_cx23885_pcm(struct cx23885_audio_dev *chip, int device,
	char *name)
{
	int err;
	struct snd_pcm *pcm;

	err = snd_pcm_new(chip->card, name, device, 0, 1, &pcm);
	if (err < 0)
		return err;
	pcm->private_data = chip;
	strscpy(pcm->name, name, sizeof(pcm->name));
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_cx23885_pcm_ops);

	return 0;
}

/****************************************************************************
			Basic Flow for Sound Devices
 ****************************************************************************/

/*
 * Alsa Constructor - Component probe
 */

struct cx23885_audio_dev *cx23885_audio_register(struct cx23885_dev *dev)
{
	struct snd_card *card;
	struct cx23885_audio_dev *chip;
	int err;

	if (disable_analog_audio)
		return NULL;

	if (dev->sram_channels[AUDIO_SRAM_CHANNEL].cmds_start == 0) {
		pr_warn("%s(): Missing SRAM channel configuration for analog TV Audio\n",
		       __func__);
		return NULL;
	}

	err = snd_card_new(&dev->pci->dev,
			   SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1,
			THIS_MODULE, sizeof(struct cx23885_audio_dev), &card);
	if (err < 0)
		goto error_msg;

	chip = (struct cx23885_audio_dev *) card->private_data;
	chip->dev = dev;
	chip->pci = dev->pci;
	chip->card = card;
	spin_lock_init(&chip->lock);

	err = snd_cx23885_pcm(chip, 0, "CX23885 Digital");
	if (err < 0)
		goto error;

	strscpy(card->driver, "CX23885", sizeof(card->driver));
	sprintf(card->shortname, "Conexant CX23885");
	sprintf(card->longname, "%s at %s", card->shortname, dev->name);

	err = snd_card_register(card);
	if (err < 0)
		goto error;

	dprintk(0, "registered ALSA audio device\n");

	return chip;

error:
	snd_card_free(card);
error_msg:
	pr_err("%s(): Failed to register analog audio adapter\n",
	       __func__);

	return NULL;
}

/*
 * ALSA destructor
 */
void cx23885_audio_unregister(struct cx23885_dev *dev)
{
	struct cx23885_audio_dev *chip = dev->audio_dev;

	snd_card_free(chip->card);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *  Support for CX23885 analog audio capture
 *
 *    (c) 2008 Mijhail Moreyra <mijhail.moreyra@gmail.com>
 *    Adapted from cx88-alsa.c
 *    (c) 2009 Steven Toth <stoth@kernellabs.com>
 */

#include "cx23885.h"
#include "cx23885-reg.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>
#include <linux/pci.h>

#include <asm/delay.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>

#include <sound/tlv.h>

#define AUDIO_SRAM_CHANNEL	SRAM_CH07

#define dprintk(level, fmt, arg...) do {				\
	if (audio_debug + 1 > level)					\
		printk(KERN_DEBUG pr_fmt("%s: alsa: " fmt), \
			chip->dev->name, ##arg); \
} while(0)

/****************************************************************************
			Module global static vars
 ****************************************************************************/

static unsigned int disable_analog_audio;
module_param(disable_analog_audio, int, 0644);
MODULE_PARM_DESC(disable_analog_audio, "disable analog audio ALSA driver");

static unsigned int audio_debug;
module_param(audio_debug, int, 0644);
MODULE_PARM_DESC(audio_debug, "enable debug messages [analog audio]");

/****************************************************************************
			Board specific functions
 ****************************************************************************/

/* Constants taken from cx88-reg.h */
#define AUD_INT_DN_RISCI1       (1 <<  0)
#define AUD_INT_UP_RISCI1       (1 <<  1)
#define AUD_INT_RDS_DN_RISCI1   (1 <<  2)
#define AUD_INT_DN_RISCI2       (1 <<  4) /* yes, 3 is skipped */
#define AUD_INT_UP_RISCI2       (1 <<  5)
#define AUD_INT_RDS_DN_RISCI2   (1 <<  6)
#define AUD_INT_DN_SYNC         (1 << 12)
#define AUD_INT_UP_SYNC         (1 << 13)
#define AUD_INT_RDS_DN_SYNC     (1 << 14)
#define AUD_INT_OPC_ERR         (1 << 16)
#define AUD_INT_BER_IRQ         (1 << 20)
#define AUD_INT_MCHG_IRQ        (1 << 21)
#define GP_COUNT_CONTROL_RESET	0x3

static int cx23885_alsa_dma_init(struct cx23885_audio_dev *chip,
				 unsigned long nr_pages)
{
	struct cx23885_audio_buffer *buf = chip->buf;
	struct page *pg;
	int i;

	buf->vaddr = vmalloc_32(nr_pages << PAGE_SHIFT);
	if (NULL == buf->vaddr) {
		dprintk(1, "vmalloc_32(%lu pages) failed\n", nr_pages);
		return -ENOMEM;
	}

	dprintk(1, "vmalloc is at addr %p, size=%lu\n",
		buf->vaddr, nr_pages << PAGE_SHIFT);

	memset(buf->vaddr, 0, nr_pages << PAGE_SHIFT);
	buf->nr_pages = nr_pages;

	buf->sglist = vzalloc(array_size(sizeof(*buf->sglist), buf->nr_pages));
	if (NULL == buf->sglist)
		goto vzalloc_err;

	sg_init_table(buf->sglist, buf->nr_pages);
	for (i = 0; i < buf->nr_pages; i++) {
		pg = vmalloc_to_page(buf->vaddr + i * PAGE_SIZE);
		if (NULL == pg)
			goto vmalloc_to_page_err;
		sg_set_page(&buf->sglist[i], pg, PAGE_SIZE, 0);
	}
	return 0;

vmalloc_to_page_err:
	vfree(buf->sglist);
	buf->sglist = NULL;
vzalloc_err:
	vfree(buf->vaddr);
	buf->vaddr = NULL;
	return -ENOMEM;
}

static int cx23885_alsa_dma_map(struct cx23885_audio_dev *dev)
{
	struct cx23885_audio_buffer *buf = dev->buf;

	buf->sglen = dma_map_sg(&dev->pci->dev, buf->sglist,
			buf->nr_pages, DMA_FROM_DEVICE);

	if (0 == buf->sglen) {
		pr_warn("%s: cx23885_alsa_map_sg failed\n", __func__);
		return -ENOMEM;
	}
	return 0;
}

static int cx23885_alsa_dma_unmap(struct cx23885_audio_dev *dev)
{
	struct cx23885_audio_buffer *buf = dev->buf;

	if (!buf->sglen)
		return 0;

	dma_unmap_sg(&dev->pci->dev, buf->sglist, buf->nr_pages, DMA_FROM_DEVICE);
	buf->sglen = 0;
	return 0;
}

static int cx23885_alsa_dma_free(struct cx23885_audio_buffer *buf)
{
	vfree(buf->sglist);
	buf->sglist = NULL;
	vfree(buf->vaddr);
	buf->vaddr = NULL;
	return 0;
}

/*
 * BOARD Specific: Sets audio DMA
 */

static int cx23885_start_audio_dma(struct cx23885_audio_dev *chip)
{
	struct cx23885_audio_buffer *buf = chip->buf;
	struct cx23885_dev *dev  = chip->dev;
	struct sram_channel *audio_ch =
		&dev->sram_channels[AUDIO_SRAM_CHANNEL];

	dprintk(1, "%s()\n", __func__);

	/* Make sure RISC/FIFO are off before changing FIFO/RISC settings */
	cx_clear(AUD_INT_DMA_CTL, 0x11);

	/* setup fifo + format - out channel */
	cx23885_sram_channel_setup(chip->dev, audio_ch, buf->bpl,
		buf->risc.dma);

	/* sets bpl size */
	cx_write(AUD_INT_A_LNGTH, buf->bpl);

	/* This is required to get good audio (1 seems to be ok) */
	cx_write(AUD_INT_A_MODE, 1);

	/* reset counter */
	cx_write(AUD_INT_A_GPCNT_CTL, GP_COUNT_CONTROL_RESET);
	atomic_set(&chip->count, 0);

	dprintk(1, "Start audio DMA, %d B/line, %d lines/FIFO, %d periods, %d byte buffer\n",
		buf->bpl, cx_read(audio_ch->cmds_start+12)>>1,
		chip->num_periods, buf->bpl * chip->num_periods);

	/* Enables corresponding bits at AUD_INT_STAT */
	cx_write(AUDIO_INT_INT_MSK, AUD_INT_OPC_ERR | AUD_INT_DN_SYNC |
				    AUD_INT_DN_RISCI1);

	/* Clean any pending interrupt bits already set */
	cx_write(AUDIO_INT_INT_STAT, ~0);

	/* enable audio irqs */
	cx_set(PCI_INT_MSK, chip->dev->pci_irqmask | PCI_MSK_AUD_INT);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5)); /* Enables Risc Processor */
	cx_set(AUD_INT_DMA_CTL, 0x11); /* audio downstream FIFO and
					  RISC enable */
	if (audio_debug)
		cx23885_sram_channel_dump(chip->dev, audio_ch);

	return 0;
}

/*
 * BOARD Specific: Resets audio DMA
 */
static int cx23885_stop_audio_dma(struct cx23885_audio_dev *chip)
{
	struct cx23885_dev *dev = chip->dev;
	dprintk(1, "Stopping audio DMA\n");

	/* stop dma */
	cx_clear(AUD_INT_DMA_CTL, 0x11);

	/* disable irqs */
	cx_clear(PCI_INT_MSK, PCI_MSK_AUD_INT);
	cx_clear(AUDIO_INT_INT_MSK, AUD_INT_OPC_ERR | AUD_INT_DN_SYNC |
				    AUD_INT_DN_RISCI1);

	if (audio_debug)
		cx23885_sram_channel_dump(chip->dev,
			&dev->sram_channels[AUDIO_SRAM_CHANNEL]);

	return 0;
}

/*
 * BOARD Specific: Handles audio IRQ
 */
int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask)
{
	struct cx23885_audio_dev *chip = dev->audio_dev;

	if (0 == (status & mask))
		return 0;

	cx_write(AUDIO_INT_INT_STAT, status);

	/* risc op code error */
	if (status & AUD_INT_OPC_ERR) {
		pr_warn("%s/1: Audio risc op code error\n",
			dev->name);
		cx_clear(AUD_INT_DMA_CTL, 0x11);
		cx23885_sram_channel_dump(dev,
			&dev->sram_channels[AUDIO_SRAM_CHANNEL]);
	}
	if (status & AUD_INT_DN_SYNC) {
		dprintk(1, "Downstream sync error\n");
		cx_write(AUD_INT_A_GPCNT_CTL, GP_COUNT_CONTROL_RESET);
		return 1;
	}
	/* risc1 downstream */
	if (status & AUD_INT_DN_RISCI1) {
		atomic_set(&chip->count, cx_read(AUD_INT_A_GPCNT));
		snd_pcm_period_elapsed(chip->substream);
	}
	/* FIXME: Any other status should deserve a special handling? */

	return 1;
}

static int dsp_buffer_free(struct cx23885_audio_dev *chip)
{
	struct cx23885_riscmem *risc;

	BUG_ON(!chip->dma_size);

	dprintk(2, "Freeing buffer\n");
	cx23885_alsa_dma_unmap(chip);
	cx23885_alsa_dma_free(chip->buf);
	risc = &chip->buf->risc;
	dma_free_coherent(&chip->pci->dev, risc->size, risc->cpu, risc->dma);
	kfree(chip->buf);

	chip->buf = NULL;
	chip->dma_size = 0;

	return 0;
}

/****************************************************************************
				ALSA PCM Interface
 ****************************************************************************/

/*
 * Digital hardware definition
 */
#define DEFAULT_FIFO_SIZE	4096

static const struct snd_pcm_hardware snd_cx23885_digital_hw = {
	.info = SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_BLOCK_TRANSFER |
		SNDRV_PCM_INFO_MMAP_VALID,
	.formats = SNDRV_PCM_FMTBIT_S16_LE,

	.rates =		SNDRV_PCM_RATE_48000,
	.rate_min =		48000,
	.rate_max =		48000,
	.channels_min = 2,
	.channels_max = 2,
	/* Analog audio output will be full of clicks and pops if there
	   are not exactly four lines in the SRAM FIFO buffer.  */
	.period_bytes_min = DEFAULT_FIFO_SIZE/4,
	.period_bytes_max = DEFAULT_FIFO_SIZE/4,
	.periods_min = 1,
	.periods_max = 1024,
	.buffer_bytes_max = (1024*1024),
};

/*
 * audio pcm capture open callback
 */
static int snd_cx23885_pcm_open(struct snd_pcm_substream *substream)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;

	if (!chip) {
		pr_err("BUG: cx23885 can't find device struct. Can't proceed with open\n");
		return -ENODEV;
	}

	err = snd_pcm_hw_constraint_pow2(runtime, 0,
		SNDRV_PCM_HW_PARAM_PERIODS);
	if (err < 0)
		goto _error;

	chip->substream = substream;

	runtime->hw = snd_cx23885_digital_hw;

	if (chip->dev->sram_channels[AUDIO_SRAM_CHANNEL].fifo_size !=
		DEFAULT_FIFO_SIZE) {
		unsigned int bpl = chip->dev->
			sram_channels[AUDIO_SRAM_CHANNEL].fifo_size / 4;
		bpl &= ~7; /* must be multiple of 8 */
		runtime->hw.period_bytes_min = bpl;
		runtime->hw.period_bytes_max = bpl;
	}

	return 0;
_error:
	dprintk(1, "Error opening PCM!\n");
	return err;
}

/*
 * audio close callback
 */
static int snd_cx23885_close(struct snd_pcm_substream *substream)
{
	return 0;
}


/*
 * hw_params callback
 */
static int snd_cx23885_hw_params(struct snd_pcm_substream *substream,
			      struct snd_pcm_hw_params *hw_params)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct cx23885_audio_buffer *buf;
	int ret;

	if (substream->runtime->dma_area) {
		dsp_buffer_free(chip);
		substream->runtime->dma_area = NULL;
	}

	chip->period_size = params_period_bytes(hw_params);
	chip->num_periods = params_periods(hw_params);
	chip->dma_size = chip->period_size * params_periods(hw_params);

	BUG_ON(!chip->dma_size);
	BUG_ON(chip->num_periods & (chip->num_periods-1));

	buf = kzalloc(sizeof(*buf), GFP_KERNEL);
	if (NULL == buf)
		return -ENOMEM;

	buf->bpl = chip->period_size;
	chip->buf = buf;

	ret = cx23885_alsa_dma_init(chip,
			(PAGE_ALIGN(chip->dma_size) >> PAGE_SHIFT));
	if (ret < 0)
		goto error;

	ret = cx23885_alsa_dma_map(chip);
	if (ret < 0)
		goto error;

	ret = cx23885_risc_databuffer(chip->pci, &buf->risc, buf->sglist,
				   chip->period_size, chip->num_periods, 1);
	if (ret < 0)
		goto error;

	/* Loop back to start of program */
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP|RISC_IRQ1|RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	substream->runtime->dma_area = chip->buf->vaddr;
	substream->runtime->dma_bytes = chip->dma_size;
	substream->runtime->dma_addr = 0;

	return 0;

error:
	kfree(buf);
	chip->buf = NULL;
	return ret;
}

/*
 * hw free callback
 */
static int snd_cx23885_hw_free(struct snd_pcm_substream *substream)
{

	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);

	if (substream->runtime->dma_area) {
		dsp_buffer_free(chip);
		substream->runtime->dma_area = NULL;
	}

	return 0;
}

/*
 * prepare callback
 */
static int snd_cx23885_prepare(struct snd_pcm_substream *substream)
{
	return 0;
}

/*
 * trigger callback
 */
static int snd_cx23885_card_trigger(struct snd_pcm_substream *substream,
	int cmd)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	int err;

	/* Local interrupts are already disabled by ALSA */
	spin_lock(&chip->lock);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		err = cx23885_start_audio_dma(chip);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		err = cx23885_stop_audio_dma(chip);
		break;
	default:
		err = -EINVAL;
		break;
	}

	spin_unlock(&chip->lock);

	return err;
}

/*
 * pointer callback
 */
static snd_pcm_uframes_t snd_cx23885_pointer(
	struct snd_pcm_substream *substream)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	u16 count;

	count = atomic_read(&chip->count);

	return runtime->period_size * (count & (runtime->periods-1));
