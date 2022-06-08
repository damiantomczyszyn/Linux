// SPDX-License-Identifier: GPL-2.0-or-later
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
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   = 0,
			.gpio0  = 0xff00,
		}, {
			.type   = CX23885_VMUX_DEBUG,
			.vmux   = 0,
			.gpio0  = 0xff01,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = 1,
			.gpio0  = 0xff02,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = 2,
			.gpio0  = 0xff02,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1800] = {
		.name		= "Hauppauge WinTV-HVR1800",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_ENCODER,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_PHILIPS_TDA8290,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.tuner_bus	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1,
			.amux   = CX25840_AUDIO8,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1250] = {
		.name		= "Hauppauge WinTV-HVR1250",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
#ifdef MT2131_NO_ANALOG_SUPPORT_YET
		.tuner_type	= TUNER_PHILIPS_TDA8290,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.tuner_bus	= 1,
#endif
		.force_bff	= 1,
		.input          = {{
#ifdef MT2131_NO_ANALOG_SUPPORT_YET
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1,
			.amux   = CX25840_AUDIO8,
			.gpio0  = 0xff00,
		}, {
#endif
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0xff02,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0xff02,
		} },
	},
	[CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP] = {
		.name		= "DViCO FusionHDTV5 Express",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1500Q] = {
		.name		= "Hauppauge WinTV-HVR1500Q",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1500] = {
		.name		= "Hauppauge WinTV-HVR1500",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_XC2028,
		.tuner_addr	= 0x61, /* 0xc2 >> 1 */
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.gpio0  = 0,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1200] = {
		.name		= "Hauppauge WinTV-HVR1200",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1700] = {
		.name		= "Hauppauge WinTV-HVR1700",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1400] = {
		.name		= "Hauppauge WinTV-HVR1400",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP] = {
		.name		= "DViCO FusionHDTV7 Dual Express",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP] = {
		.name		= "DViCO FusionHDTV DVB-T Dual Express",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H] = {
		.name		= "Leadtek Winfast PxDVR3200 H",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200] = {
		.name		= "Leadtek Winfast PxPVR2200",
		.porta		= CX23885_ANALOG_VIDEO,
		.tuner_type	= TUNER_XC2028,
		.tuner_addr	= 0x61,
		.tuner_bus	= 1,
		.input		= {{
			.type	= CX23885_VMUX_TELEVISION,
			.vmux	= CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2,
			.amux	= CX25840_AUDIO8,
			.gpio0	= 0x704040,
		}, {
			.type	= CX23885_VMUX_COMPOSITE1,
			.vmux	= CX25840_COMPOSITE1,
			.amux	= CX25840_AUDIO7,
			.gpio0	= 0x704040,
		}, {
			.type	= CX23885_VMUX_SVIDEO,
			.vmux	= CX25840_SVIDEO_LUMA3 |
				  CX25840_SVIDEO_CHROMA4,
			.amux	= CX25840_AUDIO7,
			.gpio0	= 0x704040,
		}, {
			.type	= CX23885_VMUX_COMPONENT,
			.vmux	= CX25840_VIN7_CH1 |
				  CX25840_VIN6_CH2 |
				  CX25840_VIN8_CH3 |
				  CX25840_COMPONENT_ON,
			.amux	= CX25840_AUDIO7,
			.gpio0	= 0x704040,
		} },
	},
	[CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000] = {
		.name		= "Leadtek Winfast PxDVR3200 H XC4000",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_XC4000,
		.tuner_addr	= 0x61,
		.radio_type	= UNSET,
		.radio_addr	= ADDR_UNSET,
		.input		= {{
			.type	= CX23885_VMUX_TELEVISION,
			.vmux	= CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2 |
				  CX25840_NONE0_CH3,
		}, {
			.type	= CX23885_VMUX_COMPOSITE1,
			.vmux	= CX25840_COMPOSITE1,
		}, {
			.type	= CX23885_VMUX_SVIDEO,
			.vmux	= CX25840_SVIDEO_LUMA3 |
				  CX25840_SVIDEO_CHROMA4,
		}, {
			.type	= CX23885_VMUX_COMPONENT,
			.vmux	= CX25840_VIN7_CH1 |
				  CX25840_VIN6_CH2 |
				  CX25840_VIN8_CH3 |
				  CX25840_COMPONENT_ON,
		} },
	},
	[CX23885_BOARD_COMPRO_VIDEOMATE_E650F] = {
		.name		= "Compro VideoMate E650F",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TBS_6920] = {
		.name		= "TurboSight TBS 6920",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TBS_6980] = {
		.name		= "TurboSight TBS 6980",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TBS_6981] = {
		.name		= "TurboSight TBS 6981",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TEVII_S470] = {
		.name		= "TeVii S470",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBWORLD_2005] = {
		.name		= "DVBWorld DVB-S2 2005",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_NETUP_DUAL_DVBS2_CI] = {
		.ci_type	= 1,
		.name		= "NetUP Dual DVB-S2 CI",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1270] = {
		.name		= "Hauppauge WinTV-HVR1270",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1275] = {
		.name		= "Hauppauge WinTV-HVR1275",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1255] = {
		.name		= "Hauppauge WinTV-HVR1255",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1255_22111] = {
		.name		= "Hauppauge WinTV-HVR1255",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1210] = {
		.name		= "Hauppauge WinTV-HVR1210",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_MYGICA_X8506] = {
		.name		= "Mygica X8506 DMB-TH",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.input		= {
			{
				.type   = CX23885_VMUX_TELEVISION,
				.vmux   = CX25840_COMPOSITE2,
			},
			{
				.type   = CX23885_VMUX_COMPOSITE1,
				.vmux   = CX25840_COMPOSITE8,
			},
			{
				.type   = CX23885_VMUX_SVIDEO,
				.vmux   = CX25840_SVIDEO_LUMA3 |
						CX25840_SVIDEO_CHROMA4,
			},
			{
				.type   = CX23885_VMUX_COMPONENT,
				.vmux   = CX25840_COMPONENT_ON |
					CX25840_VIN1_CH1 |
					CX25840_VIN6_CH2 |
					CX25840_VIN7_CH3,
			},
		},
	},
	[CX23885_BOARD_MAGICPRO_PROHDTVE2] = {
		.name		= "Magic-Pro ProHDTV Extreme 2",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.input		= {
			{
				.type   = CX23885_VMUX_TELEVISION,
				.vmux   = CX25840_COMPOSITE2,
			},
			{
				.type   = CX23885_VMUX_COMPOSITE1,
				.vmux   = CX25840_COMPOSITE8,
			},
			{
				.type   = CX23885_VMUX_SVIDEO,
				.vmux   = CX25840_SVIDEO_LUMA3 |
						CX25840_SVIDEO_CHROMA4,
			},
			{
				.type   = CX23885_VMUX_COMPONENT,
				.vmux   = CX25840_COMPONENT_ON |
					CX25840_VIN1_CH1 |
					CX25840_VIN6_CH2 |
					CX25840_VIN7_CH3,
			},
		},
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1850] = {
		.name		= "Hauppauge WinTV-HVR1850",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_ENCODER,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_COMPRO_VIDEOMATE_E800] = {
		.name		= "Compro VideoMate E800",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1290] = {
		.name		= "Hauppauge WinTV-HVR1290",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_MYGICA_X8558PRO] = {
		.name		= "Mygica X8558 PRO DMB-TH",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_LEADTEK_WINFAST_PXTV1200] = {
		.name           = "LEADl£Áy
bc‚πP´±≈;
©∞9‘∑§ÛÀü ∑4'7Çkõ◊—˙]™E&œñ˜a2«ÒÛüΩHe?%∆ÅUˆVdß∑>!mÍeY£®[
b¢ßRC;n@	DÔrÔ°OÅäsXæìƒıÈ„°¯{)FOÎlÏO∆‘≤\…§·v4Ö€r¡P‹â…€ò˘Â¥eONƒÄıHm±ó=œpDK£ë‹Á&æ}û'XíÉﬁˇÀï%c’àBÜ6=pWyù‰È&dPa@˛÷õ¡˝^˜Ò´Ÿç´ˆc“~ÇyÊ∏Mﬂwoç“å≠ Ä∞®•Î˝„>˚©n´Ø’ö£MO¿v4ÉÇrÀ≈ù≤[~˝-˝‡BK!S7(Á°¶’¸Á$Y÷ﬁßnzakg=^ΩÙ±™ÉÿÛ+ß{í&≠è«¿Ω?.!µ≥;ªÆ¨ŒÃ˛@Æ£ˇ|Ó~1ú#∫YniËÕŸŸá_˛él_‰8ºé˘≤ f5–≈îˆ`ï
Ìu‘QnŸ˜ËÒ~Â˚ƒÌÀHüw&€fMA⁄¯°Ú—Å;gkñ¢4áyÚBÑ
*˛’DÕpArÿ¿0a◊<‹ìÛØk5Õ§√Xïfñ+[“¶´g§æ÷@‡¨L-›§∞bè%óßˇifY‹Îÿ;„´ô…DéÑåΩóa7æN‚˜™Œw˙Mo:cÑc∞° ÆÃ¿d\ıœ¢/nk˚®ÜRçç2¶∫ˆÚCã‡++<Úñ$
ﬂ®Ã4ﬂÖy*cñær´Áã?/:$TûçıBW's∞ É£Ø#rtKv)ØÓk0,†◊Bœ)†˘àÈ‹&Ø¡ ≥
êªRã)è∆I¸„Tàm5±	>'bkrŸx‡i™b6âMe=”U.±ö¿ÁÙû	˝çî0A&qÃlWK]5ú˝o%>¯é31≠vÎyÚ

F’˚Ì^¿äqZ§v‘ï ∞–÷(j¡kj¨Ó˛2—Cû<®Ù¥´ïÙ8%£1U:õ2«Ï»A¯Äöç›“Öî–ê∞∏ä0‚ 6∑∆6ÿ~πjóz.õmß"5¸ˇz◊À÷fÕg«¡Õ$≤=îP¯˙I]ƒöπ≠∞“)t+2ÁÑ1T®Àı‹ı≠≥~^KÛ∆íπ3æ¸]À&.◊¡ß÷≤uñÆ∫÷›æÅ”@¿´í_e‚NÀÆàÖüå‰ΩdŸ‘€W¨µô ≠D¢àào““^Ÿ:ä6"Hﬂ£ìıΩaú!I,è}à:†>eQw–8˜1èx˜›≈«g«…˙A”ìQ¿ˆÏf-ÍjÜIuxk‚6∂c¥bQΩ»q{Úà∆ﬂPÕçºp∏|$JÈ±x´π∑!∫˛]û@vC\F;T˜LÉÚìQ[£ga"£a^c˚¿RÖTÈw’ÛK—ó {¬Ô‰ü⁄ºGı€ˇH'ø∏Ôl ‘ûøó¡ﬁ˛E˝çp»TSí)(l*ü(£Ñ≈P€nìd@ËLX ≥Nª ÚÉJ‡{≥’ìı9R¨f8úW[oÃX,S≥Â®ÒY≈,ß¡Í¿¯J2"n©⁄>“≥M:¬eüîöAÕ•<#>ôRﬁ‘Ä‹|`Ú˘ó`˚Õ≈ÎËîåÈ"hHœ{3ëΩ•Kõ5¥Ï4‡Xµ‹≥±sÕVm:eC≠4äL=A<ı~_∑Û⁄pÅÿÿ}í€Wmåﬂ¿RcÁ45:È•n†L√vï!BRDﬁ√ù,√Ùb˛ËJ$ñ`íBfR3¥Îí¬k*2ˆ‘œÈÛÏò¶≤ò‘oZÛ≥ƒ„6	êô»v"†Ú€∆á4)LtÒYÈï∆Ë«èéé6*øÓ¬„÷~y$,Áçƒ0…≠N,ÿÆ’êPbb>KE¥≥‹/AÒÕVOº™Úºf t06ö ÏlpÍXSòO∫6@af†ìKXì…ﬁøÎÅÂpe8%)±/bù[ÿÌßÓ⁄d`c=QﬂvÛø„Éœª#Á}6àd€ƒt¥€O÷µ+	¨ssN¿ôÙﬂßü·’0.3cìûlïÖ"è…É„GÉ©Ùf`ì≠
†$—ø:Í¬‹g)!Õ¿ïÿd˛≈üoh3ô∫¯ân±A˙ÒŒl¶!k_V?Ú˙•aTdœ_(1jò≤aGõˇ,∆˜fŸ≤π»Cû-i‹z ∫ﬁπYò€„áﬂ‡≥yÚÁÀs,ä!ç≤ÂÒn≈ÏÆú˝Õ"Ω#•‚˛qõW1ìı¯ªv¢lK7¢Áô,ÍçôezüÖàr:ˆ„‹eﬂåq9ò¢‚Q-†+Ÿ§‘ufø`åù .D‹ r$ÎB÷¥êzäé®®ñ%§i€∏í¡ˆp\¨€ÕÇæåˇáZúù¸|÷a'æò¨\ßDït'˜“~*≥>6&¥…†[C&-sª*âV„}ú+∫ÎY{ﬂ<¨†{Wß∆uÊ¨ΩˇFTì⁄§Å”ÇäbXÿãp"¶Ü+â›ı∆∞ç8¸≥›#=√zªeö–>˜cNÚ.˘qòaøey◊007Õr[É™äÅ$Â1Æıé≠'hΩpv@LrÈ¸¥@éwÔNc$–  «‡1˝—Ù§9h√‰Yf`_s•C˜Ù—:°„ÊâÄ}–Fe&⁄4L∞∑›Äñ÷gé≤EÎN‰¯ù:;AŸS&L'ô√züó¿$“7ÀI…S÷∞≥ênïxﬁÿUôkvR∂˙'Rƒ+ÛN‡}0˜¸C´)o©,√Cq'Ìrù|69√ÊC…†”∞≤ù÷€üt#·0LFÀQ’·/Œötiúázc†_˚/aoóÔÙÊ5†Ó©éƒi•˜^”µÕpôÿ•ëZÙë`'ﬁ"Y¯◊tX›Ÿ8:$Rê¡øöcÍ§‰EΩ‡i:-[ŸÄrøçïn‡0gÃ¿<@l©¬¯´Ë’+!è|ÑÃ%ü™ùÿhî⁄#}©:K–=]ì52+jF+(∂∏•vsU:‘∑V_ZˇûÙ9»ÂPvà°ãΩÿa°¯Bkç)ŸhﬂÁü6p ÏT/J)ÿJ∑<˘KB˙ÖåÔm¢z¯cÀU$◊h¡i§ﬁd„)"Á,›X|‘Ië÷¢ﬁKL,&÷π.#§yx÷*∫s»ôDh®2¨Yf«”Û6Ó›∆„B¡
[Áº€e-Óq%Ì{Û^+∞úÑﬁÎusÊùAõ_-ø†„ÿ…=\ã•û≤dÜ}⁄; Î<89Ùñÿ%HÊQ–ÅZò:»«pôµ˙‚‹ZÏ04zº™`EÒ!„∑˙Í~)>ä‡éjåÅMü∆≈ëtH◊
2I6∂‰ù˝ÛªÑ;Êüê≈”∫ZC∂∫áú‰àrh(ä»’y	ÀÁÄ»%†ı)äü?ä1QÙLåütﬂÕã}` /*XK2{L0‹0Ñ[s∞⁄föÈÜéìtrë¥wÛÎ‚0^íÎ\Ä<Ã‹–N¥≠,
B¡Î÷âŒ“•Œ•Ë’Ò->Ç?ìP˝¬SrºüÉ◊T†¸†ÆâÍ“mk€FúE"G¢ñô≠ıÂê∞o v0∏iøøNß˚˝´≠y0ú©Ãî‚O1Û∏⁄ß·‚ŸkY¬◊ÃóU=J=E I,€áê™#N[˜Z˝8ﬂ^”È}æ+"—/œg$ Ÿçõ~∂∞ÓÚÜ©>¿Ó¶_ô∫œnw˚êó-<C]Q 8±ÕÙiw¡ ˜4ªıtçîH=©ré˜]˛akW∏|>Ü7ì˚_ƒ…Tî,r≤Çñç_òJPG2Cﬁ´d=üp	I¨8~K&ÌËQLÁ€äàå∏›ùùπq◊ÚsπI)Ã©6ı[e.Ñ±6gÊÇÁKﬂ%Á·izøõ ΩAOÇö„‹vTıLê¡¯ó≥± 2U5e†ÂﬂZ:Ì(ƒ|BÕ◊jÓ(˙Uµ√>76‹÷æÊ1‹E,Ö–:_Ê›âÆ£m’µLˆÄﬂ·rëÛ!b°sö@ÛÕ+ø ˘hy˛&_VöÌ>0‚;.åŒá˚'ÜB∂Ï`}‹ÁNQ@ˇ˘0$èP)9Œ„)ÖNJä¡<=≠Ñ]|vÓ2x¸⁄ÒŸòhñ~to§V¿ˆ¨`Ÿâá›|UãËlëÃúÕì¨\oe+≈Ω ?D—
j)–‚Áû}Àv∫ø 1*•F¡kƒ†£ã‘Rä`ˇòûùÇö∆âEÉT$ÿ∆eË0&xm‡ZïfØP∫6÷‰Ïa≠Í'fõ0&HXÕFSÍË4Ø≤'ÿÆU⁄DHfß·Äo(≤˜A¿—_NØpkÌsC…±•†L0˙_»9[6«ﬁt}ÒÜ*µã’rØ©à1Ûvd°M‚ë/-¬88#ΩëoÀÂÁ´Ì¯i∫V2kóSX«sÚ;zπ+R$L€a”4$F+›å(ı!%TPî˚Ã‡√åeë≤‡Õ`kTÏEqMö$≠˚‚¿§_Æ„Û≠˝ÈÚÕül‰ò}àÅ˙%˛≤%˚¥îy:Îø[·!£\QπΩL•µ[c.c≠ k±∑o?î»{˚•¸k?µû…’áF;∂+±mêöúƒ/⁄FmµÁæÁÜbo=Ô†)U§ˆ°≤®≤`˘π9M]®Bœl Ò}Ô¸ó˝øe<bÀ¿“u¿
ê≤±£i›Iö	ÕgáÚ{}Õ€JA÷∫)Í»√ª¨=uë4∞ÁÿîMwûJ`“bZ?„ ‹(Ûg1Á(§ESâ\o,Ñÿ˘6√◊$∞‰ÂbX∆÷1‘_u	Ω2Xœ|µ˝†cB.ˆı˜0Ôó÷æê2‘°V}S¢ ßYéZªèßnCû^ö†Î8åY`®¡˙Ä?‹…	¶ºŸ^ΩåèHFîü-‚kƒÚ=.í^vÈä·√gèEÒ»¸ã∑å˘/ƒ÷,ÍAÓ@˛,K„O´wêoowÑ}“p1®í$ÇZ‡ΩÙâøeW¯∆'…ëÜ∑>Z[≤àEˆƒD[$äÂÙï-p≤EﬂMvVÇO'‘9ÀÀ^p{`do%ê\‹∞Ä¸∞;Éß≥Ùä˚—≤…ÇÅC∏Ö@‚åÑ$Á¨üZ”cµI·NTr∞”vÌ]"´Æx∆uÍÇX”&ìˆŒP˝˚6m$≈$ñÊÊ·¡ê˙Üò )ãÅ?_l4)wı≠9ŸﬂZòí°áÎ™s˘áÚI¬„	CŸ·ï€<O≤Ü/ºÃ€uP `Í⁄ˆ¥R°np¨‘Ï˙ﬁ’\yeóáUá∏«~Â/÷Ç,“äké“ÅtØÉÅP∏#¿ æbúr‡ÒIÅ“.û©
Ω‘ŒëWpˇ~ê¬á|¥™ih∫∂¯˛∫1ß¡⁄ê±˙At6*KL"/$qÙˆ+ÕE∫∞¶’_#◊àô)Åº∑˝£Â])	ÅÖjfÌbKÚÙ>ò'§R‡:⁄Ï˛!¿ÿÒj S5su≤Ñ’J˘ìÜ™BØèÀ2amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN8_CH1 |
				  CX25840_NONE_CH2 |
				  CX25840_VIN7_CH3 |
				  CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO6,
		}, {
			.type   = CX23885_VMUX_COMPONENT,
			.vmux   = CX25840_VIN1_CH1 |
				  CX25840_NONE_CH2 |
				  CX25840_NONE0_CH3 |
				  CX25840_NONE1_CH3,
			.amux   = CX25840_AUDIO6,
		} },
	},
	[CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2] = {
		.name		= "DViCO FusionHDTV DVB-T Dual Express2",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_IMPACTVCBE] = {
		.name		= "Hauppauge ImpactVCB-e",
		.tuner_type	= TUNER_ABSENT,
		.porta		= CX23885_ANALOG_VIDEO,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN4_CH2 |
				  CX25840_VIN8_CH1 |
				  CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_DVBSKY_T9580] = {
		.name		= "DVBSky T9580",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_T980C] = {
		.name		= "DVBSky T980C",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_S950C] = {
		.name		= "DVBSky S950C",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TT_CT2_4500_CI] = {
		.name		= "Technotrend TT-budget CT2-4500 CI",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_S950] = {
		.name		= "DVBSky S950",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_S952] = {
		.name		= "DVBSky S952",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_T982] = {
		.name		= "DVBSky T982",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR5525] = {
		.name		= "Hauppauge WinTV-HVR5525",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.force_bff	= 1,
		.input		= {{
			.type	= CX23885_VMUX_TELEVISION,
			.vmux	=	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN7_CH3 |
				  CX25840_VIN8_CH1 |
				  CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_VIEWCAST_260E] = {
		.name		= "ViewCast 260e",
		.porta		= CX23885_ANALOG_VIDEO,
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN7_CH3 |
					CX25840_VIN5_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_COMPONENT,
			.vmux   = CX25840_VIN7_CH3 |
					CX25840_VIN6_CH2 |
					CX25840_VIN5_CH1 |
					CX25840_COMPONENT_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_VIEWCAST_460E] = {
		.name		= "ViewCast 460e",
		.porta		= CX23885_ANALOG_VIDEO,
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN4_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN7_CH3 |
					CX25840_VIN6_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_COMPONENT,
			.vmux   = CX25840_VIN7_CH3 |
					CX25840_VIN6_CH1 |
					CX25840_VIN5_CH2 |
					CX25840_COMPONENT_ON,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_COMPOSITE2,
			.vmux   = CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_QUADHD_DVB] = {
		.name         = "Hauppauge WinTV-QuadHD-DVB",
		.porta        = CX23885_ANALOG_VIDEO,
		.portb        = CX23885_MPEG_DVB,
		.portc        = CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885] = {
		.name         = "Hauppauge WinTV-QuadHD-DVB(885)",
		.portb        = CX23885_MPEG_DVB,
		.portc        = CX23885_MPEG_DVB,
		.tuner_type   = TUNER_ABSENT,
	},
	[CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC] = {
		.name         = "Hauppauge WinTV-QuadHD-ATSC",
		.porta        = CX23885_ANALOG_VIDEO,
		.portb        = CX23885_MPEG_DVB,
		.portc        = CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885] = {
		.name         = "Hauppauge WinTV-QuadHD-ATSC(885)",
		.portb        = CX23885_MPEG_DVB,
		.portc        = CX23885_MPEG_DVB,
		.tuner_type   = TUNER_ABSENT,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1265_K4] = {
		.name		= "Hauppauge WinTV-HVR-1265(161111)",
		.porta          = CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type     = TUNER_ABSENT,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_STARBURST2] = {
		.name		= "Hauppauge WinTV-Starburst2",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_AVERMEDIA_CE310B] = {
		.name		= "AVerMedia CE310B",
		.porta		= CX23885_ANALOG_VIDEO,
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN1_CH1 |
				  CX25840_NONE_CH2 |
				  CX25840_NONE0_CH3,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN8_CH1 |
				  CX25840_NONE_CH2 |
				  CX25840_VIN7_CH3 |
				  CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
};
const unsigned int cx23885_bcount = ARRAY_SIZE(cx23885_boards);

/* ------------------------------------------------------------------ */
/* PCI subsystem IDs                                                  */

struct cx23885_subid cx23885_subids[] = {
	{
		.subvendor = 0x0070,
		.subdevice = 0x3400,
		.card      = CX23885_BOARD_UNKNOWN,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7600,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1800lp,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7800,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1800,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7801,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1800,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7809,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1800,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7911,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1250,
	}, {
		.subvendor = 0x18ac,
		.subdevice = 0xd500,
		.card      = CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7790,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1500Q,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7797,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1500Q,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7710,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1500,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7717,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1500,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x71d1,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1200,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x71d3,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1200,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x8101,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1700,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x8010,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1400,
	}, {
		.subvendor = 0x18ac,
		.subdevice = 0xd618,
		.card      = CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP,
	}, {
		.subvendor = 0x18ac,
		.subdevice = 0xdb78,
		.card      = CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP,
	}, {
		.subvendor = 0x107d,
		.subdevice = 0x6681,
		.card      = CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H,
	}, {
		.subvendor = 0x107d,
		.subdevice = 0x6f21,
		.card      = CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200,
	}, {
		.subvendor = 0x107d,
		.subdevice = 0x6f39,
		.card	   = CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000,
	}, {
		.subvendor = 0x185b,
		.subdevice = 0xe800,
		.card      = CX23885_BOARD_COMPRO_VIDEOMATE_E650F,
	}, {
		.subvendor = 0x6920,
		.subdevice = 0x8888,
		.card      = CX23885_BOARD_TBS_6920,
	}, {
		.subvendor = 0x6980,
		.su