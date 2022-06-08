// SPDX-License-Identifier: GPL-2.0-or-later
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
+-------+-------+-------+-------+-------+----