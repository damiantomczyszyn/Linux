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
		.name           = "LEADTEK WinFast PxTV1200",
		.porta          = CX23885_ANALOG_VIDEO,
		.tuner_type     = TUNER_XC2028,
		.tuner_addr     = 0x61,
		.tuner_bus	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   = CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2 |
				  CX25840_NONE0_CH3,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_COMPOSITE1,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_SVIDEO_LUMA3 |
				  CX25840_SVIDEO_CHROMA4,
		}, {
			.type   = CX23885_VMUX_COMPONENT,
			.vmux   = CX25840_VIN7_CH1 |
				  CX25840_VIN6_CH2 |
				  CX25840_VIN8_CH3 |
				  CX25840_COMPONENT_ON,
		} },
	},
	[CX23885_BOARD_GOTVIEW_X5_3D_HYBRID] = {
		.name		= "GoTView X5 3D Hybrid",
		.tuner_type	= TUNER_XC5000,
		.tuner_addr	= 0x64,
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   = CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2,
			.gpio0	= 0x02,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX23885_VMUX_COMPOSITE1,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_SVIDEO_LUMA3 |
				  CX25840_SVIDEO_CHROMA4,
		} },
	},
	[CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF] = {
		.ci_type	= 2,
		.name		= "NetUP Dual DVB-T/C-CI RF",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
		.num_fds_portb	= 2,
		.num_fds_portc	= 2,
		.tuner_type	= TUNER_XC5000,
		.tuner_addr	= 0x64,
		.input          = { {
				.type   = CX23885_VMUX_TELEVISION,
				.vmux   = CX25840_COMPOSITE1,
		} },
	},
	[CX23885_BOARD_MPX885] = {
		.name		= "MPX-885",
		.porta		= CX23885_ANALOG_VIDEO,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_COMPOSITE1,
			.amux   = CX25840_AUDIO6,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE2,
			.vmux   = CX25840_COMPOSITE2,
			.amux   = CX25840_AUDIO6,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE3,
			.vmux   = CX25840_COMPOSITE3,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE4,
			.vmux   = CX25840_COMPOSITE4,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0,
		} },
	},
	[CX23885_BOARD_MYGICA_X8507] = {
		.name		= "Mygica X8502/X8507 ISDB-T",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.input		= {
			{
				.type   = CX23885_VMUX_TELEVISION,
				.vmux   = CX25840_COMPOSITE2,
				.amux   = CX25840_AUDIO8,
			},
			{
				.type   = CX23885_VMUX_COMPOSITE1,
				.vmux   = CX25840_COMPOSITE8,
				.amux   = CX25840_AUDIO7,
			},
			{
				.type   = CX23885_VMUX_SVIDEO,
				.vmux   = CX25840_SVIDEO_LUMA3 |
						CX25840_SVIDEO_CHROMA4,
				.amux   = CX25840_AUDIO7,
			},
			{
				.type   = CX23885_VMUX_COMPONENT,
				.vmux   = CX25840_COMPONENT_ON |
					CX25840_VIN1_CH1 |
					CX25840_VIN6_CH2 |
					CX25840_VIN7_CH3,
				.amux   = CX25840_AUDIO7,
			},
		},
	},
	[CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL] = {
		.name		= "TerraTec Cinergy T PCIe Dual",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TEVII_S471] = {
		.name		= "TeVii S471",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_PROF_8000] = {
		.name		= "Prof Revolution DVB-S2 8000",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR4400] = {
		.name		= "Hauppauge WinTV-HVR4400/HVR5500",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_NXP_TDA18271,
		.tuner_addr	= 0x60, /* 0xc0 >> 1 */
		.tuner_bus	= 1,
	},
	[CX23885_BOARD_HAUPPAUGE_STARBURST] = {
		.name		= "Hauppauge WinTV Starburst",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_AVERMEDIA_HC81R] = {
		.name		= "AVerTV Hybrid Express Slim HC81R",
		.tuner_type	= TUNER_XC2028,
		.tuner_addr	= 0x61, /* 0xc2 >> 1 */
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   = CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2 |
				  CX25840_NONE0_CH3 |
				  CX25840_NONE1_CH3,
			.amux   = CX25840_AUDIO8,
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
		.subdevice = 0x8888,
		.card      = CX23885_BOARD_TBS_6980,
	}, {
		.subvendor = 0x6981,
		.subdevice = 0x8888,
		.card      = CX23885_BOARD_TBS_6981,
	}, {
		.subvendor = 0xd470,
		.subdevice = 0x9022,
		.card      = CX23885_BOARD_TEVII_S470,
	}, {
		.subvendor = 0x0001,
		.subdevice = 0x2005,
		.card      = CX23885_BOARD_DVBWORLD_2005,
	}, {
		.subvendor = 0x1b55,
		.subdevice = 0x2a2c,
		.card      = CX23885_BOARD_NETUP_DUAL_DVBS2_CI,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2211,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1270,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2215,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1275,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x221d,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1275,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2251,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1255,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2259,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1255_22111,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2291,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2295,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2299,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x229d,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210, /* HVR1215 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f0,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f1,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1255,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f2,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1275,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f3,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210, /* HVR1215 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f4,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f5,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210, /* HVR1215 */
	}, {
		.subvendor = 0x14f1,
		.subdevice = 0x8651,
		.card      = CX23885_BOARD_MYGICA_X8506,
	}, {
		.subvendor = 0x14f1,
		.subdevice = 0x8657,
		.card      = CX23885_BOARD_MAGICPRO_PROHDTVE2,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x8541,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1850,
	}, {
		.subvendor = 0x1858,
		.subdevice = 0xe800,
		.card      = CX23885_BOARD_COMPRO_VIDEOMATE_E800,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x8551,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1290,
	}, {
		.subvendor = 0x14f1,
		.subdevice = 0x8578,
		.card      = CX23885_BOARD_MYGICA_X8558PRO,
	}, {
		.subvendor = 0x107d,
		.subdevice = 0x6f22,
		.card      = CX23885_BOARD_LEADTEK_WINFAST_PXTV1200,
	}, {
		.subvendor = 0x5654,
		.subdevice = 0x2390,
		.card      = CX23885_BOARD_GOTVIEW_X5_3D_HYBRID,
	}, {
		.subvendor = 0x1b55,
		.subdevice = 0xe2e4,
		.card      = CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF,
	}, {
		.subvendor = 0x14f1,
		.subdevice = 0x8502,
		.card      = CX23885_BOARD_MYGICA_X8507,
	}, {
		.subvendor = 0x153b,
		.subdevice = 0x117e,
		.card      = CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL,
	}, {
		.subvendor = 0xd471,
		.subdevice = 0x9022,
		.card      = CX23885_BOARD_TEVII_S471,
	}, {
		.subvendor = 0x8000,
		.subdevice = 0x3034,
		.card      = CX23885_BOARD_PROF_8000,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xc108,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR4400, /* Hauppauge WinTV HVR-4400 (Model 121xxx, Hybrid DVB-T/S2, IR) */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xc138,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR4400, /* Hauppauge WinTV HVR-5500 (Model 121xxx, Hybrid DVB-T/C/S2, IR) */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xc12a,
		.card      = CX23885_BOARD_HAUPPAUGE_STARBURST, /* Hauppauge WinTV Starburst (Model 121x00, DVB-S2, IR) */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xc1f8,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR4400, /* Hauppauge WinTV HVR-5500 (Model 121xxx, Hybrid DVB-T/C/S2, IR) */
	}, {
		.subvendor = 0x1461,
		.subdevice = 0xd939,
		.card      = CX23885_BOARD_AVERMEDIA_HC81R,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7133,
		.card      = CX23885_BOARD_HAUPPAUGE_IMPACTVCBE,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7137,
		.card      = CX23885_BOARD_HAUPPAUGE_IMPACTVCBE,
	}, {
		.subvendor = 0x18ac,
		.subdevice = 0xdb98,
		.card      = CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x9580,
		.card      = CX23885_BOARD_DVBSKY_T9580,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x980c,
		.card      = CX23885_BOARD_DVBSKY_T980C,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x950c,
		.card      = CX23885_BOARD_DVBSKY_S950C,
	}, {
		.subvendor = 0x13c2,
		.subdevice = 0x3013,
		.card      = CX23885_BOARD_TT_CT2_4500_CI,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x0950,
		.card      = CX23885_BOARD_DVBSKY_S950,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x0952,
		.card      = CX23885_BOARD_DVBSKY_S952,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x0982,
		.card      = CX23885_BOARD_DVBSKY_T982,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xf038,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR5525,
	}, {
		.subvendor = 0x1576,
		.subdevice = 0x0260,
		.card      = CX23885_BOARD_VIEWCAST_260E,
	}, {
		.subvendor = 0x1576,
		.subdevice = 0x0460,
		.card      = CX23885_BOARD_VIEWCAST_460E,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x6a28,
		.card      = CX23885_BOARD_HAUPPAUGE_QUADHD_DVB, /* Tuner Pair 1 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x6b28,
		.card      = CX23885_BOARD_HAUPPAUGE_QUADHD_DVB, /* Tuner Pair 2 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x6a18,
		.card      = CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC, /* Tuner Pair 1 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x6b18,
		.card      = CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC, /* Tuner Pair 2 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2a18,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1265_K4, /* Hauppauge WinTV HVR-1265 (Model 161xx1, Hybrid ATSC/QAM-B) */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xf02a,
		.card      = CX23885_BOARD_HAUPPAUGE_STARBURST2,
	}, {
		.subvendor = 0x1461,
		.subdevice = 0x3100,
		.card      = CX23885_BOARD_AVERMEDIA_CE310B,
	},
};
const unsigned int cx23885_idcount = ARRAY_SIZE(cx23885_subids);

void cx23885_card_list(struct cx23885_dev *dev)
{
	int i;

	if (0 == dev->pci->subsystem_vendor &&
	    0 == dev->pci->subsystem_device) {
		pr_info("%s: Board has no valid PCIe Subsystem ID and can't\n"
			"%s: be autodetected. Pass card=<n> insmod option\n"
			"%s: to workaround that. Redirect complaints to the\n"
			"%s: vendor of the TV card.  Best regards,\n"
			"%s:         -- tux\n",
			dev->name, dev->name, dev->name, dev->name, dev->name);
	} else {
		pr_info("%s: Your board isn't known (yet) to the driver.\n"
			"%s: Try to pick one of the existing card configs via\n"
			"%s: card=<n> insmod option.  Updating to the latest\n"
			"%s: version might help as well.\n",
			dev->name, dev->name, dev->name, dev->name);
	}
	pr_info("%s: Here is a list of valid choices for the card=<n> insmod option:\n",
	       dev->name);
	for (i = 0; i < cx23885_bcount; i++)
		pr_info("%s:    card=%d -> %s\n",
			dev->name, i, cx23885_boards[i].name);
}

static void viewcast_eeprom(struct cx23885_dev *dev, u8 *eeprom_data)
{
	u32 sn;

	/* The serial number record begins with tag 0x59 */
	if (*(eeprom_data + 0x00) != 0x59) {
		pr_info("%s() eeprom records are undefined, no serial number\n",
			__func__);
		return;
	}

	sn =	(*(eeprom_data + 0x06) << 24) |
		(*(eeprom_data + 0x05) << 16) |
		(*(eeprom_data + 0x04) << 8) |
		(*(eeprom_data + 0x03));

	pr_info("%s: card '%s' sn# MM%d\n",
		dev->name,
		cx23885_boards[dev->board].name,
		sn);
}

static void hauppauge_eeprom(struct cx23885_dev *dev, u8 *eeprom_data)
{
	struct tveeprom tv;

	tveeprom_hauppauge_analog(&tv, eeprom_data);

	/* Make sure we support the board model */
	switch (tv.model) {
	case 22001:
		/* WinTV-HVR1270 (PCIe, Retail, half height)
		 * ATSC/QAM and basic analog, IR Blast */
	case 22009:
		/* WinTV-HVR1210 (PCIe, Retail, half height)
		 * DVB-T and basic analog, IR Blast */
	case 22011:
		/* WinTV-HVR1270 (PCIe, Retail, half height)
		 * ATSC/QAM and basic analog, IR Recv */
	case 22019:
		/* WinTV-HVR1210 (PCIe, Retail, half height)
		 * DVB-T and basic analog, IR Recv */
	case 22021:
		/* WinTV-HVR1275 (PCIe, Retail, half height)
		 * ATSC/QAM and basic analog, IR Recv */
	case 22029:
		/* WinTV-HVR1210 (PCIe, Retail, half height)
		 * DVB-T and basic analog, IR Recv */
	case 22101:
		/* WinTV-HVR1270 (PCIe, Retail, full height)
		 * ATSC/QAM and basic analog, IR Blast */
	case 22109:
		/* WinTV-HVR1210 (PCIe, Retail, full height)
		 * DVB-T and basic analog, IR Blast */
	case 22111:
		/* WinTV-HVR1270 (PCIe, Retail, full height)
		 * ATSC/QAM and basic analog, IR Recv */
	case 22119:
		/* WinTV-HVR1210 (PCIe, Retail, full height)
		 * DVB-T and basic analog, IR Recv */
	case 22121:
		/* WinTV-HVR1275 (PCIe, Retail, full height)
		 * ATSC/QAM and basic analog, IR Recv */
	case 22129:
		/* WinTV-HVR1210 (PCIe, Retail, full height)
		 * DVB-T and basic analog, IR Recv */
	case 71009:
		/* WinTV-HVR1200 (PCIe, Retail, full height)
		 * DVB-T and basic analog */
	case 71100:
		/* WinTV-ImpactVCB-e (PCIe, Retail, half height)
		 * Basic analog */
	case 71359:
		/* WinTV-HVR1200 (PCIe, OEM, half height)
		 * DVB-T and basic analog */
	case 71439:
		/* WinTV-HVR1200 (PCIe, OEM, half height)
		 * DVB-T and basic analog */
	case 71449:
		/* WinTV-HVR1200 (PCIe, OEM, full height)
		 * DVB-T and basic analog */
	case 71939:
		/* WinTV-HVR1200 (PCIe, OEM, half height)
		 * DVB-T and basic analog */
	case 71949:
		/* WinTV-HVR1200 (PCIe, OEM, full height)
		 * DVB-T and basic analog */
	case 71959:
		/* WinTV-HVR1200 (PCIe, OEM, full height)
		 * DVB-T and basic analog */
	case 71979:
		/* WinTV-HVR1200 (PCIe, OEM, half height)
		 * DVB-T and basic analog */
	case 71999:
		/* WinTV-HVR1200 (PCIe, OEM, full height)
		 * DVB-T and basic analog */
	case 76601:
		/* WinTV-HVR1800lp (PCIe, Retail, No IR, Dual
			channel ATSC and MPEG2 HW Encoder */
	case 77001:
		/* WinTV-HVR1500 (Express Card, OEM, No IR, ATSC
			and Basic analog */
	case 77011:
		/* WinTV-HVR1500 (Express Card, Retail, No IR, ATSC
			and Basic analog */
	case 77041:
		/* WinTV-HVR1500Q (Express Card, OEM, No IR, ATSC/QAM
			and Basic analog */
	case 77051:
		/* WinTV-HVR1500Q (Express Card, Retail, No IR, ATSC/QAM
			and Basic analog */
	case 78011:
		/* WinTV-HVR1800 (PCIe, Retail, 3.5mm in, IR, No FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 78501:
		/* WinTV-HVR1800 (PCIe, OEM, RCA in, No IR, FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 78521:
		/* WinTV-HVR1800 (PCIe, OEM, RCA in, No IR, FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 78531:
		/* WinTV-HVR1800 (PCIe, OEM, RCA in, No IR, No FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 78631:
		/* WinTV-HVR1800 (PCIe, OEM, No IR, No FM,
			Dual channel ATSC and MPEG2 HW Encoder */
	case 79001:
		/* WinTV-HVR1250 (PCIe, Retail, IR, full height,
			ATSC and Basic analog */
	case 79101:
		/* WinTV-HVR1250 (PCIe, Retail, IR, half height,
			ATSC and Basic analog */
	case 79501:
		/* WinTV-HVR1250 (PCIe, No IR, half height,
			ATSC [at least] and Basic analog) */
	case 79561:
		/* WinTV-HVR1250 (PCIe, OEM, No IR, half height,
			ATSC and Basic analog */
	case 79571:
		/* WinTV-HVR1250 (PCIe, OEM, No IR, full height,
		 ATSC and Basic analog */
	case 79671:
		/* WinTV-HVR1250 (PCIe, OEM, No IR, half height,
			ATSC and Basic analog */
	case 80019:
		/* WinTV-HVR1400 (Express Card, Retail, IR,
		 * DVB-T and Basic analog */
	case 81509:
		/* WinTV-HVR1700 (PCIe, OEM, No IR, half height)
		 * DVB-T and MPEG2 HW Encoder */
	case 81519:
		/* WinTV-HVR1700 (PCIe, OEM, No IR, full height)
		 * DVB-T and MPEG2 HW Encoder */
		break;
	case 85021:
		/* WinTV-HVR1850 (PCIe, Retail, 3.5mm in, IR, FM,
			Dual channel ATSC and MPEG2 HW Encoder */
		break;
	case 85721:
		/* WinTV-HVR1290 (PCIe, OEM, RCA in, IR,
			Dual channel ATSC and Basic analog */
	case 121019:
		/* WinTV-HVR4400 (PCIe, DVB-S2, DVB-C/T) */
		break;
	case 121029:
		/* WinTV-HVR5500 (PCIe, DVB-S2, DVB-C/T) */
		break;
	case 150329:
		/* WinTV-HVR5525 (PCIe, DVB-S/S2, DVB-T/T2/C) */
		break;
	case 161111:
		/* WinTV-HVR-1265 K4 (PCIe, Analog/ATSC/QAM-B) */
		break;
	case 166100: /* 888 version, hybrid */
	case 166200: /* 885 version, DVB only */
		/* WinTV-QuadHD (DVB) Tuner Pair 1 (PCIe, IR, half height,
		   DVB-T/T2/C, DVB-T/T2/C */
		break;
	case 166101: /* 888 version, hybrid */
	case 166201: /* 885 version, DVB only */
		/* WinTV-QuadHD (DVB) Tuner Pair 2 (PCIe, IR, half height,
		   DVB-T/T2/C, DVB-T/T2/C */
		break;
	case 165100: /* 888 version, hybrid */
	case 165200: /* 885 version, digital only */
		/* WinTV-QuadHD (ATSC) Tuner Pair 1 (PCIe, IR, half height,
		 * ATSC/QAM-B, ATSC/QAM-B */
		break;
	case 165101: /* 888 version, hybrid */
	case 165201: /* 885 version, digital only */
		/* WinTV-QuadHD (ATSC) Tuner Pair 2 (PCIe, IR, half height,
		 * ATSC/QAM-B, ATSC/QAM-B */
		break;
	default:
		pr_warn("%s: warning: unknown hauppauge model #%d\n",
			dev->name, tv.model);
		break;
	}

	pr_info("%s: hauppauge eeprom: model=%d\n",
		dev->name, tv.model);
}

/* Some TBS cards require initing a chip using a bitbanged SPI attached
   to the cx23885 gpio's. If this chip doesn't get init'ed the demod
   doesn't respond to any command. */
static void tbs_card_init(struct cx23885_dev *dev)
{
	int i;
	static const u8 buf[] = {
		0xe0, 0x06, 0x66, 0x33, 0x65,
		0x01, 0x17, 0x06, 0xde};

	switch (dev->board) {
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		cx_set(GP0_IO, 0x00070007);
		usleep_range(1000, 10000);
		cx_clear(GP0_IO, 2);
		usleep_range(1000, 10000);
		for (i = 0; i < 9 * 8; i++) {
			cx_clear(GP0_IO, 7);
			usleep_range(1000, 10000);
			cx_set(GP0_IO,
				((buf[i >> 3] >> (7 - (i & 7))) & 1) | 4);
			usleep_range(1000, 10000);
		}
		cx_set(GP0_IO, 7);
		break;
	}
}

int cx23885_tuner_callback(void *priv, int component, int command, int arg)
{
	struct cx23885_tsport *port = priv;
	struct cx23885_dev *dev = port->dev;
	u32 bitmask = 0;

	if ((command == XC2028_RESET_CLK) || (command == XC2028_I2C_FLUSH))
		return 0;

	if (command != 0) {
		pr_err("%s(): Unknown command 0x%x.\n",
		       __func__, command);
		return -EINVAL;
	}

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_LEADTEK_WINFAST_PXTV1200:
		/* Tuner Reset Command */
		bitmask = 0x04;
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		/* Two identical tuners on two different i2c buses,
		 * we need to reset the correct gpio. */
		if (port->nr == 1)
			bitmask = 0x01;
		else if (port->nr == 2)
			bitmask = 0x04;
		break;
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
		/* Tuner Reset Command */
		bitmask = 0x02;
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		altera_ci_tuner_reset(dev, port->nr);
		break;
	case CX23885_BOARD_AVERMEDIA_HC81R:
		/* XC3028L Reset Command */
		bitmask = 1 << 2;
		break;
	}

	if (bitmask) {
		/* Drive the tuner into reset and back out */
		cx_clear(GP0_IO, bitmask);
		mdelay(200);
		cx_set(GP0_IO, bitmask);
	}

	return 0;
}

void cx23885_gpio_setup(struct cx23885_dev *dev)
{
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		/* GPIO-0 cx24227 demodulator reset */
		cx_set(GP0_IO, 0x00010001); /* Bring the part out of reset */
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
		/* GPIO-0 cx24227 demodulator */
		/* GPIO-2 xc3028 tuner */

		/* Put the parts into reset */
		cx_set(GP0_IO, 0x00050000);
		cx_clear(GP0_IO, 0x00000005);
		msleep(5);

		/* Bring the parts out of reset */
		cx_set(GP0_IO, 0x00050005);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
		/* GPIO-0 cx24227 demodulator reset */
		/* GPIO-2 xc5000 tuner reset */
		cx_set(GP0_IO, 0x00050005); /* Bring the part out of reset */
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
		/* GPIO-0 656_CLK */
		/* GPIO-1 656_D0 */
		/* GPIO-2 8295A Reset */
		/* GPIO-3-10 cx23417 data0-7 */
		/* GPIO-11-14 cx23417 addr0-3 */
		/* GPIO-15-18 cx23417 READY, CS, RD, WR */
		/* GPIO-19 IR_RX */

		/* CX23417 GPIO's */
		/* EIO15 Zilog Reset */
		/* EIO14 S5H1409/CX24227 Reset */
		mc417_gpio_enable(dev, GPIO_15 | GPIO_14, 1);

		/* Put the demod into reset and protect the eeprom */
		mc417_gpio_clear(dev, GPIO_15 | GPIO_14);
		msleep(100);

		/* Bring the demod and blaster out of reset */
		mc417_gpio_set(dev, GPIO_15 | GPIO_14);
		msleep(100);

		/* Force the TDA8295A into reset and back */
		cx23885_gpio_enable(dev, GPIO_2, 1);
		cx23885_gpio_set(dev, GPIO_2);
		msleep(20);
		cx23885_gpio_clear(dev, GPIO_2);
		msleep(20);
		cx23885_gpio_set(dev, GPIO_2);
		msleep(20);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
		/* GPIO-0 tda10048 demodulator reset */
		/* GPIO-2 tda18271 tuner reset */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x00050000);
		msleep(20);
		cx_clear(GP0_IO, 0x00000005);
		msleep(20);
		cx_set(GP0_IO, 0x00050005);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
		/* GPIO-0 TDA10048 demodulator reset */
		/* GPIO-2 TDA8295A Reset */
		/* GPIO-3-10 cx23417 data0-7 */
		/* GPIO-11-14 cx23417 addr0-3 */
		/* GPIO-15-18 cx23417 READY, CS, RD, WR */

		/* The following GPIO's are on the interna AVCore (cx25840) */
		/* GPIO-19 IR_RX */
		/* GPIO-20 IR_TX 416/DVBT Select */
		/* GPIO-21 IIS DAT */
		/* GPIO-22 IIS WCLK */
		/* GPIO-23 IIS BCLK */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x00050000);
		msleep(20);
		cx_clear(GP0_IO, 0x00000005);
		msleep(20);
		cx_set(GP0_IO, 0x00050005);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
		/* GPIO-0  Dibcom7000p demodulator reset */
		/* GPIO-2  xc3028L tuner reset */
		/* GPIO-13 LED */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x00050000);
		msleep(20);
		cx_clear(GP0_IO, 0x00000005);
		msleep(20);
		cx_set(GP0_IO, 0x00050005);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP:
		/* GPIO-0 xc5000 tuner reset i2c bus 0 */
		/* GPIO-1 s5h1409 demod reset i2c bus 0 */
		/* GPIO-2 xc5000 tuner reset i2c bus 1 */
		/* GPIO-3 s5h1409 demod reset i2c bus 0 */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x000f0000);
		msleep(20);
		cx_clear(GP0_IO, 0x0000000f);
		msleep(20);
		cx_set(GP0_IO, 0x000f000f);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		/* GPIO-0 portb xc3028 reset */
		/* GPIO-1 portb zl10353 reset */
		/* GPIO-2 portc xc3028 reset */
		/* GPIO-3 portc zl10353 reset */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x000f0000);
		msleep(20);
		cx_clear(GP0_IO, 0x0000000f);
		msleep(20);
		cx_set(GP0_IO, 0x000f000f);
		break;
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_LEADTEK_WINFAST_PXTV1200:
		/* GPIO-2  xc3028 tuner reset */

		/* The following GPIO's are on the internal AVCore (cx25840) */
		/* GPIO-?  zl10353 demod reset */

		/* Put the parts into reset and back */
		cx_set(GP0_IO, 0x00040000);
		msleep(20);
		cx_clear(GP0_IO, 0x00000004);
		msleep(20);
		cx_set(GP0_IO, 0x00040004);
		break;
	case CX23885_BOARD_TBS_6920:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_PROF_8000:
		cx_write(MC417_CTL, 0x00000036);
		cx_write(MC417_OEN, 0x00001000);
		cx_set(MC417_RWD, 0x00000002);
		msleep(200);
		cx_clear(MC417_RWD, 0x00000800);
		msleep(200);
		cx_set(MC417_RWD, 0x00000800);
		msleep(200);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		/* GPIO-0 INTA from CiMax1
		   GPIO-1 INTB from CiMax2
		   GPIO-2 reset chips
		   GPIO-3 to GPIO-10 data/addr for CA
		   GPIO-11 ~CS0 to CiMax1
		   GPIO-12 ~CS1 to CiMax2
		   GPIO-13 ADL0 load LSB addr
		   GPIO-14 ADL1 load MSB addr
		   GPIO-15 ~RDY from CiMax
		   GPIO-17 ~RD to CiMax
		   GPIO-18 ~WR to CiMax
		 */
		cx_set(GP0_IO, 0x00040000); /* GPIO as out */
		/* GPIO1 and GPIO2 as INTA and INTB from CiMaxes, reset low */
		cx_clear(GP0_IO, 0x00030004);
		msleep(100);/* reset delay */
		cx_set(GP0_IO, 0x00040004); /* GPIO as out, reset high */
		cx_write(MC417_CTL, 0x00000037);/* enable GPIO3-18 pins */
		/* GPIO-15 IN as ~ACK, rest as OUT */
		cx_write(MC417_OEN, 0x00001000);
		/* ~RD, ~WR high; ADL0, ADL1 low; ~CS0, ~CS1 high */
		cx_write(MC417_RWD, 0x0000c300);
		/* enable irq */
		cx_write(GPIO_ISM, 0x00000000);/* INTERRUPTS active low*/
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
		/* GPIO-5 RF Control: 0 = RF1 Terrestrial, 1 = RF2 Cable */
		/* GPIO-6 I2C Gate which can isolate the demod from the bus */
		/* GPIO-9 Demod reset */

		/* Put the parts into reset and back */
		cx23885_gpio_enable(dev, GPIO_9 | GPIO_6 | GPIO_5, 1);
		cx23885_gpio_set(dev, GPIO_9 | GPIO_6 | GPIO_5);
		cx23885_gpio_clear(dev, GPIO_9);
		msleep(20);
		cx23885_gpio_set(dev, GPIO_9);
		break;
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
	case CX23885_BOARD_MYGICA_X8507:
		/* GPIO-0 (0)Analog / (1)Digital TV */
		/* GPIO-1 reset XC5000 */
		/* GPIO-2 demod reset */
		cx23885_gpio_enable(dev, GPIO_0 | GPIO_1 | GPIO_2, 1);
		cx23885_gpio_clear(dev, GPIO_1 | GPIO_2);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_0 | GPIO_1 | GPIO_2);
		msleep(100);
		break;
	case CX23885_BOARD_MYGICA_X8558PRO:
		/* GPIO-0 reset first ATBM8830 */
		/* GPIO-1 reset second ATBM8830 */
		cx23885_gpio_enable(dev, GPIO_0 | GPIO_1, 1);
		cx23885_gpio_clear(dev, GPIO_0 | GPIO_1);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_0 | GPIO_1);
		msleep(100);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		/* GPIO-0 656_CLK */
		/* GPIO-1 656_D0 */
		/* GPIO-2 Wake# */
		/* GPIO-3-10 cx23417 data0-7 */
		/* GPIO-11-14 cx23417 addr0-3 */
		/* GPIO-15-18 cx23417 READY, CS, RD, WR */
		/* GPIO-19 IR_RX */
		/* GPIO-20 C_IR_TX */
		/* GPIO-21 I2S DAT */
		/* GPIO-22 I2S WCLK */
		/* GPIO-23 I2S BCLK */
		/* ALT GPIO: EXP GPIO LATCH */

		/* CX23417 GPIO's */
		/* GPIO-14 S5H1411/CX24228 Reset */
		/* GPIO-13 EEPROM write protect */
		mc417_gpio_enable(dev, GPIO_14 | GPIO_13, 1);

		/* Put the demod into reset and protect the eeprom */
		mc417_gpio_clear(dev, GPIO_14 | GPIO_13);
		msleep(100);

		/* Bring the demod out of reset */
		mc417_gpio_set(dev, GPIO_14);
		msleep(100);

		/* CX24228 GPIO */
		/* Connected to IF / Mux */
		break;
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
		cx_set(GP0_IO, 0x00010001); /* Bring the part out of reset */
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		/* GPIO-0 ~INT in
		   GPIO-1 TMS out
		   GPIO-2 ~reset chips out
		   GPIO-3 to GPIO-10 data/addr for CA in/out
		   GPIO-11 ~CS out
		   GPIO-12 ADDR out
		   GPIO-13 ~WR out
		   GPIO-14 ~RD out
		   GPIO-15 ~RDY in
		   GPIO-16 TCK out
		   GPIO-17 TDO in
		   GPIO-18 TDI out
		 */
		cx_set(GP0_IO, 0x00060000); /* GPIO-1,2 as out */
		/* GPIO-0 as INT, reset & TMS low */
		cx_clear(GP0_IO, 0x00010006);
		msleep(100);/* reset delay */
		cx_set(GP0_IO, 0x00000004); /* reset high */
		cx_write(MC417_CTL, 0x00000037);/* enable GPIO-3..18 pins */
		/* GPIO-17 is TDO in, GPIO-15 is ~RDY in, rest is out */
		cx_write(MC417_OEN, 0x00005000);
		/* ~RD, ~WR high; ADDR low; ~CS high */
		cx_write(MC417_RWD, 0x00000d00);
		/* enable irq */
		cx_write(GPIO_ISM, 0x00000000);/* INTERRUPTS active low*/
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR4400:
	case CX23885_BOARD_HAUPPAUGE_STARBURST:
		/* GPIO-8 tda10071 demod reset */
		/* GPIO-9 si2165 demod reset (only HVR4400/HVR5500)*/

		/* Put the parts into reset and back */
		cx23885_gpio_enable(dev, GPIO_8 | GPIO_9, 1);

		cx23885_gpio_clear(dev, GPIO_8 | GPIO_9);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_8 | GPIO_9);
		msleep(100);

		break;
	case CX23885_BOARD_AVERMEDIA_HC81R:
		cx_clear(MC417_CTL, 1);
		/* GPIO-0,1,2 setup direction as output */
		cx_set(GP0_IO, 0x00070000);
		usleep_range(10000, 11000);
		/* AF9013 demod reset */
		cx_set(GP0_IO, 0x00010001);
		usleep_range(10000, 11000);
		cx_clear(GP0_IO, 0x00010001);
		usleep_range(10000, 11000);
		cx_set(GP0_IO, 0x00010001);
		usleep_range(10000, 11000);
		/* demod tune? */
		cx_clear(GP0_IO, 0x00030003);
		usleep_range(10000, 11000);
		cx_set(GP0_IO, 0x00020002);
		usleep_range(10000, 11000);
		cx_set(GP0_IO, 0x00010001);
		usleep_range(10000, 11000);
		cx_clear(GP0_IO, 0x00020002);
		/* XC3028L tuner reset */
		cx_set(GP0_IO, 0x00040004);
		cx_clear(GP0_IO, 0x00040004);
		cx_set(GP0_IO, 0x00040004);
		msleep(60);
		break;
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		/* enable GPIO3-18 pins */
		cx_write(MC417_CTL, 0x00000037);
		cx23885_gpio_enable(dev, GPIO_2 | GPIO_11, 1);
		cx23885_gpio_clear(dev, GPIO_2 | GPIO_11);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_2 | GPIO_11);
		break;
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
		/*
		 * GPIO-0 INTA from CiMax, input
		 * GPIO-1 reset CiMax, output, high active
		 * GPIO-2 reset demod, output, low active
		 * GPIO-3 to GPIO-10 data/addr for CAM
		 * GPIO-11 ~CS0 to CiMax1
		 * GPIO-12 ~CS1 to CiMax2
		 * GPIO-13 ADL0 load LSB addr
		 * GPIO-14 ADL1 load MSB addr
		 * GPIO-15 ~RDY from CiMax
		 * GPIO-17 ~RD to CiMax
		 * GPIO-18 ~WR to CiMax
		 */

		cx_set(GP0_IO, 0x00060002); /* GPIO 1/2 as output */
		cx_clear(GP0_IO, 0x00010004); /* GPIO 0 as input */
		msleep(100); /* reset delay */
		cx_set(GP0_IO, 0x00060004); /* GPIO as out, reset high */
		cx_clear(GP0_IO, 0x00010002);
		cx_write(MC417_CTL, 0x00000037); /* enable GPIO3-18 pins */

		/* GPIO-15 IN as ~ACK, rest as OUT */
		cx_write(MC417_OEN, 0x00001000);

		/* ~RD, ~WR high; ADL0, ADL1 low; ~CS0, ~CS1 high */
		cx_write(MC417_RWD, 0x0000c300);

		/* enable irq */
		cx_write(GPIO_ISM, 0x00000000); /* INTERRUPTS active low */
		break;
	case CX23885_BOARD_DVBSKY_S950:
		cx23885_gpio_enable(dev, GPIO_2, 1);
		cx23885_gpio_clear(dev, GPIO_2);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_2);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_STARBURST2:
		/*
		 * HVR5525 GPIO Details:
		 *  GPIO-00 IR_WIDE
		 *  GPIO-02 wake#
		 *  GPIO-03 VAUX Pres.
		 *  GPIO-07 PROG#
		 *  GPIO-08 SAT_RESN
		 *  GPIO-09 TER_RESN
		 *  GPIO-10 B2_SENSE
		 *  GPIO-11 B1_SENSE
		 *  GPIO-15 IR_LED_STATUS
		 *  GPIO-19 IR_NARROW
		 *  GPIO-20 Blauster1
		 *  ALTGPIO VAUX_SWITCH
		 *  AUX_PLL_CLK : Blaster2
		 */
		/* Put the parts into reset and back */
		cx23885_gpio_enable(dev, GPIO_8 | GPIO_9, 1);
		cx23885_gpio_clear(dev, GPIO_8 | GPIO_9);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_8 | GPIO_9);
		msleep(100);
		break;
	case CX23885_BOARD_VIEWCAST_260E:
	case CX23885_BOARD_VIEWCAST_460E:
		/* For documentation purposes, it's worth noting that this
		 * card does not have any GPIO's connected to subcomponents.
		 */
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885:
		/*
		 * GPIO-08 TER1_RESN
		 * GPIO-09 TER2_RESN
		 */
		/* Put the parts into reset and back */
		cx23885_gpio_enable(dev, GPIO_8 | GPIO_9, 1);
		cx23885_gpio_clear(dev, GPIO_8 | GPIO_9);
		msleep(100);
		cx23885_gpio_set(dev, GPIO_8 | GPIO_9);
		msleep(100);
		break;
	}
}

int cx23885_ir_init(struct cx23885_dev *dev)
{
	static struct v4l2_subdev_io_pin_config ir_rxtx_pin_cfg[] = {
		{
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_INPUT),
			.pin	  = CX23885_PIN_IR_RX_GPIO19,
			.function = CX23885_PAD_IR_RX,
			.value	  = 0,
			.strength = CX25840_PIN_DRIVE_MEDIUM,
		}, {
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_OUTPUT),
			.pin	  = CX23885_PIN_IR_TX_GPIO20,
			.function = CX23885_PAD_IR_TX,
			.value	  = 0,
			.strength = CX25840_PIN_DRIVE_MEDIUM,
		}
	};
	const size_t ir_rxtx_pin_cfg_count = ARRAY_SIZE(ir_rxtx_pin_cfg);

	static struct v4l2_subdev_io_pin_config ir_rx_pin_cfg[] = {
		{
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_INPUT),
			.pin	  = CX23885_PIN_IR_RX_GPIO19,
			.function = CX23885_PAD_IR_RX,
			.value	  = 0,
			.strength = CX25840_PIN_DRIVE_MEDIUM,
		}
	};
	const size_t ir_rx_pin_cfg_count = ARRAY_SIZE(ir_rx_pin_cfg);

	struct v4l2_subdev_ir_parameters params;
	int ret = 0;
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		/* FIXME: Implement me */
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
		ret = cx23888_ir_probe(dev);
		if (ret)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_888_IR);
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rx_pin_cfg_count, ir_rx_pin_cfg);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		ret = cx23888_ir_probe(dev);
		if (ret)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_888_IR);
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rxtx_pin_cfg_count, ir_rxtx_pin_cfg);
		/*
		 * For these boards we need to invert the Tx output via the
		 * IR controller to have the LED off while idle
		 */
		v4l2_subdev_call(dev->sd_ir, ir, tx_g_parameters, &params);
		params.enable = false;
		params.shutdown = false;
		params.invert_level = true;
		v4l2_subdev_call(dev->sd_ir, ir, tx_s_parameters, &params);
		params.shutdown = true;
		v4l2_subdev_call(dev->sd_ir, ir, tx_s_parameters, &params);
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
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
		if (!enable_885_ir)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_AV_CORE);
		if (dev->sd_ir == NULL) {
			ret = -ENODEV;
			break;
		}
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rx_pin_cfg_count, ir_rx_pin_cfg);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		if (!enable_885_ir)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_AV_CORE);
		if (dev->sd_ir == NULL) {
			ret = -ENODEV;
			break;
		}
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rxtx_pin_cfg_count, ir_rxtx_pin_cfg);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		request_module("ir-kbd-i2c");
		break;
	}

	return ret;
}

void cx23885_ir_fini(struct cx23885_dev *dev)
{
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		cx23885_irq_remove(dev, PCI_MSK_IR);
		cx23888_ir_remove(dev);
		dev->sd_ir = NULL;
		break;
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
		cx23885_irq_remove(dev, PCI_MSK_AV_CORE);
		/* sd_ir is a duplicate pointer to the AV Core, just clear it */
		dev->sd_ir = NULL;
		break;
	}
}

static int netup_jtag_io(void *device, int tms, int tdi, int read_tdo)
{
	int data;
	int tdo = 0;
	struct cx23885_dev *dev = (struct cx23885_dev *)device;
	/*TMS*/
	data = ((cx_read(GP0_IO)) & (~0x00000002));
	data |= (tms ? 0x00020002 : 0x00020000);
	cx_write(GP0_IO, data);

	/*TDI*/
	data = ((cx_read(MC417_RWD)) & (~0x0000a000));
	data |= (tdi ? 0x00008000 : 0);
	cx_write(MC417_RWD, data);
	if (read_tdo)
		tdo = (data & 0x00004000) ? 1 : 0; /*TDO*/

	cx_write(MC417_RWD, data | 0x00002000);
	udelay(1);
	/*TCK*/
	cx_write(MC417_RWD, data);

	return tdo;
}

void cx23885_ir_pci_int_enable(struct cx23885_dev *dev)
{
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		if (dev->sd_ir)
			cx23885_irq_add_enable(dev, PCI_MSK_IR);
		break;
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
		if (dev->sd_ir)
			cx23885_irq_add_enable(dev, PCI_MSK_AV_CORE);
		break;
	}
}

void cx23885_card_setup(struct cx23885_dev *dev)
{
	struct cx23885_tsport *ts1 = &dev->ts1;
	struct cx23885_tsport *ts2 = &dev->ts2;

	static u8 eeprom[256];

	if (dev->i2c_bus[0].i2c_rc == 0) {
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client,
			      eeprom, sizeof(eeprom));
	}

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		if (dev->i2c_bus[0].i2c_rc == 0) {
			if (eeprom[0x80] != 0x84)
				hauppauge_eeprom(dev, eeprom+0xc0);
			else
				hauppauge_eeprom(dev, eeprom+0x80);
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
		if (dev->i2c_bus[0].i2c_rc == 0)
			hauppauge_eeprom(dev, eeprom+0x80);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR4400:
	case CX23885_BOARD_HAUPPAUGE_STARBURST:
	case CX23885_BOARD_HAUPPAUGE_IMPACTVCBE:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_STARBURST2:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885:
		if (dev->i2c_bus[0].i2c_rc == 0)
			hauppauge_eeprom(dev, eeprom+0xc0);
		break;
	case CX23885_BOARD_VIEWCAST_260E:
	case CX23885_BOARD_VIEWCAST_460E:
		dev->i2c_bus[1].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[1].i2c_client,
			      eeprom, sizeof(eeprom));
		if (dev->i2c_bus[0].i2c_rc == 0)
			viewcast_eeprom(dev, eeprom);
		break;
	}

	switch (dev->board) {
	case CX23885_BOARD_AVERMEDIA_HC81R:
		/* Defaults for VID B */
		ts1->gen_ctrl_val  = 0x4; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		/* Defaults for VID C */
		/* DREQ_POL, SMODE, PUNC_CLK, MCLK_POL Serial bus + punc clk */
		ts2->gen_ctrl_val  = 0x10e;
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val     = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		fallthrough;
	case CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
		/* Defaults for VID B - Analog encoder */
		/* DREQ_POL, SMODE, PUNC_CLK, MCLK_POL Serial bus + punc clk */
		ts1->gen_ctrl_val    = 0x10e;
		ts1->ts_clk_en_val   = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val     = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;

		/* APB_TSVALERR_POL (active low)*/
		ts1->vld_misc_val    = 0x2000;
		ts1->hw_sop_ctrl_val = (0x47 << 16 | 188 << 4 | 0xc);
		cx_write(0x130184, 0xc);

		/* Defaults for VID C */
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_TBS_6920:
		ts1->gen_ctrl_val  = 0x4; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TEVII_S471:
	case CX23885_BOARD_DVBWORLD_2005:
	case CX23885_BOARD_PROF_8000:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		tbs_card_init(dev);
		break;
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
	case CX23885_BOARD_MYGICA_X8507:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_MYGICA_X8558PRO:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR4400:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_STARBURST:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T982:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0x8; /* Serial bus */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_DVBSKY_S952:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xe; /* Serial bus */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_STARBURST2:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
	case CX23885_BOARD_HAUPPAUGE_IMPACTVCBE:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
	default:
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
	}

	/* Certain boards support analog, or require the avcore to be
	 * loaded, ensure this happens.
	 */
	switch (dev->board) {
	case CX23885_BOARD_TEVII_S470:
		/* Currently only enabled for the integrated IR controller */
		if (!enable_885_ir)
			break;
		fallthrough;
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
	case CX23885_BOARD_HAUPPAUGE_IMPACTVCBE:
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_LEADTEK_WINFAST_PXTV1200:
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_MPX885:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_AVERMEDIA_HC81R:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_VIEWCAST_260E:
	case CX23885_BOARD_VIEWCAST_460E:
	case CX23885_BOARD_AVERMEDIA_CE310B:
		dev->sd_cx25840 = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[2].i2c_adap,
				"cx25840", 0x88 >> 1, NULL);
		if (dev->sd_cx25840) {
			/* set host data for clk_freq configuration */
			v4l2_set_subdev_hostdata(dev->sd_cx25840,
						&dev->clk_freq);

			dev->sd_cx25840->grp_id = CX23885_HW_AV_CORE;
			v4l2_subdev_call(dev->sd_cx25840, core, load_fw);
		}
		break;
	}

	switch (dev->board) {
	case CX23885_BOARD_VIEWCAST_260E:
		v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[0].i2c_adap,
				"cs3308", 0x82 >> 1, NULL);
		break;
	case CX23885_BOARD_VIEWCAST_460E:
		/* This cs3308 controls the audio from the breakout cable */
		v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[0].i2c_adap,
				"cs3308", 0x80 >> 1, NULL);
		/* This cs3308 controls the audio from the onboard header */
		v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[0].i2c_adap,
				"cs3308", 0x82 >> 1, NULL);
		break;
	}

	/* AUX-PLL 27MHz CLK */
	switch (dev->board) {
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		netup_initialize(dev);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF: {
		int ret;
		const struct firmware *fw;
		const char *filename = "dvb-netup-altera-01.fw";
		char *action = "configure";
		static struct netup_card_info cinfo;
		struct altera_config netup_config = {
			.dev = dev,
			.action = action,
			.jtag_io = netup_jtag_io,
		};

		netup_initialize(dev);

		netup_get_card_info(&dev->i2c_bus[0].i2c_adap, &cinfo);
		if (netup_card_rev)
			cinfo.rev = netup_card_rev;

		switch (cinfo.rev) {
		case 0x4:
			filename = "dvb-netup-altera-04.fw";
			break;
		default:
			filename = "dvb-netup-altera-01.fw";
			break;
		}
		pr_info("NetUP card rev=0x%x fw_filename=%s\n",
			cinfo.rev, filename);

		ret = request_firmware(&fw, filename, &dev->pci->dev);
		if (ret != 0)
			pr_err("did not find the firmware file '%s'. You can use <kernel_dir>/scripts/get_dvb_firmware to get the firmware.",
			       filename);
		else
			altera_init(&netup_config, fw);

		release_firmware(fw);
		break;
	}
	}
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later
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
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x10380,
		.cdt		= 0x104c0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "ch2",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10400,
		.cdt		= 0x10580,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH06] = {
		.name		= "TS2 C",
		.cmds_start	= 0x10140,
		.ctrl_start	= 0x10440,
		.cdt		= 0x105e0,
		.fifo_start	= 0x6000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH07] = {
		.name		= "TV Audio",
		.cmds_start	= 0x10190,
		.ctrl_start	= 0x10480,
		.cdt		= 0x10a00,
		.fifo_start	= 0x7000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA6_PTR1,
		.ptr2_reg	= DMA6_PTR2,
		.cnt1_reg	= DMA6_CNT1,
		.cnt2_reg	= DMA6_CNT2,
	},
	[SRAM_CH08] = {
		.name		= "ch8",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA7_PTR1,
		.ptr2_reg	= DMA7_PTR2,
		.cnt1_reg	= DMA7_CNT1,
		.cnt2_reg	= DMA7_CNT2,
	},
	[SRAM_CH09] = {
		.name		= "ch9",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA8_PTR1,
		.ptr2_reg	= DMA8_PTR2,
		.cnt1_reg	= DMA8_CNT1,
		.cnt2_reg	= DMA8_CNT2,
	},
};

static struct sram_channel cx23887_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x105b0,
		.cdt		= 0x107b0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "VID A (VBI)",
		.cmds_start	= 0x10050,
		.ctrl_start	= 0x105F0,
		.cdt		= 0x10810,
		.fifo_start	= 0x3000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10630,
		.cdt		= 0x10870,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH06] = {
		.name		= "TS2 C",
		.cmds_start	= 0x10140,
		.ctrl_start	= 0x10670,
		.cdt		= 0x108d0,
		.fifo_start	= 0x6000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH07] = {
		.name		= "TV Audio",
		.cmds_start	= 0x10190,
		.ctrl_start	= 0x106B0,
		.cdt		= 0x10930,
		.fifo_start	= 0x7000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA6_PTR1,
		.ptr2_reg	= DMA6_PTR2,
		.cnt1_reg	= DMA6_CNT1,
		.cnt2_reg	= DMA6_CNT2,
	},
	[SRAM_CH08] = {
		.name		= "ch8",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA7_PTR1,
		.ptr2_reg	= DMA7_PTR2,
		.cnt1_reg	= DMA7_CNT1,
		.cnt2_reg	= DMA7_CNT2,
	},
	[SRAM_CH09] = {
		.name		= "ch9",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA8_PTR1,
		.ptr2_reg	= DMA8_PTR2,
		.cnt1_reg	= DMA8_CNT1,
		.cnt2_reg	= DMA8_CNT2,
	},
};

static void cx23885_irq_add(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask |= mask;

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask |= mask;
	cx_set(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask)
{
	u32 v;
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	v = mask & dev->pci_irqmask;
	if (v)
		cx_set(PCI_INT_MSK, v);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static inline void cx23885_irq_enable_all(struct cx23885_dev *dev)
{
	cx23885_irq_enable(dev, 0xffffffff);
}

void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	cx_clear(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static inline void cx23885_irq_disable_all(struct cx23885_dev *dev)
{
	cx23885_irq_disable(dev, 0xffffffff);
}

void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask &= ~mask;
	cx_clear(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static u32 cx23885_irq_get_mask(struct cx23885_dev *dev)
{
	u32 v;
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	v = cx_read(PCI_INT_MSK);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
	return v;
}

static int cx23885_risc_decode(u32 risc)
{
	static char *instr[16] = {
		[RISC_SYNC    >> 28] = "sync",
		[RISC_WRITE   >> 28] = "write",
		[RISC_WRITEC  >> 28] = "writec",
		[RISC_READ    >> 28] = "read",
		[RISC_READC   >> 28] = "readc",
		[RISC_JUMP    >> 28] = "jump",
		[RISC_SKIP    >> 28] = "skip",
		[RISC_WRITERM >> 28] = "writerm",
		[RISC_WRITECM >> 28] = "writecm",
		[RISC_WRITECR >> 28] = "writecr",
	};
	static int incr[16] = {
		[RISC_WRITE   >> 28] = 3,
		[RISC_JUMP    >> 28] = 3,
		[RISC_SKIP    >> 28] = 1,
		[RISC_SYNC    >> 28] = 1,
		[RISC_WRITERM >> 28] = 3,
		[RISC_WRITECM >> 28] = 3,
		[RISC_WRITECR >> 28] = 4,
	};
	static char *bits[] = {
		"12",   "13",   "14",   "resync",
		"cnt0", "cnt1", "18",   "19",
		"20",   "21",   "22",   "23",
		"irq1", "irq2", "eol",  "sol",
	};
	int i;

	printk(KERN_DEBUG "0x%08x [ %s", risc,
	       instr[risc >> 28] ? instr[risc >> 28] : "INVALID");
	for (i = ARRAY_SIZE(bits) - 1; i >= 0; i--)
		if (risc & (1 << (i + 12)))
			pr_cont(" %s", bits[i]);
	pr_cont(" count=%d ]\n", risc & 0xfff);
	return incr[risc >> 28] ? incr[risc >> 28] : 1;
}

static void cx23885_wakeup(struct cx23885_tsport *port,
			   struct cx23885_dmaqueue *q, u32 count)
{
	struct cx23885_buffer *buf;
	int count_delta;
	int max_buf_done = 5; /* service maximum five buffers */

	do {
		if (list_empty(&q->active))
			return;
		buf = list_entry(q->active.next,
				 struct cx23885_buffer, queue);

		buf->vb.vb2_buf.timestamp = ktime_get_ns();
		buf->vb.sequence = q->count++;
		if (count != (q->count % 65536)) {
			dprintk(1, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
				buf->vb.vb2_buf.index, count, q->count);
		} else {
			dprintk(7, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
				buf->vb.vb2_buf.index, count, q->count);
		}
		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
		max_buf_done--;
		/* count register is 16 bits so apply modulo appropriately */
		count_delta = ((int)count - (int)(q->count % 65536));
	} while ((count_delta > 0) && (max_buf_done > 0));
}

int cx23885_sram_channel_setup(struct cx23885_dev *dev,
				      struct sram_channel *ch,
				      unsigned int bpl, u32 risc)
{
	unsigned int i, lines;
	u32 cdt;

	if (ch->cmds_start == 0) {
		dprintk(1, "%s() Erasing channel [%s]\n", __func__,
			ch->name);
		cx_write(ch->ptr1_reg, 0);
		cx_write(ch->ptr2_reg, 0);
		cx_write(ch->cnt2_reg, 0);
		cx_write(ch->cnt1_reg, 0);
		return 0;
	} else {
		dprintk(1, "%s() Configuring channel [%s]\n", __func__,
			ch->name);
	}

	bpl   = (bpl + 7) & ~7; /* alignment */
	cdt   = ch->cdt;
	lines = ch->fifo_size / bpl;
	if (lines > 6)
		lines = 6;
	BUG_ON(lines < 2);

	cx_write(8 + 0, RISC_JUMP | RISC_CNT_RESET);
	cx_write(8 + 4, 12);
	cx_write(8 + 8, 0);

	/* write CDT */
	for (i = 0; i < lines; i++) {
		dprintk(2, "%s() 0x%08x <- 0x%08x\n", __func__, cdt + 16*i,
			ch->fifo_start + bpl*i);
		cx_write(cdt + 16*i, ch->fifo_start + bpl*i);
		cx_write(cdt + 16*i +  4, 0);
		cx_write(cdt + 16*i +  8, 0);
		cx_write(cdt + 16*i + 12, 0);
	}

	/* write CMDS */
	if (ch->jumponly)
		cx_write(ch->cmds_start + 0, 8);
	else
		cx_write(ch->cmds_start + 0, risc);
	cx_write(ch->cmds_start +  4, 0); /* 64 bits 63-32 */
	cx_write(ch->cmds_start +  8, cdt);
	cx_write(ch->cmds_start + 12, (lines*16) >> 3);
	cx_write(ch->cmds_start + 16, ch->ctrl_start);
	if (ch->jumponly)
		cx_write(ch->cmds_start + 20, 0x80000000 | (64 >> 2));
	else
		cx_write(ch->cmds_start + 20, 64 >> 2);
	for (i = 24; i < 80; i += 4)
		cx_write(ch->cmds_start + i, 0);

	/* fill registers */
	cx_write(ch->ptr1_reg, ch->fifo_start);
	cx_write(ch->ptr2_reg, cdt);
	cx_write(ch->cnt2_reg, (lines*16) >> 3);
	cx_write(ch->cnt1_reg, (bpl >> 3) - 1);

	dprintk(2, "[bridge %d] sram setup %s: bpl=%d lines=%d\n",
		dev->bridge,
		ch->name,
		bpl,
		lines);

	return 0;
}

void cx23885_sram_channel_dump(struct cx23885_dev *dev,
				      struct sram_channel *ch)
{
	static char *name[] = {
		"init risc lo",
		"init risc hi",
		"cdt base",
		"cdt size",
		"iq base",
		"iq size",
		"risc pc lo",
		"risc pc hi",
		"iq wr ptr",
		"iq rd ptr",
		"cdt current",
		"pci target lo",
		"pci target hi",
		"line / byte",
	};
	u32 risc;
	unsigned int i, j, n;

	pr_warn("%s: %s - dma channel status dump\n",
		dev->name, ch->name);
	for (i = 0; i < ARRAY_SIZE(name); i++)
		pr_warn("%s:   cmds: %-15s: 0x%08x\n",
			dev->name, name[i],
			cx_read(ch->cmds_start + 4*i));

	for (i = 0; i < 4; i++) {
		risc = cx_read(ch->cmds_start + 4 * (i + 14));
		pr_warn("%s:   risc%d: ", dev->name, i);
		cx23885_risc_decode(risc);
	}
	for (i = 0; i < (64 >> 2); i += n) {
		risc = cx_read(ch->ctrl_start + 4 * i);
		/* No consideration for bits 63-32 */

		pr_warn("%s:   (0x%08x) iq %x: ", dev->name,
			ch->ctrl_start + 4 * i, i);
		n = cx23885_risc_decode(risc);
		for (j = 1; j < n; j++) {
			risc = cx_read(ch->ctrl_start + 4 * (i + j));
			pr_warn("%s:   iq %x: 0x%08x [ arg #%d ]\n",
				dev->name, i+j, risc, j);
		}
	}

	pr_warn("%s: fifo: 0x%08x -> 0x%x\n",
		dev->name, ch->fifo_start, ch->fifo_start+ch->fifo_size);
	pr_warn("%s: ctrl: 0x%08x -> 0x%x\n",
		dev->name, ch->ctrl_start, ch->ctrl_start + 6*16);
	pr_warn("%s:   ptr1_reg: 0x%08x\n",
		dev->name, cx_read(ch->ptr1_reg));
	pr_warn("%s:   ptr2_reg: 0x%08x\n",
		dev->name, cx_read(ch->ptr2_reg));
	pr_warn("%s:   cnt1_reg: 0x%08x\n",
		dev->name, cx_read(ch->cnt1_reg));
	pr_warn("%s:   cnt2_reg: 0x%08x\n",
		dev->name, cx_read(ch->cnt2_reg));
}

static void cx23885_risc_disasm(struct cx23885_tsport *port,
				struct cx23885_riscmem *risc)
{
	struct cx23885_dev *dev = port->dev;
	unsigned int i, j, n;

	pr_info("%s: risc disasm: %p [dma=0x%08lx]\n",
	       dev->name, risc->cpu, (unsigned long)risc->dma);
	for (i = 0; i < (risc->size >> 2); i += n) {
		pr_info("%s:   %04d: ", dev->name, i);
		n = cx23885_risc_decode(le32_to_cpu(risc->cpu[i]));
		for (j = 1; j < n; j++)
			pr_info("%s:   %04d: 0x%08x [ arg #%d ]\n",
				dev->name, i + j, risc->cpu[i + j], j);
		if (risc->cpu[i] == cpu_to_le32(RISC_JUMP))
			break;
	}
}

static void cx23885_clear_bridge_error(struct cx23885_dev *dev)
{
	uint32_t reg1_val, reg2_val;

	if (!dev->need_dma_reset)
		return;

	reg1_val = cx_read(TC_REQ); /* read-only */
	reg2_val = cx_read(TC_REQ_SET);

	if (reg1_val && reg2_val) {
		cx_write(TC_REQ, reg1_val);
		cx_write(TC_REQ_SET, reg2_val);
		cx_read(VID_B_DMA);
		cx_read(VBI_B_DMA);
		cx_read(VID_C_DMA);
		cx_read(VBI_C_DMA);

		dev_info(&dev->pci->dev,
			"dma in progress detected 0x%08x 0x%08x, clearing\n",
			reg1_val, reg2_val);
	}
}

static void cx23885_shutdown(struct cx23885_dev *dev)
{
	/* disable RISC controller */
	cx_write(DEV_CNTRL2, 0);

	/* Disable all IR activity */
	cx_write(IR_CNTRL_REG, 0);

	/* Disable Video A/B activity */
	cx_write(VID_A_DMA_CTL, 0);
	cx_write(VID_B_DMA_CTL, 0);
	cx_write(VID_C_DMA_CTL, 0);

	/* Disable Audio activity */
	cx_write(AUD_INT_DMA_CTL, 0);
	cx_write(AUD_EXT_DMA_CTL, 0);

	/* Disable Serial port */
	cx_write(UART_CTL, 0);

	/* Disable Interrupts */
	cx23885_irq_disable_all(dev);
	cx_write(VID_A_INT_MSK, 0);
	cx_write(VID_B_INT_MSK, 0);
	cx_write(VID_C_INT_MSK, 0);
	cx_write(AUDIO_INT_INT_MSK, 0);
	cx_write(AUDIO_EXT_INT_MSK, 0);

}

static void cx23885_reset(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	cx23885_shutdown(dev);

	cx_write(PCI_INT_STAT, 0xffffffff);
	cx_write(VID_A_INT_STAT, 0xffffffff);
	cx_write(VID_B_INT_STAT, 0xffffffff);
	cx_write(VID_C_INT_STAT, 0xffffffff);
	cx_write(AUDIO_INT_INT_STAT, 0xffffffff);
	cx_write(AUDIO_EXT_INT_STAT, 0xffffffff);
	cx_write(CLK_DELAY, cx_read(CLK_DELAY) & 0x80000000);
	cx_write(PAD_CTRL, 0x00500300);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
	msleep(100);

	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH01],
		720*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH03],
		188*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH04], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH05], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH06],
		188*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH07], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH08], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH09], 128, 0);

	cx23885_gpio_setup(dev);

	cx23885_irq_get_mask(dev);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
}


static int cx23885_pci_quirks(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	/* The cx23885 bridge has a weird bug which causes NMI to be asserted
	 * when DMA begins if RDR_TLCTL0 bit4 is not cleared. It does not
	 * occur on the cx23887 bridge.
	 */
	if (dev->bridge == CX23885_BRIDGE_885)
		cx_clear(RDR_TLCTL0, 1 << 4);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
	return 0;
}

static int get_resources(struct cx23885_dev *dev)
{
	if (request_mem_region(pci_resource_start(dev->pci, 0),
			       pci_resource_len(dev->pci, 0),
			       dev->name))
		return 0;

	pr_err("%s: can't get MMIO memory @ 0x%llx\n",
	       dev->name, (unsigned long long)pci_resource_start(dev->pci, 0));

	return -EBUSY;
}

static int cx23885_init_tsport(struct cx23885_dev *dev,
	struct cx23885_tsport *port, int portno)
{
	dprintk(1, "%s(portno=%d)\n", __func__, portno);

	/* Transport bus init dma queue  - Common settings */
	port->dma_ctl_val        = 0x11; /* Enable RISC controller and Fifo */
	port->ts_int_msk_val     = 0x1111; /* TS port bits for RISC */
	port->vld_misc_val       = 0x0;
	port->hw_sop_ctrl_val    = (0x47 << 16 | 188 << 4);

	spin_lock_init(&port->slock);
	port->dev = dev;
	port->nr = portno;

	INIT_LIST_HEAD(&port->mpegq.active);
	mutex_init(&port->frontends.lock);
	INIT_LIST_HEAD(&port->frontends.felist);
	port->frontends.active_fe_id = 0;

	/* This should be hardcoded allow a single frontend
	 * attachment to this tsport, keeping the -dvb.c
	 * code clean and safe.
	 */
	if (!port->num_frontends)
		port->num_frontends = 1;

	switch (portno) {
	case 1:
		port->reg_gpcnt          = VID_B_GPCNT;
		port->reg_gpcnt_ctl      = VID_B_GPCNT_CTL;
		port->reg_dma_ctl        = VID_B_DMA_CTL;
		port->reg_lngth          = VID_B_LNGTH;
		port->reg_hw_sop_ctrl    = VID_B_HW_SOP_CTL;
		port->reg_gen_ctrl       = VID_B_GEN_CTL;
		port->reg_bd_pkt_status  = VID_B_BD_PKT_STATUS;
		port->reg_sop_status     = VID_B_SOP_STATUS;
		port->reg_fifo_ovfl_stat = VID_B_FIFO_OVFL_STAT;
		port->reg_vld_misc       = VID_B_VLD_MISC;
		port->reg_ts_clk_en      = VID_B_TS_CLK_EN;
		port->reg_src_sel        = VID_B_SRC_SEL;
		port->reg_ts_int_msk     = VID_B_INT_MSK;
		port->reg_ts_int_stat    = VID_B_INT_STAT;
		port->sram_chno          = SRAM_CH03; /* VID_B */
		port->pci_irqmask        = 0x02; /* VID_B bit1 */
		break;
	case 2:
		port->reg_gpcnt          = VID_C_GPCNT;
		port->reg_gpcnt_ctl      = VID_C_GPCNT_CTL;
		port->reg_dma_ctl        = VID_C_DMA_CTL;
		port->reg_lngth          = VID_C_LNGTH;
		port->reg_hw_sop_ctrl    = VID_C_HW_SOP_CTL;
		port->reg_gen_ctrl       = VID_C_GEN_CTL;
		port->reg_bd_pkt_status  = VID_C_BD_PKT_STATUS;
		port->reg_sop_status     = VID_C_SOP_STATUS;
		port->reg_fifo_ovfl_stat = VID_C_FIFO_OVFL_STAT;
		port->reg_vld_misc       = VID_C_VLD_MISC;
		port->reg_ts_clk_en      = VID_C_TS_CLK_EN;
		port->reg_src_sel        = 0;
		port->reg_ts_int_msk     = VID_C_INT_MSK;
		port->reg_ts_int_stat    = VID_C_INT_STAT;
		port->sram_chno          = SRAM_CH06; /* VID_C */
		port->pci_irqmask        = 0x04; /* VID_C bit2 */
		break;
	default:
		BUG();
	}

	return 0;
}

static void cx23885_dev_checkrevision(struct cx23885_dev *dev)
{
	switch (cx_read(RDR_CFG2) & 0xff) {
	case 0x00:
		/* cx23885 */
		dev->hwrevision = 0xa0;
		break;
	case 0x01:
		/* CX23885-12Z */
		dev->hwrevision = 0xa1;
		break;
	case 0x02:
		/* CX23885-13Z/14Z */
		dev->hwrevision = 0xb0;
		break;
	case 0x03:
		if (dev->pci->device == 0x8880) {
			/* CX23888-21Z/22Z */
			dev->hwrevision = 0xc0;
		} else {
			/* CX23885-14Z */
			dev->hwrevision = 0xa4;
		}
		break;
	case 0x04:
		if (dev->pci->device == 0x8880) {
			/* CX23888-31Z */
			dev->hwrevision = 0xd0;
		} else {
			/* CX23885-15Z, CX23888-31Z */
			dev->hwrevision = 0xa5;
		}
		break;
	case 0x0e:
		/* CX23887-15Z */
		dev->hwrevision = 0xc0;
		break;
	case 0x0f:
		/* CX23887-14Z */
		dev->hwrevision = 0xb1;
		break;
	default:
		pr_err("%s() New hardware revision found 0x%x\n",
		       __func__, dev->hwrevision);
	}
	if (dev->hwrevision)
		pr_info("%s() Hardware revision = 0x%02x\n",
			__func__, dev->hwrevision);
	else
		pr_err("%s() Hardware revision unknown 0x%x\n",
		       __func__, dev->hwrevision);
}

/* Find the first v4l2_subdev member of the group id in hw */
struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw)
{
	struct v4l2_subdev *result = NULL;
	struct v4l2_subdev *sd;

	spin_lock(&dev->v4l2_dev.lock);
	v4l2_device_for_each_subdev(sd, &dev->v4l2_dev) {
		if (sd->grp_id == hw) {
			result = sd;
			break;
		}
	}
	spin_unlock(&dev->v4l2_dev.lock);
	return result;
}

static int cx23885_dev_setup(struct cx23885_dev *dev)
{
	int i;

	spin_lock_init(&dev->pci_irqmask_lock);
	spin_lock_init(&dev->slock);

	mutex_init(&dev->lock);
	mutex_init(&dev->gpio_lock);

	atomic_inc(&dev->refcount);

	dev->nr = cx23885_devcount++;
	sprintf(dev->name, "cx23885[%d]", dev->nr);

	/* Configure the internal memory */
	if (dev->pci->device == 0x8880) {
		/* Could be 887 or 888, assume an 888 default */
		dev->bridge = CX23885_BRIDGE_888;
		/* Apply a sensible clock frequency for the PCIe bridge */
		dev->clk_freq = 50000000;
		dev->sram_channels = cx23887_sram_channels;
	} else
	if (dev->pci->device == 0x8852) {
		dev->bridge = CX23885_BRIDGE_885;
		/* Apply a sensible clock frequency for the PCIe bridge */
		dev->clk_freq = 28000000;
		dev->sram_channels = cx23885_sram_channels;
	} else
		BUG();

	dprintk(1, "%s() Memory configured for PCIe bridge type %d\n",
		__func__, dev->bridge);

	/* board config */
	dev->board = UNSET;
	if (card[dev->nr] < cx23885_bcount)
		dev->board = card[dev->nr];
	for (i = 0; UNSET == dev->board  &&  i < cx23885_idcount; i++)
		if (dev->pci->subsystem_vendor == cx23885_subids[i].subvendor &&
		    dev->pci->subsystem_device == cx23885_subids[i].subdevice)
			dev->board = cx23885_subids[i].card;
	if (UNSET == dev->board) {
		dev->board = CX23885_BOARD_UNKNOWN;
		cx23885_card_list(dev);
	}

	if (dev->pci->device == 0x8852) {
		/* no DIF on cx23885, so no analog tuner support possible */
		if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC)
			dev->board = CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885;
		else if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB)
			dev->board = CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885;
	}

	/* If the user specific a clk freq override, apply it */
	if (cx23885_boards[dev->board].clk_freq > 0)
		dev->clk_freq = cx23885_boards[dev->board].clk_freq;

	if (dev->board == CX23885_BOARD_HAUPPAUGE_IMPACTVCBE &&
		dev->pci->subsystem_device == 0x7137) {
		/* Hauppauge ImpactVCBe device ID 0x7137 is populated
		 * with an 888, and a 25Mhz crystal, instead of the
		 * usual third overtone 50Mhz. The default clock rate must
		 * be overridden so the cx25840 is properly configured
		 */
		dev->clk_freq = 25000000;
	}

	dev->pci_bus  = dev->pci->bus->number;
	dev->pci_slot = PCI_SLOT(dev->pci->devfn);
	cx23885_irq_add(dev, 0x001f00);

	/* External Master 1 Bus */
	dev->i2c_bus[0].nr = 0;
	dev->i2c_bus[0].dev = dev;
	dev->i2c_bus[0].reg_stat  = I2C1_STAT;
	dev->i2c_bus[0].reg_ctrl  = I2C1_CTRL;
	dev->i2c_bus[0].reg_addr  = I2C1_ADDR;
	dev->i2c_bus[0].reg_rdata = I2C1_RDATA;
	dev->i2c_bus[0].reg_wdata = I2C1_WDATA;
	dev->i2c_bus[0].i2c_period = (0x9d << 24); /* 100kHz */

	/* External Master 2 Bus */
	dev->i2c_bus[1].nr = 1;
	dev->i2c_bus[1].dev = dev;
	dev->i2c_bus[1].reg_stat  = I2C2_STAT;
	dev->i2c_bus[1].reg_ctrl  = I2C2_CTRL;
	dev->i2c_bus[1].reg_addr  = I2C2_ADDR;
	dev->i2c_bus[1].reg_rdata = I2C2_RDATA;
	dev->i2c_bus[1].reg_wdata = I2C2_WDATA;
	dev->i2c_bus[1].i2c_period = (0x9d << 24); /* 100kHz */

	/* Internal Master 3 Bus */
	dev->i2c_bus[2].nr = 2;
	dev->i2c_bus[2].dev = dev;
	dev->i2c_bus[2].reg_stat  = I2C3_STAT;
	dev->i2c_bus[2].reg_ctrl  = I2C3_CTRL;
	dev->i2c_bus[2].reg_addr  = I2C3_ADDR;
	dev->i2c_bus[2].reg_rdata = I2C3_RDATA;
	dev->i2c_bus[2].reg_wdata = I2C3_WDATA;
	dev->i2c_bus[2].i2c_period = (0x07 << 24); /* 1.95MHz */

	if ((cx23885_boards[dev->board].portb == CX23885_MPEG_DVB) ||
		(cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER))
		cx23885_init_tsport(dev, &dev->ts1, 1);

	if ((cx23885_boards[dev->board].portc == CX23885_MPEG_DVB) ||
		(cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER))
		cx23885_init_tsport(dev, &dev->ts2, 2);

	if (get_resources(dev) < 0) {
		pr_err("CORE %s No more PCIe resources for subsystem: %04x:%04x\n",
		       dev->name, dev->pci->subsystem_vendor,
		       dev->pci->subsystem_device);

		cx23885_devcount--;
		return -ENODEV;
	}

	/* PCIe stuff */
	dev->lmmio = ioremap(pci_resource_start(dev->pci, 0),
			     pci_resource_len(dev->pci, 0));

	dev->bmmio = (u8 __iomem *)dev->lmmio;

	pr_info("CORE %s: subsystem: %04x:%04x, board: %s [card=%d,%s]\n",
		dev->name, dev->pci->subsystem_vendor,
		dev->pci->subsystem_device, cx23885_boards[dev->board].name,
		dev->board, card[dev->nr] == dev->board ?
		"insmod option" : "autodetected");

	cx23885_pci_quirks(dev);

	/* Assume some sensible defaults */
	dev->tuner_type = cx23885_boards[dev->board].tuner_type;
	dev->tuner_addr = cx23885_boards[dev->board].tuner_addr;
	dev->tuner_bus = cx23885_boards[dev->board].tuner_bus;
	dev->radio_type = cx23885_boards[dev->board].radio_type;
	dev->radio_addr = cx23885_boards[dev->board].radio_addr;

	dprintk(1, "%s() tuner_type = 0x%x tuner_addr = 0x%x tuner_bus = %d\n",
		__func__, dev->tuner_type, dev->tuner_addr, dev->tuner_bus);
	dprintk(1, "%s() radio_type = 0x%x radio_addr = 0x%x\n",
		__func__, dev->radio_type, dev->radio_addr);

	/* The cx23417 encoder has GPIO's that need to be initialised
	 * before DVB, so that demodulators and tuners are out of
	 * reset before DVB uses them.
	 */
	if ((cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) ||
		(cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER))
			cx23885_mc417_init(dev);

	/* init hardware */
	cx23885_reset(dev);

	cx23885_i2c_register(&dev->i2c_bus[0]);
	cx23885_i2c_register(&dev->i2c_bus[1]);
	cx23885_i2c_register(&dev->i2c_bus[2]);
	cx23885_card_setup(dev);
	call_all(dev, tuner, standby);
	cx23885_ir_init(dev);

	if (dev->board == CX23885_BOARD_VIEWCAST_460E) {
		/*
		 * GPIOs 9/8 are input detection bits for the breakout video
		 * (gpio 8) and audio (gpio 9) cables. When they're attached,
		 * this gpios are pulled high. Make sure these GPIOs are marked
		 * as inputs.
		 */
		cx23885_gpio_enable(dev, 0x300, 0);
	}

	if (cx23885_boards[dev->board].porta == CX23885_ANALOG_VIDEO) {
		if (cx23885_video_register(dev) < 0) {
			pr_err("%s() Failed to register analog video adapters on VID_A\n",
			       __func__);
		}
	}

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_DVB) {
		if (cx23885_boards[dev->board].num_fds_portb)
			dev->ts1.num_frontends =
				cx23885_boards[dev->board].num_fds_portb;
		if (cx23885_dvb_register(&dev->ts1) < 0) {
			pr_err("%s() Failed to register dvb adapters on VID_B\n",
			       __func__);
		}
	} else
	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) {
		if (cx23885_417_register(dev) < 0) {
			pr_err("%s() Failed to register 417 on VID_B\n",
			       __func__);
		}
	}

	if (cx23885_boards[dev->board].portc == CX23885_MPEG_DVB) {
		if (cx23885_boards[dev->board].num_fds_portc)
			dev->ts2.num_frontends =
				cx23885_boards[dev->board].num_fds_portc;
		if (cx23885_dvb_register(&dev->ts2) < 0) {
			pr_err("%s() Failed to register dvb on VID_C\n",
			       __func__);
		}
	} else
	if (cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER) {
		if (cx23885_417_register(dev) < 0) {
			pr_err("%s() Failed to register 417 on VID_C\n",
			       __func__);
		}
	}

	cx23885_dev_checkrevision(dev);

	/* disable MSI for NetUP cards, otherwise CI is not working */
	if (cx23885_boards[dev->board].ci_type > 0)
		cx_clear(RDR_RDRCTL1, 1 << 8);

	switch (dev->board) {
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TEVII_S471:
		cx_clear(RDR_RDRCTL1, 1 << 8);
		break;
	}

	return 0;
}

static void cx23885_dev_unregister(struct cx23885_dev *dev)
{
	release_mem_region(pci_resource_start(dev->pci, 0),
			   pci_resource_len(dev->pci, 0));

	if (!atomic_dec_and_test(&dev->refcount))
		return;

	if (cx23885_boards[dev->board].porta == CX23885_ANALOG_VIDEO)
		cx23885_video_unregister(dev);

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_DVB)
		cx23885_dvb_unregister(&dev->ts1);

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
		cx23885_417_unregister(dev);

	if (cx23885_boards[dev->board].portc == CX23885_MPEG_DVB)
		cx23885_dvb_unregister(&dev->ts2);

	if (cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER)
		cx23885_417_unregister(dev);

	cx23885_i2c_unregister(&dev->i2c_bus[2]);
	cx23885_i2c_unregister(&dev->i2c_bus[1]);
	cx23885_i2c_unregister(&dev->i2c_bus[0]);

	iounmap(dev->lmmio);
}

static __le32 *cx23885_risc_field(__le32 *rp, struct scatterlist *sglist,
			       unsigned int offset, u32 sync_line,
			       unsigned int bpl, unsigned int padding,
			       unsigned int lines,  unsigned int lpi, bool jump)
{
	struct scatterlist *sg;
	unsigned int line, todo, sol;


	if (jump) {
		*(rp++) = cpu_to_le32(RISC_JUMP);
		*(rp++) = cpu_to_le32(0);
		*(rp++) = cpu_to_le32(0); /* bits 63-32 */
	}

	/* sync instruction */
	if (sync_line != NO_SYNC_LINE)
		*(rp++) = cpu_to_le32(RISC_RESYNC | sync_line);

	/* scan lines */
	sg = sglist;
	for (line = 0; line < lines; line++) {
		while (offset && offset >= sg_dma_len(sg)) {
			offset -= sg_dma_len(sg);
			sg = sg_next(sg);
		}

		if (lpi && line > 0 && !(line % lpi))
			sol = RISC_SOL | RISC_IRQ1 | RISC_CNT_INC;
		else
			sol = RISC_SOL;

		if (bpl <= sg_dma_len(sg)-offset) {
			/* fits into current chunk */
			*(rp++) = cpu_to_le32(RISC_WRITE|sol|RISC_EOL|bpl);
			*(rp++) = cpu_to_le32(sg_dma_address(sg)+offset);
			*(rp++) = cpu_to_le32(0); /* bits 63-32 */
			offset += bpl;
		} else {
			/* scanline needs to be split */
			todo = bpl;
			*(rp++) = cpu_to_le32(RISC_WRITE|sol|
					    (sg_dma_len(sg)-offset));
			*(rp++) = cpu_to_le32(sg_dma_address(sg)+offset);
			*(rp++) = cpu_to_le32(0); /* bits 63-32 */
			todo -= (sg_dma_len(sg)-offset);
			offset = 0;
			sg = sg_next(sg);
			while (todo > sg_dma_len(sg)) {
				*(rp++) = cpu_to_le32(RISC_WRITE|
						    sg_dma_len(sg));
				*(rp++) = cpu_to_le32(sg_dma_address(sg));
				*(rp++) = cpu_to_le32(0); /* bits 63-32 */
				todo -= sg_dma_len(sg);
				sg = sg_next(sg);
			}
			*(rp++) = cpu_to_le32(RISC_WRITE|RISC_EOL|todo);
			*(rp++) = cpu_to_le32(sg_dma_address(sg));
			*(rp++) = cpu_to_le32(0); /* bits 63-32 */
			offset += todo;
		}
		offset += padding;
	}

	return rp;
}

int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
			struct scatterlist *sglist, unsigned int top_offset,
			unsigned int bottom_offset, unsigned int bpl,
			unsigned int padding, unsigned int lines)
{
	u32 instructions, fields;
	__le32 *rp;

	fields = 0;
	if (UNSET != top_offset)
		fields++;
	if (UNSET != bottom_offset)
		fields++;

	/* estimate risc mem: worst case is one write per page border +
	   one write per scan line + syncs + jump (all 2 dwords).  Padding
	   can cause next bpl to start close to a page border.  First DMA
	   region may be smaller than PAGE_SIZE */
	/* write and jump need and extra dword */
	instructions  = fields * (1 + ((bpl + padding) * lines)
		/ PAGE_SIZE + lines);
	instructions += 5;
	risc->size = instructions * 12;
	risc->cpu = dma_alloc_coherent(&pci->dev, risc->size, &risc->dma,
				       GFP_KERNEL);
	if (risc->cpu == NULL)
		return -ENOMEM;

	/* write risc instructions */
	rp = risc->cpu;
	if (UNSET != top_offset)
		rp = cx23885_risc_field(rp, sglist, top_offset, 0,
					bpl, padding, lines, 0, true);
	if (UNSET != bottom_offset)
		rp = cx23885_risc_field(rp, sglist, bottom_offset, 0x200,
					bpl, padding, lines, 0, UNSET == top_offset);

	/* save pointer to jmp instruction address */
	risc->jmp = rp;
	BUG_ON((risc->jmp - risc->cpu + 2) * sizeof(*risc->cpu) > risc->size);
	return 0;
}

int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines, unsigned int lpi)
{
	u32 instructions;
	__le32 *rp;

	/* estimate risc mem: worst case is one write per page border +
	   one write per scan line + syncs + jump (all 2 dwords).  Here
	   there is no padding and no sync.  First DMA region may be smaller
	   than PAGE_SIZE */
	/* Jump and write need an extra dword */
	instructions  = 1 + (bpl * lines) / PAGE_SIZE + lines;
	instructions += 4;

	risc->size = instructions * 12;
	risc->cpu = dma_alloc_coherent(&pci->dev, risc->size, &risc->dma,
				       GFP_KERNEL);
	if (risc->cpu == NULL)
		return -ENOMEM;

	/* write risc instructions */
	rp = risc->cpu;
	rp = cx23885_risc_field(rp, sglist, 0, NO_SYNC_LINE,
				bpl, 0, lines, lpi, lpi == 0);

	/* save pointer to jmp instruction address */
	risc->jmp = rp;
	BUG_ON((risc->jmp - risc->cpu + 2) * sizeof(*risc->cpu) > risc->size);
	return 0;
}

int cx23885_risc_vbibuffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
			struct scatterlist *sglist, unsigned int top_offset,
			unsigned int bottom_offset, unsigned int bpl,
			unsigned int padding, unsigned int lines)
{
	u32 instructions, fields;
	__le32 *rp;

	fields = 0;
	if (UNSET != top_offset)
		fields++;
	if (UNSET != bottom_offset)
		fields++;

	/* estimate risc mem: worst case is one write per page border +
	   one write per scan line + syncs + jump (all 2 dwords).  Padding
	   can cause next bpl to start close to a page border.  First DMA
	   region may be smaller than PAGE_SIZE */
	/* write and jump need and extra dword */
	instructions  = fields * (1 + ((bpl + padding) * lines)
		/ PAGE_SIZE + lines);
	instructions += 5;
	risc->size = instructions * 12;
	risc->cpu = dma_alloc_coherent(&pci->dev, risc->size, &risc->dma,
				       GFP_KERNEL);
	if (risc->cpu == NULL)
		return -ENOMEM;
	/* write risc instructions */
	rp = risc->cpu;

	/* Sync to line 6, so US CC line 21 will appear in line '12'
	 * in the userland vbi payload */
	if (UNSET != top_offset)
		rp = cx23885_risc_field(rp, sglist, top_offset, 0,
					bpl, padding, lines, 0, true);

	if (UNSET != bottom_offset)
		rp = cx23885_risc_field(rp, sglist, bottom_offset, 0x200,
					bpl, padding, lines, 0, UNSET == top_offset);



	/* save pointer to jmp instruction address */
	risc->jmp = rp;
	BUG_ON((risc->jmp - risc->cpu + 2) * sizeof(*risc->cpu) > risc->size);
	return 0;
}


void cx23885_free_buffer(struct cx23885_dev *dev, struct cx23885_buffer *buf)
{
	struct cx23885_riscmem *risc = &buf->risc;

	dma_free_coherent(&dev->pci->dev, risc->size, risc->cpu, risc->dma);
}

static void cx23885_tsport_reg_dump(struct cx23885_tsport *port)
{
	struct cx23885_dev *dev = port->dev;

	dprintk(1, "%s() Register Dump\n", __func__);
	dprintk(1, "%s() DEV_CNTRL2               0x%08X\n", __func__,
		cx_read(DEV_CNTRL2));
	dprintk(1, "%s() PCI_INT_MSK              0x%08X\n", __func__,
		cx23885_irq_get_mask(dev));
	dprintk(1, "%s() AUD_INT_INT_MSK          0x%08X\n", __func__,
		cx_read(AUDIO_INT_INT_MSK));
	dprintk(1, "%s() AUD_INT_DMA_CTL          0x%08X\n", __func__,
		cx_read(AUD_INT_DMA_CTL));
	dprintk(1, "%s() AUD_EXT_INT_MSK          0x%08X\n", __func__,
		cx_read(AUDIO_EXT_INT_MSK));
	dprintk(1, "%s() AUD_EXT_DMA_CTL          0x%08X\n", __func__,
		cx_read(AUD_EXT_DMA_CTL));
	dprintk(1, "%s() PAD_CTRL                 0x%08X\n", __func__,
		cx_read(PAD_CTRL));
	dprintk(1, "%s() ALT_PIN_OUT_SEL          0x%08X\n", __func__,
		cx_read(ALT_PIN_OUT_SEL));
	dprintk(1, "%s() GPIO2                    0x%08X\n", __func__,
		cx_read(GPIO2));
	dprintk(1, "%s() gpcnt(0x%08X)          0x%08X\n", __func__,
		port->reg_gpcnt, cx_read(port->reg_gpcnt));
	dprintk(1, "%s() gpcnt_ctl(0x%08X)      0x%08x\n", __func__,
		port->reg_gpcnt_ctl, cx_read(port->reg_gpcnt_ctl));
	dprintk(1, "%s() dma_ctl(0x%08X)        0x%08x\n", __func__,
		port->reg_dma_ctl, cx_read(port->reg_dma_ctl));
	if (port->reg_src_sel)
		dprintk(1, "%s() src_sel(0x%08X)        0x%08x\n", __func__,
			port->reg_src_sel, cx_read(port->reg_src_sel));
	dprintk(1, "%s() lngth(0x%08X)          0x%08x\n", __func__,
		port->reg_lngth, cx_read(port->reg_lngth));
	dprintk(1, "%s() hw_sop_ctrl(0x%08X)    0x%08x\n", __func__,
		port->reg_hw_sop_ctrl, cx_read(port->reg_hw_sop_ctrl));
	dprintk(1, "%s() gen_ctrl(0x%08X)       0x%08x\n", __func__,
		port->reg_gen_ctrl, cx_read(port->reg_gen_ctrl));
	dprintk(1, "%s() bd_pkt_status(0x%08X)  0x%08x\n", __func__,
		port->reg_bd_pkt_status, cx_read(port->reg_bd_pkt_status));
	dprintk(1, "%s() sop_status(0x%08X)     0x%08x\n", __func__,
		port->reg_sop_status, cx_read(port->reg_sop_status));
	dprintk(1, "%s() fifo_ovfl_stat(0x%08X) 0x%08x\n", __func__,
		port->reg_fifo_ovfl_stat, cx_read(port->reg_fifo_ovfl_stat));
	dprintk(1, "%s() vld_misc(0x%08X)       0x%08x\n", __func__,
		port->reg_vld_misc, cx_read(port->reg_vld_misc));
	dprintk(1, "%s() ts_clk_en(0x%08X)      0x%08x\n", __func__,
		port->reg_ts_clk_en, cx_read(port->reg_ts_clk_en));
	dprintk(1, "%s() ts_int_msk(0x%08X)     0x%08x\n", __func__,
		port->reg_ts_int_msk, cx_read(port->reg_ts_int_msk));
	dprintk(1, "%s() ts_int_status(0x%08X)  0x%08x\n", __func__,
		port->reg_ts_int_stat, cx_read(port->reg_ts_int_stat));
	dprintk(1, "%s() PCI_INT_STAT           0x%08X\n", __func__,
		cx_read(PCI_INT_STAT));
	dprintk(1, "%s() VID_B_INT_MSTAT        0x%08X\n", __func__,
		cx_read(VID_B_INT_MSTAT));
	dprintk(1, "%s() VID_B_INT_SSTAT        0x%08X\n", __func__,
		cx_read(VID_B_INT_SSTAT));
	dprintk(1, "%s() VID_C_INT_MSTAT        0x%08X\n", __func__,
		cx_read(VID_C_INT_MSTAT));
	dprintk(1, "%s() VID_C_INT_SSTAT        0x%08X\n", __func__,
		cx_read(VID_C_INT_SSTAT));
}

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf)
{
	struct cx23885_dev *dev = port->dev;
	u32 reg;

	dprintk(1, "%s() w: %d, h: %d, f: %d\n", __func__,
		dev->width, dev->height, dev->field);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);

	/* Stop the fifo and risc engine for this port */
	cx_clear(port->reg_dma_ctl, port->dma_ctl_val);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev,
				   &dev->sram_channels[port->sram_chno],
				   port->ts_packet_size, buf->risc.dma);
	if (debug > 5) {
		cx23885_sram_channel_dump(dev,
			&dev->sram_channels[port->sram_chno]);
		cx23885_risc_disasm(port, &buf->risc);
	}

	/* write TS length to chip */
	cx_write(port->reg_lngth, port->ts_packet_size);

	if ((!(cx23885_boards[dev->board].portb & CX23885_MPEG_DVB)) &&
		(!(cx23885_boards[dev->board].portc & CX23885_MPEG_DVB))) {
		pr_err("%s() Unsupported .portb/c (0x%08x)/(0x%08x)\n",
			__func__,
			cx23885_boards[dev->board].portb,
			cx23885_boards[dev->board].portc);
		return -EINVAL;
	}

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
		cx23885_av_clk(dev, 0);

	udelay(100);

	/* If the port supports SRC SELECT, configure it */
	if (port->reg_src_sel)
		cx_write(port->reg_src_sel, port->src_sel_val);

	cx_write(port->reg_hw_sop_ctrl, port->hw_sop_ctrl_val);
	cx_write(port->reg_ts_clk_en, port->ts_clk_en_val);
	cx_write(port->reg_vld_misc, port->vld_misc_val);
	cx_write(port->reg_gen_ctrl, port->gen_ctrl_val);
	udelay(100);

	/* NOTE: this is 2 (reserved) for portb, does it matter? */
	/* reset counter to zero */
	cx_write(port->reg_gpcnt_ctl, 3);
	q->count = 0;

	/* Set VIDB pins to input */
	if (cx23885_boards[dev->board].portb == CX23885_MPEG_DVB) {
		reg = cx_read(PAD_CTRL);
		reg &= ~0x3; /* Clear TS1_OE & TS1_SOP_OE */
		cx_write(PAD_CTRL, reg);
	}

	/* Set VIDC pins to input */
	if (cx23885_boards[dev->board].portc == CX23885_MPEG_DVB) {
		reg = cx_read(PAD_CTRL);
		reg &= ~0x4; /* Clear TS2_SOP_OE */
		cx_write(PAD_CTRL, reg);
	}

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) {

		reg = cx_read(PAD_CTRL);
		reg = reg & ~0x1;    /* Clear TS1_OE */

		/* FIXME, bit 2 writing here is questionable */
		/* set TS1_SOP_OE and TS1_OE_HI */
		reg = reg | 0xa;
		cx_write(PAD_CTRL, reg);

		/* Sets MOE_CLK_DIS to disable MoE clock */
		/* sets MCLK_DLY_SEL/BCLK_DLY_SEL to 1 buffer delay each */
		cx_write(CLK_DELAY, cx_read(CLK_DELAY) | 0x80000011);

		/* ALT_GPIO_ALT_SET: GPIO[0]
		 * IR_ALT_TX_SEL: GPIO[1]
		 * GPIO1_ALT_SEL: VIP_656_DATA[0]
		 * GPIO0_ALT_SEL: VIP_656_CLK
		 */
		cx_write(ALT_PIN_OUT_SEL, 0x10100045);
	}

	switch (dev->bridge) {
	case CX23885_BRIDGE_885:
	case CX23885_BRIDGE_887:
	case CX23885_BRIDGE_888:
		/* enable irqs */
		dprintk(1, "%s() enabling TS int's and DMA\n", __func__);
		/* clear dma in progress */
		cx23885_clear_bridge_error(dev);
		cx_set(port->reg_ts_int_msk,  port->ts_int_msk_val);
		cx_set(port->reg_dma_ctl, port->dma_ctl_val);

		/* clear dma in progress */
		cx23885_clear_bridge_error(dev);
		cx23885_irq_add(dev, port->pci_irqmask);
		cx23885_irq_enable_all(dev);

		/* clear dma in progress */
		cx23885_clear_bridge_error(dev);
		break;
	default:
		BUG();
	}

	cx_set(DEV_CNTRL2, (1<<5)); /* Enable RISC controller */
	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
		cx23885_av_clk(dev, 1);

	if (debug > 4)
		cx23885_tsport_reg_dump(port);

	cx23885_irq_get_mask(dev);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);

	return 0;
}

static int cx23885_stop_dma(struct cx23885_tsport *port)
{
	struct cx23885_dev *dev = port->dev;
	u32 reg;
	int delay = 0;
	uint32_t reg1_val;
	uint32_t reg2_val;

	dprintk(1, "%s()\n", __func__);

	/* Stop interrupts and DMA */
	cx_clear(port->reg_ts_int_msk, port->ts_int_msk_val);
	cx_clear(port->reg_dma_ctl, port->dma_ctl_val);
	/* just in case wait for any dma to complete before allowing dealloc */
	mdelay(20);
	for (delay = 0; delay < 100; delay++) {
		reg1_val = cx_read(TC_REQ);
		reg2_val = cx_read(TC_REQ_SET);
		if (reg1_val == 0 || reg2_val == 0)
			break;
		mdelay(1);
	}
	dev_dbg(&dev->pci->dev, "delay=%d reg1=0x%08x reg2=0x%08x\n",
		delay, reg1_val, reg2_val);

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) {
		reg = cx_read(PAD_CTRL);

		/* Set TS1_OE */
		reg = reg | 0x1;

		/* clear TS1_SOP_OE and TS1_OE_HI */
		reg = reg & ~0xa;
		cx_write(PAD_CTRL, reg);
		cx_write(port->reg_src_sel, 0);
		cx_write(port->reg_gen_ctrl, 8);
	}

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER)
		cx23885_av_clk(dev, 0);

	return 0;
}

/* ------------------------------------------------------------------ */

int cx23885_buf_prepare(struct cx23885_buffer *buf, struct cx23885_tsport *port)
{
	struct cx23885_dev *dev = port->dev;
	int size = port->ts_packet_size * port->ts_packet_count;
	struct sg_table *sgt = vb2_dma_sg_plane_desc(&buf->vb.vb2_buf, 0);

	dprintk(1, "%s: %p\n", __func__, buf);
	if (vb2_plane_size(&buf->vb.vb2_buf, 0) < size)
		return -EINVAL;
	vb2_set_plane_payload(&buf->vb.vb2_buf, 0, size);

	cx23885_risc_databuffer(dev->pci, &buf->risc,
				sgt->sgl,
				port->ts_packet_size, port->ts_packet_count, 0);
	return 0;
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back