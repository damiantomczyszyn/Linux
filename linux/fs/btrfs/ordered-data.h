cx23885_irq_add_enable(dev, PCI_MSK_GPIO0);
		break;
	}

	/*
	 * The CX2388[58] IR controller can start firing interrupts when
	 * enabled, so these have to take place after the cx23885_irq() handler
	 * is hooked up by the call to request_irq() above.
	 */
	cx23885_ir_pci_int_enable(dev);
	cx23885_input_init(dev);

	return 0;

fail_irq:
	cx23885_dev_unregister(dev);
fail_ctrl:
	v4l2_ctrl_handler_free(hdl);
	v4l2_device_unregister(&dev->v4l2_dev);
fail_free:
	kfree(dev);
	return err;
}

static void cx23885_finidev(struct pci_dev *pci_dev)
{
	struct v4l2_device *v4l2_dev = pci_get_drvdata(pci_dev);
	struct cx23885_dev *dev = to_cx23885(v4l2_dev);

	cx23885_input_fini(dev);
	cx23885_ir_fini(dev);

	cx23885_shutdown(dev);

	/* unregister stuff */
	free_irq(pci_dev->irq, dev);

	pci_disable_device(pci_dev);

	cx23885_dev_unregister(dev);
	v4l2_ctrl_handler_free(&dev->ctrl_handler);
	v4l2_device_unregister(v4l2_dev);
	kfree(dev);
}

static const struct pci_device_id cx23885_pci_tbl[] = {
	{
		/* CX23885 */
		.vendor       = 0x14f1,
		.device       = 0x8852,
		.subvendor    = PCI_ANY_ID,
		.subdevice    = PCI_ANY_ID,
	}, {
		/* CX23887 Rev 2 */
		.vendor       = 0x14f1,
		.device       = 0x8880,
		.subvendor    = PCI_ANY_ID,
		.subdevice    = PCI_ANY_ID,
	}, {
		/* --- end of list --- */
	}
};
MODULE_DEVICE_TABLE(pci, cx23885_pci_tbl);

static struct pci_driver cx23885_pci_driver = {
	.name     = "cx23885",
	.id_table = cx23885_pci_tbl,
	.probe    = cx23885_initdev,
	.remove   = cx23885_finidev,
};

static int __init cx23885_init(void)
{
	pr_info("cx23885 driver version %s loaded\n",
		CX23885_VERSION);
	return pci_register_driver(&cx23885_pci_driver);
}

static void __exit cx23885_fini(void)
{
	pci_unregister_driver(&cx23885_pci_driver);
}

module_init(cx23885_init);
module_exit(cx23885_fini);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kmod.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/div64.h>
#include <linux/firmware.h>

#include "cimax2.h"
#include "altera-ci.h"
#include "cx23888-ir.h"
#include "cx23885-ir.h"
#include "cx23885-av.h"
#include "cx23885-input.h"

MODULE_DESCRIPTION("Driver for cx23885 based TV cards");
MODULE_AUTHOR("Steven Toth <stoth@linuxtv.org>");
MODULE_LICENSE("GPL");
MODULE_VERSION(CX23885_VERSION);

/*
 * Some platforms have been found to require periodic resetting of the DMA
 * engine. Ryzen and XEON platforms are known to be affected. The symptom
 * encountered is "mpeg risc op code error". Only Ryzen platforms employ
 * this workaround if the option equals 1. The workaround can be explicitly
 * disabled for all platforms by setting to 0, the workaround can be forced
 * on for any platform by setting to 2.
 */
static unsigned int dma_reset_workaround = 1;
module_param(dma_reset_workaround, int, 0644);
MODULE_PARM_DESC(dma_reset_workaround, "periodic RiSC dma engine reset; 0-force disable, 1-driver detect (default), 2-force enable");

static unsigned int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "enable debug messages");

static unsigned int card[]  = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };
module_param_array(card,  int, NULL, 0444);
MODULE_PARM_DESC(card, "card type");

#define dprintk(level, fmt, arg...)\
	do { if (debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: " fmt), \
		       __func__, ##arg); \
	} while (0)

static unsigned int cx23885_devcount;

#define NO_SYNC_LINE (-1U)

/* FIXME, these allocations will change when
 * analog arrives. The be reviewed.
 * CX23887 Assumptions
 * 1 line = 16 bytes of CDT
 * cmds size = 80
 * cdt size = 16 * linesize
 * iqsize = 64
 * maxlines = 6
 *
 * Address Space:
 * 0x00000000 0x00008fff FIFO clusters
 * 0x00010000 0x000104af Channel Management Data Structures
 * 0x000104b0 0x000104ff Free
 * 0x00010500 0x000108bf 15 channels * iqsize
 * 0x000108c0 0x000108ff Free
 * 0x00010900 0x00010e9f IQ's + Cluster Descriptor Tables
 *                       15 channels * (iqsize + (maxlines * linesize))
 * 0x00010ea0 0x00010xxx Free
 */

static struct sram_channel cx23885_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VI