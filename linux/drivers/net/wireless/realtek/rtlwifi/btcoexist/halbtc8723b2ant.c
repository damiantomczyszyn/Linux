return cx_read(addr);
}

static inline int cx23888_ir_and_or4(struct cx23885_dev *dev, u32 addr,
				     u32 and_mask, u32 or_value)
{
	cx_andor(addr, ~and_mask, or_value);
	return 0;
}

/*
 * Rx and Tx Clock Divider register computations
 *
 * Note the largest clock divider value of 0xffff corresponds to:
 *	(0xffff + 1) * 1000 / 108/2 MHz = 1,213,629.629... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_clock_divider(unsigned int d)
{
	if (d > RXCLK_RCD + 1)
		d = RXCLK_RCD;
	else if (d < 2)
		d = 1;
	else
		d--;
	return (u16) d;
}

static inline u16 carrier_freq_to_clock_divider(unsigned int freq)
{
	return count_to_clock_divider(
			  DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, freq * 16));
}

static inline unsigned int clock_divider_to_carrier_freq(unsigned int divider)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, (divider + 1) * 16);
}

static inline unsigned int clock_divider_to_freq(unsigned int divider,
						 unsigned int rollovers)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ,
				 (divider + 1) * rollovers);
}

/*
 * Low Pass Filter register calculations
 *
 * Note the largest count value of 0xffff corresponds to:
 *	0xffff * 1000 / 108/2 MHz = 1,213,611.11... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_lpf_count(unsigned int d)
{
	if (d > FILTR_LPF)
		d = FILTR_LPF;
	else if (d < 4)
		d = 0;
	return (u16) d;
}

static inline u16 ns_to_lpf_count(unsigned int ns)
{
	return count_to_lpf_count(
		DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ / 1000000 * ns, 1000));
}

static inline unsigned int lpf_count_to_ns(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in ns */
	return DIV_ROUND_CLOSEST(count * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static inline unsigned int lpf_count_to_us(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in us */
	return DIV_ROUND_CLOSEST(count, CX23888_IR_REFCLK_FREQ / 1000000);
}

/*
 * FIFO register pulse width count computations
 */
static u32 clock_divider_to_resolution(u16 divider)
{
	/*
	 * Resolution is the duration of 1 tick of the readable portion of
	 * of the pulse width counter as read from the FIFO.  The two lsb's are
	 * not readable, hence the << 2.  This function returns ns.
	 */
	return DIV_ROUND_CLOSEST((1 << 2)  * ((u32) divider + 1) * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static u64 pulse_width_count_to_ns(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1) * 1000; /* millicycles */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000);     /* / MHz => ns */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return n;
}

static unsigned int pulse_width_count_to_us(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1);    /* cycles      */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000); /* / MHz => us */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return (unsigned int) n;
}

/*
 * Pulse Clocks computations: Combined Pulse Width Count & Rx Clock Counts
 *
 * The total pulse clock count is an 18 bit pulse width timer count as the most
 * significant part and (up to) 16 bit clock divider count as a modulus.
 * When the Rx clock divider ticks down to 0, it increments the 18 bit pulse
 * width timer count's least significant bit.
 */
static u64 ns_to_pulse_clocks(u32 ns)
{
	u64 clocks;
	u32 rem;
	clocks = CX23888_IR_REFCLK_FREQ / 1000000 * (u64) ns; /* millicycles  */
	rem = do_div(clocks, 1000);                         /* /1000 = cycles */
	if (rem >= 1000 / 2)
		clocks++;
	return clocks;
}

static u16 pulse_clocks_to_clock_divider(u64 count)
{
	do_div(count, (FIFO_RXTX << 2) | 0x3);

	/* net result needs to be rounded down and decremented by 1 */
	if (count > RXCLK_RCD + 1)
		count = RXCLK_RCD;
	else if (count < 2)
		count = 1;
	else
		count--;
	return (u16) count;
}

/*
 * IR Control Register helpers
 */
enum tx_fifo_watermark {
	TX_FIFO_HALF_EMPTY = 0,
	TX_FIFO_EMPTY      = CNTRL_TIC,
};

enum rx_fifo_watermark {
	RX_FIFO_HALF_FULL = 0,
	RX_FIFO_NOT_EMPTY = CNTRL_RIC,
};

static inline void control_tx_irq_watermark(struct cx23885_dev *dev,
					    enum tx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_TIC, level);
}

static inline void control_rx_irq_watermark(struct cx23885_dev *dev,
					    enum rx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_RIC, level);
}

static inline void control_tx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_TXE | CNTRL_TFE),
			   enable ? (CNTRL_TXE | CNTRL_TFE) : 0);
}

static inline void control_rx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_RXE | CNTRL_RFE),
			   enable ? (CNTRL_RXE | CNTRL_RFE) : 0);
}

static inline void control_tx_modulation_enable(struct cx23885_dev *dev,
						bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_MOD,
			   enable ? CNTRL_MOD : 0);
}

static inline void control_rx_demodulation_enable(struct cx23885_dev *dev,
						  bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_DMD,
			   enable ? CNTRL_DMD : 0);
}

static inline void control_rx_s_edge_detection(struct cx23885_dev *dev,
					       u32 edge_types)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_EDG_BOTH,
			   edge_types & CNTRL_EDG_BOTH);
}

static void control_rx_s_carrier_window(struct cx23885_dev *dev,
					unsigned int carrier,
					unsigned int *carrier_range_low,
					unsigned int *carrier_range_high)
{
	u32 v;
	unsigned int c16 = carrier * 16;

	if (*carrier_range_low < DIV_ROUND_CLOSEST(c16, 16 + 3)) {
		v = CNTRL_WIN_3_4;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 4);
	} else {
		v = CNTRL_WIN_3_3;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 3);
	}

	if (*carrier_range_high > DIV_ROUND_CLOSEST(c16, 16 - 3)) {
		v |= CNTRL_WIN_4_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 4);
	} else {
		v |= CNTRL_WIN_3_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 3);
	}
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_WIN, v);
}

static inline void control_tx_polarity_invert(struct cx23885_dev *dev,
					      bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_CPL,
			   invert ? CNTRL_CPL : 0);
}

static inline void control_tx_level_invert(struct cx23885_dev *dev,
					  bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_IVO,
			   invert ? CNTRL_IVO : 0);
}

/*
 * IR Rx & Tx Clock Register helpers
 */
static unsigned int txclk_tx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static unsigned int rxclk_rx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static u32 txclk_tx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

static u32 rxclk_rx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

/*
 * IR Tx Carrier Duty Cycle register helpers
 */
static unsigned int cduty_tx_s_duty_cycle(struct cx23885_dev *dev,
					  unsigned int duty_cycle)
{
	u32 n;
	n = DIV_ROUND_CLOSEST(duty_cycle * 100, 625); /* 16ths of 100% */
	if (n != 0)
		n--;
	if (n > 15)
		n = 15;
	cx23888_ir_write4(dev, CX23888_IR_CDUTY_REG, n);
	return DIV_ROUND_CLOSEST((n + 1) * 100, 16);
}

/*
 * IR Filter Register helpers
 */
static u32 filter_rx_s_min_width(struct cx23885_dev *dev, u32 min_width_ns)
{
	u32 count = ns_to_lpf_count(min_width_ns);
	cx23888_ir_write4(dev, CX23888_IR_FILTR_REG, count);
	return lpf_count_to_ns(count);
}

/*
 * IR IRQ Enable Register helpers
 */
static inline void irqenable_rx(struct cx23885_dev *dev, u32 mask)
{
	mask &= (IRQEN_RTE | IRQEN_ROE | IRQEN_RSE);
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG,
			   ~(IRQEN_RTE | IRQEN_ROE | IRQEN_RSE), mask);
}

static inline void irqenable_tx(struct cx23885_dev *dev, u32 mask)
{
	mask &= IRQEN_TSE;
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG, ~IRQEN_TSE, mask);
}

/*
 * V4L2 Subdevice IR Ops
 */
static int cx23888_ir_irq_handler(struct v4l2_subdev *sd, u32 status,
				  bool *handled)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	unsigned long flags;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);

	union cx23888_ir_fifo_rec rx_data[FIFO_RX_DEPTH];
	unsigned int i, j, k;
	u32 events, v;
	int tsr, rsr, rto, ror, tse, rse, rte, roe, kror;

	tsr = stats & STATS_TSR; /* Tx FIFO Service Request */
	rsr = stats & STATS_RSR; /* Rx FIFO Service Request */
	rto = stats & STATS_RTO; /* Rx Pulse Width Timer Time Out */
	ror = stats & STATS_ROR; /* Rx FIFO Over Run */

	tse = irqen & IRQEN_TSE; /* Tx FIFO Service Request IRQ Enable */
	rse = irqen & IRQEN_RSE; /* Rx FIFO Service Request IRQ Enable */
	rte = irqen & IRQEN_RTE; /* Rx Pulse Width Timer Time Out IRQ Enable */
	roe = irqen & IRQEN_ROE; /* Rx FIFO Over Run IRQ Enable */

	*handled = false;
	v4l2_dbg(2, ir_888_debug, sd, "IRQ Status:  %s %s %s %s %s %s\n",
		 tsr ? "tsr" : "   ", rsr ? "rsr" : "   ",
		 rto ? "rto" : "   ", ror ? "ror" : "   ",
		 stats & STATS_TBY ? "tby" : "   ",
		 stats & STATS_RBY ? "rby" : "   ");

	v4l2_dbg(2, ir_888_debug, sd, "IRQ Enables: %s %s %s %s\n",
		 tse ? "tse" : "   ", rse ? "rse" : "   ",
		 rte ? "rte" : "   ", roe ? "roe" : "   ");

	/*
	 * Transmitter interrupt service
	 */
	if (tse && tsr) {
		/*
		 * TODO:
		 * Check the watermark threshold setting
		 * Pull FIFO_TX_DEPTH or FIFO_TX_DEPTH/2 entries from tx_kfifo
		 * Push the data to the hardware FIFO.
		 * If there was nothing more to send in the tx_kfifo, disable
		 *	the TSR IRQ and notify the v4l2_device.
		 * If there was something in the tx_kfifo, check the tx_kfifo
		 *      level and notify the v4l2_device, if it is low.
		 */
		/* For now, inhibit TSR interrupt until Tx is implemented */
		irqenable_tx(dev, 0);
		events = V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ;
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_TX_NOTIFY, &events);
		*handled = true;
	}

	/*
	 * Receiver interrupt service
	 */
	kror = 0;
	if ((rse && rsr) || (rte && rto)) {
		/*
		 * Receive data on RSR to clear the STATS_RSR.
		 * Receive data on RTO, since we may not have yet hit the RSR
		 * watermark when we receive the RTO.
		 */
		for (i = 0, v = FIFO_RX_NDV;
		     (v & FIFO_RX_NDV) && !kror; i = 0) {
			for (j = 0;
			     (v & FIFO_RX_NDV) && j < FIFO_RX_DEPTH; j++) {
				v = cx23888_ir_read4(dev, CX23888_IR_FIFO_REG);
				rx_data[i].hw_fifo_data = v & ~FIFO_RX_NDV;
				i++;
			}
			if (i == 0)
				break;
			j = i * sizeof(union cx23888_ir_fifo_rec);
			k = kfifo_in_locked(&state->rx_kfifo,
				      (unsigned char *) rx_data, j,
				      &state->rx_kfifo_lock);
			if (k != j)
				kror++; /* rx_kfifo over run */
		}
		*handled = true;
	}

	events = 0;
	v = 0;
	if (kror) {
		events |= V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver software FIFO overrun\n");
	}
	if (roe && ror) {
		/*
		 * The RX FIFO Enable (CNTRL_RFE) must be toggled to clear
		 * the Rx FIFO Over Run status (STATS_ROR)
		 */
		v |= CNTRL_RFE;
		events |= V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver hardware FIFO overrun\n");
	}
	if (rte && rto) {
		/*
		 * The IR Receiver Enable (CNTRL_RXE) must be toggled to clear
		 * the Rx Pulse Width Timer Time Out (STATS_RTO)
		 */
		v |= CNTRL_RXE;
		events |= V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED;
	}
	if (v) {
		/* Clear STATS_ROR & STATS_RTO as needed by resetting hardware */
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl & ~v);
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl);
		*handled = true;
	}

	spin_lock_irqsave(&state->rx_kfifo_lock, flags);
	if (kfifo_len(&state->rx_kfifo) >= CX23888_IR_RX_KFIFO_SIZE / 2)
		events |= V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ;
	spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);

	if (events)
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_RX_NOTIFY, &events);
	return 0;
}

/* Receiver */
static int cx23888_ir_rx_read(struct v4l2_subdev *sd, u8 *buf, size_t count,
			      ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	bool invert = (bool) atomic_read(&state->rx_invert);
	u16 divider = (u16) atomic_read(&state->rxclk_divider);

	unsigned int i, n;
	union cx23888_ir_fifo_rec *p;
	unsigned u, v, w;

	n = count / sizeof(union cx23888_ir_fifo_rec)
		* sizeof(union cx23888_ir_fifo_rec);
	if (n == 0) {
		*num = 0;
		return 0;
	}

	n = kfifo_out_locked(&state->rx_kfifo, buf, n, &state->rx_kfifo_lock);

	n /= sizeof(union cx23888_ir_fifo_rec);
	*num = n * sizeof(union cx23888_ir_fifo_rec);

	for (p = (union cx23888_ir_fifo_rec *) buf, i = 0; i < n; p++, i++) {

		if ((p->hw_fifo_data & FIFO_RXTX_RTO) == FIFO_RXTX_RTO) {
			/* Assume RTO was because of no IR light input */
			u = 0;
			w = 1;
		} else {
			u = (p->hw_fifo_data & FIFO_RXTX_LVL) ? 1 : 0;
			if (invert)
				u = u ? 0 : 1;
			w = 0;
		}

		v = (unsigned) pulse_width_count_to_ns(
				  (u16)(p->hw_fifo_data & FIFO_RXTX), divider) / 1000;
		if (v > IR_MAX_DURATION)
			v = IR_MAX_DURATION;

		p->ir_core_data = (struct ir_raw_event)
			{ .pulse = u, .duration = v, .timeout = w };

		v4l2_dbg(2, ir_888_debug, sd, "rx read: %10u ns  %s  %s\n",
			 v, u ? "mark" : "space", w ? "(timed out)" : "");
		if (w)
			v4l2_dbg(2, ir_888_debug, sd, "rx read: end of rx\n");
	}
	return 0;
}

static int cx23888_ir_rx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->rx_params_lock);
	memcpy(p, &state->rx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->rx_params_lock);

	/* Disable or slow down all IR Rx circuits and counters */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);
	control_rx_demodulation_enable(dev, false);
	control_rx_s_edge_detection(dev, CNTRL_EDG_NONE);
	filter_rx_s_min_width(dev, 0);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, RXCLK_RCD);

	state->rx_params.shutdown = true;

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->rx_params;
	u16 rxclk_divider;

	if (p->shutdown)
		return cx23888_ir_rx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->rx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the receiver */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);

	control_rx_demodulation_enable(dev, p->modulation);
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         /* SPDX-License-Identifier: GPL-2.0-or-later */
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later

/*
 * netup-eeprom.c
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#
#include "cx23885.h"
#include "netup-eeprom.h"

#define EEPROM_I2C_ADDR 0x50

int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr)
{
	int ret;
	unsigned char buf[2];

	/* Read from EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &buf[0],
			.len	= 1
		}, {
			.addr	= EEPROM_I2C_ADDR,
			.flags	= I2C_M_RD,
			.buf	= &buf[1],
			.len	= 1
		}

	};

	buf[0] = addr;
	buf[1] = 0x0;

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		pr_err("eeprom i2c read error, status=%d\n", ret);
		return -1;
	}

	return buf[1];
};

int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data)
{
	int ret;
	unsigned char bufw[2];

	/* Write into EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &bufw[0],
			.len	= 2
		}
	};

	bufw[0] = addr;
	bufw[1] = data;

	ret = i2c_transfer(i2c_adap, msg, 1);

	if (ret != 1) {
		pr_err("eeprom i2c write error, status=%d\n", ret);
		return -1;
	}

	mdelay(10); /* prophylactic delay, datasheet write cycle time = 5 ms */
	return 0;
};

void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo)
{
	int i, j;

	cinfo->rev =  netup_eeprom_read(i2c_adap, 63);

	for (i = 64, j = 0; i < 70; i++, j++)
		cinfo->port[0].mac[j] =  netup_eeprom_read(i2c_adap, i);

	for (i = 70, j = 0; i < 76; i++, j++)
		cinfo->port[1].mac[j] =  netup_eeprom_read(i2c_adap, i);
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             // SPDX-License-Identifier: GPL-2.0-or-later

/*
 * netup-eeprom.c
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#
#include "cx23885.h"
#include "netup-eeprom.h"

#define EEPROM_I2C_ADDR 0x50

int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr)
{
	int ret;
	unsigned char buf[2];

	/* Read from EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &buf[0],
			.len	= 1
		}, {
			.addr	= EEPROM_I2C_ADDR,
			.flags	= I2C_M_RD,
			.buf	= &buf[1],
			.len	= 1
		}

	};

	buf[0] = addr;
	buf[1] = 0x0;

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		pr_err("eeprom i2c read error, status=%d\n", ret);
		return -1;
	}

	return buf[1];
};

int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data)
{
	int ret;
	unsigned char bufw[2];

	/* Write into EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &bufw[0],
			.len	= 2
		}
	};

	bufw[0] = addr;
	bufw[1] = data;

	ret = i2c_transfer(i2c_adap, msg, 1);

	if (ret != 1) {
		pr_err("eeprom i2c write error, status=%d\n", ret);
		return -1;
	}

	mdelay(10); /* prophylactic delay, datasheet write cycle time = 5 ms */
	return 0;
};

void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo)
{
	int i, j;

	cinfo->rev =  netup_eeprom_read(i2c_adap, 63);

	for (i = 64, j = 0; i < 70; i++, j++)
		cinfo->port[0].mac[j] =  netup_eeprom_read(i2c_adap, i);

	for (i = 70, j = 0; i < 76; i++, j++)
		cinfo->port[1].mac[j] =  netup_eeprom_read(i2c_adap, i);
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-eeprom.h
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef NETUP_EEPROM_H
#define NETUP_EEPROM_H

struct netup_port_info {
	u8 mac[6];/* card MAC address */
};

struct netup_card_info {
	struct netup_port_info port[2];/* ports - 1,2 */
	u8 rev;/* card revision */
};

extern int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr);
extern int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data);
extern void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-eeprom.h
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef NETUP_EEPROM_H
#define NETUP_EEPROM_H

struct netup_port_info {
	u8 mac[6];/* card MAC address */
};

struct netup_card_info {
	struct netup_port_info port[2];/* ports - 1,2 */
	u8 rev;/* card revision */
};

extern int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr);
extern int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data);
extern void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * netup-init.c
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "netup-init.h"

static void i2c_av_write(struct i2c_adapter *i2c, u16 reg, u8 val)
{
	int ret;
	u8 buf[3];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 3
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static void i2c_av_write4(struct i2c_adapter *i2c, u16 reg, u32 val)
{
	int ret;
	u8 buf[6];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 6
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val & 0xff;
	buf[3] = (val >> 8) & 0xff;
	buf[4] = (val >> 16) & 0xff;
	buf[5] = val >> 24;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static u8 i2c_av_read(struct i2c_adapter *i2c, u16 reg)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 2
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);

	msg.flags = I2C_M_RD;
	msg.len = 1;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c read error!\n", __func__);

	return buf[0];
}

static void i2c_av_and_or(struct i2c_adapter *i2c, u16 reg, unsigned and_mask,
								u8 or_value)
{
	i2c_av_write(i2c, reg, (i2c_av_read(i2c, reg) & and_mask) | or_value);
}
/* set 27MHz on AUX_CLK */
void netup_initialize(struct cx23885_dev *dev)
{
	struct cx23885_i2c *i2c_bus = &dev->i2c_bus[2];
	struct i2c_adapter *i2c = &i2c_bus->i2c_adap;

	/* Stop microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x00);

	/* Aux PLL frac for 27 MHz */
	i2c_av_write4(i2c, 0x114, 0xea0eb3);

	/* Aux PLL int for 27 MHz */
	i2c_av_write4(i2c, 0x110, 0x090319);

	/* start microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x10);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * netup-init.c
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "netup-init.h"

static void i2c_av_write(struct i2c_adapter *i2c, u16 reg, u8 val)
{
	int ret;
	u8 buf[3];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 3
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static void i2c_av_write4(struct i2c_adapter *i2c, u16 reg, u32 val)
{
	int ret;
	u8 buf[6];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 6
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val & 0xff;
	buf[3] = (val >> 8) & 0xff;
	buf[4] = (val >> 16) & 0xff;
	buf[5] = val >> 24;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static u8 i2c_av_read(struct i2c_adapter *i2c, u16 reg)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 2
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);

	msg.flags = I2C_M_RD;
	msg.len = 1;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c read error!\n", __func__);

	return buf[0];
}

static void i2c_av_and_or(struct i2c_adapter *i2c, u16 reg, unsigned and_mask,
								u8 or_value)
{
	i2c_av_write(i2c, reg, (i2c_av_read(i2c, reg) & and_mask) | or_value);
}
/* set 27MHz on AUX_CLK */
void netup_initialize(struct cx23885_dev *dev)
{
	struct cx23885_i2c *i2c_bus = &dev->i2c_bus[2];
	struct i2c_adapter *i2c = &i2c_bus->i2c_adap;

	/* Stop microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x00);

	/* Aux PLL frac for 27 MHz */
	i2c_av_write4(i2c, 0x114, 0xea0eb3);

	/* Aux PLL int for 27 MHz */
	i2c_av_write4(i2c, 0x110, 0x090319);

	/* start microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x10);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      ¬2     4     (            GNU  ΐ       ΐ          θόÿÿÿWV‰Ζ‰Π‰ΚS‹ά   ‹|$‹™  ƒγύƒψ‹ά   ΐƒΰώ  	Ψ‰  ‹†ά      ‹ 1Ϋ€δ_…Ò•ΓΑγ	Γ‹†ά   ‰   ‰Ψ‰ΪΑψƒΰ…ÿEψ‹†ά   €Ξ ‰   ΈΗ  θόÿÿÿ‹†ά   ‰   ‰ψ[^_ΓfθόÿÿÿS‰ΓΓΌ  ƒμHd΅    ‰D$D‰ΰθόÿÿÿ‹D$,=9‰ ‡L  =7‰ ‡   =Υ ‡6  =Σ ‡   =Q… ‡   =O… ‡   =ν„ ‡
  =λ„ ‡   =Wu „   ‡ξ  =9K „   ‡έ  =ΕΨ „   ‡Μ  =»Ψ „   ‡»  =ΩN „   ‡ª  =L „   ‡™  =o> „   ‡  =e> „   ‡w  =“8 „   ‡f  =77 „   ‡U  =Σ6 „   ‡D  =Ι6 „   ‡3  =6 „   ‡"  =ύ4 „   ‡  =™4 „   ‡   ='3 „   =Γ2 ‡κ   =¤2 ‡?   =»0 „   ‡Ξ   =ϋ, „   ‡½   =ρ, „   ‡¬   =Σ, „   ‡›   =Ι, „   ‡   =9+ „   w}=? „   wp=+ „   = w^= ‡W   = „   wF= „   w9=Ώ „   w,=Ό „   w=a „   w=V  ‡όÿÿÿ=πU  wPSh    θόÿÿÿƒΔι   ιo   ΄&    v θόÿÿÿQÿUWVSƒϊ†Σ   ‹8‰Ξ…Ι…ν   ‹Έ  ƒωwƒωws[‰π^_]Γƒω„Ν   ƒω,„®   wεƒω*uΈϋÿÿÿ½   ‹—ά   Β  ‹
!Θ‰»Θ   ¶    ΈX‰A θόÿÿÿƒλuρ‹‡ά     ‹€  	Ε‰*[‰π^_]ΓΊ   ΣβχΒ`2 u€ζu@ƒω…pÿÿÿΈύÿÿÿ½   λv ƒιƒαχ„uÿÿÿιPÿÿÿ΄&    1φ[‰π^_]Γ΄&    ‹hƒύtƒύ…(ÿÿÿιCÿÿÿ‹P‰ψθόÿÿÿ‰π[^_]ΓΈώÿÿÿι2ÿÿÿ΄&    θόÿÿÿS‰Γ‹€Έ  ƒψ„	  wwƒψ	„#  †   ƒψ„'  †Q  ƒψt†  ƒψ„½  Ή   ‰ΨΊ `  θόÿÿÿΊ `  ‰ΨθόÿÿÿΈd   θόÿÿÿ‰ΨΊ @  θόÿÿÿΈd   [ιόÿÿÿ΄&    ƒψ)‡  ƒψ'‡  ƒψ!„Γ  †G  ƒψ&„r  ƒψ'…ι  ‹ƒά   (  ‹ƒβώ‰‹ƒά     ‹Κ   ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹Κ  ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹βώÿώÿ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹Κ  ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹βόÿόÿ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹Κ  ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹Κ  ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹βύÿύÿ‰‹ƒά     ‹Κ  ‰‹ƒά     ‹βϋÿϋÿ‰‹ƒά     ‹€    ‰Έ<   [ιόÿÿÿ΄&    fƒψvPƒψ„ξ   Ή   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈd   θόÿÿÿΊ   ‰ΨθόÿÿÿΈd   [ιόÿÿÿƒψ$…[  Ή   ‰ΨΊ`  θόÿÿÿ‰ΨΊ`  θόÿÿÿΊ   ‰ΨθόÿÿÿΈ   θόÿÿÿ‰ΨΊ   [ιόÿÿÿv ƒψ„/  v]ƒψuT‹ƒά     ‹Κ   ‰Έ   θόÿÿÿ‹ƒά     ‹ƒβϋ‰Έ   θόÿÿÿ‹ƒά     ‹€    ‰[Γfƒψt£ƒψuς‹ƒά     ‹€    ‰[Γv ƒψ„  †  ƒψ†hÿÿÿƒψu·‹ƒά   Ί6   ‰(  Ί   ‹ƒά   ‰$  ‹ƒά      ‹ƒΚ‰ΈΘ   θόÿÿÿ‹ƒά      ‹€ζχ‰ΈΘ   θόÿÿÿ‹ƒά      ‹€   €Μ‰ΈΘ   [ιόÿÿÿ΄&    ƒψ„·  †Ι   ƒψt	ƒψ…ÿÿÿ‹ƒά     ‹Κ   ‰Έ   θόÿÿÿ‹ƒά     ‹ƒβϊ‰Έ   θόÿÿÿ‹ƒά     ‹€    ‰[Γƒψ
…Έώÿÿ‹ƒά     ‹Κ   ‰Έ   θόÿÿÿ‹ƒά     ‹ƒβπ‰Έ   θόÿÿÿ‹ƒά     ‹€    ‰[Γ¶    ƒψ„aώÿÿƒψ„nÿÿÿƒψ…AώÿÿΉ   ‰ΨΊ ΐ  θόÿÿÿΊ ΐ  ‰ΨθόÿÿÿΈd   θόÿÿÿΊ ΐ  ‰ΨθόÿÿÿΈd   θόÿÿÿΉ   ‰ΨΊ   θόÿÿÿΊ   ‰ΨθόÿÿÿΈ   θόÿÿÿΊ   ‰ΨθόÿÿÿΈ   θόÿÿÿ‰ΨΊ   θόÿÿÿΈ   [ιόÿÿÿt& ƒψ1„χ  †   ƒψ5w\ƒψ3wc‹ƒά   Ί7   ‰(  Ή   ‰ΨΊ  θόÿÿÿΊ  ‰ΨθόÿÿÿΈd   θόÿÿÿ‰ΨΊ  [ιόÿÿÿfƒψ%„ύÿÿι>ύÿÿfƒθ8ƒψ‡0ύÿÿΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈd   θόÿÿÿΊ   ι=όÿÿƒψ-„cÿÿÿ†ª   ƒψ1„ηόÿÿ‹ƒά     ‹Κ  ‰‹ƒά     ‹βϋÿώÿ‰Έd   θόÿÿÿ‹ƒά     ‹Κ  ‰‹ƒά     ‹βύÿώÿ‰Ί7   ‹ƒά   ‰(  Ί   ‹ƒά   ‰$  ‹ƒά   Ί Γ  ‰   1Ò‹ƒά   ‰  [Γƒψ*„ιϋÿÿƒψ,„|ύÿÿ[Γ¶    ‹ƒά     ‹Κ   ‰‹ƒά     ‹βωÿώÿ‰Έd   θόÿÿÿ‹ƒά     ‹ƒΚ‰Ί7   ‹ƒά   ‰(  Ί P  ‹ƒά   ‰$  Ί   ‹ƒά   ιXÿÿÿ΄&    Ή   ‰ΨΊ   θόÿÿÿΊ   ‰ΨθόÿÿÿΈd   θόÿÿÿ‰ΨΊ   [ιόÿÿÿt& ‹ƒά     ‹Κ   ‰‹ƒά     ‹ƒβϊ‰Έ   θόÿÿÿιbόÿÿfΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈd   θόÿÿÿΊ   ιIϊÿÿ΄&    ‹ƒά     ‹Κ   ‰‹ƒά     ‹βϋÿόÿ‰Έd   θόÿÿÿ‹ƒά     ‹Κ  ι-ώÿÿ΄&    t& θόÿÿÿUWVS‰Γƒμ0‹‹Έ  d΅    ‰D$,1ΐƒω„w  vEƒιƒωwΈ   Σΰ© γud¨…Χ  ƒωt71φ‹D$,d+    …±  ƒΔ0‰π[^_]Γt& ƒω„§   1φƒωvΠƒωuh    1φjθόÿÿÿXZλΉ΄&    fƒωu«‹5   …φtΊ   ‰Ψθόÿÿÿ‰ƒ¨  …ΐ„?  ‹ƒ”  …ΐ„wÿÿÿ‹Pd‹…Ò„jÿÿÿ‹Z…Ϋ„_ÿÿÿ‹    Ή   …Ò„ζ  ‹r…φ„Ϋ  Ί   θόÿÿÿ1φι2ÿÿÿf‹   …Ι„ ÿÿÿΊ   ‰Ψθόÿÿÿ‰ƒ¨  …ΐ„ΐ  ‹ƒ”  …ΐ„ψώÿÿ‹Pd‹…Ò„λώÿÿ‹Z…Ϋ„ΰώÿÿ‹    Ή    …Ò„x  ‹r…φ„m  Ί   θόÿÿÿ1φι³ώÿÿv ‰Ψθόÿÿÿ‰Ζ…ΐ…ώÿÿΊ   ‰Ψθόÿÿÿ‰ƒ¨  ‹ƒ”  …ΐ„}ώÿÿ‹Pd‹…Ò„pώÿÿ‹Z…Ϋ„eώÿÿ‹    Ή   …Ò„έ  ‹z…ÿ„Ò  Ί   θόÿÿÿι:ώÿÿ΄&    v ‰Ψθόÿÿÿ‰Ζ…ΐ…ώÿÿΊ   ‰Ψθόÿÿÿ‰ƒ¨  ‰Α‹ƒ”  …ΐtA‹Pd‹…Òt8‹z…ÿt1‹    Ή    …Ò„@  ‹j…ν„5  Ί   θόÿÿÿ‹‹¨  v …Ι„¶ύÿÿ‹Ad‹@…ΐ„Ϊ   ‹x…ÿ„Ο   ΅   ‰β…ΐ„Τ   ‹h…ν„Ι   ‰Θθόÿÿÿ‹‹¨  ΖD$ ΖD$
 ΖD$…Ι„_ύÿÿ‹Ad‹@…ΐtw‹x…ÿtp΅   ‰β…ΐ„Έ   ‹h…ν„­   ‰Θθόÿÿÿ‹‹¨  ΖD$
…Ι„ύÿÿ‹Ad‹@…ΐ„ύÿÿ‹X…Ϋ„ύÿÿ΅   ‰β…ΐtN‹x…ÿtG‰Θθόÿÿÿιεόÿÿt& ΖD$
λ½΄&    fΖD$ ΖD$
 ΖD$ι[ÿÿÿ‰Θθόÿÿÿ‹‹¨  ι2ÿÿÿ‰ΘθόÿÿÿιόÿÿΊ   θόÿÿÿ‹‹¨  ιΙώÿÿ‰Θθόÿÿÿ‹‹¨  ιNÿÿÿΊ   θόÿÿÿιhόÿÿΊ   1φθόÿÿÿιWόÿÿΊ   θόÿÿÿιFόÿÿΎνÿÿÿι>όÿÿθόÿÿÿ΄&    fθόÿÿÿ‹Έ  ƒω3wpS‰ΓƒωvƒιΈ   Σΰ© γu¨u3[Γƒωt
ƒωt&ƒωuξ‰ΨΊ   θόÿÿÿΗƒ¨      [Γ΄&    f‰ΨΊ   θόÿÿÿ‰ΨθόÿÿÿΗƒ¨      [ΓΓ΄&    ΄&    θόÿÿÿ‹Έ  ƒω3wƒωvƒιΊ   ΣβχΒ γuƒβu,Γv ƒωt
ƒωtƒωuν‹¨  …ÒtγΊ   ιόÿÿÿt& ‹¨  …ΙtΚΊ   ιόÿÿÿt& θόÿÿÿWVS‰Γƒμ‹»`  d΅    ‰D$1ΐ…ÿ„9  ‹ƒΈ  ‰Βƒψ	t‡ζ  ƒψ„¬  Hϋƒω‡1  ‰Βƒψ&†  ƒψ2„µ  †η  ƒψ5„f  †Ψ  ƒψ;„  †¶  HΔƒω‡\  ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ƒψ"‡½  JϊΈ   Σΰ©ΕXί„³  j “
  Ή9   CjDθόÿÿÿ‰ƒ”  ZY…ΐ„Τ  “π   ‰   ‹ƒ”  Η€      ‹ƒ”  …ΐ„ª  ‹Pd‹…Ò„  ‹R…Ò„’  ‹    …Ι„|  ‹I…Ι„q  θόÿÿÿ‹“Έ  ιo  ƒψt3†Ϋ   Hφƒω‡X  Ηƒ     Ηƒ      Ηƒ$     v ΗƒΨ     Ηƒά     Ηƒΰ     ƒϊ"‡¶  ƒϊ‡πώÿÿBÿƒψ†ωώÿÿƒϊ„"  ƒϊ„  ‹D$d+    …3  ƒΔ[^_ΓfHΚƒω‡%ώÿÿΎP   ƒτ  Ή   Ί    f‰³φ  θόÿÿÿ‹»`  …ÿ„
  ‹ƒΈ  ιιύÿÿƒψ…€  ΗƒΨ    ‹ƒά   Ί   Ηƒά     Ηƒΰ     Ηƒδ      Ηƒθ  ΜG ‰„ Ηƒ     ‹“Έ  Ηƒ      Ηƒ$     ιϋώÿÿt& θόÿÿÿv ‹“Έ  ƒϊ6…Α   j “ό   CΉA   jAθόÿÿÿ‹“Έ  _Xƒϊ…ήώÿÿ‹D$d+    …  ƒΔ‰Ψ[^_ιόÿÿÿHΘƒω†Jύÿÿι΅  fƒψ3…§   ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     Bάƒψ‡MÿÿÿΉωώ}£Α‚>ύÿÿƒϊ6„?ÿÿÿƒϊ7…Uÿÿÿj »ό   sΉA   j@‰ϊ‰πθόÿÿÿ‰ϊΉA   ‰πj jAθόÿÿÿ‹“Έ  ƒΔιÿÿÿt& ƒψ4…ζ  ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ιKÿÿÿ¶    ƒψ)wKƒψ'…  ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ    Ηƒ      Ηƒ$     ιNόÿÿƒψ-„°ώÿÿv,ƒψ2„;  ΗƒΨ     Ηƒά     Ηƒΰ     ιψϋÿÿƒψ,„·όÿÿι
  v ƒψ‡Χ   ƒψ„φ  †€όÿÿƒψ„fύÿÿ†	  ƒψ…Χ   ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ι­όÿÿ΄&    ΎP   Ί    ƒl  Ή   f‰³n  θόÿÿÿ‹ƒΈ  ‰Βƒψ†®   Hάƒω‡³ϊÿÿΎ   Σζ‰ρχΖ… σ…H  α   …mόÿÿƒψ&‡’ϊÿÿHβΎ   Σζ‰ρφΑ¨…Χώÿÿƒα…­ϊÿÿƒψ&„΅  Ηƒ     ‰ΒΗƒ      Ηƒ$     ιΒϋÿÿt& ƒψ„rϊÿÿvHκƒω†…ώÿÿλΎ΄&    …vώÿÿλ―…ΐt«‰ΑΎ   Σζ‰ρχΖ†<……   α`  u]ƒψ……ώÿÿ‹‹`  …Ι…uÿÿÿ€=    „tKΊΰ   ‰ΨθΡηÿÿ‹ƒΈ  ‰Βιµωÿÿt& ƒϊ…mόÿÿ‹5   …φ„Oϋÿÿι1ϊÿÿt& ‹“`  …Ò…ωÿÿΊ    ‰Ψθ†ηÿÿι|ϋÿÿ‹‹`  …Ιt™ιώÿÿΗƒΨ     Ηƒά     Ηƒΰ     ισϊÿÿt& ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ι”ωÿÿ΄&    ΗƒΨ     ‹ƒά   Ηƒά     Ηƒΰ       Ηƒ     Ηƒ      Ηƒ$     ‹Κ  ‰Ή   Ί'  Έθ  θόÿÿÿ‹ƒά     ‹ƒβύ‰Ή   Ί'  Έθ  1φθόÿÿÿv ‹ƒά     ‹ƒβψ‰Ή   Ί'  Έθ  θόÿÿÿ‹ƒά     ‹:‰π‰ρΑψχΡ¶€    ƒαΣψ‰Αƒα‰ΘƒΘχΠ!ψ	ΘƒΘ‰Ή   Ί'  Έθ  ƒΖθόÿÿÿƒώHu†‹ƒά     ‹ƒΚ‰‹“Έ  ιOωÿÿ΄&    fΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ι5ωÿÿ΄&    ΗƒΨ     Ηƒά     Ηƒΰ     ιωÿÿƒ»`   „{ύÿÿιDχÿÿ‰Ψ‰\$ΗD$H   ΗD$    θόÿÿÿƒό   Ί    θόÿÿÿ΅F  …ΐ„6  ι1  θόÿÿÿθόÿÿÿ‹P(Η   ‹‚μ  ―‚π  ‹’θ  ―B‹T$Αθ‰1ΐΓ΄&    ΄&    θόÿÿÿUWVS‰Γƒμ‹ ‹“  ‹p(‹ƒ  ƒΐΎΈ  ‰B‹ƒ  Η   p‹ƒ  ‹“  ƒΐ‰B‹ƒ  Η@    ‰ψθόÿÿÿ‹®ψ  ψ  ‰Βƒx  9ιtS‹«  M    ‹®ό  ‰†ό  ‰‹x  ‰«|  ‰E ‹E‹‹  ƒ=x  ‰H‡ψ  ƒΔ‰ψ[^_]ιόÿÿÿt& ‹®ό  ‰†ό  ‰‹x  ‰«|  ‰E ƒ=x  ‡Φ  ƒΔ‰ψ[^_]ιόÿÿÿfθόÿÿÿUWVS‹h(‹…ά   @  ‹ƒβξ‰½Έ  ψ  ‰ψθόÿÿÿ‰Ζ‹…ψ  9Γt@v ‹…ψ  ‹P‹-x  ‰Q‰
Ί   Η€x     Η€|  "  θόÿÿÿ‹…ψ  9ΨuΓ[‰ς‰ψ^_]ιόÿÿÿ¶    θόÿÿÿVSθόÿÿÿ‹°(  ‹FƒΖ9Ζt$X΄‹Cd‹ …ΐt‹…Òt‰Ψθόÿÿÿ‹CLX΄9Ζuΰ1ΐ[^Γ΄&    t& θόÿÿÿWV‰ΞSθόÿÿÿ‹Έ(  ‹‡Έ  ƒψ4tƒθ8ƒψwPΗF   ‹‡ά  ƒΗ‰F‹X΄9Ηt(t& ‹Cd‹@…ΐt‹H…Ιt	‰ς‰Ψθόÿÿÿ‹CLX΄9Ηuά1ΐ[^_Γ¶    ƒΏ€  u§Έκÿÿÿλζ΄&    θόÿÿÿWV‰ΞSθόÿÿÿ‹Έ(  ‹‡Έ  ƒψ4tƒθ8ƒψwH‹…ΐuK‹GƒΗX΄9Ηt)t& ‹Cd‹@…ΐt‹H…Ιt	‰ς‰Ψθόÿÿÿ‹CLX΄9Ηuά1ΐ[^_Γ¶    ƒΏ€  u―ΈκÿÿÿλζθόÿÿÿWV‰ΞSθόÿÿÿ‹Έ(  ‹‡Έ  ƒψ4tƒθ8ƒψwX‹…ΐu[FΉ   Ίª  ƒΗθόÿÿÿ‹X΄9Ηt(t& ‹Cd‹@…ΐt‹H…Ιt	‰ς‰Ψθόÿÿÿ‹CLX΄9Ηuά1ΐ[^_Γ¶    ƒΏ€  uΈκÿÿÿλζθόÿÿÿS‰Λθόÿÿÿƒ;‹(  uH‹Cƒθƒψw=ΗC    ΗC    ΗCΠ  ‹‚x  % ω  ƒψΐƒΰ`ΰ  ‰C1ΐ[Γ΄&    fΈκÿÿÿ[Γ΄&    fθόÿÿÿV‰ΞS‹\$θόÿÿÿ‹€(  ‹€x  %ÿÿ ƒώu(ƒψÒƒβΥƒΒ6ƒψΐ‰ƒΰΟƒΐ;‰C1ΐ[^Γ΄&    Έκÿÿÿλξ΄&    fθόÿÿÿVS‰Λθόÿÿÿ‹°(  ΅x  …ΐ…  ‹†x  ‹–|  ‰1ΐ‰S[^Γ΄&    fθόÿÿÿVS‰Λθόÿÿÿ‹(  ‹‚μ  ‰C‹π  ‰K‹²τ  ‰s‹²θ  ‹6‰s‹’θ  ―BΗC   Αθ―Θ‰C1ΐ‰K[^Γt& θόÿÿÿ‹…ΐu΅ΤI  ‰A,1ΐΓ΄&    v ΈκÿÿÿΓ΄&    v θόÿÿÿ‰Β‹ ‹@(ιόÿÿÿ΄&    ΄&    θόÿÿÿU1ÒWVS‰Γƒμ‹ ‹p(‰Ψθόÿÿÿ‹k‰Η‹†θ  ‹@―†μ  Αθ‰ƒ€  ‰Β…ν…Ό   ―–π  …Ò…  ‹–τ  ƒϊ„ΐ   wTƒϊ„+  ƒϊ…  ‹®Μ   ÿ¶π  “„  j P‰θj jÿ‹θόÿÿÿƒΔƒ=x  ‡6  1ΐƒΔ[^_]Γ¶    ƒϊ„  ƒϊ…J  ‹π  ‹®Μ   “„  ΡιQ―Θj P‰θj Q‹θόÿÿÿƒΔλ¥΄&    v ―–π  9ST‚  ‰SP‹–τ  ƒϊ…@ÿÿÿ‹x  χ†x   ω  tiΈ    ‹‰  …Ι„΄   …Ò…p  Η$    ‰Α‹–Μ   «„  ‰T$‹–π  ΡκR‰κQQPÿt$‹‹D$θόÿÿÿƒΔι	ÿÿÿ¶    ‹®Μ   ÿ¶π  “„  j Pjÿj ‹‰θθόÿÿÿƒΔιΩώÿÿ¶    ‹π  ‹®Μ   “„  ΡιQ―Θj PQλΙ΄&    v ƒΔΈκÿÿÿ[^_]Γv …Ò…’  ‰$‰Α1ΐιIÿÿÿΈκÿÿÿιώÿÿθόÿÿÿVS‰Λθόÿÿÿ‹5ΤI  ‹€(  9s…­   ‹€x  ‹S% ω  ƒψΐƒΰ`ΰ  …Òt^ƒϊwyƒϊ…   Ί   j sΉΠ  j PC‰SΊ0   j P‰πjθόÿÿÿ‹C―ΨI  ΗC   ƒΔΑθ‰C―C‰C1ΐ[^Γ‰ΑΊ   Ρι9Kw§Ί   ‰Θλ΄&    fJόƒωΉ   CΡλ…Έκÿÿÿ[^Γ΄&    Ρθ‰Α‰Θιjÿÿÿt& θόÿÿÿU‰ΝWV‰ΦS‰Γƒμpd΅    ‰D$l‰ΨθόÿÿÿΉ   ‹€(  |$ΗD$   ‰$1ΐƒ=x  σ«‡΄  ‰ι‰ς‰Ψθ·ώÿÿ‰D$‰Α…ΐ…U  ‹<$‹   …Ϋ…]  ‹—`  …Ò…O  ‹‡Δ  …ΐ…A  ΅ΤI  9EΈΤI  EΑƒ=x  ‰‡θ  ‹E‰‡μ  ‹U‰—π  ‹M‰τ  ‡Π  ‹E‹]0ΗD$$   ‹<$‹u‹M‹U‰D$(‰D$‹E(ƒΗ‰t$ f‰D$0f‰D$‹E,‰L$,f‰D$2f‰D$·Γf‰\$4‹‰T$‰\$ƒλL;|$tYt& ‹Cd‹@…ΐt‹p…φtL$1Ò‰Ψθόÿÿÿ‹CLX΄9ΗuΨ‹D$(‹T$‹t$ ‹L$,‰D$·D$0f‰D$·D$2f‰D$·D$4‰U‹T$‰E0‹$‰U·T$‰u‰U(·T$‰M‰U,‹€τ  ‰E‹D$ld+    u‹D$ƒΔp[^_]ΓΗD$πÿÿÿλέθόÿÿÿ΄&    t& θόÿÿÿUWVS‰ΛθόÿÿÿΊΠ  Ή   ‹°(  ‰ΨθόÿÿÿΊ    i†Έ    ‹Έ    ‰ψθόÿÿÿƒψÿ„ω  k‰Αƒψ tƒΑƒω ‡ο  ‰ϊ‰θθόÿÿÿ‹–Μ   ‹‚΄   …ΐu‹‚   PC0hΨ  PθόÿÿÿΗCT …‹†Έ  ƒΔƒψ4tƒθ8ƒψwΗCT …1ΐ[^_]Γt& ƒΎ€  uδ[1ΐ^_]ΓθόÿÿÿW‰ΧV‰ΖS‹‹@P‰V‰ƒτώÿÿθόÿÿÿ‰ƒύÿÿ‰“ ύÿÿƒ=x  ‡  ³ύÿÿ‹‹C‰B‰‰πΊ   Η   ΗC"  [^_ιόÿÿÿ΄&    fθόÿÿÿWVS‰Γ΅x  …ΐ…+  ‹ƒϊwrs{1ΐΗC    ƒηό‰ρΗF,    )ωƒΑ0Αισ«‹<•ΐ   Ί    ‰ψθόÿÿÿ‰Αƒψÿ„Q  ƒψ tƒΑƒω ‡G  ‰ϊ‰πθόÿÿÿΗC$   1ΐ[^_Γ΄&    Έκÿÿÿλν΄&    fθόÿÿÿ‰ΘιTÿÿÿt& θόÿÿÿS‰Λθόÿÿÿ‹(  kl  i‚Έ    ‹„,   Ή   ƒθƒψv‹p  ‰΅x  …ΐ…[  ‰Ψ[ιϋώÿÿ΄&    t& θόÿÿÿU‰ΝWV‰ΦS‰Γƒμ΅x  …ΐ…x  θόÿÿÿ‰Β…ΐt]‰ΗΉΝ   Cσ¥‰‚¬  ƒ  Η‚      ‰‚0  iƒΈ    Uÿ°    ‚ΐ  hΰ  j ‰T$Pθόÿÿÿ‹T$ƒΔ‰(  ƒΔ‰Π[^_]Γ΄&    ΄&    θόÿÿÿV‹x  S‹X(‹³ψ  …Ò…”  ‹ƒά   @  ‹ƒβξ‰‹“d  ‹Nÿv‰Ψθόÿÿÿ‹ƒά   Ί   ‰0  Ηƒ       ‰ΨΊ   θόÿÿÿ‹ƒά      ‹ƒΚ‰‹ƒά      ‹ƒΚ ‰‹ƒά   @  ‹€@  ƒΘ‰X1ΐ[^Γt& θόÿÿÿS‰Λθόÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…°  1ΐ[Γv θόÿÿÿ‰Π‰Κ‹9Θt	ιμόÿÿt& Γ΄&    θόÿÿÿUWV‰ΖS‰ΛΉ   ƒμd‰$|$d΅    ‰D$`1ΐΗD$   σ«‹=x  ΗD$   …ÿ…6  ‹†|  ‹$3–x  1Ψ	Π„ÿ   ‹   …Ι…  ‹–`  …Ò…ϊ   ‹†Δ  …ΐ…μ   ‹$~‰|  ‰έΗ†μ  Π  ‰†x  % ω  Η†τ     ƒψΐƒΰ`ΰ  ‰†π  ‹FX΄9Η„‡   ‰t$v ‹Cd‹@…ΐt‹p…φt‹$‰ι‰Ψθόÿÿÿ‹CLX΄9ΗuΩ‹t$‹π  ‹–τ  ‹F‹¶μ  ‰L$‰t$X΄‰T$9Ηt-t& ‹Cd‹@…ΐt‹p…φtL$1Ò‰Ψθόÿÿÿ‹CLX΄9ΗuΨ1ΐ‹T$`d+    u8ƒΔd[^_]ΓΈπÿÿÿλδ‰Χ‰Π‰ΪθόÿÿÿPWh  h  h0  θόÿÿÿƒΔι΅ώÿÿθόÿÿÿt& θόÿÿÿWVS‹t$‹|$θόÿÿÿ‹(  ΅x  …ΐ…Ν  ‰ς‰ω‰Ψ[^_ιώÿÿ΄&    v θόÿÿÿS
  ƒμd‹    ‰\$1ΫT$Ί   f‰T$T$L$Ή   ‰T$‰βΗ$L   θόÿÿÿ‹T$d+    uƒΔ[Γθόÿÿÿθόÿÿÿƒμ 
  d‹    ‰L$1ΙT$T$Ή   ‰T$Ί   f‰T$T$f‰L$Ή   ‰T$‰βΖD$ Η$L   ΗD$L  θόÿÿÿƒψ…ι  ¶D$‹T$d+    uƒΔ Γθόÿÿÿ΄&    v θόÿÿÿWVS‰Γƒμd΅    ‰D$1ΐ΅x  …ΐ…ÿ  ƒϊ„©   ƒϊ…   Ί   ‰ΨθόÿÿÿƒΘƒΘ |$T$Ή   ³
  D$f‰L$‰πΉ   ΖD$ΗD$L   ‰|$θόÿÿÿΈ   Ή   f‰D$Έ   T$f‰D$‰πΗD$L   ‰|$θόÿÿÿ΅x  …ΐ…$  ‹D$d+    uƒΔ[^_Γt& Ί   ‰Ψθόÿÿÿƒΰχι[ÿÿÿθόÿÿÿ΄&    ΄&    θόÿÿÿU‹-x  WV‰ΦS‰Γ…ν…o  ‹“Έ  ‰³l  Bκ‰Ρƒψ†›   ƒϊ!„’   ‹ƒ”  …ΐ„Δ   ‹Hd‹y…ÿ„&  ‹?…ÿ„  kξ‹   iÒ  ‹”0   …Ι„^  ‹)…ν„T  j 1Ιθόÿÿÿ_‹“Έ  ƒϊ„‹   ƒϊ „‚   ƒϊvlBίƒψw
ΉIθ#£Αrk[^_]Γ¶    kώiΒ  ƒΌ,   …WÿÿÿΊ   ‰Ψθόÿÿÿ‹“Έ  ‹ƒ”  ‰Ρ…ΐ…Cÿÿÿ΄&    ƒϊ t}ƒϊtxƒϊw”ƒϊv΅Έ £Πs—f‹ƒ”  …ΐtV‹Hd‹I…ΙtL‹y…ÿtEkξ‹   iÒ  ‹”4   …Ιtc‹i…νt\j 1Ιθόÿÿÿ‹‹Έ  Zλfƒϊ tΈƒϊ… ÿÿÿλ­‰ΡkφΊ   iΙ  ‹„4   ƒψtƒψ…ÿÿÿΊ   ‰Ψ[^_]ι;ύÿÿv j 1Ιθόÿÿÿ‹‹Έ  XλΈ¶    j 1ΙθόÿÿÿYι§ώÿÿθόÿÿÿS‰Γƒμ΅x  …ΐ…―  kÒiƒΈ    ƒΌ4   tƒΔ‰ΨΊ   [ιΡόÿÿƒΔ‰ΨΊ   [ιΐόÿÿθόÿÿÿV‰ΞSθόÿÿÿ‹(  k“l  iƒΈ    ‹„,   ƒθƒψv,‹ƒϊw=΅x  …ΐ…Τ  ‰“p  ‰ΨƒΒθhόÿÿ1ΐ[^Γv ƒ>ΈκÿÿÿΊ    [DΒ^Γt& ΈκÿÿÿλΫθόÿÿÿUWV‰ΦS‰Γƒμ‹x  …Ι…σ  ‹>ƒÿ‡ύ   kΗi“Έ    ‹”,   …Ò„α   ΗF$   i“Έ    ‹„,   Ί    ‹,…@  F‰$‰θθόÿÿÿ‰Αƒψÿ„  ƒψ tƒΑƒω ‡  ‹$‰κθόÿÿÿΗF0χr kΧΗF4    iƒΈ    ‹„,   Ί   ƒθƒψwΗF$   Ί   ‰V(9»l  t1ΐƒΔ[^_]Γt& ‹CƒΓx΄9ΓtεƒΖ8‹Gd‹@…ΐt‹H$…Ιt	‰ς‰ψθόÿÿÿ‹GLx΄9ΓuάλΌΈκÿÿÿλ·v θόÿÿÿVS‰Λθόÿÿÿ‹°(  ΅x  …ΐ…#  ‰Ϊ‰π[^ι¤ώÿÿt& θόÿÿÿS‰Λθόÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…?  1ΐ[Γv θόÿÿÿVS‰Λθόÿÿÿ‹x  ‹°(  …Ò…\  ƒϋw9kΣi†Έ    ‹„,   …ΐt!‰Ϊ‰πθhϋÿÿ‰π‰ΪθOύÿÿ1ΐ[^Γ΄&    v Έκÿÿÿλμ΄&    fθόÿÿÿλ‰΄&    fθόÿÿÿUWVS‰Λƒμ$d‹    ‰T$ 1Òθόÿÿÿ‹°(  ‹†Έ  ƒψ:wƒψ#‡Ξ   Pμƒβϋ„β   ƒθ8ƒψ‡6  ‹…Ò…<  ‹CΊ		 n‰†ά  F\θόÿÿÿ‰Η…ΐ„A  θόÿÿÿ‰D$…ΐ„  ‹Fp΄9Ε„8  f‹Fd‹@…ΐt‹H…Ιt	‰Ϊ‰πθόÿÿÿ‹FLp΄9ΕuάΈd   θόÿÿÿ‹D$Η$    …ΐ…  ‹G‹@θόÿÿÿ1Ò‰ψθόÿÿÿ‹G‹@θόÿÿÿι]  v PάΉ q £Ρrƒψ4„4ÿÿÿι#ÿÿÿt& ‹K‹†x  ΗD$   ‹–|  ΗD$   ‰ά  ‰D$F\‰T$Ί		 ‰L$θόÿÿÿ‰D$Η$   …ΐtθόÿÿÿ‰$…ΐ„k  ‹=x  …ÿ…«  †,  Ί   θόÿÿÿ…ΐ„ν   ‹@‰D$‹†Έ  Pμƒβϋ„δ   ƒθ$ƒψwΊ q £Β‚Ξ   ‹D$…ΐ„Υ  ‹Τ  …Ι„Υ  ‹FnX΄9Εt?΄&    ‹Cd‹@…ΐt‹x…ÿt‹–x  ‹|  ‰Ψθόÿÿÿ‹CLX΄9ΕuÒ‹D$‹Τ  ‹D$T$θόÿÿÿΈd   θόÿÿÿ‹$…Ι„ώ   Η$    ‹D$ d+    …  ‹$ƒΔ$[^_]Γv ƒΎ€  …½ύÿÿv Η$κÿÿÿλΘ΄&    †ψ  ‰D$ι#ÿÿÿ΄&    f‹FΗD$   p΄9Ε…ΚύÿÿΈd   θόÿÿÿΗ$    λ€΄&    ‹|$‹G‹@θόÿÿÿΊ   ‰ψθόÿÿÿ‹G‹@θόÿÿÿιjώÿÿt& ‹G‹@θόÿÿÿΊ   ‰ψθόÿÿÿ‹G‹@θόÿÿÿ‹Fp΄9Ε…PύÿÿΈd   θόÿÿÿι‚ύÿÿ‹|$‹G‹@θόÿÿÿ1Ò‰ψθόÿÿÿ‹G‹@θόÿÿÿιαώÿÿθόÿÿÿv θόÿÿÿιfόÿÿ¶    θόÿÿÿWV‰ΖS‹€ά      ‹ …Β„΅   ‹†ά   ‰Σ‰$  χΒ  t+χΒ   …  φΗ…†   φΗtƒ=x  ‡λ  t& 1ÿφΓu‰Ϊ‰πθόÿÿÿ[^ψ_Γ΄&    v ΎΈ  ‰ψθόÿÿÿ‹†ά   ‹   ‹–ψ  †ψ  9Πt‰Κθΰοÿÿ‰ψΏ   θόÿÿÿλ©f[1ΐ^_Γ΄&    v ƒ=x  ‡L  1ÿφΓtƒλ΄&    v θόÿÿÿ‹x  S‰Γ…Ò…h  ‰ΨΊ   θόÿÿÿ‹ƒδ  …ΐt‹π  ƒβuYθόÿÿÿΗƒδ      ‹ƒΰ  …ΐt‹π  ƒβu%θόÿÿÿΗƒΰ      ‹ƒ0  …ΐt,‰Ψ[ιόÿÿÿt& θόÿÿÿλΩ΄&    fθόÿÿÿλ¥΄&    f[Γ΄&    ΄&    θόÿÿÿUWVS‰Γƒμ@‹x  d΅    ‰D$<1ΐ…Ι…@  Ώ@  Ύ F  ΉΝ   Ί  σ¥Έ   ‰ΝΉ   θόÿÿÿΈΤI  Ί   =ΤI  YUYVΗƒx     EΕΗƒ|      ‰ƒθ  ƒψ  ‰ƒψ  ‰ƒό  ƒX  ‰ƒX  ‰ƒ\  ‰ΨΗƒτ     Ηƒμ  Π  Ηƒπ  ΰ  θόÿÿÿƒ»€  t‹ƒ  ƒψ†ÿ  ³  »  ‰πθόÿÿÿ‹‹|  ‹“x  ‰Ψθόÿÿÿ1Ò‰Ψθΰτÿÿ1Ò‰ΨθΗφÿÿ‰πθόÿÿÿ‹ƒΜ   Ηƒ     Ηƒ        ‰›,  ‰ƒ  ‰ψΗƒ<     Ηƒ@     Ηƒ4     Ηƒ      Ηƒ$      Ηƒ8      ‰³  θόÿÿÿ‰Β…ΐ  ‹ƒΜ   ‰›  «d  Ηƒd        ‰³x  ‰ƒl  ‰θΗƒh     Ηƒ     Ηƒ      Ηƒ”     Ηƒ€      Ηƒ„      Ηƒ      θόÿÿÿ‰Β…ΐ–   ΉK  Ί F  ‰Ψθ=ξÿÿ‰ƒΰ  ‰ΈΈ  ‹ƒΰ  Η€δ    ‹ƒΈ  ƒψ4tƒθ8ƒψ‡  ‹ƒΰ  δ      ‹“  ‹ƒΰ  ³Ό  ‹• J  ‹ΰ   ÿ21Òjθόÿÿÿ‰ΒXY…Ò‰„  ι\  ΄&    ‰Ψ‰$θόÿÿÿ‹$‹D$<d+    …†  ƒΔ@‰Π[^_]Γ¶“„  s„Ò…^  Έ   θόÿÿÿΉ(  i“    P‰πj ”ό   θόÿÿÿ_]‰Ζ…φ„Ήύÿÿ‹ƒ€  ΗD$    ΗD$    ‰D$‰πΗD$    ΗD$   θόÿÿÿf‰D$‹FdΗD$    ‹@…ΐt*‹H$…Ιt#΅   T$…ΐ„   ‹x$…ÿ„•  ‰πθόÿÿÿ‹ƒΈ  ƒψ„ο   ƒψ*„ζ   ƒψ'…&ύÿÿ‹ƒ€  ΗD$(    ΗD$,    ‰D$D$ ΗD$0    ΗD$4    ΗD$8    ΗD$ <  ΗD$$@   ‰D$‹Fd‹@…ΐ„Ξόÿÿ‹H(…Ι„Γόÿÿ΅   T$…ΐ„!  ‹x(…ÿ„  ‰πθόÿÿÿι›όÿÿ΄&    v iΐ  j Ή(  R„ό   ‰Β‰πθόÿÿÿ‰ΖXZι©ώÿÿ΄&    fƒ»€  …Ωύÿÿιδύÿÿ¶    ‹“€  ΗD$(    ΗD$,    ‰T$T$ ΗD$0    ΗD$4    ΗD$8    ΗD$ .  ΗD$$@   ‰T$‹Vd‹R…Ò„Βώÿÿ‹J(…Ι„·ώÿÿ΅   T$…ΐt0‹x(…ÿt)‰πθόÿÿÿ‹ƒΈ  ι‘ώÿÿ΄&    ‰πθόÿÿÿιfώÿÿt& ‰πθόÿÿÿ‹ƒΈ  ιhώÿÿ¶    ‰πθόÿÿÿι…ϋÿÿθόÿÿÿfffffffθόÿÿÿ‹@(‹T$‹€x  Η   % ω  ƒψΐ%€C   ‡  ‰1ΐΓ΄&    ΄&    θόÿÿÿUWVS‰Γ«x  ƒμ‹ ‹“  ‹p(‹ƒ  ƒΐX  ΎΈ  ‰B‹ƒ  Η   p‹ƒ  ‹“  ƒΐ‰B‹ƒ  Η@    ‹†X  9Αtt‹ƒ  ‰L$   ‹†\  ‰$‰ψθόÿÿÿ‹L$‰Β‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰ψθόÿÿÿ‹$‹“  ƒ=|  ‹@‰P‡ύ  ƒΔ[^_]Γ΄&    f‰ψ‰$θόÿÿÿ‹$‰Β‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰ψθόÿÿÿƒ=|  ‡β  ƒΔ[^_]Γ΄&    t& θόÿÿÿUWVS‹h(‹…ά   @  ‹ƒβέ‰½Έ  X  ‰ψθόÿÿÿ‰Ζ‹…X  9Γt@v ‹…X  ‹P‹-x  ‰Q‰
Ί   Η€x     Η€|  "  θόÿÿÿ‹…X  9ΨuΓ[‰ς‰ψ^_]ιόÿÿÿ¶    θόÿÿÿ‰Β‹ ‹@(ιόÿÿÿ΄&    ΄&    θόÿÿÿW1ÒVS‰Γ‹ ‹p(‰Ψθόÿÿÿ‹S‰Α‹†x  % ω  ƒψΐƒΰƒΐ…Òu	[Έκÿÿÿ^_Γiψ@  ‹ST9ϊrκGΧ‰SP“„  ‹Μ   Piΐ   j h   P‰Ψj ‹	θόÿÿÿ1ΐƒΔ[^_Γ΄&    ΄&    θόÿÿÿV‹|  S‹X(‹³X  …Ò…  ‹ƒd  ÿvΉ   P,‰Ψθόÿÿÿ‹ƒά   Ί   ‰  ‹ƒά   ‰4  Ηƒ`      ‰ΨΊ   θόÿÿÿ‹ƒά      ‹ƒΚ"‰‹ƒά      ‹ƒΚ ‰‹ƒά   @  ‹€@  ƒΘ"‰X1ΐ[^Γ¶    θόÿÿÿS‰Λθόÿÿÿ‹€(  ΗCΐό›ΗC   ΗCGREYΗC    ΗC$    ‹€x  φΔωt'ΗC
   ΗC  ΗC   ΗC    1ΐ[Γ΄&    ©ÿÿ tξΗC   1ΐΗC?  ΗC   ΗC    [ΓθόÿÿÿƒβtFVS‰Γ΅|  …ΐ…4  ³Έ  ‰πθόÿÿÿ‹ƒά   ‹$  “X  ‰Ψθόÿÿÿ‰πθόÿÿÿΈ   [^Γ1ΐΓffffffθόÿÿÿV°θ   S‰Γ‰πθόÿÿÿ‰Β‹ƒά   ‹  ‰πθόÿÿÿ‰Ψ[^ΓθόÿÿÿUWV‰ΞS‰Σƒμ€|$4 ‹T$ tΗ    pƒΐΗ@ψ    Η@ό    ‰ΗƒϊÿtΚ € €ƒΗ‰‹T$,…Ò„­   ‹D$0Η$    …ΐ•D$¶    ‹K‹l$$…φt-‰Ψλ΄&    f)Ξθόÿÿÿ…φ„  ‹H9ρvκ‹l$$‰Γ)ρυ‹$…ΐti€|$ tb1Òχt$0ƒϊWΐ%     ;L$$rRD$$   ‰s‰wΗG    ‰Χ‹t$(ƒ$‹$ξ9D$,…kÿÿÿƒΔ‰ψ[^_]Γ΄&    fΈ   W;L$$s®	Θ‰T$   ‰s‰Ψ‰wΗG    +kθόÿÿÿ‹T$‰Γ‹@9Εv5‰Α‰Ψ‰Σv Ι   ƒΓ‰Kτ‹P‰SψΗCό    +hθόÿÿÿ‹H9ιrΨ‰Ϊ‰Γ‰θz   ‰‹CΗB    ‰BιDÿÿÿ¶    ‹H‹l$$‰Γικώÿÿ΄&    v θόÿÿÿWVS‹Μ   ‰Γ‹‘\  ‹X  ‹±P  ‹ΉT  ‰Ρ	Αt
ƒΐƒÒ )πϊR‰ω‰ςPΈ    θόÿÿÿπÿ”ΐZY„ΐt|‹ƒΈ  iΠ  ƒΊ   tiΠ  ‹’   ƒϊ„‚   ƒϊ„   iΐ  ‹€   ƒψ„¤   ƒψt7ƒ
  θόÿÿÿƒ|  θόÿÿÿƒτ   θόÿÿÿ‹ƒά   [^_ιόÿÿÿ[^_Γt& ‰Ψθόÿÿÿλΐ΄&    ‰Ψθόÿÿÿ‹ƒΈ  ιoÿÿÿ¶    ƒά  θόÿÿÿ‹ƒΈ  iΠ  ‹’   ƒϊ…aÿÿÿ‰Ψθόÿÿÿ‹ƒΈ  ιOÿÿÿ΄&    ƒ   θόÿÿÿiƒΈ    ‹€   ι<ÿÿÿθόÿÿÿ…ΐt7S‹X`ϊ v@tϊv@t[Γ9ƒ¤  uυ‹[ιόÿÿÿ9ƒ¤  uε‹[ιόÿÿÿΓ΄&    ΄&    θόÿÿÿφ€4  t*S‹ά   ‹‹  ‹ά   ‹“”  …Ιt…Òu
[Γ΄&    Γ‹ά   ‰‹  ‹ά   ‰“”  ‹ά   Γ  ‹‹ά   Γ ‹‹ά   Γ  ‹‹ά   Γ ‹RQhθ
  ‹€Μ      PθόÿÿÿƒΔ[Γ΄&    v θόÿÿÿUWΏ   V‰ΖSƒμ‰$λM΄&    ‹„  …Ò…Τ  ‹‹Cƒο‰B‰Ί   ‰θΗ   ΗC"  θόÿÿÿ·V‹$)Π…ΐ~O…ÿ~K‹9πtE‹θόÿÿÿ‰“ ύÿÿ«ύÿÿ‰ƒύÿÿ‹FP‰V‰ƒτώÿÿ‹F·Π;$u‚ƒ=„  v‡ιχ  t& ƒΔ[^_]Γ΄&    θόÿÿÿWV1φS‰Γ‹€ά   ‰°   ‹ƒά   ‰°   ‹ƒά   ‰°@  ‹ƒά   ‰°@ ‹ƒά   ‰°@ ‹ƒά   ‰°@  ‹ƒά   ‰°@ ‹ƒά   ‰°   »θ   ‰ψθόÿÿÿ‰Β‹ƒά     ‹‰1‰ψθόÿÿÿ‹ƒά   ‰°   ‹ƒά   ‰°0  ‹ƒά   ‰°@  ‹ƒά   ‰°P  ‹ƒά   ‰°`  [^_Γ΄&    ΄&    θόÿÿÿWV‰ΖS‹ΈΘ   _ό‰Ψθόÿÿÿ‰Ψθόÿÿÿ‰Ψθψώÿÿ‹†L  ‰Ϊθόÿÿÿ‰πθόÿÿÿ‰Ψθ½ϋÿÿGXθόÿÿÿ‰ψθόÿÿÿ‰Ψ[^_ιόÿÿÿt& θόÿÿÿWΈθ   V‰ΦS‰Γ‰ψθόÿÿÿ	³δ   ‰Β‹ƒά     ‹€  	π‰[‰ψ^_ιόÿÿÿ΄&    ΄&    θόÿÿÿWΈθ   V‰Ζ‰ψS‰Σθόÿÿÿ‰Β#δ   t‹†ά     ‹€  	Ψ‰[‰ψ^_ιόÿÿÿ΄&    ¶    θόÿÿÿWΈθ   V‰Ζ‰ψS‰Σθόÿÿÿ‰Β‹†ά     ‹€  χΣ!Γ‰[‰ψ^_ιόÿÿÿv θόÿÿÿWΈθ   V‰Ζ‰ψS‰Σθόÿÿÿ‹ά   ‰Β‰ΨχΠ™  !†δ   ‹‰  !Θ‰[‰ψ^_ιόÿÿÿ΄&    t& θόÿÿÿUW‰ΧVS‰Γƒμ‹r΅„  …φ„  …ΐ…8	  ‹GƒΑ1Òƒαψ‰D$‹G‰L$‰$χρΊ   9ΠFΠ‰T$ƒψ†>  ‹ƒά   Ί  p‰PΊ   ‹ƒά   ‰P1Ò‹ƒά   ‰P1νΗD$    ‹t$‹L$9$ro‰|$1Ιfƒ=„  ‡]	  ‰πƒΰόƒά   ‰$‹D$‹x‹$ο‰8Fƒΰόƒά   ‰Fƒΰόƒά   ‰Fƒΰόƒά   ‰ƒΑƒΖl$9L$w‹|$‹G‹W(ƒΰόƒά   …Ò…  ‹T$(‰‹G1Òƒΐƒΰόƒά   ‰‹G‹L$ƒΐƒΰόƒά   ‰‹G‹t$ƒΐΑζƒΰόΑξƒά   ‰0‹G‹Wƒΐƒΰόƒά   ‰‹G(…ΐ‹G„χ   ƒΐΊ  €ƒΰόƒά   ‰Ί   1Ιt& ‹GΠƒΰόƒά   ‰ƒΒƒϊPuθ‹G‹Wƒΰόƒά   ‰‹G‹L$ƒΰόƒά   ‰‹G$ƒΰόƒά   ‰0‹T$‹G Ακƒΰόƒά   ƒκ‰ƒ=„  ‡	  ƒΔ1ΐ[^_]ΓΊ   ‰ιψώÿÿt& …ΐ…	  ‹G1Òƒΰόƒά   ‰‹Gƒΰόƒά   ‰‹G$ƒΰόƒά   ‰‹G ƒΰόƒά   ‰ƒΔ1ΐ[^_]Γt& ƒΐΊ   ƒΰόƒά   ‰ιÿÿÿ¶    θόÿÿÿUWV‰ΖS‰Σβ  ‹8…€  φΓt`ƒ=„  ‡k  ®΄   ‰θθόÿÿÿ‹†Έ   ƒΰό‡ά   ‹†   θΪωÿÿ‰θθόÿÿÿ‹†θ   ƒΰό‡ά   ‰Έ   [^_]Γ΄&    …ΫuΩ[1ΐ^_]Γt& θόÿÿÿUhW‰ΧV‰Ζ‰θSθόÿÿÿ‹FV9Βt/X΄λ΄&    v ‹CLX΄9Βt9»   uξ‰θθόÿÿÿ‰Ψ[^_]Γ‰θ1Ϋθόÿÿÿ‰Ψ[^_]ΓθόÿÿÿUWV‰Ξ1ΙS‹l$‰Σ‹|$$ƒύÿ•Α1Òƒ|$ÿ•Β   Ρ‹T$T$ j ―Χhΐ  ΑκT―ΡKTRΑβ‰θόÿÿÿZY‰C…ΐtnƒύÿtj‰ι‰ςj Wÿt$,ÿt$,j θτÿÿƒΔƒ|$ÿt'1Òƒύÿ”ΒR‰ςj Wÿt$,ÿt$,h   ‹L$0θqτÿÿƒΔ‰C+C1Òƒΐ;w[‰Π^_]Γ΄&    Ίτÿÿÿλθ΄&    θόÿÿÿU   WV‰ΞS‰Σ‹|$‹T$‹l$j Khΐ  ―ΧΑκTRΑβ‰θόÿÿÿZY‰C…ΐt:1Ò…ν”Β1ΙR‰ςUWj ÿt$$jÿθέσÿÿ1Ò‰C+CƒΔƒΐ;w[‰Π^_]Γt& Ίτÿÿÿλμ΄&    θόÿÿÿUWV‰Ξ1ΙS‹l$‰Σ‹|$$ƒύÿ•Α1Òƒ|$ÿ•Β   Ρ‹T$T$ j ―Χhΐ  ΑκT―ΡKTRΑβ‰θόÿÿÿZY‰C…ΐtnƒύÿtj‰ι‰ςj Wÿt$,ÿt$,j θ/σÿÿƒΔƒ|$ÿt'1Òƒύÿ”ΒR‰ςj Wÿt$,ÿt$,h   ‹L$0θσÿÿƒΔ‰C+C1Òƒΐ;w[‰Π^_]Γ΄&    Ίτÿÿÿλθ΄&    θόÿÿÿS‹„  ‹  ‹€Μ   j ÿ²”  ÿ²     ‰ΪθόÿÿÿƒΔ[Γ΄&    fθόÿÿÿU‰ΥW‰ΟV‰ΖSƒμ‹„  ‹…Ι…q  ‰ΨθΧυÿÿ‹†ΐ   ƒΰόƒά   ‹‹–τ   χÒ!Κ‰kV,‹¤   ‰Ψ“d  ÿ·  θόÿÿÿƒ=„  Z‡²  ‹†Δ   ‹–¤   ƒΰόƒά   ‰iƒΈ        ‹PφΒu‹@¨„  ƒϊ„  ΈΌ θόÿÿÿ‹†μ   …ΐtƒΰό‹–  ƒά   ‰‹†Θ   ‹–  ƒΰόƒά   ‰‹†ΰ   ‹–   ƒΰόƒά   ‰‹†ά   ‹–  ƒΰόƒά   ‰‹†Μ   ‹–ό   ƒΰόƒά   ‰ΈΌ θόÿÿÿ‹†Ό   Ί   ƒΰόƒά   ‰ΗE    ‹ƒΈ  iΠ  ƒΊ   u"‹ƒά   L  ‹ ƒΰό‹»ά   ‰‡L  ‹ƒΈ  iΠ  ƒΊ   u"‹ƒά   L  ‹ ƒΰϋ‹»ά   ‰‡L  ‹ƒΈ  iΐ  ƒΈ   uE‹ƒά   L  ‹ ƒΰώ‹»ά   ƒΘ
‰‡L  ‹ƒά   H  ‹Κ  €‰ΊE ‹ƒά   ‰,  ‹ƒh  =u  t-w  ƒψ‡,  ΅„  …ΐ…–  ‰ΨθΊσÿÿ‹†δ   ƒΰόƒά   ‹–ψ   ‰‹†ΐ   ƒΰόƒά   ‹–τ   ‰»θ   ‰Ψθ{σÿÿ‰ψ‹®π   θόÿÿÿ	«δ   ‰Β‰ψθόÿÿÿ‰ψθόÿÿÿ‰Β‹ƒδ   …ΐut‰ψθόÿÿÿ‰Ψθ>σÿÿ‹ƒά      ‹ƒΚ ‰‰Ψθ%σÿÿiƒΈ    ƒΈ   tbƒ=„  wi‰ψθόÿÿÿ‰Β‹ƒά     ‹ ‰ψθόÿÿÿ‰Ψθεςÿÿ1ΐƒΔ[^_]Γv ‹‹ά   Α  ‹)	θ‰ιuÿÿÿ1Ò‰ΨθόÿÿÿιeύÿÿfΊ   ‰ΨθόÿÿÿλιΚ  ΄&    t& θόÿÿÿUWV‰ΦS‰Γƒμ‹Ί¤   ‹*―Ί¨   1Òθόÿÿÿ‹„  ‰Β…Ι…!  ‹C…ΐuCΈκÿÿÿ…ÿu(‹…Μ   j Γ„  ÿ¶¨   ÿ¶¤   ‹
‰Ϊθόÿÿÿ1ΐƒΔƒΔ[^_]Γ΄&    v ;{Tw‰{PλΌΈκÿÿÿλέ΄&    ΄&    θόÿÿÿUWV‰ΖS‰Σƒμ‹8‹ƒ  ‹’  ƒΐΗΈ  ‰B‹ƒ  Η   p‹ƒ  ‹“  ƒΐ‰B‹ƒ  Η@    ‰ψθόÿÿÿ‹®      ‰Βƒx  9ιtT‹«  M    ‹®   ‰†   ‰‹x  ‰«|  ‰E ‹E‹‹  ‰H΅„  …ΐ…l  ƒΔ‰ψ[^_]ιόÿÿÿt& ‹®   ‰†   ‰‹x  ‰«|  ‰E ‹„  …Ι…E  ƒΔ‰ψ[^_]ιόÿÿÿθόÿÿÿUW‰ΗVSƒμ‹„  …Ò…“  ‹0‹‡δ   ƒΰό†ά   ‹‹—ψ   χÒ!Κ‰‹‡ΐ   ƒΰό†ά   ‹‹—τ   χÒ!Κ‰»   v ΈX‰A θόÿÿÿƒλuρ»d   λ΄&    v …ΐt,ΈX‰A θόÿÿÿƒλt‹†ά   ‹  ‹†ά   ”  ‹ …ÒuΠi†Έ    ƒΈ   t}‡΄      ‰$θόÿÿÿ‰D$λ?v ‹·   ‹‹F®ύÿÿ‰B‰‰θΊ   Η   ΗF"  θόÿÿÿ΅„  …ΐ…Υ  ‹‡   9ΨuΊ‹T$‹$ƒΔ[^_]ιόÿÿÿ΄&    ‹†ά   L  ‹ ƒΰυ‹ά   ƒΘ‰L  ‹‡μ   1Òƒΰό†ά   ‰‹‡Μ   Ί   ƒΰό†ά   ‰i†Έ    ƒΈ   …$ÿÿÿ1Ò‰πθόÿÿÿιÿÿÿv θόÿÿÿ…Ò„ƒ   UWV‰ΦS‰Γ‹€”  ƒΰόƒά   ‹8ƒ=„  wdχΖ 3 …ϋ  χΖ   t.ƒ=„  ‡G  «  ‰θθόÿÿÿƒt  ‰ϊθ†οÿÿ‰θθόÿÿÿ‹ƒΔ  ƒΰόƒά   ‰0Έ   [^_]Γt& 1ΐΓ‹ƒΐ  ƒΰόƒά   ‹ WPRhΤ  hD  θόÿÿÿƒΔιqÿÿÿfθόÿÿÿUWVS‰Σƒμ<d΅    ‰D$8‹‚ά   ‹¨  Ίθ   ‰ψθόÿÿÿ‰Β‹ƒά   ‹°  ‰ψθόÿÿÿ‰θ!π‰$„%  ‹ƒά   $  ‹ ‰D$‹ƒά      ‹ ‰D$‹ƒά   T  ‹ ‰D$‹ƒά   P  ‹ ‰D$‹ƒά   ‹4  ‹ƒά   0  ‹ ‰D$‹ƒά   D  ‹ ‰D$‹ƒά   @  ‹ ‰D$‹ƒά   ‹   ‹ƒά      ‹ ‰D$ ‹ƒ”  ƒΰόƒά   ‹ ‰D$$‹ƒΨ  ƒΰόƒά   ‹ ƒ=„  ‰D$(‡x  ‰θ%   ‰D$χΕ€„ς   χΕ   tƒ=„  ‡K  χΕ   tƒ=„  ‡o  χΕ   tƒ=„  ‡  χΕ   tƒ=„  ‡'  χΕ   tƒ=„  ‡ί  χΕ   …Ϊ  χΕ   tƒ=„  ‡>  χΕ   „Ò  ƒ=„  ‡  χΕ   „  ƒ=„  ‡ό  χΕ  € uχΕ   tƒ=„  ‡Ψ  χΕ   „ϊ  ƒ=„  ‡΄  iƒΈ    1φ‹€  ƒψ„2  ƒψ„a  …Ιt"iƒΈ    ‹€   ƒψ„Μ  ƒψ„S  ‹D$…ΐt"iƒΈ    ‹€   ƒψ„Ί  ƒψ„9  ‹L$…Ι…-  ‹T$…Ò…‘  ‹D$…ΐt[‹ƒ¨  ΖD$7 …ΐtL‹Pd‹…ÒtC‹J$‰L$…Ιt8‹    …Ò„x  ‹J$‰L$…Ι„i  T$7‰Ρ‰κ‹l$θόÿÿÿ€|$7 tƒΖχ$   …Ρ   …φu-Η$    ‹D$8d+    …9  ‹$ƒΔ<[^_]Γ΄&    v ‹ƒά   ‹<$‰Έ  Η$   λΒ΄&    χΕ  €„Λώÿÿ‰κ‰Ψ‰L$θόÿÿÿ‹L$‰ΖiƒΈ    ‹€  ιώÿÿt& χΕ  € „“ώÿÿ‰Ψ‰L$θόÿÿÿ‹L$Ζι}ώÿÿ¶    ‹T$‰Ψθόÿÿÿ‹T$Ζ…Ò„ΖώÿÿλUt& ‰ψθόÿÿÿ‰Β‹ƒά     ‹αÿÿÿχ‰‰ψƒΖθόÿÿÿ‹  Έ    ‹    θόÿÿÿ…φ„νώÿÿιÿÿÿt& ‹L$‹T$‰ΨθόÿÿÿΖ‹D$…ΐ„µώÿÿιUώÿÿ¶    ƒ=„  †µώÿÿι_  ¶    ƒ=„  ‡“  χΕ   ….ύÿÿχΕ   „/ύÿÿƒ=„  †"ύÿÿι  ΄&    f‰Κ‰ΨθόÿÿÿΖιύÿÿ‹T$‰ΨθόÿÿÿΖιµύÿÿ¶    χΕ  € „ύÿÿƒ=„  †	ύÿÿι»  ΄&    v ‹D$…ΐ„ύÿÿƒ=„  †ϊόÿÿιv  fƒά  ‰Κθ³οÿÿΖι)ύÿÿt& ‹T$ƒ   θ™οÿÿΖι9ύÿÿf‰κL$7‹l$θόÿÿÿι”ύÿÿθόÿÿÿ΄&    θόÿÿÿV‰Ζ‰ΠS‰Σƒΰt‹ά   ‘  ‹
	Θ‰χΓψÿ t>i†Έ    ƒΈ   „_  ƒΈ   „_  ‹†ά      ‹
‰ΨΑθ·ΐ	Θ‰γ  ψ …w  [^Γ΄&    θόÿÿÿV‰Ζ‰ΠS‰Σƒΰt‹ά   ‘  ‹
χΠ!Θ‰χΓψÿ t@i†Έ    ƒΈ   „  ƒΈ   „  ‹†ά      ‹
‰ΨΑθ·ΐχΠ!Θ‰γ  ψ …§  [^Γv θόÿÿÿV‰ΖS‰ΣƒβuhχΓψÿ u‰Ψ%  ψ …Ώ  [^Γ΄&    i€Έ    ƒΈ   „Ω  ƒΈ   „Ω  ‹†ά   ‹   ‰Ψ[^Αθ!ΠΑΰ%ψÿ Γ¶    ‹€ά   ‹€  Αθ!Ψ[^ƒΰΓθόÿÿÿW‰ΟV‰ΖS‰Σƒβt ‹€ά     ‰ΨΑΰ%   …Ιts‹
	Θ‰χΓψÿ tFi†Έ    ƒΈ   „ρ  ƒΈ   „ρ  ‹–ά   Αλ·Ϋ‚$  …ÿt‹’$  χΣ!Σ‰[^_Γv ‹’$  	Σ‰[^_Γ΄&    v ‹
χΠ!Θ‰λ‰¶    θόÿÿÿUΉ8  Ίΐ  W‰ΗVSƒμ΅4   θόÿÿÿ…ΐ„  ‰Γ΅`N  …ΐ„Η   Ί   ƒψtM1ΙΈ"  ΊQ  1φθόÿÿÿ…ΐu%ƒΖƒώ„   ‹υ	  ‹υ 	  1Ιθόÿÿÿ…ΐtΫθόÿÿÿΊ   ΄&    ¶ƒ4  kƒΰώ	Π‰κƒ4  ‡   ‰D$θόÿÿÿ‰Ζ…ΐx/j C\1ΙΊ   ‰D$θόÿÿÿ‹³   X…φt,‹$θόÿÿÿ‰θθόÿÿÿ‰ΨθόÿÿÿƒΔ‰π[^_]Γt& 1Òλt& ‹$‰»Μ   ΎϋÿÿÿΗƒ  ΰÿÿÿ‰C@ƒ  ‰ƒ  ‰ƒ   ƒ°  ‰ƒ°  ‰ƒ΄  ƒΔ  ‰ƒΔ  ‰ƒΘ  ‰ψΗƒ¤      Ηƒ¬  ΰÿÿÿΗƒΈ      Ηƒΐ  ΰÿÿÿΗƒΜ      ΗC< E  θόÿÿÿ‰D$…ΐ…7ÿÿÿƒ  Ή€  Ί°  Ηƒθ       ΗƒΈ      θόÿÿÿƒ¤  Ή€  Ί»  θόÿÿÿπÿ΅€  ‹Ό  ‰L$‰ƒ  PPhΛ  Q‰€  θόÿÿÿ‹ƒΜ   ƒΔ·@"f=€„§  f=RtΈu  Ή ?«Ί L  ‰‹π   ‹„  ‰ƒh  ‰“d  …Ι…'  ‹ƒ  ‹…@N  ;    ‚(  ΗƒΈ  ÿÿÿÿ‹    ‹t$λf‹“Έ  ƒΖƒϊÿuR9Ξt1‹ƒΜ   ·υ    f9P$uΪ·υ   f9P&uΜ‹υ   ‰“Έ  λΓΗƒΈ      ‰Ψθόÿÿÿ‹ƒΜ   ‹“Έ  fx"R„Ω  iΚ  ‹‰(   …Ιt‰‹π   ƒϊ+„  ‹P³θ   ¶Rt‰“Τ   ‹@Αθƒΰ‰ƒΨ   ‰πθόÿÿÿ‹δ      ‰Β‰πθόÿÿÿ‹ƒΈ  Ηƒψ       ‰›τ   iΠ  ‰›|  Ηƒl    Ηƒh    Ηƒp     Ηƒt    Ηƒx    Ηƒd     Ηƒ€     Ηƒτ	    Ηƒπ	    Ηƒψ	     Ηƒό	    Ηƒ 
    Ηƒμ	     Ηƒ
     ‰›
  Ηƒ|    Ηƒx    Ηƒ€     Ηƒ„    Ηƒ    Ηƒt     ‹’   ƒβύƒϊ„ό  iΐ  ‹€   ƒΰύƒψ„™  ‹‹Μ   ‹‘\  ‹X  ‰T$‹‘T  ‰D$‹P  ‰T$‹T$‰D$‹D$‰Ρ	ΑtƒΐƒÒ +D$T$‰D$‰T$j Έ    ÿt$ÿt$ÿt$‹T$$‹L$(θόÿÿÿƒΔ…ΐ„  ‹³Μ   ‹T  ‹†P  ‹–\  ‰L$‹X  ƒΑ)Α–X  Ύ    ‹T$DΞθόÿÿÿ‹“  Ή•  ‰ƒά   ‰ƒΰ   ‹ƒΈ  9•@N  Ί£  EΚ‹“Μ   QPiΐ  ÿ°    ·B&P·B$Pÿt$$hP  θόÿÿÿ΅„  ƒΔ…ΐ„[  ιD  ΄&    ‰“Έ  ƒϊÿ„Σόÿÿ‹ƒΜ   ι6ύÿÿfx&7q…UύÿÿΗƒπ   @x}ιFύÿÿƒϊ9„Ι  ƒϊ8…ύÿÿΗƒΈ  <   ‹Ψ@  …Ò„ύÿÿ‰“π   ιύÿÿ΅„  …ΐ…	  ƒΈ  ‰›   Ή€  Ίυ  ‰ƒΈ  ‰ƒΌ  ƒ4  Ηƒ     Ηƒ    Ηƒ(      Ηƒ,  ΐG ΗƒΤ      Ηƒ$     θόÿÿÿƒ,  Ηƒ°      ‰ƒ,  ‰ƒ0  ‹ƒ0  …ΐu
Ηƒ0     ΗƒΨ    Ηƒά  0 Ηƒΰ  @ Ηƒδ  P Ηƒθ  T Ηƒμ  X Ηƒπ  \ Ηƒτ  ` Ηƒψ  d Ηƒό  h Ηƒ   l Ηƒ      Ηƒ  @  Ηƒ  D  Ηƒ(     Ηƒ     ιύÿÿ‹„  …Ò…Q  ƒt  ‰›ά  Ή€  Ίυ  ‰ƒt  ‰ƒx  ƒπ  ΗƒΠ     ΗƒΤ    Ηƒδ      Ηƒθ  ΐG Ηƒ      Ηƒΰ     θόÿÿÿƒθ  Ηƒl      ‰ƒθ  ‰ƒμ  ‹ƒμ  …ΐu
Ηƒμ     Ηƒ”    ‹ƒΈ  Ηƒ  4 Ηƒ  @ Ηƒ   P Ηƒ¤  T Ηƒ¨  X Ηƒ¬  \ Ηƒ°  ` Ηƒ΄  d ΗƒΈ  h ΗƒΌ  l ΗƒΘ  D Ηƒΐ  0  ΗƒΔ  4  Ηƒδ     ΗƒΜ     ι²ϋÿÿΗƒΈ  =   ‹μA  …Ò„\ϊÿÿι;ύÿÿΎτÿÿÿι$ψÿÿΈx  Ή€πϊΊ K  ι]ωÿÿfffffffθόÿÿÿΈ	 ÿΓt& θόÿÿÿV‰ΖS»@   λΈΰ θόÿÿÿƒλt)‹F‹‹€x  ƒΰό‚ά   ‹ ¨uΧΈ   [^Γ΄&    1ΐ[^Γv θόÿÿÿU‰ΥWVSƒμ‹x‰D$΅  ‹…ΐt	φΑ„"  ·Ef…ΐ„Β   ‹  …Ϋ…κ!  ‰λ1φ‰Υ‰Ϊλ1f‹‡€  ƒΰό…ά   ‹‹B0΅  …ΐ…o"  ·BƒΖ9πvm‹p  ƒθ‰$‰ΩΛ  Ι  9π‹‡|  FΩ·
ƒΰό…ά   Αα‰‹‡t  ƒΰό…ά   ‰‹D$θαώÿÿ‹$…ΐ…vÿÿÿ΅  …ΐ…   ΈϋÿÿÿƒΔ[^_]Γt& ‹‡|  ·M ƒΰόΑα‚ά   ‰‹‡t  ƒΰό‚ά   ‹—p  ƒΚ‰‹D$θ}ώÿÿ…ΐt°‹D$‹@‹‹€x  ƒΰό‚ά   ‹ ¨t‹5  …φ…Μ!  ƒΔ1ΐ[^_]ΓΈϊÿÿÿιuÿÿÿιΏ"  t& θόÿÿÿU‰ΥW‰ΟVSƒμ‹p‰D$΅  ‰L$‹·M…ÿ„b  …ΐ…#  ‹|  ·E ‹Ίά   ƒγόΑΰίf…Ι„‰  ‹]¶	Ψ‰D$‹†p  fƒω„-    ‰$‹D$‰‹†„  ƒΰό‚ά   ‰‹†t  ‹$‰T$ƒΰό‚ά   ‰‹D$θ}ύÿÿ…ΐ„  ‹  …Ò‹T$…a#  ·Eƒψω   ‰ιΏ   ‰Υ‰Κλv΄&    fΛ  ‹†|  ‰$‹T$ƒΰό…ά   ‰‹†„  ƒΰό…ά   ‰‹†t  ƒΰό…ά   ‰‹D$θχόÿÿ…ΐ„   ‹  ‹$…Ι…¤#  ·BƒΗ9ψ~v‹Jƒθ‹p  ¶99ψ{ÿÿÿ‰ΨΛ    ƒ|$ DΨιgÿÿÿ…ΐ„ώÿÿι9#  v ‰Α  Ι  ƒ|$ EΘ‰$ιΎώÿÿv ‹  …Ι…υ"  ΈϋÿÿÿƒΔ[^_]Γt& ‰‹†t  ƒΰό‚ά   ‹–p  ƒΚ‰‹D$θ+όÿÿ…ΐtΕ‹D$‹@‹‹€x  ƒΰό‚ά   ‹ ¨t΅  …ΐ…Χ"  ƒΔ1ΐ[^_]ΓΈϊÿÿÿλ΄&    t& θόÿÿÿU‰ΥW‰ΗV‰ΞSƒμ‹  …Ò…Ϊ#  1Ϋ…φ<ι•   t& C‰$9π}D‰Αφ@t· f9tJ1Ι‰ψθYύÿÿ…ΐx1ƒΓ9ή~^[΅  ΑαT …ΐ…χ#  φBt±1Ι‰ψθ¨ϋÿÿ…ΐyΟƒΔ[^_]Γt& ‰Λ·I‰ψθύÿÿ…ΐxγ‰ΪΉ   ‰ψθyϋÿÿ‹$λ›t& ƒΔ‰π[^_]Γ¶    θόÿÿÿUWVS‰ΓƒμH‹3d΅    ‰D$D1ΐ΅  …ΐ…ο$  ‰ςk1ΐΉ   ‰οσ«»x  Ή}   ΗC    ΗC   Ηƒ  cx23Ηƒ  885 ‰|$σ«ΊΌ  Ί    Ηƒ|  cx23Ηƒ€  885 Ηƒ„  inteΗƒ  rnal‹†Μ      ‰Chƒ  ‰$‰ψθόÿÿÿ‰Αƒψ†¨   ƒψ0…ε$  ‹$‰ϊθόÿÿÿF‰[‰ƒ„   ‰θθόÿÿÿ‹ƒl  ‰«  …ΐ…Μ$  ‹=  …ÿ…$  ΅  …ΐ…-$  t$1ΐΉ   Ί!	  ‰χσ«Ή	   ‰πθόÿÿÿ‰ςΉΜ	  ‰θh    θόÿÿÿ‹ƒl  Z‹T$Dd+    u&ƒΔH[^_]Γt& ƒψ0„XÿÿÿƒΑƒω0†Lÿÿÿι%  θόÿÿÿθόÿÿÿƒΐθόÿÿÿ1ΐΓθόÿÿÿS‰Γƒμ‹  d΅    ‰D$1ΐ…Ι…%  ƒϊΈD  Ί   Ή   f‰D$”ΐ€f‰T$T$D$D$‰D$ƒ
  ΗD$D  θ&ύÿÿ‹D$d+    uƒΔ[Γθόÿÿÿfffffffθόÿÿÿ‹@(Η€¤   π  Η€¨       Η   ‹¤   ―¨   ‰Θ‹L$‰1ΐΗ    Γθόÿÿÿ‹€$  ‹@ ‹ ‹€ά      ƒϊt …ÒtΊ   ‰1ΐΓfΊ   ‰1ΐΓ¶    Ί   ‰1ΐΓ¶    θόÿÿÿS1Ι‹4  …Ò”ΑΊ   Q1Ι‹[θόÿÿÿZ[Γ΄&    v θόÿÿÿ1ΐΓ΄&    θόÿÿÿS‹ΰ  …Ιtθόÿÿÿ‰Γ…ΐ?%  1Ϋ‰Ψ[Γ΄&    t& θόÿÿÿWVS‹Έ<  ‰X  ‹Π  Η€<      …Òt‰Γθόÿÿÿ‰»<  ‰Ζ…ΐU%  1φ‰π[^_Γ΄&    fθόÿÿÿS‹ά  …Ιtθόÿÿÿ‰Γ…ΐk%  1Ϋ‰Ψ[Γ΄&    t& θόÿÿÿWVS‹ΈX  ‰<  ‹Π  Η€X      …Òt‰Γθόÿÿÿ‰»X  ‰Ζ…ΐ%  1φ‰π[^_Γ΄&    fθόÿÿÿS‰Σ‹”   Hƒϊ~2‰Θθόÿÿÿ…ΐt‹@…ΐt‹  …Ιt‰Ϊ[ιόÿÿÿ¶    [Γ¶    Ί   ‰ΘθόÿÿÿλΗ΄&    v θόÿÿÿ‰Β‹ ‹@(ιόÿÿÿ΄&    ΄&    θόÿÿÿ‹@(ιόÿÿÿv θόÿÿÿ‹@(‹   ύÿÿ   θόÿÿÿ1ΐΓ΄&    ΄&    θόÿÿÿ‰Β‹ ‹@(‹ ιόÿÿÿ΄&    ¶    θόÿÿÿ‹‹R(ιόÿÿÿθόÿÿÿUΉ   WV‰ΖS‹€$  ‰ΣΊ 0  ‹h ‹} ‰ψθόÿÿÿƒϋt!ƒϋt|…ΫtH‹@  ‰Ϊ‰π[^_]ιόÿÿÿt& ‰ψΊ    θόÿÿÿ‰ψΊ   θόÿÿÿ‹@  ‰Ϊ‰π[^_]ιόÿÿÿt& ‰ψΊ    θόÿÿÿ‰ψΊ   θόÿÿÿ‹@  ‰Ϊ‰π[^_]ιόÿÿÿt& ‰ψΊ    θόÿÿÿ‰ψΊ   θόÿÿÿ‹@  ‰Ϊ‰π[^_]ιόÿÿÿt& θόÿÿÿUΉ   WV‰ΖS‹€$  ‰ΣΊ   ‹h ‹} ‰ψθόÿÿÿƒϋt!ƒϋt\…Ϋt8‹@  ‰Ϊ‰πθόÿÿÿ1ΐ[^_]Γf‰ψΊ   θόÿÿÿΊ   ‰ψθόÿÿÿλΞ¶    ‰ψΊ   θόÿÿÿΊ   ‰ψθόÿÿÿλ®¶    ‰ψΊ   θόÿÿÿΊ   ‰ψθόÿÿÿλ¶    θόÿÿÿVS‰Γ‹€$  ‹p ‹‹Έ  ƒψw9ƒψw9ƒψuƒ»@  Ί    ‰Θt?θόÿÿÿ‹–<  …Òt(‰Ψ[^ιόÿÿÿ΄&    ƒψ!uαΊ   ‰ΘλΣ΄&    v 1ΐ[^Γv θόÿÿÿλΏθόÿÿÿUW‰ΟVSƒμ‹0‰$‹\$®¤  ‰θθόÿÿÿ‹†ά   Ί   ‰$  ‹†ά      ‰ψ¶ΐ€ΜΗ‰‹†ά      ‹€ζϋ‰‰ω‹†ά   ¶ύΟ Λ  ‰Έ   ‹†ά      ‹€ζχ‰€<$ ‹†ά   „Ò   Ίÿ  ‰$  ‹†ά      ‹€ζώ‰‹†ά      ‹
€<$ΐ% ΐÿÿ-@  !Θ‰‹=    ΅    ƒΗ9Ηy&ι   t& Έ2   Ή   Ίd   θόÿÿÿ΅    9Ηx‹†ά   ‹   φΗuÒ¶Ϋ‹†ά      ‹€   €ΜΓ‰‹D$‰‰θθόÿÿÿ1ΐ€<$ u‹D$‹ Αψƒΰ‡ƒΔ[^_]Γ΄&    ¶Ϋ€ΟΓ‰   ι(ÿÿÿ1Ϋλ¤v θόÿÿÿUWVS‰Γƒμ‹5    ‹€ά   ‰$‹|$ƒΖ   ‹ ‹‹ά   …ÿ„—   Α$  ‹)Νÿ   ‰)‰Α€δύ€Ν…ÒEΑ‹‹ά   €δς‰Β€Μ€Ξ…ÿEΒ‰   λt& ‹    9ΦxΈΗ  θόÿÿÿ‹ƒά      ‹ φΔuΪ‹“ά      ‹’   €Ξ‰¶ΐ…ÿEψƒΔ[^‰ψ_]Γ΄&    f©$  ‰ι‹m ε ÿÿÿ‰)$ι^ÿÿÿt& θόÿÿÿVpΊ   S‰Γ‰πθόÿÿÿ…ΐt‹@…ΐt‰πθόÿÿÿ‹³8  …φt‹FT‹@θόÿÿÿ‰πθόÿÿÿ‹³4  …φt‹FT‹@θόÿÿÿ‰πθόÿÿÿ‹³0  …φt‹FT‹@θόÿÿÿ‰πθόÿÿÿ‹³,  …φt‹FT‹@θόÿÿÿ‰πθόÿÿÿ‹‹Έ  ƒψt!ƒψu
‹S‰ΘθόÿÿÿΗƒ      1ΐ[^Γv ‰Ψθόÿÿÿ1ΐΗƒ      [^ΓfθόÿÿÿS‹tY  ‹@(‰€  ‹xY  ‰„  Η   ΅tY  ―xY  ‹L$‰΅|Y  [‰1ΐΓ΄&    t& θόÿÿÿ‹…ΐuΗA,MPEG1ΐΓv ΈκÿÿÿΓfθόÿÿÿ‰Β‹ ‹@(ά  ιόÿÿÿ΄&    v θόÿÿÿ‰Β‹ ‹@(ιόÿÿÿ΄&    ΄&    θόÿÿÿ‹‹R(Βά  ιόÿÿÿ΄&    t& θόÿÿÿUWVSƒμ(d‹    ‰T$$1Òθόÿÿÿ‹°(  †Ό  nPhI  j"|$Wθόÿÿÿ‹FƒΔ9Εt'X΄t& ‹Cd‹ …ΐt‹…Òt‰Ψθόÿÿÿ‹CLX΄9Εuΰ†Τ  ‰ϊθόÿÿÿ‹D$$d+    u
ƒΔ(1ΐ[^_]Γθόÿÿÿ΄&    θόÿÿÿWV‰ΞSθόÿÿÿ‹Έ(  ƒΏ€  tLΗF   ‹‡ά  ƒΗ‰F‹X΄9Ηt*¶    ‹Cd‹@…ΐt‹H…Ιt	‰ς‰Ψθόÿÿÿ‹CLX΄9Ηuά1ΐ[^_ΓΈκÿÿÿλυ΄&    ΄&    θόÿÿÿWV‰ΞSθόÿÿÿ‹Έ(  ƒΏ€  t<‹GƒΗX΄9Ηt)t& ‹Cd‹@…ΐt‹H…Ιt	‰ς‰Ψθόÿÿÿ‹CLX΄9Ηuά1ΐ[^_ΓΈκÿÿÿλυ΄&    ΄&    θόÿÿÿS‰Λθόÿÿÿ‹€(  ‹|  ‹€x  ‰S‰1ΐ[Γ΄&    θόÿÿÿS‰Λθόÿÿÿ‹(  ΗCMPEGΗC    ‹‚€  ―‚„  ΗC    ΗC   ‰C΅¤  …ΐ…©k  1ΐ[ΓθόÿÿÿS‰Λθόÿÿÿ‹€(  ΗCMPEGΗC    ‹€  ―„  ΗC    ‰S‹  ‰S‹  ΗC   ‰S‹¤  …Ò…Μk  1ΐ[Γ΄&    ¶    θόÿÿÿιόÿÿÿ¶    θόÿÿÿWV‰ΞSθόÿÿÿ‹Έ(  ƒΏ€  th‹…Òu[FΉ   ΊN  ƒΗθόÿÿÿ‹X΄9Ηt*¶    ‹Cd‹@…ΐt‹H…Ιt	‰ς‰Ψθόÿÿÿ‹CLX΄9Ηuά΅¤  …ΐ…οk  1ΐ[^_ΓΈκÿÿÿλυΈκÿÿÿλξv θόÿÿÿιόÿÿÿ¶    θόÿÿÿιόÿÿÿ¶    θόÿÿÿVS‰Λθόÿÿÿ‹°(  ΅¤  …ΐ…l  ‰Ϊ‰π[^ιόÿÿÿt& θόÿÿÿUWVS‹l$1Ϋ‹|$θόÿÿÿΊ   ‹°(  1ΐλ΄&    f›‹…δ  ‹…θ  !κ!ψ	ΠuƒΓƒϋuίΈκÿÿÿ[^_]Γt& ‰κ‰ω‰πθόÿÿÿ…ΐuη›Αα‘ΰ  ‹‰ΰ  ‰  ‹J‰   ‹J‰$  ‹J‹R‰(  ‰–,  [^_]ΓθόÿÿÿUWVS‰ΛθόÿÿÿΊ   ‹°(  ΎΌ  ‰ψθόÿÿÿƒψ‡1l  ‰ΑƒψtƒΑƒω‡'l  ‰ϊ‰Ψkθόÿÿÿ‹†ά  Ί    i€Έ    ‹Έ    ‰ψθόÿÿÿ‰Αƒψÿ„1l  ƒψ tƒΑƒω ‡'l  ‰ϊ‰θθόÿÿÿ‹–Μ   ‹‚΄   …ΐu‹‚   PC0hr  PθόÿÿÿΈ …Ί …ΗCT …ƒΔƒΎ€  DΒ‰CT[1ΐ^_]Γ΄&    t& θόÿÿÿS‰Λθόÿÿÿ‹(  ΗCMPEGΗC    ‹‚€  ―‚„  ΗC    ΗC   ‰C΅¤  …ΐ…;l  1ΐ[Γθόÿÿÿƒ=¤  S‰Γ‡Zl  ‹ƒά   Ί7   ‰(  Ί   ‹ƒά   ‰$  Ί ΰ  ‹ƒά   ‰   [Γ¶    θόÿÿÿWVΎ   S‰Γ‰Π‰Κ‹»ά   ‰·$  ¶Ι‹³ά   ‰ΟΟ X  ‰Ύ   ‹»ά   €Νψ‰   ¶Ξ‹³ά   ‰ΟΟ Y  ‰Ύ   ‹»ά   €Νω‰   ‰Ρ‹³ά   Αι¶Ι‰ΟΟ Z  ‰Ύ   ‹»ά   €Νϊ‰   Ακ‹»ά   ‰ΦΞ [  ‰·   ‹»ά   €Ξϋ‰—   ¶Π‹»ά   ‰Ρ€Ν\·Ι‰   fΚ ό‹»ά   ·Ò‰—   fΑθ‹»ά   ‰Β€Ξ]·Ò‰—   f ύ‹“ά   ·ΐ‰‚   Ί^  ‹ƒά   ‰   Ίώ  ‹ƒά   ‰   ‹5    ƒΖλ΅    9Ζx'ΈΗ  θόÿÿÿ‹ƒά      ‹ φΔtΫ[1ΐ^_Γt& [Έÿÿÿÿ^_Γ΄&    θόÿÿÿUWVS‰Γ‰Πƒμ‹»ά   ‰$Ή   ‰$  ¶Ò‹»ά   ‰Ρ€Ν\·Ι‰   fΚ ό‹»ά   ·Ò‰—   fΑθ‹»ά   ‰Β€Ξ]·Ò‰—   f ύ‹»ά   ·ΐ‰‡   Ί ^  ‹ƒά   ‰   Ί ώ  ‹ƒά   ‰   ‹5    ƒΖλ ΄&    f΅    9Ζ[  ΈΗ  θόÿÿÿ‹ƒά      ‹ φΔtΧ1ÿ‹ƒά   Ίÿ  ‰$  Ί X  ‹ƒά   ‰   Ί   ‹ƒά   ‰   ‹ƒά   ‹   Ή π  ‹ƒά   ‰   Ύ Y  ‹ƒά   ‰°   Ύ ™  ‹ƒά   ‰°   ‹ƒά      ‹ ‰D$‹«ά   ‰   Ύ Z  ‹«ά   ‰µ   Ύ   ‹«ά   ‰µ   ‹«ά   ‹…   ‹«ά   ‰   ‹«ά   µ   ½ [  ‰.‹«ά   µ   ½ ›  ‰.‹«ά   ‹µ   Αζ¶Ò	ς‹t$Αζ·φ	Φ‹“ά   ‰   Αΰ%  ÿ 	π‰Β‹$‰ƒΔ‰ψ[^_]Γ΄&    ΏÿÿÿÿιΉώÿÿ΄&    ΄&    θόÿÿÿWVΎ   S‰Γ‰Θ‹»ά   ‰·$  ¶Ι‹³ά   ‰ΟΟ P  ‰Ύ   ‹»ά   €Νπ‰   ¶Μ‹³ά   ‰ΟΟ Q  ‰Ύ   ‹»ά   €Νρ‰   ‰Α‹³ά   Αι¶Ι‰ΟΟ R  ‰Ύ   ‹»ά   €Νς‰   Αθ‹»ά   ‰ΖΞ S  ‰·   ‹»ά   €Μσ‰‡   ‰Π‹»ά   Αθƒΰ?‰ΖΞ@T  ‰·   ‹»ά   @τ  ‰‡   ¶Ζ‹»ά   ‰ΖΞ U  ‰·   ‹»ά   €Μυ‰‡   ¶Ò‹ƒά   ‰Ρ€ΝV‰   ‹ƒά   €Ξφ‰   ‹5    ƒΖλt& ΅    9Ζx'ΈΗ  θόÿÿÿ‹ƒά      ‹ φΔtΫ[1ΐ^_Γt& [Έÿÿÿÿ^_Γ΄&    θόÿÿÿUWVS‰Γ‰Πƒμ‹»ά   ‰$Ή   ‰$  Ακ‹»ά   ƒβ?‰ΦΞ T  ‰·   ‹»ά   €Ξτ‰—   ¶Τ‹»ά   ‰ΦΞ U  ‰·   ‹»ά   €Ξυ‰—   ¶ΐ‹»ά   ‰Α€ΝV‰   ‹»ά   €Μφ‰‡   ‹5    ƒΖλ΄&    ΅    9Ζc  ΈΗ  θόÿÿÿ‹ƒά      ‹ φΔtΧ1ÿ‹ƒά   Ίÿ  ‰$  Ί S  ‹ƒά   ‰   Ί “  ‹ƒά   ‰   ‹ƒά   ‹   Ή π  ‹ƒά   ‰   Ύ R  ‹ƒά   ‰°   Ύ ’  ‹ƒά   ‰°   ‹ƒά      ‹ ‰D$‹«ά   ‰   Ύ Q  ‹«ά   ‰µ   Ύ ‘  ‹«ά   ‰µ   ‹«ά   ‹…   ‰D$‹«ά   ‰   ‹«ά   µ   ½ P  ‰.‹«ά   µ   ½   ‰.‹«ά   ‹µ   ‰πΑβ¶π	ς‹t$Αζζ  ÿ 	Φ‹“ά   ‰   ‹D$Αΰ·Π‹$	ς‰ƒΔ‰ψ[^_]Γ¶    Ώÿÿÿÿι±ώÿÿ΄&    ΄&    θόÿÿÿU‰ΝWV‰ΦS‰Γƒμd΅    ‰D$1ΐƒ=¤  ΗD$    ‹|$(‡!m  ‹ƒΌ  ‰αPό‰Ψθόÿÿÿ‹$=xV4…ύl  ‹“Ό  L$‰Ψθόÿÿÿ‹D$…ΐ…Ωl  ‹“Ό  Ή   ‰ΨΗD$   θόÿÿÿ‹ƒΌ  ‰ρ1φP‰Ψθόÿÿÿ‹ƒΌ  Ήτ  P‰Ψθόÿÿÿ…ν~5t& ‹ƒΌ  ‹·T‰Ψθόÿÿÿƒ=¤  ‡Ύl  ƒΖ9υuΨƒώv ‹ƒΌ  1ΙT‰ΨƒΖθόÿÿÿƒώuε‹“Ό  Ή   ‰ΨΗD$   θόÿÿÿ‹5    ƒΖ
λ΅    9Ζ«l  ΈΖ§  θόÿÿÿ‹“Ό  L$‰ΨθόÿÿÿφD$tΡ‹T$$‹l$$1φ…Ò~-v ‹ƒΌ  ‰ωT‰Ψθόÿÿÿƒ=¤  ‡‘l  ƒΖƒΗ9υuΦ‹ƒΌ  L$P‰Ψθόÿÿÿƒ=¤  ‡vl  ‹“Ό  1Ι‰Ψθόÿÿÿ‹D$‹T$d+    uƒΔ[^_]Γθόÿÿÿ΄&    θόÿÿÿUWVSƒμHd΅    ‰D$D1ΐ‹t$\‹|$dƒ=¤  ‡gm  \$l…ÿt{‰4$D$pl$‰κΈv ‹pό‰ΓƒΐƒΒ‰rό9Θuξ‹4$U‰ω‰πÿt$l‹T$hθ΅ύÿÿZY‹t$h…φt‹‹L$ƒ|$h‰
u	‹S‹L$‰
‹T$Dd+    uƒΔH[^_]Γ΄&    v l$λ§θόÿÿÿt& θόÿÿÿιFύÿÿ¶    θόÿÿÿUWVS‰Γƒμ,‹¤  ‰T$d΅    ‰D$(1ΐ…Ι…λm  j j h€   SθυώÿÿƒΔ…ΐ‰;  ƒ=¤  ‡Εn  ΗD$    ΗD$    L$Ί   ‰ΨθόÿÿÿΊ  ‰Ψ|$‰ωθόÿÿÿΉνÿÿÿΊX  ‰ΨθόÿÿÿΉÿÿÿÿΊT  ‰Ζ‰ΨθόÿÿÿΉ  €Ίψ  	Ζ‰ΨθόÿÿÿΉ   Ίό  	Ζ‰Ψθόÿÿÿ1ΙΊd   	Ζ‰Ψθόÿÿÿ	π…―n  ‹ƒΜ   Ί‚     D$θόÿÿÿ…ΐ…n  ‹D$‹ϊΐ …n  ‹@8§  ‰Αt8h/  θόÿÿÿ‹D$θόÿÿÿXhD  hh0  θόÿÿÿXZΗD$ÿÿÿÿιτ  ¶    xf»UªuΏƒ=¤  ‡.  ‰$1ν1φ‰Λλ	΄&    ‰Ν‹«‹$‰κ)Ξ‰L$ƒξθόÿÿÿ…ΐ…Òm  ‹D$M‹ Αθ9ΘwΠ‹¤  ‹$…Ι…]o  …νx%v L$‰κ‰Ψθόÿÿÿ…ΐ…Do  ‹D$tƒνsή…φ…+o  ‹D$θόÿÿÿ΅¤  …ΐ…o  ΉÿÿÿÿΊT  ‰Ψθόÿÿÿ‹L$Ί   ‰Ζ‰Ψθόÿÿÿ‹L$Ί  	Ζ‰ΨθόÿÿÿΊX  	ΖL$‰Ψθόÿÿÿ‹L$ΊX  	Ζ‰ΨƒαθθόÿÿÿΉ @  Ί   	Ζ‰ΨθόÿÿÿΉ @  Ί  	Ζ‰ΨθόÿÿÿΊ   	ΖL$‰Ψθόÿÿÿ‰ωΊ  ‰Ψθόÿÿÿ…φo  ƒ=¤  ΗD$xV4ΗD$xV4ΗD$ 4xVΗD$$V4x‡so  1φ1νλv 1φƒΕύΐ „¥o  ‰ω‰κ‰Ψθόÿÿÿ‹D$9D΄uΪƒΖƒώuΤ΅¤  ƒΕ…ΐ…Ίm  ‰«Ό  j j h€   SθνϋÿÿƒΔ…ΐQ  Wjj hΔ   SθÒϋÿÿƒΔ…ΐΆm  ‹=¤  …ÿ…‡m  ΈΘ   θόÿÿÿ‹5¤  …φ…rn  ‹ƒ   Ύ   ‰ί% ω  ƒψΐƒΰ`ΰ  ‰ƒ  ÿ³  Pj jh‘   Sθfϋÿÿ‹ƒ  ƒΔf‰ƒΘ  ‹ƒ  f‰ƒΚ  1ΐχƒ   ÿÿ •ΐf‰ƒΜ  ƒΐ  θόÿÿÿjjj jhά   Sθϋÿÿjjj jhά   SθϋÿÿΈ<   ƒΔ0θόÿÿÿhπ   hπ   j jhΦ   Sθάϊÿÿj j j j j j j j j j j j j jhΧ   SθµϊÿÿƒΔXj@jhΐ€` hU‘U‘jjh ½ j jhΘ   SθϊÿÿƒΔ,΄&    fFν1Ϋj ƒψj –Γj SVj jh·   Wθ^ϊÿÿ‰πƒΖƒΔ$   €j j j SPj jh·   Wθ:ϊÿÿƒΔ$ƒώu²j j jhΪ   Wθ!ϊÿÿΈ<   θόÿÿÿj j hΝ   WθϊÿÿΈ<   ƒΔ$θόÿÿÿΉ€   ΊH  ‰ψθόÿÿÿ‹L$…Ιu"‹D$(d+    u?‹D$ƒΔ,[^_]Γ΄&    fj j j jh   Wθ­ωÿÿΈ
   θόÿÿÿƒΔΗD$    λ΄θόÿÿÿιΞo  ι(n  ΄&    θόÿÿÿUΊ   WVSƒμ‹h(‰θt  θ/ϊÿÿ‰Ζ…ΐty½Έ  ‰ψθόÿÿÿ‰$‹…t  9ΨtD΄&    ‹…t  ‹P‹-x  ‰Q‰
Ί   Η€x     Η€|  "  θόÿÿÿ‹…t  9ΓuΓ‹$‰ψθόÿÿÿ‰πƒΔ[^_]Γ΄&    ‹…t  ‰Ϊύÿÿ…ά  θόÿÿÿ‰πƒΔ[^_]Γ΄&    ¶    θόÿÿÿV‰ΖS‰ΣΊ  ƒμd΅    ‰D$1ΐ‰α‰πθόÿÿÿ‹D$d+    u·Λ$ƒΔ‰π[Ί  ^ιοÿÿθόÿÿÿ΄&    ΄&    θόÿÿÿV‰ΖS‰ΣΊ  ƒμd΅    ‰D$1ΐ‰α‰πθόÿÿÿ‹D$d+    u·Λ‰πΊ  χΡ#$ƒΔ[^ι²ξÿÿθόÿÿÿ΄&    ¶    θόÿÿÿW‰ΟV‰ΖS‰ΣΊ   ƒμd΅    ‰D$1ΐ‰α‰πθόÿÿÿ·Λ…ÿt&$‹D$d+    uƒΔ‰πΊ   [^_ιLξÿÿt& χΡ#$λΦθόÿÿÿ΄&    t& θόÿÿÿƒμd‹    ‰T$1ÒT$ΗD$    Η$    RT$Rjj hΖ   PθCχÿÿ΅¤  ƒΔ…ΐ…ζo  ‹D$d+    uƒΔΓθόÿÿÿ΄&    ¶    θόÿÿÿS‹X(j j jj jh‚   SθςφÿÿΈτ  θόÿÿÿ‰Ψθόÿÿÿƒά  ƒΔ[ιόÿÿÿfθόÿÿÿS‰Γ΅¤  …ΐ…
p  ‹ƒΔ  …ΐt%‹π  ƒβu&θόÿÿÿƒΤ  θόÿÿÿΗƒΔ      [Γ΄&    v θόÿÿÿλΨ΄&    fθόÿÿÿUWVS‰Γƒμ΅¤  …ΐ…op  iƒΈ    ƒΈ   …N  Ηƒ(      ³ΐ  Ί2   Ηƒ,      ‰πΗƒ  •  Ηƒ      Ηƒ$      Ηƒ  ΰ  Ηƒ  Π  ΗƒΔ     θόÿÿÿ‰Ε…ΐtƒΔ‰θ[^_]Γt& ‰›D  1Ò‰π»  @  ΗƒH  p‡  ”Βθόÿÿÿ“Τ  C\1Ιj θόÿÿÿ‹ƒά  ‰D$X΅¤  …ΐ…‹p  θόÿÿÿ‰Β…ΐt^‰ΗΎ@V  ΉΝ   σ¥h  ‹ƒά  i€Έ    ÿ°    ‚ΐ  h΅  j ‰T$Pθόÿÿÿ‹D$‹T$ƒΔΗ‚      ƒΐ‰‚¬  ‹ƒΜ   »  ‰“Δ  ³Θ  ΗƒΘ        ‰›π  ‰ƒΠ  ‰πΗƒΜ     Ηƒ      Ηƒ     Ηƒψ     Ηƒδ     Ηƒθ      Ηƒό      ‰»ά  θόÿÿÿ…ΐ   ‹ƒΔ  ‰(  ‹ƒΔ  ‰Έ0  ‹ƒΔ  ‰°Έ  ‹ƒΔ  Η€δ     ƒ»€  t‹ƒΔ  δ      ‹ƒΔ  Ήÿÿÿÿ‹ΰ   ÿ21ÒjθόÿÿÿY_‰ΖƒΌ  …φ‰&p  ιSp  ƒΔ‰Ε[‰θ^_]Γt& ½νÿÿÿιώÿÿfffθόÿÿÿSπΊp’Β¶ÒΑβπΊp’Γ‰ΡƒΙ„ΫEΡπΊp’Γ‰ΡƒΙ„ΫEΡπΊp sƒΚ‹H(…Ιt-¬  [ιόÿÿÿ΄&    …Òuβ[Γ΄&    v θόÿÿÿπΊp Γt& θόÿÿÿ‹H`φΒtπ€‰Έ  φΒtπ€‰Έ  φΒtπ€‰Έ  ƒβtπ€‰Έ  ‘¨  9  t‰Ρ‹    Έ    ιόÿÿÿt& ‰Πιÿÿÿ΄&    fθόÿÿÿƒβ‹H`tπ€‰Μ  9  t‹    ΑΌ  Έ    ιόÿÿÿt& πΊ±Μ   ΓfffffffθόÿÿÿWVShζÿÿƒμ‹@όd‹    ‰T$1ÒΖD$ …ΐt6‹Pd‹…Òt-‹r$…φt&‹    L$…Òtx‹z$…ÿtqΊ   θόÿÿÿ€|$ u<Ί   ‰ΨθόÿÿÿΊ   ƒΘ€¶Θ‰ΨθόÿÿÿΊ#   ‰ΨθόÿÿÿΊ#   ƒΘ€¶Θ‰Ψθόÿÿÿ‰ΨΊ   θόÿÿÿ‹D$d+    uƒΔ[^_Γt& Ί   θόÿÿÿλθόÿÿÿfffffffθόÿÿÿVSƒμ0‹€h  d‹    ‰T$,1Ò‹…Ϋ„)  ‹ƒ¨  …ΐ„  ΗƒΨ      ‹Pd‹R…Ò„³   ‹J…Ι„¨   ‹   …Ò„Κ  ‹r…φ„Ώ  ‰βθόÿÿÿ‹‹Έ  ‹ƒ¨  ƒω†{  ƒι!ƒωw#Ί   ΣβχΒπ…0  β‚  …€   t& …ΐ„Θ   ‹Pd‹R…Ò„Ί   ‹J…Ι„―   ‹   …Ò„Y  ‹Z…Ϋ„N  ‰βθόÿÿÿ1ΐι   ‹‹Έ  ƒω†™   ƒι!ƒωw―»   Σγ‰ΩχΓπ…¬   α‚  t’ΗD$ θ€  ΗD$$ψ§  ΗD$ ] ΗD$    ΗD$  ΗD$¦ίΌ ΗD$”  ΗD$!   ΖD$…ΐ…?ÿÿÿ΄&    1ΐ‹T$,d+    …Ώ   ƒΔ0[^Γ΄&    ƒω†ÿÿÿ»   Σγγ uƒω„kÿÿÿ…Ò…ύώÿÿλµt& ΗD$    ΗD$  ΗD$Υά2 ΗD$ ΖD$ι¶ώÿÿ¶    ƒω†§ώÿÿΊ   Σββ uΈƒω…ώÿÿιÿÿÿf‰βθόÿÿÿι<ώÿÿt& ‰βθόÿÿÿι<ÿÿÿt& Ένÿÿÿι0ÿÿÿθόÿÿÿθόÿÿÿU‰ΕW1ÿVSμ  ‹Τ  ‰T$d΅    ‰„$  1ΐ‹…¨  ΗD$    …ΐtz‹Pd‹R…Òtp‹2…φtj‹   …Ι„„   ‹   ‹	‰$…ΙtuL$QΉ   ‹t$T$θόÿÿÿZ‹D$‰ΖΑξt1ÿ‹CTόƒΗθόÿÿÿ9ώuν‹D$Ώ   …ΐ…xÿÿÿt& €|$ uA‰ψ„ΐu1‹„$  d+    u^Δ  [^_]ΓL$QΉ   T$θόÿÿÿXλ‹CθόÿÿÿλΕ‹[T$ΗD$    ΗD$    ‰ΨΖD$θόÿÿÿΖƒ0  ‰Ψθόÿÿÿλ’θόÿÿÿ΄&    θόÿÿÿWVS‰Γƒμ0d΅    ‰D$,‹ƒ¨  …ΐ„ό   ΗƒΨ     ‹Pd‹R…Òt'‹J…Ιt ‹   …Ò„
  ‹r…φ„ÿ   ‰βθόÿÿÿ€|$
 ‹5   …   ΄&    1ΐΖD$
f‰D$‹ƒ¨  …ΐtr‹Pd‹R…Ò„΅   ‹J…Ιt4‹   …Ò„—   ‹z…ÿ„   ‰βθόÿÿÿ‹ƒ¨  …ΐtp‹Pd‹R…Òtf‹J…Ιt_‰β…φtT‹~…ÿtMθόÿÿÿ¶D$
„ΐ„{ÿÿÿv ƒ  θόÿÿÿƒ¬  θόÿÿÿƒΐ  θόÿÿÿ‹D$,d+    u6ƒΔ0[^_Γv θόÿÿÿ¶D$
λ±t& ‰βθόÿÿÿιoÿÿÿt& ‰βθόÿÿÿιόώÿÿθόÿÿÿ΄&    θόÿÿÿ‹€h  ‹ …ΐtιzώÿÿ΄&    v Γ΄&    ΄&    θόÿÿÿUWVS‰Γƒμ0‹‹¨  d΅    ‰D$,1ΐ…Ιt…Òt‹ƒΈ  ƒψw-ƒψ‡  t& ‹D$,d+    …Σ  ƒΔ0[^_]Γ΄&    ƒθ!ƒψwΨΎƒρ£ΖsΞ‰Φƒζƒβ„ά   ‹Ad‹@…ΐ„ϊ   ‹x…ÿ„ο   ΅   ‰β…ΐ„0  ‹h…ν„%  ‰Θθόÿÿÿ‹‹¨  ‹ƒΨ  ΖD$ …ΐ•D$
…Ι…ΐ   …φ…  ‹ƒΨ  ΖD$…ΐ‹ƒ¨  •D$
…ΐ„>ÿÿÿ‹Pd‹R…Ò„0ÿÿÿ‹J…Ι„%ÿÿÿ‹   …Ò„ί   ‹Z…Ϋ„Τ   ‰βθόÿÿÿι ÿÿÿΎ€£Ζƒςώÿÿ‰Φƒζƒβ…$ÿÿÿ…φ„άώÿÿ‹D$,d+    …―   ƒΔ0‰Ψ1Ò[^_]ι›ϋÿÿv ‹ƒΨ  ΖD$ …ΐ•D$
‹Ad‹@…ΐ„2ÿÿÿ‹x…ÿ„'ÿÿÿ΅   ‰β…ΐtZ‹h…νtS‰Θθόÿÿÿι	ÿÿÿ΄&    ‰Θθόÿÿÿ‹‹¨  ιΦώÿÿ¶    Ί   ‰Ψθ$ϋÿÿιαώÿÿ΄&    ‰βθόÿÿÿι,ώÿÿt& ‰Θθόÿÿÿι¶ώÿÿθόÿÿÿ΄&    θόÿÿÿUWVS‹¨  …Ι„Δ  ‹Έ  ‰Ζƒω"„Έ  †Ϊ  ƒω0„I  †‹  ΏΑ  ƒω3‡e  t& ΅   Ή   Ίΐ  θόÿÿÿ‰Ε…ΐ„|  ‰0i†Έ    ÿ°    hΦ  hΐ  θόÿÿÿ‰EƒΔ…ΐ„^  ‹–Μ   ‹‚΄   …ΐ„§  Phζ  hΐ  θόÿÿÿ‰EƒΔ…ΐ„%  Έ   θόÿÿÿ‰Γ…ΐ„ν  ‰E‹EΊ   ‰ƒΨ  ‹Ef‰“ζ  ‰ƒά  Έ   f‰ƒΰ  ‹†Μ   ·P$f…Ò…E  ·@ f‰ƒβ  ‹†Μ   ·@"f‰ƒδ  ‹†Μ   ‰»μ     ‰«h  ‰C$‰ΨΗƒ4  όÿΏΗƒ8      Ηƒΰ  π”  Ηƒδ     Ηƒθ  ρ  ‰®Τ  θόÿÿÿ‰Η…ΐ…ξ   [‰ψ^_]Γ΄&    Ώνÿÿÿƒω:uθΏg  ιώÿÿt& ƒω)w#Ώ¶  ƒω'‡uώÿÿΏνÿÿÿ[‰ψ^_]Γ΄&    fƒι-ƒωwγΏΑ  ιNώÿÿ¶    ƒωw3Ώνÿÿÿƒωv‹Έ   Σΰ© u•ƒω…tÿÿÿΏ  ιώÿÿ΄&    Ώ  ƒω!„ώÿÿΏνÿÿÿλ‹v ‹‚   ιNώÿÿt& f‰“β  ‹†Μ   ·@&ιΊώÿÿfΏΛ  ιΖύÿÿ¶    ‰πθ™ωÿÿ‰ΨΗ†Τ      θόÿÿÿ‹Eθόÿÿÿ‹Eθόÿÿÿ‰θθόÿÿÿ‰ψ[^_]Γ΄&    v ΏτÿÿÿλΡ΄&    fΏt  ιfύÿÿΏτÿÿÿι®ώÿÿΏτÿÿÿλµΏτÿÿÿλ¶΄&    ΄&    θόÿÿÿS‰Γθωÿÿ‹ƒΤ  …ΐt9‹@θόÿÿÿ‹ƒΤ  ‹@θόÿÿÿ‹ƒΤ  ‹@θόÿÿÿ‹ƒΤ  θόÿÿÿΗƒΤ      [Γfffffffθόÿÿÿ‰Β‰Θ‹’”   ‹’ΐ   ‹’ά     ‹’  ƒΚ ‰‹T$‰1ΐΓ΄&    t& θόÿÿÿWVS‹°”   ‰ΣΎP  ‰ψθόÿÿÿ‹–$  ‰‹–(  ‰S‹–,  ‰S‹–0  ‰S‹–4  ‰S‹–8  ‰S‹–<  ‰S‹–@  ‰S‹–D  ‰S ‹–H  ‰S$‹†L  ‰C(‰ψθόÿÿÿ1ΐ[^_Γt& θόÿÿÿWVS‹°”   ‰ΣΎπ   ‰ψθόÿÿÿ‹–Δ   ‰‹–Θ   ‰S‹–Μ   ‰S‹–Π   ‰S‹–Τ   ‰S‹–Ψ   ‰S‹–ά   ‰S‹–ΰ   ‰S‹–δ   ‰S ‹–θ   ‰S$‹†μ   ‰C(‰ψθόÿÿÿ1ΐ[^_Γt& θόÿÿÿWΈP  VS‹°ΐ   ‰Γ‰ψθόÿÿÿ‹†ά     ‹ƒβί‰‹†ά      ‹βύÿÿ‰‹†ά      ‹ƒβί‰Ίÿÿ  ‹†ά   ‰  Ζƒ.  ‰ψθόÿÿÿ1ΐ[^_Γt& θόÿÿÿWΈπ   V‰ΖS‹ΐ   ‰ψθόÿÿÿ‹ƒά     ‹ƒβμ‰‹ƒά      ‹βΏώÿÿ‰‹ƒά      ‹ƒβο‰‹ƒά      ‹ƒβσ‰1Ò‹ƒά   ‰  Ίÿÿ  ‹ƒά   ‰  Ζ†Ξ   ‰ψθόÿÿÿ1ΐ[^_Γt& θόÿÿÿUW‰ΗVSμ„   ‰D$‰L$d΅    ‰„$€   ‹‡”   ‹°ΐ   ‰D$‹†ά      ‹ ‰D$ ‹†ά   ‹Έ  ‹†ά   ‹  ‰Ψ‰Ϊ‰ύΖ ƒΰƒβ ƒε ‰D$‰Ψƒΰ‰$‰ψƒΰ‰D$‰ψƒΰƒ=¨  ‰D$‡μp  …νt…Ò…¶  !ϋ‹|$‰Ψ‹$ƒΰD$…Ϋ‹D$•Β…ÿ¨   •ΐ!Β‹D$T$…ΐ„p  ‹L$1Ϋ…Ι„b  ‹D$  ‰$¶    ‹†ά   @  ‹ ‰Ββÿÿύÿ‰Tά@ƒΓ©   t=ƒϋuΨ‰θθόÿÿÿΉ@   ‰Η‹$T$@θόÿÿÿ‰ϊ‰Γ‰θθόÿÿÿƒϋ@„κ   ιυq  t& ‰θΑγθόÿÿÿ‰Ω‰Η‹$T$@θόÿÿÿ‰ϊ‰$‰θθόÿÿÿ;$…υq  ‹D$ΗD$<    Ζ €|$ …r  €|$ Έÿώÿÿt*ƒL$<‹|$ ‹ά   !ψ‰   ‹†ά   ‰Έ   ‹D$Ζ ‰θθόÿÿÿ‹L$‰Β‹  +  =ÿ  vƒL$<‰θθόÿÿÿ‹\$<…Ϋt‹L$…Ι…©   ‹„$€   d+    …Δ   Δ„   1ΐ[^_]Γ1ΫιΉώÿÿ΄&    f€|$ tU1Ϋιώÿÿ‹†ά     ‹ƒβί‰‹D$ΗD$<   …ΐt ‹@`…ΐt‹h8…νt‹D$L$<Ίv@θόÿÿÿ‹D$Ζ ιόύÿÿ€|$ ΗD$<    „ ÿÿÿιQr  t& ‹|$‹G`…ΐ„Hÿÿÿ‹X8…Ϋ„=ÿÿÿL$<Ί v@‰ψθόÿÿÿι(ÿÿÿθόÿÿÿt& θόÿÿÿU‰ΥWVSƒμ$‹”   ‰D$ ‹‚  ‹  ‰D$ƒαψu‹D$8Η     ƒΔ$1ΐ[^_]Γ΄&    Ί   ‰L$‰ψ‰$θόÿÿÿ‹$‹L$‰Ζ‚  ‰κθόÿÿÿ‰ς‰$‰ψθόÿÿÿ‹$‹|$8‰Θƒΰψ‰‰ΘΑθt¤·ΫDΕ ƒΓ‰D$‰$Αϋ‰\$ιλ   ‰π‹\$Αθƒΰ‰D$…Ϋtƒt$‹D$ƒΰD$1ΫΗD$    ·φ1ÿ‹L$‹$¤χΑζ‰π‰ώ‰ΟƒΘ―ς―ψ‰D$χ$$ώΏθ  ςiςθ  χης1φ‰Ρ‰Η…Òt‰ΦΊχΪKΡξ‰πχβ‰ΠΑκΑθ‰Φkΐ6)Α‰ψ‰ΚΏ6   χχ‰Ρ‰ςƒωvƒΐΊ ΅ =ηhΝw
ΊΣMbχβΑκΑγ‰U 
\$ƒ=¨  ΗE    ]‡wr  ƒΕ;l$„ώÿÿ‹u ‰π%ÿÿ =ÿÿ … ÿÿÿΖD$ »   ΗD$   ΗD$    ιÿÿÿθόÿÿÿUWVSƒμ$‹°”   €z
 ‹®ΐ   …Ξ  ‹B‰Σ‰D$…ΐ…  †P  ‰$θόÿÿÿ¶C
†.  ΗC    Η†(      Η   Η†$     ‹…ά     ‹ƒβί‰‹…ά      ‹βύÿÿ‰¶K‹…ά   Αα   ‹1Θƒΰί1Θ‰¶C†/  „ΐ„ε   ‹K1ÒΑα‰ΘΡθ€ω7χρ‰Α=   ‡M  ƒψ†¬  @ÿ‰Ο‰D$Αη‰D$Έόχι‰D$‰ψΡθ‰T$ 1Ò€ω7χχiΡ   ΉχΪKƒΒΡκ‰Η‰ΠχαΑκ‰T$‹…ά   ‹L$‰  ‰{ΊY·Ρ‰Ύ4  kKdΑ8  ‰ΘχβΈ   Ακ	ωq  ƒÒÿ9ΒGΠ‹…ά   ‰  ƒΒ‹D$kÒdƒΒΑκ‰S‰–8  ‹T$ ι  v ‹CΊ ΅ ΗD$    9ΠGΒΊ6   χβ‰Ρ‰Η…Òt‰ΘΊΣMbχβ‰ΠΑκΑθ‰T$iΐθ  )Α‰ψ‰ΚΏθ  χχ‰Ρ‹T$ωσ  vƒΐƒÒ 1Ι‰D$‰Χ…Òt"Ή@  ‰Πχα‰ψ)ΠΡθ‰ΚΑκ‰Π‰ΡΑΰ)Π)Η‰ϊ‹D$Ώÿÿ χχ‰ΚΏ   1Ι9Η‰ΟΧ‚¨  Ώ   9ΗΡƒ  ƒΐÿ‰ΑƒÒÿ‰D$‰ΗƒΑΈό‰T$χι‰T$iΡ   ΉχΪK‰D$ƒΒΡκ‰ΠχαΑκ‰T$‹…ά   ‰Έ  ‹D$‹T$ΗD$    ‰Ρ‰Η…Òt‰ΠΊχΪKΡθχβ‰ΠΑκΑθ‰T$kΐ6)Α‰ψ‰ΚΏ6   χχ‰Ρ‹T$ƒωvƒΐ‰C‰†0  ‹D$‰†d  ‹D$‰C(‰†L  ‹…ά      ‹€ζχ‰¶K‹…ά   Αα   ‹1Θ€δο1Θ‰¶C†=  ¶K‹…ά   Αα   ‹1Θ€δ1Θ‰¶C†<  ¶C	†-  ¶C†,  „ΐtC€{	 t‹…ά     ‹ƒΚ ‰‹…ά   €{ Ώ€  D|$   ‹€   1ψ%ύÿÿ1ψ‰‹$θόÿÿÿ1ΐƒΔ$[^_]ΓfƒΔ$‰π[^_]ιφÿÿfΗD$χJ Ώÿÿ  ΗD$ÿÿ  ΗD$  όΗD$  ι|ώÿÿ¶    ΗD$χJ Ώ3   ΗD$  όΗD$   ΗD$ÿÿ  ΗD$ÿÿ  ιΣόÿÿ¶    ΗD$”   Ώ   ΗD$   ΗD$0ψ?ΗD$    ιώÿÿ¶    ΗD$”   ΏΜΏ ΗD$0ψ?ΗD$     ΗD$   ΗD$   ιkόÿÿ¶    ΈΪÿÿÿιÿÿÿ¶    θόÿÿÿUWVSƒμ$‹¨”   €z
 ‹½ΐ   …  ‹B‰Σ‰D$…ΐ…}  …π   ‰$θόÿÿÿ¶C
…Ξ   ΗC    Η…Θ       Η   Η…Δ      ‹‡ά     ‹ƒβμ‰‹‡ά      ‹βΏώÿÿ‰¶K‹‡ά   Αα   ‹1Θƒΰο1Θ‰¶C…Ο   „ΐ„5  ‹s1ÒΑζ‰πΡθ€ω7χφ‰Α=   ‡  ƒψ†  @ÿ‰Ξ‰D$Αζ‰D$Έόχι‰D$‰πΡθ‰T$ 1Ò€ω7χφiΡ   ΉχΪKƒΒΡκ‰Ζ‰ΠχαΑκ‰T$‹‡ά   ‹L$‰  ‰sΊΛk(―‰µΤ   ΗC2   Η…Ψ   2   ‹KΑαq	‰πχβ)ΦΡξς1φΑκ9S sQ
ΎΝΜΜΜ‰ΠχζΎ   ΑκƒΑ‰S ΊOμΔN‰ΘχβΑκ9S$vΊ«ªªª‰ΘƒΞχβΑκ‰S$‹‡ά      ‹1πƒΰό1π‰‹C ‹T$ ‰…δ   ‹C$‰…θ   ‹D$ι  t& ‹CΊ ΅ ΗD$    9ΠGΒΊ6   χβ‰Φ‰Α…Òt‰πΊΣMbχβ‰ΠΑκΑθ‰T$iΐθ  )Ζ‰Θ‰ςΉθ  χρ‰Φ‹T$ώσ  vƒΐƒÒ 1φ‰D$‰Ρ…Òt"‰ΘΊ@  χβ‰Θ)ΠΡθ4‰ςΑκ‰Π‰ΦΑΰ)Π)Α‰Κ‹D$Ήÿÿ χρ‰ς1ΙΎ   9Ζ‰ΞΦ‚  Ύ   9ΖΡƒ‘  ƒΐÿ‰ΑƒÒÿ‰D$‰ΖƒΑΈό‰T$χι‰T$iΡ   ΉχΪK‰D$ƒΒΡκ‰ΠχαΑκ‰T$‹‡ά   ‰°  ‹D$‹T$1φ‰D$‰Ρ…Òt‰ΦΊχΪKΡξ‰πχβ‰Π‰ΦΑθΑξkΐ6)Α‰Κ‹D$Ή6   χρ‰Ρ‰ςƒωvƒΐ‰C‰…Π   ‹D$‰…  kK6‹·ά   Ατ  ωÿÿη‡.  ω  †²  ‰ΘΊΣMbχβΈχΪK‰ΡΑιiΡθ  ƒΒΡκχβΑκ‰  ‹D$‰S‰•ΰ   ‰C(‰…μ   ‹‡ά      ‹€ζϋ‰‹‡ά      ‹ƒΚ‰¶C…ά   ‰…  ¶C	…Ν   ¶C…Μ   „ΐ…«   ‹$θόÿÿÿ1ΐƒΔ$[^_]Γ΄&    fƒΔ$‰θ[^_]ιrρÿÿfΗD$χJ Ύÿÿ  ΗD$ÿÿ  ΗD$  όΗD$  ιώÿÿ¶    ΗD$χJ Ύ3   ΗD$  όΗD$   ΗD$ÿÿ  ΗD$ÿÿ  ι“όÿÿ¶    Ί«„ Ήÿÿ  ιςώÿÿ΄&    fµ   ‰πθόÿÿÿΗ…      Η…      ‰Β‰πθόÿÿÿ€{	 t‹‡ά     ‹ƒΚ‰€{ Έ@  DD$‰Γ‹‡ά      ‹€   1Ψ%Ώώÿÿ1Ψ‰ιαώÿÿt& 1Ò1Ιιhώÿÿ΄&    ΗD$”   Ύ   ΗD$   ΗD$0ψ?ΗD$    ι„ύÿÿ¶    ΗD$”   ΎΜΏ ΗD$0ψ?ΗD$     ΗD$   ΗD$   ι‹ϋÿÿ¶    ΈΪÿÿÿιeώÿÿ¶    θόÿÿÿUWVS‰Γƒμ$‹€”   ‹€ΐ   ‹Έά   ‹·   ‹Έά   ‹  ‰L$‹Έά   ‹  ‰L$‹Έά   ‹Ώ  ‰|$‹Έά   ‹Ώ  ‰|$‹Έά   ‹Ώ  ‰|$‹€ά   ‹€  ƒΓp‰D$½f  Ώ^  Sh  θόÿÿÿχΖ   XZΈW  Ί[  DΒPSh3  θόÿÿÿ‰π‰ιƒΔƒΰEΟ‰$QSh<3  θόÿÿÿ‰κƒΔχΖ@   EΧRSh46  θόÿÿÿ‰ρƒαÿ4   ShΠ7  θόÿÿÿΊz  Έo  ƒΔχΖ @  DΒPShl3  θόÿÿÿΊ”  Έ  ƒΔχΖ   DΒPSh3  θόÿÿÿΊΉ  Έ©  ·|$ƒΔχΖ    DΒPShΜ3  θόÿÿÿ‹L$ƒΔ…Ι„(s  ιcv  t& θόÿÿÿUΉh  Ίΐ  WV‰ΖSƒμ0d΅    ‰D$,1ΐ΅$   θόÿÿÿ…ΐ„Ψ  Έ  Ί   ‰ΓΉ   Η€       ‰ψhΐ  θόÿÿÿZ…ΐ…™  ‰³ΐ   Ί`  ‰Ψθόÿÿÿ†Ό  ‰›”   PCph7  j PθόÿÿÿF‰ΪΗƒ      θόÿÿÿ‰ΕƒΔ…ΐ…	  ‹†ά   ‰¨  Ή¨  ƒπ   ΊA  θόÿÿÿ‰θΉ
   t$Η$   ‰χσ«‹CdΖD$
ΗD$ ΗD$ Έ  ΗD$$  ‹@ΖD$ΗD$   …ΐt(‹H…Ιt!΅   ‰β…ΐ„¨   ‹x…ÿ„   ‰ΨθόÿÿÿƒP  Ή¨  ΊX  ‰χθόÿÿÿ1ΐΉ
   σ«‹CdΗD$   Η$   ‹@ΖD$
ΖD$ΗD$   …ΐt2‹H…Ιt+΅   ‰β…ΐtI‹p…φtB‰Ψθόÿÿÿλ΄&    f‰ψθόÿÿÿ‹D$,d+    uAƒΔ0‰θ[^_]Γf‰Ψθόÿÿÿι^ÿÿÿt& ‰ΨθόÿÿÿλΞ΄&    ‰Ψ½τÿÿÿθόÿÿÿλΉ½τÿÿÿλ²θόÿÿÿ¶    θόÿÿÿVΊ   Sθόÿÿÿ…ΐt<‰Γ‹€”   θμÿÿ‹ƒ”   θ“λÿÿ‹³”   ‰Ψθόÿÿÿ†  θόÿÿÿ‰πθόÿÿÿ1ΐ[^ΓΈνÿÿÿλφffffθόÿÿÿS‰ΓfΑΒΉ   ƒμd΅    ‰D$1ΐΈ   f‰T$‰βf‰D$D$‰D$‰ΨΗ$D   θόÿÿÿƒψ…Jw  Ή   ‰β‰ΨΗD$  θόÿÿÿƒψ…4w  ¶D$‹T$d+    uƒΔ[Γθόÿÿÿt& θόÿÿÿVS
  ‰Ψƒμd‹    ‰T$1ÒΊ  θGÿÿÿΉ   Ί  ƒΰοf‰L$t$Ή   f‰T$‰βD$‰ΨΗ$D   ‰t$θόÿÿÿƒψ…Άw  Ή   Έκ   ‰β‰t$f‰L$Ή   f‰D$‰ΨΗ$D   ΗD$³θόÿÿÿƒψ…w  Ή   Έ	   ‰β‰t$f‰L$Ή   f‰D$‰ΨΗ$D   ΗD$θόÿÿÿƒψ…vw  Ί  ‰Ψθ~ώÿÿΉ   ƒΘf‰L$‰βΉ   ‰t$Ύ  D$‰ΨΗ$D   f‰t$θόÿÿÿƒψ…`w  ‹D$d+    uƒΔ[^Γθόÿÿÿffffθόÿÿÿ1ΐΓ΄&    θόÿÿÿU¶ÒWV‰ΖS‰ΛƒμPd‹    ‰L$L1Ι‹D$hf‰$1Ιf‰L$‹|$dP\$f‰T$T$‰T$ƒψs4…ΐupΉ   ‰β‰πθόÿÿÿƒψu}1ΐ‹T$Ld+    …‚   ƒΔP[^_]Γf‹l$L$)Ν‰T$‹Tό)ο‰T	θ‰ϊƒΰόƒψrªƒΰό1ÿ‹,:‰,9ƒΗ9Ηrσλ–¶    ¶L$¨t…·Tώf‰Tιvÿÿÿ¶    ‹°  …Ò…Έw  Έÿÿÿÿιmÿÿÿθόÿÿÿv θόÿÿÿWVSƒμ‹p,d‹    ‰L$1ΙΖD$€…Òu|Έ¬Δ  θόÿÿÿ¶VL‹FH1Ιj|$Wθ½ώÿÿY‰ΓX…Ϋu5Έ¬Δ  θόÿÿÿ¶VL‹FH1ΙΖD$ jWθ•ώÿÿΈθ  θόÿÿÿ1Ò‰πθόÿÿÿXZ‹D$d+    uƒΔ‰Ψ[^_Γ΄&    v »κÿÿÿλΩθόÿÿÿt& θόÿÿÿV‰ΞS‰Σƒμ d‹    ‰T$1Ò¶ΣL$‰t$f‰T$f‰T$‹T$,‰L$Ή   f‰L$Ή   f‰T$T$ΖD$ ΗD$   θόÿÿÿƒψu)΅°  …ΐ…άw  1ΐ‹T$d+    u"ƒΔ [^Γ΄&    ‹°  …Ò…x  ΈÿÿÿÿλΡθόÿÿÿ¶    θόÿÿÿS‰Γ1Òƒμ(d΅    ‰D$$1ΐC¬θόÿÿÿ΅    9Cx‹Cό…ΐt‹D$$d+    uOƒΔ([Γt& ¶Sψ‹Cτj!L$θφώÿÿY‰Β΅    θ  ‰C…ÒuΔ΅°  …ΐ….x  ¶D$ΑΰΑψƒΰ‰Cόλ¤θόÿÿÿ΄&    ¶    θόÿÿÿUWV‰ΖSƒμ‹D$,‰L$‰$‹D$4‰D$d΅    ‰D$‹Fd‹…Ò…ή  ¶ω9~lte¶VL‹FHjL$θ^ώÿÿ‰ΒX…Òt‹D$d+    …¶  ƒΔ‰Π[^_]Γt& ¶D$¶VLƒΰσ
D$D$‹FHjL$Q1ΙθcόÿÿY]‰Β…ΐu΄‰~l«¤  ‰θθόÿÿÿ‹ƒά   Ί   ‰$  ‹ƒά      ¶D$0€ΜΗ‰‹ƒά      ‹€ζϋ‰‹ƒά      ‹D$0¶Δ€ΜΛ‰‹ƒά      ‹€ζχ‰€<$ …ή   ‹ƒά      ¶D$€ΜΓ‰‹ƒά      ‹Ώÿύÿÿ€~L@ΉÿώÿÿEΟ!Κ‰‹ƒά      ‹
€<$ΐ% ΐÿÿ-@  !Θ‰‹=    ƒΗλ΄&    f‹    9ΧxΈΗ  θόÿÿÿ‹ƒά      ‹ φΔuΪ‹“ά      ‹’   €ΞΓ‰¶Π‰θ‰T$θόÿÿÿ΅°  ‹T$…ΐ…Xx  1ΐ€<$ DΠι_ώÿÿ¶    ‹ƒά   Ίÿ  ‰$  ι"ÿÿÿΊκÿÿÿι9ώÿÿθόÿÿÿ΄&    t& θόÿÿÿS¶\$¶Ι‹@,SQΉ   j θ±ύÿÿƒΔ[Γ΄&    t& θόÿÿÿ¶Ι‹@,j QΉ   jθ†ύÿÿƒΔΓfθόÿÿÿS¶\$‹@,SQ1Ιj θgύÿÿƒΔ[Γfθόÿÿÿ‹@,j Q1ΙjθLύÿÿƒΔΓ΄&    θόÿÿÿWVS‰ΣƒμT‹p,d‹    ‰T$P1Ò…Ϋ…‹   ¶VL‹FHjL$θΉϋÿÿ¶VL¶D$Ή   ‹~HΗD$
   ƒΘ`f‰T$T$D$‰T$T$D$‰ψΖD$ θόÿÿÿYƒψu‰Ψ‹T$Pd+    u*ƒΔT[^_Γt& ‹°  …Ò…Εx  »ÿÿÿÿλΡv ΈκÿÿÿλΙθόÿÿÿt& θόÿÿÿVSƒμP‹p,d‹    ‰\$L1Ϋ…Ò…®   …Ιur»   Έ   8^htF‹°  …Ι…ηx  ¶VL‹FHΉ   \$ΗD$   f‰$T$‰T$‰βΖD$θόÿÿÿƒψuH^h‹FP‹T$Ld+    uLƒΔP[^Γ΄&    ƒ=¬  ΫƒγόƒΓƒ=¬  ΐƒΰόƒΐιuÿÿÿt& ‹°  …Òt±ι	y  Έκÿÿÿλ§θόÿÿÿt& θόÿÿÿ‹°  V‰ΖS‰Σ…Ι…ey  1ΐχΓ  €tχΓ  € uEγ   uΈ   [^Γt& ‹8  ‹    Έ    ƒΑTθόÿÿÿ΅°  …ΐtΡι(y  ΄&    f‹†τ  ‹    HTΈ    θόÿÿÿ‹°  …Ò…Iy  γ   t”λ΄&    ΄&    θόÿÿÿUWΏ31  V‰ΖS»   ƒμ(‹-°  d΅    ‰D$$1ΐƒ=¬  ΗD$    ΐΗD$ D  ƒΰόΗD$
    ƒΐΗD$  D ΗD$    ΗD$   f‰\$ΖD$ D$ΗD$  f‰|$"…ν…Ίy  ΅   Ήt   Ίΐ  θόÿÿÿ‰Γ…ΐ„y  ‰†  ‹Fƒψ„T  ƒψ…  ΖCLAΊA   ‹ƒ=¬  Η    ΗC    Έό   ΐ1ΙΗC    ƒΰό‰{HƒΐΗC    ChΗC    ΗC    ΗC     ΗC$    ΗC(    ‰[,‰sdj"D$P‰ψθ±φÿÿ¶SLΉ   ‰Η‹CHjl$&Uθ—φÿÿ¶SLΉ   	Η‹CHjUθφÿÿƒΔ	Ηuzj1ΙF(‰ΪθόÿÿÿY‰Η…ΐueCXΗCTΰÿÿÿKT‹    ‰CX‰C\Έ    ΗC` Ί  θόÿÿÿ‹°  …Ò…Φy  ‹D$$d+    uNƒΔ(‰ψ[^_]Γ¶    ¶SLιεώÿÿ΄&    ΅°  …ΐ…y  ‰Ψθόÿÿÿλ½΄&    v ΖCL@Ί@   ι°ώÿÿθόÿÿÿƒ=°   ΏτÿÿÿtΜιy  ΄&    v θόÿÿÿ…ΐt/S‹  …Ϋt‹C,…ΐt‰Ψθόÿÿÿ‰Ψ[ιόÿÿÿ¶    [Γ¶    Γfffθόÿÿÿƒμ d‹    ‰L$1ΙΉ   T$‰βf‰L$L$‰L$Ή   f‰L$L$‰L$Ή   Η$P   ΗD$P  ΖD$ θόÿÿÿƒψ…ςy  ¶D$‹T$d+    uƒΔ Γθόÿÿÿ΄&    ΄&    θόÿÿÿSƒμd‹    ‰\$1Ϋ»   T$‰βf‰\$\$L$Ή   Η$P   ‰\$θόÿÿÿƒψ…z  »
   ΄&    ΈX‰A θόÿÿÿƒλuρ1ΐ‹T$d+    uƒΔ[ΓθόÿÿÿθόÿÿÿW‰ΗV‰ΦΊ?   S»@   θόÿÿÿFf‰Ϊ‰ψθόÿÿÿDΐƒΓƒϋFuλv ‰Ϊ‰ψθόÿÿÿDΐƒΓƒϋLuλ[^_ΓfffffffθόÿÿÿWΏ   V1φS‰Γ΄&    Ή   Ί   ‰ΨφθόÿÿÿΊ   ‰ΨθόÿÿÿΈRχ θόÿÿÿΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈRχ θόÿÿÿ1ΙΊ   ‰ΨθόÿÿÿΊ   ‰Ψθόÿÿÿ	Ζ‰ψ,‰Ηu‰π[^_Γ΄&    ΄&    θόÿÿÿWΏ   V‰ΦS‰ΓλEt& θόÿÿÿΈRχ φθόÿÿÿΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈRχ θόÿÿÿ‰ψ,‰ΗtXΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈRχ θόÿÿÿΊ   ‰ΨΉ   θόÿÿÿ‰πΊ   „ΐ‰Ψ‰wÿÿÿθόÿÿÿιrÿÿÿt& [^_Γ΄&    t& θόÿÿÿWΏ8  VSƒμd‹    ‰L$1Ιf‰<$ΖD$ƒϊ„I  ƒϊ„p  …Ò„P  ‹€$  L$Ί   ‹@ ‹‰ΰ΄&    v Pƒΐ9ΑuφχΪΉ   ‰ΨT$Ί   θόÿÿÿΊ   ‰ΨθόÿÿÿΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈRχ θόÿÿÿΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈ   θόÿÿÿΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈ   θόÿÿÿΊΰ   ‰Ψθ2ώÿÿΈ   θόÿÿÿΊ   |$t$λ΄&    f¶ƒΗ‰Ψθώÿÿ9ώuοΉ   Ί   ‰Ψ1φθόÿÿÿΊ   ‰ΨθόÿÿÿΉ   Ί   ‰ΨθόÿÿÿΊ   ‰Ψθόÿÿÿv Έ   ƒΖθόÿÿÿ1ΙΊ   ‰ΨθόÿÿÿΊ   ‰Ψθόÿÿÿ„ΐt‰π<vΟ‰π<„z  Ή   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈ   θόÿÿÿΊα   ‰ΨθRύÿÿΈ   θόÿÿÿ‰Ψθ΅όÿÿΊ   8Π‰ΧFψ1φv ‰Ψθ‰όÿÿ‰π‰ωƒΖ8ΘuξΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΉ   Ί   ‰Ψθόÿÿÿ‰ΨΊ   θόÿÿÿ1ΐ‹T$d+    uNƒΔ[^_Γ¶    »  ΖD$f‰\$ι΄ύÿÿt& Ύ  ΖD$ f‰t$ιύÿÿt& 1ΙΖD$ f‰L$ι‡ύÿÿθόÿÿÿfθόÿÿÿ1ΐΓ΄&    θόÿÿÿ‹P‹H|‹R‹AP·Òƒθ!Π―ALΓfθόÿÿÿ‹@|4  ‰Πιόÿÿÿ΄&    t& θόÿÿÿUWV‰ΦSƒμ‹X{‰ψθόÿÿÿ…φ„=  ½κÿÿÿƒώ…  ‹3‹k ‹†d  ‰D$  ‰$΅΄  ƒΐƒψ‡7z  ‹†ά   @  ‹ƒβξ‰ÿu‹M ‹T$‹θόÿÿÿ‹†ά   ‹U ‰P  Ί   ‹†ά   ‰X  Ί   ‹†ά   ‰0  ΅΄  ΗC    Zƒΐƒψ‡  ‹†ά   Ί ‰P  Ίÿÿÿÿ‹†ά   ‰T  ‹†ά     ‹*‹‹δ   ‰ΘƒΘχΠ!θ	ΘƒΘ‰‹†ά      ‹ƒΚ ‰‹†ά   @  ‹€@  ƒΘ‰‹5΄  …φ…‹   1ν‰ψθόÿÿÿ‰θƒΔ[^_]Γ΄&    ΅΄  ‹3ƒΐƒψ‡Uz  ‹†ά   @  ‹ƒβξ‰‹†ά     ‹ƒβχ‰‹†ά   P  ‹βώοώÿ‰΅΄  …ΐt‹–d  ‹1νΒ  θόÿÿÿιxÿÿÿ‹$‹1νθόÿÿÿιfÿÿÿ‹D$‹S‹M ‹€  ―Κƒΐƒΰό†ά   ‹ ΡθQRPÿu ‹Ό  Ph<;  θόÿÿÿƒΔιΆώÿÿ΄&    t& θόÿÿÿUWVS‹h‹X|…ν„mz  ‰ΗΉ   1Ò‰Ψθόÿÿÿ‰Ζ…ΐ―   ‰}$1ΐ»ψ   Ή   σ«Ζƒ)  1φΗƒψ    Ηƒό      Ηƒ       Ηƒ  €   Ηƒ  €»  Ηƒ  €»  Ηƒ     Ηƒ     Ηƒ    Ηƒ!    ‹E ‹€d  ‹€  =   tΑθƒΰψ‰ƒ  ‰ƒ   [‰π^_]Γ΅΄  ƒΐƒψvλι‚z  ΄&    ΄&    θόÿÿÿ‹P…Ò„°   VS‰Γ΅΄  ƒΐƒψ‡z  ‹s ‹N …Ιua‹FθόÿÿÿΗF    ‹FθόÿÿÿΗF    ‹S ‹C‹J‹rj ÿr   ÿr‰ςθόÿÿÿ‹C θόÿÿÿΗC     ƒΔΗC    [^Γ΄&    ‹V‹N$‹Cj j   θόÿÿÿΗF     X‹s Zιwÿÿÿ΄&    ΄&    ΄&    θόÿÿÿS‰Γ‹@|‹€4  …ΐt‹Cθÿÿÿ‹C|Η€4      1ΐ[ΓfθόÿÿÿUW‰ΗVS‰Σƒμ‹p‹@|‹€4  …ΐt‰πθΩώÿÿ‹G|Η€4      ‹ƒL  ‰F‹“X  ―Β‰V‰F…ΐ„Τ  Bÿ…Π…Ρ  ΅   Ή(   Ίΐ  θόÿÿÿ‰Γ…ΐ„µ  ‹F‰‹F‰^ ÿ  ‰Να πÿÿ‰Θ‰$Ανθόÿÿÿ‹$…ΐ‰C„  ‹΄  ƒΒƒϊ‡µz  1Òθόÿÿÿ‰k$Dm Αΰθόÿÿÿ‰C…ΐ„  ‹S$θόÿÿÿ‹C$…ΐtU1νλ#΄&    f	ΒΗA    ƒΕ‰ΗA   9k$v.‰θΑΰCθόÿÿÿ…ΐ„±   ‹KTm Ρ‹ƒβ¨tΎt& ‹n ‹F‹M$‹Uj    jθόÿÿÿ‰E Y]…ΐ„άz  ‹K‹FjSÿvÿvθόÿÿÿ‰ΒƒΔ…ΐxu‹CΗ   q‹C‹S‰P‹CΗ@    ‹V ‹G|‹R‰4  ‹G|‹V‰@  ‹G|1ÒΗ€8      Η€<      λ7‹CθόÿÿÿΗC    ‹CθόÿÿÿΗC    Ίτÿÿÿ‰Ψ‰$θόÿÿÿΗF     ‹$ƒΔ‰Π[^_]Γ΄&    ¶    Ίτÿÿÿλή΅΄  ΊτÿÿÿƒΐƒψvΈιχz  θόÿÿÿ1ΐΓ΄&    θόÿÿÿUWVS…Κts‹¨0  ‰Ζ‰Σ‹€ά   ‰T  χΒ   …{  φΗuƒγΏ   uR[‰ψ^_]Γv ΅΄  ƒΐƒψ‡V{  ‹†ά   Ί   ‰0  Ώ   [^‰ψ_]Γ΄&    v 1ÿ[^‰ψ_]Γ΄&    ‹†ά   ‹€   ‰E‹E$θόÿÿÿ‰ψ[^_]ΓfθόÿÿÿVS‰Γƒμd΅    ‰D$1ΐ΅Έ  …ΐ…+  ‹ƒd  ‹€  …ΐ„p{  ‹ƒΜ   T1ΙΊÿÿÿÿj(h       θόÿÿÿƒΔ…ΐ΅{  ‹$T$1Ι‹°  ‰‹ƒΜ   ΗF    ‰F‹$‰FRΊ  jj θόÿÿÿƒΔ…ΐ°   ‹D$Ή   Ί  ΓΌ  ‰°Έ  ƒΐXθόÿÿÿ‹D$Ή   Ί   θόÿÿÿ‹$Ή   Ί)  ƒΐθόÿÿÿh1  ‹D$ƒΐ$Pθόÿÿÿ‹D$SP$ƒΐDRhB  Pθόÿÿÿ‹D$θόÿÿÿƒΔ…ΐx-ƒ=΄  ÿ…{  ‹D$d+    u ƒΔ‰π[^Γ΄&    1φλΰ‹$θόÿÿÿι΅{  θόÿÿÿ΄&    v θόÿÿÿ‹€0  ‹@ιόÿÿÿ«©ÿÿƒω‡  ΊΣκ€β„  ÿt$,Sh8   θόÿÿÿƒΔ‹D$Dd+    u=ƒΔH[Γ[ΝώÿΊ @Σκ€β„  λΔύζώÿΊ Σκ€β„  λ¬ªÿÿλ”θόÿÿÿθόÿÿÿVSΌ  ‹€Μ   ƒx$ uSSSSShd   θόÿÿÿƒΔλSSSShD  θόÿÿÿƒΔS1φh  θόÿÿÿXZiΖ  ÿ°    VFSh\  θόÿÿÿƒΔƒώ?uί[^ΓQΎκÿÿÿh   h€  θόÿÿÿƒΔιm  €=    Yt}h   h¨  θόÿÿÿ‹ƒΈ  _Y‰Βι  Ά   ¶   Έ"   Ύ   €ϊEπVRh  θόÿÿÿ‹‹Μ   ‰ςD$Α   θόÿÿÿƒΔ…ΐuP‹$D$θόÿÿÿ‹$θόÿÿÿιB  ÿ5#   iƒΈ    ÿ°    ƒΌ  Phθ  θόÿÿÿ‹ƒΈ  ƒΔ‰Βι  Vh<  θόÿÿÿ[^λ­‰$ÿsSh4  h  θόÿÿÿ‹T$ƒΔι‚  ‰$ÿsSh4  hP  θόÿÿÿ‹T$ƒΔι‚  h   h   hµ  θόÿÿÿƒΔι=   ‹†θ  ÿ³  ÿ0ÿpÿ¶π  ÿ¶μ  ÿsSh  h(  θόÿÿÿ1ΐƒΔ$ι¶!  h  h  hΔ  θόÿÿÿ‹ƒ€  ƒΔι`"  h  h  h  θόÿÿÿ‹ƒ€  ƒΔι#  ht   ht   hµ  θόÿÿÿƒΔιj$  QRPht   ht   hx  θόÿÿÿƒΔιξ$  Έ0  θόÿÿÿΈ(  θόÿÿÿÿv³ύÿÿWÿ³ύÿÿVh¬  h°  θόÿÿÿƒΔι'  hΜ   hΜ   hµ  θόÿÿÿƒΔιc'  Έ0  θόÿÿÿΈ(  θόÿÿÿQhδ   hδ   hδ  θόÿÿÿƒΔιI(  h`   h`   hµ  θόÿÿÿƒΔι{(  h  h  hµ  θόÿÿÿƒΔι)  Ph  h  h  θόÿÿÿƒΔιΥ)  h   h   hµ  θόÿÿÿƒΔιΤ+  h€  hθ  θόÿÿÿXZι·,  R‰T$hΤ  hΤ  hd  θόÿÿÿ‹T$ƒΔι-  hΌ  1φh  θόÿÿÿXZƒ=x   uƒΖƒώ$uοι-  ‰ς‰Ψθόÿÿÿ¶ΐPVhΌ  h°  θόÿÿÿƒΔλΡi€Έ    kÒ„    ÿpDÿp@ÿp<ÿp8ÿp0Vhμ  hμ  hΨ  θόÿÿÿƒΔ$ιχ-  R‰T$h¨  h¨  hδ  θόÿÿÿ‹T$ƒΔιδ/  Rhψ   hψ   hÿ  θόÿÿÿ‹ƒΔιd0  hl  hl  hµ  θόÿÿÿƒΔιΊ0  Έ0  θόÿÿÿΈ(  θόÿÿÿh¬   h¬   hµ  θόÿÿÿƒΔιν1  Ph  h  h  θόÿÿÿƒΔι%2  Sh   h   hÿ  θόÿÿÿƒΔƒϋ†S2  ƒ=x   „2  h   h   h(  θόÿÿÿΈκÿÿÿƒΔι2  ÿt$ÿt$ÿ3ÿt$h  h  hL  θόÿÿÿƒΔι?4  h  h  θόÿÿÿ[^ιω4  h   hx  h\	  θόÿÿÿƒΔι¬6  ƒ=x  vh   hx  h΄  θόÿÿÿƒΔ†Ό  Phθ  θόÿÿÿ‹–d  ‰πθόÿÿÿXZι6  h   hx  h	  θόÿÿÿƒΔι–6  h\  h\  hµ  θόÿÿÿƒΔιR7  ‹“ΰ  ‹‚  …ΐ„ά   PVhΔ	  θόÿÿÿΉQ  Ί@  ‰Ψθ\(  ‰ƒδ  ƒΔ‰¨Έ  ‹ƒδ  Η€δ    ‹ƒΈ  ƒψ4tƒθ8ƒψ‡”   ‹ƒδ  δ      ‹ƒδ  ‹“  ‹• J  ‹ΰ   ÿ2Ί   jθόÿÿÿY_…ΐy‰$Vhπ	  θόÿÿÿXZ‹$ι¤:  hD  hD  hµ  θόÿÿÿƒΔι8  ‰$Vh	  θόÿÿÿ]X‹$ι¤:  ‹‚θ   ιÿÿÿƒ»€  ‹ƒδ  „oÿÿÿδ      ‹ƒδ  ιZÿÿÿ‹“δ  ‹‚  …ΐt#PVh
  θόÿÿÿ‰Ψθόÿÿÿ1Ò‰ƒ0  ƒΔι±:  ‹‚θ   λΥÿsShΈ  hD
  θόÿÿÿƒΔιk>  ÿsShΈ  h|
  θόÿÿÿƒΔιk>  h   h   hΠ  θόÿÿÿƒΔι*@  hΘ  hΘ  hΈ
  θόÿÿÿƒΔιgA  θόÿÿÿUW‰ΗΑοVS‰Γ‹½   …ΐuΈι  PΎ   ½   Shρ  θόÿÿÿƒΔ‰θ‰ρΣΰ…Ψtÿ4µ  hÿ  θόÿÿÿXZNƒώuέγÿ  Sh  θόÿÿÿ‹½€  ZY…ΐuΈ   [^_]ΓPÿt$ÿ³ύÿÿUhΔ  h  θόÿÿÿƒΔι*F  Pÿt$ÿ³ύÿÿUhΔ  h  θόÿÿÿƒΔι*F  ÿ2hx  hx  hL  θόÿÿÿƒΔιdK  ‰$ÿ2hx  hx  hx  θόÿÿÿ‹L$ƒΔιTI  ‰$‹L$‹AθPVhx  hx  hΤ  θόÿÿÿ1Ò‹L$ƒΔιΩI  ÿt$ÿt$ÿ7ÿ³h  hx  hό  θόÿÿÿƒΔιBK  θόÿÿÿU1νW‰ΗV°Ό  S‰Σƒμÿ2Vh8  θόÿÿÿƒΔ‹Cθƒΰό‡ά   ‹ PƒΕÿµ<  Vhh  θόÿÿÿƒΔƒύ8uΣ1ν‹CD¨8ƒΰό‡ά   ‹‰$UEVh  θόÿÿÿ‹D$θώÿÿƒΔƒύuΝ‰4$‰ω1ν‰ί‹G­    ‰L$Ψƒΰόά   ‹0U_S»   ÿt$h  θόÿÿÿ‰πuθΜύÿÿ‰Β‰l$‹l$ƒΔ‡Χ9ϋs1‹B‰T$°ƒΰό…ά   ‹ SCPVFÿt$h΄  θόÿÿÿ‹T$ƒΔλΛ‰ι‹l$‡ϊΥƒύ†lÿÿÿ‰ϋ‹4$‰Ο‹C‹SΒRPVhΰ  θόÿÿÿ‹CP`RPVh  θόÿÿÿ‹Cƒΰό‡ά   ‹ ƒΔ PVh0  θόÿÿÿ‹Cƒΰό‡ά   ‹ PVhT  θόÿÿÿ‹C ƒΰό‡ά   ‹ PVhx  θόÿÿÿ‹C$ƒΰό‡ά   ‹ ƒΔ$PVh  θόÿÿÿƒΔ[^_]ΓχΓ   t ƒ=„  vh   h¨  hΐ  θόÿÿÿƒΔχΓ   t$ƒ=„  †Η   h   h¨  hπ  θόÿÿÿƒΔ‰έε   φΗt ƒ=„  v;h   h¨  h   θόÿÿÿƒΔ…νt ƒ=„  vh   h¨  hP  θόÿÿÿƒΔ‡Ό  Ph€  θόÿÿÿ‹†ΐ   ƒΰό‡ά   ‹‹–τ   χÒ!Κ‰kV,‰ψ—d  θόÿÿÿXZι)L  jh¨  h¨  θόÿÿÿƒΔιωK  ‰έε   φΗu”ιkÿÿÿhd  hd  h”  θόÿÿÿƒΔι‹Q  kV,‰Ψ“d  θόÿÿÿ‹Ό  ‰$ÿ·  ÿ·  Ph   θόÿÿÿ1ΐ‰\$ƒΔ‰t$‰Ζ‹‡„  Αθ9ΖƒΉ   V»   ÿt$h.  θόÿÿÿµ    ‰D$‹‡  ‹°θϋÿÿVƒΔ‰t$‰Φ‰l$‰ύ‰Η‹…  9ί†e  SƒΓÿ4°VƒΖÿt$h0  θόÿÿÿƒΔλΤÿ³τ  ÿ³π  ÿ³μ  hd  hd  hΤ  θόÿÿÿƒΔιnO  PRhd  h\  θόÿÿÿΈκÿÿÿƒΔιiR  ‹\$‹t$ι½O  ‹.hL  hL  h  θόÿÿÿƒΔƒ=„   „CR  ‹…ά      ‹ PhL  hL  hΔ  θόÿÿÿƒΔƒ=„   „CR  ‰θθ¬A  PhL  hL  hό  θόÿÿÿƒΔƒ=„   „CR  ‹…ά   P  ‹ PhL  hL  h4  θόÿÿÿƒΔƒ=„   „CR  ‹…ά   @  ‹ PhL  hL  hl  θόÿÿÿƒΔƒ=„   „CR  ‹…ά   `  ‹ PhL  hL  h¤  θόÿÿÿƒΔƒ=„   „CR  ‹…ά   @ ‹ PhL