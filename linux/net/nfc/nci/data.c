_wake_remote);
				break;
			}
		}
		rcu_read_unlock();
	}

	if (wake_flags & WF_MIGRATED)
		__schedstat_inc(p->stats.nr_wakeups_migrate);
#endif /* CONFIG_SMP */

	__schedstat_inc(rq->ttwu_count);
	__schedstat_inc(p->stats.nr_wakeups);

	if (wake_flags & WF_SYNC)
		__schedstat_inc(p->stats.nr_wakeups_sync);
}

/*
 * Mark the task runnable and perform wakeup-preemption.
 */
static void ttwu_do_wakeup(struct rq *rq, struct task_struct *p, int wake_flags,
			   struct rq_flags *rf)
{
	check_preempt_curr(rq, p, wake_flags);
	WRITE_ONCE(p->__state, TASK_RUNNING);
	trace_sched_wakeup(p);

#ifdef CONFIG_SMP
	if (p->sched_class->task_woken) {
		/*
		 * Our task @p is fully woken up and running; so it's safe to
		 * drop the rq->lock, hereafter rq is only used for statistics.
		 */
		rq_unpin_lock(rq, rf);
		p->sched_class->task_woken(rq, p);
		rq_repin_lock(rq, rf);
	}

	if (rq->idle_stamp) {
		u64 delta = rq_clock(rq) - rq->idle_stamp;
		u64 max = 2*rq->max_idle_balance_cost;

		update_avg(&rq->avg_idle, delta);

		if (rq->avg_idle > max)
			rq->avg_idle = max;

		rq->wake_stamp = jiffies;
		rq->wake_avg_idle = rq->avg_idle / 2;

		rq->idle_stamp = 0;
	}
#endif
}

static void
ttwu_do_activate(struct rq *rq, struct task_struct *p, int wake_flags,
		 struct rq_flags *rf)
{
	int en_flags = ENQUEUE_WAKEUP | ENQUEUE_NOCLOCK;

	lockdep_assert_rq_held(rq);

	if (p->sched_contributes_to_load)
		rq->nr_uninterruptible--;

#ifdef CONFIG_SMP
	if (wake_flags & WF_MIGRATED)
		en_flags |= ENQUEUE_MIGRATED;
	else
#endif
	if (p->in_iowait) {
		delayacct_blkio_end(p);
		atomic_dec(&task_rq(p)->nr_iowait);
	}

	activate_task(rq, p, en_flags);
	ttwu_do_wakeup(rq, p, wake_flags, rf);
}

/*
 * Consider @p being inside a wait loop:
 *
 *   for (;;) {
 *      set_current_state(TASK_UNINTERRUPTIBLE);
 *
 *      if (CONDITION)
 *         break;
 *
 *      schedule();
 *   }
 *   __set_current_state(TASK_RUNNING);
 *
 * between set_current_state() and schedule(). In this case @p is still
 * runnable, so all that needs doing is change p->state back to TASK_RUNNING in
 * an atomic manner.
 *
 * By taking task_rq(p)->lock we serialize against schedule(), if @p->on_rq
 * then schedule() must still happen and p->state can be changed to
 * TASK_RUNNING. Otherwise we lost the race, schedule() has happened, and we
 * need to do a full wakeup with enqueue.
 *
 * Returns: %true when the wakeup is done,
 *          %false otherwise.
 */
static int ttwu_runnable(struct task_struct *p, int wake_flags)
{
	struct rq_flags rf;
	struct rq *rq;
	int ret = 0;

	rq = __task_rq_lock(p, &rf);
	if (task_on_rq_queued(p)) {
		/* check_preempt_curr() may use rq clock */
		update_rq_clock(rq);
		ttwu_do_wakeup(rq, p, wake_flags, &rf);
		ret = 1;
	}
	__task_rq_unlock(rq, &rf);

	return ret;
}

#ifdef CONFIG_SMP
void sched_ttwu_pending(void *arg)
{
	struct llist_node *llist = arg;
	struct rq *rq = this_rq();
	struct task_struct *p, *t;
	struct rq_flags rf;

	if (!llist)
		return;

	/*
	 * rq::ttwu_pending racy indication of out-standing wakeups.
	 * Races such that false-negatives are possible, since they
	 * are shorter lived that false-positives would be.
	 */
	WRITE_ONCE(rq->ttwu_pending, 0);

	rq_lock_irqsave(rq, &rf);
	update_rq_clock(rq);

	llist_for_each_entry_safe(p, t, llist, wake_entry.llist) {
		if (WARN_ON_ONCE(p->on_cpu))
			smp_cond_load_acquire(&p->on_cpu, !VAL);

		if (WARN_ON_ONCE(task_cpu(p) != cpu_of(rq)))
			set_task_cpu(p, cpu_of(rq));

		ttwu_do_activate(rq, p, p->sched_remote_wakeup ? WF_MIGRATED : 0, &rf);
	}

	rq_unlock_irqrestore(rq, &rf);
}

void send_call_function_single_ipi(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (!set_nr_if_polling(rq->idle))
		arch_send_call_function_single_ipi(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

/*
 * Queue a task on the target CPUs wake_list and wake the CPU via IPI if
 * necessary. The wakee CPU on receipt of the IPI will queue the task
 * via sched_ttwu_wakeup() for activation so the wakee incurs the cost
 * of the wakeup instead of the waker.
 */
static void __ttwu_queue_wakelist(struct task_struct *p, int cpu, int wake_flags)
{
	struct rq *rq = cpu_rq(cpu);

	p->sched_remote_wakeup = !!(wake_flags & WF_MIGRATED);

	WRITE_ONCE(rq->ttwu_pending, 1);
	__smp_call_single_queue(cpu, &p->wake_entry.llist);
}

void wake_up_if_idle(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	rcu_read_lock();

	if (!is_idle_task(rcu_dereference(rq->curr)))
		goto out;

	rq_lock_irqsave(rq, &rf);
	if (is_idle_task(rq->curr))
		resched_curr(rq);
	/* Else CPU is not idle, do nothing here: */
	rq_unlock_irqrestore(rq, &rf);

out:
	rcu_read_unlock();
}

bool cpus_share_cache(int this_cpu, int that_cpu)
{
	if (this_cpu == that_cpu)
		return true;

	return per_cpu(sd_llc_id, this_cpu) == per_cpu(sd_llc_id, that_cpu);
}

static inline bool ttwu_queue_cond(int cpu, int wake_flags)
{
	/*
	 * Do not complicate things with the async wake_list while the CPU is
	 * in hotplug state.
	 */
	if (!cpu_active(cpu))
		return false;

	/*
	 * If the CPU does not share cache, then queue the task on the
	 * remote rqs wakelist to avoid accessing remote data.
	 */
	if (!cpus_share_cache(smp_processor_id(), cpu))
		return true;

	/*
	 * If the task is descheduling and the only running task on the
	 * CPU then use the wakelist to offload the task activation to
	 * the soon-to-be-idle CPU as the current CPU is likely busy.
	 * nr_running is checked to avoid unnecessary task stacking.
	 */
	if ((wake_flags & WF_ON_CPU) && cpu_rq(cpu)->nr_running <= 1)
		return true;

	return false;
}

static bool ttwu_queue_wakelist(struct task_struct *p, int cpu, int wake_flags)
{
	if (sched_feat(TTWU_QUEUE) && ttwu_queue_cond(cpu, wake_flags)) {
		if (WARN_ON_ONCE(cpu == smp_processor_id()))
			return false;

		sched_clock_cpu(cpu); /* Sync clocks across CPUs */
		__ttwu_queue_wakelist(p, cpu, wake_flags);
		return true;
	}

	return false;
}

#else /* !CONFIG_SMP */

static inline bool ttwu_queue_wakelist(struct task_struct *p, int cpu, int wake_flags)
{
	return false;
}

#endif /* CONFIG_SMP */

static void ttwu_queue(struct task_struct *p, int cpu, int wake_flags)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	if (ttwu_queue_wakelist(p, cpu, wake_flags))
		return;

	rq_lock(rq, &rf);
	update_rq_clock(rq);
	ttwu_do_activate(rq, p, wake_flags, &rf);
	rq_unlock(rq, &rf);
}

/*
 * Invoked from try_to_wake_up() to check whether the task can be woken up.
 *
 * The caller holds p::pi_lock if p != current or has preemption
 * disabled when p == current.
 *
 * The rules of PREEMPT_RT saved_state:
 *
 *   The related locking code always holds p::pi_lock when updating
 *   p::saved_state, which means the code is fully serialized in both cases.
 *
 *   The lock wait and lock wakeups happen via TASK_RTLOCK_WAIT. No other
 *   bits set. This allows to distinguish all wakeup scenarios.
 */
static __always_inline
bool ttwu_state_match(struct task_struct *p, unsigned int state, int *success)
{
	if (IS_ENABLED(CONFIG_DEBUG_PREEMPT)) {
		WARN_ON_ONCE((state & TASK_RTLOCK_WAIT) &&
			     state != TASK_RTLOCK_WAIT);
	}

	if (READ_ONCE(p->__state) & 