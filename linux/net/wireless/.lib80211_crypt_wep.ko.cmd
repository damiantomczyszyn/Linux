dth_count_to_ns(FIFO_RXTX, *divider);
}

/*
 * IR Tx Carrier Duty Cycle register helpers
 */
static unsigned int cduty_tx_s_duty_cycle(struct cx23885_dev *dev,
					  unsigned int duty_cycle)
{
	u32 n;
	n = DIV_ROUND