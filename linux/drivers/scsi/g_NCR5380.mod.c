// SPDX-License-Identifier: GPL-2.0-or-later
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
		printk(KERN_DEBUG pr_fmt("%s: i2c:" fmt)