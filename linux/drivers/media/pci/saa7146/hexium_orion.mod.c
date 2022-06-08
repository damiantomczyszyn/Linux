 of reset */
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
		mc417_