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

static const struct v4l2_subdev_ops cx23888_ir_c