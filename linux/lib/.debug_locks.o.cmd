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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      Fùº:C:†«¼>×˜ú3j#5Ò7$’/ƒC¡bG)”°‚¸qŠĞÍï³ôÖ~}õÓŞü ^‹›2Êó§Û‡9şNŠğˆc.ŒoërOj±^Œ³m³ÉiÃæ ¼ÊÊäüöÉ¢çöfÿ×¬e¸©İ éì0D†N¥¤i\wmM[DÃ©hºëz]’¿%ä}yw.·t_ıÆU`ñÜú‹8ÅÇ‡]&Ò!‹j‘·->¥Ìà#s@Òw²!W±'=
¿¾—\†0`¼Áu"â6×4Š×è—à7í¸ ÿRN€²‘ku\%Ä=7.õùMòl»¯±;IeÙ¥ĞÈî¡g¹zã4ö9¿T¥‰Fš£¼À‹şÎ"¦‡¤P…^™^Ñ­¯ÇÉÿ	FÛ×M —c™âµ´ÆÀÕİ~äŸã@¦¾§o¥#tÁVY&§7xÇ¹˜"äšØÊV"Êše…Î$Š‡b~—àô¾<‡:øğ=qİåØ’È<åùÔ€´­	>²şÖh#2+H,5^‹#†‡Áæo[‘Èp¦èaX¶Û¢>Z°Ôk9å†UPÔÏ/ì¶¨”qwpHvû[¡ıEóş0lÀlœ›•jË¹m‘cC—öÿ^`¼U; Æ$#´ŒYO‰,œ_ã	*İ>œ±^ª{coa*3¸öCr.+ƒ†
Ú2c"×ãÖ@K¿¡+ÁÍcW c.Ÿ-ºóø &:ø`p¬» ıÚêk’IÓª€´\É¶LU/ş“yHrf0FÃÊ“NÛ0¼Lf1©,:Ô½´²~ŞÂEH	Dê3.+°™®ıÚó¬tƒ Œhúª¢»ƒ¨:®ÉQNÖ’qü/çGĞRVbo`oñZh‹R
×tàr˜eÊ—ÁÍ1¬´µÁ•ªµF—ÕàÖƒ8RÇPuZ˜=•ñ§s À/O% Ç#…È²ÁĞàDv@´cÀ‡lr^z?¹¡{l¿ å¯i\Š(U[IÓ#Á,sŸbßí3¨}@#”³._|k	¦Eıu&¾&^¼?ÜU¤¤ÙJhÁ_1VKG‘Ã\ûƒVÒCH§åœ¬w+TœõÆ“Ó¾z¢±°kMòÒî@ÎİDšÌ®‘BÛeó} ‹{S®¦¤(Ö°Äu	¸›¼ÉğÎiX·™‰Y˜6T¨ªP¦BuRŞªa½ip^•îZeWfî),ŒœİÿİĞªq
:ğ¬Uö˜káLë£¯‰‰ä›PóT¦E¿’;(ÙV*Ã9ãÁ=>8¢*Z²¼F“îuË(8¼ŒĞU›LÁ&!–mŞ&Î‘v,˜Á&@{R­QNªíw_¦oú·¦DâÆ‚î;ŠùÑÖ{DyO{ÏäyXÕ?hM?'ˆsU5ÊUÖ'®Ó†çÒ:µ:»±Où´½Ø9¼}ñ/¡ªt»äìïo¾` :fúü®úNãØŠ§«È.=%)À’Bò¡ÎÚôïÎ_Àz]+ÂB{câS0Âáâç(|ßo…qætIŸ¹Ñ¹aÛ8tk'’4/ô°ı”â&‰>¯Şùà“pˆÆ-®dµiøÙ;1OFš~yW9“ìË¤Ù–î@fØÏU‡âÃÇè=g—ÃÂ
Ás¯îĞŠL|º¶<ŒÑ:¹Nf/‘ëØQ>ãeÈo º5Àæ‘‹kš.lO¦œÛÅø"Ï¼Ê1kñ÷,"È² ‚/¼-OÖºPÈxìØ½äœ'½7ü¶ó¾,ƒıOÏj)ïHm>)’^úÙÜÒé—zìÙğµ&ï¢;-0¤âÖB¸ÚÕ%Ã#ªT¡¨G‹¸ÃYıUNc‹#,|#)fMyìf+¼§”<¶ 8ì#yÆK£ç“­;â æOG­½)wN³Ï1˜n‰F©‰Æ1
†`c™d!gr½­ĞäePl.•TŸ%âŸæ,Šzšµ!Æ©[u´µ’–áU‹É ºğC·éˆ~«=°.É½-$j…«”' ª¾…•é…PÊêª«ƒV}Ò(«Š¸Äö87À]ír’¬ ‰âmwËÖßÎ¶¶ºi(Û§OßN¥÷¢2\Q_=r<¾åZs¤*i2ä5;ÎÃŒVtÛØV%ÎWJë¢VE_”A†ANdP“kP™',(ÆtI:¥A{2[cî¥¬-¼º=Â‡iªÿ‹j¢”ƒ|=? %´)±ªµÆé@^ÿ»áÚ2¡7ß² ûÎì*ÖëĞ	ÿ0´0<•ÎQà{¢êC’K0¹ªV>5xÈNøƒ'Ë¤¥#uaceg}õäk"åÒ¨"y·O˜×÷f6ºC]#çPû;>sãİ8Ë¯+MÆËÅåVh§ÖkãVãÏá…ªó0zóÒ‹(-Š/RøÏ­[út{|ñ{w©¹äzít£K@
)“½™¦AÇUÅ”l¬´ÍÆ†<Şçğ]„}°ÓB¨0¥†Üß©·ß°·jQ \Çp#0EWˆŸ²;‡b”å¬ÛÈBkf*nÎŸïcJ&â¸bB+Vœ¢YµÒ8×ZnÀ¼<}”)Ï*¾¾âıG­=Y²'UX®z*¹’AÇŸØÂ0Dô+eÕ½ú#¨(f?ûu¥jùÛì/Ë,wÆâ bû±ïËÆ·d‰ Š_ı $=™·~õÊZ&ÖF,p›˜%Ç‘™­™g w<i-A0eïùK¿Åº #Ô£¥¨¢öDÉ¢ßô	E–KüŞ>àõ€CÏ€°e¥d.“S
|L«†ĞCGzŒV´ü[DôZ%~ÂJ0ºe‡´ûˆqPkó;²àp´+égñİ®ßx“	šÿ*bdÈKƒnÃF-òÁ¤RÏgE@.-Ç9÷y' øD–KRVvœàĞ L×Âm’1ë²ø‘÷nìnRDï‚ÈA£¨ˆ¾”ÈÚl§ÜOM¦5Q^ÆÏØrQe!Xû€³©¨ş×í|çé‘¼2à¯õ¡á&YJ§CŠcpßB$àâ¢kq¡É»9¢)ÚÍ:ŸÏ*â2ø­IGE6•| …—’9ì`g!V2]6{4T2&Sf»%>ï2ãE|!¶Ï7ÄšT
Ş1¨‹ëë¿Zºª·'ƒßw´”ˆXŸÇQK	Â°|GÒ^Û(úƒ]'ô}ÀmbKh6²¾1ìê®7œ=µ“‹$ÆÇï3üOîı’=ó…_±Ğ¦Ù¹Kùè”%›)|o·¯•4j×²«_„‘…Ù5<Ä'4Î™İa"ãİù©Ğd0°µ
’w‹Üú*ÍÔ“€ƒ–o½ËD­d4²*½.Á6—pÿYç/¸5ÿ’şƒ åw¸xÁèµdG*äÀ.øhïÔ¿²¨ç†°©sk[	.ôÃ/ÈıÊû	÷c{Ä°s.GÄŠjaÔœè¹N¬¨è!ù#ÎşÄH¦Guî¿îH‘¿WzÃø]˜ºn8o¸º˜ofé:¦*ìKU%†ë6ì<’ğÏ«¢ÿ&pgşä•(õÅ×ÀOÇÊ%x }’l+#ˆ‘[¿‚Ùó§<÷²^.½€3#Oœ×	Ü™½¥ˆBXİËL.L“Íf¹’†;¹$İ%®xHºZù0~Gq“œ5‚˜ÿÕ»é®…O4Nş*zØÿ4½®à€İÍ9'QäïHH :¤ÜÉÛ{`ôÓRlÌ(ø€G3WKêè˜woPœ’ßŒèc(ËÛ–ºÁØ[0èPdI”1+6@'ÆıÂÅ1)[ÅJœ ÉÔ°QÂßV*¸*l[Å3…»ŞxaË
¡çê…¼¯\¬×EÃ¥QtÔGœd¹Aˆ˜à`òë‡R¼ßÉKóœ;có O7Ÿ¸ÿ¯Œ<J¿Ôãm;óêÇ”D¾Şl«ƒ_®|¸Š.ó©x^ÜİÍåŒ¯Ô]-ÈÄ„ûÏq"´½xÀwñ\__Ò”©² İßoWâS§"Zö+İÎñÃDÄö»hÇú™ÈçBi½›¹·¶ymîE_d9 ‚k3#Äì+ÄÅÕ–h>RúqÛ7Ü:œª¥Z9[äC*CĞ÷µnĞ})“×Ñ–hvÎÂ¿ò Å} ñŒöºÿ ²°WW	ö5EŠ¼ÿn!Aáª¶Ş×‚1`7–ã‘]–Ùü^Àá6Ù±q`ûë=GIgï6ıâ‹­T+d!æC@ğ$û_ù©îNOqe»CX\îúaÃÈ:r<ş'Óià€ÇóßnIÍ­Ã5×Ó´\ì8bÜ‘vfa¡”@ø ¶†´ûè2şä¹ÑÛT:!TÂ¡õ­³$»±6Õ	üø.1	ÆmQˆ—rF¯>9}Ùôá“Û\aà‡Lû
:Úvvhş#°A¤MÌï¶Â¨Ïxy†ÁéØ#|ÓGS}ëAe9ûdw­P— ilyñ§æ¶Iõk·Noß¾U;Á§wÿÄ¸”§z«N]IØl–=^ÙÚ•‹WÁ«çUòğ‡©•9ÿ–Áğ‰×«‘á(QC³S½ÿŠş/&à lá3KÂ²:x”–uA5ãtÀ¸œÁyåK6sÌ£<YW¡¹LH±M/æûëP>Û«é+ÚîÌ'¢å´´n ÒqijXPìšşt
ïu\9Šæ˜#ÀKSÊR1Kvy>ûºvE£KrùÇ?÷q*+­è3š¨dÀÆn‰ïíÈççÓ¨>†h¡í	Îf;R¤Úş¹!;?ª¾¸@l›rp÷z0‹z¿Ã+ÜR+Mã„ª$’´ÍÌ‚k
%èißhêKÂÑÏtƒ&RâHˆX°ré`eô­3”FT8¾ş#Äõ-l–Eô°J­óìY£Òºt[¼qú
ŞyiúUğ	­†0µ¬4ìáEøPnZÍ•$÷yÚ@lÄ.‹[Ä ‹VGû4
‚¹Û¼-ª'T¶mèÕ¯“^$búEÏôÚÊXİQ9,üàë -yÅÍtûGjšäãïŸ	ùnGnJÏOÒwv=^SúËgÔé×$ï<…ıÔ~ O¼Ğ7ù€ØÜğİOrzƒl`vªìµwï}ImÒi‡ï÷ÒïñÕc|ME–bÌû´¶ÁÇ³¥kÒvª %ù=‚Ã‡§ó‹š’Tí$UH¹ªi»AÉ!@¡s*j	Ñ{':œ‚¥2h¥$’áÓô~ú©És¥D?˜
bvqD‰øêìªeo¸ƒ1
Ni³Ä-d4èl¥^°¯`4o?¬AÍÃZÃ¯á~Ş†ÍŸö;î¶t‡µv"ƒ2ãáŒÍRûšHàd;Ú¬*K…ÿĞğJ¸à šÎŒâùœs÷&L#ş)Ç¦’ï$cpu() more frequently to try to loosen things up a bit.
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
		trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("NoG