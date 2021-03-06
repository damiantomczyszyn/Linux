e        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   E????????????5wDf???>??{i??????_`???x??X?k??
?X?a-@?????oO??5j?a??&D@,0?:-?M???Y#?"TV??????oYYfJ?-???????@?,?cr?~?T?&??m?C???&#j?P?x??9M??C??p???VEC?{z?k2h??[wK???Pa???r??4*???>4???HGX??7\?????\?????mE?:???????TQ?tt?n?r???E??????ok#?k???b?EY?
??T????o?|????????7????????d?_??????6??&h^??n?I? ????k?????t?{??2???@?\ HW{??+????Z?$??r?"??{???(s??5<?????/4?U?`???^???L?f?w????$????0??&2?\ ?h(d??H??l?vM?oT?g????V?2?x?4??{??U????|4p{??J`??N?tT?????G Jx?=@ c??@rh{?j	-?v{???????OX??e??e??0??l???5!????I?????? ?a?qz1%???%?y??Cw?L?6???????L?????[??c????~.???*?M??z+?????U??:>??????*tPhi??Q?(??? z?/?D?@??/???}???I~rmW??/??D@S@:Qp????<CJu?[??F?]D??R@y?&??&
?f????T ?gJ?j@3E?v^?)C?X??/??l??T?KN3jf??????a??\i?g??6,?=|Y????S?g??????pI??Wno???e]1??,k)??]%???p??:]H??????bM? ???Z??????yPZ???+zg@t?K???1???B???`?
 ?/??P-??l??I?H<???1??\??jE???k??????8??????????U??c?E????b,????????,n?Y??J??$>M????2rW"??3??_`????8Y+?T6-w? ???B@Z?zq??)?????0?????}?'??????z?h??a?1"?W7m.???h?q?S9???A?t??_?N?R?2????S?7??p????:???i?ZCH?a????yz0?^m??p?u#X$g4?"U?????Cu?!xc??C?^??>UH?????R???/?/??i??/????LT?D??K??????U5UC???j??p??-?l??`????6???r#?????chQ[?jL??"j2~????>H1)qJ$?U???Q~Q??.?Be????7?J??G?y????.?r?U?0?)???l??)S???;?@??$?????????'?cR1o#_??9^??z22?*??Pk`l???*?????#|'?YJ?SV????L8+?B?	???+???;0????m??w???M??L?8u??K??>S%(S7k?.? "?w?gY?7??'?0!y????L????"?/???z??????MU?D????$???"?Q?????+59????-? ???????F?????y y?l?cjg???y?v?"?&?D??3?T?2?P??L7?p??5??q)m??yS????l??!?/O?,N??Zp???P??c?^Q~??M?Z??m5??n?????????????Z??P?\?*??t???X?D??(i?G^???"?????N?????J???K?6}?R#:?u????q??Qm???????@FMg???pM??7P*e?2*a?mAk~Oj;y\q???;??I?N]????[o^?F??\b3"???W????r	??????K9??kgt??0??Jx???$??????&?CqP?%??7??i???J?ox??1?????*?sA0??????<	vw??t ?S?|?????A???u?2s?{???zJ??????6f?sg?6???wQ?R????? ???
?~???@((%Om?F?8?5?c?`?2=?K??hMT??l?5l92A??L?-??f???v????(?4?????C>_x?AB??j? ???~O?@???|??b?B???fvqx%#??T??W??N???e????????}?Qs??g??????t?????????Q?+?_?'?i;o????}?J?=?*?a-??(K?	??"?6??g?????_??#,??9'mJ????1???dC??=JfI?t?bV'????????hh??(S??o?
?]??HD??9&??lx??Gr1R?$????y??>3??\??(? ??B/6??????? ?'a?L5??(??r?jG??M?Dj??1???e???6B/??F ???_???1????%%iU
D?15$en???????????????????f?B?O????a[?!5o??h???vI??M?eo????<VY`?5??w ?*Vm}%?V?\7sL?xf???^LfhB?51n?(2???8u?????????P?.y???VVM???[?A?M?V@??c??s???wj6???????J???X??w"?;?z?~?"??(?????????@????I?n?????B?Q??kT?gz3?t??nP!)?|?8??4]A`j:?(?????d??
????V??H??mZ0??????0CZ?H?i??B?z?u;?R??6???R^?$Cv??C{?"???|?6bQt\v>???]!?e??@?N???$?????w?>t??P?L??????9
???L|?S[??bZ?????v??????l?:????m?)?( ???S?? ?<c9???????????q3??:+?!??P)io?????"t1)????.	^?%??????G?b+???m?d??s?;?;,?S??N?{qD?T?J}WXu??!?e??mm1??AMh9?7=?~?2?gu????G?P4?'? S???9Y?????????&6B?? p??=?:?????f?B?v ???m<e??1^[??V???~??O3?????gyv??K??J??NYfHyG????????e????jg?n???????????h?1???l?x??????S?;?U????B??%?{????????? 9[N??>u??T? ?\y ??pttf?W@???7?t?7?4??$?	1?c??r???>Fm??A??^????cP9L???r?l???/?h4?+)3`?FX?yU?d??*Z???H????0K?.j?D?TW??z7T8ap????Q?H??Th}?AO&???Ry??s'X?%-	??|j	"}?F?y???=????&v??<'?#????K}?o??Z7??UJ??,Yiu?????Y??e?d???'??[%c??k?z?L??0?uw??r?????e???????7?|????'??`P?????q*h?f?$?????B'L?O??~???iUK?k3??YF?e+?sN?e????7j?)8?l90??C?v??????c>??%???????????????"??G#???U?@??!Y?2???ayE?A.n??0?????7bL??"?ah]?????DM?we???"*?G??U?#????? ?v??????b????I???8??7$???J)??(??tM-v*:G?|?Wx?"???K?j?`??[????n"U?9]????v0??6?\E?X?R?(\G??~???????`7%????D?????U1vK?]GU?y?X???++/A5QS?y)
??n:C????f??5=?N?`???~??
=/?b?L?????mv?Y??k L`?0??/??3H[????0??b??M5V0??????j??|?'t??;??@???>??q
N?I4 ?}????aAD?Fp???6^?^??rg,m???????G??Mm??????????pf?B??c?k?6??X??m??6y\?O???Q???????>????>,n??Z???c?_?/??5??!6.??vO?m??Ip??9=??K??b2??*??????$?? /W????f?~LC| ?Y?h??L?33?7G9?7?9/??????H??u??????*{<A??cYL@??m?S?e?&???????????'S????W?6??[?k?????3??&??}?0???1????V'6?}??-????}Xi?=[?{???????e{-/????L?????????c????U??	&u?4K8?.?n.?????e6F-0,?yP9y~??<R4?\?"C-???]?t@??)9????,???????:.fM?c??1G??<?%?/?U"g??Q??f??;???#??5VWC}????^?+?N;????]b ?????iS??g=?=??+6e??x?\?wp0#????E???8?N???J[V?(??yy?0Gj2??n^\??G? ?.???W??T?4?-???j?X??j;????#?L?J??|??Nd5#???B??Z?yo??k???K/???
	3?3fAgw??????o?????qTJb???|?R9tR???0??p???????-????9V"?v?????????4??r????w???,?Q6?????6Q?f???B#L????7<???L?????	??o?????b?"W??{r?,T??I?9????AQ?w?Ved??o=??,?i??*???????
6~?^?~NkIk?i???Xk?%?%?)?+???????Jr?6?}?Q'???H?????\?H,)??S??gP?C??e?L???oE??sV?e??G?H?T?????Y?G?&C??6@?e?23?Q??/I???h?G?C ?Qa??I (zP????J??k????????m??S?!?[????-M?E??r?c???e?_??X?	G ??'ZI??z?]?^???z?"A?Ea?L?g7"?y ???v?'12?
???j[/=V???,???1l[?g?aC?A???)i??v?V???y?g???Q??(???6???<?i?1M?CQ??=?P?+?K??=]?R??|?(T?:*??$m?n,?gZ0?????,?P53.?'~??5????M???y?}??????0Z?~??g*??'x??P?	??????"???~U? ??f?s???qq??u????:~%????}q'??? ??*?5=?<?Y??`?h??3??	??=??^t
1?]dysS? ?"??W???Y?C?x?L4???6?????e??
??X?:?K??A??K?(i {.zSC&(???l?MU #??-1+???????E?9F??i+YEBEL????*v?,??:????qXJ??<???ZNd?HHcp?^????Q|??m?@Lx???h {L)?????-??>1??(6??|?(_??!?eIHz???`???H???????|n?j\??m9???????|?;?????????'MY?#?Gb??*:??'?s$uk?xM;???????N??1?d?????x???h1??*???????-k??)A?i???D? :?q????}w??xb??S]??r$??Zx??????K?O-?????({??;(???z|;Y??v?F??"??????@8??S?? ??`?????!?A??4]?_???
??????5??????Ws????qX4Cw??+?????F???oa???+?#S??K???z??e???A?!?*?????V'?J=????6VF?h[??
s????F> 7?u???ta??,M????????
?y???md??????????7?MV9%????v?F?a4??&?????????8????3bk??-???yw?????i??l?A??????.fzgE???K?	?}???????W??o-@????G????O?U0h~?z??-?E?u?*?=?NA ?Zx~?z?m?+?Y0	o?W?;????????'??q??????=??N?&?E???(??~G?????????5??L??u?
?3a? ??C}A??N'BU?q??M?"???????????p????jMR?+al???$P?g]t?R??? ?X?!???l`X??p??J???T[-??????	??????5?????}"?5?????0
T??????Z???y??q?\=y_M??|??hg?([ml?B+????????>?,q????H?/??????H?E?u?????+??????|?+8Z?J?z???<?Tf?hY?Ma??8????rL.2??i?lS$??U??i?????E{?w?	?????K?V?4Jqs??;??0@QV??????G ?B-?~q?X?=>?2%HE??R}????=.?????7?z????=
??R|????mXw???7??E??o?],\?????? ?&?[?j?	.R?
L?^?? ?????K?-??
???i?O	S,?%FM?Cf??bg|??gL?L??b??7o?C?x?e?????TH?j????^???O????fn??f?SP??p?l`w)?`?d?EC?Ef?Fd???????n???X????{T$-???^??JS?+??==????4`W???Z??`[??????53?sR??{VW_C??-?zK???????Q_?X?????$??R ;>?`?????@ ?#??-??Wf? ?o?fj?$?Y??\?B???bY?P??\?????Ui0?>k???????H?????"U^?????0>????>????"5?D??k~??z???J???` t?????OI|?0??ei-Z?????%;?$g}U?5q)??C"????f(???5???ll^??^<??i?%?d??O`,5H???X?G?|DRQ??;?M??D??:??? vB?uV|?????2??*????^R??[t|??d7G?)?????9x?????k??d?&;G?'?!???R??3?9*\??~o?????5??M?|???Q?e&Y{?<^??g '??>?q[??eH\a?,?y????V?|BC?????r??] ?uK_?8??x??X?!?L?6?QS????9?N??d??ZE???\9????9?4KHXI([??P??je???P????#?!?36????????m??x#0P??W??%??.'?/K>=L??eHL$$?R;?R 5?lKI?$|?8??CwD7??uz?ix6?Q[d??????9?<?????n:m???&U??'E?n???8v/=)U3??????
??X?(??p????5gu?`bB.?7??],9??????Z%?[?k}?]???0??nr???SLd]? ???b??????o?L?u??A?,[?v?=?z]?':???
???UET?m ?0e`????"GR???&C?~??????N?????y9\r)?@?M?[???@????P?w?\8@?[??\7??>~???	???V?k????%?????^?W|d?????
?-?$??BR5sEp1????????@h????R??csh?1O???>??G?????$<??? 'A??.???G?g?????"q\?????Z{K ????9?K?UD?.?[e??}4W???((????[]_?e,?_???j?\?a??4?;???!E? ????S????x?<?mp??@~A??<u??=(?0I?3a??ss??Y0????)???~^6[??%_?@??%?Z?M5??H??a?=???Sp??3??#4"?
~?wzkEis???r??W???
?;???(V??1	'?#gN?t,5?q??^????~evh%kQ?z?o????t??Z	ej%?[??(k?????????
B??k/? g????_?wj???3???e??z?jF?Y?"??j9?g<C?^GF?Z?????????tL#???7;????v!!??j$????????m~8?ru?????yq?'O ?P ??D?$k?&??xXZd.???}??'w/?+???~v?2?????M,_????-?o???:B??SS?2?2?U_`S???,EX\t??????3B?	L*l????N$???
?????2<y)?"f???6v???	a/????`???3}?&?8!,h-D?G?????B?y~??O??s
?Jb?????N????Ud??:????8(o???O+???@???w????ja?^m??I?S??L|}?/?4???lqtc?Od????j??R!!?}?z??Y|a??m?W???L??l*?2??+4??0??i??_??}??a??7\?+???<'u??????+.?;4_>?LUc?Q ??L;???????![+?5?????q ??????+????1s?j?l???S??VFR*"m??L?`????z??????????(u????O??~G?3????1g??Tc;P????EQ??????????q\??v\=??M _s???	?I[??5??D?_?v?Uki??zl??3????B+~??U?0p????????`??v?E??*????!?v?q?aM?>e?Lh	?#&??lOi??}Y? ?|(???N-]??z???;??6????	(S)i8Ag?@?:???.x???B?>!??????t????k???/??????e?|tS!?/?????mM???m????q*??f??[i%?bjL?*}??z???G????'	?????l???=4???R3rwF?pJ???n??T??/p??[x???
$9n??0???K??.^??t??=????L???	??o??}?EK!J?r?%??=?5?^??gPDat??M???{?? ??????m??|?h&????UJ???V6?d????Em_w????tD?,?????4???_:??v I?'?????w$??k?m???&b?'?N?2U????U;Ht	o?D[?v????????gZ-??@N??s?????$.}?}???????C??C??:h0*27Y?N??????X?????|??[35#??_?z??f?????$?J.xzfY?	?]?>????????????g?9?4??A???(:~???>2?|E?g?)???????-?Kn?PQE???+pw4~??????_??q!~???q????v?o???????GD?zse?x??KC?W?y??????jz?????dz???X
?o??(2????{A?v???????/??s+H(i??k)?BhdaH?eq??P]????u/?j????|@A?c)z?h?h?S?W?]?????$???o??t7SO%?? -`P???N?HC?l??# ?!sC?	??L??l0,?D??l?????$??{?7?zE??????r?qjk??{??^?? ?tG?(?=v$[?i?V?>5*???U?X?`??<W??Pd???y??(?M^???!?'??? W?M?pX&???2??????#??A????Dz??0?)J?(}????-??c??E????xN?'?u?^U?4???R??????{????yO???&?I1?j[??c;/=??>$k???Y?[??:!3??TT?) 4+?? ??Q???g?w(?yU???j?.hj*V???????>????M#???.j?p?w[~/J???	O?5?4B???=?tA?k???`??'(?V?en?.????DB?(?gV?l?#???]+?aGi??^g??h?lB????	U????W??k??0????+??????v???c?	hu???!??KJ?zo??ii?Q???'mh????KB$d???G??A?o???~q???L*n8???.????a|??\?*??2?O?a+5??'?10???'??>??t??r??????|?r?VxYONIF^$!?Q?[??????^9??G?0?O???G?????$5?e"Bd?????m???n?u?????Sd%@tL?E??r?.???)??!1?M?D~<?M?%(?`<?)??[???????:}?!?9?)+?{????????q5????????u?e??!^F?kZIRO?'0q$	;?,???c27?@*1???Y?#???	3??????(K??dP? ????0???n?nY?y????vI?s?K?1??2???????,??Y?????0E1????@?g??-Zl??.??'???X??M??}3???+?d??F?d??????^xG??-????U???????6?Rqy???m?L?? ??^???????B=)??"1u???R?-?%?'?+?q0s??Q$1??_????????Y(??kv$?2?;?'>???P ?vH?t?	L??WDx????????V+?k?@m??R<?#?kd??{? ?K???tpZPr7?J?5|??|)??mTd?N???eC????*M??&????hX?%??????8??)!L~???.????%??e6H?HX?Rm'??Y?k??"???$??????0/??F?L??N??~?PL?]?-v??????????[?+q(P???|7?G?1?[tgxh?S?6???????+??E?<?S???????????Z7???9?p&>?*9????t?~?H?G?`?p???????4?i*H??_8??m?Zu?6? ?)?~??K?r?W?z?v}nOz:{));?????????:5^?X????D?4???=l???@<?aH???????P?1??`?+??/????L? ?t???-??7R??Ct????a?????A?_PBc1{?I?8C9??"TF}??qgf?????x???S?R?{Q?y??X?0?????.???!??/??+?WKe"???!?|?W?,;????1-+?;^?????S??>X??\?}????u???Q??	????????g?EI??/?;aR?????iNy?Q?@?}uWCU???e?g?*(?C?<8"??&(j?+G?????S~es?Q ?t?S?Y???5	??a?:????`?u????S????I??	E?^??h?w*??8?"???ri#??????????L???Y	??????!??C?? ??G[??c??o-?????gt?)??????????S-8???????????os>?b	h??l?o????l?????y=????????"?RU????{?@??l?$_W;NB??7[}??d2{?	??~y?????H??1?4	? 
H?????#>O??>???S?=???????j????3????v?"???????q1xi???????????mk>?T????'?Ai4??h???J^/??X j?~'??????@??P\?_gO_??6?5epH_o??1?h???s??7? ]Vv??&?;xRa??J?????u??^GO??^?w?t-w?Z?r???????3;g???????b?g?u???;?R?fsz???????????:???<??`b??'r???????0?L??j
s^? ???3??[????????@]????C?]?X??????.????"?5T`4J????????+9????`?6de??3QM?: ^????S?[?p??i?!|PP?:??????b???6?]??????A??/[G??A?+?|??? ????In<????????*?2`l.?_\[w??J\0L???/]	?+?Q.?<gK?,??d???C?K?zm?????????J???????)????`????oQ}???????x<rgh?????E]*???x???%i????=	??4.)????B?ip????}?0??X4j??%9???{S#?93
%?zm?7???^?9,??.??1y5??4]Q?d??w?/\[????$?*?????????L?a???.?;U?!???q?????D????8???S?CN?B????????0?_?!?????LOI?u???O???Y<????b8??Q????M'?tPc?^?
??I?? ???i????l???mG??1?X?ip????pAe??a???Q?/??$?]?	e?7??x??<d/|w#??:?[?Q?????S????????Rk??Q??@?P???n????]??b?????LP"??`?-[p?1?!J????o(V???R??????=?,? ??K<?p?j??k5%:?t?j?&?E??f?/??y???????= ?0?9??V?*&?	TO2c'?,??b:d??????61?}4?1?F??n???"^,(s\*7?%^?_?A??? ^AtR?<_? ?????H?m??A5F??B??w ]? j???A???p?Pg[(`?wu?L-Z?d?@?015:???,??fM?qJ???5???(???	*??????<?=?qV??i??r?Ro???8???%?z??y????Z6??Y?/_??.??P?Q??????\?G????}???s???azT:m?QE?(??????r???%?H?o?c??{p?9OL ?4??Q?FZ?O???x
?ls"?5????????P??t%???j?_??-.??6?????R?X5?ea???S??`?.&??NX5??????wt????%??????????qv??Pd!hG??r?6]+????m??n	????^?;(?s????"??O????MW>?mY???rT`v?Z??uV?~?NH????????!>7v[n%??T??????s??????:?p????Sa<M?(/>??b_%?????????@???<??_????$??uR???i?/8?KV5_?df%?1???v??Q?>?+c6?pS??S??z????I>?E??x-????{2vh???q?	?=&r?Ezj??Y`??.??d?m????????+?l??????si?pu????nsCQ#??_?O??e??nq?M?c???LUK ?AQpn??4?Z??d?b4w`????Y??=???=???Eb?H^?k_?^?	:?2?3????(??]v????c?????!???o????n?????i??f?:[?z]???s?q??????b?_`??,pIy??zS??Ae*?b? [?#M?=@?5p?J?PM0\????7?h?(ks?E?????%??}3J?r??+?y?Czk(?{??#tj???>?1?Zl?O???m?l8?4???H?F#\E??D*??z,????#k???!*^;E?????A7pY????^?E(???^7???`??R??8C?m?[w???$??????:???????	Q?ft??@1L???WZ%?gu?a?`r?KN???$?(?N?;Hi???<?]??e??????8o4???e?c??0????Gv?jF??9G?[?q????6X????E?j?B??C????==TA??_Y;??V?5?iL?Pi??_???????#E,??q}???W???????W??cs?????#O}???D??P??'?g?[?????????Z??a??@"??w?y????87??w ????K?Qb?????y???c?!??`??? .?Mk6T?q?0???&??TpZ???+B????k?=P?????m64??????)/?X????X?u)?c?]??|?b?qH??(????]?F=P???b??bD4????:#?????2b???t2p6?b+?s?	?w?*$C??:??*???r?????m?-N?<?7????_?+ma?j?y?`??%?_?h?+/??m?Z??`J??^L$?7;XS?b??n ;N3{?:a?)??u?????B??`T??E!??R^?z????k?B.R???kRQ|?lJ?$??$?&?*???,???J?8a???,}O[?g3X??w?M?S?&!\?????1?yI??e?s?y7\?G????*ShO?&G?????A3?E????SeY???L>?xI@????X???Gb Mh?k??)w?????????^?`???????yY?v??#??cu???????	??i?????o?=?n+?????}??0???J??EP?\??????n(N<????~??yI4-???????
$%?
?F??p?B~hj??N??d??)??}?gL??????"??L),?F???v~?)!??`????:?z??S??&?j`3??%??Y?\?s????v?"7??@???x??J5.??????6?-?!0???e5????????H???oL??M??p5b@???'p??O??'d???]qW&??QU??~P?0]M??$JM?_8?1??56?fC?|;?????????k
?#i??_?,?w??Qx??A???/xW??Xm6???e9??$O?O+???v?]B?o???????? ?:?K3???W`?k?&N???Ta?????"_O`d?9??Rn7???????????2?U.G{?????+?xl????E???t?B? .3??Q"?@*????F????\?????Af????rIh?O???TO?????&?????????>I?Ty??,%?/?w??o??G ?\?b??~?????b??1? ?????(??t?x)??-KE2H?[?f??????\??jB????i?????,?4?????41?=vl?4?|?????)^??]???Y?v??5qZ??n????????????MYJ??k<o???IB??e_?a.??P???:W????K???????D??`?4?}"sI??/vG?E?:l"4	?=!??BP]???kv;?8s??:?]???c??;)-?W?ux{;5?E|z??o??????N???? ???k?Jw??]x"?{???R??A?l,C??? ?????hS??o??+Y?!Wf?G?s!??m? ?8~??????????c????L?? ???jgk}JYO??^~m?_?>L?Hk?nE?yd???????3n??)??6???7l?V??Y?!OE?vZC?r????v??)??O3Y??[z6L?= ~:+'"Fc$??j??]??? Z?I??A82^?????l`d??J???J??Pb?y?`??Q?5?A??`J?-??8O#?a_G?u?????R??RoB???x??x??????s??
?@?m????j??}?`6????????A?.??xGb?? ??}.?,?"[?????l%?V?~Q?e???Z?> ????Ta?EC"????p????????????eo?NqQ?\???dN??U?7???wO??O???~?<u??????<L??iR????????bJ?2'?f??pQ??;??????yt?E]K?z??????"??DU?2"IU??v!q????I??)???p???F?D????V?6Nw??~B?6??K?????"%????E?4?9? 4?????Q???(K_hy#???????????>?B,?DD}?t????Y,????g???*!( ??????H?9>??k???m??????XB???8???G7?a	?CH???B?k????l???Oa???!??yQC??%??^???~?????K?SC????oQ??<NJ9t????<?66-~
l??t?????R?a?j?x??t??b'l.??#?????=vt?1?-?*??!rQ@??????C??/??4?Ak"'&d??	??3???*g1?#??|??)X?I??i??????/??d??<?7?m ?????[C?N??=	?O@I?|r??8?L'`?si?tK??W'z?8,%??????|M?i
???k??	???g??y??a_^I?YkvX0?5P6?????6???5<0&????R_]??????L??-?,??`????n??'O????O,~??3?T???){!?yD?eH9?/9??'4o?3?;?????????}x??????6O9d?H7-???[]????=?????????\????(?vP???4????L?n\Ku"q??n?m?|.}2???$?1??h??a??.q??kl=''?
]'W?l??zY??tr%g0????-~d???gb+?t????8?MQC?y??Xd?2J??????Q???5???l6?????o(SQ??????o,f??g?????O???C?h??]o^?2?^8??O?7????+qBq?????S?w&4??_k?H?4??Z5??:K???9
????7c?i9?^??????-VkQ?;IHx????+????jSo?N??a:???p??*yY???\z???2??S???hIS.p?3U????3?@???C*(A$c?E??>?D?????k???d?.???S??B6L?5	?#??d??$??!n?3????uk_?oq??T?*???@??????/????yv?u??????1?`b?d??????2C???]? ?]?"?2??1??^?	???~KeO?wI?L1m?N?&?Z?&?:%??|??"??E?x	l??gq????\??_	 ???????dr?w`???r?S( ??_??Z	??vF????
J?TvxOb??????QC??:U?J0?????qS??2ORRU????*g??.A??X+?VW'?xy?	a?h?y?1N????g_????y??f??????KwRs??? ???a?????`^??????? ??@??z*?+Ck8\.q???7L"Sz???Q?d???X ?[c{?OL8j???B??'q.?C?G??????2X??"lZ?]????Shn$???+?}h?h,	???-????r????PN{t??_???4??X?C
??5*?!??;AV:? ??rA?w???????g?U23?w?H+??O?@?`??7?w?1 ????$y?^??????&?e?H?"4?2?m?!????????????OB$????e?W?]	?~C?bw?X[5?2???????????p????U?v?????@?Qj????+?	??m{`r?????	?*???'???????a??dy ?k??!???{;???;?;=W+?pN5]U?k??e??h????'(??$????o$?N???????AO????^ ??Oh??K??n
:??J??{]!(
Hm??Q??????u?K???*?.?ee}??(??PC?h??l?~1gw???<?`??5!Fi?`?9W???????b?5?????????it???K?y?????>?*?c??hN?@???B??`K??????"3?8?G??????3q13Ra??????m?8??Bw???z*c??I???K???&????=<j2?8_?H??Y???"?????????E[?7?b??(V<{????l??In?????1(?I???dx??????????.h?;?iU}3?;?:??2t?A@?Bx??>???6?????n@? ?HE??S??~????M??*??????p???T?S,?r0k???|??????!L,6?Gb???9Yc?7&???Io?0??)1??/U??????2C?i?3C??hjd?*F??????O,}G'e?s??P'%,?c?eJ0?61W?N?V??????FXAN???U??o?$????uK??'?	?)????A??^x???`?e?????n?\?y
?`I	h*?P&??+?????_?~?????}??????=?G?d?8??e(???"V?N?v????????,?F??Tp6???a??q???Q?X?????????????EG????????`~?o????bS???3????c??q?%?P?{???y?;GO?E??????????o]po|~?C#???&??k?Z??P8?a!???(p?????$bv???d???V??[d?c$?B??e??*?K?????????"???I?q?(D@?7?qV`??
??0t??|	??8?z~???C??[???._10?`??!??????[d??:?/o
??,7??Ep??B?vl?/??W#??p???mNp\?k??V??????hEg?'?w?*??%?N?	?I?9d?Z?????L??????????Rq?lo???6{?Ka?p}??X???{5????#`???o????zP???V_??
Wu??????????gQ?????9???f???q?y?{?/?u+"???? ????c8?S?	kq??M6????P:?q??9:0d????%i4U???:??a???n???s???,?.H$6M??'m?&????????[????+??:?f?}&E???????h????ON??}J???:Dg?/?? LA?=?{??)???w????ph??y?!D? ??$^?uVO !<Nw?5???20??S?????7???C?!?d?j?<#d?[SO??ZO?Z?l???;"?{???Qd?Bz%?y??P????|??n?q??'???D&	sM?{C?j?????[F??.&?????Y?+?Q??9?,w??Y??Y@*C??[????w???QV/????v?[(??ht-?m
>?y?x?!?~?r9?b??0;E?&?O('?oo??W???D?c?<?lw?i8??@??MI?,?????I?????0?w
?I	:????9??!-?1???hO???????~i??=b-V?OdWKg,???O???"{dP???1D???Q_1@?^,??JhIJ-u?
%???X?\??????+k??6?<7??I&s ZS??=vYmu????D s??u*????
?7??2??|s??h!??U??u]?%???^?D?G?????a1???L^???_???\??S{p@Hk:xo??|?+?;??x????????#?????0??s@d??????a???x?[???? ??dM????ny-<Cd&??M<}????mXq3W????^j?g????~????z?0???1???@G|<?L??V?c49/D"??7?(?]?D????9H??Z?R???5????	?%^????L??LJ?fC??t)?.?)??A?=??iw????!J9rr9X????73??qy??????K!?}??|\6??kY??`?j?0|ca??q??J2?v?P?4??(????F?????j4b??-?r??;??? o"??????q????t??1d?c?????z??????z`[$?????(?fAU???????]???8k?"20??[?@ ??`???ox??M??n??	8]V????x?	?zR??o?c?^????5y??????? ?'????m??Sx?'<?\_#?IR???P??Z????[?0??#??;??6?J?~?sp???&l???@"g?7?+j?LE:?+??dt-??????b'?0?YC?4?A?F??6D??e5Ih&????t<?T???9?6?T??}?C?G??ud?k??????[*?fQ???^?ER??e{?=o?z???+{gN"^??]?n???a????s??x?????H.?C??.??6?"????!?OF??????[?sH?k1?4?'[n??c??[????iK	??r???,q;??K??]?%??V^