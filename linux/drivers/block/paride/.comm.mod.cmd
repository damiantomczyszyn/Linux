nable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = rxclk_rx_s_ca