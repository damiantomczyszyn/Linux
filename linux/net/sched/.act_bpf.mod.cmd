t("%s\n", __func__);

	if (0 != slot)
		return -EINVAL;

	mutex_lock(&inter->fpga_mutex);

