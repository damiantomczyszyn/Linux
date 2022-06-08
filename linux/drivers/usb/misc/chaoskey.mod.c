00000036);
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
		msle