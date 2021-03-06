!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				client_tuner = NULL;
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;

			dev->ts1.analog_fe.tuner_priv = client_tuner;
			memcpy(&dev->ts1.analog_fe.ops.tuner_ops,
			       &fe0->dvb.frontend->ops.tuner_ops,
			       sizeof(struct dvb_tuner_ops));
			break;
		}
		break;
	default:
		pr_info("%s: The frontend of your DVB/ATSC card  isn't supported yet\n",
			dev->name);
		break;
	}

	if ((NULL == fe0->dvb.frontend) || (fe1 && NULL == fe1->dvb.frontend)) {
		pr_err("%s: frontend initialization failed\n",
		       dev->name);
		goto frontend_detach;
	}

	/* define general-purpose callback pointer */
	fe0->dvb.frontend->callback = cx23885_tuner_callback;
	if (fe1)
		fe1->dvb.frontend->callback = cx23885_tuner_callback;
#if 0
	/* Ensure all frontends negotiate bus access */
	fe0->dvb.frontend->ops.ts_bus_ctrl = cx23885_dvb_bus_ctrl;
	if (fe1)
		fe1->dvb.frontend->ops.ts_bus_ctrl = cx23885_dvb_bus_ctrl;
#endif

	/* Put the tuner in standby to keep it quiet */
	call_all(dev, tuner, standby);

	if (fe0->dvb.frontend->ops.analog_ops.standby)
		fe0->dvb.frontend->ops.analog_ops.standby(fe0->dvb.frontend);

	/* register everything */
	ret = vb2_dvb_register_bus(&port->frontends, THIS_MODULE, port,
				   &dev->pci->dev, NULL,
				   adapter_nr, mfe_shared);
	if (ret)
		goto frontend_detach;

	ret = dvb_register_ci_mac(port);
	if (ret)
		goto frontend_detach;

	return 0;

frontend_detach:
	/* remove I2C client for SEC */
	client_sec = port->i2c_client_sec;
	if (client_sec) {
		module_put(client_sec->dev.driver->owner);
		i2c_unregister_device(client_sec);
		port->i2c_client_sec = NULL;
	}

	/* remove I2C client for tuner */
	client_tuner = port->i2c_client_tuner;
	if (client_tuner) {
		module_put(client_tuner->dev.driver->owner);
		i2c_unregister_device(client_tuner);
		port->i2c_client_tuner = NULL;
	}

	/* remove I2C client for demodulator */
	client_demod = port->i2c_client_demod;
	if (client_demod) {
		module_put(client_demod->dev.driver->owner);
		i2c_unregister_device(client_demod);
		port->i2c_client_demod = NULL;
	}

	port->gate_ctrl = NULL;
	vb2_dvb_dealloc_frontends(&port->frontends);
	return -EINVAL;
}

int cx23885_dvb_register(struct cx23885_tsport *port)
{

	struct vb2_dvb_frontend *fe0;
	struct cx23885_dev *dev = port->dev;
	int err, i;

	/* Here we need to allocate the correct number of frontends,
	 * as reflected in the cards struct. The reality is that currently
	 * no cx23885 boards support this - yet. But, if we don't modify this
	 * code then the second frontend would never be allocated (later)
	 * and fail with error before the attach in dvb_register().
	 * Without these changes we risk an OOPS later. The changes here
	 * are for safety, and should provide a good foundation for the
	 * future addition of any multi-frontend cx23885 based boards.
	 */
	pr_info("%s() allocating %d frontend(s)\n", __func__,
		port->num_frontends);

	for (i = 1; i <= port->num_frontends; i++) {
		struct vb2_queue *q;

		if (vb2_dvb_alloc_frontend(
			&port->frontends, i) == NULL) {
			pr_err("%s() failed to alloc\n", __func__);
			return -ENOMEM;
		}

		fe0 = vb2_dvb_get_frontend(&port->frontends, i);
		if (!fe0)
			return -EINVAL;

		dprintk(1, "%s\n", __func__);
		dprintk(1, " ->probed by Card=%d Name=%s, PCI %02x:%02x\n",
			dev->board,
			dev->name,
			dev->pci_bus,
			dev->pci_slot);

		/* dvb stuff */
		/* We have to init the queue for each frontend on a port. */
		pr_info("%s: cx23885 based dvb card\n", dev->name);
		q = &fe0->dvb.dvbq;
		q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
		q->gfp_flags = GFP_DMA32;
		q->min_buffers_needed = 2;
		q->drv_priv = port;
		q->buf_struct_size = sizeof(struct cx23885_buffer);
		q->ops = &dvb_qops;
		q->mem_ops = &vb2_dma_sg_memops;
		q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
		q->lock = &dev->lock;
		q->dev = &dev->pci->dev;

		err = vb2_queue_init(q);
		if (err < 0)
			return err;
	}
	err = dvb_register(port);
	if (err != 0)
		pr_err("%s() dvb_register failed err = %d\n",
		       __func__, err);

	return err;
}

int cx23885_dvb_unregister(struct cx23885_tsport *port)
{
	struct vb2_dvb_frontend *fe0;
	struct i2c_client *client;

	fe0 = vb2_dvb_get_frontend(&port->frontends, 1);

	if (fe0 && fe0->dvb.frontend)
		vb2_dvb_unregister_bus(&port->frontends);

	/* remove I2C client for CI */
	client = port->i2c_client_ci;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	/* remove I2C client for SEC */
	client = port->i2c_client_sec;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	/* remove I2C client for tuner */
	client = port->i2c_client_tuner;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	/* remove I2C client for demodulator */
	client = port->i2c_client_demod;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	switch (port->dev->board) {
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
		netup_ci_exit(port);
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		altera_ci_release(port->dev, port->nr);
		break;
	}

	port->gate_ctrl = NULL;

	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Silicon Labs C8051F300 microcontroller.
 *
 * It is used for LNB power control in TeVii S470,
 * TBS 6920 PCIe DVB-S2 cards.
 *
 * Microcontroller connected to cx23885 GPIO pins:
 * GPIO0 - data		- P0.3 F300
 * GPIO1 - reset	- P0.2 F300
 * GPIO2 - clk		- P0.1 F300
 * GPIO3 - busy		- P0.0 F300
 *
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@me.by>
 */

#include "cx23885.h"
#include "cx23885-f300.h"

#define F300_DATA	GPIO_0
#define F300_RESET	GPIO_1
#define F300_CLK	GPIO_2
#define F300_BUSY	GPIO_3

static void f300_set_line(struct cx23885_dev *dev, u32 line, u8 lvl)
{
	cx23885_gpio_enable(dev, line, 1);
	if (lvl == 1)
		cx23885_gpio_set(dev, line);
	else
		cx23885_gpio_clear(dev, line);
}

static u8 f300_get_line(struct cx23885_dev *dev, u32 line)
{
	cx23885_gpio_enable(dev, line, 0);

	return cx23885_gpio_get(dev, line);
}

static void f300_send_byte(struct cx23885_dev *dev, u8 dta)
{
	u8 i;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		f300_set_line(dev, F300_DATA, (dta & 0x80) >> 7);/* msb first */
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
	}
}

static u8 f300_get_byte(struct cx23885_dev *dev)
{
	u8 i, dta = 0;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
		dta |= f300_get_line(dev, F300_DATA);/* msb first */

	}

	return dta;
}

static u8 f300_xfer(struct dvb_frontend *fe, u8 *buf)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;
	u8 i, temp, ret = 0;

	temp = buf[0];
	for (i = 0; i < buf[0]; i++)
		temp += buf[i + 1];
	temp = (~temp + 1);/* get check sum */
	buf[1 + buf[0]] = temp;

	f300_set_line(dev, F300_RESET, 1);
	f300_set_line(dev, F300_CLK, 1);
	udelay(30);
	f300_set_line(dev, F300_DATA, 1);
	msleep(1);

	/* question: */
	f300_set_line(dev, F300_RESET, 0);/* begin to send data */
	msleep(1);

	f300_send_byte(dev, 0xe0);/* the slave address is 0xe0, write */
	msleep(1);

	temp = buf[0];
	temp += 2;
	for (i = 0; i < temp; i++)
		f300_send_byte(dev, buf[i]);

	f300_set_line(dev, F300_RESET, 1);/* sent data over */
	f300_set_line(dev, F300_DATA, 1);

	/* answer: */
	temp = 0;
	for (i = 0; ((i < 8) & (temp == 0)); i++) {
		msleep(1);
		if (f300_get_line(dev, F300_BUSY) == 0)
			temp = 1;
	}

	if (i > 7) {
		pr_err("%s: timeout, the slave no response\n",
								__func__);
		ret = 1; /* timeout, the slave no response */
	} else { /* the slave not busy, prepare for getting data */
		f300_set_line(dev, F300_RESET, 0);/*ready...*/
		msleep(1);
		f300_send_byte(dev, 0xe1);/* 0xe1 is Read */
		msleep(1);
		temp = f300_get_byte(dev);/*get the data length */
		if (temp > 14)
			temp = 14;

		for (i = 0; i < (temp + 1); i++)
			f300_get_byte(dev);/* get data to empty buffer */

		f300_set_line(dev, F300_RESET, 1);/* received data over */
		f300_set_line(dev, F300_DATA, 1);
	}

	return ret;
}

int f300_set_voltage(struct dvb_frontend *fe, enum fe_sec_voltage voltage)
{
	u8 buf[16];

	buf[0] = 0x05;
	buf[1] = 0x38;/* write port */
	buf[2] = 0x01;/* A port, lnb power */

	switch (voltage) {
	case SEC_VOLTAGE_13:
		buf[3] = 0x01;/* power on */
		buf[4] = 0x02;/* B port, H/V */
		buf[5] = 0x00;/*13V v*/
		break;
	case SEC_VOLTAGE_18:
		buf[3] = 0x01;
		buf[4] = 0x02;
		buf[5] = 0x01;/* 18V h*/
		break;
	case SEC_VOLTAGE_OFF:
		buf[3] = 0x00;/* power off */
		buf[4] = 0x00;
		buf[5] = 0x00;
		break;
	}

	return f300_xfer(fe, buf);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Silicon Labs C8051F300 microcontroller.
 *
 * It is used for LNB power control in TeVii S470,
 * TBS 6920 PCIe DVB-S2 cards.
 *
 * Microcontroller connected to cx23885 GPIO pins:
 * GPIO0 - data		- P0.3 F300
 * GPIO1 - reset	- P0.2 F300
 * GPIO2 - clk		- P0.1 F300
 * GPIO3 - busy		- P0.0 F300
 *
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@me.by>
 */

#include "cx23885.h"
#include "cx23885-f300.h"

#define F300_DATA	GPIO_0
#define F300_RESET	GPIO_1
#define F300_CLK	GPIO_2
#define F300_BUSY	GPIO_3

static void f300_set_line(struct cx23885_dev *dev, u32 line, u8 lvl)
{
	cx23885_gpio_enable(dev, line, 1);
	if (lvl == 1)
		cx23885_gpio_set(dev, line);
	else
		cx23885_gpio_clear(dev, line);
}

static u8 f300_get_line(struct cx23885_dev *dev, u32 line)
{
	cx23885_gpio_enable(dev, line, 0);

	return cx23885_gpio_get(dev, line);
}

static void f300_send_byte(struct cx23885_dev *dev, u8 dta)
{
	u8 i;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		f300_set_line(dev, F300_DATA, (dta & 0x80) >> 7);/* msb first */
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
	}
}

static u8 f300_get_byte(struct cx23885_dev *dev)
{
	u8 i, dta = 0;

	for (i = 0; i < 8; i++) {
		f300_set_line(dev, F300_CLK, 0);
		udelay(30);
		dta <<= 1;
		f300_set_line(dev, F300_CLK, 1);
		udelay(30);
		dta |= f300_get_line(dev, F300_DATA);/* msb first */

	}

	return dta;
}

static u8 f300_xfer(struct dvb_frontend *fe, u8 *buf)
{
	struct cx23885_tsport *port = fe->dvb->priv;
	struct cx23885_dev *dev = port->dev;
	u8 i, temp, ret = 0;

	temp = buf[0];
	for (i = 0; i < buf[0]; i++)
		temp += buf[i + 1];
	temp = (~temp + 1);/* get check sum */
	buf[1 + buf[0]] = temp;

	f300_set_line(dev, F300_RESET, 1);
	f300_set_line(dev, F300_CLK, 1);
	udelay(30);
	f300_set_line(dev, F300_DATA, 1);
	msleep(1);

	/* question: */
	f300_set_line(dev, F300_RESET, 0);/* begin to send data */
	msleep(1);

	f300_send_byte(dev, 0xe0);/* the slave address is 0xe0, write */
	msleep(1);

	temp = buf[0];
	temp += 2;
	for (i = 0; i < temp; i++)
		f300_send_byte(dev, buf[i]);

	f300_set_line(dev, F300_RESET, 1);/* sent data over */
	f300_set_line(dev, F300_DATA, 1);

	/* answer: */
	temp = 0;
	for (i = 0; ((i < 8) & (temp == 0)); i++) {
		msleep(1);
		if (f300_get_line(dev, F300_BUSY) == 0)
			temp = 1;
	}

	if (i > 7) {
		pr_err("%s: timeout, the slave no response\n",
								__func__);
		ret = 1; /* timeout, the slave no response */
	} else { /* the slave not busy, prepare for getting data */
		f300_set_line(dev, F300_RESET, 0);/*ready...*/
		msleep(1);
		f300_send_byte(dev, 0xe1);/* 0xe1 is Read */
		msleep(1);
		temp = f300_get_byte(dev);/*get the data length */
		if (temp > 14)
			temp = 14;

		for (i = 0; i < (temp + 1); i++)
			f300_get_byte(dev);/* get data to empty buffer */

		f300_set_line(dev, F300_RESET, 1);/* received data over */
		f300_set_line(dev, F300_DATA, 1);
	}

	return ret;
}

int f300_set_voltage(struct dvb_frontend *fe, enum fe_sec_voltage voltage)
{
	u8 buf[16];

	buf[0] = 0x05;
	buf[1] = 0x38;/* write port */
	buf[2] = 0x01;/* A port, lnb power */

	switch (voltage) {
	case SEC_VOLTAGE_13:
		buf[3] = 0x01;/* power on */
		buf[4] = 0x02;/* B port, H/V */
		buf[5] = 0x00;/*13V v*/
		break;
	case SEC_VOLTAGE_18:
		buf[3] = 0x01;
		buf[4] = 0x02;
		buf[5] = 0x01;/* 18V h*/
		break;
	case SEC_VOLTAGE_OFF:
		buf[3] = 0x00;/* power off */
		buf[4] = 0x00;
		buf[5] = 0x00;
		break;
	}

	return f300_xfer(fe, buf);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ????mo(:????M??I?????r?@?X[?qN?Z??:???B???B????/KF^? ?????8^?5]??#?K?F`??Cy~!;+???X??,???4????????6r;3??/Uh???%? 0???40t?)???A?>?J-?=??DT?xz?A?4!d:?(*?!1H???h????uVA???F?V???7	?Z?j??M??u?q(G?????@?????????!{??Q&!`?B????6????E??Rf???#????@0??v9?~??????-?>?/S??[|,3r???O#???eM?????@	??????'?c24W??y?0????8C?
?X?84?????9?4??W???%L?SfY???2?L??????v???b? aU}???x??Bf?????H?????K< ????K}?O??n??NN?B?A?rt??w???y?T???uU?_]S??hq??]??%?C?O???O???XS2J?A?I??BU????w<?1??s?????)????r_?m??'?`(???n{??(z??z???@?#?????q?/y??&*???e?[?N??Q????0qO?a?UC[8?0??~??J????H??r?rgk4cRHfz.=w)?????iFV??1B}i<???W??Z>????Re??v?WW?e????#A?2?%r???Y??2????*\h?n??????"?	???????b?????po?#z??????c?????B?e??????	??k??"???~Bkbi0?R-T?<5z<.?3?_|???[?BTa8,?
d#?????e5??[d,???N+???o|???"??q?X?c0??J?4<0?????????[?S??C?3???J?sW?????m??N)gc7?i???<5??U?V[?:??D?Y???q??X%????[?2???)????????{?'e??&??z;???QWO????????#?u?????W=?b???y?kb{?_l?{??T?38&?Q?!?*?aw??)*F2|]I;$?:6?????+@?z?g]?8???KcE???z?5?????dTVx? ??@1??#?3??????%?(7???@??<(f	?M??h?R*?&??*4??6????l?7?,??~=??????w"??n??|%??,?&?|????e??g~$5?O$???Q??i?>?E>C?
K? ? ??;l?^3??=????:2?a?eky?*???? ???r?????tl??M??Q?r?E???~??????? ??3}?h?h?L??C#?
R?P?srd??`!e.,*(:? ??dV(,?? ?dg??????:?>??F^E???A+!?/x??7t[g?????#?k????0~&&v;???z?y/????"?)?G????L#5?M^?&????L?????E?nw8????<?uoqA??L&?????G?E?????sZ???IgS????X??)????I???????'W?>?lP????&?9i?j.WD? R??*???????]X?1???T-???D?q<?|ST?????;???2R?1?;.pa-?.?rh?t"&?k????/zo?Y?Uy?J?J??w??s#O???q?a'??H??8?iy???~??w:_?2??i~0U?a'? QZ????k?\?????I?q????Ir^gg?)??????
?: M6???1?zD??7-???Z???U???b?;????{}??Q?*????d?
r?W??a?m???tYMu?T????`t?K???u??????R[F???o???;s????V????}5j~
o???)?????:?\?E*J???L7I@???z???8e??-^???????JI??D@?E??_?TwU??s???????????,???#??m?Dn??)Pb??Mj?8b???
?
?q?[?.?wX]??n?2+????[???'??C?kR?????;???Rb?.	????)u????P^??0?xG??r????m???.=h"???|?[?D?=s??\?\U?4l?;???l?????v? ????)?Rw??D???qf^???I?????????7K^Y:?C?,D`?d?R?J`??}dG????????y????????????sr????v%???????8?*?lT?0?z?\?*az?8?{?g??_??a???+M????/?Kn?3;???/X?Z???T?'u?/?????5?f??~????x??61???h????`?\??1*?M???|1?Q??r?Iz?Y??????????5????[O?t?e`v????b?X???1,t??8??r`lY?? ???F_?(ley??$u6%'??!B??v?a?S`????Z??68?????k??????g???TSV??.?$5????????	?T???(?S	yrhp???$H???;(?,???i$[???NH?\?? x?l??e?3??????.rK?b?????y???n??? ???????Q???@????QL?S??_?ai????-?&?-?r?F?i	?Z?s<9????<"?3????3Eb"???*d?A????!?]h?????)??x???E???????????{N>('?V?1????J??GJf75?ue??1?? ?Q???O[????d1??,????@rve????Rh???(Y????????[????$C??Y????M????$??@L?m?l???}??W?Bw?m?|?b??BF?,V?{?2B#(?"-i???_?W ?????"???S?????6?????????Hf?^Y (?s%VD???2?q??y?1?h?????I?&???=,t???bXQ?h?[?r?y/???KwvEN??Px@{?????L??<??????B/?V????MF?!C??.{?~?t???XR????^i?>???	???]#??????y ?9??:Kr???E???JTN? Fq??_q y?????]??J???.??~?@,?f?96?6~D?ug?Z??????yb/e???$???vh???????d??+???????T???????W/??^Y3?Gn??%[fA-?o?2|S?t5?#?/????~6g???s???j?h?????e%??I?V1?@???6;????????+kQY#??I"??>`k?B9_J??b[?3#E?|???p???G?wh??g???+?R??r?j??H%?+?v??????6???"xshs???B?????U?%g?f?`??U????p????o?<>???C?????&??"%?l?$bxm_!?1?A?q&???????sT??#??qN]???g??74B?h??Q?????d??c???:n?X:h?H5??O30Xvt??B?^m?)a?^}?5wz6??}????8]vU???Z=$m??
?Y???B????0???/??u?????????OB7???_'?p-ga?????w??T-88?h??i???9?wDB??[???*?^#?S?g??????R?	$??H?S???\?????A?$????8?M??y???n?P?&,?/X(Q~%CB???B?<???????T7?l???L?2`?P?>??]???????K???$????????????Ki?z???????|?U9???raUc,???.\B?&?S?%?"Y??X	????I ??dV?8?]?ab?~p>??i??!?B????%???6