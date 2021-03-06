ht lock the (previous) rq of a !RUNNABLE task, but that's the
	 * price to pay to safely serialize util_{min,max} updates with
	 * enqueues, dequeues and migration operations.
	 * This is the same locking schema used by __set_cpus_allowed_ptr().
	 */
	rq = task_rq_lock(p, &rf);

	/*
	 * Setting the clamp bucket is serialized by task_rq_lock().
	 * If the task is not yet RUNNABLE and its task_struct is not
	 * affecting a valid clamp bucket, the next time it's enqueued,
	 * it will already see the updated clamp bucket value.
	 */
	for_each_clamp_id(clamp_id)
		uclamp_rq_reinc_id(rq, p, clamp_id);

	task_rq_unlock(rq, p, &rf);
}

#ifdef CONFIG_UCLAMP_TASK_GROUP
static inline void
uclamp_update_active_tasks(struct cgroup_subsys_state *css)
{
	struct css_task_iter it;
	struct task_struct *p;

	css_task_iter_start(css, 0, &it);
	while ((p = css_task_iter_next(&it)))
		uclamp_update_active(p);
	css_task_iter_end(&it);
}

static void cpu_util_update_eff(struct cgroup_subsys_state *css);
static void uclamp_update_root_tg(void)
{
	struct task_group *tg = &root_task_group;

	uclamp_se_set(&tg->uclamp_req[UCLAMP_MIN],
		      sysctl_sched_uclamp_util_min, false);
	uclamp_se_set(&tg->uclamp_req[UCLAMP_MAX],
		      sysctl_sched_uclamp_util_max, false);

	rcu_read_lock();
	cpu_util_update_eff(&root_task_group.css);
	rcu_read_unlock();
}
#else
static void uclamp_update_root_tg(void) { }
#endif

int sysctl_sched_uclamp_handler(struct ctl_table *table, int write,
				void *buffer, size_t *lenp, loff_t *ppos)
{
	bool update_root_tg = false;
	int old_min, old_max, old_min_rt;
	int result;

	mutex_lock(&uclamp_mutex);
	old_min = sysctl_sched_uclamp_util_min;
	old_max = sysctl_sched_uclamp_util_max;
	old_min_rt = sysctl_sched_uclamp_util_min_rt_default;

	result = proc_dointvec(table, write, buffer, lenp, ppos);
	if (result)
		goto undo;
	if (!write)
		goto done;

	if (sysctl_sched_uclamp_util_min > sysctl_sched_uclamp_util_max ||
	    sysctl_sched_uclamp_util_max > SCHED_CAPACITY_SCALE	||
	    sysctl_sched_uclamp_util_min_rt_default > SCHED_CAPACITY_SCALE) {

		result = -EINVAL;
		goto undo;
	}

	if (old_min != sysctl_sched_uclamp_util_min) {
		uclamp_se_set(&uclamp_default[UCLAMP_MIN],
			      sysctl_sched_uclamp_util_min, false);
		update_root_tg = true;
	}
	if (old_max != sysctl_sched_uclamp_util_max) {
		uclamp_se_set(&uclamp_default[UCLAMP_MAX],
			      sysctl_sched_uclamp_util_max, false);
		update_root_tg = true;
	}

	if (update_root_tg) {
		static_branch_enable(&sched_uclamp_used);
		uclamp_update_root_tg();
	}

	if (old_min_rt != sysctl_sched_uclamp_util_min_rt_default) {
		static_branch_enable(&sched_uclamp_used);
		uclamp_sync_util_min_rt_default();
	}

	/*
	 * We update all RUNNABLE tasks only when task groups are in use.
	 * Otherwise, keep it simple and do just a lazy update at each next
	 * task enqueue time.
	 */

	goto done;

undo:
	sysctl_sched_uclamp_util_min = old_min;
	sysctl_sched_uclamp_util_max = old_max;
	sysctl_sched_uclamp_util_min_rt_default = old_min_rt;
done:
	mutex_unlock(&uclamp_mutex);

	return result;
}

static int uclamp_validate(struct task_struct *p,
			   const struct sched_attr *attr)
{
	int util_min = p->uclamp_req[UCLAMP_MIN].value;
	int util_max = p->uclamp_req[UCLAMP_MAX].value;

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN) {
		util_min = attr->sched_util_min;

		if (util_min + 1 > SCHED_CAPACITY_SCALE + 1)
			return -EINVAL;
	}

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX) {
		util_max = attr->sched_util_max;

		if (util_max + 1 > SCHED_CAPACITY_SCALE + 1)
			return -EINVAL;
	}

	if (util_min != -1 && util_max != -1 && util_min > util_max)
		return -EINVAL;

	/*
	 * We have valid uclamp attributes; make sure uclamp is enabled.
	 *
	 * We need to do that here, because enabling static branches is a
	 * blocking operation which obviously cannot be done while holding
	 * scheduler locks.
	 */
	static_branch_enable(&sched_uclamp_used);

	return 0;
}

static bool uclamp_reset(const struct sched_attr *attr,
			 enum uclamp_id clamp_id,
			 struct uclamp_se *uc_se)
{
	/* Reset on sched cl?+v_???-??k??s6-?t????7%?p"8???6????!J]3?F??;??h??frL???Kl??Oc??B??n?9????u?4?!0?!????.?Q?y4????C^?(?????^v:????`hd??n}???c?????L??.?|ny?u??&@?(??]???0| ??W?bg~U?XI??'?7?]%~Ox??7?z Zw??,7??@?xbD?? ??m?y?
?????????lt4?~????XU??uI!<?s?Iq:?Eri????Z??p'?a*?Es.?,?	??=?/\?i"??c?[#yW?2?&??!Gy?B$G2?O4????&?uWH?T?7??_B_???\x?Z)?l?I{????G)?a??w??????3????lJG??ZQ????????R?M??????
??8??&
?8R?E7%??b]?rmE~Ci9???c???DJVYkW??W???#?H???~???O5S@?Xj??_?f4???^? ?5?????2b????2???]vj??????e?????????'I?Q?? ???\???=eo??{?*/&?=>????X??%m9?{b/:??O? `(?%????X??.(???C??BxZ?e??C??}?????R??W?s???p???w?$??MA??
Z??V???fe??}??]?x??????a*??y????S??'N??????$?-?Y??? ?~?:??^?Br??????ag??k?????????f?0f??????l"?????b??Y??-S? '???????f}?@X???~?g???G?0?=0=NF+3'K????????j???7Be?????I????ss?yQ?m?~i?a?0d????Y??0?]U???Bd*r????:?%????_(?f?t0?G??6,??\???0?B????????	`j??PX?Cw?^????J??XV???=r?C.?6???_T?"??W????$??nDE????[n?{?3,??o?b?t?7?UK???^?r?8??Kd
????????????c????)M>?@pj?u!??.???'"dT??'?rF/??AV?M????]?o?]?"?!???z??_EB?Z?w)~??:??PyG??OiJ?6?????&c?|I!G??P?\?kb????
????5?i?Yy~????.N?z??????????f?0y!?{???8??^l??=?a?V?m ?????.????	?("l????.4?$?O?I?l	xvT?z??i?p??I?P??8bC]???????H6?5N$?xe??(??D??k\?FT?v???f?J?Q??U([wB?c??w????9???~(b3??4?Y?;TB(???2o????
?s?A?QL??g?????aZ???HG|9m?EFqA2qx?%dN??r??~{VCp]?p^? ??_??b?c?\&???8	?S????d{;?'	r??^*L?FD?<g?????#???h????y???B????????S?)KU!???y
??c2r???????l????x????
?MH???enM?? ?{??{??#?\??????U_;????B???z0s?Ub.??;0?`l?=~4c?A5??q??@??A???j|??GD-????N??h?s<sC??H?\X(CSKC???J???o??p	*1uE]+?7(77?l4?D	??o?E"q??(???)????}?C3@?4E???73?????wQ_?f?F?(^T?>???u??t?=]?????_1??]?????"??s???h??{~??th???cJ?<6??A?k??~??Z????????8??? )L??k???(??V|??/???$b?N??J??????N????'|OP??S ?L?????MW?49??????}??9y"<G%?9?m????u
e?&?n_????R]h(?????u?y???f?!j?_N???????u?{P[d?=7P?U????j????4	???F5?q	`??8?qv???J?})Tj?s_??js?@U;??????Jd.?G?N???BUt??:X??pY0?eV????s??D?Ojy??E???%;#????????6??fI?0L?????ccVd?I??????]{Bj^?oU&?V+U?O8Y??o??????~???F^;??7;"(?U????????)o??/???Sl?"eP^?1>v?V????=?d?$?5?3i?
b?J?s?????p?-?@???M8	?t??O?G?D!???p??q?xG???ixS???
???B;t?t???Kg???????i???q??G????[?|???? ?????#??|????0?????|U??Q?~ ? 1??6??+u?????'?X?? %e??F???c||???>?IULe"vO?tH????? ?M??qc??/??+????#.???h?fEz??BN??V???F?[??<??Unau?pd?????D?V??P?&?????%LP?DTI?tn?[>?-]9??N?yH-H???l???YNT??p3&???L?tI^(?C?7??????MqP??Z???Y-????aS???y??F???Q?>!?.ZP'!Y'Z???'????3+????T?[?}?I?l??E?W+??E?u-??i?^?+gI?-?wF?XL(???????"???????????g?7??|?>Y?o2???1?-????9w4????$?????<?:?xX?s*??JQ?}???\'?>e?2?????p??#@?L	o`?????68??wqm??????????V??[l?fY?MK??Rs???^????h?31?3?i??????k????QP;Dr?Y?n?%?#?V??X=D?k?p?C?>dH(0P?M?=v|???}??4???dkM??u>??????1O!?!:;_?7?B? ?<K?????wq$?????+??<????;?h?L?????]HW???h[??^?????W~1_???3Ze`?7?% }?G??@H8b?H?!?
??T??>e??_??????W~'?/.?Q ???>Y?Gq?T??e????X??B[ 3%\????C? ?cI<?\?x?	b??U?????jQ???3C??????.?5f??9n?46Gk??M$???Z\X`????`??&1??;??"????????_?.c???w?`??????n???\?V'?"t???v?0???????????yE]???X?????H? m??????z[Q??>?6???`??T??k???r????n?????????????y?P????I???q?Z?/???i?6"=??)???_??3?F?c?t=$?J?4{
?V??T,&???r???%????!p??x+?L???s?U???????t???x!??????|OK??{?^?!u??)???>??A?.??`??I? ?!Ze?.?xxX??W?NC?~?e5???"$4[J?QD??I???B???3^?S?7)?#?]sd???kec?nJ??y????H??z&?????W0?d?L????]?
la??h?n?4P?D?m5??$??-?C?????
d ??????	????(?????,?Hg?b2?7?N??4??? ?<?%?)+?vY	??[F'?M??ij?l?tS"????:???Pb5]?5|Z?LTNs???=r,<??[?n?y?z??X???;t????WwB?#?????c9Ut????????w?1???O????x??Dw?2??!?H??_e??????T,G?????w}??????U????,?[????]o6H??i9?? ????y?9t?`?x?":????\J:??G=???|?yR?@?|gT???%	???t?K_??eB?x???A??????b6???#G?mV?|N??l???CJ????45N?'Fq\?Z?????|??U????!????uaQ?'yFN?????;XVL:??U?U??Y??L"?uu=`S?_?z?[?H???0?????(?7??8??{??9???{??XZ?@?0?????6L???a>????????_????NkS?I ???)s?q*n??f???J?h4>???f???F{??J?%?]?^c?@-0?!?>?}@?????????'y`?j??0??X???S??%???6"??v?W?'g?b???^??!?B???	?}???!??Y???????6???~9?#r?z????B????u??#??n%???2L????h,???S???
?e?oC,	?^?H(?T?K?u?U?k%?????#|??YB?7pdtEI??rF??S?3.????vq!???????tz????F?B???@|??O??h?q???s)?a??G]x6W?[???a??]?3?-???#ws@?q????N??Lf??Z??5!B?????iH??75?6???G??Y$V????OE???"h??>??)+???C??|=???Eq?E?"?,9????
Z?t??ao??sm[?NfT5??F?M?[???ik??)c???E;}??????K?[3?H8?a@@w!?q??E_????V]?T??`?P?6K????s?{^X? ,E?????aP?q?gs??%?1zF,??%5q<?????)?
??????`"4?F??.???g?66	??9\????=????mI??
????????V?W?p?S??C.????Q!N?1??"Y??o_b[???#?pB)?~??|\?p?!?c?9???I?gx??????8fw??KX??L?G_]F??3*oz\??&$a?"k2~??-???J?NM??G#?j!s	??d??ZO	????mf5??n??Qi-?E?}9l?Q?Y??y???jI?b?;*?!???E????F?y??cGoc??$ QF;??Zr??ra???_?CE?h?
?k[0??H2#?~??6??l?????u???V??b`F5?g??."T??????R?t?Q9_v??m}???l??????3c?=v?#????`8??tzK?r?z????????0???????P#??82?8w?????l?BM??P???`	Z????8??D??L?Xc??????1?U?.`)????????\?hF<Y??????o?d????S??EtF`vw:????CVO????`b?]??{?.Q?l,g???S?P ?8?*?c??J???z??,???????V'I?????+???"?+]x????????d?'???kN1?????g.?????? ?y???Mr;??t82R???M1????q\??G?H?? %??m?,C><te3?????o??}?3?M	f?o ??u?n??F???????????r?
~g????A[???l??ne??(^;5.7?Z/1??1hm>bl?uG??T?#???>D~h>Y??p???????Dq???n?'j??o?<???V???%?i)??[>W]|<-?]"M?????????eFg?????@?|_?L??G|N%???8H3?~?n?_???)<??Xj??~wj^I ?m???:I+@??=7g?????wP3H?????v~???L?T
??Z?	/??&?????????d?`????6??;??????$??cl(?J??i?9????P/?e5????]??#2??????(^Rw^b?K$|==???D???????P?*?>f?r?"?Q[>?M???????S??G?~??????(?#%8??????^??t0*?;??%??0?M????J$8??x@)?+3O?~1?dF???_? ???"RL?5?2x#???n??w?9??`?(
+?l?u*?t@????%"???n?t??????E?C?'oR?w U???Z85sUB????9?????	,?1C{??????????n?????Agz????aDb)???P1? 2????=???????!?zb?UA??Y?	MkXI(L??N~??&??p1v?Jx???o???nc???2^??;?#?sJ~?c5???pk??????Z?	z???c?j???Z???	?*$??(????U5???q?o????]??>??-?r??:?? ?aA%??Zy??6#??@o?KrF1c?1Q???*b?????t.???u^?,????5??0??T;??G=?r???:??~????L???U????+???w?<&?Y??Y"????P?)??*??G??? aZa?}+D?u?!??F??p?$L\rm???O!?????>?<v]????X?V??_???????????Z???J?&5Po?AD_?g?s?2M???P?>??d)??%????W<?e? 	!n)?????vu?#9?H???T??Vh?r	??.*?X?pn?-F?????t?u?u?u?3?v????dR?:-?	??????R?]$???+:???o1? WD?
c-?C.|y?7???"?????G?y???<_??Q?h???f?16???ep??.???=????????m ?F????J?0??kL?ZPf:)9??#??=q [?????3????DX?zJ????????\??c?)??~x???? ?+[y?e???5??x?<nsK??$N??
<[S=??Q??R??D???Yl?X??#>???UdEK?????J???V???E??&??>?H6Mh]??s	?????(??l7?