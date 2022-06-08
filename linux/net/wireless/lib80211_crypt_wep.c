nable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = rxclk_rx_s_carrier(dev, p->carrier_freq,
						     &rxclk_divider);

		o->carrier_freq = p->carrier_freq;

		o->duty_cycle = p->duty_cycle = 50;

		control_rx_s_carrier_window(dev, p->carrier_freq,
					    &p->carrier_range_lower,
					    &p->carrier_range_upper);
		o->carrier_range_lower = p->carrier_range_lower;
		o->carrier_range_upper = p->carrier_range_upper;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, rxclk_divider);
	} else {
		p->max_pulse_width =
			    rxclk_rx_s_max_pulse_width(dev, p->max_pulse_width,
						       &rxclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->rxclk_divider, rxclk_divider);

	p->noise_filter_min_width =
			  filter_rx_s_min_width(dev, p->noise_filter_min_width);
	o->noise_filter_min_width = p->noise_filter_min_width;

	p->resolution = clock_divider_to_resolution(rxclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_rx_irq_watermark(dev, RX_FIFO_HALF_FULL);

	control_rx_s_edge_detection(dev, CNTRL_EDG_BOTH);

	o->invert_level = p->invert_level;
	atomic_set(&state->rx_invert, p->invert_level);

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		unsigned long flags;

		spin_lock_irqsave(&state->rx_kfifo_lock, flags);
		kfifo_reset(&state->rx_kfifo);
		/* reset tx_fifo too if there is one... */
		spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);
		if (p->interrupt_enable)
			irqenable_rx(dev, IRQEN_RSE | IRQEN_RTE | IRQEN_ROE);
		control_rx_enable(dev, p->enable);
	}

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

/* Transmitter */
static int cx23888_ir_tx_write(struct v4l2_subdev *sd, u8 *buf, size_t count,
			       ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	/* For now enable the Tx FIFO Service interrupt & pretend we did work */
	irqenable_tx(dev, IRQEN_TSE);
	*num = count;
	return 0;
}

static int cx23888_ir_tx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->tx_params_lock);
	memcpy(p, &state->tx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->tx_params_lock);

	/* Disable or slow down all IR Tx circuits and counters */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);
	control_tx_modulation_enable(dev, false);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, TXCLK_TCD);

	state->tx_params.shutdown = true;

	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->tx_params;
	u16 txclk_divider;

	if (p->shutdown)
		return cx23888_ir_tx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->tx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the transmitter */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);

	control_tx_modulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = txclk_tx_s_carrier(dev, p->carrier_freq,
						     &txclk_divider);
		o->carrier_freq = p->carrier_freq;

		p->duty_cycle = cduty_tx_s_duty_cycle(dev, p->duty_cycle);
		o->duty_cycle = p->duty_cycle;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, txclk_divider);
	} else {
		p->max_pulse_width =
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
	o->invert_carrier_sense = p->invert_carrier_sense;

	control_tx_level_invert(dev, p->invert_level);
	o->invert_level = p->invert_level;

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		if (p->interrupt_enable)
			irqenable_tx(dev, IRQEN_TSE);
		control_tx_enable(dev, p->enable);
	}

	mutex_unlock(&state->tx_params_lock);
	return 0;
}


/*
 * V4L2 Subdevice Core Ops
 */
static int cx23888_ir_log_status(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	char *s;
	int i, j;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 txclk = cx23888_ir_read4(dev, CX23888_IR_TXCLK_REG) & TXCLK_TCD;
	u32 rxclk = cx23888_ir_read4(dev, CX23888_IR_RXCLK_REG) & RXCLK_RCD;
	u32 cduty = cx23888_ir_read4(dev, CX23888_IR_CDUTY_REG) & CDUTY_CDC;
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 filtr = cx23888_ir_read4(dev, CX23888_IR_FILTR_REG) & FILTR_LPF;

	v4l2_info(sd, "IR Receiver:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_RXE ? "yes" : "no");
	v4l2_info(sd, "\tDemodulation from a carrier:       %s\n",
		  cntrl & CNTRL_DMD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_RFE ? "enabled" : "disabled");
	switch (cntrl & CNTRL_EDG) {
	case CNTRL_EDG_NONE:
		s = "disabled";
		break;
	case CNTRL_EDG_FALL:
		s = "falling edge";
		break;
	case CNTRL_EDG_RISE:
		s = "rising edge";
		break;
	case CNTRL_EDG_BOTH:
		s = "rising & falling edges";
