evision(struct cx23885_dev *dev)
{
	switch (cx_read(RDR_CFG2) & 0xff) {
	case 0x00:
		/* cx