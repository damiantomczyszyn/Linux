// SPDX-License-Identifier: GPL-2.0-or-later
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      F˘∫:C:Ü´º>◊ò˙3j#5“7$í/ÉC°bGç)î∞Ç∏qä–ÕÔ≥Ù÷~}ı”ﬁ¸ ^ãõ2 Ûß€á9˛Näàc.åoÎrOj±^å≥m≥…i√ÊÅ º  ‰¸ˆ…¢êÁˆfˇ◊¨e∏©› ÈÏ0DÜN•§i\ÅwmM[D√©h∫Îz]íø%‰}yw.∑t_˝∆U`Ò‹˙ã8≈«á]&“!ãjë∑->•Ã‡#s@“w≤!W±'=
øæó\Ü0`º¡u"‚6◊4ä◊Ëó‡7Ì∏ ˇRèNÄ≤ëkêu\%ƒ=7.ı˘MÚlªØ±;IeŸ•–»Ó°gπz„4ˆ9øT•âFö£º¿ã˛Œ"¶á§PÖ^Åô^—≠Ø«…ˇ	F€◊M ócô‚µ¥∆¿’›~‰ü„@¶æßo•#t¡VY&ß7x«πò"‰öÿ V" öeÖŒ$äáb~ó‡Ùæ<á:¯=q›Âÿí»<Â˘‘Ä¥≠	>≤˛÷h#2+H,5^ã#Üá¡Êoç[ë»p¶ËaX∂€¢>ûZ∞‘k9ÂÜUP‘œ/Ï∂®îqwpHv˚[°˝EÛ˛0l¿lúõïjÀπmëcCóˆˇ^`ºU; ù∆$#¥åYOâç,ú_„	*›>ú±^™{coa*3∏ˆCr.+ÉÜ
⁄2c"◊„÷@Kø°+¡ÕcW c.ü-∫Û¯ &:¯`p¨ª ˝⁄ÍkíI”™Ä¥\…∂LU/˛ìyHrf0F√ ìN€0ºLf1©,:‘Ω¥≤~ﬁ¬EH	DÍ3.+∞ôÆ˝⁄Û¨tÉÅ åh˙™¢ªÉ®:Æ…QêN÷íq¸/ÁG–RVbo`oÒZhãR
◊t‡ròe ó¡Õ1¨¥µ¡ï™µFó’‡÷É8R«PuZò=ïÒßs ¿/O%†«#Ö»≤¡–‡Dv@¥c¿álr^z?π°{lø ÂØi\ä(U[I”#¡,sübﬂÌ3®}@#î≥._|k	¶E˝u&æ&^º?‹U§§ŸJh¡_1VKGë√\˚ÉV“CHßÂú¨w+Túı∆ì”æz¢±∞kMÚ“Ó@Œ›éDöÃÆëB€eÛ} Åã{SÆ¶§(÷∞ƒuè	∏õº…ŒiX∑ôâYò6T®™P¶BuRﬁ™aΩip^ïÓZeWfÓ),åú›ˇ›–™q
:¨Uˆòk·LÎ£Øââ‰õPÛT¶Eøí;(ŸV*√9„¡=>8¢*Z≤ºFìÓuÀ(8ºå–UõL¡&è!ñmﬁ&Œëv,ò¡&@{R≠QN™Ìw_¶ço˙∑¶D‚∆ÇÓ;ä˘—÷{DyO{œû‰yX’?hM?'àsùU5 U÷'Æ”ÜÁ“:µ:ª±O˘¥Ωÿ9Åº}Ò/°™tª‰ÏÔoæ`†:f˙¸Æ˙N„ÿäß´».=%)¿íBÚ°Œ⁄ÙÔŒ_¿z]+¬B{c‚êS0¬·‚Á(|ﬂoÖqÊtIüπ—πa€8tk'í4/Ù∞˝î‚&â>Øﬁ˘‡ìpà∆-Ædµi¯Ÿ;1OFö~yW9ìÏÀ§ŸñÓ@fÿœUá‚√«Ë=gó√¬
¡sØÓ–äL|∫∂<å—:πNf/ëÎÿQ>„èe»o ∫5¿Êëãkö.lO¶ú€≈¯"œº 1kÒ˜,"»≤†Ç/º-O÷∫P»xÏÿΩ‰ú'Ω7¸∂Ûæ,É˝Oœj)ÔHm>)í^˙Ÿè‹“ÈóêzÏŸµ&Ô¢;-0§‚÷B∏⁄’%√#™T°®Gã∏√Y˝UNcã#,|#)fMyÏf+ºßî<∂†8Ï#y∆K£Áì≠;‚ ÊOG≠Ω)wN≥œ1ònâF©â∆1
Ü`côd!grΩ≠–‰ePl.ïTü%‚üÊ,äzöµ!∆©[u¥µíñ·Uã… ∫C∑Èà~´=∞.…Ω-$jÖ´î' ™æÖïÈÖP Í™´ÉV}“(´ä∏ƒˆ87¿]Ìrí¨ â‚mwÀ÷ﬂŒ∂∂∫i(€ßOﬂN•˜¢2\Q_=r<æÂZs§*i2‰5;Œ√åVt€ÿV%ŒWJÎ¢VE_îéAçÜANdPìkPô',(∆tI:•A{2[cÓ•¨-º∫=¬ái™ˇãj¢îÉ|=? %¥)±™µ∆È@^ˇª·⁄2°7ﬂ≤ ˚ŒÏ*÷Î–	ˇ0¥0<çïŒQ‡{¢ÍCíK0π™Vù>5x»N¯ÅÉ'À§•#uaceg}ı‰k"Â“®"y∑Oò◊˜f6∫C]#ÁP˚;ê>s„›8ÀØ+M∆À≈ÂVhß÷k„V„œ·Ö™çÛ0zÛ“ã(-ä/R¯œ≠[˙t{ç|Ò{w©π‰zÌt£K@
)ìΩô¶A«U≈îl¨¥Õ∆Ü<ﬁÁ]Ñ}∞”B®0•Ü‹ﬂ©∑èﬂ∞∑jQ†\«p#0EWàü≤;ábîÂ¨û€»ùBkfé*nŒüÔcJ&‚∏bB+èVú¢Yµ“8◊Zn¿º<}î)œ*ææ‚˝G≠=Y≤'UXÆz*πíA«üÿ¬0DÙ+e’Ω˙#®(f?˚u•j˘€Ï/À,w∆‚ b˚±ÔÀ∆∑dâ†ä_˝ $=ô∑~ı Z&÷F,põò%«ëô≠ôg w<i-A0eÔ˘Kø≈∫†#‘£•®¢ˆD…¢ﬂÙ	EñK¸ﬁ>‡ıÄCœÄ∞e•d.ìS
|L´Ü–CGzùåV¥¸[DÙZ%~¬J0∫eá¥˚àqPkÛ;≤‡p¥+ÈgÒ›Æﬂxì	öˇ*bd»KÉnç√F-Ú¡§RœgE@.-«9˜y'†¯DñKRVvú‡– L◊¬mí1Î≤¯ë˜nÏnRDÔùÇ»A£®àæî»⁄lß‹OM¶5Q^∆œèÿrQe!X˚éÄ≥©®˛◊Ì|ÁÈëº2‡Øı°·&YJßCäcpﬂB$‡‚¢kq°…ª9¢)⁄Õ:üœ*‚2¯≠IGE6ï| Öóí9Ïê`g!V2ç]6{4T2&Sfª%>Ô2„E|!∂œ7ƒöT
ﬁ1®ãÎÎøZ∫™∑'Éﬂw¥îàXü«QK	¬∞|G“^€(˙çÉ]'Ù}¿mbKh6≤æ1ÏÍÆ7ú=µìçã$∆«Ô3¸OÓ˝í=ÛÖ_±–¶ŸπK˘éËî%õ)|o∑Øï4j◊≤´_ÑëÖŸ5<ƒ'4Œô›a"„›˘©–d0∞µè
íwã‹˙*Õ‘ìÄÉñoΩÀD≠d4≤*Ω.¡6ópˇYÁ/∏è5ˇí˛É Âw∏x¡ËµdG*‰¿.¯hÔ‘ø≤ù®ÁÜ∞©sk[	.Ù√/»˝ ˚	˜c{ƒ∞s.Gƒäja‘úËπN¨®Ë!˘#Œ˛ƒH¶GuÓøÓHëøWz√¯]ò∫n8o∏∫òofÈ:¶*ÏKU%ÜÎ6Ï<íœ´¢ˇ&pg˛‰ï(ı≈è◊¿O« %x }ûíl+#àéë[øÇŸÛß<˜≤^.ΩÄ3#Oú◊	‹ôΩé•àBX›ÀL.LìÕfπíÜ;π$›%ÆxH∫Z˘0~Gqìú5Çòˇ’ªÈÆÖO4N˛*zÿˇ4ΩÆ‡Ä›Õ9'Q‰ÔHH :§‹…€{`Ù”RlÃ(¯ÄùG3WKÍËòwoPúíﬂåËc(À€ñ∫¡ÿ[0ËPdIî1+Å6@'∆˝¬≈1)[≈Jú …‘∞Q¬ﬂV*∏*l[≈3ÖªﬁxaÀ
°ÁÍÖºØ\¨◊E√•Qt‘GúdπAàò‡`ÚÎáRºﬂ…KÛú;cÛ†O7ü∏ˇØå<Jø‘„m;ÛÍ«îDæﬁl´É_Æ|∏ä.Û©x^‹›ÕÂåçØ‘]-»ƒÑ˚œq"¥Ωx¿wÒ\__“î©≤ ›ﬂoW‚Sß"Zˆ+›ŒÒ√Dƒˆªh«ê˙ô»ÁBiΩõπ∑∂ymÓE_d9 Çk3#ƒèÏ+èƒ≈’ñh>R˙q€7‹:ú™•Z9[‰C*C–ù˜µn–})ì◊—ñhvŒ¬øÚç ≈} Òåûˆç∫ˇ†≤∞WW	ˆ5Eäºˇn!A·™∂ﬁ◊Ç1`7ñ„ë]ñŸ¸^¿·6Ÿ±q`˚Î=GIgÔ6˝‚ã≠T+d!ÅÊC@$˚_˘©ÓNOqeûªCX\Ó˙a√»:r<˛'”i‡Ä«ÛﬂnIÕ≠√5◊”¥\Ï8b‹ëvfa°î@¯ ∂Ü¥˚Ë2˛‰π—€T:!T¬°ı≠≥$ª±6’	¸¯.1	∆mQàórFØ>9}ŸÙ·ì€\a‡áL˚
:⁄vçvh˛#∞ÅA§MÃÔ∂¬®œxyÜ¡Èÿ#|”GS}ÎAe9˚dw≠Pó ilyÒßÊ∂Iık∑NoﬂæU;¡ßwˇƒ∏îßz´N]Iÿlñ=^Ÿ⁄ïãW¡´ÁUÚá©ï9ˇñ¡â◊´ë·(QC≥SΩˇä˛/&‡ l·3K¬≤:xîñuA5„t¿∏ú¡yÂK6sÃ£<YW°πLH±M/Ê˚ÎP>€´È+⁄ÓÃ'¢Â¥¥n†“qijXPÏö˛t
êÔuç\9äÊò#¿KS R1Kvy>˚∫vE£Kr˘«?˜qê*+ç≠Ë3ö®d¿∆nêâÔÌ»ÁÁ”®ê>Üh°Ì	Œfè;R§⁄˛π!;?™æ∏@lõrp˜z0ãzø√+‹R+M„Ñ™$í¥ÕêÃÇk
%ËiﬂhÍK¬—œtÉ&Rê‚HàX∞ÅrÈ`eÙ≠3îFT8æ˛#ƒı-lñEÙ∞J≠ÛÏY£“∫t[ºq˙
ﬁyi˙U	≠Ü0µ¨4Ï·E¯PnZÕï$˜y⁄@lƒ.ã[ƒ†ãVG˚4
Çπ€º-™'T∂mË’Øì^ù$b˙EœÙ⁄ X›ùÅQ9,¸‡Î -y≈Õt˚ÅGjö‰„Ôü	˘nGnJœO“wv=^S˙Àg‘È◊$Ô<Ö˝‘~ Oº–7˘Äÿ‹›OrzÉl`v™ÏµwÔ}Im“iáÔ˜“ÔÒ’èc|MEñbÃ˚¥∂¡«≥•k“v™ %˘=Çê√áßÛãöíTÌ$UHπ™iªAê…!@°s*j	—{':úÇ•2h•$í·”Ù~˙û©…s•D?ò
bvqDùâ¯ÍÏ™eo∏É1
Ni≥ƒ-d4Ël•^∞Ø`4o?¨AÕ√Z√Ø·~ﬁÜÕüˆ;Ó∂táµv"É2„·åÕR˚öH‡d;⁄¨*KÖˇ–J∏‡ öŒå‚˘ús˜&L#˛)«¶íÔ$cpu() more frequently to try to loosen things up a bit.
	 * Also check to see if the CPU is getting hammered with interrupts,
	 * but only once per grace period, just to keep the IPIs down to
	 * a dull roar.
	 */
	if (time_after(jiffies, rcu_state.jiffies_resched)) {
		if (time_after(jiffies,
			       READ_ONCE(rdp->last_fqs_resched) + jtsq)) {
			resched_cpu(rdp->cpu);
			WRITE_ONCE(rdp->last_fqs_resched, jiffies);
		}
		if (IS_ENABLED(CONFIG_IRQ_WORK) &&
		    !rdp->rcu_iw_pending && rdp->rcu_iw_gp_seq != rnp->gp_seq &&
		    (rnp->ffmask & rdp->grpmask)) {
			rdp->rcu_iw_pending = true;
			rdp->rcu_iw_gp_seq = rnp->gp_seq;
			irq_work_queue_on(&rdp->rcu_iw, rdp->cpu);
		}
	}

	return 0;
}

/* Trace-event wrapper function for trace_rcu_future_grace_period.  */
static void trace_rcu_this_gp(struct rcu_node *rnp, struct rcu_data *rdp,
			      unsigned long gp_seq_req, const char *s)
{
	trace_rcu_future_grace_period(rcu_state.name, READ_ONCE(rnp->gp_seq),
				      gp_seq_req, rnp->level,
				      rnp->grplo, rnp->grphi, s);
}

/*
 * rcu_start_this_gp - Request the start of a particular grace period
 * @rnp_start: The leaf node of the CPU from which to start.
 * @rdp: The rcu_data corresponding to the CPU from which to start.
 * @gp_seq_req: The gp_seq of the grace period to start.
 *
 * Start the specified grace period, as needed to handle newly arrived
 * callbacks.  The required future grace periods are recorded in each
 * rcu_node structure's ->gp_seq_needed field.  Returns true if there
 * is reason to awaken the grace-period kthread.
 *
 * The caller must hold the specified rcu_node structure's ->lock, which
 * is why the caller is responsible for waking the grace-period kthread.
 *
 * Returns true if the GP thread needs to be awakened else false.
 */
static bool rcu_start_this_gp(struct rcu_node *rnp_start, struct rcu_data *rdp,
			      unsigned long gp_seq_req)
{
	bool ret = false;
	struct rcu_node *rnp;

	/*
	 * Use funnel locking to either acquire the root rcu_node
	 * structure's lock or bail out if the need for this grace period
	 * has already been recorded -- or if that grace period has in
	 * fact already started.  If there is already a grace period in
	 * progress in a non-leaf node, no recording is needed because the
	 * end of the grace period will scan the leaf rcu_node structures.
	 * Note that rnp_start->lock must not be released.
	 */
	raw_lockdep_assert_held_rcu_node(rnp_start);
	trace_rcu_this_gp(rnp_start, rdp, gp_seq_req, TPS("Startleaf"));
	for (rnp = rnp_start; 1; rnp = rnp->parent) {
		if (rnp != rnp_start)
			raw_spin_lock_rcu_node(rnp);
		if (ULONG_CMP_GE(rnp->gp_seq_needed, gp_seq_req) ||
		    rcu_seq_started(&rnp->gp_seq, gp_seq_req) ||
		    (rnp != rnp_start &&
		     rcu_seq_state(rcu_seq_current(&rnp->gp_seq)))) {
			trace_rcu_this_gp(rnp, rdp, gp_seq_req,
					  TPS("Prestarted"));
			goto unlock_out;
		}
		WRITE_ONCE(rnp->gp_seq_needed, gp_seq_req);
		if (rcu_seq_state(rcu_seq_current(&rnp->gp_seq))) {
			/*
			 * We just marked the leaf or internal node, and a
			 * grace period is in progress, which means that
			 * rcu_gp_cleanup() will see the marking.  Bail to
			 * reduce contention.
			 */
			trace_rcu_this_gp(rnp_start, rdp, gp_seq_req,
					  TPS("Startedleaf"));
			goto unlock_out;
		}
		if (rnp != rnp_start && rnp->parent != NULL)
			raw_spin_unlock_rcu_node(rnp);
		if (!rnp->parent)
			break;  /* At root, and perhaps also leaf. */
	}

	/* If GP already in progress, just leave, otherwise start one. */
	if (rcu_gp_in_progress()) {
		trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("Startedleafroot"));
		goto unlock_out;
	}
	trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("Startedroot"));
	WRITE_ONCE(rcu_state.gp_flags, rcu_state.gp_flags | RCU_GP_FLAG_INIT);
	WRITE_ONCE(rcu_state.gp_req_activity, jiffies);
	if (!READ_ONCE(rcu_state.gp_kthread)) {
		trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("NoGPkthread"));
		goto unlock_out;
	}
	trace_rcu_grace_period(rcu_state.name, data_race(rcu_state.gp_seq), TPS("newreq"));
	ret = true;  /* Caller must wake GP kthread. */
unlock_out:
	/* Push furthest requested GP to leaf node and rcu_data structure. */
	if (ULONG_CMP_LT(gp_seq_req, rnp->gp_seq_needed)) {
		WRITE_ONCE(rnp_start->gp_seq_needed, rnp->gp_seq_needed);
		WRITE_ONCE(rdp->gp_seq_needed, rnp->gp_seq_needed);
	}
	if (rnp != rnp_start)
		raw_spin_unlock_rcu_node(rnp);
	return ret;
}

/*
 * Clean up any old requests for the just-ended grace period.  Also return
 * whether any additional grace periods have been requested.
 */
static bool rcu_future_gp_cleanup(struct rcu_node *rnp)
{
	bool needmore;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);

	needmore = ULONG_CMP_LT(rnp->gp_seq, rnp->gp_seq_needed);
	if (!needmore)
		rnp->gp_seq_needed = rnp->gp_seq; /* Avoid counter wrap. */
	trace_rcu_this_gp(rnp, rdp, rnp->gp_seq,
			  needmore ? TPS("CleanupMore") : TPS("Cleanup"));
	return needmore;
}

/*
 * Awaken the grace-period kthread.  Don't do a self-awaken (unless in an
 * interrupt or softirq handler, in which case we just might immediately
 * sleep upon return, resulting in a grace-period hang), and don't bother
 * awakening when there is nothing for the grace-period kthread to do
 * (as in several CPUs raced to awaken, we lost), and finally don't try
 * to awaken a kthread that has not yet been created.  If all those checks
 * are passed, track some debug information and awaken.
 *
 * So why do the self-wakeup when in an interrupt or softirq handler
 * in the grace-period kthread's context?  Because the kthread might have
 * been interrupted just as it was going to sleep, and just after the final
 * pre-sleep check of the awaken condition.  In this case, a wakeup really
 * is required, and is therefore supplied.
 */
static void rcu_gp_kthread_wake(void)
{
	struct task_struct *t = READ_ONCE(rcu_state.gp_kthread);

	if ((current == t && !in_hardirq() && !in_serving_softirq()) ||
	    !READ_ONCE(rcu_state.gp_flags) || !t)
		return;
	WRITE_ONCE(rcu_state.gp_wake_time, jiffies);
	WRITE_ONCE(rcu_state.gp_wake_seq, READ_ONCE(rcu_state.gp_seq));
	swake_up_one(&rcu_state.gp_wq);
}

/*
 * If there is room, assign a ->gp_seq number to any callbacks on this
 * CPU that have not already been assigned.  Also accelerate any callbacks
 * that were previously assigned a ->gp_seq number that has since proven
 * to be too conservative, which can happen if callbacks get assigned a
 * ->gp_seq number while RCU is idle, but with reference to a non-root
 * rcu_node structure.  This function is idempotent, so it does not hurt
 * to call it repeatedly.  Returns an flag saying that we should awaken
 * the RCU grace-period kthread.
 *
 * The caller must hold rnp->lock with interrupts disabled.
 */
static bool rcu_accelerate_cbs(struct rcu_node *rnp, struct rcu_data *rdp)
{
	unsigned long gp_seq_req;
	bool ret = false;

	rcu_lockdep_assert_cblist_protected(rdp);
	raw_lockdep_assert_held_rcu_node(rnp);

	/* If no pending (not yet ready to invoke) callbacks, nothing to do. */
	if (!rcu_segcblist_pend_cbs(&rdp->cblist))
		return false;

	trace_rcu_segcb_stats(&rdp->cblist, TPS("SegCbPreAcc"));

	/*
	 * Callbacks are often registered with incomplete grace-period
	 * information.  Something about the fact that getting exact
	 * information requires acquiring a global lock...  RCU therefore
	 * makes a conservative estimate of the grace period number at which
	 * a given callback will become ready to invoke.	The following
	 * code checks this estimate and improves it when possible, thus
	 * accelerating callback invocation to an earlier grace-period
	 * number.
	 */
	gp_seq_req = rcu_seq_snap(&rcu_state.gp_seq);
	if (rcu_segcblist_accelerate(&rdp->cblist, gp_seq_req))
		ret = rcu_start_this_gp(rnp, rdp, gp_seq_req);

	/* Trace depending on how much we were able to accelerate. */
	if (rcu_segcblist_restempty(&rdp->cblist, RCU_WAIT_TAIL))
		trace_rcu_grace_period(rcu_state.name, gp_seq_req, TPS("AccWaitCB"));
	else
		trace_rcu_grace_period(rcu_state.name, gp_seq_req, TPS("AccReadyCB"));

	trace_rcu_segcb_stats(&rdp->cblist, TPS("SegCbPostAcc"));

	return ret;
}

/*
 * Similar to rcu_accelerate_cbs(), but does not require that the leaf
 * rcu_node structure's ->lock be held.  It consults the cached value
 * of ->gp_seq_needed in the rcu_data structure, and if that indicates
 * that a new grace-period request be made, invokes rcu_accelerate_cbs()
 * while holding the leaf rcu_node structure's ->lock.
 */
static void rcu_accelerate_cbs_unlocked(struct rcu_node *rnp,
					struct rcu_data *rdp)
{
	unsigned long c;
	bool needwake;

	rcu_lockdep_assert_cblist_protected(rdp);
	c = rcu_seq_snap(&rcu_state.gp_seq);
	if (!READ_ONCE(rdp->gpwrap) && ULONG_CMP_GE(rdp->gp_seq_needed, c)) {
		/* Old request still live, so mark recent callbacks. */
		(void)rcu_segcblist_accelerate(&rdp->cblist, c);
		return;
	}
	raw_spin_lock_rcu_node(rnp); /* irqs already disabled. */
	needwake = rcu_accelerate_cbs(rnp, rdp);
	raw_spin_unlock_rcu_node(rnp); /* irqs remain disabled. */
	if (needwake)
		rcu_gp_kthread_wake();
}

/*
 * Move any callbacks whose grace period has completed to the
 * RCU_DONE_TAIL sublist, then compact the remaining sublists and
 * assign ->gp_seq numbers to any callbacks in the RCU_NEXT_TAIL
 * sublist.  This function is idempotent, so it does not hurt to
 * invoke it repeatedly.  As long as it is not invoked -too- often...
 * Returns true if the RCU grace-period kthread needs to be awakened.
 *
 * The caller must hold rnp->lock with interrupts disabled.
 */
static bool rcu_advance_cbs(struct rcu_node *rnp, struct rcu_data *rdp)
{
	rcu_lockdep_assert_cblist_protected(rdp);
	raw_lockdep_assert_held_rcu_node(rnp);

	/* If no pending (not yet ready to invoke) callbacks, nothing to do. */
	if (!rcu_segcblist_pend_cbs(&rdp->cblist))
		return false;

	/*
	 * Find all callbacks whose ->gp_seq numbers indicate that they
	 * are ready to invoke, and put them into the RCU_DONE_TAIL sublist.
	 */
	rcu_segcblist_advance(&rdp->cblist, rnp->gp_seq);

	/* Classify any remaining callbacks. */
	return rcu_accelerate_cbs(rnp, rdp);
}

/*
 * Move and classify callbacks, but only if doing so won't require
 * that the RCU grace-period kthread be awakened.
 */
static void __maybe_unused rcu_advance_cbs_nowake(struct rcu_node *rnp,
						  struct rcu_data *rdp)
{
	rcu_lockdep_assert_cblist_protected(rdp);
	if (!rcu_seq_state(rcu_seq_current(&rnp->gp_seq)) || !raw_spin_trylock_rcu_node(rnp))
		return;
	// The grace period cannot end while we hold the rcu_node lock.
	if (rcu_seq_state(rcu_seq_current(&rnp->gp_seq)))
		WARN_ON_ONCE(rcu_advance_cbs(rnp, rdp));
	raw_spin_unlock_rcu_node(rnp);
}

/*
 * In CONFIG_RCU_STRICT_GRACE_PERIOD=y kernels, attempt to generate a
 * quiescent state.  This is intended to be invoked when the CPU notices
 * a new grace period.
 */
static void rcu_strict_gp_check_qs(void)
{
	if (IS_ENABLED(CONFIG_RCU_STRICT_GRACE_PERIOD)) {
		rcu_read_lock();
		rcu_read_unlock();
	}
}

/*
 * Update CPU-local rcu_data state to record the beginnings and ends of
 * grace periods.  The caller must hold the ->lock of the leaf rcu_node
 * structure corresponding to the current CPU, and must have irqs disabled.
 * Returns true if the grace-period kthread needs to be awakened.
 */
static bool __note_gp_changes(struct rcu_node *rnp, struct rcu_data *rdp)
{
	bool ret = false;
	bool need_qs;
	const bool offloaded = rcu_rdp_is_offloaded(rdp);

	raw_lockdep_assert_held_rcu_node(rnp);

	if (rdp->gp_seq == rnp->gp_seq)
		return false; /* Nothing to do. */

	/* Handle the ends of any preceding grace periods first. */
	if (rcu_seq_completed_gp(rdp->gp_seq, rnp->gp_seq) ||
	    unlikely(READ_ONCE(rdp->gpwrap))) {
		if (!offloaded)
			ret = rcu_advance_cbs(rnp, rdp); /* Advance CBs. */
		rdp->core_needs_qs = false;
		trace_rcu_grace_period(rcu_state.name, rdp->gp_seq, TPS("cpuend"));
	} else {
		if (!offloaded)
			ret = rcu_accelerate_cbs(rnp, rdp); /* Recent CBs. */
		if (rdp->core_needs_qs)
			rdp->core_needs_qs = !!(rnp->qsmask & rdp->grpmask);
	}

	/* Now handle the beginnings of any new-to-this-CPU grace periods. */
	if (rcu_seq_new_gp(rdp->gp_seq, rnp->gp_seq) ||
	    unlikely(READ_ONCE(rdp->gpwrap))) {
		/*
		 * If the current grace period is waiting for this CPU,
		 * set up to detect a quiescent state, otherwise don't
		 * go looking for one.
		 */
		trace_rcu_grace_period(rcu_state.name, rnp->gp_seq, TPS("cpustart"));
		need_qs = !!(rnp->qsmask & rdp->grpmask);
		rdp->cpu_no_qs.b.norm = need_qs;
		rdp->core_needs_qs = need_qs;
		zero_cpu_stall_ticks(rdp);
	}
	rdp->gp_seq = rnp->gp_seq;  /* Remember new grace-period state. */
	if (ULONG_CMP_LT(rdp->gp_seq_needed, rnp->gp_seq_needed) || rdp->gpwrap)
		WRITE_ONCE(rdp->gp_seq_needed, rnp->gp_seq_needed);
	WRITE_ONCE(rdp->gpwrap, false);
	rcu_gpnum_ovf(rnp, rdp);
	return ret;
}

static void note_gp_changes(struct rcu_data *rdp)
{
	unsigned long flags;
	bool needwake;
	struct rcu_node *rnp;

	local_irq_save(flags);
	rnp = rdp->mynode;
	if ((rdp->gp_seq == rcu_seq_current(&rnp->gp_seq) &&
	     !unlikely(READ_ONCE(rdp->gpwrap))) || /* w/out lock. */
	    !raw_spin_trylock_rcu_node(rnp)) { /* irqs already off, so later. */
		local_irq_restore(flags);
		return;
	}
	needwake = __note_gp_changes(rnp, rdp);
	raw_spin_unlock_irqrestore_rcu_node(rnp, flags);
	rcu_strict_gp_check_qs();
	if (needwake)
		rcu_gp_kthread_wake();
}

static void rcu_gp_slow(int delay)
{
	if (delay > 0 &&
	    !(rcu_seq_ctr(rcu_state.gp_seq) %
	      (rcu_num_nodes * PER_RCU_NODE_PERIOD * delay)))
		schedule_timeout_idle(delay);
}

static unsigned long sleep_duration;

/* Allow rcutorture to stall the grace-period kthread. */
void rcu_gp_set_torture_wait(int duration)
{
	if (IS_ENABLED(CONFIG_RCU_TORTURE_TEST) && duration > 0)
		WRITE_ONCE(sleep_duration, duration);
}
EXPORT_SYMBOL_GPL(rcu_gp_set_torture_wait);

/* Actually implement the aforementioned wait. */
static void rcu_gp_torture_wait(void)
{
	unsigned long duration;

	if (!IS_ENABLED(CONFIG_RCU_TORTURE_TEST))
		return;
	duration = xchg(&sleep_duration, 0UL);
	if (duration > 0) {
		pr_alert("%s: Waiting %lu jiffies\n", __func__, duration);
		schedule_timeout_idle(duration);
		pr_alert("%s: Wait complete\n", __func__);
	}
}

/*
 * Handler for on_each_cpu() to invoke the target CPU's RCU core
 * processing.
 */
static void rcu_strict_gp_boundary(void *unused)
{
	invoke_rcu_core();
}

/*
 * Initialize a new grace period.  Return false if no grace period required.
 */
static noinline_for_stack bool rcu_gp_init(void)
{
	unsigned long flags;
	unsigned long oldmask;
	unsigned long mask;
	struct rcu_data *rdp;
	struct rcu_node *rnp = rcu_get_root();

	WRITE_ONCE(rcu_state.gp_activity, jiffies);
	raw_spin_lock_irq_rcu_node(rnp);
	if (!READ_ONCE(rcu_state.gp_flags)) {
		/* Spurious wakeup, tell caller to go back to sleep.  */
		raw_spin_unlock_irq_rcu_node(rnp);
		return false;
	}
	WRITE_ONCE(rcu_state.gp_flags, 0); /* Clear all flags: New GP. */

	if (WARN_ON_ONCE(rcu_gp_in_progress())) {
		/*
		 * Grace period already in progress, don't start another.
		 * Not supposed to be able to happen.
		 */
		raw_spin_unlock_irq_rcu_node(rnp);
		return false;
	}

	/* Advance to a new grace period and initialize state. */
	record_gp_stall_check_time();
	/* Record GP times before starting GP, hence rcu_seq_start(). */
	rcu_seq_start(&rcu_state.gp_seq);
	ASSERT_EXCLUSIVE_WRITER(rcu_state.gp_seq);
	trace_rcu_grace_period(rcu_state.name, rcu_state.gp_seq, TPS("start"));
	raw_spin_unlock_irq_rcu_node(rnp);

	/*
	 * Apply per-leaf buffered online and offline operations to
	 * the rcu_node tree. Note that this new grace period need not
	 * wait for subsequent online CPUs, and that RCU hooks in the CPU
	 * offlining path, when combined with checks in this function,
	 * will handle CPUs that are currently going offline or that will
	 * go offline later.  Please also refer to "Hotplug CPU" section
	 * of RCU's Requirements documentation.
	 */
	WRITE_ONCE(rcu_state.gp_state, RCU_GP_ONOFF);
	/* Exclude CPU hotplug operations. */
	rcu_for_each_leaf_node(rnp) {
		local_irq_save(flags);
		arch_spin_lock(&rcu_state.ofl_lock);
		raw_spin_lock_rcu_node(rnp);
		if (rnp->qsmaskinit == rnp->qsmaskinitnext &&
		    !rnp->wait_blkd_tasks) {
			/* Nothing to do on this leaf rcu_node structure. */
			raw_spin_unlock_rcu_node(rnp);
			arch_spin_unlock(&rcu_sta// SPDX-License-Identifier: GPL-2.0-or-later
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  