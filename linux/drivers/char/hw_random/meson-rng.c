2_FIELD_SEQ_BT:
		break;
	default:
		field = V4L2_FIELD_INTERLACED;
		break;
	}

	f->fmt.pix.field = field;
	v4l_bound_align_image(&f->fmt.pix.width, 48, maxw, 2,
			      &f->fmt.pix.height, 32, maxh, 0, 0);
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct v4l2_subdev_format format = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
	};
	int err;

	dprintk(2, "%s()\n", __func__);
	err = vidioc_try_fmt_vid_cap(file, priv, f);

	if (0 != err)
		return err;

	if (vb2_is_busy(&dev->vb2_vidq) || vb2_is_busy(&dev->vb2_vbiq) ||
	    vb2_is_busy(&dev->vb2_mpegq))
		return -EBUSY;

	dev->fmt        = format_by_fourcc(f->fmt.pix.pixelformat);
	dev->width      = f->fmt.pix.width;
	dev->height     = f->fmt.pix.height;
	dev->field	= f->fmt.pix.field;
	dprintk(2, "%s() width=%d height=%d field=%d\n", __func__,
		dev->width, dev->height, dev->field);
	v4l2_fill_mbus_format(&format.format, &f->fmt.pix, MEDIA_BUS_FMT_FIXED);
	call_all(dev, pad, set_fmt, NULL, &format);
	v4l2_fill_pix_format(&f->fmt.pix, &format.format);
	/* set_fmt overwrites f->fmt.pix.field, restore it */
	f->fmt.pix.field = dev->field;
	return 0;
}

static int vidioc_querycap(struct file *file, void  *priv,
	struct v4l2_capability *cap)
{
	struct cx23885_dev *dev = video_drvdata(file);

	strscpy(cap->driver, "cx23885", sizeof(cap->driver));
	strscpy(cap->card, cx23885_boards[dev->board].name,
		sizeof(cap->card));
	sprintf(cap->bus_info, "PCIe:%s", pci_name(dev->pci));
	cap->capabilities = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
			    V4L2_CAP_AUDIO | V4L2_CAP_VBI_CAPTURE |
			    V4L2_CAP_VIDEO_CAPTURE |
			    V4L2_CAP_DEVICE_CAPS;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		cap->capabilities |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			cap->capabilities |= V4L2_CAP_TUNER;
		break;
	}
	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void