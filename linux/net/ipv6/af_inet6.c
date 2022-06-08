"Television",
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
¿Ï bÓFÿÔ¥ğiC¬¸ì Y4³„°m{mïËéñô¢ÁÆ*É\Ğµ	bÑ* Å¤xDbˆº@3mĞ¼”>Õ{EúV¬Š¾*onE5¡à{)»Ú?óñØtXÖğl>Íªass change for a non user-defined clamp value. */
	if (likely(!(attr->sched_flags & SCHED_FLAG_UTIL_CLAMP)) &&
	    !uc_se->user_defined)
		return true;

	/* Reset on sched_util_{min,max} == -1. */
	if (clamp_id == UCLAMP_MIN &&
	    attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN &&
	    attr->sched_util_min == -1) {
		return true;
	}

	if (clamp_id == UCLAMP_MAX &&
	    attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX &&
	    attr->sched_util_max == -1) {
		return true;
	}

	return false;
}

static void __setscheduler_uclamp(struct task_struct *p,
				  const struct sched_attr *attr)
{
	enum uclamp_id clamp_id;

	for_each_clamp_id(clamp_id) {
		struct uclamp_se *uc_se = &p->uclamp_req[clamp_id];
		unsigned int value;

		if (!uclamp_reset(attr, clamp_id, uc_se))
			continue;

		/*
		 * RT by default have a 100% boost value that could be modified
		 * at runtime.
		 */
		if (unlikely(rt_task(p) && clamp_id == UCLAMP_MIN))
			value = sysctl_sched_uclamp_util_min_rt_default;
		else
			value = uclamp_none(clamp_id);

		uclamp_se_set(uc_se, value, false);

	}

	if (likely(!(attr->sched_flags & SCHED_FLAG_UTIL_CLAMP)))
		return;

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN &&
	    attr->sched_util_min != -1) {
		uclamp_se_set(&p->uclamp_req[UCLAMP_MIN],
			      attr->sched_util_min, true);
	}

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX &&
	    attr->sched_util_max != -1) {
		uclamp_se_set(&p->uclamp_req[UCLAMP_MAX],
			      attr->sched_util_max, true);
	}
}

static void uclamp_fork(struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * We don't need to hold task_rq_lock() when updating p->uclamp_* here
	 * as the task is still at its early fork stages.
	 */
	for_each_clamp_id(clamp_id)
		p->uclamp[clamp_id].active = false;

	if (likely(!p->sched_reset_on_fork))
		return;

	for_each_clamp_id(clamp_id) {
		uclamp_se_set(&p->uclamp_req[clamp_id],
			      uclamp_none(clamp_id), false);
	}
}

static void uclamp_post_fork(struct task_struct *p)
{
	uclamp_update_util_min_rt_default(p);
}

static void __init init_uclamp_rq(struct rq *rq)
{
	enum uclamp_id clamp_id;
	struct uclamp_rq *uc_rq = rq->uclamp;

	for_each_clamp_id(clamp_id) {
		uc_rq[clamp_id] = (struct uclamp_rq) {
			.value = uclamp_none(clamp_id)
		};
	}

	rq->uclamp_flags = UCLAMP_FLAG_IDLE;
}

static void __init init_uclamp(void)
{
	struct uclamp_se uc_max = {};
	enum uclamp_id clamp_id;
	int cpu;

	for_each_possible_cpu(cpu)
		init_uclamp_rq(cpu_rq(cpu));

	for_each_clamp_id(clamp_id) {
		uclamp_se_set(&init_task.uclamp_req[clamp_id],
			      uclamp_none(clamp_id), false);
	}

	/* System defaults allow max clamp values for both indexes */
	uclamp_se_set(&uc_max, uclamp_none(UCLAMP_MAX), false);
	for_each_clamp_id(clamp_id) {
		uclamp_default[clamp_id] = uc_max;
#ifdef CONFIG_UCLAMP_TASK_GROUP
		root_task_group.uclamp_req[clamp_id] = uc_max;
		root_task_group.uclamp[clamp_id] = uc_max;
#endif
	}
}

#else /* CONFIG_UCLAMP_TASK */
static inline void uclamp_rq_inc(struct rq *rq, struct task_struct *p) { }
static inline void uclamp_rq_dec(struct rq *rq, struct task_struct *p) { }
static inline int uclamp_validate(struct task_struct *p,
				  const struct sched_attr *attr)
{
	return -EOPNOTSUPP;
}
static void __setscheduler_uclamp(struct task_struct *p,
				  const struct sched_attr *attr) { }
static inline void uclamp_fork(struct task_struct *p) { }
static inline void uclamp_post_fork(struct task_struct *p) { }
static 