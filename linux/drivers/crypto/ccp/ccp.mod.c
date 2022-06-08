nnel *audio_ch =
		&dev->sram_channels[AUDIO_SRAM_CHANNEL];

	dprintk(1, "%s()\n", __func__);

	/* Make sure RISC/FIFO are off before changing FIFO/RISC settings */
	cx_clear(AUD_INT_DMA_CTL, 0x11);

	/* setup fifo + format - out channel */
	cx23885_sram_channel_setup(chip->dev, audio_ch, buf->bpl,
		buf->risc.dma);

	/* sets bpl size */
	cx_write(AUD_INT_A_LNGTH, buf->bpl);

	/* This is required to get good audio (1 seems to be ok) */
	cx_write(AUD_INT_A_MODE, 1);

	/* reset counter */
	cx_write(AUD_INT_A_GPCNT_CTL, GP_COUNT_CONTROL_RESET);
	atomic_set(&chip->count, 0);

	dprintk(1, "Start audio DMA, %d B/line, %d lines/FIFO, %d periods, %d byte buffer\n",
		buf->bpl, cx_read(audio_ch->cmds_start+12)>>1,
		chip->num_periods, buf->bpl * chip->num_periods);

	/* Enables corresponding bits at AUD_INT_STAT */
	cx_write(AUDIO_INT_INT_MSK, AUD_INT_OPC_ERR | AUD_INT_DN_SYNC |
				    AUD_INT_DN_RISCI1);

	/* Clean any pending interrupt bits already set */
	cx_write(AUDIO_INT_INT_STAT, ~0);

	/* enable audio irqs */
	cx_set(PCI_INT_MSK, chip->dev->pci_irqmask | PC