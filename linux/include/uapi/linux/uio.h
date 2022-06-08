         = &mpeg_fops,
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
	snprintf(vfd->name, si