args);

	return err;
}

static int cx23885_api_func(void *priv, u32 cmd, int in, int out, u32 data[CX2341X_MBOX_MAX_DATA])
{
	return cx23885_mbox_func(priv, cmd, in, out, data);
}

static int cx23885_find_mailbox(struct cx23885_dev *dev)
{
	u32 signature[4] = {
		0x12345678, 0x34567812, 0x56781234, 0x78123456
	};
	int signaturecnt = 0;
	u32 value;
	int i;

	dprintk(2, "%s()\n", __func__);

	for (i = 0; i < CX23885_FIRM_IMAGE_SIZE; i++) {
		mc417_memory_read(dev, i, &value);
		if (value == signature[signaturecnt])
			signaturecnt++;
		else
			signaturecnt = 0;
		if (4 == signaturecnt) {
			dprintk(1, "Mailbox signature found at 0x%x\n", i+1);
			return i+1;
		}
	}
	pr_err(