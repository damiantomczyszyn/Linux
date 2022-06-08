/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-eeprom.h
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef NETUP_EEPROM_H
#define NETUP_EEPROM_H

struct netup_port_info {
	u8 mac[6];/* card MAC address */
};

struct netup_card_info {
	struct netup_port_in