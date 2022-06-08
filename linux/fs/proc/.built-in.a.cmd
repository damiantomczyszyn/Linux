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
		jiffies_to_msecs