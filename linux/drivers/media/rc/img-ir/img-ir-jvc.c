ient_tuner);
				module_put(client_demod->dev.driver->owner);
				i2c_unregister_device(client_demod);
				port->i2c_client_demod = NULL;
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;
			break;
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885:
		pr_info("%s(): board=%d port=%d\n", __func__,
			dev->board, port->nr);
		switch (port->nr) {
		/* port b - Terrestrial/cable */
		case 1:
			/* attach frontend */
			i2c_bus = &dev->i2c_bus[0];
			fe0->dvb.frontend = dvb_attach(lgdt3306a_attach,
				&hauppauge_quadHD_ATSC_a_config, &i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;

			/* attach tuner */
			memset(&si2157_config, 0, sizeof(si2157_config));
			si2157_config.fe = fe0->dvb.frontend;
			si2157_config.if_port = 1;
			si2157_config.inversion = 1;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2157", I2C_NAME_SIZE);
			info.addr = 0x60;
			info.platform_data = &si2157_config;
			request_module("%s", info.type);
			client_tuner = i2c_new_client_device(&dev->i2c_bus[1].i2c_adap, &info);
			if (!i2c_client_has_driver(client_tuner)) {
				module_put(client_demod->dev.driver->owner);
				i2c_unregister_device(client_demod);
				port->i2c_client_demod = NULL;
				goto frontend_detach;
			}
			if (!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				module_put(client_demod->dev.driver->owner);
				i2c_unregister_device(client_demod);
				port->i2c_client_demod = NULL;
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;

			/* we only attach tuner for analog on the 888 version */
			if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC) {
				pr_info("%s(): QUADHD_ATSC analog setup\n",
					__func__);
				dev->ts1.analog_fe.tuner_priv = client_tuner;
				memcpy(&dev->ts1.a