// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Silicon Labs C8051F300 microcontroller.
 *
 * It is used for LNB power control in TeVii S470,
 * TBS 6920 PCIe DVB-S2 cards.
 *
 * Microcontroller connected to cx23885 GPIO pins:
 * GPIO0 - data		- P0.3 F300
 * GPIO1 - reset	- P0.2 F300
 * GPIO2 - clk		- P0.1 F300
 * GPIO3 - busy		- P0.0 F300
 *
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@me.by>
 */

#include "cx23885.h"
#include "cx23885-f300.h"

#define F300_DATA	GPIO_0
#define F300_RESET	GPIO_1
#define F300_CLK	GPIO_2
#define F300_BUSY	GPIO_3

static void f300_set_line(struct cx23885_dev *dev, u32 line, u8 lvl)
{
	cx23885_gpio_enable(dev, line, 1);
	if (lvl == 1)
		cx23885_gpio_set(dev, line);
	else
		cx23885_gpio_clear(dev, line);
}

static u8 f300_get_line(struct cx23885_dev *dev, u32 line)
{
	cx23885_gpio_enable(dev, line, 0);

	return cx23885_gpio_get(dev, line);
}

static void f300_send_byte(struct cx23885_dev *dev, u8 dta)
{
	u8 i;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		f300_set_line(dev, F300_DATA, (dta & 0x80) >> 7);/* msb first */
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
	}
}

static u8 f300_get_byte(struct cx23885_dev *dev)
{
	u8 i, dta = 0;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
		dta |= f300_get_line(dev, F300_DATA);/* msb first */

	}

	return dta;
}

static u8 f300_xfer(struct dvb_frontend *fe, u8 *buf)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;
	u8 i, temp, ret = 0;

	temp = buf[0];
	for (i = 0; i < buf[0]; i++)
		temp += buf[i + 1];
	temp = (~temp + 1);/* get check sum */
	buf[1 + buf[0]] = temp;

	f300_set_line(dev, F300_RESET, 1);
	f300_set_line(dev, F300_CLK, 1);
	udelay(30);
	f300_se