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
		if (!i2c_client_has_driver(cla@<ˆnooÔZâ‹?¶‰jÉB®
}3aaN÷S¾å5¼@¤e0ÜN´¥*Ì/‘¼"vQŸÞ>‹)*=‡xÀ}´Æ¦*À#úzyÔä¾òÒŽuÊ\…tà]/e”BæƒÑÊ‡³ãÞPßïØÔúJ^IèÒ>ž±ã\]±–ÊEíæ•¯_9züÕTEÎzsôŒ7-Ø&$Í˜¦zÕ9Å°úäØ¹(¨a{¼‘deù0q«Ížè|Ð©âœ#Záü®{$`¤Âv±¬[o<©xfÉ>¿·Ò ÝÛRÁm´ÆÏËúF	t¦(Ÿ)Y•6‹F¼XüaXDÿ^û—lˆèm¶pË$ŸƒÓy×ˆ~%øf,Bî½&¾¢2ÊhÔpÑ§]¡,MjVjE])¨·ôúÈÓè$*4l2©Zµý5ÂqŽÉäï–b§GÈ\7Ó1DO#×M6<Ü·ûmBoacùŸì(ä"«-fBoe|Zò'-"À’Á$7Á?ðAR¾T;=£Fä‡8NÏv’›rP‚HMA›dáØ×ÆÒ«|³™²×ÚÛ+p¨Í¯z$#¡‰az°½éºÆ~˜/ú}Ìm,+µ€Y^]¶s3$âB©§
íüE&½
]–(ãÀÉ¬ÚáYoeÜBŽÈæ9wÓAŽ)z!±p‰‚ŒŽK;¨o(ÕÇ€Yï÷±}õ{ØÂ8% z§¤y¿²¢Èw&êÍHÙôp‰€˜è÷¤°ñAGyOÁMeŠøGXÿÚà©€+¼LçN öøàáàRxÅÑ)},)f¨
1þÑ§tr§\rŒA³ƒæ°§ê)òCGxÎºvÞÄ¿_¢Ô|h*Œï`,©»to4Ä˜sÊ9o>ª5Ñ•„`+Úƒ	SGs.aÁ›ßôb"h·g‹`›Ò‰¢ÍJçd;Á
ò^A]­S)I-_1/@/üV@(á Ÿ’nÒô…i½ïLÄR¢¶+ñæÚ‚²h\"2ÂX{Þøê»d+C‹›ò#ðTù²#@®3›Ýó§ÑŸù,“oCg_K	ÅÎ`?|—Îçk˜ÙÞ¦ÃvJvZÒ0:D²H(m8kdÙÇðÑéçñü28Úì§±ÕL¬dLå2#êà¶åEmys4UA<ËAz[Gx¨Ñév·eñ1ÃÈ9Æ@øÎÕš ø!Õ¢[$\ÅÇwýf»‹ÉÞËPpz¸aò4¤.còJ¡¯ û{I¯yÄ²ïË:½SžöCácýÂÚ·pØ±²)Åð€d~Ê/ÆYì¡(AÚ G´ùw‰¯æÞž¯ñ£KÜe0™ ßŠàþ=zz¬³B\‹«¸nùvj­Áýu¼ë{\æsÅ‰ Ž¤)‚´­˜n&\pÁ­í5)üÂÉœrþ¦ýRgZÇe„”Id¯Ë÷|mæB?ƒÅhñßañÉX =¯._–a÷†žUôqÐ[ÀÞÂ–/š•þD)£VL·gøc…£ÍL};R+ý·zÇ#L¬žXEÆˆº«ŽmËùf“(dÍGç¯‚³"	%¥îª9âîSÑf"é¸‹sÍ„kÚdôä©ÁR¢‚ø?vk[û¿ÎØé…¡^êçÛ=
„Èç°—ñþ'ZÊöÍ’”;úý )<$'Ôö@&Ã¨Y¢‘³MþÃì©‹1”xïgò)Ÿ¤¬ªNL”M^²ûKZ6æ>Kµü!$ÓñÒ[LÓ9#mpÙ¢N
lˆÄ ökyÕ
å7Eqû HSØ	YdöÁë¶tëãƒS‰ÆÞÿOÓAÔéùž½$õk%ÑdêQH"jÓo2ø'NOm"AÕ9|ØAAúh­pøA»óÃT9Š­»Ù¹OuÕ²¶RÑ+àÌu>ºèJ…á”Y‰½O®C–F–hizyŽ1k@„]LéáÞuÜðüf¢Zpê7øÂzlöEq7xÓ=%•‘T“ÍRqäDK2åÚµÝºN>þ{	Ú€î¡ç‰"|„ñÄ`á/,¹e}./Ír+›¶é¥\2÷©/ŽÖzf«äj+<ñß›¡¨Ös™Ãž'Rì¨Ø:ƒØ‰’“|Èœ¦,tàhöŒœƒ^Ï#ñÇ	;‡ÿÆªõôaõqÝ51æ;­_9 ÿë…ÃÝ?Nï¥tŠWø§QèÖ.Øë<í‘zw#]ý
ÿƒ5‹1Ô’)y¼1ëàýIæÃ¨:*ýo@§V@â§Ø¢UJ«I/ôcÉ½¾.Æ?Å­i•e(@ü™Yðd h“òÀª=÷†OŒ6À_Rcê6Úá§¾µ» ,UGÃã„þáL@@ª°Ã»d>ÑœÕ²MFÅ£ènüN?Û¿û%#M›Ñ­ž™îVæÝŒN-GÖC@3vR°º@ê"}ÀóUü6Ô',‹ÝÊnfœ¾§É×P¿@w0n´8Ëac;ÃX©çšŸµ®ãuYæ,×2Ô´^uà½l"š[¤áxB‹¤)%¿ˆ× áƒ“?T”Œ²yÔYöÙÜ±ÃCpÊ"„röA¶×ÏJ`€9¿§œõûÁà<Y1¹tj;†—^®]ÜœŸìH™§UD¯ñ¤y9†ÛI¡ãí6»¶Uµ(Å¡ç·t¦¤ú‡Ûl}’×Bü$s‰|b††ûÝ´JÞ–<f{Ê¶¦Š]ãÑ(wàæîeÈ°Gƒêøÿà ÊÙW´bs(¹!%#Ã÷i&8îù—Ôþñ_ÈÈ}ÒT]³AG,ÚZ„ÿFmÒ_,íÅí*É<â°7‡ˆ”K,§áXâÖ+s«¬8q5œ?Z.TUk6ž½E?z=ËpÜÝ<½þ)Å³Š2%E†ç”­ö4CµÜíÚÏ í—†T¬þ»¯dCùæÄà½ÅWDìVc2ÅLX£˜+á3˜TÁÕMúlìY‚¯”ã]™ O“¬ÄdÁ”ë&mòC€6¦ôŒ©©jìA3ç+>;öåç
Ï“PJØîR’€Ï¾²qm¿SM~Î¢EšÓÓc&–dGözï¤"[QÊ45Øòá»Ü¢ª*\ï#Ó1tÞª,¤ÌOÔ&Ë2^y¢÷uø„FTØÖúÏ)‰Úpæ0Eu	ÓóÄòGe^HWðß×¥ k—yvQA-Î°[ýÀª*¦=ˆÊÒ‘-2ž=˜¿ÂUòJ0²ëÎÒóQãÓV•WÿÁËNá·­Ó‚ª9:¥Á#ï3*~™"1A À¶ŸPXÉgú;%°Òö?ØwƒÞ«°n]!Á;¸dÎÊÌ°µ0_ú®áeö Ë–§¿Šþ9˜Ÿ3O$ƒ[¾ÔÓã
š¦Ìlx;(šI[;ÌÉQü~¶v¨—rÇ:Íem»[px,([R£ yÿÆöÚ§BýcšÒ¤­ë¥É[p³æqcbÅ$+4¹+‘Ý‹]GN×zÆp„Í!¾c(4ùóû»‹¾M=¹»ÈdËð0à2„½¿ûNGÄ_ØÐ,„~Ÿ«»"<‚´n—ç¹KRŠÕx¨ÂŒT<Å¯ýTb>®K2ØÕ¢D ,6¢„¢ÆŠ'Þ=p‘vÿNt´PBu¢Zíë:ìÑ³±J»ª?SPý5¯q—UÁÔ	lO{5ú×£ Œ8z'8_Mc7¢&Éã2óQŒãE(Õ¡î«ið”Úõ$×íE)ºù¹Åž{t²ÂYPw|¦;!¤çd’#‘ÿO[Þ­÷	ùF+^ÀÒqÎoûGs³ý¯ùJÃË «å(8;b«á¼¦:Ï‹js^gZfË„À„ÊF²~¿iWûäúù@5›T.B€Þüqæc`éFBçÉ·&dñ÷´{	§Hi…{/C)¼‹3Å‡Ö^ºå£”x¶w§Á•P-¡Lû³à¹·tQOï›‰y8TÌXöà¦`1ö&†öŸhïÁéV»p{Vžš¸Ô[=£(9 Xºû[RM ´á€Z{Ç…GWb'n:¿¢în[MÐn{	Ê~Ë0k_-è½õGbS)ËöÿW´ëøTÖ¤µåô;œ:®ß¦C‡Ì´‘#‹‡V¿üÓÁæZ%tr&9 IÎr¯Úƒ‰5þX™ÒƒKôÓîP”Ôã±6¬@3á>8«Žï, áð~ôžnPðXÃHAPªÐ¤éÃô‡a5Š§hE¯zÚÂá™ýDpXœ(Aá­:0Gc¹Ð±oÈDGÞÙ	š¿Õ<4€“˜
kLå›¼Þ9qÌôûžøù†¼Ô( Îeœ9á9	Ôn¼…±“@NPô—ºÉ¯ÕŠ­*ò¾µíbï}Æ¦˜’HÌ­«P\‹Žô‚ÁG`O“7&b;¤C–l(í28ý~Œ‰5‹¬e4à£ÊÒ{€ùæ¹?„ôD7iÏ6è;„ÌYyõÅif[DÆnK?”4Äæüä,†ú±`hnÅ¢¸tÉmÁÈKõ¶ad‚e»K;‘JF2/½õ¹;Ësé¼òáfbÍøSpÒ€hvB|Übjƒo;vmM€œd¡lHë|$±Õ$É …CGÅ}Ro‰^³~<Á,,=ÜŒ$¶TÜþôÓa}¤çÂ´05!'Yì¢hýáôÙ˜üÐªÿ:›Gç:[ö¢®d>²Õ¤SÒ=zöÛžrRi‘ß'Ö-•eO¢vŒŽ‹¼&”üXß+†¿<øŸ¸FÛYB™@AN$ôÍ™•š¸<=T˜ª£Çýwí[çL`_ƒÀ­úÜ«¹¢V‰ÓA ¸†2ÄÆiÅbc™ø¦€ðŒ‘{ÒÎí³(²–ùÊg‹OE3ÜÓÙ`ÎÛP3ôH|Å˜‡R)6ÛRˆ˜•ŒzŒ± :qÊÓ;ûá_e­¼Ö
 ^zbj›[@Ðlœ¶$¥V-êÊÓïþÛ(žvYãDÙ2$~ê!NhøêQ X­lˆE:y&o”óKßí?2s§öï1Å‚d
ŽP¦‰*ì@/ÃÕÐ%‘ùÌ!~VY_çy÷l-§Qª±úA&2–µ—#,Â–Ç°æ€Ÿr²ß–šÙÚ1gzOM”ÄÌUÈ^sDJ+a}¡ÈëšŸÅÌ«ÐhƒT•§rx¬Oüž[¥ñIÌqAÕŽ>'?Ÿ¶¯ìŠ˜ü
»ÿ-Ôî£á5«ÆÏìmu(:&8ÚÚšº±?åºA¨Þ&÷†£dABÁù\ùÓ»Òó[£FùÉyämÇ&üqë6¦‘zJ)À@)žoúó'±þÎÖ¬Zgþ!rÂÖ™*šSBóÒ¨åpO­g}ûw¨FèC” &XÃª6¯ê÷À¤_ÞyàÅ×È	H¶¼›îQ¶lE[¬ó‚ÈMÇZ#½`yÿS)6v;) æÁã%õâDÕÁ6er->owner);
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
				i2c_unregister_device(cl$Fžå‘íAJë4ò¦eÙÈïÑÚ†ºX^è(”LzKIt-yÌqãÀÙöŸM–9ÿÿÆÿ„ùšóßG3þ‡ó½þTû^j˜ÕŽ¾MÝtgüx•oÛCJžˆðöÝCÏZi7›'ì+æß<7âQN?¼œ‡m•~4yóeÿH§¤ÚûèD¦p¿„9Iã®y?X ¾kü=NT§èåBù(C7N@G,æX»Œ—¹r¬êßH«‚Ã³ÐÒæÂ€!bŸo®“ƒWá£i©Vv·—ëŸÚßfäòÚ9à†5<	bÕç³ìˆCEƒãˆd6m=`-l3Ž$±DÂä¦Ôþ­òd©Õ“ãFG‘m`ª¯,szkuÄçåîýžN{z«w…ÝŠªc/ÇeY”LMë‘Yç×§mG‡uõ­ë™›­ŒHBpK¬Ù	°þæ¦Îô[¿J_;õ`¹ŽØ&$¬ò È	é7§jl’’žvA²£?/°N
„¶Ð,©Ì‘:o©V›4]¤<Co¨Û4¡¹ÑÑµB=ÇáyáÞ	‹zÔ‘[B½yÆìÿ‡»Ï¦†â³œt—1)ƒ´Ï{ž*ÉÑ)ûŠ†õõgôÉdž*iM	•î´/õ?­0ÿi-!F^6¢ˆ8½ô™’áŠó•¨ ömÂÓÊ¿¡Ö¯ï+ˆ#¡½5Ç”°eìŠ¿0,²¦ž­åoú­xEñS3Ë®¢P"}ô7á·;ôV¦?ù"ÅD^ ¡£úÒRæOEºemŸ ê¹2	+¿Øšáµà<üóƒ ]Ý‚ƒ4@K^5³¯€¾ÍRýþŠóž¿7hT†â>7ªÔw•´.£51‹ýêF¤ƒ¸b’çÇü˜“MÁM`ºãýÑÿ€0y_˜#Œ{Ñ
ZŠZ¬lï›ô:Ú^ç¥òp”4ë·š[N'Í¯ñw QpG·¬—2¢l>Ú¤4 ¬o‡\ŸsÁOˆ ™Z|
XQxL·‘(]è%†ˆ‘Ú+I÷{/ZÂ…vqC- »¹·Ñ¶MmÄÊÝñ_^ û~Þ“cº2,)qã^˜ºåé‰Å	¦@@L°•¡\Õå9¬u,§1v$ô_ÄöQ¯KèUá·mçÀŽÖ-1‹öcIlšdj7¯T0«_ 6Á!A³FË.æš ñoC«Ñ#N Ÿ.êþšù1# 	÷ˆw0>$„œîK¼goš€þî•Î¼Â¯üi-$÷~×®wi&ÂŽºsQ˜
ý–¯Ž¿ü/!ÂC“ÚF¨ªØ*q8ìÑÛ¬îøî3É>€{Sq]Ö’DLKÓøuüÍ=ôÏwÉ×S2-s#T²XÂ
­pjC€”[Úp…wìÉà°í5¢òCµa~÷pt¦‘VÜ¢h¯Ñ¬»L	ùô1ža…1qµSûŠ>(<m?üâûæà?ðC“Ñ]7?àá_pÝ›8§ ß»rfìs­ “ò,1^döXéÄuŠ=´éÂ%k¨ŽÝmQAÎ‹G…\ˆË’òrùÜÉ$êµ8"×-µE,Q\xq9@31	=m|_ºýN¥ø RÃä°{I@ob¸ÞØiÑ?‘.Œ…°4á´ßø{Vtæƒ‡?DäàÓ8|•9ÖFßEjlùcõ*å–ž…8Ü¹bW.o¤Æsž ²
.oí¥nqr‚n]¿'4¹Ø’ø›Â>ÝXÇYB,4ÙgÐ+|~,Ã%9‡¶Ûàµ1+ðŸÅÄaHòÐðÍQl×ï!;áË˜è"*pˆŽ)é…±]kK0­šO|÷Wðô¢.]ªy¤èŠÆ\DTºGžhÁ9®Ó²q?•{aÉj€9v¾lÝ AÈ,1è…‘.Š:K\
.ÄxÃ§á¡õ|GŸÖ<¿i÷´F‹õ"á¢š}ß
£gÊ4€O
°§S‹gÿIÄmÈ$V,ª¸îètp/f^ÞÇ1'jÃó¤üC#øÓçˆ1J¦Õ›óårìªí×!'µ
 ")uÿz£Zs¢¹ñØ¶õ[oË‘ê~¿Ê¤‰¼Av(ÍÛ5å!Áˆ£j½–Z2a	!L5 R.%³›ÙØ$,ï½îß°cÌ´U.†0	…0Î­Õð±ö˜Þçjx¥|+1lS¬¤r_:þµø%•_L’²“ì4qT¤>u\½u{&-)8|'­ƒO£.óBó¼9'Ã¨ÐdÑæ©3Êóß¢	³	ÉÇ€\â´•¤2Å¼·ôKœ^¢ðÝÄLVF²¢|¼I²tj%'=ærtO¾êuäpkrqèýYœg^“µ>.€‡Ê2G7@£Wß¿·ÀªRù:æÐW$Œüë”C[‘Y m³òØ)¡Ë›šöTŒ~Øó©Â¨ëÏ“lÚ ñæº“uŸ-õ¼OHT¹Žb:‡£T§L¡ÿ  c¼Ô) R8ûv{¡RÎ¶xkHºß‹þÞ#ºí2ÍËp:t%¨\+1ýùB¹¨^˜Ú] gRtuƒ_yþ…ïÅïy²v>8GVHC³Þ{œø™}@“Gõ ”×°¤(5½_µ^êIû2³ãNü<ØÍ«¾iPk0žJæØ“Á-Š¿ë½ZXò‹²d»e)‰†ì[ù!Q¿¬¥n»ÇýüÿÉn¦M<Ï<¡=Ô­ºJuåÀÞ
Ôƒ¯
*N$'›<¿1¼°OŒ±9h<'!³yåWl¥NDÁüÏÅ
ãÇ)NŠ˜ã+?¦ëçõp~ØMBè½ÈPZ›üc€¬Ì XFôîÉ®7~-öáà¡¼bŸq
¹‹Ä×‹ÎÏ>fíDç©Ö)MHÒ¬ÞiB\¢ì,¯z,Ý9Û"PhÔMôT…²j§‚Ž£Åõ_3Âá(K$#òÓ:Cñƒö:±*‘KmŠ‚ ÕDÍÓL¤‚Ÿdzk·ž|FÒ¹ƒó°mZ);­3€^ku|`k*˜FõÓÐ!ËP“¼Ž'¾Å1•¿ïUSMb‡ „GLÅPqÙ ×"I@+kFº
NºËp«á†ì;Ø†¥‡4¤¸fª«KoáF&iÈÊô¸ž°ìAU|myµ¡;%Ýc´IcËÄ!—˜šYˆh(Áˆ¶.Ÿ@rÖ2Ì»°qR™É˜Å` 'qågx¥fŸcÿÜÃ,®zØÊ·Ø.Å0Qê6a‡ÆF[øg+KYž`~.1H '®Ð€øuÓ1%ôžt´üÔB˜J¡4TJÔ¯ÈbaÚTZ¿!pT¦ñ9~fl'Sƒ=:õ•sÉóœIwßÔ½ó2³ËZÀ×¨U†?kN^C5Ûeü“t´&²…^kßm­Þ¦£X fJ&÷ã|äNàù¾EY3×ÿÜ¯üÛW‡Cá"ŠûM@Ë¤f8ezO-c@#ÚH=’˜ÜÈdá£ò„Ú=Lã/–ëþB´óo•M“•OŒp,ìÔÉa¢LõÓ‰áì(w¬ä™âT½ó9dMª¹dÛÎÞ´ÛËà-Q¿??Í›2Õ›$ï{?¢ûdÁZÃ—²E•€î~HçF}[­a´Jt>²Ã%
Á=¦ªÎôÀ"éØTòWƒK?\®l;MðÓJWÿœáãée+†t›”P‰gÏàèHÀ’¬“¹† ™¸Ï ·ž€¯ÝÐ†ñ³Äi^Û²±xJÕÑéŠªidq¼Ò_®VÄò?óo«äµè/ÙðTtŠm-­jWKêË˜ªDáš£bÆKKæ
Ç¼Ø&…˜?h@°C2PŽB¬`~,g?æ?ÅÓ½žWþ,ªJrœ¹«zøÌ¡wßIdÁŽg¶y3—åªÝ×Uø|²+ùæ	Û¥ãZ†!FfF»Z÷…/GœÑ’Áüåh¡ÔýÛ´âÛSrÉUþÄš?Cÿ¹€mçZCUÿ-Ô““Z3^È„uê	Gôù\
õAÇp¾I˜‹Ûí³pråw‚…žg©f9GIá¤Ì4j?¡ú/ûÆ£7hL5ÚÈs[€'rVzûX<eéHÚ×:FÆñZÿêŸx-ô’žãš†wÍÁQ+¥Ô.ƒ?ÔÛAûvvM»Ó,ä†‡1LfDÌæþ…Ç½ìøëg.±1M
ró1×e|˜Âê‡
f°µz)&ó@ÝÊ6òÈò‡
¯æÅ ­3>°f@Œ^ /­^>ëK¢‡¹ýÎ¦]t;QCÛÄ?_d1ß *LýËp—Í”—U¿-ÎgµìÈ(U½DL
8ØÄÇQn>)ÚH=§Ô½]üªš?òHh‹Üu àÛ‘YókFø üÓT¸„ŠlKRt²ËŽÇŽefÚ¥~³êÍªk™ÏÐ5º.â#ß¾š½â#	6©±/·ŽA¬Î¾QO{/L$ÏY<z®IIPnhò_è¬¹'È¼ÓöŒ¡~/Zu¤\ÐnÞúã’œ€ÕtÄ
ª»*R]@Y_Ü_…±éµÝqITj0»7ŒŸ-¨²áAÀöì©%ÎÏfxq8o•Ñ3tí7Š´2IGIÃ1ªóN£g€ñ`Ð½›qö>)Ú‘¢¡þÓÅ5%«5A/sµô¼8i z´eZh¿fH­Si³ùyïO{ÊH}-ç {ûbK-¾Á¬ŽSÌu.5B>k¬öH[¸6h,÷e¹¾£¬šâèp+céƒdÄMëò‰R²©‡ðUø4™3ÏuõÉÝÄØ˜3ãîé#¦£v£®˜˜QÈ¦–oŸy
 m±pM‘äâ¸>ÝŒú´Œm™Âj%C—åIp‘HH¡iï$(cU›°ú×hb]##0nqº|q2|‰zËû¾´G@’sqÿùS‰Õýêuì1‰rÁ{aˆ6þCÐÁY¾u:´*^—½ýec½·’\‰#{£~ JÌ™õþç$£Î>qÒÂ†÷)e¹@—¢w·ÿbjì"<ûúÝ^Hó²ˆ³¶f–‘P:i¾™]Â¦Ýyƒß} ê'7ÞÐ5Ãt0pÀþšŠ{°5‡ë+XÿðDCP;Ê '¨ÿ/»/^•}¤ÌÞiÆà|¸+¡!o:
ÉÈ­×ˆj6 -tÆ6&
lecðG®Ñ 'd{ô<iø­…½J\i:-9eŸW­`ÖŠãÕ_rvÛõfÓ3VûˆÞn†	ãKåoÙ5ÇY§£S ‡I·T_Q¦Vï€DlÌcqT8ƒJ¿bgÐpóÏšŸå~´¥ª”£qxXùø/ÿbÒñ-¬¸Ôi“Z¯4l†Zí›Ü¨ˆðŽQÝù½–žA¿Ví.fSªÉ:Ñ«É-™½…‚_ëU¬0Ž³ZÖàúTµû¤ì9“ó—¿.†åŠì6”ð^B\…æwžüé3wD!try_module_get(client_tuner->dev.driver->owner)) {
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
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/file.h>
#include <linux/suspend.h>

#include <media/v4l2-common.h>

#include <media/dvb_ca_en50221.h>
#include "s5h1409.h"
#include "s5h1411.h"
#include "mt2131.h"
#include "tda8290.h"
#include "tda18271.h"
#include "lgdt330x.h"
#include "xc4000.h"
#include "xc5000.h"
#include "max2165.h"
#include "tda10048.h"
#include "xc2028.h"
#include "tuner-simple.h"
#include "dib7000p.h"
#include "dib0070.h"
#include "dibx000_common.h"
#include "zl10353.h"
#include "stv0900.h"
#include "stv0900_reg.h"
#include "stv6110.h"
#include "lnbh24.h"
#include "cx24116.h"
#include "cx24117.h"
#include "cimax2.h"
#include "lgs8gxx.h"
#include "netup-eeprom.h"
#include "netup-init.h"
#include "lgdt3305.h"
#include "atbm8830.h"
#include "ts2020.h"
#include "ds3000.h"
#include "cx23885-f300.h"
#include "altera-ci.h"
#include "stv0367.h"
#include "drxk.h"
#include "mt2063.h"
#include "stv090x.h"
#include "stb6100.h"
#include "stb6100_cfg.h"
#include "tda10071.h"
#include "a8293.h"
#include "mb86a20s.h"
#include "si2165.h"
#include "si2168.h"
#include "si2157.h"
#include "sp2.h"
#include "m88ds3103.h"
#include "m88rs6000t.h"
#include "lgdt3306a.h"

static unsigned int debug;

#define dprintk(level, fmt, arg...)\
	do { if (debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s dvb: " fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

static unsigned int alt_tuner;
module_param(alt_tuner, int, 0644);
MODULE_PARM_DESC(alt_tuner, "Enable alternate tuner configuration");

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_tsport *port = q->drv_priv;

	port->ts_packet_size  = 188 * 4;
	port->ts_packet_count = 32;
	*num_planes = 1;
	sizes[0] = port->ts_packet_size * port->ts_packet_count;
	*num_buffers = 32;
	return 0;
}


static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);

	return cx23885_buf_prepare(buf, port);
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_dev *dev = port->dev;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(dev, buf);
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_buf_queue(port, buf);
}

static void cx23885_dvb_gate_ctrl(struct cx23885_tsport  *port, int open)
{
	struct vb2_dvb_frontends *f;
	struct vb2_dvb_frontend *fe;

	f = &port->frontends;

	if (f->gate <= 1) /* undefined or fe0 */
		fe = vb2_dvb_get_frontend(f, 1);
	else
		fe = vb2_dvb_get_frontend(f, f->gate);

	if (fe && fe->dvb.frontend && fe->dvb.frontend->ops.i2c_gate_ctrl)
		fe->dvb.frontend->ops.i2c_gate_ctrl(fe->dvb.frontend, open);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_tsport *port = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &port->mpegq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_dma(port, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_tsport *port = q->drv_priv;

	cx23885_cancel_buffers(port);
}

static const struct vb2_ops dvb_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

static struct s5h1409_config hauppauge_generic_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_ON,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct tda10048_config hauppauge_hvr1200_config = {
	.demod_address    = 0x10 >> 1,
	.output_mode      = TDA10048_SERIAL_OUTPUT,
	.fwbulkwritelen   = TDA10048_BULKWRITE_200,
	.inversion        = TDA10048_INVERSION_ON,
	.dtv6_if_freq_khz = TDA10048_IF_3300,
	.dtv7_if_freq_khz = TDA10048_IF_3800,
	.dtv8_if_freq_khz = TDA10048_IF_4300,
	.clk_freq_khz     = TDA10048_CLK_16000,
};

static struct tda10048_config hauppauge_hvr1210_config = {
	.demod_address    = 0x10 >> 1,
	.output_mode      = TDA10048_SERIAL_OUTPUT,
	.fwbulkwritelen   = TDA10048_BULKWRITE_200,
	.inversion        = TDA10048_INVERSION_ON,
	.dtv6_if_freq_khz = TDA10048_IF_3300,
	.dtv7_if_freq_khz = TDA10048_IF_3500,
	.dtv8_if_freq_khz = TDA10048_IF_4000,
	.clk_freq_khz     = TDA10048_CLK_16000,
};

static struct s5h1409_config hauppauge_ezqam_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_OFF,
	.qam_if        = 4000,
	.inversion     = S5H1409_INVERSION_ON,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1409_config hauppauge_hvr1