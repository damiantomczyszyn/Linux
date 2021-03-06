

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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             ?$????????q??d?S?????-~?????;??????D?????k4?HI;???v??e? =v\iqKd]??6%?&?x?&cT?P??0SRXp?????o?Oq(???
l?{?g??3??On??q?HA?44?y??\?H???i???D?4s??WZ?:????s?j??????|??~e??,?q*L?{?G?o?+V??u ?v:Q??8?";??*?NFS#!?M?K???m[???")?&?c?p????2S????h???)q/?;K?3l?h??,?Ae?`???O??h?S)????60Z?>.??p?gu+?=-?+f???$??z?O|?
?^??o?V?E?>??d@d??Bl?TC?K????'??t??b* ?JuG|?}??r9??/?H?oa??+?v??n??+??|?????|??Yl??P?%?
????P?V????????T#?k#%,?i4i????\?%T???"??u???3Jr??8	?]??=2?,?(D??????=RT>?Sb~?? Z??+l3?_?Z???H?N&?v?Cxu'?G?>?bW)???,?o??D?T???{*=????[M???;??&??P?,??????[:?.N!??q?Xe
??F??<0?K???	?????*???s???|m?s&?r??6??g???"H?q?	??f???m-??B????X?VC?"i?*?m???????U?PN<?n??[k?Y?????u????^?\r?"?-???? ?[??ej-???&/???s?5?W4?I?bB??n???C???t?Wk?????8?????2?:+????8????k6?]|
?2 ????gt??$?D???Z??  X??>\?Q!??N$p???	)??J????m?L?\_?gV??:~/?D?YM??R??A?J

 9????!U??Y??/<{?o????}? ??E??9?????????G|?)???X?@?3w??lM]??GN??M??Ax???CW?0Q?-F?i???d???hD??J??j?\Y?c<??y?A????+???M1?/?wg?J|K???"???&?F,??Vg=?"?19^?:?,+^???Q????3?????Cl<w?`?6??)??W???;??ra??	???????I??$??oU? ???}??)???'9b?V???M??a????np?ct?x????\Q???#L????<?`??"???????eb?|???z????????????
W?( p^(L????X???0??@???m???D?I@RzWA?Vr?x?: !?VL????#D???o?$??E?[?Na???????TZ??X????iG?-????C????n??E,Jx???PwoN?s?R?-[???E????3b??1T?n????T???yMC??:????h?mJ??Wd?,O?<??]?P?1??"??Dqh??????????=??+??wI?=S?2???/?M?wV??n?mc1??:?? ?? _\?T??C`????t??D?P3?yDR Q5?9??b_F??~??@?C??A???EI??N?5??p?,?0E??????%????????????h?gz?p??????-y??????u?{??/"??]?O$??AOv?NX??U]?????kE???F?It?t?I6[cu7L)R??-?A}?g4q? ,?c5??o?hfz)??g`??h?G???U?'?V?(?_??l???5??z?_??^????]?O??i?bI?qi)?0w(???]??&??????`a?g?h?/H??<ln1?;???,???_v??;??oF??zu~5?_N=j?yU?[??N???{h?o????n?aY&*?h7??bg?0?Rrd???`>4
x,????1?????8w.????9h?K3c^??Yj#T??i?u3K"??_>?m????????>??.O z????????^????1?p@??/?lc?????L?????b??[O????_??n??f?BG?3?U}yTX?t6?,|?Wa6!?EG
L???h0S?????????+p??l?x?B1?jO??o?D?V??~x????N1?????#???????{???9??[???	?I?[???7
??w??????? ??^q?9{s?u??M?4???3???7{??0????z@W?B?1?J? ?H??`????? ????s0???Ws???????=4?&?B?^?&C?&nK????l2*G?+.????m???+z`??.
j7m(u??V?#??t????w?????99????m'?|???f?????t???+????n??*?#-?,@??DM??a??=aZb?]????*???r?????????3h?a?N?2??????Xl%x??za??u??|/???x?pE???	4g?%JN??[V"1???<!eA??F?^?7?v1???????j?(o??R??&??Z???0???{?}?????~?JX_mw??n?##V?7U?I?O????:?$??/????F0?>??4???nj	???c????
?d?gl?GUO9??H?+??(??|?????&?28?XE?lf????s?z*???m??/??X??????l"U"?s??].?]????I?q??9F?[0?$u? B?MO`P*?p??i??????
?OM?????ph?5g??O??t?A????7?#g?;ik?G?s??hT _?x??:?????I???;?/??b??s$?#???w?d[?S??'>]+v???N?{??u????&?,?p??}?????RX]??V>Qqsg`R???ogIzR,?D??I/9?X?ir8??V??J????
Y??20?k?0`??| ?%l(8M<??7????1{??S=W? ???N'W??dC?????v?r/`5?2-A?,?4???5????cxb=?Ci????!}???	,????[z??Y ??;???????JY%???????? ?}?$#????C??.HM???B?*oJO?s)?????T>jme???b?49A^???7??#5LZww.)_)j'??J:='EB?_???*BgT} ??[$???(C}??R?s??M+?I?\???_?%???N_,t?k??<??w7???yL?U?J?????????Znd????:?O?d????2?}???Ak???_%MHKEQ?B??%?g7?k?m?I?????????|D?????,??~N72????Z+uk?]??F?z?&y4?j?f]???|?'k>??R?M^?3?X?_y?(n???% ????.,1??k??MDOu?>?~Hn.?C?@?-?4???????????0cZf????z\Spk??l???
?J??????}???eG???.8N
?<?2?? \??C?@:?(?Bz???Z&@?n?*???i???9-|??r6?w??Y??7A)????:K}??U????p??G?$??w??h??? A[?GF?p^?=??X?U<?-2v?Cc?	?p9P???o?cJ??O???CB=???	?+8Y??S?&?
? ?-?$a?!??????\?G	wl&?&h???2??o?k?d??/38?????????????=d?Zp???y|*?x??*???Rv^?;?l!?????bFz?A?cbbk?P??????v????)????W????P?#???P???xw?Hn?/8??JB?}?????????[?^????pVk=j_???%?$????P7???G?B?p?-????}9f*?v?+ ???GFN?eYQi???YI8h?QsJw???}?$eW%?????;p????"??#?~?1#?h???U&??wE????0?s?>w/\-??X?-	Hh??
??)L?L"??[?9b?a???=?}???$???0???i~o??G?dT?m????e??@n?+?????&?????????6JO?e??qeT>??Hn?\8??a??!S?1Z??Ux?7??'>??IO3?.rD?%??J??A?t?&?w3Y?_?&"#?y??^??ie???AF??p?}??z??>??M?Ik&{X???D?1;??!Ki,?5S?????E?FX?B????????U*'????? ?iBk???jmAVP??gr??t@^????????~'?,N??#?%??92??zK???@auiF????M?V?;?ob???T??D\X>???N??^?q???p??Kg???t???3?Nm$?z?.??'b?&???????\I???gyr???*)???]????7???U7?b?/d>??7?l?J?????Nx?|J.?I??i8???	?X??]*?B????'b3c????8????N???b??l%=
? ?'?l???5???b?A?D?t?e??.?]?H?b??????????O?s??????H???I????????U3!E??????*?? P??? wB?=F?H????&_?<L??H????oP?;.]}?	?j??3?IXR????j0??)?????U??j??*]?,?_??????????;&%R?&J?[Q?!R??'??
?H??4????c	?x???"?r?FF?_?j&C???6