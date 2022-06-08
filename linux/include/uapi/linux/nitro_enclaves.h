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
	.d