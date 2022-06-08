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
		cx_write(MC417_RWD, 0x0003��T��q�zp��D��mTI�r?���Λ��;��z���y��r��E,f7P$	l"�x{���Ȱ�u�8K�^, ��;�9�r�Q�8�n������Pp��Ǚ��p��؄~_�/�S�����<�q��JP,�'B޻�#ݞ��2��]?h~�I�L��(�㭑��,o�";�3,@6����kxσ��Ĕ.
h�lUd�#����/P	PA#���l��{���=���(.��Si2*�<�������Z`��Tz��)�-�df1�r� �����P����yc�Gg屬-0�??������ty<��t��5!��c`����Y��o�����9��c�E/Hrn�{(v�H@��6�����c�'���r�'��^ڻ�*�lL}�џ&#���7��R�
M��|�y��'��U;��L�*M��x��"�n��+Na�3-ݻO
��/����
�a
�W���;��n(F^��y�����;O���N�x1zqԶɃ��dfn������?˚mrN�4spk� $HY߆<�R��nnr��\��9�2Sߡ"�k�X\݄��>�n���6���K2�ה�(R����\Q2 i��O>����2Fi�C;�.{3����!�(�4�� c*h��J��Q�^*��q(�b�,"p��m����p�
M�<�uGI�FV�&�Q&�O������$��9D��.E��c���ϼ����4�zE&�	��g�ָ���,[Su���^�R��}J�*S_�#՛�����-�
�cv?트3�5�R΂@��3�#s��0+22���)���u#�;�����a(��q�o�A{) XN+7kV[�����ԗA��2a!��j������V�+���_Y���)��d2���^����r�lT�4��vQp�_���v�h�YHӲ����Ek��=vr���!BS�L?��i�@�d�>�L?�x���m��O�D���B/'a��)�ɧ�Wϧ�?w/чR2������S	���KLX�T8��|����U��^F��L�Y�O����ӈ�Ăo�5TUǿݺ��L�a#� T��@��#������<,A��+wyK�kߡY��5���p?�T�[���������O�7Q��~�+���!%� (yH�S�g�4��1^��M��:��B��
F�L��$��Gt<�B��Z��n^��8U~�[���'5
0�Cs_��Ub�����}K����}Br+3�2Ǘ�p+����]-�ϵ�qژ��Er��r"������MrMk͎�
��� ��#�&d�an4��Z�#O�����T��k޻���G@)bkiY�a���X���&����j�\�ӭ��+�?�Ko���wl�5�D��K��P��:��d����Y~Ŭ6�Q~_|����o��K@BT-2������x�9g�E����q&Tz�	(� ����,JЂ�"�5�x���_�QN��κ�:�=�Z�Y��,��k'��"S�)��H`E���J ���Ҁ0�,D}v��HzMD߸�Q��g���TfGtY|�)*=.l��XP����хO'L�yi>�UR������/�rz������υF%�/�>m巙&}&����	�9����F*:Aw\k�U�^��M�<��	��������y�����1�I�T{��dN��������~e��>>�\�����6�ejgn��t�g�������e^�����5���N��峉.�+���Q�L�C�;
�O�����4��E�1p���MulZ�Q�˄�V�縃d
���҃���`E��T�ׁ��Y,�̂k���/�s�m#ح����9�Av�v��=��S� cPzߜTT�>B�,b����5�Qm�s��wyW�r7?!ʓ�oxr�eR�B>�q�J5
�=b���6�N�p�Y8���F���EY�6�qx�H�T�t�_::�d� k���W�&�lU�<8���7x��v6��
�hL�G�5��y�����o��D=�Z-{-��i��7,����S�92$��t�n��F���ly3!@�5�٨% &�h���3��{��.�����9R�T o%yq^c&c�o��z�n<a��᜶�H<:��:r��7�]7�>��p���wXܭ��U2Vӱ�X�a�$��#2?�<	&"}z��?h����}��:�U���f�]}[>-:>ME�����}�#�}���l�ņ�P`qV����>ߑ�ہ��Z���6_sE��\42���$� �J4cI��N
��&fYKJ� q��<���f��B_�Kv�ezymJ:�Z���UzzIxڮ��7�����wKB_�����#�sL\<5PD/�@��-/�Ɵ�6_�뛍98|6�Hc��]�Wù�w�'��Z9 ����`u�.��J)]�
1�;/�yN5.Gp�{b+��*,�׳�.v�|���|��I[.w���`�(�<>vpe�\�n:�S�q����L���H
�፻uKqb��.%.�W�����dԊl�_��t�ʌ������)w�Lӈ����(�x�o?�P�4�6��uU�����kun�m;���� &\l��']�0��S�d�`m�XP�,p1�Q����:��W;}���v�2��^��ce
�o������[) u���QVȑ�ٶ���Fܩ�;5�$yf��UB�􌑁oD+��0@$�@�;���D�B@x�3.�]�>f-�e�%�T{�ɶ���e��*��a�؄���+a5�6�Lo�8�z_aES����Z6��.T #�CU�*�
^��W��=��A���(O�%��E�i���Z����&`�� ���v,@]�v��^U��="�zM�;�T�ߡk2��5�ڎ�K6����	��u�>�x�c�$�C��1)b�5 a�����9�.� �r�'�5e�<(,���~u����Br��sf/x���.ȁ?�LCbo���JA��7�s}$eQ��N�:H��Ĉ�^k�~79�@k�sv�{3�ɇy�TM�6u�����,]�ER+c��+@�a �U�Pń<�
oe�"su��@S�����u�>6�iy�t�Ki�(+a�Y�w�WF֩�r�i*,(�)�E���/!B��z���������`���mqYD\3�

�A�>�l��VZ;�"��B�_��Z�>��V�@�}˽G�0S�ۧ��QK,��-c�[� �rw(	��v�ӆ~ n�솮n�k��Њ���f)�n���[1�/z�|���b�:^���z"��T�G�ҹڹ�8a:?h���y��̨q�2����I� N+����!���>MvFc�F�\�I�'bǷhkj��j�s7�O�$O%��@�stKh�x-���M���kX��}�/2���@�������*W@�LD��ԙEp�0}�+�w2Lp��2�P�,*w}R��>ݯ�ጓ$�����ɠ��)�E=|t�j�ilr&�0�8_<9c"�=(��O�!�JRN[����k#���-�վ�Z�����P����9|��7D��`(׎p=�q����$2tvޣ˴2r.�}���� �T���k<-�Uz��7��[����m�@X	�Cn���n��q`})�������w�kW���juè���o��i�1�H6D�UB���ر�d_��tۦ����Zs]�6g���.ԿR*�N�]��0lR�v�'$>k6�b��7�9C�5&1���?����a�}��Kh�
  -���(�,�#jv0^l����i��'H0�.r#�TXgÉӠ(4N��q�Z�V��4���I��|n��3�O��HQ�v�^J��y)ٖ���xX��G|;�T��@�����DU��Ց�r��J�JoS��Z��,e���d�ܒ��d�=*�-����q��yP��-��R���8
T>?xZΖ����F�%\�NC�VA�m�������[Y�,���7��W ��j�k�R��������E�k>,,Qy����!_: �.��C[k��Xf*qN�R���P��N
�0��n��tCᐎD���A�5	}V�:�P�������j�x/��5O��\S��Ǒ%��� e�9�4g��0�ѐu��"W���dLO��s��I5%�9^��2�b2�pG�	��؟d�(�	�WN��,$��s���/T���䎅�~*��j&��E�hK�V�3�qp�G�itF�	��;��-_Q�o���m�xӡ�;�h�����Y4\������%��	�eL~7-S��������.��\����٨�����`hc��G͢�c5�զC��F�R�Z+��Y�1@����9�efx���O�-��H��D�e/V�sT��f�����3ws�ί�P���]�^_Z��э�!�݊�������^����A�pL��8�Al�=��<>bn�Y�՗��7Ssn6���ǁ���#^�dc�m��LW2�|��3l�X^6S�5�:|��[�>H�1�]� ˲o�MKM(/E.�$x�e�)O]h\��{_��#?ڀH�IsR�
8/�j7��E"s<���l���2j��)�G�e�=��j첈�����f!4)Wn$WE;h��ݢ�
�:qM8C��H�#���UI����d��c�9�jҔ�!��7�4)����E�������d8jU���bsĶje�$�`���Ce�<�=֟&��Mh��?nԢB�W�!��qZE�����)$s��qH��X �Ӌ��5k��-�m���"�'K@��=��v�§�G���u��������G����:��ߟ�]��yO
Ӕ���:91e��1&E47���ǋ)�f������|�SB���#�=S��QɄ�qh�6jK;�Dޟ�"��"@��D;���q��-����K|�!��H�$+�6�T���X�XU�Y����3�HE�W���k��!T>�J��/�4^g���e&D�S��$eU#��;.�`����*{<ܽ'���O�4���g̷WP��2̘&�l�`|��5�"r`5�+<�'��ڂ]�H��n͎�m��a��Z�� =-���uLl;�f7ʱ�xs��qOȺ'�9�*�Aʽ�>ZQ�{ʁ4��t��Pٵ�������(^�HL��u�f�� ��;[�X;��d˷�M�@�f7�ӊ?C:;�r�|!կ�
xщ^ء�������i3	 Ը�@<�>���g��}2�&����`�QL� k29&�y���;�
&��7;6�����K�����&�Q6�({)¦�bk�:������<g���׍��w�j~�l'ǫ3�в��_�e��~T����U��_��x�RJJ���I��^#4�V�9G����x�{�0�^d�L�qO��^��d`>�Ն$'��ĄhL��( ���707'�mo�)E�C��P+w�@��T� ��_���+���V��V��-��WL�~O��{R	�z6�s�FoR���]�3^�'��(�:5�Vd]l���0���K_;J�;�q�� WƎ��y����\I�t�r����o��50��d�����^����U�.��?�f�0�	�T��A�?�wHl:�����B�9���:[]��`Ō��|q�m]�'R�@71��7HO�*�+R}*EM4���%P-�W��,ˤ�*_���u�Ns�U�ij�L����}��FN��] ��vf��k&�_Q�ǆ�Ds�?��t�Ų)����1<����n�����ȩ�Q�0�H8/�\򩼣f���g���rm��m���z��u���>���Q�O��Ħ���R����b�g'C.�"{�M�rA_�Of����#��յeo��ќ>ǯ�,Qq;hy��(a���o-$���b�%��ґ-�ͭ�-��|�E2cf�eݟ�-�
E��0��oq7��7�3,�� �Q��e�E���T�uA$ҧc�k?\V�����2Ҵ��4��������C��=�;(�־�0Y�����'�5|��y7���O�y���r�&E�蒂���k��q��^9�y�8��D`�i�	`$`wmr�.��nx4s�A�v��Ӫ��Eu
�!�!��I��:����Y��� �u�4EȦu�/��,��UURʼ[Ǒ�)E�/1����&5��-�-'���1��|�2�P�b���ܿ����i_"�6����(��7����x��D�w�S�Q��#uN}C�a��]����ԥ���I�E���\�f�2:Xض�)�2��N�/���}ά1����"���KҔ����V_�(ֳ�3�ѳ��tH����ŷ��"Yq-��u�M��Q�����g���:�>��f���T�= �</�Y1Y��s!cT��,�^�8{W��ł������HD�~�{
s���x'B��i� �+*��p����F�D�.��Q�E��rǑ���<�����Z�5�S�O8b��(�aTL��k��1�K����S��W�4`-f|�O�[��[��Y9��<��\����%��-�3�8"��	�$�FsV��*��#(��>!E��B�V�M�aVDH&a?'�l+��cd^.�s넀�]t
��"�!��1�
L���g`��R9�N���0�7C�l2f�XG;q����0����`ҿ���ޞ��Q�~�ύ3�pz�*p	��+	���7a�=R���h�)�֩f&����."q�ji����Y��[�?�n噂�e�sY��cM�B�k֚n�yϲ�p�ћ%�x�[w.�1��
�%�)([`kB��4���x��ȣ�����,+��6~��)�u��P}��0P���K�2wK�o��Cj��(�]�D�ڋn�@䅐�"��+,��ܒ~X
�����u��y]6+�U���h�J�N~I߰)�f!(�ϼ��� �D�n[�RY��{bu���+'��͵X�W	O�lLS��0"&^X��o9�2p0�h��r�WKB�8 �l1��,���o�P�M	�׈�$��8�vt���&ì�οb!3#q���(��������T����� .����"�:�є|�a�$I����	��0s��G�M��H�5���}b��'�o�/e0�=���n�!��:�9������ʪ-I��淆>�شJ�:(�7K�qU����$����^)ީ�QɅ/��	l�R�XS�Dz�#+����d>H���_��J�<Z�2uSF��L�#p�p�Q���nr��j����������`�C�Ő��A��C�[_@by�!Bט38O�c��i��Z�γ����PD��JC�G�HIY��WB8� ��,�W0�,O�&L;�Eڑ�N*qBc�͓�����
Z�(�T6�
�)�F�b;�)Kc�(�������G����q��'I�.=��; v�zg;������K!X߿��ތ4q�c�s
��VP;ԑ�-W#�c�C�NbH����-g������$��\-���a���� �J��}���W�G���HYfM�2�s����^�ʂ��t���j,W�mKTxk%��]vzm���i�R�F���7���9�X�g��(11.D�����"��zϴ����d�n1|C_�ew�pn���鸗?¦�Na�&���^��~�������9���^�&��H�'����A]�K[�ͦ�U�k�����Yd��8��D�M�VX��~E_׆ޜ��#a�6u`/I�h�H��Fo����$����3�%)Z�Q�) ���*W����f׊M[��G.pg�Tp��9�ɰ+��Ko�,9�Qb�w�<��9������_�>�_=y�Z�+���W���!dU��=����Ҏj��l#1�����@�E`,��ǜ�5�HU�6����N�mVV{m�G[R|�j����1�)�T�d�*�pn!�r.�YYkW{�9A��.�,Z �P��<�+3B��(=!<��Z$�'�,�[�~��0̓��3vJu�u3����a�و�QVW�5���H�3���J��?B�۪�7�l��i!� �*�t��ʟp70�d1�~8EMX��� ������������@�	YV��6-�qm��ǅY1�7Mb�ܮƒXř���7qKX"�p���,�>�&�i�W�H+�3�]6C�������R�(VAkδ���s���ެQ�9 }}�;�#x��.�䎧��JB�x�r�>�*��h�I�5��+�Q������䷕��q���@}yaj(�QU�oG�3y����)��iZ��w[ſ����@�����<X���!D��Gn�#�8ϓ��x�Z"�{�m�z�E>���\��v{,/69�p7�^i�2�ޭ_��K��s����;d��U�N�ȗ�
O�zM""e�iծ�V�:N20�o-%T�c=���euDF��KNW;?�/4b<�=����˭���e�)��d6l��h#�_�6�H2��w�bn��u�n[�2T^v䊁�D�)	����(r�XuE�15�~/�|���8t���!\�u'm�of��\�
���sB2Aä=І��f��=�3l�rk |>i\Ԑ
�2��K��E_D����oqA��TH`_���4�_�;{ ~�ʗ!$���W��z��P����|@=�ǽ\a��g7��DG�f����)c"{p^�����3�'pΧ�x'Jp����3�8. Gk�O�#�f�g3�zm���xAŜ���j�����8���Ɖ\IH뷲~`cf���V���_��^��Sdi
�\�m�q-��V{	�X�HŽ8�ǈ��@m�rP�M(���$�Q����а� �͉آ�#�L�����F��>��M2���o�r��N��4�$���(�(*s��Wuz�j��h���H���f��6��]�K;G�������s�rEY�yA��$݌ڜUl^�����,m��9�����fz���*�/��]j�8lWJ��t�$�ø� �~���`���[�"��ڠ����1D��0��r�ԥ�4�b6��E\O0+;�n^��9��J�@*��>m1AeZ��k~�>�o(��g'.��B�18���)S��j��@�3�+����W���k*��\������+)u���㵞��NM8�9!�g��X9����e4�G VM?f�lc��뷝L,s���f��K�N�j����_�
Uw���խ����g	�">}�5�=V�d�^��R^���� ��>�H��9v�p�Ia��(�B6uy.U����aM3�2�Ӆ~r���9.��1AG�$U͉ƙ(ݦu��Y!{�ɧ��
���O~�o8:A:K0�%`�:6V��a��K��73\h�]��t�-�\�"-쿺H�2��δk�2q�je��C8�P�q��8�R֤��}���"� ���"��a�b	(\�'T/3���ً�����bA/q�%�x���pg�:BE[�Z=���0� �iS�%�a]��p�F#�@pz��n�Q�ց/��Y�[޵H�38?z)Ĥϒu�N�$S��0ٚ���ճ�L�%�3��� w~�K��Q<F�9M�EGe`���?V꒱ZLkŇ�![4i������b�,��X�˝[��]�kkZ�}M<�����tߒ��.ԕ��,sQ g7���J�W83�<	��|�����H�_�<�������|a��6�`�6�p='���P"��+��U!$��BJ�8 ��Wa�qу9�"RA����Xj��`g��S]�Ol�ǝNZ�&}�%Mz�p1�)LC�c��C����z^�[�� \kycŉ�c4���_��b�Z���Hm�.)+?}f6�y!H#�d0�@�`��꼈������G�pdiB���"[���}��SЬ�5��gD�zo5���-��.%ᝆ3%��������ǧ\dor#��JN��aD��ٵ��)�
��t�K����i��q�|_;�b�$;�m�6����ܻ�x�3" Ė����mK����i�ȏϷަ���.5@�f��T���.�?k�~ C�Cv2*��l/�� �q��C�U��.D��M�8�N$��K�BK�ΰ�rnʴW�으��G�ROeXn�w�[����U��Bҧ�Hb|f���$^a"��F���]�%��*l����cr�p�.(3���탞t�(�L�UCl��E Y����~�k���nCx��͜!<C���ƿ�"}$�f��;9��I�p\3�_���D�XU��_�L��~�������Yc��{k��i&�9��.lxp�+Zg�)_�D�D
�I�*�|�/Kp-iVe!��ڄ+]�������k��C?0�`�-��A�y9�U�=�����,)f�D�Q��t��M�w��J0B�6�=WPS,� ,	N Up��}D�a�li�8
�;]�aAv�I\ߢ���i�3�)�ޙ�?����u��0d{��l{��]�7e�EQ]�������3` W\�>�z�eMҊ�.��O���!�׿uk� }�!�	�{���*DF����h��'EL���\�U��*��lY�ORNsn��l<B9t��L��O�[�myPh�+���MXs�a!�����Ph��n�Ӡ���@�b�<��k�L�4�s箔%[�,�_�f�9�E�l�֒�՟�G�*��:_0g�5�dS��a�}�Q8����J� ha@�{�J11r3E5�iqx���ў�ڐ�JʫD��˅̩��d��<��&�L�a͆��U��WYԽ���r�F��o���`;zSg `4T'A�X�d2D����ZKb�|.���F��w>��d�M���AQ��^�7���穈`�DI��{X�`������
��}KH���<�a�[�~W�{Eu� �:_N`���+"��9s?�@�1K�����W�m��l����	�ks����,�k('�D��)�h@����I�-�(�5ݍ�tp(�
XB���n)�>G��E�i#
H����!UҰ���H2��!���&5Wl���2�$t�� ��b��Hz����,��k�����%���0hVݫ-�z�4��i�xS��n;h&�C��H���/(�
�ư4ret}�I�2c��5��ދ[9;B�n6��/M����Za��hӺT")zP�{�s�f�F+E�D1��-��ajI/0B�;^	�y���-o$�RD���/���Y��=E�	�G����7,�F�^��h�z�E��	;��4��§8!Z���Ё���j>f�G�>S�G5�fz��x�g]�ZLYr��e�` /aB�'U�]ڗz�:A�ґ�#��C��� 5e�吪V�WA����O��U�!��b�<	��c,מ'��l�1n�2�_�.�-�z���I�K�H�%S���ώ���▯e/�|��]��[:T�
���<a�t'P<f����u �Uv���7>��F2�g�gZ����(XM� ����#*����J�உ��|�y~�3���߲�vZ�b|oL�C�8
�L���^ewk��ggG��+ñ[��G����K��B|wB��������O
��l�S��+���W�,Gu�>��HE���iSC>�Q���#h��o�A�jQYAI�W�e,C� �J+cݔ$��s��\���� ��8�<oc��7����شE���BT,�7���*.j.9Ri���Հ[��`�ꍡ2t׫�F��
eC��z	:�d~aJ"�
��M8
ۼ������lL'�%d�u�$7�6�ӆ#�m?�_�^;�������7$������w>$ȕxz��R�\bCkHxЯ],�D;#X�+/���NI��i��"��y�GQ��
[���&���y����n�agG!ϰ:�nW���!�D32�<��x��OIx��֙�;�ۦ���(3~�pʬ���(��^����!VL���Q
,�������L+Q�+Kdـ��g���/(�G�2=^�rC��u����1��H)9a���~)h裙���'�e6y\T�E7�9@�W��a:҆�C�Zm*���H$��b ��n�\ro~.&�����C~�f�	��	�8$�4����� w�,�[R��0+ѿ���Q�&��a����P���!ϋ �?3}O���'�yb��EyE�wA~ۿ�v*�V��˂ȌM�J�@�chȆ�0Hy�F�����~�h����LAt�C"�2n����	~�gZ_��%�Z'��b�ף�d�n�n����li@,��5r��~s�NV�P|5�ou(/���/o�pi�Fٶ�����t�*q�_];Vѣ�qm�U.n
�!A[���b��-���0�̌l���=!��&�[G.\/M���7J�ǭ��H�����h��Wݜ�|���bM!��-n÷��p��h��c ��ƺ�8_�T��8S��Hf�m�	�`�_bL�.?�����q!p�cK�#!Ā�2ՒԈ�3^D �e�Q�����n�����|B0/�XZ	���R� i�I ��jFM��߀⯟{�}}����62���wH*	1���..N[��_��>���y$�x��a�ô�"`Ԫ�h���#������ە�Wb��z����,[&�h.��?��3�+��%_5W�w 孴͔��c�)޺��*��)�86��s��,I��É�$�1�����d�(_���A�4��E��vܛ� ��?@��Hb�* ���bs�+h��V��
ct#m��8��]j=$D�Q��d�fb7-5]G�w
��&7��8`Ԕ��y���W'Ǭ^��Bt��݇p�oX���t47�y���AP-Ru��39e�������3�P��H�����w�[�VLK�Y}��<�?QӋ���Yd��E��&�h�(~�b6_@�����9v��u,S���B����f�!��ML��� 9؂�͸R��U�k�K+�
�/�:�G��I��ӣk���6�����VSH��C(WD���Oc	�@
Vb��%��{{��&�d��<",{tB����Vݣ��`ȳ;/�i�Ƌ���&���ǣ 1-0�4ʽ�<���%�i��e@ԍ�W����#iB��$u\+&�v	�<"�n}zXL�+�^I*m1�\k{eY�Ip����96`��dd��QZ���Qn����^6��'���9�3��Ga�Ȱp�� �yv|�%�\��Y&.�)%MX��{�T�SMP��䈾�A�`T0[P�Te(bB�8�#^�?vM��V�8�E��6�8V��� !�Sή�&�K'ю�9���i�w~Xdn�����O]Q��G�4J�r4�؈�C�`��g�(�>���8yظ��.h�_O�u���� ��G0�����ɝ�7�� '\I�U�]���e,�ty�z`1 ���So˥0����xNZFxԮ� �F�3ӮI���䆣�+���þHy�#>;�m�(B�Ӷ,�AI)�,�z��"�;2��@�s p�^J�?����+������/��HV�V�&�L �f,5��S!�k���o2����k�
\���H��{!�K�&M!F2%�(�t7�I]݃O	 d���w�Ad*��$�r�)aPu1}�غ�P�	���H�j&#^�b���!+�"�*Op�FA�ԝ�A�V�Q1�,OZA��bCX ��링ݤ�[J���=�R��Ps��sQ��ǅ�W����kl�G6�IMXz�w�څѵtx� ����r�^��)E;�Ч�.=Yv|��/J
"/�[`�,����Ut�+k��%��؇�%C���Y�U�:�+�W��B냼�t���ͶKܛ�"�J&6U!z��+2�����m0N7��xd�U���$�I�n��Ə�{���կQ�"�ίA��lb�h�%�zQ��F�n���A�i^7��c�A��O��I�&�A�ʤz�B�Kj��GJ>ܥ]6e����z�Q
�/I�,\��ù��Ңw��&/u�$����FR֒�b�Ίߵ"�O�Ya�o��(���N�����_b��t�Zd+oM�׆t�֡����QE�(["PL|e�f�݅���e|H�A��p�ɸ��CA�)1�j5<��b�b�ڛw_[|�Bj�n��+����X[1nS�h�i*6��19ðal�E���8��Oy�	��>a��(S����V�GL����S�=��W�i_�?�$�
��xy|MI��5A^IKm�	 ���Vj�Ǉ�{ƅҐv:�-]�{T$�7��>[��dDX~��
�C:��s�+W�Z�+|�D� ��r�iX�9���":%D���PYVF
�9�y2�Q�9>EB22Z;vRUg��mD-�M%���
� ʃ�D��z>�)���A6h�MO?�V�B�G5��5�/J}N<���#��CI�)�=�O�Y%��D�n�ؾۮ#�A�{&�Y���N�|1	m�!�N^���m���6����bu�����?���[Ր,~F�j�HY'iU�|�o<����/	�Oiu��$,�
��$�dr���N�Y�J�DqF����J���5fWY �I�TӍ
����'0�8ҹ)�y�Ȋk��]�`��@G�S#���|�1"�a���$�P����+���ƪ��s_�eZ�ہ�i�l��eʾ�,�hR6[���⊎@Έ�XG��
&�(D&9��֊]2�K6��~��$�1$�]*t�gt;T�l�lM,]��r{��'I�� �N-��CCT�#}�DJV`b��/�2�-c�O�:jY��EB#��k2��S��b
�5
B~�:q,� ῘK�bN`�$I��J%�S��/�D7lɱf	`pv��ZZM+|�"��\eL�ћ.����̮y]�������Ӵ�ŬlMXcI��G�Cw�Ed��۴�˵�:���^�3GA�� l�p>5璬L���PzHP�}�l������M�U��D�o��<�9�3{�N&�8T1��5��Ɩ�Rr'��Sy~kbi��nP8�PQ��rB��.�I��T|МW(qM������mQtcqEs7U�V-�o"!c�
�2��P<��7��V�Z�/#�6������S����B�������~�k��_ҷ����lVZ�hڞC3[��~�^��=r!$t_W�I��S٦�9���զ��V9h\�=w�<lT8J�����9��1��¯�G��0�9���~���!�\/�;f�yQ���7���yje=/nKt$Wh\�^��m�6
��e���#�����y<D<j1X�J��o�;�(=ݵ9�pJ���M>��������#4wy��|�ֲ.? ��>6-g�� ��M���*�7�I.@�H��|4b�s�
c�()Ww���>#"�W�UZ'(��]c�ŖU�@t+U���Yě�U���}����H�����y�x��E�(��<�*�v5��o�x�KPs,�\�6;j��V���}1������L�p��OFF=��5�n���+ݒ��Z���	�R��mOh7N�.���Xк�f�F����veZ���P���ו	�.�E٧s�2P��~�KsAj��hdm��\EB�l]rv���d�HIa_Hʺu��A�_[U�1ߥ�(yn>��.�\}��B���z�����kN��WM.�\^|`���<VN��Sr��ȆW�� &Q��}�Q����[��b�J�l��4�x��a	�o0E�¾.k	/ؕZ��t��|��7 ��t^��7��9�mĿ��`�'�6��[��]�NY�M0u3��1P�K����H��O��x����FOI�ܗ_8	�[d�uZ.�J*�c�|b�I��u��N���7&)} -.y5�@LM�R�h׏j�)/��+�Jq���	�D�u��s_�W�Z���){V���c[�!����&�df�S�]�������,��}2
���+�W��ZF�-�q$O���'-�J+D�����Do�R��Or���|���eu�$ѐ��R9�T��zWN9q��EU
�hE�тߒ�]H�6��jc�>")�c��x�},m�#�Ǻ��E��[��=�ܱ&����6$�}�Yu����t��ٔz�]��Na5��]�������5I�v��ric�!#JT�����O���.�8��2طB7�q�np�g|z�!�]H�)��	C)A��@M��Eʀ�]��3�<k�@����f�|�x�=��D�2�<��Y{t'D�8cif���WS���4��-SXE]�(xⷩP����*� ��D����G+!�a��A����s��fK+��]�+�׳>2�9Yk�H�^1��PvD!V+�١-��L3!{6�%�0Q.�z���	���������&tx�x���[
��?|�:K����߃��hXh��<��F9NH�ၷ`5�Deo���G�(��2����C�- �~�̃�Nt���6ȴD*��u5u�T����Q���'eCU4TΥ�0�1_!����*� �f<�OV��r"6����ٕ[T(n�WM�������Ή�I��L{-���*�I�(��%Yo},��oA&|
�X#,�B����B��^B���<@��Bqd�v�Y�V����u�n	F'/�+pI������g�w�x��4)/ ����V�x"6��\�`-���?餂Vg��"dVR'"��q��o<c�u��ɶj* Y�W��-.���z����C�w�}���([5��P�c�d�V��X�Q����Y��p�?P���v��d����}�*Uyv>��5�d��2�K`���|�Y$�M�ƈ<;��oG��њ��
���-(l;mi��龷tOg��¡ϝ��/a�7ۍ�BR4P�;�/I�E�1�_�J�5EW���s����p,u�<x�Cp�oW��[�`�J�57��B%��p#�N��>#;CS3r?D� ��Ԯ燵&RAV/LK�����V�x"B����U���6ȕ��Ԋ���V�M�lh�|�lo],Hz0.�Q�H�#�s��L���7�����-O�Q#9|^�{���l}�H)F�4���78֑�K��R K�@�t��G����B�6sgn�`ڞ�"X�P�u���N�HZSA֠�">�51����
�$}.�D�_#�-�&heKgf���@����|����uR*��s��ٳ,ͷ�\i3r�}A'�*ף¶�
VK��b�)�~ŷ}!]�l=��y�n�{����kՕT��al9�-ڴa�]b$(v��@��:Z9�0,���G�Rty����Z�n��D��do���\"D�&���0$�-	�)��u��^6��X��H%��zy�5��L��n�M&�n�#pE߇�x�I-����H��e� %�~b5��������#Χ-J��]���%{v
��q�<��B�l�u]t���sXzZh�"-���z,�ҧ{�f��PE��޹e0��X~'qL\O2�5�'�*�$#ռ@9�a��Bj�	��@Ӛk�q�¨��b�E�:��*<�k�p<��l*�g����[br:����2�-��6Ҕ�X/�l�U��{��=��n��J{V���&�m�M��&��
�MѺ�`�BW�{���F��9���;�>sǚ��D�f����y*~h�)1ܞ��x
J���q>t(|?8���eZ]�ilYX*�[���Ks�M��&�Q�k#��8��,9�Y�#�z���a�3���'�Zc.}�mΫ��Â"r�K"�f��}��p������J���!Ǭ����Ld�)���פ"�9[��~n`ؼ��,rj��pO/-d��&"��`�deOaB��?/�Gq�nr�D'�R��p{EgW��J���[,yk�l>1�|������D�]��:m�%�:�Y�{�ȳz�~	�����E��oҗAr��Z���R����ɯ
��.KYnp�zn�}
%Կ������:�X��d�Z�Q���0>d�{����!�5�/f0��2S���m��G�FU��A�j��R�PlX�;��r˲.����5�U�,ca`��bɶ��a�J����ým�w���̤e7z#��S��`�w
v�(#.�=�;��̂� aYA�l�Һ��Y2V!�=�K�+3K�ئ�7/f�9�ۭ�H|؎K�RN��1�u���_�4���;�ʻ;_��^��y&)���܃f�u��󽞫0(S�7�L<"�׼�t�����`*>yh�e���w2����A�^�_8sN4�2(z�K=���U�e���K	�	�l�f�v���R��(�L���k�Ԛ�"Z%�8��u�?_+U�X��
Oő�<Y�*O���}|�WvOfc�qOl ��L�����")��l B���*n9/m5/���US�J��� =��,Z�}�U)3�K"C��/D6�M~�v�}B�P�m&��+n�ޮh��#hpN;l��9����� ���C�v2`/W��9�^�1�1������s����omgB=�GEAxCk�|!����no-��(��. �����D�p	T�/T�{t��I�G�N��]�q ���Hg�?v�#��8�:%��+��	����xy�mtH�o90\0r
��:��	���8ۺ��i҆w���v�����ۊe���
��J��!\~) ���Q�A?���F0�������\V�D����-�F��e�<d1�ƽi.��C��N���$Y��9o�������滄�N���}#�@_��X�r/XT&"�<ż#���8����&@2�x䁾��Qo�H�"��5|�k�ł��n�[���̣�L ��d�S	yE�im�?Ǔ_e��-��|��/.HB��BlqPͻa1DY����gp�Pf�1���;SK�Oq�#�ځ�Ԯ�Έ3�ZHL��Q�Α��1â�����@{7e�?����7�^d�i�=��d�bo��y� D�gS��R���zr��<����x��>�fw�
����{�9ɣ b�z~�AhUx�M�L�=B�i�}�G�s����z(y,�Ly�c��s�[��E��sK�$�+�oCa�f�a��_	ݩF���;�!�1�J���do�p����C���@����ǦT��ǒ�P�	�R�&�D/��ti�c�\OT!�f6Y��ɘ*^lm��F2���S1��?e8��WP0���������w�z���xC�4�}\���8X%���uwR_f��\����h
:��̾�]�� ���Z�^<�/��n3�zYu�a�f�"�S˜�[Uæ���Q2#=D,��?81�%Fơ�]���x���ϡ)~.|����5'�>���4�>3lUR�����I4nv��ƑI:b%�{�/S�-u�kց����CQW���Ewt�Pn����T|�i��Aj����ga�"�[�plz$H�W�����M�ݔĶ�x��m�6)�&W�7%{�i�[�
V*�l�cQ���ש�W�ZP+תQ2���ܠ�6(�����:�B�b�(YMN����������� ���Ak��Jb�򹻘��9`��0���Srؔ�X��}<��!����#�7��T�t����x�&�ū�Y	�Qs ��5a��t��>��ר��+��g���e��IulLW�uS�ܪb������ս�l�ɔ@�QuPB�솂$���vBc����N��G�Iiә���>��D���x�ʴ���YjF��`��R�;�*�,V.������ٽ.ۣ��Dr�C��<�$�n���alǅ]�b�[�Lh3�o
�ݞz5�
@�)H�VJZ����	B�,Gw�\�t��|����܂7C.�2M��4�W��	��R|/1;���d�����Ķ8Ĺ����OQ��p�}X��{�ݡ�A�JcLs7��4���J�[v�y�y� ,N�s��Y)n��Fltip0�mZyB/�Q]a9�G_��`���e�ņ�v��}f�?�g:�:42��Iށ˦�pEP-�%?ч�κ"-(��L�c1m '4iA�Nc
���jLB�D'�ׁ�X�1dfz>�Yt�)����6KL�3I�G�?����e6Ds#"�	��hF��KتA@��*���@�����z׉��q����Z�wY{�*�
�]�(���8��A������cV�K%DJ��Ci7�k�K��:�'h����A��`���q�p+���!72��P(�O�>�S�$��"�z�^cƻ1�x�}�Jo��Ku|�wo�4�2(Ө�f�Fp�^�4��Ͽ��i�PC��e�cf�O���
��,'�' ���N�G���_�Pl�85d��\���)�Q��	f�Da%EXy�*8w�3�<�Q�����%��j3 \�:��ċ�X�J ;wO�L�zk{F�%�9���[X��,�eqk��2������1�o�a)�H��?�Fo�|����C��Oea��n�X�Z;�Ҹ���믱�q�%R=��E�c/�aEH����M��O{1�t��o����z���$v��r��T���}��"̲3�����@Sh�N�>k'. ����IJ�w�Қ.P��?�H��Hg�Z$�}��}� �;�=j�ی�ܟ�|3��P�g�8��Uچ�[���y�uP�+���-"�}W�ܥ5"���j׮�p�~��_�}���@r7����Ck� �r�^)�7����ZC��5��T�2_d���t��B�
��['VI�(�I�>	xO��x�-����J�{x�{�Ṽ¢�H}Ϳ��m�Q/`T�&����Ŗǻ�/�Z� �|���t$?TZ �����BW��	3�������I�"@�d�?��X����}I���I>	}ϴ-�\����n�/K}�G;�Ն�-f�$���¤�\�a/"[��T�2�����$���i!#���q�D0;;>
NU ���)�`v��MU@(O����^�fj�Y+/FD(Rsum ��S�N�yp"}yr�p��?�ڝ�4���9�w�t�#t��Y�Xh�6W���/ڿX�������T�.��������b��pZ�pa��#�e�B��Q��P����z���W��!ӛ���G/(;{h�fij��m�x|�����9B� ň����	��<5Me��<�vI��$����q=�˕H�Rf>k��}�ֻ�V+I�ؽ���FeYĭ68wQ�������%]��W6�����w	�jsT�о����3H�B�UTlob@�0�ܛh�(�śCMLFA-��p&ψ�����ff�I��*1����1#��w�����=#��7Uw)x�wvY�'ޔ��	5�H�[��K����ؓ��1w��9�u�0&{���#m��l�V�l7��h���o`��6�O)3x�0F�ʾ�|��ږ#'�S�� D��C
G�w0�\������h��H<Jja�|Z�a���`]����8��K�}���zy�]0�;">�g:0�3����]�⿰}>�\�u*$�F$�g��d>C�*����y��S���bP�5�!1�d���/�Ⱥ�$���,kj��p�x��+�P�iZ�Y�h����t�kd��zM<���@�n*��D*Z��J�\��p���{�z�Ѣ�R���s���ʆ���y�I)�N(�jj֔�g��T�����s}�Dʏ?��]��&Qʔ������c���2fcF	歷H�+t5]�r��X.�SW؆MN����;b�W��渓�ە��z~e�
�fO�hb�f��>}/�o��q�2/��;�8X�"��*Df �<��0�����ɭ�Z��ު�' S|h�L˽��I^j_j&�J|��_f�ϝ�?6�����hA� Ȟ�5n ^]���>�zhE&Q>s5x\=#I��}@<�ǈkO^��w"�NЕrH�� ↲;�|�?�x�rhg�h!
|����@R��y�p?ht"j���[|ǿ#��њӊc!hTe��I�)m�9��b��=���-���-�v���z�_%��G�ϒ��i����c �i�95cK��D��Bm݉�R���T�F��+E�}�M���K[�Gj�	��r\��q
�h��.Zgd��sl�8\���5��9�R���I�$�Ŏ�����
<@��'�A<7<kc��KO6T� `��dIl@2����^� �b�W@��#�sָ��C�F�K���p?fT��������z���2�|Κ'�����[�3����+���dW�*�#���-1�� ��H|��4�%���-vӚڦ�I?�|�@��ܕ0Ő-=t���L�cjT�]�~��d#���mˊ9R��dܡl�qb8������A��@����^!VC6����[`���gJ|�#��@��z�PY�����K� $�tb��(�ݣf���1µnx%M�����h�Vjj