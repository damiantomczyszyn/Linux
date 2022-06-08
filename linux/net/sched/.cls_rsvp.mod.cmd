ine void cx23885_irq_disable_all(struct cx23885_dev *dev)
{
	cx23885_irq_disable(dev, 0xffffff