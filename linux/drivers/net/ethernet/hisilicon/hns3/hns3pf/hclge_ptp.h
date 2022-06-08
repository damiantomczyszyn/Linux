filt->nr = hw_filt_nr - 1;
	/* store old feed controls */
	pid_filt->start_feed = config->demux->start_feed;
	pid_filt->stop_feed = config->demux->stop_feed;
	/* replace with new feed controls */
	if (hw_filt_nr == 1) {
		pid_filt->demux->start_feed = altera_ci_start_feed_1;
		pid_filt->demux->stop_feed = altera_ci_stop_feed_1;
	} else if (hw_filt_nr == 2) {
		pid_filt->demux->start_feed = altera_ci_start_feed_2;
		pid_filt->demux->stop_feed = altera_ci_stop_feed_2;
	}

	altera_toggle_fullts_streaming(pid_filt, 0, 1);

	return 0;
err:
	ci_dbg_print("%s: Can't init hardware filter: Error %d\n",
		     __func__, ret);

	kfree(pid_filt);
	kfree(inter);

	return ret;
}

int altera_ci_init(struct altera_ci_config *config, int ci_nr)
{
	struct altera_ci_state *state;
	struct fpga_inode *temp_int = find_inode(config->dev);
	struct fpga_internal *inter = NULL;
	int ret = 0;
	u8 store = 0;

	state = kzalloc(sizeof(struct altera_ci_state), GFP_KERNEL);

	ci_dbg_print("%s\n", __func__);

	if (!state) {
		ret = -ENOMEM;
		goto err;
	}

	if (temp_int != NULL) {
		inter = temp_int->internal;
		(inter->cis_used)++;
		inter->fpga_rw = config->fpga_rw;
		ci_dbg_print("%s: Find Internal Structure!\n", __func__);
	} else {
		inter = kzalloc(sizeof(struct fpga_internal), GFP_KERNEL);
		if (!inter) {
			ret = -ENOMEM;
			goto err;
		}

		temp_int = append_internal(inter);
		if (!temp_int) {
			ret = -ENOMEM;
			goto err;
		}
		inter->cis_used = 1;
		inter->dev = config->dev;
		inter->fpga_rw = config->fpga_rw;
		mutex_init(&inter->fpga_mutex);
		inter->strt_wrk = 1;
		ci_dbg_print("%s: Create New Internal Structure!\n", __func__);
	}

	ci_dbg_print("%s: setting state = %p for ci = %d\n", __func__,
						state, ci_nr - 1);
	state->internal = inter;
	state->nr = ci_nr - 1;

	state->ca.owner = THIS_MODULE;
	state->ca.read_attribute_mem = altera_ci_read_attribute_mem;
	state->ca.write_attribute_mem = altera_ci_write_attribute_mem;
	state->ca.read_cam_control = altera_ci_read_cam_ctl;
	state->ca.write_cam_control = altera_ci_write_cam_ctl;
	state->ca.slot_reset = altera_ci_slot_reset;
	state->ca.slot_shutdown = altera_ci_slot_shutdown;
	state->ca.slot_ts_enable = altera_ci_slot_ts_ctl;
	state->ca.poll_slot_status = altera_poll_ci_slot_status;
	state->ca.data = state;

	ret = dvb_ca_en50221_init(config->adapter,
				   &state->ca,
				   /* flags */ 0,
				   /* n_slots */ 1);
	if (0 != ret)
		goto err;

	inter->state[ci_nr - 1] = state;

	altera_hw_filt_init(config, ci_nr);

	if (inter->strt_wrk) {
		INIT_WORK(&inter->work, netup_read_ci_status);
		inter->strt_wrk = 0;
	}

	ci_dbg_print("%s: CI initialized!\n", __func__);

	mutex_lock(&inter->fpga_mutex);

	/* Enable div */
	netup_fpga_op_rw(inter, NETUP_CI_TSA_DIV, 0x0, 0);
	netup_fpga_op_rw(inter, NETUP_CI_TSB_DIV, 0x0, 0);

	/* enable TS out */
	store = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, 0, NETUP_CI_FLG_RD);
	store |= (3 << 4);
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, store, 0);

	ret = netup_fpga_op_rw(inter, NETUP_CI_REVISION, 0, NETUP_CI_FLG_RD);
	/* enable irq */
	netup_fpga_op_rw(inter, NETUP_CI_INT_CTRL, 0x44, 0);

	mutex_unlock(&inter->fpga_mutex);

	ci_dbg_print("%s: NetUP CI Revision = 0x%x\n", __func__, ret);

	schedule_work(&inter->work);

	return 0;
err:
	ci_dbg_print("%s: Cannot initialize CI: Error %d.\n", __func__, ret);

	kfree(state);
	kfree(inter);

	return ret;
}
EXPORT_SYMBOL(altera_ci_init);

int altera_ci_tuner_reset(void *dev, int ci_nr)
{
	struct fpga_inode *temp_int = find_inode(dev);
	struct fpga_internal *inter = NULL;
	u8 store;

	ci_dbg_print("%s\n", __func__);

	if (temp_int == NULL)
		return -1;

	if (temp_int->internal == NULL)
		return -1;

	inter = temp_int->internal;

	mutex_lock(&inter->fpga_mutex);

	store = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, 0, NETUP_CI_FLG_RD);
	store &= ~(4 << (2 - ci_nr));
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, store, 0);
	msleep(100);
