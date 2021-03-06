# SPDX-License-Identifier: GPL-2.0-only
config VIDEO_CX23885
	tristate "Conexant cx23885 (2388x successor) support"
	depends on DVB_CORE && VIDEO_DEV && PCI && I2C && INPUT && SND
	select SND_PCM
	select I2C_ALGOBIT
	select VIDEO_TUNER
	select VIDEO_TVEEPROM
	depends on RC_CORE
	select VIDEOBUF2_DVB
	select VIDEOBUF2_DMA_SG
	select VIDEO_CX25840
	select VIDEO_CX2341X
	select VIDEO_CS3308
	select DVB_DIB7000P if MEDIA_SUBDRV_AUTOSELECT
	select DVB_DRXK if MEDIA_SUBDRV_AUTOSELECT
	select DVB_S5H1409 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_S5H1411 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_LGDT330X if MEDIA_SUBDRV_AUTOSELECT
	select DVB_ZL10353 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_TDA10048 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_LNBP21 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STV090x if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STB6100 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STV6110 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_CX24116 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_CX24117 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STV0900 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_DS3000 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_TS2020 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STV0367 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_TDA10071 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_A8293 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_MB86A20S if MEDIA_SUBDRV_AUTOSELECT
	select DVB_SI2165 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_SI2168 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_M88DS3103 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_MT2063 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_MT2131 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_XC2028 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_TDA8290 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_TDA18271 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_XC5000 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_SI2157 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_M88RS6000T if MEDIA_SUBDRV_AUTOSELECT
	select DVB_TUNER_DIB0070 if MEDIA_SUBDRV_AUTOSELECT
	help
	  This is a video4linux driver for Conexant 23885 based
	  TV cards.

	  To compile this driver as a module, choose M here: the
	  module will be called cx23885

config MEDIA_ALTERA_CI
	tristate "Altera FPGA based CI module"
	depends on VIDEO_CX23885 && DVB_CORE
	select ALTERA_STAPL
	help
	  An Altera FPGA CI module for NetUP Dual DVB-T/C RF CI card.

	  To compile this driver as a module, choose M here: the
	  module will be called altera-ci
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 # SPDX-License-Identifier: GPL-2.0-only
config VIDEO_CX23885
	tristate "Conexant cx23885 (2388x successor) support"
	depends on DVB_CORE && VIDEO_DEV && PCI && I2C && INPUT && SND
	select SND_PCM
	select I2C_ALGOBIT
	select VIDEO_TUNER
	select VIDEO_TVEEPROM
	depends on RC_CORE
	select VIDEOBUF2_DVB
	select VIDEOBUF2_DMA_SG
	select VIDEO_CX25840
	select VIDEO_CX2341X
	select VIDEO_CS3308
	select DVB_DIB7000P if MEDIA_SUBDRV_AUTOSELECT
	select DVB_DRXK if MEDIA_SUBDRV_AUTOSELECT
	select DVB_S5H1409 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_S5H1411 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_LGDT330X if MEDIA_SUBDRV_AUTOSELECT
	select DVB_ZL10353 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_TDA10048 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_LNBP21 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STV090x if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STB6100 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STV6110 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_CX24116 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_CX24117 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STV0900 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_DS3000 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_TS2020 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_STV0367 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_TDA10071 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_A8293 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_MB86A20S if MEDIA_SUBDRV_AUTOSELECT
	select DVB_SI2165 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_SI2168 if MEDIA_SUBDRV_AUTOSELECT
	select DVB_M88DS3103 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_MT2063 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_MT2131 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_XC2028 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_TDA8290 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_TDA18271 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_XC5000 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_SI2157 if MEDIA_SUBDRV_AUTOSELECT
	select MEDIA_TUNER_M88RS6000T if MEDIA_SUBDRV_AUTOSELECT
	select DVB_TUNER_DIB0070 if MEDIA_SUBDRV_AUTOSELECT
	help
	  This is a video4linux driver for Conexant 23885 based
	  TV cards.

	  To compile this driver as a module, choose M here: the
	  module will be called cx23885

config MEDIA_ALTERA_CI
	tristate "Altera FPGA based CI module"
	depends on VIDEO_CX23885 && DVB_CORE
	select ALTERA_STAPL
	help
	  An Altera FPGA CI module for NetUP Dual DVB-T/C RF CI card.

	  To compile this driver as a module, choose M here: the
	  module will be called altera-ci
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 +?Z????vY???z?y?_????.?G9??2qS'??F??Y?x]??f?@?H?qe??}r????E?7??e???L?w?qD?b?/?r
??B%,?k?f)?E?????????;???Qe?JC??}??Xts?r}gx??o?I??L?E?w???evl??^fu??r?V????r?~j????n??o????????Qx?D?JQ?2?????Y?v??o_??6??,??/???*?p???
?}?iL?t,!?Xc?<??
?c,$tE{?'?#d?	\???3??V}?$??Tee?Obxs??M???veYR[????m???I??I ?-?P	#???duZa?,????o?v?[?????a?a??XsT??????????Oc??C7???a\??M????>m S?$:??X??2Q(?}?m?????b??(Tf9^X?`K?f?y?1?q?0???????{*l????S????(??e???=?H?????+?}?q????e?Y????J??c???u????w?;?r=?JeQRL??U??U??????dQ?J>?c???Rc?-?"?2
ulT? 1?Y?G?\?gu&}?b`??Ms??f?/?^J?9?Y|??+?R???h<?w>t?;?w?;?,?X)?k??=RN??h??S
?4??*???G?r?6R???1?????{0?B??+??>??n????????iP"`7??98?)?????????{?F&??(^n?`??'?$???u?a]?lK??r?/??2<_v7K??n\???9oH?K)@?{? ?b???"?t?,F?F?9?|~?)?SJ?^?;??L4????e???')-?????N?
d??????B?oi?C?A

D?	q#??????+L??`~9??p%}4:=????,????$???Sp|???l?BF???mr???a???(yq?:??W??{^??9d??B??o???tV???^]@??Y{?6?? m?????F?f?viO????j?3+l{c??EI???	?3Qvo}~?i?9u8?? ???s??;????-?-??^????28r?????r??*$?$?-??????C?Xi??v.???\q????????h9??Rjux??<S>E??{???Uo?vP?V?;??p?????x?2}?????Xr??oE??1f????R,?G???#?'?+?N??,??????=??????}???-??oC???O???X?	\??*??^GYW??s???(??X+E.?(K????aj?$$<?????????7T?!$??^??????M??6|sT%??`???(T2????]WR98?D?<?????????+???;??A??<? ????4,???_???f???L??'x??5???Jg???i?&????"???^?<\3?NN~?????h*?%?ZFE?c
?	E??.??@<??R???3?????????9??<?3??????x??J??]??\K??J{????_o(??b)??,?`Z?|k????l???~??QY kB???R???KJ??_?H???t??????z?k??q1??vQ?b*?Uo0????zX?$??;????A&-F?caI?4?P|??i	?ws???6?A??r3?w???S??C??b{????I??	?8!A>??:3?x|?Qk?h??P??O??U:????)??E?#?wCq??r>??9?gP??N?=y1B#???????B??-?`@?d?ZV'?EN1?E(??.~r/????????W[~??
@????-????X"??C??`^HS???F?`[ ?? ^?%?PCjZ????*?????*?[??~ ?}!????Z@?/R?x%(??T???I?2=>??P?r????I?????1?D}??P? e?98?4Oa??"??O?,D???bM`c????m`?H6dU?C[g?JLc??G???h?Ei~V:?n???K_??by???w???4?????&g;?hS?c????u}?v??????8??m?=??/i0i???7??8\?????m&D-6`?d`1*???M?????u??uO?4n???C&?:l??Q????n???5?-???5<????t??r?????p????????@??ZD??=g???}g?????s?wdK*?
{._???L????Nia??z???b9?o)?MRI|?H?of????2???T????p?;?1?`)?/?djn#h?S;y?,???{??l?????vDc'?????P?x?/ont?I????_?????????JU?@R@$/	-?*???[?b????????a{
V!???YF~>??_~Z?3? ???'"]??$??I???;@????L? `4C????w?Mj#i???u)	???h???V?0r%?;5-??C>
?A??I???4?2?8??n?a??MA?{@???%?????????????oC????^?&P?~ydk#%j??{^6/?.k?8?ks;??u????? ?i?J(????H?T?!????-???*??y??;P??\R??????d?d??{4<+<`?C3_?y?}Q,?I??n)?[q,?w?!??6???"???L}?B?[?^??%KSy?3g&3???V???/&?(??>\?j$b?v(?9QF??n?`E??|??WP???lN???n4?7;:P???3N??X????Mj?kMB??t???)?[??z??1jB???+c]??????R?Qu??*wM??a"????E??U2??'`d8Q?,**?5?????a??<??1?O??1????{Iu??')???>???AO?V+t??????K???m?F?Wv?H???u]/??j???b4?w???9bu?5??fr??So???%?????????????A???$?N?-??,?3??Ojo}w?+0??B???E+?]Ul????2o-6?:5Y$?L??M	9?h5??o)5?9-??$M?T?q?h?????Ce??y???????????%r?D?????47?L?/?nS?1?sr??Q??q???????Q8S???r??????A??U?"p???*?#Ivpx???^0l?7?	????!?=?q??p??w dy??ys?B??H??#:????$???O/i???N?sM??b???Y??
S??#?o??u:$c!kA?fL???? ???H?m????J?Z}oZ??2???????D? H???b??,??	z????@a%???0F????^K?????i:#0?H??X\?m??:K????(z??????X???? e????????f????g??????UWxZ-?^'?8???8?????	?@?`?=?f?l???p??=!'???U????^?7y?????x?9-)???
4@?Y??`<?????b??H?}2???6????????4??p)y???\>?c?????
 C?4?d?M?_??G?5??V}????nV????0?x?????3VH?4???2?rC???Y=??d??U?ZJ??;.CCs?|7>i??g????L?j9`5??;wO?????#???????C?$??'??@`~??f,?n?5X?????\~?a??P>?3??w?9??K?????|L??\c???v????T????<?????
?#?AW4S?D?">??&w?m???%T?????#!%???q?otF?A_9??U??Zz?7????X?p?????m?????????H?Ld?i?F?`?Bm??^???#)???5??!????I???K???u2lY?s!GL??<?M??^? ?exg?	s\@?7?Z?8????P@??]}??????%?`??}i?5??????N???EI?H????(&?i?????.??8K???{???"??????/?Z?`7?a3~???0?!????????????mO??J??o? T???P????9R?p???9??-???9??N???1??pcT????I????y] ???:??9*?9JJ????3pidy???3??
g??2M????Q?el??"E???O??a??4`C???-f6??^{?N???vf????n2~?????????1??e????;??}???S|?{? ???u?}J??Sr??WE?"?$/???x??V{g??_?R???b??L-t?E?(@k?v?b?$n????`<?????dJ>?a???}?b#f??/t?2?D?????q? ?1?E?S?!??HA???82w??j??/`?~?F?*f?"b?8?@?Q???????r????X??U3J??L J*ED??F?>m?#???,?%???[1%z?D???E??|T?gJqp:?k??w8Q[?b-????uP?z?7`???UU?{Rh>v,[?l\nz	??????LG??E?$?=?bD??	? y?????'?? #HJ?hbl???C?bO?? ?5???lFU?hn???x?????k?|?R>?H9?>? ?k$?????`?'1?C~?Fk+?b_RsS??	???0?SC?BN|??????_?????|?{?i?:??????p?
s??L??N?"E??s??)??EgLJ?2?OA??f[?:?o<5????S?????I?????Mv?>?~@???\Y??x????????????BgI'?cZ:J??H???1 Y?O?(0K?X-?@g?G?X?q#?)>????? r??R?J???j???8??????m??!I??(?1??}o<????5?D{|???4?kp6?c?D?FG??6?\?%??N?`?;Xcsz??~u????9??'????0???????@?e?<???S????@????Y>J??Gd??2o??Vm?|-lV????N6XG???i5q?R8????$?3L.???CA2?QU?B???s?]?>??1OZ2??Lu??R?nu??	?? ?r???`(g???????y??sm?^x???0?????=???L?gk>[&???Qa???????r????2?Q??+??DU??q???h?????(?<??? Fa???@?z??G?V5?^[5??
??	(m?Ref???=?D???&{???]dot?x??q????????????????p\?{?2?y[M?>???Qij?"??(B??N???7?t??D???"_0W??^?	???k9)I(???uc,?? ???zk??Yu0?w??O?a?qge}p????f1?*3?8?V??;R?
???Tl????0Ip%???Y??i,?_?[??Y???"??d??dQ???C?T??+p?A??;W(?T+??T/Dr?^???kT?ly ??5V??o??v??C?{?X?/?pL/??a]E?o<??'&?v??????C%??W:B???o???M????????a??%?4?DK????]_???1U?&?N??t??Op??$0I?|?|}?
?}???&???P??Z???v?(0q;??g?w?o??*??z??])????~??&{n{???La7???e$?3????X"O?I?d?????=+?v????c???qr??5x? 2M???????S?????A9EE?%Lt????qip?/?G??
??Z?`?S?%???d^_???Hl$???J?.???Za?N8Gkn?i ??l%????A??p???"????x?0@=C????f4>w????@???d!?.???X{5>u?i??CU^_?????~g?js??3?????l|?,?s??:?????.-?
?[	?????R????*!4q?!?0gV?$?>R??????I??0??.???1F???V??R???(]??`??)?gz??4\(???uF??????.*a[??? ~????u?z?2?????=????iq?LNE1B?K?h??X@??????????)??F???N?9?(?(????%??Zu?-'H???f#&R??&??$qt??z???J??VN?u6s?7Y?be?7T???~???[{???L??vO:?T?!??_.Zg*PW\?3??LK????O?????k????19s??j?4???,* ??????y????????8nv?{??-e???)?+Kh|N?kv>??P??????-?0?"?}l?B?Pp?????i1fJ(?.V)???:???n???M?R{U? W?9KX]?\?%?f?Bw???7Z?o???=S??MZ<????????-??a>??J1??t?|?Q??a?t??M??0????{??7mG???..???+?vV?g?7?L??fD?????wv?e2fG??+?}cR5???Y??q7??+h%X??u[G[:=?v?????,:?J?(B?(?$?"???n<?x??@EP?a?$@?v????????|???a??+???`CAJ?PNP?^<'?????T?y??&????????,Qm<?1?<f??~????#4K?????IP(dR\NZw???Y??h4k?p-????T???Ak?????o?M???#`??????o	z????/Sg7?\?n?{3??|;?y\wS?H?????:??????F?X??	b?U?????Li??k(l???T?'? :e?k0?s?u?n?#?e?b?s??zJ??7.?^????^?4w? l?X?)??8?????????+,wy????)?e?}????e?^[W??^???L?8D??z?K??q[??h??WB?k?G?mfJ????l??) 	?4?????????tV??????Z??Ub?r{?I?09?r?????S?)O%E??a(?\?????#?I? p&'o???t	&O{#ZG2g???-&??T???&??/?>UB*
???~i?'?b??d??????????Oe??f?x\?0t???e6!2,?:k???c??S??L#?N?????M?V#???0"G0 ?.???H? r???P???,??c?hn?6??????-?i??9??=rt5?L?vDe???z??(?"???8?nwZ~h?n?? u?\Dn?THpU{3???kR??????m??????:?`a{w??? K#AG??????+S??Y?????f}?>???2N???L???#l??X?&??????Y?k?@#0???q8????^?)c4U???x&6??C?u??-?T?&y5?????B?=??d???????B?U-6?.)??????f3It?dD??5?????????x? ????|??Jt???l?+=e?)?K??:??,.?K?f???? be?????u??&.E??????bx7?"? ?????w???(?D?(s?jwx=#$????H	mQ??? ?"&?%f???JZ??hu=?i?????O?m?x??q???4<???5?%???S??	?G???`)P-??[t??D???rX"M]???;?n!{?????`????2?r7q?????G?YGne-???I.?????S?#?6?C6??jq3?$!???q??$8*2?z(???yS???2?b??????R*d?pe????V??1P?i??E????????Mo,F??)?5
?b?1?o'%#?RI???Q{?oEs?Q?2???????h????A?k????-?
Y?uG:
??C)?HX?^??|??b6???(??d?????7??]?k??????mM!!si+??YW????zgC??tv*!?#;??t?????r?
?]??R? ?"????d??\?\??%?X????.????U???d'?Hd??"??e?/X
??d?????N????L?	N~L??H{???s?)w'h???qXv?^??{?v??d???Q????7????l???2HM?k????o??IS???????+pT????P?J|)a?q+?H?????V??~}?????`F?W???c?vG???&?2?cF?
?Z
t$+ul-?rT???????rq??R>?L)u??S?=?Ni??	z?e???????\?v(@B=?W??r????2i&:h?]|???l??D:?Q?>???_V??g????????2??4???3?7o~m?RaZ?+??b?>??kG?Ja???dW?nb28???l??f??hm???????V(4?|?sO??d???Z??\ ?7???BG?i?!B????>?V??L????"??!???F??T?8???p(P?Nz??M
??%???Q?????G??Z?U??????N??Vg ?// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * altera-ci.c
 *
 *  CI driver in conjunction with NetUp Dual DVB-T/C RF CI card
 *
 * Copyright (C) 2010,2011 NetUP Inc.
 * Copyright (C) 2010,2011 Igor M. Liplianin <liplianin@netup.ru>
 */

/*
 * currently cx23885 GPIO's used.
 * GPIO-0 ~INT in
 * GPIO-1 TMS out
 * GPIO-2 ~reset chips out
 * GPIO-3 to GPIO-10 data/addr for CA in/out
 * GPIO-11 ~CS out
 * GPIO-12 AD_RG out
 * GPIO-13 ~WR out
 * GPIO-14 ~RD out
 * GPIO-15 ~RDY in
 * GPIO-16 TCK out
 * GPIO-17 TDO in
 * GPIO-18 TDI out
 */
/*
 *  Bit definitions for MC417_RWD and MC417_OEN registers
 * bits 31-16
 * +-----------+
 * | Reserved  |
 * +-----------+
 *   bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 * |  TDI  |  TDO  |  TCK  |  RDY# |  #RD  |  #WR  | AD_RG |  #CS  |
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 *  bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 * |  DATA7|  DATA6|  DATA5|  DATA4|  DATA3|  DATA2|  DATA1|  DATA0|
 * +-------+-------+-------+-------+