 when a subsequent affinity request
 * occurs after the stopper bailed out due to the targeted task still being
 * Migrate-Disable. Consider:
 *
 *     Initial conditions: P0->cpus_mask = [0, 1]
 *
 *     CPU0		  P1				P2
 *     <P0>
 *       migrate_disable();
 *       <preempted>
 *                        set_cpus_allowed_ptr(P0, [1]);
 *                          <blocks>
 *     <migration/0>
 *       migration_cpu_stop()
 *         is_migration_disabled()
 *           <bails>
 *                                                       set_cpus_allowed_ptr(P0, [0, 1]);
 *                                                         <signal completion>
 *                          <awakes>
 *
 * Note that the above is safe vs a concurrent migrate_enable(), as any
 * pending affinity completion is preceded by an uninstallation of
 * p->migration_pending done with p->pi_lock held.
 */
static int affine_move_task(struct rq *rq, struct task_struct *p, struct rq_flags *rf,
			    int dest_cpu, unsigned int flags)
{
	struct set_affinity_pending my_pending = { }, *pending = NULL;
	bool stop_pending, complete = false;

	/* Can the task run on the task's current CPU? If so, we're done */
	if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask)) {
		struct task_struct *push_task = NULL;

		if ((flags & SCA_MIGRATE_ENABLE) &&
		    (p->migration_flags & MDF_PUSH) && !rq->push_busy) {
			rq->push_busy = true;
			push_task = get_task_struct(p);
		}

		/*
		 * If there are pending waiters, but no pending stop_work,
		 * then complete now.
		 */
		pending = p->migration_pending;
		if (pending && !pending->stop_pending) {
			p->migration_pending = NULL;
			complete = true;
		}

		task_rq_unlock(rq, p, rf);

		if (push_task) {
			stop_one_cpu_nowait(rq->cpu, push_cpu_stop,
					    p, &rq->push_work);
		}

		if (complete)
			complete_all(&pending->done);

		return 0;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		/* serialized by p->pi_lock */
		if (!p->migration_pending) {
			/* Install the request */
			refcount_set(&my_pending.refs, 1);
			init_completion(&my_pending.done);
			my_pending.arg = (struct migration_arg) {
				.task = p,
				.dest_cpu = dest_cpu,
				.pending = &my_pending,
			};

			p->migration_pending = &my_pending;
		} else {
			pending = p->migration_pending;
			refcount_inc(&pending->refs);
			/*
			 * Affinity has changed, but we've already installed a
			 * pending. migration_cpu_stop() *must* see this, else
			 * we risk a completion of the pending despite having a
			 * task on a disallowed CPU.
			 *
			 * Serialized by p->pi_lock, so this is safe.
			 */
			pending->arg.dest_cpu = dest_cpu;
		}
	}
	pending = p->migration_pending;
	/*
	 * - !MIGRATE_ENABLE:
	 *   we'll have installed a pending if there wasn't one already.
	 *
	 * - MIGRATE_ENABLE:
	 *   we're here because the current CPU isn't matching anymore,
	 *   the only way that can happen is because of a concurrent
	 *   set_cpus_allowed_ptr() call, which should then still be
	 *   pending completion.
	 *
	 * Either way, we really should have a @pending here.
	 */
	if (WARN_ON_ONCE(!pending)) {
		task_rq_unlock(rq, p, rf);
		return -EINVAL;
	}

	if (task_running(rq, p) || READ_ONCE(p->__state) == TASK_WAKING) {
		/*
		 * MIGRATE_ENABLE gets here because 'p == current', but for
		 * anything else we cannot do is_migration_disabled(), punt
		 * and have the stopper function handle it all race-free.
		 */
		stop_pending = pending->stop_pending;
		if (!stop_pending)
			pending->stop_pending = true;

		if (flags & SCA_MIGRATE_ENABLE)
			p->migration_flags &= ~MDF_PUSH;

		task_rq_unlock(rq, p, rf);

		if (!stop_pending) {
			stop_one_cpu_nowait(cpu_of(rq), migration_cpu_stop,
					    &pending->arg, &pending->stop_work);
		}

		if (flags & SCA_MIGRATE_ENABLE)
			return 0;
	} else {

		if (!is_migration_disabled(p)) {
			if (task_on_rq_queued(p))
				rq = move_queued_task(rq, rf, p, dest_cpu);

			if (!pending->stop_pending) {
				p->migration_pending = NULL;
				complete = true;
			}
		}
		task_rq_unlock(rq, p, rf);

		if (complete)
			complete_all(&pending->done);
	}

	wait_for_completion(&pending->done);

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
	 * Clearly, migrati«èæ¬Üî>ŒĞ´_ÒˆÖ4–ÏÅœgl¦T²Øœh\
wùåı%&|²çí°ZÑYEİ,òíÕQémê¸'%-KhĞª¥zPêªí7óŠ‡L[ãlQåš^{rĞ¹P^ëıé—,ÔéŞ¯{Ê±´¯Ø«¡‘x³hÏ¸gA%çˆ=>¼ßpûGŞßhkùülòû”U—Oï-˜ŠÜS%ôáJßT®F¹3ÒnÉÍüÊ®LİdÕš`l²BÇ|jÿ×ßæ›ÔHvò˜ê/«^7ªÇÒv’v²¨¶s±€ÀÍAï†ÆåB;NŒ”#v¨Ş£‡tÿtÖêßu Gô–Q,YR)ß³½Ş%+¤—kÖ0dA})$áloÁ]ÌSlŸIÃ'+ Tdw’{‘VHèııiÖ¢å§Ë‡°©dAõì83ÜXREhBü†¥;ø¡cbÖ.^Ú?|h0uğPĞÁå@¨“Îaã³Ò1]N45à´ˆFkïb_<u¤ 1Âƒ1‚Htİ—.ü£oLÖÌæEÍï¤í°Ö Æ([œ¹èfÜ€¿ğƒ[½]I4ô,J–8oc ÀMBcüö“/<ÿ—;Mo¥hw ÔÉ6àiŞ»J/¶ñ	íñÕx†ó„ {•'}éù{SÅ%µ¢9Ü¯'v–çtÄ* Iü“Å*Ë7åÈ5Ş){„Xšñõ¯i¶’Ç,qà½ğzı4È5æ«L…Ç»«Z*ÅÖğË]óÖÁ@jXFµ.Rø:ùãöŒ}¹gâ£Õ¢¼hØÓÌì22ÛTøNÉÁrLa›ôõq³%[ÈÏÌ@ôå½ùˆƒÀsÉïó±äúæO WÓ2jÚl®Ü>òú
¤í†êñ|KäÌÿ¿¤©é)å†væ’'Eí\Ù#(Ø(ÿ9¥ÑWPmEuÔ"v‘,SõÜ5¢\æ£ÁBZğ6Y¯ì”%-l’
z›^ÏYÓD¦…e¢Õ,S"¿ ÊuYÏ£a5½Úª]u|¸zş	¯L±Íq^(©APô$ÇNıM±Ô¦‡¼XxHÌ*Ğ?‹¦X/-€ƒ”õş³³`Œ2&Á:h) l°D¿@Bî€¼‚k¯³ƒ2Á R[,.²ğîÇİp”îèÑíA^›Æ%Ôİ»ŒôEsŞ€ƒ¯á—T—Spß¾q¥\D}¡ñs—SAØÉ50Öke?Òqë<ÙóN0M¢NŒx­
¡,S~©*­”—ÓeOŠ‡™ÂÕøn’}¥Ê™ñÅ‹~w8û:´|Ë×›gÑ×%¦2½–8eÎkíëÖÑ/µ^‰Å i¢*zø~LSñ4\WÅ™W¥UöP< c‘wà™»¾­¥áNêŠ¯;ÍÚ½HLAû¡Ö‰ş"æ^V›’Šsßk¡KµW~Xµfœâ¡T«'>èª˜Vî‡Œ×oõ'<>`-¥²4XÔT;p`Òö§#+ÙaŒ$üo ºñ+®%4üSSî›à_qãŞò¾zù$.{§
.‹lQaÀS.,Æ.Sq]i\ƒï®ƒşµOX=x°ˆÔğ—>ıh™û~xô¬Èìq2b?p{w*ó¶A4œ0J,c}.2©.­MÄ\°A³³T2Æ ğ±9nhÎåîßz¨‘äÈrñRlÜÑ\Ñòã#ÿ²½ÀúÛ×JM9)šCïJø-ŞÑÍè# ”O2ÈH;¦OHŠ,Ğ°o9Ëg!Ï»ª˜O_d’24ˆç4-u+±æUËæCq:Uº5LdHÓ!·Ã	LÉÉj` 9ÄËøÛeÑÕ'ß¤Ën/ucU6Ã“RíoN~â
ºSøjÄ†Øø¢tˆÛµ9õ2›+^ê¥„Z§Au{é:¥böÚ»aP÷ÄŠöÚëÉeÓ”Ğa3²¢9—b†Tˆòú_7:f¯HŸ~é4„ÌËû+íaBÔxhÎ½«ƒS»òĞº¤LñPx»Xëb¡ÃÁöƒúç!ş%¡ı6SÍ–™€f®xÊ”x>6R½<±®s~–Ğµ”ıÆb<ue‚0öùVi4§Ò%‚=Óİ3ş1˜æ/¢§ĞtF¿ØaŞå`İ­+Áõ†nŠg H2€G Cê+(“v®IóÈ5/À¸¥î¡Óë6gg‹YÚ=«w÷WE·Cìğ`êí4s6¥Ú·Õğ¶£ğ‘òkrf”•ÈúÕ·á¶{p/ ˆ	S¿no°h¶û²Ñ¡†z³x&î*xÈæ£ÄùcgÒ›ÕG8¯è9#ùJ‡HöòÄf`cÌ°Ş¯á¶¾öÍ¢òG5¥%*5¦Øı=xÛÈ¶ªÔw/xz­\#‹éV–Ô†ß³QˆM¤n­oÂDûÔûÍ]xÁ°[%uL8‹mæ¢ìº÷¼§Š™¨#¬Dõ{é İOft­Õ«ŠDúqy‚g2Ì¥TğÙ/Ù¦1´Hä†–-ÆŠ…é/ÁşBÔ„¬Ó¥¤™¿F$¶XAéô»dJPÄ)åò¬ğø7îûb'ÅÏWU!Ş.•gİ)}«ÏÓ½ìèÚ‘{¥µ?¥¦[A®Êş]oE6ÕØ(ŞELv
“!ßn¼À÷„ıÃ”Ú{Éj çÖç}Â[ú¤‚¯>L3#£ëxÂİaÏ‹ÖvtÑkª>8ç!¬’ah2½s÷Ø,°—ÛpœC_ñî»GÆ2>!:‡YO_û2ÈÅ.ÑÄù¸5w’PO±Zlc5Ë/Om®Ä~¾ÁaËxOú„ãå–NŒİ[%Ac"I,Ü@Ü²¡…Æ¥W´¢Îë(Œ{l× H?@„B/zX+;N3úmwØ@=Ê²÷°$tı }-³’“Nšï½†àHlPâ#Í†°^,!ëô”ƒ•×V‹N/ı–ÕYŒrË<ÍuÜÚú7G ¹u)rS#›†8ìS65gRŸÅWc_Tï2Ér”.Ø„/¬©^U}å2dÇ¦JÂ[óŞ—üÆH	>Ê¦L}Ï:•–±Ì8Õ†œ%€ë
E˜4j—É´zA¥+Ğ™…&}U>Õ´~Ÿ3w×gÿÕÀoÎá}ãîè|ê8àc
Şœ8ÈNAè¯ñş&jËµÃt}@›âŠMó0;úÉ‚§™(‰¹‹s§Õ	ªTˆ7]êš&6kIÄö¬	[R«(ò˜ÇÃ°‡&|sõZî±lVÈÏd,ê°e¤¬õ°ªv ŒcI@û*ÆáøCG 9şúÓH7˜m”…ò!)Œ‰ÔüvoYåiÎ»_n…»_NChşŞ1c-Q"G+œ8Ñ4HgğpfVÊKµäŒèá³hÜ1•Ùl3ÜJ~YØ¸ûEİéÊ7u#®uXã¨ÍíÕ¶W©mÑ¸e6ÅR_¤¤$­‘âíÒ%ºNv+Ş¼Š¿í+‘Ü/s1&*k™ş²—W2
$îïS©TmÑ!Ó¸<vI'2«ÒÊ[5–šÊºä6²YƒÂu™¤Qy,—7Ø*µD”é¥øGá¥·:î‹æ:5XC×ŸSÇx­Zdeğ¿ğ
¥aÛõ"73ĞW
ÿtØp
&m°ïçõÅ4RØ–>«e<§®‹µLıDøë„Ş”Ö=Ù÷ƒğ
Ãx$».jIxRUYT{BòCz;ïŞ}
&ŠËb`ÚÀ"gJií¼­µ>]?vŠ­ê¶ÃÎ)î ~tXR"z«´·í 5'å%Óí1›Çñ‚ {À”rÛöu‚:[ğR
áé#Ãì†œ2w=mÃ‡Ü€`ygHÕxãhf£×@•ea˜Ú—)‡{]"ƒtÂĞJÒö-àÅæ‹å°’ŞòE—¿ğ6¥‘)
M”ãk‡Qq*>ÇGŒökâNFÇ›Mœ‹ŒÒbh³7N{î~–W8ïÓ”¼8ÅĞËR”_¡o›n‰ÃÀUÑşÒg¯B:{¾–l*»éâÑMüãzÅ{Çn…ç{`ÑKiµ]P+ĞáÖ[&ûCn—kNcÎD>‹;öA^*h‹âæ˜¨õMÈ”MAÙà
fQgü¬v hĞ–¥†0s;[I¹;[fæŞ„ÿ±®.$…„&ARµ•¤t‡-[ÂRãyÖ´PéûÜ¾¸®6(âd(´3Ï’”yH6H,VôÉÚè¼ç‘M%MWœ¡)¢ªCşMXâ‚†n7äÖ5ÒïvsÍiaG}ş-WÄ™ÎŞÌ[|°ä¤!†îñ£ d}õ)ŞödÚöo©µg°à€ö•ƒ[[!f/÷g©]Í:'&S‘ÕıT%õ†9í¸°}r¥YØ2¯ç½T1ƒBóiSŞŒ(\î×Î·îzgÍú¾Mø[¤)8óC¤‚ MnNÿó°R’W‘Hƒ	ˆ²ŸññŞÀıp´U-©Ç«Y^*+”¢¯	å{¥†8Ò)‘Şœ¡pÚL¼ñy6Tæê²Å—Öƒª],m2E—½FS™k·
”mˆØÚêHÂJ‡o…2h
Da_f”©½ïåÌô\WOd+¹;[ı1¹7]˜¡ÌF‘€Ô÷©Ã"Hº/PÃWwjë!–WŠ†*0ı¯hâŞ(W±H»pZ¼~.®®‰y>ÙAŞºmOô#™7á=i¸•À€*=ÚFFæ(#Äæ¹œDVcsœ]†¨a7	›
`k¨LŸûâaıÒü ,Ï}¯óTÚUçÚ®rWb7,¦…p6ö‘.8œ÷N]Ç‹;(‚'è{JÖş(gÿ4¬âB7#ë*(§^ãXb#2BŒ²‘ã¼¯«ÿ´+ˆ‘g[U;ªœçc£ú¢©po8ŸWz¾¹vçN]ş½qYádÂ¥™6m¯{¥aIyµœLu,fvJİ«(ø\ÆšùWk;4»»rÿÚ$G;€K™Ï|
k$à±œáH™Èeà±ÙÅ8ˆqgªô·9Æµ¼3“m±„*EÃÈA° XéØ=Å®µ]“7ª$¬ÌèDÏÃÍ8	]ŸÏãä7…ğ.b¯†Î‰“°l­ÖÔl¬­ëáJ

P r­ë]¶GÂÔÊĞ1ÌĞ¹Ğp¶™yY=XY@V2:<øĞ3ë*êĞZî´¢ôK%éx6CFO&¼´¶\(ÓxKƒ˜KéÃ™Å†¯ÛÜ’‚ÀıÂğ¨öÿ·„ |	ŞÖĞáy›×~®å©·«š‹“Qzíãk…BY¯„Y–€¿…Ùhï©­ï&%Sh"÷
h4’“N#Â‘0N9gâjäÇ­'oê‰ê‰GfôfŠÜÍÄfÌj`Kh™5Ö**À´|hñÙšßj61ËşÛsVÏ„ı@™_kıô!`0.‡èªQÇ-é@¢)#dê-<œªù×Äœ=&ôä¸šiĞ¸:vœ²€Óç1¿%¤(Ø'æùÛx‚üòfÆßa§äSo:ÅÄ®÷
îÿÊ!¼2¯‰†úÕ~Pm1¿ÕûÕÇ[E!C«·<3×?E®¡c´$#¸ÃbfeGgÆş÷ˆ°ÍHÏ|ªâ»ãñ.ÀrU9µ•­ï½³ZÁşÏbå´ã4ñqà+å7èÀÍ³pQS,˜ŒŸ°£¡£ŠÛ~¡÷¶±×g¿U­YR’%Ğßk#¬üê˜Ï!RI(V?Ø=rO<XXTZÑ
^ê`c&h(¡Í°åRzZùvĞA0eÚİ‘ÜgâÙ8?ÈÊ1¢C3VÈoÛ,! ¦H4å¯^ha ±:r´#•kæúdü^ñÊUÃ2L²¥[ú—á‚üw#hƒ?¸({™$9(»hKÏpdBÅƒG’6˜*	å1ì÷5«õÙ«F(;ÄB2Â²5ÕÅ®+cÂ†îæXı/315°*y†Ò!æz­3Z0ÚÇ8xR/¼êùy
h©O³ÕÌk)¢½l<‘EÇ¤o@#vWÓim%ûq3KLRŸ|_vV3j(Ÿ¶ùúù¹XFò°{´L¹VØ¾kÃÆÁ¡ŒàFÑ!gheKIx³<<{»³‰ÇPŸZÍçYS¥/XÇ£A>íTó˜ÆàİŒ ñúùê±WÓÚØÊ-â}É›ŠqlIòêÎŞğNí²-LE¥óHW_¸IŠİuæI•·|ÈbÏWL44êˆHV
$¹U¦¾ı¾Qéùtİß‡X¼iìÁ§#àÙ2{”ûögc.ş=™I¢âgÃÀ÷)gŸ\¢à
röã¦t·7 S«MwÒ4¾mRìÂûb÷t3Ç…Râj”._+6­ŒKOMàyğ“c@º›Ñs0V¿ˆ:>ƒ ƒÕ¸á¢¥áåQ» ÎDŒ„'3<® |? ÏÁ_?Ş’âïM¸³³BÆ]L­r&äëií‡æYnq°<Ueª§h}­éè£¼µí#`å>şEuF¬Ò+Pf,„ıß”Kuò8±Hc—İÔ®qƒ^@	Y/’Ÿêq¤Å$íS·ŠÁx¡ÄÌSíp
ÒÙTÁ ¾Ëìe»[l¯G0cïQpn¯Wfkm[Êı=1ÂË…Gm&z–ìër•ª†_Øc¹ !ÄúÿÁ“¬ËÀ<ï8]­Œ=œÄyœˆ†úd=n’±¯Rœ7¿qCù6•Sá~ãbÚçW«tŸC´ä2	·åuëƒ4çÕ¶\2"2ªrşßïDøœîÈ=xlØmu[‘•º@¹İ¬8Ê´Ÿ4&c
Ytå5ÂSÈÄDgÌ‡ü&@änÓìJöì?nkúŸm´[÷UÈÆVè
%{–Ş¸K{ê¯™€xÄUší®‹€ùTéX ÅDmÄÃ±ßkƒ&§@Ö;?„‘õ’pğEs‰ÚPÆ6~M}M½ FÚ¶éÄC´Bì/AHã2«ÔdŠ%øXS6pUUï¶¬ÅDâ#>Üm)•fîÉùÒfV¿ÉP‚6R˜pşàát—Ô
¾Í? ,E˜åø‰œÜùKßÜmÁ1ÛŸ¯ª7kêZ³ˆSvôóxèB‘Ç	yÏ*4hœµHR]¼y1U·Uc36Ó€Sğ÷îºIP:œ{Û[¡ş€_éË9ÌÎiq²ß“r9Jp,ÇD˜g½#Õ‘vMÓwÑ8ùd7hŞÈW¸­÷o§'³åí©L…Ôú¡¦¨ÓÚÜŠÙœ»—Ê=o…Iÿ£ğÎ]Nâ¶î]#`1ïW¶ÅÊEì•|ì†É’m«}ê
¶Ù,N·u‹ğŠ¤Tƒèeò×ÂñÏâië`EóÂ…³Î_’ôfÅwp»²p)Xøh§äSZ¬¨áı¯ôÖŸâ?šÊAƒÈ°0E×g0\ßİ• é”ÕŸÛh¤TÕæ<TÓWŸ¬PZ ãuÃ.'4d¢QÆ{½Ù‚"Ì“Mlå†¡^´-`Å//âÌâM¹ÉÁ©ómJsˆÌ\‹óÍuáQšW¤ô¬ûì>€Çx%ta{ÄéDÔPQÍÛÄ²âÔúæ’'ó Ë&NÅ#]¤ 2Jz²THªäe®Jé<·êˆtBÏS²aky -Své·CsÓ´°b!­'äén¤v”QĞCR‹.ì [˜…¨ËX/É=ğØ13y¿bNä˜šÿÄlšñßì”8	·g“•±µCœ
øşHñm5úµ*ŒÃË>õSV´¿åÈ¸6‰Q—ŒV‚L|ù2á~¡–p¤È£÷ŸKD1x¤¨‚,68ŠZf­72 }2ßT×âs_¬»’¬vJÍPâÌ4¼"–k6˜Ç,6ÙdropË´¸¯Dş7B4ª\¢Éİ¸ bn'´şµ!’›&¨Ag°kHßÓ*xa]51UÚ8ÂÜ‹İÊƒ¿·RïÄ¬"õÑ„GøÖ—k@c=fL™Ÿ\Jü³w8®ûÖÖän /N–÷ÙLÕrÊé?¦ªY%’ÕÒR"‘ÊÖ‰Ò¯IüI­ÔR¶-£ë…L#·ŠÔ‡ªéOÂ@„;]™›±(YUÜµ­âYôšÉ”2‚·aÂ‰ïû0ÂcóŸß\rDºUøŸ÷§Ú¾ÉÈ{«æ3€«Û%3ÿİ¶$QmÒ¬òSå‹º5—i°È™ì(‰)8¥ŞIb½g,÷4#¹)ä}hÖ1´Bÿ‹âëCïÃ³.ôœ?–À°ŞÕâhÆ’æmœÙêDÎr˜Î
)Äs¼’SF?›ô1ÎgÇAi±R>”TŞ,ÛºBxy°t³LÊÃ1ÿ£È /ËÀ.¼ø+“Şõ_TŞ²²w2"¤ñ¸õ¤wg+BjÚïÙlj‹¦^%º§—¡Z6$Ôş¢E}î+äFÌÙº€a+†¾Â¡hG/!üaI ÃIì—®«ñtX‡±}nl  pa—š:Áï–·JñF/íˆ¶ãH.÷lÅÍ@§aËŸ¾îi­e·dk˜
qO”İÃÑGÆ'&±›â(@ò=èQP×¬A†û©˜ÌÎì³xfâÙÎ°a İí/2Xõ;ãFHÓ8&¦y¸“{,ïCê ¤ê¿)úÜ+é'ÍàE.iğÂ¾ú˜Tî)9‹œ™xÕñ¬?F#*LÄ#Â\1¥‹×	µ^Ò¨Ûë ß_ç”“Î0ÜãDÕ’¿‹É_Gé‡uà+Ø`—Í‰aäæ(1¬j´„)AYùXSäò|R±	Ÿ¯6R“aG7„*?â2Œs¶ÊYøTùşnw‡ÎògvºØúàˆ™¿>ˆI]°±(0{¹€>æø…ÊVeóâ!J„­Ïñ‹DáI±Ê½ Ót&²at˜ƒ7dr¬~¸íeüµÚ­¶~4ÁŒ½ƒxY0,tTÂ¬™T¡;:íœü>Ü!ˆŒ*´Œÿ ¿øI ¿‹æ-4j)rÁ¶}Ëò±{ÿ4#ÆàBÚµv<·sÒjØ²ıÏÿì ô
Töt'"Çn=ObÈœ÷Arü«-xÙ¸íh’ÏÓ­ë-QVHçW[qzÍP°éèâgÖÎË[ì±¥ŒÊ5;?Í‚€h³jãé:%~¢$3š)UHtƒÅSìhç(Íø¢7•=m-GÚYÃÖ²4ö9µˆ¡$1ä &ı–•‹yFIx}ÁÁO<+§>	ã®uá(vìü”ËcvG.ç*ŒöÒHVä‹¢Øçhµ9&ÛÒ×ç!@v\åTai?Ÿ¯®ô“pkvŸv,AµxFSŞÿ%Ú¦×O
ïYwn [·ÎËQ¿¤{ÙÃvË´g=õüîO1Rû„6Gnc²	¡ï¸pCxÂÕû{Çğ›±¼ÓÒUb)tƒN8ĞÍˆÉgoÎ@¨ä#’Öh]€ï=Ñ¢¨ÈèCCëE	ŒªzWa¶7¦*“TŒ÷ƒ3Ê_ÉZ6.ûœâg€Y=
¿¼†É<T?œ&étªvxå¹–e!kó¢	¨£4Iå½Ñ—ÕA¤e2€!M…Pg§•òÖ„ÍzçÍl>ùJ³/¿^¦_ñ+Jüké!«ˆ†Z:îÒ–õÛ¤ñ”ÌŠg/¶q	
HkÖ¾ñFÿ ·şåï;b†©¸ôwO‚Ú£:!pDÎ¹=x±nc7İ—šÌ!Ÿáy¹f.Ûí*Éšrs•-r¸Ø	¸Õ³q‡À©éF‡.ğ¸u˜vpz|ßp>cñx €ÉDÒ½/WdGÖOÖÏ®g`Aè0è‚}@P­s2w$<ğ¶|Ÿè@J¼²t†p©òO… x+0¥ÑùíÁ¸áL¿>&  sĞûË3¶€J×gwÁ\øòN+¾vöÍ…ÃLw€„Ş¹Ñì^÷[h¯òEFÿ@S'Ù"Hö¬İ¯Íe_5‡`^¼ŠS9Y°ÈºÒù):Øƒq§O#ÿê6ÀİYÎâÏ	Ãç™yÀŠ‡qì“ÜoîŞ(å3Tğ´C_\:Zå‹Ş­EîéÏØyÑZ©¶•Ñ¯ò`RÉ5°ÿW/¶ã¢¾J&G(Öá
„ÄJC)•Ò:TINßĞİĞ€W¦¼HìæË­@Ã¹M…‚N5B,q‘M›ø¹8 ¹¹©²0Ê?¨VV½³8İ2³	rımâ§ĞÆƒ^k£ğ† ŸJ6ÜÌF˜ê$ÖqtÙë÷Õô–>K,?I+è¥ı»óÔ‰€®:ÓV>N7r­j Ò™İˆ£Hkä¯tV5'ãÚFĞ©.Î ›[Ø1'ĞÓüÉû·r™ÂÅÇùQ§võ´pì¼g=+Ù’§VD¿—¨Oğ*öUñÛÅ!/>b2å82ÊÜoA<üh#à¾ç¨WX•PL¨×OîA%ï»]=«ëZÆ0+NVx¼Î¨E¸í¤µGu™A¬NÕuŸeYüQ£Ğ«Ù]‹£†ß˜¬¾ÚƒÊíAaÜNán€5Å¦¢½&³û>·q§µ‹µí‘:ê&¦½®^-sQäU`Dájì/P£½TÎÊÉİ™µÕA*œ¥åo¯§¦”„ï/úîÆâB¤FĞo%ÑOò@ç¤ôz–šbÜ…0 9ÂWeŠ«±7ñ;B÷ø¦ö.*-3»%
ç °¨»“Šù“(ë|
çƒG{5«Æß£øg¯Î›€.F*,™Ö˜)7_=?¹«¼÷	Kßß"ûÑ¾;G¦F…[^-o•æt¿ fÜÈS†U`édrMÇ9uKâoĞˆÌó(	O/* SPDX-License-Identifier: GPL-2.0-or-later */
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
	__le3