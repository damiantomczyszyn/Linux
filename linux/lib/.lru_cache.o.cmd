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
 *      task, then the wakeup sets TIF_NEED_RESCHED and schedule() gets
 *      called on the nearest possible occasion:
 *
 *       - If the kernel is preemptible (CONFIG_PREEMPTION=y):
 *
 *         - in syscall or exception context, at the next outmost
 *           preempt_enable(). (this might be as soon as the wake_up()'s
 *           spin_unlock()!)
 *
 *         - in IRQ context, return from interrupt-handler to
 *           preemptible context
 *
 *       - If the kernel is not preemptible (CONFIG_PREEMPTION is not set)
 *         then at the next:
 *
 *          - cond_resched() call
 *          - explicit schedule() call
 *          - return from syscall or exception to user-space
 *          - return from interrupt-handler to user-space
 *
 * WARNING: must be called with preemption disabled!
 */
static void __sched notrace __schedule(unsigned int sched_mode)
{
	struct task_struct *prev, *next;
	unsigned long *switch_count;
	unsigned long prev_state;
	struct rq_flags rf;
	struct rq *rq;
	int cpu;

	cpu = smp_processor_id();
	rq = cpu_rq(cpu);
	prev = rq->curr;

	schedule_debug(prev, !!sched_mode);

	if (sched_feat(HRTICK) || sched_feat(HRTICK_DL))
		hrtick_clear(rq);

	local_irq_disable();
	rcu_note_context_switch(!!sched_mode);

	/*
	 * Make sure that signal_pending_state()->signal_pending() below
	 * can't be reordered with __set_current_state(TASK_INTERRUPTIBLE)
	 * done by the caller to avoid the race with signal_wake_up():
	 *
	 * __set_current_state(@state)		signal_wake_up()
	 * schedule()				  set_tsk_thread_flag(p, TIF_SIGPENDING)
	 *					  wake_up_state(p, state)
	 *   LOCK rq->lock			    LOCK p->pi_state
	 *   smp_mb__after_spinlock()		    smp_mb__after_spinlock()
	 *     if (signal_pending_state())	    if (p->state & @state)
	 *
	 * Also, the membarrier system call requires a full memory barrier
	 * after coming from user-space, before storing to rq->curr.
	 */
	rq_lock(rq, &rf);
	smp_mb__after_spinlock();

	/* Promote REQ to ACT */
	rq->clock_update_flags <<= 1;
	update_rq_clock(rq);

	switch_count = &prev->nivcsw;

	/*
	 * We must load prev->state once (task_struct::state is volatile), such
	 * that:
	 *
	 *  - we form a control dependency vs deactivate_task() below.
	 *  - ptrace_{,un}freeze_traced() can change ->state underneath us.
	 */
	prev_state = READ_ONCE(prev->__state);
	if (!(sched_mode & SM_MASK_PREEMPT) && prev_state) {
		if (signal_pending_state(prev_state, prev)) {
			WRITE_ONCE(prev->__state, TASK_RUNNING);
		} else {
			prev->sched_contributes_to_load =
				(prev_state & TASK_UNINTERRUPTIBLE) &&
				!(prev_state & TASK_NOLOAD) &&
				!(prev->flags & PF_FROZEN);

			if (prev->sched_contributes_to_load)
				rq->nr_uninterruptible++;

			/*
			 * __schedule()			ttwu()
			 *   prev_state = prev->state;    if (p->on_rq && ...)
			 *   if (prev_state)		    goto out;
			 *     p->on_rq = 0;		  smp_acquire__after_ctrl_dep();
			 *				  p->state = TASK_WAKING
			 *
			 * Where __schedule() and ttwu() have matching control dependencies.
			 *
			 * After this, schedule() must not care about p->state any more.
			 */
			deactivate_task(rq, prev, DEQUEUE_SLEEP | DEQUEUE_NOCLOCK);

			if (prev->in_iowait) {
				atomic_inc(&rq->nr_iowait);
				delayacct_blkio_start();
			}
		}
		switch_count = &prev->nvcsw;
	}

	next = pick_next_task(rq, prev, &rf);
	clear_tsk_need_resched(prev);
	clear_preempt_need_resched();
#ifdef CONFIG_SCHED_DEBUG
	rq->last_seen_need_resched_ns = 0;
#endif

	if (likely(prev != next)) {
		rq->nr_switches++;
		/*
		 * RCU users of rcu_dereference(rq->curr) may not see
		 * changes to task_struct made by pick_next_task().
		 */
		RCU_INIT_POINTER(rq->curr, next);
		/*
		 * The membarrier system call requires each architecture
		 * to have a full memory barrier after updating
		 * rq->curr, before returning to user-space.
		 *
		 * Here are the schemes providing that barrier on the
		 * various architectures:
		 * - mm ? switch_mm() : mmdrop() for x86, s390, sparc, PowerPC.
		 *   switch_mm() rely on membarrier_arch_switch_mm() on PowerPC.
		 * - finish_lock_switch() for weakly-ordered
		 *   architectures where spin_unlock is a full barrier,
		 * - switch_to() for arm64 (weakly-ordered, spin_unlock
		 *   is a RELEASE barrier),
		 */
		++*switch_count;

		migrate_disable_switch(rq, prev);
		psi_sched_switch(prev, next, !task_on_rq_queued(prev));

		trace_sched_switch(sched_mode & SM_MASK_PREEMPT, prev, next, prev_state);

		/* Also unlocks the rq: */
		rq = context_switch(rq, prev, next, &rf);
	} else {
		rq->clock_update_flags &= ~(RQCF_ACT_SKIP|RQCF_REQ_SKIP);

		rq_unpin_lock(rq, &rf);
		__balance_callbacks(rq);
		raw_spin_rq_unlock_irq(rq);
	}
}

void __noreturn do_task_dead(void)
{
	/* Causes final put_task_struct in finish_task_switch(): */
	set_special_state(TASK_DEAD);

	/* Tell freezer to ignore us: */
	current->flags |= PF_NOFREEZE;

	__schedule(SM_NONE);
	BUG();

	/* Avoid "noreturn function does return" - but don't continue if BUG() is a NOP: */
	for (;;)
		cpu_relax();
}

static inline void sched_submit_work(struct task_struct *tsk)
{
	unsigned int task_flags;

	if (task_is_running(tsk))
		return;

	task_flags = tsk->flags;
	/*
	 * If a worker goes to sleep, notify and ask workqueue whether it
	 * wants to wake up a task to maintain concurrency.
	 */
	if (task_flags & (PF_WQ_WORKER | PF_IO_WORKER)) {
		if (task_flags & PF_WQ_WORKER)
			wq_worker_sleeping(tsk);
		else
			io_wq_worker_sleeping(tsk);
	}

	if (tsk_is_pi_blocked(tsk))
		return;

	/*
	 * If we are going to sleep and we have plugged IO queued,
	 * make sure to submit it to avoid deadlocks.
	 */
	blk_flush_plug(tsk->plug, true);
}

static void sched_update_worker(struct task_struct *tsk)
{
	if (tsk->flags & (PF_WQ_WORKER | PF_IO_WORKER)) {
		if (tsk->flags & PF_WQ_WORKER)
			wq_worker_running(tsk);
		else
			io_wq_worker_running(tsk);
	}
}

asmlinkage __visible void __sched schedule(void)
{
	struct task_struct *tsk = current;

	sched_submit_work(tsk);
	do {
		preempt_disable();
		__schedule(SM_NONE);
		sched_preempt_enable_no_resched();
	} while (need_resched());
	sched_update_worker(tsk);
}
EXPORT_SYMBOL(schedule);

/*
 * synchronize_rcu_tasks() makes sure that no task is stuck in preempted
 * state (have scheduled out non-voluntarily) by making sure that all
 * tasks have either left the run queue or have gone into user space.
 * As idle tasks do not do either, they must not ever be preempted
 * (schedule out non-voluntarily).
 *
 * schedule_idle() is similar to schedule_preempt_disable() except that it
 * never enables preemption because it does not call sched_submit_work().
 */
void __sched schedule_idle(void)
{
	/*
	 * As this skips calling sched_submit_work(), which the idle task does
	 * regardless because that function is a nop when the task is in a
	 * TASK_RUNNING state, make sure this isn't used someplace that the
	 * current task can be in any other state. Note, idle is always in the
	 * TASK_RUNNING state.
	 */
	WARN_ON_ONCE(current->__state);
	do {
		__schedule(SM_NONE);
	} while (need_resched());
}

#if defined(CONFIG_CONTEXT_TRACKING) && !defined(CONFIG_HAVE_CONTEXT_TRACKING_OFFSTACK)
asmlinkage __visible void __sched schedule_user(void)
{
	/*
	 * If we come here after a random call to set_need_resched(),
	 * or we have been woken up remotely but the IPI has not yet arrived,
	 * we haven't yet exited the RCU idle mode. Do it here manually until
	 * we find a better solution.
	 *
	 * NB: There are buggy callers of this function.  Ideally we
	 * should warn if prev_state != CONTEXT_USER, but that will trigger
	 * too frequently to make sense yet.
	 */
	enum ctx_state prev_state = exception_enter();
	schedule();
	exception_exit(prev_state);
}
#endif

/**
 * schedule_preempt_disabled - called with preemption disabled
 *
 * Returns with preemption disabled. Note: preempt_count must be 1
 */
void __sched schedule_preempt_disabled(void)
{
	sched_preempt_enable_no_resched();
	schedule();
	preempt_disable();
}

#ifdef CONFIG_PREEMPT_RT
void __sched notrace schedule_rtlock(void)
{
	do {
		preempt_disable();
		__schedule(SM_RTLOCK_WAIT);
		sched_preempt_enable_no_resched();
	} while (need_resched());
}
NOKPROBE_SYMBOL(schedule_rtlock);
#endif

static void __sched notrace preempt_schedule_common(void)
{
	do {
		/*
		 * Because the function tracer can trace preempt_count_sub()
		 * and it also uses preempt_enable/disable_notrace(), if
		 * NEED_RESCHED is set, the preempt_enable_notrace() called
		 * by the function tracer will call this function again and
		 * cause infinite recursion.
		 *
		 * Preemption must be disabled here before the function
		 * tracer can trace. Break up preempt_disable() into two
		 * calls. One to disable preemption without fear of being
		 * traced. The other to still record the preemption latency,
		 * which can also be traced by the function tracer.
		 */
		preempt_disable_notrace();
		preempt_latency_start(1);
		__schedule(SM_PREEMPT);
		preempt_latency_stop(1);
		preempt_enable_no_resched_notrace();

		/*
		 * Check again in case we missed a preemption opportunity
		 * between schedule and now.
		 */
	} while (need_resched());
}

#ifdef CONFIG_PREEMPTION
/*
 * This is the entry point to schedule() from in-kernel preemption
 * off of preempt_enable.
 */
asmlinkage __visible void __sched notrace preempt_schedule(void)
{
	/*
	 * If there is a non-zero preempt_count or interrupts are disabled,
	 * we do not want to preempt the current task. Just return..
	 */
	if (likely(!preemptible()))
		return;
	preempt_schedule_common();
}
NOKPROBE_SYMBOL(preempt_schedule);
EXPORT_SYMBOL(preempt_schedule);

#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#ifndef preempt_schedule_dynamic_enabled
#define preempt_schedule_dynamic_enabled	preempt_schedule
#define preempt_schedule_dynamic_disabled	NULL
#endif
DEFINE_STATIC_CALL(preempt_schedule, preempt_schedule_dynamic_enabled);
EXPORT_STATIC_CALL_TRAMP(preempt_schedule);
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
static DEFINE_STATIC_KEY_TRUE(sk_dynamic_preempt_schedule);
void __sched notrace dynamic_preempt_schedule(void)
{
	if (!static_branch_unlikely(&sk_dynamic_preempt_schedule))
		return;
	preempt_schedule();
}
NOKPROBE_SYMBOL(dynamic_preempt_schedule);
EXPORT_SYMBOL(dynamic_preempt_schedule);
#endif
#endif

/**
 * preempt_schedule_notrace - preempt_schedule called by tracing
 *
 * The tracing infrastructure uses preempt_enable_notrace to prevent
 * recursion and tracing preempt enabling caused by the tracing
 * infrastructure itself. But as tracing can happen in areas coming
 * from userspace or just about to enter userspace, a preempt enable
 * can occur before user_exit() is called. This will cause the scheduler
 * to be called when the system is still in usermode.
 *
 * To prevent this, the preempt_enable_notrace will use this function
 * instead of preempt_schedule() to exit user context if needed before
 * calling the scheduler.
 */
asmlinkage __visible void __sched notrace preempt_schedule_notrace(void)
{
	enum ctx_state prev_ctx;

	if (likely(!preemptible()))
		return;

	do {
		/*
		 * Because the function tracer can trace preempt_count_sub()
		 * and it also uses preempt_enable/disable_notrace(), if
		 * NEED_RESCHED is set, the preempt_enable_notrace() called
		 * by the function tracer will call this function again and
		 * cause infinite recursion.
		 *
		 * Preemption must be disabled here before the function
		 * tracer can trace. Break up preempt_disable() into two
		 * calls. One to disable preemption without fear of being
		 * traced. The other to still record the preemption latency,
		 * which can also be traced by the function tracer.
		 */
		preempt_disable_notrace();
		preempt_latency_start(1);
		/*
		 * Needs preempt disabled in case user_exit() is traced
		 * and the tracer calls preempt_enable_notrace() causing
		 * an infinite recursion.
		 */
		prev_ctx = exception_enter();
		__schedule(SM_PREEMPT);
		exception_exit(prev_ctx);

		preempt_latency_stop(1);
		preempt_enable_no_resched_notrace();
	} while (need_resched());
}
EXPORT_SYMBOL_GPL(preempt_schedule_notrace);

#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#ifndef preempt_schedule_notrace_dynamic_enabled
#define preempt_schedule_notrace_dynamic_enabled	preempt_schedule_notrace
#define preempt_schedule_notrace_dynamic_disabled	NULL
#endif
DEFINE_STATIC_CALL(preempt_schedule_notrace, preempt_schedule_notrace_dynamic_enabled);
EXPORT_STATIC_CALL_TRAMP(preempt_schedule_notrace);
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
static DEFINE_STATIC_KEY_TRUE(sk_dynamic_preempt_schedule_notrace);
void __sched notrace dynamic_preempt_schedule_notrace(void)
{
	if (!static_branch_unlikely(&sk_dynamic_preempt_schedule_notrace))
		return;
	preempt_schedule_notrace();
}
NOKPROBE_SYMBOL(dynamic_preempt_schedule_notrace);
EXPORT_SYMBOL(dynamic_preempt_schedule_notrace);
#endif
#endif

#endif /* CONFIG_PREEMPTION */

/*
 * This is the entry point to schedule() from kernel preemption
 * off of irq context.
 * Note, that this is called and return with irqs disabled. This will
 * protect us against recursive calling from irq.
 */
asmlinkage __visible void __sched preempt_schedule_irq(void)
{
	enum ctx_state prev_state;

	/* Catch callers which need to be fixed */
	BUG_ON(preempt_count() || !irqs_disabled());

	prev_state = exception_enter();

	do {
		preempt_disable();
		local_irq_enable();
		__schedule(SM_PREEMPT);
		local_irq_disable();
		sched_preempt_enable_no_resched();
	} while (need_resched());

	exception_exit(prev_state);
}

int default_wake_function(wait_queue_entry_t *curr, unsigned mode, int wake_flags,
			  void *key)
{
	WARN_ON_ONCE(IS_ENABLED(CONFIG_SCHED_DEBUG) && wake_flags & ~WF_SYNC);
	return try_to_wake_up(curr->private, mode, wake_flags);
}
EXPORT_SYMBOL(default_wake_function);

static void __setscheduler_prio(struct task_struct *p, int prio)
{
	if (dl_prio(prio))
		p->sched_class = &dl_sched_class;
	else if (rt_prio(prio))
		p->sched_class = &rt_sched_class;
	else
		p->sched_class = &fair_sched_class;

	p->prio = prio;
}

#ifdef CONFIG_RT_MUTEXES

static inline int __rt_effective_prio(struct task_struct *pi_task, int prio)
{
	if (pi_task)
		prio = min(prio, pi_task->prio);

	return prio;
}

static inline int rt_effective_prio(struct task_struct *p, int prio)
{
	struct task_struct *pi_task = rt_mutex_get_top_task(p);

	return __rt_effective_prio(pi_task, prio);
}

/*
 * rt_mutex_setprio - set the current priority of a task
 * @p: task to boost
 * @pi_task: donor task
 *
 * This function changes the 'effective' priority of a task. It does
 * not touch ->normal_prio like __setscheduler().
 *
 * Used by the rt_mutex code to implement priority inheritance
 * logic. Call site only calls if the priority of the task changed.
 */
void rt_mutex_setprio(struct task_struct *p, struct task_struct *pi_task)
{
	int prio, oldprio, queued, running, queue_flag =
		DEQUEUE_SAVE | DEQUEUE_MOVE | DEQUEUE_NOCLOCK;
	const struct sched_class *prev_class;
	struct rq_flags rf;
	struct rq *rq;

	/* XXX used to be waiter->prio, not waiter->task->prio */
	prio = __rt_effective_prio(pi_task, p->normal_prio);

	/*
	 * If nothing changed; bail early.
	 */
	if (p->pi_top_task == pi_task && prio == p->prio && !dl_prio(prio))
		return;

	rq = __task_rq_lock(p, &rf);
	update_rq_clock(rq);
	/*
	 * Set under pi_lock && rq->lock, such that the value can be used under
	 * either lock.
	 *
	 * Note that there is loads of tricky to make this pointer cache work
	 * right. rt_mutex_slowunlock()+rt_mutex_postunlock() work together to
	 * ensure a task is de-boosted (pi_task is set to NULL) before the
	 * task is allowed to run again (and can exit). This ensures the pointer
	 * points to a blocked task -- which guarantees the task is present.
	 */
	p->pi_top_task = pi_task;

	/*
	 * For FIFO/RR we only need to set prio, if that matches we're done.
	 */
	if (prio == p->prio && !dl_prio(prio))
		goto out_unlock;

	/*
	 * Idle task boosting is a nono in general. There is one
	 * exception, when PREEMPT_RT and NOHZ is active:
	 *
	 * The idle task calls get_next_timer_interrupt() and holds
	 * the timer wheel base->lock on the CPU and another CPU wants
	 * to access the timer (probably to cancel it). We can safely
	 * ignore the boosting request, as the idle CPU runs this code
	 * with interrupts disabled and will complete the lock
	 * protected section without being interrupted. So there is no
	 * real need to boost.
	 */
	if (unlikely(p == rq->idle)) {
		WARN_ON(p != rq->curr);
		WARN_ON(p->pi_blocked_on);
		goto out_unlock;
	}

	trace_sched_pi_setprio(p, pi_task);
	oldprio = p->prio;

	if (oldprio == prio)
		queue_flag &= ~DEQUEUE_MOVE;

	prev_class = p->sched_class;
	queued = task_on_rq_queued(p);
	running = task_current(rq, p);
	if (queued)
		dequeue_task(rq, p, queue_flag);
	if (running)
		put_prev_task(rq, p);

	/*
	 * Boosting condition are:
	 * 1. -rt task is running and holds mutex A
	 *      --> -dl task blocks on mutex A
	 *
	 * 2. -dl task is running and holds mutex A
	 *      --> -dl task blocks on mutex A and could preempt the
	 *          running task
	 */
	if (dl_prio(prio)) {
		if (!dl_prio(p->normal_prio) ||
		    (pi_task && dl_prio(pi_task->prio) &&
		     dl_entity_preempt(&pi_task->dl, &p->dl))) {
			p->dl.pi_se = pi_task->dl.pi_se;
			queue_flag |= ENQUEUE_REPLENISH;
		} else {
			p->dl.pi_se = &p->dl;
		}
	} else if (rt_prio(prio)) /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#ifndef _CX23885_REG_H_
#define _CX23885_REG_H_

/*
Address Map
0x00000000 -> 0x00009000   TX SRAM  (Fifos)
0x00010000 -> 0x00013c00   RX SRAM  CMDS + CDT

EACH CMDS struct is 0x80 bytes long

DMAx_PTR1 = 0x03040 address of first cluster
DMAx_PTR2 = 0x10600 address of the CDT
DMAx_CNT1 = cluster size in (bytes >> 4) -1
DMAx_CNT2 = total cdt size for all entries >> 3

Cluster Descriptor entry = 4 DWORDS
 DWORD 0 -> ptr to cluster
 DWORD 1 Reserved
 DWORD 2 Reserved
 DWORD 3 Reserved

Channel manager Data Structure entry = 20 DWORD
  0  IntialProgramCounterLow
  1  IntialProgramCounterHigh
  2  ClusterDescriptorTableBase
  3  ClusterDescriptorTableSize
  4  InstructionQueueBase
  5  InstructionQueueSize
...  Reserved
 19  Reserved
*/

/* Risc Instructions */
#define RISC_CNT_INC		 0x00010000
#define RISC_CNT_RESET		 0x00030000
#define RISC_IRQ1		 0x01000000
#define RISC_IRQ2		 0x02000000
#define RISC_EOL		 0x04000000
#define RISC_SOL		 0x08000000
#define RISC_WRITE		 0x10000000
#define RISC_SKIP		 0x20000000
#define RISC_JUMP		 0x70000000
#define RISC_SYNC		 0x80000000
#define RISC_RESYNC		 0x80008000
#define RISC_READ		 0x90000000
#define RISC_WRITERM		 0xB0000000
#define RISC_WRITECM		 0xC0000000
#define RISC_WRITECR		 0xD0000000
#define RISC_WRITEC		 0x50000000
#define RISC_READC		 0xA0000000


/* Audio and Video Core */
#define HOST_REG1		0x00000000
#define HOST_REG2		0x00000001
#define HOST_REG3		0x00000002

/* Chip Configuration Registers */
#define CHIP_CTRL		0x00000100
#define AFE_CTRL		0x00000104
#define VID_PLL_INT_POST	0x00000108
#define VID_PLL_FRAC		0x0000010C
#define AUX_PLL_INT_POST	0x00000110
#define AUX_PLL_FRAC		0x00000114
#define SYS_PLL_INT_POST	0x00000118
#define SYS_PLL_FRAC		0x0000011C
#define PIN_CTRL		0x00000120
#define AUD_IO_CTRL		0x00000124
#define AUD_LOCK1		0x00000128
#define AUD_LOCK2		0x0000012C
#define POWER_CTRL		0x00000130
#define AFE_DIAG_CTRL1		0x00000134
#define AFE_DIAG_CTRL3		0x0000013C
#define PLL_DIAG_CTRL		0x00000140
#define AFE_CLK_OUT_CTRL	0x00000144
#define DLL1_DIAG_CTRL		0x0000015C

/* GPIO[23:19] Output Enable */
#define GPIO2_OUT_EN_REG	0x00000160
/* GPIO[23:19] Data Registers */
#define GPIO2			0x00000164

#define IFADC_CTRL		0x00000180

/* Infrared Remote Registers */
#define IR_CNTRL_REG	0x00000200
#define IR_TXCLK_REG	0x00000204
#define IR_RXCLK_REG	0x00000208
#define IR_CDUTY_REG	0x0000020C
#define IR_STAT_REG	0x00000210
#define IR_IRQEN_REG	0x00000214
#define IR_FILTR_REG	0x00000218
#define IR_FIFO_REG	0x0000023C

/* Video Decoder Registers */
#define MODE_CTRL		0x00000400
#define OUT_CTRL1		0x00000404
#define OUT_CTRL2		0x00000408
#define GEN_STAT		0x0000040C
#define INT_STAT_MASK		0x00000410
#define LUMA_CTRL		0x00000414
#define HSCALE_CTRL		0x00000418
#define VSCALE_CTRL		0x0000041C
#define CHROMA_CTRL		0x00000420
#define VBI_LINE_CTRL1		0x00000424
#define VBI_LINE_CTRL2		0x00000428
#define VBI_LINE_CTRL3		0x0000042C
#define VBI_LINE_CTRL4		0x00000430
#define VBI_LINE_CTRL5		0x00000434
#define VBI_FC_CFG		0x00000438
#define VBI_MISC_CFG1		0x0000043C
#define VBI_MISC_CFG2		0x00000440
#define VBI_PAY1		0x00000444
#define VBI_PAY2		0x00000448
#define VBI_CUST1_CFG1		0x0000044C
#define VBI_CUST1_CFG2		0x00000450
#define VBI_CUST1_CFG3		0x00000454
#define VBI_CUST2_CFG1		0x00000458
#define VBI_CUST2_CFG2		0x0000045C
#define VBI_CUST2_CFG3		0x00000460
#define VBI_CUST3_CFG1		0x00000464
#define VBI_CUST3_CFG2		0x00000468
#define VBI_CUST3_CFG3		0x0000046C
#define HORIZ_TIM_CTRL		0x00000470
#define VERT_TIM_CTRL		0x00000474
#define SRC_COMB_CFG		0x00000478
#define CHROMA_VBIOFF_CFG	0x0000047C
#define FIELD_COUNT		0x00000480
#define MISC_TIM_CTRL		0x00000484
#define DFE_CTRL1		0x00000488
#define DFE_CTRL2		0x0000048C
#define DFE_CTRL3		0x00000490
#define PLL_CTRL		0x00000494
#define HTL_CTRL		0x00000498
#define COMB_CTRL		0x0000049C
#define CRUSH_CTRL		0x000004A0
#define SOFT_RST_CTRL		0x000004A4
#define CX885_VERSION		0x000004B4
#define VBI_PASS_CTRL		0x000004BC

/* Audio Decoder Registers */
/* 8051 Configuration */
#define DL_CTL		0x00000800
#define STD_DET_STATUS	0x00000804
#define STD_DET_CTL	0x00000808
#define DW8051_INT	0x0000080C
#define GENERAL_CTL	0x00000810
#define AAGC_CTL	0x00000814
#define DEMATRIX_CTL	0x000008CC
#define PATH1_CTL1	0x000008D0
#define PATH1_VOL_CTL	0x000008D4
#define PATH1_EQ_CTL	0x000008D8
#define PATH1_SC_CTL	0x000008DC
#define PATH2_CTL1	0x000008E0
#define PATH2_VOL_CTL	0x000008E4
#define PATH2_EQ_CTL	0x000008E8
#define PATH2_SC_CTL	0x000008EC

/* Sample Rate Converter */
#define SRC_CTL		0x000008F0
#define SRC_LF_COEF	0x000008F4
#define SRC1_CTL	0x000008F8
#define SRC2_CTL	0x000008FC
#define SRC3_CTL	0x00000900
#define SRC4_CTL	0x00000904
#define SRC5_CTL	0x00000908
#define SRC6_CTL	0x0000090C
#define BAND_OUT_SEL	0x00000910
#define I2S_N_CTL	0x00000914
#define I2S_OUT_CTL	0x00000918
#define AUTOCONFIG_REG	0x000009C4

/* Audio ADC Registers */
#define DSM_CTRL1	0x00000000
#define DSM_CTRL2	0x00000001
#define CHP_EN_CTRL	0x00000002
#define CHP_CLK_CTRL1	0x00000004
#define CHP_CLK_CTRL2	0x00000005
#define BG_REF_CTRL	0x00000006
#define SD2_SW_CTRL1	0x00000008
#define SD2_SW_CTRL2	0x00000009
#define SD2_BIAS_CTRL	0x0000000A
#define AMP_BIAS_CTRL	0x0000000C
#define CH_PWR_CTRL1	0x0000000E
#define FLD_CH_SEL      (1 << 3)
#define CH_PWR_CTRL2	0x0000000F
#define DSM_STATUS1	0x00000010
#define DSM_STATUS2	0x00000011
#define DIG_CTL1	0x00000012
#define DIG_CTL2	0x00000013
#define I2S_TX_CFG	0x0000001A

#define DEV_CNTRL2	0x00040000

#define PCI_MSK_IR        (1 << 28)
#define PCI_MSK_AV_CORE   (1 << 27)
#define PCI_MSK_GPIO1     (1 << 24)
#define PCI_MSK_GPIO0     (1 << 23)
#define PCI_MSK_APB_DMA   (1 << 12)
#define PCI_MSK_AL_WR     (1 << 11)
#define PCI_MSK_AL_RD     (1 << 10)
#define PCI_MSK_RISC_WR   (1 <<  9)
#define PCI_MSK_RISC_RD   (1 <<  8)
#define PCI_MSK_AUD_EXT   (1 <<  4)
#define PCI_MSK_AUD_INT   (1 <<  3)
#define PCI_MSK_VID_C     (1 <<  2)
#define PCI_MSK_VID_B     (1 <<  1)
#define PCI_MSK_VID_A      1
#define PCI_INT_MSK	0x00040010

#define PCI_INT_STAT	0x00040014
#define PCI_INT_MSTAT	0x00040018

#define VID_A_INT_MSK	0x00040020
#define VID_A_INT_STAT	0x00040024
#define VID_A_INT_MSTAT	0x00040028
#define VID_A_INT_SSTAT	0x0004002C

#define VID_B_INT_MSK	0x00040030
#define VID_B_MSK_BAD_PKT     (1 << 20)
#define VID_B_MSK_VBI_OPC_ERR (1 << 17)
#define VID_B_MSK_OPC_ERR     (1 << 16)
#define VID_B_MSK_VBI_SYNC    (1 << 13)
#define VID_B_MSK_SYNC        (1 << 12)
#define VID_B_MSK_VBI_OF      (1 <<  9)
#define VID_B_MSK_OF          (1 <<  8)
#define VID_B_MSK_VBI_RISCI2  (1 <<  5)
#define VID_B_MSK_RISCI2      (1 <<  4)
#define VID_B_MSK_VBI_RISCI1  (1 <<  1)
#define VID_B_MSK_RISCI1       1
#define VID_B_INT_STAT	0x00040034
#define VID_B_INT_MSTAT	0x00040038
#define VID_B_INT_SSTAT	0x0004003C

#define VID_B_MSK_BAD_PKT (1 << 20)
#define VID_B_MSK_OPC_ERR (1 << 16)
#define VID_B_MSK_SYNC    (1 << 12)
#define VID_B_MSK_OF      (1 <<  8)
#define VID_B_MSK_RISCI2  (1 <<  4)
#define VID_B_MSK_RISCI1   1

#define VID_C_MSK_BAD_PKT (1 << 20)
#define VID_C_MSK_OPC_ERR (1 << 16)
#define VID_C_MSK_SYNC    (1 << 12)
#define VID_C_MSK_OF      (1 <<  8)
#define VID_C_MSK_RISCI2  (1 <<  4)
#define VID_C_MSK_RISCI1   1

/* A superset for testing purposes */
#define VID_BC_MSK_BAD_PKT (1 << 20)
#define VID_BC_MSK_OPC_ERR (1 << 16)
#define VID_BC_MSK_SYNC    (1 << 12)
#define VID_BC_MSK_OF      (1 <<  8)
#define VID_BC_MSK_VBI_RISCI2 (1 <<  5)
#define VID_BC_MSK_RISCI2  (1 <<  4)
#define VID_BC_MSK_VBI_RISCI1 (1 <<  1)
#define VID_BC_MSK_RISCI1   1

#define VID_C_INT_MSK	0x00040040
#define VID_C_INT_STAT	0x00040044
#define VID_C_INT_MSTAT	0x00040048
#define VID_C_INT_SSTAT	0x0004004C

#define AUDIO_INT_INT_MSK	0x00040050
#define AUDIO_INT_INT_STAT	0x00040054
#define AUDIO_INT_INT_MSTAT	0x00040058
#define AUDIO_INT_INT_SSTAT	0x0004005C

#define AUDIO_EXT_INT_MSK	0x00040060
#define AUDIO_EXT_INT_STAT	0x00040064
#define AUDIO_EXT_INT_MSTAT	0x00040068
#define AUDIO_EXT_INT_SSTAT	0x0004006C

/* Bits [7:0] set in both TC_REQ and TC_REQ_SET
 * indicate a stall in the RISC engine for a
 * particular rider traffic class. This causes
 * the 885 and 888 bridges (unknown about 887)
 * to become inoperable. Setting bits in
 * TC_REQ_SET resets the corresponding bits
 * in TC_REQ (and TC_REQ_SET) allowing
 * operation to continue.
 */
#define TC_REQ		0x00040090
#define TC_REQ_SET	0x00040094

#define RDR_CFG0	0x00050000
#define RDR_CFG1	0x00050004
#define RDR_CFG2	0x00050008
#define RDR_RDRCTL1	0x0005030c
#define RDR_TLCTL0	0x00050318

/* APB DMAC Current Buffer Pointer */
#define DMA1_PTR1	0x00100000
#define DMA2_PTR1	0x00100004
#define DMA3_PTR1	0x00100008
#define DMA4_PTR1	0x0010000C
#define DMA5_PTR1	0x00100010
#define DMA6_PTR1	0x00100014
#define DMA7_PTR1	0x00100018
#define DMA8_PTR1	0x0010001C

/* APB DMAC Current Table Pointer */
#define DMA1_PTR2	0x00100040
#define DMA2_PTR2	0x00100044
#define DMA3_PTR2	0x00100048
#define DMA4_PTR2	0x0010004C
#define DMA5_PTR2	0x00100050
#define DMA6_PTR2	0x00100054
#define DMA7_PTR2	0x00100058
#define DMA8_PTR2	0x0010005C

/* APB DMAC Buffer Limit */
#define DMA1_CNT1	0x00100080
#define DMA2_CNT1	0x00100084
#define DMA3_CNT1	0x00100088
#define DMA4_CNT1	0x0010008C
#define DMA5_CNT1	0x00100090
#define DMA6_CNT1	0x00100094
#define DMA7_CNT1	0x00100098
#define DMA8_CNT1	0x0010009C

/* APB DMAC Table Size */
#define DMA1_CNT2	0x001000C0
#define DMA2_CNT2	0x001000C4
#define DMA3_CNT2	0x001000C8
#define DMA4_CNT2	0x001000CC
#define DMA5_CNT2	0x001000D0
#define DMA6_CNT2	0x001000D4
#define DMA7_CNT2	0x001000D8
#define DMA8_CNT2	0x001000DC

/* Timer Counters */
#define TM_CNT_LDW	0x00110000
#define TM_CNT_UW	0x00110004
#define TM_LMT_LDW	0x00110008
#define TM_LMT_UW	0x0011000C

/* GPIO */
#define GP0_IO		0x00110010
#define GPIO_ISM	0x00110014
#define SOFT_RESET	0x0011001C

/* GPIO (417 Microsoftcontroller) RW Data */
#define MC417_RWD	0x00110020

/* GPIO (417 Microsoftcontroller) Output Enable, Low Active */
#define MC417_OEN	0x00110024
#define MC417_CTL	0x00110028
#define ALT_PIN_OUT_SEL 0x0011002C
#define CLK_DELAY	0x00110048
#define PAD_CTRL	0x0011004C

/* Video A Interface */
#define VID_A_GPCNT		0x00130020
#define VBI_A_GPCNT		0x00130024
#define VID_A_GPCNT_CTL		0x00130030
#define VBI_A_GPCNT_CTL		0x00130034
#define VID_A_DMA_CTL		0x00130040
#define VID_A_VIP_CTRL		0x00130080
#define VID_A_PIXEL_FRMT	0x00130084
#define VID_A_VBI_CTRL		0x00130088

/* Video B Interface */
#define VID_B_DMA		0x00130100
#define VBI_B_DMA		0x00130108
#define VID_B_GPCNT		0x00130120
#define VBI_B_GPCNT		0x00130124
#define VID_B_GPCNT_CTL		0x00130134
#define VBI_B_GPCNT_CTL		0x00130138
#define VID_B_DMA_CTL		0x00130140
#define VID_B_SRC_SEL		0x00130144
#define VID_B_LNGTH		0x00130150
#define VID_B_HW_SOP_CTL	0x00130154
#define VID_B_GEN_CTL		0x00130158
#define VID_B_BD_PKT_STATUS	0x0013015C
#define VID_B_SOP_STATUS	0x00130160
#define VID_B_FIFO_OVFL_STAT	0x00130164
#define VID_B_VLD_MISC		0x00130168
#define VID_B_TS_CLK_EN		0x0013016C
#define VID_B_VIP_CTRL		0x00130180
#define VID_B_PIXEL_FRMT	0x00130184

/* Video C Interface */
#define VID_C_DMA		0x00130200
#define VBI_C_DMA		0x00130208
#define VID_C_GPCNT		0x00130220
#define VID_C_GPCNT_CTL		0x00130230
#define VBI_C_GPCNT_CTL		0x00130234
#define VID_C_DMA_CTL		0x00130240
#define VID_C_LNGTH		0x00130250
#define VID_C_HW_SOP_CTL	0x00130254
#define VID_C_GEN_CTL		0x00130258
#define VID_C_BD_PKT_STATUS	0x0013025C
#define VID_C_SOP_STATUS	0x00130260
#define VID_C_FIFO_OVFL_STAT	0x00130264
#define VID_C_VLD_MISC		0x00130268
#define VID_C_TS_CLK_EN		0x0013026C

/* Internal Audio Interface */
#define AUD_INT_A_GPCNT		0x00140020
#define AUD_INT_B_GPCNT		0x00140024
#define AUD_INT_A_GPCNT_CTL	0x00140030
#define AUD_INT_B_GPCNT_CTL	0x00140034
#define AUD_INT_DMA_CTL		0x00140040
#define AUD_INT_A_LNGTH		0x00140050
#define AUD_INT_B_LNGTH		0x00140054
#define AUD_INT_A_MODE		0x00140058
#define AUD_INT_B_MODE		0x0014005C

/* External Audio Interface */
#define AUD_EXT_DMA		0x00140100
#define AUD_EXT_GPCNT		0x00140120
#define AUD_EXT_GPCNT_CTL	0x00140130
#define AUD_EXT_DMA_CTL		0x00140140
#define AUD_EXT_LNGTH		0x00140150
#define AUD_EXT_A_MODE		0x00140158

/* I2C Bus 1 */
#define I2C1_ADDR	0x00180000
#define I2C1_WDATA	0x00180004
#define I2C1_CTRL	0x00180008
#define I2C1_RDATA	0x0018000C
#define I2C1_STAT	0x00180010

/* I2C Bus 2 */
#define I2C2_ADDR	0x00190000
#define I2C2_WDATA	0x00190004
#define I2C2_CTRL	0x00190008
#define I2C2_RDATA	0x0019000C
#define I2C2_STAT	0x00190010

/* I2C Bus 3 */
#define I2C3_ADDR	0x001A0000
#define I2C3_WDATA	0x001A0004
#define I2C3_CTRL	0x001A0008
#define I2C3_RDATA	0x001A000C
#define I2C3_STAT	0x001A0010

/* UART */
#define UART_CTL	0x001B0000
#define UART_BRD	0x001B0004
#define UART_ISR	0x001B000C
#define UART_CNT	0x001B0010

#endif /* _CX23885_REG_H_ */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               