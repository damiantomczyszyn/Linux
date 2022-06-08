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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             �$�������q��d�S�����-~�����;������D�����k4�HI;���vۃe� =v\iqKd]��6%�&�x�&cT�P��0SRXp����o�Oq(��
l�{�g��3��On��q�HA�44�y��\�Hż�i���D�4s��WZ�:����s�j��㫋�|��~e��,�q*L�{�G�o�+V��u �v:Q��8�";��*�NFS#!�M�K���m[���")�&�c�p����2S����h���)q/�;K�3l�h��,�Ae�`䲸O��h�S)��ǩ60Z�>.��p�gu+�=-�+f�ә$��z�O|�
�^��o�V�E�>��d@d��Bl�TC�Kޙ��'��t��b* �JuG|�}Šr9��/�H�oa��+�v�n��+��|�Ă�|͋Yl��P�%�
����P�V�������T#�k#%,�i4i���\�%T˳�"��u���3Jr��8	�]��=2�,�(D������=RT>�Sb~�� Z��+l3�_�Z���H�N&�v�Cxu'�G�>�bW)��,�o��D�T��{*=����[M��;��&��P�,�ٮ���[:�.N!��q�Xe
��F��<0�K���	�古�*���s�܋|m�s&�r��6��g���"H�q�	��f͊�m-��B�絢X�VC�"i�*�m���ц�U�PN<�n��[k�Y�����uɗ��^�\r�"�-��߉ �[��ej-���&/���s�5�W4�I�bB��nʞ�C�̱t�Wk?����8��ǅ�2�:+��ސ8�̲�k6�]|
�2 ���gt�$�D���Z��  X��>\�Q!��N$pӠ�	)��J����m�L�\_�gV��:~/�D�YM��R��A�J

 9���!U��Y��/<{�o���}� ��E��9�����ۃ�G|�)Ɯ�X�@�3w�lM]�GN��M��Ax̝�CW�0Q�-F�i�˿d���hD��J��j�\Y�c<��y�A���+���M1�/�wg�J|K��"ה�&�F,��Vg=�"�19^�:�,+^���Q����3�����Cl<w�`�6��)��W���;ۋra��	����ߵ�I��$��oU� ���}��)ӊ�'9b�V�צM��a����np�ct�x����\Q��#L寜�<�`��"�������eb�|��z����ǜ���
W�( p^(L����XҪ�0��@���m���D�I@RzWA�Vr�x�: !�VL����#D��o�$��E�[�Na��؆ŴTZ��X����iG�-���C��јn��E,Jx���PwoN�s�R�-[���E����3b��1T�n����T���yMC��:Տ��h�mJāWd�,O�<��]�P�1��"��Dqh���������=��+��wI�=S�2���/�M�wV��n�mc1�:�� �� _\�T��C`����t��D�P3�yDR Q5�9��b_F��~��@�C��A���EI�N�5��p�,�0E�ט�ِ%�̯�������h�gz�p����۴-y���Ô�u�{��/"��]�O$��AOv�NX��U]�ص��kE���F�It�t�I6[cu7L)R��-�A}�g4q� ,�c5��o�hfz)ֱg`��h�G�֨U�'�V�(�_��l���5��z�_��^��ǌ]�O��i�bI�qi)�0w(���]��&������`a�g�h�/H��<ln1�;���,ա�_v�;oF�zu~5�_N=j�yU�[��N���{h�o����n�aY&*�h7��bg�0�Rrd�Ł`>4
x,��ΐ1���ל8w.��ߨ9h�K3c^��Yj#T�i�u3K"�_>�m��������>��.O z��������^�ԛ�1�p@��/�lc��ϯL����b��[O���_��n��f�BG�3�U}yTX�t6�,|�Wa6!�EG
L���h0S��󄑁���+p��l�x�B1�jO��o�D�V��~x����N1���#�������{�ݧ9��[�ʏ	�I�[���7
��w�ϔ׺�� ��^q�9{s�u��M�4���3��7{��0����z@W�B�1�J� �H��`痯�� ����s0���Ws����ů�=4�&�B�^�&C�&nK��ҧl2*G�+.���mػ�+z`ģ.
j7m(u��V�#��t�ې�w�����99����m'�|���f�����t�ǯ+���n��*�#-�,@?�DM��a��=aZb�]����*܂�r����ѳ뚧3h�a�N�2������Xl%x��za��uߋ|/���x�pE���	4g�%JN��[V"1��<!eA��F�^�7�v1�հ��Јj�(o��R�&��Z�ԭ0���{�}���~�JX_mw��n�##V�7U�I�Oԏ��:�$�/�ϔ�F0�>��4�ްnj	�c����
�d�gl�GUO9��H�+��(��|Ћ��&�28?XE�lf��Մs�z*���m��/��Xհ����l"U"�s��].�]��ŇI�qҭ9F�[0�$u� B�MO`P*�p��i���ڝ�
�OM�厒�ph�5g��O��t�A����7�#g?;ik�G�s��hT _�x�:���ܿI���;�/��b��s$�#���w�d[�S��'>]+v���N�{��u����&�,�pϕ}���ΌRX]��V>Qqsg`R�ܵogIzR,�D��I/9�X?ir8��V��J�̴
Y͇20�k�0`��| �%l(8M<�?7ا��1{��S=W� ���N'W��dC����v�r/`5�2-A�,�4���5����cxb=�Ci���!}���	,�Ͻ�[z�Y �;�֞���JY%�Ɔ����� �}�$#����C��.HM�ڼB�*oJO�s)�����T>jme�Ǆb�49A^���7��#5LZww.)_)j'��J:='EB�_���*BgT} ��[$���(C}��R�s��M+�I�\���_�%���N_,t�k��<�w7���yL�U�J���������Znd����:�O�d�̱�2�}���Akٱ�_%MHKEQ�B��%�g7�k�m�I��荦����|D�����,��~N72���Z+uk�]��F�z�&y4�j�f]���|�'k>�R�M^�3�X�_y�(n���% �ϛ�.,1�k��MDOu�>�~Hn.�C�@�-�4������Ŝ���0cZf����z\Spk��l���
�J���˭�}���eG���.8N
�<�2�� \��C�@:�(�Bzβ�Z&@�n�*?��i���9-|��r6�w��Yɳ7A)����:K}��U���p��G�$��w��h��� A[�GF�p^�=��X�U<�-2v�Cc�	�p9P���o�cJ��O҅�CB=���	�+8Y��S�&�
� �-�$a�!��?ҋ�\�G	wl&�&h��2��o�k�d��/38�������ӱ���=d�Zp���y|*�xć*֎�Rv^?;�l!�����bFz�A�cbbk�P����v����)����W���?P�#���P��xw�Hn�/8��JB�}��������[�^�?ǦpVk=j_���%�$��ƘP7���G�B�p�-����}9f*�v�+ ���GFN�eYQi���YI8h�QsJw}�$eW%�����;p����"��#�~�1#�h���U&��wE���0�s�>w/\-��X�-	Hh��
��)L�L"��[�9b?a��=�}���$��0���i~o��G�dT�m����e��@n�+�����&��������6JO�e��qeT>�Hn�\8��a��!S�1Z��Ux�7��'>��IO3�.rD�%ܖJ��A�t�&�w3Y�_�&"#�y��^��ieñ�AF��p�}�z��>��M�Ik&{X���D�1;��!Ki,�5S��ÑE�FX�B��������U*'����� �iBk���jmAVP��gr��t@^��������~'�,N��#�%��92ѕzK���@auiF�¸�M�V�;�ob���T��D\X>���N�^�q��p��Kg���t���3�Nm$�z�.�'b�&���з��\I���gyr���*)���]��ϩ7���U7�b�/d>��7�l�Jף���Nx�|J.�I��i8���	�Xʆ]*�B��ѯ'b3c����8����N���b��l%=
� �'�l��5���b�A�D�t�e�.�]�H�b����ةO�s½�ѝ�H��I̩����U3!EǏ����*�� P��� wB�=F�H�?&_�<L��H���oP�;.]}�	�j��3�IXRք��j0��)֩���U��j��*]�,�_����Җ����;&%R�&J�[Q�!R��'ձ
�H��4ݜ��c	�x���"�r�FF�_�j&C��6�앏�%���� �H亴7fx���n��4��\z�^JZvMM �Z��u�6ݕB���p!\��,F��[�?�_2�)��a|
��*\�zS��%�6"J��n�DW˚Q~�ٓ)Z%�6_�_� *ł*M@�x�} ^�����fCqg;����:8�� b���pb��tP+/=f�L�%t�T������NQ'�>U�T������f(49�w �16�3���4��p��D[��#P����z`�~��Q�O��A7a��H��xwA�"c+��Hbv��C�?ʝV�9y�!>x���L84�V�k�����S(��QB)oJ��(C\쌄_�"��vYzn���}XI~&�*o�JL]ᭆ�(c�`+�F[��.7V]�t&'"#�q�7�����`���ǵA�X�B �A�z�K����� G}���ڝ���y@��b��=�,��C��-nER(
�n}YWA����A�b�d���v Ǥv�~��7�%%C�(�6�Lx�w�k�o]p��)��離6��/>�7�uL<�����f�E���V�V��K@Z���ɫ<�fv�{~�fD"�\@��o?���*=�������?DQL��Y#X�+�@��w��f�tƞ(�`�����_��W{�i��߄Ea�j��]9_>���-B�4��<�Q'ZG<OR<di
������>3� .���P�`�X���e�S"�<�pw���p��%��V\��z�dӬ���L��ؘ9�0��բ����ʽ`���V����C[���L�9KL� uȤV�gX�NC7�d&���O���:��C��he$ݎJql1�L����)/��_^�y��if���5F�?�g�N.D3$�ٝt�6d�1#�l��NJͳ�6F˴MU%�Ng�4F�"��5��^�\Z~bc@'��a7=|�C'���oY:�[=e����L������c�o[v���y��q�Pov�ǅi�Pz��e��,��-���͕���V���T��� c�r�~��
��/�H�C����_ʫ���6�i,��0���7�늺����U��z�3t���@y��s���Th�Θ)v��{�Sn��:� "��}�>�c3 !M�R���ғ��)_9��:gɲ�����0	��n&���o���M*��xC����A�}{F���L�= у8�x7�+��u�y�R�;�UE4yo�	�	��O�z��s��p����TZ�?X�k=h�4�����͒��rW�F�}&.��s;ț��+nl����i;<� �:W�Gr����4�vSp9���#{���~8�[��	*�pǴ����Ҡ:��H,�����Wy���8�0\��͂]_c�>���Ǥi�����y�װ��'ü@���|�����=B0�Ċz�)hD��g@-�u����_=��c���H,�6�#_�;�|ф4Q@m:�_HX�����a-_��Y=��t�x�2=�-�&�ƶ�>��@kX�P��o��o���ѬZ#?���[k8p�V��m��6KC�uܶF���UH�N����U���nı-/^�\7=3_�9��t��>������449�#n"�ir��{����)���]w��Tl��$�?�lg�Ma����Dҁd0U��>��ܔ�z�z$+f��UW6j�bЍ� 96K�]�^�L��C4�ר�=���62��qU�1;x�Z�뇷ˉ��,B�ܵ�U�g���J�6ץ]"��%s�GrX�7�`ѡgm���K���T�Z�J9t��h���y�I�C��cy�Zu���-���ᴀ��e����'"v���R��A3X�VT��.�������a�_韂��78����TN�6�e�}�#F�>;�b���)o!��"z#�C�B��U}e,[�#`Ey��"���OL~� �nj3mR���r�c�(��B�,`jK�����X�����0�����+�X3����1��P�Nk���TGTntN�^�"�3x�P#����$�i���'2�p���$
ŧf��ҠA$�o�C8��"AF7}���n�����/�G�'��d�o�Ó��x
�O��
t	�И������*w�sՊ���ȝ �uW����qZ�tm��x�"9����2�\�j���}Siw;9�`��Goj I"gr���������f�p,������`kF�/�~UP�2����#�Ѕ ���֭��V���>�hS���S�a�\8�6�6��Z�n��9��c$n���Ï�85�{��@�f�e�@ �4�J:��)+��dڠP����J�d��D�Ded�0�nw�� ���Տ,�%��כ�C�%TiW����?���^��*�E6�X����]�H��Ġb��}d�42�lChKQ�3Z&�a_����i-<��ߺ��)"�P�[/��(q ��(������-�QMc��>�j����HMR�J[�3(ɒ1
���C�"o�\�I)�Y�g�.vA�gk*�R_��������m�$��H�"����w3�HG�б���k�~2Ad>���jJ�w�mxlK�i��	iq�]u�(�S.��A>��E94��R7,ܳzA""��D��x<��Q�N�ޥ$b4u�u0���x@�l�p�;�h[>=���b��2b�F=�d�l0"�k��XEu�$�0��7}���~S5�>G� �=��Yjڦ3N?�p��/}S�^�+Te���kD����*( j�|���^p�� 
p�WI~d�;I�NԜ�I����,î�Z��l�#�%l�3]�@%��$j�ܧ$���3yQL����ޣ�{K�,�ޟ�Ų��	x��2,c���]N]b�{z��3��D�ID���Xo���!U�L	+�
e�����&p�r��gP��|y�E����Ħ��px�p}���<K����Zɩ�c����L+����/(q�gH��&�<5�6�s����uQ��ޖ����?i�!�hW�=�8��{�ڞ���v)Vf�"��w��,R �ė�p%Q/�'0��r�M����j"����e�-g�A��7Z$P��E;��ӿ��Z���>=E"}���	+�W��%lχ��M�@�!��wK%gD�2(���b��vX�`�t1��W*���쥼V��^�w疊��rO]��s�~���ʸEguQB�%����V���k���x? %��y����&hL��~J�{�/����[�a譨/R@9��5�Tp1%"�&�=;���,�| [{+:v���{/X�A�y��t|J�+c�8zp쒧	1-�r��߰ E�O"�̞nBT��5��>�-���{C$�tGNy�]%e�ϴ����trW�n'�;u3S"ҵG Za�a��>�XX�:�D_By�2HW�E�R���	@*��@�$2�����e.��L5��F�H6���v E�����9ISe�URM1 ���{�p��1$�jE�m��v���(�l�L6`ŖJCf5���E����C^�_�%����\�U&��Mle�Vi<�2�� �h�v�^)8��x$/EtC��NT;�����^b���]�����Ɍ6�Pq�-^<�FQs��/% �L�O�������"X.v+Oy��o�"@[t^
�be5��ʡ�7>?KCɬ�~#��C��'*�� aI�E��ݪ
�F�������kǖ>Tv���N�p� �M��b�[Z/M�Ѩ��ŉ� ���=�m���h�85�?6aZ�of����ޟ���$����
��O6f���Uu�6C0��*o��?\A��8=������K�b޷4�����/��S���S8?R�["�td}r�-H��^��"Q�U�ǝ)�%���lRkk� �q��o���լ�������V�{���w�y�������'r,nZʦK=-w�GQ}��E��^?�^_>�a��b&�Z�V���j��,��4#��Q[�H�}5���;�3�h�:��1��B@#��$�{ ��;*M�)E���2*�҉��4�����[������.�H�8?:D5<�Κ��A��˟�I���Ї
������T/����j�e4�V����V �X��)��+J�+.�?�����-�oO�IY�
c	��_\�~h�5a�pP�*�dr�����?/�ea�+";��wr�q �šd.9�4i|�"��w�s��6����UEb�j�3x��`n3��~�<P�:�Qx�@�*�vP��6��:ܳq�GH���7�8�mcg��p����V�R�Z��!��<�7��0ly�Y^�8@D���2k���q�hZ��d�V
΂���b�����a��@o���i��n�E���	��_ᬟ����)�
���q�V�����m޷ 74��!b;��5>�S}��Z1����o�]�-�-0��k��Z�1���oK�i�(�i�	�g�И�몠�E�F*�c ��k�2�^��R��D�Łq�.8��|�n�#�F�|7�4�τ|YUJ\f�4�%$���$l�\�th�2�,�����C��J9u�j�d��e´���Yz��$��@��ˠ�o�M��sJӇ|�Jzh8�m��Ȉ7b�KxVc�6�I����"��`��Ux�% ����ܼ��1g>v�pPG�FY��:�M%����b��d='�������\GdYzD(|/]6���C����?ߛ��B"� >f�	0́���IG|��TX(�"�;r�����n�NY�]8��օ����D��w-K�����0���h9P404#�C��a�5�RC$�N�9�Z�-3�k���!��}�QU�D�s�̫t�6wv�M�m�om�����rd�o�W�)���!��I���N���GzՔ<�L̷T��*�"�������&R����YHS�{$�;N�,
V��:���^"x����w|���Mܵ�ye d`V��>�xm<�f5��3_�]`��vJ��U}��K)�g']��65�s��֦S�UIF��0��#�I�Y�`������C�)}�1��{z�&�K+h�G���MhH��ꊯ�����b�V�sAL���1ښx7�Ę|}0> ��K&Q���}��W��ü�6y=J?s&=��˵������$�2��d����}��h��:fA��DȮ��|��Gk�S�-���7w�Fa����֟�p�?�Qu$,!�*X�"d��2Z��)�t�@C������rX�N�F��s�e&���'�˻s���D�b�1c��Q��CV�9��$h�
u�o
/ԩ�E6���݈�}�8�L�y�z~��ͧP�x��V���� mcu�e��*Ьf�u�}�Q8T,�B�A��ޢ\��R��7'9��s�
�ٌ6�RD>S����{��ɪ?!��]��8-�b�`�?�I:��q�N94����=R�x7J�J����~t���%*D�,s�����)����l��?�ds��VZ׼p�a�:̆�4�O��^NIShA>�3��jpQ<���$����v��%K��r5�6`.I8�P�x��9���4�&���c�儏�%���N���#��@Q0^���4(3	's��p�������;Im)�ι�9/�t�1N���D"��o�)�+=Jp)g&��c�\H8��2x���I��JPBOm�\zGZ1�%�_�Fr�� zn��j'����ɩ�I����m��iR|��>�.E�Yxյ0Σ/��-+�Ã���`��k^8=�j��8eٿF���9����= ʹ���+斏Z䶝5��R�p�P'/CSB؁��\���U�R��nt�z���+)����M�'�6�q�u�-�\��	q#�A����m2d7����SR�i�kb�S$��:��d��c�e�D\$�h}(�7��I���L�����Z!q��1�t����ߩ�9�y�hm�жw�h�*xz/ �VA�r����#�	$��#Lة��'4�8�O�J�8����o@����x��O��*YzX�%�&�����s	M/�A�~���Lh�֧1���}؈��ư�gϔ����˹	|:A�L�ݑN�~8Y�K��EM�U��Lw����o}�5��N��e�w�WLl�y*$�i���Z7�	J��"�fj�mq&�:�߻�OeID���ȴsĞ����(&�%q��PR�A�c�~����2�W�m��iqf�&a�B>�3t�Ə����&�e8�d��L'�T6;�0�`r[���xNu��Z�s��(�뒊b���(�$���:�n�'�[
����q8��z�<A�(�W� ��q[Om^<���l/c���$��!��JӲ72DP���+�3�B��H.�����i
x�g��#�vj����E$������sk�>tn����6̳�v5_��S��ۆ�����;N�w�H3�7���S�IVs�>'�������K�5���2G�%��noI���f$���Sp�����2���~��V����i_ڳ�TE.����b��neN�/��.Y_w2�YV�~��ЍꫡK(	�,��ZoW�_M"R��jj���#�:`H�W'��d%AU��ٲҭ�:C��+���,F����m��ul���(�5�S�/�`�(�[ќ��hJ;�}ݼ�AZ��k<�?!$�֚��R�K��] ��D���FZg.Z
�!�����֘tǆ�0�������t��0�rQ����6�Ui�M�-�:��}6"������𓜿��,����{�̇`��1�E�$ߺ/t�BfR3�f�ԕ��.�Q>M���S�2�	���r�Z���XdJMd��l-�d��J��7����p}�gn�7�}����#�9�d�tB��|{Oi`rD c�Zy�<i��zm$:ñ�ҳip�ק;�b�I�w�'��zt�8��4�;s�s�}�l�V�X�(l��l�R�5N`��CEEb�zل�6k�;���ԋ1������slw8�N'��%�����io���XByZ��m���nHU��;��v�a2��>�+�d��� �Xc(�	YyP�Hʷ����Ozmޢ�qq6r��4wU�iT)?�Kd&��_��L1�e�[YC�Mj��R!��?(
(�����$0ֹ�� �;t�g�b��ukٽZH�O��ﵠ�`�Y���hxH�%�_��0xĵ��D�zK��&;c��<�Xר��FI>�3@�g�ױ����ΘN�@�cD~�&������~��oς���Z�3��}��XC�K��X�G��Qd-��r��R`�Y Z�̡I��^�v�)3��]�V0�h�R�RX,q��ڃ8\$�L�{	�o�1K�u[�QG��"���F�B��ߦ-?&���(<��&�G�p�]��
-�jпR�`+܅d�V�@R�.�x���=����Ո�/�A���ڲq��~|��wv������CDq[�d�~�V\��f�tέ�_�B�`��M��e��G��hn��\���͵MI\�'F^���K���D����i���Q���T4Ԏ���kE?�������B6��==� �a;�T��>��d�僐������l��t����+�T艰\�Ż���?��// SPDX-License-Identifier: GPL-2.0-or-later
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      F��:C:���>ט�3j#5�7$�/�C�bG�)����q������~}���� ^��2��ۇ9�N���c.�o�rOj�^��m��i�� ������ɢ���f���e��� ��0D�N��i\�wmM[Déh��z]��%�}yw.�t_��U`����8�Ǉ]&�!�j��->���#s@�w�!W�'=
���\�0`��u"�6�4����7� �R�N���k�u\%�=7.��M�l���;Ie٥���g�z�4�9�T��F�������"���P�^��^ѭ����	F��M �c�⵴����~��@���o�#t�VY&�7x���"���V"ʚe��$��b~����<�:��=q��ؒ�<��Ԁ��	>���h#2+H,5^�#����o�[��p��aX�ۢ>�Z��k9�UP��/����qwpHv�[��E��0l�l���j˹m�cC���^`�U; ��$#��YO��,�_�	*�>��^�{coa*3��Cr.+��
�2c"���@K��+��cW c.�-��� &:�`p�� ���k�I����\ɶLU/��yHrf0F�ʓN�0�Lf1�,:Խ��~��EH	D�3.+������t�� �h������:��Q�N֒q�/�G�RVbo`o�Zh�R
�t�r�eʗ��1�������F���փ8R�PuZ�=��s �/O%��#�Ȳ���Dv@�c��lr^z?��{l� �i\�(U[I�#�,s�b��3�}@#��._|k	�E�u&�&^�?�U���Jh�_1VKG��\��V�CH�圬w+T��ƓӾz���kM���@�ݎD�̮�B�e�} ��{S���(ְ�u�	������iX���Y�6T��P�BuRުa�ip^��ZeWf�),�����Ъq
:�U��k�L룯���P�T�E��;(�V*�9��=>8�*Z��F��u�(8���U�L�&�!�m�&Αv,��&@{R�QN��w_��o���D�Ƃ�;����{DyO{Ϟ�yX�?hM?'�s�U5�U�'�����:�:��O����9��}�/��t����o�`�:f����N�؊���.=%)��B������_�z]+�B{c�S0����(|�o�q�tI��ѹa�8tk'�4/�����&�>�����p��-�d�i��;1OF�~yW9��ˤٖ�@f��U�����=g���
�s��ЊL|��<��:�Nf/���Q>�e�o �5���k�.lO�����"ϼ�1k��,"Ȳ��/�-OֺP�x����'�7���,��O�j)�Hm>)�^�����闐z���&�;-0���B���%�#�T��G���Y�UNc�#,|#)fMy�f+���<��8�#y�K�瓭;� �OG��)wN��1�n�F���1
�`c�d!gr����ePl