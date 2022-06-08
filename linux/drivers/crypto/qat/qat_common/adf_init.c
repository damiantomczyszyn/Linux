= 1,
	.agc = &xc3028_agc_config,
	.bw  = &xc3028_bw_config,

	.gpio_dir = DIB7000P_GPIO_DEFAULT_DIRECTIONS,
	.gpio_val = DIB7000P_GPIO_DEFAULT_VALUES,
	.gpio_pwm_pos = DIB7000P_GPIO_DEFAULT_PWM_POS,

	.pwm_freq_div = 0,
	.agc_control  = NULL,
	.spur_protect = 0,

	.output_mode = OUTMODE_MPEG2_SERIAL,
};

static struct zl10353_config dvico_fusionhdtv_xc3028 = {
	.demod_address = 0x0f,
	.if2           = 45600,
	.no_tuner      = 1,
	.disable_i2c_gate_ctrl = 1,
};

static struct stv0900_reg stv0900_ts_regs[] = {
	{ R0900_TSGENERAL, 0x00 },
	{ R0900_P1_TSSPEED, 0x40 },
	{ R0900_P2_TSSPEED, 0x40 },
	{ R0900_P1_TSCFGM, 0xc0 },
	{ R0900_P2_TSCFGM, 0xc0 },
	{ R0900_P1_TSCFGH, 0xe0 },
	{ R0900_P2_TSCFGH, 0xe0 },
	{ R0900_P1_TSCFGL, 0x20 },
	{ R0900_P2_TSCFGL, 0x20 },
	{ 0xffff, 0xff }, /* terminate */
};

static struct stv0900_config netup_stv0900_config = {
	.demod_address = 0x68,
	.demod_mode = 1, /* dual */
	.xtal = 8000000,
	.clkmode = 3,/* 0-CLKI, 2-XTALI, else AUTO */
	.diseqc_mode = 2,/* 2/3 PWM */
	.ts_config_regs = stv0900_ts_regs,
	.tun1_maddress = 0,/* 0x60 */
	.tun2_maddress = 3,/* 0x63 */
	.tun1_adc = 1,/* 1 Vpp */
	.tun2_adc = 1,/* 1 Vpp */
};

static struct stv6110_config netup_stv6110_tunerconfig_a = {
	.i2c_address = 0x60,
	.mclk = 16000000,
	.clk_div = 1,
	.gain = 8, /* +16 dB  - maximum gain */
};

static struct stv6110_config netup_stv6110_tunerconfig_b = {
	.i2c_address = 0x63,
	.mclk = 16000000,
	.clk_div = 1,
	.gain = 8, /* +16 dB  - maximum gain */
};

static struct cx24116_config tbs_cx24116_config = {
	.demod_address = 0x55,
};

static struct cx24117_config tbs_cx24117_config = {
	.demod_address = 0x55,
};

static struct ds3000_config tevii_ds3000_config = {
	.demod_address = 0x68,
};

static struct ts2020_config tevii_ts2020_config  = {
	.tuner_address = 0x60,
	.clk_out_div = 1,
	.frequency_div = 1146000,
};

static struct cx24116_config dvbworld_cx24116_config = {
	.demod_address = 0x05,
};

static struct lgs8gxx_config mygica_x8506_lgs8gl5_config = {
	.prod = LGS8GXX_PROD_LGS8GL5,
	.demod_address = 0x19,
	.serial_ts = 0,
	.ts_clk_pol = 1,
	.ts_clk_gated = 1,
	.if_clk_freq = 30400, /* 30.4 MHz */
	.if_freq = 5380, /* 5.38 MHz */
	.if_neg_center = 1,
	.ext_adc = 0,
	.adc_signed = 0,
	.if_neg_edge = 0,
};

static struct xc5000_config mygica_x8506_xc5000_config = {
	.i2c_address = 0x61,
	.if_khz = 5380,
};

static struct mb86a20s_config mygica_x8507_mb86a20s_config = {
	.demod_address = 0x10,
};

static struct xc5000_config mygica_x8507_xc5000_config = {
	.i2c_address = 0x61,
	.if_khz = 4000,
};

static struct stv090x_config prof_8000_stv090x_config = {
	.device                 = STV0903,
	.demod_mode             = STV090x_SINGLE,
	.clk_mode               = STV090x_CLK_EXT,
	.xtal                   = 27000000,
	.address                = 0x6A,
	.ts1_mode               = STV090x_TSMODE_PARALLEL_PUNCTURED,
	.repeater_level         = STV090x_RPTLEVEL_64,
	.adc1_range             = STV090x_ADC_2Vpp,
	.diseqc_envelope_mode   = false,

	.tuner_get_frequency    = stb6100_get_frequency,
	.tuner_set_frequency    = stb6100_set_frequency,
	.tuner_set_bandwidth    = stb6100_set_bandwidth,
	.tuner_get_bandwidth    = stb6100_get_bandwidth,
};

static struct stb6100_config prof_8000_stb6100_config = {
	.tuner_address = 0x60,
	.refclock = 27000000,
};

static struct lgdt3306a_config hauppauge_quadHD_ATSC_a_config = {
	.i2c_addr               = 0x59,
	.qam_if_khz             = 4000,
	.vsb_if_khz             = 3250,
	.deny_i2c_rptr          = 1, /* Disabled */
	.spectral_inversion     = 0, /* Disabled */
	.mpeg_mode              = LGDT3306A_MPEG_SERIAL,
	.tpclk_edge             = LGDT3306A_TPCLK_RISING_EDGE,
	.tpvalid_polarity       = LGDT3306A_TP_VALID_HIGH,
	.xtalMHz                = 25, /* 24 or 25 */
};

static struct lgdt3306a_config hauppauge_quadHD_ATSC_b_config = {
	.i2c_addr               = 0x0e,
	.qam_if_khz             = 4000,
	.vsb_if_khz             = 3250,
	.deny_i2c_rptr          = 1, /* Disabled */
	.spectral_inversion     = 0, /* Disabled */
	.mpeg_mode              = LGDT3306A_MPEG_SERIAL,
	.tpclk_edge             = LGDT3306A_TPCLK_RISING_EDGE,
	.tpvalid_polarity       = LGDT3306A_TP_VALID_HIGH,
	.xtalMHz                = 25, /* 24 or 25 */
};

static int p8000_set_voltage(struct dvb_frontend *fe,
			     enum fe_sec_voltage voltage)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	if (voltage == SEC_VOLTAGE_18)
		cx_write(MC417_RWD, 0x00001e00);
	else if (voltage == SEC_VOLTAGE_13)
		cx_write(MC417_RWD, 0x00001a00);
	else
		cx_write(MC417_RWD, 0x00001800);
	return 0;
}

static int dvbsky_t9580_set_voltage(struct dvb_frontend *fe,
					enum fe_sec_voltage voltage)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	cx23885_gpio_enable(dev, GPIO_0 | GPIO_1, 1);

	switch (voltage) {
	case SEC_VOLTAGE_13:
		cx23885_gpio_set(dev, GPIO_1);
		cx23885_gpio_clear(dev, GPIO_0);
		break;
	case SEC_VOLTAGE_18:
		cx23885_gpio_set(dev, GPIO_1);
		cx23885_gpio_set(dev, GPIO_0);
		break;
	case SEC_VOLTAGE_OFF:
		cx23885_gpio_clear(dev, GPIO_1);
		cx23885_gpio_clear(dev, GPIO_0);
		break;
	}

	/* call the frontend set_voltage function */
	port->fe_set_voltage(fe, voltage);

	return 0;
}

static int dvbsky_s952_portc_set_voltage(struct dvb_frontend *fe,
					enum fe_sec_voltage voltage)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	cx23885_gpio_enable(dev, GPIO_12 | GPIO_13, 1);

	switch (voltage) {
	case SEC_VOLTAGE_13:
		cx23885_gpio_set(dev, GPIO_13);
		cx23885_gpio_clear(dev, GPIO_12);
		break;
	case SEC_VOLTAGE_18:
		cx23885_gpio_set(dev, GPIO_13);
		cx23885_gpio_set(dev, GPIO_12);
		break;
	case SEC_VOLTAGE_OFF:
		cx23885_gpio_clear(dev, GPIO_13);
		cx23885_gpio_clear(dev, GPIO_12);
		break;
	}
	/* call the frontend set_voltage function */
	return port->fe_set_voltage(fe, voltage);
}

static int cx23885_sp2_ci_ctrl(void *priv, u8 read, int addr,
				u8 data, int *mem)
{
	/* MC417 */
	#define SP2_DATA              0x000000ff
	#define SP2_WR                0x00008000
	#define SP2_RD                0x00004000
	#define SP2_ACK               0x00001000
	#define SP2_ADHI              0x00000800
	#define SP2_ADLO              0x00000400
	#define SP2_CS1               0x00000200
	#define SP2_CS0               0x00000100
	#define SP2_EN_ALL            0x00001000
	#define SP2_CTRL_OFF          (SP2_CS1 | SP2_CS0 | SP2_WR | SP2_RD)

	struct cx23885_tsport *port = priv;
	struct cx23885_dev *dev = port->dev;
	int ret;
	int tmp = 0;
	unsigned long timeout;

	mutex_lock(&dev->gpio_lock);

	/* write addr */
	cx_write(MC417_OEN, SP2_EN_ALL);
	cx_write(MC417_RWD, SP2_CTRL_OFF |
				SP2_ADLO | (0xff & addr));
	cx_clear(MC417_RWD, SP2_ADLO);
	cx_write(MC417_RWD, SP2_CTRL_OFF |
				SP2_ADHI | (0xff & (addr >> 8)));
	cx_clear(MC417_RWD, SP2_ADHI);

	if (read)
		/* data in */
		cx_write(MC417_OEN, SP2_EN_ALL | SP2_DATA);
	else
		/* data out */
		cx_write(MC417_RWD, SP2_CTRL_OFF | data);

	/* chip select 0 */
	cx_clear(MC417_RWD, SP2_CS0);

	/* read/write */
	cx_clear(MC417_RWD, (read) ? SP2_RD : SP2_WR);

	/* wait for a maximum of 1 msec */
	timeout = jiffies + msecs_to_jiffies(1);
	while (!time_after(jiffies, timeout)) {
		tmp = cx_read(MC417_RWD);
		if ((tmp & SP2_ACK) == 0)
			break;
		usleep_range(50, 100);
	}

	cx_set(MC417_RWD, SP2_CTRL_OFF);
	*mem = tmp & 0xff;

	mutex_unlock(&dev->gpio_lock);

	if (!read) {
		if (*mem < 0) {
			ret = -EREMOTEIO;
			goto err;
		}
	}

	return 0;
err:
	return ret;
}

static int cx23885_dvb_set_frontend(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *p = &fe->dtv_property_cache;
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
		switch (p->modulation) {
		case VSB_8:
			cx23885_gpio_clear(dev, GPIO_5);
			break;
		case QAM_64:
		case QAM_256:
		default:
			cx23885_gpio_set(dev, GPIO_5);
			break;
		}
		break;
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
		/* Select Digital TV */
		cx23885_gpio_set(dev, GPIO_0);
		break;
	}

	/* Call the real set_frontend */
	if (port->set_frontend)
		return port->set_frontend(fe);

	return 0;
}

static void cx23885_set_frontend_hook(struct cx23885_tsport *port,
				     struct dvb_frontend *fe)
{
	port->set_frontend = fe->ops.set_frontend;
	fe->ops.set_frontend = cx23885_dvb_set_frontend;
}

static struct lgs8gxx_config magicpro_prohdtve2_lgs8g75_config = {
	.prod = LGS8GXX_PROD_LGS8G75,
	.demod_address = 0x19,
	.serial_ts = 0,
	.ts_clk_pol = 1,
	.ts_clk_gated = 1,
	.if_clk_freq = 30400, /* 30.4 MHz */
	.if_freq = 6500, /* 6.50 MHz */
	.if_neg_center = 1,
	.ext_adc = 0,
	.adc_signed = 1,
	.adc_vpp = 2, /* 1.6 Vpp */
	.if_neg_edge = 1,
};

static struct xc5000_config magicpro_prohdtve2_xc5000_config = {
	.i2c_address = 0x61,
	.if_khz = 6500,
};

static struct atbm8830_config mygica_x8558pro_atbm8830_cfg1 = {
	.prod = ATBM8830_PROD_8830,
	.demod_address = 0x44,
	.serial_ts = 0,
	.ts_sampling_edge = 1,
	.ts_clk_gated = 0,
	.osc_clk_freq = 30400, /* in kHz */
	.if_freq = 0, /* zero IF */
	.zif_swap_iq = 1,
	.agc_min = 0x2E,
	.agc_max = 0xFF,
	.agc_hold_loop = 0,
};

static struct max2165_config mygic_x8558pro_max2165_cfg1 = {
	.i2c_address = 0x60,
	.osc_clk = 20
};

static struct atbm8830_config mygica_x8558pro_atbm8830_cfg2 = {
	.prod = ATBM8830_PROD_8830,
	.demod_address = 0x44,
	.serial_ts = 1,
	.ts_sampling_edge = 1,
	.ts_clk_gated = 0,
	.osc_clk_freq = 30400, /* in kHz */
	.if_freq = 0, /* zero IF */
	.zif_swap_iq = 1,
	.agc_min = 0x2E,
	.agc_max = 0xFF,
	.agc_hold_loop = 0,
};

static struct max2165_config mygic_x8558pro_max2165_cfg2 = {
	.i2c_address = 0x60,
	.osc_clk = 20
};
static struct stv0367_config netup_stv0367_config[] = {
	{
		.demod_address = 0x1c,
		.xtal = 27000000,
		.if_khz = 4500,
		.if_iq_mode = 0,
		.ts_mode = 1,
		.clk_pol = 0,
	}, {
		.demod_address = 0x1d,
		.xtal = 27000000,
		.if_