// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * netup-init.c
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "netup-init.h"

static void i2c_av_write(struct i2c_adapter *i2c, u16 reg, u8 val)
{
	int ret;
	u8 buf[3];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 3
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static void i2c_av_write4(struct i2c_adapter *i2c, u16 reg, u32 val)
{
	int ret;
	u8 buf[6];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 6
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val & 0xff;
	buf[3] = (val >> 8) & 0xff;
	buf[4] = (val >> 16) & 0xff;
	buf[5] = val >> 24;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static u8 i2c_av_read(struct i2c_adapter *i2c, u16 reg)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 2
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);

	msg.flags = I2C_M_RD;
	msg.len = 1;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c read error!\n", __func__);

	return buf[0];
}

static void i2c_av_and_or(struct i2c_adapter *i2c, u16 reg, unsigned and_mask,
								u8 or_value)
{
	i2c_av_write(i2c, reg, (i2c_av_read(i2c, reg) & and_mask) | or_value);
}
/* set 27MHz on AUX_CLK */
void netup_initialize(struct cx23885_dev *dev)
{
	struct cx23885_i2c *i2c_bus = &dev->i2c_bus[2];
	struct i2c_adapter *i2c = &i2c_bus->i2c_adap;

	/* Stop microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x00);

	/* Aux PLL frac for 27 MHz */
	i2c_av_write4(i2c, 0x114, 0xea0eb3);

	/* Aux PLL int for 27 MHz */
	i2c_av_write4(i2c, 0x110, 0x090319);

	/* start microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x10);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      ?2     4     (            GNU  ?       ?          ?????WV??????S???   ?|$??  ????????   ????  	???  ???      ? 1???_??????	????   ??   ??????????E????   ?? ??   ??  ????????   ??   ??[^_?f??????S?????  ??Hd?    ?D$D????????D$,=9? ?L  =7? ?   =?? ?6  =?? ?   =Q? ?   =O? ?   =?? ?
  =?? ?   =Wu ?   ??  =9K ?   ??  =?? ?   ??  =?? ?   ??  =?N ?   ??  =L ?   ??  =o> ?   ??  =e> ?   ?w  =?8 ?   ?f  =77 ?   ?U  =?6 ?   ?D  =?6 ?   ?3  =?6 ?   ?"  =?4 ?   ?  =?4 ?   ?   ='3 ?   =?2 ??   =?2 ??   =?0 ?   ??   =?, ?   ??   =?, ?   ??   =?, ?   ??   =?, ?   ??   =9+ ?   w}=? ?   wp=+ ?   = w^= ?W   = ?   wF= ?   w9=? ?   w,=? ?   w=a ?   w=V  ?????=?U  wPSh    ????????   ?o   ??&    ?v ??????Q?UWVS????   ?8??????   ???  ??w??ws[??^_]?????   ??,??   w???*u??????   ???   ??  ?
!????   ??    ?X?A ???????u????   ??  ??  	??*[??^_]??   ????`2 u???u@???p?????????   ???v ??????u????P?????&    1?[??^_]???&    ?h??t???(????C????P?????????[^_]???????2?????&    ?????S?????  ???	  ww??	?#  ?   ???'  ?Q  ??t?  ????  ?   ??? `  ?????? `  ????????d   ???????? @  ??????d   [???????&    ??)?  ??'?  ??!??  ?G  ??&?r  ??'??  ???   (  ????????     ???   ??   ??*  ?'  ????????     ???  ??   ??*  ?'  ????????     ?????????   ??*  ?'  ????????     ???  ??   ??*  ?'  ????????     ?????????   ??*  ?'  ????????     ???  ??   ??*  ?'  ????????     ???  ??   ??*  ?'  ????????     ???????????     ???  ????     ???????????   ??  ??    ??<   [???????&    f???vP????   ?   ?   ????????   ????????d   ??????   ????????d   [???????$?[  ?   ???`  ????????`  ??????   ????????   ????????   [??????v ???/  v]??uT???     ???   ??   ????????     ??????   ????????   ??  ??    ?[?f???t???u????   ??  ??    ?[??v ???  ?  ???h?????u????   ?6   ??(  ?   ???   ??$  ???      ??????   ????????      ???????   ????????   ??   ??   ?????   [???????&    ????  ??   ??t	?????????     ???   ??   ????????     ??????   ????????   ??  ??    ?[???
????????     ???   ??   ????????     ??????   ????????   ??  ??    ?[???    ???a??????n??????A????   ??? ?  ?????? ?  ????????d   ?????? ?  ????????d   ??????   ???   ??????   ????????   ??????   ????????   ????????   ??????   [??????t& ??1??  ??   ??5w\??3wc???   ?7   ??(  ?   ???  ??????  ????????d   ????????  [?????f???%??????>???f???8???0????   ?   ????????   ????????d   ??????   ?=?????-?c?????   ??1????????     ???  ????     ?????????d   ????????     ???  ????     ?????????7   ???   ??(  ?   ???   ??$  ???   ? ?  ??   1????   ??  [???*???????,?|???[???    ???     ???   ????     ?????????d   ????????     ?????7   ???   ??(  ? P  ???   ??$  ?   ???   ?X?????&    ??   ???   ??????   ????????d   ????????   [??????t& ???     ???   ????     ??????   ??????b???f??   ?   ????????   ????????d   ??????   ?I?????&    ???     ???   ????     ?????????d   ????????     ???  ?-?????&    ?t& ??????UWVS????0???  d?    ?D$,1????w  vE????w?   ??? ?ud???  ??t71??D$,d+    ??  ??0??[^_]??t& ?????   1???v???uh    1?j?????XZ????&    f???u??5   ??t??   ??????????  ????  ???  ???w????Pd????j????Z???_????    ?   ????  ?r????  ?   ?????1??2???f??   ??? ????   ??????????  ????  ???  ????????Pd?????????Z????????    ?    ???x  ?r???m  ?   ?????1???????v ?????????????????   ??????????  ???  ???}????Pd????p????Z???e????    ?   ????  ?z????  ?   ??????:?????&    ?v ????????????????   ??????????  ?????  ??tA?Pd???t8?z??t1?    ?    ???@  ?j???5  ?   ????????  ?v ????????Ad?@????   ?x????   ?   ??????   ?h????   ??????????  ?D$ ?D$
 ?D$???_????Ad?@??tw?x??tp?   ??????   ?h????   ??????????  ?D$
???????Ad?@???????X???????   ????tN?x??tG?????????????t& ??D$
????&    f??D$ ?D$
 ?D$?[?????????????  ?2????????????????   ????????  ???????????????  ?N????   ??????h????   1???????W????   ??????F?????????>??????????&    f?????????  ??3wpS????v???   ??? ?u?u3[????t
??t&??u????   ????????      [???&    f????   ???????????????      [?????&    ??&    ?????????  ??3w??v???   ???? ?u??u,??v ??t
??t??u????  ??t??   ??????t& ????  ??t??   ??????t& ?????WVS??????`  d?    ?D$1????9  ???  ????	t??  ????  ?H????1  ????&??  ??2??  ??  ??5?f  ??  ??;??  ??  ?H????\  ???     ???     ???     ??     ??      ??$     ??"??  ?J??   ????X???  j ??
  ?9   ?CjD????????  ZY????  ???   ???   ???  ???      ???  ????  ?Pd?????  ?R????  ?    ???|  ?I???q  ????????  ?o  ???t3??   ?H????X  ??     ??      ??$     ?v ???     ???     ???     ??"??  ????????B???????????"  ???  ?D$d+    ?3  ??[^_?f??H????%????P   ???  ?   ?    f???  ???????`  ???
  ???  ?????????  ???    ???   ?   ???     ???     ???      ???  ?G ??? ??     ???  ??      ??$     ??????t& ???????v ???  ??6??   j ???   ?C?A   jA????????  _X????????D$d+    ?  ????[^_??????H????J?????  f???3??   ???     ???     ???     ??     ??      ??$     ?B????M??????}???>?????6???????7?U???j ???   ?s?A   j@????????????A   ??j jA????????  ???????t& ??4??  ???     ???     ???     ??     ??      ??$     ?K?????    ??)wK??'?  ???     ???     ???     ??    ??      ??$     ?N??????-?????v,??2?;  ???     ???     ???     ???????,??????
  ?v ????   ????  ????????f????	  ????   ???     ???     ???     ??     ??      ??$     ???????&    ?P   ?    ??l  ?   f??n  ????????  ??????   ?H?????????   ??????? ??H  ??   ?m?????&??????H??   ?????????????????????&??  ??     ????      ??$     ??????t& ???r???v?H????????????&    ?v???????t????   ???????<??   ??`  u]?????????`  ???u????=?   ?tK??   ??????????  ????????t& ???m????5   ???O????1????t& ??`  ?????????   ????????|??????`  ??t?????????     ???     ???     ??????t& ????     ???     ???     ??     ??      ??$     ???????&    ???     ???   ???     ???       ??     ??      ??$     ???  ??   ?'  ??  ????????     ??????   ?'  ??  1???????v ???     ??????   ?'  ??  ????????   ??  ?:??????????    ??????????????!?	?????   ?'  ??  ?????????Hu????     ???????  ?O?????&    f????     ???     ???     ??     ??      ??$     ?5?????&    ???     ???     ???     ??????`   ?{????D??????\$?D$H   