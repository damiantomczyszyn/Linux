tore;
	int mem;
	int ret;

	if (0 != slot)
		return -EINVAL;

	if (state->current_ci_flag != flag) {
		ret = netup_read_i2c(state->i2c_adap, state->ci_i2c_addr,
				0, &store, 1);
		if (ret != 0)
			return ret;

		store &= ~0x0c;
		store |= flag;

		ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
				0, &store, 1);
		if (ret != 0)
			return ret;
	}
	state->current_ci_flag = flag;

	mutex_lock(&dev->gpio_lock);

	/* write addr */
	cx_write(MC417_OEN, NETUP_EN_ALL);
	cx_write(MC417_RWD, NETUP_CTRL_OFF |
				NETUP_ADLO | (0xff & addr));
	cx_clear(MC417_RWD, NETUP_ADLO);
	cx_write(MC417_RWD, NETUP_CTRL_OFF |
				NETUP_ADHI | (0xff & (addr >> 8)));
	cx_clear(MC417_RWD, NETUP_ADHI);

	if (read) { /* data in */
		cx_write(MC417_OEN, NETUP_EN_ALL | NETUP_DATA);
	} else /* data out */
		cx_write(MC417_RWD, NETUP_CTRL_OFF | data);

	/* choose chip */
	cx_clear(MC417_RWD,
			(state->ci_i2c_addr == 0x40) ? NETUP_CS0 : NETUP_CS1);
	/* read/write */
	cx_clear(MC417_RWD, (read) ? NETUP_RD : NETUP_WR);
	mem = netup_ci_get_mem(dev);

	mutex_unlock(&dev->gpio_lock);

	if (!read)
		if (mem < 0)
			return -EREMOTEIO;

	ci_dbg_print("%s: %s: chipaddr=[0x%x] addr=[0x%02x], %s=%x\n", __func__,
			(read) ? "read" : "write", state->ci_i2c_addr, addr,
			(flag == NETUP_CI_CTL) ? "ctl" : "mem",
			(read) ? mem : data);

	if (read)
		return mem;

	return 0;
}

int netup_ci_read_attribute_mem(struct dvb_ca_en50221 *en50221,
						int slot, int addr)
{
	return netup_ci_op_cam(en50221, slot, 0, NETUP_CI_RD, addr, 0);
}

int netup_ci_write_attribute_mem(struct dvb_ca_en50221 *en50221,
						int slot, int addr, u8 data)
{
	return netup_ci_op_cam(en50221, slot, 0, 0, addr, data);
}

int netup_ci_read_cam_ctl(struct dvb_ca_en50221 *en50221, int slot,
				 u8 addr)
{
	return netup_ci_op_cam(en50221, slot, NETUP_CI_CTL,
							NETUP_CI_RD, addr, 0);
}

int netup_ci_write_cam_ctl(struct dvb_ca_en50221 *en50221, int slot,
							u8 addr, u8 data)
{
	return netup_ci_op_cam(en50221, slot, NETUP_CI_CTL, 0, addr, data);
}

int netup_ci_slot_reset(struct dvb_ca_en50221 *en50221, int slot)
{
	struct netup_ci_state *state = en50221->data;
	u8 buf =  0x80;
	int ret;

	if (0 != slot)
		return -EINVAL;

	udelay(500);
	ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
							0, &buf, 1);

	if (ret != 0)
		return ret;

	udelay(500);

	buf = 0x00;
	ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
							0, &buf, 1);

	msleep(1000);
	dvb_ca_en50221_camready_irq(&state->ca, 0);

	return 0;

}

int netup_ci_slot_shutdown(struct dvb_ca_en50221 *en50221, int slot)
{
	/* not implemented */
	return 0;
}

static int netup_ci_set_irq(struct dvb_ca_en50221 *en50221, u8 irq_mode)
{
	struct netup_ci_state *state = en50221->data;
	int ret;

	if (irq_mode == state->current_irq_mode)
		return 0;

	ci_dbg_print("%s: chipaddr=[0x%x] setting ci IRQ to [0x%x] \n",
			__func__, state->ci_i2c_addr, irq_mode);
	ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
							0x1b, &irq_mode, 1);

	if (ret != 0)
		return ret;

	state->current_irq_mode = irq_mode;

	return 0;
}

int netup_ci_slot_ts_ctl(struct dvb_ca_en50221 *en50221, int slot)
{
	struct netup_ci_state *state = en50221->data;
	u8 buf;

	if (0 != slot)
		return -EINVAL;

	netup_read_i2c(state->i2c_adap, state->ci_i2c_addr,
			0, &buf, 1);
	buf |= 0x60;

	return netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
							0, &buf, 1);
}

/* work handler */
static void netup_read_ci_status(struct work_struct *work)
{
	struct netup_ci_state *state =
			container_of(work, struct netup_ci_state, work);
	u8 buf[33];
	int ret;

	/* CAM module IRQ processing. fast operation */
	dvb_ca_en50221_frda_irq(&state->ca, 0);

	/* CAM module INSERT/REMOVE processing. slow operation because of i2c
	 * transfers */
	if (time_after(jiffies, state->next_status_checked_time)
			|| !state->status) {
		ret = netup_read_i2c(state->i2c_adap, state->ci_i2c_addr,
				0, &buf[0], 33);

		state->next_status_checked_time = jiffies
			+ msecs_to_jiffies(1000);

		if (ret != 0)
			return;

		ci_dbg_print("%s: Slot Status Addr=[0x%04x], Reg=[0x%02x], data=%02x, TS config = %02x\n",
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
		0x00, 