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

static struct s5h1409_config hauppauge_hvr1800lp_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_OFF,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1409_config hauppauge_hvr1500_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_OFF,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct mt2131_config hauppauge_generic_tunerconfig = {
	0x61
};

static struct lgdt330x_config fusionhdtv_5_express = {
	.demod_chip = LGDT3303,
	.serial_mpeg = 0x40,
};

static struct s5h1409_config hauppauge_hvr1500q_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_ON,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1409_config dvico_s5h1409_config = {
	.demod_address = 0x32 >> 1,
	.output_mode   = S5H1409_SERIAL_OUTPUT,
	.gpio          = S5H1409_GPIO_ON,
	.qam_if        = 44000,
	.inversion     = S5H1409_INVERSION_OFF,
	.status_mode   = S5H1409_DEMODLOCKING,
	.mpeg_timing   = S5H1409_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1411_config dvico_s5h1411_config = {
	.output_mode   = S5H1411_SERIAL_OUTPUT,
	.gpio          = S5H1411_GPIO_ON,
	.qam_if        = S5H1411_IF_44000,
	.vsb_if        = S5H1411_IF_44000,
	.inversion     = S5H1411_INVERSION_OFF,
	.status_mode   = S5H1411_DEMODLOCKING,
	.mpeg_timing   = S5H1411_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct s5h1411_config hcw_s5h1411_config = {
	.output_mode   = S5H1411_SERIAL_OUTPUT,
	.gpio          = S5H1411_GPIO_OFF,
	.vsb_if        = S5H1411_IF_44000,
	.qam_if        = S5H1411_IF_4000,
	.inversion     = S5H1411_INVERSION_ON,
	.status_mode   = S5H1411_DEMODLOCKING,
	.mpeg_timing   = S5H1411_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};

static struct xc5000_config hauppauge_hvr1500q_tunerconfig = {
	.i2c_address      = 0x61,
	.if_khz           = 5380,
};

static struct xc5000_config dvico_xc5000_tunerconfig = {
	.i2c_address      = 0x64,
	.if_khz           = 5380,
};

static struct tda829x_config tda829x_no_probe = {
	.probe_tuner = TDA829X_DONT_PROBE,
};

static struct tda18271_std_map hauppauge_tda18271_std_map = {
	.atsc_6   = { .if_freq = 5380, .agc_mode = 3, .std = 3,
		      .if_lvl = 6, .rfagc_top = 0x37 },
	.qam_6    = { .if_freq = 4000, .agc_mode = 3, .std = 0,
		      .if_lvl = 6, .rfagc_top = 0x37 },
};

static struct tda18271_std_map hauppauge_hvr1200_tda18271_std_map = {
	.dvbt_6   = { .if_freq = 3300, .agc_mode = 3, .std = 4,
		      .if_lvl = 1, .rfagc_top = 0x37, },
	.dvbt_7   = { .if_freq = 3800, .agc_mode = 3, .std = 5,
		      .if_lvl = 1, .rfagc_top = 0x37, },
	.dvbt_8   = { .if_freq = 4300, .agc_mode = 3, .std = 6,
		      .if_lvl = 1, .rfagc_top = 0x37, },
};

static struct tda18271_config hauppauge_tda18271_config = {
	.std_map = &hauppauge_tda18271_std_map,
	.gate    = TDA18271_GATE_ANALOG,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda18271_config hauppauge_hvr1200_tuner_config = {
	.std_map = &hauppauge_hvr1200_tda18271_std_map,
	.gate    = TDA18271_GATE_ANALOG,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda18271_config hauppauge_hvr1210_tuner_config = {
	.gate    = TDA18271_GATE_DIGITAL,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda18271_config hauppauge_hvr4400_tuner_config = {
	.gate    = TDA18271_GATE_DIGITAL,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda18271_std_map hauppauge_hvr127x_std_map = {
	.atsc_6   = { .if_freq = 3250, .agc_mode = 3, .std = 4,
		      .if_lvl = 1, .rfagc_top = 0x58 },
	.qam_6    = { .if_freq = 4000, .agc_mode = 3, .std = 5,
		      .if_lvl = 1, .rfagc_top = 0x58 },
};

static struct tda18271_config hauppauge_hvr127x_config = {
	.std_map = &hauppauge_hvr127x_std_map,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct lgdt3305_config hauppauge_lgdt3305_config = {
	.i2c_addr           = 0x0e,
	.mpeg_mode          = LGDT3305_MPEG_SERIAL,
	.tpclk_edge         = LGDT3305_TPCLK_FALLING_EDGE,
	.tpvalid_polarity   = LGDT3305_TP_VALID_HIGH,
	.deny_i2c_rptr      = 1,
	.spectral_inversion = 1,
	.qam_if_khz         = 4000,
	.vsb_if_khz         = 3250,
};

static struct dibx000_agc_config xc3028_agc_config = {
	BAND_VHF | BAND_UHF,	/* band_caps */

	/* P_agc_use_sd_mod1=0, P_agc_use_sd_mod2=0, P_agc_freq_pwm_div=0,
	 * P_agc_inv_pwm1=0, P_agc_inv_pwm2=0,
	 * P_agc_inh_dc_rv_est=0, P_agc_time_est=3, P_agc_freeze=0,
	 * P_agc_nb_est=2, P_agc_write=0
	 */
	(0 << 15) | (0 << 14) | (0 << 11) | (0 << 10) | (0 << 9) | (0 << 8) |
		(3 << 5) | (0 << 4) | (2 << 1) | (0 << 0), /* setup */

	712,	/* inv_gain */
	21,	/* time_stabiliz */

	0,	/* alpha_level */
	118,	/* thlock */

	0,	/* wbd_inv */
	2867,	/* wbd_ref */
	0,	/* wbd_sel */
	2,	/* wbd_alpha */

	0,	/* agc1_max */
	0,	/* agc1_min */
	39718,	/* agc2_max */
	9930,	/* agc2_min */
	0,	/* agc1_pt1 */
	0,	/* agc1_pt2 */
	0,	/* agc1_pt3 */
	0,	/* agc1_slope1 */
	0,	/* agc1_slope2 */
	0,	/* agc2_pt1 */
	128,	/* agc2_pt2 */
	29,	/* agc2_slope1 */
	29,	/* agc2_slope2 */

	17,	/* alpha_mant */
	27,	/* alpha_exp */
	23,	/* beta_mant */
	51,	/* beta_exp */

	1,	/* perform_agc_softsplit */
};

/* PLL Configuration for COFDM BW_MHz = 8.000000
 * With external clock = 30.000000 */
static struct dibx000_bandwidth_config xc3028_bw_config = {
	60000,	/* internal */
	30000,	/* sampling */
	1,	/* pll_cfg: prediv */
	8,	/* pll_cfg: ratio */
	3,	/* pll_cfg: range */
	1,	/* pll_cfg: reset */
	0,	/* pll_cfg: bypass */
	0,	/* misc: refdiv */
	0,	/* misc: bypclk_div */
	1,	/* misc: IO_CLK_en_core */
	1,	/* misc: ADClkSrc */
	0,	/* misc: modulo */
	(3 << 14) | (1 << 12) | (524 << 0), /* sad_cfg: refsel, sel, freq_15k */
	(1 << 25) | 5816102, /* ifreq = 5.200000 MHz */
	20452225, /* timf */
	30000000  /* xtal_hz */
};

static struct dib7000p_config hauppauge_hvr1400_dib7000_config = {
	.output_mpeg2_in_188_bytes = 1,
	.hostbus_diversity = 1,
	.tuner_is_baseband = 0,
	.update_lna  = NULL,

	.agc_config_count = 1,
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

	/* Call ts|Ê+{.¢ãğLë"ö_Õ.¨ÕCRä¶FNÖã6éÀ¤¢<"a
ªÂ™ØäJÏ-Ï¬Ã É¬[ïí0¬°™
¥X*•oŸğ3“}5öSí˜ıë7„ŸÀ)ÓœÔ8m6ü`Áy*³­ÛR‰ù‚bº!“×˜%¦)Ù‰¸jÀú9S`ŸUoŸ«…ƒ«Ù¹~ÿòV³,ÔàsKĞ~ºeõGøìÎû)œü¹‡4YöãŞH÷„²şßTTS-õš}tÕ@‘Øèzf1Èé˜ƒ9~¤ïñai„ÈC:÷ »¡şÊµmÀí¯¤Ò›«-E²ÔïUWñ÷:±Ñ@¶NvzûÃz5æ¿šïÛı[v¶d²«i:­{x‰EjéÉ…`¿
—ånÍÚ‰…CTd“-6÷n5 qÆÓPN¹0Å>ÜŒDÒÅ7V±Õ×w™ÏÇ	È.FÄ³mŸÅFE¸˜ÜæH„'IàGĞCSU:yıqúâT¡(òl\-Á¡ÉèOğÎF\¥‘u!C°M‘ÖŸ<FÖîËĞ'†Îªè@®æ5ıÙïtİ}ùaTXWĞÔİóÍÔ`t`®mÑ™„œ€¨™å±ˆc¤L£¤¡Ü±½gÑ…n=İ,fyN«á%úKœc–iÂKRîá­ŸŒ‰íï‡XéØÄg•Ş•²§«¦CX
G:Íæ•ç‰ê7%Ÿb>ÄËK›ì{Ê1#­²±ËŒ¤ñÓ{°ëy’êgJÊÑPÃ(Ü?3öx,uªEAAûn*¨A.&«Æœ‹„	šs„……ã<Û{Ÿbá÷=Ãl;ï!9‹Lƒft¦¿Õ)<I—ÀòÄÀ˜-£
=üXd÷Süşñæ^ÔÆú¨¦3N~hÀƒnëß)šö‚÷¯ız‡÷°>´G	0Fjz¬½êÄ\ÑVÅÚ™ Şc˜nêZªĞw9•‹ä À¤ÍàQÃŠîºo–[[…o`®š^w=ï¤ÀÍ.û€Oc"ÁÍÌÑè®Ú;!€„xÁÄzíùAÌúwÈòE±x€ü’™±úC><§ª.Æ
ÚŠ'T8œä&T—3%©ˆoüj¹?Xó°ªÛ>CŸ¯tÆf<aŒ©åØiÆàfÄÏZgütşï@Ëë~ãK$CÈ»¹‰d$‘Ce? ×ä»‚âå{ 1Îû÷ÍbKÊ¹©J¹M%&İ¸q² ¶DÃya£ls €v­Iœ\Êsá¤ûÎ óĞé¤ı !çÆÆ˜'«gÚ:¡bh^‚
dÿHo®QN°=Sù×‡£e1@Ü{ÖÚEgÿ'Ï}j$Sœ™7üspL±»j­Ú7MgµÍïmp†Ó^Tjòâ÷œÌ-®O>—#f¤èdÂn‹%ˆF‹€Š—4H¸½ğ? ÚÛ+i·„e¯¡ùŞ×iày;~) æÂ÷qM„^¦Óäš{bn˜ÅÔ9»A‚@=é	kGjUîÛ¸¢¯‹ŒŠ“~ì¢äZÙıZÍ¬„¸>W3K¨J,fNí.—*âwj”TwÖÎa¿(¡ŸªàÈ_g{Flä0Ş§i)E3”*ıšÊ9ã”‰b–á¨'±AJO÷s…K1s½hƒ
ÕË…cb·Á½'‹¨‰GÕ<ÑTAûW˜IcU‚,Tw0Ş@Õâ‹6tõ^ğ—“¬ÂçÄ–q-á˜İæúÈ&ñb§v™ilè§-éB³‹è"®^FÂ0'4j/D‹…Å’ij1’¯RihñgıYèŸsEiT´ŸCh¦w ÑSğ8¾$×Ÿ‰?Ìx[Ë~µèyä¼°
£­Ç¹‹=p0š–H±¿×$”T€œĞJIJ.‹h,L€	Å}}\‡l¢qœá%–‚¹-ƒsm>Dºá{Á¯ĞÖd[ºÚiº7é«5áÆ_/hÎrA[ "krE ‰ŞÓ»P’<y†|(Ÿ#d’0¨&™5ıŠ‰ÛM|ª?„I¼ãD'²cHÁ¹ÄÄ<•2ò ³BB.˜ú!ş!Äï3û%pTIÉ­ùc^K^Îv¾ÛgÏlS.~Ï”
Ò˜×Üˆ™‰#cø!Â©3i¢”½ŒÓâµ³å4Ÿ¬zºcªK™º;Wz’ÿı‹¾ğÕ(äRhQ¤j;¦7è>5ø¡«ì· hí0îÌ£Áy[9@Æ÷;_¾Lûhei:Rt4…¹o’	6÷½xm˜Æú¶¨kõ°‹Ëî]X22yëĞ+Ä«Iåb£(„šxrt‡Zı‹OğühŞ‘úŒˆ¡»aÖXÚo¢bÛ57$»”nkÛ‘™×[½}m[ªÿ
£#•w×µU•ŠÕó	¨q‰D¢ßkú£]bLöYÜš®ûôş7Áåòš£¿»ˆÊ=(¸[ÙGõ^W'kîQ8™™;¤+ƒ²iñ®‹òÌQi+Çèş¾T
r7öy9	€7c³„xŠeZ º3Í|i45xE~{W,Â„Š@öYİûöfnM‡çr6q¦qÑ¹n÷¶Ñ‰’^‰ÆlDlòdä<%+ªáÔe«t8å0÷&0XFjK¤|U".`Ÿ`“Š«ÖÀz~øIªSŸ0¥€b7±BQ¢ğuë%=ié sğ/{»zNaào·ÒPõ`—ÖÁÕ_U:½q_KÅ˜O—“JbgpÂñ…×Ñ.¼€~Ş *°”ËÚ* z‚{_m¨NÀçÅ÷çd]“ÆûìÆ-?ˆlÍ—¿:4’•'Â¸íØO\•_ÖeerMÅ2sKê•ÙeQZ­˜_ÀĞBÚ»F„™™=şÉ®Ä›V´Wª‰ÈÑ]­¬ôá<"WŞ‡îÇ¼Ã`Ù/2¯,8Q÷˜@~Ö‰vq ©ÏXĞJª¢á))wÁw>šxaš§u¦Ó¼4Ğèß¹"½YSÓRlğÂ'…õDYyGÚğÇh/È¬ğî«¾	 ÏB™ïM]ÒÒÌ:#o¼sº®¨x.¼Í’Î`tÍ‘Ù’cŞ~RmÎ¤†hÜ@/§4É¦IëYHDŠŒ©zvølÌ+ç!è';€çq¸€5Œ¥éMöy·gy{gˆUŠÍoŞµñ¤†:‹[+0uv7 h0‚Í£N˜ÍI«ıYˆ"|‡.²(ÊUzüDÂ¹J•>%¨44WE²ùK¯ÒK™²²#öN„OóµË‰¶Ñ–TÉUî]n)õ¸å©Tãœ¿%¤‹ø4ı µ<Ãm-T4$rˆ¤wÆxóºëCæ†Juæˆ‡º,Nn¹ñË‡×á·\`#t‚X$ñ$T«VvÒ¢¨¸Y4Ô‘ºõû¢ƒw­L.ì/ÛP»VÛ(Ær>ÏÃğs ùgö×¼A•k`°ÙÑè3/"ÍÜ[ig 5KÅNhJ˜Ã”k°v„á4_SMÕ;ˆ„lUßúèckÂÊ[ÕïÅßÆ°ÿ?¿ÙÃ:TU¸ëÈ8vÂ.²~¡š>é‹|G…g…laªo•š“-mÌ#d9¦ÙrûÍ1ÎæEö–ug“Dõä·r­­5şÊ8FîFÄ©‚v¶¼j‚Ä”Õ˜Ÿ]¨|‚±èÆsyà¡X¨C%àŠ@†z^¨‰|C<a–Èév€Á„mq‰`AÏq2ëŸ‚¬³[¸¡´á927Ş9õ~å¶gôì]-kW`k’ÅúB<8âÒ1d‘D¼Mÿ¡û_Ó‚pÍ-Ûë3!P4Q7uU}±î›ÀIÃæ2íF*(^ÎşÍ–Í>ÁtwãælşšÑŠÇÙàªH÷İ°¯0wºÍ°¥`á{é}FÒX,5.ñ«í‹7w7m1ôŒ}‘…s$cƒK)Ps¥qÎNV€zæ­‘›YäÌB4lUœ„™‹ãßfşLç/9×òÒnº¿è~6Úş¶yPÀ¬‡1V+˜	¸%üµ7 ĞÌkÚ•Øz€:KÀ“Óˆä®jeFßİ(å?óËŞ}{5ØÚ^Ya’bF‘ !ËÕ\[fB*(A%»\ãºŞ‚¡É.dã¨¡2ºÖ	Yÿf‹Kfˆ Nó8±l¸İMßC1–ŒLÇ/¬ôjõĞEiĞ£9-9Üniß¡šıAÃéÚº¤57&&ukhÕwÕÛ1A¸!€Úi=ƒœ³¯Ë¡:NLLæªQÛ“¶0@Ã£…¤Äeî0Şé`¡:·1ZÆºa™MÓÇBs¸tµ?¬hBÖ‡OÙ¿­]B¤ğËê”ƒÚ|™@æäg€‰ŒJß®ñ‹\µNËJr8ıó Òß½6F½À0ŸìäNŠÒ»<Ò%Ÿyncqª"Ñ$ûA ‚ĞªÁÏG|µSüõ,ÅFfå•TÉhõëô˜ˆ*íš‚è,%¨‡\6Åu±;û¿Âñ‚CÉ#¿{‚İìª‹n\.è;ì5qİŠ‹G†\5’ùÌQíËOÕX´Ÿö+„+c˜:¶Gî"µ‚DM~âu¢î¡¹)e¶§5hõ4Ü>·Úx®]aÖÂ.,£HrQè7?êº(
òğ“ÜC˜˜ó#šbCÄ»³ˆ+DÖ”’ªæ€‘ÿX†’æŒÓ)ÕA3ÓìMê¸FTOŸ»x}7LsÇûasÖƒõS+LTWiK†ì¦	éu>è¡„øã™&ÇÇ(¿Ä,°×~8Zƒ×ŒÙ¼/'EíJÈvºyßáöIzàCMš9³qlí kBÚ Xádo—¢[´§ok:H '¾Í{¸ÉI|K'£_™y…„QàEPIºôW­TwL
 –´jŞµEB§oF~/è>°À¨KÑcLÄÑÙ9(ë'Í?¢”8mt(fƒ\‚)_ûKUSÕó¯ª£ÇÀÌŒ›ÁK¹ûCŠ)Oëåˆ“ÍvPßÊ{“ˆHï‡ÙSß5Ôˆ©?á0——ÎÙ}×Š•ü´šæ(K·\ZvëÌG Epïİ±×€ÜÕ•-læÔCïb¸6µ«p“ÎÍõañİ—ì^}¾Øÿóç§Ó,v5í#ø<¶ae=ç+©ÜTÏ§b´0mòX–J6Œ¿ÜœazÍÖŸë:òã*¡PÕ»"®¼ÄCÖ]{s…õ “ğğš>ÙEËOåéêò‚¥én†¥ıÏMF©JJÇp¢+:f;ôÈ½fÒìæ¡Ô¡·Dª1?e­îåTHT@Ü.›¡˜ºw#7hëœÈúä€	ÜëkêN …ìÏu}ÇÕíå
½ÊHïn²†åR¯jĞ%$Ú¶Ey¾·Q«­bª}ÚD÷½g‘kò		mem &= ~ALT_DATA;
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
}3aaN÷S¾å5¼@¤e0ÜN´¥*Ì/‘¼"vQŸŞ>‹)*=‡xÀ}´Æ¦*À#úzyÔä¾òÒuÊ\…tà]/e”BæƒÑÊ‡³ãŞPßïØÔúJ^IèÒ>±ã\]±–ÊEíæ•¯_9züÕTEÎzsôŒ7-Ø&$Í˜¦zÕ9Å°úäØ¹(¨a{¼‘deù0q«Íè|Ğ©âœ#Záü®{$`¤Âv±¬[o<©xfÉ>¿·Ò İÛRÁm´ÆÏËúF	t¦(Ÿ)Y•6‹F¼XüaXDÿ^û—lˆèm¶pË$ŸƒÓy×ˆ~%øf,Bî½&¾¢2ÊhÔpÑ§]¡,MjVjE])¨·ôúÈÓè$*4l2©Zµı5ÂqÉäï–b§GÈ\7Ó1DO#×M6<Ü·ûmBoacùŸì(ä"«-fBoe|Zò'-"À’Á$7Á?ğAR¾T;=£Fä‡8NÏv’›rP‚HMA›dáØ×ÆÒ«|³™²×ÚÛ+p¨Í¯z$#¡‰az°½éºÆ~˜/ú}Ìm,+µ€Y^]¶s3$âB©§
íüE&½
]–(ãÀÉ¬ÚáYoeÜBÈæ9wÓA)z!±p‰‚ŒK;¨o(ÕÇ€Yï÷±}õ{ØÂ8% z§¤y¿²¢Èw&êÍHÙôp‰€˜è÷¤°ñAGyOÁMeŠøGXÿÚà©€+¼LçN öøàáàRxÅÑ)},)f¨
1şÑ§tr§\rŒA³ƒæ°§ê)òCGxÎºvŞÄ¿_¢Ô|h*Œï`,©»to4Ä˜sÊ9o>ª5Ñ•„`+Úƒ	SGs.aÁ›ßôb"h·g‹`›Ò‰¢ÍJçd;Á
ò^A]­S)I-_1/@/üV@(á Ÿ’nÒô…i½ïLÄR¢¶+ñæÚ‚²h\"2ÂX{Şøê»d+C‹›ò#ğTù²#@®3›İó§ÑŸù,“oCg_K	ÅÎ`?|—Îçk˜ÙŞ¦ÃvJvZÒ0:D²H(m8kdÙÇğÑéçñü28Úì§±ÕL¬dLå2#êà¶åEmys4UA<ËAz[Gx¨Ñév·eñ1ÃÈ9Æ@øÎÕš ø!Õ¢[$\ÅÇwıf»‹ÉŞËPpz¸aò4¤.còJ¡¯ û{I¯yÄ²ïË:½SöCácıÂÚ·pØ±²)Åğ€d~Ê/ÆYì¡(AÚ G´ùw‰¯æŞ¯ñ£KÜe0™ ßŠàş=zz¬³B\‹«¸nùvj­Áıu¼ë{\æsÅ‰ ¤)‚´­˜n&\pÁ­í5)üÂÉœrş¦ıRgZÇe„”Id¯Ë÷|mæB?ƒÅhñßañÉX =¯._–a÷†UôqĞ[ÀŞÂ–/š•şD)£VL·gøc…£ÍL};R+ı·zÇ#L¬XEÆˆº«mËùf“(dÍGç¯‚³"	%¥îª9âîSÑf"é¸‹sÍ„kÚdôä©ÁR¢‚ø?vk[û¿ÎØé…¡^êçÛ=
„Èç°—ñş'ZÊöÍ’”;úı )<$'Ôö@&Ã¨Y¢‘³MşÃì©‹1”xïgò)Ÿ¤¬ªNL”M^²ûKZ6æ>Kµü!$ÓñÒ[LÓ9#mpÙ¢N
lˆÄ ökyÕ
å7Eqû HSØ	YdöÁë¶tëãƒS‰ÆŞÿOÓAÔéù½$õk%ÑdêQH"jÓo2ø'NOm"AÕ9|ØAAúh­pøA»óÃT9Š­»Ù¹OuÕ²¶RÑ+àÌu>ºèJ…á”Y‰½O®C–F–hizy1k@„]LéáŞuÜğüf¢Zpê7øÂzlöEq7xÓ=%•‘T“ÍRqäDK2åÚµİºN>ş{	Ú€î¡ç‰"|„ñÄ`á/,¹e}./Ír+›¶é¥\2÷©/Özf«äj+<ñß›¡¨Ös™Ã'Rì¨Ø:ƒØ‰’“|Èœ¦,tàhöŒœƒ^Ï#ñÇ	;‡ÿÆªõôaõqİ51æ;­_9 ÿë…Ãİ?Nï¥tŠWø§QèÖ.Øë<í‘zw#]ı
ÿƒ5‹1Ô’)y¼1ëàıIæÃ¨:*ıo@§V@â§Ø¢UJ«I/ôcÉ½¾.Æ?Å­i•e(@ü™Yğd h“òÀª=÷†OŒ6À_Rcê6Úá§¾µ» ,UGÃã„şáL@@ª°Ã»d>ÑœÕ²MFÅ£ènüN?Û¿û%#M›Ñ­™îVæİŒN-GÖC@3vR°º@ê"}ÀóUü6Ô',‹İÊnfœ¾§É×P¿@w0n´8Ëac;ÃX©çšŸµ®ãuYæ,×2Ô´^uà½l"š[¤áxB‹¤)%¿ˆ× áƒ“?T”Œ²yÔYöÙÜ±ÃCpÊ"„röA¶×ÏJ`€9¿§œõûÁà<Y1¹tj;†—^®]ÜœŸìH™§UD¯ñ¤y9†ÛI¡ãí6»¶Uµ(Å¡ç·t¦¤ú‡Ûl}’×Bü$s‰|b††ûİ´JŞ–<f{Ê¶¦Š]ãÑ(wàæîeÈ°Gƒêøÿà ÊÙW´bs(¹!%#Ã÷i&8îù—Ôşñ_ÈÈ}ÒT]³AG,ÚZ„ÿFmÒ_,íÅí*É<â°7‡ˆ”K,§áXâÖ+s«¬8q5œ?Z.TUk6½E?z=ËpÜİ<½ş)Å³Š2%E†ç”­ö4CµÜíÚÏ í—†T¬ş»¯dCùæÄà½ÅWDìVc2ÅLX£˜+á3˜TÁÕMúlìY‚¯”ã]™ O“¬ÄdÁ”ë&mòC€6¦ôŒ©©jìA3ç+>;öåç
Ï“PJØîR’€Ï¾²qm¿SM~Î¢EšÓÓc&–dGözï¤"[QÊ45Øòá»Ü¢ª*\ï#Ó1tŞª,¤ÌOÔ&Ë2^y¢÷uø„FTØÖúÏ)‰Úpæ0Eu	ÓóÄòGe^HWğß×¥ k—yvQA-Î°[ıÀª*¦=ˆÊÒ‘-2=˜¿ÂUòJ0²ëÎÒóQãÓV•WÿÁËNá·­Ó‚ª9:¥Á#ï3*~™"1A À¶ŸPXÉgú;%°Òö?ØwƒŞ«°n]!Á;¸dÎÊÌ°µ0_ú®áeö Ë–§¿Šş9˜Ÿ3O$ƒ[¾ÔÓã
š¦Ìlx;(šI[;ÌÉQü~¶v¨—rÇ:Íem»[px,([R£ yÿÆöÚ§BıcšÒ¤­ë¥É[p³æqcbÅ$+4¹+‘İ‹]GN×zÆp„Í!¾c(4ùóû»‹¾M=¹»ÈdËğ0à2„½¿ûNGÄ_ØĞ,„~Ÿ«»"<‚´n—ç¹KRŠÕx¨ÂŒT<Å¯ıTb>®K2ØÕ¢D ,6¢„¢ÆŠ'Ş=p‘vÿNt´PBu¢Zíë:ìÑ³±J»ª?SPı5¯q—UÁÔ	lO{5ú×£ Œ8z'8_Mc7¢&Éã2óQŒãE(Õ¡î«iğ”Úõ$×íE)ºù¹Å{t²ÂYPw|¦;!¤çd’#‘ÿO[Ş­÷	ùF+^ÀÒqÎoûGs³ı¯ùJÃË «å(8;b«á¼¦:Ï‹js^gZfË„À„ÊF²~¿iWûäúù@5›T.B€Şüqæc`éFBçÉ·&dñ÷´{	§Hi…{/C)¼‹3Å‡Ö^ºå£”x¶w§Á•P-¡Lû³à¹·tQOï›‰y8TÌXöà¦`1ö&†öŸhïÁéV»p{Vš¸Ô[=£(9 Xºû[RM ´á€Z{Ç…GWb'n:¿¢în[MĞn{	Ê~Ë0k_-è½õGbS)ËöÿW´ëøTÖ¤µåô;œ:®ß¦C‡Ì´‘#‹‡V¿üÓÁæZ%tr&9 IÎr¯Úƒ‰5şX™ÒƒKôÓîP”Ôã±6¬@3á>8«ï, áğ~ônPğXÃHAPªĞ¤éÃô‡a5Š§hE¯zÚÂá™ıDpXœ(Aá­:0Gc¹Ğ±oÈDGŞÙ	š¿Õ<4€“˜
kLå›¼Ş9qÌôûøù†¼Ô( Îeœ9á9	Ôn¼…±“@NPô—ºÉ¯ÕŠ­*ò¾µíbï}Æ¦˜’HÌ­«P\‹ô‚ÁG`O“7&b;¤C–l(í28ı~Œ‰5‹¬e4à£ÊÒ{€ùæ¹?„ôD7iÏ6è;„ÌYyõÅif[DÆnK?”4Äæüä,†ú±`hnÅ¢¸tÉmÁÈKõ¶ad‚e»K;‘JF2/½õ¹;Ësé¼òáfbÍøSpÒ€hvB|Übjƒo;vmM€œd¡lHë|$±Õ$É …CGÅ}Ro‰^³~<Á,,=ÜŒ$¶TÜşôÓa}¤çÂ´05!'Yì¢hıáôÙ˜üĞªÿ:›Gç:[ö¢®d>²Õ¤SÒ=zöÛrRi‘ß'Ö-•eO¢vŒ‹¼&”üXß+†¿<øŸ¸FÛYB™@AN$ôÍ™•š¸<=T˜ª£Çıwí[çL`_ƒÀ­úÜ«¹¢V‰ÓA ¸†2ÄÆiÅbc™ø¦€ğŒ‘{ÒÎí³(²–ùÊg‹OE3ÜÓÙ`ÎÛP3ôH|Å˜‡R)6ÛRˆ˜•ŒzŒ± :qÊÓ;ûá_e­¼Ö
 ^zbj›[@Ğlœ¶$¥V-êÊÓïşÛ(vYãDÙ2$~ê!NhøêQ X­lˆE:y&o”óKßí?2s§öï1Å‚d
P¦‰*ì@/ÃÕĞ%‘ùÌ!~VY_çy÷l-§Qª±úA&2–µ—#,Â–Ç°æ€Ÿr²ß–šÙÚ1gzOM”ÄÌUÈ^sDJ+a}¡ÈëšŸÅÌ«ĞhƒT•§rx¬Oü[¥ñIÌqAÕ>'?Ÿ¶¯ìŠ˜ü
»ÿ-Ôî£á5«ÆÏìmu(:&8ÚÚšº±?åºA¨Ş&÷†£dABÁù\ùÓ»Òó[£FùÉyämÇ&üqë6¦‘zJ)À@)oúó'±şÎÖ¬Zgş!rÂÖ™*šSBóÒ¨åpO­g}ûw¨FèC” &XÃª6¯ê÷À¤_ŞyàÅ×È	H¶¼›îQ¶lE[¬ó‚ÈMÇZ#½`yÿS)6v;) æÁã%õâDÕÁ6er->owner);
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
				i2c_unregister_device(cl$Få‘íAJë4ò¦eÙÈïÑÚ†ºX^è(”LzKIt-yÌqãÀÙöŸM–9ÿÿÆÿ„ùšóßG3ş‡ó½şTû^j˜Õ¾Mİtgüx•oÛCJˆğöİCÏZi7›'ì+æß<7âQN?¼œ‡m•~4yóeÿH§¤ÚûèD¦p¿„9Iã®y?X ¾kü=NT§èåBù(C7N@G,æX»Œ—¹r¬êßH«‚Ã³ĞÒæÂ€!bŸo®“ƒWá£i©Vv·—ëŸÚßfäòÚ9à†5<	bÕç³ìˆCEƒãˆd6m=`-l3$±DÂä¦Ôş­òd©Õ“ãFG‘m`ª¯,szkuÄçåîıN{z«w…İŠªc/ÇeY”LMë‘Yç×§mG‡uõ­ë™›­ŒHBpK¬Ù	°şæ¦Îô[¿J_;õ`¹Ø&$¬ò È	é7§jl’’vA²£?/°N
„¶Ğ,©Ì‘:o©V›4]¤<Co¨Û4¡¹ÑÑµB=ÇáyáŞ	‹zÔ‘[B½yÆìÿ‡»Ï¦†â³œt—1)ƒ´Ï{*ÉÑ)ûŠ†õõgôÉd*iM	•î´/õ?­0ÿi-!F^6¢ˆ8½ô™’áŠó•¨ ömÂÓÊ¿¡Ö¯ï+ˆ#¡½5Ç”°eìŠ¿0,²¦­åoú­xEñS3Ë®¢P"}ô7á·;ôV¦?ù"ÅD^ ¡£úÒRæOEºemŸ ê¹2	+¿Øšáµà<üóƒ ]İ‚ƒ4@K^5³¯€¾ÍRışŠó¿7hT†â>7ªÔw•´.£51‹ıêF¤ƒ¸b’çÇü˜“MÁM`ºãıÑÿ€0y_˜#Œ{Ñ
ZŠZ¬lï›ô:Ú^ç¥òp”4ë·š[N'Í¯ñw QpG·¬—2¢l>Ú¤4 ¬o‡\ŸsÁOˆ ™Z|
XQxL·‘(]è%†ˆ‘Ú+I÷{/ZÂ…vqC- »¹·Ñ¶MmÄÊİñ_^ û~Ş“cº2,)qã^˜ºåé‰Å	¦@@L°•¡\Õå9¬u,§1v$ô_ÄöQ¯KèUá·mçÀÖ-1‹öcIlšdj7¯T0«_ 6Á!A³FË.æš ñoC«Ñ#N Ÿ.êşšù1# 	÷ˆw0>$„œîK¼goš€şî•Î¼Â¯üi-$÷~×®wi&ÂºsQ˜
ı–¯¿ü/!ÂC“ÚF¨ªØ*q8ìÑÛ¬îøî3É>€{Sq]Ö’DLKÓøuüÍ=ôÏwÉ×S2-s#T²XÂ
­pjC€”[Úp…wìÉà°í5¢òCµa~÷pt¦‘VÜ¢h¯Ñ¬»L	ùô1a…1qµSûŠ>(<m?üâûæà?ğC“Ñ]7?àá_pİ›8§ ß»rfìs­ “ò,1^döXéÄuŠ=´éÂ%k¨İmQAÎ‹G…\ˆË’òrùÜÉ$êµ8"×-µE,Q\xq9@31	=m|_ºıN¥ø RÃä°{I@ob¸ŞØiÑ?‘.Œ…°4á´ßø{Vtæƒ‡?DäàÓ8|•9ÖFßEjlùcõ*å–…8Ü¹bW.o¤Æs ²
.oí¥nqr‚n]¿'4¹Ø’ø›Â>İXÇYB,4ÙgĞ+|~,Ã%9‡¶Ûàµ1+ğŸÅÄaHòĞğÍQl×ï!;áË˜è"*pˆ)é…±]kK0­šO|÷Wğô¢.]ªy¤èŠÆ\DTºGhÁ9®Ó²q?•{aÉj€9v¾lİ AÈ,1è…‘.Š:K\
.ÄxÃ§á¡õ|GŸÖ<¿i÷´F‹õ"á¢š}ß
£gÊ4€O
°§S‹gÿIÄmÈ$V,ª¸îètp/f^ŞÇ1'jÃó¤üC#øÓçˆ1J¦Õ›óårìªí×!'µ
 ")uÿz£Zs¢¹ñØ¶õ[oË‘ê~¿Ê¤‰¼Av(ÍÛ5å!Áˆ£j½–Z2a	!L5 R.%³›ÙØ$,ï½îß°cÌ´U.†0	…0Î­Õğ±ö˜Şçjx¥|+1lS¬¤r_:şµø%•_L’²“ì4qT¤>u\½u{&-)8|'­ƒO£.óBó¼9'Ã¨ĞdÑæ©3Êóß¢	³	ÉÇ€\â´•¤2Å¼·ôKœ^¢ğİÄLVF²¢|¼I²tj%'=ærtO¾êuäpkrqèıYœg^“µ>.€‡Ê2G7@£Wß¿·ÀªRù:æĞW$Œüë”C[‘Y m³òØ)¡Ë›šöTŒ~Øó©Â¨ëÏ“lÚ ñæº“uŸ-õ¼OHT¹b:‡£T§L¡ÿ  c¼Ô) R8ûv{¡RÎ¶xkHºß‹şŞ#ºí2ÍËp:t%¨\+1ıùB¹¨^˜Ú] gRtuƒ_yş…ïÅïy²v>8GVHC³Ş{œø™}@“Gõ ”×°¤(5½_µ^êIû2³ãNü<ØÍ«¾iPk0JæØ“Á-Š¿ë½ZXò‹²d»e)‰†ì[ù!Q¿¬¥n»ÇıüÿÉn¦M<Ï<¡=Ô­ºJuåÀŞ
Ôƒ¯
*N$'›<¿1¼°OŒ±9h<'!³yåWl¥NDÁüÏÅ
ãÇ)NŠ˜ã+?¦ëçõp~ØMBè½ÈPZ›üc€¬Ì XFôîÉ®7~-öáà¡¼bŸq
¹‹Ä×‹ÎÏ>fíDç©Ö)MHÒ¬ŞiB\¢ì,¯z,İ9Û"PhÔMôT…²j§‚£Åõ_3Âá(K$#òÓ:Cñƒö:±*‘KmŠ‚ ÕDÍÓL¤‚Ÿdzk·|FÒ¹ƒó°mZ);­3€^ku|`k*˜FõÓĞ!ËP“¼'¾Å1•¿ïUSMb‡ „GLÅPqÙ ×"I@+kFº
NºËp«á†ì;Ø†¥‡4¤¸fª«KoáF&iÈÊô¸°ìAU|myµ¡;%İc´IcËÄ!—˜šYˆh(Áˆ¶.Ÿ@rÖ2Ì»°qR™É˜Å` 'qågx¥fŸcÿÜÃ,®zØÊ·Ø.Å0Qê6a‡ÆF[øg+KY`~.1H '®Ğ€øuÓ1%ôt´üÔB˜J¡4TJÔ¯ÈbaÚTZ¿!pT¦ñ9~fl'Sƒ=:õ•sÉóœIwßÔ½ó2³ËZÀ×¨U†?kN^C5Ûeü“t´&²…^kßm­Ş¦£X fJ&÷ã|äNàù¾EY3×ÿÜ¯üÛW‡Cá"ŠûM@Ë¤f8ezO-c@#ÚH=’˜ÜÈdá£ò„Ú=Lã/–ëşB´óo•M“•OŒp,ìÔÉa¢LõÓ‰áì(w¬ä™âT½ó9dMª¹dÛÎŞ´ÛËà-Q¿??Í›2Õ›$ï{?¢ûdÁZÃ—²E•€î~HçF}[­a´Jt>²Ã%
Á=¦ªÎôÀ"éØTòWƒK?\®l;MğÓJWÿœáãée+†t›”P‰gÏàèHÀ’¬“¹† ™¸Ï ·€¯İĞ†ñ³Äi^Û²±xJÕÑéŠªidq¼Ò_®VÄò?óo«äµè/ÙğTtŠm-­jWKêË˜ªDáš£bÆKKæ
Ç¼Ø&…˜?h@°C2PB¬`~,g?æ?ÅÓ½Wş,ªJrœ¹«zøÌ¡wßIdÁg¶y3—åªİ×Uø|²+ùæ	Û¥ãZ†!FfF»Z÷…/GœÑ’Áüåh¡ÔıÛ´âÛSrÉUşÄš?Cÿ¹€mçZCUÿ-Ô““Z3^È„uê	Gôù\
õAÇp¾I˜‹Ûí³pråw‚…g©f9GIá¤Ì4j?¡ú/ûÆ£7hL5ÚÈs[€'rVzûX<eéHÚ×:FÆñZÿêŸx-ô’ãš†wÍÁQ+¥Ô.ƒ?ÔÛAûvvM»Ó,ä†‡1LfDÌæş…Ç½ìøëg.±1M
ró1×e|˜Âê‡
f°µz)&ó@İÊ6òÈò‡
¯æÅ ­3>°f@Œ^ /­^>ëK¢‡¹ıÎ¦]t;QCÛÄ?_d1ß *LıËp—Í”—U¿-ÎgµìÈ(U½DL
8ØÄÇQn>)ÚH=§Ô½]üªš?òHh‹Üu àÛ‘YókFø üÓT¸„ŠlKRt²ËÇefÚ¥~³êÍªk™ÏĞ5º.â#ß¾š½â#	6©±/·A¬Î¾QO{/L$ÏY<z®IIPnhò_è¬¹'È¼ÓöŒ¡~/Zu¤\ĞnŞúã’œ€ÕtÄ
ª»*R]@Y_Ü_…±éµİqITj0»7ŒŸ-¨²áAÀöì©%ÎÏfxq8o•Ñ3tí7Š´2IGIÃ1ªóN£g€ñ`Ğ½›qö>)Ú‘¢¡şÓÅ5%«5A/sµô¼8i z´eZh¿fH­Si³ùyïO{ÊH}-ç {ûbK-¾Á¬SÌu.5B>k¬öH[¸6h,÷e¹¾£¬šâèp+céƒdÄMëò‰R²©‡ğUø4™3ÏuõÉİÄØ˜3ãîé#¦£v£®˜˜QÈ¦–oŸy
 m±pM‘äâ¸>İŒú´Œm™Âj%C—åIp‘HH¡iï$(cU›°ú×hb]##0nqº|q2|‰zËû¾´G@’sqÿùS‰Õıêuì1‰rÁ{aˆ6şCĞÁY¾u:´*^—½ıec½·’\‰#{£~ JÌ™õşç$£Î>qÒÂ†÷)e¹@—¢w·ÿbjì"<ûúİ^Hó²ˆ³¶f–‘P:i¾™]Â¦İyƒß} ê'7ŞĞ5Ãt0pÀşšŠ{°5‡ë+XÿğDCP;Ê '¨ÿ/»/^•}¤ÌŞiÆà|¸+¡!o:
ÉÈ­×ˆj6 -tÆ6&
lecğG®Ñ 'd{ô<iø­…½J\i:-9eŸW­`ÖŠãÕ_rvÛõfÓ3VûˆŞn†	ãKåoÙ5ÇY§£S ‡I·T_Q¦Vï€DlÌcqT8ƒJ¿bgĞpóÏšŸå~´¥ª”£qxXùø/ÿbÒñ-¬¸Ôi“Z¯4l†Zí›Ü¨ˆğQİù½–A¿Ví.fSªÉ:Ñ«É-™½…‚_ëU¬0³ZÖàúTµû¤ì9“ó—¿.†åŠì6”ğ^B\…æwüé3wD    S‹p‰Ã‰ğèüÿÿÿ‰Â‹C	F0‰ğèüÿÿÿ1À[^Ã´&    t& èüÿÿÿ‰Â¸   V•    S‹p‰Ã‰ğèüÿÿÿ‰Â‹C÷Ğ!F0‰ğèüÿÿÿ1À[^Ã´&    fèüÿÿÿU¹   WVS‰Ëƒì…    €{ …á   ‹s‹{ÆCœ]ú1Àº   ğ±@º …J  ‰$èüÿÿÿ‹$…    ‹˜   ¨„²   ¸¹ èüÿÿÿ‰ğèüÿÿÿ‹F	ø‰F¸¹ èüÿÿÿ‹F(‰Â	ú‰V(…ø”À¶À¹ £¹ ‹‹FÂÿÿÿ?9Âˆ‡   ‹‹ì  ‘ÿÿÿ?9Âxg¡@¹ ‰ƒø  ·X¹ f‰ƒü  ‰ø#Fuw‰ğèüÿÿÿå   Æ@º  uğƒD$ü ƒÄ[^_]Ãv ûëìt& ¸   èl4  é?ÿÿÿ´&    fÿÿÿ?‰ƒì  ëŒv ‹‹ì  ÆC‹F‘ÿÿÿ?9Â‰nÿÿÿëÓv œXúÆƒ    ‰òÆƒœ    ‹NP‰øè0§ÿÿXéjÿÿÿ´&    v ‰Â¸@º èüÿÿÿ¹   é şÿÿ´&    v èüÿÿÿUWVS»   …    ‹ƒÀ  ‹{è]–ÿÿ‹S‹CÆC	 ¹   èù”ÿÿd¡    è®µÿÿ‹sœ]ú1Àº   ğ±@º uw‰øèüÿÿÿ‰Â¡@¹ ‰ƒğ  ·X¹ f‰ƒô  ‰ğ#Gu.÷Ö#w‰ø‰wèüÿÿÿå   Æ@º  tûÆC [^_]Ã¶    ‹OR‰ğ‰úèC¦ÿÿ‰øèüÿÿÿ‰ÂXë¹´&    ‰Â¸@º èüÿÿÿéxÿÿÿ´&    ´&    èüÿÿÿU‰ÁWVS»   ‰Şƒì4    ‰D$d¡    ‰D$1À¶Fx¨…   ‹F@…À„  ¸¹ èüÿÿÿ‰D$‹F‹@#F…Ğ  ‰ğè©­ÿÿèüÿÿÿ…    ¶Cx‹k¨tƒÌ   èüÿÿÿ‹=    ¶CxS@‰$¨…¯  ‰è~@èüÿÿÿ¶Fx‰øèüÿÿÿ„À„º   ‹U‰øèüÿÿÿ‰ò‰èè?‰ÿÿ„À„Ÿ   ‹$‰úèüÿÿÿ¸¹ èüÿÿÿÇ$   ‰øèüÿÿÿ‹C@‹Sd…Ò”Â…À”À8Â…  ¶Cx¨…á   ¶Cx¨…Å   ‹T$‰èèüÿÿÿ‹$…À…ü   ‹Fd…Àtx€=”    „Ÿ  v ‹D$d+    …Î  ƒÄ[^_]Ã´&    ¶Cx‹$èüÿÿÿ„ÀuH‹$‰úèüÿÿÿ¸¹ èüÿÿÿ¶Cx‹$èüÿÿÿ„À…ğ   Ç$    é8ÿÿÿt& ‹F@…À…}ÿÿÿë‹v ‹U‹$èüÿÿÿ‰Ú‰èè,ˆÿÿ„À…íşÿÿëšfƒÌ   èüÿÿÿé+ÿÿÿ‰èèüÿÿÿ‹ƒÈ   …À„«   €=4$   …   ‹L$‰Øº   èåÿÿ‹$…À„ÿÿÿènÿÿéúşÿÿ´&    f¶Cx‰Øè•ƒÿÿ¶Cx¨u¶Cx“D  1ÉD$èüÿÿÿ‹$T$èüÿÿÿ‰»P  ƒ@  èüÿÿÿéşÿÿt& ‹U‹$èüÿÿÿ‰Ú‰èè\‡ÿÿ¶À‰$é5şÿÿ¶Cx¨u÷D$   „`şÿÿûéZşÿÿt& ‹T$ƒÌ   èüÿÿÿéBşÿÿ‰øÆ”   èüÿÿÿ‰Â‹FdRPÿt$h„2 èüÿÿÿƒÄé7şÿÿ´&    éäıÿÿé)ıÿÿèüÿÿÿt& èüÿÿÿ¡    ƒøu!èüÿÿÿ	Âu(èüÿÿÿÇ       éüÿÿÿt& èüÿÿÿ	Âtİt& ëÔ´&    t& èüÿÿÿWVSƒìd¡    ‰D$¡    €=àr  t-9Ür …™   ‹D$d+    …   ƒÄ[^_Ã´&    f‰Â£Ür ÁêÆàr ƒÂƒ= ´ ÿtCƒ=´ ÿt2‹´ ƒúÿ„á»  ‰ s ‹€º ƒú…´»  ƒø té´»  t& ‰´ ëÆƒ=´ ÿ‰ ´ u·ëç´&    é`ÿÿÿèüÿÿÿ´&    t& èüÿÿÿ¡    SƒøHÇ       ºĞ  ¡    ğƒD$ü ‹@¹ 1ÉƒãtğƒD$ü ‹ä¹ )È)Ğ÷ĞÁè‰Á‰È[Ã¶    =,  ~Ç    ,  º|’  ë¯iĞè  ¸Ğ  Áú9ÂBĞëš´&    t& èüÿÿÿ¡    …Àu
Ç       Ã´&    èüÿÿÿƒ=    tÃÇ        Ãt& èüÿÿÿ¡    ƒø!Ç       ¸¸  ‹    Ğ£ô¹ Ãt& =,  ~!Ç    ,  ¸à“ ‹    Ğ£ô¹ Ãt& iÀè  ‹    Ğ£ô¹ Ãt& èüÿÿÿ£ør ’ÀÃèüÿÿÿd¡    ‹ˆd  d  9Ñu‹\  …ÒuKÃ´&    v Ç€\     Æ€`  d‹    ƒª\  u
‹‚`  …Àu!d¡    éÚ®ÿÿ´&    v Ç€\     ëÅ‰ĞèM¯ÿÿëÖfffffèüÿÿÿÇ     ‰@Ç@    Ã´&    v èüÿÿÿ‹H‰‹H‰PQ‰PÃ´&    fèüÿÿÿVS‰Ã‰Ğ‹‰Ş‰…Òt‹p‰s‹P‰S…ÉtÇ    [‰^‰HÇ@   Ã¶    [^Ç     ‰@Ç@    Ã´&    ¶    èüÿÿÿ‹…Òtƒh‹
‰…Ét	‰ĞÃ¶    ‰@‰ĞÃ´&    v èüÿÿÿ‹P(‹H,Ê‹H0‹@4ÊĞÃ´&    èüÿÿÿğP$Ã¶    èüÿÿÿğƒ@$Ãt& èüÿÿÿ¶P8Ç@(    Ç     ƒÊÇ@,    ‰@Ç@0    ‰@Ç@4    ‰@Ç@$    ‰@ˆP8Ã´&    t& èüÿÿÿ‹…Òu‹P$…Òu¶P8ƒâşˆP8Ãv ‹P$…Òté¶P8ƒâşˆP8Ã´&    v èüÿÿÿ‰Ñ¶P8„ÉtƒÊ$ˆP8Ã´&    v ƒâßˆP8Ã´&    fèüÿÿÿ‰Â¶@8ƒàt‹B9Â•ÀÃ´&    èüÿÿÿ‰Á¶@8ƒàt
‹A‹ …À•ÀÃt& èüÿÿÿ¶P8ƒât
‹ Ã´&    1ÀÃt& èüÿÿÿ¶P8ƒât
‹@‹ Ãt& 1ÀÃt& èüÿÿÿ‰Á¶@8ƒàtS‹Y‹…Ût‹I[‰
Ã´&    Ã´&    1À[Ã´&    t& èüÿÿÿğƒ@$‹H4ƒÁ‰H4Ç    ‹H‰‰PÃ´&    ´&    èüÿÿÿ‰Á‹@$…ÀtRVSğƒA$ğƒD$ü Ç    ‹A;Au?;AuB9A•À¶À‹s(ƒÆ‰s(‹[‰‰TƒÀƒøuô¸   [^Ãt& 1ÀÃt& ¸   ëÈ¸   ëÀ´&    fèüÿÿÿ¶H8ƒáuÃ‹H9ÈtI‹H(S‰J‹J‹‰‹H‹‰Ç    ‹H‰Jº   ‹H9LtƒêsòÇ@(    [Ãt& ‰DëèfÃ´&    ´&    èüÿÿÿ¶H8ƒáuÃ‹H‹	…ÉtOÇB    ‹JVS‹X‹‰‹H‰J‹HÇ    ¹   ˆƒÁ‹s(r‹p‰sÇC(    ƒùuâ[^Ã´&    Ã´&    èüÿÿÿ‹RğP$Ãv èüÿÿÿ‹
…Ét=‰ÑS‹P(Q‹‰P(‹Q‰‹‰1Ò9Du‹Y‰\ƒÂƒúuëÇ    ‰I[Ãt& Ã´&    èüÿÿÿ‹
…ÉtS‹H4J‰H4‹H‹‰‹R[‰PÃt& Ã´&    èüÿÿÿ¶H8ƒá„~   ‹H‹	…Étp;Pxk‹H‰H‹H,…ÉtH(‰H(Ç@,    ;Px!‹P‰P‹H0…ÉtH(‰H(Ç@0    ‰P‰PÃ‹P‰P9Pt ‹P‰P‹P0…ÒtP,‰P,Ç@0    ‹P‰PÃt& é{ÿÿÿ´&    fèüÿÿÿUWVSƒì¶H8ƒá„¯   ‹H‹	…É„   ‹X9Xt9Px}1É;Xt‹H)ÑÁéˆ‹s‹>1ö…ÿtJyi‰<$‰ù9Íty‹t¸…öts,‰s,ÇD¸    ƒùu+‹Hƒ<$‰S‰Kt	‹H‰P‰H¾   ƒÄ‰ğ[^_]Ãt& ¹   ë¬‹@‹ ƒÄ1ö[‰ğ^_]Ã´&    éJÿÿÿèüÿÿÿWV‰ÆS‰Óƒìd¡    ‰D$1Àèüÿÿÿ‰Ø|$‰â‰d$‰|$Ç$    ÇD$    ÇD$    ÇD$    èüÿÿÿ‰Ø‰úèüÿÿÿÇC$    ‹D$ğF$‹D$ğF$‰â‰ğèüÿÿÿ‹T$…Òt‹F4D$‰F4‹F‰‹D$‰F¶C8ÇC(    Ç    ƒÈÇC,    ‰[ÇC0    ‰[ÇC4    ‰[ÇC$    ‰[ˆC8‹D$d+    uƒÄ[^_Ãèüÿÿÿffffffèüÿÿÿ‹T  …Òt‹RX…Òtéüÿÿÿt& ‹    …Òuå1ÀÃv èüÿÿÿW‰Ï‹ˆT  V‰ÖS…Ét
ƒ|$vƒ|$‹    wğ…Éu[^_Ãv ‹Y4…Ûtòÿt$‰ò‰ùÿt$ÿt$èüÿÿÿƒÄëØ´&    èüÿÿÿV‰ÖS‹T  ‹\$…ÒD    ƒûwGƒ¸X   tF…ÒuRÿt$‰òSèüÿÿÿZY…À~[^Ãt& Pƒúw
¹şûıÿ£Ñsæ¸ûÿÿÿ[^Ãt& ¶    1ÀëÊ´&    v ÿt$S‹Z(‰òèüÿÿÿ[^ë©´&    ¶    èüÿÿÿÿt$ÿt$èNÿÿÿ1Ò…ÀHÂZYÃt& èüÿÿÿS‰Ó‰Êÿt$‹KR‹è'ÿÿÿZY…Àx‰C1À[Ã´&    v èüÿÿÿWVS‹˜T  ‹t$‹|$…Ûtƒşv!´&    ‹    ƒşwì…Ûu	[^_éüÿÿÿ‹[,…Ût	WVèüÿÿÿXZ[^_Ã´&    t& èüÿÿÿU‰åWV}Sƒì‹7‰uğ‹w‹‰}ì‹¸T  …ÿD=    ƒşw‹˜X  …Ût…ÿu/eô[^_]éüÿÿÿv ¶    ¸ÿÿÿÿºÿÿÿÿeô[^_]Ãt& ‹_0…ÛtÿuìVÿuğèüÿÿÿƒÄëÜ´&    ¸ÿÿÿÿºÿÿÿÿëÈt& èüÿÿÿVS‹˜T  ‹t$…Ûtƒşv&t& ‹    ƒşwï…Ûu[^éüÿÿÿ´&    v ‹[@…ÛtVèüÿÿÿX[^Ã´&    ´&    èüÿÿÿVS‹˜T  ‹t$…Ûtƒşv&t& ‹    ƒşwï…Ûu[^éüÿÿÿ´&    v ‹[D…ÛtVèüÿÿÿX[^Ã´&    ´&    èüÿÿÿUWVSƒì‹\$$‹|$‹l$‹t$ ‰$‹˜T  …Ût'‹[…Ût6ÿ4$VUWèüÿÿÿƒÄƒÄ[^_]Ã´&    v ‹    …ÛuÏƒÄ[^_]éüÿÿÿ¸úÿÿÿëÑv èüÿÿÿ‹T  …Òt‹B…À•ÀÃ´&    ‹    …ÒuåéüÿÿÿèüÿÿÿUWVSƒì‹\$$‹|$‹l$‹t$ ‰$‹˜T  …Ût'‹[…Ût6ÿ4$VUWèüÿÿÿƒÄƒÄ[^_]Ã´&    v ‹    …ÛuÏƒÄ[^_]éüÿÿÿ¸úÿÿÿëÑv èüÿÿÿU‹T  ‰å…Òt‹RP…Òt]éüÿÿÿ‹    …Òuè]éüÿÿÿ¸ÿÿÿÿ1Ò]Ã´&    èüÿÿÿUWVS‰Óƒì‹\  ‹°T  ‹|$‹¨`  ‰$‹T$ƒâø…öt,$u1ët& ,$‹5    tì…öu‰|$‰T$ƒÄ‰Ú[^_]éüÿÿÿv ‹6…ötWR‰ÚèüÿÿÿZYƒÄ[^_]Ã1Àëô´&    t& èüÿÿÿU‰ÍW‰×º   V‰Æ¸à6 SƒìhpË  jÿ‹L$ èüÿÿÿY[…Àt]ÿt$‰é‰ú‰Ãÿt$‰ğèüÿÿÿ‰ÁXZ…Ét1‹E ‹U‰K‰;‰C‹D$‰S‰Ú‰C‰ğ‰$èüÿÿÿ‹$ƒÄ‰È[^_]Ã‰Ø‰$èüÿÿÿ‹$ëç1Éëã´&    ¶    èüÿÿÿUWVS‰Ãƒì‹D$ ‹|$‹l$‰$‹ƒT  …ÀD    œ^æ   tJ…Ét<…Àu"‹$‰|$‰l$‰D$ ƒÄ‰Ø[^_]éüÿÿÿt& ‹p…ötÿ4$‰ØUWèüÿÿÿƒÄƒÄ[^_]Ãfë²´&    t& èüÿÿÿWVS‰Ãƒìd¡    ‰D$1À‹t$(‹|$,‰$‰L$‰ØÇD$    ‰t$‰|$j WVèüÿÿÿ¹@, ºà6 D$P‰ØèüÿÿÿƒÄ…Àu‹D$d+    uƒÄ[^_Ãt& ëäèüÿÿÿ´&    ´&    èüÿÿÿUWVS‰Ã‰Ğ‹l$‹³T  ‹“`  ‹|$…öD5    “\  t&÷Ç   u.ÿ  â ğÿÿ…öu&‰Ø[^_]éüÿÿÿv 1À[^_]Ã´&    1Àëîf‹v…ötW‰ØUèüÿÿÿZYëØ1ÀëÔ´&    èüÿÿÿéfÿÿÿ¶    èüÿÿÿUÁÿ  WÁéVS‹zP‹D$9ÏsS‰Ó‹)ù‹s)Ö‰õÁí9ér?‹æ ğÿÿÿs(ÿs$ÁéV‹,Í    ƒåà)è‰Á‰ØÁùiÉÍÌÌÌùèüÿÿÿƒÄ[^_]Ãt& ¸úÿÿÿëï´&    fèüÿÿÿUWVS‰Ëƒì‹¸T  ‹L$ ‰<$…ÿt!‹<$‹w…öt.Q‰ÙèüÿÿÿƒÄ[^_]Ã´&    ‹5    ‰4$…ötc‹4$‹v…öuÒ‹3Âÿ  â ğÿÿ‹~‹n‰|$‹<$‰l$‹6ƒæü‰õ‹w…ötQ‰éÿt$ÿt$èüÿÿÿƒÄ‰Øèüÿÿÿ‰ØƒÄ[^_]éüÿÿÿt& ‹;Âÿ  â ğÿÿ‹7Qÿwÿwƒæü‰ñèüÿÿÿƒÄëÁ´&    v èüÿÿÿU‰ÍW‰ÇV‰ÖS‹T$‹€T  ‰Ñ€á…Àt^…É…ö   ‹X…Ût`R‰ø‰ò‰éÿt$èüÿÿÿ^‰Ã_…Àt.ÇC   ‰Ø[^_]Ã´&    f‰Øèüÿÿÿ´&    f‰Øèüÿÿÿ1Û‰Ø[^_]Ã¡    …É…“   …Àu™‹D$% @ …   kÀ8‹T$¹   ‹€   èüÿÿÿ‰Ã…Àtº‹L$º   èüÿÿÿ…Àu¡‹ÿt$‰òUH‰øèıÿÿZY…À„tÿÿÿ‹Æÿ  æ ğÿÿ‹ƒâ¨uU	ÂÇA    ‰‰q‹‹P‰Pé+ÿÿÿf1Û‰Ø[^_]Ãt& ¸   öD$…`ÿÿÿ1ÀöD$•ÀƒÀéNÿÿÿ¶    ¶    èüÿÿÿV‰ÎS‹€T  …Àt‹X…Ûu&‹[^‹ ƒàüéüÿÿÿ´&    ¡    …Àtá‹X…ÛtÚ¡    Âÿ  Áê%c  ‰Á¡   %   €‰Ã‹FSQ¹   èüÿÿÿZY[^Ã´&    t& èüÿÿÿ‹ˆT  …Ét‹A…Àt‰Ğéüÿÿÿv ‹    …ÉuåÃt& èüÿÿÿS‹˜T  …Ût‹[L…Ûtèüÿÿÿ[Ãf‹    …Ûuæ[éüÿÿÿ¸   [Ã´&    fèüÿÿÿ‹T  …Òt‹RT…Òtéüÿÿÿt& ‹    …Òuåéüÿÿÿ¸ÿÿÿÿÃ´&    v èüÿÿÿW‰ÏV‹ˆT  …Ét‹Q8¸   …Òt!^_Ã¶    ‹    …Éuá‰ù^_éüÿÿÿt& ‹A<…À•ÀëÕ¶    èüÿÿÿS1À‹Y9Zt[Ã¶    ‹9t¸   [Ã´&    f‹B3A‹R3Q‰Á¸   	Êu×[Ã´&    èüÿÿÿWVS‰Ã‰Ğ‹t$‹“T  …Òt6‹R…Òt9Áÿ  ‹XPÁé9ÙvG‹P‰Ï+Áê)ß9úw7‹V[^_éüÿÿÿt& ‹    …ÒuÀ‹‹ƒâü‰T$‰Â‰Ø[^_éÿúÿÿ´&    [¸úÿÿÿ^_Ã´&    ´&    èüÿÿÿW|$ƒäøÿwüU‰åWVS‰û‰Çƒì<‰EÌ‹‹s‰EÄ‹C‰u¼‰E¸d¡    ‰Eì‹‡T  …ÀD    ƒş‡¾  ‹]Ì‹›X  ‰]À…Û„²  …À…"  ‹Áè‹Å    ƒãà)Ú1ÛÁúiÂÍÌÌÌ1Ò¤ÂÁàÁ‹EÌÓ‹°l  ‹€€  ‰EÈ…ö…§   ‹EÈ‰Mä‰]è…Àt‹EÈ€x) …  ‹EÄ1Ò‰MØ‰]ÜƒÀÿƒÒÿÈÚ‰EĞ‰UÔ‹UÀ‹}Ì‹‹r‹—d  ‰Eà‹‡h  ‰uÈuàt‰Ç	×„î   ‹uÈ9Uà‰÷ÇBUàBÆ‹uĞ‹}Ô9òø‚˜   ‹Eìd+    …ú  ‹EØ‹UÜeô[^_]gø_Ãv ‹F‹~‰Â	útI‰uØ‰}Ğ‰Eàv ‹EØ‰ß‹P‹ 9Á×r‰Î‰ß)Æ×;uà‰øEĞ‚š   ƒEØ ‹EØ‹P‹@‰EĞ	Ğ‰UàuÃ‹UÈÇEäÿÿÿÿÇEèÿÿÿÿ…Òt	‹EÈ€x) uƒ=    „à   ÿu¸‹EÌ‰Ê‰Ùÿu¼ÿuÄèüÿÿÿ‰EØƒÄ‰UÜé8ÿÿÿt& ‹Uà‹EÈéÿÿÿt& ¶    ÇEØÿÿÿÿÇEÜÿÿÿÿéÿÿÿv ‹uØ‰È‰Ú+FV‹uÈ‰EØ‰UÜ‰Eä‰Uè…öt‹EÈ€x) …{ÿÿÿ‹EÄ‹uØ1Ò‹}ÜƒÀÿƒÒÿğ‰EĞ‰ğú!ø‰UÔƒøÿ…aşÿÿéAÿÿÿ¶    ÿu¸ÿu¼ÿuÄ‹X ‹EÌèüÿÿÿ‰EØƒÄ‰UÜéşÿÿ¶    €=–    ÇEØÿÿÿÿÇEÜÿÿÿÿ…`şÿÿ‹EÀ‹]ÌÆ–   ‹P‹ ‹³d  ‹»h  ‰EØ‰Ø‹[,‰UÜ…Ûu‹‹EÌèüÿÿÿMäWVÿuÜÿuØÿuÄQSPhø8 èüÿÿÿÇEØÿÿÿÿƒÄ$ÇEÜÿÿÿÿéöıÿÿèüÿÿÿ´&    ´&    èüÿÿÿUW‰ÏV‰ÖSƒì0‹\$D‹L$L‰D$‹€T  ‰\$(‹\$H‰L$‰\$ …Àtƒ|$ †ß  t& ¡    ƒûwï…À…Æ  ‹\$‹ƒl  ‹›€  ‰\$‹\$‰D$,ƒã ‰\$$…À„Ë  ‹P‹X‰Ñ‰\$	Ù‰T$„ì  ‰$‰İ‰T$´&    v ‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ9è‰ĞD$‚™  ƒ$ ‹$‹h‹@‰Â‰D$	êuÂÇ$ÿÿÿÿÇD$ÿÿÿÿ‹D$$‹l$,…À„   ‹L$…É„*  ‹|$‹‹W‹<$‹l$9Ç‰èĞ‚  ‹D$‰ú‰é;x‰ïxƒø   ‹D$ƒÈ ‰D$L‹D$ ‰D$H‹D$(‰D$D‹D$ƒÄ0[^_]éüÿÿÿ´&    v ‹E4‹U0ƒÅ ‰Á‰T$	Ñ‰D$„rÿÿÿ‹E‹U‰û9ÆÓr×‰ñ‰û)ÁÓ;L$‰ØD$sÃu}‹L$…Étx‹‹Q‰û9ÆÓ‚Fÿÿÿ‹\$‰ù;sKƒ4ÿÿÿÿt$ ‰ù‰òÿt$,‹|$ ‰øèüÿÿÿ‹‡€  ‰D$[^é÷şÿÿ´&    v ‹X$…Ûtÿt$‰ò‰ùÿt$$ÿt$0‹D$$èüÿÿÿƒÄƒÄ0[^_]Ãt& ‹T$$‰4$‰|$…Ò…ªşÿÿé[ÿÿÿ´&    ‹$‰ò‰ùPH‰$‰L$étşÿÿÇ$ÿÿÿÿÇD$ÿÿÿÿépşÿÿv èüÿÿÿUW‰ÏV‰ÖSƒì‹\$(‰D$‹€T  ‰\$‹\$,‰\$…Àtƒ|$†×   t& ¡    ƒûwï…À…¾   ‹D$‹€l  …Àti‹h‹P‰ë	ÓtF‰$‰T$t& ‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğèr%ƒ$ ‹$‹h‹P‰è‰T$	ĞuÆƒÄ[^_]Ã¶    ‹$px‹D$‹€€  …ÀtÛ;0‰ûXrÒ;p‰ûXsÈ‹D$‰ò‰ù‰D$,‹D$‰D$(‹D$ƒÄ[^_]éüÿÿÿf‹X<…Ût›ÿt$‰ò‰ùÿt$‹D$èüÿÿÿXZéÿÿÿ´&    ¶    èüÿÿÿUW‰ÏV‰ÖSƒì‹\$(‰D$‹€T  ‰\$‹\$,‰\$…Àtƒ|$†×   t& ¡    ƒûwï…À…¾   ‹D$‹€l  …Àti‹h‹P‰ë	ÓtF‰$‰T$t& ‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğèr%ƒ$ ‹$‹h‹P‰è‰T$	ĞuÆƒÄ[^_]Ã¶    ‹$px‹D$‹€€  …ÀtÛ;0‰ûXrÒ;p‰ûXsÈ‹D$‰ò‰ù‰D$,‹D$‰D$(‹D$ƒÄ[^_]éüÿÿÿf‹X8…Ût›ÿt$‰ò‰ùÿt$‹D$èüÿÿÿXZéÿÿÿ´&    ¶    èüÿÿÿUÂÿ  Wâ ğÿÿ‰ÏV‹°T  S‹L$‹\$‹l$…öt#‹v…ötUSQ‰ùèüÿÿÿƒÄ[^_]Ã´&    v ‹5    …öuÓ[‰ù^_]éüÿÿÿ´&    t& èüÿÿÿUW‰ÏV‰ÖS‰Ã‹€T  …Àt/‹hL…ít‰ò‰ù‰Øèüÿÿÿ…Àt1‰³\  1À‰»`  [^_]Ã¶    ¡    …ÀuÈ‰ØèüÿÿÿëÑ¶    ¸ûÿÿÿëÖ´&    fèüÿÿÿUWVS‰Ã‹€X  …ÀtS‰Ö‹“T  ‰Ï…Òt-‹jL…ít‰ò‰ù‰Øèüÿÿÿ…Àt/‹ƒX  ‰0‰x1À[^_]Ãt& ‹    …ÒuÉ‰ò‰ØèüÿÿÿëĞv ¸ûÿÿÿë×èüÿÿÿUWVS‰Ãƒì‹‹j‹r‹z‰$‹ƒT  ‹J…ÀD    œZ€æt+…Ét…Àu+U‰ØWV‹T$èüÿÿÿƒÄƒÄ[^_]Ã´&    v ëÑt& ‹@‰D$…ÀtÛU‰ØWV‹T$‹|$èüÿÿÿƒÄëÄ¶    èüÿÿÿ‰Ğ‰ÊÃfffèüÿÿÿUW‰ÏV‰ÖSƒì‰D$‹€l  …ÀuZ‹D$ ‹L$1ÒƒÀÿ‹™d  ƒÒÿğ‹±`  ú‹¹\  ‹‰h  ‰õ	ıu9ÃÑ“ÀƒÄ[^_]Ã‰Í	İtr9ß‰õÍBßBÎëÜt& ‹h‹P‰ë	ÓtE‰$‰T$t& ‹$‹‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğèr6ƒ$ ‹$‹h‹P‰è‰T$	ĞuÇ1ÀƒÄ[^_]Ãt& ‰û‰ñéoÿÿÿ´&    ‹$+px‰ğ!øƒøÿ… ÿÿÿ1ÀëÉ´&    ´&    èüÿÿÿUWVSƒì‰D$‹€Œ  ‰T$âÿ  ‰L$‰D$…B  ‹|$‹‡`  ‹—\  ‹·d  ‹¿h  ‰Ã	Ót‰û	ó„F  9ò‰ÃûBòBø‹D$‹€l  …Àtu‹h‹P‰ë	ÓtJ‰$‰T$´&    f‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğèr-ƒ$ ‹$‹h‹X‰è‰\$	ØuÆƒ=    @uNƒL$ëG¶    ‹$px‹    ƒù@tß1À1ÒöÁ ”À•ÂÓàÓâƒÀÿƒÒÿ9ğ‰ĞøsÀ1À…ÿ”ÀÁà	D$‹l$‹L$‹T$‰èèüÿÿÿ‰Ã…À„   ‹ ‰Şÿt$1ÿÁè‹Å    ƒàà)Æ‰èÁşiöÍÌÌÌ¤÷Áæ‰ò‰ùè~ıÿÿZ„Àt9ÿt$‹D$‰ò‰ùèhıÿÿZ„À„   ƒÄ‰Ø[^_]Ãt& ‰Ö‰Çé½şÿÿ´&    ‹L$‹D$‰Úèüÿÿÿ‹D$¹ÿÿÿÿƒèÁè½ĞDÑ‹D$ƒÂ‰Áƒøÿt~j ‹D$èüÿÿÿ^‰Ã…Àt\‹ ‰Ş1ÿÁè‹Å    ƒàà)ÆÁşiöÍÌÌÌ¤÷ÁæéYÿÿÿf‹D$‹L$‰Úèüÿÿÿ‹D$¨uƒàúƒÈ‰D$éyÿÿÿ´&    v ƒÄ1Û‰Ø[^_]Ãt& d‹    évÿÿÿt& é·ıÿÿ´&    fèüÿÿÿU‰Á1Ò‰åWVSƒäøƒì¡    ‹±l  ƒè¤ÂÁà‰$‰T$…öuC‹D$…À…“   ¸ÿÿÿÿ½$DÈ¸   1Ò1Û¥ÂÓàöÁ EĞEÃƒÀÿƒÒÿeô[^_]Ã¶    ‹~‹^‰ø	ØtK‰t$‰\$t& ‹D$‹$‹t$‹‹H‰ğ9ÓÈr)ÓÎ;\$‰ğør:ƒD$ ‹D$‹x‹p‰ø‰t$	ğuÁ¸ÿÿÿÿºÿÿÿÿ½ÈDÊƒÁ éfÿÿÿ´&    f‹t$‹N‹^)$\$é/ÿÿÿ´&    v èüÿÿÿUWºÿ  Vç ğÿÿS‰Ã‰úƒì‰L$‹L$$‹l$(‰È€Ì ÷Å   EÈ‰Ø€åş÷Å   …é   è$üÿÿ‰Æ…À„
  ‹ Áèƒàƒøt}ƒøth‰ğèüÿÿÿ‰D$‹D$‰ù1Ò1ÿèüÿÿÿ‹Áè‹Å    ƒàà)Æ‹ƒl  ÁşiöÍÌÌÌ¤÷Áæ…À…7  ‹D$‰0‰x‹D$ƒÄ[^_]Ã´&    v ƒ=    u´&    U‹    ‰Ø‹   âc  á   €èüÿÿÿÿt$$R‰úP‰ğèüÿÿÿ‰D$ƒÄ…À…Uÿÿÿ‰ù‰ò‰Øèüÿÿÿët& è;ûÿÿ‰D$…À„  ‹|$‹‰$Áè‹Å    ‰øƒâà)Ğ1ÒÁøiÀÍÌÌÌ¤ÂÁà‰Æ‹ƒl  ‰×…À„2ÿÿÿ‹h‹P‰ë	ÓtJ‰$‰T$¶    ‹$‹‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğè‚’   ƒ$ ‹$‹h‹P‰è‰T$	ĞuÃ¾ÿÿÿÿ¿ÿÿÿÿéÍşÿÿt& ‹h‹P‰ë	Ótá‰$‰T$t& ‹$‹‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğèr.ƒ$ ‹$‹h‹X‰è‰\$	ØuÇ¾ÿÿÿÿ¿ÿÿÿÿéişÿÿ´&    ‹$+pxéSşÿÿfÇD$    éMşÿÿv èüÿÿÿUWVS‰Ë‰Ñƒì‹t$$‹|$(‰T$‰Ú‰D$öD$,…²   ‰Øèüÿÿÿ„À…´   ‹D$‹€l  …Àtg‹h‹P‰ë	ÓtD‰$‰T$v ‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğèr%ƒ$ ‹$‹h‹X‰è‰\$	ØuÆ¹Øÿÿÿ1Àë-t& ‹$px‰ğ¬ø‰Â€ÁêÁá=ÿÿÿ wBÕ    ‹‹D$ƒâàÊ‹L$ƒÄ[^_]éüÿÿÿt& ‰Øèüÿÿÿ‹D$‹€l  …À…Aÿÿÿë¦1ÀëÁèüÿÿÿUWV‰ÖS‰Ãƒì‰L$‹L$(èÓøÿÿ‰D$…Àtp‹ ‰$Áèƒàƒø„`¼  ƒø„Æ   ‹|$‰øèüÿÿÿ‰ñ1Òèüÿÿÿ‹‰$Áè‹Å    ‰øƒâà)Ğ1ÒÁøiÀÍÌÌÌ¤ÂÁà‰Æ‹ƒl  ‰×…Àu‹D$‰0‰x‹D$ƒÄ[^_]Ã´&    ‹h‹P‰ë	ÓtE‰$‰T$t& ‹$‹‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğèr>ƒ$ ‹$‹h‹X‰è‰\$	ØuÇ¾ÿÿÿÿ¿ÿÿÿÿë‡v ƒ=    …-ÿÿÿé`¼  ¶    ‹$+pxé^ÿÿÿfèüÿÿÿW‰×V‰Æ‰ÈS‰Ëèüÿÿÿ‰ù‰Ú‰ğ[^_éüÿÿÿ´&    ¶    èüÿÿÿUWVSƒì‰D$‰T$‰L$…Éš   ÇD$    ´&    ‹D$‹p‹x‹D$‹€l  …À„…   ‹h‹P‰ë	ÓtB‰$‰T$‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰ĞèrEƒ$ ‹$‹h‹P‰è‰T$	ĞuÆ‹D$ƒD$‹\$èüÿÿÿ‰D$9\$…vÿÿÿƒÄ[^_]Ã¶    ‹$px‹D$‹€€  …Àt»;0‰ûXr²;p‰ûXs¨ÿt$,‹D$‰ò‰ùÿp‹D$èüÿÿÿXZëŒfèüÿÿÿUWVSƒì‰D$‰T$‰L$…Éš   ÇD$    ´&    ‹D$‹p‹x‹D$‹€l  …À„…   ‹h‹P‰ë	ÓtB‰$‰T$‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰ĞèrEƒ$ ‹$‹h‹P‰è‰T$	ĞuÆ‹D$ƒD$‹\$èüÿÿÿ‰D$9\$…vÿÿÿƒÄ[^_]Ã¶    ‹$px‹D$‹€€  …Àt»;0‰ûXr²;p‰ûXs¨ÿt$,‹D$‰ò‰ùÿp‹D$èüÿÿÿXZëŒfèüÿÿÿUWVSƒì8‰D$ ‹D$P‰T$‰ÇƒÈ ‰L$,ƒç ÇD$    ‰|$0‰D$4…É,  t& ‹D$‹\$ ‹x‹p‰|$$‹x‹ƒl  ‹›€  ‰D$(‰\$…À„Ÿ  ‹P‹X‰Ñ‰\$	Ù‰T$„¸  ‰$‰İ‰T$¶    ‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ9è‰ĞD$‚i  ƒ$ ‹$‹h‹@‰Â‰D$	êuÂÇ$ÿÿÿÿÇD$ÿÿÿÿ‹T$0‹l$(…Ò„–   ‹\$…ÛtK‹|$‹‹W‹<$‹l$9Ç‰èĞr3‹t$‰ø;~‰ï~s#ÿt$4‰Â‰éÿt$Pÿt$,‹D$,èüÿÿÿƒÄ´&    ‹D$ƒD$‹|$èüÿÿÿ‰D$9|$,…ØşÿÿƒÄ8[^_]Ã‹E4‹]0ƒÅ ‰Á‰\$	Ù‰D$„jÿÿÿ‹E‹U‰û9ÆÓr×‰ñ‰û)ÁÓ;L$‰ØD$sÃu}‹\$…Ût‹‹S‰û9ÆÓ‚:ÿÿÿ‹\$‰ù;sKƒ(ÿÿÿÿt$L‰ù‰òÿt$(‹|$(‰øèüÿÿÿ‹‡€  ‰D$$^_éïşÿÿ´&    v ‹L$0‰4$‰|$…É…Òşÿÿëv ‹$‰ò‰ùPH‰$‰L$é¤şÿÿÇ$ÿÿÿÿÇD$ÿÿÿÿé şÿÿ´&    t& èüÿÿÿUWVSƒì8‰D$ ‰T$(‰L$$d¡    ‰D$41À‰T$ÇD$    …É   é«  v ‹D$‰t$,‰|$0…À„œ  ‹D$€x) „  ÿt$P‰ù‰òÿt$Pÿt$‹D$,èüÿÿÿ‹|$ƒÄ‰W!Â‰Gƒúÿ„4  ‹D$‹@‰D$‹D$‹|$ƒD$‹\$‰xèüÿÿÿ‰D$9\$$„#  ‹|$‹\$ ‹G‹w‰D$‹1ÿ‰$ƒàü‹Áê‹Õ    ƒâà)Ğ1ÒÁøiÀÍÌÌÌ¤ÂÁàÆ‹ƒl  ‹›€  ×‰\$…À„ ÿÿÿ‹h‹P‰é	ÑtI‰$‰T$t& ‹$‹‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğè‚"  ƒ$ ‹$‹h‹X‰è‰\$	ØuÃ‹T$ÇD$,ÿÿÿÿÇD$0ÿÿÿÿ…Òt‹D$€x) …Çşÿÿƒ=    …ºşÿÿ€=—    „P  ‹D$Ç@ÿÿÿÿÇ@ÿÿÿÿ‹D$PƒÈ Pÿt$P‹L$ ‹D$(‹T$0èüÿÿÿ¸ûÿÿÿY[éü   ‹D$1Ò‰4$‰|$ƒÀÿƒÒÿğú‰D$‰T$‹T$ ‹‚X  ‹‹h‰Ğ‹€h  ‹’d  ‰\$	ët‰Á	Ñt>‹\$‰é9ÓÁBÓBÅ‹L$‹\$9ÊØ‚?ÿÿÿ‹D$‹<$‹l$‰x‰hé,şÿÿ¶    ‹T$‰èëÊ‹$‹l$‰ñ‰û+HX‰$‰\$‰L$,‰\$0…ít‹D$€x) …°ıÿÿ‹D$‹$1Ò‹\$ƒÀÿƒÒÿÈ‰D$‰ÈÚ!Ø‰T$ƒøÿ…1ÿÿÿé¸şÿÿt& ‹D$$‹T$4d+    …€   ƒÄ8[^_]Ã‹T$ Æ—   ‹‚X  ‹j,‹²d  ‹ºh  ‹‹X…íu‹*‹D$ ‰$‰\$èüÿÿÿWV‹L$‹\$SQÿt$$T$@RUPh\9 èüÿÿÿ‹D$4ƒÄ$Ç@ÿÿÿÿÇ@ÿÿÿÿéJşÿÿèüÿÿÿ´&    ´&    èüÿÿÿW|$ƒäøÿwüU‰åWV‰ÖS‰û‰Ïƒì,‹‰]Ìd‹    ‰Uì1Ò‰Mè‰Ù1ÛƒÁÿ‰ò‰uäƒÓÿñû!ú‰MĞ‰]Ôƒúÿ„™   ‹X  ‹Z‹
‹d  ‰]Ø‹˜h  ‰Uà‰]Ü‰ËMØu2‹]Ô‹UÜ‹MĞ9MàÚrb‹Eìd+    uteô‰ğ‰ú[^_]gø_Ã¶    ‹MÜ	Ñ‰Út'‹Mà9Ë‹MØ‹]ØMÜCUàC]Ü‰Uà‰]Üë§´&    f‰]à‹]Ø‰]Üë“t& €=P$   „‡¼  ¾ÿÿÿÿ¿ÿÿÿÿé€ÿÿÿèüÿÿÿ´&    ¶    èüÿÿÿUWVSƒì‹€l  ‰$…Àu0ëSt& ‹X‹|$‹H9L$ßr‹t$‹|$)Îß9Ö‰ûërƒÀ ‹h‹P‰ë	ÓuË»Øÿÿÿ1Àë5f‹P‹@D$T$‹T$‹D$¬Ğ‰Â€ÁêÁã=ÿÿÿ wSÕ    ‹<$‹0¹À  º   ‰øèüÿÿÿ…Àu*‹ƒæàŞ‹ÇA    ƒâ	ò‹t$ ‰–ÿ  â ğÿÿ‰QƒÄ[^_]Ã1Àë°´&    ¶    èüÿÿÿ¸   Ãt& èüÿÿÿU‰ÁWVSƒì$‹t$8‹|$<‰D$‹D$D‰T$‰D$‹B+‰Ã‰D$ ‹D$@Áëÿ  ‰\$Áè‰D$‹l  …À„Ê   ‹h‹P‰ë	ÓtL‰$‰T$´&    ‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğè‚™   ƒ$ ‹$‹h‹P‰è‰T$	ĞuÂ»ÿÿÿÿ‹|$‹G$‹W(‰$‰Ñ‰T$‰Âÿt$‹D$èüÿÿÿ‹OP‰G$‰W(^‹t$9ñs])Î;t$rU‰D$<‹D$ Ù‰T$@% ğÿÿ‰D$8‹ƒÄ$‰ø[^_]éüÿÿÿ´&    v ‰ğ¬ø‰Ãë‰¶    ‹$px‰ó¬ûéoÿÿÿƒÄ$¸úÿÿÿ[^_]Ã´&    ´&    èüÿÿÿUWVS‰Óƒì‰T$ºşÿÿÿ9Úº    ‰L$Ê‚Ö   ‹=    ‰ÅWÿ1ÿÁâ‰Ö‹    ƒú@t/‰Ñ¸   1Ò1Û¥ÂÓàöÁ EĞEÃ‰Á‰ÓƒÁÿƒÓÿ9ñ‰ÚúBñBû‹…l  …Àu ‹D$9t$ø“ÀƒÄ[¶À^_]Ã´&    ‹h‹P‰ë	ÓtE‰$‰T$t& ‹$‹‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğèr6ƒ$ ‹$‹h‹X‰è‰\$	ØuÇ¾ÿÿÿÿ¿ÿÿÿÿëƒv ƒÄ¸   [^_]Ãv ‹$+pxébÿÿÿfèüÿÿÿWVS‰Ãèüÿÿÿ„Àu[¸ÿÿÿÿ^_Ãt& ‹ƒX  …Àt‹p‹8‰ğ	øu¿ÿÿÿÿ1ö‹ƒh  ‹“d  ‰Á	Ñt9ú‰ÁñBúBğ‰Øèüÿÿÿ9ÇÖr‹ƒ€  …Àt¢€x) tœ‰Ø[^_éüÿÿÿ´&    ´&    èüÿÿÿUW‰ÏV‰ÖSƒì‰D$‹€l  …Àtk‹h‹P‰ë	ÓtH‰$‰T$´&    ‹$‹H‹X‰ø9ÎØr‰ğ‰ú)ÈÚ;D$‰Ğèr%ƒ$ ‹$‹h‹X‰è‰\$	ØuÆ1ÒƒÄ‰Ğ[^_]Ãf‹$px‹D$1Ò‹€€  …ÀtÛ;0‰ûXrÒ;px’ÂƒÄ[‰Ğ^_]Ã´&    t& èüÿÿÿUWV‰ÖS‰Ã‰Ğ‰Êƒì‹»l  +D$T$ ‰$‰T$…ÿ…¿¼  ‹D$1í$‰ÏuƒÄ‰è[^_]Ã´&    ¡   ¹@   ºÀ  èüÿÿÿ…Àt>‹$‹L$‰0‰x‹t$‹|$ ‰P‰p‹t$$‰x‹|$(‰H‰p‰x‰ƒl  ƒÄ‰è[^_]Ã½ôÿÿÿë‘fèüÿÿÿV‰È‰ÖS‰Ëèüÿÿÿ„ÀujÃ   @‰ØÁëÁè›‹Å    ƒààĞ¹À  º   ‰ğèüÿÿÿ…Àu*‹L$‹6‘ÿ  ‹â ğÿÿƒáöÃu+	ÙÇF    ‰‰V[^Ã´&    v ‰Øèüÿÿÿ‰Ãë©t& ´&    ´&    èüÿÿÿUWV‰ÖS‰Ëƒì‹zP‰D$‹D$0‰D$‹B+‰ÅÁè‰D$‹D$,ÿ  Áè‰$‰Èèüÿÿÿ„À…   Ã   @‰ØÁëÁè›‹Å    ƒààĞ‹N(‹V$ÿt$‹D$èüÿÿÿY‹$‰F$‰V(9ùv^)ù;L$rV‹‰D$(‰è% ğÿÿ‰T$,‹Áé‰D$$‰ğ‹<Í    ƒçà)û‰ÙÁùiÉÍÌÌÌNPƒÄ[^_]éüÿÿÿt& ‰Øèüÿÿÿ‰Ãë†t& ƒÄ¸úÿÿÿ[^_]Ã´&    t& èüÿÿÿUW‰×V‰ÆSƒì‹¨T  ‰$‹L$…íD-    èüÿÿÿ‰Ã…ÀtGj 1É‰Ú‰ğÿt$W‹m èüÿÿÿ‹L$ƒÄ‰!Ğ‰Qƒøÿtg‰Øèüÿÿÿ‰ù1ÒèüÿÿÿƒÄ‰Ø[^_]Ãt& Gÿ¹ÿÿÿÿÁè½ĞDÑ‹Œ  ƒÂƒùÿtj ‹D$ èüÿÿÿ‰ÃX…Ûuˆë¾t& d‹    ëŞ´&    ‰Ú‰ù‰ğ1ÛèüÿÿÿëšèüÿÿÿUW‰ÏV‰ÖS‰Ã‹T$‹«T  ‹D$‹L$…íD-    ‹m$…ítj Q‰Ñ‰Â‰ØVèüÿÿÿƒÄ‰ñ‰ú‰Ø[^_]éüÿÿÿfffffffèüÿÿÿ¸úÿÿÿÃt& èüÿÿÿU¸ÿÿÿÿºÿÿÿÿ‰å]Ã´&    t& èüÿÿÿ¸êÿÿÿÃt& èüÿÿÿ1ÀÃffffèüÿÿÿW¿ÿÿÿÿVrÿSÁî‰Ã‰È½ÎDÏ¾   ƒÁ9ñOÎÁèƒàÁêP‰ØèüÿÿÿZ[^_ÃèüÿÿÿÃ´&    v èüÿÿÿS»   9ÙGË¶\$…Àt‹€|  …Àt‰\$[éüÿÿÿv ¡    ‰\$[éüÿÿÿèüÿÿÿ…Àt‹€|  …Àtéüÿÿÿ´&    ¡    éüÿÿÿ¶    èüÿÿÿöÅt&‹€|  …Àuú   v¡    …Àtéÿÿÿ¶    1ÀÃ´&    ¶    èüÿÿÿV‰ÖS‹€|  ‰ËÁÿ  Áé…Àt2èüÿÿÿ„Àu!ƒë¸ÿÿÿÿÁë½ÓDĞ‰ğ[ƒÂ^éüÿÿÿt& [^Ãt& ¡    ëÇffffèüÿÿÿUWVS‹    ‹l$‹|$ …Ût'‰Öv ‹‹CƒÃW‰òUÿt$ ÿt$ èüÿÿÿ‹ƒÄ…ÀuŞ[1À^_]Ã´&    èüÿÿÿ¡   …ÀEts Ã´&    t& èüÿÿÿ‹€€  …Àt‹@…À•ÀÃ´&    1ÀÃ´&    ¶    V‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ‹V C@¹€°  èüÿÿÿPÿvÿvÿvÿvÿv·FÆVh(: Sèüÿÿÿƒ`  ƒÄ$[^éüÿÿÿ´&    ´&    W1ÿ1ÉV‹t$W1ÿV‹t$WVÿt$ ÿt$ èüÿÿÿƒÄ^_Ã´&    fèüÿÿÿUW‰ÇVS‰Ë‰Ñƒì(‹·€  ‹D$<‰L$‹V‰\$‰D$‹D$@‰T$‰D$‹)Á‰D$Ó‰È¬ØÁàF,‹v‹‹X‰t$ ‰$‹4$‰\$#t$‹X‹L$ƒşÿt3‹‡p  ‰Íåÿ  …Àt+‹4$#p‰êæÿ  )ò9õs€=›    „ê  ƒÄ([^_]Ãf‰ê1ö9Ú‡|  ‰Ğ1Ò$T$)ë‰D$‰T$Ş;t$s€=™    „ò  ‰t$‹$‹T$¬Ğ‰ÅÁè=€   Å    ¸    BÂT­ +L$L$ ‰$‹ ƒàà‹ĞÁèƒàƒøt@ƒøt+‹D$-   @ƒ|$„  ‹L$‹$ƒÄ([^_]éüÿÿÿt& ƒ=    uÌ´&    ‹t$‹D$æÿ  …À„ ÿÿÿd¡    ‰D$$ëu¶    ƒûƒ?  …Ût‹<$¶ˆöÃ…
  ´&    v èüÿÿÿd¡    ƒ¨<  ¸   èüÿÿÿd¡    …À„–  ÷D$   tû$ƒÅ1ö)\$„Ÿşÿÿ‹D$»   )ó9ÃGØœD$ú‰ê1ÿ‹   Áêú€   Õ    T­ CÇá   €‰L$‹ ƒàà<Ğ‹    ¸   âc  ‰T$ èüÿÿÿ‹D$$ƒ€<  ‹T$ ‹L$‰øèüÿÿÿÆƒ|$…ÿÿÿƒûsz…Û„"ÿÿÿ¶‹<$ˆöÃ„ÿÿÿ·Tş‹<$f‰Tşéÿşÿÿ´&    ‰Â‹L$‹$ézşÿÿ´&    v ‹<$‰ñ‹‰‰<$‹Tü~ƒçü‰Tü‹4$)ù)ÎÙÁéó¥é³şÿÿv ‹‹$‰y‹Tüƒçü‰Tü)ù)ÎÙÁéó¥é‹şÿÿv €=š    …aıÿÿ‹w,Æš   …öu‹7‰ø‰$èüÿÿÿ‹$Rÿt$SVPh´: èüÿÿÿƒÄé(ıÿÿfèüÿÿÿé`şÿÿ¶    ‹_,Æ›   …Ûu‹‰øèüÿÿÿUVSPh`: èüÿÿÿƒÄéçüÿÿ‹<$·Tşf‰Tşéîıÿÿ¶    ‹_,Æ™   …Ûu‹‰ø‰L$$èüÿÿÿÿt$VSPh; èüÿÿÿƒÄ‰t$‹L$$éÔüÿÿ´&    v èüÿÿÿUW‰Ç‰ÈV‰ÎSÁæ1Ûƒì‰   ‰ñù‰   Ó‹T$ƒ=    ‰4$‰=    ‰   ‰   Ç       ˆ(   uÆ)   Ç        …ÀtV1À¾€   ´&    f‰Á‰ó‰ÂƒÀƒáÁâ)Ë‹,   ‰\‹,   ÇÿÿÿÿÇDÿÿÿÿ‹,   ÇD    ;   rº¡       uŸ   À‹$1Ò‰Øèüÿÿÿ‰   ƒÄ[^_]Ã´&    ¶    U‰åW‰ÇV‰ÖSƒìd¡    ‰Eğ‹B,…À„  èüÿÿÿ‰Ã@ÁàƒÈ$‰Eà‹W@‰Uäèüÿÿÿ‹Uä…    ‹GD‰Uä…À„Ñ   ƒÃ0MìUèƒãøCü‰EÜèüÿÿÿ‰Ã…Àt‹U‹Eè‰P0º`   ‰h<f‰P4Ç@8    ‹Eà‰C‹V,…ÒtsC$èüÿÿÿ‹X  1À1Ò…Ét‹‹Q‰C‹E‰S‹U‰C‹E‰S‹Mì‰C‹E‹UÜ‰C ‰Øj ÿuäÿuèj jWèüÿÿÿƒÄ‹Eğd+    uTeô[^_]Ã´&    f‹¸?Ì  …ÒDĞé|ÿÿÿ´&    ‹…À…%ÿÿÿë¿t& ‹ÇEà$  »   …À„æşÿÿéÎşÿÿèüÿÿÿ´&    W‰×VS‰Ãƒìd¡    ‰D$‹C$©À  …¾   ‹G,…À„“   èüÿÿÿpH%ÁæƒÎ$‰Ú‰àèüÿÿÿ‰Ã…ÀtN‰p‹W,…ÒtYC$èüÿÿÿ‹X  1À1Ò…Ét‹‹Q‰C‹D$,‰S‹T$0‰C‹D$4‰S‰C‹D$8‰C ‰àèüÿÿÿ‹D$d+    uRƒÄ[^_Ã‹¸?Ì  …ÒDĞë™f‹¾$  ¹+   …À„jÿÿÿéTÿÿÿ´&    öÄ…9ÿÿÿ‰Øèüÿÿÿ„À„*ÿÿÿë¡èüÿÿÿèüÿÿÿ‹    % ğÿÿƒúEĞ‰ts Ãv èüÿÿÿ¡¼º ÁàÃfèüÿÿÿƒ=    „®   ‹   …É…¨   V‰Ñ‰ÖS‰Ã‰ĞÁàÁé…¡   ƒèÁè¹ÿÿÿÿ½ĞDÑ¸À  ƒÂèüÿÿÿ£,   …Àts‰ò‰ØÁâÁêèüÿÿÿ1Òƒ   @‰ñjèóûÿÿèüÿÿÿƒ=    ¡   ZtÁà% ğÿÿ£ts 1À[^Ãt& Çts    1À[^Ã´&    f1ÀÃt& ¸ôÿÿÿÃ¸ôÿÿÿëÉ¸ÿÿ é[ÿÿÿ´&    ´&    èüÿÿÿUÁèWƒÀV1öSƒìƒ=    „–   Áàºÿÿÿÿ¾   ¿   %  üÿƒèÁè½ÈDÊi‰éÓæ‰ğ‰4$Áà‰D$ş   ëg´&    ƒí‰ş‰éÓæş   ~O‰ê¸   èüÿÿÿ‰Ã…Àtİ‹D$¿ÿÿÿÿƒèÁè½ĞD×B9èuL‹$‰Øèüÿÿÿ‰Æ…Àu)ƒÄ‰ğ[^_]Ã´&    ƒÄ¾ôÿÿÿ[‰ğ^_]Ã´&    f‰Ø‰êèüÿÿÿ‰ğƒÄ[^_]Ãé½  ´&    èüÿÿÿW|$ƒäøÿwüU‰åWV‰ÖSƒì\‰Uè‹‰EÀ‹EÀ‰U¬‹W‰Mì‰U´‹W‰U¸‹W‰U ‹€  ‹€p  ‰UÜ…À„;  ‹P‰UÄ!òâÿ  ‰U¨‹}Ü…ÿ„~½  ‹U´9U¬‡+  ‹u´u¨‰u¼‹uÀ‹l  ‹uÜ‹‹N‰UÈ‰MÌ…Àt‹@‰EĞ…À…!  …Û…  ‹EÈÇEä    ÇEØ    ‰Eà‹EÄ1Ò% øÿÿ‰EÄ‹E¼ÿ  ƒÒ ¬Ğ‰E¤…À„  ‹EÄ‹]ÜÁèƒÀ 9ĞCĞ}¼ÿ  GÂ‹U¸ÁêƒÂ9ÂCÂ‰E¸‰ÆC ‰Eœèüÿÿÿ‹K‹}¤‰E˜‰È+C‰MÔ9Ç‡  ‹C‰}ĞTÿ÷Ş1À!ò‹uì9ÑGÂ‹Uä‰EÈ‰Ã‹Eàÿ  ƒÒ ¬Ğ‰E¼‹Eè	Æ‰ò‹uØ‰UØFÿët& ƒÃ1É9]ÔFÙ;]È„¶   ‹}Ø…ÿt‰ÙÁáMà3Mè…MÄuÕ‰Á!ñ‰M°t\´&    v ‹EĞÇEä    ‰Eà…Û…ë   ‹EÈ‹]ĞÇEä    !Ø‰Eàƒûÿ„(  ‹EĞ1ÒƒÀÿ  ƒÒ ¬Ğ‰EØé¯şÿÿt& ‹M¼Ù!ÁMĞ9ñw‹}Ü‹UĞ‹O,‰ßÁç;T9†R  ]¸1É9]ÔFÙ;]È…Jÿÿÿ‹U˜‹Eœèüÿÿÿ÷E    uGº@°  ¸ º èüÿÿÿ…Àt4é1½  ´&    v ÇE¨    ÇEÄ    éÃıÿÿt& €=Q$   „W½  »ÿÿÿÿ¾ÿÿÿÿeô‰Ø‰ò[^_]gø_Ã‹{‹s‰ø	ğtB‰uØ‰}Ô¶    ‹‹uÈ‹}Ì‹K9Ö‰øÈr)ÖÏ;uØ‰øEÔr_‹C4‹s0ƒÃ ‰Ç‰uØ	÷‰EÔuÊƒ}Ğÿ…ØşÿÿÇEØ    é—ıÿÿt& ‹{‹sÇEàÿÿÿÿÇEä    ‰ø	ğtÖÇEĞÿÿÿÿ‰uØ‰}Ôë‡´&    ‹EÈ‹UÌ+CS‹Mä‰Ö‹Uà‰÷!Ğ!Ï‰Eà‰}äë”¶    ¶    ‹E¤‰ş‹}°‹E´‰Uà‰EØ‰Ø9ÓsK‰}È‹UØ‰]Ô‹}à‰uØ‹uÜë´&    v ‹N,‰ÃƒÀÁãÇD    ‹N,‰Tê   9øuÜ‹]Ô‹uØ‹}ÈCÿ‰Âƒâƒút4‰uØ‹uÜë¶    ‰Ù)Áƒè‰J‰Âƒâƒút‰ÂÁâV,‹J…Éuİ‹uØ‹MÜ‹Uà1À9QGÂ‹U˜‰A‹E¤A‹Eœèüÿÿÿ‹UàÁâƒûÿ„öıÿÿ‰]Ø‹MÜ‰Uà´&    ‹Y,‰ø1ÒEèUìÇ   ‰3‰T3ƒÆ9uàuŞ‹uÜ‹]Ø1Ò‹E¨VÁãj‰Şÿu¬ÁşÃ‹EÀÖ‰Ú‰ñè‘ñÿÿXZeô‰Ø‰ò[^_]gø_Ã´&    ´&    èüÿÿÿUW‰ÏV‰ÖS‰Ãƒì‹D$,‹T$(‹L$$ƒà ƒâı	Ğ„4  ‹ƒp  ‹«€  …À„  ‹X!ó‰Ø1Û%ÿ  ‰Á+u })Îß¬ş1ÿ‰ò‰ó‰t$ÁâU,B‰ÆE Æÿ  ‰$ƒ× ¬ş‰t$Şèüÿÿÿ“€   ƒâ€‰D$1À9Ö}‰ğÁàE,‹@ƒî9ó@‹<$´&    ‹M,‰òƒÀƒîÁâ‰D‹M,ÇÿÿÿÿÇDÿÿÿÿ‹M,ÇD    9ó~Í‰<$ƒë‹<$D$‰Úƒâƒúuë*t& ‰Á)Ùƒë‰J‰Úƒâƒút‰ÚÁâU,‹J…Éuİ‰<$‹D$)E‹T$‹$ƒÄ[^_]éüÿÿÿ1É1Û1Àéõşÿÿt& j‰ò‰ØQ‰ùèğÿÿXZé·şÿÿ´&    t& èüÿÿÿS‹\$ƒûvƒûu[Ã´&    v ÇD$   [éÂïÿÿf´&    ´&    èüÿÿÿS‹\$÷Ãıÿÿÿtƒûu[Ã´&    ÇD$   [é‚ïÿÿf´&    ´&    èüÿÿÿW|$ƒäøÿwüU‰åWV‰ş‰ÇS‰Ë‰Ñƒì<‰Eà‹‰EÄ‹F‰EÀ‹F‹·l  ‰E¼d¡    ‰Eì1À‰Ğ‰Ú…ö…V  >t& ÿu¼‰Ê‰ÙÿuÀj ‹EÄPP‹Eàèüÿÿÿ‰Á!Ğ‰ÓƒÄƒøÿ„  ‹Eà‹°l  …ö…}  ‹EÄ‰Uè1Ò‰MäƒÀÿ‰MĞƒÒÿÈ‰]ÔÚ‰EÈ‰UÌ‹Uà‹‚X  ‹8‹p‰Ğ‹€h  ‹’d  ‰EØ‰ğ	ø…£   ‹uÈ‹}Ì‹EØ9òø‚  ‹Eìd+    …y  ‹EĞ‹UÔeô[^_]gø_Ã¶    ‹5    d‹=    £=    ƒ%ÿÿÿdÿ    ‹=    …ÿt"‰MØ‹‰]ÜVÿuÄRP‹Uà‰øèüÿÿÿ‹MØ‹]ÜƒÄdÿ    …åşÿÿèüÿÿÿéÛşÿÿt& ‹EØ	Ğ„%  9×‰ğEØB×‰÷C}Ø‰}Øé:ÿÿÿt& ‹F‹~‰Â	útI‰uØ‰}È‰EĞv ‹EØ‰ß‹P‹ 9Á×r‰Î‰ß)Æ×;uĞ‰øEÈ‚"  ƒEØ ‹EØ‹P‹@‰EÈ	Ğ‰UĞuÃ¸ÿÿÿÿºÿÿÿÿéFşÿÿt& ‹F‹~‰Â	útE‰uØ‰}È‰EĞv ‹EØ‰Ş‹P‹ 9ÁÖr‰Î‰ß)Æ×;uĞ‰øEÈrvƒEØ ‹EØ‹x‹@‰EÈ	ø‰}ĞuÇÇEäÿÿÿÿÇEèÿÿÿÿ‹E¼‰Ê‰ÙƒÈ PÿuÀÿuÄ‹EàèüÿÿÿƒÄ€=˜    „‚   ÇEĞÿÿÿÿÇEÔÿÿÿÿéEşÿÿt& ‰uØ‰úé#şÿÿ¶    ‹uØ‰È‰Ú+FV‰EĞ‰Æ‰×‰Eä‹EÄ‰UÔ‰Uè1ÒƒÀÿƒÒÿğ‰EÈ‰ğú!ø‰UÌƒøÿ…µıÿÿékÿÿÿt& ‹uØ‰È‰Ú+FVé8ıÿÿ‹]àÆ˜   ‹ƒX  ‹³d  ‹»h  ‹P‹ ‰EØ‰Ø‹[,‰UÜ…Ûu‹‹EàèüÿÿÿMäWVÿuÜÿuØÿuÄQSPhÜ= èüÿÿÿƒÄ$éÿÿÿèüÿÿÿfèüÿÿÿ¸   Ãffèüÿÿÿèüÿÿÿ…Àtƒxu‹@Ã´&    1ÀÃ´&    ¶    èüÿÿÿVÂÿ  ¹   SÁê‰Ãÿt$ÿt$èüÿÿÿZY‰Æ…Àtèüÿÿÿ‰X‰ğ[^Ã´&    èüÿÿÿUÂÿ  WÁêV‰×‰Æ•    Sƒì‰$ºÀ  èüÿÿÿ…À„’   ‰Ã…ÿt_‹‹$Áè‹Å    ƒàà)ÆÁşiÆÍÌÌÌ‰Ş…    <1É)Ö´&    ‰ÅÁíı€   í    ,€CÑ‹ƒâàê‰†ƒÀ9ÇuÖÿt$¹   ‰Øÿt$‹T$èüÿÿÿ‰Æ‰ØèüÿÿÿXZƒÄ‰ğ[^_]Ã1öëòt& èüÿÿÿS‰Ãèüÿÿÿ…ÀtƒxtSh > èüÿÿÿXZ[Ã´&    ‰Ø[éüÿÿÿffffèüÿÿÿUWVS‹    …Ût$‰Õ‰Ï´&    f‹3‹CƒÃ‰ù‰êèüÿÿÿ‹…Àué[1À^_]ÃfèüÿÿÿUWVS‹    …Ût$‰Õ‰Ï´&    f‹3‹CƒÃ‰ù‰êèüÿÿÿ‹…Àué[1À^_]ÃfU‰åW‰×V‰ÆSƒì‰Mè‹X@èüÿÿÿ…    ‹FD…ÀtbMğUì¸   èüÿÿÿ…ÀtA‹M‹Uì‰J0¹`   ‰j<f‰J4ÇB8    ‹O,‹}è‰H‹Mğ‰xj SRº   j jVèüÿÿÿƒÄeô[^_]Ãt& ‹…Àu˜ëëW‰ÇV‰ÖS‰Ëƒì‹@$©À  u*¹   ‰ú‰àèüÿÿÿ…Àt‹V,‰X‰P‰àèüÿÿÿƒÄ[^_ÃöÄuÑ‰øèüÿÿÿ„ÀtÆëæ´&    ´&    V‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ÿv ÿvÿvÿvÿvÿvÿvhL> Sèüÿÿÿƒ`  ƒÄ$[^éüÿÿÿV‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ÿvÿvhõÌ  Sèüÿÿÿƒ`  ƒÄ[^éüÿÿÿ´&    ´&    W1ÿV‰Î1ÉWVèüÿÿÿXZ^_Ã´&    t& èüÿÿÿd¡    ©ÿÿÿud¡    ‹ ¨uÃéüÿÿÿ´&    t& èüÿÿÿV‰ÆS‰Óƒâ td¡    €¸   …İ   d¡    ‹€è  …À…±   öÃu4öÃuöÃ@t
‰ğ[^éüÿÿÿƒãtd‹    ‹C ¨u][^Ã´&    v >t& ëÅ´&    f‹Nd¡    £    sªdÿ    ¡    …Àt
‹@‰òèüÿÿÿdÿ    u‡èüÿÿÿë€t& Áàº   %€   ƒÈèüÿÿÿ…Àu7[^d¡    ‹ Ãt& ‹V1Àú ğÿÿ–Àèüÿÿÿé7ÿÿÿÆ€   éWÿÿÿt& ¹   ‰Úèüÿÿÿë»fW1ÿV‰Î1ÉWVèüÿÿÿXZ^_Ã´&    t& èüÿÿÿUW‰ÇVS‰Óƒìƒâ u;öÃuD1ööÃuWƒã‹o,…£   d¡    ‹ˆè  …É…Ÿ   …öDõƒÄ‰ğ[^_]Ãèüÿÿÿ„ÀugöÃt¼d‹5    ‹F ¨u$öÃuO1ööÃt©1Àèüÿÿÿ‰Æƒøÿu›ë8´&    Áàº   %€   ƒÈèüÿÿÿ…Àud¡    ‹ƒât¶ö€Ñ  t­¾ÿÿÿÿé|ÿÿÿ¶    >t& éSÿÿÿ¶    ‹G‹_‹‹O‰D$‹G‰\$‹_‰$‰L$‰D$‰\$„3ÿÿÿP‰èÿt$èüÿÿÿXZé ÿÿÿ´&    ¹   ‰òèüÿÿÿépÿÿÿ´&    d¡    £    ƒÚşÿÿdÿ    ¡    …Àt‹@‰é‰úèüÿÿÿdÿ    …±şÿÿèüÿÿÿé§şÿÿ´&    v W‰ÇV‰ÎS‰Óƒì‹@$©À  uJ¹$   ‰ú‰àèüÿÿÿ…Àt0‰p‹P‰H‹C‰B‹C‰B‹C‰B‹C‰B‹C‰B‰àèüÿÿÿƒÄ[^_ÃöÄu±‰øèüÿÿÿ„Àt¦ëæ´&    ´&    U‰åW‰ÇV‰ÖSƒì‰Mä‹X@èüÿÿÿ…    ‹GD‰]è…ÀtMğUì¸$   èüÿÿÿ…Àt`‹M‹Uì‰J0¹`   f‰J4‹Mä‰j<ÇB8    ‰H‹‹Mğ‰X‹^‰X‹^‰X‹^‰X‹^‰X‹v‰p j ÿuèRº$   j jWèüÿÿÿƒÄeô[^_]Ãv ‹…À…wÿÿÿëé´&    t& èüÿÿÿ‰Á‰Ğd‹    ‹RöÂ?uÃ´&    ‰Èé	ıÿÿ´&    fèüÿÿÿÃ´&    v èüÿÿÿW‰ÇVSd‹5    ‹èüÿÿÿ÷Ã0 uu÷Ã  @ …4  èüÿÿÿ€ç@…>  >t& ‹5    …öud¡    ‹˜¸  …Û…Ë   [^_Ã´&    èüÿÿÿöÇu.÷Ã  u5ƒãu<úèüÿÿÿd¡    ‹÷Ã0 t‹ûöÃuÍöÇtÒ‰øèüÿÿÿ÷Ã  tË‰øèüÿÿÿƒãtÄğ€&ı‹†ä  …À…»   ‹†t  …À…½   èüÿÿÿèüÿÿÿd¡    ‹€0  …ÀtŠ‰ú1Àèüÿÿÿé|ÿÿÿt& d‹    1¶À1Ád‰    éÿÿÿ¶    èüÿÿÿ…À„(ÿÿÿ‹    …É…ÿÿÿhÍ  h0Í  èüÿÿÿXZéÿÿÿt& èüÿÿÿèüÿÿÿ€ç@„Çşÿÿt& èüÿÿÿé¸şÿÿ¶    èüÿÿÿé;ÿÿÿ¶    ‹†t  èüÿÿÿÇ†t      é)ÿÿÿ´&    ´&    èüÿÿÿƒìd‹    ‹RöÂvu	úƒÄé şÿÿ‰$èÈùÿÿ‹$ëêfèüÿÿÿVS‰Ãd‹5    ƒì$d¡    ‰D$ 1À‹C0+†  ;†  s 1À‹T$ d+    …¸   ƒÄ$[^Ã´&    ‰Øèüÿÿÿ„ÀuÕ‹†   …Àtèüÿÿÿ…Àue„Òt¾€úupÆ†  ‹K,1Ò1À‰K‰TƒÀƒørôd¡    ‹@Ç$   ÇD$   ‹€ä  ÇD$  @‰L$‰D$‰àèüÿÿÿ¸   é_ÿÿÿ¸   èüÿÿÿ¸   éKÿÿÿ¸   èüÿÿÿ¸   é7ÿÿÿèüÿÿÿèüÿÿÿS…Àtfƒøu(…Òu‹D$…Àt%|$ÿÿÿ¿¸òÿÿÿv[Ãt& 
9ĞwÜ¸êÿÿÿ[Ãf‹\$d¡    ‰˜   ‰  ‰ˆ  Æ€   ğ€H 1À[Ãt& L$	Ñu¿d¡    Ç€       Ç€      Ç€      Æ€   ğ€`ß1À[ÃffèüÿÿÿUWVSƒì(‹H‹d‹    ‰T$$1Ò‹p‰L$‹H‰L$‹H‰$èüÿÿÿ‰Øèüÿÿÿ‰Ã‰ğèüÿÿÿ…Û„  ‰Æ…À„w  C‰D$¸   ğÁC…À„V  P	ÂˆË  F‰D$¸   ğÁF…À„J  P	Âˆ¿  èüÿÿÿ‹†¨  ¨´  ‹ƒ¨  ´  9Õ†º   ‰è‰T$èüÿÿÿ‹T$…À‰Ç…ı   ‰Ğèüÿÿÿ‰Ç…À…å   º   ‰Øèüÿÿÿ„À„¬   º   ‰ğèüÿÿÿ„À„˜   ‹$ƒø„Ü  ‡†  ƒø„M  ƒø…<  ¡x$  ‹“˜  ‹|$  1Â3†˜  ¯Ñ´&    ¯Á1É9ÂŸÁœÀ<	¶À	Çë>´&    f‰Ğ‰T$èüÿÿÿ‰Ç…ÀuK‹T$9Õ„Sÿÿÿ‰è‰Õ‰Âé7ÿÿÿ´&    ¿ÿÿÿÿ‹†¨  ‹‹¨  ´  ©´  9ètèüÿÿÿ‰èèüÿÿÿ¸ÿÿÿÿğÁCƒøt=…À¥  ¸ÿÿÿÿğÁFƒøt6…À¦  ‹D$$d+    …Î  ƒÄ(‰ø[^_]Ãv ‰ØèüÿÿÿëÂ´&    ‰ğèüÿÿÿëÉ´&    ‹D$º   èüÿÿÿé"şÿÿt& ‹D$º   èüÿÿÿé.şÿÿt& …À„¨  ƒø…e  ¡h$  ‹“à  ‹l$  1Â3†à  ¯Ñé¶şÿÿ¶    ‹$ƒø„ì  ƒø…¥   ‹T$¹   D$èüÿÿÿ…À…  èüÿÿÿ‹T$‰Øèüÿÿÿ‰Åèüÿÿÿ…í„á  ‹T$‰ğèüÿÿÿ‰Ç…À„Ì  ‹T$‹L$ èüÿÿÿ‰$‰øèüÿÿÿ‹$‰×ú ğÿÿ‡zşÿÿ¡d$  3-`$  3`$  ¯è¯Ğ9ÕŸÀ¶À< œÀ¶À	ÇéKşÿÿƒø…{  ¡ˆ$  ‹“P  ‹Œ$  1Â3†P  ¯ÑéÌıÿÿt& ‹D$º   èüÿÿÿéHşÿÿt& ‹D$º   èüÿÿÿéGşÿÿt& ‹D$º   èüÿÿÿé¢üÿÿt& ‹D$º   èüÿÿÿé®üÿÿ¿òÿÿÿé¸ıÿÿv ¡€$  ‹“¬  ‹„$  1Â3†¬  ¯Ñé?ıÿÿ´&    èüÿÿÿ‹T$‰Øèüÿÿÿ‰Çèüÿÿÿèüÿÿÿ‹T$‰ğèüÿÿÿ‰Åèüÿÿÿ…ÿ„   …ít}¡d$  3=`$  3-`$  ¯ø¯è9ïéÔşÿÿ¡p$  ‹“œ  ‹t$  1Â3†œ  ¯Ñé¿üÿÿ´&    ¡$  ‹“Œ  ‹”$  1Â3†Œ  ¯Ñé—üÿÿ´&    èüÿÿÿ¿ıÿÿÿé+ıÿÿ¿÷ÿÿÿéÌüÿÿèüÿÿÿ¿êÿÿÿé½üÿÿffffèüÿÿÿ÷@ € €t
1ÀÃ´&    S‰Ã‹ ©   u€=     t¸   [Ãt& 1À[Ãt& ‰Øèüÿÿÿ„Àuâ€=     tä‹C[ÁèƒğƒàÃ´&    ´&    èüÿÿÿU‰ÅWV1öSd‹    ‹{ë´&    èüÿÿÿ¾   ¸   ‡C¸€s èüÿÿÿ¡    K   …Àu,cÿÿşÿ¸€s èüÿÿÿd¡    ö@u·‡x[‰ğ^_]Ãt& ‰Øèüÿÿÿ„ÀtÉ‰è„ÀtÊèüÿÿÿ„Àuºë¿´&    t& èüÿÿÿSd‹    èüÿÿÿ¸€s èüÿÿÿcÿÿÿ¸€s èüÿÿÿèüÿÿÿ¡    …Àu1À[Ã‰Øèüÿÿÿ„Àtğ1À[éÿÿÿ´&    ¶    èüÿÿÿWVS‰Ãƒìd¡    ‰D$1ÀğƒD$ü 1ööC@t!‹D$d+    …   ƒÄ‰ğ[^_Ã´&    ¸€s èüÿÿÿ‰Ç¡    …Àu‰ú¸€s 1öèüÿÿÿëºv ‰Øèüÿÿÿ‰Æ„Àtà‹C©   uÖ©    u:‰â‰Øèüÿÿÿ…Àt1Ò‰Øèüÿÿÿ‹ƒ¬  ‹$èüÿÿÿ‰ú¸€s èüÿÿÿédÿÿÿt& º   ‰ØèüÿÿÿëÜèüÿÿÿt& èüÿÿÿS‰Ã¸€s ƒìèüÿÿÿ‰ÂöCuƒÄ¸€s [éüÿÿÿt& ‰$‰Øèüÿÿÿ‹$¸€s ƒÄ[éüÿÿÿffèüÿÿÿS‰Ã¹   ºĞ°  ƒì‰$d¡    ‰D$1À‰Øèüÿÿÿ…À„ˆ½  ‹$¹   ºÄ°  èüÿÿÿ…Àt@‹$¹   ºÀ°  èüÿÿÿ…Àt`T$‰àèüÿÿÿ…À…˜½  ‹D$d+    u;ƒÄ¸   [ÃÇ       ‹$€x,uƒÀ	‰$T$‰àèüÿÿÿ…À„ê½  éÏ½  èüÿÿÿÇ       ‹$€x,uƒÀ‰$T$‰àèüÿÿÿ…À„¾  é¾  èüÿÿÿğ«„s 1ÀÃèüÿÿÿèüÿÿÿ1É‰Â¸ a  ‹•    ƒ< ”Á‰Ã´&    t& èüÿÿÿ‰Ğ1ÉºP~ éüÿÿÿ´&    ¶    èüÿÿÿh„s ÿ5    hjÍ  Pèüÿÿÿ1ÀƒÄÃ´&    ´&    èüÿÿÿUWVSƒì‰$d¡    ‰D$1Àƒù„ü   ¸t¿ » a  èüÿÿÿ¸   èüÿÿÿ¸pÍ  èüÿÿÿd¡    ‹…    ‹,¸   èüÿÿÿd¡    …À„é   h    º ~ 1À1Éj¾a  »ÿÿÿÿèüÿÿÿXZë7¶    ®    ‹1ÀyÇ    Çü      ƒçü)ùÁ   Áéó«‰Øº    èüÿÿÿ‰Ã;    r¹¸t¿ èüÿÿÿ¡Œs 1Ò…    ¡s èüÿÿÿ‹$‹T$d+    uQƒÄ[^_]Ã´&    ¹   D$èüÿÿÿ‰Â¸òÿÿÿ…ÒuË‹D$èüÿÿÿ‰Â¸êÿÿÿ…Ò„Óşÿÿë±t& èüÿÿÿéÿÿÿèüÿÿÿ´&    fèüÿÿÿUWVSƒì‹D$0‰T$‰L$·ˆs ‰D$‰Ã‹3» a  d¡    ‰D$1À¸   Óà‰D$¸t¿ èüÿÿÿ¸   èüÿÿÿ¸pÍ  èüÿÿÿd¡    ‹…    ‹‰$¸   èüÿÿÿd¡    …À„V  h    º ~ 1À1Éj¿ÿÿÿÿ»a  èüÿÿÿXZ‰t$´&    ‰øº    èüÿÿÿ‰Ç;    sM‹$ƒ½    ‹ ¨   ë!v ‹s ğ4ŠÇ     Ç@    ƒÀ9èt²‹p‹…öuÙ…ÉtìÇ     ëä¸t¿ ‹t$èüÿÿÿ¡Œs …   1À9òvO‹D$)ò9ÂGĞ‰×…Ò„¨   ƒş‡Ÿ   ‹l$ëƒï‰ÓƒÆƒÅ)ûƒşw.…ÿt*¶D4‰éèüÿÿÿ…ÉtÚ¸òÿÿÿ‹T$d+    uYƒÄ[^_]Ã‰l$¡s t0ü…ÿx<¹   ‰ú‰ğèüÿÿÿ‹D$‰ù‰òèüÿÿÿ…Àu¶;‹|$‰ÃÁû_ë¨èüÿÿÿé şÿÿë•èüÿÿÿv 1Ûë§t& èüÿÿÿWV‰ÆSğ³„s 1Û¿a      µ    ø‹…Òt/‹Â   @‰ÑÁé‹Í    Ç     ‰Ğ1ÒÁèƒáà€Áèüÿÿÿƒûu[1À^_Ã´&    »   ë¢´&    fèüÿÿÿUW¿a  V1öS‰Ã¸    ƒì‹    ‹,¸ a  Ç    ·    ‹…Ét%ƒşu1ÀƒÄ[^_]Ã¾   ·    ‹…ÉuÛj 1Ò‰é¸À  èüÿÿÿZ…Àt‹    ú‰$èüÿÿÿ‹$‰²ë­t& ‰Øèáşÿÿ¸ôÿÿÿë¡´&    v èüÿÿÿU-    WVS‰Ãƒì·ˆs ¡Œs ‰T$Óëƒè9ÃGØİ    %ø  ‰D$‰Ç‰ØÁàƒğø%ø  ‰D$¸   èüÿÿÿ¸pÍ  èüÿÿÿ¸a  d‹    ‹•    º a  ‹‹,…í„   œD$úE@‰$4ı    ‰êD5 4$´&    ‹H;ta…É„‘   ƒÀ9Æuê|$çÿ  9|$uÃ¡s ‹|$ğ<˜Å   ´&    ‹
¡s ‹ZğˆÇB    ƒÂÇBø    9Õuİë
v L$‰H÷D$   tû¸   èüÿÿÿd¡    …ÀtƒÄ[^_]Ã´&    v ‹|$‰‰xëÄèüÿÿÿƒÄ[^_]Ã´&    èüÿÿÿS9    u¡s …Àt‰Ğ[‰Êéoşÿÿ´&    [Ã¶    èüÿÿÿS‰Ğ‰Ëƒìd‹    ‰T$1ÒÇ$    ‰Új L$èüÿÿÿZ…Àt‹T$d+    uƒÄ[Ãv ‹$‰Ø‰„s ëŞèüÿÿÿ¶    èüÿÿÿV‰Æ¸pÍ  Sèüÿÿÿd‹    ·C4ƒàfƒøu	[^Ã¶    èüÿÿÿ£„s sé‰Øèüÿÿÿ;5    uÚ‹s …ÒtĞ[º   ^éıÿÿ´&    ¶    èüÿÿÿh ±  1Éº€  ¸»Í  èüÿÿÿXÃfèüÿÿÿ‹H;Hs3VS‹X…Ûu‹q‰p‰‹‹p9p[’À^Ãv ƒë‰X¸   [^Ãf1ÀÃ´&    ¶    èüÿÿÿV‰Æ‰ĞS…Òt1É´&    ‹û    rû    sA[^Ã´&    û    rû    rãƒÁ9ÈuÉ[^Ã´&    ´&    èüÿÿÿSƒÁƒìd‹    ‰\$1Û‰$¸°… ‰T$‰L$ÇD$    j d‹    T$èüÿÿÿ‹D$Z‹T$d+    uƒÄ[Ãèüÿÿÿèüÿÿÿ…Àt…ÒuÃfÃWVSSé7¾  t& èüÿÿÿUWV‰ÆSƒì‰$…Étk‰Ó…Òtg‹D$‹T$ …À•ÀƒÂ1í1ÿ‰T$„ÀtK‰ğ‰î‰Åv ‹$ÿ4¸j ÿt$háÍ  SUèüÿÿÿÆƒÄ9ØrƒÄ‰ñ[‰È^_]Ãv Å)ÃƒÇ9|$wÃëâƒÄ1É[‰È^_]ÃfèüÿÿÿV‰ÖS‰Ã‰Ğèüÿÿÿ…Àt
¸   [^Ãf‹S;Ssó‹C…Àu!‹J‰K‰4‹C9C[’À^Ã´&    v ƒè‰C¸   [^ÃfèüÿÿÿVS‰Ãƒìd¡    ‰D$1ÀÇD$    d¡    9Ã‰$‹“X  ”À‰L$‹X  ¶ÀD$ ‰D$…Òt[r‰Ğğ±1uX	òx<‹C…ÀtEj ‰Ù¸p‡ T$èüÿÿÿ‰Øèüÿÿÿ‹D$Z‹T$d+    u&ƒÄ[^Ãt& 1Ò‰Èèüÿÿÿë¹t& 1ÀëÕt& ‰Âë•èüÿÿÿ´&    èüÿÿÿSƒìd‹    ‰\$1Û‰$‹T$‰L$‰T$ÇD$    P¸°… d‹    T$èüÿÿÿ‹D$Z‹T$d+    uƒÄ[Ãèüÿÿÿèüÿÿÿƒìd‹    ‰L$1É‰$‰T$1ÒÇD$    ÇD$    d¡    ö@ u‹H‰â¸°… Á´  èüÿÿÿ‹T$‹D$d+    u‰ĞƒÄÃèüÿÿÿffèüÿÿÿÃ´&    v èüÿÿÿiÀè  Ãt& èüÿÿÿU‰åW‰ÏJşVS‰Ãƒì…ÉJ
ƒë‰Ø¾…ëQÁè‰$‰Ø÷æ‰Ö‰ĞiÑo  Áè$ÁîÇ¹«ªªª‰Ğ÷áiËm  1ÛÁê1Ò¿   )ğ¾<   ÈÚ1ÛkÊ÷çÊ‹MÈÚ1ÛkÊ<÷æÊ‹MÈÚ1ÛkÊ<÷æÊ‹MÁ€¿±†ƒÓñÈÚƒÄ[^_]ÃèüÿÿÿUWV¾ÿÉš;S‰Ë‰Ñƒì‰$‹D$‹T$9Æ¾    Ö};¿ÿÉš;1ö¶     6eÄ‰õƒÒÿƒÁƒÓ 9ÇÕ|ê…Òy¶     Êš;ƒÒ ƒÁÿƒÓÿ…Òxî‹<$‰‰_‰GƒÄ[^_]Ãv èüÿÿÿ…Àºşÿÿ?HÂÃèüÿÿÿ=0øÿÿwç  ºÓMb÷â‰ĞÁèÃf¸şÿÿ?Ã´&    v èüÿÿÿWºLb VS‹8‹p9úº    òrK‹@º½7†iö   ú?B ‰Ã÷âÁûiÛ½7†Ú¬ĞÁê‰Á¸   ú‰Ó÷çòÁÓ¬Ù[^‰È_Ã´&    [¸Èüÿ?^_Ã´&    ´&    èüÿÿÿUW1ÿV‰ÖS»@B ÷ã‰Á‰ÕúÿÉš;vÁê	¿ƒK ‰Ğ÷ç‰×ÁïiÇ Êš;)Å[‰~‰È‰ê¹ Êš;÷ñ‰‰V^_]Ã´&    ´&    èüÿÿÿVS»@B ÷ã‰Á‰Öú–˜ v‰ğºkÊ_k÷âÁêiÒ€–˜ )Ö‰È‰ò¹€–˜ [÷ñ^Ãt& èüÿÿÿ=˜™™w€ÀÃ¶    ¸ÿÿÿÿÃfèüÿÿÿUiÊ@B ‰åW¿@B ÷çVS1ÛÑ‰Æù–˜ v‰ÈºkÊ_k÷â‰ÓÁëiÃ€–˜ )Á‰ğ‰Ê¹€–˜ ÷ñ‰Ú[^_]ÃfèüÿÿÿiÊ@B º@B U÷â‰å]ÊÃ´&    èüÿÿÿU‰å]Ã¶    èüÿÿÿU‰Ñ‰åV‰ÆS1Ûú?B v‰ÈºƒŞC÷â‰ÓÁëiÃ@B )Á‰ğ‰Ê¹@B ÷ñ‰Ú[^]ÃèüÿÿÿS‰Ñ‰Ãú?B v‰ÈºƒŞC÷âÁêiÒ@B )Ñ‰Ø‰Ê[¹@B ÷ñÃ´&    ¶    èüÿÿÿWV‰Æ1À9ĞS‰ÓÈ}L‰Ï1ÉÿÿÉš;w ‰Ø‰ú» Êš;÷ó‰Ã‰‰ğ[‰N‰V^_Ãt& ‰ú¹ƒK Áê	‰Ğ÷áÁêiÂ Êš;‰Ñ)ÇëÃv ‰È1Ò	ØtÅ‰Ï÷Ó1É÷×ÿÿÉš;v‰ùºƒK Áé	‰È÷â‰ÑÁéiÁ Êš;)Ç‰Ø‰ú» Êš;÷Ñ÷ó÷Ğ‰Ã¸ÿÉš;)Ğ‰Âé{ÿÿÿ´&    ´&    èüÿÿÿS‰Óƒì‹Hd‹    ‰T$‹‹@‰L$¹   ‰D$Áø‰D$‰Ø‰$‰âèüÿÿÿ÷ØÀƒàò‹T$d+    uƒÄ[Ãèüÿÿÿt& èüÿÿÿS¹   ‰Ãƒìd¡    ‰D$1À‰àèüÿÿÿ…Àu)‹$‹T$‰‹D$‰S‰C1À‹T$d+    uƒÄ[Ãf¸òÿÿÿëåèüÿÿÿt& èüÿÿÿƒì‰Á‰Ğd‹    ‰T$‹‰$‹Q¹   ‰T$‰âèüÿÿÿ÷ØÀƒàò‹T$d+    uƒÄÃèüÿÿÿt& èüÿÿÿS¹   ‰Ãƒìd¡    ‰D$1À‰àèüÿÿÿ…Àu)‹$‰Áø‰C‹D$‰C1À‹T$d+    uƒÄ[Ãv ¸òÿÿÿëäèüÿÿÿt& èüÿÿÿV‰Ö¹   S‰Ãƒìd¡    ‰D$‹‰â‰$‹C‰D$‰ğèüÿÿÿ…Àu:‹C¹   ‰â‰$‹C‰D$Fèüÿÿÿ÷ØÀƒàò‹T$d+    uƒÄ[^Ãt& ¸òÿÿÿëâèüÿÿÿt& èüÿÿÿV¹   ‰ÆS‰Óƒìd¡    ‰D$1À‰àèüÿÿÿ…ÀuV‹$S¹   ‰Áø‰F‹D$‰F‰àèüÿÿÿ…Àu1‹$‰FÁø‰F‹D$‰F1À‹T$d+    uƒÄ[^Ã´&    f¸òÿÿÿëİèüÿÿÿt& èüÿÿÿV‰Ö¹   S‰Ãƒì‹Sd¡    ‰D$‹‰T$‰â‰$‹C‰D$Áø‰D$‰ğèüÿÿÿ…ÀuL‹C‹S¹   ‰$‹C‰T$‰â‰D$Áø‰D$Fèüÿÿÿ÷ØÀƒàò‹T$d+    uƒÄ[^Ã´&    ¸òÿÿÿëŞèüÿÿÿt& èüÿÿÿV¹   ‰ÆS‰Óƒìd¡    ‰D$1À‰àèüÿÿÿ…ÀuV‹$‹T$¹   ‰‹D$‰VS‰F‰àèüÿÿÿ…Àu0‹$‹T$‰F‹D$‰V‰F1À‹T$d+    uƒÄ[^Ã´&    ¸òÿÿÿëßèüÿÿÿt& èüÿÿÿV‰Æ1À‰Ñ9ğSĞ}MúÿÉš;w%» Êš;‰Ê‰ğ÷ó‰Ñ‰Ã‰ÈºÓMbÁù÷êÁú)Ê‰Ø[^ÃÁê	»ƒK ‰Ğ÷ãÁêiÒ Êš;)ÑëÂ´&    ‰È1Ò1Û	ğtÑ÷Ñ÷ÖùÿÉš;v‰Ê»ƒK Áê	‰Ğ÷ãÁêiÒ Êš;)Ñ‰Ê‰ğ¹ Êš;÷ñ¹ÿÉš;÷Ğ‰Ã)Ñëƒ´&    fèüÿÿÿS‹èüÿÿÿ…Ût‰Ùèüÿÿÿºòÿÿÿ…ÉEÂ[Ã´&    t& èüÿÿÿSƒì‹ d‹    ‰T$1Òèüÿÿÿ‰$Áú‰T$…ÀuE1Ò‰àÇD$    èüÿÿÿ‰Ã…Àt‹D$d+    u(ƒÄ‰Ø[Ã´&    v ‰àèüÿÿÿëÙ´&    »òÿÿÿëËèüÿÿÿt& èüÿÿÿWVSƒì‹8‹pd‹    ‰T$1Ò…ÿt8‰àèüÿÿÿ‰ù‹$èüÿÿÿ…ÉuZ‹\$ºÓMbO‰ØÁû÷ê‰ĞÁø)Øèüÿÿÿ…Éu7…öu1À‹T$d+    u+ƒÄ[^_Ãt& ¹   º    ‰ğèüÿÿÿ…ÀtĞv ¸òÿÿÿëÈèüÿÿÿt& èüÿÿÿV‰ÖS‰Ãƒì…Àt"‹‹@…Àx}{ÿÉš;wt¹t]í9Ñº   Â|d‰ò‰Øèüÿÿÿ…Àu\…ötc‰$‹„  ú  w@‹‹V£    ‰   èüÿÿÿ¡ˆ¿ …À‹$t/Çˆ¿     …Ût1ƒÄ‰Ø[^éüÿÿÿt& ¸êÿÿÿƒÄ[^Ãt& …ÛuÛƒÄ[^Ã¶    ‰$èüÿÿÿ‹$ëØ´&    t& èüÿÿÿVSƒì‹‹Xd‹    ‰T$1Ò…É„|   ‰Èèüÿÿÿ‰T$Áú‰T$…Àu~Aèüÿÿÿ…Àurú@B wxiÒè  ‰T$…Ût4¹   ‰Ú‰àèüÿÿÿ…ÀuJ‰âD$èüÿÿÿ‹T$d+    uGƒÄ[^Ãt& 1ÒD$ëÜ´&    …Ût‰Ú¹   ‰àèüÿÿÿ‰â…Àt¼¸òÿÿÿëº1Ò1Àë®¸êÿÿÿë¬èüÿÿÿ´&    ´&    èüÿÿÿWS‰Ã{‰Ùƒçüì„   )ùd¡    ‰„$€   1ÀÁĞ   Ç    ÇƒÌ       Áéó«¹€   ‰àèüÿÿÿ…À…)  ‹$‰‹D$‰CÁø‰C‹D$‰CÁø‰C‹D$‰CÁø‰C‹D$‰C Áø‰C$‹D$‰C(‹D$‰C0Áø‰C4‹D$‰C8Áø‰C<‹D$ ‰C@Áø‰CD‹D$$‰CHÁø‰CL‹D$(‰CPÁø‰CT‹D$,‰CXÁø‰C\‹D$0‰C`Áø‰Cd‹D$4‰ChÁø‰Cl‹D$8‰Cp‹D$<‰CxÁø‰C|‹D$@‰ƒ€   Áø‰ƒ„   ‹D$D‰ƒˆ   Áø‰ƒŒ   ‹D$H‰ƒ   Áø‰ƒ”   ‹D$L‰ƒ˜   Áø‰ƒœ   1À‹”$€   d+    uÄ„   [_Ã¶    ¸òÿÿÿëÚèüÿÿÿt& èüÿÿÿW¹   VS‰Ãì„   d¡    ‰„$€   1Àt$T‰÷ó«‹¹€   ‰$‹B‰D$‹B‰D$‹B‰D$‹B ‰D$‹B(‰D$‹B0‰D$‹B8‰D$‹B@‰D$ ‹BH‰D$$‹BP‰D$(‹BX‰D$,‹B`‰D$0‹Bh‰D$4‹Bp‰D$8‹Bx‰D$<‹‚€   ‰D$@‹‚ˆ   ‰D$D‹‚   ‰D$H‹‚˜   ‰D$L‹‚    ‰â‰D$P‰Øèüÿÿÿ÷ØÀƒàò‹”$€   d+    u
Ä„   [^_ÃèüÿÿÿV‰ÆS‰òìÔ   d¡    ‰„$Ğ   1À‰àèüÿÿÿ‰Ã…Àu‰àèüÿÿÿ‰Ã‰â‰ğèüÿÿÿ…ÀEØ‹„$Ğ   d+    uÄÔ   ‰Ø[^Ãèüÿÿÿèüÿÿÿ‹ ë—´&    èüÿÿÿU‰Ñ‰åV‰ÆS1Ûú–˜ v‰ÈºkÊ_k÷â‰ÓÁëiÃ€–˜ )Á‰ğ‰Ê¹€–˜ ÷ñ‰Ú[^]ÃèüÿÿÿUWVSƒì‹\$4‹|$(‰D$‹L$<L$0‹t$8‹l$,‰È‰|$™‰$ß»ÿÉš;‰l$õ9Ë¹    Ñ‰t$‰ş‰ï}41Ét&  6eÄ‰ÍƒÒÿƒÆƒ× 9ÃÕ|ê…Òy¶     Êš;ƒÒ ƒÆÿƒ×ÿ…Òxî‹$‹L$‹\$‹l$9Ñ‰ÚT$‹$MëMÑ9Ö‰úê|‹T$‰B‰Ğ‰2‰zƒÄ[^_]Ã´&    1À¾ÿÿÿÿ¿ÿÿÿëÔfèüÿÿÿVS‹    …Ût‰Öt& ‹‹CƒÃ‰òèüÿÿÿ‹…Àuë1À[^Ã´&    ´&    èüÿÿÿUWVS‹    …Ût)‰Õ‰Ï´&    f‹C‹3ƒÃÿt$‰ê‰ùèüÿÿÿ‹X…Òuä[1À^_]Ã´&    ¶    èüÿÿÿUWVS‹    …Ût$‰Õ‰Ï´&    f‹3‹CƒÃ‰ù‰êèüÿÿÿ‹…Àué[1À^_]ÃfèüÿÿÿVS‹    …Ût‰Öt& ‹‹CƒÃ‰òèüÿÿÿ‹…Àuë1À[^Ã´&    ´&    èüÿÿÿVS‹    …Ût‰Öt& ‹‹CƒÃ‰òèüÿÿÿ‹…Àuë1À[^Ã´&    ´&    èüÿÿÿUWVS‹    …Ût)‰Õ‰Ï´&    f‹C‹3ƒÃÿt$‰ê‰ùèüÿÿÿ‹X…Òuä[1À^_]Ã´&    ¶    èüÿÿÿUWVS‹    …Ût$‰Õ‰Ï´&    f‹3‹CƒÃ‰ù‰êèüÿÿÿ‹…Àué[1À^_]ÃfèüÿÿÿUWVS‹    …Ût$‰Õ‰Ï´&    f‹3‹CƒÃ‰ù‰êèüÿÿÿ‹…Àué[1À^_]ÃfèüÿÿÿVS‹    …Ût‰Öt& ‹‹CƒÃ‰òèüÿÿÿ‹…Àuë1À[^Ã´&    ´&    èüÿÿÿVS‹    …Ût‰Öt& ‹‹CƒÃ‰òèüÿÿÿ‹…Àuë1À[^Ã´&    ´&    èüÿÿÿUWVSƒì‹    …Ût1‹t$‹|$ ‰$‰L$v ‹C‹+ƒÃWV‹L$‹T$èüÿÿÿ‹XZ…ÉuáƒÄ1À[^_]Ã´&    ´&    èüÿÿÿUWVSƒì‹    …Ût1‹t$‹|$ ‰$‰L$v ‹C‹+ƒÃWV‹L$‹T$èüÿÿÿ‹XZ…ÉuáƒÄ1À[^_]Ã´&    ´&    èüÿÿÿUWVS‹    …Ût$‰Õ‰Ï´&    f‹3‹CƒÃ‰ù‰êèüÿÿÿ‹…Àué[1À^_]ÃfèüÿÿÿS‰Ã)Óƒû>wP‰Ğƒà?‰[Ãt& û÷  wƒÀ[‰ÂÁèƒâøƒà?ƒÀ@‰Ãv û¿  v(ûÿ}  w8   ‰ÂÁè	ƒà?â şÿÿÀ   ë­¶    ƒÀ@[‰ÂÁèƒâÀƒà?ƒè€‰Ãv ûÿï v(ûÿ w@ €  ‰ÂÁèƒà?â €ÿÿ@  ébÿÿÿv    ‰ÂÁèƒà?â ğÿÿ   éBÿÿÿv ûÿÿû w   ‰ÂÁèƒà?â  üÿ€  éÿÿÿûÿÿßw    ‰ÂÁèƒà?â  àÿÀ  éõşÿÿ…Ûx,Â   >ûÿÿÿ>GÂ   ‰ÂÁèƒà?â   ÿ   éÅşÿÿ‰Ğƒà?é»şÿÿt& èüÿÿÿUW¿Àj  V‰ÆS‰ıƒì‰$ë¶    ‰øèüÿÿÿó‹^÷Ã   uó‰Ú¸@a  âÿÿ ‹•    <*÷Ã   u<‰øèüÿÿÿ‰Â‹$‰9^u·ƒÄ‰ø[^_]Ã´&    ¶    U‰åW‰×V‰ÆSƒì‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD…ÀtbMìUè¸   èüÿÿÿ…Àt8‹Uè‹M‰J0¹`   f‰J4‹Mì‰j<ÇB8    ‰xj SRº   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãf‹…Àu˜ëâèüÿÿÿv U‰åW‰×V‰ÆSƒì‰Mä‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD…ÀtMìUè¸   èüÿÿÿ…ÀtS‹M‹Uè‰J0¹`   ‰j<f‰J4ÇB8    ‰x‹O‹}ä‰H‹    ‰x‰H‹M‰H‹Mìj SRº   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãt& ‹…À…wÿÿÿëÜèüÿÿÿ´&    ´&    U‰åW‰×V‰ÆSƒì‰Mä‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD…ÀtMìUè¸   èüÿÿÿ…ÀtM‹M‹Uè‰J0¹`   f‰J4‹    ‰j<ÇB8    ‰x‰H‹O‹}ä‰H‹Mì‰xj SRº   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ã´&    v ‹…À…wÿÿÿëÖèüÿÿÿ´&    ´&    U‰åW‰×V‰ÆSƒì‰Mä‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD…ÀtoMìUè¸   èüÿÿÿ…ÀtD‹M‹Uè‰J0¹`   f‰J4‹M‰j<ÇB8    ‰x‹}ä‰H‹Mì‰xj SRº   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãv ‹…Àu‹ëáèüÿÿÿv U‰åW‰×V‰ÆSƒì‰Mà‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD‰]ä…À„ˆ   MìUè¸$   èüÿÿÿ…Àt^‹M‹Uè‰J0¹`   ‰j<f‰J4ÇB8    ‰x‹O‰H‹O‹_‰H‰X‹O‹_‹}à‰H‹Mì‰X‰x j ÿuäRº$   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãf‹…À…nÿÿÿëŞèüÿÿÿ´&    ´&    U‰åW‰ÏV‰ÖS‰Ãƒì‹S@d¡    ‰Eğ1À‰Uäèüÿÿÿ‹Uä…    ‹CD‰Uä…ÀtwMìUè¸   èüÿÿÿ…ÀtM‹M‹Uè‰J0¹`   f‰J4‰ñ‰j<ÇB8    ‰p‹7‹‰p‰x‹I‰H‹Mìj ÿuäRº   j jSèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãf‹…Àuƒëâèüÿÿÿv U‰åW‰×V‰ÆSƒì‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD…ÀtbMìUè¸   èüÿÿÿ…Àt8‹Uè‹M‰J0¹`   f‰J4‹Mì‰j<ÇB8    ‰xj SRº   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãf‹…Àu˜ëâèüÿÿÿv U‰åW‰ÏV‰ÆSƒì‰Uà‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD‰]ä…À„ˆ   MìUè¸$   èüÿÿÿ…Àt`‹M‹Uè‹]à‰J0¹`   f‰J4‹M‰j<ÇB8    ‰X‹]‰H‰X‹O‰H‹O‰H‹‰H‹O‰H ‹Mìj ÿuäRº$   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ã‹…À…nÿÿÿëàèüÿÿÿ´&    ´&    U‰åW‰ÏV‰ÆSƒì‰Uä‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD…ÀtMìUè¸   èüÿÿÿ…ÀtU‹M‹Uè‰J0¹`   f‰J4‹Mä‰j<ÇB8    ‹U‰H‹M‰P1Ò‰H…ÿt‹W4‰P‹Mìº   j Sÿuèj jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãf‹…À…wÿÿÿëŞèüÿÿÿ´&    ´&    U‰åW‰×V‰ÆSƒì‰Mä‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD…ÀtoMìUè¸   èüÿÿÿ…Àt>‹M‹Uè‰J0¹`   f‰J4‹Mì‰j<ÇB8    ‰x‹}ä‰xj SRº   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ã´&    f‹…Àu‹ëÛèüÿÿÿv V‰ÖS‰Ãƒìd¡    ‰D$‹C$©À  u3¹   ‰Ú‰àèüÿÿÿ…Àt
‰p‰àèüÿÿÿ‹D$d+    uƒÄ[^Ãt& öÄuÈ‰Øèüÿÿÿ„Àt½ë×èüÿÿÿ´&    fW‰ÇV‰ÎS‰Óƒìd¡    ‰D$‹G$©À  uP¹   ‰ú‰àèüÿÿÿ…Àt#‰X‹S‰p‰P‹    ‰P‹T$,‰P‰àèüÿÿÿ‹D$d+    u ƒÄ[^_Ã´&    öÄu«‰øèüÿÿÿ„Àt ëÓèüÿÿÿ´&    fW‰ÇV‰ÎS‰Óƒìd¡    ‰D$‹G$©À  uH¹   ‰ú‰àèüÿÿÿ…Àt‹    ‰X‰P‹S‰p‰P‰àèüÿÿÿ‹D$d+    uƒÄ[^_Ã¶    öÄu³‰øèüÿÿÿ„Àt¨ëÔèüÿÿÿW‰ÇV‰ÖS‰Ëƒìd¡    ‰D$‹G$©À  u@¹   ‰ú‰àèüÿÿÿ…Àt‹T$,‰p‰X‰P‰àèüÿÿÿ‹D$d+    uƒÄ[^_Ã¶    öÄu»‰øèüÿÿÿ„Àt°ëÔèüÿÿÿ´&    fW‰ÇV‰ÎS‰Óƒìd¡    ‰D$‹G$©À  uX¹$   ‰ú‰àèüÿÿÿ‰Á…Àt+‰X‹C‰A‹C‹S‰A‰Q‹C‹S‰q ‰A‰à‰Qèüÿÿÿ‹D$d+    uƒÄ[^_Ãt& öÄu£‰øèüÿÿÿ„Àt˜ëÕèüÿÿÿW‰ÇV‰ÎS‰Óƒìd¡    ‰D$‹G$©À  uH¹   ‰ú‰àèüÿÿÿ…Àt‰X‹~‹6‰x‰p‹S‰P‰àèüÿÿÿ‹D$d+    u ƒÄ[^_Ã´&    öÄu³‰øèüÿÿÿ„Àt¨ëÓèüÿÿÿV‰ÖS‰Ãƒìd¡    ‰D$‹C$©À  u3¹   ‰Ú‰àèüÿÿÿ…Àt
‰p‰àèüÿÿÿ‹D$d+    uƒÄ[^Ãt& öÄuÈ‰Øèüÿÿÿ„Àt½ë×èüÿÿÿ´&    fW‰×V‰ÆS‰Ëƒìd¡    ‰D$‹F$©À  uX¹$   ‰ò‰àèüÿÿÿ…Àt/‰x‹t$,‹|$0‰p‰x‹S‰P‹S‰P‹‰P‹S‰P ‰àèüÿÿÿ‹D$d+    uƒÄ[^_Ãv öÄu£‰ğèüÿÿÿ„Àt˜ë×èüÿÿÿW‰ÇV‰ÖS‰Ëƒìd¡    ‰D$‹G$©À  uP¹   ‰ú‰àèüÿÿÿ…Àt$‰p‹|$01Ò‹t$,‰x‰p…Ût‹S4‰P‰àèüÿÿÿ‹D$d+    uƒÄ[^_Ã¶    öÄu«‰øèüÿÿÿ„Àt ëÔèüÿÿÿ´&    fW‰ÇV‰ÖS‰Ëƒìd¡    ‰D$‹G$©À  u8¹   ‰ú‰àèüÿÿÿ…Àt‰p‰X‰àèüÿÿÿ‹D$d+    uƒÄ[^_Ãt& öÄuÃ‰øèüÿÿÿ„Àt¸ëÕèüÿÿÿV‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ÿvhéÍ  Sèüÿÿÿƒ`  ƒÄ[^éüÿÿÿfV‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ÿvÿvÿvÿvhX? Sèüÿÿÿƒ`  ƒÄ[^éüÿÿÿ´&    fV‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ÿvÿvÿvÿvh„? Sèüÿÿÿƒ`  ƒÄ[^éüÿÿÿ´&    fV‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ÿvhóÍ  Sèüÿÿÿƒ`  ƒÄ[^éüÿÿÿfW‰ÊVS‰Ã‹°x   èüÿÿÿƒøt	[^_Ãt& ‹~ ¹ÓMb‰øÁÿ÷éÁú)úRÿv‹~‰øÁÿ÷éÁú)úRÿvÿvÿvÿvh¨? Sèüÿÿÿƒ`  ƒÄ$[^_éüÿÿÿV‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ÿvÿvÿvÿvhÿÍ  Sèüÿÿÿƒ`  ƒÄ[^éüÿÿÿ´&    fV‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ‹Nh ²  C@ºÎ  á  < èüÿÿÿ‹V‰Á‹FQ‰Á%ÿÿ ÁéQP‰Ğ+FPRÿvÿvhè? Sèüÿÿÿƒ`  ƒÄ([^éüÿÿÿ´&    U‰ÊWVS‰Ã‹°x   èüÿÿÿƒøt[^_]Ãv k@‹V¹@²  ‰èèüÿÿÿ‹V¹ ²  ‰Ç‰èèüÿÿÿWPÿvh0@ Sèüÿÿÿƒ`  ƒÄ[^_]éüÿÿÿ´&    t& V‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ‹V C@¹ ±  èüÿÿÿPÿvÿvÿvÿvÿvÿvhP@ Sèüÿÿÿƒ`  ƒÄ$[^éüÿÿÿfV‰ÊS‰Ã‹°x   èüÿÿÿƒøt
[^Ã´&    ‹VC@¹`±  èüÿÿÿPÿvhÎ  Sèüÿÿÿƒ`  ƒÄ[^éüÿÿÿ1Ééüÿÿÿ´&    fW1ÿVS‹t$‰Ë1ÉW1ÿVWSèüÿÿÿƒÄ[^_ÃW1ÿVS‹t$‰Ë1ÉW1ÿVWSèüÿÿÿƒÄ[^_ÃW1ÿV‰Î1Éÿt$ÿt$WVèüÿÿÿƒÄ^_Ãv W1ÿV‰Î1ÉWVèüÿÿÿXZ^_Ã´&    t& W1ÿV‰Î1ÉWVèüÿÿÿXZ^_Ã´&    t& W1ÿV‰Î1ÉWVèüÿÿÿXZ^_Ã´&    t& W1ÿV‰Î1ÉWVèüÿÿÿXZ^_Ã´&    t& èüÿÿÿ‹@éüÿÿÿv 1Ééüÿÿÿ´&    fW1ÿV‰Î1Éÿt$ÿt$WVèüÿÿÿƒÄ^_Ãv èüÿÿÿUWVS1Ûƒì‹h‰D$½ÿÿÿ?ÇD$    ‰|$xëDt& )ğ…Àx#¶L$(Óâ‹L$9ÊIÑ‰T$‹$÷Úƒâ9ĞvwÁíƒ<$ƒİÿƒD$û@  t`‰î‰\$1Ò‰øƒæ?j Şj ƒÃ@V‰Ùèüÿÿÿ‰éƒá‰L$ƒÄ9Ãw’j ‰ñ1Ò‰øj ÿt$èüÿÿÿƒÄ9Æv¡º@   )òĞélÿÿÿ´&    v ‹|$‹\$‹GÆG ‰$ÿÿÿ?9Ø‰Ø•GƒÄ[^_]Ã´&    fèüÿÿÿU‰ÁWVSd¡    ‹    <@ºÓMb4ş‰ğ÷âÁêiêè  ‹    )î…è  şù   FÅûú‰Æ)Ş9Â[HÎ^_]‰ÈÃt& èüÿÿÿ¸´¿ èüÿÿÿ‹    ¸    …Ét
‹    …Òu èüÿÿÿ¸¤s èüÿÿÿ¸´¿ éüÿÿÿ´&    èüÿÿÿëŞèüÿÿÿW‰×V‰ÎS‰Ã>t& ÇC    ‰{÷ÆÿÿÇÿuSd¡    	ğ‰C[^_Ãt& d¡    £    sÌdÿ    ¡    …Àt
‹@‰Úèüÿÿÿdÿ    u©èüÿÿÿë¢´&    æ  8 ë£´&    ´&    èüÿÿÿVS‰Ã‰Ğ‹S…Ò„š   ‹‰Î…Òtr>t& ‹‹S‰…Àt‰P‰ğ„ÀtÇC    Ç"  ¸   [^Ãt& d¡    £    sÄdÿ    ¡    …Àt
‹@‰Úèüÿÿÿdÿ    u¡èüÿÿÿëš´&    ‹SÁêL`9K…{ÿÿÿ³PÆ@énÿÿÿ1À[^Ã´&    t& èüÿÿÿWV1öS‰Ãƒìd¡    ‰D$‹C…Àt%‰â‰Øèëÿÿ¹   ‰Â‰Ç‰Øèÿÿÿ‹$‰Æ‰øèüÿÿÿ‹D$d+    u	ƒÄ‰ğ[^_ÃèüÿÿÿèüÿÿÿV‰ÆSƒìd¡    ‰D$1À‰â‰ğè¯êÿÿ‰Ã9pt8‰Â¹   ‰ğèªşÿÿ‰Æ‹$‰Øèüÿÿÿ‹D$d+    uƒÄ‰ğ[^Ã´&    f¾ÿÿÿÿëÑèüÿÿÿt& èüÿÿÿW‰ÇVSƒìd¡    ‰D$1Àd¡    ©   t
öG t\t& ‰â‰øè'êÿÿ‰Ã;xt8‰Â¹   ‰øè"şÿÿ‹$‰Æ‰Øèüÿÿÿ…öx"‹D$d+    u!ƒÄ‰ğ[^_Ãt& ‹$èüÿÿÿóë¬t& ë¤èüÿÿÿ´&    ´&    èüÿÿÿUWV‰Æ‰ÈS‰ÓT`‹|$‹
‰…Ét‰Y‰‰S«F‹SÁàâÿÿ? 	Ğ‰C>t& ;~xR[^_]Ãt& ‹Kd‹    £    sŞdÿ    ‹    …Òt‹jP‰Ú‰èèüÿÿÿXdÿ    u¶èüÿÿÿë¯¶    ‰~ÆFÆF >t& [^_]Ãv öCu€~ tŠ‹F[^_]éüÿÿÿ´&    èüÿÿÿW‰×V‰ÆSd‹    ãÿÿÿ>t& ‰ğèüÿÿÿ>t& d¡    %ÿÿÿ9Øt6€=œ    „Â   ´&    v d‹    ‰Ê‰Èâ   €	Úd±    9Áuá[^_Ãt& d¡    £    s—dÿ    ¡    …Àt
‹@‰òèüÿÿÿdÿ    …pÿÿÿèüÿÿÿéfÿÿÿ´&    d¡    £    ƒWÿÿÿdÿ    ¡    …Àt
‹@‰òèüÿÿÿdÿ    …0ÿÿÿèüÿÿÿé&ÿÿÿt& Æœ   PSWh@ èüÿÿÿƒÄé*ÿÿÿ´&    v UWV‰ÆS‰õƒì4d¡    ‰D$0‰ğèüÿÿÿ¡    ;Fˆ¶   ¶    ‹    ‹E9Âˆ   u`‰E}1Ò‰t$L$‰l$‰<$1ÿv ‰Ã‹4$ƒã?Ó³s ‹t$i4‹‰…Ût‰KÇ    ƒÇ‰é¨uƒÂ@Áèú@  u¿‹l$‹EƒÀ…ÿuQ€} „O  ‰E‰èèÍøÿÿ‰E‹E´&    ‹    9Â‰Rÿÿÿ‰î‰ğèüÿÿÿ‹D$0d+    …!  ƒÄ4[^_]Ã‰E‰èè†øÿÿ‰|$t¼‰E‹E´&    ‹xÿ…Òu,é‚   fèüÿÿÿ‹$‰ù‰Øèıÿÿ‰èèüÿÿÿ‹ÇE    …ÀtX‹‰]>t& ‹‹S‰…Àt‰P‹SöC ÇC    ‰èÇ"  ‰$u¨èüÿÿÿ‹$‰ù‰Øè7ıÿÿ‰èèüÿÿÿ‹ÇE    …Àu¨‹Eƒîƒl$…bÿÿÿéÿÿÿt& d¡    £    sˆdÿ    ¡    …Àt
‹@‰Úèüÿÿÿdÿ    …aÿÿÿèüÿÿÿéWÿÿÿ€} „§şÿÿé şÿÿèüÿÿÿfèüÿÿÿS»@a  èüÿÿÿ‹    …    ;Sx‰Øè·ıÿÿèüÿÿÿ»Àj  ‹    …    ‰Ø;Sx	[é“ıÿÿv [Ã´&    ´&    èüÿÿÿW‰ÁV4RºÓMbS0‰Ø÷âÁêiÂè  ‰Ú)Âƒè  ‹    ‰Çó)÷Ó)×9Ã[^HÏ_‰ÈÃ´&    fèüÿÿÿW‰ÁVSRºÓMb4‰ğ÷âÁêiúè  ‹    )ş‡è  şù   FÇÚ‰Æ)Ş9Â[HÎ^_‰ÈÃ¶    èüÿÿÿW‰ÁVSd¡    @ºÓMb4‰ğ÷âÁêiúè  ‹    )ş‡è  şù   FÇÚ‰Æ)Ş9Â[HÎ^_‰ÈÃèüÿÿÿW‰ÁVSd¡    4@ºÓMb1‰Ø÷âÁêiÂè  ‰Ú)Âƒè  ‹    ‰Çó)÷Ó)×9Ã[^HÏ_‰ÈÃv èüÿÿÿU‰ÁW<RºÓMbVS‹    4ş‰ğ÷âÁêiêè  ‹    )î…è  şù   FÅûú‰Æ)Ş9Â[HÎ^_]‰ÈÃ´&    v èüÿÿÿW‰ÁV‹5    SRºÓMbğ<‰ø÷âÁêiÂè  ‰ú)Â‡è  ‹=    ß×9ÇyŞ)ğ)Ğ‰Á[‰È^_ÃèüÿÿÿW‰ÁVSd¡    ‹5    @ºÓMb1<‰ø÷âÁêiÂè  ‰ú)Â‡è  ‹=    ß×9ÇyŞ)ğ)Ğ‰Á[‰È^_Ã´&    fèüÿÿÿWVS‰Ãƒìd¡    ‰D$‹C…À…¾   ‹K…É„³   ¸@a  ‹4•    ‰×öCt¸Àj  Æ‰â‰ØèŸâÿÿ9ğt K   èüÿÿÿ‰ğèüÿÿÿ‹C%  øÿ	ø‰C¡    ‹V‰Á)Ñ…É~‹N9ÈxD9ÑxX‰N‰Ê‹CL$èïàÿÿÿt$‰Ú‰Á‰ğè€øÿÿ‹T$‰ğèüÿÿÿX‹D$d+    u#ƒÄ[^_Ã‰F‰Âë¾´&    f¶    ë©èüÿÿÿ´&    ´&    UWVS‰Ãƒì$‰$d¡    ‰D$ 1À‹C…À„…  ‰ÎöÁ…Â  ‹C…À„·  ÇD$   ‹C+$„~  ‰Ïƒç‰|$t…Àk  T$‰Øè…áÿÿ‰Ç¡    ‹W‰Á)Ñ…É~‹O9Èˆ   9Ñˆ   ‰O‹C…À„¢  ‹l$…ít‹$;C‰d  ‹GL$‰Â‰D$‹$èÉßÿÿ‰D$‰Â‹CÁè9Â„5  t& 1É‰ú‰Øèõÿÿ‰D$…Àuƒæ‰ı…Â   ‹kë÷Å   „±  èüÿÿÿ¾@a  å   ‹…    …  Æ‰õ9÷tV‰ı9_„a  K   ‰ø‰õèüÿÿÿ‰ğèüÿÿÿ‹C%  øÿF‰C¡    ‹V‰Á)Ñ…É~‹N9Èˆ1  9Ñˆ©  ‰N‹$ƒ|$ÿ‰C‹Vt
;T$„L  ‹$L$èàŞÿÿÿt$‰Ú‰Á‰ğèqöÿÿX‹T$‰èèüÿÿÿ‹D$ d+    …Ë  ‹D$ƒÄ$[^_]Ã´&    T$‰Øèõßÿÿ‰Ç¡    ‹W‰Á)Ñ…É   ‹O9ÈxX9Ñx4‰OÇD$ÿÿÿÿÇD$    é¬şÿÿt& èüÿÿÿ¾Àj  4…    éàşÿÿfÇD$ÿÿÿÿÇD$    éyşÿÿ´&    f‰GÇD$ÿÿÿÿÇD$    éXşÿÿ´&    ÇD$ÿÿÿÿÇD$    é;şÿÿv ¶    ‰FéÒşÿÿ´&    èüÿÿÿ‰Â¸@a  å   ‹4•    „Mşÿÿ¸Àj  éCşÿÿ´&    ÿt$‹L$‰Ú‰ğè/õÿÿZé¹şÿÿ´&    f‹$‰CÇD$   ‰ıé›şÿÿv ‰GécıÿÿéSşÿÿ´&    f‹GL$‰Â‰D$‹$è;İÿÿ‰D$‰Â‹CÁè9Â…wıÿÿ‹L$‰ı…Ét‹$;Cx•ÇD$   é8şÿÿ´&    éüüÿÿ´&    f‰şéãıÿÿèüÿÿÿt& èüÿÿÿ¹   éAüÿÿèüÿÿÿ1Éé4üÿÿt& èüÿÿÿ¹   é!üÿÿèüÿÿÿ‹P…Òu‹P¹   éüÿÿ´&    ´&    ´&    èüÿÿÿSèüÿÿÿƒÀt d‹    ´&    fÇC   èüÿÿÿ…Àuğ[Ã´&    ´&    èüÿÿÿSd‹    èüÿÿÿƒÀu ë7´&    ‹ƒâuÇC   èüÿÿÿ…Àt‹â   tß[éüÿÿÿ´&    f1À[éüÿÿÿ´&    èüÿÿÿ‹    ¹¤¿ ¸    éüÿÿÿ¶    èüÿÿÿW‰ÏV‰ÖS‰Ã¸´¿ èüÿÿÿÿt$‰ù‰òÿt$‰Øèüÿÿÿ_Z‰Ã…Àu…öu¸´¿ èüÿÿÿ‰Ø[^_Ã´&    f‹    ¸    …Ét
‹    …Òuèüÿÿÿ¸´¿ èüÿÿÿ‰Ø[^_Ãfèüÿÿÿë°´&    fèüÿÿÿU‰åWV‰ÆS»@a  ƒäøƒì‰$‰L$èüÿÿÿ…    èüÿÿÿ£    ƒC  ‰Øèüÿÿÿ€{ …  ‹C‹S‰ñ)Á9òˆØ   …Éˆ‹   ‹4$‹|$ÆC ‰Øèüÿÿÿèüÿÿÿ9ğ‰ÑùsW‹4$‹|$9Æ‰ùÑsH?B ƒÒ 1ÿ‰Æ‰Ñ…Òt¿ƒŞC‰Ğ÷ç‰Ğ‰×ÁèÁïiÀ@B )Á»@B ‰ğ‰Ê÷ó‰úiÊ@B ÷ã‰×‰ÆÏeô‰ğ‰ú[^_]Ã¶    ‰s€{ ug¸ÿÿÿÿºÿÿÿ+$T$‰Ñ¾ÿÿÿÿ‰Â¿ÿÿÿ¸@B 9Ğ¸    ÈƒGÿÿÿÆCé>ÿÿÿv …Éx´9Â‰$ÿÿÿ‰Céÿÿÿt& ‰Øè	íÿÿ‰Céòşÿÿ)ğ¹@B ‹4$‹|$÷á‰ÑÆ‰ÂÏë¶    eô¾ÿÿÿÿ¿ÿÿÿ[‰ğ‰ú^_]Ã´&    v èüÿÿÿS»@a  èüÿÿÿ‹…    ÆD [ÃfèüÿÿÿV¾@a  S‰Ã¡    d‹    1ĞØd‹    ÁÀ‰ÊÈÁÂ1Ğ‰Úd£    ‰Èèüÿÿÿèüÿÿÿ4…    èüÿÿÿ¡    ;Fy¡    ;†Œ	  x
¸   èüÿÿÿ‰Øèüÿÿÿd¡    ©   uèüÿÿÿ[^éüÿÿÿ¶    èüÿÿÿèüÿÿÿ[^éüÿÿÿ´&    èüÿÿÿ‰Áº@a  …    ¡    ‰Bÿÿÿ?‰B1Àf‰B‹    ¸Àj      ‰PÂÿÿÿ?‰P1Òf‰P1ÀÃèüÿÿÿUW‰ÇVSƒì‰D$d¡    ‰D$1À£=    ÇD$@a  ‚¤  ‹D$‹|$‹4…    ¸   ş‰t$èüÿÿÿèüÿÿÿ<…    ‰øèüÿÿÿ‰ğèüÿÿÿ‹    ‹O‰Ğ)È…À~‹G9ÂˆG  9Èˆ,  ‰G‹D$‹H…É…+  ‹D$p``	  ‰$´&    ‹‹o…ÀtT´&    ‹>t& ‹‹S‰…Àt‰P‹CÇ"  L$%  øÿ	è‰C‹C‹Wèq×ÿÿÿt$‰Ú‰Á‰øèïÿÿ‹Z…Àu³ƒÆ94$u›‹D$èüÿÿÿ‰øèüÿÿÿ¸   èüÿÿÿd¡    …À„”   D$€	  ‹D$=@t  …×şÿÿ‹D$d+    uzƒÄ1À[^_]Ãt& d¡    £    ƒDÿÿÿdÿ    ¡    …Àt
‹@‰Úèüÿÿÿdÿ    …ÿÿÿèüÿÿÿéÿÿÿ‹D$‹H…É„Õşÿÿ‰Wé¼şÿÿèüÿÿÿébÿÿÿèüÿÿÿffffèüÿÿÿU‰Á‰åW‰×V‰ÆS‰Óƒì‹EM]1Òƒ} Ğ¸    MUME‰$9ğ‰Ğø‹$L×LÆ9Á‰ØĞ}
¹ÿÿÿÿ»ÿÿÿƒÄ‰Ú‰È[^_]Ã´&    t& èüÿÿÿS´&    v ‹H ‹QöÂtt& ó‹QöÂuö€x$ u9At‹Y9ÓuÔ‹P 9ÑuÍ1À[Ã¸   [ÃfèüÿÿÿU‰åWV‰ÆSƒì‹E‰T$‰D$‹E‰D$…Étj‰Ëó¼É‰³ËDÆ8…ÀtV‹x9|$t}‰‹W‹O+TÆXLÆ\‰Ğ‰Ê;D$L$}‰D$‹D$‰T$…Àt>…Ût¸   ó¼ËÓà÷Ğ!Ã‰DÆ8…Àuª‹t$‹|$1À1Ò‰û9ÆÓMÆM×ƒÄ[^_]Ã€& t*‰~4ë·t& ‹D$‰$èüÿÿÿ‹$…À‰Ç…jÿÿÿë—t& ‰~(ë´&    t& U¸   ‰å]éüÿÿÿfU¸   ‰å]éüÿÿÿfU1À‰å]éüÿÿÿt& èüÿÿÿU‰åWV‰ÆS‰÷‰Ó‰Ùƒäøƒì÷ß‹U‹EƒÑ ÷ÙI÷HË‰4$‰Ö‰L$Áş‰Ñ	ñt61É‰Ö¬ĞƒÁ‰÷ÑşÁÿÑú	÷uê‹|$‹4$1Ò­şÓïƒá E÷Eú‰4$‰|$‹T$‰Á1ÿ‹$‰Æ…Òt‰Ğ1Ò÷ñ‰Ç‰ğ÷ñ‰ú…Ûy÷ØƒÒ ÷Úeô[^_]Ã´&    v èüÿÿÿW|$ƒäøÿwüU‰åWVS‰Ã‰Ğ‰Êƒì4‰MÄ‹‹‹s‰EÀ‰}Ğ‹{)ğ‰uàú‰}ä‰EØ‰UÜ…Òˆõ   öC$…û   ¡    1Ò‹}Ğ‰Ö9Èş‹uØMÈ‰Ğ‹UäLÇ‹}Ü9Î‰Mè‰Eì‰ùMì‹EàÜ   ‹s‹KÇEĞ   ÇEÔ    ‰uÌ‰MØ‰uÜ‹uè‹}ìğú;uà}ä‹}èL}à‹uäMuì9ø‰×÷}
¸ÿÿÿÿºÿÿÿ‹uè‹}ì‰C‹EØ‰S‹UÜğú‰Eè‹EÌ9Î‰Uì‰úÂ‹UìMÇMÎ‰Æ‹Eè9È‰Ğğ}ÇEèÿÿÿÿÇEìÿÿÿ‹Eè‹Uì‰C‰S‹EĞ‹UÔeô[^_]gø_ÃÇEĞ    ÇEÔ    ëŞÇEĞ    ÇEÔ    ëÌ¶    ‹EØ‹UÜÿuìÿuèèüÿÿÿY^‰×‰Æ‰Ñ‹Eè‹Uì‰uĞ¯È‰}Ô‰Ğ¯ÆÁ‰ğ÷eè‰Æ‹Eà‰×‹UäÏğús{‰Eà‰uØ‰ñ‰s‹uÀ‰}Ü‰}Ì9Æ‰{‹}Ä‰Uä×‰C‰SŒOÿÿÿƒEĞƒUÔ é½şÿÿt& èüÿÿÿU‰Õ‰ÊWÁêV‰ÎS‰Ã‰Ğƒâ{‰Ùƒàƒçü)ùƒìÇ    ƒÁ(ˆD$1ÀÁéÇC$    ó«¹    <•    d    …íu=‰õƒå‹­à²  ƒøt1Ç¶D$Áî‰ˆC&‰ğƒàˆC'¿DÁ8‰C ƒÄ[^_]Ãv ƒı~Ã‰L$Uhè@ èüÿÿÿXZ‹L$ë¸´&    v èüÿÿÿ‹P(Ç@(    …Òt‰Ğèüÿÿÿ1ÀÃv èüÿÿÿUWVS‰Ã¸    ƒì‰T$‰D$èüÿÿÿ‹k ‹S‹K‹…    ‰Ö+u ‰Ï}$‰D$‰4$‰|$…Éˆ³   ‹$‹T$1ö1É9ğ‰Ğ‹T$È‹$LÑMğ€{& ‹E u\‹D$D$9E tƒÄ[^_]Ã´&    ;p ‰ÑH$}ç¶HöÁuŞ‰X(‰p ‰P$öÁtĞƒáuËƒÄ‰ğ¹   [^_]éüÿÿÿ´&    ö@u«;p,‰×x0}¡;p ‰×x$‰X4‰p,‰P0}€|$ t‡éuÿÿÿéFÿÿÿ´&    fèüÿÿÿV‰ÆS‰Ó>t& ‹‹K¸   Óà	BC‰òÆF$èüÿÿÿ[^¶ÀÃt& d¡    £    sÉdÿ    ¡    …Àt
‹@‰òèüÿÿÿdÿ    u¦èüÿÿÿëŸ´&    ´&    èüÿÿÿU‰åWVS‰Ã‹Hö@t*Ç@(    ƒá1Òhÿÿÿjÿè¢ùÿÿ‰Ç‰ÖXZeô‰ø‰ò[^_]ÃÇ@4    áğ   1Òhÿÿÿjÿèuùÿÿ‹KÇC(    ‰C,‰Ç‰Ö‰Ø‰S0ƒá1ÒhÿÿÿjÿèNùÿÿ‰ñƒÄ9ÇÑ}‹C4‰ò‰C(eô‰ø[^_]Ã‰Ç‰Öë´&    t& èüÿÿÿV‰ÖS‰Ãè@ÿÿÿ…öt‹K$‹s 1Ñ1Æ	ñt¶K‰C ‰S$öÁtƒát
[^Ã´&    [¹   ^éüÿÿÿt& èüÿÿÿUWVS‰Ã¶@$‹:‹l$ˆK$¨u[^_]Ã´&    ‰ÖB‰Úèüÿÿÿ„Àu‹N¸şÿÿÿÓÀ!G…ítÓ9_(uÎ[‰ø^º   _]éRÿÿÿfèüÿÿÿU1íWVS‰Ãƒìèüÿÿÿ„ÀuƒÄ‰è[^_]Ãt& ‹‰úèüÿÿÿót& ‹s ş¸Ó tî‹èüÿÿÿ‰Ç‹C 9ÆuÕ½ÿÿÿÿ;Xt1íöC$u‹ ‰úèüÿÿÿ‰èƒÄ[^_]Ã´&    >t& ‹.º    ‰$èüÿÿÿ‹$…    1À9Õ‰ò½   ”À1ÉP‰Øèêşÿÿ‹C Zë©t& d¡    £    s¶dÿ    ¡    …Àt
‹@‰Úèüÿÿÿdÿ    u“èüÿÿÿëŒ´&    èüÿÿÿS‰Ã‰Øèüÿÿÿ…Àx[Ãv óëìt& èüÿÿÿUW‰ÇVSƒì(‰T$‹D$@‰L$d‹    ‰T$$1Ò#G„P  ó¼È³È‰D$‰lÇ8…í„6  ‰‹]‹T$‰Á4Å    ‰$‹D$DÏXTÏ\‰t$‰D$‰T$ …Û„Ğ  L7L‰L$ë9¶    ƒE‹$ƒE9\ÇH…ó  ‹$ÇDÇH    ‹]…Û„•  ‹D$‹T$ ;C‰ĞCŒ  >t& ‹$‰\ÇHƒE‹D$ƒE¶C$‹q8‰L$ÆC$ ¨t"‹D$‰Úèüÿÿÿ„Àu‹L$¸   ‹I<Óà÷Ğ!F‹T$<‰ø‹sèüÿÿÿ>t& ‰Øèüÿÿÿ‰Æ>t& ‰øèüÿÿÿ…ö„?ÿÿÿöC$…5ÿÿÿ1É‰ê‰Øèšûÿÿé%ÿÿÿt& d¡    £    ƒSÿÿÿdÿ    ¡    …Àt
‹@‰Úèüÿÿÿdÿ    …,ÿÿÿèüÿÿÿé"ÿÿÿt& d¡    £    ƒwÿÿÿdÿ    ¡    …Àt
‹@‰Úèüÿÿÿdÿ    …PÿÿÿèüÿÿÿéFÿÿÿt& d¡    £    ƒ!ÿÿÿdÿ    ¡    …Àt‹@L$‰Úèüÿÿÿdÿ    …öşÿÿèüÿÿÿéìşÿÿ´&    ‹T$…Òt(¸   ó¼ÊÓà÷Ğ!Â‰lÇ8‰T$…í…Ğıÿÿ¶    ‹D$$d+    uƒÄ([^_]Ãv éşÿÿèüÿÿÿ´&    t& èüÿÿÿUWVS»    èüÿÿÿ…    ‰Ø»Ğ   èüÿÿÿ‹¨   “€   W‰ÆCèüÿÿÿ‹»€   ‹«„   ‰Ñ‰Â‰Ø‰»   ‹»¨   ‰«$  ‹«¬   ‰»H  ‹»Ğ   ‰«L  ‹«Ô   ‰»p  ‰«t  hğ   Vè¹üÿÿ€c÷‹K1ÒÇC4    ‰Øhÿÿÿáğ   jÿèåóÿÿòÿÿÿ÷ĞƒÄ	Ât‹C4º   èiøÿÿ‰ò‰Ø[^_]éüÿÿÿ´&    t& èüÿÿÿVS»    èüÿÿÿ…    öCu‹    …Ò„´   ‰Ø³Ğ   èüÿÿÿ‹¨   “€   VCèüÿÿÿ‹ƒ€   ‹“„   ‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒĞ   ‰“L  ‹“Ô   ‰ƒp  ‰“t  X‰ØöCt1è<ùÿÿ¶K‰C ‰S$öÁt"ƒáu¹   èüÿÿÿ‰Ø[^éüÿÿÿt& èùÿÿ‰Ø[^éüÿÿÿf[^Ã´&    ¶    èüÿÿÿW‰ÏV‰ÖS‰Ã>t& ‰ù‰ò‰Ø[^_é_öÿÿ´&    d¡    £    sÜdÿ    ¡    …Àt‹@Q‰Ñ‰ÚèüÿÿÿXdÿ    uµèüÿÿÿë®´&    t& èüÿÿÿW‰ÏV‰ÖS‰Ã>t& ‰Ø‰ù‰òèòõÿÿÇCĞË d¡    ‰C([^_Ã¶    d¡    £    sÌdÿ    ¡    …Àt‹@Q‰Ñ‰ÚèüÿÿÿXdÿ    u¥èüÿÿÿë´&    t& èüÿÿÿU‰åWVS‰Ãƒì‹K ù¸Ó tN‹‰$èüÿÿÿ‹$‰D$;K u/‹s‹{‹Aèüÿÿÿ)Æ‹C ×‹T$‹ èüÿÿÿ‰ğ‰úƒÄ[^_]Ãv ‰Â‹èüÿÿÿóë£v èüÿÿÿUW‰ÇVSƒì(‰$‹D$<‰T$‹T$@‰D$‹D$D‰T$‹$‰D$Áèƒğ‰L$ƒà€& ”Â8Ğ…´  ‹s ş¸Ó „ö  ‹èüÿÿÿ;s …İ  ‹>‰t$º    ‰D$$‰$‰T$èüÿÿÿ‹.‹T$…    9×¶{$”Â9](”À!ÂˆT$÷Ç   …¬  ‹\$‹t$1í1ÿ‰ğ9ë‰Ú‰ñøMëMşöD$…•   ‹\$‹t$9ë‰ğøMëMş‹$‹\$‹t$‰X‹\$‰p‹t$ÓÎ9ë‰Ø‰ñ‰òù}
¸ÿÿÿÿºÿÿÿ‹4$€|$ ‰F‰V„Ò   ‹t$‹L$‹$‰òèÈõÿÿ‹º   èìöÿÿ‹$‹T$$‹@ ‹ ƒÄ([^_]éüÿÿÿt& ‹D$‹@èüÿÿÿ‹L$‰Ö‹T$‰Ã‰ğÚñ9ë‰T$ø‰È‰L$MëMş9êø}0ºÿÿÿÿ¹ÿÿÿ½ÿÿÿÿÇD$ÿÿÿÿÇD$ÿÿÿ¿ÿÿÿéÿÿÿ´&    ‹D$1ÿ1í9l$øM|$Ml$‰ø9ÕÈLêLùéîşÿÿ´&    ‹D$‹l$¿    ‹Hƒå‰L$èüÿÿÿ‹L$<…    ‰|$éx  …í„à  ‰‹t$‹$‰ı‰D$Áà‰D$ ƒÀ8‰D$ër‹C ;X„¤   ÇC ¸Ó ‹èüÿÿÿ‹D$ L ‹A8‰$èüÿÿÿ9l$„y  ‹$‹|$‹C‹S‹I8+DıXTı\;A ‰ĞA$S  ‰Èèüÿÿÿ‹èüÿÿÿ‰s ‹l$‹D$è‰D$9Æu€9l$t*‹D$‹SLÅ ‹C+AXQ\‹I8;A ‰ĞA$|Ç´&    f‰t$‰$‹L$‹T$‹$èéóÿÿ…À„%şÿÿ‹$º   èÄòÿÿéşÿÿ´&    ‰Â‹èüÿÿÿóéôüÿÿ´&    >t& º    ‰T$ èüÿÿÿ‰ú¶Ê‹T$ …    9Õ¶T$”Àƒò!Ğ¶ÀP‹D$‹T$èíôÿÿXéıÿÿ´&    ‹|$é‡şÿÿ´&    d¡    £    s$dÿ    ¡    …Àt‹@‹$èüÿÿÿdÿ    tG‹D$‹(égÿÿÿf‰ûéCüÿÿ´&    ‹l$‰$‰k éøşÿÿèüÿÿÿ¿    ‹L$<…    éşÿÿèüÿÿÿë²´&    ´&    èüÿÿÿS‹H‹X+HRX‹PSQ‹HèüÿÿÿƒÄ[Ã´&    t& èüÿÿÿUWVS»    ƒì ‰D$d¡    ‰D$1À‰Ød    ö@u‹5    …ö„À  ÇD$    ½ÿÿÿÿèüÿÿÿ‰Ø‰D$´&    v ‰èº    èüÿÿÿ‰Å;    ƒN  ‹\$­    ‰Ø»Ğ   èüÿÿÿ‹s‹¨   “€   ‰D$CWèüÿÿÿ‹ƒ€   ‹“„   ‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒĞ   ‰“L  ‹“Ô   ‰ƒp  ‰“t  Y;s„²   ¶SöÂ…¥   ‹t$#s„˜   ¸   ó¼ÎÓà÷Ğ!ğ4‰tó8…öt~‹{ ƒâˆT$‰Ú‰<$‹{$‰|$‹^‰4Ê‹K‹[+NX^\‰ß;$|$Œ²   ƒ~@~€|$ u;J,‰ßz0Œ—   …Àt#»   ó¼ÈÓã÷Ó!Ø‰tÚ8…öu£´&    f‰Ó‹T$‰Øèüÿÿÿéşÿÿ¶    ¸   èüÿÿÿj1Éº Ó D$èüÿÿÿ¸   èüÿÿÿZd¡    …Àt1èüÿÿÿèüÿÿÿ‹D$d+    u!ƒÄ [^_]Ã´&    ‰Óğ«l$ëˆèüÿÿÿëÈèüÿÿÿ´&    v èüÿÿÿ¸ª   é±ıÿÿèüÿÿÿ‹    ¹@Ó ¸    éüÿÿÿ¶    èüÿÿÿ1Àé´öÿÿt& èüÿÿÿU‰åWVS»    ƒìèüÿÿÿ…    ‰Øèüÿÿÿ‰Eğ¶C¨t'¾ÿÿÿÿ¿ÿÿÿ‹Uğ‰Øèüÿÿÿeô‰ğ‰ú[^_]Ã´&    ¨uD‹KÇC4    1Ò‰Øhÿÿÿjÿáğ   èäéÿÿ‹s4Y_‰s(‹KR1ÒPƒá‰ØèËéÿÿ‰Æ‰×XZëv ¸ÿÿÿÿºÿÿÿ1öëÓfèüÿÿÿU‰åWV‰ÆS»    ƒìèüÿÿÿ…    ‰Øèüÿÿÿ¶K‰EğöÁt\ƒá¸ÿÿÿÿºÿÿÿt-‹KR‰òP‰Øƒáè\éÿÿ‰Æ‰×XZ‹Uğ‰Øèüÿÿÿeô‰ğ‰ú[^_]Ã‹Khÿÿÿ‰ò‰Øjÿáğ   è'éÿÿY_ë¶v ¾ÿÿÿÿ¿ÿÿÿë¾t& èüÿÿÿUWV‰ÆS»    ƒì èüÿÿÿ…    öC„7  ƒC‰Ø»Ğ   ÇFÿÿÿÿÇFÿÿÿèüÿÿÿ“€   ‹¨   ‰ÅC‰|$‰T$‰L$‰D$Wèüÿÿÿ‰D$‰Æ‰×‹ƒ€   ‰T$ ‹“„   ‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒĞ   ‰“L  ‹“Ô   ‰ƒp  ‰“t  XÇ$   ¶C‰ùÇC ÿÿÿÿÇC$ÿÿÿ‰ÂƒÊ;s,K0ˆS|ƒÈ
ÇC,ÿÿÿÿˆC¸   ÇC0ÿÿÿèüÿÿÿj‰ù‰ò‰ØUè¥ğÿÿ‰Øèîÿÿ€cı‰C ‰Æ‰×‰Ø‰S$‰êèüÿÿÿ1É‰ú‰ğèüÿÿÿZY…À„  ‰Øèüÿÿÿÿt$‹L$‹T$‰Å‹D$èüÿÿÿfƒC‰Æ‰×‹ƒ€   ‹“„   ‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒĞ   ‰“L  ‹“Ô   ‰ƒp  ‰“t  Xƒ,$…ÿÿÿfƒC‰ê‰Ø€Kèüÿÿÿ‰ğ‰ú+D$T$‰$‰T$;Cv‰C‹$¸ áõ‹\$‰ú9È¸    Ø‰ğ|0$T$¹   èüÿÿÿ€=    „d¾  ƒÄ [^_]Ã´&    v  áõƒÒ ëÍ¶    €cûƒÄ [^_]Ãt& ´&    ´&    èüÿÿÿUWV¾    Sèüÿÿÿ‹…    óöCt[^_]Ã´&    f¶¨$  ƒğ¶Àèüÿÿÿ…À…¶   ‰Ø»Ğ   èüÿÿÿ‹¨   “€   W‰ÆCèüÿÿÿ‰Å‹ƒ€   ‰×‹“„   ;k,‰ƒ   ‹ƒ¨   ‰“$  ‹“¬   ‰ƒH  ‹ƒĞ   ‰“L  ‹“Ô   ‰ƒp  ‰øC0‰“t  Y|€K¸   ÇC,ÿÿÿÿÇC0ÿÿÿèüÿÿÿj‰ê‰Ø‰ùVèaîÿÿX‰ØZ‰ò[^_]éüÿÿÿfèüÿÿÿ4…    èüÿÿÿ…À……¾  €NÇ       èüÿÿÿ1À[^_]é¹ñÿÿ´&    fèüÿÿÿ‰Á‰Ğ‹Qƒútƒúu,‹Qèüÿÿÿ…Àu¸üıÿÿÃ´&    ‹Qèüÿÿÿ…Àtæ¸òÿÿÿÃ´&    t& èüÿÿÿUWVS‰Ëƒì@‰T$‰$d¡    ‰D$<1Àd¡    ‹PH‹¸h  ‹¨l  …Òˆr  ƒxHch  ‰|$‰l$>t& t$‹T$T‰Ù‰ğèjèÿÿ‹$‹L$d¡    ‰D$8‰ĞÇ‰T$$‰ÊÍ‰<$1ÿ‰l$‰Õ‰L$(1É9øÍ‹l$ÇD$,ĞË MÊMø‹T$‰È9ïĞ‹$LÊLı‹T$9ø‰ĞÈ}Ç$ÿÿÿÿÇD$ÿÿÿ‹$‹T$‰D$‰ğ‰T$ ‰Úè¼Y  =üıÿÿu:…Û„¡   ‹L$0‹\$ d‹    ‹I‰šP  Ç‚<  0[  ‰Š@  ‹L$‰ŠL  ‹T$<d+    …‡   ƒÄ@[^_]Ã¶    d¡    £    ƒòşÿÿdÿ    ¡    t$…Àt‹@S‰ò‹L$XèüÿÿÿXdÿ    …Åşÿÿèüÿÿÿé»şÿÿf¸şıÿÿëŠ´&    fÇD$    1ÿ1íÇD$    é‡şÿÿèüÿÿÿfèüÿÿÿWVSƒì‹Xd‹    ‰T$1Ò‹‰àèüÿÿÿ…À…”   ‹T$‹4$¸êÿÿÿ…ÒxZ‹L$ùÿÉš;wNd‹=    1À…Û‰ŸH  •ÀÀ‰‡D  ¸}Á%9ğ¸   Ğ|<¸ Êš;iÚ Êš;÷æÚ1ÛÈÚj¹   èüÿÿÿZ‹T$d+    u$ƒÄ[^_Ã¶    ¸ÿÿÿÿºÿÿÿëÍt& ¸òÿÿÿëÏèüÿÿÿt& èüÿÿÿ¹    S…    Q8™x  t& ‰
ƒÂ(ÇBä    ÇBì    ÇBğ    9Úuâ‰A1À€aúÇA    ÇA(    ÇA4    ÇA ÿÿÿÿÇA$ÿÿÿÇA,ÿÿÿÿÇA0ÿÿÿ[Ã¶    èüÿÿÿ£    ‚Ş  UWVS‰Ãƒìèüÿÿÿ¸   èüÿÿÿú‹<    ¾    ÷‰|$èüÿÿÿ4…    ‰ğ‰t$èüÿÿÿ‰øèüÿÿÿ‰ø8x  ‰ı‰D$‹_‹t$G+t$‰D$ş…Û„«   ‰<$ëNt& >t& ¶C$‹} ÆC$¨t‹D$‰Úèüÿÿÿ„Àu‹M¸   Óà÷Ğ!G‰s ‰Ø1É‰òèĞæÿÿ‹$‹X…ÛtV‹C 9Xu®t& d¡    £    sdÿ    ¡    …Àt
‹@‰Úèüÿÿÿdÿ    …wÿÿÿèüÿÿÿémÿÿÿ´&    ‰ÇƒÇ(ƒÅ(9|$…)ÿÿÿ‹|$‰øöGu9‹OÇG4    1Ò‰$áğ   hÿÿÿjÿèàÿÿY[òÿÿÿ÷Ğ	Ât
‹G41Òè	åÿÿ‹D$»    èüÿÿÿ‹D$èüÿÿÿèüÿÿÿ‹…    öDt»    èüÿÿÿ…    t‹…Àtèüÿÿÿû¸/ç º   èüÿÿÿ1ÀƒÄ[^_]Ã´&    fffffffU‰åWVSƒäøƒìt& ‹@Õ ‰È‰L$ƒàkÀ4‹hÕ ‹ˆlÕ ˜DÕ ‹¸LÕ ‹°HÕ ‰$‹PÕ ‰L$‹ˆTÕ ‹€DÕ ‰T$‰L$‹èüÿÿÿ+D$T$‰Á!ú!ñ¬şÑï‰ğ‰|$‹|$÷Ğ‰Æ¸    ÷×!Î!×	÷‰Ç‹sDùEĞ‹C‰t$‰|$‹{ ‹[‰|$‹=@Õ ‹L$9ù…Gÿÿÿ‰Æ‹D$‰Ñ¯Î÷æÊD$‰ÙT$­Ğ1ÛÓêöÁ EÂEÓ$T$eô[^_]Ã´&    ´&    èüÿÿÿU‰åWVSƒäøƒì‹< ³  ‰$‰T$‹5@u ÷Æ   u#‹‹_¡@u 9Æuä‹$‹T$ÈÚeô[^_]ÃfóëË´&    t& èüÿÿÿU‰åWV‹@u öÂu#‹5¬u ‹=°u ‹@u 9Êuß‰ğ‰ú^_]Ãt& óëÎt& èüÿÿÿUWV‰ÆS‹@u öÃuB‹\u ‹du ¡`u ‹=¬u ‹-°u ­ĞÓêöÁ ‰>EÂ‰n‰F¡@u 9Ãu¼[^_]Ãt& óë¯t& èüÿÿÿ¡    S…Àu1‹@u öÂu¡Xu ‹\u ‹@u 9ÚuàÓè[Ãt& óëÓt& ëË´&    t& èüÿÿÿU‰åWVSƒäøƒì‹5    ‹… ³  …öu~¡@u ¨um‹lu ‹3‹{‹hu ‰L$‹\u ‰4$‹5`u ‰|$‹=du ‰T$‹@u 9Ğu»‹D$‹T$$T$­ş1ÛÓïöÁ E÷Eûğúeô[^_]Ã¶    óë†t& é{ÿÿÿ´&    fèüÿÿÿU¡    ‰å…Àu¡´u 1Ò]Ã¶    ¡´u 1Ò]Ãt& èüÿÿÿ‹    ¡Du …Òu…Àt‹èüÿÿÿÃ1ÀÃ´&    ¶    èüÿÿÿVS‰Ã¸    èüÿÿÿ‰Ú‰Æ¸0t èüÿÿÿ¹Du º   ‰Ã¸0t èüÿÿÿ‰ò¸    èüÿÿÿ‰Ø[^Ã´&    èüÿÿÿVS‰Ã¸    èüÿÿÿ‰Ú‰Æ¸0t èüÿÿÿ‰ò‰Ã¸    èüÿÿÿ‰Ø[^Ã´&    t& èüÿÿÿUWVS‰Óƒì‰$‹D$4L$‰D$‰Ç‹Wd¡    ‰D$1À‹Qÿt$4ÿt$4èüÿÿÿ‰Ö‰ÇƒÄºÿÿÿÿ…Û…Ş   ½$DÂº@   ƒÀ¹ÿÿÿÿ)Â…ö…İ   ½ÇDÁƒÀ9ÂŒÿ   ºÿÿÿÿ…Û…â   ½$DÂƒÀº@   ‹L$½ÿÿÿÿ)Â‹D$‰D$…À…¨   ½ÁDÅƒÀ9ÂŒ·   ‹,$‰Ú¯Ù‰ø¯×¯õÖ÷å‰4$‹<$‰,$‰Æ‰è‹l$×¯è‹$ÿt$0ÿt$0÷áİêèüÿÿÿ‹L$ğú‰‰QX1ÀZ‹T$d+    u`ƒÄ[^_]Ã½ÃDÂº@   ƒÀ!¹ÿÿÿÿ)Â…ö„#ÿÿÿ½ÆDÁƒÀ!éÿÿÿt& ½D$DÅƒÀ!éQÿÿÿ½ÃDÂƒÀ!éÿÿÿf¸µÿÿÿë“èüÿÿÿt& èüÿÿÿUWVS‰Ãƒì‹st‹{xd¡    ‰D$1À‹C|÷Şƒ× ÷Ø™÷ßR‰ù‰òPD$èüÿÿÿ‹D$‹|$º}Á%‹t$ƒÄ‹«€   9Âº   ‹‹„   úŒÍ   º Êš;iÿ Êš;÷âú1ÿÆ×1õ1ù	é…Ä   ‹D$$‹t$$‹|$(‰Ct‹D$(÷Şƒ× ‰Cx‹D$,÷ß‰ù‰C|÷Ø™R‰òPD$èüÿÿÿ‹D$‹L$‹t$Zº}Á%9Âº   _ÊŒ|   º Êš;iÉ Êš;1ÿ÷âÊÆ×¸ Êš;÷«˜   ‰³€   ‰»„   ğú‰ƒ   ‰“”   ‹D$d+    u?ƒÄ[^_]Ãv ¾ÿÿÿÿ¿ÿÿÿ1ù1õ	é„>ÿÿÿfé5ÿÿÿ´&    f¾ÿÿÿÿ¿ÿÿÿëèüÿÿÿ´&    ´&    èüÿÿÿUWVSƒì‹@u öÂ…•   ‹-\u ‹`u ‹du ‹5¬u ‰l$‹-Àu ‰L$‹=°u ‰\$‹¸u ‰,$‹¼u ‹-@u 9êu¨Î¶L$‹,$ß‰t$‹\$‹t$‰|$­óÓîöÁ EŞİ‰ëÁûSU‹T$‹L$èüÿÿÿƒÄ [^_]Ã´&    óéUÿÿÿ´&    fèüÿÿÿS‰Á‰ÓBƒ‹‰S‹Q‰P‹Q‰P‹Q‰P‹Q‰P‹Q‰P‹Q‰P‹Q‰P‹Q ‰P ‹Q$‰P$‹Q(‰P(‹Q,‰P,‹Q0‰P0ƒ‹K‰K8‹H‰K<‹H‰K@‹H‰KD‹H‰KH‹H‰KL‹H‰KP‹H‰KT‹H ‰KX‹H$‰K\‹H(‰K`‹H,‰Kd‹@0‰Ch[ÃèüÿÿÿU‰åS‰Ãƒäğƒì ‹Èu d¡    ‰D$¡Äu +Ìu Ğu ‰Ñ‰Â‰àèüÿÿÿ‹$‰‹D$‰C‹D$‰C‹D$d+    u‹]üÉÃèüÿÿÿ´&    fèüÿÿÿU¡    ‰å…Àt¡8t ‹<t ]Ãfd¡    ]éüÿÿÿt& èüÿÿÿUWVS‰Ãƒì¡Du €äu ‰Du ‹S‰D$‹C‰Lu £Hu ¡Du ‹èüÿÿÿ‰xu £Pu ‰Tu ‹s‹{£„u ¸@B ‰5|u 1ö‰=€u ‰ˆu ‹K1Ò¥ÂÓàöÁ EĞEÆ‹s‰Ç‰$‰ğ‰ÕÑè‰T$1Òøê1ÿ‰Á…Òt‰Ğ1Ò÷ö‰Ç‰È¹   ‹l$÷ö1ö‰ú9È‰×÷CÈCò‰v ‰È‰5v ‹{÷c¯ş£v ú‹<$‰v )Ç‰ÈÕ‰=v ‰-v ¯s÷c‹l$ò£v ‰ v …ítZ‹K¡`u ‹du ‹5”u ‹=˜u +Mˆº   ¥Â1íÓàöÁ EĞEÅ¥÷Óæ£`u 1Àƒá EşEğ‰du ‰5”u ‰=˜u ‹C‹T$¹    1öÇ,v     £\u £u ‹$Ç0v     +K¥ÂÓàöÁ ‰4v EĞEÆ‰(v £$v ‹CÇ8v     £Xu £Œu Ç<v     ƒÄ[^_]Ã´&    f÷Ù1í­ĞÓêöÁ EÂEÕ­şÓï£`u 1ÀöÁ E÷Eø‰du ‰5”u ‰=˜u é?ÿÿÿfèüÿÿÿU‰åWVSƒäøƒì´&    ‹ÀÕ ‰È‰L$ƒàkÀ4‹ğÕ ‹ˆôÕ ˜ÄÕ ‹¸ÌÕ ‹°ÈÕ ‰$‹ĞÕ ‰L$‹ˆÔÕ ‹€ÄÕ ‰T$‰L$‹èüÿÿÿ+D$T$‰Á!ú!ñ¬şÑï‰ğ‰|$‹|$÷Ğ‰Æ¸    ÷×!Î!×	÷‰Ç‹sDùEĞ‹C‰t$‰|$‹{ ‹[‰|$‹=ÀÕ ‹L$9ù…Gÿÿÿ‰Æ‹D$‰Ñ¯Î÷æÊD$‰ÙT$­Ğ1ÛÓêöÁ EÂEÓ$T$eô[^_]Ã´&    U‰åWVSƒäøƒìt& ‹ÀÕ ‰È‰L$ƒàkÀ4‹èÕ ‹ˆìÕ ˜ÄÕ ‹¸ÌÕ ‹°ÈÕ ‰$‹ĞÕ ‰L$‹ˆÔÕ ‹€ÄÕ ‰T$‰L$‹èüÿÿÿ+D$T$‰Á!ú!ñ¬şÑï‰ğ‰|$‹|$÷Ğ‰Æ¸    ÷×!Î!×	÷‰Ç‹sDùEĞ‹C‰t$‰|$‹{ ‹[‰|$‹=ÀÕ ‹L$9ù…Gÿÿÿ‰Æ‹D$‰Ñ¯Î÷æÊD$‰ÙT$­Ğ1ÛÓêöÁ EÂEÓ$T$eô[^_]Ã´&    ´&    U‰åèüÿÿÿÌu ]Ğu Ã´&    v èüÿÿÿUWV‰ÖS‰Ãƒìƒâ…/  èüÿÿÿ‰Ñ‰Ç‰ƒ¤   ñÿÿÿ÷×‰“¨   	ùt+ƒ€   “„   ‰ƒ¤   ‰“¨   ‹CtCh‰Ç‹Sx¸ Êš;SliÊ Êš;÷ç‰T$1ÒL$‹K‰$‹C|$T$‰C$‹C‰S(‹S ­ĞÓêöÁ t‰ĞC|=ÿÉš;vƒÇi‹°    Êš;¸ Êš;‰{p‰÷÷£¬   Ê‰CX‰Ø‰S\èüÿÿÿ1Òƒç‰Ù•Â¸0t èüÿÿÿ‹ƒ€   C$‹“„   S(‰C,‰Ø‰S0ºÀÕ è#ùÿÿC4º@Õ èùÿÿ…ÿtƒƒœ   ƒæt¿@t ¾Du ¹?   ó¥ƒÄ[^_]Ãt& Ç€è       Ç€ì       èüÿÿÿé³şÿÿ´&    v èüÿÿÿUWVSƒì¡Du ‹èüÿÿÿ‹=Lu ‹5Hu ‰Á+Pu ‰ÓTu ‰Í!û£Pu !õ¬ş‰Tu Ñï‰,$‰õ‹$÷Õ‰ş£„u ‰ï÷Ö1í‰ˆu !Ï!Ş	ş‹5Xu EÍEİ‰È¯ó÷%Xu ò`u du ‰Æ‰×£`u ‰È¯Œu ‰du ÷%Œu ¶\u Ú”u ˜u 1Û‰D$‰T$£”u ¸ Êš;‰˜u 1Ò¥ÂÓàöÁ EĞEÃ9Æ‰Á‰øĞrM¡¬u ‰Ó‹°u ƒÀƒÒ v )Î‰$ßƒÀ‰T$‰ıƒÒ 9Îİsç‹$‹T$‰5`u ‰=du £¬u ‰°u ‹u ¸ Êš;1Ò1Û‹t$‹|$¥ÂÓàöÁ EĞEÃ‰û9ÆÓrV‹ğu ‹ôu ƒÁƒÓ ´&    v )Æ‰$×ƒÁ‰\$‰ıƒÓ 9ÆÕsç‰ú‹l$‹<$‰5”u ‰˜u ‰=ğu ‰-ôu ƒÄ[^_]Ã´&    ´&    èüÿÿÿUWVS‰Ã‹@xèüÿÿÿ‰Æ„Àt‹SH…Òt‰Øèüÿÿÿ…Àuu¸    èüÿÿÿƒ@u ‰Åèİıÿÿ‰ğ1ÿ„ÀuEº   ¸Du èVüÿÿ¸    ‰êƒ@u èüÿÿÿ…ÿt‹WL…Òt‰øèüÿÿÿ‹Gxèüÿÿÿ[1À^_]Ãf‰Ø‹=Du è“÷ÿÿë¬‹Cx1öèüÿÿÿé|ÿÿÿèüÿÿÿU‰ÅWVSƒìL‹=    …ÿ…4  ¡@u ‰$¨…  ¡Du ‹èüÿÿÿ‰Æ¡Du ‰×‰|$‹@<‰t$‰E¶äu ˆE ¡àu ‰E‹Äu ‹lu ¡hu ‰L$‹ u ‰T$‹œu ‰D$‹Èu ‰ğ‰L$(+Pu ‹Hu ‰T$$‰ú‰ÇTu ‰\$ !Ï‹Lu ‰ş‰×!ß¬ÙÑë‰Ê¹    ÷Ó÷Ò‰Ø!ò»    !ø	Ğ‰Ø‹`u DÆDÏ‹=Xu ‰D$@‰L$D‹du ‰|$0‰T$4‹=\u ‹D$‹T$‰L$8‰|$<‹=|u +„u ˆu !Ç¡|u ‹5˜u ‰ù‹=€u !×‹€u ¬ĞÑê‰T$‹T$÷Ğ!È‰t$÷Ò!ú	Â‹Œu EËEû‹”u ‰\$‹u ‰\$,¡@u 9$…ƒşÿÿ‹t$0‹D$‰$‰û‹T$‹|$@‰L$H‹L$D‰E ‰ğ‰U÷ç‹|$¯Î‹t$Ê¶L$<D$4T$8t$|$ ­ĞÓêöÁ t‰Ğ1ÒÆ‰Ø‰u‹4$×‰}¯Æ‰Á‰ğ÷d$HÊ¶L$,D$T$1Û­ĞÓêöÁ EÂEÓD$$T$(‰E‰UƒÄL[^_]ÃóéÕıÿÿ´&    féÅıÿÿ´&    fèüÿÿÿUWVSì”   ‰D$(‹„$¨   ‰T$,‰L$4‰D$d¡    ‰„$   1ÀÇD$l    ¡@u ‰D$¨…ª  ‹L$,‹D$”$„   ‹t$(èüÿÿÿ‰D$$…À…^  ‹„$Œ   9Du …ë  ‹”$„   ‹Œ$ˆ   ¡Du ‰$‰L$‹èüÿÿÿ‹L$‹5Pu ‰Å‰Ğ‹$‹=Tu ‰Ë‰l$9ê‰ıÃ’Ã9Öl$’Á Ëˆ\$…W  ‹$‹\$‹l$‰Ú9éÂ‰êCÑCÃ9òør ¡àu ‰4$‰|$‰D$l¶äu ÆD$ˆD$s‹Äu ‹Èu ¡hu ‹lu ‰L$@‹œu ‰\$D‹ u ‰L$H‹Œ$„   ‰\$L‹œ$ˆ   )ñ‰D$8¡Hu ‰T$<‹Lu û‰Î!Æ¿    ‹-du !Ó¬Ğ‰ùÑê÷Ğ‰l$`÷Ò!ğ!Ú	Â‰ø‹\u DÆEß‹=Xu ‹´$„   +5„u ‰T$d‰D$h¡|u ‰õ‰|$X‹=`u ‹€u ‰|$\‹¼$ˆ   =ˆu !Å¬Ğ‰l$‰ı‹|$÷Ğ!ÕÑê‹5”u ÷Ò!ø!ê‰t$P	Â‰È‹Œu DÇ‹=˜u Eé‰|$T‹=u ‰D$¡@u 9D$…ôıÿÿ‹L$h‹t$8‰|$(‹|$X‰T$‰Ú¯×‰ø‹|$<‰Ó÷á¶L$dÚD$\T$`t$@|$D­Ğ1ÛÓêöÁ ¶L$(EÂEÓ‹\$Æ‹D$×‰s¯è‰{‹|$L÷d$êD$PT$T1ö­ĞÓêöÁ EÂEÖ‹t$HÆ×€|$ ‰t$‰|$‰s‰{„à  ‹t$4…ö„-  ‹>‹n‹„$„   ‹”$ˆ   ‰î‰|$9Ç‰l$Öƒİ  ‹<$‹l$‰Ö9øîƒÊ  ‹t$4¶\$s8^ …á  ‹<$‹l$‰ù‰ë)ÁÓ+|$l$‰L$‰ù‹~‰\$ ‰l$‰Ë\$‰|$(‰$„M  ‹L$‹\$ ‰Ï	ß„;  ‹|$‹4$¬şÑï‰t$,9Î‰şŞ‰|$0s+D$T$‰Á‰Ó‹D$4‹|$‰Ú‹t$+pxD$t‰t$t‰|$xP‰Èÿt$ÿt$‰L$‰\$è
ìÿÿ‰D$0ƒÄ…À…Ë   ‹\$l9\$(‹L$‹\$„e  ‹5Xu ‹D$t÷æ‰$‰Á‹D$x÷æ‰Æ‹$‰×1Ò‹-Œu ğú1ÿ‰Æ…Òt‰Ğ1Ò÷õ‰Ç‰ğ1Û÷õ‰Ö‰ú‰ò‰Ç‰È‰Á…Ò…ñ   ‰È‰¼$€   ÷õ‰Ú‰û‰D$|‰Á‹t$‹|$,‹l$0‹D$t9÷‹T$x‰ît$ ƒ¢   ‹t$4‹|$FV‰W‰GN^‰O‰_´&    v ‹„$   d+    …ñ   ‹D$$Ä”   [^_]Ãt& óé>ûÿÿ‹<$‹l$‰Ö9ø‰û‰é‹|$î‹l$CØCÊ9ûé‚şÿÿÇD$$êÿÿÿët& ÆD$ éŞûÿÿ¶    ‹t$)FV)N^ésÿÿÿv ‰Ğ1Ò÷õ‰Ãéÿÿÿv ÇD$$íÿÿÿéSÿÿÿv ‹D$‹t$4‹P‹@+FV‰D$|D$|‰”$€   ‰ÚP‰Èÿt$ÿt$èKêÿÿƒÄ…Àu‹L$|‹œ$€   é¸şÿÿ‰D$$éûşÿÿèüÿÿÿ¶    èüÿÿÿUWV¾êÿÿÿS‰Ãƒì,‹d¡    ‰D$(1À‹C…Àˆ  {ÿÉš;‡  ¹t]í9Ñº   ÂŒ{  ¸    èüÿÿÿƒ@u ‰$èÊôÿÿ‹‹S+¬u °u ‹\u ‰Ç‰Õ¡`u ‹du ­ĞÓêöÁ t‰Ğ‹K)Á‰È™R‰úQ‰éD$$èüÿÿÿ‹D$,‹|$$‹¼u ‹l$(‰D$¡¸u ƒÄ‰Ñ9øéŒ:  9Ç‰éÑŒ•   ‹Àu +L$…Éƒ   )ø‰L$êÁù‰L$‰Ñ‰Âÿt$ÿt$D$$èüÿÿÿ‹D$$‹T$(‹L$,‰D$‰T$‰L$ QRP¸Du èpêÿÿ‹‹S‹\u ‰°u £¬u ‹C1Û™¥ÂÓàöÁ EĞEÃƒÄ1ö£`u ‰du º   ¸Du è7òÿÿ‹$¸    ƒ@u èüÿÿÿ¸ª   èüÿÿÿ…öt%‹D$(d+    ujƒÄ,‰ğ[^_]Ã¶    ¾êÿÿÿëÜ‰è	øu‹T$…ÒtÍd¡    ‹€è  …Àt½‹ …Àu·‰ø‰|$‹|$‰l$ ‹T$ ‰ù‰|$$èüÿÿÿë˜‹Àu +L$éÕşÿÿèüÿÿÿ´&    fèüÿÿÿU‰åWVSƒäøƒì‹@u öÃ…à   ¡œu ‹ u ‰D$¡xu ‰T$‹èüÿÿÿ+„u ˆu ‰Æ‰×¡|u ‹€u !ğ!ú‹5|u ‹=€u ¬şÑï‰|$‰÷‹L$÷×‰ş÷Ñ¿    !Æ!Ñ	ñ‹Œu ‹5”u EÇE×‹=˜u ‰L$‹u ‰t$‰|$‹=@u 9û…Nÿÿÿ‹t$‰Ó¯Ş÷æÚD$T$1Û­ĞÓêöÁ EÂEÓD$T$eô[^_]Ã´&    óé
ÿÿÿ´&    fèüÿÿÿU‰åWVSƒäøƒì‹    …Û…  ‹@u öÃ…â   ¡hu ‹lu ‰D$¡Du ‰T$‹èüÿÿÿ+Pu Tu ‰Æ‰×¡Hu ‹Lu !ğ!ú‹5Hu ‹=Lu ¬şÑï‰|$‰÷‹L$÷×‰ş÷Ñ¿    !Æ!Ñ	ñ‹Xu ‹5`u EÇE×‹=du ‰L$‹\u ‰t$‰|$‹=@u 9û…Nÿÿÿ‹t$‰Ó¯Ş÷æÚD$T$1Û­ĞÓêöÁ EÂEÓD$T$eô[^_]Ã´&    v óéÿÿÿ´&    féøşÿÿ´&    fèüÿÿÿU‰åWVSƒäøƒì ‹… ³  ‹    ‰D$…Û…  ‹@u öÃ…ÿ   ¡hu ‹lu ‹L$‰D$¡Du ‰T$‹‹I‰T$‹‰L$èüÿÿÿ+Pu Tu ‰×#=Lu ‹Hu ‰ú‹5Hu ‹=Lu !Á¬şÑï‰È‰|$‰÷‹L$÷×‰ş÷Ñ¿    !Æ!Ñ	ñ‹Xu ‹5`u EÇE×‹=du ‰$‹\u ‰t$‰|$‹=@u 9û…=ÿÿÿ‹4$‰Ó÷æ¯Ş‰×‰Æ‹T$‹D$ßt$|$D$T$­ş1ÛÓïöÁ E÷Eûğúeô[^_]Ã´&    óéëşÿÿéãşÿÿ´&    fèüÿÿÿUWVSƒì‰D$‹@u ‰Øƒà‰$…ü   ¡ğu ‹ôu ‹|$‰‰W¡xu ‹èüÿÿÿ‹=€u ‹5|u +„u ˆu ‰Á‰Õ‰ğ‰ú¬øÑê!ñ!ı‰Æ÷Ò¿    ‰Ğ÷Ö‹u !Î!è	ğ‰T$¡Œu EÏEï‹5”u ‹=˜u ‹@u 9Ó…`ÿÿÿ‰ê‹,$‰Ó¯Ø÷á¶L$Úğú1Û­ĞÓêöÁ ¹ÿÉš;EÂEÓ‰ë9ÁÓsL»ÿÉš;1Ét&  6eÄ‰ÏƒÒÿƒÅ9Ã×rí‰é1Û‹t$‰F^ƒÄ[^_]Ãt& óééşÿÿ´&    f1É1ÛëÑ´&    v èüÿÿÿUWVSƒì‹=    ‰D$…ÿ…2  ‹@u ‰Øƒà‰$…ş   ¡¬u ‹°u ‹|$‰‰W¡Du ‹èüÿÿÿ‹=Lu ‹5Hu +Pu Tu ‰Á‰Õ‰ğ‰ú¬øÑê!ñ!ı‰Æ÷Ò¿    ‰Ğ÷Ö‹\u !Î!è	ğ‰T$¡Xu EÏEï‹5`u ‹=du ‹@u 9Ó…`ÿÿÿ‰ê‹,$‰Ó¯Ø÷á¶L$Úğú1Û­ĞÓêöÁ ¹ÿÉš;EÂEÓ‰ë9ÁÓsN»ÿÉš;1É´&     6eÄ‰ÏƒÒÿƒÅ9Ã×rí‰é1Û‹t$‰F^ƒÄ[^_]Ãt& óéçşÿÿ´&    f1É1ÛëÑ´&    v éÇşÿÿ´&    fèüÿÿÿUWVSƒì‹=    ‰$…ÿ…ƒ  ‹@u ‰İƒå…b  ¡¬u ‹°u ‹<$‰‰W¡Du ‹èüÿÿÿ‹Hu ‹=Lu +Pu Tu !×!Á‹Lu ¡Hu ¬ĞÑê‰Æ‰Ğ÷Ö÷Ğ‰ò!ø‹5Àu !Ê	Ğ¸    ‹du EÈEø¡Xu ‰T$‹\u ‰D$¡`u ‰T$‹¼u ‰D$¡¸u ‰T$‰D$‹@u 9Ó…;ÿÿÿ‰ú‹<$‹D$‰L$‹\$‹L$_‰÷‰\$‰Ó¯Ø‰L$¶L$÷d$ÚD$T$1Û­ĞÓêöÁ EÂEÓÁÿÆ¸ÿÉš;×9ğ‰èøs,ºÿÉš;1À´&    Æ 6eÄ‰Áƒ×ÿƒÅ9òùrì1Òl$T$‹$‹D$‹T$‰‰S‰sƒÄ[^_]Ã´&    óé†şÿÿ´&    févşÿÿ´&    fèüÿÿÿUWVS‰Ãƒì8d¡    ‰D$41À{ÿÉš;‡	  ¸    èüÿÿÿƒ@u ‰Åèáêÿÿ‹\u ‹du ¡`u ‹3‹{5¬u =°u ­ĞÓêöÁ t‰ĞC‰ù™R‰òPD$0èüÿÿÿ‹D$0‹¼u ‹|$8‰D$‹D$4‰T$‹S‰D$¡¸u ‰D$Y^‹L$‹9$Ñ|%‰Ö;$t$¾êÿÿÿŒ  ‹Àu +K…Éú  ÿÿÉš;‡6  ¹t]í;L$¹   L$‚  ¬u °u ‰°u ‹\u ‰D$£¬u ‹C‰T$™‰Æ‰×Óæ¥Ç1ÀöÁ EşEğ¸ Êš;5`u =du 1Ò‰5`u ¥Â‰=du ÓàöÁ t‰Â1À‰Ñ9Æ‰Â‰ø‰L$D$‰T$r_‹D$‹T$‰l$‰\$‹L$ƒÀ‹\$ƒÒ )Î‰D$ßƒÀ‰T$‰ıƒÒ 9Îİsæ‹D$‹T$‰5`u ‹l$‹\$‰=du £¬u ‰°u ‹u ¸ Êš;1Ò‹5”u ‹=˜u ¥ÂÓàöÁ t‰Â1À‰T$9Æ‰ùL$‰D$rg‹ğu ‹ôu ‰l$‰\$‹\$ƒÂƒÑ ‰Ğ‰Ê‹L$)Î‰D$ßƒÀ‰T$‰ıƒÒ 9Îİsæ‹D$‹T$‰5”u ‹l$‹\$‰=˜u £ğu ‰ôu ‹$‹T$+‹Àu S+K1ö‰ËÁûSQ‰Ñ‰ÂD$0èüÿÿÿ‹D$0‹T$4‹L$8‰D$$‰T$(‰L$,QRP¸Du èÏŞÿÿƒÄt& º   ¸Du èÉæÿÿ¸    ‰êƒ@u èüÿÿÿ¸ª   èüÿÿÿ‹D$4d+    u&ƒÄ8‰ğ[^_]Ãt& ¾êÿÿÿë±´&    f¾êÿÿÿëÍèüÿÿÿt& èüÿÿÿUWVS‰ÃƒìTd¡    ‰D$P¸    èüÿÿÿ‰D$(¡    …À…®   ¡Lt ‹Pt ‹=Ht ‹5Dt ‰$¡@t ‰T$‹èüÿÿÿ+$T$ƒó‰Á‰Ğƒã!ø!ñ¬şÑï‰D$‰ò‹l$‰ø‰L$‹|$÷Ò÷Ğ!ú!è	Ğu8;=v ‰èv ‚i  ‹l$…í„J  ¸ÿÿÿÿ½İDØƒÃ!ëd´&    ¡v v t/„Ût+1Û‹T$(¸    èüÿÿÿ‹D$Pd+    …	  ƒÄT‰Ø[^_]ÃÇD$    1ÀÇD$    ºÿÿÿÿ½ØDÚƒÃ‹u ‹ u …Ò…  ºÿÿÿÿ½ÁDÂƒÀ)Ã¸    HØèüÿÿÿ‰Á‰Ğ…Ò…™  ¸ÿÿÿÿ½if
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
	priÎÈûmo(:é“ŒÏMœ¿I•û»¯r¯@ãX[íqNíZ˜ù:š™ıB¹Â¡BîüÌ/KF^„ íøËÈ8^¸5]æ¿#êKµF`›Cy~!;+ÇêÇX¾¥,Â–Ô4•ù­ºßù6r;3¤/Uhƒ¿ %ı 0´40tÛ)µñ÷Aù>J-Æ=Á†DT»xz¶Aé4!d:®(*Ñ!1HÍ½¦h’¨äƒuVAˆ÷¥F„Všåµ7	°Z¿jÀM•æuıq(Géë—ÿ @šï÷ Í¡Ñ”Î!{ğ˜Q&!`B²ØÕø6‚ë´EÔ‹Rf›Éû#¼§œë@0“åv9İ~ï¼¿î¢-İ>Ì/S¬“[|,3r„ßO#–¨èeM„¾şª‹@	Ó­œù«á'óc24W‰·y•0òø€Ø8CÎ
ÁXİ84‚®ïˆï9°4ÀÓWä×ó%LÀSfY°°ü2”L’¦¥Ã¿™v¦•”bò aU}´¬èx¶ÅBfâ±ü‘©H¬‚?®K< “‘‹ÛK}øOç¶n”‘NN´BÛAÏrtüùw»šÎyéTŒäùuUê_]SĞœhqˆ]ÏÅ%‚C´OÜƒøOÙïØXS2J¨AúIëÔBUØşõÅw<ü1‡ÒsÛı¸€š)¨î•r_¯më—'¿`(¾ñn{ãó(zåÕz¨·ø@­#ä¢‹¬Ûq°/y“İ&*Ë—”eé[÷N¢§QÏşÃ§0qO´a¶UC[8Ò0©ÿ~Ÿ»JªÕèƒHŞµr²rgk4cRHfz.=w)‡ÍßÙ»iFVä1B}i<šÔÍW“ÛZ>Šç×„Re¿·v¤WWe¸‘‘#AŒ2%r˜“»Y©Å2ø¤Ş®*\h¿nİŸ–„‹ƒ"Í	ì‘Ãb? ¤Ç¡poé#zöÄ”Öòİc“?‰·ûB÷e–ÀÚì½µ	ÀûkŸ´"©ƒè~Bkbi0äR-T…<5z<.ã3?_|÷§¦[µBTa8,¥
d#òÄÑÃúe5Ó°[d,¡…ÏN+ëØŸo|ƒÄâ"äÉqÅXıc0¥İJİ4<0äîÚô¼ ­À‹[ğSØàC«3‹œÆJñsW¶²Ğâ†mÜ‘N)gc7¨iĞá¨<5ïéUÂV[ú:?ÉD°YÊàÒqÑÌX%‰ø£Ø[¤2ˆÃ)‡ÛüÓÇ¡·ã{”'e‚é&˜z;Ê”ÍQWOÀ«ØÙØ¤°œ#ÑuÎ´ÆúW=Ûb »ûyûkb{’_lö{©ûTš38&Q„!ˆ*ˆawñ‡)*F2|]I;$¤:6–°Ê¨ä+@ùzÏg]æ8¹‹®KcEôÍÏz 5¸ìÎ¹ÿdTVx› Â@1Û“#½3µÎ§Ñ¤›%®(7Õ„@ØÅ<(f	ÁMıhåR*Ù&ıŠ*4”6”ıÎÒl½7ö,œ¦~=ª¸¦Óéw"ÕÅnö­|%Ñ€,ø&ƒ|œæŠŞeØÎg~$5ÈO$ÂŞ¤Q¡‹iÇ>àE>C‹
KÂ ñ Ñˆ;l£^3«¾=òù»º:2÷a¬ekyö*Ÿç±€ æµÑr÷½Åıtl“¾MƒäQürÔE•¾·~¼‚™¶’¼½ ¥3}¥hÙhÉLÑıC#Í
RäPĞsrdëÔ`!e.,*(:Ş §ˆdV(,òœ ùdgìÃœ‚á¸:î>…®F^Eê„ŸA+!Ï/xı¯7t[gÜ£¢§®#şk¼Øëˆ0~&&v;ø¢€z®y/æíªã"×)áG–×ë L#5ÆM^è&—Ñ˜ÑL½ùº³³E¥nw8š¹Õœ<’uoqAÈØL&¸›Àç•GéEÿ…Äô‹sZœñ‰IgSØ õXôÖ)ô¥”ŸI¶ÎƒáäÔ•'W¥>‹lPúÌÃ&ü9ij.WDô R»œ*¯áÎ´ğªÔ]XÏ1åş½T-ÀãÍD‚q<Å|STÀŞòŠ;Èê2R¸1”;.pa-Å.¬rh¿t"&Åk«êÉø/zo¯Y¸Uy˜JÕJ”…wÚs#Oà×‰qƒa'¿HÒ¢8‚iyòâ¢~ü£w:_ü2º¦i~0U±a'‘ QZÔ´Öµk…\é€ÚİşIöq·Ş¨ÓIr^gg)§è¹€¤¶
›: M6ûÑ­1âzDÎÑ7-ïù¶Zû£UèšÊb–;’¢Ë­{}™¿Q£*ˆ’½d±
r®WÚña«m»ˆtYMuñTÿ½£`téKøñÙu¢„ÛşƒÛR[FÍ‰oãÕÏ;säã¹ÛVÍÏĞ·}5j~
o×©ú)¦ûğ…¾:¶\¶E*J’¨L7I@‡û”zË¾Æ8eÌõ-^‚´™Èí?øJIöşD@ªE¼·_¯TwUİİsÓÈË©Âì´¯ƒ«,è©©#ªÏm®DnªÌ)PbÛ Mjğ8bÙ°Á
©
áq¤[†.½wX]£¿n‡2+·úÁ[šÂ¶'ôCÚkR¶çİÏ ;å´¯RbÃ.	¦«¤•)uÏ¶Š‹P^‘¢0‚xGÁğróìÈøm€æø.=h"á¬®|”[ËD¬=sı©\š\U4l¡;ñû?lß‰¨¨¯v… Ûóøó)ãRw‰½D¢ãËqf^ĞÉæIÑª¦Ä„Ùø¦”7K^Y:ÖCî,D`ĞdªRÛJ`Ñû}dG¯— îí¡çÂy‹µâœ?›ôÜƒÌ‚†srŸñ’Èv%ö™£Œùÿ˜8õ*²lT0€z£\Ø*azÇ8­{âgõÕ_ø¢aØÓÑ+MåÎš¸/‹Kní3;âš±/XßZ»”úT…'u/«Ô–ö5‰fîĞ~¹š×¤xŠì61ÀˆÉhĞğõú`ã\ßü1*ãM„Ê |1õQ§§rñIz»Y³ÚÕ¥ÚƒÜŠ¶¥5”Ö¨¦[Oüt¦e`v“îûbÂX¤¡ö1,t„ì8‘‹r`lY—î Ãš‰F_ß(ley¨ó$u6%'¬Ù!Bávça£S`¤àıíZÈÊ68Î›ÁÎòk·âØà¶–g¡şèTSVÊ¢.£$5³äšìš…á	¾T¯ÿ…(ÍS	yrhp“†§$Hç´ã;(¤,ºôÄi$[„ŒšNH¹\ç xÔlˆše¨3¢æê‹âÚ.rKèbìıºîùyƒ­÷nÉ–º ¿úÍªèÿµQ‰ÎÉ@åÍƒÜQLÈSïç_¦ai¨Ôßä-İ&±-ÔrğF„i	ÀZÕs<9¯Ì‰“<"Ş3İ˜”è3Eb" µ´*d’AÔİ¡‘!ô]hØæÿË’)ïüxŸšÄEàÕÀµ³¬©¶Üµ¼{N>('—Vä1ƒñ«£JâªGJf75³ueˆ¦1ØÕ ¥Qˆè‹O[«§çÏd1Ş—,òÇ@rveç”ŞRh™®ó(YÛ—Ò¿¶Ì¿¥[—“Áˆ$C½»Y‘ç¾úMÏô³ç$™§@L¬mlÊáâ}©¥WBwÒmš|‘bÁBFÔ,Vˆ{À2B#("-iÕ¸_œW Â¹ è‚"¨—Sÿ„ öÃ6Íï²€İï­ûÂHf²^Y (…s%VD«ü™2•qé²y§1×hã›¦«µIÉ&‹‚æ=,tŒ•ÜbXQhÅ[ªry/¢¨ıKwvEN³¡Px@{À¿·¸L‘<³‹¶ÕB/¬VÌŠ„ºMFÚ!CïŸ.{”~£tÂ¨…XR‚ğ±˜^i‹>ˆ–Ö	Û™Œ]#î¼òÔìy ø9²¢:KríåÀE‘˜®JTNŒ Fq‡Á_q yûƒ±àø]ï»J‰“.ã²~¾@,ÕfŠ96å6~D…ugàZôÈº‚Àyb/eÍáÚ$‡Í?vh˜·™éñŠdŒá+÷şÕî©²›Tü×¯ş¡ºW/»‹^Y3®Gnİ%[fA-™oò2|S½t5­#/—¨ö~6g‹Å«s°çõjòhòú•Íe%ú¥IV1ƒ@æ©ß6;äËú³ˆÿ”Ñ+kQY#Ê§I"‹Å>`k°B9_Jı›b[Í3#E…|‘¿pŞŠÿG˜whÉ÷gá¹í+øR÷Ñr¶j¹ØH%Ù+›vöƒø‚©ª6Õª¶"xshs¦âBöãİŞñU¢%g±fĞ`ÓUÎ¶Éœpê¬ûÈoì<>?¾ŒC¯ÿ…ÿ†&Ïé"%–l¤$bxm_!¸1éAÔq&ûêÇíÃ‘sTâĞ#µõqN]Û‹Ğg„ñ74BühııQ¾”?™ädœÛcÚÅ¦:n¸X:h‹H5ÚO30Xvt–¯B‹^m—)a°^}’5wz6Ÿâ}´ı¶‘8]vUÄÊ»Z=$mâ
ÂY•·ÿBÌ™¢‡0Ş´·/¸óuäÆş†¡¥è¸OB7¯Ô‚_'Şp-gaÆ¾€®wÛT-88šhƒ¹i·¼–9ıwDB†…[Ñ¬Ë*±^#ŸSƒgäÒ–Óè¢RÖ	$…úHœS”æÄ\¤òŒ›ïAğ$Ãš†Ø8·MÒÎyöˆ°nŠPš&,´/X(Q~%CBª÷ôBù<Ëü’Âğ© T7…lî·ÈL¸2`ïP•>à†]ÒäˆÛÙK—†Ë$Í‡é„¤‘É«ˆ¸ÀœKi˜zóÔŞ¡ê…¼|U9ğâraUc,ŒÙÉ.\Bã&½S®%¤"Y¦»X	ÉõíêI ìdV‚8ë]òab¢~p>îİiä?!ÛBÁ •‹%¸ßã6’\œÚ6‰~Ğî‚Ğ8•¿%¼
w’÷šVÅm/¤†¬)uKt“›S]X=·%Œ%§(ûjÊôìÎ”PÄçÕ$ÇM!	IEHüi«ŸéD¥¶Ô,
¿XÀÊ6^şÅ"+>œØ&"ıÛª* ]ì#)zîÚ­±ùl*²ŠMš!#D§ao0ó¡!·Â¯òhñÃdß<•{>‰ç^Mé¬OfV¤s4óÍnm±8“÷:ŒÏ)äS.O`2QŞ“6é‚àBuš¬Œœ6„q„ µo@ÇgMêô†q!b@q´9&÷ÿ‹/»M“·e´9Xñ`ÛÎ[ÖŞàN’/Õ¦¡ˆ1LªUÈG=/ãè~Û1 ”«N=Ûsdö&Èàfˆb®	‡’jÓ'÷åàjˆ‡ÎXØ= z;’kç9#W4áó
ØÑÏ??ş8	a¡`ø,ë‚~íL¹Á™z&˜Uàf¹“¶tLÈŠõÛVä•²>N£.|»
á½¥,è-ûÒYñà—úwÕÚ¸w†âÌ±AÓvÕEıf‹Ú½Àçwî¤ó˜ë+‰¹¬ÕÂu<.Ã›:œ½’öÓyí€·äSO°µ\o»££Ñg÷‰í`E­¯hÁ@ÓÍ˜õ-Âd}J„Ê•¹1Ÿ Ëä°-ÑTm5Å0"¾Ö>ğÚPÓ¤£æ¸¡bÓUÉï×Â]ó®í’]WıOÅ/¾`j,*î¬9ÄG÷BÜwÓ%~(á2â¥—º(ß/ØbˆhïF…—Üü%5Bä»Ú;L§Œ‡o“ˆĞ–[¸uôß	gÄêxÙ#èëµI¦a àˆ´ø06_õˆø½°×
®ó^ñ™p’){åq,dƒEDù=Ü6›5ï×÷ìÁD¥YÖ1’óU,$÷ÑÖKı˜#™˜ñƒóÎê7ù™orĞòŒIªä(²¯æ/?ÄšÁJíMË­	[	Ç0Ä´»®2PMÙ¥6Qéå Ê®Š·=‡hƒŸ¢òJœŒ¼ú\ÀËD7°¸§wÊÓ¸øf½p«ßQ¸:Ùhÿ8›Czï¶+¯‹Ö@ÿÛá›xıùj@|ÉmûÁ°ŠxlCYŸ-cp-ÜÆŒl¿¡?=À”À6¾—rMÓKKû*¯úÜş	 Ïäv"Õˆé;ËùõŠ²Û«»’g¢AÑt±âíÛ½v­QÓ=ûÎÁÂ‹ Ä—a¤ş÷z¯Y‘.dG{e>IcrO¬%Ü4ô2’àí5¸sı%hxŸ«nƒ!ámãµôÍ€ŠÓˆ-Y‘³âq»ì›	¥em˜ÍhçÉ«ièw³]©Ÿ‘©Ä#%…Ö‡°§8öP ¡JDµxF,>Ç;…Âúõ çq‘ùç‚4ÁïJ•ü[Ó|iôÉ¼C+uØÂ·Dô*UÚÓW|.<­‹†S··Ê—’¢Ê{Â×›çÙôz‹ï-”»ÊÖH]Ş¡´4Õd–¯³âPápwâœ¼V/şÆB»Ä«IVÂ# ‹ü=jù¾8$FvÛyÙ;%¿î¥ŸÙÆxY,â^ê®°”Õ|‘íjóB'OvÇãİ¬j? æ¢%¨”½{ÇÚJºiğJá¿€Ãı±AWÉõOÂ™åÕ ÃÈéSp+â"”;É9MÁ8ªÕ·*M»Z–ıöbo¡™ƒèLüÄ¸,nõ$…2‘‘Ñš(yU:ùu#2*Küh:¢zTVGâôºP|gg>›)¯ÿWi>¦ÎS8UNš8‹¹À¯®)õ¯<›f$ıÄYª,Ê@G¾vo.ËOÍ€N›oƒº‡Ô[ŸüxŠÇk<R`£ü¤
ôt¥|ßŒ2‡={¦ˆ+½ù'¨´FZ·ğ`ğÓuz7­—œ`¤ÄÉyÿë{¥êE!4dÍı·l™ò‰ Dbû.*Cø£ªŸŠ'{vëÒtÇËÏ:É%KëmÎ,Dæ/v >¸²bç§Ã î¢V‚¼~­¾ßá²¯¡>·e„ívŞwÄ.qú]àÌ¢¢!-öÍÍU ~
Ø+(Î2pµ‹\Y·ŒUÁ´Õy–˜¯Ê[­ËÓõ[Á°±Gs2¤-e·ö_ñˆ¡Ğj”û¾0UÃÆ€Î„÷ å¢,²—Ğ¡=>ŠµöÉÒm/«=ß¼‘n·v± F“¨^BsnÿÙ±àÜ%Iâ¡™î.vf*P1Ú¬á±LOÒé©õëa¹ƒAµ>Ì¤CAO!’D‹[îÎòÓ+ÅÌC&¾<7•@ÏZÄÖôKğ7¿»2„á¿øBmSDÜCtx?,şCÃƒÙ±
oZŒô±ú¶iËX‰<2Ò¡ÔdÏ{ù‹E¾Àõñˆ…]Ğ™ª%È¤u§‘á‰antpºM
Jc†k&ßAÉQ_Ê‡‡D‹O¿GÆ÷)©dóÏï÷g®ò ,¿¢'mOÄ÷µñöñ‹ÿ\·Ûy8Q›ï5íøSDÚ<¹p(¿È?Öİ—ÏFª{ë~‰ä{H/FL O¿Y‰¼<‚ñª†u{¬Î^«u…‰®s©ğ®ô4wª÷fE_·f›ÇÄË»VCè/r–_…›W‰¾H?Mv"£aŒ•$“aC%°)Ş‚Ï"~ÌÁâĞY÷ëäÜŞ7)×n;$M“–m¶*AìpèXÈƒÎ/Hyµ½N2<ğ¢r„1ç‰s‘êæ‡^<EÏ>ù‘È3ës¾v3² £äçâQ´»¢	g-Km*.åúLhíIif#Ê :™äïÓcÜŒ!ÓêŞİ¥ÉS¡‡õïM›lg‰eãEÓ¤ï®Ñ+Ô-Û=AüiW
ˆŒàiIÌÀ¬„&³¨*#kQmÍ`?[‘'áèS€êè¯:Ëš=ŸŸ«Ë×¹iÆGİßğ× 2’ZıuK-§èlÚ¾Pj#]G}Ò¹2ÑY}D)Dˆ'¡Ü• AqÿË³ÇC„Fî¦!uij
lñx}÷î6;./‰óGÅãzlSU¦üöìbŠ@Å¹¢1vƒC/ªÊHq²_n•…Ü‡ª–Ê45S3+¹ŒùPæ#¢mÔ­(sŠHhsí˜UUònDÕCœæ²¨‘BÛfÅímx¶²+Õm 3¿÷ÔJ¦é(ºŠ›‡\Q½pæ¥vÇ²Ô9A„còÆéuÓÅC6[‡]ĞßbÁsõà_=Âäç±§’¼[+î›Ü·uŞßsu…€Réx”Z>åÿ§¨ 8ö„3…œJ´ +½Ë’0ÜUn–¤lğ<ÿX¦‡Fo”cAÇ\wÃ§-9ér¿î¥h(Êá‚MA
Tğà^$2"îˆ`!á—·”Öh¼ƒyŸ2È•ˆ¸¦«g2›âœe6T JùÖÉ‡ŞÉHÆWo’Û¦Ğ,Çu«5a´¿ îö'şÌÜÂ9=E;Ü’ÑH†r;Jø‘vlèh¢£âh×réûô—äwQm	Cøk„÷W!­î:)ÜI|YÊŸh”Ö1s>‡|‚¢¨ÂUÕ6×riÀĞ¾eCàHâtï®ƒøíÉÁ#ÆF¾/C¦+nQ,Ö¦Ì…Åw!Òu¥ØW!µ­,Iè|*|42Ê»[QñÄ!°°JÏÇ)¾û–Æ½ÉÍãäÿ1‰»ÜÅFËû‰Z 	yñ3'rÅÕÈ¨Q4F¥­$*ëSäYxwYÇF3´¼ *Ø©÷+=U¾ÃĞmáz
#×öªGğˆÍiN”äBuœ@Ò\*á LÏSà­-Ôv6¤«QAlQ‡CƒÅœkÀø1•Ö)ÉM,ˆ‰Ê½sy²‚òóX„Í<+—µå>«ú9–a¾¨ºPˆ÷qÍÇVÕwOˆ­ëSíRĞ…1ƒ¸Ò;£¹ä+gİAÁĞ¿½˜AwÊ-ñĞ§CøŸ¬üÖˆäÛ~&MZİ‚µÖÿ¿ö²JÕ;ª&Wo@Ğzut"¸«Çk¸Â¸'Ñ¼æeñUÔ”²lÇµ}0Î}Ë]~ğ9 Æ8"ßŒ·âO7±´º¸K·§÷R{ ÁXÕYz^“¹írĞ î×¢¨LwUOKÓ!`Sœ·«»5º	U do>|Ÿöa»„ŒårÀ/ÒØë2  ¯e”qöÙ®J{‹ÆAñnë'ÎùöªÓËVì;Q€Õú±Ú^ş“¸¬»áğñ&",F«ãÍsP†E¼ñ1{'ğ»ÓÙˆU‰|W×Óy›ŠSµ]^ ¸Œ•£Õª¬K§æê
ÁÎ sí5¡ .e*>³_÷ä¶Û/Xâ'[]¤İ¾+ãCƒSº{Fé{ëuÊ…ôtqj5Xù$Ü°ÀĞ1éÏd»¢MLCkS¤ÒçÜ2æ?9Ñ>á†’jıäa·Îê}#şk"¨úªœcº¨}ÏõºÀ•]CŠ ÏVRáİš™kzœHÙ€¸0fMùŒ¨J<-õ¡í¢$.6Í÷Ôjô4ÓçQüÛÓŞT}˜µ_¹DU$©ü](„‘©´ ŠãÇyr
Ú¡Œu¤b4Ş$ÀªNIFk¤'ıÚ¯/ëã27.-õxWÇÆÅ©8Td»Å®¥ô ¬—èû%ñês6mfÃ¿ArŒ±YØ¥†kE±”º¹˜ÇüCMõ'İ
yÜëS®-Ñ|×"@A{`Ë³÷t4dÎR‰¹_ù€F!Ÿ’Tr¯ÜäŒˆìhÉŞÈ.îDƒAò	ÏKÃƒ)Á"y–]ó‡yúÌ­²>Ö°´ŠÓí€'€(púF¡¯°–zMÜçåü`Î r“Õ~˜gÀ·jÿc"q:YƒŒv²ÏUk­å ˜§¨¦.°—Ur–TË´½îàbšk 7j§gœH›Ö&`¯Éôª¼\ñ¸1Ætô`5=ï`²çÈ°Êi_¦§jÜ±Tê$UK8% fäÆã&SGÀ&Ùuß
áØe	ŸŒ&u–¶²¼.5Á	Eâ{ØşÌjñxB¤ñv†Ü>‹Åg•ZM¨#Kı¸ÆyLY?ü@Éh-uÉ²&TéÖÑÆ>‡ÎÈ
ê¦3Ê!Œö <ÈIyiõ–ºNSQlúİÃiGƒ*>‰·?Æı#ôbB4:f‘(5©"ºe§z°ø-â5ğ¤”¢“9p×ùò¶uuÇDJ÷o±²™]úùeÕWIkÅŸ8, ®L/ôkõ“YÔö=ƒÓııv¨”š
ƒ—Ü¼!í4:HCüáb»¸©¼›ï¶%CrbzĞYmÏ<7œ O< cO×¶½“ÊRäKë;oŠSÉø+Æ.²IÉÍ'?÷Q¿¤}ÅTE62(=Ù|EÉEÀ*pQ… ã¢<›5^•DˆÛ*¯Öê.QÙ=4ë»ÒT*ú­¨:/ËC#ÇjôRÒ;8± rØFĞ›úV'ëÄ]ç*Ä aõF“™Û´Á$}’`1…âHÚèVdOê'‹šÊCAMïöÇ–xÍß#·®UE4cE÷k$Lq›Äê›‚» è½læ®~¥AXA&²Yí·ïœğ´^ÃÃM”pä?ºX&Ì+1ëÎqÊ-¢í«» –ˆğèÃF´ıà?z6]Èlú<SÓh‘`'gQ€{yåÙá\üZ³X~×'ö‚5 CãÅ´Î’»¾án8ò.´rÙ-¾/›RºIÏ|W!İ—AûÃN,[<šC$¼4rkfhkl¸Oy“áí^Éõ÷Of¸ĞÔ// SPDX-License-Identifier: GPL-2.0-or-later
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
#define I2C_NOSTOP  (1 << 4)

static inline int i2c_slave_did_ack(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x01;
}

static inline int i2c_is_busy(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x02 ? 1 : 0;
}

static int i2c_wait_done(struct i2c_adapter *i2c_adap)
{
	int count;

	for (count = 0; count < I2C_WAIT_RETRY; count++) {
		if (!i2c_is_busy(i2c_adap))
			break;
		udelay(I2C_WAIT_DELAY);
	}

	if (I2C_WAIT_RETRY == count)
		return 0;

	return 1;
}

static int i2c_sendbytes(struct i2c_adapter *i2c_adap,
			 const struct i2c_msg *msg, int joined_rlen)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	u32 wdata, addr, ctrl;
	int retval, cnt;

	if (joined_rlen)
		dprintk(1, "%s(msg->wlen=%d, nextmsg->rlen=%d)\n", __func__,
			msg->len, joined_rlen);
	else
		dprintk(1, "%s(msg->len=%d)\n", __func__, msg->len);

	/* Deal with i2c probe functions with zero payload */
	if (msg->len == 0) {
		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, bus->i2c_period | (1 << 2));
		if (!i2c_wait_done(i2c_adap))
			return -EIO;
		if (!i2c_slave_did_ack(i2c_adap))
			return -ENXIO;

		dprintk(1, "%s() returns 0\n", __func__);
		return 0;
	}


	/* dev, reg + first byte */
	addr = (msg->addr << 25) | msg->buf[0];
	wdata = msg->buf[0];
	ctrl = bus->i2c_period | (1 << 12) | (1 << 2);

	if (msg->len > 1)
		ctrl |= I2C_NOSTOP | I2C_EXTEND;
	else if (joined_rlen)
		ctrl |= I2C_NOSTOP;

	cx_write(bus->reg_addr, addr);
	cx_write(bus->reg_wdata, wdata);
	cx_write(bus->reg_ctrl, ctrl);

	if (!i2c_wait_done(i2c_adap))
		goto eio;
	if (i2c_debug) {
		printk(KERN_DEBUG " <W %02x %02x", msg->addr << 1, msg->buf[0]);
		if (!(ctrl & I2C_NOSTOP))
			pr_cont(" >\n");
	}

	for (cnt = 1; cnt < msg->len; cnt++) {
		/* following bytes */
		wdata = msg->buf[cnt];
		ctrl = bus->i2c_period | (1 << 12) | (1 << 2);

		if (cnt < msg->len - 1)
			ctrl |= I2C_NOSTOP | I2C_EXTEND;
		else if (joined_rlen)
			ctrl |= I2C_NOSTOP;

		cx_write(bus->reg_addr, addr);
		cx_write(bus->reg_wdata, wdata);
		cx_write(bus->reg_ctrl, ctrl);

		if (!i2c_wait_done(i2c_adap))
			goto eio;
		if (i2c_debug) {
			pr_cont(" %02x", msg->buf[cnt]);
			if (!(ctrl & I2C_NOSTOP))
				pr_cont(" >\n");
		}
	}
	return msg->len;

 eio:
	retval = -EIO;
	if (i2c_debug)
		pr_err(" ERR: %d\n", retval);
	return retval;
}

static int i2c_readbytes(struct i2c_adapter *i2c_adap,
			 const struct i2c_msg *msg, int joined)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	u32 ctrl, cnt;
	int retval;


	if (i2c_debug && !joined)
		dprintk(1, "%s(msg->len=%d)\n", __func__, msg->len);

	/* Deal with i2c probe functions with zero payload */
	if (msg->len == 0) {
		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, bus->i2c_period | (1 << 2) | 1);
		if (!i2c_wait_done(i2c_adap))
			return -EIO;
		if (!i2c_slave_did_ack(i2c_adap))
			return -ENXIO;


		dprintk(1, "%s() returns 0\n", __func__);
		return 0;
	}

	if (i2c_debug) {
		if (joined)
			dprintk(1, " R");
		else
			dprintk(1, " <R %02x", (msg->addr << 1) + 1);
	}

	for (cnt = 0; cnt < msg->len; cnt++) {

		ctrl = bus->i2c_period | (1 << 12) | (1 << 2) | 1;

		if (cnt < msg->len - 1)
			ctrl |= I2C_NOSTOP | I2C_EXTEND;

		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, ctrl);

		if (!i2c_wait_done(i2c_adap))
			goto eio;
		msg->buf[cnt] = cx_read(bus->reg_rdata) & 0xff;
		if (i2c_debug) {
			dprintk(1, " %02x", msg->buf[cnt]);
			if (!(ctrl & I2C_NOSTOP))
				dprintk(1, " >\n");
		}
	}
	return msg->len;

 eio:
	retval = -EIO;
	if (i2c_debug)
		pr_err(" ERR: %d\n", retval);
	return retval;
}

static int i2c_xfer(struct i2c_adapter *i2c_adap,
		    struct i2c_msg *msgs, int num)
{
	int i, retval = 0;

	dprintk(1, "%s(num = %d)\n", __func__, num);

	for (i = 0 ; i < num; i++) {
		dprintk(1, "%s(num = %d) addr = 0x%02x  len = 0x%x\n",
			__func__, num, msgs[i].addr, msgs[i].len);
		if (msgs[i].flags & I2C_M_RD) {
			/* read */
			retval = i2c_readbytes(i2c_adap, &msgs[i], 0);
		} else if (i + 1 < num && (msgs[i + 1].flags & I2C_M_RD) &&
			   msgs[i].addr == msgs[i + 1].addr) {
			/* write then read from same address */
			retval = i2c_sendbytes(i2c_adap, &msgs[i],
					       msgs[i + 1].len);
			if (retval < 0)
				goto err;
			i++;
			retval = i2c_readbytes(i2c_adap, &msgs[i], 1);
		} else {
			/* write */
			retval = i2c_sendbytes(i2c_adap, &msgs[i], 0);
		}
		if (retval < 0)
			goto err;
	}
	return num;

 err:
	return retval;
}

static u32 cx23885_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_SMBUS_EMUL | I2C_FUNC_I2C;
}

static const struct i2c_algorithm cx23885_i2c_algo_template = {
	.master_xfer	= i2c_xfer,
	.functionality	= cx23885_functionality,
};

/* ----------------------------------------------------------------------- */

static const struct i2c_adapter cx23885_i2c_adap_template = {
	.name              = "cx23885",
	.owner             = THIS_MODULE,
	.algo              = &cx23885_i2c_algo_template,
};

static const struct i2c_client cx23885_i2c_client_template = {
	.name	= "cx23885 internal",
};

static char *i2c_devs[128] = {
	[0x10 >> 1] = "tda10048",
	[0x12 >> 1] = "dib7000pc",
	[0x1c >> 1] = "lgdt3303",
	[0x80 >> 1] = "cs3308",
	[0x82 >> 1] = "cs3308",
	[0x86 >> 1] = "tda9887",
	[0x32 >> 1] = "cx24227",
	[0x88 >> 1] = "cx25837",
	[0x84 >> 1] = "tda8295",
	[0x98 >> 1] = "flatiron",
	[0xa0 >> 1] = "eeprom",
	[0xc0 >> 1] = "tuner/mt2131/tda8275",
	[0xc2 >> 1] = "tuner/mt2131/tda8275/xc5000/xc3028",
	[0xc8 >> 1] = "tuner/xc3028L",
};

static void do_i2c_scan(char *name, struct i2c_client *c)
{
	unsigned char buf;
	int i, rc;

	for (i = 0; i < 128; i++) {
		c->addr = i;
		rc = i2c_master_recv(c, &buf, 0);
		if (rc < 0)
			continue;
		pr_info("%s: i2c scan: found device @ 0x%04x  [%s]\n",
		       name, i, i2c_devs[i] ? i2c_devs[i] : "???");
	}
}

/* init + register i2c adapter */
int cx23885_i2c_register(struct cx23885_i2c *bus)
{
	struct cx23885_dev *dev = bus->dev;

	dprintk(1, "%s(bus = %d)\n", __func__, bus->nr);

	bus->i2c_adap = cx23885_i2c_adap_template;
	bus->i2c_client = cx23885_i2c_client_template;
	bus->i2c_adap.dev.parent = &dev->pci->dev;

	strscpy(bus->i2c_adap.name, bus->dev->name,
		sizeof(bus->i2c_adap.name));

	bus->i2c_adap.algo_data = bus;
	i2c_set_adapdata(&bus->i2c_adap, &dev->v4l2_dev);
	i2c_add_adapter(&bus->i2c_adap);

	bus->i2c_client.adapter = &bus->i2c_adap;

	if (0 == bus->i2c_rc) {
		dprintk(1, "%s: i2c bus %d registered\n", dev->name, bus->nr);
		if (i2c_scan) {
			pr_info("%s: scan bus %d:\n",
					dev->name, bus->nr);
			do_i2c_scan(dev->name, &bus->i2c_client);
		}
	} else
		pr_warn("%s: i2c bus %d register FAILED\n",
			dev->name, bus->nr);

	/* Instantiate the IR receiver device, if present */
	if (0 == bus->i2c_rc) {
		struct i2c_board_info info;
		static const unsigned short addr_list[] = {
			0x6b, I2C_CLIENT_END
		};

		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "ir_video", I2C_NAME_SIZE);
		/* Use quick read command for probe, some IR chips don't
		 * support writes */
		i2c_new_scanned_device(&bus->i2c_adap, &info, addr_list,
				       i2c_probe_func_quick_read);
	}

	return bus->i2c_rc;
}

int cx23885_i2c_unregister(struct cx23885_i2c *bus)
{
	i2c_del_adapter(&bus->i2c_adap);
	return 0;
}

void cx23885_av_clk(struct cx23885_dev *dev, int enable)
{
	/* write 0 to bus 2 addr 0x144 via i2x_xfer() */
	char buffer[3];
	struct i2c_msg msg;
	dprintk(1, "%s(enabled = %d)\n", __func__, enable);

	/* Register 0x144 */
	buffer[0] = 0x01;
	buffer[1] = 0x44;
	if (enable == 1)
		buffer[2] = 0x05;
	else
		buffer[2] = 0x00;

	msg.addr = 0x44;
	msg.flags = I2C_M_TEN;
	msg.len = 3;
	msg.buf = buffer;

	i2c_xfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             ³$¸¶í‹ÈÌæôqı¶dñS¿ô­÷Ÿ-~¼Ÿ‚§Ğ;Œù‡ÉÿDõ‡•ˆÆk4æHI;í²ÔvÛƒe§ =v\iqKd]Ïı6%ù&¿x&cTñP¬¾0SRXpÿŒ§îoÄOq(å±É
l•{ƒg¦Æ3ˆOnÊÁq€HAò44˜yõí\HÅ¼­i©®îDÌ4sà†WZÓ:«¾¯šsÌjìã«‹ó|¯¬~e×ç,ïq*LÓ{–G…oì+V§úu ìv:Q¦8İ";¥°*ÃNFS#!šMK¸°àm[ş½á")²&ºcÚpˆ®Ü2Sš¸ƒ˜h“õ£)q/²;K3lÓh®¥,ÌAe…`ä²¸OÁöháS)±Ç©60Zè>.ı®pØgu+ê=-Ö+føÓ™$ÑŞz…O|–
Ê^°–o¬VƒEò>ÂÅd@d†Bl˜TCÕKŞ™Ùµ'‰ùtÓÛb* ŒJuG|Î}Å r9¾Ô/˜Hoa–ö+òvâŠnÇä+š·|÷Ä‚á°|Í‹YlìÈPÎ%
Ÿ¿ÒÍPŒVÿù¾‹í÷T#òk#%,íi4iîçºÒ\İ%TË³ı"ÆÏuˆÏú3JrÇô8	Ú]íé=2ê,ö(DÀÆÁƒ¤²=RT>©Sb~” Z´Â+l3•_¾Z‘¸¬HöN&ıvœCxu'¶Gå>übW)íî¬,–o½ˆDTíï•{*=°¶´Â[Mï¹„;¤–&®ôPë,ĞÙ®”‘Î[:ò.N!éÍq°Xe
İÜFÏ×<0€K”Ûß	 å¤Š*“÷ËsÜÜ‹|m¼s&›rÖæ6Ö¢g……ã"HÒqè	ÂfÍŠ¶m-Ë÷BÌçµ¢X€VCä"iÁ*Åm¾ì×Ñ†á¸UßPN<nú²[kòYÔêñÔÜuÉ—¦ù^£\rµ"-ÅÇß‰ £[ïıej-ÌÀñ&/“·½s™5ƒW4‘I®bBíînÊİC Ì±tÙWk?“ƒô£8¹ìÇ…†2å:+×Ş8²Ì²k6Š]|
ç2 Îî¶îgtñ¹$«DêÌ×Z¾š  X‰Â>\ıQ!’ØN$pÓ …	)Ÿ¹Jõ˜ám¡Lú\_¥gV²§:~/ÿDÀYM«ùR÷»A¹J

 9½¯áƒ!U‰ëYø/<{†oò³µ¶}„ œE˜Í9´ãì­Ä ÛƒóG|˜)Æœ¸Xë@Ù3wælM]îGNŒÑMşÃAxÌCWİ0Q“-F¦iŠË¿dÿ‹‚hDîÏJ«Şj“\Y‹c<³ñyÌA°é¾+±şÅM1ô/ºwg°J|Kúë¯"×”ó&àF,¾‘Vg=…"§19^Ô:„,+^«ÚQƒ”–¦3ÑÙ•Œ—Cl<wæ`”6›ƒ)êûWı·‰;Û‹raÎù	İæãúßµ”Iµ³$çÕoUè ‚é}Ã×)ÓŠæ'9b‚VÓ×¦MóÃaª¦ëÜnpäctºxàİ£ç\Qå†Ä#Lå¯œÿ<’`¡ù"ùü¬½¬¯‚ebŒ|Âé­z©î¯­¸€”Çœ¶ñĞ
W—( p^(Lëî¨ÒXÒª¯0ŒÃ@˜ŠmäşÖDI@RzWAÀVr£x‘: !ÆVLù‡Ô#D©æo‰$“•E [ïNa¢óµØ†Å´TZäµX˜§»“iG-ëéí„CÊàÑ˜n’äE,Jx˜ÒûPwoN«sŸRÏ-[éàE¨ôÔ3bóÊ1TÙnÑàÁTÛüÉyMC²:Õîêh¥mJÄWd„,O¤<ºú]ÃP‡1µô"—ŠDqhÌÂÜıœ÷ïîâ“=¤Å+•£wIŞ=SÎ2Èú•/±MÕwVÉÒn˜mc1î›:±‰ úı _\‘T¿ÓC`şÛít¨İDôP3ºyDR Q5Ì9¤íb_F•™~¾@CªôA•EIã¨N™5˜ûpµ,Æ0E—×˜øÙ%¾Ì¯º¥ñ†äˆ÷ïh–gzëp­ìûÿÛ´-yåùôÃ”¸u•{Éâ/"Ïï]ƒO$ÊAOvËNXå¦U]ŸØµâ”kEÓÕøF•It€tãI6[cu7L)R÷§-ºA}g4që ,Äc5¤o´hfz)Ö±g`èúhåG‹Ö¨U€'V¼(±_úİlûùğ5 ¶z_×È^£˜ÇŒ]šOÿ²i±bI¶qi)ï0w(‡‹™]ÑÔ&ÓÁ·Ì¯`a»g´hÀ/Hÿ­<ln1Â;À¼é,Õ¡ê_vã¦;Â‘oFç†zu~5ú_N=jøyU¿[ÛÿN‰‘ğ{hÃoö»˜¤n˜aY&*¸h7ïşbgò0‚RrdŠÅ`>4
x,¥æÎ1Â³Ë×œ8w.˜şß¨9hK3c^¡¤Yj#Têƒiçu3K"ç²_>ÙmíçßûêÊ÷»>õ”.O z™Îù‹¤Ù ¬^ÎÔ›à1ùp@‡/Ælc°ë¾Ï¯LğÂâæ­b¥¤[Oê­öÉ_ıÃnîÇfíBG¢3˜U}yTXìt6é,|ÙWa6!åEG
LôèÈh0S„Âó„‘½†¢+p×Èlšx¬B1µjO­«oàDğV¿Á~x§ÿ’×N1° ñš#ùÿù×çÑó{˜İ§9à[‰Ê	šIš[·˜¨7
€wâÏ”×ºêÕ œ“^qí9{sîuñ¼M 4”ëİ3šè›7{‰­0‘¥Ê™z@WBˆ1åJè Hº`ç—¯¦š ²ƒ³¹s0®úüWsÆÉù¤Å¯¼=4‰&ØBÛ^ö&CÍ&nKœ‚Ò§l2*Gû+.ıìïˆmØ»–+z`Ä£.
j7m(uØíV±#€ÑtñÛßwƒ»¶¦ú99 ¥«òm'Â|ÿˆöf ‰ÜäŞt³Ç¯+îßğ°nÑ*À#-Ë,@?ÍDM£éa”…=aZbì]šŸ·„*Ü‚–r´ı±—Ñ³ëš§3hùaìNÓ2¨‘ˆ¹¤‹Xl%x„¾za·uß‹|/—óßxçpE¶©¿	4gÌ%JNåĞ[V"1å®¶<!eA™äFä^ª7¸v1ÜÕ°¶ˆĞˆj•(o¡÷Rá’&ãZ‹Ô­0µÔÀ{ë}®°ñ·…~ÙJX_mw­nÉ##VÑ7UÒIôOÔ‹Ğ:á$î²/†Ï”ÁF0ü>òÃ4ÕŞ°nj	‚Â˜c¥¸‰ 
‚dÀglíGUO9êŞHµ+‹€(ãÜ|Ğ‹ã€å&È28?XE¯lf«ğÕ„sèz*³¯÷mˆ¬/ÊõXÕ°…¢•·l"U"ÿs´Õ].ƒ]£¡Å‡IòqÒ­9F­[0’$u¨ B½MO`P*ØpËài€¯áÚß
©OM³å’¿phë5g°¸OŠ·t°A—Íèê7ó#g?;ikGˆs¤ôhT _ìxîµ:—‘×Ü¿I‡›´;Ê/£´b¾Ês$‘#öÌßw¬d[—SæŞ'>]+v¬ÒàNî{ªƒu•‡ıœ&,ÒpÏ•}§ÉÌÎŒRX]ıV>Qqsg`RÎÜµogIzR,€DİåI/9ˆX?ir8ı¯V óJå„Ì´
YÍ‡20ïkÉ0`¦ü| ê%l(8M<ˆ?7Ø§Ÿº1{èÖS=Wã ÑÌN'W¼çdCšôñ´Áv‹r/`5“2-Aæ,–4¬°¶5”õ˜âcxb=‡Ci„ï¨Ù!}£ÀË	,‹Ï½Å[zë©Y ï™;à³ÖƒJY%ÔÆ†³¾ˆ—‘ }ğ$#ª…ìC›ò.HM×Ú¼B *oJO»s)ëœ·—ùT>jmeÖÇ„b†49A^®£Ï7©ë#5LZww.)_)j'º»J:='EB³_üÚİ*BgT} ¨û[$™ß–(C} ¬RÇs€ãM+ŠI‚\ãê¤_è%œØ¢N_,t÷k¤»<ãˆw7®º—yLÏUšJ«ü¾›©Ò–Znd“ÀßÈ:¢OïdÇÌ±¥2°}ÔÁÂAkÙ±Ä_%MHKEQ®BÅâ%‰g7Şk­mIˆèè¦‰ü½§|D­Ÿöà,Üô~N72¯ä¬÷Z+ukÿ]»ØFŠzÖ&y4Œjçf]Áç|Ê'k>ãµR²M^´3‚Xş_yş(nˆÎû% ÔÏ›û.,1äškïÿMDOuâ>›~Hn.ëCÆ@’-–4óìôãû“ÅœŠÖî0cZf®œüˆz\SpkÇl¦€¢
‰J‘ÆßË­´}³ÙÍeGÆ¾„.8N
ı<È2™Ó \úĞCÜ@:Ë(ÍBzÎ²ÇZ&@¡nÄ*?Öâi´µ¦9-|ğêr6´wª“YÉ³7A)Ìğü:K}—U½íp”ÊGú$¶Ëw¬¨h™‚´ A[ GF–p^è=ÇüX–U<ã-2v»Cc¾	õp9PÀÆÒo”cJ´°OÒ…ÇCB=ú¤­	ç+8Y…ÑS¡&÷
ê İ-¹$aó!ÜÁ?Ò‹¼\ƒG	wl&×&h³ç“2oík—d¼¿/38â˜úıúÄ®¾Ó±¢çó=dŠZpêúúy|*†xÄ‡*ÖğRv^?;Êl!’ı€bFzÒAÑcbbkŠPÂ‡½ØáÕvÓËôù)ÂÌÁÇW‘°?Pü#‘÷¨PÇï¼xwÛHnì/8ĞåJB¶}ğõØô¤ä³ê[’^î?Ç¦pVk=j_ê©”%ô$İ¶Æ˜P7Õş“GB‰p…-÷üªŠ}9f*´v×+ ÄõèGFN¶eYQiÒòûYI8hÔQsJwî´›}’$eW%’Î½—;p§îæß"ŞÚ#ø~¹1#ÈhÀôU&ú¼wE‚àç™0€sƒ>w/\-‡€X“-	Hh¦
™)L‰L"Ø÷[…9b?añ­Â=¼}ûÜŞ$é†Ç0Ò×Ëi~oÌ°GùdTm¯ŸüÎe¨³@nù+§…ÊÉÍ&õ·‹ÿ£¢ï€Å6JO·e·ÀqeT>áHnÍ\8˜‚a‚§!Sô1Zô¯Ux»7¢Ñ'> ‡IO3±.rDï%Ü–JòïAèt±&…w3Y¨_¦&"#„y—Ç^ûËieÃ±ËAF³÷pâ}î™zÎê>çŞMÅIk&{X  DÓ1;ø!Ki,á5Sî†áÃ‘EÖFXŸB²é÷«—ä˜úU*'ğ‰§ú ¼iBk™ÌjmAVPË•gr¶Át@^‘£¦£·½ø~'¸,NØĞ#Ç%Ñë92Ñ•zK‰µ§@auiFĞÂ¸ÃMÒV§;ob°®åTÖğD\X>ñõæNì‰^ˆq…ô€p€ KgÇû·téô—3Nm$¿zî.ê¼'bõ&¿úÆĞ·Şè\I¤¿ágyr÷ÑÅ*)ˆ‹ª]óØÏ©7ÿòøU7Çbô/d>·”7Ál‘J×£ÊùÑNx”|J.¥IŠ´i8ıÓá	XÊ†]*µB›“Ñ¯'b3c¶±°¼8¸½ÇöN¿¬búól%=
§ 'şlŒçµ5ªæòbèAîDÖtöeé€.‹]ÀHbó¼èˆ²äØ©OƒsÂ½˜ÑÍHèœïIÌ©° ¡ñ¥–U3!EÇ¦Œõş*¯Ú PøÀ± wB¦=FüHñ‰‡?&_·<L§®Hññğ«oP³;.]}§	»j°ğ3ÆIXRÖ„Âùj0åß)Ö©ô¸ÉUôûj·õ*]ó,è_Íø›íÒ–ïõïì;&%RÖ&J’[QÃ!RÆğ'Õ±
×HôÒ4İœªûc	¦xàğã"ÅrèFFÚ_ìj&Cê—6ôì•Ó%¤óÌá µHäº´7fx²nş´4”ˆ\zí^JZvMM ZŒÊuë6İ•Búøèp!\Ï,F„é[­?ä_2)¸óa|
¯õ*\ÿzS’¥%É6"J„‚nÅDWËšQ~×Ù“)Z%«6_à_Ñ *Å‚*M@¸xğ} ^“ã¼‹ífCqg;ÒÏÛÿ:8ù¶ b¢¢pbäÈtP+/=fŠLÁ%tœT«£Û‡ÉãNQ'Ò>U¿T¾æËşf(49Šw £16²3™Ø4õ×pŸüD[ô»#PºØz`ò~®ÜQ–OªÄA7aÿÊHÿçxwAô"c+òÉHbvçÿC€?ÊVû9yî!>x©¹ÄL84ÉV”k°¾‡¤òS(ŠÏQB)oJôÉ(C\ìŒ„_†"–vYzní©˜â}XI~&Ú*o JL]á­†ş(c¼`+‚F[ëú.7V]t&'"#™qá7üªÊĞ`†ÒğÇµA¥XÄB ³AÏzK‹ØÎÂáŒ G}‡—·ÚÜºóy@–Ûbë=„,úÅC¹ï-nER(
Ån}YWA–µ’¸Aæb‚d’·v Ç¤vÈ~ÓÉ7Ë%%CØ(î6ÿLx­w²kÙo]p¬×)œˆé›¢6úŒ/>¯7°uL<‰™×Á–fĞEŠŠ‰VŒV¹‘K@ZÓö¦É«<Šfvµ{~ÆfD"Œ\@¡ôo? Š¡*=æÏæşø–õ?DQL£ˆY#XÏ+´@‹øwÎftÆ(ã™`¢÷€÷Ò_†W{İiÆËß„EaĞj»ø]9_>±­«-Bõ4¢ß<»Q'ZG<OR<di
êšë½®Ë>3Ö .ü‡ÈP›`øXÛï•üeáS"Ì<¶pwâĞÔpòó%ıV\š z¥dÓ¬ŸñØLóêØ˜9­0„Õ¢ÖŞ‹ÀÊ½`Å‹­VÁúúÑC[¥ÌL¿9KLš uÈ¤VôgXëNC7¸d&øâõOàâÊ:ÜCù˜he$İJql1ÈL‘úœÃ)/ÓÖ_^Ìyúî¬if­ÅÉ5F÷?¤gN.D3$¡Ùt˜6dÓ1#˜lÔÚNJÍ³­6FË´MU%æ¯NgË4FÜ"«×5¼Ú^³\Z~bc@'Úÿa7=|C'«ÛóoY:û[=eÓÇÔåL¬ƒ½ºğšñcá³o[v‘»áyËÇq¤Pov“Ç…iÜPz•­eë,î-İ§üÍ•İà–VŠå¶T˜ìË cÍrÍ~˜—
¯¥/çH×Cü¤¾_Ê«´¸º6ºi,±ò0‹®7ÎëŠºÉøæU¸áz–3tÈù²@y¹Çs¹İTh‘Î˜)và{­SnÚÔ:º "ù¼}³>ïc3 !M“R»†äÒ“ )_9§:gÉ²€€¸Ÿå¾0	ü¬n&­æÃo®¯ãM*±ÉxC§ÿğéAÕ}{F½ÌÑÂŠLÁ= Ñƒ8öx7‘+úuyèR¹;ÆUE4yoó	û	¾ÚOáz¼s˜²pöôóàTZ¤?X€k=h†4õş²°¡Í’òırW™Fİ}&.·Õs;È›¡×+nl‰Éç¯Úi;<â ü:WGrû­ş4²vSp9¼‹€#{×˜~8´[¶ 	*¢pÇ´·­µ¨Ò :‰ÚH,¼¶¢•ñWyŠ›ñ8¡0\ÎÍ‚]_c÷> İÍÇ¤iı¾¹‰y½×°õö'Ã¼@û®ç|õÁ¦Şï=B0ÄŠzË)hDå†g@-·uù“Äã_=Íğc¬˜®H,ü6Ï#_’;á|Ñ„4Q@m:_HXÖ™ĞŞĞa-_½ÿY=¥¥t¬xÛ2=Å-¢&Æ¶Õ>‡@kX›P™Ío®úoĞ×êÑ¬Z#?‰§÷[k8páVƒ¹mÿ6KC¦uÜ¶F¯ÖUHüN›£¨Uº½ûnÄ±-/^ä\7=3_í9ï½ñtˆŠ>ğ›·‰Š«449ï°#n"¥ir¬å{·¦Ï)ı´¤]w­üTlÒ$å?õlg™Ma¸üÂÖDÒd0UÕÈ>Èî„Ü”Ûzƒz$+fºŞUW6j™bĞ¯ 96KÂ]š^ëL–C4ı×¨•=¯ŞÑ62ÔÌqUÃ1;xíZæë‡·Ë‰³é,BıÜµÉU¥gºÁJ®6×¥]"›’%s–GrX˜7«`Ñ¡gmœ¾K¦àT“ZåJ9t°¥h¼ÕÓyñIÔC™ècyëZu³‚È-»˜¢á´€Œùe…éßÿ'"v’RÂÛA3X¢VT¦ñ.•Œ÷®à™şaò_éŸ‚Úâ78şÛø°TNò6†eˆ}Í#Fö>;Íbáıá)o!®Ë"z#ïCÔBÀåU}e,[æ¾#`EyÙô"ĞûOL~ ünj3mR§ì§äröc («¶BÃ,`jKƒúİûXı¶¹€å0öî’+¢X3Óç–ªÈ1†ıP¢NkÉªíTGTntNƒ^Ó"’3xÑP#Á§–û$ĞiÕéü'2ƒpÌñğ$
Å§f•ó®Ò A$Õo˜C8ìá"AF7}–å¡°n‰¯ó„¯/½GŞ'àdoıÃ“•Àx
ğOÒò
t	ÑĞ˜££´°¤×*wësÕŠ‹®‘È ÖuW¡†ìqZ¥tmÊŸxˆ"9×Æñğ2¸\ÍjÚã¨ö}Siw;9ò`¦µGoj I"gr¦…©™ °îÂfp,¯±’¯Å§`kFÔ/¾~UP’2½±¸â#ÙĞ… ¤²ÄÖ­‘VÉâù>èhSÒè×S•a¦\8ì¹6£6ìïZƒnßÍ9î¤c$n´İÕÃ®85™{Ï’@æ£f§eÀ@ Ã4ˆJ: ›)+±í’dÚ P‡±ÊJdú DDedŞ0¯nwÁï ¤º®Õ,”%‡Ã×›ÂCÛ%TiWÆôœÖ?¹ìğ^şÆ*İE6ÉXØöÉÉ]‚HÄ bÛÅ}d¤42lChKQš3Z&Áa_·Œàài-<œïßº¢Š)"¼P£[/‹»(q ÿŒ(†œ›¦Œí-—QMc¢¶>ójÊ©ÅÕHMR’J[à3(É’1
éš©Cò"o–\ûI)¿Y½gÈ.vAÈgk*ªR_“ƒÆÔğ´ÅıÜmÑ$ëÎHÜ"‰ë„ûæw3İHGûĞ±úÉÇkÈ~2Ad>œ—îjJ†wşmxlKßi‘	iqÿ]uÿ(¾S.ØA>øôE94ƒ›R7,Ü³zA""”ßD¶³x<çí‹QšNŞ¥$b4u¼u0Üëæx@ŞlÃpî¼;İh[>=‡´üb¯ü2bíF=’dˆl0"ûkÓXEu²$ë²0ŸÎ7}€À‚~S5±>GÛ ä=²ŸYjÚ¦3N?Æpè¶/}SÉ^ì¦+Teì¥ÇÆkD™àèó*( j×|§Àş^pí¶ 
p¸WI~dí›;IÜNÔœğIÉûª,Ã®ÓZĞ¾l’#³%l‡3]š@%Š¸$jİÜ§$™İì3yQL‘÷áŞ£æ{Kß,ÓŞŸÁÅ²·”	xÈÒ2,c§…]N]bÌ{z Ö3Ÿ­DûIDøüüXo¯†!UL	+ğ
eªØÏæ&pàr¿‡gP°Ÿ|yœEÌô³Ä¦‡Ñpx¿p}á±™<Kï—şò»ZÉ©©céÆÒöL+ú‚Ïó/(qİgH±Ò&®<5ö6¿sõñİÛuQ­ûŞ–‡Ë÷²?i“!˜hW®=ğ8Ìı{¡ÚïÒêv)VfÚ"šîwÁá,R ÂÄ—›p%Q/ú'0¢r®M¨íú¢j"ü™şÀeÌ-g‘A˜ó7Z$P¾ÚE;ÃÆÓ¿òõZ¥§ˆ>=E"}‡Äñ	+ŠW‚æ%lÏ‡ÑåšMÎ@ß!ÿÈwK%gDÂ2(“ú²b®°vXÁ`Òt1ıÏW*²ì¥¼Võâ^Êwç–ŠÃrO]÷²sá~‘„Ê¸EguQB´%©†”‚V°ƒ¢k·äx? %ÂÔy¤»„&hLÏ­~J¸{“/ÂÿÂı[’aè­¨/R@9ƒ¸5ç™Tp1%"Ú&Ê=;çÇŞ,¥| [{+:vİÙ‹{/XàA©y‡Ÿt|JÌ+cè8zpì’§	1-Òr×Ãß° EÁO"¼ÌnBT·à5¨´>õ-¬ş‰{C$…tGNyü]%eÏ´ÆÑ¢•trWán'î;u3S"ÒµG ZaıaØÖ>¶XXå:õD_By¦2HWãšEŸR¹İ	@*µÎ@•$2 Øü„Åe. ·L5àÄFøH6“ÊÃv EÊæˆˆ÷9ISeÃURM1 ¸Â{ÛpªÉ1$øjEÂm®ôvĞĞĞ(Şl¬L6`Å–JCf5ËíÍE¢™ÉßC^»_ˆ%şü”Ú\éU&ÌñMleàVi<ü2Ñü h¯v¡^)8‡x$/EtCì¹NT;šÌÕ÷ƒ^bö¦é˜]öÌó±ĞÉŒ6ÁPq-^<¨FQsñË/% ÎLîOÑÀÿµŸËçœ"X.v+OyÔçºoŒ"@[t^
Ïbe5İáˆÊ¡Õ7>?KCÉ¬ï~#âØC­İ'*¾Ë aI¼E€øİª
æFØëÔõº©kÇ–>TvÇÂäNàp˜ ùMøŠb§[Z/M¶Ñ¨ãºîÅ‰™ £±ò=…m¼Öşhù85ä?6aZ˜of­º¿ïŞŸ²µİ$ş¦¶
ƒO6f´ÏÀUu³6C0éà*oö£?\AÿÜ8=“¢ë®«KçbŞ·4²Éğã×/ÿ×Sñ”¿ÌS8?R›["Ûtd}rè-H‰ƒ^€È"Q¦UøÇ)²%ôïälRkkê ±qÀío¯£Õ¬‘Ú¨‰­ŒŠVÃ{¹¬wïyŠ½¿šğö'r,nZÊ¦K=-wğGQ}ˆEÃÃ^?ï³^_>¿aõõb&áZVÎÒ÷jÕÊ,õ§4#Š«Q[¼HØ}5÷µ;™3ï¶hÌ:¿1âæB@#´™$Á{ ´Â;*Mº)EÙÁ·2*ÎÒ‰Á‚4¼£¾¤[»ĞÂàèÃ.ôHŞ8?:D5<¨Îš§İA‚¢ËŸ¨Iœ¬‚Ğ‡
ÑûéÚÀT/­ø±İjïe4›V£Á¹¿V éX–¯)€¼+Jà+.„?Ê®Æùë-óoOÑIY¾
c	Õ_\ó~h‡5aØpPâ*èdrÒÖÍúø?/ò‹›eaŠ+";ÍÊwr¸q ™Å¡d.9ı4i|"†³w€sÄÒ6ÛÎüËUEbÆjº3x™ş`n3Ù~’<P¡:àQxì@é*ävPÕè6¾–:Ü³q÷GH¦ÿİ7ì8ªmcg«¨päàÒıVÏR†Zää!¸Ç<Î7Ùü0lyÑY^Ü8@Dú¾ê2k¤“İqhZŒêd»V
Î‚øğä»b½ŒÈÜaÂì@o’ê ÃiÓĞnìEÁŸ	¸ú_á¬Ÿş—ã×)Ü
¦™„q³V×ïÕÛ÷mŞ· 74‹à!b;˜ò‚5>ÔS}ÙZ1»òìêo¢]Á-‹-0ÕÅk»£Z·1ÜoK¨i(Üi™	gŠĞ˜•ëª ÉEñF*c ç ìk³2À^ÖúRŒàD‰Åq².8ëñ|ün#±Fò|7©4ÁÏ„|YUJ\fã4¾%$çêí$lç\ÚthÏ2¿,Å‚‹×CşùJ9uûj‡d–ŠeÂ´º¨âYzˆÀ$éû@œÇË ½o©M¡âsJÓ‡|’Jzh8²m½Èˆ7bKxVcğ6ÈIûœ—"éï`©¦UxÒ% ÓˆÎ–Ü¼™õ1g>všpPG©FY˜í:ŠM%õÂÛbÕd='½Ñƒªà¥\GdYzD(|/]6ƒ¤ C£õìæ?ß›éB"° >fŸ	0Í›ÿIG|ƒÓTX(û"ƒ;r¼¦ª‡•n‚NYô]8™”Ö…ÜÚìøD’‰w-K¸ËÃ0¬¤•h9P4ï¡²04#ıCÿÑa¼5½RC$¨NÀ9æ¤ZÎ-3ûkŠ¸›!‹¿}ÒQUâDœsÌ«tç6wvİMÎm¨omŠ°èÃÃrdµoÂW‡)üÙû!Á†IŒŸ„NúİĞGzÕ”<¹LÌ·T©˜*"©¤™…øô&RŠıû¾YHS{$;Nè±,
V’¸:Şù‚^"x£’×öw|¦è¤MÜµãye d`V‰Ù>¥xm<¿f5Øõ3_Â]`’ŠvJ¶U}ßÊK)‚g']éÔ65²s°°Ö¦SóUIFİâ0±¬#¼IÇY¨`ìèÃãßËCÅ)}Ç1Œ§{zï&ÏK+hÌGÀ¡¾MhHèÕêŠ¯©ö§‘ÜbÔVğ sALŒ®†1Úšx7ÃÄ˜|}0> øK&QíâÜ}ƒáWúñÃ¼Š6y=J?s&=ÕçËµ¼¾¯‰±ˆ$¿2Íød»ìÇ}×­h®”:fA½¹DÈ®ûô«|“•Gk·S”-ÛíŸ7w¨Fa“È÷ñÖŸ€pé?èQu$,!ó*Xİ"d«ç2Z€«)ßtá@C‹Ÿš¨ŸãrX¨NœFö¾sÚe&´ïÚ'¤Ë»s…ÂÙD¬b™1c†‹QŠşCVä9£í$h©
uÄo
/Ô©öE6Õóíİˆ÷}Â8ÑL¡yëz~¶ÉÍ§PÕx¯âVßİ÷ù mcu»eÕÌ*Ğ¬f÷uª}ØQ8T,âBõA¿¡Ş¢\¼óµRèè¹7'9Ëüs±
€ÙŒ6§RD>S®½ÍÄ{úÉª?!ğˆ]Î8-šb§`§?¸I:óûq”N94á“¾=Rôx7JÂJèæ‹åß~t’„Õ%*D¶,s‘‹³çË)µİûélÔş?ÌdsğµäVZ×¼pğaî:Ì†Ó4†O‡ô^NIShA>‡3ÂjpQ<“õä$¨ƒ‰êvù¬%KÓær5Ç6`.I8­P¦xÆæ9“ï4ˆ&ï®íòƒc“å„•%‡’ßNœöæ#–í@Q0^­ƒË4(3	's„pÿ®éƒÒÖ;Im)ßÎ¹½9/§tß1NâõªD"ñâ oÈ)ù+=Jp)g&˜ùcî\H8‰Ü2xúó–ÌIøJPBOm×\zGZ1è%”_–Fr¯— znŒğj'ö€âÆÉ©ªIüôÍm­½iR|µ„>¥.E³YxÕµ0Î£/ÿ-+µÃƒ¡¸Õ`‘İk^8=µjÈï8eÙ¿F¤À9›…Óù= Ê¹¤ªÆ+æ–Zä¶5¦ŠR¾p®P'/CSBØ¸¦\ğœ™ô¡UçR£ùnt¸z¹¹ş+)€ú·ĞMö'‡6ëquÛ-˜\°í	q#²AÊç²ñm2d7ÆŒıSRëi§kbS$‹ :Àd÷ğcÖe´D\$Ùh}(®7âƒIëùÔLû›ûøÁZ!q€€1ôtı•ö¬ß©Ú9×yøhm®Ğ¶wøh*xz/ ®VA¶rıßõ’#Û	$‰«#LØ©ñó¬'4çœ8°OóJ¬8™§Ño@– x£üOùò*YzX²%&ªıµºís	M/ÅAº~¹·LhìÖ§1‹Š­}Øˆ¯çÆ°œgÏ”õ½ßøË¹	|:A’L‚İ‘NÒ~8YåKßşEMÕUîĞLwºÿ€ o}€5ÎÛNÉáe’w›WLl­y*$èi‰µÿZ7Ì	Jöô"šfjšmq&Í:âß»´OeIDš’­È´sÄ‰¬¨°(&Ó%qğãPRäAÀc†~†§³»2ÇWè‡mŞÚiqfŞ&aĞB>ª3tèÆƒ”‘…&·e8ød­L'ğT6;ã0ô`r[ÿxNu½šZ•s¾‡(èë’Šbµø¾(à$„ó÷:únö'¶[
ßéñ¾ïq8ù¢zß<A¨(ÒW¸ ÃÀq[Om^<ô¶Ël/cî®ó$¼õÂˆ!‘ÎJÓ²72DPªªê+Î3†B‡ÉH.ìòÖŸi
xæg»Ÿ#ùvj›‚¯ŒE$ƒ¬ÚÂ¯sk»>tnŒ¼©6Ì³îv5_ª‘S±¿Û†°“ï¦”ó;NwàH3›7øÃÖSóIVsØ>'¦µÖı«‡’KÄ5ş„2GÄ%îænoI¤ÍËf$ş–Sp°·ù¼‹2êªëå~ ÃV¨ş‡Ùi_Ú³ğTE.¯¡Ób‰˜neNÌ/ò.Y_w2ÖYV°~…§Ğê«¡K(	­,œ ZoWİ_M"RÚÉjj¡Ê#ò:`H¾W'ˆ©d%AUØÙ²Ò­º:CªÜ+½¡€,FÇÉÿÁm›˜ul¼‚Ä(Œ5àS—/Ä`“(±[Ñœ¼±hJ;²}İ¼äAZÅ×k<‰?!$úÖš››RğK½Î] »ŠD²·»FZg.Z
Ì!ÔÄüñçÖ˜tÇ†‚0Ÿ­šê¦ûºÌtëÌ0©rQ€øÕ–6„UiˆMÁ-ş:àÚ}6"–Œÿª¹Ãğ“œ¿€•,èÑö·{şÌ‡`©Ğ1â€E‰$ßº/të©BfR3ªf¹Ô•­Ş.Q>Mâ¡ŞÿS’2´	˜ÌÆrÜZœ¸XdJMdŒ·l-Édù»Jä7²¯„Øp}ê™gn°7˜}„ßú×#éŠ9Ëd­tB’|{Oi`rD cZy©<i´ëzm$:Ã±§Ò³ip÷×§;ÁbÑI¢w'Éòzt8å4İ;sÉsã}ìlÀVÒX‡(láûl¢Rã5N`íÇCEEb‡zÙ„Œ6k¡;ÚËüÔ‹1’¨ª‚ËØslw8±N'Ôò%ØÇ°ã¨Êio­Á…XByZøşm§ nHU¹£;«v‚a2ÆÚ>½+êdšÆÀ ÖXc(×	YyP¦HÊ·ğ‚‰¶OzmŞ¢æqq6r”˜4wUÜiT)?«Kd&ã_ÊL1¯e[YCˆMjƒ–R!³ˆ?(
(—”ÑÒÉ$0Ö¹Ìã ¥;t†gåbĞõukÙ½ZHïˆúOœÈïµ `»Y•ÇÍhxHä’%ã_›ò0xÄµ¦âD™zK¹á&;cÉ<¯X×¨‹€FI>è3@øg×±ÁËğÎ˜NØ@åcD~¢&‚¸‚ÜÄê~ìæoÏ‚Ÿ®âZú3ªš}½§XCÆKêX GÕÑQd-—r”›R`†Y Z£Ì¡Iüò^ˆvş)3êÄ]V0êh°Rê•RX,q§–Úƒ8\$ÖLõ{	‚oñ1Kîu[öQG ¢"¥¸ôF–BâÅß¦-?&ŸØı(<ğÿ&Gºpè]ãì
-ÃjĞ¿R±`+Ü…d£VÓ@R¦.šx‡—™=áÖğğÕˆ¨/½A‰ôîÚ²qòİ~|¤•wvšƒÎêßCDq[Êdæ~ÍV\¼ŠfÚtÎ­Ø_‘B´`åÍMşÃeµòG„±hn¤­\ê£ÎÊÍµMI\í'F^£íK­ùñ‡DŠÖúæiÓğ¨²ÿQ«§åT4Ô£ûï¼kE?‚¶™±¡€—B6¥™==Š a;ä°Tô´>çÿd÷åƒ¶Ó±²¹lòÚt»ÔïÖ+ôTè‰°\ÕÅ»§ÏÙ?ª©// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared remote control input device
 *
 *  Most of this file is
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 *
 *  However, the cx23885_input_{init,fini} functions contained herein are
 *  derived from Linux kernel files linux/media/video/.../...-input.c marked as:
 *
 *  Copyright (C) 2008 <srinivasa.deevi at conexant dot com>
 *  Copyright (C) 2005 Ludovico Cavedon <cavedon@sssup.it>
 *		       Markus Rechberger <mrechberger@gmail.com>
 *		       Mauro Carvalho Chehab <mchehab@kernel.org>
 *		       Sascha Sommer <saschasommer@freenet.de>
 *  Copyright (C) 2004, 2005 Chris Pascoe
 *  Copyright (C) 2003, 2004 Gerd Knorr
 *  Copyright (C) 2003 Pavel Machek
 */

#include "cx23885.h"
#include "cx23885-input.h"

#include <linux/slab.h>
#include <media/rc-core.h>
#include <media/v4l2-subdev.h>

#define MODULE_NAME "cx23885"

static void cx23885_input_process_measurements(struct cx23885_dev *dev,
					       bool overrun)
{
	struct cx23885_kernel_ir *kernel_ir = dev->kernel_ir;

	ssize_t num;
	int count, i;
	bool handle = false;
	struct ir_raw_event ir_core_event[64];

	do {
		num = 0;
		v4l2_subdev_call(dev->sd_ir, ir, rx_read, (u8 *) ir_core_event,
				 sizeof(ir_core_event), &num);

		count = num / sizeof(struct ir_raw_event);

		for (i = 0; i < count; i++) {
			ir_raw_event_store(kernel_ir->rc,
					   &ir_core_event[i]);
			handle = true;
		}
	} while (num != 0);

	if (overrun)
		ir_raw_event_overflow(kernel_ir->rc);
	else if (handle)
		ir_raw_event_handle(kernel_ir->rc);
}

void cx23885_input_rx_work_handler(struct cx23885_dev *dev, u32 events)
{
	struct v4l2_subdev_ir_parameters params;
	int overrun, data_available;

	if (dev->sd_ir == NULL || events == 0)
		return;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/*
		 * The only boards we handle right now.  However other boards
		 * using the CX2388x integrated IR controller should be similar
		 */
		break;
	default:
		return;
	}

	overrun = events & (V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN |
			    V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN);

	data_available = events & (V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED |
				   V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ);

	if (overrun) {
		/* If there was a FIFO overrun, stop the device */
		v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
		params.enable = false;
		/* Mitigate race with cx23885_input_ir_stop() */
		params.shutdown = atomic_read(&dev->ir_input_stopping);
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	}

	if (data_available)
		cx23885_input_process_measurements(dev, overrun);

	if (overrun) {
		/* If there was a FIFO overrun, clear & restart the device */
		params.enable = true;
		/* Mitigate race with cx23885_input_ir_stop() */
		params.shutdown = atomic_read(&dev->ir_input_stopping);
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	}
}

static int cx23885_input_ir_start(struct cx23885_dev *dev)
{
	struct v4l2_subdev_ir_parameters params;

	if (dev->sd_ir == NULL)
		return -ENODEV;

	atomic_set(&dev->ir_input_stopping, 0);

	v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		*/
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
		params.shutdown = false;

		/* Setup for baseband compatible with both RC-5 and RC-6A */
		params.modulation = false;
		/* RC-5:  2,222,222 ns = 1/36 kHz * 32 cycles * 2 marks * 1.25*/
		/* RC-6A: 3,333,333 ns = 1/36 kHz * 16 cycles * 6 marks * 1.25*/
		params.max_pulse_width = 3333333; /* ns */
		/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
		/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
		params.noise_filter_min_width = 333333; /* ns */
		/*
		 * This board has inverted receive sense:
		 * mark is received as low logic level;
		 * falling edges are detected as rising edges; etc.
		 */
		params.invert_level = true;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		 */
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
		params.shutdown = false;

		/* Setup for a standard NEC protocol */
		params.carrier_freq = 37917; /* Hz, 455 kHz/12 for NEC */
		params.carrier_range_lower = 33000; /* Hz */
		params.carrier_range_upper = 43000; /* Hz */
		params.duty_cycle = 33; /* percent, 33 percent for NEC */

		/*
		 * NEC max pulse width: (64/3)/(455 kHz/12) * 16 nec_units
		 * (64/3)/(455 kHz/12) * 16 nec_units * 1.375 = 12378022 ns
		 */
		params.max_pulse_width = 12378022; /* ns */

		/*
		 * NEC noise filter min width: (64/3)/(455 kHz/12) * 1 nec_unit
		 * (64/3)/(455 kHz/12) * 1 nec_units * 0.625 = 351648 ns
		 */
		params.noise_filter_min_width = 351648; /* ns */

		params.modulation = false;
		params.invert_level = true;
		break;
	}
	v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	return 0;
}

static int cx23885_input_ir_open(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx == NULL)
		return -ENODEV;

	return cx23885_input_ir_start(kernel_ir->cx);
}

static void cx23885_input_ir_stop(struct cx23885_dev *dev)
{
	struct v4l2_subdev_ir_parameters params;

	if (dev->sd_ir == NULL)
		return;

	/*
	 * Stop the sd_ir subdevice from generating notifications and
	 * scheduling work.
	 * It is shutdown this way in order to mitigate a race with
	 * cx23885_input_rx_work_handler() in the overrun case, which could
	 * re-enable the subdevice.
	 */
	atomic_set(&dev->ir_input_stopping, 1);
	v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	while (params.shutdown == false) {
		params.enable = false;
		params.interrupt_enable = false;
		params.shutdown = true;
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
		v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	}
	flush_work(&dev->cx25840_work);
	flush_work(&dev->ir_rx_work);
	flush_work(&dev->ir_tx_work);
}

static void cx23885_input_ir_close(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx != NULL)
		cx23885_input_ir_stop(kernel_ir->cx);
}

int cx23885_input_init(struct cx23885_dev *dev)
{
	struct cx23885_kernel_ir *kernel_ir;
	struct rc_dev *rc;
	char *rc_map;
	u64 allowed_protos;

	int ret;

	/*
	 * If the IR device (hardware registers, chip, GPIO lines, etc.) isn't
	 * encapsulated in a v4l2_subdev, then I'm not going to deal with it.
	 */
	if (dev->sd_ir == NULL)
		return -ENODEV;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/* Integrated CX2388[58] IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* The grey Hauppauge RC-5 remote */
		rc_map = RC_MAP_HAUPPAUGE;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* The grey Terratec remote with orange buttons */
		rc_map = RC_MAP_NEC_TERRATEC_CINERGY_XS;
		break;
	case CX23885_BOARD_TEVII_S470:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TEVII_NEC;
		break;
	case CX23885_BOARD_MYGICA_X8507:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TOTAL_MEDIA_IN_HAND_02;
		break;
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TBS_NEC;
		break;
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		rc_map = RC_MAP_DVBSKY;
		break;
	case CX23885_BOARD_TT_CT2_4500_CI:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		rc_map = RC_MAP_TT_1500;
		break;
	default:
		return -ENODEV;
	}

	/* cx23885 board instance kernel IR state */
	kernel_ir = kzalloc(sizeof(struct cx23885_kernel_ir), GFP_KERNEL);
	if (kernel_ir == NULL)
		return -ENOMEM;

	kernel_ir->cx = dev;
	kernel_ir->name = kasprintf(GFP_KERNEL, "cx23885 IR (%s)",
				    cx23885_boards[dev->board].name);
	if (!kernel_ir->name) {
		ret = -ENOMEM;
		goto err_out_free;
	}

	kernel_ir->phys = kasprintf(GFP_KERNEL, "pci-%s/ir0",
				    pci_name(dev->pci));
	if (!kernel_ir->phys) {
		ret = -ENOMEM;
		goto err_out_free_name;
	}

	/* input device */
	rc = rc_allocate_device(RC_DRIVER_IR_RAW);
	if (!rc) {
		ret = -ENOMEM;
		goto err_out_free_phys;
	}

	kernel_ir->rc = rc;
	rc->device_name = kernel_ir->name;
	rc->input_phys = kernel_ir->phys;
	rc->input_id.bustype = BUS_PCI;
	rc->input_id.version = 1;
	if (dev->pci->subsystem_vendor) {
		rc->input_id.vendor  = dev->pci->subsystem_vendor;
		rc->input_id.product = dev->pci->subsystem_device;
	} else {
		rc->input_id.vendor  = dev->pci->vendor;
		rc->input_id.product = dev->pci->device;
	}
	rc->dev.parent = &dev->pci->dev;
	rc->allowed_protocols = allowed_protos;
	rc->priv = kernel_ir;
	rc->open = cx23885_input_ir_open;
	rc->close = cx23885_input_ir_close;
	rc->map_name = rc_map;
	rc->driver_name = MODULE_NAME;

	/* Go */
	dev->kernel_ir = kernel_ir;
	ret = rc_register_device(rc);
	if (ret)
		goto err_out_stop;

	return 0;

err_out_stop:
	cx23885_input_ir_stop(dev);
	dev->kernel_ir = NULL;
	rc_free_device(rc);
err_out_free_phys:
	kfree(kernel_ir->phys);
err_out_free_name:
	kfree(kernel_ir->name);
err_out_free:
	kfree(kernel_ir);
	return ret;
}

void cx23885_input_fini(struct cx23885_dev *dev)
{
	/* Always stop the IR hardware from generating interrupts */
	cx23885_input_ir_stop(dev);

	if (dev->kernel_ir == NULL)
		return;
	rc_unregister_device(dev->kernel_ir->rc);
	kfree(dev->kernel_ir->phys);
	kfree(dev->kernel_ir->name);
	kfree(dev->kernel_ir);
	dev->kernel_ir = NULL;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      Fùº:C:†«¼>×˜ú3j#5Ò7$’/ƒC¡bG)”°‚¸qŠĞÍï³ôÖ~}õÓŞü ^‹›2Êó§Û‡9şNŠğˆc.ŒoërOj±^Œ³m³ÉiÃæ ¼ÊÊäüöÉ¢çöfÿ×¬e¸©İ éì0D†N¥¤i\wmM[DÃ©hºëz]’¿%ä}yw.·t_ıÆU`ñÜú‹8ÅÇ‡]&Ò!‹j‘·->¥Ìà#s@Òw²!W±'=
¿¾—\†0`¼Áu"â6×4Š×è—à7í¸ ÿRN€²‘ku\%Ä=7.õùMòl»¯±;IeÙ¥ĞÈî¡g¹zã4ö9¿T¥‰Fš£¼À‹şÎ"¦‡¤P…^™^Ñ­¯ÇÉÿ	FÛ×M —c™âµ´ÆÀÕİ~äŸã@¦¾§o¥#tÁVY&§7xÇ¹˜"äšØÊV"Êše…Î$Š‡b~—àô¾<‡:øğ=qİåØ’È<åùÔ€´­	>²şÖh#2+H,5^‹#†‡Áæo[‘Èp¦èaX¶Û¢>Z°Ôk9å†UPÔÏ/ì¶¨”qwpHvû[¡ıEóş0lÀlœ›•jË¹m‘cC—öÿ^`¼U; Æ$#´ŒYO‰,œ_ã	*İ>œ±^ª{coa*3¸öCr.+ƒ†
Ú2c"×ãÖ@K¿¡+ÁÍcW c.Ÿ-ºóø &:ø`p¬» ıÚêk’IÓª€´\É¶LU/ş“yHrf0FÃÊ“NÛ0¼Lf1©,:Ô½´²~ŞÂEH	Dê3.+°™®ıÚó¬tƒ Œhúª¢»ƒ¨:®ÉQNÖ’qü/çGĞRVbo`oñZh‹R
×tàr˜eÊ—ÁÍ1¬´µÁ•ªµF—ÕàÖƒ8RÇPuZ˜=•ñ§s À/O% Ç#…È²ÁĞàDv@´cÀ‡lr^z?¹¡{l¿ å¯i\Š(U[IÓ#Á,sŸbßí3¨}@#”³._|k	¦Eıu&¾&^¼?ÜU¤¤ÙJhÁ_1VKG‘Ã\ûƒVÒCH§åœ¬w+TœõÆ“Ó¾z¢±°kMòÒî@ÎİDšÌ®‘BÛeó} ‹{S®¦¤(Ö°Äu	¸›¼ÉğÎiX·™‰Y˜6T¨ªP¦BuRŞªa½ip^•îZeWfî),ŒœİÿİĞªq
:ğ¬Uö˜káLë£¯‰‰ä›PóT¦E¿’;(ÙV*Ã9ãÁ=>8¢*Z²¼F“îuË(8¼ŒĞU›LÁ&!–mŞ&Î‘v,˜Á&@{R­QNªíw_¦oú·¦DâÆ‚î;ŠùÑÖ{DyO{ÏäyXÕ?hM?'ˆsU5ÊUÖ'®Ó†çÒ:µ:»±Où´½Ø9¼}ñ/¡ªt»äìïo¾` :fúü®úNãØŠ§«È.=%)À’Bò¡ÎÚôïÎ_Àz]+ÂB{câS0Âáâç(|ßo…qætIŸ¹Ñ¹aÛ8tk'’4/ô°ı”â&‰>¯Şùà“pˆÆ-®dµiøÙ;1OFš~yW9“ìË¤Ù–î@fØÏU‡âÃÇè=g—ÃÂ
Ás¯îĞŠL|º¶<ŒÑ:¹Nf/‘ëØQ>ãeÈo º5Àæ‘‹kš.lO¦œÛÅø"Ï¼Ê1kñ÷,"È² ‚/¼-OÖºPÈxìØ½äœ'½7ü¶ó¾,ƒıOÏj)ïHm>)’^úÙÜÒé—zìÙğµ&ï¢;-0¤âÖB¸ÚÕ%Ã#ªT¡¨G‹¸ÃYıUNc‹#,|#)fMyìf+¼§”<¶ 8ì#yÆK£ç“­;â æOG­½)wN³Ï1˜n‰F©‰Æ1
†`c™d!gr½­ĞäePl.•TŸ%âŸæ,Šzšµ!Æ©[u´µ’–áU‹É ºğC·éˆ~«=°.É½-$j…«”' ª¾…•é…PÊêª«ƒV}Ò(«Š¸Äö87À]ír’¬ ‰âmwËÖßÎ¶¶ºi(Û§OßN¥÷¢2\Q_=r<¾åZs¤*i2ä5;ÎÃŒVtÛØV%ÎWJë¢VE_”A†ANdP“kP™',(ÆtI:¥A{2[cî¥¬-¼º=Â‡iªÿ‹j¢”ƒ|=? %´)±ªµÆé@^ÿ»áÚ2¡7ß² ûÎì*ÖëĞ	ÿ0´0<•ÎQà{¢êC’K0¹ªV>5xÈNøƒ'Ë¤¥#uaceg}õäk"åÒ¨"y·O˜×÷f6ºC]#çPû;>sãİ8Ë¯+MÆËÅåVh§ÖkãVãÏá…ªó0zóÒ‹(-Š/RøÏ­[út{|ñ{w©¹äzít£K@
)“½™¦AÇUÅ”l¬´ÍÆ†<Şçğ]„}°ÓB¨0¥†Üß©·ß°·jQ \Çp#0EWˆŸ²;‡b”å¬ÛÈBkf*nÎŸïcJ&â¸bB+Vœ¢YµÒ8×ZnÀ¼<}”)Ï*¾¾âıG­=Y²'UX®z*¹’AÇŸØÂ0Dô+eÕ½ú#¨(f?ûu¥jùÛì/Ë,wÆâ bû±ïËÆ·d‰ Š_ı $=™·~õÊZ&ÖF,p›˜%Ç‘™­™g w<i-A0eïùK¿Åº #Ô£¥¨¢öDÉ¢ßô	E–KüŞ>àõ€CÏ€°e¥d.“S
|L«†ĞCGzŒV´ü[DôZ%~ÂJ0ºe‡´ûˆqPkó;²àp´+égñİ®ßx“	šÿ*bdÈKƒnÃF-òÁ¤RÏgE@.-Ç9÷y' øD–KRVvœàĞ L×Âm’1ë²ø‘÷nìnRDï‚ÈA£¨ˆ¾”ÈÚl§ÜOM¦5Q^ÆÏØrQe!Xû€³©¨ş×í|çé‘¼2à¯õ¡á&YJ§CŠcpßB$àâ¢kq¡É»9¢)ÚÍ:ŸÏ*â2ø­IGE6•| …—’9ì`g!V2]6{4T2&Sf»%>ï2ãE|!¶Ï7ÄšT
Ş1¨‹ëë¿Zºª·'ƒßw´”ˆXŸÇQK	Â°|GÒ^Û(úƒ]'ô}ÀmbKh6²¾1ìê®7œ=µ“‹$ÆÇï3üOîı’=ó…_±Ğ¦Ù¹Kùè”%›)|o·¯•4j×²«_„‘…Ù5<Ä'4Î™İa"ãİù©Ğd0°µ
’w‹Üú*ÍÔ“€ƒ–o½ËD­d4²*½.Á6—pÿYç/¸5ÿ’şƒ åw¸xÁèµdG*äÀ.øhïÔ¿²¨ç†°©sk[	.ôÃ/ÈıÊû	÷c{Ä°s.GÄŠjaÔœè¹N¬¨è!ù#ÎşÄH¦Guî¿îH‘¿WzÃø]˜ºn8o¸º˜ofé:¦*ìKU%†ë6ì<’ğÏ«¢ÿ&pgşä•(õÅ×ÀOÇÊ%x }’l+#ˆ‘[¿‚Ùó§<÷²^.½€3#Oœ×	Ü™½¥ˆBXİËL.L“Íf¹’†;¹$İ%®xHºZù0~Gq“œ5‚˜ÿÕ»é®…O4Nş*zØÿ4½®à€İÍ9'QäïHH :¤ÜÉÛ{`ôÓRlÌ(ø€G3WKêè˜woPœ’ßŒèc(ËÛ–ºÁØ[0èPdI”1+6@'ÆıÂÅ1)[ÅJœ ÉÔ°QÂßV*¸*l[Å3…»ŞxaË
¡çê…¼¯\¬×EÃ¥QtÔGœd¹Aˆ˜à`òë‡R¼ßÉKóœ;có O7Ÿ¸ÿ¯Œ<J¿Ôãm;óêÇ”D¾Şl«ƒ_®|¸Š.ó©x^ÜİÍåŒ¯Ô]-ÈÄ„ûÏq"´½xÀwñ\__Ò”©² İßoWâS§"Zö+İÎñÃDÄö»hÇú™ÈçBi½›¹·¶ymîE_d9 ‚k3#Äì+ÄÅÕ–h>RúqÛ7Ü:œª¥Z9[äC*CĞ÷µnĞ})“×Ñ–hvÎÂ¿ò Å} ñŒöºÿ ²°WW	ö5EŠ¼ÿn!Aáª¶Ş×‚1`7–ã‘]–Ùü^Àá6Ù±q`ûë=GIgï6ıâ‹­T+d!æC@ğ$û_ù©îNOqe»CX\îúaÃÈ:r<ş'Óià€ÇóßnIÍ­Ã5×Ó´\ì8bÜ‘vfa¡”@ø ¶†´ûè2şä¹ÑÛT:!TÂ¡õ­³$»±6Õ	üø.1	ÆmQˆ—rF¯>9}Ùôá“Û\aà‡Lû
:Úvvhş#°A¤MÌï¶Â¨Ïxy†ÁéØ#|ÓGS}ëAe9ûdw­P— ilyñ§æ¶Iõk·Noß¾U;Á§wÿÄ¸”§z«N]IØl–=^ÙÚ•‹WÁ«çUòğ‡©•9ÿ–Áğ‰×«‘á(QC³S½ÿŠş/&à lá3KÂ²:x”–uA5ãtÀ¸œÁyåK6sÌ£<YW¡¹LH±M/æûëP>Û«é+ÚîÌ'¢å´´n ÒqijXPìšşt
ïu\9Šæ˜#ÀKSÊR1Kvy>ûºvE£KrùÇ?÷q*+­è3š¨dÀÆn‰ïíÈççÓ¨>†h¡í	Îf;R¤Úş¹!;?ª¾¸@l›rp÷z0‹z¿Ã+ÜR+Mã„ª$’´ÍÌ‚k
%èißhêKÂÑÏtƒ&RâHˆX°ré`eô­3”FT8¾ş#Äõ-l–Eô°J­óìY£Òºt[¼qú
ŞyiúUğ	­†0µ¬4ìáEøPnZÍ•$÷yÚ@lÄ.‹[Ä ‹VGû4
‚¹Û¼-ª'T¶mèÕ¯“^$búEÏôÚÊXİQ9,üàë -yÅÍtûGjšäãïŸ	ùnGnJÏOÒwv=^SúËgÔé×$ï<…ıÔ~ O¼Ğ7ù€ØÜğİOrzƒl`vªìµwï}ImÒi‡ï÷ÒïñÕc|ME–bÌû´¶ÁÇ³¥kÒvª %ù=‚Ã‡§ó‹š’Tí$UH¹ªi»AÉ!@¡s*j	Ñ{':œ‚¥2h¥$’áÓô~ú©És¥D?˜
bvqD‰øêìªeo¸ƒ1
Ni³Ä-d4èl¥^°¯`4o?¬AÍÃZÃ¯á~Ş†ÍŸö;î¶t‡µv"ƒ2ãáŒÍRûšHàd;Ú¬*K…ÿĞğJ¸à šÎŒâùœs÷&L#ş)Ç¦’ï$cpu() more frequently to try to loosen things up a bit.
	 * Also check to see if the CPU is getting hammered with interrupts,
	 * but only once per grace period, just to keep the IPIs down to
	 * a dull roar.
	 */
	if (time_after(jiffies, rcu_state.jiffies_resched)) {
		if (time_after(jiffies,
			       READ_ONCE(rdp->last_fqs_resched) + jtsq)) {
			resched_cpu(rdp->cpu);
			WRITE_ONCE(rdp->last_fqs_resched, jiffies);
		}
		if (IS_ENABLED(CONFIG_IRQ_WORK) &&
		    !rdp->rcu_iw_pending && rdp->rcu_iw_gp_seq != rnp->gp_seq &&
		    (rnp->ffmask & rdp->grpmask)) {
			rdp->rcu_iw_pending = true;
			rdp->rcu_iw_gp_seq = rnp->gp_seq;
			irq_work_queue_on(&rdp->rcu_iw, rdp->cpu);
		}
	}

	return 0;
}

/* Trace-event wrapper function for trace_rcu_future_grace_period.  */
static void trace_rcu_this_gp(struct rcu_node *rnp, struct rcu_data *rdp,
			      unsigned long gp_seq_req, const char *s)
{
	trace_rcu_future_grace_period(rcu_state.name, READ_ONCE(rnp->gp_seq),
				      gp_seq_req, rnp->level,
				      rnp->grplo, rnp->grphi, s);
}

/*
 * rcu_start_this_gp - Request the start of a particular grace period
 * @rnp_start: The leaf node of the CPU from which to start.
 * @rdp: The rcu_data corresponding to the CPU from which to start.
 * @gp_seq_req: The gp_seq of the grace period to start.
 *
 * Start the specified grace period, as needed to handle newly arrived
 * callbacks.  The required future grace periods are recorded in each
 * rcu_node structure's ->gp_seq_needed field.  Returns true if there
 * is reason to awaken the grace-period kthread.
 *
 * The caller must hold the specified rcu_node structure's ->lock, which
 * is why the caller is responsible for waking the grace-period kthread.
 *
 * Returns true if the GP thread needs to be awakened else false.
 */
static bool rcu_start_this_gp(struct rcu_node *rnp_start, struct rcu_data *rdp,
			      unsigned long gp_seq_req)
{
	bool ret = false;
	struct rcu_node *rnp;

	/*
	 * Use funnel locking to either acquire the root rcu_node
	 * structure's lock or bail out if the need for this grace period
	 * has already been recorded -- or if that grace period has in
	 * fact already started.  If there is already a grace period in
	 * progress in a non-leaf node, no recording is needed because the
	 * end of the grace period will scan the leaf rcu_node structures.
	 * Note that rnp_start->lock must not be released.
	 */
	raw_lockdep_assert_held_rcu_node(rnp_start);
	trace_rcu_this_gp(rnp_start, rdp, gp_seq_req, TPS("Startleaf"));
	for (rnp = rnp_start; 1; rnp = rnp->parent) {
		if (rnp != rnp_start)
			raw_spin_lock_rcu_node(rnp);
		if (ULONG_CMP_GE(rnp->gp_seq_needed, gp_seq_req) ||
		    rcu_seq_started(&rnp->gp_seq, gp_seq_req) ||
		    (rnp != rnp_start &&
		     rcu_seq_state(rcu_seq_current(&rnp->gp_seq)))) {
			trace_rcu_this_gp(rnp, rdp, gp_seq_req,
					  TPS("Prestarted"));
			goto unlock_out;
		}
		WRITE_ONCE(rnp->gp_seq_needed, gp_seq_req);
		if (rcu_seq_state(rcu_seq_current(&rnp->gp_seq))) {
			/*
			 * We just marked the leaf or internal node, and a
			 * grace period is in progress, which means that
			 * rcu_gp_cleanup() will see the marking.  Bail to
			 * reduce contention.
			 */
			trace_rcu_this_gp(rnp_start, rdp, gp_seq_req,
					  TPS("Startedleaf"));
			goto unlock_out;
		}
		if (rnp != rnp_start && rnp->parent != NULL)
			raw_spin_unlock_rcu_node(rnp);
		if (!rnp->parent)
			break;  /* At root, and perhaps also leaf. */
	}

	/* If GP already in progress, just leave, otherwise start one. */
	if (rcu_gp_in_progress()) {
		trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("Startedleafroot"));
		goto unlock_out;
	}
	trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("Startedroot"));
	WRITE_ONCE(rcu_state.gp_flags, rcu_state.gp_flags | RCU_GP_FLAG_INIT);
	WRITE_ONCE(rcu_state.gp_req_activity, jiffies);
	if (!READ_ONCE(rcu_state.gp_kthread)) {
		trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("NoGPkthread"));
		goto unlock_out;
	}
	trace_rcu_grace_period(rcu_state.name, data_race(rcu_state.gp_seq), TPS("newreq"));
	ret = true;  /* Caller must wake GP kthread. */
unlock_out:
	/* Push furthest requested GP to leaf node and rcu_data structure. */
	if (ULONG_CMP_LT(gp_seq_req, rnp->gp_seq_needed)) {
		WRITE_ONCE(rnp_start->gp_seq_needed, rnp->gp_seq_needed);
		WRITE_ONCE(rdp->gp_seq_needed, rnp->gp_seq_needed);
	}
	if (rnp != rnp_start)
		raw_spin_unlock_rcu_node(rnp);
	return ret;
}

/*
 * Clean up any old requests for the just-ended grace period.  Also return
 * whether any additional grace periods have been requested.
 */
static bool rcu_future_gp_cleanup(struct rcu_node *rnp)
{
	bool needmore;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);

	needmore = ULONG_CMP_LT(rnp->gp_seq, rnp->gp_seq_needed);
	if (!needmore)
		rnp->gp_seq_needed = rnp->gp_seq; /* Avoid counter wrap. */
	trace_rcu_this_gp(rnp, rdp, rnp->gp_seq,
			  needmore ? TPS("CleanupMore") : TPS("Cleanup"));
	return needmore;
}

/*
 * Awaken the grace-period kthread.  Don't do a self-awaken (unless in an
 * interrupt or softirq handler, in which case we just might immediately
 * sleep upon return, resulting in a grace-period hang), and don't bother
 * awakening when there is nothing for the grace-period kthread to do
 * (as in several CPUs raced to awaken, we lost), and finally don't try
 * to awaken a kthread that has not yet been created.  If all those checks
 * are passed, track some debug information and awaken.
 *
 * So why do the self-wakeup when in an interrupt or softirq handler
 * in the grace-period kthread's context?  Because the kthread might have
 * been interrupted just as it was going to sleep, and just after the final
 * pre-sleep check of the awaken condition.  In this case, a wakeup really
 * is required, and is therefore supplied.
 */
static void rcu_gp_kthread_wake(void)
{
	struct task_struct *t = READ_ONCE(rcu_state.gp_kthread);

	if ((current == t && !in_hardirq() && !in_serving_softirq()) ||
	    !READ_ONCE(rcu_state.gp_flags) || !t)
		return;
	WRITE_ONCE(rcu_state.gp_wake_time, jiffies);
	WRITE_ONCE(rcu_state.gp_wake_seq, READ_ONCE(rcu_state.gp_seq));
	swake_up_one(&rcu_state.gp_wq);
}

/*
 * If there is room, assign a ->gp_seq number to any callbacks on this
 * CPU that have not already been assigned.  Also accelerate any callbacks
 * that were previously assigned a ->gp_seq number that has since proven
 * to be too conservative, which can happen if callbacks get assigned a
 * ->gp_seq number while RCU is idle, but with reference to a non-root
 * rcu_node structure.  This function is idempotent, so it does not hurt
 * to call it repeatedly.  Returns an flag saying that we should awaken
 * the RCU grace-period kthread.
 *
 * The caller must hold rnp->lock with interrupts disabled.
 */
static bool rcu_accelerate_cbs(struct rcu_node *rnp, struct rcu_data *rdp)
{
	unsigned long gp_seq_req;
	bool ret = false;

	rcu_lockdep_assert_cblist_protected(rdp);
	raw_lockdep_assert_held_rcu_node(rnp);

	/* If no pending (not yet ready to invoke) callbacks, nothing to do. */
	if (!rcu_segcblist_pend_cbs(&rdp->cblist))
		return false;

	trace_rcu_segcb_stats(&rdp->cblist, TPS("SegCbPreAcc"));

	/*
	 * Callbacks are often registered with incomplete grace-period
	 * information.  Something about the fact that getting exact
	 * information requires acquiring a global lock...  RCU therefore
	 * makes a conservative estimate of the grace period number at which
	 * a given callback will become ready to invoke.	The following
	 * code checks this estimate and improves it when possible, thus
	 * accelerating callback invocation to an earlier grace-period
	 * number.
	 */
	gp_seq_req = rcu_seq_snap(&rcu_state.gp_seq);
	if (rcu_segcblist_accelerate(&rdp->cblist, gp_seq_req))
		ret = rcu_start_this_gp(rnp, rdp, gp_seq_req);

	/* Trace depending on how much we were able to accelerate. */
	if (rcu_segcblist_restempty(&rdp->cblist, RCU_WAIT_TAIL))
		trace_rcu_grace_period(rcu_state.name, gp_seq_req, TPS("AccWaitCB"));
	else
		trace_rcu_grace_period(rcu_state.name, gp_seq_req, TPS("AccReadyCB"));

	trace_rcu_segcb_stats(&rdp->cblist, TPS("SegCbPostAcc"));

	return ret;
}

/*
 * Similar to rcu_accelerate_cbs(), but does not require that the leaf
 * rcu_node structure's ->lock be held.  It consults the cached value
 * of ->gp_seq_needed in the rcu_data structure, and if that indicates
 * that a new grace-period request be made, invokes rcu_accelerate_cbs()
 * while holding the leaf rcu_node structure's ->lock.
 */
static void rcu_accelerate_cbs_unlocked(struct rcu_node *rnp,
					struct rcu_data *rdp)
{
	unsigned long c;
	bool needwake;

	rcu_lockdep_assert_cblist_protected(rdp);
	c = rcu_seq_snap(&rcu_state.gp_seq);
	if (!READ_ONCE(rdp->gpwrap) && ULONG_CMP_GE(rdp->gp_seq_needed, c)) {
		/* Old request still live, so mark recent callbacks. */
		(void)rcu_segcblist_accelerate(&rdp->cblist, c);
		return;
	}
	raw_spin_lock_rcu_node(rnp); /* irqs already disabled. */
	needwake = rcu_accelerate_cbs(rnp, rdp);
	raw_spin_unlock_rcu_node(rnp); /* irqs remain disabled. */
	if (needwake)
		rcu_gp_kthread_wake();
}

/*
 * Move any callbacks whose grace period has completed to the
 * RCU_DONE_TAIL sublist, then compact the remaining sublists and
 * assign ->gp_seq numbers to any callbacks in the RCU_NEXT_TAIL
 * sublist.  This function is idempotent, so it does not hurt to
 * invoke it repeatedly.  As long as it is not invoked -too- often...
 * Returns true if the RCU grace-period kthread needs to be awakened.
 *
 * The caller must hold rnp->lock with interrupts disabled.
 */
static bool rcu_advance_cbs(struct rcu_node *rnp, struct rcu_data *rdp)
{
	rcu_lockdep_assert_cblist_protected(rdp);
	raw_lockdep_assert_held_rcu_node(rnp);

	/* If no pending (not yet ready to invoke) callbacks, nothing to do. */
	if (!rcu_segcblist_pend_cbs(&rdp->cblist))
		return false;

	/*
	 * Find all callbacks whose ->gp_seq numbers indicate that they
	 * are ready to invoke, and put them into the RCU_DONE_TAIL sublist.
	 */
	rcu_segcblist_advance(&rdp->cblist, rnp->gp_seq);

	/* Classify any remaining callbacks. */
	return rcu_accelerate_cbs(rnp, rdp);
}

/*
 * Move and classify callbacks, but only if doing so won't require
 * that the RCU grace-period kthread be awakened.
 */
static void __maybe_unused rcu_advance_cbs_nowake(struct rcu_node *rnp,
						  struct rcu_data *rdp)
{
	rcu_lockdep_assert_cblist_protected(rdp);
	if (!rcu_seq_state(rcu_seq_current(&rnp->gp_seq)) || !raw_spin_trylock_rcu_node(rnp))
		return;
	// The grace period cannot end while we hold the rcu_node lock.
	if (rcu_seq_state(rcu_seq_current(&rnp->gp_seq)))
		WARN_ON_ONCE(rcu_advance_cbs(rnp, rdp));
	raw_spin_unlock_rcu_node(rnp);
}

/*
 * In CONFIG_RCU_STRICT_GRACE_PERIOD=y kernels, attempt to generate a
 * quiescent state.  This is intended to be invoked when the CPU notices
 * a new grace period.
 */
static void rcu_strict_gp_check_qs(void)
{
	if (IS_ENABLED(CONFIG_RCU_STRICT_GRACE_PERIOD)) {
		rcu_read_lock();
		rcu_read_unlock();
	}
}

/*
 * Update CPU-local rcu_data state to record the beginnings and ends of
 * grace periods.  The caller must hold the ->lock of the leaf rcu_node
 * structure corresponding to the current CPU, and must have irqs disabled.
 * Returns true if the grace-period kthread needs to be awakened.
 */
static bool __note_gp_changes(struct rcu_node *rnp, struct rcu_data *rdp)
{
	bool ret = false;
	bool need_qs;
	const bool offloaded = rcu_rdp_is_offloaded(rdp);

	raw_lockdep_assert_held_rcu_node(rnp);

	if (rdp->gp_seq == rnp->gp_seq)
		return false; /* Nothing to do. */

	/* Handle the ends of any preceding grace periods first. */
	if (rcu_seq_completed_gp(rdp->gp_seq, rnp->gp_seq) ||
	    unlikely(READ_ONCE(rdp->gpwrap))) {
		if (!offloaded)
			ret = rcu_advance_cbs(rnp, rdp); /* Advance CBs. */
		rdp->core_needs_qs = false;
		trace_rcu_grace_period(rcu_state.name, rdp->gp_seq, TPS("cpuend"));
	} else {
		if (!offloaded)
			ret = rcu_accelerate_cbs(rnp, rdp); /* Recent CBs. */
		if (rdp->core_needs_qs)
			rdp->core_needs_qs = !!(rnp->qsmask & rdp->grpmask);
	}

	/* Now handle the beginnings of any new-to-this-CPU grace periods. */
	if (rcu_seq_new_gp(rdp->gp_seq, rnp->gp_seq) ||
	    unlikely(READ_ONCE(rdp->gpwrap))) {
		/*
		 * If the current grace period is waiting for this CPU,
		 * set up to detect a quiescent state, otherwise don't
		 * go looking for one.
		 */
		trace_rcu_grace_period(rcu_state.name, rnp->gp_seq, TPS("cpustart"));
		need_qs = !!(rnp->qsmask & rdp->grpmask);
		rdp->cpu_no_qs.b.norm = need_qs;
		rdp->core_needs_qs = need_qs;
		zero_cpu_stall_ticks(rdp);
	}
	rdp->gp_seq = rnp->gp_seq;  /* Remember new grace-period state. */
	if (ULONG_CMP_LT(rdp->gp_seq_needed, rnp->gp_seq_needed) || rdp->gpwrap)
		WRITE_ONCE(rdp->gp_seq_needed, rnp->gp_seq_needed);
	WRITE_ONCE(rdp->gpwrap, false);
	rcu_gpnum_ovf(rnp, rdp);
	return ret;
}

static void note_gp_changes(struct rcu_data *rdp)
{
	unsigned long flags;
	bool needwake;
	struct rcu_node *rnp;

	local_irq_save(flags);
	rnp = rdp->mynode;
	if ((rdp->gp_seq == rcu_seq_current(&rnp->gp_seq) &&
	     !unlikely(READ_ONCE(rdp->gpwrap))) || /* w/out lock. */
	    !raw_spin_trylock_rcu_node(rnp)) { /* irqs already off, so later. */
		local_irq_restore(flags);
		return;
	}
	needwake = __note_gp_changes(rnp, rdp);
	raw_spin_unlock_irqrestore_rcu_node(rnp, flags);
	rcu_strict_gp_check_qs();
	if (needwake)
		rcu_gp_kthread_wake();
}

static void rcu_gp_slow(int delay)
{
	if (delay > 0 &&
	    !(rcu_seq_ctr(rcu_state.gp_seq) %
	      (rcu_num_nodes * PER_RCU_NODE_PERIOD * delay)))
		schedule_timeout_idle(delay);
}

static unsigned long sleep_duration;

/* Allow rcutorture to stall the grace-period kthread. */
void rcu_gp_set_torture_wait(int duration)
{
	if (IS_ENABLED(CONFIG_RCU_TORTURE_TEST) && duration > 0)
		WRITE_ONCE(sleep_duration, duration);
}
EXPORT_SYMBOL_GPL(rcu_gp_set_torture_wait);

/* Actually implement the aforementioned wait. */
static void rcu_gp_torture_wait(void)
{
	unsigned long duration;

	if (!IS_ENABLED(CONFIG_RCU_TORTURE_TEST))
		return;
	duration = xchg(&sleep_duration, 0UL);
	if (duration > 0) {
		pr_alert("%s: Waiting %lu jiffies\n", __func__, duration);
		schedule_timeout_idle(duration);
		pr_alert("%s: Wait complete\n", __func__);
	}
}

/*
 * Handler for on_each_cpu() to invoke the target CPU's RCU core
 * processing.
 */
static void rcu_strict_gp_boundary(void *unused)
{
	invoke_rcu_core();
}

/*
 * Initialize a new grace period.  Return false if no grace period required.
 */
static noinline_for_stack bool rcu_gp_init(void)
{
	unsigned long flags;
	unsigned long oldmask;
	unsigned long mask;
	struct rcu_data *rdp;
	struct rcu_node *rnp = rcu_get_root();

	WRITE_ONCE(rcu_state.gp_activity, jiffies);
	raw_spin_lock_irq_rcu_node(rnp);
	if (!READ_ONCE(rcu_state.gp_flags)) {
		/* Spurious wakeup, tell caller to go back to sleep.  */
		raw_spin_unlock_irq_rcu_node(rnp);
		return false;
	}
	WRITE_ONCE(rcu_state.gp_flags, 0); /* Clear all flags: New GP. */

	if (WARN_ON_ONCE(rcu_gp_in_progress())) {
		/*
		 * Grace period already in progress, don't start another.
		 * Not supposed to be able to happen.
		 */
		raw_spin_unlock_irq_rcu_node(rnp);
		return false;
	}

	/* Advance to a new grace period and initialize state. */
	record_gp_stall_check_time();
	/* Record GP times before starting GP, hence rcu_seq_start(). */
	rcu_seq_start(&rcu_state.gp_seq);
	ASSERT_EXCLUSIVE_WRITER(rcu_state.gp_seq);
	trace_rcu_grace_period(rcu_state.name, rcu_state.gp_seq, TPS("start"));
	raw_spin_unlock_irq_rcu_node(rnp);

	/*
	 * Apply per-leaf buffered online and offline operations to
	 * the rcu_node tree. Note that this new grace period need not
	 * wait for subsequent online CPUs, and that RCU hooks in the CPU
	 * offlining path, when combined with checks in this function,
	 * will handle CPUs that are currently going offline or that will
	 * go offline later.  Please also refer to "Hotplug CPU" section
	 * of RCU's Requirements documentation.
	 */
	WRITE_ONCE(rcu_state.gp_state, RCU_GP_ONOFF);
	/* Exclude CPU hotplug operations. */
	rcu_for_each_leaf_node(rnp) {
		local_irq_save(flags);
		arch_spin_lock(&rcu_state.ofl_lock);
		raw_spin_lock_rcu_node(rnp);
		if (rnp->qsmaskinit == rnp->qsmaskinitnext &&
		    !rnp->wait_blkd_tasks) {
			/* Nothing to do on this leaf rcu_node structure. */
			raw_spin_unlock_rcu_node(rnp);
			arch_spin_unlock(&rcu_stant_modules();
	if (irqs_disabled())
		print_irqtrace_events(prev);
	if (IS_ENABLED(CONFIG_DEBUG_PREEMPT)
	    && in_atomic_preempt_off()) {
		pr_err("Preemption disabled at:");
		print_ip_sym(KERN_ERR, preempt_disable_ip);
	}
	if (panic_on_warn)
		panic("scheduling while atomic\n");

	dump_stack();
	add_taint(TAINT_WARN, LOCKDEP_STILL_OK);
}

/*
 * Various schedule()-time debugging checks and statistics:
 */
static inline void schedule_debug(struct task_struct *prev, bool preempt)
{
#ifdef CONFIG_SCHED_STACK_END_CHECK
	if (task_stack_end_corrupted(prev))
		panic("corrupted stack end detected inside scheduler\n");

	if (task_scs_end_corrupted(prev))
		panic("corrupted shadow stack detected inside scheduler\n");
#endif

#ifdef CONFIG_DEBUG_ATOMIC_SLEEP
	if (!preempt && READ_ONCE(prev->__state) && prev->non_block_count) {
		printk(KERN_ERR "BUG: scheduling in a non-blocking section: %s/%d/%i\n",
			prev->comm, prev->pid, prev->non_block_count);
		dump_stack();
		add_taint(TAINT_WARN, LOCKDEP_STILL_OK);
	}
#endif

	if (unlikely(in_atomic_preempt_off())) {
		__schedule_bug(prev);
		preempt_count_set(PREEMPT_DISABLED);
	}
	rcu_sleep_check();
	SCHED_WARN_ON(ct_state() == CONTEXT_USER);

	profile_hit(SCHED_PROFILING, __builtin_return_address(0));

	schedstat_inc(this_rq()->sched_count);
}

static void put_prev_task_balance(struct rq *rq, struct task_struct *prev,
				  struct rq_flags *rf)
{
#ifdef CONFIG_SMP
	const struct sched_class *class;
	/*
	 * We must do the balancing pass before put_prev_task(), such
	 * that when we release the rq->lock the task is in the same
	 * state as before we took rq->lock.
	 *
	 * We can terminate the balance pass as soon as we know there is
	 * a runnable task of @class priority or higher.
	 */
	for_class_range(class, prev->sched_class, &idle_sched_class) {
		if (class->balance(rq, prev, rf))
			break;
	}
#endif

	put_prev_task(rq, prev);
}

/*
 * Pick up the highest-prio task:
 */
static inline struct task_struct *
__pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	const struct sched_class *class;
	struct task_struct *p;

	/*
	 * Optimization: we know that if all tasks are in the fair class we can
	 * call that function directly, but only if the @prev task wasn't of a
	 * higher scheduling class, because otherwise those lose the
	 * opportunity to pull in more work from other CPUs.
	 */
	if (likely(prev->sched_class <= &fair_sched_class &&
		   rq->nr_running == rq->cfs.h_nr_running)) {

		p = pick_next_task_fair(rq, prev, rf);
		if (unlikely(p == RETRY_TASK))
			goto restart;

		/* Assume the next prioritized class is idle_sched_class */
		if (!p) {
			put_prev_task(rq, prev);
			p = pick_next_task_idle(rq);
		}

		return p;
	}

restart:
	put_prev_task_balance(rq, prev, rf);

	for_each_class(class) {
		p = class->pick_next_task(rq);
		if (p)
			return p;
	}

	BUG(); /* The idle class should always have a runnable task. */
}

#ifdef CONFIG_SCHED_CORE
static inline bool is_task_rq_idle(struct task_struct *t)
{
	return (task_rq(t)->idle == t);
}

static inline bool cookie_equals(struct task_struct *a, unsigned long cookie)
{
	return is_task_rq_idle(a) || (a->core_cookie == cookie);
}

static inline bool cookie_match(struct task_struct *a, struct task_struct *b)
{
	if (is_task_rq_idle(a) || is_task_rq_idle(b))
		return true;

	return a->core_cookie == b->core_cookie;
}

static inline struct task_struct *pick_task(struct rq *rq)
{
	const struct sched_class *class;
	struct task_struct *p;

	for_each_class(class) {
		p = class->pick_task(rq);
		if (p)
			return p;
	}

	BUG(); /* The idle class should always have a runnable task. */
}

extern void task_vruntime_update(struct rq *rq, struct task_struct *p, bool in_fi);

static void queue_core_balance(struct rq *rq);

static struct task_struct *
pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	struct task_struct *next, *p, *max = NULL;
	const struct cpumask *smt_mask;
	bool fi_before = false;
	bool core_clock_updated = (rq == rq->core);
	unsigned long cookie;
	int i, cpu, occ = 0;
	struct rq *rq_i;
	bool need_sync;

	if (!sched_core_enabled(rq))
		return __pick_next_task(rq, prev, rf);

	cpu = cpu_of(rq);

	/* Stopper task is switching into idle, no need core-wide selection. */
	if (cpu_is_offline(cpu)) {
		/*
		 * Reset core_pick so that we don't enter the fastpath when
		 * coming online. core_pick would already be migrated to
		 * another cpu during offline.
		 */
		rq->core_pick = NULL;
		return __pick_next_task(rq, prev, rf);
	}

	/*
	 * If there were no {en,de}queues since we picked (IOW, the task
	 * pointers are all still valid), and we haven't scheduled the last
	 * pick yet, do so now.
	 *
	 * rq->core_pick can be NULL if no selection was made for a CPU because
	 * it was either offline or went offline during a sibling's core-wide
	 * selection. In this case, do a core-wide selection.
	 */
	if (rq->core->core_pick_seq == rq->core->core_task_seq &&
	    rq->core->core_pick_seq != rq->core_sched_seq &&
	    rq->core_pick) {
		WRITE_ONCE(rq->core_sched_seq, rq->core->core_pick_seq);

		next = rq->core_pick;
		if (next != prev) {
			put_prev_task(rq, prev);
			set_next_task(rq, next);
		}

		rq->core_pick = NULL;
		goto out;
	}

	put_prev_task_balance(rq, prev, rf);

	smt_mask = cpu_smt_mask(cpu);
	need_sync = !!rq->core->core_cookie;

	/* reset state */
	rq->core->core_cookie = 0UL;
	if (rq->core->core_forceidle_count) {
		if (!core_clock_updated) {
			update_rq_clock(rq->core);
			core_clock_updated = true;
		}
		sched_core_account_forceidle(rq);
		/* reset after accounting force idle */
		rq->core->core_forceidle_start = 0;
		rq->core->core_forceidle_count = 0;
		rq->core->core_forceidle_occupation = 0;
		need_sync = true;
		fi_before = true;
	}

	/*
	 * core->core_task_seq, core->core_pick_seq, rq->core_sched_seq
	 *
	 * @task_seq guards the task state ({en,de}queues)
	 * @pick_seq is the @task_seq we did a selection on
	 * @sched_seq is the @pick_seq we scheduled
	 *
	 * However, preemptions can cause multiple picks on the same task set.
	 * 'Fix' this by also increasing @task_seq for every pick.
	 */
	rq->core->core_task_seq++;

	/*
	 * Optimize for common case where this CPU has no cookies
	 * and there are no cookied tasks running on siblings.
	 */
	if (!need_sync) {
		next = pick_task(rq);
		if (!next->core_cookie) {
			rq->core_pick = NULL;
			/*
			 * For robustness, update the min_vruntime_fi for
			 * unconstrained picks as well.
			 */
			WARN_ON_ONCE(fi_before);
			task_vruntime_update(rq, next, false);
			goto out_set_next;
		}
	}

	/*
	 * For each thread: do the regular task pick and find the max prio task
	 * amongst them.
	 *
	 * Tie-break prio towards the current CPU
	 */
	for_each_cpu_wrap(i, smt_mask, cpu) {
		rq_i = cpu_rq(i);

		/*
		 * Current cpu always has its clock updated on entrance to
		 * pick_next_task(). If the current cpu is not the core,
		 * the core may also have been updated above.
		 */
		if (i != cpu && (rq_i != rq->core || !core_clock_updated))
			update_rq_clock(rq_i);

		p = rq_i->core_pick = pick_task(rq_i);
		if (!max || prio_less(max, p, fi_before))
			max = p;
	}

	cookie = rq->core->core_cookie = max->core_cookie;

	/*
	 * For each thread: try and find a runnable task that matches @max or
	 * force idle.
	 */
	for_each_cpu(i, smt_mask) {
		rq_i = cpu_rq(i);
		p = rq_i->core_pick;

		if (!cookie_equals(p, cookie)) {
			p = NULL;
			if (cookie)
				p = sched_core_find(rq_i, cookie);
			if (!p)
				p = idle_sched_class.pick_task(rq_i);
		}

		rq_i->core_pick = p;

		if (p == rq_i->idle) {
			if (rq_i->nr_running) {
				rq->core->core_forceidle_count++;
				if (!fi_before)
					rq->core->core_forceidle_seq++;
			}
		} else {
			occ++;
		}
	}

	if (schedstat_enabled() && rq->core->core_forceidle_count) {
		rq->core->core_forceidle_start = rq_clock(rq->core);
		rq->core->core_forceidle_occupation = occ;
	}

	rq->core->core_pick_seq = rq->core->core_task_seq;
	next = rq->core_pick;
	rq->core_sched_seq = rq->core->core_pick_seq;

	/* Something should have been selected for current CPU */
	WARN_ON_ONCE(!next);

	/*
	 * Reschedule siblings
	 *
	 * NOTE: L1TF -- at this point we're no longer running the old task and
	 * sending an IPI (below) ensures the sibling will no longer be running
	 * their task. This ensures there is no inter-sibling overlap between
	 * non-matching user state.
	 */
	for_each_cpu(i, smt_mask) {
		rq_i = cpu_rq(i);

		/*
		 * An online sibling might have gone offline before a task
		 * could be picked for it, or it might be offline but later
		 * happen to come online, but its too late and nothing was
		 * picked for it.  That's Ok - it will pick tasks for itself,
		 * so ignore it.
		 */
		if (!rq_i->core_pick)
			continue;

		/*
		 * Update for new !FI->FI transitions, or if continuing to be in !FI:
		 * fi_before     fi      update?
		 *  0            0       1
		 *  0            1       1
		 *  1            0       1
		 *  1            1       0
		 */
		if (!(fi_before && rq->core->core_forceidle_count))
			task_vruntime_update(rq_i, rq_i->core_pick, !!rq->core->core_forceidle_count);

		rq_i->core_pick->core_occupation = occ;

		if (i == cpu) {
			rq_i->core_pick = NULL;
			continue;
		}

		/* Did we break L1TF mitigation requirements? */
		WARN_ON_ONCE(!cookie_match(next, rq_i->core_pick));

		if (rq_i->curr == rq_i->core_pick) {
			rq_i->core_pick = NULL;
			continue;
		}

		resched_curr(rq_i);
	}

out_set_next:
	set_next_task(rq, next);
out:
	if (rq->core->core_forceidle_count && next == rq->idle)
		queue_core_balance(rq);

	return next;
}

static bool try_steal_cookie(int this, int that)
{
	struct rq *dst = cpu_rq(this), *src = cpu_rq(that);
	struct task_struct *p;
	unsigned long cookie;
	bool success = false;

	local_irq_disable();
	double_rq_lock(dst, src);

	cookie = dst->core->core_cookie;
	if (!cookie)
		goto unlock;

	if (dst->curr != dst->idle)
		goto unlock;

	p = sched_core_find(src, cookie);
	if (p == src->idle)
		goto unlock;

	do {
		if (p == src->core_pick || p == src->curr)
			goto next;

		if (!is_cpu_allowed(p, this))
			goto next;

		if (p->core_occupation > dst->idle->core_occupation)
			goto next;

		deactivate_task(src, p, 0);
		set_task_cpu(p, this);
		activate_task(dst, p, 0);

		resched_curr(dst);

		success = true;
		break;

next:
		p = sched_core_next(p, cookie);
	} while (p);

unlock:
	double_rq_unlock(dst, src);
	local_irq_enable();

	return success;
}

static bool steal_cookie_task(int cpu, struct sched_domain *sd)
{
	int i;

	for_each_cpu_wrap(i, sched_domain_span(sd), cpu) {
		if (i == cpu)
			continue;

		if (need_resched())
			break;

		if (try_steal_cookie(cpu, i))
			return true;
	}

	return false;
}

static void sched_core_balance(struct rq *rq)
{
	struct sched_domain *sd;
	int cpu = cpu_of(rq);

	preempt_disable();
	rcu_read_lock();
	raw_spin_rq_unlock_irq(rq);
	for_each_domain(cpu, sd) {
		if (need_resched())
			break;

		if (steal_cookie_task(cpu, sd))
			break;
	}
	raw_spin_rq_lock_irq(rq);
	rcu_read_unlock();
	preempt_enable();
}

static DEFINE_PER_CPU(struct callback_head, core_balance_head);

static void queue_core_balance(struct rq *rq)
{
	if (!sched_core_enabled(rq))
		return;

	if (!rq->core->core_cookie)
		return;

	if (!rq->nr_running) /* not forced idle */
		return;

	queue_balance_callback(rq, &per_cpu(core_balance_head, rq->cpu), sched_core_balance);
}

static void sched_core_cpu_starting(unsigned int cpu)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	struct rq *rq = cpu_rq(cpu), *core_rq = NULL;
	unsigned long flags;
	int t;

	sched_core_lock(cpu, &flags);

	WARN_ON_ONCE(rq->core != rq);

	/* if we're the first, we'll be our own leader */
	if (cpumask_weight(smt_mask) == 1)
		goto unlock;

	/* find the leader */
	for_each_cpu(t, smt_mask) {
		if (t == cpu)
			continue;
		rq = cpu_rq(t);
		if (rq->core == rq) {
			core_rq = rq;
			break;
		}
	}

	if (WARN_ON_ONCE(!core_rq)) /* whoopsie */
		goto unlock;

	/* install and validate core_rq */
	for_each_cpu(t, smt_mask) {
		rq = cpu_rq(t);

		if (t == cpu)
			rq->core = core_rq;

		WARN_ON_ONCE(rq->core != core_rq);
	}

unlock:
	sched_core_unlock(cpu, &flags);
}

static void sched_core_cpu_deactivate(unsigned int cpu)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	struct rq *rq = cpu_rq(cpu), *core_rq = NULL;
	unsigned long flags;
	int t;

	sched_core_lock(cpu, &flags);

	/* if we're the last man standing, nothing to do */
	if (cpumask_weight(smt_mask) == 1) {
		WARN_ON_ONCE(rq->core != rq);
		goto unlock;
	}

	/* if we're not the leader, nothing to do */
	if (rq->core != rq)
		goto unlock;

	/* find a new leader */
	for_each_cpu(t, smt_mask) {
		if (t == cpu)
			continue;
		core_rq = cpu_rq(t);
		break;
	}

	if (WARN_ON_ONCE(!core_rq)) /* impossible */
		goto unlock;

	/* copy the shared state to the new leader */
	core_rq->core_task_seq             = rq->core_task_seq;
	core_rq->core_pick_seq             = rq->core_pick_seq;
	core_rq->core_cookie               = rq->core_cookie;
	core_rq->core_forceidle_count      = rq->core_forceidle_count;
	core_rq->core_forceidle_seq        = rq->core_forceidle_seq;
	core_rq->core_forceidle_occupation = rq->core_forceidle_occupation;

	/*
	 * Accounting edge for forced idle is handled in pick_next_task().
	 * Don't need another one here, since the hotplug thread shouldn't
	 * have a cookie.
	 */
	core_rq->core_forceidle_start = 0;

	/* install new leader */
	for_each_cpu(t, smt_mask) {
		rq = cpu_rq(t);
		rq->core = core_rq;
	}

unlock:
	sched_core_unlock(cpu, &flags);
}

static inline void sched_core_cpu_dying(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (rq->core != rq)
		rq->core = rq;
}

#else /* !CONFIG_SCHED_CORE */

static inline void sched_core_cpu_starting(unsigned int cpu) {}
static inline void sched_core_cpu_deactivate(unsigned int cpu) {}
static inline void sched_core_cpu_dying(unsigned int cpu) {}

static struct task_struct *
pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	return __pick_next_task(rq, prev, rf);
}

#endif /* CONFIG_SCHED_CORE */

/*
 * Constants for the sched_mode argument of __schedule().
 *
 * The mode argument allows RT enabled kernels to differentiate a
 * preemption from blocking on an 'sleeping' spin/rwlock. Note that
 * SM_MASK_PREEMPT for !RT has all bits set, which allows the compiler to
 * optimize the AND operation out and just check for zero.
 */
#define SM_NONE			0x0
#define SM_PREEMPT		0x1
#define SM_RTLOCK_WAIT		0x2

#ifndef CONFIG_PREEMPT_RT
# define SM_MASK_PREEMPT	(~0U)
#else
# define SM_MASK_PREEMPT	SM_PREEMPT
#endif

/*
 * __schedule() is the main scheduler function.
 *
 * The main means of driving the scheduler and thus entering this function are:
 *
 *   1. Explicit blocking: mutex, semaphore, waitqueue, etc.
 *
 *   2. TIF_NEED_RESCHED flag is checked on interrupt and userspace return
 *      paths. For example, see arch/x86/entry_64.S.
 *
 *      To drive preemption between tasks, the scheduler sets the flag in timer
 *      interrupt handler scheduler_tick().
 *
 *   3. Wakeups don't really cause entry into schedule(). They add a
 *      task to the run-queue and that's it.
 *
 *      Now, if the new task added to the run-queue preempts the current
 *      task, then the wakeup sets TIF_NEED_RESCHED and schedule() gets
 *      called on the nearest possible occasion:
 *
 *       - If the kernel is preemptible (CONFIG_PREEMPTION=y):
 *
 *         - in syscall or exception context, at the next outmost
 *           preempt_enable(). (this might be as soon as the wake_up()'s
 *           spin_unlock()!)
 *
 *         - in IRQ context, return from interrupt-handler to
 *           preemptible context
 *
 *       - If the kernel is not preemptible (CONFIG_PREEMPTION is not set)
 *         then at the next:
 *
 *          - cond_resched() call
 *          - explicit schedule() call
 *          - return from syscall or exception to user-space
 *          - return from interrupt-handler to user-space
 *
 * WARNING: must be called with preemption disabled!
 */
static void __sched notrace __schedule(unsigned int sched_mode)
{
	struct task_struct *prev, *next;
	unsigned long *switch_count;
	unsigned long prev_state;
	struct rq_flags rf;
	struct rq *rq;
	int cpu;

	cpu = smp_processor_id();
	rq = cpu_rq(cpu);
	prev = rq->curr;

	schedule_debug(prev, !!sched_mode);

	if (sched_feat(HRTICK) || sched_feat(HRTICK_DL))
		hrtick_clear(rq);

	local_irq_disable();
	rcu_note_context_switch(!!sched_mode);

	/*
	 * Make sure that signal_pending_state()->signal_pending() below
	 * can't be reordered with __set_current_state(TASK_INTERRUPTIBLE)
	 * done by the caller to avoid the race with signal_wake_up():
	 *
	 * __set_current_state(@state)		signal_wake_up()
	 * schedule()				  set_tsk_thread_flag(p, TIF_SIGPENDING)
	 *					  wake_up_state(p, state)
	 *   LOCK rq->lock			    LOCK p->pi_state
	 *   smp_mb__after_spinlock()		    smp_mb__after_spinlock()
	 *     if (signal_pending_state())	    if (p->state & @state)
	 *
	 * Also, the membarrier system call requires a full memory barrier
	 * after coming from user-space, before storing to rq->curr.
	 */
	rq_lock(rq, &rf);
	smp_mb__after_spinlock();

	/* Promote REQ to ACT */
	rq->clock_update_flags <<= 1;
	update_rq_clock(rq);

	switch_count = &prev->nivcsw;

	/*
	 * We must load prev->state once (task_struct::state is volatile), such
	 * that:
	 *
	 *  - we form a control dependency vs deactivate_task() below.
	 *  - ptrace_{,un}freeze_traced() can change ->state underneath us.
	 */
	prev_state = READ_ONCE(prev->__state);
	if (!(sched_mode & SM_MASK_PREEMPT) && prev_state) {
		if (signal_pending_state(prev_state, prev)) {
			WRITE_ONCE(prev->__state, TASK_RUNNING);
		} else {
			prev->sched_contributes_to_load =
				(prev_state & TASK_UNINTERRUPTIBLE) &&
				!(prev_state & TASK_NOLOAD) &&
				!(prev->flags & PF_FROZEN);

			if (prev->sched_contributes_to_load)
				rq->nr_uninterruptible++;

			/*
			 * __schedule()			ttwu()
			 *   prev_state = prev->state;    if (p->on_rq && ...)
			 *   if (prev_state)		    goto out;
			 *     p->on_rq = 0;		  smp_acquire__after_ctrl_dep();
			 *				  p->state = TASK_WAKING
			 *
			 * Where __schedule() and ttwu() have matching control dependencies.
			 *
			 * After this, schedule() must not care about p->state any more.
			 */
			deactivate_task(rq, prev, DEQUEUE_SLEEP | DEQUEUE_NOCLOCK);

			if (prev->in_iowait) {
				atomic_inc(&rq->nr_iowait);
				delayacct_blkio_start();
			}
		}
		switch_count = &prev->nvcsw;
	}

	next = pick_next_task(rq, prev, &rf);
	clear_tsk_need_resched(prev);
	clear_preempt_need_resched();
#ifdef CONFIG_SCHED_DEBUG
	rq->last_seen_need_resched_ns = 0;
#endif

	if (likely(prev != next)) {
		rq->nr_switches++;
		/*
		 * RCU users of rcu_dereference(rq->curr) may not see
		 * changes to task_struct made by pick_next_task().
		 */
		RCU_INIT_POINTER(rq->curr, next);
		/*
		 * The membarrier system call requires each architecture
		 * to have a full memory barrier after updating
		 * rq->curr, before returning to user-space.
		 *
		 * Here are the schemes providing that barrier on the
		 * various architectures:
		 * - mm ? switch_mm() : mmdrop() for x86, s390, sparc, PowerPC.
		 *   switch_mm() rely on membarrier_arch_switch_mm() on PowerPC.
		 * - finish_lock_switch() for weakly-ordered
		 *   architectures where spin_unlock is a full barrier,
		 * - switch_to() for arm64 (weakly-ordered, spin_unlock
		 *   is a RELEASE barrier),
		 */
		++*switch_count;

		migrate_disable_switch(rq, prev);
		psi_sched_switch(prev, next, !task_on_rq_queued(prev));

		trace_sched_switch(sched_mode & SM_MASK_PREEMPT, prev, next, prev_state);

		/* Also unlocks the rq: */
		rq = context_switch(rq, prev, next, &rf);
	} else {
		rq->clock_update_flags &= ~(RQCF_ACT_SKIP|RQCF_REQ_SKIP);

		rq_unpin_lock(rq, &rf);
		__balance_callbacks(rq);
		raw_spin_rq_unlock_irq(rq);
	}
}

void __noreturn do_task_dead(void)
{
	/* Causes final put_task_struct in finish_task_switch(): */
	set_special_state(TASK_DEAD);

	/* Tell freezer to ignore us: */
	current->flags |= PF_NOFREEZE;

	__schedule(SM_NONE);
	BUG();

	/* Avoid "noreturn function does return" - but don't continue if BUG() is a NOP: */
	for (;;)
		cpu_relax();
}

static inline void sched_submit_work(struct task_struct *tsk)
{
	unsigned int task_flags;

	if (task_is_running(tsk))
		return;

	task_flags = tsk->flags;
	/*
	 * If a worker goes to sleep, notify and ask workqueue whether it
	 * wants to wake up a task to maintain concurrency.
	 */
	if (task_flags & (PF_WQ_WORKER | PF_IO_WORKER)) {
		if (task_flags & PF_WQ_WORKER)
			wq_worker_sleeping(tsk);
		else
			io_wq_worker_sleeping(tsk);
	}

	if (tsk_is_pi_blocked(tsk))
		return;

	/*
	 * If we are going to sleep and we have plugged IO queued,
	 * make sure to submit it to avoid deadlocks.
	 */
	blk_flush_plug(tsk->plug, true);
}

static void sched_update_worker(struct task_struct *tsk)
{
	if (tsk->flags & (PF_WQ_WORKER | PF_IO_WORKER)) {
		if (tsk->flags & PF_WQ_WORKER)
			wq_worker_running(tsk);
		else
			io_wq_worker_running(tsk);
	}
}

asmlinkage __visible void __sched schedule(void)
{
	struct task_struct *tsk = current;

	sched_submit_work(tsk);
	do {
		preempt_disable();
		__schedule(SM_NONE);
		sched_preempt_enable_no_resched();
	} while (need_resched());
	sched_update_worker(tsk);
}
EXPORT_SYMBOL(schedule);

/*
 * synchronize_rcu_tasks() makes sure that no task is stuck in preempted
 * state (have scheduled out non-voluntarily) by making sure that all
 * tasks have either left the run queue or have gone into user space.
 * As idle tasks do not do either, they must not ever be preempted
 * (schedule out non-voluntarily).
 *
 * schedule_idle() is similar to schedule_preempt_disable() except that it
 * never enables preemption because it does not call sched_submit_work().
 */
void __sched schedule_idle(void)
{
	/*
	 * As this skips calling sched_submit_work(), which the idle task does
	 * regardless because that function is a nop when the task is in a
	 * TASK_RUNNING state, make sure this isn't used someplace that the
	 * current task can be in any other state. Note, idle is always in the
	 * TASK_RUNNING state.
	 */
	WARN_ON_ONCE(current->__state);
	do {
		__schedule(SM_NONE);
	} while (need_resched());
}

#if defined(CONFIG_CONTEXT_TRACKING) && !defined(CONFIG_HAVE_CONTEXT_TRACKING_OFFSTACK)
asmlinkage __visible void __sched schedule_user(void)
{
	/*
	 * If we come here after a random call to set_need_resched(),
	 * or we have been woken up remotely but the IPI has not yet arrived,
	 * we haven't yet exited the RCU idle mode. Do it here manually until
	 * we find a better solution.
	 *
	 * NB: There are buggy callers of this function.  Ideally we
	 * should warn if prev_state != CONTEXT_USER, but that will trigger
	 * too frequently to make sense yet.
	 */
	enum ctx_state prev_state = exception_enter();
	schedule();
	exception_exit(prev_state);
}
#endif

/**
 * schedule_preempt_disabled - called with preemption disabled
 *
 * Returns with preemption disabled. Note: preempt_count must be 1
 */
void __sched schedule_preempt_disabled(void)
{
	sched_preempt_enable_no_resched();
	schedule();
	preempt_disable();
}

#ifdef CONFIG_PREEMPT_RT
void __sched notrace schedule_rtlock(void)
{
	do {
		preempt_disable();
		__schedule(SM_RTLOCK_WAIT);
		sched_preempt_enable_no_resched();
	} while (need_resched());
}
NOKPROBE_SYMBOL(schedule_rtlock);
#endif

static void __sched notrace preempt_schedule_common(void)
{
	do {
		/*
		 * Because the function tracer can trace preempt_count_sub()
		 * and it also uses preempt_enable/disable_notrace(), if
		 * NEED_RESCHED is set, the preempt_enable_notrace() called
		 * by the function tracer will call this function again and
		 * cause infinite recursion.
		 *
		 * Preemption must be disabled here before the function
		 * tracer can trace. Break up preempt_disable() into two
		 * calls. One to disable preemption without fear of being
		 * traced. The other to still record the preemption latency,
		 * which can also be traced by the function tracer.
		 */
		preempt_disable_notrace();
		preempt_latency_start(1);
		__schedule(SM_PREEMPT);
		preempt_latency_stop(1);
		preempt_enable_no_resched_notrace();

		/*
		 * Check again in case we missed a preemption opportunity
		 * between schedule and now.
		 */
	} while (need_resched());
}

#ifdef CONFIG_PREEMPTION
/*
 * This is the entry point to schedule() from in-kernel preemption
 * off of preempt_enable.
 */
asmlinkage __visible void __sched notrace preempt_schedule(void)
{
	/*
	 * If there is a non-zero preempt_count or interrupts are disabled,
	 * we do not want to preempt the current task. Just return..
	 */
	if (likely(!preemptible()))
		return;
	preempt_schedule_common();
}
NOKPROBE_SYMBOL(preempt_schedule);
EXPORT_SYMBOL(preempt_schedule);

#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#ifndef preempt_schedule_dynamic_enabled
#define preempt_schedule_dynamic_enabled	preempt_schedule
#define preempt_schedule_dynamic_disabled	NULL
#endif
DEFINE_STATIC_CALL(preempt_schedule, preempt_schedule_dynamic_enabled);
EXPORT_STATIC_CALL_TRAMP(preempt_schedule);
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
static DEFINE_STATIC_KEY_TRUE(sk_dynamic_preempt_schedule);
void __sched notrace dynamic_preempt_schedule(void)
{
	if (!static_branch_unlikely(&sk_dynamic_preempt_schedule))
		return;
	preempt_schedule();
}
NOKPROBE_SYMBOL(dynamic_preempt_schedule);
EXPORT_SYMBOL(dynamic_preempt_schedule);
#endif
#endif

/**
 * preempt_schedule_notrace - preempt_schedule called by tracing
 *
 * The tracing infrastructure uses preempt_enable_notrace to prevent
 * recursion and tracing preempt enabling caused by the tracing
 * infrastructure itself. But as tracing can happen in areas coming
 * from userspace or just about to enter userspace, a preempt enable
 * can occur before user_exit() is called. This will cause the scheduler
 * to be called when the system is still in usermode.
 *
 * To prevent this, the preempt_enable_notrace will use this function
 * instead of preempt_schedule() to exit user context if needed before
 * calling the scheduler.
 */
asmlinkage __visible void __sched notrace preempt_schedule_notrace(void)
{
	enum ctx_state prev_ctx;

	if (likely(!preemptible()))
		return;

	do {
		/*
		 * Because the function tracer can trace preempt_count_sub()
		 * and it also uses preempt_enable/disable_notrace(), if
		 * NEED_RESCHED is set, the preempt_enable_notrace() called
		 * by the function tracer will call this function again and
		 * cause infinite recursion.
		 *
		 * Preemption must be disabled here before the function
		 * tracer can trace. Break up preempt_disable() into two
		 * calls. One to disable preemption without fear of being
		 * traced. The other to still record the preemption latency,
		 * which can also be traced by the function tracer.
		 */
		preempt_disable_notrace();
		preempt_latency_start(1);
		/*
		 * Needs preempt disabled in case user_exit() is traced
		 * and the tracer calls preempt_enable_notrace() causing
		 * an infinite recursion.
		 */
		prev_ctx = exception_enter();
		__schedule(SM_PREEMPT);
		exception_exit(prev_ctx);

		preempt_latency_stop(1);
		preempt_enable_no_resched_notrace();
	} while (need_resched());
}
EXPORT_SYMBOL_GPL(preempt_schedule_notrace);

#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#ifndef preempt_schedule_notrace_dynamic_enabled
#define preempt_schedule_notrace_dynamic_enabled	preempt_schedule_notrace
#define preempt_schedule_notrace_dynamic_disabled	NULL
#endif
DEFINE_STATIC_CALL(preempt_schedule_notrace, preempt_schedule_notrace_dynamic_enabled);
EXPORT_STATIC_CALL_TRAMP(preempt_schedule_notrace);
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
static DEFINE_STATIC_KEY_TRUE(sk_dynamic_preempt_schedule_notrace);
void __sched notrace dynamic_preempt_schedule_notrace(void)
{
	if (!static_branch_unlikely(&sk_dynamic_preempt_schedule_notrace))
		return;
	preempt_schedule_notrace();
}
NOKPROBE_SYMBOL(dynamic_preempt_schedule_notrace);
EXPORT_SYMBOL(dynamic_preempt_schedule_notrace);
#endif
#endif

#endif /* CONFIG_PREEMPTION */

/*
 * This is the entry point to schedule() from kernel preemption
 * off of irq context.
 * Note, that this is called and return with irqs disabled. This will
 * protect us against recursive calling from irq.
 */
asmlinkage __visible void __sched preempt_schedule_irq(void)
{
	enum ctx_state prev_state;

	/* Catch callers which need to be fixed */
	BUG_ON(preempt_count() || !irqs_disabled());

	prev_state = exception_enter();

	do {
		preempt_disable();
		local_irq_enable();
		__schedule(SM_PREEMPT);
		local_irq_disable();
		sched_preempt_enable_no_resched();
	} while (need_resched());

	exception_exit(prev_state);
}

int default_wake_function(wait_queue_entry_t *curr, unsigned mode, int wake_flags,
			  void *key)
{
	WARN_ON_ONCE(IS_ENABLED(CONFIG_SCHED_DEBUG) && wake_flags & ~WF_SYNC);
	return try_to_wake_up(curr->private, mode, wake_flags);
}
EXPORT_SYMBOL(default_wake_function);

static void __setscheduler_prio(struct task_struct *p, int prio)
{
	if (dl_prio(prio))
		p->sched_class = &dl_sched_class;
	else if (rt_prio(prio))
		p->sched_class = &rt_sched_class;
	else
		p->sched_class = &fair_sched_class;

	p->prio = prio;
}

#ifdef CONFIG_RT_MUTEXES

static inline int __rt_effective_prio(struct task_struct *pi_task, int prio)
{
	if (pi_task)
		prio = min(prio, pi_task->prio);

	return prio;
}

static inline int rt_effective_prio(struct task_struct *p, int prio)
{
	struct task_struct *pi_task = rt_mutex_get_top_task(p);

	return __rt_effective_prio(pi_task, prio);
}

/*
 * rt_mutex_setprio - set the current priority of a task
 * @p: task to boost
 * @pi_task: donor task
 *
 * This function changes the 'effective' priority of a task. It does
 * not touch ->normal_prio like __setscheduler().
 *
 * Used by the rt_mutex code to implement priority inheritance
 * logic. Call site only calls if the priority of the task changed.
 */
void rt_mutex_setprio(struct task_struct *p, struct task_struct *pi_task)
{
	int prio, oldprio, queued, running, queue_flag =
		DEQUEUE_SAVE | DEQUEUE_MOVE | DEQUEUE_NOCLOCK;
	const struct sched_class *prev_class;
	struct rq_flags rf;
	struct rq *rq;

	/* XXX used to be waiter->prio, not waiter->task->prio */
	prio = __rt_effective_prio(pi_task, p->normal_prio);

	/*
	 * If nothing changed; bail early.
	 */
	if (p->pi_top_task == pi_task && prio == p->prio && !dl_prio(prio))
		return;

	rq = __task_rq_lock(p, &rf);
	update_rq_clock(rq);
	/*
	 * Set under pi_lock && rq->lock, such that the value can be used under
	 * either lock.
	 *
	 * Note that there is loads of tricky to make this pointer cache work
	 * right. rt_mutex_slowunlock()+rt_mutex_postunlock() work together to
	 * ensure a task is de-boosted (pi_task is set to NULL) before the
	 * task is allowed to run again (and can exit). This ensures the pointer
	 * points to a blocked task -- which guarantees the task is present.
	 */
	p->pi_top_task = pi_task;

	/*
	 * For FIFO/RR we only need to set prio, if that matches we're done.
	 */
	if (prio == p->prio && !dl_prio(prio))
		goto out_unlock;

	/*
	 * Idle task boosting is a nono in general. There is one
	 * exception, when PREEMPT_RT and NOHZ is active:
	 *
	 * The idle task calls get_next_timer_interrupt() and holds
	 * the timer wheel base->lock on the CPU and another CPU wants
	 * to access the timer (probably to cancel it). We can safely
	 * ignore the boosting request, as the idle CPU runs this code
	 * with interrupts disabled and will complete the lock
	 * protected section without being interrupted. So there is no
	 * real need to boost.
	 */
	if (unlikely(p == rq->idle)) {
		WARN_ON(p != rq->curr);
		WARN_ON(p->pi_blocked_on);
		goto out_unlock;
	}

	trace_sched_pi_setprio(p, pi_task);
	oldprio = p->prio;

	if (oldprio == prio)
		queue_flag &= ~DEQUEUE_MOVE;

	prev_class = p->sched_class;
	queued = task_on_rq_queued(p);
	running = task_current(rq, p);
	if (queued)
		dequeue_task(rq, p, queue_flag);
	if (running)
		put_prev_task(rq, p);

	/*
	 * Boosting condition are:
	 * 1. -rt task is running and holds mutex A
	 *      --> -dl task blocks on mutex A
	 *
	 * 2. -dl task is running and holds mutex A
	 *      --> -dl task blocks on mutex A and could preempt the
	 *          running task
	 */
	if (dl_prio(prio)) {
		if (!dl_prio(p->normal_prio) ||
		    (pi_task && dl_prio(pi_task->prio) &&
		     dl_entity_preempt(&pi_task->dl, &p->dl))) {
			p->dl.pi_se = pi_task->dl.pi_se;
			queue_flag |= ENQUEUE_REPLENISH;
		} else {
			p->dl.pi_se = &p->dl;
		}
	} else if (rt_prio(prio)) /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#ifndef _CX23885_REG_H_
#define _CX23885_REG_H_

/*
Address Map
0x00000000 -> 0x00009000   TX SRAM  (Fifos)
0x00010000 -> 0x00013c00   RX SRAM  CMDS + CDT

EACH CMDS struct is 0x80 bytes long

DMAx_PTR1 = 0x03040 address of first cluster
DMAx_PTR2 = 0x10600 address of the CDT
DMAx_CNT1 = cluster size in (bytes >> 4) -1
DMAx_CNT2 = total cdt size for all entries >> 3

Cluster Descriptor entry = 4 DWORDS
 DWORD 0 -> ptr to cluster
 DWORD 1 Reserved
 DWORD 2 Reserved
 DWORD 3 Reserved

Channel manager Data Structure entry = 20 DWORD
  0  IntialProgramCounterLow
  1  IntialProgramCounterHigh
  2  ClusterDescriptorTableBase
  3  ClusterDescriptorTableSize
  4  InstructionQueueBase
  5  InstructionQueueSize
...  Reserved
 19  Reserved
*/

/* Risc Instructions */
#define RISC_CNT_INC		 0x00010000
#define RISC_CNT_RESET		 0x00030000
#define RISC_IRQ1		 0x01000000
#define RISC_IRQ2		 0x02000000
#define RISC_EOL		 0x04000000
#define RISC_SOL		 0x08000000
#define RISC_WRITE		 0x10000000
#define RISC_SKIP		 0x20000000
#define RISC_JUMP		 0x70000000
#define RISC_SYNC		 0x80000000
#define RISC_RESYNC		 0x80008000
#define RISC_READ		 0x90000000
#define RISC_WRITERM		 0xB0000000
#define RISC_WRITECM		 0xC0000000
#define RISC_WRITECR		 0xD0000000
#define RISC_WRITEC		 0x50000000
#define RISC_READC		 0xA0000000


/* Audio and Video Core */
#define HOST_REG1		0x00000000
#define HOST_REG2		0x00000001
#define HOST_REG3		0x00000002

/* Chip Configuration Registers */
#define CHIP_CTRL		0x00000100
#define AFE_CTRL		0x00000104
#define VID_PLL_INT_POST	0x00000108
#define VID_PLL_FRAC		0x0000010C
#define AUX_PLL_INT_POST	0x00000110
#define AUX_PLL_FRAC		0x00000114
#define SYS_PLL_INT_POST	0x00000118
#define SYS_PLL_FRAC		0x0000011C
#define PIN_CTRL		0x00000120
#define AUD_IO_CTRL		0x00000124
#define AUD_LOCK1		0x00000128
#define AUD_LOCK2		0x0000012C
#define POWER_CTRL		0x00000130
#define AFE_DIAG_CTRL1		0x00000134
#define AFE_DIAG_CTRL3		0x0000013C
#define PLL_DIAG_CTRL		0x00000140
#define AFE_CLK_OUT_CTRL	0x00000144
#define DLL1_DIAG_CTRL		0x0000015C

/* GPIO[23:19] Output Enable */
#define GPIO2_OUT_EN_REG	0x00000160
/* GPIO[23:19] Data Registers */
#define GPIO2			0x00000164

#define IFADC_CTRL		0x00000180

/* Infrared Remote Registers */
#define IR_CNTRL_REG	0x00000200
#define IR_TXCLK_REG	0x00000204
#define IR_RXCLK_REG	0x00000208
#define IR_CDUTY_REG	0x0000020C
#define IR_STAT_REG	0x00000210
#define IR_IRQEN_REG	0x00000214
#define IR_FILTR_REG	0x00000218
#define IR_FIFO_REG	0x0000023C

/* Video Decoder Registers */
#define MODE_CTRL		0x00000400
#define OUT_CTRL1		0x00000404
#define OUT_CTRL2		0x00000408
#define GEN_STAT		0x0000040C
#define INT_STAT_MASK		0x00000410
#define LUMA_CTRL		0x00000414
#define HSCALE_CTRL		0x00000418
#define VSCALE_CTRL		0x0000041C
#define CHROMA_CTRL		0x00000420
#define VBI_LINE_CTRL1		0x00000424
#define VBI_LINE_CTRL2		0x00000428
#define VBI_LINE_CTRL3		0x0000042C
#define VBI_LINE_CTRL4		0x00000430
#define VBI_LINE_CTRL5		0x00000434
#define VBI_FC_CFG		0x00000438
#define VBI_MISC_CFG1		0x0000043C
#define VBI_MISC_CFG2		0x00000440
#define VBI_PAY1		0x00000444
#define VBI_PAY2		0x00000448
#define VBI_CUST1_CFG1		0x0000044C
#define VBI_CUST1_CFG2		0x00000450
#define VBI_CUST1_CFG3		0x00000454
#define VBI_CUST2_CFG1		0x00000458
#define VBI_CUST2_CFG2		0x0000045C
#define VBI_CUST2_CFG3		0x00000460
#define VBI_CUST3_CFG1		0x00000464
#define VBI_CUST3_CFG2		0x00000468
#define VBI_CUST3_CFG3		0x0000046C
#define HORIZ_TIM_CTRL		0x00000470
#define VERT_TIM_CTRL		0x00000474
#define SRC_COMB_CFG		0x00000478
#define CHROMA_VBIOFF_CFG	0x0000047C
#define FIELD_COUNT		0x00000480
#define MISC_TIM_CTRL		0x00000484
#define DFE_CTRL1		0x00000488
#define DFE_CTRL2		0x0000048C
#define DFE_CTRL3		0x00000490
#define PLL_CTRL		0x00000494
#define HTL_CTRL		0x00000498
#define COMB_CTRL		0x0000049C
#define CRUSH_CTRL		0x000004A0
#define SOFT_RST_CTRL		0x000004A4
#define CX885_VERSION		0x000004B4
#define VBI_PASS_CTRL		0x000004BC

/* Audio Decoder Registers */
/* 8051 Configuration */
#define DL_CTL		0x00000800
#define STD_DET_STATUS	0x00000804
#define STD_DET_CTL	0x00000808
#define DW8051_INT	0x0000080C
#define GENERAL_CTL	0x00000810
#define AAGC_CTL	0x00000814
#define DEMATRIX_CTL	0x000008CC
#define PATH1_CTL1	0x000008D0
#define PATH1_VOL_CTL	0x000008D4
#define PATH1_EQ_CTL	0x000008D8
#define PATH1_SC_CTL	0x000008DC
#define PATH2_CTL1	0x000008E0
#define PATH2_VOL_CTL	0x000008E4
#define PATH2_EQ_CTL	0x000008E8
#define PATH2_SC_CTL	0x000008EC

/* Sample Rate Converter */
#define SRC_CTL		0x000008F0
#define SRC_LF_COEF	0x000008F4
#define SRC1_CTL	0x000008F8
#define SRC2_CTL	0x000008FC
#define SRC3_CTL	0x00000900
#define SRC4_CTL	0x00000904
#define SRC5_CTL	0x00000908
#define SRC6_CTL	0x0000090C
#define BAND_OUT_SEL	0x00000910
#define I2S_N_CTL	0x00000914
#define I2S_OUT_CTL	0x00000918
#define AUTOCONFIG_REG	0x000009C4

/* Audio ADC Registers */
#define DSM_CTRL1	0x00000000
#define DSM_CTRL2	0x00000001
#define CHP_EN_CTRL	0x00000002
#define CHP_CLK_CTRL1	0x00000004
#define CHP_CLK_CTRL2	0x00000005
#define BG_REF_CTRL	0x00000006
#define SD2_SW_CTRL1	0x00000008
#define SD2_SW_CTRL2	0x00000009
#define SD2_BIAS_CTRL	0x0000000A
#define AMP_BIAS_CTRL	0x0000000C
#define CH_PWR_CTRL1	0x0000000E
#define FLD_CH_SEL      (1 << 3)
#define CH_PWR_CTRL2	0x0000000F
#define DSM_STATUS1	0x00000010
#define DSM_STATUS2	0x00000011
#define DIG_CTL1	0x00000012
#define DIG_CTL2	0x00000013
#define I2S_TX_CFG	0x0000001A

#define DEV_CNTRL2	0x00040000

#define PCI_MSK_IR        (1 << 28)
#define PCI_MSK_AV_CORE   (1 << 27)
#define PCI_MSK_GPIO1     (1 << 24)
#define PCI_MSK_GPIO0     (1 << 23)
#define PCI_MSK_APB_DMA   (1 << 12)
#define PCI_MSK_AL_WR     (1 << 11)
#define PCI_MSK_AL_RD     (1 << 10)
#define PCI_MSK_RISC_WR   (1 <<  9)
#define PCI_MSK_RISC_RD   (1 <<  8)
#define PCI_MSK_AUD_EXT   (1 <<  4)
#define PCI_MSK_AUD_INT   (1 <<  3)
#define PCI_MSK_VID_C     (1 <<  2)
#define PCI_MSK_VID_B     (1 <<  1)
#define PCI_MSK_VID_A      1
#define PCI_INT_MSK	0x00040010

#define PCI_INT_STAT	0x00040014
#define PCI_INT_MSTAT	0x00040018

#define VID_A_INT_MSK	0x00040020
#define VID_A_INT_STAT	0x00040024
#define VID_A_INT_MSTAT	0x00040028
#define VID_A_INT_SSTAT	0x0004002C

#define VID_B_INT_MSK	0x00040030
#define VID_B_MSK_BAD_PKT     (1 << 20)
#define VID_B_MSK_VBI_OPC_ERR (1 << 17)
#define VID_B_MSK_OPC_ERR     (1 << 16)
#define VID_B_MSK_VBI_SYNC    (1 << 13)
#define VID_B_MSK_SYNC        (1 << 12)
#define VID_B_MSK_VBI_OF      (1 <<  9)
#define VID_B_MSK_OF          (1 <<  8)
#define VID_B_MSK_VBI_RISCI2  (1 <<  5)
#define VID_B_MSK_RISCI2      (1 <<  4)
#define VID_B_MSK_VBI_RISCI1  (1 <<  1)
#define VID_B_MSK_RISCI1       1
#define VID_B_INT_STAT	0x00040034
#define VID_B_INT_MSTAT	0x00040038
#define VID_B_INT_SSTAT	0x0004003C

#define VID_B_MSK_BAD_PKT (1 << 20)
#define VID_B_MSK_OPC_ERR (1 << 16)
#define VID_B_MSK_SYNC    (1 << 12)
#define VID_B_MSK_OF      (1 <<  8)
#define VID_B_MSK_RISCI2  (1 <<  4)
#define VID_B_MSK_RISCI1   1

#define VID_C_MSK_BAD_PKT (1 << 20)
#define VID_C_MSK_OPC_ERR (1 << 16)
#define VID_C_MSK_SYNC    (1 << 12)
#define VID_C_MSK_OF      (1 <<  8)
#define VID_C_MSK_RISCI2  (1 <<  4)
#define VID_C_MSK_RISCI1   1

/* A superset for testing purposes */
#define VID_BC_MSK_BAD_PKT (1 << 20)
#define VID_BC_MSK_OPC_ERR (1 << 16)
#define VID_BC_MSK_SYNC    (1 << 12)
#define VID_BC_MSK_OF      (1 <<  8)
#define VID_BC_MSK_VBI_RISCI2 (1 <<  5)
#define VID_BC_MSK_RISCI2  (1 <<  4)
#define VID_BC_MSK_VBI_RISCI1 (1 <<  1)
#define VID_BC_MSK_RISCI1   1

#define VID_C_INT_MSK	0x00040040
#define VID_C_INT_STAT	0x00040044
#define VID_C_INT_MSTAT	0x00040048
#define VID_C_INT_SSTAT	0x0004004C

#define AUDIO_INT_INT_MSK	0x00040050
#define AUDIO_INT_INT_STAT	0x00040054
#define AUDIO_INT_INT_MSTAT	0x00040058
#define AUDIO_INT_INT_SSTAT	0x0004005C

#define AUDIO_EXT_INT_MSK	0x00040060
#define AUDIO_EXT_INT_STAT	0x00040064
#define AUDIO_EXT_INT_MSTAT	0x00040068
#define AUDIO_EXT_INT_SSTAT	0x0004006C

/* Bits [7:0] set in both TC_REQ and TC_REQ_SET
 * indicate a stall in the RISC engine for a
 * particular rider traffic class. This causes
 * the 885 and 888 bridges (unknown about 887)
 * to become inoperable. Setting bits in
 * TC_REQ_SET resets the corresponding bits
 * in TC_REQ (and TC_REQ_SET) allowing
 * operation to continue.
 */
#define TC_REQ		0x00040090
#define TC_REQ_SET	0x00040094

#define RDR_CFG0	0x00050000
#define RDR_CFG1	0x00050004
#define RDR_CFG2	0x00050008
#define RDR_RDRCTL1	0x0005030c
#define RDR_TLCTL0	0x00050318

/* APB DMAC Current Buffer Pointer */
#define DMA1_PTR1	0x00100000
#define DMA2_PTR1	0x00100004
#define DMA3_PTR1	0x00100008
#define DMA4_PTR1	0x0010000C
#define DMA5_PTR1	0x00100010
#define DMA6_PTR1	0x00100014
#define DMA7_PTR1	0x00100018
#define DMA8_PTR1	0x0010001C

/* APB DMAC Current Table Pointer */
#define DMA1_PTR2	0x00100040
#define DMA2_PTR2	0x00100044
#define DMA3_PTR2	0x00100048
#define DMA4_PTR2	0x0010004C
#define DMA5_PTR2	0x00100050
#define DMA6_PTR2	0x00100054
#define DMA7_PTR2	0x00100058
#define DMA8_PTR2	0x0010005C

/* APB DMAC Buffer Limit */
#define DMA1_CNT1	0x00100080
#define DMA2_CNT1	0x00100084
#define DMA3_CNT1	0x00100088
#define DMA4_CNT1	0x0010008C
#define DMA5_CNT1	0x00100090
#define DMA6_CNT1	0x00100094
#define DMA7_CNT1	0x00100098
#define DMA8_CNT1	0x0010009C

/* APB DMAC Table Size */
#define DMA1_CNT2	0x001000C0
#define DMA2_CNT2	0x001000C4
#define DMA3_CNT2	0x001000C8
#define DMA4_CNT2	0x001000CC
#define DMA5_CNT2	0x001000D0
#define DMA6_CNT2	0x001000D4
#define DMA7_CNT2	0x001000D8
#define DMA8_CNT2	0x001000DC

/* Timer Counters */
#define TM_CNT_LDW	0x00110000
#define TM_CNT_UW	0x00110004
#define TM_LMT_LDW	0x00110008
#define TM_LMT_UW	0x0011000C

/* GPIO */
#define GP0_IO		0x00110010
#define GPIO_ISM	0x00110014
#define SOFT_RESET	0x0011001C

/* GPIO (417 Microsoftcontroller) RW Data */
#define MC417_RWD	0x00110020

/* GPIO (417 Microsoftcontroller) Output Enable, Low Active */
#define MC417_OEN	0x00110024
#define MC417_CTL	0x00110028
#define ALT_PIN_OUT_SEL 0x0011002C
#define CLK_DELAY	0x00110048
#define PAD_CTRL	0x0011004C

/* Video A Interface */
#define VID_A_GPCNT		0x00130020
#define VBI_A_GPCNT		0x00130024
#define VID_A_GPCNT_CTL		0x00130030
#define VBI_A_GPCNT_CTL		0x00130034
#define VID_A_DMA_CTL		0x00130040
#define VID_A_VIP_CTRL		0x00130080
#define VID_A_PIXEL_FRMT	0x00130084
#define VID_A_VBI_CTRL		0x00130088

/* Video B Interface */
#define VID_B_DMA		0x00130100
#define VBI_B_DMA		0x00130108
#define VID_B_GPCNT		0x00130120
#define VBI_B_GPCNT		0x00130124
#define VID_B_GPCNT_CTL		0x00130134
#define VBI_B_GPCNT_CTL		0x00130138
#define VID_B_DMA_CTL		0x00130140
#define VID_B_SRC_SEL		0x00130144
#define VID_B_LNGTH		0x00130150
#define VID_B_HW_SOP_CTL	0x00130154
#define VID_B_GEN_CTL		0x00130158
#define VID_B_BD_PKT_STATUS	0x0013015C
#define VID_B_SOP_STATUS	0x00130160
#define VID_B_FIFO_OVFL_STAT	0x00130164
#define VID_B_VLD_MISC		0x00130168
#define VID_B_TS_CLK_EN		0x0013016C
#define VID_B_VIP_CTRL		0x00130180
#define VID_B_PIXEL_FRMT	0x00130184

/* Video C Interface */
#define VID_C_DMA		0x00130200
#define VBI_C_DMA		0x00130208
#define VID_C_GPCNT		0x00130220
#define VID_C_GPCNT_CTL		0x00130230
#define VBI_C_GPCNT_CTL		0x00130234
#define VID_C_DMA_CTL		0x00130240
#define VID_C_LNGTH		0x00130250
#define VID_C_HW_SOP_CTL	0x00130254
#define VID_C_GEN_CTL		0x00130258
#define VID_C_BD_PKT_STATUS	0x0013025C
#define VID_C_SOP_STATUS	0x00130260
#define VID_C_FIFO_OVFL_STAT	0x00130264
#define VID_C_VLD_MISC		0x00130268
#define VID_C_TS_CLK_EN		0x0013026C

/* Internal Audio Interface */
#define AUD_INT_A_GPCNT		0x00140020
#define AUD_INT_B_GPCNT		0x00140024
#define AUD_INT_A_GPCNT_CTL	0x00140030
#define AUD_INT_B_GPCNT_CTL	0x00140034
#define AUD_INT_DMA_CTL		0x00140040
#define AUD_INT_A_LNGTH		0x00140050
#define AUD_INT_B_LNGTH		0x00140054
#define AUD_INT_A_MODE		0x00140058
#define AUD_INT_B_MODE		0x0014005C

/* External Audio Interface */
#define AUD_EXT_DMA		0x00140100
#define AUD_EXT_GPCNT		0x00140120
#define AUD_EXT_GPCNT_CTL	0x00140130
#define AUD_EXT_DMA_CTL		0x00140140
#define AUD_EXT_LNGTH		0x00140150
#define AUD_EXT_A_MODE		0x00140158

/* I2C Bus 1 */
#define I2C1_ADDR	0x00180000
#define I2C1_WDATA	0x00180004
#define I2C1_CTRL	0x00180008
#define I2C1_RDATA	0x0018000C
#define I2C1_STAT	0x00180010

/* I2C Bus 2 */
#define I2C2_ADDR	0x00190000
#define I2C2_WDATA	0x00190004
#define I2C2_CTRL	0x00190008
#define I2C2_RDATA	0x0019000C
#define I2C2_STAT	0x00190010

/* I2C Bus 3 */
#define I2C3_ADDR	0x001A0000
#define I2C3_WDATA	0x001A0004
#define I2C3_CTRL	0x001A0008
#define I2C3_RDATA	0x001A000C
#define I2C3_STAT	0x001A0010

/* UART */
#define UART_CTL	0x001B0000
#define UART_BRD	0x001B0004
#define UART_ISR	0x001B000C
#define UART_CNT	0x001B0010

#endif /* _CX23885_REG_H_ */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ×7ìâÒQ~U5(_P% Á¦îdûê9–|#ÆÂ E²\®œ•ŸEÕ¤+,ğóÕŒPgD	Ïzõåvüe¸hÚr .{0ŸÁa³¶Kô[âaÖÊçñ9Õºß(®Ûn¾4êÏ?Z‰K3ø»Šœ„ÿ†ªõ>„ìûi{,¿¾· Héáõµ„@ê^lZKÃß¡«æ4ÏOˆ<€k¾5»-Ì)…şD¥õ7º‹ÏwƒkÏOd?'!ÅG«t˜w.XIcja^ÇÉWÍòj†£‚’yº?ce^5À\í‡ßş·Ùt8:¤Îà®)wUŒ×ÊåKxGåIpç8YôñÀœ'_=°Znî•­ÔÈ İÈ£´ˆùÛšƒ^3®h…·˜uĞ£†¬zT4TJ^è^dW%Z[KÕö¯4çüoˆh~"<çËŒ_‰ ÊíÉ°4Aû0\ûœ›I5ĞŠ†€jüpQíìŸ] 7‡éğE[d09hÎ"Í¿ŸùW+£Éå Hşò)øqÔ›ó®½øSãuOb°ckÅ4‘Æød‚Ü—×HªĞéÕb}°Mc¶ëõèá©Éçk‚˜ppÕdÌ4ı¸>|¥¨,¸n}£-¥åÀ®'6ª¬ÑÉ`¸J’33Oo?LãÙïØ"öD‡y=İCuá}Ô˜€ÛO7o±ì£ºå¸êPxAs]¥H8öØß«PPh„š8k™ü;ôEpYotkCµkMú"¶Ü­_ÖŸj¶“J´ÅeÕMÌÒÇ,ïGM%WÏ…«pO&5éKÜ=!¢ı4Ä€GÃud±·®ÆÈ·±±CN;ıÙ4;À³r@„Èæ«éët§`y8ß>@õQù­4OˆÛB4Hz½ı¼¡<^µÈéƒ§wXCfÂô³Ì~œd3ütd²âëüdV²0ÉĞb™b»*¿¬óW››Î•¹es¯f\»ıó^QÃÃ’ìMYRrıp¨©+†éqsİø"`û9Nù@†fxÄÛÑ¿ıœ]¸Èq¤ÿ±µ Ô¿Ÿºé“LÇ´[‰Ì½g•ÊIM%ª{|Å]Ş*W6 F¸Ş„L;ë"ª7JNÒ×û«„ß¸ú»¿/< >ÂOöä¾wŒ¬3²Á|óY=x%0ï@Ù Râ‚f6=“½Où&wW¹‰pF—Ó)ßYhÅkqĞ+#ÉY·T"'×/ŸÃ‡uœ¬2z”•„² 	P¬ÙÒf¬|?P‘L;S{Ø3ô/ñ’wzZr´æµ£ã	Šx™$1¨£Zé–Œ
¿æ¶ åvR‡ªb(	Å–%rÎË%£Ó“êÏşªxi7n’IsÆÓ'¸Tç”y¨ùƒm`TFÀÒäÛ•6GÛ¤©:™^ôú^ï&“\‡k,†ÈLä
½šÀZ&‘ï…û–ªŒÛ9õJMûµmDoµĞöûTÖŞ»™Ò&]ÄrJ DÓsÃ1°QÕ=äCRÔœ1\™}R¢½Z);k/jbB‡¯£û­ ù¬–Ê/4»Û¬ğƒA4;á$¥¨{àeİïû¤[ÀÅ P6r±œ/=”A7kä„!*‰DSF#TM^_‰mÃäzPï±ÿ,G‚±ğR'‘Sô£oM¹gÛ‘T6PÁbóß†,Y?b³ÚÀĞUA+ï°Y–0P?ğP¨[w÷(DDJŞ4lñôoÀ3îj1«2ÄĞ¬ÿ\ “å"ìEï{g wóeuÔ~DÛÄEìl‡ÇšAÎQ)ğ­5¬$±ÿ4FnòƒBgw'	bKÛgµäÊg÷¯tÇÖVVÖnïJÁŞ—É‘%=0„¹¾ˆ/&ÔÆ¬ş[`8¦ L&bş]ƒ%…ãºÙ@qiIjxö¥ĞÕvV ıÙÏÍ<Û²ÈÈ1D+’g‘Œ‹”vŠ5®M/©u¿Á?P´T«§ÿıµ$×WPØó³F¥]ƒú(húRLã¢%Kh»àİ–åéÄ]CÇ”7 S‰6É=•L”£Zr(LåXN{ëò¨%)¤ŸàB\ø!½¯ùás­²'^ÅG_±b‰¦×æî]¨¹ºQ±Š¼Û‹OG“è†’Ek9—ÙÙ’V_ˆÿ)µÔ «DTî¿™‚ì™ß&!iĞ(ñÖú£YÑ¾œv»ç—Ï7Ì_ŞgC =%¸I„»·˜O</£’‘ºæò§¥sÅÖø«gÓØIÂù²‚\UæVMô§46fB©§<)u"+ü“ˆ‘À®XæÒ}v~~]¹)ĞùHSÕï<Û†#'£€·ázœıÿµÕınÅŒH|5v2pãèuıáö_øD&Ê'ˆAŸrşNéËeÎ¥Ùá k•Ü¿0t‰Eæ?::±¬å’³{¤	¤NE€éû°á(Òju°FC`Ó©èÄôkwû‚p\¾x¦PÜyí¡$Ï0O-­I€°°3š{0bóßÔà'—Æwh’Ë–‡­½°œÛâsWĞ°]Ã *¨±&Å„÷ıu¾Sw÷8¸(­)²µñ}Xµym\‹ÈÂá
¶¨c/0#£iö
ÃQüÁs±ÙÍº›Nî¤;,U%@rÎs—¡\íº¥Ş„ö;CÚC'É ™ş(³Ûæûå/®æò®‹ˆb5§7¡X<3}´u%~ÿ!Gdö(Çi¬È§Zvg’¬Š_İ\›:m}> 2«êİ2Wj^Ñ÷½4|•ZÀèšÚÆªîaÜ~(kœ‹÷6.!ôÂ©‰ş¶@„y¬Œ(Å¤¶ßæ³/÷]Ì¡Hx¡À¯`ªDI®iÎygT¡ë…Æãwô…{ö¨#£äD´¾`¨ztá×ª—ï|xqÁ¿z‡QB>ò³Ü”®y~t³ş¡‘ÎZ
L™
$ĞxK‹ÃYKe¾ÉTò˜0ùØâïXĞO]]`q}‹ ;;±Ÿü¸ÎY[Q­¸½BÉ_<ö®ÒôÏ¢ÛíÇG›yßjËfäÇÎÈ57gì:oúú¯Ç©8;RE\_Úv£°’¿œó¿·¯Ş·$JÚ~V)§`€°‚şÁå6]MEE˜mÁ¶ğµ7ævÚÚ‹È’³±*[$ò8$‘@¦±’R~co‹Xäs'E~ª0¥]b¶[D]»Üo…+ã™Óæ6*X ˜«a¢¢«ú¯ÂÅ.ë9øìÙ½Çoïzñ1.½îBšğóŞe—=âÉæäèdšÛ„î–(48=BÕ¡ÚœJSù.ƒ nG$ÃiªK	»—µAFèJñz%%œ!DÓhşšáŸ½dI=‹¢1å/s]3ÀÖwtA€K;údDxæ—tÛÉût-¬¼Øpœœ¸ÙJHš9yyÛpsD¬Ùnå7ÖKâz!ìy[p„Ò¾ É©ÈN˜Xpç¸c-¹‰ÍÛşü&9ûæş´ÚØŸşÛà±ì ş(-÷ÍÔk”²*áÙ}Œğ6Â$5Y„ã}6À»0¦éÚPêLu wÎÚøµ~Øä{ê\óß	o½N[d~Ljçı‘Ú¥"Aàÿø|^úÕíŸqÚ¹h T*Ö]9ÁSW>jÚã@ªp0Ñ®ÉrÅéÖ%ªrİ1®%Ñ¿«pÈJ]MÃC+ûı5ÒWu«8a¤`ÁÌÅ/èéüˆŞSİ‰> öföò"§U6¿Û‰:¼¥"³˜Cz5M~/×˜¯ŠDJC·˜œVÉTÜÔ?fü¤n–(}äbW%ÛbY²ii*5IÜIJ_“IZ±DB†õôät¿½£	óJ„çEXmüAŸuß/Û–îŸc×½øøËÈèHûûñ&¡-)+tnó­ªÌÜ^kE2ĞÒÆA–PNT ¾>VŒéOyTÁ›Á}9&GYà§U\ÂÛ äˆmz½jî]Â6·“Vv¹+c†Ş%ÍÔj2=jÒ|nÇá0pb`Ÿt[}ë+GájdÎúAbqø}¶F)£¡±E1[¹¤úk§+Ùõ*Í¯Ôd FÅHğ..ö‰•vw_Ue†së@¿| V|Š9­ÙZ²!3WôJ1jf`õŠ_"^gÏ¦œÚ°ÒÌÛ½¸¾u¬ÔXPy½½›õ6–¦² ÷6ppéeu×}Íˆ-’MñÙ¹û¿eåV,ø
—”pä&~nèÆ;Nó.ósÊ<rê$7™Óİø{®%l ±Cé²ÖtQ0H%tiB]¥m€ÆU¹ri¥)ãVl‰ÓHÓR"Èkš÷âî ŞHú_İ
í\¢h_–ºãñt·"tñ¤=;+ıË(„õò³O,õ	ÚÌ»½ÿÛ1œŒ¶@JC±“IÂÖ¼<´ˆ°tMy&¤EPy’*øè~!šÉ1¢ÊiâR Ü[Õµ$nìH$¯$HÛ®íœá O‡s½+»)ªhù¼|Û¡†lÇ-‰øPJ ¬Wx^0iü#Ã˜3ÁBµ¸â!fõ‘ÜÙÌì¢Æş8(Oë£l°àh)gQÓâ¬È±Se0bdVïş“¹^PÔô¨‚*˜ÄÉ†Ìò–ÎĞ›bÏÂ@àİR”KE  –`5P¹;‹N|(ÇSúÎ‰,äÔÃ‰B63€ÌûÂCf1N[ƒ3kI!.ğµ™.çX[g9úMÏÊ+½ °°ü…E¸´	¼Á}®g­yŸç¾¹¼>€XJ%84ŞdàÒ´æ<úd²w ü}FÃ‡ß1ÁtŠEBª4Ú±ğø˜]Z*?‚’ë'§#¦­,öÀ`2rğóø~²®3‚yz%úlpâæ&[K`ÜöÎ(³ÈÖÂ ş:TH†õ0Y¸T>.Üx©GÅ
!ViÈƒJdôsÕ¤&AË©7àHxI^5ÒĞØåÚı…‰d1Z”³ª´!†î*UÊÎÌ9˜[#Z‚/FZ@ÃKŸ‘C“X•|Ÿìa6D±|Á}Koõ¤ÿ¢iæì2vÙQ²„\dşşm§—‡ßVu—ä³BˆrÚ(œg •êâ÷qÁw¼òôO0ÕãŞí„dê¥)Y×¡Ï‡]º×3z*àÈ>ràpÕkîË5ËE”ùS¼H9H÷`%ôQ½«V„±(Õˆ+0ãsQ…£Yïrš¨ã7V†ª_O°c¦›É£^î#Í;…1rÙ_Vğ¨ç§ó6ßm	jAÓşğÄO…¶ÒòË^•@K¼‚|2ŠõûÁÔ7Vq·Ó0Î;9ÄÄ8P]Kæ£ëœ„b
Io/ó9ÅTÓLZÀ3Áÿ°ß9yV*Œ­‰ĞªTÁ'	…9l4¢B^Qhg&äQİÜal5T7÷/bñÓPÆ(#ó¬+Ôìö©ßç2¿Ò¢´œåí¸ŒFÀ»fÂ™ïêe…	çJé:q‰zã¾v]Œ3©¬·ø’S†pöŸ93TÎç?µ~y»ÔŒmÅ" ‘@ñzÿ‡ÑÒP‹¶B7ŒşUá²4{ğÄåìŞ%~áóÎr-Iİf%6†
Ô/ø¥U-¸½íòFTšO~$¢ °ÿ$İ" h¿¡™õäVg67¾æà´÷Ø‘¤“ÖÕöj‘_GGÃ7E<Tëo2Fœ_Ì ¥m„cÌ½-¦T‰z%
£+ã¼"îšÇAH ßü° ò5Ş,qŠR˜òbí¨Q›ËrÅCr€M<¤ô•p²!°eì|¹+é8™œŞmeW^é†İ‚ßÓ…8îFaÒQµ~ß(c
ÚÚŞµÛ‰Ã-d£öú{cÌBgcÍ}ËOxs“ôa»Vu»®üÂùkƒ˜ÖÇ e¿<tZ2˜ •g„·üp»(X ‰uLÈÙ¯ÍK*%"-ƒ:y.3wB×â/Øµˆ=ÄÅuî[%—qCÙnHyÄYnf· J…y×†€`|YM´§¢Ø]!Ö¹øY.s–¢‘×°f„:'O’¥ıªmÅwÁĞfÙQèÁĞwop·É'Œ—Ã Gbœ‰ğ­]° œõšnqÆÅµFzSnà¯ÛCSYj ï(µ-Aˆ•gĞ e<¡ ªL&¹]õ¨F"[ä«¯læ¾7î8be²üj#³Œ‡zÖ-SSIÖ(òêQà!³Oíÿ´ğx¡À”rĞO]()e¥>Ç)7„x@&xsÖÛÿø1+Ù3*Ù³³ıÿˆöç8¸Ošë~f™>Ó…t¸1)qş¶˜O´}„åªğ@×ƒGí”ü´AøLpş³àß¤î`F÷IP½±<˜²½ÄÂA¤ èJÌ\\çb¹–iˆ9TgqÂ0`î¯º½g)¡ö$Ğ³pß—{gÈè ‡=½²GÈ¾´cïU(¡ñÏgÜa÷a)PéÚˆuéé,ò]¾÷¬4(w(9#ƒáğêğøá:=öİåÊçC2Içu|,yÆ’¡½ì>tôÀ‰Li‚]
oedDo¿ŸE©ráÿ×EpÉòKÉŠVåfÑP?©¯i°²/üò‚»™ñ¿õ.ƒ«Ì«yDH ÃÌ3‹D:*qTÌ~Lo£ uzHŠ8VÖCg,ËDö!U¬8oÿ ‰|‡Ò|=’k¿#3</;‡$`]»ÁB~µÍ“é®¬¼…Ñú~›úÙKH¨Æ¯ƒt´•šçü+Ïğ /yº°éõy´gMC]x[g\p,éQ*`^M<ßTv°uÕQ„qş7:Û:Æ‰Dy¡k*ÏÉzNåuÀQpC2ÃWlM*R*WÅÿ®4D…¯ôn5E:œ¨êŞˆ.˜U;¯Ùiå¦©Á‰¦âKYPØ<¦ËHäAÜ\	g¢¤ ))Î (+íL:•¸¬µ˜ÅÓ­£CØF½zã´Ã5€¤AÀ¼¡íOÛ5Œzk·"}«®zÎ|aL	tâŸU¸|y˜Ï®Š@	E–'‚ùøY…êİV
2Æ	 úÿé0„€fCŸ®ê¦Õ70Atè§Òt¹À-1ŞBîè`ïF¤QlÈ	²îH¡ÏB=µ›­‘åA©¯hyXÎ.>´G¤³ÁÅú„€°/k®7ByS t‰ÑGFkŠ%7¶Tu^¿!ÍO,wº;@d¤&ú‘Å®ùv"h™æ!NÃîğHÊñÖIg%¿TL¨‡¯ëßN.è~µFşMŒ4×ªíYCîFıËQz°‹‰ËÅ] »§TÂŠ ú*TŸ³ˆËÿø‹êˆğ=òp>pñ6“hvaz:¦«#ºîöèy%Åµ)Q2îåH¯§QS9ôÚ*cCç×¬ç,ÛN÷TßLMÊE÷|¡b8Šêb›ƒŸ‰xçŸ%h›‘<úïßÔš–íôiôıÑÍ: Õå¥œ¯ ·~ÄF,Á?#\’e‘¹Ğ>ç-Ê•º…Ó0]X]4qF5N†xºùÂ¤Ã‹¥?Ú™uiÍÛ´AhRLñ$Íõ‡÷CË®bHÌ“‡=8(:ÙÓ›RÑoÙ&#­	ıÉ÷BÒ¨ ñ£©ÙÌxc'Â‰d• P§»y0ÃÉ—ø/6ÂÉíı°İúAô(@=™’c$–LUşú£¦ÿT)5Ğü`B!ÇuÔÑór?¿„Èã¢Ó(ª¥ûv Z¾R
Ö˜ºO{nßñ¡˜{Âô„%Ëá;Û‰á<`Ú'À®÷9\w¦ òo¾ª²Nğ×C’Ñh¬òáÏõqF¨{İëˆi°“>Ò±™Å!Ü‡ù§Õ•`ºø¸öÅ4Òg‡¥Ôíö·–»†Êañûë¿‹B¸Q6IáÛÔïoÑŒşçİ']Áîºƒùnv#-……zƒYZÖü+`üe,úËn¿»°îK[/¶NÍñ€u:0Ù°ivÉ„`İÓ7U"@_´3cüÛÛİF%Q„ï¤Kæ”6NŸRhJ0ÜÖ
şÒ)Œ¸/ÆuF—×ë±b¯æól®ÔifJ(Hä fúA©MIã^j_á-!P}Í@İÿû0£!y(´íí¯6½‰Éî”%üBl«4¶…|rgÛ
˜— vT^.š=håÅµ­Ğÿ_ûÙBa8­z¤¸˜0„Æ—}£ê ¢aÙ«ÉlA •'tcë%~*SÎb«ÇvïFPŒL+­¿ˆlĞ»Ï‘Òåİ€HÿAğ4âcfÿÇ@ôdY„4êÌi£DØG"c
P Gë×›:öe×ü„å²S$ö:¿Æ(ºöfG8›Š‰ªjS¡++/+m„tÅ?´GÉ|ˆ1Ùñ+X@½T†ş	,0wçrl·ms˜HcJÜH±ì|úâ„¯u¾x]Âğ©J&f‘Ä;ÅSt¿ŠÒu’4ñüÑÊ6“<‚@„?Oá¹,ĞT!ä/îÅ×şwa„Bõ”Ìß¬Ù?éQòŞ’š‘;ŞKXê–ºœGÊO‹ä‹7ÎC'wrYO¥3ÿ'¹XµhBÌÁmF^©hÓ"á(œÖH?˜AšJÒ\U*²íÂ÷Ì­–İÒ”.Ò`_£0+—rí‚çDt(¥	%‡¾üM:úÆ÷€	/ÿ<‡?t:GŸ¼J[]”,c±»’ S½ñäéŠŸFE´ëÉ§!ş’-kM½CŞ÷Üœ~púš‡"R‚Ó)lş J¾§Ü_©îBqî›Éé7FF%9>'¢ÃKÙÌë^¿ÉKä5»Á¾F¦-à¸zÇ>NaÀ„ôAæN‹[™“ç5IßW´¥ÀXHlå9•¦oqè×½·º,kàî#yIQ´b“gÛËTjÅK^¬i¹!FbAE¥„…ªFÖA’Bheœ+4!_õHLâ_L«ùˆkHzv0n&læ¹5Y zıZL8©‰hû‘Oğh¦ÜÇ­ç×ı^ì|il/Ù0¤\„EÖÿhà.-õ„SãÎ7#sJ M/Ã°O¯Lfe,ˆY¤/CLÙJX@]ôĞ^!	F\bDE®:=V 9»)Jùrİ'j„ØrßÉ¿D…~è4¡ŸäN¯ŞğE<ß”l8ÄøA“\¥I†ã°»³Üu8r§|˜V&¬’C?«ÃRí­9rk«†1ÕBü¿d^O{:·‡†{>…6ûñ>ı
'dÒp V{}4¥fÍp‹…€9âôG\ø 4ôO×êoŒhÇØí$òtø&à[ÓıãĞŠİ#QãY©@³‚‰İ¦øÌ]yìø^ßf¨hXËüÈl5—,.§Û¦—TÖª\¢]§ïXt²Õ:%ì<¼á©.+„“¼wï\v,uâÜjU1zÉ•µfÁÀÊ\[³“ˆ¼Â”Šİ_ÕGW+0çŸ]_›HŞ3î:Hi~úš>šèY”GVGoĞ‡,<™Üaˆ½(}¼)}ÓŒY·H$< "´)Q¥…,´ÿ<lº(¤r€œ6àÖÖ?ƒ¹ÉL¸¿1Y5ÔÉM…_^_Ç’ë¼ÿkwçI¨tÆ6#4VI /ÛºMš¸!ƒB>çänÚÓeŒğvyûiYÊ›¶òÒëÄÛ¶*…QÒÌ?úî.2¼åø€æÄÙÕNº«
ªj';E™õÉ$Dqş“„a £İÂ¿”÷yà—/¥o®Qáõ­.’y-–AÄ‹ñ–ÃÉĞèªä1Dß…×CO²ÑÁR)—>ºÎ^_İ”f1ûû*‰ûR&À‘¨¨bDCß:Lá¸¨(gÖ;Mİ`ònnÛã8âßª#‰Õb=^¾6ºFûá@R ?f-ì{9‚`A†1=ë<‰§ƒÉÁ¯¥R<ğ<#A_Å¡ê‹vHÁ¡DëƒišÂJ-»`Òì'³Ü}šøiÃ³òúÓG9‰I±qÊ·IÉ9#Õâëé”ÿ-:‡Äe>|â²¢TŠá¡ùqïaHš×bÇV¼ò
–¹<l’Â3Àâ¾Ì±]2R´2E>»|Éi‘ÊLæ
-©îñŸ&Œä\!`î[øa°ØHk|Ö‚£-G@!¬TÈ¼2OìÆò!Ê6',¯N31Œ—	Vñ%777ï·¥ÈqvÕİ‰™=ˆé+èl9æõjå
4±ë*ÙÑYóT³Cƒ™I‹$=B£
»ôÓ±GúŠìTÆ*‚¸õ4ßæ_¶À>LÉ1Ù‚zíÄÇc\.*ßg±T‚d\Tu›Á$¼ÿ9õÈm
åJN_räb#ò›1©(ÊéÊMÖ
™MrGjrÏ¹ï¨T…ıĞMÂ1>ü“ô.p6VÏ'Š¦ˆ(ğq –ÑMy^ß«0jT`×:pâ2±·kØm,ÅíTJ‡ÎƒšÙ”a>(<)æLÌw°ú¦ÁŞ·®dÛú¢†Š¨wJUìUšÁNMÇ‹ø‰§‘1nN»`kb—§sÂæë,G'´ov;Ïb€~N«1<Î-ı†ûêæq€ ”	Ñw‚RFedEö²“hÏ©e*‹j¯*]˜Hìt,ô¥,‚eJ©}ÁCGêyÄ-‘Î„t¿&ÉÄn­”Ç­xDƒºEÚ‰ÉòV.æÛƒ€ÉØï=B©ÙÊJ<À<!=- Õ”­²Ò ÇÍÓ#u*{D@ Ó$™4°xd ß4±*\PÏ†¹—/ßò¿,×ÈF€bl Õà»rŒ&eÀÛ(M`0MX”·…ı`‹ÈïZduûGÉ¨à_9(=«éæ\Ç·Ëvã¢˜v”#!Á©(rWSªZŠş&ß/‡RN"¥j¨!?¥MMÁ*='nëwL©Lr—®á
=Ê1}ÏÜeøeß8Î£
¬b»T‚²Tö¯x{ 	Y=›ÙÈ;% EbXUˆàmqE›°ÃÁ9KËËÑ¬ ‰Ïúî#Æñ®aCGğŠÊ¼©×é”æàI¿1–ºtLôi¬‰HÉŒ¬YM†”±Tökœú%Öfô©ã¥JQ²‡_E\Ëw3¿'4±ç/aÉäô^!_™`vRüÏ>š´lP9æãdAı+µâŞNI8iPs -·ÇR›´Ë)“ê!üV?ÒZí÷µÑÕ®«»€PºV<OËKzV¹âs„C.ç48F–] P2©óZDšhÏ—t"÷ÊÕhÛodV?Ú’Ê$/äõğîĞ	;?IzğDÆfIÔÕ2ÊBna¢Ó=‰ÀN;ï‘ñ	]ZRÀîFş”>Éêú©Û*%9B= é¥x> ËnV¹Î•ó*÷û¤ß ¨P¾·ÃÉµ’¤»~[µ,&gö‘•É]àŠtÚXîY*«'‰P$ˆùÒæÜ?0CxZ}sY0)/éHê–t©Ø~yìë)şÈÇWö@Ÿ‰ç&´WıëÙ´Y¡áj9¨g±6w– „wş{'¢ØÍCW¶ÌU%¿%—.ºõ"0ıôA½bS†;Ø ¹WæÜß4}ü4W]¨ÄYñ‘¾OIm[2›4Ü¤œA×·‚Pº‡°X×Ì»ÂÅd²Pn£*›W©ú²È§´öÏçŠ7¶ÇZC9q‰Ü&áƒ_b¦`QëùGd®Te¡C¶Ë³™ fD%²àç_ï³ÍKJ0šm;ÑR|P‚$ôs&““©c}:ü]
¬¼2,c?k«·Ï˜^®]Ÿ*húsâì ÿ*“¡È(Û/5ôA6JÛÍşˆ‡íGô·2EIˆËN×ûBâo<í(i10-ì=Ñ;ÆU*¢QºÕ*Â©I!¬91JÆ_)³Æ*?€™Å^0ÅR@E‡Ş-‰Éi§±&Œ>,ìuzÇ?PZp0nv ›ûhù\|¾Ø‘C`É?EJ•r&“ƒ Sß“Ùå‘ÅŸï™|¶õj ‹ÃD¿ıšÈÓÕ’èr@6|Å/…|+`#À}aßÑæ3	ánR€NİSEşç¥è¦…sÇÉ›g49¼ÌÅ×%ˆùöÁ©æ8C=ĞR:{hê· |ú¦3™¿¶b?¡©¼Äı“L¹˜æñ—ô4€æ¿3úXiÀ€Æ:>=¸¢|çò=Ù^}¶cËÛûúÑ›T‚ïX4aUeÎıAµYÇ+ò£Y¦*ÅQ9`H+¬¤”_¯ _‡L‰Äf””l¤ÿDªV’Î±Ä_
ƒûö8Ìäö¯µØ¼\rj£MòÙkŒƒ¢©$Åd¸w›ïF„Qähé§1”…†Ê  cOÁMÎ‹“šn»etU’Hbõpdï¦{öÿLB\½ş ÍiËr|8a­½ÔF¥‹«¢ı)ÏÃ¼©1Ißæ}p†.ë00PÓÔÕŸæºª5EœÙ"­‚š=İ¤4<Î,ñt±=“œ Æü>“¼®ñå,¸X´ÿ%„vÿE<€»®õQp×*Ó/ªv÷3Æg›ĞV¶şlÍU¹Ûe´5;ßê¼ÅFÔük²¡TÑÇ©ÄŞ=æ‡‘Gbî?èù9ÜÁG°¦fçé,6æØgVözõdñiªK?Ø`¡j¿ğ0Õ	L£• ëşC†jiÔa±Şfq
.ˆë:ÀÆ.øt$M’°Qm9ÌœOÆxÂG—!›8éü“óšáëWÂJ…?T= RgŞyº×.fPƒ¢ Ü!g‹Ëßìïâ0:›»eîëûJDn]`Ó®$Ôü¿ùˆB8gp‰Í%åøŒ×U‡†ËÅ——·;ËÙ
/†°øfÉ MsÃ˜>Jq—çküÄ÷ÃÈCË™˜^äf8 ñş˜é®°1bb~¹`7ÿ€vy¼‰ªµºç!Âå…ïŸ‘>m%Åñ+8Ä&óó;¾l+˜Å¥|Áôûƒÿˆ$Íàe¥ÁŒ¬<üéªk»oŠ3ñ†¹´İ
ì*ã0ıó€XWÊl ƒ½kÊÓØ--J°Ö¨h/­êTgk Îlë1,mÃ"šs×ÊG.6ª iWdV=çlúèõªù ,,‰¿’ZËé‰©p¾šĞ<öwÈs,®U¬OŸR»·¿‚ X·!p‘zÜyı°5'_ºiB"Š²Ia;A{YZ_+Ò5ôè†
ÇD×&yÆQ2ıò9v'émÍÓ¸Tşş–^îø*¦ıVbP@g<ÕÃ~3·Fhkd9·­Kš‘²¥èë. BT}‘¤“1º¬Îø²4†œcı§¢„&Á°À}Àn¶;Çí9ı?q!læ†–7ş°…Á±‰o[CÁïWtôDÅF¥RGŞO‘ØíÍÂ#=°ŞÁ—ûJ"sêÍw°Eh.Èî©á1|€Ù[°	Í‘€á–iÙ,¼èmÄ(A2t>_ÂèbQÖÍ¤Îe$òĞ–_r›ñ´*ˆŒÄ˜|¥v#¡Àdùæö…{š˜Gø\j°£áÏ‚QÊpCu®{°=¿‚™¿Û†ù¶ƒ+G;b†ªÉ'Öî<É¯ŸFÛF`¥‰lª÷ì‰ëŠ9ÕQ¢‡'Í#&+aŠS”§©|®ŸT4ø’'ÓXz0Í7²]d]&I.–5Â7z¥pgõşÌ<‡›e‚ *ŠÔF9WŞÈ Œs‚‚ŸitÒ3å°Atµ¼à-©Ó’°Ü2ZSı®LG\‘°¥Ä>¢î³nŸw¦9ÃÅk?Î«ÎS¼ãoùÜ¶¾A-åz#^P~Ti×“	 nfÂ@Ó®tC©¥³ÿ…ø¬ì\ìZJjk{Š„Ã“ï«v²ùNíèYn ædì”mTbù#îyf&ø'"?Ë¢œ¥İ<PÛø{;ğqÏø›Ñ	_™Ä¥ã^‘•[Ízí"3O´”V1*yj1Ùø†ó{å Ë‹òÙÛ6øØ!ÙÍ8;hô2üêŠÇß}gbp$,òSÉü,V‹˜õÑˆîÙ³}c~ÿñÿşj—™;­äQ3GOÉPC‰ŸìYëEºÁÂ›ES]d´f·‡ŠéµŸ¡‹î6˜sd¤#ár;m/¼HPAVµñØÏ&»RNÿá›ıIe‡é™'°5²Ú›÷AZ±Lø"R•ş‚Â¡™ÁÙ·Aí
?´Œ×Nÿ{g=Â~—uŠ¾Ûüé»´Æ¬wÖ[‘=(31JS
ññJÖíĞj ØéÈ~fC¿&İhèĞ«‰ªkû^ó=ò_Í®Ø6½£‚±JË)•Î‹\Ní;¾¾:e.²Âª7wö«*÷%Z
Ç8&ç·Qs4¨âß\ÚŞ†A’Ç…kA·‰„˜@}°¿Šj±÷h§Á;Ÿô_ò=T"-#„§!˜±Èİ¢Õç^{)öÀ@çûÿ¢=¨^ğË5ƒ·ñ	¼ı®úÚ(IÌsq„3e ôÙnt¸ q”ß¯-Bä¼|Xİ4
Åk.¥æ¦ x—@M¨´.~^ülÏ™JhÌ$;¨ÔåNÁ²~ï5ã‹y-ŠBÌÇtüÉ+¸ºÛŞ¸ùcÂ‰<<o+i	¦&ô·hx@87Äë¬Fy÷¤´CqbÀ-ø¿T"|Yq²…Y®%Î²æ›,*Mm§Gœ°õ*ˆœUVœoyµ©Gl®VÕŞ¿Í¢şÀ`¦ÅÌ>Ö›Wá[Óéê‰tZ7nl`ä¦÷4SOD»­ğ¾é-¿€<Óy¯„ûOi‡SÈOço7¬V6üxAm:ä<î¯©	ø2ëìÔØ«	L„pñ"3 EŠ©[ üA‚Çöóèü*yôğœøP$å·
ì£VÑ§6†hˆäŠèƒŠ™aé)`pÂYKŠä#6Ã_WóL|‘˜H\DÁR˜àÉ†åeÃ–»«¼c;”}ˆñÓ.ş8T™[VÙÈÁ×Ù3«òF@¬‹Â=Õ*òÛ´µø>Õ‘Æof1¬ÁU{ğ~Ù,…7¦a³ÿT˜aÙZŒKãÊAvSïh}ŒÏW6Q³°æòÓ¨ÜŸ£×Å<îßDXäÜ•6 ‘gÑ°4‘%6¶§‚á¼9««ªÛ•WŸ pªŒ!µ_–nj&my×˜Æ·‡l<€óm væ
Ût»1(ÛØ7şyÉÌ(Ö/«‚Ğ&ª¨k‚PÌÙuE=Aä™UèU{ºEÑÉŠà;_£ MÕĞ©h­©VãÙ‚æÅ##,Z¤ôÄ•eÅÁ´À$¹5£ôù¦™W4yéßó	}Ì÷g™4Rjd-Qr=ı€Òç9¬Rms;ğÎ;"¿Lô˜Ò<ÚBĞÄúÜP‘ò¬ÃãZä[ØCåR#Z÷4hŠwÍŞ¢Èwé®óbÿ‡öïM±opWbÿ~"R ÂÚUüòK%àQ—óAôÅXÜÆÅ¥½Cwo[A¦^ö«ÍkÕcè:vª1Ã³aVMC»ìB%oûŠ²6¾'ˆ“ªb)¬;ıÙïj£Ÿ³s=Àø&®½´1‚§µ»W_:=ç? r%øs?1“Ø°x™IÉBáZpä,6k“Ü£g™¼ñFÅQ²ıLq|"úë(*NÛ ß´‘ån‘	r«zaãOe‡@¯é^>„ğ~®TÇ{*GŞXª”£»ø”\`h¹H¢2êAzx…Ä^L÷ãbîû±ŠÂRNö¿K ‰ÄU©é›2Ğ½½¨*ÓLµÎ(ÉpÚ2 Ø¹î	5ĞF¹’Z¨UÍ_z†WÎ+ÃĞ)ågpî¡2æ–&7VJ¬¡Tûß@ñ‰ºM™ğe‘5Û9”]û¦Å@„	î› BVÌU(ô5`×¨ÄSB})‚4‚ÁN E3½j.Ø,İ[õ5Ø|-–/™‹EÜÈä÷VÌëšŒŸïX8Ëi !bóK­wïŞõ(¼<ÊAIÄ«¹ì=2¿ä(ã¶şÆ7ÔH-~bCÕ’~QşŒvÉ"»toRkÒ–û**}Ql*¤†Ö‡´H0
Våßx«6©> ğ‚H…ÁÚ÷#;"‡xW~ICsç—,]([â£¬Ş´«?~ÄKïHrÌÚ´©òª‰şªô]¨´?³Ş9`uyéD*U£lò	û²“Îu´Í!Öóè°¼ ¨ /ï"Ù
Ôõ×âTŒÍúÂˆYtÉ-KøË7¤ íÅUà#6²ÕeĞÍ?Ü”àÛÀ©h‰7ä–KaÌĞ¨vEáe“Xÿ‰më¼ÏìÒæ1k*ã11Uå“P—CzÚÊôv<x¬O±¬*Ã¡ÑÃì&ö[é¶^RˆÍ¼,óÕA ÏçªH(5R¤Ì¬Šp/¸”½?‰©™§ªS9#JÄh@Kopêö1ÉZÏË¿tÔVAug^.síE1ƒªø+ù¶!Ÿ®q,½£‚¯#¡³?|\]VW¼Yƒ»x? .ˆZ>ğşÔ“0sHPÄŒa!$yŞJÆE íÂnIb#a†t¼—¨ÄÂÀ¾¨SA]‘¤*…§d«?hø(µ\I÷E?	19B×2\´®'s~`¢wŞCõ	ÛıÄÈÛRœgküsŸÏ¹§kĞY¦¿´:<¹g.;PŒÚ¾MÃ*°P|Ä4`@<Ç#ßtºGgÓc„	ÒçQÜBÁÛ;!Õb’²î™ë‰Ş¼Zİı Zp#1“Áyˆ~_m–ÙjL¶Ä7ûnÿŸ$_®Jğ;e›t­êõ®BµXnÉ:\è÷v~ ¥{1™×ÅAñóŞ†DxD¶Ÿš:¡P÷KXCmïûTJİ”±Hİ}0 q¥eäçïlÎ8T2ËšÏuÌ#$¸Ù2wÊ<ïBÕ—çC¿L$*©?â¾,vøº'H@‹OVªTÃ§móÅ0«W^6PDÂï•j¾’tŒïcö|…Ş$Ô³ÌûŒŸe¹lë¼TƒÂjèÃíÖ¹rŠVà¿9œ\$û=„¹±•m-f²x»,š^´öö$©´å~¦’“O‹£ĞaÁ—„Ñçû‰rï‘ñúP´¨]cø6ïœ¯ _ÆÀÀ‹ÏM‘ı°…qbÃ^F&¦ÖLwæp†®óíà%İÜ~P/}Œ´uô_Œ‡Ôş",î…–2^«ë¥ÕCr‘¯¡ÆËÎêR4À¼Ò|dİMğ1}ı¼±‹B‘Œ·şÑaI…ı1]9*9¶òîÈî'aø­ö™¦?("Ówğ}Ÿj{³£àhçk!>j””™}Ï?Œ(Ï}=·ä@‚RÒü8V¢§¥ˆ.+—³kô%n{âŠûö@cÉ¾B¦4èI¸o\á¾{13°Ã—£ó¢§Oç°´0¢>aÇîU+ï[ëÑª¬(:"„bÒZá)Æ–DE^g‰å¿&Çú¶– €ıçó¸ò6m"´üÂñxi+*jÎ“ù’ÇQ>>¢jé6½™DY© CE[cèÖíˆ¦»oš¬ô6ªĞ+lx£¢YNˆÛä”ıÈ¤?¬õU Èz²Õkz²İ¯
Üö'º4±á>ŸUšRƒ1û®Ô8¡iÃ¡¦Çájñ;"Íï1dÈÛ’RxÍ%
˜!o¡¯Ü”dbøJ€		ó‚‘5N“å´B¦=ƒf½½Ø„´[$-Œ1™#²uæÁüBå­mÙk[M‡›,[¼&è |/´cRnz‚êâ£7®lwŞ¡!·š9oøFğ¡ø:²,ËvÉV°¦ÒÊĞb·6Xa[æJ:.0æ¿Cem';eœĞµnl‘¹MìÄ=ë$¸8sê®&’‚5S9j[º¥&"è§K¿ö<‡)á6yÆôşÙlu4O¯:ÚÚ	ASL=dås¦’4’x¡ã1¨§ä°cd/ıâıYitC ~îE’ÄÍœ­f÷!ÓÃşß¬¸YØüÆ³aNÅÃè±˜Œƒcº½(nŒ9¼ãm:1 ’¹õö¨òQŸx/îcnm_:ô‘ìIêÛÜë×~L‚ÿ‹hä÷ë½şøbÃî”’IÒ½b,$ïÀCOZcO²Âf/R#+BïNª"ØAûŒ¦ï”3³\Fr¢~êÖ¥ÄÉ¿¶….Ï®_ø1ùûë–T?qÏa#$WS¡{ËÄHÕ0‚<S ê†¾.kA`X4°£Å*Eß‡gîpÇ¸–À|tû'Úøæ z¸DŸò£¸8ûÓ9¬s;Ö/2tºT¤şT›¾ÓÎÉ×jt[Nôµ=áëT@%XÃ‚A2Ì¥çï×A×~•F&»©XDP£FG	(.>¼P”6sx•(Óˆ»±ÓÔ‚½ÕM1¸¡•ò@ˆ~Ô—vë®O*Ä¬<ñõÎ¥>â^ ÕÌêİ'¦óş…1jÔË'¤œß¦º¤yµŞOàç‘õé&Ü)ï+]güÚìÀŠOL~z`¿T²6Å]¥åĞóçÕyÆOŒõE/¸¯ÖKë¶a¨ŠÊü?ÜÆ!Kû?Ú™$›r6ÁfO'x´ÏJÛØûêì>›|ÍÌ)¨ÀŸgiÃ(ÇúİĞ@Yı÷a6Š–ú½wATy£ğÁáaúfóRE]İ÷’WWJ«Ak® nîğ€	?ğ3|†,ÁU30MÙ	“ ê1ó~†6üÛ*6‡ÅL}i«Êæ2K~Ãâü2(>­'¸7ÃÖU>M&í# ¾“ ,†·àvÊšv‹ã|>	
[yµù[Ç)Ì$(©îJ.±çû‘«=wÓ'çûj…²:èéÑ{P¸íF<S(wKy©Ç·ó‰Ùÿ†ÿğIgáy7
³<ëJÛo³!ÿ\1ïY0‹ì}O=qEnZßg­âhm‚Šş¿ò¿°$g*©øa†o>é¶4àÀÂ0yÑ5¬İŞ{Ë
Gšh¬=œ ˆã¡¸dTe j
…<ò!J,§·rjiêX8<:<_úw”Ê•
ûB;Ì‰ñÅ/ÙÿéZ|Ï>np†=9Ûöc¸L‡E{ö7õnMÙ{Ğ¥òE:×æ‰fS_LY¢ö6ô¸¼üUlÅg©n4¯¯,ëéôñ(Nà¦”
¼œæå§÷ù˜çÌ¿ØÓ»ûÆ€5€2š‘q¢¼Óiyj©B~WR*ñ?Ù%wt'fCÛ­Gdo¤03z:õ1M…“[7÷r4Ä‘@ï™6Ü~„‡œƒ¿×‹;
~	k³oó‹ÈŒ<È“Élex ¿pc6q.Ú#faà¾»“ë7Úxj.£:¥8±RßÚ†­µÜ-–® Ò½ä òWsà<HµLÓ±Äı2ˆ²İíT:¯ÿP€½û‰ÉjÜ“:æğD°hö‘MÕ¿¸¡S*öhQ2ğÜƒC¼(ÿ¬7o—:\ıW(Aû¾¾É\íRÌ}B‚§Õô¹XR]¡™i2xàXş=ß"˜VYx|šysÎE§Xù‹Ğ?´û}xæk2¾î’ı¬À_a%Ÿ°Õ×8\øJK…(:æ¿‹ëàt{åUô*““ê·H“È,
j¡”\—cOŒ¯[Ìxwg2ô öcÓîÔ\QÔü#î<O¸çH¦½n0Õ´m™£´?Ò[/ßÕ1¬“¾fyÑ(pÁò:Û+>€Æ 3mµqÜ€"¤‡Q´htË„ÖÖê[8ÄùÕUs†hN#aäı»Øİ„Ã›ê–&k‡àæWgr>c¹èªc.®>`vÙÂkÚCºµÓFƒß¼X#Ã]UÚ3–Æ‹ÑJj‡Lö«Á–-D9/rÿ$h>ê:¡Ğ‹ï2fŒN÷üB7ÃüCÏj€Îzâ`9[x!ä16zOgí‡ËL-â¼Í¨«ÿ;z‰€\ğÿé££Æé…|®[fè¦Şv[ÔîkjŞ6ÎŞ9ª¾ÑpW¼²
Gä*Öa`5®I½ù¶rÆ4àî«°ï®üÍHjxÕW°¤VÕÆëÔ7çÛ—c½¢÷÷qVÌr‚Ï™i0Fs€•\Pª|`ÓX4±K–õríån·ŸÂÔ·ê´!tø­ÀìÌ•vtdÜíúe»ŠÉÃºB6Ş}jè¢œq¨gµ3»à"£•$¬wÓ+;€àíƒOVx2ˆ¨jõŒÍwuc÷ä¹¿ÍgM*¬šFW^÷ıs?G;
_Ò£OŠºê§©]±Ò¬à"ãÃ:‰W8ºªˆìƒë×4áx§×—PH¤4ŸGâ]¯§m,*É|iK³ŒŠşbñd¦Eß˜7Ôë§X#¾Î¡åßF¶ß«íåàzÌXÌÔÇo&c0¸îÉé3%ÌB(Õ‰X	¿Ûİ¡Ì·^mŒ·ƒBı¸¸ ¹/úÎ*NŞA; ç…C°EYZ¸†k“Üu.µ+~Í¼	ªQv«ƒ=™êšàŠú¯Y’Ë‰ØTÜJƒg×7¦Q'
y]iÈ=™,!–o2EE 5Áï Zu+¦²DßSŠÛP
	Ğ’Šc§4ÌÑª£¡}åONÆ)ÑÍJû7¬wà¾ç¢ó¸¾|'™Ñ^ü&\ôen›À³ †ñ‰e‡SGN­¶èäT%®ÓÙ&ŠgÑ¯¶g•E­EúY}¾Kñ„&€MĞ‚û%²[ràpÓêôl½#L"#*8uøÈ.<ÌŞÜO¯&¡¦™Æ‘åTVì‡¤²7Úe{srB‰Nêâ8xfê½Ò(ÇAÁONVŸÕó­pIJºD€réñì¿G0ñJ¢½BP¯ĞrÃO<Éìù0Œb<ªãüs2ÂàGçëˆWEe·„¸'Fğ“‡{iÊ{š:í˜ïL¼BõébqL]FXqR‹@9Ï‘`ñƒƒ¥Ğqˆz¤=<ÁÂ„‚A¼“RøM|–‘qşN¾Mïşƒ°ºìW#~Î-¥lM‡j–ZÂ‚9›d„SzÕU¬¬ªĞ(°¥C£µ%G‰ìd¬tÁ\^æÃû¯øİ³aµî·l›çĞ-Nª°,‘‹÷º‚çu¢à‡ÚPœi¢ş™&,6aˆ?Q×0ÛÆÅˆèÇ`c
æMoQÉ8}	ş’9À
u§(^òÕ0İ­x’G(@óã{€®Ó÷äx¡Ç°»x(Ö1Ùn½s:*UwœšcñB%f×ÑTR8™Mß@i}H]-†h×óÓ<(«>ûÓrÆşvBQXÉ€õÌIµ‘‘úÁÄÿv®uÏçJ4lHğo³U?ôVx‚í–2XîAº®BiK4PÆƒ²œOÍŒºĞ†{¹¯ø£Fk×yğ»cœãZp¶¦Àˆ–©_2ÁcÍ}oÚm1ĞÁÜêpç>Ñï÷™ùØqÙ«ï	h¸Ğg—// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

static unsigned int vbibufs = 4;
module_param(vbibufs, int, 0644);
MODULE_PARM_DESC(vbibufs, "number of vbi buffers, range 2-32");

static unsigned int vbi_debug;
module_param(vbi_debug, int, 0644);
MODULE_PARM_DESC(vbi_debug, "enable debug messages [vbi]");

#define dprintk(level, fmt, arg...)\
	do { if (vbi_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: vbi:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

#define VBI_LINE_LENGTH 1440
#define VBI_NTSC_LINE_COUNT 12
#define VBI_PAL_LINE_COUNT 18


int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.vbi.sampling_rate = 27000000;
	f->fmt.vbi.samples_per_line = VBI_LINE_LENGTH;
	f->fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
	f->fmt.vbi.offset = 0;
	f->fmt.vbi.flags = 0;
	if (dev->tvnorm & V4L2_STD_525_60) {
		/* ntsc */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_525_F1_START + 9;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_525_F2_START + 9;
		f->fmt.vbi.count[0] = VBI_NTSC_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_NTSC_LINE_COUNT;
	} else if (dev->tvnorm & V4L2_STD_625_50) {
		/* pal */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_625_F1_START + 5;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_625_F2_START + 5;
		f->fmt.vbi.count[0] = VBI_PAL_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_PAL_LINE_COUNT;
	}

	return 0;
}

/* We're given the Video Interrupt status register.
 * The cx23885_video_irq() func has already validated
 * the potential error bits, we just need to
 * deal with vbi payload and return indication if
 * we actually processed any payload.
 */
int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status)
{
	u32 count;
	int handled = 0;

	if (status & VID_BC_MSK_VBI_RISCI1) {
		dprintk(1, "%s() VID_BC_MSK_VBI_RISCI1\n", __func__);
		spin_lock(&dev->slock);
		count = cx_read(VBI_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vbiq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	return handled;
}

static int cx23885_start_vbi_dma(struct cx23885_dev    *dev,
			 struct cx23885_dmaqueue *q,
			 struct cx23885_buffer   *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02],
				VBI_LINE_LENGTH, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_VBI_CTRL, 3);
	cx_write(VBI_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000022);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */

	return 0;
}

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;
	*num_planes = 1;
	sizes[0] = lines * VBI_LINE_LENGTH * 2;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, 0);
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;

	if (vb2_plane_size(vb, 0) < lines * VBI_LINE_LENGTH * 2)
		return -EINVAL;
	vb2_set_plane_payload(vb, 0, lines * VBI_LINE_LENGTH * 2);

	cx23885_risc_vbibuffer(dev->pci, &buf->risc,
			 sgt->sgl,
			 0, VBI_LINE_LENGTH * lines,
			 VBI_LINE_LENGTH, 0,
			 lines);
	return 0;
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(vb->vb2_queue->drv_priv, buf);
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back to the start + 12 (skipping
 * the initial JUMP).
 *
 * This is the risc program of the first buffer to be queued if the active list
 * is empty and it just keeps DMAing this buffer without generating any
 * interrupts.
 *
 * If a new buffer is added then the initial JUMP in the code for that buffer
 * will generate an interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
			struct cx23885_buffer, vb);
	struct cx23885_buffer *prev;
	struct cx23885_dmaqueue *q = &dev->vbiq;
	unsigned long flags;

	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	if (list_empty(&q->active)) {
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		dprintk(2, "[%p/%d] vbi_queue - first active\n",
			buf, buf->vb.vb2_buf.index);

	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
			buf, buf->vb.vb2_buf.index);
	}
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_vbi_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}


const struct vb2_ops cx23885_vbi_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         %İ]1Ùºã^"«ÜçÀ‰òPÛ,Lå©QÏÑYƒ j_©7\ÏaÒ£<5l¹Ô2œZV{j£T«š+À"`Ò“ïİÁä.Ÿ¸aJ0ÛxWcL. sçŸâ‚(°a©WÄ*Iã­^aÊº(Bü€É¥©wf¶îÔ…?ÙïÁš¶ÌşºÍwœAú-DÇ¡_éSƒWpÓ<YaÈ™y/ôñ©$„sÂjgyb	*O`æˆü4x- "‘ı–Xrèà1o<º>ë†c3´èÌ6ÌóÉàVŒ0@„?b:bé>1›Æcˆ´[ÅÛb=Gœç¤ÇáÃ-ŒX4ç=3ShxÁ-Ïf¹'xh%ØÿhQhæa@Ñ ä¦vcÆ
gi…i,c2 ÀL*ÚseÍi
Ÿ®Ÿ^)GGb=ÿ@k«FäôÎ<a¼†7ÄÙTnô;ÔùVcg¤# Ü|tköıÖe×âÕ»P›¹A{ÍKÃg]èOÔÊëÌ¿tUòŞ,UöPÿ–GÄë¡x²Ã5%äyş2Ú|¶À<Oºù;U”iŞæ³Ÿê—æòÎ`ÎäÊİ9M÷O¶>‚F¶Ç“pgõ5j`¶dq/iêñ¥:îi|ÅCî¡+!Û†ÆB?xqóÀj»ßÃp–wdr;-Çùé!«Ê]ĞÃ^o3U‡ó´ï^¾µD¾>$”§É¨{p.4¤R6pêü¥¦Ë!]s„YøÜ*bjjÁÚ|
Ô?1Lê7íÍ‰ ÏWIN'TìÈ»ÀsVôàiEËæté¦ÂíZy¨9­wöÂc¹ÂÑüÏ"$­%eï*äG‹DÇ©ÿ†š‡ˆˆU®[‘C(R›|Â£ïyàë½ty=ê¦ É£&ğödwt7²u½E5Ì=7rŒÕ÷\8k*YnÿÛœ ˜µ@ƒS^/b<áËh›º2÷a9
<ŒYz=Ö¡~£Šº‹ïÏš6ù5[b2Ì¸€·„5&KL¯±×¨¿y†Õ|y,fL—ÌvxàÙcWöş›U33:ñWĞ
s0 l.ùå7¸æ¸ãiŠuõ†²® ú/<lí¡v}İı>q$óÊ1À»ïÇ&„Ï5?µv"%­|S…Õİ|w$¶ã+,¸Dõª‹•‚§¸&í!_×ZğÀOÑG ’)oÄ"`Ã"#ìoÅÜu¨Ç«¤ÑªWÕ›5¬õÑRåe×ºØÑ,ŸÕ•A´
…Ü‡a—7¾.0/7d„>³&ŸN Wî©Ä)m´¤u—UšÀ,)×¹Ü{L|wÖ7n?r™/n>Ünö{»¥™[ó/XW¹®”ÂÌje/9yû(ÿŸŞP­Üo‰ÊÕ)ıyNƒxšàÜ&Hşr	açdtL´1ZİM¿0¿#ÀgK¶5aİ•gcÏe1‰Áò¥±Úò]½®é³W§[ÃşJ¬Mf Á„gÇÚøGò›V,´~Ïè òĞ;Š%J‡Õ£Òp€á~ä:ªn³øÑš¤s¢lÒò.^d¾†ì4J¢g.DÑ¹iÀÕzœ7¯ZÂÜ$VqÒı€¡ÎÙ…BK²ë)ˆ&ÒğâQ¸Û`E¹BšÅ=;9ü"Õ˜ sn `öêzõ¹[}#<¡ÔyZqûÑ˜ƒ'^YZ§İˆ³É¢ôÑRğ ¼@9S¥¸<~c½Dí\TÓW½g‡~xóGÊÛš7RM·¤×r"Ö„*'¦f-ÍU'í.óË\H×µ#±şO:ç?ÎCµ\äò¿ÏÛËë¸UMËsD«hÌ®©ñQSWú˜]ÕêÆĞ©;?šWÁ«gÙiÅ~}İò¥Ó&ï^1nÆ\»)Ş˜1p6ÈŞË‘VR·}ebIÎÆ]g˜à*8!’a&V¢ÙgT»7±1©¦°öÙO~$¾oQià-
¬˜§GxË©¹‘• $í«®Ã¦»h@ûëÃ™Ÿ^w%åN" $ÕÌ.å~ò¢yüNaå0T0æÊ‚–€>İHK=Gƒ“ŸK*kµB_Æ™m6¢yl¬’YÇÂíÙ¢Šî/‡ëÏ+³Ûs÷äT37ú5öŒW;(LÇ*ä(³¹ôìœ†ÏØ£·ŸÄ¶qo4¿ ­ün¬|Ò%f¿´Î×1aìhÍ&©A!Ã[èÆ]¶;/>Ü2É¥aˆtòÍä™¥ûƒ÷ş>lF»B½ƒ4ŸŒ\ljÛb¬	ÿlÕÌ­VLŸÏ«§»GñÜÕTDÎTå[ >Dv@ß¿–_A®,™šñM	k	}“KT!ø¯å,`ÒĞå*†£°z”„'1ÇÊÏà/œnØ7«ö5›_åî"·Œ3²Úû?³ş;àóêy(ZAe{£:UštàVş8ôV!^‡@”¯	–NjöwˆXÆ	 ùtLäRÑjò5zÊ
_¹uP}ë9¢‡™'±Ä¥¶û½×tIñkÃPë>k~ 1AOlçÅioc ¯Ğµ¼ş{:õ¤9å0‘&^ÿ #çù è`~ÛcªÉC¸j Ò–æi`ÑU!!<¬ı’–#åĞêãb‰îc[>•ëHv+Í¡î–UØ­vJÕ%|†lÒ`ˆ32]/jî›ôL®~| :9ÿV)İ<?¡Üw{]Ğ9ŠC÷İäU´ØßøÇØu²à›?çêÓV¹‰³ğ`ë\¬”0ìë÷ÓüëA6½ ‰ò/°{™B€áº[¯@KÃïÕvi<’¢··m«†â°MT@Mñûôt÷Æw
WD®EOPI{Ì´& ehº_q
âÓX²Ä•”Wö?ËF.&£Qwgkğ^«}2ìP¸31 9õGs"ƒ«…£`n®œıŞ®Á|æ@„LYæAn8\™ñ„Èœ'À¥F¿¦›æIÇm/²e!ë«ò‘÷ëÁşJ?ÍG|’¤3…şYJ«Ù&ñ¥#gL˜[h§¯ºQÛ\•¶Ó½hJá˜<ÿÅÉ&Ğ"X-IS¦hÃ!¢?´|##k"§ø3h—Óq¹`á†„İ2+:F½Gl/†è‚¹Iƒ¾+$Q.»¶0_S9–UŒZzß¥9‡ÀØ{Şb)i9ÂRºz]+û“Ój%~¿÷f	.;0’,NŸ…kÂ\u‹çZE¿ƒÚz5ÈPÙR8mª¶4dsEbŸ¢læëç~ú¸YiM§´fâ+ˆß˜ ïé,|éT5Y•ò…8ûÒˆ¯oÒªWÜoùPÙMYÑN%¾ì¼Ç°G…e^‡dlÕÿÿ~öŞéø5åZÃ©WV3–ˆ²­¡¯¯Ší¹Ç½(ëÓ”]ÇißFrì5eMn6¢Nm&ÈÌŸuí wş)rÿ7®¿‰ÇÀ9¹,ıT¢DqS×ÑIS”&èßÁîè°°Wz®yÉ§&jUµ™„„=L©Ğæã@Ì<®¬ï²xLbÁÌşs8¤9Íî‰E…Ø@ÕJCç×·UfU”zê@\‘Ÿ1…×XÍi×¾.iíŒó¸2óöh;ˆô¢B@H—FÏøùü95®¼Ÿø`yZµ%ØB;Tèßä<¶yıİ£ÕÕ^b“4â°¾3 ÿ"¦/ŞÂíú€SÆ,#îºú@2ù á=%ƒg:÷wG-I<Ê \Û¶Ô‘ÔqŠê|µb2&;rÚöJç!¸½â‡"!1„#–Dç%P¨àIáôë%ƒ`ŒmíJ3ò-Œo6¹å"gL*Í´²GjÉ¦î…y+ŞîÙ‹æ@:næ&MÕ“éjÆhÓ •D³í(İ…`êMí=“s›¦á\ÃíPÍY[¨iÉ/âc²ä)b§Z~Øb-WjC¸O·ñµ– ?ãSƒYaö$`&¥KÎ…p®ŠXf›ÉÄl·a,¨u<‹Ò.]†¥CãØg¾çé7SÉ¯œÌã²Eàî¬wµıeîµkó‹
JêkvòïŒç~Ã0ò‰Ñ©–+xEöƒkì9pò—^‹;T_ë=ïşêãøfà90?H›	—ƒÄ¥qñ‘ïjï5<ş{’Úì¼‘½J1õiLİ³.S—âÏ™6]@-‚wú¥ÃgŞãÅv=jª‡¤¼`_¢€Ï¦/_‡Şï+ní*ÀZıÿt
™ÌŸÿ—ğó-¿Ó®Jú†m<D	¬Ë”‰ 	Ã¡·}@1œBMòø 1míë’šO¾Ñ§Ş°o÷2©ÖFQvWvØ- {Â9:à(öMóTLÜ(ıÒ%ÏÓÄÊÄp‹É¤º¿]íDş‘/û·ˆ*¤zz4«·¦{åß9^ePã~nà¡‹†s*1
œ-rpeÖ»xÓMÈœü´i=¢ˆE=w¨´à&ES*Wã:å4ÿ[=¼&V°(7*a(Ù'‡&Xri?¥‚ë3ÕU}Ò/Èë“‹÷Hq™ÓÜaÍ„µÇõ¡Ğ£ö-U6”]I¹&ÍÈ`Å/
µÇT£¾Ã„Yàâ <:¶kóZ.r¹w3¦d`7ôæœêøs ìao"~ª<*˜0Ş?ËÖk0½&¥-—ªÿëÍŸòw¹dí†§=„a&P†”Eè)g¶=AÑRv’òšíÙäSsb³)QäIºïë¿LV°“44¥šzœ§7îñõüJ%5•£
åëø$î+IĞ0ºÍ¡Ò„êúzˆ* ¨öXtzCğ¤÷Çâÿƒ~Û­\6bln_—ÉõÛi¡PÔ÷ÜÆÂÆõÛQ¶ˆQŸˆÉ£­Tôë¿Mí]©Lqhü0|±îºğĞïXæs']X-¨˜1æH‚f/Â>'™8ùØV>Òt¯¿2ëÍ++\ÄÌ³cÌi0ÀHñ%V.ÿ²åùí§#’Y¨½y—ğö #>›¹'ë¸ìHj¤2+èdÅ/¥Íe]4CÄÔ¯ªJÊftÇëâùõğeF+gåøñE3­¸‡î„ÁÃÒ®bm—³p³cxhæ{µ}ÿbì5[Ş±ö•¬ÿ«aãğ×yNL\OA	£¨ÑFKÅéĞvÕĞ'Âiš øˆN¸×DŠ_¥
£:É0Wû-prÜèÀ¸95orüv¥³ßÁ2×ŒüCk$*–¡u³{¶Ÿ)!}f	Eƒ?n)kØ,®-RR­õ÷ËWG…Qu×Eíg8dÏ?+×Í0guŞ+œq5®eáı„qb?¢7¶á©f9pc&¬Òf)7§@Øùk–Û˜+„ë(–vof1èÂ ŞceN½Ç…c¤^³îT¨¯£ö)òã
_upÉÃ¹“S\D¼¡‚Få§ô‰/&!ÿ°<a5×œN-¯•=j×ÎÈX õ™õî¨­À¾
0Î_n…Ó«-†ÆjxïÈé™Šº,­¼ü˜`>êD¶‹dª¢?sƒ˜‘O¨œ2%ƒP M`À­k½:R?
Òò$6‡iWĞp%vˆá¾x[“°ÃÚß~åt|Áw(¬ÆĞXõšê²0SN‡éŸ¥Ö&x¼??´äı¯>å¬õ†µg`FĞ7Ù÷ø;m?4Lf[*Ïóç¯Mhƒº0fjãYçAáfRp…¦ÜéCôìMå9ÇÉô@ùÀjÄºª/1"wãÏÃi‡±z8°¬C~”êWKƒ_Ú•òa“	÷E4ÌÌs<pQh3]6¼Yß%@×A´Iÿ8ìuáæºÂïB-İ—Îì¥x"Zµq\G*$ˆÆş”arã;Ê£JZ“‹fü‡Njjoä±stÜ?‡FÈÿÄI)LÀêo@P)Qˆæqe¾ù ,4iàéÎ¡‘ÉÚ†¤ •± zæ­âò]­çÚ¨vœ|f½­i›ïéÜª÷Pè (d+é›²_e¼W {µÓÄ›·lô)N?”j-ÀÂ…:²€ı¨õ¨OY9Ç†ºº&ŒeUøÔÀ¥Ì`ŠäÚ4JŸùíüÊÄ7EØ¾ZTHøn‡KA8°n çÉ8ÅP0ÆÃÃœìèÅVxn{%á.|Ñ·cá»uSñØº*#Ï_#Všñˆ»!H4Ü´Ö