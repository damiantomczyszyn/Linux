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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      �2     4     (            GNU  �       �          �����WV�ƉЉ�S���   �|$��  ��������   ����  	؉�  ���      � 1ۀ�_������	Ë��   ��   �؉�������E����   �� ��   ��  ��������   ��   ��[^_�f������S�Áü  ��Hd�    �D$D��������D$,=9� �L  =7� �   =Ո �6  =ӈ �   =Q� �   =O� �   =� �
  =� �   =Wu �   ��  =9K �   ��  =�� �   ��  =�� �   ��  =�N �   ��  =L �   ��  =o> �   ��  =e> �   �w  =�8 �   �f  =77 �   �U  =�6 �   �D  =�6 �   �3  =�6 �   �"  =�4 �   �  =�4 �   �   ='3 �   =�2 ��   =�2 �?   =�0 �   ��   =�, �   ��   =�, �   ��   =�, �   ��   =�, �   ��   =9+ �   w}=? �   wp=+ �   = w^= �W   = �   wF= �   w9=� �   w,=� �   w=a �   w=V  �����=�U  wPSh    ��������   �o   ��&    �v ������Q�UWVS����   �8�΅���   ���  ��w��ws[��^_]Ã���   ��,��   w��*u������   ���   ��  �
!ȉ��   ��    �X�A �������u��   ��  ��  	ŉ*[��^_]ú   ����`2 u���u@���p���������   뎍v ������u����P�����&    1�[��^_]Í�&    �h��t���(����C����P���������[^_]ø�����2�����&    �����S�Ë��  ���	  ww��	�#  �   ���'  �Q  ��t�  ����  �   �غ `  ������ `  ��������d   ������غ @  ������d   [�������&    ��)�  ��'�  ��!��  �G  ��&�r  ��'��  ���   (  ��������     ���   ��   ��*  �'  ��������     ���  ��   ��*  �'  ��������     ���������   ��*  �'  ��������     ���  ��   ��*  �'  ��������     ���������   ��*  �'  ��������     ���  ��   ��*  �'  ��������     ���  ��   ��*  �'  ��������     �����������     ���  ����     �����������   ��  ��    ��<   [�������&    f���vP����   �   �   ��������   ��������d   ������   ��������d   [�������$�[  �   �غ`  ������غ`  ������   ��������   ������غ   [������v ���/  v]��uT���     ���   ��   ��������     ������   ��������   ��  ��    �[�f���t���u��   ��  ��    �[Ív ���  �  ���h�����u����   �6   ��(  �   ���   ��$  ���      ������   ��������      �������   ��������   ��   ��   �����   [�������&    ����  ��   ��t	���������     ���   ��   ��������     ������   ��������   ��  ��    �[Ã�
��������     ���   ��   ��������     ������   ��������   ��  ��    �[Í�    ���a������n������A����   �غ �  ������ �  ��������d   ������ �  ��������d   ������   �غ   ������   ��������   ������   ��������   ������غ   ������   [������t& ��1��  ��   ��5w\��3wc���   �7   ��(  �   �غ  ������  ��������d   ������غ  [�����f���%������>���f���8���0����   �   ��������   ��������d   ������   �=�����-�c�����   ��1��������     ���  ����     ���������d   ��������     ���  ����     ���������7   ���   ��(  �   ���   ��$  ���   � �  ��   1ҋ��   ��  [Ã�*�������,�|���[Í�    ���     ���   ����     ���������d   ��������     �����7   ���   ��(  � P  ���   ��$  �   ���   �X�����&    ��   �غ   ������   ��������d   ������غ   [������t& ���     ���   ����     ������   ������b���f��   �   ��������   ��������d   ������   �I�����&    ���     ���   ����     ���������d   ��������     ���  �-�����&    �t& ������UWVS�Ã�0���  d�    �D$,1����w  vE����w�   �� �ud���  ��t71��D$,d+    ��  ��0��[^_]Ít& �����   1���vЃ�uh    1�j�����XZ빍�&    f���u��5   ��t��   ����������  ���?  ���  ���w����Pd����j����Z���_����    �   ����  �r����  �   �����1��2���f��   ��� ����   ����������  ����  ���  ��������Pd���������Z��������    �    ���x  �r���m  �   �����1������v ��������ƅ�������   ����������  ���  ���}����Pd����p����Z���e����    �   ����  �z����  �   ������:�����&    �v ��������ƅ������   ����������  �����  ��tA�Pd���t8�z��t1�    �    ���@  �j���5  �   ��������  �v ��������Ad�@����   �x����   �   �����   �h����   ����������  �D$ �D$
 �D$���_����Ad�@��tw�x��tp�   �����   �h����   ����������  �D$
�������Ad�@�������X�������   ���tN�x��tG�������������t& ��D$
뽍�&    f��D$ �D$
 �D$�[�������������  �2���������������   ��������  ���������������  �N����   ������h����   1�������W����   ������F���������>����������&    f���������  ��3wpS�Ã�v���   �� �u�u3[Ð��t
��t&��u�غ   �����ǃ�      [Í�&    f��غ   ������������ǃ�      [ÐÍ�&    ��&    ���������  ��3w��v���   ���� �u��u,Ív ��t
��t��u틐�  ��t�   ������t& ����  ��tʺ   ������t& �����WVS�Ã���`  d�    �D$1����9  ���  ��	t��  ����  �H����1  ��&��  ��2��  ��  ��5�f  ��  ��;��  ��  �Hă��\  ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ��"��  �J��   ���X���  j ��
  �9   �CjD��������  ZY����  ���   ���   ���  ǀ�      ���  ����  �Pd�����  �R����  �    ���|  �I���q  ��������  �o  ���t3��   �H����X  ǃ     ǃ      ǃ$     �v ǃ�     ǃ�     ǃ�     ��"��  ��������B�����������"  ���  �D$d+    �3  ��[^_�f��Hʃ��%����P   ���  �   �    f���  �������`  ���
  ���  ���������  ǃ�    ���   �   ǃ�     ǃ�     ǃ�      ǃ�  �G ��� ǃ     ���  ǃ      ǃ$     ������t& �������v ���  ��6��   j ���   �C�A   jA��������  _X��������D$d+    �  ����[^_������Hȃ��J����  f���3��   ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     �B܃��M������}���>�����6�?�����7�U���j ���   �s�A   j@������������A   ��j jA��������  �������t& ��4��  ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     �K�����    ��)wK��'�  ǃ�     ǃ�     ǃ�     ǃ    ǃ      ǃ$     �N������-�����v,��2�;  ǃ�     ǃ�     ǃ�     �������,������
  �v ����   ����  ��������f����	  ����   ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ������&    �P   �    ��l  �   f��n  ��������  ����   �H܃�������   ����ƅ ��H  ��   �m�����&������H�   ��������������������&��  ǃ     ��ǃ      ǃ$     ������t& ���r���v�H�������뾍�&    �v���믅�t����   ����Ɔ<��   ��`  u]���������`  ���u����=�   �tK��   ����������  �������t& ���m����5   ���O����1����t& ��`  ���������   �������|������`  ��t������ǃ�     ǃ�     ǃ�     ������t& �ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ������&    ǃ�     ���   ǃ�     ǃ�       ǃ     ǃ      ǃ$     ���  ��   �'  ��  ��������     ������   �'  ��  1�������v ���     ������   �'  ��  ��������   ��  �:����������    ���������ȃ���!�	ȃ���   �'  ��  ���������Hu����     �������  �O�����&    f�ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     �5�����&    ǃ�     ǃ�     ǃ�     ������`   �{����D����؉\$�D$H   �D$    ��������   �    �������F  ���6  �1  ������������P(�   ���  ���  ���  �B�T$���1�Í�&    ��&    ������UWVS�Ã�� ���  �p(���  �����  �B���  �   p���  ���  ���B���  �@    ����������  ���  ��x  9�tS���  �M    ���  ���  ��x  ��|  �E �E���  �=x  �H��  ����[^_]������t& ���  ���  ��x  ��|  �E �=x  ��  ����[^_]�����f������UWVS�h(���   @  �������  ���  ��������Ƌ��  9�t@�v ���  �P�-x  �Q�
�   ǀx     ǀ|  "  ��������  9�u�[���^_]�������    �����VS�������(  �F��9�t$�X���Cd� ��t���t��������CL�X�9�u�1�[^Í�&    �t& �����WV��S�������(  ���  ��4t��8��wP�F   ���  ���F��X�9�t(�t& �Cd�@��t�H��t	���������CL�X�9�u�1�[^_Í�    ���  u�������捴&    ������WV��S�������(  ���  ��4t��8��wH���uK�G���X�9�t)�t& ��Cd�@��t�H��t	���������CL�X�9�u�1�[^_Í�    ���  u�������������WV��S�������(  ���  ��4t��8��wX���u[�F�   ��  ���������X�9�t(�t& �Cd�@��t�H��t	���������CL�X�9�u�1�[^_Í�    ���  u�������������S��������;��(  uH�C����w=�C    �C    �C�  ��x  % �  �����`�  �C1�[Í�&    f������[Í�&    f������V��S�\$�������(  ��x  %�� ��u(��҃�Ճ�6������σ�;�C1�[^Í�&    �������&    f������VS���������(  �x  ���  ��x  ��|  �1��S[^Í�&    f������VS���������(  ���  �C���  �K���  �s���  �6�s���  �B�C   ���ȉC1��K[^Ít& ��������u��I  �A,1�Í�&    �v �����Í�&    �v ������ �@(�������&    ��&    ������U1�WVS�Ã�� �p(��������k�ǋ��  �@���  �����  ����   ���  ����  ���  ����   wT���+  ����  ���   ���  ���  j P��j j����������=x  �6  1���[^_]Í�    ���  ���J  ���  ���   ���  ��Q��j P��j Q��������륍�&    �v ���  9ST�  �SP���  ���@����x  ��x   �  ti��    ��  ����   ���p  �$    �����   ���  �T$���  ��R��QQP�t$��D$��������	�����    ���   ���  ���  j Pj�j �����������������    ���  ���   ���  ��Q��j PQ�ɍ�&    �v �������[^_]Ív ����  �$��1��I�����������������VS��������5�I  ��(  9s��   ��x  �S% �  �����`�  ��t^��wy����   �   j �s��  j P�C�S�0   j P��j������C��I  �C   �����C�C�C1�[^Ð���   ��9Kw��   ��랍�&    f��J����   C�노����[^Í�&    �������j����t& ������U��WV��S�Ã�pd�    �D$l��������   ��(  �|$�D$   �$1��=x  ���  ���������D$�����U  �<$��   ���]  ��`  ���O  ���  ���A  ��I  9E��I  E��=x  ���  �E���  �U���  �M���  ��  �E�]0�D$$   �<$�u�M�U�D$(�D$�E(���t$ f�D$0f�D$�E,�L$,f�D$2f�D$��f�\$4��T$�\$��L;|$tY�t& �Cd�@��t�p��t�L$1҉�������CL�X�9�u؋D$(�T$�t$ �L$,�D$�D$0f�D$�D$2f�D$�D$4�U�T$�E0�$�U�T$�u�U(�T$�M�U,���  �E�D$ld+    u�D$��p[^_]��D$�������������&    �t& ������UWVS���������  �   ��(  ��������    i��    ��    ������������  �k���� t���� ��  ������������   ���   ��u���   P�C0h�  P������CT ����  ����4t��8��w�CT �1�[^_]Ít& ����  u�[1�^_]������W��V��S��@�P�V������������������������=x  �  ��������C�B���   �   �C"  [^_�������&    f������WVS�áx  ���+  ���wr�s�{1��C    ������F,    )���0���<��   �    �������������Q  �� t���� �G  ����������C$   1�[^_Í�&    �������퍴&    f���������T����t& �����S���������(  k�l  i��    ��,   �   ����v��p  ��x  ���[  ��[�������&    �t& �����U��WV��S�Ã��x  ���x  �������t]�ǹ�   �C󥉂�  ���  ǂ      ��0  i��    U��    ���  h�  j �T$P������T$����(  ����[^_]Í�&    ��&    ������V�x  S�X(���  ����  ���   @  ������d  �N�v����������   �   ��0  ǃ       �غ   ��������      �������      ��� ����   ��@  ��@  ���X1�[^Ít& ������S���������(  ��l  ��x  ����  1�[Ív ������Љʋ9�t	������t& Í�&    �����UWV��S�˹   ��d�$�|$d�    �D$`1��D$   �=x  �D$   ���6  ��|  �$3�x  1�	���   ��   ���  ��`  ����   ���  ����   �$�~��|  ��ǆ�  �  ��x  % �  ǆ�     �����`�  ���  �F�X�9���   �t$�v �Cd�@��t�p��t�$���������CL�X�9�uًt$���  ���  �F���  �L$�t$�X��T$9�t-�t& ��Cd�@��t�p��t�L$1҉�������CL�X�9�u�1��T$`d+    u8��d[^_]ø������׉Љ������PWh�  h�  h0  �����������������t& �����WVS�t$�|$�������(  �x  ����  �����[^_������&    �v �����S
  ��d�    �\$1ۈT$�   f�T$�T$�L$�   �T$���$L   ������T$d+    u��[�������������� 
  d�    �L$1ɈT$�T$�   �T$�   f�T$�T$f�L$�   �T$���D$ �$L   �D$L  ���������  �D$�T$d+    u�� ��������&    �v �����WVS�Ã�d�    �D$1��x  ����  ����   ����   �   ����������� �|$�T$�   ��
  �D$f�L$��   �D$�D$L   �|$������   �   f�D$�   �T$f�D$���D$L   �|$������x  ���$  �D$d+    u��[^_Ít& �   �����������[����������&    ��&    ������U�-x  WV��S�Å��o  ���  ��l  �B�у���   ��!��   ���  ����   �Hd�y���&  �?���  k��   i�  ��0   ���^  �)���T  j 1������_���  ����   �� ��   ��vl�B߃�w
�I�#��rk[^_]Í�    k�i�  ��,   �W����   ����������  ���  �х��C�����&    �� t}��tx��w���v�� ��s�f����  ��tV�Hd�I��tL�y��tEk��   i�  ��4   ��tc�i��t\j 1���������  Z�f��� t���� ���뭉�k��   i�  ��4   ��t�������   ��[^_]�;����v j 1���������  X븍�    j 1������Y����������S�Ã��x  ����  k�i��    ��4   t���غ   [���������غ   [����������V��S�������(  k�l  i��    ��,   ����v,���w=�x  ����  ��p  �؃��h���1�[^Ív �>������    [D�^Ít& �������ې�����UWV��S�Ã��x  ����  �>����   k�i��    ��,   ����   �F$   i��    ��,   �    �,�@  �F�$�������������  �� t���� �  �$��������F0�r k��F4    i��    ��,   �   ����w�F$   �   �V(9�l  t1���[^_]Ít& �C���x�9�t��8�Gd�@��t�H$��t	���������GL�x�9�u�뼸����뷍v �����VS���������(  �x  ���#  �ډ�[^�����t& �����S���������(  ��l  ��x  ���?  1�[Ív �����VS��������x  ��(  ���\  ��w9k�i��    ��,   ��t!�ډ��h��������O���1�[^Í�&    �v ������썴&    f������뉍�&    f������UWVS�˃�$d�    �T$ 1��������(  ���  ��:w��#��   �P�����   ��8���6  ����<  �C�		� �n���  �F\������ǅ��A  ������D$����  �F�p�9��8  f��Fd�@��t�H��t	�ډ�������FL�p�9�uܸd   ������D$�$    ����  �G�@�����1҉�������G�@������]  �v �Pܹ q ��r��4�4����#����t& ��K��x  �D$   ��|  �D$   ���  �D$�F\�T$�		� �L$������D$�$   ��t������$���k  �=x  ����  ��,  �   ���������   �@�D$���  �P�����   ��$��w� q ����   �D$����  ���  ����  �F�n�X�9�t?��&    �Cd�@��t�x��t��x  ��|  ��������CL�X�9�uҋD$���  �D$�T$������d   ������$����   �$    �D$ d+    �  �$��$[^_]Ív ���  ������v �$�����ȍ�&    ���  �D$�#�����&    f��F�D$   �p�9�������d   ������$    뀍�&    �|$�G�@������   ��������G�@������j����t& ��G�@������   ��������G�@������F�p�9��P����d   �����������|$�G�@�����1҉�������G�@����������������v ������f�����    �����WV��S���      � ����   ���   �Ӊ�$  ��  t+��   �  ����   ��t�=x  ��  �t& 1���u�ډ������[^�_Í�&    �v ���  ����������   ��   ���  ���  9�t����������   ������f�[1�^_Í�&    �v �=x  �L  1���t�뚍�&    �v ������x  S�Å��h  �غ   ��������  ��t���  ��uY�����ǃ�      ���  ��t���  ��u%�����ǃ�      ��0  ��t,��[������t& ������ٍ�&    f������륍�&    f�[Í�&    ��&    �����UWVS�Ã�@�x  d�    �D$<1����@  �@  ��F  ��   �  �   �͹   �������I  �   �=�I  YUYVǃx     E�ǃ|      ���  ���  ���  ���  ��X  ��X  ��\  ��ǃ�     ǃ�  �  ǃ�  �  ��������  t���  ����  ���  ��  ���������|  ��x  �������1҉������1҉����������������   ǃ     ǃ     �   ��,  ��  ��ǃ<     ǃ@     ǃ4  �  ǃ      ǃ$      ǃ8      ��  ��������  ���   ���  ��d  ǃd     �   ��x  ��l  ��ǃh     ǃ�     ǃ�     ǃ�  �  ǃ�      ǃ�      ǃ�      ���������   �K  ��F  ���=������  ���  ���  ǀ�    ���  ��4t��8���  ���  ���      ���  ���  ���  �� J  ���   �21�j�������XY����  �\  ��&    �؉$������$�D$<d+    ��  ��@��[^_]����  �s���^  �   ������(  i��  �  P��j ���   �����_]�ƅ���������  �D$    �D$    �D$���D$    �D$   �����f�D$�Fd�D$    �@��t*�H$��t#�   �T$����  �x$����  ����������  ����   ��*��   ��'�&������  �D$(    �D$,    �D$�D$ �D$0    �D$4    �D$8    �D$ <  �D$$@   �D$�Fd�@��������H(��������   �T$���!  �x(���  �������������&    �v i��  j �(  R���   ���������XZ������&    f����  ������������    ���  �D$(    �D$,    �T$�T$ �D$0    �D$4    �D$8    �D$ .  �D$$@   �T$�Vd�R��������J(��������   �T$��t0�x(��t)����������  ������&    ��������f����t& ����������  �h�����    ����������������f�f�f�f�f�f�f��������@(�T$��x  �   % �  ���%�C   �  �1�Í�&    ��&    ������UWVS�Í�x  ��� ���  �p(���  ����X  ���  �B���  �   p���  ���  ���B���  �@    ��X  9�tt���  �L$�   ��\  �$��������L$��\  ��\  ��x  ��|  �(��������$���  �=|  �@�P��  ��[^_]Í�&    f����$������$��\  ��\  ��x  ��|  �(��������=|  ��  ��[^_]Í�&    �t& �����UWVS�h(���   @  ���݉���  ��X  ��������Ƌ�X  9�t@�v ��X  �P�-x  �Q�
�   ǀx     ǀ|  "  �������X  9�u�[���^_]�������    ������ �@(�������&    ��&    ������W1�VS�Ë �p(��������S����x  % �  ���������u	[�����^_�i�@  �ST9�r�G׉SP���  ���   Pi��  j h�  P��j �	�����1���[^_Í�&    ��&    �����V�|  S�X(��X  ���  ��d  �v��  �P,����������   �   ���  ���   ��4  ǃ`      �غ   ��������      ���"����      ��� ����   ��@  ��@  ��"�X1�[^Í�    �����S���������(  �C����C�  �CGREY�C    �C$    ��x  ���t'�C
   �C  �C   �C    1�[Í�&    ��� t��C   1��C?  �C   �C    [Ð�������tFVS�á|  ���4  ���  ����������   ��$  ��X  ���������������   [^�1��f�f�f�f�f�f�������V���   S�É���������   ��  ���������[^������UWV��S�Ӄ��|$4 �T$ t�    p���@�    �@�    �ǃ��t�� � �����T$,����   �D$0�$    ���D$��    �K�l$$��t-�����&    f�)���������  �H9�v�l$$��)���$��ti�|$ tb1��t$0���W�%     ;L$$rRD$$   �s�w�G    �׋t$(�$�$�9D$,�k�������[^_]Í�&    f��   �W;L$$s�	ȉT$   �s�؉w�G    +k������T$�Ë@9�v5���؉Ӎv ��   ���K�P�S��C�    +h������H9�r؉ډÉ�z   ��C�B    �B�D�����    �H�l$$���������&    �v �����WVS���   �Ë�\  ��X  ��P  ��T  ��	�t
���� )��R����P�    ���������ZY��t|���  i�  ��   ti�  ��   ����   ����   i�  ��   ����   ��t7��
  �������|  ��������   ��������   [^_�����[^_Ít& �����������&    ����������  �o�����    ���  ��������  i�  ��   ���a�������������  �O�����&    ���   �����i��    ��   �<����������t7S�X`�� v@t��v@t[Ð9��  u��[�����9��  u�[�����Í�&    ��&    ��������4  t*S���   ���  ���   ���  ��t��u
[Í�&    Ë��   ���  ���   ���  ���   ��  ����   �� ����   ��  ����   �� �RQh�
  ���   �   P�������[Í�&    �v �����UW�   V��S���$�M��&    ���  ����  ��C���B��   ���   �C"  ������V�$)Ѕ�~O��~K�9�tE�������������������������F�P�V�������F��;$u��=�  v���  �t& ��[^_]Í�&    ������WV1�S�Ë��   ��   ���   ��   ���   ��@  ���   ��@ ���   ��@ ���   ��@  ���   ��@ ���   ��   ���   ����������   ��  ��1����������   ��   ���   ��0  ���   ��@  ���   ��P  ���   ��`  [^_Í�&    ��&    �����WV��S���   �_������������������������L  ���������������������GX��������������[^_������t& �����W���   V��S�É������	��   ���   ��  ��  	��[��^_�������&    ��&    ������W���   V�Ɖ�S���������#��   t���   ��  ��  	؉[��^_�������&    ��    �����W���   V�Ɖ�S����������   ��  ��  ��!É[��^_������v �����W���   V�Ɖ�S����������   ���Ѝ�  !��   ��  !ȉ[��^_�������&    �t& �����UW��VS�Ã��r��  ���  ���8	  �G��1҃���D$�G�L$�$��   9�FЉT$���>  ���   �  p�P�   ���   �P1ҋ��   �P1��D$    �t$�L$9$ro�|$1�f��=�  �]	  �������   �$�D$�x�$�8�F�����   ��F�����   ��F�����   �����l$9L$w��|$�G�W(�����   ���  �T$(��G1҃������   ��G�L$�������   ��G�t$�����������   �0�G�W�������   ��G(���G��   ���  ������   ��   1ɍt& ��GЃ����   �����Pu�G�W�����   ��G�L$�����   ��G$�����   �0�T$�G �������   ����=�  ��	  ��1�[^_]ú   �������t& ���	  �G1҃����   ��G�����   ��G$�����   ��G �����   ���1�[^_]Ít& ���   �����   �������    �����UWV��S�Ӂ�  �8��  ��t`�=�  �k  ���   ����������   �����   ����   ���������������   �����   ��   [^_]Í�&    ���u�[1�^_]Ít& ������U�hW��V�Ɖ�S������F�V9�t/�X����&    �v �CL�X�9�t9��   u���������[^_]É�1��������[^_]������UWV��1�S�l$�Ӌ|$$�����1҃|$����   ыT$T$ j ��h�  ���T�эK�TR��������ZY�C��tn���tj���j W�t$,�t$,j �������|$�t'1҃����R��j W�t$,�t$,h   �L$0�q������C+C1҃�;w[��^_]Í�&    �������荴&    �����U�   WV��S�Ӌ|$�T$�l$j �Kh�  �����T�R��������ZY�C��t:1҅���1�R��UWj �t$$j������1҉C+C����;w[��^_]Ít& ������썴&    �����UWV��1�S�l$�Ӌ|$$�����1҃|$����   ыT$T$ j ��h�  ���T�эK�TR��������ZY�C��tn���tj���j W�t$,�t$,j �/������|$�t'1҃����R��j W�t$,�t$,h   �L$0�������C+C1҃�;w[��^_]Í�&    �������荴&    �����S���  ���  ���   j ���  ���  �   ���������[Í�&    f������U��W��V��S����  ����q  ����������   �����   ����   ��!ʉkV,���   ���d  ���  ������=�  Z��  ���   ���   �����   �i��        �P��u�@���  ����  ��� ��������   ��t�����  ��   ����   ��  �����   ����   ��   �����   ����   ��  �����   ����   ���   �����   ���� ��������   �   �����   ��E    ���  i�  ��   u"���   L  � ������   ��L  ���  i�  ��   u"���   L  � ������   ��L  ���  i�  ��   uE���   L  � ������   ��
��L  ���   H  ���  ���E ���   ��,  ��h  =u  t-w  ���,  ��  ����  ���������   �����   ���   ����   �����   ���   ����   ���{��������   �����	��   �����������������   ��ut����������>������      ��� ����%���i��    ��   tb�=�  wi����������     � ��������������1���[^_]Ív ���   ��  �)	��u����1҉�������e���f��   ����������  ��&    �t& �����UWV��S�Ã����   �*���   1��������  ���!  �C��uC�������u(���   j �Ä  ���   ���   �
�������1�����[^_]Í�&    �v ;{Tw�{P뼸�����ݍ�&    ��&    ������UWV��S�Ӄ��8���  ���  ���Ǹ  �B���  �   p���  ���  ���B���  �@    ����������   ���   ��x  9�tT���  �M    ���   ���   ��x  ��|  �E �E���  �H��  ���l  ����[^_]������t& ����   ���   ��x  ��|  �E ��  ���E  ����[^_]�����������UW��VS����  ����  �0���   �����   ����   ��!ʉ���   �����   ����   ��!ʉ�   �v �X�A �������u�d   ���&    �v ��t,�X�A �������t���   ���  ���   �  � ��u�i��    ��   t}���   ���   �$������D$�?�v ���   ��F�������B���   �   �F"  �������  ����  ���   9�u��T$�$��[^_]�������&    ���   L  � ������   ����L  ���   1҃����   ����   �   �����   �i��    ��   �$���1҉�����������v ���������   UWV��S�Ë��  �����   �8�=�  wd�� 3 ��  ��   t.�=�  �G  ���  ���������t  ����������������  �����   �0�   [^_]Ít& 1�Ë��  �����   � WPRh�  hD  ��������q���f������UWVS�Ӄ�<d�    �D$8���   ��  ���   ����������   ��  ���������!��$�%  ���   $  � �D$���      � �D$���   T  � �D$���   P  � �D$���   ��4  ���   0  � �D$���   D  � �D$���   @  � �D$���   ��   ���      � �D$ ���  �����   � �D$$���  �����   � �=�  �D$(�x  ��%   �D$�����   ��   t�=�  �K  ��   t�=�  �o  ��   t�=�  �  ��   t�=�  �'  ��   t�=�  ��  ��   ��  ��   t�=�  �>  ��   ��  �=�  �  ��   �  �=�  ��  ��  � u��   t�=�  ��  ��   ��  �=�  ��  i��    1���  ���2  ���a  ��t"i��    ��   ����  ���S  �D$��t"i��    ��   ����  ���9  �L$���-  �T$����  �D$��t[���  �D$7 ��tL�Pd���tC�J$�L$��t8�    ���x  �J$�L$���i  �T$7�щ�l$������|$7 t���$   ��   ��u-�$    �D$8d+    �9  �$��<[^_]Í�&    �v ���   �<$��  �$   ��&    ���  ��������؉L$������L$��i��    ��  �����t& ��  � ������؉L$������L$��}�����    �T$��������T$ƅ�������U�t& �����������     ��������������������  �    �    �����������������t& ��L$�T$�������ƋD$��������U�����    �=�  ������_  ��    �=�  ��  ��   �.�����   �/����=�  �"����  ��&    f��ʉ������������T$��������������    ��  � �����=�  �	����  ��&    �v �D$�������=�  ������v  f����  ��������)����t& �T$��   ������9���f���L$7�l$����������������&    �����V�Ɖ�S�Ӄ�t���   ��  �
	ȉ���� t>i��    ��   �_  ��   �_  ���   ��   �
������	ȉ��  � �w  [^Í�&    �����V�Ɖ�S�Ӄ�t���   ��  �
��!ȉ���� t@i��    ��   ��  ��   ��  ���   ��   �
��������!ȉ��  � ��  [^Ív �����V��S�Ӄ�uh���� u��%  � ��  [^Í�&    �i��    ��   ��  ��   ��  ���   ��   ��[^��!���%�� Í�    ���   ��  ��!�[^��Ð�����W��V��S�Ӄ�t ���   ��  ����%   ��ts�
	ȉ���� tFi��    ��   ��  ��   ��  ���   ���ۍ�$  ��t��$  ��!Ӊ[^_Ív ��$  	Ӊ[^_Í�&    �v �
��!ȉ뉍�    �����U�8  ��  W��VS���4   ���������  �á`N  ����   �   ��tM1ɸ"  �Q  1��������u%������   ��	  �� 	  1��������t�������   ��&    ��4  �k���	Љꈃ4  ���   �D$������ƅ�x/j �C\1ɺ   �D$��������   X��t,�$�����������������������[^_]Ít& 1�댍t& �$���   �����ǃ�  �����C@���  ���  ���  ���  ���  ���  ���  ���  ���  ��ǃ�      ǃ�  ����ǃ�      ǃ�  ����ǃ�      �C< E  ������D$���7������  ��  ��  ǃ�       ǃ�      ��������  ��  ��  ���������  ���  �L$���  �PPh�  Q��  ��������   ���@"f=����  f=R�t��u  � ?���L  ���   ��  ��h  ��d  ���'  ���  ��@N  ;    �(  ǃ�  �����    �t$�f����  �����uR9�t1���   ��    f9P$u���   f9P&ű�   ���  ��ǃ�      ����������   ���  f�x"R���  i�  ��(   ��t���   ��+��  �P���   �Rt���   �@�������   ����������      ����������  ǃ�       ���   i�  ��|  ǃl    ǃh    ǃp     ǃt    ǃx    ǃd     �ǃ�     ǃ�	    ǃ�	    ǃ�	     ǃ�	    ǃ 
    ǃ�	     �ǃ
     ��
  ǃ|    ǃx    ǃ�     ǃ�    ǃ�    ǃt     ��   �������  i�  ��   �������  ���   ��\  ��X  �T$��T  �D$��P  �T$�T$�D$�D$��	�t���� +D$T$�D$�T$j �    �t$�t$�t$�T$$�L$(�����������  ���   ��T  ��P  ��\  �L$��X  ��)��X  �    �T$D���������  ��  ���   ���   ���  9�@N  ��  Eʋ��   QPi�  ��    �B&P�B$P�t$$hP  �������  �����[  �D  ��&    ���  �����������   �6���f�x&7q�U���ǃ�   @x}�F�����9��  ��8����ǃ�  <   ��@  ���������   ������  ���	  ���  ��   ��  ��  ���  ���  ��4  ǃ     ǃ    ǃ(      ǃ,  �G ǃ�      ǃ$     �������,  ǃ�      ��,  ��0  ��0  ��u
ǃ0     ǃ�    ǃ�  0 ǃ�  @ ǃ�  P ǃ�  T ǃ�  X ǃ�  \ ǃ�  ` ǃ�  d ǃ�  h ǃ   l ǃ      ǃ  @  ǃ  D  ǃ(     ǃ     ������  ���Q  ��t  ���  ��  ��  ��t  ��x  ���  ǃ�     ǃ�    ǃ�      ǃ�  �G ǃ�      ǃ�     ��������  ǃl      ���  ���  ���  ��u
ǃ�     ǃ�    ���  ǃ�  4 ǃ�  @ ǃ�  P ǃ�  T ǃ�  X ǃ�  \ ǃ�  ` ǃ�  d ǃ�  h ǃ�  l ǃ�  D ǃ�  0  ǃ�  4  ǃ�     ǃ�     ����ǃ�  =   ��A  ���\����;���������$����x  ����� K  �]���f�f�f�f�f�f�f�������	 �Ít& ������V��S�@   ��� �������t)�F���x  �����   � �u׸   [^Í�&    1�[^Ív �����U��WVS���x�D$��  ���t	���"  �Ef����   ��  ����!  ��1��Չ��1f����  �����   ��B�0��  ���o"  �B��9�vm��p  ���$�ف�  ��  9���|  F��
�����   �����t  �����   ��D$������$���v�����  ����   �������[^_]Ít& ��|  �M �������   ���t  �����   ��p  ����D$�}�����t��D$�@���x  �����   � �t�5�  ����!  ��1�[^_]ø�����u����"  �t& �����U��W��VS���p�D$��  �L$��M���b  ���#  ��|  �E ���   ������f����  �]�	؉D$��p  f���-    �$�D$����  �����   ���t  �$�T$�����   ��D$�}������  ��  �ҋT$�a#  �E����   ��   �Չ��v��&    f���  ��|  �$�T$�����   ����  �����   ���t  �����   ��D$��������   ��  �$����#  �B��9�~v�J����p  �99��{����؁�    �|$ D��g������������9#  �v ��  ��  �|$ Eȉ$�����v ��  ����"  �������[^_]Ít& ����t  �����   ��p  ����D$�+�����tŋD$�@���x  �����   � �t��  ����"  ��1�[^_]ø����뎍�&    �t& �����U��W��V��S����  ����#  1ۅ�<�   �t& ��C�$9�}�D���@t� f9tJ1ɉ��Y�����x1��9�~^�[��  ���T ����#  �Bt�1ɉ�������yσ�[^_]Ít& ���I��������x�ڹ   ���y����$뛍t& ����[^_]Í�    �����UWVS�Ã�H�3d�    �D$D1���  ����$  ��k1���   ��󫍻x  �}   �C    �C   ǃ  cx23ǃ  885 �|$󫍺�  �    ǃ|  cx23ǃ�  885 ǃ�  inteǃ�  rnal���   �   �Ch��  �$�������������   ��0��$  �$��������F�[���   ���������l  ���  ����$  �=�  ����$  ��  ���-$  �t$1��   �!	  ���	   ����������	  ��h    �������l  Z�T$Dd+    u&��H[^_]Ít& ��0�X�������0�L����%  ������������������1�������S�Ã���  d�    �D$1����%  ���D  �   �   f�D$����f�T$�T$�D$�D$�D$��
  �D$D  �&����D$d+    u��[������f�f�f�f�f�f�f��������@(ǀ�   �  ǀ�       �   ���   ���   �ȋL$�1��    ��������$  �@ � ���      ��t ��t�   �1��f��   �1�Í�    �   �1�Í�    �����S1ɋ�4  �����   Q1ɋ[�����Z[Í�&    �v �����1�Í�&    ������S���  ��t������Å��?%  1ۉ�[Í�&    �t& �����WVS��<  ��X  ���  ǀ<      ��t���������<  �ƅ��U%  1���[^_Í�&    f������S���  ��t������Å��k%  1ۉ�[Í�&    �t& �����WVS��X  ��<  ���  ǀX      ��t���������X  �ƅ���%  1���[^_Í�&    f������S�Ӌ��   �H��~2���������t�@��t��  ��t��[�������    [Í�    �   ��������Ǎ�&    �v ������ �@(�������&    ��&    �������@(������v ������@(���   ���������   �����1�Í�&    ��&    ������ �@(� �������&    ��    �������R(�����������U�   WV��S��$  �Ӻ 0  �h �} ���������t!��t|��tH��@  �ډ�[^_]������t& ����    ��������   �������@  �ډ�[^_]������t& ����    ��������   �������@  �ډ�[^_]������t& ����    ��������   �������@  �ډ�[^_]������t& ������U�   WV��S��$  �Ӻ   �h �} ���������t!��t\��t8��@  �ډ������1�[^_]�f����   ������   ��������΍�    ���   ������   �������뮍�    ���   ������   �������뎍�    �����VS�Ë�$  �p ����  ��w9��w9��u��@  �    ��t?�������<  ��t(��[^�������&    ��!u�   ���Ӎ�&    �v 1�[^Ív �����뿐�����UW��VS���0�$�\$���  ����������   �   ��$  ���   ��   ������ǉ���      ����������   ���� �  ��   ���      ������<$ ���   ��   ��  ��$  ���      ��������   ��   �
�<$�% ���-@  !ȉ�=    �    ��9�y&�   �t& �2   �   �d   ������    9�x���   ��   ��u��ۋ��   ��   ��   ��É�D$��������1��<$ u�D$� �������[^_]Í�&    �ۀ�É�   �(���1�뤍v �����UWVS�Ã��5    ���   �$�|$��   � ���   ����   ��$  �)���   �)���������E����   ��������E�   ��t& �    9�x��  ��������      � ��uڋ��   ��   ��   �������E���[^��_]Í�&    f���$  ��m �� ����)�$�^����t& ������V�p�   S�É��������t�@��t���������8  ��t�FT�@��������������4  ��t�FT�@��������������0  ��t�FT�@��������������,  ��t�FT�@����������������  ��t!��u
�S�������ǃ      1�[^Ív �������1�ǃ      [^�f������S�tY  �@(���  �xY  ���  �   �tY  �xY  �L$��|Y  [�1�Í�&    �t& ���������u�A,MPEG1�Ív ������f������� �@(�  �������&    �v ������ �@(�������&    ��&    ��������R(���  �������&    �t& �����UWVS��(d�    �T$$1��������(  ���  �nPhI  j"�|$W������F��9�t'�X��t& �Cd� ��t���t��������CL�X�9�u����  ��������D$$d+    u
��(1�[^_]��������&    �����WV��S�������(  ���  tL�F   ���  ���F��X�9�t*��    �Cd�@��t�H��t	���������CL�X�9�u�1�[^_ø��������&    ��&    ������WV��S�������(  ���  t<�G���X�9�t)�t& ��Cd�@��t�H��t	���������CL�X�9�u�1�[^_ø��������&    ��&    ������S���������(  ��|  ��x  �S�1�[Í�&    ������S���������(  �CMPEG�C    ���  ���  �C    �C   �C��  ����k  1�[������S���������(  �CMPEG�C    ���  ���  �C    �S���  �S���  �C   �S��  ����k  1�[Í�&    ��    ������������    �����WV��S�������(  ���  th���u[�F�   �N  ���������X�9�t*��    �Cd�@��t�H��t	���������CL�X�9�uܡ�  ����k  1�[^_ø�������������v ������������    ������������    �����VS���������(  ��  ���l  �ډ�[^������t& �����UWVS�l$1ۋ|$������   ��(  1����&    f������  ���  !�!�	�u����u߸����[^_]Ít& �������������u�������  ���  ��  �J��   �J��$  �J�R��(  ��,  [^_]������UWVS��������   ��(  ���  ����������1l  ����t�����'l  ���؍k��������  �    i��    ��    �������������1l  �� t���� �'l  ������������   ���   ��u���   P�C0hr  P������ �� ��CT ������  DCT[1�^_]Í�&    �t& ������S���������(  �CMPEG�C    ���  ���  �C    �C   �C��  ���;l  1�[�������=�  S���Zl  ���   �7   ��(  �   ���   ��$  � �  ���   ��   [Í�    �����WV�   S�ÉЉʋ��   ��$  �ɋ��   �ρ� X  ��   ���   �����   �΋��   �ρ� Y  ��   ���   �����   �ы��   ���ɉρ� Z  ��   ���   �����   �����   �ց� [  ��   ���   �����   �Ћ��   �р�\�ɉ�   f�� ����   �҉�   f�����   ��]�҉�   f ����   ����   �^  ���   ��   ��  ���   ��   �5    �����    9�x'��  ��������      � ��t�[1�^_Ít& �[�����^_Í�&    �����UWVS�ÉЃ����   �$�   ��$  �ҋ��   �р�\�ɉ�   f�� ����   �҉�   f�����   ��]�҉�   f ����   ����   � ^  ���   ��   � �  ���   ��   �5    ��� ��&    f��    9��[  ��  ��������      � ��t�1����   ��  ��$  � X  ���   ��   � �  ���   ��   ���   ��   � �  ���   ��   � Y  ���   ��   � �  ���   ��   ���      � �D$���   ��   � Z  ���   ��   � �  ���   ��   ���   ��   ���   ��   ���   ��   � [  �.���   ��   � �  �.���   ��   ����	�t$����	֋��   ��   ��%  � 	��$�����[^_]Í�&    �����������&    ��&    �����WV�   S�Éȋ��   ��$  �ɋ��   �ρ� P  ��   ���   �����   �̋��   �ρ� Q  ��   ���   ���   �����   ���ɉρ� R  ��   ���   ���   �����   �Ɓ� S  ��   ���   ���   �Ћ��   ����?�Ɓ�@T  ��   ���   @�  ��   �Ƌ��   �Ɓ� U  ��   ���   �����   �ҋ��   �р�V��   ���   �����   �5    ����t& ��    9�x'��  ��������      � ��t�[1�^_Ít& �[�����^_Í�&    �����UWVS�ÉЃ����   �$�   ��$  �����   ��?�ց� T  ��   ���   ���   �ԋ��   �ց� U  ��   ���   �����   �����   ����V��   ���   �����   �5    �����&    �    9��c  ��  ��������      � ��t�1����   ��  ��$  � S  ���   ��   � �  ���   ��   ���   ��   � �  ���   ��   � R  ���   ��   � �  ���   ��   ���      � �D$���   ��   � Q  ���   ��   � �  ���   ��   ���   ��   �D$���   ��   ���   ��   � P  �.���   ��   � �  �.���   ��   ������	�t$����  � 	֋��   ��   �D$���Ћ$	�����[^_]Í�    �����������&    ��&    �����U��WV��S�Ã�d�    �D$1��=�  �D$    �|$(�!m  ���  ��P���������$=xV4��l  ���  �L$��������D$����l  ���  �   ���D$   ��������  ��1��P����������  ��  �P���������~5�t& ����  ���T��������=�  ��l  ��9�u؃��v ���  1ɍT�؃��������u勓�  �   ���D$   ������5    ��
��    9���l  �Ƨ  ��������  �L$��������D$tыT$$�l$$1���~-�v ���  ���T��������=�  ��l  ����9�u֋��  �L$�P��������=�  �vl  ���  1ɉ�������D$�T$d+    u��[^_]��������&    ������UWVS��Hd�    �D$D1��t$\�|$d�=�  �gm  �\$l��t{�4$�D$p�l$����v �p��Ã����r�9�u�4$U�����t$l�T$h����ZY�t$h��t��L$�|$h�
u	�S�L$�
�T$Dd+    u��H[^_]Í�&    �v �l$�������t& �������F�����    �����UWVS�Ã�,��  �T$d�    �D$(1�����m  j j h�   S����������;  �=�  ��n  �D$    �D$    �L$� �  ���������  �؍|$�������������X�  �������������T�  �Ɖ�������  ���  	Ɖ�������   ��  	Ɖ������1ɺd�  	Ɖ������	���n  ���   ��  ���   �D$���������n  �D$���� �n  �@�8�  ��t8h/  ������D$�����XhD  hh0  �����XZ�D$������  ��    �xf�U�u��=�  �.  �$1�1����	��&    �͋��$��)ΉL$�����������m  �D$�M� ��9�wЋ�  �$���]o  ��x%�v �L$�����������Do  �D$�t��sޅ��+o  �D$�������  ���o  ������T�  ��������L$� �  �Ɖ�������L$��  	Ɖ�������X�  	ƍL$��������L$�X�  	Ɖ؃�������� @  � �  	Ɖ������� @  ��  	Ɖ������� �  	ƍL$�����������  �����������o  �=�  �D$xV4�D$xV4�D$ 4xV�D$$V4x�so  1�1���v 1������ ��o  �����������D$9D�uڃ���uԡ�  ������m  ���  j j h�   S����������Q  Wjj h�   S�����������m  �=�  ����m  ��   ������5�  ���rn  ��   �   ��% �  �����`�  ���  ���  Pj jh�   S�f������  ��f���  ���  f���  1���   �� ��f���  ���  �����jjj jh�   S����jjj jh�   S�����<   ��0�����h�   h�   j jh�   S�����j j j j j j j j j j j j j jh�   S������Xj@jh��` hU�U�jjh � j jh�   S������,��&    f��F�1�j ��j ��j SVj jh�   W�^���������$   �j j j SPj jh�   W�:�����$��u�j j jh�   W�!����<   �����j j h�   W�����<   ��$�������   �H  ��������L$��u"�D$(d+    u?�D$��,[^_]Í�&    f�j j j jh�   W�����
   ��������D$    ��������o  �(n  ��&    �����U�   WVS���h(�荝t  �/����ƅ�ty���  ��������$��t  9�tD��&    ��t  �P�-x  �Q�
�   ǀx     ǀ|  "  �������t  9�uË$�����������[^_]Í�&    ��t  �ڍ��������  ���������[^_]Í�&    ��    �����V��S�Ӻ�  ��d�    �D$1����������D$d+    u��$����[��  ^�����������&    ��&    ������V��S�Ӻ�  ��d�    �D$1����������D$d+    u�ˉ��  ��#$��[^�����������&    ��    �����W��V��S�Ӻ �  ��d�    �D$1����������˅�t&$�D$d+    u���� �  [^_�L����t& ��#$���������&    �t& ��������d�    �T$1ҍT$�D$    �$    R�T$Rjj h�   P�C�����  ������o  �D$d+    u����������&    ��    �����S�X(j j jj jh�   S�������  ���������������  ��[�����f������S�á�  ���
p  ���  ��t%���  ��u&��������  �����ǃ�      [Í�&    �v ������؍�&    f������UWVS�Ã���  ���op  i��    ��   �N  ǃ(      ���  �2   ǃ,      ��ǃ  �  ǃ      ǃ$      ǃ�  �  ǃ�  �  ǃ�     ������Ņ�t����[^_]Ít& ��D  1҉����  @  ǃH  p�  ����������  �C\1�j ��������  �D$X��  ����p  �������t^�Ǿ@V  ��   �h�  ���  i��    ��    ���  h�  j �T$P������D$�T$��ǂ      �����  ���   ���  ���  ���  ǃ�     �   ���  ���  ��ǃ�     ǃ      ǃ     ǃ�  �  ǃ�  �  ǃ�      ǃ�      ���  ���������   ���  ��(  ���  ��0  ���  ���  ���  ǀ�     ���  t���  ���      ���  ��������   �21�j�����Y_�ƍ��  ���&p  �Sp  �����[��^_]Ít& ���������f�f�f������S��p��������p�Éу���E���p�Éу���E���p s���H(��t-�  [�������&    ���u�[Í�&    �v �������p Ít& ������H`��t����  ��t����  ��t����  ��t����  ���  9��  t�ы    �    ������t& ��������&    f���������H`t����  9��  t�    ���  �    ������t& �����   �f�f�f�f�f�f�f������WVS��h������@�d�    �T$1��D$ ��t6�Pd���t-�r$��t&�    �L$��tx�z$��tq�   ������|$ u<�   ��������   �Ȁ�ȉ�������#   ��������#   �Ȁ�ȉ�������غ   ������D$d+    u��[^_Ít& �   �����������f�f�f�f�f�f�f�������VS��0��h  d�    �T$,1ҋ���)  ���  ���  ǃ�      �Pd�R����   �J����   �   ����  �r����  ����������  ���  ���{  ��!��w#�   ������0  ��  ��   �t& ����   �Pd�R����   �J����   �   ���Y  �Z���N  �������1��   ���  ����   ��!��w��   ��������   ��  t��D$ �  �D$$��  �D$�] �D$    �D$  �D$�߼ �D$�  �D$!   �D$���?�����&    1��T$,d+    ��   ��0[^Í�&    �������   ��� u���k����������뵍t& ��D$    �D$  �D$��2 �D$ �D$������    ��������   ��� u������������f���������<����t& ��������<����t& ������0��������������U��W1�VS��  ���  �T$d�    ��$  1����  �D$    ��tz�Pd�R��tp�2��tj�   ����   �   �	�$��tu�L$Q�   �t$�T$�����Z�D$����t1��C�T��������9�u�D$�   ���x����t& �|$ uA����u1��$  d+    u^��  [^_]ÍL$Q�   �T$�����X덋C������ŋ[�T$�D$    �D$    ���D$�����ƃ0  ���������������&    ������WVS�Ã�0d�    �D$,���  ����   ǃ�     �Pd�R��t'�J��t �   ���
  �r����   ��������|$
 �5   ��   ��&    1��D$
f�D$���  ��tr�Pd�R����   �J��t4�   ����   �z����   ����������  ��tp�Pd�R��tf�J��t_���tT�~��tM������D$
���{����v ���  ��������  ��������  ������D$,d+    u6��0[^_Ív ������D$
뱍t& ��������o����t& �������������������&    �������h  � ��t�z�����&    �v Í�&    ��&    ������UWVS�Ã�0���  d�    �D$,1���t��t���  ��w-���  �t& �D$,d+    ��  ��0[^_]Í�&    ��!��wؾ����sΉփ�����   �Ad�@����   �x����   �   ����0  �h���%  ����������  ���  �D$ ���D$
����   ���  ���  �D$�����  �D$
���>����Pd�R���0����J���%����   ����   �Z����   �������� �������������փ����$�����������D$,d+    ��   ��0��1�[^_]�����v ���  �D$ ���D$
�Ad�@���2����x���'����   ���tZ�h��tS��������	�����&    ����������  �������    �   ���$����������&    ��������,����t& ������������������&    �����UWVS���  ����  ���  �ƃ�"��  ��  ��0�I  ��  ��  ��3�e  �t& ��   �   ��  ������Ņ��|  �0i��    ��    h�  h�  ������E�����^  ���   ���   ����  Ph�  h�  ������E�����%  �   ������Å���  �E�E�   ���  �Ef���  ���  �   f���  ���   �P$f���E  �@ f���  ���   �@"f���  ���   ���  �   ��h  �C$��ǃ4  ���ǃ8      ǃ�  �  ǃ�   �  ǃ�  �  ���  ������ǅ���   [��^_]Í�&    �������:u�g  �����t& ��)w#��  ��'�u��������[��^_]Í�&    f���-��w��  �N�����    ��w3�������v��   �� u����t�����  ������&    ���  ��!���������닍v ���   �N����t& �f���  ���   �@&����f���  �������    ��������ǆ�      ������E������E��������������[^_]Í�&    �v ������э�&    f��t  �f�����������������뵿����붍�&    ��&    �����S���������  ��t9�@��������  �@��������  �@��������  �����ǃ�      [�f�f�f�f�f�f�f�������ȋ��   ���   ���   ��  ��  �� ��T$�1�Í�&    �t& �����WVS���   �Ӎ�P  ���������$  ���(  �S��,  �S��0  �S��4  �S��8  �S��<  �S��@  �S��D  �S ��H  �S$��L  �C(�������1�[^_Ít& �����WVS���   �Ӎ��   ����������   ����   �S���   �S���   �S���   �S���   �S���   �S���   �S���   �S ���   �S$���   �C(�������1�[^_Ít& �����W��P  VS���   �É���������     ���߉���      ����������      ���߉���  ���   ��  ƃ.  �������1�[^_Ít& ������W���   V��S���   ����������     �������      ����������      �������      ����1ҋ��   ��  ���  ���   ��  Ɔ�   �������1�[^_Ít& ������UW��VS��   �D$�L$d�    ��$�   ���   ���   �D$���      � �D$ ���   ��  ���   ��  �؉ډ�� ���� �� �D$�؃��$�����D$�����=�  �D$��p  ��t����  !��|$�؋$���D$�ۋD$����   ��!D$�T$���p  �L$1ۅ��b  �D$  �$��    ���   @  � �������T�@���   t=��u؉�������@   �ǋ$�T$@��������É��������@��   ��q  �t& �����������ىǋ$�T$@��������$�������;$��q  �D$�D$<    � �|$ �r  �|$ �����t*�L$<�|$ ���   !���   ���   ��   �D$� ��������L$��  +�  =�  v�L$<��������\$<��t�L$����   ��$�   d+    ��   �Ą   1�[^_]�1�������&    f��|$ tU1��������     ���߉�D$�D$<   ��t �@`��t�h8��t�D$�L$<�v@������D$� ������|$ �D$<    � ����Qr  �t& �|$�G`���H����X8���=����L$<� v@��������(���������t& �����U��WVS��$���   �D$ ��  ��  �D$���u�D$8�     ��$1�[^_]Í�&    ��   �L$���$������$�L$�ƍ�  ���������$��������$�|$8�ȃ�������t��ۍD� ���D$�$���\$��   ���\$�����D$��t�t$�D$���D$1��D$    ��1��L$�$���������σ������D$�$$���  �i��  ���1��щǅ�t�ֺ��K������������k�6)����ʿ6   ���щ��v��� � =�h�w
��Mb�������U 
\$�=�  �E    �]�wr  ��;l$������u ��%�� =�� � ����D$ �   �D$   �D$    ����������UWVS��$���   �z
 ���   ��  �B�ӉD$����  ��P  �$������C
��.  �C    ǆ(      �   ǆ$     ���     ���߉���      ��������K���   ����   �1ȃ��1ȉ�C��/  ����   �K1���������7���=   �M  ����  �@��ωD$���D$�����D$����T$ 1���7��iѠ  ���K����ǉ������T$���   �L$��  �{�Y�щ�4  kKd��8  ����   ��	��q  ���9�GЋ��   ��  ���D$k�d�����S��8  �T$ �  �v �C� � �D$    9�Gº6   ��щǅ�t�Ⱥ�Mb��������T$i��  )����ʿ�  ���ыT$���  v���� 1ɉD$�ׅ�t"�@  �����)��������Љ���)�)ǉ��D$��� ���ʿ   1�9ǉ����  �   9���  ���������D$�ǃ�����T$��T$iѠ  ���K�D$����������T$���   ��  �D$�T$�D$    �щǅ�t�к��K����������T$k�6)����ʿ6   ���ыT$��v���C��0  �D$��d  �D$�C(��L  ���      ������K���   ����   �1Ȁ��1ȉ�C��=  �K���   ����   �1Ȁ�1ȉ�C��<  �C	��-  �C��,  ��tC�{	 t���     ��� ����   �{ ��  D|$��   ��   1�%���1���$�����1���$[^_]�f���$��[^_]����f��D$�J ���  �D$��  �D$  ��D$�  �|�����    �D$�J �3   �D$  ��D$ �  �D$��  �D$��  �������    �D$�   �   �D$   �D$0�?�D$    ������    �D$�   �̿ �D$0�?�D$     �D$   �D$   �k�����    �����������    �����UWVS��$���   �z
 ���   �  �B�ӉD$���}  ���   �$������C
���   �C    ǅ�       �   ǅ�      ���     �������      ��������K���   ����   �1ȃ��1ȉ�C���   ���5  �s1���������7����=   ��  ����  �@��ΉD$���D$�����D$����T$ 1���7��iѠ  ���K����Ɖ������T$���   �L$��  �s��k(����   �C2   ǅ�   2   �K���q	����)����1���9S s�Q
����̉���   �����S �O��N������9S$v������ȃ������S$���   ��   �1����1���C �T$ ���   �C$���   �D$�  �t& �C� � �D$    9�Gº6   ��։���t���Mb��������T$i��  )Ɖȉ��  ��֋T$���  v���� 1��D$�х�t"�Ⱥ@  ���)���4�����Љ���)�)��ʋD$��� ���1ɾ   9Ɖ����  �   9����  ���������D$�ƃ�����T$��T$iѠ  ���K�D$����������T$���   ��  �D$�T$1��D$�х�t�ֺ��K�����Љ�����k�6)��ʋD$�6   ��щ��v���C���   �D$��  kK6���   ���  ������.  ���  ��  �Ⱥ�Mb����K����i��  ����������  �D$�S���   �C(���   ���      ��������      �����C���   ��  �C	���   �C���   ����   �$�����1���$[^_]Í�&    f���$��[^_]�r���f��D$�J ���  �D$��  �D$  ��D$�  ������    �D$�J �3   �D$  ��D$ �  �D$��  �D$��  ������    ��� ���  �������&    f���   �������ǅ      ǅ      ��������{	 t���     �����{ �@  DD$�Ë��   ��   ��   1�%����1؉������t& 1�1��h�����&    �D$�   �   �D$   �D$0�?�D$    ������    �D$�   �̿ �D$0�?�D$     �D$   �D$   ������    ������e�����    �����UWVS�Ã�$���   ���   ���   ��   ���   ��  �L$���   ��  �L$���   ��  �|$���   ��  �|$���   ��  �|$���   ��  ��p�D$�f  �^  Sh  �������   XZ�W  �[  D�PSh3  ������������Eω$QSh<3  ����������@   E�RSh46  ���������4�   Sh�7  ������z  �o  ���� @  D�PShl3  �������  ��  ����   D�PSh�3  �������  ��  �|$����    D�PSh�3  ������L$�����(s  �cv  �t& ������U�h  ��  WV��S��0d�    �D$,1��$   ���������  ��  �   �ù   ǀ       ��h�  �����Z����  ���   �`  ����������  ���   P�Cph7  j P������F��ǃ�      ������Ń����	  ���   ��  ��  ���   �A  �������
   �t$�$   ���Cd�D$
�D$ �D$ ��  �D$$��  �@�D$�D$��  ��t(�H��t!�   �����   �x����   ���������P  ��  �X  �������1��
   �Cd�D$   �$   �@�D$
�D$�D$��  ��t2�H��t+�   ���tI�p��tB����������&    f���������D$,d+    uA��0��[^_]�f���������^����t& ��������΍�&    �ؽ���������빽������������    �����V�   S�������t<�Ë��   �������   �������   ���������  ������������1�[^ø������f�f�f�f������S��f���   ��d�    �D$1��   f�T$��f�D$�D$�D$���$D   ��������Jw  �   ����D$  ��������4w  �D$�T$d+    u��[�������t& ������VS��
  �؃�d�    �T$1Һ  �G����   �  ���f�L$�t$�   f�T$��D$���$D   �t$���������w  �   ��   ��t$f�L$�   f�D$���$D   �D$����������w  �   �	   ��t$f�L$�   f�D$���$D   �D$��������vw  �  ���~����   ��f�L$��   �t$�  �D$���$D   f�t$��������`w  �D$d+    u��[^������f�f�f�f�������1�Í�&    ������U��WV��S�˃�Pd�    �L$L1ɋD$hf�$1�f�L$�|$d�P�\$f�T$�T$�T$��s4��up�   ����������u}1��T$Ld+    ��   ��P[^_]�f���l$�L$)͉T$�T�)�T	�������r����1��,:�,9��9�r�떍�    ��L$�t��T�f�T�v�����    ��  ����w  ������m���������v �����WVS���p,d�    �L$1��D$���u|���  ������VL�FH1�j�|$W����Y��X��u5���  ������VL�FH1��D$ jW������  �����1҉������XZ�D$d+    u����[^_Í�&    �v �������������t& �����V��S�Ӄ� d�    �T$1��ӍL$�t$f�T$f�T$�T$,�L$�   f�L$�   f�T$�T$�D$ �D$   �������u)��  ����w  1��T$d+    u"�� [^Í�&    ��  ���x  ��������������    �����S��1҃�(d�    �D$$1��C�������    9Cx�C���t�D$$d+    uO��([Ít& �S��C�j!�L$�����Y�¡    �  �C��uġ�  ���.x  �D$�������C���������&    ��    �����UWV��S���D$,�L$�$�D$4�D$d�    �D$�Fd�����  ��9~lte�VL�FHj�L$�^�����X��t�D$d+    ��  ����[^_]Ít& �D$�VL���
D$�D$�FHj�L$Q1��c���Y]��u��~l���  ����������   �   ��$  ���   ��   �D$0��ǉ���      ��������   ��   �D$0�Ā�ˉ���      ������<$ ��   ���   ��   �D$��É���      �������~L@�����E�!ʉ���   ��   �
�<$�% ���-@  !ȉ�=    �����&    f��    9�x��  ��������      � ��uڋ��   ��   ��   ��É�Љ�T$�������  �T$���Xx  1��<$ D��_�����    ���   ��  ��$  �"���������9����������&    �t& �����S�\$�ɋ@,SQ�   j ������[Í�&    �t& �������ɋ@,j Q�   j�������f������S�\$�@,SQ1�j �g�����[�f�������@,j Q1�j�L�����Í�&    ������WVS�Ӄ�T�p,d�    �T$P1҅���   �VL�FHj�L$�����VL�D$�   �~H�D$
   ��`f�T$�T$�D$�T$�T$�D$���D$ �����Y��u�؋T$Pd+    u*��T[^_Ít& ��  ����x  ������эv �������������t& �����VS��P�p,d�    �\$L1ۅ���   ��ur�   �   8^htF��  ����x  �VL�FH�   �\$�D$   f�$�T$�T$���D$�������uH�^h�FP�T$Ld+    uL��P[^Í�&    �=�  ۃ�����=�  �������u����t& ���  ��t��	y  �������������t& �������  V��S�Ӆ��ey  1���  �t��  � uE��   u�   [^Ít& ���8  �    �    ��T�������  ��t��(y  ��&    f����  �    �HT�    �������  ���Iy  ��   t�럍�&    ��&    ������UW�31  V��S�   ��(�-�  d�    �D$$1��=�  �D$    ��D$ D  ����D$
    ���D$  D �D$    �D$   f�\$�D$ �D$�D$  f�|$"����y  �   �t   ��  ������Å��y  ��  �F���T  ���  �CLA�A   ��=�  �    �C    ���   �1��C    ����{H���C    �Ch�C    �C    �C     �C$    �C(    �[,�sdj"�D$P�������SL�   �ǋCHj�l$&U�����SL�   	ǋCHjU������	�uzj1ɍF(�������Y�ǅ�ue�CX�CT�����KT�    �CX�C\�    �C` �  �������  ����y  �D$$d+    uN��(��[^_]Í�    �SL�������&    ��  ����y  �������뽍�&    �v �CL@�@   ����������=�   �����t��y  ��&    �v �������t/S��  ��t�C,��t���������[�������    [Í�    �f�f�f��������� d�    �L$1ɹ   �T$��f�L$�L$�L$�   f�L$�L$�L$�   �$P   �D$P  �D$ ���������y  �D$�T$d+    u�� ��������&    ��&    ������S��d�    �\$1ۻ   �T$��f�\$�\$�L$�   �$P   �\$��������z  �
   ��&    �X�A �������u�1��T$d+    u��[�����������W��V�ֺ?   S�@   ������Ff��ډ�������D�����Fu�v �ډ�������D�����Lu�[^_�f�f�f�f�f�f�f�������W�   V1�S�Í�&    �   �   ���������   ��������R� ������   �   ��������   ��������R� �����1ɺ   ��������   �������	Ɖ�,��u���[^_Í�&    ��&    �����W�   V��S���E�t& �������R� �������   �   ��������   ��������R� �������,��tX�   �   ��������   ��������R� ������   �ع   �������   �����w���������r����t& �[^_Í�&    �t& ������W�8  VS��d�    �L$1�f�<$�D$���I  ���p  ���P  ��$  �L$�   �@ �����&    �v P��9�u��ڹ   �؈T$�   ������   ��������   �   ��������   ��������R� ������   �   ��������   ��������   ������   �   ��������   ��������   �������   ���2����   ������   �|$�t$���&    f����������9�u�   �   ��1�������   ��������   �   ��������   ��������v �   �������1ɺ   ��������   ���������t��<vω�<�z  �   �   ��������   ��������   �������   ���R����   ������������   8Љ�F�1��v ������������8�u�   �   ��������   ��������   �   ��������غ   �����1��T$d+    uN��[^_Í�    �  �D$f�\$�����t& �  �D$ f�t$�����t& 1��D$ f�L$���������f������1�Í�&    �������P�H|�R�AP�҃�!��AL�f�������@|�4  ���������&    �t& �����UWV��S���X�{����������=  ��������  �3�k ��d  �D$  �$��  �����7z  ���   @  �����u�M �T$���������   �U ��P  �   ���   ��X  �   ���   ��0  ��  �C    Z�����  ���   � ��P  ��������   ��T  ���   ��  �*����   �ȃ���!�	ȃ�����      ��� ����   ��@  ��@  ����5�  ����   1����������[^_]Í�&    ���  �3�����Uz  ���   @  �������     ��������   P  ����������  ��t���d  �1��  ������x�����$�1�������f����D$�S�M ��  �ʃ������   � ��QRP�u ��  Ph<;  �������������&    �t& ������UWVS�h�X|���mz  �ǹ   1҉�������ƅ���   �}$1����   �   �ƃ)  1�ǃ�    ǃ�      ǃ       ǃ  �   ǃ  ��  ǃ  ��  ǃ     ǃ     ǃ    ǃ!    �E ��d  ��  =   t�������  ��   [��^_]Ð��  ����v��z  ��&    ��&    ������P����   VS�á�  ������z  �s �N ��ua�F������F    �F������F    �S �C�J�rj �r�   �r��������C ������C     ���C    [^Í�&    �V�N$�Cj j�   ������F     X�s Z�w�����&    ���&    ��&    �����S�Ë@|��4  ��t�C�����C|ǀ4      1�[�f������UW��VS�Ӄ��p�@|��4  ��t��������G|ǀ4      ��L  �F��X  �V�F����  �B�����  �   �(   ��  ������Å���  �F��F�^ ���  �́� ����ȉ$��������$���C��  ��  ������z  1�������k$�Dm ��������C���  �S$������C$��tU1��#��&    f�	��A    ����A   9k$v.����C���������   �K�Tm �ы���t��t& �n �F�M$�Uj �   j������E Y]����z  �K�Fj�S�v�v���������xu�C�   q�C�S�P�C�@    �V �G|�R��4  �G|�V��@  �G|1�ǀ8      ǀ<      �7�C������C    �C������C    ������؉$������F     �$����[^_]Í�&    ��    ������ޡ�  ���������v���z  �����1�Í�&    ������UWVS��ts��0  �ƉӋ��   ��T  ��   �{  ��u���   uR[��^_]Ív ��  �����V{  ���   �   ��0  �   [^��_]Í�&    �v 1�[^��_]Í�&    ���   ��   �E�E$�������[^_]�f������VS�Ã�d�    �D$1���  ���+  ��d  ��  ���p{  ���   T1ɺ����j(h    �   �����������{  �$�T$1ɋ��  ����   �F    �F�$�FR�  jj �����������   �D$�   �  �ü  ���  ��X������D$�   �   ������$�   �)  �������h1  �D$��$P������D$S�P$��DRhB  P������D$���������x-�=�  ���{  �D$d+    u ����[^Í�&    1����$������{  �������&    �v �������0  �@��������������  �����  �t$,Sh8   ��������D$Dd+    u=��H[Í�[���� @����  �č������ ����  묍���������������VS���  ���   �x$ uSSSSShd   ��������SSSShD  �������S1�h  �����XZi�  ��    VFSh\  ���������?u�[^�Q�����h   h�  ��������m  �=    Yt}h   h�  ��������  _Y���  �   �   �"   �   ��E�VRh  ��������   ��D$���   ���������uP�$�D$������$������B  �5#   i��    ��    ���  Ph�  ��������  �����  Vh<  �����[^뭉$�sSh4  h  ������T$���  �$�sSh4  hP  ������T$���  h�   h�   h�  ��������=   ���  ���  �0�p���  ���  �sSh  h(  �����1���$�!  h  h  h�  ��������  ���`"  h  h  h�  ��������  ���#  ht   ht   h�  ��������j$  QRPht   ht   hx  ���������$  �0  ������(  ������v������W������Vh�  h�  ��������'  h�   h�   h�  ��������c'  �0  ������(  �����Qh�   h�   h�  ��������I(  h`   h`   h�  ��������{(  h  h  h�  ��������)  Ph  h  h  ���������)  h�   h�   h�  ���������+  h�  h�  �����XZ�,  R�T$h�  h�  hd  ������T$���-  h�  1�h�  �����XZ�=x   u����$u��-  ����������PVh�  h�  ���������i��    k���    �pD�p@�p<�p8�p0Vh�  h�  h�  �������$��-  R�T$h�  h�  h�  ������T$����/  Rh�   h�   h�  ���������d0  hl  hl  h�  ��������0  �0  ������(  �����h�   h�   h�  ���������1  Ph  h  h  ��������%2  Sh   h   h�  ����������S2  �=x   ��2  h   h   h(  �������������2  �t$�t$�3�t$h�  h�  hL  ��������?4  h�  h�  �����[^��4  h   hx  h\	  ��������6  �=x  vh   hx  h�  ����������  Ph�  �������d  �������XZ�6  h   hx  h	  ��������6  h\  h\  h�  ��������R7  ���  ��  ����   PVh�	  ������Q  �@  ���\(  ���  �����  ���  ǀ�    ���  ��4t��8����   ���  ���      ���  ���  �� J  ���   �2�   j�����Y_��y�$Vh�	  �����XZ�$�:  hD  hD  h�  ��������8  �$Vh�	  �����]X�$�:  ���   �������  ���  �o������      ���  �Z������  ��  ��t#PVh
  ������������1҉�0  ���:  ���   ���sSh�  hD
  ��������k>  �sSh�  h|
  ��������k>  h�  h�  h�  ��������*@  h�  h�  h�
  ��������gA  �����UW����VS�Ë�   ��u��  P�   �   Sh�  ��������������t�4��  h�  �����XZN��u݁��  Sh  ��������  ZY��u�   [^_]�P�t$������Uh�  h  ��������*F  P�t$������Uh�  h  ��������*F  �2hx  hx  hL  ��������dK  �$�2hx  hx  hx  ������L$���TI  �$�L$�A�PVhx  hx  h�  �����1ҋL$����I  �t$�t$�7��h  hx  h�  ��������BK  �����U1�W��V���  S�Ӄ��2Vh8  ��������C�����   � P����<  Vhh  ���������8u�1�C�D�8�����   ��$UEVh  ������D$��������u͉4$��1�ߋG��    �L$؃����   �0U_S�   �t$h�  ��������u������l$�l$����9�s1�B�T$�������   � SCPVF�t$h�  ������T$���ˉ�l$��Ճ��l������4$�ϋC�S�RPVh�  ������C�P`RPVh  ������C�����   � �� PVh0  ������C�����   � PVhT  ������C �����   � PVhx  ������C$�����   � ��$PVh�  �������[^_]���   t �=�  vh   h�  h�  ���������   t$�=�  ��   h   h�  h�  ��������݁�   ��t �=�  v;h   h�  h   ���������t �=�  vh   h�  hP  ����������  Ph�  ��������   �����   ����   ��!ʉkV,���d  �����XZ�)L  jh�  h�  ���������K  �݁�   ��u��k���hd  hd  h�  ��������Q  kV,���d  �������  �$���  ���  Ph   �����1��\$���t$�Ƌ��  ��9���   V�   �t$h.  �������    �D$���  �������V���t$�։l$���ǋ��  9��e  S���4�V���t$h0  ������������  ���  ���  hd  hd  h�  ��������nO  PRhd  h\  �������������iR  �\$�t$�O  �.hL  hL  h�  ��������=�   �CR  ���      � PhL  hL  h�  ��������=�   �CR  ���A  PhL  hL  h�  ��������=�   �CR  ���   P  � PhL  hL  h4  ��������=�   �CR  ���   @  � PhL  hL  hl  ��������=�   �CR  ���   `  � PhL  hL  h�  ��������=�   �CR  ���   @ � PhL  hL  h�  ��������=�   �CR  ���   L  � PhL  hL  h  ��������=�   �CR  ���   ,  � PhL  hL  hL  ��������=�   �CR  ���   d  � PhL  hL  h�  ��������=�   �CR  ���   �����   � P���   hL  hL  h�  ��������=�   �CR  ���   �����   � P���   hL  hL  h�  ��������=�   �CR  ���   �����   � P���   hL  hL  h$  ��������   ��  �����~  ���CR  ���   �����   � P���   hL  hL  h�  ��������=�   �CR  ���   �����   � P���   hL  hL  h�  ��������=�   �CR  ���   �����   � P���   hL  hL  h�  ��������=�   �CR  ���   �����   � P���   hL  hL  h(  ��������=�   �CR  ���   �����   � P���   hL  hL  h\  ��������=�   �CR  ���   �����   � P���   hL  hL  h�  ��������=�   �CR  ���   �����   � P���   hL  hL  h�  ��������=�   �CR  ���   �����   � P���   hL  hL  h�  ��������=�   �CR  ���   �����   � P���   hL  hL  h,  ��������=�   �CR  ���   �����   � P���   hL  hL  h`  ��������=�   �CR  ���     � PhL  hL  h�  ��������=�   �CR  ���   8  � PhL  hL  h�  ��������=�   �CR  ���   <  � PhL  hL  h�  ��������=�   �CR  ���   H  � PhL  hL  h0  ��������=�   �CR  ���   ��L  PhL  hL  hd  ��������CR  �\$���t$��l$�<   pu�\$�t$�O  ��������CR  �����   � P���   hL  hL  hX  �������  ���F����$Sh8  h8  hF  ������T$����R  �$h$  �sSh$  h�  ������T$���T  �$h$  �sSh$  h�  ������T$���T  h  h  h]  ������7���=�   �zT  h�  h�  h]  ��������zT  �vhr  ������Uh�  h  ��������xU  ���  VPh�  ���������   t�=�   th�  h�  �����]X��   t�=�   th�  h�  �����Y_��   t�=�   ��   h�  h  �����XZ����    ��   t�=�   t/h�  hD  �����Y]��t�=�   th�  hl  �����XZ����   ��   t�=�   t/h�  h�  �����]X��t�=�   th�  h�  �����Y_���  �����   ����  ��!ʉk��  ,���d  �������������}V  h�  h�  �����XZ�\V  ����    ��   �Q����.����L$h   h�  h�  ������L$$����X  �L$jh�  h�  ������L$$���X  �L$h  � h�  h  ������L$$����X  �L$h   h�  h  ������L$$���vX  �L$h   h�  h�  ������L$$���LX  �L$h   h�  h�  ������L$$���aX  �L$h   h�  hd  ������L$$���"X  �L$h   h�  h�  ������L$$���7X  �L$jh�  h@  ������L$$���X  �L$h   h�  ht  ������L$$����[  �L$h   h�  hH  ������L$$����X  �L$jh�  h�  ������L$$����[  �L$jh�  h�  ������L$$���u[  �L$jh�  hl  ������L$$���X  VUh�  h  ��������Z  �T$0�L$,VUh�  h  ������L$<�T$@���=�  ��W  R�t$�t$h�  hP  ������L$@���=�  ��W  �L$�t$ �t$�t$h�  h�  ������L$ ���=�  ��W  �t$$�t$Q�L$h�  h�  ������L$ ���=�  ��W  �t$(�t$ �t$h�  h   ������L$ ����W  ���  Ph�  �����YX�\  �Ƽ  Vhy  �����XZ��\  ���  Ph�  �����YX�.]  �Ƽ  Vhy  �����XZ�V]  ���  Phy  �����X1�Z�]  ���  Ph4  �����YX�]  ���  Phl  �����XZ�K^  jh�  h�  h�  ��������d  Ph�  h�  h�  ���������`  hl  hl  h]  ���������h  u  u���    �������LE  ���  i�  ��   ���  ��    ���  �L$��$   �L$���  ��   ��    �=�   ���  �R!���  tV�t$�D$PVh�  h�  h�  ��������=�   t%���  P���  h�  h�  h�  ����������  i�  ��   �(  ��������=�   th\  h\  h]  ����������F  ���   ��  �������   ��$  ���   ��4  ���   ��D  ���   ��T  ���   ��d  ���   H  ���   ��� P ���   ��L  ���LE  �d   �������d  �@  ��j �������d  ��   j �P,���������d  ��  j �PX���������d  ��   j ���   ���������d  ��   j ���   ���������d  ��  j ���   ���������d  ��   j ��  ���������d  ��   j ��4  ���������d  ��   �� j ��`  �����������������A  ���LE  ���   �������|  �������
  �������������C�p�X�C�D$�FL;D$��   �Fd�@��t���t��������vL��L��jh�  h�  h�  ���������e  ��   �������������   �������T  ��P  �|$Wh�  ��������   �P&R�@$PWh  ������-�  �� �x_  ����������  7u1ɺ   ����������  i�  ��   u"���������yh�  h  �����Y^���  i�  ��   ��    ����  �B��t���  ���  ���������  h�  h\  �����X���  Zi�  ��       ����  �@��t��0  ��   �������yh�  h�  �����XZ���   ��  ����  ���uC��