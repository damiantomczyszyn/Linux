ity *cap)
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

	return