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
			.type   = CX23885_