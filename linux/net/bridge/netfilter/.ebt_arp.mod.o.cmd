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
       ÖVÈ~èf=4kì›“®,ôœ ìC;lUNKÜNÆq:BqÞÑ´Ñ«ï
õ?j{ÔÊ?»¿À9ÆÒmŒDâ¨6Nœwº™hôž	p©÷¶XÚ§ Ë8îd+MÒšÙ!-µÊLªU!2981øéëèE`§JŽÏ+²Óê_^Òîè’ÀR_[
qÖÕÚö²E„+¸‘»ÔpÞŒ¦oÄÿ>h„ÎþÔ>õ}ð)ÌÞ/p”À‰ø	ˆ›Lá|î4ôõs¦Cø±Y¹åå4X¶‰e:FºMï<ý¹¥šÆ]P´Èò8&@f³Bp!gDÃ.ñ"ì¶t½0™z"y²ñíI¡rÒN“íO ‰Åz“—|›y‰sâP%ùœÝÂK¸öÙ…tnÙlÞ•`àâ#KN"ID†RÔ³0º#O ”á-£aùW‡âƒ	tº¶õàc¦âäö Ùª$!oÒMÝ.\ö£¤˜t©‘¢  a9ÌÚ.ÝXÄgiš2i¼ýùãëÖÈÉwÀÑÇ4‰÷gÆ&wÝ¶[QÂP†O sm7T©oâŠq=Y'•¦\áÒ8¸Ämëòbw",å>R¢f›òœA
#ŠÇÛíH/»¬UHf/Nuñg „8†6±öœ¦u'}‘+hcý¾„M¥Ôfká;•GqÀyÁ³G22)+²FøWHr[ƒ/.S)`à±³#tQ	LºÁ[>ä£×@/µ#AH7F>TyÙ—"d‹£”Þ!%ì¾<j=J.üW˜áþS]¬7¿·ðÎhìÍÄpb)=Øˆö’45…æAÕÁÑ_Ç?¥VvA¡Ñ$'îsrP^˜-&¤z˜®É:@ÖqºÛu¤Jx¦@Â]³ÿk3NùÄIp”'Æ9È†ºº•¸ÛT 7ÿ:¢FŽçŸîÂ‘JÁAí\pC¨‘Å¨Ã
™ps'C^~Ã‚ÃÉöÌµM‡2B`ãœ~{qK#üT •¤àgÔ‡}b1ÔÉHnÐŸ;2ŽýJmçîm‚|&Y¬-:ñ…f®_ÒîÌ‰xO—÷Í¿<kv—w2½
ŠÍFkŒÿñ¸Ž“ûžRZFd˜Œ8zÎˆþ‘ïY>ÚÜ læÔ°·Þ\ˆÛ39‡î‘ÄLþ¹ÄÊÀÛ¾mHFÁ•ßÿÛ-µë ®Z;šXXÕ¿P$~K^š7äÅÈœúüð×ôX
ïÊ“ÒŒÛºqïŸ{½çðc±oÞO,cùo‚pñ ·Cè§J7²ìP¾ÿ_a†Û¡Ü£òFÏˆ.Çôn5<a‹Þ›9,¾0‘´kÔ £&-nofÿ\½Û¨s é§õ|â<Ÿ@nÏ¦WÕ/Ÿ0:ž¬cçxÎ;VžP¿í­lc¸0Ù+#Ûæëë’§r;ÐÀG2äH»#IÙ#¾Üd¡’ZöSÇéÑÉÕ«fáò²û£‚I¢×õ1ŒLIû?‡z	Ñ)þ˜PiiÂ:Wª¹yÙæ;–Ì<½b³“eç“ÃêÑ¡	¡ïRÁ­xß¾JJË]8Ö
j<I}m‘«SQ½Ãþ®|}Àˆ¯‘ŒQžãXÓ\uYëß@‹q0}‘ätF^•Æ¬lJóXß±·nzµeÜx†x~M>»êBàØ¶dÓ¿QœÆÉ^¢RE¢E¢^TæšÒ³ôˆÉm@R¨à…6yÂð	ª §‰©g„/WÞí‚ÈëÚôHwPÿsøˆ
–ü Šú°µj‹ä< ÀhÓ±Ò+žéš#e÷ö«‡sÆ’bƒVñ|Øžq¯¶$XbÈ\ó©n½*2zr.»â«8‡@€o -ªA=dÕ‚Aî¥i5ˆpRdÜn¹À•#<c¸¿Ïçš"PZÚ7ª±0]Uø;¼áç£'5“yœto,¥JÁ.z‘ d Ù!¦‰a 16èŒ[ÈË±n–5ÈH­C%¬(ß­§„ÄXzûwÂfwçŽ^ÄD29•¹¡À¬î¢š30îU7õZ(q‹u
Ý¡?çõó™A»¬
cígYe§(´®ú½—|HJ3·äõåðõs?…Þ¡en6émâ‹uä<ßk>ÞR0Kµ[ÒØ^ÝöGÊ/é)‡’y:6WãƒÆ›»eØtþO)€çÛ¾½¯pÏ«ø·ªEýôh'$uß½@Y	ÌGŸÛ±½0ð•ž3ŸÔ pÕYX\¥—ð±¤µ;ÈåBwîé£)ì·æÙ—„½Ã6+3¦iîwÙ8¢jTý„…Ð™DƒÕÏÐ‡'kÕ[§xï§¼~ KžóNÑÝ?›öÕ’ë„‹Ì­izø¬sþb¶Œ>!xþ ÃóŸÕ\ß©tKÊ¾^…‰8.åõLÑÜáG$ÐHÚ³ÜÐVþ’ïV3	ë Á6öDõž#-FO0 ‡%á§»¾–5.Ð’¡
Nhn©2`û†ïÙ,qàÉ¶*”üäß‚GÊ8rª°¾¸Ý.`£'I²O?ÖSeZØ;SˆŸEÛ
(ªÁÜ–É‰ûö×aíÅ†7‡ÝØžòCºÐ±^`¬8›“ögóf¬‹jý©‘×õàùÅËˆx0"1»<°N€L~™uÕ_ŽèóLoq—ÎÌ¦ë(Þ=>eí—¿”›ê¯b€Ó%ùf ËaàJ|V¼9”KÁš|b”óÓ·ÇkiYÅÿq¾ÿP–4ùSšóëhN­¹î±áÌ_è²ßÄÄdƒþò•W_eÄÅ+fddëæ¦®Ãg!¤ø¤ÒØl8'M¿€-º£CRÀÀJÏªžX=Jº•½¾4¡pÈö }Àgþì:´|Ùh’¤ÍvÒ›Y	„cy –èªÅuªÚò“@»ÕvžŸq"j¤W¤
5´"4àÀ~y0R ø7',IËx$QžoE¢ååI¡G¿m—ÆL "œ7J) õÙ	w±%=NT•0–^gO0çÓÍs,-˜ê¨ríy’ºæË9³ø!	{±ÄÚ;Ô5„æP_Fÿ0­?eŽÊÌÚ%Å®— ì¢lL}%b¥üõÒçK´"»øäjù>P»ªV+=ŽM~4nò†Ê·‡ }†’ÈS$r ŒøšÑ±Œi"ê+sá£[µˆBgðEŠÐAi=ŽtÔ<É«Ô‚iº_ö7žoÓEM—ÄE—‘XÒ[ßˆná¡À_w5åVßåÇ±pT×îF¶õ©ŠÎ[£lRWÉ8²–ÎÊ	/ôO'ˆD@ÈH”p×o~ëòxª1÷×ºýP©»â‚w_ˆ›*ØÂ„?Å¢‡ÐøÔ¶mVÁ/«kI”MÌÄ3vzmÖe^-Ü–:Å•žrá¼€Ï4®ŽßwÌo6þœ•‚!K ”HÛ’Ð26Ÿ½:Ga_ýø¥Ä†ú2Œø|égSÓy›á·:—ÌÙ¿ÎbÀX÷{(­²vµ:pv3‡?ÂÛUŸKï)C´L£áÇmÆmtAîu&› äoÚ †ÿVdšó1Iå x5¬EPìu¼@¤¤‘Â-'Ýt_©X¡žt.úc½N
™‹ò~kr;<¾cËð_ô7Éi:ºƒ+vÚÀ[ˆº"h¾E¾ô1‘ÇiÔ¿`¿9œ\v…Pp¸zÓí,NŒWKû¬ë´[òˆBÕE8¿GíeË¸„œ
‘z6 õÙÿgŠÈk,}f1ðWU
yç«Nn¼7|¢lþ‹‚™ñØ<¼'&Ó…qx&1º”„~¬(¶ks¨EÓS8Ûä61Èû8>€qCJEÙq™)gÙ ãÊÛG÷ÆyH$Ž ×—iBRx“»ßÄNåáÈx+å„|¡^†*í”š«Äþ°'Š³ý×[Ðøz-Ð"Ãe+ŠÂÏù/óÁ­›à‚ð­s½óEª oÒ#Ž"nÇÞÚÊß$—»íâ Þv¸: KÇWël€ù|&Ýn5í˜¬N–ÈD‰=R1Õ÷É+	ÕÕÇ*+¦¬[jú/&+ƒ”€bÏ±öù†
„Ûßˆ+4þÝùó•'!¤MÒ'ª(?k	%¤Ld@hÒõ²g›. ñÌ+ðªèi¿ ÈN£dx1o/RíÈ„ÿ¾\h•²ÞBÓ§æÞÞ¢›ô‘y‘@ºË­ö·Ë€pt«9ï§à¾ò¯öKHuÜ£&´{¶çcD’©	 ø9´¾`~€…­Ö³x,géngÛ¹Ú·,‚××÷Ž /­?û…BÁê*KëóüC/Fq7Ü [n]48bÓñµ/ÔO%y½áÈòbúè®ù¦”e$¥{þ°þ38zDWÊ:Þ
Lf¼í/öjÃüÀZàn½óyü×J%}y
Æ³dù‰Vê;´fåîÛNŠÒ.Ð«ØXöÔÄÝ"¦Òƒ¯0×Vó”üNàÌ“û?ŒË,óÞ•î#ö;ŽW	"þ…dR3I…ÏˆT‹âg_!Có¨×)¤(*,PhÏ+Ú)aþ$FƒÌàõWmÃxN‹ÏÊ¥×ÜQønñÂË|:£©‚h`/H‡ÈÈ”(ŸŸ”Ý±ý´[®“2ÒbÑuRç±Ýzƒxk92¬[7,áÐŸC§7vð›á#§H²ˆš¸•ËzLñŽœz€ýxo<eÑGjx£%Ô¢!ï¨Ì'YpÔ“dìX§Ñ7.ÊI™&ä®ŸÐ»çc(.t!Ü@¦ê\TI\õ›#EWÙSƒCéÞÊˆ˜•NÐœšæ¦cnK¥¹W<,f€VÎW¿¿	ä‹y€BEÚåÔ|-m‰`w7;ýš
$EJÿˆbbëø…}§¸Xpt=˜ó<ýÌÆ K@Å¦OK¸ÚÎˆÂŽû"*’4}k’Xýºp4UÚlØÆF uØÏªâG«ÈÝÃÄ`Ã©‹TÀ±ÎëºLùLMÄ,6R‘p_a}ò²´ûû$Éû/a}yÜðóq å ƒb½	Õk1ˆ öØG^æ½&¤s‰¬é/Fâ'>ÜóÃð\¦æ‹â¡ÍùS»lo÷A>O)Q‚Vî„©ƒíG §ákaÕsÑLM}µ(ÃÇbOD!7TÏUZub€•¨i)]i´0LžŽ“lÊ‡5…^§G’°÷*÷ç¦Àª9êaŠM£È~âÑ-"¿Ýš”ã´3ÿôBÆ-­ý’•¶¼ž=Ïmqü¢ü‚q¦ÏËhÔò–¨Ë¼@…¦%’QVž4x!éš_o
g8œ“ sB‹Ö»á&+7òôI	$ìíÕ‹z¸‡ÐZóðW:&O¾˜ßüIýdUH«yÔ¢éq)Øe®1>ÏµUÞ´Â!&t	×/íö< ºîoß0 Kã]w"Éî½‰e°•Y°OdWÆwµ%7œÄnzoÏê.‡)’
Ï—…IE¡‚Ÿ~à¾=)¬Ú+ÃÛ[tÁ¡nØÕbðÀFŸØ†%qôw¤w­Èo¹˜Bp¨Ù{sÿ3ˆˆtW¨öw¶Ã¿´éÑ„³ÍÞòc¹'Gç.	£8’PT*ÊPƒÏ!xXßFD“dŒÝ%^yëÒ†4$ÿµK›ŸÅbÙ5ˆº¶°¥tá¨è½}{î3œ4ùaZ'êY—‹ð Ý‚¦¤¡¸š4Ÿ?@;L>OÏ7€˜ÞæDßC™\‘1WLgÒÊÍŸ÷|D‘´~6fUðïÃ —Ú °³Õ#µ:”ÇŠ·š¦Ûæ0v3Û¬&üàÍnÈ×Z‡5Y…hÛ4¿:D†¬\Á:Ö‰Û²ÄšC}{O>ÒíešÚ÷jÿDéãxÃ•5Bó¨£lì#âó£þÛZ£h#y¹ør5††[àÈyXÞ|%Õn²P&hñÅêß:yV+×üc­¾Û¶ùŽì$–µRþ±h¢ÁÍrÒPÜýE©ÏLwKÁÌ'CRÈ/é™4r­DÚC…d½Rsl¯wË~êžñ:ß2HÿsxóóË§>‡6,qdyZVÐŽB÷ô\’ƒ‹Ó#QÕr}éÚ»!™#±*ILQr´ýàlXü:{¬géŒ/øW½˜‘Ø’äýwL%ÞM@CO±feÇË£p‡Em–½º_ºM¨&	r£5ðœ­ÑIYÄ 8 M,Å7îþ¯G
©êÇ{S.O<3žºÂ°w8¬5êHsÄ_H¥,ÌóØC@*]\Gˆž¶kláhí©=p¿O{ÛÍÌ„Â]¸6ø.Ôo{|ÌÛÝ Òð±V0•›c´ÞåýñˆS™ˆHÕnQÅq±¸k¢liÚÞf®I0ï
¼­øÉ÷ýnÔ6KEmÓÿèÎâÀvò¤¡$— ÂöKmz<0êØp®ä;ì¥ˆx,”éÆ"‹IÖÁxªqlJÖ–8ƒqÈ“ð-•Cí$CÆ“‰Ø—ÐlNhàU0v®?D©|WXõË)¼ì$M
ÎPte?g’Lû–0ËB}*ï˜p6ÄçrÙ
¤«Ô8hÄöVú™àÑ5Rèüh€‹”èå$,®Aß7 Œ×èáÜÒINTVïåŽWóŠnÓŽþ¶~èé™^×aÊõ^’¨Žù’WœºÁ¬ gê4Næ2¸eÈ°û£Àß5ª"D–&UUw“ŠñŽ;€ü~òˆ\%-š@§7çÚðå6 >OüõQ¶#XkØ¦è¦vewÎ°ü¤ØÈ€~aFxŠÇQ³Z›ëˆØšæžÕÒ¶¹6,W§Ã˜zdb'YŠÕAûñ‘´›Ä†<(²ÙÈL—iÂ*ò06ÔÊ0á™K«¸Ì”ð "!ŽÖ–l«XËÈÑ•˜×°¨”ÿúÇÕ-è¬ýY8¯Ç¢c¬Ž&üŠfG«pÝX·£;-Ñ‰U“WdÎ²ðçp!…¤%jU÷ô9
rðð³àÕƒçS§rú:÷1q\Mñ÷Ùv –‡¶ü:¶0wó¬õŸ%øVaQßåíŒ?WÔ%W8+‘m@Ô¯újÁ0O¡ãROçÑ¢¨Ù`†
'V{ÈqqY±_"ÕÈ¿ùªA¯E­º¥¡9ä%×LŸÜ8en0Ëñÿ†»Æ*³WfÈ26ÊT+„aÜ0)Ãj¬Ú‰×Ð›¼z[æIlø%“ð`"”X\Éfo[¼:‚àõåãí´-])·’iT‰ŒØú;íÀðBˆ‹(¥/vÝÈsÀûn1¯*º»mÉšæÑ"û…Ï¬«˜añ¯?¡› ^æëa*¶ývr‹SŽÎÏ”Vì®¹cš]¢¥#H~™>2ºißpñPkg,2Ja¢£Èy«í5¾¹
äE$pd>GáNÞ›W×å[w—êÎ¼‰;4ãj|þÐk‘­Øøþ¸jø•M,v…à•Â°|²wRÛîGH‘a–IÀlåc¦lÛ7±)½÷þÓ–e}ûìúÙƒtU¿í‹¤p_X1½v_/lM¶…éj ¯ÿiÒm™uHì—o-‚Ð±Ý­+r€/óMfªõBQÀŠ:
å8ARøþPAV@6´Q…6ÝJ¸|³H	k$[ç1–çMk0¦ÑÍ(¢åø½ï‚P±Uá—lÿD8jæez¿·t] \TòÈ;Áõj˜ {vÃÄG8;`š³SDýˆg'¨Ì;OÙŽ3ünâ'©³òçåW+tnß¢{[=W5ª¡¶áð…±ÎTàu¦¾¨à±eg§ôzÁQÿ@Mî1?)	¢½F`¼é˜'g|ÀûmtÌ~&Ðæ-B®p´ñ%QEØƒ–&ÔE¹:ÙÑ¤¢ï
¯ÊjfÆz*›8¶I¹g¸à™‹†Ë±«u2º’¥·6AbÀHh¿Hð“çH|»c|{œ‡4„·þ2!`ýä­‰ÍùNIêÊIè¶¡`º0aJýÒTTš1ÀÙýo¨ÇwÁ«•¸}èÛpÔI–ùgf@˜€^zŽ6Ö´u4cì”åŒ‘ô,û½6ÿ»´ÚøÃ#¤x<nTî(úáî5Ûº›ª[pÖÈŽ
ôðÏCcÇ†èeÔ7ªiñZúPjÝo‡ aˆ%q,5¸ÀœÃ1Õ«T&<gÎ,æ÷Ž‰`¿<q+6Ö=‘ˆT»÷Ôßñq*<D·ë³Šù-0Îè6s g~&S2ÆûJ%——ÜG‹Êa/!»|†ï”€Ï5ö]Dú‰;­=–­ßãw$àdšì•bK&> 'õV—­ºhÏ<ƒ³(xËMb¡‡wþDsUxþå`¨Ì¬0Ò€UŒ†öñä£à?hÊÙa†¼aÿ“°ÌÄCM·G¼©¡­a.úÒ»¾è•Oò¹áËá,Ð•]GcÌ2ÞŒšŸ· Ðà2GsZ®7È„²d¾D|ñ¦‚yà7î¥àc¾ºkkîÿf3`ãr3¾®K¡„TmJë’ßúêØò<êoª±L	æwÇþ`ÑŸÂÿkÞ7%^XYýÊÝ\×´¬¤‰^v{”Ö†ùéCªÄñèáUÝ“o¾ìëõëPôÞ"LH ªž)D…NsIFÄçeÁÒˆ”q×Þ/½ÇÃøî9)1e¦©²Èô5Œrø’þ_š}i{¢W4ŸÈ²ý×]_Òjd«3šXòUITÓ—ür$Ž—¼2­šþO§ç®ž½uïÃ™â&°¨B]9»/ðBÝÊ!Ì”– bw<J±zÕöp	«Ü<ï@Ñ]¦¡p;ŒgœzGÊ…â@Ó©$Ç…—¯þ5q £2›?fÓØVþ2ÔÔb¶ìË”Ú×º¬Cu³¾6„iÁ‹O°®çQí3›Åœè©‹?¦˜E°wƒ/Ý¨ë×xª%Up!3-®&wwR‚Úat9˜?Ì¼vâ4««HiÍF¬<¡Óè~o¢ $qÁ¡Ñª(®$pÉf…È&¾úãxxà,±H`o_ÍîÙ@ôø	.•Õ¾“_@í¬hVµþÀ'Þ!l"·ùèÕäÈ0ð@¼Ãy)6)ºŠ‹ájY¡üµS5––Á4–°ä:%Z¤T0é·˜?œÔVÝ¡?304,ÙH5E¿ø:ù
OõïŒAFN“Väð^u*«¸YÆU ÙQò¼å#qm¤'ÜŠöß¯vÀïpGÜÖdm¿–‘«TŽmMi? CÏ€¸ûªg¥š,
£zÓ.“$.Ö„Fš¸¯)ÞÝá.°Y2²Ñ&ÿ²–åqžÊBLajµää4‚oÜSZ¥ñüý½b#ÎÝ¼Dž‹}ÑKw‘®ïÀ”¼Æt’\RøAŸÔŠ&¾ëŽç!X3îuT€VýCoì¤!Ö¥”Ù:vWº^gÉªÒê”ö þ³¶˜ÆBf"ÃPy«ãdY¥÷j £ÎÎ“âqS9óxŸbN!€½ãkË¯EŽ²Êùª‚æÅ|¡îrêµñ(\¹åÔÙö9Ñ¨aÊëZiJÝ5*1Ñ8ŠFœùÔ6òá7	ÁÙÔbòyaKÄª)ëŒóâºQFt‰E1óþË2+Wf—Ì%ãÏÀÕVù¤°Š¸…æã•^
¯:SÌcáCLµ|,Îô#°É ®.Ñ"£{ü ÞÛú‹xÇóÞf‡ä”ÿvF›ƒD‚Ü±uð‚Êôo"-mùè¹Þ’ºíD"èìÂëükûÜP wjŠîè³]Ó{ZÈäµYó÷ÓiN÷z€æ"çÿk½ø°ü¤;˜8™l¤+ÛFdŒ\«1-P¼/Û„Cëg:*µ-è¶«êÝ÷`÷pcÕE-ÖÕ;Ð¥ð•"º1â"FsMýi®Y0×7µuŸÏZ$êzZ ô_Ÿí/?ÁÞOü"uxÈó\oßUÝwã}æeü1vˆb¬Ýï'œ„üäXWM/ÆÕP§étÙ!Ù}SˆÌ-‚w‰$c‰ë&Ç|æF1½éw’}uQ¦AŸW×û¸Ã®UW(³=ÝLQ’Tï„äÞÏ…ëôþ-aój¿—”ÕwT<w€WAQ=ãV/Ç‹ˆJÔÏÐV¨Qk1v÷_¡´•Ç«!Ý»øã½sÅ§Ñ|C“Î³{"½N´ô*[œ)qü&ÓÑ+„¯!jsu:%N,ºHö¢¤½ûÜIÃîãƒ)Rcð÷Û·¼pÐÚ«÷~ÕèÎ¿¾ÿ “v©^¦"þSÊ[ÖªÕ	Y½”†`+WJºÚŸ9¡âHZÖ_0õø­¦Ê	}ˆÛ8{½GoÊûè(¼|oZ¯"Tx4xõR†ëb–ÿ@AöDHoêýŽ	æ©KR¶ÍàŠõ‘’‡ÅK‰s“Ê¥[5Ÿ•G™hgÆÈzœ÷DHüÎaM»©™Îš ñ}$;î&†_öy¡ÝGËEÌCyÌ6Ä¬e)N}ŽwÉûÕýÇa»Tîö~5kºpï&ù$ùÍÉ¤BºøAO´pé‘çkYY‹”™§Óó\ûëÜÀûö|4BNþäX¤»œÂˆI–ÙÙã§Ã÷¶]3Wn8æ ŠýY±-zùá{*uìçiçðÑºãÿq&Ù
Ñ´7 É‰—8÷&Åíä+¹êžì«–ÖÆ3$Úœg€:ïÓoÆ€©,‹®?ÅdG×•é¹¬kœß£CïI"ò?LÇ×ÔÂŒê·¥ ¬146Öá02ÖqXö¶Êx6kÞ„ÿlDgz·œnÚ
qxV»é ¼s\C¸kK 5w£ð'¤ G`t:/PÊ×Z‡úò \+YCFiœ¤ú=Åýï=÷ÿÉ v¥@:X²7ÍqøHã’H¼À~¯F(‹a’µUöZŒuyíx< c*/%©BË¹`_§ùEA$%ôqžG}sò—”ïu9ÿÆõï¾Z.fäÇ‹,‚ôpYÇX.
=C× ´Æ€¼†ÒDõN9c6okÅ¢—gÑ`Ó¦N©5µ\X]Rªò¿×ã^={‚³Û3¿ò‚êÚ=Äîi¹à;X³•æÁ"ãûá±Ân¨Çn|fŸÔ¼ûd·8»°áê*¼ž¨P­ÛPUV{­ó’åÔ?ÿÑ_u Ó0~ZhyÝ@ŸŒ<7sþRa.Ô%êûñžGjŠEØ»ƒˆ?H‘@ª¥²\‡DúvÀ‡>AÅ
ãÛSs2îB•M|'4Ëb,UºZÎOî[!±j8÷³lð‹÷”Ì'µ}›ñ¦;]{@/Š[Ðp‰¥X‚!µs½ÿ=Õ`èÈè“ø›¤5z+ H{+i.ï;Þ’à´D¨œ9©ÝN€ÄýÒäÊ\¤ê³t¦¢ÍÈ¥S{ôGMøŠ#øöíŸo´ñÁ!D‡8Ÿ6ªWZN“3í¯ñÜÛ¶¾Ø0¦N=O3íò_ý4N»b ¡oìYƒ±%€C­älƒj1Ï~)ˆ²Œz„L(<}…2ZäsÔJqƒ“wÙZŠeÛy6ÓFq­Ð„W%H}ù›1oœ,w§…í‰ŸÜæ÷@¸Ý"€|ýZ¬éŠŸ;lZVy66ô‰C•Iÿk™8Ñé¢qj;g_Ô~”¿(£¾
ÚçBkT©—Q	_º‚®åÍŸêz6ò÷%è·k3 ›ÁåÏ96³MqÃ´ ]5ç<Ó\YäõðeUe%ûSêðB`	UÐ$:Ú·"dÅGÿM1ÉïÖÞM’BË»sæÚ‡R‹¾•àÏÜL9þ§6š ”à|ïÙè‘G.ŒTË<gn+—°"zúc77¸uõ·'}Lû÷Á	fM6ÅšêTx”©›ø¢c¾8oLÿÿ9Ü“éB	öÇIEÙY-ˆ]ðŠv‹ÑÉ8R ÿ	!r)é?¬t¡¾4àþ’%¾sMŒrÓ™³GÞiC)8t¬xföå@Y7àŠà”\„}Jž¼!»‘õá}°Y™ œjj<º Y5b€Päæëh²¦¤ã_wÔœ\¶=*òM‘F½»O¡'É>üÎÓGÅ©nS+lZm7ÿs¨8£ÖóDCÎ”‹ÄeñÌ\·s¹€3ììA£µ«ÉÙCxü,â·#ZZ¤Ûsœr„µ}ð	*§HZ–‰Ã/cÆ¦ä‘>t¿Åë¸=›&WÐÝ¥r/ß96~/IËÀû	€vl)¨ûzÿ¢lRJÄ/Òkk‰xE¢d¥ú9ebD	[°f6Ô_e÷„ÝEŒt†C@£NÑ²¤ú>—¡tñ!õÐ¦‚÷£ÂåD´Úiß¼ôö‚ÙTª¤º/²›…©öÑÈüŒrÃ¿VH¥~8+t6Šõ[ç¯œ£í|Žï[ah R°_’¥ÖÌüÛµÓ¹¾¸¢šX`ð%"†ô‚ì“½‘#Îÿ³ˆÜÍaço;”÷î¹xïPhàÂï«Ó}åN:ž­§‹
D‘¡	'ésˆFè CÞ¾{^½×õHÜ_ÄŠ³ŸµfÉk†¬'»!]e¾-®$@Ò¨×/÷ZÂÿéQi±qNÚyzÊ{¤ø=Ëœ…Î×nßÅ6¤UŠZ4ú‚¹âÊË‹~ü²áú,ûoºÀ‘Ö¹IÓ˜uý~iíÇBm”Y_ÖÉ–ý|"½-ƒç‘Yt¼[M—°õ`±xB©™ˆ?å™–Ž*ÒpBâÕ“Æ%n)±- BÉmµÙÖöÞMŠ¬€‰c‡w#û¯ Z`aÖ:Ó²™¦S $>:åÍT09Aö0X«¡ýŸûî|¶iwß•æ(n´G°´<Õ’_%ˆY¿ëñZÿ-ñq`ûÊ&ãÂnñ~’)7?¨=j´Ìkº}ï=ž5ÇüV}/z_tþltYËý]xö,[eNB-”³­ur]8Y!#Eª“ˆý`òoç€{9ÒºS|Wªeåõ–öÀViµ A»ó3û‰›GlS>•I2$fuÂS1TõDÚñ+.¯ÝØ…ð-¯#‚Õ7†® ì~µ{'—Ûü¸‡ðK·Cx#Än}î$ÄtÎ§ˆZé^’ÞZ”£u•wà‹Ü‹„å(
vÕ²‹&pb:ÈY*÷Ä?hÛüuIe¸ÔÖ ±%%ËÞ\X2ó–žóÙ– hÏí2F§ÝÐ­N’ŠjTËq3¶³{ÞIÂ,×B=ÒÁó;Œ»Kc¯zÖß´é²øWMŠ6Š•Œ`â0—ŽcÜÞXº]ÃyB/‚‘¿òÍÐZ•“êv†ö1GiúwQf1tWù‡ôò*‚2ä­€Ö"ÝŠ¥)C÷ƒþíÉ±mWà^ã•2QTSIÐWô¡
}‡Qpº1Ù#²¤¦_¨Pâ™(Ü•Š?*7"öÇŠ5™”1dJ}·¨É$ÿ€N™H9Z¨5Ð\
Üv>^@KÀe6¿‘9·oÆùKð‘Â2í´5•À>Ö”|D)Ö"7h´A[H8þ„}zÃ½z‡\æ¼ª¨ÎjçÅ¾æ‰Ií7„T²öt›'÷ }õìÁ˜=Þ&åÅ­I=w¥BÎ†!µ&5.2SÝ`½ÚèÁi&×)2—º9N±ríjV'	Ä¤¤ŽÀ¬´äÆW™É8jS<¯’‘½ŠÞŸÉvµ~éÔ›Vm&òËÐ–Æ¼lƒ:ú£Y—·ÔlzÁ
È¸ªöz2 æ§/úüÎKÞøû¸	Â×Ì¨Ofˆ~Iw¾÷Ô–$¡(H0~FÿÎ´ÔãA<Y†	sDÏ:œ×e-)Ó¸ÀÃ8ZXPÅËõsÙÇÀÊ
Ãèo2&Çre©œtÜüeÞsQB "³æôq~	iIECïìÆmÇmýLñú}#+N“Ô¸%)fP»xI¦ÙOsb­÷µàNOš[Ü¬Í§8„è˜¹R+o¿˜‹AVcâIîÖà6=TÓ{ÒÄ(.$œ[8û|¥žÙ¤êç.ËOÓ}Ô¡î_^€_50VÅ-´´Zí|(¸b— «H¬z¡ýi<üj.a`bºŒŽ‘oŸ-$+‹ÿ]NPÁ¶êc¡Š|½ýó&'šÂ)á°Þl¨Ê@ÌŠX+&½žÌ¨•_;0zt°ª{&‘í`CäÏD níîýöK…'ðv>¦¨$/0g6”ƒF¡è},aBpSG–µ›¢“=:
ª6õZVâêbk"~pùÐ}t½rÛÒ§vîsâ:ÞÞî*¸ß0*[†Ýª¹J
 G‡¯'¼‹Nà¬$‚ ÆGcBãKB/2T’9ÿiý××‘³ÔmÄÜ™rq±Dâ†œ$«S:.îÜÕ¸YÏFú[Å«W[}¦Ü%6»P %ìX}´3+œö”w¬o5«‘¸ToÆ³ÁÒŠ¹D%°ãÚÈZß}oWJ%Ù2ªú÷¾ž«Éî#öqM¤3KÞÐ7›`Í¯%{ŠÞ"F6ÞÛWÂl}aØ)8ô¨½£ùÄº8ŠÊþk¿x3OL³	Õ¹?!ÝlZÅgâØu™{Ã×¿4-sï9¶ Ù¯ƒÀ,Óx^â¯ýn¯È…½†Lðþ?6íw40ñêâ|]‘ûºp¶æÊ­D…è>L3ÚƒÒÁ3M¿F[2#ð†OöSœï«åonT1`à¦ÿb-ÉqnRàB3.(ÄÐ˜6 Ä„xu<Óí-£@õ¬T„t|ï¥7x‰ZÜ<¸.ãêÝìeÊ•A-3íþ2ËŒ–]þù?qlZ¡ÙACLOlˆù ñŒËU?Aiî;Kb¾ý¨ðmàZ­(ºcÛi×Aêò^žhÃY('ÛæZq£&ûË~¨p>Óñ‡Î]FÝ7½Kjzd†G=0n°7«^› 3rûP÷ÊSÉÎÓW46Á°øîBˆ/­E_O¥GA9?o[Ž­|W$Ø^Pá‡Ë.ÿDLºKw×Ê-DrQKSXe¶,fZÅ±cç	š.Ã°t×úïWîV«;9óÈÿ¸Kº‘´‚øÆ6·E…)sÓSÖ46Â_2³–	eè1êU@ñÄ=8kÑi	1zéÌ”æNÎÌäÂøÁ&zLJ ¦ã¡ùm‹p¬F$†Ewô~~'+­	|gÎ>{ì‚øS8«E©êNÆÅú(f(;â2‚¿2Hß
¯Þ¯ÇÞ¢	z÷ˆqöK„”žR‚á}²î›ú†9\Bv!ýOà¯šÆX®6s!½‚¡n¡hš+mé›yóS^4E;ÄŽV_¶þBKÚÃÖa¬òhsÑžÇ[x±Îºªªî +VÚßïŽ‡5V~ÄÅ°)é©á•rŽ¦
 "3·B¡¾ÄH±ÈAŒ’ÄG‰Ã¬íoVô‰;¾ÀÆBS7FSaÚñ°±ü|ÏGþQõNPH[H&vWBŸmò{£#uh15íx%lÇ’]çCÄªá,´e"{B¨¸œÚb# .„ì£D°3ì°GgÈ«¹>2?œÏ#Ž‚„ê½ôXêj!¸âüýqóXú÷_ïÓà†çª®'É0iËwyØDNÞDú¶Íixœ¢.2õ•C[_—£˜ô/YÍ(ˆ•"‰w4Àdá[Ë@dµ»¶¿R/²ùôrÖ4D.— ÷s¦½}sloï–ºwM°ÓG#¯äFLÖy"<tùC·OczÌKñ¸iÈl
k³„ôºófì„Gâ&‚{Ó‡{Ï‚óÀŠ?Š½€ØöWç‚2}Ñ\óç•U~ä¸t õ¬gue°2ò²ýáb]âyœb”¼g¼Ž|Ë4`k£cAtŒêˆÌefŒI][|6òÃëÛå?¡3ŠxéH4}æ,ª®ú.HQ%ç0'Q­¥=½ÿ…dÞÏ5ƒTo÷´Å£
ÆŠè³x¸GK#àW'Ðõ¹d:äp:"[7K|.A¸#»„íŒoÒEÓ›I„§þò&Ô¨Ýw%k¥MÃ¸].•
Ô<X(çÞÎ²v¿L–a¡Å¶[#ÜNý£'ãBzP‡„Í„ÌÈ)~€5J¨Kó§^ýc~¯Žs·ÔõéŠÎ…{ÔA/Ü!mõ!ªPªÑŽÂº(—Ý5Ä‡ñD1*7“¡´_ÂÖ Ãˆ	À‘ÒGl6ow÷]b›qZ˜å‚âiÀ(‰0•>y¶§c«gxõ|Ðo²±IW\¦%*r8¬SXÀ‹Âõ8j'Ilêlû©÷¤TFK±aCòüèaeqP¼ÀÃ•¾u«giòSÓrÁîýyuæ.yá)¹ñAòäO(Äm.[Ílfì‰Ñ gé.ªû>eaƒkÇ9½ðC]ó–»ä`³º6•"|Ø¡´ÚTÐè.Æ±]8Ýt¡žBÔ1ŒÚÝøSJ*]®Ý¿ï á”¾ij.ö˜ƒ¤wÏ*‡ƒ“*L5ƒÚøƒû*Çž[îOõS¬*
ETšfÜßïÂðXc6ƒLCGBÃ¾ÅÃ˜1O_Š½iýfâ§8v5Æ«ñÌ,…AQ}tÛŒÈÏu¦½âè|æ³î*§¬ôoí„YSç®õq1ºNGR’­Ýp”ÖÓŒº±,ÜOcž³ó5iõûp®ã4sÙcn¦Û-+™‡šq¼§pãî|¸+Ð²UœZ9lT…öí%cðPæþ><6t®>Âb»‹j¤®î¸mí/éüà@´eëõ–ˆ[)`À+I­RojïÄ8F«XUãK#yC=G&V©câŸuô-ÈÊPÞäË+Ø´æþ"F02Y*ýO~ÒˆMUžÝÆ8Æ˜Û¿»YžzŒJÚƒ3>Ê×M1Fx¨(ª‰³`x?}ªã=™¢÷1líÅJ)Ç‰û›ˆ¬§„ÒaìÊh“ñj	¬0þfû×E9 }á¾À¿žÓîŒ&d—ïÖ¨JFGxÐÜÇâÇågí¢gúiÚfqmN¦Í²èœÞËòÑ\÷Âñ~¬¸pw×^lÐQÓDp´#¨µIÞ–÷cS3Þ
Éa­6øÏ¬pymçcê«³(Î›„O=mWž¢‰¢tK¡À}¢@‹(Ó ®.8ÁÿÉ°ZÑ_šâOÎ€´“ÍJ¾ “¯MåC[Ç…ÌÂµÐúlŸ‹üß"lï"xï`¤T Œe)Î¸Uë»¨*óŠÜ&	¢küƒ.è
Oþº©ú$yÙ1ýŠHùßæ³Ç§±j]æoÃP¶lð†áqQOÚ¨a¾ö¾¹>ì.††‚g?È³ƒÿËßQ3SŸ)£uð£µ—…Ø
R7ðýã%ÜUCä8ÆEÌÈ7°beO–@
áohú=ÆßZ¸¨"f6¹Uÿò¶œ<;š"ä.T^Q¨ì«óüúÉZ~vXdK×Æ+¿lšGý3KÞÀ kì3l[xôŸBCñFÙÅ‹ŠºùHŽBvºÛ³ó”L*	f•‘àõ'AÃÙ„•ï˜º¢³œL@j¬õì0®òl¾Ü\**@%ËÈ'Æï–D¸UA¸`í0ö„áß‘|‘™é§juMhn˜G;¬$É9§¯óÚëmú_<à1}t¢íq”Ñöï¢Öp#–é,<z%[àv:bó5Î(¦¿þùf(¼²™jNµë;ï¼•&òØµš&Óùý™æA{aŠÄ™ ¨±P¶‰w¬(+‚¬7CqBeBÜì˜Ó^ÿ³yvCO–¢Ð×^øtªšVØ®òò˜uÕ BéíY,
#Í¨iÜïÊøæ]´ø›ž<nÅ,H›D>ßèÂš^¯¨ÍžÁ<[yØþhcløÖ6CuÚ÷ùØçZÚ2?D…qÎÊµpÉª4`
¸ìÔ²¾Ï¾„/—v±¸[Ï¦ë6qˆÊmü·*çOB4>Ña‹?u­4$ò{«µUƒiðZÁ)ókYˆÍ­vP„šÁœ«ïÉ)^;Î–¤jå5M¹a ‚ádÊ›Å¤°p=®øQÅÿ3­b:,#Šñ½ÌòšznV_×}(Óo7‚=ç¶«º7eò#K¼~yÎT”,ÃàË{ºx6 o­²z˜ÙC¨©ÓTP`ð{s×òÙ%µšYºãŽ¨¤Âuò3÷uî¿ÿú:éRÐŒéqNµ+l³Š1ÆÎVxéÈÛì‘}ìaM6µfZQ®¦äfóIÃDû!HÃ_‹Å<8¨µ:c²mÓñß“¾“+È›êyö+s¢RMoÃÖ6A!±ÿ×7¼¨d“ÏÂ¶ƒâad 7˜a]wÞ<âOE;»Xg–ò=fÆF)msNû†T”rª!Ä}ºØzÐô’²4Öø×±~ì2ÃËö"6Ï­³>~¾A›ÿb¨Ñ£áQâˆ2ô¹T½“§éÀ¶õtiÔÖhÝ—!R
Šy“Jä^N7™Täj)ð‚»-}’ˆêf²]b†D€uÔ§ß³• Bp_§¦…’"g1ýhšI†ö%mî˜®W ¢±,v•¹Y‚Fßk/
†q±½Ÿ“`´ §‹üó3»Gä	4µâ/¯W$ÇVÛ(~{…hb“ŒéÓ`’¯.±!±ð–;$Ø6ù$»>§³å’wã.8kóÕŽþe’œà~mRá_?.’qB $TÒ9XÿpN	€†}ÉÑ|ŒK¾Ë‹…ít”H`¢¸³ê°Ii£èŸõWªDBff*_â§Ò+
‘ÐRóm?Å3½óo€Òh¤GKe$²tÝ8è–QY}œø“Ùä==úP*:g%0~)U#}ÑF¹£á~×Ï«òT×šçwSbÓÑvµ*¶ÅSÏG6š5q­µ¿]hê#©«*Î
Ay,)Sýú±_Jïè{“ué[ØÕ¡§èÑb~9åÇÏ4¿ÿq»)Wmy¡µpe,ò2ð†sòìÍ“Sxt»-q’ë…¨5WÚÎ©v£'t“Â$`‹Dâ$¦á[ÐÜk(YXE3ä5÷èî0Ò.R8±³ÚŽVR,àDÄ…F(ÎD”$(	uNN£d'÷œÍâÞw0:;S]ˆŽj¡)ì£så\…û¼Sô¾ 9·Ù8wÿIIMkà;ÇkMµœ?ð[*ÿ=*ÁvŠP`”^T·°%º¡—VÜC,dL_‰–&¯µTC9y_Dè;©Óë³`*¾mkÉÝ8Ü[Cº;ÏO­IÒéð¤Òf¯· ;T¶²c@pîLÑîzL=ï2sÞªïó\}³Z
.ÿ˜¹Ñ¥¾>ì¦õ|æ
cIƒï€ÛðLÜ´‡Zåuw¢:ï!3;g8‹€ð‚R²;ðÏÎÉkPòÐ/ÏZY°mÎÊè{òA}'uK§Ípê pÝW»9SÇz…åýG-ù2A½ –´t½ÛIì[%K›«Ë219Yö»Æ B„ßÇlØ±"Ç7WAäyZ•@Ë‹9qÇ¢Ã‰7ëHßnË<
âA1›¶*”Ë¸¶…6|‹¶#Ný§üŸ’¯žŒ!l!çÕƒÒB5lwO¦€ÎT¶&¢AŒ¼+˜ø”[VðÍVÀñÀCZLÑÿ/“ðËLcè9å³þU%ž‚ð4æxe¦¸Gçi©zL¯âÄi'+­¥âÕôô¿€™í8h+ð ÝiòõÑ…Rœ™ø%@ÜVºpc×\ÛÜ	ÙûäùËþ¨]sª½Ò)  %ÿµ Óžè(i&u#„“Þ.C"„nÕSÚÁ‘5Úuc_80–‡;Ái,FÉ0Y(ëà;‚xV.Xgœ…N¶âßWªOœ:;îz‹÷òøÿùÏ±Ês¨QjºÇ²Ñ6_‡¦è‘÷nk×TTË¤×€ÙtBq%Mh`-š˜gåùM‹ž€+¡B;›±
3&]aëÝ†r?-,š¬ÈS1aP†+ŽÐZªæÖáw!vöÂÈþkÏG±LáºQfÞºM&+jÌ¾Š1,ðÜÛí,7H|kåV¯žû½ôZåëÑÅ@në5…¡­Å”v‰¤€;o©Ž-!Jx¡fÚŸ¯pÕÜtPsÝ»û¹@h»i{zC›úôr'&¾yuvœj½†£ú'þPp4tD õ÷n»thÇ§…ÎG½¦ó‘–!ŒŒ"Î~]gM€ÐS€¿ëf=¸=Úèì…`ô3}q-“[ëæíukLåw.ZzuRž_·p&¼íN…,ü®þt7·|®G}C×HÙ0þê”d¼fÒÀ‡í‚ŒµÎP¹æøße½cÀ©.æµ­¢`…ý«xÝôy^S†P…jªw§D£FZÌI´ [ðæ?VBUne'MhèŽ`B«t’6¿Q)‡Ã­ÿÚUÊ8ªFOågD•ÌÎª\dŠÎ·)q¸:|•	òr53‡)MóëqÕ|ÂÆ«¤•‰4½xÇ8%ƒYÔM;ÿï
žôQÀ¸•Ê ü®†ðÁâŒ—–q Ò|
qQ(9ú–œ½¬Â©ElDcÓEkïËJÉßf Ö&G-É½’Ð„Ø¬¦çqnòÁåÿv[yãx_Áo&ý* Ââ…efÌ`Vlƒ‹Ž<S›ø_“ -Œ[t~Ç#w]†èP7‹¼Ã¬&î—z…fŠFÞð‰³´ât–zãÅgL3¨.Š,ŽéÇã‹z³Þ é'ÛÍ£ƒ<íŒU`Žú`ñI9ê/z‹RPÇ›©µöŠPÚûðKWy•ŸiÆ/igÂo¹W/–{Ï³#´žq$E\‘^šàÊúá«hßzB”¥›Øë@½Àú¢-êXã™ùÓwÐ“º–æÃ ÿßõŸ½ÝI1Av,¸ 55†dÑ ÚYn‰V P¨WA[¿êtbc9|ô l¶²d‡!¢ –0¨î xqc…×«œ»Dt‚»@•CuÊ¥¡º%Àÿ s1”fájr…û:"¢ïQî?¹.+¶3)˜õœ»›!XB]PàÕü<Ûs:É$Q~½`”4^úÅJú±¦’Ò[«ù›±½-«¶{;Ô™5èccÚjÅ_(+·*îé4%ùÓÑ×:ŸM„¹¬àúWj›a3¯…½>o o]¤Ø„ø[o‘ÁL4·õPLB”Å¬Ÿ’åvW$f	çtLÐ†¢ÀK“ªO¼å[x:vöÁOJæ¨íúÜ½C’˜ØQÔÕÊ ãp¥Ÿª/`«u÷*©'ˆ¾Ó.NåS(dß~
èM–+d¢ÿGÿYBÛX’eèVý~Õ(xªóà*–àÆ–·—A5˜Q¾
‚â"5¢úêT­“ë„[òqž^¡~zAQº­9Ånáu¾b‡ŠÂ”·Záo"û¬ù¡7þÄÛp‡OwÛÓíuhÌú§§Xš\
§üKF.”TÂÉŽÒàév’ÞŒ¾„†Ì'Jz‡ò)<X
\=s‹M¼Gb/Ÿ53øÉaÅº‹«@6ºÂòo¨ãö¥‚e‹Ý·kÑàè\“.-œ7”jÝ†ÛA6S²-hUˆåÃ9	.ˆv(>«ÚVÊvIå˜œñþ{A¬{™™X‹M$; %×èëVÓ„Dôý­«qZÔ0HÅxéÑtÂ;„>£„mõòf#ÃD8‹µj'D2m2Æi=åeiÅ†&ŽAØ¡i«Ž·ç˜±ãm^E¬•c™i¯-è`‰¶,ÅÒM+»ŠˆÛ_$Œ{0o¡¯4`oÐÏãõ
^x=Oï7¼~ÏVÔ~"ÕÖ¹úY*‚?2¡#Öz>‘_¯¿wKüÐË5Q6!‚õ«Î•0Ugþê˜anyû+3õt–ŒÕÑM~±'ŒkÉ#Ÿ“2êëÐ›¼ÏÆyrZV Øª‹“­èV$õx¦.¤lG*6h?Ñf­Ç	!cüBè.¯Io\4îÿD®ˆÏ†'iî?.—-ë–üh¡…ª~Ñ
Ê¡hSZ]šE—ýÝšo­à§ÜDíÉÆ†1ãªÈ¼^<"HSŽN:3»ÚýpjE4Wkåª¿×{ÛEŒ›L'¤ä2©í„g
y÷Œúä˜W`X%,È¯é
 •4˜À¿ì€ÓtÜÖfdm¨¹džEî62ïdÕYØ4é?GÛü­©KFöÃµ}Ã¢˜Å±;šœû!DûfLXq«–°™•š¾‚¬ÒX£þ¬¥¶1ù$Æ\®ÊKIxx`Ÿ,ê³’3¥ua  ÝL&£wÑŒ|9P®¬oA±4ªD‰ë[µ52bz¾¾þª:»h›B±ÕÔ{L.è¯î4SÊbáYà„$2ÂGþyótý–á¹v¤úËÇó_WÉXF,cª[js
“E(c/p¬yL¶Þ*S€ƒ?C)[teç»9<tF^
öVRåÉ §“ØëÇü}ÔEò:=U±Ö¨êÄùÞŠ
”]lÑ®®¾À$"ƒÛ2`®¬8‰ÏX§É&«ò%°ÕHø5If¿ƒ? Šì#_xàÝµx"¦äÝ¡Y6ÏØŠ®îü5UµÄÜNauÝnü+Ë¸F:`WÑÀpŠÚI=Ð£g±v®[â¯/W}§¸¿Â÷-Ø[Â/‚_èðÿ·´2ú@3ëó¡­Ò*#T­îÄÌ°cZðM»„Mî¬‹¢ÀŸKYˆP‘€J‡Ÿz½’B¤e¨&žÔà¹p0¶	‡?•²¨|ÎÃ±×HÍgO¸Ï<19fœðNÊ)w6ù­ìÉV×#ª¶Ušˆ'	?=­Sô2Wº_âM§nI7Øu$Ê¼^óF£U|¸¦ x}?Ir6ýÍßºåd{´aÎ—~µˆŠ±ó	ùü•y„¬ÈXäDL÷€šý¤‰#x1Š|jsAô8î…€Ü±Ï4ùúôÂ_­kNÜ“µyò‚‡\·“„¹,2	îÛ¾”rúv+zÑÒ2ëXˆRòð
8¾RÂWþ…§ÐÉ_8q¾`Šîì"mŸ‘ûÐÄ’í¤=wuüú†y:„ÙÁ™«K-ßM
j²Ùïëa›~mèyqáRônÂClÓª (Á³¯<|0È §ûDí ) Lè‹Á=$Ì¥uS/ £ïP¶$, ëÚ²þÃ'©/ÐäÃ #ë\êN%ˆÚÎ(R¼fº¬žud6µ‚Cáý?s¦oµÒjäRêr«¨;”zYžÉüi°S‰WÁÜÚâU,^°Àþ+ééÃef¦Ëãê_HÎtÈƒt&€ gÝk¶F.Ð·>CþÿÏ1NÚu#CÛÆ *Çþ6¹«Â“î¢š|Ø©1{µñ®òìÄ¸ûkË+ŒS›ÒMÉv½w_ÀC ïƒ“3q{&Ëhý1èª/ÔÞ¥áÜ„þÊßi¨13®ä‹–L­ÆSÙßµooz¼Ê_%o=Ä ,`Q Î·ƒ¬Ãù™»±¸Ä SˆHsçÕà+ÿ¼P£ù¸d© ”Ë¼‰U¥hÇ7')ï¦‚ÌœùEƒ¹“!»ñ30åÛ†×bÑ0üê {ee¯’qàÂ¾‚¬^÷²døb_óÞ0“SêP	)‡­…5®·µò€†“NrŠrõ FŠ_"¿¤ÀØ$&õ^îøÆ¶Cµ;Ï	˜†½M40ºÛäáÕaA
BÍ¹G$w*Ø=ÝUô. ¢‘™’z€ÍÁ°cŽS¬ªiAôÔ+8·÷É¬&{N›ñ¡VÓzRú)9Á·6Ö&&íèÆš’Yÿ×ùfy¸ÍÚtÆÒö”ÛáBòÜXÅ Šž=-°ÎSÀ?à	^ð —ÝBƒ,•&º“>€xl2kj¯=vJJì]Ûm<#”Øº\<›"|‰¼võ«‘€(Ü*¯Ç^eLt`"ù,[íÉ…2÷ì÷tX@92NäßÁÖd½c]d®Q4§)Kµ›ýg¹F‡{«üãqÇÅ^ÔP`hÄå³ †Å§%ó¿‘j§¾"GVáDö¶9ï i”/86êI#ê	|L˜#¨<£œ©0)ÌG’& F¥ÕŸ"Ú@"’Žzºz/bªœžùº:HÁ’B¹Ëàáô¶©Ç—ª\T¸åq`ºfÜI°g#£›@0¡R&`@«Ž—oz†iè‚)²c½lª‘Î,ÖI†Éæ Ãùn &s*#Lµ÷Oê7@eF,”1Àh@U+¹üÍè¢ c<Ó,øžÜ”ºÑä¦'-UŽÅ².~tC²…´~”€ÕCweŸ]rÎ¼àÃ©¬;W©´ÉžqÃ©uêþ‰ñÝh{âªô…‹Ýpwª`–:¿@XKa~à¹ÜÁfÑ\uìæÈ ;Äü„	DzÁä?Óù™ÄÙ'o\À>În7]¦	H5!•+@,ï0B‹rºÏã]â€…ž¿E&½dod£õMÒil“ã4s×æ/·ætÁ=zK«‰xz ö¡Ên¨Pp—Ar:¯š<BÇÉÞžiEDÀzBÃTÛž]å/"xù9ä›KWV}Ûí¤<¯Ó"…MŒm$OnS€”)±ÛT	¥»Ú'5ÞG¼°QÄVg~éíäéõ¨Xu” )ÎñˆG^~\Ò¸ùaúÚ"ž	Xy2üòJ1ª…¬NVçÀô÷EÏz8Ä÷œŽ'Kÿã¼)wUkSUÖÆ:OÙßÑ†8èùÃpF¤-²Àô>‡iþÍ¿OÅ¢“wÏ…FWQiX×³7Ì®e%Nf§>ßžOšlÖ•ËW.ÌEkÇ­;‚Šü‰ßË_ª¤£3?ZUAhYˆ‘…ÄmûB¦¤¥	û4Ìôšïó~èpžú,‚æ}1ƒWq*ËQg<)ø”ÿ›Ç^€“¦Q!ßžYJ¼ÝNq8èiéw#Ûg.VðTCc&)ÂgêÔ&ò&3”^¨£(åØq •ŸÏjú~‹žw^Ã­ÌgLÂ ƒ¸û¦4Cn©üŠÃ_ƒb„€eâ^ÛÉIê¤Ÿv„;„Izì›Èsw|‡‚3Â(%* r=ç­E˜ÈDgsždEsSQ†wŠJ8Ÿª»PØ1Ödb3Æ}ÿ{÷	#%dÀvQ<™!n y­üË·ô¼æf­ø­÷ãlÁÆŠGú3 ÌW#¼Üºž,ÿDnLé€`ü“°/ÃÀj´·Ã¸¯zÆ?djxŠÁ‘.¾GÜÔºZëH1ŠÒ@Z‚ÂåòéôM=Š„`é»E¼Tj5¿ºÁ7)ÚÅåV@{7]òý8%g ¸ÇØšç¯_6{"V©p×L!G˜&màã”¾]îáhQ„wLÓä· wŒ„”û(çªµÛ@âá'¶&7é'm$R¶ÔÆÁÈê+ØüÄï<ÇßÕ^Ë8­ñ{›(àPKá2GlíòðMü¢ze¹¥:g–QÔŒvº	]7‰í°T¢¿1†Ð[x'"°%m~…I_¦ìxeÍ.6Ö½[’„Ä:±ÿM(ˆï*½·V•­ÈÕU9Õ]ƒ=6D)X}l]&Ù
jw(Ø7à ý«y1Žã(ž@¨ÄiwÎÝ³øl‹(½áP²74×CæoDÏô÷VK\³tÇ©iz×)Ø
Ãw#ÜÂ¼CâIàÉ÷ºbBq°—5T\Í¥L¹ß‹ýKÖI+ÑÂëì.ÔQiD:Ò«žÂ)^ê›òÇ¢n4V#úGW#ôšNó=-ÆLîèúÂ³;ü^â¶-‰ÞzÍ@_ÖSÈjã¤ÏVº4»O¥ºò c–Ã(J‘