nput)->amux == CX25840_AUDIO6)
		cx23885_flatiron_mux(dev, 2);
	else {
		/* Not specifically defined, assume the default. */
		cx23885_flatiron_mux(dev, 1);
	}

	return 0;
}

/* ------------------------------------------------------------------ */
static int cx23885_start_video_dma(struct cx23885_dev *dev,
			   struct cx23885_dmaqueue *q,
			   struct cx23885_buffer *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* Stop the dma/fifo before we tamper with it's risc programs */
	cx_clear(VID_A_DMA_CTL, 0x11);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH01],
				buf->bpl, buf->risc