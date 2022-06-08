ET_SEQ_END:
		return  "GET_SEQ_END";
	case CX2341X_ENC_SET_PGM_INDEX_INFO:
		return  "SET_PGM_INDEX_INFO";
	case CX2341X_ENC_SET_VBI_CONFIG:
		return  "SET_VBI_CONFIG";
	case CX2341X_ENC_SET_DMA_BLOCK_SIZE:
		return  "SET_DMA_BLOCK_SIZE";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_10:
		return  "GET_PREV_DMA_INFO_MB_10";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_9:
		return  "GET_PREV_DMA_INFO_MB_9";
	case CX2341X_ENC_SCHED_DMA_TO_HOST:
		return  "SCHED_DMA_TO_HOST";
	case CX2341X_ENC_INITIALIZE_INPUT:
		return  "INITIALIZE_INPUT";
	case CX2341X_ENC_SET_FRAME_DROP_RATE:
		return  "SET_FRAME_DROP_RATE";
	case CX2341X_ENC_PAUSE_ENCODER:
		return  "PAUSE_ENCODER";
	case CX2341X_ENC_REFRESH_INPUT:
		return  "REFRESH_INPUT";
	case CX2341X_ENC_SET_COPYRIGHT:
		return  "SET_COPYRIGHT";
	case CX2341X_ENC_SET_EVENT_NOTIFICATION:
		return  "SET_EVENT_NOTIFICATION";
	case CX2341X_ENC_SET_NUM_VSYNC_LINES:
		return  "SET_NUM_VSYNC_LINES";
	case CX2341X_ENC_SET_PLACEHOLDER:
		return  "SET_PLACEHOLDER";
	case CX2341X_ENC_MUTE_VIDEO:
		return  "MUTE_VIDEO";
	case CX2341X_ENC_MUTE_AUDIO:
		return  "MUTE_AUDIO";
	case CX2341X_ENC_MISC:
		return  "MISC";
	default:
		return "UNKNOWN";
	}
}

static int cx23885_mbox_func(void *priv,
			     u32 command,
			     int in,
			     int out,
			     u32 data[CX2341X_MBOX_MAX_DATA])
{
	struct cx23885_dev *dev = priv;
	unsigned long timeout;
	u32 value, flag, retval = 0;
	int i;

	dprintk(3, "%s: command(0x%X) = %s\n", __func__, command,
		cmd_to_str(command));

	/* this may not be 100% safe if we can't read any memory location
	   without side effects */
	mc417_memory_read(dev, dev->cx23417_mailbox - 4, &value);
	if (value != 0x12345678) {
		pr_err("Firmware and/or mailbox pointer not initialized or corrupted, signature = 0x%x, cmd = %s\n",
			value, cmd_to_str(command));
		return -1;
	}

	/* This read looks at 32 bits, but flag is only 8 bits.
	 * Seems we also bail if CMD or TIMEOUT bytes are set???
	 */
	mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
	if (flag) {
		pr_err("ERROR: Mailbox appears to be in use (%x), cmd = %s\n",
		       flag, cmd_to_str(command));
		return -1;
	}

	flag |= 1; /* tell 'em we're working on it */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* write command + args + fill remaining with zeros */
	/* command code */
	mc417_memory_write(dev, dev->cx23417_mailbox + 1, command);
	mc417_memory_write(dev, dev->cx23417_mailbox + 3,
		IVTV_API_STD_TIMEOUT); /* timeout */
	for (i = 0; i < in; i++) {
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, data[i]);
		dprintk(3, "API Input %d = %d\n", i, data[i]);
	}
	for (; i < CX2341X_MBOX_MAX_DATA; i++)
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, 0);

	flag |= 3; /* tell 'em we're done writing */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* wait for firmware to handle the API command */
	timeout = jiffies + msecs_to_jiffies(10);
	for (;;) {
		mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
		if (0 != (flag & 4))
			break;
		if (time_after(jiffies, timeout)) {
			pr_err("ERROR: API Mailbox timeout\n");
			return -1;
		}
		udelay(10);
	}

	/* read output values */
	for (i = 0; i < out; i++) {
		mc417_memory_read(dev, dev->cx23417_mailbox + 4 + i, data + i);
		dprintk(3, "API Output %d = %d\n", i, data[i]);
	}

	mc417_memory_read(dev, dev->cx23417_mailbox + 2, &retval);
	dprintk(3, "API result = %d\n", retval);

	flag = 0;
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	return retval;
}

/* We don't need to call the API often, so using just one
 * mailbox will probably suffice
 */
static int cx23885_api_cmd(struct cx23885_dev *dev,
			   u32 command,
			   u32 inputcnt,
			   u32 outputcnt,
			   ...)
{
	u32 data[CX2341X_MBOX_MAX_DATA];
	va_list vargs;
	int i, err;

	dprintk(3, "%s() cmds = 0x%08x\n", __func__, command);

	va_start(vargs, outputcnt);
	for (i = 0; i < inputcnt; i++)
		data[i] = va_arg(vargs, int);

	err = cx23885_mbox_func(dev, command, inputcnt, outputcnt, data);
	for (i = 0; i < outputcnt; i++) {
		int *vptr = va_arg(vargs, int *);
		*vptr = data[i];
	}
	va_end(vargs);

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
	pr_err("Mailbox signature values not found!\n");
	return -1;
}

static int cx23885_load_firmware(struct cx23885_dev *dev)
{
	static const unsigned char magic[8] = {
		0xa7, 0x0d, 0x00, 0x00, 0x66, 0xbb, 0x55, 0xaa
	};
	const struct firmware *firmware;
	int i, retval = 0;
	u32 value = 0;
	u32 gpio_output = 0;
	u32 gpio_value;
	u32 checksum = 0;
	u32 *dataptr;

	dprintk(2, "%s()\n", __func__);

	/* Save GPIO settings before reset of APU */
	retval |= mc417_memory_read(dev, 0x9020, &gpio_output);
	retval |= mc417_memory_read(dev, 0x900C, &gpio_value);

	retval  = mc417_register_write(dev,
		IVTV_REG_VPU, 0xFFFFFFED);
	retval |= mc417_register_write(dev,
		IVTV_REG_HW_BLOCKS, IVTV_CMD_HW_BLOCKS_RST);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_REFRESH, 0x80000800);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_PRECHARGE, 0x1A);
	retval |= mc417_register_write(dev,
		IVTV_REG_APU, 0);

	if (retval != 0) {
		pr_err("%s: Error with mc417_register_write\n",
			__func__);
		return -1;
	}

	retval = request_firmware(&firmware, CX23885_FIRM_IMAGE_NAME,
				  &dev->pci->dev);

	if (retval != 0) {
		pr_err("ERROR: Hotplug firmware request failed (%s).\n",
		       CX23885_FIRM_IMAGE_NAME);
		pr_err("Please fix your hotplug setup, the board will not work without firmware loaded!\n");
		return -1;
	}

	if (firmware->size != CX23885_FIRM_IMAGE_SIZE) {
		pr_err("ERROR: Firmware size mismatch (have %zu, expected %d)\n",
		       firmware->size, CX23885_FIRM_IMAGE_SIZE);
		release_firmware(firmware);
		return -1;
	}

	if (0 != memcmp(firmware->data, magic, 8)) {
		pr_err("ERROR: Firmware magic 