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
lecðG®Ñ 'd{ô<iø­…½J\i:-9eŸW­`ÖŠãÕ_rvÛõfÓ3VûˆÞn†	ãKåoÙ5ÇY§£S ‡I·T_Q¦Vï€DlÌcqT8ƒJ¿bgÐpóÏšŸå~´¥ª”£qxXùø/ÿbÒñ-¬¸Ôi“Z¯4l†Zí›Ü¨ˆðŽQÝù½–žA¿Ví.fSªÉ:Ñ«É-™½…‚_ëU¬0Ž³ZÖàúTµû¤ì9“ó—¿.†åŠì6”ð^B\…æwžüé3wD    S‹p‰Ã‰ðèüÿÿÿ‰Â‹C	F0‰ðèüÿÿÿ1À[^Ã´&    t& èüÿÿÿ‰Â¸   V•    S‹p‰Ã‰ðèüÿÿÿ‰Â‹C÷Ð!F0‰ðèüÿÿÿ1À[^Ã´&    fèüÿÿÿU¹   WVS‰Ëƒì…    €{ …á   ‹s‹{ÆCœ]ú1Àº   ð±@º …J  ‰$èüÿÿÿ‹$…    ‹˜   ¨„²   ¸¹ èüÿÿÿ‰ðèüÿÿÿ‹F	ø‰F¸¹ èüÿÿÿ‹F(‰Â	ú‰V(…ø”À¶À¹ £¹ ‹‹FÂÿÿÿ?9Âˆ‡   ‹‹ì  ‘ÿÿÿ?9Âxg¡@¹ ‰ƒø  ·X¹ f‰ƒü  ‰ø#Fuw‰ðèüÿÿÿå   Æ@º  uðƒD$ü ƒÄ[^_]Ãv ûëìt& ¸   èl4  é?ÿÿÿ´&    fÿÿÿ?‰ƒì  ëŒv ‹‹ì  ÆC‹F‘ÿÿÿ?9Â‰nÿÿÿëÓv œXúÆƒ    ‰òÆƒœ    ‹NP‰øè0§ÿÿXéjÿÿÿ´&    v ‰Â¸@º èüÿÿÿ¹   é þÿÿ´&    v èüÿÿÿUWVS»   …    ‹ƒÀ  ‹{è]–ÿÿ‹S‹CÆC	 ¹   èù”ÿÿd¡    è®µÿÿ‹sœ]ú1Àº   ð±@º uw‰øèüÿÿÿ‰Â¡@¹ ‰ƒð  ·X¹ f‰ƒô  ‰ð#Gu.÷Ö#w‰ø‰wèüÿÿÿå   Æ@º  tûÆC [^_]Ã¶    ‹OR‰ð‰úèC¦ÿÿ‰øèüÿÿÿ‰ÂXë¹´&    ‰Â¸@º èüÿÿÿéxÿÿÿ´&    ´&    èüÿÿÿU‰ÁWVS»   ‰Þƒì4    ‰D$d¡    ‰D$1À¶Fx¨…   ‹F@…À„  ¸¹ èüÿÿÿ‰D$‹F‹@#F…Ð  ‰ðè©­ÿÿèüÿÿÿ…    ¶Cx‹k¨tƒÌ   èüÿÿÿ‹=    ¶CxS@‰$¨…¯  ‰è~@èüÿÿÿ¶Fx‰øèüÿÿÿ„À„º   ‹U‰øèüÿÿÿ‰ò‰èè?‰ÿÿ„À„Ÿ   ‹$‰úèüÿÿÿ¸¹ èüÿÿÿÇ$   ‰øèüÿÿÿ‹C@‹Sd…Ò”Â…À”À8Â…  ¶Cx¨…á   ¶Cx¨…Å   ‹T$‰èèüÿÿÿ‹$…À…ü   ‹Fd…Àtx€=”    „Ÿ  v ‹D$d+    …Î  ƒÄ[^_]Ã´&    ¶Cx‹$èüÿÿÿ„ÀuH‹$‰úèüÿÿÿ¸¹ èüÿÿÿ¶Cx‹$èüÿÿÿ„À…ð   Ç$    é8ÿÿÿt& ‹F@…À…}ÿÿÿë‹v ‹U‹$èüÿÿÿ‰Ú‰èè,ˆÿÿ„À…íþÿÿëšfƒÌ   èüÿÿÿé+ÿÿÿ‰èèüÿÿÿ‹ƒÈ   …À„«   €=4$   …ž   ‹L$‰Øº   èåÿÿ‹$…À„ÿÿÿènÿÿéúþÿÿ´&    f¶Cx‰Øè•ƒÿÿ¶Cx¨u¶Cx“D  1ÉD$èüÿÿÿ‹$T$èüÿÿÿ‰»P  ƒ@  èüÿÿÿéþÿÿt& ‹U‹$èüÿÿÿ‰Ú‰èè\‡ÿÿ¶À‰$é5þÿÿ¶Cx¨u÷D$   „`þÿÿûéZþÿÿt& ‹T$ƒÌ   èüÿÿÿéBþÿÿ‰øÆ”   èüÿÿÿ‰Â‹FdRPÿt$h„2 èüÿÿÿƒÄé7þÿÿ´&    éäýÿÿé)ýÿÿèüÿÿÿt& èüÿÿÿ¡    ƒøu!èüÿÿÿ	Âu(èüÿÿÿÇ       éüÿÿÿt& èüÿÿÿ	ÂtÝt& ëÔ´&    t& èüÿÿÿWVSƒìd¡    ‰D$¡    €=àr  t-9Ür …™   ‹D$d+    …   ƒÄ[^_Ã´&    f‰Â£Ür ÁêÆàr ƒÂƒ= ´ ÿtCƒ=´ ÿt2‹´ ƒúÿ„á»  ‰ s ‹€º ƒú…´»  ƒø té´»  t& ‰´ ëÆƒ=´ ÿ‰ ´ u·ëç´&    é`ÿÿÿèüÿÿÿ´&    t& èüÿÿÿ¡    SƒøHÇ       ºÐ  ¡    ðƒD$ü ‹@¹ 1ÉƒãtðƒD$ü ‹ä¹ )È)Ð÷ÐÁè‰Á‰È[Ã¶    =,  ~Ç    ,  º|’  ë¯iÐè  ¸Ð  Áú9ÂBÐëš´&    t& èüÿÿÿ¡    …Àu
Ç       Ã´&    èüÿÿÿƒ=    tÃÇ        Ãt& èüÿÿÿ¡    ƒø!Ç       ¸¸  ‹    Ð£ô¹ Ãt& =,  ~!Ç    ,  ¸à“ ‹    Ð£ô¹ Ãt& iÀè  ‹    Ð£ô¹ Ãt& èüÿÿÿ£ør ’ÀÃèüÿÿÿd¡    ‹ˆd  d  9Ñu‹\  …ÒuKÃ´&    v Ç€\     Æ€`  d‹    ƒª\  u
‹‚`  …Àu!d¡    éÚ®ÿÿ´&    v Ç€\     ëÅ‰ÐèM¯ÿÿëÖfffffèüÿÿÿÇ     ‰@Ç@    Ã´&    v èüÿÿÿ‹H‰‹H‰PQ‰PÃ´&    fèüÿÿÿVS‰Ã‰Ð‹‰Þ‰…Òt‹p‰s‹P‰S…ÉtÇ    [‰^‰HÇ@   Ã¶    [^Ç     ‰@Ç@    Ã´&    ¶    èüÿÿÿ‹…Òtƒh‹
‰…Ét	‰ÐÃ¶    ‰@‰ÐÃ´&    v èüÿÿÿ‹P(‹H,Ê‹H0‹@4ÊÐÃ´&    èüÿÿÿðP$Ã¶    èüÿÿÿðƒ@$Ãt& èüÿÿÿ¶P8Ç@(    Ç     ƒÊÇ@,    ‰@Ç@0    ‰@Ç@4    ‰@Ç@$    ‰@ˆP8Ã´&    t& èüÿÿÿ‹…Òu‹P$…Òu¶P8ƒâþˆP8Ãv ‹P$…Òté¶P8ƒâþˆP8Ã´&    v èüÿÿÿ‰Ñ¶P8„ÉtƒÊ$ˆP8Ã´&    v ƒâßˆP8Ã´&    fèüÿÿÿ‰Â¶@8ƒàt‹B9Â•ÀÃ´&    èüÿÿÿ‰Á¶@8ƒàt
‹A‹ …À•ÀÃt& èüÿÿÿ¶P8ƒât
‹ Ã´&    1ÀÃt& èüÿÿÿ¶P8ƒât
‹@‹ Ãt& 1ÀÃt& èüÿÿÿ‰Á¶@8ƒàtS‹Y‹…Ût‹I[‰
Ã´&    Ã´&    1À[Ã´&    t& èüÿÿÿðƒ@$‹H4ƒÁ‰H4Ç    ‹H‰‰PÃ´&    ´&    èüÿÿÿ‰Á‹@$…ÀtRVSðƒA$ðƒD$ü Ç    ‹A;Au?;AuB9A•À¶À‹s(ƒÆ‰s(‹[‰‰TƒÀƒøuô¸   [^Ãt& 1ÀÃt& ¸   ëÈ¸   ëÀ´&    fèüÿÿÿ¶H8ƒáuÃ‹H9ÈtI‹H(S‰J‹J‹‰‹H‹‰Ç    ‹H‰Jº   ‹H9LtƒêsòÇ@(    [Ãt& ‰DëèfÃ´&    ´&    èüÿÿÿ¶H8ƒáuÃ‹H‹	…ÉtOÇB    ‹JVS‹X‹‰‹H‰J‹HÇ    ¹   ˆƒÁ‹s(r‹p‰sÇC(    ƒùuâ[^Ã´&    Ã´&    èüÿÿÿ‹RðP$Ãv èüÿÿÿ‹
…Ét=‰ÑS‹P(Q‹‰P(‹Q‰‹‰1Ò9Du‹Y‰\ƒÂƒúuëÇ    ‰I[Ãt& Ã´&    èüÿÿÿ‹
…ÉtS‹H4J‰H4‹H‹‰‹R[‰PÃt& Ã´&    èüÿÿÿ¶H8ƒá„~   ‹H‹	…Étp;Pxk‹H‰H‹H,…ÉtH(‰H(Ç@,    ;Px!‹P‰P‹H0…ÉtH(‰H(Ç@0    ‰P‰PÃ‹P‰P9Pt ‹P‰P‹P0…ÒtP,‰P,Ç@0    ‹P‰PÃt& é{ÿÿÿ´&    fèüÿÿÿUWVSƒì¶H8ƒá„¯   ‹H‹	…É„   ‹X9Xt9Px}1É;Xt‹H)ÑÁéˆ‹s‹>1ö…ÿtJyi‰<$‰ù9Íty‹t¸…öts,‰s,ÇD¸    ƒùu+‹Hƒ<$‰S‰Kt	‹H‰P‰H¾   ƒÄ‰ð[^_]Ãt& ¹   ë¬‹@‹ ƒÄ1ö[‰ð^_]Ã´&    éJÿÿÿèüÿÿÿWV‰ÆS‰Óƒìd¡    ‰D$1Àèüÿÿÿ‰Ø|$‰â‰d$‰|$Ç$    ÇD$    ÇD$    ÇD$    èüÿÿÿ‰Ø‰úèüÿÿÿÇC$    ‹D$ðF$‹D$ðF$‰â‰ðèüÿÿÿ‹T$…Òt‹F4D$‰F4‹F‰‹D$‰F¶C8ÇC(    Ç    ƒÈÇC,    ‰[ÇC0    ‰[ÇC4    ‰[ÇC$    ‰[ˆC8‹D$d+    uƒÄ[^_Ãèüÿÿÿffffffèüÿÿÿ‹T  …Òt‹RX…Òtéüÿÿÿt& ‹    …Òuå1ÀÃv èüÿÿÿW‰Ï‹ˆT  V‰ÖS…Ét
ƒ|$vƒ|$‹    wð…Éu[^_Ãv ‹Y4…Ûtòÿt$‰ò‰ùÿt$ÿt$èüÿÿÿƒÄëØ´&    èüÿÿÿV‰ÖS‹T  ‹\$…ÒD    ƒûwGƒ¸X   tF…ÒuRÿt$‰òSèüÿÿÿZY…À~[^Ãt& Pƒúw
¹þûýÿ£Ñsæ¸ûÿÿÿ[^Ãt& ¶    1ÀëÊ´&    v ÿt$S‹Z(‰òèüÿÿÿ[^ë©´&    ¶    èüÿÿÿÿt$ÿt$èNÿÿÿ1Ò…ÀHÂZYÃt& èüÿÿÿS‰Ó‰Êÿt$‹KR‹è'ÿÿÿZY…Àx‰C1À[Ã´&    v èüÿÿÿWVS‹˜T  ‹t$‹|$…Ûtƒþv!´&    ‹    ƒþwì…Ûu	[^_éüÿÿÿ‹[,…Ût	WVèüÿÿÿXZ[^_Ã´&    t& èüÿÿÿU‰åWV}Sƒì‹7‰uð‹w‹‰}ì‹¸T  …ÿD=    ƒþw‹˜X  …Ût…ÿu/eô[^_]éüÿÿÿv ¶    ¸ÿÿÿÿºÿÿÿÿeô[^_]Ãt& ‹_0…ÛtÿuìVÿuðèüÿÿÿƒÄëÜ´&    ¸ÿÿÿÿºÿÿÿÿëÈt& èüÿÿÿVS‹˜T  ‹t$…Ûtƒþv&t& ‹    ƒþwï…Ûu[^éüÿÿÿ´&    v ‹[@…ÛtVèüÿÿÿX[^Ã´&    ´&    èüÿÿÿVS‹˜T  ‹t$…Ûtƒþv&t& ‹    ƒþwï…Ûu[^éüÿÿÿ´&    v ‹[D…ÛtVèüÿÿÿX[^Ã´&    ´&    èüÿÿÿUWVSƒì‹\$$‹|$‹l$‹t$ ‰$‹˜T  …Ût'‹[…Ût6ÿ4$VUWèüÿÿÿƒÄƒÄ[^_]Ã´&    v ‹    …ÛuÏƒÄ[^_]éüÿÿÿ¸úÿÿÿëÑv èüÿÿÿ‹T  …Òt‹B…À•ÀÃ´&    ‹    …ÒuåéüÿÿÿèüÿÿÿUWVSƒì‹\$$‹|$‹l$‹t$ ‰$‹˜T  …Ût'‹[…Ût6ÿ4$VUWèüÿÿÿƒÄƒÄ[^_]Ã´&    v ‹    …ÛuÏƒÄ[^_]éüÿÿÿ¸úÿÿÿëÑv èüÿÿÿU‹T  ‰å…Òt‹RP…Òt]éüÿÿÿ‹    …Òuè]éüÿÿÿ¸ÿÿÿÿ1Ò]Ã´&    èüÿÿÿUWVS‰Óƒì‹\  ‹°T  ‹|$‹¨`  ‰$‹T$ƒâø…öt,$u1ët& ,$‹5    tì…öu‰|$‰T$ƒÄ‰Ú[^_]éüÿÿÿv ‹6…ötWR‰ÚèüÿÿÿZYƒÄ[^_]Ã1Àëô´&    t& èüÿÿÿU‰ÍW‰×º   V‰Æ¸à6 SƒìhpË  jÿ‹L$ èüÿÿÿY[…Àt]ÿt$‰é‰ú‰Ãÿt$‰ðèüÿÿÿ‰ÁXZ…Ét1‹E ‹U‰K‰;‰C‹D$‰S‰Ú‰C‰ð‰$èüÿÿÿ‹$ƒÄ‰È[^_]Ã‰Ø‰$èüÿÿÿ‹$ëç1Éëã´&    ¶    èüÿÿÿUWVS‰Ãƒì‹D$ ‹|$‹l$‰$‹ƒT  …ÀD    œ^æ   tJ…Ét<…Àu"‹$‰|$‰l$‰D$ ƒÄ‰Ø[^_]éüÿÿÿt& ‹p…ötÿ4$‰ØUWèüÿÿÿƒÄƒÄ[^_]Ãfë²´&    t& èüÿÿÿWVS‰Ãƒìd¡    ‰D$1À‹t$(‹|$,‰$‰L$‰ØÇD$    ‰t$‰|$j WVèüÿÿÿ¹@, ºà6 D$P‰ØèüÿÿÿƒÄ…Àu‹D$d+    uƒÄ[^_Ãt& ëäèüÿÿÿ´&    ´&    èüÿÿÿUWVS‰Ã‰Ð‹l$‹³T  ‹“`  ‹|$…öD5    “\  t&÷Ç   u.ÿ  â ðÿÿ…öu&‰Ø[^_]éüÿÿÿv 1À[^_]Ã´&    1Àëîf‹v…ötW‰ØUèüÿÿÿZYëØ1ÀëÔ´&    èüÿÿÿéfÿÿÿ¶    èüÿÿÿUÁÿ  WÁéVS‹zP‹D$9ÏsS‰Ó‹)ù‹s)Ö‰õÁí9ér?‹æ ðÿÿÿs(ÿs$ÁéV‹,Í    ƒåà)è‰Á‰ØÁùiÉÍÌÌÌùèüÿÿÿƒÄ[^_]Ãt& ¸úÿÿÿëï´&    fèüÿÿÿUWVS‰Ëƒì‹¸T  ‹L$ ‰<$…ÿt!‹<$‹w…öt.Q‰ÙèüÿÿÿƒÄ[^_]Ã´&    ‹5    ‰4$…ötc‹4$‹v…öuÒ‹3Âÿ  â ðÿÿ‹~‹n‰|$‹<$‰l$‹6ƒæü‰õ‹w…ötQ‰éÿt$ÿt$èüÿÿÿƒÄ‰Øèüÿÿÿ‰ØƒÄ[^_]éüÿÿÿt& ‹;Âÿ  â ðÿÿ‹7Qÿwÿwƒæü‰ñèüÿÿÿƒÄëÁ´&    v èüÿÿÿU‰ÍW‰ÇV‰ÖS‹T$‹€T  ‰Ñ€á…Àt^…É…ö   ‹X…Ût`R‰ø‰ò‰éÿt$èüÿÿÿ^‰Ã_…Àt.ÇC   ‰Ø[^_]Ã´&    f‰Øèüÿÿÿ´&    f‰Øèüÿÿÿ1Û‰Ø[^_]Ã¡    …É…“   …Àu™‹D$% @ …   kÀ8‹T$¹   ‹€   èüÿÿÿ‰Ã…Àtº‹L$º   èüÿÿÿ…Àu¡‹ÿt$‰òUH‰øèýÿÿZY…À„tÿÿÿ‹Æÿ  æ ðÿÿ‹ƒâ¨uU	ÂÇA    ‰‰q‹‹P‰Pé+ÿÿÿf1Û‰Ø[^_]Ãt& ¸   öD$…`ÿÿÿ1ÀöD$•ÀƒÀéNÿÿÿ¶    ¶    èüÿÿÿV‰ÎS‹€T  …Àt‹X…Ûu&‹[^‹ ƒàüéüÿÿÿ´&    ¡    …Àtá‹X…ÛtÚ¡    Âÿ  Áê%c  ‰Á¡   %   €‰Ã‹FSQ¹   èüÿÿÿZY[^Ã´&    t& èüÿÿÿ‹ˆT  …Ét‹A…Àt‰Ðéüÿÿÿv ‹    …ÉuåÃt& èüÿÿÿS‹˜T  …Ût‹[L…Ûtèüÿÿÿ[Ãf‹    …Ûuæ[éüÿÿÿ¸   [Ã´&    fèüÿÿÿ‹T  …Òt‹RT…Òtéüÿÿÿt& ‹    …Òuåéüÿÿÿ¸ÿÿÿÿÃ´&    v èüÿÿÿW‰ÏV‹ˆT  …Ét‹Q8¸   …Òt!^_Ã¶    ‹    …Éuá‰ù^_éüÿÿÿt& ‹A<…À•ÀëÕ¶    èüÿÿÿS1À‹Y9Zt[Ã¶    ‹9t¸   [Ã´&    f‹B3A‹R3Q‰Á¸   	Êu×[Ã´&    èüÿÿÿWVS‰Ã‰Ð‹t$‹“T  …Òt6‹R…Òt9Áÿ  ‹XPÁé9ÙvG‹P‰Ï+Áê)ß9úw7‹V[^_éüÿÿÿt& ‹    …ÒuÀ‹‹ƒâü‰T$‰Â‰Ø[^_éÿúÿÿ´&    [¸úÿÿÿ^_Ã´&    ´&    èüÿÿÿW|$ƒäøÿwüU‰åWVS‰û‰Çƒì<‰EÌ‹‹s‰EÄ‹C‰u¼‰E¸d¡    ‰Eì‹‡T  …ÀD    ƒþ‡¾  ‹]Ì‹›X  ‰]À…Û„²  …À…"  ‹Áè‹Å    ƒãà)Ú1ÛÁúiÂÍÌÌÌ1Ò¤ÂÁàÁ‹EÌÓ‹°l  ‹€€  ‰EÈ…ö…§   ‹EÈ‰Mä‰]è…Àt‹EÈ€x) …  ‹EÄ1Ò‰MØ‰]ÜƒÀÿƒÒÿÈÚ‰EÐ‰UÔ‹UÀ‹}Ì‹‹r‹—d  ‰Eà‹‡h  ‰uÈuàt‰Ç	×„î   ‹uÈ9Uà‰÷ÇBUàBÆ‹uÐ‹}Ô9òø‚˜   ‹Eìd+    …ú  ‹EØ‹UÜeô[^_]gø_Ãv ‹F‹~‰Â	útI‰uØ‰}Ð‰Eàv ‹EØ‰ß‹P‹ 9Á×r‰Î‰ß)Æ×;uà‰øEÐ‚š   ƒEØ ‹EØ‹P‹@‰EÐ	Ð‰UàuÃ‹UÈÇEäÿÿÿÿÇEèÿÿÿÿ…Òt	‹EÈ€x) uƒ=    „à   ÿu¸‹EÌ‰Ê‰Ùÿu¼ÿuÄèüÿÿÿ‰EØƒÄ‰UÜé8ÿÿÿt& ‹Uà‹EÈéÿÿÿt& ¶    ÇEØÿÿÿÿÇEÜÿÿÿÿéÿÿÿv ‹uØ‰È‰Ú+FV‹uÈ‰EØ‰UÜ‰Eä‰Uè…öt‹EÈ€x) …{ÿÿÿ‹EÄ‹uØ1Ò‹}ÜƒÀÿƒÒÿð‰EÐ‰ðú!ø‰UÔƒøÿ…aþÿÿéAÿÿÿ¶    ÿu¸ÿu¼ÿuÄ‹X ‹EÌèüÿÿÿ‰EØƒÄ‰UÜéþÿÿ¶    €=–    ÇEØÿÿÿÿÇEÜÿÿÿÿ…`þÿÿ‹EÀ‹]ÌÆ–   ‹P‹ ‹³d  ‹»h  ‰EØ‰Ø‹[,‰UÜ…Ûu‹‹EÌèüÿÿÿMäWVÿuÜÿuØÿuÄQSPhø8 èüÿÿÿÇEØÿÿÿÿƒÄ$ÇEÜÿÿÿÿéöýÿÿèüÿÿÿ´&    ´&    èüÿÿÿUW‰ÏV‰ÖSƒì0‹\$D‹L$L‰D$‹€T  ‰\$(‹\$H‰L$‰\$ …Àtƒ|$ †ß  t& ¡    ƒûwï…À…Æ  ‹\$‹ƒl  ‹›€  ‰\$‹\$‰D$,ƒã ‰\$$…À„Ë  ‹P‹X‰Ñ‰\$	Ù‰T$„ì  ‰$‰Ý‰T$´&    v ‹$‹H‹X‰ø9ÎØr‰ð‰ú)ÈÚ9è‰ÐD$‚™  ƒ$ ‹$‹h‹@‰Â‰D$	êuÂÇ$ÿÿÿÿÇD$ÿÿÿÿ‹D$$‹l$,…À„Ž   ‹L$…É„*  ‹|$‹‹W‹<$‹l$9Ç‰èÐ‚  ‹D$‰ú‰é;x‰ïxƒø   ‹D$ƒÈ ‰D$L‹D$ ‰D$H‹D$(‰D$D‹D$ƒÄ0[^_]éüÿÿÿ´&    v ‹E4‹U0ƒÅ ‰Á‰T$	Ñ‰D$„rÿÿÿ‹E‹U‰û9ÆÓr×‰ñ‰û)ÁÓ;L$‰ØD$sÃu}‹L$…Étx‹‹Q‰û9ÆÓ‚Fÿÿÿ‹\$‰ù;sKƒ4ÿÿÿÿt$ ‰ù‰òÿt$,‹|$ ‰øèüÿÿÿ‹‡€  ‰D$[^é÷þÿÿ´&    v ‹X$…Ûtÿt$‰ò‰ùÿt$$ÿt$0‹D$$èüÿÿÿƒÄƒÄ0[^_]Ãt& ‹T$$‰4$‰|$…Ò…ªþÿÿé[ÿÿÿ´&    ‹$‰ò‰ùPH‰$‰L$étþÿÿÇ$ÿÿÿÿÇD$ÿÿÿÿépþÿÿv èüÿÿÿUW‰ÏV‰ÖSƒì‹\$(‰D$‹€T  ‰\$‹\$,‰\$…Àtƒ|$†×   t& ¡    ƒûwï…À…¾   ‹D$‹€l  …Àti‹h‹P‰ë	ÓtF‰$‰T$t& ‹$‹H‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰Ðèr%ƒ$ ‹$‹h‹P‰è‰T$	ÐuÆƒÄ[^_]Ã¶    ‹$px‹D$‹€€  …ÀtÛ;0‰ûXrÒ;p‰ûXsÈ‹D$‰ò‰ù‰D$,‹D$‰D$(‹D$ƒÄ[^_]éüÿÿÿf‹X<…Ût›ÿt$‰ò‰ùÿt$‹D$èüÿÿÿXZéÿÿÿ´&    ¶    èüÿÿÿUW‰ÏV‰ÖSƒì‹\$(‰D$‹€T  ‰\$‹\$,‰\$…Àtƒ|$†×   t& ¡    ƒûwï…À…¾   ‹D$‹€l  …Àti‹h‹P‰ë	ÓtF‰$‰T$t& ‹$‹H‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰Ðèr%ƒ$ ‹$‹h‹P‰è‰T$	ÐuÆƒÄ[^_]Ã¶    ‹$px‹D$‹€€  …ÀtÛ;0‰ûXrÒ;p‰ûXsÈ‹D$‰ò‰ù‰D$,‹D$‰D$(‹D$ƒÄ[^_]éüÿÿÿf‹X8…Ût›ÿt$‰ò‰ùÿt$‹D$èüÿÿÿXZéÿÿÿ´&    ¶    èüÿÿÿUÂÿ  Wâ ðÿÿ‰ÏV‹°T  S‹L$‹\$‹l$…öt#‹v…ötUSQ‰ùèüÿÿÿƒÄ[^_]Ã´&    v ‹5    …öuÓ[‰ù^_]éüÿÿÿ´&    t& èüÿÿÿUW‰ÏV‰ÖS‰Ã‹€T  …Àt/‹hL…ít‰ò‰ù‰Øèüÿÿÿ…Àt1‰³\  1À‰»`  [^_]Ã¶    ¡    …ÀuÈ‰ØèüÿÿÿëÑ¶    ¸ûÿÿÿëÖ´&    fèüÿÿÿUWVS‰Ã‹€X  …ÀtS‰Ö‹“T  ‰Ï…Òt-‹jL…ít‰ò‰ù‰Øèüÿÿÿ…Àt/‹ƒX  ‰0‰x1À[^_]Ãt& ‹    …ÒuÉ‰ò‰ØèüÿÿÿëÐv ¸ûÿÿÿë×èüÿÿÿUWVS‰Ãƒì‹‹j‹r‹z‰$‹ƒT  ‹J…ÀD    œZ€æt+…Ét…Àu+U‰ØWV‹T$èüÿÿÿƒÄƒÄ[^_]Ã´&    v ëÑt& ‹@‰D$…ÀtÛU‰ØWV‹T$‹|$èüÿÿÿƒÄëÄ¶    èüÿÿÿ‰Ð‰ÊÃfffèüÿÿÿUW‰ÏV‰ÖSƒì‰D$‹€l  …ÀuZ‹D$ ‹L$1ÒƒÀÿ‹™d  ƒÒÿð‹±`  ú‹¹\  ‹‰h  ‰õ	ýu9ÃÑ“ÀƒÄ[^_]Ã‰Í	Ýtr9ß‰õÍBßBÎëÜt& ‹h‹P‰ë	ÓtE‰$‰T$t& ‹$‹‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰Ðèr6ƒ$ ‹$‹h‹P‰è‰T$	ÐuÇ1ÀƒÄ[^_]Ãt& ‰û‰ñéoÿÿÿ´&    ‹$+px‰ð!øƒøÿ… ÿÿÿ1ÀëÉ´&    ´&    èüÿÿÿUWVSƒì‰D$‹€Œ  ‰T$âÿ  ‰L$‰D$…B  ‹|$‹‡`  ‹—\  ‹·d  ‹¿h  ‰Ã	Ót‰û	ó„F  9ò‰ÃûBòBø‹D$‹€l  …Àtu‹h‹P‰ë	ÓtJ‰$‰T$´&    f‹$‹H‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰Ðèr-ƒ$ ‹$‹h‹X‰è‰\$	ØuÆƒ=    @uNƒL$ëG¶    ‹$px‹    ƒù@tß1À1ÒöÁ ”À•ÂÓàÓâƒÀÿƒÒÿ9ð‰ÐøsÀ1À…ÿ”ÀÁà	D$‹l$‹L$‹T$‰èèüÿÿÿ‰Ã…À„   ‹ ‰Þÿt$1ÿÁè‹Å    ƒàà)Æ‰èÁþiöÍÌÌÌ¤÷Áæ‰ò‰ùè~ýÿÿZ„Àt9ÿt$‹D$‰ò‰ùèhýÿÿZ„À„   ƒÄ‰Ø[^_]Ãt& ‰Ö‰Çé½þÿÿ´&    ‹L$‹D$‰Úèüÿÿÿ‹D$¹ÿÿÿÿƒèÁè½ÐDÑ‹D$ƒÂ‰Áƒøÿt~j ‹D$èüÿÿÿ^‰Ã…Àt\‹ ‰Þ1ÿÁè‹Å    ƒàà)ÆÁþiöÍÌÌÌ¤÷ÁæéYÿÿÿf‹D$‹L$‰Úèüÿÿÿ‹D$¨uƒàúƒÈ‰D$éyÿÿÿ´&    v ƒÄ1Û‰Ø[^_]Ãt& d‹    évÿÿÿt& é·ýÿÿ´&    fèüÿÿÿU‰Á1Ò‰åWVSƒäøƒì¡    ‹±l  ƒè¤ÂÁà‰$‰T$…öuC‹D$…À…“   ¸ÿÿÿÿ½$DÈ¸   1Ò1Û¥ÂÓàöÁ EÐEÃƒÀÿƒÒÿeô[^_]Ã¶    ‹~‹^‰ø	ØtK‰t$‰\$t& ‹D$‹$‹t$‹‹H‰ð9ÓÈr)ÓÎ;\$‰ðør:ƒD$ ‹D$‹x‹p‰ø‰t$	ðuÁ¸ÿÿÿÿºÿÿÿÿ½ÈDÊƒÁ éfÿÿÿ´&    f‹t$‹N‹^)$\$é/ÿÿÿ´&    v èüÿÿÿUWºÿ  Vç ðÿÿS‰Ã‰úƒì‰L$‹L$$‹l$(‰È€Ì ÷Å   EÈ‰Ø€åþ÷Å   …é   è$üÿÿ‰Æ…À„
  ‹ Áèƒàƒøt}ƒøth‰ðèüÿÿÿ‰D$‹D$‰ù1Ò1ÿèüÿÿÿ‹Áè‹Å    ƒàà)Æ‹ƒl  ÁþiöÍÌÌÌ¤÷Áæ…À…7  ‹D$‰0‰x‹D$ƒÄ[^_]Ã´&    v ƒ=    u´&    U‹    ‰Ø‹   âc  á   €èüÿÿÿÿt$$R‰úP‰ðèüÿÿÿ‰D$ƒÄ…À…Uÿÿÿ‰ù‰ò‰Øèüÿÿÿët& è;ûÿÿ‰D$…À„  ‹|$‹‰$Áè‹Å    ‰øƒâà)Ð1ÒÁøiÀÍÌÌÌ¤ÂÁà‰Æ‹ƒl  ‰×…À„2ÿÿÿ‹h‹P‰ë	ÓtJ‰$‰T$¶    ‹$‹‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰Ðè‚’   ƒ$ ‹$‹h‹P‰è‰T$	ÐuÃ¾ÿÿÿÿ¿ÿÿÿÿéÍþÿÿt& ‹h‹P‰ë	Ótá‰$‰T$t& ‹$‹‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰Ðèr.ƒ$ ‹$‹h‹X‰è‰\$	ØuÇ¾ÿÿÿÿ¿ÿÿÿÿéiþÿÿ´&    ‹$+pxéSþÿÿfÇD$    éMþÿÿv èüÿÿÿUWVS‰Ë‰Ñƒì‹t$$‹|$(‰T$‰Ú‰D$öD$,…²   ‰Øèüÿÿÿ„À…´   ‹D$‹€l  …Àtg‹h‹P‰ë	ÓtD‰$‰T$v ‹$‹H‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰Ðèr%ƒ$ ‹$‹h‹X‰è‰\$	ØuÆ¹Øÿÿÿ1Àë-t& ‹$px‰ð¬ø‰Â€ÁêÁá=ÿÿÿ wBÕ    ‹‹D$ƒâàÊ‹L$ƒÄ[^_]éüÿÿÿt& ‰Øèüÿÿÿ‹D$‹€l  …À…Aÿÿÿë¦1ÀëÁèüÿÿÿUWV‰ÖS‰Ãƒì‰L$‹L$(èÓøÿÿ‰D$…Àtp‹ ‰$Áèƒàƒø„`¼  ƒø„Æ   ‹|$‰øèüÿÿÿ‰ñ1Òèüÿÿÿ‹‰$Áè‹Å    ‰øƒâà)Ð1ÒÁøiÀÍÌÌÌ¤ÂÁà‰Æ‹ƒl  ‰×…Àu‹D$‰0‰x‹D$ƒÄ[^_]Ã´&    ‹h‹P‰ë	ÓtE‰$‰T$t& ‹$‹‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰Ðèr>ƒ$ ‹$‹h‹X‰è‰\$	ØuÇ¾ÿÿÿÿ¿ÿÿÿÿë‡v ƒ=    …-ÿÿÿé`¼  ¶    ‹$+pxé^ÿÿÿfèüÿÿÿW‰×V‰Æ‰ÈS‰Ëèüÿÿÿ‰ù‰Ú‰ð[^_éüÿÿÿ´&    ¶    èüÿÿÿUWVSƒì‰D$‰T$‰L$…ÉŽš   ÇD$    ´&    ‹D$‹p‹x‹D$‹€l  …À„…   ‹h‹P‰ë	ÓtB‰$‰T$‹$‹H‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰ÐèrEƒ$ ‹$‹h‹P‰è‰T$	ÐuÆ‹D$ƒD$‹\$èüÿÿÿ‰D$9\$…vÿÿÿƒÄ[^_]Ã¶    ‹$px‹D$‹€€  …Àt»;0‰ûXr²;p‰ûXs¨ÿt$,‹D$‰ò‰ùÿp‹D$èüÿÿÿXZëŒfèüÿÿÿUWVSƒì‰D$‰T$‰L$…ÉŽš   ÇD$    ´&    ‹D$‹p‹x‹D$‹€l  …À„…   ‹h‹P‰ë	ÓtB‰$‰T$‹$‹H‹X‰ø9ÎØr‰ð‰ú)ÈÚ;D$‰ÐèrEƒ$ ‹$‹h‹P‰è‰T$	ÐuÆ‹D$ƒD$‹\$èüÿÿÿ‰D$9\$…vÿÿÿƒÄ[^_]Ã¶    ‹$px‹D$‹€€  …Àt»;0‰ûXr²;p‰ûXs¨ÿt$,‹D$‰ò‰ùÿp‹D$èüÿÿÿXZëŒ