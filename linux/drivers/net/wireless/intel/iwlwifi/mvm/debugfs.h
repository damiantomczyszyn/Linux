t("%s\n", __func__);

	if (0 != slot)
		return -EINVAL;

	mutex_lock(&inter->fpga_mutex);

	ret = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, 0, NETUP_CI_FLG_RD);
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL,
				(ret & 0xcf) | (1 << (5 - state->nr)), 0);

	mutex_unlock(&inter->fpga_mutex);

	for (;;) {
		msleep(50);

		mutex_lock(&inter->fpga_mutex);

		ret = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL,
						0, NETUP_CI_FLG_RD);
		mutex_unlock(&inter->fpga_mutex);

		if ((ret & (1 << (5 - state->nr))) == 0)
			break;
		if (time_after(jiffies, t_out))
			break;
	}


	ci_dbg_print("%s: %d msecs\n", __func__,
		jiffies_to_msecs(jiffies + msecs_to_jiffies(9999) - t_out));

	return 0;
}

static int altera_ci_slot_shutdown(struct dvb_ca_en50221 *en50221, int slot)
{
	/* not implemented */
	return 0;
}

static int altera_ci_slot_ts_ctl(struct dvb_ca_en50221 *en50221, int slot)
{
	struct altera_ci_state *state = en50221->data;
	struct fpga_internal *inter = state->internal;
	int ret;

	ci_dbg_print("%s\n", __func__);

	if (0 != slot)
		return -EINVAL;

	mutex_lock(&inter->fpga_mutex);

	ret = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, 0, NETUP_CI_FLG_RD);
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL,
				(ret & 0x0f) | (1 << (3 - state->nr)), 0);

	mutex_unlock(&inter->fpga_mutex);

	return 0;
}

/* work handler */
static void netup_read_ci_status(struct work_struct *work)
{
	struct fpga_internal *inter =
			container_of(work, struct fpga_internal, work);
	int ret;

	ci_dbg_print("%s\n", __func__);

	mutex_lock(&inte