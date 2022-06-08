pauge WinTV-HVR1275",
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
		.card      = CX23885_BOARD_HAUPPAUGE_