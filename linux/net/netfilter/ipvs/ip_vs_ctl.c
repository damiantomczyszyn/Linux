
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
   <          7  N          U             q  O          ‹  _          §                           Á            É  À   j     ç  0  ñ       0                               A          p                     Ï          X       W     °      )                _  €   —     ±  `   +                                             (             >             H             V             d             p             ‚             ”             ¤             ´             Ì             Ü             ó                                       $             7             Ã             L             ]             r                          Œ                          ±             ¾              percpu-rwsem.c __kstrtab___percpu_init_rwsem __kstrtabns___percpu_init_rwsem __ksymtab___percpu_init_rwsem __kstrtab_percpu_free_rwsem __kstrtabns_percpu_free_rwsem __ksymtab_percpu_free_rwsem __kstrtab___percpu_down_read __kstrtabns___percpu_down_read __ksymtab___percpu_down_read __kstrtab_percpu_down_write __kstrtabns_percpu_down_write __ksymtab_percpu_down_write __kstrtab_percpu_up_write __kstrtabns_percpu_up_write __ksymtab_percpu_up_write __key.0 __percpu_rwsem_trylock.part.0 percpu_rwsem_wake_function percpu_rwsem_wait __UNIQUE_ID___addressable_percpu_up_write161 __UNIQUE_ID___addressable_percpu_down_write160 __UNIQUE_ID___addressable___percpu_down_read158 __UNIQUE_ID___addressable_percpu_free_rwsem152 __UNIQUE_ID___addressable___percpu_init_rwsem151 __fentry__ __alloc_percpu rcu_sync_init __init_waitqueue_head __wake_up rcu_sync_exit rcu_sync_dtor free_percpu preempt_count_add preempt_count_sub __preempt_count rcuwait_wake_up __SCT__preempt_schedule wake_up_process refcount_warn_saturate __put_task_struct __stack_chk_guard current_task _raw_spin_lock_irq _raw_spin_unlock_irq __stack_chk_fail __SCT__might_resched rcu_sync_enter nr_cpu_ids __per_cpu_offset __cpu_possible_mask cpumask_next finish_rcuwait       #     $  !   %  0     5     :   &  a   #  ~   '  ‘   #      )  ¨   *  Á   #  Ï   +  ñ   ,  ÷   -    .  !  /  1  #  –  0  à  1  ó  2    1    1  1  #  J  3  ^  4  l    y  5    6  ª  4  ¼  3    4    6  !  7  8  8  ‡   (                                            
           !                "                                                                   #  4   .  F   ,  L   -  \     f   +  y   /     #  Œ   9  “   :  ­   4  Â   ;  Ó   <  Ş   =  ã   >        7  ı   ?      "     !                   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab_gpl+__percpu_init_rwsem .rel___ksymtab_gpl+percpu_free_rwsem .rel___ksymtab_gpl+__percpu_down_read .rel___ksymtab_gpl+percpu_down_write .rel___ksymtab_gpl+percpu_up_write .rodata.str1.1 .rel__mcount_loc .rel.smp_locks .rel.sched.text .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                         5            @   <                    	   @       ˆ                 %             |                     +             |                     0      2       |  `                 F             Ü                    B   	   @       ¨                 m             è                    i   	   @       À                 ’             ô                       	   @       Ø        
         ¸                                  ´   	   @       ğ                 İ                                 Ù   	   @                        ü      2                                      &                       	   @          @                            H                      	   @       `                 /            P                   +  	   @       p                 ?            h                    ;  	   @          (               T     0       |                   ]                                  m              (                                ¸                 	              ¸	  Í                               (  €                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ELF                      ¨      4     (               èüÿÿÿSº   ‰Ã¸   èüÿÿÿ‰C…Àt2‰ØèüÿÿÿÇC     C$¹    º    èüÿÿÿÇC0    1À[Ã´&    ¸ôÿÿÿ[Ã´&    fèüÿÿÿS‰ÃÇ@0    ¹   @$º   SèüÿÿÿX‰Ø[éüÿÿÿt& èüÿÿÿ‹P…ÒtS‰Ãèüÿÿÿ‹CèüÿÿÿÇC    [Ãv Ã´&    èüÿÿÿVS‰Ã¸   èüÿÿÿ‹Cdÿ ğƒD$ü ‹C0¾   …Àu¸   èüÿÿÿd¡    …Àt!‰ğ[^Ãt& ‹CdÿC 1öèüÿÿÿëÑ¶    èüÿÿÿ‰ğ[^Ã¶    èüÿÿÿUWVS‰Ã‹D$‹;ƒçuz‹P0…Òu|º   ‡P0…Òup‹s¸   nğÁF…À„Ÿ   P	Âx`‹K‹SC‰Q‰
‰C‰CÇC    ‰ğèüÿÿÿ¸ÿÿÿÿğÁFƒøtG…À~S1À…ÿ[^”À_]Ã´&    èûşÿÿ„Àu[¸   ^_]Ãt& º   ‰èèüÿÿÿë’´&    v ‰ğèüÿÿÿë´´&    º   ‰èèüÿÿÿëŸfº   ‰èèüÿÿÿéWÿÿÿ´&    ´&    èüÿÿÿUWVS‰Ãk$ƒì‰$|$d¡    ‰D$1ÀÇD$    d¡    ‰D$‰èÇD$0  ‰|$‰|$èüÿÿÿ‹$¶ò„ÒuI‹C0…ÀuR¸   ‡C0…ÀuF‰èèüÿÿÿ´&    d¡    Ç@    ‹D$d+    uuƒÄ[^_]Ã¶    ‰Øèéıÿÿ„Àu¿t& ‹C,Áæt$‰{,ƒÎƒÃ(‰t$¾   ‰\$‰D$‰8‰èd‹    èüÿÿÿë´&    v èüÿÿÿ‰ğ‡C‹D$…Àuîéqÿÿÿèüÿÿÿ__percpu_init_rwsem  percpu_free_rwsem  __percpu_down_read  percpu_down_write  percpu_up_write                                                              &sem->waiters     `      À   0  0      €     d  Ÿ  èüÿÿÿVS‰Ã‹@dÿ ğƒD$ü ‹C0…Àu¸   [^Ãt& ‹C‰ÖdÿC èüÿÿÿ‰ñ1À„Éuá¸   èüÿÿÿd¡    …Àt$º   ‰Øè,  ¸   èüÿÿÿ¸   [^Ã¶    èüÿÿÿëÕèüÿÿÿUWVS‰Ãèüÿÿÿ‰Øèüÿÿÿ‹C0…Àuc¸   ‡C0…ÀuWd‹=    ‰{ t& ¸   ‡G‹5    ¸ÿÿÿÿ1íë‹…    ‹S,º    èüÿÿÿ9ğrå…íu!ğƒD$ü C [^_]éüÿÿÿ1Ò‰Øè,  ët& èüÿÿÿë¡                      GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ                                          .             N              l             ˆ   '          ¦              Â   (          ß   ;          ş             
   <          7  N          U             q  O          ‹  _          §                           Á            É  À   j     ç  0  ñ       0                               A          p                     Ï          X       W     °      )                _  €   —     ±  `   +                                             (             >             H             V             d             p             ‚             ”             ¤             ´             Ì             Ü             ó                                       $             7             Ã             L             ]             r                          Œ                          ±             ¾              percpu-rwsem.c __kstrtab___percpu_init_rwsem __kstrtabns___percpu_init_rwsem __ksymtab___percpu_init_rwsem __kstrtab_percpu_free_rwsem __kstrtabns_percpu_free_rwsem __ksymtab_percpu_free_rwsem __kstrtab___percpu_down_read __kstrtabns___percpu_down_read __ksymtab___percpu_down_read __kstrtab_percpu_down_write __kstrtabns_percpu_down_write __ksymtab_percpu_down_write __kstrtab_percpu_up_write __kstrtabns_percpu_up_write __ksymtab_percpu_up_write __key.0 __percpu_rwsem_trylock.part.0 percpu_rwsem_wake_function percpu_rwsem_wait __UNIQUE_ID___addressable_percpu_up_write161 __UNIQUE_ID___addressable_percpu_down_write160 __UNIQUE_ID___addressable___percpu_down_read158 __UNIQUE_ID___addressable_percpu_free_rwsem152 __UNIQUE_ID___addressable___percpu_init_rwsem151 __fentry__ __alloc_percpu rcu_sync_init __init_waitqueue_head __wake_up rcu_sync_exit rcu_sync_dtor free_percpu preempt_count_add preempt_count_sub __preempt_count rcuwait_wake_up __SCT__preempt_schedule wake_up_process refcount_warn_saturate __put_task_struct __stack_chk_guard current_task _raw_spin_lock_irq _raw_spin_unlock_irq __stack_chk_fail __SCT__might_resched rcu_sync_enter nr_cpu_ids __per_cpu_offset __cpu_possible_mask cpumask_next finish_rcuwait       #     $  !   %  0     5     :   &  a   #  ~   '  ‘   #      )  ¨   *  Á   #  Ï   +  ñ   ,  ÷   -    .  !  /  1  #  –  0  à  1  ó  2    1    1  1  #  J  3  ^  4  l    y  5    6  ª  4  ¼  3    4    6  !  7  8  8  ‡   (                                            
           !                "                                                                   #  4   .  F   ,  L   -  \     f   +  y   /     #  Œ   9  “   :  ­   4  Â   ;  Ó   <  Ş   =  ã   >        7  ı   ?      "     !                   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab_gpl+__percpu_init_rwsem .rel___ksymtab_gpl+percpu_free_rwsem .rel___ksymtab_gpl+__percpu_down_read .rel___ksymtab_gpl+percpu_down_write .rel___ksymtab_gpl+percpu_up_write .rodata.str1.1 .rel__mcount_loc .rel.smp_locks .rel.sched.text .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                         5            @   <                    	   @       ˆ                 %             |                     +             |                     0      2       |  `                 F             Ü                    B   	   @       ¨                 m             è                    i   	   @       À                 ’             ô                       	   @       Ø        
         ¸                                  ´   	   @       ğ                 İ                                 Ù   	   @                        ü      2                                      &                       	   @          @                            H                      	   @       `                 /            P                   +  	   @       p                 ?            h                    ;  	   @          (               T     0       |                   ]                                  m              (                                ¸                 	              ¸	  Í                               (  €                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 # SPDX-License-Identifier: GPL-2.0
# Any varying coverage in these files is non-deterministic
# and is generally not a function of system call inputs.
KCOV_INSTRUMENT		:= n

obj-y += mutex.o semaphore.o rwsem.o percpu-rwsem.o

# Avoid recursion lockdep -> KCSAN -> ... -> lockdep.
KCSAN_SANITIZE_lockdep.o := n

ifdef CONFIG_FUNCTION_TRACER
CFLAGS_REMOVE_lockdep.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_lockdep_proc.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_mutex-debug.o = $(CC_FLAGS_FTRACE)
endif

obj-$(CONFIG_DEBUG_IRQFLAGS) += irqflag-debug.o
obj-$(CONFIG_DEBUG_MUTEXES) += mutex-debug.o
obj-$(CONFIG_LOCKDEP) += lockdep.o
ifeq ($(CONFIG_PROC_FS),y)
obj-$(CONFIG_LOCKDEP) += lockdep_proc.o
endif
obj-$(CONFIG_SMP) += spinlock.o
obj-$(CONFIG_LOCK_SPIN_ON_OWNER) += osq_lock.o
obj-$(CONFIG_PROVE_LOCKING) += spinlock.o
obj-$(CONFIG_QUEUED_SPINLOCKS) += qspinlock.o
obj-$(CONFIG_RT_MUTEXES) += rtmutex_api.o
obj-$(CONFIG_PREEMPT_RT) += spinlock_rt.o ww_rt_mutex.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock_debug.o
obj-$(CONFIG_QUEUED_RWLOCKS) += qrwlock.o
obj-$(CONFIG_LOCK_TORTURE_TEST) += locktorture.o
obj-$(CONFIG_WW_MUTEX_SELFTEST) += test-ww_mutex.o
obj-$(CONFIG_LOCK_EVENT_COUNTS) += lock_events.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       # SPDX-License-Identifier: GPL-2.0
# Any varying coverage in these files is non-deterministic
# and is generally not a function of system call inputs.
KCOV_INSTRUMENT		:= n

obj-y += mutex.o semaphore.o rwsem.o percpu-rwsem.o

# Avoid recursion lockdep -> KCSAN -> ... -> lockdep.
KCSAN_SANITIZE_lockdep.o := n

ifdef CONFIG_FUNCTION_TRACER
CFLAGS_REMOVE_lockdep.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_lockdep_proc.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_mutex-debug.o = $(CC_FLAGS_FTRACE)
endif

obj-$(CONFIG_DEBUG_IRQFLAGS) += irqflag-debug.o
obj-$(CONFIG_DEBUG_MUTEXES) += mutex-debug.o
obj-$(CONFIG_LOCKDEP) += lockdep.o
ifeq ($(CONFIG_PROC_FS),y)
obj-$(CONFIG_LOCKDEP) += lockdep_proc.o
endif
obj-$(CONFIG_SMP) += spinlock.o
obj-$(CONFIG_LOCK_SPIN_ON_OWNER) += osq_lock.o
obj-$(CONFIG_PROVE_LOCKING) += spinlock.o
obj-$(CONFIG_QUEUED_SPINLOCKS) += qspinlock.o
obj-$(CONFIG_RT_MUTEXES) += rtmutex_api.o
obj-$(CONFIG_PREEMPT_RT) += spinlock_rt.o ww_rt_mutex.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock_debug.o
obj-$(CONFIG_QUEUED_RWLOCKS) += qrwlock.o
obj-$(CONFIG_LOCK_TORTURE_TEST) += locktorture.o
obj-$(CONFIG_WW_MUTEX_SELFTEST) += test-ww_mutex.o
obj-$(CONFIG_LOCK_EVENT_COUNTS) += lock_events.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ×®Ür€ÕØÕk·(]¼Ùí^Š¯»üd{DA“Yáª¾YÎ¥W¡ÕùÃØÂ04ª²>gz!ŞB…ïïpzq†¾ €µ>sİ85<¬ãP
Y¡à#0¡¸±4D©˜Éh]¡™İ,jflˆzŒµĞq¥‘]±²ıOÛD‰•OP¢'^ÜqÄoPagö±Nı‡«ñÏS#V² E	SÍ_4pÖ¡1(³ tÑVÃ8}Ÿõ¾‡R½Ò/Õlİn§î<Áİ‘ú¯BgÓjLM³c
ÊTµFÛAiå[
—’»!â´ï¸&Yİ8õá§M^¤òiy›˜,È˜ë¦dò¥•|	Û>ÛàØB:¦¢^z×`İÄî{ÁÖK€lsNØê¯}Ôç:ì¡—™†9$YÛ#qMşö0G€Âgqq×ÅŸ' Xf 1Ãtét¬ßn"QÓ2°ÉïûÈH%g_à-Sc~å:²²ˆ0…6g^Û ¬ñ{!2ûÃCÿîJ±¬}ßuœ•¢ãwE%œ^tšI®	|ß4ôøBaÚÃ±7" X—`6IdÌÙ$UÛƒäQÁæ8š…hü[íŠA:¤À¾egğ¹t~ú@aæ¹ÈT‘Øä-Ãê‹h+ä?9‹j)\›ñ•oûG\â—cøÕ/¢øaçşF3`Ÿ§P-İ2›òrV	I¬-pºûÍ¹¥¾$Fe¾äö4qìÏüÿúG‡-˜	OÒ–³±±áØ‚a23…ÚÓd;Åæ…ÇœØ¡ë…•Rñÿ…ÏÌmN \áá;ûİ?šéÏ`bÊ«ÏåØ”}9\²ˆËa}U°¸L…0×¸‹<Ô¼y„Yìy.)NºAÒ0îU‹N•2—3¶C$Ì¨orî¤mg*í	9/‹CyW-n®ÒĞÁü(Â8ä^ı¥ré&ÿ¡(J=}vF–`¼¬	,K¡÷†_5~|ï– üÇî¸= Éz¹9#²W›.hS#*S¿cTß
>ıê^÷ô7KtÛúü`r´D®Öûjo
Ùˆ f1-Ö‹ç{D(ïêÄá¸J}©íÿÔÑÇæùò¬´¬íãÌÎÚt'í@‚xìÅ²ù*^`ZÁ)w›Œ«Ú‡ië[YîµÉ>‹×.™­{CÉÊë¥Ä……ş>[Ğ
w$ú#ZñTcØ¸T °>Ê1×€ÿ²1½ìmEÕ¯nY¬ëJ…Q"»ÚÖ^D×á¿'ÇE‘Â>¥y9™•Ş„O`e6Cö	æ9şøÊÛŒ'‰7º¨”]Š‚)X‹iÿcHœ=º¼iÛ¤î•†ßŠVùß²VŸ)æGLyi3$ŒÒ-š[?ÎüWìB€¯ëÖ;0AƒUø]Î¯[]9ôOÇÎhÓ¾KCÁÎo½cä/½l®x¬¸^„¸n+?Çê‘°ˆƒÊ˜û@#UzìDıg…Ú“zZ%ËW§|ÎşaÔ¸İ2©Íêx9mfz™œü0tGìĞh/¶Š'3¸ÓkÅW|»«ë*@Õi,(W*¦fôôª*µWwŸnM—‘¬sCûYu"=¤G¡C¡ĞÌÜy¢pú'™³Ì^¬WÏü4.€	„Îä
z39’š7ƒ;†ÉhÜü,‚àÏh3D<{g1\ß2 æİ²v†£ı}²$ÉÚˆåm?Æ@­ssG)õŒ×™ÌfKCà\ŒL[õ;¥'¥Ÿ˜5#ÈI–b6Yú‹È âzI¶sÔ¶v›«“¼éš—ÓÎĞw}‹ê#„°ß¦Ã®\<"¼*^llW‰j^®ñ,ˆŒÑ‚,kvœ'Ø#ÔyÿÉ ò£Áü»Ù$İwËdà9ÇV¾ZÄ%®
8Š¿wOAàwV²q¸Û„¹˜ú{µÑ/ R!«!!˜3½š<û êÒÁ·Ê=ï` /Wbsöë²kıÖ„/L!Ó'|	Uó†ÅVß¸PkˆRºÀÒ&Tª˜šM¿Ğ¹Å´ÇfExŠY\ödÂ›rÆ>;ÅW›ó;i8™mıÕ–•«‡Ïşt7@ŸšN?ÙR+Tß\«úÌKÑªfN´£å>eNšR	†2SÇVé¾o}hPç~Ãeİf|Õ’¾$E	ĞRáƒêây˜¹–|°±ÌV>ŒØø•A×Â‰·ÿxNı.`I”]®Ô¥U€—Ã ²ÒÃØ	ëwU‘Ï”AÏ…fp¦ü;Æ¾¿µ-½sÚc¹a]NDØÕÓ«·ôõ8jzCóÅ+[2vDkj™æ2·}·€eİ]?è†€‰G†{€éŠeÕCt(äÔZÉ`è'ï`ÈÆÂ—e+.®“~(¦¿Å›#Ì¤Ó¶”èÏÃ'[1›Do^Ë„ü£ÉIo¤ÒÙ„z€¾F¸ÿÜ1µ/¤2Z+u1E‚ÔŒaÚÉÜ§˜ç&şELñÆ`x‹½æˆó-êËÀ	Æs|-İí3•J¦ö£Á’¤º,~v¼B§ñ<4,,R*VP†|®N´õãw%øvxù6pÛÁ08ò»XQãª“§ĞóÿhĞRéıÊS:µv7`]Õ2~o®ø#Ú„­[^;.^ì7ËuJàA¶k9¶£àwBº·Ù4ç§ˆ´¶

]­¸y¾ˆç£A–q‚¸QÖŞØfMÊ¿ã†Ó1ßøGÜYş•jz;¶µ Ífº¸:î´¤ÿ^š¯ê¦Ç8~í#7L HáÍø`ŒÊyào’ZŞbyœµÆØÜœ â}¨€Şˆç’†Û’/ô>šÁ€™Ò€bI.Ì¶,s}`¤*gˆ–.´JlS\®R.?Ÿ Ò)CÒBŠ­ˆ©äØl„V¢Ğô|ù)!å“óªUá%è„Ê{ü•º
®úîİ$]$p2` ·azãiÛwKK$Z¶sÑAÕØMÜƒaexÙCÑ.›ÔÈšv‘ù…“»¤C‹ËÍ>¥•aı6'ui»­b*8:ƒ=ßSÿ¡rEú²Lø’J?36n)e-‘¶®Ëì¬Æ„Í¾)˜ÅÏN?2^îÍK@¬#ù­Ñ ’P•{kfd¢·+œ³Òå!”—•Ú’ZH¨CHc÷Tø›$ƒ{±fş&-Z•¤Hf€¡ü±‘89.@Z4XÎ†b­-]’Ûsúb–™ëÒû‘h˜n€biû=¢—ÿ1xØµ
ÏäW´q„V^ê‚i »Ş˜\òÄñ¤wr|‹¬öâ[6ı‡.«^(´\¤Iß\a¤Ö•?â°?ûŠtÔU¿b¥=	”SpÁ¥N/²¹÷µ¤\X`ø0 ”g2x }’Ë‰ SîıqÆ ‚SêDİ\A0LäFLª
³9Êå=Û¼²zH!yÑ&ş©nª?Z+¾c&®^ÌšÇ¯âNf¹ık3SçQOèÆvù†¨–’ ÃvÊ›7˜OE}±:<ıaÂ–ğó9w=K"Í?ùİg¡™õ4¸b6Ñù…™ek†n;~²Ü?IS£»‡¼îºŞÌP2l¾r³¦#mfÚ)Ø7
YıÎíPm”û(÷+¾ªÈ«
_”è„F_ÓÙTB¿?.jğVŠ©™å*ŒènX­âXWO©Y+aXGXìj°Î†ç/‹;ûVåû­ó!ÓÓ¾Ğ>ŠÌc—ÚŞ ´Üƒı/rãÆË&3g´0e¤jIĞÚÎ‡bD—”Š·)©ïu/¸[Æ.?‹œ„#òÇÂ–ªfÕ§o7 è@]9MÀãIÕ/$şÎ[ĞÄøŸ«œÊ;ĞöäÇ=Qn#÷¿Ìê¶Aã’æÃ³¨‡J'hãjšÛÈIóH”ğWÅp9×–uŒ1J<)nncz×(’T@·ÔîsØ‘cmÁ{"%8Õ„½ƒ:Üæ™ÌR“ĞÒÎ5Ñ´6>2¶xÌKé#ò5>`Ç@d……©¿´‰ˆÿê¦G*—ÅSW Qó|óÚNz¬8JŒÔ®ÓlÛ
Ö`“ş«(6ºQáŸÓë7¨Ç‚,B©.¹>Ù§¶mİ¹Ò\3ªüDF
ûŸN¢¿ˆ¸yÌÕÊ¡E³ªËW¸èœ p*€éÂH™Ì(KÍ(`öï	Î~À1×Œı”â:ÚÔW¼±$óÃ‚0SRaŒ¬à`¸eP¼ÌUqp¸ÔúVd¦˜g#®uXö~f‹Nt>í5±¦'¦+32(Kxöôá¹ñé¹ÿLµåØtŒ–Ï;¯n†€c…ùÄ8MË¢Ìye¯†óìv ‡RbÒ“?õb@	Ş”g¦ò=9ÕÓf.GÜ­ßm¯ q£8¨p?†|±<Ÿ„aq-Å´[<!ª¥Í@×j.éÌ"ÿ^Š/ŸHª|IÄt wøueŞ|ÌÁğ¤+Ù§"bóh““|†£ºûÉ-Êk&t÷r"¢ÙƒæSäÒûéx4‰¼è§ÂÈøÄt¾k)ı†1L¥‚âÜZVéî±…º—À2nıA®æ|›Ê•Sf¦îUP:ƒÛ€-ßš0!™ò5Q€Oª ~g·XèØ„^ÇºĞ”ó^_;9)­â.,ÒPŒWn0…*‚&—‘r:h`Wßêx‚#mÙ`©ù­ÄŸzv³’ıöUÒEÖhÔÀÒã#É½@w¥×2 T#ÀßÂDÊ,'Dƒm4:ª ?§†uuùyüİòÛU<8¨S$ÄÃğº
DŸòNâ¶$Y	%fj‰	„ğU·ÙîûÁãÆU(aÜfØ/ÄZ~MıËÅ[:ÇeqV°8'¤oX¼RûÄ¹WÑg…=$ê[>İNS*
ú½9ÉŠZ‰ÉrÒ½x£™Û££€ú^OÊ…=4„ %?ÄZ¤°ı&8Gmz®¥à€(bF—èÑVF.‘1´B,Ô°7i­E98£§}m‘ 6ğÒÎ%w¤Ç—9?š‰/T¡Ê~IÍdN§¼;rÔÉïF¯š6eV¤˜7îİ‡Ú
îÀ5‘U®EJ÷9å£›kôcl5…»”Õ&¸ß%²]7ÅO4Æã}@/$Um­İv/È‚ë¬izÅ¯jJÙ‰É’–KFÕD®¤µ÷ &EÛ/"HùuÏë›î`oDª^,:¯6Ÿòq!r)€èñ
1ä‘
9MÈ ê,+|›ŸPƒé¼Eœ˜ñ¡„")¡UÜûH¤…
Kt	Àµï:·6®ëU]Í·Ôrmñ™ÆC®%>û(äŸ$4æÊÁ@(ti‡bÂøv]‰€<r4¥MĞ[Ï"±D4¾Ätk‡Òk!Uy¬ös¾¶TTî‰ŠÔEq(³«V£Y*ÄæÛñ#RUé2!<5œ}ÀÚşY½Úà	%‰cIĞs~zNÃW+ñgû"‹û£š­šuÌwCVºúO2(²g¬˜³ñl"Ö}Øúñx»µ+B.oC§·4ºàÔûÚ½J¾úàhÄgN>™•ååiV¼ZæpÊb¾È½¥kş%{¤	vA|Ï\‡–°\6sœp.ÏÍ<>]F{tœ4•´Ô´8$²†£˜„_&LÙ¤ÌÃu…!jKVEãò¿^–â÷Ya­>q®ïô¶ÉoxCÈ®KÌóâ²¾ÃîÑƒíR€Ô 2¿ˆõt17Ê~Â“bD@_ˆÀjÅú0§ãÜ¼ğùÒÖ•}¯P”oÇªNklïŒQ­v xßwñI¤¿¾Dıa*#ÈpR—ùÚ%u¸·v‡ÑMX¶É°^gy‡§…Ú2±bµ™³]}zb¹éXéÏºW<yiQ¡ˆ˜Çèæ³mãÜºj7ïW¤©É @@Ö.Âl'«r}s¶3³©˜UQ³š´CÓ4JŠÂ•¡Ä:‰6™œ&@šsšm‡7ç< ØvhËÀc×°=ˆ3Íù:÷8kÎ“LªÄè1&á€mTYÔeÛ${UuÆUSı¨:‡8£¨¡]@Ô0ò¤ª´ÁÏØdR¸˜O¹±ü?H‚®p-¿cé´Ãì²#5Õ¾1‚4CfY†+"|Éºi3O·UW€Mè9• Cñ6»Å^f;6<1æZ%O%ÌGˆK­sc°*%-¡Ù:ŠõÖbÈpgºÒÏHÖE¸ÊBº´;|6¨ön§›=ôÃ03ªU,|™¥õÕÆı;@ï ó)ÒìÊÌ¾f-ÄÑ ÷œî†5ç0'sæ³òº‘Æ~*3áè5€o!V’/u`›˜ìQ°Rtà—ëÿ/‘y)·?š$!•;¦^¤Œ¹VäfÄç²¼‹óÄZ]Æ[}cı!eEC[òŒÀøÑ¦sz‘9J†Ú¦ĞÈÙŠ·Úo<S9–=Ou%ULªÂ¤à¶§1 €:óïœ…hfü'Oa 
L.K r nùª	XP£®ç,¬àµN
?ƒsÂ8ò0ú3:Yó²(qHPq§ÏK2'Ï‡X" Ä\o¨GÒğºáœ?4P0Õæ¸Ía‚ ­1’gçË«2¦Æ Šİ…àpâˆ&ˆLM€±ñ{·ØÜÃ”jàl2ô¿µY$wæ„¾Â¦WF³²³·iªø¡»x´¶o,¤=,Å.AC¿Á¨šÉçÄnİ éò„ñÔd²näSÜ´Ğé™Äwcè  ƒÚ@A[¨OÂÎ€d iÊíÔüfÌ#ƒû‹'¬—Ó¡QÊ±Íïñ©MÊYĞ´¢íùĞÅB—Ì œ¼ÿÅ¡ÙÁ¤Î,5ÅÍ©Î°œ3_¼ÚUÎäe¹ñÇõZÜPŠš*ò"ç~8²ì½\æ)ÿù¿B®İè‹M¤ö5kê’4ênC0D#ÑÃ¸3‚¥' /!¥ì\´ô¾<Œ˜ÏÑ›½İ¿N¹àSKpáxPìà6@b©>ÿQ
_ˆÂIÅÆ|µÌ/Â}aôÕÂ¸‚]9b©[¯¹¨9‘Øã½Òã°ÚcA¡§`OdcG–"&+öÉŸı>ÎNm—|ˆ°Õ¼1Ï¿_;l"óé”ú_ÊÂ+ÀvúEFßôuËd¾æ0Ğ¸xYM~YtümJÈ‡ñ9,ùÏŒvşy›’àÒ±ÅéÍ5âóÓğùog M÷&şÄ½zAæ~@1×¦„Õ«qÇuª;— o»`¦‰"ÔÎK ¼ÒÅı\KÃEô*}¬mè« ›z_Œcqıß	Vû—a‰”´¢Q&A¥½?c:4€À§£k0²ØwfV-bóe€R˜_GNí•¼Ù”×úw¢Û”^ÂRoğŒ€ákÃe^½Ï~¸ŒËŠŠvïla•¶¥fğ’áßt}VbMOÀ&tX°peö*¡ğ\Êéö9—
}ˆÊUéO?áÍ@a{®Œƒ%4îê™ã¶¢åkaŞÍ…'IìÃsz1dÙ£njóÌçW1è¨¸@ùûa< ÍiA ƒöú‹î€TñQ›4vhsßO_Ÿ(§9!œ‡ß0L¨Iòä†éÉKÿ¬b`€¡b7€Õ|¿Œ(®¤¶^á5ï˜9’h§‚¢ŒÀX’'¾¬ÊîîèÓu‡W4kÀ¿[3?æëäH‹p¨*·åì)ÿw‰œØËm—‹şÚ¸¹,Ù!¨}E˜Ùh@DÉ³x²¢Ä÷‹Ïd²íÊÔã-J†Îßî±T\Ä­.`B\|¤ÉŠCûéºn8v”@ÉÉE;Ãçó}˜¨ ĞOTßØÜÛDhìÇO«UÑáv“qä­8Ã»dª:?‰ù±±¿|*J¤qzŒQU»"‘Bƒcg(Ë"R¸^İ¹GŒ*9¤~I½™^‘©4«ƒ"”"á§‰ËªY2dZ®Ú´À €Ç™’ô:&âÕ¹İÒƒC°µ‚‡qÖ7ø:+ÈIÅ:nÙ\H—ÊaI´õ”Î¿±ö]uã‰ĞİÛ°az¤¯³Ğ!U Op»K›[é0cÜ­”$ñíŸô¯q{AÎk<5¼‘ â+­]¬Ë­€õ€Ç€›A^ ºÿWxÔ~ ÁŒqŸ©¥¼57e•ğÚ&ä÷dÉ~$Äâk2ï¹oúŞn‹˜^Hšo4ïCOß.9¿¨!ëÇ%ê0´‚;²Ç*vQ¼ş¢%e°Ñ(…â¶ªQ“‘5(êKÂ"p+JÀR‡F"qÃYÓTR¸Ãƒnş§ükqØiâ&Ø§´«ƒEæaà#šÃç=‚ê¥*•mÒ¸ßÇª ÉäJÏ•‚% ‘8…°{•ƒdğ‘²‡–ÍÄQ¨Q2^²éÈîKfTÊ|CjÖ§ü·nŠ£ÅB$I(~±Œ­ŞÖ€—Y;[~¥Qï-sÍñ½(§J	¾øÎƒùÑ4¤‘òvõõ4µE=>Dòù‚ŞHÄªmİå°ª;ã'fzŸtØ9	A%?±TUıaxÅŒu³<4–:@ñ¹º½ÔBJÄ=µ÷’ººy"”eµ&1¾éÅËT¾”ÌkwOğ\¨“Ã&?áùĞNòwâ]›æP@ş³P¹s–´)åRdf,X)/og		œí¿Åo!ûG¤Ñ³ÀÖm”ÿnÄl¹Ñ‘hï/3]<“–3H¨ª—ƒ˜%Çôó/·œkÃ@6Âî,× nN ³ıÀ
¸g!›‚jqäØÌ;$1pÅíDKó"G:ÎĞâÈ!‰Ïf¹Ç
í^*ùË!“ ­²Œ†-ú¯”/77P7GÁ24ª2+:ZÁòEŸ^VD|â’°³»ÂÛ«Y‰ÄQXíP51¿X—Ó°Zõ©Îî 	€[L.!Vƒêşù±Ö«ºL aÄóFµöôŸÑt¬ÎÎ›q³ÖPù¬ŸÊX²u‡[}²i+}g‡ï™\ª96•!Ö_aòo¢Qy´#]¸’B2•Ø¦~×5AGÂÊd³­æñù‡î%Yòºzs­RÇuì±ô+ø &ğîep‘šËÕQìÄf¯¯J‡œOÆüuüğ¹y•è\«Êqçzê
—…d2‚H<*Ç%iü›îÚ›´dŠfc/˜œÕÍh·l?Áp2“[¶Ë×ØöñÍ+2Ü´Óé­›F ôÉ!t•Á‰£â ³ëÌêã¾4ï8[OvZ1|êÑ&&Ä‰"UADäĞßñLÆji²\oxUÁä8Ï¯‚Z!+²@==::ìİ(—Nİ?WQ>ÓvoûR³­Ÿ5,6¡ÑşÏ0Ë	oÍÑ·6p¬àı»{7!Å«½•–YáyWİ£–ˆÏl)1ŸíX¶rÿÂëLo¬EXøÎs­ÃQ«×CS_7?µ5©¡–Ã~Næ 9Q;n»ËÄH°õK>¶	¼e=ÏÑ‡j	‡ÆìÉ;y®›òÂİğYuÏR^"‹ô“€`˜K:ÈÆá¨ç5RpìoÁ¢qs‚Á7ßeV,2qÖOÂĞ†ëåy+¦48¯öÏ®µÂq÷­°ù)ì1~me:“Ò RÜºÿ]gj§ªı3‰uÆI#‰mì ›x±µW¸§<ÙdŠÑ“_µÕŞÃÃa§êÁè%™¸\¥ı™¨±0cœRUBÌ&>HzÈ˜/cl¬º¬ÛfÀH"dÎ·S0:íAôU x¾·‚4Yò¸ˆÆÅ>ÿjœ^ô`¿ÓGRê—šDßíõN•@ioÑ÷ÖÇ˜¬@cù~¸Ğ;n¥·vOÏİ›“o•½ÄOï\¤‘¬İ‡˜„ñI©øÎï•M¢B3ÃØN÷x«iAºÁçøZ(`I¢®j?íƒ&§°9YRL,Äcñ
‘­Yq4 B$‹'æLœ¿Ê»œZÆğ_1¶-
»ç…æU› C/ÄchÚit€‘ûî\Í±iÿtikÆëCl.ğw¨Š­CƒÎ¬2ëÕËMî‰"N7e4úßè½'³qñz·q¼Ûçz³Ø‘	¤XIªÖVKëÄs¾ÍÇÎ9;ùÏ„Şü1<0:}†Dì¥´ô´¯TŒ­4Ø>Ä»áúT"¾T>¢¼Ü×›‡ÎÓåú»ãzbŸÿñv¨î»yXÜ*ÚÂ62i‰B¨¨ë¶uÆ+çl-ìæ/;r?;áï#ÿBqDâdè)$É¸Ìò¤Œ-ifğ?Ñıë>Õ†ëº+ãlVd¦‚É[‘·êxˆºëX¬`—\ş£ó¨óG3Ég^ÑH£¡bàÈ‡ÉŸ©I˜¸ØaC•ù<J®DGªØİÜİ|9¯2G®çíóƒ2ş€,";gä½‡µûè°tJYªÄ´µ¦ùz,e$”yT¢N§=râæeSH ƒO”úîJ‘İ{&í‰D-¿3«,‡uĞşÍSH‚¸“Ÿi*T‘Y®>ıTÀg*˜çQÛWªæº›ˆ|Ş6ş¤QßĞ&„1sY@¶Ş– ŸWuTTpwãEG¼~q Ï)S¤é]z-ËMùggåœ}ÂïŸ‹F¿s›G¡ß™ÁÀk£æ­°kgN°³8LÙ î·é’;›;”Ãê3Hı$Q#/9pÀQ¥å!²‰”¡H<a®Şîø¨€Édœš7UqsÂÚçÜ:K’Å™è¡¡B‰ûœ´³_£ŒüÆıÀZ0¹ÙZbsËkĞü€İ\FxÇ,5£ƒo oô-lD¬XjÔŒ¤°Í£pÚÙ"¯{sb)Æš®Õ ‘›îÁ­ä¢iÓñê]Ô—[T¸HïÌÊÏš‰™J¯	+Çƒ¶Ô;ƒ$.‡// SPDX-License-Identifier: GPL-2.0-or-later
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 Â
t´>¤íç\J$ßõíXdL¢=×èí¬CV=p©Î<‡]–ó^qˆûÚyøBæXçòB”±ıt=m® 2Ÿ°"“·6D¼b‹O1b3z=8„,ÆÄBp5e/şóâO¤f¯×±IüŠ=ÌJ¬œ;v&/oÓ)H`J‘TÚ‘?>š‚•fî£!ûhª÷†S”²‹ğ5ËÍÔ$¹7)2%`|À¨¥-Üo0$õÁyµqÖŠrp>¸Î¼ëf˜ˆİmY·J»…Ê\YsìbÉ„uÇCŞ…Ãw^ëó™K]3#Cğ}X’9XV¯t>Ş"Noo#%üås/íºÔ­¬ÙñÔíiq«`r@ 7›^]õ¡sA~Ğ0~İ”/(À·Pİ+P` ÷ø ¥zê0E€y\éMØ‡J^¦Ì”–‹Ò¬Vi'¾ kS©{¸²Ëbç¨R|)8ä»Á»¼²Î7r¹vzW†‚³%H(>~a.‘_Ï•ó"´«·‚·WÆ¹B?0låù@P[éªüxDÛ`äT$:ÚŒ‘Ë|!¦†ä#¶.Æ­§}_æ’SúÃ‹vds~E÷äjWë¿™Ú8è<-´,Î*Ê¡ËŞwöJó-ĞÁ|AÖpÈ¦!Uît
†±‡¯Â’WhÅğ ¨tó/ı»Ó%MÖ›„k^/ñøu$yRnÆºd" Õ)•.ìØŒŞÄõ+¥[Á¢-	'†ÓXÄn€$ÊÜÿ³ILº¯8\B¤¡+6Q‡Ú¢¥a<HÔrŠútX"bÉ38£
«ñó#Ä9ë†(	ô²^\t¥~'ÖdÓUâ1c(›¿ã|ÚV§µM§øA²Ç‹î6ÄVÎZùGüf¥ßf\‰>{IŠQA(z×ÿXÜY$Côl;í³ Ì9X¿“†à4§hƒÿòkºTel†´©ÃöqÌë æC©)Á^¨G•àeJÿQ³–,s(ª¶µca-+[ı„ZÂ¿Ë÷ş»ŒŞå)M{<8›EFb’·Ã-¹ıû{XµâxÜ×“i²ùÊ}<xnŒ“ş¸éÉÊ-I‘oT©Âüç^Ë„ ZDrÜµ¼ãc‰‡ÛPÑÈ£³î@ÿX#×é:šW–•ÀOòeŒ¤9ªÌU¢¹%—£É¦ı;å®ËìVÕ"İ°$ÄU$*Û>Ésó«¥‘~I¼äá„¼D¶š·öSBoİc=%Æs’5}»¸ˆÙ°±Ôi¸ğ|µ¤Zñ»?§UŒòl°®üº\4Ù1’Í÷Ó”Š÷ÓëÍä›q×áGÓ%Jô¦µ Œ>£§¡Zn–Ù:­é{‚BŒÈ.T5Ù9…ˆ™Áú—_	gÿŠÑşµ¼§¿ÇéõŠg}&bŠ-åÿ›€ÙC…G]Ša×ûJò¸6‰©ƒ¾¯•ÑW&$ï€#NÂ0Èqç‚¦Óš\Œµx¿JÒ ¬U%÷FX(øzª¶aÜ¸ Ğå17xnsu¡\#V¡Ê'ò›Píáˆ¿k
ÒÛ¤¯Äü3¯·ç»İ3A	Ô¢>|	Î6Anæñù¡ù5g‹†		dƒû²¿ ä&G«—/K¢Ï1ÎÍo×„‘¹nbÍwFˆ€_FŸE8|Ï¤Éâb~¼åÎƒ®i„ê2fg¿ëºÃ8öÔ¤ ß¨? ÍA×¥>áXË/–~÷\À6¨ÍÕ«¢k÷ò‹*Á²¨C¯Ö‚Q!,ÖÔÉ,Ü¿	-/§jÎ¿õóÖiTØú|5Ñ:dJQ !tqy3›y5‡îyÎfDãPÖ^Uº?W—~A°œÃ%·Ù'“‡íŠ+™&‡t£oQlW¸¯ ºEK¬¡ï€ÑL¢7+ÒÆeo ì·WT—ë!µYÙ“G®§5ÏäËµ#å”’?;w-ğDpRi¯ŞèÎpIéhwy^0œ‚å\®ñz°[£‚åZ(‚‘OÜsi€¶ë‡h4Rèß}Ú[ÁJ=Jˆ·N’–[–h^äƒ­T;ÓšùÂ´Ú|ÒÉ6Û	Ğ!d¹ÊjG-vs…’™Ö¶ê:óæ'¬ \»¯ce¡«2ƒeüÊÜßa{W¯
ÉÜây>	ŒòU¥Z™´òˆ „Ëáîg'AöÍCy#qû¢;–¿÷ëïÙ‚M7(‘A	N¸©WN~wÒ´SB,ë{¨ *ß*5½éX U¨¹ÜàV	ËXIŞ
Š¹€3ÈtèCdÖ‰@¾ãj¾­S¹¼RZŠ+s<»Bå_HiBÃE£«UÈ²Ñøé°:ú0k„ß÷Ôàğİ	¿JÚNÿ¥ƒ‹M'…'­I^‚¢†wÌ®H+&£kaÔ!˜[å¹¨9ˆ*óÁî_7æn…8F‡nßé³æe%nâ,·œó‰“h³'¶ßVØx´Kã¨
1ûúÀ"g°ªCCõüO+7rşû‹¯¨¨l.;’	²ÇZ·•<ôNP™ÇûiîŞÍrõ™¤é8ã¿gP ­°»¸X@âÿ Ü"¯`ØsÒ“øâG ¨yfî=§ }ï“¢é9m™˜ÓÀ/Cı<Íú¿¡€
)4{ó´o2«»7½uïc±€ ‘Ä*Ûäş©¹“²FW[1(q¨!uÏD´Ø±´?÷T·Tş*+O„cŒT­A>8#¼Z‡h®ú$ñü£-ºûó]NµYÓ8™ã!w/u9'>¿£N gÿtÆ2I3E¬1·†ÌTíÕî…¥yjx®F"Ÿ¾báôÖÖG™w¾ã§‘ØĞì”zşŒ·¿«€JxTCÕXãÈ‰ÔAL›lQşÃü"¥YrS_›´T(| 0üWSLø½”,Xs›,õ2gÓImB6ó4Ü4&7Ëş|„‹úˆ»A{±±äkÖóßp¶pE+­İ«xÁ\ûÙØš^ªÄ–"ö›1ç™9R6ã’AsU¾´~í±Ji©&
¹ßa¿Oyx€‘–IìOÌÄ²ä¤×ê¢«Aö©ã 
«˜a‡i¤Ã¨›5²_œŠåÉ«uf—>Ì¦Xo±Jy¸dRéàÿŠhüË°ãÄT*ªßk‘~vÀ|Ô[§ĞUÉ|UÑ‹;Ù%kK4¶}Ve©«øZè*ğÎµYç‡
DÎ’N@fRZ76“„RUóø—mc‚&Ò:Ç[Cµ•FDEÌ×±'Ø¦	ıó4*M\Gû_½±p`/@øéÑ¤57#ï‚è~'#«J[šÜµFÑÎ—‘Û’¶ÈÑÑ§áÇ§ 7t~¾ì…€ŸÀ8ª	ôÚóÛ;è“e˜À6<‚£ÒÿX6£÷Uy÷Gl<sÕbå3ÅtU
!r³—7ÎCdè½¼AF›fÅc«~	:uìÉ[Âr¢¥7°EzÇPUŠ¹9öç'æë‰…[$‹‰;–=Îí#¯ÔF£P #yI´-8Ùğ{b~H¥é©ÚŞ–#nrØŒŒZ¾y^'ä’¬`äƒ€“6k€¡õüL0((>|›yD»<uLâ~^oô™*¤™ú2`_M‘sTB—LìÀj×“‹1#ÚÀ[G±
®#æ0ğÓĞÃw› â*õíL}ù¾íşØÈ:]'O½“ïl”bbkKğØP»}í‡\Fì±‡È¾èD^gÈœ#¡qj'q_NqpˆĞ?5Wä“¥\š5‰Æ”ëYoÓXòSy<ºPFÒA³À<J}
 H÷¼8×ÕVãøc!-—&Œ½wUµ®—ˆÊjgí›ò‹!8¸ªÅÅæw!"Á´DT_ik¥*à<ëz„Ó	%[Ğ9ÅÏ¯ˆœšv•VÎ)Ğ•WÅÑa„¿¶„.ÔÓ„k·:xµ[™–	É×õjZÄg[¬S°CıËåÙÏÖ¥ÕJ+$|eç›·ek´8³9? áàªTEü½xÕ‰ø@vÎL«;çŒšÊ½Uõõ|Ÿ¡¿ƒÅaIMnİGÛJTBS3šešäœş!S¯­ñH ˆá%àKô±6„á8ï²Ó«épä‚³AêV>Õ•]¸j[ùí”èŠ4@2ıoJ³Ğ+Ğw«4!¸kgÿ£Ú\Uhîsù†ß×Kh¼_4Z}@7à/K<Ò²õõŞ„|ùóÄ6(fß¸†ÂBI½ê°§=LkÄ‚òY-Ñ†©Šµl¨èˆûkî"‹ğf&iûï%ìaíŒ¾‰#²Ïˆ?àyfÜ:ØÇK76OM3U•FK„¥kº+±ZU…Ø=UIEÉUkùÆ80‚°'ò7é,¢zš^+7´IïÔSE„QÈØ/
âCa1Oƒõ¡Á³=vÆ°Òå¨7‹tÒ¿]F¹×¿$”†¢+aî²Åµ'ö@p\è¥x%ßı%š„.}bÕOîa»©lvkˆäşğø9…t%I¯'Óñ'‰±XæV}2Ø câ›
`òC+õ­Uë#sÈ`(ïj®+uÛ~CÓ9?“·»£"ˆ«öÏ(·ˆOKÚø˜;.+¹d*Ğ½Ãè¹NDîa´¤ÿÍû‚i±·Ö
ÇKã²¢½+$ƒpÎyW¹±­ñF†ç@Œ‡¾³?<pç¯§ª›µM¢c3±)ÖŸÉßêQÂCyĞ©äà‹{íñ·¶—9°MG/•I4ìÊ,Cx0qWİ4­„ãgú²ì‘Ò.Yrn{Ö¿ÊSªuÄ÷,˜,¡æáÎ£uŞ6- eœÿO_£jsÿ×…cô²’ğ,Ä¿òù\Ì¯¸ù9c˜Q§ræéø?‰(÷<=şF©/¾¹g½!ş¶@üÏ\’b õQ@·AsôàŒ„]9Hk›˜PP†Säf`7IÖ¶‡lÇéÜı×D¢FV:"äz¦œÊ·½¡	X(í$µkµõœ‡Áái2 Í?ãk¸f‹øF!Ë tÌ@„WMĞUMüWIÇMS'Pb:s³}a÷ïj|v‰¯(‚.—ÄÍ¾ƒ(¨61Ÿ¼ÅJÊgïg KO¢èï¦¼™H¦î˜
4¤m	k4´³“n²Tˆïú˜‡`É0[®õ
góJjÁôƒûU·Û1~ï¡i÷¨O9‘¿
AŠjÑ}ÎåÒãØ£±ˆøÄI‹¯m,VöÏ>.º%cÀ 1eÿäy²íİ'….µèS¿ÚRÏr„P,[
µ±ˆ¾XZˆG^ Ì0 ˜s¼‘<[õ•Ù#ˆ³ÿ=Èm‚ZhusşæÏ’Ùéb‡×…YsT¢èy|ŸÕTâ½Xÿ`’wÈ£P
…t¤Ågu|E“|ÂÜº½pªÎÌ²‹éãe„I§ËõõKÖ7ŞG—ºÊRB(â%2Ÿ½‹ºĞ“ğñLÈX»ÈcÖtR£ü*H¾v­4^Wª½1ğ3-šùğ•‡%8¬yİVÛw–od·hºÃæ¦&ûvxß‚çÆA»,Qód ÍhNRïšÃ.…¯pÍ5ÿ4u²Šï|ò‹šQ­„{ª»æş,}ô.‰uãÉhÁà–.,Ì†&Ê¾x@Î€˜LQ:çfeÉs7Ïx =M U
ñfD¿»sùAÔÉaNìŞÔy
À@´Š55†ò5ù®Isó¿»Ëèô‘ENYY oä„î5C•ÜÃ÷H¾Pƒ,Á°…NÀu:ÿíÙã+¶XÂ_ƒ©y^X+ô2M¸´v„~XÔ,:ßdgüô¥HÏJr:jş•@øí´h£Ë´Wåµµ I@E(d#}¦‡îØê¼‰›K%Í¡€ ıGPÿ{Ş*jò±NòqTgµ#è[¿ˆJèÖê¦o=táÚ:ó¤Ÿî0hÍ)èkdØ!ÔyTõëÒ}{HãáÛR	ƒN¥~ˆhØp¨‚áy+ÛÕÚN’O<í’èG#]ÏSQ”‰ÁüİT¶ºÊROìP¦»+ëøUÕ_ƒìŸnÊ[†ßôÏ’|ZmígÁ–L‘Ğòâkœ¸1“¬3ïu¢Â¾„u‘sk<‘ÓšÙÆ*àqÆ¨8vÑWŞóŞ0I„f?:³±n¸(Ğ²^# ù‚k£®Âæ"¯yÚ_u7x…óÃ­' ÒRìÏş+ˆ!uïT¯[u ˆ)yw"x‰¶İd	åq`âû_k¨/¤}ş÷1Y‹ıÚ“‘€Ïd·@¬ÜËÎÓ\Ş_ëÒûç¡¾ĞSù’$ñ»Zë)-¹~· ½ ¦ù‘çäùíhŒuà v®‰×Ğ‘GfJñ€Ÿ Ø]›} ;©¡ÆÇ÷Gé›ãUÀ›ÿÿHÌr@š÷–^,!qÍ¡á­ªgÚÁº6êà©áÓ¶)©Îw Ñ[Íèf]sÆia¨Q·d§×£’Q±šGYS„ A¦÷ &TSGUò F!ñMÂtß(œ¦AvŒXJ—†Û}éî„¾‹‚L‡Äâ×'š* ©­ê°ã‹+±/ç2Ü“AĞ ¶Â
(.À`W/ğÎC0FktmV,¡oÍ_¯Ş‡ñ€o…°Göã•ç±Ÿ‡¥Éh›ìNyw“bñ(PD–fN#~@İŠüY¾jE>â‹ë”ÀTóë>Ô¼¬°ğmÀG`±#Şp”7ÿÊº˜rğ’w£CT›Ú[£) ğ	§aâ_MY}ş©şõ'…)KÌ¡K®~ñéÃ»'"ĞÎ]'éÓ[¶¡ì#é6˜ŠØ‹î‰Ñä´½Í»KXC¥çûKâ°Ô†İç0°öı3~ï„érØwì`é™Ïd>ÍgĞù,~_ÛîÕ¹œï_>Âº)’½ò¿úaD·¯¤[yšæÜ¦=Şv¦±İ/«äç8ö6òî®oS f	äd×? %€(P€8ÒbœÎÁà.jkqŒV`ƒ‡T[,÷/"ÒZª7Œyj³½¿ƒ	fy Ç¢;™“>×,@.•Wî“éxùè¡cR³¶4t¸ëMS”…gº+ß% i"¥¶z©ä!<>(âÈ¶¿J¼•Ç›°Ù«S`İŠF»·…EJÆ•p½wŞ¢÷(W°¦Î®µkdÊì±#r nWg•Ú/¼wÁ‘dfPúÁŸ0%u$`ß¿?-¿á~S²ÒN÷8wÙ.ùm…İÅ)Íf{·HÏ e¥ºÿÃv’Wp.ŠöfJÌDß•İû®ñ6;Yİ”˜TƒîtÚDs·’'Z¬’>ƒ€Ê¦?«Öi¤ÒAù
‰×OêÏ¹êÕ¡•gİÙ°Şr	9!¢úĞ„®°v‰+­ =ƒ¡éx}íkbŒkõÄï¤ê+q)t@i€[Š¦ªŒQ¯+“"‹vÃk:~ş”`÷•¾¦•ªS“män§£˜6x¸Q yf-_é'ëÏ/=²^Qnã¿0Ã84ëièı·_ìd²)(-ÄÇ;:&É4“õîê–öZÂ—bÈs¿l¼ÂW¦7dÍ@'?áµ={Ï€ á·$hİ¤U¾È?Ù¯BÄá£Õ^øÉHª“­–ÌÀAêÑÍ§LÜíx[ÀwÚÌ3ğè“_Àÿ(èèKÆyo:}¶ãªNÒ´ÿD’îıÚ–âUÂØ§¢ÂşşuFö ½:,œüÌÖd9‡ğ»}ŒÛe»YWáÁßÒê:ä.û3çĞESÉ$— Øsf2Ñ¹6Ã5g}u2¥ac—u2ıIØàB;ª^0qÒj^ñ0HÆft‹9{£(G«,\³ôÖÁé'b¯zéÈ®²}s'ıSâæC¨Z©å 4Ø³ßôn`bé­ñqdÛÕ“ğŠª8\G .eÊ4E¬_ÍKªn2Éil¾jö7shv›XqX%l‹VÛê¨Ì	ªfª¨v\Æ)Ö(èİM±é>±?*a;–½¢KĞ:×hµ$T×©Ùç/&&ÆŠÛH—¿ïò¼Rõ@•{d¥~mœèBŞèÆijedY¨(	—kN˜<š•/<§³’vŠò¦zù¡—œ¶fÊÜD%¶É#o>‹±õCïğcF1â?´ğ ÿt$a¥¯Š1¾g¾D5uÚõhIßJ€êÄ @5
âæÔ@gæ!ÂI;f’µO_Àâ‰â?Œ¦7äzòNZ®‘zÄ‚¸7(|Ç"\Bğ«PÃC¹0ÒjËÈ†U]ˆ¦ŞÇ­3S•ÚŠ\±	 |9vÁ«³ˆ?I
Y”Yˆ1’mş¢¨°œ8-J¡u58·½«@¶T×	ì#`™ı\·©[!ceM_¸¬}{ß§iÒ)óO×0™%ƒæØ„Õ:dG¯æiOHáYN³gOCÑè2É¤ôÀ±|×áÉÂ¾Í²R#ËÓ`¤Ôlô›U¬Ô¡=eXD¥|¯]oæLmç)‚o]`‚ ÉlšlÕyG7§r‘ÛK8/­ZŠ¯8hå¼¦ßØ»©Y«:‚ä›³t…£4¾Ê…I	q™°9%vìØ‚³ãÃRå#üåô%¬É¥£YäÅß”oPıØ¦j‘Ñ¾“VTmêEÂKñxØKd(´Gïñïh*”šÌp¿‘Tøa’O_RªÈï1¤ıãOîjÜNçú¡Í«l®¿¥0¶;mO/,"kÙSø‰âƒƒ8ÈÄ›[X¥"Â!Øhé=ƒc@İNQ 6’Û	ÜpZ E½3@Ï×^'æ%U¨]¡«ÉeG Ìõ'OkD÷²[@Ş}ï© sÖxškOj£—xåòsT%p"˜SMZ7ÊB|
ÅmLaê!'¦¾şÁôsQ5‰ü:Ô5|¸Ï³×|ç+È$š/'w|;ÚòoÈ QÀJwå»„5[½IÔMô4–hah•w~×b|Æ@¢ª€H‡å”d3=êµ
Ç[ÑGh`Z y9ßkê)`Bœ4‹"À^Ììü“òÙûíú¥Ç z\ÚÿAÏıÌ¢òªÃV›´? ¾{v|[®Ä zòrˆäud](ò™É„â@EmÂ-ƒJ¾—yÆÊ<ZœÚÎ{<åŸ½rØ‹>5…ê)ÒBÒã7ÏÓz Œ!Ã34tpUƒ9L­„7²Ÿî­1{zï­ä¡=D~»¡¯ÿfË*
Yó›+Ñ fÏk§©$XLPxbá®4Kø*0|8“Ì²4Óxw@É%Š²†g~ªGõPà©?g×.ê_º·0í|›ğ­¼1<7çE3Åt›m³ ´²œ™Ÿ}ö±tÏKĞôÁ±¤ÏÄ‡\EÂnº_îÉJ›ÔŒ7óW°ìĞ•¯¯Ñõx}µ‘¡ Ã„½‹şÉ‚i)Í˜•º–píÉu¾â’ÁƒDa:ÃÔØpy·õ²"Ğ·rsë.ºCè=:x”iÈT´Òœó”4ë†ãéŠgaletÀ»¸_şÃôæsÜ]$K¤;êm€ƒdeòdàü ÅöÔ@GÛğxãi»ÇìLıêw6ÀÇĞôs¦Kqœg^°!*¼–Ç›h„µø»4X¨{\Ô%9t­qBç4Î6øí“8ú¼zÖ³Çú¡Á+P‹Îgmº}VŸìhZ Nß¥é@8¹«¢óezÈ×èäÜ÷üM?Î/ê§óJÔ#lİıdïLÇëÁ7Ö6’Î˜E$€Kñl¯›ˆ2¯Ljí?°€æ½ÇêFSÓAàôt¬çÊ_yÕÑX-†pUÁŞîïOÿ~—m6ú5EÅÇW­ó¬çø©K¦ÙK¤Ìgúqî¤GÚœ'6³/¾Áu/ñÃ†p_õAHßuùôØQ:¥FÚğ€±™‘JSŠÕ†b"ºÖğ÷º d°ÄšëÿËNŒ’Ï.š1c“l€ˆ¦(G e""°Ğëj–
æ¹ß› ô |]úànFİ².†ulÌ¥ÛÀVQ!èÍşç©›óâ’(”4‹ S‚[ï"gx>B”ÔrİÿÆ òÌÙ†ûŞ4„Hä+¬Ø …’À§êŠˆUÃIKœµ{¯‰¥kÍ?â—í
¶á!£Cp©5y{è@¤·Ú¤Â§sxSò£ ”5 ëg ¡?2ål†ì±^SË*ä¨;›%Ê¥·èåEÆä18oÖ”ÒÆƒ#=Ã„¹^²31ÿº{©–L<Ø%Éæ³èø+éUèNe\JkBãıìtî²ùò|æå–¾n0ùŞeßĞg-ø'Ø‘r9ˆBtv¼W¥¡Í(6[ß€ö8@’„İŒZa®/®•,Ú_²—!×»0àß›èã®3õ
¤õŞãWÍƒáÎŠ‰®ã2 ]2¾¯Aù=ÆøßFì|<
{o%]÷]%¡}«À?Vt|§RLªdáì¿Ó"ó‰V¸‡4¹‡[VaÕ k²Xp‚,Á-,…sG¢©<­a¨÷ëñÛ\œ¢pÔšùZìµxâà¥0vÛuŸ¡9]üêÛ|êL_ãDïªó‚âÌf» 1wÍ_‚#tLNİ3ó·¨}’pÌ„Ãc-oKv™-LbÆü °ÙA{˜Kè7¨…µyß0Ñ/g·§­²:„ƒJŞX`Eã4zµƒAîlxı±ÉYÌ…æ;Ø!]y˜V•Œ~»Wvîä7v‰çÿŠ+¬£$UÀ£¬&,?|`•N >Ö|I æ@>öAø*º©_Q*uÑƒß¶D¨®Ûá¾êÚ-ï”äòµƒ/ìí[b?•äÊ´b|¶@÷L;™;ÜÆUêİ‰B.ùœ¢%,&ç* M)-EpçÚ¡ÁãƒœÑ`é”ıa\Ğz­œÉôXÚMJoúvÀêò¯”„>KÊTÓ%¢CĞ¢
åvçötµ,pËü]}Ìë³ŸÕÃvü%šû€¢õïö‰¦Å=	i¡R/CDŸÆ
Éğ¤//n‘€dÕ»Í0‰?	Ææ¹vîáƒ¢İÇ!áò>Š±¾†¨ÖÚIP¦“ÊáV³G/ÚrgHŒĞl'È	w\j3pvŒ¥pZç•œ;„ßÊæS‘ËŞÚ_PLÖÚb–‚Æ¼m¥´9:*x‚¾È«¬rsİˆ÷ñ«w<¹Út¼y×eqrøI ¥Àˆõq5—~¢æÖü«ğpë’¶ÕÛ"îeëîx²Ø+L÷Ë'¾TÕ»79³sÃ^ ô¶>1µ0:±NÆ,~I‡Ë€ä-Oëâ±„ókÔGdC¤şCoõ³ª$RWW\¼ÉÉš]n¡x¹ç°]cÖ	ß–NÓsèsÚKÃÉªäZm| xßA¸ ~DÔ8eİ[Ç˜Swz™Ó&å$Œùİ½Ü)ÔúÚjâÄ.½œpWä ú	ãêO­lo+*-‰­M}Õ¹”‹£÷•
%S{'ã†€k`t]ÖEÕ§PP¤hR|v`ˆH·í5bÕwQ©ş'Ëª¶+Ïäç@YÎñü&OlÓ‹|•U§£-¡ñœ¢ì;™Ìü<-‹ÛÖ8›¿ÓÆ÷lBŠõë³	jİÊ¶ÿSs¤+ìvÕÄ$ÌttYŠB?¯¶:ØCğÖ€”½M`3²ÓË½1ƒ3_NødŒ§»]6M6µtÂtdù»œwZäLc€‹pà¯1É½;ù8çÊÕÖò’}|p|àŒ–›;Gêò¡¸'½®ü$N¤ÏôJ<¨ô~cg£âˆJµsßÑpIxÊM®&I¢£²>sÒkcìÀ½B¯-‡ñ}sêiˆœ†,<ÌÆèSå¼ÙÊ{µm¦Ò…5“aqmÍ£-Bë58Ş|f¯ÜìğXåùàyöóßík^±“nW¨Üt‰[ÿrÿñG‘ıb5H™`ÂÁ·_$“¥Óö7
û7ğ\dqZó‡;Ñàñ=8­]Äô!O&Ûö¤¾Ş•.İ\¯BSY±Ùâ½YÀ“™‰Kœ6¸<5ÅâPÁ{«ñ|G`ëp’Êüò¢şG†!œ†õÆ­­aš¼û°a=C/
ÎMôJ›F=¤¦”[ˆÇyáM"Âk#³á@åèOÑ§À‹¦>­‰<“(hä? Æ÷}fûé¬ÆaÍEİõÒóğ…HmEÙ,0~D…“Rg;y‹~ÁÜ ×½Ë†àwQ-É¢]©îĞêGô·"?*RÉ´$úâ"Ñ,ËÅ	ÈÛlJä?Ã]ò-Z¹ĞÖÓÇ¤,#9·–ÂHKŒ¯ÅÿpĞâğí›É\6U<zOdİë¡áKwæµ™±/ŞËæ¤F\\Ğr ª *¶3İœ@Ô\ér7äùó,îjòcsœ¶Y_åÊ>úûwììõÙõÅ%ãvsóˆ¸'Ç ÿD^9‰2RyØ@!İÆlèÛ¾ÿf}âàRß•‚ı–?:„¯óaxpa0{"E›Ñ¹\|‹–·V´ÄÑ=š’	\µ¡õ7Tø;e>¾;V—"(½y‰ÀÅÇ=óµÅJjµ¶Ğ¯CÅ~8!×F™ßÁM6aC:U™-¤NoÂådJF#°‚¦¿†
Äø#Y;ï–;ò# ˜ş¤ú ô¥r”ó‹«çitAÛÒJßx¨~*	:©Áô~`À)Ä™Ä&ˆâD§HV+lˆRÁ*Ì¨ÿ¼snZTq:cˆúXÇ.¥!t}ó~ ,‚˜ı'qª›è´Ÿ†°$Í0SCˆËMä›è‹õ
zÇ©%T$Ô¼5Tôbªj ³©Í|ºÅ©€yNšä“.m~d óÇpò^İ$ÊÇ¸í¨ä“{Ã	¾¨Š¿ŠkşBİM1)ü‹’||úÿ×T"q­00ºÍ[“@{„›0¡R ³×CWÆu,—qT»Äáğ4i€|åRºãÍ±tÊóÓaQ>Ç{ìA™ôR¤åéÔÌ~8ØnwÒPNã6B—‰PJ'L­PbWÜ>ƒg»'|#º´İœM,+Åbh-aú<x¯ÇÆ)V—ëşÿ'¼³
NèS¤ï;ê–ß.ÇÎvL>ãnÉŞãH ùµÊ8ƒ+H-»ål€Åü¾b"y,ÎD”uÑ(üJØ¹ôãB‡EUDW•h¥·ôk$:é¸¨¢¨ŸöÃİ")[vØóû®é2¸È÷œöøy¾¯[*½¬)C&ˆÚş•WaM\¤ûÂ€çAQMSih•·8z˜+ƒL"4r!vÏ1H
ÀTØ¦xş'6ü¾öªÿ1†ñ^Û€{w¦4Î©cŒ†F£·äF½!œÁıvø»ñûÜöøœP
Åşîº9áu3¥°9Æ®LbŞñ3Je`Âå`m÷V¿”Än•A$ß§#0x&wÊ¬3²<[hr¬F wë?‚;Nû<òGÔel/'ºÓ¯€bÇ­W÷1-‰j¦Ÿó«˜İj¹êµø¬ĞüÑFèsLq™ä÷˜V¿ák{Ğp²JF
äËğ/= Ï4Ÿ&·ºÏ5ˆŞ×³vsé<&¯Yİ#{HíûÒp«¢¹]M®˜–__Ó¦­ğ4	Ä)éLK™~­7+!´Z$ùWvWK«ßµh 4\;¥õû/Iì±ƒz·~‰”©Í©ˆåÃÇÏf³mû©hX„âlÒFrĞŠ€cÉ£él°<³_˜š¨Q¥!Å¼™B	W…L6¥ç,$ƒ8f´05..%eñï<¿Ä7»¹T ÷€}Uæİ˜I½“6»vv´EÇè%,•ØÙ•ş²³;^M)[~òüDJ×B¦#F†²¢T¾R{NÒZ¨!ÉBH!ªö´pâLae´ Kp‹íT¨o"W•®@ú¸¹UÎGşÜÇ¯¹×”ï{*Ş†|4Ø2– [;ÿ$mÒ9 åúõ0:¬›nZJ–àñ§ÊzÊ¥„wÅ›ğ˜¾úa
ó-ş€oÏĞI¬6ÕIi³mrß5‹I ¨ıëªò)…ÛÍpuÀ9dŠÏ,™hÊòdÜMßß¦…‡ÊpL·x‰(zİÈ~Äé+ë$÷JñD´X!T¯KûÚDëd·!Ú`¢ïÆß¢|ák“c2¾æ¹ÿ?<µ}9¡7F˜mO¶ÔÜt K&ğBT{-Ä‰ù-sc—<â}µÒÿôÄC©AM×è	U!"Yj\â÷£ÇÃÊsbAÙµÖæê\e¨nÌo»òè-ŠH>øµÿİl¬üüÖã
_U•6Lr;ç+Ò½\÷i-öòo†'î…€RÒB]#œŞË¤²˜Å•şUıc¨XÓ˜°˜{ÎeA5ß³B„ÕwØfYqƒ2fİÛq&3}³8*Wf 4Ğİ×5íßì§äkñ»jèrR²İ›˜õÜ#.¡)XÀáI¤—ÓcfÃ4Ôû³ûL£ßŸ€­ê”Êë4kh{]¤!â+Cú[<–^dÅk!£Öéyë¾ Ü4©p%«D¿8Jùt£D¸¯…+²§ÅĞ»Õë73œ¼"YœÉê£O]‚NbÏ^"Ó\[è‡.C)¯ƒUÀZé ğâóå< ³“¨–âš|êFyøpšö[¸ZÀòƒ¤/êÍïyOç¾-Gı‰[<Q‰ÃTØ(ê¦t|P‚Yêö¨JÑOuÔóÂ*¾ùëDEx°\×¹IË—ó(ªg`øtíuÓ
k'aOÎÖo2L—!6‚*ë9#Òè ì–0Ü³0bdtw'³½Qï$>ONxKk
9ºşKhş±¿÷Â£‰ü`fÿÂ’7›'mó}{«à2SÉ¡|Ò¿­ò95)?	}5î=VÖ‰æˆ‚(cmL)ÿ«+¡,æt‘å¢•éBÎŒâ†8°Ø¢.¥“9Û†#++Ìô\½&Ä-egÍ_³¦ ï	U‡è?Ú(K'µ"§:‘“|àAÌûò(à~–j4ŠŞš·lDŠ¹÷(ûLLĞ{$\
bÜN×ÚçY@"b’™›g>’zQr‹Hd3Ã#R‡+µFˆ/ì,¦àüån±6Xdh€ûÔlN€Œ`Ë†á35\±â°"Ç÷ d"B‚ƒ=ÇŞ†º7³¬NCB2´Œ¯fÂØÛ™Fv¥±Æ¢á	õ×P’‚¢ø‰ü‘6²¢4Œ^Ígåsäo1Ğ{:ëy¦?î*—$û]^/*£µ>}ÕÚ†$ëê ’ èJ	sÁËOÓ;Ûmâd ÀÇ¾zÃ0âD‹aòİçO>€§š,ÀEeïf{Fp<3Ïÿ4ûs%ŒY  <³)•Û\^Ô¿oäï##Zå­Js"IôºÚÖ$P’ÁğgÑv¦]8:êÎ] ĞÿûÚB)İ‘&+Ù{åFÅü)%V)|Û1Æ{l›å{«d~V’!&Ä‰›™´øÎ´ÑØ“ã¸—0ı­Wz·9şãvÈùí{ØP…MxÜH
Ôé‡C
-	G{Ò´µÎ’Æ”y¶ÇŠT!–=Ã×ªÎë[>°Û¿Åw%KÑ7:?å u“Ù`tQß@72¸!ÜRà`L‹œñŸµCèHÍcNû¿‘GÍøn˜DAI}iÀğØC«“¹¯jJ÷åY>XøxG(7›§è¹j?%Èˆ ù•º,ª°Ï+µŞB…óù¶—:LA@Ğ’sÏl:²¢Ù— ›±¡A,ĞùK¹jÆmùw°QàÚVw÷â¤GêşzÀ¿¢ÔO$İ‚÷ÏY½İkÒó-û#½ïÇÏ›®ş'$¦°/sSl>øãg‹ŠãD¹÷÷ëèÙ+MÏ
‚ã±¶Pi«gã²Ë!<q=‹ûm°ú•Ş·è“Æzš€¯ÀN¤W~£)Åxla¨_HPÉšª=”[åŒ|¬	½¡Ò3ŒÌ2üI®x*îÛğ5Wòe·sÓšĞË¨Cd§ğZiOÖ8evXY^êL$_q°…¦©ø¾™`àH}?ŠY*$[9Òt{Ûh!ïnf&%"¥Œ@N'XJ‡Ú®ıôÒ4ßAIò=.çx¸ƒw¥1¤ÌLA¾¾®Œ¶–å°Çe†ŒáŞGE»øÑO›Ì¥ªw_:éHÖ§ÁävV¨¹Ø^ÀK¹¡¦²Çm]å	{ŸF &­Ú‘G`
~07œœX·oÍ¨õ 8øè®åš…Üi)Ô&
SUluûöJÈ29Ù‚{°ÂÍ UåÆ˜í
¼L‘ÁÛÅs·)x¦Àk¼·ÚèN9k.ø¿‡),ÛTMß5&‹ÖÃ‡Yò±˜,²Z‘Õ?¬^°ò+C–€îó <ù¶ûúHS½bbš‚8d¹×û«oÓßrw(Wû}ÛhĞé®–ı
[¼¦:izÆà¡ñvéÂz_¥şƒn¨¶C¯å¾Ÿk+zìÉíLÊæ²,Ùàÿ¥B²fÒ£ÿ¨Œ°-5í¸­¦Z ùÓ¦eå Œ<®²ëè3…§ÉCpÖ:§€dÃ4¨÷Y±5¤«TñBËJŠ¯u„5ÛèY‹i¢M.‹‡	€)®×&Æ0âŞõµÇTûsÃÄ4.~ÇPß›«yˆá–®88pUÛÛ$³É&D–Sù™Yë0û¶âŸZÅ×%q5x›<Ô¨û;ÿ›k‡jğ  óşôİÜË„¸V³Çæ/¨1SÇªªl2¾@¡³xY!Î®­š)MA‰–pÌ‘•ƒpıå1’¤5dØN½=ĞôÙ8®ÂŒë¬ÀÓ;1:5§ƒwIh7èÓÅ±³]©ìÚ&è6™¤îĞ‹ÿ)µ’C0µ‘MÇ%ÒŸšº±(Ñvx‡}ÇùSap2gGĞ@Š,}›z¢hE•è¾“¢$Ú†›h\ás_È×ê[¤/jÃ[¼1ş1OÈXMëf5®‘_vzA}l>¡ŠiOa”ßb'ßy~™ÕZğêHÄwÛ…¾O_ù k”EúÂëº—0²÷
[Lı€ÇaO¯Ö\o0E™Œ3³mñTÆ<¥EÙ¨æ?|×RÕ~Ã±ó§'§ù™]&˜ û?€J\<)pØ†eÀG;·Wì1ªc^_D´|ÕSNÍP\è	aşo¨–Œ®<ÑíÛu/P>6¯¹lóV{œ=Ï={6¥iü²$|º~ìÛÇuß`:Y’÷c¥©»`w‚¯tÜMÄ‡4åüKşl
‹÷Ùÿ÷3ô)Qáüy¯­N³$¾ÿ+c
}Ğ ª¡í‹os_w´õ¥n—ìŠÍØ%b›‡¨šk|èºû°®è+öñ¨~àÄ.Ûÿ$ÿDL«k˜"şöÏ"Ô|}f§dYÕ{©Ã¬ì;ÇP;€“¨sJ|î¡ìé&Œ©J—uÓÄOŒ^O¾N¸B«èß¬5ê’]¢TV;u0_kÊêP¦>åCp¢ÊIåŞEmÙº<­öQô„¬V`@$F9ÊÂÂ¡.­j‹¶Ò¿Ù G.…ÚŒzÛœÑ[eœ§â=”IS[Š‹ÃŒÓã‘Èmô6ú™YŸ‰mÊJC‡«%¥F‰ZÚ½ãG‚!\ŞÍøÍ¯«¬µ>Ch˜ŸCSÕ¡À§¶Rÿ“Öz[R¡/ĞÊ¾WõZBâDÅe(”E¶Ì® –	¸›^ÔùtÛc>? /kõyµ"‹*—ŞÌ˜ôdõÏ“U¯W©%ß¡Èæ’Ë:(S}-úG÷Hçª5«V?˜‡@‡5”· ƒ#µjİÒ¤íêKQe¨ÆÌÀ8uÅèÒqùÁNL‚/‡/šd£æºæ•yàR›Bš•¨aK—`QÎÔ“Bûâ?ÖH¶~»ßŸ>Íê(zeI=GŸœ*"¿BÊ-Ë8Ş[Ü/ìrv0MOfª}\ñÓJ`‘Ë,Ú Ize©6c±”0à5s¬+@7šğ¼äõ-—Î–Ïõ÷ƒ- NM71K›}™^ÆôÌ&Bñ-”»®†\YöQ±9˜GÖóDqy…2íòipìÜ­ÚX©úÈ$Rt¡$äÿ•Ğ\ó‡ª/")zÅCØÖ¬°v@çVDV‚së”kñß3)'ÄšË:9£kÇªõ9DˆèÑ¢°Ì9B•ÓøÀ­ŞB¦IŠç“c–İ>/gfÆˆrg/½”à)˜,3y=ØI¨µ…FƒoĞe²º…NM-HäHf™ÒŒG¢ÅÈU‡­¶pÁÀƒgÜ]D®,çã şYgwgßÈéºû4Ïh”ãìf/òÆÇ0sH#'f¾qŒÔ:ÜS• Àw &q_Ä¦£:DŠÈËÄø}fÀJ<¸ÂùW™Š†Lá¸•Ò—×°8çà“¬³©V™¶@‡Ğ^?9fóï½ïRÖyß‘©™wÀVáÍÅ‹òw†.+£ÿ¯Mˆ½²9æ$4YšvÚ‹&CÚ±B©˜Bu:H¼•
Dà˜¾¾¦Maªë"<OØÏ©Úø1¶iJ4›¾ƒ¬kÅØz<2OÍl‘¾5ı‰Å‰)J“£Ó[Êä~´¼5/"çC^çüµÍÏ¤Öİ@‹ı“•zL&Op¶…¬ùødú‹Û<hsQüpÁ1eo±1­&3„ä7°Ø?À{(	šÜ]È¢ ª‹·&<6 Xç½sævÙæÄ½(«ß€JGøçKÂÿ,´QA}û¢
¦¸[æˆ?Ufö,Ge¥ pa_Ï¾¿WÖ0î„§â2ÊPFNÓ(z¡Ö${iœHoT2q¬IÇÓùÂzòY"ÑAŠ<§mnéÆ—¥±Gš{ä›é´¾‰µR9¼‘ì³¶Ã…ºà­¤4İ(ô[øÒdœ²eóWªÌÔ£×©JÛ‰yPs¸àÁQÁl”yù[gŠè¢¯ù÷iûßìW¦¬òÊ^'Ğ»‹©ÏhÿÈ!€M{	Æ¦C)‹µö´ó²ÅÓòè„¤s%ş€ ¢D—R4êÀÍ¾x‚:ójŒsK\ñD2Tf
çñ’(>$ßzÊUFúÉ•equBNe"#A&_YygguH¦h-2›±éãFx‡wû¯_-ÛÿEs¶62îƒ®¨½@BQ£[ÚĞf$„A_s·¹6ÇÏÛª~]’UÇŞİmcÙ
ªÔ>«·¡ÂÄä;ñ8g¶ÜÈºÙ²o–pfÑ€ÙÒbc\šD¶ØŒñvs«œ›`Í¶ıX¨'k£íÊ¯ë¬{Û}&ÑS
éniú:õF™÷LœTºbQvˆ²ÕèIªÍ&23Píœg~›<ZIöZ?|–ò&¹Ò¹œtÒÀÊ}¦—féIoÈàùWìê úM3Ì.‰£ÜâÃ òÁ{€èÙ/ãa½·^.‹©îæx#ËÊE¥¸ŸA6ÿ¯æ\+B~Z¹Ø—ÁÄæÔòİğ©õÖ*C^ŞŸuâ©*Y®ˆ£Yi‘•·ºd‹ı7§¨Æ²Eşƒ>¦ YómIúxàókºUÄ“‘ĞÂ)›Ğ­0çôRø|šÛ#‰ ›\Zí ¤ã!kİ4ÔSb¦`…·6ãş”¾¯¡„†h‚g;  ‚­Ù—-.†ª×x:-bæ%ko?öÂÃ‹µPX¤î#ôªôSd’ÿ;	Ü_¶KŒù “—Ío1óqÄGıJ”u©Bõ{[7‰Ç‰ı„pŒÚ¬ ÍÈa@©Ôd?Åv…ä}ğı¥;Û½ë^ÏIõ
q JîêR­ígØËmÕ
ßwój„ÎºÇ¡!ç âÓ[ÁyUËiŒ@à`ÂN>Í;o'FÉ±¥~Ô]]…šŒL…te6//\ m6’¤°7‡¼ü¬´7»¯ÕFõÅå•TÁ¤õU‰0Ë„£jœM5ìå»
ìy¡,8Ñ»ğbiÀú*>¿´D+«•Ğ¸#˜_İa'×£IKßê1vUÖÔ)#6ğFqßªİø¸QLqèêxC¾ÛƒÒ‹1jY¶šî Ğ(âS½˜LLHXÊk‘D;ÊD„Hy¶‚ÂĞ#ÏºçÄ„à°^ÀÜ7|¹&R<3<¬º’kH¦Fı«Êä^st’x8~G9ùĞx{s3t#\ÁzBQâ÷U¦É4æL*CS÷¶v´tnmÀUsm¯ÎFç’FË¹„¶Iş2â"ÙÚşÓ4áé«¶şÚŸŠÙ	<ƒËH…ĞÆÀ!·JÒ¹XR/Gd[œ¶WÍ^\WˆÙòvº¢lz5&¿®EVÚÌºÑœ²DôÍ¤y£èâ½x¯±se]º(“¢4æ£W7Õí‘êhoZ'âËÅ1HÁ5J· ±C"ª–r|Mi_‰‡Ï…Uâ*ôëÿñS'ê
§}³äÄŒFT='<SB°µW¼	ÖÏ±c#ƒ–M£N:d+»+©VFiÆ[úÒ İÑRy-¤…©8Ğµ¢hg5{¡4`ÆÂ–ìyq†X¸GS‚Qşæ®(E´:~èùdTEL5C¹Ùãzœ²E°”WR¤„Øzøèe¦öéÄ3í§<ÁÓ>íõ.T’Õg[h1`šl[oWÓzé²YÖçUáhí1NF™fam’MšÿrØß´¿DàX(=í±E¸è<Ñ ô‰¼Ê*ÇåP—ÿi›á`ÄR=£êIëfvĞŞà™íAm—£³>Ğ¡äXÉØsÜ.æcL$ÓÓ»*¨äKòÌ›J€É@Pƒh‚n Üpvæé¬â	s~	`¸º)IAËŠËæÉ]oÚ¡éÂ¹h†WKòÁ¿Ê@TqrwfKj<¾c‚£:[{´Q°†î)9İ‰Sƒ}D¦—ã¶§Ô”¶ç!¯Å»	Ô;Œañö˜O’î€¾¿ıC$ÃÆ\Æ#Ù¥#TU_v6ééÑ]²ë]-Joy-É3¨<¤À1b¸IojaŒ¬ÍEÔ·Y1…Å.ÌZö•SÖgY	ÁÜ¥ÛFÜeÆmô¡ª]d(à-¹`½Š:;Õ ©w°N»Ò„J#y÷ä²ŞC¸Ë9½å·±(‰4)t»ŸƒÑ}¥!ïú¢.[¨¥[÷Éå#^782Ó¹E\F9zs ©sûos)–Æs[ş>
â¡˜İ?[#?¨ Ùà¯(L<Õ°sf»¹ğ¹ô2‹T™¿¾ÃÊ¨©^H‹;½ÔÎ@
ªÅüüósU’ó={=WV!<…yP	•ÓÒ¡ÆO”ésn.xT)cÅ°‰T¥L3«»ËÄ|ñd8DsªŒíÂĞY2é[]©Øm«’>ŞúãSdYë×KÊ<Æ«kŠÀÄµ` =ì,ĞdoD§¯ûªƒ©ø)Tuü_Ñ2`¬h-_Ñ÷óKiAå€•ÎáI34İ¬êvRØ{ış'+»>Õ­óg„æöÿibáLÌ ®4ßâ	ûCâ:ŒXBGˆó€»†^ù
Æ–‡Òg
Ú\~Óúš Í~z3:ê¢cQ2jwàRæ©¯ÉöM¤’}ecJ ·jÃ’Ü3 ëöäİ 'ü©[{mâ–G¿c¦§zÊÊ!bd°lªÊ§ñìXË¼eE‘0lô ØÒ ®Ìyh*äUîì*e_hí×:LÈVû²pÉ[ím3™á^ Ş‘NpD„Hâïµ³<~÷MX®¦–Æ]sL<“¥‡Ø=¿ğ:¤)¼Íx1ÜL…GÏR±íçP¯•zp‘ü[Š7GÊzr_“h#˜ıÂàr?V_<é}ùHmcç”x„ªÖ{‰§¼£L™¤+íÀËwø¬Â+I}zEC®ßÜ¿¹«éªÚÜDXTŞím‚À‹7Ag8Û°õqÌÁ‚Fş“·±8A—.œ‰œˆå¦ÏS&à¿„Íë<€Â!q[¤ü¢Ú¢y¦Võ™CL(	?½Ìxÿ»->Iõù®ü ãŞÛ[ó¼z©o¸—c›1}6Wô©ù(ø'4ªËa§B)JaÙK«]ÄI¥Â‰IİoTî%8S`!İc ¶rSéœ~h™§¤¿ t¤9°t	8—]bZ_;:”Æû­éÆâ)!ï[/,¢„¨Òı Wµ Ô1¾u2µQO¹Xh©ÕÄm†æöWjéÂİ\5m’-3A=_0ø€¬
7¦PTS-£Šı¨îº¸œuÈ&fßZˆ²]2‡åx®ª-z=¾‡% -ÆNG·4évÊl9p•œZÛKfÂM<GÄÓ#Úü©wÚ‹ DÔÌ®+Ñ5+ÊüÙ	igY1Å¶ÍÎùµ¼ q,=áR?ƒC,Ntğªq–
JJËa.”ÙŸë3“1¡¡Ê(ŒÈ×¯Ø®°˜•Lñ1İ:ºT‰?~j€ˆL"&¯3È÷ªâ+“Sb9a"91Àõ)’†¨Î#oºèDV1ÄT{ñ¤À­¹BN‡æJë²[×Œ‚N¾"»«µÃt­ 0ô	}İ“ö+®z¸†	d´«8Æ²²Ê/ïÇGÕm•×ÜøÕ›„dú“¼6‡\Úñê34í¶‰ÖwBf‹#ŞR‚ÉMâÛ¥î« ÇÀøFn1ud*|°óŸTÎFm*š÷Õ„¼nÙj›­¹‰R:Ï¬lù}®{Ô«Ë²ÙKã€¼íyñWºœC	ğøè"€Î‚±÷ƒ†A*Ó®f~C=°Xäİ}òÓW/‰¢¤eEŠ‡Ošÿ_ –ûiÓ97	`àWÓF	á^Ì·UæXn*)û™›hÂC%¿úS-Şm9lèèšÀşd^è¼õ¨TnclN»Üf¯‰DŒ@F©Š™£÷Yèã5#’^Äğo+¸Ûñ2¨íBvü©4D¡³»î	½ =¯>Ÿä
aã'˜*fûP'+ZG¨9	[¹êv%!cƒ¸ÃÛá·]ÁâÈ×'Ó´”h1®-ÅFÖ%E§Søç)uJÙ*ÜçÜ (°áHü\mØİÊµë‘M_^’@µ9PV2+Ï·mèoó¹ìs€4wÖşW×ŠÅ§g°’™ 6H1ï1‡¯Iµ‘i’Æxr‚çŸ!ê—«ü[­aÎdBãÃ¸ş( `‹Ê¼[KµÅâÜ„òz¼D„z('¦i&ƒ.9¦H`}úü¾LÙ
OiıÜ“1)ÍYc­"Øá¾–ĞÇXñy•İ)=}Ñ¹"¯fİÛTDç´v7‰6ÎTvn),ì”mŞƒk;ïlgX£ÇÂØ}ò¶
éğßg\%+È5*dØp%}¿+EñË(¥I;fàTK8ÉĞx•Ï‰EØÌc+	ú(;¦OtÑ·ƒ¿µ]½Ó Â¥ÆïFÃ¬-íz¤F¥œk¯Ö@ş²ë½?lÂ4%q§r|j"cVÙJ!ƒHl¥¿ì;‚AñOpo—àîRZôÑF7(Ø‡ÌbS%eúÃÖú§tDn:ÉCÎelâÿåQ'ˆòjM' _kh>ø4€)…&iğt“/®¾ŞAéWË—Û	|0!…ü‚óˆ¶ı]XÁS‘m„@0ğGòmİs½g¤°¿Ì ŒhÙ£6‡ÿ%Ò©ÇÉªÜ‹*KŠzÎÁµäéÄ¯_ö!”ÿ-‘´Ï`s sÂb;Æœ(\ÜØPÓÒHºàÖ*¾¨ŞTı¬û`Ö
\%p¹‚ê¤ÉT›×SLŸ*{CŞŒy	8½Š\÷l·™±ÚåÂW -q_xÍ•ò?zf2Nµ—Ó#ÀÆ¿2¢@ş~ĞòÁ«€P•ı#1CëÏEy!É~×CdĞ{ÅÓm`Ù>ßÿÓpï®…•ËvI‹Û<¯bü–.Ì½yµâYª$-Œy¨ÆÅÚ@a¿Zc]Ù±ÇhèÛÏŒL	yzè æŞyãÊêc_µ;1ÈÀÓ™/DJÕ%LÂŞŞ@¸àÕÍY`ZS,s+ş€-Ãc+„QrQaFèª}ŠÌ|õ)ÕÚƒ çi'©eÿ¦ï=¼ºüHü™Ò†–É6_JÿuÓåxrïXßÈcQ„G‘›@Hê*EƒZÔÌ‚uû°o`ªò±Ÿ„¨ïXıÚÖ5aÁÌ7P’Âë-fCëxÆÁ½yêâCÂØÙĞUO9tø¬¨eÎ°•8PUl¿/«b'%Û³ª7$œÊ¼;VÄÎâ81]Îó”°m<ìyAÀü#ëĞ)ä6İèíF¹œÓ]DJ¥Ğ®kHMáÛKÔÿ£–üXË´,ıÊe+F…º®gˆûåÈŸs‹oE8uáçºÆIÇa†ğ‚¿[ìÒŒ”»Óø|Áxc6IV›ª¬F‘øuüÌ£K¥|F½Òõ»[â÷…zß²çJëÀÑÛî¤¸Ø3ùA,N–®åp¡	“C¸‘Y{Å¯¿&Î4âf‘5_(óV›ÕÁ£àÆÕøˆb‚)ôCe¸EwÆYB7Ğ‹ª·H%¢÷Ä ïki”ø¹åñVŸ(?îÃ«Ÿö·0±¸q5ìµoHwú•-b$E£‡Aæv\Ò¿ì»æ–ÇñhñD9]Ëm¿Å™<ä!6œå·œ£gëñTĞ¦áUş3E*7;mWÒOÔ3ESÿdÄ%?ÕÅ¥ŸuªºéÉÁ“×A;*lÑùgÓšjÎàÊëù´Âyª0Ã
tYÑ4oÔ»_Í‡x„
æ£Y™gÄ.F—çÑåŒ½J‘´ÕYü€­ëîQXQ¥í`@)`{¢%àéšÌ6`¥q¤sÅ®
L>÷‚ H¸ê^/ãKëø!…xZ”eAU4çO¹VşJ *§ÒÔ=Ú‡ÕDBHø””< PÃŠzÏ›ò£F~ÜmlŒ¤*©³–Š	|’Î;Ÿ†£‘!m!Šº=‹ï 9×ğî˜ºw×@…«}¼¢©Í…,§=uÇ`ãn‚Mƒ´êf 2|©]·ÃdµpqÆæÓçéİP*éHntÑÿ	Å6á¤ñÎ@¤ÿı\à«yÇ‚Ïè”' ÀáŒ€–ĞİîRceŒG-o_yK%ã†ÇÀS…¯C¬Ç‡iÍb7ì–”)‘IB¶ĞÍC ÿråÎÈ1'n¼ù½kí”ñ©cÚ¨ˆ;\…Ş®}D­ªªâÿ§ì`nEó9/ŒÕ¢'åxoáÄ'y÷ıt#›ÔT‚é2Ûa	OßXå{*v,Í>İìÈ™Ën”Í	ÁT³&+ÎòRªëÙÕî˜å×—Äx÷/®Šaz¥2mT“"6€P|]kW¼§ã­œ1ò¼ Ö¬º/Ù(gczÚ´l”?¨Q­‘-ŞÙ
Â£Ûò$·{º>ŒÓ(ú—+¸pC{ {İ)ÿ;ùô×-)ØÅŒU€jä@µñ´ÚåâEÌ0¼ªşÚüA¥ÃR¶k}ñYĞ!°Ïo4wÇ€t6e”&ˆ%‚˜4¸R[Ñ.=O½÷ã)g¯àP\¥Cy)µ
­úµıÛî¸ä dJËãÓ™Ñ²E)ã«;™,òr—âC‡Öå N°ÿõVYeBğ~4ÑçÓ_ÔŸş„=Ş<FÔ˜GD½­>r<4røkr9Õdïú3í=
R%Ïé¿ä•«©¦_‘OĞ;5S‘•Øb}ÊEú©(¡ñ^Ú}úÒ%³ë(ç/%‚d¡ZÇ%I^Ï¤¬™’w^WxˆtfÙ½´®ŠäÀŠaºÃ²ïñğ.3= %_[”£€]ÂÀÏ	'ƒ¡¨8Ñå5t$Ş±ÎiÎh5(_Óî\şö|Ü<À·Ë„¸(˜;•	Û.U›©Zh’J”ò ÚúÜ¼çÒi¤fé‰“ÂT½Å¬ €Å°q0?¾@¤r3ñnoe>²–kóKj»ü8V.6€n)]ß’új>¾F&rƒğædeé;ânFIr\EÜt)^¢T
¯~¯F«'!öòü8HKÒÉÎê÷ïàŸnWnè=íŠ.èˆ;>ÑûØæ™_­q|Úgßáµş`ùbteªWC®?FaÛ˜ÉXÄı/Vñ¢úïc”Hm]8%.#FBm®\=­¿E¢öa±¤pÏ¸QçS1S†öÒru‘Ò‰4“#PĞ7Ş±ì,>8lò½P)ÀPB	|8—ü6|‘Õv¹p3ºeÜè™ğ?µ.·3;xÍ¸ÀÈDZöhİøFia5F–×ærÓÓêhƒáßì˜·Ò»„‹~‰½ş¶/ÒqrvıIˆ·–P@¤K=¡óU®Wô®‰ Õ4¹~PB¸pÃb¡Ø‹^Iüƒ*Ä·:’GN5mú×€ôYê>6„‰Ì4´yàí¬»ßf÷à[XW¸”yU|¸ÿøÎDÀ^Î#›ß×ËB¨W•VÛr§z9œèL)EB­½o F^ê€§nP&Utâ£;{zS|öõ÷ÄÎî~EÍfğŒ‰ƒ2ÌĞ¹†D^™.ˆT(¬dÅê±÷È#ZMí´K‘E4Ë*óÅ{®ıd£Z‰·rÍ5õuxc7qñné~[„”çZ|ûúvg$ÏŞUŒÙÃ&*Ìœß#`Ö‘G0+Xæ£)œà:*:;+Ü@âúŒ…¤øõ©è&JV\ÿ)"6µ“m\æ fË;íeÂ›‚´¯ôÄÄKtåºcŸ‚´;£ 4Ô×C“Ÿš*—%7i…?£¾Bœàç¸6xm<ƒ€IÂ‰rÉh{ke.e€tò•5ÎÍµk_¬‹ı/ß°4±ÇØ¼c÷×c;íCH-–U­·íŒ·ıé…Ã¨×r^qöKÎ‡¶kğı`
›fÔq({ä4ëƒÄªÈ%Ê§däoë39é7ó.¨îán55·‰`cÀÂ´[n©¬uø7dĞ¼W»5ãÌ§\ÛƒÖ,¡ü¬o0a<éü˜¡èŞÏLµì¸pöL	kR®Ó½aúÌä|ñ†—¹á.Y£Õmbb,O†?îŞ¼%moğş)OÍäÃ~Ò¼y·¤Yb@2µ«¢¡}’ŸV”%5~Ê,Ó†]é'Ğ¹1ö”›ô©àËD¼¶SâbHÏ/#MîU8¨1ßö¡_¾¹qq€1³AµW/÷xàáwã_g¾A€«8şò(I†EBëœ²–VúÓtN£#ì¨9ÅÎp~1³d#‘6ª‘ ÊŸ[/ÆAaøÑ}KT“ßé0´•9y „Ëæ/|$<m¦	•ˆcÖ\åMàA-F4ß»28
4‘Â'½ÊéğftQ ËÒœñój|rç`pnÛ§é»b¦ßŠ8¡üæï¾¼l89`ƒq:¯0h\äá–¡DÚ37]Ù|-ŠZá9L‚½›t9¬Œù_kÛi8	Î$¹U$vş¡ÆÜ0s/	·‡dHr`å?c:pPjHÒë¶M›­\YÏ]‰ÂFšÆô z74¹>?ÖíŒN	)šw.¿)ù¸ª¢+w‚ÇÖ!_GŠÒ÷Ù„Ã ø¬ü£³T4Éšm4kìÚú?¶¦p7Ã¦xË‘C§ßŸÊ1- àµqë±á(#
—§8¡·kÜÙB·ù{í[İ²n
r
ô®>w•ô½nÂS>©VUeL;èvV‡ìÓpA2îyì¹G¼Ó.óBXnş£8ûŞk-ãè$Q·&•¬ş 3Õ4Óîåcq¼àÿ$m8èƒ‘R6$’‰Òè­šG(x[†k3À8½­RÓ*±iÄ¯k¤´WCĞìöàÒk~¬ëh&¼´0Ëõ’áA›Áï	tp:H{ißüöÆ-ë
{|IÊ<öö÷”Æsy6•Ç]'½·Ğú êı5ÌË4vFà	‘Ã½Í7š)d[ò;û+õVniSÎ#(¶uw”ÿ’Põ‚r*Ø±Û# c'LÌ0T†¾‚=ÉşkÊµÚß€"^:o"²²Ú]?_|h m?´ÏåˆuLÀo ~½Ó‘IS!p­M#­]kÁ8ÔiN}Ç ‘\PT_VÚ¼è~aÙ“v!?ÊŒöjÎàÂórÒ`¼<%x%.”[z{zÈk«»<G…Kä¡ğı!'Éa;y /Bk°4$_avÚ¿døıvAI;ª±ø=[ï[ºôÁ%ê´¹f¸9Ş'[Âà4¸ÄÊF+/©êA‹ãj@Ì^2Vr$°HŸ…üãñ½ÿˆCòÖP(2X ;óÚ¤±“î¨óê‹b9ncØ>áyi1å’–·AÎ=A &è¨ÏâÏÜb¹8÷M}xG€2yú{³İdX«À)ìMn2uãæX8Ô3Wqê`…«Ü"‹ÉõAxâÚ[µÌÏÒçzÖµ4<ÇŸ¢_¾ıÄ.¥İÄù1Â(ù1s÷9ÛW™›eâÅ#n.|có^ÆÍgÉÒµ	ëVñ¨ka7TDEk~±¾ale¡+us0ÿıòäàı¶ìD<(¥*ÓZŒˆY_™óÅğú`=ŠşÑÙ‰TË‹ƒÄõ°p+ ÉÀo}úâÂle•œ«“š!ÈRÕ›N—Û½Æ“¦ejA)DŸ0èˆoöËˆÀ*/ËµdÁÛf¸}`ÿ¿6è÷®ãCšóTËMÍIÔ'>îvg?·v°DQ‰îĞìÖ‚N•—dkçS	Â¼›ç¯<cÿì`‘dÕt’…ìË2ÔÉM®>vµ›¤”î¸$ós[MŒdãŒhÒ
¦/ñJ‘)øØ‹9R/Á,Œ¯ÓM,E~¹4A¯«µÜ*\š¥¿¨v _.z]"WG5£CŠóíQ5ÏS;–-ÀªÛ›
úSµ=óÖË µ¶÷Ó:ô–>è8ôJ¤â6œIÍ•Åb‡7<_$"²|Ô©"2îc^?ÊÏÎú×âÏådæ>$Óæ`}‚3	J"¬¥¥ÈƒG‰ñ¢#õ«GHÛğ*‹ïÉ½ákÛıÛÒÎãYØ€Ÿ«P:Z[óP÷î»m¾GÇ´$èÌba³kÚµzˆ‚…êä´ãákz •£il¹A~øºGU²ülùóÛ‰Tâjür¤)•D(ñ«1nsí\¤bAjLŸ`'EWƒn—¹”|~pğA<ÛÉ†Ú÷Ú ò†ËˆØÆû2YÎÚ¨¾áªHKlœ6OÈV}1†¶ë2—ãmqN±yø‹+¢ºânÆÎ—Ü)ßs2•¾ ûF=ì§«×Òµ|Ô:$KõÊË*$v¹zÈMŠ<¯ö=øñû€r@‰b€ ]ø¹¸?Nç,¼=v¿íÒ'¹Q"Ç¸)<Ûû°Á!n9Öî%¢uäÍ ƒôäL¶"ksb^Ÿï4Ù×K¤Ír);(âéaCkF öÑuêdê=¾;Šs”½Cwä¹œ|sq|mIØ£×ÍP•[¤æÊY¿­{^:¦ª]¼Z•>İêÚÓB
Ô×M_,ibÇr£ñ—-yò:”Ö¹İ™€;i‘”€Ú6$9¸&Ûc`sÚK…^Óqä}‚Áß:øÌÎmÏtĞäààê,;®HYşbhmiñİQVxLùné,(øŒ‚|¤ÌŠ»äp•ö?„04‚}ê¦¨¶ÖïªŞùÙöíT-P?·Q:•Ö7–Wïã·GŞÕ÷ã ˜Ÿ2İ}1pÓ2%B˜È=~&½5 /jñyHÔ¤^­”g´îíÍèÿ¸ìàï ş¦”L¨™al¤¾ŞÈdµàÎå¿Az/étDÓš²É“éJÒmûË0-.P€WM€X aµVÄp´r´PíÆ*ËÿĞÀ©*C"­
8db€D/H5Ó¤²iõú_»ødCß‰¨-mn¼_©jØƒ£ÿ§\TÑI‹¨ªFAñÉqşC1qØV¿(>
*rùó·q)OÑ]ÇœÜÑxÉÑÑ¡C¾ğ¹ñ½ù3Ù
©êEqÇÒuBÌÖÄ|ù<g§âHJßüŸ1=¿ql×f(İª€ïI,ï&àşjàÈÎĞdòª2^_LË	ÔqtŠ	d>B
µ¾~<æwsûÿ9êZıqb\9Ûyã„}€x#‘!ºï©êD5ò¤
oÑúë+W\~V£8¿¢ì›ÕUO¼|%W`MŒäÂ89iìL5:·¸ıŒ1j>lŞˆêËÇı.^®t8›X ¾â›otĞ\Ä®ğ%v¦«í©L|éqò<<– ª7y3à)å•Ûë0ô	öµé°Ü5Zm	–›ã[LOd±H$±ŞQehŸê*4¯&-¤ÔtjğEsozt[v~¸ÿÑ¯Hß)7Rxúƒ„Ióëz’Gg.PNÂ7Ôùƒ
`
o*’;&t˜	$¿¾¼dÅåÚRd|_8Ó™4ĞKıF¼…ºOÇıåt‰k«ˆgùc–-Røœ˜˜†6)Ãû†=ŒĞª‚~n;¥)„w´í_q‘sÿ ÍKf—GÓaˆ€Î—LSçÈŒíÔáJnnVtÎ¨:-”dOø“G×B‡Ö¬DWªJóWj‘E	úÉ¾eëĞ‡dWÑéÈD=­uebEƒ4®¡–ĞgÍñÎkŞœŠÊ³}¦p´k‹-"»×Èô«SîİùÒ3FŸƒc½w³DâT„x½›Ë-%—Ík‰{n7ü±ı/'ETºkBra¯Ù ŠÖl¾ÊŞ¢Ô€íLƒ*èv¦úèŸûT8RØqó”p´]û^Ïª³¹ğ”Õ‚W×We@Í•qbÔb*84šQ£ Í@iyÍ™v¼õoöÅ0d2ğêˆ;f5»¼‡UÚ™îs°şyyõ†^DCÌ’_a`¯•­"ËQ¬§VuúÎ†8åO ÿgºí HÄõ$Õ§'ßö³ËrhÏW¬üâÆ¦l¦şZ#G=ğÃÙ1%'ªHFskH:Å.¾I—kâ–Úğ•\Ş£­]İØİ]lˆ³•-YU”2ó6›şgZ(Ãâx¿*Xt>ƒøXÎ„>¥rß‹³ZØ•=º—2ó#Rzx;¾„ŒE5v„òH‰»ºe§©^y/Eé¿ç­:ì7g«Ø¥)³q°^Ş¯£ÅH”Ë¬-¦)îÔ‹W%Ôb¡Oxß¿ñ”Ñô¡ÅÎ/O§@nî­Ë‡²ß Ú•š§\9Û8û?§€ŸãyøóC¸b·£ªÄpµŸ^³üÀ5ß.³µ«ÕâiY½Î|Ö	uá¿r••Â¬´'ö‹Ì¿ß’"TlÓV¼İ¡×&>~0û‘mZ¦ş˜-JÒÔå£X­Ã©³’Ö#æŒ§¬[N·9ºÊÅÃ°»YÚååUJ™n|B^œvZ{—Ü¸ªâÆ‘gõœ…±½²¼¨`öµIÅHvÆ×fÇÃÊ$0÷â”ı†ßu·ë²Ô3˜Á¢©[èªk3Nú‰mébz@œêg`×H¬°ZGßÒÚ³‘Ùüpšû>SWo¥¢ƒèœæªÂ©…d†œ03˜C½ñü?Á²†,o¨7”—ìLE›yA‘ôO ŒlS+åÔ—ÂGœæ©;)‚º”Í¥	UnLêwÆ"9”AÖ/nÌ=/<ÍjPà2Åe×ªºYÿ'·jï=îUälÿ”P›º|øõ­2÷b[
p|l/pÎ”ˆøKiñÖâ$ßéE×ğ#Â·F)Ä1ƒÆßrû·¾V¾’ßØÖ¾ót>7¡c¶·—]>hÀÊÿÀÄ•nÁšÓú.lïG*ÌcyHIW9Y‡á„ù+(òğğ$Ä
àÅ$d_ ±gœ ‡€0i‡¦<vóéoQf…‘³`¿JÍ™×ä …C‘2 1ÇZUO®]ß£bË9zyšj)& |3S]üšîŸÌ€\1 ;¥)E@Üç)´7Á‘×d®+´äE°Á„ï= ‹éAÁÍgp`í¼ãöWeƒç8.)DMo¡{Ì˜­^ó"½¸:’Ï:r«ÓÏ³ib¢a{d-uÒ@Ñ¤]A˜š“Éù°EHæÚ£İ&İ×s«ÁÕ™9M˜#‰]İ?šà—SÒ Wÿ>;2Ò±Tåüğ˜œJ&?ày%yÀ¶¨@E¢M<åWıe
„mEÛj!É¿ªÍ#opå'µh;E·„‡nrCòë¥aTQÌ(z‰Ù¿ÔR°z/åt-" ¤JLIcK~‘Çh÷aQ¶ÂœiâHmÎ»n úÊp%IHQª£—~@òzÛ½,ßF™ÏxË´sˆ‚ƒ‘¶‰ı©<´]é1¹ Õ	ĞÆ;ªK2ŸšN…½”…˜óùh™ˆÚoµn—¶6£êEH’.'‰î)¹œ£X”Y†¬Íg½†vjã³¿3§Ğh‹!Ó)l©,OQvİN:­Jxœ9e3~Ÿ›Rq¤§_n †cLÄÄÒ«mÂ}"?•¯DÏIAJ“äÿşš[ıÁ,¦[îÁïNşûgÁjAWÕÇ~õ@ÿaÏÙ6J#Må˜ÖïvØ!¿‰‘‰ht»òtİ íGì¼LŞ!…nÂ®Âú¦òèİ;å´O["[·½#âĞßÅ’ ò•Xàt×à/€"Î”Cô¤{:{*ˆÆ{ÚÑ>Ğ@"Ú’ŞÇ7WùÖejàWI"™æqñJŞµ¹§‡¸Î9…¹0c°!®:“ıàÙ¢ÓdŒÅFn7É”ÀFL•¬WÕ³o8ûÜä_¿Nÿ[g`·ã æt2‹ñIQwGª.Ï?åÓo•„~Á•¾ã…¿À+Q/)$9òĞ-?k;+(um1w¬¹Evc†!ß¼ãëú‰…bCÜUï®,(ÑmR3-œ98ÕJScC(æåëÙkÜJZ¸|ò8Ã(b´Ë©G'İ[F±ˆ˜Ü2#ŠÔEä`t\²4\¢òxÙzÜ•CÌ¥ÒïğĞÌ»ı´ÚÎĞèâ'’ÀTèÚØ•	ízX®Q¬•å¹£‰%¨(Ã»QËs¾5Üi û på$T†İ?}ç5¯œ¬ 	9‹Éï?ç…©w.’¨n ï#{åQœà/÷ p*õıHu2Z®ıã†¦LlŠfé¿¹Œ,„ÄdÔ½²‡¼™ÕÊó%~6xg°£ôºèÄ~~cqÆÕO.èğL‹÷¾hÀÎí´ËËc@”ÜÑ3&ôÓ…Î#O‚_{ß‰à«ßºÍß¥{ÈëTgÕo¹ú9…¨úVÇ:ãyp]îÄ7«@_T¾TÙñrE¼ş½#‹¨/Êb¦ÕÓaî¨L¹Ø?œàÿœšPŒäF¾XÇÀcYpk”ÏÛÈ‚jùUç~˜GD>“€JbÖ¡™”OÈìÆT@ EM;ı%‡¿*¦à<àù'eM§G¥x-Öi>s¡³ŞZJ«Ÿüî'vÎƒ©´ÚT²Š=ÑËŸŸˆUòÅÃØ&Â_GÙ­®E2%ä³á¾òHë‰K¤Ë3aó›:ôıÎšy5úÑAé `£>:R¯dJ.¼IŠË¸VÉ»`¨Ãˆoˆ¹KøšãDÙ
|Õ£—´¡LŠH½ÑÄ}‹ö´»ËÁÛ j¢¯¬CğkŸó-é·*’ñ"Ê”„ŞmÓåÂ†8yP ‰€³ì!İÙ¼öC>Êé1U| (e)nVbÉWhL~EôôÉZ_x[1ö:bmmGÜt/ÁÜÛµ.$Ÿ˜ÖœŸÜ“’k¢ËãÔ±£#È€s§• Ï¢u‘|níûyqôù‚|•Ëœ}St8‘>Ö$¢2çïW'
ğèÏ>§&Ò]“95†Z^2¾%múïbœuWğ&¥MP9€PWDè„÷‰7Züà¿³ËÈ7VÄÕ0s´Rd‹Ò*"z´æÿXuÉşs¤£.^õ¢‡zd%-)ãä8I•4**4DDs±nY~ÜÙF£&`:8X)¼³ÈšZÅSè%Oeê‡	äµFÆœj&ÏËÛû•M_{8¨nz°pXPv8Aˆ
Uà^ùv¬jıœ/µ7|›Ìë8c).VàŠœ‚Šëy,š½´é¥loD‹wnTRÑGª 9ÅAhô“´şÕEß ÌŒä#³Itß[Ø—“«TÏ#A:7oIå‡êÃûä>­.Lš$6(ñËÛÂhÊµ¿Á57«„B’w}Ë¡J»‹<ÃÇ;1’#·HE³lĞà«3[‚Ã!É‡¿p1Vìİêa6KşÁ+R(¬OLÊyâ%P,ÔDz¿J7¡Ù¿–Y"¿®°Ø¥Ÿ¢fzÁdÜ>»'ÊÍñnƒ¿½;ÜÚw½9±B&'×À¯fyÏHQã1ÛQš^:—¹O]Õ´\Ã¥Òû+"ªk¢QVbÓëø—W¦ğ£–NRÕŞB¼èT™»kIé¼wáW<½‹Äd~q2Î…7ëñäîHı2§Ä(E‘Šy¯ædvPTö×:CQ`± ñMöşKTW	Ø#õ°L¿ cëÜÀ/[ä©Ÿÿö—[= 2 ×YLµäÄï«ï0/$:¾^™j4&Ei•çÛ]…ã6USñ(cÜ3òÚ¿ÕE?J>OEj’Ä„è"”èÎ$]c¿xÏ¢EÀxÑOèvÙª#ß÷YàTî¬ÿÎ˜ÂK]C\«¨P+rq¯¬.Å]ûoÈ2‹ë˜Dƒ˜£D ˜ÎİÚ .šîN@#­í§Mu‘vY‚!³†t&õäC¤×ÛË	N¾ù„ŞÙµÒ*¢IÈ#\—Í-…°YÔ¨ãñÒ0K€Î[ÿ#A’İÚµbàˆõèK•‰ÃK7F‘]Ç–(ÚR0Lñ‚•&{÷S/o\¡²H¹ñ“š’Ov‹ÄËñıÑw9İƒ9;
«×—§Ròµ§	Ãn õ»næ8¹ø^rãõ»\Fjôj•eÈÑ|8”ñá¤ÀnÏ28Ü£Ä¶È°Órğ3f%/€7"µ;]\·oó™6¾ €1ÛN#–Š‘ÒPVú„şİÉÖá4eñèšIÖÛßúÛt4;`¥¤S%Á oñc9«‘|›/çt"<½ñ{7q§[`ıXğq grD ‹zŸ,ŠUß&6u$ùÒz]âœ:gZì‚ş&ÃÙ¤Ñüéh40wöpòúIó3 Œ½úFc BÃä5ÅœxE
fËf¤gku“›>ĞŸY}~Ğ©`œğV?n\‡ï­âWN¦Fª" 6ø…^ïÚ /t”Ôø›S ğIÊTıä4”ºNŞ8äp#’?>_ZÍce®r¢ØÁkkÇ-6ì_lK>Ø¥o•óËIÎkgp›åô™óŸ:»S•ËÙ]JcE/+õ6•¢Àsao¸µ(qØğïG˜tÜ¢íM‚İü[!³S×g±ŒòÛväXêÿùéJØ!äšá–‘Š0C¿Ë+Ò¡ê¬ªƒ9ÌáDV¡
8 Ÿ-á–[â¼™\Ø›ïc
-Zæ{ì?C{»LëBòsé¦'EqCK*®\„ÉÎxÊöË^~—Gp†LaÙé™J;RqsiFÃ‹›í³FéÄÈ¯ÏtçØIëï1¶®ì¬LçÊÇ+3X²mê'3|¸«î†Ø}å|£KèêÊÌLHsøáÿó{¹O5««™Ê‘{Ñ>iXDÌ3ñ}'í–Yb¥×ºîİºa'ûã*<¨İ “ò¯ Ö).^‚šÈöÂÃ}‚7WIN‹Âí
HNáÙ¿%)LMçg°»§áæ1bß¾šØŠ&‡ßÏ;äûaa×Ö‘«IoŒyù§¼¤1u Ç8á¹t™Y‹z‘èƒîyÄRuĞâÜ
‹%êÑ#agùº™F%ÚØêhÎsOŸ|İ
»‚Iç‘¯À.*ëÏ­4½„y·éê«N>à>ğKN Ş
t4h„îÙØ-—4l¿}QTíS	(­»_}®yÊG7ÍÓ£QÆ±È28’ãXÏÑ%.˜êd_%Íä¯qgV¤ìûhb~ÍĞ Ğy2®nD<^t£¢ÍÈf`ÖíÉoÑú$Ê¹rôøi
¶l¹½€€Í0”¯rZUN¿Î³9È·î ‘Så¡åjÂ±_İø¿ôlœšÒ—–Ì§¹©ltÆÈóâÛzÒXÂœÄnÜÂïuÍq ÂÒÒÛıW’©×Ú±GŠÕÅD‰l ô»äç[ÎZÅ6r·%2-ŞìÁû„}­’6¹·"1‹º¶póğ¿¹Ÿg¥tİç_İ\>*ŠãÚw¥lçõ¢w@u[L>Şï±àëø¤oyzíÛb+§ñTõÿì"ƒ¤Ñûqí ._¼bSŠ£!ª,-ãŞ„%®èÃ_Fœm*éÀ(åìÅçıöÃ”¼JÒº|`m¨”	I ú71l´ı¿`wä‹@+XKÀMôÙîN©MF´šÎ“ºVJbˆ=­…ïÍrR;Ğï5Í]_%µ"ŞÇ@ß¡ÍÁT5Šÿ©g÷¾Ğ›¶ÕˆÏˆjzP–ß€¬W‡ˆü\ÇKùâ¥û•EfAÙ(Ü$€õÑjßÕ : 4,J…€Ë]=Îiuoÿs
Ğ‹[Û]“´¡Tı¤‘XwV¯HK¿4Bäkï/Âİvã7 ,kî€™`ZWl\”Df/ö’–³Ó…ø|ôHõQVfùz
+ãü ¶9à…¡™¤}gã#ú¸­êğ¼'°¹©OW‘"É¼pzıêi&‘8ñ_]¯ÿüêÂWR@¤üt’Û0ÆõŒ¥¡çn„¦hO4~!0ÏyızÜ?BµdtÌz ã½cr_]=ù°å	Ìôè£vÉnSşĞõÂ§ĞÜZ L]E‰É3Fˆw(¥•Ç8A®ëE‚­¸8û„¶Iapp|Àj¼wT¦µjÅîè2/ˆ²«û¾ÉQTÙãVêîUv§³q]'v~M¼S˜õE(ÄÜ¼	Ïıƒş]á!†  “M]¼âa…"khËŞ,w• §¦1¨c gvâ/8Ù9<gKx°.ÖfAUkÌ'‡õ@–¨(·ÈòFÄE=Àéw•xbK²å`HõñÊ§üóxïÅ'£¡9£ë¥Û¿…Õ6«ºL¬ÅIf2˜ßAŒoç§7gN¹\¡mŠ •N»Ÿ*R±KjxéíQèª}¼\©â l¶»)Ë“UZwCŠƒ{ëPì›·ø†#qä°ZÓ•CÇ^¤g¡¤)­SLøÓà1ª"2íË¤Ü˜ï#RDÍX7³Üå(ÍÔX.ÔÒ²Ï0ñ
™{LšÄf¹#‹AåÓg½û2MI‡5ÿ¬ÛµËìv†tÚ^Ë¢ksæQ]ßáX9»………å‚Šòş#g>ÉVëö$¨õåŞŞöğÑÌLŒÅğ ù¾ú¤¯š*éJ&F±—¢mZºàõAúmŠö‚nòÙ†Ÿ2}Ëå6Ê‡õ#ç˜pµÉĞ™ĞB9ø2}×•ì”­~ÅîJhÉn}Ş4}œØmÉy*¼¤mÅÓûä”}xğmˆÈ€®¦ü\/øˆ­ç1@y²±Î5i7t›õGì²	°s/™!¯6oõ	 ¸c¼íÙHyËğƒ0-1ÕgJ8wÃ²‚Ùë®9fA½}2—¡BŞm‹×q™TÀáwtöË%3–‹«ì…À&vKs“ü–u]çÍxX^ÛÙíàõò²(ÑØ”I4™47«¢Ïàê{±E Ö°9“qÁLVXÌ^‡hı°ÂJ€¡½.6Bo¨TEœš‰L8ÌìU±Úu@p4M7èÂ×²Z½Y¶zläF€N9=œUÀ‚ùC»Ì»p3õİÀÏù$@x£pÊ”ñŸTYµÌz[Éøã5641…bi•Y9K™/!œQ¨P.7(qnlµD¾«>ó+³ïÄ½tæè}õ¤ğkú;àYy9Ì÷#$ïóÖËÁ£©X|-ú-]\`&Ê§Qr<—7j<BÙãì¨ÑTMv©ëzJ\ “f<d‚\†å«ïş bÿElmwƒër
³³àg¤‚OˆyĞU/Hš\
úJ	É#¬)Å€šÇa¢îàbŸ=9¦ÀøÚÑ‘G³i%ş§B6ß^øÇYuIw-Éè-Ãë7[R®§PÉĞĞÅOmÚ­,ĞŸ°ò—ÊÔÁÑQÁŞyoÛs¡ÇØftûR1|Ô™MCg8ÖY“›ò7ªk²lÒF‹¯Q,0-÷“t_AOtíxuÍ¸iIUàêXwpÀĞyİ2ÏÓ»ÛâdTo¤úÀÖùÛCïa’ó)J‹{\¨<o¢áV
6­ş…™¸uÍÃ—mØY5Œ<Ì´s2v[bïdü¬9"‘ytÿ=Ù5Ì¡‰ÔŒô=’à|vpövûíÊ·“Ø—¶˜ŠwP(<½tÚ›»¢EÌ(S(Ş:Z$kí`›q˜msòt`ĞC‘êy:¥$èğô˜¨æ>à%|­÷—°¯Ëí¿÷¤æ–Ç¿)&¼Ù»:U [•àë¿H~`Hæ¡;
lï]¡	"_KŸ2#›	Añá…oŸ<ŠèòlG#â}>eUÔoãU¼äÒÜ0Áôª¿K²¬ÿ EÁS;ÏİŞ1¨PÒã²z–‚B“ÌE8’ªç®öÛÏ¬½^ÇO'SéMºinQ²CxIºDÙ³ıv"¸§SÔ&3
4,r%"Y_¥kë²pÇ•Ÿ+–‰ÊXs° ëƒií_[ZÇëÒ8iİ=]s…»Yy¬±³k4^Z`Ç`ÒÄKÂØùÄüY–ÙŒ,ÇÙ'vÉ3¥:m|ĞRnüè

™«öaçIı
ì6jQ(#)œÏáT\	{±}ÒÑ°‰ææı¸?~†³k„LÖÆIi@iaqÛ? M—£VN-"©ü„\Ú“ò<BYål²C p¦ŠN7°Mœ«åXä÷Çl™€üOïMÓEç¤.Î-Ft©î‡Ü%6m‚¦•×Î€wX:.kÄ¯¯JçNÙoŒØhË08èñÛ$‹z^ ¼îQ]¿‘Ğwíæ”ßÍbr›~JŠ=mFù0Ÿ³L@*S˜Ï- k-ZãŠ.Èö¢Ô­qŒe^£ëxŠ†,^ãB•eÜöp¨«x0µû¬öŞWaé$ŸX`ğšçğÂÎÀf<@6WÂ§B¬›q$õÑ	¢ø€-¥Ñ³“›&"5WE³»¥¨s j~Ä„°_;î5Õdº°ÓIç‡9Yì”
ÒPÍ8×]"´R$*oÆ'mñöH`ÏÕ/„‘ÚÇà?ĞXŒSÒ~ùpKÒ´t. ğ?Ì&ğÖßmvİx.<y¦Íá®“Vİ´Bm"e2®1@vÅáùĞ4HbÂˆwtÃÄ[şÜÊLÃ¶UO89.fcKSeáw´¬Ÿ°ua‘vM¸Ëq»ÔùÕ<
@<ò¨#·íB«y¨!1Ğ<¡A¬2ò‘BõùËöD\$N¸†Qä†©<ÎBú2 röÿ.ë«$½BÔöó´ùñEñØiÄ‡è‰Fn$Ğ/­Ú£zAÖ+&Å¡Ckíz;×sm¿²#«QÊç-ßw	F\&~Ä}$-}¶ô¸
ô–ÑAéòR7O+hUåŞWÎÖš>{–…ë‹5Á[hvúHë½OËÓ×ê‹ÿY‹xªÅ9Sôğ¦Æ¨)Åœqh«O5 ²m¿z2¦ Â§ÜPïß£ç­¸„ñ³'–¡Ü!$ôŠm½|gâIUKZŒÈüJ®×ÛæÌQ¥Û†4 È@MN: “Gøş¥]„w\¾È¨Í%¡)”:g÷mLCÿauL§ÃK<)0›iÔoÿ{|Œ«oÓÚj-3i-¥Küu¯¢åò•¨ÈÙ&l´É+ø„%ıõ¼šr•„¹\Ë”=ï¼#Eõ!ºo8²{ÆèÀ]°‰w‡©®9¸àí÷«~†Ö¯4ÿÇ=Ó“lJ× ¿¾Ïõ8x®1ûL‚•œ•¾eÔ)îô­­””#>‡ ÎSVH o Œÿp˜+ßÕúğ¸cåLáµt¾_˜!‚vş½v
ü+*l]MÃ8.AûÇßŞ•[223[Àôró3ÃŒí/|‡ˆ<llaPß‚ß¢©QXZß$ÿ”4¹†¾ì«©è½şÀ¡°Ãßø¡k—¹V«¬p-?«g´ MAxÛ?À=F¯> b‘£ÑŠûÙËÒ#='o^ğÕ÷÷¶væg*YÜÊ€'Ä€ÛøO²!\álË6ø¿B$0ÊJ‰Xöß	ï;~¤ü~Ç­^Sóÿ—sÔa‚Æ(›.­à…qB,ÀQ©ºê5OD;£T„À6£Z;“±ÅO"š"Ã[ø™¸0±S·áˆµ¯]YŒ1°ıå5(ºêEÚ7¬ëIÚ ğù'’É>ø}.R\†Yğˆ¾nöæ·[d÷ï˜FÊö–áÚı602+tç÷è#¡¯I¼;ô¸[ú©`pÃíqfôì!k<Ø(„Ï¯%/~:Dù63¢¹	ÜÊ¨ë¬Ê6oq˜d7¹Ó|_mƒØ~ˆIkL±rŒÑ³gB3¨ìoA^¸ìö $êµ?±R•šÏ0³fZ.x`mflóåÕ@èÜ.Y=\-—Üv6íî¡gËÀûe
í¹¾N}RßZà¶7ëj¤ w¾Š>Ë\5Å½Rİğh˜ÂÜËKXeÔéı¹Hü¼|4ùú²;"îBI™Eè›ò–ŠÏ×FG½dØİwkHtV¤SNİ;6Ë<ÇÑ3czCªàJ$ÿu fL:–|Z §°#ùW^¢ØÙíy;V™Dfé<ô7½¿»lxå×l$(Ğ¼Oês‡‹Ëavb^Ò1gr°IlÑãŞ¤ì#Õ•ä0ÃD*–~¹Ş6„;UZZyÊÜMß?4À.Hça"gƒ/O@øãMhD€®-	ìkŠdme]L‚7j'5Åsİ}—Ş_Ô­ á ËPåŠn€Û7üdw´>£ôü%+@×Hæ·ğpQÊÉ5Cª]ëâvÏo„¿+<CÅü<FBB	?c½©‡9%aË÷7˜"•Xı#t²$b*¶VxUsc±Y´HÑ=úÅòYS84Ò$İH’Â;h÷‰“.µLWè5¼F/aTª¥Ë¾5ßæ°µ-Á©±Üß\OtDÓğ˜àk.&Ò±wŒ<ÌĞ…§êû¢Ä‰æëÃlƒºeĞôYÙŠ@È.áÓ… úúè†4›b+K/§Ú‰T ô5eÁ3¿GM@bı¶k}ÎAt.öP´>¢‚Ï¿İã™œ+1?÷!ÜlPz²w×8ÀVt“…Àa—û¼jß¾hß¢¢’ìÑûò¯Ó47®oùc:Ziü±©²Ù·Ñp	¥ZÒÅ©"€|Y}Ãt¤Ï1ÂA@êğ!©RÕ˜1 CV}•XÒ™ˆ!f’şËE@^—,K$BÉ¬{åõw”Ã^Etbr'ÁêïsG5Œï¢ (]2[4¨‘ú\»‚Fë
Ö»`®é^’ÿß„Ê½§h³€ú¹»%ø(ÍvO¼ğ·ÑV²ñfl¨ ?¿õØ¨û4úìŞ*á2Û:ƒ5ÁúïM±Ÿ9øx!L(	H7lÃuİâñ3WrJX”±'³XKo	Å“	9¢JËLúôTLŠ˜[‹Ö>‚şscËíÁÓ¡€qÊøUC$±á$M¿‡ÕwnÏdWu‡¬À´Ç°sq‡….c¼Å”æ¶¥µ’?ÆÓÓÎD·{Ay_â>±Bš÷Îˆ¿{Å~É2»™èŸZî¦1sô-yÁq¸LÃöÇ]W\ÔÜcŸäh
*ÅCÔ¢İWş%µª¹“,q–#6ˆoô{–UXXıéV_tSÙµ`‹1sÃĞÑï-Õ*Áí®¬^Í
·òÓÿ ûÃ³ykg“ìè76øà±İ“ó¾Äƒ]„ç9—X¾Ì®ÈY$š.*Øï_Š¹[®à-™EY3ş"kÕØıÊ°ÿµ„Êİ€†:›çÅğ™ò4ƒ¡Áñ´ujSÒ’Îó Ì4°±¢=·„Ì¬MNxÛ'ì