00);
		break;
	}
}

int cx23885_ir_init(struct cx23885_dev *dev)
{
	static struct v4l2_subdev_io_pin_config ir_rxtx_pin_cfg[] = {
		{
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_INPUT),
			.pin	  = CX23885_PIN_IR_RX_GPIO19,
			.function = CX23885_PAD_IR_RX,
			.value	  = 0,
			.strength = CX25840_PIN_DRIVE_MEDIUM,
		}, {
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_OUTPUT),
			.pin	  = CX23885_PIN_IR_TX_GPIO20,
			.function = CX23885_PAD_IR_TX,
			.value	  = 0,
			.strength = CX25840_PIN_DRIVE_MEDIUM,
		}
	};
	const size_t ir_rxtx_pin_cfg_count = ARRAY_SIZE(ir_rxtx_pin_cfg);

	static struct v4l2_subdev_io_pin_config ir_rx_pin_cfg[] = {
		{
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_INPUT),
			.pin	  = CX23885_PIN_IR_RX_GPIO19,
			.function = CX23885_PAD_IR_RX,
			.value	  = 0,
			.strength = CX25840_PIN_DRIVE_MEDIUM,
		}
	};
	const size_t ir_rx_pin_cfg_count = ARRAY_SIZE(ir_rx_pin_cfg);

	struct v4l2_subdev_ir_parameters params;
	int ret = 0;
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		/* FIXME: Implement me */
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
		ret = cx23888_ir_probe(dev);
		if (ret)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_888_IR);
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rx_pin_cfg_count, ir_rx_pin_cfg);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		ret = cx23888_ir_probe(dev);
		if (ret)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_888_IR);
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rxtx_pin_cfg_count, ir_rxtx_pin_cfg);
		/*
		 * For these boards we need to invert the Tx output via the
		 * IR controller to have the LED off while idle
		 */
		v4l2_subdev_call(dev->sd_ir, ir, tx_g_parameters, &params);
		params.enable = false;
		params.shutdown = false;
		params.invert_level = true;
		v4l2_subdev_call(dev->sd_ir, ir, tx_s_parameters, &params);
		params.shutdown = true;
		v4l2_subdev_call(dev->sd_ir, ir, tx_s_parameters, &params);
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		if (!enable_885_ir)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_AV_CORE);
		if (dev->sd_ir == NULL) {
			ret = -ENODEV;
			break;
		}
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rx_pin_cfg_count, ir_rx_pin_cfg);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		if (!enable_885_ir)
			break;
		dev->sd_ir = cx23885_find_hw(dev, CX23885_HW_AV_CORE);
		if (dev->sd_ir == NULL) {
			ret = -ENODEV;
			break;
		}
		v4l2_subdev_call(dev->sd_cx25840, core, s_io_pin_config,
				 ir_rxtx_pin_cfg_count, ir_rxtx_pin_cfg);
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		request_module("ir-kbd-i2c");
		break;
	}

	return ret;
}

void cx23885_ir_fini(struct cx23885_dev *dev)
{
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		cx23885_irq_remove(dev, PCI_MSK_IR);
		cx23888_ir_remove(dev);
		dev->sd_ir = NULL;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		cx23885_irq_remove(dev, PCI_MSK_AV_CORE);
		/* sd_ir is a duplicate pointer to the AV Core, just clear it */
		dev->sd_ir = NULL;
		break;
	}
}

static int netup_jtag_io(void *device, int tms, int tdi, int read_tdo)
{
	int data;
	int tdo = 0;
	struct cx23885_dev *dev = (struct cx23885_dev *)device;
	/*TMS*/
	data = ((cx_read(GP0_IO)) & (~0x00000002));
	data |= (tms ? 0x00020002 : 0x00020000);
	cx_write(GP0_IO, data);

	/*TDI*/
	data = ((cx_read(MC417_RWD)) & (~0x0000a000));
	data |= (tdi ? 0x00008000 : 0);
	cx_write(MC417_RWD, data);
	if (read_tdo)
		tdo = (data & 0x00004000) ? 1 : 0; /*TDO*/

	cx_write(MC417_RWD, data | 0x00002000);
	udelay(1);
	/*TCK*/
	cx_write(MC417_RWD, data);

	return tdo;
}

void cx23885_ir_pci_int_enable(struct cx23885_dev *dev)
{
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		if (dev->sd_ir)
			cx23885_irq_add_enable(dev, PCI_MSK_IR);
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		if (dev->sd_ir)
			cx23885_irq_add_enable(dev, PCI_MSK_AV_CORE);
		break;
	}
}

void cx23885_card_setup(struct cx23885_dev *dev)
{
	struct cx23885_tsport *ts1 = &dev->ts1;
	struct cx23885_tsport *ts2 = &dev->ts2;

	static u8 eeprom[256];

	if (dev->i2c_bus[0].i2c_rc == 0) {
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client,
			      eeprom, sizeof(eeprom));
	}

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
		if (dev->i2c_bus[0].i2c_rc == 0) {
			if (eeprom[0x80] != 0x84)
				hauppauge_eeprom(dev, eeprom+0xc0);
			else
				hauppauge_eeprom(dev, eeprom+0x80);
		}
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
		if (dev->i2c_bus[0].i2c_rc == 0)
			hauppauge_eeprom(dev, eeprom+0x80);
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR4400:
	case CX23885_BOARD_HAUPPAUGE_STARBURST:
	case CX23885_BOARD_HAUPPAUGE_IMPACTVCBE:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_STARBURST2:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885:
		if (dev->i2c_bus[0].i2c_rc == 0)
			hauppauge_eeprom(dev, eeprom+0xc0);
		break;
	case CX23885_BOARD_VIEWCAST_260E:
	case CX23885_BOARD_VIEWCAST_460E:
		dev->i2c_bus[1].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[1].i2c_client,
			      eeprom, sizeof(eeprom));
		if (dev->i2c_bus[0].i2c_rc == 0)
			viewcast_eeprom(dev, eeprom);
		break;
	}

	switch (dev->board) {
	case CX23885_BOARD_AVERMEDIA_HC81R:
		/* Defaults for VID B */
		ts1->gen_ctrl_val  = 0x4; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		/* Defaults for VID C */
		/* DREQ_POL, SMODE, PUNC_CLK, MCLK_POL Serial bus + punc clk */
		ts2->gen_ctrl_val  = 0x10e;
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2†”ºèÏï¶'AÍ
µzCÝçŒŒM†tIá¸ 'ì±ÐÄFòžˆŒà+ÑÌ¤àÀàÚ¾%NÔK¿9d„Ý"ó"áHx+¨j#åHn÷~„‚Ò9Œ®_Gzø¦·ªóEU›Q Äª	þÚ!"_UúšEË×mCÝ:GÖÊ$Kî®ÕC7‘k²#9/j°ó¼pé0‡äS¨ø–~ó2Õ×‡šî+…XùÅ¬ÏûÍ!¢ç¢€[Àù-“ýÝxètd.äºàêì·àS:‰Ê.ªR%“ÆM`W=:dþuUT•Ï©÷u»´«Dˆ…¨*…|P&Bf¾y#+6_}xÞDy SEÀhùM­¼£ÅD<}ÍOÁR¥è¼Íô	Šña"=ËÓQé¹¤á’=Úè:Z8Ä(â«¿ßÿlÔ±X©ã.œµ°c% <ù}rJ×”
~´fXmÅÞ9¥nÕhù'™†4rí-"$ÈV=Šš“:šOYÈJ÷#aì`Y Bõúú65ÎÀÛè’É©‹Eçádî™q.Ôúÿ‹{ü KÞÕË4.IýðœÖ¡ø6a4ä„9µZ_4\!'|kÒÿÿ·cÙ
<&	q6ÃŠH-óÕîoª±6ñ¥öãp_äÃFëücô¾Eµ†~Fª]®‰¾Ö„ëÝP-špþ@Ü	.ù‰Ø?  ¸sêÇî³Ð{]ŒTPEÃ	¼ëEKz\š¸^1´]N*Bq2$H…¡Å#94hxK´Ù¯—Æ‚É‰GµÊÐ—‚„Möª@¡ÄµJÄþ€‚ê¨"„‘[~WdV‹KýƒšÓa]o”2kx&‡•jÖÛÏ"ggÝ-‘4ÄZ%i9eNV‡lÐ(3'¡Ôe›“T½'épè6%aA™1ðñ®éP¤mö=òŽ×ËKÇ–Ÿ`”5ÖÜ5†÷½U÷¸+ÜžƒÒHÛÊ/
U¨Ü	Î§-ƒ|ˆ[9bŒÌM‹4Y“"˜ôB¨Ó°Š\WU#T¾vå$~ƒÇ_¦	eÛ†Ž@\Óí0Â-–—	sŠíõÈ€ÈTöÌ”ä2ÎóÊ|Q„dðµ;‚	^ µ’™9k®%Š‰ö1ƒá8ã–8Ah…ØÎ-Ü’Y¾_0å?/­¼K`þj žœý0¼átRFmAYö¸ƒ>‰•¬¸$¦jága¹Õ§{ìù—·¢=èô9’¸‘6i£Î§¤î?ò+šQ¤Hé¨=‘5Á/`»ì¬¨P(ÏÀÀêÓõù)ÆX™6œ/q<ü·këU¬2U¹ŸÃp‹¢J-£fF:s±òs;\,?i{.YÍ5á‰mŽí9…ŒN¯‘°¦›Ùø9_‚ìj­,¼ŒDt°Éq‡I}Ãºv2~9lµB¹… ŸMÉ™ärøÆhÔ°ÿò«gT2Zˆö&rŸÕëÌ>}ˆûêÖ|á·:ªHTÃ¾O˜ ™ERÇ" @5`= Þ¿ dY¨üt#nûæ^<&ËRxÎqró•Þ£±¹>ëÇéÃó@â­Ë¸ˆãñšzlænpkX‹XÄ»_`òÖæ|!R¢‡ê?o÷1e@˜3vÈ¦²ÒîæLXË@Q… ÿv½æog}}¸0G´4W1Ú? •/ö=:^ÒI¿	Î¢ñÕ¸ZwùÿU¡"	K±¡>ŸI¤Ï™×Ä\qr}PSG±ãAb<Ç-™Æf]¨­sH@{&OpúH.é&doÂÀ5º‹ÎzÐ\È•õ)Q€_Üø:‘ËÔUÌ ¸ìRÞZÆ½”ÌAó#v¯«B@æÀv_LŒ—å\~Yã&~¨‘Ý|ÆC„¡\Î¥~
yàilQcQç0g·°Ä`-yíEï·h#îð÷uÇƒ‰ãÔ¼l,ïWÈèOÐD,[³#¡_ù¨$àˆ„WèºÒ
!´Ø©õÀ“°¦	£öd’„×W CÇÇ²Õ6Äã« n¬†Ö^·ü•ž¬ªâË6hhƒñ«ùzÇ¬D]^ÎÞ’Q=i+cÄLQ©,€Ç8óvš‚°ËAV¤)°s¨Nÿ)WJåËk¡ªÿ±k)f4Víàe ×™ØP©òT€ží… ^
Vsg·Ù«ÿ®`W¦¢:¥˜£
FŠ]c WUV>%Ž‰’…ò£úßÐ¯ù»NñtaœKEŒº³’+¤Ë¨„ï¹û«‹¾=÷ ™*Ó¿¹U?°øx6"gâzŸoéç2ÖOž*ˆtE¾¨ á!™³%e}ÎÔ‚1{€ n„BÞw†‡«¦Èê©†p aíæ:…¹ØÅS¥µ<êu<­z^|œx äKZ
Ù™ª`,2"`£µý·OŒ˜­’‘3b×|éû[àH–\ä$5š#%Òýpºs†øŽáÜüHãc­ÿ‡¼@äßžCÿôÀÃ^·`GRL—àÎM†_')¡NŒØVÿy§"G”íö¢ [Iˆª[Sq±r)ûó ®'0ƒ\ñ?‰[>;(Â…ª!É)dBÊ#rÊ¢s&5C±»¤V…’Z©
/ÍfÍ<tfÏ&>¬èCãdÕããœv# È{eô¢•òèçw–g*×þ‰Á÷8`þ!òJÀÓïR¹Á¡žE[yPQÉW!&%ÜM—n»DO5ºQÇ½l?,
Pì²û8tË–´28²aþTÄ—"/d·ÝÇ±áOÍ¢Ù8zµ¦·-X%¢l·ú%„ BÏÌ Ì^ÏÌõ¯/!JÁ2Ô·ª,)	û}¬¤¤ðÎM¹Ë|rš_gLwCy±Ï«ëŽ1®EAêÅÜbö‹/…a›qëèþYÛsÌéiR2vL†fa¥{µûÅMÃ‚›JžÞ¹<µ¨/jÅQhÚi›[^;Œ<×yÌâ‰..jé[0æ°ì­Jÿ;JÊ?Ì·žôð’,uÝÖ¤"oþl·âÖ¹càÐhï„ã‹5ùôÞzR­êz?öôŽGé"Ëò(ta[kúoøÉBh¨æ4m-Du+Ÿ‰z§+ÓmÜ£èHÏÜ×fïè¡ÊE¼„Vô²ç=¥Pp±úÏ>
eÕÿ•K#‹P2‹Ì««Â
Š¸8rA]oRA’S½Mn%Î—,à_¶^ÅÚ+W2
Êqñæ:¸ÙªÈà€Äí$`»î1aåÏga+¯ÐÖ±\“‹Ð‚}6”\ý¯'ŠMWRBû‡ÜNqÞ Ò)÷½Ñª'Ù1´ïž=`°ñ˜NUùø2sÎs×„¢Çê$¨¶«Ñ€<Öˆ¬& \òöN†±/#N’›²­þLÇŠ¨.r*½ÛgvŸy		FfÔû4É®ÅøXÍpi@Ï„œ™ 1i:©:þžË lÝÂ4("ñ<P8¶pÝuèAþt8	”g<ªÑaÐ	î­ ™{óCúNˆ$+Æ#	bï4pæ—0qµgêÜÄpBˆ'l¬»Î±šW+œHð.¸:˜¢Ûž²ÞÆ3!Ÿp¸AËÕ¡·3¹²²¤€¶EQ†j…]XoLMÔLãžF†ïCYÍQóÌD@TÂAlæ¼¦­•‹™a*ÐjÏØAÓ*×º4J 6ø-ž³dáê¼¾Ýû<°f%Oÿ_eÏ½RCùçFŠÿ[Uúgxr s¡ƒs1Àçuñn¼3‡ÆÙüˆ„Ôm·ÔÙÓe%²„£Çcœ8ÃÌ7ƒ~0«=%ÛµÓ
þÊÂ¤³QÜ+T¹GÔ¡_Pç,l®„¨ÌÝ?žoý¿Fr"b)ÊÛ.Õý}4³2=x€gÀp1¸­Ñž0)FÑGþÍþ[_7éÐK¯Ÿü}¨ºTè)°¿/¦õF‰¿á€@2 Óç.ŸÞ7¥6ÜŸ¯Y¯
f°L©öp‹bÏÂÈ=r=F9xw$T‰”³“ëtiTnÿ­dEŠ¢8Ù”‡•©»vôr=ÚOðPaïB L0Ph×FŒù¥=¹ÙÛøÞ5²ÉÕ'ú¸7v¡Oâ=b–Ã%`¶š!›39\ïÀ¬o®OætaÂæ»Ç`oå†c×‹«Ë|B³=y&*þ£ôG"ÉbG£‚õ\i‘üéæêIgûñ:¼L~ž …½/3ë*íRÝaãÑ‘Ý‡Ý©%H±™‹Hïjj_¾ž%OØˆXÝ.u½Óêxº`$\}óÁ±„-@ bxO²Y	AÇÂ«”Öt[TÐô¶|²È–
gÙßÐ&€ž´Å)n$çe¿cw¹DáBod"Œ®AqåšŒ+_×
|ÔŸ&çU«Ñ8íb‘moù@(díÔð;–v5s*:#C!`	.c6ÝV³`xþF°¥“Q€H;Q°e.ÚcñôñtÄ‡7$ürBµÿn:ùãqÙfšxõã(j¾ö_WK2òCµÀ½+›©*vyˆë£ý<WãFI‰ÿü±;ÜnÚ}º‹J)wx¨±FƒÙÜÝègüÝïÁ¯r}ŸœÀh¼Ãr§Ò
–;M~6%ˆJó„ÒÿÜVÙÄ½\†GV!x¤ÿSv¶Qlµ¨H–U(!”’’ Ò•¦¯ÚrOTœ&g¬AøñÙA6BJxz8À	µ|ô,k‘	tZ„jYbÃØô5Skue„¢4Øûº¬0®ôa’Äòh‹‘:6R³À0œÑù»›\£Ï¾•íšìÅÜù³Ïïœà¯eaNÀ½JÛO8wÿ÷P®ûÑKPoÎÙ¬(êf9mö
%±L×ñ£»40©M‡,›Á_´G/0Jõ#Áú$„!“8Ó/'$!N¡Îúj¾‡cz¨`\`wB:FË?Í&~Ù1i»t¥¾£Ú,òùn¿‡zyh¥­Ýd%*ËÝŠµ(Çd÷™àôÂ][/Œ˜°ÉÙI…(¬é±ä;>FËRå(\õ¾‘©Õ›#¦<«/¸„<ð°iÑDæÞâ!ì>x°˜ˆâ¨<¬l.nÌÔ*A%»6mÝe¹*&øèÂ÷QÏÉ•Í±À¹ÎÌÐ@ûw›÷ù8¥ÃHÈ2€ ±`æržÐã¿ŸW¨µ VÕÆx”ùÇjžÜ¤éÀW¡Ê°üÛö¸J&à9±Zì¨œéB$gÃwå+ô”eÉãv×
frìþ¶ýø/í %(Ø™dèÇ=¯Þã Ê2èT?-?ú¹.öYú%ˆõh¾ùé+?—GAD~ Gíð­uá0@Ÿ%´?û®Š#OÒØùå®ÇÖðòóÐšpp»m*ŸkysO Ìpµ7Úo²¯‚œÍÂˆý²Í¯Üµ§	Ì]G*|‡v@qYIÿ¥¥Ç]µ±p;éö>Í
Ú’I`°Äñ‹nÌÁvØðØŽ…IDEO;
		break;
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T982:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0x8; /* Serial bus */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_DVBSKY_S952:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xe; /* Serial bus */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_STARBURST2:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1500:
	case CX23885_BOARD_HAUPPAUGE_HVR1500Q:
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
	case CX23885_BOARD_HAUPPAUGE_HVR1200:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
	case CX23885_BOARD_HAUPPAUGE_HVR1400:
	case CX23885_BOARD_HAUPPAUGE_IMPACTVCBE:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1275:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1210:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
	default:
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
	}

	/* Certain boards support analog, or require the avcore to be
	 * loaded, ensure this happens.
	 */
	switch (dev->board) {
	case CX23885_BOARD_TEVII_S470:
		/* Currently only enabled for the integrated IR controller */
		if (!enable_885_ir)
			break;
		fallthrough;
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
	case CX23885_BOARD_HAUPPAUGE_IMPACTVCBE:
	case CX23885_BOARD_HAUPPAUGE_HVR1800lp:
	case CX23885_BOARD_HAUPPAUGE_HVR1700:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H:
	case CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200:
	case CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E650F:
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
	case CX23885_BOARD_COMPRO_VIDEOMATE_E800:
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_LEADTEK_WINFAST_PXTV1200:
	case CX23885_BOARD_GOTVIEW_X5_3D_HYBRID:
	case CX23885_BOARD_HAUPPA›û˜ÌEÆMð·AØ¦^7JˆvàÝAí´=·s¸éq|J…5º†÷Á&1JM1½êz~G˜»¸¿ðCŽ£Ë#¯Ÿ7ðkö’GZ¿–¡>¾ÛOù×é£´â.%šfS·z½j=ÞOÑ`ÉÝ¡ëVgVÉ©*¡ªYCYEiq‚Ü¢ß/`™ÞAš½I0<YJCtÊaªNH›ñ#ÇYÄJL¥u¦µSO£Vº°\Ì¯àsÕ^”ê¢¼¯#Ÿ(yq’¸áé›>ÆwP Ý‰Îä¢À–»íJKþn¡ô ñU(5Ãgýµckèôé~ü“²US°†§nøfnÀ}J5Ê%ØÞ8*ƒ¢èãZû{?ûã,ÄB™Œ©‹86ýÇ·Kóð	ÃCBàiõˆÍ‰^%ïÎµ/vCÙ†›Ô[(ï™!‡"[05gfƒÄ©K
>·³o”[s“6Ð„´þ–¿wÏÆ«-l^™C?<zb¿£Ëñ/–I1ÿÖ‰@!‘9ý§<H²s¸;ç¨p&¢’Ï‡û“¹]n%ÝT-T†æ7ž[n«U˜_]­zÍtžO>õI¿»áÝ†2.	c¦±l\ºâ.¿çà¤wƒÅßvgno£ˆ›ž?ØÆ§ì€ñ¬øôÕÿ!T›þ.•xoFðšs2ïØ(ÓÑv€¯€h6ÍgÞòg"ÈƒžLÎ5K[ŸCq·n™›4—€GgsB¨¥êš±w. /ÑP\~0LÌC¿Ã’n4›q!Ù`¤ö”•þ"¿$V+$.‰ ŠUÄÚ2hJ¾zœ;g£"×:Ã±R!À1%îËyebëú [COaùÞ+,Aª·Z^å¶ØU;¿EjT·f~›¶·"íû0° Â`Hdì#oÐe¢+ö›m4!†•äÆ¡AÄè£Ë×in …7tÖ&ú£û¡ŠõkUq&¶Nhx8¥Ž!¢kî%ð¸BE#ðý\ÆÍ<·@'þKJ%¸¾}á_HIi¨QØé¨Ã´Ù	"2;´þí‚´amtÙ+®þc”ƒñ"ô“b·hH7JßñC²ßÝ·«àTÙ+Šr ×ðÝhË"ñ¦> ¤hŽ]ºÛ(nÊKÈ¡ þs÷à¥Édx4cË™"²Ù½•+‚x?mÈ¦9DFÆaá7'©Š`xÂQDÒ“óDà-L.†ªIDáGø6#}£Yß·qâ]Öäœoúõ—?xïot6§š¥£§%= ËP®ZìiDÆ[Rï‚ûéüú§F5¸v;e³òœ¦Ä“n©•|Ôï!
¸1Øî›]¯O6B"(£ûû-ä†]CâHcBò&÷Ž»MŸisEÕ“·6nœ¿Ö¿•Ú	JâöÕN¦íAÍ’6[ zž<8I{fÄÇÏ`T©ä1ØóÆÑ¿IvLñÓ*¦0w“ïy'·'å´6W`]-ª>Ãw-·ãøCƒœÍŒÖó°;TAd°/’Hbý…ˆwÇZM=ÖÓÆ¢q–¦.1š¸Lqj¨lgdCÓWƒpG%?<+4Í„å30'FK›ñ<FŠDäðV†òuŽg¬U¢AR2­¸ßQ«˜§£?ÖûYžÐõ .4òyOHÂY®…]1Kf…‡°‚Á)ÆÍõuŽ`¹Ö=î€(‡µu|+á ÑÂÉ@— rMCýTï}|Þ’|¬Ê¢ Ú›¡usï^CO?LO…¼²°RÃŒØÊV©ƒ°/"IÞkªŒüì¿ØÄéóªÂ£ÅBKFë÷Ó*,Ž ?šàõ‡ùÉ( šŸÕjq¡!›lÁ^š`®sÃ¤U”A­7cˆP>RÝþ}Èû™Ð]Lb¾mB3Z¯m„©ü¦N‹.8È‚/Û„rJ»-Tz‘æ+êc<ý} 9´,UÙtÍGé¦Öù*½[J‰XÉÁ \Ý7@€­nà"¯_ÜfÃè‚ %cFæ¦[Äb¨‰6ô1ä²v<Y‡Ø;ÚQÿ«®¤^ ncûm{£;Å#N,ô7PA¿ Wï7xê}ðÅ9”	gñôbt@TøÀ$'pÐI^9W*í²…µ°‹J®Amnîmžï
b:Yw`0»I½±CÞíH9œ'tiS ±‡L7Ã¿³T*Er5ñJÂ	ømZT·w?'†S”EùãÖåIGðö÷ª­OOÓƒ:•Vá²>-ÛÏCH¾!åv<4·êSâ©j#-)cÒÎ<Ú—©Âítýo¸.Æàü3‡9l=ÉÄugõ¡•½]ö
:¤ÊõÒ9ÿ<,l7Â @ÓðüòEÄ…þ{Ž ‘½ÃÏÍsŸ}vwB×î?l~8ÙÅ!çÑÞ©çH9s.wq¬ûÙšTë<êê*Ñ¶¬„%]Mš¨cŒª§˜ÖOõb|ÆuÄæÙj©+Z§I#P	S[¼Úb˜¼2@³tŒDèõ×¨™VqµÓØzý4éu™0ú M+í‡T*:wõ¢I°R=éœCä1ï9OmœØZÉø“óHa’î;2°+Zé}/Ç¹è©ùäñÅ¿`hNU4¤“ÑxþÊúJò}OoRF²ÆC3çì%n/§h›óë»‹À†I'Ä` /óô3\ÒØ	‘4†'æˆƒWÜZiÓié>Iº?Ç²çPÜ•›îàƒtÞ‹ìñùõ¬CµÔW£ŠOø	y^©4ºš½ƒ)Ëà<›ÔõüpÝrÏ+@“§É~òÅ¼Éýž¹ØÑ0,‘}{Jçû2ÐþkÎ¸¨¯lžóÎ8×Ö»àý©šÿ¢O©fšï°¨ë1†KüÊn1M/Òptšø;/½2¸L0f}õÄøå%9Ÿ‡AsÐ-|1%~Â^¦êŠîïÄ×¼¬Y¹V
$\ø’­«ˆƒŽîìÀwyhn@aírÔ&ñ*©§yñÀ»¹•‡£ZtÝÁÀ@a²“-“”*/¹fó›¡1){þ*,|Šõˆø¹!*õ’w<,ä	[ªbµ|nwzÀ×V÷ôfvIK©UV)<§×0úCîý˜pçàX+ÃuOî@ÔÒ,\ü?i–Èâè|˜ŽõT>Oòïö%«„F¦Eš×Pf@9ktº	ÅËà«¯M[öÑsðèêI—KÕÌi;ª \ò
ôX¢’MÙ[Z¤¬VX)4os;­íiÑ%WâÒ¶ÈåÏ-`„œëùâ¿·1´O@c)Œƒ£8Ô­7V¨;b9k\—W6Ê‰Šû-Æ˜gPÕƒmÐ8¥§SD¿Ä^G¤twÅH$¦3¡ñ¥ë§‡Û±¦-oécAn
ÍÚ5å/	A}éùëUi”Ô·LŽ­
ì4µ÷Ðæ kˆæX…³Ã¯©¸Éo›4ÿÊÞõÇƒEGQ"X9xB5ã49ea?#‚2”e' 0„1ÑŸ3ØÅ"žËUWYšd©Ä©RÝh\ÌŒë0XG±cé:®ªú¨HõaÎL¨J I¯ÙAh5mnß=)4ÎûêŸ*”5¹sti-³òÑIš úÏêITr¦ò,Z®è|Á-ó·IsÙ©lJ™n¨[fBï6_°õ~doê¥]Ý)eWuÝ`µ¡ã‹º"G˜bNÓUâø‡¹+ ‘Êp¯5ÉD„Eµ’Œ/²!}†²ðë*¤3¾¿7*Gï@s£«I…|x	MŽ—§“Tþðc×~Ð`z~1É­r18&3Ø¼æõb”£7!ÁÑ³Ã¸Ì=Ùµy»Š^?ÉU¶%XÝ¥4Í•Šk<·ŒìTzöÇ’`Kä\uªˆš¬¡4±M‹oWL&k›>Ë®µÚAhœ¬:}
œ¶’Ø»øx»ôSÃ8:ÄS{»_sÒóù»“ùVÑj|ß´æôOX<‘k«_écqØ¨W1ãœÝ~(£î66
Î8“ˆ¡4KJ(fSSó‚mQÜËÞÄ’/nŒø=±_ýÔ¤ì”˜]´0ÎåXE´Í»mFu5#²B”à»¶4Ì%r»R²3EüÌ5C_TÔ=©“ö‚€hœµ­/ÕnÁ¯õì—d+—¼÷Ïæw’ÍnÐýï3¶lÒä»˜-¿üpùUÑO±nI$z®sGD39n)éÝX5£u¥azðÉÔ™]ÖB¡ß›Ówþ¼z(°À~EL$þz«îË™\ç÷?£¶*ä¥,m@Ïµ.Øy,q×P,™[Ì-‚ð¿á,ýG„dm›`ÙHª=¢Èv´–3ÚF+Øþ¶¿^Rk÷ùÀ®w„’è¶Ä	¨õPvÔ‰Ã<9QSÒl‹'î„µ'¶v·%ê°¦FW³g¦’ÝÈ£H{¿A= žDÒy	¶p¯m=ÜÜ®d“ì@4³ìÍÇ0ãn˜ÞÅ³‰â£ããµï£ðÜÉ ¼AtÁ†ña‡ƒàLA	fµ²Q\÷—nÕðùIÒn‹l~¬Ì÷áªr
3…ÕmÎk]´ÐëìžÃuå[m>Ø¬ª¡~
DS9¶äÂ*õ[a2)—hðËG@q‚f@ìu¸WS†ñä4(€‰á´uG€Ã4­ì,MóŠ¡°‹3Þ1•ÿÐÞìïN”oh3ÆQ÷3ôxpï}¥n‰?Và]ø@Ñâ=³ûT€·ôJNÿæxÅtþ&f¹‰¨„–ûqõŽåü‹@”e¹èÃC	ÿûk¥ÍÙŸ·eØ›ÿŠ
‰Â1†Oon´–¸¼ðhuoJ%8ßkúß"øíÝö­Ñz]loÒsV'ýÃÙ”Ò9ô@D†úªJÎ„Q1€ÈÝÅ©8X ÃúA¦øŸ’ÌúK®2ç*ß­DŸ¥ø‚èE¸­»óërNïAJ¼”ƒ[ïŠ\†Ö!„‡uÆ8‚­?¤*(Âö¥xÆ)õÝ@ÎœaÝ¤êòï×çY­f	‰†¹ô1§A•óšÓí©ýŒ‘„Ø¥©u©2Ã
–NÁéž»½ý†ôCèKg%6rrý}x¬Žµ‘z|WñA{@Å¼$Åx|}³G3­”† fGgµ^‘9ýGWþ”Ú1T†.ýÇ¬šr`ŽJäSNÞc^‹;8Ü››c~„?"F ½hñÿÂ×’”Ì–y?•‘>Rù P¦˜†®©l‚ ð=’Ávr ÝäL!ýÄ­õ_‡§Â\ˆ6î8»YÃ=ƒ³Ð¢™—-UU©é}£ï–ÍÈ†j¿·‚LÈì[C: ‘¾f5Z\I¦·_vc$!¶ö%GcØÖ/]x-"<ŠTƒvÔ›@Fd(zÔd)}Dì“$?¦~XD’ YÌîÝ¬JQfQr¬€&¦”Ì"1|87S¨£8}ªÇ¸!Ù?.²NhO1ðJíÙOMh Ë·5¸ùNuUvIb,èiÈäÓðø+ªä³3q¬/æLL‚G‡ÃÑ ²x²ºáá7×Noîl&Àxú¡rˆg\3š@M’üÐÜ~'±†²™’ËÆFÜÝ)oƒóÙˆ*´]™Ü^4ºøÏwµÖ[¨¶…¿ƒNb“ƒ=æ\šÔ2^ÓrãÝeýBòÜ@`ôÕÓÃ3?óùÃ9“F€rô<)—	Ýof6ÓÒ /C0DHâð»ß!.2÷v‘š]T˜ÎzÃøè~UPÍos¹é‰;Õ7,°µÔ[äs†òN('OÜ4z¢Øü"—50¯E
‰\ÕTèy9Îª·DíNKš{m®^Nü™Îq¨õª‹Žü7ñ©c$Øßb>›z[›0ð_toß]ÀSë×ìo\m/vÂ0•íÄ|IÊÉè—­šx"IåG#ó*	–µÑšóäÛé<Vtî\Ó“¨7Ë”¡ér§ûÙ“íSçø¡9R/"wuL—ÈaTÞ¿~=Âqëò#ýoÑî>žxïûu(Ð¨~˜~¥¤xÈ3³)T¯znhÜ3Ù¨º—‹Û&¨šP¡¬ò;¬Xç~Æ·;*Xç{Š}ï aJ ±`ðÎ1Äëb4û¶EÆItÈ_çz¶mm+tâá3ÌÉÇÐkdêàÙ*ÁLmeH‹	¶‰Lô”Û±îì^[5u½h)©™É¿ßë…€%U¼ÔÇ,Ú™>¥Î?~’¦x=a™æLžH±Ÿ§KîŠÖÝMÊÑÜ¬’¶„¬¸Íž£šûO*à”È—O{SùU´Sµ©b	ü€@¤}?§9ˆüXá`¡kÀ¤•Q'>vÙO`—Ù<YÎ/-9
*¥ëÚÅsp,±â
×J-ˆ©ë;¡?˜ËP&<”­d^Ä{®ÐYi°k’6l¤WŒ™f…"±BMxõ>–[´Ê•b‹x´Ô˜Äš\{s1~!:(‡TCÂ$¹~_$©cÅÏµ`
Nñ#†9ô¢íqæ~øŠ2ì¨4„õŸÔ6ï¤Š¸V\"»=Ë¸Å(/;|z/ätûT”;bÐ)Q‚”––ªÍØÃU¼€züÛ.ŽPQHZFrwžs·¾Nå¬ï…Yœ‘ú‹päìcÈì¸#wõ¹¸pºµPLsðçÜ¯â8y}º/†Ž¼w×@ö€EF5Y‡OÛÞ\›” ©å°â=F6ñ¦½g³@?ÏÜ¦I½u:Ñc;¼f™èV¥°²+÷èô†º‚]Ù1ÞðGW-B‰'~0z±æºò¼hÔXÐÐ5Âkß¥à.–,9jj—aÞÞâ…K·TÚœ%—Ìvz¤¯À.Y»hLI•÷`‡»Ê;€ÈE]ŸÀ¼™º›>iþE§åô¡u‡„tN€R1»ñì+›–-ÒØJ›®Š•°Ûcæ°²øEÂ¯¥*žöõú÷pr±u†Ã<½·t,T°…ãHØÖì¤‰J^üqè>xd±õÇ«âåSÎÈ—²µ2 ¨NÒÍ˜:ŠÛÁ:Ä<~¨Ñ2Œ	²*¡ñÎ#/ã`!û?ï· ¬cÿÏ›ÖÅ#N^..¤^Ö©.ÔT;Ú#OÑù…
<Hßfƒ/H\’Ò@]áR#Wn2ä¶
[É©'¡sîfó¨ëÜú›¸ãÀÙ«Zšä>ÅÉ0‹”­Ñm¨,tbÌé\Üí9°`;å1æmÅ‰Þ S
Ë%»+&æ)­3gÅ€4$ËÀ7¥ØOi¬ßÓÑ¨*.AÆ•Ù˜us¦<)‡4ÿC=dO„Ù—Æ†ïTl5(³j÷ÒFÂÌª!:—ùÍ%IÝ²ìq!%?ô­tâÎ–ßï«É$läê{Í
ÌØÃ“&Û+8Ùnå¾Ã(r„Y‚Å”'7‰ð¦Jb|¢iÇ­rl84ŸÚ)> ‹ƒMÉãz¦=üÕ'±	<¡¦1Ð€ÔC·Óˆˆ+åÉÝ*ÓMÊõÝÄ7Cˆ8èS²%ÃÚ*`ïÛŸ#é5¡8ÄZÀZ˜Ý®¯¾«žˆÀQ9™0Òà‰ö—îë^¼üV‰º0`/«» ÄØ‡Lcq…õ(+Ö9L~0+½ºHÏäYLù_"—åÀ&OËzû"ˆöùwpâkÅÒ„õ~„}à”×u…ÞóVýn(íy55 Êƒ
mœ¯Hˆ[à½šÅJ(5×Àfm¯éŒÑ5¨j÷#½^Ö7ü€­-ÇGÇÔ¡þâ†E+*[‡eeé˜ü¾„IgbC¢v¸¤òjúIþf3á08Ã†e¼ûòg/soæÿ½SãxÆcÑ·—|!Åð²íDw›iÉÄhÝin¾½Õ\uÞ¨h4€ŸXPÑ#}7ã¢ .õ†ïK¼W*+v?óÇ{âÖ·÷DÁnkSS~1—¸60¢jïj…ºÄbw™¶iêÑÀ>D<Dk—=ÐŽ	„MÁ;é F#Ù.¾PiIF¨DÒà‡"OAdeU9„í{Ûß‚qWN=þjàmXe›š0†{"ƒ?~AòÅ’ƒæI’)„c”ýôûJÉË¼—¾q~/+ÛÀrýõm”®¬ÒÊÔÌüµâI›d}Þ3ùÜéê´@ÙÏQ?ˆÛðAãYDÏÍQ R¸–’1JÌ¬À©i<b‚9…‚rvP%§WöÀëè´—å|÷J%Î»B¬+sŒÝd¢÷:¶Œø€ä=›t9ëHÝ‡—þøYœ#’…rgÞàý“ :¢ÛÅR)ø²	Ýé¬aXF—ÏIò"8ãÃ4YÖ}ÁÈc2òx¦Ò¼ìÒ	BˆÆrp)¶òIÊ§ÈCCbÈD`DÈ¡‚åÅUÝUB=JãÁîÇ§ÆW‡aV/DmÿÑìôï€,!»‚E¯ñé>;<ySå…¹‡
>É†ï»† ÑïÊ}õG@ö¥h_n{ß±èÐÿì£{-t\A¹Ý	¦óÔ…F˜è©tÌYÆE_æRÒ%å‡hk’ÍÆ2Ê:V·q±ø4‰Ä”6³ŠÙ[•ÁrêÄ²¸AÓ‚×\…T÷µµ!:8X0]ßyÙ	²NU´’É®7}P ãô—w<Åx’‡´'ôi‚l–ñ»I\))Nc×±6÷M²±°ú¥*ç8±Ÿw­B ''ñŠ]ñ¡ô•rU…ºsá:€4ò?¯•„õ†sJIÃŒÔë#ç`}÷”N”9Pp)/—d@&!Äzx·ígT>Ð‹ó´@ç•Ð™dˆ@Ðº|
ßZœ:]Ó£(w©–ZôñÒÜ:]tRÙëîÂòŠ¿ªá!ÙZkÅ±~¯õ’vÎ”-ØP{(f?9Up9R%g¢ù¿É:ŒöçH`2ªÜów˜…bð*ñþEH¾°ïP^Pe\!FÔG¿&š0J“ïï¦“Š$£†³/ö ±- õzúï“ÑËn'iðÌ*Hv¬ñõ·=÷R9[Ÿº¤žâ0Eq|è@ÇÄù²•ºGÃ’4jƒÆ/ç¼ã%ÜQ%SII‘S”ìzuÊ Ð3k˜fÐ«­_¼gºÒ2üßœ(aìx‚õÑZcÓ2R$‘•ù›;#N¤¬æ£nâßhÊ1ï!jæ°ÝDÓ@èú±¥éz¢Œ½òÇèUJ-`UÇC„Owç=kÜ3èûþòRÀ^\Òþì<›·]zÀF©öÆúGLE
äšŸ9óhŠO&¡Èøˆ§×!—ÿÔ¾NVÂë_ó&oåÝ.^Sy0£1Ý­M@ús0i¥k¬gtñãIô*œ2‘ž3ÃùV5œ$Ä€T‡ÕÑ4_÷é=7Õ^Èe&‘«±!:p,qïOº®DqNwOžmíJKîØ¬~¯|âÐk½W		(c¬á1±¢ï”dü¾êŸisàCc¹úƒ_Ó €'
Ï®‚w’¨ÏR’£½-à/.Î.Ø¢%ùJ‡xB‡éao3õãEÎèòÛDÑ¨Hc€­Þ#I½ƒÍÆgSŒL
Ûí àXÔŒP vç<½žQâ­¯ï½Õq^`É·2*æ…Œ†Ž(pæ
IÖc©,T”<aèà«“R;à…”AèumÂre_}«®,–íý®¡”
€”Þ3®€JáSŠ½‘~ÕÓóC‡j×[ÆW€‡ºÖÀ`|JE€Aðˆ-0‚>á™vOÈh¸Ïæh	Å;3»Ý#h•$ÂD3c©Þ,¶gI4r%ë ÔO|ù°©6_j@Ï€,^Ñ
ç\òc9wHæj˜*OP&™8Ôˆ‰°Ôbzñe´-Ö²Ð¶¹…¾uª£hý%&ÉÔ´©[}_DE98]‰LÌäé½äQËæR€Ä««°llÍ"*/Á~Àïf¼±ü«N}$š¢–Môû+ye-îÖFtÐ]®JÍ¨xªáó‘&¼¬uç»½0ÊÜsúÇP²‰Ô™;¡`/,Us?
´œ5 ¦†“ž™“Š¨­ä~óÿ#¢l=BSþ£«£’•«Ô85éG2ýxeMÕ|ò‹uþ¦‰ïŽ(ç")Œ’ž4Ê>‘tn –®Ã9€Aë‹¬=Ü{ódt¯¨åÆ2‡†gHbòåïÚ#øfµHXI £¢ò	¡7×í¶+ít¢¼™èþ¿bÊ<¡}ŽFÞÜ¯¡³Xä·Ï?­‡À×¼þdx`¢ÿ¯2¸\«q²Ê<B’¸ØñŒwcè(KÔ |Õ×pÔcŽÓàá@'Ö8ú"'Ð°‚«Í¸üVâDÐ±“ >SîkbÕ§|l„€úçZ½œG¹i}hFˆ(gS›2ø‚À¨Ë"]b°;<×„êñ‰?k²²xR‹›àìµ	s!+(D¯˜­¡á(&PA}º¢•;#”Žô„|.m©jÓùÀÌ¾çÉøTÐyÊE¼–¥þ.Ñ2nÖóoYf;µzÀ„åyÿ»|×7`†%¤Ž[¤ÏãôZ½½$C<åH¶Ü­Ùk	§@SF3-›lBz†ôcÕDf¿Åèuî%Þo"Ð²¦J¹1ÆX™@‚Ã˜Á¦Î;JËÐÑ ™K>nsœ;&ª÷v&´_< öC\[4lƒ›½ÅÀ»«:£dâªk¹AÈ®’”¿¶_¿øDñ|y“¤·ËI~þb.¹ÓH)Ws[‘VÀ·k;t¶‰ýùQ@¹æµ÷BŸHö’U–!î—{'‰TVŒ¸¢Ì’–p\,p/pvLŽìæÏOÓK™ßv7ïµÎÔ€"Üaó>ÒcwüyÅÄ‰¾;™·{¬ÚkZñvE€¡×»h÷‰%‰5øÁj“Î\ÛO¬ØµX9)zÛ˜·i	ÅRÑ‰™W²êA7ô@.…ãJlzËgV$y¡Ž­d“öa¼¾þÇ—BJ Fj’j¥ªõÊ±ËGt+‹Âß%”øAéX†>»Ç­]ÖB™|0h0ä»5y»Ã.IWw¦.†ÍÂnw!Þ2ÔV‰ª^ùÄepZ›B©Ž6,VÆ%KFÕž=FŸGxùÌ˜Â¼w:o²Á+|Rš‚ñõf•²\w°©ãrrþñ4É$9›[h£QLâÑÁ…K–rÎÎAï@æÍ‰˜„ÿ™†@ëïT!èBóÉ)ŠÄÿ#ÝêcD¸žÿÇôK£}:Ú¿ëB·Ðy4Æ~k*‚âÂjÒ{uåTèW¸%à»¬|nòXé“}ðÙ|B£ë$ÈˆTcß~„cÖ +ÎÅbÂÝ®ƒ¢W§¦7ÝjCúÖ âeE*Öâ-bÂÎoi)T•hL,ÓpüpöÑ°ÖmwZTì°µ¶ÿ.µ8j/Yx” Q×ˆ­Ï°Îx=S–.Ánù¦§^¥#™vpL{n šÝJò=s^ÂG~
uåøõ
ÈÙrš÷™¼'*øÄfsQ¡ä4É+èV?šE¹×ã€5ÚŒkl;ëkSI¾°î8«iX5Çê”I–Îur~ÛÓ(àÔÝ–ïTÆmùŽ%$Ð±oÇ®ÑƒLñÀíìv‰ü³º¨Ê{x'?l/¬“à•-7rV{¡*œ^åýjAŠµiÙ5IoÕR„µ5Dš@37\Ó™t¦¶Š~°Mqs¥é\2µóa¹gJ¬ìÃz{ÇaÜR‚—ÈYHÜèÇ)H¬pÌànÊŠ/Cg‰pÆ¼è..¾m4„NZÃ)8ÔgEëd9rŽ–Röl÷;ô!¤ãû?–Šù˜•—&iŽ½l¤ß4¤}I4Éß
Œ´©…ÊñC·¢`üJ—ý.qEóªý„cÂmiwöÝ›m†å+Ý³Ï“ ¼ì‰c@ÈlØ¦¸Rföžä††CjÂÉS½hˆjQ†„úÑ·õÝ§oÜä®Êq	Ã†!6¤ˆ8n·tÈ–WðwÒ"*äÛA¢ŸWöIŒUh¤M+åz–5zCP›Öù J
úòÐÉíæIm{f•OucŒ›¥]8d½ ž¤ÈëZ¶”¼?•RFš™jÝ…K¯=Òâ¸H(ªro£N$‹ÍÞ©WË<	ËNQúÞNÎ¤·u94›ÀÜ‘÷‹Ê ` ]9±È¤/ñ$6ëýK)Ái†¼aT) ?%ãš„é”?=aoÕ‚ži ¾o_÷fóÀÀ¡úš¿òB§ r\‘3ÆÔI{²v–õ®€ï®½O !£PÛþO£c€%œ}ÒË1øé¾Ì:‰æOªBNé÷PÜð ÑV[ÇÄÖÕ~Ä^?µ±A:ÏÓÊ±Yd‰„É^\Ú×íŒ8@…. 	QS„Š&b´é·Igm–· §ó`×¢ÄÖï)ˆËï ðì‘o}2ÞFb•˜$Ü= /’;ðma4ŸýsÐìbQ~±Ø:ƒ‡R¯¦§`¿4ÄÕD%{Z=ÌÅ&r•Ðî1¸9Ò¢¿ìwæcTÊ,¯ÊßŽwŠmN¼K1ÕU”=º<0Ð‰´ý¼gÔ•0È­='Få@jÛ±ež¶S`Be Åš
Êwjµ&÷û(ä×d1„4¾ry"´È	A‚U¥±‘‘IÜ“ì‡ñ0õ]?!õ5ØŒ^ÆÌ@>‹-R@Ï|/£CØ–*åÉá—ÁA—ÓÆLC‡Dûþ®ß ·ÿóº[ñÖ°_×¼­`´—„¾4:‰€¶©1™V%ºøp@/_ÏÀâ	w«êu ¹‹Kø»4;æÖ+ÞHñ„.@ùbúšúaüeâøÍPËOå#ºg’$ÝËï+OÛPpIyv¬Òô‰Ú¥)¿×ˆKœe& Z
ré¤{’#jïˆ¼eaƒIfùÎ:ªY¡Ð#æù€!úËÇôá×$­´ƒ‘_=¢S