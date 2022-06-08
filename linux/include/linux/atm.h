// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Silicon Labs C8051F300 microcontroller.
 *
 * It is used for LNB power control in TeVii S470,
 * TBS 6920 PCIe DVB-S2 cards.
 *
 * Microcontroller connected to cx23885 GPIO pins:
 * GPIO0 - data		- P0.3 F300
 * GPIO1 - reset	