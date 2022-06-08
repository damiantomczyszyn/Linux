ze, &risc->dma,
				       GFP_KERNEL);
	if (risc->cpu == NULL)
		return -ENOMEM;

	/* write risc instructions */
	rp