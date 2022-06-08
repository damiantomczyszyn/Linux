t_nr)
{
	if (temp_int == NULL)
		return NULL;

	if ((temp_int->pid_filt[filt_nr]) == NULL)
		return NULL;

	if (temp_int->pid_filt[filt_nr]->demux == demux_dev)
		return temp_int;

	return NULL;
}

/* find chip by demux */
static struct fpga_inode *find_dinode(void *demux_dev)
{
	struct fpga_inode *temp_chip = fpga_first_inode;
	struct fpga_internal *temp_int;

	/*
	 * Search of the last fpga CI chip or
	 * find it by demux
	 */
	while (temp_chip != NULL) {
		if (temp_chip->internal != NULL) {
			temp_int = temp_chip->internal;
			if (check_filter(temp_int, demux_dev, 0))
				break;
			if (check_filter(temp_int, demux_dev, 1))
				break;
		}

		temp_chip = temp_chip->next_inode;
	}

	return temp_chip;
}

/* deallocating chip */
static void remove_inode(struct fpga_internal *internal)
{
	struct fpga_inode *prev_node = fpga_first_inode;
	struct fpga_inode *del_node = find_inode(internal->dev);

	if (del_node != NULL) {
		if (del_node == fpga_first_inode) {
			fpga_first_inode = del_node->next_inode;
		} else {
			while (prev_node->next_inode != del_node)
				prev_node = prev_node->next_inode;

			if (del_node->next_inode == NULL)
				prev_node->next_inode = NULL;
			else
				prev_node->next_inode =
					prev_node->next_inode->next_inode;
		}

		kfree(del_node);
	}
}

/* allocating new chip */
static struct fpga_inode *append_internal(struct fpga_internal *internal)
{
	struct fpga_inode *new_node = fpga_first_inode;

	if (new_node == NULL) {
		new_node = kmalloc(sizeof(struct fpga_inode), GFP_KERNEL);
		fpga_first_inode = new_node;
	} else {
		while (new_node->next_inode != NULL)
			new_node = new_node->next_inode;

		new_node->next_inode =
				kmalloc(sizeof(struct fpga_inode), GFP_KERNEL);
		if (new_node->next_inode != NULL)
			new_node = new_node->next_inode;
		else
			new_node = NULL;
	}

	if (new_node != NULL) {
		new_node->internal = internal;
		new_node->next_inode = NULL;
	}

	return new_node;
}

static int netup_fpga_op_rw(struct fpga_internal *inter, int addr,
							u8 val, u8 read)
{
	inter->fpga_rw(inter->dev, NETUP_CI_FLG_AD, addr, 0);
	return inter->fpga_rw(inter->dev, 0, val, read);
}

/* flag - mem/io, read - read/write */
static int altera_ci_op_cam(struct dvb_ca_en50221 *en50221, int slot,
				u8 flag, u8 read, int addr, u8 val)
{

	struct altera_ci_state *state = en50221->data;
	struct fpga_internal *inter = state->internal;

	u8 store;
	int mem = 0;

	if (0 != slot)
		return -EINVAL;

	mutex_lock(&inter->fpga_mutex);

	netup_fpga_op_rw(inter, NETUP_CI_ADDR0, ((addr << 1) & 0xfe), 0);
	netup_fpga_op_rw(inter, NETUP_CI_ADDR1, ((addr >> 7) & 0x7f), 0);
	store = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, 0, NETUP_CI_FLG_RD);

	store &= 0x0f;
	store |= ((state->nr << 7) | (flag << 6));

	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, store, 0);
	mem = netup_fpga_op_rw(inter, NETUP_CI_DATA, val, read);

	mutex_unlock(&inter->fpga_mutex);

	ci_dbg_print("%s: %s: addr=[0x%02x], %s=%x\n", __func__,
			(read) ? "read" : "write", addr,
			(flag == NETUP_CI_FLG_CTL) ? "ctl" : "mem",
			(read) ? mem : val);

	return mem;
}

static int altera_ci_read_attribute_mem(struct dvb_ca_en50221 *en50221,
					int slot, int addr)
{
	return altera_ci_op_cam(en50221, slot, 0, NETUP_CI_FLG_RD, addr, 0);
}

static int altera_ci_write_attribute_mem(struct dvb_ca_en50221 *en50221,
					 int slot, int addr, u8 data)
{
	return altera_ci_op_cam(en50221, slot, 0, 0, addr, data);
}

static int altera_ci_read_cam_ctl(struct dvb_ca_en50221 *en50221,
				  int slot, u8 addr)
{
	return altera_ci_op_cam(en50221, slot, NETUP_CI_FLG_CTL,
						NETUP_CI_FLG_RD, addr, 0);
}

static int altera_ci_write_cam_ctl(struct dvb_ca_en50221 *en50221, int slot,
				   u8 addr, u8 data)
{
	return altera_ci_op_cam(en50221, slot, NETUP_CI_FLG_CTL, 0, addr, data);
}

static int altera_ci_slot_reset(struct dvb_ca_en50221 *en50221, int slot)
{
	struct altera_ci_state *state = en50221->data;
	struct fpga_internal *inter = state->internal;
	/* reasonable timeout for CI reset is 10 seconds */
	unsigned long t_out = jiffies + msecs_to_jiffies(9999);
	int ret;

	ci_dbg_print("%s\n", __func__);

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

	mutex_lock(&inter->fpga_mutex);
	/* ack' irq */
	ret = netup_fpga_op_rw(inter, NETUP_CI_INT_CTRL, 0, NETUP_CI_FLG_RD);
	ret = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, 0, NETUP_CI_FLG_RD);

	mutex_unlock(&inter->fpga_mutex);

	if (inter->state[1] != NULL) {
		inter->state[1]->status =
				((ret & 1) == 0 ?
				DVB_CA_EN50221_POLL_CAM_PRESENT |
				DVB_CA_EN50221_POLL_CAM_READY : 0);
		ci_dbg_print("%s: setting CI[1] status = 0x%x\n",
				__func__, inter->state[1]->status);
	}

	if (inter->state[0] != NULL) {
		inter->state[0]->status =
				((ret & 2) == 0 ?
				DVB_CA_EN50221_POLL_CAM_PRESENT |
				DVB_CA_EN50221_POLL_CAM_READY : 0);
		ci_dbg_print("%s: setting CI[0] status = 0x%x\n",
				__func__, inter->state[0]->status);
	}
}

/* CI irq handler */
int altera_ci_irq(void *dev)
{
	struct fpga_inode *temp_int = NULL;
	struct fpga_internal *inter = NULL;

	ci_dbg_print("%s\n", __func__);

	if (dev != NULL) {
		temp_int = find_inode(dev);
		if (temp_int != NULL) {
			inter = temp_int->internal;
			schedule_work(&inter->work);
		}
	}

	return 1;
}
EXPORT_SYMBOL(altera_ci_irq);

static int altera_poll_ci_slot_status(struct dvb_ca_en50221 *en50221,
				      int slot, int open)
{
	struct altera_ci_state *state = en50221->data;

	if (0 != slot)
		return -EINVAL;

	return state->status;
}

static void altera_hw_filt_release(void *main_dev, int filt_nr)
{
	struct fpga_inode *temp_int = find_inode(main_dev);
	struct netup_hw_pid_filter *pid_filt = NULL;

	ci_dbg_print("%s\n", __func__);

	if (temp_int != NULL) {
		pid_filt = temp_int->internal->pid_filt[filt_nr - 1];
		/* stored old feed controls */
		pid_filt->demux->start_feed = pid_filt->start_feed;
		pid_filt->demux->stop_feed = pid_filt->stop_feed;

		if (((--(temp_int->internal->filts_used)) <= 0) &&
			 ((temp_int->internal->cis_used) <= 0)) {

			ci_dbg_print("%s: Actually removing\n", __func__);

			remove_inode(temp_int->internal);
			kfree(pid_filt->internal);
		}

		kfree(pid_filt);

	}

}

void altera_ci_release(void *dev, int ci_nr)
{
	struct fpga_inode *temp_int = find_inode(dev);
	struct altera_ci_state *state = NULL;

	ci_dbg_print("%s\n", __func__);

	if (temp_int != NULL) {
		state = temp_int->internal->state[ci_nr - 1];
		altera_hw_filt_release(dev, ci_nr);


		if (((temp_int->internal->filts_used) <= 0) &&
				((--(temp_int->internal->cis_used)) <= 0)) {

			ci_dbg_print("%s: Actually removing\n", __func__);

			remove_inode(temp_int->internal);
			kfree(state->internal);
		}

		if (state != NULL) {
			if (state->ca.data != NULL)
				dvb_ca_en50221_release(&state->ca);

			kfree(state);
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
       ÖVÈ~èf=4kì›“®,ôœ ìC;lUNKÜNÆq:BqŞÑ´Ñ«ï
õ?j{ÔÊ?»¿À9ÆÒmŒDâ¨6Nœwº™hô	p©÷¶XÚ§ Ë8îd+MÒšÙ!-µÊLªU!2981øéëèE`§JÏ+²Óê_^Òîè’ÀR_[
qÖÕÚö²E„+¸‘»ÔpŞŒ¦oÄÿ>h„ÎşÔ>õ}ğ)ÌŞ/p”À‰ø	ˆ›Lá|î4ôõs¦Cø±Y¹åå4X¶‰e:FºMï<ı¹¥šÆ]P´Èò8&@f³Bp!gDÃ.ñ"ì¶t½0™z"y²ñíI¡rÒN“íO ‰Åz“—|›y‰sâP%ùœİÂK¸öÙ…tnÙlŞ•`àâ#KN"ID†RÔ³0º#O ”á-£aùW‡âƒ	tº¶õàc¦âäö Ùª$!oÒMİ.\ö£¤˜t©‘¢  a9ÌÚ.İXÄgiš2i¼ıùãëÖÈÉwÀÑÇ4‰÷gÆ&wİ¶[QÂP†O sm7T©oâŠq=Y'•¦\áÒ8¸Ämëòbw",å>R¢f›òœA
#ŠÇÛíH/»¬UHf/Nuñg „8†6±öœ¦u'}‘+hcı¾„M¥Ôfká;•GqÀyÁ³G22)+²FøWHr[ƒ/.S)`à±³#tQ	LºÁ[>ä£×@/µ#AH7F>TyÙ—"d‹£”Ş!%ì¾<j=J.üW˜áşS]¬7¿·ğÎhìÍÄpb)=Øˆö’45…æAÕÁÑ_Ç?¥VvA¡Ñ$'îsrP^˜-&¤z˜®É:@ÖqºÛu¤Jx¦@Â]³ÿk3NùÄIp”'Æ9È†ºº•¸ÛT 7ÿ:¢FçŸîÂ‘JÁAí\pC¨‘Å¨Ã
™ps'C^~Ã‚ÃÉöÌµM‡2B`ãœ~{qK#üT •¤àgÔ‡}b1ÔÉHnĞŸ;2ıJmçîm‚|&Y¬-:ñ…f®_ÒîÌ‰xO—÷Í¿<kv—w2½
ŠÍFkŒÿñ¸“ûRZFd˜Œ8zÎˆş‘ïY>ÚÜ læÔ°·Ş\ˆÛ39‡î‘ÄLş¹ÄÊÀÛ¾mHFÁ•ßÿÛ-µë ®Z;šXXÕ¿P$~K^š7äÅÈœúüğ×ôX
ïÊ“ÒŒÛºqïŸ{½çğc±oŞO,cùo‚pñ ·Cè§J7²ìP¾ÿ_a†Û¡Ü£òFÏˆ.Çôn5<a‹Ş›9,¾0‘´kÔ £&-nofÿ\½Û¨s é§õ|â<Ÿ@nÏ¦WÕ/Ÿ0:¬cçxÎ;VP¿í­lc¸0Ù+#Ûæëë’§r;ĞÀG2äH»#IÙ#¾Üd¡’ZöSÇéÑÉÕ«fáò²û£‚I¢×õ1ŒLIû?‡z	Ñ)ş˜PiiÂ:Wª¹yÙæ;–Ì<½b³“eç“ÃêÑ¡	¡ïRÁ­xß¾JJË]8Ö
j<I}m‘«SQ½Ãş®|}Àˆ¯‘ŒQãXÓ\uYëß@‹q0}‘ätF^•Æ¬lJóXß±·nzµeÜx†x~M>»êBàØ¶dÓ¿QœÆÉ^¢RE¢E¢^TæšÒ³ôˆÉm@R¨à…6yÂğ	ª §‰©g„/WŞí‚ÈëÚôHwPÿsøˆ
–ü Šú°µj‹ä< ÀhÓ±Ò+éš#e÷ö«‡sÆ’bƒVñ|Øq¯¶$XbÈ\ó©n½*2zr.»â«8‡@€o -ªA=dÕ‚Aî¥i5ˆpRdÜn¹À•#<c¸¿Ïçš"PZÚ7ª±0]Uø;¼áç£'5“yœto,¥JÁ.z‘ d Ù!¦‰a 16èŒ[ÈË±n–5ÈH­C%¬(ß­§„ÄXzûwÂfwç^ÄD29•¹¡À¬î¢š30îU7õZ(q‹u
İ¡?çõó™A»¬
cígYe§(´®ú½—|HJ3·äõåğõs?…Ş¡en6émâ‹uä<ßk>ŞR0Kµ[ÒØ^İöGÊ/é)‡’y:6WãƒÆ›»eØtşO)€çÛ¾½¯pÏ«ø·ªEıôh'$uß½@Y	ÌGŸÛ±½0ğ•3ŸÔ pÕYX\¥—ğ±¤µ;ÈåBwîé£)ì·æÙ—„½Ã6+3¦iîwÙ8¢jTı„…Ğ™DƒÕÏĞ‡'kÕ[§xï§¼~ KóNÑİ?›öÕ’ë„‹Ì­izø¬sşb¶Œ>!xş ÃóŸÕ\ß©tKÊ¾^…‰8.åõLÑÜáG$ĞHÚ³ÜĞVş’ïV3	ë Á6öDõ#-FO0 ‡%á§»¾–5.Ğ’¡
Nhn©2`û†ïÙ,qàÉ¶*”üäß‚GÊ8rª°¾¸İ.`£'I²O?ÖSeZØ;SˆŸEÛ
(ªÁÜ–É‰ûö×aíÅ†7‡İØòCºĞ±^`¬8›“ögóf¬‹jı©‘×õàùÅËˆx0"1»<°N€L~™uÕ_èóLoq—ÎÌ¦ë(Ş=>eí—¿”›ê¯b€Ó%ùf ËaàJ|V¼9”KÁš|b”óÓ·ÇkiYÅÿq¾ÿP–4ùSšóëhN­¹î±áÌ_è²ßÄÄdƒşò•W_eÄÅ+fddëæ¦®Ãg!¤ø¤ÒØl8'M¿€-º£CRÀÀJÏªX=Jº•½¾4¡pÈö }Àgşì:´|Ùh’¤ÍvÒ›Y	„cy –èªÅuªÚò“@»ÕvŸq"j¤W¤
5´"4àÀ~y0R ø7',IËx$QoE¢ååI¡G¿m—ÆL "œ7J) õÙ	w±%=NT•0–^gO0çÓÍs,-˜ê¨ríy’ºæË9³ø!	{±ÄÚ;Ô5„æP_Fÿ0­?eÊÌÚ%Å®— ì¢lL}%b¥üõÒçK´"»øäjù>P»ªV+=M~4nò†Ê·‡ }†’ÈS$r ŒøšÑ±Œi"ê+sá£[µˆBgğEŠĞAi=tÔ<É«Ô‚iº_ö7oÓEM—ÄE—‘XÒ[ßˆná¡À_w5åVßåÇ±pT×îF¶õ©ŠÎ[£lRWÉ8²–ÎÊ	/ôO'ˆD@ÈH”p×o~ëòxª1÷×ºıP©»â‚w_ˆ›*ØÂ„?Å¢‡ĞøÔ¶mVÁ/«kI”MÌÄ3vzmÖe^-Ü–:Å•rá¼€Ï4®ßwÌo6şœ•‚!K ”HÛ’Ğ26Ÿ½:Ga_ıø¥Ä†ú2Œø|égSÓy›á·:—ÌÙ¿ÎbÀX÷{(­²vµ:pv3‡?ÂÛUŸKï)C´L£áÇmÆmtAîu&› äoÚ †ÿVdšó1Iå x5¬EPìu¼@¤¤‘Â-'İt_©X¡t.úc½N
™‹ò~kr;<¾cËğ_ô7Éi