// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  AV device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23885-av.h"
#include "cx23885-video.h"

void cx23885_av_work_handler(struct work_struct *work)
{
	struct cx23885_dev *dev =
			   container_of(work, struct cx23885_dev, cx25840_work);
	bool handled = false;

	v4l2_subdev_call(dev->sd_cx25840, core, interrupt_service_routine,
			 PCI_MSK_AV_CORE, &handled);

	/* Getting here with the interrupt not handled
	   then probbaly flatiron does have pending interrupts.
	*/
	if (!handled) {
		/* clear left and right adc channel interrupt request flag */
		cx23885_flatiron_write(dev, 0x1f,
			cx23885_flatiron_read(dev, 0x1f) | 0x80);
		cx23885_flatiron_write(dev, 0x23,
			cx23885_flatiron_read(dev, 0x23) | 0x80);
	}

	cx23885_irq_enable(dev, PCI_MSK_AV_CORE);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  AV device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_AV_H_
#define _CX23885_AV_H_
void cx23885_av_work_handler(struct work_struct *work);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  AV device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_AV_H_
#define _CX23885_AV_H_
void cx23885_av_work_handler(struct work_struct *work);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <media/drv-intf/cx25840.h>
#include <linux/firmware.h>
#include <misc/altera.h>

#include "xc2028.h"
#include "netup-eeprom.h"
#include "netup-init.h"
#include "altera-ci.h"
#include "xc4000.h"
#include "xc5000.h"
#include "cx23888-ir.h"

static unsigned int netup_card_rev = 4;
module_param(netup_card_rev, int, 0644);
MODULE_PARM_DESC(netup_card_rev,
		"NetUP Dual DVB-T/C CI card revision");
static unsigned int enable_885_ir;
module_param(enable_885_ir, int, 0644);
MODULE_PARM_DESC(enable_885_ir,
		 "Enable integrated IR controller for supported\n"
		 "\t\t    CX2388[57] boards that are wired for it:\n"
		 "\t\t\tHVR-1250 (reported safe)\n"
		 "\t\t\tTerraTec Cinergy T PCIe Dual (not well tested, appears to be safe)\n"
		 "\t\t\tTeVii S470 (reported unsafe)\n"
		 "\t\t    This can cause an interrupt storm with some cards.\n"
		 "\t\t    Default: 0 [Disabled]");

/* ------------------------------------------------------------------ */
/* board config info                                                  */

struct cx23885_board cx23885_boards[] = {
	[CX23885_BOARD_UNKNOWN] = {
		.name		= "UNKNOWN/GENERIC",
		/* Ensure safe default for unknown boards */
		.clk_freq       = 0,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE2,
			.vmux   = 1,
		}, {
			.type   = CX23885_VMUX_COMPOSITE3,
			.vmux   = 2,
		}, {
			.type   = CX23885_VMUX_COMPOSITE4,
			.vmux   = 3,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1800lp] = {
		.name		= "Hauppauge WinTV-HVR1800lp",
		.portc		= CX23885_MPEG_DVB,
		.input          = {{
			.type