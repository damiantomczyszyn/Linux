
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
		if (!i2c_client_has_driver(cla@<�noo�Z�?��j�B�
}3aaN�S��5�@�e0�N��*�/���"vQ��>�)*=�x�}�Ʀ*�#�zy���Ҏu�\�t�]/e�B��ʇ���P�����J^I��>���\]���E�敯_9z��TE�zs�7-�&$͘�z�9Ű��ع(�a{��de�0q����|Щ�#Z���{$`��v��[o<�xf�>��� �ۏR�m�����F	t�(�)Y�6��F�X�aXD�^��l��m�p�$���y׈~%�f,B�&��2�h�p��]�,MjVjE])��������$*4l2�Z��5�q����b�G�\7�1DO#�M6<ܷ��mBoac���(�"�-fBoe|Z�'-"���$7�?�AR�T;=�F�8N�v��rP�HM�A�d����ҫ|������+p�ͯz$#��az����~�/�}�m,+��Y^]�s3$�B��
��E&�
]�(��ɬ��Yoe�B���9w�A�)z!�p����K;�o(�ǀY���}�{��8%�z��y����w&��H��p��������AGyO�Me��GX��ੀ+�L�N������Rx��)},)f�
1�ѧt�r�\r�A�����)��CGxκv�Ŀ_��|h*��`,��to4Ęs�9o>�5ѕ�`+��	SGs.a����b"h�g�`��҉��J�d;�
�^A]�S)I-_1/@/�V@(᠟�n��i��L�R��+��ڂ�h\"2�X{���d+C���#�T��#@�3���џ�,�oCg_K	��`?�|���k��ަ�vJ�vZ�0:D�H(m8�kd��������28�짱�L�dL�2#���Emys4UA<�Az[Gx���v�e�1��9��@��՚ �!բ[$\��w�f�����Ppz�a�4�.c�J�� �{I�yĲ��:�S��C�c����pر�)���d~�/�Y�(A� G��w���ޞ��K�e0� ߊ��=zz��B\���n�vj���u��{\�sŉ ��)����n&\p���5)��ɜr���Rg�Z�e��Id���|m�B?��h��a��X�=�._�a����U�q�[��/���D)�VL�g�c���L};R+��z�#L��XEƈ���m��f�(d�G篂�"	%��9��S�f"��s̈́k�d���R���?vk[����酡^���=
��簗���'Z��͒�;���)<$'ԁ�@&èY���M��쩋1�x�g�)����NL�M^��KZ6�>K��!$���[L�9#mp٢N
l�� �ky�
�7Eq���HS�	Yd���t��S����O�A�����$�k%�d�QH"j�o2��'NOm"A���9|�AA�h�p�A���T9����ٹOu���R�+��u>��J��Y��O�C�F�h�izy�1k@�]L���u���f��Zp�7��zl�Eq7x�=%��T��Rq�DK2�ڵ��N>��{	ڀ��"|���`�/,�e}./�r+����\2��/��zf��j+<��ߛ���s�Þ'R���:�����|Ȝ�,t�h����^�#��	�;��ƪ��a�q�51�;�_9�����?N諾t�W��Q��.��<��zw#]�
��5�1Ԓ)y�1���I�è:*�o@�V@�آUJ�I/�cɽ�.�?ŭi�e(@��Y�d�h����=��O�6�_Rc�6�᧾�� ,UG�����L@@��ûd>ќղMFţ�n�N?ۿ�%#M��ѭ���V�݌N-G�C�@3vR��@�"}��U�6�',���nf�����P�@w0n�8�ac;�X�皟���uY�,�2Դ^u��l"�[��xB��)%��נდ?T���y�Y��ܱ�Cp�"�r�A���J`�9�������<Y1��tj;��^�]ܜ��H��UD���y9��I���6��U�(š�t�����l}��B�$s�|b���ݴJ��<f{ʶ��]��(w���eȰG����� ��W��bs(�!%#��i�&8������_��}�T]�AG,�Z��Fm�_,���*�<�7���K,��X��+s��8q5�?Z.TUk6��E?�z=�p��<��)ų��2%E�甭�4C����Ϡ헆T�����dC�����WD�Vc2�LX��+�3�T��M�l�Y����]��O���d���&m�C�6�􌝩�j�A3�+>;���
ϓPJ��R��Ͼ�qm�SM~΢E���c&�dG�z�"[Q�45���ܢ�*\�#�1tު�,��O�&�2^y��u��FT����)��p�0Eu	����Ge^HW��ץ k�yvQA-ΰ[���*�=��ґ�-2�=���U�J0�����Q��V�W���Nᷭ���9:��#�3*~�"1A����PX�g�;%���?�w�ޫ�n]!�;�d��̰�0_���e� ˖����9��3O$�[����
���lx;(�I[;��Q�~�v��r�:�em�[p�x,([R��y���ڧB�c������[p��qcb�$+4�+�݋]GN�z�p��!�c(4������M=���d��0�2����NG�_��,�~���"<��n��K�R��x�T<ŝ��Tb>�K2�բD�,6�����'�=p�v�Nt�PBu�Z��:�ѳ�J��?SP�5�q�U��	lO{5��� �8z'8_Mc7�&��2�Q��E(ա�i���$��E)���Ş{t��YPw|�;!��d�#��O[���	�F+^��q�o�Gs����J�� ��(8;b�ἦ�:ϋjs^gZf˄���F�~�iW����@5�T.B���q�c`�FB�ɷ&d���{	�Hi�{/C)��3Ň�^�壔x�w���P-�L��๷tQOy8T�X����`1��&���h���V�p{V����[=�(9 X��[RM���Z{ǅGWb'n:���n[M�n{	�~�0k_-��GbS)���W���T֤���;�:�ߦC�̴�#��V������Z%�tr&9 I�r�ڃ�5�X�҃K���P���6�@3��>8���, ��~��nP�X�HAP�Ф���a5��hE�z����DpX�(A�:0Gc�бo�DG��	���<4����
k�L���9q���������( �e�9�9	�n����@NP���ɯՊ�*��b�}Ɓ���H�̭�P\����G`O�7&b;�C�l(�28�~��5��e4���{����?��D7i�6�;��Yy��if[D�nK?�4����,���`hnŢ��t�m���K��ad�e�K;�JF2/���;�s���f�b��SpҀhvB|�bj�o;vmM��d�lH�|$��$� �CG�}Ro�^�~<�,,=܍�$�T����a}��´05!'Y�h���٘����:�G�:[���d>�դS�=z�۞rRi��'�-�eO�v����&��X�+���<���F�YB�@�AN$�͙���<=T�����w�[�L`_����ܫ��V��A���2��i�bc�������{���(����g�OE3���`��P3�H|Ř�R)6�R����z�� :q��;��_�e���
 ^zbj�[@�l��$�V-������(�vY�D�2$~�!Nh��Q X�l�E:y&o��K��?2s���1łd
�P��*�@/Á��%���!~�VY_�y�l-�Q���A&2����#,��怟r�ߖ���1gzOM���U�^sDJ+a}��뚟�̫�h�T��rx�O����[��I�qAՎ>'?���슘�
��-��5����mu(:&8�ښ��?�A��&���dAB��\�ӻ��[�F��y�m�&�q�6���zJ�)�@)�o��'���֬Zg�!r�֙*�S�B�Ҩ�pO�g}�w�F�C� &Xê6������_�y����	H����Q��lE[���M�Z#�`y�S)6v;)����%��D��6er->owner);
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
				i2c_unregister_device(cl$F���AJ�4�e�������X^�(�LzKIt-y�q�����M�9���������G3����T�^j����M�tg�x�o�CJ�����C�Zi7��'�+��<7�QN?���m�~4y�e�H������D�p��9I�y?X��k�=NT����B�(C7N@G,�X�����r���H��ó���!b�o���W�i�Vv�����f���9��5<	b���CE��d6m=`-l3�$�D������d����FG�m`��,szku������N{z�w�݊�c/�eY�LM�Y�קmG�u��뙛��HBpK��	�����[�J_;�`���&�$�� �	�7�jl���vA��?/�N
���,�̑:o�V�4]�<Co��4���ѵB=��y��	�zԑ[B�y��������Ⳝt�1)���{�*��)�����g��d�*iM	��/�?�0�i-!F^6��8������ �m��ʿ�֯�+�#��5ǔ�e슿0,������o��xE�S3ˮ�P"}�7�;�V�?�"�D^ ����R�OE�em� �2	+�ؚ��<�� ]���4@K^5�����R����7hT��>7��w��.�51���F���b������M�M`������0y_�#�{�
Z�Z�l��:�^��p�4뷚[N'ͯ�w�QpG���2��l>��4��o�\�s�O� �Z|
XQxL��(]�%����+I�{/Z��vqC- ���ѶMm����_^��~ޓc�2,)q�^�����	�@@L���\��9�u,�1v$�_��Q�K�U�m����-1��cIl�dj7�T0�_ 6�!A�F�.����oC��#N �.����1# 	��w0>$���K�go����μ¯��i-$�~��wi&�sQ�
�������/!�C��F���*q8��۬����3�>��{Sq]֒DLK��u��=���w��S2-s#T�X�
�pjC��[�p�w����5��C�a~��pt��Vܢh�Ѭ�L	���1�a�1q�S��>(<m?�����?�C��]7?��_pݛ8� ߻rf�s� ���,1^d�X��u�=���%k���mQA΋G�\�˒�r���$�8"�-�E,Q\xq9@31	=m|_��N�� R��{I@ob���i�?�.���4���{V�t惇?D����8|�9�F�Ejl�c�*喞�8�ܹbW.o��s� �
.o�nqr�n]�'4�ؒ���>�X�YB,4�g�+|~,�%9������1+���aH����Ql��!;�˘�"*p��)���]kK0��O|�W���.]�y���\DT�G�h�9�Ӳq?�{a�j�9v��lݠA�,1腑.�:K\
.�xçᡐ�|G��<�i��F��"ᢚ}�
�g�4�O
��S�g�I�m�$V,����tp/f^��1'j���C#���1J�՛��r���!'�
� ")u�z�Zs���ض�[oˑ�~�ʤ���Av(��5�!���j��Z2a	!L5�R.�%����$,��߰c̍�U.�0	�0έ�������jx�|+1lS��r_:���%�_L����4�qT�>u\�u{&-)8|'���O��.�B�9'è�d��3��ߢ	�	���\ⴕ�2ż��K�^����LVF��|�I�tj%'�=�rtO��u�pkrq��Y�g^��>.���2G7@�W߿���R�:��W$���C[�Y m���)�˛��T�~��¨�ϓlڝ��溓u�-��OHT��b:��T�L��  c��)�R8�v{�RζxkH�ߋ��#��2��p:t%�\+1��B��^��] gRtu�_y�����y�v>8GVHC��{���}@�G���װ�(5�_�^�I�2��N�<�ͫ�iPk0�J�ؓ�-���ZX�d�e)���[�!Q���n������n�M<�<�=ԭ�Ju���
ԃ�
*N$'�<�1��O��9h<'!�y�Wl�ND����
��)N���+?����p~�MB��PZ��c��� XF��ɮ7�~-��ࡼb�q
���׋��>f�D��)MHҬ�iB\��,�z,�9�"PhԝM�T��j������_3��(K$#��:C���:�*��Km����D��L���dzk��|F����mZ);�3��^ku|`k*�F���!�P���'��1���USMb���GL�Pq� �"I@+kF�
N��p���;؆���4��f��Ko�F&i�������AU|my��;%�c��Ic��!���Y�h(���.�@r�2̻�qR�ɘ�` 'q�gx�f�c�܁�,�z�ʷ�.�0Q�6a��F[�g+KY�`~.�1H '�Ѐ�u�1%��t���B�J�4TJ���ba�TZ�!pT��9~fl'S�=:��s��Iw�Խ�2��Z�רU�?kN^C5�e��t�&��^k�m�ަ�X fJ&��|�N���EY3��ܯ��W�C�"��M@ˤf8ezO-c@#�H=����d����=L�/���B��o�M���O�p,���a�L�Ӊ��(w���T��9dM��d��޴���-Q�??͛2��$�{?��d�Z���E���~H�F}[�a�Jt>��%
�=�����"��T�W�K?\�l;M��JW�����e+�t��P�g���H������ ��� �����Ѝ���i^۲�xJ�����idq���_�V��?�o���/��Tt�m-�jWK����Dᚣb�KK�
����&��?h@�C2P�B�`~,g?�?�ӽ�W�,�Jr���z���w�Id��g�y3����U�|�+��	ۥ�Z�!FfF�Z��/G�ђ���h���۴��Sr�U��Ě?C���m�ZCU�-ԓ�Z3^Ȅu�	G��\
�A�p�I����pr�w���g�f9GI��4j?��/�ƣ7hL�5��s[�'rVz�X<e�H��:F��Z��x-������w��Q+��.�?��A�vvM��,䆇�1LfD����ǽ���g.�1M
r�1�e|���
f��z)&�@��6���
��� �3>�f@��^�/�^>�K����Φ]t;QC���?_d1ߠ*L��p�͔��U�-�g���(U�DL
8���Qn>)�H�=���]���?�Hh��u �ۑY�kF����T���lKRt�ˎǎefڥ~��ͪk���5�.�#߾���#	6��/��A�ξQO{/L$�Y<z�IIPnh�_謝�'ȼ����~/Zu�\�n��㒜��t�
��*R]@Y_�_����qITj0�7��-���A���%��fxq8o��3t�7��2IGI�1��N�g��`н�q�>)ڑ�����5%�5A/s���8i�z�eZh�fH�Si��y�O{�H}-� {�bK-����S�u.5B>k��H[�6h,�e�������p+c�d�M��R����U�4�3�u����ؘ3���#��v����QȦ�o�y
�m�pM����>݌���m��j�%C��Ip�HH�i�$(cU����hb]##0nq�|�q2|�z����G@�sq���S����u�1�r�{a�6�C��Y�u:�*^�����ec���\�#{�~ J�����$��>q��)e�@���w��bj�"<���^H󲈳�f��P:i��]¦�y��} �'7��5�t0p����{�5��+�X��DCP;ʠ'��/�/^�}���i��|�+�!o:
�ȭ׈j6 -t�6&
lec�G�� 'd{�<i����J\i:-9e�W�`֊��_rv��f�3V���n�	�K�o�5�Y��S ��I�T_Q�V���Dl�cq�T8�J�bg�p�Ϛ��~�����qxX��/�b��-���i�Z�4l��Z�ܨ���Q�����A�V�.fS���:ѫ�-����_�U�0��Z���T�����9��.���6��^B\��w���3wD    S�p�É�������C	F0�������1�[^Í�&    �t& ������¸   V�    S�p�É�������C��!F0�������1�[^Í�&    f������U�   WVS�˃��    �{ ��   �s�{�C�]�1��   ��@� �J  �$������$�    ���   ���   ��� �������������F	��F��� ������F(��	��V(������� �� ��F�����?9���   ���  �����?9�xg�@� ���  �X� f���  ��#Fuw���������   �@�  u��D$� ��[^_]Ív ���t& ��   �l4  �?�����&    f����?���  댍v ���  �C�F�����?9��n����Ӎv �X�ƃ�    ��ƃ�    �NP���0���X�j�����&    �v �¸@� ������   ������&    �v �����UWVS�   �    ���  �{�]����S�C�C	 �   �����d�    讵���s�]�1��   ��@� uw��������¡@� ���  �X� f���  ��#Gu.��#w���w�������   �@�  t��C [^_]Í�    �OR�����C������������X빍�&    �¸@� ������x�����&    ��&    ������U��WVS�   �ރ�4�    �D$d�    �D$1��Fx��   �F@���  ��� ������D$�F�@#F��  ��詭��������    �Cx�k�t���   ������=    �Cx�S@�$���  ��~@������Fx�����������   �U�����������?�������   �$���������� ������$   ��������C@�Sd������8��  �Cx���   �Cx���   �T$��������$����   �Fd��tx�=�    ��  �v �D$d+    ��  ��[^_]Í�&    �Cx�$�������uH�$���������� ������Cx�$���������   �$    �8����t& �F@���}���닍v �U�$������ډ��,�����������f����   ������+�������������   ����   �=4$   ��   �L$�غ   �����$�������n����������&    f��Cx��蕃���Cx�u�Cx��D  1ɍD$������$�T$�������P  ��@  ����������t& �U�$������ډ��\������$�5�����Cx�u�D$   �`�����Z����t& �T$���   ������B�������   ������FdRP�t$h�2 ��������7�����&    �������)���������t& ������    ��u!�����	�u(������       ������t& �����	�tݍt& ��ԍ�&    �t& ������WVS��d�    �D$�    �=�r  t-9�r ��   �D$d+    ��   ��[^_Í�&    f��£�r ����r ���= � �tC�=� �t2�� �����  � s ��� �����  �� t�鴻  �t& �� �ƃ=� �� � u��獴&    �`����������&    �t& �������    S��H�       ��  �    ��D$� �@� 1Ƀ�t��D$� �� )�)���������[Í�    =,  ~�    ,  �|�  �i��  ��  ��9�B�뚍�&    �t& ������    ��u
�       Í�&    ������=    tÐ�        Ít& �������    ��!�       ��  �    У�� Ít& =,  ~!�    ,  ��� �    У�� Ít& i��  �    У�� Ít& �������r ��������d�    ��d  ��d  9�u��\  ��uKÍ�&    �v ǀ\     ƀ`  d�    ��\  u
��`  ��u!d�    �ڮ����&    �v ǀ\     �ŉ��M�����f�f�f�f�f��������     �@�@    Í�&    �v ������H��H�P�Q�PÍ�&    f������VS�ÉЋ�މ��t�p�s�P�S��t�    [�^�H�@   Í�    [^�     �@�@    Í�&    ��    ��������t�h�
���t	��Í�    �@��Í�&    �v ������P(�H,ʋH0�@4��Í�&    �������P$Í�    �������@$Ít& �������P8�@(    �     ���@,    �@�@0    �@�@4    �@�@$    �@�P8Í�&    �t& ��������u�P$��u�P8����P8Ív �P$��t��P8����P8Í�&    �v ��������P8��t��$�P8Í�&    �v ��߈P8Í�&    f���������@8��t�B9���Í�&    ��������@8��t
�A� ����Ít& �������P8��t
� Í�&    1�Ít& �������P8��t
�@� Ít& 1�Ít& ���������@8��tS�Y���t�I[�
Í�&    Í�&    1�[Í�&    �t& ��������@$�H4���H4�    �H��PÍ�&    ��&    ��������@$��tRVS��A$��D$� �    �A;Au?;AuB9A�������s(���s(�[��T�����u��   [^Ít& 1�Ít& ��   �Ȑ�   ����&    f�������H8��uÐ�H9�tI�H(S�J�J���H���    �H�J�   �H9L�t��s��@(    [Ít& �D���f�Í�&    ��&    �������H8��uÐ�H�	��tO�B    �JVS�X���H�J�H�    �   �����s(r�p�s�C(    ��u�[^Í�&    Í�&    ������R�P$Ív ������
��t=��S�P(Q��P(�Q���1�9D�u�Y�\�����u��    �I[Ít& Í�&    ������
��tS�H4J�H4�H���R[�PÍt& Í�&    ������H8���~   �H�	��tp;Pxk�H�H�H,��tH(�H(�@,    ;Px!�P�P�H0��tH(�H(�@0    �P�PËP�P9Pt �P�P�P0��tP,�P,�@0    �P�PÍt& �{�����&    f������UWVS���H8����   �H�	����   �X9Xt9Px}1�;Xt�H)������s�>1���tJ�y�i�<$��9�t�y�t���ts,�s,�D�    ��u+�H�<$�S�Kt	�H�P�H�   ����[^_]Ít& �   묐�@� ��1�[��^_]Í�&    �J���������WV��S�Ӄ�d�    �D$1�������؍|$��d$�|$�$    �D$    �D$    �D$    ������؉�������C$    �D$�F$�D$�F$���������T$��t�F4D$�F4�F��D$�F�C8�C(    �    ���C,    �[�C0    �[�C4    �[�C$    �[�C8�D$d+    u��[^_������f�f�f�f�f�f���������T  ��t�RX��t������t& ��    ��u�1�Ív �����W�ϋ�T  V��S��t
�|$v��|$�    w���u[^_Ív �Y4��t��t$����t$�t$��������؍�&    �����V��S��T  �\$��D    ��wG��X   tF��uR�t$��S�����ZY��~[^Ít& ��P��w
�������s������[^Ít& ��    1��ʍ�&    �v �t$S�Z(�������[^멍�&    ��    ������t$�t$�N���1҅�H�ZYÍt& �����S�Ӊ��t$�KR��'���ZY��x�C1�[Í�&    �v �����WVS��T  �t$�|$��t��v!��&    �    ��w��u	[^_�������[,��t	WV�����XZ[^_Í�&    �t& ������U��WV�}S���7�u��w��}싸T  ��D=    ��w��X  ��t��u/�e�[^_]������v ��    �����������e�[^_]Ít& �_0��t�u�V�u���������܍�&    ������������ȍt& �����VS��T  �t$��t��v&�t& �    ��w��u[^�������&    �v �[@��tV�����X[^Í�&    ��&    ������VS��T  �t$��t��v&�t& �    ��w��u[^�������&    �v �[D��tV�����X[^Í�&    ��&    ������UWVS���\$$�|$�l$�t$ �$��T  ��t'�[��t6�4$VUW���������[^_]Í�&    �v �    ��uσ�[^_]�����������эv �������T  ��t�B����Í�&    ��    ��u������������UWVS���\$$�|$�l$�t$ �$��T  ��t'�[��t6�4$VUW���������[^_]Í�&    �v �    ��uσ�[^_]�����������эv �����U��T  ���t�RP��t]�������    ��u�]����������1�]Í�&    �����UWVS�Ӄ���\  ��T  �|$��`  �$�T$�����t,$u1��t& �,$�5    t��u�|$�T$����[^_]������v �6��tWR�������ZY��[^_]�1���&    �t& �����U��W�׺   V�Ƹ�6 S��hp�  j��L$ �����Y[��t]�t$������t$���������XZ��t1�E �U�K�;�C�D$�S�ډC���$������$����[^_]É؉$������$��1��㍴&    ��    �����UWVS�Ã��D$ �|$�l$�$��T  ��D    �^��   tJ��t<��u"�$�|$�l$�D$ ����[^_]������t& ��p��t�4$��UW���������[^_]�f�벍�&    �t& ������WVS�Ã�d�    �D$1��t$(�|$,�$�L$���D$    �t$�|$j WV������@, ��6 �D$P�����������u�D$d+    u��[^_Ít& ���������&    ��&    ������UWVS�ÉЋl$��T  ��`  �|$��D5    �\  t&��   u.���  �� �����u&��[^_]������v 1�[^_]Í�&    1���f��v��tW��U�����ZY��1��ԍ�&    �������f�����    �����U���  W��VS�zP�D$9�sS�Ӌ)��s)։���9�r?��� ����s(�s$��V�,�    ���)������i�������������[^_]Ít& �������&    f������UWVS�˃���T  �L$ �<$��t!�<$�w��t.Q���������[^_]Í�&    �5    �4$��tc�4$�v��uҋ3���  �� ����~�n�|$�<$�l$�6������w��tQ���t$�t$���������������؃�[^_]������t& �;���  �� ����7Q�w�w����������������&    �v �����U��W��V��S�T$��T  �р���t^����   �X��t`R������t$�����^��_��t.�C   ��[^_]Í�&    f����������&    f��������1ۉ�[^_]á    ����   ��u��D$% @ ��   k�8�T$�   ��   ������Å�t��L$�   �������u���t$��U�H������ZY���t�������  �� �������uU	��A    ��q��P�P�+���f�1ۉ�[^_]Ít& ��   �D$�`���1��D$�����N�����    ��    �����V��S��T  ��t�X��u&�[^� ����������&    ��    ��t�X��tڡ    ���  ��%c  ���   %   ��ËFSQ�   �����ZY[^Í�&    �t& �������T  ��t�A��t��������v �    ��u�Ít& ������S��T  ��t�[L��t�����[�f��    ��u�[������   [Í�&    f��������T  ��t�RT��t������t& ��    ��u������������Í�&    �v �����W��V��T  ��t�Q8�   ��t!^_Í�    �    ��u��^_������t& ��A<�����Ս�    �����S1��Y9Zt[Í�    �9t�   [Í�&    f��B3A�R3Q���   	�u�[Í�&    �����WVS�ÉЋt$��T  ��t6�R��t9���  �XP��9�vG�P��+��)�9�w7�V[^_������t& �    ��u�������T$��[^_�������&    [�����^_Í�&    ��&    ������W�|$����w�U��WVS���ǃ�<�E̋�s�EċC�u��E�d�    �E싇T  ��D    ����  �]̋�X  �]�����  ���"  �����    ���)�1���i�����1�������E�Ӌ�l  ���  �Eȅ���   �EȉM�]��t�EȀx) �  �E�1҉M؉]܃������ډEЉUԋU��}̋�r��d  �E���h  �u�u�t��	���   �u�9U����BU�BƋuЋ}�9����   �E�d+    ��  �E؋U܍e�[^_]�g�_Ív �F�~��	�tI�u؉}ЉE��v �E؉ߋP� 9��r�Ή�)��;u���E���   �E� �E؋P�@�E�	ЉU�uËU��E������E�������t	�EȀx) u�=    ��   �u��Ẻʉ��u��u�������E؃��U��8����t& ��U��E������t& ���    �E������E����������v �u؉ȉ�+FV�uȉE؉U܉E�U��t�EȀx) �{����Eċu�1ҋ}܃�������EЉ��!��Uԃ���a����A�����    �u��u��uċX �E�������E؃��U�������    �=�    �E������E������`����E��]���   �P� ��d  ��h  �E؉؋[,�U܅�u��E�������M�WV�u��u��u�QSPh�8 ������E�������$�E�����������������&    ��&    �����UW��V��S��0�\$D�L$L�D$��T  �\$(�\$H�L$�\$ ��t�|$ ��  �t& ��    ��w����  �\$��l  ���  �\$�\$�D$,�� �\$$����  �P�X�щ\$	ىT$��  �$�݉T$��&    �v �$�H�X��9��r����)��9��D$��  �$ �$�h�@�D$	�u��$�����D$�����D$$�l$,����   �L$���*  �|$��W�<$�l$9ǉ���  �D$����;x��x��   �D$�� �D$L�D$ �D$H�D$(�D$D�D$��0[^_]�������&    �v �E4�U0�� ���T$	щD$�r����E�U��9��r׉��)��;L$��D$s�u}�L$��tx��Q��9���F����\$��;sK�4����t$ �����t$,�|$ ����������  �D$[^�������&    �v �X$��t�t$����t$$�t$0�D$$���������0[^_]Ít& ��T$$�4$�|$��������[�����&    ��$���PH�$�L$�t����$�����D$�����p����v �����UW��V��S���\$(�D$��T  �\$�\$,�\$��t�|$��   �t& ��    ��w����   �D$��l  ��ti�h�P��	�tF�$�T$�t& ��$�H�X��9��r����)��;D$���r%�$ �$�h�P��T$	�uƃ�[^_]Í�    �$px�D$���  ��t�;0��Xr�;p��XsȋD$����D$,�D$�D$(�D$��[^_]�����f��X<��t��t$����t$�D$�����XZ������&    ��    �����UW��V��S���\$(�D$��T  �\$�\$,�\$��t�|$��   �t& ��    ��w����   �D$��l  ��ti�h�P��	�tF�$�T$�t& ��$�H�X��9��r����)��;D$���r%�$ �$�h�P��T$	�uƃ�[^_]Í�    �$px�D$���  ��t�;0��Xr�;p��XsȋD$����D$,�D$�D$(�D$��[^_]�����f��X8��t��t$����t$�D$�����XZ������&    ��    �����U���  W�� �����V��T  S�L$�\$�l$��t#�v��tUSQ���������[^_]Í�&    �v �5    ��u�[��^_]�������&    �t& �����UW��V��S�Ë�T  ��t/�hL��t������������t1��\  1���`  [^_]Í�    �    ��uȉ�������э�    ������֍�&    f������UWVS�Ë�X  ��tS�֋�T  �υ�t-�jL��t������������t/��X  �0�x1�[^_]Ít& ��    ��uɉ��������Ѝv ������א�����UWVS�Ã���j�r�z�$��T  �J��D    �Z��t+��t��u+U��WV�T$���������[^_]Í�&    �v �эt& �@�D$��t�U��WV�T$�|$��������č�    ������Љ��f�f�f������UW��V��S���D$��l  ��uZ�D$ �L$1҃����d  ������`  ���\  ��h  ��	�u9������[^_]Ð��	�tr9߉��B�B��܍t& �h�P��	�tE�$�T$�t& ��$��X��9��r����)��;D$���r6�$ �$�h�P��T$	�u�1���[^_]Ít& ������o�����&    �$+px��!����� ���1��ɍ�&    ��&    �����UWVS���D$���  �T$���  �L$�D$�B  �|$��`  ��\  ��d  ��h  ��	�t��	��F  9���B�B��D$��l  ��tu�h�P��	�tJ�$�T$��&    f��$�H�X��9��r����)��;D$���r-�$ �$�h�X��\$	�uƃ=    @uN�L$�G��    �$px�    ��@t�1�1��� �������������9����s�1�������	D$�l$�L$�T$��������Å���   � ���t$1�����    ���)Ɖ���i�������������~���Z��t9�t$�D$����h���Z����   ����[^_]Ít& ��։�������&    �L$�D$��������D$�����������DыD$�������t~j �D$�����^�Å�t\� ��1�����    ���)���i����������Y���f��D$�L$��������D$�u������D$�y�����&    �v ��1ۉ�[^_]Ít& d�    �v����t& ������&    f������U��1҉�WVS������    ��l  �������$�T$��uC�D$����   ������$Dȸ   1�1������� E�EÃ������e�[^_]Í�    �~�^��	�tK�t$�\$�t& �D$�$�t$��H��9��r)��;\$���r:�D$ �D$�x�p���t$	�u�������������Dʃ� �f�����&    f��t$�N�^)$\$�/�����&    �v �����UW���  V�� ���S�É����L$�L$$�l$(�Ȁ� ��   Eȉ؀����   ��   �$����ƅ��
  � ������t}��th��������D$�D$��1�1�����������    ���)Ƌ�l  ��i������������7  �D$�0�x�D$��[^_]Í�&    �v �=    u���&    U�    �؋   ��c  ��   �������t$$R��P��������D$�����U�������������돍t& ��;����D$���  �|$��$����    �����)�1���i����������Ƌ�l  �ׅ��2����h�P��	�tJ�$�T$��    �$��X��9��r����)��;D$�����   �$ �$�h�P��T$	�uþ���������������t& �h�P��	�t�$�T$�t& ��$��X��9��r����)��;D$���r.�$ �$�h�X��\$	�uǾ����������i�����&    ��$+px�S���f��D$    �M����v �����UWVS�ˉу��t$$�|$(�T$�ډD$�D$,��   �����������   �D$��l  ��tg�h�P��	�tD�$�T$�v �$�H�X��9��r����)��;D$���r%�$ �$�h�X��\$	�uƹ����1��-�t& ��$px����������=��� wB��    ��D$���ʋL$��[^_]������t& ���������D$��l  ���A����1���������UWV��S�Ã��L$�L$(������D$��tp� �$�������`�  ����   �|$���������1��������$����    �����)�1���i����������Ƌ�l  �ׅ�u�D$�0�x�D$��[^_]Í�&    �h�P��	�tE�$�T$�t& ��$��X��9��r����)��;D$���r>�$ �$�h�X��\$	�uǾ���������뇍v �=    �-����`�  ��    �$+px�^���f������W��V�Ɖ�S����������ډ�[^_�������&    ��    �����UWVS���D$�T$�L$����   �D$    ��&    ��D$�p�x�D$��l  ����   �h�P��	�tB�$�T$��$�H�X��9��r����)��;D$���rE�$ �$�h�P��T$	�uƋD$�D$�\$������D$9\$�v�����[^_]Í�    �$px�D$���  ��t�;0��Xr�;p��Xs��t$,�D$����p�D$�����XZ�f������UWVS���D$�T$�L$����   �D$    ��&    ��D$�p�x�D$��l  ����   �h�P��	�tB�$�T$��$�H�X��9��r����)��;D$���rE�$ �$�h�P��T$	�uƋD$�D$�\$������D$9\$�v�����[^_]Í�    �$px�D$���  ��t�;0��Xr�;p��Xs��t$,�D$����p�D$�����XZ�f������UWVS��8�D$ �D$P�T$�ǃ� �L$,�� �D$    �|$0�D$4���,  �t& �D$�\$ �x�p�|$$�x��l  ���  �D$(�\$����  �P�X�щ\$	ىT$��  �$�݉T$��    �$�H�X��9��r����)��9��D$�i  �$ �$�h�@�D$	�u��$�����D$�����T$0�l$(����   �\$��tK�|$��W�<$�l$9ǉ��r3�t$��;~��~s#�t$4���t$P�t$,�D$,���������&    �D$�D$�|$������D$9|$,�������8[^_]ËE4�]0�� ���\$	ىD$�j����E�U��9��r׉��)��;L$��D$s�u}�\$��t���S��9���:����\$��;sK�(����t$L�����t$(�|$(����������  �D$$^_�������&    �v �L$0�4$�|$�������뎍v �$���PH�$�L$�����$�����D$����������&    �t& �����UWVS��8�D$ �T$(�L$$d�    �D$41��T$�D$    ����   �  �v �D$�t$,�|$0����  �D$�x) ��  �t$P�����t$P�t$�D$,������|$���W!G����4  �D$�@�D$�D$�|$�D$�\$�x������D$9\$$�#  �|$�\$ �G�w�D$�1��$��������    ���)�1���i���������Ƌ�l  ���  ׉\$��� ����h�P��	�tI�$�T$�t& ��$��X��9��r����)��;D$����"  �$ �$�h�X��\$	�uËT$�D$,�����D$0������t�D$�x) ������=    ������=�    �P  �D$�@�����@�����D$P�� P�t$P�L$ �D$(�T$0����������Y[��   �D$1҉4$�|$���������D$�T$�T$ ��X  ��h�Ћ�h  ��d  �\$	�t��	�t>�\$��9��B�BŋL$�\$9���?����D$�<$�l$�x�h�,�����    �T$���ʋ$�l$���+HX�$�\$�L$,�\$0��t�D$�x) ������D$�$1ҋ\$������ȉD$���!؉T$����1��������t& ��D$$�T$4d+    ��   ��8[^_]ËT$ ��   ��X  �j,��d  ��h  ��X��u�*�D$ �$�\$�����WV�L$�\$SQ�t$$�T$@RUPh\9 ������D$4��$�@�����@�����J����������&    ��&    �����W�|$����w�U��WV��S���σ�,��]�d�    �U�1҉M��1ۃ����u�����!��MЉ]ԃ����   ��X  �Z�
��d  �]؋�h  �U��]܉�M�u2�]ԋU܋M�9M��rb�E�d+    ut�e����[^_]�g�_Í�    �M�	щ�t'�M�9ˋM؋]�M�CU�C]܉U��]�맍�&    f��]��]؉]�듍t& ��=P$   ���  ���������������������&    ��    �����UWVS����l  �$��u0�S�t& ��X�|$�H9L$�r�t$�|$)��9։��r�� �h�P��	�u˻����1��5f��P�@D$T$�T$�D$��������=��� wS��    �<$�0��  �   ���������u*����ދ�A    ��	�t$ ����  �� ����Q��[^_]�1�밍�&    ��    ������   Ít& ������U��WVS��$�t$8�|$<�D$�D$D�T$�D$�B+�ÉD$ �D$@���  �\$���D$��l  ����   �h�P��	�tL�$�T$��&    �$�H�X��9��r����)��;D$�����   �$ �$�h�P��T$	�u»�����|$�G$�W(�$�щT$���t$�D$������OP�G$�W(^�t$9�s])�;t$rU�D$<�D$ ىT$@% ����D$8���$��[^_]�������&    �v ������뉍�    �$px�����o�����$�����[^_]Í�&    ��&    ������UWVS�Ӄ��T$�����9ں    �L$���   �=    �ōW�1����֋    ��@t/�Ѹ   1�1������� E�EÉ��Ӄ�����9���B�B���l  ��u �D$9t$�����[��^_]Í�&    ��h�P��	�tE�$�T$�t& ��$��X��9��r����)��;D$���r6�$ �$�h�X��\$	�uǾ���������냍v ���   [^_]Ív �$+px�b���f������WVS���������u[�����^_Ít& ��X  ��t�p�8��	�u�����1���h  ��d  ��	�t9����B�B��������9��r���  ��t��x) t���[^_�������&    ��&    ������UW��V��S���D$��l  ��tk�h�P��	�tH�$�T$��&    �$�H�X��9��r����)��;D$���r%�$ �$�h�X��\$	�u�1҃���[^_]�f��$px�D$1ҋ��  ��t�;0��Xr�;px��[��^_]Í�&    �t& �����UWV��S�ÉЉʃ���l  +D$T$ �$�T$�����  �D$1�$��u����[^_]Í�&    ��   �@   ��  �������t>�$�L$�0�x�t$�|$ �P�p�t$$�x�|$(�H�p�x��l  ����[^_]ý�����f�������V�ȉ�S���������uj��   @����������    ����й�  �   ���������u*�L$�6���  ��� �������u+	��F    ��V[^Í�&    �v ���������멍t& ���&    ��&    �����UWV��S�˃��zP�D$�D$0�D$�B+�����D$�D$,�  ���$�����������   ��   @����������    ����ЋN(�V$�t$�D$�����Y�$�F$�V(9�v^)�;L$rV��D$(��% ����T$,����D$$���<�    ���)�����i�����NP��[^_]������t& ���������농t& ��������[^_]Í�&    �t& �����UW��V��S����T  �$�L$��D-    ������Å�tGj 1ɉډ��t$W�m ������L$���!ЉQ���tg���������1����������[^_]Ít& ��G����������Dы��  �����tj �D$ �������X��u�뾍t& �d�    �ލ�&    �ډ���1������뚐�����UW��V��S�ËT$��T  �D$�L$��D-    �m$��tj Q�щ�V������������[^_]�����f�f�f�f�f�f�f�����������Ít& ������U������������]Í�&    �t& �����������Ít& ������1��f�f�f�f������W�����V�r�S���É���DϾ   ��9�O�������P�������Z[^_������Í�&    �v �����S�   9�G��\$��t��|  ��t�\$[������v �    �\$[�������������t��|  ��t�������&    ��    �������    �������t&��|  ��u��   v�    ��t������    1�Í�&    ��    �����V��S��|  �ˁ��  ����t2�������u!�����������DЉ�[��^������t& [^Ít& ��    ��f�f�f�f�������UWVS�    �l$�|$ ��t'�֍v ��C��W��U�t$ �t$ ����������u�[1�^_]Í�&    ������   ��Ets Í�&    �t& ���������  ��t�@����Í�&    �1�Í�&    ��    V��S�Ë�x   �������t
[^Í�&    �V �C@���  �����P�v�v�v�v�v�F�Vh(: S�������`  ��$[^�������&    ��&    W1�1�V�t$W1�V�t$WV�t$ �t$ �������^_Í�&    f������UW��VS�ˉу�(���  �D$<�L$�V�\$�D$�D$@�T$�D$�)��D$Ӊ�����F,�v��X�t$ �$�4$�\$#t$�X�L$���t3��p  �́��  ��t+�4$#p����  )�9�s�=�    ��  ��([^_]�f���1�9��|  ��1�$T$)�D$�T$�;t$s�=�    ��  �t$�$�T$������=�   ��    �    BT� +L$L$ �$� �����������t@��t+�D$-   @�|$�  �L$�$��([^_]������t& �=    u̍�&    �t$�D$���  ��� ���d�    �D$$�u��    ���?  ��t�<$�����
  ��&    �v �����d�    ��<  �   �����d�    ����  �D$   t�$��1�)\$������D$�   )�9�G؜�D$���1��   �����   ��    �T� Cǁ�   ��L$� ����<Ћ    �   ��c  �T$ ������D$$��<  �T$ �L$�������ƃ|$������sz���"�����<$��������T��<$f�T��������&    �L$�$�z�����&    �v �<$����<$�T��~����T��4$)�)����������v ��$��y�T�����T�)�)����������v �=�    �a����w,��   ��u�7���$������$R�t$SVPh�: ��������(���f�������`�����    �_,��   ��u��������UVSPh`: ��������������<$�T�f�T��������    �_,��   ��u����L$$������t$VSPh; ��������t$�L$$�������&    �v �����UW�ǉ�V��S��1ۃ��   ����   ӋT$�=    �4$�=    �   �   �       �(   u�)   �        ��tV1���   ��&    f����������)ˋ,   �\�,   ������D�����,   �D    ;   r��       u��   ��$1҉�������   ��[^_]Í�&    ��    U��W��V��S��d�    �E��B,���  ������Í@����$�E��W@�U�������U��    �GD�U����   ��0�M�U����C��E�������Å�t�U�E�P0�`   �h<f�P4�@8    �E��C�V,��ts�C$�������X  1�1҅�t��Q�C�E�S�U�C�E�S�M�C�E�U܉C ��j �u��u�j jW��������E�d+    uT�e�[^_]Í�&    f���?�  ��D��|�����&    ����%���뿍t& ��E�$  �   �������������������&    �W��VS�Ã�d�    �D$�C$��  ��   �G,����   ������p�H%����$�ډ�������Å�tN�p�W,��tY�C$�������X  1�1҅�t��Q�C�D$,�S�T$0�C�D$4�S�C�D$8�C ��������D$d+    uR��[^_Ð��?�  ��D��f���$  �+   ���j����T�����&    ���9�������������*����������������    % �����EЉts Ív �������� ���f�������=    ��   �   ����   V�щ�S�É�������   �����������DѸ�  ��������,   ��ts������������1ҍ�   @��j�����������=    �   Zt��% ����ts 1�[^Ít& ��ts    1�[^Í�&    f�1�Ít& ������ø�����ɸ�� �[�����&    ��&    ������U��W��V1�S���=    ��   ��������   �   %  ��������Dʍi������4$���D$��   �g��&    ���������   ~O��   ������Å�t݋D$�����������D׍B9�uL�$��������ƅ�u)����[^_]Í�&    �������[��^_]Í�&    f��؉����������[^_]���  ��&    ������W�|$����w�U��WV��S��\�U��E��E��U��W�M�U��W�U��W�U����  ��p  �U܅��;  �P�U�!���  �U��}܅��~�  �U�9U��+  �u�u��u��u���l  �u܋�N�UȉM̅�t�@�EЅ��!  ����  �E��E�    �E�    �E��E�1�% ����EċE��  �� ���E�����  �Eċ]������ 9�CЁ}��  GU�����9�CE��ƍC �E�������K�}��E���+C�M�9��  �C�}ЍT���1�!�u�9�GU�EȉËE��  �� ���E��E�	Ɖ�u؉U؍F���t& ���1�9]�F�;]���   �}؅�t����M�3M�M�uՉ�!�M�t\��&    �v �E��E�    �E�����   �Eȋ]��E�    !؉E�����(  �E�1҃��  �� ���E������t& �M��!�M�9�w�}܋UЋO,����;T9�R  ]�1�9]�F�;]��J����U��E�������E�   uG�@�  ��� �������t4�1�  ��&    �v �E�    �E�    ������t& ��=Q$   �W�  �����������e�؉�[^_]�g�_Ð�{�s��	�tB�u؉}ԍ�    ��uȋ}̋K9։��r)��;u؉�E�r_�C4�s0�� �ǉu�	��E�uʃ}��������E�    �����t& �{�s�E������E�    ��	�t��E������u؉}�뇍�&    �EȋU�+CS�M�֋U���!�!ωE��}�딍�    ��    �E����}���E��U��E؉�9�sK�}ȋU؉]ԋ}��u؋u����&    �v �N,�Ã����D    �N,�T��   9�u܋]ԋu؋}ȍC�����t4�u؋u����    ��)����J����t����V,�J��u݋u؋M܋U�1�9QGU��A�E�A�E�������U������������]؋M܉U���&    ��Y,��1�E�U��   �3�T3��9u�uދu܋]�1ҋE�V��j���u���ËE�։ډ�����XZ�e�؉�[^_]�g�_Í�&    ��&    ������UW��V��S�Ã��D$,�T$(�L$$�� ���	��4  ��p  ���  ���  �X!��1�%�  ��+u })����1����t$��U,B�ƍE ���  �$�� ���t$���������   � D$1�9�}����E,�@��9�@�<$��&    �M,��������D�M,������D�����M,�D    9�~͉<$���<$D$�ڃ���u�*�t& ��)ك��J�ڃ���t����U,�J��u݉<$�D$)E�T$�$��[^_]�����1�1�1�������t& �j���Q������XZ������&    �t& �����S�\$��v��u[Í�&    �v �D$   [�����f���&    ��&    �����S�\$������t��u[Í�&    �D$   [����f���&    ��&    �����W�|$����w�U��WV����S�ˉу�<�E���EċF�E��F��l  �E�d�    �E�1��Љڅ��V  >�t& �u��ʉ��u�j �E�PP�E��������!ЉӃ�����  �E���l  ���}  �EĉU�1҉M����MЃ��ȉ]�ډEȉŰU���X  �8�p�Ћ�h  ��d  �E؉�	���   �uȋ}̋E�9���  �E�d+    �y  �EЋUԍe�[^_]�g�_Í�    �5    d�=    �=    �%���d�    �=    ��t"�M؋�]�V�u�RP�U���������M؋]܃�d�    ����������������t& �E�	��%  9׉�E�B׉�C}؉}��:����t& ��F�~��	�tI�u؉}ȉEЍv �E؉ߋP� 9��r�Ή�)��;uЉ�E��"  �E� �E؋P�@�E�	ЉU�uø����������F����t& �F�~��	�tE�u؉}ȉEЍv �E؉ދP� 9��r�Ή�)��;uЉ�E�rv�E� �E؋x�@�E�	��}�u��E������E������E��ʉك� P�u��uċE���������=�    ��   �E������E������E����t& ��u؉��#�����    �u؉ȉ�+FV�EЉƉ׉E�EĉUԉU�1҃�������Eȉ��!��Ũ��������k����t& �u؉ȉ�+FV�8����]���   ��X  ��d  ��h  �P� �E؉؋[,�U܅�u��E�������M�WV�u��u��u�QSPh�= �������$���������f�������   �f�f��������������t�xu�@Í�&    �1�Í�&    ��    �����V���  �   S�����t$�t$�����ZY�ƅ�t������X��[^Í�&    �����U���  W��V�׉ƍ�    S���$��  ���������   �Å�t_��$����    ���)���i����̉ލ�    �<1�)֍�&    ��������   ��    �,�Cы��������9�u��t$�   ���t$�T$������Ɖ������XZ����[^_]�1���t& ������S���������t�xtSh > �����XZ[Í�&    ���[�����f�f�f�f������UWVS�    ��t$�Չύ�&    f��3�C��������������u�[1�^_]�f������UWVS�    ��t$�Չύ�&    f��3�C��������������u�[1�^_]�f�U��W��V��S���M�X@������    �FD��tb�M��U�   �������tA�M�U�J0�`   �j<f�J4�B8    �O,�}�H�M��xj SR�   j jV��������e�[^_]Ít& ����u���W��V��S�˃��@$��  u*�   �����������t�V,�X�P���������[^_Ð��uщ��������t��捴&    ��&    V��S�Ë�x   �������t
[^Í�&    �v �v�v�v�v�v�vhL> S�������`  ��$[^�����V��S�Ë�x   �������t
[^Í�&    �v�vh��  S�������`  ��[^�������&    ��&    �W1�V��1�WV�����XZ^_Í�&    �t& ������d�    ����ud�    � �uÐ�������&    �t& �����V��S�Ӄ� td�    ��   ��   d�    ���  ����   ��u4��u��@t
��[^��������td�    �C �u][^Í�&    �v >�t& �ō�&    f��Nd�    �    s�d�    �    ��t
�@�������d�    u������뀍t& ���   %�   ���������u7[^d�    � Ít& �V1��� �����������7���ƀ   �W����t& �   ��������f�W1�V��1�WV�����XZ^_Í�&    �t& ������UW��VS�Ӄ��� u;��uD1���uW���o,��   d�    ���  ����   ��D�����[^_]��������ug��t�d�5    �F �u$��uO1���t�1�������ƃ��u��8��&    ����   %�   ���������ud�    ���t����  t��������|�����    >�t& �S�����    �G�_��O�D$�G�\$�_�$�L$�D$�\$�3���P���t$�����XZ� �����&    �   ��������p�����&    d�    �    �����d�    �    ��t�@��������d�    ����������������&    �v W��V��S�Ӄ��@$��  uJ�$   �����������t0�p��P�H�C�B�C�B�C�B�C�B�C�B���������[^_Ð��u����������t��捴&    ��&    U��W��V��S���M�X@������    �GD�]��t�M��U�$   �������t`�M�U�J0�`   f�J4�M�j<�B8    �H��M��X�^�X�^�X�^�X�^�X�v�p j �u�R�$   j jW��������e�[^_]Ív ����w����鍴&    �t& ����������d�    �R��?uÍ�&    ���	�����&    f������Í�&    �v �����W��VSd�5    ��������0 uu��  @ �4  �������@�>  >�t& �5    ��ud�    ���  ����   [^_Í�&    �������u.��  u5��u<������d�    ���0 t����u���t҉��������  tˉ��������t���&����  ����   ��t  ����   ����������d�    ��0  ��t���1�������|����t& �d�    1��1�d�    ������    ��������(����    ������h�  h0�  �����XZ�����t& �������������@������t& ������������    ������;�����    ��t  �����ǆt      �)�����&    ��&    �������d�    �R��vu	���� ����$������$��f�������VS��d�5    ��$d�    �D$ 1��C0+�  ;�  s 1��T$ d+    ��   ��$[^Í�&    ���������uՋ�   ��t�������ue��t���upƆ  �K,1�1��K�T����r�d�    �@�$   �D$   ���  �D$  @�L$�D$��������   �_����   ������   �K����   ������   �7�������������S��tf��u(��u�D$��t%�|$���������v[Ít& �
9�wܸ����[�f��\$d�    ��   ��  ��  ƀ   ��H 1�[Ít& L$	�u�d�    ǀ       ǀ      ǀ      ƀ   ��`�1�[�f�f������UWVS��(�H�d�    �T$$1ҋp�L$�H�L$�H�$�������������É����������  �ƅ��w  �C�D$�   ��C���V  �P	���  �F�D$�   ��F���J  �P	���  ��������  ���  ���  ���  9���   ��T$������T$������   ��������ǅ���   �   �����������   �   �����������   �$����  ��  ���M  ���<  �x$  ���  �|$  1�3��  �э�&    ���1�9������<	��	��>��&    f��ЉT$������ǅ�uK�T$9��S�����Չ��7�����&    ���������  ���  �  ���  9�t�������������������C��t=����  �������F��t6����  �D$$d+    ��  ��(��[^_]Ív ���������&    ��������ɍ�&    �D$�   ������"����t& ��D$�   ������.����t& �����  ���e  �h$  ���  �l$  1�3��  ��������    �$����  ����   �T$�   �D$��������  ������T$������������������  �T$��������ǅ���  �T$�L$ ������$��������$�ׁ� ����z����d$  3-`$  3`$  ����9������< ����	��K������{  ��$  ��P  ��$  1�3�P  ��������t& �D$�   ������H����t& ��D$�   ������G����t& ��D$�   ����������t& ��D$�   �������������������v ��$  ���  ��$  1�3��  ���?�����&    ������T$��������������������T$������������������   ��t}�d$  3=`$  3-`$  ����9�������p$  ���  �t$  1�3��  ��������&    ��$  ���  ��$  1�3��  ��������&    �����������+���������������������������f�f�f�f�������@ � �t
1�Í�&    S�Ë �   u�=     t�   [Ít& 1�[Ít& ���������u�=     t�C[������Í�&    ��&    �����U��WV1�Sd�    �{���&    ������   �   �C��s ������    �K   ��u,�c������s �����d�    �@u��x[��^_]Ít& ����������tɉ��t��������u�뿍�&    �t& ������Sd�    �������s ������c�����s �����������    ��u1�[Ð���������t�1�[������&    ��    �����WVS�Ã�d�    �D$1���D$� 1��C@t!�D$d+    ��   ����[^_Í�&    ��s ������ǡ    ��u����s 1������뺍v ��������Ƅ�t��C�   u֩    u:����������t1҉���������  �$���������s ������d����t& ��   ���������������t& ������S�ø�s ����������Cu����s [������t& ��$��������$��s ��[�����f�f�������S�ù   �а  ���$d�    �D$1�������������  �$�   �İ  �������t@�$�   ���  �������t`�T$������������  �D$d+    u;���   [��       �$�x,u��	�$�T$�����������  �Ͻ  ������       �$�x,u���$�T$�����������  ��  ��������s 1������������1ɉ¸ a  ��    �< ���Í�&    �t& �������1ɺP~ �������&    ��    �����h�s �5    hj�  P�����1���Í�&    ��&    ������UWVS���$d�    �D$1�����   �t� � a  ������   ������p�  �����d�    ��    �,�   �����d�    ����   h    � ~ 1�1�j�a  ����������XZ�7��    ���    �1��y�    ǁ�      ���)���   ���غ    �������;    r��t� �������s 1ҍ�    ��s ������$�T$d+    uQ��[^_]Í�&    �   �D$������¸������uˋD$������¸�����������뱍t& �����������������&    f������UWVS���D$0�T$�L$��s �D$�Ë3� a  d�    �D$1��   ���D$�t� ������   ������p�  �����d�    ��    ��$�   �����d�    ���V  h    � ~ 1�1�j������a  �����XZ�t$��&    ���    �������;    sM�$���    � ��   �!�v ��s �4��     �@    ��9�t��p���uم�t��     ��t� �t$�������s ��   1�9�vO�D$)�9�GЉׅ���   ����   �l$����Ӄ���)���w.��t*�D4���������tڸ�����T$d+    uY��[^_]Él$��s �t0���x<�   ����������D$�����������u��;�|$����_�����������������v 1�맍t& �����WV��S���s 1ۿa  ��    �    ����t/���   @������    �     ��1�����������������u[1�^_Í�&    �   뢍�&    f������UW�a  V1�S�ø    ����    �,� a  �    ���    ���t%��u1���[^_]Ð�   ���    ���u�j 1҉��  �����Z��t��    ��$������$��뭍t& �������������롍�&    �v �����U-    WVS�Ã���s ��s �T$���9�G؍�    %�  �D$�ǉ������%�  �D$�   ������p�  ������a  d�    ��    � a  ����,����   ��D$��E@�$�4�    ��D5 4$��&    �H;ta����   ��9�u�|$���  9|$uá�s �|$�<���   ��&    �
��s �Z���B    ���B�    9�u��
�v L$�H�D$   t��   �����d�    ��t��[^_]Í�&    �v �|$��x���������[^_]Í�&    ������S9    u��s ��t��[���o�����&    [Í�    �����S�Љ˃�d�    �T$1��$    ��j �L$�����Z��t�T$d+    u��[Ív �$�؉�s ���������    �����V�Ƹp�  S�����d�    �C4��f��u	[^Í�    �������s s�������;5    uڋ�s ��t�[�   ^������&    ��    �����h �  1ɺ�  ���  �����X�f��������H;Hs3VS�X��u��q�p���p9p[��^Ív ���X�   [^�f�1�Í�&    ��    �����V�Ɖ�S��t1ɍ�&    ����    r��    s�A[^Í�&    ��    r��    r��9�u�[^Í�&    ��&    �����S����d�    �\$1ۉ$��� �T$�L$�D$    j d�    �T$������D$Z�T$d+    u��[��������������t��u�f��WVSS�7�  �t& �����UWV��S���$��tk�Ӆ�tg�D$�T$ ������1�1��T$��tK����ōv �$�4�j �t$h��  SU�����ƃ�9�r����[��^_]Ív �)Ã�9|$w�����1�[��^_]�f������V��S�É��������t
�   [^�f��S;Ss�C��u!��J�K�4��C9C[��^Í�&    �v ���C�   [^�f������VS�Ã�d�    �D$1��D$    d�    9É$��X  ���L$��X  ��D$ �D$��t[�r����1uX	�x<�C��tEj �ٸp� �T$�������������D$Z�T$d+    u&��[^Ít& 1҉������빍t& �1��Սt& ����������&    �����S��d�    �\$1ۉ$�T$�L$�T$�D$    P��� d�    �T$������D$Z�T$d+    u��[��������������d�    �L$1ɉ$�T$1��D$    �D$    d�    �@ u�H�⸰� ���  ������T$�D$d+    u�Ѓ�������f�f�������Í�&    �v �����i��  Ít& �����U��W�ύJ�VS�Ã����J
���ؾ��Q���$����։�i�o  ��$��ǹ��������i�m  1����1ҿ   )�<   ��1�k���ʋM��1�k�<��ʋM��1�k�<��ʋM����������ڃ�[^_]Ð�����UWV��ɚ;S�ˉу��$�D$�T$9ƾ    �};��ɚ;1���     6eĉ�������� 9��|��y��     ʚ;�� ��������x�<$��_�G��[^_]Ív �����������?H�������=0���w�  ��Mb������f�����?Í�&    �v �����W�Lb VS�8�p9��    �rK�@��7�i�   �?B ������i��7���������   ����������[^��_Í�&    [����?^_Í�&    ��&    ������UW1�V��S�@B ����Ձ��ɚ;v��	��K �������i� ʚ;)�[�~�ȉ� ʚ;���V^_]Í�&    ��&    ������VS�@B ����ց��� v��k�_k����iҀ�� )։ȉ򹀖� [��^Ít& �����=���w���Í�    ������f������Ui�@B ��W�@B ��VS1�щƁ��� v�Ⱥk�_k�����iÀ�� )����ʹ��� ���[^_]�f������i�@B �@B U���]�Í�&    �����U��]Í�    �����U�щ�V��S1ہ�?B v�Ⱥ��C�����i�@B )����ʹ@B ���[^]Ð�����S�щÁ�?B v�Ⱥ��C����i�@B )щ؉�[�@B ��Í�&    ��    �����WV��1�9�S���}L��1Ɂ��ɚ;w �؉�� ʚ;��É��[�N�V^_Ít& �����K ��	������i� ʚ;��)��Ív ��1�	�tŉ���1��ׁ��ɚ;v����K ��	�������i� ʚ;)ǉ؉�� ʚ;�����Љø�ɚ;)Љ��{�����&    ��&    �����S�Ӄ��Hd�    �T$��@�L$�   �D$���D$�؉$�������������T$d+    u��[�������t& �����S�   �Ã�d�    �D$1����������u)�$�T$��D$�S�C1��T$d+    u��[�f��������������t& �����������d�    �T$��$�Q�   �T$�������������T$d+    u���������t& �����S�   �Ã�d�    �D$1����������u)�$����C�D$�C1��T$d+    u��[Ív �������������t& �����V�ֹ   S�Ã�d�    �D$���$�C�D$���������u:�C�   ��$�C�D$�F�����������T$d+    u��[^Ít& �������������t& �����V�   ��S�Ӄ�d�    �D$1����������uV�$�S�   ����F�D$�F���������u1�$�F���F�D$�F1��T$d+    u��[^Í�&    f��������������t& �����V�ֹ   S�Ã��Sd�    �D$��T$��$�C�D$���D$���������uL�C�S�   �$�C�T$��D$���D$�F�����������T$d+    u��[^Í�&    ��������������t& �����V�   ��S�Ӄ�d�    �D$1����������uV�$�T$�   ��D$�V�S�F���������u0�$�T$�F�D$�V�F1��T$d+    u��[^Í�&    �������������t& �����V��1���9�S�}M���ɚ;w%� ʚ;�ʉ���щÉȺ�Mb������)ʉ�[^���	��K ������i� ʚ;)���&    ��1�1�	�t����ց��ɚ;v�ʻ�K ��	������i� ʚ;)щʉ� ʚ;���ɚ;�Љ�)�냍�&    f������S��������t��������������E�[Í�&    �t& ������S��� d�    �T$1�������$���T$��uE1҉��D$    ������Å�t�D$d+    u(����[Í�&    �v ��������ٍ�&    �������������t& �����WVS���8�pd�    �T$1҅�t8����������$�������uZ�\$��Mb�O���������)��������u7��u1��T$d+    u+��[^_Ít& ��   �    ���������tЍv �������������t& �����V��S�Ã���t"��@��x}�{�ɚ;wt�t]�9Ѻ   �|d����������u\��tc�$����  ��  w@��V�    �   �������� ���$t/���     ��t1����[^������t& ��������[^Ít& ���uۃ�[^Í�    �$������$�؍�&    �t& �����VS����Xd�    �T$1҅��|   ��������T$���T$��u~�A�������ur��@B wxi��  �T$��t4�   �ډ��������uJ��D$������T$d+    uG��[^Ít& 1ҍD$�܍�&    ���t�ڹ   ����������t������뺐1�1�뮸������������&    ��&    �����WS�Í{�ك����   )�d�    ��$�   1����   �    ǃ�       ��󫹀   ����������)  �$��D$�C���C�D$�C���C�D$�C���C�D$�C ���C$�D$�C(�D$�C0���C4�D$�C8���C<�D$ �C@���CD�D$$�CH���CL�D$(�CP���CT�D$,�CX���C\�D$0�C`���Cd�D$4�Ch���Cl�D$8�Cp�D$<�Cx���C|�D$@���   �����   �D$D���   �����   �D$H���   �����   �D$L���   �����   1���$�   d+    u�Ą   [_Í�    �������������t& �����W�   VS�Á�   d�    ��$�   1��t$T�����   �$�B�D$�B�D$�B�D$�B �D$�B(�D$�B0�D$�B8�D$�B@�D$ �BH�D$$�BP�D$(�BX�D$,�B`�D$0�Bh�D$4�Bp�D$8�Bx�D$<���   �D$@���   �D$D���   �D$H���   �D$L���   ��D$P�������������$�   d+    u
�Ą   [^_������V��S����   d�    ��$�   1���������Å�u��������É���������E؋�$�   d+    u���   ��[^������������� 뗍�&    �����U�щ�V��S1ہ��� v�Ⱥk�_k�����iÀ�� )����ʹ��� ���[^]Ð�����UWVS���\$4�|$(�D$�L$<L$0�t$8�l$,�ȉ|$��$߻�ɚ;�l$�9˹    щt$����}41ɍt&  6eĉ̓������ 9��|��y��     ʚ;�� ��������x�$�L$�\$�l$9щ�T$�$M�M�9։��|�T$�B�Љ2�z��[^_]Í�&    �1������������f������VS�    ��t�֍t& ���C������������u�1�[^Í�&    ��&    �����UWVS�    ��t)�Չύ�&    f��C�3���t$���������X��u�[1�^_]Í�&    ��    �����UWVS�    ��t$�Չύ�&    f��3�C��������������u�[1�^_]�f������VS�    ��t�֍t& ���C������������u�1�[^Í�&    ��&    �����VS�    ��t�֍t& ���C������������u�1�[^Í�&    ��&    �����UWVS�    ��t)�Չύ�&    f��C�3���t$���������X��u�[1�^_]Í�&    ��    �����UWVS�    ��t$�Չύ�&    f��3�C��������������u�[1�^_]�f������UWVS�    ��t$�Չύ�&    f��3�C��������������u�[1�^_]�f������VS�    ��t�֍t& ���C������������u�1�[^Í�&    ��&    �����VS�    ��t�֍t& ���C������������u�1�[^Í�&    ��&    �����UWVS���    ��t1�t$�|$ �$�L$�v �C�+��WV�L$�T$������XZ��u��1�[^_]Í�&    ��&    ������UWVS���    ��t1�t$�|$ �$�L$�v �C�+��WV�L$�T$������XZ��u��1�[^_]Í�&    ��&    ������UWVS�    ��t$�Չύ�&    f��3�C��������������u�[1�^_]�f������S��)Ӄ�>w�P�Ѓ�?�[Ít& ����  w��[���������?��@�Ív ���  v(���}  w8   ����	��?�� ����   뭍�    ��@[���������?�耉Ív ���� v(��� w@ �  ������?�� ���@  �b����v    ������?�� ���   �B����v ����� w   ������?��  ���  ���������w    ������?��  ���  �������x,��   >�����>G�   ������?��   �   ������Ѓ�?�����t& �����UW��j  V��S�����$���    ��������^��   u�ڸ@a  ���� ��    �<*��   u�<��������$�9^u�����[^_]Í�&    ��    U��W��V��S���^@d�    �E�1�������    �FD��tb�M�U�   �������t8�U�M�J0�`   f�J4�M�j<�B8    �xj SR�   j jV��������E�d+    u�e�[^_]�f����u���������v U��W��V��S���M�^@d�    �E�1�������    �FD��t�M�U�   �������tS�M�U�J0�`   �j<f�J4�B8    �x�O�}�H�    �x�H�M�H�M�j SR�   j jV��������E�d+    u�e�[^_]Ít& ����w������������&    ��&    �U��W��V��S���M�^@d�    �E�1�������    �FD��t�M�U�   �������tM�M�U�J0�`   f�J4�    �j<�B8    �x�H�O�}�H�M�xj SR�   j jV��������E�d+    u�e�[^_]Í�&    �v ����w������������&    ��&    �U��W��V��S���M�^@d�    �E�1�������    �FD��to�M�U�   �������tD�M�U�J0�`   f�J4�M�j<�B8    �x�}�H�M�xj SR�   j jV��������E�d+    u�e�[^_]Ív ���u���������v U��W��V��S���M��^@d�    �E�1�������    �FD�]����   �M�U�$   �������t^�M�U�J0�`   �j<f�J4�B8    �x�O�H�O�_�H�X�O�_�}��H�M�X�x j �u�R�$   j jV��������E�d+    u�e�[^_]�f�����n������������&    ��&    �U��W��V��S�Ã��S@d�    �E�1��U�������U��    �CD�U��tw�M�U�   �������tM�M�U�J0�`   f�J4��j<�B8    �p�7��p�x�I�H�M�j �u�R�   j jS��������E�d+    u�e�[^_]�f����u���������v U��W��V��S���^@d�    �E�1�������    �FD��tb�M�U�   �������t8�U�M�J0�`   f�J4�M�j<�B8    �xj SR�   j jV��������E�d+    u�e�[^_]�f����u���������v U��W��V��S���U��^@d�    �E�1�������    �FD�]����   �M�U�$   �������t`�M�U�]��J0�`   f�J4�M�j<�B8    �X�]�H�X�O�H�O�H��H�O�H �M�j �u�R�$   j jV��������E�d+    u�e�[^_]Ë���n������������&    ��&    �U��W��V��S���U�^@d�    �E�1�������    �FD��t�M�U�   �������tU�M�U�J0�`   f�J4�M�j<�B8    �U�H�M�P1҉H��t�W4�P�M�   j S�u�j jV��������E�d+    u�e�[^_]�f�����w������������&    ��&    �U��W��V��S���M�^@d�    �E�1�������    �FD��to�M�U�   �������t>�M�U�J0�`   f�J4�M�j<�B8    �x�}�xj SR�   j jV��������E�d+    u�e�[^_]Í�&    f����u���������v V��S�Ã�d�    �D$�C$��  u3�   �ډ��������t
�p��������D$d+    u��[^Ít& ��uȉ��������t����������&    f�W��V��S�Ӄ�d�    �D$�G$��  uP�   �����������t#�X�S�p�P�    �P�T$,�P��������D$d+    u ��[^_Í�&    ��u����������t����������&    f�W��V��S�Ӄ�d�    �D$�G$��  uH�   �����������t�    �X�P�S�p�P��������D$d+    u��[^_Í�    ��u����������t���������W��V��S�˃�d�    �D$�G$��  u@�   �����������t�T$,�p�X�P��������D$d+    u��[^_Í�    ��u����������t����������&    f�W��V��S�Ӄ�d�    �D$�G$��  uX�$   �������������t+�X�C�A�C�S�A�Q�C�S�q �A���Q������D$d+    u��[^_Ít& ���u����������t���������W��V��S�Ӄ�d�    �D$�G$��  uH�   �����������t�X�~�6�x�p�S�P��������D$d+    u ��[^_Í�&    ��u����������t���������V��S�Ã�d�    �D$�C$��  u3�   �ډ��������t
�p��������D$d+    u��[^Ít& ��uȉ��������t����������&    f�W��V��S�˃�d�    �D$�F$��  uX�$   ����������t/�x�t$,�|$0�p�x�S�P�S�P��P�S�P ��������D$d+    u��[^_Ív ��u����������t���������W��V��S�˃�d�    �D$�G$��  uP�   �����������t$�p�|$01ҋt$,�x�p��t�S4�P��������D$d+    u��[^_Í�    ��u����������t����������&    f�W��V��S�˃�d�    �D$�G$��  u8�   �����������t�p�X��������D$d+    u��[^_Ít& ���uÉ��������t���������V��S�Ë�x   �������t
[^Í�&    �vh��  S�������`  ��[^�����f�V��S�Ë�x   �������t
[^Í�&    �v�v�v�vhX? S�������`  ��[^�������&    f�V��S�Ë�x   �������t
[^Í�&    �v�v�v�vh�? S�������`  ��[^�������&    f�V��S�Ë�x   �������t
[^Í�&    �vh��  S�������`  ��[^�����f�W��VS�Ë�x   �������t	[^_Ít& ��~ ��Mb��������)�R�v�~��������)�R�v�v�v�vh�? S�������`  ��$[^_�����V��S�Ë�x   �������t
[^Í�&    �v�v�v�vh��  S�������`  ��[^�������&    f�V��S�Ë�x   �������t
[^Í�&    �Nh��  �C@��  ��  < ������V���FQ��%�� ��QP��+FPR�v�vh�? S�������`  ��([^�������&    �U��WVS�Ë�x   �������t[^_]Ív �k@�V�@�  ��������V� �  �ǉ������WP�vh0@ S�������`  ��[^_]�������&    �t& V��S�Ë�x   �������t
[^Í�&    �V �C@���  �����P�v�v�v�v�v�vhP@ S�������`  ��$[^�����f�V��S�Ë�x   �������t
[^Í�&    �V�C@�`�  �����P�vh�  S�������`  ��[^������1��������&    f�W1�VS�t$��1�W1�VWS�������[^_ÐW1�VS�t$��1�W1�VWS�������[^_ÐW1�V��1��t$�t$WV�������^_Ív W1�V��1�WV�����XZ^_Í�&    �t& �W1�V��1�WV�����XZ^_Í�&    �t& �W1�V��1�WV�����XZ^_Í�&    �t& �W1�V��1�WV�����XZ^_Í�&    �t& �������@������v 1��������&    f�W1�V��1��t$�t$WV�������^_Ív �����UWVS1ۃ��h�D$�����?�D$    �|$�x�D�t& )���x#�L$�(��L$9�IщT$�$�ڃ�9�vw���<$����D$��@  t`��\$1҉���?j �j ��@V�����������L$��9�w�j ��1҉�j �t$�������9�v��@   )���l�����&    �v �|$�\$�G�G �$���?9؉��G��[^_]Í�&    f������U��WVSd�    �    �<@��Mb�4�������i��  �    )�  ���   F�����)�9�[H�^_]��Ít& �������� ������    �    ��t
�    ��u �������s �������� �������&    ������ސ�����W��V��S��>�t& �C    �{������uSd�    	��C[^_Ít& d�    �    s�d�    �    ��t
�@�������d�    u������뢍�&    ��  8 룍�&    ��&    �����VS�ÉЋS����   ��΅�tr>�t& ��S���t�P����t�C    �"  �   [^Ít& d�    �    s�d�    �    ��t
�@�������d�    u������뚍�&    �S���L�`9K�{����P�@�n���1�[^Í�&    �t& �����WV1�S�Ã�d�    �D$�C��t%��������   �ǉ������$�Ɖ�������D$d+    u	����[^_�����������V��S��d�    �D$1����������9pt8�¹   �������Ƌ$��������D$d+    u����[^Í�&    f��������������t& �����W��VS��d�    �D$1�d�    �   t
�G t\�t& ����'�����;xt8�¹   ���"����$�Ɖ��������x"�D$d+    u!����[^_Ít& �$������묍t& ��������&    ��&    ������UWV�Ɖ�S�ӍT�`�|$�
���t�Y��S�F�S������? 	ЉC>�t& ;~xR[^_]Ít& ��Kd�    �    s�d�    �    ��t�jP�ډ������Xd�    u������믍�    �~�F�F >�t& [^_]Ív �Cu��~ t��F[^_]�������&    ������W��V��Sd�    �����>�t& �������>�t& d�    %���9�t6�=�    ��   ��&    �v d�    �ʉȁ�   �	�d�    9�u�[^_Ít& �d�    �    s�d�    �    ��t
�@�������d�    �p���������f�����&    �d�    �    �W���d�    �    ��t
�@�������d�    �0���������&����t& ��   PSWh�@ ��������*�����&    �v UWV��S����4d�    �D$0��������    ;F��   ��    �    �E9���   �u`�E�}1҉t$�L$�l$�<$1��v �Ë4$��?��s �t$�i�4�����t�K�    ����u��@����@  u��l$�E����uQ�} �O  �E��������E�E��&    �    9��R������������D$0d+    �!  ��4[^_]ÉE�������|$�t��E�E��&    ���x���u,�   f�������$�����������������E    ��tX��]>�t& ��S���t�P�S�C �C    ���"  �$u�������$�����7������������E    ��u��E���l$�b��������t& �d�    �    s�d�    �    ��t
�@�������d�    �a���������W����} ��������������f������S�@a  ������    �    ;Sx�������������j  �    �    ��;Sx	[�����v [Í�&    ��&    �����W��V�4R��MbS�0������i��  ��)��  �    ���)��)�9�[^H�_��Í�&    f������W��VS�R��Mb�4������i��  �    )����  ���   F�ډ�)�9�[H�^_��Í�    �����W��VSd�    �@��Mb�4������i��  �    )����  ���   F�ډ�)�9�[H�^_��������W��VSd�    �4@��Mb�1������i��  ��)��  �    ���)��)�9�[^H�_��Ív �����U��W�<R��MbVS�    �4�������i��  �    )�  ���   F�����)�9�[H�^_]��Í�&    �v �����W��V�5    S�R��Mb��<������i��  ��)��  �=    ��9�y�)�)Љ�[��^_������W��VSd�    �5    �@��Mb�1�<������i��  ��)��  �=    ��9�y�)�)Љ�[��^_Í�&    f������WVS�Ã�d�    �D$�C����   �K����   �@a  �4�    ���Ct��j  Ɖ������9�t �K   �������������C%  ��	��C�    �V��)х�~�N9�xD9�xX�N�ʋC�L$������t$�ډ��������T$�������X�D$d+    u#��[^_ÉF��뾍�&    f���    ��������&    ��&    �UWVS�Ã�$�$d�    �D$ 1��C����  ������  �C����  �D$   �C+$�~  �σ��|$t���k  �T$�������ǡ    �W��)х�~�O9���  9��   �O�C����  �l$��t�$;C�d  �G�L$�D$�$������D$�C��9��5  �t& �1ɉ��������D$��u������   �k���   ��  ������@a  ��   ��    �  Ɖ�9�tV��9_�a  �K   �����������������C%  ��F�C�    �V��)х�~�N9��1  9���  �N�$�|$��C�Vt
;T$�L  �$�L$������t$�ډ����q���X�T$��������D$ d+    ��  �D$��$[^_]Í�&    ��T$��������ǡ    �W��)х��   �O9�xX9�x4�O�D$�����D$    �����t& �������j  4�    �����f��D$�����D$    �y�����&    f��G�D$�����D$    �X�����&    ��D$�����D$    �;����v ��    �F�������&    �������¸@a  ��   �4�    �M�����j  �C�����&    �t$�L$�ډ��/���Z������&    f��$�C�D$   �������v �G�c����S�����&    f��G�L$�D$�$�;����D$�C��9��w����L$����t��$;Cx��D$   �8�����&    ��������&    f��������������t& ������   �A���������1��4����t& ������   �!����������P��u�P�   ������&    ��&    ��&    �����S�������t d�    ��&    f��C   �������u�[Í�&    ��&    �����Sd�    �������u �7��&    ���u�C   �������t���   t�[�������&    f�1�[�������&    �������    ��� �    �������    �����W��V��S�ø�� ������t$�����t$�������_Z�Å�u��u��� �������[^_Í�&    f��    �    ��t
�    ��u�������� �������[^_�f������밍�&    f������U��WV��S�@a  ������$�L$������    ������    �C  ��������{ �  �C�S��)�9���   ����   �4$�|$�C ������������9����sW�4$�|$9Ɖ��sH?B �� 1��Ɖх�t���C����Љ�����i�@B )��@B �������i�@B ��׉�ύe����[^_]Í�    �s�{ ug���������+$T$�Ѿ�����¿����@B 9и    ��G����C�>����v ��x�9��$����C�����t& ���	����C������)�@B �4$�|$���Ɖ��랍�    �e����������[����^_]Í�&    �v �����S�@a  �������    �D [�f������V�@a  S�á    d�    1��d�    �������1Љ�d�    ������������4�    ������    ;Fy�    ;��	  x
�   ������������d�    �   u�����[^�������    ����������[^�������&    ��������@a  �    �    �B���?�B1�f�B�    ��j  �    �P�����?�P1�f�P1�������UW��VS���D$d�    �D$1��=    �D$@a  ��  �D$�|$�4�    �   ��t$����������<�    ���������������    �O��)ȅ�~�G9��G  9��,  �G�D$�H���+  �D$�p``	  �$��&    ��o��tT��&    �>�t& ��S���t�P�C�"  �L$%  ��	�C�C�W�q����t$�ډ��������Z��u���94$u��D$�������������   �����d�    ����   �D$�	  �D$=@t  ������D$d+    uz��1�[^_]Ít& �d�    �    �D���d�    �    ��t
�@�������d�    ��������������D$�H��������W����������b��������f�f�f�f������U����W��V��S�Ӄ��EM]1҃} и    MUME�$9�����$L�L�9����}
������������ډ�[^_]Í�&    �t& ������S��&    �v �H �Q��t�t& ��Q��u��x$ u9At�Y9�uԋP 9�u�1�[ø   [�f������U��WV��S���E�T$�D$�E�D$��tj����ɍ��ˍD�8��tV�x9|$t}���W�O+T�XL�\�Љ�;D$L$}�D$�D$�T$��t>��t�   �������!Í��D�8��u��t$�|$1�1҉�9��M�M׃�[^_]À& t*�~4뷍t& ��D$�$������$�����j���뗍t& ��~(덍�&    �t& U�   ��]�����f�U�   ��]�����f�U1���]������t& ������U��WV��S���Ӊك�����ߋU�E�� ��I�Hˉ4$�։L$����	�t61ɉ�������������	�u�|$�4$1������ E�E��4$�|$�T$��1��$�ƅ�t��1���ǉ������y�؃� �ڍe�[^_]Í�&    �v �����W�|$����w�U��WVS�ÉЉʃ�4�Mċ��s�E��}Ћ{)��u���}�E؉U܅���   �C$��   �    1ҋ}Љ�9���u�MȉЋU�Lǋ}�9ΉM�E��M�E���   �s�K�E�   �E�    �ủM؉u܋u�}���;u�}�}�L}��u�Mu�9����}
����������u�}�C�E؉S�U����E�E�9ΉU��U�M�MΉƋE�9ȉ��}�E������E�����E�U�C�S�EЋUԍe�[^_]�g�_��E�    �E�    ���E�    �E�    �̍�    �E؋U��u��u������Y^�׉ƉыE�U�u��ȉ}ԉ�������e�ƋE��׋U����s{�E��u؉�s�u��}܉}�9Ɖ{�}ĉU�׉C�S�O����E��U� �����t& �����U�Չ�W��V��S�ÉЃ��{�ك����)����    ��(�D$1����C$    �    �<�    d    ��u=�������  ��t1��D$����C&�����C'���D�8�C ��[^_]Ív ��~ÉL$Uh�@ �����XZ�L$븍�&    �v ������P(�@(    ��t�������1�Ív �����UWVS�ø    ���T$�D$������k �S�K��    ��+u ��}$�D$�4$�|$����   �$�T$1�1�9��ЋT$ȋ$L�M��{& �E u\�D$D$9E t��[^_]Í�&    ;p ��H$}��H��uމX(�p �P$��tЃ�u˃���   [^_]�������&    �@u�;p,��x0}�;p ��x$�X4�p,�P0}��|$ t��u�����F�����&    f������V��S��>�t& ��K�   ��	B�C���F$�����[^��Ít& �d�    �    s�d�    �    ��t
�@�������d�    u������럍�&    ��&    ������U��WVS�ËH�@t*�@(    ��1�h���j������ǉ�XZ�e����[^_]��@4    ���   1�h���j��u����K�C(    �C,�ǉ։؉S0��1�h���j��N������9��}�C4��C(�e��[^_]Éǉ�돍�&    �t& �����V��S���@�����t�K$�s 1�1�	�t�K�C �S$��t��t
[^Í�&    [�   ^������t& �����UWVS���@$�:�l$�K$�u[^_]Í�&    �֍B���������u�N�������!G��t�9_(u�[��^�   _]�R���f������U1�WVS�Ã��������u����[^_]Ít& ����������t& ��s ���� t�������ǋC 9�uս����;Xt1��C$u� ����������[^_]Í�&    �>�t& �.�    �$������$�    1�9Չ�   ��1�P��������C Z멍t& d�    �    s�d�    �    ��t
�@�������d�    u������댍�&    �����S�É��������x[Ív ���t& �����UW��VS��(�T$�D$@�L$d�    �T$$1�#G�P  ����ȉD$���l�8���6  ���]�T$���4�    �$�D$D�XT�\�t$�D$�T$ ����  �L7L�L$�9��    �E�$�E9\�H��  �$�D�H    �]����  �D$�T$ ;C��C�  >�t& �$�\�H�E�D$�E��C$�q8�L$�C$ �t"�D$���������u�L$�   �I<����!F�T$<���s�����>�t& ���������>�t& ����������?����C$�5���1ɉ�������%����t& �d�    �    �S���d�    �    ��t
�@�������d�    �,���������"����t& d�    �    �w���d�    �    ��t
�@�������d�    �P���������F����t& d�    �    �!���d�    �    ��t�@�L$�������d�    �����������������&    ��T$��t(�   �������!��l�8�T$���������    �D$$d+    u��([^_]Ív �����������&    �t& ������UWVS�    ������    �؍��   ��������   ���   W�ƍC��������   ���   �щ؉�   ���   ��$  ���   ��H  ���   ��L  ���   ��p  ��t  h�   V�����c��K1��C4    ��h������   j������������Ѓ�	�t�C4�   �i������[^_]�������&    �t& ������VS�    ������    �Cu�    ����   �؍��   ��������   ���   V�C��������   ���   ��   ���   ��$  ���   ��H  ���   ��L  ���   ��p  ��t  X���Ct1�<����K�C �S$��t"��u�   �������[^������t& �������[^�����f�[^Í�&    ��    �����W��V��S��>�t& �����[^_�_�����&    d�    �    s�d�    �    ��t�@Q�щ������Xd�    u������뮍�&    �t& �����W��V��S��>�t& �؉���������C�� d�    �C([^_Í�    d�    �    s�d�    �    ��t�@Q�щ������Xd�    u������랍�&    �t& �����U��WVS�Ã��K ���� tN��$������$�D$;K u/�s�{�A�����)ƋC ׋T$� �����������[^_]Ív �������룍v �����UW��VS��(�$�D$<�T$�T$@�D$�D$D�T$�$�D$�����L$���& ��8���  �s ���� ��  ������;s ��  �>�t$�    �D$$�$�T$������.�T$�    9��{$��9](��!T$��   ��  �\$�t$1�1���9�ډ��M�M��D$��   �\$�t$9���M�M��$�\$�t$�X�\$�p�t$��9�؉���}
����������4$�|$ �F�V��   �t$�L$�$���������   ������$�T$$�@ � ��([^_]������t& �D$�@������L$�֋T$�É���9�T$��ȉL$M�M�9��}0���������������D$�����D$�������������&    �D$1�1�9l$�M|$Ml$��9��L�L��������&    �D$�l$�    �H���L$������L$<�    �|$�x  ����  ���t$�$���D$���D$ ��8�D$�r�C ;X��   �C �� �������D$ �L �A8�$�����9l$�y  �$�|$�C�S�I8+D�XT�\;A ��A$�S  ��������������s �l$�D$�D$9�u�9l$t*�D$�S�L� �C+AXQ\�I8;A ��A$|Ǎ�&    f��t$�$�L$�T$�$��������%����$�   �����������&    ��������������&    �>�t& �    �T$ ��������ʋT$ �    9��T$����!���P�D$�T$�����X������&    �|$������&    d�    �    s$d�    �    ��t�@�$�����d�    tG�D$�(�g���f����C�����&    �l$�$�k ������������    �L$<�    ���������벍�&    ��&    ������S�H�X+HRX�PSQ�H�������[Í�&    �t& �����UWVS�    �� �D$d�    �D$1���d    �@u�5    ����  �D$    �����������؉D$��&    �v ��    �������;    �N  �\$�    �؍��   ������s���   ���   �D$�CW��������   ���   ��   ���   ��$  ���   ��H  ���   ��L  ���   ��p  ��t  Y;s��   �S����   �t$#s��   �   �������!��4��t�8��t~�{ ���T$�ډ<$�{$�|$�^���4ʋK�[+NX^\��;$|$��   �~@~�|$ u;J,��z0��   ��t#�   �������!؍��t�8��u���&    f��ӋT$�������������    �   �����j1ɺ�� �D$������   �����Zd�    ��t1�����������D$d+    u!�� [^_]Í�&    �����l$���������������&    �v �������   �����������    �@� �    �������    �����1������t& �����U��WVS�    ��������    ��������E��C�t'����������U���������e����[^_]Í�&    �uD�K�C4    1҉�h���j����   ������s4Y_�s(�KR1�P����������Ɖ�XZ랍v ���������1���f������U��WV��S�    ��������    ��������K�E���t\�����������t-�KR��P�؃��\����Ɖ�XZ�U���������e����[^_]ËKh������j����   �'���Y_붍v ���������뾍t& �����UWV��S�    �� ������    �C�7  �C�؍��   �F�����F�����������   ���   �ōC�|$�T$�L$�D$W������D$�Ɖ׋��   �T$ ���   ��   ���   ��$  ���   ��H  ���   ��L  ���   ��p  ��t  X�$   �C���C �����C$�����;s,K0�S|��
�C,�����C�   �C0��������j�����U�����������c��C �Ɖ׉؉S$�������1ɉ��������ZY���  ��������t$�L$�T$�ŋD$�����f�C�Ɖ׋��   ���   ��   ���   ��$  ���   ��H  ���   ��L  ���   ��p  ��t  X�,$����f�C��؀K���������+D$T$�$�T$;Cv�C�$� ���\$��9ȸ    ؉�|0$T$�   ������=�    �d�  �� [^_]Í�&    �v  ���� �͍�    �c��� [^_]Ít& ��&    ��&    �����UWV�    S�������    ��Ct[^_]Í�&    f���$  �������������   �؍��   ��������   ���   W�ƍC������ŋ��   �׋��   ;k,��   ���   ��$  ���   ��H  ���   ��L  ���   ��p  ��C0��t  Y|�K�   �C,�����C0��������j��؉�V�a���X��Z��[^_]�����f������4�    ����������  �N�       �����1�[^_]������&    f���������ЋQ��t��u,�Q�������u�����Í�&    ��Q�������t��������&    �t& ������UWVS�˃�@�T$�$d�    �D$<1�d�    �PH��h  ��l  ���r  �xHc�h  �|$�l$>�t& �t$�T$T�ى��j����$�L$d�    �D$8��ǉT$$��͉<$1��l$�ՉL$(1�9�͋l$�D$,�� M�M��T$��9�Ћ$L�L��T$9����}�$�����D$����$�T$�D$���T$ ���Y  =����u:����   �L$0�\$ d�    �I��P  ǂ<  0[  ��@  �L$��L  �T$<d+    ��   ��@[^_]Í�    d�    �    �����d�    �    �t$��t�@S��L$X�����Xd�    ��������������f������늍�&    f��D$    1�1��D$    ���������f������WVS���Xd�    �T$1ҋ�����������   �T$�4$�������xZ�L$���ɚ;wNd�=    1��ۉ�H  �����D  �}�%9�   �|<� ʚ;i� ʚ;���1���j�   �����Z�T$d+    u$��[^_Í�    ����������͍t& �������������t& ������    S�    �Q8��x  �t& ��
��(�B�    �B�    �B�    9�u�A1��a��A    �A(    �A4    �A �����A$����A,�����A0���[Í�    ������    ��  UWVS�Ã�������   �������<�    �    ��|$�����4�    ���t$���������������8x  ���D$�_�t$�G+t$�D$�����   �<$�N�t& >�t& �C$�} �C$�t�D$���������u�M�   ����!G�s ��1ɉ�������$�X��tV�C 9Xu��t& d�    �    s�d�    �    ��t
�@�������d�    �w���������m�����&    ��ǃ�(��(9|$�)����|$���Gu9�O�G4    1҉$���   h���j�����Y[�������	�t
�G41��	����D$�    ������D$������������    �Dt�    ������    t���t�������/� �   �����1���[^_]Í�&    f�f�f�f�f�f�f�U��WVS������t& �@� �ȉL$��k�4��h� ��l� ��D� ��L� ��H� �$��P� �L$��T� ��D� �T$�L$������+D$T$��!�!�������|$�|$�ЉƸ    ��!�!�	��ǋsD�EЋC�t$�|$�{ �[�|$�=@� �L$9��G����ƋD$�������D$��T$��1����� E�E�$T$�e�[^_]Í�&    ��&    ������U��WVS������<� �  �$�T$�5@u ��   u#��_�@u 9�u�$�T$�ڍe�[^_]�f���ˍ�&    �t& ������U��WV�@u ��u#�5�u �=�u �@u 9�u߉���^_]Ít& ���΍t& �����UWV��S�@u ��uB�\u �du �`u �=�u �-�u ������ �>En�F�@u 9�u�[^_]Ít& �믍t& ������    S��u1�@u ��u�Xu �\u �@u 9�u���[Ít& ���Ӎt& �ˍ�&    �t& ������U��WVS������5    �� �  ��u~�@u �um�lu �3�{�hu �L$�\u �4$�5`u �|$�=du �T$�@u 9�u��D$�T$$T$��1����� E�E����e�[^_]Í�    �농t& �{�����&    f������U�    ���u��u 1�]Í�    ��u 1�]Ít& �������    �Du ��u��t�������1�Í�&    ��    �����VS�ø    ������ډƸ0t ������Du �   �ø0t �������    �������[^Í�&    ������VS�ø    ������ډƸ0t �������ø    �������[^Í�&    �t& ������UWVS�Ӄ��$�D$4�L$�D$�ǋWd�    �D$1��Q�t$4�t$4������։ǃ����������   �$Dº@   �������)���   ��D���9���   ���������   �$D��@   �L$�����)D$�D$����   ��DŃ�9���   �,$���ى��������4$�<$�,$�Ɖ�l$���$�t$0�t$0����������L$����QX1�Z�T$d+    u`��[^_]Ð��Dº@   ��!�����)��#�����D���!�����t& ��D$DŃ�!�Q�����D�!����f�������������t& �����UWVS�Ã��st�{xd�    �D$1��C|�ރ� �ؙ��R����P�D$������D$�|$�}�%�t$�����   9º   ���   ���   � ʚ;i� ʚ;���1���1�1�	���   �D$$�t$$�|$(�Ct�D$(�ރ� �Cx�D$,�߉��C|�ؙR��P�D$������D$�L$�t$Z�}�%9º   _��|   � ʚ;i� ʚ;1�����׸ ʚ;���   ���   ���   �����   ���   �D$d+    u?��[^_]Ív ���������1�1�	��>���f��5�����&    f������������������&    ��&    ������UWVS���@u ����   �-\u �`u �du �5�u �l$�-�u �L$�=�u �\$��u �,$��u �-@u 9�u���L$�,$߉t$�\$�t$�|$������ E�݉���SU�T$�L$������� [^_]Í�&    ��U�����&    f������S���ӍB���S�Q�P�Q�P�Q�P�Q�P�Q�P�Q�P�Q�P�Q �P �Q$�P$�Q(�P(�Q,�P,�Q0�P0��K�K8�H�K<�H�K@�H�KD�H�KH�H�KL�H�KP�H�KT�H �KX�H$�K\�H(�K`�H,�Kd�@0�Ch[������U��S�Ã���� ��u d�    �D$��u +�u �u �щ�������$��D$�C�D$�C�D$d+    u�]����������&    f������U�    ���t�8t �<t ]�f�d�    ]������t& �����UWVS�Ã��Du ��u �Du �S�D$�C�Lu �Hu �Du �������xu �Pu �Tu �s�{��u �@B �5|u 1��=�u ��u �K1������� E�EƋs�ǉ$������T$1���1�����t��1����ǉȹ   �l$��1���9ȉ��C�C�v �ȉ5v �{�c���v ��<$�v )ǉ�Չ=v �-v �s�c�l$�v � v ��tZ�K�`u �du �5�u �=�u +M��   ��1����� E�E�����`u 1��� E�E��du �5�u �=�u �C�T$�    1��,v     �\u ��u �$�0v     +K������ �4v E�EƉ(v �$v �C�8v     �Xu ��u �<v     ��[^_]Í�&    f���1������� E�E�����`u 1��� E�E��du �5�u �=�u �?���f������U��WVS�������&    ��� �ȉL$��k�4���� ���� ���� ���� ���� �$���� �L$���� ���� �T$�L$������+D$T$��!�!�������|$�|$�ЉƸ    ��!�!�	��ǋsD�EЋC�t$�|$�{ �[�|$�=�� �L$9��G����ƋD$�������D$��T$��1����� E�E�$T$�e�[^_]Í�&    U��WVS������t& ��� �ȉL$��k�4���� ���� ���� ���� ���� �$���� �L$���� ���� �T$�L$������+D$T$��!�!�������|$�|$�ЉƸ    ��!�!�	��ǋsD�EЋC�t$�|$�{ �[�|$�=�� �L$9��G����ƋD$�������D$��T$��1����� E�E�$T$�e�[^_]Í�&    ��&    �U��������u ]�u Í�&    �v �����UWV��S�Ã����/