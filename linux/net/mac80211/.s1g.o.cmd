d domains by considering the
		 * cpuset configurations.
		 */
		cpuset_force_rebuild();
	}
	cpuset_update_active_cpus();
}

static int cpuset_cpu_inactive(unsigned int cpu)
{
	if (!cpuhp_tasks_frozen) {
		int ret = dl_cpu_busy(cpu, NULL);

		if (ret)
			return ret;
		cpuset_update_active_cpus();
	} else {
		num_cpus_frozen++;
		partition_sched_domains(1, NULL, NULL);
	}
	return 0;
}

int sched_cpu_activate(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	/*
	 * Clear the balance_push callback and prepare to schedule
	 * regular tasks.
	 */
	balance_push_set(cpu, false);

#ifdef CONFIG_SCHED_SMT
	/*
	 * When going up, increment the number of cores with SMT present.
	 */
	if (cpumask_weight(cpu_smt_mask(cpu)) == 2)
		static_branch_inc_cpuslocked(&sched_smt_present);
#endif
	set_cpu_active(cpu, true);

	if (sched_smp_initialized) {
		sched_update_numa(cpu, true);
		sched_domains_numa_masks_set(cpu);
		cpuset_cpu_active();
	}

	/*
	 * Put the rq online, if not already. This happens:
	 *
	 * 1) In the early boot process, because we build the real domains
	 *    after all CPUs have been brought up.
	 *
	 * 2) At runtime, if cpuset_cpu_active() fails to rebuild the
	 *    domains.
	 */
	rq_lock_irqsave(rq, &rf);
	if (rq->rd) {
		BUG_ON(!cpumask_test_cpu(cpu, rq->rd->span));
		set_rq_online(rq);
	}
	rq_unlock_irqrestore(rq, &rf);

	return 0;
}

int sched_cpu_deactivate(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;
	int ret;

	/*
	 * Remove CPU from nohz.idle_cpus_mask to prevent participating in
	 * load balancing when not active
	 */
	nohz_balance_exit_idle(rq);

	set_cpu_active(cpu, false);

	/*
	 * From this point forward, this CPU will refuse to run any task that
	 * is not: migrate_disable() or KTHREAD_IS_PER_CPU, and will actively
	 * push those tasks away until this gets cleared, see
	 * sched_cpu_dying().
	 */
	balance_push_set(cpu, true);

	/*
	 * We've cleared cpu_active_mask / set balance_push, wait for all
	 * preempt-disabled and RCU users of this state to go away such that
	 * all new such users will observe it.
	 *
	 * Specifically, we rely on ttwu to no longer target this CPU, see
	 * ttwu_queue_cond() and is_cpu_allowed().
	 *
	 * Do sync before park smpboot threads to take care the rcu boost case.
	 */
	synchronize_rcu();

	rq_lock_irqsave(rq, &rf);
	if (rq->rd) {
		update_rq_clock(rq);
		BUG_ON(!cpumask_test_cpu(cpu, rq->rd->span));
		set_rq_offline(rq);
	}
	rq_unlock_irqrestore(rq, &rf);

#ifdef CONFIG_SCHED_SMT
	/*
	 * When going down, decrement the number of cores with SMT present.
	 */
	if (cpumask_weight(cpu_smt_mask(cpu)) == 2)
		static_branch_dec_cpuslocked(&sched_smt_present);

	sched_core_cpu_deactivate(cpu);
#endif

	if (!sched_smp_initialized)
		return 0;

	sched_update_numa(cpu, false);
	ret = cpuset_cpu_inactive(cpu);
	if (ret) {
		balance_push_set(cpu, false);
		set_cpu_active(cpu, true);
		sched_update_numa(cpu, true);
		return ret;
	}
	sched_domains_numa_masks_clear(cpu);
	return 0;
}

static void sched_rq_cpu_starting(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	rq->calc_load_update = calc_load_update;
	update_max_interval();
}

int sched_cpu_starting(unsigned int cpu)
{
	sched_core_cpu_starting(cpu);
	sched_rq_cpu_starting(cpu);
	sched_tick_start(cpu);
	return 0;
}

#ifdef CONFIG_HOTPLUG_CPU

/*
 * Invoked immediately before the stopper thread is invoked to bring the
 * CPU down completely. At this point all per CPU kthreads except the
 * hotplug thread (current) and the stopper thread (inactive) have been
 * either parked or have been unbound from the outgoing CPU. Ensure that
 * any of those which might be on the way out are gone.
 *
 * If after this point a bound task is being woken on this CPU then the
 * responsible hotplug callback has failed to do it's job.
 * sched_cpu_dying() will catch it with the appropriate fireworks.
 */
int sched_cpu_wait_empty(unsigned int cpu)
{
	balance_hotplug_wait();
	return 0;
}

/*
 * Since this CPU is going 'away' for a while, fold any nr_active delta we
 * might have. Called from the CPU stopper task after ensuring that the
 * stopper is the last running task on the CPU, so nr_active count is
 * stable. We need to take the teardown thread which is calling this into
 * account, so we hand in adjust = 1 to the load calculation.
 *
 * Also see the comment "Global load-average calculations".
 */
static void calc_load_migrate(struct rq *rq)
{
	long delta = calc_load_fold_active(rq, 1);

	if (delta)
		atomic_long_add(delta, &calc_load_tasks);
}

static void dump_rq_tasks(struct rq *rq, const char *loglvl)
{
	struct task_struct *g, *p;
	int cpu = cpu_of(rq);

	lockdep_assert_rq_held(rq);

	printk("%sCPU%d enqueued tasks (%u total):\n", loglvl, cpu, rq->nr_running);
	for_each_process_thread(g, p) {
		if (task_cpu(p) != cpu)
			continue;

		if (!task_on_rq_queued(p))
			continue;

		printk("%s\tpid: %d, name: %s\n", loglvl, p->pid, p->comm);
	}
}

int sched_cpu_dying(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	/* Handle pending wakeups and then migrate everything off */
	sched_tick_stop(cpu);

	rq_lock_irqsave(rq, &rf);
	if (rq->nr_running != 1 || rq_has_pinned_tasks(rq)) {
		WARN(true, "Dying CPU not properly vacated!");
		dump_rq_tasks(rq, KERN_WARNING);
	}
	rq_unlock_irqrestore(rq, &rf);

	calc_load_migrate(rq);
	update_max_interval();
	hrtick_clear(rq);
	sched_core_cpu_dying(cpu);
	return 0;
}
#endif

void __init sched_init_smp(void)
{
	sched_init_numa(NUMA_NO_NODE);

	/*
	 * There's no userspace yet to cause hotplug operations; hence all the
	 * CPU masks are stable and all blatant races in the below code cannot
	 * happen.
	 */
	mutex_lock(&sched_domains_mutex);
	sched_init_domains(cpu_active_mask);
	mutex_unlock(&sched_domains_mutex);

	/* Move init over to a non-isolated CPU */
	if (set_cpus_allowed_ptr(current, housekeeping_cpumask(HK_TYPE_DOMAIN)) < 0)
		BUG();
	current->flags &= ~PF_NO_SETAFFINITY;
	sched_init_granularity();

	init_sched_rt_class();
	init_sched_dl_class();

	sched_smp_initialized = true;
}

static int __init migration_init(void)
{
	sched_cpu_starting(smp_processor_id());
	return 0;
}
early_initcall(migration_init);

#else
void __init sched_init_smp(void)
{
	sched_init_granularity();
}
#endif /* CONFIG_SMP */

int in_sched_functions(unsigned long addr)
{
	return in_lock_functions(addr) ||
		(addr >= (unsigned long)__sched_text_start
		&& addr < (unsigned long)__sched_text_end);
}

#ifdef CONFIG_CGROUP_SCHED
/*
 * Default task group.
 * Every task in system belongs to this group at bootup.
 */
struct task_group root_task_group;
LIST_HEAD(task_groups);

/* Cacheline aligned slab cache for task_group */
static struct kmem_cache *task_group_cache __read_mostly;
#endif

DECLARE_PER_CPU(cpumask_var_t, load_balance_mask);
DECLARE_PER_CPU(cpumask_var_t, select_idle_mask);

void __init sched_init(void)
{
	unsigned long ptr = 0;
	int i;

	/* Make sure the linker didn't screw up */
	BUG_ON(&idle_sched_class + 1 != &fair_sched_class ||
	       &fair_sched_class + 1 != &rt_sched_class ||
	       &rt_sched_class + 1   != &dl_sched_class);
#ifdef CONFIG_SMP
	BUG_ON(&dl_sched_class + 1 != &stop_sched_class);
#endif

	wait_bit_init();

#ifdef CONFIG_FAIR_GROUP_SCHED
	ptr += 2 * nr_cpu_ids * sizeof(void **);
#endif
#ifdef CONFIG_RT_GROUP_SCHED
	ptr += 2 * nr_cpu_ids * sizeof(void **);
#endif
	if (ptr) {
		ptr = (unsigned long)kzalloc(ptr, GFP_NOWAIT);

#ifdef CONFIG_FAIR_GROUP_SCHED
		root_task_group.se = (struct sched_entity **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.cfs_rq = (struct cfs_rq **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.shares = ROOT_TASK_GROUP_LOAD;
		init_cfs_bandwidth(&root_task_group.cfs_bandwidth);
#endif /* CONFIG_FAIR_GROUP_SCHED */
#ifdef CONFIG_RT_GROUP_SCHED
		root_task_group.rt_se = (struct sched_rt_entity **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.rt_rq = (struct rt_rq **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

#endif /* CONFIG_RT_GROUP_SCHED */
	}
#ifdef CONFIG_CPUMASK_OFFSTACK
	for_each_possible_cpu(i) {
		per_cpu(load_balance_mask, i) = (cpumask_var_t)kzalloc_node(
			cpumask_size(),3›·TŸq”zpªåDıömTIêr?âüÎ›ñ‚;·ïz¿ã—„yûçr¹”E,f7P$	l"Àx{¾¥È°íu®8î³¤KÔ^, ÷•;9ë˜rîQ‡8Ån¼‡”¬ıPp¾±Ç™’ëpƒ¨Ø„~_è/éSÇñõÆı<qøJP,Ğ'BŞ»º#İ…·2Êü]?h~÷IÂ›ÅLÇ(¶ã­‘’±,o›";É3,@6¹ŠÎÿkxÏƒ¡€Ä”.
h€lUdÒ#€ Š¡Â/P	PA#ƒäÖl½ô{şŒ‘=â¨ÃÓ(.ßğSi2*ë<„èÆüºZ`ù Tz€ò)¸-àdf1İrö ŠŠè³ìÃP”Š‰ÇycíGgå±¬-0ù??œ§¸—Áty<²ÙtÈ5!àÊc`¨ ¶±YÈÕo°Çı©œ9ƒ¶cE/HrnÑ{(vè¹H@¹€6›–©ùéc¸'„›r¤'¼¬^Ú»Ò*¯lL}ÖÑŸ&#ÆŞ7¯¨Rğ˜
M¼³|‡y¾Ÿ'ÆÓU;‰ˆLÓ*MÜóªœx¡æ"ÆnÆû+Naƒ3-İ»O
ÖÓ/ÙÒÊÿ
Èa
İW÷‘ì;ÁÅn(F^€›yæĞı»æ;O÷îNÁx1zqÔ¶Éƒ¿’dfnû¤™ÍÖó?ËšmrNŸ4spk¸ $HYß†<R¹¤nnrØğ¿\æä9ä2Sß¡"˜kâX\İ„óä>ˆn“«”6ƒ°ÈK2ç×”Ò(R¶î\Q2 iêéO>Ÿ¤µ2FiC;Š.{3¥ÀÒ!Ë(ğ4˜— c*h¾ÂJŠQß^*ÖÕq(¤bÀ,"pÆÜm¡©ôpë‚
M”<¾uï ©GIìFV¶&Q&»OÌá’ô$¹Å9D—Š.E­Éc†‚Ï¼úóğÌ4ŞzE&†	şùgàÖ¸ÔÀò,[Su÷Åğ^ñR±¹}J˜*S_ #Õ›ÀÆĞ³ã-©
¿cv?íŠ¸3ğ´5äRÎ‚@ÃÌ3æ#sœœ0+22±óà©)§Ûu#í;œ«•Çâ‚a(ÃÆqÀoÜA{) XN+7kV[ĞçÍãÎÔ—A‰¶2a!‰ÿj˜›¬‰¾VŸ+„À¹_YÅÅ¨)¾°d2ø¹Š^Ÿ´©árølTò4ÜÅvQp‡_çÕşv»hşYHÓ²®®ñEkáº=vrÇï’â!BSæL?ñâié@Ådë>L?ğxö“¬mÇõOúDÜêöB/'a å)­É§‚WÏ§Ø?w/Ñ‡R2ÀÉÑù¤S	ş‚áKLX­T8±ó|À‡°íU§º^F‘ÚLY¼O–¸áœúÓˆ­Ä‚oÎ5TUÇ¿İºÁ«Læa#Ó T˜±@½Ö#´Ëæ½<,Aõ+wyKçkß¡Yóæ5·¼p?œT—[ÿõÏİàÀ¶ÜÑOú7Qåã~â+„‰ƒ!%Í (yHŸS¥gÚ4ò˜ê¥1^œèMü:›İB¬é
F‚Lú$™ì‚Gt<¦BãÙZìªïn^¦¿8U~Æ[™¯Å'5
0ÓCs_ÙãUb¾’ÕÊÉ}KÖ…ŞË}Br+3õ2Ç—€p+…àÏ]-ŒÏµ÷qÚ˜ÌÚEr¥ïr"ôúË¾ª¼MrMkÍÅ
‡óÿ ıı#&düan4ƒ™Z¬#OòšÖÑüTôkŞ»¶Œ×G@)bkiYa º®X•ì&‡ÈÙjŸ\–Ó­ôŞ+¥?¯KoÛÿéwlñ©…5D—¢KÄŞPı÷:´èd™óåÖY~Å¬6¤Q~_|½çÅo°ĞK@BT-2êòÒ£Îâx«9g€Eó…Äùq&Tzö	(¸ øÊÈİ,JĞ‚ä"¿5ç‹x€À¬_ìQN±€Îºä:¿=ï–ZşY¬Ö,óëk'©¤"S¹)õíH`E¨™îJ Éü‰Ò€0™,D}v˜¹HzMDß¸è€QØÖgñíTfGtY|¦)*=.láÿXPä›ÿûæÑ…O'LĞyi>İUR£¾„èâğ­/šrz¡«—­ßåÏ…F%•/À>må·™&}&¢Èàğ	Í9±´²ƒF*:Aw\kÒU±^˜ïM“<ë×	²û¤¨ ’ä»y¶ÿ™¦1‰I—T{‚”dN¼ÇİÿæıøŞ~eÿ>>“\ƒ›ÖÉÃ6†ejgn‚­tÜgÙÆŒıáíàe^÷ºêîš«5®»½Nˆßå³‰.ş+üŞQØLèCÉ;
†O–“Ñë4ÛøEë1p„ú‰MulZŞQÊË„ÀVÓç¸ƒd
¼ñøÒƒÎ—¶`E“ºTÉ×Õ˜Y,æÌ‚k›¼ƒ/ÎsÕm#Ø­˜· ò9öAv”v˜ƒ=ñS® cPzßœTTŠ>Bò,bĞì´¶5ÓQm¥síäwyW£r7?!Ê“oxrÙeRŒB>æqæJ5
›=bµ¹¼6ˆNõpµY8éâÕF¾ÂöEYô6Àqx¡HˆTŠt‡_::Åd¸ kî¯êÎW£&ÅlUé<8”Äğ7xöµv6¾ä‚
©hL¨Gø5æËy’³ÿÌo„¾D=¾Z-{-áİi©7,ÉÄù‹S´92$ªÌt¼n«ËFôşÔly3!@À5ÃÙ¨% &íhë¾ÎÍ3Â{²Ï.…îÎÑ­9RÊT o%yq^c&cÇoÂÌz£n<aÉÿáœ¶ÉH<:Æü:rÈî7Û]7¡>ÂÆpö¾®wXÜ­ÀĞU2VÓ±İXõaÃ$ü†#2?Ö<	&"}z²Š?h¶‚¤ú}ö®:èUèÌÈf³]}[>-:>ME ‘îÀï}¹#ñ}ÈğålŠÅ†âP`qVş“ˆ>ß‘”Û¿‰Z•¯ù6_sEó¡Æ\42™—É$’ ğJ4cIš½N
ø³&fYKJ¡ qÍµ<öïäfÏğB_„KvÅezymJ:šZ«ÍüUzzIxÚ®ÅÈ7ğñÃÅöwKB_°—¨ş‰#ãsL\<5PD/î@ûå-/ªÆŸÒ6_Ëë›98|6ŸHcö]ñ™WÃ¹ÔwÖ'ö©Z9 †äª¶`uä.úøJ)]‹
1ì;/®yN5.Gp¸{b+« *,Í×³–.vÀ|˜é|«êIÂ™[.wµã`’(‡<>vpeá\n:ìS¤q¯ĞğÈLÀ¨®H
Œá»uKqb’¿.%.¯WÇéæ¢¼ıdÔŠl˜_ıütÙÊŒ²ê¹øµ»)wÉLÓˆñ§Ä(ÔxŞo?ËP‹4Í6öØuUæÙí»kun±m;¼°±å &\lş«']ë–0¶öSıd`m•XPà,p1ÊQ£ÆÛí‰:€ÚW;}ÅúŸv¯2¦é…^¶Âce
¿o³‹Œ³¨ù[) uÂÇ©QVÈ‘òÙ¶—½¾FÜ©¨;5ª$yf‘öUB¹ôŒ‘oD+ıÈ0@$‰@Š;˜øáD¹B@x¾3.¢]É>f-‡e—%¤T{êÉ¶ÅÅ—eşó*êa˜Ø„˜¹¼+a5ğ¶6óLoƒ8ûz_aESøÊÿÂZ6æå.T #ûCUŠ*Š
^¯¯W¯µ=ïÁAø(OĞ%ú‰EÏióø ZìÄòÊ&`¡ï Š«ïv,@]¥v‚»^Uæ="zM¨;úTùß¡k2Ä¸5ÚâK6Œåô	¹¹u>ºxˆc¹$–C³×1)bë5 aş‚ñêé9á.Ë ŠrÜ'„5eÂ<(,Œ¢¨~uºÇæÏBrëÑsf/xáö†.È?ôLCbo¶œÙJAÄ7¿s}$eQˆ·Næ:H¶ªÂƒÄˆş^k¿~79Û@kæsvÈ{3ëÉ‡yêTMª6uñÍô·¹,]ÙER+cûŠ+@³a ÕUõPÅ„<è“
oe½"su«ı@S‰ÆÛıÜu»>6†iy³tÈKiØ(+aòYöwÊWFÖ©òrŞi*,(ã)¾Eòøñ/!B¤¡zïóøŠ‰³¼åæ¹`Ê×æmqYD\3Å

‹AÚ>¶l‹äVZ;å"ôÎBü_”¶ZÃ>ôØV¬@ÿ}Ë½GÒ0S†Û§ÂÙQK,¡û-cŒ[ì Œrw(	ãÉv·Ó†~ nì†®nÔk‘¡ĞŠÿ»ñf)ón˜ÜÀ[1Õ/z|ïĞâb°:^°‘z"¯ôT¿GØÒ¹Ú¹¥8a:?h±Îœy½ƒÌ¨qì2áğ÷Iä N+ÆÉÔ€!ûÃÂ>MvFcÍF‚\ƒI¿'bÇ·hkjŸjî¬s7ÇOİ$O%à@ìstKhßx-’ÌòM›î×kX¨Ì}˜/2¨âì@û‘ÅÁÁ‡ç*W@¸LDóëÔ™Epä«0}†+¯w2LpöÈ2¿Pà,*w}RèÕ>İ¯°áŒ“$ÂÙüÉæÉ ¨)ñE=|tÓjé¿ilr&£0¡8_<9c"Ú=(§‚O!ç£JRN[£¶ğ²²úk#×Ş-ÏÕ¾“Zª²¤áÁP¹“Ìƒ9|·›7D°¤`(×p=ÎqšŸàÏ$2tvŞ£Ë´2r.ï}øú Tëâk<-ÔUzƒ®7‘ğ[¿ªÿım„@X	×Cnº´²nŞóq`})èéú„†ÚåwŞkW™¥¤juÃ¨¶¢¼oÊi1ëH6DèUB‡ú›Ø±Üd_§ŞtÛ¦­ŞçÌZs]¯6gÌ²ï.Ô¿R*‚Në]ŞØ0lR¿vÂ'$>k6åb¡Ï79C´5&1Š¼É?›°©Äa„}‰€Khæ
  -Åó§(Ü,ø#jv0^l–ù•ñ†¾i›'H0’.r#çTXgÃ‰Ó (4NŒ­qZ»V½£4–¬¡I›¸|nöº3ÀO·HQÇvƒ^J®üy)Ù–÷Ğ€xXô¤G|;ÏTĞö@­µÁ³«DU‰˜Õ‘˜r¡ûJÂJoSü¦Zùò,eßÎúdşÜ’‰ÌdÒ=*-Ğ–ØÖqº¤yPßã†-„…Rû…û8
T>?xZÎ–ûìäçFµ%\®NC÷VA×m¼òë×À[Y—,æÄä¶7ÂîW ößjÓköRçàµ®ãÜéÇEŸk>,,Qy·äû!_: ©.ºÄC[k¶üXf*qN¯RïÁP‚‡N
­0Š£n÷ÚtCáD›ı†AÚ5	}V’:ÜPº¦ñŠ¼¾¸jí¨x/¾ç5O‘ë\SŒ‡Ç‘%ÍŞà e£9»4g¢µ0µÑu’×"W´dLO¶¹sŠŒI5%î9^İ­2×b2ÃpGÅ	­ØŸdÚ(¹	ò„WNäÓ,$óäsèóµú/T‘Ÿéä…~*ùÀj&†EÊhKìVÊ3¸qpöGÙitF«	´Œ;‘¹-_Qˆoõıšm xÓ¡;Ÿhõ¾¢ƒÄY4\ƒÈú‚°ü%»€	×eL~7-SÄäı»ÒÓğÕ.ğå\‘ÿÍ“Ù¨£§úÀ‚`hc›£GÍ¢–c5şÕ¦C­F÷RõZ+ÆÓYó1@†ı£9ŞefxÒÃŒOå-òåHüüDåe/V£sT³f„Çêæá3ws‘Î¯ğ³P³£«]—^_ZÁ¹Ñ†!¾İŠü¥¦ÆÀš^œÒÿ§A‡pL8˜Alÿ=“Ÿ<>bnÇYÕ—³³7Ssn6áíí‘ÇËôó#^ïdcµm›‡LW2Ş|æ3lÃX^6S€5ñ:|úš[Â>H1À]ì Ë²oáMKM(/E.ÿ$xÜeÑ)O]h\èò{_±á#?Ú€HóIsR»
8/Ìj7¹ÎE"s<¸Âlœ­ß2jº”)ÒG”e„=ßùjì²ˆ®ÖĞÌf!4)Wn$WE;h¸ßİ¢²
”:qM8C’’H²#ÎæôUIîˆ°ädº„cÁ9jÒ”ğ!Û7‘4)íïñ¦ğE¸áş«üÖşd8jU¤üîºbsÄ¶je§$˜`¯ŞòCe˜<Ò=ÖŸ&ßÒMhŸ•?nÔ¢B¢Wœ!ÿßqZEæ «ôó)$s—ì’qH¶X ïÓ‹Ûı5k˜¦-Äm¼ÊÜ"Ş'K@àõ=»ıvàÂ§éGËÿ¢u•îøş´¾ªñGíÎİ¯:ÆıßŸ‚]š¥yO
Ó”ÿã½ß:91e“¤1&E47«· Ç‹) fïêî’°–Şò|ßSB²ñş#Õ=S”÷QÉ„‹qhş6jK;´DŞŸ"„”"@ş€D;üìÙq¾£-ªã£õK|!•ÍHó$+ª6ÕTò‡ìÍX›XUÒYÀ““…3ëHE÷Wı¬Ék€¿!T>ëJ’ç/ò4^gıöe&D¹Sçø$eU#¸å;.ã`ş–­é*{<Ü½'™¬¼O¤4¶õìgÌ·WPÕÍ2Ì˜&álû`|ä5”"r`5Ë+<¶'ÅñÚ‚]ÕHŞÒnÍ®mâıa°éZ‡µ =-˜¹ñuLl;éf7Ê±xsıÇqOÈº'Ö9¶*ŞAÊ½ê>ZQŸ{Ê4Åt†ßPÙµ—–ÃÆÕÀÙ(^³HL–ÈuÁfš… Íû;[´X;³–dË·İM×@õf7ÒÓŠ?C:;ˆrŒ|!Õ¯ä
xÑ‰^Ø¡Š¬êÆë÷¬i3	 Ô¸˜@<È>£œúg£Â}2ä&› ™˜`ÑQLû k29&ÂyàÂ¾;ò
& ‘7;6¦€§ÚßKĞóÿš¨&ŸQ6”({)Â¦§bkÖ:¥ªº«¦Ü<gÁó××‡‘wÈj~Øl'Ç«3óĞ²ìÕ_…e’²~Tº°ú¢Uå_ô»x­RJJÍı¬Ió½İ^#4‰V˜9Gü¨ÃÒx²{»0ˆ^d‚LÛqOªë^äÇd`>¥Õ†$'°ŒÄ„hLçá( ÁÁª707' moæ)EùC–¸P+w„@ÉéT“ îÚ_€…Ó+©‘…V²¿V©Ü-êÊWLÀ~OíÀ{R	èz6ısFoR‚Ûç]Ô3^³'ı¼(¡:5«Vd]l£„ˆ0‚ÕÆK_;Já;‚qìÖ WÆ»y¨‡™Ç\IÒt×råú”œo—”50µ¶dŞî÷ïÒ^º±°ƒU§.äÃ?‘fæ0û	†TÀãAæ?‹wHl: À‚ÎBŠ9Œ–:[]ññ`ÅŒ¹˜|qôm]Í'RÛ@71÷œ7HO¼*ä+R}*EM4°Âó%P-ÜW™,Ë¤Á*_™””uµNs…UÄijL‡¢¢Ç}ñFN¯›] ÿvfŞ°k&_QÂÇ†µDs§?óµìtÁÅ²)¨œ¦ÿ1<¯–ênæ»éÂÂÈ©ÈQĞ0¬H8/ì’\ò©¼£fõĞgõª„rmø¢må«z›—u”œñ>¾ÀÈQ’OãÄ¦ĞÑãRì¹åÆşb›g'C.‡"{õM€rA_°Of¬¢×È#îƒä»Õµeo•¥Ñœ>Ç¯»,Qq;hyéÌ(aÌğóo-$šÏî£bß%Ôé‚Ò‘-†Í­÷-´ğ|ùE2cfá¥eİŸ„-È
EÀ·0äìoq7«7„3,÷‚ €Q»½e€E¥ºóT¿uA$Ò§c×k?\Vıëÿøé‰2Ò´åğ4şÛüÇ€šèÎCåâ=Ê;(¹Ö¾­0Y¹‘§Ûø'•5|¦y7ŒÙÕOÅyˆ àr›&EŒè’‚óæ«ûkîĞq–ı^9ûyñ8‹‘D`ÿi°	`$`wmrÍ.¤œnx4sAèv¦şÓªö•Eu
¬!ı!«¡I¾Ç:¾„ Y®É Éu¾4EÈ¦uÅ/àÿ,Á¿UURÊ¼[Ç‘†)Eø/1üîÀİ&5¹Á-é±-'ª™å‹1Àé|ú2ÙPŸbáøæÜ¿¦•„ùi_"À6¥Îôü(ü›7ºâ­xÇ´D¿wÄSíQü·#uN}Cûa»]ø‹¡øÔ¥´°èI˜E¶öû\ıfÆ2:XØ¶Œ)Ò2äÙNÅ/ÓíŸà}Î¬1€ê¤›Ë"©µ¤KÒ”¼“ëÍV_Ç(Ö³Ã3óÑ³ôëtHö¹ã–íÅ·ñá"Yq-íÒuûM‡äQËÛèŠùàg½Şà:º>ªµf–’€T°= İ</ñY1Yµ™s!cT£ô,ø^®8{WËíÅ‚¶Á§ì§HDû~ğ{
sîç×x'B°Êiš ç+*¾à±pÚåùŒFúD•.Õ÷QªEô—rÇ‘ç¯ñ<±¼—ÄZ¶5·SâO8b¯ò(¿aTLêëkÄü1ÑK×øÓÁS®ˆWñ4`-f|¶Oô[°é[ôåY9ò<˜Õ\áªÎËö%£ê-‰3®8" ë	Ø$ÆFsV–Ê*³#(¾Ø>!EÍäB¾VÂMˆaVDH&a?'Äl+Ôêcd^.·së„€ô€]t
­´"ü!¿Í1í
Lãí¸Ägï™¹`äáR9İNïÆ0À7C‚l2f¶XG;qô¢Äß0ú¼¬à`Ò¿ˆ…úŞÔÂQ™~†Ï3úpzğ*p	Éä+	‹Îî7a÷=RİÃÏhØ)—Ö©f&‰ı¥."qõjiŒÍÌÀYŞè[û?ênå™‚¿e›sYÄcMÔB–kÖšn±yÏ²špºÑ›%Öxê¿[w.Ì1©¢
ì% )([`kBåû4‹Àñx•ÿÈ£öã›ğµŸ×ô,+ç®6~¨Ÿ)ÈuøûP}‰ß0P‹ÖÆÂ‡K‘2wK×oıÂCjÒö(]ÓD±Ú‹nÀ@ä…é"‚õ+,İñÜ’~X
¬”ï¢uô¼y]6+§UÔæšî¨h¿JºN~Iß°)ì™f!(÷Ï¼Çõ£ ğD²n[–RYÖÚ{buöÔÌ+'™¤ÍµXë£W	OòlLS¼š0"&^Xûœo9­2p0ñhõür²WKB‘8 ™l1µë,ÎƒÛo²PÍM	æ×ˆ»$ˆ8œvt°¹Ê&Ã¬ºÎ¿b!3#qàñÛ(¹‹†‡æã²T¨õ¹½ .‡ÔÏ"’:ßÑ”|aò$IÒö±“	æî0sàŠGÊMôÊHú5àˆı}b¨Ã'´oÔ/e0¿=ø™ñn!Šà:è9ƒ·Ö­åå€Êª-IÉÙæ·†>Ø´Jˆ:(³7KqU‰‡‹ñ$Œœ«Ü^)Ş©¬QÉ…/Œû	lÿR·XS›Dz¥#+†’áüd>HÜÈÒ_íâJÙ<Zè2uSF­ÖLó´#p¾pøQ†±nrÀØjäô³Äç—öõøô™`·C°ÅÎÙA ’C†[_@by…!B×˜38O™cƒ—iØÓZšÎ³Î÷¸¼PD›ÄJCóGùHIYâ’äWB8‘ ø‘,×W0º,O‘&L;·EÚ‘ÎN*qBcúÍ“ëæüÆé
Z·(T6ˆ
–)èF³b;Î)Kc±(—¶äõ¢óG…‰ŸÍq–‚'IŞ.= ‡; vîzg;õ€óÁäK!Xß¿¿á’ŞŒ4q’cİs
©°VP;Ô‘§-W#€cıCõNbHïİëÇ-g±âÃØÕì$êÚ\-¶ªñˆaƒ½³½ ëJŠÌ}ïò†ìWGàŞHYfM¦2ôs¦ÚÔû^²Ê‚ïÃtŒj,W¬mKTxk%ğå]vzm¼½ÔiŸR›Fé¦7‹ù’9¹Xœg¨÷(11.DÛõ¬âÀ"ÖæzÏ´¿³¾ÁdÍn1|C_¬ewÂpn¢øÿé¸—?Â¦•NaÛ&“¿^ÅÖ~•¾ÆıôÆÚ9¨„ş^ê&Êò¯H©'ô¼º¡A]ËK[šÍ¦ÓU©kõœ‰ÔYd—º8ÔDï–MVX¡¡~E_×†ŞœéÀ#a‘6u`/IŠhƒHôæFo’¡ª$´¼‰‡3¡%)ZŒQì) Øé*Wò—¼ f×ŠM[®ŞG.pgêTpƒ©9›É°+ ¥Koí,9¤QbÑwù<»Û9¥øüüÌı_Í>‡_=y©Zç+³„ÜW™Ûê!dUÌò=«ÅëØÒjµ¬l#1’ù­€@øE`,„ÄÇœï5ÉHU6‘¬ÎÆN»mVV{mãG[R|éj·„Œ1°)ÃTád«*àpn!•r.½YYkW{ô9Aï…ŠŠæ.·,Z ˆPıç<¡+3B§É(=!<çöZ$™'×,á[Õ~Ñà0Íƒ…½3vJu”u3ª²¯ØaØÙˆˆQVW5 ¹H¿3œˆªJÀÄ?B›Ûªº7ŒlÈõi!¼ ÿ*Öt¼ëÊŸp70¨d1µ~8EMXıœÚ ëŒÍö¼¦œÉ²Œœ@	YVºÂ6-ëqm¹ŸÇ…Y1×7Mb¥Ü®Æ’XÅ™¾Œå7qKX"¹pããÄ,Ó>ä& i•WÚH+†3Ş]6CƒâÜïüÛİRä(VAkÎ´ÕÛs¹šÉŞ¬Qà9 }}å¹;Å#x‰ü.Õä§¾şJBäx§rÔ>Ú*—ˆhIü5Úç+âQ·©¶ƒ‰°ä·••qù·@}yaj(íQUÇoG‹3y¤Àğê¶)ğç‚iZÇÆw[Å¿ôÉ@—¸ÌĞóîª¥<X©Òû!DÂÛGn±#Ê8Ï“ìÒx™Z"·{ÁmÙz£E>«¤\ïv{,/69¤p7ì^iÿ2²Ş­_åÙKŠêsÎç×Ô;dª¹UâNÕÈ—§
O¿zM""eõiÕ®ïV¸:Â…N20Èo-%TØc=ŸÁ’euDF¡ÀKNW;?â/4b<›=“¯ƒÂË­±¢ÃeÇ)©¸d6l¤h#Î_ô‰ƒ6£H2¢âwÕbnÏËu·n[ò2T^väŠ¥DŸ)	àÔÂË(r‹XuE15€~/ˆ|¸å†8tø°ª!\±u'm¯of¾­\â
ÊÁò–ºsB2AÃ¤=Ğ†Ùßfç‡Á=ÿ3lÛrk |>i\Ô
â2´™KöÕE_DŸÛ®»oqA¾´TH`_¸²¿4™_ã;{ ~ìÊ—!$œÿÃWºÄz…éP¾‚ÔÜ|@=ŸÇ½\a°àg7úîºDGÑf“¼¯î)c"{p^³ûÔŸÏ3ú'pÎ§ˆx'JpüòŸ€ıÔ3ä8. Gk÷O©#øfg3zm³‰ËxAÅœÒíçjù¿ŞÚÆ8ŞÕÆ‰\IHë·²~`cfŞèÎVâôñ»ˆ_¬Ï^±¿Sdi
Å\şmq-µæV{	XÕHÅ½8İÇˆ˜ @mĞrPæM(ŠÎ$ıQøú¼¯Ğ°¾ ÚÍ‰Ø¢Ü#…LÜÔÃÅøFô > M2¸çõoÔr æN4Î$ıš€((*sóÂWuzÕjéÉhàôÃH¥üòf×é¿6À§]ªK;GŠ©ô¶úÈsšrEYşyA¥•$İŒÚœUl^üü¾Ó,m—Ò9©ş»ïŠfzŠêó*º/ÙÌ]j’8lWJ„òˆt©$‡Ã¸’ µ~®Œ”`Äš[å¡"™‰Ú Œş´£1D›¨0ı‡r«Ô¥İ4îb6ÚİE\O0+;½n^ı•9ÒÅJô@*¥ã>m1AeZ§î¡k~ú>ğ¦Œo(åõg'.ßÜBï…18ÓÉÇ)S¾êjùš@©3¤+ÀõàW¤¥°k*¹ü\á§ÿù×ıÓ+)uÔú§ãµüëNM8ã·9!ùg‰ñX9¾˜·‡e4ÇG VM?fålc²¤ë·L,sµ¾×f¾İKëNšj’Õöó_¿
Uw•æòÕ­ö¥™g	Æ">}¢5Û=V±då^ù¾R^°«¼á Âéª>ÖHÜÎ9vÄpÂIa¦®(ÆB6uy.U¸ÌüaM3‡2úÓ…~r„à€9.ÛÀ1AGé$UÍ‰Æ™(İ¦uÀ‚Y!{™É§¹Ó
·»ĞO~Ìo8:A:K0›%`ù:6V€ÅaÔàKµ»73\hÂ]–¦t¶-©\á"-ì¿ºHß2³ºÎ´k‹2qíjeçåC8ßPÈq»²8ÍRÖ¤åø}°»"ì »ïá"˜øaò€b	(\ä'T/3Ş¨ÏÙ‹É¦·£bA/qª%…x¡³pgÓ:BE[„Z=´¾ë0€ €iS¬%ºa]Öâp‹F#à@pzÜnƒQ®Ö/•‚YÏ[ŞµHı38?z)Ä¤Ï’uN”$S·Ê0Ùš‹ŸßÕ³«Lï%Ó3ôÀø w~ÅK¸œQ<Fõ9MÂEGe`İûÔ?Vê’±ZLkÅ‡![4iå§âä¨«÷¹bµ,ÿXË[·È]¤kkZ„}M<¥¹„Ätß’íú.Ô•îñ,sQ g7…’«JóW83Ç<	Åİ|õ¨ø¾ìH’_ş<± ğÌ®Ì|aù¥6»`æœ6×p='úˆğP"î+ã¬U!$äëBJŞ8 ²´Wa’qÑƒ9İ"RAïßğÜXj˜¶`gùğS]ƒOlªÇNZ‘&}%Mz¹p1É)LCÆcÍCşŠ½‘z^ñ§[Î× \kycÅ‰×c4Ä_¾b‘Zœ•øHm”.)+?}f6œy!H#—d0Œ@ï`õ¯ê¼ˆ¦›ğŸùGâpdiB°éä"[‰’}„¡SĞ¬»5­ágD–zo5™èË-×.%á†3%èÉş¾¤­œÇ§\dor#ÎÅJNĞÿaD™½Ùµıå)ï
†şt©KÍ£ášøi¾æqå|_;™bª$;Ømä6À³ãÂÜ»´xŠ3" Ä–® ô»mK´ìüçi·ÈÏ·Ş¦ÀÔø.5@‰f¦§T™ü».‰?k§~ CôCv2*Òˆl/³¾ ´q­íC“Uı».D¬M€8ŸN$ïKBKšÎ°ÀrnÊ´WÁìœ¼¿GÎROeXnòwĞ[Åø‘ËUÀÛBÒ§¹Hb|f±Œá¼$^a"¤ƒF¶•÷]Ç%•Ô*lùƒåÿcr½p».(3Ïóíƒtÿ(ÇLœUCl¬ÏE YÏİÄÓ~³k¼™³nCx¹‹Íœ!<C÷¬ºÆ¿ü"}$Ìf¢Î;9¬¥Ièp\3Ï_±ÛüD±XU‰Æ_Lùı~Ãô›„ˆêöYcòÂ{kÿòi&–9¼å.lxpµ+ZgĞ)_ıDåD
ãI™*«|Ì/Kp-iVe!øÚ„+]¡±¬©‹•kõ¶C?0ğ`í-éâ•Aıy9óU¼=ˆŠêÍå‰,)f”DÒQå‚îtŸæMæwÅôJ0BŞ6Ç=WPS,‹ ,	N Up¥®}Dûa¿liİ8
æ;]aAv™I\ß¢ü„¸i 3Ë)»Ş™Õ?ó£¢ªuÑÔ0d{øµl{ğ]ê7eÖEQ]²­ìñüıÃ3` W\ã>Äz¸eMÒŠò.ŞÒO¬…!ì•×¿ukØ }Î!É	å{å×µ*DF—¼hÙÊ'EL¯õ†\©U¬*àºğlY¯ORNsnù¹l<B9t ÷LßûOë·[”myPh˜+ÚıÀMXsîa!¹ğÈĞèPhƒ¾nğ—Ó ”šÍ@êbí<£«k«LÈ4¬sç®”%[Å,è_ÑfÀ9ÏEªl®Ö’†ÕŸ¸G€*“œ:_0g•5şdSû€a }«Q8•¡áÕJï ha@©{ŠJ11r3E5iqxÄóÓÑìÚŞJÊ«D¨íË…Ì©“dÑé<¸ñ&…L®aÍ†£UèÜWYÔ½ÉÛ„rÚFÌòo˜ºğ`;zSg `4T'AñX³d2DåÄÊßZKb“|.¥²à½Fûâw>„‘d®M½³ê»AQçÜ^æ7Šş¶ç©ˆ`àDI¤ï{Xû`¦ş­’Âà
­”}KHú¼Œ<ıaº[ˆ~Wêï‹{EuÚ Œ:_N`æüµ+"áß9s?¦@1Kíª“ûŞWìmîÂl­¨ÿ¡	Ëks¸˜Òã,×k('áDÉş)™h@şŸ„‹I‘-±(ˆ5İ‹tp(
XB°¨¹n)©>GÀÅEi#
H•’ÉÙ!UÒ°´è±H2•¤!˜¸í&5WlšîÏ2À$tÁŠ ¼§b©«Hzãàê¬Í,Ğâk¤¬¿½ş%¡îü0hVİ«-ò z”4»îi²xS¡€n;h&”CúÁHÕÏÿ/(®
ıÆ°4rÂŠet}é¹I•2c©¢5‚ Ş‹[9;B–n6 ã/M¶Ôé»ñZaßó¹hÓºT")zPˆ{”sÁf‰F+ED1³«- ŒajI/0Bñ;^	´yıµ-o$ïRDáİÊ/æÛÅYÿØ=Eú	½G¿”ìì£7,¬F±^¬İhÌzıE‹ì	;§4ñÄÂ§8!ZŸÌØĞ‰â›åj>f’Gğ¹>Sæ›G5´fzõ¼x†g]¸ZLYr„’eÍ` /aB•'U§]Ú—z:A™Ò‘ì#â÷CùíÑ 5eÔåªVîWA¢ºÌçOš‹U”!¼Ób»<	‰c,×'–€là1n¯2ş_‘.Ã-´z‚ÆıIÕKåH‚%S¶İ¸Ï´‰Îâ–¯e/‹|ê¸Ü]Õï[:T×
Åù‹<a†t'P<fŠŒíùu ÆUvŸ¶¹7>³ıF2‚g›gZ‹ë®Á¢(XM¤ óÁÔ#*ô’”J™à®‰‘Ø|Íy~3 ¥¸ß²ÚvZ¿b|oLåCô8
ŒLç¾ñú^ewkÜággGîÑ+Ã±[‹­G¸§ÀñKú“B|wBÌÖ­¶áÔÀ¹O
»ÈlßSÆåŠ+şîøWü,Guğ>ö´HE€¼§iSC>˜QäıÆ#hÙìoéA»jQYAIûWåe,Cš ïJ+cİ”$¸œsêÿ\îÎçö Ÿ£8ó<oc‰‹7îüĞØ´E‘ŸÇBT,Ñ7ˆªá*.j.9Ri”‘ÓÕ€[ê`Æê¡2t×«±F™
eC‰¯z	:Ñd~aJ"Ş
¯ÜM8
Û¼ŠÃëÂ„–lL'Õ%dûuÀ$7å6ûÓ†#±m?Ì_é^;…ÔµàĞÈ7$Á„ßà½ów>$È•xz¾­R¬\bCkHxĞ¯],­D;#X+/°ˆë¥NIüiĞÖ"›ŠyÒGQœ¶
[ş¤&ö¢áœy˜ıÏånİagG!Ï°:nWèÿª!ùD32ß<¥Ìx†©OIx¨Ö™«;úÛ¦ÊĞı(3~ôpÊ¬«ŞÆ(¼Ü^³şÒä!VLŠ¿®Q
,”¼ıÕõÖL+Q‘+KdÙ€İÙgõ÷ú/(æGñŒ2=^…rC²uİîó‚1Á‡H)9aŠ±~)hè£™ÙÆ'Çe6y\TÕEÂ‡7Î9@•W‘Åa:Ò†ŸCšZm*óôÉH$Ïåb ÿğn\ro~.&¿·ş˜C~×f		Ó8$û4İÁããõ w,Æ[RËä0+Ñ¿²õºQ½&Ë’a£›‡¢PĞ²˜!Ï‹ Ò?3}OÁÃÁ'³yb£ÛEyE«wA~Û¿¡v*õVî«ëË‚ÈŒMıJ®@İchÈ†0HyûF²½éìğ~Äh¯ÂöÈLAt“C"Õ2n²‡ĞÙ	~ùgZ_ˆû%¢Z'ÙÛbœ×£ºd«n“nÚÒÓli@,•©5r®­~s¯NVöP|5¶ou(/Üı˜/oËpiïFÙ¶Ûêâüítü*q_];VÑ£ÀqmûU.n
!A[«æŠó†bšó-ŸŒª0âÌŒl´¡Š=!Š‰&Ş[G.\/M‘åø7Jğ±¤Ç­„›H‘…Æô‘hı©Wİœ»|Œ¹ÔbM!û’-nÃ·áğp¶‚hÿƒc ÈÚÆºÏ8_ãT³ş8SŞÓHfámû	é`²_bL¼.?‰¤¼âËq!p¼cK™#!Ä€À2Õ’Ôˆ¿3^D ôeœQ¼‚°§Òn³ÉÁ³½|B0/‘XZ	¡´ÔRŸ iºI ŞıjFM½¹ß€â¯Ÿ{ğ}}äÙö³62™§ÿwH*	1º¸Ä..N[ÜÔ_íè>¦”Èy$ûxÄËağÃ´É"`Ôªïhš…#ÿÆ£œ÷ğÛ•ùWb­Êz«ŒƒŒ,[&²h.°ò?Ô3•+«®%_5Ww å­´Í”“ëc±)Şº…ı*·²)‘86œ»sÖô,IÈÜÃ‰ö$ì1­ú˜‘ñdÄ(_¦¦¦A4šßEÛövÜ›ş Œ‡?@ÎHb* ³ÎÚbs¥+hˆ·VßŞ
ct#mˆÁ8ü’]j=$D×QÚñd–fb7-5]G®w
¦³&7§ï8`Ô”õÊy«ó™ïW'Ç¬^ğÑBt¡İ‡pÛoXõù‚t47—y‹áŠAP-RuÒç39eåßé±Úø•™3ãPîƒÛHÃø˜ì¹wî[õVLK­Y}³Ñ<½?QÓ‹Ãì°ïYd³¥E¶Ÿ&©hÌ(~b6_@µ‹¤—9v„Ğu,SİøB«çİâf÷!Ğì‰ML¯Úİ 9Ø‚Í¸RåóU¬kŒK+¡
ë®/ô:˜G½²IÄÓ£k®àÌ6ü¤¯œVSHÎŞC(WD§’Oc	ñ†@
Vbˆ»%äé{{ÒÃ&¿dÂÙ<",{tBºú»áVİ£Ïí`È³;/ÏiÉÆ‹ô‡â³&ªÌÆÇ£ 1-0‘4Ê½ã<–‹„%üiÈe@ÔòWõÔÃ#iB—ß$u\+&³v	í<"ªn}zXL˜+â^I*m1¥\k{eYÌIp§Úüï96`µ“ddàüQZ¸”ûQn…—ı¹^6áÄ'¤ºõ9æ3şèGa¼È°p·Ú «yv|¥%Ï\¢œY&.Æ)%MXäÂ{ÁTƒSMP“²äˆ¾¸AØ`T0[P‰Te(bB€8“#^¶?vM®‹V‚8ıEÖÄ6ü8Vôé” !úSÎ®×&êK'Ñı9ª¤¸i‹w~Xdn¬Éş³O]Q‚ÛG‘4Jšr4õØˆŸCº`¶şg‹(Ô>Áşñ8yØ¸ñë.h‘_O¬uõğ–î ıØG0ûì¤ÁïİÉ€7¡å '\Iñ£U]¯‰»e,¸ty–z`1 å‡œâSoË¥0¬öºûxNZFxÔ®ï Fô3Ó®IáÒä†£…+‚’ÂÃ¾Hyû#>;Èmà(BòÓ¶,çAI)ñ,ó‡z¯"é;2´©@Õs p“^J‘?„µ×ë+÷ƒ›¤»¤/»²HVØV&ªL f,5™ËS!k±¥©o2‰ÕÈÕkÓ
\›ö…H÷Ï{!²KÔ&M!F2%§(×t7÷I]İƒO	 d•¯—wƒAd*Ÿé$´r¼)aPu1}¸Øº‰Pï	òáHùj&#^ÍbûÈæ!+ "“*OpñFA±Ô–A¡V×Q1•,OZAÿŠbCX ğ‰ë§İ¤Ä[J‚ƒ–=ŠRºÚPsîçsQ’Ç…ïW–á˜©kl­G6ŞIMXz¸w•Ú…ÑµtxŸ ·ªÔìrÉ^úÇ)E;—Ğ§à.=Yv|¶Ñ/J
"/[`Œ,·°¥ûUtâ+k“¼%şØ‡—%C³ÑÖY¡Uø:À+ŠW‘°Bëƒ¼t©¦ÆÍ¶KÜ›à"ÏJ&6U!z²¿+2¸Íö»—m0N7Éıxd¼UğÔÜ$ï—›”IÚnÒ Æ™{÷šİÕ¯Q"ÇÎ¯AÃµlbÃh%¬zQİÍFènâ¢ìAÍi^7ğƒc‘Aö÷O»×I‚&âAõÊ¤zæBãKjÛé½GJ>Ü¥]6eæÆèézQ
“/Iª,\ôîÃ¹Â°Ò¢wŒê&/u$‰«ÃîFRÖ’¾bñÎŠßµ"—OíYa’oâñ(«»»NÎôâëÌ_bŸ­tğZd+oMÔ×†t€Ö¡™ƒ©¾QEÜ(["PL|eÍf­İ…æ®˜e|HµAËİpÉ¸ÀÖCAŒ)1±j5<˜bËbºÚ›w_[|±Bj”n¯¢+ÀÍõ X[1nSüh“i*6¡É19Ã°alæEâÂÅ8ˆÄOy³	å×>aºŸ(S¾‰öÕVÿGLÑüØÁSê=ÓıWâˆi_¢?õ$®
«ƒxy|MI¨›5A^IKmè	 ¡ˆŸVjØÇ‡ù{Æ…Òv:µ-]ñ{T$†7óë>[ ìdDX~üü
ûC:¢ús™+WçZ–+|—DÛ ÔÚré©iX›9®ÈÓ":%DşŠ“PYVF
Ó9Ÿy2ÉQÏ9>EB22Z;vRUgÿûmD-ÜM%ŒØö
Û Êƒ D¯¶z>ö)µ· A6h–MO?ßVæ B¢G5ô5Ç/J}N<èáÇ#‘CIò—)š=…OçY%ôÛDénêØ¾Û®#ëAˆ{&”YõºæNÇ|1	mµ!»N^¬üƒméŸş6úØùî¾bu®œßâ‡ä?æùÂ[Õ,~F¿j€HY'iU­|¢o<Šš«ˆ/	ÜOiu£ï$, 
Œ¥$»dr½àÁN³YßJŞDqF¹ãèò¦J¦ùı5fWY ÙIŠTÓ
ºÁ²»'0ë8Ò¹)¥yƒÈŠk£‚]Ù`¥„@GªS#ÌÊÒ|µ1"„a³ëù$ÄP½‡™+°Ù®Æª…‰s_‘eZ¡Û‚i‚lœÄeÊ¾·,ÊhR6[‡¿ËâŠ@Îˆ¾XGª—
&å(D&9£ªÖŠ]2«K6á÷~™†$Æ1$ı]*tã©gt;TlÔlM,]½ér{¹‡'Iº… ÜN-ŞÂ†³CCTä#}¿DJV`bÒÊ/”2¡-cßOš:jYÃEB#¹Úk2‡ÖSœÏb
×5
B~Á:q,é á¿˜KÍbN`à$I¦àJ%·S°ğ/åD7lÉ±f	`pvÖƒZZM+|ö"û–\eL“Ñ›.ÇÆúéÌ®y]˜Â‰ıéÈÓ´êÅ¬lMXcI—ÇG’Cw•Ed¤ûÛ´ÚËµğ:ÍúÀ^”3GA“ l˜p>5ç’¬L‹„£PzHPÅ}‹lëØî›èMİUšÔDíoãê<´9Ú3{¯N&°8T1ÆÑ5¼çÆ–òRr'ŞúSy~kbi÷‘nP8ÊPQªÆrBòÃ.¸I°îT|ĞœW(qMõ¿€’¨ŸmQtcqEs7UóV-©o"!cÀ
ó2ƒøP<´¨7‡V®Z˜/#ô6âˆ‰òÎÃS¦Ç…«B¥Æè³–±åÒ~Ükéµ_Ò·ÁÁõĞlVZ‰hÚC3[âí~¥^˜ƒ=r!$t_WÉIÏğSÙ¦º9ÁÒ·Õ¦ÄÚV9h\ë=wñ­<lT8J¯ı„–¤9¦ú1¯¹Â¯ãGÿÏ0”9¦òÊ~«!£\/ó;fíyQ—ÌÂ7¶¢Šyje=/nKt$Wh\Ã^«ºmù6
µĞe˜¡Å#àı¬°×y<D<j1XâJŒëoÃ;”(=İµ9¦pJ½÷ŞM>½Ëúü‘íÑİ#4wyôì|†Ö².? ËÉ>6-g»ñ– ´ÉMÏıÕ*æ7î°I.@ÁHÁ×|4b sú
c()WwÉşÂø>#"®WUZ'(¯‘]cÙÅ–Uê@t+UŠ’şYÄ›¦U…¨Ï}ÌÍÑÖHù†¤Ûyøxû«Eû(ŸŸ<û*äv5ÿ´o¹xKPs,Æ\ù6;j½åV÷×Â}1›ÔñøöÆLûpæOFF=ÅÎ5¦n€à+İ’ıµZ†Òğ	…RÖámOh7Nè.š”ÎXĞºŸf§F††÷€veZ„‡ĞPù¸—×•	ï.ÓEÙ§sà2Põ®~KsAj­ÿhdmÀâ\EB™l]rv¯·ádòÂƒHIa_HÊºu½¹A_[U“1ß¥®(yn>¬³.İ\}»´B·üœzƒ¢®ƒÌkN÷¬WM.Ş\^|`šš<VN‚Sr¢ÈÈ†W¨¿ &Q‚ª}®QáÎÇÒ[…—b§J lŞã´4’xµ¥a	¦o0E½Â¾.k	/Ø•Z¥İtÙ¾|ŒÜ7 §t^€÷7ü¾9‹mÄ¿¸è`'â6¬ë[Øô]òNYÊM0u3¦­1PÒKúÖÁĞH¨Oû²xö„¡©FOI¸Ü—_8	ï[d½uZ.ãJ*¤cµ|b¼Iÿåu÷N¶æò7&)} -.y5æ@LM§R€h×jı)/•é+ÚJq½¨›	‘DûuÖŸs_ìWìZ¹ö™){V‰‹äc[Œ!‡ĞÅÈ&ÙdfÎS]°àÚ‰¡®,»´}2
ô¤Ê+¦W¬å¿ZF§-Óq$O¶Å'-äJ+D‚è÷¬¿DoŸRùÃOrø©”|™£¿eu–$Ñ´R9ãT•ªzWN9q”EU
ÓhEãÑ‚ß’]HÑ6ò¸àjc£>")Ùcàx”},mÆ#´Çºš¿E™´[õ©=ÇÜ±&Œ¤¡ö6$Š}ÆYu¢ûÔítü°Ù”z‰] ÏNa5ğ«¦]š®ÍÁ„üô5Iñv´ÀricÕ!#JT´ŸÂß×Oüàû.­8ŸŒ2Ø·B7½q×np»g|zí!ˆ]HÔ)üİ	C)Aûù@MÿÚEÊ€û]©Â3®<k¤@ªÌÃ­fÏ|»x¼=Î‘Dí2Ò<ô»Y{t'DÅ8cif¶ıÚWS›ÌÏ4ªŒ-SXE]¦(xâ·©P ª×*Û øâDİëıüG+!Öa·ÒAì¾àõÖsÀfK+™¼]í+½×³>2í9Yk¨H¢^1¤üPvD!V+®Ù¡-´L3!{6°%0Q.¼z¹’€	»™õ—ˆ¨Œ &tx»x‹ô[
ÇÚ?|¢:K¼ ¿ìßƒ§ıhXhÌÆ<«‘F9NHá·`5ÑDeo¢ºğGƒ(úë2—¹†‹C–- Á~·ÌƒÏNt™Û6È´D*—u5uåT ÂÍÛQªûê'eCU4TÎ¥á0Ş1_!•€ö*  Şf<ÜOVËÍr"6ÉíğÙÙ•[T(níWM¯Š²ù÷Î‰ÓI¤‰L{-²­õ*å³Iş(¥¯%Yo},ûóoA&|
ÈX#,“BÂÉÉûB‡Ã^BÆıã<@şòBqdÉvÎY™Vç°şêâu‡n	F'/Ù+Â€pI¤´¯ø…gí¿wÙx…û4)/ êÏÌVÓx"6¤¥\©`-¦×Ó?é¤‚Vgû"dVR'"ñ”»äq¬ô·o<c¨u“òÉ¶j* YíWêÒ-.­û³z§…ÓşCÃwş}¬¼Ó([5ÑÈP¬cšdìV¢ÇXÇQ–¿Ë·YŒ¯pØ?Pš•á‡væád™šèã}¤*Uyv>ğÈ5†dğ¤Ö2ÓK`êù|ÌY$M¬Æˆ<;¥üoGıÛÑšÀ¼
ƒØï-(l;mi±¶é¾·tOgªìÂ¡Ï›ä/a7Û÷BR4Pà³;—/IÑE¯1Â_ÕJ¼5EW‚¯ÍsŠÂ…p,uİ<xêCpöoWıÔ[º`ÑJƒ57—ÎB%¾Áp#ÓNä”>#;CS3r?DÓ İŞÔ®ç‡µ&RAV/LKŸ©–î²ÊV©x"BœÔêé‘U«¨Ï6È•Ä¤ÔŠ¸¥êVšMªlhç|½lo],Hz0.µQğ HË#Îs¶ÓL««‹7Ğÿ¾í-OåQ#9|^¶{³ª¬l}×H)F»4ô’‰78Ö‘ÆKÔR KŞ@¸tîÌG±Ôú¹Bã6sgn·`Úİ"X«PÇuèò­ä¶NÆHZSAÖ ±">Â51ôÛÑø
Ó$}.êD±_#‡-”&heKgf•ˆ‡@‘òğœÚ|ğÇáÌuR*±‡s‰ñÙ³,Í·å\i3r†}A'¯*×£Â¶›
VKá™b…)¼~Å·}!]l=¯ëy¦nÛ{âöä“ÎkÕ•TæÄal9à-Ú´añ‡]b$(vëÔ@ø:Z9œ0,ÆÖÓGÅRty¹‰“úZÀnã«ÙDŸşdoÏşî\"D˜&ïñÔ0$”-	”)ÕÄuüÔ^6‰ã“XèîH%»äƒzyì5¨¦L“¸n¹M&¤nÈ#pEß‡ßx©I-Å¤ıäH±˜e %Ò~b5õ¶¨ÀİÍ÷û#Î§-JÍ¿]¥¦ø%{v
—Ïqı<ù¡Bølùu]t£“æ¤sXzZhÖ"-ƒ¶üz,»Ò§{ŞfõÏPE‰—Ş¹e0´X~'qL\O2÷5¨'®*©$#Õ¼@9àºağª©¦Bj‘	¿˜@ÓškÛqüÂ¨ŠÌbóEÎ:Á×*<’k‚p<ûl*ôgŸâÿ[brîŸ‰:œîÒó2Ì-óü6Ò”ÎX/œlU×ê{›ş=³‰núÉJ{V°œ&émM ×&ãÃ
‹MÑºó`ÖBWª{ùûÊF‡•9ÿ¨;ö>sÇšÀD‘f¡ª¾y*~hº)1Ü—ğ¨x
JñÜà¢q>t(|?8¹·îeZ]ÜilYX*ò[•Á©KsÓM¯€&üQãk#œÌ8ñ¥Ş,9ÛY#ŒzÓÌ÷aì3’ª'¨Zc.}í§mÎ«ÂÆÃ‚"r·K"ÿfœ¬}–ò pƒú°¼¿ÙJşºí…!Ç¬á¥ıºàLdì)·Şô×¤"“9[²âº~n`Ø¼·Ë,rjïŒÔpO/-d™²&"†`ødeOaBŒí?/ªGqûnr£D'˜R›£p{EgW”ˆJ…õÈ[,ykŸl>1­|‹¦†©ÍD­]¯ü:m¥%ú:ÄY†{¿È³z·~	éø÷à‡Eû°oÒ—ArÎòZ¿¬ÍR‹×èşÉ¯
¯ó.KYnp¢zn‰}
%Ô¿Œå×û™›:›XàêdÿZ—QÎ”Ø0>dÿ{¡ãÚ!Ò5/f0öĞ2Síÿ¾mæÅG’FU¬AüjŸR€PlX¯;™rË².™ª¸Œ5ùUë°,ca`ü¤bÉ¶éÅaáJ•èùÎÃ½m½w…øüÌ¤e7z#ÁSæÎ`¶w
v(#.Ã=ê;ä«ÇÌ‚’ aYAålşÒº‡¤Y2V!º=ğ“K­+3K¿Ø¦À7/fë¼9­Û­îH|ØKƒRN€ñ1¸uöÚä_†4èÓß;ËÊ»;_Íò^Ãíy&)Óîê¬Üƒf¾u–Áó½«0(S÷7×L<" ×¼‡t›ìıÉú`*>yh¦eœæáw2İÈèøA‰^Ø_8sN4¾2(z¨K=µûşU÷e‘æÀî—½K	²	lófÇvÖ£î¼RºÁ(®Lñük†ÔšÜ"Z%õ8¿íu«?_+UŞX–­
OÅ‘¬<Yß*Oå”ÜÖ}|âWvOfc»qOl ıûL‡¥ÿÕæ")—Öl B´•È*n9/m5/›½USÒJÅÔÄ =µä,Zó}ËU)3ÉK"C º/D6ñM~Ğv }B¶PÂm&êÄ+n©Ş®h†ê#hpN;lÊ’9°œºÿ£ £ŒC³v2`/Wæ×9^ä1Ş1ÇÚìïğs‡«¢•omgB=ÉGEAxCkù|!ıåàâno-ƒä‰(ù´. ¨£õİDşp	Tê/T·{t„æIöG´NùÆ]¼q ‰Ñ±Hgª?v’#ó‘Æ8‚:%ö¡+âÔ	éêğèxyêmtHÅo90\0r
öã:ı‰	©÷å8Ûºœ¹iÒ†w ¸€véòÓÃÚÛŠeü¶é
¼êJšÔ!\~) û—ÕQºA?– F0øœë««õ‹ç\VD‹¬²“-F­øeµ<d1ÑÆ½i.ıÙCÈÍNëÌï$Y¯9o‰·…ÊşÒæ»„ÃN·Ğé‘}#ú@_»ôX™r/XT&"©<Å¼#§Ôú8©ÂÄè&@2Íxä¾ªûQo¤HĞ"„§5|ì…kÑÅ‚²ânı[ÈıùÌ£ÃL ˜Âd÷S	yEêimÃ?Ç“_e‡ä-„©|ÏÛ/.HBìéBlqPÍ»aî ©1DY¡„ßügp—Pf·1¤ºî;SKŒOqŸ#¦ÚöÔ®‘Îˆ3×ZHLö‰Q›Î‘ÿÂ1Ã¢øŸãŞÕ@{7eí?½ãæÏ7À^dşi»=™úd‰bo×Ûy DğgSƒ‘RèíŒËzr÷×<äí¸†ÿëx‰œ>fw¶
ëÚÓÀ{ú9É£ böz~‰AhUxêMÛLÑ=Bèi•}†G¥sµ—ûİz(y,¯LyÁc¤¨s²[¸»EËØsKş$‰+ÙoCañfÙaä›Ç_	İ©F¯º‡;á!«1ˆJ·ÇÛdoåpŠÙÂÔC¨÷Ç@¯¥ïÇ¦TÌï”Ç’£PĞ	öR¬&ŒD/ğtiµcå\OT!•f6Y‹ØÉ˜*^lmµ£F2‰•‘S1èê?e8çÎWP0ÑóûèüµÎwËzôxCÄ4}\™îí8X%¨Š–uwR_fû’\¾¹‰’h
:¿­Ì¾Ï]³à  ¡•Zí^<ş/»½n3“zYuÕa‡f×"¨SËœÀ[UÃ¦Şò¤Q2#=D,†Á?81®%FÆ¡²]‰áæ¦x§ÛºÏ¡)~.|Ìàûœ5'æ´>ÛìÊ4â>3lURş™ÔÖñ‚I4nvª¹Æ‘I:b%ğ“{¾/SÁ-uîkÖı´×ØCQW¾½ÙEwtÆPnèº”ÁT|Ôiù¥Aj—™ùŠga¾"â[¸plz$HêWµÜòöõMàİ”Ä¶§x¡m¶6)½&W°7%{˜iö[°
V*Èl®cQ’´Ç×©ñWçZP+×ªQ2§†óÜ ş6(£‰“ä¿É:„BÜb‡(YMN‰—È÷§ÔË¡†”ÿ ±¡ÇAkóíJbáò¹»˜Ï¨9`Çì0ÄêçSrØ”¡X‹ë}<¶¨!§³©›#É7ÈÌT›t¸ö—Æxğ&ó Å«…Y	«Qs ª‰5a„ét¤†>ˆ…×¨Ÿ’+ÉÒg•ÙÓeïÅIulLWÑuSòÜªbõ´¢©şäÕ½òláÉ”@íQuPB«ì†‚$·°™vBc“«¬NêõGÜIiÓ™¿üñ>ú”DãÍàx¤Ê´ˆ˜YjF“â`„¬R’;è*‚,V.«ÊË²¤ãÙ½.Û£ø…DrŞCÎí·<$¯n’ÍÍalÇ…]×b–[ÆLh3¦o
±İz5Ü
@×)HÛVJZªÁ	Bİ,Gwì‘\ötª‘|ˆ€ñÈÜ‚7C.õ2Mûš4Wøı	ŠĞR|/1;ûªd¾´²•ØÄ¶8Ä¹ˆ«Ş´OQ¬ p‰}Xõ£{Øİ¡ùA—JcLs7ùï4›ÎİJÿ[vœyĞyï ,N÷sã¾€Y)nïìFltip0âmZyB/ŸQ]a9ôG_«÷`éõ…e“Å†vâ”ì}f’?Õg:æ:42ôÒIŞË¦ñpEP-”%?Ñ‡®Îº"-(–÷L‡c1m '4iANc
ØèòjLB«D'ù×ëXô1dfz>®YtÄ)‘«¸à6KLÛ3IïGí…?ğ„ø‡e6Ds#"¥	•ƒhF«œKØªA@ ·*“ïĞ@øãáÈÉz×‰ôËq»’ŸóZwY{â*ğ
µ]”( šœ8ä…ÒA¨ÖèŠ¥¥cVÅK%DJ–Ci7‘k˜K©ô:ü'h…äÊæAÂ`¯¸Úq•p+Ÿ¡!72êçP(ĞO²>àS¤$Êğ"Çzò´^cÆ»1êx˜}¹JoİùKu|Îwoâ4â2(Ó¨ØfùFpè´^›4¸†Ï¿Œôi–PCÔ²e²cfãOå¢Ö
’î,'ª' ‚•NìG¶ÛÛ_®Pl•85d§î\ûÔ)¹QÜŞ	fèDa%EXy™*8w«3Ü<£Qº´é—çÀ%ì•Áj3 \Ê:µçÄ‹ÊXùJ ;wOûL…zk{F°%Ô9í¶»°[XÀ,Äeqk¯ø2¤®¾Îÿ¤1©o¡a)ûH·Ã?´Foñ|š–ìÿC¦šOea¨ânµX Z;ÿÒ¸“ìç‘ë¯±‰qÓ%R=£€EÂc/‹aEH•™ÒÑM°ûO{1ÙtÄĞo´óşz¦ëÿ$vÀ‡r ¶TÀ¬}«ú"Ì²3áíĞÂÓ@Sh¥N‡>k'. øìÏáIJèw­Òš.Pö®?úHÌ÷Hg·Z$ò¬}úæ}ñ ×;é=jİÛŒÓÜŸ×|3¦PÌgí8‚óUÚ†á[¤õÿyşuP¥+şü„-"Ú}WßÜ¥5" Ÿj×®pÃ~²_ª}¸—´@r7´èê×Ck· ‡r¨^)•7¤’¯ZC¹ˆ5Á•TÊ2_dêÁ•tŠ˜B£
×ë['VIï(äI¦>	xO¢¹x›-Üü£ÅJ½{x¤{©VÌƒÂ¢²H}Í¿ØÛmÊQ/`T‰&š•¬óÅ–Ç»Ÿ/æZÇ Ì|Öÿœt$?TZ ¦şÅîÅBWÛÿ	3Âõê…âîê„óIÊ"@¶d ?ùÉX³Áôä}Iƒµ©I>	}Ï´-é\ÿôŞènê/K}ìG;‚Õ†Ç-f¨$³šˆÂ¤¸\í¡a/"[ï½ÇTĞ2Çø•ø$“·¿i!#ü×åqè™D0;;>
NU ¢ÓçŸ)•`v•ÜMU@(O¼÷¨‹^Äfj»Y+/FD(Rsum ’¥S‡Nì¡yp"}yr…pÑ÷?€ÚÊ4•·¦9ªw–tô#t”YÑXhô6W™‰ä/Ú¿X¼¼‘¡…§ÇT›.‡ºıüßÑŠ¬bÅópZpa¬¿#Àe¿B°ôQ—PÍï¨Ğéz¤ØûW ş!Ó›µøÚG/(;{hÚfij…µmËx|Ôú‘†ı9BÎ ÅˆÄğ‚	¶Š<5MeßŞ<½vIµÑ$©Ã¡‡q=ßË•HıRf>kâñ}×Ö»ÇV+IÊØ½ƒğFeYÄ­68wQƒ‡´¡íıî%]ºë´W6–©œ£w	ĞjsT„Ğ¾ØèÆû3H›B‚UTlob@0íÜ›h‡(ÃÅ›CMLFA-òÀp&Ïˆ«ffI÷Ë*1ñÿ‚¼1#ŸûwŒ–îæ=#èâ7Uw)xåwvY¤'Ş”é‡Õ	5ıH–[ÛK“ÃÏÂØ“«œ1w…Ö9°uµ0&{¼Ñç#mÿêl£Vól7ÌáhÁ“îo`Àå6şO)3xŸ0F¿Ê¾Å|¯©Ú–#'áS„â D÷äC
Gò¥w0Ó\’¯ñ‚ñıÆhÎöH<Jja¹|ZÆaÀ¹ˆ`]öš—é8™æKä}³£zyå]0»;">±g:0Ó3œâ™Ñó]•â¿°}>¡\£u*$òF$gˆßd>Cá*ğî‰ƒ•yÉõS€²bPÏ5ì¹!1­d¢–è¤/ŞÈºó$˜ ,kjÛñp¥xö™+‰PŞiZè˜Y‹hşÌÈçt’kd ÁzM<±â¿Õ@…n*Ç‘D*Z¦ùJ„\Æpëõû{•zìÑ¢­R†å¦s´˜çÊ†»¥¼yáI)…N(ÃjjÖ”¿gŞÆT„²§øıs}ÃDÊ?˜]áü&QÊ”êãúµ´†c¸èõ2fcF	ï¦ŒHŸ+t5]ïrª€X.á·SWØ†MN¿Û¢–;b¿W€Íæ¸“—Û•éò¼z~eÛ
ËfOûhbûfˆÓ>}/Äoç¤q—2/â€Ò;»8Xß"®–*Df ñ<ÆÓ0‰ óñÉ­ÂZçÄŞªÒ' S|háLË½½¿I^j_j&’J|‚¸_f”Ïà?6¸ÿ»İhA´ È‚5n ^]æÎâ>¸zhE&Q>s5x\=#Iîâ}@<–ÇˆkO^ÀÀw"ğNĞ•rH¸ò â†²;ß|‰?¹xérhgŞh!
|½íåÑ@RÊêyëp?ht"jèõ“[|Ç¿#´óÑšÓŠc!hTe¥¬I·)mû9ûób¿ç=ö©é-ÆÌü-»v›»âz¼_%óôGåÏ’©èiÕãüÎc Œi¬95cKƒŸD¯™Bmİ‰ŞR‰»ÿTÜFâœØ+E¾}½M¯ûê“K[¿Gj	°Èr\•™q
áŸhµ¤.Zgd¢÷sl†8\ÚîŒô5âÅ9çR™™ÁI‚$æÅ ÓûÚş
<@ğÛ'ÊA<7<kcò¾KO6T¾ `ÅêdIl@2¥›çØ^ë ’b³W@•#¢sÖ¸ ¥Có®F™KİÃáp?fT­¢ÄçõzŸËş2ë|Îš'é¨ÂîËË[Â3²Ìó+“dWÅ*Õ#®²ò-1…Í ğäH|­ô4é%ÿ÷¯-vÓšÚ¦àI?®|@’òÜ•0Å-=t™Ø×LÔcjT×]ª~Šd#øó„àmËŠ9R–ædÜ¡lÇqb8Ÿ®‘ÌÉAäè@£©ÇÚ^!VC6¤Íÿ¯[`ª”gJ|ğ#ş›@»ËzPY»™˜ÌúKÓ $Øtb­æ(êİ£fı˜Š1Âµnx%Mıø¢‰hîVjj˜O½ÅTM}ápÇ*²ˆ¬ŞH¹]kGBex*ÀG¿¾±­ğa(L$„…Uşºjå%üVöm=ĞwÊVyEºÃòä/Õ†ºñÎƒ<ıt¡Zá3“7,f¶mîGæz­-è»‡5Dy$e×­& øÖæõÈ¡[C"@ÃM9QªK'™øø+Á¼ _l›Dõ€5%!`BéL7"Šs˜Ö9Œ`V_ıì#?%j §7ZnB©q„l£Âè± DĞß,Í‰/ÌÁ=Ã˜Mo…néqŸÕL	Ø*ôv«:Q)ÿñúıÂkªE#>;Ú|Ì% ñzÜèÎ—h@U=UZ»oÖ‘Æì´­ºêwé*sÓòÚ1ÌÀæ_óßši&Aæn²4Œu~‚ú[u‹œbıË¥Âa!2å»X!?q'Å>Š·’%Úi{ˆ!hşÅÈÏNµ…´û´!û,4ÔŞğQ&Fà7&L*ûÛ½3@<ªi}ä–[§Ñkİxiş[L’nQ,d"éÛ³Qÿµ§’hfÒá6ô|ÅV1¶›p˜ĞœÚñaHNó8c•E‘óœ ‰ÉVB_áÀ=á³†`êoç…‰Şz´æu³L`–¼&Ÿ[m°D6b—ßƒ¤XJ!o¹¯ØpËZõÒ†³íÜgNÇ­xæWå¿ÃOüió™nXägLòGœa¦µI¼/ÂËÆ™Î>vè7È`î
)Š]šµ Ç{Pëv‰³½çû0JDÄ÷‹Hî„ßÃ=„Ä“â™ˆüéšİr{(áozS‚Jiöu®”¸G‚¾âÂ’Ÿk—èµÍT¾k«†°Z¨…w°ËuC¿©§ç®‰õ¦Ôè%¶= G‘D×œ¥)áAÒĞa,¦¦\è™Šdñ™ÒXÙU¾Š*kİjvßX>ï/!ğ Y}¶’Êls‹¶òEÖ.˜z-è;¯±Q[ãh†ÇQgáQ’7â{\C­“ËÓnûwR
é$-çy¢Û®8Y³í^5~#£ó.æ³‚îğ1“lŞÓ213K¾:KM¶¸İ…5ÖÜõCN§ú=ËÏTÍ’pÚ`Ôdb”Ô|.ÖJÂ°Eç<^…?]´IÏ?ÔúŸØl›ò9†.kĞ+Aíóö¦ÖkJİ²}aŞŠ‚ùæ	Q’±1{d–İWÆ^+šo‹H¿ÖÑÆ…Œàe¥>½Ø+2$§ÔîçW·¡Ú±y—¨ù lÂÒ†‹|½«][Eµw>Ç¯A}Ò`ã­p°av°ğ%^¨.Ø-µùAQ±u‰»eZû2}2$6!å,UÉ…íMÎ¢†íähí¡Ää€Ó3ã”€ç‚lµ«.1Q•g< ãŒ
bPMã/-SÚ‚CÜßÊ{J;{–Gq>e›Âx#^÷)fm!BV,ËÙ¥ªÅçxh'İ$@- ®YŠ¶¿…K8{ˆØi4[¹-î4›û˜"nı­òEã—lyx¬ìòtô)´Ñá=ºŠÚØ—'PørÀ^ŒjLØ‡¿‰m€®7çRË;ã™vŒ(ç-^|4is–¨Ïé@¾´E¾7×.÷¼lÕ¿dV´BßçT²İ¥ná_ÃvZ³ßc(+š)±:Äá÷OX6rÊéXÖ{¸³‹sÍWî~Ñme%±od–qÔqëÕâ¨~²F·x¿§*\’Ô!ÇÜ!ëÉBC§TI’% = ğ ¡GÀáN¹­&d+gP~K]¥xqåto©ì"ñ	kÅÀãjsò3¸ˆÜ¡"û°ÃeıPGãPQí²¸á!³çó³#®%§ğ[ø/+5mº¦Ñ`Ş=ÒsS¥Ú<)ÔTá”a
Èñ¦¯&Ömås>Pù‰:Q Ú¡Òİ2& ¨”JôK¨©oüÈ{5ò&ŠlEÜ8:„ÈMögõènO@‰û5Fo!öàŞbß3aÿi]äş÷aß¥¶IY9Cˆ15¸Ü}	<¸»pƒ#o'dq«˜@7,¸’Ä1<¿ÍÌâ'æÅ?ß¸B˜ßÓ¡Kå«çzËH:Ç-b]oiB#gk/ı?0¢œË³Ëˆºûëïwo.Îõ&>@oÅ`åŠ¤;ğêã¥ç"üH+|®}–$®Ö‰ĞTZK¬vzhè“{šÚQ€½ú›®›HºØ«z€!wã~İŞó€½ ·ìlXìä-˜w¿<,çpu#1‘ó!}|˜î€Õß	Ş5ezqàm¶CSı™¿ùéé<"ˆ‰op±šL!ÁÚÀ1¿^1Tø¾RTeË¦¡Ò­î9*zˆò' Ñzº9ŸhÈëÛWì5aìßôWÛ¨]âC{Ò(¿c¹I~§TşÛécQŠÔ>{É”âëïXCóß5³±sâLVïß<„
q„aãú×l¹¨CäW‹Á¥?âèÀåÃşÊ)!Ë,Gàs×]~ÄbŸ“3Œ.¬ßiK.BŒ¯²³lÿE6åpd!{ÜÉ:Ñ0©Ù<ı[Eµ2 _]?a&Ãœƒ#'³O¢}¾ã •u|lÔºÄô¯U:oDØš,D7ŒÇvÛ}"¾ô.¾Ä]õ// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kmod.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/div64.h>
#include <linux/firmware.h>

#include "cimax2.h"
#include "altera-ci.h"
#include "cx23888-ir.h"
#include "cx23885-ir.h"
#include "cx23885-av.h"
#include "cx23885-input.h"

MODULE_DESCRIPTION("Driver for cx23885 based TV cards");
MODULE_AUTHOR("Steven Toth <stoth@linuxtv.org>");
MODULE_LICENSE("GPL");
MODULE_VERSION(CX23885_VERSION);

/*
 * Some platforms have been found to require periodic resetting of the DMA
 * engine. Ryzen and XEON platforms are known to be affected. The symptom
 * encountered is "mpeg risc op code error". Only Ryzen platforms employ
 * this workaround if the option equals 1. The workaround can be explicitly
 * disabled for all platforms by setting to 0, the workaround can be forced
 * on for any platform by setting to 2.
 */
static unsigned int dma_reset_workaround = 1;
module_param(dma_reset_workaround, int, 0644);
MODULE_PARM_DESC(dma_reset_workaround, "periodic RiSC dma engine reset; 0-force disable, 1-driver detect (default), 2-force enable");

static unsigned int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "enable debug messages");

static unsigned int card[]  = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };
module_param_array(card,  int, NULL, 0444);
MODULE_PARM_DESC(card, "card type");

#define dprintk(level, fmt, arg...)\
	do { if (debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: " fmt), \
		       __func__, ##arg); \
	} while (0)

static unsigned int cx23885_devcount;

#define NO_SYNC_LINE (-1U)

/* FIXME, these allocations will change when
 * analog arrives. The be reviewed.
 * CX23887 Assumptions
 * 1 line = 16 bytes of CDT
 * cmds size = 80
 * cdt size = 16 * linesize
 * iqsize = 64
 * maxlines = 6
 *
 * Address Space:
 * 0x00000000 0x00008fff FIFO clusters
 * 0x00010000 0x000104af Channel Management Data Structures
 * 0x000104b0 0x000104ff Free
 * 0x00010500 0x000108bf 15 channels * iqsize
 * 0x000108c0 0x000108ff Free
 * 0x00010900 0x00010e9f IQ's + Cluster Descriptor Tables
 *                       15 channels * (iqsize + (maxlines * linesize))
 * 0x00010ea0 0x00010xxx Free
 */

static struct sram_channel cx23885_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x10380,
		.cdt		= 0x104c0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "ch2",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10400,
		.cdt		= 0x10580,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH06] = {
		.name		= "TS2 C",
		.cmds_start	= 0x10140,
		.ctrl_start	= 0x10440,
		.cdt		= 0x105e0,
		.fifo_start	= 0x6000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH07] = {
		.name		= "TV Audio",
		.cmds_start	= 0x10190,
		.ctrl_start	= 0x10480,
		.cdt		= 0x10a00,
		.fifo_start	= 0x7000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA6_PTR1,
		.ptr2_reg	= DMA6_PTR2,
		.cnt1_reg	= DMA6_CNT1,
		.cnt2_reg	= DMA6_CNT2,
	},
	[SRAM_CH08] = {
		.name		= "ch8",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA7_PTR1,
		.ptr2_reg	= DMA7_PTR2,
		.cnt1_reg	= DMA7_CNT1,
		.cnt2_reg	= DMA7_CNT2,
	},
	[SRAM_CH09] = {
		.name		= "ch9",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA8_PTR1,
		.ptr2_reg	= DMA8_PTR2,
		.cnt1_reg	= DMA8_CNT1,
		.cnt2_reg	= DMA8_CNT2,
	},
};

static struct sram_channel cx23887_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x105b0,
		.cdt		= 0x107b0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "VID A (VBI)",
		.cmds_start	= 0x10050,
		.ctrl_start	= 0x105F0,
		.cdt		= 0x10810,
		.fifo_start	= 0x3000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10630,
		.cdt		= 0x10870,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH06] = {
		.name		= "TS2 C",
		.cmds_start	= 0x10140,
		.ctrl_start	= 0x10670,
		.cdt		= 0x108d0,
		.fifo_start	= 0x6000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH07] = {
		.name		= "TV Audio",
		.cmds_start	= 0x10190,
		.ctrl_start	= 0x106B0,
		.cdt		= 0x10930,
		.fifo_start	= 0x7000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA6_PTR1,
		.ptr2_reg	= DMA6_PTR2,
		.cnt1_reg	= DMA6_CNT1,
		.cnt2_reg	= DMA6_CNT2,
	},
	[SRAM_CH08] = {
		.name		= "ch8",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA7_PTR1,
		.ptr2_reg	= DMA7_PTR2,
		.cnt1_reg	= DMA7_CNT1,
		.cnt2_reg	= DMA7_CNT2,
	},
	[SRAM_CH09] = {
		.name		= "ch9",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA8_PTR1,
		.ptr2_reg	= DMA8_PTR2,
		.cnt1_reg	= DMA8_CNT1,
		.cnt2_reg	= DMA8_CNT2,
	},
};

static void cx23885_irq_add(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask |= mask;

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask |= mask;
	cx_set(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask)
{
	u32 v;
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	v = mask & dev->pci_irqmask;
	if (v)
		cx_set(PCI_INT_MSK, v);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static inline void cx23885_irq_enable_all(struct cx23885_dev *dev)
{
	cx23885_irq_enable(dev, 0xffffffff);
}

void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	cx_clear(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static inline void cx23885_irq_disable_all(struct cx23885_dev *dev)
{
	cx23885_irq_disable(dev, 0xffffffff);
}

void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask &= ~mask;
	cx_clear(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static u32 cx23885_irq_get_mask(struct cx23885_dev *dev)
{
	u32 v;
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	v = cx_read(PCI_INT_MSK);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
	return v;
}

static int cx23885_risc_decode(u32 risc)
{
	static char *instr[16] = {
		[RISC_SYNC    >> 28] = "sync",
		[RISC_WRITE   >> 28] = "write",
		[RISC_WRITEC  >> 28] = "writec",
		[RISC_READ    >> 28] = "read",
		[RISC_READC   >> 28] = "readc",
		[RISC_JUMP    >> 28] = "jump",
		[RISC_SKIP    >> 28] = "skip",
		[RISC_WRITERM >> 28] = "writerm",
		[RISC_WRITECM >> 28] = "writecm",
		[RISC_WRITECR >> 28] = "writecr",
	};
	static int incr[16] = {
		[RISC_WRITE   >> 28] = 3,
		[RISC_JUMP    >> 28] = 3,
		[RISC_SKIP    >> 28] = 1,
		[RISC_SYNC    >> 28] = 1,
		[RISC_WRITERM >> 28] = 3,
		[RISC_WRITECM >> 28] = 3,
		[RISC_WRITECR >> 28] = 4,
	};
	static char *bits[] = {
		"12",   "13",   "14",   "resync",
		"cnt0", "cnt1", "18",   "19",
		"20",   "21",   "22",   "23",
		"irq1", "irq2", "eol",  "sol",
	};
	int i;

	printk(KERN_DEBUG "0x%08x [ %s", risc,
	       instr[risc >> 28] ? instr[risc >> 28] : "INVALID");
	for (i = ARRAY_SIZE(bits) - 1; i >= 0; i--)
		if (risc & (1 << (i + 12)))
			pr_cont(" %s", bits[i]);
	pr_cont(" count=%d ]\n", risc & 0xfff);
	return incr[risc >> 28] ? incr[risc >> 28] : 1;
}

static void cx23885_wakeup(struct cx23885_tsport *port,
			   struct cx23885_dmaqueue *q, u32 count)
{
	struct cx23885_buffer *buf;
	int count_delta;
	int max_buf_done = 5; /* service maximum five buffers */

	do {
		if (list_empty(&q->active))
			return;
		buf = list_entry(q->active.next,
				 struct cx23885_buffer, queue);

		buf->vb.vb2_buf.timestamp = ktime_get_ns();
		buf->vb.sequence = q->count++;
		if (count != (q->count % 65536)) {
			dprintk(1, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
				buf->vb.vb2_buf.index, count, q->count);
		} else {
			dprintk(7, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
				buf->vb.vb2_buf.index, count, q->count);
		}
		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
		max_buf_done--;
		/* count register is 16 bits so apply modulo appropriately */
		count_delta = ((int)count - (int)(q->count % 65536));
	} while ((count_delta > 0) && (max_buf_done > 0));
}

int cx23885_sram_channel_setup(struct cx23885_dev *dev,
				      struct sram_channel *ch,
				      unsigned int bpl, u32 risc)
{
	unsigned int i, lines;
	u32 cdt;

	if (ch->cmds_start == 0) {
		dprintk(1, "%s() Erasing channel [%s]\n", __func__,
			ch->name);
		cx_write(ch->ptr1_reg, 0);
		cx_write(ch->ptr2_reg, 0);
		cx_write(ch->cnt2_reg, 0);
		cx_write(ch->cnt1_reg, 0);
		return 0;
	} else {
		dprintk(1, "%s() Configuring channel [%s]\n", __func__,
			ch->name);
	}

	bpl   = (bpl + 7) & ~7; /* alignment */
	cdt   = ch->cdt;
	lines = ch->fifo_size / bpl;
	if (lines > 6)
		lines = 6;
	BUG_ON(lines < 2);

	cx_write(8 + 0, RISC_JUMP | RISC_CNT_RESET);
	cx_write(8 + 4, 12);
	cx_write(8 + 8, 0);

	/* write CDT */
	for (i = 0; i < lines; i++) {
		dprintk(2, "%s() 0x%08x <- 0x%08x\n", __func__, cdt + 16*i,
			ch->fifo_start + bpl*i);
		cx_write(cdt + 16*i, ch->fifo_start + bpl*i);
		cx_write(cdt + 16*i +  4, 0);
		cx_write(cdt + 16*i +  8, 0);
		cx_write(cdt + 16*i + 12, 0);
	}

	/* write CMDS */
	if (ch->jumponly)
		cx_write(ch->cmds_start + 0, 8);
	else
		cx_write(ch->cmds_start + 0, risc);
	cx_write(ch->cmds_start +  4, 0); /* 64 bits 63-32 */
	cx_write(ch->cmds_start +  8, cdt);
	cx_write(ch->cmds_start + 12, (lines*16) >> 3);
	cx_write(ch->cmds_start + 16, ch->ctrl_start);
	if (ch->jumponly)
		cx_write(ch->cmds_start + 20, 0x80000000 | (64 >> 2));
	else
		cx_write(ch->cmds_start + 20, 64 >> 2);
	for (i = 24; i < 80; i += 4)
		cx_write(ch->cmds_start + i, 0);

	/* fill registers */
	cx_write(ch->ptr1_reg, ch->fifo_start);
	cx_write(ch->ptr2_reg, cdt);
	cx_write(ch->cnt2_reg, (lines*16) >> 3);
	cx_write(ch->cnt1_reg, (bpl >> 3) - 1);

	dprintk(2, "[bridge %d] sram setup %s: bpl=%d lines=%d\n",
		dev->bridge,
		ch->name,
		bpl,
		lines);

	return 0;
}

void cx23885_sram_channel_dump(struct cx23885_dev *dev,
				      struct sram_channel *ch)
{
	static char *name[] = {
		"init risc lo",
		"init risc hi",
		"cdt base",
		"cdt size",
		"iq base",
		"iq size",
		"risc pc lo",
		"risc pc hi",
		"iq wr ptr",
		"iq rd ptr",
		"cdt current",
		"pci target lo",
		"pci target hi",
		"line / byte",
	};
	u32 risc;
	unsigned int i, j, n;

	pr_warn("%s: %s - dma channel status dump\n",
		dev->name, ch->name);
	for (i = 0; i < ARRAY_SIZE(name); i++)
		pr_warn("%s:   cmds: %-15s: 0x%08x\n",
			dev->name, name[i],
			cx_read(ch->cmds_start + 4*i));

	for (i = 0; i < 4; i++) {
		risc = cx_read(ch->cmds_start + 4 * (i + 14));
		pr_warn("%s:   risc%d: ", dev->name, i);
		cx23885_risc_decode(risc);
	}
	for (i = 0; i < (64 >> 2); i += n) {
		risc = cx_read(ch->ctrl_start + 4 * i);
		/* No consideration for bits 63-32 */

		pr_warn("%s:   (0x%08x) iq %x: ", dev->name,
			ch->ctrl_start + 4 * i, i);
		n = cx23885_risc_decode(risc);
		for (j = 1; j < n; j++) {
			risc = cx_read(ch->ctrl_start + 4 * (i + j));
			pr_warn("%s:   iq %x: 0x%08x [ arg #%d ]\n",
				dev->name, i+j, risc, j);
		}
	}

	pr_warn("%s: fifo: 0x%08x -> 0x%x\n",
		dev->name, ch->fifo_start, ch->fifo_start+ch->fifo_size);
	pr_warn("%s: ctrl: 0x%08x -> 0x%x\n",
		dev->name, ch->ctrl_start, ch->ctrl_start + 6*16);
	pr_warn("%s:   ptr1_reg: 0x%08x\n",
		dev->name, cx_read(ch->ptr1_reg));
	pr_warn("%s:   ptr2_reg: 0x%08x\n",
		dev->name, cx_read(ch->ptr2_reg));
	pr_warn("%s:   cnt1_reg: 0x%08x\n",
		dev->name, cx_read(ch->cnt1_reg));
	pr_warn("%s:   cnt2_reg: 0x%08x\n",
		dev->name, cx_read(ch->cnt2_reg));
}

static void cx23885_risc_disasm(struct cx23885_tsport *port,
				struct cx23885_riscmem *risc)
{
	struct cx23885_dev *dev = port->dev;
	unsigned int i, j, n;

	pr_info("%s: risc disasm: %p [dma=0x%08lx]\n",
	       dev->name, risc->cpu, (unsigned long)risc->dma);
	for (i = 0; i < (risc->size >> 2); i += n) {
		pr_info("%s:   %04d: ", dev->name, i);
		n = cx23885_risc_decode(le32_to_cpu(risc->cpu[i]));
		for (j = 1; j < n; j++)
			pr_info("%s:   %04d: 0x%08x [ arg #%d ]\n",
				dev->name, i + j, risc->cpu[i + j], j);
		if (risc->cpu[i] == cpu_to_le32(RISC_JUMP))
			break;
	}
}

static void cx23885_clear_bridge_error(struct cx23885_dev *dev)
{
	uint32_t reg1_val, reg2_val;

	if (!dev->need_dma_reset)
		return;

	reg1_val = cx_read(TC_REQ); /* read-only */
	reg2_val = cx_read(TC_REQ_SET);

	if (reg1_val && reg2_val) {
		cx_write(TC_REQ, reg1_val);
		cx_write(TC_REQ_SET, reg2_val);
		cx_read(VID_B_DMA);
		cx_read(VBI_B_DMA);
		cx_read(VID_C_DMA);
		cx_read(VBI_C_DMA);

		dev_info(&dev->pci->dev,
			"dma in progress detected 0x%08x 0x%08x, clearing\n",
			reg1_val, reg2_val);
	}
}

static void cx23885_shutdown(struct cx23885_dev *dev)
{
	/* disable RISC controller */
	cx_write(DEV_CNTRL2, 0);

	/* Disable all IR activity */
	cx_write(IR_CNTRL_REG, 0);

	/* Disable Video A/B activity */
	cx_write(VID_A_DMA_CTL, 0);
	cx_write(VID_B_DMA_CTL, 0);
	cx_write(VID_C_DMA_CTL, 0);

	/* Disable Audio activity */
	cx_write(AUD_INT_DMA_CTL, 0);
	cx_write(AUD_EXT_DMA_CTL, 0);

	/* Disable Serial port */
	cx_write(UART_CTL, 0);

	/* Disable Interrupts */
	cx23885_irq_disable_all(dev);
	cx_write(VID_A_INT_MSK, 0);
	cx_write(VID_B_INT_MSK, 0);
	cx_write(VID_C_INT_MSK, 0);
	cx_write(AUDIO_INT_INT_MSK, 0);
	cx_write(AUDIO_EXT_INT_MSK, 0);

}

static void cx23885_reset(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	cx23885_shutdown(dev);

	cx_write(PCI_INT_STAT, 0xffffffff);
	cx_write(VID_A_INT_STAT, 0xffffffff);
	cx_write(VID_B_INT_STAT, 0xffffffff);
	cx_write(VID_C_INT_STAT, 0xffffffff);
	cx_write(AUDIO_INT_INT_STAT, 0xffffffff);
	cx_write(AUDIO_EXT_INT_STAT, 0xffffffff);
	cx_write(CLK_DELAY, cx_read(CLK_DELAY) & 0x80000000);
	cx_write(PAD_CTRL, 0x00500300);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
	msleep(100);

	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH01],
		720*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH03],
		188*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH04], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH05], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH06],
		188*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH07], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH08], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH09], 128, 0);

	cx23885_gpio_setup(dev);

	cx23885_irq_get_mask(dev);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
}


static int cx23885_pci_quirks(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	/* The cx23885 bridge has a weird bug which causes NMI to be asserted
	 * when DMA begins if RDR_TLCTL0 bit4 is not cleared. It does not
	 * occur on the cx23887 bridge.
	 */
	if (dev->bridge == CX23885_BRIDGE_885)
		cx_clear(RDR_TLCTL0, 1 << 4);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
	return 0;
}

static int get_resources(struct cx23885_dev *dev)
{
	if (request_mem_region(pci_resource_start(dev->pci, 0),
			       pci_resource_len(dev->pci, 0),
			       dev->name))
		return 0;

	pr_err("%s: can't get MMIO memory @ 0x%llx\n",
	       dev->name, (unsigned long long)pci_resource_start(dev->pci, 0));

	return -EBUSY;
}

static int cx23885_init_tsport(struct cx23885_dev *dev,
	struct cx23885_tsport *port, int portno)
{
	dprintk(1, "%s(portno=%d)\n", __func__, portno);

	/* Transport bus init dma queue  - Common settings */
	port->dma_ctl_val        = 0x11; /* Enable RISC controller and Fifo */
	port->ts_int_msk_val     = 0x1111; /* TS port bits for RISC */
	port->vld_misc_val       = 0x0;
	port->hw_sop_ctrl_val    = (0x47 << 16 | 188 << 4);

	spin_lock_init(&port->slock);
	port->dev = dev;
	port->nr = portno;

	INIT_LIST_HEAD(&port->mpegq.active);
	mutex_init(&port->frontends.lock);
	INIT_LIST_HEAD(&port->frontends.felist);
	port->frontends.active_fe_id = 0;

	/* This should be hardcoded allow a single frontend
	 * attachment to this tsport, keeping the -dvb.c
	 * code clean and safe.
	 */
	if (!port->num_frontends)
		port->num_frontends = 1;

	switch (portno) {
	case 1:
		port->reg_gpcnt          = VID_B_GPCNT;
		port->reg_gpcnt_ctl      = VID_B_GPCNT_CTL;
		port->reg_dma_ctl        = VID_B_DMA_CTL;
		port->reg_lngth          = VID_B_LNGTH;
		port->reg_hw_sop_ctrl    = VID_B_HW_SOP_CTL;
		port->reg_gen_ctrl       = VID_B_GEN_CTL;
		port->reg_bd_pkt_status  = VID_B_BD_PKT_STATUS;
		port->reg_sop_status     = VID_B_SOP_STATUS;
		port->reg_fifo_ovfl_stat = VID_B_FIFO_OVFL_STAT;
		port->reg_vld_misc       = VID_B_VLD_MISC;
		port->reg_ts_clk_en      = VID_B_TS_CLK_EN;
		port->reg_src_sel        = VID_B_SRC_SEL;
		port->reg_ts_int_msk     = VID_B_INT_MSK;
		port->reg_ts_int_stat    = VID_B_INT_STAT;
		port->sram_chno          = SRAM_CH03; /* VID_B */
		port->pci_irqmask        = 0x02; /* VID_B bit1 */
		break;
	case 2:
		port->reg_gpcnt          = VID_C_GPCNT;
		port->reg_gpcnt_ctl      = VID_C_GPCNT_CTL;
		port->reg_dma_ctl        = VID_C_DMA_CTL;
		port->reg_lngth          = VID_C_LNGTH;
		port->reg_hw_sop_ctrl    = VID_C_HW_SOP_CTL;
		port->reg_gen_ctrl       = VID_C_GEN_CTL;
		port->reg_bd_pkt_status  = VID_C_BD_PKT_STATUS;
		port->reg_sop_status     = VID_C_SOP_STATUS;
		port->reg_fifo_ovfl_stat = VID_C_FIFO_OVFL_STAT;
		port->reg_vld_misc       = VID_C_VLD_MISC;
		port->reg_ts_clk_en      = VID_C_TS_CLK_EN;
		port->reg_src_sel        = 0;
		port->reg_ts_int_msk     = VID_C_INT_MSK;
		port->reg_ts_int_stat    = VID_C_INT_STAT;
		port->sram_chno          = SRAM_CH06; /* VID_C */
		port->pci_irqmask        = 0x04; /* VID_C bit2 */
		break;
	default:
		BUG();
	}

	return 0;
}

static void cx23885_dev_checkrevision(struct cx23885_dev *dev)
{
	switch (cx_read(RDR_CFG2) & 0xff) {
	case 0x00:
		/* cx23885 */
		dev->hwrevision = 0xa0;
		break;
	case 0x01:
		/* CX23885-12Z */
		dev->hwrevision = 0xa1;
		break;
	case 0x02:
		/* CX23885-13Z/14Z */
		dev->hwrevision = 0xb0;
		break;
	case 0x03:
		if (dev->pci->device == 0x8880) {
			/* CX23888-21Z/22Z */
			dev->hwrevision = 0xc0;
		} else {
			/* CX23885-14Z */
			dev->hwrevision = 0xa4;
		}
		break;
	case 0x04:
		if (dev->pci->device == 0x8880) {
			/* CX23888-31Z */
			dev->hwrevision = 0xd0;
		} else {
			/* CX23885-15Z, CX23888-31Z */
			dev->hwrevision = 0xa5;
		}
		break;
	case 0x0e:
		/* CX23887-15Z */
		dev->hwrevision = 0xc0;
		break;
	case 0x0f:
		/* CX23887-14Z */
		dev->hwrevision = 0xb1;
		break;
	default:
		pr_err("%s() New hardware revision found 0x%x\n",
		       __func__, dev->hwrevision);
	}
	if (dev->hwrevision)
		pr_info("%s() Hardware revision = 0x%02x\n",
			__func__, dev->hwrevision);
	else
		pr_err("%s() Hardware revision unknown 0x%x\n",
		       __func__, dev->hwrevision);
}

/* Find the first v4l2_subdev member of the group id in hw */
struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw)
{
	struct v4l2_subdev *result = NULL;
	struct v4l2_subdev *sd;

	spin_lock(&dev->v4l2_dev.lock);
	v4l2_device_for_each_subdev(sd, &dev->v4l2_dev) {
		if (sd->grp_id == hw) {
			result = sd;
			break;
		}
	}
	spin_unlock(&dev->v4l2_dev.lock);
	return result;
}

static int cx23885_dev_setup(struct cx23885_dev *dev)
{
	int i;

	spin_lock_init(&dev->pci_irqmask_lock);
	spin_lock_init(&dev->slock);

	mutex_init(&dev->lock);
	mutex_init(&dev->gpio_lock);

	atomic_inc(&dev->refcount);

	dev->nr = cx23885_devcount++;
	sprintf(dev->name, "cx23885[%d]", dev->nr);

	/* Configure the internal memory */
	if (dev->pci->device == 0x8880) {
		/* Could be 887 or 888, assume an 888 default */
		dev->bridge = CX23885_BRIDGE_888;
		/* Apply a sensible clock frequency for the PCIe bridge */
		dev->clk_freq = 50000000;
		dev->sram_channels = cx23887_sram_channels;
	} else
	if (dev->pci->device == 0x8852) {
		dev->bridge = CX23885_BRIDGE_885;
		/* Apply a sensible clock frequency for the PCIe bridge */
		dev->clk_freq = 28000000;
		dev->sram_channels = cx23885_sram_channels;
	} else
		BUG();

	dprintk(1, "%s() Memory configured for PCIe bridge type %d\n",
		__func__, dev->bridge);

	/* board config */
	dev->board = UNSET;
	if (card[dev->nr] < cx23885_bcount)
		dev->board = card[dev->nr];
	for (i = 0; UNSET == dev->board  &&  i < cx23885_idcount; i++)
		if (dev->pci->subsystem_vendor == cx23885_subids[i].subvendor &&
		    dev->pci->subsystem_device == cx23885_subids[i].subdevice)
			dev->board = cx23885_subids[i].card;
	if (UNSET == dev->board) {
		dev->board = CX23885_BOARD_UNKNOWN;
		cx23885_card_list(dev);
	}

	if (dev->pci->device == 0x8852) {
		/* no DIF on cx23885, so no analog tuner support possible */
		if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC)
			dev->board = CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885;
		else if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB)
			dev->board = CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885;
	}

	/* If the user specific a clk freq override, apply it */
	if (cx23885_boards[dev->board].clk_freq > 0)
		dev->clk_freq = cx23885_boards[dev->board].clk_freq;

	if (dev->board == CX23885_BOARD_HAUPPAUGE_IMPACTVCBE &&
		dev->pci->subsystem_device == 0x7137) {
		/* Hauppauge ImpactVCBe device ID 0x7137 is populated
		 * with an 888, and a 25Mhz crystal, instead of the
		 * usual third overtone 50Mhz. The default clock rate must
		 * be overridden so the cx25840 is properly configured
		 */
		dev->clk_freq = 25000000;
	}

	dev->pci_bus  = dev->pci->bus->number;
	dev->pci_slot = PCI_SLOT(dev->pci->devfn);
	cx23885_irq_add(dev, 0x001f00);

	/* External Master 1 Bus */
	dev->i2c_bus[0].nr = 0;
	dev->i2c_bus[0].dev = dev;
	dev->i2c_bus[0].reg_stat  = I2C1_STAT;
	dev->i2c_bus[0].reg_ctrl  = I2C1_CTRL;
	dev->i2c_bus[0].reg_addr  = I2C1_ADDR;
	dev->i2c_bus[0].reg_rdata = I2C1_RDATA;
	dev->i2c_bus[0].reg_wdata = I2C1_WDATA;
	dev->i2c_bus[0].i2c_period = (0x9d << 24); /* 100kHz */

	/* External Master 2 Bus */
	dev->i2c_bus[1].nr = 1;
	dev->i2c_bus[1].dev = dev;
	dev->i2c_bus[1].reg_stat  = I2C2_STAT;
	dev->i2c_bus[1].reg_ctrl  = I2C2_CTRL;
	dev->i2c_bus[1].reg_addr  = I2C2_ADDR;
	dev->i2c_bus[1].reg_rdata = I2C2_RDATA;
	dev->i2c_bus[1].reg_wdata = I2C2_WDATA;
	dev->i2c_bus[1].i2c_period = (0x9d << 24); /* 100kHz */

	/* Internal Master 3 Bus */
	dev->i2c_bus[2].nr = 2;
	dev->i2c_bus[2].dev = dev;
	dev->i2c_bus[2].reg_stat  = I2C3_STAT;
	dev->i2c_bus[2].reg_ctrl  = I2C3_CTRL;
	dev->i2c_bus[2].reg_addr  = I2C3_ADDR;
	dev->i2c_bus[2].reg_rdata = I2C3_RDATA;
	dev->i2c_bus[2].reg_wdata = I2C3_WDATA;
	dev->i2c_bus[2].i2c_period = (0x07 << 24); /* 1.95MHz */

	if ((cx23885_boards[dev->board].portb == CX23885_MPEG_DVB) ||
		(cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER))
		cx23885_init_tsport(dev, &dev->ts1, 1);

	if ((cx23885_boards[dev->board].portc == CX23885_MPEG_DVB) ||
		(cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER))
		cx23885_init_tsport(dev, &dev->ts2, 2);

	if (get_resources(dev) < 0) {
		pr_err("CORE %s No more PCIe resources for subsystem: %04x:%04x\n",
		       dev->name, dev->pci->subsystem_vendor,
		       dev->pci->subsystem_device);

		cx23885_devcount--;
		return -ENODEV;
	}

	/* PCIe stuff */
	dev->lmmio = ioremap(pci_resource_start(dev->pci, 0),
			     pci_resource_len(dev->pci, 0));

	dev->bmmio = (u8 __iomem *)dev->lmmio;

	pr_info("CORE %s: subsystem: %04x:%04x, board: %s [card=%d,%s]\n",
		dev->name, dev->pci->subsystem_vendor,
		dev->pci->subsystem_device, cx23885_boards[dev->board].name,
		dev->board, card[dev->nr] == dev->board ?
		"insmod option" : "autodetected");

	cx23885_pci_quirks(dev);

	/* Assume some sensible defaults */
	dev->tuner_type = cx23885_boards[dev->board].tuner_type;
	dev->tuner_addr = cx23885_boards[dev->board].tuner_addr;
	dev->tuner_bus = cx23885_boards[dev->board].tuner_bus;
	dev->radio_type = cx23885_boards[dev->board].radio_type;
	dev->radio_addr = cx23885_boards[dev->board].radio_addr;

	dprintk(1, "%s() tuner_type = 0x%x tuner_addr = 0x%x tuner_bus = %d\n",
		__func__, dev->tuner_type, dev->tuner_addr, dev->tuner_bus);
	dprintk(1, "%s() radio_type = 0x%x radio_addr = 0x%x\n",
		__func__, dev->radio_type, dev->radio_addr);

	/* The cx23417 encoder has GPIO's that need to be initialised
	 * before DVB, so that demodulators and tuners are out of
	 * reset before DVB uses them.
	 */
	if ((cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) ||
		(cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER))
			cx23885_mc417_init(dev);

	/* init hardware */
	cx23885_reset(dev);

	cx23885_i2c_register(&dev->i2c_bus[0]);
	cx23885_i2c_register(&dev->i2c_bus[1]);
	cx23885_i2c_register(&dev->i2c_bus[2]);
	cx23885_card_setup(dev);
	c