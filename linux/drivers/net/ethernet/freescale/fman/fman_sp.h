TEK WinFast PxTV1200",
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
	[CX23885_BO