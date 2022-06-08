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
       �V�~�f=4k웓�,�� �C;lUNK�N�q:Bq�я�ѫ�
�?j{��?���9��m�D�6N�w��h��	p���Xڧ��8�d+M���!-��L�U!2981����E`�J��+���_^����R_[
q�����E�+�����pތ�o��>h����>�}�)��/p�����	��L�|�4��s�C����Y���4X��e:F�M�<�����]P���8&@f�Bp!gD�.�"�t�0�z"y���I�rҝ�N���O ��z��|�y�s�P%���K���مtn�l��`��#KN"ID�R��0�#O ��-�a�W��	t����c���� ٪$!o�M�.\����t���  a9��.�X�gi�2i��������ɐw���4��g�&w��[Q�P�O�s�m7T�o�q=Y'��\��8��m��bw"�,�>R�f��A
#�����H/��UHf/�Nu�g �8�6����u'}�+h�c����M��fk�;�Gq�y���G22)+�F�WHr[�/.S)`౳#tQ	L��[>��@/�#AH7F>Tyٗ"d����!%��<j=J.�W���S]�7����h���pb)=؈���45��A���_�?�VvA��$'�srP^�-&�z���:@�q��u�Jx�@�]��k3N���Ip�'�9Ȇ�����T �7�:�F���J�A�\pC���Ũ�
�ps'C^~�����̵M�2B`�~�{qK#�T ���gԇ}b1��HnП;2��J�m��m�|&Y��-:��f��_��̉xO��Ϳ<kv�w2�
��Fk��񸎓��RZFd��8zΈ���Y>��� l�԰��\��39����L�����۾mHF�����-�렮Z;�XXտP$~K^��7��Ȝ�����X
�ʓ��ۺq�{���c�o�O,c�o�p� �C�J7��P��_a�ۡܣ�Fψ.��n5<a�ޛ9,�0��k� �&-nof�\�ۨs���|�<�@nϦW�/�0�:��c��x�;V�P���lc�0�+#�����r�;��G2�H�#I�#���d��Z�S����իf�����I���1�LI�?�z	�)��Pii�:W��y��;��<�b��e���ѡ	��R��x߾JJ�]8�
j<I}m��SQ����|}�����Q��Xӏ\uY��@�q0}��tF^�ƬlJ�X߱�nz�e�x�x~M>��B�ضdӿQ���^�RE�E�^T�ҳ��m@R���6y��	������g�/W������HwP�s��
�� ����j��< �hӱ�+��#e����sƒb�V�|��q��$Xb�\�n�*2zr.��8�@�o�-�A=dՂA�i5�pRd�n���#<c����"PZ�7��0]U�;���'5�y�to,�J�.z� d��!��a�16�[�˱n�5�H��C%�(߭���Xz�w�fw�^�D29�����30�U7�Z(q�u
ݡ?����A��
c�gYe�(�����|�HJ3������s?�ޡen6�m�u�<�k>�R0K�[��^��G�/�)��y:6W�ƛ�e�t�O)��۾���pϫ���E��h'$u߽@Y	�G�۱�0��3�� p�YX\��𱤵;��Bw��)��ٗ���6+3�i�w�8�jT����ЙD�����'k�[�x識~ K��N��?��Ւ��̭iz��s�b��>!x� ���\ߩtKʾ^��8.��L���G$�Hڳ���V���V3	��6�D��#-FO0 �%����5.Ѝ��
Nhn��2`����,q���*���߂G�8r�����.`�'I�O?�SeZ�;S��E�
(��ܖɉ���a�ņ7��؍��C�б^`�8���g�f��j��������ˈx0"1�<�N�L~�u�_���Loq��̦�(�=>e헿���b��%�f �a�J|V�9�K��|b��ӷ�kiY��q��P�4�S���hN�������_����d���W_e��+fdd�榮�g!�����l8'M��-���CR��JϪ�X=J����4�p���}�g��:�|�h���vқY	�cy ���u���@��v��q"�j�W�
5�"4��~y0R �7',I�x$Q�oE���I�G�m��L "�7J) ��	w�%=NT�0�^gO0���s,-��r�y����9��!	{���;�5��P_F�0�?e����%Ů� ��lL}%b�����K�"����j�>P��V+=�M~�4n�ʷ��}���S$�r ���ѱ�i"�+s�[���Bg�E��Ai=�t�<ɫԂi�_�7�o�EM��E��X�[߈n��_w5�V��ǱpT��F�����[�lRW�8��΁�	/�O'�D@�H�p�o~��x�1�׺��P����w_���*�?Ţ���ԶmV�/�kI�M��3vzm��e^-ܖ:ŕ�rἀ�4���w�o6����!K �Hے�26��:Ga_���Ć�2��|�gS�y��:��ٿ�b�X�{(��v�:pv3�?��U�K�)C�L���m�mtA�u&� �o� ��Vd��1I� x5�EP�u�@����-'�t_�X��t.�c�N
���~kr;<��c��_�7�i:��+v��[��"h�E��1��iԿ`��9�\v�Pp�z��,N�WK���[��B�E8�G�e˸��
�z6 ���g��k,}f1�WU
y��Nn�7|�l�������<�'&Ӆqx&1���~�(�ks�E�S8��61��8>�qCJE�q�)g٠���G��yH$� חiBRx����N���x+�|�^�*픚����'����[��z-�"�e+����/�������s��E��o�#�"n�����$���� �v�: K�W�l���|&ݍn5혬N��D�=R1���+	���*+��[j��/&+���bϱ���
��߈+4����'!�M�'�(?k	%�Ld@h���g�. ��+���i� �N�dx1o/R�����\h���Bӧ��ޢ���y�@��˭��ˀpt�9������KHuܣ�&�{��cD��	���9��`~����ֳx,g�ng۹ڷ,������/�?��B��*K���C/Fq7� [n]48b��/�O%�y����b������e$�{���38zD�W�:�
Lf��/��j���Z�n��y��J%}y
Ƴd��V�;�f���N��.Ы�X����"�҃�0�V��N�̓�?��,�ޕ�#�;�W	"��dR3I�ψT��g_!C��)�(*,Ph�+�)a�$F����Wm�xN��ʥ��Q�n���|:���h`/H��Ȕ(���ݱ���[��2��b�uR��z�xk92�[7,�ПC��7v��#�H�������zL�z��xo<e�Gjx�%Ԣ!��'Ypԓd�X��7.�I�&䮟���c(.t!�@��\TI\��#E�W�S�C��ʈ��NМ��cnK��W<,f��V�W��	�y�BE���|-m�`w7;��
$EJ��bb���}��Xpt=��<��� K@��OK��Έ�"*�4}k�X��p4U��l��F�u�Ϫ�G�����`é�T����L�LM�,6R�p_a}���$��/a}y���q � �b�	�k1� ��G^潍&�s����/F�'>����\�����S�lo�A>�O)Q�V�����G ���ka�s�LM}�(��bOD!7T�UZub���i)]i�0L���lʇ5�^�G���*����9�a�M��~��-"�ݚ��3��B���-�������=��mq����q���h��˼@��%�QV�4x!�_o
g8�� sB�ֻ�&+7��I	$��Ջz���Z��W:&O����I�dUH�yԢ�q)�e�1>ϵU޴�!&t	�/��<���o�0�K�]w"����e��Y��OdW�w�%7��nzo��.�)�
ϗ�IE���~�=)��+��[t��n��b��F�؆%q�w�w��o��Bp��{s�3��tW��w�����ф����c�'G�.	�8�PT*�P��!xX�FD�d��%^y�҆4$��K���b�5�����t���}{�3�4�aZ'��Y��� ݂�����4�?@;L>O�7����D�C�\�1WLg��͟�|D��~�6fU��� �� ���#��:�Ǌ�����0v3�۬&���n��Z��5Y�h�4�:D��\�:։۲ĚC}{O>��e���j�D��xÕ5B�l�#����Z�h#y��r5��[��yX�|%�n�P&h����:yV+��c��۶���$��R��h���r�P��E��LwK��'CR�/�4r�D�C�d�Rsl�w�~��:�2H�sx�����>�6,qdyZVЎB��\����#Q�r}�ڻ!�#�*ILQr���lX�:{�g錝/�W���ؒ��wL%�M@CO�fe�ˣp�Em���_�M�&	r�5��IY� 8 M,�7���G
���{S.O<3��°w8�5�H�s�_H�,���C@*]\G����kl�h�=p�O{��̄�]�6�.�o{|��ݠ���V0��c�����S��H�nQ�q��k�li��f�I0�
���ɍ��n�6KEm������v򤍡$� ��Kmz<0��p��;쥈x,���"�I��x�qlJ֖8�qȓ�-��C�$C���ؗ�lNh�U0v�?D�|WX��)��$M
�Pte?g�L��0�B}*�p6��rٝ
���8h��V����5R��h�����$,�A�7�������INTV��W�nӎ��~��^�a��^�����W�����g�4N�2�eȰ����5�"D�&UUw���;��~�\%-�@��7����6 >O��Q�#Xkئ覝vewΰ���Ȁ~aFx��Q�Z��ؚ��Ҷ�6,W�Øzdb'Y��A�񑴁�Ć<(���L�i�*�06��0�K��̔� "!�֖l�X��ѕ�ׁ�������-��Y8�Ǣc���&��fG�p�X��;-щU��Wdβ��p!��%jU��9
r���Ճ�S�r�:�1q\M���v�����:�0w���%�VaQ���?W�%W8+�m@ԯ�j�0O��RO�Ѣ�ف`�
'V�{�qqY�_"�ȿ��A�E����9��%�L��8en0������*�Wf�26�T+�a�0)��j�ډ�Л�z[�Il�%��`"�X\�fo[�:�������-])��iT����;���B��(�/v��s���n1�*��mɚ��"��Ϭ��a�?��� ^��a*��vr�S��ϔV쮹c�]��#H~�>2�i�p�Pkg,2Ja���y��5��
��E$pd>G�NޛW��[w��μ�;4�j|��k�������j��M,v���°|�wR��GH�a�I�l�c�l�7�)���Ӗe}���كtU�틤p_X1�v_/lM���j���i�m�uH�o-�бݭ+r�/�Mf��BQ��:
�8AR��PAV@6�Q�6�J�|�H	k$[�1��Mk0���(�������P�U�l�D8j�ez��t�] \T��;��j� {v��G8;`���SD��g'��;Oِ�3�n�'�����W+tnߢ{[=W5��������T�u����eg���z�Q�@M��1?)	��F`��'g|��mt�~&��-B�p��%QE؃�&�E�:�Ѥ��
��jf�z*�8��I�g�����˱�u2����6Ab�Hh�H��H|�c|{��4����2�!`���͐�NI��I趡`�0aJ��TT�1���o��w�����}��p�I��gf@��^z�6ִu4c�匑�,��6������#�x<nT�(���5ۺ��[p��Ȏ
���Cc���e�7�i�Z�Pj�o� a�%q,5����1իT&<g�,����`�<q+6�=��T�����q*<D�볊�-0��6s g~&S2��J%���G��a/!�|���5�]D��;�=����w$�d��bK&>�'�V���h�<��(x�Mb��w�DsUx��`�̬0ҀU������?h��a��a�����CM�G�����a.�һ��O����,���]Gc�2����� ��2GsZ�7Ȅ�d�D|�y��7��c��kk��f3`�r3��K���TmJ������<�o��L	�w��`џ���k�7%^XY���\״���^v{�ֆ��C�����Uݓo�����P��"LH ��)D�NsIF��e�����q��/�����9)1e�����5�r���_�}i{�W4�Ȳ��]_�j�d�3�X�UIT���r$���2���O�箞�u�Ù�&��B]9�/�B��!̔��bw<J�z��p	��<�@�]��p;�g�zGʅ�@�ө$ǅ���5q �2�?f��V�2��b��˔�׺�Cu��6�i��O���Q�3�Ŝ詋?��E�w�/ݨ��x�%Up!3-�&wwR��at9�?̼�v�4��Hi�F�<���~o� $q����(��$p�f��&���xx�,�H`o_���@��	.����_@�hV���'�!l"������0�@��y)6)����jY���S5���4���:%Z�T0鷘?��Vݡ?304,�H5E��:�
O��AFN�V��^u*��Y�U �Q��#qm�'܊���v��pG��dm����T�mMi?�C�����g��,
�z�.�$.քF���)���.�Y2��&����q��BLaj���4�o�SZ�����b#�ݼD��}�Kw�������t�\R�A�Ԋ&����!X3�uT�V�Co�!֥��:vW�^gɪ���������Bf"�Py��dY��j ��Γ�qS9�x�bN!���k˯E��������|��r��(\�����9Ѩa��ZiJ�5*1�8�F���6��7	���b�yaKĪ)���QFt�E1���2+Wf��%����V���������^
�:S�c�CL�|,��#�� �.�"�{������xǁ����f�����vF��D���u����o�"-m��ޒ��D"�����k��P wj���]�{Z���Y���iN�z��"��k�����;�8�l�+�Fd�\�1-P�/ۄC�g:*�-������`�pc�E-��;Х�"�1�"FsM�i�Y0�7�u��Z$�zZ ��_��/?��O�"ux��\o�U�w�}�e�1v�b���'����XWM/��P��t�!�}S��-��w�$c��&�|�F1��w�}uQ�A�W���îU�W(�=�LQ�T���υ���-a�j����wT<w�WAQ=�V/ǋ�J���V�Qk1v�_�����!ݻ��s����|C�γ{"�N��*��[�)q�&��+��!jsu:%N,�H������I���)Rc��۷�p����~������ �v�^�"�S�[֪�	Y���`+WJ�ڟ9��HZ�_0�����	}��8{�Go���(�|oZ�"Tx4x�R��b��@A�D�Ho���	�KR���������K�s�ʥ[5��G��hg��z��DH��aM���Κ��}$;�&�_�y��G�E�Cy�6Ĭe)N}�w�����a�T��~5k�p�&�$���ɤB��AO�p��kYY������\������|4BN��X���I��������]3Wn8栊�Y�-z��{*u��i��Ѻ��q&�
Ѵ7 ���8�&���+��쫖��3$ڜg�:��oƀ�,��?ōdGו鹬k�ߣC�I"�?L�׏��� �146��02�qX���x6k�ބ�lDgz��n�
qx�V�頁�s\C�kK 5w��'� G`t:/P��Z��� \+YCFi���=���=��� v�@:X�7�q�H��H��~�F(�a��U�Z�uy�x<�c*/%�B˹`_��EA$%�q�G}s��u9����Z.f�ǋ,��pY�X.
=C� �ƀ����D�N9c6okŢ�g�`ӦN�5�\X]R�����^={���3����=��i��;X����"����n��n|f�Լ�d�8����*���P��PUV{����?��_u �0~Zhy�@��<7�s�Ra.�%����Gj�Eػ��?H�@����\�D�v��>A�
��Ss2�B�M|'4�b,U�Z�O�[!�j�8��l�����'�}���;]{@/�[�p���X�!�s��=�`�������5z+ H{+i.�;ޒ�D��9��N������\��t���ȥS{�GM��#���o���!D�8�6�WZN�3���۶��0�N=O3��_�4N�b �o�Y��%�C��l�j1�~)���z�L(<}�2Z�s�Jq��w�Z�e�y6�Fq���W%H}��1o�,w��퉟���@��"�|�Z�銟;lZVy66�C�I�k�8��qj;g_�~��(��
��BkT��Q	_����͟�z6��%��k3 �����96�Mqô ]5�<�\Y���eUe%�S��B`	U�$:ڷ"d�G�M1����M�B˻s�ڇR������L9��6� ��|���G.�T�<gn+��"z�c77�u��'}L���	fM6Ś�Tx�����c�8oL��9���B	���IE�Y-�]��v���8R �	!r)�?�t��4���%�sM�r���G�iC)8t�xf��@Y7����\�}J��!����}�Y� �jj<� Y5b�P���h����_wԜ\�=*�M��F��O�'��>����G��nS+lZm7�s�8���DCΔ��e��\�s��3��A�����Cx�,�#ZZ��s�r��}��	*�HZ���/cƦ�>t���븝=��&W���r/�96~�/I˝��	�vl)��z��lRJ�/�kk�xE�d��9ebD	[�f6�_e���E�t�C@�NѲ��>��t�!�Ц�����D��iߐ�����T���/���������rÿVH�~8+t6��[篜��|��[ah�R��_�����۵������X`�%"��쓽�#�������a�o;���x�Ph����}�N:����
D���	'�s�F� C޾{^���H�_Ċ���f�k��'�!]e�-��$@Ҩ�/�Z���Qi�qN�yz�{��=˜���n��6�U�Z4�����ˋ~����,�o���ֹIӘu�~i��Bm�Y_�ɖ�|"�-��Yt�[M���`�xB���?噖�*�pB�Փ�%n)�- BɁm�����M����c�w#�� Z`a�:Ӳ��S �$>:��T09A�0X������|�iw���(n�G��<Ւ_%�Y���Z��-�q`��&��n�~�)7?�=j��k�}�=�5��V}/z_t�ltY��]x��,[eNB-���ur]8Y!#E����`�o�{9ҺS|W�e�����Vi� A��3���GlS>�I2$fu�S1T�Dڝ�+.��؏��-�#��7�� �~�{'������K�Cx#�n}�$�t���Z�^��Z��u�w��܋��(
vղ�&pb:�Y*��?h��uI�e��� �%%��\X2����� h��2F���ЭN��jT�q3��{�I�,�B=���;��Kc�z��ߴ��WM�6���`�0��c��X�]��yB/������Z���v��1Gi�wQf1tW����*�2���"݊�)�C����ɱmW�^�2QTSI�W��
}�Qp�1�#���_�P��(���?*7"�Ǌ5��1dJ}���$��N�H9Z�5�\
�v>^@K�e6��9�o��K���2�5��>֔|D)�"7h�A[H8��}�zýz�\�漪��j�ž��I�7�T��t��'��}����=�&���I=w�BΆ!�&5.2S�`�����i&�)2��9N��r�jV'	Ĥ��������W��8jS<��������v�~�ԛ�Vm&��ЖƼl�:��Y���lz�
ȸ��z2 �/���K����	��̨Of�~Iw���Ԗ$�(H0~F�δ��A<Y�	sD�:��e-)Ӹ��8ZXP���s����
��o2&�re��t��e�sQB "���q~	iI�EC���m�m�L��}#+N�Ը%)fP�xI��Os�b����NO�[ܬͧ8�蘹R+o���AVc�I���6=T�{��(.$�[8�|��٤��.�O�}���_^�_50V�-��Z�|(�b� �H�z��i<�j.a`b����o�-$+��]NP���c��|����&'��)���l��@̝�X+&��̨�_;0zt��{&��`C��D n����K�'�v>��$/0g6��F��},aBpSG�����=:
��6�ZV��bk"~p��}t�r�ҧv�s�:���*��0*[�ݪ�J
�G��'��N�$� �GcB��KB�/2T�9�i��ב��m�ܙrq�D↜$�S:.��ոY�F�[ūW[}��%�6�P %�X}�3+���w�o5���ToƳ�Ҋ�D%����Z�}oWJ%�2��������#�qM�3K��7�`ͯ%{��"F6��W�l}a�)8�����ĺ�8���k�x3O�L�	��?!�lZ�g��u�{�׿4-s�9�� ٯ���,�x^��n�ȅ��L��?6��w40���|]����p����D��>L3ڃ��3M�F[2#��O�S����onT1`��b-�qnR�B3.(�И6 Ąxu<��-�@��T�t|�7x�Z�<�.����e�ʕA-3��2ˌ�]��?qlZ��ACLOl�����U?Ai�;Kb����m�Z��(�c��i�A��^�h�Y('��Zq�&��~�p>���]F�7�Kjzd�G=0n�7�^� 3r�P��S���W46�����B��/�E_O�G�A9?o[��|W$�^P��.�DL�Kw��-DrQKSXe�,fZűc�	�.ðt���W�V�;9����K������6�E�)s�S�46�_2��	e�1�U@��=8k�i	1z�̔�N�̝���&zLJ ���m�p�F$�Ew�~~'+�	|g�>{썂�S8�E��NƁ��(f(;�2��2H�
�ޯ�ޢ	z��q�K���R��}�����9\Bv!�O௚ƍX�6s!���n�h�+m�y�S^4E;ĎV_��BK����a��hs���[x�κ��� +V����5V~�Ű)��r��
 "3��B���H��A���G�ì�oV�;���BS7FSa���|�G�Q�NPH[H&vWB�m�{�#uh15�x%lǒ]�CĪ�,�e"{B����b#�.��D�3�Ggȫ�>2?��#�����ꁽ�X�j!����q�X��_����窮'�0i�wy�DNށD����ix��.2��C[_����/Y�(��"�w4�d�[�@d����R/���r�4D.� �s��}s�lo�wM��G#��FL�y"<t�C�Ocz�K�i�l
k�����f�G�&��{Ӈ{�����?�����W�2}�\��U~�t ��gue�2����b]�y�b��g��|�4`k��cAt���ef�I][�|6������?�3�x�H4}�,���.HQ%�0'Q��=���d��5�To����
Ɗ�x�GK#�W'���d:�p:"[7K|.A�#���o�EӛI����&Ԩ�w%k�Mø].�
�<X(��βv�L�a�Ŷ[#�N��'�BzP��̈́��)~�5�J�K�^�c~��s����΅{�A/�!m�!�P���º(��5ć�D1*7����_�� È	���G�l6ow�]b�q�Z���i�(�0�>y��c�gx�|�o��IW\�%*r8�SX����8j'Il�l����TFK�aC���aeqP��Õ�u�gi�S�r���yu�.y�)��A��O(�m.[�lf�Ѡg�.��>ea�k�9��C]��`��6�"|ء��T��.Ʊ�]8�t��B�1����SJ*]�ݿ�ᔾij.����w�*���*L5�����*Ǟ[�O�S�*
ET�f�����Xc6�LCGBþ�Ø1O�_��i�f�8v5ƫ��,�AQ}tی��u����|��*����o�YS��q1�NGR���p��ӌ��,�Oc����5i��p��4�s�cn��-+���q��p��|�+вU�Z9lT���%c�P��><6t�>�b��j���m�/���@�e����[)`�+I�Roj��8F�X�U�K#yC=G&V�c�u�-��P���+ش��"F02Y*�O~��MU���8Ƙہ��Y�z�Jڃ3>��M1Fx�(���`x?}��=���1l��J)�ǉ�������a��h��j	�0�f��E9 }������&d��֨JFGx������g��g�i�fqmN�Ͳ������\���~��pw�^l�Q�Dp�#��Iޖ�cS3�
�a�6�Ϭpym�cꫳ(Λ�O=mW����tK���}�@�(� �.8���ɰZ�_��O΀���J����M�C[ǅ�µ��l����"l�"x�`�T �e)θU뻨*��&	�k��.�
O����$y�1��H���ǧ�j]�o�P�l���qQOڨa����>�.���g?ȳ����Q3S�)�u𣵗��
R7���%�UC�8�E��7�beO�@
�oh�=��Z��"f6�U�����<;�"�.T�^Q������Z~vXdK��+�l�G�3K���k�3l[x��BC�F������H�Bv�۳��L*	f����'A�������L@j���0��l��\**@%��'��D�UA�`�0����ߑ|���juMhn�G;�$�9�����m�_<�1}t��q�����p#��,<z%[�v:b�5�(����f(���jN��;５&��ص�&�����A{a�ę���P��w�(+��7CqBeB���^��yvCO����^�t��Vخ���u� B��Y,
#ͨi�����]����<n�,H�D>��^��͞�<[y��hcl��6Cu�����Z�2?D�q�ʵpɪ4`
��Բ�Ͼ�/�v��[Ϧ�6q��m��*�OB4>ѐa�?u�4$�{��U�i�Z�)�kY�ͭvP�������)^;Ζ�j�5M�a ��dʛŤ�p=��Q��3�b�:,#����znV_�}(�o7�=綫�7e�#K�~y�T�,���{�x6 o���z��C��ӐTP`�{s���%��Y������u�3�u���:�RЌ�qN�+l��1��Vx����}�aM6�fZQ���f�I�D�!H�_��<8��:c�mӐ�����+�ț�y�+s�RM�o��6A�!���7��d��¶��ad 7�a]w�<�OE;�Xg��=f�F)msN��T�r�!�}��z����4��ױ~�2���"6���>~�A��b�ѣ�Q�2��T�������ti��hݗ!R
�y�J�^N7�T�j)����-}���f�]b�D�uԧ߳��Bp_����"g1��h�I��%m����W ��,v��Y�F�k/
�q����`� ����3�G�	4��/�W$�V�(~{�hb����`��.�!��;$�6��$�>���w�.8k�Վ��e���~mR�_?.�qB $T�9X�pN	��}��|�K�ˋ��t�H`����Ii���W�DBff*_��+
��R�m?�3��o��h�GKe$�t�8�QY}�����==�P*:g%0~)U#}�F���~�ϫ�Tך�wSb��v��*��S�G6�5q���]h�#��*�
Ay,)S����_J��{�u�[������b~9���4��q�)Wmy��pe,�2��s��͓Sxt�-q�녨5W�Ωv�'t��$`�D�$��[��k(YXE3�5���0Ґ.R8��ڎVR,�DąF(�D�$(	uNN�d'�����w0:;S]���j�)��s�\���S�� 9��8w�IIMk�;�kM��?�[*�=*�v�P`�^T��%���V�C,dL_��&��TC9y_D�;���`*�mk��8�[C�;�O�I����f����;T��c@p�L��zL=�2sު��\}�Z
.���я��>��|�
cI����Lܴ�Z�uw�:�!3;g8�����R�;����kP��/�ZY�m���{�A}'uK��p�p�W�9S�z���G-�2A����t��I�[%K���219Y��� B���lر"�7WA�yZ�@ˋ9qǢÉ7�H�n�<
�A1���*�˸��6|��#N��������!l!�Ճ�B5lwO���T�&�A��+���[V��V���CZL��/���Lc�9���U%���4�xe���G�i�zL���i'+����������8h+��i��хR���%@�V�pc�\��	�������]s���)� %�� Ӟ�(i&u#���.C"�n�S���5�uc_80��;�i,F�0Y(��;�xV.Xg��N���W�O�:;�z������ϱ�s�Qj�ǲ�6_������nk�TTˤ׀�tBq%Mh`-��g��M���+�B;��
3&]a�݆r?-,���S1a�P�+��Z����w!v����k�G�L�Qf޺M&+�j̾�1,����,7H|k�V������Z����@�n�5���Ŕv���;o��-!Jx�fڟ�p��tPsݻ��@h�i{zC���r'&�yuv�j����'�Pp4tD ��n�th����G���!��"�~�]gM��S���f=�=���`�3}q-�[���ukL�w.ZzuR�_�p&��N�,���t7�|�G}C�H�0�ꁔd�f����킌��P����e�c��.浭�`���x��y^S�P�j�w�D�FZ�I��[��?VBUne'Mh�`B�t�6�Q)�í��U�8�FO�gD��Ϊ\d���)q�:|�	�r53�)M��q�|�ƫ���4�x�8%�Y�M;��
��Q���� ���������q��|
qQ(9�����©ElDc�Ek��J��f �&G-ɽ�Єج��qn����v[y�x_�o&��* ��ef�`Vl���<S��_��-�[t�~�#w]��P7��ì&�z�f�F������t�z��gL3�.�,���か�z�ޝ �'�ͣ��<�U`��`�I9�/z�RP������P���KWy��i�/igo�W/�{ϳ#��q$E\�^�����h�zB������@����-�X����wГ����� ������I�1Av,��55�d� �Yn�V�P�WA[��tbc9|� l��d�!� �0�� xqc�׫��Dt��@�Cuʥ��%�� s1�f�jr���:"��Q�?�.+��3)�����!XB]P���<�s:�$Q~�`�4^��J����ҏ[�����-���{;ԙ5�ccڏj�_(+�*��4%����:�M�����Wj�a3���>o o]�؄�[o��L4��PLB�Ŭ���vW�$f	�tLІ��K��O��[x:v��OJ���ܽC����Q��� �p���/`�u�*��'���.N��S(d�~
�M��+d��G�YB�X�e�V�~�(x���*��Ɩ��A5�Q�
��"5���T���[�q�^�~zAQ��9�n�u�b���Z�o"����7���p�Ow���uh����X�\
��KF.�T�Ɏ���v�ތ����'Jz��)<X
\=s�M�Gb/�53��aź��@6���o�����e�ݷk���\�.-�7�j݆�A6S�-hU���9	.�v(>��V�vI嘜��{A�{��X�M$;�%���VӄD����qZ�0H�x���t�;�>��m��f#�D8��j'D2m2�i=�eiņ&�Aءi���瘱�m^E��c�i�-�`��,��M+����_$��{0o��4`o����
^�x=O�7�~�V�~"�ֹ�Y*�?2��#�z>�_��wK���5Q6!���Ε0Ug����any�+3�t����M~�'�k�#��2��Л���yrZV ت����V$�x�.�lG*6h?�f�Ǎ	!c�B�.�Io\4��D��φ'i�?.�-���h���~�
ʡhSZ]�E��ݚo���D��Ɔ1�ȼ^<"HS�N:3�ڍ�pjE4Wk媿�{�E��L'��2��g
y����W`X%�,ȯ�
 �4�����t��fdm��d�E�62�d�Y�4�?G����KF�õ�}â�ű;���!D�fLXq�����������X�����1�$�\��KIxx`�,ꁳ��3�u�a  �L&�wь|9P��oA�4�D��[�52bz����:�h�B���{L.��4S�b�Y��$2�G�y�t���v�����_W�XF,c�[js
�E�(c/p�yL��*S��?C)[te�9<tF^
�VR�ɠ������}�E�:=U�֨���ފ�
�]l������$"��2`��8��X��&��%���H�5If��? ��#_�x�ݵx"��ݡY6�؊���5U���Nau�n�+˸F:`W��p��I=Уg�v�[���/W}�����-�[�/�_�����2�@3���*#T���̰cZ��M��M����KY�P��J��z��B�e�&���p0��	�?���|�ñ�H�g�O��<19f��N�)w6����V�#��U��'	?=�S�2W�_�M�nI7�u$ʼ^�F�U|�� x}?Ir6��ߺ�d{�aΗ~������	���y���X�DL������#x1�|jsA�8ܱ�4����_�kNܓ�y�\����,2	�۾�r�v+z��2�X�R��
8�R�W�����_8q�`���"m����Ē�=wu���y:������K-�M
j����a�~m�yq�R�n�Cl�� (����<|0� ��D� ) L���=$̥uS/ ��P�$, �ڲ��'�/��� #�\�N%���(R�f���ud6��C��?s�o��j�R�r��;�zY���i�S�W����U,^���+���ef����_H�t���t&� g�k�F.з>C���1N�u#C�� *��6��|ة1�{����ĸ�k�+�S��M�v�w_�C 3q{&�h�1�/�ޥ��܄���i�13���L��S�ߵoo�z��_%o=Ġ,`Q������������ S�Hs���+��P���d���˼�U�h�7')廬̜�E���!��30�ۆ�b�0�� {ee��q�¾��^��d�b_��0�S�P	)���5������Nr�r� F�_"����$&�^��ƶC�;�	���M40�����a�A
B͹G$w*�=�U�. ����z����c�S��iA��+8��ɬ&{N��V�zR�)9��6�&&��ƚ�Y���fy���t������B��X� ��=-���S�?�	^� ��B�,�&��>�xl2kj�=vJJ�]�m<#���\<�"|���v����(�*��^eLt`"�,[�Ʌ2���tX@92N����d�c]d�Q4�)K����g�F�{���q�ŏ^�P`h�� �ŧ%�j��"GV�D��9� i�/86�I#�	|L�#�<����0)�G�& F��՟"�@"��z�z/b�����:H��B�������Ǘ�\T��q`�f�I�g#��@0�R&`@���oz�i�)�c�l���,�I��� ��n &s*#L��O�7@eF,�1�h@U+���� c<�,�������'-U�Ų.~tC���~���Cwe�]r�μ����;W��ɞqéu�����h{���pw�`�:��@XKa~���f�\u��� ;���	Dz��?�����'o\�>�n7]�	H5!�+@,�0B�r���] ��E&�dod��M�il��4s��/��t�=zK��xz ���n�Pp�A�r:��<B��ޞiED�zB�T۞]�/"x�9�KWV}��<���"�M�m$OnS��)��T	����'5�G��Q�Vg~������Xu� )��G^~\Ҹ�a����"�	Xy2��J1���NV�����E�z8����'K��)wUkSU��:O����8���pF�-����>�i�ͿOŢ�wυFWQiX׳7̮e%Nf�>ߞO�l֕�W.�Ekǭ;������_���3?ZUAhY����m�B���	�4������~�p��,��}1�Wq*�Qg<)�����^���Q!��YJ��Nq8�i�w#�g.V�TCc&)�g��&�&3�^��(��q ���j�~��w^í�gL ����4Cn����_�b��e�^��Iꤟv�;�Iz����sw|��3�(%* r=�E��Dgs�dEsSQ�w�J8���P�1��db�3�}�{�	#%d�vQ<�!n�y��˷���f�����l�ƊG�3 ̐W#�ܺ�,�DnL�`���/��j��ø�z�?djx���.�G�ԺZ�H1��@Z�������M=��`�E�Tj5���7)�ō�V@{7]��8%g ��ؚ�_6{"V�p�L!G�&m�㔾�]��hQ�wL�����w����(���@��'��&7�'m$R������+����<���^��8��{�(�PK�2Gl���M��ze��:g�QԌv�	]7���T��1���[x'"�%m~�I_��xe�.6ֽ[���:��M(��*��V����U9�]�=6�D)X}l]&�
jw(�7� ��y�1��(�@��iw�ݳ�l�(��P�74�C�oD���VK\�tǩiz�)�
�w#�¼C�I����bBq��5T\ͥL�ߋ�K�I+����.�QiD:ҫ��)^��Ǣn4V�#�GW#��N�=-�L���³;��^�-���z�@_�S�j��V�4�O��� c��(J�