// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <media/v4l2-common.h>

static unsigned int i2c_debug;
module_param(i2c_debug, int, 0644);
MODULE_PARM_DESC(i2c_debug, "enable debug messages [i2c]");

static unsigned int i2c_scan;
module_param(i2c_scan, int, 0444);
MODULE_PARM_DESC(i2c_scan, "scan i2c bus at insmod time");

#define dprintk(level, fmt, arg...)\
	do { if (i2c_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: i2c:" fmt), \
			__func__, ##arg); \
	} while (0)

#define I2C_WAIT_DELAY 32
#define I2C_WAIT_RETRY 64

#define I2C_EXTEND  (1 << 3)
#define I2C_NOSTOP  (1 << 4)

static inline int i2c_slave_did_ack(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x01;
}

static inline int i2c_is_busy(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x02 ? 1 : 0;
}

static int i2c_wait_done(struct i2c_adapter *i2c_adap)
{
	int count;

	for (count = 0; count < I2C_WAIT_RETRY; count++) {
		if (!i2c_is_busy(i2c_adap))
			break;
		udelay(I2C_WAIT_DELAY);
	}

	if (I2C_WAIT_RETRY == count)
		return 0;

	return 1;
}

static int i2c_sendbytes(struct i2c_adapter *i2c_adap,
			 const struct i2c_msg *msg, int joined_rlen)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	u32 wdata, addr, ctrl;
	int retval, cnt;

	if (joined_rlen)
		dprintk(1, "%s(msg->wlen=%d, nextmsg->rlen=%d)\n", __func__,
			msg->len, joined_rlen);
	else
		dprintk(1, "%s(msg->len=%d)\n", __func__, msg->len);

	/* Deal with i2c probe functions with zero payload */
	if (msg->len == 0) {
		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, bus->i2c_period | (1 << 2));
		if (!i2c_wait_done(i2c_adap))
			return -EIO;
		if (!i2c_slave_did_ack(i2c_adap))
			return -ENXIO;

		dprintk(1, "%s() returns 0\n", __func__);
		return 0;
	}


	/* dev, reg + first byte */
	addr = (msg->addr << 25) | msg->buf[0];
	wdata = msg->buf[0];
	ctrl = bus->i2c_period | (1 << 12) | (1 << 2);

	if (msg->len > 1)
		ctrl |= I2C_NOSTOP | I2C_EXTEND;
	else if (joined_rlen)
		ctrl |= I2C_NOSTOP;

	cx_write(bus->reg_addr, addr);
	cx_write(bus->reg_wdata, wdata);
	cx_write(bus->reg_ctrl, ctrl);

	if (!i2c_wait_done(i2c_adap))
		goto eio;
	if (i2c_debug) {
		printk(KERN_DEBUG " <W %02x %02x", msg->addr << 1, msg->buf[0]);
		if (!(ctrl & I2C_NOSTOP))
			pr_cont(" >\n");
	}

	for (cnt = 1; cnt < msg->len; cnt++) {
		/* following bytes */
		wdata = msg->buf[cnt];
		ctrl = bus->i2c_period | (1 << 12) | (1 << 2);

		if (cnt < msg->len - 1)
			ctrl |= I2C_NOSTOP | I2C_EXTEND;
		else if (joined_rlen)
			ctrl |= I2C_NOSTOP;

		cx_write(bus->reg_addr, addr);
		cx_write(bus->reg_wdata, wdata);
		cx_write(bus->reg_ctrl, ctrl);

		if (!i2c_wait_done(i2c_adap))
			goto eio;
		if (i2c_debug) {
			pr_cont(" %02x", msg->buf[cnt]);
			if (!(ctrl & I2C_NOSTOP))
				pr_cont(" >\n");
		}
	}
	return msg->len;

 eio:
	retval = -EIO;
	if (i2c_debug)
		pr_err(" ERR: %d\n", retval);
	return retval;
}

static int i2c_readbytes(struct i2c_adapter *i2c_adap,
			 const struct i2c_msg *msg, int joined)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	u32 ctrl, cnt;
	int retval;


	if (i2c_debug && !joined)
		dprintk(1, "%s(msg->len=%d)\n", __func__, msg->len);

	/* Deal with i2c probe functions with zero payload */
	if (msg->len == 0) {
		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, bus->i2c_period | (1 << 2) | 1);
		if (!i2c_wait_done(i2c_adap))
			return -EIO;
		if (!i2c_slave_did_ack(i2c_adap))
			return -ENXIO;


		dprintk(1, "%s() returns 0\n", __func__);
		return 0;
	}

	if (i2c_debug) {
		if (joined)
			dprintk(1, " R");
		else
			dprintk(1, " <R %02x", (msg->addr << 1) + 1);
	}

	for (cnt = 0; cnt < msg->len; cnt++) {

		ctrl = bus->i2c_period | (1 << 12) | (1 << 2) | 1;

		if (cnt < msg->len - 1)
			ctrl |= I2C_NOSTOP | I2C_EXTEND;

		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, ctrl);

		if (!i2c_wait_done(i2c_adap))
			goto eio;
		msg->buf[cnt] = cx_read(bus->reg_rdata) & 0xff;
		if (i2c_debug) {
			dprintk(1, " %02x", msg->buf[cnt]);
			if (!(ctrl & I2C_NOSTOP))
				dprintk(1, " >\n");
		}
	}
	return msg->len;

 eio:
	retval = -EIO;
	if (i2c_debug)
		pr_err(" ERR: %d\n", retval);
	return retval;
}

static int i2c_xfer(struct i2c_adapter *i2c_adap,
		    struct i2c_msg *msgs, int num)
{
	int i, retval = 0;

	dprintk(1, "%s(num = %d)\n", __func__, num);

	for (i = 0 ; i < num; i++) {
		dprintk(1, "%s(num = %d) addr = 0x%02x  len = 0x%x\n",
			__func__, num, msgs[i].addr, msgs[i].len);
		if (msgs[i].flags & I2C_M_RD) {
			/* read */
			retval = i2c_readbytes(i2c_adap, &msgs[i], 0);
		} else if (i + 1 < num && (msgs[i + 1].flags & I2C_M_RD) &&
			   msgs[i].addr == msgs[i + 1].addr) {
			/* write then read from same address */
			retval = i2c_sendbytes(i2c_adap, &msgs[i],
					       msgs[i + 1].len);
			if (retval < 0)
				goto err;
			i++;
			retval = i2c_readbytes(i2c_adap, &msgs[i], 1);
		} else {
			/* write */
			retval = i2c_sendbytes(i2c_adap, &msgs[i], 0);
		}
		if (retval < 0)
			goto err;
	}
	return num;

 err:
	return retval;
}

static u32 cx23885_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_SMBUS_EMUL | I2C_FUNC_I2C;
}

static const struct i2c_algorithm cx23885_i2c_algo_template = {
	.master_xfer	= i2c_xfer,
	.functionality	= cx23885_functionality,
};

/* ----------------------------------------------------------------------- */

static const struct i2c_adapter cx23885_i2c_adap_template = {
	.name              = "cx23885",
	.owner             = THIS_MODULE,
	.algo              = &cx23885_i2c_algo_template,
};

static const struct i2c_client cx23885_i2c_client_template = {
	.name	= "cx23885 internal",
};

static char *i2c_devs[128] = {
	[0x10 >> 1] = "tda10048",
	[0x12 >> 1] = "dib7000pc",
	[0x1c >> 1] = "lgdt3303",
	[0x80 >> 1] = "cs3308",
	[0x82 >> 1] = "cs3308",
	[0x86 >> 1] = "tda9887",
	[0x32 >> 1] = "cx24227",
	[0x88 >> 1] = "cx25837",
	[0x84 >> 1] = "tda8295",
	[0x98 >> 1] = "flatiron",
	[0xa0 >> 1] = "eeprom",
	[0xc0 >> 1] = "tuner/mt2131/tda8275",
	[0xc2 >> 1] = "tuner/mt2131/tda8275/xc5000/xc3028",
	[0xc8 >> 1] = "tuner/xc3028L",
};

static void do_i2c_scan(char *name, struct i2c_client *c)
{
	unsigned char buf;
	int i, rc;

	for (i = 0; i < 128; i++) {
		c->addr = i;
		rc = i2c_master_recv(c, &buf, 0);
		if (rc < 0)
			continue;
		pr_info("%s: i2c scan: found device @ 0x%04x  [%s]\n",
		       name, i, i2c_devs[i] ? i2c_devs[i] : "???");
	}
}

/* init + register i2c adapter */
int cx23885_i2c_register(struct cx23885_i2c *bus)
{
	struct cx23885_dev *dev = bus->dev;

	dprintk(1, "%s(bus = %d)\n", __func__, bus->nr);

	bus->i2c_adap = cx23885_i2c_adap_template;
	bus->i2c_client = cx23885_i2c_client_template;
	bus->i2c_adap.dev.parent = &dev->pci->dev;

	strscpy(bus->i2c_adap.name, bus->dev->name,
		sizeof(bus->i2c_adap.name));

	bus->i2c_adap.algo_data = bus;
	i2c_set_adapdata(&bus->i2c_adap, &dev->v4l2_dev);
	i2c_add_adapter(&bus->i2c_adap);

	bus->i2c_client.adapter = &bus->i2c_adap;

	if (0 == bus->i2c_rc) {
		dprintk(1, "%s: i2c bus %d registered\n", dev->name, bus->nr);
		if (i2c_scan) {
			pr_info("%s: scan bus %d:\n",
					dev->name, bus->nr);
			do_i2c_scan(dev->name, &bus->i2c_client);
		}
	} else
		pr_warn("%s: i2c bus %d register FAILED\n",
			dev->name, bus->nr);

	/* Instantiate the IR receiver device, if present */
	if (0 == bus->i2c_rc) {
		struct i2c_board_info info;
		static const unsigned short addr_list[] = {
			0x6b, I2C_CLIENT_END
		};

		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "ir_video", I2C_NAME_SIZE);
		/* Use quick read command for probe, some IR chips don't
		 * support writes */
		i2c_new_scanned_device(&bus->i2c_adap, &info, addr_list,
				       i2c_probe_func_quick_read);
	}

	return bus->i2c_rc;
}

int cx23885_i2c_unregister(struct cx23885_i2c *bus)
{
	i2c_del_adapter(&bus->i2c_adap);
	return 0;
}

void cx23885_av_clk(struct cx23885_dev *dev, int enable)
{
	/* write 0 to bus 2 addr 0x144 via i2x_xfer() */
	char buffer[3];
	struct i2c_msg msg;
	dprintk(1, "%s(enabled = %d)\n", __func__, enable);

	/* Register 0x144 */
	buffer[0] = 0x01;
	buffer[1] = 0x44;
	if (enable == 1)
		buffer[2] = 0x05;
	else
		buffer[2] = 0x00;

	msg.addr = 0x44;
	msg.flags = I2C_M_TEN;
	msg.len = 3;
	msg.buf = buffer;

	i2c_xfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             ³$¸¶í‹ÈÌæôqı¶dñS¿ô­÷Ÿ-~¼Ÿ‚§Ğ;Œù‡ÉÿDõ‡•ˆÆk4æHI;í²ÔvÛƒe§ =v\iqKd]Ïı6%ù&¿x&cTñP¬¾0SRXpÿŒ§îoÄOq(å±É
l•{ƒg¦Æ3ˆOnÊÁq€HAò44˜yõí\HÅ¼­i©®îDÌ4sà†WZÓ:«¾¯šsÌjìã«‹ó|¯¬~e×ç,ïq*LÓ{–G…oì+V§úu ìv:Q¦8İ";¥°*ÃNFS#!šMK¸°àm[ş½á")²&ºcÚpˆ®Ü2Sš¸ƒ˜h“õ£)q/²;K3lÓh®¥,ÌAe…`ä²¸OÁöháS)±Ç©60Zè>.ı®pØgu+ê=-Ö+føÓ™$ÑŞz…O|–
Ê^°–o¬VƒEò>ÂÅd@d†Bl˜TCÕKŞ™Ùµ'‰ùtÓÛb* ŒJuG|Î}Å r9¾Ô/˜Hoa–ö+òvâŠnÇä+š·|÷Ä‚á°|Í‹YlìÈPÎ%
Ÿ¿ÒÍPŒVÿù¾‹í÷T#òk#%,íi4iîçºÒ\İ%TË³ı"ÆÏuˆÏú3JrÇô8	Ú]íé=2ê,ö(DÀÆÁƒ¤²=RT>©Sb~” Z´Â+l3•_¾Z‘¸¬HöN&ıvœCxu'¶Gå>übW)íî¬,–o½ˆDTíï•{*=°¶´Â[Mï¹„;¤–&®ôPë,ĞÙ®”‘Î[:ò.N!éÍq°Xe
İÜFÏ×<0€K”Ûß	 å¤Š*“÷ËsÜÜ‹|m¼s&›rÖæ6Ö¢g……ã"HÒqè	ÂfÍŠ¶m-Ë÷BÌçµ¢X€VCä"iÁ*Åm¾ì×Ñ†á¸UßPN<nú²[kòYÔêñÔÜuÉ—¦ù^£\rµ"-ÅÇß‰ £[ïıej-ÌÀñ&/“·½s™5ƒW4‘I®bBíînÊİC Ì±tÙWk?“ƒô£8¹ìÇ…†2å:+×Ş8²Ì²k6Š]|
ç2 Îî¶îgtñ¹$«DêÌ×Z¾š  X‰Â>\ıQ!’ØN$pÓ …	)Ÿ¹Jõ˜ám¡Lú\_¥gV²§:~/ÿDÀYM«ùR÷»A¹J

 9½¯áƒ!U‰ëYø/<{†oò³µ¶}„ œE˜Í9´ãì­Ä ÛƒóG|˜)Æœ¸Xë@Ù3wælM]îGNŒÑMşÃAxÌCWİ0Q“-F¦iŠË¿dÿ‹‚hDîÏJ«Şj“\Y‹c<³ñyÌA°é¾+±şÅM1ô/ºwg°J|Kúë¯"×”ó&àF,¾‘Vg=…"§19^Ô:„,+^«ÚQƒ”–¦3ÑÙ•Œ—Cl<wæ`”6›ƒ)êûWı·‰;Û‹raÎù	İæãúßµ”Iµ³$çÕoUè ‚é}Ã×)ÓŠæ'9b‚VÓ×¦MóÃaª¦ëÜnpäctºxàİ£ç\Qå†Ä#Lå¯œÿ<’`¡ù"ùü¬½¬¯‚ebŒ|Âé­z©î¯­¸€”Çœ¶ñĞ
W—( p^(Lëî¨ÒXÒª¯0ŒÃ@˜ŠmäşÖDI@RzWAÀVr£x‘: !ÆVLù‡Ô#D©æo‰$“•E [ïNa¢óµØ†Å´TZäµX˜§»“iG-ëéí„CÊàÑ˜n’äE,Jx˜ÒûPwoN«sŸRÏ-[éàE¨ôÔ3bóÊ1TÙnÑàÁTÛüÉyMC²:Õîêh¥mJÄWd„,O¤<ºú]ÃP‡1µô"—ŠDqhÌÂÜıœ÷ïîâ“=¤Å+•£wIŞ=SÎ2Èú•/±MÕwVÉÒn˜mc1î›:±‰ úı _\‘T¿ÓC`şÛít¨İDôP3ºyDR Q5Ì9¤íb_F•™~¾@CªôA•EIã¨N™5˜ûpµ,Æ0E—×˜øÙ%¾Ì¯º¥ñ†äˆ÷ïh–gzëp­ìûÿÛ´-yåùôÃ”¸u•{Éâ/"Ïï]ƒO$ÊAOvËNXå¦U]ŸØµâ”kEÓÕøF•It€tãI6[cu7L)R÷§-ºA}g4që ,Äc5¤o´hfz)Ö±g`èúhåG‹Ö¨U€'V¼(±_úİlûùğ5 ¶z_×È^£˜ÇŒ]šOÿ²i±bI¶qi)ï0w(‡‹™]ÑÔ&ÓÁ·Ì¯`a»g´hÀ/Hÿ­<ln1Â;À¼é,Õ¡ê_vã¦;Â‘oFç†zu~5ú_N=jøyU¿[ÛÿN‰‘ğ{hÃoö»˜¤n˜aY&*¸h7ïşbgò0‚RrdŠÅ`>4
x,¥æÎ1Â³Ë×œ8w.˜şß¨9hK3c^¡¤Yj#Têƒiçu3K"ç²_>ÙmíçßûêÊ÷»>õ”.O z™Îù‹¤Ù ¬^ÎÔ›à1ùp@‡/Ælc°ë¾Ï¯LğÂâæ­b¥¤[Oê­öÉ_ıÃnîÇfíBG¢3˜U}yTXìt6é,|ÙWa6!åEG
LôèÈh0S„Âó„‘½†¢+p×Èlšx¬B1µjO­«oàDğV¿Á~x§ÿ’×N1° ñš#ùÿù×çÑó{˜İ§9à[‰Ê	šIš[·˜¨7
€wâÏ”×ºêÕ œ“^qí9{sîuñ¼M 4”ëİ3šè›7{‰­0‘¥Ê™z@WBˆ1åJè Hº`ç—¯¦š ²ƒ³¹s0®úüWsÆÉù¤Å¯¼=4‰&ØBÛ^ö&CÍ&nKœ‚Ò§l2*Gû+.ıìïˆmØ»–+z`Ä£.
j7m(uØíV±#€ÑtñÛßwƒ»¶¦ú99 ¥«òm'Â|ÿˆöf ‰ÜäŞt³Ç¯+îßğ°nÑ*À#-Ë,@?ÍDM£éa”…=aZbì]šŸ·„*Ü‚–r´ı±—Ñ³ëš§3hùaìNÓ2¨‘ˆ¹¤‹Xl%x„¾za·uß‹|/—óßxçpE¶©¿	4gÌ%JNåĞ[V"1å®¶<!eA™äFä^ª7¸v1ÜÕ°¶ˆĞˆj•(o¡÷Rá’&ãZ‹Ô­0µÔÀ{ë}®°ñ·…~ÙJX_mw­nÉ##VÑ7UÒIôOÔ‹Ğ:á$î²/†Ï”ÁF0ü>òÃ4ÕŞ°nj	‚Â˜c¥¸‰ 
‚dÀglíGUO9êŞHµ+‹€(ãÜ|Ğ‹ã€å&È28?XE¯lf«ğÕ„sèz*³¯÷mˆ¬/ÊõXÕ°…¢•·l"U"ÿs´Õ].ƒ]£¡Å‡IòqÒ­9F­[0’$u¨ B½MO`P*ØpËài€¯áÚß
©OM³å’¿phë5g°¸OŠ·t°A—Íèê7ó#g?;ikGˆs¤ôhT _ìxîµ:—‘×Ü¿I‡›´;Ê/£´b¾Ês$‘#öÌßw¬d[—SæŞ'>]+v¬ÒàNî{ªƒu•‡ıœ&,ÒpÏ•}§ÉÌÎŒRX]ıV>Qqsg`RÎÜµogIzR,€DİåI/9ˆX?ir8ı¯V óJå„Ì´
YÍ‡20ïkÉ0`¦ü| ê%l(8M<ˆ?7Ø§Ÿº1{èÖS=Wã ÑÌN'W¼çdCšôñ´Áv‹r/`5“2-Aæ,–4¬°¶5”õ˜âcxb=‡Ci„ï¨Ù!}£ÀË	,‹Ï½Å[zë©Y ï™;à³ÖƒJY%ÔÆ†³¾ˆ—‘ }ğ$#ª…ìC›ò.HM×Ú¼B *oJO»s)ëœ·—ùT>jmeÖÇ„b†49A^®£Ï7©ë#5LZww.)_)j'º»J:='EB³_üÚİ*BgT} ¨û[$™ß–(C} ¬RÇs€ãM+ŠI‚\ãê¤_è%œØ¢N_,t÷k¤»<ãˆw7®º—yLÏUšJ«ü¾›©Ò–Znd“ÀßÈ:¢OïdÇÌ±¥2°}ÔÁÂAkÙ±Ä_%MHKEQ®BÅâ%‰g7Şk­mIˆèè¦‰ü½§|D­Ÿöà,Üô~N72¯ä¬÷Z+ukÿ]»ØFŠzÖ&y4Œjçf]Áç|Ê'k>ãµR²M^´3‚Xş_yş(nˆÎû% ÔÏ›û.,1äškïÿMDOuâ>›~Hn.ëCÆ@’-–4óìôãû“ÅœŠÖî0cZf®œüˆz\SpkÇl¦€¢
‰J‘ÆßË­´}³ÙÍeGÆ¾„.8N
ı<È2™Ó \úĞCÜ@:Ë(ÍBzÎ²ÇZ&@¡nÄ*?Öâi´µ¦9-|ğêr6´wª“YÉ³7A)Ìğü:K}—U½íp”ÊGú$¶Ëw¬¨h™‚´ A[ GF–p^è=ÇüX–U<ã-2v»Cc¾	õp9PÀÆÒo”cJ´°OÒ…ÇCB=ú¤­	ç+8Y…ÑS¡&÷
ê İ-¹$aó!ÜÁ?Ò‹¼\ƒG	wl&×&h³ç“2oík—d¼¿/38â˜úıúÄ®¾Ó±¢çó=dŠZpêúúy|*†xÄ‡*ÖğRv^?;Êl!’ı€bFzÒAÑcbbkŠPÂ‡½ØáÕvÓËôù)ÂÌÁÇW‘°?Pü#‘÷¨PÇï¼xwÛHnì/8ĞåJB¶}ğõØô¤ä³ê[’^î?Ç¦pVk=j_ê©”%ô$İ¶Æ˜P7Õş“GB‰p…-÷üªŠ}9f*´v×+ ÄõèGFN¶eYQiÒòûYI8hÔQsJwî´›}’$eW%’Î½—;p§îæß"ŞÚ#ø~¹1#ÈhÀôU&ú¼wE‚àç™0€sƒ>w/\-‡€X“-	Hh¦
™)L‰L"Ø÷[…9b?añ­Â=¼}ûÜŞ$é†Ç0Ò×Ëi~oÌ°GùdTm¯ŸüÎe¨³@nù+§…ÊÉÍ&õ·‹ÿ£¢ï€Å6JO·e·ÀqeT>áHnÍ\8˜‚a‚§!Sô1Zô¯Ux»7¢Ñ'> ‡IO3±.rDï%Ü–JòïAèt±&…w3Y¨_¦&"#„y—Ç^ûËieÃ±ËAF³÷pâ}î™zÎê>çŞMÅIk&{X  DÓ1;ø!Ki,á5Sî†áÃ‘EÖFXŸB²é÷«—ä˜úU*'ğ‰§ú ¼iBk™ÌjmAVPË•gr¶Át@^‘£¦£·½ø~'¸,NØĞ#Ç%Ñë92Ñ•zK‰µ§@auiFĞÂ¸ÃMÒV§;ob°®åTÖğD\X>ñõæNì‰^ˆq…ô€p€ KgÇû·téô—3Nm$¿zî.ê¼'bõ&¿úÆĞ·Şè\I¤¿ágyr÷ÑÅ*)ˆ‹ª]óØÏ©7ÿòøU7Çbô/d>·”7Ál‘J×£ÊùÑNx”|J.¥IŠ´i8ıÓá	XÊ†]*µB›“Ñ¯'b3c¶±°¼8¸½ÇöN¿¬búól%=
§ 'şlŒçµ5ªæòbèAîDÖtöeé€.‹]ÀHbó¼èˆ²äØ©OƒsÂ½˜ÑÍHèœïIÌ©° ¡ñ¥–U3!EÇ¦Œõş*¯Ú PøÀ± wB¦=FüHñ‰‡?&_·<L§®Hññğ«oP³;.]}§	»j°ğ3ÆIXRÖ„Âùj0åß)Ö©ô¸ÉUôûj·õ*]ó,è_Íø›íÒ–ïõïì;&%RÖ&J’[QÃ!RÆğ'Õ±
×HôÒ4İœªûc	¦xàğã"ÅrèFFÚ_ìj&Cê—6ôì•Ó%¤óÌá µHäº´7fx²nş´4”ˆ\zí^JZvMM ZŒÊuë6İ•Búøèp!\Ï,F„é[­?ä_2)¸óa|
¯õ*\ÿzS’¥%É6"J„‚nÅDWËšQ~×Ù“)Z%«6_à_Ñ *Å‚*M@¸xğ} ^“ã¼‹ífCqg;ÒÏÛÿ:8ù¶ b¢¢pbäÈtP+/=fŠLÁ%tœT«£Û‡ÉãNQ'Ò>U¿T¾æËşf(49Šw £16²3™Ø4õ×pŸüD[ô»#PºØz`ò~®ÜQ–OªÄA7aÿÊHÿçxwAô"c+òÉHbvçÿC€?ÊVû9yî!>x©¹ÄL84ÉV”k°¾‡¤òS(ŠÏQB)oJôÉ(C\ìŒ„_†"–vYzní©˜â}XI~&Ú*o JL]á­†ş(c¼`+‚F[ëú.7V]t&'"#™qá7üªÊĞ`†ÒğÇµA¥XÄB ³AÏzK‹ØÎÂáŒ G}‡—·ÚÜºóy@–Ûbë=„,úÅC¹ï-nER(
Ån}YWA–µ’¸Aæb‚d’·v Ç¤vÈ~ÓÉ7Ë%%CØ(î6ÿLx­w²kÙo]p¬×)œˆé›¢6úŒ/>¯7°uL<‰™×Á–fĞEŠŠ‰VŒV¹‘K@ZÓö¦É«<Šfvµ{~ÆfD"Œ\@¡ôo? Š¡*=æÏæşø–õ?DQL£ˆY#XÏ+´@‹øwÎftÆ(ã™`¢÷€÷Ò_†W{İiÆËß„EaĞj»ø]9_>±­«-Bõ4¢ß<»Q'ZG<OR<di
êšë½®Ë>3Ö .ü‡ÈP›`øXÛï•üeáS"Ì<¶pwâĞÔpòó%ıV\š z¥dÓ¬ŸñØLóêØ˜9­0„Õ¢ÖŞ‹ÀÊ½`Å‹­VÁúúÑC[¥ÌL¿9KLš uÈ¤VôgXëNC7¸d&øâõOàâÊ:ÜCù˜he$İJql1ÈL‘úœÃ)/ÓÖ_^Ìyúî¬if­ÅÉ5F÷?¤gN.D3$¡Ùt˜6dÓ1#˜lÔÚNJÍ³­6FË´MU%æ¯NgË4FÜ"«×5¼Ú^³\Z~bc@'Úÿa7=|C'«ÛóoY:û[=eÓÇÔåL¬ƒ½ºğšñcá³o[v‘»áyËÇq¤Pov“Ç…iÜPz•­eë,î-İ§üÍ•İà–VŠå¶T˜ìË cÍrÍ~˜—
¯¥/çH×Cü¤¾_Ê«´¸º6ºi,±ò0‹®7ÎëŠºÉøæU¸áz–3tÈù²@y¹Çs¹İTh‘Î˜)và{­SnÚÔ:º "ù¼}³>ïc3 !M“R»†äÒ“ )_9§:gÉ²€€¸Ÿå¾0	ü¬n&­æÃo®¯ãM*±ÉxC§ÿğéAÕ}{F½ÌÑÂŠLÁ= Ñƒ8öx7‘+úuyèR¹;ÆUE4yoó	û	¾ÚOáz¼s˜²pöôóàTZ¤?X€k=h†4õş²°¡Í’òırW™Fİ}&.·Õs;È›¡×+nl‰Éç¯Úi;<â ü:WGrû­ş4²vSp9¼‹€#{×˜~8´[¶ 	*¢pÇ´·­µ¨Ò :‰ÚH,¼¶¢•ñWyŠ›ñ8¡0\ÎÍ‚]_c÷> İÍÇ¤iı¾¹‰y½×°õö'Ã¼@û®ç|õÁ¦Şï=B0ÄŠzË)hDå†g@-·uù“Äã_=Íğc¬˜®H,ü6Ï#_’;á|Ñ„4Q@m:_HXÖ™ĞŞĞa-_½ÿY=¥¥t¬xÛ2=Å-¢&Æ¶Õ>‡@kX›P™Ío®úoĞ×êÑ¬Z#?‰§÷[k8páVƒ¹mÿ6KC¦uÜ¶F¯ÖUHüN›£¨Uº½ûnÄ±-/^ä\7=3_í9ï½ñtˆŠ>ğ›·‰Š«449ï°#n"¥ir¬å{·¦Ï)ı´¤]w­üTlÒ$å?õlg™Ma¸üÂÖDÒd0UÕÈ>Èî„Ü”Ûzƒz$+fºŞUW6j™bĞ¯ 96KÂ]š^ëL–C4ı×¨•=¯ŞÑ62ÔÌqUÃ1;xíZæë‡·Ë‰³é,BıÜµÉU¥gºÁJ®6×¥]"›’%s–GrX˜7«`Ñ¡gmœ¾K¦àT“ZåJ9t°¥h¼ÕÓyñIÔC™ècyëZu³‚È-»˜¢á´€Œùe…éßÿ'"v’RÂÛA3X¢VT¦ñ.•Œ÷®à™şaò_éŸ‚Úâ78şÛø°TNò6†eˆ}Í#Fö>;Íbáıá)o!®Ë"z#ïCÔBÀåU}e,[æ¾#`EyÙô"ĞûOL~ ünj3mR§ì§äröc («¶BÃ,`jKƒúİûXı¶¹€å0öî’+¢X3Óç–ªÈ1†ıP¢NkÉªíTGTntNƒ^Ó"’3xÑP#Á§–û$ĞiÕéü'2ƒpÌñğ$
Å§f•ó®Ò A$Õo˜C8ìá"AF7}–å¡°n‰¯ó„¯/½GŞ'àdoıÃ“•Àx
ğOÒò
t	ÑĞ˜££´°¤×*wësÕŠ‹®‘È ÖuW¡†ìqZ¥tmÊŸxˆ"9×Æñğ2¸\ÍjÚã¨ö}Siw;9ò`¦µGoj I"gr¦…©™ °îÂfp,¯±’¯Å§`kFÔ/¾~UP’2½±¸â#ÙĞ… ¤²ÄÖ­‘VÉâù>èhSÒè×S•a¦\8ì¹6£6ìïZƒnßÍ9î¤c$n´İÕÃ®85™{Ï’@æ£f§eÀ@ Ã4ˆJ: ›)+±í’dÚ P‡±ÊJdú DDedŞ0¯nwÁï ¤º®Õ,”%‡Ã×›ÂCÛ%TiWÆôœÖ?¹ìğ^şÆ*İE6ÉXØöÉÉ]‚HÄ bÛÅ}d¤42lChKQš3Z&Áa_·Œàài-<œïßº¢Š)"¼P£[/‹»(q ÿŒ(†œ›¦Œí-—QMc¢¶>ójÊ©ÅÕHMR’J[à3(É’1
éš©Cò"o–\ûI)¿Y½gÈ.vAÈgk*ªR_“ƒÆÔğ´ÅıÜmÑ$ëÎHÜ"‰ë„ûæw3İHGûĞ±úÉÇkÈ~2Ad>œ—îjJ†wşmxlKßi‘	iqÿ]uÿ(¾S.ØA>øôE94ƒ›R7,Ü³zA""”ßD¶³x<çí‹QšNŞ¥$b4u¼u0Üëæx@ŞlÃpî¼;İh[>=‡´üb¯ü2bíF=’dˆl0"ûkÓXEu²$ë²0ŸÎ7}€À‚~S5±>GÛ ä=²ŸYjÚ¦3N?Æpè¶/}SÉ^ì¦+Teì¥ÇÆkD™àèó*( j×|§Àş^pí¶ 
p¸WI~dí›;IÜNÔœğIÉûª,Ã®ÓZĞ¾l’#³%l‡3]š@%Š¸$jİÜ§$™İì3yQL‘÷áŞ£æ{Kß,ÓŞŸÁÅ²·”	xÈÒ2,c§…]N]bÌ{z Ö3Ÿ­DûIDøüüXo¯†!UL	+ğ
eªØÏæ&pàr¿‡gP°Ÿ|yœEÌô³Ä¦‡Ñpx¿p}á±™<Kï—şò»ZÉ©©céÆÒöL+ú‚Ïó/(qİgH±Ò&®<5ö6¿sõñİÛuQ­ûŞ–‡Ë÷²?i“!˜hW®=ğ8Ìı{¡ÚïÒêv)VfÚ"šîwÁá,R ÂÄ—›p%Q/ú'0¢r®M¨íú¢j"ü™şÀeÌ-g‘A˜ó7Z$P¾ÚE;ÃÆÓ¿òõZ¥§ˆ>=E"}‡Äñ	+ŠW‚æ%lÏ‡ÑåšMÎ@ß!ÿÈwK%gDÂ2(“ú²b®°vXÁ`Òt1ıÏW*²ì¥¼Võâ^Êwç–ŠÃrO]÷²sá~‘„Ê¸EguQB´%©†”‚V°ƒ¢k·äx? %ÂÔy¤»„&hLÏ­~J¸{“/ÂÿÂı[’aè­¨/R@9ƒ¸5ç™Tp1%"Ú&Ê=;çÇŞ,¥| [{+:vİÙ‹{/XàA©y‡Ÿt|JÌ+cè8zpì’§	1-Òr×Ãß° EÁO"¼ÌnBT·à5¨´>õ-¬ş‰{C$…tGNyü]%eÏ´ÆÑ¢•trWán'î;u3S"ÒµG ZaıaØÖ>¶XXå:õD_By¦2HWãšEŸR¹İ	@*µÎ@•$2 Øü„Åe. ·L5àÄFøH6“ÊÃv EÊæˆˆ÷9ISeÃURM1 ¸Â{ÛpªÉ1$øjEÂm®ôvĞĞĞ(Şl¬L6`Å–JCf5ËíÍE¢™ÉßC^»_ˆ%şü”Ú\éU&ÌñMleàVi<ü2Ñü h¯v¡^)8‡x$/EtCì¹NT;šÌÕ÷ƒ^bö¦é˜]öÌó±ĞÉŒ6ÁPq-^<¨FQsñË/% ÎLîOÑÀÿµŸËçœ"X.v+OyÔçºoŒ"@[t^
Ïbe5İáˆÊ¡Õ7>?KCÉ¬ï~#âØC­İ'*¾Ë aI¼E€øİª
æFØëÔõº©kÇ–>TvÇÂäNàp˜ ùMøŠb§[Z/M¶Ñ¨ãºîÅ‰™ £±ò=…m¼Öşhù85ä?6aZ˜of­º¿ïŞŸ²µİ$ş¦¶
ƒO6f´ÏÀUu³6C0éà*oö£?\AÿÜ8=“¢ë®«KçbŞ·4²Éğã×/ÿ×Sñ”¿ÌS8?R›["Ûtd}rè-H‰ƒ^€È"Q¦UøÇ)²%ôïälRkkê ±qÀío¯£Õ¬‘Ú¨‰­ŒŠVÃ{¹¬wïyŠ½¿šğö'r,nZÊ¦K=-wğGQ}ˆEÃÃ^?ï³^_>¿aõõb&áZVÎÒ÷jÕÊ,õ§4#Š«Q[¼HØ}5÷µ;™3ï¶hÌ:¿1âæB@#´™$Á{ ´Â;*Mº)EÙÁ·2*ÎÒ‰Á‚4¼£¾¤[»ĞÂàèÃ.ôHŞ8?:D5<¨Îš§İA‚¢ËŸ¨Iœ¬‚Ğ‡
ÑûéÚÀT/­ø±İjïe4›V£Á¹¿V éX–¯)€¼+Jà+.„?Ê®Æùë-óoOÑIY¾
c	Õ_\ó~h‡5aØpPâ*èdrÒÖÍúø?/ò‹›eaŠ+";ÍÊwr¸q ™Å¡d.9ı4i|"†³w€sÄÒ6ÛÎüËUEbÆjº3x™ş`n3Ù~’<P¡:àQxì@é*ävPÕè6¾–:Ü³q÷GH¦ÿİ7ì8ªmcg«¨päàÒıVÏR†Zää!¸Ç<Î7Ùü0lyÑY^Ü8@Dú¾ê2k¤“İqhZŒêd»V
Î‚øğä»b½ŒÈÜaÂì@o’ê ÃiÓĞnìEÁŸ	¸ú_á¬Ÿş—ã×)Ü
¦™„q³V×ïÕÛ÷mŞ· 74‹à!b;˜ò‚5>ÔS}ÙZ1»òìêo¢]Á-‹-0ÕÅk»£Z·1ÜoK¨i(Üi™	gŠĞ˜•ëª ÉEñF*c ç ìk³2À^ÖúRŒàD‰Åq².8ëñ|ün#±Fò|7©4ÁÏ„|YUJ\fã4¾%$çêí$lç\ÚthÏ2¿,Å‚‹×CşùJ9uûj‡d–ŠeÂ´º¨âYzˆÀ$éû@œÇË ½o©M¡âsJÓ‡|’Jzh8²m½Èˆ7bKxVcğ6ÈIûœ—"éï`©¦UxÒ% ÓˆÎ–Ü¼™õ1g>všpPG©FY˜í:ŠM%õÂÛbÕd='½Ñƒªà¥\GdYzD(|/]6ƒ¤ C£õìæ?ß›éB"° >fŸ	0Í›ÿIG|ƒÓTX(û"ƒ;r¼¦ª‡•n‚NYô]8™”Ö…ÜÚìøD’‰w-K¸ËÃ0¬¤•h9P4ï¡²04#ıCÿÑa¼5½RC$¨NÀ9æ¤ZÎ-3ûkŠ¸›!‹¿}ÒQUâDœsÌ«tç6wvİMÎm¨omŠ°èÃÃrdµoÂW‡)üÙû!Á†IŒŸ„NúİĞGzÕ”<¹LÌ·T©˜*"©¤™…øô&RŠıû¾YHS{$;Nè±,
V’¸:Şù‚^"x£’×öw|¦è¤MÜµãye d`V‰Ù>¥xm<¿f5Øõ3_Â]`’ŠvJ¶U}ßÊK)‚g']éÔ65²s°°Ö¦SóUIFİâ0±¬#¼IÇY¨`ìèÃãßËCÅ)}Ç1Œ§{zï&ÏK+hÌGÀ¡¾MhHèÕêŠ¯©ö§‘ÜbÔVğ sALŒ®†1Úšx7ÃÄ˜|}0> øK&QíâÜ}ƒáWúñÃ¼Š6y=J?s&=ÕçËµ¼¾¯‰±ˆ$¿2Íød»ìÇ}×­h®”:fA½¹DÈ®ûô«|“•Gk·S”-ÛíŸ7w¨Fa“È÷ñÖŸ€pé?èQu$,!ó*Xİ"d«ç2Z€«)ßtá@C‹Ÿš¨ŸãrX¨NœFö¾sÚe&´ïÚ'¤Ë»s…ÂÙD¬b™1c†‹QŠşCVä9£í$h©
uÄo
/Ô©öE6Õóíİˆ÷}Â8ÑL¡yëz~¶ÉÍ§PÕx¯âVßİ÷ù mcu»eÕÌ*Ğ¬f÷uª}ØQ8T,âBõA¿¡Ş¢\¼óµRèè¹7'9Ëüs±
€ÙŒ6§RD>S®½ÍÄ{úÉª?!ğˆ]Î8-šb§`§?¸I:óûq”N94á“¾=Rôx7JÂJèæ‹åß~t’„Õ%*D¶,s‘‹³çË)µİûélÔş?ÌdsğµäVZ×¼pğaî:Ì†Ó4†O‡ô^NIShA>‡3ÂjpQ<“õä$¨ƒ‰êvù¬%KÓær5Ç6`.I8­P¦xÆæ9“ï4ˆ&ï®íòƒc“å„•%‡’ßNœöæ#–í@Q0^­ƒË4(3	's„pÿ®éƒÒÖ;Im)ßÎ¹½9/§tß1NâõªD"ñâ oÈ)ù+=Jp)g&˜ùcî\H8‰Ü2xúó–ÌIøJPBOm×\zGZ1è%”_–Fr¯— znŒğj'ö€âÆÉ©ªIüôÍm­½iR|µ„>¥.E³YxÕµ0Î£/ÿ-+µÃƒ¡¸Õ`‘İk^8=µjÈï8eÙ¿F¤À9›…Óù= Ê¹¤ªÆ+æ–Zä¶5¦ŠR¾p®P'/CSBØ¸¦\ğœ™ô¡UçR£ùnt¸z¹¹ş+)€ú·ĞMö'‡6ëquÛ-˜\°í	q#²AÊç²ñm2d7ÆŒıSRëi§kbS$‹ :Àd÷ğcÖe´D\$Ùh}(®7âƒIëùÔLû›ûøÁZ!q€€1ôtı•ö¬ß©Ú9×yøhm®Ğ¶wøh*xz/ ®VA¶rıßõ’#Û	$‰«#LØ©ñó¬'4çœ8°OóJ¬8™§Ño@– x£üOùò*YzX²%&ªıµºís	M/ÅAº~¹·LhìÖ§1‹Š­}Øˆ¯çÆ°œgÏ”õ½ßøË¹	|:A’L‚İ‘NÒ~8YåKßşEMÕUîĞLwºÿ€ o}€5ÎÛNÉáe’w›WLl­y*$èi‰µÿZ7Ì	Jöô"šfjšmq&Í:âß»´OeIDš’­È´sÄ‰¬¨°(&Ó%qğãPRäAÀc†~†§³»2ÇWè‡mŞÚiqfŞ&aĞB>ª3tèÆƒ”‘…&·e8ød­L'ğT6;ã0ô`r[ÿxNu½šZ•s¾‡(èë’Šbµø¾(à$„ó÷:únö'¶[
ßéñ¾ïq8ù¢zß<A¨(ÒW¸ ÃÀq[Om^<ô¶Ël/cî®ó$¼õÂˆ!‘ÎJÓ²72DPªªê+Î3†B‡ÉH.ìòÖŸi
xæg»Ÿ#ùvj›‚¯ŒE$ƒ¬ÚÂ¯sk»>tnŒ¼©6Ì³îv5_ª‘S±¿Û†°“ï¦”ó;NwàH3›7øÃÖSóIVsØ>'¦µÖı«‡’KÄ5ş„2GÄ%îænoI¤ÍËf$ş–Sp°·ù¼‹2êªëå~ ÃV¨ş‡Ùi_Ú³ğTE.¯¡Ób‰˜neNÌ/ò.Y_w2ÖYV°~…§Ğê«¡K(	­,œ ZoWİ_M"RÚÉjj¡Ê#ò:`H¾W'ˆ©d%AUØÙ²Ò­º:CªÜ+½¡€,FÇÉÿÁm›˜ul¼‚Ä(Œ5àS—/Ä`“(±[Ñœ¼±hJ;²}İ¼äAZÅ×k<‰?!$úÖš››RğK½Î] »ŠD²·»FZg.Z
Ì!ÔÄüñçÖ˜tÇ†‚0Ÿ­šê¦ûºÌtëÌ0©rQ€øÕ–6„UiˆMÁ-ş:àÚ}6"–Œÿª¹Ãğ“œ¿€•,èÑö·{şÌ‡`©Ğ1â€E‰$ßº/të©BfR3ªf¹Ô•­Ş.Q>Mâ¡ŞÿS’2´	˜ÌÆrÜZœ¸XdJMdŒ·l-Édù»Jä7²¯„Øp}ê™gn°7˜}„ßú×#éŠ9Ëd­tB’|{Oi`rD cZy©<i´ëzm$:Ã±§Ò³ip÷×§;ÁbÑI¢w'Éòzt8å4İ;sÉsã}ìlÀVÒX‡(láûl¢Rã5N`íÇCEEb‡zÙ„Œ6k¡;ÚËüÔ‹1’¨ª‚ËØslw8±N'Ôò%ØÇ°ã¨Êio­Á…XByZøşm§ nHU¹£;«v‚a2ÆÚ>½+êdšÆÀ ÖXc(×	YyP¦HÊ·ğ‚‰¶OzmŞ¢æqq6r”˜4wUÜiT)?«Kd&ã_ÊL1¯e[YCˆMjƒ–R!³ˆ?(
(—”ÑÒÉ$0Ö¹Ìã ¥;t†gåbĞõukÙ½ZHïˆúOœÈïµ `»Y•ÇÍhxHä’%ã_›ò0xÄµ¦âD™zK¹á&;cÉ<¯X×¨‹€FI>è3@øg×±ÁËğÎ˜NØ@åcD~¢&‚¸‚ÜÄê~ìæoÏ‚Ÿ®âZú3ªš}½§XCÆKêX GÕÑQd-—r”›R`†Y Z£Ì¡Iüò^ˆvş)3êÄ]V0êh°Rê•RX,q§–Úƒ8\$ÖLõ{	‚oñ1Kîu[öQG ¢"¥¸ôF–BâÅß¦-?&ŸØı(<ğÿ&Gºpè]ãì
-ÃjĞ¿R±`+Ü…d£VÓ@R¦.šx‡—™=áÖğğÕˆ¨/½A‰ôîÚ²qòİ~|¤•wvšƒÎêßCDq[Êdæ~ÍV\¼ŠfÚtÎ­Ø_‘B´`åÍMşÃeµòG„±hn¤­\ê£ÎÊÍµMI\í'F^£íK­ùñ‡DŠÖúæiÓğ¨²ÿQ«§åT4Ô£ûï¼kE?‚¶™±¡€—B6¥™==Š a;ä°Tô´>çÿd÷åƒ¶Ó±²¹lòÚt»ÔïÖ+ôTè‰°\ÕÅ»§ÏÙ?ª©// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared remote control input device
 *
 *  Most of this file is
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 *
 *  However, the cx23885_input_{init,fini} functions contained herein are
 *  derived from Linux kernel files linux/media/video/.../...-input.c marked as:
 *
 *  Copyright (C) 2008 <srinivasa.deevi at conexant dot com>
 *  Copyright (C) 2005 Ludovico Cavedon <cavedon@sssup.it>
 *		       Markus Rechberger <mrechberger@gmail.com>
 *		       Mauro Carvalho Chehab <mchehab@kernel.org>
 *		       Sascha Sommer <saschasommer@freenet.de>
 *  Copyright (C) 2004, 2005 Chris Pascoe
 *  Copyright (C) 2003, 2004 Gerd Knorr
 *  Copyright (C) 2003 Pavel Machek
 */

#include "cx23885.h"
#include "cx23885-input.h"

#include <linux/slab.h>
#include <media/rc-core.h>
#include <media/v4l2-subdev.h>

#define MODULE_NAME "cx23885"

static void cx23885_input_process_measurements(struct cx23885_dev *dev,
					       bool overrun)
{
	struct cx23885_kernel_ir *kernel_ir = dev->kernel_ir;

	ssize_t num;
	int count, i;
	bool handle = false;
	struct ir_raw_event ir_core_event[64];

	do {
		num = 0;
		v4l2_subdev_call(dev->sd_ir, ir, rx_read, (u8 *) ir_core_event,
				 sizeof(ir_core_event), &num);

		count = num / sizeof(struct ir_raw_event);

		for (i = 0; i < count; i++) {
			ir_raw_event_store(kernel_ir->rc,
					   &ir_core_event[i]);
			handle = true;
		}
	} while (num != 0);

	if (overrun)
		ir_raw_event_overflow(kernel_ir->rc);
	else if (handle)
		ir_raw_event_handle(kernel_ir->rc);
}

void cx23885_input_rx_work_handler(struct cx23885_dev *dev, u32 events)
{
	struct v4l2_subdev_ir_parameters params;
	int overrun, data_available;

	if (dev->sd_ir == NULL || events == 0)
		return;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
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
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/*
		 * The only boards we handle right now.  However other boards
		 * using the CX2388x integrated IR controller should be similar
		 */
		break;
	default:
		return;
	}

	overrun = events & (V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN |
			    V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN);

	data_available = events & (V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED |
				   V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ);

	if (overrun) {
		/* If there was a FIFO overrun, stop the device */
		v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
		params.enable = false;
		/* Mitigate race with cx23885_input_ir_stop() */
		params.shutdown = atomic_read(&dev->ir_input_stopping);
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	}

	if (data_available)
		cx23885_input_process_measurements(dev, overrun);

	if (overrun) {
		/* If there was a FIFO overrun, clear & restart the device */
		params.enable = true;
		/* Mitigate race with cx23885_input_ir_stop() */
		params.shutdown = atomic_read(&dev->ir_input_stopping);
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	}
}

static int cx23885_input_ir_start(struct cx23885_dev *dev)
{
	struct v4l2_subdev_ir_parameters params;

	if (dev->sd_ir == NULL)
		return -ENODEV;

	atomic_set(&dev->ir_input_stopping, 0);

	v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		*/
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
		params.shutdown = false;

		/* Setup for baseband compatible with both RC-5 and RC-6A */
		params.modulation = false;
		/* RC-5:  2,222,222 ns = 1/36 kHz * 32 cycles * 2 marks * 1.25*/
		/* RC-6A: 3,333,333 ns = 1/36 kHz * 16 cycles * 6 marks * 1.25*/
		params.max_pulse_width = 3333333; /* ns */
		/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
		/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
		params.noise_filter_min_width = 333333; /* ns */
		/*
		 * This board has inverted receive sense:
		 * mark is received as low logic level;
		 * falling edges are detected as rising edges; etc.
		 */
		params.invert_level = true;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		 */
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
		params.shutdown = false;

		/* Setup for a standard NEC protocol */
		params.carrier_freq = 37917; /* Hz, 455 kHz/12 for NEC */
		params.carrier_range_lower = 33000; /* Hz */
		params.carrier_range_upper = 43000; /* Hz */
		params.duty_cycle = 33; /* percent, 33 percent for NEC */

		/*
		 * NEC max pulse width: (64/3)/(455 kHz/12) * 16 nec_units
		 * (64/3)/(455 kHz/12) * 16 nec_units * 1.375 = 12378022 ns
		 */
		params.max_pulse_width = 12378022; /* ns */

		/*
		 * NEC noise filter min width: (64/3)/(455 kHz/12) * 1 nec_unit
		 * (64/3)/(455 kHz/12) * 1 nec_units * 0.625 = 351648 ns
		 */
		params.noise_filter_min_width = 351648; /* ns */

		params.modulation = false;
		params.invert_level = true;
		break;
	}
	v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	return 0;
}

static int cx23885_input_ir_open(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx == NULL)
		return -ENODEV;

	return cx23885_input_ir_start(kernel_ir->cx);
}

static void cx23885_input_ir_stop(struct cx23885_dev *dev)
{
	struct v4l2_subdev_ir_parameters params;

	if (dev->sd_ir == NULL)
		return;

	/*
	 * Stop the sd_ir subdevice from generating notifications and
	 * scheduling work.
	 * It is shutdown this way in order to mitigate a race with
	 * cx23885_input_rx_work_handler() in the overrun case, which could
	 * re-enable the subdevice.
	 */
	atomic_set(&dev->ir_input_stopping, 1);
	v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	while (params.shutdown == false) {
		params.enable = false;
		params.interrupt_enable = false;
		params.shutdown = true;
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
		v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	}
	flush_work(&dev->cx25840_work);
	flush_work(&dev->ir_rx_work);
	flush_work(&dev->ir_tx_work);
}

static void cx23885_input_ir_close(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx != NULL)
		cx23885_input_ir_stop(kernel_ir->cx);
}

int cx23885_input_init(struct cx23885_dev *dev)
{
	struct cx23885_kernel_ir *kernel_ir;
	struct rc_dev *rc;
	char *rc_map;
	u64 allowed_protos;

	int ret;

	/*
	 * If the IR device (hardware registers, chip, GPIO lines, etc.) isn't
	 * encapsulated in a v4l2_subdev, then I'm not going to deal with it.
	 */
	if (dev->sd_ir == NULL)
		return -ENODEV;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/* Integrated CX2388[58] IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* The grey Hauppauge RC-5 remote */
		rc_map = RC_MAP_HAUPPAUGE;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* The grey Terratec remote with orange buttons */
		rc_map = RC_MAP_NEC_TERRATEC_CINERGY_XS;
		break;
	case CX23885_BOARD_TEVII_S470:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TEVII_NEC;
		break;
	case CX23885_BOARD_MYGICA_X8507:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TOTAL_MEDIA_IN_HAND_02;
		break;
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TBS_NEC;
		break;
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		rc_map = RC_MAP_DVBSKY;
		break;
	case CX23885_BOARD_TT_CT2_4500_CI:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		rc_map = RC_MAP_TT_1500;
		break;
	default:
		return -ENODEV;
	}

	/* cx23885 board instance kernel IR state */
	kernel_ir = kzalloc(sizeof(struct cx23885_kernel_ir), GFP_KERNEL);
	if (kernel_ir == NULL)
		return -ENOMEM;

	kernel_ir->cx = dev;
	kernel_ir->name = kasprintf(GFP_KERNEL, "cx23885 IR (%s)",
				    cx23885_boards[dev->board].name);
	if (!kernel_ir->name) {
		ret = -ENOMEM;
		goto err_out_free;
	}

	kernel_ir->phys = kasprintf(GFP_KERNEL, "pci-%s/ir0",
				    pci_name(dev->pci));
	if (!kernel_ir->phys) {
		ret = -ENOMEM;
		goto err_out_free_name;
	}

	/* input device */
	rc = rc_allocate_device(RC_DRIVER_IR_RAW);
	if (!rc) {
		ret = -ENOMEM;
		goto err_out_free_phys;
	}

	kernel_ir->rc = rc;
	rc->device_name = kernel_ir->name;
	rc->input_phys = kernel_ir->phys;
	rc->input_id.bustype = BUS_PCI;
	rc->input_id.version = 1;
	if (dev->pci->subsystem_vendor) {
		rc->input_id.vendor  = dev->pci->subsystem_vendor;
		rc->input_id.product = dev->pci->subsystem_device;
	} else {
		rc->input_id.vendor  = dev->pci->vendor;
		rc->input_id.product = dev->pci->device;
	}
	rc->dev.parent = &dev->pci->dev;
	rc->allowed_protocols = allowed_protos;
	rc->priv = kernel_ir;
	rc->open = cx23885_input_ir_open;
	rc->close = cx23885_input_ir_close;
	rc->map_name = rc_map;
	rc->driver_name = MODULE_NAME;

	/* Go */
	dev->kernel_ir = kernel_ir;
	ret = rc_register_device(rc);
	if (ret)
		goto err_out_stop;

	return 0;

err_out_stop:
	cx23885_input_ir_stop(dev);
	dev->kernel_ir = NULL;
	rc_free_device(rc);
err_out_free_phys:
	kfree(kernel_ir->phys);
err_out_free_name:
	kfree(kernel_ir->name);
err_out_free:
	kfree(kernel_ir);
	return ret;
}

void cx23885_input_fini(struct cx23885_dev *dev)
{
	/* Always stop the IR hardware from generating interrupts */
	cx23885_input_ir_stop(dev);

	if (dev->kernel_ir == NULL)
		return;
	rc_unregister_device(dev->kernel_ir->rc);
	kfree(dev->kernel_ir->phys);
	kfree(dev->kernel_ir->name);
	kfree(dev->kernel_ir);
	dev->kernel_ir = NULL;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      Fùº:C:†«¼>×˜ú3j#5Ò7$’/ƒC¡bG)”°‚¸qŠĞÍï³ôÖ~}õÓŞü ^‹›2Êó§Û‡9şNŠğˆc.ŒoërOj±^Œ³m³ÉiÃæ ¼ÊÊäüöÉ¢çöfÿ×¬e¸©İ éì0D†N¥¤i\wmM[DÃ©hºëz]’¿%ä}yw.·t_ıÆU`ñÜú‹8ÅÇ‡]&Ò!‹j‘·->¥Ìà#s@Òw²!W±'=
¿¾—\†0`¼Áu"â6×4Š×è—à7í¸ ÿRN€²‘ku\%Ä=7.õùMòl»¯±;IeÙ¥ĞÈî¡g¹zã4ö9¿T¥‰Fš£¼À‹şÎ"¦‡¤P…^™^Ñ­¯ÇÉÿ	FÛ×M —c™âµ´ÆÀÕİ~äŸã@¦¾§o¥#tÁVY&§7xÇ¹˜"äšØÊV"Êše…Î$Š‡b~—àô¾<‡:øğ=qİåØ’È<åùÔ€´­	>²şÖh#2+H,5^‹#†‡Áæo[‘Èp¦èaX¶Û¢>Z°Ôk9å†UPÔÏ/ì¶¨”qwpHvû[¡ıEóş0lÀlœ›•jË¹m‘cC—öÿ^`¼U; Æ$#´ŒYO‰,œ_ã	*İ>œ±^ª{coa*3¸öCr.+ƒ†
Ú2c"×ãÖ@K¿¡+ÁÍcW c.Ÿ-ºóø &:ø`p¬» ıÚêk’IÓª€´\É¶LU/ş“yHrf0FÃÊ“NÛ0¼Lf1©,:Ô½´²~ŞÂEH	Dê3.+°™®ıÚó¬tƒ Œhúª¢»ƒ¨:®ÉQNÖ’qü/çGĞRVbo`oñZh‹R
×tàr˜eÊ—ÁÍ1¬´µÁ•ªµF—ÕàÖƒ8RÇPuZ˜=•ñ§s À/O% Ç#…È²ÁĞàDv@´cÀ‡lr^z?¹¡{l¿ å¯i\Š(U[IÓ#Á,sŸbßí3¨}@#”³._|k	¦Eıu&¾&^¼?ÜU¤¤ÙJhÁ_1VKG‘Ã\ûƒVÒCH§åœ¬w+TœõÆ“Ó¾z¢±°kMòÒî@ÎİDšÌ®‘BÛeó} ‹{S®¦¤(Ö°Äu	¸›¼ÉğÎiX·™‰Y˜6T¨ªP¦BuRŞªa½ip^•îZeWfî),ŒœİÿİĞªq
:ğ¬Uö˜káLë£¯‰‰ä›PóT¦E¿’;(ÙV*Ã9ãÁ=>8¢*Z²¼F“îuË(8¼ŒĞU›LÁ&!–mŞ&Î‘v,˜Á&@{R­QNªíw_¦oú·¦DâÆ‚î;ŠùÑÖ{DyO{ÏäyXÕ?hM?'ˆsU5ÊUÖ'®Ó†çÒ:µ:»±Où´½Ø9¼}ñ/¡ªt»äìïo¾` :fúü®úNãØŠ§«È.=%)À’Bò¡ÎÚôïÎ_Àz]+ÂB{câS0Âáâç(|ßo…qætIŸ¹Ñ¹aÛ8tk'’4/ô°ı”â&‰>¯Şùà“pˆÆ-®dµiøÙ;1OFš~yW9“ìË¤Ù–î@fØÏU‡âÃÇè=g—ÃÂ
Ás¯îĞŠL|º¶<ŒÑ:¹Nf/‘ëØQ>ãeÈo º5Àæ‘‹kš.lO¦œÛÅø"Ï¼Ê1kñ÷,"È² ‚/¼-OÖºPÈxìØ½äœ'½7ü¶ó¾,ƒıOÏj)ïHm>)’^úÙÜÒé—zìÙğµ&ï¢;-0¤âÖB¸ÚÕ%Ã#ªT¡¨G‹¸ÃYıUNc‹#,|#)fMyìf+¼§”<¶ 8ì#yÆK£ç“­;â æOG­½)wN³Ï1˜n‰F©‰Æ1
†`c™d!gr½­ĞäePl