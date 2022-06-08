egval = MC417_SPD_CTL(MC417_SPD_CTL_FAST)	|
		 MC417_GPIO_SEL(MC417_GPIO_SEL_GPIO3)	|
		 MC417_UART_GPIO_EN;
	cx_write(MC417_CTL, regval);

	/* Configure MC417_OEN to defaults. */
	regval = MC417_MIRDY;
	cx_write(MC417_OEN, regval);

	/* Configure MC417_RWD to defaults. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS;
	cx_write(MC417_RWD, regval);
}

static int mc417_wait_ready(struct cx23885_dev *dev)
{
	u32 mi_ready;
	unsigned long timeout = jiffies + msecs_to_jiffies(1);

	for (;;) {
		mi_ready = cx_read(MC417_RWD) & MC417_MIRDY;
		if (mi_ready != 0)
			return 0;
		if (time_after(jiffies, timeout))
			return -1;
		udelay(1);
	}
}

int mc417_register_write(struct cx23885_dev *dev,