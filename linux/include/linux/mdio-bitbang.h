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