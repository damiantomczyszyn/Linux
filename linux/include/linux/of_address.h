x], Reg=[0x%02x], data=%02x, TS config = %02x\n",
			     __func__,	state->ci_i2c_addr, 0, buf[0], buf[0]);


		if (buf[0] & 1)
			state->status = DVB_CA_EN50221_POLL_CAM_PRESENT |
				DVB_CA_EN50221_POLL_CAM_READY;
		else
			state->status = 0;
	}
}

/* CI irq handler */
int netup_ci_slot_status(struct cx23885_dev *dev, u32 pci_status)
{
	struct cx23885_tsport *port = NULL;
	struct netup_ci_state *state = NULL;

	ci_dbg_print("%s:\n", __func__);

	if (0 == (pci_status & (PCI_MSK_GPIO0 | PCI_MSK_GPIO1)))
		return 0;

	if (pci_status & PCI_MSK_GPIO0) {
		port = &dev->ts1;
		state = port->port_priv;
		schedule_work(&state->work);
		ci_dbg_print("%s: Wakeup CI0\n", __func__);
	}

	if (pci_status & PCI_MSK_GPIO1) {
		port = &dev->ts2;
		state = port->port_priv;
		schedule_work(&state->work);
		ci_dbg_print("%s: Wakeup CI1\n", __func__);
	}

	return 1;
}

int netup_poll_ci_slot_status(struct dvb_ca_en50221 *en50221,
				     int slot, int open)
{
	struct netup_ci_state *state = en50221->data;

	if (0 != slot)
		return -EINVAL;

	netup_ci_set_irq(en50221, open ? (NETUP_IRQ_DETAM | ci_irq_flags())
			: NETUP_IRQ_DETAM);

	return state->status;
}

int netup_ci_init(struct cx23885_tsport *port)
{
	struct netup_ci_state *state;
	u8 cimax_init[34] = {
		0x00, /* module A control*/
		0x00, /* auto select mask high A */
		0x00, /* auto select mask low A */
		0x00, /* auto select pattern high A */
		0x00, /* auto select pattern low A */
		0x44, /* memory access time A */
		0x00, /* invert input A */
		0x00, /* RFU */
		0x00, /* RFU */
		0x00, /* module B control*/
		0x00, /* auto select mask high B */
		0x00, /* auto select mask low B */
		0x00, /* auto select pattern high B */
		0x00, /* auto select pattern low B */
		0x44, /* memory access time B */
		0x00, /* invert input B */
		0x00, /* RFU */
		0x00, /* RFU */
		0x00, /* auto select mask high Ext */
		0x00, /* auto select mask low Ext */
		0x00, /* auto select pattern high Ext */
		0x00, /* auto select pattern low Ext */
		0x00, /* RFU */
		0x02, /* destination - module A */
		0x01, /* power on (use it like store place) */
		0x00, /* RFU */
		0x00, /* int status read only */
		ci_irq_flags() | NETUP_IRQ_DETAM, /* DETAM, IRQAM unmasked */
		0x05, /* EXTINT=active-high, INT=push-pull */
		0x00, /* USCG1 */
		0x04, /* ack active low */
		0x00, /* LOCK = 0 */
		0x33, /* serial mode, rising in, rising out, MSB first*/
		0x31, /* synchronization */
	};
	int ret;

	ci_dbg_print("%s\n", __func__);
	state = kzalloc(sizeof(struct netup_ci_state), GFP_KERNEL);
	if (!state) {
		ci_dbg_print("%s: Unable create CI structure!\n", __func__);
		ret = -ENOMEM;
		goto err;
	}

	port->port_priv = state;

	switch (port->nr) {
	case 1:
		state->ci_i2c_addr = 0x40;
		break;
	case 2:
		state->ci_i2c_addr = 0x41;
		break;
	}

	state->i2c_adap = &port->dev->i2c_bus[0].i2c_adap;
	state->ca.owner = THIS_MODULE;
	state->ca.read_attribute_mem = netup_ci_read_attribute_mem;
	state->ca.write_attribute_mem = netup_ci_write_attribute_mem;
	state->ca.read_cam_control = netup_ci_read_cam_ctl;
	state->ca.write_cam_control = netup_ci_write_cam_ctl;
	state->ca.slot_reset = netup_ci_slot_reset;
	state->ca.slot_shutdown = netup_ci_slot_shutdown;
	state->ca.slot_ts_enable = netup_ci_slot_ts_ctl;
	state->ca.poll_slot_status = netup_poll_ci_slot_status;
	state->ca.data = state;
	state->priv = port;
	state->current_irq_mode = ci_irq_flags() | NETUP_IRQ_DETAM;

	ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0, &cimax_init[0], 34);
	/* lock registers */
	ret |= netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0x1f, &cimax_init[0x18], 1);
	/* power on slots */
	ret |= netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0x18, &cimax_init[0x18], 1);

	if (0 != ret)
		goto err;

	ret = dvb_ca_en50221_init(&port->frontends.adapter,
				   &state->ca,
				   /* flags */ 0,
				   /* n_slots */ 1);
	if (0 != ret)
		goto err;

	INIT_WORK(&state->work, netup_read_ci_status);
	schedule_work(&state->work);

	ci_dbg_print("%s: CI initialized!\n", __func__);

	return 0;
err:
	ci_dbg_print("%s: Cannot initialize CI: Error %d.\n", __func__, ret);
	kfree(state);
	return ret;
}

void netup_ci_exit(struct cx23885_tsport *port)
{
	struct netup_ci_state *state;

	if (NULL == port)
