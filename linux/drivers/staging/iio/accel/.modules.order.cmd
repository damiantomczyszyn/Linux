

	return bus->i2c_rc;
}

int cx23885_i2c_unregister(struct cx23885_i2c *bus)
{
	i2c_del_adapter(&bus->i2c_adap);
	re