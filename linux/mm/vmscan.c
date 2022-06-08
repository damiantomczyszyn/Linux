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
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(ts2020_attach, fe0->dvb.frontend,
				&tevii_ts2020_config, &i2c_bus->i2c_adap);
			fe0->dvb.frontend->ops.set_voltage = f300_set_voltage;
		}

		break;
	case CX23885_BOARD_DVBWORLD_2005:
		i2c_bus = &dev->i2c_bus[1];

		fe0->dvb.frontend = dvb_attach(cx24116_attach,
			&dvbworld_cx24116_config,
			&i2c_bus->i2c_adap);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		i2c_bus = &dev->i2c_bus[0];
		switch (port->nr) {
		/* port B */
		case 1:
			fe0->dvb.frontend = dvb_attach(stv0900_attach,
							&netup_stv0900_config,
							&i2c_bus->i2c_adap, 0);
			if (fe0->dvb.frontend != NULL) {
				if (dvb_attach(stv6110_attach,
						fe0->dvb.frontend,
						&netup_stv6110_tunerconfig_a,
						&i2c_bus->i2c_adap)) {
					if (!dvb_attach(lnbh24_attach,
							fe0->dvb.frontend,
							&i2c_bus->i2c_adap,
							LNBH24_PCL | LNBH24_TTX,
							LNBH24_TEN, 0x09))
						pr_err("No LNBH24 found!\n");

				}
			}
			break;
		/* port C */
		case 2:
			fe0->dvb.frontend = dvb_attach(stv0900_attach,
							&netup_stv0900_config,
							&i2c_bus->i2c_adap, 1);
			if (fe0->dvb.frontend != NULL) {
				if (dvb_attach(stv6110_attach,
						fe0->dvb.frontend,
						&netup_stv6110_tunerconfig_b,
						&i2c_bus->i2c_adap)) {
					if (!dvb_attach(lnbh24_attach,
							fe0->dvb.frontend,
							&i2c_bus->i2c_adap,
							LNBH24_PCL | LNBH24_TTX,
							LNBH24_TEN, 0x0a))
						pr_err("No LNBH24 found!\n");

				}
			}
			break;
		}
		break;
	case CX23885_BOARD_MYGICA_X8506:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(lgs8gxx_attach,
			&mygica_x8506_lgs8gl5_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(xc5000_attach, fe0->dvb.frontend,
			   &i2c_bus2->i2c_adap, &mygica_x8506_xc5000_config);
		cx23885_set_frontend_hook(port, fe0->dvb.frontend);
		break;
	case CX23885_BOARD_MYGICA_X8507:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(mb86a20s_attach,
			&mygica_x8507_mb86a20s_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;

		dvb_attach(xc5000_attach, fe0->dvb.frontend,
			   &i2c_bus2->i2c_adap,
			   &mygica_x8507_xc5000_config);
		cx23885_set_frontend_hook(port, fe0->dvb.frontend);
		break;
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(lgs8gxx_attach,
			&magicpro_prohdtve2_lgs8g75_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(xc5000_attach, fe0->dvb.frontend,
			   &i2c_bus2->i2c_adap,
			   &magicpro_prohdtve2_xc5000_config);
		cx23885_set_frontend_hook(port, fe0->dvb.frontend);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1411_attach,
			&hcw_s5h1411_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[0].i2c_adap,
			   &hauppauge_tda18271_config);

		tda18271_attach(&dev->ts1.analog_fe,
			0x60, &dev->i2c_bus[1].i2c_adap,
			&hauppauge_tda18271_config);

		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1411_attach,
			&hcw_s5h1411_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[0].i2c_adap,
			   &hauppauge_tda18271_config);
		break;
	case CX23885_BOARD_MYGICA_X8558PRO:
		switch (port->nr) {
		/* port B */
		case 1:
			i2c_bus = &dev->i2c_bus[0];
			fe0->dvb.frontend = dvb_attach(atbm8830_attach,
				&mygica_x8558pro_atbm8830_cfg1,
				&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			dvb_attach(max2165_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &mygic_x8558pro_max2165_cfg1);
			break;
		/* port C */
		case 2:
			i2c_bus = &dev->i2c_bus[1];
			fe0->dvb.frontend = dvb_attach(atbm8830_attach,
				&mygica_x8558pro_atbm8830_cfg2,
				&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			dvb_attach(max2165_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &mygic_x8558pro_max2165_cfg2);
		}
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		if (port->nr > 2)
			return 0;

		i2c_bus = &dev->i2c_bus[0];
		mfe_shared = 1;/* MFE */
		port->frontends.gate = 0;/* not clear for me yet */
		/* ports B, C */
		/* MFE frontend 1 DVB-T */
		fe0->dvb.frontend = dvb_attach(stv0367ter_attach,
					&netup_stv0367_config[port->nr - 1],
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		if (NULL == dvb_attach(xc5000_attach, fe0->dvb.frontend,
					&i2c_bus->i2c_adap,
					&netup_xc5000_config[port->nr - 1]))
			goto frontend_detach;
		/* load xc5000 firmware */
		fe0->dvb.frontend->ops.tuner_ops.init(fe0->dvb.frontend);

		/* MFE frontend 2 */
		fe1 = vb2_dvb_get_frontend(&port->frontends, 2);
		if (fe1 == NULL)
			goto frontend_detach;
		/* DVB-C init */
		fe1->dvb.frontend = dvb_attach(stv0367cab_attach,
					&netup_stv0367_config[port->nr - 1],
					&i2c_bus->i2c_adap);
		if (fe1->dvb.frontend == NULL)
			break;

		fe1->dvb.frontend->id = 1;
		if (NULL == dvb_attach(xc5000_attach,
				       fe1->dvb.frontend,
				       &i2c_bus->i2c_adap,
				       &netup_xc5000_config[port->nr - 1]))
			goto frontend_detach;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		switch (port->nr) {
		/* port b */
		case 1:
			fe0->dvb.frontend = dvb_attach(drxk_attach,
					&terratec_drxk_config[0],
					&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			if (!dvb_attach(mt2063_attach,
					fe0->dvb.frontend,
					&terratec_mt2063_config[0],
					&i2c_bus2->i2c_adap))
				goto frontend_detach;
			break;
		/* port c */
		case 2:
			fe0->dvb.frontend = dvb_attach(drxk_attach,
					&terratec_drxk_config[1],
					&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			if (!dvb_attach(mt2063_attach,
					fe0->dvb.frontend,
					&terratec_mt2063_config[1],
					&i2c_bus2->i2c_adap))
				goto frontend_detach;
			break;
		}
		break;
	case CX23885_BOARD_TEVII_S471:
		i2c_bus = &dev->i2c_bus[1];

		fe0->dvb.frontend = dvb_attach(ds3000_attach,
					&tevii_ds3000_config,
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(ts2020_attach, fe0->dvb.frontend,
			   &tevii_ts2020_config, &i2c_bus->i2c_adap);
		break;
	case CX23885_BOARD_PROF_8000:
		i2c_bus = &dev->i2c_bus[0];

		fe0->dvb.frontend = dvb_attach(stv090x_attach,
						&prof_8000_stv090x_config,
						&i2c_bus->i2c_adap,
						STV090x_DEMODULATOR_0);
		if (fe0->dvb.frontend == NULL)
			break;
		if (!dvb_attach(stb6100_attach,
				fe0->dvb.frontend,
				&prof_8000_stb6100_config,
				&i2c_bus->i2c_adap))
			goto frontend_detach;

		fe0->dvb.frontend->ops.set_voltage = p8000_set_voltage;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR4400: {
		struct tda10071_platform_data tda10071_pdata = hauppauge_tda10071_pdata;
		struct a8293_platform_data a8293_pdata = {};

		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		switch (port->nr) {
		/* port b */
		case 1:
			/* attach demod + tuner combo */
			memset(&info, 0, sizeof(info));
			strscpy(info.type, "tda10071_cx24118", I2C_NAME_SIZE);
			info.addr = 0x05;
			info.platform_data = &tda10071_pdata;
			request_module("tda10071");
			client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			fe0->dvb.frontend = tda10071_pdata.get_dvb_frontend(client_demod);
			port->i2c_client_demod = client_demod;

			/* attach SEC */
			a8293_pdata.dvb_frontend = fe0->dvb.frontend;
			memset(&info, 0, sizeof(info));
			strscpy(info.type, "a8293", I2C_NAME_SIZE);
			info.addr = 0x0b;
			info.platform_data = &a8293_pdata;
			request_module("a8293");
			client_sec = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_sec))
				goto frontend_detach;
			if (!try_module_get(client_sec->dev.driver->owner)) {
				i2c_unregister_device(client_sec);
				goto frontend_detach;
			}
			port->i2c_client_sec = client_sec;
			break;
		/* port c */
		case 2:
			/* attach frontend */
			memset(&si2165_pdata, 0, sizeof(si2165_pdata));
			si2165_pdata.fe = &fe0->dvb.frontend;
			si2165_pdata.chip_mode = SI2165_MODE_PLL_XTAL;
			si2165_pdata.ref_freq_hz = 16000000;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2165", I2C_NAME_SIZE);
			info.addr = 0x64;
			info.platform_data = &si2165_pdata;
			request_module(info.type);
			client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			port->i2c_client_demod = client_demod;

			if (fe0->dvb.frontend == NULL)
				break;
			fe0->dvb.frontend->ops.i2c_gate_ctrl = NULL;
			if (!dvb_attach(tda18271_attach,
					fe0->dvb.frontend,
					0x60, &i2c_bus2->i2c_adap,
				  &hauppauge_hvr4400_tuner_config))
				goto frontend_detach;
			break;
		}
		break;
	}
	case CX23885_BOARD_HAUPPAUGE_STARBURST: {
		struct tda10071_platform_data tda10071_pdata = hauppauge_tda10071_pdata;
		struct a8293_platform_data a8293_pdata = {};

		i2c_bus = &dev->i2c_bus[0];

		/* attach demod + tuner combo */
		memset(&info, 0, sizeof(info));
		strscpy(info.type, "tda10071_cx24118", I2C_NAME_SIZE);
		info.addr = 0x05;
		info.platform_data = &tda10071_pdata;
		request_module("tda10071");
		client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_demod))
			goto frontend_detach;
		if (!try_module_get(client_demod->dev.driver->owner)) {
			i2c_unregister_device(client_demod);
			goto frontend_detach;
		}
		fe0->dvb.frontend = tda10071_pdata.get_dvb_frontend(client_demod);
		port->i2c_client_demod = client_demod;

		/* attach SEC */
		a8293_pdata.dvb_frontend = fe0->dvb.frontend;
		memset(&info, 0, sizeof(info));
		strscpy(info.type, "a8293", I2C_NAME_SIZE);
		info.addr = 0x0b;
		info.platform_data = &a8293_pdata;
		request_module("a8293");
		client_sec = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_sec))
			goto frontend_detach;
		if (!try_module_get(client_sec->dev.driver->owner)) {
			i2c_unregister_device(client_sec);
			goto frontend_detach;
		}
		port->i2c_client_sec = client_sec;
		break;
	}
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_S950:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		switch (port->nr) {
		/* port b - satellite */
		case 1:
			/* attach frontend */
			fe0->dvb.frontend = dvb_attach(m88ds3103_attach,
					&dvbsky_t9580_m88ds3103_config,
					&i2c_bus2->i2c_adap, &adapter);
			if (fe0->dvb.frontend == NULL)
				break;

			/* attach tuner */
			memset(&ts2020_config, 0, sizeof(ts2020_config));
			ts2020_config.fe = fe0->dvb.frontend;
			ts2020_config.get_agc_pwm = m88ds3103_get_agc_pwm;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "ts2020", I2C_NAME_SIZE);
			info.addr = 0x60;
			info.platform_data = &ts2020_config;
			request_module(info.type);
			client_tuner = i2c_new_client_device(adapter, &info);
			if (!i2c_client_has_driver(client_tuner))
				goto frontend_detach;
			if (!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				goto frontend_detach;
			}

			/* delegate signal strength measurement to tuner */
			fe0->dvb.frontend->ops.read_signal_strength =
				fe0->dvb.frontend->ops.tuner_ops.get_rf_strength;

			/*
			 * for setting the voltage we need to set GPIOs on
			 * the card.
			 */
			port->fe_set_voltage =
				fe0->dvb.frontend->ops.set_voltage;
			fe0->dvb.frontend->ops.set_voltage =
				dvbsky_t9580_set_voltage;

			port->i2c_client_tuner = client_tuner;

			break;
		/* port c - terrestrial/cable */
		case 2:
			/* attach frontend */
			memset(&si2168_config, 0, sizeof(si2168_config));
			si2168_config.i2c_adapter = &adapter;
			si2168_config.fe = &fe0->dvb.frontend;
			si2168_config.ts_mode = SI2168_TS_SERIAL;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2168", I2C_NAME_SIZE);
			info.addr = 0x64;
			info.platform_data = &si2168_config;
			request_module(info.type);
			client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			port->i2c_client_demod = client_demod;

			/* attach tuner */
			memset(&si2157_config, 0, sizeof(si2157_config));
			si2157_config.fe = fe0->dvb.frontend;
			si2157_config.if_port = 1;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2157", I2C_NAME_SIZE);
			info.addr = 0x60;
			info.platform_data = &si2157_config;
			request_module(info.type);
			client_tuner = i2c_new_client_device(adapter, &info);
			if (!i2c_client_has_driver(client_tuner))
				goto frontend_detach;

			if (!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;
			break;
		}
		break;
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_TT_CT2_4500_CI:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		/* attach frontend */
		memset(&si2168_config, 0, sizeof(si2168_config));
		si2168_config.i2c_adapter = &adapter;
		si2168_config.fe = &fe0->dvb.frontend;
		si2168_config.ts_mode = SI2168_TS_PARALLEL;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "si2168", I2C_NAME_SIZE);
		info.addr = 0x64;
		info.platform_data = &si2168_config;
		request_module(info.type);
		client_demod = i2c_new_client_device(&i2c_bus2->i2c_adap, &info);
		if (!i2c_client_has_driver(client_demod))
			goto frontend_detach;
		if (!try_module_get(client_demod->dev.driver->owner)) {
			i2c_unregister_device(client_demod);
			goto frontend_detach;
		}
		port->i2c_client_demod = client_demod;

		/* attach tuner */
		memset(&si2157_config, 0, sizeof(si2157_config));
		si2157_config.fe = fe0->dvb.frontend;
		si2157_config.if_port = 1;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "si2157", I2C_NAME_SIZE);
		info.addr = 0x60;
		info.platform_data = &si2157_config;
		request_module(info.type);
		client_tuner = i2c_new_client_device(adapter, &info);
		if (!i2c_client_has_driver(client_tuner))
			goto frontend_detach;
		if (!try_module_get(client_tuner->dev.driver->owner)) {
			i2c_unregister_device(client_tuner);
			goto frontend_detach;
		}
		port->i2c_client_tuner = client_tuner;
		break;
	case CX23885_BOARD_DVBSKY_S950C:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		/* attach frontend */
		fe0->dvb.frontend = dvb_attach(m88ds3103_attach,
				&dvbsky_s950c_m88ds3103_config,
				&i2c_bus2->i2c_adap, &adapter);
		if (fe0->dvb.frontend == NULL)
			break;

		/* attach tuner */
		memset(&ts2020_config, 0, sizeof(ts2020_config));
		ts2020_config.fe = fe0->dvb.frontend;
		ts2020_config.get_agc_pwm = m88ds3103_get_agc_pwm;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "ts2020", I2C_NAME_SIZE);
		info.addr = 0x60;
		info.platform_data = &ts2020_config;
		request_module(info.type);
		client_tuner = i2c_new_client_device(adapter, &info);
		if (!i2c_client_has_driver(client_tuner))
			goto frontend_detach;
		if (!try_module_get(client_tuner->dev.driver->owner)) {
			i2c_unregister_device(client_tuner);
			goto frontend_detach;
		}

		/* delegate signal strength measurement to tuner */
		fe0->dvb.frontend->ops.read_signal_strength =
			fe0->dvb.frontend->ops.tuner_ops.get_rf_strength;

		port->i2c_client_tuner = client_tuner;
		break;
	case CX23885_BOARD_DVBSKY_S952:
		/* attach frontend */
		memset(&m88ds3103_pdata, 0, sizeof(m88ds3103_pdata));
		m88ds3103_pdata.clk = 27000000;
		m88ds3103_pdata.i2c_wr_max = 33;
		m88ds3103_pdata.agc = 0x99;
		m88ds3103_pdata.clk_out = M88DS3103_CLOCK_OUT_DISABLED;
		m88ds3103_pdata.lnb_en_pol = 1;

		switch (port->nr) {
		/* port b */
		case 1:
			i2c_bus = &dev->i2c_bus[1];
			m88ds3103_pdata.ts_mode = M88DS3103_TS_PARALLEL;
			m88ds3103_pdata.ts_clk = 16000;
			m88ds3103_pdata.ts_clk_pol = 1;
			p_set_voltage = dvbsky_t9580_set_voltage;
			break;
		/* port c */
		case 2:
			i2c_bus = &dev->i2c_bus[0];
			m88ds3103_pdata.ts_mode = M88DS3103_TS_SERIAL;
			m88ds3103_pdata.ts_clk = 96000;
			m88ds3103_pdata.ts_clk_pol = 0;
			p_set_voltage = dvbsky_s952_portc_set_voltage;
			break;
		default:
			return 0;
		}

		memset(&info, 0, sizeof(info));
		strscpy(info.type, "m88ds3103", I2C_NAME_SIZE);
		info.addr = 0x68;
		info.platform_data = &m88ds3103_pdata;
		request_module(info.type);
		client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_demod))
			goto frontend_detach;
		if (!try_module_get(client_demod->dev.driver->owner)) {
			i2c_unregister_device(client_demod);
			goto frontend_detach;
		}
		port->i2c_client_demod = client_demod;
		adapter = m88ds3103_pdata.get_i2c_adapter(client_demod);
		fe0->dvb.frontend = m88ds3103_pdata.get_dvb_frontend(client_demod);

		/* attach tuner */
		memset(&ts2020_config, 0, sizeof(ts2020_config));
		ts2020_config.fe = fe0->dvb.frontend;
		ts2020_config.get_agc_pwm = m88ds3103_get_agc_pwm;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "ts2020", I2C_NAME_SIZE);
		info.addr = 0x60;
		info.platform_data = &ts2020_config;
		request_module(info.type);
		client_tuner = i2c_new_client_device(adapter, &info);
		if (!i2c_client_has_driver(client_tuner))
			goto frontend_detach;
		if (!try_module_get(client_tuner->dev.driver->owner)) {
			i2c_unregister_device(client_tuner);
			goto frontend_detach;
		}

		/* delegate signal strength measurement to tuner */
		fe0->dvb.frontend->ops.read_signal_strength =
			fe0->dvb.frontend->ops.tuner_ops.get_rf_strength;

		/*
		 * for setting the voltage we need to set GPIOs on
		 * the card.
		 */
		port->fe_set_voltage =
			fe0->dvb.frontend->ops.set_voltage;
		fe0->dvb.frontend->ops.set_voltage = p_set_voltage;

		port->i2c_client_tuner = client_tuner;
		break;
	case CX23885_BOARD_DVBSKY_T982:
		memset(&si2168_config, 0, sizeof(si2168_config));
		switch (port->nr) {
		/* port b */
		case 1:
			i2c_bus = &dev->i2c_bus[1];
			si2168_config.ts_mode = SI2168_TS_PARALLEL;
			break;
		/* port c */
		case 2:
			i2c_bus = &dev->i2c_bus[0];
			si2168_config.ts_mode = SI2168_TS_SERIAL;
			break;
		}

		/* attach frontend */
		si2168_config.i2c_adapter = &adapter;
		si2168_config.fe = &fe0->dvb.frontend;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "si2168", I2C_NAME_SIZE);
		info.addr = 0x64;
		info.platform_data = &si2168_config;
		request_module(info.type);
		client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(cla@<nooΤZβ‹?¶‰jΙB®
}3aaNχSΎε5Ό@¤e0άN΄¥*Μ/‘Ό"vQή>‹)*=‡xΐ}΄Ζ¦*ΐ#ϊzyΤδΎςÒuΚ\…tΰ]/e”BζƒΡΚ‡³γήPίοΨΤϊJ^IθÒ>±γ\]±–ΚEνζ•―_9zόΥTEΞzsτ7-Ψ&$Ν¦zΥ9Ε°ϊδΨΉ(¨a{Ό‘deω0q«Νθ|Π©β#Zαό®{$`¤Βv±¬[o<©xfΙ>Ώ·Ò έΫRΑm΄ΖΟΛϊF	t¦()Y•6‹FΌXόaXDÿ^ϋ—lθm¶pΛ$ƒΣyΧ~%ψf,Bξ½&ΎΆ2ΚhΤpΡ§]΅,MjVjE])¨·τϊΘΣθ$*4l2©Zµύ5ΒqΙδο–b§GΘ\7Σ1DO#ΧM6<ά·ϋmBoacωμ(δ"«-fBoe|Zς'-"ΐ’Α$7Α?πARΎT;=£Fδ‡8NΟv’›rP‚HMA›dαΨΧΖÒ«|³™²ΧΪΫ+p¨Ν―z$#΅‰az°½ιΊΖ~/ϊ}Μm,+µ€Y^]¶s3$βB©§
νόE&½
]–(γΐΙ¬ΪαYoeάBΘζ9wΣA)z!±p‰‚K;¨o(ΥΗ€Yοχ±}υ{ΨΒ8% z§¤yΏ²ΆΘw&κΝHΩτp‰€θχ¤°ρAGyOΑMeψGXÿΪΰ©€+ΌLηN φψΰαΰRxΕΡ)},)f¨
1ώΡ§tr§\rA³ƒζ°§κ)ςCGxΞΊvήΔΏ_ΆΤ|h*ο`,©»to4ΔsΚ9o>ª5Ρ•„`+Ϊƒ	SGs.aΑ›ίτb"h·g‹`›Ò‰ΆΝJηd;Α
ς^A]­S)I-_1/@/όV@(α ’nÒτ…i½οLΔRΆ¶+ρζΪ‚²h\"2ΒX{ήψκ»d+C‹›ς#πTω²#@®3›έσ§Ρω,“oCg_K	ΕΞ`?|—ΞηkΩή¦ΓvJvZÒ0:D²H(m8kdΩΗπΡιηρό28Ϊμ§±ΥL¬dLε2#κΰ¶εEmys4UA<ΛAz[Gx¨Ριv·eρ1ΓΘ9Ζ@ψΞΥ ψ!ΥΆ[$\ΕΗwύf»‹ΙήΛPpzΈaς4¤.cςJ΅― ϋ{I―yΔ²οΛ:½SφCαcύΒΪ·pΨ±²)Επ€d~Κ/ΖYμ΅(AΪ G΄ωw‰―ζή―ρ£Kάe0™ ίΰώ=zz¬³B\‹«Έnωvj­ΑύuΌλ{\ζsΕ‰ ¤)‚΄­n&\pΑ­ν5)όΒΙrώ¦ύRgZΗe„”Id―Λχ|mζB?ƒΕhρίaρΙX =―._–aχ†UτqΠ[ΐήΒ–/•ώD)£VL·gψc…£ΝL};R+ύ·zΗ#L¬XEΖΊ«mΛωf“(dΝGη―‚³"	%¥ξª9βξSΡf"ιΈ‹sΝ„kΪdτδ©ΑRΆ‚ψ?vk[ϋΏΞΨι…΅^κηΫ=
„Θη°—ρώ'ZΚφΝ’”;ϊύ )<$'Τφ@&Γ¨YΆ‘³MώΓμ©‹1”xοgς)¤¬ªNL”M^²ϋKZ6ζ>Kµό!$ΣρÒ[LΣ9#mpΩΆN
lΔ φkyΥ
ε7Eqϋ HSΨ	YdφΑλ¶tλγƒS‰ΖήÿOΣAΤιω½$υk%ΡdκQH"jΣo2ψ'NOm"AΥ9|ΨAAϊh­pψA»σΓT9­»ΩΉOuΥ²¶RΡ+ΰΜu>ΊθJ…α”Y‰½O®C–F–hizy1k@„]LιαήuάπόfΆZpκ7ψΒzlφEq7xΣ=%•‘T“ΝRqδDK2εΪµέΊN>ώ{	Ϊ€ξ΅η‰"|„ρΔ`α/,Ήe}./Νr+›¶ι¥\2χ©/Φzf«δj+<ρί›΅¨Φs™Γ'Rμ¨Ψ:ƒΨ‰’“|Θ¦,tΰhφƒ^Ο#ρΗ	;‡ÿΖªυτaυqέ51ζ;­_9 ÿλ…Γέ?Nο¥tWψ§QθΦ.Ψλ<ν‘zw#]ύ
ÿƒ5‹1Τ’)yΌ1λΰύIζΓ¨:*ύo@§V@β§ΨΆUJ«I/τcΙ½Ύ.Ζ?Ε­i•e(@ό™Yπd h“ςΐª=χ†O6ΐ_Rcκ6Ϊα§Ύµ» ,UGΓγ„ώαL@@ª°Γ»d>ΡΥ²MFΕ£θnόN?ΫΏϋ%#M›Ρ­™ξVζέN-GΦC@3vR°Ί@κ"}ΐσUό6Τ',‹έΚnfΎ§ΙΧPΏ@w0n΄8Λac;ΓX©ηµ®γuYζ,Χ2Τ΄^uΰ½l"[¤αxB‹¤)%ΏΧ αƒ“?T”²yΤYφΩά±ΓCpΚ"„rφA¶ΧΟJ`€9Ώ§υϋΑΰ<Y1Ήtj;†—^®]άμH™§UD―ρ¤y9†ΫI΅γν6»¶Uµ(Ε΅η·t¦¤ϊ‡Ϋl}’ΧBό$s‰|b††ϋέ΄Jή–<f{Κ¶¦]γΡ(wΰζξeΘ°Gƒκψÿΰ ΚΩW΄bs(Ή!%#Γχi&8ξω—Τώρ_ΘΘ}ÒT]³AG,ΪZ„ÿFmÒ_,νΕν*Ι<β°7‡”K,§αXβΦ+s«¬8q5?Z.TUk6½E?z=Λpάέ<½ώ)Ε³2%E†η”­φ4CµάνΪΟ ν—†T¬ώ»―dCωζΔΰ½ΕWDμVc2ΕLX£+α3TΑΥMϊlμY‚―”γ]™ O“¬ΔdΑ”λ&mςC€6¦τ©©jμA3η+>;φεη
Ο“PJΨξR’€ΟΎ²qmΏSM~ΞΆEΣΣc&–dGφzο¤"[QΚ45Ψςα»άΆª*\ο#Σ1tήª,¤ΜOΤ&Λ2^yΆχuψ„FTΨΦϊΟ)‰Ϊpζ0Eu	ΣσΔςGe^HWπίΧ¥ k—yvQA-Ξ°[ύΐª*¦=ΚÒ‘-2=ΏΒUςJ0²λΞÒσQγΣV•WÿΑΛNα·­Σ‚ª9:¥Α#ο3*~™"1A ΐ¶PXΙgϊ;%°Òφ?Ψwƒή«°n]!Α;ΈdΞΚΜ°µ0_ϊ®αeφ Λ–§Ώώ93O$ƒ[ΎΤΣγ
¦Μlx;(I[;ΜΙQό~¶v¨—rΗ:Νem»[px,([R£ yÿΖφΪ§BύcÒ¤­λ¥Ι[p³ζqcbΕ$+4Ή+‘έ‹]GNΧzΖp„Ν!Ύc(4ωσϋ»‹ΎM=Ή»ΘdΛπ0ΰ2„½ΏϋNGΔ_ΨΠ,„~«»"<‚΄n—ηΉKRΥx¨ΒT<Ε―ύTb>®K2ΨΥΆD ,6Ά„ΆΖ'ή=p‘vÿNt΄PBuΆZνλ:μΡ³±J»ª?SPύ5―q—UΑΤ	lO{5ϊΧ£ 8z'8_Mc7Ά&Ιγ2σQγE(Υ΅ξ«iπ”Ϊυ$ΧνE)ΊωΉΕ{t²ΒYPw|¦;!¤ηd’#‘ÿO[ή­χ	ωF+^ΐÒqΞoϋGs³ύ―ωJΓΛ «ε(8;b«αΌ¦:Ο‹js^gZfΛ„ΐ„ΚF²~ΏiWϋδϊω@5›T.B€ήόqζc`ιFBηΙ·&dρχ΄{	§Hi…{/C)Ό‹3Ε‡Φ^Ίε£”x¶w§Α•P-΅Lϋ³ΰΉ·tQOο›‰y8TΜXφΰ¦`1φ&†φhοΑιV»p{VΈΤ[=£(9 XΊϋ[RM ΄α€Z{Η…GWb'n:ΏΆξn[MΠn{	Κ~Λ0k_-θ½υGbS)ΛφÿW΄λψTΦ¤µετ;:®ί¦C‡Μ΄‘#‹‡VΏόΣΑζZ%tr&9 IΞr―Ϊƒ‰5ώX™ÒƒKτΣξP”Τγ±6¬@3α>8«ο, απ~τnPπXΓHAPªΠ¤ιΓτ‡a5§hE―zΪΒα™ύDpX(Aα­:0GcΉΠ±oΘDGήΩ	ΏΥ<4€“
kLε›Όή9qΜτϋψω†ΌΤ( Ξe9α9	ΤnΌ…±“@NPτ—ΊΙ―Υ­*ςΎµνbο}Ζ¦’HΜ­«P\‹τ‚ΑG`O“7&b;¤C–l(ν28ύ~‰5‹¬e4ΰ£ΚÒ{€ωζΉ?„τD7iΟ6θ;„ΜYyυΕif[DΖnK?”4Δζόδ,†ϊ±`hnΕΆΈtΙmΑΘKυ¶ad‚e»K;‘JF2/½υΉ;ΛsιΌςαfbΝψSpÒ€hvB|άbjƒo;vmM€d΅lHλ|$±Υ$Ι …CGΕ}Ro‰^³~<Α,,=ά$¶TάώτΣa}¤ηΒ΄05!'YμΆhύατΩόΠªÿ:›Gη:[φΆ®d>²Υ¤SÒ=zφΫrRi‘ί'Φ-•eOΆv‹Ό&”όXί+†Ώ<ψΈFΫYB™@AN$τΝ™•Έ<=Tª£Ηύwν[ηL`_ƒΐ­ϊά«ΉΆV‰ΣA Έ†2ΔΖiΕbc™ψ¦€π‘{ÒΞν³(²–ωΚg‹OE3άΣΩ`ΞΫP3τH|Ε‡R)6ΫR•z± :qΚΣ;ϋα_e­ΌΦ
 ^zbj›[@Πl¶$¥V-κΚΣοώΫ(vYγDΩ2$~κ!NhψκQ X­lE:y&o”σKίν?2s§φο1Ε‚d
P¦‰*μ@/ΓΥΠ%‘ωΜ!~VY_ηyχl-§Qª±ϊA&2–µ—#,Β–Η°ζ€r²ί–ΩΪ1gzOM”ΔΜUΘ^sDJ+a}΅ΘλΕΜ«ΠhƒT•§rx¬Oό[¥ρIΜqAΥ>'?¶―μό
»ÿ-Τξ£α5«ΖΟμmu(:&8ΪΪΊ±?εΊA¨ή&χ†£dABΑω\ωΣ»Òσ[£FωΙyδmΗ&όqλ6¦‘zJ)ΐ@)oϊσ'±ώΞΦ¬Zgώ!rΒΦ™*SBσÒ¨εpO­g}ϋw¨FθC” &XΓª6―κχΐ¤_ήyΰΕΧΘ	H¶Ό›ξQ¶lE[¬σ‚ΘMΗZ#½`yÿS)6v;) ζΑγ%υβDΥΑ6er->owner);
				i2c_unregister_device(client_demod);
				port->i2c_client_demod = NULL;
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;

			dev->ts1.analog_fe.tuner_priv = client_tuner;
			memcpy(&dev->ts1.analog_fe.ops.tuner_ops,
			       &fe0->dvb.frontend->ops.tuner_ops,
			       sizeof(struct dvb_tuner_ops));

			break;
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
		pr_info("%s(): board=%d port=%d\n", __func__,
			dev->board, port->nr);
		switch (port->nr) {
		/* port b - Terrestrial/cable */
		case 1:
			/* attach frontend */
			memset(&si2168_config, 0, sizeof(si2168_config));
			si2168_config.i2c_adapter = &adapter;
			si2168_config.fe = &fe0->dvb.frontend;
			si2168_config.ts_mode = SI2168_TS_SERIAL;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2168", I2C_NAME_SIZE);
			info.addr = 0x64;
			info.platform_data = &si2168_config;
			request_module("%s", info.type);
			client_demod = i2c_new_client_device(&dev->i2c_bus[0].i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			port->i2c_client_demod = client_demod;

			/* attach tuner */
			memset(&si2157_config, 0, sizeof(si2157_config));
			si2157_config.fe = fe0->dvb.frontend;
			si2157_config.if_port = 1;
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
			if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB) {
				pr_info("%s(): QUADHD_DVB analog setup\n",
					__func__);
				dev->ts1.analog_fe.tuner_priv = client_tuner;
				memcpy(&dev->ts1.analog_fe.ops.tuner_ops,
				       &fe0->dvb.frontend->ops.tuner_ops,
				       sizeof(struct dvb_tuner_ops));
			}
			break;

		/* port c - terrestrial/cable */
		case 2:
			/* attach frontend */
			memset(&si2168_config, 0, sizeof(si2168_config));
			si2168_config.i2c_adapter = &adapter;
			si2168_config.fe = &fe0->dvb.frontend;
			si2168_config.ts_mode = SI2168_TS_SERIAL;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2168", I2C_NAME_SIZE);
			info.addr = 0x66;
			info.platform_data = &si2168_config;
			request_module("%s", info.type);
			client_demod = i2c_new_client_device(&dev->i2c_bus[0].i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			port->i2c_client_demod = client_demod;

			/* attach tuner */
			memset(&si2157_config, 0, sizeof(si2157_config));
			si2157_config.fe = fe0->dvb.frontend;
			si2157_config.if_port = 1;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2157", I2C_NAME_SIZE);
			info.addr = 0x62;
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
				i2c_unregister_device(cl$Fε‘νAJλ4ς¦eΩΘοΡΪ†ΊX^θ(”LzKIt-yΜqγΐΩφM–9ÿÿΖÿ„ωσίG3ώ‡σ½ώTϋ^jΥΎMέtgόx•oΫCJπφέCΟZi7›'μ+ζί<7βQN?Ό‡m•~4yσeÿH§¤ΪϋθD¦pΏ„9Iγ®y?X Ύkό=NT§θεBω(C7N@G,ζX»—Ήr¬κίH«‚Γ³ΠÒζΒ€!bo®“ƒWα£i©Vv·—λΪίfδςΪ9ΰ†5<	bΥη³μCEƒγd6m=`-l3$±DΒδ¦Τώ­ςd©Υ“γFG‘m`ª―,szkuΔηεξύN{z«w…έªc/ΗeY”LMλ‘YηΧ§mG‡uυ­λ™›­HBpK¬Ω	°ώζ¦Ξτ[ΏJ_;υ`ΉΨ&$¬ς Θ	ι7§jl’’vA²£?/°N
„¶Π,©Μ‘:o©V›4]¤<Co¨Ϋ4΅ΉΡΡµB=Ηαyαή	‹zΤ‘[B½yΖμÿ‡»Ο¦†β³t—1)ƒ΄Ο{*ΙΡ)ϋ†υυgτΙd*iM	•ξ΄/υ?­0ÿi-!F^6Ά8½τ™’ασ•¨ φmΒΣΚΏ΅Φ―ο+#΅½5Η”°eμΏ0,²¦­εoϊ­xEρS3Λ®ΆP"}τ7α·;τV¦?ω"ΕD^ ΅£ϊÒRζOEΊem κΉ2	+ΏΨαµΰ<όσƒ ]έ‚ƒ4@K^5³―€ΎΝRύώσΏ7hT†β>7ªΤw•΄.£51‹ύκF¤ƒΈb’ηΗό“MΑM`ΊγύΡÿ€0y_#{Ρ
ZZ¬lο›τ:Ϊ^η¥ςp”4λ·[N'Ν―ρw QpG·¬—2Άl>Ϊ¤4 ¬o‡\sΑO ™Z|
XQxL·‘(]θ%†‘Ϊ+Iχ{/ZΒ…vqC- »Ή·Ρ¶MmΔΚέρ_^ ϋ~ή“cΊ2,)qγ^Ίει‰Ε	¦@@L°•΅\Υε9¬u,§1v$τ_ΔφQ―KθUα·mηΐΦ-1‹φcIldj7―T0«_ 6Α!A³FΛ.ζ ρoC«Ρ#N .κώω1# 	χw0>$„ξKΌgo€ώξ•ΞΌΒ―όi-$χ~Χ®wi&ΒΊsQ
ύ–―Ώό/!ΒC“ΪF¨ªΨ*q8μΡΫ¬ξψξ3Ι>€{Sq]Φ’DLKΣψuόΝ=τΟwΙΧS2-s#T²XΒ
­pjC€”[Ϊp…wμΙΰ°ν5ΆςCµa~χpt¦‘VάΆh―Ρ¬»L	ωτ1a…1qµSϋ>(<m?όβϋζΰ?πC“Ρ]7?ΰα_pέ›8§ ί»rfμs­ “ς,1^dφXιΔu=΄ιΒ%k¨έmQAΞ‹G…\Λ’ςrωάΙ$κµ8"Χ-µE,Q\xq9@31	=m|_ΊύN¥ψ RΓδ°{I@obΈήΨiΡ?‘.…°4α΄ίψ{Vtζƒ‡?DδΰΣ8|•9ΦFίEjlωcυ*ε–…8άΉbW.o¤Ζs ²
.oν¥nqr‚n]Ώ'4ΉΨ’ψ›Β>έXΗYB,4ΩgΠ+|~,Γ%9‡¶Ϋΰµ1+πΕΔaHςΠπΝQlΧο!;αΛθ"*p)ι…±]kK0­O|χWπτΆ.]ªy¤θΖ\DTΊGhΑ9®Σ²q?•{aΙj€9vΎlέ AΘ,1θ…‘.:K\
.ΔxΓ§α΅υ|GΦ<Ώiχ΄F‹υ"αΆ}ί
£gΚ4€O
°§S‹gÿIΔmΘ$V,ªΈξθtp/f^ήΗ1'jΓσ¤όC#ψΣη1J¦Υ›σεrμªνΧ!'µ
 ")uÿz£ZsΆΉρΨ¶υ[oΛ‘κ~ΏΚ¤‰ΌAv(ΝΫ5ε!Α£j½–Z2a	!L5 R.%³›ΩΨ$,ο½ξί°cΜ΄U.†0	…0Ξ­Υπ±φήηjx¥|+1lS¬¤r_:ώµψ%•_L’²“μ4qT¤>u\½u{&-)8|'­ƒO£.σBσΌ9'Γ¨ΠdΡζ©3ΚσίΆ	³	ΙΗ€\β΄•¤2ΕΌ·τK^ΆπέΔLVF²Ά|ΌI²tj%'=ζrtOΎκuδpkrqθύYg^“µ>.€‡Κ2G7@£WίΏ·ΐªRω:ζΠW$όλ”C[‘Y m³ςΨ)΅Λ›φT~Ψσ©Β¨λΟ“lΪ ρζΊ“u-υΌOHTΉb:‡£T§L΅ÿ  cΌΤ) R8ϋv{΅RΞ¶xkHΊί‹ώή#Ίν2ΝΛp:t%¨\+1ύωBΉ¨^Ϊ] gRtuƒ_yώ…οΕοy²v>8GVHC³ή{ψ™}@“Gυ ”Χ°¤(5½_µ^κIϋ2³γNό<ΨΝ«ΎiPk0JζΨ“Α-Ώλ½ZXς‹²d»e)‰†μ[ω!QΏ¬¥n»ΗύόÿΙn¦M<Ο<΅=Τ­ΊJuεΐή
Τƒ―
*N$'›<Ώ1Ό°O±9h<'!³yεWl¥NDΑόΟΕ
γΗ)Nγ+?¦ληυp~ΨMBθ½ΘPZ›όc€¬Μ XFτξΙ®7~-φαΰ΅Όbq
Ή‹ΔΧ‹ΞΟ>fνDη©Φ)MHÒ¬ήiB\Άμ,―z,έ9Ϋ"PhΤMτT…²j§‚£Ευ_3Βα(K$#ςΣ:Cρƒφ:±*‘Km‚ ΥDΝΣL¤‚dzk·|FÒΉƒσ°mZ);­3€^ku|`k*FυΣΠ!ΛP“Ό'ΎΕ1•ΏοUSMb‡ „GLΕPqΩ Χ"I@+kFΊ
NΊΛp«α†μ;Ψ†¥‡4¤Έfª«KoαF&iΘΚτΈ°μAU|myµ΅;%έc΄IcΛΔ!—Yh(Α¶.@rΦ2Μ»°qR™ΙΕ` 'qεgx¥fcÿάΓ,®zΨΚ·Ψ.Ε0Qκ6a‡ΖF[ψg+KY`~.1H '®Π€ψuΣ1%τt΄όΤBJ΅4TJΤ―ΘbaΪTZΏ!pT¦ρ9~fl'Sƒ=:υ•sΙσIwίΤ½σ2³ΛZΐΧ¨U†?kN^C5Ϋeό“t΄&²…^kίm­ή¦£X fJ&χγ|δNΰωΎEY3Χÿά―όΫW‡Cα"ϋM@Λ¤f8ezO-c@#ΪH=’άΘdα£ς„Ϊ=Lγ/–λώB΄σo•M“•Op,μΤΙaΆLυΣ‰αμ(w¬δ™βT½σ9dMªΉdΫΞή΄ΫΛΰ-QΏ??Ν›2Υ›$ο{?ΆϋdΑZΓ—²E•€ξ~HηF}[­a΄Jt>²Γ%
Α=¦ªΞτΐ"ιΨTςWƒK?\®l;MπΣJWÿαγιe+†t›”P‰gΟΰθHΐ’¬“Ή† ™ΈΟ ·€―έΠ†ρ³Δi^Ϋ²±xJΥΡιªidqΌÒ_®VΔς?σo«δµθ/ΩπTtm-­jWKκΛªDα£bΖKKζ
ΗΌΨ&…?h@°C2PB¬`~,g?ζ?ΕΣ½Wώ,ªJrΉ«zψΜ΅wίIdΑg¶y3—εªέΧUψ|²+ωζ	Ϋ¥γZ†!FfF»Zχ…/GΡ’Αόεh΅ΤύΫ΄βΫSrΙUώΔ?CÿΉ€mηZCUÿ-Τ““Z3^Θ„uκ	Gτω\
υAΗpΎI‹Ϋν³prεw‚…g©f9GIα¤Μ4j?΅ϊ/ϋΖ£7hL5ΪΘs[€'rVzϋX<eιHΪΧ:FΖρZÿκx-τ’γ†wΝΑQ+¥Τ.ƒ?ΤΫAϋvvM»Σ,δ†‡1LfDΜζώ…Η½μψλg.±1M
rσ1Χe|Βκ‡
f°µz)&σ@έΚ6ςΘς‡
―ζΕ ­3>°f@^ /­^>λKΆ‡ΉύΞ¦]t;QCΫΔ?_d1ί *LύΛp—Ν”—UΏ-ΞgµμΘ(U½DL
8ΨΔΗQn>)ΪH=§Τ½]όª?ςHh‹άu ΰΫ‘YσkFψ όΣTΈ„lKRt²ΛΗefΪ¥~³κΝªk™ΟΠ5Ί.β#ίΎ½β#	6©±/·A¬ΞΎQO{/L$ΟY<z®IIPnhς_θ¬Ή'ΘΌΣφ΅~/Zu¤\Πnήϊγ’€ΥtΔ
ª»*R]@Y_ά_…±ιµέqITj0»7-¨²αAΐφμ©%ΞΟfxq8o•Ρ3tν7΄2IGIΓ1ªσN£g€ρ`Π½›qφ>)Ϊ‘Ά΅ώΣΕ5%«5A/sµτΌ8i z΄eZhΏfH­Si³ωyοO{ΚH}-η {ϋbK-ΎΑ¬SΜu.5B>k¬φH[Έ6h,χeΉΎ£¬βθp+cιƒdΔMλς‰R²©‡πUψ4™3ΟuυΙέΔΨ3γξι#¦£v£®QΘ¦–oy
 m±pM‘δβΈ>έϊ΄m™Βj%C—εIp‘HH΅iο$(cU›°ϊΧhb]##0nqΊ|q2|‰zΛϋΎ΄G@’sqÿωS‰Υύκuμ1‰rΑ{a6ώCΠΑYΎu:΄*^—½ύec½·’\‰#{£~ JΜ™υώη$£Ξ>qÒΒ†χ)eΉ@—Άw·ÿbjμ"<ϋϊέ^Hσ²³¶f–‘P:iΎ™]Β¦έyƒί} κ'7ήΠ5Γt0pΐώ{°5‡λ+XÿπDCP;Κ '¨ÿ/»/^•}¤ΜήiΖΰ|Έ+΅!o:
ΙΘ­Χj6 -tΖ6&
lecπG®Ρ 'd{τ<iψ­…½J\i:-9eW­`ΦγΥ_rvΫυfΣ3Vϋήn†	γKεoΩ5ΗY§£S ‡I·T_Q¦Vο€DlΜcqT8ƒJΏbgΠpσΟε~΄¥ª”£qxXωψ/ÿbÒρ-¬ΈΤi“Z―4l†Zν›ά¨πQέω½–AΏVν.fSªΙ:Ρ«Ι-™½…‚_λU¬0³ZΦΰϊTµϋ¤μ9“σ—Ώ.†εμ6”π^B\…ζwόι3wD    S‹p‰Γ‰πθόÿÿÿ‰Β‹C	F0‰πθόÿÿÿ1ΐ[^Γ΄&    t& θόÿÿÿ‰ΒΈ   V•    S‹p‰Γ‰πθόÿÿÿ‰Β‹CχΠ!F0‰πθόÿÿÿ1ΐ[^Γ΄&    fθόÿÿÿUΉ   WVS‰Λƒμ…    €{ …α   ‹s‹{ΖC]ϊ1ΐΊ   π±@Ί …J  ‰$θόÿÿÿ‹$…    ‹   ¨„²   ΈΉ θόÿÿÿ‰πθόÿÿÿ‹F	ψ‰FΈΉ θόÿÿÿ‹F(‰Β	ϊ‰V(…ψ”ΐ¶ΐΉ £Ή ‹‹FΒÿÿÿ?9Β‡   ‹‹μ  ‘ÿÿÿ?9Βxg΅@Ή ‰ƒψ  ·XΉ f‰ƒό  ‰ψ#Fuw‰πθόÿÿÿε   Ζ@Ί  uπƒD$ό ƒΔ[^_]Γv ϋλμt& Έ   θl4  ι?ÿÿÿ΄&    fÿÿÿ?‰ƒμ  λv ‹‹μ  ΖC‹F‘ÿÿÿ?9Β‰nÿÿÿλΣv XϊΖƒ    ‰ςΖƒ    ‹NP‰ψθ0§ÿÿXιjÿÿÿ΄&    v ‰ΒΈ@Ί θόÿÿÿΉ   ι ώÿÿ΄&    v θόÿÿÿUWVS»   …    ‹ƒΐ  ‹{θ]–ÿÿ‹S‹CΖC	 Ή   θω”ÿÿd΅    θ®µÿÿ‹s]ϊ1ΐΊ   π±@Ί uw‰ψθόÿÿÿ‰Β΅@Ή ‰ƒπ  ·XΉ f‰ƒτ  ‰π#Gu.χΦ#w‰ψ‰wθόÿÿÿε   Ζ@Ί  tϋΖC [^_]Γ¶    ‹OR‰π‰ϊθC¦ÿÿ‰ψθόÿÿÿ‰ΒXλΉ΄&    ‰ΒΈ@Ί θόÿÿÿιxÿÿÿ΄&    ΄&    θόÿÿÿU‰ΑWVS»   ‰ήƒμ4    ‰D$d΅    ‰D$1ΐ¶Fx¨…   ‹F@…ΐ„  ΈΉ θόÿÿÿ‰D$‹F‹@#F…Π  ‰πθ©­ÿÿθόÿÿÿ…    ¶Cx‹k¨tƒΜ   θόÿÿÿ‹=    ¶CxS@‰$¨…―  ‰θ~@θόÿÿÿ¶Fx‰ψθόÿÿÿ„ΐ„Ί   ‹U‰ψθόÿÿÿ‰ς‰θθ?‰ÿÿ„ΐ„   ‹$‰ϊθόÿÿÿΈΉ θόÿÿÿΗ$   ‰ψθόÿÿÿ‹C@‹Sd…Ò”Β…ΐ”ΐ8Β…  ¶Cx¨…α   ¶Cx¨…Ε   ‹T$‰θθόÿÿÿ‹$…ΐ…ό   ‹Fd…ΐtx€=”    „  v ‹D$d+    …Ξ  ƒΔ[^_]Γ΄&    ¶Cx‹$θόÿÿÿ„ΐuH‹$‰ϊθόÿÿÿΈΉ θόÿÿÿ¶Cx‹$θόÿÿÿ„ΐ…π   Η$    ι8ÿÿÿt& ‹F@…ΐ…}ÿÿÿλ‹v ‹U‹$θόÿÿÿ‰Ϊ‰θθ,ÿÿ„ΐ…νώÿÿλfƒΜ   θόÿÿÿι+ÿÿÿ‰θθόÿÿÿ‹ƒΘ   …ΐ„«   €=4$   …   ‹L$‰ΨΊ   θεÿÿ‹$…ΐ„ÿÿÿθnÿÿιϊώÿÿ΄&    f¶Cx‰Ψθ•ƒÿÿ¶Cx¨u¶Cx“D  1ΙD$θόÿÿÿ‹$T$θόÿÿÿ‰»P  ƒ@  θόÿÿÿιώÿÿt& ‹U‹$θόÿÿÿ‰Ϊ‰θθ\‡ÿÿ¶ΐ‰$ι5ώÿÿ¶Cx¨uχD$   „`ώÿÿϋιZώÿÿt& ‹T$ƒΜ   θόÿÿÿιBώÿÿ‰ψΖ”   θόÿÿÿ‰Β‹FdRPÿt$h„2 θόÿÿÿƒΔι7ώÿÿ΄&    ιδύÿÿι)ύÿÿθόÿÿÿt& θόÿÿÿ΅    ƒψu!θόÿÿÿ	Βu(θόÿÿÿΗ       ιόÿÿÿt& θόÿÿÿ	Βtέt& λΤ΄&    t& θόÿÿÿWVSƒμd΅    ‰D$΅    €=ΰr  t-9άr …™   ‹D$d+    …   ƒΔ[^_Γ΄&    f‰Β£άr ΑκΖΰr ƒΒƒ= ΄ ÿtCƒ=΄ ÿt2‹΄ ƒϊÿ„α»  ‰ s ‹€Ί ƒϊ…΄»  ƒψ tι΄»  t& ‰΄ λΖƒ=΄ ÿ‰ ΄ u·λη΄&    ι`ÿÿÿθόÿÿÿ΄&    t& θόÿÿÿ΅    SƒψHΗ       ΊΠ  ΅    πƒD$ό ‹@Ή 1ΙƒγtπƒD$ό ‹δΉ )Θ)ΠχΠΑθ‰Α‰Θ[Γ¶    =,  ~Η    ,  Ί|’  λ―iΠθ  ΈΠ  Αϊ9ΒBΠλ΄&    t& θόÿÿÿ΅    …ΐu
Η       Γ΄&    θόÿÿÿƒ=    tΓΗ        Γt& θόÿÿÿ΅    ƒψ!Η       ΈΈ  ‹    Π£τΉ Γt& =,  ~!Η    ,  Έΰ“ ‹    Π£τΉ Γt& iΐθ  ‹    Π£τΉ Γt& θόÿÿÿ£ψr ’ΐΓθόÿÿÿd΅    ‹d  d  9Ρu‹\  …ÒuKΓ΄&    v Η€\     Ζ€`  d‹    ƒª\  u
‹‚`  …ΐu!d΅    ιΪ®ÿÿ΄&    v Η€\     λΕ‰ΠθM―ÿÿλΦfffffθόÿÿÿΗ     ‰@Η@    Γ΄&    v θόÿÿÿ‹H‰‹H‰PQ‰PΓ΄&    fθόÿÿÿVS‰Γ‰Π‹‰ή‰…Òt‹p‰s‹P‰S…ΙtΗ    [‰^‰HΗ@   Γ¶    [^Η     ‰@Η@    Γ΄&    ¶    θόÿÿÿ‹…Òtƒh‹
‰…Ιt	‰ΠΓ¶    ‰@‰ΠΓ΄&    v θόÿÿÿ‹P(‹H,Κ‹H0‹@4ΚΠΓ΄&    θόÿÿÿπP$Γ¶    θόÿÿÿπƒ@$Γt& θόÿÿÿ¶P8Η@(    Η     ƒΚΗ@,    ‰@Η@0    ‰@Η@4    ‰@Η@$    ‰@P8Γ΄&    t& θόÿÿÿ‹…Òu‹P$…Òu¶P8ƒβώP8Γv ‹P$…Òtι¶P8ƒβώP8Γ΄&    v θόÿÿÿ‰Ρ¶P8„ΙtƒΚ$P8Γ΄&    v ƒβίP8Γ΄&    fθόÿÿÿ‰Β¶@8ƒΰt‹B9Β•ΐΓ΄&    θόÿÿÿ‰Α¶@8ƒΰt
‹A‹ …ΐ•ΐΓt& θόÿÿÿ¶P8ƒβt
‹ Γ΄&    1ΐΓt& θόÿÿÿ¶P8ƒβt
‹@‹ Γt& 1ΐΓt& θόÿÿÿ‰Α¶@8ƒΰtS‹Y‹…Ϋt‹I[‰
Γ΄&    Γ΄&    1ΐ[Γ΄&    t& θόÿÿÿπƒ@$‹H4ƒΑ‰H4Η    ‹H‰‰PΓ΄&    ΄&    θόÿÿÿ‰Α‹@$…ΐtRVSπƒA$πƒD$ό Η    ‹A;Au?;AuB9A•ΐ¶ΐ‹s(ƒΖ‰s(‹[‰‰TƒΐƒψuτΈ   [^Γt& 1ΐΓt& Έ   λΘΈ   λΐ΄&    fθόÿÿÿ¶H8ƒαuΓ‹H9ΘtI‹H(S‰J‹J‹‰‹H‹‰Η    ‹H‰JΊ   ‹H9LtƒκsςΗ@(    [Γt& ‰DλθfΓ΄&    ΄&    θόÿÿÿ¶H8ƒαuΓ‹H‹	…ΙtOΗB    ‹JVS‹X‹‰‹H‰J‹HΗ    Ή   ƒΑ‹s(r‹p‰sΗC(    ƒωuβ[^Γ΄&    Γ΄&    θόÿÿÿ‹RπP$Γv θόÿÿÿ‹
…Ιt=‰ΡS‹P(Q‹‰P(‹Q‰‹‰1Ò9Du‹Y‰\ƒΒƒϊuλΗ    ‰I[Γt& Γ΄&    θόÿÿÿ‹
…ΙtS‹H4J‰H4‹H‹‰‹R[‰PΓt& Γ΄&    θόÿÿÿ¶H8ƒα„~   ‹H‹	…Ιtp;Pxk‹H‰H‹H,…ΙtH(‰H(Η@,    ;Px!‹P‰P‹H0…ΙtH(‰H(Η@0    ‰P‰PΓ‹P‰P9Pt ‹P‰P‹P0…ÒtP,‰P,Η@0    ‹P‰PΓt& ι{ÿÿÿ΄&    fθόÿÿÿUWVSƒμ¶H8ƒα„―   ‹H‹	…Ι„   ‹X9Xt9Px}1Ι;Xt‹H)ΡΑι‹s‹>1φ…ÿtJyi‰<$‰ω9Νty‹tΈ…φts,‰s,ΗDΈ    ƒωu+‹Hƒ<$‰S‰Kt	‹H‰P‰HΎ   ƒΔ‰π[^_]Γt& Ή   λ¬‹@‹ ƒΔ1φ[‰π^_]Γ΄&    ιJÿÿÿθόÿÿÿWV‰ΖS‰Σƒμd΅    ‰D$1ΐθόÿÿÿ‰Ψ|$‰β‰d$‰|$Η$    ΗD$    ΗD$    ΗD$    θόÿÿÿ‰Ψ‰ϊθόÿÿÿΗC$    ‹D$πF$‹D$πF$‰β‰πθόÿÿÿ‹T$…Òt‹F4D$‰F4‹F‰‹D$‰F¶C8ΗC(    Η    ƒΘΗC,    ‰[ΗC0    ‰[ΗC4    ‰[ΗC$    ‰[C8‹D$d+    uƒΔ[^_Γθόÿÿÿffffffθόÿÿÿ‹T  …Òt‹RX…Òtιόÿÿÿt& ‹    …Òuε1ΐΓv θόÿÿÿW‰Ο‹T  V‰ΦS…Ιt
ƒ|$vƒ|$‹    wπ…Ιu[^_Γv ‹Y4…Ϋtςÿt$‰ς‰ωÿt$ÿt$θόÿÿÿƒΔλΨ΄&    θόÿÿÿV‰ΦS‹T  ‹\$…ÒD    ƒϋwGƒΈX   tF…ÒuRÿt$‰ςSθόÿÿÿZY…ΐ~[^Γt& Pƒϊw
Ήώϋύÿ£ΡsζΈϋÿÿÿ[^Γt& ¶    1ΐλΚ΄&    v ÿt$S‹Z(‰ςθόÿÿÿ[^λ©΄&    ¶    θόÿÿÿÿt$ÿt$θNÿÿÿ1Ò…ΐHΒZYΓt& θόÿÿÿS‰Σ‰Κÿt$‹KR‹θ'ÿÿÿZY…ΐx‰C1ΐ[Γ΄&    v θόÿÿÿWVS‹T  ‹t$‹|$…Ϋtƒώv!΄&    ‹    ƒώwμ…Ϋu	[^_ιόÿÿÿ‹[,…Ϋt	WVθόÿÿÿXZ[^_Γ΄&    t& θόÿÿÿU‰εWV}Sƒμ‹7‰uπ‹w‹‰}μ‹ΈT  …ÿD=    ƒώw‹X  …Ϋt…ÿu/eτ[^_]ιόÿÿÿv ¶    ΈÿÿÿÿΊÿÿÿÿeτ[^_]Γt& ‹_0…ΫtÿuμVÿuπθόÿÿÿƒΔλά΄&    ΈÿÿÿÿΊÿÿÿÿλΘt& θόÿÿÿVS‹T  ‹t$…Ϋtƒώv&t& ‹    ƒώwο…Ϋu[^ιόÿÿÿ΄&    v ‹[@…ΫtVθόÿÿÿX[^Γ΄&    ΄&    θόÿÿÿVS‹T  ‹t$…Ϋtƒώv&t& ‹    ƒώwο…Ϋu[^ιόÿÿÿ΄&    v ‹[D…ΫtVθόÿÿÿX[^Γ΄&    ΄&    θόÿÿÿUWVSƒμ‹\$$‹|$‹l$‹t$ ‰$‹T  …Ϋt'‹[…Ϋt6ÿ4$VUWθόÿÿÿƒΔƒΔ[^_]Γ΄&    v ‹    …ΫuΟƒΔ[^_]ιόÿÿÿΈϊÿÿÿλΡv θόÿÿÿ‹T  …Òt‹B…ΐ•ΐΓ΄&    ‹    …ÒuειόÿÿÿθόÿÿÿUWVSƒμ‹\$$‹|$‹l$‹t$ ‰$‹T  …Ϋt'‹[…Ϋt6ÿ4$VUWθόÿÿÿƒΔƒΔ[^_]Γ΄&    v ‹    …ΫuΟƒΔ[^_]ιόÿÿÿΈϊÿÿÿλΡv θόÿÿÿU‹T  ‰ε…Òt‹RP…Òt]ιόÿÿÿ‹    …Òuθ]ιόÿÿÿΈÿÿÿÿ1Ò]Γ΄&    θόÿÿÿUWVS‰Σƒμ‹\  ‹°T  ‹|$‹¨`  ‰$‹T$ƒβψ…φt,$u1λt& ,$‹5    tμ…φu‰|$‰T$ƒΔ‰Ϊ[^_]ιόÿÿÿv ‹6…φtWR‰ΪθόÿÿÿZYƒΔ[^_]Γ1ΐλτ΄&    t& θόÿÿÿU‰ΝW‰ΧΊ   V‰ΖΈΰ6 SƒμhpΛ  jÿ‹L$ θόÿÿÿY[…ΐt]ÿt$‰ι‰ϊ‰Γÿt$‰πθόÿÿÿ‰ΑXZ…Ιt1‹E ‹U‰K‰;‰C‹D$‰S‰Ϊ‰C‰π‰$θόÿÿÿ‹$ƒΔ‰Θ[^_]Γ‰Ψ‰$θόÿÿÿ‹$λη1Ιλγ΄&    ¶    θόÿÿÿUWVS‰Γƒμ‹D$ ‹|$‹l$‰$‹ƒT  …ΐD    ^ζ   tJ…Ιt<…ΐu"‹$‰|$‰l$‰D$ ƒΔ‰Ψ[^_]ιόÿÿÿt& ‹p…φtÿ4$‰ΨUWθόÿÿÿƒΔƒΔ[^_]Γfλ²΄&    t& θόÿÿÿWVS‰Γƒμd΅    ‰D$1ΐ‹t$(‹|$,‰$‰L$‰ΨΗD$    ‰t$‰|$j WVθόÿÿÿΉ@, Ίΰ6 D$P‰ΨθόÿÿÿƒΔ…ΐu‹D$d+    uƒΔ[^_Γt& λδθόÿÿÿ΄&    ΄&    θόÿÿÿUWVS‰Γ‰Π‹l$‹³T  ‹“`  ‹|$…φD5    “\  t&χΗ   u.ÿ  β πÿÿ…φu&‰Ψ[^_]ιόÿÿÿv 1ΐ[^_]Γ΄&    1ΐλξf‹v…φtW‰ΨUθόÿÿÿZYλΨ1ΐλΤ΄&    θόÿÿÿιfÿÿÿ¶    θόÿÿÿUΑÿ  WΑιVS‹zP‹D$9ΟsS‰Σ‹)ω‹s)Φ‰υΑν9ιr?‹ζ πÿÿÿs(ÿs$ΑιV‹,Ν    ƒεΰ)θ‰Α‰ΨΑωiΙΝΜΜΜωθόÿÿÿƒΔ[^_]Γt& Έϊÿÿÿλο΄&    fθόÿÿÿUWVS‰Λƒμ‹ΈT  ‹L$ ‰<$…ÿt!‹<$‹w…φt.Q‰ΩθόÿÿÿƒΔ[^_]Γ΄&    ‹5    ‰4$…φtc‹4$‹v…φuÒ‹3Βÿ  β πÿÿ‹~‹n‰|$‹<$‰l$‹6ƒζό‰υ‹w…φtQ‰ιÿt$ÿt$θόÿÿÿƒΔ‰Ψθόÿÿÿ‰ΨƒΔ[^_]ιόÿÿÿt& ‹;Βÿ  β πÿÿ‹7Qÿwÿwƒζό‰ρθόÿÿÿƒΔλΑ΄&    v θόÿÿÿU‰ΝW‰ΗV‰ΦS‹T$‹€T  ‰Ρ€α…ΐt^…Ι…φ   ‹X…Ϋt`R‰ψ‰ς‰ιÿt$θόÿÿÿ^‰Γ_…ΐt.ΗC   ‰Ψ[^_]Γ΄&    f‰Ψθόÿÿÿ΄&    f‰Ψθόÿÿÿ1Ϋ‰Ψ[^_]Γ΅    …Ι…“   …ΐu™‹D$% @ …   kΐ8‹T$Ή   ‹€   θόÿÿÿ‰Γ…ΐtΊ‹L$Ί   θόÿÿÿ…ΐu΅‹ÿt$‰ςUH‰ψθύÿÿZY…ΐ„tÿÿÿ‹Ζÿ  ζ πÿÿ‹ƒβ¨uU	ΒΗA    ‰‰q‹‹P‰Pι+ÿÿÿf1Ϋ‰Ψ[^_]Γt& Έ   φD$…`ÿÿÿ1ΐφD$•ΐƒΐιNÿÿÿ¶    ¶    θόÿÿÿV‰ΞS‹€T  …ΐt‹X…Ϋu&‹[^‹ ƒΰόιόÿÿÿ΄&    ΅    …ΐtα‹X…ΫtΪ΅    Βÿ  Ακ%c  ‰Α΅   %   €‰Γ‹FSQΉ   θόÿÿÿZY[^Γ΄&    t& θόÿÿÿ‹T  …Ιt‹A…ΐt‰Πιόÿÿÿv ‹    …ΙuεΓt& θόÿÿÿS‹T  …Ϋt‹[L…Ϋtθόÿÿÿ[Γf‹    …Ϋuζ[ιόÿÿÿΈ   [Γ΄&    fθόÿÿÿ‹T  …Òt‹RT…Òtιόÿÿÿt& ‹    …ÒuειόÿÿÿΈÿÿÿÿΓ΄&    v θόÿÿÿW‰ΟV‹T  …Ιt‹Q8Έ   …Òt!^_Γ¶    ‹    …Ιuα‰ω^_ιόÿÿÿt& ‹A<…ΐ•ΐλΥ¶    θόÿÿÿS1ΐ‹Y9Zt[Γ¶    ‹9tΈ   [Γ΄&    f‹B3A‹R3Q‰ΑΈ   	ΚuΧ[Γ΄&    θόÿÿÿWVS‰Γ‰Π‹t$‹“T  …Òt6‹R…Òt9Αÿ  ‹XPΑι9ΩvG‹P‰Ο+Ακ)ί9ϊw7‹V[^_ιόÿÿÿt& ‹    …Òuΐ‹‹ƒβό‰T$‰Β‰Ψ[^_ιÿϊÿÿ΄&    [Έϊÿÿÿ^_Γ΄&    ΄&    θόÿÿÿW|$ƒδψÿwόU‰εWVS‰ϋ‰Ηƒμ<‰EΜ‹‹s‰EΔ‹C‰uΌ‰EΈd΅    ‰Eμ‹‡T  …ΐD    ƒώ‡Ύ  ‹]Μ‹›X  ‰]ΐ…Ϋ„²  …ΐ…"  ‹Αθ‹Ε    ƒγΰ)Ϊ1ΫΑϊiΒΝΜΜΜ1Ò¤ΒΑΰΑ‹EΜΣ‹°l  ‹€€  ‰EΘ…φ…§   ‹EΘ‰Mδ‰]θ…ΐt‹EΘ€x) …  ‹EΔ1Ò‰MΨ‰]άƒΐÿƒÒÿΘΪ‰EΠ‰UΤ‹Uΐ‹}Μ‹‹r‹—d  ‰Eΰ‹‡h  ‰uΘuΰt‰Η	Χ„ξ   ‹uΘ9Uΰ‰χΗBUΰBΖ‹uΠ‹}Τ9ςψ‚   ‹Eμd+    …ϊ  ‹EΨ‹Uάeτ[^_]gψ_Γv ‹F‹~‰Β	ϊtI‰uΨ‰}Π‰Eΰv ‹EΨ‰ί‹P‹ 9ΑΧr‰Ξ‰ί)ΖΧ;uΰ‰ψEΠ‚   ƒEΨ ‹EΨ‹P‹@‰EΠ	Π‰UΰuΓ‹UΘΗEδÿÿÿÿΗEθÿÿÿÿ…Òt	‹EΘ€x) uƒ=    „ΰ   ÿuΈ‹EΜ‰Κ‰ΩÿuΌÿuΔθόÿÿÿ‰EΨƒΔ‰Uάι8ÿÿÿt& ‹Uΰ‹EΘιÿÿÿt& ¶    ΗEΨÿÿÿÿΗEάÿÿÿÿιÿÿÿv ‹uΨ‰Θ‰Ϊ+FV‹uΘ‰EΨ‰Uά‰Eδ‰Uθ…φt‹EΘ€x) …{ÿÿÿ‹EΔ‹uΨ1Ò‹}άƒΐÿƒÒÿπ‰EΠ‰πϊ!ψ‰UΤƒψÿ…aώÿÿιAÿÿÿ¶    ÿuΈÿuΌÿuΔ‹X ‹EΜθόÿÿÿ‰EΨƒΔ‰Uάιώÿÿ¶    €=–    ΗEΨÿÿÿÿΗEάÿÿÿÿ…`ώÿÿ‹Eΐ‹]ΜΖ–   ‹P‹ ‹³d  ‹»h  ‰EΨ‰Ψ‹[,‰Uά…Ϋu‹‹EΜθόÿÿÿMδWVÿuάÿuΨÿuΔQSPhψ8 θόÿÿÿΗEΨÿÿÿÿƒΔ$ΗEάÿÿÿÿιφύÿÿθόÿÿÿ΄&    ΄&    θόÿÿÿUW‰ΟV‰ΦSƒμ0‹\$D‹L$L‰D$‹€T  ‰\$(‹\$H‰L$‰\$ …ΐtƒ|$ †ί  t& ΅    ƒϋwο…ΐ…Ζ  ‹\$‹ƒl  ‹›€  ‰\$‹\$‰D$,ƒγ ‰\$$…ΐ„Λ  ‹P‹X‰Ρ‰\$	Ω‰T$„μ  ‰$‰έ‰T$΄&    v ‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ9θ‰ΠD$‚™  ƒ$ ‹$‹h‹@‰Β‰D$	κuΒΗ$ÿÿÿÿΗD$ÿÿÿÿ‹D$$‹l$,…ΐ„   ‹L$…Ι„*  ‹|$‹‹W‹<$‹l$9Η‰θΠ‚  ‹D$‰ϊ‰ι;x‰οxƒψ   ‹D$ƒΘ ‰D$L‹D$ ‰D$H‹D$(‰D$D‹D$ƒΔ0[^_]ιόÿÿÿ΄&    v ‹E4‹U0ƒΕ ‰Α‰T$	Ρ‰D$„rÿÿÿ‹E‹U‰ϋ9ΖΣrΧ‰ρ‰ϋ)ΑΣ;L$‰ΨD$sΓu}‹L$…Ιtx‹‹Q‰ϋ9ΖΣ‚Fÿÿÿ‹\$‰ω;sKƒ4ÿÿÿÿt$ ‰ω‰ςÿt$,‹|$ ‰ψθόÿÿÿ‹‡€  ‰D$[^ιχώÿÿ΄&    v ‹X$…Ϋtÿt$‰ς‰ωÿt$$ÿt$0‹D$$θόÿÿÿƒΔƒΔ0[^_]Γt& ‹T$$‰4$‰|$…Ò…ªώÿÿι[ÿÿÿ΄&    ‹$‰ς‰ωPH‰$‰L$ιtώÿÿΗ$ÿÿÿÿΗD$ÿÿÿÿιpώÿÿv θόÿÿÿUW‰ΟV‰ΦSƒμ‹\$(‰D$‹€T  ‰\$‹\$,‰\$…ΐtƒ|$†Χ   t& ΅    ƒϋwο…ΐ…Ύ   ‹D$‹€l  …ΐti‹h‹P‰λ	ΣtF‰$‰T$t& ‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθr%ƒ$ ‹$‹h‹P‰θ‰T$	ΠuΖƒΔ[^_]Γ¶    ‹$px‹D$‹€€  …ΐtΫ;0‰ϋXrÒ;p‰ϋXsΘ‹D$‰ς‰ω‰D$,‹D$‰D$(‹D$ƒΔ[^_]ιόÿÿÿf‹X<…Ϋt›ÿt$‰ς‰ωÿt$‹D$θόÿÿÿXZιÿÿÿ΄&    ¶    θόÿÿÿUW‰ΟV‰ΦSƒμ‹\$(‰D$‹€T  ‰\$‹\$,‰\$…ΐtƒ|$†Χ   t& ΅    ƒϋwο…ΐ…Ύ   ‹D$‹€l  …ΐti‹h‹P‰λ	ΣtF‰$‰T$t& ‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθr%ƒ$ ‹$‹h‹P‰θ‰T$	ΠuΖƒΔ[^_]Γ¶    ‹$px‹D$‹€€  …ΐtΫ;0‰ϋXrÒ;p‰ϋXsΘ‹D$‰ς‰ω‰D$,‹D$‰D$(‹D$ƒΔ[^_]ιόÿÿÿf‹X8…Ϋt›ÿt$‰ς‰ωÿt$‹D$θόÿÿÿXZιÿÿÿ΄&    ¶    θόÿÿÿUΒÿ  Wβ πÿÿ‰ΟV‹°T  S‹L$‹\$‹l$…φt#‹v…φtUSQ‰ωθόÿÿÿƒΔ[^_]Γ΄&    v ‹5    …φuΣ[‰ω^_]ιόÿÿÿ΄&    t& θόÿÿÿUW‰ΟV‰ΦS‰Γ‹€T  …ΐt/‹hL…νt‰ς‰ω‰Ψθόÿÿÿ…ΐt1‰³\  1ΐ‰»`  [^_]Γ¶    ΅    …ΐuΘ‰ΨθόÿÿÿλΡ¶    ΈϋÿÿÿλΦ΄&    fθόÿÿÿUWVS‰Γ‹€X  …ΐtS‰Φ‹“T  ‰Ο…Òt-‹jL…νt‰ς‰ω‰Ψθόÿÿÿ…ΐt/‹ƒX  ‰0‰x1ΐ[^_]Γt& ‹    …ÒuΙ‰ς‰ΨθόÿÿÿλΠv ΈϋÿÿÿλΧθόÿÿÿUWVS‰Γƒμ‹‹j‹r‹z‰$‹ƒT  ‹J…ΐD    Z€ζt+…Ιt…ΐu+U‰ΨWV‹T$θόÿÿÿƒΔƒΔ[^_]Γ΄&    v λΡt& ‹@‰D$…ΐtΫU‰ΨWV‹T$‹|$θόÿÿÿƒΔλΔ¶    θόÿÿÿ‰Π‰ΚΓfffθόÿÿÿUW‰ΟV‰ΦSƒμ‰D$‹€l  …ΐuZ‹D$ ‹L$1Òƒΐÿ‹™d  ƒÒÿπ‹±`  ϊ‹Ή\  ‹‰h  ‰υ	ύu9ΓΡ“ΐƒΔ[^_]Γ‰Ν	έtr9ί‰υΝBίBΞλάt& ‹h‹P‰λ	ΣtE‰$‰T$t& ‹$‹‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθr6ƒ$ ‹$‹h‹P‰θ‰T$	ΠuΗ1ΐƒΔ[^_]Γt& ‰ϋ‰ριoÿÿÿ΄&    ‹$+px‰π!ψƒψÿ… ÿÿÿ1ΐλΙ΄&    ΄&    θόÿÿÿUWVSƒμ‰D$‹€  ‰T$βÿ  ‰L$‰D$…B  ‹|$‹‡`  ‹—\  ‹·d  ‹Ώh  ‰Γ	Σt‰ϋ	σ„F  9ς‰ΓϋBςBψ‹D$‹€l  …ΐtu‹h‹P‰λ	ΣtJ‰$‰T$΄&    f‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθr-ƒ$ ‹$‹h‹X‰θ‰\$	ΨuΖƒ=    @uNƒL$λG¶    ‹$px‹    ƒω@tί1ΐ1ÒφΑ ”ΐ•ΒΣΰΣβƒΐÿƒÒÿ9π‰Πψsΐ1ΐ…ÿ”ΐΑΰ	D$‹l$‹L$‹T$‰θθόÿÿÿ‰Γ…ΐ„   ‹ ‰ήÿt$1ÿΑθ‹Ε    ƒΰΰ)Ζ‰θΑώiφΝΜΜΜ¤χΑζ‰ς‰ωθ~ύÿÿZ„ΐt9ÿt$‹D$‰ς‰ωθhύÿÿZ„ΐ„   ƒΔ‰Ψ[^_]Γt& ‰Φ‰Ηι½ώÿÿ΄&    ‹L$‹D$‰Ϊθόÿÿÿ‹D$ΉÿÿÿÿƒθΑθ½ΠDΡ‹D$ƒΒ‰Αƒψÿt~j ‹D$θόÿÿÿ^‰Γ…ΐt\‹ ‰ή1ÿΑθ‹Ε    ƒΰΰ)ΖΑώiφΝΜΜΜ¤χΑζιYÿÿÿf‹D$‹L$‰Ϊθόÿÿÿ‹D$¨uƒΰϊƒΘ‰D$ιyÿÿÿ΄&    v ƒΔ1Ϋ‰Ψ[^_]Γt& d‹    ιvÿÿÿt& ι·ύÿÿ΄&    fθόÿÿÿU‰Α1Ò‰εWVSƒδψƒμ΅    ‹±l  ƒθ¤ΒΑΰ‰$‰T$…φuC‹D$…ΐ…“   Έÿÿÿÿ½$DΘΈ   1Ò1Ϋ¥ΒΣΰφΑ EΠEΓƒΐÿƒÒÿeτ[^_]Γ¶    ‹~‹^‰ψ	ΨtK‰t$‰\$t& ‹D$‹$‹t$‹‹H‰π9ΣΘr)ΣΞ;\$‰πψr:ƒD$ ‹D$‹x‹p‰ψ‰t$	πuΑΈÿÿÿÿΊÿÿÿÿ½ΘDΚƒΑ ιfÿÿÿ΄&    f‹t$‹N‹^)$\$ι/ÿÿÿ΄&    v θόÿÿÿUWΊÿ  Vη πÿÿS‰Γ‰ϊƒμ‰L$‹L$$‹l$(‰Θ€Μ χΕ   EΘ‰Ψ€εώχΕ   …ι   θ$όÿÿ‰Ζ…ΐ„
  ‹ Αθƒΰƒψt}ƒψth‰πθόÿÿÿ‰D$‹D$‰ω1Ò1ÿθόÿÿÿ‹Αθ‹Ε    ƒΰΰ)Ζ‹ƒl  ΑώiφΝΜΜΜ¤χΑζ…ΐ…7  ‹D$‰0‰x‹D$ƒΔ[^_]Γ΄&    v ƒ=    u΄&    U‹    ‰Ψ‹   βc  α   €θόÿÿÿÿt$$R‰ϊP‰πθόÿÿÿ‰D$ƒΔ…ΐ…Uÿÿÿ‰ω‰ς‰Ψθόÿÿÿλt& θ;ϋÿÿ‰D$…ΐ„  ‹|$‹‰$Αθ‹Ε    ‰ψƒβΰ)Π1ÒΑψiΐΝΜΜΜ¤ΒΑΰ‰Ζ‹ƒl  ‰Χ…ΐ„2ÿÿÿ‹h‹P‰λ	ΣtJ‰$‰T$¶    ‹$‹‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθ‚’   ƒ$ ‹$‹h‹P‰θ‰T$	ΠuΓΎÿÿÿÿΏÿÿÿÿιΝώÿÿt& ‹h‹P‰λ	Σtα‰$‰T$t& ‹$‹‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθr.ƒ$ ‹$‹h‹X‰θ‰\$	ΨuΗΎÿÿÿÿΏÿÿÿÿιiώÿÿ΄&    ‹$+pxιSώÿÿfΗD$    ιMώÿÿv θόÿÿÿUWVS‰Λ‰Ρƒμ‹t$$‹|$(‰T$‰Ϊ‰D$φD$,…²   ‰Ψθόÿÿÿ„ΐ…΄   ‹D$‹€l  …ΐtg‹h‹P‰λ	ΣtD‰$‰T$v ‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθr%ƒ$ ‹$‹h‹X‰θ‰\$	ΨuΖΉΨÿÿÿ1ΐλ-t& ‹$px‰π¬ψ‰Β€ΑκΑα=ÿÿÿ wBΥ    ‹‹D$ƒβΰΚ‹L$ƒΔ[^_]ιόÿÿÿt& ‰Ψθόÿÿÿ‹D$‹€l  …ΐ…Aÿÿÿλ¦1ΐλΑθόÿÿÿUWV‰ΦS‰Γƒμ‰L$‹L$(θΣψÿÿ‰D$…ΐtp‹ ‰$Αθƒΰƒψ„`Ό  ƒψ„Ζ   ‹|$‰ψθόÿÿÿ‰ρ1Òθόÿÿÿ‹‰$Αθ‹Ε    ‰ψƒβΰ)Π1ÒΑψiΐΝΜΜΜ¤ΒΑΰ‰Ζ‹ƒl  ‰Χ…ΐu‹D$‰0‰x‹D$ƒΔ[^_]Γ΄&    ‹h‹P‰λ	ΣtE‰$‰T$t& ‹$‹‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθr>ƒ$ ‹$‹h‹X‰θ‰\$	ΨuΗΎÿÿÿÿΏÿÿÿÿλ‡v ƒ=    …-ÿÿÿι`Ό  ¶    ‹$+pxι^ÿÿÿfθόÿÿÿW‰ΧV‰Ζ‰ΘS‰Λθόÿÿÿ‰ω‰Ϊ‰π[^_ιόÿÿÿ΄&    ¶    θόÿÿÿUWVSƒμ‰D$‰T$‰L$…Ι   ΗD$    ΄&    ‹D$‹p‹x‹D$‹€l  …ΐ„…   ‹h‹P‰λ	ΣtB‰$‰T$‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰ΠθrEƒ$ ‹$‹h‹P‰θ‰T$	ΠuΖ‹D$ƒD$‹\$θόÿÿÿ‰D$9\$…vÿÿÿƒΔ[^_]Γ¶    ‹$px‹D$‹€€  …ΐt»;0‰ϋXr²;p‰ϋXs¨ÿt$,‹D$‰ς‰ωÿp‹D$θόÿÿÿXZλfθόÿÿÿUWVSƒμ‰D$‰T$‰L$…Ι   ΗD$    ΄&    ‹D$‹p‹x‹D$‹€l  …ΐ„…   ‹h‹P‰λ	ΣtB‰$‰T$‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰ΠθrEƒ$ ‹$‹h‹P‰θ‰T$	ΠuΖ‹D$ƒD$‹\$θόÿÿÿ‰D$9\$…vÿÿÿƒΔ[^_]Γ¶    ‹$px‹D$‹€€  …ΐt»;0‰ϋXr²;p‰ϋXs¨ÿt$,‹D$‰ς‰ωÿp‹D$θόÿÿÿXZλfθόÿÿÿUWVSƒμ8‰D$ ‹D$P‰T$‰ΗƒΘ ‰L$,ƒη ΗD$    ‰|$0‰D$4…Ι,  t& ‹D$‹\$ ‹x‹p‰|$$‹x‹ƒl  ‹›€  ‰D$(‰\$…ΐ„  ‹P‹X‰Ρ‰\$	Ω‰T$„Έ  ‰$‰έ‰T$¶    ‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ9θ‰ΠD$‚i  ƒ$ ‹$‹h‹@‰Β‰D$	κuΒΗ$ÿÿÿÿΗD$ÿÿÿÿ‹T$0‹l$(…Ò„–   ‹\$…ΫtK‹|$‹‹W‹<$‹l$9Η‰θΠr3‹t$‰ψ;~‰ο~s#ÿt$4‰Β‰ιÿt$Pÿt$,‹D$,θόÿÿÿƒΔ΄&    ‹D$ƒD$‹|$θόÿÿÿ‰D$9|$,…ΨώÿÿƒΔ8[^_]Γ‹E4‹]0ƒΕ ‰Α‰\$	Ω‰D$„jÿÿÿ‹E‹U‰ϋ9ΖΣrΧ‰ρ‰ϋ)ΑΣ;L$‰ΨD$sΓu}‹\$…Ϋt‹‹S‰ϋ9ΖΣ‚:ÿÿÿ‹\$‰ω;sKƒ(ÿÿÿÿt$L‰ω‰ςÿt$(‹|$(‰ψθόÿÿÿ‹‡€  ‰D$$^_ιοώÿÿ΄&    v ‹L$0‰4$‰|$…Ι…Òώÿÿλv ‹$‰ς‰ωPH‰$‰L$ι¤ώÿÿΗ$ÿÿÿÿΗD$ÿÿÿÿι ώÿÿ΄&    t& θόÿÿÿUWVSƒμ8‰D$ ‰T$(‰L$$d΅    ‰D$41ΐ‰T$ΗD$    …Ι   ι«  v ‹D$‰t$,‰|$0…ΐ„  ‹D$€x) „  ÿt$P‰ω‰ςÿt$Pÿt$‹D$,θόÿÿÿ‹|$ƒΔ‰W!Β‰Gƒϊÿ„4  ‹D$‹@‰D$‹D$‹|$ƒD$‹\$‰xθόÿÿÿ‰D$9\$$„#  ‹|$‹\$ ‹G‹w‰D$‹1ÿ‰$ƒΰό‹Ακ‹Υ    ƒβΰ)Π1ÒΑψiΐΝΜΜΜ¤ΒΑΰΖ‹ƒl  ‹›€  Χ‰\$…ΐ„ ÿÿÿ‹h‹P‰ι	ΡtI‰$‰T$t& ‹$‹‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθ‚"  ƒ$ ‹$‹h‹X‰θ‰\$	ΨuΓ‹T$ΗD$,ÿÿÿÿΗD$0ÿÿÿÿ…Òt‹D$€x) …Ηώÿÿƒ=    …Ίώÿÿ€=—    „P  ‹D$Η@ÿÿÿÿΗ@ÿÿÿÿ‹D$PƒΘ Pÿt$P‹L$ ‹D$(‹T$0θόÿÿÿΈϋÿÿÿY[ιό   ‹D$1Ò‰4$‰|$ƒΐÿƒÒÿπϊ‰D$‰T$‹T$ ‹‚X  ‹‹h‰Π‹€h  ‹’d  ‰\$	λt‰Α	Ρt>‹\$‰ι9ΣΑBΣBΕ‹L$‹\$9ΚΨ‚?ÿÿÿ‹D$‹<$‹l$‰x‰hι,ώÿÿ¶    ‹T$‰θλΚ‹$‹l$‰ρ‰ϋ+HX‰$‰\$‰L$,‰\$0…νt‹D$€x) …°ύÿÿ‹D$‹$1Ò‹\$ƒΐÿƒÒÿΘ‰D$‰ΘΪ!Ψ‰T$ƒψÿ…1ÿÿÿιΈώÿÿt& ‹D$$‹T$4d+    …€   ƒΔ8[^_]Γ‹T$ Ζ—   ‹‚X  ‹j,‹²d  ‹Ίh  ‹‹X…νu‹*‹D$ ‰$‰\$θόÿÿÿWV‹L$‹\$SQÿt$$T$@RUPh\9 θόÿÿÿ‹D$4ƒΔ$Η@ÿÿÿÿΗ@ÿÿÿÿιJώÿÿθόÿÿÿ΄&    ΄&    θόÿÿÿW|$ƒδψÿwόU‰εWV‰ΦS‰ϋ‰Οƒμ,‹‰]Μd‹    ‰Uμ1Ò‰Mθ‰Ω1ΫƒΑÿ‰ς‰uδƒΣÿρϋ!ϊ‰MΠ‰]Τƒϊÿ„™   ‹X  ‹Z‹
‹d  ‰]Ψ‹h  ‰Uΰ‰]ά‰ΛMΨu2‹]Τ‹Uά‹MΠ9MΰΪrb‹Eμd+    uteτ‰π‰ϊ[^_]gψ_Γ¶    ‹Mά	Ρ‰Ϊt'‹Mΰ9Λ‹MΨ‹]ΨMάCUΰC]ά‰Uΰ‰]άλ§΄&    f‰]ΰ‹]Ψ‰]άλ“t& €=P$   „‡Ό  ΎÿÿÿÿΏÿÿÿÿι€ÿÿÿθόÿÿÿ΄&    ¶    θόÿÿÿUWVSƒμ‹€l  ‰$…ΐu0λSt& ‹X‹|$‹H9L$ίr‹t$‹|$)Ξί9Φ‰ϋλrƒΐ ‹h‹P‰λ	ΣuΛ»Ψÿÿÿ1ΐλ5f‹P‹@D$T$‹T$‹D$¬Π‰Β€ΑκΑγ=ÿÿÿ wSΥ    ‹<$‹0Ήΐ  Ί   ‰ψθόÿÿÿ…ΐu*‹ƒζΰή‹ΗA    ƒβ	ς‹t$ ‰–ÿ  β πÿÿ‰QƒΔ[^_]Γ1ΐλ°΄&    ¶    θόÿÿÿΈ   Γt& θόÿÿÿU‰ΑWVSƒμ$‹t$8‹|$<‰D$‹D$D‰T$‰D$‹B+‰Γ‰D$ ‹D$@Αλÿ  ‰\$Αθ‰D$‹l  …ΐ„Κ   ‹h‹P‰λ	ΣtL‰$‰T$΄&    ‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθ‚™   ƒ$ ‹$‹h‹P‰θ‰T$	ΠuΒ»ÿÿÿÿ‹|$‹G$‹W(‰$‰Ρ‰T$‰Βÿt$‹D$θόÿÿÿ‹OP‰G$‰W(^‹t$9ρs])Ξ;t$rU‰D$<‹D$ Ω‰T$@% πÿÿ‰D$8‹ƒΔ$‰ψ[^_]ιόÿÿÿ΄&    v ‰π¬ψ‰Γλ‰¶    ‹$px‰σ¬ϋιoÿÿÿƒΔ$Έϊÿÿÿ[^_]Γ΄&    ΄&    θόÿÿÿUWVS‰Σƒμ‰T$Ίώÿÿÿ9ΪΊ    ‰L$Κ‚Φ   ‹=    ‰ΕWÿ1ÿΑβ‰Φ‹    ƒϊ@t/‰ΡΈ   1Ò1Ϋ¥ΒΣΰφΑ EΠEΓ‰Α‰ΣƒΑÿƒΣÿ9ρ‰ΪϊBρBϋ‹…l  …ΐu ‹D$9t$ψ“ΐƒΔ[¶ΐ^_]Γ΄&    ‹h‹P‰λ	ΣtE‰$‰T$t& ‹$‹‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθr6ƒ$ ‹$‹h‹X‰θ‰\$	ΨuΗΎÿÿÿÿΏÿÿÿÿλƒv ƒΔΈ   [^_]Γv ‹$+pxιbÿÿÿfθόÿÿÿWVS‰Γθόÿÿÿ„ΐu[Έÿÿÿÿ^_Γt& ‹ƒX  …ΐt‹p‹8‰π	ψuΏÿÿÿÿ1φ‹ƒh  ‹“d  ‰Α	Ρt9ϊ‰ΑρBϊBπ‰Ψθόÿÿÿ9ΗΦr‹ƒ€  …ΐtΆ€x) t‰Ψ[^_ιόÿÿÿ΄&    ΄&    θόÿÿÿUW‰ΟV‰ΦSƒμ‰D$‹€l  …ΐtk‹h‹P‰λ	ΣtH‰$‰T$΄&    ‹$‹H‹X‰ψ9ΞΨr‰π‰ϊ)ΘΪ;D$‰Πθr%ƒ$ ‹$‹h‹X‰θ‰\$	ΨuΖ1ÒƒΔ‰Π[^_]Γf‹$px‹D$1Ò‹€€  …ΐtΫ;0‰ϋXrÒ;px’ΒƒΔ[‰Π^_]Γ΄&    t& θόÿÿÿUWV‰ΦS‰Γ‰Π‰Κƒμ‹»l  +D$T$ ‰$‰T$…ÿ…ΏΌ  ‹D$1ν$‰ΟuƒΔ‰θ[^_]Γ΄&    ΅   Ή@   Ίΐ  θόÿÿÿ…ΐt>‹$‹L$‰0‰x‹t$‹|$ ‰P‰p‹t$$‰x‹|$(‰H‰p‰x‰ƒl  ƒΔ‰θ[^_]Γ½τÿÿÿλ‘fθόÿÿÿV‰Θ‰ΦS‰Λθόÿÿÿ„ΐujΓ   @‰ΨΑλΑθ›‹Ε    ƒΰΰΠΉΐ  Ί   ‰πθόÿÿÿ…ΐu*‹L$‹6‘ÿ  ‹β πÿÿƒαφΓu+	ΩΗF    ‰‰V[^Γ΄&    v ‰Ψθόÿÿÿ‰Γλ©t& ΄&    ΄&    θόÿÿÿUWV‰ΦS‰Λƒμ‹zP‰D$‹D$0‰D$‹B+‰ΕΑθ‰D$‹D$,ÿ  Αθ‰$‰Θθόÿÿÿ„ΐ…   Γ   @‰ΨΑλΑθ›‹Ε    ƒΰΰΠ‹N(‹V$ÿt$‹D$θόÿÿÿY‹$‰F$‰V(9ωv^)ω;L$rV‹‰D$(‰θ% πÿÿ‰T$,‹Αι‰D$$‰π‹<Ν    ƒηΰ)ϋ‰ΩΑωiΙΝΜΜΜNPƒΔ[^_]ιόÿÿÿt& ‰Ψθόÿÿÿ‰Γλ†t& ƒΔΈϊÿÿÿ[^_]Γ΄&    t& θόÿÿÿUW‰ΧV‰ΖSƒμ‹¨T  ‰$‹L$…νD-    θόÿÿÿ‰Γ…ΐtGj 1Ι‰Ϊ‰πÿt$W‹m θόÿÿÿ‹L$ƒΔ‰!Π‰Qƒψÿtg‰Ψθόÿÿÿ‰ω1ÒθόÿÿÿƒΔ‰Ψ[^_]Γt& GÿΉÿÿÿÿΑθ½ΠDΡ‹  ƒΒƒωÿtj ‹D$ θόÿÿÿ‰ΓX…ΫuλΎt& d‹    λή΄&    ‰Ϊ‰ω‰π1ΫθόÿÿÿλθόÿÿÿUW‰ΟV‰ΦS‰Γ‹T$‹«T  ‹D$‹L$…νD-    ‹m$…νtj Q‰Ρ‰Β‰ΨVθόÿÿÿƒΔ‰ρ‰ϊ‰Ψ[^_]ιόÿÿÿfffffffθόÿÿÿΈϊÿÿÿΓt& θόÿÿÿUΈÿÿÿÿΊÿÿÿÿ‰ε]Γ΄&    t& θόÿÿÿΈκÿÿÿΓt& θόÿÿÿ1ΐΓffffθόÿÿÿWΏÿÿÿÿVrÿSΑξ‰Γ‰Θ½ΞDΟΎ   ƒΑ9ρOΞΑθƒΰΑκP‰ΨθόÿÿÿZ[^_ΓθόÿÿÿΓ΄&    v θόÿÿÿS»   9ΩGΛ¶\$…ΐt‹€|  …ΐt‰\$[ιόÿÿÿv ΅    ‰\$[ιόÿÿÿθόÿÿÿ…ΐt‹€|  …ΐtιόÿÿÿ΄&    ΅    ιόÿÿÿ¶    θόÿÿÿφΕt&‹€|  …ΐuϊ   v΅    …ΐtιÿÿÿ¶    1ΐΓ΄&    ¶    θόÿÿÿV‰ΦS‹€|  ‰ΛΑÿ  Αι…ΐt2θόÿÿÿ„ΐu!ƒλΈÿÿÿÿΑλ½ΣDΠ‰π[ƒΒ^ιόÿÿÿt& [^Γt& ΅    λΗffffθόÿÿÿUWVS‹    ‹l$‹|$ …Ϋt'‰Φv ‹‹CƒΓW‰ςUÿt$ ÿt$ θόÿÿÿ‹ƒΔ…ΐuή[1ΐ^_]Γ΄&    θόÿÿÿ΅   …ΐEts Γ΄&    t& θόÿÿÿ‹€€  …ΐt‹@…ΐ•ΐΓ΄&    1ΐΓ΄&    ¶    V‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ‹V C@Ή€°  θόÿÿÿPÿvÿvÿvÿvÿv·FΖVh(: Sθόÿÿÿƒ`  ƒΔ$[^ιόÿÿÿ΄&    ΄&    W1ÿ1ΙV‹t$W1ÿV‹t$WVÿt$ ÿt$ θόÿÿÿƒΔ^_Γ΄&    fθόÿÿÿUW‰ΗVS‰Λ‰Ρƒμ(‹·€  ‹D$<‰L$‹V‰\$‰D$‹D$@‰T$‰D$‹)Α‰D$Σ‰Θ¬ΨΑΰF,‹v‹‹X‰t$ ‰$‹4$‰\$#t$‹X‹L$ƒώÿt3‹‡p  ‰Νεÿ  …ΐt+‹4$#p‰κζÿ  )ς9υs€=›    „κ  ƒΔ([^_]Γf‰κ1φ9Ϊ‡|  ‰Π1Ò$T$)λ‰D$‰T$ή;t$s€=™    „ς  ‰t$‹$‹T$¬Π‰ΕΑθ=€   Ε    Έ    BΒT­ +L$L$ ‰$‹ ƒΰΰ‹ΠΑθƒΰƒψt@ƒψt+‹D$-   @ƒ|$„  ‹L$‹$ƒΔ([^_]ιόÿÿÿt& ƒ=    uΜ΄&    ‹t$‹D$ζÿ  …ΐ„ ÿÿÿd΅    ‰D$$λu¶    ƒϋƒ?  …Ϋt‹<$¶φΓ…
  ΄&    v θόÿÿÿd΅    ƒ¨<  Έ   θόÿÿÿd΅    …ΐ„–  χD$   tϋ$ƒΕ1φ)\$„ώÿÿ‹D$»   )σ9ΓGΨD$ϊ‰κ1ÿ‹   Ακϊ€   Υ    T­ CΗα   €‰L$‹ ƒΰΰ<Π‹    Έ   βc  ‰T$ θόÿÿÿ‹D$$ƒ€<  ‹T$ ‹L$‰ψθόÿÿÿΖƒ|$…ÿÿÿƒϋsz…Ϋ„"ÿÿÿ¶‹<$φΓ„ÿÿÿ·Tώ‹<$f‰Tώιÿώÿÿ΄&    ‰Β‹L$‹$ιzώÿÿ΄&    v ‹<$‰ρ‹‰‰<$‹Tό~ƒηό‰Tό‹4$)ω)ΞΩΑισ¥ι³ώÿÿv ‹‹$‰y‹Tόƒηό‰Tό)ω)ΞΩΑισ¥ι‹ώÿÿv €=    …aύÿÿ‹w,Ζ   …φu‹7‰ψ‰$θόÿÿÿ‹$Rÿt$SVPh΄: θόÿÿÿƒΔι(ύÿÿfθόÿÿÿι`ώÿÿ¶    ‹_,Ζ›   …Ϋu‹‰ψθόÿÿÿUVSPh`: θόÿÿÿƒΔιηόÿÿ‹<$·Tώf‰Tώιξύÿÿ¶    ‹_,Ζ™   …Ϋu‹‰ψ‰L$$θόÿÿÿÿt$VSPh; θόÿÿÿƒΔ‰t$‹L$$ιΤόÿÿ΄&    v θόÿÿÿUW‰Η‰ΘV‰ΞSΑζ1Ϋƒμ‰   ‰ρω‰   Σ‹T$ƒ=    ‰4$‰=    ‰   ‰   Η       (   uΖ)   Η        …ΐtV1ΐΎ€   ΄&    f‰Α‰σ‰ΒƒΐƒαΑβ)Λ‹,   ‰\‹,   ΗÿÿÿÿΗDÿÿÿÿ‹,   ΗD    ;   rΊ΅       u   ΐ‹$1Ò‰Ψθόÿÿÿ‰   ƒΔ[^_]Γ΄&    ¶    U‰εW‰ΗV‰ΦSƒμd΅    ‰Eπ‹B,…ΐ„  θόÿÿÿ‰Γ@ΑΰƒΘ$‰Eΰ‹W@‰Uδθόÿÿÿ‹Uδ…    ‹GD‰Uδ…ΐ„Ρ   ƒΓ0MμUθƒγψCό‰Eάθόÿÿÿ‰Γ…ΐt‹U‹Eθ‰P0Ί`   ‰h<f‰P4Η@8    ‹Eΰ‰C‹V,…ÒtsC$θόÿÿÿ‹X  1ΐ1Ò…Ιt‹‹Q‰C‹E‰S‹U‰C‹E‰S‹Mμ‰C‹E‹Uά‰C ‰Ψj ÿuδÿuθj jWθόÿÿÿƒΔ‹Eπd+    uTeτ[^_]Γ΄&    f‹Έ?Μ  …ÒDΠι|ÿÿÿ΄&    ‹…ΐ…%ÿÿÿλΏt& ‹ΗEΰ$  »   …ΐ„ζώÿÿιΞώÿÿθόÿÿÿ΄&    W‰ΧVS‰Γƒμd΅    ‰D$‹C$©ΐ  …Ύ   ‹G,…ΐ„“   θόÿÿÿpH%ΑζƒΞ$‰Ϊ‰ΰθόÿÿÿ‰Γ…ΐtN‰p‹W,…ÒtYC$θόÿÿÿ‹X  1ΐ1Ò…Ιt‹‹Q‰C‹D$,‰S‹T$0‰C‹D$4‰S‰C‹D$8‰C ‰ΰθόÿÿÿ‹D$d+    uRƒΔ[^_Γ‹Έ?Μ  …ÒDΠλ™f‹Ύ$  Ή+   …ΐ„jÿÿÿιTÿÿÿ΄&    φΔ…9ÿÿÿ‰Ψθόÿÿÿ„ΐ„*ÿÿÿλ΅θόÿÿÿθόÿÿÿ‹    % πÿÿƒϊEΠ‰ts Γv θόÿÿÿ΅ΌΊ ΑΰΓfθόÿÿÿƒ=    „®   ‹   …Ι…¨   V‰Ρ‰ΦS‰Γ‰ΠΑΰΑι…΅   ƒθΑθΉÿÿÿÿ½ΠDΡΈΐ  ƒΒθόÿÿÿ£,   …ΐts‰ς‰ΨΑβΑκθόÿÿÿ1Òƒ   @‰ρjθσϋÿÿθόÿÿÿƒ=    ΅   ZtΑΰ% πÿÿ£ts 1ΐ[^Γt& Ηts    1ΐ[^Γ΄&    f1ΐΓt& ΈτÿÿÿΓΈτÿÿÿλΙΈÿÿ ι[ÿÿÿ΄&    ΄&    θόÿÿÿUΑθWƒΐV1φSƒμƒ=    „–   ΑΰΊÿÿÿÿΎ   Ώ   %  όÿƒθΑθ½ΘDΚi‰ιΣζ‰π‰4$Αΰ‰D$ώ   λg΄&    ƒν‰ώ‰ιΣζώ   ~O‰κΈ   θόÿÿÿ‰Γ…ΐtέ‹D$ΏÿÿÿÿƒθΑθ½ΠDΧB9θuL‹$‰Ψθόÿÿÿ‰Ζ…ΐu)ƒΔ‰π[^_]Γ΄&    ƒΔΎτÿÿÿ[‰π^_]Γ΄&    f‰Ψ‰κθόÿÿÿ‰πƒΔ[^_]Γι½  ΄&    θόÿÿÿW|$ƒδψÿwόU‰εWV‰ΦSƒμ\‰Uθ‹‰Eΐ‹Eΐ‰U¬‹W‰Mμ‰U΄‹W‰UΈ‹W‰U ‹€  ‹€p  ‰Uά…ΐ„;  ‹P‰UΔ!ςβÿ  ‰U¨‹}ά…ÿ„~½  ‹U΄9U¬‡+  ‹u΄u¨‰uΌ‹uΐ‹l  ‹uά‹‹N‰UΘ‰MΜ…ΐt‹@‰EΠ…ΐ…!  …Ϋ…  ‹EΘΗEδ    ΗEΨ    ‰Eΰ‹EΔ1Ò% ψÿÿ‰EΔ‹EΌÿ  ƒÒ ¬Π‰E¤…ΐ„  ‹EΔ‹]άΑθƒΐ 9ΠCΠ}Όÿ  GΒ‹UΈΑκƒΒ9ΒCΒ‰EΈ‰ΖC ‰Eθόÿÿÿ‹K‹}¤‰E‰Θ+C‰MΤ9Η‡  ‹C‰}ΠTÿχή1ΐ!ς‹uμ9ΡGΒ‹Uδ‰EΘ‰Γ‹Eΰÿ  ƒÒ ¬Π‰EΌ‹Eθ	Ζ‰ς‹uΨ‰UΨFÿλt& ƒΓ1Ι9]ΤFΩ;]Θ„¶   ‹}Ψ…ÿt‰ΩΑαMΰ3Mθ…MΔuΥ‰Α!ρ‰M°t\΄&    v ‹EΠΗEδ    ‰Eΰ…Ϋ…λ   ‹EΘ‹]ΠΗEδ    !Ψ‰Eΰƒϋÿ„(  ‹EΠ1Òƒΐÿ  ƒÒ ¬Π‰EΨι―ώÿÿt& ‹MΌΩ!ΑMΠ9ρw‹}ά‹UΠ‹O,‰ίΑη;T9†R  ]Έ1Ι9]ΤFΩ;]Θ…Jÿÿÿ‹U‹EθόÿÿÿχE    uGΊ@°  Έ Ί θόÿÿÿ…ΐt4ι1½  ΄&    v ΗE¨    ΗEΔ    ιΓύÿÿt& €=Q$   „W½  »ÿÿÿÿΎÿÿÿÿeτ‰Ψ‰ς[^_]gψ_Γ‹{‹s‰ψ	πtB‰uΨ‰}Τ¶    ‹‹uΘ‹}Μ‹K9Φ‰ψΘr)ΦΟ;uΨ‰ψEΤr_‹C4‹s0ƒΓ ‰Η‰uΨ	χ‰EΤuΚƒ}Πÿ…ΨώÿÿΗEΨ    ι—ύÿÿt& ‹{‹sΗEΰÿÿÿÿΗEδ    ‰ψ	πtΦΗEΠÿÿÿÿ‰uΨ‰}Τλ‡΄&    ‹EΘ‹UΜ+CS‹Mδ‰Φ‹Uΰ‰χ!Π!Ο‰Eΰ‰}δλ”¶    ¶    ‹E¤‰ώ‹}°‹E΄‰Uΰ‰EΨ‰Ψ9ΣsK‰}Θ‹UΨ‰]Τ‹}ΰ‰uΨ‹uάλ΄&    v ‹N,‰ΓƒΐΑγΗD    ‹N,‰Tκ   9ψuά‹]Τ‹uΨ‹}ΘCÿ‰Βƒβƒϊt4‰uΨ‹uάλ¶    ‰Ω)Αƒθ‰J‰Βƒβƒϊt‰ΒΑβV,‹J…Ιuέ‹uΨ‹Mά‹Uΰ1ΐ9QGΒ‹U‰A‹E¤A‹Eθόÿÿÿ‹UΰΑβƒϋÿ„φύÿÿ‰]Ψ‹Mά‰Uΰ΄&    ‹Y,‰ψ1ÒEθUμΗ   ‰3‰T3ƒΖ9uΰuή‹uά‹]Ψ1Ò‹E¨VΑγj‰ήÿu¬ΑώΓ‹EΐΦ‰Ϊ‰ρθ‘ρÿÿXZeτ‰Ψ‰ς[^_]gψ_Γ΄&    ΄&    θόÿÿÿUW‰ΟV‰ΦS‰Γƒμ‹D$,‹T$(‹L$$ƒΰ ƒβύ	Π„4  ‹ƒp  ‹«€  …ΐ„  ‹X!σ‰Ψ1Ϋ%ÿ  ‰Α+u })Ξί¬ώ1ÿ‰ς‰σ‰t$ΑβU,B‰ΖE Ζÿ  ‰$ƒΧ ¬ώ‰t$ήθόÿÿÿ“€   ƒβ€‰D$1ΐ9Φ}‰πΑΰE,‹@ƒξ9σ@‹<$΄&    ‹M,‰ςƒΐƒξΑβ‰D‹M,ΗÿÿÿÿΗDÿÿÿÿ‹M,ΗD    9σ~Ν‰<$ƒλ‹<$D$‰Ϊƒβƒϊuλ*t& ‰Α)Ωƒλ‰J‰Ϊƒβƒϊt‰ΪΑβU,‹J…Ιuέ‰<$‹D$)E‹T$‹$ƒΔ[^_]ιόÿÿÿ1Ι1Ϋ1ΐιυώÿÿt& j‰ς‰ΨQ‰ωθπÿÿXZι·ώÿÿ΄&    t& θόÿÿÿS‹\$ƒϋvƒϋu[Γ΄&    v ΗD$   [ιΒοÿÿf΄&    ΄&    θόÿÿÿS‹\$χΓύÿÿÿtƒϋu[Γ΄&    ΗD$   [ι‚οÿÿf΄&    ΄&    θόÿÿÿW|$ƒδψÿwόU‰εWV‰ώ‰ΗS‰Λ‰Ρƒμ<‰Eΰ‹‰EΔ‹F‰Eΐ‹F‹·l  ‰EΌd΅    ‰Eμ1ΐ‰Π‰Ϊ…φ…V  >t& ÿuΌ‰Κ‰Ωÿuΐj ‹EΔPP‹Eΰθόÿÿÿ‰Α!Π‰ΣƒΔƒψÿ„  ‹Eΰ‹°l  …φ…}  ‹EΔ‰Uθ1Ò‰Mδƒΐÿ‰MΠƒÒÿΘ‰]ΤΪ‰EΘ‰UΜ‹Uΰ‹‚X  ‹8‹p‰Π‹€h  ‹’d  ‰EΨ‰π	ψ…£   ‹uΘ‹}Μ‹EΨ9ςψ‚  ‹Eμd+    …y  ‹EΠ‹UΤeτ[^_]gψ_Γ¶    ‹5    d‹=    £=    ƒ%ÿÿÿdÿ    ‹=    …ÿt"‰MΨ‹‰]άVÿuΔRP‹Uΰ‰ψθόÿÿÿ‹MΨ‹]άƒΔdÿ    …εώÿÿθόÿÿÿιΫώÿÿt& ‹EΨ	Π„%  9Χ‰πEΨBΧ‰χC}Ψ‰}Ψι:ÿÿÿt& ‹F‹~‰Β	ϊtI‰uΨ‰}Θ‰EΠv ‹EΨ‰ί‹P‹ 9ΑΧr‰Ξ‰ί)ΖΧ;uΠ‰ψEΘ‚"  ƒEΨ ‹EΨ‹P‹@‰EΘ	Π‰UΠuΓΈÿÿÿÿΊÿÿÿÿιFώÿÿt& ‹F‹~‰Β	ϊtE‰uΨ‰}Θ‰EΠv ‹EΨ‰ή‹P‹ 9ΑΦr‰Ξ‰ί)ΖΧ;uΠ‰ψEΘrvƒEΨ ‹EΨ‹x‹@‰EΘ	ψ‰}ΠuΗΗEδÿÿÿÿΗEθÿÿÿÿ‹EΌ‰Κ‰ΩƒΘ PÿuΐÿuΔ‹EΰθόÿÿÿƒΔ€=    „‚   ΗEΠÿÿÿÿΗEΤÿÿÿÿιEώÿÿt& ‰uΨ‰ϊι#ώÿÿ¶    ‹uΨ‰Θ‰Ϊ+FV‰EΠ‰Ζ‰Χ‰Eδ‹EΔ‰UΤ‰Uθ1ÒƒΐÿƒÒÿπ‰EΘ‰πϊ!ψ‰UΜƒψÿ…µύÿÿιkÿÿÿt& ‹uΨ‰Θ‰Ϊ+FVι8ύÿÿ‹]ΰΖ   ‹ƒX  ‹³d  ‹»h  ‹P‹ ‰EΨ‰Ψ‹[,‰Uά…Ϋu‹‹EΰθόÿÿÿMδWVÿuάÿuΨÿuΔQSPhά= θόÿÿÿƒΔ$ιÿÿÿθόÿÿÿfθόÿÿÿΈ   Γffθόÿÿÿθόÿÿÿ…ΐtƒxu‹@Γ΄&    1ΐΓ΄&    ¶    θόÿÿÿVΒÿ  Ή   SΑκ‰Γÿt$ÿt$θόÿÿÿZY‰Ζ…ΐtθόÿÿÿ‰X‰π[^Γ΄&    θόÿÿÿUΒÿ  WΑκV‰Χ‰Ζ•    Sƒμ‰$Ίΐ  θόÿÿÿ…ΐ„’   ‰Γ…ÿt_‹‹$Αθ‹Ε    ƒΰΰ)ΖΑώiΖΝΜΜΜ‰ή…    <1Ι)Φ΄&    ‰ΕΑνύ€   ν    ,€CΡ‹ƒβΰκ‰†ƒΐ9ΗuΦÿt$Ή   ‰Ψÿt$‹T$θόÿÿÿ‰Ζ‰ΨθόÿÿÿXZƒΔ‰π[^_]Γ1φλςt& θόÿÿÿS‰Γθόÿÿÿ…ΐtƒxtSh > θόÿÿÿXZ[Γ΄&    ‰Ψ[ιόÿÿÿffffθόÿÿÿUWVS‹    …Ϋt$‰Υ‰Ο΄&    f‹3‹CƒΓ‰ω‰κθόÿÿÿ‹…ΐuι[1ΐ^_]ΓfθόÿÿÿUWVS‹    …Ϋt$‰Υ‰Ο΄&    f‹3‹CƒΓ‰ω‰κθόÿÿÿ‹…ΐuι[1ΐ^_]ΓfU‰εW‰ΧV‰ΖSƒμ‰Mθ‹X@θόÿÿÿ…    ‹FD…ΐtbMπUμΈ   θόÿÿÿ…ΐtA‹M‹Uμ‰J0Ή`   ‰j<f‰J4ΗB8    ‹O,‹}θ‰H‹Mπ‰xj SRΊ   j jVθόÿÿÿƒΔeτ[^_]Γt& ‹…ΐuλλW‰ΗV‰ΦS‰Λƒμ‹@$©ΐ  u*Ή   ‰ϊ‰ΰθόÿÿÿ…ΐt‹V,‰X‰P‰ΰθόÿÿÿƒΔ[^_ΓφΔuΡ‰ψθόÿÿÿ„ΐtΖλζ΄&    ΄&    V‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ÿv ÿvÿvÿvÿvÿvÿvhL> Sθόÿÿÿƒ`  ƒΔ$[^ιόÿÿÿV‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ÿvÿvhυΜ  Sθόÿÿÿƒ`  ƒΔ[^ιόÿÿÿ΄&    ΄&    W1ÿV‰Ξ1ΙWVθόÿÿÿXZ^_Γ΄&    t& θόÿÿÿd΅    ©ÿÿÿud΅    ‹ ¨uΓιόÿÿÿ΄&    t& θόÿÿÿV‰ΖS‰Σƒβ td΅    €Έ   …έ   d΅    ‹€θ  …ΐ…±   φΓu4φΓuφΓ@t
‰π[^ιόÿÿÿƒγtd‹    ‹C ¨u][^Γ΄&    v >t& λΕ΄&    f‹Nd΅    £    sªdÿ    ΅    …ΐt
‹@‰ςθόÿÿÿdÿ    u‡θόÿÿÿλ€t& ΑΰΊ   %€   ƒΘθόÿÿÿ…ΐu7[^d΅    ‹ Γt& ‹V1ΐϊ πÿÿ–ΐθόÿÿÿι7ÿÿÿΖ€   ιWÿÿÿt& Ή   ‰Ϊθόÿÿÿλ»fW1ÿV‰Ξ1ΙWVθόÿÿÿXZ^_Γ΄&    t& θόÿÿÿUW‰ΗVS‰Σƒμƒβ u;φΓuD1φφΓuWƒγ‹o,…£   d΅    ‹θ  …Ι…   …φDυƒΔ‰π[^_]Γθόÿÿÿ„ΐugφΓtΌd‹5    ‹F ¨u$φΓuO1φφΓt©1ΐθόÿÿÿ‰Ζƒψÿu›λ8΄&    ΑΰΊ   %€   ƒΘθόÿÿÿ…ΐud΅    ‹ƒβt¶φ€Ρ  t­Ύÿÿÿÿι|ÿÿÿ¶    >t& ιSÿÿÿ¶    ‹G‹_‹‹O‰D$‹G‰\$‹_‰$‰L$‰D$‰\$„3ÿÿÿP‰θÿt$θόÿÿÿXZι ÿÿÿ΄&    Ή   ‰ςθόÿÿÿιpÿÿÿ΄&    d΅    £    ƒΪώÿÿdÿ    ΅    …ΐt‹@‰ι‰ϊθόÿÿÿdÿ    …±ώÿÿθόÿÿÿι§ώÿÿ΄&    v W‰ΗV‰ΞS‰Σƒμ‹@$©ΐ  uJΉ$   ‰ϊ‰ΰθόÿÿÿ…ΐt0‰p‹P‰H‹C‰B‹C‰B‹C‰B‹C‰B‹C‰B‰ΰθόÿÿÿƒΔ[^_ΓφΔu±‰ψθόÿÿÿ„ΐt¦λζ΄&    ΄&    U‰εW‰ΗV‰ΦSƒμ‰Mδ‹X@θόÿÿÿ…    ‹GD‰]θ…ΐtMπUμΈ$   θόÿÿÿ…ΐt`‹M‹Uμ‰J0Ή`   f‰J4‹Mδ‰j<ΗB8    ‰H‹‹Mπ‰X‹^‰X‹^‰X‹^‰X‹^‰X‹v‰p j ÿuθRΊ$   j jWθόÿÿÿƒΔeτ[^_]Γv ‹…ΐ…wÿÿÿλι΄&    t& θόÿÿÿ‰Α‰Πd‹    ‹RφΒ?uΓ΄&    ‰Θι	ύÿÿ΄&    fθόÿÿÿΓ΄&    v θόÿÿÿW‰ΗVSd‹5    ‹θόÿÿÿχΓ0 uuχΓ  @ …4  θόÿÿÿ€η@…>  >t& ‹5    …φud΅    ‹Έ  …Ϋ…Λ   [^_Γ΄&    θόÿÿÿφΗu.χΓ  u5ƒγu<ϊθόÿÿÿd΅    ‹χΓ0 t‹ϋφΓuΝφΗtÒ‰ψθόÿÿÿχΓ  tΛ‰ψθόÿÿÿƒγtΔπ€&ύ‹†δ  …ΐ…»   ‹†t  …ΐ…½   θόÿÿÿθόÿÿÿd΅    ‹€0  …ΐt‰ϊ1ΐθόÿÿÿι|ÿÿÿt& d‹    1¶ΐ1Αd‰    ιÿÿÿ¶    θόÿÿÿ…ΐ„(ÿÿÿ‹    …Ι…ÿÿÿhΝ  h0Ν  θόÿÿÿXZιÿÿÿt& θόÿÿÿθόÿÿÿ€η@„Ηώÿÿt& θόÿÿÿιΈώÿÿ¶    θόÿÿÿι;ÿÿÿ¶    ‹†t  θόÿÿÿΗ†t      ι)ÿÿÿ΄&    ΄&    θόÿÿÿƒμd‹    ‹RφΒvu	ϊƒΔι ώÿÿ‰$θΘωÿÿ‹$λκfθόÿÿÿVS‰Γd‹5    ƒμ$d΅    ‰D$ 1ΐ‹C0+†  ;†  s 1ΐ‹T$ d+    …Έ   ƒΔ$[^Γ΄&    ‰Ψθόÿÿÿ„ΐuΥ‹†   …ΐtθόÿÿÿ…ΐue„ÒtΎ€ϊupΖ†  ‹K,1Ò1ΐ‰K‰Tƒΐƒψrτd΅    ‹@Η$   ΗD$   ‹€δ  ΗD$  @‰L$‰D$‰ΰθόÿÿÿΈ   ι_ÿÿÿΈ   θόÿÿÿΈ   ιKÿÿÿΈ   θόÿÿÿΈ   ι7ÿÿÿθόÿÿÿθόÿÿÿS…ΐtfƒψu(…Òu‹D$…ΐt%|$ÿÿÿΏΈςÿÿÿv[Γt& 
9ΠwάΈκÿÿÿ[Γf‹\$d΅    ‰   ‰  ‰  Ζ€   π€H 1ΐ[Γt& L$	ΡuΏd΅    Η€       Η€      Η€      Ζ€   π€`ί1ΐ[ΓffθόÿÿÿUWVSƒμ(‹H‹d‹    ‰T$$1Ò‹p‰L$‹H‰L$‹H‰$θόÿÿÿ‰Ψθόÿÿÿ‰Γ‰πθόÿÿÿ…Ϋ„  ‰Ζ…ΐ„w  C‰D$Έ   πΑC…ΐ„V  P	ΒΛ  F‰D$Έ   πΑF…ΐ„J  P	ΒΏ  θόÿÿÿ‹†¨  ¨΄  ‹ƒ¨  ΄  9Υ†Ί   ‰θ‰T$θόÿÿÿ‹T$…ΐ‰Η…ύ   ‰Πθόÿÿÿ‰Η…ΐ…ε   Ί   ‰Ψθόÿÿÿ„ΐ„¬   Ί   ‰πθόÿÿÿ„ΐ„   ‹$ƒψ„ά  ‡†  ƒψ„M  ƒψ…<  ΅x$  ‹“  ‹|$  1Β3†  ―Ρ΄&    ―Α1Ι9ΒΑΐ<	¶ΐ	Ηλ>΄&    f‰Π‰T$θόÿÿÿ‰Η…ΐuK‹T$9Υ„Sÿÿÿ‰θ‰Υ‰Βι7ÿÿÿ΄&    Ώÿÿÿÿ‹†¨  ‹‹¨  ΄  ©΄  9θtθόÿÿÿ‰θθόÿÿÿΈÿÿÿÿπΑCƒψt=…ΐ¥  ΈÿÿÿÿπΑFƒψt6…ΐ¦  ‹D$$d+    …Ξ  ƒΔ(‰ψ[^_]Γv ‰ΨθόÿÿÿλΒ΄&    ‰πθόÿÿÿλΙ΄&    ‹D$Ί   θόÿÿÿι"ώÿÿt& ‹D$Ί   θόÿÿÿι.ώÿÿt& …ΐ„¨  ƒψ…e  ΅h$  ‹“ΰ  ‹l$  1Β3†ΰ  ―Ρι¶ώÿÿ¶    ‹$ƒψ„μ  ƒψ…¥   ‹T$Ή   D$θόÿÿÿ…ΐ…  θόÿÿÿ‹T$‰Ψθόÿÿÿ‰Εθόÿÿÿ…ν„α  ‹T$‰πθόÿÿÿ‰Η…ΐ„Μ  ‹T$‹L$ θόÿÿÿ‰$‰ψθόÿÿÿ‹$‰Χϊ πÿÿ‡zώÿÿ΅d$  3-`$  3`$  ―θ―Π9Υΐ¶ΐ< ΐ¶ΐ	ΗιKώÿÿƒψ…{  ΅$  ‹“P  ‹$  1Β3†P  ―ΡιΜύÿÿt& ‹D$Ί   θόÿÿÿιHώÿÿt& ‹D$Ί   θόÿÿÿιGώÿÿt& ‹D$Ί   θόÿÿÿιΆόÿÿt& ‹D$Ί   θόÿÿÿι®όÿÿΏςÿÿÿιΈύÿÿv ΅€$  ‹“¬  ‹„$  1Β3†¬  ―Ρι?ύÿÿ΄&    θόÿÿÿ‹T$‰Ψθόÿÿÿ‰Ηθόÿÿÿθόÿÿÿ‹T$‰πθόÿÿÿ‰Εθόÿÿÿ…ÿ„   …νt}΅d$  3=`$  3-`$  ―ψ―θ9οιΤώÿÿ΅p$  ‹“  ‹t$  1Β3†  ―ΡιΏόÿÿ΄&    ΅$  ‹“  ‹”$  1Β3†  ―Ρι—όÿÿ΄&    θόÿÿÿΏύÿÿÿι+ύÿÿΏχÿÿÿιΜόÿÿθόÿÿÿΏκÿÿÿι½όÿÿffffθόÿÿÿχ@ € €t
1ΐΓ΄&    S‰Γ‹ ©   u€=     tΈ   [Γt& 1ΐ[Γt& ‰Ψθόÿÿÿ„ΐuβ€=     tδ‹C[ΑθƒπƒΰΓ΄&    ΄&    θόÿÿÿU‰ΕWV1φSd‹    ‹{λ΄&    θόÿÿÿΎ   Έ   ‡CΈ€s θόÿÿÿ΅    K   …ΐu,cÿÿώÿΈ€s θόÿÿÿd΅    φ@u·‡x[‰π^_]Γt& ‰Ψθόÿÿÿ„ΐtΙ‰θ„ΐtΚθόÿÿÿ„ΐuΊλΏ΄&    t& θόÿÿÿSd‹    θόÿÿÿΈ€s θόÿÿÿcÿÿÿΈ€s θόÿÿÿθόÿÿÿ΅    …ΐu1ΐ[Γ‰Ψθόÿÿÿ„ΐtπ1ΐ[ιÿÿÿ΄&    ¶    θόÿÿÿWVS‰Γƒμd΅    ‰D$1ΐπƒD$ό 1φφC@t!‹D$d+    …   ƒΔ‰π[^_Γ΄&    Έ€s θόÿÿÿ‰Η΅    …ΐu‰ϊΈ€s 1φθόÿÿÿλΊv ‰Ψθόÿÿÿ‰Ζ„ΐtΰ‹C©   uΦ©    u:‰β‰Ψθόÿÿÿ…ΐt1Ò‰Ψθόÿÿÿ‹ƒ¬  ‹$θόÿÿÿ‰ϊΈ€s θόÿÿÿιdÿÿÿt& Ί   ‰Ψθόÿÿÿλάθόÿÿÿt& θόÿÿÿS‰ΓΈ€s ƒμθόÿÿÿ‰ΒφCuƒΔΈ€s [ιόÿÿÿt& ‰$‰Ψθόÿÿÿ‹$Έ€s ƒΔ[ιόÿÿÿffθόÿÿÿS‰ΓΉ   ΊΠ°  ƒμ‰$d΅    ‰D$1ΐ‰Ψθόÿÿÿ…ΐ„½  ‹$Ή   ΊΔ°  θόÿÿÿ…ΐt@‹$Ή   Ίΐ°  θόÿÿÿ…ΐt`T$‰ΰθόÿÿÿ…ΐ…½  ‹D$d+    u;ƒΔΈ   [ΓΗ       ‹$€x,uƒΐ	‰$T$‰ΰθόÿÿÿ…ΐ„κ½  ιΟ½  θόÿÿÿΗ       ‹$€x,uƒΐ‰$T$‰ΰθόÿÿÿ…ΐ„Ύ  ιΎ  θόÿÿÿπ«„s 1ΐΓθόÿÿÿθόÿÿÿ1Ι‰ΒΈ a  ‹•    ƒ< ”Α‰Γ΄&    t& θόÿÿÿ‰Π1ΙΊP~ ιόÿÿÿ΄&    ¶    θόÿÿÿh„s ÿ5    hjΝ  Pθόÿÿÿ1ΐƒΔΓ΄&    ΄&    θόÿÿÿUWVSƒμ‰$d΅    ‰D$1ΐƒω„ό   ΈtΏ » a  θόÿÿÿΈ   θόÿÿÿΈpΝ  θόÿÿÿd΅    ‹…    ‹,Έ   θόÿÿÿd΅    …ΐ„ι   h    Ί ~ 1ΐ1ΙjΎa  »ÿÿÿÿθόÿÿÿXZλ7¶    ®    ‹1ΐyΗ    Ηό      ƒηό)ωΑ   Αισ«‰ΨΊ    θόÿÿÿ‰Γ;    rΉΈtΏ θόÿÿÿ΅s 1Ò…    ΅s θόÿÿÿ‹$‹T$d+    uQƒΔ[^_]Γ΄&    Ή   D$θόÿÿÿ‰ΒΈςÿÿÿ…ÒuΛ‹D$θόÿÿÿ‰ΒΈκÿÿÿ…Ò„Σώÿÿλ±t& θόÿÿÿιÿÿÿθόÿÿÿ΄&    fθόÿÿÿUWVSƒμ‹D$0‰T$‰L$·s ‰D$‰Γ‹3» a  d΅    ‰D$1ΐΈ   Σΰ‰D$ΈtΏ θόÿÿÿΈ   θόÿÿÿΈpΝ  θόÿÿÿd΅    ‹…    ‹‰$Έ   θόÿÿÿd΅    …ΐ„V  h    Ί ~ 1ΐ1ΙjΏÿÿÿÿ»a  θόÿÿÿXZ‰t$΄&    ‰ψΊ    θόÿÿÿ‰Η;    sM‹$ƒ½    ‹ ¨   λ!v ‹s π4Η     Η@    ƒΐ9θt²‹p‹…φuΩ…ΙtμΗ     λδΈtΏ ‹t$θόÿÿÿ΅s …   1ΐ9ςvO‹D$)ς9ΒGΠ‰Χ…Ò„¨   ƒώ‡   ‹l$λƒο‰ΣƒΖƒΕ)ϋƒώw.…ÿt*¶D4‰ιθόÿÿÿ…ΙtΪΈςÿÿÿ‹T$d+    uYƒΔ[^_]Γ‰l$΅s t0ό…ÿx<Ή   ‰ϊ‰πθόÿÿÿ‹D$‰ω‰ςθόÿÿÿ…ΐu¶;‹|$‰ΓΑϋ_λ¨θόÿÿÿι ώÿÿλ•θόÿÿÿv 1Ϋλ§t& θόÿÿÿWV‰ΖSπ³„s 1ΫΏa      µ    ψ‹…Òt/‹Β   @‰ΡΑι‹Ν    Η     ‰Π1ÒΑθƒαΰ€Αθόÿÿÿƒϋu[1ΐ^_Γ΄&    »   λΆ΄&    fθόÿÿÿUWΏa  V1φS‰ΓΈ    ƒμ‹    ‹,Έ a  Η    ·    ‹…Ιt%ƒώu1ΐƒΔ[^_]ΓΎ   ·    ‹…ΙuΫj 1Ò‰ιΈΐ  θόÿÿÿZ…ΐt‹    ϊ‰$θόÿÿÿ‹$‰²λ­t& ‰ΨθαώÿÿΈτÿÿÿλ΅΄&    v θόÿÿÿU-    WVS‰Γƒμ·s ΅s ‰T$Σλƒθ9ΓGΨέ    %ψ  ‰D$‰Η‰ΨΑΰƒπψ%ψ  ‰D$Έ   θόÿÿÿΈpΝ  θόÿÿÿΈa  d‹    ‹•    Ί a  ‹‹,…ν„   D$ϊE@‰$4ύ    ‰κD5 4$΄&    ‹H;ta…Ι„‘   ƒΐ9Ζuκ|$ηÿ  9|$uΓ΅s ‹|$π<Ε   ΄&    ‹
΅s ‹ZπΗB    ƒΒΗBψ    9Υuέλ
v L$‰HχD$   tϋΈ   θόÿÿÿd΅    …ΐtƒΔ[^_]Γ΄&    v ‹|$‰‰xλΔθόÿÿÿƒΔ[^_]Γ΄&    θόÿÿÿS9    u΅s …ΐt‰Π[‰Κιoώÿÿ΄&    [Γ¶    θόÿÿÿS‰Π‰Λƒμd‹    ‰T$1ÒΗ$    ‰Ϊj L$θόÿÿÿZ…ΐt‹T$d+    uƒΔ[Γv ‹$‰Ψ‰„s λήθόÿÿÿ¶    θόÿÿÿV‰ΖΈpΝ  Sθόÿÿÿd‹    ·C4ƒΰfƒψu	[^Γ¶    θόÿÿÿ£„s sι‰Ψθόÿÿÿ;5    uΪ‹s …ÒtΠ[Ί   ^ιύÿÿ΄&    ¶    θόÿÿÿh ±  1ΙΊ€  Έ»Ν  θόÿÿÿXΓfθόÿÿÿ‹H;Hs3VS‹X…Ϋu‹q‰p‰‹‹p9p[’ΐ^Γv ƒλ‰XΈ   [^Γf1ΐΓ΄&    ¶    θόÿÿÿV‰Ζ‰ΠS…Òt1Ι΄&    ‹ϋ    rϋ    sA[^Γ΄&    ϋ    rϋ    rγƒΑ9ΘuΙ[^Γ΄&    ΄&    θόÿÿÿSƒΑƒμd‹    ‰\$1Ϋ‰$Έ°… ‰T$‰L$ΗD$    j d‹    T$θόÿÿÿ‹D$Z‹T$d+    uƒΔ[Γθόÿÿÿθόÿÿÿ…ΐt…ÒuΓfΓWVSSι7Ύ  t& θόÿÿÿUWV‰ΖSƒμ‰$…Ιtk‰Σ…Òtg‹D$‹T$ …ΐ•ΐƒΒ1ν1ÿ‰T$„ΐtK‰π‰ξ‰Εv ‹$ÿ4Έj ÿt$hαΝ  SUθόÿÿÿΖƒΔ9ΨrƒΔ‰ρ[‰Θ^_]Γv Ε)ΓƒΗ9|$wΓλβƒΔ1Ι[‰Θ^_]ΓfθόÿÿÿV‰ΦS‰Γ‰Πθόÿÿÿ…ΐt
Έ   [^Γf‹S;Ssσ‹C…ΐu!‹J‰K‰4‹C9C[’ΐ^Γ΄&    v ƒθ‰CΈ   [^ΓfθόÿÿÿVS‰Γƒμd΅    ‰D$1ΐΗD$    d΅    9Γ‰$‹“X  ”ΐ‰L$‹X  ¶ΐD$ ‰D$…Òt[r‰Ππ±1uX	ςx<‹C…ΐtEj ‰ΩΈp‡ T$θόÿÿÿ‰Ψθόÿÿÿ‹D$Z‹T$d+    u&ƒΔ[^Γt& 1Ò‰ΘθόÿÿÿλΉt& 1ΐλΥt& ‰Βλ•θόÿÿÿ΄&    θόÿÿÿSƒμd‹    ‰\$1Ϋ‰$‹T$‰L$‰T$ΗD$    PΈ°… d‹    T$θόÿÿÿ‹D$Z‹T$d+    uƒΔ[Γθόÿÿÿθόÿÿÿƒμd‹    ‰L$1Ι‰$‰T$1ÒΗD$    ΗD$    d΅    φ@ u‹H‰βΈ°… Α΄  θόÿÿÿ‹T$‹D$d+    u‰ΠƒΔΓθόÿÿÿffθόÿÿÿΓ΄&    v θόÿÿÿiΐθ  Γt& θόÿÿÿU‰εW‰ΟJώVS‰Γƒμ…ΙJ
ƒλ‰ΨΎ…λQΑθ‰$‰Ψχζ‰Φ‰ΠiΡo  Αθ$ΑξΗΉ«ªªª‰ΠχαiΛm  1ΫΑκ1ÒΏ   )πΎ<   ΘΪ1ΫkΚχηΚ‹MΘΪ1ΫkΚ<χζΚ‹MΘΪ1ΫkΚ<χζΚ‹MΑ€Ώ±†ƒΣρΘΪƒΔ[^_]ΓθόÿÿÿUWVΎÿΙ;S‰Λ‰Ρƒμ‰$‹D$‹T$9ΖΎ    Φ};ΏÿΙ;1φ¶     6eΔ‰υƒÒÿƒΑƒΣ 9ΗΥ|κ…Òy¶     Κ;ƒÒ ƒΑÿƒΣÿ…Òxξ‹<$‰‰_‰GƒΔ[^_]Γv θόÿÿÿ…ΐΊώÿÿ?HΒΓθόÿÿÿ=0ψÿÿwη  ΊΣMbχβ‰ΠΑθΓfΈώÿÿ?Γ΄&    v θόÿÿÿWΊLb VS‹8‹p9ϊΊ    ςrK‹@Ί½7†iφ   ϊ?B ‰ΓχβΑϋiΫ½7†Ϊ¬ΠΑκ‰ΑΈ   ϊ‰ΣχηςΑΣ¬Ω[^‰Θ_Γ΄&    [ΈΘόÿ?^_Γ΄&    ΄&    θόÿÿÿUW1ÿV‰ΦS»@B χγ‰Α‰ΥϊÿΙ;vΑκ	ΏƒK ‰Πχη‰ΧΑοiΗ Κ;)Ε[‰~‰Θ‰κΉ Κ;χρ‰‰V^_]Γ΄&    ΄&    θόÿÿÿVS»@B χγ‰Α‰Φϊ– v‰πΊkΚ_kχβΑκiÒ€– )Φ‰Θ‰ςΉ€– [χρ^Γt& θόÿÿÿ=™™w€ΐΓ¶    ΈÿÿÿÿΓfθόÿÿÿUiΚ@B ‰εWΏ@B χηVS1ΫΡ‰Ζω– v‰ΘΊkΚ_kχβ‰ΣΑλiΓ€– )Α‰π‰ΚΉ€– χρ‰Ϊ[^_]ΓfθόÿÿÿiΚ@B Ί@B Uχβ‰ε]ΚΓ΄&    θόÿÿÿU‰ε]Γ¶    θόÿÿÿU‰Ρ‰εV‰ΖS1Ϋϊ?B v‰ΘΊƒήCχβ‰ΣΑλiΓ@B )Α‰π‰ΚΉ@B χρ‰Ϊ[^]ΓθόÿÿÿS‰Ρ‰Γϊ?B v‰ΘΊƒήCχβΑκiÒ@B )Ρ‰Ψ‰Κ[Ή@B χρΓ΄&    ¶    θόÿÿÿWV‰Ζ1ΐ9ΠS‰ΣΘ}L‰Ο1ΙÿÿΙ;w ‰Ψ‰ϊ» Κ;χσ‰Γ‰‰π[‰N‰V^_Γt& ‰ϊΉƒK Ακ	‰ΠχαΑκiΒ Κ;‰Ρ)ΗλΓv ‰Θ1Ò	ΨtΕ‰ΟχΣ1ΙχΧÿÿΙ;v‰ωΊƒK Αι	‰Θχβ‰ΡΑιiΑ Κ;)Η‰Ψ‰ϊ» Κ;χΡχσχΠ‰ΓΈÿΙ;)Π‰Βι{ÿÿÿ΄&    ΄&    θόÿÿÿS‰Σƒμ‹Hd‹    ‰T$‹‹@‰L$Ή   ‰D$Αψ‰D$‰Ψ‰$‰βθόÿÿÿχΨΐƒΰς‹T$d+    uƒΔ[Γθόÿÿÿt& θόÿÿÿSΉ   ‰Γƒμd΅    ‰D$1ΐ‰ΰθόÿÿÿ…ΐu)‹$‹T$‰‹D$‰S‰C1ΐ‹T$d+    uƒΔ[ΓfΈςÿÿÿλεθόÿÿÿt& θόÿÿÿƒμ‰Α‰Πd‹    ‰T$‹‰$‹QΉ   ‰T$‰βθόÿÿÿχΨΐƒΰς‹T$d+    uƒΔΓθόÿÿÿt& θόÿÿÿSΉ   ‰Γƒμd΅    ‰D$1ΐ‰ΰθόÿÿÿ…ΐu)‹$‰Αψ‰C‹D$‰C1ΐ‹T$d+    uƒΔ[Γv Έςÿÿÿλδθόÿÿÿt& θόÿÿÿV‰ΦΉ   S‰Γƒμd΅    ‰D$‹‰β‰$‹C‰D$‰πθόÿÿÿ…ΐu:‹CΉ   ‰β‰$‹C‰D$FθόÿÿÿχΨΐƒΰς‹T$d+    uƒΔ[^Γt& Έςÿÿÿλβθόÿÿÿt& θόÿÿÿVΉ   ‰ΖS‰Σƒμd΅    ‰D$1ΐ‰ΰθόÿÿÿ…ΐuV‹$SΉ   ‰Αψ‰F‹D$‰F‰ΰθόÿÿÿ…ΐu1‹$‰FΑψ‰F‹D$‰F1ΐ‹T$d+    uƒΔ[^Γ΄&    fΈςÿÿÿλέθόÿÿÿt& θόÿÿÿV‰ΦΉ   S‰Γƒμ‹Sd΅    ‰D$‹‰T$‰β‰$‹C‰D$Αψ‰D$‰πθόÿÿÿ…ΐuL‹C‹SΉ   ‰$‹C‰T$‰β‰D$Αψ‰D$FθόÿÿÿχΨΐƒΰς‹T$d+    uƒΔ[^Γ΄&    Έςÿÿÿλήθόÿÿÿt& θόÿÿÿVΉ   ‰ΖS‰Σƒμd΅    ‰D$1ΐ‰ΰθόÿÿÿ…ΐuV‹$‹T$Ή   ‰‹D$‰VS‰F‰ΰθόÿÿÿ…ΐu0‹$‹T$‰F‹D$‰V‰F1ΐ‹T$d+    uƒΔ[^Γ΄&    Έςÿÿÿλίθόÿÿÿt& θόÿÿÿV‰Ζ1ΐ‰Ρ9πSΠ}MϊÿΙ;w%» Κ;‰Κ‰πχσ‰Ρ‰Γ‰ΘΊΣMbΑωχκΑϊ)Κ‰Ψ[^ΓΑκ	»ƒK ‰ΠχγΑκiÒ Κ;)ΡλΒ΄&    ‰Θ1Ò1Ϋ	πtΡχΡχΦωÿΙ;v‰Κ»ƒK Ακ	‰ΠχγΑκiÒ Κ;)Ρ‰Κ‰πΉ Κ;χρΉÿΙ;χΠ‰Γ)Ρλƒ΄&    fθόÿÿÿS‹θόÿÿÿ…Ϋt‰ΩθόÿÿÿΊςÿÿÿ…ΙEΒ[Γ΄&    t& θόÿÿÿSƒμ‹ d‹    ‰T$1Òθόÿÿÿ‰$Αϊ‰T$…ΐuE1Ò‰ΰΗD$    θόÿÿÿ‰Γ…ΐt‹D$d+    u(ƒΔ‰Ψ[Γ΄&    v ‰ΰθόÿÿÿλΩ΄&    »ςÿÿÿλΛθόÿÿÿt& θόÿÿÿWVSƒμ‹8‹pd‹    ‰T$1Ò…ÿt8‰ΰθόÿÿÿ‰ω‹$θόÿÿÿ…ΙuZ‹\$ΊΣMbO‰ΨΑϋχκ‰ΠΑψ)Ψθόÿÿÿ…Ιu7…φu1ΐ‹T$d+    u+ƒΔ[^_Γt& Ή   Ί    ‰πθόÿÿÿ…ΐtΠv ΈςÿÿÿλΘθόÿÿÿt& θόÿÿÿV‰ΦS‰Γƒμ…ΐt"‹‹@…ΐx}{ÿΙ;wtΉt]ν9ΡΊ   Β|d‰ς‰Ψθόÿÿÿ…ΐu\…φtc‰$‹„  ϊ  w@‹‹V£    ‰   θόÿÿÿ΅Ώ …ΐ‹$t/ΗΏ     …Ϋt1ƒΔ‰Ψ[^ιόÿÿÿt& ΈκÿÿÿƒΔ[^Γt& …ΫuΫƒΔ[^Γ¶    ‰$θόÿÿÿ‹$λΨ΄&    t& θόÿÿÿVSƒμ‹‹Xd‹    ‰T$1Ò…Ι„|   ‰Θθόÿÿÿ‰T$Αϊ‰T$…ΐu~Aθόÿÿÿ…ΐurϊ@B wxiÒθ  ‰T$…Ϋt4Ή   ‰Ϊ‰ΰθόÿÿÿ…ΐuJ‰βD$θόÿÿÿ‹T$d+    uGƒΔ[^Γt& 1ÒD$λά΄&    …Ϋt‰ΪΉ   ‰ΰθόÿÿÿ‰β…ΐtΌΈςÿÿÿλΊ1Ò1ΐλ®Έκÿÿÿλ¬θόÿÿÿ΄&    ΄&    θόÿÿÿWS‰Γ{‰Ωƒηόμ„   )ωd΅    ‰„$€   1ΐΑΠ   Η    ΗƒΜ       Αισ«Ή€   ‰ΰθόÿÿÿ…ΐ…)  ‹$‰‹D$‰CΑψ‰C‹D$‰CΑψ‰C‹D$‰CΑψ‰C‹D$‰C Αψ‰C$‹D$‰C(‹D$‰C0Αψ‰C4‹D$‰C8Αψ‰C<‹D$ ‰C@Αψ‰CD‹D$$‰CHΑψ‰CL‹D$(‰CPΑψ‰CT‹D$,‰CXΑψ‰C\‹D$0‰C`Αψ‰Cd‹D$4‰ChΑψ‰Cl‹D$8‰Cp‹D$<‰CxΑψ‰C|‹D$@‰ƒ€   Αψ‰ƒ„   ‹D$D‰ƒ   Αψ‰ƒ   ‹D$H‰ƒ   Αψ‰ƒ”   ‹D$L‰ƒ   Αψ‰ƒ   1ΐ‹”$€   d+    uΔ„   [_Γ¶    ΈςÿÿÿλΪθόÿÿÿt& θόÿÿÿWΉ   VS‰Γμ„   d΅    ‰„$€   1ΐt$T‰χσ«‹Ή€   ‰$‹B‰D$‹B‰D$‹B‰D$‹B ‰D$‹B(‰D$‹B0‰D$‹B8‰D$‹B@‰D$ ‹BH‰D$$‹BP‰D$(‹BX‰D$,‹B`‰D$0‹Bh‰D$4‹Bp‰D$8‹Bx‰D$<‹‚€   ‰D$@‹‚   ‰D$D‹‚   ‰D$H‹‚   ‰D$L‹‚    ‰β‰D$P‰ΨθόÿÿÿχΨΐƒΰς‹”$€   d+    u
Δ„   [^_ΓθόÿÿÿV‰ΖS‰ςμΤ   d΅    ‰„$Π   1ΐ‰ΰθόÿÿÿ‰Γ…ΐu‰ΰθόÿÿÿ‰Γ‰β‰πθόÿÿÿ…ΐEΨ‹„$Π   d+    uΔΤ   ‰Ψ[^Γθόÿÿÿθόÿÿÿ‹ λ—΄&    θόÿÿÿU‰Ρ‰εV‰ΖS1Ϋϊ– v‰ΘΊkΚ_kχβ‰ΣΑλiΓ€– )Α‰π‰ΚΉ€– χρ‰Ϊ[^]ΓθόÿÿÿUWVSƒμ‹\$4‹|$(‰D$‹L$<L$0‹t$8‹l$,‰Θ‰|$™‰$ί»ÿΙ;‰l$υ9ΛΉ    Ρ‰t$‰ώ‰ο}41Ιt&  6eΔ‰ΝƒÒÿƒΖƒΧ 9ΓΥ|κ…Òy¶     Κ;ƒÒ ƒΖÿƒΧÿ…Òxξ‹$‹L$‹\$‹l$9Ρ‰ΪT$‹$MλMΡ9Φ‰ϊκ|‹T$‰B‰Π‰2‰zƒΔ[^_]Γ΄&    1ΐΎÿÿÿÿΏÿÿÿλΤfθόÿÿÿVS‹    …Ϋt‰Φt& ‹‹CƒΓ‰ςθόÿÿÿ‹…ΐuλ1ΐ[^Γ΄&    ΄&    θόÿÿÿUWVS‹    …Ϋt)‰Υ‰Ο΄&    f‹C‹3ƒΓÿt$‰κ‰ωθόÿÿÿ‹X…Òuδ[1ΐ^_]Γ΄&    ¶    θόÿÿÿUWVS‹    …Ϋt$‰Υ‰Ο΄&    f‹3‹CƒΓ‰ω‰κθόÿÿÿ‹…ΐuι[1ΐ^_]ΓfθόÿÿÿVS‹    …Ϋt‰Φt& ‹‹CƒΓ‰ςθόÿÿÿ‹…ΐuλ1ΐ[^Γ΄&    ΄&    θόÿÿÿVS‹    …Ϋt‰Φt& ‹‹CƒΓ‰ςθόÿÿÿ‹…ΐuλ1ΐ[^Γ΄&    ΄&    θόÿÿÿUWVS‹    …Ϋt)‰Υ‰Ο΄&    f‹C‹3ƒΓÿt$‰κ‰ωθόÿÿÿ‹X…Òuδ[1ΐ^_]Γ΄&    ¶    θόÿÿÿUWVS‹    …Ϋt$‰Υ‰Ο΄&    f‹3‹CƒΓ‰ω‰κθόÿÿÿ‹…ΐuι[1ΐ^_]ΓfθόÿÿÿUWVS‹    …Ϋt$‰Υ‰Ο΄&    f‹3‹CƒΓ‰ω‰κθόÿÿÿ‹…ΐuι[1ΐ^_]ΓfθόÿÿÿVS‹    …Ϋt‰Φt& ‹‹CƒΓ‰ςθόÿÿÿ‹…ΐuλ1ΐ[^Γ΄&    ΄&    θόÿÿÿVS‹    …Ϋt‰Φt& ‹‹CƒΓ‰ςθόÿÿÿ‹…ΐuλ1ΐ[^Γ΄&    ΄&    θόÿÿÿUWVSƒμ‹    …Ϋt1‹t$‹|$ ‰$‰L$v ‹C‹+ƒΓWV‹L$‹T$θόÿÿÿ‹XZ…ΙuαƒΔ1ΐ[^_]Γ΄&    ΄&    θόÿÿÿUWVSƒμ‹    …Ϋt1‹t$‹|$ ‰$‰L$v ‹C‹+ƒΓWV‹L$‹T$θόÿÿÿ‹XZ…ΙuαƒΔ1ΐ[^_]Γ΄&    ΄&    θόÿÿÿUWVS‹    …Ϋt$‰Υ‰Ο΄&    f‹3‹CƒΓ‰ω‰κθόÿÿÿ‹…ΐuι[1ΐ^_]ΓfθόÿÿÿS‰Γ)Σƒϋ>wP‰Πƒΰ?‰[Γt& ϋχ  wƒΐ[‰ΒΑθƒβψƒΰ?ƒΐ@‰Γv ϋΏ  v(ϋÿ}  w8   ‰ΒΑθ	ƒΰ?β ώÿÿΐ   λ­¶    ƒΐ@[‰ΒΑθƒβΐƒΰ?ƒθ€‰Γv ϋÿο v(ϋÿ w@ €  ‰ΒΑθƒΰ?β €ÿÿ@  ιbÿÿÿv    ‰ΒΑθƒΰ?β πÿÿ   ιBÿÿÿv ϋÿÿϋ w   ‰ΒΑθƒΰ?β  όÿ€  ιÿÿÿϋÿÿίw    ‰ΒΑθƒΰ?β  ΰÿΐ  ιυώÿÿ…Ϋx,Β   >ϋÿÿÿ>GΒ   ‰ΒΑθƒΰ?β   ÿ   ιΕώÿÿ‰Πƒΰ?ι»ώÿÿt& θόÿÿÿUWΏΐj  V‰ΖS‰ύƒμ‰$λ¶    ‰ψθόÿÿÿσ‹^χΓ   uσ‰ΪΈ@a  βÿÿ ‹•    <*χΓ   u<‰ψθόÿÿÿ‰Β‹$‰9^u·ƒΔ‰ψ[^_]Γ΄&    ¶    U‰εW‰ΧV‰ΖSƒμ‹^@d΅    ‰Eπ1ΐθόÿÿÿ…    ‹FD…ΐtbMμUθΈ   θόÿÿÿ…ΐt8‹Uθ‹M‰J0Ή`   f‰J4‹Mμ‰j<ΗB8    ‰xj SRΊ   j jVθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γf‹…ΐuλβθόÿÿÿv U‰εW‰ΧV‰ΖSƒμ‰Mδ‹^@d΅    ‰Eπ1ΐθόÿÿÿ…    ‹FD…ΐtMμUθΈ   θόÿÿÿ…ΐtS‹M‹Uθ‰J0Ή`   ‰j<f‰J4ΗB8    ‰x‹O‹}δ‰H‹    ‰x‰H‹M‰H‹Mμj SRΊ   j jVθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γt& ‹…ΐ…wÿÿÿλάθόÿÿÿ΄&    ΄&    U‰εW‰ΧV‰ΖSƒμ‰Mδ‹^@d΅    ‰Eπ1ΐθόÿÿÿ…    ‹FD…ΐtMμUθΈ   θόÿÿÿ…ΐtM‹M‹Uθ‰J0Ή`   f‰J4‹    ‰j<ΗB8    ‰x‰H‹O‹}δ‰H‹Mμ‰xj SRΊ   j jVθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γ΄&    v ‹…ΐ…wÿÿÿλΦθόÿÿÿ΄&    ΄&    U‰εW‰ΧV‰ΖSƒμ‰Mδ‹^@d΅    ‰Eπ1ΐθόÿÿÿ…    ‹FD…ΐtoMμUθΈ   θόÿÿÿ…ΐtD‹M‹Uθ‰J0Ή`   f‰J4‹M‰j<ΗB8    ‰x‹}δ‰H‹Mμ‰xj SRΊ   j jVθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γv ‹…ΐu‹λαθόÿÿÿv U‰εW‰ΧV‰ΖSƒμ‰Mΰ‹^@d΅    ‰Eπ1ΐθόÿÿÿ…    ‹FD‰]δ…ΐ„   MμUθΈ$   θόÿÿÿ…ΐt^‹M‹Uθ‰J0Ή`   ‰j<f‰J4ΗB8    ‰x‹O‰H‹O‹_‰H‰X‹O‹_‹}ΰ‰H‹Mμ‰X‰x j ÿuδRΊ$   j jVθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γf‹…ΐ…nÿÿÿλήθόÿÿÿ΄&    ΄&    U‰εW‰ΟV‰ΦS‰Γƒμ‹S@d΅    ‰Eπ1ΐ‰Uδθόÿÿÿ‹Uδ…    ‹CD‰Uδ…ΐtwMμUθΈ   θόÿÿÿ…ΐtM‹M‹Uθ‰J0Ή`   f‰J4‰ρ‰j<ΗB8    ‰p‹7‹‰p‰x‹I‰H‹Mμj ÿuδRΊ   j jSθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γf‹…ΐuƒλβθόÿÿÿv U‰εW‰ΧV‰ΖSƒμ‹^@d΅    ‰Eπ1ΐθόÿÿÿ…    ‹FD…ΐtbMμUθΈ   θόÿÿÿ…ΐt8‹Uθ‹M‰J0Ή`   f‰J4‹Mμ‰j<ΗB8    ‰xj SRΊ   j jVθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γf‹…ΐuλβθόÿÿÿv U‰εW‰ΟV‰ΖSƒμ‰Uΰ‹^@d΅    ‰Eπ1ΐθόÿÿÿ…    ‹FD‰]δ…ΐ„   MμUθΈ$   θόÿÿÿ…ΐt`‹M‹Uθ‹]ΰ‰J0Ή`   f‰J4‹M‰j<ΗB8    ‰X‹]‰H‰X‹O‰H‹O‰H‹‰H‹O‰H ‹Mμj ÿuδRΊ$   j jVθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γ‹…ΐ…nÿÿÿλΰθόÿÿÿ΄&    ΄&    U‰εW‰ΟV‰ΖSƒμ‰Uδ‹^@d΅    ‰Eπ1ΐθόÿÿÿ…    ‹FD…ΐtMμUθΈ   θόÿÿÿ…ΐtU‹M‹Uθ‰J0Ή`   f‰J4‹Mδ‰j<ΗB8    ‹U‰H‹M‰P1Ò‰H…ÿt‹W4‰P‹MμΊ   j Sÿuθj jVθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γf‹…ΐ…wÿÿÿλήθόÿÿÿ΄&    ΄&    U‰εW‰ΧV‰ΖSƒμ‰Mδ‹^@d΅    ‰Eπ1ΐθόÿÿÿ…    ‹FD…ΐtoMμUθΈ   θόÿÿÿ…ΐt>‹M‹Uθ‰J0Ή`   f‰J4‹Mμ‰j<ΗB8    ‰x‹}δ‰xj SRΊ   j jVθόÿÿÿƒΔ‹Eπd+    ueτ[^_]Γ΄&    f‹…ΐu‹λΫθόÿÿÿv V‰ΦS‰Γƒμd΅    ‰D$‹C$©ΐ  u3Ή   ‰Ϊ‰ΰθόÿÿÿ…ΐt
‰p‰ΰθόÿÿÿ‹D$d+    uƒΔ[^Γt& φΔuΘ‰Ψθόÿÿÿ„ΐt½λΧθόÿÿÿ΄&    fW‰ΗV‰ΞS‰Σƒμd΅    ‰D$‹G$©ΐ  uPΉ   ‰ϊ‰ΰθόÿÿÿ…ΐt#‰X‹S‰p‰P‹    ‰P‹T$,‰P‰ΰθόÿÿÿ‹D$d+    u ƒΔ[^_Γ΄&    φΔu«‰ψθόÿÿÿ„ΐt λΣθόÿÿÿ΄&    fW‰ΗV‰ΞS‰Σƒμd΅    ‰D$‹G$©ΐ  uHΉ   ‰ϊ‰ΰθόÿÿÿ…ΐt‹    ‰X‰P‹S‰p‰P‰ΰθόÿÿÿ‹D$d+    uƒΔ[^_Γ¶    φΔu³‰ψθόÿÿÿ„ΐt¨λΤθόÿÿÿW‰ΗV‰ΦS‰Λƒμd΅    ‰D$‹G$©ΐ  u@Ή   ‰ϊ‰ΰθόÿÿÿ…ΐt‹T$,‰p‰X‰P‰ΰθόÿÿÿ‹D$d+    uƒΔ[^_Γ¶    φΔu»‰ψθόÿÿÿ„ΐt°λΤθόÿÿÿ΄&    fW‰ΗV‰ΞS‰Σƒμd΅    ‰D$‹G$©ΐ  uXΉ$   ‰ϊ‰ΰθόÿÿÿ‰Α…ΐt+‰X‹C‰A‹C‹S‰A‰Q‹C‹S‰q ‰A‰ΰ‰Qθόÿÿÿ‹D$d+    uƒΔ[^_Γt& φΔu£‰ψθόÿÿÿ„ΐtλΥθόÿÿÿW‰ΗV‰ΞS‰Σƒμd΅    ‰D$‹G$©ΐ  uHΉ   ‰ϊ‰ΰθόÿÿÿ…ΐt‰X‹~‹6‰x‰p‹S‰P‰ΰθόÿÿÿ‹D$d+    u ƒΔ[^_Γ΄&    φΔu³‰ψθόÿÿÿ„ΐt¨λΣθόÿÿÿV‰ΦS‰Γƒμd΅    ‰D$‹C$©ΐ  u3Ή   ‰Ϊ‰ΰθόÿÿÿ…ΐt
‰p‰ΰθόÿÿÿ‹D$d+    uƒΔ[^Γt& φΔuΘ‰Ψθόÿÿÿ„ΐt½λΧθόÿÿÿ΄&    fW‰ΧV‰ΖS‰Λƒμd΅    ‰D$‹F$©ΐ  uXΉ$   ‰ς‰ΰθόÿÿÿ…ΐt/‰x‹t$,‹|$0‰p‰x‹S‰P‹S‰P‹‰P‹S‰P ‰ΰθόÿÿÿ‹D$d+    uƒΔ[^_Γv φΔu£‰πθόÿÿÿ„ΐtλΧθόÿÿÿW‰ΗV‰ΦS‰Λƒμd΅    ‰D$‹G$©ΐ  uPΉ   ‰ϊ‰ΰθόÿÿÿ…ΐt$‰p‹|$01Ò‹t$,‰x‰p…Ϋt‹S4‰P‰ΰθόÿÿÿ‹D$d+    uƒΔ[^_Γ¶    φΔu«‰ψθόÿÿÿ„ΐt λΤθόÿÿÿ΄&    fW‰ΗV‰ΦS‰Λƒμd΅    ‰D$‹G$©ΐ  u8Ή   ‰ϊ‰ΰθόÿÿÿ…ΐt‰p‰X‰ΰθόÿÿÿ‹D$d+    uƒΔ[^_Γt& φΔuΓ‰ψθόÿÿÿ„ΐtΈλΥθόÿÿÿV‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ÿvhιΝ  Sθόÿÿÿƒ`  ƒΔ[^ιόÿÿÿfV‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ÿvÿvÿvÿvhX? Sθόÿÿÿƒ`  ƒΔ[^ιόÿÿÿ΄&    fV‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ÿvÿvÿvÿvh„? Sθόÿÿÿƒ`  ƒΔ[^ιόÿÿÿ΄&    fV‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ÿvhσΝ  Sθόÿÿÿƒ`  ƒΔ[^ιόÿÿÿfW‰ΚVS‰Γ‹°x   θόÿÿÿƒψt	[^_Γt& ‹~ ΉΣMb‰ψΑÿχιΑϊ)ϊRÿv‹~‰ψΑÿχιΑϊ)ϊRÿvÿvÿvÿvh¨? Sθόÿÿÿƒ`  ƒΔ$[^_ιόÿÿÿV‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ÿvÿvÿvÿvhÿΝ  Sθόÿÿÿƒ`  ƒΔ[^ιόÿÿÿ΄&    fV‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ‹Nh ²  C@ΊΞ  α  < θόÿÿÿ‹V‰Α‹FQ‰Α%ÿÿ ΑιQP‰Π+FPRÿvÿvhθ? Sθόÿÿÿƒ`  ƒΔ([^ιόÿÿÿ΄&    U‰ΚWVS‰Γ‹°x   θόÿÿÿƒψt[^_]Γv k@‹VΉ@²  ‰θθόÿÿÿ‹VΉ ²  ‰Η‰θθόÿÿÿWPÿvh0@ Sθόÿÿÿƒ`  ƒΔ[^_]ιόÿÿÿ΄&    t& V‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ‹V C@Ή ±  θόÿÿÿPÿvÿvÿvÿvÿvÿvhP@ Sθόÿÿÿƒ`  ƒΔ$[^ιόÿÿÿfV‰ΚS‰Γ‹°x   θόÿÿÿƒψt
[^Γ΄&    ‹VC@Ή`±  θόÿÿÿPÿvhΞ  Sθόÿÿÿƒ`  ƒΔ[^ιόÿÿÿ1Ιιόÿÿÿ΄&    fW1ÿVS‹t$‰Λ1ΙW1ÿVWSθόÿÿÿƒΔ[^_ΓW1ÿVS‹t$‰Λ1ΙW1ÿVWSθόÿÿÿƒΔ[^_ΓW1ÿV‰Ξ1Ιÿt$ÿt$WVθόÿÿÿƒΔ^_Γv W1ÿV‰Ξ1ΙWVθόÿÿÿXZ^_Γ΄&    t& W1ÿV‰Ξ1ΙWVθόÿÿÿXZ^_Γ΄&    t& W1ÿV‰Ξ1ΙWVθόÿÿÿXZ^_Γ΄&    t& W1ÿV‰Ξ1ΙWVθόÿÿÿXZ^_Γ΄&    t& θόÿÿÿ‹@ιόÿÿÿv 1Ιιόÿÿÿ΄&    fW1ÿV‰Ξ1Ιÿt$ÿt$WVθόÿÿÿƒΔ^_Γv θόÿÿÿUWVS1Ϋƒμ‹h‰D$½ÿÿÿ?ΗD$    ‰|$xλDt& )π…ΐx#¶L$(Σβ‹L$9ΚIΡ‰T$‹$χΪƒβ9ΠvwΑνƒ<$ƒέÿƒD$ϋ@  t`‰ξ‰\$1Ò‰ψƒζ?j ήj ƒΓ@V‰Ωθόÿÿÿ‰ιƒα‰L$ƒΔ9Γw’j ‰ρ1Ò‰ψj ÿt$θόÿÿÿƒΔ9Ζv΅Ί@   )ςΠιlÿÿÿ΄&    v ‹|$‹\$‹GΖG ‰$ÿÿÿ?9Ψ‰Ψ•GƒΔ[^_]Γ΄&    fθόÿÿÿU‰ΑWVSd΅    ‹    <@ΊΣMb4ώ‰πχβΑκiκθ  ‹    )ξ…θ  ώω   FΕϋϊ‰Ζ)ή9Β[HΞ^_]‰ΘΓt& θόÿÿÿΈ΄Ώ θόÿÿÿ‹    Έ    …Ιt
‹    …Òu θόÿÿÿΈ¤s θόÿÿÿΈ΄Ώ ιόÿÿÿ΄&    θόÿÿÿλήθόÿÿÿW‰ΧV‰ΞS‰Γ>t& ΗC    ‰{χΖÿÿΗÿuSd΅    	π‰C[^_Γt& d΅    £    sΜdÿ    ΅    …ΐt
‹@‰Ϊθόÿÿÿdÿ    u©θόÿÿÿλΆ΄&    ζ  8 λ£΄&    ΄&    θόÿÿÿVS‰Γ‰Π‹S…Ò„   ‹‰Ξ…Òtr>t& ‹‹S‰…ΐt‰P‰π„ΐtΗC    Η"  Έ   [^Γt& d΅    £    sΔdÿ    ΅    …ΐt
‹@‰Ϊθόÿÿÿdÿ    u΅θόÿÿÿλ΄&    ‹SΑκL`9K…{ÿÿÿ³PΖ@ιnÿÿÿ1ΐ[^Γ΄&    t& θόÿÿÿWV1φS‰Γƒμd΅    ‰D$‹C…ΐt%‰β‰ΨθλÿÿΉ   ‰Β‰Η‰Ψθÿÿÿ‹$‰Ζ‰ψθόÿÿÿ‹D$d+    u	ƒΔ‰π[^_ΓθόÿÿÿθόÿÿÿV‰ΖSƒμd΅    ‰D$1ΐ‰β‰πθ―κÿÿ‰Γ9pt8‰ΒΉ   ‰πθªώÿÿ‰Ζ‹$‰Ψθόÿÿÿ‹D$d+    uƒΔ‰π[^Γ΄&    fΎÿÿÿÿλΡθόÿÿÿt& θόÿÿÿW‰ΗVSƒμd΅    ‰D$1ΐd΅    ©   t
φG t\t& ‰β‰ψθ'κÿÿ‰Γ;xt8‰ΒΉ   ‰ψθ"ώÿÿ‹$‰Ζ‰Ψθόÿÿÿ…φx"‹D$d+    u!ƒΔ‰π[^_Γt& ‹$θόÿÿÿσλ¬t& λ¤θόÿÿÿ΄&    ΄&    θόÿÿÿUWV‰Ζ‰ΘS‰ΣT`‹|$‹
‰…Ιt‰Y‰‰S«F‹SΑΰβÿÿ? 	Π‰C>t& ;~xR[^_]Γt& ‹Kd‹    £    sήdÿ    ‹    …Òt‹jP‰Ϊ‰θθόÿÿÿXdÿ    u¶θόÿÿÿλ―¶    ‰~ΖFΖF >t& [^_]Γv φCu€~ t‹F[^_]ιόÿÿÿ΄&    θόÿÿÿW‰ΧV‰ΖSd‹    γÿÿÿ>t& ‰πθόÿÿÿ>t& d΅    %ÿÿÿ9Ψt6€=    „Β   ΄&    v d‹    ‰Κ‰Θβ   €	Ϊd±    9Αuα[^_Γt& d΅    £    s—dÿ    ΅    …ΐt
‹@‰ςθόÿÿÿdÿ    …pÿÿÿθόÿÿÿιfÿÿÿ΄&    d΅    £    ƒWÿÿÿdÿ    ΅    …ΐt
‹@‰ςθόÿÿÿdÿ    …0ÿÿÿθόÿÿÿι&ÿÿÿt& Ζ   PSWh@ θόÿÿÿƒΔι*ÿÿÿ΄&    v UWV‰ΖS‰υƒμ4d΅    ‰D$0‰πθόÿÿÿ΅    ;F¶   ¶    ‹    ‹E9Β   u`‰E}1Ò‰t$L$‰l$‰<$1ÿv ‰Γ‹4$ƒγ?Σ³s ‹t$i4‹‰…Ϋt‰KΗ    ƒΗ‰ι¨uƒΒ@Αθϊ@  uΏ‹l$‹Eƒΐ…ÿuQ€} „O  ‰E‰θθΝψÿÿ‰E‹E΄&    ‹    9Β‰Rÿÿÿ‰ξ‰πθόÿÿÿ‹D$0d+    …!  ƒΔ4[^_]Γ‰E‰θθ†ψÿÿ‰|$tΌ‰E‹E΄&    ‹xÿ…Òu,ι‚   fθόÿÿÿ‹$‰ω‰Ψθύÿÿ‰θθόÿÿÿ‹ΗE    …ΐtX‹‰]>t& ‹‹S‰…ΐt‰P‹SφC ΗC    ‰θΗ"  ‰$u¨θόÿÿÿ‹$‰ω‰Ψθ7ύÿÿ‰θθόÿÿÿ‹ΗE    …ΐu¨‹Eƒξƒl$…bÿÿÿιÿÿÿt& d΅    £    sdÿ    ΅    …ΐt
‹@‰Ϊθόÿÿÿdÿ    …aÿÿÿθόÿÿÿιWÿÿÿ€} „§ώÿÿι ώÿÿθόÿÿÿfθόÿÿÿS»@a  θόÿÿÿ‹    …    ;Sx‰Ψθ·ύÿÿθόÿÿÿ»ΐj  ‹    …    ‰Ψ;Sx	[ι“ύÿÿv [Γ΄&    ΄&    θόÿÿÿW‰ΑV4RΊΣMbS0‰ΨχβΑκiΒθ  ‰Ϊ)Βƒθ  ‹    ‰Ησ)χΣ)Χ9Γ[^HΟ_‰ΘΓ΄&    fθόÿÿÿW‰ΑVSRΊΣMb4‰πχβΑκiϊθ  ‹    )ώ‡θ  ώω   FΗΪ‰Ζ)ή9Β[HΞ^_‰ΘΓ¶    θόÿÿÿW‰ΑVSd΅    @ΊΣMb4‰πχβΑκiϊθ  ‹    )ώ‡θ  ώω   FΗΪ‰Ζ)ή9Β[HΞ^_‰ΘΓθόÿÿÿW‰ΑVSd΅    4@ΊΣMb1‰ΨχβΑκiΒθ  ‰Ϊ)Βƒθ  ‹    ‰Ησ)χΣ)Χ9Γ[^HΟ_‰ΘΓv θόÿÿÿU‰ΑW<RΊΣMbVS‹    4ώ‰πχβΑκiκθ  ‹    )ξ…θ  ώω   FΕϋϊ‰Ζ)ή9Β[HΞ^_]‰ΘΓ΄&    v θόÿÿÿW‰ΑV‹5    SRΊΣMbπ<‰ψχβΑκiΒθ  ‰ϊ)Β‡θ  ‹=    ίΧ9Ηyή)π)Π‰Α[‰Θ^_ΓθόÿÿÿW‰ΑVSd΅    ‹5    @ΊΣMb1<‰ψχβΑκiΒθ  ‰ϊ)Β‡θ  ‹=    ίΧ9Ηyή)π)Π‰Α[‰Θ^_Γ΄&    fθόÿÿÿWVS‰Γƒμd΅    ‰D$‹C…ΐ…Ύ   ‹K…Ι„³   Έ@a  ‹4•    ‰ΧφCtΈΐj  Ζ‰β‰Ψθβÿÿ9πt K   θόÿÿÿ‰πθόÿÿÿ‹C%  ψÿ	ψ‰C΅    ‹V‰Α)Ρ…Ι~‹N9ΘxD9ΡxX‰N‰Κ‹CL$θοΰÿÿÿt$‰Ϊ‰Α‰πθ€ψÿÿ‹T$‰πθόÿÿÿX‹D$d+    u#ƒΔ[^_Γ‰F‰ΒλΎ΄&    f¶    λ©θόÿÿÿ΄&    ΄&    UWVS‰Γƒμ$‰$d΅    ‰D$ 1ΐ‹C…ΐ„…  ‰ΞφΑ…Β  ‹C…ΐ„·  ΗD$   ‹C+$„~  ‰Οƒη‰|$t…ΐk  T$‰Ψθ…αÿÿ‰Η΅    ‹W‰Α)Ρ…Ι~‹O9Θ   9Ρ   ‰O‹C…ΐ„Ά  ‹l$…νt‹$;C‰d  ‹GL$‰Β‰D$‹$θΙίÿÿ‰D$‰Β‹CΑθ9Β„5  t& 1Ι‰ϊ‰Ψθυÿÿ‰D$…ΐuƒζ‰ύ…Β   ‹kλχΕ   „±  θόÿÿÿΎ@a  ε   ‹…    …  Ζ‰υ9χtV‰ύ9_„a  K   ‰ψ‰υθόÿÿÿ‰πθόÿÿÿ‹C%  ψÿF‰C΅    ‹V‰Α)Ρ…Ι~‹N9Θ1  9Ρ©  ‰N‹$ƒ|$ÿ‰C‹Vt
;T$„L  ‹$L$θΰήÿÿÿt$‰Ϊ‰Α‰πθqφÿÿX‹T$‰θθόÿÿÿ‹D$ d+    …Λ  ‹D$ƒΔ$[^_]Γ΄&    T$‰Ψθυίÿÿ‰Η΅    ‹W‰Α)Ρ…Ι   ‹O9ΘxX9Ρx4‰OΗD$ÿÿÿÿΗD$    ι¬ώÿÿt& θόÿÿÿΎΐj  4…    ιΰώÿÿfΗD$ÿÿÿÿΗD$    ιyώÿÿ΄&    f‰GΗD$ÿÿÿÿΗD$    ιXώÿÿ΄&    ΗD$ÿÿÿÿΗD$    ι;ώÿÿv ¶    ‰FιÒώÿÿ΄&    θόÿÿÿ‰ΒΈ@a  ε   ‹4•    „MώÿÿΈΐj  ιCώÿÿ΄&    ÿt$‹L$‰Ϊ‰πθ/υÿÿZιΉώÿÿ΄&    f‹$‰CΗD$   ‰ύι›ώÿÿv ‰GιcύÿÿιSώÿÿ΄&    f‹GL$‰Β‰D$‹$θ;έÿÿ‰D$‰Β‹CΑθ9Β…wύÿÿ‹L$‰ύ…Ιt‹$;Cx•ΗD$   ι8ώÿÿ΄&    ιόόÿÿ΄&    f‰ώιγύÿÿθόÿÿÿt& θόÿÿÿΉ   ιAόÿÿθόÿÿÿ1Ιι4όÿÿt& θόÿÿÿΉ   ι!όÿÿθόÿÿÿ‹P…Òu‹PΉ   ιόÿÿ΄&    ΄&    ΄&    θόÿÿÿSθόÿÿÿƒΐt d‹    ΄&    fΗC   θόÿÿÿ…ΐuπ[Γ΄&    ΄&    θόÿÿÿSd‹    θόÿÿÿƒΐu λ7΄&    ‹ƒβuΗC   θόÿÿÿ…ΐt‹β   tί[ιόÿÿÿ΄&    f1ΐ[ιόÿÿÿ΄&    θόÿÿÿ‹    Ή¤Ώ Έ    ιόÿÿÿ¶    θόÿÿÿW‰ΟV‰ΦS‰ΓΈ΄Ώ θόÿÿÿÿt$‰ω‰ςÿt$‰Ψθόÿÿÿ_Z‰Γ…ΐu…φuΈ΄Ώ θόÿÿÿ‰Ψ[^_Γ΄&    f‹    Έ    …Ιt
‹    …ÒuθόÿÿÿΈ΄Ώ θόÿÿÿ‰Ψ[^_Γfθόÿÿÿλ°΄&    fθόÿÿÿU‰εWV‰ΖS»@a  ƒδψƒμ‰$‰L$θόÿÿÿ…    θόÿÿÿ£    ƒC  ‰Ψθόÿÿÿ€{ …  ‹C‹S‰ρ)Α9ςΨ   …Ι‹   ‹4$‹|$ΖC ‰Ψθόÿÿÿθόÿÿÿ9π‰ΡωsW‹4$‹|$9Ζ‰ωΡsH?B ƒÒ 1ÿ‰Ζ‰Ρ…ÒtΏƒήC‰Πχη‰Π‰ΧΑθΑοiΐ@B )Α»@B ‰π‰Κχσ‰ϊiΚ@B χγ‰Χ‰ΖΟeτ‰π‰ϊ[^_]Γ¶    ‰s€{ ugΈÿÿÿÿΊÿÿÿ+$T$‰ΡΎÿÿÿÿ‰ΒΏÿÿÿΈ@B 9ΠΈ    ΘƒGÿÿÿΖCι>ÿÿÿv …Ιx΄9Β‰$ÿÿÿ‰Cιÿÿÿt& ‰Ψθ	νÿÿ‰Cιςώÿÿ)πΉ@B ‹4$‹|$χα‰ΡΖ‰ΒΟλ¶    eτΎÿÿÿÿΏÿÿÿ[‰π‰ϊ^_]Γ΄&    v θόÿÿÿS»@a  θόÿÿÿ‹…    ΖD [ΓfθόÿÿÿVΎ@a  S‰Γ΅    d‹    1ΠΨd‹    Αΐ‰ΚΘΑΒ1Π‰Ϊd£    ‰Θθόÿÿÿθόÿÿÿ4…    θόÿÿÿ΅    ;Fy΅    ;†	  x
Έ   θόÿÿÿ‰Ψθόÿÿÿd΅    ©   uθόÿÿÿ[^ιόÿÿÿ¶    θόÿÿÿθόÿÿÿ[^ιόÿÿÿ΄&    θόÿÿÿ‰ΑΊ@a  …    ΅    ‰Bÿÿÿ?‰B1ΐf‰B‹    Έΐj      ‰PΒÿÿÿ?‰P1Òf‰P1ΐΓθόÿÿÿUW‰ΗVSƒμ‰D$d΅    ‰D$1ΐ£=    ΗD$@a  ‚¤  ‹D$‹|$‹4…    Έ   ώ‰t$θόÿÿÿθόÿÿÿ<…    ‰ψθόÿÿÿ‰πθόÿÿÿ‹    ‹O‰Π)Θ…ΐ~‹G9ΒG  9Θ,  ‰G‹D$‹H…Ι…+  ‹D$p``	  ‰$΄&    ‹‹o…ΐtT΄&    ‹>t& ‹‹S‰…ΐt‰P‹CΗ"  L$%  ψÿ	θ‰C‹C‹WθqΧÿÿÿt$‰Ϊ‰Α‰ψθοÿÿ‹Z…ΐu³ƒΖ94$u›‹D$θόÿÿÿ‰ψθόÿÿÿΈ   θόÿÿÿd΅    …ΐ„”   D$€	  ‹D$=@t  …Χώÿÿ‹D$d+    uzƒΔ1ΐ[^_]Γt& d΅    £    ƒDÿÿÿdÿ    ΅    …ΐt
‹@‰Ϊθόÿÿÿdÿ    …ÿÿÿθόÿÿÿιÿÿÿ‹D$‹H…Ι„Υώÿÿ‰WιΌώÿÿθόÿÿÿιbÿÿÿθόÿÿÿffffθόÿÿÿU‰Α‰εW‰ΧV‰ΖS‰Σƒμ‹EM]1Òƒ} ΠΈ    MUME‰$9π‰Πψ‹$LΧLΖ9Α‰ΨΠ}
Ήÿÿÿÿ»ÿÿÿƒΔ‰Ϊ‰Θ[^_]Γ΄&    t& θόÿÿÿS΄&    v ‹H ‹QφΒtt& σ‹QφΒuφ€x$ u9At‹Y9ΣuΤ‹P 9ΡuΝ1ΐ[ΓΈ   [ΓfθόÿÿÿU‰εWV‰ΖSƒμ‹E‰T$‰D$‹E‰D$…Ιtj‰ΛσΌΙ‰³ΛDΖ8…ΐtV‹x9|$t}‰‹W‹O+TΖXLΖ\‰Π‰Κ;D$L$}‰D$‹D$‰T$…ΐt>…ΫtΈ   σΌΛΣΰχΠ!Γ‰DΖ8…ΐuª‹t$‹|$1ΐ1Ò‰ϋ9ΖΣMΖMΧƒΔ[^_]Γ€& t*‰~4λ·t& ‹D$‰$θόÿÿÿ‹$…ΐ‰Η…jÿÿÿλ—t& ‰~(λ΄&    t& UΈ   ‰ε]ιόÿÿÿfUΈ   ‰ε]ιόÿÿÿfU1ΐ‰ε]ιόÿÿÿt& θόÿÿÿU‰εWV‰ΖS‰χ‰Σ‰Ωƒδψƒμχί‹U‹EƒΡ χΩIχHΛ‰4$‰Φ‰L$Αώ‰Ρ	ρt61Ι‰Φ¬ΠƒΑ‰χΡώΑÿΡϊ	χuκ‹|$‹4$1Ò­ώΣοƒα EχEϊ‰4$‰|$‹T$‰Α1ÿ‹$‰Ζ…Òt‰Π1Òχρ‰Η‰πχρ‰ϊ…ΫyχΨƒÒ χΪeτ[^_]Γ΄&    v θόÿÿÿW|$ƒδψÿwόU‰εWVS‰Γ‰Π‰Κƒμ4‰MΔ‹‹‹s‰Eΐ‰}Π‹{)π‰uΰϊ‰}δ‰EΨ‰Uά…Òυ   φC$…ϋ   ΅    1Ò‹}Π‰Φ9Θώ‹uΨMΘ‰Π‹UδLΗ‹}ά9Ξ‰Mθ‰Eμ‰ωMμ‹Eΰά   ‹s‹KΗEΠ   ΗEΤ    ‰uΜ‰MΨ‰uά‹uθ‹}μπϊ;uΰ}δ‹}θL}ΰ‹uδMuμ9ψ‰Χχ}
ΈÿÿÿÿΊÿÿÿ‹uθ‹}μ‰C‹EΨ‰S‹Uάπϊ‰Eθ‹EΜ9Ξ‰Uμ‰ϊΒ‹UμMΗMΞ‰Ζ‹Eθ9Θ‰Ππ}ΗEθÿÿÿÿΗEμÿÿÿ‹Eθ‹Uμ‰C‰S‹EΠ‹UΤeτ[^_]gψ_ΓΗEΠ    ΗEΤ    λήΗEΠ    ΗEΤ    λΜ¶    ‹EΨ‹UάÿuμÿuθθόÿÿÿY^‰Χ‰Ζ‰Ρ‹Eθ‹Uμ‰uΠ―Θ‰}Τ‰Π―ΖΑ‰πχeθ‰Ζ‹Eΰ‰Χ‹UδΟπϊs{‰Eΰ‰uΨ‰ρ‰s‹uΐ‰}ά‰}Μ9Ζ‰{‹}Δ‰UδΧ‰C‰SOÿÿÿƒEΠƒUΤ ι½ώÿÿt& θόÿÿÿU‰Υ‰ΚWΑκV‰ΞS‰Γ‰Πƒβ{‰Ωƒΰƒηό)ωƒμΗ    ƒΑ(D$1ΐΑιΗC$    σ«Ή    <•    d    …νu=‰υƒε‹­ΰ²  ƒψt1Η¶D$Αξ‰C&‰πƒΰC'ΏDΑ8‰C ƒΔ[^_]Γv ƒύ~Γ‰L$Uhθ@ θόÿÿÿXZ‹L$λΈ΄&    v θόÿÿÿ‹P(Η@(    …Òt‰Πθόÿÿÿ1ΐΓv θόÿÿÿUWVS‰ΓΈ    ƒμ‰T$‰D$θόÿÿÿ‹k ‹S‹K‹…    ‰Φ+u ‰Ο}$‰D$‰4$‰|$…Ι³   ‹$‹T$1φ1Ι9π‰Π‹T$Θ‹$LΡMπ€{& ‹E u\‹D$D$9E tƒΔ[^_]Γ΄&    ;p ‰ΡH$}η¶HφΑuή‰X(‰p ‰P$φΑtΠƒαuΛƒΔ‰πΉ   [^_]ιόÿÿÿ΄&    φ@u«;p,‰Χx0}΅;p ‰Χx$‰X4‰p,‰P0}€|$ t‡ιuÿÿÿιFÿÿÿ΄&    fθόÿÿÿV‰ΖS‰Σ>t& ‹‹KΈ   Σΰ	BC‰ςΖF$θόÿÿÿ[^¶ΐΓt& d΅    £    sΙdÿ    ΅    …ΐt
‹@‰ςθόÿÿÿdÿ    u¦θόÿÿÿλ΄&    ΄&    θόÿÿÿU‰εWVS‰Γ‹Hφ@t*Η@(    ƒα1ÒhÿÿÿjÿθΆωÿÿ‰Η‰ΦXZeτ‰ψ‰ς[^_]ΓΗ@4    απ   1Òhÿÿÿjÿθuωÿÿ‹KΗC(    ‰C,‰Η‰Φ‰Ψ‰S0ƒα1ÒhÿÿÿjÿθNωÿÿ‰ρƒΔ9ΗΡ}‹C4‰ς‰C(eτ‰ψ[^_]Γ‰Η‰Φλ΄&    t& θόÿÿÿV‰ΦS‰Γθ@ÿÿÿ…φt‹K$‹s 1Ρ1Ζ	ρt¶K‰C ‰S$φΑtƒαt
[^Γ΄&    [Ή   ^ιόÿÿÿt& θόÿÿÿUWVS‰Γ¶@$‹:‹l$K$¨u[^_]Γ΄&    ‰ΦB‰Ϊθόÿÿÿ„ΐu‹NΈώÿÿÿΣΐ!G…νtΣ9_(uΞ[‰ψ^Ί   _]ιRÿÿÿfθόÿÿÿU1νWVS‰Γƒμθόÿÿÿ„ΐuƒΔ‰θ[^_]Γt& ‹‰ϊθόÿÿÿσt& ‹s ώΈΣ tξ‹θόÿÿÿ‰Η‹C 9ΖuΥ½ÿÿÿÿ;Xt1νφC$u‹ ‰ϊθόÿÿÿ‰θƒΔ[^_]Γ΄&    >t& ‹.Ί    ‰$θόÿÿÿ‹$…    1ΐ9Υ‰ς½   ”ΐ1ΙP‰Ψθκώÿÿ‹C Zλ©t& d΅    £    s¶dÿ    ΅    …ΐt
‹@‰Ϊθόÿÿÿdÿ    u“θόÿÿÿλ΄&    θόÿÿÿS‰Γ‰Ψθόÿÿÿ…ΐx[Γv σλμt& θόÿÿÿUW‰ΗVSƒμ(‰T$‹D$@‰L$d‹    ‰T$$1Ò#G„P  σΌΘ³Θ‰D$‰lΗ8…ν„6  ‰‹]‹T$‰Α4Ε    ‰$‹D$DΟXTΟ\‰t$‰D$‰T$ …Ϋ„Π  L7L‰L$λ9¶    ƒE‹$ƒE9\ΗH…σ  ‹$ΗDΗH    ‹]…Ϋ„•  ‹D$‹T$ ;C‰ΠC  >t& ‹$‰\ΗHƒE‹D$ƒE¶C$‹q8‰L$ΖC$ ¨t"‹D$‰Ϊθόÿÿÿ„ΐu‹L$Έ   ‹I<ΣΰχΠ!F‹T$<‰ψ‹sθόÿÿÿ>t& ‰Ψθόÿÿÿ‰Ζ>t& ‰ψθόÿÿÿ…φ„?ÿÿÿφC$…5ÿÿÿ1Ι‰κ‰Ψθϋÿÿι%ÿÿÿt& d΅    £    ƒSÿÿÿdÿ    ΅    …ΐt
‹@‰Ϊθόÿÿÿdÿ    …,ÿÿÿθόÿÿÿι"ÿÿÿt& d΅    £    ƒwÿÿÿdÿ    ΅    …ΐt
‹@‰Ϊθόÿÿÿdÿ    …PÿÿÿθόÿÿÿιFÿÿÿt& d΅    £    ƒ!ÿÿÿdÿ    ΅    …ΐt‹@L$‰Ϊθόÿÿÿdÿ    …φώÿÿθόÿÿÿιμώÿÿ΄&    ‹T$…Òt(Έ   σΌΚΣΰχΠ!Β‰lΗ8‰T$…ν…Πύÿÿ¶    ‹D$$d+    uƒΔ([^_]Γv ιώÿÿθόÿÿÿ΄&    t& θόÿÿÿUWVS»    θόÿÿÿ…    ‰Ψ»Π   θόÿÿÿ‹¨   “€   W‰ΖCθόÿÿÿ‹»€   ‹«„   ‰Ρ‰Β‰Ψ‰»   ‹»¨   ‰«$  ‹«¬   ‰»H  ‹»Π   ‰«L  ‹«Τ   ‰»p  ‰«t  hπ   VθΉόÿÿ€cχ‹K1ÒΗC4    ‰Ψhÿÿÿαπ   jÿθεσÿÿςÿÿÿχΠƒΔ	Βt‹C4Ί   θiψÿÿ‰ς‰Ψ[^_]ιόÿÿÿ΄&    t& θόÿÿÿVS»    θόÿÿÿ…    φCu‹    …Ò„΄   ‰Ψ³Π   θόÿÿÿ‹¨   “€   VCθόÿÿÿ‹ƒ€   ‹“„   ‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒΠ   ‰“L  ‹“Τ   ‰ƒp  ‰“t  X‰ΨφCt1θ<ωÿÿ¶K‰C ‰S$φΑt"ƒαuΉ   θόÿÿÿ‰Ψ[^ιόÿÿÿt& θωÿÿ‰Ψ[^ιόÿÿÿf[^Γ΄&    ¶    θόÿÿÿW‰ΟV‰ΦS‰Γ>t& ‰ω‰ς‰Ψ[^_ι_φÿÿ΄&    d΅    £    sάdÿ    ΅    …ΐt‹@Q‰Ρ‰ΪθόÿÿÿXdÿ    uµθόÿÿÿλ®΄&    t& θόÿÿÿW‰ΟV‰ΦS‰Γ>t& ‰Ψ‰ω‰ςθςυÿÿΗCΠΛ d΅    ‰C([^_Γ¶    d΅    £    sΜdÿ    ΅    …ΐt‹@Q‰Ρ‰ΪθόÿÿÿXdÿ    u¥θόÿÿÿλ΄&    t& θόÿÿÿU‰εWVS‰Γƒμ‹K ωΈΣ tN‹‰$θόÿÿÿ‹$‰D$;K u/‹s‹{‹Aθόÿÿÿ)Ζ‹C Χ‹T$‹ θόÿÿÿ‰π‰ϊƒΔ[^_]Γv ‰Β‹θόÿÿÿσλ£v θόÿÿÿUW‰ΗVSƒμ(‰$‹D$<‰T$‹T$@‰D$‹D$D‰T$‹$‰D$Αθƒπ‰L$ƒΰ€& ”Β8Π…΄  ‹s ώΈΣ „φ  ‹θόÿÿÿ;s …έ  ‹>‰t$Ί    ‰D$$‰$‰T$θόÿÿÿ‹.‹T$…    9Χ¶{$”Β9](”ΐ!ΒT$χΗ   …¬  ‹\$‹t$1ν1ÿ‰π9λ‰Ϊ‰ρψMλMώφD$…•   ‹\$‹t$9λ‰πψMλMώ‹$‹\$‹t$‰X‹\$‰p‹t$ΣΞ9λ‰Ψ‰ρ‰ςω}
ΈÿÿÿÿΊÿÿÿ‹4$€|$ ‰F‰V„Ò   ‹t$‹L$‹$‰ςθΘυÿÿ‹Ί   θμφÿÿ‹$‹T$$‹@ ‹ ƒΔ([^_]ιόÿÿÿt& ‹D$‹@θόÿÿÿ‹L$‰Φ‹T$‰Γ‰πΪρ9λ‰T$ψ‰Θ‰L$MλMώ9κψ}0ΊÿÿÿÿΉÿÿÿ½ÿÿÿÿΗD$ÿÿÿÿΗD$ÿÿÿΏÿÿÿιÿÿÿ΄&    ‹D$1ÿ1ν9l$ψM|$Ml$‰ψ9ΥΘLκLωιξώÿÿ΄&    ‹D$‹l$Ώ    ‹Hƒε‰L$θόÿÿÿ‹L$<…    ‰|$ιx  …ν„ΰ  ‰‹t$‹$‰ύ‰D$Αΰ‰D$ ƒΐ8‰D$λr‹C ;X„¤   ΗC ΈΣ ‹θόÿÿÿ‹D$ L ‹A8‰$θόÿÿÿ9l$„y  ‹$‹|$‹C‹S‹I8+DύXTύ\;A ‰ΠA$S  ‰Θθόÿÿÿ‹θόÿÿÿ‰s ‹l$‹D$θ‰D$9Ζu€9l$t*‹D$‹SLΕ ‹C+AXQ\‹I8;A ‰ΠA$|Η΄&    f‰t$‰$‹L$‹T$‹$θισÿÿ…ΐ„%ώÿÿ‹$Ί   θΔςÿÿιώÿÿ΄&    ‰Β‹θόÿÿÿσιτόÿÿ΄&    >t& Ί    ‰T$ θόÿÿÿ‰ϊ¶Κ‹T$ …    9Υ¶T$”ΐƒς!Π¶ΐP‹D$‹T$θντÿÿXιύÿÿ΄&    ‹|$ι‡ώÿÿ΄&    d΅    £    s$dÿ    ΅    …ΐt‹@‹$θόÿÿÿdÿ    tG‹D$‹(ιgÿÿÿf‰ϋιCόÿÿ΄&    ‹l$‰$‰k ιψώÿÿθόÿÿÿΏ    ‹L$<…    ιώÿÿθόÿÿÿλ²΄&    ΄&    θόÿÿÿS‹H‹X+HRX‹PSQ‹HθόÿÿÿƒΔ[Γ΄&    t& θόÿÿÿUWVS»    ƒμ ‰D$d΅    ‰D$1ΐ‰Ψd    φ@u‹5    …φ„ΐ  ΗD$    ½ÿÿÿÿθόÿÿÿ‰Ψ‰D$΄&    v ‰θΊ    θόÿÿÿ‰Ε;    ƒN  ‹\$­    ‰Ψ»Π   θόÿÿÿ‹s‹¨   “€   ‰D$CWθόÿÿÿ‹ƒ€   ‹“„   ‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒΠ   ‰“L  ‹“Τ   ‰ƒp  ‰“t  Y;s„²   ¶SφΒ…¥   ‹t$#s„   Έ   σΌΞΣΰχΠ!π4‰tσ8…φt~‹{ ƒβT$‰Ϊ‰<$‹{$‰|$‹^‰4Κ‹K‹[+NX^\‰ί;$|$²   ƒ~@~€|$ u;J,‰ίz0—   …ΐt#»   σΌΘΣγχΣ!Ψ‰tΪ8…φu£΄&    f‰Σ‹T$‰Ψθόÿÿÿιώÿÿ¶    Έ   θόÿÿÿj1ΙΊ Σ D$θόÿÿÿΈ   θόÿÿÿZd΅    …ΐt1θόÿÿÿθόÿÿÿ‹D$d+    u!ƒΔ [^_]Γ΄&    ‰Σπ«l$λθόÿÿÿλΘθόÿÿÿ΄&    v θόÿÿÿΈª   ι±ύÿÿθόÿÿÿ‹    Ή@Σ Έ    ιόÿÿÿ¶    θόÿÿÿ1ΐι΄φÿÿt& θόÿÿÿU‰εWVS»    ƒμθόÿÿÿ…    ‰Ψθόÿÿÿ‰Eπ¶C¨t'ΎÿÿÿÿΏÿÿÿ‹Uπ‰Ψθόÿÿÿeτ‰π‰ϊ[^_]Γ΄&    ¨uD‹KΗC4    1Ò‰Ψhÿÿÿjÿαπ   θδιÿÿ‹s4Y_‰s(‹KR1ÒPƒα‰ΨθΛιÿÿ‰Ζ‰ΧXZλv ΈÿÿÿÿΊÿÿÿ1φλΣfθόÿÿÿU‰εWV‰ΖS»    ƒμθόÿÿÿ…    ‰Ψθόÿÿÿ¶K‰EπφΑt\ƒαΈÿÿÿÿΊÿÿÿt-‹KR‰ςP‰Ψƒαθ\ιÿÿ‰Ζ‰ΧXZ‹Uπ‰Ψθόÿÿÿeτ‰π‰ϊ[^_]Γ‹Khÿÿÿ‰ς‰Ψjÿαπ   θ'ιÿÿY_λ¶v ΎÿÿÿÿΏÿÿÿλΎt& θόÿÿÿUWV‰ΖS»    ƒμ θόÿÿÿ…    φC„7  ƒC‰Ψ»Π   ΗFÿÿÿÿΗFÿÿÿθόÿÿÿ“€   ‹¨   ‰ΕC‰|$‰T$‰L$‰D$Wθόÿÿÿ‰D$‰Ζ‰Χ‹ƒ€   ‰T$ ‹“„   ‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒΠ   ‰“L  ‹“Τ   ‰ƒp  ‰“t  XΗ$   ¶C‰ωΗC ÿÿÿÿΗC$ÿÿÿ‰ΒƒΚ;s,K0S|ƒΘ
ΗC,ÿÿÿÿCΈ   ΗC0ÿÿÿθόÿÿÿj‰ω‰ς‰ΨUθ¥πÿÿ‰Ψθξÿÿ€cύ‰C ‰Ζ‰Χ‰Ψ‰S$‰κθόÿÿÿ1Ι‰ϊ‰πθόÿÿÿZY…ΐ„  ‰Ψθόÿÿÿÿt$‹L$‹T$‰Ε‹D$θόÿÿÿfƒC‰Ζ‰Χ‹ƒ€   ‹“„   ‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒΠ   ‰“L  ‹“Τ   ‰ƒp  ‰“t  Xƒ,$…ÿÿÿfƒC‰κ‰Ψ€Kθόÿÿÿ‰π‰ϊ+D$T$‰$‰T$;Cv‰C‹$Έ αυ‹\$‰ϊ9ΘΈ    Ψ‰π|0$T$Ή   θόÿÿÿ€=    „dΎ  ƒΔ [^_]Γ΄&    v  αυƒÒ λΝ¶    €cϋƒΔ [^_]Γt& ΄&    ΄&    θόÿÿÿUWVΎ    Sθόÿÿÿ‹…    σφCt[^_]Γ΄&    f¶¨$  ƒπ¶ΐθόÿÿÿ…ΐ…¶   ‰Ψ»Π   θόÿÿÿ‹¨   “€   W‰ΖCθόÿÿÿ‰Ε‹ƒ€   ‰Χ‹“„   ;k,‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒΠ   ‰“L  ‹“Τ   ‰ƒp  ‰ψC0‰“t  Y|€KΈ   ΗC,ÿÿÿÿΗC0ÿÿÿθόÿÿÿj‰κ‰Ψ‰ωVθaξÿÿX‰ΨZ‰ς[^_]ιόÿÿÿfθόÿÿÿ4…    θόÿÿÿ…ΐ……Ύ  €NΗ       θόÿÿÿ1ΐ[^_]ιΉρÿÿ΄&    fθόÿÿÿ‰Α‰Π‹Qƒϊtƒϊu,‹Qθόÿÿÿ…ΐuΈόύÿÿΓ΄&    ‹Qθόÿÿÿ…ΐtζΈςÿÿÿΓ΄&    t& θόÿÿÿUWVS‰Λƒμ@‰T$‰$d΅    ‰D$<1ΐd΅    ‹PH‹Έh  ‹¨l  …Òr  ƒxHch  ‰|$‰l$>t& t$‹T$T‰Ω‰πθjθÿÿ‹$‹L$d΅    ‰D$8‰ΠΗ‰T$$‰ΚΝ‰<$1ÿ‰l$‰Υ‰L$(1Ι9ψΝ‹l$ΗD$,ΠΛ MΚMψ‹T$‰Θ9οΠ‹$LΚLύ‹T$9ψ‰ΠΘ}Η$ÿÿÿÿΗD$ÿÿÿ‹$‹T$‰D$‰π‰T$ ‰ΪθΌY  =όύÿÿu:…Ϋ„΅   ‹L$0‹\$ d‹    ‹I‰P  Η‚<  0[  ‰@  ‹L$‰L  ‹T$<d+    …‡   ƒΔ@[^_]Γ¶    d΅    £    ƒςώÿÿdÿ    ΅    t$…ΐt‹@S‰ς‹L$XθόÿÿÿXdÿ    …Εώÿÿθόÿÿÿι»ώÿÿfΈώύÿÿλ΄&    fΗD$    1ÿ1νΗD$    ι‡ώÿÿθόÿÿÿfθόÿÿÿWVSƒμ‹Xd‹    ‰T$1Ò‹‰ΰθόÿÿÿ…ΐ…”   ‹T$‹4$Έκÿÿÿ…ÒxZ‹L$ωÿΙ;wNd‹=    1ΐ…Ϋ‰H  •ΐΐ‰‡D  Έ}Α%9πΈ   Π|<Έ Κ;iΪ Κ;χζΪ1ΫΘΪjΉ   θόÿÿÿZ‹T$d+    u$ƒΔ[^_Γ¶    ΈÿÿÿÿΊÿÿÿλΝt& ΈςÿÿÿλΟθόÿÿÿt& θόÿÿÿΉ    S…    Q8™x  t& ‰
ƒΒ(ΗBδ    ΗBμ    ΗBπ    9Ϊuβ‰A1ΐ€aϊΗA    ΗA(    ΗA4    ΗA ÿÿÿÿΗA$ÿÿÿΗA,ÿÿÿÿΗA0ÿÿÿ[Γ¶    θόÿÿÿ£    ‚ή  UWVS‰ΓƒμθόÿÿÿΈ   θόÿÿÿϊ‹<    Ύ    χ‰|$θόÿÿÿ4…    ‰π‰t$θόÿÿÿ‰ψθόÿÿÿ‰ψ8x  ‰ύ‰D$‹_‹t$G+t$‰D$ώ…Ϋ„«   ‰<$λNt& >t& ¶C$‹} ΖC$¨t‹D$‰Ϊθόÿÿÿ„ΐu‹MΈ   ΣΰχΠ!G‰s ‰Ψ1Ι‰ςθΠζÿÿ‹$‹X…ΫtV‹C 9Xu®t& d΅    £    sdÿ    ΅    …ΐt
‹@‰Ϊθόÿÿÿdÿ    …wÿÿÿθόÿÿÿιmÿÿÿ΄&    ‰ΗƒΗ(ƒΕ(9|$…)ÿÿÿ‹|$‰ψφGu9‹OΗG4    1Ò‰$απ   hÿÿÿjÿθΰÿÿY[ςÿÿÿχΠ	Βt
‹G41Òθ	εÿÿ‹D$»    θόÿÿÿ‹D$θόÿÿÿθόÿÿÿ‹…    φDt»    θόÿÿÿ…    t‹…ΐtθόÿÿÿϋΈ/η Ί   θόÿÿÿ1ΐƒΔ[^_]Γ΄&    fffffffU‰εWVSƒδψƒμt& ‹@Υ ‰Θ‰L$ƒΰkΐ4‹hΥ ‹lΥ DΥ ‹ΈLΥ ‹°HΥ ‰$‹PΥ ‰L$‹TΥ ‹€DΥ ‰T$‰L$‹θόÿÿÿ+D$T$‰Α!ϊ!ρ¬ώΡο‰π‰|$‹|$χΠ‰ΖΈ    χΧ!Ξ!Χ	χ‰Η‹sDωEΠ‹C‰t$‰|$‹{ ‹[‰|$‹=@Υ ‹L$9ω…Gÿÿÿ‰Ζ‹D$‰Ρ―ΞχζΚD$‰ΩT$­Π1ΫΣκφΑ EΒEΣ$T$eτ[^_]Γ΄&    ΄&    θόÿÿÿU‰εWVSƒδψƒμ‹< ³  ‰$‰T$‹5@u χΖ   u#‹‹_΅@u 9Ζuδ‹$‹T$ΘΪeτ[^_]ΓfσλΛ΄&    t& θόÿÿÿU‰εWV‹@u φΒu#‹5¬u ‹=°u ‹@u 9Κuί‰π‰ϊ^_]Γt& σλΞt& θόÿÿÿUWV‰ΖS‹@u φΓuB‹\u ‹du ΅`u ‹=¬u ‹-°u ­ΠΣκφΑ ‰>EΒ‰n‰F΅@u 9ΓuΌ[^_]Γt& σλ―t& θόÿÿÿ΅    S…ΐu1‹@u φΒu΅Xu ‹\u ‹@u 9ΪuΰΣθ[Γt& σλΣt& λΛ΄&    t& θόÿÿÿU‰εWVSƒδψƒμ‹5    ‹… ³  …φu~΅@u ¨um‹lu ‹3‹{‹hu ‰L$‹\u ‰4$‹5`u ‰|$‹=du ‰T$‹@u 9Πu»‹D$‹T$$T$­ώ1ΫΣοφΑ EχEϋπϊeτ[^_]Γ¶    σλ†t& ι{ÿÿÿ΄&    fθόÿÿÿU΅    ‰ε…ΐu΅΄u 1Ò]Γ¶    ΅΄u 1Ò]Γt& θόÿÿÿ‹    ΅Du …Òu…ΐt‹θόÿÿÿΓ1ΐΓ΄&    ¶    θόÿÿÿVS‰ΓΈ    θόÿÿÿ‰Ϊ‰ΖΈ0t θόÿÿÿΉDu Ί   ‰ΓΈ0t θόÿÿÿ‰ςΈ    θόÿÿÿ‰Ψ[^Γ΄&    θόÿÿÿVS‰ΓΈ    θόÿÿÿ‰Ϊ‰ΖΈ0t θόÿÿÿ‰ς‰ΓΈ    θόÿÿÿ‰Ψ[^Γ΄&    t& θόÿÿÿUWVS‰Σƒμ‰$‹D$4L$‰D$‰Η‹Wd΅    ‰D$1ΐ‹Qÿt$4ÿt$4θόÿÿÿ‰Φ‰ΗƒΔΊÿÿÿÿ…Ϋ…ή   ½$DΒΊ@   ƒΐΉÿÿÿÿ)Β…φ…έ   ½ΗDΑƒΐ9Βÿ   Ίÿÿÿÿ…Ϋ…β   ½$DΒƒΐΊ@   ‹L$½ÿÿÿÿ)Β‹D$‰D$…ΐ…¨   ½ΑDΕƒΐ9Β·   ‹,$‰Ϊ―Ω‰ψ―Χ―υΦχε‰4$‹<$‰,$‰Ζ‰θ‹l$Χ―θ‹$ÿt$0ÿt$0χαέκθόÿÿÿ‹L$πϊ‰‰QX1ΐZ‹T$d+    u`ƒΔ[^_]Γ½ΓDΒΊ@   ƒΐ!Ήÿÿÿÿ)Β…φ„#ÿÿÿ½ΖDΑƒΐ!ιÿÿÿt& ½D$DΕƒΐ!ιQÿÿÿ½ΓDΒƒΐ!ιÿÿÿfΈµÿÿÿλ“θόÿÿÿt& θόÿÿÿUWVS‰Γƒμ‹st‹{xd΅    ‰D$1ΐ‹C|χήƒΧ χΨ™χίR‰ω‰ςPD$θόÿÿÿ‹D$‹|$Ί}Α%‹t$ƒΔ‹«€   9ΒΊ   ‹‹„   ϊΝ   Ί Κ;iÿ Κ;χβϊ1ÿΖΧ1υ1ω	ι…Δ   ‹D$$‹t$$‹|$(‰Ct‹D$(χήƒΧ ‰Cx‹D$,χί‰ω‰C|χΨ™R‰ςPD$θόÿÿÿ‹D$‹L$‹t$ZΊ}Α%9ΒΊ   _Κ|   Ί Κ;iΙ Κ;1ÿχβΚΖΧΈ Κ;χ«   ‰³€   ‰»„   πϊ‰ƒ   ‰“”   ‹D$d+    u?ƒΔ[^_]Γv ΎÿÿÿÿΏÿÿÿ1ω1υ	ι„>ÿÿÿfι5ÿÿÿ΄&    fΎÿÿÿÿΏÿÿÿλθόÿÿÿ΄&    ΄&    θόÿÿÿUWVSƒμ‹@u φΒ…•   ‹-\u ‹`u ‹du ‹5¬u ‰l$‹-ΐu ‰L$‹=°u ‰\$‹Έu ‰,$‹Όu ‹-@u 9κu¨Ξ¶L$‹,$ί‰t$‹\$‹t$‰|$­σΣξφΑ Eήέ‰λΑϋSU‹T$‹L$θόÿÿÿƒΔ [^_]Γ΄&    σιUÿÿÿ΄&    fθόÿÿÿS‰Α‰ΣBƒ‹‰S‹Q‰P‹Q‰P‹Q‰P‹Q‰P‹Q‰P‹Q‰P‹Q‰P‹Q ‰P ‹Q$‰P$‹Q(‰P(‹Q,‰P,‹Q0‰P0ƒ‹K‰K8‹H‰K<‹H‰K@‹H‰KD‹H‰KH‹H‰KL‹H‰KP‹H‰KT‹H ‰KX‹H$‰K\‹H(‰K`‹H,‰Kd‹@0‰Ch[ΓθόÿÿÿU‰εS‰Γƒδπƒμ ‹Θu d΅    ‰D$΅Δu +Μu Πu ‰Ρ‰Β‰ΰθόÿÿÿ‹$‰‹D$‰C‹D$‰C‹D$d+    u‹]όΙΓθόÿÿÿ΄&    fθόÿÿÿU΅    ‰ε…ΐt΅8t ‹<t ]Γfd΅    ]ιόÿÿÿt& θόÿÿÿUWVS‰Γƒμ΅Du €δu ‰Du ‹S‰D$‹C‰Lu £Hu ΅Du ‹θόÿÿÿ‰xu £Pu ‰Tu ‹s‹{£„u Έ@B ‰5|u 1φ‰=€u ‰u ‹K1Ò¥ΒΣΰφΑ EΠEΖ‹s‰Η‰$‰π‰ΥΡθ‰T$1Òψκ1ÿ‰Α…Òt‰Π1Òχφ‰Η‰ΘΉ   ‹l$χφ1φ‰ϊ9Θ‰ΧχCΘCς‰v ‰Θ‰5v ‹{χc―ώ£v ϊ‹<$‰v )Η‰ΘΥ‰=v ‰-v ―sχc‹l$ς£v ‰ v …νtZ‹K΅`u ‹du ‹5”u ‹=u +MΊ   ¥Β1νΣΰφΑ EΠEΕ¥χΣζ£`u 1ΐƒα EώEπ‰du ‰5”u ‰=u ‹C‹T$Ή    1φΗ,v     £\u £u ‹$Η0v     +K¥ΒΣΰφΑ ‰4v EΠEΖ‰(v £$v ‹CΗ8v     £Xu £u Η<v     ƒΔ[^_]Γ΄&    fχΩ1ν­ΠΣκφΑ EΒEΥ­ώΣο£`u 1ΐφΑ EχEψ‰du ‰5”u ‰=u ι?ÿÿÿfθόÿÿÿU‰εWVSƒδψƒμ΄&    ‹ΐΥ ‰Θ‰L$ƒΰkΐ4‹πΥ ‹τΥ ΔΥ ‹ΈΜΥ ‹°ΘΥ ‰$‹ΠΥ ‰L$‹ΤΥ ‹€ΔΥ ‰T$‰L$‹θόÿÿÿ+D$T$‰Α!ϊ!ρ¬ώΡο‰π‰|$‹|$χΠ‰ΖΈ    χΧ!Ξ!Χ	χ‰Η‹sDωEΠ‹C‰t$‰|$‹{ ‹[‰|$‹=ΐΥ ‹L$9ω…Gÿÿÿ‰Ζ‹D$‰Ρ―ΞχζΚD$‰ΩT$­Π1ΫΣκφΑ EΒEΣ$T$eτ[^_]Γ΄&    U‰εWVSƒδψƒμt& ‹ΐΥ ‰Θ‰L$ƒΰkΐ4‹θΥ ‹μΥ ΔΥ ‹ΈΜΥ ‹°ΘΥ ‰$‹ΠΥ ‰L$‹ΤΥ ‹€ΔΥ ‰T$‰L$‹θόÿÿÿ+D$T$‰Α!ϊ!ρ¬ώΡο‰π‰|$‹|$χΠ‰ΖΈ    χΧ!Ξ!Χ	χ‰Η‹sDωEΠ‹C‰t$‰|$‹{ ‹[‰|$‹=ΐΥ ‹L$9ω…Gÿÿÿ‰Ζ‹D$‰Ρ―ΞχζΚD$‰ΩT$­Π1ΫΣκφΑ EΒEΣ$T$eτ[^_]Γ΄&    ΄&    U‰εθόÿÿÿΜu ]Πu Γ΄&    v θόÿÿÿUWV‰ΦS‰Γƒμƒβ…/  θόÿÿÿ‰Ρ‰Η‰ƒ¤   ρÿÿÿχΧ‰“¨   	ωt+ƒ€   “„   ‰ƒ¤   ‰“¨   ‹CtCh‰Η‹SxΈ Κ;SliΚ Κ;χη‰T$1ÒL$‹K‰$‹C|$T$‰C$‹C‰S(‹S ­ΠΣκφΑ t‰ΠC|=ÿΙ;vƒΗi‹°    Κ;Έ Κ;‰{p‰χχ£¬   Κ‰CX‰Ψ‰S\θόÿÿÿ1Òƒη‰Ω•ΒΈ0t θόÿÿÿ‹ƒ€   C$‹“„   S(‰C,‰Ψ‰S0ΊΐΥ θ#ωÿÿC4Ί@Υ θωÿÿ…ÿtƒƒ   ƒζtΏ@t ΎDu Ή?   σ¥ƒΔ[^_]Γt& Η€θ       Η€μ       θόÿÿÿι³ώÿÿ΄&    v θόÿÿÿUWVSƒμ΅Du ‹θόÿÿÿ‹=Lu ‹5Hu ‰Α+Pu ‰ΣTu ‰Ν!ϋ£Pu !υ¬ώ‰Tu Ρο‰,$‰υ‹$χΥ‰ώ£„u ‰οχΦ1ν‰u !Ο!ή	ώ‹5Xu EΝEέ‰Θ―σχ%Xu ς`u du ‰Ζ‰Χ£`u ‰Θ―u ‰du χ%u ¶\u Ϊ”u u 1Ϋ‰D$‰T$£”u Έ Κ;‰u 1Ò¥ΒΣΰφΑ EΠEΓ9Ζ‰Α‰ψΠrM΅¬u ‰Σ‹°u ƒΐƒÒ v )Ξ‰$ίƒΐ‰T$‰ύƒÒ 9Ξέsη‹$‹T$‰5`u ‰=du £¬u ‰°u ‹u Έ Κ;1Ò1Ϋ‹t$‹|$¥ΒΣΰφΑ EΠEΓ‰ϋ9ΖΣrV‹πu ‹τu ƒΑƒΣ ΄&    v )Ζ‰$ΧƒΑ‰\$‰ύƒΣ 9ΖΥsη‰ϊ‹l$‹<$‰5”u ‰u ‰=πu ‰-τu ƒΔ[^_]Γ΄&    ΄&    θόÿÿÿUWVS‰Γ‹@xθόÿÿÿ‰Ζ„ΐt‹SH…Òt‰Ψθόÿÿÿ…ΐuuΈ    θόÿÿÿƒ@u ‰Εθέύÿÿ‰π1ÿ„ΐuEΊ   ΈDu θVόÿÿΈ    ‰κƒ@u θόÿÿÿ…ÿt‹WL…Òt‰ψθόÿÿÿ‹Gxθόÿÿÿ[1ΐ^_]Γf‰Ψ‹=Du θ“χÿÿλ¬‹Cx1φθόÿÿÿι|ÿÿÿθόÿÿÿU‰ΕWVSƒμL‹=    …ÿ…4  ΅@u ‰$¨…  ΅Du ‹θόÿÿÿ‰Ζ΅Du ‰Χ‰|$‹@<‰t$‰E¶δu E ΅ΰu ‰E‹Δu ‹lu ΅hu ‰L$‹ u ‰T$‹u ‰D$‹Θu ‰π‰L$(+Pu ‹Hu ‰T$$‰ϊ‰ΗTu ‰\$ !Ο‹Lu ‰ώ‰Χ!ί¬ΩΡλ‰ΚΉ    χΣχÒ‰Ψ!ς»    !ψ	Π‰Ψ‹`u DΖDΟ‹=Xu ‰D$@‰L$D‹du ‰|$0‰T$4‹=\u ‹D$‹T$‰L$8‰|$<‹=|u +„u u !Η΅|u ‹5u ‰ω‹=€u !Χ‹€u ¬ΠΡκ‰T$‹T$χΠ!Θ‰t$χÒ!ϊ	Β‹u EΛEϋ‹”u ‰\$‹u ‰\$,΅@u 9$…ƒώÿÿ‹t$0‹D$‰$‰ϋ‹T$‹|$@‰L$H‹L$D‰E ‰π‰Uχη‹|$―Ξ‹t$Κ¶L$<D$4T$8t$|$ ­ΠΣκφΑ t‰Π1ÒΖ‰Ψ‰u‹4$Χ‰}―Ζ‰Α‰πχd$HΚ¶L$,D$T$1Ϋ­ΠΣκφΑ EΒEΣD$$T$(‰E‰UƒΔL[^_]ΓσιΥύÿÿ΄&    fιΕύÿÿ΄&    fθόÿÿÿUWVSμ”   ‰D$(‹„$¨   ‰T$,‰L$4‰D$d΅    ‰„$   1ΐΗD$l    ΅@u ‰D$¨…ª  ‹L$,‹D$”$„   ‹t$(θόÿÿÿ‰D$$…ΐ…^  ‹„$   9Du …λ  ‹”$„   ‹$   ΅Du ‰$‰L$‹θόÿÿÿ‹L$‹5Pu ‰Ε‰Π‹$‹=Tu ‰Λ‰l$9κ‰ύΓ’Γ9Φl$’Α Λ\$…W  ‹$‹\$‹l$‰Ϊ9ιΒ‰κCΡCΓ9ςψr ΅ΰu ‰4$‰|$‰D$l¶δu ΖD$D$s‹Δu ‹Θu ΅hu ‹lu ‰L$@‹u ‰\$D‹ u ‰L$H‹$„   ‰\$L‹$   )ρ‰D$8΅Hu ‰T$<‹Lu ϋ‰Ξ!ΖΏ    ‹-du !Σ¬Π‰ωΡκχΠ‰l$`χÒ!π!Ϊ	Β‰ψ‹\u DΖEί‹=Xu ‹΄$„   +5„u ‰T$d‰D$h΅|u ‰υ‰|$X‹=`u ‹€u ‰|$\‹Ό$   =u !Ε¬Π‰l$‰ύ‹|$χΠ!ΥΡκ‹5”u χÒ!ψ!κ‰t$P	Β‰Θ‹u DΗ‹=u Eι‰|$T‹=u ‰D$΅@u 9D$…τύÿÿ‹L$h‹t$8‰|$(‹|$X‰T$‰Ϊ―Χ‰ψ‹|$<‰Σχα¶L$dΪD$\T$`t$@|$D­Π1ΫΣκφΑ ¶L$(EΒEΣ‹\$Ζ‹D$Χ‰s―θ‰{‹|$Lχd$κD$PT$T1φ­ΠΣκφΑ EΒEΦ‹t$HΖΧ€|$ ‰t$‰|$‰s‰{„ΰ  ‹t$4…φ„-  ‹>‹n‹„$„   ‹”$   ‰ξ‰|$9Η‰l$Φƒέ  ‹<$‹l$‰Φ9ψξƒΚ  ‹t$4¶\$s8^ …α  ‹<$‹l$‰ω‰λ)ΑΣ+|$l$‰L$‰ω‹~‰\$ ‰l$‰Λ\$‰|$(‰$„M  ‹L$‹\$ ‰Ο	ί„;  ‹|$‹4$¬ώΡο‰t$,9Ξ‰ώή‰|$0s+D$T$‰Α‰Σ‹D$4‹|$‰Ϊ‹t$+pxD$t‰t$t‰|$xP‰Θÿt$ÿt$‰L$‰\$θ
μÿÿ‰D$0ƒΔ…ΐ…Λ   ‹\$l9\$(‹L$‹\$„e  ‹5Xu ‹D$tχζ‰$‰Α‹D$xχζ‰Ζ‹$‰Χ1Ò‹-u πϊ1ÿ‰Ζ…Òt‰Π1Òχυ‰Η‰π1Ϋχυ‰Φ‰ϊ‰ς‰Η‰Θ‰Α…Ò…ρ   ‰Θ‰Ό$€   χυ‰Ϊ‰ϋ‰D$|‰Α‹t$‹|$,‹l$0‹D$t9χ‹T$x‰ξt$ ƒΆ   ‹t$4‹|$FV‰W‰GN^‰O‰_΄&    v ‹„$   d+    …ρ   ‹D$$Δ”   [^_]Γt& σι>ϋÿÿ‹<$‹l$‰Φ9ψ‰ϋ‰ι‹|$ξ‹l$CΨCΚ9ϋι‚ώÿÿΗD$$κÿÿÿλt& ΖD$ ιήϋÿÿ¶    ‹t$)FV)N^ιsÿÿÿv ‰Π1Òχυ‰Γιÿÿÿv ΗD$$νÿÿÿιSÿÿÿv ‹D$‹t$4‹P‹@+FV‰D$|D$|‰”$€   ‰ΪP‰Θÿt$ÿt$θKκÿÿƒΔ…ΐu‹L$|‹$€   ιΈώÿÿ‰D$$ιϋώÿÿθόÿÿÿ¶    θόÿÿÿUWVΎκÿÿÿS‰Γƒμ,‹d΅    ‰D$(1ΐ‹C…ΐ  {ÿΙ;‡  Ήt]ν9ΡΊ   Β{  Έ    θόÿÿÿƒ@u ‰$θΚτÿÿ‹‹S+¬u °u ‹\u ‰Η‰Υ΅`u ‹du ­ΠΣκφΑ t‰Π‹K)Α‰Θ™R‰ϊQ‰ιD$$θόÿÿÿ‹D$,‹|$$‹Όu ‹l$(‰D$΅Έu ƒΔ‰Ρ9ψι:  9Η‰ιΡ•   ‹ΐu +L$…Ιƒ   )ψ‰L$κΑω‰L$‰Ρ‰Βÿt$ÿt$D$$θόÿÿÿ‹D$$‹T$(‹L$,‰D$‰T$‰L$ QRPΈDu θpκÿÿ‹‹S‹\u ‰°u £¬u ‹C1Ϋ™¥ΒΣΰφΑ EΠEΓƒΔ1φ£`u ‰du Ί   ΈDu θ7ςÿÿ‹$Έ    ƒ@u θόÿÿÿΈª   θόÿÿÿ…φt%‹D$(d+    ujƒΔ,‰π[^_]Γ¶    Ύκÿÿÿλά‰θ	ψu‹T$…ÒtΝd΅    ‹€θ  …ΐt½‹ …ΐu·‰ψ‰|$‹|$‰l$ ‹T$ ‰ω‰|$$θόÿÿÿλ‹ΐu +L$ιΥώÿÿθόÿÿÿ΄&    fθόÿÿÿU‰εWVSƒδψƒμ‹@u φΓ…ΰ   ΅u ‹ u ‰D$΅xu ‰T$‹θόÿÿÿ+„u u ‰Ζ‰Χ΅|u ‹€u !π!ϊ‹5|u ‹=€u ¬ώΡο‰|$‰χ‹L$χΧ‰ώχΡΏ    !Ζ!Ρ	ρ‹u ‹5”u EΗEΧ‹=u ‰L$‹u ‰t$‰|$‹=@u 9ϋ…Nÿÿÿ‹t$‰Σ―ήχζΪD$T$1Ϋ­ΠΣκφΑ EΒEΣD$T$eτ[^_]Γ΄&    σι
ÿÿÿ΄&    fθόÿÿÿU‰εWVSƒδψƒμ‹    …Ϋ…  ‹@u φΓ…β   ΅hu ‹lu ‰D$΅Du ‰T$‹θόÿÿÿ+Pu Tu ‰Ζ‰Χ΅Hu ‹Lu !π!ϊ‹5Hu ‹=Lu ¬ώΡο‰|$‰χ‹L$χΧ‰ώχΡΏ    !Ζ!Ρ	ρ‹Xu ‹5`u EΗEΧ‹=du ‰L$‹\u ‰t$‰|$‹=@u 9ϋ…Nÿÿÿ‹t$‰Σ―ήχζΪD$T$1Ϋ­ΠΣκφΑ EΒEΣD$T$eτ[^_]Γ΄&    v σιÿÿÿ΄&    fιψώÿÿ΄&    fθόÿÿÿU‰εWVSƒδψƒμ ‹… ³  ‹    ‰D$…Ϋ…  ‹@u φΓ…ÿ   ΅hu ‹lu ‹L$‰D$΅Du ‰T$‹‹I‰T$‹‰L$θόÿÿÿ+Pu Tu ‰Χ#=Lu ‹Hu ‰ϊ‹5Hu ‹=Lu !Α¬ώΡο‰Θ‰|$‰χ‹L$χΧ‰ώχΡΏ    !Ζ!Ρ	ρ‹Xu ‹5`u EΗEΧ‹=du ‰$‹\u ‰t$‰|$‹=@u 9ϋ…=ÿÿÿ‹4$‰Σχζ―ή‰Χ‰Ζ‹T$‹D$ίt$|$D$T$­ώ1ΫΣοφΑ EχEϋπϊeτ[^_]Γ΄&    σιλώÿÿιγώÿÿ΄&    fθόÿÿÿUWVSƒμ‰D$‹@u ‰Ψƒΰ‰$…ό   ΅πu ‹τu ‹|$‰‰W΅xu ‹θόÿÿÿ‹=€u ‹5|u +„u u ‰Α‰Υ‰π‰ϊ¬ψΡκ!ρ!ύ‰ΖχÒΏ    ‰ΠχΦ‹u !Ξ!θ	π‰T$΅u EΟEο‹5”u ‹=u ‹@u 9Σ…`ÿÿÿ‰κ‹,$‰Σ―Ψχα¶L$Ϊπϊ1Ϋ­ΠΣκφΑ ΉÿΙ;EΒEΣ‰λ9ΑΣsL»ÿΙ;1Ιt&  6eΔ‰ΟƒÒÿƒΕ9ΓΧrν‰ι1Ϋ‹t$‰F^ƒΔ[^_]Γt& σιιώÿÿ΄&    f1Ι1ΫλΡ΄&    v θόÿÿÿUWVSƒμ‹=    ‰D$…ÿ…2  ‹@u ‰Ψƒΰ‰$…ώ   ΅¬u ‹°u ‹|$‰‰W΅Du ‹θόÿÿÿ‹=Lu ‹5Hu +Pu Tu ‰Α‰Υ‰π‰ϊ¬ψΡκ!ρ!ύ‰ΖχÒΏ    ‰ΠχΦ‹\u !Ξ!θ	π‰T$΅Xu EΟEο‹5`u ‹=du ‹@u 9Σ…`ÿÿÿ‰κ‹,$‰Σ―Ψχα¶L$Ϊπϊ1Ϋ­ΠΣκφΑ ΉÿΙ;EΒEΣ‰λ9ΑΣsN»ÿΙ;1Ι΄&     6eΔ‰ΟƒÒÿƒΕ9ΓΧrν‰ι1Ϋ‹t$‰F^ƒΔ[^_]Γt& σιηώÿÿ΄&    f1Ι1ΫλΡ΄&    v ιΗώÿÿ΄&    fθόÿÿÿUWVSƒμ‹=    ‰$…ÿ…ƒ  ‹@u ‰έƒε…b  ΅¬u ‹°u ‹<$‰‰W΅Du ‹θόÿÿÿ‹Hu ‹=Lu +Pu Tu !Χ!Α‹Lu ΅Hu ¬ΠΡκ‰Ζ‰ΠχΦχΠ‰ς!ψ‹5ΐu !Κ	ΠΈ    ‹du EΘEψ΅Xu ‰T$‹\u ‰D$΅`u ‰T$‹Όu ‰D$΅Έu ‰T$‰D$‹@u 9Σ…;ÿÿÿ‰ϊ‹<$‹D$‰L$‹\$‹L$_‰χ‰\$‰Σ―Ψ‰L$¶L$χd$ΪD$T$1Ϋ­ΠΣκφΑ EΒEΣΑÿΖΈÿΙ;Χ9π‰θψs,ΊÿΙ;1ΐ΄&    Ζ 6eΔ‰ΑƒΧÿƒΕ9ςωrμ1Òl$T$‹$‹D$‹T$‰‰S‰sƒΔ[^_]Γ΄&    σι†ώÿÿ΄&    fιvώÿÿ΄&    fθόÿÿÿUWVS‰Γƒμ8d΅    ‰D$41ΐ{ÿΙ;‡	  Έ    θόÿÿÿƒ@u ‰Εθακÿÿ‹\u ‹du ΅`u ‹3‹{5¬u =°u ­ΠΣκφΑ t‰ΠC‰ω™R‰ςPD$0θόÿÿÿ‹D$0‹Όu ‹|$8‰D$‹D$4‰T$‹S‰D$΅Έu ‰D$Y^‹L$‹9$Ρ|%‰Φ;$t$Ύκÿÿÿ  ‹ΐu +K…Ιϊ  ÿÿΙ;‡6  Ήt]ν;L$Ή   L$‚  ¬u °u ‰°u ‹\u ‰D$£¬u ‹C‰T$™‰Ζ‰ΧΣζ¥Η1ΐφΑ EώEπΈ Κ;5`u =du 1Ò‰5`u ¥Β‰=du ΣΰφΑ t‰Β1ΐ‰Ρ9Ζ‰Β‰ψ‰L$D$‰T$r_‹D$‹T$‰l$‰\$‹L$ƒΐ‹\$ƒÒ )Ξ‰D$ίƒΐ‰T$‰ύƒÒ 9Ξέsζ‹D$‹T$‰5`u ‹l$‹\$‰=du £¬u ‰°u ‹u Έ Κ;1Ò‹5”u ‹=u ¥ΒΣΰφΑ t‰Β1ΐ‰T$9Ζ‰ωL$‰D$rg‹πu ‹τu ‰l$‰\$‹\$ƒΒƒΡ ‰Π‰Κ‹L$)Ξ‰D$ίƒΐ‰T$‰ύƒÒ 9Ξέsζ‹D$‹T$‰5”u ‹l$‹\$‰=u £πu ‰τu ‹$‹T$+‹ΐu S+K1φ‰ΛΑϋSQ‰Ρ‰ΒD$0θόÿÿÿ‹D$0‹T$4‹L$8‰D$$‰T$(‰L$,QRPΈDu θΟήÿÿƒΔt& Ί   ΈDu θΙζÿÿΈ    ‰κƒ@u θόÿÿÿΈª   θόÿÿÿ‹D$4d+    u&ƒΔ8‰π[^_]Γt& Ύκÿÿÿλ±΄&    fΎκÿÿÿλΝθόÿÿÿt& θόÿÿÿUWVS‰ΓƒμTd΅    ‰D$PΈ    θόÿÿÿ‰D$(΅    …ΐ…®   ΅Lt ‹Pt ‹=Ht ‹5Dt ‰$΅@t ‰T$‹θόÿÿÿ+$T$ƒσ‰Α‰Πƒγ!ψ!ρ¬ώΡο‰D$‰ς‹l$‰ψ‰L$‹|$χÒχΠ!ϊ!θ	Πu8;=v ‰θv ‚i  ‹l$…ν„J  Έÿÿÿÿ½έDΨƒΓ!λd΄&    ΅v v t/„Ϋt+1Ϋ‹T$(Έ    θόÿÿÿ‹D$Pd+    …	  ƒΔT‰Ψ[^_]ΓΗD$    1ΐΗD$    Ίÿÿÿÿ½ΨDΪƒΓ‹u ‹ u …Ò…  Ίÿÿÿÿ½ΑDΒƒΐ)ΓΈ    HΨθόÿÿÿ‰Α‰Π…Ò…™  Έÿÿÿÿ½if
}

DEFINE_STATIC_KEY_FALSE(sched_numa_balancing);

#ifdef CONFIG_NUMA_BALANCING

int sysctl_numa_balancing_mode;

static void __set_numabalancing_state(bool enabled)
{
	if (enabled)
		static_branch_enable(&sched_numa_balancing);
	else
		static_branch_disable(&sched_numa_balancing);
}

void set_numabalancing_state(bool enabled)
{
	if (enabled)
		sysctl_numa_balancing_mode = NUMA_BALANCING_NORMAL;
	else
		sysctl_numa_balancing_mode = NUMA_BALANCING_DISABLED;
	__set_numabalancing_state(enabled);
}

#ifdef CONFIG_PROC_SYSCTL
int sysctl_numa_balancing(struct ctl_table *table, int write,
			  void *buffer, size_t *lenp, loff_t *ppos)
{
	struct ctl_table t;
	int err;
	int state = sysctl_numa_balancing_mode;

	if (write && !capable(CAP_SYS_ADMIN))
		return -EPERM;

	t = *table;
	t.data = &state;
	err = proc_dointvec_minmax(&t, write, buffer, lenp, ppos);
	if (err < 0)
		return err;
	if (write) {
		sysctl_numa_balancing_mode = state;
		__set_numabalancing_state(state);
	}
	return err;
}
#endif
#endif

#ifdef CONFIG_SCHEDSTATS

DEFINE_STATIC_KEY_FALSE(sched_schedstats);

static void set_schedstats(bool enabled)
{
	if (enabled)
		static_branch_enable(&sched_schedstats);
	else
		static_branch_disable(&sched_schedstats);
}

void force_schedstat_enabled(void)
{
	if (!schedstat_enabled()) {
		pr_info("kernel profiling enabled schedstats, disable via kernel.sched_schedstats.\n");
		static_branch_enable(&sched_schedstats);
	}
}

static int __init setup_schedstats(char *str)
{
	int ret = 0;
	if (!str)
		goto out;

	if (!strcmp(str, "enable")) {
		set_schedstats(true);
		ret = 1;
	} else if (!strcmp(str, "disable")) {
		set_schedstats(false);
		ret = 1;
	}
out:
	if (!ret)
		pr_warn("Unable to parse schedstats=\n");

	return ret;
}
__setup("schedstats=", setup_schedstats);

#ifdef CONFIG_PROC_SYSCTL
int sysctl_schedstats(struct ctl_table *table, int write, void *buffer,
		size_t *lenp, loff_t *ppos)
{
	struct ctl_table t;
	int err;
	int state = static_branch_likely(&sched_schedstats);

	if (write && !capable(CAP_SYS_ADMIN))
		return -EPERM;

	t = *table;
	t.data = &state;
	err = proc_dointvec_minmax(&t, write, buffer, lenp, ppos);
	if (err < 0)
		return err;
	if (write)
		set_schedstats(state);
	return err;
}
#endif /* CONFIG_PROC_SYSCTL */
#endif /* CONFIG_SCHEDSTATS */

/*
 * fork()/clone()-time setup:
 */
int sched_fork(unsigned long clone_flags, struct task_struct *p)
{
	__sched_fork(clone_flags, p);
	/*
	 * We mark the process as NEW here. This guarantees that
	 * nobody will actually run it, and a signal or other external
	 * event cannot wake it up and insert it on the runqueue either.
	 */
	p->__state = TASK_NEW;

	/*
	 * Make sure we do not leak PI boosting priority to the child.
	 */
	p->prio = current->normal_prio;

	uclamp_fork(p);

	/*
	 * Revert to default priority/policy on fork if requested.
	 */
	if (unlikely(p->sched_reset_on_fork)) {
		if (task_has_dl_policy(p) || task_has_rt_policy(p)) {
			p->policy = SCHED_NORMAL;
			p->static_prio = NICE_TO_PRIO(0);
			p->rt_priority = 0;
		} else if (PRIO_TO_NICE(p->static_prio) < 0)
			p->static_prio = NICE_TO_PRIO(0);

		p->prio = p->normal_prio = p->static_prio;
		set_load_weight(p, false);

		/*
		 * We don't need the reset flag anymore after the fork. It has
		 * fulfilled its duty:
		 */
		p->sched_reset_on_fork = 0;
	}

	if (dl_prio(p->prio))
		return -EAGAIN;
	else if (rt_prio(p->prio))
		p->sched_class = &rt_sched_class;
	else
		p->sched_class = &fair_sched_class;

	init_entity_runnable_average(&p->se);


#ifdef CONFIG_SCHED_INFO
	if (likely(sched_info_on()))
		memset(&p->sched_info, 0, sizeof(p->sched_info));
#endif
#if defined(CONFIG_SMP)
	p->on_cpu = 0;
#endif
	init_task_preempt_count(p);
#ifdef CONFIG_SMP
	plist_node_init(&p->pushable_tasks, MAX_PRIO);
	RB_CLEAR_NODE(&p->pushable_dl_tasks);
#endif
	return 0;
}

void sched_cgroup_fork(struct task_struct *p, struct kernel_clone_args *kargs)
{
	unsigned long flags;

	/*
	 * Because we're not yet on the pid-hash, p->pi_lock isn't strictly
	 * required yet, but lockdep gets upset if rules are violated.
	 */
	raw_spin_lock_irqsave(&p->pi_lock, flags);
#ifdef CONFIG_CGROUP_SCHED
	if (1) {
		struct task_group *tg;
		tg = container_of(kargs->cset->subsys[cpu_cgrp_id],
				  struct task_group, css);
		tg = autogroup_task_group(p, tg);
		p->sched_task_group = tg;
	}
#endif
	rseq_migrate(p);
	/*
	 * We're setting the CPU for the first time, we don't migrate,
	 * so use __set_task_cpu().
	 */
	__set_task_cpu(p, smp_processor_id());
	if (p->sched_class->task_fork)
		p->sched_class->task_fork(p);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
}

void sched_post_fork(struct task_struct *p)
{
	uclamp_post_fork(p);
}

unsigned long to_ratio(u64 period, u64 runtime)
{
	if (runtime == RUNTIME_INF)
		return BW_UNIT;

	/*
	 * Doing this here saves a lot of checks in all
	 * the calling paths, and returning zero seems
	 * safe for them anyway.
	 */
	if (period == 0)
		return 0;

	return div64_u64(runtime << BW_SHIFT, period);
}

/*
 * wake_up_new_task - wake up a newly created task for the first time.
 *
 * This function will do some initial scheduler statistics housekeeping
 * that must be done for every newly created context, then puts the task
 * on the runqueue and wakes it.
 */
void wake_up_new_task(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;

	raw_spin_lock_irqsave(&p->pi_lock, rf.flags);
	WRITE_ONCE(p->__state, TASK_RUNNING);
#ifdef CONFIG_SMP
	/*
	 * Fork balancing, do it here and not earlier because:
	 *  - cpus_ptr can change in the fork path
	 *  - any previously selected CPU might disappear through hotplug
	 *
	 * Use __set_task_cpu() to avoid calling sched_class::migrate_task_rq,
	 * as we're not fully set-up yet.
	 */
	p->recent_used_cpu = task_cpu(p);
	rseq_migrate(p);
	__set_task_cpu(p, select_task_rq(p, task_cpu(p), WF_FORK));
#endif
	rq = __task_rq_lock(p, &rf);
	update_rq_clock(rq);
	post_init_entity_util_avg(p);

	activate_task(rq, p, ENQUEUE_NOCLOCK);
	trace_sched_wakeup_new(p);
	check_preempt_curr(rq, p, WF_FORK);
#ifdef CONFIG_SMP
	if (p->sched_class->task_woken) {
		/*
		 * Nothing relies on rq->lock after this, so it's fine to
		 * drop it.
		 */
		rq_unpin_lock(rq, &rf);
		p->sched_class->task_woken(rq, p);
		rq_repin_lock(rq, &rf);
	}
#endif
	task_rq_unlock(rq, p, &rf);
}

#ifdef CONFIG_PREEMPT_NOTIFIERS

static DEFINE_STATIC_KEY_FALSE(preempt_notifier_key);

void preempt_notifier_inc(void)
{
	static_branch_inc(&preempt_notifier_key);
}
EXPORT_SYMBOL_GPL(preempt_notifier_inc);

void preempt_notifier_dec(void)
{
	static_branch_dec(&preempt_notifier_key);
}
EXPORT_SYMBOL_GPL(preempt_notifier_dec);

/**
 * preempt_notifier_register - tell me when current is being preempted & rescheduled
 * @notifier: notifier struct to register
 */
void preempt_notifier_register(struct preempt_notifier *notifier)
{
	if (!static_branch_unlikely(&preempt_notifier_key))
		WARN(1, "registering preempt_notifier while notifiers disabled\n");

	hlist_add_head(&notifier->link, &current->preempt_notifiers);
}
EXPORT_SYMBOL_GPL(preempt_notifier_register);

/**
 * preempt_notifier_unregister - no longer interested in preemption notifications
 * @notifier: notifier struct to unregister
 *
 * This is *not* safe to call from within a preemption notifier.
 */
void preempt_notifier_unregister(struct preempt_notifier *notifier)
{
	hlist_del(&notifier->link);
}
EXPORT_SYMBOL_GPL(preempt_notifier_unregister);

static void __fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
	struct preempt_notifier *notifier;

	hlist_for_each_entry(notifier, &curr->preempt_notifiers, link)
		notifier->ops->sched_in(notifier, raw_smp_processor_id());
}

static __always_inline void fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
	if (static_branch_unlikely(&preempt_notifier_key))
		__fire_sched_in_preempt_notifiers(curr);
}

static void
__fire_sched_out_preempt_notifiers(struct task_struct *curr,
				   struct task_struct *next)
{
	struct preempt_notifier *notifier;

	hlist_for_each_entry(notifier, &curr->preempt_notifiers, link)
		notifier->ops->sched_out(notifier, next);
}

static __always_inline void
fire_sched_out_preempt_notifiers(struct task_struct *curr,
				 struct task_struct *next)
{
	if (static_branch_unlikely(&preempt_notifier_key))
		__fire_sched_out_preempt_notifiers(curr, next);
}

#else /* !CONFIG_PREEMPT_NOTIFIERS */

static inline void fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
}

static inline void
fire_sched_out_preempt_notifiers(struct task_struct *curr,
				 struct task_struct *next)
{
}

#endif /* CONFIG_PREEMPT_NOTIFIERS */

static inline void prepare_task(struct task_struct *next)
{
#ifdef CONFIG_SMP
	/*
	 * Claim the task as running, we do this before switching to it
	 * such that any running task will have this set.
	 *
	 * See the ttwu() WF_ON_CPU case and its ordering comment.
	 */
	WRITE_ONCE(next->on_cpu, 1);
#endif
}

static inline void finish_task(struct task_struct *prev)
{
#ifdef CONFIG_SMP
	/*
	 * This must be the very last reference to @prev from this CPU. After
	 * p->on_cpu is cleared, the task can be moved to a different CPU. We
	 * must ensure this doesn't happen until the switch is completely
	 * finished.
	 *
	 * In particular, the load of prev->state in finish_task_switch() must
	 * happen before this.
	 *
	 * Pairs with the smp_cond_load_acquire() in try_to_wake_up().
	 */
	smp_store_release(&prev->on_cpu, 0);
#endif
}

#ifdef CONFIG_SMP

static void do_balance_callbacks(struct rq *rq, struct callback_head *head)
{
	void (*func)(struct rq *rq);
	struct callback_head *next;

	lockdep_assert_rq_held(rq);

	while (head) {
		func = (void (*)(struct rq *))head->func;
		next = head->next;
		head->next = NULL;
		head = next;

		func(rq);
	}
}

static void balance_push(struct rq *rq);

struct callback_head balance_push_callback = {
	.next = NULL,
	.func = (void (*)(struct callback_head *))balance_push,
};

static inline struct callback_head *splice_balance_callbacks(struct rq *rq)
{
	struct callback_head *head = rq->balance_callback;

	lockdep_assert_rq_held(rq);
	if (head)
		rq->balance_callback = NULL;

	return head;
}

static void __balance_callbacks(struct rq *rq)
{
	do_balance_callbacks(rq, splice_balance_callbacks(rq));
}

static inline void balance_callbacks(struct rq *rq, struct callback_head *head)
{
	unsigned long flags;

	if (unlikely(head)) {
		raw_spin_rq_lock_irqsave(rq, flags);
		do_balance_callbacks(rq, head);
		raw_spin_rq_unlock_irqrestore(rq, flags);
	}
}

#else

static inline void __balance_callbacks(struct rq *rq)
{
}

static inline struct callback_head *splice_balance_callbacks(struct rq *rq)
{
	return NULL;
}

static inline void balance_callbacks(struct rq *rq, struct callback_head *head)
{
}

#endif

static inline void
prepare_lock_switch(struct rq *rq, struct task_struct *next, struct rq_flags *rf)
{
	/*
	 * Since the runqueue lock will be released by the next
	 * task (which is an invalid locking op but in the case
	 * of the scheduler it's an obvious special-case), so we
	 * do an early lockdep release here:
	 */
	rq_unpin_lock(rq, rf);
	spin_release(&__rq_lockp(rq)->dep_map, _THIS_IP_);
#ifdef CONFIG_DEBUG_SPINLOCK
	/* this is a valid case when another task releases the spinlock */
	rq_lockp(rq)->owner = next;
#endif
}

static inline void finish_lock_switch(struct rq *rq)
{
	/*
	 * If we are tracking spinlock dependencies then we have to
	 * fix up the runqueue lock - which gets 'carried over' from
	 * prev into current:
	 */
	spin_acquire(&__rq_lockp(rq)->dep_map, 0, 0, _THIS_IP_);
	__balance_callbacks(rq);
	raw_spin_rq_unlock_irq(rq);
}

/*
 * NOP if the arch has not defined these:
 */

#ifndef prepare_arch_switch
# define prepare_arch_switch(next)	do { } while (0)
#endif

#ifndef finish_arch_post_lock_switch
# define finish_arch_post_lock_switch()	do { } while (0)
#endif

static inline void kmap_local_sched_out(void)
{
#ifdef CONFIG_KMAP_LOCAL
	if (unlikely(current->kmap_ctrl.idx))
		__kmap_local_sched_out();
#endif
}

static inline void kmap_local_sched_in(void)
{
#ifdef CONFIG_KMAP_LOCAL
	if (unlikely(current->kmap_ctrl.idx))
		__kmap_local_sched_in();
#endif
}

/**
 * prepare_task_switch - prepare to switch tasks
 * @rq: the runqueue preparing to switch
 * @prev: the current task that is being switched out
 * @next: the task we are going to switch to.
 *
 * This is called with the rq lock held and interrupts off. It must
 * be paired with a subsequent finish_task_switch after the context
 * switch.
 *
 * prepare_task_switch sets up locking and calls architecture specific
 * hooks.
 */
static inline void
prepare_task_switch(struct rq *rq, struct task_struct *prev,
		    struct task_struct *next)
{
	kcov_prepare_switch(prev);
	sched_info_switch(rq, prev, next);
	perf_event_task_sched_out(prev, next);
	rseq_preempt(prev);
	fire_sched_out_preempt_notifiers(prev, next);
	kmap_local_sched_out();
	prepare_task(next);
	prepare_arch_switch(next);
}

/**
 * finish_task_switch - clean up after a task-switch
 * @prev: the thread we just switched away from.
 *
 * finish_task_switch must be called after the context switch, paired
 * with a prepare_task_switch call before the context switch.
 * finish_task_switch will reconcile locking set up by prepare_task_switch,
 * and do any other architecture-specific cleanup actions.
 *
 * Note that we may have delayed dropping an mm in context_switch(). If
 * so, we finish that here outside of the runqueue lock. (Doing it
 * with the lock held can cause deadlocks; see schedule() for
 * details.)
 *
 * The context switch have flipped the stack from under us and restored the
 * local variables which were saved when this task called schedule() in the
 * past. prev == current is still correct but we need to recalculate this_rq
 * because prev may have moved to another CPU.
 */
static struct rq *finish_task_switch(struct task_struct *prev)
	__releases(rq->lock)
{
	struct rq *rq = this_rq();
	struct mm_struct *mm = rq->prev_mm;
	unsigned int prev_state;

	/*
	 * The previous task will have left us with a preempt_count of 2
	 * because it left us after:
	 *
	 *	schedule()
	 *	  preempt_disable();			// 1
	 *	  __schedule()
	 *	    raw_spin_lock_irq(&rq->lock)	// 2
	 *
	 * Also, see FORK_PREEMPT_COUNT.
	 */
	if (WARN_ONCE(preempt_count() != 2*PREEMPT_DISABLE_OFFSET,
		      "corrupted preempt_count: %s/%d/0x%x\n",
		      current->comm, current->pid, preempt_count()))
		preempt_count_set(FORK_PREEMPT_COUNT);

	rq->prev_mm = NULL;

	/*
	 * A task struct has one reference for the use as "current".
	 * If a task dies, then it sets TASK_DEAD in tsk->state and calls
	 * schedule one last time. The schedule call will never return, and
	 * the scheduled task must drop that reference.
	 *
	 * We must observe prev->state before clearing prev->on_cpu (in
	 * finish_task), otherwise a concurrent wakeup can get prev
	 * running on another CPU and we could rave with its RUNNING -> DEAD
	 * transition, resulting in a double drop.
	 */
	prev_state = READ_ONCE(prev->__state);
	vtime_task_switch(prev);
	perf_event_task_sched_in(prev, current);
	finish_task(prev);
	tick_nohz_task_switch();
	finish_lock_switch(rq);
	finish_arch_post_lock_switch();
	kcov_finish_switch(current);
	/*
	 * kmap_local_sched_out() is invoked with rq::lock held and
	 * interrupts disabled. There is no requirement for that, but the
	 * sched out code does not have an interrupt enabled section.
	 * Restoring the maps on sched in does not require interrupts being
	 * disabled either.
	 */
	kmap_local_sched_in();

	fire_sched_in_preempt_notifiers(current);
	/*
	 * When switching through a kernel thread, the loop in
	 * membarrier_{private,global}_expedited() may have observed that
	 * kernel thread and not issued an IPI. It is therefore possible to
	 * schedule between user->kernel->user threads without passing though
	 * switch_mm(). Membarrier requires a barrier after storing to
	 * rq->curr, before returning to userspace, so provide them here:
	 *
	 * - a full memory barrier for {PRIVATE,GLOBAL}_EXPEDITED, implicitly
	 *   provided by mmdrop(),
	 * - a sync_core for SYNC_CORE.
	 */
	if (mm) {
		membarrier_mm_sync_core_before_usermode(mm);
		mmdrop_sched(mm);
	}
	if (unlikely(prev_state == TASK_DEAD)) {
		if (prev->sched_class->task_dead)
			prev->sched_class->task_dead(prev);

		/* Task is done with its stack. */
		put_task_stack(prev);

		put_task_struct_rcu_user(prev);
	}

	return rq;
}

/**
 * schedule_tail - first thing a freshly forked thread must call.
 * @prev: the thread we just switched away from.
 */
asmlinkage __visible void schedule_tail(struct task_struct *prev)
	__releases(rq->lock)
{
	/*
	 * New tasks start with FORK_PREEMPT_COUNT, see there and
	 * finish_task_switch() for details.
	 *
	 * finish_task_switch() will drop rq->lock() and lower preempt_count
	 * and the preempt_enable() will end up enabling preemption (on
	 * PREEMPT_COUNT kernels).
	 */

	finish_task_switch(prev);
	preempt_enable();

	if (current->set_child_tid)
		put_user(task_pid_vnr(current), current->set_child_tid);

	calculate_sigpending();
}

/*
 * context_switch - switch to the new MM and the new thread's register state.
 */
static __always_inline struct rq *
context_switch(struct rq *rq, struct task_struct *prev,
	       struct task_struct *next, struct rq_flags *rf)
{
	prepare_task_switch(rq, prev, next);

	/*
	 * For paravirt, this is coupled with an exit in switch_to to
	 * combine the page table reload and the switch backend into
	 * one hypercall.
	 */
	arch_start_context_switch(prev);

	/*
	 * kernel -> kernel   lazy + transfer active
	 *   user -> kernel   lazy + mmgrab() active
	 *
	 * kernel ->   user   switch + mmdrop() active
	 *   user ->   user   switch
	 */
	if (!next->mm) {                                // to kernel
		enter_lazy_tlb(prev->active_mm, next);

		next->active_mm = prev->active_mm;
		if (prev->mm)                           // from user
			mmgrab(prev->active_mm);
		else
			prev->active_mm = NULL;
	} else {                                        // to user
		membarrier_switch_mm(rq, prev->active_mm, next->mm);
		/*
		 * sys_membarrier() requires an smp_mb() between setting
		 * rq->curr / membarrier_switch_mm() and returning to userspace.
		 *
		 * The below provides this either through switch_mm(), or in
		 * case 'prev->active_mm == next->mm' through
		 * finish_task_switch()'s mmdrop().
		 */
		switch_mm_irqs_off(prev->active_mm, next->mm, next);

		if (!prev->mm) {                        // from kernel
			/* will mmdrop() in finish_task_switch(). */
			rq->prev_mm = prev->active_mm;
			prev->active_mm = NULL;
		}
	}

	rq->clock_update_flags &= ~(RQCF_ACT_SKIP|RQCF_REQ_SKIP);

	prepare_lock_switch(rq, next, rf);

	/* Here we just switch the register state and the stack. */
	switch_to(prev, next, prev);
	barrier();

	return finish_task_switch(prev);
}

/*
 * nr_running and nr_context_switches:
 *
 * externally visible scheduler statistics: current number of runnable
 * threads, total number of context switches performed since bootup.
 */
unsigned int nr_running(void)
{
	unsigned int i, sum = 0;

	for_each_online_cpu(i)
		sum += cpu_rq(i)->nr_running;

	return sum;
}

/*
 * Check if only the current task is running on the CPU.
 *
 * Caution: this function does not check that the caller has disabled
 * preemption, thus the result might have a time-of-check-to-time-of-use
 * race.  The caller is responsible to use it correctly, for example:
 *
 * - from a non-preemptible section (of course)
 *
 * - from a thread that is bound to a single CPU
 *
 * - in a loop with very short iterations (e.g. a polling loop)
 */
bool single_task_running(void)
{
	return raw_rq()->nr_running == 1;
}
EXPORT_SYMBOL(single_task_running);

unsigned long long nr_context_switches(void)
{
	int i;
	unsigned long long sum = 0;

	for_each_possible_cpu(i)
		sum += cpu_rq(i)->nr_switches;

	return sum;
}

/*
 * Consumers of these two interfaces, like for example the cpuidle menu
 * governor, are using nonsensical data. Preferring shallow idle state selection
 * for a CPU that has IO-wait which might not even end up running the task when
 * it does become runnable.
 */

unsigned int nr_iowait_cpu(int cpu)
{
	return atomic_read(&cpu_rq(cpu)->nr_iowait);
}

/*
 * IO-wait accounting, and how it's mostly bollocks (on SMP).
 *
 * The idea behind IO-wait account is to account the idle time that we could
 * have spend running if it were not for IO. That is, if we were to improve the
 * storage performance, we'd have a proportional reduction in IO-wait time.
 *
 * This all works nicely on UP, where, when a task blocks on IO, we account
 * idle time as IO-wait, because if the storage were faster, it could've been
 * running and we'd not be idle.
 *
 * This has been extended to SMP, by doing the same for each CPU. This however
 * is broken.
 *
 * Imagine for instance the case where two tasks block on one CPU, only the one
 * CPU will have IO-wait accounted, while the other has regular idle. Even
 * though, if the storage were faster, both could've ran at the same time,
 * utilising both CPUs.
 *
 * This means, that when looking globally, the current IO-wait accounting on
 * SMP is a lower bound, by reason of under accounting.
 *
 * Worse, since the numbers are provided per CPU, they are sometimes
 * interpreted per CPU, and that is nonsensical. A blocked task isn't strictly
 * associated with any one particular CPU, it can wake to another CPU than it
 * blocked on. This means the per CPU IO-wait number is meaningless.
 *
 * Task CPU affinities can make all that even more 'interesting'.
 */

unsigned int nr_iowait(void)
{
	unsigned int i, sum = 0;

	for_each_possible_cpu(i)
		sum += nr_iowait_cpu(i);

	return sum;
}

#ifdef CONFIG_SMP

/*
 * sched_exec - execve() is a valuable balancing opportunity, because at
 * this point the task has the smallest effective memory and cache footprint.
 */
void sched_exec(void)
{
	struct task_struct *p = current;
	unsigned long flags;
	int dest_cpu;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	dest_cpu = p->sched_class->select_task_rq(p, task_cpu(p), WF_EXEC);
	if (dest_cpu == smp_processor_id())
		goto unlock;

	if (likely(cpu_active(dest_cpu))) {
		struct migration_arg arg = { p, dest_cpu };

		raw_spin_unlock_irqrestore(&p->pi_lock, flags);
		stop_one_cpu(task_cpu(p), migration_cpu_stop, &arg);
		return;
	}
unlock:
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
}

#endif

DEFINE_PER_CPU(struct kernel_stat, kstat);
DEFINE_PER_CPU(struct kernel_cpustat, kernel_cpustat);

EXPORT_PER_CPU_SYMBOL(kstat);
EXPORT_PER_CPU_SYMBOL(kernel_cpustat);

/*
 * The function fair_sched_class.update_curr accesses the struct curr
 * and its field curr->exec_start; when called from task_sched_runtime(),
 * we observe a high rate of cache misses in practice.
 * Prefetching this data results in improved performance.
 */
static inline void prefetch_curr_exec_start(struct task_struct *p)
{
#ifdef CONFIG_FAIR_GROUP_SCHED
	struct sched_entity *curr = (&p->se)->cfs_rq->curr;
#else
	struct sched_entity *curr = (&task_rq(p)->cfs)->curr;
#endif
	prefetch(curr);
	prefetch(&curr->exec_start);
}

/*
 * Return accounted runtime for the task.
 * In case the task is currently running, return the runtime plus current's
 * pending runtime that have not been accounted yet.
 */
unsigned long long task_sched_runtime(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;
	u64 ns;

#if defined(CONFIG_64BIT) && defined(CONFIG_SMP)
	/*
	 * 64-bit doesn't need locks to atomically read a 64-bit value.
	 * So we have a optimization chance when the task's delta_exec is 0.
	 * Reading ->on_cpu is racy, but this is ok.
	 *
	 * If we race with it leaving CPU, we'll take a lock. So we're correct.
	 * If we race with it entering CPU, unaccounted time is 0. This is
	 * indistinguishable from the read occurring a few cycles earlier.
	 * If we see ->on_cpu without ->on_rq, the task is leaving, and has
	 * been accounted, so we're correct here as well.
	 */
	if (!p->on_cpu || !task_on_rq_queued(p))
		return p->se.sum_exec_runtime;
#endif

	rq = task_rq_lock(p, &rf);
	/*
	 * Must be ->curr _and_ ->on_rq.  If dequeued, we would
	 * project cycles that may never be accounted to this
	 * thread, breaking clock_gettime().
	 */
	if (task_current(rq, p) && task_on_rq_queued(p)) {
		prefetch_curr_exec_start(p);
		update_rq_clock(rq);
		p->sched_class->update_curr(rq);
	}
	ns = p->se.sum_exec_runtime;
	task_rq_unlock(rq, p, &rf);

	return ns;
}

#ifdef CONFIG_SCHED_DEBUG
static u64 cpu_resched_latency(struct rq *rq)
{
	int latency_warn_ms = READ_ONCE(sysctl_resched_latency_warn_ms);
	u64 resched_latency, now = rq_clock(rq);
	static bool warned_once;

	if (sysctl_resched_latency_warn_once && warned_once)
		return 0;

	if (!need_resched() || !latency_warn_ms)
		return 0;

	if (system_state == SYSTEM_BOOTING)
		return 0;

	if (!rq->last_seen_need_resched_ns) {
		rq->last_seen_need_resched_ns = now;
		rq->ticks_without_resched = 0;
		return 0;
	}

	rq->ticks_without_resched++;
	resched_latency = now - rq->last_seen_need_resched_ns;
	if (resched_latency <= latency_warn_ms * NSEC_PER_MSEC)
		return 0;

	warned_once = true;

	return resched_latency;
}

static int __init setup_resched_latency_warn_ms(char *str)
{
	long val;

	if ((kstrtol(str, 0, &val))) {
		pr_warn("Unable to set resched_latency_warn_ms\n");
		return 1;
	}

	sysctl_resched_latency_warn_ms = val;
	return 1;
}
__setup("resched_latency_warn_ms=", setup_resched_latency_warn_ms);
#else
static inline u64 cpu_resched_latency(struct rq *rq) { return 0; }
#endif /* CONFIG_SCHED_DEBUG */

/*
 * This function gets called by the timer code, with HZ frequency.
 * We call it with interrupts disabled.
 */
void scheduler_tick(void)
{
	int cpu = smp_processor_id();
	struct rq *rq = cpu_rq(cpu);
	struct task_struct *curr = rq->curr;
	struct rq_flags rf;
	unsigned long thermal_pressure;
	u64 resched_latency;

	arch_scale_freq_tick();
	sched_clock_tick();

	rq_lock(rq, &rf);

	update_rq_clock(rq);
	thermal_pressure = arch_scale_thermal_pressure(cpu_of(rq));
	update_thermal_load_avg(rq_clock_thermal(rq), rq, thermal_pressure);
	curr->sched_class->task_tick(rq, curr, 0);
	if (sched_feat(LATENCY_WARN))
		resched_latency = cpu_resched_latency(rq);
	calc_global_load_tick(rq);
	sched_core_tick(rq);

	rq_unlock(rq, &rf);

	if (sched_feat(LATENCY_WARN) && resched_latency)
		resched_latency_warn(cpu, resched_latency);

	perf_event_task_tick();

#ifdef CONFIG_SMP
	rq->idle_balance = idle_cpu(cpu);
	trigger_load_balance(rq);
#endif
}

#ifdef CONFIG_NO_HZ_FULL

struct tick_work {
	int			cpu;
	atomic_t		state;
	struct delayed_work	work;
};
/* Values for ->state, see diagram below. */
#define TICK_SCHED_REMOTE_OFFLINE	0
#define TICK_SCHED_REMOTE_OFFLINING	1
#define TICK_SCHED_REMOTE_RUNNING	2

/*
 * State diagram for ->state:
 *
 *
 *          TICK_SCHED_REMOTE_OFFLINE
 *                    |   ^
 *                    |   |
 *                    |   | sched_tick_remote()
 *                    |   |
 *                    |   |
 *                    +--TICK_SCHED_REMOTE_OFFLINING
 *                    |   ^
 *                    |   |
 * sched_tick_start() |   | sched_tick_stop()
 *                    |   |
 *                    V   |
 *          TICK_SCHED_REMOTE_RUNNING
 *
 *
 * Other transitions get WARN_ON_ONCE(), except that sched_tick_remote()
 * and sched_tick_start() are happy to leave the state in RUNNING.
 */

static struct tick_work __percpu *tick_work_cpu;

static void sched_tick_remote(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct tick_work *twork = container_of(dwork, struct tick_work, work);
	int cpu = twork->cpu;
	struct rq *rq = cpu_rq(cpu);
	struct task_struct *curr;
	struct rq_flags rf;
	u64 delta;
	int os;

	/*
	 * Handle the tick only if it appears the remote CPU is running in full
	 * dynticks mode. The check is racy by nature, but missing a tick or
	 * having one too much is no big deal because the scheduler tick updates
	 * statistics and checks timeslices in a time-independent way, regardless
	 * of when exactly it is running.
	 */
	if (!tick_nohz_tick_stopped_cpu(cpu))
		goto out_requeue;

	rq_lock_irq(rq, &rf);
	curr = rq->curr;
	if (cpu_is_offline(cpu))
		goto out_unlock;

	update_rq_clock(rq);

	if (!is_idle_task(curr)) {
		/*
		 * Make sure the next tick runs within a reasonable
		 * amount of time.
		 */
		delta = rq_clock_task(rq) - curr->se.exec_start;
		WARN_ON_ONCE(delta > (u64)NSEC_PER_SEC * 3);
	}
	curr->sched_class->task_tick(rq, curr, 0);

	calc_load_nohz_remote(rq);
out_unlock:
	rq_unlock_irq(rq, &rf);
out_requeue:

	/*
	 * Run the remote tick once per second (1Hz). This arbitrary
	 * frequency is large enough to avoid overload but short enough
	 * to keep scheduler internal stats reasonably up to date.  But
	 * first update state to reflect hotplug activity if required.
	 */
	os = atomic_fetch_add_unless(&twork->state, -1, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_OFFLINE);
	if (os == TICK_SCHED_REMOTE_RUNNING)
		queue_delayed_work(system_unbound_wq, dwork, HZ);
}

static void sched_tick_start(int cpu)
{
	int os;
	struct tick_work *twork;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_RUNNING);
	if (os == TICK_SCHED_REMOTE_OFFLINE) {
		twork->cpu = cpu;
		INIT_DELAYED_WORK(&twork->work, sched_tick_remote);
		queue_delayed_work(system_unbound_wq, &twork->work, HZ);
	}
}

#ifdef CONFIG_HOTPLUG_CPU
static void sched_tick_stop(int cpu)
{
	struct tick_work *twork;
	int os;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	/* There cannot be competing actions, but don't rely on stop-machine. */
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_OFFLINING);
	WARN_ON_ONCE(os != TICK_SCHED_REMOTE_RUNNING);
	/* Don't cancel, as this would mess up the state machine. */
}
#endif /* CONFIG_HOTPLUG_CPU */

int __init sched_tick_offload_init(void)
{
	tick_work_cpu = alloc_percpu(struct tick_work);
	BUG_ON(!tick_work_cpu);
	return 0;
}

#else /* !CONFIG_NO_HZ_FULL */
static inline void sched_tick_start(int cpu) { }
static inline void sched_tick_stop(int cpu) { }
#endif

#if defined(CONFIG_PREEMPTION) && (defined(CONFIG_DEBUG_PREEMPT) || \
				defined(CONFIG_TRACE_PREEMPT_TOGGLE))
/*
 * If the value passed in is equal to the current preempt count
 * then we just disabled preemption. Start timing the latency.
 */
static inline void preempt_latency_start(int val)
{
	if (preempt_count() == val) {
		unsigned long ip = get_lock_parent_ip();
#ifdef CONFIG_DEBUG_PREEMPT
		current->preempt_disable_ip = ip;
#endif
		trace_preempt_off(CALLER_ADDR0, ip);
	}
}

void preempt_count_add(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON((preempt_count() < 0)))
		return;
#endif
	__preempt_count_add(val);
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Spinlock count overflowing soon?
	 */
	DEBUG_LOCKS_WARN_ON((preempt_count() & PREEMPT_MASK) >=
				PREEMPT_MASK - 10);
#endif
	preempt_latency_start(val);
}
EXPORT_SYMBOL(preempt_count_add);
NOKPROBE_SYMBOL(preempt_count_add);

/*
 * If the value passed in equals to the current preempt count
 * then we just enabled preemption. Stop timing the latency.
 */
static inline void preempt_latency_stop(int val)
{
	if (preempt_count() == val)
		trace_preempt_on(CALLER_ADDR0, get_lock_parent_ip());
}

void preempt_count_sub(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON(val > preempt_count()))
		return;
	/*
	 * Is the spinlock portion underflowing?
	 */
	if (DEBUG_LOCKS_WARN_ON((val < PREEMPT_MASK) &&
			!(preempt_count() & PREEMPT_MASK)))
		return;
#endif

	preempt_latency_stop(val);
	__preempt_count_sub(val);
}
EXPORT_SYMBOL(preempt_count_sub);
NOKPROBE_SYMBOL(preempt_count_sub);

#else
static inline void preempt_latency_start(int val) { }
static inline void preempt_latency_stop(int val) { }
#endif

static inline unsigned long get_preempt_disable_ip(struct task_struct *p)
{
#ifdef CONFIG_DEBUG_PREEMPT
	return p->preempt_disable_ip;
#else
	return 0;
#endif
}

/*
 * Print scheduling while atomic bug:
 */
static noinline void __schedule_bug(struct task_struct *prev)
{
	/* Save this before calling printk(), since that will clobber it */
	unsigned long preempt_disable_ip = get_preempt_disable_ip(current);

	if (oops_in_progress)
		return;

	printk(KERN_ERR "BUG: scheduling while atomic: %s/%d/0x%08x\n",
		prev->comm, prev->pid, preempt_count());

	debug_show_held_locks(prev);
	priΞΘϋmo(:ι“ΟMΏI•ϋ»―r―@γX[νqNνZω:™ύBΉΒ΅BξόΜ/KF^„ νψΛΘ8^Έ5]ζΏ#κKµF`›Cy~!;+ΗκΗXΎ¥,Β–Τ4•ω­Ίίω6r;3¤/UhƒΏ %ύ 0΄40tΫ)µρχAω>J-Ζ=Α†DT»xz¶Aι4!d:®(*Ρ!1HΝ½¦h’¨δƒuVAχ¥F„Vεµ7	°ZΏjΐM•ζuύq(Gιλ—ÿ @οχ Ν΅Ρ”Ξ!{πQ&!`B²ΨΥψ6‚λ΄EΤ‹Rf›Ιϋ#Ό§λ@0“εv9έ~οΌΏξΆ-έ>Μ/S¬“[|,3r„ίO#–¨θeM„Ύώª‹@	Σ­ω«α'σc24W‰·y•0ςψ€Ψ8CΞ
ΑXέ84‚®οο9°4ΐΣWδΧσ%LΐSfY°°ό2”L’¦¥ΓΏ™v¦•”bς aU}΄¬θx¶ΕBfβ±ό‘©H¬‚?®K< “‘‹ΫK}ψOη¶n”‘NN΄BΫAΟrtόωw»ΞyιTδωuUκ_]SΠhq]ΟΕ%‚C΄OάƒψOΩοΨXS2J¨AϊIλΤBUΨώυΕw<ό1‡ÒsΫύΈ€)¨ξ•r_―mλ—'Ώ`(Ύρn{γσ(zεΥz¨·ψ@­#δΆ‹¬Ϋq°/y“έ&*Λ—”eι[χNΆ§QΟώΓ§0qO΄a¶UC[8Ò0©ÿ~»JªΥθƒHήµr²rgk4cRHfz.=w)‡ΝίΩ»iFVδ1B}i<ΤΝW“ΫZ>ηΧ„ReΏ·v¤WWeΈ‘‘#A2%r“»Y©Ε2ψ¤ή®*\hΏnέ–„‹ƒ"Ν	μ‘Γb? ¤Η΅poι#zφΔ”Φςέc“?‰·ϋBχe–ΐΪμ½µ	ΐϋk΄"©ƒθ~Bkbi0δR-T…<5z<.γ3?_|χ§¦[µBTa8,¥
d#ςΔΡΓϊe5Σ°[d,΅…ΟN+λΨo|ƒΔβ"δΙqΕXύc0¥έJέ4<0δξΪτΌ ­ΐ‹[πSΨΰC«3‹ΖJρsW¶²Πβ†mά‘N)gc7¨iΠα¨<5οιUΒV[ϊ:?ΙD°YΚΰÒqΡΜX%‰ψ£Ψ[¤2Γ)‡ΫόΣΗ΅·γ{”'e‚ι&z;Κ”ΝQWOΐ«ΨΩΨ¤°#ΡuΞ΄ΖϊW=Ϋb »ϋyϋkb{’_lφ{©ϋT38&Q„!*awρ‡)*F2|]I;$¤:6–°Κ¨δ+@ωzΟg]ζ8Ή‹®KcEτΝΟz 5ΈμΞΉÿdTVx› Β@1Ϋ“#½3µΞ§Ρ¤›%®(7Υ„@ΨΕ<(f	ΑMύhεR*Ω&ύ*4”6”ύΞÒl½7φ,¦~=ªΈ¦Σιw"ΥΕnφ­|%Ρ€,ψ&ƒ|ζήeΨΞg~$5ΘO$Βή¤Q΅‹iΗ>ΰE>C‹
KΒ ρ Ρ;l£^3«Ύ=ςω»Ί:2χa¬ekyφ*η±€ ζµΡrχ½Εύtl“ΎMƒδQόrΤE•Ύ·~Ό‚™¶’Ό½ ¥3}¥hΩhΙLΡύC#Ν
RδPΠsrdλΤ`!e.,*(:ή §dV(,ς ωdgμΓ‚αΈ:ξ>…®F^Eκ„A+!Ο/xύ―7t[gά£Ά§®#ώkΌΨλ0~&&v;ψΆ€z®y/ζνªγ"Χ)αG–Χλ L#5ΖM^θ&—ΡΡL½ωΊ³³E¥nw8ΉΥ<’uoqAΘΨL&Έ›ΐη•GιEÿ…Δτ‹sZρ‰IgSΨ υXτΦ)τ¥”I¶ΞƒαδΤ•'W¥>‹lPϊΜΓ&ό9ij.WDτ R»*―αΞ΄πªΤ]XΟ1εώ½T-ΐγΝD‚q<Ε|STΐής;Θκ2RΈ1”;.pa-Ε.¬rhΏt"&Εk«κΙψ/zo―YΈUyJΥJ”…wΪs#OΰΧ‰qƒa'ΏHÒΆ8‚iyςβΆ~ό£w:_ό2Ί¦i~0U±a'‘ QZΤ΄Φµk…\ι€ΪέώIφq·ή¨ΣIr^gg)§θΉ€¤¶
›: M6ϋΡ­1βzDΞΡ7-οω¶Zϋ£UθΚb–;’ΆΛ­{}™ΏQ£*’½d±
r®WΪρa«m»tYMuρTÿ½£`tιKψρΩuΆ„ΫώƒΫR[FΝ‰oγΥΟ;sδγΉΫVΝΟΠ·}5j~
oΧ©ϊ)¦ϋπ…Ύ:¶\¶E*J’¨L7I@‡ϋ”zΛΎΖ8eΜυ-^‚΄™Θν?ψJIφώD@ªEΌ·_―TwUέέsΣΘΛ©Βμ΄―ƒ«,θ©©#ªΟm®DnªΜ)PbΫ Mjπ8bΩ°Α
©
αq¤[†.½wX]£Ώn‡2+·ϊΑ[Β¶'τCΪkR¶ηέΟ ;ε΄―RbΓ.	¦«¤•)uΟ¶‹P^‘Ά0‚xGΑπrσμΘψm€ζψ.=h"α¬®|”[ΛD¬=sύ©\\U4l΅;ρϋ?lί‰¨¨―v… Ϋσψσ)γRw‰½DΆγΛqf^ΠΙζIΡª¦Δ„Ωψ¦”7K^Y:ΦCξ,D`ΠdªRΫJ`Ρϋ}dG―— ξν΅ηΒy‹µβ?›τάƒΜ‚†srρ’Θv%φ™£ωÿ8υ*²lT0€z£\Ψ*azΗ8­{βgυΥ_ψΆaΨΣΡ+MεΞΈ/‹Knν3;β±/XίZ»”ϊT…'u/«Τ–φ5‰fξΠ~ΉΧ¤xμ61ΐΙhΠπυϊ`γ\ίό1*γM„Κ |1υQ§§rρIz»Y³ΪΥ¥Ϊƒά¶¥5”Φ¨¦[Oόt¦e`v“ξϋbΒX¤΅φ1,t„μ8‘‹r`lY—ξ Γ‰F_ί(ley¨σ$u6%'¬Ω!Bαvηa£S`¤ΰύνZΘΚ68Ξ›ΑΞςk·βΨΰ¶–g΅ώθTSVΚΆ.£$5³δμ…α	ΎT―ÿ…(ΝS	yrhp“†§$Hη΄γ;(¤,ΊτΔi$[„NHΉ\η xΤle¨3Άζκ‹βΪ.rKθbμύΊξωyƒ­χnΙ–Ί ΏϊΝªθÿµQ‰ΞΙ@εΝƒάQLΘSοη_¦ai¨Τίδ-έ&±-ΤrπF„i	ΐZΥs<9―Μ‰“<"ή3έ”θ3Eb" µ΄*d’AΤέ΅‘!τ]hΨζÿΛ’)οόxΔEΰΥΐµ³¬©¶άµΌ{N>('—Vδ1ƒρ«£JβªGJf75³ue¦1ΨΥ ¥Qθ‹O[«§ηΟd1ή—,ςΗ@rveη”ήRh™®σ(YΫ—ÒΏ¶ΜΏ¥[—“Α$C½»Y‘ηΎϊMΟτ³η$™§@L¬mlΚαβ}©¥WBwÒm|‘bΑBFΤ,V{ΐ2B#("-iΥΈ_W ΒΉ θ‚"¨—Sÿ„ φΓ6Νο²€έο­ϋΒHf²^Y (…s%VD«ό™2•qι²y§1Χhγ›¦«µIΙ&‹‚ζ=,t•άbXQhΕ[ªry/Ά¨ύKwvEN³΅Px@{ΐΏ·ΈL‘<³‹¶ΥB/¬VΜ„ΊMFΪ!Cο.{”~£tΒ¨…XR‚π±^i‹>–Φ	Ϋ™]#ξΌςΤμy ψ9²Ά:KrνεΐE‘®JTN Fq‡Α_q yϋƒ±ΰψ]ο»J‰“.γ²~Ύ@,Υf96ε6~D…ugΰZτΘΊ‚ΐyb/eΝαΪ$‡Ν?vh·™ιρdα+χώΥξ©²›TόΧ―ώ΅ΊW/»‹^Y3®Gnέ%[fA-™oς2|S½t5­#/—¨φ~6g‹Ε«s°ηυjςhςϊ•Νe%ϊ¥IV1ƒ@ζ©ί6;δΛϊ³ÿ”Ρ+kQY#Κ§I"‹Ε>`k°B9_Jύ›b[Ν3#E…|‘ΏpήÿGwhΙχgαΉν+ψRχΡr¶jΉΨH%Ω+›vφƒψ‚©ª6Υª¶"xshs¦βBφγέήρUΆ%g±fΠ`ΣUΞ¶Ιpκ¬ϋΘoμ<>?ΎC―ÿ…ÿ†&Οι"%–l¤$bxm_!Έ1ιAΤq&ϋκΗνΓ‘sTβΠ#µυqN]Ϋ‹Πg„ρ74BόhύύQΎ”?™δdΫcΪΕ¦:nΈX:h‹H5ΪO30Xvt–―B‹^m—)a°^}’5wz6β}΄ύ¶‘8]vUΔΚ»Z=$mβ
ΒY•·ÿBΜ™Ά‡0ή΄·/ΈσuδΖώ†΅¥θΈOB7―Τ‚_'ήp-gaΖΎ€®wΫT-88hƒΉi·Ό–9ύwDB†…[Ρ¬Λ*±^#SƒgδÒ–ΣθΆRΦ	$…ϊHS”ζΔ\¤ς›οAπ$Γ†Ψ8·MÒΞyφ°nP&,΄/X(Q~%CBªχτBω<Λό’Βπ© T7…lξ·ΘLΈ2`οP•>ΰ†]ÒδΫΩK—†Λ$Ν‡ι„¤‘Ι«ΈΐKizσΤή΅κ…Ό|U9πβraUc,ΩΙ.\Bγ&½S®%¤"Y¦»X	ΙυνκI μdV‚8λ]ςabΆ~p>ξέiδ?!ΫBΑ •‹%Έίγ6’\Ϊ6‰~Πξ‚Π8•Ώ%Ό
w’χVΕm/¤†¬)uKt“›S]X=·%%§(ϋjΚτμΞ”PΔηΥ$ΗM!	IEHόi«ιD¥¶Τ,
ΏXΐΚ6^ώΕ"+>Ψ&"ύΫª* ]μ#)zξΪ­±ωl*²M!#D§ao0σ΅!·Β―ςhρΓdί<•{>‰η^Mι¬OfV¤s4σΝnm±8“χ:Ο)δS.O`2Qή“6ι‚ΰBu¬6„q„ µo@ΗgMκτ†q!b@q΄9&χÿ‹/»M“·e΄9Xρ`ΫΞ[ΦήΰN’/Υ¦΅1LªUΘG=/γθ~Ϋ1 ”«N=Ϋsdφ&Θΰfb®	‡’jΣ'χεΰj‡ΞXΨ= z;’kη9#W4ασ
ΨΡΟ??ώ8	a΅`ψ,λ‚~νLΉΑ™z&UΰfΉ“¶tLΘυΫVδ•²>N£.|»
α½¥,θ-ϋÒYρΰ—ϊwΥΪΈw†βΜ±AΣvΥEύf‹Ϊ½ΐηwξ¤σλ+‰Ή¬ΥΒu<.Γ›:½’φΣyν€·δSO°µ\o»££Ρgχ‰ν`E­―hΑ@ΣΝυ-Βd}J„Κ•Ή1 Λδ°-ΡTm5Ε0"ΎΦ>πΪPΣ¤£ζΈ΅bΣUΙοΧΒ]σ®ν’]WύOΕ/Ύ`j,*ξ¬9ΔGχBάwΣ%~(α2β¥—Ί(ί/ΨbhοF…—άό%5Bδ»Ϊ;L§‡o“Π–[Έuτί	gΔκxΩ#θλµI¦a ΰ΄ψ06_υψ½°Χ
®σ^ρ™p’){εq,dƒEDω=ά6›5οΧχμΑD¥YΦ1’σU,$χΡΦKύ#™ρƒσΞκ7ω™orΠςIªδ(²―ζ/?ΔΑJνMΛ­	[	Η0Δ΄»®2PMΩ¥6Qιε Κ®·=‡hƒΆςJΌϊ\ΐΛD7°Έ§wΚΣΈψf½p«ίQΈ:Ωhÿ8›Czο¶+―‹Φ@ÿΫα›xύωj@|ΙmϋΑ°xlCY-cp-άΖlΏ΅?=ΐ”ΐ6Ύ—rMΣKKϋ*―ϊάώ	 Οδv"Υι;Λωυ²Ϋ«»’gΆAΡt±βνΫ½v­QΣ=ϋΞΑΒ‹ Δ—a¤ώχz―Y‘.dG{e>IcrO¬%ά4τ2’ΰν5Έsύ%hx«nƒ!αmγµτΝ€Σ-Y‘³βq»μ›	¥emΝhηΙ«iθw³]©‘©Δ#%…Φ‡°§8φP ΅JDµxF,>Η;…Βϊυ ηq‘ωη‚4ΑοJ•ό[Σ|iτΙΌC+uΨΒ·Dτ*UΪΣW|.<­‹†S··Κ—’ΆΚ{ΒΧ›ηΩτz‹ο-”»ΚΦH]ή΅΄4Υd–―³βPαpwβΌV/ώΖB»Δ«IVΒ# ‹ό=jωΎ8$FvΫyΩ;%Ώξ¥ΩΖxY,β^κ®°”Υ|‘νjσB'OvΗγέ¬j? ζΆ%¨”½{ΗΪJΊiπJαΏ€Γύ±AWΙυOΒ™εΥ ΓΘιSp+β"”;Ι9MΑ8ªΥ·*M»Z–ύφbo΅™ƒθLόΔΈ,nυ$…2‘‘Ρ(yU:ωu#2*Kόh:ΆzTVGβτΊP|gg>›)―ÿWi>¦ΞS8UN8‹Ήΐ―®)υ―<›f$ύΔYª,Κ@GΎvo.ΛOΝ€N›oƒΊ‡Τ[όxΗk<R`£ό¤
τt¥|ί2‡={¦+½ω'¨΄FZ·π`πΣuz7­—`¤ΔΙyÿλ{¥κE!4dΝύ·l™ς‰ Dbϋ.*Cψ£ª'{vλÒtΗΛΟ:Ι%KλmΞ,Dζ/v >Έ²bη§Γ ξΆV‚Ό~­Ύία²―΅>·e„νvήwΔ.qϊ]ΰΜΆΆ!-φΝΝU ~
Ψ+(Ξ2pµ‹\Y·UΑ΄Υy–―Κ[­ΛΣυ[Α°±Gs2¤-e·φ_ρ΅Πj”ϋΎ0UΓΖ€Ξ„χ εΆ,²—Π΅=>µφΙÒm/«=ίΌ‘n·v± F“¨^BsnÿΩ±ΰά%Iβ΅™ξ.vf*P1Ϊ¬α±LOÒι©υλaΉƒAµ>Μ¤CAO!’D‹[ξΞςΣ+ΕΜC&Ύ<7•@ΟZΔΦτKπ7Ώ»2„αΏψBmSDάCtx?,ώCΓƒΩ±
oZτ±ϊ¶iΛX‰<2Ò΅ΤdΟ{ω‹EΎΐυρ…]Π™ª%Θ¤u§‘α‰antpΊM
Jc†k&ίAΙQ_Κ‡‡D‹OΏGΖχ)©dσΟοχg®ς ,ΏΆ'mOΔχµρφρ‹ÿ\·Ϋy8Q›ο5νψSDΪ<Ήp(ΏΘ?Φέ—ΟFª{λ~‰δ{H/FL OΏY‰Ό<‚ρª†u{¬Ξ^«u…‰®s©π®τ4wªχfE_·f›ΗΔΛ»VCθ/r–_…›W‰ΎH?Mv"£a•$“aC%°)ή‚Ο"~ΜΑβΠYχλδάή7)Χn;$M“–m¶*AμpθXΘƒΞ/Hyµ½N2<πΆr„1η‰s‘κζ‡^<EΟ>ω‘Θ3λsΎv3² £δηβQ΄»Ά	g-Km*.εϊLhνIif#Κ :™δοΣcά!Σκήέ¥ΙS΅‡υοM›lg‰eγEΣ¤ο®Ρ+Τ-Ϋ=AόiW
ΰiIΜΐ¬„&³¨*#kQmΝ`?[‘'αθS€κθ―:Λ=«ΛΧΉiΖGέίπΧ 2’ZύuK-§θlΪΎPj#]G}ÒΉ2ΡY}D)D'΅ά• AqÿΛ³ΗC„Fξ¦!uij
lρx}χξ6;./‰σGΕγzlSU¦όφμb@ΕΉΆ1vƒC/ªΚHq²_n•…ά‡ª–Κ45S3+ΉωPζ#ΆmΤ­(sHhsνUUςnDΥCζ²¨‘BΫfΕνmx¶²+Υm 3ΏχΤJ¦ι(Ί›‡\Q½pζ¥vΗ²Τ9A„cςΖιuΣΕC6[‡]ΠίbΑsυΰ_=Βδη±§’Ό[+ξ›ά·uήίsu…€Rιx”Z>εÿ§¨ 8φ„3…J΄ +½Λ’0άUn–¤lπ<ÿX¦‡Fo”cAΗ\wΓ§-9ιrΏξ¥h(Κα‚MA
Tπΰ^$2"ξ`!α—·”ΦhΌƒy2Θ•Έ¦«g2›βe6T JωΦΙ‡ήΙHΖWo’Ϋ¦Π,Ηu«5a΄Ώ ξφ'ώΜάΒ9=E;ά’ΡH†r;Jψ‘vlθhΆ£βhΧrιϋτ—δwQm	Cψk„χW!­ξ:)άI|YΚh”Φ1s>‡|‚Ά¨ΒUΥ6ΧriΐΠΎeCΰHβtο®ƒψνΙΑ#ΖFΎ/C¦+nQ,Φ¦Μ…Εw!Òu¥ΨW!µ­,Iθ|*|42Κ»[QρΔ!°°JΟΗ)Ύϋ–Ζ½ΙΝγδÿ1‰»άΕFΛϋ‰Z 	yρ3'rΕΥΘ¨Q4F¥­$*λSδYxwYΗF3΄Ό *Ψ©χ+=UΎΓΠmαz
#ΧφªGπΝiN”δBu@Ò\*α LΟSΰ­-Τv6¤«QAlQ‡CƒΕkΐψ1•Φ)ΙM,‰Κ½sy²‚ςσX„Ν<+—µε>«ϊ9–aΎ¨ΊPχqΝΗVΥwO­λSνRΠ…1ƒΈÒ;£Ήδ+gέAΑΠΏ½AwΚ-ρΠ§Cψ¬όΦδΫ~&MZέ‚µΦÿΏφ²JΥ;ª&Wo@Πzut"Έ«ΗkΈΒΈ'ΡΌζeρUΤ”²lΗµ}0Ξ}Λ]~π9 Ζ8"ί·βO7±΄ΊΈK·§χR{ ΑXΥYz^“ΉνrΠ ξΧΆ¨LwUOKΣ!`S·«»5Ί	U do>|φa»„εrΐ/ÒΨλ2  ―e”qφΩ®J{‹ΖAρnλ'ΞωφªΣΛVμ;Q€Υϊ±Ϊ^ώ“Έ¬»απρ&",F«γΝsP†EΌρ1{'π»ΣΩU‰|WΧΣy›Sµ]^ Έ•£Υª¬K§ζκ
ΑΞ sν5΅ .e*>³_χδ¶Ϋ/Xβ'[]¤έΎ+γCƒSΊ{Fι{λuΚ…τtqj5Xω$ά°ΐΠ1ιΟd»ΆMLCkS¤Òηά2ζ?9Ρ>α†’jύδa·Ξκ}#ώk"¨ϊªcΊ¨}ΟυΊΐ•]C ΟVRαέ™kzHΩ€Έ0fMω¨J<-υ΅νΆ$.6ΝχΤjτ4ΣηQόΫΣήT}µ_ΉDU$©ό](„‘©΄ γΗyr
Ϊ΅u¤b4ή$ΐªNIFk¤'ύΪ―/λγ27.-υxWΗΖΕ©8Td»Ε®¥τ ¬—θϋ%ρκs6mfΓΏAr±YΨ¥†kE±”ΊΉΗόCMυ'έ
yάλS®-Ρ|Χ"@A{`Λ³χt4dΞR‰Ή_ω€F!’Tr―άδμhΙήΘ.ξDƒAς	ΟKΓƒ)Α"y–]σ‡yϊΜ­²>Φ°΄Σν€'€(pϊF΅―°–zMάηεό`Ξ r“Υ~gΐ·jÿc"q:Yƒv²ΟUk­ε §¨¦.°—Ur–TΛ΄½ξΰbk 7j§gH›Φ&`―ΙτªΌ\ρΈ1Ζtτ`5=ο`²ηΘ°Κi_¦§jά±Tκ$UK8% fδΖγ&SGΐ&Ωuί
αΨe	&u–¶²Ό.5Α	Eβ{ΨώΜjρxB¤ρv†ά>‹Εg•ZM¨#KύΈΖyLY?ό@Ιh-uΙ²&TιΦΡΖ>‡ΞΘ
κ¦3Κ!φ <ΘIyiυ–ΊNSQlϊέΓiGƒ*>‰·?Ζύ#τbB4:f‘(5©"Ίe§z°ψ-β5π¤”Ά“9pΧως¶uuΗDJχo±²™]ϊωeΥWIkΕ8, ®L/τkυ“YΤφ=ƒΣύύv¨”
ƒ—άΌ!ν4:HCόαb»Έ©Ό›ο¶%CrbzΠYmΟ<7 O< cOΧ¶½“ΚRδKλ;oSΙψ+Ζ.²IΙΝ'?χQΏ¤}ΕTE62(=Ω|EΙEΐ*pQ… γΆ<›5^•DΫ*―Φκ.QΩ=4λ»ÒT*ϊ­¨:/ΛC#ΗjτRÒ;8± rΨFΠ›ϊV'λΔ]η*Δ aυF“™Ϋ΄Α$}’`1…βHΪθVdOκ'‹ΚCAMοφΗ–xΝί#·®UE4cEχk$Lq›Δκ›‚» θ½lζ®~¥AXA&²Yν·οπ΄^ΓΓM”pδ?ΊX&Μ+1λΞqΚ-Άν«» –πθΓF΄ύΰ?z6]Θlϊ<SΣh‘`'gQ€{yεΩα\όZ³X~Χ'φ‚5 CγΕ΄Ξ’»Ύαn8ς.΄rΩ-Ύ/›RΊIΟ|W!έ—AϋΓN,[<C$Ό4rkfhklΈOy“αν^ΙυχOfΈΠΤ// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <media/v4l2-common.h>

static unsigned int i2c_debug;
module_param(i2c_debug, int, 0644);
MODULE_PARM_DESC(i2c_debug, "enable debug messages [i2c]");

static unsigned int i2c_scan;
module_param(i2c_scan, int, 0444);
MODULE_PARM_DESC(i2c_scan, "scan i2c bus at insmod time");

#define dprintk(level, fmt, arg...)\
	do { if (i2c_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: i2c:" fmt), \
			__func__, ##arg); \
	} while (0)

#define I2C_WAIT_DELAY 32
#define I2C_WAIT_RETRY 64

#define I2C_EXTEND  (1 << 3)
#de