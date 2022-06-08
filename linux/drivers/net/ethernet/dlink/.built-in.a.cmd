nput)->amux == CX25840_AUDIO6)
		cx23885_flatiron_mux(dev, 2);
	else {
		/* Not specifically defined, assume the default. */
		cx23885_flatiron_mux(dev, 1);