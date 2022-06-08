gister(port);
	if (err != 0)
		pr_err("%s() dvb_register failed err = %d\n",
		       __func__, err);

	return err;
}

int cx23885_dvb_unregister(struct cx23885_tsport *port)
{
	struct vb2_dvb_frontend *fe0;
	struct i2c_client *client;

	fe0 = vb2_dvb_get_frontend(&port->frontends, 1);

	if (fe0 && fe0->dvb.frontend)
		vb2_dvb_unregister_bus(&port->frontends);

	/* remove I2C client for CI */
	client = port->i2c_client_ci;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	/* remove I2C client for SEC */
	client = port->i2c_client_sec;
	i