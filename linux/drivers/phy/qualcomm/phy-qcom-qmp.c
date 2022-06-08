onst struct v4l2_frequency *f)
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_VIDEO_H_
#define _CX23885_VIDEO_H_
int cx23885_flatiron_write(struct cx23885_dev *dev, u8 reg, u8 data);
u8 cx23885_flatiron_read(struct cx23885_dev *dev, u8 reg);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_VIDEO_H_
#define _CX23885_VIDEO_H_
int cx23885_flatiron_write(struct cx23885_dev *dev, u8 reg, u8 data);
u8 cx23885_flatiron_read(struct cx23885_dev *dev, u8 reg);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/pci.h>
#include <linux/i2c.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ctrls.h>
#include <media/tuner.h>
#include <media/tveeprom.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dvb.h>
#include <media/rc-core.h>

#include "cx23885-reg.h"
#include "media/drv-intf/cx2341x.h"

#include <linux/mutex.h>

#define CX23885_VERSION "0.0.4"

#define UNSET (-1U)

#define CX23885_MAXBOARDS 8

/* Max number of inputs by card */
#define MAX_CX23885_INPUT 8
#define INPUT(nr) (&cx23885_boards[dev->board].input[nr])

#define BUFFER_TIMEOUT     (HZ)  /* 0.5 seconds */

#define CX23885_BOARD_NOAUTO               UNSET
#define CX23885_BOARD_UNKNOWN                  0
#define CX23885_BOARD_HAUPPAUGE_HVR1800lp      1
#define CX23885_BOARD_HAUPPAUGE_HVR1800        2
#define CX23885_BOARD_HAUPPAUGE_HVR1250        3
#define CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP   4
#define CX23885_BOARD_HAUPPAUGE_HVR1500Q       5
#define CX23885_BOARD_HAUPPAUGE_HVR1500        6
#define CX23885_BOARD_HAUPPAUGE_HVR1200        7
#define CX23885_BOARD_HAUPPAUGE_HVR1700        8
#define CX23885_BOARD_HAUPPAUGE_HVR1400        9
#define CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP 10
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP 11
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H 12
#define CX23885_BOARD_COMPRO_VIDEOMATE_E650F   13
#define CX23885_BOARD_TBS_6920                 14
#define CX23885_BOARD_TEVII_S470               15
#define CX23885_BOARD_DVBWORLD_2005            16
#define CX23885_BOARD_NETUP_DUAL_DVBS2_CI      17
#define CX23885_BOARD_HAUPPAUGE_HVR1270        18
#define CX23885_BOARD_HAUPPAUGE_HVR1275        19
#define CX23885_BOARD_HAUPPAUGE_HVR1255        20
#define CX23885_BOARD_HAUPPAUGE_HVR1210        21
#define CX23885_BOARD_MYGICA_X8506             22
#define CX23885_BOARD_MAGICPRO_PROHDTVE2       23
#define CX23885_BOARD_HAUPPAUGE_HVR1850        24
#define CX23885_BOARD_COMPRO_VIDEOMATE_E800    25
#define CX23885_BOARD_HAUPPAUGE_HVR1290        26
#define CX23885_BOARD_MYGICA_X8558PRO          27
#define CX23885_BOARD_LEADTEK_WINFAST_PXTV1200 28
#define CX23885_BOARD_GOTVIEW_X5_3D_HYBRID     29
#define CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF 30
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000 31
#define CX23885_BOARD_MPX885                   32
#define CX23885_BOARD_MYGICA_X8507             33
#define CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL 34
#define CX23885_BOARD_TEVII_S471               35
#define CX23885_BOARD_HAUPPAUGE_HVR1255_22111  36
#define CX23885_BOARD_PROF_8000                37
#define CX23885_BOARD_HAUPPAUGE_HVR4400        38
#define CX23885_BOARD_AVERMEDIA_HC81R          39
#define CX23885_BOARD_TBS_6981                 40
#define CX23885_BOARD_TBS_6980                 41
#define CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200 42
#define CX23885_BOARD_HAUPPAUGE_IMPACTVCBE     43
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2 44
#define CX23885_BOARD_DVBSKY_T9580             45
#define CX23885_BOARD_DVBSKY_T980C             46
#define CX23885_BOARD_DVBSKY_S950C             47
#define CX23885_BOARD_TT_CT2_4500_CI           48
#define CX23885_BOARD_DVBSKY_S950              49
#define CX23885_BOARD_DVBSKY_S952              50
#define CX23885_BOARD_DVBSKY_T982              51
#define CX23885_BOARD_HAUPPAUGE_HVR5525        52
#define CX23885_BOARD_HAUPPAUGE_STARBURST      53
#define CX23885_BOARD_VIEWCAST_260E            54
#define CX23885_BOARD_VIEWCAST_460E            55
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB     56
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC    57
#define CX23885_BOARD_HAUPPAUGE_HVR1265_K4     58
#define CX23885_BOARD_HAUPPAUGE_STARBURST2     59
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885 60
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885 61
#define CX23885_BOARD_AVERMEDIA_CE310B         62

#define GPIO_0 0x00000001
#define GPIO_1 0x00000002
#define GPIO_2 0x00000004
#define GPIO_3 0x00000008
#define GPIO_4 0x00000010
#define GPIO_5 0x00000020
#define GPIO_6 0x00000040
#define GPIO_7 0x00000080
#define GPIO_8 0x00000100
#define GPIO_9 0x00000200
#define GPIO_10 0x00000400
#define GPIO_11 0x00000800
#define GPIO_12 0x00001000
#define GPIO_13 0x00002000
#define GPIO_14 0x00004000
#define GPIO_15 0x00008000

/* Currently unsupported by the driver: PAL/H, NTSC/Kr, SECAM B/G/H/LC */
#define CX23885_NORMS (\
	V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_443 | \
	V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK    |  V4L2_STD_PAL_I    | \
	V4L2_STD_PAL_M  |  V4L2_STD_PAL_N     |  V4L2_STD_PAL_Nc   | \
	V4L2_STD_PAL_60 |  V4L2_STD_SECAM_L   |  V4L2_STD_SECAM_DK)

struct cx23885_fmt {
	u32   fourcc;          /* v4l2 format id */
	int   depth;
	int   flags;
	u32   cxformat;
};

struct cx23885_tvnorm {
	char		*name;
	v4l2_std_id	id;
	u32		cxiformat;
	u32		cxoformat;
};

enum cx23885_itype {
	CX23885_VMUX_COMPOSITE1 = 1,
	CX23885_VMUX_COMPOSITE2,
	CX23885_VMUX_COMPOSITE3,
	CX23885_VMUX_COMPOSITE4,
	CX23885_VMUX_SVIDEO,
	CX23885_VMUX_COMPONENT,
	CX23885_VMUX_TELEVISION,
	CX23885_VMUX_CABLE,
	CX23885_VMUX_DVB,
	CX23885_VMUX_DEBUG,
	CX23885_RADIO,
};

enum cx23885_src_sel_type {
	CX23885_SRC_SEL_EXT_656_VIDEO = 0,
	CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO
};

struct cx23885_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

/* buffer for one video frame */
struct cx23885_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head queue;

	/* cx23885 specific */
	unsigned int           bpl;
	struct cx23885_riscmem risc;
	struct cx23885_fmt     *fmt;
	u32                    count;
};

struct cx23885_input {
	enum cx23885_itype type;
	unsigned int    vmux;
	unsigned int    amux;
	u32             gpio0, gpio1, gpio2, gpio3;
};

typedef enum {
	CX23885_MPEG_UNDEFINED = 0,
	CX23885_MPEG_DVB,
	CX23885_ANALOG_VIDEO,
	CX23885_MPEG_ENCODER,
} port_t;

struct cx23885_board {
	char                    *name;
	port_t			porta, portb, portc;
	int		num_fds_portb, num_fds_portc;
	unsigned int		tuner_type;
	unsigned int		radio_type;
	unsigned char		tuner_addr;
	unsigned char		radio_addr;
	unsigned int		tuner_bus;

	/* Vendors can and do run the PCIe bridge at different
	 * clock rates, driven physically by crystals on the PCBs.
	 * The core has to accommodate this. This allows the user
	 * to add new boards with new frequencys. The value is
	 * expressed in Hz.
	 *
	 * The core framework will default this value based on
	 * current designs, but it can vary.
	 */
	u32			clk_freq;
	struct cx23885_input    input[MAX_CX23885_INPUT];
	int			ci_type; /* for NetUP */
	/* Force bottom field first during DMA (888 workaround) */
	u32                     force_bff;
};

struct cx23885_subid {
	u16     subvendor;
	u16     subdevice;
	u32     card;
};

struct cx23885_i2c {
	struct cx23885_dev *dev;

	int                        nr;

	/* i2c i/o */
	struct i2c_adapter         i2c_adap;
	struct i2c_client          i2c_client;
	u32                        i2c_rc;

	/* 885 registers used for raw address */
	u32                        i2c_period;
	u32                        reg_ctrl;
	u32                        reg_stat;
	u32                        reg_addr;
	u32                        reg_rdata;
	u32                        reg_wdata;
};

struct cx23885_dmaqueue {
	struct list_head       active;
	u32                    count;
};

struct cx23885_tsport {
	struct cx23885_dev *dev;

	unsigned                   nr;
	int                        sram_chno;

	struct vb2_dvb_frontends   frontends;

	/* dma queues */
	struct cx23885_dmaqueue    mpegq;
	u32                        ts_packet_size;
	u32                        ts_packet_count;

	int                        width;
	int                        height;

	spinlock_t                 slock;

	/* registers */
	u32                        reg_gpcnt;
	u32                        reg_gpcnt_ctl;
	u32                        reg_dma_ctl;
	u32                        reg_lngth;
	u32                        reg_hw_sop_ctrl;
	u32                        reg_gen_ctrl;
	u32                        reg_bd_pkt_status;
	u32                        reg_sop_status;
	u32                        reg_fifo_ovfl_stat;
	u32                        reg_vld_misc;
	u32                        reg_ts_clk_en;
	u32                        reg_ts_int_msk;
	u32                        reg_ts_int_stat;
	u32                        reg_src_sel;

	/* Default register vals */
	int                        pci_irqmask;
	u32                        dma_ctl_val;
	u32                        ts_int_msk_val;
	u32                        gen_ctrl_val;
	u32                        ts_clk_en_val;
	u32                        src_sel_val;
	u32                        vld_misc_val;
	u32                        hw_sop_ctrl_val;

	/* Allow a single tsport to have multiple frontends */
	u32                        num_frontends;
	void                (*gate_ctrl)(struct cx23885_tsport *port, int open);
	void                       *port_priv;

	/* Workaround for a temp dvb_frontend that the tuner can attached to */
	struct dvb_frontend analog_fe;

	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	struct i2c_client *i2c_client_sec;
	struct i2c_client *i2c_client_ci;

	int (*set_frontend)(struct dvb_frontend *fe);
	int (*fe_set_voltage)(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage);
};

struct cx23885_kernel_ir {
	struct cx23885_dev	*cx;
	char			*name;
	char			*phys;

	struct rc_dev		*rc;
};

struct cx23885_audio_buffer {
	unsigned int		bpl;
	struct cx23885_riscmem	risc;
	void			*vaddr;
	struct scatterlist	*sglist;
	int			sglen;
	unsigned long		nr_pages;
};

struct cx23885_audio_dev {
	struct cx23885_dev	*dev;

	struct pci_dev		*pci;

	struct snd_card		*card;

	spinlock_t		lock;

	atomic_t		count;

	unsigned int		dma_size;
	unsigned int		period_size;
	unsigned int		num_periods;

	struct cx23885_audio_buffer   *buf;

	struct snd_pcm_substream *substream;
};

struct cx23885_dev {
	atomic_t                   refcount;
	struct v4l2_device	   v4l2_dev;
	struct v4l2_ctrl_handler   ctrl_handler;

	/* pci stuff */
	struct pci_dev             *pci;
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/pci.h>
#include <linux/i2c.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ctrls.h>
#include <media/tuner.h>
#include <media/tveeprom.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dvb.h>
#include <media/rc-core.h>

#include "cx23885-reg.h"
#include "media/drv-intf/cx2341x.h"

#include <linux/mutex.h>

#define CX23885_VERSION "0.0.4"

#define UNSET (-1U)

#define CX23885_MAXBOARDS 8

/* Max number of inputs by card */
#define MAX_CX23885_INPUT 8
#define INPUT(nr) (&cx23885_boards[dev->board].input[nr])

#define BUFFER_TIMEOUT     (HZ)  /* 0.5 seconds */

#define CX23885_BOARD_NOAUTO               UNSET
#define CX23885_BOARD_UNKNOWN                  0
#define CX23885_BOARD_HAUPPAUGE_HVR1800lp      1
#define CX23885_BOARD_HAUPPAUGE_HVR1800        2
#define CX23885_BOARD_HAUPPAUGE_HVR1250        3
#define CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP   4
#define CX23885_BOARD_HAUPPAUGE_HVR1500Q       5
#define CX23885_BOARD_HAUPPAUGE_HVR1500        6
#define CX23885_BOARD_HAUPPAUGE_HVR1200        7
#define CX23885_BOARD_HAUPPAUGE_HVR1700        8
#define CX23885_BOARD_HAUPPAUGE_HVR1400        9
#define CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP 10
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP 11
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H 12
#define CX23885_BOARD_COMPRO_VIDEOMATE_E650F   13
#define CX23885_BOARD_TBS_6920                 14
#define CX23885_BOARD_TEVII_S470               15
#define CX23885_BOARD_DVBWORLD_2005            16
#define CX23885_BOARD_NETUP_DUAL_DVBS2_CI      17
#define CX23885_BOARD_HAUPPAUGE_HVR1270        18
#define CX23885_BOARD_HAUPPAUGE_HVR1275        19
#define CX23885_BOARD_HAUPPAUGE_HVR1255        20
#define CX23885_BOARD_HAUPPAUGE_HVR1210        21
#define CX23885_BOARD_MYGICA_X8506             22
#define CX23885_BOARD_MAGICPRO_PROHDTVE2       23
#define CX23885_BOARD_HAUPPAUGE_HVR1850        24
#define CX23885_BOARD_COMPRO_VIDEOMATE_E800    25
#define CX23885_BOARD_HAUPPAUGE_HVR1290        26
#define CX23885_BOARD_MYGICA_X8558PRO          27
#define CX23885_BOARD_LEADTEK_WINFAST_PXTV1200 28
#define CX23885_BOARD_GOTVIEW_X5_3D_HYBRID     29
#define CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF 30
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000 31
#define CX23885_BOARD_MPX885                   32
#define CX23885_BOARD_MYGICA_X8507             33
#define CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL 34
#define CX23885_BOARD_TEVII_S471               35
#define CX23885_BOARD_HAUPPAUGE_HVR1255_22111  36
#define CX23885_BOARD_PROF_8000                37
#define CX23885_BOARD_HAUPPAUGE_HVR4400        38
#define CX23885_BOARD_AVERMEDIA_HC81R          39
#define CX23885_BOARD_TBS_6981                 40
#define CX23885_BOARD_TBS_6980                 41
#define CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200 42
#define CX23885_BOARD_HAUPPAUGE_IMPACTVCBE     43
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2 44
#define CX23885_BOARD_DVBSKY_T9580             45
#define CX23885_BOARD_DVBSKY_T980C             46
#define CX23885_BOARD_DVBSKY_S950C             47
#define CX23885_BOARD_TT_CT2_4500_CI           48
#define CX23885_BOARD_DVBSKY_S950              49
#define CX23885_BOARD_DVBSKY_S952              50
#define CX23885_BOARD_DVBSKY_T982              51
#define CX23885_BOARD_HAUPPAUGE_HVR5525        52
#define CX23885_BOARD_HAUPPAUGE_STARBURST      53
#define CX23885_BOARD_VIEWCAST_260E            54
#define CX23885_BOARD_VIEWCAST_460E            55
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB     56
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC    57
#define CX23885_BOARD_HAUPPAUGE_HVR1265_K4     58
#define CX23885_BOARD_HAUPPAUGE_STARBURST2     59
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885 60
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885 61
#define CX23885_BOARD_AVERMEDIA_CE310B         62

#define GPIO_0 0x00000001
#define GPIO_1 0x00000002
#define GPIO_2 0x00000004
#define GPIO_3 0x00000008
#define GPIO_4 0x00000010
#define GPIO_5 0x00000020
#define GPIO_6 0x00000040
#define GPIO_7 0x00000080
#define GPIO_8 0x00000100
#define GPIO_9 0x00000200
#define GPIO_10 0x00000400
#define GPIO_11 0x00000800
#define GPIO_12 0x00001000
#define GPIO_13 0x00002000
#define GPIO_14 0x00004000
#define GPIO_15 0x00008000

/* Currently unsupported by the driver: PAL/H, NTSC/Kr, SECAM B/G/H/LC */
#define CX23885_NORMS (\
	V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_443 | \
	V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK    |  V4L2_STD_PAL_I    | \
	V4L2_STD_PAL_M  |  V4L2_STD_PAL_N     |  V4L2_STD_PAL_Nc   | \
	V4L2_STD_PAL_60 |  V4L2_STD_SECAM_L   |  V4L2_STD_SECAM_DK)

struct cx23885_fmt {
	u32   fourcc;          /* v4l2 format id */
	int   depth;
	int   flags;
	u32   cxformat;
};

struct cx23885_tvnorm {
	char		*name;
	v4l2_std_id	id;
	u32		cxiformat;
	u32		cxoformat;
};

enum cx23885_itype {
	CX23885_VMUX_COMPOSITE1 = 1,
	CX23885_VMUX_COMPOSITE2,
	CX23885_VMUX_COMPOSITE3,
	CX23885_VMUX_COMPOSITE4,
	CX23885_VMUX_SVIDEO,
	CX23885_VMUX_COMPONENT,
	CX23885_VMUX_TELEVISION,
	CX23885_VMUX_CABLE,
	CX23885_VMUX_DVB,
	CX23885_VMUX_DEBUG,
	CX23885_RADIO,
};

enum cx23885_src_sel_type {
	CX23885_SRC_SEL_EXT_656_VIDEO = 0,
	CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO
};

struct cx23885_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

/* buffer for one video frame */
struct cx23885_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head queue;

	/* cx23885 specific */
	unsigned int           bpl;
	struct cx23885_riscmem risc;
	struct cx23885_fmt     *fmt;
	u32                    count;
};

struct cx23885_input {
	enum cx23885_itype type;
	unsigned int    vmux;
	unsigned int    amux;
	u32             gpio0, gpio1, gpio2, gpio3;
};

typedef enum {
	CX23885_MPEG_UNDEFINED = 0,
	CX23885_MPEG_DVB,
	CX23885_ANALOG_VIDEO,
	CX23885_MPEG_ENCODER,
} port_t;

struct cx23885_board {
	char                    *name;
	port_t			porta, portb, portc;
	int		num_fds_portb, num_fds_portc;
	unsigned int		tuner_type;
	unsigned int		radio_type;
	unsigned char		tuner_addr;
	unsigned char		radio_addr;
	unsigned int		tuner_bus;

	/* Vendors can and do run the PCIe bridge at different
	 * clock rates, driven physically by crystals on the PCBs.
	 * The core has to accommodate this. This allows the user
	 * to add new boards with new frequencys. The value is
	 * expressed in Hz.
	 *
	 * The core framework will default this value based on
	 * current designs, but it can vary.
	 */
	u32			clk_freq;
	struct cx23885_input    input[MAX_CX23885_INPUT];
	int			ci_type; /* for NetUP */
	/* Force bottom field first during DMA (888 workaround) */
	u32                     force_bff;
};

struct cx23885_subid {
	u16     subvendor;
	u16     subdevice;
	u32     card;
};

struct cx23885_i2c {
	struct cx23885_dev *dev;

	int                        nr;

	/* i2c i/o */
	struct i2c_adapter         i2c_adap;
	struct i2c_client          i2c_client;
	u32                        i2c_rc;

	/* 885 registers used for raw address */
	u32                        i2c_period;
	u32                        reg_ctrl;
	u32                        reg_stat;
	u32                        reg_addr;
	u32                        reg_rdata;
	u32                        reg_wdata;
};

struct cx23885_dmaqueue {
	struct list_head       active;
	u32                    count;
};

struct cx23885_tsport {
	struct cx23885_dev *dev;

	unsigned                   nr;
	int                        sram_chno;

	struct vb2_dvb_frontends   frontends;

	/* dma queues */
	struct cx23885_dmaqueue    mpegq;
	u32                        ts_packet_size;
	u32                        ts_packet_count;

	int                        width;
	int                        height;

	spinlock_t                 slock;

	/* registers */
	u32                        reg_gpcnt;
	u32                        reg_gpcnt_ctl;
	u32                        reg_dma_ctl;
	u32                        reg_lngth;
	u32                        reg_hw_sop_ctrl;
	u32                        reg_gen_ctrl;
	u32                        reg_bd_pkt_status;
	u32                        reg_sop_status;
	u32                        reg_fifo_ovfl_stat;
	u32                        reg_vld_misc;
	u32                        reg_ts_clk_en;
	u32                        reg_ts_int_msk;
	u32                        reg_ts_int_stat;
	u32                        reg_src_sel;

	/* Default register vals */
	int                        pci_irqmask;
	u32                        dma_ctl_val;
	u32                        ts_int_msk_val;
	u32                        gen_ctrl_val;
	u32                        ts_clk_en_val;
	u32                        src_sel_val;
	u32                        vld_misc_val;
	u32                        hw_sop_ctrl_val;

	/* Allow a single tsport to have multiple frontends */
	u32                        num_frontends;
	void                (*gate_ctrl)(struct cx23885_tsport *port, int open);
	void                       *port_priv;

	/* Workaround for a temp dvb_frontend that the tuner can attached to */
	struct dvb_frontend analog_fe;

	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	struct i2c_client *i2c_client_sec;
	struct i2c_client *i2c_client_ci;

	int (*set_frontend)(struct dvb_frontend *fe);
	int (*fe_set_voltage)(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage);
};

struct cx23885_kernel_ir {
	struct cx23885_dev	*cx;
	char			*name;
	char			*phys;

	struct rc_dev		*rc;
};

struct cx23885_audio_buffer {
	unsigned int		bpl;
	struct cx23885_riscmem	risc;
	void			*vaddr;
	struct scatterlist	*sglist;
	int			sglen;
	unsigned long		nr_pages;
};

struct cx23885_audio_dev {
	struct cx23885_dev	*dev;

	struct pci_dev		*pci;

	struct snd_card		*card;

	spinlock_t		lock;

	atomic_t		count;

	unsigned int		dma_size;
	unsigned int		period_size;
	unsigned int		num_periods;

	struct cx23885_audio_buffer   *buf;

	struct snd_pcm_substream *substream;
};

struct cx23885_dev {
	atomic_t                   refcount;
	struct v4l2_device	   v4l2_dev;
	struct v4l2_ctrl_handler   ctrl_handler;

	/* pci stuff */
	struct pci_dev             *pci;
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23888-ir.h"

#include <linux/kfifo.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/rc-core.h>

static unsigned int ir_888_debug;
module_param(ir_888_debug, int, 0644);
MODULE_PARM_DESC(ir_888_debug, "enable debug messages [CX23888 IR controller]");

#define CX23888_IR_REG_BASE	0x170000
/*
 * These CX23888 register offsets have a straightforward one to one mapping
 * to the CX23885 register offsets of 0x200 through 0x218
 */
#define CX23888_IR_CNTRL_REG	0x170000
#define CNTRL_WIN_3_3	0x00000000
#define CNTRL_WIN_4_3	0x00000001
#define CNTRL_WIN_3_4	0x00000002
#define CNTRL_WIN_4_4	0x00000003
#define CNTRL_WIN	0x00000003
#define CNTRL_EDG_NONE	0x00000000
#define CNTRL_EDG_FALL	0x00000004
#define CNTRL_EDG_RISE	0x00000008
#define CNTRL_EDG_BOTH	0x0000000C
#define CNTRL_EDG	0x0000000C
#define CNTRL_DMD	0x00000010
#define CNTRL_MOD	0x00000020
#define CNTRL_RFE	0x00000040
#define CNTRL_TFE	0x00000080
#define CNTRL_RXE	0x00000100
#define CNTRL_TXE	0x00000200
#define CNTRL_RIC	0x00000400
#define CNTRL_TIC	0x00000800
#define CNTRL_CPL	0x00001000
#define CNTRL_LBM	0x00002000
#define CNTRL_R		0x00004000
/* CX23888 specific control flag */
#define CNTRL_IVO	0x00008000

#define CX23888_IR_TXCLK_REG	0x170004
#define TXCLK_TCD	0x0000FFFF

#define CX23888_IR_RXCLK_REG	0x170008
#define RXCLK_RCD	0x0000FFFF

#define CX23888_IR_CDUTY_REG	0x17000C
#define CDUTY_CDC	0x0000000F

#define CX23888_IR_STATS_REG	0x170010
#define STATS_RTO	0x00000001
#define STATS_ROR	0x00000002
#define STATS_RBY	0x00000004
#define STATS_TBY	0x00000008
#define STATS_RSR	0x00000010
#define STATS_TSR	0x00000020

#define CX23888_IR_IRQEN_REG	0x170014
#define IRQEN_RTE	0x00000001
#define IRQEN_ROE	0x00000002
#define IRQEN_RSE	0x00000010
#define IRQEN_TSE	0x00000020

#define CX23888_IR_FILTR_REG	0x170018
#define FILTR_LPF	0x0000FFFF

/* This register doesn't follow the pattern; it's 0x23C on a CX23885 */
#define CX23888_IR_FIFO_REG	0x170040
#define FIFO_RXTX	0x0000FFFF
#define FIFO_RXTX_LVL	0x00010000
#define FIFO_RXTX_RTO	0x0001FFFF
#define FIFO_RX_NDV	0x00020000
#define FIFO_RX_DEPTH	8
#define FIFO_TX_DEPTH	8

/* CX23888 unique registers */
#define CX23888_IR_SEEDP_REG	0x17001C
#define CX23888_IR_TIMOL_REG	0x170020
#define CX23888_IR_WAKE0_REG	0x170024
#define CX23888_IR_WAKE1_REG	0x170028
#define CX23888_IR_WAKE2_REG	0x17002C
#define CX23888_IR_MASK0_REG	0x170030
#define CX23888_IR_MASK1_REG	0x170034
#define CX23888_IR_MAKS2_REG	0x170038
#define CX23888_IR_DPIPG_REG	0x17003C
#define CX23888_IR_LEARN_REG	0x170044

#define CX23888_VIDCLK_FREQ	108000000 /* 108 MHz, BT.656 */
#define CX23888_IR_REFCLK_FREQ	(CX23888_VIDCLK_FREQ / 2)

/*
 * We use this union internally for convenience, but callers to tx_write
 * and rx_read will be expecting records of type struct ir_raw_event.
 * Always ensure the size of this union is dictated by struct ir_raw_event.
 */
union cx23888_ir_fifo_rec {
	u32 hw_fifo_data;
	struct ir_raw_event ir_core_data;
};

#define CX23888_IR_RX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))
#define CX23888_IR_TX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))

struct cx23888_ir_state {
	struct v4l2_subdev sd;
	struct cx23885_dev *dev;

	struct v4l2_subdev_ir_parameters rx_params;
	struct mutex rx_params_lock;
	atomic_t rxclk_divider;
	atomic_t rx_invert;

	struct kfifo rx_kfifo;
	spinlock_t rx_kfifo_lock;

	struct v4l2_subdev_ir_parameters tx_params;
	struct mutex tx_params_lock;
	atomic_t txclk_divider;
};

static inline struct cx23888_ir_state *to_state(struct v4l2_subdev *sd)
{
	return v4l2_get_subdevdata(sd);
}

/*
 * IR register block read and write functions
 */
static
inline int cx23888_ir_write4(struct cx23885_dev *dev, u32 addr, u32 value)
{
	cx_write(addr, value);
	return 0;
}

static inline u32 cx23888_ir_read4(struct cx23885_dev *dev, u32 addr)
{
	return cx_read(addr);
}

static inline int cx23888_ir_and_or4(struct cx23885_dev *dev, u32 addr,
				     u32 and_mask, u32 or_value)
{
	cx_andor(addr, ~and_mask, or_value);
	return 0;
}

/*
 * Rx and Tx Clock Divider register computations
 *
 * Note the largest clock divider value of 0xffff corresponds to:
 *	(0xffff + 1) * 1000 / 108/2 MHz = 1,213,629.629... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_clock_divider(unsigned int d)
{
	if (d > RXCLK_RCD + 1)
		d = RXCLK_RCD;
	else if (d < 2)
		d = 1;
	else
		d--;
	return (u16) d;
}

static inline u16 carrier_freq_to_clock_divider(unsigned int freq)
{
	return count_to_clock_divider(
			  DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, freq * 16));
}

static inline unsigned int clock_divider_to_carrier_freq(unsigned int divider)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, (divider + 1) * 16);
}

static inline unsigned int clock_divider_to_freq(unsigned int divider,
						 unsigned int rollovers)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ,
				 (divider + 1) * rollovers);
}

/*
 * Low Pass Filter register calculations
 *
 * Note the largest count value of 0xffff corresponds to:
 *	0xffff * 1000 / 108/2 MHz = 1,213,611.11... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_lpf_count(unsigned int d)
{
	if (d > FILTR_LPF)
		d = FILTR_LPF;
	else if (d < 4)
		d = 0;
	return (u16) d;
}

static inline u16 ns_to_lpf_count(unsigned int ns)
{
	return count_to_lpf_count(
		DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ / 1000000 * ns, 1000));
}

static inline unsigned int lpf_count_to_ns(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in ns */
	return DIV_ROUND_CLOSEST(count * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static inline unsigned int lpf_count_to_us(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in us */
	return DIV_ROUND_CLOSEST(count, CX23888_IR_REFCLK_FREQ / 1000000);
}

/*
 * FIFO register pulse width count computations
 */
static u32 clock_divider_to_resolution(u16 divider)
{
	/*
	 * Resolution is the duration of 1 tick of the readable portion of
	 * of the pulse width counter as read from the FIFO.  The two lsb's are
	 * not readable, hence the << 2.  This function returns ns.
	 */
	return DIV_ROUND_CLOSEST((1 << 2)  * ((u32) divider + 1) * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static u64 pulse_width_count_to_ns(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1) * 1000; /* millicycles */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000);     /* / MHz => ns */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return n;
}

static unsigned int pulse_width_count_to_us(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1);    /* cycles      */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000); /* / MHz => us */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return (unsigned int) n;
}

/*
 * Pulse Clocks computations: Combined Pulse Width Count & Rx Clock Counts
 *
 * The total pulse clock count is an 18 bit pulse width timer count as the most
 * significant part and (up to) 16 bit clock divider count as a modulus.
 * When the Rx clock divider ticks down to 0, it increments the 18 bit pulse
 * width timer count's least significant bit.
 */
static u64 ns_to_pulse_clocks(u32 ns)
{
	u64 clocks;
	u32 rem;
	clocks = CX23888_IR_REFCLK_FREQ / 1000000 * (u64) ns; /* millicycles  */
	rem = do_div(clocks, 1000);                         /* /1000 = cycles */
	if (rem >= 1000 / 2)
		clocks++;
	return clocks;
}

static u16 pulse_clocks_to_clock_divider(u64 count)
{
	do_div(count, (FIFO_RXTX << 2) | 0x3);

	/* net result needs to be rounded down and decremented by 1 */
	if (count > RXCLK_RCD + 1)
		count = RXCLK_RCD;
	else if (count < 2)
		count = 1;
	else
		count--;
	return (u16) count;
}

/*
 * IR Control Register helpers
 */
enum tx_fifo_watermark {
	TX_FIFO_HALF_EMPTY = 0,
	TX_FIFO_EMPTY      = CNTRL_TIC,
};

enum rx_fifo_watermark {
	RX_FIFO_HALF_FULL = 0,
	RX_FIFO_NOT_EMPTY = CNTRL_RIC,
};

static inline void control_tx_irq_watermark(struct cx23885_dev *dev,
					    enum tx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_TIC, level);
}

static inline void control_rx_irq_watermark(struct cx23885_dev *dev,
					    enum rx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_RIC, level);
}

static inline void control_tx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_TXE | CNTRL_TFE),
			   enable ? (CNTRL_TXE | CNTRL_TFE) : 0);
}

static inline void control_rx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_RXE | CNTRL_RFE),
			   enable ? (CNTRL_RXE | CNTRL_RFE) : 0);
}

static inline void control_tx_modulation_enable(struct cx23885_dev *dev,
						bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_MOD,
			   enable ? CNTRL_MOD : 0);
}

static inline void control_rx_demodulation_enable(struct cx23885_dev *dev,
						  bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_DMD,
			   enable ? CNTRL_DMD : 0);
}

static inline void control_rx_s_edge_detection(struct cx23885_dev *dev,
					       u32 edge_types)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_EDG_BOTH,
			   edge_types & CNTRL_EDG_BOTH);
}

static void control_rx_s_carrier_window(struct cx23885_dev *dev,
					unsigned int carrier,
					unsigned int *carrier_range_low,
					unsigned int *carrier_range_high)
{
	u32 v;
	unsigned int c16 = carrier * 16;

	if (*carrier_range_low < DIV_ROUND_CLOSEST(c16, 16 + 3)) {
		v = CNTRL_WIN_3_4;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 4);
	} else {
		v = CNTRL_WIN_3_3;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 3);
	}

	if (*carrier_range_high > DIV_ROUND_CLOSEST(c16, 16 - 3)) {
		v |= CNTRL_WIN_4_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 4);
	} else {
		v |= CNTRL_WIN_3_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 3);
	}
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_WIN, v);
}

static inline void control_tx_polarity_invert(struct cx23885_dev *dev,
					      bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_CPL,
			   invert ? CNTRL_CPL : 0);
}

static inline void control_tx_level_invert(struct cx23885_dev *dev,
					  bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_IVO,
			   invert ? CNTRL_IVO : 0);
}

/*
 * IR Rx & Tx Clock Register helpers
 */
static unsigned int txclk_tx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static unsigned int rxclk_rx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static u32 txclk_tx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

static u32 rxclk_rx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

/*
 * IR Tx Carrier Duty Cycle register helpers
 */
static unsigned int cduty_tx_s_duty_cycle(struct cx23885_dev *dev,
					  unsigned int duty_cycle)
{
	u32 n;
	n = DIV_ROUND_CLOSEST(duty_cycle * 100, 625); /* 16ths of 100% */
	if (n != 0)
		n--;
	if (n > 15)
		n = 15;
	cx23888_ir_write4(dev, CX23888_IR_CDUTY_REG, n);
	return DIV_ROUND_CLOSEST((n + 1) * 100, 16);
}

/*
 * IR Filter Register helpers
 */
static u32 filter_rx_s_min_width(struct cx23885_dev *dev, u32 min_width_ns)
{
	u32 count = ns_to_lpf_count(min_width_ns);
	cx23888_ir_write4(dev, CX23888_IR_FILTR_REG, count);
	return lpf_count_to_ns(count);
}

/*
 * IR IRQ Enable Register helpers
 */
static inline void irqenable_rx(struct cx23885_dev *dev, u32 mask)
{
	mask &= (IRQEN_RTE | IRQEN_ROE | IRQEN_RSE);
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG,
			   ~(IRQEN_RTE | IRQEN_ROE | IRQEN_RSE), mask);
}

static inline void irqenable_tx(struct cx23885_dev *dev, u32 mask)
{
	mask &= IRQEN_TSE;
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG, ~IRQEN_TSE, mask);
}

/*
 * V4L2 Subdevice IR Ops
 */
static int cx23888_ir_irq_handler(struct v4l2_subdev *sd, u32 status,
				  bool *handled)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	unsigned long flags;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);

	union cx23888_ir_fifo_rec rx_data[FIFO_RX_DEPTH];
	unsigned int i, j, k;
	u32 events, v;
	int tsr, rsr, rto, ror, tse, rse, rte, roe, kror;

	tsr = stats & STATS_TSR; /* Tx FIFO Service Request */
	rsr = stats & STATS_RSR; /* Rx FIFO Service Request */
	rto = stats & STATS_RTO; /* Rx Pulse Width Timer Time Out */
	ror = stats & STATS_ROR; /* Rx FIFO Over Run */

	tse = irqen & IRQEN_TSE; /* Tx FIFO Service Request IRQ Enable */
	rse = irqen & IRQEN_RSE; /* Rx FIFO Service Request IRQ Enable */
	rte = irqen & IRQEN_RTE; /* Rx Pulse Width Timer Time Out IRQ Enable */
	roe = irqen & IRQEN_ROE; /* Rx FIFO Over Run IRQ Enable */

	*handled = false;
	v4l2_dbg(2, ir_888_debug, sd, "IRQ Status:  %s %s %s %s %s %s\n",
		 tsr ? "tsr" : "   ", rsr ? "rsr" : "   ",
		 rto ? "rto" : "   ", ror ? "ror" : "   ",
		 stats & STATS_TBY ? "tby" : "   ",
		 stats & STATS_RBY ? "rby" : "   ");

	v4l2_dbg(2, ir_888_debug, sd, "IRQ Enables: %s %s %s %s\n",
		 tse ? "tse" : "   ", rse ? "rse" : "   ",
		 rte ? "rte" : "   ", roe ? "roe" : "   ");

	/*
	 * Transmitter interrupt service
	 */
	if (tse && tsr) {
		/*
		 * TODO:
		 * Check the watermark threshold setting
		 * Pull FIFO_TX_DEPTH or FIFO_TX_DEPTH/2 entries from tx_kfifo
		 * Push the data to the hardware FIFO.
		 * If there was nothing more to send in the tx_kfifo, disable
		 *	the TSR IRQ and notify the v4l2_device.
		 * If there was something in the tx_kfifo, check the tx_kfifo
		 *      level and notify the v4l2_device, if it is low.
		 */
		/* For now, inhibit TSR interrupt until Tx is implemented */
		irqenable_tx(dev, 0);
		events = V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ;
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_TX_NOTIFY, &events);
		*handled = true;
	}

	/*
	 * Receiver interrupt service
	 */
	kror = 0;
	if ((rse && rsr) || (rte && rto)) {
		/*
		 * Receive data on RSR to clear the STATS_RSR.
		 * Receive data on RTO, since we may not have yet hit the RSR
		 * watermark when we receive the RTO.
		 */
		for (i = 0, v = FIFO_RX_NDV;
		     (v & FIFO_RX_NDV) && !kror; i = 0) {
			for (j = 0;
			     (v & FIFO_RX_NDV) && j < FIFO_RX_DEPTH; j++) {
				v = cx23888_ir_read4(dev, CX23888_IR_FIFO_REG);
				rx_data[i].hw_fifo_data = v & ~FIFO_RX_NDV;
				i++;
			}
			if (i == 0)
				break;
			j = i * sizeof(union cx23888_ir_fifo_rec);
			k = kfifo_in_locked(&state->rx_kfifo,
				      (unsigned char *) rx_data, j,
				      &state->rx_kfifo_lock);
			if (k != j)
				kror++; /* rx_kfifo over run */
		}
		*handled = true;
	}

	events = 0;
	v = 0;
	if (kror) {
		events |= V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver software FIFO overrun\n");
	}
	if (roe && ror) {
		/*
		 * The RX FIFO Enable (CNTRL_RFE) must be toggled to clear
		 * the Rx FIFO Over Run status (STATS_ROR)
		 */
		v |= CNTRL_RFE;
		events |= V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver hardware FIFO overrun\n");
	}
	if (rte && rto) {
		/*
		 * The IR Receiver Enable (CNTRL_RXE) must be toggled to clear
		 * the Rx Pulse Width Timer Time Out (STATS_RTO)
		 */
		v |= CNTRL_RXE;
		events |= V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED;
	}
	if (v) {
		/* Clear STATS_ROR & STATS_RTO as needed by resetting hardware */
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl & ~v);
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl);
		*handled = true;
	}

	spin_lock_irqsave(&state->rx_kfifo_lock, flags);
	if (kfifo_len(&state->rx_kfifo) >= CX23888_IR_RX_KFIFO_SIZE / 2)
		events |= V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ;
	spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);

	if (events)
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_RX_NOTIFY, &events);
	return 0;
}

/* Receiver */
static int cx23888_ir_rx_read(struct v4l2_subdev *sd, u8 *buf, size_t count,
			      ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	bool invert = (bool) atomic_read(&state->rx_invert);
	u16 divider = (u16) atomic_read(&state->rxclk_divider);

	unsigned int i, n;
	union cx23888_ir_fifo_rec *p;
	unsigned u, v, w;

	n = count / sizeof(union cx23888_ir_fifo_rec)
		* sizeof(union cx23888_ir_fifo_rec);
	if (n == 0) {
		*num = 0;
		return 0;
	}

	n = kfifo_out_locked(&state->rx_kfifo, buf, n, &state->rx_kfifo_lock);

	n /= sizeof(union cx23888_ir_fifo_rec);
	*num = n * sizeof(union cx23888_ir_fifo_rec);

	for (p = (union cx23888_ir_fifo_rec *) buf, i = 0; i < n; p++, i++) {

		if ((p->hw_fifo_data & FIFO_RXTX_RTO) == FIFO_RXTX_RTO) {
			/* Assume RTO was because of no IR light input */
			u = 0;
			w = 1;
		} else {
			u = (p->hw_fifo_data & FIFO_RXTX_LVL) ? 1 : 0;
			if (invert)
				u = u ? 0 : 1;
			w = 0;
		}

		v = (unsigned) pulse_width_count_to_ns(
				  (u16)(p->hw_fifo_data & FIFO_RXTX), divider) / 1000;
		if (v > IR_MAX_DURATION)
			v = IR_MAX_DURATION;

		p->ir_core_data = (struct ir_raw_event)
			{ .pulse = u, .duration = v, .timeout = w };

		v4l2_dbg(2, ir_888_debug, sd, "rx read: %10u ns  %s  %s\n",
			 v, u ? "mark" : "space", w ? "(timed out)" : "");
		if (w)
			v4l2_dbg(2, ir_888_debug, sd, "rx read: end of rx\n");
	}
	return 0;
}

static int cx23888_ir_rx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->rx_params_lock);
	memcpy(p, &state->rx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->rx_params_lock);

	/* Disable or slow down all IR Rx circuits and counters */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);
	control_rx_demodulation_enable(dev, false);
	control_rx_s_edge_detection(dev, CNTRL_EDG_NONE);
	filter_rx_s_min_width(dev, 0);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, RXCLK_RCD);

	state->rx_params.shutdown = true;

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->rx_params;
	u16 rxclk_divider;

	if (p->shutdown)
		return cx23888_ir_rx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->rx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the receiver */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);

	control_rx_demodulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = rxclk_rx_s_carrier(dev, p->carrier_freq,
						     &rxclk_divider);

		o->carrier_freq = p->carrier_freq;

		o->duty_cycle = p->duty_cycle = 50;

		control_rx_s_carrier_window(dev, p->carrier_freq,
					    &p->carrier_range_lower,
					    &p->carrier_range_upper);
		o->carrier_range_lower = p->carrier_range_lower;
		o->carrier_range_upper = p->carrier_range_upper;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, rxclk_divider);
	} else {
		p->max_pulse_width =
			    rxclk_rx_s_max_pulse_width(dev, p->max_pulse_width,
						       &rxclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->rxclk_divider, rxclk_divider);

	p->noise_filter_min_width =
			  filter_rx_s_min_width(dev, p->noise_filter_min_width);
	o->noise_filter_min_width = p->noise_filter_min_width;

	p->resolution = clock_divider_to_resolution(rxclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_rx_irq_watermark(dev, RX_FIFO_HALF_FULL);

	control_rx_s_edge_detection(dev, CNTRL_EDG_BOTH);

	o->invert_level = p->invert_level;
	atomic_set(&state->rx_invert, p->invert_level);

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		unsigned long flags;

		spin_lock_irqsave(&state->rx_kfifo_lock, flags);
		kfifo_reset(&state->rx_kfifo);
		/* reset tx_fifo too if there is one... */
		spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);
		if (p->interrupt_enable)
			irqenable_rx(dev, IRQEN_RSE | IRQEN_RTE | IRQEN_ROE);
		control_rx_enable(dev, p->enable);
	}

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

/* Transmitter */
static int cx23888_ir_tx_write(struct v4l2_subdev *sd, u8 *buf, size_t count,
			       ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	/* For now enable the Tx FIFO Service interrupt & pretend we did work */
	irqenable_tx(dev, IRQEN_TSE);
	*num = count;
	return 0;
}

static int cx23888_ir_tx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->tx_params_lock);
	memcpy(p, &state->tx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->tx_params_lock);

	/* Disable or slow down all IR Tx circuits and counters */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);
	control_tx_modulation_enable(dev, false);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, TXCLK_TCD);

	state->tx_params.shutdown = true;

	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->tx_params;
	u16 txclk_divider;

	if (p->shutdown)
		return cx23888_ir_tx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->tx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the transmitter */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);

	control_tx_modulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = txclk_tx_s_carrier(dev, p->carrier_freq,
						     &txclk_divider);
		o->carrier_freq = p->carrier_freq;

		p->duty_cycle = cduty_tx_s_duty_cycle(dev, p->duty_cycle);
		o->duty_cycle = p->duty_cycle;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, txclk_divider);
	} else {
		p->max_pulse_width =
			    txclk_tx_s_max_pulse_width(dev, p->max_pulse_width,
						       &txclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->txclk_divider, txclk_divider);

	p->resolution = clock_divider_to_resolution(txclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_tx_irq_watermark(dev, TX_FIFO_HALF_EMPTY);

	control_tx_polarity_invert(dev, p->invert_carrier_sense);
	o->invert_carrier_sense = p->invert_carrier_sense;

	control_tx_level_invert(dev, p->invert_level);
	o->invert_level = p->invert_level;

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		if (p->interrupt_enable)
			irqenable_tx(dev, IRQEN_TSE);
		control_tx_enable(dev, p->enable);
	}

	mutex_unlock(&state->tx_params_lock);
	return 0;
}


/*
 * V4L2 Subdevice Core Ops
 */
static int cx23888_ir_log_status(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	char *s;
	int i, j;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 txclk = cx23888_ir_read4(dev, CX23888_IR_TXCLK_REG) & TXCLK_TCD;
	u32 rxclk = cx23888_ir_read4(dev, CX23888_IR_RXCLK_REG) & RXCLK_RCD;
	u32 cduty = cx23888_ir_read4(dev, CX23888_IR_CDUTY_REG) & CDUTY_CDC;
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 filtr = cx23888_ir_read4(dev, CX23888_IR_FILTR_REG) & FILTR_LPF;

	v4l2_info(sd, "IR Receiver:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_RXE ? "yes" : "no");
	v4l2_info(sd, "\tDemodulation from a carrier:       %s\n",
		  cntrl & CNTRL_DMD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_RFE ? "enabled" : "disabled");
	switch (cntrl & CNTRL_EDG) {
	case CNTRL_EDG_NONE:
		s = "disabled";
		break;
	case CNTRL_EDG_FALL:
		s = "falling edge";
		break;
	case CNTRL_EDG_RISE:
		s = "rising edge";
		break;
	case CNTRL_EDG_BOTH:
		s = "rising & falling edges";
		break;
	default:
		s = "??? edge";
		break;
	}
	v4l2_info(sd, "\tPulse timers' start/stop trigger:  %s\n", s);
	v4l2_info(sd, "\tFIFO data on pulse timer overflow: %s\n",
		  cntrl & CNTRL_R ? "not loaded" : "overflow marker");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_RIC ? "not empty" : "half full or greater");
	v4l2_info(sd, "\tLoopback mode:                     %s\n",
		  cntrl & CNTRL_LBM ? "loopback active" : "normal receive");
	if (cntrl & CNTRL_DMD) {
		v4l2_info(sd, "\tExpected carrier (16 clocks):      %u Hz\n",
			  clock_divider_to_carrier_freq(rxclk));
		switch (cntrl & CNTRL_WIN) {
		case CNTRL_WIN_3_3:
			i = 3;
			j = 3;
			break;
		case CNTRL_WIN_4_3:
			i = 4;
			j = 3;
			break;
		case CNTRL_WIN_3_4:
			i = 3;
			j = 4;
			break;
		case CNTRL_WIN_4_4:
			i = 4;
			j = 4;
			break;
		default:
			i = 0;
			j = 0;
			break;
		}
		v4l2_info(sd, "\tNext carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz\n",
			  i, j,
			  clock_divider_to_freq(rxclk, 16 + j),
			  clock_divider_to_freq(rxclk, 16 - i));
	}
	v4l2_info(sd, "\tMax measurable pulse width:        %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, rxclk),
		  pulse_width_count_to_ns(FIFO_RXTX, rxclk));
	v4l2_info(sd, "\tLow pass filter:                   %s\n",
		  filtr ? "enabled" : "disabled");
	if (filtr)
		v4l2_info(sd, "\tMin acceptable pulse width (LPF):  %u us, %u ns\n",
			  lpf_count_to_us(filtr),
			  lpf_count_to_ns(filtr));
	v4l2_info(sd, "\tPulse width timer timed-out:       %s\n",
		  stats & STATS_RTO ? "yes" : "no");
	v4l2_info(sd, "\tPulse width timer time-out intr:   %s\n",
		  irqen & IRQEN_RTE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO overrun:                      %s\n",
		  stats & STATS_ROR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO overrun interrupt:            %s\n",
		  irqen & IRQEN_ROE ? "enabled" : "disabled");
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_RBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_RSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_RSE ? "enabled" : "disabled");

	v4l2_info(sd, "IR Transmitter:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_TXE ? "yes" : "no");
	v4l2_info(sd, "\tModulation onto a carrier:         %s\n",
		  cntrl & CNTRL_MOD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_TFE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_TIC ? "not empty" : "half full or less");
	v4l2_info(sd, "\tOutput pin level inversion         %s\n",
		  cntrl & CNTRL_IVO ? "yes" : "no");
	v4l2_info(sd, "\tCarrier polarity:                  %s\n",
		  cntrl & CNTRL_CPL ? "space:burst mark:noburst"
				    : "space:noburst mark:burst");
	if (cntrl & CNTRL_MOD) {
		v4l2_info(sd, "\tCarrier (16 clocks):               %u Hz\n",
			  clock_divider_to_carrier_freq(txclk));
		v4l2_info(sd, "\tCarrier duty cycle:                %2u/16\n",
			  cduty + 1);
	}
	v4l2_info(sd, "\tMax pulse width:                   %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, txclk),
		  pulse_width_count_to_ns(FIFO_RXTX, txclk));
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_TBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_TSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_TSE ? "enabled" : "disabled");

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int cx23888_ir_g_register(struct v4l2_subdev *sd,
				 struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	reg->size = 4;
	reg->val = cx23888_ir_read4(state->dev, addr);
	return 0;
}

static int cx23888_ir_s_register(struct v4l2_subdev *sd,
				 const struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	cx23888_ir_write4(state->dev, addr, reg->val);
	return 0;
}
#endif

static const struct v4l2_subdev_core_ops cx23888_ir_core_ops = {
	.log_status = cx23888_ir_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = cx23888_ir_g_register,
	.s_register = cx23888_ir_s_register,
#endif
	.interrupt_service_routine = cx23888_ir_irq_handler,
};

static const struct v4l2_subdev_ir_ops cx23888_ir_ir_ops = {
	.rx_read = cx23888_ir_rx_read,
	.rx_g_parameters = cx23888_ir_rx_g_parameters,
	.rx_s_parameters = cx23888_ir_rx_s_parameters,

	.tx_write = cx23888_ir_tx_write,
	.tx_g_parameters = cx23888_ir_tx_g_parameters,
	.tx_s_parameters = cx23888_ir_tx_s_parameters,
};

static const struct v4l2_subdev_ops cx23888_ir_controller_ops = {
	.core = &cx23888_ir_core_ops,
	.ir = &cx23888_ir_ir_ops,
};

static const struct v4l2_subdev_ir_parameters default_rx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5, RC-6, and RC-6A carrier */

	/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
	/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
	.noise_filter_min_width = 333333, /* ns */
	.carrier_range_lower = 35000,
	.carrier_range_upper = 37000,
	.invert_level = false,
};

static const struct v4l2_subdev_ir_parameters default_tx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5 carrier */
	.duty_cycle = 25,      /* 25 %   - RC-5 carrier */
	.invert_level = false,
	.invert_carrier_sense = false,
};

int cx23888_ir_probe(struct cx23885_dev *dev)
{
	struct cx23888_ir_state *state;
	struct v4l2_subdev *sd;
	struct v4l2_subdev_ir_parameters default_params;
	int ret;

	state = kzalloc(sizeof(struct cx23888_ir_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	spin_lock_init(&state->rx_kfifo_lock);
	if (kfifo_alloc(&state->rx_kfifo, CX23888_IR_RX_KFIFO_SIZE,
			GFP_KERNEL)) {
		kfree(state);
		return -ENOMEM;
	}

	state->dev = dev;
	sd = &state->sd;

	v4l2_subdev_init(sd, &cx23888_ir_controller_ops);
	v4l2_set_subdevdata(sd, state);
	/* FIXME - fix the formatting of dev->v4l2_dev.name and use it */
	snprintf(sd->name, sizeof(sd->name), "%s/888-ir", dev->name);
	sd->grp_id = CX23885_HW_888_IR;

	ret = v4l2_device_register_subdev(&dev->v4l2_dev, sd);
	if (ret == 0) {
		/*
		 * Ensure no interrupts arrive from '888 specific conditions,
		 * since we ignore them in this driver to have commonality with
		 * similar IR controller cores.
		 */
		cx23888_ir_write4(dev, CX23888_IR_IRQEN_REG, 0);

		mutex_init(&state->rx_params_lock);
		default_params = default_rx_params;
		v4l2_subdev_call(sd, ir, rx_s_parameters, &default_params);

		mutex_init(&state->tx_params_lock);
		default_params = default_tx_params;
		v4l2_subdev_call(sd, ir, tx_s_parameters, &default_params);
	} else {
		kfifo_free(&state->rx_kfifo);
	}
	return ret;
}

int cx23888_ir_remove(struct cx23885_dev *dev)
{
	struct v4l2_subdev *sd;
	struct cx23888_ir_state *state;

	sd = cx23885_find_hw(dev, CX23885_HW_888_IR);
	if (sd == NULL)
		return -ENODEV;

	cx23888_ir_rx_shutdown(sd);
	cx23888_ir_tx_shutdown(sd);

	state = to_state(sd);
	v4l2_device_unregister_subdev(sd);
	kfifo_free(&state->rx_kfifo);
	kfree(state);
	/* Nothing more to free() as state held the actual v4l2_subdev object */
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23888-ir.h"

#include <linux/kfifo.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/rc-core.h>

static unsigned int ir_888_debug;
module_param(ir_888_debug, int, 0644);
MODULE_PARM_DESC(ir_888_debug, "enable debug messages [CX23888 IR controller]");

#define CX23888_IR_REG_BASE	0x170000
/*
 * These CX23888 register offsets have a straightforward one to one mapping
 * to the CX23885 register offsets of 0x200 through 0x218
 */
#define CX23888_IR_CNTRL_REG	0x170000
#define CNTRL_WIN_3_3	0x00000000
#define CNTRL_WIN_4_3	0x00000001
#define CNTRL_WIN_3_4	0x00000002
#define CNTRL_WIN_4_4	0x00000003
#define CNTRL_WIN	0x00000003
#define CNTRL_EDG_NONE	0x00000000
#define CNTRL_EDG_FALL	0x00000004
#define CNTRL_EDG_RISE	0x00000008
#define CNTRL_EDG_BOTH	0x0000000C
#define CNTRL_EDG	0x0000000C
#define CNTRL_DMD	0x00000010
#define CNTRL_MOD	0x00000020
#define CNTRL_RFE	0x00000040
#define CNTRL_TFE	0x00000080
#define CNTRL_RXE	0x00000100
#define CNTRL_TXE	0x00000200
#define CNTRL_RIC	0x00000400
#define CNTRL_TIC	0x00000800
#define CNTRL_CPL	0x00001000
#define CNTRL_LBM	0x00002000
#define CNTRL_R		0x00004000
/* CX23888 specific control flag */
#define CNTRL_IVO	0x00008000

#define CX23888_IR_TXCLK_REG	0x170004
#define TXCLK_TCD	0x0000FFFF

#define CX23888_IR_RXCLK_REG	0x170008
#define RXCLK_RCD	0x0000FFFF

#define CX23888_IR_CDUTY_REG	0x17000C
#define CDUTY_CDC	0x0000000F

#define CX23888_IR_STATS_REG	0x170010
#define STATS_RTO	0x00000001
#define STATS_ROR	0x00000002
#define STATS_RBY	0x00000004
#define STATS_TBY	0x00000008
#define STATS_RSR	0x00000010
#define STATS_TSR	0x00000020

#define CX23888_IR_IRQEN_REG	0x170014
#define IRQEN_RTE	0x00000001
#define IRQEN_ROE	0x00000002
#define IRQEN_RSE	0x00000010
#define IRQEN_TSE	0x00000020

#define CX23888_IR_FILTR_REG	0x170018
#define FILTR_LPF	0x0000FFFF

/* This register doesn't follow the pattern; it's 0x23C on a CX23885 */
#define CX23888_IR_FIFO_REG	0x170040
#define FIFO_RXTX	0x0000FFFF
#define FIFO_RXTX_LVL	0x00010000
#define FIFO_RXTX_RTO	0x0001FFFF
#define FIFO_RX_NDV	0x00020000
#define FIFO_RX_DEPTH	8
#define FIFO_TX_DEPTH	8

/* CX23888 unique registers */
#define CX23888_IR_SEEDP_REG	0x17001C
#define CX23888_IR_TIMOL_REG	0x170020
#define CX23888_IR_WAKE0_REG	0x170024
#define CX23888_IR_WAKE1_REG	0x170028
#define CX23888_IR_WAKE2_REG	0x17002C
#define CX23888_IR_MASK0_REG	0x170030
#define CX23888_IR_MASK1_REG	0x170034
#define CX23888_IR_MAKS2_REG	0x170038
#define CX23888_IR_DPIPG_REG	0x17003C
#define CX23888_IR_LEARN_REG	0x170044

#define CX23888_VIDCLK_FREQ	108000000 /* 108 MHz, BT.656 */
#define CX23888_IR_REFCLK_FREQ	(CX23888_VIDCLK_FREQ / 2)

/*
 * We use this union internally for convenience, but callers to tx_write
 * and rx_read will be expecting records of type struct ir_raw_event.
 * Always ensure the size of this union is dictated by struct ir_raw_event.
 */
union cx23888_ir_fifo_rec {
	u32 hw_fifo_data;
	struct ir_raw_event ir_core_data;
};

#define CX23888_IR_RX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))
#define CX23888_IR_TX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))

struct cx23888_ir_state {
	struct v4l2_subdev sd;
	struct cx23885_dev *dev;

	struct v4l2_subdev_ir_parameters rx_params;
	struct mutex rx_params_lock;
	atomic_t rxclk_divider;
	atomic_t rx_invert;

	struct kfifo rx_kfifo;
	spinlock_t rx_kfifo_lock;

	struct v4l2_subdev_ir_parameters tx_params;
	struct mutex tx_params_lock;
	atomic_t txclk_divider;
};

static inline struct cx23888_ir_state *to_state(struct v4l2_subdev *sd)
{
	return v4l2_get_subdevdata(sd);
}

/*
 * IR register block read and write functions
 */
static
inline int cx23888_ir_write4(struct cx23885_dev *dev, u32 addr, u32 value)
{
	cx_write(addr, value);
	return 0;
}

static inline u32 cx23888_ir_read4(struct cx23885_dev *dev, u32 addr)
{
	return cx_read(addr);
}

static inline int cx23888_ir_and_or4(struct cx23885_dev *dev, u32 addr,
				     u32 and_mask, u32 or_value)
{
	cx_andor(addr, ~and_mask, or_value);
	return 0;
}

/*
 * Rx and Tx Clock Divider register computations
 *
 * Note the largest clock divider value of 0xffff corresponds to:
 *	(0xffff + 1) * 1000 / 108/2 MHz = 1,213,629.629... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_clock_divider(unsigned int d)
{
	if (d > RXCLK_RCD + 1)
		d = RXCLK_RCD;
	else if (d < 2)
		d = 1;
	else
		d--;
	return (u16) d;
}

static inline u16 carrier_freq_to_clock_divider(unsigned int freq)
{
	return count_to_clock_divider(
			  DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, freq * 16));
}

static inline unsigned int clock_divider_to_carrier_freq(unsigned int divider)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, (divider + 1) * 16);
}

static inline unsigned int clock_divider_to_freq(unsigned int divider,
						 unsigned int rollovers)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ,
				 (divider + 1) * rollovers);
}

/*
 * Low Pass Filter register calculations
 *
 * Note the largest count value of 0xffff corresponds to:
 *	0xffff * 1000 / 108/2 MHz = 1,213,611.11... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_lpf_count(unsigned int d)
{
	if (d > FILTR_LPF)
		d = FILTR_LPF;
	else if (d < 4)
		d = 0;
	return (u16) d;
}

static inline u16 ns_to_lpf_count(unsigned int ns)
{
	return count_to_lpf_count(
		DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ / 1000000 * ns, 1000));
}

static inline unsigned int lpf_count_to_ns(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in ns */
	return DIV_ROUND_CLOSEST(count * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static inline unsigned int lpf_count_to_us(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in us */
	return DIV_ROUND_CLOSEST(count, CX23888_IR_REFCLK_FREQ / 1000000);
}

/*
 * FIFO register pulse width count computations
 */
static u32 clock_divider_to_resolution(u16 divider)
{
	/*
	 * Resolution is the duration of 1 tick of the readable portion of
	 * of the pulse width counter as read from the FIFO.  The two lsb's are
	 * not readable, hence the << 2.  This function returns ns.
	 */
	return DIV_ROUND_CLOSEST((1 << 2)  * ((u32) divider + 1) * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static u64 pulse_width_count_to_ns(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1) * 1000; /* millicycles */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000);     /* / MHz => ns */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return n;
}

static unsigned int pulse_width_count_to_us(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1);    /* cycles      */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000); /* / MHz => us */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return (unsigned int) n;
}

/*
 * Pulse Clocks computations: Combined Pulse Width Count & Rx Clock Counts
 *
 * The total pulse clock count is an 18 bit pulse width timer count as the most
 * significant part and (up to) 16 bit clock divider count as a modulus.
 * When the Rx clock divider ticks down to 0, it increments the 18 bit pulse
 * width timer count's least significant bit.
 */
static u64 ns_to_pulse_clocks(u32 ns)
{
	u64 clocks;
	u32 rem;
	clocks = CX23888_IR_REFCLK_FREQ / 1000000 * (u64) ns; /* millicycles  */
	rem = do_div(clocks, 1000);                         /* /1000 = cycles */
	if (rem >= 1000 / 2)
		clocks++;
	return clocks;
}

static u16 pulse_clocks_to_clock_divider(u64 count)
{
	do_div(count, (FIFO_RXTX << 2) | 0x3);

	/* net result needs to be rounded down and decremented by 1 */
	if (count > RXCLK_RCD + 1)
		count = RXCLK_RCD;
	else if (count < 2)
		count = 1;
	else
		count--;
	return (u16) count;
}

/*
 * IR Control Register helpers
 */
enum tx_fifo_watermark {
	TX_FIFO_HALF_EMPTY = 0,
	TX_FIFO_EMPTY      = CNTRL_TIC,
};

enum rx_fifo_watermark {
	RX_FIFO_HALF_FULL = 0,
	RX_FIFO_NOT_EMPTY = CNTRL_RIC,
};

static inline void control_tx_irq_watermark(struct cx23885_dev *dev,
					    enum tx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_TIC, level);
}

static inline void control_rx_irq_watermark(struct cx23885_dev *dev,
					    enum rx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_RIC, level);
}

static inline void control_tx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_TXE | CNTRL_TFE),
			   enable ? (CNTRL_TXE | CNTRL_TFE) : 0);
}

static inline void control_rx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_RXE | CNTRL_RFE),
			   enable ? (CNTRL_RXE | CNTRL_RFE) : 0);
}

static inline void control_tx_modulation_enable(struct cx23885_dev *dev,
						bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_MOD,
			   enable ? CNTRL_MOD : 0);
}

static inline void control_rx_demodulation_enable(struct cx23885_dev *dev,
						  bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_DMD,
			   enable ? CNTRL_DMD : 0);
}

static inline void control_rx_s_edge_detection(struct cx23885_dev *dev,
					       u32 edge_types)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_EDG_BOTH,
			   edge_types & CNTRL_EDG_BOTH);
}

static void control_rx_s_carrier_window(struct cx23885_dev *dev,
					unsigned int carrier,
					unsigned int *carrier_range_low,
					unsigned int *carrier_range_high)
{
	u32 v;
	unsigned int c16 = carrier * 16;

	if (*carrier_range_low < DIV_ROUND_CLOSEST(c16, 16 + 3)) {
		v = CNTRL_WIN_3_4;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 4);
	} else {
		v = CNTRL_WIN_3_3;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 3);
	}

	if (*carrier_range_high > DIV_ROUND_CLOSEST(c16, 16 - 3)) {
		v |= CNTRL_WIN_4_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 4);
	} else {
		v |= CNTRL_WIN_3_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 3);
	}
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_WIN, v);
}

static inline void control_tx_polarity_invert(struct cx23885_dev *dev,
					      bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_CPL,
			   invert ? CNTRL_CPL : 0);
}

static inline void control_tx_level_invert(struct cx23885_dev *dev,
					  bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_IVO,
			   invert ? CNTRL_IVO : 0);
}

/*
 * IR Rx & Tx Clock Register helpers
 */
static unsigned int txclk_tx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static unsigned int rxclk_rx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static u32 txclk_tx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

static u32 rxclk_rx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

/*
 * IR Tx Carrier Duty Cycle register helpers
 */
static unsigned int cduty_tx_s_duty_cycle(struct cx23885_dev *dev,
					  unsigned int duty_cycle)
{
	u32 n;
	n = DIV_ROUND_CLOSEST(duty_cycle * 100, 625); /* 16ths of 100% */
	if (n != 0)
		n--;
	if (n > 15)
		n = 15;
	cx23888_ir_write4(dev, CX23888_IR_CDUTY_REG, n);
	return DIV_ROUND_CLOSEST((n + 1) * 100, 16);
}

/*
 * IR Filter Register helpers
 */
static u32 filter_rx_s_min_width(struct cx23885_dev *dev, u32 min_width_ns)
{
	u32 count = ns_to_lpf_count(min_width_ns);
	cx23888_ir_write4(dev, CX23888_IR_FILTR_REG, count);
	return lpf_count_to_ns(count);
}

/*
 * IR IRQ Enable Register helpers
 */
static inline void irqenable_rx(struct cx23885_dev *dev, u32 mask)
{
	mask &= (IRQEN_RTE | IRQEN_ROE | IRQEN_RSE);
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG,
			   ~(IRQEN_RTE | IRQEN_ROE | IRQEN_RSE), mask);
}

static inline void irqenable_tx(struct cx23885_dev *dev, u32 mask)
{
	mask &= IRQEN_TSE;
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG, ~IRQEN_TSE, mask);
}

/*
 * V4L2 Subdevice IR Ops
 */
static int cx23888_ir_irq_handler(struct v4l2_subdev *sd, u32 status,
				  bool *handled)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	unsigned long flags;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);

	union cx23888_ir_fifo_rec rx_data[FIFO_RX_DEPTH];
	unsigned int i, j, k;
	u32 events, v;
	int tsr, rsr, rto, ror, tse, rse, rte, roe, kror;

	tsr = stats & STATS_TSR; /* Tx FIFO Service Request */
	rsr = stats & STATS_RSR; /* Rx FIFO Service Request */
	rto = stats & STATS_RTO; /* Rx Pulse Width Timer Time Out */
	ror = stats & STATS_ROR; /* Rx FIFO Over Run */

	tse = irqen & IRQEN_TSE; /* Tx FIFO Service Request IRQ Enable */
	rse = irqen & IRQEN_RSE; /* Rx FIFO Service Request IRQ Enable */
	rte = irqen & IRQEN_RTE; /* Rx Pulse Width Timer Time Out IRQ Enable */
	roe = irqen & IRQEN_ROE; /* Rx FIFO Over Run IRQ Enable */

	*handled = false;
	v4l2_dbg(2, ir_888_debug, sd, "IRQ Status:  %s %s %s %s %s %s\n",
		 tsr ? "tsr" : "   ", rsr ? "rsr" : "   ",
		 rto ? "rto" : "   ", ror ? "ror" : "   ",
		 stats & STATS_TBY ? "tby" : "   ",
		 stats & STATS_RBY ? "rby" : "   ");

	v4l2_dbg(2, ir_888_debug, sd, "IRQ Enables: %s %s %s %s\n",
		 tse ? "tse" : "   ", rse ? "rse" : "   ",
		 rte ? "rte" : "   ", roe ? "roe" : "   ");

	/*
	 * Transmitter interrupt service
	 */
	if (tse && tsr) {
		/*
		 * TODO:
		 * Check the watermark threshold setting
		 * Pull FIFO_TX_DEPTH or FIFO_TX_DEPTH/2 entries from tx_kfifo
		 * Push the data to the hardware FIFO.
		 * If there was nothing more to send in the tx_kfifo, disable
		 *	the TSR IRQ and notify the v4l2_device.
		 * If there was something in the tx_kfifo, check the tx_kfifo
		 *      level and notify the v4l2_device, if it is low.
		 */
		/* For now, inhibit TSR interrupt until Tx is implemented */
		irqenable_tx(dev, 0);
		events = V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ;
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_TX_NOTIFY, &events);
		*handled = true;
	}

	/*
	 * Receiver interrupt service
	 */
	kror = 0;
	if ((rse && rsr) || (rte && rto)) {
		/*
		 * Receive data on RSR to clear the STATS_RSR.
		 * Receive data on RTO, since we may not have yet hit the RSR
		 * watermark when we receive the RTO.
		 */
		for (i = 0, v = FIFO_RX_NDV;
		     (v & FIFO_RX_NDV) && !kror; i = 0) {
			for (j = 0;
			     (v & FIFO_RX_NDV) && j < FIFO_RX_DEPTH; j++) {
				v = cx23888_ir_read4(dev, CX23888_IR_FIFO_REG);
				rx_data[i].hw_fifo_data = v & ~FIFO_RX_NDV;
				i++;
			}
			if (i == 0)
				break;
			j = i * sizeof(union cx23888_ir_fifo_rec);
			k = kfifo_in_locked(&state->rx_kfifo,
				      (unsigned char *) rx_data, j,
				      &state->rx_kfifo_lock);
			if (k != j)
				kror++; /* rx_kfifo over run */
		}
		*handled = true;
	}

	events = 0;
	v = 0;
	if (kror) {
		events |= V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver software FIFO overrun\n");
	}
	if (roe && ror) {
		/*
		 * The RX FIFO Enable (CNTRL_RFE) must be toggled to clear
		 * the Rx FIFO Over Run status (STATS_ROR)
		 */
		v |= CNTRL_RFE;
		events |= V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver hardware FIFO overrun\n");
	}
	if (rte && rto) {
		/*
		 * The IR Receiver Enable (CNTRL_RXE) must be toggled to clear
		 * the Rx Pulse Width Timer Time Out (STATS_RTO)
		 */
		v |= CNTRL_RXE;
		events |= V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED;
	}
	if (v) {
		/* Clear STATS_ROR & STATS_RTO as needed by resetting hardware */
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl & ~v);
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl);
		*handled = true;
	}

	spin_lock_irqsave(&state->rx_kfifo_lock, flags);
	if (kfifo_len(&state->rx_kfifo) >= CX23888_IR_RX_KFIFO_SIZE / 2)
		events |= V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ;
	spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);

	if (events)
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_RX_NOTIFY, &events);
	return 0;
}

/* Receiver */
static int cx23888_ir_rx_read(struct v4l2_subdev *sd, u8 *buf, size_t count,
			      ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	bool invert = (bool) atomic_read(&state->rx_invert);
	u16 divider = (u16) atomic_read(&state->rxclk_divider);

	unsigned int i, n;
	union cx23888_ir_fifo_rec *p;
	unsigned u, v, w;

	n = count / sizeof(union cx23888_ir_fifo_rec)
		* sizeof(union cx23888_ir_fifo_rec);
	if (n == 0) {
		*num = 0;
		return 0;
	}

	n = kfifo_out_locked(&state->rx_kfifo, buf, n, &state->rx_kfifo_lock);

	n /= sizeof(union cx23888_ir_fifo_rec);
	*num = n * sizeof(union cx23888_ir_fifo_rec);

	for (p = (union cx23888_ir_fifo_rec *) buf, i = 0; i < n; p++, i++) {

		if ((p->hw_fifo_data & FIFO_RXTX_RTO) == FIFO_RXTX_RTO) {
			/* Assume RTO was because of no IR light input */
			u = 0;
			w = 1;
		} else {
			u = (p->hw_fifo_data & FIFO_RXTX_LVL) ? 1 : 0;
			if (invert)
				u = u ? 0 : 1;
			w = 0;
		}

		v = (unsigned) pulse_width_count_to_ns(
				  (u16)(p->hw_fifo_data & FIFO_RXTX), divider) / 1000;
		if (v > IR_MAX_DURATION)
			v = IR_MAX_DURATION;

		p->ir_core_data = (struct ir_raw_event)
			{ .pulse = u, .duration = v, .timeout = w };

		v4l2_dbg(2, ir_888_debug, sd, "rx read: %10u ns  %s  %s\n",
			 v, u ? "mark" : "space", w ? "(timed out)" : "");
		if (w)
			v4l2_dbg(2, ir_888_debug, sd, "rx read: end of rx\n");
	}
	return 0;
}

static int cx23888_ir_rx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->rx_params_lock);
	memcpy(p, &state->rx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->rx_params_lock);

	/* Disable or slow down all IR Rx circuits and counters */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);
	control_rx_demodulation_enable(dev, false);
	control_rx_s_edge_detection(dev, CNTRL_EDG_NONE);
	filter_rx_s_min_width(dev, 0);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, RXCLK_RCD);

	state->rx_params.shutdown = true;

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->rx_params;
	u16 rxclk_divider;

	if (p->shutdown)
		return cx23888_ir_rx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->rx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the receiver */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);

	control_rx_demodulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = rxclk_rx_s_carrier(dev, p->carrier_freq,
						     &rxclk_divider);

		o->carrier_freq = p->carrier_freq;

		o->duty_cycle = p->duty_cycle = 50;

		control_rx_s_carrier_window(dev, p->carrier_freq,
					    &p->carrier_range_lower,
					    &p->carrier_range_upper);
		o->carrier_range_lower = p->carrier_range_lower;
		o->carrier_range_upper = p->carrier_range_upper;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, rxclk_divider);
	} else {
		p->max_pulse_width =
			    rxclk_rx_s_max_pulse_width(dev, p->max_pulse_width,
						       &rxclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->rxclk_divider, rxclk_divider);

	p->noise_filter_min_width =
			  filter_rx_s_min_width(dev, p->noise_filter_min_width);
	o->noise_filter_min_width = p->noise_filter_min_width;

	p->resolution = clock_divider_to_resolution(rxclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_rx_irq_watermark(dev, RX_FIFO_HALF_FULL);

	control_rx_s_edge_detection(dev, CNTRL_EDG_BOTH);

	o->invert_level = p->invert_level;
	atomic_set(&state->rx_invert, p->invert_level);

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		unsigned long flags;

		spin_lock_irqsave(&state->rx_kfifo_lock, flags);
		kfifo_reset(&state->rx_kfifo);
		/* reset tx_fifo too if there is one... */
		spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);
		if (p->interrupt_enable)
			irqenable_rx(dev, IRQEN_RSE | IRQEN_RTE | IRQEN_ROE);
		control_rx_enable(dev, p->enable);
	}

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

/* Transmitter */
static int cx23888_ir_tx_write(struct v4l2_subdev *sd, u8 *buf, size_t count,
			       ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	/* For now enable the Tx FIFO Service interrupt & pretend we did work */
	irqenable_tx(dev, IRQEN_TSE);
	*num = count;
	return 0;
}

static int cx23888_ir_tx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->tx_params_lock);
	memcpy(p, &state->tx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->tx_params_lock);

	/* Disable or slow down all IR Tx circuits and counters */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);
	control_tx_modulation_enable(dev, false);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, TXCLK_TCD);

	state->tx_params.shutdown = true;

	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->tx_params;
	u16 txclk_divider;

	if (p->shutdown)
		return cx23888_ir_tx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->tx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the transmitter */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);

	control_tx_modulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = txclk_tx_s_carrier(dev, p->carrier_freq,
						     &txclk_divider);
		o->carrier_freq = p->carrier_freq;

		p->duty_cycle = cduty_tx_s_duty_cycle(dev, p->duty_cycle);
		o->duty_cycle = p->duty_cycle;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, txclk_divider);
	} else {
		p->max_pulse_width =
			    txclk_tx_s_max_pulse_width(dev, p->max_pulse_width,
						       &txclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->txclk_divider, txclk_divider);

	p->resolution = clock_divider_to_resolution(txclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_tx_irq_watermark(dev, TX_FIFO_HALF_EMPTY);

	control_tx_polarity_invert(dev, p->invert_carrier_sense);
	o->invert_carrier_sense = p->invert_carrier_sense;

	control_tx_level_invert(dev, p->invert_level);
	o->invert_level = p->invert_level;

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		if (p->interrupt_enable)
			irqenable_tx(dev, IRQEN_TSE);
		control_tx_enable(dev, p->enable);
	}

	mutex_unlock(&state->tx_params_lock);
	return 0;
}


/*
 * V4L2 Subdevice Core Ops
 */
static int cx23888_ir_log_status(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	char *s;
	int i, j;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 txclk = cx23888_ir_read4(dev, CX23888_IR_TXCLK_REG) & TXCLK_TCD;
	u32 rxclk = cx23888_ir_read4(dev, CX23888_IR_RXCLK_REG) & RXCLK_RCD;
	u32 cduty = cx23888_ir_read4(dev, CX23888_IR_CDUTY_REG) & CDUTY_CDC;
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 filtr = cx23888_ir_read4(dev, CX23888_IR_FILTR_REG) & FILTR_LPF;

	v4l2_info(sd, "IR Receiver:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_RXE ? "yes" : "no");
	v4l2_info(sd, "\tDemodulation from a carrier:       %s\n",
		  cntrl & CNTRL_DMD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_RFE ? "enabled" : "disabled");
	switch (cntrl & CNTRL_EDG) {
	case CNTRL_EDG_NONE:
		s = "disabled";
		break;
	case CNTRL_EDG_FALL:
		s = "falling edge";
		break;
	case CNTRL_EDG_RISE:
		s = "rising edge";
		break;
	case CNTRL_EDG_BOTH:
		s = "rising & falling edges";
		break;
	default:
		s = "??? edge";
		break;
	}
	v4l2_info(sd, "\tPulse timers' start/stop trigger:  %s\n", s);
	v4l2_info(sd, "\tFIFO data on pulse timer overflow: %s\n",
		  cntrl & CNTRL_R ? "not loaded" : "overflow marker");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_RIC ? "not empty" : "half full or greater");
	v4l2_info(sd, "\tLoopback mode:                     %s\n",
		  cntrl & CNTRL_LBM ? "loopback active" : "normal receive");
	if (cntrl & CNTRL_DMD) {
		v4l2_info(sd, "\tExpected carrier (16 clocks):      %u Hz\n",
			  clock_divider_to_carrier_freq(rxclk));
		switch (cntrl & CNTRL_WIN) {
		case CNTRL_WIN_3_3:
			i = 3;
			j = 3;
			break;
		case CNTRL_WIN_4_3:
			i = 4;
			j = 3;
			break;
		case CNTRL_WIN_3_4:
			i = 3;
			j = 4;
			break;
		case CNTRL_WIN_4_4:
			i = 4;
			j = 4;
			break;
		default:
			i = 0;
			j = 0;
			break;
		}
		v4l2_info(sd, "\tNext carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz\n",
			  i, j,
			  clock_divider_to_freq(rxclk, 16 + j),
			  clock_divider_to_freq(rxclk, 16 - i));
	}
	v4l2_info(sd, "\tMax measurable pulse width:        %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, rxclk),
		  pulse_width_count_to_ns(FIFO_RXTX, rxclk));
	v4l2_info(sd, "\tLow pass filter:                   %s\n",
		  filtr ? "enabled" : "disabled");
	if (filtr)
		v4l2_info(sd, "\tMin acceptable pulse width (LPF):  %u us, %u ns\n",
			  lpf_count_to_us(filtr),
			  lpf_count_to_ns(filtr));
	v4l2_info(sd, "\tPulse width timer timed-out:       %s\n",
		  stats & STATS_RTO ? "yes" : "no");
	v4l2_info(sd, "\tPulse width timer time-out intr:   %s\n",
		  irqen & IRQEN_RTE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO overrun:                      %s\n",
		  stats & STATS_ROR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO overrun interrupt:            %s\n",
		  irqen & IRQEN_ROE ? "enabled" : "disabled");
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_RBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_RSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_RSE ? "enabled" : "disabled");

	v4l2_info(sd, "IR Transmitter:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_TXE ? "yes" : "no");
	v4l2_info(sd, "\tModulation onto a carrier:         %s\n",
		  cntrl & CNTRL_MOD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_TFE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_TIC ? "not empty" : "half full or less");
	v4l2_info(sd, "\tOutput pin level inversion         %s\n",
		  cntrl & CNTRL_IVO ? "yes" : "no");
	v4l2_info(sd, "\tCarrier polarity:                  %s\n",
		  cntrl & CNTRL_CPL ? "space:burst mark:noburst"
				    : "space:noburst mark:burst");
	if (cntrl & CNTRL_MOD) {
		v4l2_info(sd, "\tCarrier (16 clocks):               %u Hz\n",
			  clock_divider_to_carrier_freq(txclk));
		v4l2_info(sd, "\tCarrier duty cycle:                %2u/16\n",
			  cduty + 1);
	}
	v4l2_info(sd, "\tMax pulse width:                   %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, txclk),
		  pulse_width_count_to_ns(FIFO_RXTX, txclk));
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_TBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_TSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_TSE ? "enabled" : "disabled");

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int cx23888_ir_g_register(struct v4l2_subdev *sd,
				 struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	reg->size = 4;
	reg->val = cx23888_ir_read4(state->dev, addr);
	return 0;
}

static int cx23888_ir_s_register(struct v4l2_subdev *sd,
				 const struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	cx23888_ir_write4(state->dev, addr, reg->val);
	return 0;
}
#endif

static const struct v4l2_subdev_core_ops cx23888_ir_core_ops = {
	.log_status = cx23888_ir_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = cx23888_ir_g_register,
	.s_register = cx23888_ir_s_register,
#endif
	.interrupt_service_routine = cx23888_ir_irq_handler,
};

static const struct v4l2_subdev_ir_ops cx23888_ir_ir_ops = {
	.rx_read = cx23888_ir_rx_read,
	.rx_g_parameters = cx23888_ir_rx_g_parameters,
	.rx_s_parameters = cx23888_ir_rx_s_parameters,

	.tx_write = cx23888_ir_tx_write,
	.tx_g_parameters = cx23888_ir_tx_g_parameters,
	.tx_s_parameters = cx23888_ir_tx_s_parameters,
};

static const struct v4l2_subdev_ops cx23888_ir_controller_ops = {
	.core = &cx23888_ir_core_ops,
	.ir = &cx23888_ir_ir_ops,
};

static const struct v4l2_subdev_ir_parameters default_rx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5, RC-6, and RC-6A carrier */

	/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
	/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
	.noise_filter_min_width = 333333, /* ns */
	.carrier_range_lower = 35000,
	.carrier_range_upper = 37000,
	.invert_level = false,
};

static const struct v4l2_subdev_ir_parameters default_tx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5 carrier */
	.duty_cycle = 25,      /* 25 %   - RC-5 carrier */
	.invert_level = false,
	.invert_carrier_sense = false,
};

int cx23888_ir_probe(struct cx23885_dev *dev)
{
	struct cx23888_ir_state *state;
	struct v4l2_subdev *sd;
	struct v4l2_subdev_ir_parameters default_params;
	int ret;

	state = kzalloc(sizeof(struct cx23888_ir_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	spin_lock_init(&state->rx_kfifo_lock);
	if (kfifo_alloc(&state->rx_kfifo, CX23888_IR_RX_KFIFO_SIZE,
			GFP_KERNEL)) {
		kfree(state);
		return -ENOMEM;
	}

	state->dev = dev;
	sd = &state->sd;

	v4l2_subdev_init(sd, &cx23888_ir_controller_ops);
	v4l2_set_subdevdata(sd, state);
	/* FIXME - fix the formatting of dev->v4l2_dev.name and use it */
	snprintf(sd->name, sizeof(sd->name), "%s/888-ir", dev->name);
	sd->grp_id = CX23885_HW_888_IR;

	ret = v4l2_device_register_subdev(&dev->v4l2_dev, sd);
	if (ret == 0) {
		/*
		 * Ensure no interrupts arrive from '888 specific conditions,
		 * since we ignore them in this driver to have commonality with
		 * similar IR controller cores.
		 */
		cx23888_ir_write4(dev, CX23888_IR_IRQEN_REG, 0);

		mutex_init(&state->rx_params_lock);
		default_params = default_rx_params;
		v4l2_subdev_call(sd, ir, rx_s_parameters, &default_params);

		mutex_init(&state->tx_params_lock);
		default_params = default_tx_params;
		v4l2_subdev_call(sd, ir, tx_s_parameters, &default_params);
	} else {
		kfifo_free(&state->rx_kfifo);
	}
	return ret;
}

int cx23888_ir_remove(struct cx23885_dev *dev)
{
	struct v4l2_subdev *sd;
	struct cx23888_ir_state *state;

	sd = cx23885_find_hw(dev, CX23885_HW_888_IR);
	if (sd == NULL)
		return -ENODEV;

	cx23888_ir_rx_shutdown(sd);
	cx23888_ir_tx_shutdown(sd);

	state = to_state(sd);
	v4l2_device_unregister_subdev(sd);
	kfifo_free(&state->rx_kfifo);
	kfree(state);
	/* Nothing more to free() as state held the actual v4l2_subdev object */
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23888_IR_H_
#define _CX23888_IR_H_
int cx23888_ir_probe(struct cx23885_dev *dev);
int cx23888_ir_remove(struct cx23885_dev *dev);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23888_IR_H_
#define _CX23888_IR_H_
int cx23888_ir_probe(struct cx23885_dev *dev);
int cx23888_ir_remove(struct cx23885_dev *dev);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later

/*
 * netup-eeprom.c
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#
#include "cx23885.h"
#include "netup-eeprom.h"

#define EEPROM_I2C_ADDR 0x50

int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr)
{
	int ret;
	unsigned char buf[2];

	/* Read from EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &buf[0],
			.len	= 1
		}, {
			.addr	= EEPROM_I2C_ADDR,
			.flags	= I2C_M_RD,
			.buf	= &buf[1],
			.len	= 1
		}

	};

	buf[0] = addr;
	buf[1] = 0x0;

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		pr_err("eeprom i2c read error, status=%d\n", ret);
		return -1;
	}

	return buf[1];
};

int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data)
{
	int ret;
	unsigned char bufw[2];

	/* Write into EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &bufw[0],
			.len	= 2
		}
	};

	bufw[0] = addr;
	bufw[1] = data;

	ret = i2c_transfer(i2c_adap, msg, 1);

	if (ret != 1) {
		pr_err("eeprom i2c write error, status=%d\n", ret);
		return -1;
	}

	mdelay(10); /* prophylactic delay, datasheet write cycle time = 5 ms */
	return 0;
};

void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo)
{
	int i, j;

	cinfo->rev =  netup_eeprom_read(i2c_adap, 63);

	for (i = 64, j = 0; i < 70; i++, j++)
		cinfo->port[0].mac[j] =  netup_eeprom_read(i2c_adap, i);

	for (i = 70, j = 0; i < 76; i++, j++)
		cinfo->port[1].mac[j] =  netup_eeprom_read(i2c_adap, i);
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             // SPDX-License-Identifier: GPL-2.0-or-later

/*
 * netup-eeprom.c
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#
#include "cx23885.h"
#include "netup-eeprom.h"

#define EEPROM_I2C_ADDR 0x50

int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr)
{
	int ret;
	unsigned char buf[2];

	/* Read from EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &buf[0],
			.len	= 1
		}, {
			.addr	= EEPROM_I2C_ADDR,
			.flags	= I2C_M_RD,
			.buf	= &buf[1],
			.len	= 1
		}

	};

	buf[0] = addr;
	buf[1] = 0x0;

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		pr_err("eeprom i2c read error, status=%d\n", ret);
		return -1;
	}

	return buf[1];
};

int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data)
{
	int ret;
	unsigned char bufw[2];

	/* Write into EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &bufw[0],
			.len	= 2
		}
	};

	bufw[0] = addr;
	bufw[1] = data;

	ret = i2c_transfer(i2c_adap, msg, 1);

	if (ret != 1) {
		pr_err("eeprom i2c write error, status=%d\n", ret);
		return -1;
	}

	mdelay(10); /* prophylactic delay, datasheet write cycle time = 5 ms */
	return 0;
};

void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo)
{
	int i, j;

	cinfo->rev =  netup_eeprom_read(i2c_adap, 63);

	for (i = 64, j = 0; i < 70; i++, j++)
		cinfo->port[0].mac[j] =  netup_eeprom_read(i2c_adap, i);

	for (i = 70, j = 0; i < 76; i++, j++)
		cinfo->port[1].mac[j] =  netup_eeprom_read(i2c_adap, i);
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-eeprom.h
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef NETUP_EEPROM_H
#define NETUP_EEPROM_H

struct netup_port_info {
	u8 mac[6];/* card MAC address */
};

struct netup_card_info {
	struct netup_port_info port[2];/* ports - 1,2 */
	u8 rev;/* card revision */
};

extern int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr);
extern int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data);
extern void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-eeprom.h
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef NETUP_EEPROM_H
#define NETUP_EEPROM_H

struct netup_port_info {
	u8 mac[6];/* card MAC address */
};

struct netup_card_info {
	struct netup_port_info port[2];/* ports - 1,2 */
	u8 rev;/* card revision */
};

extern int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr);
extern int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data);
extern void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * netup-init.c
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "netup-init.h"

static void i2c_av_write(struct i2c_adapter *i2c, u16 reg, u8 val)
{
	int ret;
	u8 buf[3];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 3
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static void i2c_av_write4(struct i2c_adapter *i2c, u16 reg, u32 val)
{
	int ret;
	u8 buf[6];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 6
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val & 0xff;
	buf[3] = (val >> 8) & 0xff;
	buf[4] = (val >> 16) & 0xff;
	buf[5] = val >> 24;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static u8 i2c_av_read(struct i2c_adapter *i2c, u16 reg)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 2
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);

	msg.flags = I2C_M_RD;
	msg.len = 1;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c read error!\n", __func__);

	return buf[0];
}

static void i2c_av_and_or(struct i2c_adapter *i2c, u16 reg, unsigned and_mask,
								u8 or_value)
{
	i2c_av_write(i2c, reg, (i2c_av_read(i2c, reg) & and_mask) | or_value);
}
/* set 27MHz on AUX_CLK */
void netup_initialize(struct cx23885_dev *dev)
{
	struct cx23885_i2c *i2c_bus = &dev->i2c_bus[2];
	struct i2c_adapter *i2c = &i2c_bus->i2c_adap;

	/* Stop microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x00);

	/* Aux PLL frac for 27 MHz */
	i2c_av_write4(i2c, 0x114, 0xea0eb3);

	/* Aux PLL int for 27 MHz */
	i2c_av_write4(i2c, 0x110, 0x090319);

	/* start microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x10);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * netup-init.c
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "netup-init.h"

static void i2c_av_write(struct i2c_adapter *i2c, u16 reg, u8 val)
{
	int ret;
	u8 buf[3];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 3
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static void i2c_av_write4(struct i2c_adapter *i2c, u16 reg, u32 val)
{
	int ret;
	u8 buf[6];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 6
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val & 0xff;
	buf[3] = (val >> 8) & 0xff;
	buf[4] = (val >> 16) & 0xff;
	buf[5] = val >> 24;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static u8 i2c_av_read(struct i2c_adapter *i2c, u16 reg)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 2
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);

	msg.flags = I2C_M_RD;
	msg.len = 1;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c read error!\n", __func__);

	return buf[0];
}

static void i2c_av_and_or(struct i2c_adapter *i2c, u16 reg, unsigned and_mask,
								u8 or_value)
{
	i2c_av_write(i2c, reg, (i2c_av_read(i2c, reg) & and_mask) | or_value);
}
/* set 27MHz on AUX_CLK */
void netup_initialize(struct cx23885_dev *dev)
{
	struct cx23885_i2c *i2c_bus = &dev->i2c_bus[2];
	struct i2c_adapter *i2c = &i2c_bus->i2c_adap;

	/* Stop microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x00);

	/* Aux PLL frac for 27 MHz */
	i2c_av_write4(i2c, 0x114, 0xea0eb3);

	/* Aux PLL int for 27 MHz */
	i2c_av_write4(i2c, 0x110, 0x090319);

	/* start microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x10);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      ¬2     4     (            GNU  À       À          èüÿÿÿWV‰Æ‰Ğ‰ÊS‹Ü   ‹|$‹™  ƒãıƒø‹Ü   Àƒàş  	Ø‰  ‹†Ü      ‹ 1Û€ä_…Ò•ÃÁã	Ã‹†Ü   ‰˜   ‰Ø‰ÚÁøƒà…ÿEø‹†Ü   €Î ‰   ¸Ç  èüÿÿÿ‹†Ü   ‰˜   ‰ø[^_ÃfèüÿÿÿS‰ÃÃ¼  ƒìHd¡    ‰D$D‰àèüÿÿÿ‹D$,=9‰ ‡L  =7‰ ‡   =Õˆ ‡6  =Óˆ ‡   =Q… ‡   =O… ‡   =í„ ‡
  =ë„ ‡   =Wu „   ‡î  =9K „   ‡İ  =ÅØ „   ‡Ì  =»Ø „   ‡»  =ÙN „   ‡ª  =L „   ‡™  =o> „   ‡ˆ  =e> „   ‡w  =“8 „   ‡f  =77 „   ‡U  =Ó6 „   ‡D  =É6 „   ‡3  =6 „   ‡"  =ı4 „   ‡  =™4 „   ‡   ='3 „   =Ã2 ‡ê   =¤2 ‡?   =»0 „   ‡Î   =û, „   ‡½   =ñ, „   ‡¬   =Ó, „   ‡›   =É, „   ‡Š   =9+ „   w}=? „   wp=+ „   = w^= ‡W   = „   wF= „   w9=¿ „   w,=¼ „   w=a „   w=V  ‡üÿÿÿ=ğU  wPSh    èüÿÿÿƒÄé   éo   ´&    v èüÿÿÿQÿUWVSƒú†Ó   ‹8‰Î…É…í   ‹¸  ƒùwƒùws[‰ğ^_]Ãƒù„Í   ƒù,„®   wåƒù*u¸ûÿÿÿ½   ‹—Ü   Â  ‹
!È‰»È   ¶    ¸X‰A èüÿÿÿƒëuñ‹‡Ü     ‹€  	Å‰*[‰ğ^_]Ãº   Óâ÷Â`2 u€æu@ƒù…pÿÿÿ¸ıÿÿÿ½   ëv ƒéƒá÷„uÿÿÿéPÿÿÿ´&    1ö[‰ğ^_]Ã´&    ‹hƒıtƒı…(ÿÿÿéCÿÿÿ‹P‰øèüÿÿÿ‰ğ[^_]Ã¸şÿÿÿé2ÿÿÿ´&    èüÿÿÿS‰Ã‹€¸  ƒø„	  wwƒø	„#  †   ƒø„'  †Q  ƒøt†  ƒø„½  ¹   ‰Øº `  èüÿÿÿº `  ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº @  èüÿÿÿ¸d   [éüÿÿÿ´&    ƒø)‡  ƒø'‡  ƒø!„Ã  †G  ƒø&„r  ƒø'…é  ‹ƒÜ   (  ‹ƒâş‰‹ƒÜ     ‹Ê   ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âşÿşÿ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âüÿüÿ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âıÿıÿ‰‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âûÿûÿ‰‹ƒÜ     ‹€    ‰¸<   [éüÿÿÿ´&    fƒøvPƒø„î   ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   ‰Øèüÿÿÿ¸d   [éüÿÿÿƒø$…[  ¹   ‰Øº`  èüÿÿÿ‰Øº`  èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ‰Øº   [éüÿÿÿv ƒø„/  v]ƒøuT‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâû‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ãfƒøt£ƒøuò‹ƒÜ     ‹€    ‰[Ãv ƒø„  †  ƒø†hÿÿÿƒøu·‹ƒÜ   º6   ‰(  º   ‹ƒÜ   ‰$  ‹ƒÜ      ‹ƒÊ‰¸È   èüÿÿÿ‹ƒÜ      ‹€æ÷‰¸È   èüÿÿÿ‹ƒÜ      ‹€   €Ì‰¸È   [éüÿÿÿ´&    ƒø„·  †É   ƒøt	ƒø…ÿÿÿ‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâú‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ãƒø
…¸şÿÿ‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâğ‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ã¶    ƒø„aşÿÿƒø„nÿÿÿƒø…Aşÿÿ¹   ‰Øº À  èüÿÿÿº À  ‰Øèüÿÿÿ¸d   èüÿÿÿº À  ‰Øèüÿÿÿ¸d   èüÿÿÿ¹   ‰Øº   èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ‰Øº   èüÿÿÿ¸   [éüÿÿÿt& ƒø1„÷  †   ƒø5w\ƒø3wc‹ƒÜ   º7   ‰(  ¹   ‰Øº  èüÿÿÿº  ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº  [éüÿÿÿfƒø%„Œıÿÿé>ıÿÿfƒè8ƒø‡0ıÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   é=üÿÿƒø-„cÿÿÿ†ª   ƒø1„çüÿÿ‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âûÿşÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âıÿşÿ‰º7   ‹ƒÜ   ‰(  º   ‹ƒÜ   ‰$  ‹ƒÜ   º Ã  ‰   1Ò‹ƒÜ   ‰  [Ãƒø*„éûÿÿƒø,„|ıÿÿ[Ã¶    ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹âùÿşÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹ƒÊ‰º7   ‹ƒÜ   ‰(  º P  ‹ƒÜ   ‰$  º   ‹ƒÜ   éXÿÿÿ´&    ¹   ‰Øº   èüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº   [éüÿÿÿt& ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹ƒâú‰¸   èüÿÿÿébüÿÿf¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   éIúÿÿ´&    ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹âûÿüÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹Ê  é-şÿÿ´&    t& èüÿÿÿUWVS‰Ãƒì0‹‹¸  d¡    ‰D$,1Àƒù„w  vEƒéƒùw¸   Óà© ãud¨…×  ƒùt71ö‹D$,d+    …±  ƒÄ0‰ğ[^_]Ãt& ƒù„§   1öƒùvĞƒùuh    1öjèüÿÿÿXZë¹´&    fƒùu«‹5   …ötŸº   ‰Øèüÿÿÿ‰ƒ¨  …À„?  ‹ƒ”  …À„wÿÿÿ‹Pd‹…Ò„jÿÿÿ‹Z…Û„_ÿÿÿ‹    ¹   …Ò„æ  ‹r…ö„Û  º   èüÿÿÿ1öé2ÿÿÿf‹   …É„ ÿÿÿº   ‰Øèüÿÿÿ‰ƒ¨  …À„À  ‹ƒ”  …À„øşÿÿ‹Pd‹…Ò„ëşÿÿ‹Z…Û„àşÿÿ‹    ¹    …Ò„x  ‹r…ö„m  º   èüÿÿÿ1öé³şÿÿv ‰Øèüÿÿÿ‰Æ…À…Ÿşÿÿº   ‰Øèüÿÿÿ‰ƒ¨  ‹ƒ”  …À„}şÿÿ‹Pd‹…Ò„pşÿÿ‹Z…Û„eşÿÿ‹    ¹   …Ò„İ  ‹z…ÿ„Ò  º   èüÿÿÿé:şÿÿ´&    v ‰Øèüÿÿÿ‰Æ…À…şÿÿº   ‰Øèüÿÿÿ‰ƒ¨  ‰Á‹ƒ”  …ÀtA‹Pd‹…Òt8‹z…ÿt1‹    ¹    …Ò„@  ‹j…í„5  º   èüÿÿÿ‹‹¨  v …É„¶ıÿÿ‹Ad‹@…À„Ú   ‹x…ÿ„Ï   ¡   ‰â…À„Ô   ‹h…í„É   ‰Èèüÿÿÿ‹‹¨  ÆD$ ÆD$
 ÆD$…É„_ıÿÿ‹Ad‹@…Àtw‹x…ÿtp¡   ‰â…À„¸   ‹h…í„­   ‰Èèüÿÿÿ‹‹¨  ÆD$
…É„ıÿÿ‹Ad‹@…À„ıÿÿ‹X…Û„ıÿÿ¡   ‰â…ÀtN‹x…ÿtG‰Èèüÿÿÿéåüÿÿt& ÆD$
ë½´&    fÆD$ ÆD$
 ÆD$é[ÿÿÿ‰Èèüÿÿÿ‹‹¨  é2ÿÿÿ‰Èèüÿÿÿéüÿÿº   èüÿÿÿ‹‹¨  éÉşÿÿ‰Èèüÿÿÿ‹‹¨  éNÿÿÿº   èüÿÿÿéhüÿÿº   1öèüÿÿÿéWüÿÿº   èüÿÿÿéFüÿÿ¾íÿÿÿé>üÿÿèüÿÿÿ´&    fèüÿÿÿ‹ˆ¸  ƒù3wpS‰Ãƒùvƒé¸   Óà© ãu¨u3[Ãƒùt
ƒùt&ƒùuî‰Øº   èüÿÿÿÇƒ¨      [Ã´&    f‰Øº   èüÿÿÿ‰ØèüÿÿÿÇƒ¨      [ÃÃ´&    ´&    èüÿÿÿ‹ˆ¸  ƒù3wƒùvƒéº   Óâ÷Â ãuƒâu,Ãv ƒùt
ƒùtƒùuí‹¨  …Òtãº   éüÿÿÿt& ‹ˆ¨  …ÉtÊº   éüÿÿÿt& èüÿÿÿWVS‰Ãƒì‹»`  d¡    ‰D$1À…ÿ„9  ‹ƒ¸  ‰Âƒø	t‡æ  ƒø„¬  Hûƒù‡1  ‰Âƒø&†  ƒø2„µ  †ç  ƒø5„f  †Ø  ƒø;„ˆ  †¶  HÄƒù‡\  ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     ƒø"‡½  Jú¸   Óà©ÅXß„³  j “
  ¹9   CjDèüÿÿÿ‰ƒ”  ZY…À„Ô  “ğ   ‰˜   ‹ƒ”  Ç€      ‹ƒ”  …À„ª  ‹Pd‹…Ò„  ‹R…Ò„’  ‹    …É„|  ‹I…É„q  èüÿÿÿ‹“¸  éo  ƒøt3†Û   Höƒù‡X  Çƒ     Çƒ      Çƒ$     v ÇƒØ     ÇƒÜ     Çƒà     ƒú"‡¶  ƒú‡ğşÿÿBÿƒø†ùşÿÿƒú„"  ƒú„  ‹D$d+    …3  ƒÄ[^_ÃfHÊƒù‡%şÿÿ¾P   ƒô  ¹   º    f‰³ö  èüÿÿÿ‹»`  …ÿ„
  ‹ƒ¸  ééıÿÿƒø…€  ÇƒØ    ‹ƒÜ   º   ÇƒÜ     Çƒà     Çƒä      Çƒè  ÌG ‰„ Çƒ     ‹“¸  Çƒ      Çƒ$     éûşÿÿt& èüÿÿÿv ‹“¸  ƒú6…Á   j “ü   C¹A   jAèüÿÿÿ‹“¸  _Xƒú…Şşÿÿ‹D$d+    …  ƒÄ‰Ø[^_éüÿÿÿHÈƒù†Jıÿÿé¡  fƒø3…§   ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     BÜƒø‡Mÿÿÿ¹ùş}£Á‚>ıÿÿƒú6„?ÿÿÿƒú7…Uÿÿÿj »ü   s¹A   j@‰ú‰ğèüÿÿÿ‰ú¹A   ‰ğj jAèüÿÿÿ‹“¸  ƒÄéÿÿÿt& ƒø4…æ  ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     éKÿÿÿ¶    ƒø)wKƒø'…  ÇƒØ     ÇƒÜ     Çƒà     Çƒ    Çƒ      Çƒ$     éNüÿÿƒø-„°şÿÿv,ƒø2„;  ÇƒØ     ÇƒÜ     Çƒà     éøûÿÿƒø,„·üÿÿé
  v ƒø‡×   ƒø„ö  †€üÿÿƒø„fıÿÿ†	  ƒø…×   ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     é­üÿÿ´&    ¾P   º    ƒl  ¹   f‰³n  èüÿÿÿ‹ƒ¸  ‰Âƒø†®   HÜƒù‡³úÿÿ¾   Óæ‰ñ÷Æ… ó…H  á   …müÿÿƒø&‡’úÿÿHâ¾   Óæ‰ñöÁ¨…×şÿÿƒá…­úÿÿƒø&„¡  Çƒ     ‰ÂÇƒ      Çƒ$     éÂûÿÿt& ƒø„rúÿÿvHêƒù†…şÿÿë¾´&    …vşÿÿë¯…Àt«‰Á¾   Óæ‰ñ÷Æ†<……   á`  u]ƒø……şÿÿ‹‹`  …É…uÿÿÿ€=    „tKºà   ‰ØèÑçÿÿ‹ƒ¸  ‰Âéµùÿÿt& ƒú…müÿÿ‹5   …ö„Oûÿÿé1úÿÿt& ‹“`  …Ò…ùÿÿº    ‰Øè†çÿÿé|ûÿÿ‹‹`  …Ét™éşÿÿÇƒØ     ÇƒÜ     Çƒà     éóúÿÿt& ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     é”ùÿÿ´&    ÇƒØ     ‹ƒÜ   ÇƒÜ     Çƒà       Çƒ     Çƒ      Çƒ$     ‹Ê  ‰¹   º'  ¸è  èüÿÿÿ‹ƒÜ     ‹ƒâı‰¹   º'  ¸è  1öèüÿÿÿv ‹ƒÜ     ‹ƒâø‰¹   º'  ¸è  èüÿÿÿ‹ƒÜ     ‹:‰ğ‰ñÁø÷Ñ¶€    ƒáÓø‰Áƒá‰ÈƒÈ÷Ğ!ø	ÈƒÈ‰¹   º'  ¸è  ƒÆèüÿÿÿƒşHu†‹ƒÜ     ‹ƒÊ‰‹“¸  éOùÿÿ´&    fÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     é5ùÿÿ´&    ÇƒØ     ÇƒÜ     Çƒà     éùÿÿƒ»`   „{ıÿÿéD÷ÿÿ‰Ø‰\$ÇD$H   ÇD$    èüÿÿÿƒü   º    èüÿÿÿ¡ŒF  …À„6  é1  èüÿÿÿèüÿÿÿ‹P(Ç   ‹‚ì  ¯‚ğ  ‹’è  ¯B‹T$Áè‰1ÀÃ´&    ´&    èüÿÿÿUWVS‰Ãƒì‹ ‹“ˆ  ‹p(‹ƒ  ƒÀ¾¸  ‰B‹ƒŒ  Ç   p‹ƒ  ‹“Œ  ƒÀ‰B‹ƒŒ  Ç@    ‰øèüÿÿÿ‹®ø  ø  ‰Âƒx  9étS‹«ˆ  M    ‹®ü  ‰†ü  ‰‹x  ‰«|  ‰E ‹E‹‹  ƒ=x  ‰H‡ø  ƒÄ‰ø[^_]éüÿÿÿt& ‹®ü  ‰†ü  ‰‹x  ‰«|  ‰E ƒ=x  ‡Ö  ƒÄ‰ø[^_]éüÿÿÿfèüÿÿÿUWVS‹h(‹…Ü   @  ‹ƒâî‰½¸  ø  ‰øèüÿÿÿ‰Æ‹…ø  9Ãt@v ‹…ø  ‹P‹-x  ‰Q‰
º   Ç€x     Ç€|  "  èüÿÿÿ‹…ø  9ØuÃ[‰ò‰ø^_]éüÿÿÿ¶    èüÿÿÿVSèüÿÿÿ‹°(  ‹FƒÆ9Æt$X´‹Cd‹ …Àt‹…Òt‰Øèüÿÿÿ‹CLX´9Æuà1À[^Ã´&    t& èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ‹‡¸  ƒø4tƒè8ƒøwPÇF   ‹‡Ü  ƒÇ‰F‹X´9Çt(t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¶    ƒ¿€  u§¸êÿÿÿëæ´&    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ‹‡¸  ƒø4tƒè8ƒøwH‹…ÀuK‹GƒÇX´9Çt)t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¶    ƒ¿€  u¯¸êÿÿÿëæèüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ‹‡¸  ƒø4tƒè8ƒøwX‹…Àu[F¹   ºª  ƒÇèüÿÿÿ‹X´9Çt(t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¶    ƒ¿€  uŸ¸êÿÿÿëæèüÿÿÿS‰Ëèüÿÿÿƒ;‹(  uH‹Cƒèƒøw=ÇC    ÇC    ÇCĞ  ‹‚x  % ù  ƒøÀƒà`à  ‰C1À[Ã´&    f¸êÿÿÿ[Ã´&    fèüÿÿÿV‰ÎS‹\$èüÿÿÿ‹€(  ‹€x  %ÿÿ ƒşu(ƒøÒƒâÕƒÂ6ƒøÀ‰ƒàÏƒÀ;‰C1À[^Ã´&    ¸êÿÿÿëî´&    fèüÿÿÿVS‰Ëèüÿÿÿ‹°(  ¡x  …À…  ‹†x  ‹–|  ‰1À‰S[^Ã´&    fèüÿÿÿVS‰Ëèüÿÿÿ‹(  ‹‚ì  ‰C‹Šğ  ‰K‹²ô  ‰s‹²è  ‹6‰s‹’è  ¯BÇC   Áè¯È‰C1À‰K[^Ãt& èüÿÿÿ‹…Àu¡ÔI  ‰A,1ÀÃ´&    v ¸êÿÿÿÃ´&    v èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿU1ÒWVS‰Ãƒì‹ ‹p(‰Øèüÿÿÿ‹k‰Ç‹†è  ‹@¯†ì  Áè‰ƒ€  ‰Â…í…¼   ¯–ğ  …Ò…  ‹–ô  ƒú„À   wTƒú„+  ƒú…  ‹®Ì   ÿ¶ğ  “„  j P‰èj jÿ‹èüÿÿÿƒÄƒ=x  ‡6  1ÀƒÄ[^_]Ã¶    ƒú„  ƒú…J  ‹ğ  ‹®Ì   “„  ÑéQ¯Èj P‰èj Q‹èüÿÿÿƒÄë¥´&    v ¯–ğ  9ST‚  ‰SP‹–ô  ƒú…@ÿÿÿ‹x  ÷†x   ù  ti¸    ‹‰  …É„´   …Ò…p  Ç$    ‰Á‹–Ì   «„  ‰T$‹–ğ  ÑêR‰êQQPÿt$‹‹D$èüÿÿÿƒÄé	ÿÿÿ¶    ‹®Ì   ÿ¶ğ  “„  j Pjÿj ‹‰èèüÿÿÿƒÄéÙşÿÿ¶    ‹ğ  ‹®Ì   “„  ÑéQ¯Èj PQëÉ´&    v ƒÄ¸êÿÿÿ[^_]Ãv …Ò…’  ‰$‰Á1ÀéIÿÿÿ¸êÿÿÿéŠşÿÿèüÿÿÿVS‰Ëèüÿÿÿ‹5ÔI  ‹€(  9s…­   ‹€x  ‹S% ù  ƒøÀƒà`à  …Òt^ƒúwyƒú…   º   j s¹Ğ  j PC‰Sº0   j P‰ğjèüÿÿÿ‹C¯ØI  ÇC   ƒÄÁè‰C¯C‰C1À[^Ã‰Áº   Ñé9Kw§º   ‰Èë´&    fJüƒù¹   CÑë…¸êÿÿÿ[^Ã´&    Ñè‰Á‰Èéjÿÿÿt& èüÿÿÿU‰ÍWV‰ÖS‰Ãƒìpd¡    ‰D$l‰Øèüÿÿÿ¹   ‹€(  |$ÇD$   ‰$1Àƒ=x  ó«‡´  ‰é‰ò‰Øè·şÿÿ‰D$‰Á…À…U  ‹<$‹Ÿ   …Û…]  ‹—`  …Ò…O  ‹‡Ä  …À…A  ¡ÔI  9E¸ÔI  EÁƒ=x  ‰‡è  ‹E‰‡ì  ‹U‰—ğ  ‹M‰ô  ‡Ğ  ‹E‹]0ÇD$$   ‹<$‹u‹M‹U‰D$(‰D$‹E(ƒÇ‰t$ f‰D$0f‰D$‹E,‰L$,f‰D$2f‰D$·Ãf‰\$4‹‰T$‰\$ƒëL;|$tYt& ‹Cd‹@…Àt‹p…ötL$1Ò‰Øèüÿÿÿ‹CLX´9ÇuØ‹D$(‹T$‹t$ ‹L$,‰D$·D$0f‰D$·D$2f‰D$·D$4‰U‹T$‰E0‹$‰U·T$‰u‰U(·T$‰M‰U,‹€ô  ‰E‹D$ld+    u‹D$ƒÄp[^_]ÃÇD$ğÿÿÿëİèüÿÿÿ´&    t& èüÿÿÿUWVS‰ËèüÿÿÿºĞ  ¹   ‹°(  ‰Øèüÿÿÿº    i†¸    ‹¸    ‰øèüÿÿÿƒøÿ„ù  k‰Áƒø tƒÁƒù ‡ï  ‰ú‰èèüÿÿÿ‹–Ì   ‹‚´   …Àu‹‚ˆ   PC0hØ  PèüÿÿÿÇCT …‹†¸  ƒÄƒø4tƒè8ƒøwÇCT …1À[^_]Ãt& ƒ¾€  uä[1À^_]ÃèüÿÿÿW‰×V‰ÆS‹‹@P‰V‰ƒôşÿÿèüÿÿÿ‰ƒœıÿÿ‰“ ıÿÿƒ=x  ‡  ³ˆıÿÿ‹‹C‰B‰‰ğº   Ç   ÇC"  [^_éüÿÿÿ´&    fèüÿÿÿWVS‰Ã¡x  …À…+  ‹ƒúwrs{1ÀÇC    ƒçü‰ñÇF,    )ùƒÁ0Áéó«‹<•À   º    ‰øèüÿÿÿ‰Áƒøÿ„Q  ƒø tƒÁƒù ‡G  ‰ú‰ğèüÿÿÿÇC$   1À[^_Ã´&    ¸êÿÿÿëí´&    fèüÿÿÿ‰ÈéTÿÿÿt& èüÿÿÿS‰Ëèüÿÿÿ‹(  kŠl  i‚¸    ‹„,   ¹   ƒèƒøv‹Šp  ‰¡x  …À…[  ‰Ø[éûşÿÿ´&    t& èüÿÿÿU‰ÍWV‰ÖS‰Ãƒì¡x  …À…x  èüÿÿÿ‰Â…Àt]‰Ç¹Í   Có¥‰‚¬  ƒ  Ç‚      ‰‚0  iƒ¸    Uÿ°    ‚À  hà  j ‰T$Pèüÿÿÿ‹T$ƒÄ‰š(  ƒÄ‰Ğ[^_]Ã´&    ´&    èüÿÿÿV‹x  S‹X(‹³ø  …Ò…”  ‹ƒÜ   @  ‹ƒâî‰‹“d  ‹Nÿv‰Øèüÿÿÿ‹ƒÜ   º   ‰0  Çƒ       ‰Øº   èüÿÿÿ‹ƒÜ      ‹ƒÊ‰‹ƒÜ      ‹ƒÊ ‰‹ƒÜ   @  ‹€@  ƒÈ‰X1À[^Ãt& èüÿÿÿS‰Ëèüÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…°  1À[Ãv èüÿÿÿ‰Ğ‰Ê‹9Èt	éìüÿÿt& Ã´&    èüÿÿÿUWV‰ÆS‰Ë¹   ƒìd‰$|$d¡    ‰D$`1ÀÇD$   ó«‹=x  ÇD$   …ÿ…6  ‹†|  ‹$3–x  1Ø	Ğ„ÿ   ‹   …É…  ‹–`  …Ò…ú   ‹†Ä  …À…ì   ‹$~‰|  ‰İÇ†ì  Ğ  ‰†x  % ù  Ç†ô     ƒøÀƒà`à  ‰†ğ  ‹FX´9Ç„‡   ‰t$v ‹Cd‹@…Àt‹p…öt‹$‰é‰Øèüÿÿÿ‹CLX´9ÇuÙ‹t$‹ğ  ‹–ô  ‹F‹¶ì  ‰L$‰t$X´‰T$9Çt-t& ‹Cd‹@…Àt‹p…ötL$1Ò‰Øèüÿÿÿ‹CLX´9ÇuØ1À‹T$`d+    u8ƒÄd[^_]Ã¸ğÿÿÿëä‰×‰Ğ‰ÚèüÿÿÿPWh˜  h˜  h0  èüÿÿÿƒÄé¡şÿÿèüÿÿÿt& èüÿÿÿWVS‹t$‹|$èüÿÿÿ‹˜(  ¡x  …À…Í  ‰ò‰ù‰Ø[^_éşÿÿ´&    v èüÿÿÿS
  ƒìd‹    ‰\$1ÛˆT$º   f‰T$T$ˆL$¹   ‰T$‰âÇ$L   èüÿÿÿ‹T$d+    uƒÄ[Ãèüÿÿÿèüÿÿÿƒì 
  d‹    ‰L$1ÉˆT$T$¹   ‰T$º   f‰T$T$f‰L$¹   ‰T$‰âÆD$ Ç$L   ÇD$L  èüÿÿÿƒø…é  ¶D$‹T$d+    uƒÄ Ãèüÿÿÿ´&    v èüÿÿÿWVS‰Ãƒìd¡    ‰D$1À¡x  …À…ÿ  ƒú„©   ƒú…ˆ   º   ‰ØèüÿÿÿƒÈƒÈ |$T$¹   ³
  ˆD$f‰L$‰ğ¹   ÆD$ÇD$L   ‰|$èüÿÿÿ¸   ¹   f‰D$¸   T$f‰D$‰ğÇD$L   ‰|$èüÿÿÿ¡x  …À…$  ‹D$d+    uƒÄ[^_Ãt& º   ‰Øèüÿÿÿƒà÷é[ÿÿÿèüÿÿÿ´&    ´&    èüÿÿÿU‹-x  WV‰ÖS‰Ã…í…o  ‹“¸  ‰³l  Bê‰Ñƒø†›   ƒú!„’   ‹ƒ”  …À„Ä   ‹Hd‹y…ÿ„&  ‹?…ÿ„  kî‹   iÒ  ‹”0   …É„^  ‹)…í„T  j 1Éèüÿÿÿ_‹“¸  ƒú„‹   ƒú „‚   ƒúvlBßƒøw
¹Iè#£Árk[^_]Ã¶    kşiÂ  ƒ¼,   …Wÿÿÿº   ‰Øèüÿÿÿ‹“¸  ‹ƒ”  ‰Ñ…À…Cÿÿÿ´&    ƒú t}ƒútxƒúw”ƒúv¡¸ £Ğs—f‹ƒ”  …ÀtV‹Hd‹I…ÉtL‹y…ÿtEkî‹   iÒ  ‹”4   …Étc‹i…ít\j 1Éèüÿÿÿ‹‹¸  Zëfƒú t¸ƒú… ÿÿÿë­‰Ñköº   iÉ  ‹„4   ƒøtƒø…ÿÿÿº   ‰Ø[^_]é;ıÿÿv j 1Éèüÿÿÿ‹‹¸  Xë¸¶    j 1ÉèüÿÿÿYé§şÿÿèüÿÿÿS‰Ãƒì¡x  …À…¯  kÒiƒ¸    ƒ¼4   tƒÄ‰Øº   [éÑüÿÿƒÄ‰Øº   [éÀüÿÿèüÿÿÿV‰ÎSèüÿÿÿ‹˜(  k“l  iƒ¸    ‹„,   ƒèƒøv,‹ƒúw=¡x  …À…Ô  ‰“p  ‰ØƒÂèhüÿÿ1À[^Ãv ƒ>¸êÿÿÿº    [DÂ^Ãt& ¸êÿÿÿëÛèüÿÿÿUWV‰ÖS‰Ãƒì‹x  …É…ó  ‹>ƒÿ‡ı   kÇi“¸    ‹”,   …Ò„á   ÇF$   i“¸    ‹„,   º    ‹,…@  F‰$‰èèüÿÿÿ‰Áƒøÿ„  ƒø tƒÁƒù ‡  ‹$‰êèüÿÿÿÇF0÷r k×ÇF4    iƒ¸    ‹„,   º   ƒèƒøwÇF$   º   ‰V(9»l  t1ÀƒÄ[^_]Ãt& ‹CƒÃx´9ÃtåƒÆ8‹Gd‹@…Àt‹H$…Ét	‰ò‰øèüÿÿÿ‹GLx´9ÃuÜë¼¸êÿÿÿë·v èüÿÿÿVS‰Ëèüÿÿÿ‹°(  ¡x  …À…#  ‰Ú‰ğ[^é¤şÿÿt& èüÿÿÿS‰Ëèüÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…?  1À[Ãv èüÿÿÿVS‰Ëèüÿÿÿ‹x  ‹°(  …Ò…\  ƒûw9kÓi†¸    ‹„,   …Àt!‰Ú‰ğèhûÿÿ‰ğ‰ÚèOıÿÿ1À[^Ã´&    v ¸êÿÿÿëì´&    fèüÿÿÿë‰´&    fèüÿÿÿUWVS‰Ëƒì$d‹    ‰T$ 1Òèüÿÿÿ‹°(  ‹†¸  ƒø:wƒø#‡Î   Pìƒâû„â   ƒè8ƒø‡6  ‹…Ò…<  ‹Cº		˜ n‰†Ü  F\èüÿÿÿ‰Ç…À„A  èüÿÿÿ‰D$…À„  ‹Fp´9Å„8  f‹Fd‹@…Àt‹H…Ét	‰Ú‰ğèüÿÿÿ‹FLp´9ÅuÜ¸d   èüÿÿÿ‹D$Ç$    …À…ˆ  ‹G‹@èüÿÿÿ1Ò‰øèüÿÿÿ‹G‹@èüÿÿÿé]  v PÜ¹ q £Ñrƒø4„4ÿÿÿé#ÿÿÿt& ‹K‹†x  ÇD$   ‹–|  ÇD$   ‰Ü  ‰D$F\‰T$º		˜ ‰L$èüÿÿÿ‰D$Ç$   …Àtèüÿÿÿ‰$…À„k  ‹=x  …ÿ…«  †,  º   èüÿÿÿ…À„í   ‹@‰D$‹†¸  Pìƒâû„ä   ƒè$ƒøwº q £Â‚Î   ‹D$…À„Õ  ‹ˆÔ  …É„Õ  ‹FnX´9Åt?´&    ‹Cd‹@…Àt‹x…ÿt‹–x  ‹|  ‰Øèüÿÿÿ‹CLX´9ÅuÒ‹D$‹ˆÔ  ‹D$T$èüÿÿÿ¸d   èüÿÿÿ‹$…É„ş   Ç$    ‹D$ d+    …  ‹$ƒÄ$[^_]Ãv ƒ¾€  …½ıÿÿv Ç$êÿÿÿëÈ´&    †ø  ‰D$é#ÿÿÿ´&    f‹FÇD$   p´9Å…Êıÿÿ¸d   èüÿÿÿÇ$    ë€´&    ‹|$‹G‹@èüÿÿÿº   ‰øèüÿÿÿ‹G‹@èüÿÿÿéjşÿÿt& ‹G‹@èüÿÿÿº   ‰øèüÿÿÿ‹G‹@èüÿÿÿ‹Fp´9Å…Pıÿÿ¸d   èüÿÿÿé‚ıÿÿ‹|$‹G‹@èüÿÿÿ1Ò‰øèüÿÿÿ‹G‹@èüÿÿÿéáşÿÿèüÿÿÿv èüÿÿÿéfüÿÿ¶    èüÿÿÿWV‰ÆS‹€Ü      ‹ …Â„¡   ‹†Ü   ‰Ó‰$  ÷Â  t+÷Â   …  öÇ…†   öÇtƒ=x  ‡ë  t& 1ÿöÃu‰Ú‰ğèüÿÿÿ[^ø_Ã´&    v ¾¸  ‰øèüÿÿÿ‹†Ü   ‹ˆ   ‹–ø  †ø  9Ğt‰Êèàïÿÿ‰ø¿   èüÿÿÿë©f[1À^_Ã´&    v ƒ=x  ‡L  1ÿöÃtƒëš´&    v èüÿÿÿ‹x  S‰Ã…Ò…h  ‰Øº   èüÿÿÿ‹ƒä  …Àt‹ğ  ƒâuYèüÿÿÿÇƒä      ‹ƒà  …Àt‹ğ  ƒâu%èüÿÿÿÇƒà      ‹ƒ0  …Àt,‰Ø[éüÿÿÿt& èüÿÿÿëÙ´&    fèüÿÿÿë¥´&    f[Ã´&    ´&    èüÿÿÿUWVS‰Ãƒì@‹x  d¡    ‰D$<1À…É…@  ¿@  ¾ F  ¹Í   º  ó¥¸   ‰Í¹   èüÿÿÿ¸ÔI  º   =ÔI  YUYVÇƒx     EÅÇƒ|      ‰ƒè  ƒø  ‰ƒø  ‰ƒü  ƒX  ‰ƒX  ‰ƒ\  ‰ØÇƒô     Çƒì  Ğ  Çƒğ  à  èüÿÿÿƒ»€  t‹ƒˆ  ƒø†ÿ  ³  »  ‰ğèüÿÿÿ‹‹|  ‹“x  ‰Øèüÿÿÿ1Ò‰Øèàôÿÿ1Ò‰ØèÇöÿÿ‰ğèüÿÿÿ‹ƒÌ   Çƒ     Çƒ     ˆ   ‰›,  ‰ƒ  ‰øÇƒ<     Çƒ@     Çƒ4     Çƒ      Çƒ$      Çƒ8      ‰³  èüÿÿÿ‰Â…Àˆ  ‹ƒÌ   ‰›Œ  «d  Çƒd     ˆ   ‰³x  ‰ƒl  ‰èÇƒh     Çƒœ     Çƒ      Çƒ”     Çƒ€      Çƒ„      Çƒ˜      èüÿÿÿ‰Â…Àˆ–   ¹K  º F  ‰Øè=îÿÿ‰ƒà  ‰¸¸  ‹ƒà  Ç€ä    ‹ƒ¸  ƒø4tƒè8ƒø‡  ‹ƒà  ˆä      ‹“Œ  ‹ƒà  ³¼  ‹• J  ‹à   ÿ21Òjèüÿÿÿ‰ÂXY…Ò‰„  é\  ´&    ‰Ø‰$èüÿÿÿ‹$‹D$<d+    …†  ƒÄ@‰Ğ[^_]Ã¶“„  s„Ò…^  ¸   èüÿÿÿ¹(  i“ˆ  ˆ  P‰ğj ”ü   èüÿÿÿ_]‰Æ…ö„¹ıÿÿ‹ƒ€  ÇD$    ÇD$    ‰D$‰ğÇD$    ÇD$   èüÿÿÿf‰D$‹FdÇD$    ‹@…Àt*‹H$…Ét#¡   T$…À„   ‹x$…ÿ„•  ‰ğèüÿÿÿ‹ƒ¸  ƒø„ï   ƒø*„æ   ƒø'…&ıÿÿ‹ƒ€  ÇD$(    ÇD$,    ‰D$D$ ÇD$0    ÇD$4    ÇD$8    ÇD$ <  ÇD$$@   ‰D$‹Fd‹@…À„Îüÿÿ‹H(…É„Ãüÿÿ¡   T$…À„!  ‹x(…ÿ„  ‰ğèüÿÿÿé›üÿÿ´&    v iÀˆ  j ¹(  R„ü   ‰Â‰ğèüÿÿÿ‰ÆXZé©şÿÿ´&    fƒ»€  …Ùıÿÿéäıÿÿ¶    ‹“€  ÇD$(    ÇD$,    ‰T$T$ ÇD$0    ÇD$4    ÇD$8    ÇD$ .  ÇD$$@   ‰T$‹Vd‹R…Ò„Âşÿÿ‹J(…É„·şÿÿ¡   T$…Àt0‹x(…ÿt)‰ğèüÿÿÿ‹ƒ¸  é‘şÿÿ´&    ‰ğèüÿÿÿéfşÿÿt& ‰ğèüÿÿÿ‹ƒ¸  éhşÿÿ¶    ‰ğèüÿÿÿé…ûÿÿèüÿÿÿfffffffèüÿÿÿ‹@(‹T$‹€x  Ç   % ù  ƒøÀ%€C   ‡  ‰1ÀÃ´&    ´&    èüÿÿÿUWVS‰Ã«x  ƒì‹ ‹“ˆ  ‹p(‹ƒ  ƒÀX  ¾¸  ‰B‹ƒŒ  Ç   p‹ƒ  ‹“Œ  ƒÀ‰B‹ƒŒ  Ç@    ‹†X  9Átt‹ƒˆ  ‰L$   ‹†\  ‰$‰øèüÿÿÿ‹L$‰Â‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰øèüÿÿÿ‹$‹“  ƒ=|  ‹@‰P‡ı  ƒÄ[^_]Ã´&    f‰ø‰$èüÿÿÿ‹$‰Â‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰øèüÿÿÿƒ=|  ‡â  ƒÄ[^_]Ã´&    t& èüÿÿÿUWVS‹h(‹…Ü   @  ‹ƒâİ‰½¸  X  ‰øèüÿÿÿ‰Æ‹…X  9Ãt@v ‹…X  ‹P‹-x  ‰Q‰
º   Ç€x     Ç€|  "  èüÿÿÿ‹…X  9ØuÃ[‰ò‰ø^_]éüÿÿÿ¶    èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿW1ÒVS‰Ã‹ ‹p(‰Øèüÿÿÿ‹S‰Á‹†x  % ù  ƒøÀƒàƒÀ…Òu	[¸êÿÿÿ^_Ãiø@  ‹ST9úrêG×‰SP“„  ‹Ì   PiÀ   j h   P‰Øj ‹	èüÿÿÿ1ÀƒÄ[^_Ã´&    ´&    èüÿÿÿV‹|  S‹X(‹³X  …Ò…  ‹ƒd  ÿv¹   P,‰Øèüÿÿÿ‹ƒÜ   º   ‰ˆ  ‹ƒÜ   ‰4  Çƒ`      ‰Øº   èüÿÿÿ‹ƒÜ      ‹ƒÊ"‰‹ƒÜ      ‹ƒÊ ‰‹ƒÜ   @  ‹€@  ƒÈ"‰X1À[^Ã¶    èüÿÿÿS‰Ëèüÿÿÿ‹€(  ÇCÀü›ÇC   ÇCGREYÇC    ÇC$    ‹€x  öÄùt'ÇC
   ÇC  ÇC   ÇC    1À[Ã´&    ©ÿÿ tîÇC   1ÀÇC?  ÇC   ÇC    [ÃèüÿÿÿƒâtFVS‰Ã¡|  …À…4  ³¸  ‰ğèüÿÿÿ‹ƒÜ   ‹ˆ$  “X  ‰Øèüÿÿÿ‰ğèüÿÿÿ¸   [^Ã1ÀÃffffffèüÿÿÿV°è   S‰Ã‰ğèüÿÿÿ‰Â‹ƒÜ   ‹˜  ‰ğèüÿÿÿ‰Ø[^ÃèüÿÿÿUWV‰ÎS‰Óƒì€|$4 ‹T$ tÇ    pƒÀÇ@ø    Ç@ü    ‰ÇƒúÿtÊ € €ƒÇ‰‹T$,…Ò„­   ‹D$0Ç$    …À•D$¶    ‹K‹l$$…öt-‰Øë´&    f)Îèüÿÿÿ…ö„  ‹H9ñvê‹l$$‰Ã)ñõ‹$…Àti€|$ tb1Ò÷t$0ƒúWÀ%     ;L$$rRD$$   ‰s‰wÇG    ‰×‹t$(ƒ$‹$î9D$,…kÿÿÿƒÄ‰ø[^_]Ã´&    f¸   W;L$$s®	È‰T$   ‰s‰Ø‰wÇG    +kèüÿÿÿ‹T$‰Ã‹@9Åv5‰Á‰Ø‰Óv É   ƒÃ‰Kô‹P‰SøÇCü    +hèüÿÿÿ‹H9érØ‰Ú‰Ã‰èz   ‰‹CÇB    ‰BéDÿÿÿ¶    ‹H‹l$$‰Ãéêşÿÿ´&    v èüÿÿÿWVS‹ˆÌ   ‰Ã‹‘\  ‹X  ‹±P  ‹¹T  ‰Ñ	Át
ƒÀƒÒ )ğúR‰ù‰òP¸    èüÿÿÿğÿ”ÀZY„Àt|‹ƒ¸  iĞ  ƒº   tiĞ  ‹’   ƒú„‚   ƒú„Ÿ   iÀ  ‹€   ƒø„¤   ƒøt7ƒ
  èüÿÿÿƒ|  èüÿÿÿƒô   èüÿÿÿ‹ƒÜ   [^_éüÿÿÿ[^_Ãt& ‰ØèüÿÿÿëÀ´&    ‰Øèüÿÿÿ‹ƒ¸  éoÿÿÿ¶    ƒÜ  èüÿÿÿ‹ƒ¸  iĞ  ‹’   ƒú…aÿÿÿ‰Øèüÿÿÿ‹ƒ¸  éOÿÿÿ´&    ƒ   èüÿÿÿiƒ¸    ‹€   é<ÿÿÿèüÿÿÿ…Àt7S‹X`ú v@túv@t[Ã9ƒ¤  uõ‹[éüÿÿÿ9ƒ¤  uå‹[éüÿÿÿÃ´&    ´&    èüÿÿÿö€4  t*S‹˜Ü   ‹‹  ‹˜Ü   ‹“”  …Ét…Òu
[Ã´&    Ã‹˜Ü   ‰‹  ‹˜Ü   ‰“”  ‹˜Ü   Ã  ‹‹˜Ü   Ã ‹‹˜Ü   Ã  ‹‹˜Ü   Ã ‹RQhè
  ‹€Ì   ˆ   PèüÿÿÿƒÄ[Ã´&    v èüÿÿÿUW¿   V‰ÆSƒì‰$ëM´&    ‹„  …Ò…Ô  ‹‹Cƒï‰B‰º   ‰èÇ   ÇC"  èüÿÿÿ·V‹$)Ğ…À~O…ÿ~K‹9ğtE‹èüÿÿÿ‰“ ıÿÿ«ˆıÿÿ‰ƒœıÿÿ‹FP‰V‰ƒôşÿÿ‹F·Ğ;$u‚ƒ=„  v‡é÷  t& ƒÄ[^_]Ã´&    èüÿÿÿWV1öS‰Ã‹€Ü   ‰°   ‹ƒÜ   ‰°   ‹ƒÜ   ‰°@  ‹ƒÜ   ‰°@ ‹ƒÜ   ‰°@ ‹ƒÜ   ‰°@  ‹ƒÜ   ‰°@ ‹ƒÜ   ‰°   »è   ‰øèüÿÿÿ‰Â‹ƒÜ   ˆ  ‹‰1‰øèüÿÿÿ‹ƒÜ   ‰°   ‹ƒÜ   ‰°0  ‹ƒÜ   ‰°@  ‹ƒÜ   ‰°P  ‹ƒÜ   ‰°`  [^_Ã´&    ´&    èüÿÿÿWV‰ÆS‹¸È   _ü‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèøşÿÿ‹†L  ‰Úèüÿÿÿ‰ğèüÿÿÿ‰Øè½ûÿÿGXèüÿÿÿ‰øèüÿÿÿ‰Ø[^_éüÿÿÿt& èüÿÿÿW¸è   V‰ÖS‰Ã‰øèüÿÿÿ	³ä   ‰Â‹ƒÜ   ˆ  ‹€  	ğ‰[‰ø^_éüÿÿÿ´&    ´&    èüÿÿÿW¸è   V‰Æ‰øS‰Óèüÿÿÿ‰Â#ä   t‹†Ü   ˆ  ‹€  	Ø‰[‰ø^_éüÿÿÿ´&    ¶    èüÿÿÿW¸è   V‰Æ‰øS‰Óèüÿÿÿ‰Â‹†Ü   ˆ  ‹€  ÷Ó!Ã‰[‰ø^_éüÿÿÿv èüÿÿÿW¸è   V‰Æ‰øS‰Óèüÿÿÿ‹Ü   ‰Â‰Ø÷Ğ™  !†ä   ‹‰  !È‰[‰ø^_éüÿÿÿ´&    t& èüÿÿÿUW‰×VS‰Ãƒì‹r¡„  …ö„  …À…8	  ‹GƒÁ1Òƒáø‰D$‹G‰L$‰$÷ñº   9ĞFĞ‰T$ƒø†>  ‹ƒÜ   º  p‰Pº   ‹ƒÜ   ‰P1Ò‹ƒÜ   ‰P1íÇD$    ‹t$‹L$9$ro‰|$1Éfƒ=„  ‡]	  ‰ğƒàüƒÜ   ‰$‹D$‹x‹$ï‰8FƒàüƒÜ   ‰FƒàüƒÜ   ‰FƒàüƒÜ   ‰ƒÁƒÆl$9L$w‹|$‹G‹W(ƒàüƒÜ   …Ò…  ‹T$(‰‹G1ÒƒÀƒàüƒÜ   ‰‹G‹L$ƒÀƒàüƒÜ   ‰‹G‹t$ƒÀÁæƒàüÁîƒÜ   ‰0‹G‹WƒÀƒàüƒÜ   ‰‹G(…À‹G„÷   ƒÀº  €ƒàüƒÜ   ‰º   1Ét& ‹GĞƒàüƒÜ   ‰ƒÂƒúPuè‹G‹WƒàüƒÜ   ‰‹G‹L$ƒàüƒÜ   ‰‹G$ƒàüƒÜ   ‰0‹T$‹G ÁêƒàüƒÜ   ƒê‰ƒ=„  ‡	  ƒÄ1À[^_]Ãº   ‰éøşÿÿt& …À…	  ‹G1ÒƒàüƒÜ   ‰‹GƒàüƒÜ   ‰‹G$ƒàüƒÜ   ‰‹G ƒàüƒÜ   ‰ƒÄ1À[^_]Ãt& ƒÀº   ƒàüƒÜ   ‰éÿÿÿ¶    èüÿÿÿUWV‰ÆS‰Óâ  ‹8…€  öÃt`ƒ=„  ‡k  ®´   ‰èèüÿÿÿ‹†¸   ƒàü‡Ü   ‹†˜   èÚùÿÿ‰èèüÿÿÿ‹†è   ƒàü‡Ü   ‰¸   [^_]Ã´&    …ÛuÙ[1À^_]Ãt& èüÿÿÿUhW‰×V‰Æ‰èSèüÿÿÿ‹FV9Ât/X´ë´&    v ‹CLX´9Ât9»   uî‰èèüÿÿÿ‰Ø[^_]Ã‰è1Ûèüÿÿÿ‰Ø[^_]ÃèüÿÿÿUWV‰Î1ÉS‹l$‰Ó‹|$$ƒıÿ•Á1Òƒ|$ÿ•Âˆ   Ñ‹T$T$ j ¯×hÀ  ÁêT¯ÑKTRÁâ‰èüÿÿÿZY‰C…Àtnƒıÿtj‰é‰òj Wÿt$,ÿt$,j èŸôÿÿƒÄƒ|$ÿt'1Òƒıÿ”ÂR‰òj Wÿt$,ÿt$,h   ‹L$0èqôÿÿƒÄ‰C+C1ÒƒÀ;w[‰Ğ^_]Ã´&    ºôÿÿÿëè´&    èüÿÿÿUˆ   WV‰ÎS‰Ó‹|$‹T$‹l$j KhÀ  ¯×ÁêTRÁâ‰èüÿÿÿZY‰C…Àt:1Ò…í”Â1ÉR‰òUWj ÿt$$jÿèİóÿÿ1Ò‰C+CƒÄƒÀ;w[‰Ğ^_]Ãt& ºôÿÿÿëì´&    èüÿÿÿUWV‰Î1ÉS‹l$‰Ó‹|$$ƒıÿ•Á1Òƒ|$ÿ•Âˆ   Ñ‹T$T$ j ¯×hÀ  ÁêT¯ÑKTRÁâ‰èüÿÿÿZY‰C…Àtnƒıÿtj‰é‰òj Wÿt$,ÿt$,j è/óÿÿƒÄƒ|$ÿt'1Òƒıÿ”ÂR‰òj Wÿt$,ÿt$,h   ‹L$0èóÿÿƒÄ‰C+C1ÒƒÀ;w[‰Ğ^_]Ã´&    ºôÿÿÿëè´&    èüÿÿÿS‹š„  ‹Šˆ  ‹€Ì   j ÿ²”  ÿ²  ˆ   ‰ÚèüÿÿÿƒÄ[Ã´&    fèüÿÿÿU‰ÕW‰ÏV‰ÆSƒì‹„  ‹…É…q  ‰Øè×õÿÿ‹†À   ƒàüƒÜ   ‹‹–ô   ÷Ò!Ê‰kV,‹¤   ‰Ø“d  ÿ·  èüÿÿÿƒ=„  Z‡²  ‹†Ä   ‹–¤   ƒàüƒÜ   ‰iƒ¸        ‹PöÂu‹@¨„Ÿ  ƒú„  ¸¼ èüÿÿÿ‹†ì   …Àtƒàü‹–  ƒÜ   ‰‹†È   ‹–  ƒàüƒÜ   ‰‹†à   ‹–   ƒàüƒÜ   ‰‹†Ü   ‹–  ƒàüƒÜ   ‰‹†Ì   ‹–ü   ƒàüƒÜ   ‰¸¼ èüÿÿÿ‹†¼   º   ƒàüƒÜ   ‰ÇE    ‹ƒ¸  iĞ  ƒº   u"‹ƒÜ   L  ‹ ƒàü‹»Ü   ‰‡L  ‹ƒ¸  iĞ  ƒº   u"‹ƒÜ   L  ‹ ƒàû‹»Ü   ‰‡L  ‹ƒ¸  iÀ  ƒ¸   uE‹ƒÜ   L  ‹ ƒàş‹»Ü   ƒÈ
‰‡L  ‹ƒÜ   H  ‹Ê  €‰ºE ‹ƒÜ   ‰,  ‹ƒh  =u  t-w  ƒø‡,  ¡„  …À…–  ‰Øèºóÿÿ‹†ä   ƒàüƒÜ   ‹–ø   ‰‹†À   ƒàüƒÜ   ‹–ô   ‰»è   ‰Øè{óÿÿ‰ø‹®ğ   èüÿÿÿ	«ä   ‰Â‰øèüÿÿÿ‰øèüÿÿÿ‰Â‹ƒä   …Àut‰øèüÿÿÿ‰Øè>óÿÿ‹ƒÜ      ‹ƒÊ ‰‰Øè%óÿÿiƒ¸    ƒ¸   tbƒ=„  wi‰øèüÿÿÿ‰Â‹ƒÜ     ‹ ‰øèüÿÿÿ‰Øèåòÿÿ1ÀƒÄ[^_]Ãv ‹‹Ü   Á  ‹)	è‰éuÿÿÿ1Ò‰Øèüÿÿÿéeıÿÿfº   ‰ØèüÿÿÿëéÊ  ´&    t& èüÿÿÿUWV‰ÖS‰Ãƒì‹º¤   ‹*¯º¨   1Òèüÿÿÿ‹„  ‰Â…É…!  ‹C…ÀuC¸êÿÿÿ…ÿu(‹…Ì   j Ã„  ÿ¶¨   ÿ¶¤   ‹
‰Úèüÿÿÿ1ÀƒÄƒÄ[^_]Ã´&    v ;{Tw‰{Pë¼¸êÿÿÿëİ´&    ´&    èüÿÿÿUWV‰ÆS‰Óƒì‹8‹ƒ  ‹’ˆ  ƒÀÇ¸  ‰B‹ƒŒ  Ç   p‹ƒ  ‹“Œ  ƒÀ‰B‹ƒŒ  Ç@    ‰øèüÿÿÿ‹®˜   ˜   ‰Âƒx  9étT‹«ˆ  M    ‹®œ   ‰†œ   ‰‹x  ‰«|  ‰E ‹E‹‹  ‰H¡„  …À…l  ƒÄ‰ø[^_]éüÿÿÿt& ‹®œ   ‰†œ   ‰‹x  ‰«|  ‰E ‹„  …É…E  ƒÄ‰ø[^_]éüÿÿÿèüÿÿÿUW‰ÇVSƒì‹„  …Ò…“  ‹0‹‡ä   ƒàü†Ü   ‹‹—ø   ÷Ò!Ê‰‹‡À   ƒàü†Ü   ‹‹—ô   ÷Ò!Ê‰»   v ¸X‰A èüÿÿÿƒëuñ»d   ë´&    v …Àt,¸X‰A èüÿÿÿƒët‹†Ü   ‹  ‹†Ü   ”  ‹ …ÒuĞi†¸    ƒ¸   t}‡´   Ÿ˜   ‰$èüÿÿÿ‰D$ë?v ‹·˜   ‹‹F®ˆıÿÿ‰B‰‰èº   Ç   ÇF"  èüÿÿÿ¡„  …À…Õ  ‹‡˜   9Øuº‹T$‹$ƒÄ[^_]éüÿÿÿ´&    ‹†Ü   L  ‹ ƒàõ‹Ü   ƒÈ‰L  ‹‡ì   1Òƒàü†Ü   ‰‹‡Ì   º   ƒàü†Ü   ‰i†¸    ƒ¸   …$ÿÿÿ1Ò‰ğèüÿÿÿéÿÿÿv èüÿÿÿ…Ò„ƒ   UWV‰ÖS‰Ã‹€”  ƒàüƒÜ   ‹8ƒ=„  wd÷Æ 3 …û  ÷Æ   t.ƒ=„  ‡G  «  ‰èèüÿÿÿƒt  ‰úè†ïÿÿ‰èèüÿÿÿ‹ƒÄ  ƒàüƒÜ   ‰0¸   [^_]Ãt& 1ÀÃ‹ƒÀ  ƒàüƒÜ   ‹ WPRhÔ  hD  èüÿÿÿƒÄéqÿÿÿfèüÿÿÿUWVS‰Óƒì<d¡    ‰D$8‹‚Ü   ‹¨  ºè   ‰øèüÿÿÿ‰Â‹ƒÜ   ‹°  ‰øèüÿÿÿ‰è!ğ‰$„%  ‹ƒÜ   $  ‹ ‰D$‹ƒÜ      ‹ ‰D$‹ƒÜ   T  ‹ ‰D$‹ƒÜ   P  ‹ ‰D$‹ƒÜ   ‹ˆ4  ‹ƒÜ   0  ‹ ‰D$‹ƒÜ   D  ‹ ‰D$‹ƒÜ   @  ‹ ‰D$‹ƒÜ   ‹   ‹ƒÜ      ‹ ‰D$ ‹ƒ”  ƒàüƒÜ   ‹ ‰D$$‹ƒØ  ƒàüƒÜ   ‹ ƒ=„  ‰D$(‡x  ‰è%   ‰D$÷Å€„ò   ÷Å   tƒ=„  ‡K  ÷Å   tƒ=„  ‡o  ÷Å   tƒ=„  ‡  ÷Å   tƒ=„  ‡'  ÷Å   tƒ=„  ‡ß  ÷Å   …Ú  ÷Å   tƒ=„  ‡>  ÷Å   „Ò  ƒ=„  ‡  ÷Å   „  ƒ=„  ‡ü  ÷Å  € u÷Å   tƒ=„  ‡Ø  ÷Å   „ú  ƒ=„  ‡´  iƒ¸    1ö‹€  ƒø„2  ƒø„a  …Ét"iƒ¸    ‹€   ƒø„Ì  ƒø„S  ‹D$…Àt"iƒ¸    ‹€   ƒø„º  ƒø„9  ‹L$…É…-  ‹T$…Ò…‘  ‹D$…Àt[‹ƒ¨  ÆD$7 …ÀtL‹Pd‹…ÒtC‹J$‰L$…Ét8‹    …Ò„x  ‹J$‰L$…É„i  T$7‰Ñ‰ê‹l$èüÿÿÿ€|$7 tƒÆ÷$   …Ñ   …öu-Ç$    ‹D$8d+    …9  ‹$ƒÄ<[^_]Ã´&    v ‹ƒÜ   ‹<$‰¸  Ç$   ëÂ´&    ÷Å  €„Ëşÿÿ‰ê‰Ø‰L$èüÿÿÿ‹L$‰Æiƒ¸    ‹€  éšşÿÿt& ÷Å  € „“şÿÿ‰Ø‰L$èüÿÿÿ‹L$Æé}şÿÿ¶    ‹T$‰Øèüÿÿÿ‹T$Æ…Ò„ÆşÿÿëUt& ‰øèüÿÿÿ‰Â‹ƒÜ     ‹áÿÿÿ÷‰‰øƒÆèüÿÿÿ‹˜  ¸    ‹    èüÿÿÿ…ö„íşÿÿéÿÿÿt& ‹L$‹T$‰ØèüÿÿÿÆ‹D$…À„µşÿÿéUşÿÿ¶    ƒ=„  †µşÿÿé_  ¶    ƒ=„  ‡“  ÷Å   ….ıÿÿ÷Å   „/ıÿÿƒ=„  †"ıÿÿéš  ´&    f‰Ê‰ØèüÿÿÿÆéıÿÿ‹T$‰ØèüÿÿÿÆéµıÿÿ¶    ÷Å  € „ıÿÿƒ=„  †	ıÿÿé»  ´&    v ‹D$…À„ıÿÿƒ=„  †úüÿÿév  fƒÜ  ‰Êè³ïÿÿÆé)ıÿÿt& ‹T$ƒ   è™ïÿÿÆé9ıÿÿf‰êL$7‹l$èüÿÿÿé”ıÿÿèüÿÿÿ´&    èüÿÿÿV‰Æ‰ĞS‰Óƒàt‹Ü   ‘  ‹
	È‰÷Ãøÿ t>i†¸    ƒ¸   „_  ƒ¸   „_  ‹†Ü      ‹
‰ØÁè·À	È‰ã  ø …w  [^Ã´&    èüÿÿÿV‰Æ‰ĞS‰Óƒàt‹Ü   ‘  ‹
÷Ğ!È‰÷Ãøÿ t@i†¸    ƒ¸   „  ƒ¸   „  ‹†Ü      ‹
‰ØÁè·À÷Ğ!È‰ã  ø …§  [^Ãv èüÿÿÿV‰ÆS‰Óƒâuh÷Ãøÿ u‰Ø%  ø …¿  [^Ã´&    i€¸    ƒ¸   „Ù  ƒ¸   „Ù  ‹†Ü   ‹   ‰Ø[^Áè!ĞÁà%øÿ Ã¶    ‹€Ü   ‹€  Áè!Ø[^ƒàÃèüÿÿÿW‰ÏV‰ÆS‰Óƒât ‹€Ü     ‰ØÁà%   …Éts‹
	È‰÷Ãøÿ tFi†¸    ƒ¸   „ñ  ƒ¸   „ñ  ‹–Ü   Áë·Û‚$  …ÿt‹’$  ÷Ó!Ó‰[^_Ãv ‹’$  	Ó‰[^_Ã´&    v ‹
÷Ğ!È‰ë‰¶    èüÿÿÿU¹8  ºÀ  W‰ÇVSƒì¡4   èüÿÿÿ…À„š  ‰Ã¡`N  …À„Ç   º   ƒøtM1É¸"  ºQ  1öèüÿÿÿ…Àu%ƒÆƒş„š   ‹õ	  ‹õ 	  1Éèüÿÿÿ…ÀtÛèüÿÿÿº   ´&    ¶ƒ4  kƒàş	Ğ‰êˆƒ4  ‡ˆ   ‰D$èüÿÿÿ‰Æ…Àx/j C\1Éº   ‰D$èüÿÿÿ‹³˜   X…öt,‹$èüÿÿÿ‰èèüÿÿÿ‰ØèüÿÿÿƒÄ‰ğ[^_]Ãt& 1ÒëŒt& ‹$‰»Ì   ¾ûÿÿÿÇƒ˜  àÿÿÿ‰C@ƒœ  ‰ƒœ  ‰ƒ   ƒ°  ‰ƒ°  ‰ƒ´  ƒÄ  ‰ƒÄ  ‰ƒÈ  ‰øÇƒ¤      Çƒ¬  àÿÿÿÇƒ¸      ÇƒÀ  àÿÿÿÇƒÌ      ÇC< E  èüÿÿÿ‰D$…À…7ÿÿÿƒ  ¹€  º°  Çƒè       Çƒ¸      èüÿÿÿƒ¤  ¹€  º»  èüÿÿÿğÿ¡€  ‹¼  ‰L$‰ƒŒ  PPhË  Q‰€  èüÿÿÿ‹ƒÌ   ƒÄ·@"f=€ˆ„§  f=Rˆt¸u  ¹ ?«º L  ‰‹ğ   ‹„  ‰ƒh  ‰“d  …É…'  ‹ƒŒ  ‹…@N  ;    ‚(  Çƒ¸  ÿÿÿÿ‹    ‹t$ëf‹“¸  ƒÆƒúÿuR9Ît1‹ƒÌ   ·õ    f9P$uÚ·õ   f9P&uÌ‹õ   ‰“¸  ëÃÇƒ¸      ‰Øèüÿÿÿ‹ƒÌ   ‹“¸  fx"Rˆ„Ù  iÊ  ‹‰(   …Ét‰‹ğ   ƒú+„Ÿ  ‹P³è   ¶Rt‰“Ô   ‹@Áèƒà‰ƒØ   ‰ğèüÿÿÿ‹ä      ‰Â‰ğèüÿÿÿ‹ƒ¸  Çƒø       ‰›ô   iĞ  ‰›|  Çƒl    Çƒh    Çƒp     Çƒt    Çƒx    Çƒd     Çƒ€     Çƒô	    Çƒğ	    Çƒø	     Çƒü	    Çƒ 
    Çƒì	     Çƒ
     ‰›
  Çƒ|    Çƒx    Çƒ€     Çƒ„    Çƒˆ    Çƒt     ‹’   ƒâıƒú„ü  iÀ  ‹€   ƒàıƒø„™  ‹‹Ì   ‹‘\  ‹X  ‰T$‹‘T  ‰D$‹P  ‰T$‹T$‰D$‹D$‰Ñ	ÁtƒÀƒÒ +D$T$‰D$‰T$j ¸    ÿt$ÿt$ÿt$‹T$$‹L$(èüÿÿÿƒÄ…À„  ‹³Ì   ‹T  ‹†P  ‹–\  ‰L$‹X  ƒÁ)Á–X  ¾    ‹T$DÎèüÿÿÿ‹“Œ  ¹•  ‰ƒÜ   ‰ƒà   ‹ƒ¸  9•@N  º£  EÊ‹“Ì   QPiÀ  ÿ°    ·B&P·B$Pÿt$$hP  èüÿÿÿ¡„  ƒÄ…À„[  éD  ´&    ‰“¸  ƒúÿ„Óüÿÿ‹ƒÌ   é6ıÿÿfx&7q…UıÿÿÇƒğ   @x}éFıÿÿƒú9„É  ƒú8…ıÿÿÇƒ¸  <   ‹Ø@  …Ò„ıÿÿ‰“ğ   éıÿÿ¡„  …À…	  ƒ¸  ‰›   ¹€  ºõ  ‰ƒ¸  ‰ƒ¼  ƒ4  Çƒ     Çƒ    Çƒ(      Çƒ,  ÀG ÇƒÔ      Çƒ$     èüÿÿÿƒ,  Çƒ°      ‰ƒ,  ‰ƒ0  ‹ƒ0  …Àu
Çƒ0     ÇƒØ    ÇƒÜ  0 Çƒà  @ Çƒä  P Çƒè  T Çƒì  X Çƒğ  \ Çƒô  ` Çƒø  d Çƒü  h Çƒ   l Çƒ      Çƒ  @  Çƒ  D  Çƒ(     Çƒ     éıÿÿ‹„  …Ò…Q  ƒt  ‰›Ü  ¹€  ºõ  ‰ƒt  ‰ƒx  ƒğ  ÇƒĞ     ÇƒÔ    Çƒä      Çƒè  ÀG Çƒ      Çƒà     èüÿÿÿƒè  Çƒl      ‰ƒè  ‰ƒì  ‹ƒì  …Àu
Çƒì     Çƒ”    ‹ƒ¸  Çƒ˜  4 Çƒœ  @ Çƒ   P Çƒ¤  T Çƒ¨  X Çƒ¬  \ Çƒ°  ` Çƒ´  d Çƒ¸  h Çƒ¼  l ÇƒÈ  D ÇƒÀ  0  ÇƒÄ  4  Çƒä     ÇƒÌ     é²ûÿÿÇƒ¸  =   ‹ìA  …Ò„\úÿÿé;ıÿÿ¾ôÿÿÿé$øÿÿ¸x  ¹€ğúº K  é]ùÿÿfffffffèüÿÿÿ¸	 ÿÃt& èüÿÿÿV‰ÆS»@   ë¸à èüÿÿÿƒët)‹F‹‹€x  ƒàü‚Ü   ‹ ¨u×¸   [^Ã´&    1À[^Ãv èüÿÿÿU‰ÕWVSƒì‹x‰D$¡Œ  ‹…Àt	öÁ„"  ·Ef…À„Â   ‹Œ  …Û…ê!  ‰ë1ö‰Õ‰Úë1f‹‡€  ƒàü…Ü   ‹‹Bˆ0¡Œ  …À…o"  ·BƒÆ9ğvm‹Ÿp  ƒè‰$‰ÙË  É  9ğ‹‡|  FÙ·
ƒàü…Ü   Áá‰‹‡t  ƒàü…Ü   ‰‹D$èáşÿÿ‹$…À…vÿÿÿ¡Œ  …À…   ¸ûÿÿÿƒÄ[^_]Ãt& ‹‡|  ·M ƒàüÁá‚Ü   ‰‹‡t  ƒàü‚Ü   ‹—p  ƒÊ‰‹D$è}şÿÿ…Àt°‹D$‹@‹‹€x  ƒàü‚Ü   ‹ ¨t‹5Œ  …ö…Ì!  ƒÄ1À[^_]Ã¸úÿÿÿéuÿÿÿé¿"  t& èüÿÿÿU‰ÕW‰ÏVSƒì‹p‰D$¡Œ  ‰L$‹·M…ÿ„b  …À…#  ‹|  ·E ‹ºÜ   ƒãüÁàßf…É„‰  ‹]¶	Ø‰D$‹†p  fƒù„-    ‰$‹D$‰‹†„  ƒàü‚Ü   ‰‹†t  ‹$‰T$ƒàü‚Ü   ‰‹D$è}ıÿÿ…À„  ‹Œ  …Ò‹T$…a#  ·Eƒøù   ‰é¿   ‰Õ‰Êëv´&    fË  ‹†|  ‰$‹T$ƒàü…Ü   ‰‹†„  ƒàü…Ü   ‰‹†t  ƒàü…Ü   ‰‹D$è÷üÿÿ…À„   ‹Œ  ‹$…É…¤#  ·BƒÇ9ø~v‹Jƒè‹p  ¶99ø{ÿÿÿ‰ØË    ƒ|$ DØégÿÿÿ…À„şÿÿé9#  v ‰Á  É  ƒ|$ EÈ‰$é¾şÿÿv ‹Œ  …É…õ"  ¸ûÿÿÿƒÄ[^_]Ãt& ‰‹†t  ƒàü‚Ü   ‹–p  ƒÊ‰‹D$è+üÿÿ…ÀtÅ‹D$‹@‹‹€x  ƒàü‚Ü   ‹ ¨t¡Œ  …À…×"  ƒÄ1À[^_]Ã¸úÿÿÿë´&    t& èüÿÿÿU‰ÕW‰ÇV‰ÎSƒì‹Œ  …Ò…Ú#  1Û…ö<é•   t& C‰$9ğ}D‰Áö@t· f9tJ1É‰øèYıÿÿ…Àx1ƒÃ9Ş~^[¡Œ  ÁáT …À…÷#  öBt±1É‰øè¨ûÿÿ…ÀyÏƒÄ[^_]Ãt& ‰Ë·I‰øèıÿÿ…Àxã‰Ú¹   ‰øèyûÿÿ‹$ë›t& ƒÄ‰ğ[^_]Ã¶    èüÿÿÿUWVS‰ÃƒìH‹3d¡    ‰D$D1À¡Œ  …À…ï$  ‰òk1À¹œ   ‰ïó«»x  ¹}   ÇC    ÇC   Çƒ  cx23Çƒ  885 ‰|$ó«º¼  º    Çƒ|  cx23Çƒ€  885 Çƒ„  inteÇƒˆ  rnal‹†Ì   ˆ   ‰Chƒ  ‰$‰øèüÿÿÿ‰Áƒø†¨   ƒø0…å$  ‹$‰úèüÿÿÿF‰[‰ƒ„   ‰èèüÿÿÿ‹ƒl  ‰«  …À…Ì$  ‹=Œ  …ÿ…$  ¡ˆ  …À…-$  t$1À¹   º!	  ‰÷ó«¹	   ‰ğèüÿÿÿ‰ò¹Ì	  ‰èh    èüÿÿÿ‹ƒl  Z‹T$Dd+    u&ƒÄH[^_]Ãt& ƒø0„XÿÿÿƒÁƒù0†Lÿÿÿé%  èüÿÿÿèüÿÿÿƒÀèüÿÿÿ1ÀÃèüÿÿÿS‰Ãƒì‹Œ  d¡    ‰D$1À…É…%  ƒú¸D  º   ¹   f‰D$”À€f‰T$T$ˆD$D$‰D$ƒ
  ÇD$D  è&ıÿÿ‹D$d+    uƒÄ[Ãèüÿÿÿfffffffèüÿÿÿ‹@(Ç€¤   ğ  Ç€¨       Ç   ‹ˆ¤   ¯ˆ¨   ‰È‹L$‰1ÀÇ    Ãèüÿÿÿ‹€$  ‹@ ‹ ‹€Ü      ƒút …Òtº   ‰1ÀÃfº   ‰1ÀÃ¶    º   ‰1ÀÃ¶    èüÿÿÿS1É‹˜4  …Ò”Áº   Q1É‹[èüÿÿÿZ[Ã´&    v èüÿÿÿ1ÀÃ´&    èüÿÿÿS‹ˆà  …Étèüÿÿÿ‰Ã…Àˆ?%  1Û‰Ø[Ã´&    t& èüÿÿÿWVS‹¸<  ‰X  ‹Ğ  Ç€<      …Òt‰Ãèüÿÿÿ‰»<  ‰Æ…ÀˆU%  1ö‰ğ[^_Ã´&    fèüÿÿÿS‹ˆÜ  …Étèüÿÿÿ‰Ã…Àˆk%  1Û‰Ø[Ã´&    t& èüÿÿÿWVS‹¸X  ‰<  ‹Ğ  Ç€X      …Òt‰Ãèüÿÿÿ‰»X  ‰Æ…Àˆ%  1ö‰ğ[^_Ã´&    fèüÿÿÿS‰Ó‹”   Hƒú~2‰Èèüÿÿÿ…Àt‹@…Àt‹ˆ  …Ét‰Ú[éüÿÿÿ¶    [Ã¶    º   ‰ÈèüÿÿÿëÇ´&    v èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿ‹@(éüÿÿÿv èüÿÿÿ‹@(‹˜   Šˆıÿÿ˜   èüÿÿÿ1ÀÃ´&    ´&    èüÿÿÿ‰Â‹ ‹@(‹ éüÿÿÿ´&    ¶    èüÿÿÿ‹‹R(éüÿÿÿèüÿÿÿU¹   WV‰ÆS‹€$  ‰Óº 0  ‹h ‹} ‰øèüÿÿÿƒût!ƒût|…ÛtH‹@  ‰Ú‰ğ[^_]éüÿÿÿt& ‰øº    èüÿÿÿ‰øº   èüÿÿÿ‹@  ‰Ú‰ğ[^_]éüÿÿÿt& ‰øº    èüÿÿÿ‰øº   èüÿÿÿ‹@  ‰Ú‰ğ[^_]éüÿÿÿt& ‰øº    èüÿÿÿ‰øº   èüÿÿÿ‹@  ‰Ú‰ğ[^_]éüÿÿÿt& èüÿÿÿU¹   WV‰ÆS‹€$  ‰Óº   ‹h ‹} ‰øèüÿÿÿƒût!ƒût\…Ût8‹@  ‰Ú‰ğèüÿÿÿ1À[^_]Ãf‰øº   èüÿÿÿº   ‰øèüÿÿÿëÎ¶    ‰øº   èüÿÿÿº   ‰øèüÿÿÿë®¶    ‰øº   èüÿÿÿº   ‰øèüÿÿÿë¶    èüÿÿÿVS‰Ã‹€$  ‹p ‹‹¸  ƒøw9ƒøw9ƒøuƒ»@  º    ‰Èt?èüÿÿÿ‹–<  …Òt(‰Ø[^éüÿÿÿ´&    ƒø!uáº   ‰ÈëÓ´&    v 1À[^Ãv èüÿÿÿë¿èüÿÿÿUW‰ÏVSƒì‹0‰$‹\$®¤  ‰èèüÿÿÿ‹†Ü   º   ‰$  ‹†Ü      ‰ø¶À€ÌÇ‰‹†Ü      ‹€æû‰‰ù‹†Ü   ¶ıÏ Ë  ‰¸   ‹†Ü      ‹€æ÷‰€<$ ‹†Ü   „Ò   ºÿ  ‰$  ‹†Ü      ‹€æş‰‹†Ü      ‹
€<$À% Àÿÿ-@  !È‰‹=    ¡    ƒÇ9Çy&é   t& ¸2   ¹   ºd   èüÿÿÿ¡    9Çx‹†Ü   ‹˜   öÇuÒ¶Û‹†Ü      ‹€   €ÌÃ‰‹D$‰‰èèüÿÿÿ1À€<$ u‹D$‹ Áøƒà‡ƒÄ[^_]Ã´&    ¶Û€ÏÃ‰˜   é(ÿÿÿ1Ûë¤v èüÿÿÿUWVS‰Ãƒì‹5    ‹€Ü   ‰$‹|$ƒÆ   ‹ ‹‹Ü   …ÿ„—   Á$  ‹)Íÿ   ‰)‰Á€äı€Í…ÒEÁ‹‹Ü   €äò‰Â€Ì€Î…ÿEÂ‰   ët& ‹    9Öx¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄuÚ‹“Ü   Š   ‹’   €Î‰¶À…ÿEøƒÄ[^‰ø_]Ã´&    f©$  ‰é‹m å ÿÿÿ‰)Š$é^ÿÿÿt& èüÿÿÿVpº   S‰Ã‰ğèüÿÿÿ…Àt‹@…Àt‰ğèüÿÿÿ‹³8  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹³4  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹³0  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹³,  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹‹¸  ƒøt!ƒøu
‹S‰ÈèüÿÿÿÇƒ      1À[^Ãv ‰Øèüÿÿÿ1ÀÇƒ      [^ÃfèüÿÿÿS‹tY  ‹@(‰˜€  ‹xY  ‰˜„  Ç   ¡tY  ¯xY  ‹L$‰¡|Y  [‰1ÀÃ´&    t& èüÿÿÿ‹…ÀuÇA,MPEG1ÀÃv ¸êÿÿÿÃfèüÿÿÿ‰Â‹ ‹@(Ü  éüÿÿÿ´&    v èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿ‹‹R(ÂÜ  éüÿÿÿ´&    t& èüÿÿÿUWVSƒì(d‹    ‰T$$1Òèüÿÿÿ‹°(  †¼  nPhI  j"|$Wèüÿÿÿ‹FƒÄ9Åt'X´t& ‹Cd‹ …Àt‹…Òt‰Øèüÿÿÿ‹CLX´9Åuà†Ô  ‰úèüÿÿÿ‹D$$d+    u
ƒÄ(1À[^_]Ãèüÿÿÿ´&    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ƒ¿€  tLÇF   ‹‡Ü  ƒÇ‰F‹X´9Çt*¶    ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ƒ¿€  t<‹GƒÇX´9Çt)t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿS‰Ëèüÿÿÿ‹€(  ‹|  ‹€x  ‰S‰1À[Ã´&    èüÿÿÿS‰Ëèüÿÿÿ‹(  ÇCMPEGÇC    ‹‚€  ¯‚„  ÇC    ÇC   ‰C¡¤  …À…©k  1À[ÃèüÿÿÿS‰Ëèüÿÿÿ‹€(  ÇCMPEGÇC    ‹€  ¯„  ÇC    ‰S‹ˆ  ‰S‹Œ  ÇC   ‰S‹¤  …Ò…Ìk  1À[Ã´&    ¶    èüÿÿÿéüÿÿÿ¶    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ƒ¿€  th‹…Òu[F¹   ºN  ƒÇèüÿÿÿ‹X´9Çt*¶    ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ¡¤  …À…ïk  1À[^_Ã¸êÿÿÿëõ¸êÿÿÿëîv èüÿÿÿéüÿÿÿ¶    èüÿÿÿéüÿÿÿ¶    èüÿÿÿVS‰Ëèüÿÿÿ‹°(  ¡¤  …À…l  ‰Ú‰ğ[^éüÿÿÿt& èüÿÿÿUWVS‹l$1Û‹|$èüÿÿÿº   ‹°(  1Àë´&    f›‹…ä  ‹…è  !ê!ø	ĞuƒÃƒûuß¸êÿÿÿ[^_]Ãt& ‰ê‰ù‰ğèüÿÿÿ…Àuç›Áá‘à  ‹‰à  ‰  ‹J‰   ‹J‰$  ‹J‹R‰(  ‰–,  [^_]ÃèüÿÿÿUWVS‰Ëèüÿÿÿº   ‹°(  ¾¼  ‰øèüÿÿÿƒø‡1l  ‰ÁƒøtƒÁƒù‡'l  ‰ú‰Økèüÿÿÿ‹†Ü  º    i€¸    ‹¸    ‰øèüÿÿÿ‰Áƒøÿ„1l  ƒø tƒÁƒù ‡'l  ‰ú‰èèüÿÿÿ‹–Ì   ‹‚´   …Àu‹‚ˆ   PC0hr  Pèüÿÿÿ¸ …º …ÇCT …ƒÄƒ¾€  DÂ‰CT[1À^_]Ã´&    t& èüÿÿÿS‰Ëèüÿÿÿ‹(  ÇCMPEGÇC    ‹‚€  ¯‚„  ÇC    ÇC   ‰C¡¤  …À…;l  1À[Ãèüÿÿÿƒ=¤  S‰Ã‡Zl  ‹ƒÜ   º7   ‰(  º   ‹ƒÜ   ‰$  º à  ‹ƒÜ   ‰   [Ã¶    èüÿÿÿWV¾   S‰Ã‰Ğ‰Ê‹»Ü   ‰·$  ¶É‹³Ü   ‰ÏÏ X  ‰¾   ‹»Ü   €Íø‰   ¶Î‹³Ü   ‰ÏÏ Y  ‰¾   ‹»Ü   €Íù‰   ‰Ñ‹³Ü   Áé¶É‰ÏÏ Z  ‰¾   ‹»Ü   €Íú‰   Áê‹»Ü   ‰ÖÎ [  ‰·   ‹»Ü   €Îû‰—   ¶Ğ‹»Ü   ‰Ñ€Í\·É‰   fÊ ü‹»Ü   ·Ò‰—   fÁè‹»Ü   ‰Â€Î]·Ò‰—   f ı‹“Ü   ·À‰‚   º^  ‹ƒÜ   ‰   ºş  ‹ƒÜ   ‰   ‹5    ƒÆë¡    9Æx'¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄtÛ[1À^_Ãt& [¸ÿÿÿÿ^_Ã´&    èüÿÿÿUWVS‰Ã‰Ğƒì‹»Ü   ‰$¹   ‰$  ¶Ò‹»Ü   ‰Ñ€Í\·É‰   fÊ ü‹»Ü   ·Ò‰—   fÁè‹»Ü   ‰Â€Î]·Ò‰—   f ı‹»Ü   ·À‰‡   º ^  ‹ƒÜ   ‰   º ş  ‹ƒÜ   ‰   ‹5    ƒÆë ´&    f¡    9Æˆ[  ¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄt×1ÿ‹ƒÜ   ºÿ  ‰$  º X  ‹ƒÜ   ‰   º ˜  ‹ƒÜ   ‰   ‹ƒÜ   ‹   ¹ ğ  ‹ƒÜ   ‰ˆ   ¾ Y  ‹ƒÜ   ‰°   ¾ ™  ‹ƒÜ   ‰°   ‹ƒÜ      ‹ ‰D$‹«Ü   ‰   ¾ Z  ‹«Ü   ‰µ   ¾ š  ‹«Ü   ‰µ   ‹«Ü   ‹…   ‹«Ü   ‰   ‹«Ü   µ   ½ [  ‰.‹«Ü   µ   ½ ›  ‰.‹«Ü   ‹µ   Áæ¶Ò	ò‹t$Áæ·ö	Ö‹“Ü   ‰Š   Áà%  ÿ 	ğ‰Â‹$‰ƒÄ‰ø[^_]Ã´&    ¿ÿÿÿÿé¹şÿÿ´&    ´&    èüÿÿÿWV¾   S‰Ã‰È‹»Ü   ‰·$  ¶É‹³Ü   ‰ÏÏ P  ‰¾   ‹»Ü   €Íğ‰   ¶Ì‹³Ü   ‰ÏÏ Q  ‰¾   ‹»Ü   €Íñ‰   ‰Á‹³Ü   Áé¶É‰ÏÏ R  ‰¾   ‹»Ü   €Íò‰   Áè‹»Ü   ‰ÆÎ S  ‰·   ‹»Ü   €Ìó‰‡   ‰Ğ‹»Ü   Áèƒà?‰ÆÎ@T  ‰·   ‹»Ü   @ô  ‰‡   ¶Æ‹»Ü   ‰ÆÎ U  ‰·   ‹»Ü   €Ìõ‰‡   ¶Ò‹ƒÜ   ‰Ñ€ÍV‰ˆ   ‹ƒÜ   €Îö‰   ‹5    ƒÆët& ¡    9Æx'¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄtÛ[1À^_Ãt& [¸ÿÿÿÿ^_Ã´&    èüÿÿÿUWVS‰Ã‰Ğƒì‹»Ü   ‰$¹   ‰$  Áê‹»Ü   ƒâ?‰ÖÎ T  ‰·   ‹»Ü   €Îô‰—   ¶Ô‹»Ü   ‰ÖÎ U  ‰·   ‹»Ü   €Îõ‰—   ¶À‹»Ü   ‰Á€ÍV‰   ‹»Ü   €Ìö‰‡   ‹5    ƒÆë´&    ¡    9Æˆc  ¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄt×1ÿ‹ƒÜ   ºÿ  ‰$  º S  ‹ƒÜ   ‰   º “  ‹ƒÜ   ‰   ‹ƒÜ   ‹   ¹ ğ  ‹ƒÜ   ‰ˆ   ¾ R  ‹ƒÜ   ‰°   ¾ ’  ‹ƒÜ   ‰°   ‹ƒÜ      ‹ ‰D$‹«Ü   ‰   ¾ Q  ‹«Ü   ‰µ   ¾ ‘  ‹«Ü   ‰µ   ‹«Ü   ‹…   ‰D$‹«Ü   ‰   ‹«Ü   µ   ½ P  ‰.‹«Ü   µ   ½   ‰.‹«Ü   ‹µ   ‰ğÁâ¶ğ	ò‹t$Áææ  ÿ 	Ö‹“Ü   ‰Š   ‹D$Áà·Ğ‹$	ò‰ƒÄ‰ø[^_]Ã¶    ¿ÿÿÿÿé±şÿÿ´&    ´&    èüÿÿÿU‰ÍWV‰ÖS‰Ãƒìd¡    ‰D$1Àƒ=¤  ÇD$    ‹|$(‡!m  ‹ƒ¼  ‰áPü‰Øèüÿÿÿ‹$=xV4…ıl  ‹“¼  L$‰Øèüÿÿÿ‹D$…À…Ùl  ‹“¼  ¹   ‰ØÇD$   èüÿÿÿ‹ƒ¼  ‰ñ1öP‰Øèüÿÿÿ‹ƒ¼  ¹ô  P‰Øèüÿÿÿ…í~5t& ‹ƒ¼  ‹·T‰Øèüÿÿÿƒ=¤  ‡¾l  ƒÆ9õuØƒşv ‹ƒ¼  1ÉT‰ØƒÆèüÿÿÿƒşuå‹“¼  ¹   ‰ØÇD$   èüÿÿÿ‹5    ƒÆ
ë¡    9Æˆ«l  ¸Æ§  èüÿÿÿ‹“¼  L$‰ØèüÿÿÿöD$tÑ‹T$$‹l$$1ö…Ò~-v ‹ƒ¼  ‰ùT‰Øèüÿÿÿƒ=¤  ‡‘l  ƒÆƒÇ9õuÖ‹ƒ¼  L$P‰Øèüÿÿÿƒ=¤  ‡vl  ‹“¼  1É‰Øèüÿÿÿ‹D$‹T$d+    uƒÄ[^_]Ãèüÿÿÿ´&    èüÿÿÿUWVSƒìHd¡    ‰D$D1À‹t$\‹|$dƒ=¤  ‡gm  \$l…ÿt{‰4$D$pl$‰ê¸v ‹pü‰ÃƒÀƒÂ‰rü9Èuî‹4$U‰ù‰ğÿt$l‹T$hè¡ıÿÿZY‹t$h…öt‹‹L$ƒ|$h‰
u	‹S‹L$‰
‹T$Dd+    uƒÄH[^_]Ã´&    v l$ë§èüÿÿÿt& èüÿÿÿéFıÿÿ¶    èüÿÿÿUWVS‰Ãƒì,‹¤  ‰T$d¡    ‰D$(1À…É…ëm  j j h€   SèõşÿÿƒÄ…À‰;  ƒ=¤  ‡Ån  ÇD$    ÇD$    L$º   ‰Øèüÿÿÿº  ‰Ø|$‰ùèüÿÿÿ¹íÿÿÿºX  ‰Øèüÿÿÿ¹ÿÿÿÿºT  ‰Æ‰Øèüÿÿÿ¹  €ºø  	Æ‰Øèüÿÿÿ¹   ºü  	Æ‰Øèüÿÿÿ1Éºd   	Æ‰Øèüÿÿÿ	ğ…¯n  ‹ƒÌ   º‚  ˆˆ   D$èüÿÿÿ…À…n  ‹D$‹úÀ …n  ‹@8§  ‰Át8h/  èüÿÿÿ‹D$èüÿÿÿXhD  hh0  èüÿÿÿXZÇD$ÿÿÿÿéô  ¶    xf»Uªu¿ƒ=¤  ‡.  ‰$1í1ö‰Ëë	´&    ‰Í‹«‹$‰ê)Î‰L$ƒîèüÿÿÿ…À…Òm  ‹D$M‹ Áè9ÈwĞ‹¤  ‹$…É…]o  …íx%v L$‰ê‰Øèüÿÿÿ…À…Do  ‹D$tƒísŞ…ö…+o  ‹D$èüÿÿÿ¡¤  …À…o  ¹ÿÿÿÿºT  ‰Øèüÿÿÿ‹L$º   ‰Æ‰Øèüÿÿÿ‹L$º  	Æ‰ØèüÿÿÿºX  	ÆL$‰Øèüÿÿÿ‹L$ºX  	Æ‰Øƒáèèüÿÿÿ¹ @  º   	Æ‰Øèüÿÿÿ¹ @  º  	Æ‰Øèüÿÿÿº   	ÆL$‰Øèüÿÿÿ‰ùº  ‰Øèüÿÿÿ…öˆo  ƒ=¤  ÇD$xV4ÇD$xV4ÇD$ 4xVÇD$$V4x‡so  1ö1íëv 1öƒÅıÀ „¥o  ‰ù‰ê‰Øèüÿÿÿ‹D$9D´uÚƒÆƒşuÔ¡¤  ƒÅ…À…ºm  ‰«¼  j j h€   SèíûÿÿƒÄ…ÀˆQ  Wjj hÄ   SèÒûÿÿƒÄ…Àˆ¢m  ‹=¤  …ÿ…‡m  ¸È   èüÿÿÿ‹5¤  …ö…rn  ‹ƒ   ¾   ‰ß% ù  ƒøÀƒà`à  ‰ƒŒ  ÿ³ˆ  Pj jh‘   Sèfûÿÿ‹ƒˆ  ƒÄf‰ƒÈ  ‹ƒŒ  f‰ƒÊ  1À÷ƒ   ÿÿ •Àf‰ƒÌ  ƒÀ  èüÿÿÿjjj jhÜ   Sèûÿÿjjj jhÜ   Sèûÿÿ¸<   ƒÄ0èüÿÿÿhğ   hğ   j jhÖ   SèÜúÿÿj j j j j j j j j j j j j jh×   SèµúÿÿƒÄXj@jhÀ€` hU‘U‘jjh ½ j jhÈ   SèŒúÿÿƒÄ,´&    fFí1Ûj ƒøj –Ãj SVj jh·   Wè^úÿÿ‰ğƒÆƒÄ$   €j j j SPj jh·   Wè:úÿÿƒÄ$ƒşu²j j jhÚ   Wè!úÿÿ¸<   èüÿÿÿj j hÍ   Wèúÿÿ¸<   ƒÄ$èüÿÿÿ¹€   ºH  ‰øèüÿÿÿ‹L$…Éu"‹D$(d+    u?‹D$ƒÄ,[^_]Ã´&    fj j j jh   Wè­ùÿÿ¸
   èüÿÿÿƒÄÇD$    ë´èüÿÿÿéÎo  é(n  ´&    èüÿÿÿUº   WVSƒì‹h(‰èt  è/úÿÿ‰Æ…Àty½¸  ‰øèüÿÿÿ‰$‹…t  9ØtD´&    ‹…t  ‹P‹-x  ‰Q‰
º   Ç€x     Ç€|  "  èüÿÿÿ‹…t  9ÃuÃ‹$‰øèüÿÿÿ‰ğƒÄ[^_]Ã´&    ‹…t  ‰Úˆˆıÿÿ…Ü  èüÿÿÿ‰ğƒÄ[^_]Ã´&    ¶    èüÿÿÿV‰ÆS‰Óº  ƒìd¡    ‰D$1À‰á‰ğèüÿÿÿ‹D$d+    u·Ë$ƒÄ‰ğ[º  ^éïÿÿèüÿÿÿ´&    ´&    èüÿÿÿV‰ÆS‰Óº  ƒìd¡    ‰D$1À‰á‰ğèüÿÿÿ‹D$d+    u·Ë‰ğº  ÷Ñ#$ƒÄ[^é²îÿÿèüÿÿÿ´&    ¶    èüÿÿÿW‰ÏV‰ÆS‰Óº   ƒìd¡    ‰D$1À‰á‰ğèüÿÿÿ·Ë…ÿt&$‹D$d+    uƒÄ‰ğº   [^_éLîÿÿt& ÷Ñ#$ëÖèüÿÿÿ´&    t& èüÿÿÿƒìd‹    ‰T$1ÒT$ÇD$    Ç$    RT$Rjj hÆ   PèC÷ÿÿ¡¤  ƒÄ…À…æo  ‹D$d+    uƒÄÃèüÿÿÿ´&    ¶    èüÿÿÿS‹X(j j jj jh‚   Sèòöÿÿ¸ô  èüÿÿÿ‰ØèüÿÿÿƒÜ  ƒÄ[éüÿÿÿfèüÿÿÿS‰Ã¡¤  …À…
p  ‹ƒÄ  …Àt%‹ğ  ƒâu&èüÿÿÿƒÔ  èüÿÿÿÇƒÄ      [Ã´&    v èüÿÿÿëØ´&    fèüÿÿÿUWVS‰Ãƒì¡¤  …À…op  iƒ¸    ƒ¸   …N  Çƒ(      ³À  º2   Çƒ,      ‰ğÇƒ  •  Çƒ      Çƒ$      ÇƒŒ  à  Çƒˆ  Ğ  ÇƒÄ     èüÿÿÿ‰Å…ÀtƒÄ‰è[^_]Ãt& ‰›D  1Ò‰ğ»Œ  @  ÇƒH  p‡  ”Âèüÿÿÿ“Ô  C\1Éj èüÿÿÿ‹ƒÜ  ‰D$X¡¤  …À…‹p  èüÿÿÿ‰Â…Àt^‰Ç¾@V  ¹Í   ó¥hœ  ‹ƒÜ  i€¸    ÿ°    ‚À  h¡  j ‰T$Pèüÿÿÿ‹D$‹T$ƒÄÇ‚      ƒÀ‰‚¬  ‹ƒÌ   »  ‰“Ä  ³È  ÇƒÈ     ˆ   ‰›ğ  ‰ƒĞ  ‰ğÇƒÌ     Çƒ      Çƒ     Çƒø     Çƒä     Çƒè      Çƒü      ‰»Ü  èüÿÿÿ…Àˆ   ‹ƒÄ  ‰˜(  ‹ƒÄ  ‰¸0  ‹ƒÄ  ‰°¸  ‹ƒÄ  Ç€ä     ƒ»€  t‹ƒÄ  ˆä      ‹ƒÄ  ¹ÿÿÿÿ‹à   ÿ21ÒjèüÿÿÿY_‰Æƒ¼  …ö‰&p  éSp  ƒÄ‰Å[‰è^_]Ãt& ½íÿÿÿéşÿÿfffèüÿÿÿSğºp’Â¶ÒÁâğºp’Ã‰ÑƒÉ„ÛEÑğºp’Ã‰ÑƒÉ„ÛEÑğºp sƒÊ‹H(…Ét-¬  [éüÿÿÿ´&    …Òuâ[Ã´&    v èüÿÿÿğºp Ãt& èüÿÿÿ‹H`öÂtğ€‰¸  öÂtğ€‰¸  öÂtğ€‰¸  ƒâtğ€‰¸  ‘¨  9  t‰Ñ‹    ¸    éüÿÿÿt& ‰Ğéÿÿÿ´&    fèüÿÿÿƒâ‹H`tğ€‰Ì  9  t‹    Á¼  ¸    éüÿÿÿt& ğº±Ì   ÃfffffffèüÿÿÿWVS˜hæÿÿƒì‹@üd‹    ‰T$1ÒÆD$ …Àt6‹Pd‹…Òt-‹r$…öt&‹    L$…Òtx‹z$…ÿtqº   èüÿÿÿ€|$ u<º   ‰Øèüÿÿÿº   ƒÈ€¶È‰Øèüÿÿÿº#   ‰Øèüÿÿÿº#   ƒÈ€¶È‰Øèüÿÿÿ‰Øº   èüÿÿÿ‹D$d+    uƒÄ[^_Ãt& º   èüÿÿÿëèüÿÿÿfffffffèüÿÿÿVSƒì0‹€h  d‹    ‰T$,1Ò‹…Û„)  ‹ƒ¨  …À„  ÇƒØ      ‹Pd‹R…Ò„³   ‹J…É„¨   ‹   …Ò„Ê  ‹r…ö„¿  ‰âèüÿÿÿ‹‹¸  ‹ƒ¨  ƒù†{  ƒé!ƒùw#º   Óâ÷Âğ…0  â‚  …€   t& …À„È   ‹Pd‹R…Ò„º   ‹J…É„¯   ‹   …Ò„Y  ‹Z…Û„N  ‰âèüÿÿÿ1ÀéŠ   ‹‹¸  ƒù†™   ƒé!ƒùw¯»   Óã‰Ù÷Ãğ…¬   á‚  t’ÇD$ è€  ÇD$$ø§  ÇD$ ] ÇD$    ÇD$  ÇD$¦ß¼ ÇD$”  ÇD$!   ÆD$…À…?ÿÿÿ´&    1À‹T$,d+    …¿   ƒÄ0[^Ã´&    ƒù†ÿÿÿ»   Óãã uƒù„kÿÿÿ…Ò…ışÿÿëµt& ÇD$    ÇD$  ÇD$ÕÜ2 ÇD$ ÆD$é¶şÿÿ¶    ƒù†§şÿÿº   Óââ u¸ƒù…şÿÿéÿÿÿf‰âèüÿÿÿé<şÿÿt& ‰âèüÿÿÿé<ÿÿÿt& ¸íÿÿÿé0ÿÿÿèüÿÿÿèüÿÿÿU‰ÅW1ÿVSì  ‹Ô  ‰T$d¡    ‰„$  1À‹…¨  ÇD$    …Àtz‹Pd‹R…Òtp‹2…ötj‹   …É„„   ‹   ‹	‰$…ÉtuL$Q¹   ‹t$T$èüÿÿÿZ‹D$‰ÆÁît1ÿ‹CTüƒÇèüÿÿÿ9şuí‹D$¿   …À…xÿÿÿt& €|$ uA‰ø„Àu1‹„$  d+    u^Ä  [^_]ÃL$Q¹   T$èüÿÿÿXë‹CèüÿÿÿëÅ‹[T$ÇD$    ÇD$    ‰ØÆD$èüÿÿÿÆƒ0  ‰Øèüÿÿÿë’èüÿÿÿ´&    èüÿÿÿWVS‰Ãƒì0d¡    ‰D$,‹ƒ¨  …À„ü   ÇƒØ     ‹Pd‹R…Òt'‹J…Ét ‹   …Ò„
  ‹r…ö„ÿ   ‰âèüÿÿÿ€|$
 ‹5   …   ´&    1ÀÆD$
f‰D$‹ƒ¨  …Àtr‹Pd‹R…Ò„¡   ‹J…Ét4‹   …Ò„—   ‹z…ÿ„Œ   ‰âèüÿÿÿ‹ƒ¨  …Àtp‹Pd‹R…Òtf‹J…Ét_‰â…ötT‹~…ÿtMèüÿÿÿ¶D$
„À„{ÿÿÿv ƒ˜  èüÿÿÿƒ¬  èüÿÿÿƒÀ  èüÿÿÿ‹D$,d+    u6ƒÄ0[^_Ãv èüÿÿÿ¶D$
ë±t& ‰âèüÿÿÿéoÿÿÿt& ‰âèüÿÿÿéüşÿÿèüÿÿÿ´&    èüÿÿÿ‹€h  ‹ …Àtézşÿÿ´&    v Ã´&    ´&    èüÿÿÿUWVS‰Ãƒì0‹‹¨  d¡    ‰D$,1À…Ét…Òt‹ƒ¸  ƒøw-ƒø‡  t& ‹D$,d+    …Ó  ƒÄ0[^_]Ã´&    ƒè!ƒøwØ¾ƒñ£ÆsÎ‰Öƒæƒâ„Ü   ‹Ad‹@…À„ú   ‹x…ÿ„ï   ¡   ‰â…À„0  ‹h…í„%  ‰Èèüÿÿÿ‹‹¨  ‹ƒØ  ÆD$ …À•D$
…É…À   …ö…  ‹ƒØ  ÆD$…À‹ƒ¨  •D$
…À„>ÿÿÿ‹Pd‹R…Ò„0ÿÿÿ‹J…É„%ÿÿÿ‹   …Ò„ß   ‹Z…Û„Ô   ‰âèüÿÿÿé ÿÿÿ¾€£Æƒòşÿÿ‰Öƒæƒâ…$ÿÿÿ…ö„Üşÿÿ‹D$,d+    …¯   ƒÄ0‰Ø1Ò[^_]é›ûÿÿv ‹ƒØ  ÆD$ …À•D$
‹Ad‹@…À„2ÿÿÿ‹x…ÿ„'ÿÿÿ¡   ‰â…ÀtZ‹h…ítS‰Èèüÿÿÿé	ÿÿÿ´&    ‰Èèüÿÿÿ‹‹¨  éÖşÿÿ¶    º   ‰Øè$ûÿÿéáşÿÿ´&    ‰âèüÿÿÿé,şÿÿt& ‰Èèüÿÿÿé¶şÿÿèüÿÿÿ´&    èüÿÿÿUWVS‹ˆ¨  …É„Ä  ‹ˆ¸  ‰Æƒù"„¸  †Ú  ƒù0„I  †‹  ¿Á  ƒù3‡e  t& ¡   ¹   ºÀ  èüÿÿÿ‰Å…À„|  ‰0i†¸    ÿ°    hÖ  hÀ  èüÿÿÿ‰EƒÄ…À„^  ‹–Ì   ‹‚´   …À„§  Phæ  hÀ  èüÿÿÿ‰EƒÄ…À„%  ¸   èüÿÿÿ‰Ã…À„í  ‰E‹Eº   ‰ƒØ  ‹Ef‰“æ  ‰ƒÜ  ¸   f‰ƒà  ‹†Ì   ·P$f…Ò…E  ·@ f‰ƒâ  ‹†Ì   ·@"f‰ƒä  ‹†Ì   ‰»ì  ˆ   ‰«h  ‰C$‰ØÇƒ4  üÿ¿Çƒ8      Çƒà  ğ”  Çƒä   š  Çƒè  ñ  ‰®Ô  èüÿÿÿ‰Ç…À…î   [‰ø^_]Ã´&    ¿íÿÿÿƒù:uè¿g  éŒşÿÿt& ƒù)w#¿¶  ƒù'‡uşÿÿ¿íÿÿÿ[‰ø^_]Ã´&    fƒé-ƒùwã¿Á  éNşÿÿ¶    ƒùw3¿íÿÿÿƒùv‹¸   Óà© u•ƒù…tÿÿÿ¿  éşÿÿ´&    ¿œ  ƒù!„şÿÿ¿íÿÿÿë‹v ‹‚ˆ   éNşÿÿt& f‰“â  ‹†Ì   ·@&éºşÿÿf¿Ë  éÆıÿÿ¶    ‰ğè™ùÿÿ‰ØÇ†Ô      èüÿÿÿ‹Eèüÿÿÿ‹Eèüÿÿÿ‰èèüÿÿÿ‰ø[^_]Ã´&    v ¿ôÿÿÿëÑ´&    f¿t  éfıÿÿ¿ôÿÿÿé®şÿÿ¿ôÿÿÿëµ¿ôÿÿÿë¶´&    ´&    èüÿÿÿS‰Ãèùÿÿ‹ƒÔ  …Àt9‹@èüÿÿÿ‹ƒÔ  ‹@èüÿÿÿ‹ƒÔ  ‹@èüÿÿÿ‹ƒÔ  èüÿÿÿÇƒÔ      [Ãfffffffèüÿÿÿ‰Â‰È‹’”   ‹’À   ‹’Ü   Š  ‹’  ƒÊ ‰‹T$‰1ÀÃ´&    t& èüÿÿÿWVS‹°”   ‰Ó¾P  ‰øèüÿÿÿ‹–$  ‰‹–(  ‰S‹–,  ‰S‹–0  ‰S‹–4  ‰S‹–8  ‰S‹–<  ‰S‹–@  ‰S‹–D  ‰S ‹–H  ‰S$‹†L  ‰C(‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿWVS‹°”   ‰Ó¾ğ   ‰øèüÿÿÿ‹–Ä   ‰‹–È   ‰S‹–Ì   ‰S‹–Ğ   ‰S‹–Ô   ‰S‹–Ø   ‰S‹–Ü   ‰S‹–à   ‰S‹–ä   ‰S ‹–è   ‰S$‹†ì   ‰C(‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿW¸P  VS‹°À   ‰Ã‰øèüÿÿÿ‹†Ü     ‹ƒâß‰‹†Ü      ‹âıÿÿ‰‹†Ü      ‹ƒâß‰ºÿÿ  ‹†Ü   ‰  Æƒ.  ‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿW¸ğ   V‰ÆS‹˜À   ‰øèüÿÿÿ‹ƒÜ     ‹ƒâì‰‹ƒÜ      ‹â¿şÿÿ‰‹ƒÜ      ‹ƒâï‰‹ƒÜ      ‹ƒâó‰1Ò‹ƒÜ   ‰  ºÿÿ  ‹ƒÜ   ‰  Æ†Î   ‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿUW‰ÇVSì„   ‰D$‰L$d¡    ‰„$€   ‹‡”   ‹°À   ‰D$‹†Ü      ‹ ‰D$ ‹†Ü   ‹¸  ‹†Ü   ‹˜  ‰Ø‰Ú‰ıÆ ƒàƒâ ƒå ‰D$‰Øƒà‰$‰øƒà‰D$‰øƒàƒ=¨  ‰D$‡ìp  …ít…Ò…¶  !û‹|$‰Ø‹$ƒàˆD$…Û‹D$•Â…ÿ¨   •À!Â‹D$ˆT$…À„p  ‹L$1Û…É„b  ‹D$  ‰$¶    ‹†Ü   @  ‹ ‰Ââÿÿıÿ‰TÜ@ƒÃ©   t=ƒûuØ‰èèüÿÿÿ¹@   ‰Ç‹$T$@èüÿÿÿ‰ú‰Ã‰èèüÿÿÿƒû@„ê   éõq  t& ‰èÁãèüÿÿÿ‰Ù‰Ç‹$T$@èüÿÿÿ‰ú‰$‰èèüÿÿÿ;$…õq  ‹D$ÇD$<    Æ €|$ …r  €|$ ¸ÿşÿÿt*ƒL$<‹|$ ‹Ü   !ø‰   ‹†Ü   ‰¸   ‹D$Æ ‰èèüÿÿÿ‹L$‰Â‹  +  =ÿ  vƒL$<‰èèüÿÿÿ‹\$<…Ût‹L$…É…©   ‹„$€   d+    …Ä   Ä„   1À[^_]Ã1Ûé¹şÿÿ´&    f€|$ tU1Ûéşÿÿ‹†Ü     ‹ƒâß‰‹D$ÇD$<   …Àt ‹@`…Àt‹h8…ít‹D$L$<ºv@èüÿÿÿ‹D$Æ éüıÿÿ€|$ ÇD$<    „ ÿÿÿéQr  t& ‹|$‹G`…À„Hÿÿÿ‹X8…Û„=ÿÿÿL$<º v@‰øèüÿÿÿé(ÿÿÿèüÿÿÿt& èüÿÿÿU‰ÕWVSƒì$‹”   ‰D$ ‹‚  ‹š  ‰D$ƒáøu‹D$8Ç     ƒÄ$1À[^_]Ã´&    º   ‰L$‰ø‰$èüÿÿÿ‹$‹L$‰Æ‚  ‰êèüÿÿÿ‰ò‰$‰øèüÿÿÿ‹$‹|$8‰Èƒàø‰‰ÈÁèt¤·ÛDÅ ƒÃ‰D$‰$Áû‰\$éë   ‰ğ‹\$Áèƒà‰D$…Ûtƒt$‹D$ƒàˆD$1ÛÇD$    ·ö1ÿ‹L$‹$¤÷Áæ‰ğ‰ş‰ÏƒÈ¯ò¯ø‰D$÷$$ş¿è  òiòè  ÷çò1ö‰Ñ‰Ç…Òt‰Öº÷ÚKÑî‰ğ÷â‰ĞÁêÁè‰ÖkÀ6)Á‰ø‰Ê¿6   ÷÷‰Ñ‰òƒùvƒÀº ¡ =çhÍw
ºÓMb÷âÁêÁã‰U 
\$ƒ=¨  ÇE    ˆ]‡wr  ƒÅ;l$„œşÿÿ‹u ‰ğ%ÿÿ =ÿÿ … ÿÿÿÆD$ »   ÇD$   ÇD$    éÿÿÿèüÿÿÿUWVSƒì$‹°”   €z
 ‹®À   …Î  ‹B‰Ó‰D$…À…  †P  ‰$èüÿÿÿ¶C
ˆ†.  ÇC    Ç†(      Ç   Ç†$     ‹…Ü     ‹ƒâß‰‹…Ü      ‹âıÿÿ‰¶K‹…Ü   Áá   ‹1Èƒàß1È‰¶Cˆ†/  „À„å   ‹K1ÒÁá‰ÈÑè€ù7÷ñ‰Á=   ‡M  ƒø†¬  @ÿ‰Ï‰D$Áç‰D$¸üŸ÷é‰D$‰øÑè‰T$ 1Ò€ù7÷÷iÑ   ¹÷ÚKƒÂÑê‰Ç‰Ğ÷áÁê‰T$‹…Ü   ‹L$‰ˆ  ‰{ºY·Ñ‰¾4  kKdÁ8  ‰È÷â¸   Áê	ùq  ƒÒÿ9ÂGĞ‹…Ü   ‰  ƒÂ‹D$kÒdƒÂÁê‰S‰–8  ‹T$ é  v ‹Cº ¡ ÇD$    9ĞGÂº6   ÷â‰Ñ‰Ç…Òt‰ÈºÓMb÷â‰ĞÁêÁè‰T$iÀè  )Á‰ø‰Ê¿è  ÷÷‰Ñ‹T$ùó  vƒÀƒÒ 1É‰D$‰×…Òt"¹@  ‰Ğ÷á‰ø)ĞÑè‰ÊÁê‰Ğ‰ÑÁà)Ğ)Ç‰ú‹D$¿ÿÿ ÷÷‰Ê¿   1É9Ç‰Ï×‚¨  ¿   9ÇÑƒ  ƒÀÿ‰ÁƒÒÿ‰D$‰ÇƒÁ¸üŸ‰T$÷é‰T$iÑ   ¹÷ÚK‰D$ƒÂÑê‰Ğ÷áÁê‰T$‹…Ü   ‰¸  ‹D$‹T$ÇD$    ‰Ñ‰Ç…Òt‰Ğº÷ÚKÑè÷â‰ĞÁêÁè‰T$kÀ6)Á‰ø‰Ê¿6   ÷÷‰Ñ‹T$ƒùvƒÀ‰C‰†0  ‹D$‰†d  ‹D$‰C(‰†L  ‹…Ü      ‹€æ÷‰¶K‹…Ü   Áá   ‹1È€äï1È‰¶Cˆ†=  ¶K‹…Ü   Áá   ‹1È€ä1È‰¶Cˆ†<  ¶C	ˆ†-  ¶Cˆ†,  „ÀtC€{	 t‹…Ü     ‹ƒÊ ‰‹…Ü   €{ ¿€  D|$   ‹€   1ø%ıÿÿ1ø‰‹$èüÿÿÿ1ÀƒÄ$[^_]ÃfƒÄ$‰ğ[^_]éöÿÿfÇD$÷J ¿ÿÿ  ÇD$ÿÿ  ÇD$  üÇD$Ÿ  é|şÿÿ¶    ÇD$÷J ¿3   ÇD$  üÇD$ Ÿ  ÇD$ÿÿ  ÇD$ÿÿ  éÓüÿÿ¶    ÇD$”   ¿   ÇD$   ÇD$0ø?ÇD$    éşÿÿ¶    ÇD$”   ¿Ì¿ ÇD$0ø?ÇD$     ÇD$   ÇD$   éküÿÿ¶    ¸Úÿÿÿéÿÿÿ¶    èüÿÿÿUWVSƒì$‹¨”   €z
 ‹½À   …  ‹B‰Ó‰D$…À…}  …ğ   ‰$èüÿÿÿ¶C
ˆ…Î   ÇC    Ç…È       Ç   Ç…Ä      ‹‡Ü     ‹ƒâì‰‹‡Ü      ‹â¿şÿÿ‰¶K‹‡Ü   Áá   ‹1Èƒàï1È‰¶Cˆ…Ï   „À„5  ‹s1ÒÁæ‰ğÑè€ù7÷ö‰Á=   ‡  ƒø†Œ  @ÿ‰Î‰D$Áæ‰D$¸üŸ÷é‰D$‰ğÑè‰T$ 1Ò€ù7÷öiÑ   ¹÷ÚKƒÂÑê‰Æ‰Ğ÷áÁê‰T$‹‡Ü   ‹L$‰ˆ  ‰sºËk(¯‰µÔ   ÇC2   Ç…Ø   2   ‹KÁáq	‰ğ÷â)ÖÑîò1öÁê9S sQ
¾ÍÌÌÌ‰Ğ÷æ¾   ÁêƒÁ‰S ºOìÄN‰È÷âÁê9S$vº«ªªª‰ÈƒÎ÷âÁê‰S$‹‡Ü      ‹1ğƒàü1ğ‰‹C ‹T$ ‰…ä   ‹C$‰…è   ‹D$é  t& ‹Cº ¡ ÇD$    9ĞGÂº6   ÷â‰Ö‰Á…Òt‰ğºÓMb÷â‰ĞÁêÁè‰T$iÀè  )Æ‰È‰ò¹è  ÷ñ‰Ö‹T$şó  vƒÀƒÒ 1ö‰D$‰Ñ…Òt"‰Èº@  ÷â‰È)ĞÑè4‰òÁê‰Ğ‰ÖÁà)Ğ)Á‰Ê‹D$¹ÿÿ ÷ñ‰ò1É¾   9Æ‰ÎÖ‚˜  ¾   9ÆÑƒ‘  ƒÀÿ‰ÁƒÒÿ‰D$‰ÆƒÁ¸üŸ‰T$÷é‰T$iÑ   ¹÷ÚK‰D$ƒÂÑê‰Ğ÷áÁê‰T$‹‡Ü   ‰°  ‹D$‹T$1ö‰D$‰Ñ…Òt‰Öº÷ÚKÑî‰ğ÷â‰Ğ‰ÖÁèÁîkÀ6)Á‰Ê‹D$¹6   ÷ñ‰Ñ‰òƒùvƒÀ‰C‰…Ğ   ‹D$‰…  kK6‹·Ü   Áô  ùÿÿç‡.  ùŸ  †²  ‰ÈºÓMb÷â¸÷ÚK‰ÑÁéiÑè  ƒÂÑê÷âÁê‰  ‹D$‰S‰•à   ‰C(‰…ì   ‹‡Ü      ‹€æû‰‹‡Ü      ‹ƒÊ‰¶Cˆ…Ü   ‰…  ¶C	ˆ…Í   ¶Cˆ…Ì   „À…«   ‹$èüÿÿÿ1ÀƒÄ$[^_]Ã´&    fƒÄ$‰è[^_]érñÿÿfÇD$÷J ¾ÿÿ  ÇD$ÿÿ  ÇD$  üÇD$Ÿ  éŒşÿÿ¶    ÇD$÷J ¾3   ÇD$  üÇD$ Ÿ  ÇD$ÿÿ  ÇD$ÿÿ  é“üÿÿ¶    º«„ ¹ÿÿ  éòşÿÿ´&    fµ   ‰ğèüÿÿÿÇ…      Ç…      ‰Â‰ğèüÿÿÿ€{	 t‹‡Ü     ‹ƒÊ‰€{ ¸@  DD$‰Ã‹‡Ü      ‹€   1Ø%¿şÿÿ1Ø‰éáşÿÿt& 1Ò1Ééhşÿÿ´&    ÇD$”   ¾   ÇD$   ÇD$0ø?ÇD$    é„ıÿÿ¶    ÇD$”   ¾Ì¿ ÇD$0ø?ÇD$     ÇD$   ÇD$   é‹ûÿÿ¶    ¸Úÿÿÿéeşÿÿ¶    èüÿÿÿUWVS‰Ãƒì$‹€”   ‹€À   ‹¸Ü   ‹·   ‹¸Ü   ‹  ‰L$‹¸Ü   ‹  ‰L$‹¸Ü   ‹¿  ‰|$‹¸Ü   ‹¿  ‰|$‹¸Ü   ‹¿  ‰|$‹€Ü   ‹€  ƒÃp‰D$½f  ¿^  Sh  èüÿÿÿ÷Æ   XZ¸W  º[  DÂPSh3  èüÿÿÿ‰ğ‰éƒÄƒàEÏ‰$QSh<3  èüÿÿÿ‰êƒÄ÷Æ@   E×RSh46  èüÿÿÿ‰ñƒáÿ4   ShĞ7  èüÿÿÿºz  ¸o  ƒÄ÷Æ @  DÂPShl3  èüÿÿÿº”  ¸Š  ƒÄ÷Æ   DÂPShœ3  èüÿÿÿº¹  ¸©  ·|$ƒÄ÷Æ    DÂPShÌ3  èüÿÿÿ‹L$ƒÄ…É„(s  écv  t& èüÿÿÿU¹h  ºÀ  WV‰ÆSƒì0d¡    ‰D$,1À¡$   èüÿÿÿ…À„Ø  ¸  º   ‰Ã¹   Ç€       ‰øhÀ  èüÿÿÿZ…À…™  ‰³À   º`  ‰Øèüÿÿÿ†¼  ‰›”   PCph7  j PèüÿÿÿF‰ÚÇƒ      èüÿÿÿ‰ÅƒÄ…À…	  ‹†Ü   ‰¨  ¹¨  ƒğ   ºA  èüÿÿÿ‰è¹
   t$Ç$   ‰÷ó«‹CdÆD$
ÇD$ ÇD$ ¸ˆ  ÇD$$ˆ  ‹@ÆD$ÇD$ Œ  …Àt(‹H…Ét!¡   ‰â…À„¨   ‹x…ÿ„   ‰ØèüÿÿÿƒP  ¹¨  ºX  ‰÷èüÿÿÿ1À¹
   ó«‹CdÇD$   Ç$   ‹@ÆD$
ÆD$ÇD$ Œ  …Àt2‹H…Ét+¡   ‰â…ÀtI‹p…ötB‰Øèüÿÿÿë´&    f‰øèüÿÿÿ‹D$,d+    uAƒÄ0‰è[^_]Ãf‰Øèüÿÿÿé^ÿÿÿt& ‰ØèüÿÿÿëÎ´&    ‰Ø½ôÿÿÿèüÿÿÿë¹½ôÿÿÿë²èüÿÿÿ¶    èüÿÿÿVº   Sèüÿÿÿ…Àt<‰Ã‹€”   èìÿÿ‹ƒ”   è“ëÿÿ‹³”   ‰Øèüÿÿÿ†  èüÿÿÿ‰ğèüÿÿÿ1À[^Ã¸íÿÿÿëöffffèüÿÿÿS‰ÃfÁÂ¹   ƒìd¡    ‰D$1À¸   f‰T$‰âf‰D$D$‰D$‰ØÇ$D   èüÿÿÿƒø…Jw  ¹   ‰â‰ØÇD$  èüÿÿÿƒø…4w  ¶D$‹T$d+    uƒÄ[Ãèüÿÿÿt& èüÿÿÿVS˜
  ‰Øƒìd‹    ‰T$1Òº  èGÿÿÿ¹   º  ƒàïf‰L$t$¹   f‰T$‰âˆD$‰ØÇ$D   ‰t$èüÿÿÿƒø…¢w  ¹   ¸ê   ‰â‰t$f‰L$¹   f‰D$‰ØÇ$D   ÇD$³èüÿÿÿƒø…Œw  ¹   ¸	   ‰â‰t$f‰L$¹   f‰D$‰ØÇ$D   ÇD$èüÿÿÿƒø…vw  º  ‰Øè~şÿÿ¹   ƒÈf‰L$‰â¹   ‰t$¾  ˆD$‰ØÇ$D   f‰t$èüÿÿÿƒø…`w  ‹D$d+    uƒÄ[^Ãèüÿÿÿffffèüÿÿÿ1ÀÃ´&    èüÿÿÿU¶ÒWV‰ÆS‰ËƒìPd‹    ‰L$L1É‹D$hf‰$1Éf‰L$‹|$dPˆ\$f‰T$T$‰T$ƒøs4…Àup¹   ‰â‰ğèüÿÿÿƒøu}1À‹T$Ld+    …‚   ƒÄP[^_]Ãf‹l$L$)Í‰T$‹Tü)ï‰T	è‰úƒàüƒørªƒàü1ÿ‹,:‰,9ƒÇ9Çróë–¶    ¶ˆL$¨t…·Tşf‰Tévÿÿÿ¶    ‹°  …Ò…¸w  ¸ÿÿÿÿémÿÿÿèüÿÿÿv èüÿÿÿWVSƒì‹p,d‹    ‰L$1ÉÆD$€…Òu|¸¬Ä  èüÿÿÿ¶VL‹FH1Éj|$Wè½şÿÿY‰ÃX…Ûu5¸¬Ä  èüÿÿÿ¶VL‹FH1ÉÆD$ jWè•şÿÿ¸è  èüÿÿÿ1Ò‰ğèüÿÿÿXZ‹D$d+    uƒÄ‰Ø[^_Ã´&    v »êÿÿÿëÙèüÿÿÿt& èüÿÿÿV‰ÎS‰Óƒì d‹    ‰T$1Ò¶ÓL$‰t$f‰T$f‰T$‹T$,‰L$¹   f‰L$¹   f‰T$T$ÆD$ ÇD$   èüÿÿÿƒøu)¡°  …À…Üw  1À‹T$d+    u"ƒÄ [^Ã´&    ‹°  …Ò…x  ¸ÿÿÿÿëÑèüÿÿÿ¶    èüÿÿÿS‰Ã1Òƒì(d¡    ‰D$$1ÀC¬èüÿÿÿ¡    9Cx‹Cü…Àt‹D$$d+    uOƒÄ([Ãt& ¶Sø‹Côj!L$èöşÿÿY‰Â¡    è  ‰C…ÒuÄ¡°  …À….x  ¶D$ÁàÁøƒà‰Cüë¤èüÿÿÿ´&    ¶    èüÿÿÿUWV‰ÆSƒì‹D$,‰L$‰$‹D$4‰D$d¡    ‰D$‹Fd‹…Ò…Ş  ¶ù9~lte¶VL‹FHjL$è^şÿÿ‰ÂX…Òt‹D$d+    …¶  ƒÄ‰Ğ[^_]Ãt& ¶D$¶VLƒàó
D$ˆD$‹FHjL$Q1ÉècüÿÿY]‰Â…Àu´‰~l«¤  ‰èèüÿÿÿ‹ƒÜ   º   ‰$  ‹ƒÜ      ¶D$0€ÌÇ‰‹ƒÜ      ‹€æû‰‹ƒÜ      ‹D$0¶Ä€ÌË‰‹ƒÜ      ‹€æ÷‰€<$ …Ş   ‹ƒÜ      ¶D$€ÌÃ‰‹ƒÜ      ‹¿ÿıÿÿ€~L@¹ÿşÿÿEÏ!Ê‰‹ƒÜ      ‹
€<$À% Àÿÿ-@  !È‰‹=    ƒÇë´&    f‹    9×x¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄuÚ‹“Ü   Š   ‹’   €ÎÃ‰¶Ğ‰è‰T$èüÿÿÿ¡°  ‹T$…À…Xx  1À€<$ DĞé_şÿÿ¶    ‹ƒÜ   ºÿ  ‰$  é"ÿÿÿºêÿÿÿé9şÿÿèüÿÿÿ´&    t& èüÿÿÿS¶\$¶É‹@,SQ¹   j è±ıÿÿƒÄ[Ã´&    t& èüÿÿÿ¶É‹@,j Q¹   jè†ıÿÿƒÄÃfèüÿÿÿS¶\$‹@,SQ1Éj ègıÿÿƒÄ[Ãfèüÿÿÿ‹@,j Q1ÉjèLıÿÿƒÄÃ´&    èüÿÿÿWVS‰ÓƒìT‹p,d‹    ‰T$P1Ò…Û…‹   ¶VL‹FHjL$è¹ûÿÿ¶VL¶D$¹   ‹~HÇD$
   ƒÈ`f‰T$T$ˆD$‰T$T$ˆD$‰øÆD$ èüÿÿÿYƒøu‰Ø‹T$Pd+    u*ƒÄT[^_Ãt& ‹°  …Ò…Åx  »ÿÿÿÿëÑv ¸êÿÿÿëÉèüÿÿÿt& èüÿÿÿVSƒìP‹p,d‹    ‰\$L1Û…Ò…®   …Éur»   ¸   8^htF‹°  …É…çx  ¶VL‹FH¹   ˆ\$ÇD$   f‰$T$‰T$‰âÆD$èüÿÿÿƒøuHˆ^h‹FP‹T$Ld+    uLƒÄP[^Ã´&    ƒ=¬  ÛƒãüƒÃƒ=¬  ÀƒàüƒÀéuÿÿÿt& ‹°  …Òt±é	y  ¸êÿÿÿë§èüÿÿÿt& èüÿÿÿ‹°  V‰ÆS‰Ó…É…ey  1À÷Ã  €t÷Ã  € uEã   u¸   [^Ãt& ‹8  ‹    ¸    ƒÁTèüÿÿÿ¡°  …ÀtÑé(y  ´&    f‹†ô  ‹    HT¸    èüÿÿÿ‹°  …Ò…Iy  ã   t”ëŸ´&    ´&    èüÿÿÿUW¿31  V‰ÆS»   ƒì(‹-°  d¡    ‰D$$1Àƒ=¬  ÇD$    ÀÇD$ D  ƒàüÇD$
    ƒÀÇD$  D ÇD$    ÇD$   f‰\$ÆD$ ˆD$ÇD$  f‰|$"…í…ºy  ¡   ¹t   ºÀ  èüÿÿÿ‰Ã…À„y  ‰†  ‹Fƒø„T  ƒø…  ÆCLAºA   ‹ƒ=¬  Ç    ÇC    ¸ü   À1ÉÇC    ƒàü‰{HƒÀÇC    ˆChÇC    ÇC    ÇC     ÇC$    ÇC(    ‰[,‰sdj"D$P‰øè±öÿÿ¶SL¹   ‰Ç‹CHjl$&Uè—öÿÿ¶SL¹   	Ç‹CHjUèöÿÿƒÄ	Çuzj1ÉF(‰ÚèüÿÿÿY‰Ç…ÀueCXÇCTàÿÿÿKT‹    ‰CX‰C\¸    ÇC` º  èüÿÿÿ‹°  …Ò…Öy  ‹D$$d+    uNƒÄ(‰ø[^_]Ã¶    ¶SLéåşÿÿ´&    ¡°  …À…y  ‰Øèüÿÿÿë½´&    v ÆCL@º@   é°şÿÿèüÿÿÿƒ=°   ¿ôÿÿÿtÌéy  ´&    v èüÿÿÿ…Àt/S‹˜  …Ût‹C,…Àt‰Øèüÿÿÿ‰Ø[éüÿÿÿ¶    [Ã¶    Ãfffèüÿÿÿƒì d‹    ‰L$1É¹   ˆT$‰âf‰L$L$‰L$¹   f‰L$L$‰L$¹   Ç$P   ÇD$P  ÆD$ èüÿÿÿƒø…òy  ¶D$‹T$d+    uƒÄ Ãèüÿÿÿ´&    ´&    èüÿÿÿSƒìd‹    ‰\$1Û»   ˆT$‰âf‰\$\$ˆL$¹   Ç$P   ‰\$èüÿÿÿƒø…z  »
   ´&    ¸X‰A èüÿÿÿƒëuñ1À‹T$d+    uƒÄ[ÃèüÿÿÿèüÿÿÿW‰ÇV‰Öº?   S»@   èüÿÿÿˆFf‰Ú‰øèüÿÿÿˆDÀƒÃƒûFuëv ‰Ú‰øèüÿÿÿˆDÀƒÃƒûLuë[^_ÃfffffffèüÿÿÿW¿   V1öS‰Ã´&    ¹   º   ‰Øöèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ1Éº   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ	Æ‰ø,‰ÇuŒ‰ğ[^_Ã´&    ´&    èüÿÿÿW¿   V‰ÖS‰ÃëEt& èüÿÿÿ¸R÷ öèüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ‰ø,‰ÇtX¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿº   ‰Ø¹   èüÿÿÿ‰ğº   „À‰Ø‰wÿÿÿèüÿÿÿérÿÿÿt& [^_Ã´&    t& èüÿÿÿW¿8  VSƒìd‹    ‰L$1Éf‰<$ÆD$ƒú„I  ƒú„p  …Ò„P  ‹€$  L$º   ‹@ ‹‰à´&    v PƒÀ9Áuö÷Ú¹   ‰ØˆT$º   èüÿÿÿº   ‰Øèüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿºà   ‰Øè2şÿÿ¸   èüÿÿÿº   |$t$ë´&    f¶ƒÇ‰Øèşÿÿ9şuï¹   º   ‰Ø1öèüÿÿÿº   ‰Øèüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿv ¸   ƒÆèüÿÿÿ1Éº   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ„Àt‰ğ<vÏ‰ğ<„z  ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿºá   ‰ØèRıÿÿ¸   èüÿÿÿ‰Øè¡üÿÿº   8Ğ‰×Fø1öv ‰Øè‰üÿÿ‰ğ‰ùƒÆ8Èuî¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¹   º   ‰Øèüÿÿÿ‰Øº   èüÿÿÿ1À‹T$d+    uNƒÄ[^_Ã¶    »  ÆD$f‰\$é´ıÿÿt& ¾  ÆD$ f‰t$éœıÿÿt& 1ÉÆD$ f‰L$é‡ıÿÿèüÿÿÿfèüÿÿÿ1ÀÃ´&    èüÿÿÿ‹P‹H|‹R‹AP·Òƒè!Ğ¯ALÃfèüÿÿÿ‹@|4  ‰Ğéüÿÿÿ´&    t& èüÿÿÿUWV‰ÖSƒì‹X{‰øèüÿÿÿ…ö„=  ½êÿÿÿƒş…  ‹3‹k ‹†d  ‰D$  ‰$¡´  ƒÀƒø‡7z  ‹†Ü   @  ‹ƒâî‰ÿu‹M ‹T$‹èüÿÿÿ‹†Ü   ‹U ‰P  º   ‹†Ü   ‰X  º   ‹†Ü   ‰0  ¡´  ÇC    ZƒÀƒø‡  ‹†Ü   º ‰P  ºÿÿÿÿ‹†Ü   ‰T  ‹†Ü     ‹*‹‹ˆä   ‰ÈƒÈ÷Ğ!è	ÈƒÈ‰‹†Ü      ‹ƒÊ ‰‹†Ü   @  ‹€@  ƒÈ‰‹5´  …ö…‹   1í‰øèüÿÿÿ‰èƒÄ[^_]Ã´&    ¡´  ‹3ƒÀƒø‡Uz  ‹†Ü   @  ‹ƒâî‰‹†Ü     ‹ƒâ÷‰‹†Ü   P  ‹âşïşÿ‰¡´  …Àt‹–d  ‹1íÂ  èüÿÿÿéxÿÿÿ‹$‹1íèüÿÿÿéfÿÿÿ‹D$‹S‹M ‹€  ¯ÊƒÀƒàü†Ü   ‹ ÑèQRPÿu ‹¼  Ph<;  èüÿÿÿƒÄé¢şÿÿ´&    t& èüÿÿÿUWVS‹h‹X|…í„mz  ‰Ç¹   1Ò‰Øèüÿÿÿ‰Æ…Àˆ¯   ‰}$1À»ø   ¹   ó«Æƒ)  1öÇƒø    Çƒü      Çƒ       Çƒ  €   Çƒ  €»  Çƒ  €»  Çƒ     Çƒ     Çƒ    Çƒ!    ‹E ‹€d  ‹€  =   tÁèƒàø‰ƒ  ‰ƒ   [‰ğ^_]Ã¡´  ƒÀƒøvëé‚z  ´&    ´&    èüÿÿÿ‹P…Ò„°   VS‰Ã¡´  ƒÀƒø‡œz  ‹s ‹N …Éua‹FèüÿÿÿÇF    ‹FèüÿÿÿÇF    ‹S ‹C‹J‹rj ÿrˆ   ÿr‰òèüÿÿÿ‹C èüÿÿÿÇC     ƒÄÇC    [^Ã´&    ‹V‹N$‹Cj jˆ   èüÿÿÿÇF     X‹s Zéwÿÿÿ´&    ´&    ´&    èüÿÿÿS‰Ã‹@|‹€4  …Àt‹Cèÿÿÿ‹C|Ç€4      1À[ÃfèüÿÿÿUW‰ÇVS‰Óƒì‹p‹@|‹€4  …Àt‰ğèÙşÿÿ‹G|Ç€4      ‹ƒL  ‰F‹“X  ¯Â‰V‰F…À„Ô  Bÿ…Ğ…Ñ  ¡   ¹(   ºÀ  èüÿÿÿ‰Ã…À„µ  ‹F‰‹F‰^ ˆÿ  ‰Íá ğÿÿ‰È‰$Áíèüÿÿÿ‹$…À‰C„ˆ  ‹´  ƒÂƒú‡µz  1Òèüÿÿÿ‰k$Dm Áàèüÿÿÿ‰C…À„  ‹S$èüÿÿÿ‹C$…ÀtU1íë#´&    f	ÂÇA    ƒÅ‰ÇA   9k$v.‰èÁàCèüÿÿÿ…À„±   ‹KTm Ñ‹ƒâ¨t¾t& ‹n ‹F‹M$‹Uj ˆ   jèüÿÿÿ‰E Y]…À„Üz  ‹K‹FjSÿvÿvèüÿÿÿ‰ÂƒÄ…Àxu‹CÇ   q‹C‹S‰P‹CÇ@    ‹V ‹G|‹R‰4  ‹G|‹V‰@  ‹G|1ÒÇ€8      Ç€<      ë7‹CèüÿÿÿÇC    ‹CèüÿÿÿÇC    ºôÿÿÿ‰Ø‰$èüÿÿÿÇF     ‹$ƒÄ‰Ğ[^_]Ã´&    ¶    ºôÿÿÿëŞ¡´  ºôÿÿÿƒÀƒøv¸é÷z  èüÿÿÿ1ÀÃ´&    èüÿÿÿUWVS…Êts‹¨0  ‰Æ‰Ó‹€Ü   ‰T  ÷Â   …{  öÇuƒã¿   uR[‰ø^_]Ãv ¡´  ƒÀƒø‡V{  ‹†Ü   º   ‰0  ¿   [^‰ø_]Ã´&    v 1ÿ[^‰ø_]Ã´&    ‹†Ü   ‹€   ‰E‹E$èüÿÿÿ‰ø[^_]ÃfèüÿÿÿVS‰Ãƒìd¡    ‰D$1À¡¸  …À…+  ‹ƒd  ‹€  …À„p{  ‹ƒÌ   T1Éºÿÿÿÿj(h    ˆ   èüÿÿÿƒÄ…Àˆ¡{  ‹$T$1É‹°ˆ  ‰‹ƒÌ   ÇF    ‰F‹$‰FRº  jj èüÿÿÿƒÄ…Àˆ°   ‹D$¹   º  Ã¼  ‰°¸  ƒÀXèüÿÿÿ‹D$¹   º   èüÿÿÿ‹$¹   º)  ƒÀèüÿÿÿh1  ‹D$ƒÀ$Pèüÿÿÿ‹D$SP$ƒÀDRhB  Pèüÿÿÿ‹D$èüÿÿÿƒÄ…Àx-ƒ=´  ÿ…ˆ{  ‹D$d+    u ƒÄ‰ğ[^Ã´&    1öëà‹$èüÿÿÿé¡{  èüÿÿÿ´&    v èüÿÿÿ‹€0  ‹@éüÿÿÿˆ«©ÿÿƒù‡  ºÓê€â„  ÿt$,Sh8   èüÿÿÿƒÄ‹D$Dd+    u=ƒÄH[Ãˆ[Íşÿº @Óê€â„  ëÄˆıæşÿº Óê€â„  ë¬ˆªÿÿë”èüÿÿÿèüÿÿÿVS˜¼  ‹€Ì   ƒx$ uSSSSShd   èüÿÿÿƒÄëSSSShD  èüÿÿÿƒÄS1öh  èüÿÿÿXZiÆ  ÿ°    VFSh\  èüÿÿÿƒÄƒş?uß[^ÃQ¾êÿÿÿh   h€  èüÿÿÿƒÄém  €=    Yt}h   h¨  èüÿÿÿ‹ƒ¸  _Y‰Âé  ¢   ¶   ¸"   ¾   €úEğVRh  èüÿÿÿ‹‹Ì   ‰òD$Áˆ   èüÿÿÿƒÄ…ÀuP‹$D$èüÿÿÿ‹$èüÿÿÿéB  ÿ5#   iƒ¸    ÿ°    ƒ¼  Phè  èüÿÿÿ‹ƒ¸  ƒÄ‰Âé  Vh<  èüÿÿÿ[^ë­‰$ÿsSh4  h  èüÿÿÿ‹T$ƒÄé‚  ‰$ÿsSh4  hP  èüÿÿÿ‹T$ƒÄé‚  hŒ   hŒ   hµ  èüÿÿÿƒÄé=   ‹†è  ÿ³  ÿ0ÿpÿ¶ğ  ÿ¶ì  ÿsSh  h(  èüÿÿÿ1ÀƒÄ$é¶!  h  h  hÄ  èüÿÿÿ‹ƒ€  ƒÄé`"  h  h  h  èüÿÿÿ‹ƒ€  ƒÄé#  ht   ht   hµ  èüÿÿÿƒÄéj$  QRPht   ht   hx  èüÿÿÿƒÄéî$  ¸0  èüÿÿÿ¸(  èüÿÿÿÿv³ˆıÿÿWÿ³ŒıÿÿVh¬  h°  èüÿÿÿƒÄé'  hÌ   hÌ   hµ  èüÿÿÿƒÄéc'  ¸0  èüÿÿÿ¸(  èüÿÿÿQhä   hä   hä  èüÿÿÿƒÄéI(  h`   h`   hµ  èüÿÿÿƒÄé{(  h  h  hµ  èüÿÿÿƒÄé)  Ph  h  h  èüÿÿÿƒÄéÕ)  hœ   hœ   hµ  èüÿÿÿƒÄéÔ+  h€  hè  èüÿÿÿXZé·,  R‰T$hÔ  hÔ  hd  èüÿÿÿ‹T$ƒÄé-  h¼  1öhŒ  èüÿÿÿXZƒ=x   uƒÆƒş$uïéœ-  ‰ò‰Øèüÿÿÿ¶ÀPVh¼  h°  èüÿÿÿƒÄëÑi€¸    kÒ„    ÿpDÿp@ÿp<ÿp8ÿp0Vhì  hì  hØ  èüÿÿÿƒÄ$é÷-  R‰T$h¨  h¨  hä  èüÿÿÿ‹T$ƒÄéä/  Rhø   hø   hÿ  èüÿÿÿ‹ƒÄéd0  hl  hl  hµ  èüÿÿÿƒÄéº0  ¸0  èüÿÿÿ¸(  èüÿÿÿh¬   h¬   hµ  èüÿÿÿƒÄéí1  Ph  h  h  èüÿÿÿƒÄé%2  Sh   h   hÿ  èüÿÿÿƒÄƒû†S2  ƒ=x   „Œ2  h   h   h(  èüÿÿÿ¸êÿÿÿƒÄé2  ÿt$ÿt$ÿ3ÿt$hŒ  hŒ  hL  èüÿÿÿƒÄé?4  hŒ  hˆ  èüÿÿÿ[^éù4  h   hx  h\	  èüÿÿÿƒÄé¬6  ƒ=x  vh   hx  h´  èüÿÿÿƒÄ†¼  Phè  èüÿÿÿ‹–d  ‰ğèüÿÿÿXZé6  h   hx  h	  èüÿÿÿƒÄé–6  h\  h\  hµ  èüÿÿÿƒÄéR7  ‹“à  ‹‚  …À„Ü   PVhÄ	  èüÿÿÿ¹Q  º@  ‰Øè\(  ‰ƒä  ƒÄ‰¨¸  ‹ƒä  Ç€ä    ‹ƒ¸  ƒø4tƒè8ƒø‡”   ‹ƒä  ˆä      ‹ƒä  ‹“Œ  ‹• J  ‹à   ÿ2º   jèüÿÿÿY_…Ày‰$Vhğ	  èüÿÿÿXZ‹$é¤:  hD  hD  hµ  èüÿÿÿƒÄé8  ‰$Vh˜	  èüÿÿÿ]X‹$é¤:  ‹‚è   éÿÿÿƒ»€  ‹ƒä  „oÿÿÿˆä      ‹ƒä  éZÿÿÿ‹“ä  ‹‚  …Àt#PVh
  èüÿÿÿ‰Øèüÿÿÿ1Ò‰ƒ0  ƒÄé±:  ‹‚è   ëÕÿsSh¸  hD
  èüÿÿÿƒÄék>  ÿsSh¸  h|
  èüÿÿÿƒÄék>  h   h   hĞ  èüÿÿÿƒÄé*@  hÈ  hÈ  h¸
  èüÿÿÿƒÄégA  èüÿÿÿUW‰ÇÁïVS‰Ã‹½   …Àu¸é  P¾   ½   Shñ  èüÿÿÿƒÄ‰è‰ñÓà…Øtÿ4µ  hÿ  èüÿÿÿXZNƒşuİãÿ  Sh  èüÿÿÿ‹½€  ZY…Àu¸   [^_]ÃPÿt$ÿ³ŒıÿÿUhÄ  h  èüÿÿÿƒÄé*F  Pÿt$ÿ³ŒıÿÿUhÄ  h  èüÿÿÿƒÄé*F  ÿ2hx  hx  hL  èüÿÿÿƒÄédK  ‰$ÿ2hx  hx  hx  èüÿÿÿ‹L$ƒÄéTI  ‰$‹L$‹AèPVhx  hx  hÔ  èüÿÿÿ1Ò‹L$ƒÄéÙI  ÿt$ÿt$ÿ7ÿ³h  hx  hü  èüÿÿÿƒÄéBK  èüÿÿÿU1íW‰ÇV°¼  S‰Óƒìÿ2Vh8  èüÿÿÿƒÄ‹Cèƒàü‡Ü   ‹ PƒÅÿµ<  Vhh  èüÿÿÿƒÄƒı8uÓ1í‹CD¨8ƒàü‡Ü   ‹‰$UEVh  èüÿÿÿ‹D$èşÿÿƒÄƒıuÍ‰4$‰ù1í‰ß‹G­    ‰L$ØƒàüÜ   ‹0U_S»   ÿt$h  èüÿÿÿ‰ğuèÌıÿÿ‰Â‰l$‹l$ƒÄ‡×9ûs1‹B‰T$°ƒàü…Ü   ‹ SCPVFÿt$h´  èüÿÿÿ‹T$ƒÄëË‰é‹l$‡úÕƒı†lÿÿÿ‰û‹4$‰Ï‹C‹SÂRPVhà  èüÿÿÿ‹CP`RPVh  èüÿÿÿ‹Cƒàü‡Ü   ‹ ƒÄ PVh0  èüÿÿÿ‹Cƒàü‡Ü   ‹ PVhT  èüÿÿÿ‹C ƒàü‡Ü   ‹ PVhx  èüÿÿÿ‹C$ƒàü‡Ü   ‹ ƒÄ$PVhœ  èüÿÿÿƒÄ[^_]Ã÷Ã   t ƒ=„  vh   h¨  hÀ  èüÿÿÿƒÄ÷Ã   t$ƒ=„  †Ç   h   h¨  hğ  èüÿÿÿƒÄ‰İå   öÇt ƒ=„  v;h   h¨  h   èüÿÿÿƒÄ…ít ƒ=„  vh   h¨  hP  èüÿÿÿƒÄ‡¼  Ph€  èüÿÿÿ‹†À   ƒàü‡Ü   ‹‹–ô   ÷Ò!Ê‰kV,‰ø—d  èüÿÿÿXZé)L  jh¨  h¨  èüÿÿÿƒÄéùK  ‰İå   öÇu”ékÿÿÿhd  hd  h”  èüÿÿÿƒÄé‹Q  kV,‰Ø“d  èüÿÿÿ‹¼  ‰$ÿ·  ÿ·ˆ  Ph   èüÿÿÿ1À‰\$ƒÄ‰t$‰Æ‹‡„  Áè9Æƒ¹   V»   ÿt$h.  èüÿÿÿµ    ‰D$‹‡ˆ  ‹°èûÿÿVƒÄ‰t$‰Ö‰l$‰ı‰Ç‹…ˆ  9ß†e  SƒÃÿ4°VƒÆÿt$h0  èüÿÿÿƒÄëÔÿ³ô  ÿ³ğ  ÿ³ì  hd  hd  hÔ  èüÿÿÿƒÄénO  PRhd  h\  èüÿÿÿ¸êÿÿÿƒÄéiR  ‹\$‹t$é½O  ‹.hL  hL  h˜  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü      ‹ PhL  hL  hÄ  èüÿÿÿƒÄƒ=„   „CR  ‰èè¬A  PhL  hL  hü  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   P  ‹ PhL  hL  h4  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   @  ‹ PhL  hL  hl  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   `  ‹ PhL  hL  h¤  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   @ ‹ PhL  hL  hÜ  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   L  ‹ PhL  hL  h  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   ,  ‹ PhL  hL  hL  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   d  ‹ PhL  hL  h„  èüÿÿÿƒÄƒ=„   „CR  ‹†¸   ƒàü…Ü   ‹ Pÿ¶¸   hL  hL  h¼  èüÿÿÿƒÄƒ=„   „CR  ‹†¼   ƒàü…Ü   ‹ Pÿ¶¼   hL  hL  hğ  èüÿÿÿƒÄƒ=„   „CR  ‹†À   ƒàü…Ü   ‹ Pÿ¶À   hL  hL  h$  èüÿÿÿ‹†ì   ‹„  ƒÄ…À…~  …Ò„CR  ‹†Ä   ƒàü…Ü   ‹ Pÿ¶Ä   hL  hL  hŒ  èüÿÿÿƒÄƒ=„   „CR  ‹†È   ƒàü…Ü   ‹ Pÿ¶È   hL  hL  hÀ  èüÿÿÿƒÄƒ=„   „CR  ‹†Ì   ƒàü…Ü   ‹ Pÿ¶Ì   hL  hL  hô  èüÿÿÿƒÄƒ=„   „CR  ‹†Ğ   ƒàü…Ü   ‹ Pÿ¶Ğ   hL  hL  h(  èüÿÿÿƒÄƒ=„   „CR  ‹†Ô   ƒàü…Ü   ‹ Pÿ¶Ô   hL  hL  h\  èüÿÿÿƒÄƒ=„   „CR  ‹†Ø   ƒàü…Ü   ‹ Pÿ¶Ø   hL  hL  h  èüÿÿÿƒÄƒ=„   „CR  ‹†Ü   ƒàü…Ü   ‹ Pÿ¶Ü   hL  hL  hÄ  èüÿÿÿƒÄƒ=„   „CR  ‹†à   ƒàü…Ü   ‹ Pÿ¶à   hL  hL  hø  èüÿÿÿƒÄƒ=„   „CR  ‹†ä   ƒàü…Ü   ‹ Pÿ¶ä   hL  hL  h,  èüÿÿÿƒÄƒ=„   „CR  ‹†è   ƒàü…Ü   ‹ Pÿ¶è   hL  hL  h`  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü     ‹ PhL  hL  h”  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   8  ‹ PhL  hL  hÈ  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   <  ‹ PhL  hL  hü  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   H  ‹ PhL  hL  h0  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   ‹€L  PhL  hL  hd  èüÿÿÿƒÄéCR  ‹\$‰ù‹t$‰ï‹l$<   pu‹\$‹t$é½O  Îéùÿÿ…Ò„CR  ƒàü…Ü   ‹ Pÿ¶ì   hL  hL  hX  èüÿÿÿ‹„  ƒÄéFüÿÿ‰$Sh8  h8  hF  èüÿÿÿ‹T$ƒÄéòR  ‰$h$  ÿsSh$  h¼  èüÿÿÿ‹T$ƒÄéT  ‰$h$  ÿsSh$  hè  èüÿÿÿ‹T$ƒÄéT  h  h  h]  èüÿÿÿ‹7ƒÄƒ=„   „zT  hø  hø  h]  èüÿÿÿƒÄézT  ÿvhr  ÿ¶ŒıÿÿUhä  h  èüÿÿÿƒÄéxU  ƒ¼  VPh€  èüÿÿÿƒÄ÷Æ   tƒ=„   thÔ  h¼  èüÿÿÿ]X÷Æ   tƒ=„   thÔ  hè  èüÿÿÿY_÷Æ   tƒ=„   „ô   hÔ  h  èüÿÿÿXZ‰÷ç    ÷Æ   tƒ=„   t/hÔ  hD  èüÿÿÿY]…ÿtƒ=„   thÔ  hl  èüÿÿÿXZ‰÷ç   ÷Æ   tƒ=„   t/hÔ  h˜  èüÿÿÿ]X…ÿtƒ=„   thÔ  hÀ  èüÿÿÿY_‹ƒœ  ƒàüƒÜ   ‹‹“Ğ  ÷Ò!Ê‰k“ä  ,‰Ø“d  èüÿÿÿ‰Øèüÿÿÿé}V  hÔ  hì  èüÿÿÿXZé\V  ‰÷ç    ÷Æ   …Qÿÿÿé.ÿÿÿ‰L$h   h¸  h   èüÿÿÿ‹L$$ƒÄéÿX  ‰L$jh¸  hÄ  èüÿÿÿ‹L$$ƒÄé°X  ‰L$h  € h¸  h  èüÿÿÿ‹L$$ƒÄéÑX  ‰L$h   h¸  h  èüÿÿÿ‹L$$ƒÄévX  ‰L$h   h¸  h¼  èüÿÿÿ‹L$$ƒÄéLX  ‰L$h   h¸  hè  èüÿÿÿ‹L$$ƒÄéaX  ‰L$h   h¸  hd  èüÿÿÿ‹L$$ƒÄé"X  ‰L$h   h¸  h  èüÿÿÿ‹L$$ƒÄé7X  ‰L$jh¸  h@  èüÿÿÿ‹L$$ƒÄé‚X  ‰L$h   h¸  ht  èüÿÿÿ‹L$$ƒÄéì[  ‰L$h   h¸  hH  èüÿÿÿ‹L$$ƒÄéæX  ‰L$jh¸  hğ  èüÿÿÿ‹L$$ƒÄéÄ[  ‰L$jh¸  h˜  èüÿÿÿ‹L$$ƒÄéu[  ‰L$jh¸  hl  èüÿÿÿ‹L$$ƒÄé—X  VUh¸  h  èüÿÿÿƒÄéZ  ‰T$0‰L$,VUh¸  h  èüÿÿÿ‹L$<‹T$@ƒÄƒ=„  †öW  Rÿt$ÿt$h¸  hP  èüÿÿÿ‹L$@ƒÄƒ=„  †öW  ‰L$ÿt$ ÿt$ÿt$h¸  h”  èüÿÿÿ‹L$ ƒÄƒ=„  †öW  ÿt$$ÿt$Q‰L$h¸  hÜ  èüÿÿÿ‹L$ ƒÄƒ=„  †öW  ÿt$(ÿt$ ÿt$h¸  h   èüÿÿÿ‹L$ ƒÄéöW  †¼  PhÌ  èüÿÿÿYXé¬\  Æ¼  Vhy  èüÿÿÿXZéÒ\  †¼  Phü  èüÿÿÿYXé.]  Æ¼  Vhy  èüÿÿÿXZéV]  †¼  Phy  èüÿÿÿX1ÀZé]  †¼  Ph4  èüÿÿÿYXé°]  †¼  Phl  èüÿÿÿXZéK^  jh€  h€  h×  èüÿÿÿƒÄé³d  Ph”  h”  hœ  èüÿÿÿƒÄéû`  hl  hl  h]  èüÿÿÿƒÄ»h  u  u‹ƒÜ    ‹ƒâï‰‰ØèLE  ‹ƒ¸  iĞ  ‹²   ‰³€  ¶Š    ˆ‹„  ˆL$‹Š$   ‰L$‰‹ˆ  ‹Š   Â    ƒ=„   ‰‹Œ  ¶R!ˆ“  tVÿt$¶D$PVh”  h”  h”  èüÿÿÿƒÄƒ=„   t%¶ƒ  Pÿ³Œ  h”  h”  hÜ  èüÿÿÿƒÄ‹ƒ¸  iÀ  ƒ¸   …(  ‰Øèüÿÿÿƒ=„   th\  h\  h]  èüÿÿÿƒÄ‰Øè¼F  ‹ƒÜ     ƒÈÿ‰‹‹Ü   ‰$  ‹‹Ü   ‰4  ‹‹Ü   ‰D  ‹‹Ü   ‰T  ‹‹Ü   ‰d  ‹ƒÜ   H  ‹â   €‰º P ‹ƒÜ   ‰L  ‰ØèLE  ¸d   èüÿÿÿ‹“d  ¹@  ‰Øj èüÿÿÿ‹ƒd  ¹€   j P,‰Øèüÿÿÿ‹ƒd  ¹ğ  j PX‰Øèüÿÿÿ‹ƒd  ¹€   j „   ‰Øèüÿÿÿ‹ƒd  ¹€   j °   ‰Øèüÿÿÿ‹ƒd  ¹ğ  j Ü   ‰Øèüÿÿÿ‹ƒd  ¹€   j   ‰Øèüÿÿÿ‹ƒd  ¹€   j 4  ‰Øèüÿÿÿ‹ƒd  ¹€   ƒÄ j `  ‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øè¬A  ‰ØèLE  ƒô   èüÿÿÿƒ|  èüÿÿÿƒ
  èüÿÿÿ‰Øèüÿÿÿ‹Cp´XC‰D$FL;D$„Ÿ   ‹Fd‹@…Àt‹…Òt‰ğèüÿÿÿ‹vLƒîLëÔjh€  h€  h×  èüÿÿÿƒÄéÿe  ƒ¸   „ËıÿÿéÍıÿÿ‹ƒÌ   ¾êÿÿÿÿ°T  ÿ°P  ‹|$WhÜ  èüÿÿÿ‹ƒÌ   ·P&R·@$PWh  èüÿÿÿƒ-€  ƒÄ éx_  ‰Øèüÿÿÿƒ»¸  7u1Éº   ‰Øèüÿÿÿ‹ƒ¸  iĞ  ƒº   u"‰Øèüÿÿÿ…Àyh”  h  èüÿÿÿY^‹ƒ¸  iĞ  ‹Š   Â    ƒù…ù  ‹B…Àt‰ƒì  ƒÜ  èüÿÿÿ…À‰ş  h”  h\  èüÿÿÿX‹ƒ¸  ZiÀ  ‹       ƒú…Ø  ‹@…Àt‰ƒ0  ƒ   èüÿÿÿ…Àyh”  hÌ  èüÿÿÿXZ‹ƒÜ   ‹  ¶Â€ú„Á  €âğuCƒø