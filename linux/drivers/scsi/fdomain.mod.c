

	return bus->i2c_rc;
}

int cx23885_i2c_unregister(struct cx23885_i2c *bus)
{
	i2c_del_adapter(&bus->i2c_adap);
	return 0;
}

void cx23885_av_clk(struct cx23885_dev *dev, int enable)
{
	/* write 0 to bus 2 addr 0x144 via i2x_xfer() */
	char buffer[3];
	struct i2c_msg msg;
	dprintk(1, "%s(enabled = %d)\n", __func__, enable);

	/* Register 0x144 */
	buffer[0] = 0x01;
	buffer[1] = 0x44;
	if (enable == 1)
		buffer[2] = 0x05;
	else
		buffer[2] = 0x00;

	msg.addr = 0x44;
	msg.flags = I2C_M_TEN;
	msg.len = 3;
	msg.buf = buffe