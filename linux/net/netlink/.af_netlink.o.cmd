 int cpu, int wake_flags)
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

	if (READ_ONCE(p->__state) & state) {
		*success = 1;
		return true;
	}

#ifdef CONFIG_PREEMPT_RT
	/*
	 * Saved state preserves the task state across blocking on
	 * an RT lock.  If the state matches, set p::saved_state to
	 * TASK_RUNNING, but do not wake the task because it waits
	 * for a lock wakeup. Also indicate success because from
	 * the regular waker's point of view this has succeeded.
	 *
	 * After acquiring the lock the task will restore p::__state
	 * from p::saved_state which ensures that the regular
	 * wakeup is not lost. The restore will also set
	 * p::saved_state to TASK_RUNNING so any further tests will
	 * not result in false positives vs. @success
	 */
	if (p->saved_state & state) {
		p->saved_state = TASK_RUNNING;
		*success = 1;
	}
#endif
	return false;
}

/*
 * Notes on Program-Order guarantees on SMP systems.
 *
 *  MIGRATION
 *
 * The basic program-order guarantee on SMP systems is that when a task [t]
 * migrates, all its activity on its old CPU [c0] happens-before any subsequent
 * execution on its new CPU [c1].
 *
 * For migration (of runnable tasks) this is provided by the following means:
 *
 *  A) UNLOCK of the rq(c0)->lock scheduling out task t
 *  B) migration for t is required to synchronize *both* rq(c0)->lock and
 *     rq(c1)->lock (if not at the same time, then in that order).
 *  C) LOCK of the rq(c1)->lock scheduling in task
 *
 * Release/acquire chaining guarantees that B happens after A and C after B.
 * Note: the CPU doing B need not be c0 or c1
 *
 * Example:
 *
 *   CPU0            CPU1            CPU2
 *
 *   LOCK rq(0)->lock
 *   sched-out X
 *   sched-in Y
 *   UNLOCK rq(0)->lock
 *
 *                                   LOCK rq(0)->lock // orders against CPU0
 *                                   dequeue X
 *                                   UNLOCK rq(0)->lock
 *
 *                                   LOCK rq(1)->lock
 *                                   enqueue X
 *                                   UNLOCK rq(1)->lock
 *
 *                   LOCK rq(1)->lock // orders against CPU2
 *                   sched-out Z
 *                   sched-in X
 *                   UNLOCK rq(1)->lock
 *
 *
 *  BLOCKING -- aka. SLEEP + WAKEUP
 *
 * For blocking we (obviously) need to provide the same guarantee as for
 * migration. However the means are completely different as there is no lock
 * chain to provide order. Instead we do:
 *
 *   1) smp_store_release(X->on_cpu, 0)   -- finish_task()
 *   2) smp_cond_load_acquire(!X->on_cpu) -- try_to_wake_up()
 *
 * Example:
 *
 *   CPU0 (schedule)  CPU1 (try_to_wake_up) CPU2 (schedule)
 *
 *   LOCK rq(0)->lock LOCK X->pi_lock
 *   dequeue X
 *   sched-out X
 *   smp_store_release(X->on_cpu, 0);
 *
 *                    smp_cond_load_acquire(&X->on_cpu, !VAL);
 *                    X->state = WAKING
 *                    set_task_cpu(X,2)
 *
 *                    LOCK rq(2)->lock
 *                    enqueue X
 *                    X->state = RUNNING
 *                    UNLOCK rq(2)->lock
 *
 *                                          LOCK rq(2)->lock // orders against CPU1
 *                                          sched-out Z
 *                                          sched-in X
 *                                          UNLOCK rq(2)->lock
 *
 *                    UNLOCK X->pi_lock
 *   UNLOCK rq(0)->lock
 *
 *
 * However, for wakeups there is a second guarantee we must provide, namely we
 * must ensure that CONDITION=1 done by the caller can not be reordered with
 * accesses to the task state; see try_to_wake_up() and set_current_state().
 */

/**
 * try_to_wake_up - wake up a thread
 * @p: the thread to be awakened
 * @state: the mask of task states that can be woken
 * @wake_flags: wake modifier flags (WF_*)
 *
 * Conceptually does:
 *
 *   If (@state & @p->state) @p->state = TASK_RUNNING.
 *
 * If the task was not queued/runnable, also place it back on a runqueue.
 *
 * This function is atomic against schedule() which would dequeue the task.
 *
 * It issues a full memory barrier before accessing @p->state, see the comment
 * with set_current_state().
 *
 * Uses p->pi_lock to serialize against concurrent wake-ups.
 *
 * Relies on p->pi_lock stabilizing:
 *  - p->sched_class
 *  - p->cpus_ptr
 *  - p->sched_task_group
 * in order to do migration, see its use of select_task_rq()/set_task_cpu().
 *
 * Tries really hard to only take one task_rq(p)->lock for performance.
 * Takes rq->lock in:
 *  - ttwu_runnable()    -- old rq, unavoidable, see comment there;
 *  - ttwu_queue()       -- new rq, for enqueue of the task;
 *  - psi_ttwu_dequeue() -- much sadness :-( accounting will kill us.
 *
 * As a consequence we race really badly with just about everything. See the
 * many memory barriers and their comments for details.
 *
 * Return: %true if @p->state changes (an actual wakeup was done),
 *	   %false otherwise.
 */
static int
try_to_wake_up(struct task_struct *p, unsigned int state, int wake_flags)
{
	unsigned long flags;
	int cpu, success = 0;

	preempt_disable();
	if (p == current) {
		/*
		 * We're waking current, this means 'p->on_rq' and 'task_cpu(p)
		 * == smp_processor_id()'. Together this means we can special
		 * case the whole 'p->on_rq && ttwu_runnable()' case below
		 * without taking any locks.
		 *
		 * In particular:
		 *  - we rely on Program-Order guarantees for all the ordering,
		 *  - we're serialized against set_special_state() by virtue of
		 *    it disabling IRQs (this allows not taking ->pi_lock).
		 */
		if (!ttwu_state_match(p, state, &success))
			goto out;

		trace_sched_waking(p);
		WRITE_ONCE(p->__state, TASK_RUNNING);
		trace_sched_wakeup(p);
		goto out;
	}

	/*
	 * If we are going to wake up a thread waiting for CONDITION we
	 * need to ensure that CONDITION=1 done by the caller can not be
	 * reordered with p->state check below. This pairs with smp_store_mb()
	 * in set_current_state() that the waiting thread does.
	 */
	raw_spin_lock_irqsave(&p->pi_lock, flags);
	smp_mb__after_spinlock();
	if (!ttwu_state_match(p, state, &success))
		goto unlock;

	trace_sched_waking(p);

	/*
	 * Ensure we load p->on_rq _after_ p->state, otherwise it would
	 * be possible to, falsely, observe p->on_rq == 0 and get stuck
	 * in smp_cond_load_acquire() below.
	 *
	 * sched_ttwu_pending()			try_to_wake_up()
	 *   STORE p->on_rq = 1			  LOAD p->state
	 *   UNLOCK rq->lock
	 *
	 * __schedule() (switch to task 'p')
	 *   LOCK rq->lock			  smp_rmb();
	 *   smp_mb__after_spinlock();
	 *   UNLOCK rq->lock
	 *
	 * [task p]
	 *   STORE p->state = UNINTERRUPTIBLE	  LOAD p->on_rq
	 *
	 * Pairs with the LOCK+smp_mb__after_spinlock() on rq->lock in
	 * __schedule().  See the comment for smp_mb__after_spinlock().
	 *
	 * A similar smb_rmb() lives in try_invoke_on_locked_down_task().
	 */
	smp_rmb();
	if (READ_ONCE(p->on_rq) && ttwu_runnable(p, wake_flags))
		goto unlock;

#ifdef CONFIG_SMP
	/*
	 * Ensure we load p->on_cpu _after_ p->on_rq, otherwise it would be
	 * possible to, falsely, observe p->on_cpu == 0.
	 *
	 * One must be running (->on_cpu == 1) in order to remove oneself
	 * from the runqueue.
	 *
	 * __schedule() (switch to task 'p')	try_to_wake_up()
	 *   STORE p->on_cpu = 1		  LOAD p->on_rq
	 *   UNLOCK rq->lock
	 *
	 * __schedule() (put 'p' to sleep)
	 *   LOCK rq->lock			  smp_rmb();
	 *   smp_mb__after_spinlock();
	 *   STORE p->on_rq = 0			  LOAD p->on_cpu
	 *
	 * Pairs with the LOCK+smp_mb__after_spinlock() on rq->lock in
	 * __schedule().  See the comment for smp_mb__after_spinlock().
	 *
	 * Form a control-dep-acquire with p->on_rq == 0 above, to ensure
	 * schedule()'s deactivate_task() has 'happened' and p will no longer
	 * care about it's own p->state. See the comment in __schedule().
	 */
	smp_acquire__after_ctrl_dep();

	/*
	 * We're doing the wakeup (@success == 1), they did a dequeue (p->on_rq
	 * == 0), which means we need to do an enqueue, change p->state to
	 * TASK_WAKING such that we can unlock p->pi_lock before doing the
	 * enqueue, such as ttwu_queue_wakelist().
	 */
	WRITE_ONCE(p->__state, TASK_WAKING);

	/*
	 * If the owning (remote) CPU is still in the middle of schedule() with
	 * this task as prev, considering queueing p on the remote CPUs wake_list
	 * which potentially sends an IPI instead of spinning on p->on_cpu to
	 * let the waker make forward progress. This is safe because IRQs are
	 * disabled and the IPI will deliver after on_cpu is cleared.
	 *
	 * Ensure we load task_cpu(p) after p->on_cpu:
	 *
	 * set_task_cpu(p, cpu);
	 *   STORE p->cpu = @cpu
	 * __schedule() (switch to task 'p')
	 *   LOCK rq->lock
	 *   smp_mb__after_spin_lock()		smp_cond_load_acquire(&p->on_cpu)
	 *   STORE p->on_cpu = 1		LOAD p->cpu
	 *
	 * to ensure we observe the correct CPU on which the task is currently
	 * scheduling.
	 */
	if (smp_load_acquire(&p->on_cpu) &&
	    ttwu_queue_wakelist(p, task_cpu(p), wake_flags | WF_ON_CPU))
		goto unlock;

	/*
	 * If the owning (remote) CPU is still in the middle of schedule() with
	 * this task as prev, wait until it's done referencing the task.
	 *
	 * Pairs with the smp_store_release() in finish_task().
	 *
	 * This ensures that tasks getting woken will be fully ordered against
	 * their previous state and preserve Program Order.
	 */
	smp_cond_load_acquire(&p->on_cpu, !VAL);

	cpu = select_task_rq(p, p->wake_cpu, wake_flags | WF_TTWU);
	if (task_cpu(p) != cpu) {
		if (p->in_iowait) {
			delayacct_blkio_end(p);
			atomic_dec(&task_rq(p)->nr_iowait);
		}

		wake_flags |= WF_MIGRATED;
		psi_ttwu_dequeue(p);
		set_task_cpu(p, cpu);
	}
#else
	cpu = task_cpu(p);
#endif /* CONFIG_SMP */

	ttwu_queue(p, cpu, wake_flags);
unlock:
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
out:
	if (success)
		ttwu_stat(p, task_cpu(p), wake_flags);
	preempt_enable();

	return success;
}

/**
 * task_call_func - Invoke a function on task in fixed state
 * @p: Process for which the function is to be invoked, can be @current.
 * @func: Function to invoke.
 * @arg: Argument to function.
 *
 * Fix the task in it's current state by avoiding wakeups and or rq operations
 * and call @func(@arg) on it.  This function can use ->on_rq and task_curr()
 * to work out what the state is, if required.  Given that @func can be invoked
 * with a runqueue lock held, it had better be quite lightweight.
 *
 * Returns:
 *   Whatever @func returns
 */
int task_call_func(struct task_struct *p, task_call_f func, void *arg)
{
	struct rq *rq = NULL;
	unsigned int state;
	struct rq_flags rf;
	int ret;

	raw_spin_lock_irqsave(&p->pi_lock, rf.flags);

	state = READ_ONCE(p->__state);

	/*
	 * Ensure we load p->on_rq after p->__state, otherwise it would be
	 * possible to, falsely, observe p->on_rq == 0.
	 *
	 * See try_to_wake_up() for a longer comment.
	 */
	smp_rmb();

	/*
	 * Since pi->lock blocks try_to_wake_up(), we don't need rq->lock when
	 * the task is blocked. Make sure to check @state since ttwu() can drop
	 * locks at the end, see ttwu_queue_wakelist().
	 */
	if (state == TASK_RUNNING || state == TASK_WAKING || p->on_rq)
		rq = __task_rq_lock(p, &rf);

	/*
	 * At this point the task is pinned; either:
	 *  - blocked and we're holding off wakeups	 (pi->lock)
	 *  - woken, and we're holding off enqueue	 (rq->lock)
	 *  - queued, and we're holding off schedule	 (rq->lock)
	 *  - running, and we're holding off de-schedule (rq->lock)
	 *
	 * The called function (@func) can use: task_curr(), p->on_rq and
	 * p->__state to differentiate between these states.
	 */
	ret = func(p, arg);

	if (rq)
		rq_unlock(rq, &rf);

	raw_spin_unlock_irqrestore(&p->pi_lock, rf.flags);
	return ret;
}

/**
 * wake_up_process - Wake up a specific process
 * @p: The process to be woken up.
 *
 * Attempt to wake up the nominated process and move it to the set of runnable
 * processes.
 *
 * Return: 1 if the process was woken up, 0 if it was already running.
 *
 * This function executes a full memory barrier before accessing the task state.
 */
int wake_up_process(struct task_struct *p)
{
	return try_to_wake_up(p, TASK_NORMAL, 0);
}
EXPORT_SYMBOL(wake_up_process);

int wake_up_state(struct task_struct *p, unsigned int state)
{
	return try_to_wake_up(p, state, 0);
}

/*
 * Perform scheduler related setup for a newly forked process p.
 * p is forked by current.
 *
 * __sched_fork() is basic setup used by init_idle() too:
 */
static void __sched_fork(unsigned long clone_flags, struct task_struct *p)
{
	p->on_rq			= 0;

	p->se.on_rq			= 0;
	p->se.exec_start		= 0;
	p->se.sum_exec_runtime		= 0;
	p->se.prev_sum_exec_runtime	= 0;
	p->se.nr_migrations		= 0;
	p->se.vruntime			= 0;
	INIT_LIST_HEAD(&p->se.group_node);

#ifdef CONFIG_FAIR_GROUP_SCHED
	p->se.cfs_rq			= NULL;
#endif

#ifdef CONFIG_SCHEDSTATS
	/* Even if schedstat is disabled, there should not be garbage */
	memset(&p->stats, 0, sizeof(p->stats));
#endif

	RB_CLEAR_NODE(&p->dl.rb_node);
	init_dl_task_timer(&p->dl);
	init_dl_inactive_task_timer(&p->dl);
	__dl_clear_params(p);

	INIT_LIST_HEAD(&p->rt.run_list);
	p->rt.timeout		= 0;
	p->rt.time_slice	= sched_rr_timeslice;
	p->rt.on_rq		= 0;
	p->rt.on_list		= 0;

#ifdef CONFIG_PREEMPT_NOTIFIERS
	INIT_HLIST_HEAD(&p->preempt_notifiers);
#endif

#ifdef CONFIG_COMPACTION
	p->capture_control = NULL;
#endif
	init_numa_balancing(clone_flags, p);
#ifdef CONFIG_SMP
	p->wake_entry.u_flags = CSD_TYPE_TTWU;
	p->migration_pending = NULL;
#endq;

	for (;;) {
		raw_spin_lock_irqsave(&p->pi_lock, rf->flags);
		rq = task_rq(p);
		raw_spin_rq_lock(rq);
		/*
		 *	move_queued_task()		task_rq_lock()
		 *
		 *	ACQUIRE (rq->lock)
		 *	[S] ->on_rq = MIGRATING		[L] rq = task_rq()
		 *	WMB (__set_task_cpu())		ACQUIRE (rq->lock);
		 *	[S] ->cpu = new_cpu		[L] task_rq()
		 *					[L] ->on_rq
		 *	RELEASE (rq->lock)
		 *
		 * If we observe the old CPU in task_rq_lock(), the acquire of
		 * the old rq->lock will fully serialize against the stores.
		 *
		 * If we observe the new CPU in task_rq_lock(), the address
		 * dependency headed by '[L] rq = task_rq()' and the acquire
		 * will pair with the WMB to ensure we then also see migrating.
		 */
		if (likely(rq == task_rq(p) && !task_on_rq_migrating(p))) {
			rq_pin_lock(rq, rf);
			return rq;
		}
		raw_spin_rq_unlock(rq);
		raw_spin_unlock_irqrestore(&p->pi_lock, rf->flags);

		while (unlikely(task_on_rq_migrating(p)))
			cpu_relax();
	}
}

/*
 * RQ-clock updating methods:
 */

static void update_rq_clock_task(struct rq *rq, s64 delta)
{
/*
 * In theory, the compile should just see 0 here, and optimize out the call
 * to sched_rt_avg_update. But I don't trust it...
 */
	s64 __maybe_unused steal = 0, irq_delta = 0;

#ifdef CONFIG_IRQ_TIME_ACCOUNTING
	irq_delta = irq_time_read(cpu_of(rq)) - rq->prev_irq_time;

	/*
	 * Since irq_time is only updated on {soft,}irq_exit, we might run into
	 * this case when a previous update_rq_clock() happened inside a
	 * {soft,}irq region.
	 *
	 * When this happens, we stop ->clock_task and only update the
	 * prev_irq_time stamp to account for the part that fit, so that a next
	 * update will consume the rest. This ensures ->clock_task is
	 * monotonic.
	 *
	 * It does however cause some slight miss-attribution of {soft,}irq
	 * time, a more accurate solution would be to update the irq_time using
	 * the current rq->clock timestamp, except that would require using
	 * atomic ops.
	 */
	if (irq_delta > delta)
		irq_delta = delta;

	rq->prev_irq_time += irq_delta;
	delta -= irq_delta;
#endif
#ifdef CONFIG_PARAVIRT_TIME_ACCOUNTING
	if (static_key_false((&paravirt_steal_rq_enabled))) {
		steal = paravirt_steal_clock(cpu_of(rq));
		steal -= rq->prev_steal_time_rq;

		if (unlikely(steal > delta))
			steal = delta;

		rq->prev_steal_time_rq += steal;
		delta -= steal;
	}
#endif

	rq->clock_task += delta;

#ifdef CONFIG_HAVE_SCHED_AVG_IRQ
	if ((irq_delta + steal) && sched_feat(NONTASK_CAPACITY))
		update_irq_load_avg(rq, irq_delta + steal);
#endif
	update_rq_clock_pelt(rq, delta);
}

void update_rq_clock(struct rq *rq)
{
	s64 delta;

	lockdep_assert_rq_held(rq);

	if (rq->clock_update_flags & RQCF_ACT_SKIP)
		return;

#ifdef CONFIG_SCHED_DEBUG
	if (sched_feat(WARN_DOUBLE_CLOCK))
		SCHED_WARN_ON(rq->clock_update_flags & RQCF_UPDATED);
	rq->clock_update_flags |= RQCF_UPDATED;
#endif

	delta = sched_clock_cpu(cpu_of(rq)) - rq->clock;
	if (delta < 0)
		return;
	rq->clock += delta;
	update_rq_clock_task(rq, delta);
}

#ifdef CONFIG_SCHED_HRTICK
/*
 * Use HR-timers to deliver accurate preemption points.
 */

static void hrtick_clear(struct rq *rq)
{
	if (hrtimer_active(&rq->hrtick_timer))
		hrtimer_cancel(&rq->hrtick_timer);
}

/*
 * High-resolution timer tick.
 * Runs from hardirq context with interrupts disabled.
 */
static enum hrtimer_restart hrtick(struct hrtimer *timer)
{
	struct rq *rq = container_of(timer, struct rq, hrtick_timer);
	struct rq_flags rf;

	WARN_ON_ONCE(cpu_of(rq) != smp_processor_id());

	rq_lock(rq, &rf);
	update_rq_clock(rq);
	rq->curr->sched_class->task_tick(rq, rq->curr, 1);
	rq_unlock(rq, &rf);

	return HRTIMER_NORESTART;
}

#ifdef CONFIG_SMP

static void __hrtick_restart(struct rq *rq)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	ktime_t time = rq->hrtick_time;

	hrtimer_start(timer, time, HRTIMER_MODE_ABS_PINNED_HARD);
}

/*
 * called from hardirq (IPI) context
 */
static void __hrtick_start(void *arg)
{
	struct rq *rq = arg;
	struct rq_flags rf;

	rq_lock(rq, &rf);
	__hrtick_restart(rq);
	rq_unlock(rq, &rf);
}

/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	s64 delta;

	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense and can cause timer DoS.
	 */
	delta = max_t(s64, delay, 10000LL);
	rq->hrtick_time = ktime_add_ns(timer->base->get_time(), delta);

	if (rq == this_rq())
		__hrtick_restart(rq);
	else
		smp_call_function_single_async(cpu_of(rq), &rq->hrtick_csd);
}

#else
/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense. Rely on vruntime for fairness.
	 */
	delay = max_t(u64, delay, 10000LL);
	hrtimer_start(&rq->hrtick_timer, ns_to_ktime(delay),
		      HRTIMER_MODE_REL_PINNED_HARD);
}

#endif /* CONFIG_SMP */

static void hrtick_rq_init(struct rq *rq)
{
#ifdef CONFIG_SMP
	INIT_CSD(&rq->hrtick_csd, __hrtick_start, rq);
#endif
	hrtimer_init(&rq->hrtick_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL_HARD);
	rq->hrtick_timer.function = hrtick;
}
#else	/* CONFIG_SCHED_HRTICK */
static inline void hrtick_clear(struct rq *rq)
{
}

static inline void hrtick_rq_init(struct rq *rq)
{
}
#endif	/* CONFIG_SCHED_HRTICK */

/*
 * cmpxchg based fetch_or, macro so it works for different integer types
 */
#define fetch_or(ptr, mask)						\
	({								\
		typeof(ptr) _ptr = (ptr);				\
		typeof(mask) _mask = (mask);				\
		typeof(*_ptr) _old, _val = *_ptr;			\
									\
		for (;;) {						\
			_old = cmpxchg(_ptr, _val, _val | _mask);	\
			if (_old == _val)				\
				break;					\
			_val = _old;					\
		}							\
	_old;								\
})

#if defined(CONFIG_SMP) && defined(TIF_POLLING_NRFLAG)
/*
 * Atomically set TIF_NEED_RESCHED and test for TIF_POLLING_NRFLAG,
 * this avoids any races wrt polling state changes and thereby avoids
 * spurious IPIs.
 */
static bool set_nr_and_not_polling(struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	return !(fetch_or(&ti->flags, _TIF_NEED_RESCHED) & _TIF_POLLING_NRFLAG);
}

/*
 * Atomically set TIF_NEED_RESCHED if TIF_POLLING_NRFLAG is set.
 *
 * If this returns true, then the idle task promises to call
 * sched_ttwu_pending() and reschedule soon.
 */
static bool set_nr_if_polling(struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	typeof(ti->flags) old, val = READ_ONCE(ti->flags);

	for (;;) {
		if (!(val & _TIF_POLLING_NRFLAG))
			return false;
		if (val & _TIF_NEED_RESCHED)
			return true;
		old = cmpxchg(&ti->flags, val, val | _TIF_NEED_RESCHED);
		if (old == val)
			break;
		val = old;
	}
	return true;
}

#else
static bool set_nr_and_not_polling(struct task_struct *p)
{
	set_tsk_need_resched(p);
	return true;
}

#ifdef CONFIG_SMP
static bool set_nr_if_polling(struct task_struct *p)
{
	return false;
}
#endif
#endif

static bool __wake_q_add(struct wake_q_head *head, struct task_struct *task)
{
	struct wake_q_node *node = &task->wake_q;

	/*
	 * Atomically grab the task, if ->wake_q is !nil already it means
	 * it's already queued (either by us or someone else) and will get the
	 * wakeup due to that.
	 *
	 * In order to ensure that a pending wakeup will observe our pending
	 * state, even in the failed case, an explicit smp_mb() must be used.
	 */
	smp_mb__before_atomic();
	if (unlikely(cmpxchg_relaxed(&node->next, NULL, WAKE_Q_TAIL)))
		return false;

	/*
	 * The head is context local, there can be no concurrency.
	 */
	*head->lastp = node;
	head->lastp = &node->next;
	return true;
}

/**
 * wake_q_add() - queue a wakeup for 'later' waking.
 * @head: the wake_q_head to add @task to
 * @task: the task to queue for 'later' wakeup
 *
 * Queue a task for later wakeup, most likely by the wake_up_q() call in the
 * same context, _HOWEVER_ this is not guaranteed, the wakeup can come
 * instantly.
 *
 * This function must be used as-if it were wake_up_process(); IOW the task
 * must be ready to be woken at this location.
 */
void wake_q_add(struct wake_q_head *head, struct task_struct *task)
{
	if (__wake_q_add(head, task))
		get_task_struct(task);
}

/**
 * wake_q_add_safe() - safely queue a wakeup for 'later' waking.
 * @head: the wake_q_head to add @task to
 * @task: the task to queue for 'later' wakeup
 *
 * Queue a task for later wakeup, most likely by the wake_up_q() call in the
 * same context, _HOWEVER_ this is not guaranteed, the wakeup can come
 * instantly.
 *
 * This function must be used as-if it were wake_up_process(); IOW the task
 * must be ready to be woken at this location.
 *
 * This function is essentially a task-safe equivalent to wake_q_add(). Callers
 * that already hold reference to @task can call the 'safe' version and trust
 * wake_q to do the right thing depending whether or not the @task is already
 * queued for wakeup.
 */
void wake_q_add_safe(struct wake_q_head *head, struct task_struct *task)
{
	if (!__wake_q_add(head, task))
		put_task_struct(task);
}

void wake_up_q(struct wake_q_head *head)
{
	struct wake_q_node *node = head->first;

	while (node != WAKE_Q_TAIL) {
		struct task_struct *task;

		task = container_of(node, struct task_struct, wake_q);
		/* Task can safely be re-inserted now: */
		node = node->next;
		task->wake_q.next = NULL;

		/*
		 * wake_up_process() executes a full barrier, which pairs with
		 * the queueing in wake_q_add() so as not to miss wakeups.
		 */
		wake_up_process(task);
		put_task_struct(task);
	}
}

/*
 * resched_curr - mark rq's current task 'to be rescheduled now'.
 *
 * On UP this means the setting of the need_resched flag, on SMP it
 * might also involve a cross-CPU call to trigger the scheduler on
 * the target CPU.
 */
void resched_curr(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	int cpu;

	lockdep_assert_rq_held(rq);

	if (test_tsk_need_resched(curr))
		return;

	cpu = cpu_of(rq);

	if (cpu == smp_processor_id()) {
		set_tsk_need_resched(curr);
		set_preempt_need_resched();
		return;
	}

	if (set_nr_and_not_polling(curr))
		smp_send_reschedule(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

void resched_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	unsigned long flags;

	raw_spin_rq_lock_irqsave(rq, flags);
	if (cpu_online(cpu) || cpu == smp_processor_id())
		resched_curr(rq);
	raw_spin_rq_unlock_irqrestore(rq, flags);
}

#ifdef CONFIG_SMP
#ifdef CONFIG_NO_HZ_COMMON
/*
 * In the semi idle case, use the nearest busy CPU for migrating timers
 * from an idle CPU.  This is good for power-savings.
 *
 * We don't do similar optimization for completely idle system, as
 * selecting an idle CPU will add more delays to the timers than intended
 * (as that CPU's timer base may not be uptodate wrt jiffies etc).
 */
int get_nohz_timer_target(void)
{
	int i, cpu = smp_processor_id(), default_cpu = -1;
	struct sched_domain *sd;
	const struct cpumask *hk_mask;

	if (housekeeping_cpu(cpu, HK_TYPE_TIMER)) {
		if (!idle_cpu(cpu))
			return cpu;
		default_cpu = cpu;
	}

	hk_mask = housekeeping_cpumask(HK_TYPE_TIMER);

	rcu_read_lock();
	for_each_domain(cpu, sd) {
		for_each_cpu_and(i, sched_domain_span(sd), hk_mask) {
			if (cpu == i)
				continue;

			if (!idle_cpu(i)) {
				cpu = i;
				goto unlock;
			}
		}
	}

	if (default_cpu == -1)
		default_cpu = housekeeping_any_cpu(HK_TYPE_TIMER);
	cpu = default_cpu;
unlock:
	rcu_read_unlock();
	return cpu;
}

/*
 * When add_timer_on() enqueues a timer into the timer wheel of an
 * idle CPU then this timer might expire before the next timer event
 * which is scheduled to wake up that CPU. In case of a completely
 * idle system the next event might even be infinite time into the
 * future. wake_up_idle_cpu() ensures that the CPU is woken up and
 * leaves the inner idle loop so the newly added timer is taken into
 * account when the CPU goes back to idle and evaluates the timer
 * wheel for the next timer event.
 */
static void wake_up_idle_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (cpu == smp_processor_id())
		return;

	if (set_nr_and_not_polling(rq->idle))
		smp_send_reschedule(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

static bool wake_up_full_nohz_cpu(int cpu)
{
	/*
	 * We just need the target to call irq_exit() and re-evaluate
	 * the next tick. The nohz full kick at least implies that.
	 * If needed we can still optimize that later with an
	 * empty IRQ.
	 */
	if (cpu_is_offline(cpu))
		return true;  /* Don't try to wake offline CPUs. */
	if (tick_nohz_full_cpu(cpu)) {
		if (cpu != smp_processor_id() ||
		    tick_nohz_tick_stopped())
			tick_nohz_full_kick_cpu(cpu);
		return true;
	}

	return false;
}

/*
 * Wake up the specified CPU.  If the CPU is going offline, it is the
 * caller's responsibility to deal with the lost wakeup, for example,
 * by hooking into the CPU_DEAD notifier like timers and hrtimers do.
 */
void wake_up_nohz_cpu(int cpu)
{
	if (!wake_up_full_nohz_cpu(cpu))
		wake_up_idle_cpu(cpu);
}

static void nohz_csd_func(void *info)
{
	struct rq *rq = info;
	int cpu = cpu_of(rq);
	unsigned int flags;

	/*
	 * Release the rq::nohz_csd.
	 */
	flags = atomic_fetch_andnot(NOHZ_KICK_MASK | NOHZ_NEWILB_KICK, nohz_flags(cpu));
	WARN_ON(!(flags & NOHZ_KICK_MASK));

	rq->idle_balance = idle_cpu(cpu);
	if (rq->idle_balance && !need_resched()) {
		rq->nohz_idle_balance = flags;
		raise_softirq_irqoff(SCHED_SOFTIRQ);
	}
}

#endif /* CONFIG_NO_HZ_COMMON */

#ifdef CONFIG_NO_HZ_FULL
bool sched_can_stop_tick(struct rq *rq)
{
	int fifo_nr_running;

	/* Deadline tasks, even if single, need the tick */
	if (rq->dl.dl_nr_running)
		return false;

	/*
	 * If there are more than one RR tasks, we need the tick to affect the
	 * actual RR behaviour.
	 */
	if (rq->rt.rr_nr_running) {
		if (rq->rt.rr_nr_running == 1)
			return true;
		else
			return false;
	}

	/*
	 * If there's no RR tasks, but FIFO tasks, we can skip the tick, no
	 * forced preemption between FIFO tasks.
	 */
	fifo_nr_running = rq->rt.rt_nr_running - rq->rt.rr_nr_running;
	if (fifo_nr_running)
		return true;

	/*
	 * If there are no DL,RR/FIFO tasks, there must only be CFS tasks left;
	 * if there's more than one we need the tick for involuntary
	 * preemption.
	 */
	if (rq->nr_running > 1)
		return false;

	return true;
}
#endif /* CONFIG_NO_HZ_FULL */
#endif /* CONFIG_SMP */

#if defined(CONFIG_RT_GROUP_SCHED) || (defined(CONFIG_FAIR_GROUP_SCHED) && \
			(defined(CONFIG_SMP) || defined(CONFIG_CFS_BANDWIDTH)))
/*
 * Iterate task_group tree rooted at *from, calling @down when first entering a
 * node and @up when leaving it for the final time.
 *
 * Caller must hold rcu_lock or sufficient equivalent.
 */
int walk_tg_tree_from(struct task_group *from,
			     tg_visitor down, tg_visitor up, void *data)
{
	struct task_group *parent, *child;
	int ret;

	parent = from;

down:
	ret = (*down)(parent, data);
	if (ret)
		goto out;
	list_for_each_entry_rcu(child, &parent->children, siblings) {
		parent = child;
		goto down;

up:
		continue;
	}
	ret = (*up)(parent, data);
	if (ret || parent == from)
		goto out;

	child = parent;
	parent = parent->parent;
	if (parent)
		goto up;
out:
	return ret;
}

int tg_nop(struct task_group *tg, void *data)
{
	return 0;
}
#endif

static void set_load_weight(struct task_struct *p, bool update_load)
{
	int prio = p->static_prio - MAX_RT_PRIO;
	struct load_weight *load = &p->se.load;

	/*
	 * SCHED_IDLE tasks get minimal weight:
	 */
	if (task_has_idle_policy(p)) {
		load->weight = scale_load(WEIGHT_IDLEPRIO);
		load->inv_weight = WMULT_IDLEPRIO;
		return;
	}

	/*
	 * SCHED_OTHER tasks have to update their load when changing their
	 * weight
	 */
	if (update_load && p->sched_class == &fair_sched_class) {
		reweight_task(p, prio);
	} else {
		load->weight = scale_load(sched_prio_to_weight[prio]);
		load->inv_weight = sched_prio_to_wmult[prio];
	}
}

#ifdef CONFIG_UCLAMP_TASK
/*
 * Serializes updates of utilization clamp values
 *
 * The (slow-path) user-space triggers utilization clamp value updates which
 * can require updates on (fast-path) scheduler's data structures used to
 * support enqueue/dequeue operations.
 * While the per-CPU rq lock protects fast-path update operations, user-space
 * requests are serialized using a mutex to reduce the risk of conflicting
 * updates or API abuses.
 */
static DEFINE_MUTEX(uclamp_mutex);

/* Max allowed minimum utilization */
unsigned int sysctl_sched_uclamp_util_min = SCHED_CAPACITY_SCALE;

/* Max allowed maximum utilization */
unsigned int sysctl_sched_uclamp_util_max = SCHED_CAPACITY_SCALE;

/*
 * By default RT tasks run at the maximum performance point/capacity of the
 * system. Uclamp enforces this by always setting UCLAMP_MIN of RT tasks to
 * SCHED_CAPACITY_SCALE.
 *
 * This knob allows admins to change the default behavior when uclamp is being
 * used. In battery powered devices, particularly, running at the maximum
 * capacity and frequency will increase energy consumption and shorten the
 * battery life.
 *
 * This knob only affects RT tasks that their uclamp_se->user_defined == false.
 *
 * This knob will not override the system default sched_util_clamp_min defined
 * above.
 */
unsigned int sysctl_sched_uclamp_util_min_rt_default = SCHED_CAPACITY_SCALE;

/* All clamps are required to be less or equal than these values */
static struct uclamp_se uclamp_default[UCLAMP_CNT];

/*
 * This static key is used to reduce the uclamp overhead in the fast path. It
 * primarily disables the call to uclamp_rq_{inc, dec}() in
 * enqueue/dequeue_task().
 *
 * This allows users to continue to enable uclamp in their kernel config with
 * minimum uclamp overhead in the fast path.
 *
 * As soon as userspace modifies any of the uclamp knobs, the static key is
 * enabled, since we have an actual users that make use of uclamp
 * functionality.
 *
 * The knobs that would enable this static key are:
 *
 *   * A task modifying its uclamp value with sched_setattr().
 *   * An admin modifying the sysctl_sched_uclamp_{min, max} via procfs.
 *   * An admin modifying the cgroup cpu.uclamp.{min, max}
 */
DEFINE_STATIC_KEY_FALSE(sched_uclamp_used);

/* Integer rounded range for each bucket */
#define UCLAMP_BUCKET_DELTA DIV_ROUND_CLOSEST(SCHED_CAPACITY_SCALE, UCLAMP_BUCKETS)

#define for_each_clamp_id(clamp_id) \
	for ((clamp_id) = 0; (clamp_id) < UCLAMP_CNT; (clamp_id)++)

static inline unsigned int uclamp_bucket_id(unsigned int clamp_value)
{
	return min_t(unsigned int, clamp_value / UCLAMP_BUCKET_DELTA, UCLAMP_BUCKETS - 1);
}

static inline unsigned int uclamp_none(enum uclamp_id clamp_id)
{
	if (clamp_id == UCLAMP_MIN)
		return 0;
	return SCHED_CAPACITY_SCALE;
}

static inline void uclamp_se_set(struct uclamp_se *uc_se,
				 unsigned int value, bool user_defined)
{
	uc_se->value = value;
	uc_se->bucket_id = uclamp_bucket_id(value);
	uc_se->user_defined = user_defined;
}

static inline unsigned int
uclamp_idle_value(struct rq *rq, enum uclamp_id clamp_id,
		  unsigned int clamp_value)
{
	/*
	 * Avoid blocked utilization pushing up the frequency when we go
	 * idle (which drops the max-clamp) by retaining the last known
	 * max-clamp.
	 */
	if (clamp_id == UCLAMP_MAX) {
		rq->uclamp_flags |= UCLAMP_FLAG_IDLE;
		return clamp_value;
	}

	return uclamp_none(UCLAMP_MIN);
}

static inline void uclamp_idle_reset(struct rq *rq, enum uclamp_id clamp_id,
				     unsigned int clamp_value)
{
	/* Reset max-clamp retention only on idle exit */
	if (!(rq->uclamp_flags & UCLAMP_FLAG_IDLE))
		return;

	WRITE_ONCE(rq->uclamp[clamp_id].value, clamp_value);
}

static inline
unsigned int uclamp_rq_max_value(struct rq *rq, enum uclamp_id clamp_id,
				   unsigned int clamp_value)
{
	struct uclamp_bucket *bucket = rq->uclamp[clamp_id].bucket;
	int bucket_id = UCLAMP_BUCKETS - 1;

	/*
	 * Since both min and max clamps are max aggregated, find the
	 * top most bucket with tasks in.
	 */
	for ( ; bucket_id >= 0; bucket_id--) {
		if (!bucket[bucket_id].tasks)
			continue;
		return bucket[bucket_id].value;
	}

	/* No tasks -- default clamp values */
	return uclamp_idle_value(rq, clamp_id, clamp_value);
}

static void __uclamp_update_util_min_rt_default(struct task_struct *p)
{
	unsigned int default_util_min;
	struct uclamp_se *uc_se;

	lockdep_assert_held(&p->pi_lock);

	uc_se = &p->uclamp_req[UCLAMP_MIN];

	/* Only sync if user didn't override the default */
	if (uc_se->user_defined)
		return;

	default_util_min = sysctl_sched_uclamp_util_min_rt_default;
	uclamp_se_set(uc_se, default_util_min, false);
}

static void uclamp_update_util_min_rt_default(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;

	if (!rt_task(p))
		return;

	/* Protect updates to p->uclamp_* */
	rq = task_rq_lock(p, &rf);
	__uclamp_update_util_min_rt_default(p);
	task_rq_unlock(rq, p, &rf);
}

static void uclamp_sync_util_min_rt_default(void)
{
	struct task_struct *g, *p;

	/*
	 * copy_process()			sysctl_uclamp
	 *					  uclamp_min_rt = X;
	 *   write_lock(&tasklist_lock)		  read_lock(&tasklist_lock)
	 *   // link thread			  smp_mb__after_spinlock()
	 *   write_unlock(&tasklist_lock)	  read_unlock(&tasklist_lock);
	 *   sched_post_fork()			  for_each_process_thread()
	 *     __uclamp_sync_rt()		    __uclamp_sync_rt()
	 *
	 * Ensures that either sched_post_fork() will observe the new
	 * uclamp_min_rt or for_each_process_thread() will observe the new
	 * task.
	 */
	read_lock(&tasklist_lock);
	smp_mb__after_spinlock();
	read_unlock(&tasklist_lock);

	rcu_read_lock();
	for_each_process_thread(g, p)
		uclamp_update_util_min_rt_default(p);
	rcu_read_unlock();
}

static inline struct uclamp_se
uclamp_tg_restrict(struct task_struct *p, enum uclamp_id clamp_id)
{
	/* Copy by value as we could modify it */
	struct uclamp_se uc_req = p->uclamp_req[clamp_id];
#ifdef CONFIG_UCLAMP_TASK_GROUP
	unsigned int tg_min, tg_max, value;

	/*
	 * Tasks in autogroups or root task group will be
	 * restricted by system defaults.
	 */
	if (task_group_is_autogroup(task_group(p)))
		return uc_req;
	if (task_group(p) == &root_task_group)
		return uc_req;

	tg_min = task_group(p)->uclamp[UCLAMP_MIN].value;
	tg_max = task_group(p)->uclamp[UCLAMP_MAX].value;
	value = uc_req.value;
	value = clamp(value, tg_min, tg_max);
	uclamp_se_set(&uc_req, value, false);
#endif

	return uc_req;
}

/*
 * The effective clamp bucket index of a task depends on, by increasing
 * priority:
 * - the task specific clamp value, when explicitly requested from userspace
 * - the task group effective clamp value, for tasks not either in the root
 *   group or in an autogroup
 * - the system default clamp value, defined by the sysadmin
 */
static inline struct uclamp_se
uclamp_eff_get(struct task_struct *p, enum uclamp_id clamp_id)
{
	struct uclamp_se uc_req = uclamp_tg_restrict(p, clamp_id);
	struct uclamp_se uc_max = uclamp_default[clamp_id];

	/* System default restrictions always apply */
	if (unlikely(uc_req.value > uc_max.value))
		return uc_max;

	return uc_req;
}

unsigned long uclamp_eff_value(struct task_struct *p, enum uclamp_id clamp_id)
{
	struct uclamp_se uc_eff;

	/* Task currently refcounted: use back-annotated (effective) value */
	if (p->uclamp[clamp_id].active)
		return (unsigned long)p->uclamp[clamp_id].value;

	uc_eff = uclamp_eff_get(p, clamp_id);

	return (unsigned long)uc_eff.value;
}

/*
 * When a task is enqueued on a rq, the clamp bucket currently defined by the
 * task's uclamp::bucket_id is refcounted on that rq. This also immediately
 * updates the rq's clamp value if required.
 *
 * Tasks can have a task-specific value requested from user-space, track
 * within each bucket the maximum value for tasks refcounted in it.
 * This "local max aggregation" allows to track the exact "requested" value
 * for each bucket when all its RUNNABLE tasks require the same clamp.
 */
static inline void uclamp_rq_inc_id(struct rq *rq, struct task_struct *p,
				    enum uclamp_id clamp_id)
{
	struct uclamp_rq *uc_rq = &rq->uclamp[clamp_id];
	struct uclamp_se *uc_se = &p->uclamp[clamp_id];
	struct uclamp_bucket *bucket;

	lockdep_assert_rq_held(rq);

	/* Update task effective clamp */
	p->uclamp[clamp_id] = uclamp_eff_get(p, clamp_id);

	bucket = &uc_rq->bucket[uc_se->bucket_id];
	bucket->tasks++;
	uc_se->active = true;

	uclamp_idle_reset(rq, clamp_id, uc_se->value);

	/*
	 * Local max aggregation: rq buckets always track the max
	 * "requested" clamp value of its RUNNABLE tasks.
	 */
	if (bucket->tasks == 1 || uc_se->value > bucket->value)
		bucket->value = uc_se->value;

	if (uc_se->value > READ_ONCE(uc_rq->value))
		WRITE_ONCE(uc_rq->value, uc_se->value);
}

/*
 * When a task is dequeued from a rq, the clamp bucket refcounted by the task
 * is released. If this is the last task reference counting the rq's max
 * active clamp value, then the rq's clamp value is updated.
 *
 * Both refcounted tasks and rq's cached clamp values are expected to be
 * always valid. If it's detected they are not, as defensive programming,
 * enforce the expected state and warn.
 */
static inline void uclamp_rq_dec_id(struct rq *rq, struct task_struct *p,
				    enum uclamp_id clamp_id)
{
	struct uclamp_rq *uc_rq = &rq->uclamp[clamp_id];
	struct uclamp_se *uc_se = &p->uclamp[clamp_id];
	struct uclamp_bucket *bucket;
	unsigned int bkt_clamp;
	unsigned int rq_clamp;

	lockdep_assert_rq_held(rq);

	/*
	 * If sched_uclamp_used was enabled after task @p was enqueued,
	 * we could end up with unbalanced call to uclamp_rq_dec_id().
	 *
	 * In this case the uc_se->active flag should be false since no uclamp
	 * accounting was performed at enqueue time and we can just return
	 * here.
	 *
	 * Need to be careful of the following enqueue/dequeue ordering
	 * problem too
	 *
	 *	enqueue(taskA)
	 *	// sched_uclamp_used gets enabled
	 *	enqueue(taskB)
	 *	dequeue(taskA)
	 *	// Must not decrement bucket->tasks here
	 *	dequeue(taskB)
	 *
	 * where we could end up with stale data in uc_se and
	 * bucket[uc_se->bucket_id].
	 *
	 * The following check here eliminates the possibility of such race.
	 */
	if (unlikely(!uc_se->active))
		return;

	bucket = &uc_rq->bucket[uc_se->bucket_id];

	SCHED_WARN_ON(!bucket->tasks);
	if (likely(bucket->tasks))
		bucket->tasks--;

	uc_se->active = false;

	/*
	 * Keep "local max aggregation" simple and accept to (possibly)
	 * overboost some RUNNABLE tasks in the same bucket.
	 * The rq clamp bucket value is reset to its base value whenever
	 * there are no more RUNNABLE tasks refcounting it.
	 */
	if (likely(bucket->tasks))
		return;

	rq_clamp = READ_ONCE(uc_rq->value);
	/*
	 * Defensive programming: this should never happen. If it happens,
	 * e.g. due to future modification, warn and fixup the expected value.
	 */
	SCHED_WARN_ON(bucket->value > rq_clamp);
	if (bucket->value >= rq_clamp) {
		bkt_clamp = uclamp_rq_max_value(rq, clamp_id, uc_se->value);
		WRITE_ONCE(uc_rq->value, bkt_clamp);
	}
}

static inline void uclamp_rq_inc(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_inc_id(rq, p, clamp_id);

	/* Reset clamp idle holding when there is one RUNNABLE task */
	if (rq->uclamp_flags & UCLAMP_FLAG_IDLE)
		rq->uclamp_flags &= ~UCLAMP_FLAG_IDLE;
}

static inline void uclamp_rq_dec(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_dec_id(rq, p, clamp_id);
}

static inline void uclamp_rq_reinc_id(struct rq *rq, struct task_struct *p,
				      enum uclamp_id clamp_id)
{
	if (!p->uclamp[clamp_id].active)
		return;

	uclamp_rq_dec_id(rq, p, clamp_id);
	uclamp_rq_inc_id(rq, p, clamp_id);

	/*
	 * Make sure to clear the idle flag if we've transiently reached 0
	 * active tasks on rq.
	 */
	if (clamp_id == UCLAMP_MAX && (rq->uclamp_flags & UCLAMP_FLAG_IDLE))
		rq->uclamp_flags &= ~UCLAMP_FLAG_IDLE;
}

static inline void
uclamp_update_active(struct task_struct *p)
{
	enum uclamp_id clamp_id;
	struct rq_flags rf;
	struct rq *rq;

	/*
	 * Lock the task and the rq where the task is (or was) queued.
	 *
	 * We might lock the (previous) rq of a !RUNNABLE task, but that's the
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
	/* Reset on sched cl�+v_���-��k��s6-�t��Ε7%�p"8��6�ݓ�!J]3�F��;��h��frL���Kl̩Oc��B��n�9����u�4�!0�!񺤃.�Q�y4���C^�(ӕ���^v:����`hd�n}���c��ǝ�L��.�|ny�u��&@�(�]���0| ��W�bg~U�XI��'�7�]%~Ox��7�z Zw��,7��@�xbD�� ��m�y�
�����櫉lt4�~����XU��uI!<�s�Iq:�Eri��əZ��p'�a*�Es.�,�	�?=�/\�i"��c�[#yW�2�&��!Gy�B$G2�O4���&�uWH�T�7��_B_�ي\x�Z)�l�I{���G)�a��w������3����lJG��ZQח�򃔨�R�M˱����
��8��&
�8R�E7%��b]�rmE~Ci9���c���DJVYkW��W���#�H���~���O5S@�Xj��_�f4���^� �5�����2b����2���]vj���Ȧ�e������ʹ�'I�Q�� ���\���=eo��{�*/&�=>��ȗX��%m9�{b/:��O� `(�%ᴓ�X��.(���C��BxZ�e��C��}�����R��W�s���p��w�$��MAʝ
Z��V�̂fe��}Ȳ]�xȫ��a*��y���S��'N��ǻ��$�-�Y��� �~�:ɸ^�Br�����ag��k��پ�����f�0f������l"�����b��Y�-S� '�������f}�@X���~�g���G�0�=0=NF+3'K��������j��7Be���آI��͘ss�yQ�m�~i�a�0d����Y��0�]U��Bd*r����:�%����_(�f�t0�G��6,��\���0�B���Ŝ���	`j��PX�Cw�^����J��XV���=r�C.�6��_T�"��W����$ҐnDE����[n�{�3,��o�b�t�7�UKկ�^�r�8��Kd
������٢ݖ��c���)M>�@pj�u!��.���'"dT��'�rF/��AV�M�֭�]�o�]�"�!��zԖ_EB�Z�w)~�:��PyG��OiJ�6��ᖝ&c�|I!G��P�\�kb����
����5�i�Yy~����.N�z�������?�f�0y!�{���8��^l��=�a�V�m �����.����	�("l��ڦ.4�$�O�I�l	xvT�z��i�p϶I�P��8bC]�������H6�5N$�xe��(ۆD��k\�FT�vᇨf�J�Q��U([wB�cɟw����9���~(b3��4�Y�;TB(�֙2o����
�s�A�QL��g�ə��aZ�?�HG|9m�EFqA2qx�%dN��r��~{VCp]�p^� ��_��b�c�\&��8	�S����d{;�'	r��^*L�FD�<g֟��#���h�℘y���Bǌ������S�)KU!���y
��c2r������l����x���
�MH���enM�� �{��{��#�\��˱��U_;����B��z0s�Ub.ė;0�`l�=~4c�A5��q߉@��A叓j|��GD-����N��h�s<sC��H�\X(CSKC���J���o��p	*1uE]+�7(77�l4�D	��o�E"q�(紌)����}�C3@�4E���73�����wQ_�f�F�(^T�>���u��t�=]�����_1��]��ܗ"��s���h��{~��thߣ�cJ�<6��A�k��~��Z��������8��� )L��k���(˼V|��/�Շ$b�NˑJ������N����'|OPƄS �L�����MW�49ǅǪ��}��9y"<G%�9�mĎ��u
e�&�n_��ȡR]h(ߪѻ�u�y���f�!j�_N�������u�{P[d�=7P�U����j����4	���F5�q	`Ԯ8�qv���J�})Tj�s_�js�@U;�ﵼ��Jd.�G�N�?�BUt��:XƌpY0�eV����sǐD�Ojy��E���%;#ʼ�퉠�6��fI?0L˂���ccVd�I������]{Bj^�oU&�V+U�O8Y��o������~���F^;�7;"(�U��Ȇ����)o��/���Sl�"eP^�1>v�Vχ��=�d�$�5�3i�
b�J�s𻍙�p�-�@���M8	�tՉO�G�D!���p��q�xG���ixS��
���B;t�t��Kg�����ȑi���q?�G����[�|���� ����#��|��0�ڕ�?|U��Q�~ � 1��6��+u����'�X�� %e��F�ϧc||��>�IULe"vO�tH��ֻ� �M��qc��/��+����#.���h�fEz��BN��V׀�F�[��<��Unau�pd����D�V��P�&�����%LP�DTI�tn�[>�-]9��N�yH-H���l���YNT�p3&���L�tI^(�C�7�����MqP�Z��Y-��΁aS���yؙF���Q�>!�.ZP'!Y'Z���'��²3+����T�[�}�I�l��E�W+��E�u-��i�^�+gI�-�wF�XL(�����"�����߈����g�7�|�>Y�o2���1�-��ׅ9w4��ۭ$�����<�:�xX�s*��JQ�}�ʽ\'�>e�2��?��p��#@�L	o`�����68��wqm���㝙���V��[l�fY�MK��Rs�Ռ^���h�31�3�i�����k���QP;Dr�Y�n�%�#�V��X=D�k�p�C�>dH(0P�M�=v|���}�4���dkM��u>?�����1O!�!:;_�7�B� �<K�ث�wq$��֬�+Ѩ<�Ɠ�;�h�L�����]HW���h[��^����W~1_���3Ze`�7�% }�G��@H8b�H�!�
��T��>e��_��٢��W~'�/.�Q �>Y�Gq�T��e�룩X��B[ 3%\����C� �cI<�\�x�	b��U����jQ���3C������.�5f��9n�46Gk��M$���Z\X`����`��&1�;��"�������_�.c���w�`������n���\�V'�"t���v�0����ں�����yE]���X�ծ��H� m������z[Q��>�6?��`��T��k�׺r����n����逢涄���y�P��ҜI���q�Z�/���i�6"=��)���_��3�F�c�t=$�J�4{
�V�T,&���r�ȅ%풘�!p��x+�L���s�U���Β��t���x!��舳�|OK��{�^�!u��)��?>��A�.��`ȑI� �!Ze�.�xxX��W�NC�~�e5���"$4[J�QD��I���B���3^�S�7)�#�]sd���kec�nJ��y��ЙH�z&ҫ��W0�d�L����]�
la��h�n�4P�D�m5��$��-�C��ƛ
d �����	����(����,�Hg�b2�7�N��4�Τ �<�%�)+�vY	��[F'�Mij�l�tS"㻹�:���Pb5]�5|Z�LTNs���=r,<��[�n�y�z��X���;t���?WwB�#Б���c9Ut���?��w�1�ЀO���x��Dw�2��!�H��_e�ɨ��T,G�˘��w}���ҭ�Uَ��,�[�Ǖ�]o6H��i9�� �挠y�9t�`�x�":��ϑ\J:ښG=���|�yR�@�|gT��%	袛t�K_��eB�x���A������b6���#G�mV?|N͔l���CJ����45N�'Fq\�Z�����|��U��!ͣԅuaQ�'yFN�����;XVL:��U�U��Y�L"�uu=`S�_�z�[�H���0����(�7��8��{��9ܧ�{��XZ�@�0�����6L���a>ܫ������_Ս�NkS�I ���)s�q*n��f���J�h4>��f���F{��J�%�]�^c�@-0�!�>�}@�����ą��'y`�j��0͢X���S��%���6"��v�W�'g�b���^Ⱥ!�B̀�	�}���!��Y�����6��~9�#r�z��ßB����u��#��n%��2L����h,љ�S���
�e�oC,	�^�H(�T�K�u�U�k%��Ƽ�#|��YB�7pdtEI�rFֿS�3.����vq!�آ����tz����F�B���@|?�Oéh�q���s)�a��G]x6W�[���aȑ]�3�-���#ws@�q����N��Lf��Z��5!B̟���iH��75�6�ޑG��Y$Vר��OE���"h��>ڎ)+���C��|=���Eq�E�"�,9�
Z�t��ao��sm[�NfT5��F�M�[�Ôik��)c���E;}ͽ����K�[3�H8�a@@w!�q��E_ݫ��V]�T��`�P�6K����s�{^X� ,E��ίaP�q�gs��%�1zF,��%5q<����)�
?�����`"4�F��.��g�66	��9\ڌ��=����mI��
ȹ�?�ʥ�V�W�p�S��C.����Q!N�1��"Y��o_b[�?�#�pB)�~��|\�p�!�c�9���I�gx���ߟ�8fw��KX��L�G_]F��3*oz\��&$a�"k2~��-ޞ�J�NM��G#�j!s	��d�?ZO	���mf5��n�?Qi-�E�}9l�Q�Y�yЮ�jI�b�;*�!���Eג��F�y��cGoc�$ QF;��Zr��ra���_�CE�h�
�k[0��H2#�~��6��l���ۻu㥫V��b`F5�g߾."T̎꽰�R�t�Q9_v��m}���l���ڋ�3c�=v�#�Ɓ�`8��tzK�r�z��ʿ���0�����P#��82�8w�����l�BM��P��`	Z�߷�8��D��L�Xc�����1�U�.`)������\�hF<Y������o�d��ƝS��EtF`vw:����CVO����`b�]��{�.Q�l,g���S�P �8�*�c��J���z��,������V'I�����+��"�+]x�������d�'���kN1����g.�´��� �y���Mr;��t82RҢ�M1����q\��G�H�� %Ƿm�,C><te3�����oΡ}�3�M	f�o ��u�n�F�Ԥʄ������r�
~g����A[�Ěl�?ne��(^;5.7�Z/1��1hm>bl�uG��T�#���>D~h>Y��p��͑Dq��n�'j��o�<���VΞ�%�i)��[>W]|<-�]"M�����ů�eFg�����@�|_�L��G|N%���8H3�~�n�_���)<��Xj��~wj^I �m���:I+@�=7g�����wP3H��Ɩ�v~���L�T
��Z�	/��&����ڞ���d�`����6׉;�����$��cl(�J��i�9���P/�e5����]��#2�¡Ϟ�(^Rw^b�K$|==���D�������P�*�>f�r�"�Q[>�M�������S��G�~���։�(�#%8�����^��t0*�;��%��0�M����J$8��x@)�+3O?~1�dFߺ�_� ���"RL�5�2x#���n��w�9��`�(
+�l�u*�t@����%"���n�t�ۇ���E�C�'oR�w U䱖Z85sUB���9��ʻ�	,�1C{��ˈ������n����Agz�̕�aDb)���P1� 2����=������!�zb�UA�Y�	MkXI(L�N~��&��p1v�Jx���o���nc���2^��;�#�sJ~�c5���pk桓���Z�	z���c?jܹ�Z���	�*$��(����U5���q�o�ΰ�]��>��-?r��:�� �aA%��Zyݤ6#��@o�KrF1c�1Q���*b�ϠŻt.���u^�,����5��0��T;��G=�r��:ƅ~ޡ��L���U��ث+뛑w�<&�Y��Y"����P�)��*��G�ӛ aZa�}+D�u�!��F��p�$L\rm���O!�����>�<v]�ୟX�V��_�̷њ�㐌��Z���J�&5Po�AD_�g�s�2M���P�>��d)��%���W<�e� 	!n)�����vu�#9�H�ՏT��Vh�r	��.*�X�pn�-F�����t�u�u�u�3�v�Ż�dR�:-?	����śR�]$���+:���o1� WD�
c-�C.|y�7���"�����G�y���<_�Q�h��f�16���ep��.���=���؏䩠m �F����J�0��kL�ZPf:)9��#��=q [ּ��3�щ�DX�zJ��������\��c�)��~x���� �+[y�e��5��x�<nsK��$N��
<[S=ƀQޓR��D���Yl�X��#>���UdEK����J���V���E��&��>�H6Mh]��s	�����(��l7��%��4M�e֫�$D&�a�
�^=�K�	Fo�Y]K����N�:-����`n��w�Z�+���±������y����'�C��.��=�#�}�gJ�
^[g��v$w�ۉ�Ov�l�}�?��òb�#'�tbl�C�H�ʪ��J6�a�[���s"����J?�+�	r�Gw����\π2�
��?��e�jW��u��{<��"��|~�O+�
���7ղ!c&�N��� Ir�҇}�$��A��V�D8}:��T?1T�9��|�'i`p��~��@	��u4=<\����O�x'xeC���X��,&wF��ݛ<��0K�넾܆��<I$���|��j؄�A�[[�Ho�j!��I�
ɢ���뎁����h�`�>^1C��VӘh�a���*$-{〱���yi"�<ޙ�%D`���W^��Cѷ;�Y< �L�Ǩ㩲��rCJ��OQ~c�J�͉���Z[~
I�����c�QXg���w1�贘t�ѯ�xomA�sӀR����QƦ�t�r�γf���r|��mF���O�����>�"-y\m�v�f7I�E�e>��=gD/"�n�KSy�j�(�o|t�F�m�4��{�2����N�7ʓ��6� �,�F�)�B6N����\kx���@ֈ'{,YȌQ糏��0����j�5�mď�� �C�M�Bp�;������l`2;eCw�������ۇ��W����.��������Fh;�%���;,أQ�_��"���3C������Q����Q�L7��m��7�z��|W����hY�Ϫ)����(��Mh8
gB@;],_G@:+I��N�%��n�XTn��6��ُ9�(:�@�O�v��;K�27႐��lw�ŏ�]�1���э�} w݌��@ֱWGW%aN�p��l"�R�7V�{2;���;8p����h\*�m�� ���l� �q�͹W�hhr���&N�c�W�q߷�³�g$�%���*�(�O�{c��	�Z���6R��K�@VlC.�$w�߁���@�%D���� *��ǟ�鷴�C��dw�۹�v������K���2=,6�0%�TG��F�U[r-\@"��c��Q �+'ϟ`�i�'B �_ʙ�UM;��JD��_��� R��$�*A��KU�gXz�X�軒-r:�'�	�k����?����j���tN.w*�R��5?ώ�	Z�!��=�O7��$�V<V8�4�B�����t�R�x�/�m�\r=�9r,���A�lM}��E�H$��ve&���Kς�_9��c���5��%�[�fɩ,`suPͥӀTVB����mWK�����S��o�����H�j�a?�����v��c��K�f#/A���:C_ڿٞI�9wy!���(�S0��R=������V��BZ��A��1~37���9�9���m��vgw>�$��&�4��3��I�a:(� �1:�Rx<�;��;ELF                      �2     4     (            GNU  �       �          �����WV�ƉЉ�S���   �|$��  ��������   ����  	؉�  ���      � 1ۀ�_������	Ë��   ��   �؉�������E����   �� ��   ��  ��������   ��   ��[^_�f������S�Áü  ��Hd�    �D$D��������D$,=9� �L  =7� �   =Ո �6  =ӈ �   =Q� �   =O� �   =� �
  =� �   =Wu �   ��  =9K �   ��  =�� �   ��  =�� �   ��  =�N �   ��  =L �   ��  =o> �   ��  =e> �   �w  =�8 �   �f  =77 �   �U  =�6 �   �D  =�6 �   �3  =�6 �   �"  =�4 �   �  =�4 �   �   ='3 �   =�2 ��   =�2 �?   =�0 �   ��   =�, �   ��   =�, �   ��   =�, �   ��   =�, �   ��   =9+ �   w}=? �   wp=+ �   = w^= �W   = �   wF= �   w9=� �   w,=� �   w=a �   w=V  �����=�U  wPSh    ��������   �o   ��&    �v ������Q�UWVS����   �8�΅���   ���  ��w��ws[��^_]Ã���   ��,��   w��*u������   ���   ��  �
!ȉ��   ��    �X�A �������u��   ��  ��  	ŉ*[��^_]ú   ����`2 u���u@���p���������   뎍v ������u����P�����&    1�[��^_]Í�&    �h��t���(����C����P���������[^_]ø�����2�����&    �����S�Ë��  ���	  ww��	�#  �   ���'  �Q  ��t�  ����  �   �غ `  ������ `  ��������d   ������غ @  ������d   [�������&    ��)�  ��'�  ��!��  �G  ��&�r  ��'��  ���   (  ��������     ���   ��   ��*  �'  ��������     ���  ��   ��*  �'  ��������     ���������   ��*  �'  ��������     ���  ��   ��*  �'  ��������     ���������   ��*  �'  ��������     ���  ��   ��*  �'  ��������     ���  ��   ��*  �'  ��������     �����������     ���  ����     �����������   ��  ��    ��<   [�������&    f���vP����   �   �   ��������   ��������d   ������   ��������d   [�������$�[  �   �غ`  ������غ`  ������   ��������   ������غ   [������v ���/  v]��uT���     ���   ��   ��������     ������   ��������   ��  ��    �[�f���t���u��   ��  ��    �[Ív ���  �  ���h�����u����   �6   ��(  �   ���   ��$  ���      ������   ��������      �������   ��������   ��   ��   �����   [�������&    ����  ��   ��t	���������     ���   ��   ��������     ������   ��������   ��  ��    �[Ã�
��������     ���   ��   ��������     ������   ��������   ��  ��    �[Í�    ���a������n������A����   �غ �  ������ �  ��������d   ������ �  ��������d   ������   �غ   ������   ��������   ������   ��������   ������غ   ������   [������t& ��1��  ��   ��5w\��3wc���   �7   ��(  �   �غ  ������  ��������d   ������غ  [�����f���%������>���f���8���0����   �   ��������   ��������d   ������   �=�����-�c�����   ��1��������     ���  ����     ���������d   ��������     ���  ����     ���������7   ���   ��(  �   ���   ��$  ���   � �  ��   1ҋ��   ��  [Ã�*�������,�|���[Í�    ���     ���   ����     ���������d   ��������     �����7   ���   ��(  � P  ���   ��$  �   ���   �X�����&    ��   �غ   ������   ��������d   ������غ   [������t& ���     ���   ����     ������   ������b���f��   �   ��������   ��������d   ������   �I�����&    ���     ���   ����     ���������d   ��������     ���  �-�����&    �t& ������UWVS�Ã�0���  d�    �D$,1����w  vE����w�   �� �ud���  ��t71��D$,d+    ��  ��0��[^_]Ít& �����   1���vЃ�uh    1�j�����XZ빍�&    f���u��5   ��t��   ����������  ���?  ���  ���w����Pd����j����Z���_����    �   ����  �r����  �   �����1��2���f��   ��� ����   ����������  ����  ���  ��������Pd���������Z��������    �    ���x  �r���m  �   �����1������v ��������ƅ�������   ����������  ���  ���}����Pd����p����Z���e����    �   ����  �z����  �   ������:�����&    �v ��������ƅ������   ����������  �����  ��tA�Pd���t8�z��t1�    �    ���@  �j���5  �   ��������  �v ��������Ad�@����   �x����   �   �����   �h����   ����������  �D$ �D$
 �D$���_����Ad�@��tw�x��tp�   �����   �h����   ����������  �D$
�������Ad�@�������X�������   ���tN�x��tG�������������t& ��D$
뽍�&    f��D$ �D$
 �D$�[�������������  �2���������������   ��������  ���������������  �N����   ������h����   1�������W����   ������F���������>����������&    f���������  ��3wpS�Ã�v���   �� �u�u3[Ð��t
��t&��u�غ   �����ǃ�      [Í�&    f��غ   ������������ǃ�      [ÐÍ�&    ��&    ���������  ��3w��v���   ���� �u��u,Ív ��t
��t��u틐�  ��t�   ������t& ����  ��tʺ   ������t& �����WVS�Ã���`  d�    �D$1����9  ���  ��	t��  ����  �H����1  ��&��  ��2��  ��  ��5�f  ��  ��;��  ��  �Hă��\  ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ��"��  �J��   ���X���  j ��
  �9   �CjD��������  ZY����  ���   ���   ���  ǀ�      ���  ����  �Pd�����  �R����  �    ���|  �I���q  ��������  �o  ���t3��   �H����X  ǃ     ǃ      ǃ$     �v ǃ�     ǃ�     ǃ�     ��"��  ��������B�����������"  ���  �D$d+    �3  ��[^_�f��Hʃ��%����P   ���  �   �    f���  �������`  ���
  ���  ���������  ǃ�    ���   �   ǃ�     ǃ�     ǃ�      ǃ�  �G ��� ǃ     ���  ǃ      ǃ$     ������t& �������v ���  ��6��   j ���   �C�A   jA��������  _X��������D$d+    �  ����[^_������Hȃ��J����  f���3��   ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     �B܃��M������}���>�����6�?�����7�U���j ���   �s�A   j@������������A   ��j jA��������  �������t& ��4��  ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     �K�����    ��)wK��'�  ǃ�     ǃ�     ǃ�     ǃ    ǃ      ǃ$     �N������-�����v,��2�;  ǃ�     ǃ�     ǃ�     �������,������
  �v ����   ����  ��������f����	  ����   ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ������&    �P   �    ��l  �   f��n  ��������  ����   �H܃�������   ����ƅ ��H  ��   �m�����&������H�   ��������������������&��  ǃ     ��ǃ      ǃ$  