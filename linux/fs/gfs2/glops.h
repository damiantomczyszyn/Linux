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

	netup_ci_set_irq(en50