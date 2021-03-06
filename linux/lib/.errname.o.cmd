ueue:

	/*
	 * Run the remote tick once per second (1Hz). This arbitrary
	 * frequency is large enough to avoid overload but short enough
	 * to keep scheduler internal stats reasonably up to date.  But
	 * first update state to reflect hotplug activity if required.
	 */
	os = atomic_fetch_add_unless(&twork->state, -1, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_OFFLINE);
	if (os == TICK_SCHED_REMOTE_RUNNING)
		queue_delayed_work(system_unbound_wq, dwork, HZ);
}

static void sched_tick_start(int cpu)
{
	int os;
	struct tick_work *twork;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_RUNNING);
	if (os == TICK_SCHED_REMOTE_OFFLINE) {
		twork->cpu = cpu;
		INIT_DELAYED_WORK(&twork->work, sched_tick_remote);
		queue_delayed_work(system_unbound_wq, &twork->work, HZ);
	}
}

#ifdef CONFIG_HOTPLUG_CPU
static void sched_tick_stop(int cpu)
{
	struct tick_work *twork;
	int os;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	/* There cannot be competing actions, but don't rely on stop-machine. */
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_OFFLINING);
	WARN_ON_ONCE(os != TICK_SCHED_REMOTE_RUNNING);
	/* Don't cancel, as this would mess up the state machine. */
}
#endif /* CONFIG_HOTPLUG_CPU */

int __init sched_tick_offload_init(void)
{
	tick_work_cpu = alloc_percpu(struct tick_work);
	BUG_ON(!tick_work_cpu);
	return 0;
}

#else /* !CONFIG_NO_HZ_FULL */
static inline void sched_tick_start(int cpu) { }
static inline void sched_tick_stop(int cpu) { }
#endif

#if defined(CONFIG_PREEMPTION) && (defined(CONFIG_DEBUG_PREEMPT) || \
				defined(CONFIG_TRACE_PREEMPT_TOGGLE))
/*
 * If the value passed in is equal to the current preempt count
 * then we just disabled preemption. Start timing the latency.
 */
static inline void preempt_latency_start(int val)
{
	if (preempt_count() == val) {
		unsigned long ip = get_lock_parent_ip();
#ifdef CONFIG_DEBUG_PREEMPT
		current->preempt_disable_ip = ip;
#endif
		trace_preempt_off(CALLER_ADDR0, ip);
	}
}

void preempt_count_add(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON((preempt_count() < 0)))
		return;
#endif
	__preempt_count_add(val);
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Spinlock count overflowing soon?
	 */
	DEBUG_LOCKS_WARN_ON((preempt_count() & PREEMPT_MASK) >=
				PREEMPT_MASK - 10);
#endif
	preempt_latency_start(val);
}
EXPORT_SYMBOL(preempt_count_add);
NOKPROBE_SYMBOL(preempt_count_add);

/*
 * If the value passed in equals to the current preempt count
 * then we just enabled preemption. Stop timing the latency.
 */
static inline void preempt_latency_stop(int val)
{
	if (preempt_count() == val)
		trace_preempt_on(CALLER_ADDR0, get_lock_parent_ip());
}

void preempt_count_sub(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON(val > preempt_count()))
		return;
	/*
	 * Is the spinlock portion underflowing?
	 */
	if (DEBUG_LOCKS_WARN_ON((val < PREEMPT_MASK) &&
			!(preempt_count() & PREEMPT_MASK)))
		return;
#endif

	preempt_latency_stop(val);
	__preempt_count_sub(val);
}
EXPORT_SYMBOL(preempt_count_sub);
NOKPROBE_SYMBOL(preempt_count_sub);

#else
static inline void preempt_latency_start(int val) { }
static inline void preempt_latency_stop(int val) { }
#endif

static inline unsigned long get_preempt_disable_ip(struct task_struct *p)
{
#ifdef CONFIG_DEBUG_PREEMPT
	return p->preempt_disable_ip;
#else
	return 0;
#endif
}

/*
 * Print scheduling while atomic bug:
 */
static noinline void __schedule_bug(struct task_struct *prev)
{
	/* Save this before calling printk(), since that will clobber it */
	unsigned long preempt_disable_ip = get_preempt_disable_ip(current);

	if (oops_in_progress)
		return;

	printk(KERN_ERR "BUG: scheduling while atomic: %s/%d/0x%08x\n",
		prev->comm, prev->pid, preempt_count());

	debug_show_held_locks(prev);
	pri????mo(:????M??I?????r?@?X[?qN?Z??:???B???B????/KF^? ?????8^?5]??#?K?F`??Cy~!;+???X??,???4????????6r;3??/Uh???%? 0???40t?)???A?>?J-?=??DT?xz?A?4!d:?(*?!1H???h????uVA???F?V???7	?Z?j??M??u?q(G?????@?????????!{??Q&!`?B????6????E??Rf???#????@0??v9?~??????-?>?/S??[|,3r???O#???eM?????@	??????'?c24W??y?0????8C?
?X?84?????9?4??W???%L?SfY???2?L??????v???b? aU}???x??Bf?????H?????K< ????K}?O??n??NN?B?A?rt??w???y?T???uU?_]S??hq??]??%?C?O???O???XS2J?A?I??BU????w<?1??s?????)????r_?m??'?`(???n{??(z??z???@?#?????q?/y??&*???e?[?N??Q????0qO?a?UC[8?0??~??J????H??r?rgk4cRHfz.=w)?????iFV??1B}i<???W??Z>????Re??v?WW?e????#A?2?%r???Y??2????*\h?n??????"?	???????b?????po?#z??????c?????B?e??????	??k??"???~Bkbi0?R-T?<5z<.?3?_|???[?BTa8,?
d#?????e5??[d,???N+???o|???"??q?X?c0??J?4<0?????????[?S??C?3???J?sW?????m??N)gc7?i???<5??U?V[?:??D?Y???q??X%????[?2???)????????{?'e??&??z;???QWO????????#?u?????W=?b???y?kb{?_l?{??T?38&?Q?!?*?aw??)*F2|]I;$?:6?????+@?z?g]?8???KcE???z?5?????dTVx? ??@1??#?3??????%?(7???@??<(f	?M??h?R*?&??*4??6????l?7?,??~=??????w"??n??|%??,?&?|????e??g~$5?O$???Q??i?>?E>C?
K? ? ??;l?^3??=????:2?a?eky?*???? ???r?????tl??M??Q?r?E???~??????? ??3}?h?h?L??C#?
R?P?srd??`!e.,*(:? ??dV(,?? ?dg??????:?>??F^E???A+!?/x??7t[g?????#?k????0~&&v;???z?y/????"?)?G????L#5?M^?&????L?????E?nw8????<?uoqA??L&?????G?E?????sZ???IgS????X??)????I???????'W?>?lP????&?9i?j.WD? R??*???????]X?1???T-???D?q<?|ST?????;???2R?1?;.pa-?.?rh?t"&?k????/zo?Y?Uy?J?J??w??s#O???q?a'??H??8?iy???~??w:_?2??i~0U?a'? QZ????k?\?????I?q????Ir^gg?)??????
?: M6???1?zD??7-???Z???U???b?;????{}??Q?*????d?
r?W??a?m???tYMu?T????`t?K???u??????R[F???o???;s????V????}5j~
o???)?????:?\?E*J???L7I@???z???8e??-^???????JI??D@?E??_?TwU??s???????????,???#??m?Dn??)Pb??Mj?8b???
?
?q?[?.?wX]??n?2+????[???'??C?kR?????;???Rb?.	????)u????P^??0?xG??r????m???.=h"???|?[?D?=s??\?\U?4l?;???l?????v? ????)?Rw??D???qf^???I?????????7K^Y:?C?,D`?d?R?J`??}dG????????y????????????sr????v%???????8?*?lT?0?z?\?*az?8?{?g??_??a???+M????/?Kn?3;???/X?Z???T?'u?/?????5?f??~????x??61???h????`?\??1*?M???|1?Q??r?Iz?Y??????????5????[O?t?e`v????b?X???1,t??8??r`lY?? ???F_?(ley??$u6%'??!B??v?a?S`????Z??68?????k??????g???TSV??.?$5????????	?T???(?S	yrhp???$H???;(?,???i$[???NH?\?? x?l??e?3??????.rK?b?????y???n??? ???????Q???@????QL?S??_?ai????-?&?-?r?F?i	?Z?s<9????<"?3????3Eb"???*d?A????!?]h?????)??x???E???????????{N>('?V?1????J??GJf75?ue??1?? ?Q???O[????d1??,????@rve????Rh???(Y????????[????$C??Y????M????$??@L?m?l???}??W?Bw?m?|?b??BF?,V?{?2B#(?"-i???_?W ?????"???S?????6?????????Hf?^Y (?s%VD???2?q??y?1?h?????I?&???=,t???bXQ?h?[?r?y/???KwvEN??Px@{?????L??<??????B/?V????MF?!C??.{?~?t???XR????^i?>???	???]#??????y ?9??:Kr???E???JTN? Fq??_q y?????]??J???.??~?@,?f?96?6~D?ug?Z??????yb/e???$???vh???????d??+???????T???????W/??^Y3?Gn??%[fA-?o?2|S?t5?#?/????~6g???s???j?h?????e%??I?V1?@???6;????????+kQY#??I"??>`k?B9_J??b[?3#E?|???p???G?wh??g???+?R??r?j??H%?+?v??????6???"xshs???B?????U?%g?f?`??U????p????o?<>???C?????&??"%?l?$bxm_!?1?A?q&???????sT??#??qN]???g??74B?h??Q?????d??c???:n?X:h?H5??O30Xvt??B?^m?)a?^}?5wz6??}????8]vU???Z=$m??
?Y???B????0???/??u?????????OB7???_'?p-ga?????w??T-88?h??i???9?wDB??[???*?^#?S?g??????R?	$??H?S???\?????A?$????8?M??y???n?P?&,?/X(Q~%CB???B?<???????T7?l???L?2`?P?>??]???????K???$????????????Ki?z???????|?U9???raUc,???.\B?&?S?%?"Y??X	????I ??dV?8?]?ab?~p>??i??!?B????%???6?\??6?~????8??%?
w???V?m/???)uKt??S]X=?%?%?(?j?????P????$?M!	IEH?i???D???,
?X??6^??"+>??&"???* ]?#)z?????l*??M?!#D?ao0??!????h??d?<?{>??^M??OfV?s4??nm?8??:??)?S.O`2Q??6???Bu????6?q? ?o@?gM???q!b@q?9&???/?M??e??9X?`??[???N?/????1L?U?G=/??~?1 ??N=?sd??&??f?b?	??j?'???j???X?= z;?k?9#?W4??
??????8	a?`??,??~?L???z&?U?f???tL????V???>N?.|?
???,?-??Y????w???w????A?v?E?f??????w?????+?????u?<.??:?????y????SO??\o????g???`E??h?@????-?d}J????1? ???-?Tm5?0"??>??P??????b?U????]????]W?O?/?`j,*??9?G?B?w?%~(?2?????(?/?b?h?F?????%?5B???;L???o????[?u??	g??x?#???I?a ????06_??????
??^??p?){??q,d?ED?=?6?5??????D?Y?1??U,$???K??#???????7??or???I??(???/????J?M??	[	?0????2PM??6Q?? ????=?h????J????\??D7???w????f?p??Q?:?h?8?Cz??+???@????x??j@|?m????xlCY?-cp-???l???=???6??rM?KK?*????	???v"???;?????????g?A?t?????v?Q?=????? ??a????z?Y?.dG{e>IcrO?%?4?2???5?s?%hx??n?!?m????????-Y???q???	?em???h???i?w?]?????#%?????8?P ?JD?xF,>?;???? ?q????4??J???[?|i???C??+u???D?*U??W|.?<???S???????{???????z??-????H]???4??d????P?pw??????V/??B???IV?# ??=j??8$Fv?y?;%??????xY,?^?????|??j?B'Ov?????j? ??%???{??J?i?J??????AW??O????????Sp+?"?;?9M?8???*M?Z???bo????L???,n?$?2????(yU:?u#2?*K?h:?zTVG????P|gg>?)??Wi>??S8UN?8?????)??<?f$??Y?,?@G??vo.?O??N?o????[??x??k<R`???
??t?|??2?={??+??'??FZ??`??uz7???`???y??{??E!4d???l??? Db?.*C?????'?{v??t???:?%K?m?,D?/v?>??b???? ??V??~???????>?e??v?w?.q?]?????!-????U ~
?+(?2p??\Y??U???y????[????[????Gs2??-e??_??????j???0U?????????,????=>?????m/?=???n?v? F??^Bsn?????%I????.vf*P1????LO??????a??A?>??CAO!?D?[????+??C?&?<7?@?Z???K?7??2????BmSD?Ctx?,?C????
??oZ?????i?X?<2???d?{??E??????]????%??u????antp?M
Jc?k&?A?Q_????D?O?G??)?d????g???,??'m?O????????\??y8Q??5???SD?<?p(???????F?{?~??{H/FL?O?Y???<????u{??^?u???s????4w???fE_?f?????VC?/r?_??W??H?Mv"?a??$?aC%?)???"~????Y?????7)?n;$M???m?*A?p?X???/Hy????N2<??r?1??s????^<E?>????3?s?v?3???????Q???	g-Km*.??Lh?Iif#? :????c??!??????S????M?lg?e?E?????+?-?=A?iW
???iI????&??*#kQm?`?[?'??S????:??=??????i?G??????2?Z?uK-??l??Pj#]G}??2?Y}D)D?'????Aq????C?F??!uij
l?x}??6;./??G??zlSU????b?@???1v?C/??Hq?_n???????45S3+???P?#?m??(s?Hhs??UU?nD?C?????B?f??mx??+?m 3???J??(????\Q?p??v???9A?c????u??C6[?]??b?s??_=???????[+????u???su??R?x?Z>????? 8??3??J? +???0?Un??l?<?X??Fo?cA?\?w??