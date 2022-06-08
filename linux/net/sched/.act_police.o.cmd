:
		i2c_bus = &dev->i2c_bus[0];

		if (!dvb_attach(dib7000p_attach, &dib7000p_ops))
			return -ENODEV;

		fe0->dvb.frontend = dib7000p_ops.init(&i2c_bus->i2c_adap,
			0x12, &hauppauge_hvr1400_dib7000_config);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend *fe;
			struct xc2028_config cfg = {
				.i2c_adap  = &dev->i2c_bus[1].i2c_adap,
				.i2c_addr  = 0x64,
			};
			static struct xc2028_ctrl ctl = {
				.fname   = XC3028L_DEFAULT_FIRMWARE,
				.max_len = 64,
				.demod   = XC3028_FE_DIBCOM52,
				/* This is true for all demods with
					v36 firmware? */
				.type    = XC2028_D2633,
			};

			fe = dvb_attach(xc2028_attach,
					fe0->dvb.frontend, &cfg);
			if (fe != NULL && fe->ops.tuner_ops.set_config != NULL)
				fe->ops.tuner_ops.set_config(fe, &ctl);
		}
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP:
		i2c_bus = &dev->i2c_bus[port->nr - 1];

		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&dvico_s5h1409_config,
						&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			fe0->dvb.frontend = dvb_attach(s5h1411_attach,
							&dvico_s5h1411_config,
							&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL)
			dvb_attach(xc5000_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &dvico_xc5000_tunerconfig);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP: {
		i2c_bus = &dev->i2c_bus[port->nr - 1];

		fe0->dvb.frontend = dvb_attach(zl10353_attach,
					       &dvico_fusionhdtv_xc3028,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend      *fe;
			struct xc2028_config	  cfg = {
				.i2c_adap  = &i2c_bus->i2c_adap,
				.i2c_addr  = 0x61,
			};
			static struct xc2028_ctrl ctl = {
				.fname       = XC2028_DEFAULT_FIRMWARE,
				.max_len     = 64,
				.demod       = XC3028_FE_ZARLINK456,
			};

			fe = dvb_attach(xc2028_attach, fe0->dvb.frontend,
					&cfg);
			if (fe != NULL && fe->ops.tuner_ops.set_config != NULL)
				fe->ops.tuner_ops.set_config(fe, &ctl);
		}
		break;
	}
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2: {
		i2c_bus = &dev->i2c_bus[port->nr - 1];
		/* cxusb_ctrl_msg(adap->dev, CMD_DIGITAL, NULL, 0, NULL, 0); */
		/* cxusb_bluebird_gpio_pulse(adap->dev, 0x02, 1); */

		if (!dvb_attach(dib7000p_attach, &dib7000p_ops))
			return -ENODEV;

		if (dib7000p_ops.i2c_enumeration(&i2c_bus->i2c_adap, 1, 0x12, &dib7070p_dib7000p_config) < 0) {
			pr_warn("Unable to enumerate dib7000p\n");
			return -ENODEV;
		}
		fe0->dvb.frontend = dib7000p_ops.init(&i2c_bus->i2c_adap, 0x80, &dib7070p_dib7000p_config);
		if (fe0->dvb.frontend != NULL) {
			struct i2c_adapter *tun_i2c;

			fe0->dvb.frontend->sec_priv = kmemdup(&dib7000p_ops, sizeof(dib7000p_ops), GFP_KERNEL);
			if (!fe0->dvb.frontend->sec_priv)
				return -ENOMEM;
			tun_i2c = dib7000p_ops.get_i2c_master(fe0->dvb.frontend, DIBX000_I2C_INTERFACE_TUNER, 1);
			if (!dvb_attach(dib0070_attach, fe0->dvb.frontend, tun_i2c, &dib7070p_dib0070_config))
				return -ENODEV;
		}
		break;
	}
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
		i2c_bus = &dev->i2c_bus[0];

		fe0->dvb.frontend = dvb_attach(zl10353_attach,
			&dvico_fusionhdtv_xc3028,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend      *fe;
			struct xc2028_config	  cfg = {
				.i2c_adap  = &dev->i2c_bus[1].i2c_adap,
				.i2c_addr  = 0x61,
			};
			static struct xc2028_ctrl ctl = {
				.fname       = XC2028_DEFAULT_FIRMWARE,
				.max_len     = 64,
				.demod       = XC3028_FE_ZARLINK456,
			};

			fe = dvb_attach(xc2028_attach, fe0->dvb.frontend,
				&cfg);
			if (fe != NULL && fe->ops.tuner_ops.set_config != NULL)
				fe->ops.tuner_ops.set_config(fe, &ctl);
		}
		break;
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
		i2c_bus = &dev->i2c_bus[0];

		fe0->dvb.frontend = dvb_attach(zl10353_attach,
					       &dvico_fusionhdtv_xc3028,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend	*fe;
			struct xc4000_config	cfg = {
				.i2c_address	  = 0x61,
				.default_pm	  = 0,
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
  ΄&    v θόÿÿÿd΅    ƒ¨<  Έ   θόÿÿÿd΅    …ΐ„–  χD$   tϋ$ƒΕ1φ)\$„ώÿÿ‹D$»   )σ9ΓGΨD$ϊ‰κ1ÿ‹   Ακϊ€   Υ    T­ CΗα   €‰L$‹ ƒΰΰ<Π‹    Έ   βc  ‰T$ θόÿÿÿ‹D$$ƒ€<  ‹T$ ‹L$‰ψθόÿÿÿΖƒ|$…ÿÿÿƒϋsz…Ϋ„"ÿÿÿ¶‹<$φΓ„ÿÿÿ·Tώ‹<$f‰Tώιÿώÿÿ΄&    ‰Β‹L$‹$ιzώÿÿ΄&    v ‹<$‰ρ‹‰‰<$‹Tό~ƒηό‰Tό‹4$)ω)ΞΩΑισ¥ι³ώÿÿv ‹‹$‰y‹Tόƒηό‰Tό)ω)ΞΩΑισ¥ι‹ώÿÿv €=    …aύÿÿ‹w,Ζ   …φu‹7‰ψ‰$θόÿÿÿ‹$Rÿt$SVPh΄: θόÿÿÿƒΔι(ύÿÿfθόÿÿÿι`ώÿÿ¶    ‹_,Ζ›   …Ϋu‹‰ψθόÿÿÿUVSPh`: θόÿÿÿƒΔιηόÿÿ‹<$·Tώf‰Tώιξύÿÿ¶    ‹_,Ζ™   …Ϋu‹‰ψ‰L$$θόÿÿÿÿt$VSPh; θόÿÿÿƒΔ‰t$‹L$$ιΤόÿÿ΄&    v θόÿÿÿUW‰Η‰ΘV‰ΞSΑζ1Ϋƒμ‰   ‰ρω‰   Σ‹T$ƒ=    ‰4$‰=    ‰   ‰   Η       (   uΖ)   Η        …ΐtV1ΐΎ€   ΄&    f‰Α‰σ‰ΒƒΐƒαΑβ)Λ‹,   ‰\‹,   ΗÿÿÿÿΗDÿÿÿÿ‹,   ΗD    ;   rΊ΅       u   ΐ‹$1Ò‰Ψθόÿÿÿ‰   ƒΔ[^_]Γ΄&    ¶    U‰εW‰ΗV‰ΦSƒμd΅    ‰Eπ‹B,…ΐ„  θόÿÿÿ‰Γ@ΑΰƒΘ$‰Eΰ‹W@‰Uδθόÿÿÿ‹Uδ…    ‹GD‰Uδ…ΐ„Ρ   ƒΓ0MμUθƒγψCό‰Eάθόÿÿÿ‰Γ…ΐt‹U‹Eθ‰P0Ί`   ‰h<f‰P4Η@8    ‹Eΰ‰C‹V,…ÒtsC$θόÿÿÿ‹X  1ΐ1Ò…Ιt‹‹Q‰C‹E‰S‹U‰C‹E‰S‹Mμ‰C‹E‹Uά‰C ‰Ψj ÿuδÿuθj jWθόÿÿÿƒΔ‹Eπd+    uTeτ[^_]Γ΄&    f‹Έ?Μ  …ÒDΠι|ÿÿÿ΄&    ‹…ΐ…%ÿÿÿλΏt& ‹ΗEΰ$  »   …ΐ„ζώÿÿιΞώÿÿθόÿÿÿ΄&    W‰ΧVS‰Γƒμd΅    ‰D$‹C$©ΐ  …Ύ   ‹G,…ΐ„“   θόÿÿÿpH%ΑζƒΞ$‰Ϊ‰ΰθόÿÿÿ‰Γ…ΐtN‰p‹W,…ÒtYC$θόÿÿÿ‹X  1ΐ1Ò…Ιt‹‹Q‰C‹D$,‰S‹T$0‰C‹D$4‰S‰C‹D$8‰C ‰ΰθόÿÿÿ‹D$d+    uRƒΔ[^_Γ‹Έ?Μ  …ÒDΠλ™f‹Ύ$  Ή+   …ΐ„jÿÿÿιTÿÿÿ΄&    φΔ…9ÿÿÿ‰Ψθόÿÿÿ„ΐ„*ÿÿÿλ΅θόÿÿÿθόÿÿÿ‹    % πÿÿƒϊEΠ‰ts Γv θόÿÿÿ΅ΌΊ ΑΰΓfθόÿÿÿƒ=   