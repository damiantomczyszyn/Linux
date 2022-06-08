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
		cx23885_gpio_s