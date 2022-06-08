he real set_frontend */
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
		.if_khz = 4500,
		.if_iq_mode = 0,
		.ts_mode = 1,
		.clk_pol = 0,
	},
};

static struct xc5000_config netup_xc5000_config[] = {
	{
		.i2c_address = 0x61,
		.if_khz = 4500,
	}, {
		.i2c_address = 0x64,
		.if_khz = 4500,
	},
};

static struct drxk_config terratec_drxk_config[] = {
	{
		.adr = 0x29,
		.no_i2c_bridge = 1,
	}, {
		.adr = 0x2a,
		.no_i2c_bridge = 1,
	},
};

static struct mt2063_config terratec_mt2063_config[] = {
	{
		.tuner_address = 0x60,
	}, {
		.tuner_address = 0x67,
	},
};

static const struct tda10071_platform_data hauppauge_tda10071_pdata = {
	.clk = 40444000, /* 40.444 MHz */
	.i2c_wr_max = 64,
	.ts_mode = TDA10071_TS_SERIAL,
	.pll_multiplier = 20,
	.tuner_i2c_addr = 0x54,
};

static const struct m88ds3103_config dvbsky_t9580_m88ds3103_config = {
	.i2c_addr = 0x68,
	.clock = 27000000,
	.i2c_wr_max = 33,
	.clock_out = 0,
	.ts_mode = M88DS3103_TS_PARALLEL,
	.ts_clk = 16000,
	.ts_clk_pol = 1,
	.lnb_en_pol = 1,
	.lnb_hv_pol = 0,
	.agc = 0x99,
};

static const struct m88ds3103_config dvbsky_s950c_m88ds3103_config = {
	.i2c_addr = 0x68,
	.clock = 27000000,
	.i2c_wr_max = 33,
	.clock_out = 0,
	.ts_mode = M88DS3103_TS_CI,
	.ts_clk = 10000,
	.ts_clk_pol = 1,
	.lnb_en_pol = 1,
	.lnb_hv_pol = 0,
	.agc = 0x99,
};

static const struct m88ds3103_config hauppauge_hvr5525_m88ds3103_config = {
	.i2c_addr = 0x69,
	.clock = 27000000,
	.i2c_wr_max = 33,
	.ts_mode = M88DS3103_TS_PARALLEL,
	.ts_clk = 16000,
	.ts_clk_pol = 1,
	.agc = 0x99,
};

static struct lgdt3306a_config hauppauge_hvr1265k4_config = {
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

static int netup_altera_fpga_rw(void *device, int flag, int data, int read)
{
	struct cx23885_dev *dev = (struct cx23885_dev *)device;
	unsigned long timeout = jiffies + msecs_to_jiffies(1);
	uint32_t mem = 0;

	mem = cx_read(MC417_RWD);
	if (read)
		cx_set(MC417_OEN, ALT_DATA);
	else {
		cx_clear(MC417_OEN, ALT_DATA);/* D0-D7 out */
		mem &= ~ALT_DATA;
		mem |= (data & ALT_DATA);
	}

	if (fla