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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 x—ppFåâì²`V"†N,v0 ½vµf-ã© 0h|ˆï]ÇÒ”½{3§6Ç—=–bb)ûwYjv€ÌÎ•ú=ßıõjeñû²˜ğû›¶\È­½b>åĞÑm !LĞö‹¸ĞVĞ?÷Â­sÃJÚ®•®¼s	Û¡C‹ÿz1¶pS•Ë(¶üPå¸j@ßb›¢ç"ÖĞ-n³`´ó¢?Û5‚ÔFYÜªD @ş	Ó
û¶dö'©™–ÃTŸõQ“JÑuaº\ªÒæ~
¤xO]æ›8Q	Á›ãs?åJLQ¿·ÊP—ûŞAR6èbğÆ?!ŠH&H3Ô>5`$_¡{Íé•vİ4-Rª-²Å& G5àÉs´9IÁë†d.'ç»yw´¢§{B†$7‰<­ËBGËÉïòä³ºç£"BÕÂ™¹İ{„+ĞKğâÑ6*šüeHës „PNá89ùQ!¦…>I™æwÙ83FåsÙDòÎí·Ò¡áæµú ÚÊFß¸¶:*Ÿ²Ê<¾Uü„qÊw¦3"gx$ÈÄ 	ûİùè9Ëàêìig³Öî1¼u+‚º­ÉX©éÀÿ¶Äêè¹iİ*kñwX\y`Ä„m7kî)ÍvïúV\ùcá.I&×9;GW9	ìL{`^&ƒ¦6¹Í>VÎ:Çu™}.±Œ_è:Åª…$²{X+İ Â%şÃ|åíÏ'„¥fô â”à1ğßëêú-}¸»kÁnÌÀÖ¡©RYTÖsÜ¾•Ã-NÛ3¡™À¼ª…u‹rl)8ŸøÀÂ¹¹ÓèXÒHˆÖºC"CÀGA³s·&•¢Cù/^p]Ç±ì
e,ßRQÇZëdqí{ŠYSšÂ_à&z©åUâª{ú1÷‚„7G³Úô¿î¬[å™3>Öm•ŠÒş©ş `$Û·cÍn‘3ü™™õ7+@j?æğd{NùV,sÓáüü¨§ñÛÿ2 7'mİşE)']£‘ğLGÒ\–s¢GRÄ»+Èx£89FK}¦€1-ÄĞúp¯zÆ×›s}ü¦:¶5ÚÌp ê:Zôc™ôE} 'ú¼@ƒı'"hKƒ£’G!xqƒ9À]ù'*ıe¹ó˜3+ÂC+UÌá8×ì=
JóşêO3Z\~$"v'F8İ—›Â	)Oâ¡Q\P!šòõ›/’ª
o¹äwéQÍ7œ¬Í_¼İŒ|½´Ş¬Õ‹æÙ¡ÒÔª6t÷gP/¼iviÃ/­/…ŸÇúc5(¿ŒÉVUI0V]Zİcî(¥™øò"õÛèvÅE˜úÉ;ËßŞ/
{wÿn‹–rc-Ù:†µ	õ§]¯¾:ÿ‡µ7¶€:«‘|¹µæîIxßç&fşoœÖ)ŠŠ[ïS¹<![Œxx`N?û	Täæ¸YAlÃ Ãx‹äÏî&‡Ü\²ËÇvşxÄLÔNÍ´\HuV÷Qâzî)hÆ*´W}ÄbF~Â9›SÄ{be»rÂ†\¥{ø’5~­‰V8ÍiÆÊ¯B©•£Ú-R´Ò°â$°Åx0L4èŠîEº8¯Á’qc	Ø´Ï{ÿ³t³;^™Ğ(¦Múh¶KP™¸k­Z‡9ë`}~Ë4|\N¼	#yµ9WÎ`y·Pm|šV’	Œ¡w0QŠoü(	ze¯Éétª¯Mƒó<<"
Ï0fÄ·Íæğ?¦}Fó¶<t!»LêA“Ğ[ƒXMĞ¬ˆÊƒ±_Pİ¹ìeÉØ	ª4|1ÿM§z İyóÀ Òfù³ï F²šÏ!-oE'¨gämÍœ‘q­ Ñò–^ã©‰œ£„7æ]‘†ğ
l÷ÃnÆéy2…d×¹	>•Z„<Fı[W€¥SçX»€ÖîĞ‰RÚi¦¬³ZZ>?6bÛ³¿×)XFŒƒ¤CÚªÃ
²"×ÖV´é!¬§ö/Íj^Î3J5v¤$²Ãûãõ‡'Ğª-Ú*"¯ÛwL“Vl{bä¦Ê*uö¿‹¿l{jœE b¢ÜÇùŠ©^ÇÂ>…a¡&Y„Á¥?§‰+I«ŠFğ:[²Ùƒ’*o#°ciÌvè ¤_=é©Š#<`€®# ±+—4N‹æ¥ ¦¶u•§›šPQM5–c%Å§'ğŸ¨½4µ>+ñ8¡A_gãéŠ
š'ZŠGWÅøık“ëâuñæ"‡À7Òj+¹‡‰g÷‘l-\˜°xYyZxÁÓM«4S	Sİš/$ª¼¢‘µ´CexnF¥##ºNbÅ¯Øâ„‰¥™'dÆ£6])Û#áû[9M_5³oÎ?øõ¶›yùèÆ(¬~´½£·JQ#Dâ‚X©)3
«şÔ	¹ù|Gæ¼âˆ¥ .ézÂ6Ø¡•_¥Sö|)ø¦ª»ğeµ‹èÙ——(_{4„hø–ÿÔßeÛ¦ªèîtœuÇL¢µ»Šú4Íf£CL•`u8sÍ_ÛÆš].ø¿’°ÒÙ³ğúoz±Z	Á±Lu.¬ê+Û§«²™øgêŒê%äêæ¡3˜'©Íõ·0ˆJÛKı‘ Ö+E—'/dNóİÇ`ñ{UZKjØÃˆõóE°‚ça¹´¾TÇYÅ}ı¡¢ZØb;™XƒÀ7ôde2…föSESVe¡Bn:¨/€¼ÿC¼÷ˆŠ¹ 6Ç<l”ø?t$æ/ëc›ÂÁ§zÇàdcNWD…õ3{‘ûµ¬Ñ¡}æÃnl<š Ş%İ·!§\ø±*H¾H şü>ˆƒárá)p*õl™İ.-u IR”;rî—ó.—'0±¥¨é˜¨ç0Š#X„;lx®s\	‰Ä\·†5)¥ğ4½¦^ÒšdêéÔÍ$×W¬£õÅµ÷K™–]/šË6=iäòM;Ï‡˜½’ÃBş@ŒKCó% ¥lBvÿ)‡Î-´4§È•'ìí$æÊßÒP%-É[­7Ñó·Â¼BĞ.4·.ÆåvmÍ»ÍpŒ?¹|ÙÜ?XİÂ³)r—~‡Á.Ñ	³s(1ßÆtÕË´‡öuØÿ¼TS_3L«OL¶Í‹Õš#•BÑ6sº' à-uhº¦ZéÄl7vwˆ„Î–Ììyña¼¹›iÔª.wqâEö3.÷~ÏŸĞê/Î¿½9‰HŸ}v`Âvcñë\dü-kŒš­16m¹ık\š§8u§y¢;ÏQ”9á÷âá‰¤Ã{.İÃL]uªîÈ0æ›©yQ´+K†Ey—QAPTgJlÑwú9@£´¢­(ú¨óUŠÉ¶¾«±
Wş·’ñ¸êÍMz)Şd+…¢,õ‚’=ÿ‘“e¿×àöŠ¦Æ¾Ë
ÔğlŸÁÛHÜ%oı™6˜ÛâQ×‹*¤„Š]!ÕT£àÊóÀä:¬×Hë ÀÌÖ¡SbŞ„%
™oôÏúôŸãŒG±?b‚~¦çÊu*í¾?T™Ú†.âeZÏ|Ìæ:OZt¬´#(¼öâşmm}4db‘Ò×ªŞÎŠ„€54]RdâŒJKÛ†oß¯–ÖBƒ¯FÛ®¦Õ›OÖNGä¼üf®X‡YáÀ²¶ä;.­Jİ5£jl¼*¨~ï†·Òã±2Oõ;@FO¬B–=òº<qPÓÂ«‹†“İ¹ëfn„zÕÚ§3ĞÏí.öbskƒüÒTğÌìçÎ³8—#´Ê°üK	ÎÉØH{¤÷»ŠÏİèãx,
•ú~™L—{ü¼.	-m2s“{i»œƒgë	J”•ÃZÌ½ÔZ)VØ¢˜šŞ†òÄ²2y"šR±­ø¤·´M%°¹Ë¥£rÛzqå&†Ÿ•ŠïÏØµÄ¡W§>ÿ%?Ù'ÛÖ´Õ–4¹¡¸MÛïC*4—<8šúÖY"©TìÔ4Ò½h
BÄB;3²¢cæ&ïÇßÁ½.¼iºùœm .[³–…*]Mó¶œj8XÓoi°	ì¾VöA2 NYşèG®úê‹3ìaã°‘…*iÃ©¤ùo>uå7“(ÆCn]K¨˜±¾Å@;ù¹cî3º–ƒàG9ŸÃëÏº#‰R¼³ßˆ5s¨ÙhYXÅP^T`µI-·ó[œ€K2J—=”×O©?2úËÿCÑh=dÑõ›î]ŞËupÕñîËü¸cY†¢©¥±b°}H´Cq‰ošgÚğ#ãÌ¨æGöÀtXaÎ ZØŞıê¸yõb;ùïm@]oşºS¨˜ÆÛAG‰$ã/…êğĞGÿOñ©ušhQ>dÅx„pA(hç9&È65ïï–sÉ&š“Ñš):cv¯[¸ÿÔ›“Óú_[:`	±»á±_<L@Ù|-h<Ş˜;!½j³«ÒqiGŠø¸¾@SmZÙ¬G…ØÈšºµ²€áĞÜHF$+#f€ªü5ix(!ídq†ı%1lİóàß÷nYFÂñş¨( ã½Ã	àB¢äc¢`…Ë™Â.>}9Åw	Ã"s*)q@p–ñFšâ€o6õ„‡x.Š3û‹„dµV=5Ks˜ìáù£¨¥~Âh"øÆJ“QÉ},• Ïdœ=UÆUDª,?ë^ŠeM{#:Ñ^Í—¯ı:ğeFLgô}ô½´dûB¦—pçøĞŸÛŸß@¤‘œ#©è‰—œIîîÕ7ªî*İ:_w²V•’kó1c×™&öËU y±~ñ…0ıYï.u’µş°%,(K±•Ë‰]k^L}O´]úL¾¡»t37ö'–ŸÃá\ë“]ö›—à1ke»Â+\Ó„ĞëN3&VÄ|•uª®-)ØŸ>‹û'õé$›3Üô×ûY€y•Ñ •?*øª'²yÆK°H¸fedKÎ¡Æ²úÂn=ÄÃªw¦M-Ùâ‚û.d”±.ã¸Ódå Ù´#ÔÌW‚ÔX ›¸Ã/wo¯ÆU^ Î-Š´?ªb>oPßëwZK.š$X½ä¤HâGBü…:™h<rùK¿?EzîYò:[Åì2p5öÒ'7üšëYúÑ'/£&Za˜¢ ¯ãÖM#‹+®{~vîk]¸pî„Ø>¤vzÿÙ)ò-;-@à¬Ì)®²KRO'Áœ<®[øÏä@ã8-f 6–2vÚiı«î°¿L0âøå$²«P¶0dÏìu0ß#dşc` >Y#°ˆÅ¬K¾oX9Ïâô|\ng tasks to offline CPUs is a fairly daft thing.
	 */
	WARN_ON_ONCE(!cpu_online(new_cpu));

	WARN_ON_ONCE(is_migration_disabled(p));
#endif

	trace_sched_migrate_task(p, new_cpu);

	if (task_cpu(p) != new_cpu) {
		if (p->sched_class->migrate_task_rq)
			p->sched_class->migrate_task_rq(p, new_cpu);
		p->se.nr_migrations++;
		rseq_migrate(p);
		perf_event_task_migrate(p);
	}

	__set_task_cpu(p, new_cpu);
}

#ifdef CONFIG_NUMA_BALANCING
static void __migrate_swap_task(struct task_struct *p, int cpu)
{
	if (task_on_rq_queued(p)) {
		struct rq *src_rq, *dst_rq;
		struct rq_flags srf, drf;

		src_rq = task_rq(p);
		dst_rq = cpu_rq(cpu);

		rq_pin_lock(src_rq, &srf);
		rq_pin_lock(dst_rq, &drf);

		deactivate_task(src_rq, p, 0);
		set_task_cpu(p, cpu);
		activate_task(dst_rq, p, 0);
		check_preempt_curr(dst_rq, p, 0);

		rq_unpin_lock(dst_rq, &drf);
		rq_unpin_lock(src_rq, &srf);

	} else {
		/*
		 * Task isn't running anymore; make it appear like we migrated
		 * it before it went to sleep. This means on wakeup we make the
		 * previous CPU our target instead of where it really is.
		 */
		p->wake_cpu = cpu;
	}
}

struct migration_swap_arg {
	struct task_struct *src_task, *dst_task;
	int src_cpu, dst_cpu;
};

static int migrate_swap_stop(void *data)
{
	struct migration_swap_arg *arg = data;
	struct rq *src_rq, *dst_rq;
	int ret = -EAGAIN;

	if (!cpu_active(arg->src_cpu) || !cpu_active(arg->dst_cpu))
		return -EAGAIN;

	src_rq = cpu_rq(arg->src_cpu);
	dst_rq = cpu_rq(arg->dst_cpu);

	double_raw_lock(&arg->src_task->pi_lock,
			&arg->dst_task->pi_lock);
	double_rq_lock(src_rq, dst_rq);

	if (task_cpu(arg->dst_task) != arg->dst_cpu)
		goto unlock;

	if (task_cpu(arg->src_task) != arg->src_cpu)
		goto unlock;

	if (!cpumask_test_cpu(arg->dst_cpu, arg->src_task->cpus_ptr))
		goto unlock;

	if (!cpumask_test_cpu(arg->src_cpu, arg->dst_task->cpus_ptr))
		goto unlock;

	__migrate_swap_task(arg->src_task, arg->dst_cpu);
	__migrate_swap_task(arg->dst_task, arg->src_cpu);

	ret = 0;

unlock:
	double_rq_unlock(src_rq, dst_rq);
	raw_spin_unlock(&arg->dst_task->pi_lock);
	raw_spin_unlock(&arg->src_task->pi_lock);

	return ret;
}

/*
 * Cross migrate two tasks
 */
int migrate_swap(struct task_struct *cur, struct task_struct *p,
		int target_cpu, int curr_cpu)
{
	struct migration_swap_arg arg;
	int ret = -EINVAL;

	arg = (struct migration_swap_arg){
		.src_task = cur,
		.src_cpu = curr_cpu,
		.dst_task = p,
		.dst_cpu = target_cpu,
	};

	if (arg.src_cpu == arg.dst_cpu)
		goto out;

	/*
	 * These three tests are all lockless; this is OK since all of them
	 * will be re-checked with proper locks held further down the line.
	 */
	if (!cpu_active(arg.src_cpu) || !cpu_active(arg.dst_cpu))
		goto out;

	if (!cpumask_test_cpu(arg.dst_cpu, arg.src_task->cpus_ptr))
		goto out;

	if (!cpumask_test_cpu(arg.src_cpu, arg.dst_task->cpus_ptr))
		goto out;

	trace_sched_swap_numa(cur, arg.src_cpu, p, arg.dst_cpu);
	ret = stop_two_cpus(arg.dst_cpu, arg.src_cpu, migrate_swap_stop, &arg);

out:
	return ret;
}
#endif /* CONFIG_NUMA_BALANCING */

/*
 * wait_task_inactive - wait for a thread to unschedule.
 *
 * If @match_state is nonzero, it's the @p->state value just checked and
 * not expected to change.  If it changes, i.e. @p might have woken up,
 * then return zero.  When we succeed in waiting for @p to be off its CPU,
 * we return a positive number (its total switch count).  If a second call
 * a short while later returns the same number, the caller can be sure that
 * @p has remained unscheduled the whole time.
 *
 * The caller must ensure that the task *will* unschedule sometime soon,
 * else this function might spin for a *long* time. This function can't
 * be called with interrupts off, or it may introduce deadlock with
 * smp_call_function() if an IPI is sent by the same process we are
 * waiting to become inactive.
 */
unsigned long wait_task_inactive(struct task_struct *p, unsigned int match_state)
{
	int running, queued;
	struct rq_flags rf;
	unsigned long ncsw;
	struct rq *rq;

	for (;;) {
		/*
		 * We do the initial early heuristics without holding
		 * any task-queue locks at all. We'll only try to get
		 * the runqueue lock when things look like they will
		 * work out!
		 */
		rq = task_rq(p);

		/*
		 * If the task is actively running on another CPU
		 * still, just relax and busy-wait without holding
		 * any locks.
		 *
		 * NOTE! Since we don't hold any locks, it's not
		 * even sure that "rq" stays as the right runqueue!
		 * But we don't care, since "task_running()" will
		 * return false if the runqueue has changed and p
		 * is actually now running somewhere else!
		 */
		while (task_running(rq, p)) {
			if (match_state && unlikely(READ_ONCE(p->__state) != match_state))
				return 0;
			cpu_relax();
		}

		/*
		 * Ok, time to look more closely! We need the rq
		 * lock now, to be *sure*. If we're wrong, we'll
		 * just go back and repeat.
		 */
		rq = task_rq_lock(p, &rf);
		trace_sched_wait_task(p);
		running = task_running(rq, p);
		queued = task_on_rq_queued(p);
		ncsw = 0;
		if (!match_state || READ_ONCE(p->__state) == match_state)
			ncsw = p->nvcsw | LONG_MIN; /* sets MSB */
		task_rq_unlock(rq, p, &rf);

		/*
		 * If it changed from the expected state, bail out now.
		 */
		if (unlikely(!ncsw))
			break;

		/*
		 * Was it really running after all now that we
		 * checked with the proper locks actually held?
		 *
		 * Oops. Go back and try again..
		 */
		if (unlikely(running)) {
			cpu_relax();
			continue;
		}

		/*
		 * It's not enough that it's not actively running,
		 * it must be off the runqueue _entirely_, and not
		 * preempted!
		 *
		 * So if it was still runnable (but just not actively
		 * running right now), it's preempted, and we should
		 * yield - it could be a while.
		 */
		if (unlikely(queued)) {
			ktime_t to = NSEC_PER_SEC / HZ;

			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_hrtimeout(&to, HRTIMER_MODE_REL_HARD);
			continue;
		}

		/*
		 * Ahh, all good. It wasn't running, and it wasn't
		 * runnable, which means that it will never become
		 * running in the future either. We're all done!
		 */
		break;
	}

	return ncsw;
}

/***
 * kick_process - kick a running thread to enter/exit the kernel
 * @p: the to-be-kicked thread
 *
 * Cause a process which is running on another CPU to enter
 * kernel-mode, without any delay. (to get signals handled.)
 *
 * NOTE: this function doesn't have to take the runqueue lock,
 * because all it wants to ensure is that the remote task enters
 * the kernel. If the IPI races and the task has been migrated
 * to another CPU then no harm is done and the purpose has been
 * achieved as well.
 */
void kick_process(struct task_struct *p)
{
	int cpu;

	preempt_disable();
	cpu = task_cpu(p);
	if ((cpu != smp_processor_id()) && task_curr(p))
		smp_send_reschedule(cpu);
	preempt_enable();
}
EXPORT_SYMBOL_GPL(kick_process);

/*
 * ->cpus_ptr is protected by both rq->lock and p->pi_lock
 *
 * A few notes on cpu_active vs cpu_online:
 *
 *  - cpu_active must be a subset of cpu_online
 *
 *  - on CPU-up we allow per-CPU kthreads on the online && !active CPU,
 *    see __set_cpus_allowed_ptr(). At this point the newly online
 *    CPU isn't yet part of the sched domains, and balancing will not
 *    see it.
 *
 *  - on CPU-down we clear cpu_active() to mask the sched domains and
 *    avoid the load balancer to place new tasks on the to be removed
 *    CPU. Existing tasks will remain running there and will be taken
 *    off.
 *
 * This means that fallback selection must not select !active CPUs.
 * And can assume that any active CPU must be online. Conversely
 * select_task_rq() below may allow selection of !active CPUs in order
 * to satisfy the above rules.
 */
static int select_fallback_rq(int cpu, struct task_struct *p)
{
	int nid = cpu_to_node(cpu);
	const struct cpumask *nodemask = NULL;
	enum { cpuset, possible, fail } state = cpuset;
	int dest_cpu;

	/*
	 * If the node that the CPU is on has been offlined, cpu_to_node()
	 * will return -1. There is no CPU on the node, and we should
	 * select the CPU on the other node.
	 */
	if (nid != -1) {
		nodemask = cpumask_of_node(nid);

		/* Look for allowed, online CPU in same node. */
		for_each_cpu(dest_cpu, nodemask) {
			if (is_cpu_allowed(p, dest_cpu))
				return dest_cpu;
		}
	}

	for (;;) {
		/* Any allowed, online CPU? */
		for_each_cpu(dest_cpu, p->cpus_ptr) {
			if (!is_cpu_allowed(p, dest_cpu))
				continue;

			goto out;
		}

		/* No more Mr. Nice Guy. */
		switch (state) {
		case cpuset:
			if (cpuset_cpus_allowed_fallback(p)) {
				state = possible;
				break;
			}
			fallthrough;
		case possible:
			/*
			 * XXX When called from select_task_rq() we only
			 * hold p->pi_lock and again violate locking order.
			 *
			 * More yuck to audit.
			 */
			do_set_cpus_allowed(p, task_cpu_possible_mask(p));
			state = fail;
			break;
		case fail:
			BUG();
			break;
		}
	}

out:
	if (state != cpuset) {
		/*
		 * Don't tell them about moving exiting tasks or
		 * kernel threads (both mm NULL), since they never
		 * leave kernel.
		 */
		if (p->mm && printk_ratelimit()) {
			printk_deferred("process %d (%s) no longer affine to cpu%d\n",
					task_pid_nr(p), p->comm, cpu);
		}
	}

	return dest_cpu;
}

/*
 * The caller (fork, wakeup) owns p->pi_lock, ->cpus_ptr is stable.
 */
static inline
int select_task_rq(struct task_struct *p, int cpu, int wake_flags)
{
	lockdep_assert_held(&p->pi_lock);

	if (p->nr_cpus_allowed > 1 && !is_migration_disabled(p))
		cpu = p->sched_class->select_task_rq(p, cpu, wake_flags);
	else
		cpu = cpumask_any(p->cpus_ptr);

	/*
	 * In order not to call set_task_cpu() on a blocking task we need
	 * to rely on ttwu() to place the task on a valid ->cpus_ptr
	 * CPU.
	 *
	 * Since this is common to all placement strategies, this lives here.
	 *
	 * [ this allows ->select_task() to simply return task_cpu(p) and
	 *   not worry about this generic constraint ]
	 */
	if (unlikely(!is_cpu_allowed(p, cpu)))
		cpu = select_fallback_rq(task_cpu(p), p);

	return cpu;
}

void sched_set_stop_task(int cpu, struct task_struct *stop)
{
	static struct lock_class_key stop_pi_lock;
	struct sched_param param = { .sched_priority = MAX_RT_PRIO - 1 };
	struct task_struct *old_stop = cpu_rq(cpu)->stop;

	if (stop) {
		/*
		 * Make it appear like a SCHED_FIFO task, its something
		 * userspace knows about and won't get confused about.
		 *
		 * Also, it will make PI more or less work without too
		 * much confusion -- but then, stop work should not
		 * rely on PI working anyway.
		 */
		sched_setscheduler_nocheck(stop, SCHED_FIFO, &param);

		stop->sched_class = &stop_sched_class;

		/*
		 * The PI code calls rt_mutex_setprio() with ->pi_lock held to
		 * adjust the effective priority of a task. As a result,
		 * rt_mutex_setprio() can trigger (RT) balancing operations,
		 * which can then trigger wakeups of the stop thread to push
		 * around the current task.
		 *
		 * The stop task itself will never be part of the PI-chain, it
		 * never blocks, therefore that ->pi_lock recursion is safe.
		 * Tell lockdep about this by placing the stop->pi_lock in its
		 * own class.
		 */
		lockdep_set_class(&stop->pi_lock, &stop_pi_lock);
	}

	cpu_rq(cpu)->stop = stop;

	if (old_stop) {
		/*
		 * Reset it back to a normal scheduling class so that
		 * it can die in pieces.
		 */
		old_stop->sched_class = &rt_sched_class;
	}
}

#else /* CONFIG_SMP */

static inline int __set_cpus_allowed_ptr(struct task_struct *p,
					 const struct cpumask *new_mask,
					 u32 flags)
{
	return set_cpus_allowed_ptr(p, new_mask);
}

static inline void migrate_disable_switch(struct rq *rq, struct task_struct *p) { }

static inline bool rq_has_pinned_tasks(struct rq *rq)
{
	return false;
}

#endif /* !CONFIG_SMP */

static void
ttwu_stat(struct task_struct *p, int cpu, int wake_flags)
{
	struct rq *rq;

	if (!schedstat_enabled())
		return;

	rq = this_rq();

#ifdef CONFIG_SMP
	if (cpu == rq->cpu) {
		__schedstat_inc(rq->ttwu_local);
		__schedstat_inc(p->stats.nr_wakeups_local);
	} else {
		struct sched_domain *sd;

		__schedstat_inc(p->stats.nr_wakeups_remote);
		rcu_read_lock();
		for_each_domain(rq->cpu, sd) {
			if (cpumask_test_cpu(cpu, sched_domain_span(sd))) {
				__schedstat_inc(sd->ttwu_wake_remote);
				break;
			}
		}
		rcu_read_unlock();
	}

	if (wake_flags & WF_MIGRATED)
		__schedstat_inc(p->stats.nr_wakeups_migrate);
#endif /* CONFIG_SMP */

	__schedstat_inc(rq->ttwu_count);
	__schedstat_inc(p->stats.nr_wakeups);

	if (wake_flags & WF_SYNC)
		__schedstat_inc(p->stats.nr_wakeups_sync);
}

/*
 * Mark the task runnable and perform wakeup-preemption.
 */
static void ttwu_do_wakeup(struct rq *rq, struct task_struct *p, int wake_flags,
			   struct rq_flags *rf)
{
	check_preempt_curr(rq, p, wake_flags);
	WRITE_ONCE(p->__state, TASK_RUNNING);
	trace_sched_wakeup(p);

#ifdef CONFIG_SMP
	if (p->sched_class->task_woken) {
		/*
		 * Our task @p is fully woken up and running; so it's safe to
		 * drop the rq->lock, hereafter rq is only used for statistics.
		 */
		rq_unpin_lock(rq, rf);
		p->sched_class->task_woken(rq, p);
		rq_repin_lock(rq, rf);
	}

	if (rq->idle_stamp) {
		u64 delta = rq_clock(rq) - rq->idle_stamp;
		u64 max = 2*rq->max_idle_balance_cost;

		update_avg(&rq->avg_idle, delta);

		if (rq->avg_idle > max)
			rq->avg_idle = max;

		rq->wake_stamp = jiffies;
		rq->wake_avg_idle = rq->avg_idle / 2;

		rq->idle_stamp = 0;
	}
#endif
}

static void
ttwu_do_activate(struct rq *rq, struct task_struct *p, int wake_flags,
		 struct rq_flags *rf)
{
	int en_flags = ENQUEUE_WAKEUP | ENQUEUE_NOCLOCK;

	lockdep_assert_rq_held(rq);

	if (p->sched_contributes_to_load)
		rq->nr_uninterruptible--;

#ifdef CONFIG_SMP
	if (wake_flags & WF_MIGRATED)
		en_flags |= ENQUEUE_MIGRATED;
	else
#endif
	if (p->in_iowait) {
		delayacct_blkio_end(p);
		atomic_dec(&task_rq(p)->nr_iowait);
	}

	activate_task(rq, p, en_flags);
	ttwu_do_wakeup(rq, p, wake_flags, rf);
}

/*
 * Consider @p being inside a wait loop:
 *
 *   for (;;) {
 *      set_current_state(TASK_UNINTERRUPTIBLE);
 *
 *      if (CONDITION)
 *         break;
 *
 *      schedule();
 *   }
 *   __set_current_state(TASK_RUNNING);
 *
 * between set_current_state() and schedule(). In this case @p is still
 * runnable, so all that needs doing is change p->state back to TASK_RUNNING in
 * an atomic manner.
 *
 * By taking task_rq(p)->lock we serialize against schedule(), if @p->on_rq
 * then schedule() must still happen and p->state can be changed to
 * TASK_RUNNING. Otherwise we lost the race, schedule() has happened, and we
 * need to do a full wakeup with enqueue.
 *
 * Returns: %true when the wakeup is done,
 *          %false otherwise.
 */
static int ttwu_runnable(struct task_struct *p, int wake_flags)
{
	struct rq_flags rf;
	struct rq *rq;
	int ret = 0;

	rq = __task_rq_lock(p, &rf);
	if (task_on_rq_queued(p)) {
		/* check_preempt_curr() may use rq clock */
		update_rq_clock(rq);
		ttwu_do_wakeup(rq, p, wake_flags, &rf);
		ret = 1;
	}
	__task_rq_unlock(rq, &rf);

	return ret;
}

#ifdef CONFIG_SMP
void sched_ttwu_pending(void *arg)
{
	struct llist_node *llist = arg;
	struct rq *rq = this_rq();
	struct task_struct *p, *t;
	struct rq_flags rf;

	if (!llist)
		return;

	/*
	 * rq::ttwu_pending racy indication of out-standing wakeups.
	 * Races such that false-negatives are possible, since they
	 * are shorter lived that false-positives would be.
	 */
	WRITE_ONCE(rq->ttwu_pending, 0);

	rq_lock_irqsave(rq, &rf);
	update_rq_clock(rq);

	llist_for_each_entry_safe(p, t, llist, wake_entry.llist) {
		if (WARN_ON_ONCE(p->on_cpu))
			smp_cond_load_acquire(&p->on_cpu, !VAL);

		if (WARN_ON_ONCE(task_cpu(p) != cpu_of(rq)))
			set_task_cpu(p, cpu_of(rq));

		ttwu_do_activate(rq, p, p->sched_remote_wakeup ? WF_MIGRATED : 0, &rf);
	}

	rq_unlock_irqrestore(rq, &rf);
}

void send_call_function_single_ipi(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (!set_nr_if_polling(rq->idle))
		arch_send_call_function_single_ipi(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

/*
 * Queue a task on the target CPUs wake_list and wake the CPU via IPI if
 * necessary. The wakee CPU on receipt of the IPI will queue the task
 * via sched_ttwu_wakeup() for activation so the wakee incurs the cost
 * of the wakeup instead of the waker.
 */
static void __ttwu_queue_wakelist(struct task_struct *p, int cpu, int wake_flags)
{
	struct rq *rq = cpu_rq(cpu);

	p->sched_remote_wakeup = !!(wake_flags & WF_MIGRATED);

	WRITE_ONCE(rq->ttwu_pending, 1);
	__smp_call_single_queue(cpu, &p->wake_entry.llist);
}

void wake_up_if_idle(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	rcu_read_lock();

	if (!is_idle_task(rcu_dereference(rq->curr)))
		goto out;

	rq_lock_irqsave(rq, &rf);
	if (is_idle_task(rq->curr))
		resched_curr(rq);
	/* Else CPU is not idle, do nothing here: */
	rq_unlock_irqrestore(rq, &rf);

out:
	rcu_read_unlock();
}

bool cpus_share_cache(int this_cpu, int that_cpu)
{
	if (this_cpu == that_cpu)
		return true;

	return per_cpu(sd_llc_id, this_cpu) == per_cpu(sd_llc_id, that_cpu);
}

static inline bool ttwu_queue_cond(int cpu, int wake_flags)
{
	/*
	 * Do not complicate things with the async wake_list while the CPU is
	 * in hotplug state.
	 */
	if (!cpu_active(cpu))
		return false;

	/*
	 * If the CPU does not share cache, then queue the task on the
	 * remote rqs wakelist to avoid accessing remote data.
	 */
	if (!cpus_share_cache(smp_processor_id(), cpu))
		return true;

	/*
	 * If the task is descheduling and the only running task on the
	 * CPU then use the wakelist to offload the task activation to
	 * the soon-to-be-idle CPU as the current CPU is likely busy.
	 * nr_running is checked to avoid unnecessary task stacking.
	 */
	if ((wake_flags & WF_ON_CPU) && cpu_rq(cpu)->nr_running <= 1)
		return true;

	return false;
}

static bool ttwu_queue_wakelist(struct task_struct *p, int cpu, int wake_flags)
{
	if (sched_feat(TTWU_QUEUE) && ttwu_queue_cond(cpu, wake_flags)) {
		if (WARN_ON_ONCE(cpu == smp_processor_id()))
			return false;

		sched_clock_cpu(cpu); /* Sync clocks across CPUs */
		__ttwu_queue_wakelist(p, cpu, wake_flags);
		return true;
	}

	return false;
}

#else /* !CONFIG_SMP */

static inline bool ttwu_queue_wakelist(struct task_struct *p, int cpu, int wake_flags)
{
	return false;
}

#endif /* CONFIG_SMP */

static void ttwu_queue(struct task_struct *p, int cpu, int wake_flags)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	if (ttwu_queue_wakelist(p, cpu, wake_flags))
		return;

	rq_lock(rq, &rf);
	update_rq_clock(rq);
	ttwu_do_activate(rq, p, wake_flags, &rf);
	rq_unlock(rq, &rf);
}

/*
 * Invoked from try_to_wake_up() to check whether the task can be woken up.
 *
 * The caller holds p::pi_lock if p != current or has preemption
 * disabled when p == current.
 *
 * The rules of PREEMPT_RT saved_state:
 *
 *   The related locking code always holds p::pi_lock when updating
 *   p::saved_state, which means the code is fully serialized in both cases.
 *
 *   The lock wait and lock wakeups happen via TASK_RTLOCK_WAIT. No other
 *   bits set. This allows to distinguish all wakeup scenarios.
 */
static __always_inline
bool ttwu_state_match(struct task_struct *p, unsigned int state, int *success)
{
	if (IS_ENABLED(CONFIG_DEBUG_PREEMPT)) {
		WARN_ON_ONCE((state & TASK_RTLOCK_WAIT) &&
			     state != TASK_RTLOCK_WAIT);
	}

	if (READ_ONCE(p->__state) & state) {
		*success = 1;
		return true;
	}

#ifdef CONFIG_PREEMPT_RT
	/*
	 * Saved state preserves the task state across blocking on
	 * an RT lock.  If the state matches, set p::saved_state to
	 * TASK_RUNNING, but do not wake the task because it waits
	 * for a lock wakeup. Also indicate success because from
	 * the regular waker's point of view this has succeeded.
	 *
	 * After acquiring the lock the task will restore p::__state
	 * from p::saved_state which ensures that the regular
	 * wakeup is not lost. The restore will also set
	 * p::saved_state to TASK_RUNNING so any further tests will
	 * not result in false positives vs. @success
	 */
	if (p->saved_state & state) {
		p->saved_state = TASK_RUNNING;
		*success = 1;
	}
#endif
	return false;
}

/*
 * Notes on Program-Order guarantees on SMP systems.
 *
 *  MIGRATION
 *
 * The basic program-order guarantee on SMP systems is that when a task [t]
 * migrates, all its activity on its old CPU [c0] happens-before any subsequent
 * execution on its new CPU [c1].
 *
 * For migration (of runnable tasks) this is provided by the following means:
 *
 *  A) UNLOCK of the rq(c0)->lock scheduling out task t
 *  B) migration for t is required to synchronize *both* rq(c0)->lock and
 *     rq(c1)->lock (if not at the same time, then in that order).
 *  C) LOCK of the rq(c1)->lock scheduling in task
 *
 * Release/acquire chaining guarantees that B happens after A and C after B.
 * Note: the CPU doing B need not be c0 or c1
 *
 * Example:
 *
 *   CPU0            CPU1            CPU2
 *
 *   LOCK rq(0)->lock
 *   sched-out X
 *   sched-in Y
 *   UNLOCK rq(0)->lock
 *
 *                                   LOCK rq(0)->lock // orders against CPU0
 *                                   dequeue X
 *                                   UNLOCK rq(0)->lock
 *
 *                                   LOCK rq(1)->lock
 *                                   enqueue X
 *                                   UNLOCK rq(1)->lock
 *
 *                   LOCK rq(1)->lock // orders against CPU2
 *                   sched-out Z
 *                   sched-in X
 *                   UNLOCK rq(1)->lock
 *
 *
 *  BLOCKING -- aka. SLEEP + WAKEUP
 *
 * For blocking we (obviously) need to provide the same guarantee as for
 * migration. However the means are completely different as there is no lock
 * chain to provide order. Instead we do:
 *
 *   1) smp_store_release(X->on_cpu, 0)   -- finish_task()
 *   2) smp_cond_load_acquire(!X->on_cpu) -- try_to_wake_up()
 *
 * Example:
 *
 *   CPU0 (schedule)  CPU1 (try_to_wake_up) CPU2 (schedule)
 *
 *   LOCK rq(0)->lock LOCK X->pi_lock
 *   dequeue X
 *   sched-out X
 *   smp_store_release(X->on_cpu, 0);
 *
 *                    smp_cond_load_acquire(&X->on_cpu, !VAL);
 *                    X->state = WAKING
 *                    set_task_cpu(X,2)
 *
 *                    LOCK rq(2)->lock
 *                    enqueue X
 *                    X->state = RUNNING
 *                    UNLOCK rq(2)->lock
 *
 *                                          LOCK rq(2)->lock // orders against CPU1
 *                                          sched-out Z
 *                                          sched-in X
 *                                          UNLOCK rq(2)->lock
 *
 *                    UNLOCK X->pi_lock
 *   UNLOCK rq(0)->lock
 *
 *
 * However, for wakeups there is a second guarantee we must provide, namely we
 * must ensure that CONDITION=1 done by the caller can not be reordered with
 * accesses to the task state; see try_to_wake_up() and set_current_state().
 */

/**
 * try_to_wake_up - wake up a thread
 * @p: the thread to be awakened
 * @state: the mask of task states that can be woken
 * @wake_flags: wake modifier flags (WF_*)
 *
 * Conceptually does:
 *
 *   If (@state & @p->state) @p->state = TASK_RUNNING.
 *
 * If the task was not queued/runnable, also place it back on a runqueue.
 *
 * This function is atomic against schedule() which would dequeue the task.
 *
 * It issues a full memory barrier before accessing @p->state, see the comment
 * with set_current_state().
 *
 * Uses p->pi_lock to serialize against concurrent wake-ups.
 *
 * Relies on p->pi_lock stabilizing:
 *  - p->sched_class
 *  - p->cpus_ptr
 *  - p->sched_task_group
 * in order to do migration, see its use of select_task_rq()/set_task_cpu().
 *
 * Tries really hard to only take one task_rq(p)->lock for performance.
 * Takes rq->lock in:
 *  - ttwu_runnable()    -- old rq, unavoidable, see comment there;
 *  - ttwu_queue()       -- new rq, for enqueue of the task;
 *  - psi_ttwu_dequeue() -- much sadness :-( accounting will kill us.
 *
 * As a consequence we race really badly with just about everything. See the
 * many memory barriers and their comments for details.
 *
 * Return: %true if @p->state changes (an actual wakeup was done),
 *	   %false otherwise.
 */
static int
try_to_wake_up(struct task_struct *p, unsigned int state, int wake_flags)
{
	unsigned long flags;
	int cpu, success = 0;

	preempt_disable();
	if (p == current) {
		/*
		 * We're waking current, this means 'p->on_rq' and 'task_cpu(p)
		 * == smp_processor_id()'. Together this means we can special
		 * case the whole 'p->on_rq && ttwu_runnable()' case below
		 * without taking any locks.
		 *
		 * In particular:
		 *  - we rely on Program-Order guarantees for all the ordering,
		 *  - we're serialized against set_special_state() by virtue of
		 *    it disabling IRQs (this allows not taking ->pi_lock).
		 */
		if (!ttwu_state_match(p, state, &success))
			goto out;

		trace_sched_waking(p);
		WRITE_ONCE(p->__state, TASK_RUNNING);
		trace_sched_wakeup(p);
		goto out;
	}

	/*
	 * If we are going to wake up a thread waiting for CONDITION we
	 * need to ensure that CONDITION=1 done by the caller can not be
	 * reordered with p->state check below. This pairs with smp_store_mb()
	 * in set_current_state() that the waiting thread does.
	 */
	raw_spin_lock_irqsave(&p->pi_lock, flags);
	smp_mb__after_spinlock();
	if (!ttwu_state_match(p, state, &success))
		goto unlock;

	trace_sched_waking(p);

	/*
	 * Ensure we load p->on_rq _after_ p->state, otherwise it would
	 * be possible to, falsely, observe p->on_rq == 0 and get stuck
	 * in smp_cond_load_acquire() below.
	 *
	 * sched_ttwu_pending()			try_to_wake_up()
	 *   STORE p->on_rq = 1			  LOAD p->state
	 *   UNLOCK rq->lock
	 *
	 * __schedule() (switch to task 'p')
	 *   LOCK rq->lock			  smp_rmb();
	 *   smp_mb__after_spinlock();
	 *   UNLOCK rq->lock
	 *
	 * [task p]
	 *   STORE p->state = UNINTERRUPTIBLE	  LOAD p->on_rq
	 *
	 * Pairs with the LOCK+smp_mb__after_spinlock() on rq->lock in
	 * __schedule().  See the comment for smp_mb__after_spinlock().
	 *
	 * A similar smb_rmb() lives in try_invoke_on_locked_down_task().
	 */
	smp_rmb();
	if (READ_ONCE(p->on_rq) && ttwu_runnable(p, wake_flags))
		goto unlock;

#ifdef CONFIG_SMP
	/*
	 * Ensure we load p->on_cpu _after_ p->on_rq, otherwise it would be
	 * possible to, falsely, observe p->on_cpu == 0.
	 *
	 * One must be running (->on_cpu == 1) in order to remove oneself
	 * from the runqueue.
	 *
	 * __schedule() (switch to task 'p')	try_to_wake_up()
	 *   STORE p->on_cpu = 1		  LOAD p->on_rq
	 *   UNLOCK rq->lock
	 *
	 * __schedule() (put 'p' to sleep)
	 *   LOCK rq->lock			  smp_rmb();
	 *   smp_mb__after_spinlock();
	 *   STORE p->on_rq = 0			  LOAD p->on_cpu
	 *
	 * Pairs with the LOCK+smp_mb__after_spinlock() on rq->lock in
	 * __schedule().  See the comment for smp_mb__after_spinlock().
	 *
	 * Form a control-dep-acquire with p->on_rq == 0 above, to ensure
	 * schedule()'s deactivate_task() has 'happened' and p will no longer
	 * care about it's own p->state. See the comment in __schedule().
	 */
	smp_acquire__after_ctrl_dep();

	/*
	 * We're doing the wakeup (@success == 1), they did a dequeue (p->on_rq
	 * == 0), which means we need to do an enqueue, change p->state to
	 * TASK_WAKING such that we can unlock p->pi_lock before doing the
	 * enqueue, such as ttwu_queue_wakelist().
	 */
	WRITE_ONCE(p->__state, TASK_WAKING);

	/*
	 * If the owning (remote) CPU is still in the middle of schedule() with
	 * this task as prev, considering queueing p on the remote CPUs wake_list
	 * which potentially sends an IPI instead of spinning on p->on_cpu to
	 * let the waker make forward progress. This is safe because IRQs are
	 * disabled and the IPI will deliver after on_cpu is cleared.
	 *
	 * Ensure we load task_cpu(p) after p->on_cpu:
	 *
	 * set_task_cpu(p, cpu);
	 *   STORE p->cpu = @cpu
	 * __schedule() (switch to task 'p')
	 *   LOCK rq->lock
	 *   smp_mb__after_spin_lock()		smp_cond_load_acquire(&p->on_cpu)
	 *   STORE p->on_cpu = 1		LOAD p->cpu
	 *
	 * to ensure we observe the correct CPU on which the task is currently
	 * scheduling.
	 */
	if (smp_load_acquire(&p->on_cpu) &&
	    ttwu_queue_wakelist(p, task_cpu(p), wake_flags | WF_ON_CPU))
		goto unlock;

	/*
	 * If the owning (remote) CPU is still in the middle of schedule() with
	 * this task as prev, wait until it's done referencing the task.
	 *
	 * Pairs with the smp_store_release() in finish_task().
	 *
	 * This ensures that tasks getting woken will be fully ordered against
	 * their previous state and preserve Program Order.
	 */
	smp_cond_load_acquire(&p->on_cpu, !VAL);

	cpu = select_task_rq(p, p->wake_cpu, wake_flags | WF_TTWU);
	if (task_cpu(p) != cpu) {
		if (p->in_iowait) {
			delayacct_blkio_end(p);
			atomic_dec(&task_rq(p)->nr_iowait);
		}

		wake_flags |= WF_MIGRATED;
		psi_ttwu_dequeue(p);
		set_task_cpu(p, cpu);
	}
#else
	cpu = task_cpu(p);
#endif /* CONFIG_SMP */

	ttwu_queue(p, cpu, wake_flags);
unlock:
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
out:
	if (success)
		ttwu_stat(p, task_cpu(p), wake_flags);
	preempt_enable();

	return success;
}

/**
 * task_call_func - Invoke a function on task in fixed state
 * @p: Process for which the function is to be invoked, can be @current.
 * @func: Function to invoke.
 * @arg: Argument to function.
 *
 * Fix the task in it's current state by avoiding wakeups and or rq operations
 * and call @func(@arg) on it.  This function can use ->on_rq and task_curr()
 * to work out what the state is, if required.  Given that @func can be invoked
 * with a runqueue lock held, it had better be quite lightweight.
 *
 * Returns:
 *   Whatever @func returns
 */
int task_call_func(struct task_struct *p, task_call_f func, void *arg)
{
	struct rq *rq = NULL;
	unsigned int state;
	struct rq_flags rf;
	int ret;

	raw_spin_lock_irqsave(&p->pi_lock, rf.flags);

	state = READ_ONCE(p->__state);

	/*
	 * Ensure we load p->on_rq after p->__state, otherwise it would be
	 * possible to, falsely, observe p->on_rq == 0.
	 *
	 * See try_to_wake_up() for a longer comment.
	 */
	smp_rmb();

	/*
	 * Since pi->lock blocks try_to_wake_up(), we don't need rq->lock when
	 * the task is blocked. Make sure to check @state since ttwu() can drop
	 * locks at the end, see ttwu_queue_wakelist().
	 */
	if (state == TASK_RUNNING || state == TASK_WAKING || p->on_rq)
		rq = __task_rq_lock(p, &rf);

	/*
	 * At this point the task is pinned; either:
	 *  - blocked and we're holding off wakeups	 (pi->lock)
	 *  - woken, and we're holding off enqueue	 (rq->lock)
	 *  - queued, and we're holding off schedule	 (rq->lock)
	 *  - running, and we're holding off de-schedule (rq->lock)
	 *
	 * The called function (@func) can use: task_curr(), p->on_rq and
	 * p->__state to differentiate between these states.
	 */
	ret = func(p, arg);

	if (rq)
		rq_unlock(rq, &rf);

	raw_spin_unlock_irqrestore(&p->pi_lock, rf.flags);
	return ret;
}

/**
 * wake_up_process - Wake up a specific process
 * @p: The process to be woken up.
 *
 * Attempt to wake up the nominated process and move it to the set of runnable
 * processes.
 *
 * Return: 1 if the process was woken up, 0 if it was already running.
 *
 * This function executes a full memory barrier before accessing the task state.
 */
int wake_up_process(struct task_struct *p)
{
	return try_to_wake_up(p, TASK_NORMAL, 0);
}
EXPORT_SYMBOL(wake_up_process);

int wake_up_state(struct task_struct *p, unsigned int state)
{
	return try_to_wake_up(p, state, 0);
}

/*
 * Perform scheduler related setup for a newly forked process p.
 * p is forked by current.
 *
 * __sched_fork() is basic setup used by init_idle() too:
 */
static void __sched_fork(unsigned long clone_flags, struct task_struct *p)
{
	p->on_rq			= 0;

	p->se.on_rq			= 0;
	p->se.exec_start		= 0;
	p->se.sum_exec_runtime		= 0;
	p->se.prev_sum_exec_runtime	= 0;
	p->se.nr_migrations		= 0;
	p->se.vruntime			= 0;
	INIT_LIST_HEAD(&p->se.group_node);

#ifdef CONFIG_FAIR_GROUP_SCHED
	p->se.cfs_rq			= NULL;
#endif

#ifdef CONFIG_SCHEDSTATS
	/* Even if schedstat is disabled, there should not be garbage */
	memset(&p->stats, 0, sizeof(p->stats));
#endif

	RB_CLEAR_NODE(&p->dl.rb_node);
	init_dl_task_timer(&p->dl);
	init_dl_inactive_task_timer(&p->dl);
	__dl_clear_params(p);

	INIT_LIST_HEAD(&p->rt.run_list);
	p->rt.timeout		= 0;
	p->rt.time_slice	= sched_rr_timeslice;
	p->rt.on_rq		= 0;
	p->rt.on_list		= 0;

#ifdef CONFIG_PREEMPT_NOTIFIERS
	INIT_HLIST_HEAD(&p->preempt_notifiers);
#endif

#ifdef CONFIG_COMPACTION
	p->capture_control = NULL;
#endif
	init_numa_balancing(clone_flags, p);
#ifdef CONFIG_SMP
	p->wake_entry.u_flags = CSD_TYPE_TTWU;
	p->migration_pending = NULL;
#endq;

	for (;;) {
		raw_spin_lock_irqsave(&p->pi_lock, rf->flags);
		rq = task_rq(p);
		raw_spin_rq_lock(rq);
		/*
		 *	move_queued_task()		task_rq_lock()
		 *
		 *	ACQUIRE (rq->lock)
		 *	[S] ->on_rq = MIGRATING		[L] rq = task_rq()
		 *	WMB (__set_task_cpu())		ACQUIRE (rq->lock);
		 *	[S] ->cpu = new_cpu		[L] task_rq()
		 *					[L] ->on_rq
		 *	RELEASE (rq->lock)
		 *
		 * If we observe the old CPU in task_rq_lock(), the acquire of
		 * the old rq->lock will fully serialize against the stores.
		 *
		 * If we observe the new CPU in task_rq_lock(), the address
		 * dependency headed by '[L] rq = task_rq()' and the acquire
		 * will pair with the WMB to ensure we then also see migrating.
		 */
		if (likely(rq == task_rq(p) && !task_on_rq_migrating(p))) {
			rq_pin_lock(rq, rf);
			return rq;
		}
		raw_spin_rq_unlock(rq);
		raw_spin_unlock_irqrestore(&p->pi_lock, rf->flags);

		while (unlikely(task_on_rq_migrating(p)))
			cpu_relax();
	}
}

/*
 * RQ-clock updating methods:
 */

static void update_rq_clock_task(struct rq *rq, s64 delta)
{
/*
 * In theory, the compile should just see 0 here, and optimize out the call
 * to sched_rt_avg_update. But I don't trust it...
 */
	s64 __maybe_unused steal = 0, irq_delta = 0;

#ifdef CONFIG_IRQ_TIME_ACCOUNTING
	irq_delta = irq_time_read(cpu_of(rq)) - rq->prev_irq_time;

	/*
	 * Since irq_time is only updated on {soft,}irq_exit, we might run into
	 * this case when a previous update_rq_clock() happened inside a
	 * {soft,}irq region.
	 *
	 * When this happens, we stop ->clock_task and only update the
	 * prev_irq_time stamp to account for the part that fit, so that a next
	 * update will consume the rest. This ensures ->clock_task is
	 * monotonic.
	 *
	 * It does however cause some slight miss-attribution of {soft,}irq
	 * time, a more accurate solution would be to update the irq_time using
	 * the current rq->clock timestamp, except that would require using
	 * atomic ops.
	 */
	if (irq_delta > delta)
		irq_delta = delta;

	rq->prev_irq_time += irq_delta;
	delta -= irq_delta;
#endif
#ifdef CONFIG_PARAVIRT_TIME_ACCOUNTING
	if (static_key_false((&paravirt_steal_rq_enabled))) {
		steal = paravirt_steal_clock(cpu_of(rq));
		steal -= rq->prev_steal_time_rq;

		if (unlikely(steal > delta))
			steal = delta;

		rq->prev_steal_time_rq += steal;
		delta -= steal;
	}
#endif

	rq->clock_task += delta;

#ifdef CONFIG_HAVE_SCHED_AVG_IRQ
	if ((irq_delta + steal) && sched_feat(NONTASK_CAPACITY))
		update_irq_load_avg(rq, irq_delta + steal);
#endif
	update_rq_clock_pelt(rq, delta);
}

void update_rq_clock(struct rq *rq)
{
	s64 delta;

	lockdep_assert_rq_held(rq);

	if (rq->clock_update_flags & RQCF_ACT_SKIP)
		return;

#ifdef CONFIG_SCHED_DEBUG
	if (sched_feat(WARN_DOUBLE_CLOCK))
		SCHED_WARN_ON(rq->clock_update_flags & RQCF_UPDATED);
	rq->clock_update_flags |= RQCF_UPDATED;
#endif

	delta = sched_clock_cpu(cpu_of(rq)) - rq->clock;
	if (delta < 0)
		return;
	rq->clock += delta;
	update_rq_clock_task(rq, delta);
}

#ifdef CONFIG_SCHED_HRTICK
/*
 * Use HR-timers to deliver accurate preemption points.
 */

static void hrtick_clear(struct rq *rq)
{
	if (hrtimer_active(&rq->hrtick_timer))
		hrtimer_cancel(&rq->hrtick_timer);
}

/*
 * High-resolution timer tick.
 * Runs from hardirq context with interrupts disabled.
 */
static enum hrtimer_restart hrtick(struct hrtimer *timer)
{
	struct rq *rq = container_of(timer, struct rq, hrtick_timer);
	struct rq_flags rf;

	WARN_ON_ONCE(cpu_of(rq) != smp_processor_id());

	rq_lock(rq, &rf);
	update_rq_clock(rq);
	rq->curr->sched_class->task_tick(rq, rq->curr, 1);
	rq_unlock(rq, &rf);

	return HRTIMER_NORESTART;
}

#ifdef CONFIG_SMP

static void __hrtick_restart(struct rq *rq)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	ktime_t time = rq->hrtick_time;

	hrtimer_start(timer, time, HRTIMER_MODE_ABS_PINNED_HARD);
}

/*
 * called from hardirq (IPI) context
 */
static void __hrtick_start(void *arg)
{
	struct rq *rq = arg;
	struct rq_flags rf;

	rq_lock(rq, &rf);
	__hrtick_restart(rq);
	rq_unlock(rq, &rf);
}

/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	s64 delta;

	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense and can cause timer DoS.
	 */
	delta = max_t(s64, delay, 10000LL);
	rq->hrtick_time = ktime_add_ns(timer->base->get_time(), delta);

	if (rq == this_rq())
		__hrtick_restart(rq);
	else
		smp_call_function_single_async(cpu_of(rq), &rq->hrtick_csd);
}

#else
/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense. Rely on vruntime for fairness.
	 */
	delay = max_t(u64, delay, 10000LL);
	hrtimer_start(&rq->hrtick_timer, ns_to_ktime(delay),
		      HRTIMER_MODE_REL_PINNED_HARD);
}

#endif /* CONFIG_SMP */

static void hrtick_rq_init(struct rq *rq)
{
#ifdef CONFIG_SMP
	INIT_CSD(&rq->hrtick_csd, __hrtick_start, rq);
#endif
	hrtimer_init(&rq->hrtick_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL_HARD);
	rq->hrtick_timer.function = hrtick;
}
#else	/* CONFIG_SCHED_HRTICK */
static inline void hrtick_clear(struct rq *rq)
{
}

static inline void hrtick_rq_init(struct rq *rq)
{
}
#endif	/* CONFIG_SCHED_HRTICK */

/*
 * cmpxchg based fetch_or, macro so it works for different integer types
 */
#define fetch_or(ptr, mask)						\
	({								\
		typeof(ptr) _ptr = (ptr);				\
		typeof(mask) _mask = (mask);				\
		typeof(*_ptr) _old, _val = *_ptr;			\
									\
		for (;;) {						\
			_old = cmpxchg(_ptr, _val, _val | _mask);	\
			if (_old == _val)				\
				break;					\
			_val = _old;					\
		}							\
	_old;								\
})

#if defined(CONFIG_SMP) && defined(TIF_POLLING_NRFLAG)
/*
 * Atomically set TIF_NEED_RESCHED and test for TIF_POLLING_NRFLAG,
 * this avoids any races wrt polling state changes and thereby avoids
 * spurious IPIs.
 */
static bool set_nr_and_not_polling(struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	return !(fetch_or(&ti->flags, _TIF_NEED_RESCHED) & _TIF_POLLING_NRFLAG);
}

/*
 * Atomically set TIF_NEED_RESCHED if TIF_POLLING_NRFLAG is set.
 *
 * If this returns true, then the idle task promises to call
 * sched_ttwu_pending() and reschedule soon.
 */
static bool set_nr_if_polling(struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	typeof(ti->flags) old, val = READ_ONCE(ti->flags);

	for (;;) {
		if (!(val & _TIF_POLLING_NRFLAG))
			return false;
		if (val & _TIF_NEED_RESCHED)
			return true;
		old = cmpxchg(&ti->flags, val, val | _TIF_NEED_RESCHED);
		if (old == val)
			break;
		val = old;
	}
	return true;
}

#else
static bool set_nr_and_not_polling(struct task_struct *p)
{
	set_tsk_need_resched(p);
	return true;
}

#ifdef CONFIG_SMP
static bool set_nr_if_polling(struct task_struct *p)
{
	return false;
}
#endif
#endif

static bool __wake_q_add(struct wake_q_head *head, struct task_struct *task)
{
	struct wake_q_node *node = &task->wake_q;

	/*
	 * Atomically grab the task, if ->wake_q is !nil already it means
	 * it's already queued (either by us or someone else) and will get the
	 * wakeup due to that.
	 *
	 * In order to ensure that a pending wakeup will observe our pending
	 * state, even in the failed case, an explicit smp_mb() must be used.
	 */
	smp_mb__before_atomic();
	if (unlikely(cmpxchg_relaxed(&node->next, NULL, WAKE_Q_TAIL)))
		return false;

	/*
	 * The head is context local, there can be no concurrency.
	 */
	*head->lastp = node;
	head->lastp = &node->next;
	return true;
}

/**
 * wake_q_add() - queue a wakeup for 'later' waking.
 * @head: the wake_q_head to add @task to
 * @task: the task to queue for 'later' wakeup
 *
 * Queue a task for later wakeup, most likely by the wake_up_q() call in the
 * same context, _HOWEVER_ this is not guaranteed, the wakeup can come
 * instantly.
 *
 * This function must be used as-if it were wake_up_process(); IOW the task
 * must be ready to be woken at this location.
 */
void wake_q_add(struct wake_q_head *head, struct task_struct *task)
{
	if (__wake_q_add(head, task))
		get_task_struct(task);
}

/**
 * wake_q_add_safe() - safely queue a wakeup for 'later' waking.
 * @head: the wake_q_head to add @task to
 * @task: the task to queue for 'later' wakeup
 *
 * Queue a task for later wakeup, most likely by the wake_up_q() call in the
 * same context, _HOWEVER_ this is not guaranteed, the wakeup can come
 * instantly.
 *
 * This function must be used as-if it were wake_up_process(); IOW the task
 * must be ready to be woken at this location.
 *
 * This function is essentially a task-safe equivalent to wake_q_add(). Callers
 * that already hold reference to @task can call the 'safe' version and trust
 * wake_q to do the right thing depending whether or not the @task is already
 * queued for wakeup.
 */
void wake_q_add_safe(struct wake_q_head *head, struct task_struct *task)
{
	if (!__wake_q_add(head, task))
		put_task_struct(task);
}

void wake_up_q(struct wake_q_head *head)
{
	struct wake_q_node *node = head->first;

	while (node != WAKE_Q_TAIL) {
		struct task_struct *task;

		task = container_of(node, struct task_struct, wake_q);
		/* Task can safely be re-inserted now: */
		node = node->next;
		task->wake_q.next = NULL;

		/*
		 * wake_up_process() executes a full barrier, which pairs with
		 * the queueing in wake_q_add() so as not to miss wakeups.
		 */
		wake_up_process(task);
		put_task_struct(task);
	}
}

/*
 * resched_curr - mark rq's current task 'to be rescheduled now'.
 *
 * On UP this means the setting of the need_resched flag, on SMP it
 * might also involve a cross-CPU call to trigger the scheduler on
 * the target CPU.
 */
void resched_curr(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	int cpu;

	lockdep_assert_rq_held(rq);

	if (test_tsk_need_resched(curr))
		return;

	cpu = cpu_of(rq);

	if (cpu == smp_processor_id()) {
		set_tsk_need_resched(curr);
		set_preempt_need_resched();
		return;
	}

	if (set_nr_and_not_polling(curr))
		smp_send_reschedule(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

void resched_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	unsigned long flags;

	raw_spin_rq_lock_irqsave(rq, flags);
	if (cpu_online(cpu) || cpu == smp_processor_id())
		resched_curr(rq);
	raw_spin_rq_unlock_irqrestore(rq, flags);
}

#ifdef CONFIG_SMP
#ifdef CONFIG_NO_HZ_COMMON
/*
 * In the semi idle case, use the nearest busy CPU for migrating timers
 * from an idle CPU.  This is good for power-savings.
 *
 * We don't do similar optimization for completely idle system, as
 * selecting an idle CPU will add more delays to the timers than intended
 * (as that CPU's timer base may not be uptodate wrt jiffies etc).
 */
int get_nohz_timer_target(void)
{
	int i, cpu = smp_processor_id(), default_cpu = -1;
	struct sched_domain *sd;
	const struct cpumask *hk_mask;

	if (housekeeping_cpu(cpu, HK_TYPE_TIMER)) {
		if (!idle_cpu(cpu))
			return cpu;
		default_cpu = cpu;
	}

	hk_mask = housekeeping_cpumask(HK_TYPE_TIMER);

	rcu_read_lock();
	for_each_domain(cpu, sd) {
		for_each_cpu_and(i, sched_domain_span(sd), hk_mask) {
			if (cpu == i)
				continue;

			if (!idle_cpu(i)) {
				cpu = i;
				goto unlock;
			}
		}
	}

	if (default_cpu == -1)
		default_cpu = housekeeping_any_cpu(HK_TYPE_TIMER);
	cpu = default_cpu;
unlock:
	rcu_read_unlock();
	return cpu;
}

/*
 * When add_timer_on() enqueues a timer into the timer wheel of an
 * idle CPU then this timer might expire before the next timer event
 * which is scheduled to wake up that CPU. In case of a completely
 * idle system the next event might even be infinite time into the
 * future. wake_up_idle_cpu() ensures that the CPU is woken up and
 * leaves the inner idle loop so the newly added timer is taken into
 * account when the CPU goes back to idle and evaluates the timer
 * wheel for the next timer event.
 */
static void wake_up_idle_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (cpu == smp_processor_id())
		return;

	if (set_nr_and_not_polling(rq->idle))
		smp_send_reschedule(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

static bool wake_up_full_nohz_cpu(int cpu)
{
	/*
	 * We just need the target to call irq_exit() and re-evaluate
	 * the next tick. The nohz full kick at least implies that.
	 * If needed we can still optimize that later with an
	 * empty IRQ.
	 */
	if (cpu_is_offline(cpu))
		return true;  /* Don't try to wake offline CPUs. */
	if (tick_nohz_full_cpu(cpu)) {
		if (cpu != smp_processor_id() ||
		    tick_nohz_tick_stopped())
			tick_nohz_full_kick_cpu(cpu);
		return true;
	}

	return false;
}

/*
 * Wake up the specified CPU.  If the CPU is going offline, it is the
 * caller's responsibility to deal with the lost wakeup, for example,
 * by hooking into the CPU_DEAD notifier like timers and hrtimers do.
 */
void wake_up_nohz_cpu(int cpu)
{
	if (!wake_up_full_nohz_cpu(cpu))
		wake_up_idle_cpu(cpu);
}

static void nohz_csd_func(void *info)
{
	struct rq *rq = info;
	int cpu = cpu_of(rq);
	unsigned int flags;

	/*
	 * Release the rq::nohz_csd.
	 */
	flags = atomic_fetch_andnot(NOHZ_KICK_MASK | NOHZ_NEWILB_KICK, nohz_flags(cpu));
	WARN_ON(!(flags & NOHZ_KICK_MASK));

	rq->idle_balance = idle_cpu(cpu);
	if (rq->idle_balance && !need_resched()) {
		rq->nohz_idle_balance = flags;
		raise_softirq_irqoff(SCHED_SOFTIRQ);
	}
}

#endif /* CONFIG_NO_HZ_COMMON */

#ifdef CONFIG_NO_HZ_FULL
bool sched_can_stop_tick(struct rq *rq)
{
	int fifo_nr_running;

	/* Deadline tasks, even if single, need the tick */
	if (rq->dl.dl_nr_running)
		return false;

	/*
	 * If there are more than one RR tasks, we need the tick to affect the
	 * actual RR behaviour.
	 */
	if (rq->rt.rr_nr_running) {
		if (rq->rt.rr_nr_running == 1)
			return true;
		else
			return false;
	}

	/*
	 * If there's no RR tasks, but FIFO tasks, we can skip the tick, no
	 * forced preemption between FIFO tasks.
	 */
	fifo_nr_running = rq->rt.rt_nr_running - rq->rt.rr_nr_running;
	if (fifo_nr_running)
		return true;

	/*
	 * If there are no DL,RR/FIFO tasks, there must only be CFS tasks left;
	 * if there's more than one we need the tick for involuntary
	 * preemption.
	 */
	if (rq->nr_running > 1)
		return false;

	return true;
}
#endif /* CONFIG_NO_HZ_FULL */
#endif /* CONFIG_SMP */

#if defined(CONFIG_RT_GROUP_SCHED) || (defined(CONFIG_FAIR_GROUP_SCHED) && \
			(defined(CONFIG_SMP) || defined(CONFIG_CFS_BANDWIDTH)))
/*
 * Iterate task_group tree rooted at *from, calling @down when first entering a
 * node and @up when leaving it for the final time.
 *
 * Caller must hold rcu_lock or sufficient equivalent.
 */
int walk_tg_tree_from(struct task_group *from,
			     tg_visitor down, tg_visitor up, void *data)
{
	struct task_group *parent, *child;
	int ret;

	parent = from;

down:
	ret = (*down)(parent, data);
	if (ret)
		goto out;
	list_for_each_entry_rcu(child, &parent->children, siblings) {
		parent = child;
		goto down;

up:
		continue;
	}
	ret = (*up)(parent, data);
	if (ret || parent == from)
		goto out;

	child = parent;
	parent = parent->parent;
	if (parent)
		goto up;
out:
	return ret;
}

int tg_nop(struct task_group *tg, void *data)
{
	return 0;
}
#endif

static void set_load_weight(struct task_struct *p, bool update_load)
{
	int prio = p->static_prio - MAX_RT_PRIO;
	struct load_weight *load = &p->se.load;

	/*
	 * SCHED_IDLE tasks get minimal weight:
	 */
	if (task_has_idle_policy(p)) {
		load->weight = scale_load(WEIGHT_IDLEPRIO);
		load->inv_weight = WMULT_IDLEPRIO;
		return;
	}

	/*
	 * SCHED_OTHER tasks have to update their load when changing their
	 * weight
	 */
	if (update_load && p->sched_class == &fair_sched_class) {
		reweight_task(p, prio);
	} else {
		load->weight = scale_load(sched_prio_to_weight[prio]);
		load->inv_weight = sched_prio_to_wmult[prio];
	}
}

#ifdef CONFIG_UCLAMP_TASK
/*
 * Serializes updates of utilization clamp values
 *
 * The (slow-path) user-space triggers utilization clamp value updates which
 * can require updates on (fast-path) scheduler's data structures used to
 * support enqueue/dequeue operations.
 * While the per-CPU rq lock protects fast-path update operations, user-space
 * requests are serialized using a mutex to reduce the risk of conflicting
 * updates or API abuses.
 */
static DEFINE_MUTEX(uclamp_mutex);

/* Max allowed minimum utilization */
unsigned int sysctl_sched_uclamp_util_min = SCHED_CAPACITY_SCALE;

/* Max allowed maximum utilization */
unsigned int sysctl_sched_uclamp_util_max = SCHED_CAPACITY_SCALE;

/*
 * By default RT tasks run at the maximum performance point/capacity of the
 * system. Uclamp enforces this by always setting UCLAMP_MIN of RT tasks to
 * SCHED_CAPACITY_SCALE.
 *
 * This knob allows admins to change the default behavior when uclamp is being
 * used. In battery powered devices, particularly, running at the maximum
 * capacity and frequency will increase energy consumption and shorten the
 * battery life.
 *
 * This knob only affects RT tasks that their uclamp_se->user_defined == false.
 *
 * This knob will not override the system default sched_util_clamp_min defined
 * above.
 */
unsigned int sysctl_sched_uclamp_util_min_rt_default = SCHED_CAPACITY_SCALE;

/* All clamps are required to be less or equal than these values */
static struct uclamp_se uclamp_default[UCLAMP_CNT];

/*
 * This static key is used to reduce the uclamp overhead in the fast path. It
 * primarily disables the call to uclamp_rq_{inc, dec}() in
 * enqueue/dequeue_task().
 *
 * This allows users to continue to enable uclamp in their kernel config with
 * minimum uclamp overhead in the fast path.
 *
 * As soon as userspace modifies any of the uclamp knobs, the static key is
 * enabled, since we have an actual users that make use of uclamp
 * functionality.
 *
 * The knobs that would enable this static key are:
 *
 *   * A task modifying its uclamp value with sched_setattr().
 *   * An admin modifying the sysctl_sched_uclamp_{min, max} via procfs.
 *   * An admin modifying the cgroup cpu.uclamp.{min, max}
 */
DEFINE_STATIC_KEY_FALSE(sched_uclamp_used);

/* Integer rounded range for each bucket */
#define UCLAMP_BUCKET_DELTA DIV_ROUND_CLOSEST(SCHED_CAPACITY_SCALE, UCLAMP_BUCKETS)

#define for_each_clamp_id(clamp_id) \
	for ((clamp_id) = 0; (clamp_id) < UCLAMP_CNT; (clamp_id)++)

static inline unsigned int uclamp_bucket_id(unsigned int clamp_value)
{
	return min_t(unsigned int, clamp_value / UCLAMP_BUCKET_DELTA, UCLAMP_BUCKETS - 1);
}

static inline unsigned int uclamp_none(enum uclamp_id clamp_id)
{
	if (clamp_id == UCLAMP_MIN)
		return 0;
	return SCHED_CAPACITY_SCALE;
}

static inline void uclamp_se_set(struct uclamp_se *uc_se,
				 unsigned int value, bool user_defined)
{
	uc_se->value = value;
	uc_se->bucket_id = uclamp_bucket_id(value);
	uc_se->user_defined = user_defined;
}

static inline unsigned int
uclamp_idle_value(struct rq *rq, enum uclamp_id clamp_id,
		  unsigned int clamp_value)
{
	/*
	 * Avoid blocked utilization pushing up the frequency when we go
	 * idle (which drops the max-clamp) by retaining the last known
	 * max-clamp.
	 */
	if (clamp_id == UCLAMP_MAX) {
		rq->uclamp_flags |= UCLAMP_FLAG_IDLE;
		return clamp_value;
	}

	return uclamp_none(UCLAMP_MIN);
}

static inline void uclamp_idle_reset(struct rq *rq, enum uclamp_id clamp_id,
				     unsigned int clamp_value)
{
	/* Reset max-clamp retention only on idle exit */
	if (!(rq->uclamp_flags & UCLAMP_FLAG_IDLE))
		return;

	WRITE_ONCE(rq->uclamp[clamp_id].value, clamp_value);
}

static inline
unsigned int uclamp_rq_max_value(struct rq *rq, enum uclamp_id clamp_id,
				   unsigned int clamp_value)
{
	struct uclamp_bucket *bucket = rq->uclamp[clamp_id].bucket;
	int bucket_id = UCLAMP_BUCKETS - 1;

	/*
	 * Since both min and max clamps are max aggregated, find the
	 * top most bucket with tasks in.
	 */
	for ( ; bucket_id >= 0; bucket_id--) {
		if (!bucket[bucket_id].tasks)
			continue;
		return bucket[bucket_id].value;
	}

	/* No tasks -- default clamp values */
	return uclamp_idle_value(rq, clamp_id, clamp_value);
}

static void __uclamp_update_util_min_rt_default(struct task_struct *p)
{
	unsigned int default_util_min;
	struct uclamp_se *uc_se;

	lockdep_assert_held(&p->pi_lock);

	uc_se = &p->uclamp_req[UCLAMP_MIN];

	/* Only sync if user didn't override the default */
	if (uc_se->user_defined)
		return;

	default_util_min = sysctl_sched_uclamp_util_min_rt_default;
	uclamp_se_set(uc_se, default_util_min, false);
}

static void uclamp_update_util_min_rt_default(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;

	if (!rt_task(p))
		return;

	/* Protect updates to p->uclamp_* */
	rq = task_rq_lock(p, &rf);
	__uclamp_update_util_min_rt_default(p);
	task_rq_unlock(rq, p, &rf);
}

static void uclamp_sync_util_min_rt_default(void)
{
	struct task_struct *g, *p;

	/*
	 * copy_process()			sysctl_uclamp
	 *					  uclamp_min_rt = X;
	 *   write_lock(&tasklist_lock)		  read_lock(&tasklist_lock)
	 *   // link thread			  smp_mb__after_spinlock()
	 *   write_unlock(&tasklist_lock)	  read_unlock(&tasklist_lock);
	 *   sched_post_fork()			  for_each_process_thread()
	 *     __uclamp_sync_rt()		    __uclamp_sync_rt()
	 *
	 * Ensures that either sched_post_fork() will observe the new
	 * uclamp_min_rt or for_each_process_thread() will observe the new
	 * task.
	 */
	read_lock(&tasklist_lock);
	smp_mb__after_spinlock();
	read_unlock(&tasklist_lock);

	rcu_read_lock();
	for_each_process_thread(g, p)
		uclamp_update_util_min_rt_default(p);
	rcu_read_unlock();
}

static inline struct uclamp_se
uclamp_tg_restrict(struct task_struct *p, enum uclamp_id clamp_id)
{
	/* Copy by value as we could modify it */
	struct uclamp_se uc_req = p->uclamp_req[clamp_id];
#ifdef CONFIG_UCLAMP_TASK_GROUP
	unsigned int tg_min, tg_max, value;

	/*
	 * Tasks in autogroups or root task group will be
	 * restricted by system defaults.
	 */
	if (task_group_is_autogroup(task_group(p)))
		return uc_req;
	if (task_group(p) == &root_task_group)
		return uc_req;

	tg_min = task_group(p)->uclamp[UCLAMP_MIN].value;
	tg_max = task_group(p)->uclamp[UCLAMP_MAX].value;
	value = uc_req.value;
	value = clamp(value, tg_min, tg_max);
	uclamp_se_set(&uc_req, value, false);
#endif

	return uc_req;
}

/*
 * The effective clamp bucket index of a task depends on, by increasing
 * priority:
 * - the task specific clamp value, when explicitly requested from userspace
 * - the task group effective clamp value, for tasks not either in the root
 *   group or in an autogroup
 * - the system default clamp value, defined by the sysadmin
 */
static inline struct uclamp_se
uclamp_eff_get(struct task_struct *p, enum uclamp_id clamp_id)
{
	struct uclamp_se uc_req = uclamp_tg_restrict(p, clamp_id);
	struct uclamp_se uc_max = uclamp_default[clamp_id];

	/* System default restrictions always apply */
	if (unlikely(uc_req.value > uc_max.value))
		return uc_max;

	return uc_req;
}

unsigned long uclamp_eff_value(struct task_struct *p, enum uclamp_id clamp_id)
{
	struct uclamp_se uc_eff;

	/* Task currently refcounted: use back-annotated (effective) value */
	if (p->uclamp[clamp_id].active)
		return (unsigned long)p->uclamp[clamp_id].value;

	uc_eff = uclamp_eff_get(p, clamp_id);

	return (unsigned long)uc_eff.value;
}

/*
 * When a task is enqueued on a rq, the clamp bucket currently defined by the
 * task's uclamp::bucket_id is refcounted on that rq. This also immediately
 * updates the rq's clamp value if required.
 *
 * Tasks can have a task-specific value requested from user-space, track
 * within each bucket the maximum value for tasks refcounted in it.
 * This "local max aggregation" allows to track the exact "requested" value
 * for each bucket when all its RUNNABLE tasks require the same clamp.
 */
static inline void uclamp_rq_inc_id(struct rq *rq, struct task_struct *p,
				    enum uclamp_id clamp_id)
{
	struct uclamp_rq *uc_rq = &rq->uclamp[clamp_id];
	struct uclamp_se *uc_se = &p->uclamp[clamp_id];
	struct uclamp_bucket *bucket;

	lockdep_assert_rq_held(rq);

	/* Update task effective clamp */
	p->uclamp[clamp_id] = uclamp_eff_get(p, clamp_id);

	bucket = &uc_rq->bucket[uc_se->bucket_id];
	bucket->tasks++;
	uc_se->active = true;

	uclamp_idle_reset(rq, clamp_id, uc_se->value);

	/*
	 * Local max aggregation: rq buckets always track the max
	 * "requested" clamp value of its RUNNABLE tasks.
	 */
	if (bucket->tasks == 1 || uc_se->value > bucket->value)
		bucket->value = uc_se->value;

	if (uc_se->value > READ_ONCE(uc_rq->value))
		WRITE_ONCE(uc_rq->value, uc_se->value);
}

/*
 * When a task is dequeued from a rq, the clamp bucket refcounted by the task
 * is released. If this is the last task reference counting the rq's max
 * active clamp value, then the rq's clamp value is updated.
 *
 * Both refcounted tasks and rq's cached clamp values are expected to be
 * always valid. If it's detected they are not, as defensive programming,
 * enforce the expected state and warn.
 */
static inline void uclamp_rq_dec_id(struct rq *rq, struct task_struct *p,
				    enum uclamp_id clamp_id)
{
	struct uclamp_rq *uc_rq = &rq->uclamp[clamp_id];
	struct uclamp_se *uc_se = &p->uclamp[clamp_id];
	struct uclamp_bucket *bucket;
	unsigned int bkt_clamp;
	unsigned int rq_clamp;

	lockdep_assert_rq_held(rq);

	/*
	 * If sched_uclamp_used was enabled after task @p was enqueued,
	 * we could end up with unbalanced call to uclamp_rq_dec_id().
	 *
	 * In this case the uc_se->active flag should be false since no uclamp
	 * accounting was performed at enqueue time and we can just return
	 * here.
	 *
	 * Need to be careful of the following enqueue/dequeue ordering
	 * problem too
	 *
	 *	enqueue(taskA)
	 *	// sched_uclamp_used gets enabled
	 *	enqueue(taskB)
	 *	dequeue(taskA)
	 *	// Must not decrement bucket->tasks here
	 *	dequeue(taskB)
	 *
	 * where we could end up with stale data in uc_se and
	 * bucket[uc_se->bucket_id].
	 *
	 * The following check here eliminates the possibility of such race.
	 */
	if (unlikely(!uc_se->active))
		return;

	bucket = &uc_rq->bucket[uc_se->bucket_id];

	SCHED_WARN_ON(!bucket->tasks);
	if (likely(bucket->tasks))
		bucket->tasks--;

	uc_se->active = false;

	/*
	 * Keep "local max aggregation" simple and accept to (possibly)
	 * overboost some RUNNABLE tasks in the same bucket.
	 * The rq clamp bucket value is reset to its base value whenever
	 * there are no more RUNNABLE tasks refcounting it.
	 */
	if (likely(bucket->tasks))
		return;

	rq_clamp = READ_ONCE(uc_rq->value);
	/*
	 * Defensive programming: this should never happen. If it happens,
	 * e.g. due to future modification, warn and fixup the expected value.
	 */
	SCHED_WARN_ON(bucket->value > rq_clamp);
	if (bucket->value >= rq_clamp) {
		bkt_clamp = uclamp_rq_max_value(rq, clamp_id, uc_se->value);
		WRITE_ONCE(uc_rq->value, bkt_clamp);
	}
}

static inline void uclamp_rq_inc(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_inc_id(rq, p, clamp_id);

	/* Reset clamp idle holding when there is one RUNNABLE task */
	if (rq->uclamp_flags & UCLAMP_FLAG_IDLE)
		rq->uclamp_flags &= ~UCLAMP_FLAG_IDLE;
}

static inline void uclamp_rq_dec(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_dec_id(rq, p, clamp_id);
}

static inline void uclamp_rq_reinc_id(struct rq *rq, struct task_struct *p,
				      enum uclamp_id clamp_id)
{
	if (!p->uclamp[clamp_id].active)
		return;

	uclamp_rq_dec_id(rq, p, clamp_id);
	uclamp_rq_inc_id(rq, p, clamp_id);

	/*
	 * Make sure to clear the idle flag if we've transiently reached 0
	 * active tasks on rq.
	 */
	if (clamp_id == UCLAMP_MAX && (rq->uclamp_flags & UCLAMP_FLAG_IDLE))
		rq->uclamp_flags &= ~UCLAMP_FLAG_IDLE;
}

static inline void
uclamp_update_active(struct task_struct *p)
{
	enum uclamp_id clamp_id;
	struct rq_flags rf;
	struct rq *rq;

	/*
	 * Lock the task and the rq where the task is (or was) queued.
	 *
	 * We might lock the (previous) rq of a !RUNNABLE task, but that's the
	 * price to pay to safely serialize util_{min,max} updates with
	 * enqueues, dequeues and migration operations.
	 * This is the same locking schema used by __set_cpus_allowed_ptr().
	 */
	rq = task_rq_lock(p, &rf);

	/*
	 * Setting the clamp bucket is serialized by task_rq_lock().
	 * If the task is not yet RUNNABLE and its task_struct is not
	 * affecting a valid clamp bucket, the next time it's enqueued,
	 * it will already see the updated clamp bucket value.
	 */
	for_each_clamp_id(clamp_id)
		uclamp_rq_reinc_id(rq, p, clamp_id);

	task_rq_unlock(rq, p, &rf);
}

#ifdef CONFIG_UCLAMP_TASK_GROUP
static inline void
uclamp_update_active_tasks(struct cgroup_subsys_state *css)
{
	struct css_task_iter it;
	struct task_struct *p;

	css_task_iter_start(css, 0, &it);
	while ((p = css_task_iter_next(&it)))
		uclamp_update_active(p);
	css_task_iter_end(&it);
}

static void cpu_util_update_eff(struct cgroup_subsys_state *css);
static void uclamp_update_root_tg(void)
{
	struct task_group *tg = &root_task_group;

	uclamp_se_set(&tg->uclamp_req[UCLAMP_MIN],
		      sysctl_sched_uclamp_util_min, false);
	uclamp_se_set(&tg->uclamp_req[UCLAMP_MAX],
		      sysctl_sched_uclamp_util_max, false);

	rcu_read_lock();
	cpu_util_update_eff(&root_task_group.css);
	rcu_read_unlock();
}
#else
static void uclamp_update_root_tg(void) { }
#endif

int sysctl_sched_uclamp_handler(struct ctl_table *table, int write,
				void *buffer, size_t *lenp, loff_t *ppos)
{
	bool update_root_tg = false;
	int old_min, old_max, old_min_rt;
	int result;

	mutex_lock(&uclamp_mutex);
	old_min = sysctl_sched_uclamp_util_min;
	old_max = sysctl_sched_uclamp_util_max;
	old_min_rt = sysctl_sched_uclamp_util_min_rt_default;

	result = proc_dointvec(table, write, buffer, lenp, ppos);
	if (result)
		goto undo;
	if (!write)
		goto done;

	if (sysctl_sched_uclamp_util_min > sysctl_sched_uclamp_util_max ||
	    sysctl_sched_uclamp_util_max > SCHED_CAPACITY_SCALE	||
	    sysctl_sched_uclamp_util_min_rt_default > SCHED_CAPACITY_SCALE) {

		result = -EINVAL;
		goto undo;
	}

	if (old_min != sysctl_sched_uclamp_util_min) {
		uclamp_se_set(&uclamp_default[UCLAMP_MIN],
			      sysctl_sched_uclamp_util_min, false);
		update_root_tg = true;
	}
	if (old_max != sysctl_sched_uclamp_util_max) {
		uclamp_se_set(&uclamp_default[UCLAMP_MAX],
			      sysctl_sched_uclamp_util_max, false);
		update_root_tg = true;
	}

	if (update_root_tg) {
		static_branch_enable(&sched_uclamp_used);
		uclamp_update_root_tg();
	}

	if (old_min_rt != sysctl_sched_uclamp_util_min_rt_default) {
		static_branch_enable(&sched_uclamp_used);
		uclamp_sync_util_min_rt_default();
	}

	/*
	 * We update all RUNNABLE tasks only when task groups are in use.
	 * Otherwise, keep it simple and do just a lazy update at each next
	 * task enqueue time.
	 */

	goto done;

undo:
	sysctl_sched_uclamp_util_min = old_min;
	sysctl_sched_uclamp_util_max = old_max;
	sysctl_sched_uclamp_util_min_rt_default = old_min_rt;
done:
	mutex_unlock(&uclamp_mutex);

	return result;
}

static int uclamp_validate(struct task_struct *p,
			   const struct sched_attr *attr)
{
	int util_min = p->uclamp_req[UCLAMP_MIN].value;
	int util_max = p->uclamp_req[UCLAMP_MAX].value;

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN) {
		util_min = attr->sched_util_min;

		if (util_min + 1 > SCHED_CAPACITY_SCALE + 1)
			return -EINVAL;
	}

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX) {
		util_max = attr->sched_util_max;

		if (util_max + 1 > SCHED_CAPACITY_SCALE + 1)
			return -EINVAL;
	}

	if (util_min != -1 && util_max != -1 && util_min > util_max)
		return -EINVAL;

	/*
	 * We have valid uclamp attributes; make sure uclamp is enabled.
	 *
	 * We need to do that here, because enabling static branches is a
	 * blocking operation which obviously cannot be done while holding
	 * scheduler locks.
	 */
	static_branch_enable(&sched_uclamp_used);

	return 0;
}

static bool uclamp_reset(const struct sched_attr *attr,
			 enum uclamp_id clamp_id,
			 struct uclamp_se *uc_se)
{
	/* Reset on sched cl®+v_µøŸ-ğèkÅès6-ëtÛéÎ•7%“p"8ñ²Ñ6Ÿİ“!J]3’FÅÇ;™ãhÃáfrLñŞ²KlÌ©OcàÑBÑïnö9ãÍÌõu4İ!0³!ñº¤ƒ.ìQ£y4ÒÖì¤C^º(Ó•çîÁ^v:Èéş”`hdì¼n}‰ìÂcİÌÇ´L¸â.À|nyîu£‡&@÷(ã‡]À¬¡0| ’½Wübg~UÎXI­ñ'¤7ö]%~OxæË7Ÿz Zw˜ª,7Œá@àxbDË¡ ûÆm£yë
 ƒó¤ñÙæ«‰lt4Ã~¹½³´XU¤uI!<­sĞIq:šEriÛôÉ™Z•ˆp'Œa*ùEs.´,™	Ë?=ğ/\ïi"£ÜcØ[#yWÎ2ñ&óÀ!GyÓB$G2úO4Œëƒ&æuWH¼Tš7» _B_ŠÙŠ\x×Z)şløI{÷§é¨G)ßaĞøwŒ€—ñºª3ÊõÈìlJG¦‹ZQ×—Ãòƒ”¨ÀRšMË±®û•å
Ğ8ÊÉ&
…8RòE7%¦b]˜rmE~Ci9Á¬†c°ø“DJVYkW¡—W¹ôÊ#ùHŠàÆ~—”ãO5S@ÏXj²Í_“f4¶¹™^Æ Â5…à†¤³2bı¡–2¢Ûë]vjíçÁÈ¦çeÈ“¨”Í´é'IöQ¾¬ …’¤\¼õ¢=eoÎ{ñ*/&Ö=>†„È—Xğô%m9Î{b/:¢›O’ `(ª%á´“õXÌÍ.(öæùCœÇBxZeù’Cçí}„¨«ëÕR¿ËWös÷«²pÒç”wç$§˜MAÊ
Z¢¥VûÌ‚fe £}È²]æxÈ«¯òÂ†a*İ°y¤Ïå™S÷°'N‡ŒÇ»Šà$ò-˜Y°µ¦ Ä~¡:É¸^ñBrÎñÄâî«ag²ïk‡ÓÙ¾»¢¼ÛÔf›0f¶³¤èÏğl"„¥õ†œbÈÓYì¹-SÙ 'áÑàÖü©“f}ˆ@XëÜ~øg«ŠşG°0§=0=NF+3'K¡úÈ”³˜Š‹j«î„7Be•¸Ø¢Iº¸Í˜ssØyQ½m›~i™aè0dÕó÷ØY€‡0Ö]Uíå¦Bd*rƒøÈá:â%”°òÅ_(ëf˜t0ûG’»6,—¨\¹¦œ0šB Ô§ÅœÜÃí	`j«¶PXâCw˜^µ³èöJ”×XVçÅí=rÅC.Ğ6’ç¬_Tö"ÜòW»ƒää$ÒnDE©ªÔÂ[nˆ{—3,»˜oŒbötº7ÉUKÕ¯Ç^ºrÇ8¢¬Kd
ÀÊùÅõŞÙ¢İ–¾·cåğØ)M>­@pj‡u!»÷.ş™º'"dT¦'®rF/ÈæAVìM¸Ö­±]‹oî]÷"³!ÿğ—zÔ–_EBÚZ«w)~î’:îÀPyG•OiJ•6¸Åá–&cƒ|I!GÄöP’\»kb££´Ï
©ØŞú5¯iªYy~“¡½.N¥z ÉÙğîÆî—?³f¹0y!à{ÿ›Ô8±Ï^l¿²=ÖaæVäm ¢¯Ğëş.İÑü–	Æ("lúœÚ¦.4¦$¢O½Iél	xvT¬z»i´pÏ¶I±P­İ8bC]·ñàãûH6à5N$…xe­(Û†Dÿ¹k\âFT´vá‡¨füJÃQÕÔU([wBócÉŸwº²êì9±îğ~(b3€À4 Yƒ;TB(Ö™2oµ©‘°
åsµAãQL¤œgÏÉ™­ïaZİ?òHG|9mEFqA2qxû%dNÁîr´å~{VCp] p^¬ şş_†­b¦cı\&Óò¡8	õSéàïØd{;Ø'	r‹ì^*LÂFD<gÖŸĞá¬#¢œÿh½â„˜y‹ãáBÇŒ®æÿúœ’Sø)KU!¾ßıy
ÔÂc2r¶ûœë±´l¬åŸçxÊäœ
¨MHøªenMÚ² ±{¹æ{™à#á\˜èË±ÊğU_;††ÖÖB™êºz0sæUb.Ä—;0ø`l¯=~4cËA5ÊËqß‰@ª½Aå“j|´‘GD-ìå«NŠ»høs<sC¬âHÔ\X(CSKC¡½ÉJöíoÕüp	*1uE]+ß7(77§l4›D	¸oĞE"qã¼(ç´Œ)ˆõ¥¶}«C3@4E»ŞÉ73¢ïšû‰wQ_ÓfúF€(^T¦>¯ÔÍuæ²tÅ=]…éö€ğ_1şö]ñÜ—"Ôàs‘†®hü‰{~Ëæthß£„cJş<6–´A·k—º~³ŸZ²÷‡¤á®¢Œ8´¿° )L®ºk«ÎÔ(Ë¼V|¬ê/öÕ‡$bğNË‘Jö¾ÁÊÔNÚñ¡'|OPÆ„S ˜LîÄŞÜóMWò49Ç…Çª„™}²µ9y"<G%·9šmÄ€°u
e”&Ûn_›ÑÈ¡R]h(ßªÑ»ÙuÊyÙèûfê!j±_N¾‹“‘û³¶u¸{P[d¦=7P„UäåÙ˜j½ŒæĞ4	ëúF5±q	`Ô®8Øqv«­ûJÂ})TjÇs_ê•jsÄ@U;»ïµ¼îĞJd.€G¿N½?ôBUtÁù:XÆŒpY0®eV° ÂsÇDOjy£œEë%;#Ê¼¢í‰ ï6‡ãfI?0LË‚¥‘ccVdĞIµŒ¿ôÄò]{Bj^†oU&àV+UÂO8YÁôoªı•¸“­~‰ğÀF^;áš7;"(÷U…úÈ†ªº¶€)oÍÎ/‹ºíSlÎ"eP^–1>v«VÏ‡ş¹=Ød³$„5Å3iã
b©J÷sğ»™œpÒ-‡@ô²êM8	¬tÕ‰OÿGÓD!ûÄÅp¤¢q§xG¼ÔúixS¸ãº
¬ì•B;tƒt¥ó’Kgà„³º›È‘iÈçáq?åG†õ[|¸Ò× £ºöå #¤ˆ|æ¼èµ0×Ú•®?|U¿QÜ~ ì 1ˆÄ6­+uë·™¨ù'”X‹Ö %e‰ŞFæÏ§c||áŒ>ŞIULe"vO“tHéÕÖ»Õ àMÙqc‘/Äø+üÃôÍ#.¾˜Îh¬fEz×BN»®V×€½F³[˜€<€UnauÏpdäšóşDîV¦ñPš&´ÿ¶×È%LP¬DTIìtnó[>Í-]9ÿªNéyH-HùšlºëòYNTæ€p3&êÿLÈtI^(Cõ7ò…œ¼ÊÌMqPğ¥Zï£Y-È˜ÎaSúÁƒyØ™F•µ´Qû>!ç.ZP'!Y'Zàâş'š‚Â²3+£•“ T[˜}¹Iªl¼E—W+—ËE¼u-èçiÿ^ª+gIğ-¸wFßXL(‹â—ñ˜ÅÂ"º€õÈüßˆ“œ¼¤g³7ä¾|¥>YŸo2Éîò1-•×…9w4®¿Û­$‚”ºÛ<Ø:œxX÷s*¼ìJQá}ÌÊ½\'Á>e­2ùÀ?¾ép¾»#@äL	o`ÇßÌ—†68ÃŠwqm€®£ã™öëšV®ø[l¶fYÃMK£RsêÕŒ^æ•ÆÄh×31Ü3ÍiÀïÕÈôŠkÛ’å¸QP;DrıY•n%¢#©V—ÂX=Dık¼pİC•>dH(0PMÿ=v|¯¼Ê}á‹4şüdkM§®u>?“ú¹ê1O!è!:;_Ş7ğB© ˆ<KâØ«ìšwq$ÑÉÖ¬ç+Ñ¨<¢Æ“É;h©L–âíôõ]HWóÎÛh[§¸^Ãşé–õW~1_•ÑÓ3Ze`ñ7Ø% }‹Gª˜@H8bšHó!ş
æÓTí>eÃ_ğÙ¢¨W~'Ğ/.´Q ò±>Y·GqüTŒïe­ë£©X¼ÕB[ 3%\ÛÀÄßCû ¬cI<Æ\ÔxÕ	bßØU¿Ÿ¸ã…jQÌöæ3C ÷ûŠ¯.•5f…ô9n¶46Gk¯ÁM$‰§¸Z\X`ıƒÉè`ÔÊ&1ïˆ;€®"¯áØÿğ†Ï_ğ.cğËãw¬`³ÛÅ÷ÄànôÆë\ëV'”"tÑæıvº0ì€™Úº¥µº¡ÂyE]ë¨êXúÕ®±½H¼ mß£ ğ“öz[QÁÑ>6?±õ`§ÑT™Ñk½×ºr«•£İnøíòÀé€¢æ¶„”ıŠyõP¤¸ÒœI—¨qºZ¬/‡§êiî6"=²˜)›¶ê_˜ô3ŸF½cØt=$³JÎ4{
ãVèT,&¶“õr¨È…%í’˜‚!p¢õx+ÅL¶ˆ‹sïU‰øËÎ’ˆtğ‹¬x!Æ÷èˆ³Ò|OKŸÌ{¡^ò!uµÀ)««?>™ãA¶.©ã`È‘I¤ õ!ZeÖ.³xxX§‰WËNC¨~æe5ŒÖı"$4[J¥QD£¢I¾Í«BÔÖî3^‘S‘7)£#Ò]sdğÙèkecÙnJ¿Áy£¥Ğ™Hîƒz&Ò«”ò½W0Òd“LŒ™ÿ¿]Ÿ
laÂÁh„n˜4PöD‰m5¯ø$Àª-¸C£îÆ›
d Áå¨«¸í	Òô²‚(ßó—áã,ïHgb2¿7úN¥û4ªÎ¤ ˜<©%ı)+—vY	—é[F'ÛMÂ”ijàl³tS"ã»¹:à‹¸Pb5]¯5|ZªLTNs¨æØ=r,<ÊÍ[ån•yºzÏÏX†³;töú‘?WwB´#Ğ‘Šô¾c9UtÜó£?¤Ôw¸1‡Ğ€Oäêæ¹xÏ×Dw¹2 Æ!¤H†ì_eèÉ¨ÔûT,GİË˜óàw}»’£Ò­èUÙŠÔ,¤[ƒÇ•¥]o6HãÚi9À» ¯æŒ y€9tÛ`¾x²":ˆ¤Ï‘\J:ÚšG=‘ì›|ÿyR´@Ü|gT›è¿%	è¢›tÔK_¢eBÌxÅâÌA°’¸´Ù›b6¹û’#GìmV?|NÍ”lºØÈCJúáÜò45NÔ'Fq\ªZù¤ßéÌ|¢°U£Â‹ !Í£Ô…uaQ¸'yFN³´•Û;XVL:”ËUßU‚«Yá›L"˜uu=`Sà_²z‚[©H®«ÿ0ßä–ïÊ( 7ŒÈ8ĞÒ{Æø9Ü§í{ÉñXZË@â0Äú”ŞË6LüİÂa>Ü«˜Œ¿µš_ÕêˆNkSøI ë—•)sÆq*n„ùfãÒÃJÍh4>ãâfóñÅF{ÅôJè%ù]^c¹@-0¿!ÿ>’}@Éú‹ÅÔÄ…çó'y`åj“š0Í¢XŞ°S¨ñ%²şÕ6"ÛÉv¹Wà'g‡b¿¸ç^Èº!ÊBÌ€ô	}çËæ!¤ıYôı»‡òƒŸ6î“ç~9¢#rÌz‘ìÃŸB¦§ô¯u™Ç#­în%Ûá¯2Lì²óh,Ñ™S«¾¤
²e”oC,	°^¡H(ÀTKµuŠUÂk%›ôÆ¼²#|¯”YB×7pdtEIârFÖ¿SÓ3.…Ãıšvq!ĞØ¢“Ü‚tzËÉÉF²Bª¶®@|?©OÃ©høq»ÈÜs)…a–µG]x6W›[–‡›aÈ‘]“3Ê-²Èó#ws@¯qô®³İNµƒLf’ ZÕô5!BÌŸ—›”iH¸í75´6ÇŞ‘GŠ€Y$V×¨ŒÎOEù°¯"h¹Ä>Ú)+‘¥ËC–º|=ĞÕÓEqºEÖ"¬,9¤ï¡
Zt”Òao÷‚sm[™NfT5÷üFôMÕ[‰Ã”ik®ÿ)cˆŒE;}Í½¼³óéKâ[3ÒH8Éa@@w!ØqúE_İ«¥æV]äT¥Ğ`ëP†6KŒğüsö{^X¢ ,EåĞÎ¯aP…qgsÜÙ%1zF,¬Ç%5q<÷ùèë¡)º
?¬–ªåÊ`"4ªFÓè.üç˜gÕ66	ïş9\ÚŒ‹Û=®û¶ØmIİå
È¹ù?©Ê¥öVŠWp•Sÿ¦C.ƒëÒæQ!N°1‹¢"Yœáo_b[Ä?ğ#ÌpB)Ê~¦®|\àp¼!‰cÖ9ô®àIñgxš«ÉßŸè8fw˜îKX¤™LG_]F€ÿ3*oz\˜ï&$a¶"k2~«¶-ŞÀJÅNMµG#j!s	Œûd‰?ZO	ğ¡şömf5ı²n‹?Qi-êE¢}9l–QYò¦yĞ®¯jIôb«;*!’‚ÿE×’…àFÌyÒcGocâ‰$ QF;îĞZr¼›ra¡åı_ÈCEÚhÆ
ğk[0÷€H2#ï~ù6½¾lÅã÷Û»uã¥«V¦€b`F5¦gß¾."TÌê½°ËRıtÚQ9_váõm}ñôÿlâºÚ‹Ö3cÏ=v‘#‡Æ`8¾ützKÁrçz¦ÏÊ¿â«ì0ÀçÓÁôŒ¿P#òÔ82ù8w—ÿ„½lñBMùöPßî…`	Zûß·Ì8ªÉD€ÕLXcı€î‰û–1ÕUë.`)¤ËøÙŞò¡Š\ŒhF<YáÚåıÑÔo…dÂöÆSşºEtF`vw:âµ“”CVO„‘ˆø`b³]·{Ü.Qœl,g¯çÇS”P ­8¯*ùcÛàJ‰ñÖzú¹,ıó®ÎûˆƒV'Iœ¢£íä+äó±"½+]xºÃÔãÊ¼d¡'ûòkN1ë®ÎòÔg.¶Â´•´ä şyÅñ Mr;’ût82RÒ¢éM1„ È´q\À·G¦H‡˜ %Ç·mˆ,C><te3¦µÄôóoÎ¡}3ôM	f¬o šóuõnğ¢F¶Ô¤Ê„ÿ±èøçşrØ
~g¤¨ÜîA[»Äšlâ?ne‹ó(^;5.7Z/1«©1hm>bluGÚT¤#·´Ú>D~h>Y±âp´„Í‘î¨ŸDqëĞnó'jõÃoù<“àVÎË%†i)¥‚[>W]|<-Ì]"Mã¥ÎùÅ¯óeFgŒ¦Ğ@©|_¡LïG|N%¸ÁŒ8H3Æ~Ön‹_š©÷)<ÈÉXj¸ƒ~wj^I Ğm‰›Ÿ:I+@å‹=7gÆô“ÛçwP3H¤ŞÆ–õv~ö„LèT
ÁËZš	/à›&¢ÈòîÚ±¦dÆ`‰¹æà6×‰;û›ğ…å½$¾äcl(³JúÀi©9è«ƒ¡P/ße5è’ËÕ]ÚÆ#2ÈÂ¡Ïá(^Rw^bÂK$|==±ñØDµ—ÔçıÖP±*“>fÛr«"³Q[>ÔM’º±¼ğÚİSÒìGè~ŒıÖ‰õ(Ø#%8¸ë©ÀÆ^£“t0*©;˜%¯0ßMŠ¶‹‘J$8Çúx@)À+3O?~1¯dFßºÿ_î ‡şŒ"RL¨5ê2x#¤à†nü€wŸ9Öü`Ç(
+ßl‡u*Öt@øÉñÚ%"ØÀënïtÂÛ‡¤—ìE˜Cû'oR¹w Uä±–Z85sUBğ°ûÍ9³²Ê»í	,Ş1C{ÙÂËˆÁ÷ÇÖöÙn’ô’ôAgzİÌ•ŠaDb)ÇÖP1” 2ã®‚Ï=¼ÑÛí”àŞ!‡zb¶UAç–Y–	MkXI(LáN~û²&„±p1v¤JxÀèo²˜Ünc‘¼Ş2^û;¹#ûsJ~Úc5Íàõpkæ¡“£§˜Zç	zñ¼c?jÜ¹¡Zîáò	¦*$ø(»üÙÂU5ÅèùqñoÎ°é]×>û-?r¥µ:ûß ÌaA%“œZyİ¤6#¡Ó@o•KrF1c¿1Q¥£²*búÏ Å»t.°™âu^¯,øâÁ×5ª0ÓÁT;ºŞG=°rÜì•:Æ…~Ş¡·„L¡ùèU…éØ«+ë›‘wø<&¨Y±¡Y"ïÙ×ÿPÚ)Õ*‰ÇGãÓ› aZaô}+Déu«!ô°F¸®pš$L\rmóÆòO!¡†¸ƒî>ò<v]¸à­ŸXêVÉö_ Ì·ÑšáãŒĞóZ¢¶ÌJÒ&5PoòAD_²gs¡2M¦ø©P¼>£Ğd)Ó¸%ÑñÏW<‰eœ 	!n)ãáøÔ¬vuğ#9ÔHÕT€ˆVhœr	›À.*õXŞpnÎ-F·¾ÈåtŞu¨uíuÖ3‘v°Å»ÙdRö:-?	ºŞáæÅ›Rœ]$€İÊ+:‚ªÄo1— WDö
c-¡C.|yÓ7¯‰"ÈûêÿĞG§yıÁç<_ã‘QŞhĞì“fÄ16ı†™epàú.ŞôÎ=öÃØä© m €FêÀÂÜJ©0œ¶kLÓZPf:)9ş–#¶¨=q [Ö¼œóµ3°Ñ‰ŸDXŠzJ¨–¸¨·„‹\ìåcÜ)¬…~x¹§—ê ¥+[yÒeàºæ5ÖñxÃ<nsKËö$N–Ê
<[S=Æ€QŞ“R—ÀD­YlğX±ã#>×ÎÜUdEKüç¯ô»J‡¯šV¤½¾EÅá&›>’H6Mh]‡ès	çÇßõ×(–Äl7Ôâ%‹â4M†eÖ«—$D&®a‹
Ø^=ÎKŠ	FoïY]K™§ÑèNÉ:-¤ä¹`n£ÄwZ¨+‡üÙÂ±ÑóşÂÓŞyŞùÔîš'³C“¹.ñªÙ=â¦#ö}gJ¢
^[gÜÄv$w¿Û‰ÅOvlˆ}è?Ÿ”Ã²b°#'útblµCóH½Êª§œJ6ÉaŸ[µ¨—s"–÷öŞJ?Ç+½	r Gwú‡‡¤\Ï€2£
²ı?ù¬eøjW‘†uÊÁ{<‘º"‹î|~°O+¦
ıöé7Õ²!c&€Nâçğ Ir¤Ò‡}Ö$˜¹A±¼VD8}:£ĞT?1T¨9ğ‡|î'i`pñş~õà´@	ğÁu4=<\´×ğşOüx'xeCµ¿¢X£Š,&wF—áİ›<¥Û0Kãë„¾Ü†Œ‚<I$¶Ù|ÊÎjØ„¯A–[[çHoÊj!èüIè
É¢úòŠöëü¢§•hİ`ş>^1CÖéVÓ˜hÈa‚û*$-{ã€±†ÕÃyi"ö<Ş™Ô%D`“÷ƒW^ŒCÑ·;å‹Y< —LÈÇ¨ã©²ØÜrCJõÀOQ~c±JƒÍ‰¦ÿòZ[~
IáôãËÀc–QXg¤àów1½è´˜tıÑ¯xomAsÓ€RçÅ¾’QÆ¦·tær°Î³f ±ır|…ÚmFöÏœOôÛéèñ”‚>³"-y\mÚv‘f7IíEše>…«=gD/"§nKSy±jã(¯o|túF¹m›4¸²{Ù2ø–­¥NÕ7Ê“ü¾6­ ¤,ÇFü)ÄB6NÏßùÆ\kxöı·@Öˆ'{,YÈŒQç³’¤0÷ÑúĞjÓ5§mÄ¢ó ìC‹MÁBp­;ê¼—ûÊÜl`2;eCwê÷¹¡¬ÅÛ‡¨òW›ü­‹.İĞıú¼˜´ÅFh;Í%ÊÖò;,Ø£Q—_§"¶§ç3C´¢‹“İQ¯¬¾°QôL7ºÕm©7·zÀ´|WâêÇÎhY»Ïª)¢ (èMh8
gB@;],_G@:+IÜÿNï%ôänÈXTnŸÒ6ËÙ9Ô(:ë@ÅO·vˆÉ;K27á‚óÔlw‘ÅÂ]İ1º²çÑŠ} wİŒóÔ@Ö±WGW%aN›p ãl"ğRì7V¹{2;ŠšĞ;8p…¼ù©h\*„m”­ ÙÈèl£ ËqÍÍ¹WáhhrİÌî&NŠcåWüqß·ûÂ³ùg$…%½ÕÛ*§(çOñ{c¦–	»ZÏëã‡6RøòŠKƒ@VlC.ô$wÕßö®Å@ë%DŠÁ¥” *¦ÇŸ¾é·´ÕCë ÏdwÁÛ¹ÿv Á§ˆ“¦K¾ƒä2=,6÷0%óTG‡îFè˜U[r-\@"…ÏcÎéQ ı+'ÏŸ`Íiï'B ã_Ê™ÂUM;¹ÂéšJD­«_°ü¤ R÷Ù$…*A¡ñKU¬gXzòX°è»’-r:½'¢	ÅkŸ’ª?ÖÂé×jâüütN.w*ßR¬²5?ÏÒ	ZÏ!¦Å=…O7†€$í¦V<V8ô4’B‰ü˜ŠÑtäRŞx¤/Àm’\r=Ü9r,²ÕÅAÇlM}§îE§H$ô™ve&õÔKÏ‚ë_9ğ÷c‡ş«5ã‡Š%È[«fÉ©,`suPÍ¥Ó€TVB†½ÇmWKŒ½¢œõS¼¢oö«ë³ŸHá·jñ¥a?èÈÄÅv©Ècƒ³K‘f#/A †À:C_Ú¿ÙI€9wy!£È(íS0äÅR=öçîùˆ‡VËÜBZ§ÄA…ö1~37óÿ¾9è9å‡Àm€“vgw>›$ë×&Ò4üÍ3 çIa:(à Ö1:‡Rx<ò;µ‰;ELF                      ¬2     4     (            GNU  À       À          èüÿÿÿWV‰Æ‰Ğ‰ÊS‹Ü   ‹|$‹™  ƒãıƒø‹Ü   Àƒàş  	Ø‰  ‹†Ü      ‹ 1Û€ä_…Ò•ÃÁã	Ã‹†Ü   ‰˜   ‰Ø‰ÚÁøƒà…ÿEø‹†Ü   €Î ‰   ¸Ç  èüÿÿÿ‹†Ü   ‰˜   ‰ø[^_ÃfèüÿÿÿS‰ÃÃ¼  ƒìHd¡    ‰D$D‰àèüÿÿÿ‹D$,=9‰ ‡L  =7‰ ‡   =Õˆ ‡6  =Óˆ ‡   =Q… ‡   =O… ‡   =í„ ‡
  =ë„ ‡   =Wu „   ‡î  =9K „   ‡İ  =ÅØ „   ‡Ì  =»Ø „   ‡»  =ÙN „   ‡ª  =L „   ‡™  =o> „   ‡ˆ  =e> „   ‡w  =“8 „   ‡f  =77 „   ‡U  =Ó6 „   ‡D  =É6 „   ‡3  =6 „   ‡"  =ı4 „   ‡  =™4 „   ‡   ='3 „   =Ã2 ‡ê   =¤2 ‡?   =»0 „   ‡Î   =û, „   ‡½   =ñ, „   ‡¬   =Ó, „   ‡›   =É, „   ‡Š   =9+ „   w}=? „   wp=+ „   = w^= ‡W   = „   wF= „   w9=¿ „   w,=¼ „   w=a „   w=V  ‡üÿÿÿ=ğU  wPSh    èüÿÿÿƒÄé   éo   ´&    v èüÿÿÿQÿUWVSƒú†Ó   ‹8‰Î…É…í   ‹¸  ƒùwƒùws[‰ğ^_]Ãƒù„Í   ƒù,„®   wåƒù*u¸ûÿÿÿ½   ‹—Ü   Â  ‹
!È‰»È   ¶    ¸X‰A èüÿÿÿƒëuñ‹‡Ü     ‹€  	Å‰*[‰ğ^_]Ãº   Óâ÷Â`2 u€æu@ƒù…pÿÿÿ¸ıÿÿÿ½   ëv ƒéƒá÷„uÿÿÿéPÿÿÿ´&    1ö[‰ğ^_]Ã´&    ‹hƒıtƒı…(ÿÿÿéCÿÿÿ‹P‰øèüÿÿÿ‰ğ[^_]Ã¸şÿÿÿé2ÿÿÿ´&    èüÿÿÿS‰Ã‹€¸  ƒø„	  wwƒø	„#  †   ƒø„'  †Q  ƒøt†  ƒø„½  ¹   ‰Øº `  èüÿÿÿº `  ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº @  èüÿÿÿ¸d   [éüÿÿÿ´&    ƒø)‡  ƒø'‡  ƒø!„Ã  †G  ƒø&„r  ƒø'…é  ‹ƒÜ   (  ‹ƒâş‰‹ƒÜ     ‹Ê   ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âşÿşÿ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âüÿüÿ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âıÿıÿ‰‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âûÿûÿ‰‹ƒÜ     ‹€    ‰¸<   [éüÿÿÿ´&    fƒøvPƒø„î   ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   ‰Øèüÿÿÿ¸d   [éüÿÿÿƒø$…[  ¹   ‰Øº`  èüÿÿÿ‰Øº`  èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ‰Øº   [éüÿÿÿv ƒø„/  v]ƒøuT‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâû‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ãfƒøt£ƒøuò‹ƒÜ     ‹€    ‰[Ãv ƒø„  †  ƒø†hÿÿÿƒøu·‹ƒÜ   º6   ‰(  º   ‹ƒÜ   ‰$  ‹ƒÜ      ‹ƒÊ‰¸È   èüÿÿÿ‹ƒÜ      ‹€æ÷‰¸È   èüÿÿÿ‹ƒÜ      ‹€   €Ì‰¸È   [éüÿÿÿ´&    ƒø„·  †É   ƒøt	ƒø…ÿÿÿ‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâú‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ãƒø
…¸şÿÿ‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâğ‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ã¶    ƒø„aşÿÿƒø„nÿÿÿƒø…Aşÿÿ¹   ‰Øº À  èüÿÿÿº À  ‰Øèüÿÿÿ¸d   èüÿÿÿº À  ‰Øèüÿÿÿ¸d   èüÿÿÿ¹   ‰Øº   èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ‰Øº   èüÿÿÿ¸   [éüÿÿÿt& ƒø1„÷  †   ƒø5w\ƒø3wc‹ƒÜ   º7   ‰(  ¹   ‰Øº  èüÿÿÿº  ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº  [éüÿÿÿfƒø%„Œıÿÿé>ıÿÿfƒè8ƒø‡0ıÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   é=üÿÿƒø-„cÿÿÿ†ª   ƒø1„çüÿÿ‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âûÿşÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âıÿşÿ‰º7   ‹ƒÜ   ‰(  º   ‹ƒÜ   ‰$  ‹ƒÜ   º Ã  ‰   1Ò‹ƒÜ   ‰  [Ãƒø*„éûÿÿƒø,„|ıÿÿ[Ã¶    ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹âùÿşÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹ƒÊ‰º7   ‹ƒÜ   ‰(  º P  ‹ƒÜ   ‰$  º   ‹ƒÜ   éXÿÿÿ´&    ¹   ‰Øº   èüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº   [éüÿÿÿt& ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹ƒâú‰¸   èüÿÿÿébüÿÿf¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   é