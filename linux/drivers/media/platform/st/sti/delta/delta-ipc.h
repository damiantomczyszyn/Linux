ntf(vfd->name, sizeof(vfd->name), "%s (%s)",
		 cx23885_boards[dev->board].name, type);
	video_set_drvdata(vfd, dev);
	return vfd;
}

int cx23885_flatiron_write(struct cx23885_dev *dev, u8 reg, u8 data)
{
	/* 8 bit registers, 8 bit values */
	u8 buf[] = { reg, data };

	struct i2c_msg msg = { .addr = 0x98 >> 1,
		.flags = 0, .buf = buf, .len = 2 };

	return i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}

u8 cx23885_flatiron_read(struct cx23885_dev *dev, u8 reg)
{
	/* 8 bit registers, 8 bit values */
	int ret;
	u8 b0[] = { reg };
	u8 b1[] = { 0 };

	struct i2c_msg msg[] = {
		{ .addr = 0x98 >> 1, .flags = 0, .buf = b0, .len = 1 },
		{ .addr = 0x98 >> 1, .flags = I2C_M_RD, .buf = b1, .len = 1 }
	};

	ret = i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg[0], 2);
	if (ret != 2)
		pr_err("%s() error\n", __func__);

	return b1[0];
}

static void cx23885_flatiron_dump(struct cx23885_dev *dev)
{
	int i;
	dprintk(1, "Flatiron dump\n");
	for (i = 0; i < 0x24; i++) {
		dprintk(1, "FI[%02x] = %02x\n", i,
			cx23885_flatiron_read(dev, i));
	}
}

static int cx23885_flatiron_mux(struct cx23885_dev *dev, int input)
{
	u8 val;
	dprintk(1, "%s(input = %d)\n", __func__, input);

	if (input == 1)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) & ~FLD_CH_SEL;
	else if (input == 2)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) | FLD_CH_SEL;
	else
		return -EINVAL;

	val |= 0x20; /* Enable clock to delta-sigma and dec filter */

	cx23885_flatiron_write(dev, CH_PWR_CTRL1, val);

	/* Wake up */
	cx23885_flatiron_write(dev, CH_PWR_CTRL2, 0);

	if (video_debug)
		cx23885_flatiron_dump(dev);

	return 0;
}

static int cx23885_video_mux(struct cx23885_dev *dev, unsigned int input)
{
	dprintk(1, "%s() video_mux: %d [vmux=%d, gpio=0x%x,0x%x,0x%x,0x%x]\n",
		__func__,
		input, INPUT(input)->vmux,
		INPUT(input)->gpio0, INPUT(input)->gpio1,
		INPUT(input)->gpio2, INPUT(input)->gpio3);
	dev->input = input;

	if (dev->board == CX23885_BOARD_MYGICA_X8506 ||
		dev->board == CX23885_BOARD_MAGICPRO_PROHDTVE2 ||
		dev->board == CX23885_BOARD_MYGICA_X8507) {
		/* Select Analog TV */
		if (INPUT(input)->type == CX23885_VMUX_TELEVISION)
			cx23885_gpio_clear(dev, GPIO_0);
	}

	/* Tell the internal A/V decoder */
	v4l2_subdev_call(dev->sd_cx25840, video, s_routing,
			INPUT(input)->vmux, 0, 0);

	if ((dev->board == CX23885_BOARD_HAUPPAUGE_HVR1800) ||
		(dev->board == CX23885_BOARD_MPX885) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1250) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_IMPACTVCBE) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255_22111) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1265_K4) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1850) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR5525) ||
		(dev->board == CX23885