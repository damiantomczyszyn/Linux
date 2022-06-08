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

static void cx23885_