-T/C/S2, IR) */
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
		pr_info("%s: Board has no