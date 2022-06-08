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
	.vidioc_