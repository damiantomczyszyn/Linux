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
�	�x��N�qM]������7+vA���m��n�5�h�G$���S�ϸ��])��n&�?��JF�t�~�j_d��#2�0�n���1��<�k��v~m��q\�XșG��	!a�