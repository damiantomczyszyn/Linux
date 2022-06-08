// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * netup-eeprom.c
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#
#include "cx23885.h"
#include "netup-eeprom.h"

#define EEPROM_I2C_ADDR 0x50

int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr)
{
	int ret;
	unsigned char buf[2];

	/* Read from EEPROM */
	struct i2c_ms