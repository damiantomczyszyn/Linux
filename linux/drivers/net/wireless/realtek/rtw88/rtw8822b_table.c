C:
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
		params.shutdown = false;

		/* Setup for a standard NEC protocol */
		params.carrier_freq = 37917; /* Hz, 455 kHz/12 for NEC */
		params.carrier_range_lower = 33000; /* Hz */
		params.carrier_range_upper = 43000; /* Hz */
		params.duty_cycle = 33; /* percent, 33 percent for NEC */

		/*
		 * NEC max pulse width: (64/3)/(455 kHz/12) * 16 nec_units
		 * (64/3)/(455 kHz/12) * 16 nec_units * 1.375 = 12378022 ns
		 */
		params.max_pulse_width = 12378022; /* ns */

		/*
		 * NEC noise filter min width: (64/3)/(455 kHz/12) * 1 nec_unit
		 * (64/3)/(455 kHz/12) * 1 nec_units * 0.625 = 351648 ns
		 */
		params.noise_filter_min_width = 351648; /* ns */

		params.modulation = false;
		params.invert_level = true;
		break;
	}
	v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	return 0;
}

static int cx23885_input_ir_open(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx == NULL)
		return -ENODEV;

	return cx23885_input_ir_start(kernel_ir->cx);
}

static void cx23885_input_ir_stop(struct cx23885_dev *dev)
{
	struct v4l2_subdev_ir_parameters params;

	if (dev->sd_ir == NULL)
		return;

	/*
	 * Stop the sd_ir subdevice from generating notifications and
	 * scheduling work.
	 * It is shutdown this way in order to mitigate a race with
	 * cx23885_input_rx_work_handler() in the overrun case, which could
	 * re-enable the subdevice.
	 */
	atomic_set(&dev->ir_input_stopping, 1);
	v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	while (params.shutdown == false) {
		params.enable = false;
		params.interrupt_enable = false;
		params.shutdown = true;
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
		v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	}
	flush_work(&dev->cx25840_work);
	flush_work(&dev->ir_rx_work);
	flush_work(&dev->ir_tx_work);
}

static void cx23885_input_ir_close(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx != NULL)
		cx23885_input_ir_stop(kernel_ir->cx);
}

int cx23885_input_init(struct cx23885_dev *dev)
{
	struct cx23885_kernel_ir *kernel_ir;
	struct rc_dev *rc;
	char *rc_map;
	u64 allowed_protos;

	int ret;

	/*
	 * If the IR device (hardware registers, chip, GPIO lines, etc.) isn't
	 * encapsulated in a v4l2_subdev, then I'm not going to deal with it.
	 */
	if (dev->sd_ir == NULL)
		return -ENODEV;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/* Integrated CX2388[58] IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* The grey Hauppauge RC-5 remote */
		rc_map = RC_MAP_HAUPPAUGE;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* The grey Terratec remote with orange buttons */
		rc_map = RC_MAP_NEC_TERRATEC_CINERGY_XS;
		break;
	case CX23885_BOARD_TEVII_S470:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TEVII_NEC;
		break;
	case CX23885_BOARD_MYGICA_X8507:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TOTAL_MEDIA_IN_HAND_02;
		break;
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TBS_NEC;
		break;
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		rc_map = RC_MAP_DVBSKY;
		break;
	case CX23885_BOARD_TT_CT2_4500_CI:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		rc_map = RC_MAP_TT_1500;
		break;
	default:
		return -ENODEV;
	}

	/* cx23885 board instance kernel IR state */
	kernel_ir = kzalloc(sizeof(struct cx23885_kernel_ir), GFP_KERNEL);
	if (kernel_ir == NULL)
		return -ENOMEM;

	kernel_ir->cx = dev;
	kernel_ir->name = kasprintf(GFP_KERNEL, "cx23885 IR (%s)",
				    cx23885_boards[dev->board].name);
	if (!kernel_ir->name) {
		ret = -ENOMEM;
		goto err_out_free;
	}

	kernel_ir->phys = kasprintf(GFP_KERNEL, "pci-%s/ir0",
				    pci_name(dev->pci));
	if (!kernel_ir->phys) {
		ret = -ENOMEM;
		goto err_out_free_name;
	}

	/* input device */
	rc = rc_allocate_device(RC_DRIVER_IR_RAW);
	if (!rc) {
		ret = -ENOMEM;
		goto err_out_free_phys;
	}

	kernel_ir->rc = rc;
	rc->device_name = kernel_ir->name;
	rc->input_phys = kernel_ir->phys;
	rc->input_id.bustype = BUS_PCI;
	rc->input_id.version = 1;
	if (dev->pci->subsystem_vendor) {
		rc->input_id.vendor  = dev->pci->subsystem_vendor;
		rc->input_id.product = dev->pci->subsystem_device;
	} else {
		rc->input_id.vendor  = dev->pci->vendor;
		rc->input_id.product = dev->pci->device;
	}
	rc->dev.parent = &dev->pci->dev;
	rc->allowed_protocols = allowed_protos;
	rc->priv = kernel_ir;
	rc->open = cx23885_input_ir_open;
	rc->close = cx23885_input_ir_close;
	rc->map_name = rc_map;
	rc->driver_name = MODULE_NAME;

	/* Go */
	dev->kernel_ir = kernel_ir;
	ret = rc_register_device(rc);
	if (ret)
		goto err_out_stop;

	return 0;

err_out_stop:
	cx23885_input_ir_stop(dev);
	dev->kernel_ir = NULL;
	rc_free_device(rc);
err_out_free_phys:
	kfree(kernel_ir->phys);
err_out_free_name:
	kfree(kernel_ir->name);
err_out_free:
	kfree(kernel_ir);
	return ret;
}

void cx23885_input_fini(struct cx23885_dev *dev)
{
	/* Always stop the IR hardware from generating interrupts */
	cx23885_input_ir_stop(dev);

	if (dev->kernel_ir == NULL)
		return;
	rc_unregister_device(dev->kernel_ir->rc);
	kfree(dev->kernel_ir->phys);
	kfree(dev->kernel_ir->name);
	kfree(dev->kernel_ir);
	dev->kernel_ir = NULL;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared remote control input device
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_INPUT_H_
#define _CX23885_INPUT_H_
void cx23885_input_rx_work_handler(struct cx23885_dev *dev, u32 events);

int cx23885_input_init(struct cx23885_dev *dev);
void cx23885_input_fini(struct cx23885_dev *dev);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared remote control input device
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_INPUT_H_
#define _CX23885_INPUT_H_
void cx23885_input_rx_work_handler(struct cx23885_dev *dev, u32 events);

int cx23885_input_init(struct cx23885_dev *dev);
void cx23885_input_fini(struct cx23885_dev *dev);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Various common ioctl() support functions
 *
 *  Copyright (c) 2009 Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23885-ioctl.h"

#ifdef CONFIG_VIDEO_ADV_DEBUG
int cx23885_g_chip_info(struct file *file, void *fh,
			 struct v4l2_dbg_chip_info *chip)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (chip->match.addr > 1)
		return -EINVAL;
	if (chip->match.addr == 1) {
		if (dev->v4l_device == NULL)
			return -EINVAL;
		strscpy(chip->name, "cx23417", sizeof(chip->name));
	} else {
		strscpy(chip->name, dev->v4l2_dev.name, sizeof(chip->name));
	}
	return 0;
}

static int cx23417_g_register(struct cx23885_dev *dev,
			      struct v4l2_dbg_register *reg)
{
	u32 value;

	if (dev->v4l_device == NULL)
		return -EINVAL;

	if ((reg->reg & 0x3) != 0 || reg->reg >= 0x10000)
		return -EINVAL;

	if (mc417_register_read(dev, (u16) reg->reg, &value))
		return -EINVAL; /* V4L2 spec, but -EREMOTEIO really */

	reg->size = 4;
	reg->val = value;
	return 0;
}

int cx23885_g_register(struct file *file, void *fh,
		       struct v4l2_dbg_register *reg)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (reg->match.addr > 1)
		return -EINVAL;
	if (reg->match.addr)
		return cx23417_g_register(dev, reg);

	if ((reg->reg & 0x3) != 0 || reg->reg >= pci_resource_len(dev->pci, 0))
		return -EINVAL;

	reg->size = 4;
	reg->val = cx_read(reg->reg);
	return 0;
}

static int cx23417_s_register(struct cx23885_dev *dev,
			      const struct v4l2_dbg_register *reg)
{
	if (dev->v4l_device == NULL)
		return -EINVAL;

	if ((reg->reg & 0x3) != 0 || reg->reg >= 0x10000)
		return -EINVAL;

	if (mc417_register_write(dev, (u16) reg->reg, (u32) reg->val))
		return -EINVAL; /* V4L2 spec, but -EREMOTEIO really */
	return 0;
}

int cx23885_s_register(struct file *file, void *fh,
		       const struct v4l2_dbg_register *reg)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (reg->match.addr > 1)
		return -EINVAL;
	if (reg->match.addr)
		return cx23417_s_register(dev, reg);

	if ((reg->reg & 0x3) != 0 || reg->reg >= pci_resource_len(dev->pci, 0))
		return -EINVAL;

	cx_write(reg->reg, reg->val);
	return 0;
}
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Various common ioctl() support functions
 *
 *  Copyright (c) 2009 Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23885-ioctl.h"

#ifdef CONFIG_VIDEO_ADV_DEBUG
int cx23885_g_chip_info(struct file *file, void *fh,
			 struct v4l2_dbg_chip_info *chip)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (chip->match.addr > 1)
		return -EINVAL;
	if (chip->match.addr == 1) {
		if (dev->v4l_device == NULL)
			return -EINVAL;
		strscpy(chip->name, "cx23417", sizeof(chip->name));
	} else {
		strscpy(chip->name, dev->v4l2_dev.name, sizeof(chip->name));
	}
	return 0;
}

static int cx23417_g_register(struct cx23885_dev *dev,
			      struct v4l2_dbg_register *reg)
{
	u32 value;

	if (dev->v4l_device == NULL)
		return -EINVAL;

	if ((reg->reg & 0x3) != 0 || reg->reg >= 0x10000)
		return -EINVAL;

	if (mc417_register_read(dev, (u16) reg->reg, &value))
		return -EINVAL; /* V4L2 spec, but -EREMOTEIO really */

	reg->size = 4;
	reg->val = value;
	return 0;
}

int cx23885_g_register(struct file *file, void *fh,
		       struct v4l2_dbg_register *reg)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (reg->match.addr > 1)
		return -EINVAL;
	if (reg->match.addr)
		return cx23417_g_register(dev, reg);

	if ((reg->reg & 0x3) != 0 || reg->reg >= pci_resource_len(dev->pci, 0))
		return -EINVAL;

	reg->size = 4;
	reg->val = cx_read(reg->reg);
	return 0;
}

static int cx23417_s_register(struct cx23885_dev *dev,
			      const struct v4l2_dbg_register *reg)
{
	if (dev->v4l_device == NULL)
		return -EINVAL;

	if ((reg->reg & 0x3) != 0 || reg->reg >= 0x10000)
		return -EINVAL;

	if (mc417_register_write(dev, (u16) reg->reg, (u32) reg->val))
		return -EINVAL; /* V4L2 spec, but -EREMOTEIO really */
	return 0;
}

int cx23885_s_register(struct file *file, void *fh,
		       const struct v4l2_dbg_register *reg)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (reg->match.addr > 1)
		return -EINVAL;
	if (reg->match.addr)
		return cx23417_s_register(dev, reg);

	if ((reg->reg & 0x3) != 0 || reg->reg >= pci_resource_len(dev->pci, 0))
		return -EINVAL;

	cx_write(reg->reg, reg->val);
	return 0;
}
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Various common ioctl() support functions
 *
 *  Copyright (c) 2009 Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_IOCTL_H_
#define _CX23885_IOCTL_H_

int cx23885_g_chip_info(struct file *file, void *fh,
			 struct v4l2_dbg_chip_info *chip);

#ifdef CONFIG_VIDEO_ADV_DEBUG
int cx23885_g_register(struct file *file, void *fh,
		       struct v4l2_dbg_register *reg);


int cx23885_s_register(struct file *file, void *fh,
		       const struct v4l2_dbg_register *reg);

#endif
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Various common ioctl() support functions
 *
 *  Copyright (c) 2009 Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_IOCTL_H_
#define _CX23885_IOCTL_H_

int cx23885_g_chip_info(struct file *file, void *fh,
			 struct v4l2_dbg_chip_info *chip);

#ifdef CONFIG_VIDEO_ADV_DEBUG
int cx23885_g_register(struct file *file, void *fh,
		       struct v4l2_dbg_register *reg);


int cx23885_s_register(struct file *file, void *fh,
		       const struct v4l2_dbg_register *reg);

#endif
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23885-ir.h"
#include "cx23885-input.h"

#include <media/v4l2-device.h>

#define CX23885_IR_RX_FIFO_SERVICE_REQ		0
#define CX23885_IR_RX_END_OF_RX_DETECTED	1
#define CX23885_IR_RX_HW_FIFO_OVERRUN		2
#define CX23885_IR_RX_SW_FIFO_OVERRUN		3

#define CX23885_IR_TX_FIFO_SERVICE_REQ		0


void cx23885_ir_rx_work_handler(struct work_struct *work)
{
	struct cx23885_dev *dev =
			     container_of(work, struct cx23885_dev, ir_rx_work);
	u32 events = 0;
	unsigned long *notifications = &dev->ir_rx_notifications;

	if (test_and_clear_bit(CX23885_IR_RX_SW_FIFO_OVERRUN, notifications))
		events |= V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN;
	if (test_and_clear_bit(CX23885_IR_RX_HW_FIFO_OVERRUN, notifications))
		events |= V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN;
	if (test_and_clear_bit(CX23885_IR_RX_END_OF_RX_DETECTED, notifications))
		events |= V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED;
	if (test_and_clear_bit(CX23885_IR_RX_FIFO_SERVICE_REQ, notifications))
		events |= V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ;

	if (events == 0)
		return;

	if (dev->kernel_ir)
		cx23885_input_rx_work_handler(dev, events);
}

void cx23885_ir_tx_work_handler(struct work_struct *work)
{
	struct cx23885_dev *dev =
			     container_of(work, struct cx23885_dev, ir_tx_work);
	u32 events = 0;
	unsigned long *notifications = &dev->ir_tx_notifications;

	if (test_and_clear_bit(CX23885_IR_TX_FIFO_SERVICE_REQ, notifications))
		events |= V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ;

	if (events == 0)
		return;

}

/* Possibly called in an IRQ context */
void cx23885_ir_rx_v4l2_dev_notify(struct v4l2_subdev *sd, u32 events)
{
	struct cx23885_dev *dev = to_cx23885(sd->v4l2_dev);
	unsigned long *notifications = &dev->ir_rx_notifications;

	if (events & V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ)
		set_bit(CX23885_IR_RX_FIFO_SERVICE_REQ, notifications);
	if (events & V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED)
		set_bit(CX23885_IR_RX_END_OF_RX_DETECTED, notifications);
	if (events & V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN)
		set_bit(CX23885_IR_RX_HW_FIFO_OVERRUN, notifications);
	if (events & V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN)
		set_bit(CX23885_IR_RX_SW_FIFO_OVERRUN, notifications);

	/*
	 * For the integrated AV core, we are already in a workqueue context.
	 * For the CX23888 integrated IR, we are in an interrupt context.
	 */
	if (sd == dev->sd_cx25840)
		cx23885_ir_rx_work_handler(&dev->ir_rx_work);
	else
		schedule_work(&dev->ir_rx_work);
}

/* Possibly called in an IRQ context */
void cx23885_ir_tx_v4l2_dev_notify(struct v4l2_subdev *sd, u32 events)
{
	struct cx23885_dev *dev = to_cx23885(sd->v4l2_dev);
	unsigned long *notifications = &dev->ir_tx_notifications;

	if (events & V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ)
		set_bit(CX23885_IR_TX_FIFO_SERVICE_REQ, notifications);

	/*
	 * For the integrated AV core, we are already in a workqueue context.
	 * For the CX23888 integrated IR, we are in an interrupt context.
	 */
	if (sd == dev->sd_cx25840)
		cx23885_ir_tx_work_handler(&dev->ir_tx_work);
	else
		schedule_work(&dev->ir_tx_work);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23885-ir.h"
#include "cx23885-input.h"

#include <media/v4l2-device.h>

#define CX23885_IR_RX_FIFO_SERVICE_REQ		0
#define CX23885_IR_RX_END_OF_RX_DETECTED	1
#define CX23885_IR_RX_HW_FIFO_OVERRUN		2
#define CX23885_IR_RX_SW_FIFO_OVERRUN		3

#define CX23885_IR_TX_FIFO_SERVICE_REQ		0


void cx23885_ir_rx_work_handler(struct work_struct *work)
{
	struct cx23885_dev *dev =
			     container_of(work, struct cx23885_dev, ir_rx_work);
	u32 events = 0;
	unsigned long *notifications = &dev->ir_rx_notifications;

	if (test_and_clear_bit(CX23885_IR_RX_SW_FIFO_OVERRUN, notifications))
		events |= V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN;
	if (test_and_clear_bit(CX23885_IR_RX_HW_FIFO_OVERRUN, notifications))
		events |= V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN;
	if (test_and_clear_bit(CX23885_IR_RX_END_OF_RX_DETECTED, notifications))
		events |= V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED;
	if (test_and_clear_bit(CX23885_IR_RX_FIFO_SERVICE_REQ, notifications))
		events |= V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ;

	if (events == 0)
		return;

	if (dev->kernel_ir)
		cx23885_input_rx_work_handler(dev, events);
}

void cx23885_ir_tx_work_handler(struct work_struct *work)
{
	struct cx23885_dev *dev =
			     container_of(work, struct cx23885_dev, ir_tx_work);
	u32 events = 0;
	unsigned long *notifications = &dev->ir_tx_notifications;

	if (test_and_clear_bit(CX23885_IR_TX_FIFO_SERVICE_REQ, notifications))
		events |= V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ;

	if (events == 0)
		return;

}

/* Possibly called in an IRQ context */
void cx23885_ir_rx_v4l2_dev_notify(struct v4l2_subdev *sd, u32 events)
{
	struct cx23885_dev *dev = to_cx23885(sd->v4l2_dev);
	unsigned long *notifications = &dev->ir_rx_notifications;

	if (events & V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ)
		set_bit(CX23885_IR_RX_FIFO_SERVICE_REQ, notifications);
	if (events & V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED)
		set_bit(CX23885_IR_RX_END_OF_RX_DETECTED, notifications);
	if (events & V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN)
		set_bit(CX23885_IR_RX_HW_FIFO_OVERRUN, notifications);
	if (events & V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN)
		set_bit(CX23885_IR_RX_SW_FIFO_OVERRUN, notifications);

	/*
	 * For the integrated AV core, we are already in a workqueue context.
	 * For the CX23888 integrated IR, we are in an interrupt context.
	 */
	if (sd == dev->sd_cx25840)
		cx23885_ir_rx_work_handler(&dev->ir_rx_work);
	else
		schedule_work(&dev->ir_rx_work);
}

/* Possibly called in an IRQ context */
void cx23885_ir_tx_v4l2_dev_notify(struct v4l2_subdev *sd, u32 events)
{
	struct cx23885_dev *dev = to_cx23885(sd->v4l2_dev);
	unsigned long *notifications = &dev->ir_tx_notifications;

	if (events & V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ)
		set_bit(CX23885_IR_TX_FIFO_SERVICE_REQ, notifications);

	/*
	 * For the integrated AV core, we are already in a workqueue context.
	 * For the CX23888 integrated IR, we are in an interrupt context.
	 */
	if (sd == dev->sd_cx25840)
		cx23885_ir_tx_work_handler(&dev->ir_tx_work);
	else
		schedule_work(&dev->ir_tx_work);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_IR_H_
#define _CX23885_IR_H_
void cx23885_ir_rx_v4l2_dev_notify(struct v4l2_subdev *sd, u32 events);
void cx23885_ir_tx_v4l2_dev_notify(struct v4l2_subdev *sd, u32 events);

void cx23885_ir_rx_work_handler(struct work_struct *work);
void cx23885_ir_tx_work_handler(struct work_struct *work);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_IR_H_
#define _CX23885_IR_H_
void cx23885_ir_rx_v4l2_dev_notify(struct v4l2_subdev *sd, u32 events);
void cx23885_ir_tx_v4l2_dev_notify(struct v4l2_subdev *sd, u32 events);

void cx23885_ir_rx_work_handler(struct work_struct *work);
void cx23885_ir_tx_work_handler(struct work_struct *work);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#ifndef _CX23885_REG_H_
#define _CX23885_REG_H_

/*
Address Map
0x00000000 -> 0x00009000   TX SRAM  (Fifos)
0x00010000 -> 0x00013c00   RX SRAM  CMDS + CDT

EACH CMDS struct is 0x80 bytes long

DMAx_PTR1 = 0x03040 address of first cluster
DMAx_PTR2 = 0x10600 address of the CDT
DMAx_CNT1 = cluster size in (bytes >> 4) -1
DMAx_CNT2 = total cdt size for all entries >> 3

Cluster Descriptor entry = 4 DWORDS
 DWORD 0 -> ptr to cluster
 DWORD 1 Reserved
 DWORD 2 Reserved
 DWORD 3 Reserved

Channel manager Data Structure entry = 20 DWORD
  0  IntialProgramCounterLow
  1  IntialProgramCounterHigh
  2  ClusterDescriptorTableBase
  3  ClusterDescriptorTableSize
  4  InstructionQueueBase
  5  InstructionQueueSize
...  Reserved
 19  Reserved
*/

/* Risc Instructions */
#define RISC_CNT_INC		 0x00010000
#define RISC_CNT_RESET		 0x00030000
#define RISC_IRQ1		 0x01000000
#define RISC_IRQ2		 0x02000000
#define RISC_EOL		 0x04000000
#define RISC_SOL		 0x08000000
#define RISC_WRITE		 0x10000000
#define RISC_SKIP		 0x20000000
#define RISC_JUMP		 0x70000000
#define RISC_SYNC		 0x80000000
#define RISC_RESYNC		 0x80008000
#define RISC_READ		 0x90000000
#define RISC_WRITERM		 0xB0000000
#define RISC_WRITECM		 0xC0000000
#define RISC_WRITECR		 0xD0000000
#define RISC_WRITEC		 0x50000000
#define RISC_READC		 0xA0000000


/* Audio and Video Core */
#define HOST_REG1		0x00000000
#define HOST_REG2		0x00000001
#define HOST_REG3		0x00000002

/* Chip Configuration Registers */
#define CHIP_CTRL		0x00000100
#define AFE_CTRL		0x00000104
#define VID_PLL_INT_POST	0x00000108
#define VID_PLL_FRAC		0x0000010C
#define AUX_PLL_INT_POST	0x00000110
#define AUX_PLL_FRAC		0x00000114
#define SYS_PLL_INT_POST	0x00000118
#define SYS_PLL_FRAC		0x0000011C
#define PIN_CTRL		0x00000120
#define AUD_IO_CTRL		0x00000124
#define AUD_LOCK1		0x00000128
#define AUD_LOCK2		0x0000012C
#define POWER_CTRL		0x00000130
#define AFE_DIAG_CTRL1		0x00000134
#define AFE_DIAG_CTRL3		0x0000013C
#define PLL_DIAG_CTRL		0x00000140
#define AFE_CLK_OUT_CTRL	0x00000144
#define DLL1_DIAG_CTRL		0x0000015C

/* GPIO[23:19] Output Enable */
#define GPIO2_OUT_EN_REG	0x00000160
/* GPIO[23:19] Data Registers */
#define GPIO2			0x00000164

#define IFADC_CTRL		0x00000180

/* Infrared Remote Registers */
#define IR_CNTRL_REG	0x00000200
#define IR_TXCLK_REG	0x00000204
#define IR_RXCLK_REG	0x00000208
#define IR_CDUTY_REG	0x0000020C
#define IR_STAT_REG	0x00000210
#define IR_IRQEN_REG	0x00000214
#define IR_FILTR_REG	0x00000218
#define IR_FIFO_REG	0x0000023C

/* Video Decoder Registers */
#define MODE_CTRL		0x00000400
#define OUT_CTRL1		0x00000404
#define OUT_CTRL2		0x00000408
#define GEN_STAT		0x0000040C
#define INT_STAT_MASK		0x00000410
#define LUMA_CTRL		0x00000414
#define HSCALE_CTRL		0x00000418
#define VSCALE_CTRL		0x0000041C
#define CHROMA_CTRL		0x00000420
#define VBI_LINE_CTRL1		0x00000424
#define VBI_LINE_CTRL2		0x00000428
#define VBI_LINE_CTRL3		0x0000042C
#define VBI_LINE_CTRL4		0x00000430
#define VBI_LINE_CTRL5		0x00000434
#define VBI_FC_CFG		0x00000438
#define VBI_MISC_CFG1		0x0000043C
#define VBI_MISC_CFG2		0x00000440
#define VBI_PAY1		0x00000444
#define VBI_PAY2		0x00000448
#define VBI_CUST1_CFG1		0x0000044C
#define VBI_CUST1_CFG2		0x00000450
#define VBI_CUST1_CFG3		0x00000454
#define VBI_CUST2_CFG1		0x00000458
#define VBI_CUST2_CFG2		0x0000045C
#define VBI_CUST2_CFG3		0x00000460
#define VBI_CUST3_CFG1		0x00000464
#define VBI_CUST3_CFG2		0x00000468
#define VBI_CUST3_CFG3		0x0000046C
#define HORIZ_TIM_CTRL		0x00000470
#define VERT_TIM_CTRL		0x00000474
#define SRC_COMB_CFG		0x00000478
#define CHROMA_VBIOFF_CFG	0x0000047C
#define FIELD_COUNT		0x00000480
#define MISC_TIM_CTRL		0x00000484
#define DFE_CTRL1		0x00000488
#define DFE_CTRL2		0x0000048C
#define DFE_CTRL3		0x00000490
#define PLL_CTRL		0x00000494
#define HTL_CTRL		0x00000498
#define COMB_CTRL		0x0000049C
#define CRUSH_CTRL		0x000004A0
#define SOFT_RST_CTRL		0x000004A4
#define CX885_VERSION		0x000004B4
#define VBI_PASS_CTRL		0x000004BC

/* Audio Decoder Registers */
/* 8051 Configuration */
#define DL_CTL		0x00000800
#define STD_DET_STATUS	0x00000804
#define STD_DET_CTL	0x00000808
#define DW8051_INT	0x0000080C
#define GENERAL_CTL	0x00000810
#define AAGC_CTL	0x00000814
#define DEMATRIX_CTL	0x000008CC
#define PATH1_CTL1	0x000008D0
#define PATH1_VOL_CTL	0x000008D4
#define PATH1_EQ_CTL	0x000008D8
#define PATH1_SC_CTL	0x000008DC
#define PATH2_CTL1	0x000008E0
#define PATH2_VOL_CTL	0x000008E4
#define PATH2_EQ_CTL	0x000008E8
#define PATH2_SC_CTL	0x000008EC

/* Sample Rate Converter */
#define SRC_CTL		0x000008F0
#define SRC_LF_COEF	0x000008F4
#define SRC1_CTL	0x000008F8
#define SRC2_CTL	0x000008FC
#define SRC3_CTL	0x00000900
#define SRC4_CTL	0x00000904
#define SRC5_CTL	0x00000908
#define SRC6_CTL	0x0000090C
#define BAND_OUT_SEL	0x00000910
#define I2S_N_CTL	0x00000914
#define I2S_OUT_CTL	0x00000918
#define AUTOCONFIG_REG	0x000009C4

/* Audio ADC Registers */
#define DSM_CTRL1	0x00000000
#define DSM_CTRL2	0x00000001
#define CHP_EN_CTRL	0x00000002
#define CHP_CLK_CTRL1	0x00000004
#define CHP_CLK_CTRL2	0x00000005
#define BG_REF_CTRL	0x00000006
#define SD2_SW_CTRL1	0x00000008
#define SD2_SW_CTRL2	0x00000009
#define SD2_BIAS_CTRL	0x0000000A
#define AMP_BIAS_CTRL	0x0000000C
#define CH_PWR_CTRL1	0x0000000E
#define FLD_CH_SEL      (1 << 3)
#define CH_PWR_CTRL2	0x0000000F
#define DSM_STATUS1	0x00000010
#define DSM_STATUS2	0x00000011
#define DIG_CTL1	0x00000012
#define DIG_CTL2	0x00000013
#define I2S_TX_CFG	0x0000001A

#define DEV_CNTRL2	0x00040000

#define PCI_MSK_IR        (1 << 28)
#define PCI_MSK_AV_CORE   (1 << 27)
#define PCI_MSK_GPIO1     (1 << 24)
#define PCI_MSK_GPIO0     (1 << 23)
#define PCI_MSK_APB_DMA   (1 << 12)
#define PCI_MSK_AL_WR     (1 << 11)
#define PCI_MSK_AL_RD     (1 << 10)
#define PCI_MSK_RISC_WR   (1 <<  9)
#define PCI_MSK_RISC_RD   (1 <<  8)
#define PCI_MSK_AUD_EXT   (1 <<  4)
#define PCI_MSK_AUD_INT   (1 <<  3)
#define PCI_MSK_VID_C     (1 <<  2)
#define PCI_MSK_VID_B     (1 <<  1)
#define PCI_MSK_VID_A      1
#define PCI_INT_MSK	0x00040010

#define PCI_INT_STAT	0x00040014
#define PCI_INT_MSTAT	0x00040018

#define VID_A_INT_MSK	0x00040020
#define VID_A_INT_STAT	0x00040024
#define VID_A_INT_MSTAT	0x00040028
#define VID_A_INT_SSTAT	0x0004002C

#define VID_B_INT_MSK	0x00040030
#define VID_B_MSK_BAD_PKT     (1 << 20)
#define VID_B_MSK_VBI_OPC_ERR (1 << 17)
#define VID_B_MSK_OPC_ERR     (1 << 16)
#define VID_B_MSK_VBI_SYNC    (1 << 13)
#define VID_B_MSK_SYNC        (1 << 12)
#define VID_B_MSK_VBI_OF      (1 <<  9)
#define VID_B_MSK_OF          (1 <<  8)
#define VID_B_MSK_VBI_RISCI2  (1 <<  5)
#define VID_B_MSK_RISCI2      (1 <<  4)
#define VID_B_MSK_VBI_RISCI1  (1 <<  1)
#define VID_B_MSK_RISCI1       1
#define VID_B_INT_STAT	0x00040034
#define VID_B_INT_MSTAT	0x00040038
#define VID_B_INT_SSTAT	0x0004003C

#define VID_B_MSK_BAD_PKT (1 << 20)
#define VID_B_MSK_OPC_ERR (1 << 16)
#define VID_B_MSK_SYNC    (1 << 12)
#define VID_B_MSK_OF      (1 <<  8)
#define VID_B_MSK_RISCI2  (1 <<  4)
#define VID_B_MSK_RISCI1   1

#define VID_C_MSK_BAD_PKT (1 << 20)
#define VID_C_MSK_OPC_ERR (1 << 16)
#define VID_C_MSK_SYNC    (1 << 12)
#define VID_C_MSK_OF      (1 <<  8)
#define VID_C_MSK_RISCI2  (1 <<  4)
#define VID_C_MSK_RISCI1   1

/* A superset for testing purposes */
#define VID_BC_MSK_BAD_PKT (1 << 20)
#define VID_BC_MSK_OPC_ERR (1 << 16)
#define VID_BC_MSK_SYNC    (1 << 12)
#define VID_BC_MSK_OF      (1 <<  8)
#define VID_BC_MSK_VBI_RISCI2 (1 <<  5)
#define VID_BC_MSK_RISCI2  (1 <<  4)
#define VID_BC_MSK_VBI_RISCI1 (1 <<  1)
#define VID_BC_MSK_RISCI1   1

#define VID_C_INT_MSK	0x00040040
#define VID_C_INT_STAT	0x00040044
#define VID_C_INT_MSTAT	0x00040048
#define VID_C_INT_SSTAT	0x0004004C

#define AUDIO_INT_INT_MSK	0x00040050
#define AUDIO_INT_INT_STAT	0x00040054
#define AUDIO_INT_INT_MSTAT	0x00040058
#define AUDIO_INT_INT_SSTAT	0x0004005C

#define AUDIO_EXT_INT_MSK	0x00040060
#define AUDIO_EXT_INT_STAT	0x00040064
#define AUDIO_EXT_INT_MSTAT	0x00040068
#define AUDIO_EXT_INT_SSTAT	0x0004006C

/* Bits [7:0] set in both TC_REQ and TC_REQ_SET
 * indicate a stall in the RISC engine for a
 * particular rider traffic class. This causes
 * the 885 and 888 bridges (unknown about 887)
 * to become inoperable. Setting bits in
 * TC_REQ_SET resets the corresponding bits
 * in TC_REQ (and TC_REQ_SET) allowing
 * operation to continue.
 */
#define TC_REQ		0x00040090
#define TC_REQ_SET	0x00040094

#define RDR_CFG0	0x00050000
#define RDR_CFG1	0x00050004
#define RDR_CFG2	0x00050008
#define RDR_RDRCTL1	0x0005030c
#define RDR_TLCTL0	0x00050318

/* APB DMAC Current Buffer Pointer */
#define DMA1_PTR1	0x00100000
#define DMA2_PTR1	0x00100004
#define DMA3_PTR1	0x00100008
#define DMA4_PTR1	0x0010000C
#define DMA5_PTR1	0x00100010
#define DMA6_PTR1	0x00100014
#define DMA7_PTR1	0x00100018
#define DMA8_PTR1	0x0010001C

/* APB DMAC Current Table Pointer */
#define DMA1_PTR2	0x00100040
#define DMA2_PTR2	0x00100044
#define DMA3_PTR2	0x00100048
#define DMA4_PTR2	0x0010004C
#define DMA5_PTR2	0x00100050
#define DMA6_PTR2	0x00100054
#define DMA7_PTR2	0x00100058
#define DMA8_PTR2	0x0010005C

/* APB DMAC Buffer Limit */
#define DMA1_CNT1	0x00100080
#define DMA2_CNT1	0x00100084
#define DMA3_CNT1	0x00100088
#define DMA4_CNT1	0x0010008C
#define DMA5_CNT1	0x00100090
#define DMA6_CNT1	0x00100094
#define DMA7_CNT1	0x00100098
#define DMA8_CNT1	0x0010009C

/* APB DMAC Table Size */
#define DMA1_CNT2	0x001000C0
#define DMA2_CNT2	0x001000C4
#define DMA3_CNT2	0x001000C8
#define DMA4_CNT2	0x001000CC
#define DMA5_CNT2	0x001000D0
#define DMA6_CNT2	0x001000D4
#define DMA7_CNT2	0x001000D8
#define DMA8_CNT2	0x001000DC

/* Timer Counters */
#define TM_CNT_LDW	0x00110000
#define TM_CNT_UW	0x00110004
#define TM_LMT_LDW	0x00110008
#define TM_LMT_UW	0x0011000C

/* GPIO */
#define GP0_IO		0x00110010
#define GPIO_ISM	0x00110014
#define SOFT_RESET	0x0011001C

/* GPIO (417 Microsoftcontroller) RW Data */
#define MC417_RWD	0x00110020

/* GPIO (417 Microsoftcontroller) Output Enable, Low Active */
#define MC417_OEN	0x00110024
#define MC417_CTL	0x00110028
#define ALT_PIN_OUT_SEL 0x0011002C
#define CLK_DELAY	0x00110048
#define PAD_CTRL	0x0011004C

/* Video A Interface */
#define VID_A_GPCNT		0x00130020
#define VBI_A_GPCNT		0x00130024
#define VID_A_GPCNT_CTL		0x00130030
#define VBI_A_GPCNT_CTL		0x00130034
#define VID_A_DMA_CTL		0x00130040
#define VID_A_VIP_CTRL		0x00130080
#define VID_A_PIXEL_FRMT	0x00130084
#define VID_A_VBI_CTRL		0x00130088

/* Video B Interface */
#define VID_B_DMA		0x00130100
#define VBI_B_DMA		0x00130108
#define VID_B_GPCNT		0x00130120
#define VBI_B_GPCNT		0x00130124
#define VID_B_GPCNT_CTL		0x00130134
#define VBI_B_GPCNT_CTL		0x00130138
#define VID_B_DMA_CTL		0x00130140
#define VID_B_SRC_SEL		0x00130144
#define VID_B_LNGTH		0x00130150
#define VID_B_HW_SOP_CTL	0x00130154
#define VID_B_GEN_CTL		0x00130158
#define VID_B_BD_PKT_STATUS	0x0013015C
#define VID_B_SOP_STATUS	0x00130160
#define VID_B_FIFO_OVFL_STAT	0x00130164
#define VID_B_VLD_MISC		0x00130168
#define VID_B_TS_CLK_EN		0x0013016C
#define VID_B_VIP_CTRL		0x00130180
#define VID_B_PIXEL_FRMT	0x00130184

/* Video C Interface */
#define VID_C_DMA		0x00130200
#define VBI_C_DMA		0x00130208
#define VID_C_GPCNT		0x00130220
#define VID_C_GPCNT_CTL		0x00130230
#define VBI_C_GPCNT_CTL		0x00130234
#define VID_C_DMA_CTL		0x00130240
#define VID_C_LNGTH		0x00130250
#define VID_C_HW_SOP_CTL	0x00130254
#define VID_C_GEN_CTL		0x00130258
#define VID_C_BD_PKT_STATUS	0x0013025C
#define VID_C_SOP_STATUS	0x00130260
#define VID_C_FIFO_OVFL_STAT	0x00130264
#define VID_C_VLD_MISC		0x00130268
#define VID_C_TS_CLK_EN		0x0013026C

/* Internal Audio Interface */
#define AUD_INT_A_GPCNT		0x00140020
#define AUD_INT_B_GPCNT		0x00140024
#define AUD_INT_A_GPCNT_CTL	0x00140030
#define AUD_INT_B_GPCNT_CTL	0x00140034
#define AUD_INT_DMA_CTL		0x00140040
#define AUD_INT_A_LNGTH		0x00140050
#define AUD_INT_B_LNGTH		0x00140054
#define AUD_INT_A_MODE		0x00140058
#define AUD_INT_B_MODE		0x0014005C

/* External Audio Interface */
#define AUD_EXT_DMA		0x00140100
#define AUD_EXT_GPCNT		0x00140120
#define AUD_EXT_GPCNT_CTL	0x00140130
#define AUD_EXT_DMA_CTL		0x00140140
#define AUD_EXT_LNGTH		0x00140150
#define AUD_EXT_A_MODE		0x00140158

/* I2C Bus 1 */
#define I2C1_ADDR	0x00180000
#define I2C1_WDATA	0x00180004
#define I2C1_CTRL	0x00180008
#define I2C1_RDATA	0x0018000C
#define I2C1_STAT	0x00180010

/* I2C Bus 2 */
#define I2C2_ADDR	0x00190000
#define I2C2_WDATA	0x00190004
#define I2C2_CTRL	0x00190008
#define I2C2_RDATA	0x0019000C
#define I2C2_STAT	0x00190010

/* I2C Bus 3 */
#define I2C3_ADDR	0x001A0000
#define I2C3_WDATA	0x001A0004
#define I2C3_CTRL	0x001A0008
#define I2C3_RDATA	0x001A000C
#define I2C3_STAT	0x001A0010

/* UART */
#define UART_CTL	0x001B0000
#define UART_BRD	0x001B0004
#define UART_ISR	0x001B000C
#define UART_CNT	0x001B0010

#endif /* _CX23885_REG_H_ */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#ifndef _CX23885_REG_H_
#define _CX23885_REG_H_

/*
Address Map
0x00000000 -> 0x00009000   TX SRAM  (Fifos)
0x00010000 -> 0x00013c00   RX SRAM  CMDS + CDT

EACH CMDS struct is 0x80 bytes long

DMAx_PTR1 = 0x03040 address of first cluster
DMAx_PTR2 = 0x10600 address of the CDT
DMAx_CNT1 = cluster size in (bytes >> 4) -1
DMAx_CNT2 = total cdt size for all entries >> 3

Cluster Descriptor entry = 4 DWORDS
 DWORD 0 -> ptr to cluster
 DWORD 1 Reserved
 DWORD 2 Reserved
 DWORD 3 Reserved

Channel manager Data Structure entry = 20 DWORD
  0  IntialProgramCounterLow
  1  IntialProgramCounterHigh
  2  ClusterDescriptorTableBase
  3  ClusterDescriptorTableSize
  4  InstructionQueueBase
  5  InstructionQueueSize
...  Reserved
 19  Reserved
*/

/* Risc Instructions */
#define RISC_CNT_INC		 0x00010000
#define RISC_CNT_RESET		 0x00030000
#define RISC_IRQ1		 0x01000000
#define RISC_IRQ2		 0x02000000
#define RISC_EOL		 0x04000000
#define RISC_SOL		 0x08000000
#define RISC_WRITE		 0x10000000
#define RISC_SKIP		 0x20000000
#define RISC_JUMP		 0x70000000
#define RISC_SYNC		 0x80000000
#define RISC_RESYNC		 0x80008000
#define RISC_READ		 0x90000000
#define RISC_WRITERM		 0xB0000000
#define RISC_WRITECM		 0xC0000000
#define RISC_WRITECR		 0xD0000000
#define RISC_WRITEC		 0x50000000
#define RISC_READC		 0xA0000000


/* Audio and Video Core */
#define HOST_REG1		0x00000000
#define HOST_REG2		0x00000001
#define HOST_REG3		0x00000002

/* Chip Configuration Registers */
#define CHIP_CTRL		0x00000100
#define AFE_CTRL		0x00000104
#define VID_PLL_INT_POST	0x00000108
#define VID_PLL_FRAC		0x0000010C
#define AUX_PLL_INT_POST	0x00000110
#define AUX_PLL_FRAC		0x00000114
#define SYS_PLL_INT_POST	0x00000118
#define SYS_PLL_FRAC		0x0000011C
#define PIN_CTRL		0x00000120
#define AUD_IO_CTRL		0x00000124
#define AUD_LOCK1		0x00000128
#define AUD_LOCK2		0x0000012C
#define POWER_CTRL		0x00000130
#define AFE_DIAG_CTRL1		0x00000134
#define AFE_DIAG_CTRL3		0x0000013C
#define PLL_DIAG_CTRL		0x00000140
#define AFE_CLK_OUT_CTRL	0x00000144
#define DLL1_DIAG_CTRL		0x0000015C

/* GPIO[23:19] Output Enable */
#define GPIO2_OUT_EN_REG	0x00000160
/* GPIO[23:19] Data Registers */
#define GPIO2			0x00000164

#define IFADC_CTRL		0x00000180

/* Infrared Remote Registers */
#define IR_CNTRL_REG	0x00000200
#define IR_TXCLK_REG	0x00000204
#define IR_RXCLK_REG	0x00000208
#define IR_CDUTY_REG	0x0000020C
#define IR_STAT_REG	0x00000210
#define IR_IRQEN_REG	0x00000214
#define IR_FILTR_REG	0x00000218
#define IR_FIFO_REG	0x0000023C

/* Video Decoder Registers */
#define MODE_CTRL		0x00000400
#define OUT_CTRL1		0x00000404
#define OUT_CTRL2		0x00000408
#define GEN_STAT		0x0000040C
#define INT_STAT_MASK		0x00000410
#define LUMA_CTRL		0x00000414
#define HSCALE_CTRL		0x00000418
#define VSCALE_CTRL		0x0000041C
#define CHROMA_CTRL		0x00000420
#define VBI_LINE_CTRL1		0x00000424
#define VBI_LINE_CTRL2		0x00000428
#define VBI_LINE_CTRL3		0x0000042C
#define VBI_LINE_CTRL4		0x00000430
#define VBI_LINE_CTRL5		0x00000434
#define VBI_FC_CFG		0x00000438
#define VBI_MISC_CFG1		0x0000043C
#define VBI_MISC_CFG2		0x00000440
#define VBI_PAY1		0x00000444
#define VBI_PAY2		0x00000448
#define VBI_CUST1_CFG1		0x0000044C
#define VBI_CUST1_CFG2		0x00000450
#define VBI_CUST1_CFG3		0x00000454
#define VBI_CUST2_CFG1		0x00000458
#define VBI_CUST2_CFG2		0x0000045C
#define VBI_CUST2_CFG3		0x00000460
#define VBI_CUST3_CFG1		0x00000464
#define VBI_CUST3_CFG2		0x00000468
#define VBI_CUST3_CFG3		0x0000046C
#define HORIZ_TIM_CTRL		0x00000470
#define VERT_TIM_CTRL		0x00000474
#define SRC_COMB_CFG		0x00000478
#define CHROMA_VBIOFF_CFG	0x0000047C
#define FIELD_COUNT		0x00000480
#define MISC_TIM_CTRL		0x00000484
#define DFE_CTRL1		0x00000488
#define DFE_CTRL2		0x0000048C
#define DFE_CTRL3		0x00000490
#define PLL_CTRL		0x00000494
#define HTL_CTRL		0x00000498
#define COMB_CTRL		0x0000049C
#define CRUSH_CTRL		0x000004A0
#define SOFT_RST_CTRL		0x000004A4
#define CX885_VERSION		0x000004B4
#define VBI_PASS_CTRL		0x000004BC

/* Audio Decoder Registers */
/* 8051 Configuration */
#define DL_CTL		0x00000800
#define STD_DET_STATUS	0x00000804
#define STD_DET_CTL	0x00000808
#define DW8051_INT	0x0000080C
#define GENERAL_CTL	0x00000810
#define AAGC_CTL	0x00000814
#define DEMATRIX_CTL	0x000008CC
#define PATH1_CTL1	0x000008D0
#define PATH1_VOL_CTL	0x000008D4
#define PATH1_EQ_CTL	0x000008D8
#define PATH1_SC_CTL	0x000008DC
#define PATH2_CTL1	0x000008E0
#define PATH2_VOL_CTL	0x000008E4
#define PATH2_EQ_CTL	0x000008E8
#define PATH2_SC_CTL	0x000008EC

/* Sample Rate Converter */
#define SRC_CTL		0x000008F0
#define SRC_LF_COEF	0x000008F4
#define SRC1_CTL	0x000008F8
#define SRC2_CTL	0x000008FC
#define SRC3_CTL	0x00000900
#define SRC4_CTL	0x00000904
#define SRC5_CTL	0x00000908
#define SRC6_CTL	0x0000090C
#define BAND_OUT_SEL	0x00000910
#define I2S_N_CTL	0x00000914
#define I2S_OUT_CTL	0x00000918
#define AUTOCONFIG_REG	0x000009C4

/* Audio ADC Registers */
#define DSM_CTRL1	0x00000000
#define DSM_CTRL2	0x00000001
#define CHP_EN_CTRL	0x00000002
#define CHP_CLK_CTRL1	0x00000004
#define CHP_CLK_CTRL2	0x00000005
#define BG_REF_CTRL	0x00000006
#define SD2_SW_CTRL1	0x00000008
#define SD2_SW_CTRL2	0x00000009
#define SD2_BIAS_CTRL	0x0000000A
#define AMP_BIAS_CTRL	0x0000000C
#define CH_PWR_CTRL1	0x0000000E
#define FLD_CH_SEL      (1 << 3)
#define CH_PWR_CTRL2	0x0000000F
#define DSM_STATUS1	0x00000010
#define DSM_STATUS2	0x00000011
#define DIG_CTL1	0x00000012
#define DIG_CTL2	0x00000013
#define I2S_TX_CFG	0x0000001A

#define DEV_CNTRL2	0x00040000

#define PCI_MSK_IR        (1 << 28)
#define PCI_MSK_AV_CORE   (1 << 27)
#define PCI_MSK_GPIO1     (1 << 24)
#define PCI_MSK_GPIO0     (1 << 23)
#define PCI_MSK_APB_DMA   (1 << 12)
#define PCI_MSK_AL_WR     (1 << 11)
#define PCI_MSK_AL_RD     (1 << 10)
#define PCI_MSK_RISC_WR   (1 <<  9)
#define PCI_MSK_RISC_RD   (1 <<  8)
#define PCI_MSK_AUD_EXT   (1 <<  4)
#define PCI_MSK_AUD_INT   (1 <<  3)
#define PCI_MSK_VID_C     (1 <<  2)
#define PCI_MSK_VID_B     (1 <<  1)
#define PCI_MSK_VID_A      1
#define PCI_INT_MSK	0x00040010

#define PCI_INT_STAT	0x00040014
#define PCI_INT_MSTAT	0x00040018

#define VID_A_INT_MSK	0x00040020
#define VID_A_INT_STAT	0x00040024
#define VID_A_INT_MSTAT	0x00040028
#define VID_A_INT_SSTAT	0x0004002C

#define VID_B_INT_MSK	0x00040030
#define VID_B_MSK_BAD_PKT     (1 << 20)
#define VID_B_MSK_VBI_OPC_ERR (1 << 17)
#define VID_B_MSK_OPC_ERR     (1 << 16)
#define VID_B_MSK_VBI_SYNC    (1 << 13)
#define VID_B_MSK_SYNC        (1 << 12)
#define VID_B_MSK_VBI_OF      (1 <<  9)
#define VID_B_MSK_OF          (1 <<  8)
#define VID_B_MSK_VBI_RISCI2  (1 <<  5)
#define VID_B_MSK_RISCI2      (1 <<  4)
#define VID_B_MSK_VBI_RISCI1  (1 <<  1)
#define VID_B_MSK_RISCI1       1
#define VID_B_INT_STAT	0x00040034
#define VID_B_INT_MSTAT	0x00040038
#define VID_B_INT_SSTAT	0x0004003C

#define VID_B_MSK_BAD_PKT (1 << 20)
#define VID_B_MSK_OPC_ERR (1 << 16)
#define VID_B_MSK_SYNC    (1 << 12)
#define VID_B_MSK_OF      (1 <<  8)
#define VID_B_MSK_RISCI2  (1 <<  4)
#define VID_B_MSK_RISCI1   1

#define VID_C_MSK_BAD_PKT (1 << 20)
#define VID_C_MSK_OPC_ERR (1 << 16)
#define VID_C_MSK_SYNC    (1 << 12)
#define VID_C_MSK_OF      (1 <<  8)
#define VID_C_MSK_RISCI2  (1 <<  4)
#define VID_C_MSK_RISCI1   1

/* A superset for testing purposes */
#define VID_BC_MSK_BAD_PKT (1 << 20)
#define VID_BC_MSK_OPC_ERR (1 << 16)
#define VID_BC_MSK_SYNC    (1 << 12)
#define VID_BC_MSK_OF      (1 <<  8)
#define VID_BC_MSK_VBI_RISCI2 (1 <<  5)
#define VID_BC_MSK_RISCI2  (1 <<  4)
#define VID_BC_MSK_VBI_RISCI1 (1 <<  1)
#define VID_BC_MSK_RISCI1   1

#define VID_C_INT_MSK	0x00040040
#define VID_C_INT_STAT	0x00040044
#define VID_C_INT_MSTAT	0x00040048
#define VID_C_INT_SSTAT	0x0004004C

#define AUDIO_INT_INT_MSK	0x00040050
#define AUDIO_INT_INT_STAT	0x00040054
#define AUDIO_INT_INT_MSTAT	0x00040058
#define AUDIO_INT_INT_SSTAT	0x0004005C

#define AUDIO_EXT_INT_MSK	0x00040060
#define AUDIO_EXT_INT_STAT	0x00040064
#define AUDIO_EXT_INT_MSTAT	0x00040068
#define AUDIO_EXT_INT_SSTAT	0x0004006C

/* Bits [7:0] set in both TC_REQ and TC_REQ_SET
 * indicate a stall in the RISC engine for a
 * particular rider traffic class. This causes
 * the 885 and 888 bridges (unknown about 887)
 * to become inoperable. Setting bits in
 * TC_REQ_SET resets the corresponding bits
 * in TC_REQ (and TC_REQ_SET) allowing
 * operation to continue.
 */
#define TC_REQ		0x00040090
#define TC_REQ_SET	0x00040094

#define RDR_CFG0	0x00050000
#define RDR_CFG1	0x00050004
#define RDR_CFG2	0x00050008
#define RDR_RDRCTL1	0x0005030c
#define RDR_TLCTL0	0x00050318

/* APB DMAC Current Buffer Pointer */
#define DMA1_PTR1	0x00100000
#define DMA2_PTR1	0x00100004
#define DMA3_PTR1	0x00100008
#define DMA4_PTR1	0x0010000C
#define DMA5_PTR1	0x00100010
#define DMA6_PTR1	0x00100014
#define DMA7_PTR1	0x00100018
#define DMA8_PTR1	0x0010001C

/* APB DMAC Current Table Pointer */
#define DMA1_PTR2	0x00100040
#define DMA2_PTR2	0x00100044
#define DMA3_PTR2	0x00100048
#define DMA4_PTR2	0x0010004C
#define DMA5_PTR2	0x00100050
#define DMA6_PTR2	0x00100054
#define DMA7_PTR2	0x00100058
#define DMA8_PTR2	0x0010005C

/* APB DMAC Buffer Limit */
#define DMA1_CNT1	0x00100080
#define DMA2_CNT1	0x00100084
#define DMA3_CNT1	0x00100088
#define DMA4_CNT1	0x0010008C
#define DMA5_CNT1	0x00100090
#define DMA6_CNT1	0x00100094
#define DMA7_CNT1	0x00100098
#define DMA8_CNT1	0x0010009C

/* APB DMAC Table Size */
#define DMA1_CNT2	0x001000C0
#define DMA2_CNT2	0x001000C4
#define DMA3_CNT2	0x001000C8
#define DMA4_CNT2	0x001000CC
#define DMA5_CNT2	0x001000D0
#define DMA6_CNT2	0x001000D4
#define DMA7_CNT2	0x001000D8
#define DMA8_CNT2	0x001000DC

/* Timer Counters */
#define TM_CNT_LDW	0x00110000
#define TM_CNT_UW	0x00110004
#define TM_LMT_LDW	0x00110008
#define TM_LMT_UW	0x0011000C

/* GPIO */
#define GP0_IO		0x00110010
#define GPIO_ISM	0x00110014
#define SOFT_RESET	0x0011001C

/* GPIO (417 Microsoftcontroller) RW Data */
#define MC417_RWD	0x00110020

/* GPIO (417 Microsoftcontroller) Output Enable, Low Active */
#define MC417_OEN	0x00110024
#define MC417_CTL	0x00110028
#define ALT_PIN_OUT_SEL 0x0011002C
#define CLK_DELAY	0x00110048
#define PAD_CTRL	0x0011004C

/* Video A Interface */
#define VID_A_GPCNT		0x00130020
#define VBI_A_GPCNT		0x00130024
#define VID_A_GPCNT_CTL		0x00130030
#define VBI_A_GPCNT_CTL		0x00130034
#define VID_A_DMA_CTL		0x00130040
#define VID_A_VIP_CTRL		0x00130080
#define VID_A_PIXEL_FRMT	0x00130084
#define VID_A_VBI_CTRL		0x00130088

/* Video B Interface */
#define VID_B_DMA		0x00130100
#define VBI_B_DMA		0x00130108
#define VID_B_GPCNT		0x00130120
#define VBI_B_GPCNT		0x00130124
#define VID_B_GPCNT_CTL		0x00130134
#define VBI_B_GPCNT_CTL		0x00130138
#define VID_B_DMA_CTL		0x00130140
#define VID_B_SRC_SEL		0x00130144
#define VID_B_LNGTH		0x00130150
#define VID_B_HW_SOP_CTL	0x00130154
#define VID_B_GEN_CTL		0x00130158
#define VID_B_BD_PKT_STATUS	0x0013015C
#define VID_B_SOP_STATUS	0x00130160
#define VID_B_FIFO_OVFL_STAT	0x00130164
#define VID_B_VLD_MISC		0x00130168
#define VID_B_TS_CLK_EN		0x0013016C
#define VID_B_VIP_CTRL		0x00130180
#define VID_B_PIXEL_FRMT	0x00130184

/* Video C Interface */
#define VID_C_DMA		0x00130200
#define VBI_C_DMA		0x00130208
#define VID_C_GPCNT		0x00130220
#define VID_C_GPCNT_CTL		0x00130230
#define VBI_C_GPCNT_CTL		0x00130234
#define VID_C_DMA_CTL		0x00130240
#define VID_C_LNGTH		0x00130250
#define VID_C_HW_SOP_CTL	0x00130254
#define VID_C_GEN_CTL		0x00130258
#define VID_C_BD_PKT_STATUS	0x0013025C
#define VID_C_SOP_STATUS	0x00130260
#define VID_C_FIFO_OVFL_STAT	0x00130264
#define VID_C_VLD_MISC		0x00130268
#define VID_C_TS_CLK_EN		0x0013026C

/* Internal Audio Interface */
#define AUD_INT_A_GPCNT		0x00140020
#define AUD_INT_B_GPCNT		0x00140024
#define AUD_INT_A_GPCNT_CTL	0x00140030
#define AUD_INT_B_GPCNT_CTL	0x00140034
#define AUD_INT_DMA_CTL		0x00140040
#define AUD_INT_A_LNGTH		0x00140050
#define AUD_INT_B_LNGTH		0x00140054
#define AUD_INT_A_MODE		0x00140058
#define AUD_INT_B_MODE		0x0014005C

/* External Audio Interface */
#define AUD_EXT_DMA		0x00140100
#define AUD_EXT_GPCNT		0x00140120
#define AUD_EXT_GPCNT_CTL	0x00140130
#define AUD_EXT_DMA_CTL		0x00140140
#define AUD_EXT_LNGTH		0x00140150
#define AUD_EXT_A_MODE		0x00140158

/* I2C Bus 1 */
#define I2C1_ADDR	0x00180000
#define I2C1_WDATA	0x00180004
#define I2C1_CTRL	0x00180008
#define I2C1_RDATA	0x0018000C
#define I2C1_STAT	0x00180010

/* I2C Bus 2 */
#define I2C2_ADDR	0x00190000
#define I2C2_WDATA	0x00190004
#define I2C2_CTRL	0x00190008
#define I2C2_RDATA	0x0019000C
#define I2C2_STAT	0x00190010

/* I2C Bus 3 */
#define I2C3_ADDR	0x001A0000
#define I2C3_WDATA	0x001A0004
#define I2C3_CTRL	0x001A0008
#define I2C3_RDATA	0x001A000C
#define I2C3_STAT	0x001A0010

/* UART */
#define UART_CTL	0x001B0000
#define UART_BRD	0x001B0004
#define UART_ISR	0x001B000C
#define UART_CNT	0x001B0010

#endif /* _CX23885_REG_H_ */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

static unsigned int vbibufs = 4;
module_param(vbibufs, int, 0644);
MODULE_PARM_DESC(vbibufs, "number of vbi buffers, range 2-32");

static unsigned int vbi_debug;
module_param(vbi_debug, int, 0644);
MODULE_PARM_DESC(vbi_debug, "enable debug messages [vbi]");

#define dprintk(level, fmt, arg...)\
	do { if (vbi_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: vbi:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

#define VBI_LINE_LENGTH 1440
#define VBI_NTSC_LINE_COUNT 12
#define VBI_PAL_LINE_COUNT 18


int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.vbi.sampling_rate = 27000000;
	f->fmt.vbi.samples_per_line = VBI_LINE_LENGTH;
	f->fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
	f->fmt.vbi.offset = 0;
	f->fmt.vbi.flags = 0;
	if (dev->tvnorm & V4L2_STD_525_60) {
		/* ntsc */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_525_F1_START + 9;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_525_F2_START + 9;
		f->fmt.vbi.count[0] = VBI_NTSC_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_NTSC_LINE_COUNT;
	} else if (dev->tvnorm & V4L2_STD_625_50) {
		/* pal */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_625_F1_START + 5;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_625_F2_START + 5;
		f->fmt.vbi.count[0] = VBI_PAL_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_PAL_LINE_COUNT;
	}

	return 0;
}

/* We're given the Video Interrupt status register.
 * The cx23885_video_irq() func has already validated
 * the potential error bits, we just need to
 * deal with vbi payload and return indication if
 * we actually processed any payload.
 */
int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status)
{
	u32 count;
	int handled = 0;

	if (status & VID_BC_MSK_VBI_RISCI1) {
		dprintk(1, "%s() VID_BC_MSK_VBI_RISCI1\n", __func__);
		spin_lock(&dev->slock);
		count = cx_read(VBI_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vbiq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	return handled;
}

static int cx23885_start_vbi_dma(struct cx23885_dev    *dev,
			 struct cx23885_dmaqueue *q,
			 struct cx23885_buffer   *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02],
				VBI_LINE_LENGTH, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_VBI_CTRL, 3);
	cx_write(VBI_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000022);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */

	return 0;
}

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;
	*num_planes = 1;
	sizes[0] = lines * VBI_LINE_LENGTH * 2;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, 0);
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;

	if (vb2_plane_size(vb, 0) < lines * VBI_LINE_LENGTH * 2)
		return -EINVAL;
	vb2_set_plane_payload(vb, 0, lines * VBI_LINE_LENGTH * 2);

	cx23885_risc_vbibuffer(dev->pci, &buf->risc,
			 sgt->sgl,
			 0, VBI_LINE_LENGTH * lines,
			 VBI_LINE_LENGTH, 0,
			 lines);
	return 0;
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(vb->vb2_queue->drv_priv, buf);
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back to the start + 12 (skipping
 * the initial JUMP).
 *
 * This is the risc program of the first buffer to be queued if the active list
 * is empty and it just keeps DMAing this buffer without generating any
 * interrupts.
 *
 * If a new buffer is added then the initial JUMP in the code for that buffer
 * will generate an interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
			struct cx23885_buffer, vb);
	struct cx23885_buffer *prev;
	struct cx23885_dmaqueue *q = &dev->vbiq;
	unsigned long flags;

	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	if (list_empty(&q->active)) {
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		dprintk(2, "[%p/%d] vbi_queue - first active\n",
			buf, buf->vb.vb2_buf.index);

	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
			buf, buf->vb.vb2_buf.index);
	}
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_vbi_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}


const struct vb2_ops cx23885_vbi_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

static unsigned int vbibufs = 4;
module_param(vbibufs, int, 0644);
MODULE_PARM_DESC(vbibufs, "number of vbi buffers, range 2-32");

static unsigned int vbi_debug;
module_param(vbi_debug, int, 0644);
MODULE_PARM_DESC(vbi_debug, "enable debug messages [vbi]");

#define dprintk(level, fmt, arg...)\
	do { if (vbi_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: vbi:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

#define VBI_LINE_LENGTH 1440
#define VBI_NTSC_LINE_COUNT 12
#define VBI_PAL_LINE_COUNT 18


int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.vbi.sampling_rate = 27000000;
	f->fmt.vbi.samples_per_line = VBI_LINE_LENGTH;
	f->fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
	f->fmt.vbi.offset = 0;
	f->fmt.vbi.flags = 0;
	if (dev->tvnorm & V4L2_STD_525_60) {
		/* ntsc */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_525_F1_START + 9;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_525_F2_START + 9;
		f->fmt.vbi.count[0] = VBI_NTSC_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_NTSC_LINE_COUNT;
	} else if (dev->tvnorm & V4L2_STD_625_50) {
		/* pal */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_625_F1_START + 5;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_625_F2_START + 5;
		f->fmt.vbi.count[0] = VBI_PAL_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_PAL_LINE_COUNT;
	}

	return 0;
}

/* We're given the Video Interrupt status register.
 * The cx23885_video_irq() func has already validated
 * the potential error bits, we just need to
 * deal with vbi payload and return indication if
 * we actually processed any payload.
 */
int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status)
{
	u32 count;
	int handled = 0;

	if (status & VID_BC_MSK_VBI_RISCI1) {
		dprintk(1, "%s() VID_BC_MSK_VBI_RISCI1\n", __func__);
		spin_lock(&dev->slock);
		count = cx_read(VBI_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vbiq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	return handled;
}

static int cx23885_start_vbi_dma(struct cx23885_dev    *dev,
			 struct cx23885_dmaqueue *q,
			 struct cx23885_buffer   *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02],
				VBI_LINE_LENGTH, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_VBI_CTRL, 3);
	cx_write(VBI_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000022);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */

	return 0;
}

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;
	*num_planes = 1;
	sizes[0] = lines * VBI_LINE_LENGTH * 2;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, 0);
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;

	if (vb2_plane_size(vb, 0) < lines * VBI_LINE_LENGTH * 2)
		return -EINVAL;
	vb2_set_plane_payload(vb, 0, lines * VBI_LINE_LENGTH * 2);

	cx23885_risc_vbibuffer(dev->pci, &buf->risc,
			 sgt->sgl,
			 0, VBI_LINE_LENGTH * lines,
			 VBI_LINE_LENGTH, 0,
			 lines);
	return 0;
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(vb->vb2_queue->drv_priv, buf);
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back to the start + 12 (skipping
 * the initial JUMP).
 *
 * This is the risc program of the first buffer to be queued if the active list
 * is empty and it just keeps DMAing this buffer without generating any
 * interrupts.
 *
 * If a new buffer is added then the initial JUMP in the code for that buffer
 * will generate an interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
			struct cx23885_buffer, vb);
	struct cx23885_buffer *prev;
	struct cx23885_dmaqueue *q = &dev->vbiq;
	unsigned long flags;

	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	if (list_empty(&q->active)) {
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		dprintk(2, "[%p/%d] vbi_queue - first active\n",
			buf, buf->vb.vb2_buf.index);

	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
			buf, buf->vb.vb2_buf.index);
	}
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_vbi_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}


const struct vb2_ops cx23885_vbi_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"
#include "cx23885-video.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kmod.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/div64.h>

#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-event.h>
#include "cx23885-ioctl.h"
#include "xc2028.h"

#include <media/drv-intf/cx25840.h>

MODULE_DESCRIPTION("v4l2 driver module for cx23885 based TV cards");
MODULE_AUTHOR("Steven Toth <stoth@linuxtv.org>");
MODULE_LICENSE("GPL");

/* ------------------------------------------------------------------ */

static unsigned int video_nr[] = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };
static unsigned int vbi_nr[]   = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };

module_param_array(video_nr, int, NULL, 0444);
module_param_array(vbi_nr,   int, NULL, 0444);

MODULE_PARM_DESC(video_nr, "video device numbers");
MODULE_PARM_DESC(vbi_nr, "vbi device numbers");

static unsigned int video_debug;
module_param(video_debug, int, 0644);
MODULE_PARM_DESC(video_debug, "enable debug messages [video]");

static unsigned int irq_debug;
module_param(irq_debug, int, 0644);
MODULE_PARM_DESC(irq_debug, "enable debug messages [IRQ handler]");

static unsigned int vid_limit = 16;
module_param(vid_limit, int, 0644);
MODULE_PARM_DESC(vid_limit, "capture memory limit in megabytes");

#define dprintk(level, fmt, arg...)\
	do { if (video_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: video:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------- */
/* static data                                                         */

#define FORMAT_FLAGS_PACKED       0x01
static struct cx23885_fmt formats[] = {
	{
		.fourcc   = V4L2_PIX_FMT_YUYV,
		.depth    = 16,
		.flags    = FORMAT_FLAGS_PACKED,
	}
};

static struct cx23885_fmt *format_by_fourcc(unsigned int fourcc)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(formats); i++)
		if (formats[i].fourcc == fourcc)
			return formats+i;
	return NULL;
}

/* ------------------------------------------------------------------- */

void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count)
{
	struct cx23885_buffer *buf;

	if (list_empty(&q->active))
		return;
	buf = list_entry(q->active.next,
			struct cx23885_buffer, queue);

	buf->vb.sequence = q->count++;
	buf->vb.vb2_buf.timestamp = ktime_get_ns();
	dprintk(2, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
			buf->vb.vb2_buf.index, count, q->count);
	list_del(&buf->queue);
	vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
}

int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm)
{
	struct v4l2_subdev_format format = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
		.format.code = MEDIA_BUS_FMT_FIXED,
	};

	dprintk(1, "%s(norm = 0x%08x) name: [%s]\n",
		__func__,
		(unsigned int)norm,
		v4l2_norm_to_name(norm));

	if (dev->tvnorm == norm)
		return 0;

	if (dev->tvnorm != norm) {
		if (vb2_is_busy(&dev->vb2_vidq) || vb2_is_busy(&dev->vb2_vbiq) ||
		    vb2_is_busy(&dev->vb2_mpegq))
			return -EBUSY;
	}

	dev->tvnorm = norm;
	dev->width = 720;
	dev->height = norm_maxh(norm);
	dev->field = V4L2_FIELD_INTERLACED;

	call_all(dev, video, s_std, norm);

	format.format.width = dev->width;
	format.format.height = dev->height;
	format.format.field = dev->field;
	call_all(dev, pad, set_fmt, NULL, &format);

	return 0;
}

static struct video_device *cx23885_vdev_init(struct cx23885_dev *dev,
				    struct pci_dev *pci,
				    struct video_device *template,
				    char *type)
{
	struct video_device *vfd;
	dprintk(1, "%s()\n", __func__);

	vfd = video_device_alloc();
	if (NULL == vfd)
		return NULL;
	*vfd = *template;
	vfd->v4l2_dev = &dev->v4l2_dev;
	vfd->release = video_device_release;
	vfd->lock = &dev->lock;
	snprintf(vfd->name, sizeof(vfd->name), "%s (%s)",
		 cx23885_boards[dev->board].name, type);
	video_set_drvdata(vfd, dev);
	return vfd;
}

int cx23885_flatiron_write(struct cx23885_dev *dev, u8 reg, u8 data)
{
	/* 8 bit registers, 8 bit values */
	u8 buf[] = { reg, data };

	struct i2c_msg msg = { .addr = 0x98 >> 1,
		.flags = 0, .buf = buf, .len = 2 };

	return i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}

u8 cx23885_flatiron_read(struct cx23885_dev *dev, u8 reg)
{
	/* 8 bit registers, 8 bit values */
	int ret;
	u8 b0[] = { reg };
	u8 b1[] = { 0 };

	struct i2c_msg msg[] = {
		{ .addr = 0x98 >> 1, .flags = 0, .buf = b0, .len = 1 },
		{ .addr = 0x98 >> 1, .flags = I2C_M_RD, .buf = b1, .len = 1 }
	};

	ret = i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg[0], 2);
	if (ret != 2)
		pr_err("%s() error\n", __func__);

	return b1[0];
}

static void cx23885_flatiron_dump(struct cx23885_dev *dev)
{
	int i;
	dprintk(1, "Flatiron dump\n");
	for (i = 0; i < 0x24; i++) {
		dprintk(1, "FI[%02x] = %02x\n", i,
			cx23885_flatiron_read(dev, i));
	}
}

static int cx23885_flatiron_mux(struct cx23885_dev *dev, int input)
{
	u8 val;
	dprintk(1, "%s(input = %d)\n", __func__, input);

	if (input == 1)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) & ~FLD_CH_SEL;
	else if (input == 2)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) | FLD_CH_SEL;
	else
		return -EINVAL;

	val |= 0x20; /* Enable clock to delta-sigma and dec filter */

	cx23885_flatiron_write(dev, CH_PWR_CTRL1, val);

	/* Wake up */
	cx23885_flatiron_write(dev, CH_PWR_CTRL2, 0);

	if (video_debug)
		cx23885_flatiron_dump(dev);

	return 0;
}

static int cx23885_video_mux(struct cx23885_dev *dev, unsigned int input)
{
	dprintk(1, "%s() video_mux: %d [vmux=%d, gpio=0x%x,0x%x,0x%x,0x%x]\n",
		__func__,
		input, INPUT(input)->vmux,
		INPUT(input)->gpio0, INPUT(input)->gpio1,
		INPUT(input)->gpio2, INPUT(input)->gpio3);
	dev->input = input;

	if (dev->board == CX23885_BOARD_MYGICA_X8506 ||
		dev->board == CX23885_BOARD_MAGICPRO_PROHDTVE2 ||
		dev->board == CX23885_BOARD_MYGICA_X8507) {
		/* Select Analog TV */
		if (INPUT(input)->type == CX23885_VMUX_TELEVISION)
			cx23885_gpio_clear(dev, GPIO_0);
	}

	/* Tell the internal A/V decoder */
	v4l2_subdev_call(dev->sd_cx25840, video, s_routing,
			INPUT(input)->vmux, 0, 0);

	if ((dev->board == CX23885_BOARD_HAUPPAUGE_HVR1800) ||
		(dev->board == CX23885_BOARD_MPX885) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1250) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_IMPACTVCBE) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255_22111) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1265_K4) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1850) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR5525) ||
		(dev->board == CX23885_BOARD_MYGICA_X8507) ||
		(dev->board == CX23885_BOARD_AVERMEDIA_HC81R) ||
		(dev->board == CX23885_BOARD_VIEWCAST_260E) ||
		(dev->board == CX23885_BOARD_VIEWCAST_460E) ||
		(dev->board == CX23885_BOARD_AVERMEDIA_CE310B)) {
		/* Configure audio routing */
		v4l2_subdev_call(dev->sd_cx25840, audio, s_routing,
			INPUT(input)->amux, 0, 0);

		if (INPUT(input)->amux == CX25840_AUDIO7)
			cx23885_flatiron_mux(dev, 1);
		else if (INPUT(input)->amux == CX25840_AUDIO6)
			cx23885_flatiron_mux(dev, 2);
	}

	return 0;
}

static int cx23885_audio_mux(struct cx23885_dev *dev, unsigned int input)
{
	dprintk(1, "%s(input=%d)\n", __func__, input);

	/* The baseband video core of the cx23885 has two audio inputs.
	 * LR1 and LR2. In almost every single case so far only HVR1xxx
	 * cards we've only ever supported LR1. Time to support LR2,
	 * which is available via the optional white breakout header on
	 * the board.
	 * We'll use a could of existing enums in the card struct to allow
	 * devs to specify which baseband input they need, or just default
	 * to what we've always used.
	 */
	if (INPUT(input)->amux == CX25840_AUDIO7)
		cx23885_flatiron_mux(dev, 1);
	else if (INPUT(input)->amux == CX25840_AUDIO6)
		cx23885_flatiron_mux(dev, 2);
	else {
		/* Not specifically defined, assume the default. */
		cx23885_flatiron_mux(dev, 1);
	}

	return 0;
}

/* ------------------------------------------------------------------ */
static int cx23885_start_video_dma(struct cx23885_dev *dev,
			   struct cx23885_dmaqueue *q,
			   struct cx23885_buffer *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* Stop the dma/fifo before we tamper with it's risc programs */
	cx_clear(VID_A_DMA_CTL, 0x11);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH01],
				buf->bpl, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000011);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x11); /* FIFO and RISC enable */

	return 0;
}

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;

	*num_planes = 1;
	sizes[0] = (dev->fmt->depth * dev->width * dev->height) >> 3;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);
	u32 line0_offset, line1_offset;
	struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, 0);
	int field_tff;

	buf->bpl = (dev->width * dev->fmt->depth) >> 3;

	if (vb2_plane_size(vb, 0) < dev->height * buf->bpl)
		return -EINVAL;
	vb2_set_plane_payload(vb, 0, dev->height * buf->bpl);

	switch (dev->field) {
	case V4L2_FIELD_TOP:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, 0, UNSET,
				buf->bpl, 0, dev->height);
		break;
	case V4L2_FIELD_BOTTOM:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, UNSET, 0,
				buf->bpl, 0, dev->height);
		break;
	case V4L2_FIELD_INTERLACED:
		if (dev->tvnorm & V4L2_STD_525_60)
			/* NTSC or  */
			field_tff = 1;
		else
			field_tff = 0;

		if (cx23885_boards[dev->board].force_bff)
			/* PAL / SECAM OR 888 in NTSC MODE */
			field_tff = 0;

		if (field_tff) {
			/* cx25840 transmits NTSC bottom field first */
			dprintk(1, "%s() Creating TFF/NTSC risc\n",
					__func__);
			line0_offset = buf->bpl;
			line1_offset = 0;
		} else {
			/* All other formats are top field first */
			dprintk(1, "%s() Creating BFF/PAL/SECAM risc\n",
					__func__);
			line0_offset = 0;
			line1_offset = buf->bpl;
		}
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, line0_offset,
				line1_offset,
				buf->bpl, buf->bpl,
				dev->height >> 1);
		break;
	case V4L2_FIELD_SEQ_TB:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl,
				0, buf->bpl * (dev->height >> 1),
				buf->bpl, 0,
				dev->height >> 1);
		break;
	case V4L2_FIELD_SEQ_BT:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl,
				buf->bpl * (dev->height >> 1), 0,
				buf->bpl, 0,
				dev->height >> 1);
		break;
	default:
		BUG();
	}
	dprintk(2, "[%p/%d] buffer_init - %dx%d %dbpp 0x%08x - dma=0x%08lx\n",
		buf, buf->vb.vb2_buf.index,
		dev->width, dev->height, dev->fmt->depth, dev->fmt->fourcc,
		(unsigned long)buf->risc.dma);
	return 0;
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(vb->vb2_queue->drv_priv, buf);
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back to the start + 12 (skipping
 * the initial JUMP).
 *
 * This is the risc program of the first buffer to be queued if the active list
 * is empty and it just keeps DMAing this buffer without generating any
 * interrupts.
 *
 * If a new buffer is added then the initial JUMP in the code for that buffer
 * will generate an interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct cx23885_buffer   *prev;
	struct cx23885_dmaqueue *q    = &dev->vidq;
	unsigned long flags;

	/* add jump to start */
	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	spin_lock_irqsave(&dev->slock, flags);
	if (list_empty(&q->active)) {
		list_add_tail(&buf->queue, &q->active);
		dprintk(2, "[%p/%d] buffer_queue - first active\n",
			buf, buf->vb.vb2_buf.index);
	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		list_add_tail(&buf->queue, &q->active);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
				buf, buf->vb.vb2_buf.index);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_video_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x11);
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static const struct vb2_ops cx23885_video_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

/* ------------------------------------------------------------------ */
/* VIDEO IOCTLS                                                       */

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.width        = dev->width;
	f->fmt.pix.height       = dev->height;
	f->fmt.pix.field        = dev->field;
	f->fmt.pix.pixelformat  = dev->fmt->fourcc;
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * dev->fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct cx23885_fmt *fmt;
	enum v4l2_field   field;
	unsigned int      maxw, maxh;

	fmt = format_by_fourcc(f->fmt.pix.pixelformat);
	if (NULL == fmt)
		return -EINVAL;

	field = f->fmt.pix.field;
	maxw  = 720;
	maxh  = norm_maxh(dev->tvnorm);

	if (V4L2_FIELD_ANY == field) {
		field = (f->fmt.pix.height > maxh/2)
			? V4L2_FIELD_INTERLACED
			: V4L2_FIELD_BOTTOM;
	}

	switch (field) {
	case V4L2_FIELD_TOP:
	case V4L2_FIELD_BOTTOM:
		maxh = maxh / 2;
		break;
	case V4L2_FIELD_INTERLACED:
	case V4L2_FIELD_SEQ_TB:
	case V4L2_FIELD_SEQ_BT:
		break;
	default:
		field = V4L2_FIELD_INTERLACED;
		break;
	}

	f->fmt.pix.field = field;
	v4l_bound_align_image(&f->fmt.pix.width, 48, maxw, 2,
			      &f->fmt.pix.height, 32, maxh, 0, 0);
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct v4l2_subdev_format format = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
	};
	int err;

	dprintk(2, "%s()\n", __func__);
	err = vidioc_try_fmt_vid_cap(file, priv, f);

	if (0 != err)
		return err;

	if (vb2_is_busy(&dev->vb2_vidq) || vb2_is_busy(&dev->vb2_vbiq) ||
	    vb2_is_busy(&dev->vb2_mpegq))
		return -EBUSY;

	dev->fmt        = format_by_fourcc(f->fmt.pix.pixelformat);
	dev->width      = f->fmt.pix.width;
	dev->height     = f->fmt.pix.height;
	dev->field	= f->fmt.pix.field;
	dprintk(2, "%s() width=%d height=%d field=%d\n", __func__,
		dev->width, dev->height, dev->field);
	v4l2_fill_mbus_format(&format.format, &f->fmt.pix, MEDIA_BUS_FMT_FIXED);
	call_all(dev, pad, set_fmt, NULL, &format);
	v4l2_fill_pix_format(&f->fmt.pix, &format.format);
	/* set_fmt overwrites f->fmt.pix.field, restore it */
	f->fmt.pix.field = dev->field;
	return 0;
}

static int vidioc_querycap(struct file *file, void  *priv,
	struct v4l2_capability *cap)
{
	struct cx23885_dev *dev = video_drvdata(file);

	strscpy(cap->driver, "cx23885", sizeof(cap->driver));
	strscpy(cap->card, cx23885_boards[dev->board].name,
		sizeof(cap->card));
	sprintf(cap->bus_info, "PCIe:%s", pci_name(dev->pci));
	cap->capabilities = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
			    V4L2_CAP_AUDIO | V4L2_CAP_VBI_CAPTURE |
			    V4L2_CAP_VIDEO_CAPTURE |
			    V4L2_CAP_DEVICE_CAPS;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		cap->capabilities |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			cap->capabilities |= V4L2_CAP_TUNER;
		break;
	}
	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
	struct v4l2_fmtdesc *f)
{
	if (unlikely(f->index >= ARRAY_SIZE(formats)))
		return -EINVAL;

	f->pixelformat = formats[f->index].fourcc;

	return 0;
}

static int vidioc_g_pixelaspect(struct file *file, void *priv,
				int type, struct v4l2_fract *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	bool is_50hz = dev->tvnorm & V4L2_STD_625_50;

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	f->numerator = is_50hz ? 54 : 11;
	f->denominator = is_50hz ? 59 : 10;

	return 0;
}

static int vidioc_g_selection(struct file *file, void *fh,
			      struct v4l2_selection *sel)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (sel->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	switch (sel->target) {
	case V4L2_SEL_TGT_CROP_BOUNDS:
	case V4L2_SEL_TGT_CROP_DEFAULT:
		sel->r.top = 0;
		sel->r.left = 0;
		sel->r.width = 720;
		sel->r.height = norm_maxh(dev->tvnorm);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);

	*id = dev->tvnorm;
	return 0;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id tvnorms)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);

	return cx23885_set_tvnorm(dev, tvnorms);
}

int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i)
{
	static const char *iname[] = {
		[CX23885_VMUX_COMPOSITE1] = "Composite1",
		[CX23885_VMUX_COMPOSITE2] = "Composite2",
		[CX23885_VMUX_COMPOSITE3] = "Composite3",
		[CX23885_VMUX_COMPOSITE4] = "Composite4",
		[CX23885_VMUX_SVIDEO]     = "S-Video",
		[CX23885_VMUX_COMPONENT]  = "Component",
		[CX23885_VMUX_TELEVISION] = "Television",
		[CX23885_VMUX_CABLE]      = "Cable TV",
		[CX23885_VMUX_DVB]        = "DVB",
		[CX23885_VMUX_DEBUG]      = "for debug only",
	};
	unsigned int n;
	dprintk(1, "%s()\n", __func__);

	n = i->index;
	if (n >= MAX_CX23885_INPUT)
		return -EINVAL;

	if (0 == INPUT(n)->type)
		return -EINVAL;

	i->index = n;
	i->type  = V4L2_INPUT_TYPE_CAMERA;
	strscpy(i->name, iname[INPUT(n)->type], sizeof(i->name));
	i->std = CX23885_NORMS;
	if ((CX23885_VMUX_TELEVISION == INPUT(n)->type) ||
		(CX23885_VMUX_CABLE == INPUT(n)->type)) {
		i->type = V4L2_INPUT_TYPE_TUNER;
		i->audioset = 4;
	} else {
		/* Two selectable audio inputs for non-tv inputs */
		i->audioset = 3;
	}

	if (dev->input == n) {
		/* enum'd input matches our configured input.
		 * Ask the video decoder to process the call
		 * and give it an oppertunity to update the
		 * status field.
		 */
		call_all(dev, video, g_input_status, &i->status);
	}

	return 0;
}

static int vidioc_enum_input(struct file *file, void *priv,
				struct v4l2_input *i)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);
	return cx23885_enum_input(dev, i);
}

int cx23885_get_input(struct file *file, void *priv, unsigned int *i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	*i = dev->input;
	dprintk(1, "%s() returns %d\n", __func__, *i);
	return 0;
}

static int vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
	return cx23885_get_input(file, priv, i);
}

int cx23885_set_input(struct file *file, void *priv, unsigned int i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	dprintk(1, "%s(%d)\n", __func__, i);

	if (i >= MAX_CX23885_INPUT) {
		dprintk(1, "%s() -EINVAL\n", __func__);
		return -EINVAL;
	}

	if (INPUT(i)->type == 0)
		return -EINVAL;

	cx23885_video_mux(dev, i);

	/* By default establish the default audio input for the card also */
	/* Caller is free to use VIDIOC_S_AUDIO to override afterwards */
	cx23885_audio_mux(dev, i);
	return 0;
}

static int vidioc_s_input(struct file *file, void *priv, unsigned int i)
{
	return cx23885_set_input(file, priv, i);
}

static int vidioc_log_status(struct file *file, void *priv)
{
	struct cx23885_dev *dev = video_drvdata(file);

	call_all(dev, core, log_status);
	return 0;
}

static int cx23885_query_audinput(struct file *file, void *priv,
	struct v4l2_audio *i)
{
	static const char *iname[] = {
		[0] = "Baseband L/R 1",
		[1] = "Baseband L/R 2",
		[2] = "TV",
	};
	unsigned int n;
	dprintk(1, "%s()\n", __func__);

	n = i->index;
	if (n >= 3)
		return -EINVAL;

	memset(i, 0, sizeof(*i));
	i->index = n;
	strscpy(i->name, iname[n], sizeof(i->name));
	i->capability = V4L2_AUDCAP_STEREO;
	return 0;

}

static int vidioc_enum_audinput(struct file *file, void *priv,
				struct v4l2_audio *i)
{
	return cx23885_query_audinput(file, priv, i);
}

static int vidioc_g_audinput(struct file *file, void *priv,
	struct v4l2_audio *i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if ((CX23885_VMUX_TELEVISION == INPUT(dev->input)->type) ||
		(CX23885_VMUX_CABLE == INPUT(dev->input)->type))
		i->index = 2;
	else
		i->index = dev->audinput;
	dprintk(1, "%s(input=%d)\n", __func__, i->index);

	return cx23885_query_audinput(file, priv, i);
}

static int vidioc_s_audinput(struct file *file, void *priv,
	const struct v4l2_audio *i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if ((CX23885_VMUX_TELEVISION == INPUT(dev->input)->type) ||
		(CX23885_VMUX_CABLE == INPUT(dev->input)->type)) {
		return i->index != 2 ? -EINVAL : 0;
	}
	if (i->index > 1)
		return -EINVAL;

	dprintk(1, "%s(%d)\n", __func__, i->index);

	dev->audinput = i->index;

	/* Skip the audio defaults from the cards struct, caller wants
	 * directly touch the audio mux hardware. */
	cx23885_flatiron_mux(dev, dev->audinput + 1);
	return 0;
}

static int vidioc_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	if (0 != t->index)
		return -EINVAL;

	strscpy(t->name, "Television", sizeof(t->name));

	call_all(dev, tuner, g_tuner, t);
	return 0;
}

static int vidioc_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	if (0 != t->index)
		return -EINVAL;
	/* Update the A/V core */
	call_all(dev, tuner, s_tuner, t);

	return 0;
}

static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	f->type = V4L2_TUNER_ANALOG_TV;
	f->frequency = dev->freq;

	call_all(dev, tuner, g_frequency, f);

	return 0;
}

static int cx23885_set_freq(struct cx23885_dev *dev, const struct v4l2_frequency *f)
{
	struct v4l2_ctrl *mute;
	int old_mute_val = 1;

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	if (unlikely(f->tuner != 0))
		return -EINVAL;

	dev->freq = f->frequency;

	/* I need to mute audio here */
	mute = v4l2_ctrl_find(&dev->ctrl_handler, V4L2_CID_AUDIO_MUTE);
	if (mute) {
		old_mute_val = v4l2_ctrl_g_ctrl(mute);
		if (!old_mute_val)
			v4l2_ctrl_s_ctrl(mute, 1);
	}

	call_all(dev, tuner, s_frequency, f);

	/* When changing channels it is required to reset TVAUDIO */
	msleep(100);

	/* I need to unmute audio here */
	if (old_mute_val == 0)
		v4l2_ctrl_s_ctrl(mute, old_mute_val);

	return 0;
}

static int cx23885_set_freq_via_ops(struct cx23885_dev *dev,
	const struct v4l2_frequency *f)
{
	struct v4l2_ctrl *mute;
	int old_mute_val = 1;
	struct vb2_dvb_frontend *vfe;
	struct dvb_frontend *fe;

	struct analog_parameters params = {
		.mode      = V4L2_TUNER_ANALOG_TV,
		.audmode   = V4L2_TUNER_MODE_STEREO,
		.std       = dev->tvnorm,
		.frequency = f->frequency
	};

	dev->freq = f->frequency;

	/* I need to mute audio here */
	mute = v4l2_ctrl_find(&dev->ctrl_handler, V4L2_CID_AUDIO_MUTE);
	if (mute) {
		old_mute_val = v4l2_ctrl_g_ctrl(mute);
		if (!old_mute_val)
			v4l2_ctrl_s_ctrl(mute, 1);
	}

	/* If HVR1850 */
	dprintk(1, "%s() frequency=%d tuner=%d std=0x%llx\n", __func__,
		params.frequency, f->tuner, params.std);

	vfe = vb2_dvb_get_frontend(&dev->ts2.frontends, 1);
	if (!vfe) {
		return -EINVAL;
	}

	fe = vfe->dvb.frontend;

	if ((dev->board == CX23885_BOARD_HAUPPAUGE_HVR1850) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255_22111) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1265_K4) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR5525) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC))
		fe = &dev->ts1.analog_fe;

	if (fe && fe->ops.tuner_ops.set_analog_params) {
		call_all(dev, video, s_std, dev->tvnorm);
		fe->ops.tuner_ops.set_analog_params(fe, &params);
	}
	else
		pr_err("%s() No analog tuner, aborting\n", __func__);

	/* When changing channels it is required to reset TVAUDIO */
	msleep(100);

	/* I need to unmute audio here */
	if (old_mute_val == 0)
		v4l2_ctrl_s_ctrl(mute, old_mute_val);

	return 0;
}

int cx23885_set_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	int ret;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		ret = cx23885_set_freq_via_ops(dev, f);
		break;
	default:
		ret = cx23885_set_freq(dev, f);
	}

	return ret;
}

static int vidioc_s_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	return cx23885_set_frequency(file, priv, f);
}

/* ----------------------------------------------------------- */

int cx23885_video_irq(struct cx23885_dev *dev, u32 status)
{
	u32 mask, count;
	int handled = 0;

	mask   = cx_read(VID_A_INT_MSK);
	if (0 == (status & mask))
		return handled;

	cx_write(VID_A_INT_STAT, status);

	/* risc op code error, fifo overflow or line sync detection error */
	if ((status & VID_BC_MSK_OPC_ERR) ||
		(status & VID_BC_MSK_SYNC) ||
		(status & VID_BC_MSK_OF)) {

		if (status & VID_BC_MSK_OPC_ERR) {
			dprintk(7, " (VID_BC_MSK_OPC_ERR 0x%08x)\n",
				VID_BC_MSK_OPC_ERR);
			pr_warn("%s: video risc op code error\n",
				dev->name);
			cx23885_sram_channel_dump(dev,
				&dev->sram_channels[SRAM_CH01]);
		}

		if (status & VID_BC_MSK_SYNC)
			dprintk(7, " (VID_BC_MSK_SYNC 0x%08x) video lines miss-match\n",
				VID_BC_MSK_SYNC);

		if (status & VID_BC_MSK_OF)
			dprintk(7, " (VID_BC_MSK_OF 0x%08x) fifo overflow\n",
				VID_BC_MSK_OF);

	}

	/* Video */
	if (status & VID_BC_MSK_RISCI1) {
		spin_lock(&dev->slock);
		count = cx_read(VID_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vidq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	/* Allow the VBI framework to process it's payload */
	handled += cx23885_vbi_irq(dev, status);

	return handled;
}

/* ----------------------------------------------------------- */
/* exported stuff                                              */

static const struct v4l2_file_operations video_fops = {
	.owner	       = THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

static const struct v4l2_ioctl_ops video_ioctl_ops = {
	.vidioc_querycap      = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap  = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap     = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap   = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap     = vidioc_s_fmt_vid_cap,
	.vidioc_g_fmt_vbi_cap     = cx23885_vbi_fmt,
	.vidioc_try_fmt_vbi_cap   = cx23885_vbi_fmt,
	.vidioc_s_fmt_vbi_cap     = cx23885_vbi_fmt,
	.vidioc_reqbufs       = vb2_ioctl_reqbufs,
	.vidioc_prepare_buf   = vb2_ioctl_prepare_buf,
	.vidioc_querybuf      = vb2_ioctl_querybuf,
	.vidioc_qbuf          = vb2_ioctl_qbuf,
	.vidioc_dqbuf         = vb2_ioctl_dqbuf,
	.vidioc_streamon      = vb2_ioctl_streamon,
	.vidioc_streamoff     = vb2_ioctl_streamoff,
	.vidioc_g_pixelaspect = vidioc_g_pixelaspect,
	.vidioc_g_selection   = vidioc_g_selection,
	.vidioc_s_std         = vidioc_s_std,
	.vidioc_g_std         = vidioc_g_std,
	.vidioc_enum_input    = vidioc_enum_input,
	.vidioc_g_input       = vidioc_g_input,
	.vidioc_s_input       = vidioc_s_input,
	.vidioc_log_status    = vidioc_log_status,
	.vidioc_g_tuner       = vidioc_g_tuner,
	.vidioc_s_tuner       = vidioc_s_tuner,
	.vidioc_g_frequency   = vidioc_g_frequency,
	.vidioc_s_frequency   = vidioc_s_frequency,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_chip_info   = cx23885_g_chip_info,
	.vidioc_g_register    = cx23885_g_register,
	.vidioc_s_register    = cx23885_s_register,
#endif
	.vidioc_enumaudio     = vidioc_enum_audinput,
	.vidioc_g_audio       = vidioc_g_audinput,
	.vidioc_s_audio       = vidioc_s_audinput,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};

static struct video_device cx23885_vbi_template;
static struct video_device cx23885_video_template = {
	.name                 = "cx23885-video",
	.fops                 = &video_fops,
	.ioctl_ops	      = &video_ioctl_ops,
	.tvnorms              = CX23885_NORMS,
};

void cx23885_video_unregister(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);
	cx23885_irq_remove(dev, 0x01);

	if (dev->vbi_dev) {
		if (video_is_registered(dev->vbi_dev))
			video_unregister_device(dev->vbi_dev);
		else
			video_device_release(dev->vbi_dev);
		dev->vbi_dev = NULL;
	}
	if (dev->video_dev) {
		if (video_is_registered(dev->video_dev))
			video_unregister_device(dev->video_dev);
		else
			video_device_release(dev->video_dev);
		dev->video_dev = NULL;
	}

	if (dev->audio_dev)
		cx23885_audio_unregister(dev);
}

int cx23885_video_register(struct cx23885_dev *dev)
{
	struct vb2_queue *q;
	int err;

	dprintk(1, "%s()\n", __func__);

	/* Initialize VBI template */
	cx23885_vbi_template = cx23885_video_template;
	strscpy(cx23885_vbi_template.name, "cx23885-vbi",
		sizeof(cx23885_vbi_template.name));

	dev->tvnorm = V4L2_STD_NTSC_M;
	dev->fmt = format_by_fourcc(V4L2_PIX_FMT_YUYV);
	dev->field = V4L2_FIELD_INTERLACED;
	dev->width = 720;
	dev->height = norm_maxh(dev->tvnorm);

	/* init video dma queues */
	INIT_LIST_HEAD(&dev->vidq.active);

	/* init vbi dma queues */
	INIT_LIST_HEAD(&dev->vbiq.active);

	cx23885_irq_add_enable(dev, 0x01);

	if ((TUNER_ABSENT != dev->tuner_type) &&
			((dev->tuner_bus == 0) || (dev->tuner_bus == 1))) {
		struct v4l2_subdev *sd = NULL;

		if (dev->tuner_addr)
			sd = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[dev->tuner_bus].i2c_adap,
				"tuner", dev->tuner_addr, NULL);
		else
			sd = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[dev->tuner_bus].i2c_adap,
				"tuner", 0, v4l2_i2c_tuner_addrs(ADDRS_TV));
		if (sd) {
			struct tuner_setup tun_setup;

			memset(&tun_setup, 0, sizeof(tun_setup));
			tun_setup.mode_mask = T_ANALOG_TV;
			tun_setup.type = dev->tuner_type;
			tun_setup.addr = v4l2_i2c_subdev_addr(sd);
			tun_setup.tuner_callback = cx23885_tuner_callback;

			v4l2_subdev_call(sd, tuner, s_type_addr, &tun_setup);

			if ((dev->board == CX23885_BOARD_LEADTEK_WINFAST_PXTV1200) ||
			    (dev->board == CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200)) {
				struct xc2028_ctrl ctrl = {
					.fname = XC2028_DEFAULT_FIRMWARE,
					.max_len = 64
				};
				struct v4l2_priv_tun_config cfg = {
					.tuner = dev->tuner_type,
					.priv = &ctrl
				};
				v4l2_subdev_call(sd, tuner, s_config, &cfg);
			}

			if (dev->board == CX23885_BOARD_AVERMEDIA_HC81R) {
				struct xc2028_ctrl ctrl = {
					.fname = "xc3028L-v36.fw",
					.max_len = 64
				};
				struct v4l2_priv_tun_config cfg = {
					.tuner = dev->tuner_type,
					.priv = &ctrl
				};
				v4l2_subdev_call(sd, tuner, s_config, &cfg);
			}
		}
	}

	/* initial device configuration */
	mutex_lock(&dev->lock);
	cx23885_set_tvnorm(dev, dev->tvnorm);
	cx23885_video_mux(dev, 0);
	cx23885_audio_mux(dev, 0);
	mutex_unlock(&dev->lock);

	q = &dev->vb2_vidq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_video_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		goto fail_unreg;

	q = &dev->vb2_vbiq;
	q->type = V4L2_BUF_TYPE_VBI_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_vbi_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		goto fail_unreg;

	/* register Video device */
	dev->video_dev = cx23885_vdev_init(dev, dev->pci,
		&cx23885_video_template, "video");
	dev->video_dev->queue = &dev->vb2_vidq;
	dev->video_dev->device_caps = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
				      V4L2_CAP_AUDIO | V4L2_CAP_VIDEO_CAPTURE;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		dev->video_dev->device_caps |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			dev->video_dev->device_caps |= V4L2_CAP_TUNER;
	}

	err = video_register_device(dev->video_dev, VFL_TYPE_VIDEO,
				    video_nr[dev->nr]);
	if (err < 0) {
		pr_info("%s: can't register video device\n",
			dev->name);
		goto fail_unreg;
	}
	pr_info("%s: registered device %s [v4l2]\n",
	       dev->name, video_device_node_name(dev->video_dev));

	/* register VBI device */
	dev->vbi_dev = cx23885_vdev_init(dev, dev->pci,
		&cx23885_vbi_template, "vbi");
	dev->vbi_dev->queue = &dev->vb2_vbiq;
	dev->vbi_dev->device_caps = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
				    V4L2_CAP_AUDIO | V4L2_CAP_VBI_CAPTURE;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		dev->vbi_dev->device_caps |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			dev->vbi_dev->device_caps |= V4L2_CAP_TUNER;
	}
	err = video_register_device(dev->vbi_dev, VFL_TYPE_VBI,
				    vbi_nr[dev->nr]);
	if (err < 0) {
		pr_info("%s: can't register vbi device\n",
			dev->name);
		goto fail_unreg;
	}
	pr_info("%s: registered device %s\n",
	       dev->name, video_device_node_name(dev->vbi_dev));

	/* Register ALSA audio device */
	dev->audio_dev = cx23885_audio_register(dev);

	return 0;

fail_unreg:
	cx23885_video_unregister(dev);
	return err;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"
#include "cx23885-video.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kmod.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/div64.h>

#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-event.h>
#include "cx23885-ioctl.h"
#include "xc2028.h"

#include <media/drv-intf/cx25840.h>

MODULE_DESCRIPTION("v4l2 driver module for cx23885 based TV cards");
MODULE_AUTHOR("Steven Toth <stoth@linuxtv.org>");
MODULE_LICENSE("GPL");

/* ------------------------------------------------------------------ */

static unsigned int video_nr[] = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };
static unsigned int vbi_nr[]   = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };

module_param_array(video_nr, int, NULL, 0444);
module_param_array(vbi_nr,   int, NULL, 0444);

MODULE_PARM_DESC(video_nr, "video device numbers");
MODULE_PARM_DESC(vbi_nr, "vbi device numbers");

static unsigned int video_debug;
module_param(video_debug, int, 0644);
MODULE_PARM_DESC(video_debug, "enable debug messages [video]");

static unsigned int irq_debug;
module_param(irq_debug, int, 0644);
MODULE_PARM_DESC(irq_debug, "enable debug messages [IRQ handler]");

static unsigned int vid_limit = 16;
module_param(vid_limit, int, 0644);
MODULE_PARM_DESC(vid_limit, "capture memory limit in megabytes");

#define dprintk(level, fmt, arg...)\
	do { if (video_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: video:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------- */
/* static data                                                         */

#define FORMAT_FLAGS_PACKED       0x01
static struct cx23885_fmt formats[] = {
	{
		.fourcc   = V4L2_PIX_FMT_YUYV,
		.depth    = 16,
		.flags    = FORMAT_FLAGS_PACKED,
	}
};

static struct cx23885_fmt *format_by_fourcc(unsigned int fourcc)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(formats); i++)
		if (formats[i].fourcc == fourcc)
			return formats+i;
	return NULL;
}

/* ------------------------------------------------------------------- */

void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count)
{
	struct cx23885_buffer *buf;

	if (list_empty(&q->active))
		return;
	buf = list_entry(q->active.next,
			struct cx23885_buffer, queue);

	buf->vb.sequence = q->count++;
	buf->vb.vb2_buf.timestamp = ktime_get_ns();
	dprintk(2, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
			buf->vb.vb2_buf.index, count, q->count);
	list_del(&buf->queue);
	vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
}

int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm)
{
	struct v4l2_subdev_format format = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
		.format.code = MEDIA_BUS_FMT_FIXED,
	};

	dprintk(1, "%s(norm = 0x%08x) name: [%s]\n",
		__func__,
		(unsigned int)norm,
		v4l2_norm_to_name(norm));

	if (dev->tvnorm == norm)
		return 0;

	if (dev->tvnorm != norm) {
		if (vb2_is_busy(&dev->vb2_vidq) || vb2_is_busy(&dev->vb2_vbiq) ||
		    vb2_is_busy(&dev->vb2_mpegq))
			return -EBUSY;
	}

	dev->tvnorm = norm;
	dev->width = 720;
	dev->height = norm_maxh(norm);
	dev->field = V4L2_FIELD_INTERLACED;

	call_all(dev, video, s_std, norm);

	format.format.width = dev->width;
	format.format.height = dev->height;
	format.format.field = dev->field;
	call_all(dev, pad, set_fmt, NULL, &format);

	return 0;
}

static struct video_device *cx23885_vdev_init(struct cx23885_dev *dev,
				    struct pci_dev *pci,
				    struct video_device *template,
				    char *type)
{
	struct video_device *vfd;
	dprintk(1, "%s()\n", __func__);

	vfd = video_device_alloc();
	if (NULL == vfd)
		return NULL;
	*vfd = *template;
	vfd->v4l2_dev = &dev->v4l2_dev;
	vfd->release = video_device_release;
	vfd->lock = &dev->lock;
	snprintf(vfd->name, sizeof(vfd->name), "%s (%s)",
		 cx23885_boards[dev->board].name, type);
	video_set_drvdata(vfd, dev);
	return vfd;
}

int cx23885_flatiron_write(struct cx23885_dev *dev, u8 reg, u8 data)
{
	/* 8 bit registers, 8 bit values */
	u8 buf[] = { reg, data };

	struct i2c_msg msg = { .addr = 0x98 >> 1,
		.flags = 0, .buf = buf, .len = 2 };

	return i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}

u8 cx23885_flatiron_read(struct cx23885_dev *dev, u8 reg)
{
	/* 8 bit registers, 8 bit values */
	int ret;
	u8 b0[] = { reg };
	u8 b1[] = { 0 };

	struct i2c_msg msg[] = {
		{ .addr = 0x98 >> 1, .flags = 0, .buf = b0, .len = 1 },
		{ .addr = 0x98 >> 1, .flags = I2C_M_RD, .buf = b1, .len = 1 }
	};

	ret = i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg[0], 2);
	if (ret != 2)
		pr_err("%s() error\n", __func__);

	return b1[0];
}

static void cx23885_flatiron_dump(struct cx23885_dev *dev)
{
	int i;
	dprintk(1, "Flatiron dump\n");
	for (i = 0; i < 0x24; i++) {
		dprintk(1, "FI[%02x] = %02x\n", i,
			cx23885_flatiron_read(dev, i));
	}
}

static int cx23885_flatiron_mux(struct cx23885_dev *dev, int input)
{
	u8 val;
	dprintk(1, "%s(input = %d)\n", __func__, input);

	if (input == 1)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) & ~FLD_CH_SEL;
	else if (input == 2)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) | FLD_CH_SEL;
	else
		return -EINVAL;

	val |= 0x20; /* Enable clock to delta-sigma and dec filter */

	cx23885_flatiron_write(dev, CH_PWR_CTRL1, val);

	/* Wake up */
	cx23885_flatiron_write(dev, CH_PWR_CTRL2, 0);

	if (video_debug)
		cx23885_flatiron_dump(dev);

	return 0;
}

static int cx23885_video_mux(struct cx23885_dev *dev, unsigned int input)
{
	dprintk(1, "%s() video_mux: %d [vmux=%d, gpio=0x%x,0x%x,0x%x,0x%x]\n",
		__func__,
		input, INPUT(input)->vmux,
		INPUT(input)->gpio0, INPUT(input)->gpio1,
		INPUT(input)->gpio2, INPUT(input)->gpio3);
	dev->input = input;

	if (dev->board == CX23885_BOARD_MYGICA_X8506 ||
		dev->board == CX23885_BOARD_MAGICPRO_PROHDTVE2 ||
		dev->board == CX23885_BOARD_MYGICA_X8507) {
		/* Select Analog TV */
		if (INPUT(input)->type == CX23885_VMUX_TELEVISION)
			cx23885_gpio_clear(dev, GPIO_0);
	}

	/* Tell the internal A/V decoder */
	v4l2_subdev_call(dev->sd_cx25840, video, s_routing,
			INPUT(input)->vmux, 0, 0);

	if ((dev->board == CX23885_BOARD_HAUPPAUGE_HVR1800) ||
		(dev->board == CX23885_BOARD_MPX885) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1250) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_IMPACTVCBE) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255_22111) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1265_K4) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1850) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR5525) ||
		(dev->board == CX23885_BOARD_MYGICA_X8507) ||
		(dev->board == CX23885_BOARD_AVERMEDIA_HC81R) ||
		(dev->board == CX23885_BOARD_VIEWCAST_260E) ||
		(dev->board == CX23885_BOARD_VIEWCAST_460E) ||
		(dev->board == CX23885_BOARD_AVERMEDIA_CE310B)) {
		/* Configure audio routing */
		v4l2_subdev_call(dev->sd_cx25840, audio, s_routing,
			INPUT(input)->amux, 0, 0);

		if (INPUT(input)->amux == CX25840_AUDIO7)
			cx23885_flatiron_mux(dev, 1);
		else if (INPUT(input)->amux == CX25840_AUDIO6)
			cx23885_flatiron_mux(dev, 2);
	}

	return 0;
}

static int cx23885_audio_mux(struct cx23885_dev *dev, unsigned int input)
{
	dprintk(1, "%s(input=%d)\n", __func__, input);

	/* The baseband video core of the cx23885 has two audio inputs.
	 * LR1 and LR2. In almost every single case so far only HVR1xxx
	 * cards we've only ever supported LR1. Time to support LR2,
	 * which is available via the optional white breakout header on
	 * the board.
	 * We'll use a could of existing enums in the card struct to allow
	 * devs to specify which baseband input they need, or just default
	 * to what we've always used.
	 */
	if (INPUT(input)->amux == CX25840_AUDIO7)
		cx23885_flatiron_mux(dev, 1);
	else if (INPUT(input)->amux == CX25840_AUDIO6)
		cx23885_flatiron_mux(dev, 2);
	else {
		/* Not specifically defined, assume the default. */
		cx23885_flatiron_mux(dev, 1);
	}

	return 0;
}

/* ------------------------------------------------------------------ */
static int cx23885_start_video_dma(struct cx23885_dev *dev,
			   struct cx23885_dmaqueue *q,
			   struct cx23885_buffer *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* Stop the dma/fifo before we tamper with it's risc programs */
	cx_clear(VID_A_DMA_CTL, 0x11);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH01],
				buf->bpl, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000011);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x11); /* FIFO and RISC enable */

	return 0;
}

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;

	*num_planes = 1;
	sizes[0] = (dev->fmt->depth * dev->width * dev->height) >> 3;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);
	u32 line0_offset, line1_offset;
	struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, 0);
	int field_tff;

	buf->bpl = (dev->width * dev->fmt->depth) >> 3;

	if (vb2_plane_size(vb, 0) < dev->height * buf->bpl)
		return -EINVAL;
	vb2_set_plane_payload(vb, 0, dev->height * buf->bpl);

	switch (dev->field) {
	case V4L2_FIELD_TOP:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, 0, UNSET,
				buf->bpl, 0, dev->height);
		break;
	case V4L2_FIELD_BOTTOM:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, UNSET, 0,
				buf->bpl, 0, dev->height);
		break;
	case V4L2_FIELD_INTERLACED:
		if (dev->tvnorm & V4L2_STD_525_60)
			/* NTSC or  */
			field_tff = 1;
		else
			field_tff = 0;

		if (cx23885_boards[dev->board].force_bff)
			/* PAL / SECAM OR 888 in NTSC MODE */
			field_tff = 0;

		if (field_tff) {
			/* cx25840 transmits NTSC bottom field first */
			dprintk(1, "%s() Creating TFF/NTSC risc\n",
					__func__);
			line0_offset = buf->bpl;
			line1_offset = 0;
		} else {
			/* All other formats are top field first */
			dprintk(1, "%s() Creating BFF/PAL/SECAM risc\n",
					__func__);
			line0_offset = 0;
			line1_offset = buf->bpl;
		}
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, line0_offset,
				line1_offset,
				buf->bpl, buf->bpl,
				dev->height >> 1);
		break;
	case V4L2_FIELD_SEQ_TB:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl,
				0, buf->bpl * (dev->height >> 1),
				buf->bpl, 0,
				dev->height >> 1);
		break;
	case V4L2_FIELD_SEQ_BT:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl,
				buf->bpl * (dev->height >> 1), 0,
				buf->bpl, 0,
				dev->height >> 1);
		break;
	default:
		BUG();
	}
	dprintk(2, "[%p/%d] buffer_init - %dx%d %dbpp 0x%08x - dma=0x%08lx\n",
		buf, buf->vb.vb2_buf.index,
		dev->width, dev->height, dev->fmt->depth, dev->fmt->fourcc,
		(unsigned long)buf->risc.dma);
	return 0;
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(vb->vb2_queue->drv_priv, buf);
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back to the start + 12 (skipping
 * the initial JUMP).
 *
 * This is the risc program of the first buffer to be queued if the active list
 * is empty and it just keeps DMAing this buffer without generating any
 * interrupts.
 *
 * If a new buffer is added then the initial JUMP in the code for that buffer
 * will generate an interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct cx23885_buffer   *prev;
	struct cx23885_dmaqueue *q    = &dev->vidq;
	unsigned long flags;

	/* add jump to start */
	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	spin_lock_irqsave(&dev->slock, flags);
	if (list_empty(&q->active)) {
		list_add_tail(&buf->queue, &q->active);
		dprintk(2, "[%p/%d] buffer_queue - first active\n",
			buf, buf->vb.vb2_buf.index);
	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		list_add_tail(&buf->queue, &q->active);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
				buf, buf->vb.vb2_buf.index);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_video_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x11);
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static const struct vb2_ops cx23885_video_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

/* ------------------------------------------------------------------ */
/* VIDEO IOCTLS                                                       */

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.width        = dev->width;
	f->fmt.pix.height       = dev->height;
	f->fmt.pix.field        = dev->field;
	f->fmt.pix.pixelformat  = dev->fmt->fourcc;
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * dev->fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct cx23885_fmt *fmt;
	enum v4l2_field   field;
	unsigned int      maxw, maxh;

	fmt = format_by_fourcc(f->fmt.pix.pixelformat);
	if (NULL == fmt)
		return -EINVAL;

	field = f->fmt.pix.field;
	maxw  = 720;
	maxh  = norm_maxh(dev->tvnorm);

	if (V4L2_FIELD_ANY == field) {
		field = (f->fmt.pix.height > maxh/2)
			? V4L2_FIELD_INTERLACED
			: V4L2_FIELD_BOTTOM;
	}

	switch (field) {
	case V4L2_FIELD_TOP:
	case V4L2_FIELD_BOTTOM:
		maxh = maxh / 2;
		break;
	case V4L2_FIELD_INTERLACED:
	case V4L2_FIELD_SEQ_TB:
	case V4L2_FIELD_SEQ_BT:
		break;
	default:
		field = V4L2_FIELD_INTERLACED;
		break;
	}

	f->fmt.pix.field = field;
	v4l_bound_align_image(&f->fmt.pix.width, 48, maxw, 2,
			      &f->fmt.pix.height, 32, maxh, 0, 0);
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct v4l2_subdev_format format = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
	};
	int err;

	dprintk(2, "%s()\n", __func__);
	err = vidioc_try_fmt_vid_cap(file, priv, f);

	if (0 != err)
		return err;

	if (vb2_is_busy(&dev->vb2_vidq) || vb2_is_busy(&dev->vb2_vbiq) ||
	    vb2_is_busy(&dev->vb2_mpegq))
		return -EBUSY;

	dev->fmt        = format_by_fourcc(f->fmt.pix.pixelformat);
	dev->width      = f->fmt.pix.width;
	dev->height     = f->fmt.pix.height;
	dev->field	= f->fmt.pix.field;
	dprintk(2, "%s() width=%d height=%d field=%d\n", __func__,
		dev->width, dev->height, dev->field);
	v4l2_fill_mbus_format(&format.format, &f->fmt.pix, MEDIA_BUS_FMT_FIXED);
	call_all(dev, pad, set_fmt, NULL, &format);
	v4l2_fill_pix_format(&f->fmt.pix, &format.format);
	/* set_fmt overwrites f->fmt.pix.field, restore it */
	f->fmt.pix.field = dev->field;
	return 0;
}

static int vidioc_querycap(struct file *file, void  *priv,
	struct v4l2_capability *cap)
{
	struct cx23885_dev *dev = video_drvdata(file);

	strscpy(cap->driver, "cx23885", sizeof(cap->driver));
	strscpy(cap->card, cx23885_boards[dev->board].name,
		sizeof(cap->card));
	sprintf(cap->bus_info, "PCIe:%s", pci_name(dev->pci));
	cap->capabilities = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
			    V4L2_CAP_AUDIO | V4L2_CAP_VBI_CAPTURE |
			    V4L2_CAP_VIDEO_CAPTURE |
			    V4L2_CAP_DEVICE_CAPS;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		cap->capabilities |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			cap->capabilities |= V4L2_CAP_TUNER;
		break;
	}
	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
	struct v4l2_fmtdesc *f)
{
	if (unlikely(f->index >= ARRAY_SIZE(formats)))
		return -EINVAL;

	f->pixelformat = formats[f->index].fourcc;

	return 0;
}

static int vidioc_g_pixelaspect(struct file *file, void *priv,
				int type, struct v4l2_fract *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	bool is_50hz = dev->tvnorm & V4L2_STD_625_50;

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	f->numerator = is_50hz ? 54 : 11;
	f->denominator = is_50hz ? 59 : 10;

	return 0;
}

static int vidioc_g_selection(struct file *file, void *fh,
			      struct v4l2_selection *sel)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (sel->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	switch (sel->target) {
	case V4L2_SEL_TGT_CROP_BOUNDS:
	case V4L2_SEL_TGT_CROP_DEFAULT:
		sel->r.top = 0;
		sel->r.left = 0;
		sel->r.width = 720;
		sel->r.height = norm_maxh(dev->tvnorm);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);

	*id = dev->tvnorm;
	return 0;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id tvnorms)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);

	return cx23885_set_tvnorm(dev, tvnorms);
}

int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i)
{
	static const char *iname[] = {
		[CX23885_VMUX_COMPOSITE1] = "Composite1",
		[CX23885_VMUX_COMPOSITE2] = "Composite2",
		[CX23885_VMUX_COMPOSITE3] = "Composite3",
		[CX23885_VMUX_COMPOSITE4] = "Composite4",
		[CX23885_VMUX_SVIDEO]     = "S-Video",
		[CX23885_VMUX_COMPONENT]  = "Component",
		[CX23885_VMUX_TELEVISION] = "Television",
		[CX23885_VMUX_CABLE]      = "Cable TV",
		[CX23885_VMUX_DVB]        = "DVB",
		[CX23885_VMUX_DEBUG]      = "for debug only",
	};
	unsigned int n;
	dprintk(1, "%s()\n", __func__);

	n = i->index;
	if (n >= MAX_CX23885_INPUT)
		return -EINVAL;

	if (0 == INPUT(n)->type)
		return -EINVAL;

	i->index = n;
	i->type  = V4L2_INPUT_TYPE_CAMERA;
	strscpy(i->name, iname[INPUT(n)->type], sizeof(i->name));
	i->std = CX23885_NORMS;
	if ((CX23885_VMUX_TELEVISION == INPUT(n)->type) ||
		(CX23885_VMUX_CABLE == INPUT(n)->type)) {
		i->type = V4L2_INPUT_TYPE_TUNER;
		i->audioset = 4;
	} else {
		/* Two selectable audio inputs for non-tv inputs */
		i->audioset = 3;
	}

	if (dev->input == n) {
		/* enum'd input matches our configured input.
		 * Ask the video decoder to process the call
		 * and give it an oppertunity to update the
		 * status field.
		 */
		call_all(dev, video, g_input_status, &i->status);
	}

	return 0;
}

static int vidioc_enum_input(struct file *file, void *priv,
				struct v4l2_input *i)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);
	return cx23885_enum_input(dev, i);
}

int cx23885_get_input(struct file *file, void *priv, unsigned int *i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	*i = dev->input;
	dprintk(1, "%s() returns %d\n", __func__, *i);
	return 0;
}

static int vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
	return cx23885_get_input(file, priv, i);
}

int cx23885_set_input(struct file *file, void *priv, unsigned int i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	dprintk(1, "%s(%d)\n", __func__, i);

	if (i >= MAX_CX23885_INPUT) {
		dprintk(1, "%s() -EINVAL\n", __func__);
		return -EINVAL;
	}

	if (INPUT(i)->type == 0)
		return -EINVAL;

	cx23885_video_mux(dev, i);

	/* By default establish the default audio input for the card also */
	/* Caller is free to use VIDIOC_S_AUDIO to override afterwards */
	cx23885_audio_mux(dev, i);
	return 0;
}

static int vidioc_s_input(struct file *file, void *priv, unsigned int i)
{
	return cx23885_set_input(file, priv, i);
}

static int vidioc_log_status(struct file *file, void *priv)
{
	struct cx23885_dev *dev = video_drvdata(file);

	call_all(dev, core, log_status);
	return 0;
}

static int cx23885_query_audinput(struct file *file, void *priv,
	struct v4l2_audio *i)
{
	static const char *iname[] = {
		[0] = "Baseband L/R 1",
		[1] = "Baseband L/R 2",
		[2] = "TV",
	};
	unsigned int n;
	dprintk(1, "%s()\n", __func__);

	n = i->index;
	if (n >= 3)
		return -EINVAL;

	memset(i, 0, sizeof(*i));
	i->index = n;
	strscpy(i->name, iname[n], sizeof(i->name));
	i->capability = V4L2_AUDCAP_STEREO;
	return 0;

}

static int vidioc_enum_audinput(struct file *file, void *priv,
				struct v4l2_audio *i)
{
	return cx23885_query_audinput(file, priv, i);
}

static int vidioc_g_audinput(struct file *file, void *priv,
	struct v4l2_audio *i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if ((CX23885_VMUX_TELEVISION == INPUT(dev->input)->type) ||
		(CX23885_VMUX_CABLE == INPUT(dev->input)->type))
		i->index = 2;
	else
		i->index = dev->audinput;
	dprintk(1, "%s(input=%d)\n", __func__, i->index);

	return cx23885_query_audinput(file, priv, i);
}

static int vidioc_s_audinput(struct file *file, void *priv,
	const struct v4l2_audio *i)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if ((CX23885_VMUX_TELEVISION == INPUT(dev->input)->type) ||
		(CX23885_VMUX_CABLE == INPUT(dev->input)->type)) {
		return i->index != 2 ? -EINVAL : 0;
	}
	if (i->index > 1)
		return -EINVAL;

	dprintk(1, "%s(%d)\n", __func__, i->index);

	dev->audinput = i->index;

	/* Skip the audio defaults from the cards struct, caller wants
	 * directly touch the audio mux hardware. */
	cx23885_flatiron_mux(dev, dev->audinput + 1);
	return 0;
}

static int vidioc_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	if (0 != t->index)
		return -EINVAL;

	strscpy(t->name, "Television", sizeof(t->name));

	call_all(dev, tuner, g_tuner, t);
	return 0;
}

static int vidioc_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	if (0 != t->index)
		return -EINVAL;
	/* Update the A/V core */
	call_all(dev, tuner, s_tuner, t);

	return 0;
}

static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	f->type = V4L2_TUNER_ANALOG_TV;
	f->frequency = dev->freq;

	call_all(dev, tuner, g_frequency, f);

	return 0;
}

static int cx23885_set_freq(struct cx23885_dev *dev, const struct v4l2_frequency *f)
{
	struct v4l2_ctrl *mute;
	int old_mute_val = 1;

	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
		break;
	}
	if (unlikely(f->tuner != 0))
		return -EINVAL;

	dev->freq = f->frequency;

	/* I need to mute audio here */
	mute = v4l2_ctrl_find(&dev->ctrl_handler, V4L2_CID_AUDIO_MUTE);
	if (mute) {
		old_mute_val = v4l2_ctrl_g_ctrl(mute);
		if (!old_mute_val)
			v4l2_ctrl_s_ctrl(mute, 1);
	}

	call_all(dev, tuner, s_frequency, f);

	/* When changing channels it is required to reset TVAUDIO */
	msleep(100);

	/* I need to unmute audio here */
	if (old_mute_val == 0)
		v4l2_ctrl_s_ctrl(mute, old_mute_val);

	return 0;
}

static int cx23885_set_freq_via_ops(struct cx23885_dev *dev,
	const struct v4l2_frequency *f)
{
	struct v4l2_ctrl *mute;
	int old_mute_val = 1;
	struct vb2_dvb_frontend *vfe;
	struct dvb_frontend *fe;

	struct analog_parameters params = {
		.mode      = V4L2_TUNER_ANALOG_TV,
		.audmode   = V4L2_TUNER_MODE_STEREO,
		.std       = dev->tvnorm,
		.frequency = f->frequency
	};

	dev->freq = f->frequency;

	/* I need to mute audio here */
	mute = v4l2_ctrl_find(&dev->ctrl_handler, V4L2_CID_AUDIO_MUTE);
	if (mute) {
		old_mute_val = v4l2_ctrl_g_ctrl(mute);
		if (!old_mute_val)
			v4l2_ctrl_s_ctrl(mute, 1);
	}

	/* If HVR1850 */
	dprintk(1, "%s() frequency=%d tuner=%d std=0x%llx\n", __func__,
		params.frequency, f->tuner, params.std);

	vfe = vb2_dvb_get_frontend(&dev->ts2.frontends, 1);
	if (!vfe) {
		return -EINVAL;
	}

	fe = vfe->dvb.frontend;

	if ((dev->board == CX23885_BOARD_HAUPPAUGE_HVR1850) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255_22111) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR1265_K4) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_HVR5525) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB) ||
	    (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC))
		fe = &dev->ts1.analog_fe;

	if (fe && fe->ops.tuner_ops.set_analog_params) {
		call_all(dev, video, s_std, dev->tvnorm);
		fe->ops.tuner_ops.set_analog_params(fe, &params);
	}
	else
		pr_err("%s() No analog tuner, aborting\n", __func__);

	/* When changing channels it is required to reset TVAUDIO */
	msleep(100);

	/* I need to unmute audio here */
	if (old_mute_val == 0)
		v4l2_ctrl_s_ctrl(mute, old_mute_val);

	return 0;
}

int cx23885_set_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	int ret;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		ret = cx23885_set_freq_via_ops(dev, f);
		break;
	default:
		ret = cx23885_set_freq(dev, f);
	}

	return ret;
}

static int vidioc_s_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	return cx23885_set_frequency(file, priv, f);
}

/* ----------------------------------------------------------- */

int cx23885_video_irq(struct cx23885_dev *dev, u32 status)
{
	u32 mask, count;
	int handled = 0;

	mask   = cx_read(VID_A_INT_MSK);
	if (0 == (status & mask))
		return handled;

	cx_write(VID_A_INT_STAT, status);

	/* risc op code error, fifo overflow or line sync detection error */
	if ((status & VID_BC_MSK_OPC_ERR) ||
		(status & VID_BC_MSK_SYNC) ||
		(status & VID_BC_MSK_OF)) {

		if (status & VID_BC_MSK_OPC_ERR) {
			dprintk(7, " (VID_BC_MSK_OPC_ERR 0x%08x)\n",
				VID_BC_MSK_OPC_ERR);
			pr_warn("%s: video risc op code error\n",
				dev->name);
			cx23885_sram_channel_dump(dev,
				&dev->sram_channels[SRAM_CH01]);
		}

		if (status & VID_BC_MSK_SYNC)
			dprintk(7, " (VID_BC_MSK_SYNC 0x%08x) video lines miss-match\n",
				VID_BC_MSK_SYNC);

		if (status & VID_BC_MSK_OF)
			dprintk(7, " (VID_BC_MSK_OF 0x%08x) fifo overflow\n",
				VID_BC_MSK_OF);

	}

	/* Video */
	if (status & VID_BC_MSK_RISCI1) {
		spin_lock(&dev->slock);
		count = cx_read(VID_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vidq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	/* Allow the VBI framework to process it's payload */
	handled += cx23885_vbi_irq(dev, status);

	return handled;
}

/* ----------------------------------------------------------- */
/* exported stuff                                              */

static const struct v4l2_file_operations video_fops = {
	.owner	       = THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

static const struct v4l2_ioctl_ops video_ioctl_ops = {
	.vidioc_querycap      = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap  = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap     = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap   = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap     = vidioc_s_fmt_vid_cap,
	.vidioc_g_fmt_vbi_cap     = cx23885_vbi_fmt,
	.vidioc_try_fmt_vbi_cap   = cx23885_vbi_fmt,
	.vidioc_s_fmt_vbi_cap     = cx23885_vbi_fmt,
	.vidioc_reqbufs       = vb2_ioctl_reqbufs,
	.vidioc_prepare_buf   = vb2_ioctl_prepare_buf,
	.vidioc_querybuf      = vb2_ioctl_querybuf,
	.vidioc_qbuf          = vb2_ioctl_qbuf,
	.vidioc_dqbuf         = vb2_ioctl_dqbuf,
	.vidioc_streamon      = vb2_ioctl_streamon,
	.vidioc_streamoff     = vb2_ioctl_streamoff,
	.vidioc_g_pixelaspect = vidioc_g_pixelaspect,
	.vidioc_g_selection   = vidioc_g_selection,
	.vidioc_s_std         = vidioc_s_std,
	.vidioc_g_std         = vidioc_g_std,
	.vidioc_enum_input    = vidioc_enum_input,
	.vidioc_g_input       = vidioc_g_input,
	.vidioc_s_input       = vidioc_s_input,
	.vidioc_log_status    = vidioc_log_status,
	.vidioc_g_tuner       = vidioc_g_tuner,
	.vidioc_s_tuner       = vidioc_s_tuner,
	.vidioc_g_frequency   = vidioc_g_frequency,
	.vidioc_s_frequency   = vidioc_s_frequency,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_chip_info   = cx23885_g_chip_info,
	.vidioc_g_register    = cx23885_g_register,
	.vidioc_s_register    = cx23885_s_register,
#endif
	.vidioc_enumaudio     = vidioc_enum_audinput,
	.vidioc_g_audio       = vidioc_g_audinput,
	.vidioc_s_audio       = vidioc_s_audinput,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};

static struct video_device cx23885_vbi_template;
static struct video_device cx23885_video_template = {
	.name                 = "cx23885-video",
	.fops                 = &video_fops,
	.ioctl_ops	      = &video_ioctl_ops,
	.tvnorms              = CX23885_NORMS,
};

void cx23885_video_unregister(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);
	cx23885_irq_remove(dev, 0x01);

	if (dev->vbi_dev) {
		if (video_is_registered(dev->vbi_dev))
			video_unregister_device(dev->vbi_dev);
		else
			video_device_release(dev->vbi_dev);
		dev->vbi_dev = NULL;
	}
	if (dev->video_dev) {
		if (video_is_registered(dev->video_dev))
			video_unregister_device(dev->video_dev);
		else
			video_device_release(dev->video_dev);
		dev->video_dev = NULL;
	}

	if (dev->audio_dev)
		cx23885_audio_unregister(dev);
}

int cx23885_video_register(struct cx23885_dev *dev)
{
	struct vb2_queue *q;
	int err;

	dprintk(1, "%s()\n", __func__);

	/* Initialize VBI template */
	cx23885_vbi_template = cx23885_video_template;
	strscpy(cx23885_vbi_template.name, "cx23885-vbi",
		sizeof(cx23885_vbi_template.name));

	dev->tvnorm = V4L2_STD_NTSC_M;
	dev->fmt = format_by_fourcc(V4L2_PIX_FMT_YUYV);
	dev->field = V4L2_FIELD_INTERLACED;
	dev->width = 720;
	dev->height = norm_maxh(dev->tvnorm);

	/* init video dma queues */
	INIT_LIST_HEAD(&dev->vidq.active);

	/* init vbi dma queues */
	INIT_LIST_HEAD(&dev->vbiq.active);

	cx23885_irq_add_enable(dev, 0x01);

	if ((TUNER_ABSENT != dev->tuner_type) &&
			((dev->tuner_bus == 0) || (dev->tuner_bus == 1))) {
		struct v4l2_subdev *sd = NULL;

		if (dev->tuner_addr)
			sd = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[dev->tuner_bus].i2c_adap,
				"tuner", dev->tuner_addr, NULL);
		else
			sd = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[dev->tuner_bus].i2c_adap,
				"tuner", 0, v4l2_i2c_tuner_addrs(ADDRS_TV));
		if (sd) {
			struct tuner_setup tun_setup;

			memset(&tun_setup, 0, sizeof(tun_setup));
			tun_setup.mode_mask = T_ANALOG_TV;
			tun_setup.type = dev->tuner_type;
			tun_setup.addr = v4l2_i2c_subdev_addr(sd);
			tun_setup.tuner_callback = cx23885_tuner_callback;

			v4l2_subdev_call(sd, tuner, s_type_addr, &tun_setup);

			if ((dev->board == CX23885_BOARD_LEADTEK_WINFAST_PXTV1200) ||
			    (dev->board == CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200)) {
				struct xc2028_ctrl ctrl = {
					.fname = XC2028_DEFAULT_FIRMWARE,
					.max_len = 64
				};
				struct v4l2_priv_tun_config cfg = {
					.tuner = dev->tuner_type,
					.priv = &ctrl
				};
				v4l2_subdev_call(sd, tuner, s_config, &cfg);
			}

			if (dev->board == CX23885_BOARD_AVERMEDIA_HC81R) {
				struct xc2028_ctrl ctrl = {
					.fname = "xc3028L-v36.fw",
					.max_len = 64
				};
				struct v4l2_priv_tun_config cfg = {
					.tuner = dev->tuner_type,
					.priv = &ctrl
				};
				v4l2_subdev_call(sd, tuner, s_config, &cfg);
			}
		}
	}

	/* initial device configuration */
	mutex_lock(&dev->lock);
	cx23885_set_tvnorm(dev, dev->tvnorm);
	cx23885_video_mux(dev, 0);
	cx23885_audio_mux(dev, 0);
	mutex_unlock(&dev->lock);

	q = &dev->vb2_vidq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_video_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		goto fail_unreg;

	q = &dev->vb2_vbiq;
	q->type = V4L2_BUF_TYPE_VBI_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_vbi_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		goto fail_unreg;

	/* register Video device */
	dev->video_dev = cx23885_vdev_init(dev, dev->pci,
		&cx23885_video_template, "video");
	dev->video_dev->queue = &dev->vb2_vidq;
	dev->video_dev->device_caps = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
				      V4L2_CAP_AUDIO | V4L2_CAP_VIDEO_CAPTURE;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		dev->video_dev->device_caps |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			dev->video_dev->device_caps |= V4L2_CAP_TUNER;
	}

	err = video_register_device(dev->video_dev, VFL_TYPE_VIDEO,
				    video_nr[dev->nr]);
	if (err < 0) {
		pr_info("%s: can't register video device\n",
			dev->name);
		goto fail_unreg;
	}
	pr_info("%s: registered device %s [v4l2]\n",
	       dev->name, video_device_node_name(dev->video_dev));

	/* register VBI device */
	dev->vbi_dev = cx23885_vdev_init(dev, dev->pci,
		&cx23885_vbi_template, "vbi");
	dev->vbi_dev->queue = &dev->vb2_vbiq;
	dev->vbi_dev->device_caps = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
				    V4L2_CAP_AUDIO | V4L2_CAP_VBI_CAPTURE;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		dev->vbi_dev->device_caps |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			dev->vbi_dev->device_caps |= V4L2_CAP_TUNER;
	}
	err = video_register_device(dev->vbi_dev, VFL_TYPE_VBI,
				    vbi_nr[dev->nr]);
	if (err < 0) {
		pr_info("%s: can't register vbi device\n",
			dev->name);
		goto fail_unreg;
	}
	pr_info("%s: registered device %s\n",
	       dev->name, video_device_node_name(dev->vbi_dev));

	/* Register ALSA audio device */
	dev->audio_dev = cx23885_audio_register(dev);

	return 0;

fail_unreg:
	cx23885_video_unregister(dev);
	return err;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_VIDEO_H_
#define _CX23885_VIDEO_H_
int cx23885_flatiron_write(struct cx23885_dev *dev, u8 reg, u8 data);
u8 cx23885_flatiron_read(struct cx23885_dev *dev, u8 reg);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_VIDEO_H_
#define _CX23885_VIDEO_H_
int cx23885_flatiron_write(struct cx23885_dev *dev, u8 reg, u8 data);
u8 cx23885_flatiron_read(struct cx23885_dev *dev, u8 reg);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/pci.h>
#include <linux/i2c.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ctrls.h>
#include <media/tuner.h>
#include <media/tveeprom.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dvb.h>
#include <media/rc-core.h>

#include "cx23885-reg.h"
#include "media/drv-intf/cx2341x.h"

#include <linux/mutex.h>

#define CX23885_VERSION "0.0.4"

#define UNSET (-1U)

#define CX23885_MAXBOARDS 8

/* Max number of inputs by card */
#define MAX_CX23885_INPUT 8
#define INPUT(nr) (&cx23885_boards[dev->board].input[nr])

#define BUFFER_TIMEOUT     (HZ)  /* 0.5 seconds */

#define CX23885_BOARD_NOAUTO               UNSET
#define CX23885_BOARD_UNKNOWN                  0
#define CX23885_BOARD_HAUPPAUGE_HVR1800lp      1
#define CX23885_BOARD_HAUPPAUGE_HVR1800        2
#define CX23885_BOARD_HAUPPAUGE_HVR1250        3
#define CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP   4
#define CX23885_BOARD_HAUPPAUGE_HVR1500Q       5
#define CX23885_BOARD_HAUPPAUGE_HVR1500        6
#define CX23885_BOARD_HAUPPAUGE_HVR1200        7
#define CX23885_BOARD_HAUPPAUGE_HVR1700        8
#define CX23885_BOARD_HAUPPAUGE_HVR1400        9
#define CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP 10
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP 11
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H 12
#define CX23885_BOARD_COMPRO_VIDEOMATE_E650F   13
#define CX23885_BOARD_TBS_6920                 14
#define CX23885_BOARD_TEVII_S470               15
#define CX23885_BOARD_DVBWORLD_2005            16
#define CX23885_BOARD_NETUP_DUAL_DVBS2_CI      17
#define CX23885_BOARD_HAUPPAUGE_HVR1270        18
#define CX23885_BOARD_HAUPPAUGE_HVR1275        19
#define CX23885_BOARD_HAUPPAUGE_HVR1255        20
#define CX23885_BOARD_HAUPPAUGE_HVR1210        21
#define CX23885_BOARD_MYGICA_X8506             22
#define CX23885_BOARD_MAGICPRO_PROHDTVE2       23
#define CX23885_BOARD_HAUPPAUGE_HVR1850        24
#define CX23885_BOARD_COMPRO_VIDEOMATE_E800    25
#define CX23885_BOARD_HAUPPAUGE_HVR1290        26
#define CX23885_BOARD_MYGICA_X8558PRO          27
#define CX23885_BOARD_LEADTEK_WINFAST_PXTV1200 28
#define CX23885_BOARD_GOTVIEW_X5_3D_HYBRID     29
#define CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF 30
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000 31
#define CX23885_BOARD_MPX885                   32
#define CX23885_BOARD_MYGICA_X8507             33
#define CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL 34
#define CX23885_BOARD_TEVII_S471               35
#define CX23885_BOARD_HAUPPAUGE_HVR1255_22111  36
#define CX23885_BOARD_PROF_8000                37
#define CX23885_BOARD_HAUPPAUGE_HVR4400        38
#define CX23885_BOARD_AVERMEDIA_HC81R          39
#define CX23885_BOARD_TBS_6981                 40
#define CX23885_BOARD_TBS_6980                 41
#define CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200 42
#define CX23885_BOARD_HAUPPAUGE_IMPACTVCBE     43
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2 44
#define CX23885_BOARD_DVBSKY_T9580             45
#define CX23885_BOARD_DVBSKY_T980C             46
#define CX23885_BOARD_DVBSKY_S950C             47
#define CX23885_BOARD_TT_CT2_4500_CI           48
#define CX23885_BOARD_DVBSKY_S950              49
#define CX23885_BOARD_DVBSKY_S952              50
#define CX23885_BOARD_DVBSKY_T982              51
#define CX23885_BOARD_HAUPPAUGE_HVR5525        52
#define CX23885_BOARD_HAUPPAUGE_STARBURST      53
#define CX23885_BOARD_VIEWCAST_260E            54
#define CX23885_BOARD_VIEWCAST_460E            55
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB     56
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC    57
#define CX23885_BOARD_HAUPPAUGE_HVR1265_K4     58
#define CX23885_BOARD_HAUPPAUGE_STARBURST2     59
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885 60
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885 61
#define CX23885_BOARD_AVERMEDIA_CE310B         62

#define GPIO_0 0x00000001
#define GPIO_1 0x00000002
#define GPIO_2 0x00000004
#define GPIO_3 0x00000008
#define GPIO_4 0x00000010
#define GPIO_5 0x00000020
#define GPIO_6 0x00000040
#define GPIO_7 0x00000080
#define GPIO_8 0x00000100
#define GPIO_9 0x00000200
#define GPIO_10 0x00000400
#define GPIO_11 0x00000800
#define GPIO_12 0x00001000
#define GPIO_13 0x00002000
#define GPIO_14 0x00004000
#define GPIO_15 0x00008000

/* Currently unsupported by the driver: PAL/H, NTSC/Kr, SECAM B/G/H/LC */
#define CX23885_NORMS (\
	V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_443 | \
	V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK    |  V4L2_STD_PAL_I    | \
	V4L2_STD_PAL_M  |  V4L2_STD_PAL_N     |  V4L2_STD_PAL_Nc   | \
	V4L2_STD_PAL_60 |  V4L2_STD_SECAM_L   |  V4L2_STD_SECAM_DK)

struct cx23885_fmt {
	u32   fourcc;          /* v4l2 format id */
	int   depth;
	int   flags;
	u32   cxformat;
};

struct cx23885_tvnorm {
	char		*name;
	v4l2_std_id	id;
	u32		cxiformat;
	u32		cxoformat;
};

enum cx23885_itype {
	CX23885_VMUX_COMPOSITE1 = 1,
	CX23885_VMUX_COMPOSITE2,
	CX23885_VMUX_COMPOSITE3,
	CX23885_VMUX_COMPOSITE4,
	CX23885_VMUX_SVIDEO,
	CX23885_VMUX_COMPONENT,
	CX23885_VMUX_TELEVISION,
	CX23885_VMUX_CABLE,
	CX23885_VMUX_DVB,
	CX23885_VMUX_DEBUG,
	CX23885_RADIO,
};

enum cx23885_src_sel_type {
	CX23885_SRC_SEL_EXT_656_VIDEO = 0,
	CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO
};

struct cx23885_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

/* buffer for one video frame */
struct cx23885_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head queue;

	/* cx23885 specific */
	unsigned int           bpl;
	struct cx23885_riscmem risc;
	struct cx23885_fmt     *fmt;
	u32                    count;
};

struct cx23885_input {
	enum cx23885_itype type;
	unsigned int    vmux;
	unsigned int    amux;
	u32             gpio0, gpio1, gpio2, gpio3;
};

typedef enum {
	CX23885_MPEG_UNDEFINED = 0,
	CX23885_MPEG_DVB,
	CX23885_ANALOG_VIDEO,
	CX23885_MPEG_ENCODER,
} port_t;

struct cx23885_board {
	char                    *name;
	port_t			porta, portb, portc;
	int		num_fds_portb, num_fds_portc;
	unsigned int		tuner_type;
	unsigned int		radio_type;
	unsigned char		tuner_addr;
	unsigned char		radio_addr;
	unsigned int		tuner_bus;

	/* Vendors can and do run the PCIe bridge at different
	 * clock rates, driven physically by crystals on the PCBs.
	 * The core has to accommodate this. This allows the user
	 * to add new boards with new frequencys. The value is
	 * expressed in Hz.
	 *
	 * The core framework will default this value based on
	 * current designs, but it can vary.
	 */
	u32			clk_freq;
	struct cx23885_input    input[MAX_CX23885_INPUT];
	int			ci_type; /* for NetUP */
	/* Force bottom field first during DMA (888 workaround) */
	u32                     force_bff;
};

struct cx23885_subid {
	u16     subvendor;
	u16     subdevice;
	u32     card;
};

struct cx23885_i2c {
	struct cx23885_dev *dev;

	int                        nr;

	/* i2c i/o */
	struct i2c_adapter         i2c_adap;
	struct i2c_client          i2c_client;
	u32                        i2c_rc;

	/* 885 registers used for raw address */
	u32                        i2c_period;
	u32                        reg_ctrl;
	u32                        reg_stat;
	u32                        reg_addr;
	u32                        reg_rdata;
	u32                        reg_wdata;
};

struct cx23885_dmaqueue {
	struct list_head       active;
	u32                    count;
};

struct cx23885_tsport {
	struct cx23885_dev *dev;

	unsigned                   nr;
	int                        sram_chno;

	struct vb2_dvb_frontends   frontends;

	/* dma queues */
	struct cx23885_dmaqueue    mpegq;
	u32                        ts_packet_size;
	u32                        ts_packet_count;

	int                        width;
	int                        height;

	spinlock_t                 slock;

	/* registers */
	u32                        reg_gpcnt;
	u32                        reg_gpcnt_ctl;
	u32                        reg_dma_ctl;
	u32                        reg_lngth;
	u32                        reg_hw_sop_ctrl;
	u32                        reg_gen_ctrl;
	u32                        reg_bd_pkt_status;
	u32                        reg_sop_status;
	u32                        reg_fifo_ovfl_stat;
	u32                        reg_vld_misc;
	u32                        reg_ts_clk_en;
	u32                        reg_ts_int_msk;
	u32                        reg_ts_int_stat;
	u32                        reg_src_sel;

	/* Default register vals */
	int                        pci_irqmask;
	u32                        dma_ctl_val;
	u32                        ts_int_msk_val;
	u32                        gen_ctrl_val;
	u32                        ts_clk_en_val;
	u32                        src_sel_val;
	u32                        vld_misc_val;
	u32                        hw_sop_ctrl_val;

	/* Allow a single tsport to have multiple frontends */
	u32                        num_frontends;
	void                (*gate_ctrl)(struct cx23885_tsport *port, int open);
	void                       *port_priv;

	/* Workaround for a temp dvb_frontend that the tuner can attached to */
	struct dvb_frontend analog_fe;

	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	struct i2c_client *i2c_client_sec;
	struct i2c_client *i2c_client_ci;

	int (*set_frontend)(struct dvb_frontend *fe);
	int (*fe_set_voltage)(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage);
};

struct cx23885_kernel_ir {
	struct cx23885_dev	*cx;
	char			*name;
	char			*phys;

	struct rc_dev		*rc;
};

struct cx23885_audio_buffer {
	unsigned int		bpl;
	struct cx23885_riscmem	risc;
	void			*vaddr;
	struct scatterlist	*sglist;
	int			sglen;
	unsigned long		nr_pages;
};

struct cx23885_audio_dev {
	struct cx23885_dev	*dev;

	struct pci_dev		*pci;

	struct snd_card		*card;

	spinlock_t		lock;

	atomic_t		count;

	unsigned int		dma_size;
	unsigned int		period_size;
	unsigned int		num_periods;

	struct cx23885_audio_buffer   *buf;

	struct snd_pcm_substream *substream;
};

struct cx23885_dev {
	atomic_t                   refcount;
	struct v4l2_device	   v4l2_dev;
	struct v4l2_ctrl_handler   ctrl_handler;

	/* pci stuff */
	struct pci_dev             *pci;
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/pci.h>
#include <linux/i2c.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ctrls.h>
#include <media/tuner.h>
#include <media/tveeprom.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dvb.h>
#include <media/rc-core.h>

#include "cx23885-reg.h"
#include "media/drv-intf/cx2341x.h"

#include <linux/mutex.h>

#define CX23885_VERSION "0.0.4"

#define UNSET (-1U)

#define CX23885_MAXBOARDS 8

/* Max number of inputs by card */
#define MAX_CX23885_INPUT 8
#define INPUT(nr) (&cx23885_boards[dev->board].input[nr])

#define BUFFER_TIMEOUT     (HZ)  /* 0.5 seconds */

#define CX23885_BOARD_NOAUTO               UNSET
#define CX23885_BOARD_UNKNOWN                  0
#define CX23885_BOARD_HAUPPAUGE_HVR1800lp      1
#define CX23885_BOARD_HAUPPAUGE_HVR1800        2
#define CX23885_BOARD_HAUPPAUGE_HVR1250        3
#define CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP   4
#define CX23885_BOARD_HAUPPAUGE_HVR1500Q       5
#define CX23885_BOARD_HAUPPAUGE_HVR1500        6
#define CX23885_BOARD_HAUPPAUGE_HVR1200        7
#define CX23885_BOARD_HAUPPAUGE_HVR1700        8
#define CX23885_BOARD_HAUPPAUGE_HVR1400        9
#define CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP 10
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP 11
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H 12
#define CX23885_BOARD_COMPRO_VIDEOMATE_E650F   13
#define CX23885_BOARD_TBS_6920                 14
#define CX23885_BOARD_TEVII_S470               15
#define CX23885_BOARD_DVBWORLD_2005            16
#define CX23885_BOARD_NETUP_DUAL_DVBS2_CI      17
#define CX23885_BOARD_HAUPPAUGE_HVR1270        18
#define CX23885_BOARD_HAUPPAUGE_HVR1275        19
#define CX23885_BOARD_HAUPPAUGE_HVR1255        20
#define CX23885_BOARD_HAUPPAUGE_HVR1210        21
#define CX23885_BOARD_MYGICA_X8506             22
#define CX23885_BOARD_MAGICPRO_PROHDTVE2       23
#define CX23885_BOARD_HAUPPAUGE_HVR1850        24
#define CX23885_BOARD_COMPRO_VIDEOMATE_E800    25
#define CX23885_BOARD_HAUPPAUGE_HVR1290        26
#define CX23885_BOARD_MYGICA_X8558PRO          27
#define CX23885_BOARD_LEADTEK_WINFAST_PXTV1200 28
#define CX23885_BOARD_GOTVIEW_X5_3D_HYBRID     29
#define CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF 30
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000 31
#define CX23885_BOARD_MPX885                   32
#define CX23885_BOARD_MYGICA_X8507             33
#define CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL 34
#define CX23885_BOARD_TEVII_S471               35
#define CX23885_BOARD_HAUPPAUGE_HVR1255_22111  36
#define CX23885_BOARD_PROF_8000                37
#define CX23885_BOARD_HAUPPAUGE_HVR4400        38
#define CX23885_BOARD_AVERMEDIA_HC81R          39
#define CX23885_BOARD_TBS_6981                 40
#define CX23885_BOARD_TBS_6980                 41
#define CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200 42
#define CX23885_BOARD_HAUPPAUGE_IMPACTVCBE     43
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2 44
#define CX23885_BOARD_DVBSKY_T9580             45
#define CX23885_BOARD_DVBSKY_T980C             46
#define CX23885_BOARD_DVBSKY_S950C             47
#define CX23885_BOARD_TT_CT2_4500_CI           48
#define CX23885_BOARD_DVBSKY_S950              49
#define CX23885_BOARD_DVBSKY_S952              50
#define CX23885_BOARD_DVBSKY_T982              51
#define CX23885_BOARD_HAUPPAUGE_HVR5525        52
#define CX23885_BOARD_HAUPPAUGE_STARBURST      53
#define CX23885_BOARD_VIEWCAST_260E            54
#define CX23885_BOARD_VIEWCAST_460E            55
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB     56
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC    57
#define CX23885_BOARD_HAUPPAUGE_HVR1265_K4     58
#define CX23885_BOARD_HAUPPAUGE_STARBURST2     59
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885 60
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885 61
#define CX23885_BOARD_AVERMEDIA_CE310B         62

#define GPIO_0 0x00000001
#define GPIO_1 0x00000002
#define GPIO_2 0x00000004
#define GPIO_3 0x00000008
#define GPIO_4 0x00000010
#define GPIO_5 0x00000020
#define GPIO_6 0x00000040
#define GPIO_7 0x00000080
#define GPIO_8 0x00000100
#define GPIO_9 0x00000200
#define GPIO_10 0x00000400
#define GPIO_11 0x00000800
#define GPIO_12 0x00001000
#define GPIO_13 0x00002000
#define GPIO_14 0x00004000
#define GPIO_15 0x00008000

/* Currently unsupported by the driver: PAL/H, NTSC/Kr, SECAM B/G/H/LC */
#define CX23885_NORMS (\
	V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_443 | \
	V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK    |  V4L2_STD_PAL_I    | \
	V4L2_STD_PAL_M  |  V4L2_STD_PAL_N     |  V4L2_STD_PAL_Nc   | \
	V4L2_STD_PAL_60 |  V4L2_STD_SECAM_L   |  V4L2_STD_SECAM_DK)

struct cx23885_fmt {
	u32   fourcc;          /* v4l2 format id */
	int   depth;
	int   flags;
	u32   cxformat;
};

struct cx23885_tvnorm {
	char		*name;
	v4l2_std_id	id;
	u32		cxiformat;
	u32		cxoformat;
};

enum cx23885_itype {
	CX23885_VMUX_COMPOSITE1 = 1,
	CX23885_VMUX_COMPOSITE2,
	CX23885_VMUX_COMPOSITE3,
	CX23885_VMUX_COMPOSITE4,
	CX23885_VMUX_SVIDEO,
	CX23885_VMUX_COMPONENT,
	CX23885_VMUX_TELEVISION,
	CX23885_VMUX_CABLE,
	CX23885_VMUX_DVB,
	CX23885_VMUX_DEBUG,
	CX23885_RADIO,
};

enum cx23885_src_sel_type {
	CX23885_SRC_SEL_EXT_656_VIDEO = 0,
	CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO
};

struct cx23885_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

/* buffer for one video frame */
struct cx23885_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head queue;

	/* cx23885 specific */
	unsigned int           bpl;
	struct cx23885_riscmem risc;
	struct cx23885_fmt     *fmt;
	u32                    count;
};

struct cx23885_input {
	enum cx23885_itype type;
	unsigned int    vmux;
	unsigned int    amux;
	u32             gpio0, gpio1, gpio2, gpio3;
};

typedef enum {
	CX23885_MPEG_UNDEFINED = 0,
	CX23885_MPEG_DVB,
	CX23885_ANALOG_VIDEO,
	CX23885_MPEG_ENCODER,
} port_t;

struct cx23885_board {
	char                    *name;
	port_t			porta, portb, portc;
	int		num_fds_portb, num_fds_portc;
	unsigned int		tuner_type;
	unsigned int		radio_type;
	unsigned char		tuner_addr;
	unsigned char		radio_addr;
	unsigned int		tuner_bus;

	/* Vendors can and do run the PCIe bridge at different
	 * clock rates, driven physically by crystals on the PCBs.
	 * The core has to accommodate this. This allows the user
	 * to add new boards with new frequencys. The value is
	 * expressed in Hz.
	 *
	 * The core framework will default this value based on
	 * current designs, but it can vary.
	 */
	u32			clk_freq;
	struct cx23885_input    input[MAX_CX23885_INPUT];
	int			ci_type; /* for NetUP */
	/* Force bottom field first during DMA (888 workaround) */
	u32                     force_bff;
};

struct cx23885_subid {
	u16     subvendor;
	u16     subdevice;
	u32     card;
};

struct cx23885_i2c {
	struct cx23885_dev *dev;

	int                        nr;

	/* i2c i/o */
	struct i2c_adapter         i2c_adap;
	struct i2c_client          i2c_client;
	u32                        i2c_rc;

	/* 885 registers used for raw address */
	u32                        i2c_period;
	u32                        reg_ctrl;
	u32                        reg_stat;
	u32                        reg_addr;
	u32                        reg_rdata;
	u32                        reg_wdata;
};

struct cx23885_dmaqueue {
	struct list_head       active;
	u32                    count;
};

struct cx23885_tsport {
	struct cx23885_dev *dev;

	unsigned                   nr;
	int                        sram_chno;

	struct vb2_dvb_frontends   frontends;

	/* dma queues */
	struct cx23885_dmaqueue    mpegq;
	u32                        ts_packet_size;
	u32                        ts_packet_count;

	int                        width;
	int                        height;

	spinlock_t                 slock;

	/* registers */
	u32                        reg_gpcnt;
	u32                        reg_gpcnt_ctl;
	u32                        reg_dma_ctl;
	u32                        reg_lngth;
	u32                        reg_hw_sop_ctrl;
	u32                        reg_gen_ctrl;
	u32                        reg_bd_pkt_status;
	u32                        reg_sop_status;
	u32                        reg_fifo_ovfl_stat;
	u32                        reg_vld_misc;
	u32                        reg_ts_clk_en;
	u32                        reg_ts_int_msk;
	u32                        reg_ts_int_stat;
	u32                        reg_src_sel;

	/* Default register vals */
	int                        pci_irqmask;
	u32                        dma_ctl_val;
	u32                        ts_int_msk_val;
	u32                        gen_ctrl_val;
	u32                        ts_clk_en_val;
	u32                        src_sel_val;
	u32                        vld_misc_val;
	u32                        hw_sop_ctrl_val;

	/* Allow a single tsport to have multiple frontends */
	u32                        num_frontends;
	void                (*gate_ctrl)(struct cx23885_tsport *port, int open);
	void                       *port_priv;

	/* Workaround for a temp dvb_frontend that the tuner can attached to */
	struct dvb_frontend analog_fe;

	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	struct i2c_client *i2c_client_sec;
	struct i2c_client *i2c_client_ci;

	int (*set_frontend)(struct dvb_frontend *fe);
	int (*fe_set_voltage)(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage);
};

struct cx23885_kernel_ir {
	struct cx23885_dev	*cx;
	char			*name;
	char			*phys;

	struct rc_dev		*rc;
};

struct cx23885_audio_buffer {
	unsigned int		bpl;
	struct cx23885_riscmem	risc;
	void			*vaddr;
	struct scatterlist	*sglist;
	int			sglen;
	unsigned long		nr_pages;
};

struct cx23885_audio_dev {
	struct cx23885_dev	*dev;

	struct pci_dev		*pci;

	struct snd_card		*card;

	spinlock_t		lock;

	atomic_t		count;

	unsigned int		dma_size;
	unsigned int		period_size;
	unsigned int		num_periods;

	struct cx23885_audio_buffer   *buf;

	struct snd_pcm_substream *substream;
};

struct cx23885_dev {
	atomic_t                   refcount;
	struct v4l2_device	   v4l2_dev;
	struct v4l2_ctrl_handler   ctrl_handler;

	/* pci stuff */
	struct pci_dev             *pci;
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23888-ir.h"

#include <linux/kfifo.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/rc-core.h>

static unsigned int ir_888_debug;
module_param(ir_888_debug, int, 0644);
MODULE_PARM_DESC(ir_888_debug, "enable debug messages [CX23888 IR controller]");

#define CX23888_IR_REG_BASE	0x170000
/*
 * These CX23888 register offsets have a straightforward one to one mapping
 * to the CX23885 register offsets of 0x200 through 0x218
 */
#define CX23888_IR_CNTRL_REG	0x170000
#define CNTRL_WIN_3_3	0x00000000
#define CNTRL_WIN_4_3	0x00000001
#define CNTRL_WIN_3_4	0x00000002
#define CNTRL_WIN_4_4	0x00000003
#define CNTRL_WIN	0x00000003
#define CNTRL_EDG_NONE	0x00000000
#define CNTRL_EDG_FALL	0x00000004
#define CNTRL_EDG_RISE	0x00000008
#define CNTRL_EDG_BOTH	0x0000000C
#define CNTRL_EDG	0x0000000C
#define CNTRL_DMD	0x00000010
#define CNTRL_MOD	0x00000020
#define CNTRL_RFE	0x00000040
#define CNTRL_TFE	0x00000080
#define CNTRL_RXE	0x00000100
#define CNTRL_TXE	0x00000200
#define CNTRL_RIC	0x00000400
#define CNTRL_TIC	0x00000800
#define CNTRL_CPL	0x00001000
#define CNTRL_LBM	0x00002000
#define CNTRL_R		0x00004000
/* CX23888 specific control flag */
#define CNTRL_IVO	0x00008000

#define CX23888_IR_TXCLK_REG	0x170004
#define TXCLK_TCD	0x0000FFFF

#define CX23888_IR_RXCLK_REG	0x170008
#define RXCLK_RCD	0x0000FFFF

#define CX23888_IR_CDUTY_REG	0x17000C
#define CDUTY_CDC	0x0000000F

#define CX23888_IR_STATS_REG	0x170010
#define STATS_RTO	0x00000001
#define STATS_ROR	0x00000002
#define STATS_RBY	0x00000004
#define STATS_TBY	0x00000008
#define STATS_RSR	0x00000010
#define STATS_TSR	0x00000020

#define CX23888_IR_IRQEN_REG	0x170014
#define IRQEN_RTE	0x00000001
#define IRQEN_ROE	0x00000002
#define IRQEN_RSE	0x00000010
#define IRQEN_TSE	0x00000020

#define CX23888_IR_FILTR_REG	0x170018
#define FILTR_LPF	0x0000FFFF

/* This register doesn't follow the pattern; it's 0x23C on a CX23885 */
#define CX23888_IR_FIFO_REG	0x170040
#define FIFO_RXTX	0x0000FFFF
#define FIFO_RXTX_LVL	0x00010000
#define FIFO_RXTX_RTO	0x0001FFFF
#define FIFO_RX_NDV	0x00020000
#define FIFO_RX_DEPTH	8
#define FIFO_TX_DEPTH	8

/* CX23888 unique registers */
#define CX23888_IR_SEEDP_REG	0x17001C
#define CX23888_IR_TIMOL_REG	0x170020
#define CX23888_IR_WAKE0_REG	0x170024
#define CX23888_IR_WAKE1_REG	0x170028
#define CX23888_IR_WAKE2_REG	0x17002C
#define CX23888_IR_MASK0_REG	0x170030
#define CX23888_IR_MASK1_REG	0x170034
#define CX23888_IR_MAKS2_REG	0x170038
#define CX23888_IR_DPIPG_REG	0x17003C
#define CX23888_IR_LEARN_REG	0x170044

#define CX23888_VIDCLK_FREQ	108000000 /* 108 MHz, BT.656 */
#define CX23888_IR_REFCLK_FREQ	(CX23888_VIDCLK_FREQ / 2)

/*
 * We use this union internally for convenience, but callers to tx_write
 * and rx_read will be expecting records of type struct ir_raw_event.
 * Always ensure the size of this union is dictated by struct ir_raw_event.
 */
union cx23888_ir_fifo_rec {
	u32 hw_fifo_data;
	struct ir_raw_event ir_core_data;
};

#define CX23888_IR_RX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))
#define CX23888_IR_TX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))

struct cx23888_ir_state {
	struct v4l2_subdev sd;
	struct cx23885_dev *dev;

	struct v4l2_subdev_ir_parameters rx_params;
	struct mutex rx_params_lock;
	atomic_t rxclk_divider;
	atomic_t rx_invert;

	struct kfifo rx_kfifo;
	spinlock_t rx_kfifo_lock;

	struct v4l2_subdev_ir_parameters tx_params;
	struct mutex tx_params_lock;
	atomic_t txclk_divider;
};

static inline struct cx23888_ir_state *to_state(struct v4l2_subdev *sd)
{
	return v4l2_get_subdevdata(sd);
}

/*
 * IR register block read and write functions
 */
static
inline int cx23888_ir_write4(struct cx23885_dev *dev, u32 addr, u32 value)
{
	cx_write(addr, value);
	return 0;
}

static inline u32 cx23888_ir_read4(struct cx23885_dev *dev, u32 addr)
{
	return cx_read(addr);
}

static inline int cx23888_ir_and_or4(struct cx23885_dev *dev, u32 addr,
				     u32 and_mask, u32 or_value)
{
	cx_andor(addr, ~and_mask, or_value);
	return 0;
}

/*
 * Rx and Tx Clock Divider register computations
 *
 * Note the largest clock divider value of 0xffff corresponds to:
 *	(0xffff + 1) * 1000 / 108/2 MHz = 1,213,629.629... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_clock_divider(unsigned int d)
{
	if (d > RXCLK_RCD + 1)
		d = RXCLK_RCD;
	else if (d < 2)
		d = 1;
	else
		d--;
	return (u16) d;
}

static inline u16 carrier_freq_to_clock_divider(unsigned int freq)
{
	return count_to_clock_divider(
			  DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, freq * 16));
}

static inline unsigned int clock_divider_to_carrier_freq(unsigned int divider)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, (divider + 1) * 16);
}

static inline unsigned int clock_divider_to_freq(unsigned int divider,
						 unsigned int rollovers)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ,
				 (divider + 1) * rollovers);
}

/*
 * Low Pass Filter register calculations
 *
 * Note the largest count value of 0xffff corresponds to:
 *	0xffff * 1000 / 108/2 MHz = 1,213,611.11... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_lpf_count(unsigned int d)
{
	if (d > FILTR_LPF)
		d = FILTR_LPF;
	else if (d < 4)
		d = 0;
	return (u16) d;
}

static inline u16 ns_to_lpf_count(unsigned int ns)
{
	return count_to_lpf_count(
		DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ / 1000000 * ns, 1000));
}

static inline unsigned int lpf_count_to_ns(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in ns */
	return DIV_ROUND_CLOSEST(count * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static inline unsigned int lpf_count_to_us(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in us */
	return DIV_ROUND_CLOSEST(count, CX23888_IR_REFCLK_FREQ / 1000000);
}

/*
 * FIFO register pulse width count computations
 */
static u32 clock_divider_to_resolution(u16 divider)
{
	/*
	 * Resolution is the duration of 1 tick of the readable portion of
	 * of the pulse width counter as read from the FIFO.  The two lsb's are
	 * not readable, hence the << 2.  This function returns ns.
	 */
	return DIV_ROUND_CLOSEST((1 << 2)  * ((u32) divider + 1) * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static u64 pulse_width_count_to_ns(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1) * 1000; /* millicycles */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000);     /* / MHz => ns */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return n;
}

static unsigned int pulse_width_count_to_us(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1);    /* cycles      */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000); /* / MHz => us */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return (unsigned int) n;
}

/*
 * Pulse Clocks computations: Combined Pulse Width Count & Rx Clock Counts
 *
 * The total pulse clock count is an 18 bit pulse width timer count as the most
 * significant part and (up to) 16 bit clock divider count as a modulus.
 * When the Rx clock divider ticks down to 0, it increments the 18 bit pulse
 * width timer count's least significant bit.
 */
static u64 ns_to_pulse_clocks(u32 ns)
{
	u64 clocks;
	u32 rem;
	clocks = CX23888_IR_REFCLK_FREQ / 1000000 * (u64) ns; /* millicycles  */
	rem = do_div(clocks, 1000);                         /* /1000 = cycles */
	if (rem >= 1000 / 2)
		clocks++;
	return clocks;
}

static u16 pulse_clocks_to_clock_divider(u64 count)
{
	do_div(count, (FIFO_RXTX << 2) | 0x3);

	/* net result needs to be rounded down and decremented by 1 */
	if (count > RXCLK_RCD + 1)
		count = RXCLK_RCD;
	else if (count < 2)
		count = 1;
	else
		count--;
	return (u16) count;
}

/*
 * IR Control Register helpers
 */
enum tx_fifo_watermark {
	TX_FIFO_HALF_EMPTY = 0,
	TX_FIFO_EMPTY      = CNTRL_TIC,
};

enum rx_fifo_watermark {
	RX_FIFO_HALF_FULL = 0,
	RX_FIFO_NOT_EMPTY = CNTRL_RIC,
};

static inline void control_tx_irq_watermark(struct cx23885_dev *dev,
					    enum tx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_TIC, level);
}

static inline void control_rx_irq_watermark(struct cx23885_dev *dev,
					    enum rx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_RIC, level);
}

static inline void control_tx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_TXE | CNTRL_TFE),
			   enable ? (CNTRL_TXE | CNTRL_TFE) : 0);
}

static inline void control_rx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_RXE | CNTRL_RFE),
			   enable ? (CNTRL_RXE | CNTRL_RFE) : 0);
}

static inline void control_tx_modulation_enable(struct cx23885_dev *dev,
						bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_MOD,
			   enable ? CNTRL_MOD : 0);
}

static inline void control_rx_demodulation_enable(struct cx23885_dev *dev,
						  bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_DMD,
			   enable ? CNTRL_DMD : 0);
}

static inline void control_rx_s_edge_detection(struct cx23885_dev *dev,
					       u32 edge_types)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_EDG_BOTH,
			   edge_types & CNTRL_EDG_BOTH);
}

static void control_rx_s_carrier_window(struct cx23885_dev *dev,
					unsigned int carrier,
					unsigned int *carrier_range_low,
					unsigned int *carrier_range_high)
{
	u32 v;
	unsigned int c16 = carrier * 16;

	if (*carrier_range_low < DIV_ROUND_CLOSEST(c16, 16 + 3)) {
		v = CNTRL_WIN_3_4;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 4);
	} else {
		v = CNTRL_WIN_3_3;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 3);
	}

	if (*carrier_range_high > DIV_ROUND_CLOSEST(c16, 16 - 3)) {
		v |= CNTRL_WIN_4_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 4);
	} else {
		v |= CNTRL_WIN_3_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 3);
	}
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_WIN, v);
}

static inline void control_tx_polarity_invert(struct cx23885_dev *dev,
					      bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_CPL,
			   invert ? CNTRL_CPL : 0);
}

static inline void control_tx_level_invert(struct cx23885_dev *dev,
					  bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_IVO,
			   invert ? CNTRL_IVO : 0);
}

/*
 * IR Rx & Tx Clock Register helpers
 */
static unsigned int txclk_tx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static unsigned int rxclk_rx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static u32 txclk_tx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

static u32 rxclk_rx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

/*
 * IR Tx Carrier Duty Cycle register helpers
 */
static unsigned int cduty_tx_s_duty_cycle(struct cx23885_dev *dev,
					  unsigned int duty_cycle)
{
	u32 n;
	n = DIV_ROUND_CLOSEST(duty_cycle * 100, 625); /* 16ths of 100% */
	if (n != 0)
		n--;
	if (n > 15)
		n = 15;
	cx23888_ir_write4(dev, CX23888_IR_CDUTY_REG, n);
	return DIV_ROUND_CLOSEST((n + 1) * 100, 16);
}

/*
 * IR Filter Register helpers
 */
static u32 filter_rx_s_min_width(struct cx23885_dev *dev, u32 min_width_ns)
{
	u32 count = ns_to_lpf_count(min_width_ns);
	cx23888_ir_write4(dev, CX23888_IR_FILTR_REG, count);
	return lpf_count_to_ns(count);
}

/*
 * IR IRQ Enable Register helpers
 */
static inline void irqenable_rx(struct cx23885_dev *dev, u32 mask)
{
	mask &= (IRQEN_RTE | IRQEN_ROE | IRQEN_RSE);
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG,
			   ~(IRQEN_RTE | IRQEN_ROE | IRQEN_RSE), mask);
}

static inline void irqenable_tx(struct cx23885_dev *dev, u32 mask)
{
	mask &= IRQEN_TSE;
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG, ~IRQEN_TSE, mask);
}

/*
 * V4L2 Subdevice IR Ops
 */
static int cx23888_ir_irq_handler(struct v4l2_subdev *sd, u32 status,
				  bool *handled)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	unsigned long flags;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);

	union cx23888_ir_fifo_rec rx_data[FIFO_RX_DEPTH];
	unsigned int i, j, k;
	u32 events, v;
	int tsr, rsr, rto, ror, tse, rse, rte, roe, kror;

	tsr = stats & STATS_TSR; /* Tx FIFO Service Request */
	rsr = stats & STATS_RSR; /* Rx FIFO Service Request */
	rto = stats & STATS_RTO; /* Rx Pulse Width Timer Time Out */
	ror = stats & STATS_ROR; /* Rx FIFO Over Run */

	tse = irqen & IRQEN_TSE; /* Tx FIFO Service Request IRQ Enable */
	rse = irqen & IRQEN_RSE; /* Rx FIFO Service Request IRQ Enable */
	rte = irqen & IRQEN_RTE; /* Rx Pulse Width Timer Time Out IRQ Enable */
	roe = irqen & IRQEN_ROE; /* Rx FIFO Over Run IRQ Enable */

	*handled = false;
	v4l2_dbg(2, ir_888_debug, sd, "IRQ Status:  %s %s %s %s %s %s\n",
		 tsr ? "tsr" : "   ", rsr ? "rsr" : "   ",
		 rto ? "rto" : "   ", ror ? "ror" : "   ",
		 stats & STATS_TBY ? "tby" : "   ",
		 stats & STATS_RBY ? "rby" : "   ");

	v4l2_dbg(2, ir_888_debug, sd, "IRQ Enables: %s %s %s %s\n",
		 tse ? "tse" : "   ", rse ? "rse" : "   ",
		 rte ? "rte" : "   ", roe ? "roe" : "   ");

	/*
	 * Transmitter interrupt service
	 */
	if (tse && tsr) {
		/*
		 * TODO:
		 * Check the watermark threshold setting
		 * Pull FIFO_TX_DEPTH or FIFO_TX_DEPTH/2 entries from tx_kfifo
		 * Push the data to the hardware FIFO.
		 * If there was nothing more to send in the tx_kfifo, disable
		 *	the TSR IRQ and notify the v4l2_device.
		 * If there was something in the tx_kfifo, check the tx_kfifo
		 *      level and notify the v4l2_device, if it is low.
		 */
		/* For now, inhibit TSR interrupt until Tx is implemented */
		irqenable_tx(dev, 0);
		events = V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ;
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_TX_NOTIFY, &events);
		*handled = true;
	}

	/*
	 * Receiver interrupt service
	 */
	kror = 0;
	if ((rse && rsr) || (rte && rto)) {
		/*
		 * Receive data on RSR to clear the STATS_RSR.
		 * Receive data on RTO, since we may not have yet hit the RSR
		 * watermark when we receive the RTO.
		 */
		for (i = 0, v = FIFO_RX_NDV;
		     (v & FIFO_RX_NDV) && !kror; i = 0) {
			for (j = 0;
			     (v & FIFO_RX_NDV) && j < FIFO_RX_DEPTH; j++) {
				v = cx23888_ir_read4(dev, CX23888_IR_FIFO_REG);
				rx_data[i].hw_fifo_data = v & ~FIFO_RX_NDV;
				i++;
			}
			if (i == 0)
				break;
			j = i * sizeof(union cx23888_ir_fifo_rec);
			k = kfifo_in_locked(&state->rx_kfifo,
				      (unsigned char *) rx_data, j,
				      &state->rx_kfifo_lock);
			if (k != j)
				kror++; /* rx_kfifo over run */
		}
		*handled = true;
	}

	events = 0;
	v = 0;
	if (kror) {
		events |= V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver software FIFO overrun\n");
	}
	if (roe && ror) {
		/*
		 * The RX FIFO Enable (CNTRL_RFE) must be toggled to clear
		 * the Rx FIFO Over Run status (STATS_ROR)
		 */
		v |= CNTRL_RFE;
		events |= V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver hardware FIFO overrun\n");
	}
	if (rte && rto) {
		/*
		 * The IR Receiver Enable (CNTRL_RXE) must be toggled to clear
		 * the Rx Pulse Width Timer Time Out (STATS_RTO)
		 */
		v |= CNTRL_RXE;
		events |= V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED;
	}
	if (v) {
		/* Clear STATS_ROR & STATS_RTO as needed by resetting hardware */
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl & ~v);
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl);
		*handled = true;
	}

	spin_lock_irqsave(&state->rx_kfifo_lock, flags);
	if (kfifo_len(&state->rx_kfifo) >= CX23888_IR_RX_KFIFO_SIZE / 2)
		events |= V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ;
	spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);

	if (events)
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_RX_NOTIFY, &events);
	return 0;
}

/* Receiver */
static int cx23888_ir_rx_read(struct v4l2_subdev *sd, u8 *buf, size_t count,
			      ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	bool invert = (bool) atomic_read(&state->rx_invert);
	u16 divider = (u16) atomic_read(&state->rxclk_divider);

	unsigned int i, n;
	union cx23888_ir_fifo_rec *p;
	unsigned u, v, w;

	n = count / sizeof(union cx23888_ir_fifo_rec)
		* sizeof(union cx23888_ir_fifo_rec);
	if (n == 0) {
		*num = 0;
		return 0;
	}

	n = kfifo_out_locked(&state->rx_kfifo, buf, n, &state->rx_kfifo_lock);

	n /= sizeof(union cx23888_ir_fifo_rec);
	*num = n * sizeof(union cx23888_ir_fifo_rec);

	for (p = (union cx23888_ir_fifo_rec *) buf, i = 0; i < n; p++, i++) {

		if ((p->hw_fifo_data & FIFO_RXTX_RTO) == FIFO_RXTX_RTO) {
			/* Assume RTO was because of no IR light input */
			u = 0;
			w = 1;
		} else {
			u = (p->hw_fifo_data & FIFO_RXTX_LVL) ? 1 : 0;
			if (invert)
				u = u ? 0 : 1;
			w = 0;
		}

		v = (unsigned) pulse_width_count_to_ns(
				  (u16)(p->hw_fifo_data & FIFO_RXTX), divider) / 1000;
		if (v > IR_MAX_DURATION)
			v = IR_MAX_DURATION;

		p->ir_core_data = (struct ir_raw_event)
			{ .pulse = u, .duration = v, .timeout = w };

		v4l2_dbg(2, ir_888_debug, sd, "rx read: %10u ns  %s  %s\n",
			 v, u ? "mark" : "space", w ? "(timed out)" : "");
		if (w)
			v4l2_dbg(2, ir_888_debug, sd, "rx read: end of rx\n");
	}
	return 0;
}

static int cx23888_ir_rx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->rx_params_lock);
	memcpy(p, &state->rx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->rx_params_lock);

	/* Disable or slow down all IR Rx circuits and counters */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);
	control_rx_demodulation_enable(dev, false);
	control_rx_s_edge_detection(dev, CNTRL_EDG_NONE);
	filter_rx_s_min_width(dev, 0);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, RXCLK_RCD);

	state->rx_params.shutdown = true;

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->rx_params;
	u16 rxclk_divider;

	if (p->shutdown)
		return cx23888_ir_rx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->rx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the receiver */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);

	control_rx_demodulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = rxclk_rx_s_carrier(dev, p->carrier_freq,
						     &rxclk_divider);

		o->carrier_freq = p->carrier_freq;

		o->duty_cycle = p->duty_cycle = 50;

		control_rx_s_carrier_window(dev, p->carrier_freq,
					    &p->carrier_range_lower,
					    &p->carrier_range_upper);
		o->carrier_range_lower = p->carrier_range_lower;
		o->carrier_range_upper = p->carrier_range_upper;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, rxclk_divider);
	} else {
		p->max_pulse_width =
			    rxclk_rx_s_max_pulse_width(dev, p->max_pulse_width,
						       &rxclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->rxclk_divider, rxclk_divider);

	p->noise_filter_min_width =
			  filter_rx_s_min_width(dev, p->noise_filter_min_width);
	o->noise_filter_min_width = p->noise_filter_min_width;

	p->resolution = clock_divider_to_resolution(rxclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_rx_irq_watermark(dev, RX_FIFO_HALF_FULL);

	control_rx_s_edge_detection(dev, CNTRL_EDG_BOTH);

	o->invert_level = p->invert_level;
	atomic_set(&state->rx_invert, p->invert_level);

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		unsigned long flags;

		spin_lock_irqsave(&state->rx_kfifo_lock, flags);
		kfifo_reset(&state->rx_kfifo);
		/* reset tx_fifo too if there is one... */
		spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);
		if (p->interrupt_enable)
			irqenable_rx(dev, IRQEN_RSE | IRQEN_RTE | IRQEN_ROE);
		control_rx_enable(dev, p->enable);
	}

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

/* Transmitter */
static int cx23888_ir_tx_write(struct v4l2_subdev *sd, u8 *buf, size_t count,
			       ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	/* For now enable the Tx FIFO Service interrupt & pretend we did work */
	irqenable_tx(dev, IRQEN_TSE);
	*num = count;
	return 0;
}

static int cx23888_ir_tx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->tx_params_lock);
	memcpy(p, &state->tx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->tx_params_lock);

	/* Disable or slow down all IR Tx circuits and counters */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);
	control_tx_modulation_enable(dev, false);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, TXCLK_TCD);

	state->tx_params.shutdown = true;

	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->tx_params;
	u16 txclk_divider;

	if (p->shutdown)
		return cx23888_ir_tx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->tx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the transmitter */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);

	control_tx_modulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = txclk_tx_s_carrier(dev, p->carrier_freq,
						     &txclk_divider);
		o->carrier_freq = p->carrier_freq;

		p->duty_cycle = cduty_tx_s_duty_cycle(dev, p->duty_cycle);
		o->duty_cycle = p->duty_cycle;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, txclk_divider);
	} else {
		p->max_pulse_width =
			    txclk_tx_s_max_pulse_width(dev, p->max_pulse_width,
						       &txclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->txclk_divider, txclk_divider);

	p->resolution = clock_divider_to_resolution(txclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_tx_irq_watermark(dev, TX_FIFO_HALF_EMPTY);

	control_tx_polarity_invert(dev, p->invert_carrier_sense);
	o->invert_carrier_sense = p->invert_carrier_sense;

	control_tx_level_invert(dev, p->invert_level);
	o->invert_level = p->invert_level;

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		if (p->interrupt_enable)
			irqenable_tx(dev, IRQEN_TSE);
		control_tx_enable(dev, p->enable);
	}

	mutex_unlock(&state->tx_params_lock);
	return 0;
}


/*
 * V4L2 Subdevice Core Ops
 */
static int cx23888_ir_log_status(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	char *s;
	int i, j;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 txclk = cx23888_ir_read4(dev, CX23888_IR_TXCLK_REG) & TXCLK_TCD;
	u32 rxclk = cx23888_ir_read4(dev, CX23888_IR_RXCLK_REG) & RXCLK_RCD;
	u32 cduty = cx23888_ir_read4(dev, CX23888_IR_CDUTY_REG) & CDUTY_CDC;
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 filtr = cx23888_ir_read4(dev, CX23888_IR_FILTR_REG) & FILTR_LPF;

	v4l2_info(sd, "IR Receiver:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_RXE ? "yes" : "no");
	v4l2_info(sd, "\tDemodulation from a carrier:       %s\n",
		  cntrl & CNTRL_DMD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_RFE ? "enabled" : "disabled");
	switch (cntrl & CNTRL_EDG) {
	case CNTRL_EDG_NONE:
		s = "disabled";
		break;
	case CNTRL_EDG_FALL:
		s = "falling edge";
		break;
	case CNTRL_EDG_RISE:
		s = "rising edge";
		break;
	case CNTRL_EDG_BOTH:
		s = "rising & falling edges";
		break;
	default:
		s = "??? edge";
		break;
	}
	v4l2_info(sd, "\tPulse timers' start/stop trigger:  %s\n", s);
	v4l2_info(sd, "\tFIFO data on pulse timer overflow: %s\n",
		  cntrl & CNTRL_R ? "not loaded" : "overflow marker");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_RIC ? "not empty" : "half full or greater");
	v4l2_info(sd, "\tLoopback mode:                     %s\n",
		  cntrl & CNTRL_LBM ? "loopback active" : "normal receive");
	if (cntrl & CNTRL_DMD) {
		v4l2_info(sd, "\tExpected carrier (16 clocks):      %u Hz\n",
			  clock_divider_to_carrier_freq(rxclk));
		switch (cntrl & CNTRL_WIN) {
		case CNTRL_WIN_3_3:
			i = 3;
			j = 3;
			break;
		case CNTRL_WIN_4_3:
			i = 4;
			j = 3;
			break;
		case CNTRL_WIN_3_4:
			i = 3;
			j = 4;
			break;
		case CNTRL_WIN_4_4:
			i = 4;
			j = 4;
			break;
		default:
			i = 0;
			j = 0;
			break;
		}
		v4l2_info(sd, "\tNext carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz\n",
			  i, j,
			  clock_divider_to_freq(rxclk, 16 + j),
			  clock_divider_to_freq(rxclk, 16 - i));
	}
	v4l2_info(sd, "\tMax measurable pulse width:        %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, rxclk),
		  pulse_width_count_to_ns(FIFO_RXTX, rxclk));
	v4l2_info(sd, "\tLow pass filter:                   %s\n",
		  filtr ? "enabled" : "disabled");
	if (filtr)
		v4l2_info(sd, "\tMin acceptable pulse width (LPF):  %u us, %u ns\n",
			  lpf_count_to_us(filtr),
			  lpf_count_to_ns(filtr));
	v4l2_info(sd, "\tPulse width timer timed-out:       %s\n",
		  stats & STATS_RTO ? "yes" : "no");
	v4l2_info(sd, "\tPulse width timer time-out intr:   %s\n",
		  irqen & IRQEN_RTE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO overrun:                      %s\n",
		  stats & STATS_ROR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO overrun interrupt:            %s\n",
		  irqen & IRQEN_ROE ? "enabled" : "disabled");
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_RBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_RSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_RSE ? "enabled" : "disabled");

	v4l2_info(sd, "IR Transmitter:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_TXE ? "yes" : "no");
	v4l2_info(sd, "\tModulation onto a carrier:         %s\n",
		  cntrl & CNTRL_MOD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_TFE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_TIC ? "not empty" : "half full or less");
	v4l2_info(sd, "\tOutput pin level inversion         %s\n",
		  cntrl & CNTRL_IVO ? "yes" : "no");
	v4l2_info(sd, "\tCarrier polarity:                  %s\n",
		  cntrl & CNTRL_CPL ? "space:burst mark:noburst"
				    : "space:noburst mark:burst");
	if (cntrl & CNTRL_MOD) {
		v4l2_info(sd, "\tCarrier (16 clocks):               %u Hz\n",
			  clock_divider_to_carrier_freq(txclk));
		v4l2_info(sd, "\tCarrier duty cycle:                %2u/16\n",
			  cduty + 1);
	}
	v4l2_info(sd, "\tMax pulse width:                   %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, txclk),
		  pulse_width_count_to_ns(FIFO_RXTX, txclk));
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_TBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_TSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_TSE ? "enabled" : "disabled");

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int cx23888_ir_g_register(struct v4l2_subdev *sd,
				 struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	reg->size = 4;
	reg->val = cx23888_ir_read4(state->dev, addr);
	return 0;
}

static int cx23888_ir_s_register(struct v4l2_subdev *sd,
				 const struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	cx23888_ir_write4(state->dev, addr, reg->val);
	return 0;
}
#endif

static const struct v4l2_subdev_core_ops cx23888_ir_core_ops = {
	.log_status = cx23888_ir_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = cx23888_ir_g_register,
	.s_register = cx23888_ir_s_register,
#endif
	.interrupt_service_routine = cx23888_ir_irq_handler,
};

static const struct v4l2_subdev_ir_ops cx23888_ir_ir_ops = {
	.rx_read = cx23888_ir_rx_read,
	.rx_g_parameters = cx23888_ir_rx_g_parameters,
	.rx_s_parameters = cx23888_ir_rx_s_parameters,

	.tx_write = cx23888_ir_tx_write,
	.tx_g_parameters = cx23888_ir_tx_g_parameters,
	.tx_s_parameters = cx23888_ir_tx_s_parameters,
};

static const struct v4l2_subdev_ops cx23888_ir_controller_ops = {
	.core = &cx23888_ir_core_ops,
	.ir = &cx23888_ir_ir_ops,
};

static const struct v4l2_subdev_ir_parameters default_rx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5, RC-6, and RC-6A carrier */

	/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
	/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
	.noise_filter_min_width = 333333, /* ns */
	.carrier_range_lower = 35000,
	.carrier_range_upper = 37000,
	.invert_level = false,
};

static const struct v4l2_subdev_ir_parameters default_tx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5 carrier */
	.duty_cycle = 25,      /* 25 %   - RC-5 carrier */
	.invert_level = false,
	.invert_carrier_sense = false,
};

int cx23888_ir_probe(struct cx23885_dev *dev)
{
	struct cx23888_ir_state *state;
	struct v4l2_subdev *sd;
	struct v4l2_subdev_ir_parameters default_params;
	int ret;

	state = kzalloc(sizeof(struct cx23888_ir_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	spin_lock_init(&state->rx_kfifo_lock);
	if (kfifo_alloc(&state->rx_kfifo, CX23888_IR_RX_KFIFO_SIZE,
			GFP_KERNEL)) {
		kfree(state);
		return -ENOMEM;
	}

	state->dev = dev;
	sd = &state->sd;

	v4l2_subdev_init(sd, &cx23888_ir_controller_ops);
	v4l2_set_subdevdata(sd, state);
	/* FIXME - fix the formatting of dev->v4l2_dev.name and use it */
	snprintf(sd->name, sizeof(sd->name), "%s/888-ir", dev->name);
	sd->grp_id = CX23885_HW_888_IR;

	ret = v4l2_device_register_subdev(&dev->v4l2_dev, sd);
	if (ret == 0) {
		/*
		 * Ensure no interrupts arrive from '888 specific conditions,
		 * since we ignore them in this driver to have commonality with
		 * similar IR controller cores.
		 */
		cx23888_ir_write4(dev, CX23888_IR_IRQEN_REG, 0);

		mutex_init(&state->rx_params_lock);
		default_params = default_rx_params;
		v4l2_subdev_call(sd, ir, rx_s_parameters, &default_params);

		mutex_init(&state->tx_params_lock);
		default_params = default_tx_params;
		v4l2_subdev_call(sd, ir, tx_s_parameters, &default_params);
	} else {
		kfifo_free(&state->rx_kfifo);
	}
	return ret;
}

int cx23888_ir_remove(struct cx23885_dev *dev)
{
	struct v4l2_subdev *sd;
	struct cx23888_ir_state *state;

	sd = cx23885_find_hw(dev, CX23885_HW_888_IR);
	if (sd == NULL)
		return -ENODEV;

	cx23888_ir_rx_shutdown(sd);
	cx23888_ir_tx_shutdown(sd);

	state = to_state(sd);
	v4l2_device_unregister_subdev(sd);
	kfifo_free(&state->rx_kfifo);
	kfree(state);
	/* Nothing more to free() as state held the actual v4l2_subdev object */
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23888-ir.h"

#include <linux/kfifo.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/rc-core.h>

static unsigned int ir_888_debug;
module_param(ir_888_debug, int, 0644);
MODULE_PARM_DESC(ir_888_debug, "enable debug messages [CX23888 IR controller]");

#define CX23888_IR_REG_BASE	0x170000
/*
 * These CX23888 register offsets have a straightforward one to one mapping
 * to the CX23885 register offsets of 0x200 through 0x218
 */
#define CX23888_IR_CNTRL_REG	0x170000
#define CNTRL_WIN_3_3	0x00000000
#define CNTRL_WIN_4_3	0x00000001
#define CNTRL_WIN_3_4	0x00000002
#define CNTRL_WIN_4_4	0x00000003
#define CNTRL_WIN	0x00000003
#define CNTRL_EDG_NONE	0x00000000
#define CNTRL_EDG_FALL	0x00000004
#define CNTRL_EDG_RISE	0x00000008
#define CNTRL_EDG_BOTH	0x0000000C
#define CNTRL_EDG	0x0000000C
#define CNTRL_DMD	0x00000010
#define CNTRL_MOD	0x00000020
#define CNTRL_RFE	0x00000040
#define CNTRL_TFE	0x00000080
#define CNTRL_RXE	0x00000100
#define CNTRL_TXE	0x00000200
#define CNTRL_RIC	0x00000400
#define CNTRL_TIC	0x00000800
#define CNTRL_CPL	0x00001000
#define CNTRL_LBM	0x00002000
#define CNTRL_R		0x00004000
/* CX23888 specific control flag */
#define CNTRL_IVO	0x00008000

#define CX23888_IR_TXCLK_REG	0x170004
#define TXCLK_TCD	0x0000FFFF

#define CX23888_IR_RXCLK_REG	0x170008
#define RXCLK_RCD	0x0000FFFF

#define CX23888_IR_CDUTY_REG	0x17000C
#define CDUTY_CDC	0x0000000F

#define CX23888_IR_STATS_REG	0x170010
#define STATS_RTO	0x00000001
#define STATS_ROR	0x00000002
#define STATS_RBY	0x00000004
#define STATS_TBY	0x00000008
#define STATS_RSR	0x00000010
#define STATS_TSR	0x00000020

#define CX23888_IR_IRQEN_REG	0x170014
#define IRQEN_RTE	0x00000001
#define IRQEN_ROE	0x00000002
#define IRQEN_RSE	0x00000010
#define IRQEN_TSE	0x00000020

#define CX23888_IR_FILTR_REG	0x170018
#define FILTR_LPF	0x0000FFFF

/* This register doesn't follow the pattern; it's 0x23C on a CX23885 */
#define CX23888_IR_FIFO_REG	0x170040
#define FIFO_RXTX	0x0000FFFF
#define FIFO_RXTX_LVL	0x00010000
#define FIFO_RXTX_RTO	0x0001FFFF
#define FIFO_RX_NDV	0x00020000
#define FIFO_RX_DEPTH	8
#define FIFO_TX_DEPTH	8

/* CX23888 unique registers */
#define CX23888_IR_SEEDP_REG	0x17001C
#define CX23888_IR_TIMOL_REG	0x170020
#define CX23888_IR_WAKE0_REG	0x170024
#define CX23888_IR_WAKE1_REG	0x170028
#define CX23888_IR_WAKE2_REG	0x17002C
#define CX23888_IR_MASK0_REG	0x170030
#define CX23888_IR_MASK1_REG	0x170034
#define CX23888_IR_MAKS2_REG	0x170038
#define CX23888_IR_DPIPG_REG	0x17003C
#define CX23888_IR_LEARN_REG	0x170044

#define CX23888_VIDCLK_FREQ	108000000 /* 108 MHz, BT.656 */
#define CX23888_IR_REFCLK_FREQ	(CX23888_VIDCLK_FREQ / 2)

/*
 * We use this union internally for convenience, but callers to tx_write
 * and rx_read will be expecting records of type struct ir_raw_event.
 * Always ensure the size of this union is dictated by struct ir_raw_event.
 */
union cx23888_ir_fifo_rec {
	u32 hw_fifo_data;
	struct ir_raw_event ir_core_data;
};

#define CX23888_IR_RX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))
#define CX23888_IR_TX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))

struct cx23888_ir_state {
	struct v4l2_subdev sd;
	struct cx23885_dev *dev;

	struct v4l2_subdev_ir_parameters rx_params;
	struct mutex rx_params_lock;
	atomic_t rxclk_divider;
	atomic_t rx_invert;

	struct kfifo rx_kfifo;
	spinlock_t rx_kfifo_lock;

	struct v4l2_subdev_ir_parameters tx_params;
	struct mutex tx_params_lock;
	atomic_t txclk_divider;
};

static inline struct cx23888_ir_state *to_state(struct v4l2_subdev *sd)
{
	return v4l2_get_subdevdata(sd);
}

/*
 * IR register block read and write functions
 */
static
inline int cx23888_ir_write4(struct cx23885_dev *dev, u32 addr, u32 value)
{
	cx_write(addr, value);
	return 0;
}

static inline u32 cx23888_ir_read4(struct cx23885_dev *dev, u32 addr)
{
	return cx_read(addr);
}

static inline int cx23888_ir_and_or4(struct cx23885_dev *dev, u32 addr,
				     u32 and_mask, u32 or_value)
{
	cx_andor(addr, ~and_mask, or_value);
	return 0;
}

/*
 * Rx and Tx Clock Divider register computations
 *
 * Note the largest clock divider value of 0xffff corresponds to:
 *	(0xffff + 1) * 1000 / 108/2 MHz = 1,213,629.629... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_clock_divider(unsigned int d)
{
	if (d > RXCLK_RCD + 1)
		d = RXCLK_RCD;
	else if (d < 2)
		d = 1;
	else
		d--;
	return (u16) d;
}

static inline u16 carrier_freq_to_clock_divider(unsigned int freq)
{
	return count_to_clock_divider(
			  DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, freq * 16));
}

static inline unsigned int clock_divider_to_carrier_freq(unsigned int divider)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, (divider + 1) * 16);
}

static inline unsigned int clock_divider_to_freq(unsigned int divider,
						 unsigned int rollovers)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ,
				 (divider + 1) * rollovers);
}

/*
 * Low Pass Filter register calculations
 *
 * Note the largest count value of 0xffff corresponds to:
 *	0xffff * 1000 / 108/2 MHz = 1,213,611.11... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_lpf_count(unsigned int d)
{
	if (d > FILTR_LPF)
		d = FILTR_LPF;
	else if (d < 4)
		d = 0;
	return (u16) d;
}

static inline u16 ns_to_lpf_count(unsigned int ns)
{
	return count_to_lpf_count(
		DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ / 1000000 * ns, 1000));
}

static inline unsigned int lpf_count_to_ns(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in ns */
	return DIV_ROUND_CLOSEST(count * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static inline unsigned int lpf_count_to_us(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in us */
	return DIV_ROUND_CLOSEST(count, CX23888_IR_REFCLK_FREQ / 1000000);
}

/*
 * FIFO register pulse width count computations
 */
static u32 clock_divider_to_resolution(u16 divider)
{
	/*
	 * Resolution is the duration of 1 tick of the readable portion of
	 * of the pulse width counter as read from the FIFO.  The two lsb's are
	 * not readable, hence the << 2.  This function returns ns.
	 */
	return DIV_ROUND_CLOSEST((1 << 2)  * ((u32) divider + 1) * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static u64 pulse_width_count_to_ns(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1) * 1000; /* millicycles */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000);     /* / MHz => ns */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return n;
}

static unsigned int pulse_width_count_to_us(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1);    /* cycles      */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000); /* / MHz => us */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return (unsigned int) n;
}

/*
 * Pulse Clocks computations: Combined Pulse Width Count & Rx Clock Counts
 *
 * The total pulse clock count is an 18 bit pulse width timer count as the most
 * significant part and (up to) 16 bit clock divider count as a modulus.
 * When the Rx clock divider ticks down to 0, it increments the 18 bit pulse
 * width timer count's least significant bit.
 */
static u64 ns_to_pulse_clocks(u32 ns)
{
	u64 clocks;
	u32 rem;
	clocks = CX23888_IR_REFCLK_FREQ / 1000000 * (u64) ns; /* millicycles  */
	rem = do_div(clocks, 1000);                         /* /1000 = cycles */
	if (rem >= 1000 / 2)
		clocks++;
	return clocks;
}

static u16 pulse_clocks_to_clock_divider(u64 count)
{
	do_div(count, (FIFO_RXTX << 2) | 0x3);

	/* net result needs to be rounded down and decremented by 1 */
	if (count > RXCLK_RCD + 1)
		count = RXCLK_RCD;
	else if (count < 2)
		count = 1;
	else
		count--;
	return (u16) count;
}

/*
 * IR Control Register helpers
 */
enum tx_fifo_watermark {
	TX_FIFO_HALF_EMPTY = 0,
	TX_FIFO_EMPTY      = CNTRL_TIC,
};

enum rx_fifo_watermark {
	RX_FIFO_HALF_FULL = 0,
	RX_FIFO_NOT_EMPTY = CNTRL_RIC,
};

static inline void control_tx_irq_watermark(struct cx23885_dev *dev,
					    enum tx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_TIC, level);
}

static inline void control_rx_irq_watermark(struct cx23885_dev *dev,
					    enum rx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_RIC, level);
}

static inline void control_tx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_TXE | CNTRL_TFE),
			   enable ? (CNTRL_TXE | CNTRL_TFE) : 0);
}

static inline void control_rx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_RXE | CNTRL_RFE),
			   enable ? (CNTRL_RXE | CNTRL_RFE) : 0);
}

static inline void control_tx_modulation_enable(struct cx23885_dev *dev,
						bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_MOD,
			   enable ? CNTRL_MOD : 0);
}

static inline void control_rx_demodulation_enable(struct cx23885_dev *dev,
						  bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_DMD,
			   enable ? CNTRL_DMD : 0);
}

static inline void control_rx_s_edge_detection(struct cx23885_dev *dev,
					       u32 edge_types)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_EDG_BOTH,
			   edge_types & CNTRL_EDG_BOTH);
}

static void control_rx_s_carrier_window(struct cx23885_dev *dev,
					unsigned int carrier,
					unsigned int *carrier_range_low,
					unsigned int *carrier_range_high)
{
	u32 v;
	unsigned int c16 = carrier * 16;

	if (*carrier_range_low < DIV_ROUND_CLOSEST(c16, 16 + 3)) {
		v = CNTRL_WIN_3_4;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 4);
	} else {
		v = CNTRL_WIN_3_3;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 3);
	}

	if (*carrier_range_high > DIV_ROUND_CLOSEST(c16, 16 - 3)) {
		v |= CNTRL_WIN_4_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 4);
	} else {
		v |= CNTRL_WIN_3_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 3);
	}
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_WIN, v);
}

static inline void control_tx_polarity_invert(struct cx23885_dev *dev,
					      bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_CPL,
			   invert ? CNTRL_CPL : 0);
}

static inline void control_tx_level_invert(struct cx23885_dev *dev,
					  bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_IVO,
			   invert ? CNTRL_IVO : 0);
}

/*
 * IR Rx & Tx Clock Register helpers
 */
static unsigned int txclk_tx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static unsigned int rxclk_rx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static u32 txclk_tx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

static u32 rxclk_rx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

/*
 * IR Tx Carrier Duty Cycle register helpers
 */
static unsigned int cduty_tx_s_duty_cycle(struct cx23885_dev *dev,
					  unsigned int duty_cycle)
{
	u32 n;
	n = DIV_ROUND_CLOSEST(duty_cycle * 100, 625); /* 16ths of 100% */
	if (n != 0)
		n--;
	if (n > 15)
		n = 15;
	cx23888_ir_write4(dev, CX23888_IR_CDUTY_REG, n);
	return DIV_ROUND_CLOSEST((n + 1) * 100, 16);
}

/*
 * IR Filter Register helpers
 */
static u32 filter_rx_s_min_width(struct cx23885_dev *dev, u32 min_width_ns)
{
	u32 count = ns_to_lpf_count(min_width_ns);
	cx23888_ir_write4(dev, CX23888_IR_FILTR_REG, count);
	return lpf_count_to_ns(count);
}

/*
 * IR IRQ Enable Register helpers
 */
static inline void irqenable_rx(struct cx23885_dev *dev, u32 mask)
{
	mask &= (IRQEN_RTE | IRQEN_ROE | IRQEN_RSE);
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG,
			   ~(IRQEN_RTE | IRQEN_ROE | IRQEN_RSE), mask);
}

static inline void irqenable_tx(struct cx23885_dev *dev, u32 mask)
{
	mask &= IRQEN_TSE;
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG, ~IRQEN_TSE, mask);
}

/*
 * V4L2 Subdevice IR Ops
 */
static int cx23888_ir_irq_handler(struct v4l2_subdev *sd, u32 status,
				  bool *handled)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	unsigned long flags;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);

	union cx23888_ir_fifo_rec rx_data[FIFO_RX_DEPTH];
	unsigned int i, j, k;
	u32 events, v;
	int tsr, rsr, rto, ror, tse, rse, rte, roe, kror;

	tsr = stats & STATS_TSR; /* Tx FIFO Service Request */
	rsr = stats & STATS_RSR; /* Rx FIFO Service Request */
	rto = stats & STATS_RTO; /* Rx Pulse Width Timer Time Out */
	ror = stats & STATS_ROR; /* Rx FIFO Over Run */

	tse = irqen & IRQEN_TSE; /* Tx FIFO Service Request IRQ Enable */
	rse = irqen & IRQEN_RSE; /* Rx FIFO Service Request IRQ Enable */
	rte = irqen & IRQEN_RTE; /* Rx Pulse Width Timer Time Out IRQ Enable */
	roe = irqen & IRQEN_ROE; /* Rx FIFO Over Run IRQ Enable */

	*handled = false;
	v4l2_dbg(2, ir_888_debug, sd, "IRQ Status:  %s %s %s %s %s %s\n",
		 tsr ? "tsr" : "   ", rsr ? "rsr" : "   ",
		 rto ? "rto" : "   ", ror ? "ror" : "   ",
		 stats & STATS_TBY ? "tby" : "   ",
		 stats & STATS_RBY ? "rby" : "   ");

	v4l2_dbg(2, ir_888_debug, sd, "IRQ Enables: %s %s %s %s\n",
		 tse ? "tse" : "   ", rse ? "rse" : "   ",
		 rte ? "rte" : "   ", roe ? "roe" : "   ");

	/*
	 * Transmitter interrupt service
	 */
	if (tse && tsr) {
		/*
		 * TODO:
		 * Check the watermark threshold setting
		 * Pull FIFO_TX_DEPTH or FIFO_TX_DEPTH/2 entries from tx_kfifo
		 * Push the data to the hardware FIFO.
		 * If there was nothing more to send in the tx_kfifo, disable
		 *	the TSR IRQ and notify the v4l2_device.
		 * If there was something in the tx_kfifo, check the tx_kfifo
		 *      level and notify the v4l2_device, if it is low.
		 */
		/* For now, inhibit TSR interrupt until Tx is implemented */
		irqenable_tx(dev, 0);
		events = V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ;
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_TX_NOTIFY, &events);
		*handled = true;
	}

	/*
	 * Receiver interrupt service
	 */
	kror = 0;
	if ((rse && rsr) || (rte && rto)) {
		/*
		 * Receive data on RSR to clear the STATS_RSR.
		 * Receive data on RTO, since we may not have yet hit the RSR
		 * watermark when we receive the RTO.
		 */
		for (i = 0, v = FIFO_RX_NDV;
		     (v & FIFO_RX_NDV) && !kror; i = 0) {
			for (j = 0;
			     (v & FIFO_RX_NDV) && j < FIFO_RX_DEPTH; j++) {
				v = cx23888_ir_read4(dev, CX23888_IR_FIFO_REG);
				rx_data[i].hw_fifo_data = v & ~FIFO_RX_NDV;
				i++;
			}
			if (i == 0)
				break;
			j = i * sizeof(union cx23888_ir_fifo_rec);
			k = kfifo_in_locked(&state->rx_kfifo,
				      (unsigned char *) rx_data, j,
				      &state->rx_kfifo_lock);
			if (k != j)
				kror++; /* rx_kfifo over run */
		}
		*handled = true;
	}

	events = 0;
	v = 0;
	if (kror) {
		events |= V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver software FIFO overrun\n");
	}
	if (roe && ror) {
		/*
		 * The RX FIFO Enable (CNTRL_RFE) must be toggled to clear
		 * the Rx FIFO Over Run status (STATS_ROR)
		 */
		v |= CNTRL_RFE;
		events |= V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver hardware FIFO overrun\n");
	}
	if (rte && rto) {
		/*
		 * The IR Receiver Enable (CNTRL_RXE) must be toggled to clear
		 * the Rx Pulse Width Timer Time Out (STATS_RTO)
		 */
		v |= CNTRL_RXE;
		events |= V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED;
	}
	if (v) {
		/* Clear STATS_ROR & STATS_RTO as needed by resetting hardware */
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl & ~v);
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl);
		*handled = true;
	}

	spin_lock_irqsave(&state->rx_kfifo_lock, flags);
	if (kfifo_len(&state->rx_kfifo) >= CX23888_IR_RX_KFIFO_SIZE / 2)
		events |= V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ;
	spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);

	if (events)
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_RX_NOTIFY, &events);
	return 0;
}

/* Receiver */
static int cx23888_ir_rx_read(struct v4l2_subdev *sd, u8 *buf, size_t count,
			      ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	bool invert = (bool) atomic_read(&state->rx_invert);
	u16 divider = (u16) atomic_read(&state->rxclk_divider);

	unsigned int i, n;
	union cx23888_ir_fifo_rec *p;
	unsigned u, v, w;

	n = count / sizeof(union cx23888_ir_fifo_rec)
		* sizeof(union cx23888_ir_fifo_rec);
	if (n == 0) {
		*num = 0;
		return 0;
	}

	n = kfifo_out_locked(&state->rx_kfifo, buf, n, &state->rx_kfifo_lock);

	n /= sizeof(union cx23888_ir_fifo_rec);
	*num = n * sizeof(union cx23888_ir_fifo_rec);

	for (p = (union cx23888_ir_fifo_rec *) buf, i = 0; i < n; p++, i++) {

		if ((p->hw_fifo_data & FIFO_RXTX_RTO) == FIFO_RXTX_RTO) {
			/* Assume RTO was because of no IR light input */
			u = 0;
			w = 1;
		} else {
			u = (p->hw_fifo_data & FIFO_RXTX_LVL) ? 1 : 0;
			if (invert)
				u = u ? 0 : 1;
			w = 0;
		}

		v = (unsigned) pulse_width_count_to_ns(
				  (u16)(p->hw_fifo_data & FIFO_RXTX), divider) / 1000;
		if (v > IR_MAX_DURATION)
			v = IR_MAX_DURATION;

		p->ir_core_data = (struct ir_raw_event)
			{ .pulse = u, .duration = v, .timeout = w };

		v4l2_dbg(2, ir_888_debug, sd, "rx read: %10u ns  %s  %s\n",
			 v, u ? "mark" : "space", w ? "(timed out)" : "");
		if (w)
			v4l2_dbg(2, ir_888_debug, sd, "rx read: end of rx\n");
	}
	return 0;
}

static int cx23888_ir_rx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->rx_params_lock);
	memcpy(p, &state->rx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->rx_params_lock);

	/* Disable or slow down all IR Rx circuits and counters */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);
	control_rx_demodulation_enable(dev, false);
	control_rx_s_edge_detection(dev, CNTRL_EDG_NONE);
	filter_rx_s_min_width(dev, 0);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, RXCLK_RCD);

	state->rx_params.shutdown = true;

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->rx_params;
	u16 rxclk_divider;

	if (p->shutdown)
		return cx23888_ir_rx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->rx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the receiver */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);

	control_rx_demodulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = rxclk_rx_s_carrier(dev, p->carrier_freq,
						     &rxclk_divider);

		o->carrier_freq = p->carrier_freq;

		o->duty_cycle = p->duty_cycle = 50;

		control_rx_s_carrier_window(dev, p->carrier_freq,
					    &p->carrier_range_lower,
					    &p->carrier_range_upper);
		o->carrier_range_lower = p->carrier_range_lower;
		o->carrier_range_upper = p->carrier_range_upper;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, rxclk_divider);
	} else {
		p->max_pulse_width =
			    rxclk_rx_s_max_pulse_width(dev, p->max_pulse_width,
						       &rxclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->rxclk_divider, rxclk_divider);

	p->noise_filter_min_width =
			  filter_rx_s_min_width(dev, p->noise_filter_min_width);
	o->noise_filter_min_width = p->noise_filter_min_width;

	p->resolution = clock_divider_to_resolution(rxclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_rx_irq_watermark(dev, RX_FIFO_HALF_FULL);

	control_rx_s_edge_detection(dev, CNTRL_EDG_BOTH);

	o->invert_level = p->invert_level;
	atomic_set(&state->rx_invert, p->invert_level);

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		unsigned long flags;

		spin_lock_irqsave(&state->rx_kfifo_lock, flags);
		kfifo_reset(&state->rx_kfifo);
		/* reset tx_fifo too if there is one... */
		spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);
		if (p->interrupt_enable)
			irqenable_rx(dev, IRQEN_RSE | IRQEN_RTE | IRQEN_ROE);
		control_rx_enable(dev, p->enable);
	}

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

/* Transmitter */
static int cx23888_ir_tx_write(struct v4l2_subdev *sd, u8 *buf, size_t count,
			       ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	/* For now enable the Tx FIFO Service interrupt & pretend we did work */
	irqenable_tx(dev, IRQEN_TSE);
	*num = count;
	return 0;
}

static int cx23888_ir_tx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->tx_params_lock);
	memcpy(p, &state->tx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->tx_params_lock);

	/* Disable or slow down all IR Tx circuits and counters */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);
	control_tx_modulation_enable(dev, false);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, TXCLK_TCD);

	state->tx_params.shutdown = true;

	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->tx_params;
	u16 txclk_divider;

	if (p->shutdown)
		return cx23888_ir_tx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->tx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the transmitter */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);

	control_tx_modulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = txclk_tx_s_carrier(dev, p->carrier_freq,
						     &txclk_divider);
		o->carrier_freq = p->carrier_freq;

		p->duty_cycle = cduty_tx_s_duty_cycle(dev, p->duty_cycle);
		o->duty_cycle = p->duty_cycle;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, txclk_divider);
	} else {
		p->max_pulse_width =
			    txclk_tx_s_max_pulse_width(dev, p->max_pulse_width,
						       &txclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->txclk_divider, txclk_divider);

	p->resolution = clock_divider_to_resolution(txclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_tx_irq_watermark(dev, TX_FIFO_HALF_EMPTY);

	control_tx_polarity_invert(dev, p->invert_carrier_sense);
	o->invert_carrier_sense = p->invert_carrier_sense;

	control_tx_level_invert(dev, p->invert_level);
	o->invert_level = p->invert_level;

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		if (p->interrupt_enable)
			irqenable_tx(dev, IRQEN_TSE);
		control_tx_enable(dev, p->enable);
	}

	mutex_unlock(&state->tx_params_lock);
	return 0;
}


/*
 * V4L2 Subdevice Core Ops
 */
static int cx23888_ir_log_status(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	char *s;
	int i, j;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 txclk = cx23888_ir_read4(dev, CX23888_IR_TXCLK_REG) & TXCLK_TCD;
	u32 rxclk = cx23888_ir_read4(dev, CX23888_IR_RXCLK_REG) & RXCLK_RCD;
	u32 cduty = cx23888_ir_read4(dev, CX23888_IR_CDUTY_REG) & CDUTY_CDC;
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 filtr = cx23888_ir_read4(dev, CX23888_IR_FILTR_REG) & FILTR_LPF;

	v4l2_info(sd, "IR Receiver:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_RXE ? "yes" : "no");
	v4l2_info(sd, "\tDemodulation from a carrier:       %s\n",
		  cntrl & CNTRL_DMD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_RFE ? "enabled" : "disabled");
	switch (cntrl & CNTRL_EDG) {
	case CNTRL_EDG_NONE:
		s = "disabled";
		break;
	case CNTRL_EDG_FALL:
		s = "falling edge";
		break;
	case CNTRL_EDG_RISE:
		s = "rising edge";
		break;
	case CNTRL_EDG_BOTH:
		s = "rising & falling edges";
		break;
	default:
		s = "??? edge";
		break;
	}
	v4l2_info(sd, "\tPulse timers' start/stop trigger:  %s\n", s);
	v4l2_info(sd, "\tFIFO data on pulse timer overflow: %s\n",
		  cntrl & CNTRL_R ? "not loaded" : "overflow marker");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_RIC ? "not empty" : "half full or greater");
	v4l2_info(sd, "\tLoopback mode:                     %s\n",
		  cntrl & CNTRL_LBM ? "loopback active" : "normal receive");
	if (cntrl & CNTRL_DMD) {
		v4l2_info(sd, "\tExpected carrier (16 clocks):      %u Hz\n",
			  clock_divider_to_carrier_freq(rxclk));
		switch (cntrl & CNTRL_WIN) {
		case CNTRL_WIN_3_3:
			i = 3;
			j = 3;
			break;
		case CNTRL_WIN_4_3:
			i = 4;
			j = 3;
			break;
		case CNTRL_WIN_3_4:
			i = 3;
			j = 4;
			break;
		case CNTRL_WIN_4_4:
			i = 4;
			j = 4;
			break;
		default:
			i = 0;
			j = 0;
			break;
		}
		v4l2_info(sd, "\tNext carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz\n",
			  i, j,
			  clock_divider_to_freq(rxclk, 16 + j),
			  clock_divider_to_freq(rxclk, 16 - i));
	}
	v4l2_info(sd, "\tMax measurable pulse width:        %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, rxclk),
		  pulse_width_count_to_ns(FIFO_RXTX, rxclk));
	v4l2_info(sd, "\tLow pass filter:                   %s\n",
		  filtr ? "enabled" : "disabled");
	if (filtr)
		v4l2_info(sd, "\tMin acceptable pulse width (LPF):  %u us, %u ns\n",
			  lpf_count_to_us(filtr),
			  lpf_count_to_ns(filtr));
	v4l2_info(sd, "\tPulse width timer timed-out:       %s\n",
		  stats & STATS_RTO ? "yes" : "no");
	v4l2_info(sd, "\tPulse width timer time-out intr:   %s\n",
		  irqen & IRQEN_RTE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO overrun:                      %s\n",
		  stats & STATS_ROR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO overrun interrupt:            %s\n",
		  irqen & IRQEN_ROE ? "enabled" : "disabled");
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_RBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_RSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_RSE ? "enabled" : "disabled");

	v4l2_info(sd, "IR Transmitter:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_TXE ? "yes" : "no");
	v4l2_info(sd, "\tModulation onto a carrier:         %s\n",
		  cntrl & CNTRL_MOD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_TFE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_TIC ? "not empty" : "half full or less");
	v4l2_info(sd, "\tOutput pin level inversion         %s\n",
		  cntrl & CNTRL_IVO ? "yes" : "no");
	v4l2_info(sd, "\tCarrier polarity:                  %s\n",
		  cntrl & CNTRL_CPL ? "space:burst mark:noburst"
				    : "space:noburst mark:burst");
	if (cntrl & CNTRL_MOD) {
		v4l2_info(sd, "\tCarrier (16 clocks):               %u Hz\n",
			  clock_divider_to_carrier_freq(txclk));
		v4l2_info(sd, "\tCarrier duty cycle:                %2u/16\n",
			  cduty + 1);
	}
	v4l2_info(sd, "\tMax pulse width:                   %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, txclk),
		  pulse_width_count_to_ns(FIFO_RXTX, txclk));
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_TBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_TSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_TSE ? "enabled" : "disabled");

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int cx23888_ir_g_register(struct v4l2_subdev *sd,
				 struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	reg->size = 4;
	reg->val = cx23888_ir_read4(state->dev, addr);
	return 0;
}

static int cx23888_ir_s_register(struct v4l2_subdev *sd,
				 const struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	cx23888_ir_write4(state->dev, addr, reg->val);
	return 0;
}
#endif

static const struct v4l2_subdev_core_ops cx23888_ir_core_ops = {
	.log_status = cx23888_ir_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = cx23888_ir_g_register,
	.s_register = cx23888_ir_s_register,
#endif
	.interrupt_service_routine = cx23888_ir_irq_handler,
};

static const struct v4l2_subdev_ir_ops cx23888_ir_ir_ops = {
	.rx_read = cx23888_ir_rx_read,
	.rx_g_parameters = cx23888_ir_rx_g_parameters,
	.rx_s_parameters = cx23888_ir_rx_s_parameters,

	.tx_write = cx23888_ir_tx_write,
	.tx_g_parameters = cx23888_ir_tx_g_parameters,
	.tx_s_parameters = cx23888_ir_tx_s_parameters,
};

static const struct v4l2_subdev_ops cx23888_ir_controller_ops = {
	.core = &cx23888_ir_core_ops,
	.ir = &cx23888_ir_ir_ops,
};

static const struct v4l2_subdev_ir_parameters default_rx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5, RC-6, and RC-6A carrier */

	/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
	/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
	.noise_filter_min_width = 333333, /* ns */
	.carrier_range_lower = 35000,
	.carrier_range_upper = 37000,
	.invert_level = false,
};

static const struct v4l2_subdev_ir_parameters default_tx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5 carrier */
	.duty_cycle = 25,      /* 25 %   - RC-5 carrier */
	.invert_level = false,
	.invert_carrier_sense = false,
};

int cx23888_ir_probe(struct cx23885_dev *dev)
{
	struct cx23888_ir_state *state;
	struct v4l2_subdev *sd;
	struct v4l2_subdev_ir_parameters default_params;
	int ret;

	state = kzalloc(sizeof(struct cx23888_ir_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	spin_lock_init(&state->rx_kfifo_lock);
	if (kfifo_alloc(&state->rx_kfifo, CX23888_IR_RX_KFIFO_SIZE,
			GFP_KERNEL)) {
		kfree(state);
		return -ENOMEM;
	}

	state->dev = dev;
	sd = &state->sd;

	v4l2_subdev_init(sd, &cx23888_ir_controller_ops);
	v4l2_set_subdevdata(sd, state);
	/* FIXME - fix the formatting of dev->v4l2_dev.name and use it */
	snprintf(sd->name, sizeof(sd->name), "%s/888-ir", dev->name);
	sd->grp_id = CX23885_HW_888_IR;

	ret = v4l2_device_register_subdev(&dev->v4l2_dev, sd);
	if (ret == 0) {
		/*
		 * Ensure no interrupts arrive from '888 specific conditions,
		 * since we ignore them in this driver to have commonality with
		 * similar IR controller cores.
		 */
		cx23888_ir_write4(dev, CX23888_IR_IRQEN_REG, 0);

		mutex_init(&state->rx_params_lock);
		default_params = default_rx_params;
		v4l2_subdev_call(sd, ir, rx_s_parameters, &default_params);

		mutex_init(&state->tx_params_lock);
		default_params = default_tx_params;
		v4l2_subdev_call(sd, ir, tx_s_parameters, &default_params);
	} else {
		kfifo_free(&state->rx_kfifo);
	}
	return ret;
}

int cx23888_ir_remove(struct cx23885_dev *dev)
{
	struct v4l2_subdev *sd;
	struct cx23888_ir_state *state;

	sd = cx23885_find_hw(dev, CX23885_HW_888_IR);
	if (sd == NULL)
		return -ENODEV;

	cx23888_ir_rx_shutdown(sd);
	cx23888_ir_tx_shutdown(sd);

	state = to_state(sd);
	v4l2_device_unregister_subdev(sd);
	kfifo_free(&state->rx_kfifo);
	kfree(state);
	/* Nothing more to free() as state held the actual v4l2_subdev object */
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23888_IR_H_
#define _CX23888_IR_H_
int cx23888_ir_probe(struct cx23885_dev *dev);
int cx23888_ir_remove(struct cx23885_dev *dev);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23888_IR_H_
#define _CX23888_IR_H_
int cx23888_ir_probe(struct cx23885_dev *dev);
int cx23888_ir_remove(struct cx23885_dev *dev);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later

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
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &buf[0],
			.len	= 1
		}, {
			.addr	= EEPROM_I2C_ADDR,
			.flags	= I2C_M_RD,
			.buf	= &buf[1],
			.len	= 1
		}

	};

	buf[0] = addr;
	buf[1] = 0x0;

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		pr_err("eeprom i2c read error, status=%d\n", ret);
		return -1;
	}

	return buf[1];
};

int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data)
{
	int ret;
	unsigned char bufw[2];

	/* Write into EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &bufw[0],
			.len	= 2
		}
	};

	bufw[0] = addr;
	bufw[1] = data;

	ret = i2c_transfer(i2c_adap, msg, 1);

	if (ret != 1) {
		pr_err("eeprom i2c write error, status=%d\n", ret);
		return -1;
	}

	mdelay(10); /* prophylactic delay, datasheet write cycle time = 5 ms */
	return 0;
};

void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo)
{
	int i, j;

	cinfo->rev =  netup_eeprom_read(i2c_adap, 63);

	for (i = 64, j = 0; i < 70; i++, j++)
		cinfo->port[0].mac[j] =  netup_eeprom_read(i2c_adap, i);

	for (i = 70, j = 0; i < 76; i++, j++)
		cinfo->port[1].mac[j] =  netup_eeprom_read(i2c_adap, i);
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             // SPDX-License-Identifier: GPL-2.0-or-later

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
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &buf[0],
			.len	= 1
		}, {
			.addr	= EEPROM_I2C_ADDR,
			.flags	= I2C_M_RD,
			.buf	= &buf[1],
			.len	= 1
		}

	};

	buf[0] = addr;
	buf[1] = 0x0;

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		pr_err("eeprom i2c read error, status=%d\n", ret);
		return -1;
	}

	return buf[1];
};

int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data)
{
	int ret;
	unsigned char bufw[2];

	/* Write into EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &bufw[0],
			.len	= 2
		}
	};

	bufw[0] = addr;
	bufw[1] = data;

	ret = i2c_transfer(i2c_adap, msg, 1);

	if (ret != 1) {
		pr_err("eeprom i2c write error, status=%d\n", ret);
		return -1;
	}

	mdelay(10); /* prophylactic delay, datasheet write cycle time = 5 ms */
	return 0;
};

void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo)
{
	int i, j;

	cinfo->rev =  netup_eeprom_read(i2c_adap, 63);

	for (i = 64, j = 0; i < 70; i++, j++)
		cinfo->port[0].mac[j] =  netup_eeprom_read(i2c_adap, i);

	for (i = 70, j = 0; i < 76; i++, j++)
		cinfo->port[1].mac[j] =  netup_eeprom_read(i2c_adap, i);
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0-or-later */
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
	struct netup_port_info port[2];/* ports - 1,2 */
	u8 rev;/* card revision */
};

extern int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr);
extern int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data);
extern void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0-or-later */
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
	struct netup_port_info port[2];/* ports - 1,2 */
	u8 rev;/* card revision */
};

extern int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr);
extern int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data);
extern void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             // SPDX-License-Identifier: GPL-2.0-or-later
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
		pr_err("%s: i2c write error!\n", __func__);
}

static void i2c_av_write4(struct i2c_adapter *i2c, u16 reg, u32 val)
{
	int ret;
	u8 buf[6];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 6
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val & 0xff;
	buf[3] = (val >> 8) & 0xff;
	buf[4] = (val >> 16) & 0xff;
	buf[5] = val >> 24;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static u8 i2c_av_read(struct i2c_adapter *i2c, u16 reg)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 2
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);

	msg.flags = I2C_M_RD;
	msg.len = 1;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c read error!\n", __func__);

	return buf[0];
}

static void i2c_av_and_or(struct i2c_adapter *i2c, u16 reg, unsigned and_mask,
								u8 or_value)
{
	i2c_av_write(i2c, reg, (i2c_av_read(i2c, reg) & and_mask) | or_value);
}
/* set 27MHz on AUX_CLK */
void netup_initialize(struct cx23885_dev *dev)
{
	struct cx23885_i2c *i2c_bus = &dev->i2c_bus[2];
	struct i2c_adapter *i2c = &i2c_bus->i2c_adap;

	/* Stop microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x00);

	/* Aux PLL frac for 27 MHz */
	i2c_av_write4(i2c, 0x114, 0xea0eb3);

	/* Aux PLL int for 27 MHz */
	i2c_av_write4(i2c, 0x110, 0x090319);

	/* start microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x10);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // SPDX-License-Identifier: GPL-2.0-or-later
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
		pr_err("%s: i2c write error!\n", __func__);
}

static void i2c_av_write4(struct i2c_adapter *i2c, u16 reg, u32 val)
{
	int ret;
	u8 buf[6];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 6
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val & 0xff;
	buf[3] = (val >> 8) & 0xff;
	buf[4] = (val >> 16) & 0xff;
	buf[5] = val >> 24;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static u8 i2c_av_read(struct i2c_adapter *i2c, u16 reg)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 2
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);

	msg.flags = I2C_M_RD;
	msg.len = 1;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c read error!\n", __func__);

	return buf[0];
}

static void i2c_av_and_or(struct i2c_adapter *i2c, u16 reg, unsigned and_mask,
								u8 or_value)
{
	i2c_av_write(i2c, reg, (i2c_av_read(i2c, reg) & and_mask) | or_value);
}
/* set 27MHz on AUX_CLK */
void netup_initialize(struct cx23885_dev *dev)
{
	struct cx23885_i2c *i2c_bus = &dev->i2c_bus[2];
	struct i2c_adapter *i2c = &i2c_bus->i2c_adap;

	/* Stop microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x00);

	/* Aux PLL frac for 27 MHz */
	i2c_av_write4(i2c, 0x114, 0xea0eb3);

	/* Aux PLL int for 27 MHz */
	i2c_av_write4(i2c, 0x110, 0x090319);

	/* start microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x10);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      ?2     4     (            GNU  ?       ?          ?????WV??????S???   ?|$??  ????????   ????  	???  ???      ? 1???_??????	????   ??   ??????????E????   ?? ??   ??  ????????   ??   ??[^_?f??????S?????  ??Hd?    ?D$D????????D$,=9? ?L  =7? ?   =?? ?6  =?? ?   =Q? ?   =O? ?   =?? ?
  =?? ?   =Wu ?   ??  =9K ?   ??  =?? ?   ??  =?? ?   ??  =?N ?   ??  =L ?   ??  =o> ?   ??  =e> ?   ?w  =?8 ?   ?f  =77 ?   ?U  =?6 ?   ?D  =?6 ?   ?3  =?6 ?   ?"  =?4 ?   ?  =?4 ?   ?   ='3 ?   =?2 ??   =?2 ??   =?0 ?   ??   =?, ?   ??   =?, ?   ??   =?, ?   ??   =?, ?   ??   =9+ ?   w}=? ?   wp=+ ?   = w^= ?W   = ?   wF= ?   w9=? ?   w,=? ?   w=a ?   w=V  ?????=?U  wPSh    ????????   ?o   ??&    ?v ??????Q?UWVS????   ?8??????   ???  ??w??ws[??^_]?????   ??,??   w???*u??????   ???   ??  ?
!????   ??    ?X?A ???????u????   ??  ??  	??*[??^_]??   ????`2 u???u@???p?????????   ???v ??????u????P?????&    1?[??^_]???&    ?h??t???(????C????P?????????[^_]???????2?????&    ?????S?????  ???	  ww??	?#  ?   ???'  ?Q  ??t?  ????  ?   ??? `  ?????? `  ????????d   ???????? @  ??????d   [???????&    ??)?  ??'?  ??!??  ?G  ??&?r  ??'??  ???   (  ????????     ???   ??   ??*  ?'  ????????     ???  ??   ??*  ?'  ????????     ?????????   ??*  ?'  ????????     ???  ??   ??*  ?'  ????????     ?????????   ??*  ?'  ????????     ???  ??   ??*  ?'  ????????     ???  ??   ??*  ?'  ????????     ???????????     ???  ????     ???????????   ??  ??    ??<   [???????&    f???vP????   ?   ?   ????????   ????????d   ??????   ????????d   [???????$?[  ?   ???`  ????????`  ??????   ????????   ????????   [??????v ???/  v]??uT???     ???   ??   ????????     ??????   ????????   ??  ??    ?[?f???t???u????   ??  ??    ?[??v ???  ?  ???h?????u????   ?6   ??(  ?   ???   ??$  ???      ??????   ????????      ???????   ????????   ??   ??   ?????   [???????&    ????  ??   ??t	?????????     ???   ??   ????????     ??????   ????????   ??  ??    ?[???
????????     ???   ??   ????????     ??????   ????????   ??  ??    ?[???    ???a??????n??????A????   ??? ?  ?????? ?  ????????d   ?????? ?  ????????d   ??????   ???   ??????   ????????   ??????   ????????   ????????   ??????   [??????t& ??1??  ??   ??5w\??3wc???   ?7   ??(  ?   ???  ??????  ????????d   ????????  [?????f???%??????>???f???8???0????   ?   ????????   ????????d   ??????   ?=?????-?c?????   ??1????????     ???  ????     ?????????d   ????????     ???  ????     ?????????7   ???   ??(  ?   ???   ??$  ???   ? ?  ??   1????   ??  [???*???????,?|???[???    ???     ???   ????     ?????????d   ????????     ?????7   ???   ??(  ? P  ???   ??$  ?   ???   ?X?????&    ??   ???   ??????   ????????d   ????????   [??????t& ???     ???   ????     ??????   ??????b???f??   ?   ????????   ????????d   ??????   ?I?????&    ???     ???   ????     ?????????d   ????????     ???  ?-?????&    ?t& ??????UWVS????0???  d?    ?D$,1????w  vE????w?   ??? ?ud???  ??t71??D$,d+    ??  ??0??[^_]??t& ?????   1???v???uh    1?j?????XZ????&    f???u??5   ??t??   ??????????  ????  ???  ???w????Pd????j????Z???_????    ?   ????  ?r????  ?   ?????1??2???f??   ??? ????   ??????????  ????  ???  ????????Pd?????????Z????????    ?    ???x  ?r???m  ?   ?????1???????v ?????????????????   ??????????  ???  ???}????Pd????p????Z???e????    ?   ????  ?z????  ?   ??????:?????&    ?v ????????????????   ??????????  ?????  ??tA?Pd???t8?z??t1?    ?    ???@  ?j???5  ?   ????????  ?v ????????Ad?@????   ?x????   ?   ??????   ?h????   ??????????  ?D$ ?D$
 ?D$???_????Ad?@??tw?x??tp?   ??????   ?h????   ??????????  ?D$
???????Ad?@???????X???????   ????tN?x??tG?????????????t& ??D$
????&    f??D$ ?D$
 ?D$?[?????????????  ?2????????????????   ????????  ???????????????  ?N????   ??????h????   1???????W????   ??????F?????????>??????????&    f?????????  ??3wpS????v???   ??? ?u?u3[????t
??t&??u????   ????????      [???&    f????   ???????????????      [?????&    ??&    ?????????  ??3w??v???   ???? ?u??u,??v ??t
??t??u????  ??t??   ??????t& ????  ??t??   ??????t& ?????WVS??????`  d?    ?D$1????9  ???  ????	t??  ????  ?H????1  ????&??  ??2??  ??  ??5?f  ??  ??;??  ??  ?H????\  ???     ???     ???     ??     ??      ??$     ??"??  ?J??   ????X???  j ??
  ?9   ?CjD????????  ZY????  ???   ???   ???  ???      ???  ????  ?Pd?????  ?R????  ?    ???|  ?I???q  ????????  ?o  ???t3??   ?H????X  ??     ??      ??$     ?v ???     ???     ???     ??"??  ????????B???????????"  ???  ?D$d+    ?3  ??[^_?f??H????%????P   ???  ?   ?    f???  ???????`  ???
  ???  ?????????  ???    ???   ?   ???     ???     ???      ???  ?G ??? ??     ???  ??      ??$     ??????t& ???????v ???  ??6??   j ???   ?C?A   jA????????  _X????????D$d+    ?  ????[^_??????H????J?????  f???3??   ???     ???     ???     ??     ??      ??$     ?B????M??????}???>?????6???????7?U???j ???   ?s?A   j@????????????A   ??j jA????????  ???????t& ??4??  ???     ???     ???     ??     ??      ??$     ?K?????    ??)wK??'?  ???     ???     ???     ??    ??      ??$     ?N??????-?????v,??2?;  ???     ???     ???     ???????,??????
  ?v ????   ????  ????????f????	  ????   ???     ???     ???     ??     ??      ??$     ???????&    ?P   ?    ??l  ?   f??n  ????????  ??????   ?H?????????   ??????? ??H  ??   ?m?????&??????H??   ?????????????????????&??  ??     ????      ??$     ??????t& ???r???v?H????????????&    ?v???????t????   ???????<??   ??`  u]?????????`  ???u????=?   ?tK??   ??????????  ????????t& ???m????5   ???O????1????t& ??`  ?????????   ????????|??????`  ??t?????????     ???     ???     ??????t& ????     ???     ???     ??     ??      ??$     ???????&    ???     ???   ???     ???       ??     ??      ??$     ???  ??   ?'  ??  ????????     ??????   ?'  ??  1???????v ???     ??????   ?'  ??  ????????   ??  ?:??????????    ??????????????!?	?????   ?'  ??  ?????????Hu????     ???????  ?O?????&    f????     ???     ???     ??     ??      ??$     ?5?????&    ???     ???     ???     ??????`   ?{????D??????\$?D$H   ?D$    ????????   ?    ???????F  ???6  ?1  ????????????P(?   ???  ???  ???  ?B?T$???1????&    ??&    ??????UWVS????? ???  ?p(???  ?????  ?B???  ?   p???  ???  ???B???  ?@    ??????????  ???  ????x  9?tS???  ?M    ???  ???  ??x  ??|  ?E ?E???  ?=x  ?H??  ????[^_]??????t& ???  ???  ??x  ??|  ?E ?=x  ??  ????[^_]?????f??????UWVS?h(???   @  ????????  ???  ????????????  9?t@?v ???  ?P?-x  ?Q?
?   ??x     ??|  "  ????????  9?u?[????^_]???????    ?????VS???????(  ?F??9?t$?X???Cd? ??t???t????????CL?X?9?u?1?[^???&    ?t& ?????WV??S???????(  ???  ??4t??8??wP?F   ???  ???F??X?9?t(?t& ?Cd?@??t?H??t	??????????CL?X?9?u?1?[^_???    ???  u??????????&    ??????WV??S???????(  ???  ??4t??8??wH???uK?G???X?9?t)?t& ??Cd?@??t?H??t	??????????CL?X?9?u?1?[^_???    ???  u?????????????WV??S???????(  ???  ??4t??8??wX???u[?F?   ??  ?????????X?9?t(?t& ?Cd?@??t?H??t	??????????CL?X?9?u?1?[^_???    ???  u?????????????S????????;??(  uH?C????w=?C    ?C    ?C?  ??x  % ?  ?????`?  ?C1?[???&    f??????[???&    f??????V??S?\$???????(  ??x  %?? ??u(????????6?????????;?C1?[^???&    ??????????&    f??????VS?????????(  ?x  ???  ??x  ??|  ?1??S[^???&    f??????VS?????????(  ???  ?C???  ?K???  ?s???  ?6?s???  ?B?C   ?????C1??K[^??t& ????????u??I  ?A,1????&    ?v ????????&    ?v ???????? ?@(???????&    ??&    ??????U1?WVS????? ?p(????????k?????  ?@???  ?????  ??????   ???  ????  ???  ????   wT???+  ????  ???   ???  ???  j P??j j??????????=x  ?6  1???[^_]???    ???  ???J  ???  ???   ???  ??Q??j P??j Q????????????&    ?v ???  9ST?  ?SP???  ???@????x  ??x   ?  ti??    ??  ????   ???p  ?$    ?????   ???  ?T$???  ??R??QQP?t$??D$????????	?????    ???   ???  ???  j Pj?j ?????????????????    ???  ???   ???  ??Q??j PQ????&    ?v ???????[^_]??v ????  ?$??1??I??????????????????VS????????5?I  ??(  9s??   ??x  ?S% ?  ?????`?  ??t^??wy????   ?   j ?s??  j P?C?S?0   j P??j??????C??I  ?C   ?????C?C?C1?[^?????   ??9Kw??   ??????&    f??J????   C????????[^???&    ????????j????t& ??????U??WV??S????pd?    ?D$l????????   ??(  ?|$?D$   ?$1??=x  ????  ????????????D$?????U  ?<$??   ???]  ??`  ???O  ???  ???A  ??I  9E??I  E??=x  ???  ?E???  ?U???  ?M???  ??  ?E?]0?D$$   ?<$?u?M?U?D$(?D$?E(???t$ f?D$0f?D$?E,?L$,f?D$2f?D$??f?\$4??T$?\$??L;|$tY?t& ?Cd?@??t?p??t?L$1?????????CL?X?9?u??D$(?T$?t$ ?L$,?D$?D$0f?D$?D$2f?D$?D$4?U?T$?E0?$?U?T$?u?U(?T$?M?U,???  ?E?D$ld+    u?D$??p[^_]??D$?????????????&    ?t& ??????UWVS?????????  ?   ??(  ????????    i??    ??    ????????????  ?k???? t???? ??  ????????????   ???   ??u???   P?C0h?  P??????CT ????  ????4t??8??w?CT ?1?[^_]??t& ????  u?[1?^_]??????W??V??S??@?P?V????????????????????????=x  ?  ????????C?B????   ?   ?C"  [^_???????&    f??????WVS???x  ???+  ???wr?s?{1??C    ??????F,    )???0?????<??   ?    ?????????????Q  ?? t???? ?G  ??????????C$   1?[^_???&    ??????????&    f?????????T????t& ?????S?????????(  k?l  i??    ??,   ?   ????v??p  ??x  ???[  ??[???????&    ?t& ?????U??WV??S?????x  ???x  ?????????t]????   ?C?????  ???  ??      ??0  i??    U??    ???  h?  j ?T$P??????T$????(  ????[^_]???&    ??&    ??????V?x  S?X(???  ????  ???   @  ???????d  ?N?v??????????   ?   ??0  ??       ???   ????????      ???????      ??? ????   ??@  ??@  ???X1?[^??t& ??????S?????????(  ??l  ??x  ????  1?[??v ??????????9?t	??????t& ???&    ?????UWV??S???   ??d?$?|$d?    ?D$`1??D$   ???=x  ?D$   ???6  ??|  ?$3?x  1?	???   ??   ???  ??`  ????   ???  ????   ?$?~??|  ?????  ?  ??x  % ?  ???     ?????`?  ???  ?F?X?9???   ?t$?v ?Cd?@??t?p??t?$??????????CL?X?9?u??t$???  ???  ?F???  ?L$?t$?X??T$9?t-?t& ??Cd?@??t?p??t?L$1?????????CL?X?9?u?1??T$`d+    u8??d[^_]???????????????????PWh?  h?  h0  ??????????????????t& ?????WVS?t$?|$???????(  ?x  ????  ??????[^_??????&    ?v ?????S
  ??d?    ?\$1??T$?   f?T$?T$?L$?   ?T$???$L   ??????T$d+    u??[?????????????? 
  d?    ?L$1??T$?T$?   ?T$?   f?T$?T$f?L$?   ?T$???D$ ?$L   ?D$L  ?????????  ?D$?T$d+    u?? ????????&    ?v ?????WVS????d?    ?D$1??x  ????  ????   ????   ?   ??????????? ?|$?T$?   ??
  ?D$f?L$???   ?D$?D$L   ?|$??????   ?   f?D$?   ?T$f?D$???D$L   ?|$??????x  ???$  ?D$d+    u??[^_??t& ?   ???????????[??????????&    ??&    ??????U?-x  WV??S?????o  ???  ??l  ?B???????   ??!??   ???  ????   ?Hd?y???&  ?????  k??   i?  ??0   ???^  ?)???T  j 1??????_???  ????   ?? ??   ??vl?B???w
?I?#??rk[^_]???    k?i?  ??,   ?W????   ??????????  ???  ?????C?????&    ?? t}??tx??w???v?? ??s?f????  ??tV?Hd?I??tL?y??tEk??   i?  ??4   ??tc?i??t\j 1?????????  Z?f??? t???? ???????k??   i?  ??4   ??t???????   ??[^_]?;????v j 1?????????  X????    j 1??????Y???????????S?????x  ????  k?i??    ??4   t?????   [???????????   [??????????V??S???????(  k?l  i??    ??,   ????v,???w=?x  ????  ??p  ?????h???1?[^??v ?>??????    [D?^??t& ??????????????UWV??S?????x  ????  ?>????   k?i??    ??,   ????   ?F$   i??    ??,   ?    ?,?@  ?F?$?????????????  ?? t???? ?  ?$????????F0?r k??F4    i??    ??,   ?   ????w?F$   ?   ?V(9?l  t1???[^_]??t& ?C???x?9?t???8?Gd?@??t?H$??t	??????????GL?x?9?u???????????v ?????VS?????????(  ?x  ???#  ????[^??????t& ?????S?????????(  ??l  ??x  ????  1?[??v ?????VS????????x  ??(  ???\  ??w9k?i??    ??,   ??t!?????h????????O???1?[^???&    ?v ?????????&    f??????????&    f??????UWVS????$d?    ?T$ 1????????(  ???  ??:w??#??   ?P??????   ??8???6  ????<  ?C?		? ?n???  ?F\??????????A  ??????D$????  ?F?p?9??8  f??Fd?@??t?H??t	??????????FL?p?9?u??d   ??????D$?$    ????  ?G?@?????1?????????G?@??????]  ?v ?P?? q ??r??4?4????#????t& ??K??x  ?D$   ??|  ?D$   ???  ?D$?F\?T$?		? ?L$??????D$?$   ??t??????$???k  ?=x  ????  ??,  ?   ?????????   ?@?D$???  ?P??????   ??$??w? q ????   ?D$????  ???  ????  ?F?n?X?9?t???&    ?Cd?@??t?x??t??x  ??|  ????????CL?X?9?u??D$???  ?D$?T$??????d   ??????$????   ?$    ?D$ d+    ?  ?$??$[^_]??v ???  ??????v ?$????????&    ???  ?D$?#?????&    f??F?D$   ?p?9???????d   ??????$    ????&    ?|$?G?@??????   ????????G?@??????j????t& ??G?@??????   ????????G?@??????F?p?9??P????d   ????????????|$?G?@?????1?????????G?@????????????????v ??????f?????    ?????WV??S???      ? ????   ???   ????$  ??  t+??   ?  ????   ??t?=x  ??  ?t& 1???u?????????[^?_???&    ?v ???  ??????????   ??   ???  ???  9?t??????????   ???????f?[1?^_???&    ?v ?=x  ?L  1???t?????&    ?v ??????x  S?????h  ???   ????????  ??t???  ??uY????????      ???  ??t???  ??u%????????      ??0  ??t,??[??????t& ?????????&    f??????????&    f?[???&    ??&    ?????UWVS????@?x  d?    ?D$<1????@  ?@  ??F  ??   ?  ???   ???   ???????I  ?   ?=?I  YUYV??x     E???|      ???  ???  ???  ???  ??X  ??X  ??\  ?????     ???  ?  ???  ?  ????????  t???  ????  ???  ??  ?????????|  ??x  ???????1????????1??????????????????   ??     ??     ?   ??,  ??  ????<     ??@     ??4  ?  ??      ??$      ??8      ??  ??????????  ???   ???  ??d  ??d     ?   ??x  ??l  ????h     ???     ???     ???  ?  ???      ???      ???      ???????????   ?K  ??F  ???=??????  ???  ???  ???    ???  ??4t??8???  ???  ???      ???  ???  ???  ?? J  ???   ?21?j???????XY????  ?\  ??&    ???$??????$?D$<d+    ??  ??@??[^_]????  ?s???^  ?   ??????(  i??  ?  P??j ???   ?????_]????????????  ?D$    ?D$    ?D$???D$    ?D$   ?????f?D$?Fd?D$    ?@??t*?H$??t#?   ?T$????  ?x$????  ??????????  ????   ??*??   ??'?&??????  ?D$(    ?D$,    ?D$?D$ ?D$0    ?D$4    ?D$8    ?D$ <  ?D$$@   ?D$?Fd?@????????H(????????   ?T$???!  ?x(???  ??????????????&    ?v i??  j ?(  R???   ???????????XZ???????&    f????  ????????????    ???  ?D$(    ?D$,    ?T$?T$ ?D$0    ?D$4    ?D$8    ?D$ .  ?D$$@   ?T$?Vd?R????????J(????????   ?T$??t0?x(??t)??????????  ???????&    ????????f????t& ??????????  ?h?????    ?????????????????f?f?f?f?f?f?f????????@(?T$??x  ?   % ?  ???%?C   ?  ?1????&    ??&    ??????UWVS????x  ??? ???  ?p(???  ????X  ???  ?B???  ?   p???  ???  ???B???  ?@    ??X  9?tt???  ?L$?   ??\  ?$????????L$????\  ??\  ??x  ??|  ?(????????$???  ?=|  ?@?P??  ??[^_]???&    f????$??????$????\  ??\  ??x  ??|  ?(????????=|  ??  ??[^_]???&    ?t& ?????UWVS?h(???   @  ????????  ??X  ???????????X  9?t@?v ??X  ?P?-x  ?Q?
?   ??x     ??|  "  ???????X  9?u?[????^_]???????    ???????? ?@(???????&    ??&    ??????W1?VS??? ?p(????????S????x  % ?  ?????????u	[?????^_?i?@  ?ST9?r?G??SP???  ???   Pi??  j h?  P??j ?	?????1???[^_???&    ??&    ?????V?|  S?X(??X  ???  ??d  ?v??  ?P,??????????   ?   ???  ???   ??4  ??`      ???   ????????      ???"????      ??? ????   ??@  ??@  ??"?X1?[^???    ?????S?????????(  ?C????C?  ?CGREY?C    ?C$    ??x  ???t'?C
   ?C  ?C   ?C    1?[???&    ??? t??C   1??C?  ?C   ?C    [?????????tFVS???|  ???4  ???  ??????????   ??$  ??X  ???????????????   [^?1??f?f?f?f?f?f???????V???   S??????????????   ??  ?????????[^??????UWV??S?????|$4 ?T$ t?    p???@?    ?@?    ?????t?? ? ?????T$,????   ?D$0?$    ???D$??    ?K?l$$??t-?????&    f?)?????????  ?H9?v??l$$??)???$??ti?|$ tb1??t$0???W?%     ;L$$rRD$$   ?s?w?G    ???t$(?$?$?9D$,?k???????[^_]???&    f??   ?W;L$$s?	??T$   ?s???w?G    +k??????T$???@9?v5???????v ??   ???K??P?S??C?    +h??????H9?r????????z   ??C?B    ?B?D?????    ?H?l$$?????????&    ?v ?????WVS???   ????\  ??X  ??P  ??T  ??	?t
???? )??R????P?    ?????????ZY??t|???  i?  ??   ti?  ??   ????   ????   i?  ??   ????   ??t7??
  ???????|  ????????   ????????   [^_?????[^_??t& ???????????&    ??????????  ?o?????    ???  ????????  i?  ??   ???a?????????????  ?O?????&    ???   ?????i??    ??   ?<??????????t7S?X`?? v@t??v@t[??9??  u??[?????9??  u??[????????&    ??&    ????????4  t*S???   ???  ???   ???  ??t??u
[???&    ????   ???  ???   ???  ???   ??  ????   ?? ????   ??  ????   ?? ?RQh?
  ???   ?   P???????[???&    ?v ?????UW?   V??S???$?M??&    ???  ????  ??C???B??   ???   ?C"  ??????V?$)???~O??~K?9?tE?????????????????????????F?P?V???????F??;$u??=?  v???  ?t& ??[^_]???&    ??????WV1?S?????   ??   ???   ??   ???   ??@  ???   ??@ ???   ??@ ???   ??@  ???   ??@ ???   ??   ???   ????????????   ??  ??1??????????   ??   ???   ??0  ???   ??@  ???   ??P  ???   ??`  [^_???&    ??&    ?????WV??S???   ?_????????????????????????L  ??????????????????????GX??????????????[^_??????t& ?????W???   V??S?????????	??   ?????   ??  ??  	??[??^_???????&    ??&    ??????W???   V????S?????????#??   t???   ??  ??  	??[??^_???????&    ??    ?????W???   V????S????????????   ??  ??  ??!??[??^_??????v ?????W???   V????S??????????   ????????  !??   ??  !??[??^_???????&    ?t& ?????UW??VS?????r??  ???  ???8	  ?G??1?????D$?G?L$?$???   9?F??T$???>  ???   ?  p?P?   ???   ?P1????   ?P1??D$    ?t$?L$9$ro?|$1?f??=?  ?]	  ???????   ?$?D$?x?$??8?F?????   ??F?????   ??F?????   ?????l$9L$w??|$?G?W(?????   ???  ?T$(??G1????????   ??G?L$???????   ??G?t$???????????   ?0?G?W???????   ??G(???G??   ???  ??????   ??   1??t& ??G??????   ?????Pu??G?W?????   ??G?L$?????   ??G$?????   ?0?T$?G ???????   ????=?  ??	  ??1?[^_]??   ???????t& ???	  ?G1??????   ??G?????   ??G$?????   ??G ?????   ???1?[^_]??t& ???   ?????   ???????    ?????UWV??S????  ?8??  ??t`?=?  ?k  ???   ??????????   ?????   ????   ???????????????   ?????   ??   [^_]???&    ???u?[1?^_]??t& ??????U?hW??V????S??????F?V9?t/?X????&    ?v ?CL?X?9?t9??   u??????????[^_]???1????????[^_]??????UWV??1?S?l$???|$$?????1??|$????   ??T$T$ j ??h?  ???T???K?TR????????ZY?C??tn???tj????j W?t$,?t$,j ????????|$?t'1??????R??j W?t$,?t$,h   ?L$0?q??????C+C1???;w[??^_]???&    ??????????&    ?????U?   WV??S???|$?T$?l$j ?Kh?  ?????T?R????????ZY?C??t:1?????1?R??UWj ?t$$j??????1??C+C????;w[??^_]??t& ?????????&    ?????UWV??1?S?l$???|$$?????1??|$????   ??T$T$ j ??h?  ???T???K?TR????????ZY?C??tn???tj????j W?t$,?t$,j ?/??????|$?t'1??????R??j W?t$,?t$,h   ?L$0???????C+C1???;w[??^_]???&    ??????????&    ?????S???  ???  ???   j ???  ???  ?   ?????????[???&    f??????U??W??V??S????  ????q  ??????????   ?????   ????   ??!??kV,???   ???d  ???  ??????=?  Z??  ???   ???   ?????   ?i??        ?P??u?@???  ????  ??? ????????   ??t?????  ??   ????   ??  ?????   ????   ??   ?????   ????   ??  ?????   ????   ???   ?????   ???? ????????   ?   ?????   ??E    ???  i?  ??   u"???   L  ? ??????   ??L  ???  i?  ??   u"???   L  ? ??????   ??L  ???  i?  ??   uE???   L  ? ??????   ??
??L  ???   H  ???  ???E ???   ??,  ??h  =u  t-w  ???,  ??  ????  ??????????   ?????   ???   ????   ?????   ???   ????   ???{????????   ?????	??   ?????????????????????   ??ut??????????>??????      ??? ????%???i??    ??   tb?=?  wi????????????     ? ??????????????1???[^_]??v ???   ??  ?)	???u????1?????????e???f??   ???????????  ??&    ?t& ?????UWV??S???????   ?*???   1????????  ?????!  ?C??uC???????u(???   j ???  ???   ???   ?
???????1?????[^_]???&    ?v ;{Tw?{P???????????&    ??&    ??????UWV??S?????8???  ???  ?????  ?B???  ?   p???  ???  ???B???  ?@    ??????????   ???   ????x  9?tT???  ?M    ???   ???   ??x  ??|  ?E ?E???  ?H??  ???l  ????[^_]??????t& ????   ???   ??x  ??|  ?E ??  ???E  ????[^_]???????????UW??VS????  ????  ?0???   ?????   ????   ??!?????   ?????   ????   ??!???   ?v ?X?A ???????u??d   ???&    ?v ??t,?X?A ???????t???   ???  ???   ?  ? ??u?i??    ??   t}???   ???   ?$??????D$???v ???   ??F???????B????   ?   ?F"  ???????  ????  ???   9?u??T$?$??[^_]???????&    ???   L  ? ??????   ????L  ???   1??????   ????   ?   ?????   ?i??    ??   ?$???1?????????????v ?????????   UWV??S?????  ?????   ?8?=?  wd?? 3 ??  ??   t.?=?  ?G  ???  ?????????t  ?????????????????  ?????   ?0?   [^_]??t& 1?????  ?????   ? WPRh?  hD  ????????q???f??????UWVS????<d?    ?D$8???   ??  ???   ????????????   ??  ?????????!??$?%  ???   $  ? ?D$???      ? ?D$???   T  ? ?D$???   P  ? ?D$???   ??4  ???   0  ? ?D$???   D  ? ?D$???   @  ? ?D$???   ??   ???      ? ?D$ ???  ?????   ? ?D$$???  ?????   ? ?=?  ?D$(?x  ??%   ?D$?????   ??   t?=?  ?K  ??   t?=?  ?o  ??   t?=?  ?  ??   t?=?  ?'  ??   t?=?  ??  ??   ??  ??   t?=?  ?>  ??   ??  ?=?  ?  ??   ?  ?=?  ??  ??  ? u??   t?=?  ??  ??   ??  ?=?  ??  i??    1???  ???2  ???a  ??t"i??    ??   ????  ???S  ?D$??t"i??    ??   ????  ???9  ?L$???-  ?T$????  ?D$??t[???  ?D$7 ??tL?Pd???tC?J$?L$??t8?    ???x  ?J$?L$???i  ?T$7?????l$??????|$7 t???$   ??   ??u-?$    ?D$8d+    ?9  ?$??<[^_]???&    ?v ???   ?<$??  ?$   ????&    ???  ???????????L$??????L$??i??    ??  ??????t& ??  ? ????????L$??????L$??}?????    ?T$????????T$?????????U?t& ?????????????     ????????????????????  ?    ?    ?????????????????t& ??L$?T$?????????D$????????U?????    ?=?  ??????_  ??    ?=?  ??  ??   ?.?????   ?/????=?  ?"?????  ??&    f?????????????????T$???????????????    ??  ? ?????=?  ?	?????  ??&    ?v ?D$???????=?  ??????v  f????  ?????????)????t& ?T$??   ???????9???f????L$7?l$?????????????????&    ?????V????S????t???   ??  ?
	?????? t>i??    ??   ?_  ??   ?_  ???   ??   ?
??????	????  ? ?w  [^???&    ?????V????S????t???   ??  ?
??!?????? t@i??    ??   ??  ??   ??  ???   ??   ?
????????!????  ? ??  [^??v ?????V??S????uh???? u??%  ? ??  [^???&    ?i??    ??   ??  ??   ??  ???   ??   ??[^??!???%?? ???    ???   ??  ??!?[^?????????W??V??S????t ???   ??  ????%   ??ts?
	?????? tFi??    ??   ??  ??   ??  ???   ??????$  ??t??$  ??!??[^_??v ??$  	??[^_???&    ?v ?
??!??????    ?????U?8  ??  W??VS???4   ?????????  ???`N  ????   ?   ??tM1??"  ?Q  1????????u%??????   ??	  ?? 	  1????????t???????   ??&    ??4  ?k???	?????4  ???   ?D$?????????x/j ?C\1??   ?D$????????   X??t,?$???????????????????????[^_]??t& 1????t& ?$???   ????????  ?????C@???  ???  ???  ???  ???  ???  ???  ???  ???  ?????      ???  ???????      ???  ???????      ?C< E  ??????D$???7??????  ??  ??  ???       ???      ????????  ??  ??  ?????????  ???  ?L$???  ?PPh?  Q??  ????????   ???@"f=????  f=R?t??u  ? ????L  ???   ??  ??h  ??d  ???'  ???  ??@N  ;    ?(  ???  ?????    ?t$?f????  ?????uR9?t1???   ??    f9P$u???   f9P&u???   ???  ?????      ??????????   ???  f?x"R???  i?  ??(   ??t???   ??+??  ?P???   ?Rt???   ?@???????   ??????????      ????????????  ???       ???   i?  ??|  ??l    ??h    ??p     ??t    ??x    ??d     ????     ???	    ???	    ???	     ???	    ?? 
    ???	     ???
     ??
  ??|    ??x    ???     ???    ???    ??t     ??   ???????  i?  ??   ???????  ???   ??\  ??X  ?T$??T  ?D$??P  ?T$?T$?D$?D$??	?t???? +D$T$?D$?T$j ?    ?t$?t$?t$?T$$?L$(???????????  ???   ??T  ??P  ??\  ?L$??X  ??)??X  ?    ?T$D?????????  ??  ???   ???   ???  9?@N  ??  E????   QPi?  ??    ?B&P?B$P?t$$hP  ???????  ?????[  ?D  ??&    ???  ???????????   ?6???f?x&7q?U??????   @x}?F?????9??  ??8???????  <   ??@  ?????????   ??????  ???	  ???  ??   ??  ??  ???  ???  ??4  ??     ??    ??(      ??,  ?G ???      ??$     ???????,  ???      ??,  ??0  ??0  ??u
??0     ???    ???  0 ???  @ ???  P ???  T ???  X ???  \ ???  ` ???  d ???  h ??   l ??      ??  @  ??  D  ??(     ??     ??????  ???Q  ??t  ???  ??  ??  ??t  ??x  ???  ???     ???    ???      ???  ?G ???      ???     ????????  ??l      ???  ???  ???  ??u
???     ???    ???  ???  4 ???  @ ???  P ???  T ???  X ???  \ ???  ` ???  d ???  h ???  l ???  D ???  0  ???  4  ???     ???     ????????  =   ??A  ???\????;?????????$????x  ????? K  ?]???f?f?f?f?f?f?f???????	 ???t& ??????V??S?@   ??? ???????t)?F???x  ?????   ? ?u??   [^???&    1?[^??v ?????U??WVS???x?D$??  ???t	???"  ?Ef????   ??  ????!  ??1??????1f????  ?????   ??B?0??  ???o"  ?B??9?vm??p  ???$????  ??  9???|  F??
?????   ?????t  ?????   ??D$??????$???v?????  ????   ???????[^_]??t& ??|  ?M ???????   ???t  ?????   ??p  ????D$?}?????t??D$?@???x  ?????   ? ?t?5?  ????!  ??1?[^_]???????u?????"  ?t& ?????U??W??VS???p?D$??  ?L$??M???b  ???#  ??|  ?E ???   ??????f????  ?]?	??D$??p  f???-    ?$?D$????  ?????   ???t  ?$?T$?????   ??D$?}??????  ??  ???T$?a#  ?E????   ???   ?????v??&    f???  ??|  ?$?T$?????   ????  ?????   ???t  ?????   ??D$????????   ??  ?$????#  ?B??9?~v?J????p  ?99??{???????    ?|$ D??g????????????9#  ?v ??  ??  ?|$ E??$??????v ??  ????"  ???????[^_]??t& ????t  ?????   ??p  ????D$?+?????t??D$?@???x  ?????   ? ?t??  ????"  ??1?[^_]??????????&    ?t& ?????U??W??V??S????  ????#  1???<??   ?t& ??C?$9?}?D???@t? f9tJ1????Y?????x1??9?~^?[??  ???T ????#  ?Bt?1??????????y???[^_]??t& ???I????????x????   ???y????$???t& ????[^_]???    ?????UWVS????H?3d?    ?D$D1???  ????$  ???k1???   ??????x  ?}   ?C    ?C   ??  cx23??  885 ?|$?????  ?    ??|  cx23???  885 ???  inte???  rnal???   ?   ?Ch??  ?$?????????????   ??0??$  ?$????????F?[???   ?????????l  ???  ????$  ?=?  ????$  ??  ???-$  ?t$1??   ?!	  ?????	   ???????????	  ??h    ???????l  Z?T$Dd+    u&??H[^_]??t& ??0?X???????0?L????%  ??????????????????1???????S??????  d?    ?D$1????%  ???D  ?   ?   f?D$????f?T$?T$?D$?D$?D$??
  ?D$D  ?&????D$d+    u??[??????f?f?f?f?f?f?f????????@(???   ?  ???       ?   ???   ???   ???L$?1??    ????????$  ?@ ? ???      ??t ??t?   ?1??f??   ?1????    ?   ?1????    ?????S1???4  ?????   Q1??[?????Z[???&    ?v ?????1????&    ??????S???  ??t???????????%  1???[???&    ?t& ?????WVS??<  ??X  ???  ??<      ??t?????????<  ?????U%  1???[^_???&    f??????S???  ??t??????????k%  1???[???&    ?t& ?????WVS??X  ??<  ???  ??X      ??t?????????X  ??????%  1???[^_???&    f??????S?????   ?H??~2?????????t?@??t??  ??t??[???????    [???    ?   ???????????&    ?v ???????? ?@(???????&    ??&    ???????@(??????v ??????@(???   ?????????   ?????1????&    ??&    ???????? ?@(? ???????&    ??    ???????R(???????????U?   WV??S??$  ??? 0  ?h ?} ?????????t!??t|??tH??@  ????[^_]??????t& ????    ????????   ???????@  ????[^_]??????t& ????    ????????   ???????@  ????[^_]??????t& ????    ????????   ???????@  ????[^_]??????t& ??????U?   WV??S??$  ???   ?h ?} ?????????t!??t\??t8??@  ?????????1?[^_]?f????   ??????   ???????????    ???   ??????   ???????????    ???   ??????   ???????????    ?????VS????$  ?p ????  ??w9??w9??u??@  ?    ??t????????<  ??t(??[^???????&    ??!u??   ??????&    ?v 1?[^??v ?????????????UW??VS???0?$?\$???  ??????????   ?   ??$  ???   ??   ???????????      ??????????   ???? ?  ??   ???      ??????<$ ???   ??   ??  ??$  ???      ????????   ??   ?
?<$?% ???-@  !???=    ?    ??9?y&??   ?t& ?2   ?   ?d   ??????    9?x???   ??   ??u??????   ??   ??   ?????D$????????1??<$ u?D$? ???????[^_]???&    ???????   ?(???1????v ?????UWVS?????5    ???   ?$?|$??   ? ???   ????   ??$  ?)???   ?)?????????E????   ???????????E???   ??t& ?    9?x??  ????????      ? ??u????   ??   ??   ???????E???[^??_]???&    f???$  ???m ?? ????)?$?^????t& ??????V?p?   S???????????t?@??t?????????8  ??t?FT?@??????????????4  ??t?FT?@??????????????0  ??t?FT?@??????????????,  ??t?FT?@????????????????  ??t!??u
?S?????????      1?[^??v ???????1???      [^?f??????S?tY  ?@(???  ?xY  ???  ?   ?tY  ?xY  ?L$??|Y  [?1????&    ?t& ?????????u?A,MPEG1???v ??????f????????? ?@(?  ???????&    ?v ???????? ?@(???????&    ??&    ????????R(???  ???????&    ?t& ?????UWVS??(d?    ?T$$1????????(  ???  ?nPhI  j"?|$W??????F??9?t'?X??t& ?Cd? ??t???t????????CL?X?9?u????  ????????D$$d+    u
??(1?[^_]????????&    ?????WV??S???????(  ???  tL?F   ???  ???F??X?9?t*??    ?Cd?@??t?H??t	??????????CL?X?9?u?1?[^_??????????&    ??&    ??????WV??S???????(  ???  t<?G???X?9?t)?t& ??Cd?@??t?H??t	??????????CL?X?9?u?1?[^_??????????&    ??&    ??????S?????????(  ??|  ??x  ?S?1?[???&    ??????S?????????(  ?CMPEG?C    ???  ???  ?C    ?C   ?C??  ????k  1?[??????S?????????(  ?CMPEG?C    ???  ???  ?C    ?S???  ?S???  ?C   ?S??  ????k  1?[???&    ??    ????????????    ?????WV??S???????(  ???  th???u[?F?   ?N  ?????????X?9?t*??    ?Cd?@??t?H??t	??????????CL?X?9?u???  ????k  1?[^_????????????????v ????????????    ????????????    ?????VS?????????(  ??  ???l  ????[^??????t& ?????UWVS?l$1??|$??????   ??(  1????&    f??????  ???  !?!?	?u????u??????[^_]??t& ??????????????u????????  ???  ??  ?J??   ?J??$  ?J?R??(  ??,  [^_]??????UWVS????????   ??(  ???  ??????????1l  ????t?????'l  ?????k????????  ?    i??    ??    ?????????????1l  ?? t???? ?'l  ????????????   ???   ??u???   P?C0hr  P?????? ?? ??CT ??????  D??CT[1?^_]???&    ?t& ??????S?????????(  ?CMPEG?C    ???  ???  ?C    ?C   ?C??  ???;l  1?[???????=?  S???Zl  ???   ?7   ??(  ?   ???   ??$  ? ?  ???   ??   [???    ?????WV?   S?????????   ??$  ?????   ???? X  ??   ???   ?????   ?????   ???? Y  ??   ???   ?????   ?????   ???????? Z  ??   ???   ?????   ?????   ???? [  ??   ???   ?????   ?????   ????\????   f?? ????   ????   f?????   ????]????   f ????   ????   ?^  ???   ??   ??  ???   ??   ?5    ?????    9?x'??  ????????      ? ??t?[1?^_??t& ?[?????^_???&    ?????UWVS?????????   ?$?   ??$  ?????   ????\????   f?? ????   ????   f?????   ????]????   f ????   ????   ? ^  ???   ??   ? ?  ???   ??   ?5    ??? ??&    f??    9??[  ??  ????????      ? ??t?1????   ??  ??$  ? X  ???   ??   ? ?  ???   ??   ???   ??   ? ?  ???   ??   ? Y  ???   ??   ? ?  ???   ??   ???      ? ?D$???   ??   ? Z  ???   ??   ? ?  ???   ??   ???   ??   ???   ??   ???   ??   ? [  ?.???   ??   ? ?  ?.???   ??   ????	??t$????	????   ??   ??%  ? 	????$?????[^_]???&    ????????????&    ??&    ?????WV?   S???????   ??$  ?????   ???? P  ??   ???   ?????   ?????   ???? Q  ??   ???   ?????   ?????   ???????? R  ??   ???   ?????   ?????   ???? S  ??   ???   ?????   ?????   ?????????@T  ??   ???   @?  ??   ?????   ???? U  ??   ???   ?????   ?????   ????V??   ???   ?????   ?5    ????t& ??    9?x'??  ????????      ? ??t?[1?^_??t& ?[?????^_???&    ?????UWVS?????????   ?$?   ??$  ?????   ??????? T  ??   ???   ?????   ?????   ???? U  ??   ???   ?????   ?????   ????V??   ???   ?????   ?5    ?????&    ?    9??c  ??  ????????      ? ??t?1????   ??  ??$  ? S  ???   ??   ? ?  ???   ??   ???   ??   ? ?  ???   ??   ? R  ???   ??   ? ?  ???   ??   ???      ? ?D$???   ??   ? Q  ???   ??   ? ?  ???   ??   ???   ??   ?D$???   ??   ???   ??   ? P  ?.???   ??   ? ?  ?.???   ??   ??????	??t$????  ? 	????   ??   ?D$?????$	??????[^_]???    ????????????&    ??&    ?????U??WV??S????d?    ?D$1??=?  ?D$    ?|$(?!m  ???  ???P?????????$=xV4??l  ???  ?L$????????D$????l  ???  ?   ???D$   ????????  ??1??P??????????  ??  ?P?????????~5?t& ????  ???T????????=?  ??l  ??9?u????v ???  1??T???????????u????  ?   ???D$   ??????5    ??
??    9???l  ???  ????????  ?L$????????D$t??T$$?l$$1???~-?v ???  ???T????????=?  ??l  ????9?u????  ?L$?P????????=?  ?vl  ???  1?????????D$?T$d+    u??[^_]????????&    ??????UWVS??Hd?    ?D$D1??t$\?|$d?=?  ?gm  ?\$l??t{?4$?D$p?l$?????v ?p????????r?9?u??4$U?????t$l?T$h?????ZY?t$h??t??L$?|$h?
u	?S?L$?
?T$Dd+    u??H[^_]???&    ?v ?l$????????t& ???????F?????    ?????UWVS????,??  ?T$d?    ?D$(1?????m  j j h?   S??????????;  ?=?  ??n  ?D$    ?D$    ?L$? ?  ?????????  ???|$?????????????X?  ?????????????T?  ??????????  ???  	?????????   ??  	????????1??d?  	????????	???n  ???   ??  ???   ?D$?????????n  ?D$???? ?n  ?@?8?  ??t8h/  ??????D$?????XhD  hh0  ?????XZ?D$??????  ??    ?xf?U?u??=?  ?.  ?$1?1????	??&    ?????$??)??L$???????????m  ?D$?M? ??9?w???  ?$???]o  ??x%?v ?L$????????????Do  ?D$?t??s????+o  ?D$???????  ???o  ??????T?  ????????L$? ?  ??????????L$??  	?????????X?  	??L$????????L$?X?  	???????????? @  ? ?  	????????? @  ??  	????????? ?  	??L$???????????  ???????????o  ?=?  ?D$xV4?D$xV4?D$ 4xV?D$$V4x?so  1?1???v 1?????? ??o  ????????????D$9D?u?????u???  ??????m  ???  j j h?   S??????????Q  Wjj h?   S???????????m  ?=?  ????m  ??   ??????5?  ???rn  ??   ?   ??% ?  ?????`?  ???  ???  Pj jh?   S?f??????  ??f???  ???  f???  1???   ?? ??f???  ???  ?????jjj jh?   S????jjj jh?   S?????<   ??0?????h?   h?   j jh?   S?????j j j j j j j j j j j j j jh?   S???????Xj@jh??` hU?U?jjh ? j jh?   S???????,??&    f??F?1?j ??j ??j SVj jh?   W?^?????????$   ?j j j SPj jh?   W?:?????$??u?j j jh?   W?!????<   ?????j j h?   W?????<   ??$???????   ?H  ????????L$??u"?D$(d+    u??D$??,[^_]???&    f?j j j jh?   W??????
   ????????D$    ?????????o  ?(n  ??&    ?????U?   WVS???h(????t  ?/???????ty???  ????????$??t  9?tD??&    ??t  ?P?-x  ?Q?
?   ??x     ??|  "  ???????t  9?u??$???????????[^_]???&    ??t  ???????????  ?????????[^_]???&    ??    ?????V??S????  ??d?    ?D$1???????????D$d+    u??$????[??  ^???????????&    ??&    ??????V??S????  ??d?    ?D$1???????????D$d+    u??????  ??#$??[^????????????&    ??    ?????W??V??S??? ?  ??d?    ?D$1??????????????t&$?D$d+    u????? ?  [^_?L????t& ??#$?????????&    ?t& ????????d?    ?T$1??T$?D$    ?$    R?T$Rjj h?   P?C?????  ??????o  ?D$d+    u??????????&    ??    ?????S?X(j j jj jh?   S???????  ???????????????  ??[?????f??????S????  ???
p  ???  ??t%???  ??u&????????  ????????      [???&    ?v ?????????&    f??????UWVS??????  ???op  i??    ??   ?N  ??(      ???  ?2   ??,      ????  ?  ??      ??$      ???  ?  ???  ?  ???     ?????????t????[^_]??t& ??D  1??????  @  ??H  p?  ??????????  ?C\1?j ????????  ?D$X??  ????p  ?????????t^???@V  ??   ??h?  ???  i??    ??    ???  h?  j ?T$P??????D$?T$????      ?????  ???   ???  ???  ???  ???     ?   ???  ???  ?????     ??      ??     ???  ?  ???  ?  ???      ???      ???  ?????????   ???  ??(  ???  ??0  ???  ???  ???  ???     ???  t???  ???      ???  ????????   ?21?j?????Y_?????  ???&p  ?Sp  ?????[??^_]??t& ?????????f?f?f??????S??p????????p????????E???p????????E???p s???H(??t-?  [???????&    ???u?[???&    ?v ???????p ??t& ??????H`??t????  ??t????  ??t????  ??t????  ???  9??  t???    ?    ??????t& ????????&    f?????????H`t????  9??  t?    ???  ?    ??????t& ?????   ?f?f?f?f?f?f?f??????WVS??h??????@?d?    ?T$1??D$ ??t6?Pd???t-?r$??t&?    ?L$??tx?z$??tq?   ??????|$ u<?   ????????   ?????????????#   ????????#   ???????????????   ??????D$d+    u??[^_??t& ?   ????????????f?f?f?f?f?f?f???????VS??0??h  d?    ?T$,1?????)  ???  ???  ???      ?Pd?R????   ?J????   ?   ????  ?r????  ??????????  ???  ???{  ??!??w#?   ??????0  ???  ??   ?t& ????   ?Pd?R????   ?J????   ?   ???Y  ?Z???N  ???????1???   ???  ????   ??!??w??   ?????????   ???  t??D$ ??  ?D$$??  ?D$?] ?D$    ?D$  ?D$??? ?D$?  ?D$!   ?D$?????????&    1??T$,d+    ??   ??0[^???&    ???????   ???? u???k?????????????t& ??D$    ?D$  ?D$??2 ?D$ ?D$???????    ????????   ???? u????????????f?????????<????t& ????????<????t& ??????0??????????????U??W1?VS??  ???  ?T$d?    ??$  1????  ?D$    ??tz?Pd?R??tp?2??tj?   ????   ?   ?	?$??tu?L$Q?   ?t$?T$?????Z?D$????t1??C?T????????9?u??D$?   ???x????t& ?|$ uA????u1??$  d+    u^??  [^_]??L$Q?   ?T$?????X???C????????[?T$?D$    ?D$    ???D$???????0  ????????????????&    ??????WVS????0d?    ?D$,???  ????   ???     ?Pd?R??t'?J??t ?   ???
  ?r????   ????????|$
 ?5   ??   ??&    1??D$
f?D$???  ??tr?Pd?R????   ?J??t4?   ????   ?z????   ??????????  ??tp?Pd?R??tf?J??t_????tT?~??tM??????D$
???{????v ???  ????????  ????????  ??????D$,d+    u6??0[^_??v ??????D$
???t& ????????o????t& ???????????????????&    ???????h  ? ??t?z?????&    ?v ???&    ??&    ??????UWVS????0???  d?    ?D$,1???t??t???  ??w-???  ?t& ?D$,d+    ??  ??0[^_]???&    ??!??w??????s?????????   ?Ad?@????   ?x????   ?   ?????0  ?h???%  ??????????  ???  ?D$ ???D$
????   ???  ???  ?D$?????  ?D$
???>????Pd?R???0????J???%????   ????   ?Z????   ???????? ???????????????????$???????????D$,d+    ??   ??0??1?[^_]??????v ???  ?D$ ???D$
?Ad?@???2????x???'????   ????tZ?h??tS????????	?????&    ??????????  ???????    ?   ???$??????????&    ????????,????t& ???????????????????&    ?????UWVS???  ????  ???  ????"??  ??  ??0?I  ??  ??  ??3?e  ?t& ??   ?   ??  ??????????|  ?0i??    ??    h?  h?  ??????E?????^  ???   ???   ????  Ph?  h?  ??????E?????%  ?   ???????????  ?E?E?   ???  ?Ef???  ???  ?   f???  ???   ?P$f???E  ?@ f???  ???   ?@"f???  ???   ???  ?   ??h  ?C$????4  ?????8      ???  ??  ???   ?  ???  ?  ???  ???????????   [??^_]???&    ???????:u??g  ??????t& ??)w#??  ??'?u????????[??^_]???&    f???-??w???  ?N?????    ??w3???????v??   ??? u????t?????  ??????&    ???  ??!????????????v ???   ?N????t& ?f???  ???   ?@&?????f???  ???????    ????????????      ??????E??????E??????????????[^_]???&    ?v ?????????&    f??t  ?f?????????????????????????????&    ??&    ?????S?????????  ??t9?@????????  ?@????????  ?@????????  ????????      [?f?f?f?f?f?f?f?????????????   ???   ???   ??  ??  ?? ??T$?1????&    ?t& ?????WVS???   ????P  ?????????$  ???(  ?S??,  ?S??0  ?S??4  ?S??8  ?S??<  ?S??@  ?S??D  ?S ??H  ?S$??L  ?C(???????1?[^_??t& ?????WVS???   ?????   ??????????   ????   ?S???   ?S???   ?S???   ?S???   ?S???   ?S???   ?S???   ?S ???   ?S$???   ?C(???????1?[^_??t& ?????W??P  VS???   ????????????     ????????      ??????????      ????????  ???   ??  ??.  ???????1?[^_??t& ??????W???   V??S???   ??????????     ????????      ???????????      ????????      ?????1????   ??  ???  ???   ??  ???   ???????1?[^_??t& ??????UW??VS???   ?D$?L$d?    ??$?   ???   ???   ?D$???      ? ?D$ ???   ??  ???   ??  ??????? ???? ?? ?D$?????$?????D$?????=?  ?D$??p  ??t????  !??|$???$???D$???D$??????   ??!??D$?T$???p  ?L$1????b  ?D$  ?$??    ???   @  ? ?????????T?@???   t=??u?????????@   ???$?T$@??????????????????@??   ??q  ?t& ???????????????$?T$@????????$???????;$??q  ?D$?D$<    ? ?|$ ?r  ?|$ ?????t*?L$<?|$ ???   !???   ???   ??   ?D$? ????????L$????  +?  =?  v?L$<????????\$<??t?L$????   ??$?   d+    ??   ???   1?[^_]?1????????&    f??|$ tU1?????????     ??????D$?D$<   ??t ?@`??t?h8??t?D$?L$<?v@??????D$? ??????|$ ?D$<    ? ????Qr  ?t& ?|$?G`???H????X8???=????L$<? v@????????(?????????t& ?????U??WVS??$???   ?D$ ??  ??  ?D$???u?D$8?     ??$1?[^_]???&    ??   ?L$???$??????$?L$????  ??????????$????????$?|$8??????????t????D? ???D$?$???\$??   ???\$?????D$??t?t$?D$???D$1??D$    ??1??L$?$?????????????????D$?$$???  ?i??  ???1???????t?????K??????????????k?6)??????6   ????????v??? ? =?h?w
??Mb???????U 
\$?=?  ?E    ?]?wr  ??;l$??????u ??%?? =?? ? ????D$ ?   ?D$   ?D$    ??????????UWVS??$???   ?z
 ???   ??  ?B???D$????  ??P  ?$??????C
??.  ?C    ??(      ?   ??$     ???     ????????      ????????K???   ????   ?1????1???C??/  ????   ?K1?????????7????=   ?M  ????  ?@????D$???D$??????D$?????T$ 1???7??i??  ???K?????????????T$???   ?L$??  ?{?Y????4  kKd??8  ?????   ??	??q  ???9?G????   ??  ???D$k?d?????S??8  ?T$ ?  ?v ?C? ? ?D$    9?G??6   ????????t????Mb?????????T$i??  )???????  ?????T$???  v???? 1??D$????t"?@  ??????)??????????????)?)????D$??? ?????   1?9??????  ?   9???  ?????????D$????????T$???T$i??  ???K?D$???????????T$???   ??  ?D$?T$?D$    ??????t?????K???????????T$k?6)??????6   ?????T$??v???C??0  ?D$??d  ?D$?C(??L  ???      ??????K???   ????   ?1????1???C??=  ?K???   ????   ?1???1???C??<  ?C	??-  ?C??,  ??tC?{	 t???     ??? ????   ?{ ??  D|$??   ??   1?%???1???$?????1???$[^_]?f???$??[^_]????f??D$?J ???  ?D$??  ?D$  ??D$?  ?|?????    ?D$?J ?3   ?D$  ??D$ ?  ?D$??  ?D$??  ???????    ?D$?   ?   ?D$   ?D$0???D$    ??????    ?D$?   ??? ?D$0???D$     ?D$   ?D$   ?k?????    ???????????    ?????UWVS??$???   ?z
 ???   ?  ?B???D$???}  ???   ?$??????C
???   ?C    ???       ?   ???      ???     ????????      ?????????K???   ????   ?1????1???C???   ???5  ?s1?????????7????=   ??  ????  ?@????D$???D$??????D$?????T$ 1???7??i??  ???K?????????????T$???   ?L$??  ?s??k(????   ?C2   ???   2   ?K???q	????)????1???9S s?Q
??????????   ?????S ?O??N??????9S$v??????????????S$???   ??   ?1????1???C ?T$ ???   ?C$???   ?D$?  ?t& ?C? ? ?D$    9?G??6   ????????t????Mb?????????T$i??  )???????  ?????T$???  v???? 1??D$????t"???@  ????)????4??????????)?)????D$??? ????1??   9??????  ?   9????  ?????????D$????????T$???T$i??  ???K?D$???????????T$???   ??  ?D$?T$1??D$????t?????K??????????????k?6)????D$?6   ????????v???C???   ?D$??  kK6???   ???  ??????.  ???  ??  ????Mb?????K????i??  ??????????  ?D$?S???   ?C(???   ???      ????????      ?????C???   ??  ?C	???   ?C???   ????   ?$?????1???$[^_]???&    f???$??[^_]?r???f??D$?J ???  ?D$??  ?D$  ??D$?  ???????    ?D$?J ?3   ?D$  ??D$ ?  ?D$??  ?D$??  ???????    ??? ???  ???????&    f???   ?????????      ??      ??????????{	 t???     ?????{ ?@  DD$?????   ??   ??   1?%????1????????t& 1?1??h?????&    ?D$?   ?   ?D$   ?D$0???D$    ???????    ?D$?   ??? ?D$0???D$     ?D$   ?D$   ???????    ??????e?????    ?????UWVS????$???   ???   ???   ??   ???   ??  ?L$???   ??  ?L$???   ??  ?|$???   ??  ?|$???   ??  ?|$???   ??  ??p?D$?f  ?^  Sh  ???????   XZ?W  ?[  D?PSh3  ?????????????E??$QSh<3  ???????????@   E?RSh46  ??????????4?   Sh?7  ??????z  ?o  ???? @  D?PShl3  ???????  ??  ????   D?PSh?3  ???????  ??  ?|$????    D?PSh?3  ??????L$?????(s  ?cv  ?t& ??????U?h  ??  WV??S??0d?    ?D$,1??$   ?????????  ??  ?   ???   ??       ??h?  ?????Z????  ???   ?`  ??????????  ???   P?Cph7  j P??????F?????      ????????????	  ???   ??  ??  ???   ?A  ????????
   ?t$?$   ?????Cd?D$
?D$ ?D$ ??  ?D$$??  ?@?D$?D$??  ??t(?H??t!?   ??????   ?x????   ?????????P  ??  ?X  ???????1??
   ???Cd?D$   ?$   ?@?D$
?D$?D$??  ??t2?H??t+?   ????tI?p??tB??????????&    f?????????D$,d+    uA??0??[^_]?f?????????^????t& ???????????&    ????????????????????????????    ?????V?   S???????t<?????   ???????   ????????   ?????????  ????????????1?[^????????f?f?f?f??????S??f???   ??d?    ?D$1??   f?T$??f?D$?D$?D$???$D   ????????Jw  ?   ?????D$  ????????4w  ?D$?T$d+    u??[???????t& ??????VS??
  ????d?    ?T$1??  ?G????   ?  ???f?L$?t$?   f?T$???D$???$D   ?t$?????????w  ?   ??   ???t$f?L$?   f?D$???$D   ?D$??????????w  ?   ?	   ???t$f?L$?   f?D$???$D   ?D$????????vw  ?  ???~????   ??f?L$???   ?t$?  ?D$???$D   f?t$????????`w  ?D$d+    u??[^??????f?f?f?f???????1????&    ??????U??WV??S????Pd?    ?L$L1??D$hf?$1?f?L$?|$d?P?\$f?T$?T$?T$??s4??up?   ???????????u}1??T$Ld+    ??   ??P[^_]?f???l$?L$)??T$?T?)??T	????????r????1??,:?,9??9?r?????    ??L$?t??T?f?T?v?????    ??  ????w  ??????m?????????v ?????WVS???p,d?    ?L$1??D$???u|???  ??????VL?FH1?j?|$W?????Y??X??u5???  ??????VL?FH1??D$ jW???????  ?????1????????XZ?D$d+    u????[^_???&    ?v ?????????????t& ?????V??S???? d?    ?T$1????L$?t$f?T$f?T$?T$,?L$?   f?L$?   f?T$?T$?D$ ?D$   ???????u)??  ????w  1??T$d+    u"?? [^???&    ??  ???x  ??????????????    ?????S??1???(d?    ?D$$1??C???????    9Cx?C???t?D$$d+    uO??([??t& ?S??C?j!?L$?????Y???    ?  ?C??u???  ???.x  ?D$???????C??????????&    ??    ?????UWV??S???D$,?L$?$?D$4?D$d?    ?D$?Fd?????  ??9~lte?VL?FHj?L$?^?????X??t?D$d+    ??  ????[^_]??t& ?D$?VL???
D$?D$?FHj?L$Q1??c???Y]????u??~l???  ??????????   ?   ??$  ???   ??   ?D$0???????      ????????   ??   ?D$0?????????      ??????<$ ??   ???   ??   ?D$???????      ???????~L@?????E?!?????   ??   ?
?<$?% ???-@  !???=    ?????&    f??    9?x??  ????????      ? ??u????   ??   ??   ?????????T$???????  ?T$???Xx  1??<$ D??_?????    ???   ??  ??$  ?"?????????9??????????&    ?t& ?????S?\$???@,SQ?   j ???????[???&    ?t& ?????????@,j Q?   j????????f??????S?\$?@,SQ1?j ?g?????[?f???????@,j Q1?j?L????????&    ??????WVS????T?p,d?    ?T$P1?????   ?VL?FHj?L$??????VL?D$?   ?~H?D$
   ??`f?T$?T$?D$?T$?T$?D$???D$ ?????Y??u???T$Pd+    u*??T[^_??t& ??  ????x  ????????v ?????????????t& ?????VS??P?p,d?    ?\$L1?????   ??ur?   ?   8^htF??  ????x  ?VL?FH?   ?\$?D$   f?$?T$?T$???D$???????uH?^h?FP?T$Ld+    uL??P[^???&    ?=?  ???????=?  ???????u????t& ???  ??t??	y  ??????????????t& ???????  V??S?????ey  1???  ?t??  ? uE??   u?   [^??t& ???8  ?    ?    ??T???????  ??t??(y  ??&    f????  ?    ?HT?    ???????  ???Iy  ??   t?????&    ??&    ??????UW?31  V??S?   ??(?-?  d?    ?D$$1??=?  ?D$    ??D$ D  ????D$
    ???D$  D ?D$    ?D$   f?\$?D$ ?D$?D$  f?|$"????y  ?   ?t   ??  ??????????y  ??  ?F???T  ???  ?CLA?A   ??=?  ?    ?C    ???   ?1??C    ????{H???C    ?Ch?C    ?C    ?C     ?C$    ?C(    ?[,?sdj"?D$P????????SL?   ???CHj?l$&U??????SL?   	??CHjU???????	?uzj1??F(???????Y????ue?CX?CT?????KT?    ?CX?C\?    ?C` ?  ???????  ????y  ?D$$d+    uN??(??[^_]???    ?SL???????&    ??  ????y  ???????????&    ?v ?CL@?@   ???????????=?   ?????t???y  ??&    ?v ???????t/S??  ??t?C,??t?????????[???????    [???    ?f?f?f????????? d?    ?L$1??   ?T$??f?L$?L$?L$?   f?L$?L$?L$?   ?$P   ?D$P  ?D$ ?????????y  ?D$?T$d+    u?? ????????&    ??&    ??????S??d?    ?\$1??   ?T$??f?\$?\$?L$?   ?$P   ?\$????????z  ?
   ??&    ?X?A ???????u?1??T$d+    u??[???????????W??V????   S?@   ??????Ff???????????D?????Fu??v ??????????D?????Lu?[^_?f?f?f?f?f?f?f???????W?   V1?S????&    ?   ?   ?????????   ????????R? ??????   ?   ????????   ????????R? ?????1??   ????????   ???????	???,??u???[^_???&    ??&    ?????W?   V??S???E?t& ???????R? ???????   ?   ????????   ????????R? ???????,??tX?   ?   ????????   ????????R? ??????   ???   ????????   ?????w?????????r????t& ?[^_???&    ?t& ??????W?8  VS??d?    ?L$1?f?<$?D$???I  ???p  ???P  ??$  ?L$?   ?@ ?????&    ?v P??9?u????   ???T$?   ??????   ????????   ?   ????????   ????????R? ??????   ?   ????????   ????????   ??????   ?   ????????   ????????   ???????   ???2????   ??????   ?|$?t$???&    f??????????9?u??   ?   ??1???????   ????????   ?   ????????   ????????v ?   ???????1??   ????????   ?????????t??<v???<?z  ?   ?   ????????   ????????   ???????   ???R????   ?????????????   8???F?1??v ?????????????8?u??   ?   ????????   ????????   ?   ??????????   ?????1??T$d+    uN??[^_???    ?  ?D$f?\$??????t& ?  ?D$ f?t$??????t& 1??D$ f?L$??????????f??????1????&    ???????P?H|?R?AP????!??AL?f???????@|?4  ?????????&    ?t& ?????UWV??S???X?{??????????=  ????????  ?3?k ??d  ?D$  ?$??  ?????7z  ???   @  ??????u?M ?T$?????????   ?U ??P  ?   ???   ??X  ?   ???   ??0  ??  ?C    Z?????  ???   ? ??P  ????????   ??T  ???   ??  ?*????   ??????!?	???????      ??? ????   ??@  ??@  ????5?  ????   1????????????[^_]???&    ???  ?3?????Uz  ???   @  ????????     ????????   P  ??????????  ??t???d  ?1???  ??????x?????$?1???????f????D$?S?M ??  ?????????   ? ??QRP?u ??  Ph<;  ??????????????&    ?t& ??????UWVS?h?X|???mz  ???   1??????????????   ?}$1????   ?   ????)  1????    ???      ??       ??  ?   ??  ??  ??  ??  ??     ??     ??    ??!    ?E ??d  ??  =   t???????  ??   [??^_]????  ????v???z  ??&    ??&    ??????P????   VS????  ??????z  ?s ?N ??ua?F??????F    ?F??????F    ?S ?C?J?rj ?r?   ?r????????C ??????C     ???C    [^???&    ?V?N$?Cj j?   ??????F     X?s Z?w?????&    ???&    ??&    ?????S???@|??4  ??t?C?????C|??4      1?[?f??????UW??VS?????p?@|??4  ??t????????G|??4      ??L  ?F??X  ???V?F????  ?B?????  ?   ?(   ??  ???????????  ?F??F?^ ???  ???? ??????$????????$???C??  ??  ??????z  1???????k$?Dm ????????C???  ?S$??????C$??tU1??#??&    f?	??A    ????A   9k$v.????C?????????   ?K?Tm ??????t??t& ?n ?F?M$?Uj ?   j??????E Y]????z  ?K?Fj?S?v?v???????????xu?C?   q?C?S?P?C?@    ?V ?G|?R??4  ?G|?V??@  ?G|1???8      ??<      ?7?C??????C    ?C??????C    ????????$??????F     ?$????[^_]???&    ??    ?????????  ?????????v???z  ?????1????&    ??????UWVS??ts??0  ???????   ??T  ??   ?{  ??u???   uR[??^_]??v ??  ?????V{  ???   ?   ??0  ?   [^??_]???&    ?v 1?[^??_]???&    ???   ??   ?E?E$???????[^_]?f??????VS????d?    ?D$1???  ???+  ??d  ??  ???p{  ???   T1??????j(h    ?   ???????????{  ?$?T$1????  ????   ?F    ?F?$?FR?  jj ???????????   ?D$?   ?  ???  ???  ??X??????D$?   ?   ??????$?   ?)  ???????h1  ?D$??$P??????D$S?P$??DRhB  P??????D$?????????x-?=?  ???{  ?D$d+    u ????[^???&    1????$???????{  ???????&    ?v ???????0  ?@??????????????  ??????  ?t$,Sh8   ????????D$Dd+    u=??H[???[???? @?????  ????????? ?????  ???????????????????VS???  ???   ?x$ uSSSSShd   ????????SSSShD  ???????S1?h  ?????XZi?  ??    VFSh\  ??????????u?[^?Q?????h   h?  ????????m  ?=    Yt}h   h?  ????????  _Y????  ?   ?   ?"   ?   ??E?VRh  ????????   ???D$???   ?????????uP?$?D$??????$??????B  ?5#   i??    ??    ???  Ph?  ????????  ??????  Vh<  ?????[^???$?sSh4  h  ??????T$????  ?$?sSh4  hP  ??????T$????  h?   h?   h?  ????????=   ???  ???  ?0?p???  ???  ?sSh  h(  ?????1???$??!  h  h  h?  ????????  ???`"  h  h  h?  ????????  ???#  ht   ht   h?  ????????j$  QRPht   ht   hx  ?????????$  ?0  ??????(  ??????v??????W??????Vh?  h?  ????????'  h?   h?   h?  ????????c'  ?0  ??????(  ?????Qh?   h?   h?  ????????I(  h`   h`   h?  ????????{(  h  h  h?  ????????)  Ph  h  h  ?????????)  h?   h?   h?  ?????????+  h?  h?  ?????XZ??,  R?T$h?  h?  hd  ??????T$???-  h?  1?h?  ?????XZ?=x   u????$u???-  ???????????PVh?  h?  ?????????i??    k???    ?pD?p@?p<?p8?p0Vh?  h?  h?  ???????$??-  R?T$h?  h?  h?  ??????T$????/  Rh?   h?   h?  ?????????d0  hl  hl  h?  ?????????0  ?0  ??????(  ?????h?   h?   h?  ?????????1  Ph  h  h  ????????%2  Sh   h   h?  ??????????S2  ?=x   ??2  h   h   h(  ?????????????2  ?t$?t$?3?t$h?  h?  hL  ?????????4  h?  h?  ?????[^??4  h   hx  h\	  ?????????6  ?=x  vh   hx  h?  ??????????  Ph?  ???????d  ???????XZ??6  h   hx  h	  ?????????6  h\  h\  h?  ????????R7  ???  ??  ????   PVh?	  ??????Q  ?@  ???\(  ???  ?????  ???  ???    ???  ??4t??8????   ???  ???      ???  ???  ?? J  ???   ?2?   j?????Y_??y?$Vh?	  ?????XZ?$??:  hD  hD  h?  ????????8  ?$Vh?	  ?????]X?$??:  ???   ???????  ???  ?o??????      ???  ?Z??????  ??  ??t#PVh
  ????????????1???0  ????:  ???   ???sSh?  hD
  ????????k>  ?sSh?  h|
  ????????k>  h?  h?  h?  ????????*@  h?  h?  h?
  ????????gA  ?????UW????VS????   ??u??  P?   ?   Sh?  ???????????????t?4??  h?  ?????XZN??u????  Sh  ????????  ZY??u?   [^_]?P?t$??????Uh?  h  ????????*F  P?t$??????Uh?  h  ????????*F  ?2hx  hx  hL  ????????dK  ?$?2hx  hx  hx  ??????L$???TI  ?$?L$?A?PVhx  hx  h?  ?????1??L$????I  ?t$?t$?7??h  hx  h?  ????????BK  ?????U1?W??V???  S?????2Vh8  ????????C??????   ? P????<  Vhh  ?????????8u?1??C?D?8?????   ??$UEVh  ??????D$????????u??4$??1????G??    ?L$??????   ?0U_S?   ?t$h?  ????????u????????l$?l$????9?s1?B?T$???????   ? SCPVF?t$h?  ??????T$???????l$??????l??????4$???C?S?RPVh?  ??????C?P`RPVh  ??????C?????   ? ?? PVh0  ??????C?????   ? PVhT  ??????C ?????   ? PVhx  ??????C$?????   ? ??$PVh?  ???????[^_]???   t ?=?  vh   h?  h?  ?????????   t$?=?  ??   h   h?  h?  ???????????   ??t ?=?  v;h   h?  h   ?????????t ?=?  vh   h?  hP  ??????????  Ph?  ????????   ?????   ????   ??!??kV,???d  ?????XZ?)L  jh?  h?  ?????????K  ????   ??u??k???hd  hd  h?  ?????????Q  kV,???d  ???????  ?$???  ???  Ph   ?????1??\$???t$?????  ??9???   V?   ?t$h.  ???????    ?D$???  ???????V???t$???l$???????  9??e  S???4?V???t$h0  ????????????  ???  ???  hd  hd  h?  ????????nO  PRhd  h\  ?????????????iR  ?\$?t$??O  ?.hL  hL  h?  ????????=?   ?CR  ???      ? PhL  hL  h?  ????????=?   ?CR  ????A  PhL  hL  h?  ????????=?   ?CR  ???   P  ? PhL  hL  h4  ????????=?   ?CR  ???   @  ? PhL  hL  hl  ????????=?   ?CR  ???   `  ? PhL  hL  h?  ????????=?   ?CR  ???   @ ? PhL  hL  h?  ????????=?   ?CR  ???   L  ? PhL  hL  h  ????????=?   ?CR  ???   ,  ? PhL  hL  hL  ????????=?   ?CR  ???   d  ? PhL  hL  h?  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h?  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h?  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h$  ????????   ??  ?????~  ???CR  ???   ?????   ? P???   hL  hL  h?  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h?  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h?  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h(  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h\  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h?  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h?  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h?  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h,  ????????=?   ?CR  ???   ?????   ? P???   hL  hL  h`  ????????=?   ?CR  ???     ? PhL  hL  h?  ????????=?   ?CR  ???   8  ? PhL  hL  h?  ????????=?   ?CR  ???   <  ? PhL  hL  h?  ????????=?   ?CR  ???   H  ? PhL  hL  h0  ????????=?   ?CR  ???   ??L  PhL  hL  hd  ????????CR  ?\$???t$???l$?<   pu?\$?t$??O  ????????CR  ?????   ? P???   hL  hL  hX  ???????  ???F????$Sh8  h8  hF  ??????T$????R  ?$h$  ?sSh$  h?  ??????T$???T  ?$h$  ?sSh$  h?  ??????T$???T  h  h  h]  ??????7???=?   ?zT  h?  h?  h]  ????????zT  ?vhr  ??????Uh?  h  ????????xU  ???  VPh?  ?????????   t?=?   th?  h?  ?????]X??   t?=?   th?  h?  ?????Y_??   t?=?   ??   h?  h  ?????XZ????    ??   t?=?   t/h?  hD  ?????Y]??t?=?   th?  hl  ?????XZ????   ??   t?=?   t/h?  h?  ?????]X??t?=?   th?  h?  ?????Y_???  ?????   ????  ??!??k??  ,???d  ?????????????}V  h?  h?  ?????XZ?\V  ????    ??   ?Q????.????L$h   h?  h?  ??????L$$????X  ?L$jh?  h?  ??????L$$????X  ?L$h  ? h?  h  ??????L$$????X  ?L$h   h?  h  ??????L$$???vX  ?L$h   h?  h?  ??????L$$???LX  ?L$h   h?  h?  ??????L$$???aX  ?L$h   h?  hd  ??????L$$???"X  ?L$h   h?  h?  ??????L$$???7X  ?L$jh?  h@  ??????L$$????X  ?L$h   h?  ht  ??????L$$????[  ?L$h   h?  hH  ??????L$$????X  ?L$jh?  h?  ??????L$$????[  ?L$jh?  h?  ??????L$$???u[  ?L$jh?  hl  ??????L$$????X  VUh?  h  ????????Z  ?T$0?L$,VUh?  h  ??????L$<?T$@???=?  ??W  R?t$?t$h?  hP  ??????L$@???=?  ??W  ?L$?t$ ?t$?t$h?  h?  ??????L$ ???=?  ??W  ?t$$?t$Q?L$h?  h?  ??????L$ ???=?  ??W  ?t$(?t$ ?t$h?  h   ??????L$ ????W  ???  Ph?  ?????YX??\  ???  Vhy  ?????XZ??\  ???  Ph?  ?????YX?.]  ???  Vhy  ?????XZ?V]  ???  Phy  ?????X1?Z??]  ???  Ph4  ?????YX??]  ???  Phl  ?????XZ?K^  jh?  h?  h?  ?????????d  Ph?  h?  h?  ?????????`  hl  hl  h]  ?????????h  u  u???    ????????LE  ???  i?  ??   ???  ??    ???  ?L$??$   ?L$???  ??   ??    ?=?   ???  ?R!???  tV?t$?D$PVh?  h?  h?  ????????=?   t%???  P???  h?  h?  h?  ??????????  i?  ??   ?(  ????????=?   th\  h\  h]  ???????????F  ???   ??  ???????   ??$  ???   ??4  ???   ??D  ???   ??T  ???   ??d  ???   H  ???   ??? P ???   ??L  ???LE  ?d   ???????d  ?@  ??j ???????d  ??   j ?P,?????????d  ??  j ?PX?????????d  ??   j ???   ?????????d  ??   j ???   ?????????d  ??  j ???   ?????????d  ??   j ??  ?????????d  ??   j ??4  ?????????d  ??   ?? j ??`  ??????????????????A  ???LE  ???   ???????|  ???????
  ?????????????C?p?X?C?D$?FL;D$??   ?Fd?@??t???t????????vL??L??jh?  h?  h?  ?????????e  ??   ?????????????   ???????T  ??P  ?|$Wh?  ????????   ?P&R?@$PWh  ??????-?  ?? ?x_  ??????????  7u1??   ??????????  i?  ??   u"?????????yh?  h  ?????Y^???  i?  ??   ??    ????  ?B??t???  ???  ?????????  h?  h\  ?????X???  Zi?  ??       ????  ?@??t??0  ??   ???????yh?  h?  ?????XZ???   ??  ??????  ???uC????  ??t5????  w*????  ??t????  w????  ????  ???   h@  h4  ????????   ?????c  Ph@  hh  ??????????  i?  ??   ~???    ????????  ???:  ??#?1  ?G,???   ?   ???   ??????????   ??P  ??T  ???   ?D$??L  ?D$???   ???  QR?t$?t$ VP?t$(h?  ????????? ??????D$???1????????????   ?t$h   ?????XZ?x_  ???5????????????yh?  h?  ?????Y^???  ???????L?????????????=???h?  h   ?????Y^?'?????   ???   ?????j h@  h?  ??????????????   ?????    ?????????????   ??   f?x"????   D??????   ???????L  S1???V  ?t$h?   ?????????y+?D$??L  ?t$h@   ?????????C  ?t$???x_  ???  ??t??u?  ? ?????????  ????????????????????????_  ???   ??   f?x"????   D????????   ???????   ???????   ?????h?	  h?	  h?   ?????1?????h  ?$??tXh?	  hk  ??????EZY?$f???0h  1???h  ?E?L$?$Ph?	  h?	  h?   ??????T$?L$???h  ?E ???Ph?	  h?  ??????E?T$?????B?$?0Ph?	  h?  ??????T$?????`h  ?=?   ?`h  h?	  h?  ?????Y[?$?`h  j?h?  ?????X?????Z??h  h?	  h?	  h?   ?????1?????k  j?h?  ?????X?????Z??k  ?$?t$Qh?	  h?	  h?   ??????M?T$????i  ?$Qh?	  h?	  h?   ??????M?T$????i  ?E?T$? P?E ?Ph?  ??????T$???$?9j  ?$h?  ?????X?$?9j  ?B?$?8Ph?  ???????XZ?$??j  h?  ?????[?$??j  Qh?	  h?	  h!  ?????????k  ?B?L$P??T$PVh?	  h?	  h8!  ??????T$?L$???5l  ???  ?s1?Wh	  ???????j?D$1?f??z  ?T$?????Y??yE?????   u???l  ????m  ??m  ???  ?sWh?	  h?!  ????????=?   t????? 
  ??tPVWh?!  ?????????? 	  ?????  ?sVh "  ???????????	  ??????sh?	  h?	  ht!  ??????????l  ??	  ?????R?T$h?	  h?	  h,"  ??????T$???Qn  h?  h?	  ?????XZ??o  hp  h?	  ?????XZ??o  hX  h?	  ?????XZ?-p  h@  h?	  ?????XZ?}p  ?????U?   W?????VS????L  ?3d?    ??$H  1??C????????g  ???  ????   wV??tz????   ???   ??  ??????S?K@?B?k????  ?s@f???  f?CDQRhx"  ???????????????u??0w??-?  ??   ?_??11?????   ??  ?{uF?   ?T$H??l  f??n  P ???????$?   Ph?"  ???????$?   ?C@??$?   f?CDY[1???  ?S(?  ?4$1??T$?S?D$???D$u  ??????X  ?{w??   ?T$H??l  f??n  P ??????C???   RPi??    ??    h?"  ??????C???  ???f?@?S@f?CD?e????l$?C(?   ?\$?$??1???	  ???   ????????D$?s  ?????f?D$&@ ?d$,Uj??????????   ???????XZ????   ?? ?????   ?ET??tz?@???????u	????????e1??{??8  uW?   ?T$H??l  f??n  P ???????$  Pi??    ??    h?"  ???????$  ?C@??$  ??f?CD??$H  d+    t???????L  ??[^_]??????U?   WVS??  ?D$d?    ??$   ?D$?D$    ?D$    ? ?$?D$???D$????????????????A  ?$?  ?E?D$?D$???       ??  ?p  ?$???  ????  ??   ??wv????  ???
  w9????  w????  ????  ?<  ????  ????  ??;  ??
?#  ???=  ????
  ??	?Y  ??;  ??w2???  ????  ???X  ???t  ????  ??;  ???  w????  ???;  ?|;  ????  ??  ??)w}??'??  ??!??  w0???t  w????  ????  ?3;  ????  ?%;  ??$?~  w??"??  ??#?g  ?;  ??%?D   ??&?#!  ??:  ??2??*  wB??/??)  w ??-?R%  ??.?/(  ??,??  ??:  ??0?(  ??1?)%  ??:  ??5??#  w??3??,  ??4?$.  ?}:  ??:?9  w??8??1  ??9?`:  ??5  ??;??-  ??<??1  ??=?@:  ?x5  ??	  ?????????uh?	  j???????	  ???????XZ??t7?$???   ?V  ????????????3  ??	  1???????E    ??9  h#  1?1???????E    X??9  ?
  ?????????uh
  j??????
  ???????XZ??t1?$???   ??S  ?????????u3?
  1???????E    ?p9  hx#  1?1???????E    X?U9  ?E?*
  ?????????uh:
  j??????*
  ?????^_????t,?$?`   ???  ?Eh?S  ?????[??u?*
  ??????h?#  ?????Y?<$?E???  t	1?1???8  ???   ?|$1?1???<  ???   s  ??8  ?Q
  ?????????uh`
  j??????Q
  ???????XZ??t1?$???   ?pU  ?????????u3?Q
  1???????E    ?S8  h?#  1?1???????E    X?88  ?E?*
  ?????????uh:
  j??????*
  ?????_??X??t,?$?`   ???  ?Eh?T  ?????^??u?*
  ??????h?#  ?????[?$h?T  ?`   1?1????  ?  ?????Y??7  ?=?  ??	  ??  ?????????uh?	  j???????	  ???????XZ??t1?$???   ??U  ?????????u3??	  1???????E    ?=7  h#  1?1???????E    _?"7  ?E?v
  ?????????uh?
  j??????v
  ?????Y^????t,?$?B   ???  ?Eh<U  ?????Z??u?v
  ??????h$  ?????X?*
  ?????????u"h:
  j??????*
  ?????^_??????   ?$???  ?Eh?T  ?`   ?????[????????*
  1?1???????Q6  ?E?v
  ???????????  ?$?B   ???  ?Eh<U  ?????Z??u
?v
  ??????*
  ???????????  h:
  j??????*
  ?????Y^?????j  h?#  1?1??????Y??5  ?????????uh?	  j???????	  ???????XZ??t7?$???   ?V  ?????????????   ??	  1???????E    ?f5  h#  1?1???????E    _?K5  ??	  ?????????uh?	  j???????	  ?????[^????t3?$???   ??U  ???????????u3??	  1???????E    ??4  h#  1?1???????E    Y??4  ?]??	  ?????????uh?	  j???????	  ?????_??X??t-?E??U  j ???????[?????????	  1?1???????q4  hH#  1?1??????Y?]4  ??
  ?????????uh?
  j???????
  ???????XZ??t8?$?   ???   ??U  ???????????u3??
  1???????E    ??3  h8$  1?1???????E    X??3  ?E??
  ?????????uh?
  j???????
  ?????_??X??t-?E??j@?a   ?????^???????????
  1???????~3  hh$  1?1??????Y?j3  ??	  ?????????uh?	  j???????	  ???????XZ??t1?$???   ??U  ?????????u3??	  1???????E    ?3  h#  1?1???????E    X??2  ?E??
  ?????????uh?
  j???????
  ?????^_????tj?$?\U  ???  ?E??????????????
  1?1????????2  ?E??
  ???????????  h?
  j???????
  ???????XZ???h  h?$  1?1??????Y?A2  ??	  ?????????uh?	  j???????	  ???????XZ??t1?$???   ??U  ?????????u3??	  1???????E    ??1  h#  1?1???????E    _??1  ?E?$??$?       ?  ??$?   a??$?   ?  ??$?       ?????????uh"  j??????  ?????[^????t?E??$?   ???????u?  ??????!h?$  ?????Y????  ??t
??N  ?????1?1?? 1  ?7  ?????????uhG  j??????7  ???????XZ??t5?$???   ??U  ???????????????7  1???????E    ??0  h?$  1?1???????E    _??0  h?
  j??????v
  ?????Y^?????F???h$  ?????_?`????$???  ?EhtT  ??????7  ?????????uhG  j??????7  ???????XZ??t1?$???   ??U  ?????????u3?7  1???????E    ??/  h?$  1?1???????E    X??/  ?E?*
  ?????????u"h:
  j??????*
  ?????^_??????????$???  ?EhXT  ?????^  ?????????uhn  j??????^  ???????XY??tl??$?   ?????????0  ?^  ???????????74  ?D$?@?D$?X??^  ???????????  hn  j??????^  ?????^_?????h  h,%  ??????????_??3  ?D$?@?D$?p???	  ?????????uh?	  j???????	  ???????XZ??t.i??  ?<$???   ??U  ??????????????	  ??????h#  ?????X?E    ?Q
  ?????????uh`
  j??????Q
  ?????[_????t8i??  ?<$???   ?|U  ?????????????Q
  ??????E    ?&???h?#  ??????E    Y????i??  ?$?HU  ??0?   ?E?????????????"????D$?@?D$?X???  ?????????uh?  j???????  ?????^_????t:i??  ?$???   ?S  ???????????u3??  1???????E    ?M-  h\%  1?1???????E    Y?2-  ?E?  ??$?       ??$?       ??$?   ??$?   a?????????uh"  j??????  ???????XZ??t?E??$?   ???????u?  ??????!h?$  ?????X????  ??t
??N  ?????1?1???,  ??$?   ????????a0  ?'?????  ?????????uh?  j???????  ?????[^????t1?$???   ?S  ?????????u3??  1???????E    ?,  h\%  1?1???????E    Y? ,  ?E?$??$?       ?  ??$?   a??$?   ?  ??$?       ?????????uh"  j??????  ???????XZ??t?E??$?   ???????u?  ??????!h?$  ?????_????  ??t
??N  ?????1?1??Y+  ??  ?????????uh?  j???????  ?????[^????t1?$???   ?S  ?????????u3??  1???????E    ??*  h\%  1?1???????E    Y??*  ?E??  ??$?   a ???$?   ?  ?????????uh?  j???????  ???????XZ??t,?$??$?   ???  ?E????????;.  ??  ??????h?%  ?????X?t$h&  ?????_]?+  ??  ?????????uh  j???????  ?????[^????t1?$???  ??R  ?????????u3??  1???????E    ??)  h@&  1?1???????E    Y??)  ?E1?1???      ??)  ?D$?@??t??tu1?1???)  ?  ?????????uh&  j??????  ???????XZ??t%?$???  ??R  ?????????u?  ??????hp&  1??????_?]1?1??6)  ?  ?????????uh&  j??????  ?????[^????t%?$???  ??R  ?????????u?  ??????hp&  1??????Y?]1?1???(  ?<  ?????????uhJ  j??????<  ???????XZ??t3?$???  ??R  ???????????u3?<  1???????E    ?i(  h?&  1?1???????E    X?N(  ?E?_  ?????????uhm  j??????_  ?????_??X??t?E???hR  ???????u?_  ??????h?&  ?????Y?E1?1???      ??'  ??  ?????????uh  j???????  ???????XZ??t%?$???  ?XR  ?????????u??  ??????h@&  1??????X?]1?1??q'  ?D$?@??t???+  ???????  ?????????uh?  j???????  ?????^_????t5?$1????   ??R  ???????????u3??  1???????E    ??&  h '  1?1???????E    Y??&  ?E??  ?????????uh?  j???????  ???????XZ??tc?E????R  ?????????(  ??  1?1???????~&  ?E??  ???????????   h?  j???????  ???????XZ????   h0'  1?1??????X?1&  ??  ?????????uh?  j???????  ?????^_????t<?$?   ???   ??R  ????????????a?????  1???????E    ??%  h '  1?1???????E    Y??%  ?E????R  ????????#(  ??????  ?????????uh  j??????  ???????XZ??t1?$???   ?@R  ?????????u3?  1???????E    ?1%  h?'  1?1???????E    X?%  ?E??
  ?????????uh?
  j???????
  ?????^_????t&?$?,R  ???  ?E???????u??
  ??????h?$  ?????Y?E?|$1?1????   ??<  ???   s  ??$  ?1  ?????????uhA  j??????1  ???????XZ??t1?$???   ?$R  ?????????u3?1  1???????E    ?/$  h?'  1?1???????E    X?$  ?E??
  ?????????uh?
  j???????
  ?????^_????t&?$?R  ???  ?E???????u??
  ??????h?$  ?????Y?E?|$1?1????   ??<  ???   s  ??#  ?  ?????????uh  j??????  ???????XZ??t1?$???   ?Q  ?????????u3?  1???????E    ?-#  h?'  1?1???????E    X?#  ?E??
  ?????????uh?
  j???????
  ?????^_????t&?$? Q  ???  ?E???????u??
  ??????h?$  ?????Y?E?|$1?1????   ??<  ???   s  ??"  ?Q
  ?????????uh`
  j??????Q
  ???????XZ??t3?$???   ?pU  ???????????u3?Q
  1???????E    ?)"  h?#  1?1???????E    X?"  ?E?*
  ?????????uh:
  j??????*
  ?????_??X??t%?E??h?T  ?`   ?????^??u?*
  ??????h?#  ?????[?$h?T  ?`   1?1????  ?  ?????Y??!  ?Q
  ?????????uh`
  j??????Q
  ???????XZ??t3?$???   ?pU  ???????????u3?Q
  1???????E    ?%!  h?#  1?1???????E    X?
!  ?E?*
  ?????????u"h:
  j??????*
  ?????Y_??????????E??h?T  ?O????D$?@??t???)  ?????X  ?????????uhh  j??????X  ???????XZ??t3?$???   ??P  ???????????u3?X  1???????E    ?K   h?'  1?1???????E    X?0   ?E?  ?????????uh?  j??????  ?????_??X??tc?E??P  ??????????
????  1?1????????  ?E?  ???????????   h?  j??????  ?????Y_??????   h (  1?1??????Y??  ?X  ?????????uhh  j??????X  ???????XZ??t7?$???  ??P  ????????????f????X  1???????E    ?#  h?'  1?1???????E    X?  ?E??P  ??????????????????D$?x?  ??  ?????????uh?  j???????  ???????XZ??tE?$???   ?D$???@?D$Hk??P  ?????????u9??  ?   ??????E    ?k  hP(  ?   1???????E    X?M  ?E??
  ?????????uh?
  j???????
  ?????^??X??t{?D$???@?H??D$?Ek???`P  ?????????   ??
  ???????  ?C??     ??
  ??????????S!  h?
  j???????
  ?????Y??X???1!  h?$  ?????Y?_  ?D$?@??t???&  ???????  ?????????uh  j???????  ???????XZ??t1?$???   ? P  ?????????u3??  1???????E    ?  h?(  1?1???????E    X??  ?E?  ?????????uh'  j??????  ?????^_????tf?$? P  ???  ?E?????????????  ??????m  ?E?  ???????????   h'  j??????  ?????_??X????   h?(  ?????Y?$  ??  ?????????uh  j???????  ???????XZ??t5?$???   ?8P  ??????????l?????  1???????E    ??  h?(  1?1???????E    X??  ?$?P  ???  ?E?????????????????<  ?????????uhJ  j??????<  ?????[^????t3?$???  ??R  ???????????u3?<  1???????E    ?T  h?&  1?1???????E    Y?9  ?E?_  ?????????uhm  j??????_  ???????XZ??t*?E???hR  ??????????????_  1????????  h?&  1?1??????X??  ?<  ?????????uhK  j??????<  ?????^_????t8?$?   ???   ??Q  ???????????u3?<  1???????E    ?d  h)  1?1???????E    Y?I  ?E?a  ?????????uhp  j??????a  ???????XZ??t&?E????Q  ?????????  ?a  ???????  hD)  ?????X??  ?D$??$?   ` i??$?   @   ?@?D$d    ??$?   T  ??$?       ??t???J  ?y  ??$?   1??   ??  ?????   ?????????$?   f??$?    ??$?   h?  j??????D$?????   ???????Y??X????  ?? ?????  ?GT????  ?@?????????u
???????  ??$?   ??????   ??  ?E?D$??,  ?E???D$d1????   ???????f??$?    ?D$d??$?   h?  j????????????????XZ???[  ?? ????O  ?CT???D  ?@???????u????????,  ?D$??4  ?4  ?T$@1??t$<?   ??$?   ???U?????   ?T$<????  ???   ?D$@!?D$D $? ?????f??$?   d ??$?   Sj??????D$???   ?????_??X????  ?? ?????  ?CT????  ?@???????u????????q  ?D$??,  ?E??tw??      ?*
  ?????????uh:
  j??????*
  ?????Y^????t/?$?`   ???  ?Eh<T  ?????Z??u?*
  ???????  h?#  ?????]??  1?1??  ??$?   1??   ??  ????$?   ` i???   ????$?   @   ??$?   T  ??$?       ?D$d    ???????$?   f??$?    ??$?   h?  j??????D$?????   ???????Y_?????F  = ????;  ?@T???0  ?@?????????u
??????  ??$?   ??????   ??  ?E?D$??,  ?E??$?   ???D$d1????   ???????f??$?    ?D$d??$?   h?  j????????????????XZ????  ?? ?????  ?CT????  ?@???????u????????p  ?D$1???4  1???  ?D$?@?D$??t???b  ???????  ?????????uh?  j???????  ?????^_????t5?$?L$ ???  ??  ?????????u3??  1???????E    ?  ht)  1?1???????E    Y??  ?t$L?E1??   ????$?   ?????   ?U?D$`    ???   ???T$X??  ?????f??$?   ` ??$?   Sj??????D$(?????????XZ???O  ?? ????C  ?CT???8  ?@???????u????????   ?E?|$1????  ???   ?E??  ??@  ?E??  `r  ??0  1??  ?T$-1??   ?D$,????$?   ?t$$??  ???D$ ???D$(?E?D$$1??L$?   ???   ?????????$?   f??$?   d Sj??????D$???   ?????Y_?????_  = ????T  ?@T???I  ?@???????u????????1  ?D$?T$4?L$????  ??,  ?D$?t$0???E???   ?D$9?D$0?D$???   ???????f??$?   ` ??$?   Sj??????D$(?????????XZ????  ?? ?????  ?CT????  ?@???????u?????????  ?D$1???0  1???  ?T$-1??   ?D$,????$?   ?t$$??  ???D$ ???D$(?E?D$$1??L$?   ???   ?????????$?   f??$?   d Sj??????D$???  ?????Y_??????  = ?????  ?@T????  ?@???????u?????????  ?D$?T$4?   ????  ??,  ?D$?t$0???E???   ?D$9?D$0?D$???   ???????f??$?   ` ??$?   Sj??????D$(?????????XZ???M  ?? ????A  ?CT???6  ?@???????u????????  ?D$1???0  1??=  ??  ?????????uh?  j???????  ?????^_????t5?$?L$ ???  ??  ?????????u3??  1???????E    ??  ht)  1?1???????E    Y??  ?t$L?E1??   ????$?   ?????   ?U?D$`    ???   ???T$X??  ?????f??$?   ` ??$?   Sj??????D$(?????????XZ???  ?? ????  ?CT???  ?@???????u?????????  ?E1????  ???   ?D$??0  1???  ?T$h1??   ?????D$f?D$h! ?D$d????@f?D$y???t??t/1???  ?$?L$x?D$l   ?D$p?>  ??|  ?D$`r  ??$?D$p w ?D$?q  ???   ??$?   1??   ??  ?????
   ???????f??$?   h ?D$d??$?   Sj??????F???????Y_??????  = ?????  ?@T????  ?@???????u?????????  ?D$??$?   ??,  ????????T$|?D$ ????????   ??  ?t$L?E1????????   ?E?D$`    ?D$X1????   ???????f??$?   ` ??$?   Sj??????D$(?????????XZ???4  ?? ????(  ?CT???  ?@???????u????????  ?E?|$1????  ???   ?E??  ??@  ?|$?E??  ?D$??0  1???  ?D$?D$,    ?@??t1???u?$?D$,???   ??$?D$,??|  ?D$ ?   ??  ??$?   ?D$(?E?D$$??1????   ???????f??$?   d ?D$$??$?   Sj??????F???????_??X???%  ?? ????  ?FT???  ?@???????u?????????  ?D$?T$4?   ????  ??,  1??t$0???E???   ?D$9?D$01????   ?????????$?   f??$?   ` Sj??????D$(???????Y^??????  = ????v  ?@T???k  ?@???????u????????S  ?D$1???0  1??r  ?D$?@?D$??t????  ???????  ?????????uh?  j???????  ???????XZ??t7?$?L$ ???   ??  ???????????u3??  1???????E    ??  ht)  1?1???????E    X??  ??$?   ?E?   ??  ?D$??1????   ???????f??$?    ?D$??$?   h?  j??????????????_??X???B  ?? ????6  ?FT???+  ?@???????u????????  ?D$???   ??  ??4  ?E?D$1????   ???????f??$?   ! ?D$??$?   Sh   j??????D$,???????????????  = ?????  ?@T????  ?@???????u????????~  ?D$1???0  ?E1????  ???   ??
  ?T$-1??   ?D$,????$?   ?\$$??  ???D$ ???D$(?E?D$$1??L$?   ???   ?????????$?   f??$?   d Vh   j??????D$???   ?????????????
  = ?????
  ?@T????
  ?@???????u?????????
  ?D$?T$4?L$????  ??,  ?D$???E???   ?D$9?D$0?D$???   ???????f??$?   ` ?D$0??$?   Vh   j??????D$???  ???????????t= ???w?@T??u%?CT?@?????????????D$??,      ??	  ?@???????u,????????CT?@?????????????D$??,      ??	  ?D$?7   1???0  ?$??$  ?}  ??$  ????1???  ?|$?wPh?  h?)  ??????D$?@?D$ ????t?????????  ?T$-1??   ?D$,????$?   ?\$$??  ???D$ ???D$(?E?D$$1??L$?   ???   ?????????$?   f??$?   d Vh   j??????D$???   ?????????????  = ?????  ?@T????  ?@???????u?????????  ?D$?T$4?   ????  ??,  ?D$???E???   ?D$9?D$0?D$???   ???????f??$?   ` ?D$0??$?   Vh   j??????D$???  ???????????t= ???w?@T??u%?CT?@?????????????D$??,      ??  ?@???????u,????????CT?@?????????????D$??,      ??  ?D$??0  ?$???  8?????h?  h?)  ??????D$?7   ??$  ?}  ??$  ????1?Y[1???  ?T$-1??   ?D$,????$?   ?\$$??  ???D$ ???D$(?E?D$$1??L$?   ???   ?????????$?   f??$?   f Vh   j??????D$???   ?????????????  = ?????  ?@T????  ?@???????u?????????  ?D$?T$4?L$????  ??,  ?D$???E???   ?D$9?D$0?D$???   ???????f??$?   b ?D$0??$?   Vh   j??????D$???  ???????????t= ???w?@T??u%?CT?@?????????????D$??,      ??  ?@???????u,????????CT?@?????????????D$??,      ??  ?D$1???0  1???  ?|$?wPh?  h?)  ??????D$???@??t?????????  ?  ?????????uh  j??????  ???????XZ??t1?$???   ?`Q  ?????????u3?  1???????E    ?D  h?)  1?1???????E    X?)  ?T$4?E?   1??\$0????$?   ?t$0?????   ??  ???   ??f?D$8?????f??$?   ` ??$?   Sh   j??????D$???  ???????????t= ???w?@T??u'?T   ?@?????1???????D$??,      ?I  ?@???????u.????????T   ?@?????1???????D$??,      ?  ?D$??0  ?$???  9?I???h?  h,*  ??????D$?7   ??$  ?}  1???$  ????1?_X??  ?  ?????????uh  j??????  ?????[^????t1?$???   ?@Q  ?????????u3?  1???????E    ??  h?)  1?1???????E    Y?r  ?T$4?E?   1??\$0????$?   ?t$0?????   ??  ???   ??f?D$8?????f??$?   b ??$?   Sh   j??????D$???  ???????????t= ???w?@T??u'?T   ?@?????1???????D$??,      ??  ?@???????u.????????T   ?@?????1???????D$??,      ?X  ?D$1???0  1??w  ?D$?x??????  ?????????uh  j??????  ???????XZ??t1?$???   ??O  ?????????u3?  1???????E    ?
  h?)  1?1???????E    X??   ?T$4?E?   1??\$0????$?   ?t$0?????   ??  ???   ??f?D$8?????f??$?   ` ??$?   Sh   j??????D$???  ????????????@  = ????5  ?@T???*  ?@???????u????????  ?D$?7   ??0  ?$??$  ?}  1???$  ????1???t$1?hX*  ?????^1?_?E??t
??t?{ u?t$h?*  ?????Y[??   ??      ??t?C??      ?$?@?D$?x??$?X?GL9?t?Gd?@??t???t????????L??L???E??  ??t?????V?    hV  j ?D$???   ?   P?L$?D$?????????u?D$?u??????l????D$??4  ??t ?CT?@?????????????D$??4      ?D$??0  ??t ?CT?@?????????????D$??0      ?D$??,  ??t ?CT?@?????????????D$??,      ?D$??      ?D$???????????  ?$? S  ?   ??$?   ?   ??????E????????$?D$h    ?D$l    ?  ?D$hd?D$d?  ?????????uh"  j??????  ?????[^????t?E?T$d???????u?  ??????!h?$  ?????Y????  ??t
??N  ?????1?1????????  ?????????uh?  j???????  ?????_??X??tW?Ej	j ??   ???????ZY?????????  ??????9??  ?????????u=h?  j???????  ?????Y_????uh`'  ?????Xh?  1?1??????X?%????Ej
???E???  ??????D$?   ????????????????  ?????????uh?  j???????  ???????XZ??t>?D$???@?D$Hk??P  ??????????????  ?   ??????C    ?????h?(  ?   ??????C    X?p????D$???@?H??D$?Ck???`P  ??????   ???B????J???1?1??4????E1?1???   o  ????i??  ?$?   h O  ?   ??$?   ???   ???????Z??yh?%  ??????????Y??   ??? O  ??$?   ??   ??????E???????????  ?@   ??$?   ???????4  ?]???????4   t}?   1?????$?   ?????????  ?????????uh?  j???????  ???????XZ??t(?E??O  ??????????K?????  ???????????h?%  ??????????_??$   d+    t???????  [^_]??????U?   WVS???{???0??  h?  h?*  ????????  ?D$??9?  ??   ???????????uh?  h?*  ??????????[^??   ?????????????   ?D$?4$h +  ??????T$???  ?B   ?B   ?BL   ?BP   ?Z<?BD?  ?B0   ?B4    ?BH    ?B(???   ?   ?B?B???????XY??x5E?>??????????????t"P?D$h?  hH+  ??????T$????????????[^_]????  ???  h?  hx+  ?????????y  ???  ???  hx  h?+  ????????z  ?v$h?  h?+  ?????1?????z  h?  h?  hY  ????????{  ??  ???????  ?????j?s?sh?  h,  ????????}  hd  hd  hY  ????????!}  ?t$h$  h?-  ????????w?  ?7Vh$  hl-  ????????L?  hD-  ?????X??????  ?4?Vh$  h-  ??????????  ?V???\vt?z  RPh?,  ?????????????  ?V???\vG?z  RPht,  ?????????????  ?B???\v5?z  PVh$  h$  hH,  ??????????  ???  ?????  ?????  ???t$`h8  h8  h?-  ??????????  ?t$hD  h?1  ??????????  hH1  ?????]?D$???????  Uh  h?0  ??????????  ht/  ??????D$?????[???  hD  hD  hY  ??????????  h? Rh?.  ??????D$??????????  h,  hH/  ??????D$^]??H?????  ?=?   ?z?  h,  h?/  ?????XZ?D$?z?  h?  h?  hY  ?????????  h?  h4.  ?????hp.  ??????????  h,  h.  ?????XZ???  hD  hD  h?-  ??????D$    ?D$    ???=?  ???  h,  h,  hY  ??????????  h,  h80  ?????]X???  h?/  ??????D$?????X???  h?/  ??????D$?????X???  h,  h?/  ?????XZ?E?  h  h  hY  ????????c?  h,  h.  ?????Y^?6?  h?0  ?????hD  h?0  ????????D$???????  h1  ?????X?D$???????  ?t$?t$hH  hH  h?1  ????????y?  h  h  hY  ??????????  ???  ??  ??t1RPh 2  ?????1????|?  ?????  P??h?1  ?????XZ???  ???   ??h?  h?  hY  ????????W?  h`  h`  hY  ????????4?  ?????????? V1?@S????????t?6   ??1????????????6   ????????v??[^???  ????  ?T$8D????D$$?  D??<$ ?D$(?  D????D$,?	  D??|$ ?D$0?  D????D$4?  D??L$??p?L$?t$$?t$,?t$4?t$<?t$DPQhL2  ???????  ?T$X?? ?=?  ???  ?|$ ?  ?T$,D???   ???  D??|$ ?D$$?  D????D$(?!  D?R?t$(?t$0P?t$ht2  ??????T$D?????  ?D$?D$<   ? ?D$??pPh?2  ?????XZ???  ?D$?L$<??pPh?2  ?????_X?|$ t
???????  ???????  ?D$?D$<   ??pPh?2  ?????X?????Z??  ?|$ tF?|$ tk?D$ h?  hR  ?XpRSh?2  ????????=?  ???  Sh%  ?????XZ???  ?|$ t8hQ  hR  R?D$,??pPh?2  ??????????  ?D$ h?  hK  ?Xp??hQ  hK  R?D$,??pPh?2  ??????????  ?o???????????  ?6   ??1????????6   ????????????v???? ???$?l$?L$?/????$?L$QRPShx4  ??????????}  h^  Shh7  ?????i??  ?6   1?????1?P?E??PSh?7  ????????[  ?D$?W  ??E?QSh?4  ??????^  ?f  ???D$E?RSh?4  ??????????D$E?RSh5  ??????^  ?f  ???D$E?RShD5  ??????????D$E?RSht5  ??????????D$E?PSh?5  ??????^  ?f  ???D$E?PSh?5  ?????Sh   ???????????   E?PSh3  ????????^  ?f  ???? E??$RSh6  ??????^  ?f  ?????   E?RSh46  ???????  ??  ????   D?PSh?3  ????????? ?  D?WShd6  ???????  ??  ?l$ ????   D??uPSh?6  ????????<$ t5??????71?????PSh?6  ??????D$$????PSh?6  ?????????????????p  ?6   ??1????????6   ????????????v???? ???????W?[  V?W  PSh,7  ??????l$ ??????   E?PSht5  ????????? D?VSh?5  ??????f  ?^  ???D$ D?PSh?5  ?????1???0[^_]??o1???????7????PSh?3  ???????????????ve?D$   ?$   ?D$?   +$???????????D$ ??1???7??1?P?L$$??????7??P?t$?t$Sh04  ????????4?????  ??   ?$?D$??1??4???hf  Shh7  ????????????1??????h?  h$8  ?????XZ?;?  h?  h 8  ?????YX??  h?  h 8  ?????XZ???  h?  h 8  ?????XZ?B?  h?  h 8  ?????XZ??  h?  h 8  ?????XZ???  ??PSh`  h`  hD8  ???????????!?  ???P?D$PShp  hp  h?8  ?????1??????  P?D$Php  hp  h?8  ?????????????  ?D$PP?C?j Ph  h  h9  ??????????  ?<$ ?NLuH??  ?|$??  ?\$E???  ?T$SP?t$8QVh?  h?  hd9  ??????T$$?? ???  ??  ?|$??  ??D???  ??P???j h`  h`  hD8  ??????????  P?FLPh4  h4  h?9  ?????????  Pjh`  h`  hD8  ??????????  hH  hH  h:  ??????   ???T?  hH  hH  h?9  ????????G?  hH  hH  h?  ????????5?  Wh$  h$  hx:  ??????????  h$  h$  h$:  ??????????  h$  h$  h?  ????????X?  h$  h$  hT:  ??????????  Ph?:  ?????X???Z???  Ph?:  ?????X???Z?E?  h?  h;  ?????X?   Z?z?  ???  h?  Ph?  ??????????  ???  Ph?;  ?????ZY???  h?;  ??????????Y?4?  ?E ?  Ph<  ?????XZ?4?  ??  Phh<  ?????^X???  Q?L$P??  Ph?<  ??????C?L$????  h?  h?<  ?????XZ??????_?  ?$U??  Ph?<  ??????T$???_?  ???  Ph(=  ????????   @  ???????d  ??  ???????Y_???  ?E ?  PhT=  ?????XZ??  h?  h?=  ?????^1?X???  ??  Ph?=  ?????Y[???  h?  1?h >  ?????XZ???  ?????h  hd   ??????  X?`J  Z?    ??????`J  ?????                            ?f3e?   viewcast_eeprom cx23885_tuner_callback  P   ?   enable_885_ir   netup_card_rev      cx23885_vdev_init   vidioc_s_fmt_vid_cap    vidioc_g_std    vidioc_s_std    vidioc_enum_input   U  d  s  cx23885_query_audinput  vidioc_g_audinput   vidioc_s_audinput   buffer_prepare  cx23885_start_video_dma buffer_queue    cx23885_video_register  cx23885_video_unregister    cx23885_video_irq   cx23885_set_freq_via_ops    cx23885_audio_mux   cx23885_flatiron_dump   cx23885_flatiron_mux    cx23885_video_mux   cx23885_set_input   cx23885_get_input   strnlen strscpy             v  ?  ?  ?  ?  ?  ?  ?  ?  ?  cx23885_enum_input  cx23885_flatiron_read   cx23885_set_tvnorm  cx23885_video_wakeup                                &  ?                           `                                                        $                                                      0#                                                                                                          ?+      ?1  ?)  ?2                                              ?'   (   0                      ?  P                                          ?  @  ?  @6          `                                                                                                                          ?                  !  ?        )  ?  ?      vid_limit   irq_debug   video_debug vbi_nr                 J  video_nr                   J                  cx23885_start_vbi_dma   buffer_queue    cx23885_vbi_irq         `=                  ??  `?      @  ?>  ?=      vbi_debug   vbibufs                             cx23885_dev_checkrevision   cx23885_reset   cx23885_pci_quirks  cx23885_init_tsport cx23885_dev_setup   cx23885_irq_ts  cx23885_irq cx23885_wakeup  cx23885_irq_417 do_cancel_buffers   cx23885_stop_dma    cx23885_cancel_buffers  cx23885_buf_queue   cx23885_buf_prepare cx23885_tsport_reg_dump cx23885_start_dma                                                                          "  )  .  3  6  9  <  ?  B  E  J  O  S      W  ]          b      i  n  s  x  ~  ?  ?          ?  ?  ?  ?  ?  ?  ?  ?  ?  ?  ?      $  cx23885_sram_channel_setup              ?  R?  ????????                ?  ??  ????????                                                "  Q  "  #  "  ?  "      #Z  card                  @N  debug   dma_reset_workaround                                cx23885_av_clk  strnlen strscpy i2c_sendbytes   i2c_readbytes   i2c_xfer    k ??cx23885_i2c_register                                                            *	  3	                  =	                                          F	                                                                                                                                                          N	  N	  U	  ]	  e	                              m	              v	                                                              }	  T"          ?	                                                                                                              ?k              ?g          i2c_scan    i2c_debug               stb6100_set_frequency   stb6100_get_frequency   stb6100_set_bandwidth   stb6100_get_bandwidth   dvb_register    cx23885_dvb_register    i   ???!  ?>    ?    h   ???!  '    ?   h   ???!  ?>    ?                   ?n                  pq  Pq       q  q  ?p      adapter_nr                V  alt_tuner       ?  ?  ?  z  z  z  z  z  z  ?  z  ?  z  ?  z  ?  z     z  z  z    z    z  /  z  @  z  T  z  i  z  {  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  ?  z  ?  z  ?  z  ?  z  z  z  z  z  ?  ?  ?  ?      '  :  R  i  {  z  z  ?  z  ?  ?  ?  ?  ?  ?  z      z    cx23885_codec_settings  cx23885_find_mailbox    ?  f?U?cx23885_load_firmware   cx23885_initialize_codec    cx23885_video_dev_alloc vidioc_g_fmt_vid_cap    vidioc_s_fmt_vid_cap    vidioc_try_fmt_vid_cap  vidioc_enum_input   strnlen strscpy vidioc_g_tuner  cx23885_417_register    cx23885_417_unregister  cx23885_mbox_func   cx23885_api_cmd cx23885_417_check_encoder   cx23885_mc417_init          ?{  0w                          ?y                                                      ?|                                                      py                                                                                                      @y   {      ?z  ?z  ?z                                                                                                                              @z  ?x  Px  0z          ?w                                                                                                                          ?v                  ?w  pw      0?  ??  Pw                      ?                                    (                 /  ?               6                 <                 B                 H                 O                 V    @             ^    2             v4l_debug   mpeglinesize    mpeglines   mpegbufs                                              f  o  o  o  x  o  o  o  ?  o  o  o  ?              ?                  ?          P?  ??   ?  ??  ?   ?          ??                                  P?          ir_888_debug    i2c_av_write    i2c_av_read i2c_av_write4   netup_read_ci_status    netup_ci_init   netup_ci_set_irq    netup_ci_slot_status    netup_write_i2c netup_read_i2c  netup_ci_op_cam ci_irq_enable   ci_dbg  f300_xfer               cx23885_alsa_dma_map    cx23885_start_audio_dma cx23885_audio_register                          `?  ??      `?  0?  ??  0?      ??                  ?          audio_debug disable_analog_audio     ?   ?   @  p     @  ?  @  ?  ?  ?  `  ?  @  ?  P  ?      `   ?   ?   !  0#   $  &  ?&  P'  ?'   (  `(   )  ?)  ?)   *  ?+  ?+  P,  ?,  ?-  ?/   0  ?0  ?1   2  02  ?2  ?2  @6  P6  @7  ?7  `=  ?=  ?>  `?  ??  @  ?@  PA  ?A  T  ?A  ?C   E  PE   F  ?F  ?G   H  PH  ?H  ?H  0I  ?	  ?K  `L  ?L  ?M  0N  O  PO  ?R  `S  `T  V  ?V  `\  ?\  `]  ?]  ?^      ?g  ?g  ?g  pi  ?k  ?l   n  0n  ?n   o  Po  ?o  ?o  ?o  p  @p  ?p  ?p  q   q  Pq  pq  ?q  `r  s  ?%  `(  ?s  u  xj   v  ?v  0w  Pw  pw  ?w  ?w  Px  ?x  @y  py  ?y  0z  @z  ?z  ?z  ?z   {  ?{  ?|  }  `}  ?~  ?  ??  ??  ??  p?  ??  0?   ?  `?  ??  0?  ??  ??  @?  ??  @?  P?  ??  ?  ??  P?  ??   ?  0?  `?  p?  ??  ?  ??  ?p  0?  ??  P?  P?   ?   ?  ??  P?  p?  ??  `?  ??  ??  ??  p?   ?  ??  ??   ?  @?  `?  ??  @?   ?  ??  ?  P?  ??  `?  ??  `?   ?  ??  ??  ?  0?  `?  `?  0?  `?  ??  ??  p?  ??     4cx23885: %s: warning: unknown hauppauge model #%d
    6cx23885: %s: hauppauge eeprom: model=%d
  6cx23885: %s: Board has no valid PCIe Subsystem ID and can't
%s: be autodetected. Pass card=<n> insmod option
%s: to workaround that. Redirect complaints to the
%s: vendor of the TV card.  Best regards,
%s:         -- tux
 6cx23885: %s: Your board isn't known (yet) to the driver.
%s: Try to pick one of the existing card configs via
%s: card=<n> insmod option.  Updating to the latest
%s: version might help as well.
    6cx23885: %s: Here is a list of valid choices for the card=<n> insmod option:
 6cx23885: %s:    card=%d -> %s
    3cx23885: %s(): Unknown command 0x%x.
 6cx23885: %s() eeprom records are undefined, no serial number
 6cx23885: %s: card '%s' sn# MM%d
  6cx23885: NetUP card rev=0x%x fw_filename=%s
  3cx23885: did not find the firmware file '%s'. You can use <kernel_dir>/scripts/get_dvb_firmware to get the firmware.  DViCO FusionHDTV7 Dual Express  DViCO FusionHDTV DVB-T Dual Express Leadtek Winfast PxDVR3200 H XC4000  Hauppauge WinTV-HVR4400/HVR5500 AVerTV Hybrid Express Slim HC81R    DViCO FusionHDTV DVB-T Dual Express2    Technotrend TT-budget CT2-4500 CI   Hauppauge WinTV-HVR-1265(161111)    Hauppauge WinTV-QuadHD-DVB(885) Hauppauge WinTV-QuadHD-ATSC(885)    7cx23885: %s: video:[%p/%d] buffer_queue - first active
   7cx23885: %s: video:[%p/%d] buffer_queue - append to active
   7cx23885: %s: video:%s() Creating TFF/NTSC risc
   7cx23885: %s: video:%s() Creating BFF/PAL/SECAM risc
  drivers/media/pci/cx23885/cx23885-video.c   7cx23885: %s: video:[%p/%d] buffer_init - %dx%d %dbpp 0x%08x - dma=0x%08lx
    7cx23885: %s: video:%s() width=%d height=%d field=%d
  7cx23885: %s: video:[%p/%d] wakeup reg=%d buf=%d
  7cx23885: %s: video:%s(input=%d)
  7cx23885: %s: video:%s() returns %d
   7cx23885: %s: video:%s(norm = 0x%08x) name: [%s]
  7cx23885: %s: video:%s(input = %d)
    7cx23885: %s: video:Flatiron dump
 7cx23885: %s: video:FI[%02x] = %02x
   7cx23885: %s: video:%s() video_mux: %d [vmux=%d, gpio=0x%x,0x%x,0x%x,0x%x]
    7cx23885: %s: video:%s() -EINVAL
  7cx23885: %s: video:%s() frequency=%d tuner=%d std=0x%llx
 3cx23885: %s() No analog tuner, aborting
  7cx23885: %s: video: (VID_BC_MSK_OPC_ERR 0x%08x)
  4cx23885: %s: video risc op code error
    7cx23885: %s: video: (VID_BC_MSK_SYNC 0x%08x) video lines miss-match
  7cx23885: %s: video: (VID_BC_MSK_OF 0x%08x) fifo overflow
 6cx23885: %s: can't register video device
 6cx23885: %s: registered device %s [v4l2]
 6cx23885: %s: can't register vbi device
   6cx23885: %s: registered device %s
    7cx23885: %s: vbi:[%p/%d] vbi_queue - first active
    7cx23885: %s: vbi:[%p/%d] buffer_queue - append to active
 7cx23885: %s: vbi:%s() VID_BC_MSK_VBI_RISCI1
  dma in progress detected 0x%08x 0x%08x, clearing
   7cx23885: %s: [%p/%d] wakeup reg=%d buf=%d
    7cx23885: %s: %s() Erasing channel [%s]
   7cx23885: %s: %s() Configuring channel [%s]
   drivers/media/pci/cx23885/cx23885-core.c    7cx23885: %s: %s() 0x%08x <- 0x%08x
   7cx23885: %s: [bridge %d] sram setup %s: bpl=%d lines=%d
  4cx23885: %s: %s - dma channel status dump
    4cx23885: %s:   cmds: %-15s: 0x%08x
   4cx23885: %s:   (0x%08x) iq %x:    4cx23885: %s:   iq %x: 0x%08x [ arg #%d ]
 4cx23885: %s: fifo: 0x%08x -> 0x%x
    4cx23885: %s: ctrl: 0x%08x -> 0x%x
    4cx23885: %s:   ptr1_reg: 0x%08x
  4cx23885: %s:   ptr2_reg: 0x%08x
  4cx23885: %s:   cnt1_reg: 0x%08x
  4cx23885: %s:   cnt2_reg: 0x%08x
  7cx23885: %s:  (VID_BC_MSK_OPC_ERR 0x%08x)
    7cx23885: %s:  (VID_BC_MSK_BAD_PKT 0x%08x)
    7cx23885: %s:  (VID_BC_MSK_SYNC    0x%08x)
    7cx23885: %s:  (VID_BC_MSK_OF      0x%08x)
    3cx23885: %s: mpeg risc op code error
 7cx23885: %s:  (RISCI1            0x%08x)
 7cx23885: %s: %s() w: %d, h: %d, f: %d
    6cx23885: %s: risc disasm: %p [dma=0x%08lx]
   6cx23885: %s:   %04d: 0x%08x [ arg #%d ]
  3cx23885: %s() Unsupported .portb/c (0x%08x)/(0x%08x)
 7cx23885: %s: %s() enabling TS int's and DMA
  7cx23885: %s: %s() DEV_CNTRL2               0x%08X
    7cx23885: %s: %s() PCI_INT_MSK              0x%08X
    7cx23885: %s: %s() AUD_INT_INT_MSK          0x%08X
    7cx23885: %s: %s() AUD_INT_DMA_CTL          0x%08X
    7cx23885: %s: %s() AUD_EXT_INT_MSK          0x%08X
    7cx23885: %s: %s() AUD_EXT_DMA_CTL          0x%08X
    7cx23885: %s: %s() PAD_CTRL                 0x%08X
    7cx23885: %s: %s() ALT_PIN_OUT_SEL          0x%08X
    7cx23885: %s: %s() GPIO2                    0x%08X
    7cx23885: %s: %s() gpcnt(0x%08X)          0x%08X
  7cx23885: %s: %s() gpcnt_ctl(0x%08X)      0x%08x
  7cx23885: %s: %s() dma_ctl(0x%08X)        0x%08x
  7cx23885: %s: %s() src_sel(0x%08X)        0x%08x
  7cx23885: %s: %s() lngth(0x%08X)          0x%08x
  7cx23885: %s: %s() hw_sop_ctrl(0x%08X)    0x%08x
  7cx23885: %s: %s() gen_ctrl(0x%08X)       0x%08x
  7cx23885: %s: %s() bd_pkt_status(0x%08X)  0x%08x
  7cx23885: %s: %s() sop_status(0x%08X)     0x%08x
  7cx23885: %s: %s() fifo_ovfl_stat(0x%08X) 0x%08x
  7cx23885: %s: %s() vld_misc(0x%08X)       0x%08x
  7cx23885: %s: %s() ts_clk_en(0x%08X)      0x%08x
  7cx23885: %s: %s() ts_int_msk(0x%08X)     0x%08x
  7cx23885: %s: %s() ts_int_status(0x%08X)  0x%08x
  7cx23885: %s: %s() PCI_INT_STAT           0x%08X
  7cx23885: %s: %s() VID_B_INT_MSTAT        0x%08X
  7cx23885: %s: %s() VID_B_INT_SSTAT        0x%08X
  7cx23885: %s: %s() VID_C_INT_MSTAT        0x%08X
  7cx23885: %s: %s() VID_C_INT_SSTAT        0x%08X
  7cx23885: %s: %s() Register Dump
  7cx23885: %s: [%p/%d] %s - first active
   7cx23885: %s: [%p/%d] %s - append to active
   7cx23885: %s: [%p/%d] %s - dma=0x%08lx
    7cx23885: %s: status: 0x%08x  mask: 0x%08x count: 0x%x
    3cx23885: %s: V4L mpeg risc op code error, status = 0x%x
  7cx23885: %s:         VID_B_MSK_BAD_PKT
   7cx23885: %s:         VID_B_MSK_OPC_ERR
   7cx23885: %s:         VID_B_MSK_VBI_OPC_ERR
   7cx23885: %s:         VID_B_MSK_SYNC
  7cx23885: %s:         VID_B_MSK_VBI_SYNC
  7cx23885: %s:         VID_B_MSK_OF
    7cx23885: %s:         VID_B_MSK_VBI_OF
    7cx23885: %s:         VID_B_MSK_RISCI1
    7cx23885: %s: pci_status: 0x%08x  pci_mask: 0x%08x
    7cx23885: %s: vida_status: 0x%08x vida_mask: 0x%08x count: 0x%x
   7cx23885: %s: audint_status: 0x%08x audint_mask: 0x%08x count: 0x%x
   7cx23885: %s: ts1_status: 0x%08x  ts1_mask: 0x%08x count: 0x%x
    7cx23885: %s: ts2_status: 0x%08x  ts2_mask: 0x%08x count: 0x%x
    7cx23885: %s:  (PCI_MSK_RISC_RD   0x%08x)
 7cx23885: %s:  (PCI_MSK_RISC_WR   0x%08x)
 7cx23885: %s:  (PCI_MSK_AL_RD     0x%08x)
 7cx23885: %s:  (PCI_MSK_AL_WR     0x%08x)
 7cx23885: %s:  (PCI_MSK_APB_DMA   0x%08x)
 7cx23885: %s:  (PCI_MSK_VID_C     0x%08x)
 7cx23885: %s:  (PCI_MSK_VID_B     0x%08x)
 7cx23885: %s:  (PCI_MSK_VID_A     0x%08x)
 7cx23885: %s:  (PCI_MSK_AUD_INT   0x%08x)
 7cx23885: %s:  (PCI_MSK_AUD_EXT   0x%08x)
 7cx23885: %s:  (PCI_MSK_GPIO0     0x%08x)
 7cx23885: %s:  (PCI_MSK_GPIO1     0x%08x)
 7cx23885: %s:  (PCI_MSK_AV_CORE   0x%08x)
 7cx23885: %s:  (PCI_MSK_IR        0x%08x)
 3cx23885: %s: Setting GPIO on encoder ports
   3cx23885: %s: Clearing GPIO moving on encoder ports
   3cx23885: %s: Reading GPIO moving on encoder ports
    3cx23885: %s: Enabling GPIO on encoder ports
  7cx23885: %s: %s() Memory configured for PCIe bridge type %d
  3cx23885: %s: can't get MMIO memory @ 0x%llx
  3cx23885: CORE %s No more PCIe resources for subsystem: %04x:%04x
 6cx23885: CORE %s: subsystem: %04x:%04x, board: %s [card=%d,%s]
   7cx23885: %s: %s() tuner_type = 0x%x tuner_addr = 0x%x tuner_bus = %d
 7cx23885: %s: %s() radio_type = 0x%x radio_addr = 0x%x
    3cx23885: %s() Failed to register analog video adapters on VID_A
  3cx23885: %s() Failed to register dvb adapters on VID_B
   3cx23885: %s() Failed to register 417 on VID_B
    3cx23885: %s() Failed to register dvb on VID_C
    3cx23885: %s() Failed to register 417 on VID_C
    3cx23885: %s() New hardware revision found 0x%x
   6cx23885: %s() Hardware revision = 0x%02x
 3cx23885: %s() Hardware revision unknown 0x%x
 6cx23885: %s/0: found at %s, rev: %d, irq: %d, latency: %d, mmio: 0x%llx
  3cx23885: %s/0: Oops: no 32bit PCI DMA ???
    3cx23885: %s: can't get IRQ %d
    6cx23885: cx23885 driver version %s loaded
    7cx23885: %s: i2c:%s(msg->len=%d)
 7cx23885: %s: i2c:%s() returns 0
  7cx23885: %s: i2c:%s(msg->wlen=%d, nextmsg->rlen=%d)
  7cx23885: %s: i2c:%s(num = %d)
    7cx23885: %s: i2c:%s(num = %d) addr = 0x%02x  len = 0x%x
  7cx23885: %s: i2c:%s(bus = %d)
    7cx23885: %s: i2c:%s: i2c bus %d registered
   6cx23885: %s: i2c scan: found device @ 0x%04x  [%s]
   4cx23885: %s: i2c bus %d register FAILED
  7cx23885: %s: i2c:%s(enabled = %d)
    tuner/mt2131/tda8275/xc5000/xc3028  6cx23885: NetUP Dual DVB-S2 CI card port%d MAC=%pM
    6cx23885: TeVii S470 MAC= %pM
 6cx23885: %s port %d MAC address: %pM
 6cx23885: %s MAC address: %pM
 3DVB: Unable to find symbol s5h1409_attach()
  3DVB: Unable to find symbol mt2131_attach()
   3DVB: Unable to find symbol lgdt3305_attach()
 3DVB: Unable to find symbol tda18271_attach()
 3DVB: Unable to find symbol s5h1411_attach()
  3DVB: Unable to find symbol tda829x_attach()
  3DVB: Unable to find symbol lgdt330x_attach()
 3DVB: Unable to find symbol simple_tuner_attach()
 3DVB: Unable to find symbol xc5000_attach()
   3DVB: Unable to find symbol xc2028_attach()
   3DVB: Unable to find symbol tda10048_attach()
 3DVB: Unable to find symbol dib7000p_attach()
 3DVB: Unable to find symbol zl10353_attach()
  4cx23885: Unable to enumerate dib7000p
    3DVB: Unable to find symbol dib0070_attach()
  3DVB: Unable to find symbol xc4000_attach()
   3cx23885: %s/2: xc4000 attach failed
  3DVB: Unable to find symbol cx24116_attach()
  3DVB: Unable to find symbol cx24117_attach()
  3DVB: Unable to find symbol ds3000_attach()
   3DVB: Unable to find symbol ts2020_attach()
   3DVB: Unable to find symbol stv0900_attach()
  3DVB: Unable to find symbol stv6110_attach()
  3DVB: Unable to find symbol lnbh24_attach()
   3DVB: Unable to find symbol lgs8gxx_attach()
  3DVB: Unable to find symbol mb86a20s_attach()
 3DVB: Unable to find symbol atbm8830_attach()
 3DVB: Unable to find symbol max2165_attach()
  3DVB: Unable to find symbol stv0367ter_attach()
   3DVB: Unable to find symbol stv0367cab_attach()
   3DVB: Unable to find symbol drxk_attach()
 3DVB: Unable to find symbol mt2063_attach()
   3DVB: Unable to find symbol stv090x_attach()
  3DVB: Unable to find symbol stb6100_attach()
  3DVB: Unable to find symbol m88ds3103_attach()
    6cx23885: %s(): board=%d port=%d
  6cx23885: %s(): QUADHD_DVB analog setup
   3DVB: Unable to find symbol lgdt3306a_attach()
    6cx23885: %s(): QUADHD_ATSC analog setup
  6cx23885: %s: The frontend of your DVB/ATSC card  isn't supported yet
 3cx23885: %s: frontend initialization failed
  6cx23885: %s() allocating %d frontend(s)
  3cx23885: %s() failed to alloc
    6cx23885: %s: cx23885 based dvb card
  3cx23885: %s() dvb_register failed err = %d
   7cx23885: %s: 417:VIDIOC_TRY_FMT: w: %d, h: %d
    7cx23885: %s: 417:VIDIOC_G_FMT: w: %d, h: %d
  7cx23885: %s: 417:VIDIOC_G_TUNER: tuner type %d
   7cx23885: %s: 417:VIDIOC_S_FMT: w: %d, h: %d, f: %d
   7cx23885: %s: 417:%s: command(0x%X) = %s
  3cx23885: Firmware and/or mailbox pointer not initialized or corrupted, signature = 0x%x, cmd = %s
    3cx23885: ERROR: Mailbox appears to be in use (%x), cmd = %s
  7cx23885: %s: 417:API Input %d = %d
   3cx23885: ERROR: API Mailbox timeout
  7cx23885: %s: 417:API Output %d = %d
  7cx23885: %s: 417:API result = %d
 7cx23885: %s: 417:%s() cmds = 0x%08x
  7cx23885: %s: 417:%s() PING OK
    3cx23885: %s: Error with mc417_register_write
 3cx23885: ERROR: Hotplug firmware request failed (%s).
    3cx23885: Please fix your hotplug setup, the board will not work without firmware loaded!
 3cx23885: ERROR: Firmware size mismatch (have %zu, expected %d)
   3cx23885: ERROR: Firmware magic mismatch, wrong file?
 7cx23885: %s: 417:Loading firmware ...
    3cx23885: ERROR: Loading firmware failed!
 7cx23885: %s: 417:Verifying firmware ...
  3cx23885: ERROR: Reading firmware failed!
 3cx23885: ERROR: Firmware load failed (checksum mismatch).
    7cx23885: %s: 417:Firmware upload successful.
 3cx23885: %s() f/w load failed
    7cx23885: %s: 417:Mailbox signature found at 0x%x
 3cx23885: Mailbox signature values not found!
 3cx23885: %s() mailbox < 0, error
 3cx23885: ERROR: cx23417 firmware ping failed!
    3cx23885: ERROR: cx23417 firmware get encoder :version failed!
    7cx23885: %s: 417:cx23417 firmware version is 0x%08x
  7cx23885: %s: 417:%s() status = %d, seq = %d
  6cx23885: %s: can't register mpeg device
  6cx23885: %s: registered device %s [mpeg]
 7%s: IRQ Status:  %s %s %s %s %s %s
   7%s: IRQ Enables: %s %s %s %s
 3%s: IR receiver software FIFO overrun
    3%s: IR receiver hardware FIFO overrun
    7%s: rx read: %10u ns  %s  %s
 6%s: 	Enabled:                           %s
   6%s: 	Demodulation from a carrier:       %s
   6%s: 	FIFO data on pulse timer overflow: %s
   6%s: 	FIFO interrupt watermark:          %s
   6%s: 	Loopback mode:                     %s
   6%s: 	Expected carrier (16 clocks):      %u Hz
    6%s: 	Next carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz
  6%s: 	Max measurable pulse width:        %u us, %llu ns
   6%s: 	Pulse width timer timed-out:       %s
   6%s: 	Pulse width timer time-out intr:   %s
   6%s: 	FIFO overrun:                      %s
   6%s: 	FIFO overrun interrupt:            %s
   6%s: 	Busy:                              %s
   6%s: 	FIFO service requested:            %s
   6%s: 	FIFO service request interrupt:    %s
   6%s: 	Modulation onto a carrier:         %s
   6%s: 	FIFO:                              %s
   6%s: 	Output pin level inversion         %s
   6%s: 	Carrier polarity:                  %s
   6%s: 	Carrier (16 clocks):               %u Hz
    6%s: 	Carrier duty cycle:                %2u/16
   6%s: 	Max pulse width:                   %u us, %llu ns
   6%s: 	Low pass filter:                   %s
   6%s: 	Min acceptable pulse width (LPF):  %u us, %u ns
 6%s: 	Pulse timers' start/stop trigger:  %s
   3cx23885: %s: i2c write error!
    3cx23885: %s: i2c read error!
 7cx23885: %s: %s: i2c write error, Reg=[0x%02x], Status=%d
    7cx23885: %s: %s: i2c read error, Reg = 0x%02x, Status = %d
   7cx23885: %s: %s: i2c read Addr=0x%04x, Reg = 0x%02x, data = %02x
 7cx23885: %s: %s: Slot Status Addr=[0x%04x], Reg=[0x%02x], data=%02x, TS config = %02x
    7cx23885: %s: %s: %s: chipaddr=[0x%x] addr=[0x%02x], %s=%x
    7cx23885: %s: %s: chipaddr=[0x%x] setting ci IRQ to [0x%x] 
   7cx23885: %s: %s: Wakeup CI0
  7cx23885: %s: %s: Wakeup CI1
  7cx23885: %s: %s: Unable create CI structure!
 7cx23885: %s: %s: CI initialized!
 7cx23885: %s: %s: Cannot initialize CI: Error %d.
 3cx23885: eeprom i2c read error, status=%d
    3cx23885: eeprom i2c write error, status=%d
   3cx23885: %s: timeout, the slave no response
  7cx23885: %s: alsa: Start audio DMA, %d B/line, %d lines/FIFO, %d periods, %d byte buffer
 7cx23885: %s: alsa: Stopping audio DMA
    3cx23885: BUG: cx23885 can't find device struct. Can't proceed with open
  7cx23885: %s: alsa: Error opening PCM!
    drivers/media/pci/cx23885/cx23885-alsa.c    7cx23885: %s: alsa: Freeing buffer
    7cx23885: %s: alsa: vmalloc_32(%lu pages) failed
  7cx23885: %s: alsa: vmalloc is at addr %p, size=%lu
   4cx23885: %s: cx23885_alsa_map_sg failed
  4cx23885: %s/1: Audio risc op code error
  7cx23885: %s: alsa: Downstream sync error
 4cx23885: %s(): Missing SRAM channel configuration for analog TV Audio
    7cx23885: %s: alsa: registered ALSA audio device
  3cx23885: %s(): Failed to register analog audio adapter
 ir-kbd-i2c dvb-netup-altera-04.fw dvb-netup-altera-01.fw cx25840 cs3308 configure UNKNOWN/GENERIC Hauppauge WinTV-HVR1800lp Hauppauge WinTV-HVR1800 Hauppauge WinTV-HVR1250 DViCO FusionHDTV5 Express Hauppauge WinTV-HVR1500Q Hauppauge WinTV-HVR1500 Hauppauge WinTV-HVR1200 Hauppauge WinTV-HVR1700 Hauppauge WinTV-HVR1400 Leadtek Winfast PxDVR3200 H Compro VideoMate E650F TurboSight TBS 6920 TeVii S470 DVBWorld DVB-S2 2005 NetUP Dual DVB-S2 CI Hauppauge WinTV-HVR1270 Hauppauge WinTV-HVR1275 Hauppauge WinTV-HVR1255 Hauppauge WinTV-HVR1210 Mygica X8506 DMB-TH Magic-Pro ProHDTV Extreme 2 Hauppauge WinTV-HVR1850 Compro VideoMate E800 Hauppauge WinTV-HVR1290 Mygica X8558 PRO DMB-TH LEADTEK WinFast PxTV1200 GoTView X5 3D Hybrid NetUP Dual DVB-T/C-CI RF MPX-885 Mygica X8502/X8507 ISDB-T TerraTec Cinergy T PCIe Dual TeVii S471 Prof Revolution DVB-S2 8000 TurboSight TBS 6981 TurboSight TBS 6980 Leadtek Winfast PxPVR2200 Hauppauge ImpactVCB-e DVBSky T9580 DVBSky T980C DVBSky S950C DVBSky S950 DVBSky S952 DVBSky T982 Hauppauge WinTV-HVR5525 Hauppauge WinTV Starburst ViewCast 260e ViewCast 460e Hauppauge WinTV-QuadHD-DVB Hauppauge WinTV-QuadHD-ATSC Hauppauge WinTV-Starburst2 AVerMedia CE310B Television 7cx23885: %s: video:%s()
 cx23885 PCIe:%s %s (%s) 3cx23885: %s() error
 7cx23885: %s: video:%s(%d)
 cx23885-vbi tuner xc3028-v27.fw xc3028L-v36.fw video vbi Baseband L/R 1 Baseband L/R 2 TV Composite1 Composite2 Composite3 Composite4 S-Video Component Cable TV DVB for debug only 7cx23885: %s: vbi:%s()
 INVALID 70x%08x [ %s c %s c count=%d ]
 4cx23885: %s:   risc%d:  6cx23885: %s:   %04d:  7cx23885: %s: %s: %p
 7cx23885: %s: %s()
 cancel 6cx23885: %s: Unsupported
 insmod option autodetected &dev->lock &dev->gpio_lock cx23885[%d] 7cx23885: %s: %s(portno=%d)
 &port->frontends.lock 0.0.4 cx23885 12 13 14 resync cnt0 cnt1 18 19 20 21 22 23 irq1 irq2 eol sol write skip writec jump sync read readc writerm writecm writecr init risc lo init risc hi cdt base cdt size iq base iq size risc pc lo risc pc hi iq wr ptr iq rd ptr cdt current pci target lo pci target hi line / byte VID A VID A (VBI) TS1 B ch4 ch5 TS2 C TV Audio ch8 ch9 ch2 7cx23885: %s: i2c: R 7cx23885: %s: i2c: <R %02x 7cx23885: %s: i2c: %02x 7cx23885: %s: i2c: >
 3cx23885:  ERR: %d
 7 <W %02x %02x c >
 c %02x ??? 6cx23885: %s: scan bus %d:
 ir_video tda10048 dib7000pc lgdt3303 cx24227 cs3308 tda8295 tda9887 cx25837 flatiron eeprom tuner/mt2131/tda8275 tuner/xc3028L %s: Invalid parameter
 sp2 s5h1409_attach symbol:s5h1409_attach mt2131_attach symbol:mt2131_attach lgdt3305_attach symbol:lgdt3305_attach tda18271_attach symbol:tda18271_attach s5h1411_attach symbol:s5h1411_attach tda829x_attach symbol:tda829x_attach lgdt330x_attach symbol:lgdt330x_attach simple_tuner_attach symbol:simple_tuner_attach xc5000_attach symbol:xc5000_attach xc2028_attach symbol:xc2028_attach tda10048_attach symbol:tda10048_attach dib7000p_attach symbol:dib7000p_attach zl10353_attach symbol:zl10353_attach dib0070_attach symbol:dib0070_attach xc4000_attach symbol:xc4000_attach cx24116_attach symbol:cx24116_attach cx24117_attach symbol:cx24117_attach ds3000_attach symbol:ds3000_attach ts2020_attach symbol:ts2020_attach stv0900_attach symbol:stv0900_attach stv6110_attach symbol:stv6110_attach symbol:lnbh24_attach lnbh24_attach 3cx23885: No LNBH24 found!
 lgs8gxx_attach symbol:lgs8gxx_attach mb86a20s_attach symbol:mb86a20s_attach atbm8830_attach symbol:atbm8830_attach max2165_attach symbol:max2165_attach stv0367ter_attach symbol:stv0367ter_attach stv0367cab_attach symbol:stv0367cab_attach drxk_attach symbol:drxk_attach mt2063_attach symbol:mt2063_attach stv090x_attach symbol:stv090x_attach stb6100_attach symbol:stb6100_attach tda10071_cx24118 tda10071 a8293 si2165 m88ds3103_attach symbol:m88ds3103_attach ts2020 si2168 si2157 m88ds3103 m88rs6000t %s lgdt3306a_attach symbol:lgdt3306a_attach xc3028-v27.fw xc3028L-v36.fw %s/2 Television 7cx23885: %s: 417:%s()
 PCIe:%s UNKNOWN v4l-cx23885-enc.fw NTSC-M mpeg %s (%s) PING_FW START_CAPTURE STOP_CAPTURE SET_AUDIO_ID SET_VIDEO_ID SET_PCR_ID SET_FRAME_RATE SET_FRAME_SIZE SET_BIT_RATE SET_GOP_PROPERTIES SET_ASPECT_RATIO SET_DNR_FILTER_MODE SET_DNR_FILTER_PROPS SET_CORING_LEVELS SET_SPATIAL_FILTER_TYPE SET_VBI_LINE SET_STREAM_TYPE SET_OUTPUT_PORT SET_AUDIO_PROPERTIES HALT_FW GET_VERSION SET_GOP_CLOSURE GET_SEQ_END SET_PGM_INDEX_INFO SET_VBI_CONFIG SET_DMA_BLOCK_SIZE GET_PREV_DMA_INFO_MB_10 GET_PREV_DMA_INFO_MB_9 SCHED_DMA_TO_HOST INITIALIZE_INPUT SET_FRAME_DROP_RATE PAUSE_ENCODER REFRESH_INPUT SET_COPYRIGHT SET_EVENT_NOTIFICATION SET_NUM_VSYNC_LINES SET_PLACEHOLDER MUTE_VIDEO MUTE_AUDIO MISC NTSC-JP PAL-BG PAL-DK PAL-I PAL-M PAL-N PAL-Nc PAL-60 SECAM-L SECAM-DK rc-hauppauge rc-nec-terratec-cinergy-xs rc-tevii-nec rc-total-media-in-hand-02 rc-tbs-nec rc-dvbsky rc-tt-1500 cx23885 IR (%s) pci-%s/ir0 cx23885 rby     tby ror rto rsr tsr roe rte rse tse 7%s: rx read: end of rx
 (timed out) space  mark yes no enabled disabled not loaded overflow marker not empty half full or greater loopback active normal receive half full or less space:burst mark:noburst space:noburst mark:burst 6%s: IR Receiver:
 6%s: IR Transmitter:
 %s/888-ir &state->rx_params_lock &state->tx_params_lock ??? edge falling edge rising edge rising & falling edges ctl read mem write 7cx23885: %s: %s:
 7cx23885: %s: %s
 7cx23885: %s: alsa: %s()
 include/linux/scatterlist.h CX23885 Digital CX23885 Conexant CX23885 %s at %s parm=enable_885_ir:Enable integrated IR controller for supported
		    CX2388[57] boards that are wired for it:
			HVR-1250 (reported safe)
			TerraTec Cinergy T PCIe Dual (not well tested, appears to be safe)
			TeVii S470 (reported unsafe)
		    This can cause an interrupt storm with some cards.
		    Default: 0 [Disabled] parmtype=enable_885_ir:int parm=netup_card_rev:NetUP Dual DVB-T/C CI card revision parmtype=netup_card_rev:int parm=vid_limit:capture memory limit in megabytes parmtype=vid_limit:int parm=irq_debug:enable debug messages [IRQ handler] parmtype=irq_debug:int parm=video_debug:enable debug messages [video] parmtype=video_debug:int parm=vbi_nr:vbi device numbers parm=video_nr:video device numbers parmtype=vbi_nr:array of int parmtype=video_nr:array of int license=GPL author=Steven Toth <stoth@linuxtv.org> description=v4l2 driver module for cx23885 based TV cards parm=vbi_debug:enable debug messages [vbi] parmtype=vbi_debug:int parm=vbibufs:number of vbi buffers, range 2-32 parmtype=vbibufs:int parm=card:card type parmtype=card:array of int parm=debug:enable debug messages parmtype=debug:int parm=dma_reset_workaround:periodic RiSC dma engine reset; 0-force disable, 1-driver detect (default), 2-force enable parmtype=dma_reset_workaround:int version=0.0.4 license=GPL author=Steven Toth <stoth@linuxtv.org> description=Driver for cx23885 based TV cards parm=i2c_scan:scan i2c bus at insmod time parmtype=i2c_scan:int parm=i2c_debug:enable debug messages [i2c] parmtype=i2c_debug:int parm=adapter_nr:DVB adapter numbers parmtype=adapter_nr:array of short parm=alt_tuner:Enable alternate tuner configuration parmtype=alt_tuner:int firmware=v4l-cx23885-enc.fw parm=v4l_debug:enable V4L debug messages parmtype=v4l_debug:int parm=mpeglinesize:number of bytes in each line of an MPEG buffer, range 512-1024 parmtype=mpeglinesize:int parm=mpeglines:number of lines in an MPEG buffer, range 2-32 parmtype=mpeglines:int parm=mpegbufs:number of mpeg buffers, range 2-32 parmtype=mpegbufs:int parm=ir_888_debug:enable debug messages [CX23888 IR controller] parmtype=ir_888_debug:int parm=ci_irq_enable:Enable IRQ from CAM parmtype=ci_irq_enable:int parm=ci_dbg:Enable CI debugging parmtype=ci_dbg:int parm=audio_debug:enable debug messages [analog audio] parmtype=audio_debug:int parm=disable_analog_audio:disable analog audio ALSA driver parmtype=disable_analog_audio:int    <           ??    L           ?? ?F  0          ?? ?I  <          ?? t  H          ?? x  T          $? \  p          $? |            ?? |            ?? @J  (	          $? 0	  D	          ?? ?  L	          ?? `N            $? ?  (          ?? ?  P          $? \  p          ?? ?  ?          ?? ?  ?          ?? tY  ?          ?? xY  ?          ?? |Y  ?          ?? ?  ?          ?? ?  ?          ?? ?  `          ?? ?  l          ?? ?  ?C  ?`  ??  ??  ??  ?  E?  ]?  j?  w?  ??  ??  ??                             p  4    p  v   p  x   p x   p 	x   p y   ? ?   p ?w   p ?w   p w   p w   p ?q   p ?q   p ?   p ?	   ??
   ?x?   }?f   }!o*   }9o   [ ?    i??   ?i??)   ?i??(   p?"?        U,*   p "   p "   p "   p Q"   p Y"$   p ?"   p ?"   p ?"   p ?"   p ?"   p ?"   p ?"   p ?"   p ?"   p ?"   ?Q?   ?W?   p A?   X ?   p Q?   ?x?   }"o   TV?#   U??   ??!   ;~"   q?"?#    ?40%   p ?&   p 8?&   p *?5   p ??&   a9?'   p 3q+   p 7q+   ???,   TB??-   TB?.   TB?/   ?00   TBP	1   TBR	2   TB?	3   p 8?4   v`6   v`7   p (j8   p (k8   p j9   p k9   p *:   p *?;   a 1>   R                                                                                                                                                                                                                                                                            b                                                      ?              
           ?                        ?                        ?                                                                                                                                      |                    6       B               ?                        ?                       ?                                                                                                                                                                       ?                                              ?   ?                  ?   ?                                                                                                                                                                                             ?                                                                                                                                                                                                                                                                                  ?                                                                                                                                                                                                                                                                                  ?                     G       a                ?                         ?                        ?                                                                                                                                                                        ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   '                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                                ?                                                                                                                                                                                                                                                                                ?                                                                                                                                                                                                                                                                                 [                                                                                                                                                                                                                                                                                 r                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                               ?                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                                 ?                           B               ?                        ?                       ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                           L       a                                                                 0                           ?                                                                                                                                            /                    L       a                                                                 0                           ?                                                                                                                                            K                          B               ?                        ?                       ?                                                                                                                                                                      c                                                                                                                                                                                                                                                                                 y                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                                ?                     G       a             ?  ?                                                 0                         f ?                                                                                                                                            ?                    L       d               ?                                                0                                                                                                                                                                          ?                 L       d                                                                                                                                                                                                                                                ?                    W   ????a?             ?  ?                                                 0                         f ?                                                                                                                                            ?                                                                                                                                                                                                                                                                     ?                    L       a                                                               0                          ?                                                                                                                                                                                                                                                                                                                                                                                                                           /                                                                                                                                                                                                                                                                                 ?                           B               ?                       ?                                                                                                                                                                                                  :                                                                                                                                                                                                                                                                                                    S       `                                                                                                                                                                                                                                                  <                     G       a             ?  ?                      7 ?                      ?  ?                                                                                                                                                                       V                                                                                                                                                                                                                                                                                j                                                                                                                                                                                                                                                                                ~                     G       a               ?   @@p                      @@p                0     @@p                f ?   @@p                                                                                                                                     ?                                             ?                       ?                                                                                                                                                                                                   `                                                                                                                                                                                                                                                                                ?                                                                                                                                                                                                                                                                                ?                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                                ?                                                                                                                                                                                                                                                                                ?                                          ?                        ?                       ?                                                                                                                                                                                                                                                                                                                                                                                                                                                       +                                              ?                       ?                      $ ?                                                                                                                                                                      9                                              ?                       ?                       ?                        ?                                                                                                                                          G                                          ?                                                                                                                                                                                                                              b                                          ?                                                                                                                                                                                                                               ?                                           ?                       ?                                                                                                                                                                                                   ~                                                                                                                                                                                                                                                                                 ?                                                                                                                                                                                                                                                                               ?                                                                                                                                                                                                                                                                               ?                                            ?  ?                      7 ?                                                                                                                                                                                                                                                                                                                                                                                                                                                     ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              cx23885-video                                                           ?r         ?                              YUYV                                     ????????????????????????????????????????????????????????????????                                         ?  ?^  ?G                                                                                                                                          0     ? ? @    (     @  ?  ?      6  P  ?   0       D  ?  ?      B  ?  0 p  P       H  ?  ?      H                        L  ?  ?      L                        P  ?  ?      P  @ p ?  `       P  ?  ?      V  ? ? 0	  p       T  ?  ?      _                        X  ?  ?      c                        \  ?  ?                          0     ? ? @    (     @  ?  ?      g                        D  ?  ?      B  ?    ?  P       H  ?  ?      H                        L  ?  ?      L                        P  ?  ?      P  @ @ ?  `       P  ?  ?      V  ? ?  
  p       T  ?  ?      _                        X  ?  ?      c                        \  ?  ?                          ????????????????????????????????                               ,  @               ?      ,  @               ?      :  @               P     ,  @                                             `O  @O  ????                                    `?  ?:     ?    ?8 ?  j(X
   v   ???  ??   (???H?XZ3                         `   Po  ?o      &      ?.                                      Y ??                      `       g                       )                      *                                      a   ?              d   ?                                         ????                ????             `   D    ?v      .? `   D     ?v      .? a   d                  ?v  d                           ??                      Y ??                      `   ???                                      ???j                                             @p  p  ?o  ?o                              a   ?                     a                     ?v                          `   ?|                h           U   U               c    $?   `    $?           h   z   ?R               0?  ??@ ??@ s?? s?? r?? r?? t?  t?  ???         ?                     ?S  `S  ??  ??                                     `?  0u      ?&?X?8??? d ?   v   3     &??&      ?3                         ??                           T                                                         ?S?            ?W?                                                            ?T                         U                                                                                ?Sn?Wn?[n                                                ?o            ??o                   d                 a                   ???   ????    ??     ??       @                   a             ??      ?   ?????>        ?????>         ??    ????????????????                                                                                                                                                                                                                                                            `                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              cx23885                                                                 ?r         ?                                         $#  ?  ?  ?K  ?  ?  ?M  ?  ?   N  ?  ?   O  ?  '  ?R  ?  ?  ?`  ?  ?   ?  <<    ??  ?  p   ??  <<  v  ??  <<  w                               GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                     
                                                                                                                                                                       ??       ?        ?   ?    0       ?     F   ?        b          
           m           }            ?            ?       	    
 ?              ?F       ?     ?     ?          
 ?       G    ?   G                    <       
 7  b  8     U  ?       w          ?  L       
 ?           ???  ?  1     ?  ?  ?     N	  x       ?  ?  D     ?  4      
 ?  ?  ?       `  E       ?  ?     ,  @  ?     ;  ?  ?     J  P  g     ]  ?  W     r      7              ?  ?       
 ?  `   \     ?  ?   &     ?  ?I       ?  ?        ?  !       ?  :  ~             
   0#  ?     $   $  ?    9  ?  ;     ?   t       
 S  &  ?     c  ?       x  0      
 ?  (      
 ?  ?&  g     ?    (     ?  ?      
 ?  P'  ?     ?  ?       
   /  0     b   ?       
 ,  ?'       A   (  U     S  _       j  ?       
 u  `(  ?     ?  |       ?  `       
 ?   )  ?     ?  ?       ?        
   ?)  -       ?       %        
 1  ?      
 =  ?+  6     J  ?       \  ?       
 g  ?       ?  ?      
 ?  ?,  ?     ?    p     ?  ?      
 ?  ?      
 ?  ?-  ?    ?  s  @       ?      
 &  ?/  P     ?  ?  %     ]  ?      
 i   0       {  ?       ?  ?       
 ?  @  ,    
 ?  ?  0     ?  l      
 ?  ?1  ,     ?  '       ?  ?       
 ?  C         `  O     ,         
 8  ?2       G  ?  @     b  ?      
 n  @6  
     ?  ?  }     ?  x      
 ?  l       ?  \      
 ?  @  4    ?  ?F  4    ?     0    
 
   J          ?  ^    ?	   J        )  D      
 5  ?  $    
 @  ?  ?   
 P  ?  1     i  ?       ?  (        ?  0  
    
 ?  ?I       ?  ?  3     ?  1       ?  <        ?  <  
    
 ?  t       	  H  /     '	  w       F	  P        Z	  H      
 r	  ?       ?	  ?  #     ?	  ?       ?	  d        ?	  T      
 ?	  \      
 ?	  ?       
  x        
  p  	    
 1
  |      
 F
         ]
    '     s
  A  :     ?
           ???  `=  1     ?  ?=  %    ?
  |       ?  ?  6     ?   ?      
 ?  ?>  ?     ?  `?       ?  ??  ?     ?  @  ?     ?         ?  ?      
 ?
  8       ?  ?      
 ?
  {  +     ?
  ?       ?
  ?        ?
    
    
   ?  /     &  ?       A  ?        Q        
 I  @J       e           ??t  ?A  0     ?  T  ?     ?     @    
 ?  ?  @    
 ?  ?  @    
 ?  ?A  ?    ?  ?C  `    ?   E  A     ?  PE  ?        F  ?     ?   ?       +  ?  F     ]  ?      
 K  ?F  ?     \  ?G  \     l  	  ?     \  x      
 ?  @  8    
 ?  ?K  ?     ?  ?      ?  ?      
 ?  ?  ?    ?  d      
   L      
 ?  %  $     ?  8      
 ?  I  N     ?  $      
 ?