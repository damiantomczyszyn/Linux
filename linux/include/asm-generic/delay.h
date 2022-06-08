// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23888-ir.h"

#include <linux/kfifo.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/rc-core.h>

static unsigned int ir_888_debug;
module_param(ir_888_debug, int, 0644);
MODULE_PARM_DESC(ir_888_debug, "enable debug messages [CX23888 IR controller]");

#define CX23888_IR_REG_BASE	0x170000
/*
 * These CX23888 register offsets have a straightforward one to one mapping
 * to the CX23885 register offsets of 0x200 through 0x218
 */
#define CX23888_IR_CNTRL_REG	0x170000
#define CNTRL_WIN_3_3	0x00000000
#define CNTRL_WIN_4_3	0x00000001
#define CNTRL_WIN_3_4	0x00000002
#define CNTRL_WIN_4_4	0x00000003
#define CNTRL_WIN	0x00000003
#define CNTRL_EDG_NONE	0x00000000
#define CNTRL_EDG_FALL	0x00000004
#define CNTRL_EDG_RISE	0x00000008
#define CNTRL_EDG_BOTH	0x0000000C
#define CNTRL_EDG	0x0000000C
#define CNTRL_DMD	0x00000010
#define CNTRL_MOD	0x00000020
#define CNTRL_RFE	0x000