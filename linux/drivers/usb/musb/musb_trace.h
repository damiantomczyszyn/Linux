

	return bus->i2c_rc;
}

int cx23885_i2c_unregister(struct cx23885_i2c *bus)
{
	i2c_del_adapter(&bus->i2c_adap);
	return 0;
}

void cx23885_av_clk(struct cx23885_dev *dev, int enable)
{
	/* write 0 to bus 2 addr 0x144 via i2x_xfer() */
	char buffer[3];
	struct i2c_msg msg;
	dprintk(1, "%s(enabled = %d)\n", __func__, enable);

	/* Register 0x144 */
	buffer[0] = 0x01;
	buffer[1] = 0x44;
	if (enable == 1)
		buffer[2] = 0x05;
	else
		buffer[2] = 0x00;

	msg.addr = 0x44;
	msg.flags = I2C_M_TEN;
	msg.len = 3;
	msg.buf = buffer;

	i2c_xfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared remote control input device
 *
 *  Most of this file is
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 *
 *  However, the cx23885_input_{init,fini} functions contained herein are
 *  derived from Linux kernel files linux/media/video/.../...-input.c marked as:
 *
 *  Copyright (C) 2008 <srinivasa.deevi at conexant dot com>
 *  Copyright (C) 2005 Ludovico Cavedon <cavedon@sssup.it>
 *		       Markus Rechberger <mrechberger@gmail.com>
 *		       Mauro Carvalho Chehab <mchehab@kernel.org>
 *		       Sascha Sommer <saschasommer@freenet.de>
 *  Copyright (C) 2004, 2005 Chris Pascoe
 *  Copyright (C) 2003, 2004 Gerd Knorr
 *  Copyright (C) 2003 Pavel Machek
 */

#include "cx23885.h"
#include "cx23885-input.h"

#include <linux/slab.h>
#include <media/rc-core.h>
#include <media/v4l2-subdev.h>

#define MODULE_NAME "cx23885"

static void cx23885_input_process_measurements(struct cx23885_dev *dev,
					       bool overrun)
{
	struct cx23885_kernel_ir *kernel_ir = dev->kernel_ir;

	ssize_t num;
	int count, i;
	bool handle = false;
	struct ir_raw_event ir_core_event[64];

	do {
		num = 0;
		v4l2_subdev_call(dev->sd_ir, ir, rx_read, (u8 *) ir_core_event,
				 sizeof(ir_core_event), &num);

		count = num / sizeof(struct ir_raw_event);

		for (i = 0; i < count; i++) {
			ir_raw_event_store(kernel_ir->rc,
					   &ir_core_event[i]);
			handle = true;
		}
	} while (num != 0);

	if (overrun)
		ir_raw_event_overflow(kernel_ir->rc);
	else if (handle)
		ir_raw_event_handle(kernel_ir->rc);
}

void cx23885_input_rx_work_handler(struct cx23885_dev *dev, u32 events)
{
	struct v4l2_subdev_ir_parameters params;
	int overrun, data_available;

	if (dev->sd_ir == NULL || events == 0)
		return;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/*
		 * The only boards we handle right now.  However other boards
		 * using the CX2388x integrated IR controller should be similar
		 */
		break;
	default:
		return;
	}

	overrun = events & (V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN |
			    V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN);

	data_available = events & (V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED |
				   V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ);

	if (overrun) {
		/* If there was a FIFO overrun, stop the device */
		v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
		params.enable = false;
		/* Mitigate race with cx23885_input_ir_stop() */
		params.shutdown = atomic_read(&dev->ir_input_stopping);
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	}

	if (data_available)
		cx23885_input_process_measurements(dev, overrun);

	if (overrun) {
		/* If there was a FIFO overrun, clear & restart the device */
		params.enable = true;
		/* Mitigate race with cx23885_input_ir_stop() */
		params.shutdown = atomic_read(&dev->ir_input_stopping);
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	}
}

static int cx23885_input_ir_start(struct cx23885_dev *dev)
{
	struct v4l2_subdev_ir_parameters params;

	if (dev->sd_ir == NULL)
		return -ENODEV;

	atomic_set(&dev->ir_input_stopping, 0);

	v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		*/
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
		params.shutdown = false;

		/* Setup for baseband compatible with both RC-5 and RC-6A */
		params.modulation = false;
		/* RC-5:  2,222,222 ns = 1/36 kHz * 32 cycles * 2 marks * 1.25*/
		/* RC-6A: 3,333,333 ns = 1/36 kHz * 16 cycles * 6 marks * 1.25*/
		params.max_pulse_width = 3333333; /* ns */
		/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
		/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
		params.noise_filter_min_width = 333333; /* ns */
		/*
		 * This board has inverted receive sense:
		 * mark is received as low logic level;
		 * falling edges are detected as rising edges; etc.
		 */
		params.invert_level = true;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		 */
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
