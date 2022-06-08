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
	.agc_hold_loop = 0