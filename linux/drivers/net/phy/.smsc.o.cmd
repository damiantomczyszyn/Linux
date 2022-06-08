dev->board == CX23885_BOARD_HAUPPAUGE_HVR1275)
			cx23885_set_frontend_hook(port, fe0->dvb.frontend);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1411_attach,
					       &hcw_s5h1411_config,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;

		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[1].i2c_adap,
			   &hauppauge_tda18271_config);

		tda18271_attach(&dev->ts1.analog_fe,
			0x60, &dev->i2c_bus[1].i2c_adap,
			&hauppauge_tda18271_config);

		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
		i2c_bus = &dev->i2c_bus[0];
		switch (alt_tuner) {
		case 1:
			fe0->dvb.frontend =
				dvb_attach(s5h1409_attach,
					   &hauppauge_ezqam_config,
					   &i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;

			dvb_attach(tda829x_attach, fe0->dvb.frontend,
				   &dev->i2c_bus[1].i2c_adap, 0x42,
				   &tda829x_no_probe);
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				   0x60, &dev->i2c_bus[1].i2c_adap,
				   &hauppauge_tda18271_config);
			break;
		case 0:
		default:
			fe0->dvb.frontend =
				dvb_attach(s5h1409_attach,
					   &hauppauge_generic_config,
					   &i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			dvb_attach(mt2131_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &hauppauge_generic_tunerconfig, 0);
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&hauppauge_hvr1800lp_config,
						&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(mt2131_attach, fe0->dvb.frontend,
			   &i2c_bus->i2c_adap,
			   &hauppauge_generic_tunerconfig, 0);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(lgdt330x_attach,
					       &fusionhdtv_5_express,
					       0x0e,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(simple_tuner_attach, fe0->dvb.frontend,
			   &i2c_bus->i2c_adap, 0x61,
			   TUNER_LG_TDVS_H06XF);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
		i2c_bus = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&hauppauge_hvr1500q_config,
						&dev->i2c_bus[0].i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(xc5000_attach, fe0->dvb.frontend,
			   &i2c_bus->i2c_adap,
			   &hauppauge_hvr1500q_tunerconfig);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
		i2c_bus = &dev->i2c_bus[1];
		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&hauppauge_hvr1500_config,
						&dev->i2c_bus[0].i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			struct dvb_frontend *fe;
			struct xc2028_config cfg = {
				.i2c_adap  = &i2c_bus->i2c_adap,
				.i2c_addr  = 0x61,
			};
			static struct xc2028_ctrl ctl = {
				.fname       = XC2028_DEFAULT_FIRMWARE,
				.max_len     = 64,
				.demod       = XC3028_FE_OREN538,
			};

			fe = dvb_attach(xc2028_attach,
					fe0->dvb.frontend, &cfg);
			if (fe != NULL && fe->ops.tuner_ops.set_config != NULL)
				fe->ops.tuner_ops.set_config(fe, &ctl);
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(tda10048_attach,
			&hauppauge_hvr1200_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(tda829x_attach, fe0->dvb.frontend,
			   &dev->i2c_bus[1].i2c_adap, 0x42,
			   &tda829x_no_probe);
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[1].i2c_adap,
			   &hauppauge_hvr1200_tuner_config);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(tda10048_attach,
			&hauppauge_hvr1210_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				0x60, &dev->i2c_bus[1].i2c_adap,
				&hauppauge_hvr1210_tuner_config);
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
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
	case CX23885_BOARD_HAUPPAUGE_STARBURST2:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		switch (port->nr) {

		/* port b - satellite */
		case 1:
			/* attach frontend */
			fe0->dvb.frontend = dvb_attach(m88ds3103_attach,
					&hauppauge_hvr5525_m88ds3103_config,
					&i2c_bus->i2c_adap, &adapter);
			if (fe0->dvb.frontend == NULL)
				break;

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

			/* attach tuner */
			memset(&m88rs6000t_config, 0, sizeof(m88rs6000t_config));
			m88rs6000t_config.fe = fe0->dvb.frontend;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "m88rs6000t", I2C_NAME_SIZE);
			info.addr = 0x21;
			info.platform_data = &m88rs6000t_config;
			request_module("%s", info.type);
			client_tuner = i2c_new_client_device(adapter, &info);
			if (!i2c_client_has_driver(client_tuner))
				goto frontend_detach;
			if (!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;

			/* delegate signal strength measurement to tuner */
			fe0->dvb.frontend->ops.read_signal_strength =
				fe0->dvb.frontend->ops.tuner_ops.get_rf_strength;
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
			request_module("%s", info.type);
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
			request_module("%s", info.type);
			client_tuner = i2c_new_client_device(&i2c_bus2->i2c_adap, &info);
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
				i2c_unregister_device(client_tuner);
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
				memcpy(&dev->ts1.analog_fe.ops.tuner_ops,
				       &fe0->dvb.frontend->ops.tuner_ops,
				       sizeof(struct dvb_tuner_ops));
			}
			break;

		/* port c - terrestrial/cable */
		case 2:
			/* attach frontend */
			i2c_bus = &dev->i2c_bus[0];
			fe0->dvb.frontend = dvb_attach(lgdt3306a_attach,
				&hauppauge_quadHD_ATSC_b_config, &i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;

			/* attach tuner */
			memset(&si2157_config, 0, sizeof(si2157_config));
			si2157_config.fe = fe0->dvb.frontend;
			si2157_config.if_port = 1;
			si2157_config.inversion = 1;
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
				i2c_unregister_device(client_tuner);
				module_put(client_demod->dev.driver->owner);
				i2c_unregister_device(client_demod);
				port->i2c_client_demod = NULL;
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;
			break;
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		switch (port->nr) {
		/* port c - Terrestrial/cable */
		case 2:
			/* attach frontend */
			i2c_bus = &dev->i2c_bus[0];
			fe0->dvb.frontend = dvb_attach(lgdt3306a_attach,
					&hauppauge_hvr1265k4_config,
					&i2c_bus->i2c_adap);
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
			if (!i2c_client_has_driver(client_tuner))
				goto frontend_detach;

			if (!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				client_tuner = NULL;
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
	default:
		pr_info("%s: The frontend of your DVB/ATSC card  isn't supported yet\n",
			dev->name);
		break;
	}

	if ((NULL == fe0->dvb.frontend) || (fe1 && NULL == fe1->dvb.frontend)) {
		pr_err("%s: frontend initialization failed\n",
		       dev->name);
		goto frontend_detach;
	}

	/* define general-purpose callback pointer */
	fe0->dvb.frontend->callback = cx23885_tuner_callback;
	if (fe1)
		fe1->dvb.frontend->callback = cx23885_tuner_callback;
#if 0
	/* Ensure all frontends negotiate bus access */
	fe0->dvb.frontend->ops.ts_bus_ctrl = cx23885_dvb_bus_ctrl;
	if (fe1)
		fe1->dvb.frontend->ops.ts_bus_ctrl = cx23885_dvb_bus_ctrl;
#endif

	/* Put the tuner in standby to keep it quiet */
	call_all(dev, tuner, standby);

	if (fe0->dvb.frontend->ops.analog_ops.standby)
		fe0->dvb.frontend->ops.analog_ops.standby(fe0->dvb.frontend);

	/* register everything */
	ret = vb2_dvb_register_bus(&port->frontends, THIS_MODULE, port,
				   &dev->pci->dev, NULL,
				   adapter_nr, mfe_shared);
	if (ret)
		goto frontend_detach;

	ret = dvb_register_ci_mac(port);
	if (ret)
		goto frontend_detach;

	return 0;

frontend_detach:
	/* remove I2C client for SEC */
	client_sec = port->i2c_client_sec;
	if (client_sec) {
		module_put(client_sec->dev.driver->owner);
		i2c_unregister_device(client_sec);
		port->i2c_client_sec = NULL;
	}

	/* remove I2C client for tuner */
	client_tuner = port->i2c_client_tuner;
	if (client_tuner) {
		module_put(client_tuner->dev.driver->owner);
		i2c_unregister_device(client_tuner);
		port->i2c_client_tuner = NULL;
	}

	/* remove I2C client for demodulator */
	client_demod = port->i2c_client_demod;
	if (client_demod) {
		module_put(client_demod->dev.driver->owner);
		i2c_unregister_device(client_demod);
		port->i2c_client_demod = NULL;
	}

	port->gate_ctrl = NULL;
	vb2_dvb_dealloc_frontends(&port->frontends);
	return -EINVAL;
}

int cx23885_dvb_register(struct cx23885_tsport *port)
{

	struct vb2_dvb_frontend *fe0;
	struct cx23885_dev *dev = port->dev;
	int err, i;

	/* Here we need to allocate the correct number of frontends,
	 * as reflected in the cards struct. The reality is that currently
	 * no cx23885 boards support this - yet. But, if we don't modify this
	 * code then the second frontend would never be allocated (later)
	 * and fail with error before the attach in dvb_register().
	 * Without these changes we risk an OOPS later. The changes here
	 * are for safety, and should provide a good foundation for the
	 * future addition of any multi-frontend cx23885 based boards.
	 */
	pr_info("%s() allocating %d frontend(s)\n", __func__,
		port->num_frontends);

	for (i = 1; i <= port->num_frontends; i++) {
		struct vb2_queue *q;

		if (vb2_dvb_alloc_frontend(
			&port->frontends, i) == NULL) {
			pr_err("%s() failed to alloc\n", __func__);
			return -ENOMEM;
		}

		fe0 = vb2_dvb_get_frontend(&port->frontends, i);
		if (!fe0)
			return -EINVAL;

		dprintk(1, "%s\n", __func__);
		dprintk(1, " ->probed by Card=%d Name=%s, PCI %02x:%02x\n",
			dev->board,
			dev->name,
			dev->pci_bus,
			dev->pci_slot);

		/* dvb stuff */
		/* We have to init the queue for each frontend on a port. */
		pr_info("%s: cx23885 based dvb card\n", dev->name);
		q = &fe0->dvb.dvbq;
		q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
		q->gfp_flags = GFP_DMA32;
		q->min_buffers_needed = 2;
		q->drv_priv = port;
		q->buf_struct_size = sizeof(struct cx23885_buffer);
		q->ops = &dvb_qops;
		q->mem_ops = &vb2_dma_sg_memops;
		q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
		q->lock = &dev->lock;
		q->dev = &dev->pci->dev;

		err = vb2_queue_init(q);
		if (err < 0)
			return err;
	}
	err = dvb_register(port);
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
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	/* remove I2C client for tuner */
	client = port->i2c_client_tuner;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	/* remove I2C client for demodulator */
	client = port->i2c_client_demod;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	switch (port->dev->board) {
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		netup_ci_exit(port);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		altera_ci_release(port->dev, port->nr);
		break;
	}

	port->gate_ctrl = NULL;

	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Silicon Labs C8051F300 microcontroller.
 *
 * It is used for LNB power control in TeVii S470,
 * TBS 6920 PCIe DVB-S2 cards.
 *
 * Microcontroller connected to cx23885 GPIO pins:
 * GPIO0 - data		- P0.3 F300
 * GPIO1 - reset	- P0.2 F300
 * GPIO2 - clk		- P0.1 F300
 * GPIO3 - busy		- P0.0 F300
 *
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@me.by>
 */

#include "cx23885.h"
#include "cx23885-f300.h"

#define F300_DATA	GPIO_0
#define F300_RESET	GPIO_1
#define F300_CLK	GPIO_2
#define F300_BUSY	GPIO_3

static void f300_set_line(struct cx23885_dev *dev, u32 line, u8 lvl)
{
	cx23885_gpio_enable(dev, line, 1);
	if (lvl == 1)
		cx23885_gpio_set(dev, line);
	else
		cx23885_gpio_clear(dev, line);
}

static u8 f300_get_line(struct cx23885_dev *dev, u32 line)
{
	cx23885_gpio_enable(dev, line, 0);

	return cx23885_gpio_get(dev, line);
}

static void f300_send_byte(struct cx23885_dev *dev, u8 dta)
{
	u8 i;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		f300_set_line(dev, F300_DATA, (dta & 0x80) >> 7);/* msb first */
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
	}
}

static u8 f300_get_byte(struct cx23885_dev *dev)
{
	u8 i, dta = 0;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
		dta |= f300_get_line(dev, F300_DATA);/* msb first */

	}

	return dta;
}

static u8 f300_xfer(struct dvb_frontend *fe, u8 *buf)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;
	u8 i, temp, ret = 0;

	temp = buf[0];
	for (i = 0; i < buf[0]; i++)
		temp += buf[i + 1];
	temp = (~temp + 1);/* get check sum */
	buf[1 + buf[0]] = temp;

	f300_set_line(dev, F300_RESET, 1);
	f300_set_line(dev, F300_CLK, 1);
	udelay(30);
	f300_set_line(dev, F300_DATA, 1);
	msleep(1);

	/* question: */
	f300_set_line(dev, F300_RESET, 0);/* begin to send data */
	msleep(1);

	f300_send_byte(dev, 0xe0);/* the slave address is 0xe0, write */
	msleep(1);

	temp = buf[0];
	temp += 2;
	for (i = 0; i < temp; i++)
		f300_send_byte(dev, buf[i]);

	f300_set_line(dev, F300_RESET, 1);/* sent data over */
	f300_set_line(dev, F300_DATA, 1);

	/* answer: */
	temp = 0;
	for (i = 0; ((i < 8) & (temp == 0)); i++) {
		msleep(1);
		if (f300_get_line(dev, F300_BUSY) == 0)
			temp = 1;
	}

	if (i > 7) {
		pr_err("%s: timeout, the slave no response\n",
								__func__);
		ret = 1; /* timeout, the slave no response */
	} else { /* the slave not busy, prepare for getting data */
		f300_set_line(dev, F300_RESET, 0);/*ready...*/
		msleep(1);
		f300_send_byte(dev, 0xe1);/* 0xe1 is Read */
		msleep(1);
		temp = f300_get_byte(dev);/*get the data length */
		if (temp > 14)
			temp = 14;

		for (i = 0; i < (temp + 1); i++)
			f300_get_byte(dev);/* get data to empty buffer */

		f300_set_line(dev, F300_RESET, 1);/* received data over */
		f300_set_line(dev, F300_DATA, 1);
	}

	return ret;
}

int f300_set_voltage(struct dvb_frontend *fe, enum fe_sec_voltage voltage)
{
	u8 buf[16];

	buf[0] = 0x05;
	buf[1] = 0x38;/* write port */
	buf[2] = 0x01;/* A port, lnb power */

	switch (voltage) {
	case SEC_VOLTAGE_13:
		buf[3] = 0x01;/* power on */
		buf[4] = 0x02;/* B port, H/V */
		buf[5] = 0x00;/*13V v*/
		break;
	case SEC_VOLTAGE_18:
		buf[3] = 0x01;
		buf[4] = 0x02;
		buf[5] = 0x01;/* 18V h*/
		break;
	case SEC_VOLTAGE_OFF:
		buf[3] = 0x00;/* power off */
		buf[4] = 0x00;
		buf[5] = 0x00;
		break;
	}

	return f300_xfer(fe, buf);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Silicon Labs C8051F300 microcontroller.
 *
 * It is used for LNB power control in TeVii S470,
 * TBS 6920 PCIe DVB-S2 cards.
 *
 * Microcontroller connected to cx23885 GPIO pins:
 * GPIO0 - data		- P0.3 F300
 * GPIO1 - reset	- P0.2 F300
 * GPIO2 - clk		- P0.1 F300
 * GPIO3 - busy		- P0.0 F300
 *
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@me.by>
 */

#include "cx23885.h"
#include "cx23885-f300.h"

#define F300_DATA	GPIO_0
#define F300_RESET	GPIO_1
#define F300_CLK	GPIO_2
#define F300_BUSY	GPIO_3

static void f300_set_line(struct cx23885_dev *dev, u32 line, u8 lvl)
{
	cx23885_gpio_enable(dev, line, 1);
	if (lvl == 1)
		cx23885_gpio_set(dev, line);
	else
		cx23885_gpio_clear(dev, line);
}

static u8 f300_get_line(struct cx23885_dev *dev, u32 line)
{
	cx23885_gpio_enable(dev, line, 0);

	return cx23885_gpio_get(dev, line);
}

static void f300_send_byte(struct cx23885_dev *dev, u8 dta)
{
	u8 i;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		f300_set_line(dev, F300_DATA, (dta & 0x80) >> 7);/* msb first */
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
	}
}

static u8 f300_get_byte(struct cx23885_dev *dev)
{
	u8 i, dta = 0;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
		dta |= f300_get_line(dev, F300_DATA);/* msb first */

	}

	return dta;
}

static u8 f300_xfer(struct dvb_frontend *fe, u8 *buf)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;
	u8 i, temp, ret = 0;

	temp = buf[0];
	for (i = 0; i < buf[0]; i++)
		temp += buf[i + 1];
	temp = (~temp + 1);/* get check sum */
	buf[1 + buf[0]] = temp;

	f300_set_line(dev, F300_RESET, 1);
	f300_set_line(dev, F300_CLK, 1);
	udelay(30);
	f300_set_line(dev, F300_DATA, 1);
	msleep(1);

	/* question: */
	f300_set_line(dev, F300_RESET, 0);/* begin to send data */
	msleep(1);

	f300_send_byte(dev, 0xe0);/* the slave address is 0xe0, write */
	msleep(1);

	temp = buf[0];
	temp += 2;
	for (i = 0; i < temp; i++)
		f300_send_byte(dev, buf[i]);

	f300_set_line(dev, F300_RESET, 1);/* sent data over */
	f300_set_line(dev, F300_DATA, 1);

	/* answer: */
	temp = 0;
	for (i = 0; ((i < 8) & (temp == 0)); i++) {
		msleep(1);
		if (f300_get_line(dev, F300_BUSY) == 0)
			temp = 1;
	}

	if (i > 7) {
		pr_err("%s: timeout, the slave no response\n",
								__func__);
		ret = 1; /* timeout, the slave no response */
	} else { /* the slave not busy, prepare for getting data */
		f300_set_line(dev, F300_RESET, 0);/*ready...*/
		msleep(1);
		f300_send_byte(dev, 0xe1);/* 0xe1 is Read */
		msleep(1);
		temp = f300_get_byte(dev);/*get the data length */
		if (temp > 14)
			temp = 14;

		for (i = 0; i < (temp + 1); i++)
			f300_get_byte(dev);/* get data to empty buffer */

		f300_set_line(dev, F300_RESET, 1);/* received data over */
		f300_set_line(dev, F300_DATA, 1);
	}

	return ret;
}

int f300_set_voltage(struct dvb_frontend *fe, enum fe_sec_voltage voltage)
{
	u8 buf[16];

	buf[0] = 0x05;
	buf[1] = 0x38;/* write port */
	buf[2] = 0x01;/* A port, lnb power */

	switch (voltage) {
	case SEC_VOLTAGE_13:
		buf[3] = 0x01;/* power on */
		buf[4] = 0x02;/* B port, H/V */
		buf[5] = 0x00;/*13V v*/
		break;
	case SEC_VOLTAGE_18:
		buf[3] = 0x01;
		buf[4] = 0x02;
		buf[5] = 0x01;/* 18V h*/
		break;
	case SEC_VOLTAGE_OFF:
		buf[3] = 0x00;/* power off */
		buf[4] = 0x00;
		buf[5] = 0x00;
		break;
	}

	return f300_xfer(fe, buf);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  /* SPDX-License-Identifier: GPL-2.0 */
extern int f300_set_voltage(struct dvb_frontend *fe,
			    enum fe_sec_voltage voltage);
                                                                                                                                                                                                                                                                                                                                                                                                    