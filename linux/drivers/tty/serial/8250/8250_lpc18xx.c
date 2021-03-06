/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * altera-ci.c
 *
 *  CI driver in conjunction with NetUp Dual DVB-T/C RF CI card
 *
 * Copyright (C) 2010 NetUP Inc.
 * Copyright (C) 2010 Igor M. Liplianin <liplianin@netup.ru>
 */
#ifndef __ALTERA_CI_H
#define __ALTERA_CI_H

#define ALT_DATA	0x000000ff
#define ALT_TDI		0x00008000
#define ALT_TDO		0x00004000
#define ALT_TCK		0x00002000
#define ALT_RDY		0x00001000
#define ALT_RD		0x00000800
#define ALT_WR		0x00000400
#define ALT_AD_RG	0x00000200
#define ALT_CS		0x00000100

struct altera_ci_config {
	void *dev;/* main dev, for example cx23885_dev */
	void *adapter;/* for CI to connect to */
	struct dvb_demux *demux;/* for hardware PID filter to connect to */
	int (*fpga_rw) (void *dev, int ad_rg, int val, int rw);
};

#if IS_REACHABLE(CONFIG_MEDIA_ALTERA_CI)

extern int altera_ci_init(struct altera_ci_config *config, int ci_nr);
extern void altera_ci_release(void *dev, int ci_nr);
extern int altera_ci_irq(void *dev);
extern int altera_ci_tuner_reset(void *dev, int ci_nr);

#else

static inline int altera_ci_init(struct altera_ci_config *config, int ci_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

static inline void altera_ci_release(void *dev, int ci_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
}

static inline int altera_ci_irq(void *dev)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

static inline int altera_ci_tuner_reset(void *dev, int ci_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

#endif
#if 0
static inline int altera_hw_filt_init(struct altera_ci_config *config,
							int hw_filt_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

static inline void altera_hw_filt_release(void *dev, int filt_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
}

static inline int altera_pid_feed_control(void *dev, int filt_nr,
		struct dvb_demux_feed *dvbdmxfeed, int onoff)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

#endif /* CONFIG_MEDIA_ALTERA_CI */

#endif /* __ALTERA_CI_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * cimax2.c
 *
 * CIMax2(R) SP2 driver in conjunction with NetUp Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "cimax2.h"
#include <media/dvb_ca_en50221.h>

/* Max transfer size done by I2C transfer functions */
#define MAX_XFER_SIZE  64

/**** Bit definitions for MC417_RWD and MC417_OEN registers  ***
  bits 31-16
+-----------+
| Reserved  |
+-----------+
  bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
+-------+-------+-------+-------+-------+-------+-------+-------+
|  WR#  |  RD#  |       |  ACK# |  ADHI |  ADLO |  CS1# |  CS0# |
+-------+-------+-------+-------+-------+-------+-------+-------+
 bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
+-------+-------+-------+-------+-------+-------+-------+-------+
|  DATA7|  DATA6|  DATA5|  DATA4|  DATA3|  DATA2|  DATA1|  DATA0|
+-------+-------+-------+-------+-------+-------+-------+-------+
***/
/* MC417 */
#define NETUP_DATA		0x000000ff
#define NETUP_WR		0x00008000
#define NETUP_RD		0x00004000
#define NETUP_ACK		0x00001000
#define NETUP_ADHI		0x00000800
#define NETUP_ADLO		0x00000400
#define NETUP_CS1		0x00000200
#define NETUP_CS0		0x00000100
#define NETUP_EN_ALL		0x00001000
#define NETUP_CTRL_OFF		(NETUP_CS1 | NETUP_CS0 | NETUP_WR | NETUP_RD)
#define NETUP_CI_CTL		0x04
#define NETUP_CI_RD		1

#define NETUP_IRQ_DETAM		0x1
#define NETUP_IRQ_IRQAM		0x4

static unsigned int ci_dbg;
module_param(ci_dbg, int, 0644);
MODULE_PARM_DESC(ci_dbg, "Enable CI debugging");

static unsigned