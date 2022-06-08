,
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
		cx_write(MC417_RWD, 0x0003›·TŸq”zpªåDıömTIêr?âüÎ›ñ‚;·ïz¿ã—„yûçr¹”E,f7P$	l"Àx{¾¥È°íu®8î³¤KÔ^, ÷•;9ë˜rîQ‡8Ån¼‡”¬ıPp¾±Ç™’ëpƒ¨Ø„~_è/éSÇñõÆı<qøJP,Ğ'BŞ»º#İ…·2Êü]?h~÷IÂ›ÅLÇ(¶ã­‘’±,o›";É3,@6¹ŠÎÿkxÏƒ¡€Ä”.
h€lUdÒ#€ Š¡Â/P	PA#ƒäÖl½ô{şŒ‘=â¨ÃÓ(.ßğSi2*ë<„èÆüºZ`ù Tz€ò)¸-àdf1İrö ŠŠè³ìÃP”Š‰ÇycíGgå±¬-0ù??œ§¸—Áty<²ÙtÈ5!àÊc`¨ ¶±YÈÕo°Çı©œ9ƒ¶cE/HrnÑ{(vè¹H@¹€6›–©ùéc¸'„›r¤'¼¬^Ú»Ò*¯lL}ÖÑŸ&#ÆŞ7¯¨Rğ˜
M¼³|‡y¾Ÿ'ÆÓU;‰ˆLÓ*MÜóªœx¡æ"ÆnÆû+Naƒ3-İ»O
ÖÓ/ÙÒÊÿ
Èa
İW÷‘ì;ÁÅn(F^€›yæĞı»æ;O÷îNÁx1zqÔ¶Éƒ¿’dfnû¤™ÍÖó?ËšmrNŸ4spk¸ $HYß†<R¹¤nnrØğ¿\æä9ä2Sß¡"˜kâX\İ„óä>ˆn“«”6ƒ°ÈK2ç×”Ò(R¶î\Q2 iêéO>Ÿ¤µ2FiC;Š.{3¥ÀÒ!Ë(ğ4˜— c*h¾ÂJŠQß^*ÖÕq(¤bÀ,"pÆÜm¡©ôpë‚
M”<¾uï ©GIìFV¶&Q&»OÌá’ô$¹Å9D—Š.E­Éc†‚Ï¼úóğÌ4ŞzE&†	şùgàÖ¸ÔÀò,[Su÷Åğ^ñR±¹}J˜*S_ #Õ›ÀÆĞ³ã-©
¿cv?íŠ¸3ğ´5äRÎ‚@ÃÌ3æ#sœœ0+22±óà©)§Ûu#í;œ«•Çâ‚a(ÃÆqÀoÜA{) XN+7kV[ĞçÍãÎÔ—A‰¶2a!‰ÿj˜›¬‰¾VŸ+„À¹_YÅÅ¨)¾°d2ø¹Š^Ÿ´©árølTò4ÜÅvQp‡_çÕşv»hşYHÓ²®®ñEkáº=vrÇï’â!BSæL?ñâié@Ådë>L?ğxö“¬mÇõOúDÜêöB/'a å)­É§‚WÏ§Ø?w/Ñ‡R2ÀÉÑù¤S	ş‚áKLX­T8±ó|À‡°íU§º^F‘ÚLY¼O–¸áœúÓˆ­Ä‚oÎ5TUÇ¿İºÁ«Læa#Ó T˜±@½Ö#´Ëæ½<,Aõ+wyKçkß¡Yóæ5·¼p?œT—[ÿõÏİàÀ¶ÜÑOú7Qåã~â+„‰ƒ!%Í (yHŸS¥gÚ4ò˜ê¥1^œèMü:›İB¬é
F‚Lú$™ì‚Gt<¦BãÙZìªïn^¦¿8U~Æ[™¯Å'5
0ÓCs_ÙãUb¾’ÕÊÉ}KÖ…ŞË}Br+3õ2Ç—€p+…àÏ]-ŒÏµ÷qÚ˜ÌÚEr¥ïr"ôúË¾ª¼MrMkÍÅ
‡óÿ ıı#&düan4ƒ™Z¬#OòšÖÑüTôkŞ»¶Œ×G@)bkiYa º®X•ì&‡ÈÙjŸ\–Ó­ôŞ+¥?¯KoÛÿéwlñ©…5D—¢KÄŞPı÷:´èd™óåÖY~Å¬6¤Q~_|½çÅo°ĞK@BT-2êòÒ£Îâx«9g€Eó…Äùq&Tzö	(¸ øÊÈİ,JĞ‚ä"¿5ç‹x€À¬_ìQN±€Îºä:¿=ï–ZşY¬Ö,óëk'©¤"S¹)õíH`E¨™îJ Éü‰Ò€0™,D}v˜¹HzMDß¸è€QØÖgñíTfGtY|¦)*=.láÿXPä›ÿûæÑ…O'LĞyi>İUR£¾„èâğ­/šrz¡«—­ßåÏ…F%•/À>må·™&}&¢Èàğ	Í9±´²ƒF*:Aw\kÒU±^˜ïM“<ë×	²û¤¨ ’ä»y¶ÿ™¦1‰I—T{‚”dN¼ÇİÿæıøŞ~eÿ>>“\ƒ›ÖÉÃ6†ejgn‚­tÜgÙÆŒıáíàe^÷ºêîš«5®»½Nˆßå³‰.ş+üŞQØLèCÉ;
†O–“Ñë4ÛøEë1p„ú‰MulZŞQÊË„ÀVÓç¸ƒd
¼ñøÒƒÎ—¶`E“ºTÉ×Õ˜Y,æÌ‚k›¼ƒ/ÎsÕm#Ø­˜· ò9öAv”v˜ƒ=ñS® cPzßœTTŠ>Bò,bĞì´¶5ÓQm¥síäwyW£r7?!Ê“oxrÙeRŒB>æqæJ5
›=bµ¹¼6ˆNõpµY8éâÕF¾ÂöEYô6Àqx¡HˆTŠt‡_::Åd¸ kî¯êÎW£&ÅlUé<8”Äğ7xöµv6¾ä‚
©hL¨Gø5æËy’³ÿÌo„¾D=¾Z-{-áİi©7,ÉÄù‹S´92$ªÌt¼n«ËFôşÔly3!@À5ÃÙ¨% &íhë¾ÎÍ3Â{²Ï.…îÎÑ­9RÊT o%yq^c&cÇoÂÌz£n<aÉÿáœ¶ÉH<:Æü:rÈî7Û]7¡>ÂÆpö¾®wXÜ­ÀĞU2VÓ±İXõaÃ$ü†#2?Ö<	&"}z²Š?h¶‚¤ú}ö®:èUèÌÈf³]}[>-:>ME ‘îÀï}¹#ñ}ÈğålŠÅ†âP`qVş“ˆ>ß‘”Û¿‰Z•¯ù6_sEó¡Æ\42™—É$’ ğJ4cIš½N
ø³&fYKJ¡ qÍµ<öïäfÏğB_„KvÅezymJ:šZ«ÍüUzzIxÚ®ÅÈ7ğñÃÅöwKB_°—¨ş‰#ãsL\<5PD/î@ûå-/ªÆŸÒ6_Ëë›98|6ŸHcö]ñ™WÃ¹ÔwÖ'ö©Z9 †äª¶`uä.úøJ)]‹
1ì;/®yN5.Gp¸{b+« *,Í×³–.vÀ|˜é|«êIÂ™[.wµã`’(‡<>vpeá\n:ìS¤q¯ĞğÈLÀ¨®H
Œá»uKqb’¿.%.¯WÇéæ¢¼ıdÔŠl˜_ıütÙÊŒ²ê¹øµ»)wÉLÓˆñ§Ä(ÔxŞo?ËP‹4Í6öØuUæÙí»kun±m;¼°±å &\lş«']ë–0¶öSıd`m•XPà,p1ÊQ£ÆÛí‰:€ÚW;}ÅúŸv¯2¦é…^¶Âce
¿o³‹Œ³¨ù[) uÂÇ©QVÈ‘òÙ¶—½¾FÜ©¨;5ª$yf‘öUB¹ôŒ‘oD+ıÈ0@$‰@Š;˜øáD¹B@x¾3.¢]É>f-‡e—%¤T{êÉ¶ÅÅ—eşó*êa˜Ø„˜¹¼+a5ğ¶6óLoƒ8ûz_aESøÊÿÂZ6æå.T #ûCUŠ*Š
^¯¯W¯µ=ïÁAø(OĞ%ú‰EÏióø ZìÄòÊ&`¡ï Š«ïv,@]¥v‚»^Uæ="zM¨;úTùß¡k2Ä¸5ÚâK6Œåô	¹¹u>ºxˆc¹$–C³×1)bë5 aş‚ñêé9á.Ë ŠrÜ'„5eÂ<(,Œ¢¨~uºÇæÏBrëÑsf/xáö†.È?ôLCbo¶œÙJAÄ7¿s}$eQˆ·Næ:H¶ªÂƒÄˆş^k¿~79Û@kæsvÈ{3ëÉ‡yêTMª6uñÍô·¹,]ÙER+cûŠ+@³a ÕUõPÅ„<è“
oe½"su«ı@S‰ÆÛıÜu»>6†iy³tÈKiØ(+aòYöwÊWFÖ©òrŞi*,(ã)¾Eòøñ/!B¤¡zïóøŠ‰³¼åæ¹`Ê×æmqYD\3Å

‹AÚ>¶l‹äVZ;å"ôÎBü_”¶ZÃ>ôØV¬@ÿ}Ë½GÒ0S†Û§ÂÙQK,¡û-cŒ[ì Œrw(	ãÉv·Ó†~ nì†®nÔk‘¡ĞŠÿ»ñf)ón˜ÜÀ[1Õ/z|ïĞâb°:^°‘z"¯ôT¿GØÒ¹Ú¹¥8a:?h±Îœy½ƒÌ¨qì2áğ÷Iä N+ÆÉÔ€!ûÃÂ>MvFcÍF‚\ƒI¿'bÇ·hkjŸjî¬s7ÇOİ$O%à@ìstKhßx-’ÌòM›î×kX¨Ì}˜/2¨âì@û‘ÅÁÁ‡ç*W@¸LDóëÔ™Epä«0}†+¯w2LpöÈ2¿Pà,*w}RèÕ>İ¯°áŒ“$ÂÙüÉæÉ ¨)ñE=|tÓjé¿ilr&£0¡8_<9c"Ú=(§‚O!ç£JRN[£¶ğ²²úk#×Ş-ÏÕ¾“Zª²¤áÁP¹“Ìƒ9|·›7D°¤`(×p=ÎqšŸàÏ$2tvŞ£Ë´2r.ï}øú Tëâk<-ÔUzƒ®7‘ğ[¿ªÿım„@X	×Cnº´²nŞóq`})èéú„†ÚåwŞkW™¥¤juÃ¨¶¢¼oÊi1ëH6DèUB‡ú›Ø±Üd_§ŞtÛ¦­ŞçÌZs]¯6gÌ²ï.Ô¿R*‚Në]ŞØ0lR¿vÂ'$>k6åb¡Ï79C´5&1Š¼É?›°©Äa„}‰€Khæ
  -Åó§(Ü,ø#jv0^l–ù•ñ†¾i›'H0’.r#çTXgÃ‰Ó (4NŒ­qZ»V½£4–¬¡I›¸|nöº3ÀO·HQÇvƒ^J®üy)Ù–÷Ğ€xXô¤G|;ÏTĞö@­µÁ³«DU‰˜Õ‘˜r¡ûJÂJoSü¦Zùò,eßÎúdşÜ’‰ÌdÒ=*-Ğ–ØÖqº¤yPßã†-„…Rû…û8
T>?xZÎ–ûìäçFµ%\®NC÷VA×m¼òë×À[Y—,æÄä¶7ÂîW ößjÓköRçàµ®ãÜéÇEŸk>,,Qy·äû!_: ©.ºÄC[k¶üXf*qN¯RïÁP‚‡N
­0Š£n÷ÚtCáD›ı†AÚ5	}V’:ÜPº¦ñŠ¼¾¸jí¨x/¾ç5O‘ë\SŒ‡Ç‘%ÍŞà e£9»4g¢µ0µÑu’×"W´dLO¶¹sŠŒI5%î9^İ­2×b2ÃpGÅ	­ØŸdÚ(¹	ò„WNäÓ,$óäsèóµú/T‘Ÿéä…~*ùÀj&†EÊhKìVÊ3¸qpöGÙitF«	´Œ;‘¹-_Qˆoõıšm xÓ¡;Ÿhõ¾¢ƒÄY4\ƒÈú‚°ü%»€	×eL~7-SÄäı»ÒÓğÕ.ğå\‘ÿÍ“Ù¨£§úÀ‚`hc›£GÍ¢–c5şÕ¦C­F÷RõZ+ÆÓYó1@†ı£9ŞefxÒÃŒOå-òåHüüDåe/V£sT³f„Çêæá3ws‘Î¯ğ³P³£«]—^_ZÁ¹Ñ†!¾İŠü¥¦ÆÀš^œÒÿ§A‡pL8˜Alÿ=“Ÿ<>bnÇYÕ—³³7Ssn6áíí‘ÇËôó#^ïdcµm›‡LW2Ş|æ3lÃX^6S€5ñ:|úš[Â>H1À]ì Ë²oáMKM(/E.ÿ$xÜeÑ)O]h\èò{_±á#?Ú€HóIsR»
8/Ìj7¹ÎE"s<¸Âlœ­ß2jº”)ÒG”e„=ßùjì²ˆ®ÖĞÌf!4)Wn$WE;h¸ßİ¢²
”:qM8C’’H²#ÎæôUIîˆ°ädº„cÁ9jÒ”ğ!Û7‘4)íïñ¦ğE¸áş«üÖşd8jU¤üîºbsÄ¶je§$˜`¯ŞòCe˜<Ò=ÖŸ&ßÒMhŸ•?nÔ¢B¢Wœ!ÿßqZEæ «ôó)$s—ì’qH¶X ïÓ‹Ûı5k˜¦-Äm¼ÊÜ"Ş'K@àõ=»ıvàÂ§éGËÿ¢u•îøş´¾ªñGíÎİ¯:ÆıßŸ‚]š¥yO
Ó”ÿã½ß:91e“¤1&E47«· Ç‹) fïêî’°–Şò|ßSB²ñş#Õ=S”÷QÉ„‹qhş6jK;´DŞŸ"„”"@ş€D;üìÙq¾£-ªã£õK|!•ÍHó$+ª6ÕTò‡ìÍX›XUÒYÀ““…3ëHE÷Wı¬Ék€¿!T>ëJ’ç/ò4^gıöe&D¹Sçø$eU#¸å;.ã`ş–­é*{<Ü½'™¬¼O¤4¶õìgÌ·WPÕÍ2Ì˜&álû`|ä5”"r`5Ë+<¶'ÅñÚ‚]ÕHŞÒnÍ®mâıa°éZ‡µ =-˜¹ñuLl;éf7Ê±xsıÇqOÈº'Ö9¶*ŞAÊ½ê>ZQŸ{Ê4Åt†ßPÙµ—–ÃÆÕÀÙ(^³HL–ÈuÁfš… Íû;[´X;³–dË·İM×@õf7ÒÓŠ?C:;ˆrŒ|!Õ¯ä
xÑ‰^Ø¡Š¬êÆë÷¬i3	 Ô¸˜@<È>£œúg£Â}2ä&› ™˜`ÑQLû k29&ÂyàÂ¾;ò
& ‘7;6¦€§ÚßKĞóÿš¨&ŸQ6”({)Â¦§bkÖ:¥ªº«¦Ü<gÁó××‡‘wÈj~Øl'Ç«3óĞ²ìÕ_…e’²~Tº°ú¢Uå_ô»x­RJJÍı¬Ió½İ^#4‰V˜9Gü¨ÃÒx²{»0ˆ^d‚LÛqOªë^äÇd`>¥Õ†$'°ŒÄ„hLçá( ÁÁª707' moæ)EùC–¸P+w„@ÉéT“ îÚ_€…Ó+©‘…V²¿V©Ü-êÊWLÀ~OíÀ{R	èz6ısFoR‚Ûç]Ô3^³'ı¼(¡:5«Vd]l£„ˆ0‚ÕÆK_;Já;‚qìÖ WÆ»y¨‡™Ç\IÒt×råú”œo—”50µ¶dŞî÷ïÒ^º±°ƒU§.äÃ?‘fæ0û	†TÀãAæ?‹wHl: À‚ÎBŠ9Œ–:[]ññ`ÅŒ¹˜|qôm]Í'RÛ@71÷œ7HO¼*ä+R}*EM4°Âó%P-ÜW™,Ë¤Á*_™””uµNs…UÄijL‡¢¢Ç}ñFN¯›] ÿvfŞ°k&_QÂÇ†µDs§?óµìtÁÅ²)¨œ¦ÿ1<¯–ênæ»éÂÂÈ©ÈQĞ0¬H8/ì’\ò©¼£fõĞgõª„rmø¢må«z›—u”œñ>¾ÀÈQ’OãÄ¦ĞÑãRì¹åÆşb›g'C.‡"{õM€rA_°Of¬¢×È#îƒä»Õµeo•¥Ñœ>Ç¯»,Qq;hyéÌ(aÌğóo-$šÏî£bß%Ôé‚Ò‘-†Í­÷-´ğ|ùE2cfá¥eİŸ„-È
EÀ·0äìoq7«7„3,÷‚ €Q»½e€E¥ºóT¿uA$Ò§c×k?\Vıëÿøé‰2Ò´åğ4şÛüÇ€šèÎCåâ=Ê;(¹Ö¾­0Y¹‘§Ûø'•5|¦y7ŒÙÕOÅyˆ àr›&EŒè’‚óæ«ûkîĞq–ı^9ûyñ8‹‘D`ÿi°	`$`wmrÍ.¤œnx4sAèv¦şÓªö•Eu
¬!ı!«¡I¾Ç:¾„ Y®É Éu¾4EÈ¦uÅ/àÿ,Á¿UURÊ¼[Ç‘†)Eø/1üîÀİ&5¹Á-é±-'ª™å‹1Àé|ú2ÙPŸbáøæÜ¿¦•„ùi_"À6¥Îôü(ü›7ºâ­xÇ´D¿wÄSíQü·#uN}Cûa»]ø‹¡øÔ¥´°èI˜E¶öû\ıfÆ2:XØ¶Œ)Ò2äÙNÅ/ÓíŸà}Î¬1€ê¤›Ë"©µ¤KÒ”¼“ëÍV_Ç(Ö³Ã3óÑ³ôëtHö¹ã–íÅ·ñá"Yq-íÒuûM‡äQËÛèŠùàg½Şà:º>ªµf–’€T°= İ</ñY1Yµ™s!cT£ô,ø^®8{WËíÅ‚¶Á§ì§HDû~ğ{
sîç×x'B°Êiš ç+*¾à±pÚåùŒFúD•.Õ÷QªEô—rÇ‘ç¯ñ<±¼—ÄZ¶5·SâO8b¯ò(¿aTLêëkÄü1ÑK×øÓÁS®ˆWñ4`-f|¶Oô[°é[ôåY9ò<˜Õ\áªÎËö%£ê-‰3®8" ë	Ø$ÆFsV–Ê*³#(¾Ø>!EÍäB¾VÂMˆaVDH&a?'Äl+Ôêcd^.·së„€ô€]t
­´"ü!¿Í1í
Lãí¸Ägï™¹`äáR9İNïÆ0À7C‚l2f¶XG;qô¢Äß0ú¼¬à`Ò¿ˆ…úŞÔÂQ™~†Ï3úpzğ*p	Éä+	‹Îî7a÷=RİÃÏhØ)—Ö©f&‰ı¥."qõjiŒÍÌÀYŞè[û?ênå™‚¿e›sYÄcMÔB–kÖšn±yÏ²špºÑ›%Öxê¿[w.Ì1©¢
ì% )([`kBåû4‹Àñx•ÿÈ£öã›ğµŸ×ô,+ç®6~¨Ÿ)ÈuøûP}‰ß0P‹ÖÆÂ‡K‘2wK×oıÂCjÒö(]ÓD±Ú‹nÀ@ä…é"‚õ+,İñÜ’~X
¬”ï¢uô¼y]6+§UÔæšî¨h¿JºN~Iß°)ì™f!(÷Ï¼Çõ£ ğD²n[–RYÖÚ{buöÔÌ+'™¤ÍµXë£W	OòlLS¼š0"&^Xûœo9­2p0ñhõür²WKB‘8 ™l1µë,ÎƒÛo²PÍM	æ×ˆ»$ˆ8œvt°¹Ê&Ã¬ºÎ¿b!3#qàñÛ(¹‹†‡æã²T¨õ¹½ .‡ÔÏ"’:ßÑ”|aò$IÒö±“	æî0sàŠGÊMôÊHú5àˆı}b¨Ã'´oÔ/e0¿=ø™ñn!Šà:è9ƒ·Ö­åå€Êª-IÉÙæ·†>Ø´Jˆ:(³7KqU‰‡‹ñ$Œœ«Ü^)Ş©¬QÉ…/Œû	lÿR·XS›Dz¥#+†’áüd>HÜÈÒ_íâJÙ<Zè2uSF­ÖLó´#p¾pøQ†±nrÀØjäô³Äç—öõøô™`·C°ÅÎÙA ’C†[_@by…!B×˜38O™cƒ—iØÓZšÎ³Î÷¸¼PD›ÄJCóGùHIYâ’äWB8‘ ø‘,×W0º,O‘&L;·EÚ‘ÎN*qBcúÍ“ëæüÆé
Z·(T6ˆ
–)èF³b;Î)Kc±(—¶äõ¢óG…‰ŸÍq–‚'IŞ.= ‡; vîzg;õ€óÁäK!Xß¿¿á’ŞŒ4q’cİs
©°VP;Ô‘§-W#€cıCõNbHïİëÇ-g±âÃØÕì$êÚ\-¶ªñˆaƒ½³½ ëJŠÌ}ïò†ìWGàŞHYfM¦2ôs¦ÚÔû^²Ê‚ïÃtŒj,W¬mKTxk%ğå]vzm¼½ÔiŸR›Fé¦7‹ù’9¹Xœg¨÷(11.DÛõ¬âÀ"ÖæzÏ´¿³¾ÁdÍn1|C_¬ewÂpn¢øÿé¸—?Â¦•NaÛ&“¿^ÅÖ~•¾ÆıôÆÚ9¨„ş^ê&Êò¯H©'ô¼º¡A]ËK[šÍ¦ÓU©kõœ‰ÔYd—º8ÔDï–MVX¡¡~E_×†ŞœéÀ#a‘6u`/IŠhƒHôæFo’¡ª$´¼‰‡3¡%)ZŒQì) Øé*Wò—¼ f×ŠM[®ŞG.pgêTpƒ©9›É°+ ¥Koí,9¤QbÑwù<»Û9¥øüüÌı_Í>‡_=y©Zç+³„ÜW™Ûê!dUÌò=«ÅëØÒjµ¬l#1’ù­€@øE`,„ÄÇœï5ÉHU6‘¬ÎÆN»mVV{mãG[R|éj·„Œ1°)ÃTád«*àpn!•r.½YYkW{ô9Aï…ŠŠæ.·,Z ˆPıç<¡+3B§É(=!<çöZ$™'×,á[Õ~Ñà0Íƒ…½3vJu”u3ª²¯ØaØÙˆˆQVW5 ¹H¿3œˆªJÀÄ?B›Ûªº7ŒlÈõi!¼ ÿ*Öt¼ëÊŸp70¨d1µ~8EMXıœÚ ëŒÍö¼¦œÉ²Œœ@	YVºÂ6-ëqm¹ŸÇ…Y1×7Mb¥Ü®Æ’XÅ™¾Œå7qKX"¹pããÄ,Ó>ä& i•WÚH+†3Ş]6CƒâÜïüÛİRä(VAkÎ´ÕÛs¹šÉŞ¬Qà9 }}å¹;Å#x‰ü.Õä§¾şJBäx§rÔ>Ú*—ˆhIü5Úç+âQ·©¶ƒ‰°ä·••qù·@}yaj(íQUÇoG‹3y¤Àğê¶)ğç‚iZÇÆw[Å¿ôÉ@—¸ÌĞóîª¥<X©Òû!DÂÛGn±#Ê8Ï“ìÒx™Z"·{ÁmÙz£E>«¤\ïv{,/69¤p7ì^iÿ2²Ş­_åÙKŠêsÎç×Ô;dª¹UâNÕÈ—§
O¿zM""eõiÕ®ïV¸:Â…N20Èo-%TØc=ŸÁ’euDF¡ÀKNW;?â/4b<›=“¯ƒÂË­±¢ÃeÇ)©¸d6l¤h#Î_ô‰ƒ6£H2¢âwÕbnÏËu·n[ò2T^väŠ¥DŸ)	àÔÂË(r‹XuE15€~/ˆ|¸å†8tø°ª!\±u'm¯of¾­\â
ÊÁò–ºsB2AÃ¤=Ğ†Ùßfç‡Á=ÿ3lÛrk |>i\Ô
â2´™KöÕE_DŸÛ®»oqA¾´TH`_¸²¿4™_ã;{ ~ìÊ—!$œÿÃWºÄz…éP¾‚ÔÜ|@=ŸÇ½\a°àg7úîºDGÑf“¼¯î)c"{p^³ûÔŸÏ3ú'pÎ§ˆx'JpüòŸ€ıÔ3ä8. Gk÷O©#øfg3zm³‰ËxAÅœÒíçjù¿ŞÚÆ8ŞÕÆ‰\IHë·²~`cfŞèÎVâôñ»ˆ_¬Ï^±¿Sdi
Å\şmq-µæV{	XÕHÅ½8İÇˆ˜ @mĞrPæM(ŠÎ$ıQøú¼¯Ğ°¾ ÚÍ‰Ø¢Ü#…LÜÔÃÅøFô > M2¸çõoÔr æN4Î$ıš€((*sóÂWuzÕjéÉhàôÃH¥üòf×é¿6À§]ªK;GŠ©ô¶úÈsšrEYşyA¥•$İŒÚœUl^üü¾Ó,m—Ò9©ş»ïŠfzŠêó*º/ÙÌ]j’8lWJ„òˆt©$‡Ã¸’ µ~®Œ”`Äš[å¡"™‰Ú Œş´£1D›¨0ı‡r«Ô¥İ4îb6ÚİE\O0+;½n^ı•9ÒÅJô@*¥ã>m1AeZ§î¡k~ú>ğ¦Œo(åõg'.ßÜBï…18ÓÉÇ)S¾êjùš@©3¤+ÀõàW¤¥°k*¹ü\á§ÿù×ıÓ+)uÔú§ãµüëNM8ã·9!ùg‰ñX9¾˜·‡e4ÇG VM?fålc²¤ë·L,sµ¾×f¾İKëNšj’Õöó_¿
Uw•æòÕ­ö¥™g	Æ">}¢5Û=V±då^ù¾R^°«¼á Âéª>ÖHÜÎ9vÄpÂIa¦®(ÆB6uy.U¸ÌüaM3‡2úÓ…~r„à€9.ÛÀ1AGé$UÍ‰Æ™(İ¦uÀ‚Y!{™É§¹Ó
·»ĞO~Ìo8:A:K0›%`ù:6V€ÅaÔàKµ»73\hÂ]–¦t¶-©\á"-ì¿ºHß2³ºÎ´k‹2qíjeçåC8ßPÈq»²8ÍRÖ¤åø}°»"ì »ïá"˜øaò€b	(\ä'T/3Ş¨ÏÙ‹É¦·£bA/qª%…x¡³pgÓ:BE[„Z=´¾ë0€ €iS¬%ºa]Öâp‹F#à@pzÜnƒQ®Ö/•‚YÏ[ŞµHı38?z)Ä¤Ï’uN”$S·Ê0Ùš‹ŸßÕ³«Lï%Ó3ôÀø w~ÅK¸œQ<Fõ9MÂEGe`İûÔ?Vê’±ZLkÅ‡![4iå§âä¨«÷¹bµ,ÿXË[·È]¤kkZ„}M<¥¹„Ätß’íú.Ô•îñ,sQ g7…’«JóW83Ç<	Åİ|õ¨ø¾ìH’_ş<± ğÌ®Ì|aù¥6»`æœ6×p='úˆğP"î+ã¬U!$äëBJŞ8 ²´Wa’qÑƒ9İ"RAïßğÜXj˜¶`gùğS]ƒOlªÇNZ‘&}%Mz¹p1É)LCÆcÍCşŠ½‘z^ñ§[Î× \kycÅ‰×c4Ä_¾b‘Zœ•øHm”.)+?}f6œy!H#—d0Œ@ï`õ¯ê¼ˆ¦›ğŸùGâpdiB°éä"[‰’}„¡SĞ¬»5­ágD–zo5™èË-×.%á†3%èÉş¾¤­œÇ§\dor#ÎÅJNĞÿaD™½Ùµıå)ï
†şt©KÍ£ášøi¾æqå|_;™bª$;Ømä6À³ãÂÜ»´xŠ3" Ä–® ô»mK´ìüçi·ÈÏ·Ş¦ÀÔø.5@‰f¦§T™ü».‰?k§~ CôCv2*Òˆl/³¾ ´q­íC“Uı».D¬M€8ŸN$ïKBKšÎ°ÀrnÊ´WÁìœ¼¿GÎROeXnòwĞ[Åø‘ËUÀÛBÒ§¹Hb|f±Œá¼$^a"¤ƒF¶•÷]Ç%•Ô*lùƒåÿcr½p».(3Ïóíƒtÿ(ÇLœUCl¬ÏE YÏİÄÓ~³k¼™³nCx¹‹Íœ!<C÷¬ºÆ¿ü"}$Ìf¢Î;9¬¥Ièp\3Ï_±ÛüD±XU‰Æ_Lùı~Ãô›„ˆêöYcòÂ{kÿòi&–9¼å.lxpµ+ZgĞ)_ıDåD
ãI™*«|Ì/Kp-iVe!øÚ„+]¡±¬©‹•kõ¶C?0ğ`í-éâ•Aıy9óU¼=ˆŠêÍå‰,)f”DÒQå‚îtŸæMæwÅôJ0BŞ6Ç=WPS,‹ ,	N Up¥®}Dûa¿liİ8
æ;]aAv™I\ß¢ü„¸i 3Ë)»Ş™Õ?ó£¢ªuÑÔ0d{øµl{ğ]ê7eÖEQ]²­ìñüıÃ3` W\ã>Äz¸eMÒŠò.ŞÒO¬…!ì•×¿ukØ }Î!É	å{å×µ*DF—¼hÙÊ'EL¯õ†\©U¬*àºğlY¯ORNsnù¹l<B9t ÷LßûOë·[”myPh˜+ÚıÀMXsîa!¹ğÈĞèPhƒ¾nğ—Ó ”šÍ@êbí<£«k«LÈ4¬sç®”%[Å,è_ÑfÀ9ÏEªl®Ö’†ÕŸ¸G€*“œ:_0g•5şdSû€a }«Q8•¡áÕJï ha@©{ŠJ11r3E5iqxÄóÓÑìÚŞJÊ«D¨íË…Ì©“dÑé<¸ñ&…L®aÍ†£UèÜWYÔ½ÉÛ„rÚFÌòo˜ºğ`;zSg `4T'AñX³d2DåÄÊßZKb“|.¥²à½Fûâw>„‘d®M½³ê»AQçÜ^æ7Šş¶ç©ˆ`àDI¤ï{Xû`¦ş­’Âà
­”}KHú¼Œ<ıaº[ˆ~Wêï‹{EuÚ Œ:_N`æüµ+"áß9s?¦@1Kíª“ûŞWìmîÂl­¨ÿ¡	Ëks¸˜Òã,×k('áDÉş)™h@şŸ„‹I‘-±(ˆ5İ‹tp(
XB°¨¹n)©>GÀÅEi#
H•’ÉÙ!UÒ°´è±H2•¤!˜¸í&5WlšîÏ2À$tÁŠ ¼§b©«Hzãàê¬Í,Ğâk¤¬¿½ş%¡îü0hVİ«-ò z”4»îi²xS¡€n;h&”CúÁHÕÏÿ/(®
ıÆ°4rÂŠet}é¹I•2c©¢5‚ Ş‹[9;B–n6 ã/M¶Ôé»ñZaßó¹hÓºT")zPˆ{”sÁf‰F+ED1³«- ŒajI/0Bñ;^	´yıµ-o$ïRDáİÊ/æÛÅYÿØ=Eú	½G¿”ìì£7,¬F±^¬İhÌzıE‹ì	;§4ñÄÂ§8!ZŸÌØĞ‰â›åj>f’Gğ¹>Sæ›G5´fzõ¼x†g]¸ZLYr„’eÍ` /aB•'U§]Ú—z:A™Ò‘ì#â÷CùíÑ 5eÔåªVîWA¢ºÌçOš‹U”!¼Ób»<	‰c,×'–€là1n¯2ş_‘.Ã-´z‚ÆıIÕKåH‚%S¶İ¸Ï´‰Îâ–¯e/‹|ê¸Ü]Õï[:T×
Åù‹<a†t'P<fŠŒíùu ÆUvŸ¶¹7>³ıF2‚g›gZ‹ë®Á¢(XM¤ óÁÔ#*ô’”J™à®‰‘Ø|Íy~3 ¥¸ß²ÚvZ¿b|oLåCô8
ŒLç¾ñú^ewkÜággGîÑ+Ã±[‹­G¸§ÀñKú“B|wBÌÖ­¶áÔÀ¹O
»ÈlßSÆåŠ+şîøWü,Guğ>ö´HE€¼§iSC>˜QäıÆ#hÙìoéA»jQYAIûWåe,Cš ïJ+cİ”$¸œsêÿ\îÎçö Ÿ£8ó<oc‰‹7îüĞØ´E‘ŸÇBT,Ñ7ˆªá*.j.9Ri”‘ÓÕ€[ê`Æê¡2t×«±F™
eC‰¯z	:Ñd~aJ"Ş
¯ÜM8
Û¼ŠÃëÂ„–lL'Õ%dûuÀ$7å6ûÓ†#±m?Ì_é^;…ÔµàĞÈ7$Á„ßà½ów>$È•xz¾­R¬\bCkHxĞ¯],­D;#X+/°ˆë¥NIüiĞÖ"›ŠyÒGQœ¶
[ş¤&ö¢áœy˜ıÏånİagG!Ï°:nWèÿª!ùD32ß<¥Ìx†©OIx¨Ö™«;úÛ¦ÊĞı(3~ôpÊ¬«ŞÆ(¼Ü^³şÒä!VLŠ¿®Q
,”¼ıÕõÖL+Q‘+KdÙ€İÙgõ÷ú/(æGñŒ2=^…rC²uİîó‚1Á‡H)9aŠ±~)hè£™ÙÆ'Çe6y\TÕEÂ‡7Î9@•W‘Åa:Ò†ŸCšZm*óôÉH$Ïåb ÿğn\ro~.&¿·ş˜C~×f		Ó8$û4İÁããõ w,Æ[RËä0+Ñ¿²õºQ½&Ë’a£›‡¢PĞ²˜!Ï‹ Ò?3}OÁÃÁ'³yb£ÛEyE«wA~Û¿¡v*õVî«ëË‚ÈŒMıJ®@İchÈ†0HyûF²½éìğ~Äh¯ÂöÈLAt“C"Õ2n²‡ĞÙ	~ùgZ_ˆû%¢Z'ÙÛbœ×£ºd«n“nÚÒÓli@,•©5r®­~s¯NVöP|5¶ou(/Üı˜/oËpiïFÙ¶Ûêâüítü*q_];VÑ£ÀqmûU.n
!A[«æŠó†bšó-ŸŒª0âÌŒl´¡Š=!Š‰&Ş[G.\/M‘åø7Jğ±¤Ç­„›H‘…Æô‘hı©Wİœ»|Œ¹ÔbM!û’-nÃ·áğp¶‚hÿƒc ÈÚÆºÏ8_ãT³ş8SŞÓHfámû	é`²_bL¼.?‰¤¼âËq!p¼cK™#!Ä€À2Õ’Ôˆ¿3^D ôeœQ¼‚°§Òn³ÉÁ³½|B0/‘XZ	¡´ÔRŸ iºI ŞıjFM½¹ß€â¯Ÿ{ğ}}äÙö³62™§ÿwH*	1º¸Ä..N[ÜÔ_íè>¦”Èy$ûxÄËağÃ´É"`Ôªïhš…#ÿÆ£œ÷ğÛ•ùWb­Êz«ŒƒŒ,[&²h.°ò?Ô3•+«®%_5Ww å­´Í”“ëc±)Şº…ı*·²)‘86œ»sÖô,IÈÜÃ‰ö$ì1­ú˜‘ñdÄ(_¦¦¦A4šßEÛövÜ›ş Œ‡?@ÎHb* ³ÎÚbs¥+hˆ·VßŞ
ct#mˆÁ8ü’]j=$D×QÚñd–fb7-5]G®w
¦³&7§ï8`Ô”õÊy«ó™ïW'Ç¬^ğÑBt¡İ‡pÛoXõù‚t47—y‹áŠAP-RuÒç39eåßé±Úø•™3ãPîƒÛHÃø˜ì¹wî[õVLK­Y}³Ñ<½?QÓ‹Ãì°ïYd³¥E¶Ÿ&©hÌ(~b6_@µ‹¤—9v„Ğu,SİøB«çİâf÷!Ğì‰ML¯Úİ 9Ø‚Í¸RåóU¬kŒK+¡
ë®/ô:˜G½²IÄÓ£k®àÌ6ü¤¯œVSHÎŞC(WD§’Oc	ñ†@
Vbˆ»%äé{{ÒÃ&¿dÂÙ<",{tBºú»áVİ£Ïí`È³;/ÏiÉÆ‹ô‡â³&ªÌÆÇ£ 1-0‘4Ê½ã<–‹„%üiÈe@ÔòWõÔÃ#iB—ß$u\+&³v	í<"ªn}zXL˜+â^I*m1¥\k{eYÌIp§Úüï96`µ“ddàüQZ¸”ûQn…—ı¹^6áÄ'¤ºõ9æ3şèGa¼È°p·Ú «yv|¥%Ï\¢œY&.Æ)%MXäÂ{ÁTƒSMP“²äˆ¾¸AØ`T0[P‰Te(bB€8“#^¶?vM®‹V‚8ıEÖÄ6ü8Vôé” !úSÎ®×&êK'Ñı9ª¤¸i‹w~Xdn¬Éş³O]Q‚ÛG‘4Jšr4õØˆŸCº`¶şg‹(Ô>Áşñ8yØ¸ñë.h‘_O¬uõğ–î ıØG0ûì¤ÁïİÉ€7¡å '\Iñ£U]¯‰»e,¸ty–z`1 å‡œâSoË¥0¬öºûxNZFxÔ®ï Fô3Ó®IáÒä†£…+‚’ÂÃ¾Hyû#>;Èmà(BòÓ¶,çAI)ñ,ó‡z¯"é;2´©@Õs p“^J‘?„µ×ë+÷ƒ›¤»¤/»²HVØV&ªL f,5™ËS!k±¥©o2‰ÕÈÕkÓ
\›ö…H÷Ï{!²KÔ&M!F2%§(×t7÷I]İƒO	 d•¯—wƒAd*Ÿé$´r¼)aPu1}¸Øº‰Pï	òáHùj&#^ÍbûÈæ!+ "“*OpñFA±Ô–A¡V×Q1•,OZAÿŠbCX ğ‰ë§İ¤Ä[J‚ƒ–=ŠRºÚPsîçsQ’Ç…ïW–á˜©kl­G6ŞIMXz¸w•Ú…ÑµtxŸ ·ªÔìrÉ^úÇ)E;—Ğ§à.=Yv|¶Ñ/J
"/[`Œ,·°¥ûUtâ+k“¼%şØ‡—%C³ÑÖY¡Uø:À+ŠW‘°Bëƒ¼t©¦ÆÍ¶KÜ›à"ÏJ&6U!z²¿+2¸Íö»—m0N7Éıxd¼UğÔÜ$ï—›”IÚnÒ Æ™{÷šİÕ¯Q"ÇÎ¯AÃµlbÃh%¬zQİÍFènâ¢ìAÍi^7ğƒc‘Aö÷O»×I‚&âAõÊ¤zæBãKjÛé½GJ>Ü¥]6eæÆèézQ
“/Iª,\ôîÃ¹Â°Ò¢wŒê&/u$‰«ÃîFRÖ’¾bñÎŠßµ"—OíYa’oâñ(«»»NÎôâëÌ_bŸ­tğZd+oMÔ×†t€Ö¡™ƒ©¾QEÜ(["PL|eÍf­İ…æ®˜e|HµAËİpÉ¸ÀÖCAŒ)1±j5<˜bËbºÚ›w_[|±Bj”n¯¢+ÀÍõ X[1nSüh“i*6¡É19Ã°alæEâÂÅ8ˆÄOy³	å×>aºŸ(S¾‰öÕVÿGLÑüØÁSê=ÓıWâˆi_¢?õ$®
«ƒxy|MI¨›5A^IKmè	 ¡ˆŸVjØÇ‡ù{Æ…Òv:µ-]ñ{T$†7óë>[ ìdDX~üü
ûC:¢ús™+WçZ–+|—DÛ ÔÚré©iX›9®ÈÓ":%DşŠ“PYVF
Ó9Ÿy2ÉQÏ9>EB22Z;vRUgÿûmD-ÜM%ŒØö
Û Êƒ D¯¶z>ö)µ· A6h–MO?ßVæ B¢G5ô5Ç/J}N<èáÇ#‘CIò—)š=…OçY%ôÛDénêØ¾Û®#ëAˆ{&”YõºæNÇ|1	mµ!»N^¬üƒméŸş6úØùî¾bu®œßâ‡ä?æùÂ[Õ,~F¿j€HY'iU­|¢o<Šš«ˆ/	ÜOiu£ï$, 
Œ¥$»dr½àÁN³YßJŞDqF¹ãèò¦J¦ùı5fWY ÙIŠTÓ
ºÁ²»'0ë8Ò¹)¥yƒÈŠk£‚]Ù`¥„@GªS#ÌÊÒ|µ1"„a³ëù$ÄP½‡™+°Ù®Æª…‰s_‘eZ¡Û‚i‚lœÄeÊ¾·,ÊhR6[‡¿ËâŠ@Îˆ¾XGª—
&å(D&9£ªÖŠ]2«K6á÷~™†$Æ1$ı]*tã©gt;TlÔlM,]½ér{¹‡'Iº… ÜN-ŞÂ†³CCTä#}¿DJV`bÒÊ/”2¡-cßOš:jYÃEB#¹Úk2‡ÖSœÏb
×5
B~Á:q,é á¿˜KÍbN`à$I¦àJ%·S°ğ/åD7lÉ±f	`pvÖƒZZM+|ö"û–\eL“Ñ›.ÇÆúéÌ®y]˜Â‰ıéÈÓ´êÅ¬lMXcI—ÇG’Cw•Ed¤ûÛ´ÚËµğ:ÍúÀ^”3GA“ l˜p>5ç’¬L‹„£PzHPÅ}‹lëØî›èMİUšÔDíoãê<´9Ú3{¯N&°8T1ÆÑ5¼çÆ–òRr'ŞúSy~kbi÷‘nP8ÊPQªÆrBòÃ.¸I°îT|ĞœW(qMõ¿€’¨ŸmQtcqEs7UóV-©o"!cÀ
ó2ƒøP<´¨7‡V®Z˜/#ô6âˆ‰òÎÃS¦Ç…«B¥Æè³–±åÒ~Ükéµ_Ò·ÁÁõĞlVZ‰hÚC3[âí~¥^˜ƒ=r!$t_WÉIÏğSÙ¦º9ÁÒ·Õ¦ÄÚV9h\ë=wñ­<lT8J¯ı„–¤9¦ú1¯¹Â¯ãGÿÏ0”9¦òÊ~«!£\/ó;fíyQ—ÌÂ7¶¢Šyje=/nKt$Wh\Ã^«ºmù6
µĞe˜¡Å#àı¬°×y<D<j1XâJŒëoÃ;”(=İµ9¦pJ½÷ŞM>½Ëúü‘íÑİ#4wyôì|†Ö².? ËÉ>6-g»ñ– ´ÉMÏıÕ*æ7î°I.@ÁHÁ×|4b sú
c()WwÉşÂø>#"®WUZ'(¯‘]cÙÅ–Uê@t+UŠ’şYÄ›¦U…¨Ï}ÌÍÑÖHù†¤Ûyøxû«Eû(ŸŸ<û*äv5ÿ´o¹xKPs,Æ\ù6;j½åV÷×Â}1›ÔñøöÆLûpæOFF=ÅÎ5¦n€à+İ’ıµZ†Òğ	…RÖámOh7Nè.š”ÎXĞºŸf§F††÷€veZ„‡ĞPù¸—×•	ï.ÓEÙ§sà2Põ®~KsAj­ÿhdmÀâ\EB™l]rv¯·ádòÂƒHIa_HÊºu½¹A_[U“1ß¥®(yn>¬³.İ\}»´B·üœzƒ¢®ƒÌkN÷¬WM.Ş\^|`šš<VN‚Sr¢ÈÈ†W¨¿ &Q‚ª}®QáÎÇÒ[…—b§J lŞã´4’xµ¥a	¦o0E½Â¾.k	/Ø•Z¥İtÙ¾|ŒÜ7 §t^€÷7ü¾9‹mÄ¿¸è`'â6¬ë[Øô]òNYÊM0u3¦­1PÒKúÖÁĞH¨Oû²xö„¡©FOI¸Ü—_8	ï[d½uZ.ãJ*¤cµ|b¼Iÿåu÷N¶æò7&)} -.y5æ@LM§R€h×jı)/•é+ÚJq½¨›	‘DûuÖŸs_ìWìZ¹ö™){V‰‹äc[Œ!‡ĞÅÈ&ÙdfÎS]°àÚ‰¡®,»´}2
ô¤Ê+¦W¬å¿ZF§-Óq$O¶Å'-äJ+D‚è÷¬¿DoŸRùÃOrø©”|™£¿eu–$Ñ´R9ãT•ªzWN9q”EU
ÓhEãÑ‚ß’]HÑ6ò¸àjc£>")Ùcàx”},mÆ#´Çºš¿E™´[õ©=ÇÜ±&Œ¤¡ö6$Š}ÆYu¢ûÔítü°Ù”z‰] ÏNa5ğ«¦]š®ÍÁ„üô5Iñv´ÀricÕ!#JT´ŸÂß×Oüàû.­8ŸŒ2Ø·B7½q×np»g|zí!ˆ]HÔ)üİ	C)Aûù@MÿÚEÊ€û]©Â3®<k¤@ªÌÃ­fÏ|»x¼=Î‘Dí2Ò<ô»Y{t'DÅ8cif¶ıÚWS›ÌÏ4ªŒ-SXE]¦(xâ·©P ª×*Û øâDİëıüG+!Öa·ÒAì¾àõÖsÀfK+™¼]í+½×³>2í9Yk¨H¢^1¤üPvD!V+®Ù¡-´L3!{6°%0Q.¼z¹’€	»™õ—ˆ¨Œ &tx»x‹ô[
ÇÚ?|¢:K¼ ¿ìßƒ§ıhXhÌÆ<«‘F9NHá·`5ÑDeo¢ºğGƒ(úë2—¹†‹C–- Á~·ÌƒÏNt™Û6È´D*—u5uåT ÂÍÛQªûê'eCU4TÎ¥á0Ş1_!•€ö*  Şf<ÜOVËÍr"6ÉíğÙÙ•[T(níWM¯Š²ù÷Î‰ÓI¤‰L{-²­õ*å³Iş(¥¯%Yo},ûóoA&|
ÈX#,“BÂÉÉûB‡Ã^BÆıã<@şòBqdÉvÎY™Vç°şêâu‡n	F'/Ù+Â€pI¤´¯ø…gí¿wÙx…û4)/ êÏÌVÓx"6¤¥\©`-¦×Ó?é¤‚Vgû"dVR'"ñ”»äq¬ô·o<c¨u“òÉ¶j* YíWêÒ-.­û³z§…ÓşCÃwş}¬¼Ó([5ÑÈP¬cšdìV¢ÇXÇQ–¿Ë·YŒ¯pØ?Pš•á‡væád™šèã}¤*Uyv>ğÈ5†dğ¤Ö2ÓK`êù|ÌY$M¬Æˆ<;¥üoGıÛÑšÀ¼
ƒØï-(l;mi±¶é¾·tOgªìÂ¡Ï›ä/a7Û÷BR4Pà³;—/IÑE¯1Â_ÕJ¼5EW‚¯ÍsŠÂ…p,uİ<xêCpöoWıÔ[º`ÑJƒ57—ÎB%¾Áp#ÓNä”>#;CS3r?DÓ İŞÔ®ç‡µ&RAV/LKŸ©–î²ÊV©x"BœÔêé‘U«¨Ï6È•Ä¤ÔŠ¸¥êVšMªlhç|½lo],Hz0.µQğ HË#Îs¶ÓL««‹7Ğÿ¾í-OåQ#9|^¶{³ª¬l}×H)F»4ô’‰78Ö‘ÆKÔR KŞ@¸tîÌG±Ôú¹Bã6sgn·`Úİ"X«PÇuèò­ä¶NÆHZSAÖ ±">Â51ôÛÑø
Ó$}.êD±_#‡-”&heKgf•ˆ‡@‘òğœÚ|ğÇáÌuR*±‡s‰ñÙ³,Í·å\i3r†}A'¯*×£Â¶›
VKá™b…)¼~Å·}!]l=¯ëy¦nÛ{âöä“ÎkÕ•TæÄal9à-Ú´añ‡]b$(vëÔ@ø:Z9œ0,ÆÖÓGÅRty¹‰“úZÀnã«ÙDŸşdoÏşî\"D˜&ïñÔ0$”-	”)ÕÄuüÔ^6‰ã“XèîH%»äƒzyì5¨¦L“¸n¹M&¤nÈ#pEß‡ßx©I-Å¤ıäH±˜e %Ò~b5õ¶¨ÀİÍ÷û#Î§-JÍ¿]¥¦ø%{v
—Ïqı<ù¡Bølùu]t£“æ¤sXzZhÖ"-ƒ¶üz,»Ò§{ŞfõÏPE‰—Ş¹e0´X~'qL\O2÷5¨'®*©$#Õ¼@9àºağª©¦Bj‘	¿˜@ÓškÛqüÂ¨ŠÌbóEÎ:Á×*<’k‚p<ûl*ôgŸâÿ[brîŸ‰:œîÒó2Ì-óü6Ò”ÎX/œlU×ê{›ş=³‰núÉJ{V°œ&émM ×&ãÃ
‹MÑºó`ÖBWª{ùûÊF‡•9ÿ¨;ö>sÇšÀD‘f¡ª¾y*~hº)1Ü—ğ¨x
JñÜà¢q>t(|?8¹·îeZ]ÜilYX*ò[•Á©KsÓM¯€&üQãk#œÌ8ñ¥Ş,9ÛY#ŒzÓÌ÷aì3’ª'¨Zc.}í§mÎ«ÂÆÃ‚"r·K"ÿfœ¬}–ò pƒú°¼¿ÙJşºí…!Ç¬á¥ıºàLdì)·Şô×¤"“9[²âº~n`Ø¼·Ë,rjïŒÔpO/-d™²&"†`ødeOaBŒí?/ªGqûnr£D'˜R›£p{EgW”ˆJ…õÈ[,ykŸl>1­|‹¦†©ÍD­]¯ü:m¥%ú:ÄY†{¿È³z·~	éø÷à‡Eû°oÒ—ArÎòZ¿¬ÍR‹×èşÉ¯
¯ó.KYnp¢zn‰}
%Ô¿Œå×û™›:›XàêdÿZ—QÎ”Ø0>dÿ{¡ãÚ!Ò5/f0öĞ2Síÿ¾mæÅG’FU¬AüjŸR€PlX¯;™rË².™ª¸Œ5ùUë°,ca`ü¤bÉ¶éÅaáJ•èùÎÃ½m½w…øüÌ¤e7z#ÁSæÎ`¶w
v(#.Ã=ê;ä«ÇÌ‚’ aYAålşÒº‡¤Y2V!º=ğ“K­+3K¿Ø¦À7/fë¼9­Û­îH|ØKƒRN€ñ1¸uöÚä_†4èÓß;ËÊ»;_Íò^Ãíy&)Óîê¬Üƒf¾u–Áó½«0(S÷7×L<" ×¼‡t›ìıÉú`*>yh¦eœæáw2İÈèøA‰^Ø_8sN4¾2(z¨K=µûşU÷e‘æÀî—½K	²	lófÇvÖ£î¼RºÁ(®Lñük†ÔšÜ"Z%õ8¿íu«?_+UŞX–­
OÅ‘¬<Yß*Oå”ÜÖ}|âWvOfc»qOl ıûL‡¥ÿÕæ")—Öl B´•È*n9/m5/›½USÒJÅÔÄ =µä,Zó}ËU)3ÉK"C º/D6ñM~Ğv }B¶PÂm&êÄ+n©Ş®h†ê#hpN;lÊ’9°œºÿ£ £ŒC³v2`/Wæ×9^ä1Ş1ÇÚìïğs‡«¢•omgB=ÉGEAxCkù|!ıåàâno-ƒä‰(ù´. ¨£õİDşp	Tê/T·{t„æIöG´NùÆ]¼q ‰Ñ±Hgª?v’#ó‘Æ8‚:%ö¡+âÔ	éêğèxyêmtHÅo90\0r
öã:ı‰	©÷å8Ûºœ¹iÒ†w ¸€véòÓÃÚÛŠeü¶é
¼êJšÔ!\~) û—ÕQºA?– F0øœë««õ‹ç\VD‹¬²“-F­øeµ<d1ÑÆ½i.ıÙCÈÍNëÌï$Y¯9o‰·…ÊşÒæ»„ÃN·Ğé‘}#ú@_»ôX™r/XT&"©<Å¼#§Ôú8©ÂÄè&@2Íxä¾ªûQo¤HĞ"„§5|ì…kÑÅ‚²ânı[ÈıùÌ£ÃL ˜Âd÷S	yEêimÃ?Ç“_e‡ä-„©|ÏÛ/.HBìéBlqPÍ»aî ©1DY¡„ßügp—Pf·1¤ºî;SKŒOqŸ#¦ÚöÔ®‘Îˆ3×ZHLö‰Q›Î‘ÿÂ1Ã¢øŸãŞÕ@{7eí?½ãæÏ7À^dşi»=™úd‰bo×Ûy DğgSƒ‘RèíŒËzr÷×<äí¸†ÿëx‰œ>fw¶
ëÚÓÀ{ú9É£ böz~‰AhUxêMÛLÑ=Bèi•}†G¥sµ—ûİz(y,¯LyÁc¤¨s²[¸»EËØsKş$‰+ÙoCañfÙaä›Ç_	İ©F¯º‡;á!«1ˆJ·ÇÛdoåpŠÙÂÔC¨÷Ç@¯¥ïÇ¦TÌï”Ç’£PĞ	öR¬&ŒD/ğtiµcå\OT!•f6Y‹ØÉ˜*^lmµ£F2‰•‘S1èê?e8çÎWP0ÑóûèüµÎwËzôxCÄ4}\™îí8X%¨Š–uwR_fû’\¾¹‰’h
:¿­Ì¾Ï]³à  ¡•Zí^<ş/»½n3“zYuÕa‡f×"¨SËœÀ[UÃ¦Şò¤Q2#=D,†Á?81®%FÆ¡²]‰áæ¦x§ÛºÏ¡)~.|Ìàûœ5'æ´>ÛìÊ4â>3lURş™ÔÖñ‚I4nvª¹Æ‘I:b%ğ“{¾/SÁ-uîkÖı´×ØCQW¾½ÙEwtÆPnèº”ÁT|Ôiù¥Aj—™ùŠga¾"â[¸plz$HêWµÜòöõMàİ”Ä¶§x¡m¶6)½&W°7%{˜iö[°
V*Èl®cQ’´Ç×©ñWçZP+×ªQ2§†óÜ ş6(£‰“ä¿É:„BÜb‡(YMN‰—È÷§ÔË¡†”ÿ ±¡ÇAkóíJbáò¹»˜Ï¨9`Çì0ÄêçSrØ”¡X‹ë}<¶¨!§³©›#É7ÈÌT›t¸ö—Æxğ&ó Å«…Y	«Qs ª‰5a„ét¤†>ˆ…×¨Ÿ’+ÉÒg•ÙÓeïÅIulLWÑuSòÜªbõ´¢©şäÕ½òláÉ”@íQuPB«ì†‚$·°™vBc“«¬NêõGÜIiÓ™¿üñ>ú”DãÍàx¤Ê´ˆ˜YjF“â`„¬R’;è*‚,V.«ÊË²¤ãÙ½.Û£ø…DrŞCÎí·<$¯n’ÍÍalÇ…]×b–[ÆLh3¦o
±İz5Ü
@×)HÛVJZªÁ	Bİ,Gwì‘\ötª‘|ˆ€ñÈÜ‚7C.õ2Mûš4Wøı	ŠĞR|/1;ûªd¾´²•ØÄ¶8Ä¹ˆ«Ş´OQ¬ p‰}Xõ£{Øİ¡ùA—JcLs7ùï4›ÎİJÿ[vœyĞyï ,N÷sã¾€Y)nïìFltip0âmZyB/ŸQ]a9ôG_«÷`éõ…e“Å†vâ”ì}f’?Õg:æ:42ôÒIŞË¦ñpEP-”%?Ñ‡®Îº"-(–÷L‡c1m '4iANc
ØèòjLB«D'ù×ëXô1dfz>®YtÄ)‘«¸à6KLÛ3IïGí…?ğ„ø‡e6Ds#"¥	•ƒhF«œKØªA@ ·*“ïĞ@øãáÈÉz×‰ôËq»’ŸóZwY{â*ğ
µ]”( šœ8ä…ÒA¨ÖèŠ¥¥cVÅK%DJ–Ci7‘k˜K©ô:ü'h…äÊæAÂ`¯¸Úq•p+Ÿ¡!72êçP(ĞO²>àS¤$Êğ"Çzò´^cÆ»1êx˜}¹JoİùKu|Îwoâ4â2(Ó¨ØfùFpè´^›4¸†Ï¿Œôi–PCÔ²e²cfãOå¢Ö
’î,'ª' ‚•NìG¶ÛÛ_®Pl•85d§î\ûÔ)¹QÜŞ	fèDa%EXy™*8w«3Ü<£Qº´é—çÀ%ì•Áj3 \Ê:µçÄ‹ÊXùJ ;wOûL…zk{F°%Ô9í¶»°[XÀ,Äeqk¯ø2¤®¾Îÿ¤1©o¡a)ûH·Ã?´Foñ|š–ìÿC¦šOea¨ânµX Z;ÿÒ¸“ìç‘ë¯±‰qÓ%R=£€EÂc/‹aEH•™ÒÑM°ûO{1ÙtÄĞo´óşz¦ëÿ$vÀ‡r ¶TÀ¬}«ú"Ì²3áíĞÂÓ@Sh¥N‡>k'. øìÏáIJèw­Òš.Pö®?úHÌ÷Hg·Z$ò¬}úæ}ñ ×;é=jİÛŒÓÜŸ×|3¦PÌgí8‚óUÚ†á[¤õÿyşuP¥+şü„-"Ú}WßÜ¥5" Ÿj×®pÃ~²_ª}¸—´@r7´èê×Ck· ‡r¨^)•7¤’¯ZC¹ˆ5Á•TÊ2_dêÁ•tŠ˜B£
×ë['VIï(äI¦>	xO¢¹x›-Üü£ÅJ½{x¤{©VÌƒÂ¢²H}Í¿ØÛmÊQ/`T‰&š•¬óÅ–Ç»Ÿ/æZÇ Ì|Öÿœt$?TZ ¦şÅîÅBWÛÿ	3Âõê…âîê„óIÊ"@¶d ?ùÉX³Áôä}Iƒµ©I>	}Ï´-é\ÿôŞènê/K}ìG;‚Õ†Ç-f¨$³šˆÂ¤¸\í¡a/"[ï½ÇTĞ2Çø•ø$“·¿i!#ü×åqè™D0;;>
NU ¢ÓçŸ)•`v•ÜMU@(O¼÷¨‹^Äfj»Y+/FD(Rsum ’¥S‡Nì¡yp"}yr…pÑ÷?€ÚÊ4•·¦9ªw–tô#t”YÑXhô6W™‰ä/Ú¿X¼¼‘¡…§ÇT›.‡ºıüßÑŠ¬bÅópZpa¬¿#Àe¿B°ôQ—PÍï¨Ğéz¤ØûW ş!Ó›µøÚG/(;{hÚfij…µmËx|Ôú‘†ı9BÎ ÅˆÄğ‚	¶Š<5MeßŞ<½vIµÑ$©Ã¡‡q=ßË•HıRf>kâñ}×Ö»ÇV+IÊØ½ƒğFeYÄ­68wQƒ‡´¡íıî%]ºë´W6–©œ£w	ĞjsT„Ğ¾ØèÆû3H›B‚UTlob@0íÜ›h‡(ÃÅ›CMLFA-òÀp&Ïˆ«ffI÷Ë*1ñÿ‚¼1#ŸûwŒ–îæ=#èâ7Uw)xåwvY¤'Ş”é‡Õ	5ıH–[ÛK“ÃÏÂØ“«œ1w…Ö9°uµ0&{¼Ñç#mÿêl£Vól7ÌáhÁ“îo`Àå6şO)3xŸ0F¿Ê¾Å|¯©Ú–#'áS„â D÷äC
Gò¥w0Ó\’¯ñ‚ñıÆhÎöH<Jja¹|ZÆaÀ¹ˆ`]öš—é8™æKä}³£zyå]0»;">±g:0Ó3œâ™Ñó]•â¿°}>¡\£u*$òF$gˆßd>Cá*ğî‰ƒ•yÉõS€²bPÏ5ì¹!1­d¢–è¤/ŞÈºó$˜ ,kjÛñp¥xö™+‰PŞiZè˜Y‹hşÌÈçt’kd ÁzM<±â¿Õ@…n*Ç‘D*Z¦ùJ„\Æpëõû{•zìÑ¢­R†å¦s´˜çÊ†»¥¼yáI)…N(ÃjjÖ”¿gŞÆT„²§øıs}ÃDÊ?˜]áü&QÊ”êãúµ´†c¸èõ2fcF	ï¦ŒHŸ+t5]ïrª€X.á·SWØ†MN¿Û¢–;b¿W€Íæ¸“—Û•éò¼z~eÛ
ËfOûhbûfˆÓ>}/Äoç¤q—2/â€Ò;»8Xß"®–*Df ñ<ÆÓ0‰ óñÉ­ÂZçÄŞªÒ' S|háLË½½¿I^j_j&’J|‚¸_f”Ïà?6¸ÿ»İhA´ È‚5n ^]æÎâ>¸zhE&Q>s5x\=#Iîâ}@<–ÇˆkO^ÀÀw"ğNĞ•rH¸ò â†²;ß|‰?¹xérhgŞh!
|½íåÑ@RÊêyëp?ht"jèõ“[|Ç¿#´óÑšÓŠc!hTe¥¬I·)mû9ûób¿ç=ö©é-ÆÌü-»v›»âz¼_%óôGåÏ’©èiÕãüÎc Œi¬95cKƒŸD¯™Bmİ‰ŞR‰»ÿTÜFâœØ+E¾}½M¯ûê“K[¿Gj	°Èr\•™q
áŸhµ¤.Zgd¢÷sl†8\ÚîŒô5âÅ9çR™™ÁI‚$æÅ ÓûÚş
<@ğÛ'ÊA<7<kcò¾KO6T¾ `ÅêdIl@2¥›çØ^ë ’b³W@•#¢sÖ¸ ¥Có®F™KİÃáp?fT­¢ÄçõzŸËş2ë|Îš'é¨ÂîËË[Â3²Ìó+“dWÅ*Õ#®²ò-1…Í ğäH|­ô4é%ÿ÷¯-vÓšÚ¦àI?®|@’òÜ•0Å-=t™Ø×LÔcjT×]ª~Šd#øó„àmËŠ9R–ædÜ¡lÇqb8Ÿ®‘ÌÉAäè@£©ÇÚ^!VC6¤Íÿ¯[`ª”gJ|ğ#ş›@»ËzPY»™˜ÌúKÓ $Øtb­æ(êİ£fı˜Š1Âµnx%Mıø¢‰hîVjj