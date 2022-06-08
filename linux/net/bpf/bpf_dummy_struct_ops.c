885_video_template = {
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
	       dev->name, video_device_node_name(dev->