		mem &= ~ALT_DATA;
		mem |= (data & ALT_DATA);
	}

	if (flag)
		mem |= ALT_AD_RG;
	else
		mem &= ~ALT_AD_RG;

	mem &= ~ALT_CS;
	if (read)
		mem = (mem & ~ALT_RD) | ALT_WR;
	else
		mem = (mem & ~ALT_WR) | ALT_RD;

	cx_write(MC417_RWD, mem);  /* start RW cycle */

	for (;;) {
		mem = cx_read(MC417_RWD);
		if ((mem & ALT_RDY) == 0)
			break;
		if (time_after(jiffies, timeout))
			break;
		udelay(1);
	}

	cx_set(MC417_RWD, ALT_RD | ALT_WR | ALT_CS);
	if (read)
		return mem & ALT_DATA;

	return 0;
};

static int dib7070_tuner_reset(struct dvb_frontend *fe, int onoff)
{
	struct dib7000p_ops *dib7000p_ops = fe->sec_priv;

	return dib7000p_ops->set_gpio(fe, 8, 0, !onoff);
}

static int dib7070_tuner_sleep(struct dvb_frontend *fe, int onoff)
{
	return 0;
}

static struct dib0070_config dib7070p_dib0070_config = {
	.i2c_address = DEFAULT_DIB0070_I2C_ADDRESS,
	.reset = dib7070_tuner_reset,
	.sleep = dib7070_tuner_sleep,
	.clock_khz = 12000,
	.freq_offset_khz_vhf = 550,
	/* .flip_chip = 1, */
};

/* DIB7070 generic */
static struct dibx000_agc_config dib7070_agc_config = {
	.band_caps = BAND_UHF | BAND_VHF | BAND_LBAND | BAND_SBAND,

	/*
	 * P_agc_use_sd_mod1=0, P_agc_use_sd_mod2=0, P_agc_freq_pwm_div=5,
	 * P_agc_inv_pwm1=0, P_agc_inv_pwm2=0, P_agc_inh_dc_rv_est=0,
	 * P_agc_time_est=3, P_agc_freeze=0, P_agc_nb_est=5, P_agc_write=0
	 */
	.setup = (0 << 15) | (0 << 14) | (5 << 11) | (0 << 10) | (0 << 9) |
		 (0 << 8) | (3 << 5) | (0 << 4) | (5 << 1) | (0 << 0),
	.inv_gain = 600,
	.time_stabiliz = 10,
	.alpha_level = 0,
	.thlock = 118,
	.wbd_inv = 0,
	.wbd_ref = 3530,
	.wbd_sel = 1,
	.wbd_alpha = 5,
	.agc1_max = 65535,
	.agc1_min = 0,
	.agc2_max = 65535,
	.agc2_min = 0,
	.agc1_pt1 = 0,
	.agc1_pt2 = 40,
	.agc1_pt3 = 183,
	.agc1_slope1 = 206,
	.agc1_slope2 = 255,
	.agc2_pt1 = 72,
	.agc2_pt2 = 152,
	.agc2_slope1 = 88,
	.agc2_slope2 = 90,
	.alpha_mant = 17,
	.alpha_exp = 27,
	.beta_mant = 23,
	.beta_exp = 51,
	.perform_agc_softsplit = 0,
};

static struct dibx000_bandwidth_config dib7070_bw_config_12_mhz = {
	.internal = 60000,
	.sampling = 15000,
	.pll_prediv = 1,
	.pll_ratio = 20,
	.pll_range = 3,
	.pll_reset = 1,
	.pll_bypass = 0,
	.enable_refdiv = 0,
	.bypclk_div = 0,
	.IO_CLK_en_core = 1,
	.ADClkSrc = 1,
	.modulo = 2,
	/* refsel, sel, freq_15k */
	.sad_cfg = (3 << 14) | (1 << 12) | (524 << 0),
	.ifreq = (0 << 25) | 0,
	.timf = 20452225,
	.xtal_hz = 12000000,
};

static struct dib7000p_config dib7070p_dib7000p_config = {
	/* .output_mode = OUTMODE_MPEG2_FIFO, */
	.output_mode = OUTMODE_MPEG2_SERIAL,
	/* .output_mode = OUTMODE_MPEG2_PAR_GATED_CLK, */
	.output_mpeg2_in_188_bytes = 1,

	.agc_config_count = 1,
	.agc = &dib7070_agc_config,
	.bw  = &dib7070_bw_config_12_mhz,
	.tuner_is_baseband = 1,
	.spur_protect = 1,

	.gpio_dir = 0xfcef, /* DIB7000P_GPIO_DEFAULT_DIRECTIONS, */
	.gpio_val = 0x0110, /* DIB7000P_GPIO_DEFAULT_VALUES, */
	.gpio_pwm_pos = DIB7000P_GPIO_DEFAULT_PWM_POS,

	.hostbus_diversity = 1,
};

static int dvb_register_ci_mac(struct cx23885_tsport *port)
{
	struct cx23885_dev *dev = port->dev;
	struct i2c_client *client_ci = NULL;
	struct vb2_dvb_frontend *fe0;

	fe0 = vb2_dvb_get_frontend(&port->frontends, 1);
	if (!fe0)
		return -EINVAL;

	switch (dev->board) {
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI: {
		static struct netup_card_info cinfo;

		netup_get_card_info(&dev->i2c_bus[0].i2c_adap, &cinfo);
		memcpy(port->frontends.adapter.proposed_mac,
				cinfo.port[port->nr - 1].mac, 6);
		pr_info("NetUP Dual DVB-S2 CI card port%d MAC=%pM\n",
			port->nr, port->frontends.adapter.proposed_mac);

		netup_ci_init(port);
		return 0;
		}
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF: {
		struct altera_ci_config netup_ci_cfg = {
			.dev = dev,/* magic number to identify*/
			.adapter = &port->frontends.adapter,/* for CI */
			.demux = &fe0->dvb.demux,/* for hw pid filter */
			.fpga_rw = netup_altera_fpga_rw,
		};

		altera_ci_init(&netup_ci_cfg, port->nr);
		return 0;
		}
	case CX23885_BOARD_TEVII_S470: {
		u8 eeprom[256]; /* 24C02 i2c eeprom */

		if (port->nr != 1)
			return 0;

		/* Read entire EEPROM */
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client, eeprom, sizeof(eeprom));
		pr_info("TeVii S470 MAC= %pM\n", eeprom + 0xa0);
		memcpy(port->frontends.adapter.proposed_mac, eeprom + 0xa0, 6);
		return 0;
		}
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982: {
		u8 eeprom[256]; /* 24C02 i2c eeprom */

		if (port->nr > 2)
			return 0;

		/* Read entire EEPROM */
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client, eeprom,
				sizeof(eeprom));
		pr_info("%s port %d MAC address: %pM\n",
			cx23885_boards[dev->board].name, port->nr,
			eeprom + 0xc0 + (port->nr-1) * 8);
		memcpy(port->frontends.adapter.proposed_mac, eeprom + 0xc0 +
			(port->nr-1) * 8, 6);
		return 0;
		}
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_TT_CT2_4500_CI: {
		u8 eeprom[256]; /* 24C02 i2c eeprom */
		struct sp2_config sp2_config;
		struct i2c_board_info info;
		struct cx23885_i2c *i2c_bus = &dev->i2c_bus[0];

		/* attach CI */
		memset(&sp2_config, 0, sizeof(sp2_config));
		sp2_config.dvb_adap = &port->frontends.adapter;
		sp2_config.priv = port;
		sp2_config.ci_control = cx23885_sp2_ci_ctrl;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "sp2", I2C_NAME_SIZE);
		info.addr = 0x40;
		info.platform_data = &sp2_config;
		request_module(info.type);
		client_ci = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_ci))
			return -ENODEV;
		if (!try_module_get(client_ci->dev.driver->owner)) {
			i2c_unregister_device(client_ci);
			return -ENODEV;
		}
		port->i2c_client_ci = client_ci;

		if (port->nr != 1)
			return 0;

		/* Read entire EEPROM */
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client, eeprom,
				sizeof(eeprom));
		pr_info("%s MAC address: %pM\n",
			cx23885_boards[dev->board].name, eeprom + 0xc0);
		memcpy(port->frontends.adapter.proposed_mac, eeprom + 0xc0, 6);
		return 0;
		}
	}
	return 0;
}

static int dvb_register(struct cx23885_tsport *port)
{
	struct dib7000p_ops dib7000p_ops;
	struct cx23885_dev *dev = port->dev;
	struct cx23885_i2c *i2c_bus = NULL, *i2c_bus2 = NULL;
	struct vb2_dvb_frontend *fe0, *fe1 = NULL;
	struct si2168_config si2168_config;
	struct si2165_platform_data si2165_pdata;
	struct si2157_config si2157_config;
	struct ts2020_config ts2020_config;
	struct m88ds3103_platform_data m88ds3103_pdata;
	struct m88rs6000t_config m88rs6000t_config = {};
	struct a8293_platform_data a8293_pdata = {};
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct i2c_client *client_demod = NULL, *client_tuner = NULL;
	struct i2c_client *client_sec = NULL;
	int (*p_set_voltage)(struct dvb_frontend *fe,
			     enum fe_sec_voltage voltage) = NULL;
	int mfe_shared = 0; /* bus not shared by default */
	int ret;

	/* Get the first frontend */
	fe0 = vb2_dvb_get_frontend(&port->frontends, 1);
	if (!fe0)
		return -EINVAL;

	/* init struct vb2_dvb */
	fe0->dvb.name = dev->name;

	/* multi-frontend gate control is undefined or defaults to fe0 */
	port->frontends.gate = 0;

	/* Sets the gate control callback to be used by i2c command calls */
	port->gate_ctrl = cx23885_dvb_gate_ctrl;

	/* init frontend */
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1409_attach,
						&hauppauge_generic_config,
						&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(mt2131_attach, fe0->dvb.frontend,
			   &i2c_bus->i2c_adap,
			   &hauppauge_generic_tunerconfig, 0);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(lgdt3305_attach,
					       &hauppauge_lgdt3305_config,
					       &i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[1].i2c_adap,
			   &hauppauge_hvr127x_config);
		if (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1275)
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
  � ������t}��th��������D$�D$��1�1�����������    ���)Ƌ�l  ��i������������7  �D$�0�x�D$��[^_]Í�&    �v �=    u���&    U�    �؋   ��c  ��   �������t$$R��P��������D$�����U�������������돍t& ��;����D$���  �|$��$����    �����)�1���i����������Ƌ�l  �ׅ��2����h�P��	�tJ�$�T$��    �$��X��9��r����)��;D$�����   �$ �$�h�P��T$	�uþ���������������t& �h�P��	�t�$�T$�t& ��$��X��9��r����)��;D$���r.�$ �$�h�X��\$	�uǾ����������i�����&    ��$+px�S���f��D$    �M����v �����UWVS�ˉу��t$$�|$(�T$�ډD$�D$,��   �����������   �D$��l  ��tg�h�P��	�tD�$�T$�v �$�H�X��9��r����)��;D$���r%�$ �$�h�X��\$	�uƹ����1��-�t& ��$px����������=��� wB��    ��D$���ʋL$��[^_]������t& ���������D$��l  ���A����1���������UWV��S�Ã��L$�L$(������D$��tp� �$�������`�  ����   �|$���������1��������$����    �����)�1���i����������Ƌ�l  �ׅ�u�D$�0�x�D$��[^_]Í�&    �h�P��	�tE�$�T$�t& ��$��X��9��r����)��;D$���r>�$ �$�h�X��\$	�uǾ���������뇍v �=    �-����`�  ��    �$+px�^���f������W��V�Ɖ�S����������ډ�[^_�������&    ��    �����UWVS���D$�T$�L$����   �D$    ��&    ��D$�p�x�D$��l  ����   �h�P��	�tB�$�T$��$�H�X��9��r����)��;D$���rE�$ �$�h�P��T$	�uƋD$�D$�\$������D$9\$�v�����[^_]Í�    �$px�D$���  ��t�;0��Xr�;p��Xs��t$,�D$����p�D$�����XZ�f������UWVS���D$�T$�L$����   �D$    ��&    ��D$�p�x�D$��l  ����   �h�P��	�tB�$�T$��$�H�X��9��r����)��;D$���rE�$ �$�h�P��T$	�uƋD$�D$�\$������D$9\$�v�����[^_]Í�    �$px�D$���  ��t�;0��Xr�;p��Xs��t$,�D$����p�D$�����XZ�