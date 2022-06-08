r_completion(&pending->done);

	if (refcount_dec_and_test(&pending->refs))
		wake_up_var(&pending->refs); /* No UaF, just an address */

	/*
	 * Block the original owner of &pending until all subsequent callers
	 * have seen the completion and decremented the refcount
	 */
	wait_var_event(&my_pending.refs, !refcount_read(&my_pending.refs));

	/* ARGH */
	WARN_ON_ONCE(my_pending.stop_pending);

	return 0;
}

/*
 * Called with both p->pi_lock and rq->lock held; drops both before returning.
 */
static int __set_cpus_allowed_ptr_locked(struct task_struct *p,
					 const struct cpumask *new_mask,
					 u32 flags,
					 struct rq *rq,
					 struct rq_flags *rf)
	__releases(rq->lock)
	__releases(p->pi_lock)
{
	const struct cpumask *cpu_allowed_mask = task_cpu_possible_mask(p);
	const struct cpumask *cpu_valid_mask = cpu_active_mask;
	bool kthread = p->flags & PF_KTHREAD;
	struct cpumask *user_mask = NULL;
	unsigned int dest_cpu;
	int ret = 0;

	update_rq_clock(rq);

	if (kthread || is_migration_disabled(p)) {
		/*
		 * Kernel threads are allowed on online && !active CPUs,
		 * however, during cpu-hot-unplug, even these might get pushed
		 * away if not KTHREAD_IS_PER_CPU.
		 *
		 * Specifically, migration_disabled() tasks must not fail the
		 * cpumask_any_and_distribute() pick below, esp. so on
		 * SCA_MIGRATE_ENABLE, otherwise we'll not call
		 * set_cpus_allowed_common() and actually reset p->cpus_ptr.
		 */
		cpu_valid_mask = cpu_online_mask;
	}

	if (!kthread && !cpumask_subset(new_mask, cpu_allowed_mask)) {
		ret = -EINVAL;
		goto out;
	}

	/*
	 * Must re-check here, to close a race against __kthread_bind(),
	 * sched_setaffinity() is not guaranteed to observe the flag.
	 */
	if ((flags & SCA_CHECK) && (p->flags & PF_NO_SETAFFINITY)) {
		ret = -EINVAL;
		goto out;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		if (cpumask_equal(&p->cpus_mask, new_mask))
			goto out;

		if (WARN_ON_ONCE(p == current &&
				 is_migration_disabled(p) &&
				 !cpumask_test_cpu(task_cpu(p), new_mask))) {
			ret = -EBUSY;
			goto out;
		}
	}

	/*
	 * Picking a ~random cpu helps in cases where we are changing affinity
	 * for groups of tasks (ie. cpuset), so that load balancing is not
	 * immediately required to distribute the tasks within their new mask.
	 */
	dest_cpu = cpumask_any_and_distribute(cpu_valid_mask, new_mask);
	if (dest_cpu >= nr_cpu_ids) {
		ret = -EINVAL;
		goto out;
	}

	__do_set_cpus_allowed(p, new_mask, flags);

	if (flags & SCA_USER)
		user_mask = clear_user_cpus_ptr(p);

	ret = affine_move_task(rq, p, rf, dest_cpu, flags);

	kfree(user_mask);

	return ret;

out:
	task_rq_unlock(rq, p, rf);

	return ret;
}

/*
 * Change a given task's CPU affinity. Migrate the thread to a
 * proper CPU and schedule it away if the CPU it's executing on
 * is removed from the allowed bitmask.
 *
 * NOTE: the caller must have a valid reference to the task, the
 * task must not exit() & deallocate itself prematurely. The
 * call is not atomic; no spinlocks may be held.
 */
static int __set_cpus_allowed_ptr(struct task_struct *p,
				  const struct cpumask *new_mask, u32 flags)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(p, &rf);
	return __set_cpus_allowed_ptr_locked(p, new_mask, flags, rq, &rf);
}

int set_cpus_allowed_ptr(struct task_struct *p, const struct cpumask *new_mask)
{
	return __set_cpus_allowed_ptr(p, new_mask, 0);
}
EXPORT_SYMBOL_GPL(set_cpus_allowed_ptr);

/*
 * Change a given task's CPU affinity to the intersection of its current
 * affinity mask and @subset_mask, writing the resulting mask to @new_mask
 * and pointing @p->user_cpus_ptr to a copy of the old mask.
 * If the resulting mask is empty, leave the affinity unchanged and return
 * -EINVAL.
 */
static int restrict_cpus_allowed_ptr(struct task_struct *p,
				     struct cpumask *new_mask,
				     const struct cpumask *subset_mask)
{
	struct cpumask *user_mask = NULL;
	struct rq_flags rf;
	struct rq *rq;
	int err;

	if (!p->user_cpus_ptr) {
		user_mask = kmalloc(cpumask_size(), GFP_KERNEL);
		if (!user_mask)
			return -ENOMEM;
	}

	rq = task_rq_lock(p, &rf);

	/*
	 * Forcefully restricting the affinity of a deadline task is
	 * likely to cause problems, so fail and noisily override the
	 * mask entirely.
	 */
	if (task_has_dl_policy(p) && dl_bandwidth_enabled()) {
		err = -EPERM;
		goto err_unlock;
	}

	if (!cpumask_and(new_mask, &p->cpus_mask, subset_mask)) {
		err = -EINVAL;
		goto err_unlock;
	}

	/*
	 * We're about to butcher the task affinity, so keep track of what
	 * the user asked for in case we're able to restore it later on.
	 */
	if (user_mask) {
		cpumask_copy(user_mask, p->cpus_ptr);
		p->user_cpus_ptr = user_mask;
	}

	return __set_cpus_allowed_ptr_locked(p, new_mask, 0, rq, &rf);

err_unlock:
	task_rq_unlock(rq, p, &rf);
	kfree(user_mask);
	return err;
}

/*
 * Restrict the CPU affinity of task @p so that it is a subset of
 * task_cpu_possible_mask() and point @p->user_cpu_ptr to a copy of the
 * old affinity mask. If the resulting mask is empty, we warn and walk
 * up the cpuset hierarchy until we find a suitable mask.
 */
void force_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	cpumask_var_t new_mask;
	const struct cpumask *override_mask = task_cpu_possible_mask(p);

	alloc_cpumask_var(&new_mask, GFP_KERNEL);

	/*
	 * __migrate_task() can fail silently in the face of concurrent
	 * offlining of the chosen destination CPU, so take the hotplug
	 * lock to ensure that the migration succeeds.
	 */
	cpus_read_lock();
	if (!cpumask_available(new_mask))
		goto out_set_mask;

	if (!restrict_cpus_allowed_ptr(p, new_mask, override_mask))
		goto out_free_mask;

	/*
	 * We failed to find a valid subset of the affinity mask for the
	 * task, so override it based on its cpuset hierarchy.
	 */
	cpuset_cpus_allowed(p, new_mask);
	override_mask = new_mask;

out_set_mask:
	if (printk_ratelimit()) {
		printk_deferred("Overriding affinity for process %d (%s) to CPUs %*pbl\n",
				task_pid_nr(p), p->comm,
				cpumask_pr_args(override_mask));
	}

	WARN_ON(set_cpus_allowed_ptr(p, override_mask));
out_free_mask:
	cpus_read_unlock();
	free_cpumask_var(new_mask);
}

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask);

/*
 * Restore the affinity of a task @p which was previously restricted by a
 * call to force_compatible_cpus_allowed_ptr(). This will clear (and free)
 * @p->user_cpus_ptr.
 *
 * It is the caller's responsibility to serialise this with any calls to
 * force_compatible_cpus_allowed_ptr(@p).
 */
void relax_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = p->user_cpus_ptr;
	unsigned long flags;

	/*
	 * Try to restore the old affinity mask. If this fails, then
	 * we free the mask explicitly to avoid it being inherited across
	 * a subsequent fork().
	 */
	if (!user_mask || !__sched_setaffinity(p, user_mask))
		return;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	user_mask = clear_user_cpus_ptr(p);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

	kfree(user_mask);
}

void set_task_cpu(struct task_struct *p, unsigned int new_cpu)
{
#ifdef CONFIG_SCHED_DEBUG
	unsigned int state = READ_ONCE(p->__state);

	/*
	 * We should never call set_task_cpu() on a blocked task,
	 * ttwu() will sort out the placement.
	 */
	WARN_ON_ONCE(state != TASK_RUNNING && state != TASK_WAKING && !p->on_rq);

	/*
	 * Migrating fair class task must have p->on_rq = TASK_ON_RQ_MIGRATING,
	 * because schedstat_wait_{start,end} rebase migrating task's wait_start
	 * time relying on p->on_rq.
	 */
	WARN_ON_ONCE(state == TASK_RUNNING &&
		     p->sched_class == &fair_sched_class &&
		     (p->on_rq && !task_on_rq_migrating(p)));

#ifdef CONFIG_LOCKDEP
	/*
	 * The caller should hold either p->pi_lock or rq->lock, when changing
	 * a task's CPU. ->pi_lock for waking tasks, rq->lock for runnable tasks.
	 *
	 * sched_move_task() holds both and thus holding either pins the cgroup,
	 * see task_group().
	 *
	 * Furthermore, all task_rq users should acquire both locks, see
	 * task_rq_lock().
	 */
	WARN_ON_ONCE(debug_locks && !(lockdep_is_held(&p->pi_lock) ||
				      lockdep_is_held(__rq_lockp(task_rq(p)))));
#endif
	/*
	 * Clearly, migrati«èæ¬Üî>ŒÐ´_ÒˆÖ4–ÏÅœgl¦T²Øœh\
žwùåý%&|²çí°ZÑYEÝ,òíÕQémê¸'%-KhÐª¥zPêªí7óŠ‡L[ãlQåš^{rÐ¹P^ëýé—,ÔéÞ¯{Ê±´¯Ø«¡‘x³hÏ¸gA%çˆ=>¼ßpûGÞßhkùülòû”U—Oï-˜ŠÜS%ôáJßT®F¹3ÒnÉÍüÊ®LÝdÕš`l²BÇ|jÿ×ßæ›ÔHvò˜ê/«^7ªÇÒv’v²¨¶s±€ÀÍAï†ÆåB;NŒ”#v¨Þ£‡tÿtÖêßu Gô–Q,YR)ß³½Þ%+¤—kÖ0dA})$áloÁ]ÌSlŸIÃ'+ Tdw’{‘VHèýýiÖ¢å§Ë‡°©džAõì83ÜXREhBü†¥;ø¡cbÖ.^Ú?|h0uðPÐÁå@¨“Îaã³Ò1]N45à´ˆFkïb_<u¤ 1Âƒ1‚HtÝ—.ü£oLÖÌæEÍï¤í°Ö Æ([œ¹èfÜ€¿ðƒ[Ž½]I4ô,J–8oc ÀMBcüö“/<ÿ—;Mo¥hw ÔÉ6àiÞ»J/¶ñ	íñÕx†ó„ {•'}éù{SÅ%µ¢9Ü¯'v–çtÄ* Iü“Å*Ë7åÈ5Þ){„Xšñõ¯i¶’Ç,qà½ðzý4È5æ«L…Ç»«Z*ÅÖðË]óÖÁŽ@jXFµ.Rø:ùãöŒ}¹gâ£Õ¢¼hØÓÌì22ÛTøNÉÁrLa›ôõq³%[ÈÏÌ@ôå½ùˆƒÀžsÉïó±äúæO WÓ2jÚl®Ü>òú
¤í†êñ|KäÌÿ¿¤©é)å†væ’'Eí\Ù#(Ø(ÿ9¥ÑWPmEuÔ"v‘,SõÜ5¢\æ£ÁBZð6Y¯ì”%-l’
z›^ÏYÓŽD¦…e¢Õ,S"¿ ÊuYÏ£a5½Úª]u|¸zŽþ	¯L±Íq^(©APô$ÇNýM±Ô¦‡¼XxHÌ*Ð?‹¦X/-€ƒ”õþ³³`Œ2&Á:h) l°D¿@Bî€¼‚k¯³ƒ2Á R[,.²ðîÇÝp”îèÑíA^›Æ%ÔÝ»ŒôEsÞ€ƒ¯á—T—Spß¾q¥\D}¡ñs—SAØÉ50Öke?Òqë<ÙóN0M¢NŒx­
¡,S~©*­”—ÓeOŠ‡™ÂÕøn’}¥Ê™ñÅ‹~w8û:´|Ë×›gÑ×%¦2½–8eÎkíëÖÑ/µ^‰Å i¢*zø~LSñ4\žWÅ™W¥UöP< c‘žwà™»¾­¥áNêŠ¯;ÍÚ½HLAû¡Ö‰þ"æ^V›’Šsßk¡KµW~Xµfœâ¡T«'>èª˜Vî‡Œ×oõ'<>`-¥²4XÔT;p`Òö§#+ÙaŒ$üo ºñ+®%4üSSî›à_qãÞò¾zù$.{§
.‹lQaÀS.,ÆŽ.Sq]i\ƒï®ƒþµOX=x°ˆÔð—>ýh™û~xô¬Èìq2b?p{w*ó¶A4œ0J,c}.2©.­MÄ\°A³³T2Æ ð±9nhžÎåîßz¨‘äÈrñRlÜÑ\Ñòã#ÿ²½ÀúÛ×JM9)šCïJø-ÞÑÍè# ”O2ÈH;¦OHŠ,Ð°o9Ëg!Ï»ª˜O_d’24ˆç4-u+±æUËæCqž:Uº5LdHÓ!·Ã	LÉÉj` 9ÄËøÛeÑÕ'ß¤Ën/ucU6Ã“RíoN~â
ºSøjÄ†Øø¢tˆÛµ9õ2›+^ê¥„Z§Au{é:¥böÚ»aPž÷ÄŠöÚëÉeÓ”Ða3²¢9—b†Tˆòú_7:f¯HŸ~é4„ÌËû+íaBÔxhÎ½«ƒS»òÐº¤LñPx»Xëb¡ÃÁöƒúç!þ%¡ý6SÍ–™€f®xÊ”x>6R½<±®s~–Ðµ”ýÆb<ue‚0öùVi4§Ò%‚=ÓÝ3þ1˜æ/¢§ÐtF¿ØaÞå`Ý­+Áõ†nŠg H2€G Cê+(“v®IóÈ5/À¸¥î¡Óë6gg‹YÚ=«w÷WE·Cìð`êí4s6¥Ú·Õð¶£ð‘òkrf”•ÈúÕ·á¶{p/ ˆ	S¿no°h¶û²Ñ¡†z³x&î*xÈæ£ÄùcgÒ›ÕG8¯è9#ùJ‡HöòÄf`cÌ°Þ¯á¶¾öÍ¢òG5¥%*5¦Øý=xÛÈ¶ªÔw/xz­\#‹éV–Ô†ß³QˆM¤n­oÂDûÔûÍ]xÁ°[%uL8‹mæ¢ìº÷¼§Š™¨#¬Dõ{é ÝOft­Õ«ŠDúqy‚g2Ì¥TðÙ/Ù¦1´Hä†–-ÆŠ…é/ÁþBÔ„¬Ó¥¤™¿F$¶XAéô»dJPÄ)åò¬ðø7îûb'ÅÏWU!Þ.•gÝ)}«ÏÓ½ìèÚ‘{¥µ?¥¦[A®Êþ]oE6ÕØ(ÞELv
“!ßn¼À÷„ýÃ”Ú{Éj çÖç}Â[ú¤‚¯>L3#£ëxÂÝaÏ‹ÖvtÑkª>8ç!¬’ah2½s÷Ø,°—ÛpœC_ñî»GÆŽ2>!:‡YO_û2ÈÅ.ÑÄù¸5w’PO±Zlc5Ë/Om®Ä~¾ÁaËxOú„ãå–NŒÝ[%Ac"I,Ü@Ü²¡…Æ¥W´¢Îë(Œ{l× H?@„B/zX+;N3úmwØ@=Ê²÷°$tý }-³’“Nžšï½†àHlPâ#Í†°^,!ëô”ƒ•×V‹N/ý–ÕYŒrË<ÍuÜÚú7G ¹u)rS#›†8ìS65gRŸÅWc_Tï2Ér”.Ø„/¬©^U}å2dÇ¦JÂ[óÞŽ—üÆH	>Ê¦L}Ï:•–±Ì8Õ†œ%€ë
E˜4j—É´zA¥+Ð™…&}U>Õ´~Ÿ3w×gÿÕÀoÎá}ãîè|ê8àc
Þœ8ÈNAè¯ñþ&jËµÃt}@›âŠMó0;úÉ‚§™(‰¹‹s§Õ	ªTˆ7]êš&6kIÄö¬	[R«(ò˜ÇÃ°‡&|sõZî±lVÈÏd,ê°e¤¬õž°ªvŽ ŒcI@û*ÆáøCG 9þúÓH7˜m”…ò!)Œ‰ÔüvoYåiÎ»_n…»_NChþÞ1c-Q"G+œ8Ñ4HgðpfVÊKµäŒèá³hÜ1•Ùl3ÜJ~YØ¸ûEÝéÊ7u#®uXã¨ÍíÕ¶W©mÑ¸e6ÅR_¤¤$­‘âíÒ%ºNv+Þ¼Š¿í+‘Ü/s1&*k™þ²—W2
$îïS©TmÑ!Ó¸<vI'2«ÒÊ[5–šÊºä6²YƒÂu™¤Qy,—7Ø*µD”é¥øGá¥·:î‹æ:5XC×ŸSÇx­Zdeð¿ð
¥aÛõ"73ÐW
ÿtØp
&m°ïçõÅ4RØ–>«e<§®‹µLýDøë„Þ”žÖ=Ù÷ƒð
Ãx$».jIxRUYT{BòCz;ïÞ}
&ŠËb`ÚÀ"gJií¼­µ>]?vŠ­ê¶ÃÎ)î ~tXR"z«Ž´·í 5'å%Óí1›Çñ‚ {À”rÛöu‚:[ðR
áé#Ãžì†œ2w=mÃ‡Ü€`ygHÕxãhf£×@Ž•ea˜Ú—)‡{]"ƒtÂÐJÒö-àÅæ‹å°’ÞòE—¿ð6¥‘)
M”ãk‡Qq*>ÇGžŒökâNFÇ›Mœ‹ŒÒbh³7N{î~–W8ïÓ”¼8ÅÐËR”_¡o›n‰ÃÀŽUÑþÒg¯B:{¾–l*»éâÑMüãzÅ{Çn…ç{`ÑžKiµ]P+ÐáÖ[&ûCn—kNcÎD>‹;öA^*h‹âæ˜¨õMÈ”MAÙà
fQgü¬v hÐ–¥†0s;[I¹;[fæÞ„ÿ±®.$…„&ARµ•¤t‡-[ÂRãyÖ´PéûÜ¾¸®6(âd(´3Ï’”yH6H,VôÉÚè¼ç‘M%MWœ¡)¢ªŽCþMXâ‚†n7äÖ5ÒïvsÍiaG}þ-WÄ™ÎÞÌ[|°ä¤!†îñ£ d}õ)ÞödÚöo©µg°à€ö•ƒ[[!f/÷g©]Í:'&S‘ÕýT%õ†9í¸°}r¥YØ2¯ç½T1ƒBóiSÞŒ(\î×Î·îzgÍú¾Mø[¤)8óC¤‚ MnNÿó°R’W‘Hƒ	ˆ²ŸññÞÀýp´U-©Ç«Y^*+”¢¯	å{¥†8Ò)‘Þœ¡pÚL¼ñy6Tæê²Å—Öƒª],m2E—½FS™k·
”mˆØÚêHÂJ‡o…2h
Da_f”©½ïåÌô\WOd+¹;[ý1¹7]˜¡ÌF‘€Ô÷©Ã"Hº/PÃWwjë!–WŠ†*0ý¯hâÞ(W±H»pZ¼~.®®‰y>ÙAÞºmOô#™7á=i¸•À€*=ÚFFæ(#Äæ¹œDVcsœ]†¨a7	›
`k¨LŸûâaýÒü ,Ï}¯óTÚUçÚ®rWb7,¦…p6ö‘.8œ÷N]Ç‹;(‚'è{JÖþ(gÿ4¬âB7#ë*(§^ãXb#2BŒ²‘žã¼¯«ÿ´+ˆ‘g[U;ªœçc£ú¢©po8ŸWz¾¹vçN]þ½qYádÂ¥™6m¯{¥aIyµœLu,fvJÝ«(ø\ÆšùWk;4»»rÿÚ$G;€K™Ï|
k$à±œáH™Èeà±ÙÅ8ˆqgªô·9Æµ¼3Ž“m±„*EÃÈA° XéØ=Å®µ]“7ª$¬ÌèDÏÃÍ8	]ŸÏãä7…ð.b¯†Î‰“°l­ÖÔl¬­ëáJ

P r­ë]¶GÂÔÊÐ1ÌÐ¹žÐp¶™yY=XY@V2:<øÐ3ë*êÐZî´¢ôK%éx6CFO&¼´¶\(ÓxKƒ˜KéÃ™Å†¯ÛÜ’‚ÀýÂð¨Žöÿ·„ |	ÞÖÐáy›×~®å©·«š‹“Qzíãk…BY¯„Y–€¿…Ùhï©­ï&%Sh"÷
h4’“N#Â‘0N9gâjäÇ­'oê‰ê‰GfôfŠÜÍÄfÌj`Kh™5Ö**À´|hñÙšßj61ËþÛsVÏ„ý@™_kýô!`0.‡èªQÇ-é@¢)#dê-<œªù×Äœ=&ôä¸šiÐ¸:vœ²€Óç1¿%¤(Ø'æùÛx‚üòfÆßa§äSo:ÅÄ®÷
îÿÊ!¼2¯‰†úÕ~Pm1¿ÕûÕÇ[E!C«·<3×?E®¡c´$#¸ÃbfeGgÆþ÷ˆ°ÍHÏ|ªâ»ãñ.ÀrU9µ•­ï½³ZÁþÏbå´ã4ñqà+å7èÀÍ³pQS,˜ŒŸ°£¡£ŠÛ~¡÷¶±×g¿U­YR’Ž%Ðßk#¬üê˜Ï!RI(V?Ø=rO<XXTZÑ
^ê`c&h(¡Í°åRzZùvÐA0eÚÝ‘ÜgâÙ8?ÈÊ1¢C3VÈoÛ,! ¦H4å¯^ha ±:r´#•kæúdü^ñžÊUÃ2L²¥[ú—á‚üw#hƒ?¸(Ž{™$9(»hKÏpdBÅƒG’6˜*	å1ì÷5«õÙ«F(;ÄB2Â²5ÕÅ®+cÂ†îæXý/315°*y†Ò!æz­3Z0ÚÇ8xR/¼êùy
h©O³ÕÌk)¢½l<‘EÇ¤o@#vWÓim%ûŽq3KLRŸ|_vV3j(Ÿ¶ùúù¹XFò°{´L¹VØž¾kÃÆÁ¡ŒàFÑ!gheKIx³<<{»³‰ÇžPŸZÍçYS¥/XÇ£A>íTó˜ÆàÝŒ ñúùê±WÓÚØÊ-â}É›ŠqlIòêÎÞðNí²-LE¥óHW_¸IŠÝuæI•·|ÈbÏWL44êˆHV
$¹U¦¾ý¾QéùtÝß‡X¼iìÁ§#àÙ2{”ûögc.þ=™I¢âgÃÀ÷)gŸ\¢à
röã¦t·7 S«MwÒ4¾mRìÂûb÷t3Ç…Râj”._+6­ŒKOMàyð“c@º›Ñs0V¿ˆ:>ƒ ƒÕ¸á¢¥áåQ» ÎDŒ„'3<® |? ÏÁ_?Þ’âïžM¸³³BŽÆ]L­r&äëií‡æYnq°<Ueª§h}­éè£¼µí#`å>þEuF¬Ò+Pf,„ýß”Kuò8±Hc—ÝÔ®qƒ^@	Y/’Ÿêq¤Å$íS·ŠÁx¡ÄÌSíp
ÒÙTÁ ¾Ëìe»[l¯G0cïQpn¯Wfkm[Êý=1ÂžË…Gm&z–ìër•ª†_Øc¹ !ÄúÿÁ“¬ËÀ<ï8]­Œ=œÄyœˆ†úd=n’±¯Ržœ7¿qCù6•Sá~ãbÚçW«tŸC´ä2	·åuëƒ4çÕ¶\2"2ªrþßïDøœîÈ=xlØmu[‘•º@¹Ý¬8Ê´Ÿ4&c
Ytå5ÂSÈŽÄDgÌ‡ü&@änÓìJöìž?nkúŸm´[÷UÈÆVè
%{–Þž¸K{ê¯™€xÄUší®‹€ùTéX ÅDmÄÃ±ßkƒ&§@Ö;?„‘õ’pðEs‰ÚPÆ6~M}M½ FÚž¶éÄC´Bì/AHã2«ÔdŠ%øXS6pUUï¶¬ÅDâ#>Üm)•fîÉùÒfV¿ÉP‚6R˜pþàát—Ô
¾Í? ,E˜åø‰œÜùKßÜmÁ1ÛŸ¯ª7kêZ³ˆSvôóxèB‘Ç	yÏ*4hœµHR]¼y1U·Uc36Ó€Sð÷îºIP:œ{Û[¡þ€_éË9ÌÎiq²ß“r9Jp,ÇD˜Žg½#Õ‘vMÓwÑ8ùd7hÞÈW¸­÷žo§'³åí©L…Ôú¡¦¨ÓÚÜŠÙœ»—Ê=o…Iÿ£ðÎ]Nâ¶î]#`1ïW¶ÅÊEì•|ì†É’m«Ž}ê
¶Ù,N·u‹ðŠ¤Tƒèeò×ÂñÏŽâië`EóÂ…³Î_’ôfÅwp»²p)Xøh§äSZ¬¨áý¯ôÖŸâ?šÊAƒÈ°0E×g0\ßÝ• é”ÕŸÛh¤TÕæ<TÓWŸ¬PZ ãuÃ.'4d¢QÆ{½Ù‚"Ì“Mlå†¡^´-`Å//âÌâM¹ÉÁ©ómJsˆÌ\‹óÍuáQšW¤ô¬ûì>€Çx%ta{ÄéDÔPQÍÛÄ²âÔúæ’'ó Ë&NÅ#]¤ 2Jz²THªäe®Jé<·êˆtBÏS²aky -Své·CsÓ´°b!­'äén¤v”QÐCR‹.ì [˜…¨ËX/É=ðØ13y¿bNä˜šÿÄlšñßì”8	·g“•±µCœ
øþHñm5úµ*ŒÃË>õSV´¿åÈ¸6‰Q—ŒV‚L|ù2á~¡–p¤È£÷ŸKD1x¤¨‚,68ŠZf­72 }2ßT×âs_¬»’¬vJÍPâÌ4¼"–k6˜Ç,6ÙdropË´¸¯Dþ7B4ª\¢ÉÝ¸ bn'´þµ!’›&¨Ag°kHßÓ*xa]51UÚ8ÂÜ‹ÝÊƒ¿·RïÄ¬"õÑ„GøÖ—k@c=fL™Ÿ\Jü³w8®ûÖÖän /N–÷ÙLÕrÊé?¦ªY%’ŽÕÒR"‘ÊÖ‰Ò¯IüI­žÔR¶-£ë…L#·ŠŽÔ‡ªéOÂ@„;]™›±(YUÜµ­âYôšÉ”2‚·aÂ‰ïû0ÂcóŸß\rDºUøŸ÷§Ú¾ÉÈ{«æ3€«Û%3ÿÝ¶$QŽmÒ¬òSå‹º5—i°È™ì(‰)ž8¥ÞIb½g,÷4#¹)ä}hÖ1´Bÿ‹âëCïÃ³.ôœ?–À°ÞÕâhÆ’æmœÙŽêDÎr˜Î
)Äs¼’SF?›ô1ÎgÇAi±R>”TÞ,žÛºBxy°t³LÊÃ1ÿ£È /ËÀ.¼ø+“Þõ_TÞ²²w2"¤ñ¸õ¤wg+BjÚïÙlj‹¦^%º§—¡Z6$Ôþ¢E}î+äFÌÙº€a+†¾Â¡hG/!üaI ÃIì—®«ñtX‡±}nl  pa—š:Áï–·JñF/íˆ¶ãH.÷lÅÍ@§aËŸž¾îi­e·dk˜
qO”ÝÃÑGÆ'&±›â(@ò=èQP×¬A†û©˜ÌÎì³xfâÙÎ°a Ýí/2Xõ;ãFHÓ8&¦y¸“{,ïCê ¤ê¿)úÜ+é'ÍàE.iðÂ¾ú˜Tî)9‹œ™xÕñ¬?F#*LÄ#Â\1¥‹×	µ^Ò¨Ûë ß_ç”“Î0ÜãDÕ’¿‹É_Gé‡uà+Ø`—Í‰aäæ(1¬j´Ž„)AYùXSŽäò|žR±	Ÿ¯6R“aG7„*?â2Œs¶ÊYøTùþnw‡ÎògvºØúàˆ™¿>ˆI]°±(0{¹€>æø…ÊVeóâ!J„­Ïñ‹DáI±Ê½ Ót&²at˜ƒ7dr¬~¸íeüµÚ­¶~4žÁŒ½ƒxY0,tTÂ¬™T¡;:íœü>Ü!ˆŒ*´Œÿ ¿øI ¿‹æ-4j)rÁ¶}Ëò±{ÿ4#ÆàBÚµv<·sÒjØ²ýÏÿì ô
Töt'"Çn=ObÈœ÷Arü«-xÙ¸íh’ÏÓ­ë-QVHçW[qzÍP°éèâgÖÎË[ì±¥ŒÊ5;?Í‚€h³jãé:%~¢$3š)UHtƒÅSìhç(Íø¢7•=m-GÚYÃÖ²4ö9µˆ¡$1ä &ý–•‹yFIx}ÁÁO<+Ž§>	ã®uá(vìü”ËcvG.ç*ŒöÒHVä‹¢Øçhµ9&ÛÒ×ç!@v\åTai?Ÿ¯®ô“pkvŸv,AµxFSÞÿ%Ú¦×O
ïYwžn [·ÎËQ¿¤{ÙÃvË´g=õüîO1Rû„6Gnc²	¡ï¸pCxÂÕû{Çð›±¼ÓÒUb)tƒN8ÐÍˆÉgoÎ@¨ä#’Öh]€ï=Ñ¢¨ÈèCCëE	ŒªzWa¶7¦*“TŒ÷ƒ3Ê_ÉZ6.ûœâg€Y=
¿¼†É<T?œ&étªvxå¹–e!kó¢	¨£4Iå½Ñ—ÕA¤e2€!žM…Pg§•òÖ„ÍzçÍl>ùJ³/¿^¦_ñ+Jüké!«ˆ†Z:îÒ–õÛ¤ñ”ÌŠg/¶q	
HkÖ¾ñFÿ ·þåï;b†©¸ôwO‚Ú£:!pžDÎ¹=x±žnc7Ý—šÌ!Ÿáy¹f.Ûí*Éšrs•-r¸Ø	¸Õ³q‡À©éF‡.ð¸u˜vpz|ßp>cñx €ÉDÒ½/WdGÖOžÖÏ®g`Aè0Žè‚}@P­s2w$<ð¶|Ÿè@J¼²t†p©òO… x+0¥ÑùíÁ¸áL¿>&  sÐûË3¶€J×gwÁ\øòN+¾vöÍ…ÃLw€„Þ¹Ñì^÷[h¯òEFÿ@S'Ù"Hö¬Ý¯Íe_5‡`^¼ŠS9Y°ÈºÒù):Øƒq§O#ÿê6ÀÝYÎâÏ	Ãç™yÀŠ‡qì“ÜoîÞ(å3Tð´C_\:Zå‹Þ­EîéÏØyÑZ©¶•Ñ¯ò`RÉ5°ÿW/¶ã¢¾J&G(Öá
„ÄJC)•Ò:TINßÐÝÐ€W¦¼HìæË­@Ã¹M…‚N5B,q‘M›ø¹8 ¹¹©²0Ê?¨VVž½³8Ý2³	rýmâ§ÐÆƒ^k£ð† ŸJ6ÜÌF˜ê$ÖqtÙë÷Õô–>K,?I+è¥ý»óÔ‰€®:ÓV>N7r­j Ò™Ýˆ£HkäŽ¯tV5'ãÚFÐ©.Î ›[Ø1'ÐÓüÉû·r™ÂÅÇùQ§võ´pì¼g=+Ù’§VD¿—¨Oð*öUñÛÅ!/>b2å82ÊÜoA<üh#à¾ç¨WX•PL¨×OîA%ï»]=«ëZÆ0+NVx¼Î¨E¸í¤µGu™A¬NÕuŸeYüQ£Ð«Ù]‹£†ß˜¬¾ÚƒÊíAaÜNán€5Å¦¢½&³û>·q§µ‹µí‘:ê&¦½®^-sQäU`Dájì/P£½TÎÊÉÝ™µÕA*œ¥åo¯§¦”„ï/úîÆâB¤FÐo%ÑOò@ç¤ôz–šbÜ…0 9ÂWeŠ«±7ñ;B÷ø¦ö.*-3»%
ç °¨»“Šù“(ë|
çƒG{5«Æß£øg¯Î›€.F*,™Ö˜)7_=?¹«¼÷	Kßß"ûÑ¾;G¦F…[^-o•æt¿ fÜÈS†U`édrMÇ9uKâoÐˆÌó(	O/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/pci.h>
#include <linux/i2c.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ctrls.h>
#include <media/tuner.h>
#include <media/tveeprom.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dvb.h>
#include <media/rc-core.h>

#include "cx23885-reg.h"
#include "media/drv-intf/cx2341x.h"

#include <linux/mutex.h>

#define CX23885_VERSION "0.0.4"

#define UNSET (-1U)

#define CX23885_MAXBOARDS 8

/* Max number of inputs by card */
#define MAX_CX23885_INPUT 8
#define INPUT(nr) (&cx23885_boards[dev->board].input[nr])

#define BUFFER_TIMEOUT     (HZ)  /* 0.5 seconds */

#define CX23885_BOARD_NOAUTO               UNSET
#define CX23885_BOARD_UNKNOWN                  0
#define CX23885_BOARD_HAUPPAUGE_HVR1800lp      1
#define CX23885_BOARD_HAUPPAUGE_HVR1800        2
#define CX23885_BOARD_HAUPPAUGE_HVR1250        3
#define CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP   4
#define CX23885_BOARD_HAUPPAUGE_HVR1500Q       5
#define CX23885_BOARD_HAUPPAUGE_HVR1500        6
#define CX23885_BOARD_HAUPPAUGE_HVR1200        7
#define CX23885_BOARD_HAUPPAUGE_HVR1700        8
#define CX23885_BOARD_HAUPPAUGE_HVR1400        9
#define CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP 10
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP 11
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H 12
#define CX23885_BOARD_COMPRO_VIDEOMATE_E650F   13
#define CX23885_BOARD_TBS_6920                 14
#define CX23885_BOARD_TEVII_S470               15
#define CX23885_BOARD_DVBWORLD_2005            16
#define CX23885_BOARD_NETUP_DUAL_DVBS2_CI      17
#define CX23885_BOARD_HAUPPAUGE_HVR1270        18
#define CX23885_BOARD_HAUPPAUGE_HVR1275        19
#define CX23885_BOARD_HAUPPAUGE_HVR1255        20
#define CX23885_BOARD_HAUPPAUGE_HVR1210        21
#define CX23885_BOARD_MYGICA_X8506             22
#define CX23885_BOARD_MAGICPRO_PROHDTVE2       23
#define CX23885_BOARD_HAUPPAUGE_HVR1850        24
#define CX23885_BOARD_COMPRO_VIDEOMATE_E800    25
#define CX23885_BOARD_HAUPPAUGE_HVR1290        26
#define CX23885_BOARD_MYGICA_X8558PRO          27
#define CX23885_BOARD_LEADTEK_WINFAST_PXTV1200 28
#define CX23885_BOARD_GOTVIEW_X5_3D_HYBRID     29
#define CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF 30
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000 31
#define CX23885_BOARD_MPX885                   32
#define CX23885_BOARD_MYGICA_X8507             33
#define CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL 34
#define CX23885_BOARD_TEVII_S471               35
#define CX23885_BOARD_HAUPPAUGE_HVR1255_22111  36
#define CX23885_BOARD_PROF_8000                37
#define CX23885_BOARD_HAUPPAUGE_HVR4400        38
#define CX23885_BOARD_AVERMEDIA_HC81R          39
#define CX23885_BOARD_TBS_6981                 40
#define CX23885_BOARD_TBS_6980                 41
#define CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200 42
#define CX23885_BOARD_HAUPPAUGE_IMPACTVCBE     43
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2 44
#define CX23885_BOARD_DVBSKY_T9580             45
#define CX23885_BOARD_DVBSKY_T980C             46
#define CX23885_BOARD_DVBSKY_S950C             47
#define CX23885_BOARD_TT_CT2_4500_CI           48
#define CX23885_BOARD_DVBSKY_S950              49
#define CX23885_BOARD_DVBSKY_S952              50
#define CX23885_BOARD_DVBSKY_T982              51
#define CX23885_BOARD_HAUPPAUGE_HVR5525        52
#define CX23885_BOARD_HAUPPAUGE_STARBURST      53
#define CX23885_BOARD_VIEWCAST_260E            54
#define CX23885_BOARD_VIEWCAST_460E            55
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB     56
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC    57
#define CX23885_BOARD_HAUPPAUGE_HVR1265_K4     58
#define CX23885_BOARD_HAUPPAUGE_STARBURST2     59
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885 60
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885 61
#define CX23885_BOARD_AVERMEDIA_CE310B         62

#define GPIO_0 0x00000001
#define GPIO_1 0x00000002
#define GPIO_2 0x00000004
#define GPIO_3 0x00000008
#define GPIO_4 0x00000010
#define GPIO_5 0x00000020
#define GPIO_6 0x00000040
#define GPIO_7 0x00000080
#define GPIO_8 0x00000100
#define GPIO_9 0x00000200
#define GPIO_10 0x00000400
#define GPIO_11 0x00000800
#define GPIO_12 0x00001000
#define GPIO_13 0x00002000
#define GPIO_14 0x00004000
#define GPIO_15 0x00008000

/* Currently unsupported by the driver: PAL/H, NTSC/Kr, SECAM B/G/H/LC */
#define CX23885_NORMS (\
	V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_443 | \
	V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK    |  V4L2_STD_PAL_I    | \
	V4L2_STD_PAL_M  |  V4L2_STD_PAL_N     |  V4L2_STD_PAL_Nc   | \
	V4L2_STD_PAL_60 |  V4L2_STD_SECAM_L   |  V4L2_STD_SECAM_DK)

struct cx23885_fmt {
	u32   fourcc;          /* v4l2 format id */
	int   depth;
	int   flags;
	u32   cxformat;
};

struct cx23885_tvnorm {
	char		*name;
	v4l2_std_id	id;
	u32		cxiformat;
	u32		cxoformat;
};

enum cx23885_itype {
	CX23885_VMUX_COMPOSITE1 = 1,
	CX23885_VMUX_COMPOSITE2,
	CX23885_VMUX_COMPOSITE3,
	CX23885_VMUX_COMPOSITE4,
	CX23885_VMUX_SVIDEO,
	CX23885_VMUX_COMPONENT,
	CX23885_VMUX_TELEVISION,
	CX23885_VMUX_CABLE,
	CX23885_VMUX_DVB,
	CX23885_VMUX_DEBUG,
	CX23885_RADIO,
};

enum cx23885_src_sel_type {
	CX23885_SRC_SEL_EXT_656_VIDEO = 0,
	CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO
};

struct cx23885_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

/* buffer for one video frame */
struct cx23885_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head queue;

	/* cx23885 specific */
	unsigned int           bpl;
	struct cx23885_riscmem risc;
	struct cx23885_fmt     *fmt;
	u32                    count;
};

struct cx23885_input {
	enum cx23885_itype type;
	unsigned int    vmux;
	unsigned int    amux;
	u32             gpio0, gpio1, gpio2, gpio3;
};

typedef enum {
	CX23885_MPEG_UNDEFINED = 0,
	CX23885_MPEG_DVB,
	CX23885_ANALOG_VIDEO,
	CX23885_MPEG_ENCODER,
} port_t;

struct cx23885_board {
	char                    *name;
	port_t			porta, portb, portc;
	int		num_fds_portb, num_fds_portc;
	unsigned int		tuner_type;
	unsigned int		radio_type;
	unsigned char		tuner_addr;
	unsigned char		radio_addr;
	unsigned int		tuner_bus;

	/* Vendors can and do run the PCIe bridge at different
	 * clock rates, driven physically by crystals on the PCBs.
	 * The core has to accommodate this. This allows the user
	 * to add new boards with new frequencys. The value is
	 * expressed in Hz.
	 *
	 * The core framework will default this value based on
	 * current designs, but it can vary.
	 */
	u32			clk_freq;
	struct cx23885_input    input[MAX_CX23885_INPUT];
	int			ci_type; /* for NetUP */
	/* Force bottom field first during DMA (888 workaround) */
	u32                     force_bff;
};

struct cx23885_subid {
	u16     subvendor;
	u16     subdevice;
	u32     card;
};

struct cx23885_i2c {
	struct cx23885_dev *dev;

	int                        nr;

	/* i2c i/o */
	struct i2c_adapter         i2c_adap;
	struct i2c_client          i2c_client;
	u32                        i2c_rc;

	/* 885 registers used for raw address */
	u32                        i2c_period;
	u32                        reg_ctrl;
	u32                        reg_stat;
	u32                        reg_addr;
	u32                        reg_rdata;
	u32                        reg_wdata;
};

struct cx23885_dmaqueue {
	struct list_head       active;
	u32                    count;
};

struct cx23885_tsport {
	struct cx23885_dev *dev;

	unsigned                   nr;
	int                        sram_chno;

	struct vb2_dvb_frontends   frontends;

	/* dma queues */
	struct cx23885_dmaqueue    mpegq;
	u32                        ts_packet_size;
	u32                        ts_packet_count;

	int                        width;
	int                        height;

	spinlock_t                 slock;

	/* registers */
	u32                        reg_gpcnt;
	u32                        reg_gpcnt_ctl;
	u32                        reg_dma_ctl;
	u32                        reg_lngth;
	u32                        reg_hw_sop_ctrl;
	u32                        reg_gen_ctrl;
	u32                        reg_bd_pkt_status;
	u32                        reg_sop_status;
	u32                        reg_fifo_ovfl_stat;
	u32                        reg_vld_misc;
	u32                        reg_ts_clk_en;
	u32                        reg_ts_int_msk;
	u32                        reg_ts_int_stat;
	u32                        reg_src_sel;

	/* Default register vals */
	int                        pci_irqmask;
	u32                        dma_ctl_val;
	u32                        ts_int_msk_val;
	u32                        gen_ctrl_val;
	u32                        ts_clk_en_val;
	u32                        src_sel_val;
	u32                        vld_misc_val;
	u32                        hw_sop_ctrl_val;

	/* Allow a single tsport to have multiple frontends */
	u32                        num_frontends;
	void                (*gate_ctrl)(struct cx23885_tsport *port, int open);
	void                       *port_priv;

	/* Workaround for a temp dvb_frontend that the tuner can attached to */
	struct dvb_frontend analog_fe;

	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	struct i2c_client *i2c_client_sec;
	struct i2c_client *i2c_client_ci;

	int (*set_frontend)(struct dvb_frontend *fe);
	int (*fe_set_voltage)(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage);
};

struct cx23885_kernel_ir {
	struct cx23885_dev	*cx;
	char			*name;
	char			*phys;

	struct rc_dev		*rc;
};

struct cx23885_audio_buffer {
	unsigned int		bpl;
	struct cx23885_riscmem	risc;
	void			*vaddr;
	struct scatterlist	*sglist;
	int			sglen;
	unsigned long		nr_pages;
};

struct cx23885_audio_dev {
	struct cx23885_dev	*dev;

	struct pci_dev		*pci;

	struct snd_card		*card;

	spinlock_t		lock;

	atomic_t		count;

	unsigned int		dma_size;
	unsigned int		period_size;
	unsigned int		num_periods;

	struct cx23885_audio_buffer   *buf;

	struct snd_pcm_substream *substream;
};

struct cx23885_dev {
	atomic_t                   refcount;
	struct v4l2_device	   v4l2_dev;
	struct v4l2_ctrl_handler   ctrl_handler;

	/* pci stuff */
	struct pci_dev             *pci;
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
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

/* ------