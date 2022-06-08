th =
			    txclk_tx_s_max_pulse_width(dev, p->max_pulse_width,
						       &txclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->txclk_divider, txclk_divider);

	p->resolution = clock_divider_to_resolution(txclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_tx_irq_watermark(dev, TX_FIFO_HALF_EMPTY);

	control_tx_polarity_invert(dev, p->invert_carrier_sense);
	o->invert_carrier_sense = p