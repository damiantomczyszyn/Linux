// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * netup-init.c
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "netup-init.h"

static void i2c_av_write(struct i2c_adapter *i2c, u16 reg, u8 val)
{
	int ret;
	u8 buf[3];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 3
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err(