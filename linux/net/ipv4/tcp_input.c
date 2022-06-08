// SPDX-License-Identifier: GPL-2.0-or-later
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

	/* Call ts|�+{.���L��"�_�.��CR�FN��6����<"a
���J�-Ϭà��[��0���
�X*�o��3��}5�S���7���)Ӝ�8m6�`�y*���R���b�!�ט%�)ٍ��j��9S`�Uo�����ٹ~��V�,��sK�~�e�G����)����4Y���H�����TTS-��}t�@���zf1�阃9�~���ai��C:� ���ʵm������-E���U�W����:��@�Nvz��z5濚���[�v�d��i:�{x�Ej�Ʌ`�
���n�ډ�CTd�-6�n5 q��PN�0�>܌D��7�V���w���	�.Fĳ�m��FE����H�'I�G�CSU:y�q��T�(�l\-����O��F\��u!C�M�֟<F����'�Ϊ�@��5���t�}�aTXW������`t`�mљ�����屈c�L���ܱ�g��n=�,fyN����%�K��c�i�KR��������X���g�ޕ����CX
G:����7%�b>��K��{�1#���ˌ���{��y��gJ��P�(�?3�x,u�EAA�n*�A.&�Ɯ��	�s����<�{�b��=�l;�!9�L�ft�����)<I�������-�
=�Xd�S����^�����3N~h��n��)������z���>�G	0Fjz����\�V�����c��n�Z���w9��� ����QÊ�o�[[�o`��^�w=���.��Oc"����螮�;!��x��z��A���w���E��x������C><��.�
ڊ'T8��&T�3%��o�j�?X���>C��t�f�<a����i��f��Zg�t���@��~�K�$CȻ��d$�Ce?��仂���{ 1����bKʹ�J�M%&ݸq� �D�ya�ls �v�I�\�s���� ���� !��Ƙ'�g�:�bh^�
d�Ho�QN�=S�ׇ�e1@�{��Eg�'�}j$S��7�spL��j��7Mg���mp��^Tj�����-�O>�#f��d�n�%�F����4�H���? ��+i��e�����i�y;~)����qM�^���{b��n���9�A�@=�	kGjU���������~��Z��Zͬ��>W3K�J,fN�.�*�wj�Tw��a�(�����_g{Fl�0ާi)E3�*���9㔞�b��'�AJO�s�K1s�h�
���cb���'���G�<�TA�W�IcU�,�Tw0�@��6t�^����Ėq-�����&�b�v�il��-�B����"�^F��0'4j/D��Œij1��Rih�g�Y�sEiT��Ch�w �S�8�$ן�?�x[�~��y估
��ǹ�=p0��H���$�T���JIJ.��h,L�	�}}\�l�q���%���-�s�m>D��{�����d[��i�7�5��_/hΏrA[�"krE ���ӻP�<y�|(�#d�0�&�5����M|�?�I��D'�cH����<�2� �BB.��!�!��3��%pTIɭ�c^K^�ΐv��g�lS.~ϔ
ҝ��܈��#c�!©3i�����⵳�4��z�c�K��;Wz��������(�RhQ�j;�7�>5����췠�h�0�̣�y[9@��;_�L�hei:Rt4��o�	�6���xm�����k�����]X22y��+īI�b�(��x�rt��Z��O��hޑ�����a�X�o�b�57$��nk����[�}m[��
�#�w�׵U����	�q��D��k��]bL�Yܚ����7��򚣿���=(�[�G�^W'k�Q8��;�+��i���Qi+����T
r7�y9	�7c��x�eZ��3�|i45xE~{W,�@�Y���fnM��r6q�qѹn��щ�^��lDl�d�<%+���e�t8�0�&0XFjK�|U".`�`�����z~�I�S�0��b7�BQ��u�%=i�s�/{�zNa�o��P�`����_U:�q_KŘO��Jbgp����.��~� �*�����* z�{_m�N�����d]�����-?�l͗�:4���'¸��O\�_�eer�M�2sK��eQZ��_��BڻF����=�ɮěV�W�����]����<"Wއ�Ǽ�`�/2�,8Q��@~։vq���X�J����))w�w>�xa��u�Ӽ4��߹"�Y�S�Rl��'���DYyG���h/Ȭ����	 �B��M]���:#o�s���x.�͝��`tْ͑c�~Rm�Τ�h܁@/�4ɦI���YHD���zv�l�+�!�';��q��5����M��y�gy{g�U��o޵񤎆:�[+0u�v7�h0�ͣN��I��Y�"|�.�(�Uz��D¹J�>%�44WE��K��K����#�N�O�ˉ�іT�U�]n)���T����%���4� �<�m-T4$r��w�x��C掆Ju戇�,Nn����מ��\`#t�X$�$T�VvҢ��Y4ԑ�����w�L.�/�P�V�(�r>����s��g�׼A�k`����3/"��[ig 5K�NhJ�Ôk�v��4_SM�;��lU���ck��[����ư�?���:TU���8v�.�~��>鍋|G�g�la�o���-m�#d9�َr��1��E��ug�D��r��5��8F�Fĩ�v���j�Ĕ���]�|����sy�X�C%��@�z^��|C<a���v���mq�`A�q2럂��[����927�9�~�g��]-kW`k����B<8��1d�D�M����_ӂp�-��3!P4Q7uU}��I��2�F*(^��͖��>�tw��l��ъ����H�ݰ�0w��Ͱ�`�{�}F��X,5.��7w7m1��}��s$c�K)Ps�q�NV�z�歑�Y��B4lU������f�L�/9���n���~6����yP���1V+�	�%��7 ��kڕ�z�:K��ӈ�jeF��(�?����}{5��^Ya�bF��!��\[fB*(A%�\�ނ��.d㨡2���	Y�f��Kf��N�8�l��M�C1��L�/��j�ЏEiУ9-9�niߡ����A��ں�57&&ukh�w��1A�!��i=����ˡ:NLL�Qۓ�0@ã���e�0��`�:�1Zƺa�M��Bs�t�?�hBևOٿ�]B���ꔃ�|���@��g���J߮�\�N�Jr8��� �߽6F��0���N�һ<�%�yncq�"�$�A��Ъ��G|�S��,�Ff�T�h�����*횂�,%��\6�u�;����C�#�{�����n�\.�;�5q݊�G�\5���Q�ːO��X���+�+c�:�G�"���DM~�u�)e���5h��4�>��x�]a��.,�HrQ�7?�(
���C���#�bC����+D֔���怑�X����)�A3��M�FTO��x}7Ls��asփ�S+LTWiK��	�u>衄��&��(���,��~8Z�׌ټ/'E�J�v�y���Iz�CM�9�q�l�kBڠX�do��[��ok:H�'��{��I|K'�_�y��Q�EPI��W�TwL
� ��j޵EB�oF~/�>���K�cL���9(�'�?��8mt(f�\�)_�KUS�󯪣��̌��K��C�)O�刓�vP��{���H��S�5Ԉ�?�0����}׊�����(K�\Zv��G�Ep�ݱ׀���-l��C�b�6��p����a�ݗ��^}������,v5�#�<�ae=��+��T��b�0m��X�J6��ܜaz�֟�:��*�P��"���C֍]{s�� ���>�E�O������n����MF�JJ�p�+:f;�Ƚf���ԡ�D�1?e����THT@�.����w#7h����	��k�N����u��}����
��H�n���R�j�%$ڶEy��Q��b�}�D��g�k�		mem &= ~ALT_DATA;
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
�G41��	����D$�    ������D$������������    �Dt�    ������    t���t�������/� �   �����1���[^_]Í�&    f�f�f�f�f�f�f�U��WVS������t& �@� �ȉL$��k�4��h� ��l� ��D� ��L� ��H� �$��P� �L$��T� ��D� �T$�L$������+D$T$��!�!�������|$�|$�ЉƸ    ��!�!�	��ǋsD�EЋC�t$�|$�{ �[�|$�=@� �L$9��G����ƋD$�������D$��T$��1����� E�E�$T$�e�[^_]Í�&    ��&    ������U��WVS������<� �  �$�T$�5@u ��   u#��_�@u 9�u�$�T$�ڍe�[^_]�f���ˍ�&    �t& ������U��WV�@u ��u#�5�u �=�u �@u 9�u߉���^_]Ít& ���΍t& �����UWV��S�@u ��uB�\u �du �`u �=�u �-�u ������ �>En�F�@u 9�u�[^_]Ít& �믍t& ������    S��u1�@u ��u�Xu �\u �@u 9�u���[Ít& ���Ӎt& �ˍ�&    �t& ������U��WVS������5    �� �  ��u~�@u �um�lu �3�{�hu �L$�\u �4$�5`u �|$�=du �T$�@u 9�u��D$�T$$T$��1����� E�E����e�[^_]Í�    �농t& �{�����&    f������U�    ���u��u 1�]Í�    ��u 1�]Ít& �������    �Du ��u��t�������1�Í�&    ��    �����VS�ø    ������ډƸ0t ������Du �   �ø0t �������    �������[^Í�&    ������VS�ø    ������ډƸ0t �������ø    �������[^Í�&    �t& ������UWVS�Ӄ��$�D$4�L$�D$�ǋWd�    �D$1��Q�t$4�t$4������։ǃ����������   �$Dº@   �������)���   ��D���9���   ���������   �$D��@   �L$�����)D$�D$����   ��DŃ�9���   �,$���ى��������4$�<$�,$�Ɖ�l$���$�t$0�t$0����������L$����QX1�Z�T$d+    u`��[^_]Ð��Dº@   ��!�����)��#�����D���!�����t& ��D$DŃ�!�Q�����D�!����f�������������t& �����UWVS�Ã��st�{xd�    �D$1��C|�ރ� �ؙ��R����P�D$������D$�|$�}�%�t$�����   9º   ���   ���   � ʚ;i� ʚ;���1���1�1�	���   �D$$�t$$�|$(�Ct�D$(�ރ� �Cx�D$,�߉��C|�ؙR��P�D$������D$�L$�t$Z�}�%9º   _��|   � ʚ;i� ʚ;1�����׸ ʚ;���   ���   ���   �����   ���   �D$d+    u?��[^_]Ív ���������1�1�	��>���f��5�����&    f������������������&    ��&    ������UWVS���@u ����   �-\u �`u �du �5�u �l$�-�u �L$�=�u �\$��u �,$��u �-@u 9�u���L$�,$߉t$�\$�t$�|$������ E�݉���SU�T$�L$������� [^_]Í�&    ��U�����&    f������S���ӍB���S�Q�P�Q�P�Q�P�Q�P�Q�P�Q�P�Q�P�Q �P �Q$�P$�Q(�P(�Q,�P,�Q0�P0��K�K8�H�K<�H�K@�H�KD�H�KH�H�KL�H�KP�H�KT�H �KX�H$�K\�H(�K`�H,�Kd�@0�Ch[������U��S�Ã���� ��u d�    �D$��u +�u �u �щ�������$��D$�C�D$�C�D$d+    u�]����������&    f������U�    ���t�8t �<t ]�f�d�    ]������t& �����UWVS�Ã��Du ��u �Du �S�D$�C�Lu �Hu �Du �������xu �Pu �Tu �s�{��u �@B �5|u 1��=�u ��u �K1������� E�EƋs�ǉ$������T$1���1�����t��1����ǉȹ   �l$��1���9ȉ��C�C�v �ȉ5v �{�c���v ��<$�v )ǉ�Չ=v �-v �s�c�l$�v � v ��tZ�K�`u �du �5�u �=�u +M��   ��1����� E�E�����`u 1��� E�E��du �5�u �=�u �C�T$�    1��,v     �\u ��u �$�0v     +K������ �4v E�EƉ(v �$v �C�8v     �Xu ��u �<v     ��[^_]Í�&    f���1������� E�E�����`u 1��� E�E��du �5�u �=�u �?���f������U��WVS�������&    ��� �ȉL$��k�4���� ���� ���� ���� ���� �$���� �L$���� ���� �T$�L$������+D$T$��!�!�������|$�|$�ЉƸ    ��!�!�	��ǋsD�EЋC�t$�|$�{ �[�|$�=�� �L$9��G����ƋD$�������D$��T$��1����� E�E�$T$�e�[^_]Í�&    U��WVS������t& ��� �ȉL$��k�4���� ���� ���� ���� ���� �$���� �L$���� ���� �T$�L$������+D$T$��!�!�������|$�|$�ЉƸ    ��!�!�	��ǋsD�EЋC�t$�|$�{ �[�|$�=�� �L$9��G����ƋD$�������D$��T$��1����� E�E�$T$�e�[^_]Í�&    ��&    �U��������u ]�u Í�&    �v �����UWV��S�Ã����/  ������щǉ��   ������׉��   	�t+��   ��   ���   ���   �CtCh�ǋSx� ʚ;Sli� ʚ;��T$1�L$�K�$�C|$T$�C$�C�S(�S ������ t��C|=�ɚ;v��i��    ʚ;� ʚ;�{p�����   ʉCX�؉S\�����1҃����¸0t ��������   C$���   S(�C,�؉S0��� �#����C4�@� ������t���   ��t�@t �Du �?   ��[^_]Ít& �ǀ�       ǀ�       �����������&    �v �����UWVS���Du �������=Lu �5Hu ��+Pu ��Tu ��!��Pu !����Tu ��,$���$�Չ���u ����1��u !�!�	��5Xu E�E݉����%Xu �`u du �Ɖף`u ����u �du �%�u �\u ��u �u 1ۉD$�T$��u � ʚ;��u 1������� E�E�9Ɖ����rM��u �Ӌ�u ���� �v )Ή$߃��T$���� 9��s�$�T$�5`u �=du ��u ��u ��u � ʚ;1�1ۋt$�|$������ E�EÉ�9��rV��u ��u ���� ��&    �v )Ɖ$׃��\$���� 9��s���l$�<$�5�u ��u �=�u �-�u ��[^_]Í�&    ��&    �����UWVS�Ë@x������Ƅ�t�SH��t���������uu�    ������@u ���������1���uE�   �Du �V����    ��@u �������t�WL��t��������Gx�����[1�^_]�f��؋=Du ����묐�Cx1�������|���������U��WVS��L�=    ���4  �@u �$��  �Du �������ơDu �׉|$�@<�t$�E��u �E ��u �E��u �lu �hu �L$��u �T$��u �D$��u ���L$(+Pu �Hu �T$$����Tu �\$ !ϋLu ����!�����ʹ    ���҉�!�    !�	Љ؋`u D�Dϋ=Xu �D$@�L$D�du �|$0�T$4�=\u �D$�T$�L$8�|$<�=|u +�u �u !ǡ|u �5�u ���=�u !׋�u ����T$�T$��!ȉt$��!�	�u E�E���u �\$��u �\$,�@u 9$������t$0�D$�$���T$�|$@�L$H�L$D�E ���U��|$�΋t$��L$<D$4T$8t$|$ ������ t��1�Ɖ؉u�4$׉}�Ɖ����d$H��L$,D$T$1������� E�E�D$$T$(�E�U��L[^_]Ð��������&    f��������&    f������UWVS��   �D$(��$�   �T$,�L$4�D$d�    ��$�   1��D$l    �@u �D$���  �L$,�D$��$�   �t$(������D$$���^  ��$�   9Du ��  ��$�   ��$�   �Du �$�L$�������L$�5Pu �ŉЋ$�=Tu �ˉl$9�����9�l$�� ˈ\$�W  �$�\$�l$��9��C�C�9��r ��u �4$�|$�D$l��u �D$�D$s��u ��u �hu �lu �L$@��u �\$D��u �L$H��$�   �\$L��$�   )�D$8�Hu �T$<�Lu ���!ƿ    �-du !��������Љl$`��!�!�	��\u D�Eߋ=Xu ��$�   +5�u �T$d�D$h�|u ���|$X�=`u ��u �|$\��$�   =�u !����l$���|$��!���5�u ��!�!�t$P	ȋ�u Dǋ=�u E�|$T�=�u �D$�@u 9D$������L$h�t$8�|$(�|$X�T$���׉��|$<�����L$d�D$\T$`t$@|$D��1����� �L$(E�EӋ\$ƋD$׉s��{�|$L�d$�D$PT$T1������� E�E֋t$H�׀|$ �t$�|$�s�{��  �t$4���-  �>�n��$�   ��$�   ��|$9ǉl$���  �<$�l$��9����  �t$4�\$s8^ ��  �<$�l$����)��+|$l$�L$���~�\$ �l$��\$�|$(�$�M  �L$�\$ ��	��;  �|$�4$����t$,9Ή�މ|$0s+D$T$���ӋD$4�|$�ڋt$+px�D$t�t$t�|$xP���t$�t$�L$�\$�
����D$0������   �\$l9\$(�L$�\$�e  �5Xu �D$t��$���D$x��Ƌ$��1ҋ-�u ��1��ƅ�t��1����ǉ�1����։���ǉȉ�����   �ȉ�$�   ���ډ��D$|���t$�|$,�l$0�D$t9��T$x��t$ ��   �t$4�|$FV�W�GN^�O�_��&    �v ��$�   d+    ��   �D$$�Ĕ   [^_]Ít& ���>�����<$�l$��9�����|$�l$C�C�9�������D$$����띍t& ��D$ �������    �t$)FV)N^�s����v ��1����������v �D$$�����S����v �D$�t$4�P�@+FV�D$|�D$|��$�   ��P���t$�t$�K�������u�L$|��$�   �����D$$������������    �����UWV�����S�Ã�,�d�    �D$(1��C���  �{�ɚ;��  �t]�9Ѻ   ��{  �    ������@u �$�������S+�u �u �\u �ǉա`u �du ������ t�ЋK)��șR��Q��D$$������D$,�|$$��u �l$(�D$��u ����9���:  9ǉ����   ��u +L$����   )��L$����L$�щ��t$�t$�D$$������D$$�T$(�L$,�D$�T$�L$ QRP�Du �p�����S�\u ��u ��u �C1ۙ������ E�EÃ�1��`u �du �   �Du �7����$�    �@u �������   �������t%�D$(d+    uj��,��[^_]Í�    ������ܐ��	�u�T$��t�d�    ���  ��t�� ��u����|$�|$�l$ �T$ ���|$$�����똋�u +L$������������&    f������U��WVS������@u ����   ��u ��u �D$�xu �T$������+�u �u �Ɖס|u ��u !�!��5|u �=�u ����|$���L$�׉��ѿ    !�!�	��u �5�u E�E׋=�u �L$��u �t$�|$�=@u 9��N����t$�������D$T$1������� E�E�D$T$�e�[^_]Í�&    ���
�����&    f������U��WVS������    ���  �@u ����   �hu �lu �D$�Du �T$������+Pu Tu �ƉסHu �Lu !�!��5Hu �=Lu ����|$���L$�׉��ѿ    !�!�	�Xu �5`u E�E׋=du �L$�\u �t$�|$�=@u 9��N����t$�������D$T$1������� E�E�D$T$�e�[^_]Í�&    �v �������&    f��������&    f������U��WVS����� �� �  �    �D$���  �@u ����   �hu �lu �L$�D$�Du �T$��I�T$��L$�����+Pu Tu ��#=Lu �Hu ���5Hu �=Lu !�����ȉ|$���L$�׉��ѿ    !�!�	�Xu �5`u E�E׋=du �$�\u �t$�|$�=@u 9��=����4$�����މ׉ƋT$�D$�t$|$D$T$��1����� E�E����e�[^_]Í�&    ��������������&    f������UWVS���D$�@u �؃��$��   ��u ��u �|$��W�xu �������=�u �5|u +�u �u ���Չ�������!�!����ҿ    ���֋�u !�!�	��T$��u E�E�5�u �=�u �@u 9��`�����,$�������L$���1������� ��ɚ;E�EӉ�9��sL��ɚ;1ɍt& � 6eĉσ����9��r��1ۋt$�F^��[^_]Ít& ���������&    f�1�1��э�&    �v �����UWVS���=    �D$���2  �@u �؃��$��   ��u ��u �|$��W�Du �������=Lu �5Hu +Pu Tu ���Չ�������!�!����ҿ    ���֋\u !�!�	��T$�Xu E�E�5`u �=du �@u 9��`�����,$�������L$���1������� ��ɚ;E�EӉ�9��sN��ɚ;1ɍ�&     6eĉσ����9��r��1ۋt$�F^��[^_]Ít& ���������&    f�1�1��э�&    �v �������&    f������UWVS���=    �$����  �@u �݃��b  ��u ��u �<$��W�Du �������Hu �=Lu +Pu Tu !�!��Lu �Hu ����Ɖ����Љ�!��5�u !�	и    �du E�E��Xu �T$�\u �D$�`u �T$��u �D$��u �T$�D$�@u 9��;������<$�D$�L$�\$�L$_���\$���؉L$�L$�d$�D$T$1������� E�E���Ƹ�ɚ;�9����s,��ɚ;1���&    �� 6eĉ������9��r�1�l$T$�$�D$�T$��S�s��[^_]Í�&    �������&    f��v�����&    f������UWVS�Ã�8d�    �D$41��{�ɚ;�	  �    ������@u ��������\u �du �`u �3�{5�u =�u ������ t��C���R��P�D$0������D$0��u �|$8�D$�D$4�T$�S�D$��u �D$Y^�L$�9$�|%��;$t$������  ��u +K����  ���ɚ;�6  �t]�;L$�   L$�  �u �u ��u �\u �D$��u �C�T$��Ɖ�����1��� E�E� ʚ;5`u =du 1҉5`u �=du ���� t��1���9Ɖ��L$D$�T$r_�D$�T$�l$�\$�L$���\$�� )ΉD$߃��T$���� 9��s�D$�T$�5`u �l$�\$�=du ��u ��u ��u � ʚ;1ҋ5�u �=�u ������ t��1��T$9Ɖ�L$�D$rg��u ��u �l$�\$�\$���� �ЉʋL$)ΉD$߃��T$���� 9��s�D$�T$�5�u �l$�\$�=�u ��u ��u �$�T$+��u S+K1�����SQ�щD$0������D$0�T$4�L$8�D$$�T$(�L$,QRP�Du ��������t& �   �Du ������    ��@u �������   ������D$4d+    u&��8��[^_]Ít& ������뱍�&    f��������������t& �����UWVS�Ã�Td�    �D$P�    ������D$(�    ����   �Lt �Pt �=Ht �5Dt �$�@t �T$������+$T$�����Ѓ�!�!�����D$��l$���L$�|$����!�!�	�u8;=v ��v �i  �l$���J  �������D؃�!�d��&    ��v v t/��t+1ۋT$(�    ������D$Pd+    �	  ��T��[^_]��D$    1��D$    �������Dڃ��u � u ���  �������D�)ø    H���������Ѕ���  ������if
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
	pri΍��mo(:铌�M��I�����r�@�X[�qN�Z��:���B���B���/KF^� �����8^�5]�#�K�F`��Cy~!;+���X��,�4��������6r;3��/Uh���%� 0���40t�)���A�>�J-�=��DT�xz�A�4!d:�(*�!1H���h���uVA���F�V��7	�Z�j��M��u�q(G����@����͡є�!{�Q&!`�B����6�봍EԋRf���#����@0��v9�~／��-�>�/S��[|,3r�ߞO#���eM�����@	ӭ����'�c24W��y�0����8C�
�X�84�����9�4��W���%L�SfY���2�L���ÿ�v���b� aU}���x��Bf����H���?�K< ����K}�O�n��NN�B�A�rt��w���y�T���uU�_]SМhq��]��%�C�O܃�O���XS2J�A�I��BU����w<�1��s�����)�r_�m�'�`(���n{��(z��z���@�#䢋��q�/y��&*˗�e�[�N��Q��ç0qO�a�UC[8�0��~��J���H��r�rgk4cRHfz.=w)���ٻiFV�1B}i<���W��Z>��ׄRe��v�WW�e����#A�2�%r���Y��2��ޮ*\h�nݟ����"�	응����b?����po�#z�Ĕ���c�?���B�e�����	��k��"���~Bkbi0�R-T�<5z<.�3?_|���[�BTa8,�
d#�����e5Ӱ[d,���N+�؟o|���"��q�X�c0��J�4<0���������[�S��C�3���J�sW����mܑN)gc7�i���<5��U�V[�:?�D�Y���q��X%����[�2���)����ǡ��{�'e��&��z;ʔ�QWO����ؤ��#�uΝ���W=�b���y�kb{�_l�{��T�38&�Q�!�*�aw��)*F2|]I;$�:6��ʨ�+@�z�g]�8���KcE���z�5��ι�dTVx� ��@1ۓ#�3�ΧѤ�%�(7Ս�@��<(f	�M��h�R*�&��*4��6����l�7�,��~=������w"��n��|%р,�&�|���e��g~$5�O$�ޤQ��i�>�E>C�
K� � ш;l�^3��=����:2�a�eky�*�籀 ��r�����tl��M��Q�r�E���~������� ��3}�h�h�L��C#�
R�P�srd��`!e.,*(:� ��dV(,� �dg�Ü��:�>��F^EꄟA+!�/x��7t[gܣ���#�k���0~&&v;���z�y/���"�)�G���L#5�M^�&�ј�L�����E�nw8��՜<�uoqA��L&����G�E����sZ��IgSؠ��X��)����I�΃��ԕ'W�>�lP��Ý&�9i�j.WD� R��*��δ��]X�1���T-���D�q<�|ST����;���2R�1�;.pa-�.�rh�t"&�k����/zo�Y�Uy�J�J��w��s#O�׉q�a'��HҢ8�iy��~��w:_�2��i~0U�a'� QZ����k�\����I�q�ި�Ir^gg�)�蹀��
�: M6�ѭ1�zD��7-���Z���U��b�;��˭{}��Q�*����d�
r�W��a�m���tYMu�T����`t�K���u������R[F�͉o���;s���V��з}5j~
oש�)�����:�\�E*J���L7I@���z˾�8e��-^�����?�JI��D@�E��_�TwU��s�Ȑ��������,���#��m�Dn��)Pb۠Mj�8bٰ�
�
�q�[�.�wX]��n�2+����[�¶'��C�kR���Ϡ;��Rb�.	����)u϶��P^��0�xG��r����m���.=h"ᬮ|�[�D�=s��\�\U�4l�;��?l߉���v� ����)�Rw��D���qf^���IѪ�Ą����7K^Y:�C�,D`�d�R�J`��}dG�������y����?��܃̂�sr���v%�������8�*�lT�0�z�\�*az�8�{�g��_��a���+M�Κ�/�Kn�3;��/X�Z���T�'u�/�Ԗ��5�f��~��פx��61���h����`�\��1*�M�ʠ|1�Q��r�Iz�Y��եڃ܊��5�֨�[O�t�e`v���b�X���1,t��8��r`lY�� Ú�F_�(ley��$u6%'��!B�v�a�S`����Z��68Λ���k���ඖg���TSVʢ.�$5�䝚����	�T���(�S	yrhp���$H��;(�,���i$[���NH�\� x�l��e�3�����.rK�b�����y���nɖ� ��ͪ���Q���@�̓�QL�S��_�ai����-�&�-�r�F�i	�Z�s<9�̉�<"�3ݘ��3Eb"���*d�A�ݡ�!�]h���˒)��x���E��������ܵ�{N>('�V�1��J�GJf75�ue��1�� �Q��O[����d1ޗ,����@rve甞�Rh���(Yۗҿ�̿�[����$C��Y���M����$��@L�m�l���}��W�Bw�m�|�b��BF�,V�{�2B#(�"-iՏ�_�W ¹��"���S�����6�ﲀ����Hf�^Y (�s%VD���2�q�y�1�h㛦��I�&���=,t���bXQ�h�[�r�y/���KwvEN��Px@{�����L��<������B/�V����MF�!C�.{�~�t¨�XR��^i�>���	ۙ�]#�����y �9��:Kr���E���JTN� Fq��_q y�����]�J���.��~�@,�f�96�6~D�ug�Z�Ⱥ���yb/e���$��?vh������d��+����T�ׯ����W/��^Y3�Gn��%[fA-�o�2|S�t5�#�/����~6g�ūs���j�h�����e%��I�V1�@��6;��������+kQY#ʧI"��>`k�B9_J��b[�3#E�|���pފ�G�wh��g��+�R��r�j��H%�+�v������6ժ�"xshs���B�����U�%g�f�`ӎU����p���o�<>?��C�����&��"%�l�$bxm_!�1�A�q&��Ǐ�ÑsT��#��qN]ۋ�g��74B�h��Q��?��d��c�Ŧ:n�X:h�H5��O30Xvt��B�^m�)a�^}�5wz6��}����8]vU�ʻZ=$m�
�Y���B̙��0���/��u��������OB7�Ԃ_'�p-gaƍ���w۞T-88�h��i���9�wDB��[Ѭ�*�^#�S�g�Җ��R�	$��H�S���\���A�$Ú��8�M��y���n�P�&,�/X(Q~%CB���B�<�������T7�l��L�2`�P�>��]�������K���$͇鄤�ɫ����Ki�z��ޡ���|�U9���raUc,���.\B�&�S�%�"Y��X	����I �dV�8�]�ab�~p>��i�?!�B����%���6�\��6�~���8��%�
w���V�m/���)uKt��S]X=�%�%�(�j���ΔP����$�M!	IEH�i���D���,
�X��6^��"+>��&"�۪* ]�#)z�����l*��M�!#D�ao0�!�¯�h��d�<�{>��^M�OfV�s4��nm�8��:��)�S.O`2Qޓ6��Bu����6�q� �o@�gM��q!b@q�9&���/�M��e��9X�`��[���N�/զ��1L�U�G=/��~�1 ��N=�sd��&��f�b�	��j�'���j���X�= z;�k�9#�W4��
���??�8	a�`��,�~�L���z&�U�f���tLȊ��V��>N�.|�
ὥ,�-��Y����w�ڸw��̱A�v�E�f�ڽ���w���+�����u�<.Û:�����y퀷�SO��\o����g���`E��h�@�͘�-�d}J�ʕ�1� ��-�Tm5�0"��>��PӤ�渡b�U����]��]W�O�/�`j,*�9�G�B�w�%~(�2�����(�/�b�h�F�����%�5B���;L���o��Ж[�u��	g��x�#��I�a ����06_������
��^�p�){�q,d�ED�=�6�5������D�Y�1��U,$���K��#������7��or��I��(���/?Ě�J�M˭	[	�0Ĵ��2PM٥6Q�� ʮ��=�h����J����\��D7���w�Ӹ�f�p��Q�:�h�8�Cz�+���@���x��j@|�m����xlCY�-cp-�ƌl��?=���6��rM�KK�*����	���v"���;���������g�A�t���۽v�Q�=��� ėa����z�Y�.dG{e>IcrO�%�4�2���5�s�%hx��n�!�m��̀�ӈ-Y���q��	�em���h�ɫi�w�]�����#%�և��8�P �JD�xF,>�;���� �q���4��J���[�|i�ɼC��+u�·D�*U��W|.�<���S��ʗ���{�כ����z��-����H]���4��d����P�pw�����V/��B�īIV�# ��=j��8$Fv�y�;%�����xY,�^ꮰ��|��j�B'OvǞ�ݬj? �%���{��J�i�J῀���AW��O�ՠ���Sp+�"�;�9M�8�շ*M�Z���bo����L�ĸ,n�$�2��њ(yU:�u#2�*K�h:�zTVG��P|gg>�)��Wi>��S8UN�8�����)��<�f$��Y�,�@G��vo.�O��N�o����[��x��k<R`���
�t�|ߌ2�={��+��'��FZ��`��uz7���`���y��{��E!4d���l�� Db�.*C�����'�{v��t���:�%K�m�,D�/v�>��b�Ð �V��~���Ჯ�>�e��v�w�.q�]�̢��!-�͐�U ~
�+(�2p��\Y��U���y����[����[����Gs2��-e��_�����j���0U�ƀ΄���,��С=>�����m/�=߼�n�v� F��^Bsn�ٱ��%I⡙�.vf*P1ڬ�LOҁ���a��A�>̤CAO!�D�[����+��C�&�<7�@�Z���K�7��2���BmSD�Ctx?,�CÃٱ
��oZ�����i�X�<2ҡ�d�{��E����]�Й�%��u���antp�M
Jc�k&�A�Q_ʇ��D�O�G��)�d����g��,��'m�O�������\��y8Q��5��SD�<�p(��?�ݗ�F�{�~��{H/FL�O�Y���<���u{��^�u���s���4w���fE_�f���˻VC�/r�_��W��H?Mv"�a��$�aC%�)���"~����Y�����7)�n;$M���m�*A�p�Xȃ�/Hy����N2<�r�1�s���^<E�>����3�s�v�3������Q���	g-Km*.��Lh�Iif#� :����c܌!���ݥ�S����M�lg�e�EӤ��+�-�=A�iW
���iI����&��*#kQm�`?[�'��S���:˚=����׹i�G����נ2�Z�uK-��lھPj#]G}ҹ2�Y}D)D�'�ܕ�Aq�˳�C�F��!uij
l�x}��6;./��G��zlSU����b�@Ź�1v�C/��Hq�_n��܇���45S3+���P�#�mԭ(s�Hhs�UU�nD�C�����B�f��mx��+�m 3���J��(����\Q�p�vǲ�9A�c����u��C6[�]��b�s��_=��籧��[+�ܷu�ߐsu��R�x�Z>���� 8��3��J� +�˒0�Un��l�<�X��Fo�cA�\�wç-9�r��h(��MA
T��^$2"�`!�ᗷ��h��y�2ȕ����g2��e6T�J��ɇ��H�Wo����,�u�5a�� ��'����9=E;ܒ�H�r;J��vl�h���h�r�����wQm	C�k��W!��:)�I|Yʟh��1s>�|����U�6�ri�оeC�H�tﮃ����#�F�/C�+nQ,֦̅�w!�u��W�!��,�I�|*|42ʻ[Q��!��J��)���ƽ�����1����F����Z 	y�3'rŁ�ȨQ4F��$*�S�YxwY�F3���*ة�+=U���m�z
#���G���iN��Bu��@�\*� L�S�-��v6��QAlQ�C�Ŝk��1��)�M�,��ʽsy����X��<+���>��9��a���P���q��V�wO���S�R��1���;���+g�A�п��Awʁ-�Ч�C����ֈ��~&MZ݂������J�;�&Wo@�zut"���k�¸'Ѽ�e�UԔ��lǵ}0�}�]�~�9����8"ߌ��O7����K���R{ �X��Yz^���r� �ע�LwUOK�!`S����5�	U�do>|���a������r�/���2 ��e�q�ٮJ{��A�n�'������V�;Q�����^��������&",F���sP�E��1{�'��وU�|W��y��S�]^�����ժ��K���
�� s�5� .e*>�_�䶍�/X�'[]�ݾ+�C�S��{F�{�u���tqj5X�$ܰ��1��d��MLCkS����2�?9�>ᆒj��a���}#�k"����c���}������]C� ��VR�ݚ�kz�Hـ�0fM���J<-���$.6���j�4��Q����T}��_�DU$���](��������yr
ڡ�u�b4�$��NIFk�'���/��27.-�xW��ũ8Td�Ů�� ����%��s6m�fÿAr��Yإ�kE�������CM�'�
y��S�-�|�"@A{`˳�t4d�R���_��F��!��Tr�������h���.�D�A�	�KÃ)�"y�]�y�̭�>ְ����'�(p�F����zM����`� r��~�g��j�c"q:Y��v��Uk�吠����.��Ur��Tˁ����b�k 7j�g�H��&`�����\�1�t�`5=�`��Ȱ�i_��jܱT�$UK8%�f����&SG��&�u�
��e	��&u�����.5�	�E�{���j�xB��v��>��g�ZM�#K���yLY?�@�h-u��&T����>���
�3�!�� <�Iyi���NSQl���iG�*>��?���#��bB4:f�(5����"�e�z��-�5����9p���uu�DJ�o���]��e�WIkş8, �L/�k��Yԁ�=����v����
����!��4:HC��b������%Crbz�Ym�<7��O�<�cO׶���R�K�;o�S���+�.�I��'?�Q���}�TE62(=�|�E�E�*pQ� ��<�5^�D���*����.Q�=4��T*���:/�C#�j�R�;8��r�FЛ�V'��]�*� a�F��۴�$}�`1��H��VdO�'���CAM��ǖx��#��UE4cE�k$Lq��ꛂ���l�~�AXA&�Y���^��M�p�?�X&�+1�Ξq�-�� �����F���?z6]�l�<S�h�`'g�Q�{y���\�Z�X~�'��5 C�ŴΒ���n8�.�r�-�/�R�I�|W!��A��N,[<�C$�4rk�fhkl�Oy���^���Of�Ў�// SPDX-License-Identifier: GPL-2.0-or-later
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             �$�������q��d�S�����-~�����;������D�����k4�HI;���vۃe� =v\iqKd]��6%�&�x�&cT�P��0SRXp����o�Oq(��
l�{�g��3��On��q�HA�44�y��\�Hż�i���D�4s��WZ�:����s�j��㫋�|��~e��,�q*L�{�G�o�+V��u �v:Q��8�";��*�NFS#!�M�K���m[���")�&�c�p����2S����h���)q/�;K�3l�h��,�Ae�`䲸O��h�S)��ǩ60Z�>.��p�gu+�=-�+f�ә$��z�O|�
�^��o�V�E�>��d@d��Bl�TC�Kޙ��'��t��b* �JuG|�}Šr9��/�H�oa��+�v�n��+��|�Ă�|͋Yl��P�%�
����P�V�������T#�k#%,�i4i���\�%T˳�"��u���3Jr��8	�]��=2�,�(D������=RT>�Sb~�� Z��+l3�_�Z���H�N&�v�Cxu'�G�>�bW)��,�o��D�T��{*=����[M��;��&��P�,�ٮ���[:�.N!��q�Xe
��F��<0�K���	�古�*���s�܋|m�s&�r��6��g���"H�q�	��f͊�m-��B�絢X�VC�"i�*�m���ц�U�PN<�n��[k�Y�����uɗ��^�\r�"�-��߉ �[��ej-���&/���s�5�W4�I�bB��nʞ�C�̱t�Wk?����8��ǅ�2�:+��ސ8�̲�k6�]|
�2 ���gt�$�D���Z��  X��>\�Q!��N$pӠ�	)��J����m�L�\_�gV��:~/�D�YM��R��A�J

 9���!U��Y��/<{�o���}� ��E��9�����ۃ�G|�)Ɯ�X�@�3w�lM]�GN��M��Ax̝�CW�0Q�-F�i�˿d���hD��J��j�\Y�c<��y�A���+���M1�/�wg�J|K��"ה�&�F,��Vg=�"�19^�:�,+^���Q����3�����Cl<w�`�6��)��W���;ۋra��	����ߵ�I��$��oU� ���}��)ӊ�'9b�V�צM��a����np�ct�x����\Q��#L寜�<�`��"�������eb�|��z����ǜ���
W�( p^(L����XҪ�0��@���m���D�I@RzWA�Vr�x�: !�VL����#D��o�$��E�[�Na��؆ŴTZ��X����iG�-���C��јn��E,Jx���PwoN�s�R�-[���E����3b��1T�n����T���yMC��:Տ��h�mJāWd�,O�<��]�P�1��"��Dqh���������=��+��wI�=S�2���/�M�wV��n�mc1�:�� �� _\�T��C`����t��D�P3�yDR Q5�9��b_F��~��@�C��A���EI�N�5��p�,�0E�ט�ِ%�̯�������h�gz�p����۴-y���Ô�u�{��/"��]�O$��AOv�NX��U]�ص��kE���F�It�t�I6[cu7L)R��-�A}�g4q� ,�c5��o�hfz)ֱg`��h�G�֨U�'�V�(�_��l���5��z�_��^��ǌ]�O��i�bI�qi)�0w(���]��&������`a�g�h�/H��<ln1�;���,ա�_v�;oF�zu~5�_N=j�yU�[��N���{h�o����n�aY&*�h7��bg�0�Rrd�Ł`>4
x,��ΐ1���ל8w.��ߨ9h�K3c^��Yj#T�i�u3K"�_>�m��������>��.O z��������^�ԛ�1�p@��/�lc��ϯL����b��[O���_��n��f�BG�3�U}yTX�t6�,|�Wa6!�EG
L���h0S��󄑁���+p��l�x�B1�jO��o�D�V��~x����N1���#�������{�ݧ9��[�ʏ	�I�[���7
��w�ϔ׺�� ��^q�9{s�u��M�4���3��7{��0����z@W�B�1�J� �H��`痯�� ����s0���Ws����ů�=4�&�B�^�&C�&nK��ҧl2*G�+.���mػ�+z`ģ.
j7m(u��V�#��t�ې�w�����99����m'�|���f�����t�ǯ+���n��*�#-�,@?�DM��a��=aZb�]����*܂�r����ѳ뚧3h�a�N�2������Xl%x��za��uߋ|/���x�pE���	4g�%JN��[V"1��<!eA��F�^�7�v1�հ��Јj�(o��R�&��Z�ԭ0���{�}���~�JX_mw��n�##V�7U�I�Oԏ��:�$�/�ϔ�F0�>��4�ްnj	�c����
�d�gl�GUO9��H�+��(��|Ћ��&�28?XE�lf��Մs�z*���m��/��Xհ����l"U"�s��].�]��ŇI�qҭ9F�[0�$u� B�MO`P*�p��i���ڝ�
�OM�厒�ph�5g��O��t�A����7�#g?;ik�G�s��hT _�x�:���ܿI���;�/��b��s$�#���w�d[�S��'>]+v���N�{��u����&�,�pϕ}���ΌRX]��V>Qqsg`R�ܵogIzR,�D��I/9�X?ir8��V��J�̴
Y͇20�k�0`��| �%l(8M<�?7ا��1{��S=W� ���N'W��dC����v�r/`5�2-A�,�4���5����cxb=�Ci���!}���	,�Ͻ�[z�Y �;�֞���JY%�Ɔ����� �}�$#����C��.HM�ڼB�*oJO�s)�����T>jme�Ǆb�49A^���7��#5LZww.)_)j'��J:='EB�_���*BgT} ��[$���(C}��R�s��M+�I�\���_�%���N_,t�k��<�w7���yL�U�J���������Znd����:�O�d�̱�2�}���Akٱ�_%MHKEQ�B��%�g7�k�m�I��荦����|D�����,��~N72���Z+uk�]��F�z�&y4�j�f]���|�'k>�R�M^�3�X�_y�(n���% �ϛ�.,1�k��MDOu�>�~Hn.�C�@�-�4������Ŝ���0cZf����z\Spk��l���
�J���˭�}���eG���.8N
�<�2�� \��C�@:�(�Bzβ�Z&@�n�*?��i���9-|��r6�w��Yɳ7A)����:K}��U���p��G�$��w��h��� A[�GF�p^�=��X�U<�-2v�Cc�	�p9P���o�cJ��O҅�CB=���	�+8Y��S�&�
� �-�$a�!��?ҋ�\�G	wl&�&h��2��o�k�d��/38�������ӱ���=d�Zp���y|*�xć*֎�Rv^?;�l!�����bFz�A�cbbk�P����v����)����W���?P�#���P��xw�Hn�/8��JB�}��������[�^�?ǦpVk=j_���%�$��ƘP7���G�B�p�-����}9f*�v�+ ���GFN�eYQi���YI8h�QsJw}�$eW%�����;p����"��#�~�1#�h���U&��wE���0�s�>w/\-��X�-	Hh��
��)L�L"��[�9b?a��=�}���$��0���i~o��G�dT�m����e��@n�+�����&��������6JO�e��qeT>�Hn�\8��a��!S�1Z��Ux�7��'>��IO3�.rD�%ܖJ��A�t�&�w3Y�_�&"#�y��^��ieñ�AF��p�}�z��>��M�Ik&{X���D�1;��!Ki,�5S��ÑE�FX�B��������U*'����� �iBk���jmAVP��gr��t@^��������~'�,N��#�%��92ѕzK���@auiF�¸�M�V�;�ob���T��D\X>���N�^�q��p��Kg���t���3�Nm$�z�.�'b�&���з��\I���gyr���*)���]��ϩ7���U7�b�/d>��7�l�Jף���Nx�|J.�I��i8���	�Xʆ]*�B��ѯ'b3c����8����N���b��l%=
� �'�l��5���b�A�D�t�e�.�]�H�b����ةO�s½�ѝ�H��I̩����U3!EǏ����*�� P��� wB�=F�H�?&_�<L��H���oP�;.]}�	�j��3�IXRք��j0��)֩���U��j��*]�,�_����Җ����;&%R�&J�[Q�!R��'ձ
�H��4ݜ��c	�x���"�r�FF�_�j&C��6�앏�%���� �H亴7fx���n��4��\z�^JZvMM �Z��u�6ݕB���p!\��,F��[�?�_2�)��a|
��*\�zS��%�6"J��n�DW˚Q~�ٓ)Z%�6_�_� *ł*M@�x�} ^�����fCqg;����:8�� b���pb��tP+/=f�L�%t�T������NQ'�>U�T������f(49�w �16�3���4��p��D[��#P����z`�~��Q�O��A7a��H��xwA�"c+��Hbv��C�?ʝV�9y�!>x���L84�V�k�����S(��QB)oJ��(C\쌄_�"��vYzn���}XI~&�*o�JL]ᭆ�(c�`+�F[��.7V]�t&'"#�q�7�����`���ǵA�X�B �A�z�K����� G}���ڝ���y@��b��=�,��C��-nER(
�n}YWA����A�b�d���v Ǥv�~��7�%%C�(�6�Lx�w�k�o]p��)��離6��/>�7�uL<�����f�E���V�V��K@Z���ɫ<�fv�{~�fD"�\@��o?���*=�������?DQL��Y#X�+�@��w��f�tƞ(�`�����_��W{�i��߄Ea�j��]9_>���-B�4��<�Q'ZG<OR<di
������>3� .���P�`�X���e�S"�<�pw���p��%��V\��z�dӬ���L��ؘ9�0��բ����ʽ`���V����C[���L�9KL� uȤV�gX�NC7�d&���O���:��C��he$ݎJql1�L����)/��_^�y��if���5F�?�g�N.D3$�ٝt�6d�1#�l��NJͳ�6F˴MU%�Ng�4F�"��5��^�\Z~bc@'��a7=|�C'���oY:�[=e����L������c�o[v���y��q�Pov�ǅi�Pz��e��,��-���͕���V���T��� c�r�~��
��/�H�C����_ʫ���6�i,��0���7�늺����U��z�3t���@y��s���Th�Θ)v��{�Sn��:� "��}�>�c3 !M�R���ғ��)_9��:gɲ�����0	��n&���o���M*��xC����A�}{F���L�= у8�x7�+��u�y�R�;�UE4yo�	�	��O�z��s��p����TZ�?X�k=h�4�����͒��rW�F�}&.��s;ț��+nl����i;<� �:W�Gr����4�vSp9���#{���~8�[��	*�pǴ����Ҡ:��H,�����Wy���8�0\��͂]_c�>���Ǥi�����y�װ��'ü@���|�����=B0�Ċz�)hD��g@-�u����_=��c���H,�6�#_�;�|ф4Q@m:�_HX�����a-_��Y=��t�x�2=�-�&�ƶ�>��@kX�P��o��o���ѬZ#?���[k8p�V��m��6KC�uܶF���UH�N����U���nı-/^�\7=3_�9��t��>������449�#n"�ir��{����)���]w��Tl��$�?�lg�Ma����Dҁd0U��>��ܔ�z�z$+f��UW6j�bЍ� 96K�]�^�L��C4�ר�=���62��qU�1;x�Z�뇷ˉ��,B�ܵ�U�g���J�6ץ]"��%s�GrX�7�`ѡgm���K���T�Z�J9t��h���y�I�C��cy�Zu���-���ᴀ��e����'"v���R��A3X�VT��.�������a�_韂��78����TN�6�e�}�#F�>;�b���)o!��"z#�C�B��U}e,[�#`Ey��"���OL~� �nj3mR���r�c�(��B�,`jK�����X�����0�����+�X3����1��P�Nk���TGTntN�^�"�3x�P#����$�i���'2�p���$
ŧf��ҠA$�o�C8��"AF7}���n�����/�G�'��d�o�Ó��x
�O��
t	�И������*w�sՊ���ȝ �uW����qZ�tm��x�"9����2�\�j���}Siw;9�`��Goj I"gr���������f�p,������`kF�/�~UP�2����#�Ѕ ���֭��V���>�hS���S�a�\8�6�6��Z�n��9��c$n���Ï�85�{��@�f�e�@ �4�J:��)+��dڠP����J�d��D�Ded�0�nw�� ���Տ,�%��כ�C�%TiW����?���^��*�E6�X����]�H��Ġb��}d�42�lChKQ�3Z&�a_����i-<��ߺ��)"�P�[/��(q ��(������-�QMc��>�j����HMR�J[�3(ɒ1
���C�"o�\�I)�Y�g�.vA�gk*�R_��������m�$��H�"����w3�HG�б���k�~2Ad>���jJ�w�mxlK�i��	iq�]u�(�S.��A>��E94��R7,ܳzA""��D��x<��Q�N�ޥ$b4u�u0���x@�l�p�;�h[>=���b��2b�F=�d�l0"�k��XEu�$�0��7}���~S5�>G� �=��Yjڦ3N?�p��/}S�^�+Te���kD����*( j�|���^p�� 
p�WI~d�;I�NԜ�I����,î�Z��l�#�%l�3]�@%��$j�ܧ$���3yQL����ޣ�{K�,�ޟ�Ų��	x��2,c���]N]b�{z��3��D�ID���Xo���!U�L	+�
e�����&p�r��gP��|y�E����Ħ��px�p}���<K����Zɩ�c����L+����/(q�gH��&�<5�6�s����uQ��ޖ����?i�!�hW�=�8��{�ڞ���v)Vf�"��w��,R �ė�p%Q/�'0��r�M����j"����e�-g�A��7Z$P��E;��ӿ��Z���>=E"}���	+�W��%lχ��M�@�!��wK%gD�2(���b��vX�`�t1��W*���쥼V��^�w疊��rO]��s�~���ʸEguQB�%����V���k���x? %��y����&hL��~J�{�/����[�a譨/R@9��5�Tp1%"�&�=;���,�| [{+:v���{/X�A�y��t|J�+c�8zp쒧	1-�r��߰ E�O"�̞nBT��5��>�-���{C$�tGNy�]%e�ϴ����trW�n'�;u3S"ҵG Za�a��>�XX�:�D_By�2HW�E�R���	@*��@�$2�����e.��L5��F�H6���v E�����9ISe�URM1 ���{�p��1$�jE�m��v���(�l�L6`ŖJCf5���E����C^�_�%����\�U&��Mle�Vi<�2�� �h�v�^)8��x$/EtC��NT;�����^b���]�����Ɍ6�Pq�-^<�FQs��/% �L�O�������"X.v+Oy��o�"@[t^
�be5��ʡ�7>?KCɬ�~#��C��'*�� aI�E��ݪ
�F�������kǖ>Tv���N�p� �M��b�[Z/M�Ѩ��ŉ� ���=�m���h�85�?6aZ�of����ޟ���$����
��O6f���Uu�6C0��*o��?\A��8=������K�b޷4�����/��S���S8?R�["�td}r�-H��^��"Q�U�ǝ)�%���lRkk� �q��o���լ�������V�{���w�y�������'r,nZʦK=-w�GQ}��E��^?�^_>�a��b&�Z�V���j��,��4#��Q[�H�}5���;�3�h�:��1��B@#��$�{ ��;*M�)E���2*�҉��4�����[������.�H�8?:D5<�Κ��A��˟�I���Ї
������T/����j�e4�V����V �X��)��+J�+.�?�����-�oO�IY�
c	��_\�~h�5a�pP�*�dr�����?/�ea�+";��wr�q �šd.9�4i|�"��w�s��6����UEb�j�3x��`n3��~�<P�:�Qx�@�*�vP��6��:ܳq�GH���7�8�mcg��p����V�R�Z��!��<�7��0ly�Y^�8@D���2k���q�hZ��d�V
΂���b�����a��@o���i��n�E���	��_ᬟ����)�
���q�V�����m޷ 74��!b;��5>�S}��Z1����o�]�-�-0��k��Z�1���oK�i�(�i�	�g�И�몠�E�F*�c ��k�2�^��R��D�Łq�.8��|�n�#�F�|7�4�τ|YUJ\f�4�%$���$l�\�th�2�,�����C��J9u�j�d��e´���Yz��$��@��ˠ�o�M��sJӇ|�Jzh8�m��Ȉ7b�KxVc�6�I����"��`��Ux�% ����ܼ��1g>v�pPG�FY��:�M%����b��d='�������\GdYzD(|/]6���C����?ߛ��B"� >f�	0́���IG|��TX(�"�;r�����n�NY�]8��օ����D��w-K�����0���h9P404#�C��a�5�RC$�N�9�Z�-3�k���!��}�QU�D�s�̫t�6wv�M�m�om�����rd�o�W�)���!��I���N���GzՔ<�L̷T��*�"�������&R����YHS�{$�;N�,
V��:���^"x����w|���Mܵ�ye d`V��>�xm<�f5��3_�]`��vJ��U}��K)�g']��65�s��֦S�UIF��0��#�I�Y�`������C�)}�1��{z�&�K+h�G���MhH��ꊯ�����b�V�sAL���1ښx7�Ę|}0> ��K&Q���}��W��ü�6y=J?s&=��˵������$�2��d����}��h��:fA��DȮ��|��Gk�S�-���7w�Fa����֟�p�?�Qu$,!�*X�"d��2Z��)�t�@C������rX�N�F��s�e&���'�˻s���D�b�1c��Q��CV�9��$h�
u�o
/ԩ�E6���݈�}�8�L�y�z~��ͧP�x��V���� mcu�e��*Ьf�u�}�Q8T,�B�A��ޢ\��R��7'9��s�
�ٌ6�RD>S����{��ɪ?!��]��8-�b�`�?�I:��q�N94����=R�x7J�J����~t���%*D�,s�����)����l��?�ds��VZ׼p�a�:̆�4�O��^NIShA>�3��jpQ<���$����v��%K��r5�6`.I8�P�x��9���4�&���c�儏�%���N���#��@Q0^���4(3	's��p�������;Im)�ι�9/�t�1N���D"��o�)�+=Jp)g&��c�\H8��2x���I��JPBOm�\zGZ1�%�_�Fr�� zn��j'����ɩ�I����m��iR|��>�.E�Yxյ0Σ/��-+�Ã���`��k^8=�j��8eٿF���9����= ʹ���+斏Z䶝5��R�p�P'/CSB؁��\���U�R��nt�z���+)����M�'�6�q�u�-�\��	q#�A����m2d7����SR�i�kb�S$��:��d��c�e�D\$�h}(�7��I���L�����Z!q��1�t����ߩ�9�y�hm�жw�h�*xz/ �VA�r����#�	$��#Lة��'4�8�O�J�8����o@����x��O��*YzX�%�&�����s	M/�A�~���Lh�֧1���}؈��ư�gϔ����˹	|:A�L�ݑN�~8Y�K��EM�U��Lw����o}�5��N��e�w�WLl�y*$�i���Z7�	J��"�fj�mq&�:�߻�OeID���ȴsĞ����(&�%q��PR�A�c�~����2�W�m��iqf�&a�B>�3t�Ə����&�e8�d��L'�T6;�0�`r[���xNu��Z�s��(�뒊b���(�$���:�n�'�[
����q8��z�<A�(�W� ��q[Om^<���l/c���$��!��JӲ72DP���+�3�B��H.�����i
x�g��#�vj����E$������sk�>tn����6̳�v5_��S��ۆ�����;N�w�H3�7���S�IVs�>'�������K�5���2G�%��noI���f$���Sp�����2���~��V����i_ڳ�TE.����b��neN�/��.Y_w2�YV�~��ЍꫡK(	�,��ZoW�_M"R��jj���#�:`H�W'��d%AU��ٲҭ�:C��+���,F����m��ul���(�5�S�/�`�(�[ќ��hJ;�}ݼ�AZ��k<�?!$�֚��R�K��] ��D���FZg.Z
�!�����֘tǆ�0�������t��0�rQ����6�Ui�M�-�:��}6"������𓜿��,����{�̇`��1�E�$ߺ/t�BfR3�f�ԕ��.�Q>M���S�2�	���r�Z���XdJMd��l-�d��J��7����p}�gn�7�}����#�9�d�tB��|{Oi`rD c�Zy�<i��zm$:ñ�ҳip�ק;�b�I�w�'��zt�8��4�;s�s�}�l�V�X�(l��l�R�5N`��CEEb�zل�6k�;���ԋ1������slw8�N'��%�����io���XByZ��m���nHU��;��v�a2��>�+�d��� �Xc(�	YyP�Hʷ����Ozmޢ�qq6r��4wU�iT)?�Kd&��_��L1�e�[YC�Mj��R!��?(
(�����$0ֹ�� �;t�g�b��ukٽZH�O��ﵠ�`�Y���hxH�%�_��0xĵ��D�zK��&;c��<�Xר��FI>�3@�g�ױ����ΘN�@�cD~�&������~��oς���Z�3��}��XC�K��X�G��Qd-��r��R`�Y Z�̡I��^�v�)3��]�V0�h�R�RX,q��ڃ8\$�L�{	�o�1K�u[�QG��"���F�B��ߦ-?&���(<��&�G�p�]��
-�jпR�`+܅d�V�@R�.�x���=����Ո�/�A���ڲq��~|��wv������CDq[�d�~�V\��f�tέ�_�B�`��M��e��G��hn��\���͵MI\�'F^���K���D����i���Q���T4Ԏ���kE?�������B6��==� �a;�T��>��d�僐������l��t����+�T艰\�Ż���?��// SPDX-License-Identifier: GPL-2.0-or-later
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
 *  Copyright (C) 2