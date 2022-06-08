/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * cimax2.h
 *
 * CIMax(R) SP2 driver in conjunction with NetUp Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef CIMAX2_H
#define CIMAX2_H
#include <media/dvb_ca_en50221.h>

extern int netup_ci_read_attribute_mem(struct dvb_ca_en50221 *