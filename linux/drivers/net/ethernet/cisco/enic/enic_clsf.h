fault_pm	  = 0,
				.dvb_amplitude	  = 134,
				.set_smoothedcvbs = 1,
				.if_khz		  = 4560
			};

			fe = dvb_attach(xc4000_attach, fe0->dvb.frontend,
					&dev->i2c_bus[1].i2c_adap, &cfg);
			if (!fe) {
				pr_err("%s/2: xc4000 attach failed\n",
				       dev->name);
				goto frontend_detach;
			}
		}
		break;
	case CX23885_BOARD_TBS_6920:
		i2c_bus = &dev->i2c_bus[1];

		fe0->dvb.frontend = dvb_attach(cx24116_attach,
					&tbs_cx24116_config,
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL)
			fe0->dvb.frontend->ops.set_voltage = f300_set_voltage;

		break;
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		i2c_bus = &dev->i2c_bus[1];

		switch (port->nr) {
		/* PORT B */
		case 1:
			fe0->dvb.frontend = dvb_attach(cx24117_attach,
					&tbs_cx24117_config,
					&i2c_bus->i2c_adap);
			break;
		/* PORT C */
		case 2:
			fe0->dvb.frontend = dvb_attach(cx24117_attach,
					&tbs_cx24117_config,
					&i2c_bus->i2c_adap);
			break;
		}
		break;
	case CX23885_BOARD_TEVII_S470:
		i2c_bus = &dev->i2c_bus[1];

		fe0->dvb.frontend = dvb_attach(ds3000_attach,
					&tevii_ds3000_config,
					&i2c_