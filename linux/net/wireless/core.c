D_DVB:
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        É3¾Ð%Žâ Uš ¦ îÛ£8b+ç¢ÁõšÍ¿2)4ö±p`xJ‡šj„Ï:ÅÉæÆžÖAÑ‰8>¿ääºÄ°•dÚÿÈŸRpØßs× ~ˆùM¨æ·Š&•ÔÞ­µÈls_êR±FÈ´íC‚Zàb&d·ëì#né¿ìEÓÊ|‰(Öé²`uÅÒ‹0¯Ãáß0‘07ð¥¹Ðâ)(Xt×é‘0@ö+æìKÿ­¥£®ø¤ò‰L+½NŽŽÒô´$ÞpñLB)sÍ›uq˜Dy
‚®Ä_•aÂì '4a5?XŽo×Hi4 á>¶¦òÞ°ÌQŽ¼Ž9P p°O¤M—¹…³gä£µc¨Áðƒ…Ÿt+tœyŸ[$¥,åEQ¾un¯5‹Àk.ÖùÄ¸ÌŠê  •ýÿ1éÂøÌt]#‡a1õ<Lƒ³d‡)¡‰ƒ †p*ð  juÃÀl€—Ù.¼*@“ò¯µÑ1Tc0ONÿ,ãn ZQã›HEkóz²âÄC#¬ÉzQ†óÛXmtÓyIq`STæ«‘i;leåa&µ	ßy‡îCßNb³ŒsršùÇ‘Ì¼ø˜¦ç“±gýj`X{›¡ð¡Ñg6li èlôDH.8 ò×ôfèÛý]Ž,ãSªëÍÛnÞï
W²¹Oá¼ý¹Ë9ä#JÑ ƒÞ×Ø­Kl'ËY‚¯ómq”³ÃÌðÃ
=n^’8<Í<ÀHied7W"oÏÊ0ÝwªU.ÃêóO¥R­°Â™ì_#¥K‰`^aÏ y>Ð¬¸Qi+Pk4 ªµšF[²¼M°
o–±›±Š‰’Ÿ¨oý`ã²Xx’'û+G8Ó+žÔjYs<ÈÚaÔâwm’B/8ÊÊ1GÃšŽ@
ÍÞ1is­[%º4?ŸlÍc8Y4ÀÙ”¼f:p;§_ÙŽX>³`‘> ‹²Z0“."Çü…†=KÚÎ)=iT"ƒÁ’ñ9û\–2€ "4ñHœ·5Y pþ)ÎåeËwðCÇ#õ±xéû}£§=zÙÍEè².ó_‰ÏßÈåXÃ‡”û¦}ˆðE´`‚WAÜ¼hRd&YØõ®ÌtÅ4ãLphªvÊÇñ=å *ÓÑ41¦€½§è°mÞ¤ô5rFX?9çó´‹c}§‘€J³"nÇùíníï1»°ÑÞZú,ƒÓÅÁ&yÕ|Â¸Œ…Fluafy%ÏoÄ0±É”2n#ñè-²™?¢ôÎÒeÛÝ<¡Ê[ ÒHsÅšb~nàÊTÀöÇ¨i l€—õÍuÙ_CÍÎ#±×ù©ZºþSZZ.ÆW·dx8¿9I[‘T`†¹°u’Ä *<)¹ØÌ‘ËìnWËíEÜ”#cÎ4þ%H>¦å†#’Âf5Éúþ‚[Ž>{¶ ³ZÀAom=À¿ðP¾¯ºÖ2~iÌ +Ò-¬: ‡_C—6¨P	eü‰|¯âá-d:‹
»&`ürÓ'ëªß?p™ÿ«S†²‚Ú{ñ0|1Ž–^ëêÝ¿ x«µ-Š»‚,¸^>Ø	ö
ÏÞ9
rç¦gtÔÖö^EuØg5|zû]¿ñÏdqï{Fº" Šï…^€ËZUOn­Vªí±({ìDqx ÍÐÄˆ³ØiØœGT˜ ~ã…ØW(«°µ$§µìùØ›¿.I6Ç!Œš\
åh^yÛ¡U;á<fÔ=Âê’8féýrnÐ¨ÿA‹Í›ÙÒÂQÐ¾ÎqXl-l{„3
€´™YqÅË=²~YÒèïí°ß½ÑJÙÇÖ¸tž‹y,~Døë"E¥»Ý„Ã/á—³Öš\jÂ.–änÍjÖ·eH™”ïÞKI—oCÄŒðeGVoWº'?÷OK¹¸±
Š–×^gHžl"Sýá
(ÿ›‘\ˆ´qQºlÆ‹áÑ­É$é|âT‚þöá¿9ÞåùÒ^®_Â†ÁÐA²û/¦KwU‘F­=úÿ'¤ùîSßÂkŠ×³W“#»KÂ¯}é“JûŽKI‡gÙ0ÈR²ª?ªîd9œr‡ý¿Õ²XD°FïÔÿç²ç+`F‰ú>ÿ,ƒ9·øAÃ÷ÿgØ
E´–j#óÄ”çk{á¯¾lKi5˜/ýºYlôhÉ²˜_qkùí‘<‰› ½Ø0]ˆ¶NÁ¡ÎhßµÒ,Tâ“=•{Ú/\“zå>žsè‡S»h_¹GãZÑo‘Mf ¿œ8qœè ´‰Üu¸?„ì¡¹5W%ñþj-ìøµ}O“;£Žøo{_‘";2'ÔêªâXP¯áå¾sG^—xÓ²ãîøcžÌpvÀ™Ð%äÀL’ŽÍhy'ø*WDƒðØyÄr±‡k}û$LÏòh-Œ+qýáÆ”Iï3vÔÈò/›á {ïóì‚æãVc7î¾¾ÌoPNT\ÐÙvA€"˜ôî5"’¾Wuó;ybàØ-VÏ${Ãs¹yzÜ¡êQù­ÔÞ¿Ë2?é kš?êm‹ô'×é„2Ý!û	‰Ñª+žc’1‘Ê;íœ?XŒD'%X‰ð²¼  ixÊéVé`ÇËé”â¸™G0s·gvÜîl®ìJ½È$²y/F%'b?Étä3çÖ~ºÓÊ0·Å€7m5Ÿ©ð2 F0!`4d«(üðf@½`ÿDàÂýq…e™Û•Bíõ{ÊËö“u‚’.ò	áÜÕíž¬2!Èø#UÖ6ò2aÑLyf$€H—Zbyù&æ,Á£Ž_­Œm’X>JÁü¤¯«]gÇ‰u¢S’€4U;~lŒkS]ÅÇ^€bÈV=‚	U½™cÑz¸”Æm¼CóÓ†Õ8¾çõ7‚–ƒ~ägŽ­Ææ yy[ˆ óŒˆ‹P-Bn,”€¡ñiŠl‘Ñ=¶©ª?÷Â”ÂüÐ«¬4k¡žè>4n”? '‰E°Åö@d`Ã»Åwµ©öï¥}D!-Z¡]ÌŽ,”ØÀ=ñ†_² PõÍŠŠh3«!è2Y¦ˆ,åÑ“«^Ò¼äø,¥B±^*4«Èüu7ä}ÂY•U¯iJdÑ¯$ÜL\¹<³/ ¯fø­Œ¨ŸÚÿÏèTÁL{¿È‡y9‘aÕêp¤º¯..Äqq1<­Üå[ ¶r;ËR¬ò ØüSh¡˜÷ÌÄ*<yFä˜ñ1ØQ£#Å…‹ÎœÊ~ý¦Ñà4–î^y¨$¿
 ~ôªÆ½PTÉ„Ó¦šÆßQ¯ð¼Ú^½,zM2þœ¥zBRß(ƒ=o4Uº´U"®ã@þ1å˜IÊjËdé1é…>®VR¯5›»âÍï}oQ
ËzoTÝáýÙþ¾@›:¶ë‚¶1nPwFbHqL7ž¬ók;¾K‘1p?p¹F,,Tútx`ål'ˆ¦êIx özÌ8Î“{2mÓûc ‹MÐœQøº„hðÎø/6B©{qÏçÀãó£
af•
:pÌlô^2©¸&zráÃœ:‘8N•Ó‡–Õ[‚Í&‰1gFc÷Ó6kïë¡÷~¶P•@ˆ#$sT‘’Êëy/Š`.Æñ˜Vó—Íø
0‚
5©Ê"‡ª·„Ræ¸Ö]X(R¶…I’ßjâïhàŸ…oE´™ëO!¡Çv	¦1hÃÛ5¾5Â0ã[f­jˆŒ|cÛ3.8"KÕil×$ ¸	(ÌºæßGVpÎßÖ)ð;ûxfe‡±0ÖÿëÌæ¦Jƒ"xÜÅŠ³b-î§…@XbÍ­ªùÄ¿ÕýG¢€FuÞ\ˆnáxº—ÇÓ³%½Õô Š_8C£4ÔJšbs5¸_¼më<†7;:`›”ÝÃTÇ)ÍÏÁrsíO¬1 ¸ü
é¤ègÈºÁŸ/DXê«P‡ÛÃ>ëõé2ÇÅ™|"•›ž¶$±Që½­èâ½‘¼dp‚B¶?Bd+7ˆlÜD`Æy†n³¶µ=RœPl3Å¾aÚêvÍË]ÜÊ¥[ÔÒ/ž‹ß¨ säÙÌ7t”dïŠ-ü*káõ…xÍK¥voœÖá© É',@ÿ÷·j.€“wa1hÂ1$
ÛVZØòhcé0õO/Äalá]I0ëÝ76©9÷Åbc8¾¾%LW6Ä¬	Ÿ¼,6ö`œÝÚ^b[æ"Ù<6½¿Š»»r«úÌ°!üH¯“ñ'EZñÚL‡¥Ðþ9Í˜ÆM4ÜôâÌÅ7AP¢vÉçÈ¸Ä,¸dt†¢mžÙ^òê‚äHÐ™ïäÉr¶ˆž9aî,ä/…lÙS2ç¼&Í<OV$fèt˜0¯‘VOuÃö±ásftç/p‡§†Š¼;Z‡+eN>jÃûH.‡•ÓWR!†.Œ‚Æï®Òÿz‰ãR½Šý€bþú]“èQU’)c§ÞteK…N»Î>^„€ºÚJ^®ÌÜ}pµ±&õ ñ,è—Ò¥íì‘5Ü5’k$–Ãåï»×Ìc<Üv-r"=½W[(Ý"³‡îjÂ?žª8‰Ô`§›‘ ë¢µ9ÉOœ„>ñÏë@ÝEñú=;¯ñxBÛUªˆOgÓu›³gØ_ú²FÛž®z»9á¶=cT×=ìkxA\³Òé¥¾®]â•ÕÆØ8\)@ßêTüI‚´v—Çäº^`24¬kB`äL52o;’¤á‚xžnø^C|&Ò§—Î4¿XÁ39§ØÂ-Ë{é åÇ^ö.ª×{ÖBÈØ*IÐ˜Dî½M<:å)ò\ŠTrbè@zêh¥Uoý%ž5-'jpv‡½¬†‰§_ž7Ø:‹Uá[rŠ¯Âó²†Àá©IUjêÙO&õ—úvÍxˆÛÀ:p  &ŸÞÕÅÞ´L@Ý?HA|ƒ†¤ÂÃ!LØ+·ü›’Œ“ø¬´ŽøéyÉ8
…BÓ÷ù+%†|DÒP5‡6J³-ÙYQûóÔœç’½…ëùÄ#îü_/…ÚRCÊÇR‚„Ú_6]‚»:RSAMô _1^¾]r!¯nC]¿¯ <LbÄè%±‘ËïyÜnœ¶çeq!wBžl€×jô“¶5«°öð.ÔHáçñ"p«û¹HXÞ_ÖH èK˜&Ê.–ª,¥,:öúqH\†ß«:Ðã!v/«-’i•ã æŽ-­Æ[”/ˆ] ¨—w)"¬ØEºHêöb¯&¥–úáw$ŸÎŽ¢žêˆ‰qÿxN/±A”ºö@ÌÔ’JòèH´Aå_­¤³‹‘ðÒ/=¸mé|øj1„p5~Ý‡:éõ‡E;oÐå"Ó\Ñš’ÛºÆ¥í8GñLÁ*0÷Ù|Ãözò1
œýç!Zxœ‘«Pa¹j“I7»’ä9î3Aßj©ð#~OÈ—³[G<æ]÷Š@ƒ`ñ•?eËhzœµ$=ìµ §-¥LóFÄ“ÉIb€fR‹1°~bºÿªÖ˜Zqoçôn9ZeT=ÍŠá)á¬ÑCÚ"Óßì.)p%¦8ðÓö „\W‰§·’îJmå§•¹A-­§=,E_-ÐÜ€?Åub1ê¸2{Ùê¡iuáë	“í—ÈstP%kki·ÁLÁËr¦¼±^'Â0÷›@.ÏÃ
ÊüE[%j­³O‘´× °ÊE€°
%å˜µ$ÎnWñåÊÕ”;ê'Á=(˜¤\û_lOuïú›˜#ß(×(8jbô~Ä8Û]GKÚ÷ê:òñ+]f¬µ÷”³=HFù¤±ÛjgÓ>Ls7#Žé{Ò¯×^ÁâêœÎÐn:+‰káb2žbˆìC¿#Ê;çV2r¯àhaÄ …ß¥h£¾üý.<&å .š_v±³0a$){™ vp¢ð A×ö£å©7ÄèihýCµì)jYyKöNÕõ|ˆÿZBÎÿâtæØHOXïƒÇi×FêDÇèŒßªçÈ|Ôj¨ríøv´”fô$ËÓ;çÊÑƒ­X“,.ë¬:eOî0yÚ¾‡w‹rëK3Ê…Íò:½/¼·tå×!(ýÀˆkHÄ¸ª›WÖR@t^ž§ø}„$u p.„Â’ÂæztŸéÖË1£¥~j‰hžÇ‰úDŸ›$]Áþ¬›Êð”DHÿš>U1õl0QçÃû&ïÍ=0ó³¢„Nß’dV7qhÓ@ÝÍÀ¡uâ pY·gèÑ~LÎIÕt —‘À•àå¾³¥­—›Š>eó–\´Ès˜V¬8AíÊA×ŽÞê®ž:ŠSÑ¸[X: Y¢ÉÿUÝG¤jÈ[¢üi‰¾ƒÂ*žÐÔÄå¾ ©YbQ}­ó,ÀônõL	àõÝ«88ßÙ["€Û~e¾f{=TŽöU9Q¸=ÖˆF²Ü“:4B •änÐcx2&;°ƒ”žËýÂ%:¸â>Ã.[çÀw›@»ò'ª¢s×c±^…€ØÝ§J€¿Ö¤…÷¡œbÔï-Gü&x% ³j™`ÎÚ9F½.ãÝî"‹~Áè‘óÜÎB»•¦¢ÕC¬XVáãÆÚmîÄàû²×'/,]—§¹µ‰×ûœ”‘7ov¦Øg/i»]ÞØª½“•y(9MzŽ÷¦Q;øðï‚qè Å(rŠ½ÖºÌ¨.A@Ö¢#çYZþñ‰¶˜;aÈs Õ'‚\?=ïw[ÄÅFÔìV •ùvd•õåŸš”)¨ºŠûŠJýb¦1Ð5(»½¹d*ûGÑÃš#™tW36*‡Ñþ*•UN!u½ù‚2Í0eT¦4uökÙrQ¨òså`“|˜b§1ÙúIº?m{PømZváÖìÖv­˜¸ËƒaTì¢^Œdg¼áâ@?ª‡4C*·43I²cLh/·¶ÛõT9ùEPWä‚BŸ¡°8!¿ŠN•g	¶«»Ð{­®‡D%K©” *hœB˜Ã øÐSz¸=üÄ ’ˆ–ìßã‡¢YEžðà‘÷'sY—DÜc.ÁuøÒ5*ÎIâãK_‚\ÉT£}{¸	õ¿gÍl¾‡âqºtÿº÷Q}@À,ÇÔ¢ÍÛRß¯ÙÍ¢-×&à™˜Ý­©¬6‡NåÞ
M§ÅÔÃÓ¼j²Z›³”Ð•oûZs©t>òd0NÍ/V‰
 Ixž{ær;utÊP„nJ&}ÐŠgP˜=“QÁ'¦]#ŒŸ¡8ˆ¥U’SÙP9áB…ïƒM˜‡€îPÑUPóH
€ö€ä“²îB‹­SÚÐ’€(ÎðÇSûÝKEy5;•çx˜Â,‘Ö!!îŽŒÙZI:‹ãéüÃ­uäL¦óê©¼3oßNœãõ‘·‡7å·@èZ¿™Ô/*9	çxØÀ˜ªs]¸ß¨“<,Ö”…4Èü¸MÙH´{/*ô²Ñ	@>Æµwm&J_‹Bä ²rK£D¾2˜Ì‹Ü!ÉÃ3ÖDÕöd¢oÞ`õxsM†pÒ¢_È÷Û¦à6T»‡3ü3…¡`JA—yZà/Rjä†pÁÁ·\bÊEƒ´–ñìyn oÚ^P¶1µ°Ñh¹c»ÿ*hÄ&ÀlÐH`K)ê´²ª‹: :•©û·$tuÿ€ûIo‘û¦4‰^5XUZ"Zº÷c;]®ÖÉêS
Í}ÛÊ¦€¦¥‡w
fb/6TÜ)pÙVàû%¢œ©žUÄe202½§%â—Ë®Ag'é(+‰OFÙŒTXð¸îL%ñ?°mÜfˆŽ‡L¹>´ÿî:Û÷)»edgçßwÄÐBlêgNâ„rn^O"#æFIïcç\¹S$I¡lZÀÍ‚«ü’³7¹ìîà´i’~ÙW,!—ð¶ø7òãàzëS_"ƒôÎ‚ŽZÅŽ.¬ïöÑ¥¸µ_§Œ³#µf9ÓY 5Å<”Ü¼mÚá2YZpŒm%NÄøÏÆB¦{tuöHœÂánkL³”€š²òè€F
®ˆ%Þ÷ÃØ˜1‹Ð8h¡Î0|.Òw€ÿ¼E¯Eˆ’€ÊQÄž£Æ[óÁÃ~›Hå°Ós1ˆ"siJÉäÃçµ‹]Ó¦$FszèK\òidi¶¡ZµôÓ®zûpô y˜ù ç\P¨Õ¼ZçA‡{`VŽñÏ]–~‡?„”QõL³ü¯ä¾ã(ÊKñt^s|£[eë¿žuyÌ\#ºÑ›"xD³û)Ó\ûVhÂ7	Þ[¬ã¾<`¢‡DÔR± kydN_MVO$s­1¬Ú/8<zvû8¡Šnì´P,v^šÌ
{MÎÔPRßä}æÛb[VÏO>ö
Ç#‹÷w×¡ß(´‹ÚÞÛÇÏó6÷¥ð±<…H`ç‡<´Øµ`ÓŠÓ=„ÿ 0W>Ño`µàQÉC =¿îÆÈÃ§èCæV.¸§áOÿO´]ò¦'GÚç-òÚ•·”çgOãÒƒ‰ïN_±¥"öa
Lâ©7<ì}¡'#ŽÉ¶ôÛ€\RÒ_Ö9—&ðÊiÈ©Épò™A Ö	 ²ÕüÇVƒ%4´JhôìQ0 ù#Äj(I´Â‹ã	#àñæ¬ûJÕDZ* (†ïÂÆA=Ÿ”.‡¡¾AàÁä‡Œ¦}Gu’*b]á©*–Ç‰0|ÇèÇ ÔÿÄ¬'Ò5@s§q)ãõö©|o=s-ãªrM=ˆ©¹ÞÉìBŸzï ¾ë]p´ÑF9]	[¶ ¦{îeã—5[J©ršL@ÒÐš‚¬Ìh]ýÒo [$„¡ûÊz¡š¾ˆÖ:Íƒº5]Æ¶à¼h¦¨BW(ý™çˆ2(RÃe:{‚rr÷H¸·P PóAüc»•XÂ «÷ÙXÍ†ÇÁl‡s¡Â¥•h
-”þÏ§(„Ry÷|Ì9Œùò­Ùsfü˜Õ]Üh”á«öþ‚§Ù
ã_n†ò~±GõŠýF—igÁ™®ß`x;¼’›ª–JL›8úá³¾o¯
\‘Í·øÑ´0¡+T^;ûsSÜ*¡Òwö<ð?ÀËUY†Zä±æÓèXÙ»ä9½^-ÈŸ±4—B—š–hñ}	ÞÕ|Á¹ÙÛrÁÒ¶¦ùt™a/·”ÒH9úàn½-ëòÉà\RÞ=Bu[¾T±EälN3õ=zkWpÑ$!š!-’Î\f¸Çß5ÂU8´†r†fpº.ë±#–cG Ä ƒ7
M	FŽ[7¢æEç(GŠÈE·.q:u˜%ÑÃ
;Ÿ‹±-§­ë›ŒŠ vÉµ–~ƒá3ïÏð\¶QV"ÚmÎÌÙ=Ù…ú“Ä8Hqä¡CjuµWìZè)ƒl^ÿ¡¿ÅþL%ã¼†ÛhEêºbCFC…øJë1‰Ø¼´°Ô['%‹.f¾³a_”6±„Í–áM7pÀ6ÚAc$eè¡x®HOF1ÓpaþÑæ|QPžÌ%¨ÿÍŒÒ~ã?ÁÏ½é†˜Ê˜pîÌpŠ/¡Âç#ïu™ÌÛnb’’?¢·b¯Dpxw§=¬Y¶¯ÐISÜ/HÖ³"ã2 "{^	ìÕu0ÆF²,Œ&|qÜgà¾®¡Ó[Î	dcû4“m÷J
P‹©‡!ºpmL	R©±u";·ÉðN°xÚÐ:\Ê	à¶Ý¯­B ØJfÚ`ÀŸP«FÁÖâÅã¬/&ÁÒIjx€Û"Ø[‡]ÍkàÚdìý&}Ç7SpZPnZƒ E<”Vf‚š°pZ«Óƒ(XÄ—mØFtRAUÒ‹rO«Gï8°)ÖX'^hò¹»X¼cÖ:o{ËÝ ü@&Rn²ÀC j\ÛQnl*()s‡HƒyÔËoQ.¢â‡Äš3Xt1ìyeWÓõ7Kh;îóÂGÅ{Ô"©mâûÉ%ã>FJù^¨m”Vü
yÂê7¶«Päêbóa1II4ý‡7]ÿ"B¯çØXâ@J)Üd«pþÊGR$ÕÃÝÞìÛƒÔ®ÚþŽdéïµfß*œ—9)hwvâ½sU>Ú«„Ý¤g´¤4ªþÚòUXÒ¼lQœFÆð„«e^^¢ÙÙ È*ë²ÎZ³/Gñ(<@©> /i6zY»aàŒ¯737yjV–[ œéyÄîZVgÜÛÐù¨üwœåÁúäáÚ«º’jŠuœŸÌFo¡ZàI&jWÚæ'•8ÏëÇµr¢xL[ó~³ã^	Bj]ÌGíOä1§|·Ì£Èæ›lê¯ß¡Ÿå·<oAU®äaš‹ ­’œÎ¹<£tyô»^„X8öxð†¥B¥z.Ò†§ Í^
8½ç
aÆÚ[Ô `›)¦uç9’Ž²0ÊOqc}:ŸõìMQŒ00ùy÷ÿË–@Ñ;×SÜ„ê×9 ÃNŽÀ²òƒ˜g²O…Ô‡xG4k;ín•iãT&îVÍS³†õ™Däà2ûŸîV\Ì]+*š]wŠ˜š«Ÿ%~‘üÛƒ£’&¦µ§÷$†ÿIˆVPwrXeãÖõ-— Ûž "¶µ…y~u?@YüþŒŸÚ
¿Ï bÓFÿÔ¥ðiC¬¸ì Y4³„°m{mïËéñô¢ÁÆ*É\Ðµ	bÑ* Å¤xDbˆº@3mÐ¼”>Õ{EúV¬Š¾*onE5¡à{)»Ú?óñØtXÖðl>Íªass change for a non user-defined clamp value. */
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
static inline void init_uclamp(void) { }
#endif /* CONFIG_UCLAMP_TASK */

bool sched_task_on_rq(struct task_struct *p)
{
	return task_on_rq_queued(p);
}

unsigned long get_wchan(struct task_struct *p)
{
	unsigned long ip = 0;
	unsigned int state;

	if (!p || p == current)
		return 0;

	/* Only get wchan if task is blocked and we can keep it that way. */
	raw_spin_lock_irq(&p->pi_lock);
	state = READ_ONCE(p->__state);
	smp_rmb(); /* see try_to_wake_up() */
	if (state != TASK_RUNNING && state != TASK_WAKING && !p->on_rq)
		ip = __get_wchan(p);
	raw_spin_unlock_irq(&p->pi_lock);

	return ip;
}

static inline void enqueue_task(struct rq *rq, struct task_struct *p, int flags)
{
	if (!(flags & ENQUEUE_NOCLOCK))
		update_rq_clock(rq);

	if (!(flags & ENQUEUE_RESTORE)) {
		sched_info_enqueue(rq, p);
		psi_enqueue(p, flags & ENQUEUE_WAKEUP);
	}

	uclamp_rq_inc(rq, p);
	p->sched_class->enqueue_task(rq, p, flags);

	if (sched_core_enabled(rq))
		sched_core_enqueue(rq, p);
}

static inline void dequeue_task(struct rq *rq, struct task_struct *p, int flags)
{
	if (sched_core_enabled(rq))
		sched_core_dequeue(rq, p, flags);

	if (!(flags & DEQUEUE_NOCLOCK))
		update_rq_clock(rq);

	if (!(flags & DEQUEUE_SAVE)) {
		sched_info_dequeue(rq, p);
		psi_dequeue(p, flags & DEQUEUE_SLEEP);
	}

	uclamp_rq_dec(rq, p);
	p->sched_class->dequeue_task(rq, p, flags);
}

void activate_task(struct rq *rq, struct task_struct *p, int flags)
{
	enqueue_task(rq, p, flags);

	p->on_rq = TASK_ON_RQ_QUEUED;
}

void deactivate_task(struct rq *rq, struct task_struct *p, int flags)
{
	p->on_rq = (flags & DEQUEUE_SLEEP) ? 0 : TASK_ON_RQ_MIGRATING;

	dequeue_task(rq, p, flags);
}

static inline int __normal_prio(int policy, int rt_prio, int nice)
{
	int prio;

	if (dl_policy(policy))
		prio = MAX_DL_PRIO - 1;
	else if (rt_policy(policy))
		prio = MAX_RT_PRIO - 1 - rt_prio;
	else
		prio = NICE_TO_PRIO(nice);

	return prio;
}

/*
 * Calculate the expected normal priority: i.e. priority
 * without taking RT-inheritance into account. Might be
 * boosted by interactivity modifiers. Changes upon fork,
 * setprio syscalls, and whenever the interactivity
 * estimator recalculates.
 */
static inline int normal_prio(struct task_struct *p)
{
	return __normal_prio(p->policy, p->rt_priority, PRIO_TO_NICE(p->static_prio));
}

/*
 * Calculate the current priority, i.e. the priority
 * taken into account by the scheduler. This value might
 * be boosted by RT tasks, or might be boosted by
 * interactivity modifiers. Will be RT if the task got
 * RT-boosted. If not then it returns p->normal_prio.
 */
static int effective_prio(struct task_struct *p)
{
	p->normal_prio = normal_prio(p);
	/*
	 * If we are RT tasks or we were boosted to RT priority,
	 * keep the priority unchanged. Otherwise, update priority
	 * to the normal priority:
	 */
	if (!rt_prio(p->prio))
		return p->normal_prio;
	return p->prio;
}

/**
 * task_curr - is this task currently executing on a CPU?
 * @p: the task in question.
 *
 * Return: 1 if the task is currently executing. 0 otherwise.
 */
inline int task_curr(const struct task_struct *p)
{
	return cpu_curr(task_cpu(p)) == p;
}

/*
 * switched_from, switched_to and prio_changed must _NOT_ drop rq->lock,
 * use the balance_callback list if you want balancing.
 *
 * this means any call to check_class_changed() must be followed by a call to
 * balance_callback().
 */
static inline void check_class_changed(struct rq *rq, struct task_struct *p,
				       const struct sched_class *prev_class,
				       int oldprio)
{
	if (prev_class != p->sched_class) {
		if (prev_class->switched_from)
			prev_class->switched_from(rq, p);

		p->sched_class->switched_to(rq, p);
	} else if (oldprio != p->prio || dl_task(p))
		p->sched_class->prio_changed(rq, p, oldprio);
}

void check_preempt_curr(struct rq *rq, struct task_struct *p, int flags)
{
	if (p->sched_class == rq->curr->sched_class)
		rq->curr->sched_class->check_preempt_curr(rq, p, flags);
	else if (p->sched_class > rq->curr->sched_class)
		resched_curr(rq);

	/*
	 * A queue event has occurred, and we're going to schedule.  In
	 * this case, we can save a useless back to back clock update.
	 */
	if (task_on_rq_queued(rq->curr) && test_tsk_need_resched(rq->curr))
		rq_clock_skip_update(rq);
}

#ifdef CONFIG_SMP

static void
__do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32 flags);

static int __set_cpus_allowed_ptr(struct task_struct *p,
				  const struct cpumask *new_mask,
				  u32 flags);

static void migrate_disable_switch(struct rq *rq, struct task_struct *p)
{
	if (likely(!p->migration_disabled))
		return;

	if (p->cpus_ptr != &p->cpus_mask)
		return;

	/*
	 * Violates locking rules! see comment in __do_set_cpus_allowed().
	 */
	__do_set_cpus_allowed(p, cpumask_of(rq->cpu), SCA_MIGRATE_DISABLE);
}

void migrate_disable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled) {
		p->migration_disabled++;
		return;
	}

	preempt_disable();
	this_rq()->nr_pinned++;
	p->migration_disabled = 1;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_disable);

void migrate_enable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled > 1) {
		p->migration_disabled--;
		return;
	}

	if (WARN_ON_ONCE(!p->migration_disabled))
		return;

	/*
	 * Ensure stop_task runs either before or after this, and that
	 * __set_cpus_allowed_ptr(SCA_MIGRATE_ENABLE) doesn't schedule().
	 */
	preempt_disable();
	if (p->cpus_ptr != &p->cpus_mask)
		__set_cpus_allowed_ptr(p, &p->cpus_mask, SCA_MIGRATE_ENABLE);
	/*
	 * Mustn't clear migration_disabled() until cpus_ptr points back at the
	 * regular cpus_mask, otherwise things that race (eg.
	 * select_fallback_rq) get confused.
	 */
	barrier();
	p->migration_disabled = 0;
	this_rq()->nr_pinned--;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_enable);

static inline bool rq_has_pinned_tasks(struct rq *rq)
{
	return rq->nr_pinned;
}

/*
 * Per-CPU kthreads are allowed to run on !active && online CPUs, see
 * __set_cpus_allowed_ptr() and select_fallback_rq().
 */
static inline bool is_cpu_allowed(struct task_struct *p, int cpu)
{
	/* When not in the task's cpumask, no point in looking further. */
	if (!cpumask_test_cpu(cpu, p->cpus_ptr))
		return false;

	/* migrate_disabled() must be allowed to finish. */
	if (is_migration_disabled(p))
		return cpu_online(cpu);

	/* Non kernel threads are not allowed during either online or offline. */
	if (!(p->flags & PF_KTHREAD))
		return cpu_active(cpu) && task_cpu_possible(cpu, p);

	/* KTHREAD_IS_PER_CPU is always allowed. */
	if (kthread_is_per_cpu(p))
		return cpu_online(cpu);

	/* Regular kernel threads don't get to stay during offline. */
	if (cpu_dying(cpu))
		return false;

	/* But are allowed during online. */
	return cpu_online(cpu);
}

/*
 * This is how migration works:
 *
 * 1) we invoke migration_cpu_stop() on the target CPU using
 *    stop_one_cpu().
 * 2) stopper starts to run (implicitly forcing the migrated thread
 *    off the CPU)
 * 3) it checks whether the migrated task is still in the wrong runqueue.
 * 4) if it's in the wrong runqueue then the migration thread removes
 *    it and puts it into the right queue.
 * 5) stopper completes and stop_one_cpu() returns and the migration
 *    is done.
 */

/*
 * move_queued_task - move a queued task to new rq.
 *
 * Returns (locked) new rq. Old rq's lock is released.
 */
static struct rq *move_queued_task(struct rq *rq, struct rq_flags *rf,
				   struct task_struct *p, int new_cpu)
{
	lockdep_assert_rq_held(rq);

	deactivate_task(rq, p, DEQUEUE_NOCLOCK);
	set_task_cpu(p, new_cpu);
	rq_unlock(rq, rf);

	rq = cpu_rq(new_cpu);

	rq_lock(rq, rf);
	BUG_ON(task_cpu(p) != new_cpu);
	activate_task(rq, p, 0);
	check_preempt_curr(rq, p, 0);

	return rq;
}

struct migration_arg {
	struct task_struct		*task;
	int				dest_cpu;
	struct set_affinity_pending	*pending;
};

/*
 * @refs: number of wait_for_completion()
 * @stop_pending: is @stop_work in use
 */
struct set_affinity_pending {
	refcount_t		refs;
	unsigned int		stop_pending;
	struct completion	done;
	struct cpu_stop_work	stop_work;
	struct migration_arg	arg;
};

/*
 * Move (not current) task off this CPU, onto the destination CPU. We're doing
 * this because either it can't run here any more (set_cpus_allowed()
 * away from this CPU, or CPU going down), or because we're
 * attempting to rebalance this task on exec (sched_exec).
 *
 * So we race with normal scheduler movements, but that's OK, as long
 * as the task is no longer on this CPU.
 */
static struct rq *__migrate_task(struct rq *rq, struct rq_flags *rf,
				 struct task_struct *p, int dest_cpu)
{
	/* Affinity changed (again). */
	if (!is_cpu_allowed(p, dest_cpu))
		return rq;

	update_rq_clock(rq);
	rq = move_queued_task(rq, rf, p, dest_cpu);

	return rq;
}

/*
 * migration_cpu_stop - this will be executed by a highprio stopper thread
 * and performs thread migration by bumping thread off CPU then
 * 'pushing' onto another runqueue.
 */
static int migration_cpu_stop(void *data)
{
	struct migration_arg *arg = data;
	struct set_affinity_pending *pending = arg->pending;
	struct task_struct *p = arg->task;
	struct rq *rq = this_rq();
	bool complete = false;
	struct rq_flags rf;

	/*
	 * The original target CPU might have gone down and we might
	 * be on another CPU but it doesn't matter.
	 */
	local_irq_save(rf.flags);
	/*
	 * We need to explicitly wake pending tasks before running
	 * __migrate_task() such that we will not miss enforcing cpus_ptr
	 * during wakeups, see set_cpus_allowed_ptr()'s TASK_WAKING test.
	 */
	flush_smp_call_function_from_idle();

	raw_spin_lock(&p->pi_lock);
	rq_lock(rq, &rf);

	/*
	 * If we were passed a pending, then ->stop_pending was set, thus
	 * p->migration_pending must have remained stable.
	 */
	WARN_ON_ONCE(pending && pending != p->migration_pending);

	/*
	 * If task_rq(p) != rq, it cannot be migrated here, because we're
	 * holding rq->lock, if p->on_rq == 0 it cannot get enqueued because
	 * we're holding p->pi_lock.
	 */
	if (task_rq(p) == rq) {
		if (is_migration_disabled(p))
			goto out;

		if (pending) {
			p->migration_pending = NULL;
			complete = true;

			if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask))
				goto out;
		}

		if (task_on_rq_queued(p))
			rq = __migrate_task(rq, &rf, p, arg->dest_cpu);
		else
			p->wake_cpu = arg->dest_cpu;

		/*
		 * XXX __migrate_task() can fail, at which point we might end
		 * up running on a dodgy CPU, AFAICT this can only happen
		 * during CPU hotplug, at which point we'll get pushed out
		 * anyway, so it's probably not a big deal.
		 */

	} else if (pending) {
		/*
		 * This happens when we get migrated between migrate_enable()'s
		 * preempt_enable() and scheduling the stopper task. At that
		 * point we're a regular task again and not current anymore.
		 *
		 * A !PREEMPT kernel has a giant hole here, which makes it far
		 * more likely.
		 */

		/*
		 * The task moved before the stopper got to run. We're holding
		 * ->pi_lock, so the allowed mask is stable - if it got
		 * somewhere allowed, we're done.
		 */
		if (cpumask_test_cpu(task_cpu(p), p->cpus_ptr)) {
			p->migration_pending = NULL;
			complete = true;
			goto out;
		}

		/*
		 * When migrate_enable() hits a rq mis-match we can't reliably
		 * determine is_migration_disabled() and so have to chase after
		 * it.
		 */
		WARN_ON_ONCE(!pending->stop_pending);
		task_rq_unlock(rq, p, &rf);
		stop_one_cpu_nowait(task_cpu(p), migration_cpu_stop,
				    &pending->arg, &pending->stop_work);
		return 0;
	}
out:
	if (pending)
		pending->stop_pending = false;
	task_rq_unlock(rq, p, &rf);

	if (complete)
		complete_all(&pending->done);

	return 0;
}

int push_cpu_stop(void *arg)
{
	struct rq *lowest_rq = NULL, *rq = this_rq();
	struct task_struct *p = arg;

	raw_spin_lock_irq(&p->pi_lock);
	raw_spin_rq_lock(rq);

	if (task_rq(p) != rq)
		goto out_unlock;

	if (is_migration_disabled(p)) {
		p->migration_flags |= MDF_PUSH;
		goto out_unlock;
	}

	p->migration_flags &= ~MDF_PUSH;

	if (p->sched_class->find_lock_rq)
		lowest_rq = p->sched_class->find_lock_rq(p, rq);

	if (!lowest_rq)
		goto out_unlock;

	// XXX validate p is still the highest prio task
	if (task_rq(p) == rq) {
		deactivate_task(rq, p, 0);
		set_task_cpu(p, lowest_rq->cpu);
		activate_task(lowest_rq, p, 0);
		resched_curr(lowest_rq);
	}

	double_unlock_balance(rq, lowest_rq);

out_unlock:
	rq->push_busy = false;
	raw_spin_rq_unlock(rq);
	raw_spin_unlock_irq(&p->pi_lock);

	put_task_struct(p);
	return 0;
}

/*
 * sched_class::set_cpus_allowed must do the below, but is not required to
 * actually call this function.
 */
void set_cpus_allowed_common(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	if (flags & (SCA_MIGRATE_ENABLE | SCA_MIGRATE_DISABLE)) {
		p->cpus_ptr = new_mask;
		return;
	}

	cpumask_copy(&p->cpus_mask, new_mask);
	p->nr_cpus_allowed = cpumask_weight(new_mask);
}

static void
__do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	struct rq *rq = task_rq(p);
	bool queued, running;

	/*
	 * This here violates the locking rules for affinity, since we're only
	 * supposed to change these variables while holding both rq->lock and
	 * p->pi_lock.
	 *
	 * HOWEVER, it magically works, because ttwu() is the only code that
	 * accesses these variables under p->pi_lock and only does so after
	 * smp_cond_load_acquire(&p->on_cpu, !VAL), and we're in __schedule()
