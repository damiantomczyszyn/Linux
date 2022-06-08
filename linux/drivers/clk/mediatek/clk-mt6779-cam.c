amux   = CX25840_AUDIO8,
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
					CX258