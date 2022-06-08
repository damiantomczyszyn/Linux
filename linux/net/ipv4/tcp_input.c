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
 *  Copyright (C) 2