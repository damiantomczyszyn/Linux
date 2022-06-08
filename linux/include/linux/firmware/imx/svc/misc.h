] = {
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
			.gpio0	= 