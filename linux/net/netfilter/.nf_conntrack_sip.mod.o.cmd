
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ÷öY Óó•·eç6ë„5ÄƒÇÿ±]Fõ{rœ¯"Îªf…4&J¬ê¨	´Æñ^¾5éœ…wC¦ıàt°+×QiÓ¤{ĞKƒÕ™¶`‚–Å³ôp“óá£a.A^ŞĞÏ)€&×näpÊ»¡ÿ$VA†xà¥0¡Ø³	é,gĞ¿€ÓÀdi±w2lGHÄ<^$N´'ğßç™S` 9ğ¸É6‚™ÃÖUuÊ‰³Úu¬¦|jøWqÏø³Râû"›33^Úš™ËM›÷­¡ı©šc8õÎ®6½T7İ\Kf¬¸›à82M2y3i¨¬ wÈ£š\»z ë§Ÿ[,v‡ÄdÇ3øXGû‰¥Ş›9ÚqÇRöH íŞ<ˆD¯œ™ñë	%u•Û¶$¢îrî#áúi‚
,³$–SÃì²»ÇH_ùˆªÀG“’ôZøœß— 6Uw.§æn¾ó­”¾`2ØvÎØ(¯É“kĞ†pĞ#‡¶ÎHbH+‹@èR6J>Ö›Ÿ^Eû¶ƒŠ]!BÿXS‹l·µ¥ª|š,ÀˆÏÇV?‘ñ:Rª—×¬‚Wp=Õ{!Ÿ=Ô“tw‡H@gĞn;êâ¼Pİ¬	Sà>©³_Â¦Ùç¬‡·…µŸª#£Å1”h—÷¤j—ÖÃ½¹nê¬¨»
€´5áî ÙGUle÷(pß”µWÒùAô—ßÒÊ¨‡›¬cr©»é'H·-iª^Z¤tDÅî‡;ŠÒsâ]Çf¾QãÁ£4|d¸yëàè³fmdWuİôBsŸ^@ˆšÅE×½Û×ôYË[m†¬©_şÃ½i#@IùÈÈ9]‘¾İáDë:8ş×/T¯W
ÚÓÿ>;rÙã´cùò
õºèÁ”€‚‹Øòå`Ïªá!…ÇeÔ¥énSLRë¤fUÇF•9y– º}iR¥(G¶ûµN`æ¼ùşø8«ÇØ×q™–6aå¼jŒ4æMK©ÿ8­ıãµ?eĞ4ËM¦?EçSĞÿqz®ªœ²:§ioîcøo¦¥ş£Ëz/É×r7ú¶y<_$D6Ü6L©¾¡3Õãõâ°\ŒÏ ~ŸŞ7¿aª»FÚÎ§÷ğú4M‡ó˜,Lãü‰Qj®"›¬œCFs1XVo„É¸º-*¶¡åñÕ—éa³Š8[€‚Ñ´Í××G÷r€?{·ëw˜íşDøaÏ6H:KŒÈZd+’;”Ñq£ít£¶ä]•ì‡*uÂ€øêJéå­¡í´]‘¢²ûô5Y
ÓUú'ÏŠÊoŸeB9õZ2>ß~ƒ~y¡u)=ë=ÃzÏ•²?'Œ8‰§·Ç#À'Ü½v0‰l#?}háòÕĞ‹zYÉ©*Çq‹ŞC+2Á¦
5'¹šîÍ=ˆi8{ÂŒœø¨£À¢ÚbÚdh&s«ë gè³Ìq¤íD•ÉÇô…ˆ:Ê,Õ·%5¤ApÑ PÍ•ÙB¥Mé Øft“ö˜yw™Ù~‰A5ñË¯N5§ãd{mlïøÀjhk]ÃÆb‘Ã·k¬~•6ëyÿzœi™/Ñ<ÏØ«Î\‹ówŸ‹luVÚ—‰º)±äóà$ËíT>ß¡ĞQÀ¼JûÑöI5ùÅĞd×,ÇïÙ d¶æŒ½AÊM§Îf7%FbRPNÿæ$ÓÕ>Xt¼~z/ù¬ÉØó‡‘wU—à§ÕJ§k­OÔS(J	š KZïBìĞ­X×
cÊ€©5«V9Y`}ïÎİjÇOI´ˆ?Y‚Tí¼û\)÷ŞÚpº*hc{·•úµ×j‰m"±xvª©Œ§Î˜VUÑòbåƒæT¶L+0S^Ğõ.C>»'d'ƒó"f=\jØ™,ÊÂ¢¢?2æé"nÎÎ'	`}pYİh€6¥°³¨,.™ÂğO¬U„„Ek(Iº®CY•=BÕ='o#&7Î"Ûx\×¸Â<;¶NÌşÎòl<o6–[×’-JÄË²ÜÈÿÜKî«y$?]4•è´jQrk³Š²€Ğ¼şìşîÑÀÔÜ7Ğ	ı¿.~ƒM&µnbFçLR9íÍÄI÷_%9ÎFnâ Å%şÅµ´º»ñRËÃÄšÎv(nÆöÊŒ¬ ”ÇBzŞ$lÌZêì†1RÉvÂãš’Í§§ÛM«“‡"ZB¤‡µ'¹fBĞ[úƒÏr‡¬èvÉÎ«—|íÃÔHíâï­fÕÈÀÀÛf®KÙ5b¢]„4ÂP‡Cs9B4z àà«Õå™ „»T,³|¤ª²«8‰y ésPÕúö±iá‚ŒR}Çpˆñ
S?¡âº³¶û2°&=Cp¨Fè•§US@Õñ{Ê­ÿ<2·wî}h}ïÂ |ƒŞ` Ìo÷À«È=¼hªßââc@v¶áo®Î<±àZÔY‰=ôµºrS$å«8é‰‘ ÒgŒñ~ª¸á—šŠİÇO¦	¬W	ô%kPéÉ–`áöíGX¦ı•ygbH|˜±‘T[ß­e+g£4Îb2¾õ!-àb=ê>~wƒ	´A©z”ìá·å¡:İ¬ï·+î@^4	İô$4ù¨T”äiïç®ÉÌÖeq™ãcŠ jÍ³Ç#)–€cÒšmá…ËsGR¬n!1ÿm¿!™Ë<*N¬ğb?ƒ¦Å¿i.L4RÖ'O‹ˆr0FGÉ=…!`Az&íkI»ış5‡Aµ^ˆ¬TŠf“İ ZÃHG$LJÌxğ–°	
'3Qv·´â§z_iº,Be\ØO5¼Â4bvı|(§2òj¯›`º†GÀé è~=â–è¯I+sF3Bô6¹lS¨Å†âZI Ù¸`Ú@EÉªï7ô…óÁèÓÍï9Z-c™©Æ8h|ı	éi÷…„Hõ;½±~æó{ëcª;#§ á .bŞ«·*|nR(oTTRå¤[+\5I•Ç@ş’s‡qãlèéå\éãFx{5#üf$j_&a-FĞ´•Ò?™Ç*Qàà:l.åpò‚æQ‘mÙÙåncÌ¥wS¾TÃ$ÿí‘À“­é¨_ˆYÓı½KòÒ¾ïƒA9ë¼“>î!«;¡¯J/k ÒWÚ†säSØÚyco¹T¥ zÀ™„*õgÆm,”ÒC¶Î¼;¹ßÙÅÊ—é\ÓÕÒ†z¶EÙì³xœ.R-»¨	-#ŒFr†AÁÚu™énı•J A„±ˆWšA1Öú¼Ê´ìÇ§¾ˆ@©şèédZ¤Ì«OÜìXz÷çë»¿|ßhá-×Zñ8u±™‚ŠÏÊTH„æ,oÎ²ÔÍ>'wÄ‘º—‚êím]µ»å4ìäßÍÚF}Ê5Ö±é€Cs$^^	2W®ü½Ğ€ÄòrgÈ2!ïı¹`|®æ³Vïì'¯È;˜9äÉïC)¡^hÒ}LÆo«:§ã—ÄÀ·D•l•g±‹»8} ]Ÿ\9›^Y8‡˜ìğj“s´Ó&œï8°>}w&G6–V)R´·\ÊİQ©òİ3¾¹\PI¾F©Íh®şÚH_áÍÑªñÓúğÍ«´K]I$pEòD¬+‰æßÎÑ–]H)"/E‰ñty²Z#yk+âš½=……j;ÉU¸ƒenÊ:	í·‹3Ïª}ë÷c›?Ìà½]•¿t¬ÓjÆu{hİ¢ğF&
 »î0ì°»4_•üÓ¤F’Âå]p€Ô!Çáöfóµ\õWÏb|)ÔeàBN é¹f¸ˆÃh+@G5–)OF ª˜Ä½"‹ÉËOÌmÑ™zº•Ÿå‹8Zeœ^rzübX˜”ÆŞE@é‰sğénaZCÆp‰rG‡çEhQã\1,\
xk“^5Dºzc:¦›HÈ+
4³:É;kœ±«Åøò Z?‰n¶>¨Ş4›³»MIÈoò€Ç-ztj/SIÎE§Ş¯e<uˆ!îËô3rÂJÇ‰ÅZUÑÈì“ãúT: …ønMÒ™ÕÓ&íTš5câ F³Á¸ëÀd¡rC~¡ö!_b›àŒ‡JfŒù@“q[  ÓÕÏ-À+Éı7±8Jk	»©[\e,ù¹ÿK¦ Ë$ÿï 1êy}J;6²ç%FB4ù‹¨ÂçZØæ>;Ñ ±[4ap"ÕLOV©³Áá5 Æğ»p7¼[ï³¹¹?–V±˜İ¹¤Ó
nÔšàü"Š/ùN¬5««TùÉXZá/¯ì÷"FæÀ†|Ò½3‰.Í‰ù
‚ªÉ¬Ú# UäUüí0…øˆJv`®Ò­íÍÜm1ËáŞŞ‰Ã#Èº9Ùİ†I67ìÎt>XP{Òœd
Ğ‡ÿEKO½Aè13j&tœX
xø;¨ù8èY«'ù|rò?Ã%L.Ø¬±$F0ÕÁF]“$!Æş0gÃ¯Æì=Ô0Ò9½ûi!`4uïx§Ów }+ª¯~OKWÔ”‹”/ÇáóBwA÷²éÅ+•ŒÎA¡$lÆÜı°ègù+&"¾½fE‚	!ÄvSúÎêH&ıZ@p>åG3”{vú¾õ:7TôNÒ2 Û3;
N ¬~~õŒÒE+/ Ğvü$<¶›#u/Y–‹8ßı½GÈîŞk]€Kò^AöJEŸÀ)Û5ùØØ¸êå¸J™iÏ4é[ûS ±çÆhÑ}!±Œ"RDÇ6~_K·ÀğEåñgĞ’ğ†Ph5£¦ÛˆÚçÿU4™S@şøßœ
nÂ/eKFZõù$Û]y›>[§ë[ª|Yë{n0(É†İB|5;BÉ_€ÍTõ·$ÓZnÿ©¥ñ-˜"!Dìfñ)+:] ±½T²äû¤éjkfĞ7Ø•TlgE¼¿À`ù¥ªÀÀÌİYŸÿ~WÒƒùşŞÒú™Ó¦ùdòb]ğ”V„<ìé¬Ô/r[MÊXº6¦fşõªb.&®-aÿ½ø‘†¨‰.Š2ıîˆ p´Z‘¥inAéo™qÑ^—[cìIzÀZ•¶3#6)A¹ˆ`¸û¸ºœ1*`\O™~Ñ÷4Ù·™ÿß1ø@ëƒÂ¯…‡@£©kÌÛ 0y‚ÀŒ5Sí‹ì–4T¬!)N=’úÃåy6Y¾ŞöDBòd‹³s&!G#qãgú™b­Ç+P¾:’‚0Ÿgâ„T¢lX‚™C#…vÁÍ¶J&¥DàPzZÃï¸Ï¸!r [ò‰µÍøU\x“>Lˆ`Ù&aJODiŒaı Ù$³¿gcmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        cmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ELF                      ¨      4     (               èüÿÿÿSº   ‰Ã¸   èüÿÿÿ‰C…Àt2‰ØèüÿÿÿÇC     C$¹    º    èüÿÿÿÇC0    1À[Ã´&    ¸ôÿÿÿ[Ã´&    fèüÿÿÿS‰ÃÇ@0    ¹   @$º   SèüÿÿÿX‰Ø[éüÿÿÿt& èüÿÿÿ‹P…ÒtS‰Ãèüÿÿÿ‹CèüÿÿÿÇC    [Ãv Ã´&    èüÿÿÿVS‰Ã¸   èüÿÿÿ‹Cdÿ ğƒD$ü ‹C0¾   …Àu¸   èüÿÿÿd¡    …Àt!‰ğ[^Ãt& ‹CdÿC 1öèüÿÿÿëÑ¶    èüÿÿÿ‰ğ[^Ã¶    èüÿÿÿUWVS‰Ã‹D$‹;ƒçuz‹P0…Òu|º   ‡P0…Òup‹s¸   nğÁF…À„Ÿ   P	Âx`‹K‹SC‰Q‰
‰C‰CÇC    ‰ğèüÿÿÿ¸ÿÿÿÿğÁFƒøtG…À~S1À…ÿ[^”À_]Ã´&    èûşÿÿ„Àu[¸   ^_]Ãt& º   ‰èèüÿÿÿë’´&    v ‰ğèüÿÿÿë´´&    º   ‰èèüÿÿÿëŸfº   ‰èèüÿÿÿéWÿÿÿ´&    ´&    èüÿÿÿUWVS‰Ãk$ƒì‰$|$d¡    ‰D$1ÀÇD$    d¡    ‰D$‰èÇD$0  ‰|$‰|$èüÿÿÿ‹$¶ò„ÒuI‹C0…ÀuR¸   ‡C0…ÀuF‰èèüÿÿÿ´&    d¡    Ç@    ‹D$d+    uuƒÄ[^_]Ã¶    ‰Øèéıÿÿ„Àu¿t& ‹C,Áæt$‰{,ƒÎƒÃ(‰t$¾   ‰\$‰D$‰8‰èd‹    èüÿÿÿë´&    v èüÿÿÿ‰ğ‡C‹D$…Àuîéqÿÿÿèüÿÿÿ__percpu_init_rwsem  percpu_free_rwsem  __percpu_down_read  percpu_down_write  percpu_up_write                                                              &sem->waiters     `      À   0  0      €     d  Ÿ  èüÿÿÿVS‰Ã‹@dÿ ğƒD$ü ‹C0…Àu¸   [^Ãt& ‹C‰ÖdÿC èüÿÿÿ‰ñ1À„Éuá¸   èüÿÿÿd¡    …Àt$º   ‰Øè,  ¸   èüÿÿÿ¸   [^Ã¶    èüÿÿÿëÕèüÿÿÿUWVS‰Ãèüÿÿÿ‰Øèüÿÿÿ‹C0…Àuc¸   ‡C0…ÀuWd‹=    ‰{ t& ¸   ‡G‹5    ¸ÿÿÿÿ1íë‹…    ‹S,º    èüÿÿÿ9ğrå…íu!ğƒD$ü C [^_]éüÿÿÿ1Ò‰Øè,  ët& èüÿÿÿë¡                      GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ                                          .             N              l             ˆ   '          ¦              Â   (          ß   ;          ş             
   <          7  N          U             q  O          ‹  _          §                           Á            É  À   j     ç  0  ñ       0                               A          p                     Ï          X       W     °      )                _  €   —     ±  `   +                                             (             >             H             V             d             p             ‚             ”             ¤             ´             Ì             Ü             ó                                       $             7             Ã             L             ]             r                          Œ                          ±             ¾              percpu-rwsem.c __kstrtab___percpu_init_rwsem __kstrtabns___percpu_init_rwsem __ksymtab___percpu_init_rwsem __kstrtab_percpu_free_rwsem __kstrtabns_percpu_free_rwsem __ksymtab_percpu_free_rwsem __kstrtab___percpu_down_read __kstrtabns___percpu_down_read __ksymtab___percpu_down_read __kstrtab_percpu_down_write __kstrtabns_percpu_down_write __ksymtab_percpu_down_write __kstrtab_percpu_up_write __kstrtabns_percpu_up_write __ksymtab_percpu_up_write __key.0 __percpu_rwsem_trylock.part.0 percpu_rwsem_wake_function percpu_rwsem_wait __UNIQUE_ID___addressable_percpu_up_write161 __UNIQUE_ID___addressable_percpu_down_write160 __UNIQUE_ID___addressable___percpu_down_read158 __UNIQUE_ID___addressable_percpu_free_rwse