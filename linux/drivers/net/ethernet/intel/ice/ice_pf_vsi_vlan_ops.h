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
				  CX25840_N