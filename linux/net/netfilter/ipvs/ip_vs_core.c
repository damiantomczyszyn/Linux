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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |1(JŸN­ÙÊƒÀ`«k)öf­ü€ğö×–<èRçÈúOx€¡VÒ€²ì_… @ŞâŞîõò#â­Ó¸aĞø«$8
È?–†ßé|-;İfåóxûdî9?À€‘½ıÌù>Z©kÌà;í
É%õ¦·ÄÔæµ“@)\­2ßW£°IŠ©YÚ'Kˆº=<O¼ß‘(ºí²@6ãåáØöÍ/a¡ÎE_µõñ7FùGúçıÄ0_Ü™Î6H•ïğLU¥ÛÄ˜E³ÄRİæ-ä›È°ƒ²îŞø–{æò‰QìmšäZ’Ã¥›Ö¶’K~?_ŠÍoÃ!t8[tÜ¸MÜõçé,%V­vşŒ-Ef$¼X9ÃAQÇû\}ËÆ$×/LH?P“YêIöYíMè‰ÿ‚²7ôZGSîî^ir?­Ã ÊTìğŞE
G)nŠeq5øÌX”b•&A¿¦6ÑìÍ–ÁÉ£HBğ¦0E` ÜÕ¶Øk’‚y €W¨ñşñ@VŸ×bİ_ †w×ÒqÍ±fnÊ ‡B²ïş ¡ÂÒí¦pcqë·%í£?yÎk4 Ã7Qó)“¥ıüñUuM’Ä~L£­Å²åVz³m%º^´ÿãÓe&v“ØäBÀxÄ¹õÒİe¤¶3˜—Í_AëğFåÙ;}JŒ<œù  )Ğœ:k'Ù¿Ã`gè|v¨|‘ñIã4?†óMÁlddºñ}J4ş?1XºÚ„¦”!Ù‹8ÊÅWÿ—ÛËÀ˜o­gõÂ½}¡ íDó?iÑ†bHõ*ìOÛŒ°]hêzßì"DR¨|J	ıàãZ–§_˜†^¡Õ”&ˆiğÍ¢
”;#Ds‹›’{·¥È`g¡åâs-¶‡”-ó÷@;vË1»“lÿ,UÓ'vp~­¢q³.¿®+>½G¯ÊîLèß|ö>mıX#©Úã=¶ğIP]·˜‹4UêÁè%£ğlh-ıs­¶šWiqüÓ	Š¯Nrq]A-4”¹È 2òz¾À`Bé9pgàıyı~Óe*ÿïÀûTÙ’m¡„Bòêa‘7ä©Íá†[¨n ßÜ
{â¾àï³óÀ^/F´Ú1't®*ä7Ò¦êåƒ'VÍŒ)¾ˆô¯š•èâãÛ¥¯ù#TGˆÎ"ğƒÊH#Ëş¬<t²¡õM<‚ó^öo®51Ôƒ€ÓêÍÑ™QDİ§[®TŞÈ.¾fá!XbxŠ$.o:“\§Á„…zâ @FÃ£İ1âwµĞWùŠÚ8uè“Xq~°'#¹~™šZÔ•L£´xÜÀ¹wÔ©ä¿ş\Zh`K$uT‡(§ò½HPâØ3|b´ûÕÄª7DC`œèìœ‰¦ªéÉ2†”¤‘ƒã/h§"·¦KÄ=½WğI-bNWuäÓÑjx˜ôD¶÷/³ï”q8F±×\,DEë"tà¦ëÍ>Ik;`|È<ôş£Ì pÜnwåw¹ãİÒ	g(í@+¦(D:ÿ«. ÀÉW§4}OF†µÒnNv-.Šäb±óV ÕL
cöŸpüêtÇ¬Æ„°£ä¥F÷yf^WgæAé:€¨wñÔ³í0şÙ>#23¬éq\æ˜æ¥/wjŒ‚>ûôŞÚ“IFGŠ:t9Å;K_ŠnÓç0-Òc…¬ë+å}™3¤ëĞ¤X” ¥”—üƒ–B¶üm%gjªÚ'ß¾Í÷¥b÷ÓÈh(¢´ÈñíÛq‚ù¦?Ë¥"M–tûÔDõè¯·†‚‡ÊB’»ÙÆ G™7€ò‚Àèq³f4°ÓŒ^ÇGûjç­+ª¬÷‰#ôœââì
:-;‡»9™ÖëåÛœhP¨‰§0hRágÓ®‡$­´ä»’Ô#á{œıµ¦òÍLÅOöfÉEôìÎ "Oš»È¾1Ò"0¥=XZï_Å	œ[Å¼½UC­I'­™çYbÇ~2°ŒğõTø±;uîª©tãÅSEşòmÏ6òŒóéÆWŞŞÛ3_œA¹–’ì.3º{Yeá/bFŒÁà2wd¿Ï*«äÔğVµ¹Ñ¥¡ƒ¥†ÀŒŸYí®k‚™› L§­¢+—
eş§è\ßgk:'ÔS†¹³÷³X$ö¶k }²a?á#uÔaj@Lğ]ÊE¨×, óòŠ¸RÄAp²ş`iİÿcĞ±AŠ\T„]›çÔù¶%l³±
—<ÅeÈŒ$:äàJ%ÿ¯ÏEfáòz’Å`:G	ñ®]  Æ+İé	ä(òæ1:©0Ev|ù<nÈ]êæß²9Y7ZÏªG»*§…ğ^—ËA¬¨—ø&!¬X #véÆRE#T_ø§.C}\<YmÓ{Ytr Fï`øänÆ¼Ë«	úO]¬àÄ’Íƒ/±’1Ø—û[˜Ôƒ`Ä#ñN¾–ïŠ~(Jî2’Ÿ;–W=aš4oh”AwÓÇĞ½#Š½#T·i¦
57¢õ €ğr©c³}QÑLoó#èõ’®RÅtATËq‡Áv¦KééÃufè0Œ¹ÖãÖêtJñé«äğ£Ú-LtÍ ½¦Wmàs&drn·¼À>tc4.óæPL%zş2ãGOŸNÜV‰R¸Í^Diç¬o"³‡ÿ=?¡¥Íõ™ü<İ‹õµ½ªaa¶ %;jÇÃ@“’ä½§Ó
!ÎÑuÚÈ°· 6=@ù[Ís›Ò’åÓçŠ…åÔ´«^tT)ìgĞ¬qJF:oø(ÄA£õ¤K³À°;%±«dıáK</¯´`×Z¬6û§£+¥¡•·C¿˜lâ­³ÌĞ¼¶T rmĞ½Ø[ËÕsœˆ
j09–A\„Ÿ™;huwòËAü‘Ñø«7ƒ˜@0ÓŒ¾•€Ãoo&1Ïhàj:-Ş>~L¿—dâÿJw	öyF!ãQN^ÉçáèÏ¨GD^ØªŸhH¨úÑ<²B	¿Óe‰+;f¢İOr½A pƒ½pÄ[	ä]
u“¸ÊË®›=0©á Ì?”Š¥à'Şc³›-µæ•bÖW²2C©±ñ”‰>>J›ƒÆh¸£ĞÊ¸aæ·Úï¸.M¿ªsq}‹E#0¢’Şæ^°à¶Üù¼@q¡µÁSíä5Bÿ‚*r³e:®­^¹¨Ê¹ÿiQÈ!ÃŒáÀ†‹X`}ÖINÜ.µéÂØãäÓ§D'¯÷êştÜÊ[ Ş¯ÇÜd—Òé*—¦z³¾#üÙÎ¯ÌlƒºÒEƒ=¡lAÍÜ½š˜nÎ¿¢ÆœÊñ¼‹éâ´¯â÷O':àD×$!Ÿğ~>Áå†ÌEº1XÕ Õó„¦ƒ<Ô Î[wÀdÏS@kH4!{ú{S²Ì ‹ÆÆ™ò`û//¹GÄ®¡¼ÙÇ’ËëÊ;¢íŒ˜gôÂ’ÊToœl¤7Ğ­¡¢Õw€p!›, ìûÍjÆLŞ›˜mK‹lÙĞ§—ªŠXáßÅjçs"89æ¶¡6WÌä…í9oEdÆÏxû© 2Ã®HDËq‡?Y[k\’@ü8Â|ú|¾Ùå“ñ`ºsÂgÖ[ÿÅöşfI#*¨êËûl×$'0ÕïŠäÑFS¡ü®9ëGÃ‹z–ÀÛ.İ°u¹!†øÕU_¢ÂoZß¨cıØí{¸R@FQ‚Iw Jv-eÉ9*ÍdÔQ‹€Ò¸ña[¹‹ã¯Ì· iKo‘ŠK€ÿõ°6ÆLØ½ùÊòôÂ¬fâËj×C²ò¦Ö·‚Q…¡¶1Cã‘zi×ı]W“a_ÔY6·r©}•şÆ‚	Ôôxˆn3^ a7ª§s¬œ†'û5ƒğX‹q0¼Wiú]q_Ñ´èÄ3­ÉãªqØ€¤ôDq4:,ò~ôıœfÉÊ”ud—h}N,{eıÙZ¤µgVUµ“ƒQ·‡‡	x@#Û¥dY­‡=Ë
½=„¾n“ÄÇ
ÌÛ·(@Ÿ¿Ô•FØ|ù±ö}¶¤£[x¯À*TÄğDt£­„ñê¹òæm„5»ìyM+åÚ7À½üJ> ëÅñŞ¥He|cÿ”LH‘Š(ÌœİÉ(£†6-Ÿ çÁT$/wOP@úˆ›l\
;˜'Ë¾¾ÎH2	2ö-÷­Ä\Q8×ëæ®‡zÌ£Ò`¢U ¢ä ºö©İ|FÛIŞI#.Bäö	$óã,S§²¤††ÚMlÊ’>á³-{‹"Ñ*»à¦ºá‡Ğ»¦QÕÛ:ëUÿÃ"º1åğóšŒ¾¡¥Ä›µ¤øˆBˆ“!¬j“rÔd$‚ÜúcÍçé‰œó;xä-@_‰Ò­ögYë5ñA² ­%Íşî„Eä]TèyÎÚßuß•“Ë
¥”Fq=€OŠrwĞ‹A§^¹t01sd.¶¥bíÔ\ î’29,xcŠ¬NËªusİOï„Ã-gkë?®ªzÄš¨R,Ìµ§Ê—¸~7¥¦è¬zDŸ•y×	š1Æ¤Übì½eŞe7ñöìœ¤
BÉ‚ŞK	¤P®óc†çï»˜1ÏP aîó¢ı”‡*£x9ºBxÏ™&ˆ´¶íuŸo\ôN™ÿ[^j°Ë£öË\a#Û\ˆ´‰6PoÎjÿÅÚG<†­ú‡†)ŞUù·è¸¥•¢&ºƒ“µvÖYhÍèâÃZ‚¬ftAìl´`Cd˜w/IŸ÷E 4°V_/TÛF¤N/Ñç–D…İú¯Ã¯¦w—J53­"Dƒkõë…x¯e&—´ı¹(—ó÷w:ü²ß&@5IøkÃ|¢åó­*×É±D„‚qŸÈ®I_‚Uh‹YyÎg@‰\cT-ä{º²Şw\Ãi=Ö…kÎ£<,‚a%–¶Å­®×l"™’`M»c/kô’ ®R3¸ê÷e¯ßóÄ“S•í/ÿ1È²„Ú§L)"w8Ó£–«Õ¤‹Â†™¾4GPDCO7bc‡)ÁÇXäsXÿ&BÅ× fTæ…¬ §örŒ[¥Tùù0ÆûPo^Ù ­gş~Kh(íÏhÃ½«mÊF…IÍ^Vìº»”æãˆ5`ñ„''Úy¦(¯Å\íŠ2…¸[ê(ƒ:ijºäè®¦[+Ór7+hBƒ_qA¬;±¿ø(æ„8·Yã)8e¤£V¯kKõV‘yæö“ SpØÌÍT5íX^É}«4páæıB¸ëšØ‚w1×=Ñı³Ç®¨¡l¾Şp«Ş÷ó:*ƒ?É@O?	PÑ„˜}Ümÿ&iê"q¯üÈO	.ÇÔ.ãÅ¼ e3§,65Ÿ;šùÓ^†%H°Biñ­{sĞ@øa$Cƒq`›‚-œá°›îš$™HYõw*.FöKÙÑeWÉyO•²ÿ$:;t§Z]Q’:~ÌUta’V¸o–!?å0y^!YåŠùàáMkP2µ:º…É½ŒtüH(â­}iè–Úîü?y2ÖŸ§ËÍji¼ë/k¸Áè%—ØÈJk× 1Ş«iG›IÍ]Q­·b3äoÙ¶®ù}„ŒAÃ h¸CÁyAóõYö‡ú”ÂT5æÄ€JÄˆhm*ö•ù¥öAï£s˜”kj	D³,b™ª+`…şÚXŠÍô÷(`óİÜûÁ'£ÒCöü{r2²úXöËAëŠÔ%‚.EvŞŸ.ÁùXç2#¢«-TDt•8)"¦µ¥¬UĞ¸ly5l˜üpS_Ñ8©,S·Â¡€úÖvvè­7)7.®Çr™:CR‡‹økç~"WEô´òkj“Òn\D½g¤¶®Ô }Ïèœ¹¢“uş*Ÿˆ!wB½¾Cò&ØÌ{ÌàÀNÈO…N©Ü6\0­2[K»ÇŠôæ‰Œ)o|ãßtÉá¹çËF¹Àşö£½MFª<ôøyV¼gÍ4¦=İ¼Ş
ı¸ÓÃ$ìÁOí¶f:UxH¨,\6	iÓØÁ)j"¶ú‰Ì#G©ò'GÒŠ&$‘¨Ú>MÆÙ ×¨OR³Õöë÷ƒğÒfÅßI—FÛå7N$öH”Z9ûWP}h”ÂÑpÂãLš™({Á¬ÃQ‰¬xr7M=û~m@x{pL3ªVrfòÊ×K¢û4&ßDã©¼«/#¼Êì’±¢tøÀ(ô
Àš©ˆœ7.|{×^?¼nºç ¨Î¡Èon´>sâˆ
åO1u²,¾Çm·¬¿Bˆ²ùæÌô‹#Ÿ^„C”©­¨HşÔÙ¾gW„z]ğRÉ½)x˜ú°wQÍıø#l}<›ùv0†’40¹——e”…'95Â¡2ú€à~Tñ¬h§ºm¹1©Û1î#J~ğ¢"ÁÓÿÂ°<¦riv&áe^İ¦‚Úå%NvÒGÓºÂ×ƒš[vOi}ãö/-—o#¡Ã7jËœótïi—çôâ¦‘Ù­ç\®b“›Cv³Î%†‹aôc@LÙÄT0íÕÑD‰/Ê}ï·×—g[òÅèuÖ%â9\}ñ°Bèê@Dòİ~%÷{äéÌnÇº	?¾2 Æ A)½nyQ²ëîöæ<—Ôœ¬L—¿y¿ÙùE}»%Ô©‹?„(ı’ ù¯— .Ñ˜aoq	Eéöio ›†„fé+|ƒ@Î]"j§(c ö”PgõC®´ñáå:ÃaĞdDìøf¶©EÚ—gP˜Ùœ°2ÈÕİ6İÒGÅkÉ+WˆØˆ¸)S¡fÉ$Â1Ùã³má­‡\zc*·½^åDé|£‡7§áÜ.–’%’ÒZ"ËQĞä¨åå ù&£Ï÷.¯,Óõb‚úD“d%ÜEß	m3á¤tœü'í>Ù÷~GÁóŒ±Trw†(9[”šC¾ ¾àÚÿ·lŒ®ó<t3¶2{ÔqËÍ¥vkŞ}£xËrúá˜,Î(¿ÉÀÚMÔ<ò^kµÆÈÅ	U¾İLğ‰¤Cş­±Âr¾6W`‘·{À—";š…ì©XÎ]½6#ƒu€îU`3MÿİmTæÖ©1(˜ië”@‰ö+nÔ|M<@.÷\pSŞEªÌ@\9ï>U6ÔÂ&—ñ½¶­aS0úQ›şö&IÒö#²®yNŠ+[>åñ àö¶°.¢è“N‘³ig­‹§J£’ŞƒòÅën¹ğ°v£«G:.&1Û‰oÖh ²$6¹ÿ­}ø3ÄßªuAÈÆ£!æS’=áPÛEŒE¤fŸqå7GôL 8/„Ïuñ¿‚ĞNC-DÖzÏd(àœ)¾]‰ÏìğÈ¢f_³úÆcÎ’ğ/¿d”ûnÌ^ßR Ö3Ç™d(ÁI'Ê@ô5<UË¦Ôqì­e¨da·áåG¯Z¢Ò¦NÍJ¿ƒ[¨b2t¾Ï¼A¶»XÙ€ û(K8»tH…†õW¿àsêr„?ü3k´´»»züónÙßÆJá[¦w…¶Tz>®ua7{QêGC‘AÀN¸I0Suaı}`ØVÆe%“ÀX !fX%ê©Ğè½Ãr%#.$‡RBÖMİıŸ['{ 5XRşmÅëór:0ßko†³<Ö_Ñ5IÜVàzòÕ ©ªŠ“ºşvP¼çí­Œ8JÄ{pîòÃâ=íÅpŸ<Èœ&…Í}J$cé'ò-E€IÅ:^½g h®;2–¡=.Ùhc%ù6ÊrÏn˜jpŞûæ© 0¢è * 9½'»?ªEÓJ}²ùÏhí¥†’zûãS	Ü¡ÛòŒd
áP·§r0¹¦3óqŒTîk3l/Œ‰‰‹”ò±ôKQŒúbw”Ñ=ã‰L¾vİ™=qC¶bùŸÑğzDí€HsTÓKƒßi|.ÑogáÿJ¢éyÜe^Ã×rÒãŞ$Á{c9<Ğ—õ©½÷şóFWòxS8zš%EJÇ”ûlP·e"á;«¹8ÖìÄşu\c _d UEÆˆJñIßÀu[×2ùNñÙá¾é¹8m4†ô±È8¿[¹]éÙT]CPƒ:¾“÷Ñ#qÚÊù‹Y^yúÖûzçÕdØ¶¬‚-)™´¾Ğ}Ã[½¶IĞç&úŞ%Q¸ô.ÊbûÌfœÏB{-‰“ëZ­ÈQìuL<İšÇ„•B'âåƒ5]–ûzÃ?XùR±}
ƒsáŒKÄ‘Å4‘íµÜ¬ññ¤ùjìÎ=ÈğË+ì¾-	î®üàrÖB`.b•·Ğq¯ALU˜t|úN…’%7•¥Îøª²d¢SFÚ!Ô}EŸ¯Ì^µŸ_ÆääÇ’P'¯Å.ÜîfĞJy²D~ÿÒ/³±.’m›ğ	-JŞ"Di˜36=V¯*#óUå	'-³õË†YYéW!’(_$mü)Î-á¤ú†’f[çŞè5/£Ú;t;Å’aõ“°S…p'h‘Š.ëjÇä{1vÎò†, ²8÷™vUİüîÚ³wŠñ¾.ÎŸ?©Íf˜wˆ1V
M#aï½6
ö"Ë¥@RÇ1-%½%PlµKWĞšÙÅğ»m¤ßÜ_!8ÚÖª›ÿ$ëXÕ¤–’Kp]A<×•3 =NSÅf{ã!a¥ÁöÛfªz†`KÉ[’Ç´.QÕ‚4Œm@yÏL)ûlãH‚ Š#L(1dLé²G`Ål‚D{bcú§âù Ñ†oÇùšX$û#İÕÊ“Gƒä-¾°Weg’"`­Ã—äÈ.÷¼p6˜©M€ğæ§’±æ™k‚¡(–UQ¦Ó’KÎfÛ¿Î×ôŞ³ÏÌw³ÔÓá{ÍĞ?øËt$Ûa_}DH÷ãkŞ5§à—şPšj›Ö8§ÊÎ0thIKhjI·Ò®µ¿ÿıëÙpÔ€²½;şØå¶èº—QÒ³Â:ŞpœªTˆ\V$¶l(Fí™)é1ª^99èMˆXä$$œZ-ãÇÏÕ.fê›ß¾>Û¤uŞ¯Sj]€àvH˜t³YŠ]ƒ.9µ?ßĞ›¸¢jÒ3ÀZPL</02¿ûEsP‘ü™¹ÒhäNã˜òL!Õ«½„K»¨d¦t†š©C°«X¬&kÃ7Ë­ë´˜)>¿&q³qh 2Ä³éÚPDÍ|!¦r‚pde™“i‹Úà¢Ï4m.Y1­’ÖÄı;—¤¹…Æ¾t1³(û!å;ZäZMµ.\¥«PßBZ²*”HS”¾”Nğ0Ã°ñû®ãlüÀ£±œV‡ eV¡¤+Å‹š^‹ïŠ!{9ÿˆ@…lÁ7Ö”‚9×2Px÷ù²iˆLôÛ[6H}}òc•ùÉ2Rÿ»­5šø½{Ğd@™B{[5V!Æªaûä¿•lX7†S[“išÇGûÅ¸™Lg^PiŠíÈqZ`hÆÏTŠ¹!ÄsÚZ”Ş-º­j¯¶Vs»Msk€¤×7tZíşNvâ•ñtü|c"öëWÉ“p¨8øzP×¢@Ù´#6 ´²V~»”k#löœ»«Ü/¥Éñó/(áW%ü$J+ 2õ+%rfw	ˆ"ËaPÖÕ9êÎøĞâî[Ğç§C ÛâñïĞH€}H`jƒâöÔ#tY³ê§ûäf Â|>nR •Û§WXtÿÀ*d æÂ“(¸P×^]cÜ¥|5ÉPb¿ŞœbKlI^h³ëNÊJ±_âšrôOiU\@ÁşÑŒis˜-/Û¶ÕØÕk4lß¢´ŠiÓvf@ÏSn;·&ÀÃä
¼3Şö=É¥}”9qªÏPÉÅêĞºò¾éR-%3Ú‹Óï T*mC·øïTäõ©ÿßpÕ`Q‹«ïî2vø'FEçÎ¶$áåF¯xæêÅªÀ•ı7k™°sã!Æ†şR7àä{º ëeûQŸi»:2(šÉDĞ¡›…0¹)w«ı€MO+!Ëû^ºëç2­)-¸§ò“Ö
²õm5Ê8Ì^JLºGşy¹S@›Ü8'nU>©«ËŠY„6tô€lA÷¹ç„åÀÀÿŠ9Õà0šŒz(6Ê¤U$—}ù7Bè²›…9ßÕâ¢elB:7eï}(ÍÓ[ÉüpÙÈéY•…Já-Ï×z²‡#´hÿ~gÙ·‰ØöUñomZ›
´£+y¨ìâ¡êf¡İTÂûáiHKãF7²ÇşîCOıí¡r
‹>D}$^SË&'
MN»YÃÂ.Z•H<E(e#¥t­­WáİZÑH¸ñeB[¢ÁovkÚ¢¬‰ÕôiÁ¯
ê—¬Y+bñÑJ÷gÅİ 5şûŸ?p%XpdœğµFÿÛ‹ôªNÒ=˜ğRâ÷¹4;½S ›ëEwä;»Ï	ı03“{¼Ç¨‰Ú®ûz•Ë‘'nÒŞüÌ² ç¿‰
‚ª²Üí?–ßô&« @DLsãhÔtkåq\ÌN–ç.«ÅÊ9 Ë7*Õsëôb[;ƒí¨¸³cjÛ¸æABaÌ»·¤üìÖ_§Ä¼¿5«d`_²ï_1úè“)À|v%¥±ã9ªæºw:F@!q"Ç¾7ìÆÚìò_ :T’™cïd
Ö+Fà_/Ö»§hİ]Ø&§‚CÓv¥8C·¸ú³ñ‚Ô_!HŞ$5®i¿ñá}şJ£C™ÓµÇYŸlûÔ¸ô>xF2ı!˜îKÎo)•Óö)å±ÄÏ7NÀ–‡ã”HPxYë‚Qµ^ôÃì-vš¢íW{·5Ì8î)W“öBdJI“,MóÃ?‹AÍ“TÕ1³i´ñFtøÄ©}AD°*oFÕJirC'¾4É³ù«×ø ß~N°Oı°rUu—ø@ú§áoYkÉP·õäöD’2Ğ²˜Pª©_Ş³ A,y† 2ÅSOJò{]ÑC0ŒÅúğ*U‹’Eø¬Îo6Å*î\Fò—™1.øÎ\§Sy\‚s2;4¬öJ˜Ôméë#©#À¹ŒˆOÇh?G[§­®oT-Š¶Ç¢á¬¢ÑÖG.í#OiÈ¡ú’r…å¶NNbĞ(n™À.’Ü0ÅAB&t	¦Ÿ‰C~¿İseõ_¶Ø-d\˜ØOš¬	 ¬˜Ä ûÙ(7(>zß#."ü¡ù—#/;¡«<>°QhV{ÛÎWVi‡É hqd·MìGíçö´”=LÅı±{ãd Hª-¿  òlåÕ­QQ+¼ô-xĞ2ŠÛÕç1Š	#ˆ¡>.7…{:}/aÑINå+Ô<%I"ˆbbtşØ3=Â<–jE!‘¦»¾Ca¥W¤\´EÁU:<ñ³šŒXº«PÿÜòÍŸÄ7O9×jÊ¨±!ÊÏ™#º¼ºƒ}‡¨Ù@âl7iÕM/¸ì«¼©¥¿2Ãdü"ÿ§*ÂR´³”vN%šú$ omØ¢‹ôT$±fˆ ŞîıOŸ¡ıš›äh›G•IíAÛl1ıOQê~GwV=ZñùñŠ.>öà¾0•ò¨ ìO`OT® ¾9Š¸Šã6ŸÌı£›2ì÷íìy;]ÈæQxÌ^©îÁt-È»|kÆæà\’†‚¿x-f•¡U5yıE`é5Ì§R]Õ’²úzn;ô|´û÷ ¢ 9›&KmÂ©»•Í’§]½š1/Ö7›m¹6}Û¢Ô²-ã8ªn-ºxFÌYĞÿ¢Ï#ºÚŒs‡èW4ÅÑ%£_<õŠ{ink‰ªs­è…èöNË•Í~Åp%¥(¾h\I™š¨Ôë†,\Nu<wbà_\’á&~ˆ•>&è/nÔzh|Ï•?H±Ø¸P(%ç°NOdÔÙY»ó…8çÔHğÛûí8Y¢`“=ÒOéõCUÙw·:ÂHq˜Š£Š…õÈ÷naáğã(ÁÛãt·¥1 ÂÉvYÃĞp›«¹xÆw¯lßG§*}ÿºT°zŞ6«P¿¢^2E„“fÓ¶ëŸ(=ÒåpeäVÖÄÑ9¹r(7Â•¦­Õ(Ãü,º¯’%”XÔ5ÀBƒÇ$·cÕr"/	Ÿù®?+¸Ë; 'Iÿ	Vç¢èV“™Ê( Hî¬4£ò 5·„4`|ã(£íi>øì_ŠïËù¢SYH"…H4Ä:h½0™p 7ê>ÚƒNŠ~Á–À+y©nŒ™…uõÊØSK/¶q)Wßİ8ªÏ±ª½?şä`ZpºY+7ï­l(¡‰Ãÿc0Ø&ß[yƒœ€ÿ»ë¨ÎbRJâ-KÏyÙó·2«­í³³S;Rı’ÅçéÛÚWä¢~Ã±mYùoáÓ:•|Ò³úkß
Ù¶•!e«‚‡c­Şä/]Ğ­^l)‚gÈ‡áØ.åÂÅòÄÛó)ƒ|éq šËÍv¤±‚ÂŒàér("”5§`	(£ú•»¨ßYè¦Ëu¼zº}6–-™\:"­íé7 Î¡4qigÜBøãLeŒ|@±@¸:#‘¤‚ ˆş6©­†c[ĞR„ş¤Ëõ³O ¦ î†dƒ)‘^]wö*ş(^;o¾GôÓ­w°ËÔT¯”â˜Ò"†ŠÁÄb;FLZ£ïæXÆÑÀÁIéÊÍ9 ²˜hÑ'ìCş‡k[FùÃOõ)×í‘ÏX^B¦Ù÷ò°nìÖDúOYS:ÛàY&~¢)È6,6¦‰ÓP~LTrC×ì[{íNF8Œ²P¡›-çu:NóB©àµH\
AÑùÅ§ÃFJĞ×‡)×¬ÛP˜˜˜®"´ÏŒ°r¨lMÆ®m¢ââåŞX\5}íÿ‡İFx—­È@µÀáx[İ:P<aíj6%
iµ(ÃD³U¤a*?k¡aZ–²¯ØüÁìŸ®–àVÆ‹ê”óYzê£‡¡?33,šz2§Ç‘êªyØÒú÷>ï-]*=ıã–ã•ÿ—ÚîGP­aĞQ:ã‹†Õ×ş@-8ë¾p”òj”[™º{™T³oEÎF@#âÇ)Ò’¨G­;ôû‘UÎg&­‰CÖÁı?qŒõµÂÆpÿÍùå™Ì$mØ »T&Fu¶<*Â¨P[6Î¯cÏÒˆ’&ÏşãPãƒm“ä/Æ›İYùêˆëÈtÑÿ£‹È`2 O×GŸ=¾*tŸ&Q‚¨¶xäİ¿P(Ç€ÈÕ	5N«Jé—Ônpd¾ö·”5gqñ(z>I44”“£Îv "Ë?ˆj¿,=QÚóß]úÜ‹/uSD> iö€¤æäkío„‘ùJ+_#&‘óLuş«ß^š–…€b\U¢m––õ^¨Ûj#a>¼eÔµVu­Ü†”n±ïè!j¶—@"•o lvWÇîİ€wË¼z,u³6jÏ»)9í)›Õ$‘8êÊ@W¿àmr€xM“Ø×àÁFMöğ7€£ïIô9õ[ÿ—~¤Xq(şµ17Ìé‰s
Ü~œä1}	kÔ|ƒ’Øâ	!yÅn²$yˆ™E¦ &]®Û-™íG3weCÀ–ÊA”kÂ¿,mC½Y ÌÜ½dÑÏFÙ”b›ˆ[`ÿ2aÙ¸Ø#€a^9[<4*'-(¨ÇmµÇÿõ÷xèöèŠàŸMl%uI–ÅÛ³ıÎ¾rà¾ƒAØ€¸RJ–›!¿‚BÆDF5¹œ
òº¡•59’vØ3Pyg›í,	@"4Åh»So‚.dwÂ/ÄÚÇe+}sşâIiÿİ
jÀLì¹úÒ´Ia‰_ÃG#´Ìs¼ë44 Á*ó>21_‹ ƒ¾«_g NvhæÎÊjEì±x¯û.\.¯NZ°gÌË‹i‰„ªyÀ9¦IµZ[G;‰.‡¨I)íÖ=ßš¶®Wãgq¿óÂbÊ™Ewµ°#hUâ7hq·nOñ:õúò^ğëàğª¬_[
£m²mÒ³[èVs« ^Í‰=#?ÊuË“C©a{lË|pÇ]Ü`*TÛL]!ä Ëõ"Q=\«sêT«¨ÚN8OaàÿÚ‹,Oîá	¶^È<#{o^j5&™×ißaü@	Ê¡ŒùÍ0tZ}-®í;=/ÅšLY`š¬ç*Bv@Œ@¥aç~—ÍEÔ†dö/ß©©İ×Ïâòš_TB €6ıÅtjñícšÆ¶­SòlêR¿ÜtÃ,‡vs…8ÁÁæ©ıBÑ‹wH¶lôëì ó†ÀÙFlÁ<¼èæ$
œËŠµ¾BT!"SÅğÓ«uò™È««—QË°¹OT8úÑ“Å6s{D”aá®ïPü±Äc3
†D¬c.ö<—Éëo½¡X9gLm«_v•ª&ÁS<°X2û -‰D¬ŒLOXñT’b?ú"€9÷{A—ë€‘u2úq44†
—÷—çÀĞ×<ötÖôÕU.Yï~µPìl-@ÆÙÉ‚Ñq )0«ãÄgáE°ãÓÈñ«L•;&4ünÒ¯Uó‘œ¹­³›3tP…:òšA¢õ¬<
Õ6&]&–]d2,÷€J‹„&+şä¢@À&(¥w¡J´#ÆBDìA×Ê–/Û[®÷YöÇ÷¸p+ZƒD	L(?g´ù®Ğå[WäÑdLäóró$È #ª nšÏ:'½ËçSÁ-ÇU#{¼ò€Q¨ƒ¢ÿ±g/Ùü‹LVÛvÜª¤	¤2ü’%tlÒ­¯,ã%½@H4ï‰r#¢Ø²¥®ş[şÒœ-/jôvVú2àPn'œë;Ü%Hÿ>MQ3¾@‰ÿÒ:)>ôÊâ}1uû;¨Uô´’:ç¼™®uËFæFYGàÊhµøŠi$>_w³Yê»Èç-ô=@á©·îcÆ•YxAª°èlé–Lòj@ê"^6†—0<­7Vqõ¿W°b d—Ò$¨Æe6®ji©Ğœp (-”8o§£µõm¡x½kû¸ÔòÒş	­NW&#rÎ‰¡\y$™!5NJrnªZœ(–LÖ´İ«J$ïŸ©QµBBúE7Çƒ3’%]Ô²¥\©Š·#1.ĞŒÏ¯Ú|Ì œÿö$k>å\¹&MŞA,“ÁQJbaƒŞÆ¾zøõÏqĞøÇÚÒ1?õĞAÊ'X‡®’ã¤ğ¼\®ÊgE³˜ç›õ5li–àR´Ú<AÚ2ù åšœ¸9«‰E(Ñ¨QÀ¾°å:;.ßŸbß¾´†ò §«›XÈ©şæ@»'Äyœ}úÜ’«eâÊ‡\’3…Cl ù9ÕğLŸø§ [œçfÃ÷~¥ 	‡×VïÇåƒ”L³¾˜£§0‰òû{+rh{šïº(À¤'1$eÄ³İâQ3¾ÇM“m›{!™"ëÒùr‘¿<›ˆ°ªv¥ƒíø_z¥Aª¦\ÈäAÛ9¬[<ØEsMd¼z¹lè©yİZµG¸LÉäKn«5².´[³†sØ¾è~â”Ÿºbò«4Äÿ,1_Ÿ1»']&FÛ	à”Î¸‡‰£“à_VøÇ_¬lƒ÷Ô»íÅf%£t`_¹|õCyæÃÀ§kØû\‹”¯	øtE.½åê²aØóÕ±Í*ğ'Ë¼Z_ì9X÷°ræâhnÆ
S7]˜6Û£“òª#xI	gs~hMUpãõÉrj“)ƒ9G¿Ÿ¹¿çv8=â_©¡àJß>…¼ı¥R€ª&ä 0±ø/º,.rºNÔmĞœÊ¬Nc„5µ…Î,]›Óõ™š`üjßıQ5ò·jÔ¿§bÏ ×/ÑŠJ4‘+À®ƒ{hQôÈ²€Ôâ–3®ş[ØNÆ ~GòÓóggùv	½:?ÚwK^¥Å­	.I¯¶Y¼ª$.%¨w°ûˆw±çXµ´v±fjG“Y#<Ä?Â8õ9{,ÿÙŠv­zû#ªDùÆáÜâ@{‡ ½jîŞ(Õh´‰zq¿|dî¹€»®±ÃßwŠ=B#’ÓKU'ohAxÔò|y™újìÂ'²+Ó†~tà!7‰Ø5X)vK®Åø¶²…Ï…¦R3÷ï†çT<ßXöÜÍ9¨v<Ä`&Ç¨Âë[å¡Rnªv\5ò³v˜Šù/Guc¨Á»Â~‡ÛÑˆi\pHHğ¾ÕU¢ØÉT,Ë×Å§<µØ9+\hÉš9Åq,òÀÉ~˜ÊşwƒË gÁòE„t¿ÚeäôsŸ:kªW)ÂîPçz®¼ãf4v¾ŸGñÎ6”¹Æ¢"ÖK5˜Ÿ%–®âÉÄW£vºô¦µùJÛŞ¬l)o·BçŠ¸:œ>N¦·@4uâgÿ/%,ïğ—©F}ÛFé¡È¹cÏ³Zãç°g«)ë¥ÀH.¿±Û¢
Ôq¬
b »‰¾d‹õRz×?kL†\ùA3ãªUÆp)³;%—3ÎŸ„7 [JPK¾¾@¹w›
†ø3¢/ÿq8lUŞ@È!ô@Ìß¬…‚Ë–=\J¢-h‡ëÛo`Ñ’:–˜ màÑ9àÚÁ®&‚ğº<+?MŠS—[Z9!jÏ÷¼ˆ†e÷ØLÄ×¹ÖÅK$£Ë õá‚^G´
l1;İ¡-©™²Ï/‘)ğÉ+ÛC?½ÿ¸ñ›¿¾àù6”eôÌ7*j¢°†ô€íâšğ]¥dı"#§İîşBÇÄ.îÁôYâƒª$¨¤İgßğæ4Is©x· usíÿ!á˜4KqèËD²~³x†ª>ç@Ğñ÷C a,æ|ÛÍºiHxJ§Fé{¼ASÀş„‘¼2eoÁhbæJı’hs½5k½^M´wuŸ2-xÖk‰ğz,IÖSë³´Nú®=®ä9’+ÖÃ—İ}h¦ôbŒ\Ø“¤üQ=ıvï9ü”[×8U‚M$¥7yûáéPz/á çÚ×>M…‡P'ˆÙ‚ùq™ÕázµpOÛ›’Êhë„‚yÀ10­iáñ¦MàÕhã€ºhtna$¬Úk¦r´îp±ïïàO
èuºø©i$
Ë¨³:P°TÊh¸ØIõ¥K_‚,‹ X4)Òfp˜+HsvšßM“±O|®m&àÄ)(X×‚©VîQ5¤Ê£Ìä¢qqù˜3 ã/Ø+¬7ïDîÃ±svÄ°›awn£†£?L÷ş¸€ŸÏrã5oÏÏÏ”‘`Ğ#OCh,ŞuÃÂ=@Oƒ‰¦²K1|ØúJ‡pFm‘$_!Àª†¼}ÂšõµMËßïmÊhï·ÚK°S)S*-Ÿ
[" üñ¨´
^ıØj+¾ì®hSŞqÜæÛdËƒ ‡¥#fµ¿8á¯§À}ôşé¾·rloÑÕ¥ö†Á.»èm¨‘$:‡Ëxëêqé-gÆ˜Ò×Ç“ë9:h>Ê¼9>äLEŞZO]›ö 3i¯©ŞX56Fz;THšÙÁçéOĞê~¤oÉ­cM?j„ å’²ô˜å´ö!¥?>M³Éç8ú¸øé~¾¨ì(~\ŒÜø.7ÿçæ^˜üğ]áPçñCÌ
OŠ^}v’ğÌlÿY<ªZd,¨r9!—:»aS}éú/W{Aì®Î:~Ë—¨ì®öqr‰ZyôÀë}ÔÃ[à ÕÉ¸<ÿÀıp"ÒÙynòQUàÑ2!¯Şo}›—-˜uH²ø7‹r×0LıNZ³#ÑÂ§¢	K¬bºi$[•ÛQüÛe…3$Öl†zªuj¨b ú&+ísÂôFNZfDnß+™ä~s>¶±Ò*e–‹#‘|Ô6msÄdÎRK½°2&“Ÿß«=íÊg¨#³ßkäæ<nO6ªæŒÔäfM““ò3ö·ä AAø±o¿#;Ê•›Íç-.+5PµTâšV×±ğÈêˆ¶C¯M‡=g“<ÒúŞµ)f¤ÍEciôdÊŒ—œg©¸ç¨<["¼ö,ÿ8³n/æ§ƒrº·AJ¼"ê‚¼„4µ½*~a÷E·cHt:Ô¶ÙøĞU½Ö!ÔÂ²òí—T¨8])†—ƒBËkp÷—8&Uè€«H“6Xs<sİ¿–dnÆm¥r´/ûÁ±¦É™¿ú¼¨î´W;bPX2ù…Û;Œ,B©&gÃÌ:g-ƒŒÏcÒN(ç ›¾X¹6òÇ7,Qd©}›1ïú“Õaè
'ÇŸ¬%×«=GÈ›¡mË·©.-ĞÇŞ^i}›R%IeM/R?àÂ| ¿<¯¿$
š†E'ÇKÑ	(2nĞ[ºjAiYgrw³kPÂRÊVíCqL­KzÍz¨x$êG¯H¹I'˜ƒèÃ)ó•ÜQ„èÙ SöMïÒgáÅÁ[`ø|5‘ƒŒâ 'Ğ(æx•7µ€vÒ{vT×õ!ÇÃsİn8Øÿ%ğrn:§ûÛ¼ŒÌqÓ5â(­cŒÔDû^ÅsøªqÄì¿®™é×¡ÁïS
n¾ÄTş»»òg¨k é7µV¾ â}ZŠY«œåS”FšnTC•‡¬‘;ö…tıLrÓ9ı˜åè‡öœ›é
®UxEÖ›wØ\$Œ;¯?)‘âÎ>ÁyWàÍí~s™óozç±Ÿu0!‹?
@'¯3Yl3ØeÅëĞ”¾Ãì€<½ À›MÛQ½IÌe„.Öƒ¿Ô“˜óÒ–éaªï\1ÀS~CjŠì—
¡‡ñÖ¹—!EÏæèÜ)Î$7§N‘Ø^QIö÷›E	å€¶¿msvr”ÙƒX‚8f$æ™A<|¾çvˆ»“¬z#(Åè©rYŞuR¼×™ğ„3SMösæNäO.«¬H“ìMBŞª¾LÂnË¨Bƒ|ÔşE¨+÷N'NÆp“‘©›RáizwHÌoµ‡‚åvó€ÁÿÑ[œEZxàVu}ó«õğA=×¶¨è¦öªê':~õ{Ônák` ¸?pL2¾’ä%£7K0ìÁ\5\:G±ü–B“Ş?ìpÂ ¦t"dèIõ~×åQÈàVM-«]¢ı'ê'­Š§—I#Lµí©,šÌSY†şf	)š":´Ñõç°°T˜#ÏAóålp2©ê;`9I¬Á^T*ëÃ®èö87¬³a¸VcÖ	Ş)ïõZ8‰üÕQú¶i}–IXÊ·¼#ÔÑ¢®)ŸşÍpnàŸ²Qqátº™ukØMU~ƒ`ÎØçñ{Ö31öÕçsŒŞÇz@`±$.êåç ¾,!Dğşqo¹:yæõÈ”‚–›ÍãÆ„,l<}8ş´b$ç¾B7­0M9)Âü16Ô;A¼^¾‡nÁxøˆlÏâŠPK¶h{h!˜s›wôW¿(nßû-2ÆPÑ@¾,úp÷J_—9œn¯än:d¡P*ß5ÓoJòYİf5N’¼u¢‘éf*:ÑMˆ­U÷„¯›ÑCB8[˜ÀÄ¬‘k­8og´Gg’šJD h«Ò½”jwT•<?•{½è2fòê¾1ŞåÅÉRñ	ö8FŸ˜Va€wûÍğ¢DßC•şÙAàÛµ:ej‡“3¢cŞ„×àN<íT‘›JŒ¶Ór›U8wQÀŸ£0Vş=‚ÿ0g¿L¤ÅĞöXÉ|é„‡ÑıLà«T£rÏCÒÏ;¦CâÃ3b»Ò}g>âğ¸KÀÃ’¯0ˆÏaÆ¢27·Aƒl?sŸ¤šL|RÂ bQ £Íš6Õïˆ-™éÌ%(øğ\(?Úpí„Z0[0¹í¡Ï“ÿÊ#td„nµr¨N¥}u_õŸOãŸ	®YS-æ¼K::ÿ†ÌÍ~’\Cø¼ù|”_YML)À÷«.Ä¼Ñ!/º…2°ò"Íˆ*ã„K ¿À,zMÖ¦Š|®Úğ!´Cáíñ‡,¸!§ûÁ[öØ€Ğ+µ	Û¶è>íê‡mŞ­lJ8Ùöo¬³÷4—‘Iâ'”)'=;8.¯–bxMÛ$­èïB?gW§µ4°¯vş–½ÇM„òÆ>0¸~A ½òÇÀõ3zœ‡LêäÄm8‡6ºˆ%‰e\oğÔú8¦.ts5Ù”õ7QÊ¸52é‡|K5Rèå
šA£­%#W8~v„l¶Çh{Õ€)¿¿„Dlõ–o í:(Ê•Vİíá¸î3rdj#£¸<Wøõ+ª'jïYê!Áh‚ Ü$Çi%ìû2
wwdŞ¬Ndä™§wW$Ÿ3`—b¬nZû.l÷„ëEÂ—Åª
{£øTü¼EË9ìA	vÅÕgœ'M0“ä86‡›ªmiŸ!k¢ ¡”vãbìP%A îŒ±Ş—îú÷hÎhĞ—Ó¢ş»'NI&Q‘İÊêU‰åmb@÷ÁÍz ùR´Bo~yyûdâõ¶’°îÆ) .è{ª¦+ùd.aÌAı£-Æ(ª#I¸‹­Ç°~£.¡x'¶!”µÚqõ†´’A¹°Cª'·Î³hU
)JP®Ae¯°_Ğ‹ù¿÷cÃ›5mÂ1ƒG]c £¯‚Şª¾ÚàÓOÏts¢ª‡ĞVT/—€Äf£Ev»Ìå«]ég»Ûäì\¯I ˜5ø‹°æz²©qM(±UYXè÷ŸĞĞÇh4Ü'ĞB–ÿ˜İ¬“ä±]8mKîèb2*ñâKöGÚ³˜$ äsK¤'ÉÍ»[Ş…Çk@A†ÕÖb Üê÷1¡[DX%CJ•bÓøÎ.ĞÉİP¬Áœh‹ùç™‘´IC@ÂEfş C
%DóE/:Øa¼ˆ)‚úÏ’:‹Š¶ü”	íOÍIâ0O¸3pÁeÅêNå8-äÍ¸Åï|)cà´×.öê g/ªËh®egé]Z†>Øš‰5î‚2×'„™ìÁ5h*š¸õÂ‘T—d½Ø¢#ú‡Â;j½ˆ`ÿf>p‹™bñå°ç÷*qÛ²~p§ç×P*/ø}‡×x£c¶ãS>×¼ÙŞNs·"wRHh4î:2Æ£øA]•^È‡ÊVæR‰…[m´ëb`õvó+—£İ,ã2$ŒŠQ	ôŠ\İpë»wƒ{-°›6ds»K°m(À9Æ3­*;¼ñ®ŒJK4iÙ ›5mí8¸C	®÷BNFÉO'12‚ Icw-\ô¾bGúæD>[TC2+xƒ-Ó’÷t»¤Éæ/™}“¾Œˆ•%¼õ»	ïw÷ªÅÒ	^äba’ÏÇhÒìÇğN¬R:š\AÏ¢ëÎ„MÈe
äı|5MÂÆ'Ì’÷hŠA™¤Ô¡ÂÔqgÖÈ?ÓôN0î5xìù`”ÏÆÈr†î¥á9@şßoJï[uC™µ7D÷%‘~ˆ3Ö¯ ibã”‡mÓÛúcÔ¶&A”z?ÄvÀ)dcÕëşp7á*yTy¤CÆÍı‚,†˜Înu²zÜæÜ|9‚·ÑbàÿH{ui‘‡ÿ±õÕA›´ù™Dï÷6’¥|z¥8›† Ü^U%]PÄÜHk?SXP®væ¹áñ|;¬M¯€tI iü$|É‚GÓ7› Á÷Reç5ƒJäTè2ĞâL `ğ¢Ÿh3-¿±ßú˜Ã³×ËT^mÉ}mu7TÿTa¨Wâ”Bôä³£Ñ¸h~ÖãÔĞ¥,Ç²ßTjªˆÊSãÊk=œz½‚Ï5˜,)[4ŒXÔãö¦dà_ğíçtD4æ*E"ÍL|Ë'½¤‡¿EJœ¬ v›G Ò/Ç7RR@€vÔa¨Öî{¸alÿ¯¾TXp¦Ã²1Ì|Ã¦o“Ê¡…½³bv³ŒünZSã>È>öCVÕahÇƒjqË>½§ñÎuÀN×! fªqµMy•ôMe‰@ú&•‘·£ø‹Ûtí>B ëÁÀîr‘ˆÿ>cßÆ+P N¸ä¸ì!^—¨ÑĞLJ›DŒ,yşÄ‚Á°÷D®Vkrù],3ı<v'”Û¨È`Â^mJ.!¢ÇÍ§:6¼-uÕ¶Öæ© ³œ˜NúÁNpäêºõ”Ş•ëÓğÿn¯à”3}³¢pVáTs8ŞÀd=^.ÕAD¿Øµ€inS?‘Ÿ«ƒê­šmˆmº?¥‘+‰;˜fhÂÉª’7şRâó‹8üÃ»>›ş:;(1Ø5eïbBŠñ—lkm£X‚0Šş=i€…:ê™Äı`!Ş1ä,ª.ìRÚÉ“2q…È³B¦‰·ºš#´][N–O~¹aGÊ~M)ÙÜ?£Vêv6ÈÄÇTª“lË?">Cî¸E~\³­:Öÿ•P?íšêÍ‹Ù^öØ …ªV¹ø£­ÙkC‘†xşkB»z£Ø ˜ UÃ(İj1/ lR“*Í_ªxé^´ßÃ”’Ø[„ iÚµxCd©_<‡ƒXŠ®
­Æâç¯*9Ñ•6&‚"²I_"’BÒiº;dc³Åµı³œC¥Í›Åó„C´
8ìíÇ]ÿz%qÂááTe¸Àœë›æŸ¦Cƒp‰À‹ÃJ”
m»môè´Â«¤OT}o‚Óu„1— l±ø Ä"èXê/=ïî×åÅ…Ò¢aVí}ìj¸±ìŠèb5	~2iBõG@årqÇº[˜û%•ıÍªÆÁçèÚ» 1…_º’Æ¸“Ş<50ŸâÒÿ¹ÏïT”zs¸´L_èÆÎ£KAø4æ H¯Ÿvî©²T…÷EÎ¹êÎ (Ãc¡µfÆK4X8ãûP%4Ä¦¼£ãókË™HÏ+İ_‹7ô&KÈx¼âz‘pg+Y³ØÉbòj×ªš¿/T½€®ŞÏQMÒ°‰¹æñáv)-kbÂb;FpUÃ“‘ô_›àš!qìŸìå7Ë“‚'€„•ñö¡}5BÄ¸’{æ_,Úú?/G>¢Ñ)«gWÃ{–ò$WaØ®:l£q/Ó»3÷<Ş×‰èØˆ‚G
Ğ¶ÚÕE7Ö­èë²
`öûª6w¡µ«
i«—1^Wò—
Ùƒp-KrÒp£ÌÉ®9ò¬ù40\¾0á`’:í9o`WH…'>¨9ÂÏµ"©5q[©Šå•6ó7‡Ğ!bÿ/}ı	éü›v®=de1º¯ª·HÅg	lgCßìöw‡#Ù²{„+¯½SÍ,Òë€S3¥T‰À· :öqîjxœ¿K]ÈÃ0/EÛ.÷…fÄ6,„o$}/â¹º¢¢U¼ÑèÒÿºÛğ´ÔÀèõCyÀp-V$+›3s˜÷‡‹&j§«ÖIÙoŒzK«¬"kŞÍdb™5(3ÎG	7OÑïzS7GVâ
qİQ’UÊ#ÙoR-Ğ	Å QÑ%)öy]4oäA	w.ÕûÇâ¢Iªr_N—~ˆ˜8§øfT {ß=Bñ6¸©™c¡ÄMºGÍ›õd]¯ÎmøyH]¶/QNââ-ªãˆœÚL¶{Šse¶[ÂšÇç)0|ñÌƒ¿éLô”ü®pæßsŒe†/®´Ml”nİS3pÖŸïàÃgğ!T!,wûüåõ‡&EQ†+â‘}ÍÇRÜfÇ·x“14ÆTx–6|I¿mfºS1
@‡ëkbêŠW¥Ux‚³z‚P¢½xŸ¸KõË+Éåšv~ş*²ßxj:<–¬a	›\8dä?úØÔÏM;)Ù¶ÂÒÓ]­a²Õd¥¿ĞÂîCô$£ ±1¶F)M8’ymÛh"™Q¸W1=aÊ·?+X±———m‰*b÷SŞc’gìW+ed-dÜ³¡A÷z$N$¹„…›ªî3·Ü…ÍË·œaËf°‚ÃC4ìŠkï	R‡—Š <N~[¼Õ…uU§¹Ùù2A/Uáá´÷ñà6ğ:ÁUU{ Ê
Wí¹¥ İİ_àÈ@ÿ§¿I»°åar{Üo*9—>Ë–qˆŸœVv™ÛÃ¬xİ¤YYù:mÃÔ|ô”
Ö?ÉâOµªšvø
fÀÊt;>Iòfğ‚	/yÀóõŠrÔú4ğØã8Ì•$åh“ƒÒ˜/E3%ĞÖì»dÚ@(e$Š™I˜ö»E4G 4=õØVÄuÒœ$Ğ¡~ğ¿u qŞ„ùŞë6€·šTu¥<©R«D_r¤.¥g]Òo;åš—ğY,´`bŠùşˆq5>#
sbı€Üp×&E®ŠÏ½vZÁ2^Ã¢Áı¬>X¦†<Ë2Àı*yM·PI;#’Ğ—¬ı“:Êø±ı¶ÁVµàµ\Eãy*xCj Úiî(PõeX;äßÏšSi *{îcıÆï hìº´XåxÜEhûQeî©'©ş8Ş=tFâ¢ 0b3B—Çä¸©©®/Œ‹>…ZŸ?º·àÜ¦Î=ÄL)Ö—x»€ *’“ü	¾?’Œ{ó+[Ğ‡»½Š6Ë}–ÿ‚¸‰Ù ¡*ñ½fğª~#˜õô7m©²|òä¾	R³gûõFŠ_¤;¼e)=G´í4<–# 1É8ÌM=.®\8®ƒ1¸:º®¿|ñ¶šU+Væ*Ï—Å®Ó›P7=pì^d5:Ö“aSƒasöp¡q^dî?­KTà&t:²›Ü ßk¯ÄòÉë:ëƒ‚>äj£û2{¹óÈC¶î8eŸ`9ú:¤#D|ëa)á‡5óüòàuuÄMƒæ¥ˆ¤²Æ¼ì[¼EÑš;Ø&}y5îÕQAE¢#1=:pÜ“P	ÜC¨ÖK—fÆõb°/Î ”esÓÔfH˜ÙİÑ#š‘°ù¢··—YĞs5“¤od'û¢§+í§.·Í#ûh‰k…{U/–+Áwâ2³›£%¼+nŒæø,œQ…÷~5ë$¸«Ü(Rù>+%tß%Â,ô;°,Ê4ˆ_ÈgÓÒê¡ØtÄÈ= H¼¨%¸ª£ÁÏ|ô$İ­‘H£t›‘»¯¼‰ %%yá˜v¿* å!}¬ÛúØKZ†w¿¶àô™Û(ÆÃƒt Ç¹À “*Å‹nä‹qÚ#,ˆo×ú61Ÿ6æÒ:î3½ô{Ì×`ümĞ’İ<U¶`N%Äok´Ø‡´ß†ÿSLş¤/†½ ¹eU‰h‚¸ç½&Yñ>û^†äÙKİ_)‚(|MXÆäŒ¶×ìºœöÑ3DûëGüÿ‡{•Â=æ/R~FND¦îºµş”ô%·÷Šãƒú Ã{
Â	òxøØNqM]ªÿŸÑ¸ö7+vA–âÉmú…n5­h—G$¥ƒ¡S•Ï¸ë°])ûn&?JFåtÌ~øj_d¾ù#2¸0³n“¹Å1…¦<ªkéëv~mñò˜q\ÈXÈ™G¢Ö	!aÀúr¨Z€ÄÕ‘‘"AJêySŞj‘ƒ3·ÖâÊ@ÌçuÍ4Ôi™S|èJPWBÔ‘Wf%	Ø>iÙè48V'ßÛOÍóx×ÌyS÷r¶™|…[ÇP³°¶×2Ü†¾Çw9e{è†²ë'½+…µo“Ìûá¢uéªïÖjdË„ïß-ºhíT·1“S}ö©føÚBœMe±U<vÒgSÊØ0»Õª;RpÎ}À~-”öLç*ía0È›.5!àL_·˜¦mnæ²‰)©fjjX/7uFi	kÓ¶¨t	
¥Óû«ÎŸêñ4ØGpl¾]=Ãd;¥£[`Ô`ÏÛM,“€|	,¸„X~±&Û2ï]d2À#¶füh) Ôr<%š¨î7ÆÛ®,ªìÒ«ÅÅ?Â;¸móŒÜe„,NÁÛ }Y(ë…N	ä¦å Z`çö7YUÀ˜i`À$‡İzÇ™ë¦ö”'&(„2x,õZ>,:PòŠİ*S|î‚T4rÂ`Ö8 Aå°ù.y§6!ü{P:vñœ…½MIWõˆLÏQ6°bÍîÃ=iøô†èüWŠ{Èc=¾Eù‡2<ùÿ©D¤äª³¤x¹hPŞ¸Gjx_¤İ­dšà.b>@±è@
†G 8À–õø5}tÁów¾ù…R{œ@ŒtÉ•H&¤©~, Foı>Ê¢£,¯¬:Â€£Z­ÿ?œES]›ÁqÙ¥ÖB0UOo¾4LË~Ã©£DG½Ç­«VXØltt"!¥s¿Y½NaÒ:V	à{àq;©êÉUw)‡£ç¶Úù‡†zòß¸c,(°İ¥LYTŸq„‰€ùÚ"?aÃµ¯Ş0ŒÖßæñDÏÚ@‰3Š‰.CÌ™4˜òH÷c3jÙU ?ƒ=ÆZ¸b `ßÎd6MÔl..3a0%JmĞ´¯ëyûv’ 3(ŸNA˜Å‰›IZŸÔ¨‰âr¿úÿû£´5ûÑ1WVÿMmŸ  RÎ&PÏ” ‡şì€ğ¶  ŒP½ö;0    YZ        description=Echo the kernel header artifacts used to build the kernel author=Joel Fernandes license=GPL v2 depends= retpoline=Y intree=Y name=kheaders vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII  kheaders.tar.xz           GNU  À       À                GNU ])u)UŞoÇŸÚC	a·¢¼¥        Linux                Linux                                   $                                                                      kheaders                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ñÿ       #            #     *             8            J       F    
 d   F       
 y   \       
             ñÿ   k   	    
 ´   t       
 Ì   €   	    
 á   ‰       
 ô   —   7    
                                 )             ?           N             U             a             l      #     x            Œ  0?;       ¤              kheaders.c ikheaders_read ikheaders_init kheaders_attr ikheaders_cleanup __UNIQUE_ID_description96 __UNIQUE_ID_author95 __UNIQUE_ID_license94 kheaders.mod.c __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module sysfs_create_bin_file cleanup_module memcpy kernel_kobj __fentry__ init_module kernel_headers_data kernel_headers_data_end sysfs_remove_bin_file      &     (     $     &     )          (          %     "     %          *                        Ì   '  ô  #   .symtab .strtab .shstrtab .rel.text .rel.init.text .rel.exit.text .rodata .rel__mcount_loc .modinfo .rodata.str1.1 .note.gnu.property .note.gnu.build-id .note.Linux .data .rel.data..ro_after_init .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   #                     	   @       H;                )             c   #                  %   	   @       ,H; 8               8             †                     4   	   @       dH;                C             •   0?;                O             Å?;                   K   	   @       |H;                \             Í?; Î                  e      2       ›@;                  t             ¬@; (                  ‡             Ô@; $                  š             ø@; 0                  ¦             (A;                    °             @A;                     ¬   	   @       ŒH;                É             €A;            @       Å   	   @       œH;                ã             €C;                    è      0       €C; &                 ñ              ¦C;                                  ¨C; °     !         	              XF; º                               ¬H;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // SPDX-License-Identifier: GPL-2.0-or-later
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
#define  MCI_REGISTER_DATA_BYTE2        0xA00
#define  MCI_REGISTER_DATA_BYTE3        0xB00
#define  MCI_REGISTER_ADDRESS_BYTE0     0xC00
#define  MCI_REGISTER_ADDRESS_BYTE1     0xD00
#define  MCI_REGISTER_MODE              0xE00

/* Read and write modes */
#define  MCI_MODE_REGISTER_READ         0
#define  MCI_MODE_REGISTER_WRITE        1
#define  MCI_MODE_MEMORY_READ           0
#define  MCI_MODE_MEMORY_WRITE          0x40

/*** Bit definitions for MC417_CTL register ****
 bits 31-6   bits 5-4   bit 3    bits 2-1       Bit 0
+--------+-------------+--------+--------------+------------+
|Reserved|MC417_SPD_CTL|Reserved|MC417_GPIO_SEL|UART_GPIO_EN|
+--------+-------------+--------+--------------+------------+
***/
#define MC417_SPD_CTL(x)	(((x) << 4) & 0x00000030)
#define MC417_GPIO_SEL(x)	(((x) << 1) & 0x00000006)
#define MC417_UART_GPIO_EN	0x00000001

/* Values for speed control */
#define MC417_SPD_CTL_SLOW	0x1
#define MC417_SPD_CTL_MEDIUM	0x0
#define MC417_SPD_CTL_FAST	0x3     /* b'1x, but we use b'11 */

/* Values for GPIO select */
#define MC417_GPIO_SEL_GPIO3	0x3
#define MC417_GPIO_SEL_GPIO2	0x2
#define MC417_GPIO_SEL_GPIO1	0x1
#define MC417_GPIO_SEL_GPIO0	0x0

void cx23885_mc417_init(struct cx23885_dev *dev)
{
	u32 regval;

	dprintk(2, "%s()\n", __func__);

	/* Configure MC417_CTL register to defaults. */
	regval = MC417_SPD_CTL(MC417_SPD_CTL_FAST)	|
		 MC417_GPIO_SEL(MC417_GPIO_SEL_GPIO3)	|
		 MC417_UART_GPIO_EN;
	cx_write(MC417_CTL, regval);

	/* Configure MC417_OEN to defaults. */
	regval = MC417_MIRDY;
	cx_write(MC417_OEN, regval);

	/* Configure MC417_RWD to defaults. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS;
	cx_write(MC417_RWD, regval);
}

static int mc417_wait_ready(struct cx23885_dev *dev)
{
	u32 mi_ready;
	unsigned long timeout = jiffies + msecs_to_jiffies(1);

	for (;;) {
		mi_ready = cx_read(MC417_RWD) & MC417_MIRDY;
		if (mi_ready != 0)
			return 0;
		if (time_after(jiffies, timeout))
			return -1;
		udelay(1);
	}
}

int mc417_register_write(struct cx23885_dev *dev, u16 address, u32 value)
{
	u32 regval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0 |
		(value & 0x000000FF);
	cx_write(MC417_RWD, regval);

	/* Transition CS/WR to effect write transaction across bus. */
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1 |
		((value >> 8) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2 |
		((value >> 16) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3 |
		((value >> 24) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Indicate that this is a write. */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_MODE |
		MCI_MODE_REGISTER_WRITE;
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	return mc417_wait_ready(dev);
}

int mc417_register_read(struct cx23885_dev *dev, u16 address, u32 *value)
{
	int retval;
	u32 regval;
	u32 tempval;
	u32 dataval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE0 |
		((address & 0x00FF));
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Indicate that this is a register read. */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_MODE |
		MCI_MODE_REGISTER_READ;
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	retval = mc417_wait_ready(dev);

	/* switch the DAT0-7 GPIO[10:3] to input mode */
	cx_write(MC417_OEN, MC417_MIRDY | MC417_MIDATA);

	/* Read data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0;
	cx_write(MC417_RWD, regval);

	/* Transition RD to effect read transaction across bus.
	 * Transition 0x5000 -> 0x9000 correct (RD/RDY -> WR/RDY)?
	 * Should it be 0x9000 -> 0xF000 (also why is RDY being set, its
	 * input only...)
	 */
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0;
	cx_write(MC417_RWD, regval);

	/* Collect byte */
	tempval = cx_read(MC417_RWD);
	dataval = tempval & 0x000000FF;

	/* Bring CS and RD high. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 8);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 16);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 24);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	*value  = dataval;

	return retval;
}

int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value)
{
	u32 regval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0 |
		(value & 0x000000FF);
	cx_write(MC417_RWD, regval);

	/* Transition CS/WR to effect write transaction across bus. */
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1 |
		((value >> 8) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2 |
		((value >> 16) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3 |
		((value >> 24) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE2 |
		MCI_MODE_MEMORY_WRITE | ((address >> 16) & 0x3F);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	return mc417_wait_ready(dev);
}

int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value)
{
	int retval;
	u32 regval;
	u32 tempval;
	u32 dataval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write address byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE2 |
		MCI_MODE_MEMORY_READ | ((address >> 16) & 0x3F);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	retval = mc417_wait_ready(dev);

	/* switch the DAT0-7 GPIO[10:3] to input mode */
	cx_write(MC417_OEN, MC417_MIRDY | MC417_MIDATA);

	/* Read data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3;
	cx_write(MC417_RWD, regval);

	/* Transition RD to effect read transaction across bus. */
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3;
	cx_write(MC417_RWD, regval);

	/* Collect byte */
	tempval = cx_read(MC417_RWD);
	dataval = ((tempval & 0x000000FF) << 24);

	/* Bring CS and RD high. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 16);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 8);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= (tempval & 0x000000FF);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	*value  = dataval;

	return retval;
}

void mc417_gpio_set(struct cx23885_dev *dev, u32 mask)
{
	u32 val;

	/* Set the gpio value */
	mc417_register_read(dev, 0x900C, &val);
	val |= (mask & 0x000ffff);
	mc417_register_write(dev, 0x900C, val);
}

void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask)
{
	u32 val;

	/* Clear the gpio value */
	mc417_register_read(dev, 0x900C, &val);
	val &= ~(mask & 0x0000ffff);
	mc417_register_write(dev, 0x900C, val);
}

void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput)
{
	u32 val;

	/* Enable GPIO direction bits */
	mc417_register_read(dev, 0x9020, &val);
	if (asoutput)
		val |= (mask & 0x0000ffff);
	else
		val &= ~(mask & 0x0000ffff);

	mc417_register_write(dev, 0x9020, val);
}
/* ------------------------------------------------------------------ */

/* MPEG encoder API */
static char *cmd_to_str(int cmd)
{
	switch (cmd) {
	case CX2341X_ENC_PING_FW:
		return  "PING_FW";
	case CX2341X_ENC_START_CAPTURE:
		return  "START_CAPTURE";
	case CX2341X_ENC_STOP_CAPTURE:
		return  "STOP_CAPTURE";
	case CX2341X_ENC_SET_AUDIO_ID:
		return  "SET_AUDIO_ID";
	case CX2341X_ENC_SET_VIDEO_ID:
		return  "SET_VIDEO_ID";
	case CX2341X_ENC_SET_PCR_ID:
		return  "SET_PCR_ID";
	case CX2341X_ENC_SET_FRAME_RATE:
		return  "SET_FRAME_RATE";
	case CX2341X_ENC_SET_FRAME_SIZE:
		return  "SET_FRAME_SIZE";
	case CX2341X_ENC_SET_BIT_RATE:
		return  "SET_BIT_RATE";
	case CX2341X_ENC_SET_GOP_PROPERTIES:
		return  "SET_GOP_PROPERTIES";
	case CX2341X_ENC_SET_ASPECT_RATIO:
		return  "SET_ASPECT_RATIO";
	case CX2341X_ENC_SET_DNR_FILTER_MODE:
		return  "SET_DNR_FILTER_MODE";
	case CX2341X_ENC_SET_DNR_FILTER_PROPS:
		return  "SET_DNR_FILTER_PROPS";
	case CX2341X_ENC_SET_CORING_LEVELS:
		return  "SET_CORING_LEVELS";
	case CX2341X_ENC_SET_SPATIAL_FILTER_TYPE:
		return  "SET_SPATIAL_FILTER_TYPE";
	case CX2341X_ENC_SET_VBI_LINE:
		return  "SET_VBI_LINE";
	case CX2341X_ENC_SET_STREAM_TYPE:
		return  "SET_STREAM_TYPE";
	case CX2341X_ENC_SET_OUTPUT_PORT:
		return  "SET_OUTPUT_PORT";
	case CX2341X_ENC_SET_AUDIO_PROPERTIES:
		return  "SET_AUDIO_PROPERTIES";
	case CX2341X_ENC_HALT_FW:
		return  "HALT_FW";
	case CX2341X_ENC_GET_VERSION:
		return  "GET_VERSION";
	case CX2341X_ENC_SET_GOP_CLOSURE:
		return  "SET_GOP_CLOSURE";
	case CX2341X_ENC_GET_SEQ_END:
		return  "GET_SEQ_END";
	case CX2341X_ENC_SET_PGM_INDEX_INFO:
		return  "SET_PGM_INDEX_INFO";
	case CX2341X_ENC_SET_VBI_CONFIG:
		return  "SET_VBI_CONFIG";
	case CX2341X_ENC_SET_DMA_BLOCK_SIZE:
		return  "SET_DMA_BLOCK_SIZE";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_10:
		return  "GET_PREV_DMA_INFO_MB_10";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_9:
		return  "GET_PREV_DMA_INFO_MB_9";
	case CX2341X_ENC_SCHED_DMA_TO_HOST:
		return  "SCHED_DMA_TO_HOST";
	case CX2341X_ENC_INITIALIZE_INPUT:
		return  "INITIALIZE_INPUT";
	case CX2341X_ENC_SET_FRAME_DROP_RATE:
		return  "SET_FRAME_DROP_RATE";
	case CX2341X_ENC_PAUSE_ENCODER:
		return  "PAUSE_ENCODER";
	case CX2341X_ENC_REFRESH_INPUT:
		return  "REFRESH_INPUT";
	case CX2341X_ENC_SET_COPYRIGHT:
		return  "SET_COPYRIGHT";
	case CX2341X_ENC_SET_EVENT_NOTIFICATION:
		return  "SET_EVENT_NOTIFICATION";
	case CX2341X_ENC_SET_NUM_VSYNC_LINES:
		return  "SET_NUM_VSYNC_LINES";
	case CX2341X_ENC_SET_PLACEHOLDER:
		return  "SET_PLACEHOLDER";
	case CX2341X_ENC_MUTE_VIDEO:
		return  "MUTE_VIDEO";
	case CX2341X_ENC_MUTE_AUDIO:
		return  "MUTE_AUDIO";
	case CX2341X_ENC_MISC:
		return  "MISC";
	default:
		return "UNKNOWN";
	}
}

static int cx23885_mbox_func(void *priv,
			     u32 command,
			     int in,
			     int out,
			     u32 data[CX2341X_MBOX_MAX_DATA])
{
	struct cx23885_dev *dev = priv;
	unsigned long timeout;
	u32 value, flag, retval = 0;
	int i;

	dprintk(3, "%s: command(0x%X) = %s\n", __func__, command,
		cmd_to_str(command));

	/* this may not be 100% safe if we can't read any memory location
	   without side effects */
	mc417_memory_read(dev, dev->cx23417_mailbox - 4, &value);
	if (value != 0x12345678) {
		pr_err("Firmware and/or mailbox pointer not initialized or corrupted, signature = 0x%x, cmd = %s\n",
			value, cmd_to_str(command));
		return -1;
	}

	/* This read looks at 32 bits, but flag is only 8 bits.
	 * Seems we also bail if CMD or TIMEOUT bytes are set???
	 */
	mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
	if (flag) {
		pr_err("ERROR: Mailbox appears to be in use (%x), cmd = %s\n",
		       flag, cmd_to_str(command));
		return -1;
	}

	flag |= 1; /* tell 'em we're working on it */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* write command + args + fill remaining with zeros */
	/* command code */
	mc417_memory_write(dev, dev->cx23417_mailbox + 1, command);
	mc417_memory_write(dev, dev->cx23417_mailbox + 3,
		IVTV_API_STD_TIMEOUT); /* timeout */
	for (i = 0; i < in; i++) {
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, data[i]);
		dprintk(3, "API Input %d = %d\n", i, data[i]);
	}
	for (; i < CX2341X_MBOX_MAX_DATA; i++)
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, 0);

	flag |= 3; /* tell 'em we're done writing */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* wait for firmware to handle the API command */
	timeout = jiffies + msecs_to_jiffies(10);
	for (;;) {
		mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
		if (0 != (flag & 4))
			break;
		if (time_after(jiffies, timeout)) {
			pr_err("ERROR: API Mailbox timeout\n");
			return -1;
		}
		udelay(10);
	}

	/* read output values */
	for (i = 0; i < out; i++) {
		mc417_memory_read(dev, dev->cx23417_mailbox + 4 + i, data + i);
		dprintk(3, "API Output %d = %d\n", i, data[i]);
	}

	mc417_memory_read(dev, dev->cx23417_mailbox + 2, &retval);
	dprintk(3, "API result = %d\n", retval);

	flag = 0;
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	return retval;
}

/* We don't need to call the API often, so using just one
 * mailbox will probably suffice
 */
static int cx23885_api_cmd(struct cx23885_dev *dev,
			   u32 command,
			   u32 inputcnt,
			   u32 outputcnt,
			   ...)
{
	u32 data[CX2341X_MBOX_MAX_DATA];
	va_list vargs;
	int i, err;

	dprintk(3, "%s() cmds = 0x%08x\n", __func__, command);

	va_start(vargs, outputcnt);
	for (i = 0; i < inputcnt; i++)
		data[i] = va_arg(vargs, int);

	err = cx23885_mbox_func(dev, command, inputcnt, outputcnt, data);
	for (i = 0; i < outputcnt; i++) {
		int *vptr = va_arg(vargs, int *);
		*vptr = data[i];
	}
	va_end(vargs);

	return err;
}

static int cx23885_api_func(void *priv, u32 cmd, int in, int out, u32 data[CX2341X_MBOX_MAX_DATA])
{
	return cx23885_mbox_func(priv, cmd, in, out, data);
}

static int cx23885_find_mailbox(struct cx23885_dev *dev)
{
	u32 signature[4] = {
		0x12345678, 0x34567812, 0x56781234, 0x78123456
	};
	int signaturecnt = 0;
	u32 value;
	int i;

	dprintk(2, "%s()\n", __func__);

	for (i = 0; i < CX23885_FIRM_IMAGE_SIZE; i++) {
		mc417_memory_read(dev, i, &value);
		if (value == signature[signaturecnt])
			signaturecnt++;
		else
			signaturecnt = 0;
		if (4 == signaturecnt) {
			dprintk(1, "Mailbox signature found at 0x%x\n", i+1);
			return i+1;
		}
	}
	pr_err("Mailbox signature values not found!\n");
	return -1;
}

static int cx23885_load_firmware(struct cx23885_dev *dev)
{
	static const unsigned char magic[8] = {
		0xa7, 0x0d, 0x00, 0x00, 0x66, 0xbb, 0x55, 0xaa
	};
	const struct firmware *firmware;
	int i, retval = 0;
	u32 value = 0;
	u32 gpio_output = 0;
	u32 gpio_value;
	u32 checksum = 0;
	u32 *dataptr;

	dprintk(2, "%s()\n", __func__);

	/* Save GPIO settings before reset of APU */
	retval |= mc417_memory_read(dev, 0x9020, &gpio_output);
	retval |= mc417_memory_read(dev, 0x900C, &gpio_value);

	retval  = mc417_register_write(dev,
		IVTV_REG_VPU, 0xFFFFFFED);
	retval |= mc417_register_write(dev,
		IVTV_REG_HW_BLOCKS, IVTV_CMD_HW_BLOCKS_RST);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_REFRESH, 0x80000800);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_PRECHARGE, 0x1A);
	retval |= mc417_register_write(dev,
		IVTV_REG_APU, 0);

	if (retval != 0) {
		pr_err("%s: Error with mc417_register_write\n",
			__func__);
		return -1;
	}

	retval = request_firmware(&firmware, CX23885_FIRM_IMAGE_NAME,
				  &dev->pci->dev);

	if (retval != 0) {
		pr_err("ERROR: Hotplug firmware request failed (%s).\n",
		       CX23885_FIRM_IMAGE_NAME);
		pr_err("Please fix your hotplug setup, the board will not work without firmware loaded!\n");
		return -1;
	}

	if (firmware->size != CX23885_FIRM_IMAGE_SIZE) {
		pr_err("ERROR: Firmware size mismatch (have %zu, expected %d)\n",
		       firmware->size, CX23885_FIRM_IMAGE_SIZE);
		release_firmware(firmware);
		return -1;
	}

	if (0 != memcmp(firmware->data, magic, 8)) {
		pr_err("ERROR: Firmware magic mismatch, wrong file?\n");
		release_firmware(firmware);
		return -1;
	}

	/* transfer to the chip */
	dprintk(2, "Loading firmware ...\n");
	dataptr = (u32 *)firmware->data;
	for (i = 0; i < (firmware->size >> 2); i++) {
		value = *dataptr;
		checksum += ~value;
		if (mc417_memory_write(dev, i, value) != 0) {
			pr_err("ERROR: Loading firmware failed!\n");
			release_firmware(firmware);
			return -1;
		}
		dataptr++;
	}

	/* read back to verify with the checksum */
	dprintk(1, "Verifying firmware ...\n");
	for (i--; i >= 0; i--) {
		if (mc417_memory_read(dev, i, &value) != 0) {
			pr_err("ERROR: Reading firmware failed!\n");
			release_firmware(firmware);
			return -1;
		}
		checksum -= ~value;
	}
	if (checksum) {
		pr_err("ERROR: Firmware load failed (checksum mismatch).\n");
		release_firmware(firmware);
		return -1;
	}
	release_firmware(firmware);
	dprintk(1, "Firmware upload successful.\n");

	retval |= mc417_register_write(dev, IVTV_REG_HW_BLOCKS,
		IVTV_CMD_HW_BLOCKS_RST);

	/* F/W power up disturbs the GPIOs, restore state */
	retval |= mc417_register_write(dev, 0x9020, gpio_output);
	retval |= mc417_register_write(dev, 0x900C, gpio_value);

	retval |= mc417_register_read(dev, IVTV_REG_VPU, &value);
	retval |= mc417_register_write(dev, IVTV_REG_VPU, value & 0xFFFFFFE8);

	/* Hardcoded GPIO's here */
	retval |= mc417_register_write(dev, 0x9020, 0x4000);
	retval |= mc417_register_write(dev, 0x900C, 0x4000);

	mc417_register_read(dev, 0x9020, &gpio_output);
	mc417_register_read(dev, 0x900C, &gpio_value);

	if (retval < 0)
		pr_err(