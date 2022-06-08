
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 �
t�>���\J$���XdL�=���CV=p��<�]��^q���y�B�X��B���t=m� 2��"��6D�b�O1b3z=8�,��Bp5e/���O�f�ױI��=�J��;v&/o�)H�`J�Tڑ?>���f�!�h���S����5���$�7)2%`|���-�o0$��y�q֊rp>��μ�f���mY�J���\Ys�bɄu�Cޅ�w^���K]3#C�}X�9XV�t>�"Noo#%��s/��������iq�`r@ 7�^]��sA~�0~ݔ/(��P��+P` ����z�0E�y\�M؇J^�̔��ҬVi'� kS�{���b��R|)8������7r�vzW���%H(>~a.�_ϕ�"�����WƹB?0l��@P[��xD�`�T$:ڌ��|!���#�.���}_�S�Ëvds~E��jW뿏��8�<-�,�*ʡ��w�J�-��|A�pȦ!U�t
�����Wh�� �t�/���%M���k^/��u$yRn��d" ��)�.�،���+�[��-	'��X�n�$����IL��8\B��+6Q�ڢ�a<H�r��tX"b�38�
���#�9�(	��^�\t�~'�d�U�1c(���|�V��M��A�ǋ�6�V�Z��G�f��f\�>�{I�QA(z׍�X�Y$C�l;� �9X����4��h���k�Tel�����q�� �C�)�^�G��eJ�Q��,s(���ca-+[��Z¿�������)M{<8�EFb���-���{X��x�דi���}<xn�������-I�oT����^˄ ZDr����c���P�����@�X#��:�W���O�e���9��U��%��ɦ�;���V�"ݰ�$�U$*�>�s�󫥑~I��ᄼD����SBo��c=%�s�5}���ٰ��i��|��Z�?�U���l����\4�1���Ӕ������q��G�%J��� �>���Zn��:��{�B��.T5�9������_	g������������g}&b�-�����C�G]�a��J�6�������W&$��#N�0�q炦��\��x�JҠ��U%�FX(�z��aܸ ��17xnsu�\#V��'�P�ឈ�k
������3������3A	Ԣ>|	�6An�����5g��		d���� �&G��/K���1��oׄ��nb�wF��_F�E8|Ϥ��b~��΃�i���2fg����8��Ԥ ߨ?��Aץ>�X�/�~�\�6��ի�k��*���C�ւQ!,���,ܿ	-/�jο���iT��|5�:dJQ�!tqy3�y5���y�fD�P�^U�?W�~A���%��'���+�&�t�oQlW�� �EK������L�7+��eo �WT��!�YٓG��5����#唒?;w-�DpRi����pI�hwy^0���\���z�[���Z(��O�si���h4R��}�[�J=J��N��[�h^䃭T;Ӛ�´�|��6�	�!d��jG-vs���ֶ�:��'� \��ce��2�e����a{W�
���y>	��U�Z��򈠄���g'A��Cy#q��;�����قM7(�A	N��WN~wҴSB,�{��*�*5��X U����V	�XI�
���3�t�Cd։@��j��S��RZ��+s<�B�_�HiB�E��UȲ���:�0k�������	��J�N����M'�'�I^���w̮H+&�ka�!�[幨9��*���_7��n�8F�n���e%n�,���h��'��V�x�K㨏
1���"g��CC��O+�7r������l.;�	��Z��<�NP���i���r����8�gP�����X@��� �"�`�sғ��G��yf�=��}�9m����/C�<�����
)4{�o2��7�u�c��� ��*��������FW[1(q��!u�D�ر��?�T�T�*+O�c�T��A>8#�Z�h��$���-���]N�Y��8��!w/u9'>��N�g�t�2I�3E�1���T���yjx�F"��b����G�w�㐧����z�������JxTC�X�ȉ�AL�lQ���"�YrS_��T(| 0�WSL���,Xs�,�2g�ImB6�4�4&7��|�����A{���k���p��pE+��ݫx�\���ؚ^�Ė"��1�9R6�AsU��~�Ji��&
��a�Oyx���I�O������ꢫA���
��a�i�è�5�_���ɫuf��>��Xo�Jy�dR����h�˰��T*���k�~v�|�[��U�|U��;�%kK4�}Ve���Z�*�εY�
DΒN@fRZ76��RU���mc�&�:�[C��FDE�ױ'��ئ	��4*M\G�_��p`/@��Ѥ57#���~'#�J[�ܵF�Η�ے���ѧ�ǧ 7�t~�셀��8�	����;�e��6<����X6���Uy�Gl<s�b�3��tU
!r��7�Cd轼AF�f�c�~	:u��[�r���7�Ez�PU��9��'�뉅[$���;�=��#��F�P #yI�-�8��{b~H���ލ�#nr�،�Z�y^'䒬`䃀�6k����L0((>|�yD�<uL�~^o��*���2�`_M�sTB�L��jד�1#��[G�
�#�0����w����*��L}������:]'O���l�bbkK��P��}��\F챇Ⱦ�D^gȜ#�qj'q��_Nqp��?5W���\�5�Ɣ�Yo�X�Sy<�PF�A���<J}
 H��8��V��c�!-�&��wU�����jg��!8�����w!"��DT_ik�*�<�z��	%[�9�ϯ���v�V��)ЕW��a����.�ӄk�:x�[��	���jZ�g[�S�C��������J+$|e��ek�8�9? ��TE��xՉ�@v�L�;猚ʽU��|�����aIMn�G�JTBS3�e���!S���H���%�K��6��8�ӫ�p䏂�A�V>Օ]�j[���4@2�oJ��+�w�4!�kg���\Uh�s����Kh�_4Z}@7�/K<Ҳ��ބ|����6(f߸��BI�갧=LkĂ�Y-ц���l���k�"��f&�i��%�a팾��#�ψ?�yf�:��K76OM3U�FK��k�+�ZU��=UIE�Uk��80��'�7�,�z�^+7�I��SE�Q��/
�Ca1O�����=�vư��7�tҿ]F�׿$���+a�ŵ'�@p\�x%��%��.}b�O�a���lvk�����9�t%I�'��'��X�V}2ؠc�
`�C+��U�#s�`(�j�+u�~C�9?����"����(��OK���;.+�d*н���ND�a������i���
�K����+$�p�yW����F��@����?<p篧���M�c3�)�֟���Q�CyЩ���{�񷶗9�MG/�I4��,Cx0qW�4���g����.Y�rn{ֿ�S�u��,�,���Σu�6- e��O_�js�ׅ�c����,Ŀ��\̯��9c�Q��r����?�(�<=�F��/��g�!��@��\�b��Q@�As����]9Hk��PP�S�f`7Iֶ�l�����D�FV:"�z��ʷ��	X(��$�k������i2 �?��k�f��F!� t�@�WM�UM�WI�MS'Pb:s�}a��j|v��(�.��;�(�61���J�g�g�KO��寮�H��
4�m	k�4���n�T�����`�0[��
g�Jj���U��1~�i��O9��
A��j�}����������I��m,V��>.�%c� 1e��y���'�.��S��R�r�P�,[
�����XZ�G^ �0���s��<[���#���=�m�Zhus��ϒ��b�ׅYsT��y|��T��X�`�wȣP
�t��gu|E�|�ܺ�p��̲���e�I����K�7�G���RB(�%2����Г��L�X��c�tR��*H��v�4^W��1�3-���%8��y�V�w��od�h���&�v�x߂��A�,Q�d �hNR��.��p�5�4u����|�Q��{����,}�.�u��h���.,̆&ʾx@΀�LQ:�fe�s7�x =�M U
�f�D��s�A��aN���y
��@��55���5��Is�����ENYY o��5C����H��P�,���N�u:����+�X�_��y^X+�2M��v�~�X�,:�dg���H�Jr:j��@��h�˴W嵵 I@�E(d#}����꼉�K%͡� �GP�{�*j��N�qTg�#�[��J���o=t��:��0h�)�kd�!�yT���}{H���R	�N�~�h�p���y+���N�O<��G#]�SQ�����T���RO�P���+��U�_��n�[���ϒ|Zm�g��L����k��1��3�u�¾�u�s�k<�Ӛ���*�qƨ8v�W���0I�f?:��n�(��^#���k����"�y�_u7x��í' �R���+�!�u�T�[u ��)yw"�x���d	�q`��_k�/�}��1Y�������d�@�����\�_���硾АS��$�Z�)-�~�� � �������h�u� v���БGfJ� �]�} ;�����G��U����H�r@���^,!q͡᭪g���6�����)��w �[��f]s�ia�Q�d�ף��Q��GYS��A�� &TSGU� F!�M�t�(��Av�XJ���}����L����'�* �����+�/�2ܓA� ��
(.�`W/��C0FktmV,�o�_�����o���G��珱����h��Nyw��b�(PD�fN#~@݊�Y�jE>���T��>Լ���m�G`�#�p�7����r�w�CT��[�) �	�a�_MY}����'�)K̡K�~��û'"��]'��[���#�6��؋��䴽ͻKXC���K��Ԇ��0���3~��r؍w��`��d>�g��,~_��չ��_>º)�����aD���[y��ܦ=�v���/���8�6��oS f	�d�?�%�(P�8�b����.jkq�V`��T[,�/"�Z�7�yj����	fy�Ǣ;��>�,@.�W��x��cR��4�t��MS���g�+ߞ% i"��z��!<>(�ȶ�J��Ǜ�٫S`݊F���EJƕp�wޢ�(W��ή�kd��#r nWg��/�w��dfP���0%u$`߿?-��~S��N�8w�.�m���)�f{�H�� e����v�Wp.��fJ�Dߕ����6;�Yݔ�T��t�Ds��'Z��>��ʦ?��i��A�
���O�Ϲ�ա�g�ٰ�r	9�!��Є��v�+��=���x}�kb�k����+q)t@i�[����Q���+�"�v�k:~��`������S�m�n���6x�Q yf-_�'��/=�^Qn㿍0�84�i���_�d�)(-��;:&�4�����Zb�s�l��W�7d�@'?�={�� �$hݤU��?ٯB���^��H�������A��ͧL��x[�w��3��_��(��K�yo:}��NҴ�D���ږ�U�ا����uF� �:,�����d�9���}��e�YW�����:�.�3���ES�$���sf2ѹ6�5g}u2�ac�u2�I��B;�^0q�j^�0H�ft�9{�(G��,\������'b�z�Ȯ��}s'�S��C��Z�� 4س��n`b���qd���𐊪8\G�.e�4E�_�K�n2�il�j�7shv�XqX%l�V���	��f��v\�)�(��M��>�?*a;���K�:�h�$Tש��/&&Ɗ�H����R�@�{d�~m��B���ijedY�(	�kN�<��/<�����v��z�����f��D%��#o>���C��cF1�?���t$a���1�g�D5u��hI�J��Ġ@5�
���@g�!�I;f��O_���?��7��z�NZ��z���7(|�"\B�P�C�0�j�ȆU]���ǭ3S�ڊ\�	 |9v����?I
Y�Y�1�m�����8-J�u58���@�T�	�#`��\��[!c�eM_��}{ߧi�)�O�0�%��؄�:dG��iOH�YN�gOC��2ɤ���|���¾ͲR#��`��l��U�ԡ�=eXD�|�]o�Lm�)�o]`� �l�l�yG7�r��K�8/��Z��8h弦�ػ�Y�:�䛳t���4�ʅI	q��9%v������R�#���%����Y��ߔoP�ئj�Ѿ�VTm�E�K�x�Kd(�G���h*���p��T�a�O_R���1���O�j�N���ͫl���0�;mO/,"k�S��⃃8�ě[X�"�!�h�=�c@�NQ 6��	�pZ E�3@��^'�%U�]���eG���'OkD��[@�}� s�x�kOj��x��sT%p"�SMZ7�B|
�mLa�!'�����sQ5��:�5|����|�+�$�/'w|;��o� Q�Jw廄5[�I�M�4�hah�w~�b|�@���H��d3=�
�[�G�h`Z y9�k�)`B���4�"�^�����َ����� z\��A������ÐV��? �{v|[�Ġz�r��ud](��Ʉ�@Em�-�J��y��<Z���{<埽r؋>5��)�B��7��z �!�34tpU�9�L��7���1{z��=D~����f�*
Y�+Ѡf�k���$XLPxb�4K�*0|8���4�xw@�%���g~�G�P�?g�.�_��0�|����1<7�E3�t�m��������}��t�K��������\E�n�_��J�Ԍ7�W��Е����x�}��� Ä����ɂi)͘���p��u����Da:���py���"��rs�.�C�=:x�i�T�Ҝ�4����galet���_����s��]$K�;�m��de�d������@G��x�i����L��w6����s�Kq�g^�!*��Ǜh�����4X�{��\�%9t�qB�4�6��8��zֳ�����+P�Ξgm�}V��hZ�Nߥ�@�8����ez�������M?�/��J�#l��d�L���7�6�ΘE$�K�l���2�Lj�?�����FS�A��t���_y��X-�pU����O�~�m6�5E��W�����K��K��g�q�Gڜ'6�/��u/�Æp_�AH�u���Q:�F������JS�Նb"����� d������N���.�1c�l���(�G e""���j�
�ߛ � |]��nFݲ.�ul̥��VQ!���穛��(�4��S�[�"gx>B��r��� ��ن��4�H�+�� �������U�IK��{���k�?����
��!�Cp�5y{�@����§sxS� ��5��g���?2�l��^S�*�;�%ʥ���E��18o֔�ƃ#=���^�31���{��L<�%����+�U�Ne\JkB���t��|�喾n0��e��g�-�'ؑr9�Btv�W���(6�[߀�8@��݌Za�/��,�_��!׻0�ߛ��3�
����W̓�Ί���2�]2���A�=���F�|<
{o%]�]�%�}��?Vt|�RL�d���"�V��4��[Va� k�Xp�,�-,�sG��<�a�����\��pԚ�Z�x���0v�u��9]���|�L_�D�����f� 1w�_�#tLN�3�}��p���c-oKv�-Lb�����A{�K�7���y�0�/g�����:��J�X`E�4z���A�lx���Y̅�;�!]y�V��~�Wv��7v����+��$U���&,?|`�N >�|I �@>�A�*��_Q*uу߶D��ې���-���򍵃/��[b?��ʴb|�@�L;�;��U�݉B.���%,&�* �M)-Ep�ڡ�ボ�`���a\�z��ɞ�X�MJo�v��򯔄>K�T�%�CТ
�v��t�,p��]}�볟��v�%����������=	i�R/CD��
���//n��dջ�0�?	��v�ტ��!��>�������IP�����V�G/�rgH��l'�	w\j3pv��pZ���;����S����_PL��b��Ƽm��9:*x��ȫ�rs݈��w<��t�y�eqr��I ����q5�~������p뒶��"�e��x��+L��'�TՏ�79�s�^ ��>1�0:�N�,~I�ˀ�-O�ⱐ��k�GdC��Co���$RWW\��ɚ]n�x��]c�	ߖNӞs�s�K�ɪ�Zm| x�A� ~D�8e�[ǘSwz��&�$��ݽ�)�Ԑ��j��.��pW� �	��O�lo+*-��M}չ�����
%S{'��k`t]���EէPP�hR|v`�H��5b�wQ��'˪�+���@Y���&OlӋ|�U��-���;���<�-���8�����lB���	j�ʶ�Ss�+�v�ĝ$�ttY�B?����:�C��ր��M`3��˽1�3_N�d���]6M6�t�td����wZ�Lc��p�1ɽ;�8�����}�|p|�����;G���'���$N���J<��~cg�∎J�s��pIx�M�&I���>�s�kc���B�-��}s�i����,�<���S���{�m�҅5�aqm��-B�58�|f����X����y����k^��nW��t�[�r��G��b5H�`���_$����7
�7�\d�qZ�;���=8�]��!O&�����ޕ.�\�BSY���Y����K�6�<5��P�{��|G`�p�����G�!���ƭ�a����a=C/
�M�J�F=���[��y�M"�k#��@��O�����>��<�(�h�?����}f���a�E��Ґ���HmE�,0~D��Rg;y���~�� ׽ˆ�wQ-ɢ]����G��"?*Rɴ$��"�,��	�ۏlJ�?�]�-Z����Ǥ,#9���HK����p�����\6U<zO�d���Kw浙�/���F\\�r ��*��3�ݜ@�\�r7���,��j�cs��Y_��>���w������%�vs���'Ǡ�D^9�2Ry�@!��l�۾�f}��Rߕ���?:���a�xpa0{"E�ѹ\|���V��с=��	\���7T�;e>�;V�"(�y���ǐ=���Jj��ЯC�~8!�F���M6a�C:U�-��No��dJF#�����
��#Y;�;�# �������r�����it�A��J�x�~*	:���~`�)ę�&��D�HV+l��R�*̨��snZTq:c��X�.�!t}�~��,���'q��负��$�0SC��M����
zǩ%T$Լ5T�b�j����|����yN��.m~d ��p�^�$�Ǹ���{Ð	�����k�B�M1)���||���T"q�00��[�@{��0�R ��CW�u,�qT�����4i�|�R���ͱt���aQ>�{�A����R�����~8�nw�PN�6B��PJ'L�PbWܞ>�g�'|#��ݜM,+�bh-a�<�x����)V����'��
N�S��;��.����vL>�n���H ���8�+H-��l����b"�y,�D��u�(�Jع��B�EUDW��h���k$:鸨������")[v�����2������y��[*��)C&�����WaM\���AQMSih��8z�+�L"4r!v�1H
�Tئx�'�6�����1��^ۀ{w�4��c��F���F�!����v��������P
���9�u�3��9ƮLb��3Je`��`m�V����n�A$ߧ#0x&wʬ3�<[hr�F w�?�;N�<�G�el/'��ӯ�b��W��1-�j����j������Н��F�s�Lq�����V��k{�p�JF
���/=��4�&����5��׳v�s�<&�Y�#{H���p���]M���__Ӧ��4	�)��LK�~�7+!�Z$�WvWK�ߵh� 4\;���/I챃z��~����ͩ�����f�m��hX��l�FrЊ�cɣ�l�<�_���Q�!ż�B	W�L6��,$�8f�05..%e��<��7��T ��}U�ݘI���6�vv�E��%,��ٕ���;^M)[~��DJ�B�#F����T�R{N��Z�!�BH!���p�Lae� Kp��T�o"W��@���U�G��ǯ�ה�{*ކ|4�2���[;�$m�9 ���0:��nZJ����z���wś��a
�-��o��I�6�Ii�mr�5�I �����)���pu�9d��,�h��d�M�ߦ���pL�x�(z��~��+��$�J�D�X!T�K��D�d�!�`���ߢ|�k�c2���?<�}9�7F�mO���t K&�BT{-ĉ�-sc�<�}�����C�AM��	U!�"Yj\������sbAٵ���\e�n�o���-�H>����l�����
_U�6Lr;�+��\�i-��o�'��R�B]#��ˤ�����U�c�XӘ��{�eA5߳B��w�fYq�2f��q&3}�8�*Wf 4���5����k�j�rR�ݛ���#.�)X��I����cf�4�����L�ߟ�����4kh{]�!�+C�[<�^d�k!���y� �4�p%�D�8J��t��D���+����л��73��"Y���O]�Nb�^�"�\[�.C�)��U�Z� ����< �����|�F�y�p��[�Z��/���yO��-G���[<Q��T�(�t|P�Y���J�Ou���*���DEx�\׹I˗�(�g`�t�u�
k'aO΍�o2L�!6�*�9#�� �0ܳ0bdtw'��Q�$>ONxKk
9��Kh����£��`f�7�'m�}{��2Sɡ|ҿ��95)?	}5�=V։��戂(cmL)���+�,�t�墐��BΌ�8�آ.��9ۆ#++��\�&�-eg�_�� �	U��?�(K'�"�:��|�A���(��~�j4�ޚ�lD���(�LL�{$\
b�N���Y@"b����g>�zQr�Hd3�#R�+�F�/�,����n�6Xdh���lN��`ˆ�35\�➰"�� d"B���=�ކ�7��NCB2���f��ۙFv��Ƣ�	��P�������6��4�^�g�s�o1�{:�y�?�*�$�]^/*���>}�ڏ�$�� � �J	s��O�;�m�d����z�0�D�a���O>���,�E��e�f{Fp<3��4�s%��Y  <�)��\^Կo��##Z�Js"I����$P���g�v�]8:��]� ����B)ݑ&+�{�F��)%V)|�1�{l��{�d~V�!&ĉ����δ��ؓ㸗0��Wz�9��v���{�P�Mx�H
��C
-	G{Ҵ�ΒƔy�ǊT!�=�ת��[>�ۿ�w%K�7:?� u��`tQ�@72�!�R�`L���C�H�cN���G��n�DAI�}i���C����jJ��Y>X�xG(7���j?%Ȉ ���,���+��B������:LA@Вs�l:��ٗ ���A,Н�K��j�m�w�Q��Vw���G��z�����O$݂��Y��k��-�#���ϛ��'$��/sSl>��g���D������+M�
���Pi�g�˝!<q�=��m���޷��z����N�W~�)�xla�_HP�ɚ�=�[�|��	���3��2�I�x*����5W��e�sӚ�˨Cd��ZiO��8evXY^�L$_q��������`�H}�?�Y*$[9�t{ۍh!�nf&%"��@N'XJ�������4�AI�=.�x��w�1��LA��������e����GE���O�̥�w_:�H֧��vV���^�K�����m]�	{�F &�ڑG`
~07���X�oͨ��8��嚝��i)�&
SUlu��J�29��{��� U����
�L����s�)x��k����N9k.���),�TM�5&���ÇY�,�Z��?�^��+C���� <����HS�bb��8d����o��rw(W�}��hА鞮��
[��:iz����v��z_���n��C����k+z���L��,����B�fң����-5��Z����e� �<����3���Cp�:��d�4��Y�5��T�B�J��u�5��Y�i�M.��	�)��&�0�����T�s��4.~�Pߛ�y�ᖮ88pU��$��&D�S��Y�0����Z��%q5�x�<Ԑ��;��k�j�  �����˄�V���/�1SǪ�l2�@��xY�!����)MA���p����p��1��5d��N�=���8����;1:5��wIh7��ű�]���&�6���Ћ�)��C0��M�%ҟ���(�vx�}��Sap2gG�@�,}�z�hE����$چ�h\�s_���[�/j�[�1�1O�XM�f5��_vzA}l>��iOa��b'�y~��Z����H�wۅ�O_� k�E��뺗0��
[L���aO��\o0E��3�m�T�<�E٨�?|�R�~ñ��'���]&� �?�J\<)p؆e�G;�W��1�c^_D�|�SN�P\�	a�o����<���u/P>6��l�V{�=�={6�i��$|�~���u�`:Y��c���`w��t�Mć4��K�l
�����3�)Q��y��N�$��+c
}� ���os_w���n����%b����k|�����+��~��.���$�DL�k�"���"�|}f�dY�{�ì�;�P;���sJ|���&��J��u��O�^O�N�B��߬5�]�TV;u0_k��P�>�Cp��I�ލEm��<��Q�V`@$F9ʐ�¡.�j��ҿ٠G.�ڌzۜ�[�e����=�IS[��Ì���m�6��Y��m�JC��%�F�Zڽ�G�!\���ͯ����>Ch��CS�����R����z[R��/�ʾW�ZB��D�e(�E�̮��	��^��t�c>?�/k�y�"�*�����d�ϓU�W�%ߡ�撁�:(S}-�G�H�5�V?��@�5����#�j�����KQe��̏�8u���q��NL�/��/�d���y�R�B���a�K�`Q�ԓB��?�H�~�ߟ>��(zeI=G��*"�B�-�8�[�/�r�v0MOf�}\��J`�˞,� Ize��6c��0�5s�+@7����-�Ζ����- NM71K�}�^���&B�-����\Y�Q�9�G��Dqy�2��ip�ܭ�X���$Rt�$����\�/")z�C�֬�v@�VDV�s�k��3)'Ě�:9�k���9D������9B������B�I��c��>/gfƈrg/���)�,3y=�I���F�o�e����NM-H�Hf�ҌG���U���p���g�]D�,�� �Ygwg����4�h���f/���0sH#'f�q��:�S� �w�&q_Ħ�:D�ȏ���}f�J<���W���L�ḕҗװ8�������V���@��^?9f���R�yߑ��w�V��ŋ�w�.+���M���9�$4Y�v�ڋ&C��B��Bu:H��
D������Ma��"<O�ϩ��1�iJ4����k��z<2O�l��5��ŉ)J���[��~��5�/"�C^����Ϥ��@����zL&Op�����d���<hsQ�p�1eo�1�&3���7��?�{(	��]Ȣ����&<6�X�s�v��Ľ(���JG��K��,�QA�}��
��[�?Uf�,�Ge�� p�a_Ͼ�W�0�2�PFN�(z��${i�HoT2q�I����z�Y�"�A�<�mn�����G�{䍛鴾��R�9��쳶Å����4���(�[��d��e�W��ԣשJۉyPs���Q�l�y�[g�袯��i���W����^'л���h��!�M{�	ƦC)����������脤s%�����D�R4����x�:�j�sK\�D2Tf
����(>$�z�UF��ɕequBN�e"#A&_Yygg�uH�h-2����Fx�w��_-��Es�62�����@BQ�[��f$�A_s��6��۪~]�U���mc�
��>������;�8g��Ⱥ��o�pfр��bc\�D�،�vs���`͎��X�'k��ʯ�{�}&�S
�ni�:�F��L�T�bQv����I��&23P���g~�<ZI�Z?|��&����t���}��f�Io���W����M3�.����� ��{���/�a��^.����x#��E���A6���\+B~Z�ؗ���������*C^ޟu�*Y����Yi����d��7��Ʋ�E��>� Y�mI�x��k�Uē���)�Э0��R�|��#����\Z� ��!k�4��Sb�`��6��������h�g;  ��ٗ-.���x:-b�%ko?��Ë�PX��#���Sd��;	�_�K�� ���o1�q�G�J�u�B�{[7�ǉ��p�ڬ���a@��d?�v��}���;۽�^�I��
q J��R��g��m�
�w�j�κǡ!����[�yU�i�@�`�N>�;o'Fɱ�~�]]����L�te6//\�m6���7�����7���F�Ő�T���U�0˄�j�M5��
�y�,8ѻ�bi��*>��D+��и#�_�a'ףIK��1vU��)#6�Fqߪ���QLq��xC�ہ�ҋ1jY��� �(�S��LLHX�k�D;�D�Hy����#Ϻ�Ą�^���7|�&R<3<���kH�F����^st�x8~G9��x{s3t#\��zBQ��U���4�L*CS��v�tnm�Usm��F��Fˁ���I�2�"����4�髶�ڟ��	<��H����!�JҹXR/Gd[��W��^\W���v��lz5&��EV�̺ќ�D�͞�y���x��se]�(��4�W7���hoZ'���1H�5J���C"��r|Mi_��υU�*������S'�
�}��ČFT='<SB���W�	�ϱc#��M�N:d+�+�VFi�[�� ��Ry-���8е�hg5{�4�`��yq�X�GS��Q��(E�:~��dTEL5�C�ُ�z���E��WR���z��e����3��<��>���.�T��g[�h1`�l�[oW�z�Y��U�h�1NF�fam�M���r�ߴ�D�X(=�E��<� ��*��P��i��`�R�=��I�fv�����Am���>С�X��s�.�cL$�ӻ*��K�̛J��@P�h�n �p�v���	s~	`��)IAˊ���]oڝ��¹h�WK����@TqrwfKj<�c��:[{��Q���)9��S�}D��㶧Ԕ��!�Ż	�;�a���O���C$��\�#٥#TU_v6���]��]-�Joy-�3�<��1b�Ioja���EԷY1��.�Z��S֝gY	�ܥ�F�e�m���]d(�-��`��:;� �w�N�҄J#y���C��9���(�4)t���я}�!���.[��[���#^782ӹE\�F9zs �s�os)��s[�>
���?[#?� ��(L<��sf����2�T����ʨ�^H�;���@
�����sU��={=�WV!<�yP	��ҡ�O��sn.xT)cŰ�T�L3����|�d8Ds������Y2�[]��m��>���SdY��K�<ƫk��ĵ`��=�,�doD�������)Tu�_�2`�h-_���KiA倕΁�I34ݬ�vR�{��'+�>���g����ib�L̠�4��	�C�:�XBG�󀻆^�
Ɩ��g
�\~��� �~�z3:�cQ2jw�R橯��M��}ecJ �jÒ�3����ݠ'���[{m�G���c��z��!bd�l�����X˼eE�0l� �����yh*�U��*e_h���:L�V��p�[�m3���^ ��NpD�H���<~�MX����]sL<����=��:�)��x1�L�G�R���P��zp���[�7G�zr_�h#����r?V_<�}�Hmc甁x���{����L��+���w���+I}zEC���ܿ������DXT��m���7Ag8ہ���q���F����8A�.������S&࿄��<��!q[���ڢy�V��CL(	?��x��->I���� ���[�z�o��c�1}6W���(�'4��a�B)Ja�K�]�I�I�oT�%8S`!�c �rS�~h���� t�9�t	8�]bZ�_;:�������)!�[/,������W� �1�u2�QO�Xh���m���Wj���\5m�-3A=_0���
7�PTS-������u�&f�Z��]2��x��-z=���% -�NG�4�v�l9p��Z�Kf�M<�G��#���wڋ�D�̮+�5+��ٍ	igY1Ŷ����� q,=�R?�C,Nt�q�
JJ�a.�ٟ�3�1���(���ׯ�خ���L��1�:�T��?~j��L"&�3����+�Sb9a"91��)�����#o��DV1�T{�����BN��J�[׌�N�"����t��0�	}ݓ�+�z��	d��8Ə���/��G�m����՛�d���6�\���34��wBf�#�R��M�ۥ����Fn1ud*|���T��Fm*��Մ�n�j����R:Ϭl�}�{ԫ˲�K〼�y�W��C	���"�΂����A*Ӯf~C=�X��}��W/���eE��O��_ ��i�97	`��W�F	�^̷U�Xn*)���h�C%��S-�m9�l����d^���TnclN��f��D�@F�����Y��5#�^��o+���2��Bv��4D�����	� =�>��
a�'�*f�P'+ZG�9	[��v%!c������]����'Ӵ�h�1�-�F�%E�S��)uJ�*���� (��H�\m��ʵ�M_^�@�9PV2+Ϸm�o��s�4w��W׊ŧg��� �6H1�1��I��i��xr��!���[�a�dB�Á��( `�ʼ[K���܄�z�D�z('�i&�.9�H`}���L��
Oi�ܓ1)�Yc�"�ᾖ��X�y��)=}ѹ"�f��TD�v7�6�Tvn),�m��k;�lgX����}�
���g\%+�5*d�p%}�+E��(�I;f�TK8��x�ωE��c+	�(;��Otѷ�����]�Ӡ¥Ɓ�Fì-�z�F���k��@���?l�4%q�r|j"cV�J!�Hl���;�A�Opo���RZ��F7(؇�bS%e�����tDn:�C�el���Q'��jM'��_kh>�4�)�&i�t�/���A�W˗�	|0!�����]X�S�m�@0�G�m�s�g���� �h٣6��%ҩ�ɝ�܋*K�z�����į_�!��-���`s s�b;��(\��P��H����*���T���`�
\�%p����T��SL��*{Cތy	8��\�l������W -q_x͕�?zf2N���#�ƿ2���@�~�����P��#1C��Ey!�~�Cd��{��m`�>ߎ��pﮅ��vI��<�b��.̞�y��Y�$-�y����@a�Zc]ٱ�h��όL	yz� ��y���c_�;1����/DJ�%L���@����Y`ZS,s+��-�c+�QrQaF��}��|�)�ڃ �i'�e���=���H��҆��6_J�u��xr�X��cQ�G��@H�*E�Z���u��o`�򱟄��X���5a��7P���-fC�x���y��C����UO9t���e�ΰ�8PUl�/�b'%۳�7$�ʼ;V���81]��m<�yA��#��)�6���F���]DJ���kHM��K�����Xˎ�,���e+F���g���ȟs�oE8u���I�a����[�Ҍ����|�xc6IV���F��u�̣K�|F����[���z߲�J�����3�A,N���p�	�C��Y{ů�&�4�f��5_(��V���������b�)�Ce�Ew�YB7����H%��� �ki������V�(?�ë���0��q5�oH�w��-b$E��A�v\ҿ����h�D9]�m�ř<�!�6�巜�g��TЦ�U�3E*7;mW�O�3ES�d�%?�ť�u�������A;*l��gӚj�������y��0�
tY�4oԻ_͇x�
棍Y�g�.F���匽J���Y�����QXQ��`@)`{�%���6`�q�sŮ
L>�� H��^/�K��!�xZ�eAU4�O�V�J�*���=ڇ�DBH���<�PÊz���F~�ml��*����	|��;����!m!��=�� 9��w�@��}���ͅ,�=�u�`�n�M���f�2|�]��d�pq������P*�Hnt��	�6���@���\�y����' �ጀ�Ў��Rce�G-o_yK%���S��C�Ǉi�b7얔)�IB�А�C �r���1'n���k��c����;\�ޮ}D�������`nE�9/�բ'�xo��'y��t#��T���2�a	O�X�{*v,�>ݞ�ș��n��	�T�&+��R������ח�x�/��az�2mT�"6�P|]kW�����1�֬�/�(gcz��l�?�Q��-��
£��$��{�>��(��+�pC{�{�)�;���-)�ŌU�j�@�����E�0�����A��R�k}�Y�!��o4wǀt6e�&�%��4�R[�.=O���)g��P\�Cy)�
��������dJ��әѲE)�;�,�r��C��� N���VYeB�~�4���_ԟ��=��<FԘGD��>r<4r�kr9�d��3�=
R%��䕫��_�O�;�5S���b}�E��(��^�}��%��(�/%�d�Z�%I^�����w^Wx�tfٽ������a��ò���.3= %_[���]���	'���8��5t$ޱ�i�h5(_��\��|�<��˄�(�;�	�.U��Zh�J�� ��ܼ��i��f鉓�T�Ŭ �Űq�0?�@�r3�noe>��k�K�j��8V.6�n)�]ߒ�j>�F&r���de�;�nFIr\E�t)^�T
�~�F�'!���8HK���������nWn�=�.��;>����_�q|�g���`�bte�WC�?Faۘ�X��/V���c�Hm]8%.#FBm�\=��E��a��pϏ�Q�S1S���ru�҉4�#P�7ޱ�,>8l�P)�PB	|8��6|��v�p3�e���?�.�3;x����DZ�h��Fia5F����r���h���옷Ҟ���~�����/�qrv�I���P�@�K=��U�W�����4�~PB�p�b���^I��*ķ:��GN5m����Y�>6���4�y���f��[XW��yU|����D�^�#����B�W�V��r�z9��L�)EB���o� F^���nP&Ut⣝;{zS|������~E�f����2�Ў��D^�.�T�(�d����#ZM�K�E4�*��{��d��Z��r�5�uxc7q�n�~[���Z|��vg$��U���&*̜�#`֑G0+X�)��:*:;+�@���������&JV\�)"6��m\� f�ˎ;�e�����ďĝKt�c���;� 4��C���*��%7i�?��B���6xm<��Ir�h{ke.e�t�5�͵k_���/߰4��ؼc���c;��CH-�U�������è�r^q�K·�k��`
�f�q({�4����%ʧd�o��39�7�.���n55���`c�´[n��u�7d��W�5�̧\ۃ�,����o��0a<�������L��p�L	�k�R�ӽa���|�񆗹�.Y��mbb,O�?�޼%mo��)O���~Ҽy��Yb@2����}��V�%5~�,ӆ]�'й1�������D��S�bH�/#�M�U8�1���_��qq�1�A�W/�x��w�_g�A��8��(I�EB뜍��V��tN�#�9��p~1�d#�6���ʟ[/�Aa��}KT���0��9y����/|$<m�	��c�\�M�A-F4߻28
4��'����ftQ �Ҝ��j|r�`pnۧ��b�ߊ8���ﾼl89`�q:�0h\�ᖡD�3��7]�|-�Z�9L���t9���_k�i��8	�$�U$v����0s/	��dHr`�?c:pPjH��M��\Y�]��F��� z74�>?���N	)�w.�)����+w���!_G�����à�����T4�ɚm4k���?��p7Í�xˑC�ߟ�1- ��q���(#
��8��k��B��{�[ݲn
r
��>w���n�S>�VUe�L;�vV���pA2�y�G��.�BXn��8��k-��$Q�&��� 3�4Ӑ��cq���$m8胑R6$���譚G(x[�k3�8��R�*�iįk��WC�����k~��h&��0����A���	tp:H{i����-�
{|I�<�����sy6���]'���� ��5��4vF�	�ý�7�)d�[�;�+�VniS�#(�uw���P��r*ر�# �c'L�0T���=��kʵ�߀"^:o"���]?_|h�m?���uL�o�~���IS!p�M#�]k��8�iN}� �\PT_Vڼ�~aٓv!?ʌ�j����r�`�<%x%.��[z{z�k��<G�K���!'�a�;y�/Bk�4$_avڿd��vAI;���=[�[���%���f�9�'[��4���F+/��A��j@�^2Vr$�H�������C��P(2X�;��������b9nc�>�yi1咖�A�=A�&�����b��8�M}xG�2y�{��dX��)�Mn�2u��X8�3Wq�`���"���Ax��[�����zֵ4<ǟ�_����.����1�(�1s�9�W��e�Ŏ#n.|c�^��g�ҵ	�V�ka7TDEk�~��a�le�+us0��������D<(�*�Z��Y_�����`=���ىT�������p+� ��o}����le�����!�R՛N�۽Ɠ�ejA)D�0�o�ˈ�*/˵d��f�}`��6�����C��T�M�I�'>�vg?�v�DQ����ւN��dk�S	�¼��<�c��`��d�t����2��M�>v�������$�s[M�d��h�
�/�J�)�؋9R/�,���M,E~�4A����*\����v�_.z]"WG5�C���Q5�S;�-��ۛ
�S�=��� ����:��>�8�J��6�I͕�b�7<_$"�|ԩ"�2�c^?��������d�>$��`}�3	J"���ȃG��#��GH���*��ɽ�k������Y؀��P:Z[�P��m�GǴ$��ba�kڵz�������kz����il�A~���GU��l����T�j�r��)�D(�1ns�\�bAjL�`'EW�n���|~p��A<�ɏ�ڞ�� �ˈ�Ɲ�2Y�ڨ�᪏HKl�6O�V}1���2��m�qN�y��+���n�Η�)�s2�� �F=짫�ҵ|�:$K���*$v�z�M�<��=����r@�b� ]���?N�,�=v���'�Q"Ǹ)<����!n9��%�u�� ���L�"ksb^��4��K��r);(��aCkF ��u�d�=�;�s��Cw��|sq|�mI�أ��P�[���Y��{^:��]�Z�>����B
��M_,ib�r��-y�:�ֹ���;i����6$9�&�c`s�K�^�q�}���:���m�t�����,;�HY�bhmi��QVxL�n�,(���|�̊��p��?�04�}�����������T-P?�Q:��7�W��G���㠘�2�}1p�2%B��=~&�5�/j�yHԤ^��g�������������L��al����d����Az�/�tDӚ�ɓ��J�m��0-.P�WM�X�a�V�p�r�P��*�����*C"�
8d�b�D/�H5Ӥ�i��_��dC߉�-mn�_�j؃���\T�I���FA��q�C1q�V�(>
*r��q)O�]ǜ��x��ѡ�C�����3�
��Eq��uB���|�<g��HJ����1=�ql�f(ݪ��I,�&��j����d��2^_L�	�qt�	d>B
��~<�ws��9�Z��qb\�9�y�}�x#�!���D5�
o���+W\~V�8����UO�|%W`M���89i�L5:����1j>l�ވ����.^�t8�X ��o�t�\Į�%v���L|�q�<<���7y3�)啁��0�	�����5Zm	���[LOd�H$��Qeh��*4�&-��tj�Esozt[v~��ѯH�)7Rx���I���z�Gg.PN�7���
`
o*�;&t�	$���d���Rd|_8ә4�K�F���O���t�k��g�c�-R�����6)���=�Ъ�~n;�)�w��_q�s� �Kf�G�a��ΗLS�Ȍ���JnnVtΨ:-�dO��G�B�֬DW�J�Wj�E	�ɾe�ЇdWѐ��D=�uebE�4����g���kޜ�ʳ}�p�k�-"�����S�ݝ��3F���c�w�D�T�x���-%��k�{n7���/'ET�kBra�� ��l��ޢԀ�L�*�v����T8R؝q�p�]�^Ϫ���ՂW�We@͕q�b�b��*84�Q� �@iy͙v��o��0d2��;f5���Uڙ�s��yy��^DC̒_a`���"�Q��V�u�Ά8�O �g��H��$է'ߐ���rh�W������l��Z#G=���1%'�HFsk�H:�.�I�k���\ޣ�]���]l����-YU�2�6��gZ(��x�*Xt>���X΄>�rߋ�Zؕ=��2�#Rzx;���E5v��H���e��^y�/E��:�7g�إ)�q�^ޯ��H�ˬ-�)�ԋW%�b�Ox߿������/O�@n�ˇ�ߠڕ��\9�8�?����y��C�b����p��^����5��.�����iY��|�	u�r����'���̿ߒ"�Tl�V�����&>~0��mZ���-J���X�é���#挧�[N��9���ð�Yڞ��UJ�n|B^�vZ{�ܸ��Ƒg��������`��I�Hv��f���$0�����u��ԝ3����[�k3N���m�bz@��g`�H��ZG��ڳ���p��>SW�o������©�d��03�C���?���,o�7���LE�yA��O �lS+�ԗ�G��;�)���ͥ	UnL�w�"9�A�/n�=/<�jP�2�eת�Y�'�j�=�U�l���P��|���2�b[
p|l/pΔ��K�i���$��E��#·F)�1���r���V�����־�t>7�c���]>h�����ĕn����.l�G*�cyHIW9Y���+(���$�
���$d_ �g� ���0i��<v��oQf���`�J͙�� �C�2�1�ZUO��]ߣb�9zy�j)& |3S]����̀\1 ;�)E@��)�7���d�+��E����= ��A��gp`���We��8.)�DMo�{���^�"��:��:r��ϳib�a{d-u�@��]A������EH�ڣ�&��s����9M�#�]�?���S� W�>;2ұT����J&?�y%y����@E��M<�W�e
�mE�j!����#op�'��h;�E����nrC��aTQ�(z�ٿ��R�z/�t-" �JLIcK~��h�aQ�i�Hm��n����p%IHQ���~@�z۽,�F��x��s��������<�]�1���	��;�K2��N��������h���o�n��6��EH�.'��)���X�Y���g��vj㳿3��h�!�)l�,OQv�N:�Jx�9e3~��Rq��_n��cL��č��m��}"?��D�IAJ�����[��,�[���N��g�jAW��~�@�a��6J#M���v��!����ht���t� �G�L�!�n����������;�O["[��#���Œ��X�t��/�"ΔC��{:{*��{��>��@"����7�W��ej�WI"���q�J�޵�����9��0c�!�:���٢�d��Fn7ɔ�FL��Wճo8���_�N�[g`�� �t2��IQwG�.�?��o��~���ㅿ�+Q/)$9��-?k;+(um1w��Evc��!�߼�����bC�U�,(�mR3-�98�JSc�C(����k�JZ�|�8�(b�˩G'�[F����2#��E�`t\�4\��x�zܕC������̻�������'��T��ؕ	�zX�Q�������%�(ûQ�s�5�i � p�$�T��?�}�5����	9���?煩w�.��n��#{�Q��/� p*��Hu2Z��ㆦLl�f���,��dԽ�������%~6xg������~~�cq��O.��L���h�����c@���3&����#O�_{߉�ߺ�ߥ{��Tg�o��9���V�:��yp]��7�@_T�T��rE���#��/�b���a��L��?�����P��F�X��cYpk���ȍ�j�U�~�GD>��Jb֡��O���T@�EM;��%��*��<��'eM�G�x-�i>s���ZJ����'v΃���T��=�˟��U����&�_G٭�E2%�፾�H�K��3a�:��Κy5��A� `�>:R�dJ.�I�˸Vɻ`�Èo��K���D�
|գ���L�H�ў�}�������ۏ j���C�k��-�*��"ʔ��m�����8yP�����!����C>��1U| (e)nVb�WhL�~E���Z_x[1�:bmmG�t/��۵.$��֜�ܓ�k���Ա�#��s�� ��u�|n��yq���|�˜}St8�>�$�2��W'
���>�&�]�95�Z^2�%m��b�uW�&�MP9�PWD����7Z�࿳���7V��0s�Rd��*"z���Xu��s��.^���zd%-)��8I�4**4DDs�nY~��F�&`:8X)��ȚZ�S��%Oe�	�FƜj&�����M_{8�nz�pX�Pv8A�
U��^�v�j��/�7|���8c).V������y,����l�oD�wnTR�G��9�Ah�����Eߠ���#�It�[ؗ��T�#A:7oI�����>�.L�$6(����hʵ��57��B�w}��J��<��;1�#�HE�l��3[��!ɇ�p1V���a6K��+R(�OL�y�%P,�Dz�J7����Y"���إ��fz�d�>�'���n���;��w�9�B&'���fy�HQ�1�Q�^:��O]մ\å��+"�k�QVb����W�𞣖NR��B��T��kI�w�W<����d~q2΅7����H�2��(E��y��d�vPT��:CQ`� �M��KTW	�#��L� c���/[䩟���[=�2 �YL�����0/$:�^�j4&Ei���]��6US�(c�3�ڿ�E?J>OEj�Ą��"���$]c�xϢE�x�O�v٪#��Y�T��Θ�K]C\��P+rq��.�]�o�2��D���D ���ڠ.��N@#��Mu�vY�!��t&��C����	N����ٵ�*�I�#\��-��YԨ���0K���[�#A��ڵb����K���K7F�]ǖ(�R0L��&{�S/o\��H�񓚒Ov������w9��9;
����R���	�n ��n�8��^r���\Fj�j�e��|8����n�2�8ܣĶȰ�r�3f%/�7"�;]\�o�6���1�N#����PV�������4e��I�����t4;`��S%� o�c9��|�/�t"<��{7q�[`�X�q grD ��z�,��U�&�6u$��z]�:gZ��&�٤���h40w��p��I�3 ���Fc BÎ�5ŜxE
f�f�gku��>ПY}~Щ`��V?n\���WN�F�"�6��^�� /t����S �I�T��4��N�8�p#�?>_Z�ce�r�؎�kk�-6�_lK>إo���I�kgp�����:�S���]JcE/+�6���sao��(q���G��tܢ�M���[!�S�g����v�X����J�!�ᖏ��0C��+��ꬪ�9��DV�
8 �-�[��\؛�c
-Z�{�?C{�L�B�s�'EqCK*�\���x���^~�Gp��La��J;RqsiF���ힳF��ȯ�t��I��1���L���+3X�m�'3|����}�|�K����LHs������{�O5���ʑ{�>iXD�3�}'�Yb�׺�ݺa'��*<�ݠ���).^�������}�7WIN���
HN�ٿ%)LM�g�����1b߾��؊&���;���aa����Io�y����1u��8�t�Y�z���y�Ru���
�%��#ag���F%���h�sO�|�
��I瑯�.*�ϭ4��y��ꎫN�>�>�KN��
t4h����-�4l�}QT�S	(��_}�y�G7����QƱ�28��X��%.��d_%��qgV���hb~�Р�y2�nD<^t��͎�f`���o��$ʹr��i
�l�����0��rZUN�γ9ȷS��j±_����l��җ�̧��lt�����z��X�n���u�q�������W���ڱG��ōD�l ����[�Z��6r��%2-�����}��6���"1����p�𿹟g�t��_�\>*���w�l����w@u[L�>������oyz��b+��T���"����q� ._�bS��!�,-�ބ%���_F�m*��(������Ô�JҺ�|`m��	I �71l���`w�@+XK�M���N�M�F��Γ�VJb�=����rR;��5͞]_%�"��@ߡ��T5���g��Л�ՈψjzP�߀�W���\�K����EfA�(�$���j�Ձ�: 4,J���]=�iuo�s
Ћ[�]���T����X�wV�HK�4B��k�/��v��7 ,k��`ZWl\�Df/����Ӆ�|�H�QVf�z
+�� �9�����}g�#������'���OW�"ɼpz��i&�8�_]�����WR@��t��0������n��hO4~!�0�y�z�?B�dt�z �cr_]=����	��裍v�nS���§��Z L]E�Ɏ3F�w(���8A��E���8���Iapp|�j�wT��j���2/������QT��V��Uv��q]'v~M�S��E(�ܼ	����]�!�  �M]��a�"kh��,w����1�c gv�/8�9<gKx�.�fAUk�'���@��(���F�E=��w�xbK��`H������x��'��9��ۿ���6��L��If2��A�o�7gN��\�m� �N���*R�Kjx��Q�}�\�� l��)˓UZwC��{�P웷��#q�ZӕC�^�g��)�SL���1��"2�ˤܘ�#RD�X7���(��X.�Ҳ�0�
�{L��f�#�A��g��2MI�5��۵��v�t�^�˞�ks�Q]��X9�����傊��#g>�V��$���������L��� ������*�J&F���mZ���A�m����n�ن��2}��6ʇ�#�p��Й�B9�2}ו씭~��Jh�n}�4}��m�y*��m����}x�m������\�/����1@y���5i7t��G�	�s/�!��6o�	 �c���Hy���0-1�gJ8wò�����9fA�}2��B�m��q�T��wt��%3������&vKs���u]��xX^������(���I4�47�����{�E ְ9�q�LVX�^�h����J���.6Bo�TE���L8��U��u@p4M7��ײZ�Y�zl�F�N9=�U���C����p3�����$�@x�pʔ�TY��z[����5641�bi�Y9K�/!�Q�P.7(qnl�D��>�+����t��}���k�;�Yy9��#$���������X|-�-]\`&ʧQr<�7j<B����TMv��zJ\���f<d�\���� b�Elmw��r
���g��O�y�U/H��\
�J	�#�)ŀ��a���b�=9����ёG�i%��B6�^��YuIw-��-��7[R��PɎ���Omڭ,П������ѝQ��yo�s���ft�R1|ԙMCg8�Y���7��k�l�F��Q,0-��t_AOt�xu��iIU��Xwp��y�2�ӻ��dTo������C��a��)J�{\�<o��V
6�����u��×m�Y5�<̴s2v[b�d��9"�yt�=�5̡�Ԍ�=��|vp��v��ʷ�ؗ���wP(<�tڛ���E�(S(�:�Z$k�`�q�ms�t`�C��y:�$������>�%|������������ǿ)&�ٻ�:U [���H~`H��;
l�]��	"_K�2#�	A��o�<���lG#�}>eU�o�U����0����K����E�S;���1�P��z��B��E8����ېϬ�^�O'S�M�inQ�CxI�Dٳ�v"��S�&3
4,r%"Y_�k�pǕ�+���Xs���i�_[Z���8i�=]��s��Yy���k4^Z`Ǎ`��K�����Y�ٌ,��'v�3�:m|�Rn��

���a��I�
�6jQ(#)���T\�	{�}ҐѰ�����?~��k�L��Ii@iaq�?�M��VN-"���\ړ�<BY�l�C p��N7�M���X���l���O�M�E�.�-Ft���%6m����΀wX:.kį�J�N�o��h�08���$�z^ ��Q��]���w����br�~J�=mF�0��L@*S��-� k-Z�.���ԭq�e^��x��,^�B��e��p��x0�����Wa�$�X`������f<@6W§B��q$��	����-�ѳ��&"5WE����s j~Ą�_;�5�d���I�9Y�
�P�8�]"�R$*o�'m��H`��/�����?�X�S��~�pKҴt�.��?�&���mv�x�.<y���ᮓVݍ�Bm"e�2�1@v�����4Hb�wt��[���Lö�UO89.fcKSeፏw����ua�vM���q�ԍ��<
@<�#��B�y�!1�<�A��2�B����D\$N��Q䆩<�B�2 r��.�$�B�����E��ić�Fn$�/�ڣzA�+&šCk�z;��sm���#�Q��-�w	�F\&~�}$-}���
���A��R7O+hU��W�֚>{����5�[hv�H�O�����Y�x���9S��ƨ)Ŝqh�O5 ��m�z2�� §�P�ߣ筸��'���!$�m�|g�IUKZ���J�����Q�ۆ4���@MN: �G���]�w\��Ȩ�%��)�:g�mLC�auL���K<)0�i�o�{�|��o��j-3i-�K�u������&l��+��%����r���\˔=�#E�!�o�8�{Ɛ��]��w���9�����~��֯4��=ӓlJ� ����8x�1�L�����e�)������#>� �SVH�o ��p�+����c�L�t�_�!��v��v
�+*l]M�8.A�����[223[��r�3Ì�/|��<llaP߂ߢ�QXZ�$��4���쫩轏��������k���V��p-?�g��MAx�?�=F�>���b��ъ����#='o^������v�g*Y�ʀ'Ā��O�!\�l�6��B$0�J�X��	�;�~��~ǭ^S���s�a��(�.���qB,�Q���5OD;�T��6�Z;���O"�"�[���0�S�ስ�]Y�1����5(��E�7��I� ��'�ɝ>�}.R\�Y���n��[d��F������602+t���#��I�;��[��`p��qf��!k<��(�ϯ%/~:D�63���	�ʨ��6oq�d7��|_m��~�IkL�r�ѳgB3���oA^��� $�?�R���0�fZ.x`mfl���@��.Y=�\-���v6��g���e
���N}R�Z��7�j� w��>�\5ŽR��h����KXe����H��|4���;"��BI��E����FG��d���wkHtV�SNݍ�;6�<��3czC��J$�u�fL:�|Z���#�W^����y;V�D�f�<�7���lx��l$(мO�s���a�vb^�1gr�I�l��ޤ�#Օ�0�D��*�~��6�;UZZy��M�?4��.H�a"g�/O@��MhD��-	�k�dme]L�7j�'5�s�}��_�� � ��P��n��7�dw�>���%+@�H��pQ��5C�]��v�o��+<C��<FBB	?c���9%a��7�"�X�#t�$b*�VxUsc�Y�H�=���YS84�$�H��;h����.�LW�5�F/aT��˾5���-�����\OtD���k.&ұw��<�Ѕ�����ĉ����l��e��Yي@�.�Ӆ����4�b+K/�ډT��5e�3�GM@b��k}�At.��P�>��Ͽ����+1?�!�lPz�w�8�Vt���a���j߾hߢ�������47�o�c:Zi����ٷ�p	�Z�ũ"�|Y}�t��1�A@��!�R՘1 CV}�Xҙ�!f���E@^�,K$Bɬ{��w��^Etbr'����sG5�(]2[4���\��F�
ֻ`��^��߄ʽ�h�����%�(͎vO���V��fl� ?�����4���*�2�:�5���M��9�x!L(	H7l�u���3WrJX��'�X�Ko	œ	9�J�L��TL���[��>��sc���ӡ�q��UC$��$M���wn�d�Wu����ǰsq��.c�Ŕ涥��?����D�{Ay_�>�B��Έ�{��~�2���Z��1s�-y�q�L���]W\��c��h
*�CԢ�W�%����,q�#6�o�{�UXX��V_tSٵ`�1s����-�*���^�
������óykg���76��ݓ�ă]��9�X�̮�Y$�.*��_��[��-�EY3�"k���ʰ�������:�����4����ujSҒ�� �4���=��̬MNx�'�