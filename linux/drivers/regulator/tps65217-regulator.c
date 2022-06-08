tate);
		}
	}

}
EXPORT_SYMBOL(altera_ci_release);

static void altera_pid_control(struct netup_hw_pid_filter *pid_filt,
		u16 pid, int onoff)
{
	struct fpga_internal *inter = pid_filt->internal;
	u8 store = 0;

	/* pid 0-0x1f always enabled, don't touch them */
	if ((pid == 0x2000) || (pid < 0x20))
		return;

	mutex_lock(&inter->fpga_mutex);

	netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR0, (pid >> 3) & 0xff, 0);
	netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR1,
			((pid >> 11) & 0x03) | (pid_filt->nr << 2), 0);

	store = netup_fpga_op_rw(inter, NETUP_CI_PID_DATA, 0, NETUP_CI_FLG_RD);

	if (onoff)/* 0 - on, 1 - off */
		store |= (1 << (pid & 7));
	else
		store &= ~(1 << (pid & 7));

	netup_fpga_op_rw(inter, NETUP_CI_PID_DATA, store, 0);

	mutex_unlock(&inter->fpga_mutex);

	pid_dbg_print("%s: (%d) set pid: %5d 0x%04x '%s'\n", __func__,
		pid_filt->nr, pid, pid, onoff ? "off" : "on");
}

static void altera_toggle_fullts_streaming(struct netup_hw_pid_filter *pid_filt,
					int filt_nr, int onoff)
{
	struct fpga_internal *inter = pid_filt->internal;
	u8 store = 0;
	int i;

	pid_dbg_print("%s: pid_filt->nr[%d]  now %s\n", __func__, pid_filt->nr,
			onoff ? "off" : "on");

	if (onoff)/* 0 - on, 1 - off */
		store = 0xff;/* ignore pid */
	else
		store = 0;/* enable pid */

	mutex_lock(&inter->fpga_mutex);

	for (i = 0; i < 1024; i++) {
		netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR0, i & 0xff, 0);

		netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR1,
				((i >> 8) & 0x03) | (pid_filt->nr << 2), 0);
		/* pid 0-0x1f always enabled */
		netup_fpga_op_rw(inter, NETUP_CI_PID_DATA,
				(i > 3 ? store : 0), 0);
	}

	mutex_unlock(&inter->fpga_mutex);
}

static int altera_pid_feed_control(void *demux_dev, int filt_nr,
		struct dvb_demux_feed *feed, int onoff)
{
	struct fpga_inode *temp_int = find_dinode(demux_dev);
	struct fpga_internal *inter = temp_int->internal;
	struct netup_hw_pid_filter *pid_filt = inter->pid_filt[filt_nr - 1];

	altera_pid_control(pid_filt, feed->pid, onoff ? 0 : 1);
	/* call old feed proc's */
	if (onoff)
		pid_filt->start_feed(feed);
	else
		pid_filt->stop_feed(feed);

	if (feed->pid == 0x2000)
		altera_toggle_fullts_streaming(pid_filt, filt_nr,
						onoff ? 0 : 1);

	return 0;
}

static int altera_ci_start_feed(struct dvb_demux_feed *feed, int num)
{
	altera_pid_feed_control(feed->demux, num, feed, 1);

	return 0;
}

static int altera_ci_stop_feed(struct dvb_demux_feed *feed, int num)
{
	altera_pid_feed_control(feed->demux, num, feed, 0);

	return 0;
}

static int altera_ci_start_feed_1(struct dvb_demux_feed *feed)
{
	return altera_ci_start_feed(feed, 1);
}

static int altera_ci_stop_feed_1(struct dvb_demux_feed *feed)
{
	return altera_ci_stop_feed(feed, 1);
}

static int altera_ci_start_feed_2(struct dvb_demux_feed *feed)
{
	return altera_ci_start_feed(feed, 2);
}

static int altera_ci_stop_feed_2(struct dvb_demux_feed *feed)
{
	return altera_ci_stop_feed(feed, 2);
}

static int altera_hw_filt_init(struct altera_ci_config *config, int hw_filt_nr)
{
	struct netup_hw_pid_filter *pid_filt = NULL;
	struct fpga_inode *temp_int = find_inode(config->dev);
	struct fpga_internal *inter = NULL;
	int ret = 0;

	pid_filt = kzalloc(sizeof(struct netup_hw_pid_filter), GFP_KERNEL);

	ci_dbg_print("%s\n", __func__);

	if (!pid_filt) {
		ret = -ENOMEM;
		goto err;
	}

	if (temp_int != NULL) {
		inter = temp_int->internal;
		(inter->filts_used)++;
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
		inter->filts_used = 1;
		inter->dev = config->dev;
		inter->fpga_rw = config->fpga_rw;
		mutex_init(&inter->fpga_mutex);
		inter->strt_wrk = 1;
		ci_dbg_print("%s: Create New Internal Structure!\n", __func__);
	}

	ci_dbg_print("%s: setting hw pid filter = %p for ci = %d\n", __func__,
						pid_filt, hw_filt_nr - 1);
	inter->pid_filt[hw_filt_nr - 1] = pid_filt;
	pid_filt->demux = config->demux;
	pid_filt->internal = inter;
	pid_filt->nr = hw_filt_nr - 1;
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
	store |= (4 << (2 - ci_nr));
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, store, 0);

	mutex_unlock(&inter->fpga_mutex);

	return 0;
}
EXPORT_SYMBOL(altera_ci_tuner_reset);
       // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * altera-ci.c
 *
 *  CI driver in conjunction with NetUp Dual DVB-T/C RF CI card
 *
 * Copyright (C) 2010,2011 NetUP Inc.
 * Copyright (C) 2010,2011 Igor M. Liplianin <liplianin@netup.ru>
 */

/*
 * currently cx23885 GPIO's used.
 * GPIO-0 ~INT in
 * GPIO-1 TMS out
 * GPIO-2 ~reset chips out
 * GPIO-3 to GPIO-10 data/addr for CA in/out
 * GPIO-11 ~CS out
 * GPIO-12 AD_RG out
 * GPIO-13 ~WR out
 * GPIO-14 ~RD out
 * GPIO-15 ~RDY in
 * GPIO-16 TCK out
 * GPIO-17 TDO in
 * GPIO-18 TDI out
 */
/*
 *  Bit definitions for MC417_RWD and MC417_OEN registers
 * bits 31-16
 * +-----------+
 * | Reserved  |
 * +-----------+
 *   bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 * |  TDI  |  TDO  |  TCK  |  RDY# |  #RD  |  #WR  | AD_RG |  #CS  |
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 *  bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 * |  DATA7|  DATA6|  DATA5|  DATA4