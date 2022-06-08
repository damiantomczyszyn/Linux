
	}

	for (cnt = 0; cnt < msg->len; cnt++) {

		ctrl = bus->i2c_period | (1 << 12) | (1 << 2) | 1;

		if (cnt < msg->len - 1)
			ctrl |= I2C_NOSTOP | I2C_EXTEND;

		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, ctrl);

		if (!i2c_wait_done(i2c_adap))
			goto eio;
		msg->buf[cnt] = cx_read(bus->reg_rdata) & 0xff;
		if (i2c_debug) {
			dprintk(1, " %02x", msg->buf[cnt]);
			if (!(ctrl & I2C_NOSTOP))
				dprintk(1, " >\n");
		}
	}
	return msg->len;

 eio:
	retval = -EIO;
	if (i2c_debug)
		pr_err(" ERR: %d\n", retval);
	return retval;
}

static int i2c_xfer(struct i2c_adapter *i2c_ad