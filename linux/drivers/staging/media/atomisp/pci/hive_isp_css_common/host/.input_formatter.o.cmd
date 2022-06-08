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
		break;
	default:
		s = "??? edge";
		break;
	}
	v4l2_info(sd, "\tPulse timers' start/stop trigger:  %s\n", s);
	v4l2_info(sd, "\tFIFO data on pulse timer overflow: %s\n",
		  cntrl & CNTRL_R ? "not loaded" : "overflow marker");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_RIC ? "not empty" : "half full or greater");
	v4l2_info(sd, "\tLoopback mode:                     %s\n",
		  cntrl & CNTRL_LBM ? "loopback active" : "normal receive");
	if (cntrl & CNTRL_DMD) {
		v4l2_info(sd, "\tExpected carrier (16 clocks):      %u Hz\n",
			  clock_divider_to_carrier_freq(rxclk));
		switch (cntrl & CNTRL_WIN) {
		case CNTRL_WIN_3_3:
			i = 3;
			j = 3;
			break;
		case CNTRL_WIN_4_3:
			i = 4;
			j = 3;
			break;
		case CNTRL_WIN_3_4:
			i = 3;
			j = 4;
			break;
		case CNTRL_WIN_4_4:
			i = 4;
			j = 4;
			break;
		default:
			i = 0;
			j = 0;
			break;
		}
		v4l2_info(sd, "\tNext carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz\n",
			  i, j,
			  clock_divider_to_freq(rxclk, 16 + j),
			  clock_divider_to_freq(rxclk, 16 - i));
	}
	v4l2_info(sd, "\tMax measurable pulse width:        %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, rxclk),
		  pulse_width_count_to_ns(FIFO_RXTX, rxclk));
	v4l2_info(sd, "\tLow pass filter:                   %s\n",
		  filtr ? "enabled" : "disabled");
	if (filtr)
		v4l2_info(sd, "\tMin acceptable pulse width (LPF):  %u us, %u ns\n",
			  lpf_count_to_us(filtr),
			  lpf_count_to_ns(filtr));
	v4l2_info(sd, "\tPulse width timer timed-out:       %s\n",
		  stats & STATS_RTO ? "yes" : "no");
	v4l2_info(sd, "\tPulse width timer time-out intr:   %s\n",
		  irqen & IRQEN_RTE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO overrun:                      %s\n",
		  stats & STATS_ROR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO overrun interrupt:            %s\n",
		  irqen & IRQEN_ROE ? "enabled" : "disabled");
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_RBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_RSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_RSE ? "enabled" : "disabled");

	v4l2_info(sd, "IR Transmitter:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_TXE ? "yes" : "no");
	v4l2_info(sd, "\tModulation onto a carrier:         %s\n",
		  cntrl & CNTRL_MOD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_TFE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_TIC ? "not empty" : "half full or less");
	v4l2_info(sd, "\tOutput pin level inversion         %s\n",
		  cntrl & CNTRL_IVO ? "yes" : "no");
	v4l2_info(sd, "\tCarrier polarity:                  %s\n",
		  cntrl & CNTRL_CPL ? "space:burst mark:noburst"
				    : "space:noburst mark:burst");
	if (cntrl & CNTRL_MOD) {
		v4l2_info(sd, "\tCarrier (16 clocks):               %u Hz\n",
			  clock_divider_to_carrier_freq(txclk));
		v4l2_info(sd, "\tCarrier duty cycle:                %2u/16\n",
			  cduty + 1);
	}
	v4l2_info(sd, "\tMax pulse width:                   %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, txclk),
		  pulse_width_count_to_ns(FIFO_RXTX, txclk));
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_TBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_TSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_TSE ? "enabled" : "disabled");

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int cx23888_ir_g_register(struct v4l2_subdev *sd,
				 struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	reg->size = 4;
	reg->val = cx23888_ir_read4(state->dev, addr);
	return 0;
}

static int cx23888_ir_s_register(struct v4l2_subdev *sd,
				 const struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	cx23888_ir_write4(state->dev, addr, reg->val);
	return 0;
}
#endif

static const struct v4l2_subdev_core_ops cx23888_ir_core_ops = {
	.log_status = cx23888_ir_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = cx23888_ir_g_register,
	.s_register = cx23888_ir_s_register,
#endif
	.interrupt_service_routine = cx23888_ir_irq_handler,
};

static const struct v4l2_subdev_ir_ops cx23888_ir_ir_ops = {
	.rx_read = cx23888_ir_rx_read,
	.rx_g_parameters = cx23888_ir_rx_g_parameters,
	.rx_s_parameters = cx23888_ir_rx_s_parameters,

	.tx_write = cx23888_ir_tx_write,
	.tx_g_parameters = cx23888_ir_tx_g_parameters,
	.tx_s_parameters = cx23888_ir_tx_s_parameters,
};

static const struct v4l2_subdev_ops cx23888_ir_controller_ops = {
	.core = &cx23888_ir_core_ops,
	.ir = &cx23888_ir_ir_ops,
};

static const struct v4l2_subdev_ir_parameters default_rx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5, RC-6, and RC-6A carrier */

	/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
	/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
	.noise_filter_min_width = 333333, /* ns */
	.carrier_range_lower = 35000,
	.carrier_range_upper = 37000,
	.invert_level = false,
};

static const struct v4l2_subdev_ir_parameters default_tx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5 carrier */
	.duty_cycle = 25,      /* 25 %   - RC-5 carrier */
	.invert_level = false,
	.invert_carrier_sense = false,
};

int cx23888_ir_probe(struct cx23885_dev *dev)
{
	struct cx23888_ir_state *state;
	struct v4l2_subdev *sd;
	struct v4l2_subdev_ir_parameters default_params;
	int ret;

	state = kzalloc(sizeof(struct cx23888_ir_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	spin_lock_init(&state->rx_kfifo_lock);
	if (kfifo_alloc(&state->rx_kfifo, CX23888_IR_RX_KFIFO_SIZE,
			GFP_KERNEL)) {
		kfree(state);
		return -ENOMEM;
	}

	state->dev = dev;
	sd = &state->sd;

	v4l2_subdev_init(sd, &cx23888_ir_controller_ops);
	v4l2_set_subdevdata(sd, state);
	/* FIXME - fix the formatting of dev->v4l2_dev.name and use it */
	snprintf(sd->name, sizeof(sd->name), "%s/888-ir", dev->name);
	sd->grp_id = CX23885_HW_888_IR;

	ret = v4l2_device_register_subdev(&dev->v4l2_dev, sd);
	if (ret == 0) {
		/*
		 * Ensure no interrupts arrive from '888 specific conditions,
		 * since we ignore them in this driver to have commonality with
		 * similar IR controller cores.
		 */
		cx23888_ir_write4(dev, CX23888_IR_IRQEN_REG, 0);

		mutex_init(&state->rx_params_lock);
		default_params = default_rx_params;
		v4l2_subdev_call(sd, ir, rx_s_parameters, &default_params);

		mutex_init(&state->tx_params_lock);
		default_params = default_tx_params;
		v4l2_subdev_call(sd, ir, tx_s_parameters, &default_params);
	} else {
		kfifo_free(&state->rx_kfifo);
	}
	return ret;
}

int cx23888_ir_remove(struct cx23885_dev *dev)
{
	struct v4l2_subdev *sd;
	struct cx23888_ir_state *state;

	sd = cx23885_find_hw(dev, CX23885_HW_888_IR);
	if (sd == NULL)
		return -ENODEV;

	cx23888_ir_rx_shutdown(sd);
	cx23888_ir_tx_shutdown(sd);

	state = to_state(sd);
	v4l2_device_unregister_subdev(sd);
	kfifo_free(&state->rx_kfifo);
	kfree(state);
	/* Nothing more to free() as state held the actual v4l2_subdev object */
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23888_IR_H_
#define _CX23888_IR_H_
int cx23888_ir_probe(struct cx23885_dev *dev);
int cx23888_ir_remove(struct cx23885_dev *dev);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             