nnel *audio_ch =
		&dev->sram_channels[AUDIO_SRAM_CHANNEL];

	dprintk(1, "%s()\n", __func__);

	/* Make sure RISC/FIFO are off before changing