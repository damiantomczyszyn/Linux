
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ��Y ��e�6�5Đ�����]F�{r��"Ϊf�4&J��	���^�5��wC���t�+�QiӤ{АK��ՙ�`��ų�p����a.A^���)�&�n�pʻ��$VA�x�0�س	�,gп���di�w2lGH�<^$N�'���S` 9��6�����Uu�ʉ���u��|j�Wq���R��"�33^����M�������c8�ή6�T7�\Kf����82M2y3i���wȣ�\�z 맟[,v��d�3�XG���ޛ9�q�R�H ��<�D�����	%u�۶$��r�#���i�
,�$�S�첻�H_�����G���Z��ߗ��6Uw.��n����`2؏v��(�ɓkІp�#���HbH+��@�R6J>֛�^E����]!B�XS�l����|�,����V?��:R�����Wp=�{!�=ԓtw�H@g�n;��Pݬ	S�>��_����������#��1�h���j��ý�nꬨ�
��5�� �GUle�(pߔ�W��A����ʨ���cr���'H�-i�^Z�tD��;��s�]�f�Q���4|d�y���fmdWu��Bs�^@���E׽���Y�[m���_�ýi#@I���9]����D�:8��/T�W
�Ӟ�>;rِ�c��
�����������`Ϫ�!��eԥ�nSLR덤fU�F�9y���}iR�(G���N`�����8����q��6a��j�4�MK��8���?e�4�M�?E�S��qz����:�io�c���o�����z/��r7��y<_$D6�6L����3����\�� ~��7�a���F�Χ���4M��,L���Qj�"���CFs1XV�o�ɸ��-�*����՗��a��8[��������G�r�?{��w���D�a�6H:K��Zd+�;��q��t���]��*u��J�卭��]�����5Y
�U�'ϊ�o�eB9�Z2>�~�~y�u)=�=�zϕ�?'�8����#�'ܽv0�l#?}h���ЋzYɩ*�q��C+2��
5'����=�i8{�������b�dh&s��g��q��D����:�,շ%5�ApѠP͕�B�M���ft����yw��~�A5�˝�N5��d{ml����jhk]��b�÷k�~�6�y�z�i�/�<�ث�\���w��luVڗ��)����$ˎ�T>ߡ�Q��J���I5���d�,��� d�挽�A�M��f7%FbRPN�捏$��>Xt�~z/��ɏ��wU����J�k�O�S(J	��KZB�ЭX�
cʀ�5�V9Y`}���j�O�I��?Y�T��\)���p�*hc{�����j�m"�xv����ΘVU��b��T�L+0S^��.��C>�'d'��"f=\jؙ,�¢�?2��"nΞ�'	`}pY�h�6����,.���O�U��Ek(I��CY�=B�='o#&7�"�x\׸�<�;�N����l<o6���[ג-J�˲����K�y$?]4�贞j�Qrk�����м��������7�	��.~�M&�nbF�LR9���I�_%9�Fn��%�ŝ�����R�����v(n��ʌ� ��Bz�$l�Z��1R�v�㐚�ͧ��M���"�ZB���'�fB�[���r���v�Ϋ�|���H���f�����f�K�5b�]�4�P�Cs9B4z����� ��T�,�|����8�y��sP����iႌR}�p��
S?�⺳��2�&=Cp�F蕧US@��{ʭ�<2�w�}h}�� |��`���o����=�h����c@v���o��<��Z�Y�=���rS$��8鉑 �g��~����������O��	�W	�%kP�ɖ`���GX���ygbH|���T[߭e+g�4�b2��!-�b=�>�~w�	�A�z�����:ݬ�+�@�^4	��$4��T��i�����eq��c� jͳ�#)��cҚm၅�sGR�n!1�m�!���<*N��b?��ſi.L4R�'O��r0FG�=�!`Az&�kI���5�A�^���T�f�� Z��HG$LJ�x𖰎	
'�3Qv���z_i�,Be\�O5��4bv�|(�2�j��`��G�� �~=��I+sF3B��6�lS�ņ�ZI ٸ`�@E�ɪ�7�������9Z-c���8h|�	�i���H�;��~��{�c�;#��� .bޫ�*|nR(oTTR�[+\5I��@��s�q�l����\��Fx{5#�f$j_&a-Fд��?��*Q��:l.�p��Q�m���nc��wS�T�$�푐����_�Y���K�Ҿ�A9��>�!�;���J/k �Wچs�S؞�yco�T� z���*�g�m,��C�μ;����ʗ�\��҆z�E��x�.R-��	-#�Fr�A��u��n��J A���W�A1���ʴ�ǧ���@����dZ�̫O��Xz�����|�h�-�Z�8u������TH��,oβ��>'wđ�����m]����4�����F}�5���Cs$^^	2W�������rg��2!���`|��V��'��;�9���C)�^h�}L�o�:�����D�l�g���8} ]�\9�^Y8����j�s��Ӂ&��8�>}w&G6�V)R��\��Q���3��\PI�F��h����H_��������ͫ�K]I$pE�D�+����і]H)"/E��ty�Z#yk+⚏�=��j;�U��en�:	�3Ϫ}��c�?��]��t��j�u{hݢ�F&
 ��0찻4_�����F���]p��!���f�\�W�b|)�e�BN �f���h+@G5�)OF ��Ľ"���O�mљz����8Ze��^rz�bX����E@�s��naZC�p�rG��EhQ�\1,\
xk�^5D�zc:��H�+
4�:�;k������ Z?�n�>��4���MI�o��-ztj/SI�E�ޯe�<u�!���3r�Jǉ�ZU�����T: ��nMҙ��&�T�5c�F�����d�rC~��!_b����Jf��@�q[  ���-�+��7�8Jk	��[\e,����K� �$��1�y}J;6��%FB4����Z��>;���[4�ap"�LOV����5 Ɲ�p7�[ﳞ��?�V���ݹ��
nԚ��"�/�N�5��T��XZ�/����"F���|ҽ3�.͉�
��ɬ�#�U�U��0���Jv`�ҭ���m1���މ�#Ⱥ9�݆I67��t>XP{Ҝd
���EKO�A�13j&t�X
x��;��8�Y�'�|�r�?�%L.ج�$F0��F]�$!��0gï��=�0�9���i!`4u�x��w }+��~OKWԔ��/���BwA����+���A�$l�����g�+&"��fE�	!�vS���H&�Z�@p>�G3�{v���:7T�N�2���3;
N �~~���E+/ �v�$<��#u/Y��8���G���k]�K�^A�JE��)�5��ظ��J�i�4�[�S ���h�}!��"RD�6~�_�K����E��gВ��P�h5��ۈ���U4�S@��ߜ
n�/eKFZ��$�]y�>[��[�|Y�{n0(���B|5;B�_��T��$�Zn����-�"!D�f�)+:] ��T�����jkf�7�ؕTlgE���`�������Y��~W҃������Ӧ�d�b�]�V�<���/r[M�X�6�f���b.&�-a�������.�2�p�Z���inA�o�q�^�[c�Iz�Z��3#6)A��`�����1*`\O�~��4ِ����1�@�����@��k̞� 0y����5S����4T�!)N=����y6Y���DB�d��s&!G#q�g��b���+P��:��0�g�T�lX��C#�v�ͶJ&�D�PzZ��ϸ!r [���U\x�>L�`�&aJODi�a� �$��gcmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        cmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ELF                      �      4     (               �����S�   �ø   ������C��t2��������C     �C$�    �    ������C0    1�[Í�&    �����[Í�&    f������S���@0    �   �@$�   S�����X��[������t& �������P��tS��������C������C    [Ív Í�&    �����VS�ø   ������Cd� ��D$� �C0�   ��u�   �����d�    ��t!��[^Ít& �Cd��C 1�������э�    �������[^Í�    �����UWVS�ËD$�;��uz�P0��u|�   �P0��up�s�   �n��F����   �P	�x`�K�S�C�Q�
�C�C�C    ��������������F��tG��~S1���[^��_]Í�&    �������u�[�   ^_]Ít& ��   �������뒍�&    �v �������봍�&    �   ��������f��   ��������W�����&    ��&    ������UWVS�Ík$���$�|$d�    �D$1��D$    d�    �D$���D$0  �|$�|$������$���uI�C0��uR�   �C0��uF���������&    d�    �@    �D$d+    uu��[^_]Í�    ���������u��t& ��C,��t$�{,����(�t$�   �\$�D$�8��d�    ��������&    �v ��������C�D$��u��q��������__percpu_init_rwsem  percpu_free_rwsem  __percpu_down_read  percpu_down_write  percpu_up_write                                                              &sem->waiters     `   �   �   0  0      �     d  �  �����VS�Ë@d� ��D$� �C0��u�   [^Ít& �C��d��C �������1���u�   �����d�    ��t$�   ���,  �   ������   [^Í�    ������Ր�����UWVS���������������C0��uc�   �C0��uWd�=    �{ �t& �   �G�5    �����1�����    �S,�    �����9�r��u!��D$� �C [^_]�����1҉��,  랍t& ������                      GCC: (GNU) 11.2.0           GNU  �        �                                  ��                                          .             N              l             �   '          �              �   (          �   ;          �             
   <          7  N          U             q  O          �  _          �                           �            �  �   j     �  0  �       0                               A          p          �          �          X       W     �   �   )                _  �   �     �  `   +                                             (             >             H             V             d             p             �             �             �             �             �             �             �                                       $             7             �             L             ]             r             �             �             �             �             �              percpu-rwsem.c __kstrtab___percpu_init_rwsem __kstrtabns___percpu_init_rwsem __ksymtab___percpu_init_rwsem __kstrtab_percpu_free_rwsem __kstrtabns_percpu_free_rwsem __ksymtab_percpu_free_rwsem __kstrtab___percpu_down_read __kstrtabns___percpu_down_read __ksymtab___percpu_down_read __kstrtab_percpu_down_write __kstrtabns_percpu_down_write __ksymtab_percpu_down_write __kstrtab_percpu_up_write __kstrtabns_percpu_up_write __ksymtab_percpu_up_write __key.0 __percpu_rwsem_trylock.part.0 percpu_rwsem_wake_function percpu_rwsem_wait __UNIQUE_ID___addressable_percpu_up_write161 __UNIQUE_ID___addressable_percpu_down_write160 __UNIQUE_ID___addressable___percpu_down_read158 __UNIQUE_ID___addressable_percpu_free_rwsem152 __UNIQUE_ID___addressable___percpu_init_rwsem151 __fentry__ __alloc_percpu rcu_sync_init __init_waitqueue_head __wake_up rcu_sync_exit rcu_sync_dtor free_percpu preempt_count_add preempt_count_sub __preempt_count rcuwait_wake_up __SCT__preempt_schedule wake_up_process refcount_warn_saturate __put_task_struct __stack_chk_guard current_task _raw_spin_lock_irq _raw_spin_unlock_irq __stack_chk_fail __SCT__might_resched rcu_sync_enter nr_cpu_ids __per_cpu_offset __cpu_possible_mask cpumask_next finish_rcuwait       #     $  !   %  0     5     :   &  a   #  ~   '  �   #  �   )  �   *  �   #  �   +  �   ,  �   -    .  !  /  1  #  �  0  �  1  �  2    1    1  1  #  J  3  ^  4  l    y  5  �  6  �  4  �  3    4    6  !  7  8  8  �   (                                            
           !                "                                                                   #  4   .  F   ,  L   -  \     f   +  y   /  �   #  �   9  �   :  �   4  �   ;  �   <  �   =  �   >        7  �   ?      "     !                   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab_gpl+__percpu_init_rwsem .rel___ksymtab_gpl+percpu_free_rwsem .rel___ksymtab_gpl+__percpu_down_read .rel___ksymtab_gpl+percpu_down_write .rel___ksymtab_gpl+percpu_up_write .rodata.str1.1 .rel__mcount_loc .rel.smp_locks .rel.sched.text .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                         5            @   <                    	   @       �                 %             |                     +             |                     0      2       |  `                 F             �                    B   	   @       �                 m             �                    i   	   @       �                 �             �                    �   	   @       �        
         �                                  �   	   @       �                 �                                 �   	   @                        �      2                                      &                       	   @          @                            H                      	   @       `                 /            P                   +  	   @       p  �               ?            h                    ;  	   @          (               T     0       |                   ]             �                     m            �  (                                �                 	              �	  �                               (  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ELF                      �      4     (               �����S�   �ø   ������C��t2��������C     �C$�    �    ������C0    1�[Í�&    �����[Í�&    f������S���@0    �   �@$�   S�����X��[������t& �������P��tS��������C������C    [Ív Í�&    �����VS�ø   ������Cd� ��D$� �C0�   ��u�   �����d�    ��t!��[^Ít& �Cd��C 1�������э�    �������[^Í�    �����UWVS�ËD$�;��uz�P0��u|�   �P0��up�s�   �n��F����   �P	�x`�K�S�C�Q�
�C�C�C    ��������������F��tG��~S1���[^��_]Í�&    �������u�[�   ^_]Ít& ��   �������뒍�&    �v �������봍�&    �   ��������f��   ��������W�����&    ��&    ������UWVS�Ík$���$�|$d�    �D$1��D$    d�    �D$���D$0  �|$�|$������$���uI�C0��uR�   �C0��uF���������&    d�    �@    �D$d+    uu��[^_]Í�    ���������u��t& ��C,��t$�{,����(�t$�   �\$�D$�8��d�    ��������&    �v ��������C�D$��u��q��������__percpu_init_rwsem  percpu_free_rwsem  __percpu_down_read  percpu_down_write  percpu_up_write                                                              &sem->waiters     `   �   �   0  0      �     d  �  �����VS�Ë@d� ��D$� �C0��u�   [^Ít& �C��d��C �������1���u�   �����d�    ��t$�   ���,  �   ������   [^Í�    ������Ր�����UWVS���������������C0��uc�   �C0��uWd�=    �{ �t& �   �G�5    �����1�����    �S,�    �����9�r��u!��D$� �C [^_]�����1҉��,  랍t& ������                      GCC: (GNU) 11.2.0           GNU  �        �                                  ��                                          .             N              l             �   '          �              �   (          �   ;          �             
   <          7  N          U             q  O          �  _          �                           �            �  �   j     �  0  �       0                               A          p          �          �          X       W     �   �   )                _  �   �     �  `   +                                             (             >             H             V             d             p             �             �             �             �             �             �             �                                       $             7             �             L             ]             r             �             �             �             �             �              percpu-rwsem.c __kstrtab___percpu_init_rwsem __kstrtabns___percpu_init_rwsem __ksymtab___percpu_init_rwsem __kstrtab_percpu_free_rwsem __kstrtabns_percpu_free_rwsem __ksymtab_percpu_free_rwsem __kstrtab___percpu_down_read __kstrtabns___percpu_down_read __ksymtab___percpu_down_read __kstrtab_percpu_down_write __kstrtabns_percpu_down_write __ksymtab_percpu_down_write __kstrtab_percpu_up_write __kstrtabns_percpu_up_write __ksymtab_percpu_up_write __key.0 __percpu_rwsem_trylock.part.0 percpu_rwsem_wake_function percpu_rwsem_wait __UNIQUE_ID___addressable_percpu_up_write161 __UNIQUE_ID___addressable_percpu_down_write160 __UNIQUE_ID___addressable___percpu_down_read158 __UNIQUE_ID___addressable_percpu_free_rwsem152 __UNIQUE_ID___addressable___percpu_init_rwsem151 __fentry__ __alloc_percpu rcu_sync_init __init_waitqueue_head __wake_up rcu_sync_exit rcu_sync_dtor free_percpu preempt_count_add preempt_count_sub __preempt_count rcuwait_wake_up __SCT__preempt_schedule wake_up_process refcount_warn_saturate __put_task_struct __stack_chk_guard current_task _raw_spin_lock_irq _raw_spin_unlock_irq __stack_chk_fail __SCT__might_resched rcu_sync_enter nr_cpu_ids __per_cpu_offset __cpu_possible_mask cpumask_next finish_rcuwait       #     $  !   %  0     5     :   &  a   #  ~   '  �   #  �   )  �   *  �   #  �   +  �   ,  �   -    .  !  /  1  #  �  0  �  1  �  2    1    1  1  #  J  3  ^  4  l    y  5  �  6  �  4  �  3    4    6  !  7  8  8  �   (                                            
           !                "                                                                   #  4   .  F   ,  L   -  \     f   +  y   /  �   #  �   9  �   :  �   4  �   ;  �   <  �   =  �   >        7  �   ?      "     !                   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab_gpl+__percpu_init_rwsem .rel___ksymtab_gpl+percpu_free_rwsem .rel___ksymtab_gpl+__percpu_down_read .rel___ksymtab_gpl+percpu_down_write .rel___ksymtab_gpl+percpu_up_write .rodata.str1.1 .rel__mcount_loc .rel.smp_locks .rel.sched.text .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                         5            @   <                    	   @       �                 %             |                     +             |                     0      2       |  `                 F             �                    B   	   @       �                 m             �                    i   	   @       �                 �             �                    �   	   @       �        
         �                                  �   	   @       �                 �                                 �   	   @                        �      2                                      &                       	   @          @                            H                      	   @       `                 /            P                   +  	   @       p  �               ?            h                    ;  	   @          (               T     0       |                   ]             �                     m            �  (                                �                 	              �	  �                               (  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 # SPDX-License-Identifier: GPL-2.0
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ׮�r����k�(]���^����d{DA�Y���Y��W������04��>gz!�B���pzq�����>s�85<��P
Y��#0���4D����h]���,jfl�z���q��]����O�D��O�P�'^܏q�oPag����N�����S#V� E	S�_4p֡1(�� t�V�8}����R�ҍ/�l�n��<�ݑ��Bg�jLM�c
�T�F�Ai�[
���!��&Y�8���M^��iy��,Ș�d�|	�>ې��B:��^z�`���{��K�lsN��}��:졗��9$Y�#qM��0G��gqq�ş�'�Xf�1�t�t��n"�Q�2�����H%g_�-Sc~�:���0�6g^����{!2��C��J��}�u����wE%�^�t��I�	|�4��Ba�ñ7" X�`6Id��$Uۃ�Q��8��h�[�A:���eg�t~�@a��T���-��h+�?9�j)\��o�G\�c��/��a��F3`��P-�2��rV	I�-p��͹��$Fe���4q�����G�-�	OҖ����؂a23���d;��ǜ����R�����mN \��;��?���`bʫ��ؔ}9\���a}U��L�0׸�<��y�Y�y.)N�A�0�U�N�2�3�C�$̨or�mg*�	9/�C�yW-n�����(�8��^��r�&��(J=}vF�`��	,K���_5~|���=��z�9#�W�.hS#*S�cT��
>��^��7Kt���`r��D���jo
و f1-֋�{D(����J}��������򬴬�����t'�@�x�����*^`Z�)w���ڇ�i�[Y��>��.��{C���ą��>[�
w�$�#Z�TcظT �>�1׀��1���mEկnY��J�Q"���^D��'�E��>��y9��ބO`e6C�	��9�����'�7���]��)X�i�cH�=��iۤ���ߊV�߲V�)�GLyi3$��-�[?��W�B����;0A�U�]ί[�]9�O��hӾKC��o�c�/�l�x��^��n+?ǁꑰ��ʘ�@#Uz�D�g�ړzZ�%�W�|��aԸ�2���x9mfz���0t�G��h/��'3�Ӑk�W|���*@�i,(�W*�f����*�Ww�nM���sC�Yu"�=�G�C����y�p��'���^��W��4.�	���
z39��7�;��h��,���h3D<{g1\�2 �ݲv���}�$�ڈ�m?�@�ssG)��י�fKC�\�L[��;�'���5#�I��b6Y��Ȟ��zI�sԶv�����隗���w}��#��ߦî\<"�*^llW�j^��,��т,kv�'�#�y�ɠ�����$��wˎd�9�V�Z�%�
8��wOA�wV�q�ۄ���{��/�R!��!!�3��<� �����=�` /Wbs��k�ք/L!�'|	U��V߸Pk�R���&T���M�йŴ�fEx�Y\�dr�>�;�W��;i8�m�Ֆ������t7@��N?�R+Tߎ\���K���fN���>eN�R	��2S�V�o}hP�~�e�f|Ւ�$E	�R����y���|���V>����A����xN�.`I�]�ԥU��� �����	�wU�ϔ�Aυfp��;ƾ��-�s�c�a]ND��ӫ���8jzC��+[2vDkj��2�}��e�]?����G�{��e�Ct(��Z�`�'�`��e+.��~(��ś#̤Ӷ����'[1�Do^˄���Io��لz��F���1��/�2Z+u1E�Ԍa��ܧ��&�EL��`x����-���	�s|-��3�J��������,~v�B��<4,,R*VP�|�N���w%�vx�6p��08�XQ㪓����h�R���S:�v7`]�2~o��#ڄ�[^;.^�7�uJ�A��k9���wB���4秈��

]���y���A�q��Q���fMʿ��1��G�Y��jz;����f��:�^����8~�#7L H���`��y�o�Z�by�������� �}��ވ璆ے/�>����ҀbI.��,s}`�*g��.�JlS\�R.?���)C�B������l�V���|�)!���U�%���{���
����$]$p2`��az�i�wKK$Z�s�AՍ�M��ae�x�C�.���Țv������C���>��a�6'ui��b*8:�=��S��rE��L��J?36n)e-�����Ƅ;)���N�?2�^��K@�#������P�{kfd��+����!���ڒZH�CHc�T��$�{�f�&-Z��Hf�����89.@Z4XΆb�-]��s�b�������h�n�bi�=���1xص
��W�q�V^�i��ޘ\����wr|����[6��.�^�(�\�I�\a�֕?�?��t�U��b�=	�Sp��N/�����\X`�0 �g2x }����S��q� �S�D�\A0L�FL�
�9��=���zH!y�&��n�?Z+��c&�^́����Nf��k3S�QO��v����� �vʛ7�OE}�:<��a��9w=K"�?��g���4�b6����ek�n;~��?IS������P�2�l�r��#mf�)�7
Y���Pm��(�+����
_��F_��TB�?.j�V�����*��nX��XWO�Y+aXGX�j�Ά�/�;�V����!�Ӿ��>��c��� �܃�/r���&3g�0e�jI��·bD����)��u/��[�.?���#���fէo7 �@]9M��I�/$��[�������;����=Q�n#����A��ó��J'h�j���I�H��W�p9זu�1J<)nncz�(�T@���sؑcm�{"%8Մ��:����R����5Ѵ6>2�x�K�#�5>`�@d���������G*��SW�Q�|��Nz�8�J�Ԯ�l�
�`���(6�Q���7�ǂ,B�.�>����mݹ�\3��DF
��N����y����E���W�蜠p*���H��(K͐(`��	�~�1׌���:��W��$��Â0SRa���`�eP��Uqp���Vd��g#�uX�~f�Nt>�5��'�+32(Kx������L���t����;�n��c���8Mˢ�ye����v��Rbғ?�b@	ޔg��=9��f.Gܭ�m��q�8�p?�|�<��aq-Ŵ[<!���@�j.��"�^��/�H�|I�t w�ue�|���+٧"b�h��|�����-�k&t�r"����S����x4�������t�k)��1L����ZV����2n�A��|�ʕSf��UP:�ۀ-ߚ�0!��5Q�O� ~g�X��؄^Ǻ���^_;9)��.,�P�Wn0�*�&��r:h`W��x�#m�`���ğzv����U�E�h����#ɽ@w��2 T#���D�,'D�m4:��?��uu�y����U<8�S$���
D��N�$Y	%fj�	��U�������U(a�f�/�Z~M���[:�eqV�8'�oX�R�ĹW�g�=$�[>�NS*
��9ɊZ���rҽx�������^Oʅ=4��%?�Z����&8Gmz����(bF���VF.�1�B,԰7i�E98��}m��6���%w�Ǘ9?��/T��~I�dN��;r���F��6�e�V��7�݇�
��5�U�EJ�9壛k�cl5����&��%�]7�O4��}@/$Um��v/Ȃ�izůjJىɒ�KF�D�����&E�/"H�u���`oD�^,:�6��q!r)���
1䐑
9MȠ�,+|��P��E���")�U���H���
Kt	���:�6��U]ͷ�rm��C�%>�(�$4���@(ti�b��v]��<r4�M�[�"�D4��tk��k!Uy��s��TT�E�q(���V�Y*����#�RU�2!<5�}���Y���	%�cI�s~zN�W+�g�"������u�wCV��O2(�g����l"�}���x��+B.oC��4����ڽJ���h�gN>����iV�Z�p�b�Ƚ��k�%�{�	vA|�\���\6s�p.��<>]F{t�4��Դ8$�����_&L٤�Ýu�!jKVE���^���Ya�>q�����oxCȮK�������у�R�Ԡ2���t17�~bD@_��j��0��ܼ�����}�P�oǪNkl�Q�v x�w�I���D�a*#�pR���%u��v��MX�ɰ^gy����2�b���]}zb��X�ϺW<yiQ�������m�ܺj7�W����@@�.l'�r�}s�3���UQ���C�4J���:�6��&@�s�m�7�<� �vh��cװ=��3��:�8kΓL���1&��mTY�e�${Uu�US���:�8���]@�0�������dR��O���?H��p-�c���#5վ1�4CfY�+"|ɺi3O�UW�M�9� C�6��^f;6<1�Z%O%�G�K�sc�*%-��:����b�pg���H��E��B��;|6��n��=��03�U,|�������;@��)���̾��f-�Ѡ���5�0's���~*3���5�o!V�/u`���Q�Rt����/�y)�?�$!�;�^���V�f�������Z�]�[}c�!eEC[���Ѧsz�9�J�ڦ��ي��o<S9�=Ou%UL�¤ට1��:�hf�'Oa�
L.K�r n��	XP���,��N
?�s�8�0�3:Y�(qHPq��K2'χ�X"��\o�G����?4P0���a� �1�g�˫2�� �݅�p�&�LM���{���Ôj�l�2���Y$w愾��WF������i����x��o,�=,��.AC�������nݠ����d�n�S���鎙�wc�  ��@A[�O�΀d i����f�#���'��ӡQʱ���M�Yд�����B�� ���š����,�5�ͩ���3_��U��e����Z�P��*�"�~8��\�)���B���M��5k�4�nC0D#��ø3��' /!��\���<���ћ�ݿN��SKp�xP���6@b�>�Q
_��I��|��/�}a��¸�]9b�[���9������cA��`OdcG�"&+����>�Nm�|��ռ1Ͽ_;l"���_ʍ�+�v�EF��u�d��0иxYM~Yt�mJȇ�9,�όv�y���ұ���5�����og M�&�ĽzA�~@1צ��իq�u��;� o�`��"��K ����\K�E�*}�m� �z_�cq��	V��a����Q�&A��?c:4����k0��wfV-b�e��R�_GN핼ٔ��w�۔^�Ro����k�e^��~�����v�la���f���t}VbMO�&tX�pe�*��\���9�
}��U�O?��@a{���%4�����ka�ͅ'I��sz1d��nj���W1訸@��a�< �iA �����T�Q�4vhs�O_�(�9!���0L�I����K��b`��b7��|��(���^�5�9��h������X�'�������u�W4k��[3?���H�p�*���)�w����m���ڸ�,�!�}E��h@D���x������d���ԁ�-J����T\ĭ.`B\|�ɊC��n8v�@��E;���}�� �OT����Dh��O�U��v�q�8ûd�:?�����|*J�qz�QU�"�B�cg(�"R�^ݹG�*9�~I��^��4���"�"ᧉ˪Y2dZ�ڴ� �Ǚ��:&�չ�҃C����q�7�:+�I�:n�\�H����aI���ο��]u���۰az����!U Op��K�[�0cܭ�$����q{A�k<5�� �+�]�˭���ǀ�A^���Wx�~���q����57�e���&��d�~$��k2��o��n��^H�o4�CO�.9��!��%�0��;��*vQ���%e��(�ⶁ�Q��5(�K�"p+J�R�F�"q�Y�TR�Ãn���kq�i�&ا���E�a�#����=��*�mҸ�Ǫ ��Jϕ�% ��8��{��d������čQ�Q2^����KfT�|Cj֧��n���B$I(~����ր�Y;[~�Q�-s��(�J	��΃��4���v��4�E=>D����HĪm�尪;�'fz�t�9�	A%?��TU�axō�u�<4�:@����BJ�=�����y"�e�&1����T����kwO�\���&?���N�w�]��P@��P�s��)�Rdf,X)/o��g		���o!�G�ѳ��m��nčl�ёh�/3]<��3H�����%���/��k�@6��,� nN����
�g!��jq���;$1p�퍁DK�"G:����!��f��
�^*��!� ����-���/77P7G�24�2+:Z��E�^VD|�������۫Y��QX�P51�X�ӰZ���� 	�[L.!V������֫�L a��F�����t��Λq��P����X�u�[}�i+}g��\�96�!�_a�o�Qy�#]��B2�ئ~�5AG��d�������%Y�zs�R�u��+��&��ep����Q��f��J���O��u��y��\��q�z�
��d2�H<*ǐ%i���ڛ�d�fc/����h�l?�p2��[�������+2���魛F���!t������ �����4�8[OvZ1|��&&ĉ"UAD����L�ji��\oxU��8ϯ�Z!+�@==::��(�N�?WQ>�vo�R���5,6����0�	o�ѷ��6p����{7!ū����Y�yWݣ���l)1��X�r���Lo�E�X��s��Q��CS_7?�5����~N�9Q;n����H��K>�	�e=�чj	����;y������Yu�R^"����`�K:����5Rp�o��qs��7�eV,2q�O�І��y�+�48��Ϯ��q����)�1~me:���Rܺ��]gj���3�u�I#�m젛x���W��<�d�ѓ_�����a����%��\�����0c�RU�B�&>HzȘ/cl����f�H"d��S0:�A�U x���4Y����>�j�^�`��GRꗚD���N�@io�������@c�~��;n��vO������o���O�\���݇���I����M�B3��N�x�iA����Z(`I��j?�&���9YRL,�c�
���Yq4 B$�'�L��ʻ�Z��_1�-
���U� C/�ch�it����\��i�tik��Cl.�w���C�ά2���M�"N7e4��聽'�q�z�q���z�ؑ	�XI��VK��s����9;�τ��1<0:}�D쥴���T��4؎�>�����T"�T>���כ�������zb���v��yX�*��6��2i�B����u�+�l-���/;r?;��#�BqD�d��)$ɸ��-if�?����>Ն�+�lVd���[���x���X�`�\����G3�g^�H��b�ȇɟ���I���aC��<J�DG�����|9�2G����2��,";g�����tJY�Ĵ���z,e$�yT�N�=r��eSH �O���J��{&�D-�3��,�u���SH����i*T�Y�>�T�g*��Q�W�溛�|�6��Q��&��1sY@�ޖ �WuTTpw�EG�~q �)S��]z-ˍM�gg���}�F�s�G�ߙ��k�歰k�gN���8L� ��;�;���3H�$Q#/9p�Q��!����H<a��������d��7U�qs����:K�ř衡B�����_������Z0��Zbs�k����\Fx�,5��o o�-lD�XjԌ��ͣp��"�{sb)ƚ�� ������i���]ԗ[T�H���Ϛ��J�	+ǃ��;�$.�// SPDX-License-Identifier: GPL-2.0-or-later
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
	if (NULL == buf->sgli