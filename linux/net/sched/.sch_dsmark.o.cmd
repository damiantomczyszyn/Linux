nt_modules();
	if (irqs_disabled())
		print_irqtrace_events(prev);
	if (IS_ENABLED(CONFIG_DEBUG_PREEMPT)
	    && in_atomic_preempt_off()) {
		pr_err("Preemption disabled at:");
		print_ip_sym(KERN_ERR, preempt_disable_ip);
	}
	if (panic_on_warn)
		panic("scheduling while atomic\n");

	dump_stack();
	add_taint(TAINT_WARN, LOCKDEP_STILL_OK);
}

/*
 * Various schedule()-time debugging checks and statistics:
 */
static inline void schedule_debug(struct task_struct *prev, bool preempt)
{
#ifdef CONFIG_SCHED_STACK_END_CHECK
	if (task_stack_end_corrupted(prev))
		panic("corrupted stack end detected inside scheduler\n");

	if (task_scs_end_corrupted(prev))
		panic("corrupted shadow stack detected inside scheduler\n");
#endif

#ifdef CONFIG_DEBUG_ATOMIC_SLEEP
	if (!preempt && READ_ONCE(prev->__state) && prev->non_block_count) {
		printk(KERN_ERR "BUG: scheduling in a non-blocking section: %s/%d/%i\n",
			prev->comm, prev->pid, prev->non_block_count);
		dump_stack();
		add_taint(TAINT_WARN, LOCKDEP_STILL_OK);
	}
#endif

	if (unlikely(in_atomic_preempt_off())) {
		__schedule_bug(prev);
		preempt_count_set(PREEMPT_DISABLED);
	}
	rcu_sleep_check();
	SCHED_WARN_ON(ct_state() == CONTEXT_USER);

	profile_hit(SCHED_PROFILING, __builtin_return_address(0));

	schedstat_inc(this_rq()->sched_count);
}

static void put_prev_task_balance(struct rq *rq, struct task_struct *prev,
				  struct rq_flags *rf)
{
#ifdef CONFIG_SMP
	const struct sched_class *class;
	/*
	 * We must do the balancing pass before put_prev_task(), such
	 * that when we release the rq->lock the task is in the same
	 * state as before we took rq->lock.
	 *
	 * We can terminate the balance pass as soon as we know there is
	 * a runnable task of @class priority or higher.
	 */
	for_class_range(class, prev->sched_class, &idle_sched_class) {
		if (class->balance(rq, prev, rf))
			break;
	}
#endif

	put_prev_task(rq, prev);
}

/*
 * Pick up the highest-prio task:
 */
static inline struct task_struct *
__pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	const struct sched_class *class;
	struct task_struct *p;

	/*
	 * Optimization: we know that if all tasks are in the fair class we can
	 * call that function directly, but only if the @prev task wasn't of a
	 * higher scheduling class, because otherwise those lose the
	 * opportunity to pull in more work from other CPUs.
	 */
	if (likely(prev->sched_class <= &fair_sched_class &&
		   rq->nr_running == rq->cfs.h_nr_running)) {

		p = pick_next_task_fair(rq, prev, rf);
		if (unlikely(p == RETRY_TASK))
			goto restart;

		/* Assume the next prioritized class is idle_sched_class */
		if (!p) {
			put_prev_task(rq, prev);
			p = pick_next_task_idle(rq);
		}

		return p;
	}

restart:
	put_prev_task_balance(rq, prev, rf);

	for_each_class(class) {
		p = class->pick_next_task(rq);
		if (p)
			return p;
	}

	BUG(); /* The idle class should always have a runnable task. */
}

#ifdef CONFIG_SCHED_CORE
static inline bool is_task_rq_idle(struct task_struct *t)
{
	return (task_rq(t)->idle == t);
}

static inline bool cookie_equals(struct task_struct *a, unsigned long cookie)
{
	return is_task_rq_idle(a) || (a->core_cookie == cookie);
}

static inline bool cookie_match(struct task_struct *a, struct task_struct *b)
{
	if (is_task_rq_idle(a) || is_task_rq_idle(b))
		return true;

	return a->core_cookie == b->core_cookie;
}

static inline struct task_struct *pick_task(struct rq *rq)
{
	const struct sched_class *class;
	struct task_struct *p;

	for_each_class(class) {
		p = class->pick_task(rq);
		if (p)
			return p;
	}

	BUG(); /* The idle class should always have a runnable task. */
}

extern void task_vruntime_update(struct rq *rq, struct task_struct *p, bool in_fi);

static void queue_core_balance(struct rq *rq);

static struct task_struct *
pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	struct task_struct *next, *p, *max = NULL;
	const struct cpumask *smt_mask;
	bool fi_before = false;
	bool core_clock_updated = (rq == rq->core);
	unsigned long cookie;
	int i, cpu, occ = 0;
	struct rq *rq_i;
	bool need_sync;

	if (!sched_core_enabled(rq))
		return __pick_next_task(rq, prev, rf);

	cpu = cpu_of(rq);

	/* Stopper task is switching into idle, no need core-wide selection. */
	if (cpu_is_offline(cpu)) {
		/*
		 * Reset core_pick so that we don't enter the fastpath when
		 * coming online. core_pick would already be migrated to
		 * another cpu during offline.
		 */
		rq->core_pick = NULL;
		return __pick_next_task(rq, prev, rf);
	}

	/*
	 * If there were no {en,de}queues since we picked (IOW, the task
	 * pointers are all still valid), and we haven't scheduled the last
	 * pick yet, do so now.
	 *
	 * rq->core_pick can be NULL if no selection was made for a CPU because
	 * it was either offline or went offline during a sibling's core-wide
	 * selection. In this case, do a core-wide selection.
	 */
	if (rq->core->core_pick_seq == rq->core->core_task_seq &&
	    rq->core->core_pick_seq != rq->core_sched_seq &&
	    rq->core_pick) {
		WRITE_ONCE(rq->core_sched_seq, rq->core->core_pick_seq);

		next = rq->core_pick;
		if (next != prev) {
			put_prev_task(rq, prev);
			set_next_task(rq, next);
		}

		rq->core_pick = NULL;
		goto out;
	}

	put_prev_task_balance(rq, prev, rf);

	smt_mask = cpu_smt_mask(cpu);
	need_sync = !!rq->core->core_cookie;

	/* reset state */
	rq->core->core_cookie = 0UL;
	if (rq->core->core_forceidle_count) {
		if (!core_clock_updated) {
			update_rq_clock(rq->core);
			core_clock_updated = true;
		}
		sched_core_account_forceidle(rq);
		/* reset after accounting force idle */
		rq->core->core_forceidle_start = 0;
		rq->core->core_forceidle_count = 0;
		rq->core->core_forceidle_occupation = 0;
		need_sync = true;
		fi_before = true;
	}

	/*
	 * core->core_task_seq, core->core_pick_seq, rq->core_sched_seq
	 *
	 * @task_seq guards the task state ({en,de}queues)
	 * @pick_seq is the @task_seq we did a selection on
	 * @sched_seq is the @pick_seq we scheduled
	 *
	 * However, preemptions can cause multiple picks on the same task set.
	 * 'Fix' this by also increasing @task_seq for every pick.
	 */
	rq->core->core_task_seq++;

	/*
	 * Optimize for common case where this CPU has no cookies
	 * and there are no cookied tasks running on siblings.
	 */
	if (!need_sync) {
		next = pick_task(rq);
		if (!next->core_cookie) {
			rq->core_pick = NULL;
			/*
			 * For robustness, update the min_vruntime_fi for
			 * unconstrained picks as well.
			 */
			WARN_ON_ONCE(fi_before);
			task_vruntime_update(rq, next, false);
			goto out_set_next;
		}
	}

	/*
	 * For each thread: do the regular task pick and find the max prio task
	 * amongst them.
	 *
	 * Tie-break prio towards the current CPU
	 */
	for_each_cpu_wrap(i, smt_mask, cpu) {
		rq_i = cpu_rq(i);

		/*
		 * Current cpu always has its clock updated on entrance to
		 * pick_next_task(). If the current cpu is not the core,
		 * the core may also have been updated above.
		 */
		if (i != cpu && (rq_i != rq->core || !core_clock_updated))
			update_rq_clock(rq_i);

		p = rq_i->core_pick = pick_task(rq_i);
		if (!max || prio_less(max, p, fi_before))
			max = p;
	}

	cookie = rq->core->core_cookie = max->core_cookie;

	/*
	 * For each thread: try and find a runnable task that matches @max or
	 * force idle.
	 */
	for_each_cpu(i, smt_mask) {
		rq_i = cpu_rq(i);
		p = rq_i->core_pick;

		if (!cookie_equals(p, cookie)) {
			p = NULL;
			if (cookie)
				p = sched_core_find(rq_i, cookie);
			if (!p)
				p = idle_sched_class.pick_task(rq_i);
		}

		rq_i->core_pick = p;

		if (p == rq_i->idle) {
			if (rq_i->nr_running) {
				rq->core->core_forceidle_count++;
				if (!fi_before)
					rq->core->core_forceidle_seq++;
			}
		} else {
			occ++;
		}
	}

	if (schedstat_enabled() && rq->core->core_forceidle_count) {
		rq->core->core_forceidle_start = rq_clock(rq->core);
		rq->core->core_forceidle_occupation = occ;
	}

	rq->core->core_pick_seq = rq->core->core_task_seq;
	next = rq->core_pick;
	rq->core_sched_seq = rq->core->core_pick_seq;

	/* Something should have been selected for current CPU */
	WARN_ON_ONCE(!next);

	/*
	 * Reschedule siblings
	 *
	 * NOTE: L1TF -- at this point we're no longer running the old task and
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ×7ìâÒQ~U5(_P% Á¦îdûê9–|#ÆÂ E²\®œ•ŸEÕ¤+,ğóÕŒPgD	Ïzõåvüe¸hÚr .{0ŸÁa³¶Kô[âaÖÊçñ9Õºß(®Ûn¾4êÏ?Z‰K3ø»Šœ„ÿ†ªõ>„ìûi{,¿¾· Héáõµ„@ê^lZKÃß¡«æ4ÏOˆ<€k¾5»-Ì)…şD¥õ7º‹ÏwƒkÏOd?'!ÅG«t˜w.XIcja^ÇÉWÍòj†£‚’yº?ce^5À\í‡ßş·Ùt8:¤Îà®)wUŒ×ÊåKxGåIpç8YôñÀœ'_=°Znî•­ÔÈ İÈ£´ˆùÛšƒ^3®h…·˜uĞ£†¬zT4TJ^è^dW%Z[KÕö¯4çüoˆh~"<çËŒ_‰ ÊíÉ°4Aû0\ûœ›I5ĞŠ†€jüpQíìŸ] 7‡éğE[d09hÎ"Í¿ŸùW+£Éå Hşò)øqÔ›ó®½øSãuOb°ckÅ4‘Æød‚Ü—×HªĞéÕb}°Mc¶ëõèá©Éçk‚˜ppÕdÌ4ı¸>|¥¨,¸n}£-¥åÀ®'6ª¬ÑÉ`¸J’33Oo?LãÙïØ"öD‡y=İCuá}Ô˜€ÛO7o±ì£ºå¸êPxAs]¥H8öØß«PPh„š8k™ü;ôEpYotkCµkMú"¶Ü­_ÖŸj¶“J´ÅeÕMÌÒÇ,ïGM%WÏ…«pO&5éKÜ=!¢ı4Ä€GÃud±·®ÆÈ·±±CN;ıÙ4;À³r@„Èæ«éët§`y8ß>@õQù­4OˆÛB4Hz½ı¼¡<^µÈéƒ§wXCfÂô³Ì~œd3ütd²âëüdV²0ÉĞb™b»*¿¬óW››Î•¹es¯f\»ıó^QÃÃ’ìMYRrıp¨©+†éqsİø"`û9Nù@†fxÄÛÑ¿ıœ]¸Èq¤ÿ±µ Ô¿Ÿºé“LÇ´[‰Ì½g•ÊIM%ª{|Å]Ş*W6 F¸Ş„L;ë"ª7JNÒ×û«„ß¸ú»¿/< >ÂOöä¾wŒ¬3²Á|óY=x%0ï@Ù Râ‚f6=“½Où&wW¹‰pF—Ó)ßYhÅkqĞ+#ÉY·T"'×/ŸÃ‡uœ¬2z”•„² 	P¬ÙÒf¬|?P‘L;S{Ø3ô/ñ’wzZr´æµ£ã	Šx™$1¨£Zé–Œ
¿æ¶ åvR‡ªb(	Å–%rÎË%£Ó“êÏşªxi7n’IsÆÓ'¸Tç”y¨ùƒm`TFÀÒäÛ•6GÛ¤©:™^ôú^ï&“\‡k,†ÈLä
½šÀZ&‘ï…û–ªŒÛ9õJMûµmDoµĞöûTÖŞ»™Ò&]ÄrJ DÓsÃ1°QÕ=äCRÔœ1\™}R¢½Z);k/jbB‡¯£û­ ù¬–Ê/4»Û¬ğƒA4;á$¥¨{àeİïû¤[ÀÅ P6r±œ/=”A7kä„!*‰DSF#TM^_‰mÃäzPï±ÿ,G‚±ğR'‘Sô£oM¹gÛ‘T6PÁbóß†,Y?b³ÚÀĞUA+ï°Y–0P?ğP¨[w÷(DDJŞ4lñôoÀ3îj1«2ÄĞ¬ÿ\ “å"ìEï{g wóeuÔ~DÛÄEìl‡ÇšAÎQ)ğ­5¬$±ÿ4FnòƒBgw'	bKÛgµäÊg÷¯tÇÖVVÖnïJÁŞ—É‘%=0„¹¾ˆ/&ÔÆ¬ş[`8¦ L&bş]ƒ%…ãºÙ@qiIjxö¥ĞÕvV ıÙÏÍ<Û²ÈÈ1D+’g‘Œ‹”vŠ5®M/©u¿Á?P´T«§ÿıµ$×WPØó³F¥]ƒú(húRLã¢%Kh»àİ–åéÄ]CÇ”7 S‰6É=•L”£Zr(LåXN{ëò¨%)¤ŸàB\ø!½¯ùás­²'^ÅG_±b‰¦×æî]¨¹ºQ±Š¼Û‹OG“è†’Ek9—ÙÙ’V_ˆÿ)µÔ «DTî¿™‚ì™ß&!iĞ(ñÖú£YÑ¾œv»ç—Ï7Ì_ŞgC =%¸I„»·˜O</£’‘ºæò§¥sÅÖø«gÓØIÂù²‚\UæVMô§46fB©§<)u"+ü“ˆ‘À®XæÒ}v~~]¹)ĞùHSÕï<Û†#'£€·ázœıÿµÕınÅŒH|5v2pãèuıáö_øD&Ê'ˆAŸrşNéËeÎ¥Ùá k•Ü¿0t‰Eæ?::±¬å’³{¤	¤NE€éû°á(Òju°FC`Ó©èÄôkwû‚p\¾x¦PÜyí¡$Ï0O-­I€°°3š{0bóßÔà'—Æwh’Ë–‡­½°œÛâsWĞ°]Ã *¨±&Å„÷ıu¾Sw÷8¸(­)²µñ}Xµym\‹ÈÂá
¶¨c/0#£iö
ÃQüÁs±ÙÍº›Nî¤;,U%@rÎs—¡\íº¥Ş„ö;CÚC'É ™ş(³Ûæûå/®æò®‹ˆb5§7¡X<3}´u%~ÿ!Gdö(Çi¬È§Zvg’¬Š_İ\›:m}> 2«êİ2Wj^Ñ÷½4|•ZÀèšÚÆªîaÜ~(kœ‹÷6.!ôÂ©‰ş¶@„y¬Œ(Å¤¶ßæ³/÷]Ì¡Hx¡À¯`ªDI®iÎygT¡ë…Æãwô…{ö¨#£äD´¾`¨ztá×ª—ï|xqÁ¿z‡QB>ò³Ü”®y~t³ş¡‘ÎZ
L™
$ĞxK‹ÃYKe¾ÉTò˜0ùØâïXĞO]]`q}‹ ;;±Ÿü¸ÎY[Q­¸½BÉ_<ö®ÒôÏ¢ÛíÇG›yßjËfäÇÎÈ57gì:oúú¯Ç©8;RE\_Úv£°’¿œó¿·¯Ş·$JÚ~V)§`€°‚şÁå6]MEE˜mÁ¶ğµ7ævÚÚ‹È’³±*[$ò8$‘@¦±’R~co‹Xäs'E~ª0¥]b¶[D]»Üo…+ã™Óæ6*X ˜«a¢¢«ú¯ÂÅ.ë9øìÙ½Çoïzñ1.½îBšğóŞe—=âÉæäèdšÛ„î–(48=BÕ¡ÚœJSù.ƒ nG$ÃiªK	»—µAFèJñz%%œ!DÓhşšáŸ½dI=‹¢1å/s]3ÀÖwtA€K;údDxæ—tÛÉût-¬¼Øpœœ¸ÙJHš9yyÛpsD¬Ùnå7ÖKâz!ìy[p„Ò¾ É©ÈN˜Xpç¸c-¹‰ÍÛşü&9ûæş´ÚØŸşÛà±ì ş(-÷ÍÔk”²*áÙ}Œğ6Â$5Y„ã}6À»0¦éÚPêLu wÎÚøµ~Øä{ê\óß	o½N[d~Ljçı‘Ú¥"Aàÿø|^úÕíŸqÚ¹h T*Ö]9ÁSW>jÚã@ªp0Ñ®ÉrÅéÖ%ªrİ1®%Ñ¿«pÈJ]MÃC+ûı5ÒWu«8a¤`ÁÌÅ/èéüˆŞSİ‰> öföò"§U6¿Û‰:¼¥"³˜Cz5M~/×˜¯ŠDJC·˜œVÉTÜÔ?fü¤n–(}äbW%ÛbY²ii*5IÜIJ_“IZ±DB†õôät¿½£	óJ„çEXmüAŸuß/Û–îŸc×½øøËÈèHûûñ&¡-)+tnó­ªÌÜ^kE2ĞÒÆA–PNT ¾>VŒéOyTÁ›Á}9&GYà§U\ÂÛ äˆmz½jî]Â6·“Vv¹+c†Ş%ÍÔj2=jÒ|nÇá0pb`Ÿt[}ë+GájdÎúAbqø}¶F)£¡±E1[¹¤úk§+Ùõ*Í¯Ôd FÅHğ..ö‰•vw_Ue†së@¿| V|Š9­ÙZ²!3WôJ1jf`õŠ_"^gÏ¦œÚ°ÒÌÛ½¸¾u¬ÔXPy½½›õ6–¦² ÷6ppéeu×}Íˆ-’MñÙ¹û¿eåV,ø
—”pä&~nèÆ;Nó.ósÊ<rê$7™Óİø{®%l ±Cé²ÖtQ0H%tiB]¥m€ÆU¹ri¥)ãVl‰ÓHÓR"Èkš÷âî ŞHú_İ
í\¢h_–ºãñt·"tñ¤=;+ıË(„õò³O,õ	ÚÌ»½ÿÛ1œŒ¶@JC±“IÂÖ¼<´ˆ°tMy&¤EPy’*øè~!šÉ1¢ÊiâR Ü[Õµ$nìH$¯$HÛ®íœá O‡s½+»)ªhù¼|Û¡†lÇ-‰øPJ ¬Wx^0iü#Ã˜3ÁBµ¸â!fõ‘ÜÙÌì¢Æş8(Oë£l°àh)gQÓâ¬È±Se0bdVïş“¹^PÔô¨‚*˜ÄÉ†Ìò–ÎĞ›bÏÂ@àİR”KE  –`5P¹;‹N|(ÇSúÎ‰,äÔÃ‰B63€ÌûÂCf1N[ƒ3kI!.ğµ™.çX[g9úMÏÊ+½ °°ü…E¸´	¼Á}®g­yŸç¾¹¼>€XJ%84ŞdàÒ´æ<úd²w ü}FÃ‡ß1ÁtŠEBª4Ú±ğø˜]Z*?‚’ë'§#¦­,öÀ`2rğóø~²®3‚yz%úlpâæ&[K`ÜöÎ(³ÈÖÂ ş:TH†õ0Y¸T>.Üx©GÅ
!ViÈƒJdôsÕ¤&AË©7àHxI^5ÒĞØåÚı…‰d1Z”³ª´!†î*UÊÎÌ9˜[#Z‚/FZ@ÃKŸ‘C“X•|Ÿìa6D±|Á}Koõ¤ÿ¢iæì2vÙQ²„\dşşm§—‡ßVu—ä³BˆrÚ(œg •êâ÷qÁw¼òôO0ÕãŞí„dê¥)Y×¡Ï‡]º×3z*àÈ>ràpÕkîË5ËE”ùS¼H9H÷`%ôQ½«V„±(Õˆ+0ãsQ…£Yïrš¨ã7V†ª_O°c¦›É£^î#Í;…1rÙ_Vğ¨ç§ó6ßm	jAÓşğÄO…¶ÒòË^•@K¼‚|2ŠõûÁÔ7Vq·Ó0Î;9ÄÄ8P]Kæ£ëœ„b
Io/ó9ÅTÓLZÀ3Áÿ°ß9yV*Œ­‰ĞªTÁ'	…9l4¢B^Qhg&äQİÜal5T7÷/bñÓPÆ(#ó¬+Ôìö©ßç2¿Ò¢´œåí¸ŒFÀ»fÂ™ïêe…	çJé:q‰zã¾v]Œ3©¬·ø’S†pöŸ93TÎç?µ~y»ÔŒmÅ" ‘@ñzÿ‡ÑÒP‹¶B7ŒşUá²4{ğÄåìŞ%~áóÎr-Iİf%6†
Ô/ø¥U-¸½íòFTšO~$¢ °ÿ$İ" h¿¡™õäVg67¾æà´÷Ø‘¤“ÖÕöj‘_GGÃ7E<Tëo2Fœ_Ì ¥m„cÌ½-¦T‰z%
£+ã¼"îšÇAH ßü° ò5Ş,qŠR˜òbí¨Q›ËrÅCr€M<¤ô•p²!°eì|¹+é8™œŞmeW^é†İ‚ßÓ…8îFaÒQµ~ß(c
ÚÚŞµÛ‰Ã-d£öú{cÌBgcÍ}ËOxs“ôa»Vu»®üÂùkƒ˜ÖÇ e¿<tZ2˜ •g„·üp»(X ‰uLÈÙ¯ÍK*%"-ƒ:y.3wB×â/Øµˆ=ÄÅuî[%—qCÙnHyÄYnf· J…y×†€`|YM´§¢Ø]!Ö¹øY.s–¢‘×°f„:'O’¥ıªmÅwÁĞfÙQèÁĞwop·É'Œ—Ã Gbœ‰ğ­]° œõšnqÆÅµFzSnà¯ÛCSYj ï(µ-Aˆ•gĞ e<¡ ªL&¹]õ¨F"[ä«¯læ¾7î8be²üj#³Œ‡zÖ-SSIÖ(òêQà!³Oíÿ´ğx¡À”rĞO]()e¥>Ç)7„x@&xsÖÛÿø1+Ù3*Ù³³ıÿˆöç8¸Ošë~f™>Ó…t¸1)qş¶˜O´}„åªğ@×ƒGí”ü´AøLpş³àß¤î`F÷IP½±<˜²½ÄÂA¤ èJÌ\\çb¹–iˆ9TgqÂ0`î¯º½g)¡ö$Ğ³pß—{gÈè ‡=½²GÈ¾´cïU(¡ñÏgÜa÷a)PéÚˆuéé,ò]¾÷¬4(w(9#ƒáğêğøá:=öİåÊçC2Içu|,yÆ’¡½ì>tôÀ‰Li‚]
oedDo¿ŸE©ráÿ×EpÉòKÉŠVåfÑP?©¯i°²/üò‚»™ñ¿õ.ƒ«Ì«yDH ÃÌ3‹D:*qTÌ~Lo£ uzHŠ8VÖCg,ËDö!U¬8oÿ ‰|‡Ò|=’k¿#3</;‡$`]»ÁB~µÍ“é®¬¼…Ñú~›úÙKH¨Æ¯ƒt´•šçü+Ïğ /yº°éõy´gMC]x[g\p,éQ*`^M<ßTv°uÕQ„qş7:Û:Æ‰Dy¡k*ÏÉzNåuÀQpC2ÃWlM*R*WÅÿ®4D…¯ôn5E:œ¨êŞˆ.˜U;¯Ùiå¦©Á‰¦âKYPØ<¦ËHäAÜ\	g¢¤ ))Î (+íL:•¸¬µ˜ÅÓ­£CØF½zã´Ã5€¤AÀ¼¡íOÛ5Œzk·"}«®zÎ|aL	tâŸU¸|y˜Ï®Š@	E–'‚ùøY…êİV
2Æ	 úÿé0„€fCŸ®ê¦Õ70Atè§Òt¹À-1ŞBîè`ïF¤QlÈ	²îH¡ÏB=µ›­‘åA©¯hyXÎ.>´G¤³ÁÅú„€°/k®7ByS t‰ÑGFkŠ%7¶Tu^¿!ÍO,wº;@d¤&ú‘Å®ùv"h™æ!NÃîğHÊñÖIg%¿TL¨‡¯ëßN.è~µFşMŒ4×ªíYCîFıËQz°‹‰ËÅ] »§TÂŠ ú*TŸ³ˆËÿø‹êˆğ=òp>pñ6“hvaz:¦«#ºîöèy%Åµ)Q2îåH¯§QS9ôÚ*cCç×¬ç,ÛN÷TßLMÊE÷|¡b8Šêb›ƒŸ‰xçŸ%h›‘<úïßÔš–íôiôıÑÍ: Õå¥œ¯ ·~ÄF,Á?#\’e‘¹Ğ>ç-Ê•º…Ó0]X]4qF5N†xºùÂ¤Ã‹¥?Ú™uiÍÛ´AhRLñ$Íõ‡÷CË®bHÌ“‡=8(:ÙÓ›RÑoÙ&#­	ıÉ÷BÒ¨ ñ£©ÙÌxc'Â‰d• P§»y0ÃÉ—ø/6ÂÉíı°İúAô(@=™’c$–LUşú£¦ÿT)5Ğü`B!ÇuÔÑór?¿„Èã¢Ó(ª¥ûv Z¾R
Ö˜ºO{nßñ¡˜{Âô„%Ëá;Û‰á<`Ú'À®÷9\w¦ òo¾ª²Nğ×C’Ñh¬òáÏõqF¨{İëˆi°“>Ò±™Å!Ü‡ù§Õ•`ºø¸öÅ4Òg‡¥Ôíö·–»†Êañûë¿‹B¸Q6IáÛÔïoÑŒşçİ']Áîºƒùnv#-……zƒYZÖü+`üe,úËn¿»°îK[/¶NÍñ€u:0Ù°ivÉ„`İÓ7U"@_´3cüÛÛİF%Q„ï¤Kæ”6NŸRhJ0ÜÖ
şÒ)Œ¸/ÆuF—×ë±b¯æól®ÔifJ(Hä fúA©MIã^j_á-!P}Í@İÿû0£!y(´íí¯6½‰Éî”%üBl«4¶…|rgÛ
˜— vT^.š=håÅµ­Ğÿ_ûÙBa8­z¤¸˜0„Æ—}£ê ¢aÙ«ÉlA •'tcë%~*SÎb«ÇvïFPŒL+­¿ˆlĞ»Ï‘Òåİ€HÿAğ4âcfÿÇ@ôdY„4êÌi£DØG"c
P Gë×›:öe×ü„å²S$ö:¿Æ(ºöfG8›Š‰ªjS¡++/+m„tÅ?´GÉ|ˆ1Ùñ+X@½T†ş	,0wçrl·ms˜HcJÜH±ì|úâ„¯u¾x]Âğ©J&f‘Ä;ÅSt¿ŠÒu’4ñüÑÊ6“<‚@„?Oá¹,ĞT!ä/îÅ×şwa„Bõ”Ìß¬Ù?éQòŞ’š‘;ŞKXê–ºœGÊO‹ä‹7ÎC'wrYO¥3ÿ'¹XµhBÌÁmF^©hÓ"á(œÖH?˜AšJÒ\U*²íÂ÷Ì­–İÒ”.Ò`_£0+—rí‚çDt(¥	%‡¾üM:úÆ÷€	/ÿ<‡?t:GŸ¼J[]”,c±»’ S½ñäéŠŸFE´ëÉ§!ş’-kM½CŞ÷Üœ~púš‡"R‚Ó)lş J¾§Ü_©îBqî›Éé7FF%9>'¢ÃKÙÌë^¿ÉKä5»Á¾F¦-à¸zÇ>NaÀ„ôAæN‹[™“ç5IßW´¥ÀXHlå9•¦oqè×½·º,kàî#yIQ´b“gÛËTjÅK^¬i¹!FbAE¥„…ªFÖA’Bheœ+4!_õHLâ_L«ùˆkHzv0n&læ¹5Y zıZL8©‰hû‘Oğh¦ÜÇ­ç×ı^ì|il/Ù0¤\„EÖÿhà.-õ„SãÎ7#sJ M/Ã°O¯Lfe,ˆY¤/CLÙJX@]ôĞ^!	F\bDE®:=V 9»)Jùrİ'j„ØrßÉ¿D…~è4¡ŸäN¯ŞğE<ß”l8ÄøA“\¥I†ã°»³Üu8r§|˜V&¬’C?«ÃRí­9rk«†1ÕBü¿d^O{:·‡†{>…6ûñ>ı
'dÒp V{}4¥fÍp‹…€9âôG\ø 4ôO×êoŒhÇØí$òtø&à[ÓıãĞŠİ#QãY©@³‚‰İ¦øÌ]yìø^ßf¨hXËüÈl5—,.§Û¦—TÖª\¢]§ïXt²Õ:%ì<¼á©.+„“¼wï\v,uâÜjU1zÉ•µfÁÀÊ\[³“ˆ¼Â”Šİ_ÕGW+0çŸ]_›HŞ3î:Hi~úš>šèY”GVGoĞ‡,<™Üaˆ½(}¼)}ÓŒY·H$< "´)Q¥…,´ÿ<lº(¤r€œ6àÖÖ?ƒ¹ÉL¸¿1Y5ÔÉM…_^_Ç’ë¼ÿkwçI¨tÆ6#4VI /ÛºMš¸!ƒB>çänÚÓeŒğvyûiYÊ›¶òÒëÄÛ¶*…QÒÌ?úî.2¼åø€æÄÙÕNº«
ªj';E™õÉ$Dqş“„a £İÂ¿”÷yà—/¥o®Qáõ­.’y-–AÄ‹ñ–ÃÉĞèªä1Dß…×CO²ÑÁR)—>ºÎ^_İ”f1ûû*‰ûR&À‘¨¨bDCß:Lá¸¨(gÖ;Mİ`ònnÛã8âßª#‰Õb=^¾6ºFûá@R ?f-ì{9‚`A†1=ë<‰§ƒÉÁ¯¥R<ğ<#A_Å¡ê‹vHÁ¡DëƒišÂJ-»`Òì'³Ü}šøiÃ³òúÓG9‰I±qÊ·IÉ9#Õâëé”ÿ-:‡Äe>|â²¢TŠá¡ùqïaHš×bÇV¼ò
–¹<l’Â3Àâ¾Ì±]2R´2E>»|Éi‘ÊLæ
-©îñŸ&Œä\!`î[øa°ØHk|Ö‚£-G@!¬TÈ¼2OìÆò!Ê6',¯N31Œ—	Vñ%777ï·¥ÈqvÕİ‰™=ˆé+èl9æõjå
4±ë*ÙÑYóT³Cƒ™I‹$=B£
»ôÓ±GúŠìTÆ*‚¸õ4ßæ_¶À>LÉ1Ù‚zíÄÇc\.*ßg±T‚d\Tu›Á$¼ÿ9õÈm
åJN_räb#ò›1©(ÊéÊMÖ
™MrGjrÏ¹ï¨T…ıĞMÂ1>ü“ô.p6VÏ'Š¦ˆ(ğq –ÑMy^ß«0jT`×:pâ2±·kØm,ÅíTJ‡ÎƒšÙ”a>(<)æLÌw°ú¦ÁŞ·®dÛú¢†Š¨wJUìUšÁNMÇ‹ø‰§‘1nN»`kb—§sÂæë,G'´ov;Ïb€~N«1<Î-ı†ûêæq€ ”	Ñw‚RFedEö²“hÏ©e*‹j¯*]˜Hìt,ô¥,‚eJ©}ÁCGêyÄ-‘Î„t¿&ÉÄn­”Ç­xDƒºEÚ‰ÉòV.æÛƒ€ÉØï=B©ÙÊJ<À<!=- Õ”­²Ò ÇÍÓ#u*{D@ Ó$™4°xd ß4±*\PÏ†¹—/ßò¿,×ÈF€bl Õà»rŒ&eÀÛ(M`0MX”·…ı`‹ÈïZduûGÉ¨à_9(=«éæ\Ç·Ëvã¢˜v”#!Á©(rWSªZŠş&ß/‡RN"¥j¨!?¥MMÁ*='nëwL©Lr—®á
=Ê1}ÏÜeøeß8Î£
¬b»T‚²Tö¯x{ 	Y=›ÙÈ;% EbXUˆàmqE›°ÃÁ9KËËÑ¬ ‰Ïúî#Æñ®aCGğŠÊ¼©×é”æàI¿1–ºtLôi¬‰HÉŒ¬YM†”±Tökœú%Öfô©ã¥JQ²‡_E\Ëw3¿'4±ç/aÉäô^!_™`vRüÏ>š´lP9æãdAı+µâŞNI8iPs -·ÇR›´Ë)“ê!üV?ÒZí÷µÑÕ®«»€PºV<OËKzV¹âs„C.ç48F–] P2©óZDšhÏ—t"÷ÊÕhÛodV?Ú’Ê$/äõğîĞ	;?IzğDÆfIÔÕ2ÊBna¢Ó=‰ÀN;ï‘ñ	]ZRÀîFş”>Éêú©Û*%9B= é¥x> ËnV¹Î•ó*÷û¤ß ¨P¾·ÃÉµ’¤»~[µ,&gö‘•É]àŠtÚXîY*«'‰P$ˆùÒæÜ?0CxZ}sY0)/éHê–t©Ø~yìë)şÈÇWö@Ÿ‰ç&´WıëÙ´Y¡áj9¨g±6w– „wş{'¢ØÍCW¶ÌU%¿%—.ºõ"0ıôA½bS†;Ø ¹WæÜß4}ü4W]¨ÄYñ‘¾OIm[2›4Ü¤œA×·‚Pº‡°X×Ì»ÂÅd²Pn£*›W©ú²È§´öÏçŠ7¶ÇZC9q‰Ü&áƒ_b¦`QëùGd®Te¡C¶Ë³™ fD%²àç_ï³ÍKJ0šm;ÑR|P‚$ôs&““©c}:ü]
¬¼2,c?k«·Ï˜^®]Ÿ*húsâì ÿ*“¡È(Û/5ôA6JÛÍşˆ‡íGô·2EIˆËN×ûBâo<í(i10-ì=Ñ;ÆU*¢QºÕ*Â©I!¬91JÆ_)³Æ*?€™Å^0ÅR@E‡Ş-‰Éi§±&Œ>,ìuzÇ?PZp0nv ›ûhù\|¾Ø‘C`É?EJ•r&“ƒ Sß“Ùå‘ÅŸï™|¶õj ‹ÃD¿ıšÈÓÕ’èr@6|Å/…|+`#À}aßÑæ3	ánR€NİSEşç¥è¦…sÇÉ›g49¼ÌÅ×%ˆùöÁ©æ8C=ĞR:{hê· |ú¦3™¿¶b?¡©¼Äı“L¹˜æñ—ô4€æ¿3úXiÀ€Æ:>=¸¢|çò=Ù^}¶cËÛûúÑ›T‚ïX4aUeÎıAµYÇ+ò£Y¦*ÅQ9`H+¬¤”_¯ _‡L‰Äf””l¤ÿDªV’Î±Ä_
ƒûö8Ìäö¯µØ¼\rj£MòÙkŒƒ¢©$Åd¸w›ïF„Qähé§1”…†Ê  cOÁMÎ‹“šn»etU’Hbõpdï¦{öÿLB\½ş ÍiËr|8a­½ÔF¥‹«¢ı)ÏÃ¼©1Ißæ}p†.ë00PÓÔÕŸæºª5EœÙ"­‚š=İ¤4<Î,ñt±=“œ Æü>“¼®ñå,¸X´ÿ%„vÿE<€»®õQp×*Ó/ªv÷3Æg›ĞV¶şlÍU¹Ûe´5;ßê¼ÅFÔük²¡TÑÇ©ÄŞ=æ‡‘Gbî?èù9ÜÁG°¦fçé,6æØgVözõdñiªK?Ø`¡j¿ğ0Õ	L£• ëşC†jiÔa±Şfq
.ˆë:ÀÆ.øt$M’°Qm9ÌœOÆxÂG—!›8éü“óšáëWÂJ…?T= RgŞyº×.fPƒ¢ Ü!g‹Ëßìïâ0:›»eîëûJDn]`Ó®$Ôü¿ùˆB8gp‰Í%åøŒ×U‡†ËÅ——·;ËÙ
/†°øfÉ MsÃ˜>Jq—çküÄ÷ÃÈCË™˜^äf8 ñş˜é®°1bb~¹`7ÿ€vy¼‰ªµºç!Âå…ïŸ‘>m%Åñ+8Ä&óó;¾l+˜Å¥|Áôûƒÿˆ$Íàe¥ÁŒ¬<üéªk»oŠ3ñ†¹´İ
ì*ã0ıó€XWÊl ƒ½kÊÓØ--J°Ö¨h/­êTgk Îlë1,mÃ"šs×ÊG.6ª iWdV=çlúèõªù ,,‰¿’ZËé‰©p¾šĞ<öwÈs,®U¬OŸR»·¿‚ X·!p‘zÜyı°5'_ºiB"Š²Ia;A{YZ_+Ò5ôè†
ÇD×&yÆQ2ıò9v'émÍÓ¸Tşş–^îø*¦ıVbP@g<ÕÃ~3·Fhkd9·­Kš‘²¥èë. BT}‘¤“1º¬Îø²4†œcı§¢„&Á°À}Àn¶;Çí9ı?q!læ†–7ş°…Á±‰o[C