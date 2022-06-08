TRL, reg);
	}

	/* Set VIDC pins to input */
	if (cx23885_boards[dev->board].portc == CX23885_MPEG_DVB) {
		reg = cx_read(PAD_CTRL);
		reg &= ~0x4; /* Clear TS2_SOP_OE */
		cx_write(PAD_CTRL, reg);
	}

	if (cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) {

		reg = cx_read(PAD_CTRL);
		reg = reg & ~0x1;    /* Clear TS1_OE */

		/* FIXME, bit 2 writing here is questionable */
		/* set TS1_SOP_OE and TS1_OE_HI */
		reg = reg | 0xa;
		cx_write(PAD_CTRL, reg);

		/* Sets MOE_CLK_DIS to disable MoE clock */
		/* sets MCLK_DLY_SEL/BCLK_DLY_SEL to 1 buffer delay each */
		cx_write(CLK_DELAY, cx_read(CLK_DELAY) | 0x80000011);

		/* ALT_GPIO_ALT_SET: GPIO[0]
		 * IR_ALT_TX_SEL: GPIO[1]
		 * GPIO1_ALT_SEL: VIP_656_DATA[0]
		 * GPIO0_ALT_SEL: VIP_656_CLK
		 */
		cx_write(ALT_PIN_OUT_SEL, 0x10100045);
	}

	switch (dev->bridge) {
	case CX23885_BRIDGE_885:
	case CX23885_BRIDGE_887:
	case CX23885_BRIDGE_888:
		/* enable irqs */
		dprintk(1, "%s() enabling TS int's and DMA\n", __func__);
		/* clear dma in progress */
		cx23885