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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |1(JŸN­ÙÊƒÀ`«k)öf­ü€ðö×–<èRçÈúOx€¡VÒ€²ì_… @ŽÞâÞîõò#â­Ó¸aÐø«$8
È?–†ßé|-;Ýfåóxûdî9?À€‘½ýÌù>Z©kÌà;í
É%õ¦·ÄÔæµ“@)\­2ßW£°IŠ©YÚ'Kˆº=<O¼ß‘(ºí²@6ãåáØöÍ/a¡ÎE_µõñ7FùGúçýÄ0_Ü™Î6H•ïðLU¥ÛÄ˜E³ÄRÝæ-ä›È°ƒ²îÞø–{æò‰QìmšäZ’Ã¥›Ö¶’K~?_ŠÍoÃ!t8[tÜ¸MÜõçé,%V­vþŒ-Ef$¼X9ÃAQÇû\}ËÆ$×/LH?P“YêIöYíMè‰ÿ‚²7ôZGSîî^ir?­Ã ÊTìðÞŽE
G)nŠeq5øÌX”b•&A¿¦6ÑìÍ–ÁÉ£HBð¦0E` ÜÕ¶Øk’‚y €W¨ñþñ@VŸ×bÝ_ †w×ÒqÍ±fnÊ ‡B²ïþ ¡ÂÒí¦pcqë·%í£?yÎk4 Ã7Qó)“¥ýüñUuM’Ä~L£­Å²åVz³m%º^´ÿãÓe&v“ØäŽBÀxÄ¹õÒÝe¤¶3˜—Í_AëðFåÙŽ;}JŒ<œù  )Ðœ:k'Ù¿Ã`gè|v¨|‘ñIã4?†óMÁlddºñ}J4þ?1XºÚ„¦”!Ù‹8ÊÅWÿ—ÛËÀ˜o­gõÂ½}¡ íDó?iÑ†bHõ*ìOÛŒ°]hêzßì"DR¨|J	ýàãZ–§_˜†^¡Õ”&ˆiðÍ¢
”;#Ds‹›’{·¥È`g¡åâs-¶‡”-ó÷@;vË1»“lÿ,UÓ'vp~­¢q³.¿®+>½G¯ÊîLèß|ö>mýX#©Úã=¶ðIP]·˜‹4UêÁŽè%£ðlh-ýs­¶šWiqüÓ	Š¯Nrq]A-4”¹È 2òz¾À`Bé9pŽgàýyý~Óe*ÿïÀûTÙ’m¡„Bòêa‘Ž7ä©Íá†[¨n ßÜ
{â¾àï³óÀ^/F´Ú1't®*ä7Ò¦êåƒ'VÍŒ)¾ˆô¯š•èâãÛ¥¯ù#TGˆÎ"ðƒÊH#Ëþ¬<t²¡õM<‚ó^öo®51Ôƒ€ÓêÍÑ™QDÝ§[®TŽÞÈ.¾fá!XbxŠ$.o:“\§Á„…zâ @FÃ£Ý1âwµÐWùŠÚ8uè“Xq~°'#¹~™šZÔ•L£´xÜÀ¹wÔ©ä¿þ\Zh`K$uT‡(§ò½HPâØ3|b´ûÕÄª7DC`œèìœ‰¦ªéÉ2†”¤‘ƒã/h§"·¦KÄ=½WðI-bNWuäÓÑjx˜ôD¶÷/³ï”q8F±×\,DEë"tà¦ëÍ>Ik;`|È<ôþ£Ì pÜnwåw¹ãÝÒ	g(í@Žž+¦(D:ÿ«. ÀÉW§4}OF†µÒnNv-.Šäb±óV ÕL
cöŸpüêtÇ¬Æ„°£ä¥F÷yf^WgæAé:€¨wñÔ³í0þÙ>#23¬éq\æ˜æ¥/wjŒ‚>ûôÞŽÚ“IFGŠ:t9Å;K_ŠnÓç0-Òc…¬ë+å}ž™3¤ëÐ¤X” ¥”—üƒ–B¶üm%gjªÚ'ß¾Í÷¥b÷ÓÈh(¢´ÈñíÛq‚ù¦?Ë¥"M–tûÔDõè¯·†‚‡ÊB’»ÙÆ G™7€ò‚Àèq³f4°ÓŒ^ÇGûjç­+ª¬÷‰#ôœââì
:-;‡»9™ÖëåÛœhP¨‰§0hRágÓ®‡$­´ä»’Ô#á{œýµ¦òÍLÅOöfÉEôìÎ "Oš»È¾1Ò"0¥=XZï_Å	œ[Å¼½UC­I'­™çYbÇ~2°ŒðõTø±;uîª©tãÅSEþòmÏ6òŒóéÆWÞÞÛ3_œA¹–’ì.3º{Yeá/bFŒÁà2wd¿Ï*«äÔðŽVŽµ¹Ñ¥¡ƒ¥†ÀŒŸYí®k‚™› L§­¢+—
eþ§è\ßgk:'ÔS†¹³÷³X$ö¶k }²a?á#uÔaj@Lð]ÊE¨×, óòŠ¸RÄAp²þ`iÝÿcÐ±AŠ\T„]›çÔù¶%l³±
—<ÅeÈŒ$:äàJ%ÿ¯ÏEfáòz’Å`:G	ñ®]  Æ+Ýé	ä(òæ1:©0Ev|ù<nÈ]êæß²9Y7ZÏªG»*§…ð^—ËA¬¨—ø&!¬X #véÆRE#T_ø§.C}\<YmÓ{Ytr Fžï`øänÆ¼Ë«	úO]¬àÄ’Íƒ/±’1Ø—û[˜Ôƒ`Ä#ñN¾–ïŠ~(Jî2’Ÿ;–W=aš4oh”AwÓÇÐ½#Š½#T·i¦
Ž57¢õ €ðr©cž³}QÑLoó#èõ’®RÅtATËq‡Áv¦KééÃufè0Œ¹ÖãÖêtJñé«äð£Ú-LtÍ ½¦Wmàs&drn·¼À>tc4.óæPL%zþ2ãGOŸNÜV‰R¸Í^Diç¬o"³‡ÿ=?¡¥Íõ™ü<Ý‹õµ½ªaa¶ %;jÇÃ@“’ä½§Ó
!ÎÑuÚÈ°· 6=@ù[Ís›ÒŽ’åÓçŠ…åÔ´«^tT)ìgÐ¬qJF:oø(ÄA£õ¤K³À°;%±«dýáK</¯´`×Z¬6û§£+¥¡•·C¿˜lâ­³ÌÐ¼¶T rmÐ½Ø[ËÕsœˆ
j09–A\„Ÿ™;huwòËAü‘Ñø«7ƒ˜@0ÓŒ¾•€Ãoo&1Ïhàj:-Þ>~L¿—dâÿJw	öyF!ãQN^ÉçáèÏ¨GD^ØªŸhH¨úÑ<²B	¿Óe‰+;f¢ÝOr½A pƒ½pÄ[	ä]
u“¸ÊË®›=0©á Ì?”Š¥à'Þc³›-µæ•bÖW²2C©±ñ”‰>>J›ƒÆh¸£ÐÊ¸aæ·Úï¸.M¿ªsq}‹E#0¢’Þæ^°à¶Üù¼@q¡µÁSíä5Bÿ‚*r³e:®­^¹¨Ê¹ÿiQÈ!ÃŒáÀ†‹X`}ÖINÜ.µéÂØãäÓ§D'¯÷êþtÜÊ[ Þ¯ÇÜd—Òé*—¦z³¾#üÙÎ¯ÌlƒºÒEƒ=¡lAÍÜ½š˜nÎ¿¢ÆœÊñ¼‹éâ´¯â÷O':àD×$!Ÿð~>Áå†ÌEº1XÕ Õó„¦ƒ<Ô Î[wÀdÏS@kH4!{ú{S²Ì ‹ÆÆŽ™ò`û//¹GÄ®¡¼ÙÇ’ËëÊ;¢íŒ˜gôÂ’ÊToœl¤7Ð­¡¢Õw€p!›, ìûÍjÆLÞ›˜mK‹lÙÐ§—ªŠXáßÅjçs"89æ¶¡6WÌä…í9oEdÆÏxû© 2Ã®HDËq‡?Y[k\’@ü8Â|ú|¾Ùå“ñ`ºsÂgÖ[ÿÅöþfI#*¨êËûl×$'0ÕïŠäÑFS¡ü®9ëGÃ‹z–ÀÛ.Ý°u¹!†øÕU_¢ÂoZß¨cŽýØí{¸R@FQ‚Iw Jv-eÉ9*ÍdÔQ‹€Ò¸ña[¹‹ã¯Ì· iKo‘ŠK€ÿõ°6ÆLØ½ùÊòôÂ¬fâËj×C²ò¦Ö·‚Q…¡¶1Cã‘zi×ý]W“a_ÔY6·r©}•þÆ‚	Ôôxˆn3^ a7ª§sŽ¬œ†'û5ƒðX‹q0¼Wiú]q_Ñ´èÄ3­ÉãªqØ€¤ôDq4:,ò~ôýœfÉÊ”ud—h}N,{eýÙZ¤µgVŽUµ“ƒŽQ·‡‡	x@#Û¥dY­‡=Ë
½=„¾n“ÄÇ
ÌÛ·(@Ÿ¿Ô•FØ|ù±ö}¶¤£[x¯À*TÄðDt£­„ñê¹òæm„5»ìyM+åÚ7À½üJ> ëÅñÞ¥He|cÿ”LH‘Š(ÌœÝÉ(£†6-Ÿ çÁT$/wOP@úˆ›l\
;˜'Ë¾¾ÎH2	2ö-÷­Ä\Q8×ëæ®‡zÌ£Ò`¢U ¢ä ºö©Ý|FÛIÞI#.Bäö	$óã,S§²¤††ÚMlÊ’>á³-{‹"Ñ*»à¦ºá‡Ð»¦QÕÛ:ëUÿÃ"º1åðóšŒ¾¡¥Ä›µ¤øˆBˆ“!¬j“rÔd$‚ÜúcÍçé‰œó;xä-@_‰Ò­ögYë5ñA² ­%Íþî„Eä]TèyÎÚßuß•“Ë
¥”Fq=€OŠrwÐ‹A§^¹t01sd.¶¥bíÔ\ î’29,žxcŠ¬NËªusÝOï„Ã-gkë?®ªzÄš¨R,Ìµ§Ê—¸~7¥¦è¬zDŸ•y×	š1Æ¤Übì½eÞe7ñöìœ¤
BÉ‚ÞK	¤P®óc†çï»˜1ÏP aîó¢ý”‡*£x9ºBxÏ™&ˆ´¶íuŸo\ôN™ÿ[^j°Ë£öË\a#Û\ˆ´‰6PoÎjÿÅÚG<†­ú‡†)ÞUžù·è¸¥•¢&ºƒ“µvÖYhÍèâÃZ‚¬ftAìl´`ŽCd˜w/IŸ÷E 4°V_/TÛF¤N/Ñç–D…Ýú¯Ã¯¦w—J53­"Dƒkõë…x¯e&—´ý¹(—ó÷w:ü²ß&@5IøkÃ|¢åó­*×É±D„‚qžŸÈ®I_‚Uh‹YyÎg@‰\cT-ä{º²Þw\Ãi=Ö…kÎ£<,‚a%–¶Å­ž®×l"™’`M»c/kô’ ®R3¸ê÷e¯ßóÄ“S•í/ÿ1È²„Ú§L)"w8Ó£–«Õ¤‹Â†™¾4GPDCO7bc‡)ÁÇXäsXÿ&BÅ× fTæ…¬ §öžrŒ[¥Tùù0ÆûPo^Ù ­gþ~Kh(íÏhÃ½«mÊF…IÍ^Vìº»”æãˆ5`ñ„''Úy¦(¯Å\íŠ2…¸[ê(ƒ:ijºäè®¦[+Ór7+hBƒ_qŽA¬;±¿Žø(æ„8·Yã)8e¤£V¯kKõV‘yæö“ SpØÌÍT5íX^É}ž«4páæýB¸ëšØ‚w1×=Ñý³Ç®¨¡l¾Þp«Þ÷ó:*ƒ?É@O?	PÑ„˜}Ümÿ&iê"q¯üÈO	.ÇÔ.ãÅ¼ e3§,65Ÿ;šùÓ^†%H°Biñ­{sÐ@øa$Cƒq`›‚-œá°›îš$™HYõw*.FöKÙÑeWÉyO•²ÿ$:;t§Z]Q’:~ÌUta’V¸o–!?å0y^!YåŠùàážMkP2µ:º…É½ŒtüH(â­}iè–Úîü?y2ÖŸ§ËÍji¼ë/k¸Áè%—ØŽÈJk× 1Þ«iG›IÍ]Q­·b3äoÙ¶®ù}„ŒAÃ h¸CÁyAóõYö‡ú”ÂT5æÄ€JÄˆhm*ö•ù¥öAï£s˜”kj	D³,b™ª+`…þÚXŠÍô÷(`óÝÜûÁ'£ÒCöü{r2²úXöËAëŠÔ%‚.EvÞŸ.ÁùžXç2#¢«-TDt•8)"¦µ¥¬UÐ¸ly5žl˜üpS_Ñ8©,S·Â¡€úÖvvè­7)7.®Çr™:CR‡‹økç~"WEô´òkj“Òn\D½g¤¶®Ô }Ïèœ¹¢“uþ*Ÿˆ!wB½¾Cò&ØÌ{ÌàÀNÈO…N©Ü6\ž0­2[K»ÇŠôæ‰Œ)o|ãßtÉá¹çËF¹Àþö£½MFª<ôøyV¼gÍ4¦=Ý¼Þ
ý¸ÓÃ$ìÁOí¶f:UxH¨,\6	iÓØŽÁ)j"¶ú‰Ì#G©ò'GÒŠ&$‘¨Ú>MÆÙ ×¨OR³Õöë÷ƒðÒfÅßI—FžÛå7N$öH”Z9ûWP}h”ÂÑpÂãLš™({Á¬ÃQ‰¬xr7žM=û~mž@x{pL3ªVrfòÊ×K¢û4&ßDã©¼«/#¼Êì’±¢tøÀ(ô
Àš©ˆœ7.|{×^?¼nºç ¨Î¡Èon´>sâˆ
åO1u²,¾Çm·¬¿Bˆ²ùæÌô‹#Ÿ^„C”©­¨HþÔÙ¾gW„z]ðRÉ½)x˜ú°wQÍýø#l}<›ùv0†’40¹——Že”…'95Â¡2ú€à~Tñ¬h§ºm¹1©Û1î#J~ð¢"ÁÓÿÂ°<¦riv&áe^Ý¦‚Úå%NvÒGÓºÂ×ƒš[vOi}ãö/-—o#¡Ã7jËœótïi—çôâ¦‘Ù­ç\®b“›Cv³Î%†‹aôc@LÙÄT0íÕÑD‰/Ê}ï·×—g[òÅèuÖ%â9\}ñŽ°Bèê@DòÝ~%÷{äéÌnÇº	?¾2 Æ A)½nyQ²ëîöæ<—Ôœ¬L—¿y¿ÙùE}»%Ô©‹?„(ý’ ù¯— Ž.Ñ˜aoq	Eéöio ›†„fé+|ƒ@Î]"j§(c ö”PgõC®´ñáå:ÃaÐdDìøf¶©žEÚ—gP˜Ùœ°2ÈÕÝ6ÝÒGÅkÉ+WˆØˆ¸)S¡fÉ$Â1Ùã³má­‡\zc*·½^åDéŽ|£‡7§áÜ.–’%’ÒZ"ËQÐä¨åå ù&£Ï÷.¯,Óõb‚úD“d%ÜEß	m3á¤tœü'í>Ù÷~GÁóŒ±Trw†(9[”šC¾ ¾àÚÿ·lŒ®ó<t3¶2{ÔqËÍ¥vkÞ}£xËrúá˜,Î(¿ÉÀÚMÔ<ò^kµÆÈÅ	U¾ÝLð‰¤Cþ­±Âr¾6W`‘·{À—";š…ì©XÎ]½6#ƒu€îU`3MÿÝmTæÖ©1(˜ië”@‰ö+nÔ|M<@.÷\pSÞEªÌ@\9ï>U6ÔÂ&—ñ½¶­aS0úQ›þö&IÒö#²®yNŠ+[>åñ àö¶°.¢è“N‘³ig­‹§J£’ÞƒòÅën¹ð°v£«G:.&1Û‰oÖh ²$6¹ÿ­}ø3ÄßªuAÈÆ£!æS’=áPÛEŒE¤fŸqå7GôL 8/„Ïuñ¿‚ÐNC-DÖzÏd(àœ)¾]‰ÏìðÈ¢f_³úÆcÎ’ð/¿d”ûnÌ^ßR Ö3Ç™d(ÁI'Ê@ô5<UË¦Ôqì­e¨da·áåG¯Z¢Ò¦NÍJ¿ƒ[¨b2t¾Ïž¼A¶»XÙ€ û(K8»tH…†õW¿àsêr„?ü3k´´»»züónÙßÆJá[¦w…¶Tz>®ua7{QêGC‘AÀN¸I0Suaý}`ØVÆeŽ%“ÀX !fX%ê©Ðè½Ãr%#.$‡RBÖMÝýŸ['{ 5XRþmÅëór:0ßko†³<Ö_Ñ5IÜVàzòÕ ©ªŠ“ºþvP¼çí­Œ8JÄ{pîòÃâ=íÅpŸ<Èœ&…Í}J$cé'ò-E€IÅ:^½g h®;2–¡=.ŽÙhc%ù6ÊrÏn˜jpÞûæ© 0¢è * 9½'»?ªEÓJ}²ùÏhí¥†’zžûãS	Ü¡ÛòŒd
áP·§r0¹¦3óqŒTîk3l/Œ‰‰‹”ò±ôKQŒúbw”Ñ=ã‰L¾vÝ™=qC¶bùŸÑðzDí€HsTÓKƒßi|.ŽÑogáÿJ¢éyÜe^Ã×rÒãÞ$Á{cŽ9<Ð—õ©½÷þóFWòxS8zš%EJÇ”ûlP·e"á;«¹8ÖìÄþu\c _d UžEÆˆJñIßÀu[×2ùNñÙá¾é¹8m4†ô±È8¿[¹]éÙT]CPƒ:¾“÷Ñ#qÚÊù‹Y^yúÖûzçÕdØ¶¬‚-)™´¾Ð}Ã[½¶IÐç&úÞ%Q¸ô.ÊbûÌfœÏB{-‰“ëZ­ÈQìuL<ÝšÇ„•B'âåƒŽ5]–ûzÃ?XùR±}
ƒsáŒKÄž‘Å4‘íµÜ¬ññ¤ùjìÎ=ÈðË+ì¾-	î®üàrÖBŽ`.b•·žÐq¯ALU˜t|úN…’%7•¥Îøª²d¢ŽSFÚ!Ô}EŸ¯Ì^µŸ_ÆääÇ’P'¯Å.ÜîfÐJy²D~ÿÒ/³±.’m›ð	-JÞ"DiŽ˜36=V¯*#óUå	'-³õË†YYéW!ž’(_$mü)Î-á¤ú†’f[çÞè5/£Ú;t;Å’aõ“°S…p'h‘Š.ëjÇä{1vÎò†, ²8÷™vUÝüîÚ³wŠñ¾.ÎŸ?©Íf˜wˆ1V
M#aï½6
ö"Ë¥@RÇ1-%½%PlµKWÐšÙÅð»mŽ¤ßÜ_!8ÚÖª›ÿ$ëXÕ¤–’Kp]A<×•3 =NSÅf{ã!a¥ÁöÛfªz†`KÉ[’Ç´.QÕ‚4Œm@yÏL)ûlãH‚ Š#L(1dLé²G`Ål‚D{bcú§âù Ñ†oÇùšX$û#ŽÝÕÊ“Gƒä-¾°Weg’"`­Ã—äÈ.÷¼p6˜©M€ðæ§’±æ™k‚¡(–UQ¦Ó’KÎfÛ¿Î×ôÞ³ÏÌw³ÔÓá{ÍÐ?øËt$Ûa_}DH÷ãkÞ5§à—þPšj›Ö8§ÊÎ0thIKhjI·Ò®µ¿ÿýëÙpÔ€²½;þØå¶Žèº—QÒ³Â:ÞpœªTˆ\V$¶l(Fí™)é1ª^99èMˆXä$$œZ-ãÇÏÕ.fê›ß¾>Û¤uÞ¯Sj]€àvHŽ˜t³YŠ]ƒ.9µ?ŽßÐ›¸¢jÒ3ÀZPL</02¿ûEsP‘ü™¹ÒhäNã˜òL!Õ«½„K»¨d¦t†š©C°«X¬&kÃ7Ë­ë´˜)>¿&q³qh 2Ä³éÚPDÍ|!¦r‚pdŽe™“i‹Úà¢Ï4m.Y1­’ÖÄý;—¤¹…Æ¾t1³(û!å;ZäZMµ.\¥«PßBZ²*”HS”¾”Nð0Ã°ñû®ãlüÀ£±œV‡ eV¡¤+Å‹š^‹ïŠ!{9ÿˆ@…lÁ7Ö”‚9×2Px÷ùŽ²iˆLôÛ[6H}}òc•ùÉ2Rÿ»­5šø½{Ðd@™B{[5V!Æªaûä¿•lX7†S[“išÇGûÅ¸™Lg^PiŠíÈqZ`hÆÏTŠ¹!ÄsÚZ”Þ-º­j¯¶Vs»Msk€¤×7tZíþNvâ•ñtü|c"öëWÉ“p¨8øzP×¢@Ù´#6 Ž´²V~»”k#löœ»«Ü/¥Éñó/(áW%ü$J+ 2õ+%rfw	ˆ"ËaPÖÕ9êÎøÐâî[Ðç§C ÛâñïÐH€}H`jƒâöÔ#tY³ê§ûäf Â|>nR •Û§WXtÿÀ*d ŽæÂ“(¸P×^]cÜ¥|5ÉPb¿ÞœbKlI^h³ëNÊJ±_âšrôOiU\@ÁþÑŒis˜-/Û¶ÕØÕk4lß¢´ŠiÓvf@ÏSn;·&ÀÃä
¼3Þö=É¥}”9qªÏPÉÅêÐºò¾éR-%3Ú‹Óï T*mC·øïTäõ©ÿßpÕ`Q‹«ïî2vø'FEçÎ¶$áåF¯xæêÅªÀ•ý7k™°sã!Æ†þR7àä{º ëeûQŸi»:2(šÉDŽÐ¡›…0¹)w«ý€MO+!Ëû^ºëç2­)-¸§ò“Ö
²õm5Ê8Ì^JLºGþy¹S@›Ü8'nU>©«ËŠY„6tô€lA÷¹ç„åÀÀÿŠ9Õà0šŒz(6Ê¤U$—}ù7Bè²›…9ßÕâ¢elB:7eï}(ÍÓ[ÉüpÙÈéY•…Já-Ïž×z²‡#´hÿ~gÙ·‰ØöUñomZ›
´£+y¨ìâ¡êf¡ÝTÂûáiŽHKãF7²ÇþîCOýí¡r
‹>D}$^SË&'
MN»YÃÂ.Z•H<E(e#¥t­­WáÝZÑH¸ñeB[¢ÁovkÚ¢¬‰ÕôiÁ¯
ê—¬Y+bñÑJŽ÷gÅÝ 5þûŸ?p%XpdœðµFÿÛ‹ôªNÒ=˜ðRâ÷¹4;½S ›ëEwä;»Ï	ý03“{¼Ç¨‰Ú®ûz•Ë‘'nÒÞüÌ² ç¿‰
‚ª²Üí?–ßô&« @DLsãhÔtkåq\ÌN–ç.«ÅÊ9 Ë7*Õsëôb[;ƒí¨¸³cjÛ¸æABaÌ»·¤üìÖ_§Ä¼¿5«d`_²ï_1úè“)À|v%¥±ã9ªæºw:F@!q"Ç¾7ìÆÚìò_ :T’™cïd
Ö+Fà_/Ö»§hÝ]Ø&§‚CÓv¥8C·¸ú³ñ‚Ô_!HÞ$5®i¿ñá}þJ£C™ÓµÇYŸlûÔ¸ô>xF2ý!˜îKÎo)•Óö)å±ÄÏ7NÀ–‡ã”HPxYë‚Qµ^ôÃì-vš¢íW{·5Ì8î)W“öBdJI“,MóÃ?‹ŽAÍ“TÕ1³i´ñFtøÄ©}AD°*oFÕJirC'¾4É³ù«×ø ß~N°Oý°rUu—ø@ú§áoYkÉP·õäöD’2Ð²˜Pª©_Þ³ A,y† 2ÅSOJò{]ÑC0ŒÅúð*U‹’Eø¬Îo6Å*î\Fò—™1.øÎ\§Sy\‚s2;4¬öJ˜Ôméë#©#À¹ŒˆOÇh?G[§­®oT-Š¶Ç¢á¬¢ÑÖG.í#OiÈ¡ú’r…å¶NNbÐ(n™À.’Ü0ÅAB&t	¦Ÿ‰C~¿Ýseõ_¶Ø-d\˜ØOš¬	 ¬˜Ä ûÙ(7(>zß#."ü¡ù—#/;¡«<>°QhV{ÛÎWVi‡É hqd·MìGíçö´”=LÅý±{ãd Hª-¿  òlåÕ­QQ+¼ô-xÐ2ŠÛÕç1Š	#ˆ¡>.7…{:}/aÑINå+Ô<%I"ˆbbtþØ3=Â<–jE!‘¦»¾Ca¥W¤\´EÁU:<ñ³šŒXº«PÿÜòÍŸÄ7O9×jÊ¨±!ÊŽÏ™#º¼ºƒ}‡¨Ù@âl7iÕM/¸ì«¼©¥¿2Ãdü"ÿ§*ÂR´³”vN%šú$ omØ¢‹ôT$±fˆ ÞîýOŸ¡ýš›äh›G•IíAÛl1ýOQê~GwV=ZñùñŠ.>öà¾0•ò¨ ìO`OT® ¾9Š¸Šã6ŸÌý£›2ì÷íìy;]ÈæQxÌ^©îÁt-È»|kÆæà\’†‚¿x-f•¡U5yýE`é5Ì§R]Õ’²úzn;ô|´û÷ ¢ 9›&KmÂ©»•Í’§]½š1/Ö7›m¹6}Û¢Ô²-ã8ªn-ºxFÌYÐÿ¢Ï#ºÚŒs‡èW4ÅÑ%£_<õŠ{ink‰ªs­è…èöNË•Í~Åp%¥(¾h\I™š¨Ôë†,\Nu<wbà_\’á&~ˆ•>&è/nÔzh|Ï•?H±Ø¸P(%ç°NOdÔÙY»ó…8çÔHðÛûí8Y¢`“=ÒOéõCUÙw·:ÂHq˜Š£Š…õÈ÷naáðã(ÁÛãt·¥1 ÂÉvYÃÐp›«¹xÆw¯lßG§*}ÿºT°zÞ6«P¿¢^2E„“fÓ¶ëŸ(=ÒåpeäVÖÄÑ9¹rŽ(7Â•¦­Õ(Ãü,º¯’%”XÔ5ÀBƒÇ$·cÕžr"/	Ÿù®?+¸Ë; 'Iÿ	Vç¢èV“ž™Ê( Hî¬4£ò 5Ž·„4`|ã(£íi>øì_ŠïËù¢SYH"…H4Ä:h½0™p 7ê>ÚƒNŠ~Á–À+y©nŒ™…uõÊžØSK/¶q)WßÝ8ªÏ±ª½?þä`ZpºY+7ï­l(¡‰Ãÿc0Ø&ß[yƒœ€ÿ»ë¨ÎbRJâž-KÏyÙó·2«­í³³S;Rý’ÅçéÛÚWä¢~Ã±mYùoáÓ:•|Ò³úkß
Ù¶•!e«‚‡c­Þä/]Ð­^l)‚gÈ‡áØ.åÂÅòÄÛó)ƒ|éq šËÍv¤±‚ÂŒàér("”5§`	(£ú•»¨ßYè¦Ëu¼zº}6–-™\:Ž"­íé7 Î¡4qigÜBøãLeŒ|@±@¸:#‘¤‚ ˆþ6©­†c[ÐR„þ¤Ëõ³O ¦ î†dƒ)‘^]wö*þ(^;o¾GôÓ­w°ËÔT¯”â˜Ò"†ŠÁÄb;FLZ£ïæXÆÑÀÁIéÊÍ9 ²˜hÑ'ìCþ‡k[FùÃOõ)×í‘ÏX^B¦Ù÷ò°nìÖDúOYS:ÛàY&~¢)È6,6Ž¦‰ÓP~LTrC×ì[{íNF8ŽŒ²P¡›-çu:NóB©àµH\
AÑùÅ§ÃFJÐ×‡)×¬ÛP˜˜˜®"´ÏŒ°r¨lMÆ®m¢ââåÞX\5}íÿ‡ÝFx—­È@ŽµÀáx[Ý:P<aíj6%
iµ(ÃD³U¤a*?k¡aZ–²¯ØüÁìŸ®–àVÆ‹ê”óYzêŽ£‡¡?33,šz2§Ç‘êªyØžÒú÷>ï-]*=ýã–ã•ÿ—ÚîGP­aÐQ:ã‹†Õ×þ@-8ë¾p”òj”[™º{ž™T³oEÎF@#âÇ)Ò’¨G­;ôû‘UÎŽg&­‰CÖÁý?qŒõµÂÆpÿÍùå™Ì$mØ »T&Fu¶<*Â¨P[6Î¯cÏÒˆ’&ÏþãPãƒm“ä/Æ›ÝYùêˆëÈtÑÿ£‹È`2 O×GŸ=¾*tŸ&Q‚¨¶xäÝ¿P(Ç€ÈÕ	5N«Jé—Ônpd¾ö·”5gqñ(z>I44”“£Îv "Ëž?ˆj¿,=QÚóß]úÜ‹/uSD> iö€¤æäkío„‘ùJ+_#&‘óLuþ«ß^š–…€b\U¢m––õ^¨Ûj#a>¼eÔµVu­Ü†”n±ïè!j¶—@"•o lvWÇîÝ€wË¼z,u³6jÏ»)9í)›Õ$‘8êÊ@W¿àmr€xM“Ø×àÁFMöð7€£ïIô9õ[ÿ—~¤Xq(þµ17Ìé‰s
Ü~œä1}	kÔ|ƒ’Øâ	!yÅn²$yˆ™E¦ &]®Û-™íG3weCÀ–ÊA”kÂ¿,mC½Y ÌÜž½dÑÏFÙ”b›ˆ[`ÿ2aÙ¸Ø#€a^9[<4*'-(¨ÇmµÇÿõ÷xèöèŠàŸMl%uI–ÅÛ³ýÎ¾rà¾ƒAØ€¸RJ–›!¿‚BÆDF5¹œ
òº¡•59’vØ3Pyg›í,	@"4Åh»So‚.dwÂ/ÄÚÇe+}sþâIiÿÝ
jÀLì¹úÒ´Ia‰_ŽÃG#´Ìs¼ë44 Á*ó>21_‹ ƒ¾«_g NvhæÎÊjEì±x¯û.\.¯NZ°gÌË‹i‰„ªyÀ9¦IµZ[G;‰.‡¨I)íÖ=ßš¶®Wãgq¿óÂbÊ™Ewµ°#hUâ7hq·nOñ:õúò^ðëàðª¬_[
£m²mÒ³[èVs« ^ŽÍ‰=#?ÊuË“C©aŽ{lË|pÇ]Ü`*TÛL]!ä Ëõ"Q=\«sêT«¨ÚN8OaàÿÚ‹,Oîá	¶^È<#{o^j5ž&™×ißaü@	Ê¡ŒùÍ0tZ}-®í;=/ÅšLY`š¬ç*Bv@Œ@¥aç~—ÍEÔ†dö/ß©©Ý×Ïâòš_TB €6ýÅtjñícšÆ¶­SòlêR¿ÜtÃ,‡vs…8ÁÁæ©ýBÑ‹wH¶lôëì ó†ÀÙFlÁ<¼èæ$
œËŠµ¾BT!"SÅŽðÓ«uò™È««—QË°¹OT8úÑ“Å6s{D”aá®ïPü±Äc3
†D¬c.ö<—Éëo½¡X9gLm«_v•ª&ÁS<°X2û -‰D¬ŒLOXñT’b?ú"€9÷{A—ë€‘u2úq44†
—÷—çÀÐ×<ötÖôÕU.Yï~µPìl-@ÆÙÉ‚Ñq )0«ãÄgáE°ãÓÈñ«L•;&4ünÒ¯Uó‘œ¹­³›3tP…:òšA¢õ¬<
Õ6&]&–]d2,÷€J‹„&+þä¢@À&(Ž¥w¡J´#ÆBDìA×Ê–/Û[®÷YöÇ÷¸p+ZƒD	L(?g´ù®Ðå[WäÑdLäóró$È #ª nšÏ:'½ËçSÁ-ÇU#{¼ò€Q¨ƒ¢ÿ±g/Ùü‹LVÛvÜª¤	¤2ü’%tlÒ­¯,ã%½@H4ï‰r#¢Ø²¥®þ[þÒœ-/jôvVú2àPn'œë;Ü%Hÿ>MQ3¾@‰ÿÒ:)>ôÊâ}1uû;¨Uô´’:ç¼™®uËFæFYGàÊhµøŠi$>_w³Yê»Èç-ô=@á©·îcÆ•YxAª°èlé–Lòj@ê"^6†—0<­7Vqõ¿W°b d—Ò$¨Æe6®ji©Ðœp (-”8o§£µõm¡x½kû¸ÔòÒþ	­NW&#rÎ‰¡\y$™!5NJrnªZœ(–LÖ´Ý«J$ïŸ©QµBBúE7Çƒ3’%]Ô²¥\©Š·#1.ÐŒÏ¯Ú|Ì œÿö$k>å\¹&MÞA,“ÁQJbaƒÞÆ¾zøõÏqÐøÇÚÒ1?õÐAÊ'X‡®’ã¤ð¼\®ÊgE³˜ç›õ5li–àR´Ú<AÚ2ù åšœ¸9«‰E(Ñ¨QÀ¾°å:;.ßŸbß¾´†ò §«›XÈ©þæ@»'Äyœ}úÜ’«eâÊ‡\’3…Cl ù9ÕðLŸø§ ž[œçfÃ÷~¥ 	‡×VïÇåƒ”L³¾˜£§0‰òû{+rh{šïº(À¤'1$eÄ³ÝâQ3¾ÇM“m›{!™"ëÒùr‘¿<›ˆ°ªv¥ƒíø_Žz¥Aª¦\ÈäAÛ9¬[<ØEsMd¼z¹lè©yÝZµG¸LÉäKn«5².´[³†sØ¾èž~â”ŸºbŽò«4Äÿž,1_Ÿ1»']&FÛ	à”Î¸‡‰£“à_VøÇ_¬žlƒ÷Ô»íÅf%£t`_¹|õCyæÃÀ§kØû\‹”¯	øtE.½åê²aØóÕ±Í*ð'Ë¼Z_ì9X÷°ræâhnÆ
S7]˜6Û£“òª#xI	gs~hMUpãõÉrj“)ƒ9G¿Ÿ¹¿çv8=â_©¡àJß>…¼ý¥R€ª&ä 0±ø/º,.rºNÔmÐœÊ¬Nc„5µ…Î,]›Óõ™š`üjßýQ5ò·jÔ¿§žbÏ Ž×/ÑŠJ4‘+À®Žƒ{hQôÈ²€Ôâ–3®þ[ØNÆ ~GòÓóggùv	½:?ÚwK^¥Å­	.I¯¶Y¼ª$.%¨w°ûˆw±çXµ´v±fjG“Yž#<Ä?Â8õ9{,ÿÙŠv­zû#ªDùÆáÜâ@{‡ ½jîÞ(Õh´‰zq¿|dî¹€»®±ÃßwŠ=B#’ÓKU'ohAxÔò|y™újìÂ'²+Ó†~tà!7‰Ø5X)vK®Åø¶²…Ï…¦R3÷ï†çT<ßXöÜÍ9¨v<Ä`&Ç¨Âë[å¡Rnªv\5ò³v˜Šù/Guc¨Á»Â~‡ÛÑˆi\pHHð¾ÕU¢ØÉT,Ë×Å§<µØ9+\hÉš9Åq,òÀÉ~˜ÊþwƒË gÁòE„t¿ÚeäôsŸ:kªW)ÂîPçz®¼ãf4v¾ŸGñÎ6”¹Æ¢"ÖK5˜Ÿ%–®âÉÄW£vºô¦µùJÛÞ¬l)o·BçŠ¸:œ>N¦·@4uâgÿ/%,ïð—©F}ÛFé¡È¹cÏ³Zãç°g«)ë¥ÀH.¿±Û¢
Ôq¬
b »‰¾d‹õRz×?kL†\ùA3ãªUÆp)³;%—3ÎŸ„7 [JPK¾¾@¹w›
†ø3¢/ÿq8lUÞ@È!ô@Ìß¬…‚Ë–=\J¢-h‡ëÛo`Ñ’:–˜ màÑ9àÚÁ®&‚ðžº<+?MŠS—[Z9!jÏ÷¼ˆ†e÷ØLÄ×¹ÖÅK$£Ë õá‚^G´
l1;Ý¡-©™²Ï/‘)ðÉ+ÛC?½ÿ¸ñ›¿¾àù6”eôÌ7*j¢°†ô€íâšð]¥dý"#§ŽÝîþBÇÄ.îÁôYâŽƒª$¨¤Ýgßðæ4Is©x· užsíÿ!á˜4KqèËD²~³x†ª>ç@Ðñ÷C a,æ|ÛÍºiŽHxJ§Fé{¼ASÀþ„‘¼2eoÁhbæJý’hs½5k½^M´wuŸ2-xÖk‰ðz,IÖSë³´Nú®=®ä9’+ÖÃ—Ý}h¦ôbŒ\Ø“¤üQ=ývï9ü”[×8U‚M$¥7yûáéPz/á çÚ×>M…‡P'ˆÙ‚ùq™ÕázµpOÛ›’Êhë„‚yÀ10­iáñ¦MàÕhã€ºhtna$¬Úk¦r´îp±ïïàO
èužºø©i$
Ë¨³:P°TÊh¸ØIõ¥K_‚,‹ X4)Òfp˜+HsvšßM“±O|®Žm&àÄ)(X×‚©VîQ5¤Ê£Ìä¢qqù˜3 ã/Ø+¬7ïDîÃ±svÄ°›awn£†£?L÷þ¸€ŸÏrã5oÏÏÏ”‘`Ð#OžCh,ÞuÃÂ=@Oƒ‰¦²K1|žØúJ‡pFm‘$_!Àª†¼}ÂšõµMËßïmÊhï·ÚK°S)S*-Ÿ
[" üñ¨´
^ýØj+¾ì®hSÞqÜæÛdËƒ ‡¥#fµ¿8á¯§À}ôþé¾·rloÑÕ¥ö†Á.»èm¨‘$:‡Ëxëêqé-gÆ˜Ò×Ç“ë9:h>Ê¼9>äLEÞZO]›ö 3i¯©ÞX56FŽz;THšÙÁçéOÐê~¤oÉ­cM?j„ å’²ô˜å´ö!¥?>M³Éç8ú¸øé~¾¨ì(~\ŒÜø.7ÿçæ^˜üð]áPçñCÌ
OŠ^}v’ðÌlÿY<ªZd,¨r9!—:»aS}éú/W{Aì®Î:~Ë—¨ì®öqrž‰ZyôÀë}ÔÃ[à ÕÉž¸<ÿÀýp"ÒÙynòQUàÑ2!¯Þo}›—-˜uH²ø7‹r×0LýNZ³#ÑÂ§¢	K¬bºi$[•ÛQüÛe…3$Öl†zªuj¨b ú&+ísÂôFNZfDnß+™ä~s>¶±Ò*e–‹#‘|Ô6msÄdÎRK½°2&“Ÿß«=íÊg¨#³ßkäæ<nO6ªæŒÔäfM““ò3ö·ä AAø±o¿#;Ê•›Íç-.+ž5PµTâšV×±ðÈêˆ¶C¯M‡=g“<ÒúÞµ)f¤ÍEciôdÊŒ—œg©¸ç¨<["¼ö,ÿ8³n/æ§ƒrº·AJ¼"ê‚¼„4µ½*~Ža÷E·cHt:Ô¶ÙøÐU½Ö!ÔÂ²òí—T¨8])†—ƒBËkp÷—8&Uè€«H“6Xs<sÝ¿–dnÆm¥r´/ûÁ±¦É™¿ú¼¨î´W;bPX2ù…Û;Œ,B©&gÃÌ:g-ƒŒÏcÒN(ç ›¾X¹6òÇ7,Qdž©}›1ïú“Õaè
'ÇŸ¬%×«=GÈ›¡mË·©.-ÐÇÞ^i}›R%IeM/R?àÂ| ¿<¯¿$
š†E'ÇKÑ	(2nžÐ[ºjAiYgrw³kPÂRÊVíCqL­KzÍz¨x$êG¯H¹I'˜ƒèÃ)ó•ÜQ„èÙ SöMïÒgáÅÁ[`ø|5‘ƒŒâ 'Ð(æx•7µ€vÒ{vT×õ!ÇÃsÝn8Øÿ%ðrn:§ûÛ¼ŒÌqÓ5â(­cŒÔDû^ÅsøªqÄì¿®™é×¡ÁïS
n¾ÄTþ»»òg¨k é7µV¾ â}ZŠY«œåS”FšnTCž•‡¬‘;ö…týLrÓ9ý˜åè‡öœ›é
®UxEÖ›wØ\$Œ;¯?)‘âÎ>ÁyWàÍí~s™óozç±Ÿu0!‹?
@'¯3Yl3ØeÅëÐ”¾Ãìž€<½ À›MÛQ½IÌe„.Öƒ¿Ô“˜óÒ–éaªï\1ÀS~CjŠì—
¡‡ñÖ¹—!EÏæèÜ)Î$7§N‘Ø^QIö÷Ž›E	å€¶¿msvrž”ÙƒX‚8f$æ™A<|¾çvˆ»“¬z#(Åè©rYÞuR¼×™ð„3SMösæNäO.«¬H“ìMBÞª¾LÂnË¨Bƒ|ÔþE¨+÷N'NÆp“‘©›RáizwHžÌoµ‡‚åvó€ÁÿÑ[œEZxàVu}ó«õðA=×¶¨è¦öªê':~õ{Ônák` ¸?pL2¾’ä%£7K0ìÁ\5\:G±ü–B“Þ?ìpÂ ¦t"dèIõ~×åQÈàVM-«]¢ý'ê'­Š§—I#Lµí©,šÌSY†þf	)š":´Ñõç°°T˜#ÏAóålp2©ê;`9I¬Á^T*ëÃ®èö87¬³a¸VcÖ	Þ)ïõZ8‰üÕQú¶i}–IXÊ·¼#ÔÑ¢®)ŸþÍpnàŸ²Qqátº™ukØMU~ƒ`ÎØçñ{Ö31öÕçsŒÞÇz@`±$.êåç ¾,!Dðþqo¹:yæõÈ”‚–›ÍãÆ„,l<}8þ´bŽ$ç¾žB7­0M9)Âü16Ô;A¼^¾‡nÁxøˆlÏâŠPK¶h{h!˜s›wôW¿(nßû-2ÆPÑ@¾,úp÷J_—9œn¯än:d¡P*ß5ÓoJòYÝf5N’¼u¢‘éf*:ÑMˆ­U÷„¯›ÑCB8[˜ÀÄ¬‘k­8og´Gg’šJD h«Ò½”jwT•<?•{½è2fòê¾1ÞåÅÉRñ	ö8FŸ˜Va€wûÍð¢DßC•þÙAàÛµ:ej‡“3¢cÞ„×àN<íT‘›JŒ¶Ór›U8wQÀŸ£0Vþ=‚ÿ0g¿L¤ÅÐöXÉ|é„‡ÑýLà«T£rÏCÒÏ;¦CâÃ3b»Ò}g>âð¸KÀÃ’¯0ˆÏaÆ¢27·Aƒl?sŸ¤šL|RÂ bQ £Íš6Õïˆ-™éÌžž%(øð\(?Úpí„Z0[0¹í¡Ï“ÿÊ#td„nµr¨N¥}u_õŸOãŸ	®YS-æ¼K::ÿ†ÌÍ~’\Cø¼ù|”_YML)À÷«.Ä¼Ñ!/º…2°òž"Íˆ*ã„K ¿À,zMÖ¦Š|ž®Úð!´Cáíñ‡,¸!§ûÁ[öØ€Ð+µ	Û¶è>íê‡mÞ­lJ8Ùöo¬³÷4—‘Iâ'”)'=;8.¯–bxMÛ$­èïB?gW§µ4°¯vþ–½ÇM„òÆ>0¸~A ½òÇÀõ3zœ‡LêäÄŽm8‡6ºˆ%‰e\oðÔú8¦.ts5Ù”õ7QÊ¸52é‡|K5Rèå
šA£­%#W8~v„l¶Çh{Õ€)¿¿„Dlõ–o í:(Ê•VÝíá¸î3rdžj#£¸<Wøõ+ª'jïYê!Áh‚ Ü$Çi%ìû2
wwdÞ¬Ndä™Ž§wW$Ÿ3`—b¬nZû.l÷„ëEÂ—Åª
{£øTü¼EË9ìA	vÅÕgœ'M0“ä86‡›ªmiŸ!k¢ž ¡”vãbìP%A îŒ±Þ—îú÷hÎhÐ—Ó¢þ»'NI&Q‘ÝÊêU‰åmb@÷ÁÍz ùR´Bo~yyûdâõ¶’°îÆ) .è{ª¦+ùd.aÌAý£-Æ(ª#I¸‹­Ç°~£.¡x'¶!”µÚqõ†´’A¹°Cª'·Î³hU
)JP®Ae¯°_Ð‹ù¿÷cÃ›5mÂ1ƒG]c £¯‚Þª¾ÚàÓOÏts¢ª‡ÐVT/—€Äf£Ev»Ìå«]ég»Ûäì\¯ŽI ˜5ø‹°æz²©qM(±UYXè÷ŸÐÐÇŽh4Ü'ÐB–ÿŽ˜Ý¬“ä±]8mKîèb2*ñâKöGÚ³˜$ äsK¤'ÉÍ»[Þ…Çk@A†ÕÖb ŽÜê÷1¡[DX%CJ•bÓøÎ.ÐÉžÝP¬Áœh‹ùç™‘´IC@ÂEfþ C
%DóE/:Øa¼ˆ)‚úÏ’:‹Š¶ü”	íOÍIâ0O¸3pÁeÅêNå8-äÍ¸Åï|)cà´×.öê g/ªËh®egé]Z†>Øš‰5î‚2×'„™ìÁ5h*š¸õÂ‘T—d½Ø¢#ú‡Â;j½ˆ`ÿf>p‹™bñå°ç÷*qÛ²~p§ç×P*/ø}‡×x£c¶ãS>×¼ÙÞNs·"wRHh4î:2Æ£øA]•^È‡ÊVæR‰…[m´ëb`õvó+—£Ý,ã2$ŒŠQ	ôŠ\Ýpë»wƒ{-°›6ds»K°m(À9Æ3­*;¼ñ®ŒJK4iÙ ›5mí8¸C	®÷BNFÉO'12‚ Icw-\ô¾bGúæD>[TC2+xƒ-Ó’÷t»¤Éæ/™}“¾Œˆ•%¼õ»	ïw÷ªÅÒ	^äžba’ÏžÇhÒìÇðN¬R:š\AÏ¢ëÎ„MÈe
äý|5MÂÆ'Ì’÷hŠA™¤Ô¡ÂÔqŽgÖÈ?ÓôN0î5xìù`”ÏÆÈr†î¥á9@þßoJï[uC™µ7D÷%‘~ˆ3Ö¯ ibã”‡mÓÛúcÔ¶&A”z?ÄvÀ)dcÕëþp7á*yTy¤CÆÍý‚,†˜Înu²zÜæÜ|9‚·ÑbàÿH{ui‘‡ÿ±õÕA›´ù™Dï÷6’¥|z¥8›† Ü^U%]PÄÜHk?SXP®væ¹áñ|;¬M¯€tI iü$|É‚GÓ7› Á÷Reç5ƒJäTè2ÐâL `ð¢ŽŸh3-¿±ßú˜Ã³×ËT^mÉ}mu7TÿTa¨Wâ”Bôä³£Ñ¸h~ÖãÔÐ¥,Ç²ßTjªˆÊSãÊk=œz½‚Ï5˜,Ž)[4ŒXÔãö¦dà_ðíçtD4æ*E"ÍL|Ë'½¤‡¿EJœ¬ v›G Ò/Ç7RR@€vÔa¨Öî{¸alÿ¯¾TXp¦Ã²1Ì|Ã¦o“Ê¡…½³bv³ŒünZSã>È>öCVÕahÇƒjqË>½§ñÎuÀN×! fªqµMy•ôMe‰@ú&•‘·£ø‹Ûtí>B ëÁÀîr‘ˆÿ>cßÆ+P N¸ä¸ì!^—¨ÑÐLJ›DŒ,yþÄŽ‚Á°÷D®Vkrù],3ý<v'”Ûž¨È`Â^mJ.!¢ÇÍ§:6¼-uÕ¶Öæ© ³œ˜NúÁNpäêºõ”Þ•ëÓðÿn¯à”3}³¢pVáTs8ÞÀd=^.ÕAD¿Øµ€inS?‘Ÿ«ƒê­šmˆmº?¥‘+‰;˜fhÂÉª’7þRâó‹8üÃ»>›þ:;(1Ø5eïbBŠñ—lkm£X‚0Šþ=i€…:ê™Äý`!Þ1ä,ª.ìRÚÉ“2q…È³B¦‰Ž·ºš#´][N–O~¹aGÊ~M)ÙÜ?£Vêv6ÈÄÇTª“lË?">Cî¸E~\³­:Öÿ•P?íšêÍ‹Ù^öØ …ªV¹ø£­ÙkC‘†xþkB»z£Ø ˜ UÃ(Ýj1/ lR“*Í_ªxé^Ž´ßÃ”’Ø[„ iÚµxCd©_<‡ƒXŠ®
­Æâç¯Ž*9Ñ•6&‚"²žI_"’BÒiº;dc³Åµý³œC¥Í›Åó„C´
8ìíÇ]ÿz%qÂááTe¸Àœë›æŸ¦Cƒp‰À‹ÃJ”
m»môè´ŽÂ«¤OŽT}o‚Óu„1— l±ø Ä"èXê/=ïî×åÅ…Ò¢aVí}ìj¸±ìŠèb5	~2iBõG@årqÇº[˜û%•ýÍªÆÁçèÚ» 1…_º’Æ¸“Þ<50ŸâÒÿž¹ÏïT”zs¸´L_èÆÎ£KAø4æ H¯Ÿvî©²T…÷EÎ¹êÎ (Ãc¡µfÆK4X8ãûP%4ÄŽ¦¼£ãókË™HÏ+Ý_‹7ô&KÈx¼âz‘pg+Y³ØÉbòj×ªš¿/TŽ½€®ÞÏQMÒ°‰¹æñáv)-kbÂb;FpUÃ“‘ô_›àš!qìŸìå7Ë“‚'€„•ñö¡}5BÄ¸’{æ_,Úú?/G>¢Ñ)«gWÃ{–ò$ŽWaØ®:l£q/Ó»3÷<Þ×‰èØˆ‚G
Ð¶ÚÕE7Ö­èë²
`öûª6w¡µ«
i«ž—1^Wò—
Ùƒp-KrÒp£ÌÉ®9ò¬ù40\¾0á`’:í9o`WH…'>¨9ÂÏµ"©5q[©Šå•6ó7‡Ð!bÿ/}ý	éü›v®=de1º¯ª·HÅg	lgCßìöw‡#Ù²{„+¯½SÍ,Òë€S3¥T‰À· :öqîjxœ¿K]ÈÃ0/EÛ.÷…fÄ6,„o$}/â¹º¢¢U¼ÑèÒÿºÛð´ÔÀèõCyÀp-Vž$+›3s˜÷‡‹&j§«ÖIÙoŒzK«¬"kÞÍdb™5(3ÎG	7OÑïzS7GžVâ
qÝQ’UÊ#ÙoR-Ð	Å QÑ%)öy]4oäA	w.ÕûÇâ¢Iªr_N—~ˆ˜8§øfT {ß=Bñ6¸©™c¡ÄMºGÍ›õd]¯ÎmøyH]¶/QNââ-ªãˆœÚL¶{Šse¶[ÂšÇç)0|ñÌƒ¿éLô”ü®pæßsŒe†/®´Ml”nÝS3pÖŸïàÃgð!T!,wûüåõ‡&EQ†+â‘}ÍÇRÜfÇ·x“14ÆTx–6|I¿mfºS1
@‡ëkbêŠW¥Ux‚³z‚P¢½xŸ¸KõË+Éåšv~þ*²ßxj:<–¬a	›\8dä?úØÔÏM;)Ù¶ÂÒÓ]­a²Õd¥¿ÐÂîCô$£ ±1¶F)M8’ymÛh"™Q¸W1=aÊ·?+X±———m‰*bž÷SÞc’gìW+ed-dÜ³¡A÷z$N$¹„…›ªî3·Ü…ÍË·œaËf°‚ÃC4ìŠkï	R‡—Šž <N~[¼Õ…uU§¹Ùù2A/Uáá´÷ñà6ð:ÁUU{ Ê
žWí¹¥ ÝÝ_àÈ@ÿ§¿I»°åar{Üo*9—Ž>Ë–qˆŸœVv™ÛÃ¬xÝ¤YYù:mÃÔ|ô”
Ö?ÉâOµªšvø
fÀÊt;>Iòfð‚	/yÀóõŠrÔú4ðØã8Ì•$åh“ƒÒ˜/E3%ÐÖì»dÚ@(e$Š™I˜ö»E4G 4=õØVÄuÒœ$Ð¡~ð¿u qÞ„ŽùÞë6€·šTu¥<©R«D_r¤.¥g]Òo;åš—ðY,´`bŠùþˆq5>#
sbý€Üp×&E®ŠžÏ½vZÁ2^Ã¢Ážý¬>X¦†<Ë2Àý*yM·PI;#’Ð—¬ý“:Êø±ý¶ÁVµàµ\Eãy*xCj Úiî(PõeX;äßÏšSi *{îcýÆï hìº´XåxÜEhûQeî©'©þ8Þ=tFâ¢ 0b3B—ÇŽä¸©©®/Œ‹>…ZŸ?º·àÜ¦Î=ÄL)Ö—x»€ *’“ü	¾?’Œ{ó+[Ð‡»½Š6Ë}–ÿ‚¸‰Ù ¡*ñ½fðª~#˜õô7m©²|òä¾	R³gûõFŠ_¤;¼e)=ŽG´í4<–# 1É8ÌM=.®\8®ƒ1¸:º®¿|ñ¶šU+Væ*Ï—Å®Ó›P7=pì^d5:Ö“aSƒasöp¡q^dî?­KTà&t:²›Ü ßk¯ÄòÉë:ëƒ‚>äj£û2{¹óÈC¶î8eŸ`9ú:¤#D|ëa)á‡5óüòàuuÄMƒæ¥ˆ¤²Æ¼ì[¼EÑš;Ø&}y5îÕQAE¢#1=:pÜ“P	ÜC¨ÖK—fÆõb°/Î ”ežsÓÔfH˜ÙÝÑ#š‘°ù¢··—YÐs5“¤od'û¢§+í§.·Í#ûh‰k…{U/–+Áwâ2³›£%¼+nŒæø,œQ…÷~5ë$¸«Ü(Rù>+%tß%Âž,ô;°,Ê4ˆ_ÈgÓÒê¡ØtÄÈ= H¼¨%¸ª£ÁÏ|ô$Ý­‘H£t›‘»¯¼‰ %%yá˜v¿* å!}¬ÛúØKZ†w¿¶àô™Û(ÆÃƒt Ç¹À “*Å‹nä‹qÚ#,ˆo×ú61Ÿ6æÒ:î3½ô{Ì×`ümÐ’Ý<U¶`N%Äok´Ø‡´ß†ÿSLþ¤/†½ ž¹eU‰žh‚¸ç½&Yñ>û^†äÙKÝ_)‚(|MXÆäŒ¶×ìºœöÑ3DûëGüÿ‡{•Â=æ/R~FND¦îºµþ”ô%·÷Šãƒú Ã{
Â	òxøØNqM]ªÿŸÑ¸ö7+vA–âÉmú…n5­h—G$¥ƒ¡S•Ï¸ë°])ûn&?JFåtÌ~øj_d¾ù#2¸0³n“¹Å1…¦<ªkéëv~mñò˜q\ÈXÈ™G¢Ö	!aÀ