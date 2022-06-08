t this point we're no longer running the old task and
	 * sending an IPI (below) ensures the sibling will no longer be running
	 * their task. This ensures there is no inter-sibling overlap between
	 * non-matching user state.
	 */
	for_each_cpu(i, smt_mask) {
		rq_i = cpu_rq(i);

		/*
		 * An online sibling might have gone offline before a task
		 * could be picked for it, or it might be offline but later
		 * happen to come online, but its too late and nothing was
		 * picked for it.  That's Ok - it will pick tasks for itself,
		 * so ignore it.
		 */
		if (!rq_i->core_pick)
			continue;

		/*
		 * Update for new !FI->FI transitions, or if continuing to be in !FI:
		 * fi_before     fi      update?
		 *  0            0       1
		 *  0            1       1
		 *  1            0       1
		 *  1            1       0
		 */
		if (!(fi_before && rq->core->core_forceidle_count))
			task_vruntime_update(rq_i, rq_i->core_pick, !!rq->core->core_forceidle_count);

		rq_i->core_pick->core_occupation = occ;

		if (i == cpu) {
			rq_i->core_pick = NULL;
			continue;
		}

		/* Did we break L1TF mitigation requirements? */
		WARN_ON_ONCE(!cookie_match(next, rq_i->core_pick));

		if (rq_i->curr == rq_i->core_pick) {
			rq_i->core_pick = NULL;
			continue;
		}

		resched_curr(rq_i);
	}

out_set_next:
	set_next_task(rq, next);
out:
	if (rq->core->core_forceidle_count && next == rq->idle)
		queue_core_balance(rq);

	return next;
}

static bool try_steal_cookie(int this, int that)
{
	struct rq *dst = cpu_rq(this), *src = cpu_rq(that);
	struct task_struct *p;
	unsigned long cookie;
	bool success = false;

	local_irq_disable();
	double_rq_lock(dst, src);

	cookie = dst->core->core_cookie;
	if (!cookie)
		goto unlock;

	if (dst->curr != dst->idle)
		goto unlock;

	p = sched_core_find(src, cookie);
	if (p == src->idle)
		goto unlock;

	do {
		if (p == src->core_pick || p == src->curr)
			goto next;

		if (!is_cpu_allowed(p, this))
			goto next;

		if (p->core_occupation > dst->idle->core_occupation)
			goto next;

		deactivate_task(src, p, 0);
		set_task_cpu(p, this);
		activate_task(dst, p, 0);

		resched_curr(dst);

		success = true;
		break;

next:
		p = sched_core_next(p, cookie);
	} while (p);

unlock:
	double_rq_unlock(dst, src);
	local_irq_enable();

	return success;
}

static bool steal_cookie_task(int cpu, struct sched_domain *sd)
{
	int i;

	for_each_cpu_wrap(i, sched_domain_span(sd), cpu) {
		if (i == cpu)
			continue;

		if (need_resched())
			break;

		if (try_steal_cookie(cpu, i))
			return true;
	}

	return false;
}

static void sched_core_balance(struct rq *rq)
{
	struct sched_domain *sd;
	int cpu = cpu_of(rq);

	preempt_disable();
	rcu_read_lock();
	raw_spin_rq_unlock_irq(rq);
	for_each_domain(cpu, sd) {
		if (need_resched())
			break;

		if (steal_cookie_task(cpu, sd))
			break;
	}
	raw_spin_rq_lock_irq(rq);
	rcu_read_unlock();
	preempt_enable();
}

static DEFINE_PER_CPU(struct callback_head, core_balance_head);

static void queue_core_balance(struct rq *rq)
{
	if (!sched_core_enabled(rq))
		return;

	if (!rq->core->core_cookie)
		return;

	if (!rq->nr_running) /* not forced idle */
		return;

	queue_balance_callback(rq, &per_cpu(core_balance_head, rq->cpu), sched_core_balance);
}

static void sched_core_cpu_starting(unsigned int cpu)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	struct rq *rq = cpu_rq(cpu), *core_rq = NULL;
	unsigned long flags;
	int t;

	sched_core_lock(cpu, &flags);

	WARN_ON_ONCE(rq->core != rq);

	/* if we're the first, we'll be our own leader */
	if (cpumask_weight(smt_mask) == 1)
		goto unlock;

	/* find the leader */
	for_each_cpu(t, smt_mask) {
		if (t == cpu)
			continue;
		rq = cpu_rq(t);
		if (rq->core == rq) {
			core_rq = rq;
			break;
		}
	}

	if (WARN_ON_ONCE(!core_rq)) /* whoopsie */
		goto unlock;

	/* install and validate core_rq */
	for_each_cpu(t, smt_mask) {
		rq = cpu_rq(t);

		if (t == cpu)
			rq->core = core_rq;

		WARN_ON_ONCE(rq->core != core_rq);
	}

unlock:
	sched_core_unlock(cpu, &flags);
}

static void sched_core_cpu_deactivate(unsigned int cpu)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	struct rq *rq = cpu_rq(cpu), *core_rq = NULL;
	unsigned long flags;
	int t;

	sched_core_lock(cpu, &flags);

	/* if we're the last man standing, nothing to do */
	if (cpumask_weight(smt_mask) == 1) {
		WARN_ON_ONCE(rq->core != rq);
		goto unlock;
	}

	/* if we're not the leader, nothing to do */
	if (rq->core != rq)
		goto unlock;

	/* find a new leader */
	for_each_cpu(t, smt_mask) {
		if (t == cpu)
			continue;
		core_rq = cpu_rq(t);
		break;
	}

	if (WARN_ON_ONCE(!core_rq)) /* impossible */
		goto unlock;

	/* copy the shared state to the new leader */
	core_rq->core_task_seq             = rq->core_task_seq;
	core_rq->core_pick_seq             = rq->core_pick_seq;
	core_rq->core_cookie               = rq->core_cookie;
	core_rq->core_forceidle_count      = rq->core_forceidle_count;
	core_rq->core_forceidle_seq        = rq->core_forceidle_seq;
	core_rq->core_forceidle_occupation = rq->core_forceidle_occupation;

	/*
	 * Accounting edge for forced idle is handled in pick_next_task().
	 * Don't need another one here, since the hotplug thread shouldn't
	 * have a cookie.
	 */
	core_rq->core_forceidle_start = 0;

	/* install new leader */
	for_each_cpu(t, smt_mask) {
		rq = cpu_rq(t);
		rq->core = core_rq;
	}

unlock:
	sched_core_unlock(cpu, &flags);
}

static inline void sched_core_cpu_dying(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (rq->core != rq)
		rq->core = rq;
}

#else /* !CONFIG_SCHED_CORE */

static inline void sched_core_cpu_starting(unsigned int cpu) {}
static inline void sched_core_cpu_deactivate(unsigned int cpu) {}
static inline void sched_core_cpu_dying(unsigned int cpu) {}

static struct task_struct *
pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	return __pick_next_task(rq, prev, rf);
}

#endif /* CONFIG_SCHED_CORE */

/*
 * Constants for the sched_mode argument of __schedule().
 *
 * The mode argument allows RT enabled kernels to differentiate a
 * preemption from blocking on an 'sleeping' spin/rwlock. Note that
 * SM_MASK_PREEMPT for !RT has all bits set, which allows the compiler to
 * optimize the AND operation out and just check for zero.
 */
#define SM_NONE			0x0
#define SM_PREEMPT		0x1
#define SM_RTLOCK_WAIT		0x2

#ifndef CONFIG_PREEMPT_RT
# define SM_MASK_PREEMPT	(~0U)
#else
# define SM_MASK_PREEMPT	SM_PREEMPT
#endif

/*
 * __schedule() is the main scheduler function.
 *
 * The main means of driving the scheduler and thus entering this function are:
 *
 *   1. Explicit blocking: mutex, semaphore, waitqueue, etc.
 *
 *   2. TIF_NEED_RESCHED flag is checked on interrupt and userspace return
 *      paths. For example, see arch/x86/entry_64.S.
 *
 *      To drive preemption between tasks, the scheduler sets the flag in timer
 *      interrupt handler scheduler_tick().
 *
 *   3. Wakeups don't really cause entry into schedule(). They add a
 *      task to the run-queue and that's it.
 *
 *      Now, if the new task added to the run-queue preempts the current
 *      task, then the wa