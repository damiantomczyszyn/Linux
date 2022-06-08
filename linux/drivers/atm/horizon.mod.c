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
		dev->video_dev = NULL