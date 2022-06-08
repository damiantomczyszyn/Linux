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
�w#�¼C�I����bBq��5T\ͥL�ߋ�K�I+����.�QiD:ҫ��)^��Ǣn4V�#�GW#��N�=-�L���³;��^�-���z�@_�S�j��V�4�O��� c��(J�����F��-+�>?�\ь�(����T��_�,���T�v�5�b����j���+W'�~��2VDCk��
�q�1�س�8U�g��&��-NQ��f��J�j�z&��Y�?�ދp������-G0j����y��E ����1@W)��E�G�r�P~\=	����SC�k��!W��<1IS3
}��N+��k��Њ�zv{@���1Gx�}�9������n���ڞe����摘L\J�#�6�|cb�s��!׿��Gl�Iy[" Op�.��ֽɞ�@5['���#��\�v���G�G<s7[�3,�`ч�q�=�RxT��
A2_m�?�@pp|�~O��7�
���!\�>�Q%��@����x��c%Ŭ�3Y)#�gq���k���X.����
.6�M���幻��!6���@�7�⧧�G��Zd�Όg���2ꝺ�}�H��WX �nV��ո��r"Y�K9D7�g��lf���cF5f���5�ï�$>4��n��▷�1��ƫ�����rg4��w[�Q��5����S�_Q	��h��Lm���Ҥ]-H .{9��S���T&��~�c;�]Y��>t3%gH�����T�1�R�/�L����q�J�q�A#�k�Yyӌ�2#�7��
�`��
/���yi{0��t�W*����74�;�9J�Ѩ�ss	��a����+�"������i�{Q2+q����2��b��=ޏi�PۙG�)a3��G�z)����U�.�oH�I��#��'���
��g�4�j?+jN�߸YU���O?�$m�v9�!-Q��{W�O�R���5|��K�܀�]9[���$ᒱ��g�YI�;b��?�����*`�����:�ey������\�����[���f����8!;�)�9m�R%��m�,
ElSj�J=���Rn�K�k�r���80��#1�=9 T�o*���ɱ�]��� bn�����#��52�q�������X�^�^T��IӞ>�WQ^y��q,�̥�(]���rd���[��o�s��O������ZX���V(��_�Xy��B����u?A��[�����I�IA�����s�=�H�E?��s#�Q���@7���[9��[D3�[� �'�P& S���~��Ř�ݝ�W�#�>��"f!q��)X�a��"҉W>�L�|�X5we�4��蓓#<,�2�f�j�D��y�.�`/*{����\Gt�X��)#ِ�?�/#|��:k��R<E��e=���o��ח��l���4ek��V����!<=Z���C�؈ ���I�봋�����bA2`���҆�/N�[~�B�8i[�Dp�5����/�G�1zk��J�u����͸�S��X"�fFr=&2t�Ǟ��J�ͤ��]�}��^͜��z������	-�� � 7Z�r����Z Y�s�cɒ�"���΂�vD�FޤO��^�����1i}�L;�=�� �{�I	IO�#�V���*��Gsr��^JT7i�2~ꋺ����8�MM+�y,�L:�$�����\C�m���[T���4M`�	�)�PJ�j2�Pp���G�N}@���y��jFy��ǣ#v M>�Pͪ$�@�{�ӳG7�pgO;ڹ듌��D ,�l�5��˦R6���RT[�_DGEC�vSU�.ԅ�ʏ���>�:o�t� �VY:������N�3�]}����<�[�Z+`6d���)��Êܘr�ui:ѕ�>��q�L��m��|�����" ��
8�e+�8Cj���zq]3�e!��������k��xR��Ju���ԏ�4����+�k��	7)Yݰ,ϦQ��Dm��)��r��X�p��Y+�Haؘ�&��!�d���nAE{�k�Z}.|�
d�H���
����"������~JO<��JL[�@
gZ�o��st�� Q�m��x�\���(y��N�?5�nː�z�'�㸎-dk%מ1�*��i�����\o6�~�#j�-��ۅ�~���
�$>+��^��?���v���G�iǱ�a����>椥C�P�ؗ��&� �㿚�к��]��+X�_��or���XnkA$�C�b���L�����"�PEѪ�9���7j�W>΀��q#x:p�4��((���w<f��&�Sڵ�ZK��(���#��p���j��(�P���5����	���@Dh	!��z�S����X�����;�땦UY�T	�o��Z���	����H���%�:�?�Z�����ԐoE��:�c�+���gwc}�L1��%�ݙ\�X��D�f�M�#P4"��ԧ���I E�,�Y���<EQ�2!���&�np-��4h<�9��9駼Ԯ��#����y5�|D2�&<����Pj���~\�ܫ
v�2~�ZiP?+O��X��|��[{.	Z�鮽G�j�sￎ.:�X�[��;�u^a���l�V����X	��pY��@4`:�����9#H��s�Zq���o0�����R:C(�k���tE������EC	y�@p���Ӵ��Q������2��Y�Qm�L���\bwQ��J�a�%�7ɮ1�� om�V�:��(Z�B�mh2=#�|1�4�������w���HJ��-�.+��E��C�Ǹ$�o9հ������<� �w(������"�elӫ�nQ��A|`결���Aa&e���##��;��Tp]'5é��+����A�҉L8)(i������,l�	��	�� RG�Bφ /��ǫ`ߔ_Q1�'����i��3��i~.�	�;���JOG���'*��|GO9��gu-NoN�l�TO�e�>R)ȫw//�a�\�F��-,F�}J��ԉ?'GP1�|��COf'�s<c�"�p=vAv���m�܋g�����o8�b���p�Nu�}ӊ���X���C}�F��*��z3�3S�s;�؛�iR����V����ǎP���Y^�_q~
�4�ĭ�ͼ���^����1�y8�g.AOCbʞ�7r�%[:j*�q�:y���L���*O`�`?���Z�K��9�l�ɰ��˦���Z��s�r�c �|�x�]n���&��6,;����*%��ٱTS��Fs�W�������c�{K,E�9�?��Y��HU&*_� �qj�-,���W�汥�Cr�H�7q�my���D�0�R�Ր!�o���$��|�{�c;<��P~���FG!9Hm�Jb-n�Y��4)Mi� w'͚�P��&�e@3�Kh��a>�0�q�I�7����'A�<]�7]1��S�A�h�,N�c1�\LH�ҷ��0�/i����/I��P`x��N�GGA@�L��B1�V�@j��Ȅ��S;��ˇ�;tܸh�W�nϴq�wU/)65V�|�G�Q���oB�^�qؤ!�mF�9eq�,h�fzK,�%��<�M��C!ͪ{%��4�3�Am��#'Z�L��r8�,���g��CQ��-�@��Y�X�*>�ڃݺ&d���M�5I���HH�/C���G�-vRE�w��~;ڏ�+��Z����W^!
}s�&�r��x"K������X�.�in���K��.�mHѱk������aAt� ��%��+D�^��|�rl`�c��'�)S�6��j�ŧu�X!ΓJX6����4��:˫�k�I��OfD�x�(��n左�9E#�3N�v��^Aq�'lS��>?��^�n�cN��I������6��i��b�*��ꘘ�C.�[ ��`��ST�2�&�s�p��#KF᭞�x�L/"'�hI�!�r���K&��IZ?CQ�w��#B5Io|��X�,�
��#�~4�G��h����Gڐ� �v�Ư�q��˕�a��L���Ɓ?���%�q��#�.	�����Y���6B�nU���mw���;Cwb����gT~2��b��e��UWW��< {��q�Ή��C��I^��#���1�x��	T�����˸Z�?-�x��+�0�\���{�� ��0TbǄ�� 3����`E��Ivm���B�v�����atܙ'��
j�c#3.����wl���g���:�4�Wh(�D��; ��q֋=/gmR��Q�W�5%�
4��(�~��LD,�cӏ�QC���m@�E�,%;[ب���K�[ՊSL�>'���A�(k<.��1��kڲ
!"��^���u��EDrO�~8��	|�����E�G�]����d��-�qBӒh�!�8a��WpX��E5A��r��y\̓�G6af]�N�*eh����8�� �"�s�k:��h�2-���(��5���V�Z�MD0��7�����F��4��k�ȸ`�lV;���qP�5�@�,�ϳ8��6|j?M�q���]��M
_���f��$u<R�:-�E$�v[�/�ձMP3{U�s_K�������
o&�dZ���Ci��9JLU�Α��c�b����MR���*c
ХuWU�������|��vKv�Oip��"y���Fg��〗 �49��po>�|�ay�q_ص]��X�8�!]���E���t�){�"�H��~g�~(#�+M�{����@��=��@2���,K��[ӓELAX����#�C혙�����/=d4f�4���	6h2�#[%w��Q�2�F�Z���鉐0�^�g6�H���<�JI������Z�B.�A�0�w��������=�uRT�����]�M_���B*:�+�|vF#�,�T$�Q���ا�ܸ��i��t>�� �8�\���b�|̙LS	e3�p�\����I��2�#�HeV�j���XZ�I��H�+Lw��ǕL�w��@��p0"+�p��K��zT�g�'�K=�Bƃ���-V�ƶBw)0*�T���M��gx0y�$�#���,,q���FV֋ă̯~��G#�|<)f:F��y����3q��Raʄ�ė�8�N���W�=��Ƀ��X�,F��6��T�(��XG���]["W�����kI��ܞ�p�|m���J���>/͓�u	��v��.�a8�oҎJ���TD�WƁDJ~v��i���� ��$�!� h�(� 0
z� ��J� Hc,Ai�$ؓ��p�-{A>vv���
:gCH�����@����!�h�Z_���!���7�i0m����B�J��*�q>�w��xul�ꟷ��|�������/Z������Y4��T��hˇA+��6��8���gs?T�:�y�2� �I��Y\V4����z
����Xj��K0�}y�>̾�Ы�/o]C�S<l�F�y�og��tb�̥�Gs<%��r��t����2�4�-��\Ќ�'K"%�\�4�34�|`��f�����is�Z8	u�4���m���TeH}�4m�` ���ا��*����0ד������ILc���W�U�я$U(����s/�&(�ּ�ޑ��J=��ֹ��/�V����`�,�e�Fˊ�0t��m�i�!��l�ɞ�Ur�`��HD�n �>q���W
�Z�N����r^B�Q!A;fs�N7Q�wT��ɢ��64Mm�4w��!�ȸ9x����|h+��2�Ѡ���T��N3%v�n�뷥�o�3�x㨂 �Gr$5)J�͛5�.n@!���
%�J~���|2͆�^y'��(󐦹��0Z�é�h�x�9���_"��ć�+� �WX���I/�w3S�š�-=~�+��Föt���j��L��Rjl=9p.{\����(��A>�	Yg��!�V��P��R��o4�������I��tsM�p���#�
?�ٓ�s�lŕ�&�<غ�q~�X���V"&����Y�u��-D:��ʗN���P�+���`rU�k�"��C���[����)S�8s���i��T��8���c���9��n�'�F:UF�̕���9<�VCQ����#$�#(6e��Th�r��euA#sG�R�F_�3*�>���٧g�^�jw�Q�x��C!k���雏���� ϥ��uF8��K�	��Lx����;�=3�1��S��odZ�j|��uQr:��|�W���1��C&�D�����=�WC< ̈�¨#�&g��i���K� �U���|�7ҕF&�ጜyE>C�����*��7pV�v��	K�/�_o.0�����>�������і��ҏf�\�������b�V�2�d����>R{#������+��跽k�[�BE��j��C��Y7�f�Z��Na�XM�'筲)�$/n�x���{�G��N��5VFWm����������֐���MG���+ �Y��t�-�U�,�T;�*\4���p3Q�
���k�(켿��c�wurX��^е�Ձw���UD<얘��ӓH��6�o�^��,�A]	��s����~���l�}I��������),��N���P���P�}uN9y�����S��g��B٤@�2E��a��Y���4�������]���E-����J�K�"V�?�rp�\@9v�� ��L�/��ێ]Jl\9cM�7�pSn@�/�b��"э�2���|�z#���%�9��BMQ��Q@�����,mj�,��1#�T/�f�˔���p�+аL�>�	h���s�,�����f;����G�S&�pO���}�`XQ�4n�_��B�VZy���Ӈ�+NB���Y�\��3�^�"�|�AN�W	v�8;�`�d}W��r� 	�|B���N������Td�@cV���4�	�8����h�K��-f��
kW_�Ŗ2N��qd��Fl����=Fz\y��U(�(m���ډ���p��#ǧ0Ɉ��b�m̚I��<�!��xwߎ�%}�r��p2I��S�Kb���Pj��""<FJbf�czѹ/ԒWdL��
�s�����N��c�<B��y�>��J��p��S��p�Uw�iъ\BTӗ�3,������v��iEd ��0F!�g�gsw:�P썘��\�!1�Y��%�{Ʋ���}��6���&��
hK/t@�k���!9���@4���<Tҿ��$���B^�)&�"���	Ǻ���IC�o��u@j�
�`v�A���<��3L�zA1�M<�j�]�������o�s�J9\��0(�N� t�ޯT5�w"	��^|��7_��`�i)�-w���n�mO��s�T���c�a�Q����á:�����#�嗟���IqF�X�1�>��$=��cIr�D%bM��戛���Y~w֫�>��lS�`�6��Ѫq^��U6���+PW��$�������nhQa���[${��eo�|�2�rP��9s�QX|]$P����ˢ�&��\ꓕ'���Y�7��&E����H�߷,���~��4U"Zz�����΋�f��41��B?����3�2�T+�&sqUƖ��cHq��=��1�W�Kw[߻������a!;�w��̅4�0i���Ń���
ᦙ��4�(�D�6x��C�$����-XVc���#q�x��j~���l���n.ڝ��v#�aAv�Nh��u��u�,�b��2�Q	��;����0qSiᘲV?�iX�ч�����bΫn���!Fy���b9M��4�z��Y]p�ga�h^b�b��wս���0��<$�: �4RaR�q��/���I4�t�� c�Ibת���("ͺh�T3�K�,`�{\d#Z��0F0��-+>5ݿ\��
@���f�w��NI:�E\��<����B6i���@�BtT�2�?�b�N}:�H�9��@'c��1|t�3)�1�2�����Z~�  �zl8�Ѡ{t"��:���Q�|ث�m��a��\WH�]��u�[S�o�Z'&����|-R2<��.9�n��ƹ�S,�L�%�����{/�sM���P���>�M-��]�7F�;���Nl�$ Ә��m��]I/��/˕c�05�b&:�cE,��hOS⩪@��	R�ͻ��ꕖ��"OE$�a.vKfK&�|�����Y�)pg��Z�;.�i��<� oA����;����fC���\��,s�h@f�-ua�/����q����>=�mûnY�������P$�mw�\�չ�B#B d�}j�d�Ip(z��S�Z�����{]�q<��/3W�>[�C7��1�po�������`���՝����fQ�A� ���t�s�.FfGn8��ވ>9���@�ȿ/��Z�����ʺ�v�|8�E�	у:�v�M�c �~�����s#4�hFD'Q��viF���ޖ�⡫$_8�J�ͣ%��ʬ�{�}��w��]dywB� x���:����ȒGh��ɴO�������1�<Ո�G��z<�
[����.P �ed��~6D<�ĩ?��95=v[��Ja��HM���#��]+N�M��u���d?L�l���&�U�}�#q�_N\y\�eIXEM�ͤ[Om�ѴX��=/n��=��\JH �uj7��V u���$���OpCh0-�dT�R�p�0�f���vr����|n��N�(�^}m�R����Mr�a�����f3��"��YNp}�S���k�\�\�$����ޒmC灼�'	7P�g��l���>�'���C��	��D"@@˾ߞ>��G~��(�'�:�9p�i�B�c��z&��%�8 �d��h[�~����g�҄�/�,�������բ�ާa�e�^��e�_��XթN� '����Z���6=Y���6k�!���C��[a/Ɍ��G�TL�}n	�oP�C���#��R��������p���ı;g��D+���o�_�s�����[�G�«H�(h���_��|L��c�]�J��D<H���4a3�,Mr����,q������)Aa��G�b7b�s,��{�:s�'��ه\Ȭ����6K�f// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * cimax2.c
 *
 * CIMax2(R) SP2 driver in conjunction with NetUp Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "cimax2.h"
#include <media/dvb_ca_en50221.h>

/* Max transfer size done by I2C transfer functions */
#define MAX_XFER_SIZE  64

/**** Bit definitions for MC417_RWD and MC417_OEN registers  ***
  bits 31-16
+-----------+
| Reserved  |
+-----------+
  bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
+-------+-------+-------+-------+-------+-------+-------+-------+
|  WR#  |  RD#  |       |  ACK# |  ADHI |  ADLO |  CS1# |  CS0# |
+-------+-------+-------+-------+-------+-------+-------+-------+
 bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
+-------+-------+-------+-------+-------+-------+-------+-------+
|  DATA7|  DATA6|  DATA5|  DATA4|  DATA3|  DATA2|  DATA1|  DATA0|
+-------+-------+-------+-------+-------+-------+-------+-------+
***/
/* MC417 */
#define NETUP_DATA		0x000000ff
#define NETUP_WR		0x00008000
#define NETUP_RD		0x00004000
#define NETUP_ACK		0x00001000
#define NETUP_ADHI		0x00000800
#define NETUP_ADLO		0x00000400
#define NETUP_CS1		0x00000200
#define NETUP_CS0		0x00000100
#define NETUP_EN_ALL		0x00001000
#define NETUP_CTRL_OFF		(NETUP_CS1 | NETUP_CS0 | NETUP_WR | NETUP_RD)
#define NETUP_CI_CTL		0x04
#define NETUP_CI_RD		1

#define NETUP_IRQ_DETAM		0x1
#define NETUP_IRQ_IRQAM		0x4

static unsigned int ci_dbg;
module_param(ci_dbg, int, 0644);
MODULE_PARM_DESC(ci_dbg, "Enable CI debugging");

static unsigned int ci_irq_enable;
module_param(ci_irq_enable, int, 0644);
MODULE_PARM_DESC(ci_irq_enable, "Enable IRQ from CAM");

#define ci_dbg_print(fmt, args...) \
	do { \
		if (ci_dbg) \
			printk(KERN_DEBUG pr_fmt("%s: " fmt), \
			       __func__, ##args); \
	} while (0)

#define ci_irq_flags() (ci_irq_enable ? NETUP_IRQ_IRQAM : 0)

/* stores all private variables for communication with CI */
struct netup_ci_state {
	struct dvb_ca_en50221 ca;
	struct mutex ca_mutex;
	struct i2c_adapter *i2c_adap;
	u8 ci_i2c_addr;
	int status;
	struct work_struct work;
	void *priv;
	u8 current_irq_mode;
	int current_ci_flag;
	unsigned long next_status_checked_time;
};


static int netup_read_i2c(struct i2c_adapter *i2c_adap, u8 addr, u8 reg,
						u8 *buf, int len)
{
	int ret;
	struct i2c_msg msg[] = {
		{
			.addr	= addr,
			.flags	= 0,
			.buf	= &reg,
			.len	= 1
		}, {
			.addr	= addr,
			.flags	= I2C_M_RD,
			.buf	= buf,
			.len	= len
		}
	};

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		ci_dbg_print("%s: i2c read error, Reg = 0x%02x, Status = %d\n",
						__func__, reg, ret);

		return -1;
	}

	ci_dbg_print("%s: i2c read Addr=0x%04x, Reg = 0x%02x, data = %02x\n",
						__func__, addr, reg, buf[0]);

	return 0;
}

static int netup_write_i2c(struct i2c_adapter *i2c_adap, u8 addr, u8 reg,
						u8 *buf, int len)
{
	int ret;
	u8 buffer[MAX_XFER_SIZE];

	struct i2c_msg msg = {
		.addr	= addr,
		.flags	= 0,
		.buf	= &buffer[0],
		.len	= len + 1
	};

	if (1 + len > sizeof(buffer)) {
		pr_warn("%s: i2c wr reg=%04x: len=%d is too big!\n",
		       KBUILD_MODNAME, reg, len);
		return -EINVAL;
	}

	buffer[0] = reg;
	memcpy(&buffer[1], buf, len);

	ret = i2c_transfer(i2c_adap, &msg, 1);

	if (ret != 1) {
		ci_dbg_print("%s: i2c write error, Reg=[0x%02x], Status=%d\n",
						__func__, reg, ret);
		return -1;
	}

	return 0;
}

static int netup_ci_get_mem(struct cx23885_dev *dev)
{
	int mem;
	unsigned long timeout = jiffies + msecs_to_jiffies(1);

	for (;;) {
		mem = cx_read(MC417_RWD);
		if ((mem & NETUP_ACK) == 0)
			break;
		if (time_after(jiffies, timeout))
			break;
		udelay(1);
	}

	cx_set(MC417_RWD, NETUP_CTRL_OFF);

	return mem & 0xff;
}

static int netup_ci_op_cam(struct dvb_ca_en50221 *en50221, int slot,
				u8 flag, u8 read, int addr, u8 data)
{
	struct netup_ci_state *state = en50221->data;
	struct cx23885_tsport *port = state->priv;
	struct cx23885_dev *dev = port->dev;

	u8 store;
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
		0x00, /* int status read only */
		ci_irq_flags() | NETUP_IRQ_DETAM, /* DETAM, IRQAM unmasked */
		0x05, /* EXTINT=active-high, INT=push-pull */
		0x00, /* USCG1 */
		0x04, /* ack active low */
		0x00, /* LOCK = 0 */
		0x33, /* serial mode, rising in, rising out, MSB first*/
		0x31, /* synchronization */
	};
	int ret;

	ci_dbg_print("%s\n", __func__);
	state = kzalloc(sizeof(struct netup_ci_state), GFP_KERNEL);
	if (!state) {
		ci_dbg_print("%s: Unable create CI structure!\n", __func__);
		ret = -ENOMEM;
		goto err;
	}

	port->port_priv = state;

	switch (port->nr) {
	case 1:
		state->ci_i2c_addr = 0x40;
		break;
	case 2:
		state->ci_i2c_addr = 0x41;
		break;
	}

	state->i2c_adap = &port->dev->i2c_bus[0].i2c_adap;
	state->ca.owner = THIS_MODULE;
	state->ca.read_attribute_mem = netup_ci_read_attribute_mem;
	state->ca.write_attribute_mem = netup_ci_write_attribute_mem;
	state->ca.read_cam_control = netup_ci_read_cam_ctl;
	state->ca.write_cam_control = netup_ci_write_cam_ctl;
	state->ca.slot_reset = netup_ci_slot_reset;
	state->ca.slot_shutdown = netup_ci_slot_shutdown;
	state->ca.slot_ts_enable = netup_ci_slot_ts_ctl;
	state->ca.poll_slot_status = netup_poll_ci_slot_status;
	state->ca.data = state;
	state->priv = port;
	state->current_irq_mode = ci_irq_flags() | NETUP_IRQ_DETAM;

	ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0, &cimax_init[0], 34);
	/* lock registers */
	ret |= netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0x1f, &cimax_init[0x18], 1);
	/* power on slots */
	ret |= netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0x18, &cimax_init[0x18], 1);

	if (0 != ret)
		goto err;

	ret = dvb_ca_en50221_init(&port->frontends.adapter,
				   &state->ca,
				   /* flags */ 0,
				   /* n_slots */ 1);
	if (0 != ret)
		goto err;

	INIT_WORK(&state->work, netup_read_ci_status);
	schedule_work(&state->work);

	ci_dbg_print("%s: CI initialized!\n", __func__);

	return 0;
err:
	ci_dbg_print("%s: Cannot initialize CI: Error %d.\n", __func__, ret);
	kfree(state);
	return ret;
}

void netup_ci_exit(struct cx23885_tsport *port)
{
	struct netup_ci_state *state;

	if (NULL == port)
		return;

	state = (struct netup_ci_state *)port->port_priv;
	if (NULL == state)
		return;

	if (NULL == state->ca.data)
		return;

	dvb_ca_en50221_release(&state->ca);
	kfree(state);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |1(J�N��ʃ��`�k)�f������ז<�R���Ox��Vҏ���_� @�������#�Ӹa���$8
�?����|-;�f��x�d�9?�������>Z�k��;�
�%�����浓@)\�2�W��I��Y�'K��=<O�ߑ(��@6������/a��E_���7F�G������0_ܙ�6H���LU����E��R��-�Ȱ������{��Q�m��Z�å����K~?_��o�!t8[tܸMܝ���,%V�v��-Ef$�X9�AQ��\}��$�/LH?P�Y�I�Y�M����7�ZGS��^ir?�� �T����E
G)n�eq5��X�b�&A��6��͖�ɣHB�0E` �ն�k��y��W����@V��b�_ �w��qͱfn� �B��� �����pcq�%�?y�k4��7Q�)�����UuM��~L��Ų�Vz�m�%�^����e&v���B�xĹ���e��3���_A��F�َ;}J�<����)М:k'���`g�|v�|��I�4�?��M�ldd��}J4�?1X�ڄ��!ً8��W������o�g�½}���D�?iцbH�*�Oی�]h�z��"DR�|J	���Z��_��^�Ք&�i�͢
�;#Ds���{���`g���s-���-��@;v�1��l�,U�'�vp~��q�.��+>�G���L��|�>m�X#���=���IP]���4U����%��lh-�s���Wiq��	��Nr�q]A-4��� 2�z��`B�9p��g��y�~�e*����Tْm��B��a��7���[�n ���
{�����^/F��1't��*�7����'V͌)���������ۥ��#TG��"���H#���<t���M<��^�o�51ԃ����љQDݧ[�T���.�f�!Xbx�$.o:�\����z��@Fã�1�w��W���8u�Xq~�'#�~��ZԕL��x���wԩ��\Zh`K$uT�(���HP��3|b���Ī7DC`��윉����2������/h�"��K�=�W�I-bNWu���jx���D��/��q8F��\,DE�"t���>Ik;`|�<���� p�nw�w����	g(�@��+�(D:��. ��W�4}OF���n��Nv-.��b��V �L
c��p��tǬƄ���F��yf^Wg�A�:��w�Գ�0��>#23��q\��/wj��>��ގړIFG�:�t9�;K_�n��0-ҏc���+�}��3����X� ������B��m%gj��'߾���b���h(������q���?˥"M�t��D��������B���� G�7����q�f4�ӌ^�G�j�+����#�����
:-;��9����ۜhP���0hR�gӮ�$��仒�#�{������L�O�f�E��� "O��Ⱦ1�"0��=XZ�_�	�[ż�UC�I'���Yb�~2����T��;u�t��SE��m��6�����W���3_�A����.3�{Ye�/bF���2wd��*�����V���ѥ�������Y�k��� L���+�
e���\�gk:'�S�����X$��k�}�a?�#u�aj@L�]�E��, ��R�A�p��`i��cбA�\T�]�����%l��
�<�eȌ$:��J%���Ef��z��`:�G	��]� �+��	��(��1:�0E�v|�<n�]��߲9Y7ZϪG�*���^��A����&�!�X #v��RE#T_��.C}�\<Ym�{Ytr F��`��nƼ˫�	�O]���Ē͏�/��1ؗ�[���`�#�N���~(J�2��;�W=a�4oh�Aw��н#��#T�i�
�57�����r�c��}Q�Lo�#����R�tATˁq��v�K���uf�0������tJ�����-Lt� ���Wm�s&drn���>tc4.��PL%z�2�GO�N�V�R��^Di�o"���=?������<݋����aa� %;j��@��佧�
!��u�Ȱ� 6=@�[�s��Ҏ���睊��Դ�^tT)�gЬqJF:o��(�A����K���;%��d��K</��`�Z�6���+����C��l⭳����T rmн�[��s���
j09�A\���;huw��A�����7��@0ӌ����oo&1�h�j:-�>�~L��d��Jw�	�yF!�QN^����ϨGD^ت�hH���<�B	��e�+;f��Or�A p��p�[	�]
u���ˮ�=0�� �?����'�c��-��b�W�2C���>>J���h���ʸa���.M��sq}�E#0����^�����@q���S��5B��*r�e:��^��ʹ�iQ�!Ì����X`}�IN�.������ӧD'����t��[ ޯ��d���*��z��#��ί�l���E�=�lA�����n���Ɯ����⴯���O':��D�$!��~>���E�1X� �����<� �[w��d�S@kH4!{�{S�� ��Ǝ��`�//�GĮ���ǒ����;�팘g��To�l�7Э���w�p�!�, ���j�L���mK�l�Ч���X���j�s"89��6W����9oEd��x���2îHD�q�?�Y[k\�@�8�|�|����`�s�g�[����fI#*����l�$'�0�����FS���9�GËz���.ݰu�!���U_��oZߨc����{�R@FQ�I�w�Jv-e�9*�d�Q��Ҹ�a[���̷ iKo��K����6�Lؽ����¬f��j�C�����Q���1C�zi��]W�a_�Y6�r�}��Ƃ	��x�n3^ a7��s�����'�5��X�q0�Wi�]q_Ѵ��3���q؀��Dq4:,�~���f�ʁ�ud�h}N,{e��Z���gV�U����Q����	x�@#ۥdY��=�
�=��n���
�۷(@��ԕF�|���}���[x��*T��Dt�������m�5��y�M+��7����J> ���ޥHe�|c���LH��(̜��(��6-� ���T�$/wOP@���l\
;�'˾��H2	2�-���\Q�8��殇ẓ�`�U��� ����|F�I�I#.B��	$��,S������Mlʒ�>�-{�"�*����л�Q��:�U��"�1��󚌾��ě����B��!�j�r�d$���c��鉜�;x�-@_�ҭ�gY�5�A� �%���E��]T�y���uߕ���
��Fq=�O�rwЋA�^�t01sd.��b��\ �29,�xc��N˪us�O��-gk�?��zĚ�R,̵�ʗ�~7���zD��y�	��1Ƥ�b�e�e7��윤
Bɂ�K	�P��c��ﻘ1�P a�����*�x9�Bxϙ&����u�o\�N��[^j�ˣ��\a#�\���6Po�j���G<������)�U�������&����v�Yh����Z��ftA�l�`�Cd�w/I��E ��4�V_/T�F�N/��D����ï�w�J53�"D�k��x��e&����(���w:���&@5I�k�|���*�ɱD���q����I_�Uh�Yy�g@�\cT-�{���w\�i=օk��<,�a%��ŭ���l"��`M�c/k�� �R3���e���ēS��/�1Ȳ�ڧL�)"w8ӣ��դ���4GPDCO7bc�)��X�sX�&B�נfT慬 ���r�[�T��0��Po^� �g�~Kh(��hý�m�F�I�^V캻���5`�''�y��(��\�2��[�(�:ij��讦�[+�r7+hB�_q�A�;����(�8�Y�)8e��V�kK�V�y�����Sp���T5�X^�}��4p���B���؂w1�=���Ǯ��l��p����:*�?�@O�?	Pф�}��m�&i�"q���O	.��.�ż�e3�,65�;���^�%H�Bi�{s�@�a$C�q`��-�����$�HY�w*.F�K��eW�yO����$:�;t�Z]Q�:~�Uta��V�o�!?�0y^!Y����MkP2�:��ɽ�t�H(�}i����?y2֟���ji��/k����%����J�k� 1ޫiG�I�]Q��b3�oٶ��}��A� h�C�yA��Y�����T5�Ā�JĈhm*�����A�s����kj	D�,b��+`����X����(`�����'��C��{r2��X��A��%�.Evޟ.���X�2#��-TDt�8)"����Uиly5�l��pS_�8�,S�¡���vv�7)7.��r�:CR���k�~"WE���kj��n\D�g���Ԡ�}�蜹��u�*��!wB��C�&��{���N�O�N��6\�0�2[K�Ǌ�扌)o|��t����F������MF�<��yV�g�4�=ݼ�
�����$��O��f:UxH�,\6	i�؎�)j"����#G��'GҊ&$���>M�� רOR��������f��I�F���7N$�H�Z9�WP}h���p��L��({���Q��xr7�M=�~m�@x{pL3�Vrf���K��4&�D����/#��쒱�t��(�
�����7.|{�^?�n����Ρ�on�>s�
�O1u�,��m����B������#�^�C������H��پgW�z]�Rɽ)x���wQ���#l}<��v0���40����e��'95¡2���~T�h��m�1��1�#J~�"����°<�riv&�e^ݦ���%Nv�GӺ�׃�[vOi}��/-�o#��7j˜�t�i���⦑٭�\�b��Cv��%��a�c@L��T0���D�/�}�חg[���u�%�9\}�B��@D��~%�{���nǺ	?�2�ƠA)�nyQ�����<�Ԝ�L��y���E}�%ԩ�?�(������� ��.јaoq	E��io ���f�+|�@�]"j�(c ��Pg�C�����:�a�dD��f���E��gP�ٜ�2���6��G�k�+W����)S�f�$�1��mᭇ\zc*��^�D�|��7���.���%��Z"�Q���� �&���.�,���b��D�d%�E�	m3��t��'�>��~G�󁌱Trw�(9[��C� ������l���<t3�2{�q�ͥvk�}�x�r���,�(����M�<�^k����	U��L���C����r�6W`��{��";���X�]�6#�u��U`3M��mT�֩1(�i�@���+n�|M<@.�\pS�E��@\9�>U6��&����aS0�Q���&I��#��yN�+[>�� ����.��N��ig���J���ރ���n��v��G:.&1ۉo�h �$6���}�3�ߪuA�Ɓ�!�S�=�P�E�E�f�q�7G�L�8/��u���NC-D֐z��d(��)�]�����Ȣf_���cΒ�/�d��n�^�R �3Ǚd(�I'�@�5<U˦�q�e�da���G�Z�ҦN�J��[�b2t����A��Xـ��(K8�tH���W��s�r�?�3k����z��n���J�[�w��Tz>�ua7{Q�GC�A�N�I0Sua�}�`�V�e�%��X�!fX%����r%#.$�RB�M���['{ 5XR�m���r:0�ko��<�_��5I�V�z�� ������vP����8J�{p����=��p�<��&��}J$c�'�-E�I�:^�g h�;2��=.���hc%�6�r�n�jp�����0�� * 9�'�?�E�J}���h���z���S	ܡ����d
�P��r0���3�q�T�k3�l/�������KQ��bw��=�L�v��=qC�b����zD�HsT�K��i|.��og��J��y�e^��r���$�{c�9<З������FW�xS8z�%EJǔ�lP�e"�;��8�����u\c _d U�Eƈ�J�I��u[�2�N����8m4����8�[�]��T]CP�:����#q����Y^y���z��dض��-)����}�[��I��&��%Q��.�b��f��B{�-���Z��Q�uL�<�����B'���5]��z�?X��R�}�
�s�KĞ��4��ܬ����j��=���+�-	���r�B�`.b����q�ALU�t|�N��%7�������d��SF�!�}E���^��_�����P'��.��f�Jy�D~��/��.�m��	-J�"Di��36=V�*#�U�	'-��ˆYY�W!��(_$m�)�-����f[���5/��;t;Œa���S�p'h��.�j��{1v���, �8��vU���ڳw���.Ο?��f�w�1V
M#a6
�"˥@R�1-%�%Pl�KWК���m����_!8�֪��$�Xդ��Kp]A<ו3 =NS�f{�!a����f��z�`K�[�Ǵ.QՂ4�m@y�L)�l�H� �#L(1dL��G`�l�D{bc���� цo���X$�#���ʓG��-��Weg�"`�×��.���p6��M��槒��k���(�UQ�ӒK�fۿ���޳��w����{��?��t$�a_}DH��k�5����P�j��8���0thIKhjI�Ү������pԁ���;��嶎躗Qҳ�:�p��T�\V$�l(F�)�1�^99�M�X�$$�Z-����.f�߾>��uޯSj]��vH��t��Y�]�.9�?��Л��j�3�ZPL</02���EsP�����h�N��L!ի��K��d�t���C��X�&k�7˭��)>�&q�qh 2ĳ��PD�|!�r�pd�e��i�ڍ����4m.Y�1������;����ƾt1�(�!�;Z�ZM�.\��P�BZ�*�HS���N�0ð�����l�����V� eV��+ŋ�^��!{9��@�l�7֔�9�2Px����i�L��[6H}}�c���2R���5���{�d@�B{[5V!ƪa�俕lX7�S[�i��G�Ÿ�Lg^Pi���qZ`h��T��!��s�Z��-��j��Vs�Msk���7tZ��Nv�����t�|c"��Wɓp�8�zPע@ٴ#6 ���V~��k#l�����/����/(�W%�$J+�2�+%rfw	�"�aP��9������[��C������H�}H`j����#tY����f��|>nR �ۧWXt��*d����(�P�^]cܥ|5�Pb�ޜbKl�I^h��N�J�_�r�OiU\@��ьis�-/۶���k4l����i�vf@�Sn;��&���
�3��=ɥ}�9q��P���к��R-%3ڋ�� T*mC���T�����p�`Q����2v�'FE�ζ$��F�x��Ū���7k��s�!���R7��{� �e�Q�i�:�2(��D��С��0�)w���MO+�!��^���2�)-�����
��m5�8��^JL�G�y�S@��8'nU>��ˊY�6t�lA��������9��0��z(6ʤU$�}�7B���9���elB:7e�}(��[��p���Y��J�-Ϟ�z��#�h�~gٷ���U�om�Z�
��+y�����f��T���i�HK�F7����CO��r
�>D}$^S�&'
MN�Y��.Z�H<E(e#�t��W��Z�H��eB[��ovkڢ����i��
ꗬY�+b��J��g�� 5���?p%Xpd��F�ۋ��N�=��R���4;�S���Ew�;��	�03�{�Ǩ����z�ˑ'n����� 翉
�����?���&� @DLs�h�tk�q\�N��.���9�˝7*�s��b[;���cj���ABa̻�����_�ļ�5�d`_��_1��)�|v%���9��w:F@!q"Ǿ7�����_ :T��c�d
�+F�_/ֻ�h�]�&��C�v�8C������_!H�$5�i���}�J�C�ӵ�Y�l�Ը�>xF2�!��K�o)���)���7N����HPxY�Q�^���-v���W{�5�8�)W��BdJI�,M��?��A���T�1�i��Ft���}AD�*oF�JirC'�4ɳ���� �~N�O��rUu��@���oYk�P����D�2���P��_�� A,y� 2�SOJ�{]�C0����*U��E���o6�*�\F�1.��\�Sy\�s2;4��J��m��#�#����O�h?G[���oT-����ᬢ��G.�#Oiȡ��r��NNb�(n���.��0�AB&t	���C~��se�_��-d�\��O��	 ���� ��(7(>z�#."����#/;��<>�QhV{��WVi�� hqd�M�G�����=L���{�d �H�-� ��l�խQQ+��-x�2����1�	#��>.7�{:}/a�IN�+�<%I"�bbt��3=�<�jE!����Ca�W�\�E�U:<񳚌X��P���͟�7O9�jʨ�!ʎϙ#����}���@�l7i�M/�쁫����2�d�"��*�R���vN%���$ omآ��T$�f� ���O�������h�G�I�A�l1�OQ�~G�wV=Z���.>��0�� �O`OT� �9����6�����2����y;]��Qx�^���t-Ȼ|k���\����x-f��U5y�E`�5̧R]Ւ��zn;�|��� � 9�&Km���͒�]��1/�7�m�6�}ۢԍ��-�8�n-�xF�Y����#�ڌs��W4��%�_<��{ink��s����N˕�~�p%�(�h\I�����,\Nu<wb�_\��&~��>&�/n�zh|��?H�ظP(%�NOd��Y��8��H����8Y�`�=�O��CU�w�:�Hq��������na���(���t��1���vY��p���x�w��l�G�*}��T�z�6�P��^2E��fӶ�(=��pe��V�ĝ�9�r�(7���(��,���%��X�5�B��$�c՞r"/	���?+��; 'I�	V��V����( H�4�� 5����4`|�(��i>��_�����SYH"�H4�:h�0�p�7�>ڃN�~����+y�n���u�ʞ�SK/�q)W���8�ϱ��?��`Zp�Y+7�l(����c0�&�[y�������bRJ�-K�y��2���S;R������ڏW�~ñ�mY�o��:�|ҳ�k�
ٶ�!e���c���/]�Э^l)�gȇ��.�������)�|�q ���v�����r("�5�`	(������Y要�u�z�}6�-�\:�"���7 Ρ4qig�B���Le�|@�@�:#��� ��6���c[�R������O � �d�)�^]w�*�(^;o�G�ӭw���T����"����b;FLZ���X����I�ʝ�9���h�'�C��k[F��O�)���X^B����n��D��OYS:��Y&~�)�6,6����P�~�LTrC��[{�NF8���P���-�u:N��B��H\
A��ŧ�FJ�ׇ)׬�P����"�ό�r�lM��m�����X\5}����Fx���@�����x[�:P<a�j6%
i�(�D�U�a*?k�aZ������쟮��VƋ��Yz����?33,�z2�Ǒ�y�����>�-]*=�������GP�a�Q:��������@-8��p��j�[��{���T�oE�F@#��)Ғ�G�;���UΏ�g&��C����?q�����p�����$mؠ�T&Fu�<*¨P[6ίc�҈�&���P�m��/ƛ�Y����t�����`2�O�G�=�*t�&Q���x�ݿP(ǀ��	5N�J��npd����5gq�(z>I44���Ώv�"˞?�j��,=Q���]���/uSD> i�����k�o���J+_#&��Lu���^����b\U�m���^��j#a>�eԵVu�܆�n���!j��@"�o lvW��݀w˼z,u��6jϻ)9�)��$�8��@W��mr�x��M��ׁ��F�M��7���I�9�[��~�Xq(��1�7��s
�~��1}	k�|����	!�y�n�$y��E� &]��-��G3weC���A�k¿,mC�Y �ܞ�d��Fٔb��[`�2aٸ�#�a^9[<4*'-(��m�����x�����Ml%uI�����ξrྃA؀��RJ���!��B�DF5��
���59�v�3Pyg��,�	@"4�h�So�.dw�/���e+}s��Ii��
j�L����Ia��_��G#��s��44 �*�>21_� ���_g�Nvh���jE�x��.\.�NZ�g�ˋi���y�9�I�Z[G;�.��I)��=ߚ��W�gq���b�ʙEw��#hU�7hq�nO�:���^����_[
�m�mҳ[�Vs��^�͉=#?�u˓C�a�{l�|p�]�`*T�L]!� ��"Q=\�s�T���N8Oa��ڋ,O��	�^�<#{o^j5�&��i�a�@	ʡ���0tZ}-��;=/ŚLY`���*Bv@�@�a�~��E���d�/���������_TB �6��tj��c�ƶ�S�l�R��t�,�vs�8����BыwH�l��� ���Fl�<�����$
�ˊ��BT!"SŎ�ӫu�ȫ��Q˰�OT8�ѓ�6s{D�a��P���c3
�D�c.�<���o��X9gLm�_v��&�S<�X2� -�D��LOX�T�b?�"�9��{A�뀑u2�q44�
�����Ё�<�t���U.Y�~�P�l-@��ɂ�q )0���g�E������L�;&4�nүU������3tP�:�A���<
�6&]&�]d2,��J���&+��@�&(��w�J�#�BD�A�ʖ/�[��Y����p+Z�D	L(?g������[W��dL��r�$� #� n��:'���S�-�U#{��Q�����g/���LV�vܪ�	�2��%tlҭ�,�%�@H4�r#�ز���[�ҏ�-/j�vV�2�Pn'��;�%H�>MQ3�@���:)>���}1u�;�U���:缙�u�F�FYG��h���i$>_w�Y���-�=@᩷�cƕYxA���l�L�j@�"^6��0<�7Vq��W�b d��$��e6�ji�Мp (-�8o����m�x�k�������	��N�W&#rΉ�\y$�!5NJrn�Z�(�LִݫJ$Q�BB�E7ǃ3�%]Բ�\���#1.Ќϯڏ|̠���$k>�\�&M�A,��QJba��ƾz���q�����1?��A�'X���㍤�\��gE�����5li��R��<A�2� ���9��E(ѨQ����:;.�ߟb߾��� ���Xȩ��@�'�y�}�ܒ�e�ʇ\�3�Cl��9��L���� �[��f��~�� 	��V��僔L�����0���{+rh{��(��'1$eĳݝ�Q3��M�m�{!�"���r��<����v�����_�z�A���\��A�9�[<�EsMd�z�l�yݏZ�G�L��Kn�5�.�[��sؾ�~┟�b��4����,1_�1�']&F�	��θ�����_V��_��l��Ի��f%�t`_�|�Cy����k��\���	�tE.����a��ձ͍*�'˼Z_�9X��r��hn�
S7]�6ۣ��#xI	g�s~hMUp���rj�)�9G�����v8=�_���J�>����R��&� 0��/�,.r��N�mМʬNc�5���,]�����`�j��Q5�j����b� ��/ъJ4�+����{hQ�Ȳ���3��[�N� ~G���gg�v	�:?�wK^���	.I��Y���$.%�w���w��X��v�fjG�Y�#<�?�8�9{,�يv�z�#�D���܍�@{���j��(�h��zq�|d������w�=B#��KU'ohAx��|y��j��'�+ӆ~t�!7��5X)vK������υ�R3���T<�X���9�v<�`&Ǩ��[�Rn�v\5�v���/Guc����~��шi\pHH���U���T,����<��9+\h��9�q,���~���w�ˠg��E�t��e��s�:k�W)��P�z���f4v��G��6����"��K5��%�����W�v�����J۝ެl�)o�B犸:�>N��@4u�g�/%,���F}�F�ȹcϳZ��g�)��H.���ۢ
�q�
b�����d��Rz�?kL�\�A3�U�p)�;%�3Ο�7�[JPK��@�w�
��3�/�q8lU�@�!�@�߬��˖�=\J�-h���o`��:���m��9����&����<+?M�S�[Z9!j�����e��L�׹��K$�� ��^G�
l1;ݡ-����/�)��+�C?���񛿾��6�e��7*j��������]�d�"#�����B��.���Y⎐��$���g���4Is�x� u�s��!�4Kq��D�~�x��>�@�����C �a,�|�ͺi�HxJ�F�{�AS�����2eo�hb���J��hs�5k�^M�wu�2-x�k��z,I�S보N��=��9�+�×�}h��b�\ؓ��Q=�v�9��[�8U�M$�7y�����Pz/� ���>M��P'�ق�q���z�pOۛ��h넂y��10�i��M��h���htna$��k�r��p����O
�u����i$
˨�:P�T�h��I��K_�,��X4)�fp�+Hsv��M��O|��m&���)(X���V�Q5�ʣ��qq��3 �/�+�7�D�ñsvİ�awn���?L������r�5o��ϔ�`�#O�Ch,�u��=@O�����K1|���J�pFm�$_!����}��M���m�h﷏�K�S)S*-�
["���
^���j+��hS��q���d˃ ��#f��8ᯧ�}��龷rlo�ե����.��m��$:��x��q�-gƘ��Ǔ�9:h>ʼ9>��LE�ZO]�� 3i���X56F�z;TH�����O��~�oɭcM?j��咲�����!�?>M����8����~���(~\���.7����^���]�P��C�
O�^}v����l�Y�<�Zd�,�r9�!�:�aS}��/W{A���:�~˗���qr��Zy���}��[� �ɞ�<���p"��yn�QU��2!��o}��-�uH��7�r��0L�NZ�#�§�	K�b�i$[��Q��e�3$�l�z�uj�b �&+�s��FNZfDn�+��~s>���*e��#�|�6ms�d�RK��2&��߫=���g�#���k��<nO6����fM���3���AA��o�#;ʕ���-.+�5P�T�Vױ��ꈶC�M�=g�<��޵)f��Eci�dʌ��g���<["��,�8�n/槃r��AJ�"����4��*~�a�E��cHt:Զ���U��!�²���T�8])���B�kp��8&U���H�6�X�s<sݿ�dn�m�r�/����ə������W;bPX2���;�,B��&g��:g-���c�N(� ��X�6��7,Qd��}�1����a�
'���%׫=�Gț�m˷�.-�ǝ�^i}�R%IeM/R?��|��<��$
��E'�K�	(2n��[�jAi�Ygrw�kP�R�V�CqL�Kz�z�x$�G�H�I'����)��Q��� S�M��g���[`�|5���� '�(�x�7��v�{vT��!��s�n8��%��rn:��ۼ��q�5�(�c��D�^�s��q�����ס��S
n���T����g�k��7�V���}Z�Y���S�F��nTC�����;��t�Lr�9��������
�UxE֛w�\$�;�?)���>�yW���~s��oz籟u0!�?
@'�3Yl3�e���Д��잀<� ��M�Q�I�e�.փ�ԓ��Җ�a��\1�S~Cj��
���ֹ�!E�����)�$7�N��^QI����E	倶�msvr��كX�8f$�A<|��v����z#(��rY�uR�י��3SM�s�N�O.��H��MBު�Ln˨B�|��E�+�N'N�p�����R�izwH��o����v����[�EZ�x�Vu}���A=׶�������':~��{�n�k` �?pL2���%�7K0��\5\:G����B��?�p� �t"d�I�~��Q��VM-�]��'�'����I#L��,��SY��f	)�":������T�#�A��lp2��;`9I��^T*�����87��a�Vc�	�)��Z8���Q��i}�IXʷ�#�Ѣ�)���pn���Qq�t��uk�MU~�`����{�3�1���s���z@`�$.��� �,!D��qo�:y��Ȕ�������,l<}�8��b�$羞B7�0M9)��16�;A�^��n�x��l��PK�h{h!�s�w�W�(n��-2�P�@�,�p�J_��9�n���n:d�P*�5�oJ�Y�f5N��u���f�*:�M���U�����CB8[��Ĭ�k�8og�Gg��JD�h�ҽ�jwT�<?�{��2f��1��Ł�R�	�8F��Va�w���D��C����A�۵:ej��3�cބ��N<�T��J���r��U8wQ���0V�=��0g�L����X�|�����L�T�r�C��;�C��3b��}g>��K�Ò�0��aƢ27�A�l?s���L|R� bQ �͚6���-��̞�%(��\(?�p�Z0[0��ϓ��#td�n�r�N�}u_���O�	�YS-�K::����~�\C���|�_YML)���.ļ�!/��2��"͈*�K ��,zM�֦�|����!�C�����,�!���[�؀�+�	۶�>���mޭlJ8��o���4��I�'�)'=;8.��bxM�$���B?gW��4��v���ǝM����>0�~A �����3z��L��Ďm8�6����%�e\o���8�.ts5ٔ�7Qʸ52�|K5R��
�A��%#W8~v�l��h{Հ)���Dl��o �:�(ʕV����3rd�j#��<W��+�'j�Y�!�h� �$�i%��2
wwd�ެNd䙎�wW$�3`�b�nZ�.l���EŪ
{��T��E�9�A	v��g�'M0��86���mi�!k�� ���v�b�P%A ޗ���h�hЗӢ��'NI&Q����U��mb@���z �R�Bo~yy�d�������) .�{��+�d.a�A���-�(�#I���ǰ~�.�x'�!����q����A��C�'�γhU
)JP�Ae��_Ћ����cÛ5m�1�G]c ���������Oϝts����VT/���f�Ev���]�g�ې��\��I��5����z��qM(�UYX�����ǎh4�'�B����ݬ���]8�mK��b2*��K�Gڳ�$ �sK�'�ͻ[ޅ�k@A���b ����1�[DX%C�J�b���.�ɞ�P���h��癑�IC@�Ef� C
%D�E/:�a���)��ϒ:�����	�O��I�0O�3p�e��N�8-�͸��|)c��.�� g/��h�eg�]Z�>���5�2�'����5h*���T�d�آ#���;j��`�f>p��b����*q��~p���P*/�}��x�c��S>׼��Ns�"wRHh4�:2���A]�^ȇ�V�R��[m��b`�v�+���,�2$��Q	�\�p돻w�{-��6ds�K�m(�9�3�*�;��JK4i� �5m�8�C	��BNF�O'12��Icw-\��bG��D>[TC2+x�-Ӓ�t����/�}�����%���	�w����	^�ba�Ϟ�h����N�R:�\AϢ�΄M�e
��|5M��'̒�h�A��ԡ��q�g��?��N0�5x��`����r���9@��oJ�[uC��7D�%�~�3֯�ib㔇m���cԶ&A��z?�v�)dc���p7�*yTy�C����,���nu�z���|9���b��H{ui�������A����D���6��|z�8������^U%]P�ܐHk?SXP�v����|;�M��tI��i�$|ɂG�7� ��Re�5�J�T�2��L `𢎟h3-�����ó��T^m�}mu7T�Ta�W�B�䳣Ѹh~���Х,ǲ�Tj���S��k=�z���5�,�)[4�X����d�_���tD4�*E"�L|�'����E�J���v�G��/�7RR@�v�a���{�al���TXp�ò1�|æo�ʝ����bv���nZS�>�>�CV�ahǃ�jq�>����u�N�! f�q�My��Me�@�&�������t�>B ����r���>c��+P N���!^����LJ�D�,y�Ď����D�Vkr�],�3�<v'�۞��`�^mJ.!��ͧ:6�-uն�� ���N��Np����ޕ����n���3}��pV�Ts8��d=^.�AD�ص�inS?�����ꁭ�m�m�?��+�;�fh�ɪ�7�R��8�û>��:;(1�5e�bB��lkm�X�0��=i��:����`!�1�,�.�R�ɓ2q�ȳB������#�][N�O~�aG��~M)��?�V�v6���T��l�?">�C�E~\��:���P?��͋�^�����V�����kC��x�kB�z�� � U�(�j1/ lR�*�_�x�^���Ô��[�� iڵxCd�_<��X��
������*9ѕ6&�"��I_"�B�i��;dc�ŵ���C�͛���C�
8���]�z%q���Te����柦C�p����J�
m�m�贎���O�T}o���u�1� l�� �"�X�/=����ŅҢaV�}�j����b5	~2iB�G@�rqǺ[��%���͝�����ڻ 1�_��Ƹ��<50��������T�zs��L_����KA�4� H��vT��E���� (�c��f�K4X8��P%4Ď�����k˙H�+�_�7�&K�x��z��pg+Y����b�jת��/T������QM�������v)-kbb;FpUÓ��_���!q���7˓�'�������}5Bĸ�{�_,���?/G>��)�gW�{��$�Waخ:l�q/ӻ3�<����؈�G
ж��E7����
`���6w����
i����1^W��
كp-Kr�p��ɮ9��40\�0�`�:�9o`WH�'>�9�ϵ"�5q[���6�7��!b�/}�	���v�=��de1����H�g	lgC����w�#ُ�{�+��S�,��S3��T���� :�q�jx��K]��0/E�.��f�6,�o$}/⹺��U������������Cy�p-V�$+�3s����&j���Iٍo�zK��"k��db�5(3�G	7O��zS7G�V�
q�Q�U�#�oR-�	ŠQ�%)�y]4o�A	w.�����I�r_N�~��8��fT�{�=B�6���c��M�G͛��d]��m�yH]�/QN��-�㈜ڝL�{�se�[����)0|�̃��L����p��s�e�/��Ml�n�S3p֟���g�!T!,w�����&EQ�+�}��R�fǷx��14�Tx�6|I�mf�S1
@��kb�W�Ux��z�P��x��K��+��v~�*��xj:<��a	�\8d�?����M;)ٶ���]�a��d�����C�$���1�F)M8�ym�h"�Q�W1=aʷ?+X����m��*b��S�c�g�W+ed-dܳ�A�z$N$������3�܅����a�f���C4��k�	R�����<N~[�ՅuU����2A/U�����6�:�UU{ �
�W� ��_��@���I���ar{�o*9��>˖q���Vv��ìxݤYY�:m��|��
�?���O���v�
f��t;>I�f��	/y����r��4���8̕$�h��Ҙ/E3%���d�@(e$��I���E4G 4=��V�uҜ$С~�u�qބ����6����Tu�<�R�D_r�.�g]�o;嚗�Y,�`b����q5>#
sb���p�&E���ϽvZ�2^â����>X��<�2��*yM�PI;#������:������V��\E�y*xCj �i�(P�eX;��ϚSi *{�c��� h��X�x�Eh�Qe��'��8�=tF� 0b3B�ǎ丩��/���>�Z�?���ܦ�=�L)֗x�� *����	�?��{�+[�����6�}������ �*�f�~#���7m���|��	R�g��F�_�;�e)=�G��4<��# 1�8�M=.�\8���1�:���|���U+V�*��ŮӛP7=p�^d5:֓aS�as�p�q^d�?�KT�&t:��� �k�����:냂>�j��2{���C��8e�`9�:�#D|�a)�5����uu�M�����Ƽ�[�E�њ;�&}y5��QAE�#1=:pܓP	�C��K�f��b�/� �e�s��fH����#��������Y�s5���od'���+�.��#�h��k�{U/�+�w�2���%�+n���,�Q��~5�$����(R�>+%t�%,�;�,�4�_�g����t��= H��%�������|�$ݭ�H�t������ %%y�v�* �!}����KZ�w������(���t�ǹ���*ŋn��q�#,�o���61�6��:�3��{��`�m���<U�`N%�ok����߆�SL��/�� ��eU��h���&Y�>�^���K�_)�(|MX�䌶�캜��3D��G���{��=�/R~FND����%���ハ����{
�	�x��N�qM]������7+vA���m��n�5�h�G$���S�ϸ��])��n&�?��JF�t�~�j_d��#2�0�n���1��<�k��v~m��q\�XșG��	!a��r�Z�����"AJ�yS�j���3����@��u�4�i��S|�JPWBԑWf%	�>i��48V'��O͞�x��yS�r��|�[�P����2����w9e{膲�'�+��o����u���jd˄��-�h�T�1�S}��f��B�M�e�U<v�gS��0�ժ;Rp�}�~-��L�*�a0��.5!��L_���m�n沉)�fjjX/7uFi	kӶ�t	
����Ο��4�Gpl�]=�d;��[`�`��M,��|	,��X~�&�2�]d2�#�f�h) �r<%���7�ۮ,��ҫ��?�;�m��e�,N�� }Y(�N�	��Z`��7YU���i`�$��zǙ���'&(�2�x,�Z>,:P��*S|�T4r�`�8�A���.y�6�!�{P:v񜅽MIW��L�Q6�b���=i����W�{�c=�E��2<����D��䪳�x�hP޸Gjx_�ݭd��.b>�@��@
�G 8����5}t���w���R{�@��tɕH&��~,� Fo�>ʢ�,��:��Z��?�ES]��q٥�B0UOo�4L�~é�DG����VX�ltt"!�s�Y�Na�:V	�{�q;���Uw)��������z�߸c�,(���LYT�q�����"?a����0�����D���@�3��.C��4���H��c3j�U� ?�=�Z�b `��d6M�l..3a0%Jmд��y�v��3(�NA�ŉ�IZ�Ԩ��r������5��1WV�Mm�  R�&Pϔ �����  �P��;0    YZ        description=Echo the kernel header artifacts used to build the kernel author=Joel Fernandes license=GPL v2 depends= retpoline=Y intree=Y name=kheaders vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII  kheaders.tar.xz           GNU  �       �                GNU ])u)U�oǟ�C	a����        Linux                Linux                                   $                                                                      kheaders                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ��       #            #     *             8            J       F    
 d   F       
 y   \       
 �            ���   k   	    
 �   t       
 �   �   	    
 �   �       
 �   �   7    
                                 )             ?           N             U             a             l      #     x            �  0?;       �              kheaders.c ikheaders_read ikheaders_init kheaders_attr ikheaders_cleanup __UNIQUE_ID_description96 __UNIQUE_ID_author95 __UNIQUE_ID_license94 kheaders.mod.c __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module sysfs_create_bin_file cleanup_module memcpy kernel_kobj __fentry__ init_module kernel_headers_data kernel_headers_data_end sysfs_remove_bin_file      &     (     $     &     )          (          %     "     %          *                        �   '  �  #   .symtab .strtab .shstrtab .rel.text .rel.init.text .rel.exit.text .rodata .rel__mcount_loc .modinfo .rodata.str1.1 .note.gnu.property .note.gnu.build-id .note.Linux .data .rel.data..ro_after_init .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   #                     	   @       H;                )             c   #                  %   	   @       ,H; 8               8             �                     4   	   @       dH;                C             �   0?;                O             �?;                   K   	   @       |H;                \             �?; �                  e      2       �@;                  t             �@; (                  �             �@; $                  �             �@; 0                  �             (A;                    �             @A;                     �   	   @       �H;                �             �A;            @       �   	   @       �H;                �             �C;                    �      0       �C; &                 �              �C;                                  �C; �     !         	              XF; �                               �H;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *  Support for a cx23417 mpeg encoder via cx23885 host port.
 *
 *    (c) 2004 Jelle Foks <jelle@foks.us>
 *    (c) 2004 Gerd Knorr <kraxel@bytesex.org>
 *    (c) 2008 Steven Toth <stoth@linuxtv.org>
 *      - CX23885/7/8 support
 *
 *  Includes parts from the ivtv driver <http://sourceforge.net/projects/ivtv/>
 */

#include "cx23885.h"
#include "cx23885-ioctl.h"

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/slab.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/drv-intf/cx2341x.h>

#define CX23885_FIRM_IMAGE_SIZE 376836
#define CX23885_FIRM_IMAGE_NAME "v4l-cx23885-enc.fw"

static unsigned int mpegbufs = 32;
module_param(mpegbufs, int, 0644);
MODULE_PARM_DESC(mpegbufs, "number of mpeg buffers, range 2-32");
static unsigned int mpeglines = 32;
module_param(mpeglines, int, 0644);
MODULE_PARM_DESC(mpeglines, "number of lines in an MPEG buffer, range 2-32");
static unsigned int mpeglinesize = 512;
module_param(mpeglinesize, int, 0644);
MODULE_PARM_DESC(mpeglinesize,
	"number of bytes in each line of an MPEG buffer, range 512-1024");

static unsigned int v4l_debug;
module_param(v4l_debug, int, 0644);
MODULE_PARM_DESC(v4l_debug, "enable V4L debug messages");

#define dprintk(level, fmt, arg...)\
	do { if (v4l_debug >= level) \
		printk(KERN_DEBUG pr_fmt("%s: 417:" fmt), \
			__func__, ##arg); \
	} while (0)

static struct cx23885_tvnorm cx23885_tvnorms[] = {
	{
		.name      = "NTSC-M",
		.id        = V4L2_STD_NTSC_M,
	}, {
		.name      = "NTSC-JP",
		.id        = V4L2_STD_NTSC_M_JP,
	}, {
		.name      = "PAL-BG",
		.id        = V4L2_STD_PAL_BG,
	}, {
		.name      = "PAL-DK",
		.id        = V4L2_STD_PAL_DK,
	}, {
		.name      = "PAL-I",
		.id        = V4L2_STD_PAL_I,
	}, {
		.name      = "PAL-M",
		.id        = V4L2_STD_PAL_M,
	}, {
		.name      = "PAL-N",
		.id        = V4L2_STD_PAL_N,
	}, {
		.name      = "PAL-Nc",
		.id        = V4L2_STD_PAL_Nc,
	}, {
		.name      = "PAL-60",
		.id        = V4L2_STD_PAL_60,
	}, {
		.name      = "SECAM-L",
		.id        = V4L2_STD_SECAM_L,
	}, {
		.name      = "SECAM-DK",
		.id        = V4L2_STD_SECAM_DK,
	}
};

/* ------------------------------------------------------------------ */
enum cx23885_capture_type {
	CX23885_MPEG_CAPTURE,
	CX23885_RAW_CAPTURE,
	CX23885_RAW_PASSTHRU_CAPTURE
};
enum cx23885_capture_bits {
	CX23885_RAW_BITS_NONE             = 0x00,
	CX23885_RAW_BITS_YUV_CAPTURE      = 0x01,
	CX23885_RAW_BITS_PCM_CAPTURE      = 0x02,
	CX23885_RAW_BITS_VBI_CAPTURE      = 0x04,
	CX23885_RAW_BITS_PASSTHRU_CAPTURE = 0x08,
	CX23885_RAW_BITS_TO_HOST_CAPTURE  = 0x10
};
enum cx23885_capture_end {
	CX23885_END_AT_GOP, /* stop at the end of gop, generate irq */
	CX23885_END_NOW, /* stop immediately, no irq */
};
enum cx23885_framerate {
	CX23885_FRAMERATE_NTSC_30, /* NTSC: 30fps */
	CX23885_FRAMERATE_PAL_25   /* PAL: 25fps */
};
enum cx23885_stream_port {
	CX23885_OUTPUT_PORT_MEMORY,
	CX23885_OUTPUT_PORT_STREAMING,
	CX23885_OUTPUT_PORT_SERIAL
};
enum cx23885_data_xfer_status {
	CX23885_MORE_BUFFERS_FOLLOW,
	CX23885_LAST_BUFFER,
};
enum cx23885_picture_mask {
	CX23885_PICTURE_MASK_NONE,
	CX23885_PICTURE_MASK_I_FRAMES,
	CX23885_PICTURE_MASK_I_P_FRAMES = 0x3,
	CX23885_PICTURE_MASK_ALL_FRAMES = 0x7,
};
enum cx23885_vbi_mode_bits {
	CX23885_VBI_BITS_SLICED,
	CX23885_VBI_BITS_RAW,
};
enum cx23885_vbi_insertion_bits {
	CX23885_VBI_BITS_INSERT_IN_XTENSION_USR_DATA,
	CX23885_VBI_BITS_INSERT_IN_PRIVATE_PACKETS = 0x1 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM = 0x2 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_USR_DATA = 0x4 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_PRV_DATA = 0x5 << 1,
};
enum cx23885_dma_unit {
	CX23885_DMA_BYTES,
	CX23885_DMA_FRAMES,
};
enum cx23885_dma_transfer_status_bits {
	CX23885_DMA_TRANSFER_BITS_DONE = 0x01,
	CX23885_DMA_TRANSFER_BITS_ERROR = 0x04,
	CX23885_DMA_TRANSFER_BITS_LL_ERROR = 0x10,
};
enum cx23885_pause {
	CX23885_PAUSE_ENCODING,
	CX23885_RESUME_ENCODING,
};
enum cx23885_copyright {
	CX23885_COPYRIGHT_OFF,
	CX23885_COPYRIGHT_ON,
};
enum cx23885_notification_type {
	CX23885_NOTIFICATION_REFRESH,
};
enum cx23885_notification_status {
	CX23885_NOTIFICATION_OFF,
	CX23885_NOTIFICATION_ON,
};
enum cx23885_notification_mailbox {
	CX23885_NOTIFICATION_NO_MAILBOX = -1,
};
enum cx23885_field1_lines {
	CX23885_FIELD1_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD1_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD1_MICRONAS = 0x0105, /* 261 */
};
enum cx23885_field2_lines {
	CX23885_FIELD2_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD2_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD2_MICRONAS = 0x0106, /* 262 */
};
enum cx23885_custom_data_type {
	CX23885_CUSTOM_EXTENSION_USR_DATA,
	CX23885_CUSTOM_PRIVATE_PACKET,
};
enum cx23885_mute {
	CX23885_UNMUTE,
	CX23885_MUTE,
};
enum cx23885_mute_video_mask {
	CX23885_MUTE_VIDEO_V_MASK = 0x0000FF00,
	CX23885_MUTE_VIDEO_U_MASK = 0x00FF0000,
	CX23885_MUTE_VIDEO_Y_MASK = 0xFF000000,
};
enum cx23885_mute_video_shift {
	CX23885_MUTE_VIDEO_V_SHIFT = 8,
	CX23885_MUTE_VIDEO_U_SHIFT = 16,
	CX23885_MUTE_VIDEO_Y_SHIFT = 24,
};

/* defines below are from ivtv-driver.h */
#define IVTV_CMD_HW_BLOCKS_RST 0xFFFFFFFF

/* Firmware API commands */
#define IVTV_API_STD_TIMEOUT 500

/* Registers */
/* IVTV_REG_OFFSET */
#define IVTV_REG_ENC_SDRAM_REFRESH (0x07F8)
#define IVTV_REG_ENC_SDRAM_PRECHARGE (0x07FC)
#define IVTV_REG_SPU (0x9050)
#define IVTV_REG_HW_BLOCKS (0x9054)
#define IVTV_REG_VPU (0x9058)
#define IVTV_REG_APU (0xA064)

/**** Bit definitions for MC417_RWD and MC417_OEN registers  ***
  bits 31-16
+-----------+
| Reserved  |
+-----------+
  bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
+-------+-------+-------+-------+-------+-------+-------+-------+
| MIWR# | MIRD# | MICS# |MIRDY# |MIADDR3|MIADDR2|MIADDR1|MIADDR0|
+-------+-------+-------+-------+-------+-------+-------+-------+
 bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
+-------+-------+-------+-------+-------+-------+-------+-------+
|MIDATA7|MIDATA6|MIDATA5|MIDATA4|MIDATA3|MIDATA2|MIDATA1|MIDATA0|
+-------+-------+-------+-------+-------+-------+-------+-------+
***/
#define MC417_MIWR	0x8000
#define MC417_MIRD	0x4000
#define MC417_MICS	0x2000
#define MC417_MIRDY	0x1000
#define MC417_MIADDR	0x0F00
#define MC417_MIDATA	0x00FF

/* MIADDR* nibble definitions */
#define  MCI_MEMORY_DATA_BYTE0          0x000
#define  MCI_MEMORY_DATA_BYTE1          0x100
#define  MCI_MEMORY_DATA_BYTE2          0x200
#define  MCI_MEMORY_DATA_BYTE3          0x300
#define  MCI_MEMORY_ADDRESS_BYTE2       0x400
#define  MCI_MEMORY_ADDRESS_BYTE1       0x500
#define  MCI_MEMORY_ADDRESS_BYTE0       0x600
#define  MCI_REGISTER_DATA_BYTE0        0x800
#define  MCI_REGISTER_DATA_BYTE1        0x900
#define  MCI_REGISTE