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
	 * Clearly, migrati??????>???_???4????gl?T???h\
?w????%&|????Z?YE?,???Q?m??'%-Kh???zP???7???L[?lQ???^{r??P^????,????{????????x?h??gA%??=>??p?G??hk??l???U?O?-???S%??J?T?F?3?n?????L?d??`l?B?|j???????Hv???/?^7???v?v???s????A????B;N??#v????t?t????u? G???Q,YR)????%?+??k?0dA})$?lo?]?Sl?I?'+ ?Tdw?{?VH???i????????d?A??83?XRE?hB???;??cb?.^???|h0u?P???@???a???1]N45???Fk?b_<u? 1??1?Ht??.??oL???E?????? ?([????f?????[???]I4?,J?8oc ?MBc???/<??;Mo?hw ??6?i??J/??	???x????{?'}??{S?%??9??'v??t?* I???*??7??5?){??X????i???,q???z???4?5??L????Z*????]????@jXF?.R?:????}?g?????h????22?T?N??rLa???q?%[???@?????????s???????O W?2j?l??>??
?????|K???????)??v??'E?\?#(?(?9???WPmEu?"v?,S??5?\???BZ?6Y???%-l?
z?^?Y??D??e??,S"? ?u?Y??a5???]u|?z??	?L??q^(?AP?$?N?M??????XxH?*????X/-???????`?2&?:h)?l?D?@B????k???2??R[,.?????p??????A^??%?????Es??????T?Sp??q?\D}??s?SA??50?ke??q?<??N0M?N?x?
??,S~?*????eO??????n?}??????~w8?:?|???g???%?2??8e?k????/?^?? i?*z?~L?S?4\?W???W?U?P<?c??w???????N???;???HLA?????"??^V???s?k?K?W~X?f???T?'>???V????o?'<>`-??4X?T;p`???#+?a?$?o ??+?%4?S?S???_q????z?$.{?
.?lQa?S.,???.Sq]i\??????OX=x??????>?h??~x????q2b?p{w*??A4?0J,c}.2?.?M?\?A??T2? ??9nh?????z????r?Rl??\???#????????JM9)?C?J?-????# ?O2?H;?OH?,??o9?g!????O_d?24??4-u+??U??Cq?:U?5LdH?!??	L??j`? 9????e??'???n/ucU6??R?oN~?
?S?j?????t???9?2?+^???Z?Au{?:?b???aP?????????e???a3??9?b?T???_7:f?H?~?4????+?aB?xh????S?????L?Px?X?b???????!?%??6S????f?x??x>6R?<??s~??????b<ue?0??Vi4??%?=??3?1??/????tF??a??`??+???n?g?H2?G C?+(?v?I??5/???????6gg?Y?=?w?WE?C??`???4s6??????????krf????????{p/ ?	S?no?h??????z?x&?*x?????cg???G?8??9#?J?H????f`c???????????G5?%*5???=x?????w/xz?\#??V?????Q?M?n?o?D????]x??[%uL?8?m??????????#?D?{? ?Oft????D?qy?g2??T??/??1?H???-????/??B?????????F$?XA???dJP??)?????7??b'??WU!?.?g?)}????????{?????[A???]oE6??(?ELv
?!?n????????{??j????}?[????>L3#??x??a???vt?k?>8?!??ah2??s??,????p?C_????G??2>!:?YO_?2??.????5w?PO?Zlc5??/Om???~??a?xO?????N??[%Ac"I,?@???????W????(?{l? H?@?B/zX+;N3?mw?@=????$t? }-???N??????HlP?#???^,!??????V?N/????Y?r?<?u???7G???u)rS#??8?S65gR??Wc_T?2?r?.??/??^U}?2d??J?[??????H	>??L}?:?????8???%??
E?4j???zA?+???&}U>??~?3w?g???o??}???|?8?c
??8?NA????&j???t}@???M?0;?????(???s??	?T?7]??&6kI???	[R?(???????&|s?Z??lV??d,??e??????v???cI@?*???CG 9???H7?m???!)????voY?i??_n??_NCh??1c-Q"G+?8?4Hg??pfV?K??????h?1??l3?J~Y????E????7u#??uX??????W?m??e?6?R_??$?????%??Nv+?????+??/s1&*k????W2
$??S?Tm?!??<vI'2????[5??????6?Y??u??Qy,?7??*?D????G????:???:5XC??S?x?Zde???
?a??"73?W
?t?p
&m?????4R???>?e<????L?D?????????=????
?x$?.jIxRUYT{B?Cz;??}
&??b`??"gJi????>]?v??????)? ~tXR"z??????5'?%??1?????{??r??u?:[?R
??#?????2w=m????`ygH?x?hf??@??ea???)?{]"?t??J??-?????????E???6??)
M??k?Qq*>?G???k?NF??M????bh?7N{?~?W8????8???R?_?o?n????U???g?B:{??l*????M??z?{?n???{`??Ki??]P+????[&?Cn?kNc?D>?;?A^*h??????M??MA??
fQg??v?h??????0s;[I?;[f??????.$??&AR???t?-[?R?y??P??????6(?d(?3???yH6H,V???????M%MW??)???C?MX???n7??5??vs?iaG}?-W?????[|???!???? d}?)??d??o??g??????[[!f/?g?]?:'&S???T%??9???}r?Y?2???T1?B?iS??(\?????zg???M?[?)8?C?? MnN???R?W?H?	????????p?U-???Y^*+????	?{??8?)????p?L??y6T????????],m2E??FS?k?
?m????H?J?o?2h
Da_f???????\WOd+?;[?1?7]???F??????"H?/P?Wwj?!?W???*0??h??(W??H?pZ?~.???y>?A??mO?#?7?=i????*=??FF?(#????DVcs?]??a7	?
`k?L???a??? ,?}??T??U???rWb7,??p6??.8???N]??;(?'?{J??(g?4??B7#?*(?^?Xb#2B??????????+??g?[U;???c????po8?Wz??v?N]??qY?d???6m?{?aIy??L?u,fvJ??(?\????Wk;4??r??$G;?K????|
k$????H??e????8?qg???9???3??m??*E??A??X??=???]?7?$???D???8	]????7??.b??????l???l????J

P r??]?G????1?????p??yY=XY@V2:<??3?*??Z????K%?x6CFO&???\(?xK??K????????????????????? |	????y??~????????Qz??k??BY??Y?????h????&%Sh"?
h4??N#??0N9g?j???'o????Gf?f????f?j`Kh?5?**??|h????j61???sV???@?_k??!`0.???Q?-?@?)#?d?-<??????=&????i???:v??????1?%?(?'???x???f??a??So:????
???!?2?????~Pm1?????[E!C???<3??E??c?$#??bfeGg??????H?|??????.?rU9??????Z???b???4?q?+?7????pQS,?????????~?????g?U?YR??%??k#?????!RI(V??=rO<XXTZ?
^?`c&h(????RzZ?v?A0e????g??8???1?C3V?o?,! ?H4???^ha ?:r?#?k??d?^???U?2L??[?????w#h???(?{?$9(?hK?pdB???G?6?*	?1??5????F(;?B2??5???+c????X?/315?*y??!?z?3Z0??8xR/???y
h?O???k)??l<?E??o@#vW?im%??q3KLR?|_vV3j(???????XF??{?L?V???k???????F?!gheKIx?<<{?????P?Z??YS?/X???A>?T????????????W????-?}???qlI?????N??-LE??HW_?I??u?I??|?b?WL44??HV
$?U????Q???t???X?i???#??2{???gc.?=?I???g???)g?\??
r????t?7 S?Mw?4?mR???b?t3??R?j?._+6??KOM?y??c@???s0V??:>? ????????Q? ?D??'3<? |? ???_??????M???B??]L?r&??i???Ynq?<Ue??h}???????#`?>?EuF??+Pf,????Ku?8??Hc????q?^@	Y/???q??$?S???x???S?p
??T?