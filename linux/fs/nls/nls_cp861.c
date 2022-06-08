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
	control_rx_i