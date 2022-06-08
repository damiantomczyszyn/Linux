rity,
 *				p->dl.dl_{runtime, deadline, period, flags, bw, density}
 *  - sched_setnuma():		p->numa_preferred_nid
 *  - sched_move_task()/
 *    cpu_cgroup_fork():	p->sched_task_group
 *  - uclamp_update_active()	p->uclamp*
 *
 * p->state <- TASK_*:
 *
 *   is changed locklessly using set_current_state(), __set_current_state() or
 *   set_special_state(), see their respective comments, or by
 *   try_to_wake_up(). This latter uses p->pi_lock to serialize against
 *   concurrent self.
 *
 * p->on_rq <- { 0, 1 = TASK_ON_RQ_QUEUED, 2 = TASK_ON_RQ_MIGRATING }:
 *
 *   is set by activate_task() and cleared by deactivate_task(), under
 *   rq->lock. Non-zero indicates the task is runnable, the special
 *   ON_RQ_MIGRATING state is used for migration without holding both
 *   rq->locks. It indicates task_cpu() is not stable, see task_rq_lock().
 *
 * p->on_cpu <- { 0, 1 }:
 *
 *   is set by prepare_task() and cleared by finish_task() such that it will be
 *   set before p is scheduled-in and cleared after p is scheduled-out, both
 *   under rq->lock. Non-zero indicates the task is running on its CPU.
 *
 *   [ The astute reader will observe that it is possible for two tasks on one
 *     CPU to have ->on_cpu = 1 at the same time. ]
 *
 * task_cpu(p): is changed by set_task_cpu(), the rules are:
 *
 *  - Don't call set_task_cpu() on a blocked task:
 *
 *    We don't care what CPU we're not running on, this simplifies hotplug,
 *    the CPU assignment of blocked tasks isn't required to be valid.
 *
 *  - for try_to_wake_up(), called under p->pi_lock:
 *
 *    This allows try_to_wake_up() to only take one rq->lock, see its comment.
 *
 *  - for migration called under rq->lock:
 *    [ see task_on_rq_migrating() in task_rq_lock() ]
 *
 *    o move_queued_task()
 *    o detach_task()
 *
 *  - for migration called under double_rq_lock():
 *
 *    o __migrate_swap_task()
 *    o push_rt_task() / pull_rt_task()
 *    o push_dl_task() / pull_dl_task()
 *    o dl_task_offline_migration()
 *
 */

void raw_spin_rq_lock_nested(struct rq *rq, int subclass)
{
	raw_spinlock_t *lock;

	/* Matches synchronize_rcu() in __sched_core_enable() */
	preempt_disable();
	if (sched_core_disabled()) {
		raw_spin_lock_nested(&rq->__lock, subclass);
		/* preempt_count *MUST* be > 1 */
		preempt_enable_no_resched();
		return;
	}

	for (;;) {
		lock = __rq_lockp(rq);
		raw_spin_lock_nested(lock, subclass);
		if (likely(lock == __rq_lockp(rq))) {
			/* preempt_count *MUST* be > 1 */
			preempt_enable_no_resched();
			return;
		}
		raw_spin_unlock(lock);
	}
}

bool raw_spin_rq_trylock(struct rq *rq)
{
	raw_spinlock_t *lock;
	bool ret;

	/* Matches synchronize_rcu() in __sched_core_enable() */
	preempt_disable();
	if (sched_core_disabled()) {
		ret = raw_spin_trylock(&rq->__lock);
		preempt_enable();
		return ret;
	}

	for (;;) {
		lock = __rq_lockp(rq);
		ret = raw_spin_trylock(lock);
		if (!ret || (likely(lock == __rq_lockp(rq)))) {
			preempt_enable();
			return ret;
		}
		raw_spin_unlock(lock);
	}
}

void raw_spin_rq_unlock(struct rq *rq)
{
	raw_spin_unlock(rq_lockp(rq));
}

#ifdef CONFIG_SMP
/*
 * double_rq_lock - safely lock two runqueues
 */
void double_rq_lock(struct rq *rq1, struct rq *rq2)
{
	lockdep_assert_irqs_disabled();

	if (rq_order_less(rq2, rq1))
		swap(rq1, rq2);

	raw_spin_rq_lock(rq1);
	if (__rq_lockp(rq1) == __rq_lockp(rq2))
		return;

	raw_spin_rq_lock_nested(rq2, SINGLE_DEPTH_NESTING);
}
#endif

/*
 * __task_rq_lock - lock the rq @p resides on.
 */
struct rq *__task_rq_lock(struct task_struct *p, struct rq_flags *rf)
	__acquires(rq->lock)
{
	struct rq *rq;

	lockdep_assert_held(&p->pi_lock);

	for (;;) {
		rq = task_rq(p);
		raw_spin_rq_lock(rq);
		if (likely(rq == task_rq(p) && !task_on_rq_migrating(p))) {
			rq_pin_lock(rq, rf);
			return rq;
		}
		raw_spin_rq_unlock(rq);

		while (unlikely(task_on_rq_migrating(p)))
			cpu_relax();
	}
}

/*
 * task_rq_lock - lock p->pi_lock and lock the rq @p resides on.
 */
struct rq *task_rq_lock(struct task_struct *p, struct rq_flags *rf)
	__acquires(p->pi_lock)
	__acquires(rq->lock)
{
	struct rq *rq;

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
	θόÿÿÿΓ΄&    v θόÿÿÿS‰Σθόÿÿÿ‰ΨΊ    [ιόÿÿÿfffθόÿÿÿ1ΐΉÿ  ;
‰ΑJMΒΓ΄&    fθόÿÿÿV‰ΘΎÿ  S‹	‹XƒΑƒΣ 1Ò9Ξ‰‰Φ‰Xή[^LΒΓt& θόÿÿÿΓ΄&    v θόÿÿÿSƒμ‹ϋÿ  …ΫtI‰Ϊθόÿÿÿ1ΐƒΔ[Γv λ   t‰ΪθόÿÿÿƒΔ1ΐ[ΓfΊKΧ ‰$θόÿÿÿ‹$‰Ϊθόÿÿÿλή΄&    Ί7Χ ‰$θόÿÿÿ‹$λ¥fffffffθόÿÿÿ‰Π‹    S‰ΣΑϋ;Ί    XLΒ[Γ΄&    ΄&    θόÿÿÿW‰ΘVS‹	‹XƒΑ‰‹    ƒΣ ‰X‰ΧΑÿ9ΚΊ    ί[^LΒ_Γ΄&    t& θόÿÿÿΓfffffθόÿÿÿU1ΙΊ
   WVS‰Γƒμd΅    ‰D$1ΐ‰ΰθόÿÿÿd΅    θόÿÿÿ‹-    ‹pθόÿÿÿ‹|$‹L$ƒξ‹$VUP‰ψΑο%ÿ  kΐdΑθP‰ΘΑι%ÿ  WkΐdΑθP‰ΠΑκ%ÿ  QkΐdΑθPRh$u SθόÿÿÿƒΔ,‹D$d+    u
ƒΔ1ΐ[^_]ΓθόÿÿÿfffffffθόÿÿÿΓ΄&    v θόÿÿÿUWVS‰Γƒμhd΅    ‰D$d1ΐD$$θόÿÿÿD$$θόÿÿÿθόÿÿÿ‹5L   1Ò‰D$΅   …φHς+t$@…ΐHΒ)ΖΈ    Hπ‹    1Ι…ÒHΡ‰Tƒΐƒψuηθόÿÿÿ1Ò‰Η΅   …ΐIΠ΅   ‰Υ1Ò…ΐIΠΑζ‰T$1Òj‹D$8RΊwΧ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$HRΊΧ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ½    1Ò1ÿjRΊΧ P‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Òj‹D$DRΊ―Χ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨθόÿÿÿΊΐΧ ‰ΨjWVθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅   1Òj…ΐHΒ1ÒRΊΡΧ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ‹t$‹D$1Òj‰D$πΑΰ‰t$RΊβΧ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ‹|$‹t$$1Òj7RΊσΧ ΑζΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$ RΊΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ½    1Ò1ÿjRΊΨ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$RΊ&Ψ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨθόÿÿÿΊ7Ψ ‰ΨjWVθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿj‹D$$Αΰ1ÒRΊHΨ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅   1Òj…ΐHΒ1ÒRΊYΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$lRΊjΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Òj‹D$XRΊ{Ψ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$D+D$`RΊΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$T+D$pRΊΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Òj‹D$HRΊ®Ψ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$XRΊΏΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅P   1Òj…ΐHΒ1ÒRΊΠΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅T   1Òj…ΐHΒ1ÒRΊαΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅D   1Òj…ΐHΒ1ÒRΊςΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅H   1Òj…ΐHΒ1ÒRΊΩ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Òj‹D$@RΊΩ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅   1Òj…ΐHΒ1ÒRΊ%Ω θΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹|$(RΊ6Ω /ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Ò­    jRΊGΩ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Ò½    jRΊXΩ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅”   1Ò…ΐHΒPhiΩ Sθόÿÿÿ΅   1ÒƒΔ$…ΐjHΒ1ÒRΊ‚Ω ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨθόÿÿÿΊ“Ω ‰Ψjj j θόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅    1Òj…ΐHΒ1ÒRΊ¤Ω ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅X   1Òj…ΐHΒ1ÒRΊµΩ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿθόÿÿÿ1ÒΑΰjRΊΖΩ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$$RΊΧΩ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅    -  ¶ %  ΰÿ+    -  ΐ Αθ
PhθΩ Sθόÿÿÿθόÿÿÿ1ÒΑΰjRΊΪ P‰ΨθόÿÿÿΉ   ΊΧ ‰ΨθόÿÿÿΊΪ ‰Ψjj j θόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿθόÿÿÿ1ÒΑΰjRΊ#Ϊ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅p   1Òj…ΐHΒ1ÒRΊ4Ϊ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅`   1Òj…ΐHΒ1ÒRΊEΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅d   1Òj…ΐHΒ1ÒRΊVΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅h   1Òj…ΐHΒ1ÒRΊgΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅l   1Òj…ΐHΒ1ÒRΊxΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅    1ÒjΑΰRΊ‰Ϊ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅(   1Òj…ΐHΒ1ÒRΊΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ‰Ψθόÿÿÿ‰ΨθόÿÿÿƒΔ‹D$dd+    u
ƒΔh1ΐ[^_]Γθόÿÿÿfffffθόÿÿÿ΅    S‰Σ1Ι‹    ƒΐΑΰPΊ`3 P‰ΨθόÿÿÿZ[ΓfθόÿÿÿU‰εS‰Γ£    r‹C0‹S4‹]όΙΓ‰Π1Òθόÿÿÿ‰Α!ΡƒωÿtβiΚθ  Ίθ  ‹]όΙχβΚΓ΄&    v θόÿÿÿU‰εS‰Γ£    r‹C(‹S,‹]όΙΓ‰Π1Òθόÿÿÿ‰Α!ΡƒωÿtβiΚθ  Ίθ  ‹]όΙχβΚΓ΄&    v θόÿÿÿU‰ΕΉ
   WVSμ  d΅    ‰„$  1ΐ$   ‰ίσ«„$„   θόÿÿÿ‹΄$„   ‹Ό$   d΅    ‹€¤  ‹P‹„$   +r$z(+B,‰ω1ÿ™R‰ςP„$   θόÿÿÿ‹„$   ‹”$   ‰„$   ‹„$    ‰”$   ‰„$”   XZ„$Δ   ΗD$    ΗD$     ΗD$t    ΗD$x    ΗD$l    ΗD$p    ΗD$d    ΗD$h    ΗD$\    ΗD$`    ΗD$T    ΗD$X    ΗD$L    ΗD$P    ΗD$D    ΗD$H    ΗD$<    ΗD$@    ΗD$4    ΗD$8    ΗD$,    ΗD$0    ΗD$$    ΗD$(    ΗD$ÿÿÿÿ‰D$Έ    ‰D$|‰¬$€   ‰ύ‹D$Ί    θόÿÿÿ‰D$;    ƒ’  ‹T$‹|$Ή   „$Δ   ‹t$|‰D$4•    σ¥‹΄$Δ   ‹Ό$Θ   t$$‹΄$Μ   |$(‹Ό$Π   t$,‹΄$Τ   |$0‹Ό$Ψ   t$4|$8θόÿÿÿD$<‹D$T$@‹T$θ1ύÿÿD$DΉ    ‹΄$δ   T$H‹Ό$θ   t$L‹΄$ά   |$P‹Ό$ΰ   t$T‹΄$ό   |$X‹Ό$   t$\‹΄$  |$`‹Ό$  t$d‹T$|$h‹΄$  ‹•    t$l‹Ό$  |$p1ÿ‹4‰Π‰$θόÿÿÿ‹$Ζ‰$‹\$ Χ‹T$t$t‰L$‹L$|$x‹<•    1φλ΄&    ‹$‹,²‹D$‹$°‹DΕ‰,²1ÒΑΣƒΖƒώ
uΩ‰\$ ‹¬$   ‰L$‹$ιZώÿÿ΄&    v ‰l$|‹¬$€   »ÿÿÿÿθόÿÿÿD$tT$x‰D$t‹D$$‰T$x‹T$(θόÿÿÿRΊ°Ϊ P‰θθόÿÿÿ‹D$4‹T$8θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$D‹T$HθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$T‹T$XθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$d‹T$hƒΔ θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$T‹T$XθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$d‹T$hθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$t‹T$xθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹„$„   ‹”$   ƒΔ θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$t‹T$xθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‰θΊ
   θόÿÿÿΈ    ƒΔ‰D$Tι  t& ‹|$‹t$TΉ   4    σ¥‹”$Π   ‹$Ψ   ‹„$Μ   ‹΄$Δ   ‰T$‹”$Τ   ‰L$$Δ   ‹Ό$Θ   ‰$‰Θ‰T$‰Ϊ‰L$θόÿÿÿ‰D$‹D$‰T$‰ΪθJϊÿÿ‹$θ   ‰T$(‹”$δ   ‰L$0‹$ΰ   ‰T$,‹”$ά   ‰L$8‹$   ‰T$4‹”$ό   ‰L$@‹$  ‰T$<‹”$  ‰L$H‹$  ‰T$D‹”$  ‰L$P‰D$$‰T$LShΈΪ Uθόÿÿÿ‰π‰ϊθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$‹T$θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$$‹T$(θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$4‹T$8ƒΔ$θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$,‹T$0θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$<‹T$@θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$L‹T$PθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$\‹T$`ƒΔ θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$L‹T$PθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$\‹T$`θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿΊ
   ‰θθόÿÿÿƒΔ‰ΨΊ    θόÿÿÿ‰Γ;    ‚Ούÿÿÿt$xΊΎΪ ‰θÿt$xθόÿÿÿ1ΐθόÿÿÿ‹    ^‰ΗX9ϋvg1Ϋt& ‰ώ)ήt'΄&    v »   Ί ` ‰θ9ήFήθόÿÿÿ)ήuγ‰ψ_θόÿÿÿ1ÒRΊ¶Ϊ P‰θθόÿÿÿ‰ΨθόÿÿÿZY‰Η΅    9Ηr¥‰Ϊ‰Γ)Σ…Ϋt v Ύ   Ί ` ‰θ9σFσ6θόÿÿÿ)σuγθόÿÿÿ‰D$θόÿÿÿ‹΄$„   ‹Ό$   ‰Γ΅    ‰$θόÿÿÿ‹L$QSÿt$WVRPhLu Uθόÿÿÿ‰θΊΔΪ ƒΔ$ÿt$ ÿt$ θόÿÿÿ$¨   Y^‹D$|‹t$λ
t& ‹ƒΓ1ÒRΊ¶Ϊ P‰θθόÿÿÿXZ9σuε‰θΊ
   θόÿÿÿ‹„$  d+    uΔ  1ΐ[^_]ΓθόÿÿÿfffffffθόÿÿÿU½    WVS»ÿÿÿÿƒμx‰D$d΅    ‰D$t1ΐΗ$    ΗD$    λ/΄&    f‰ΪD$$|$$Ή   ‹4    ξσ¥θόÿÿÿ$T$‰ΨΊ    θόÿÿÿ‰Γ;    rΔΈ   θόÿÿÿ‰Ρ‰ΒD$θόÿÿÿd΅    ‹€¤  ‹@‹H$‹X(L$‹@,\$D$™R‰Κ‰ΩPD$ θόÿÿÿ‹\$‹|$ ‹l$(XZϋÿΙ;v‰ΪΉƒK Ακ	‰ΠχαΑκiÒ Κ;)Σ‹4$Ή Κ;‰Ϊ‰πχρΉkΚ_k‰Ζ‰Π‰ΣχιΑϋ‰θΑύΑϊ)ΪRχιVΑϊ)κRWhΤΪ ÿt$θόÿÿÿƒΔ‹D$td+    u
ƒΔx1ΐ[^_]ΓθόÿÿÿfffffθόÿÿÿVS‹X‹pΎƒώ~<0t-‰Ω1Òλϊ—™™w’P‰π)ΘΨ…ΐ~Ύƒθ0ƒΑƒψ	vΪΊÿÿÿÿ‰Π[^Γθόÿÿÿ‰Αd΅    ‹¤  ‹RΒΓ   R‹¤  ‹RΒ‚   R‹€¤  ÿph    Qθόÿÿÿ1ΐƒΔΓffffθόÿÿÿUΊϊΪ W‰ΗVS»ÿÿÿÿθόÿÿÿλt& ShΫ WθόÿÿÿƒΔ‰ΨΊ    θόÿÿÿ‰Γ;    rΫΊ
   ‰ψ1φ½    θόÿÿÿ΄&    ÿ4µ    »ÿÿÿÿhΫ WθόÿÿÿƒΔλ t& Dµ     ÿphΫ WθόÿÿÿƒΔ‰ΨΊ    θόÿÿÿ‰Γ;    rΞΊ
   ‰ψƒΖθόÿÿÿƒώ
u[1ΐ^_]ΓfθόÿÿÿU‰ΝΉÿ΅  W‰ΧV‰ΖS‹@Tθόÿÿÿ…ΐt0‰hψ‰Γ‰ΒΗ@ΐa ‰ψθόÿÿÿ‰πΊ    θόÿÿÿ‰ς‰Ψ[^_]ιόÿÿÿf[Έώÿÿÿ^_]Γ¶    θόÿÿÿU‰ΕWVS‰Σ1Òƒμ‹@‹@Ψθόÿÿÿ…ΐ„™  ‹S‰Η‹C‰Φ	Ζ„©   ‰Αƒρ	Ρ„Χ   Ή   9ΑΉ    Ρ|Y4…80 ώd0 vλHv ƒCƒS ƒΖώd0 w2‹‹
‰T$‰Θ‰$θόÿÿÿ‹T$R‰ΪWh€'3 P‹L$‰θθόÿÿÿƒΔ„ΐu»OΈÿÿÿÿπΑGƒψ„Ν   1Ϋ…ΐΫ   ƒΔ‰Ψ[^_]Γ΄&    f‹Ej1ÒΉ   ‹@ ‹@(RΊ#Ϋ P‰Ψj j ‹3θόÿÿÿƒΔ…ΐu ΗC   ΗC    ‹U‹3BH‰$‰D$θόÿÿÿ‹$‹D$‹R‹R ‹R(‰$θόÿÿÿ1ÒΉ   j‹D$RΊ%Ϋ P‰ΨÿsÿsθόÿÿÿƒΔ…ΐ…:ÿÿÿΗC   Ύ@0 ΗC    ιπώÿÿt& ‰ψ1Ϋθόÿÿÿ‰ΨƒΔ[^_]Γt& ‰ΘΊ   θόÿÿÿ‰ΨƒΔ[^_]Γ»ώÿÿÿιÿÿÿ΄&    θόÿÿÿUW‰ΟV‰ΦSƒμ<‹@ d‹    ‰T$81Ò‹hψ‹@Ψθόÿÿÿ…ΐ„΅   Ί	   ‰ΓθόÿÿÿΉσÿÿÿ„ΐu0sΈÿÿÿÿπΑCƒψtN…ΐ~b‹D$8d+    upƒΔ<‰Θ[^_]Γv U‰ΩΊ2   D$
θόÿÿÿ‰ΑX…ΙxΈL$‰ϊ‰πθόÿÿÿ‰Αλ§΄&    ‰$‰Ψθόÿÿÿ‹$λ§΄&    fΊ   ‰π‰$θόÿÿÿ‹$λΉσÿÿÿλƒθόÿÿÿθόÿÿÿWVΎφÿÿÿSƒμd‹    ‰L$1Ι…ΐtM‹BΨ‹zψ1ÒΎσÿÿÿθόÿÿÿ‰Γ…ΐt5Ί	   θόÿÿÿ„ΐt‰ω‰Ϊ‰ΰθόÿÿÿ‰Ζ…ΐt0KΈÿÿÿÿπΑCƒψtF…ΐ~*‹D$d+    u>ƒΔ‰π[^_Γt& ‰ΰθόÿÿÿ‰ΖλΕt& Ί   ‰ΘθόÿÿÿλΘ΄&    v ‰ΨθόÿÿÿλµθόÿÿÿfθόÿÿÿU‰Υ1ÒWVSƒμ‹@Ψθόÿÿÿ‰D$…ΐ„   ‰,$‹}»@0 ‹3‹.‰θθόÿÿÿ9Ηu‹$‰ω‰κ‹@θόÿÿÿ…ΐt>ƒΓϋh0 uΣ»ώÿÿÿ‹T$ΈÿÿÿÿJπΑBƒψt0…ΐ~SƒΔ‰Ψ[^_]Γ΄&    v ‹,$‹T$‰ρ‰θθϋÿÿ‰ΓλΎt& ‹D$θόÿÿÿ‰ΨƒΔ[^_]Γt& ƒΔ»ώÿÿÿ‰Ψ[^_]Γ‰ΘΊ   θόÿÿÿ‰ΨƒΔ[^_]ΓfffffθόÿÿÿW‰ΟVS‹R‰Γd΅    ‹’  ‹
Ί   θόÿÿÿ…ΐtE‰Ζ΅   …ΫtJΉ   Ίΐ  θόÿÿÿ‰Γ…ΐtUVh(Ϋ SθόÿÿÿΗ    ‰_ƒΔ‰Ψ[^_Γ΄&    »ώÿÿÿ‰Ψ[^_Γt& Ή   Ί 
  θόÿÿÿ‰Γ…ΐu¶»φÿÿÿλΗt& »τÿÿÿλΌ΄&    fθόÿÿÿW|$ƒδπÿwόU‰εWVS‰Γƒμ<d΅    ‰Eδ‹ƒ  ‰EΔ‹C<‹x ƒΗx‰ψθόÿÿÿ‹C<Ί+Ϋ θόÿÿÿ…ΐ„   ‰Ζ‰Ψθόÿÿÿ‰Γ…ΐ„ƒ   ΅TK  ‰Ϊ‰C(EΘθόÿÿÿ‹MΘ‰KT‹UΜ‰SX‹EΠ‰S@‰SLΊÿ΅ÿÿ‰C\‰K<‰CD‰KH‰CP‰πf‰‰ΪΗC    ΗC    ΗC@b θόÿÿÿ‰ψθόÿÿÿ‹EΔ‰p1ΐ‹Uδd+    u,eτ[^_]gψ_Γ‰πθόÿÿÿ‰ψθόÿÿÿh”u θόÿÿÿXΈτÿÿÿλΘθόÿÿÿfθόÿÿÿUW‰ΟVS‰Γd‹5    ƒμ‹BΊ   ‹€  ‹‰π‰$θόÿÿÿ‹$1Ò‰Ε‰πθόÿÿÿ…ΐtM‰Ζ΅   …ΫtRΉ   Ίΐ  θόÿÿÿ‰Γ…ΐt]VUh0Ϋ SθόÿÿÿΗ    ‰_ƒΔƒΔ‰Ψ[^_]Γ΄&    v ƒΔ»ώÿÿÿ‰Ψ[^_]ΓΉ   Ί 
  θόÿÿÿ‰Γ…ΐu®»φÿÿÿλΐt& »τÿÿÿλµ΄&    fθόÿÿÿW|$ƒδπÿwόU‰εWVS‰Γƒμ<d΅    ‰Eδ‹ƒ  ‰EΔ‹C<‹x ƒΗx‰ψθόÿÿÿ‹C<Ί;Ϋ θόÿÿÿ…ΐ„   ‰Ζ‰Ψθόÿÿÿ‰Γ…ΐ„ƒ   ΅XK  ‰Ϊ‰C(EΘθόÿÿÿ‹MΘ‰KT‹UΜ‰SX‹EΠ‰S@‰SLΊÿ΅ÿÿ‰C\‰K<‰CD‰KH‰CP‰πf‰‰ΪΗC    ΗC    ΗCΐb θόÿÿÿ‰ψθόÿÿÿ‹EΔ‰p1ΐ‹Uδd+    u,eτ[^_]gψ_Γ‰πθόÿÿÿ‰ψθόÿÿÿhΔu θόÿÿÿXΈτÿÿÿλΘθόÿÿÿfθόÿÿÿƒβ@u‹@ ‹@θ‹@…ΐ”ΐ¶ΐΓt& ΈφÿÿÿΓ΄&    v θόÿÿÿ‹@ ‹@θ‹@…ΐ•ΐ¶ΐΓ΄&    fθόÿÿÿU‰Ε‰ΘW‰ΟV‰ΦSθόÿÿÿ‰Γ‰π)θD=ÿ  !‰Ω‰ϊ‰πθόÿÿÿDΖ/Ζ  [^_]Γ¶    [1ΐ^_]ΓθόÿÿÿVS‹t$‰Σ‰Κ9σ‰ρNΛ)σθόÿÿÿ…ΐDΓ[^Γ΄&    t& θόÿÿÿWVS‰Γ‰Π‰Κ‹s ‹L$»   …φtA9Au<‹Y‰Α‰Ψθόÿÿÿ‰Γ…ΐu3‹vθ…φt,ΈD ‹~θόÿÿÿ‹F…ΐt(ΈD »   θόÿÿÿ‰Ψ[^_Γv »   ‰Ψ[^_Γt& ‹…Òt"‰ψθόÿÿÿ‰ΓΈD θόÿÿÿ…Ϋ”Γ¶ΫλΔt& ΈD θόÿÿÿλ³t& θόÿÿÿW|$ƒδπÿwόU‰εWV‰ΦS»τÿÿÿƒμ<‰MΔ‹~d‹    ‰Uδ1Òθόÿÿÿ…ΐ„Γ   ‰Γθόÿÿÿ‰C(ΈD θόÿÿÿ‹V…Ò…  ‹EΔ‰sθN(Sπ‰Cμ‹F(‰Kτ‰Cπ‰V(…ΐt‰PƒFΈD θόÿÿÿ‰ΪEΘθόÿÿÿ‹MΘ‰KT‹UΜ‰SX‹EΠ‰S@‰C\‰CD‰CP‹EΔ‰SL‰K<‰KH·@‰Βfβ πfϊ @tOf €ΗC e f‰Ηƒψ   €e ‹8…ÿt`C‹UΔKP‰πθόÿÿÿX‹Eδd+    ujeτ‰Ψ[^_]gψ_Γt& f‰ΗC d Ηƒψ   €d ‹x    uª‰Ψθόÿÿÿλ΅΄&    ΗC    ΗC    λΆΈD θόÿÿÿ‰Ψ»ώÿÿÿθόÿÿÿλθόÿÿÿv θόÿÿÿV‰ΞS‹H‹I<…ΙtMθόÿÿÿ‰Γd΅    ‹€p  ‹@…ΐuΑϋ‰ΨΊσÿÿÿ[χΠ!π^ƒΰEΒΓt& 1ΐθόÿÿÿ‰Β‰ΨΑψ…ÒEΨλΣv ·Zλ΄΄&    v θόÿÿÿWVS‹z φu8Έ    ‰Λθόÿÿÿ‰Ζ…ΐu‰ϊΈ    ‰ΩθόÿÿÿΊ   ‰ψθόÿÿÿ‰π[^_Γ¶    ΎÿÿÿÿλνθόÿÿÿUΉ   W‰ΗVS‰ΣΊdΫ ƒμd΅    ‰D$1ΐ‰ψΗD$    ‹l$,ΗD$    θόÿÿÿ…ΐ…ο   ¶GO‰$ƒθ.<‡Έ   …Ϋ„B  ‰Ψθόÿÿÿ‰Η…ΐ„1  ‹E …ΐ„ξ   ÿ4$hpΫ hΐ  θόÿÿÿ‰ΖƒΔ…ΐ„Q Ή/   Ί.   θόÿÿÿ‹E Ή   ‰ς‰D$‹ ‰D$j D$θόÿÿÿY‰Ε= πÿÿ‡5Q D$‰Ϊ‰ωP‰θθόÿÿÿZ…ΐQ 9Η…ΩQ 1Ò‰θθόÿÿÿ…ΐ…ΑQ ‰πθόÿÿÿ1ΐ‹T$d+    …‰   ƒΔ[^_]Γ΄&    Ύ€c ΈGΫ λt& ‹FƒΖ…ΐtΔ‰ϊθόÿÿÿ…ΐuλ‹F‰$…ΐ…σώÿÿλ©v ΈkΫ θόÿÿÿ‰Ζ…ΐ„―Q θόÿÿÿ‰E ‰πθόÿÿÿ}  πÿÿ†βώÿÿιQ v Έκÿÿÿιfÿÿÿθόÿÿÿ΄&    fθόÿÿÿUWVS‰Σƒμ‰D$…ÒtN‰Νλ¶    tN‹[…Ϋt;‹s‰Ψ+F$Αψΐ‹<‹‰Θ‰$θόÿÿÿP‹T$ ‰θ‹L$θΘϊÿÿZ…ΐyΓ‹[…ΫuΕƒΔ‰Ψ[^_]Γf‹D$‰ϋ‰0ƒΔ‰Ψ[^_]Γ΄&    ΄&    θόÿÿÿUWV‰ΖSƒμ‹:‰$d΅    ‰D$1ΐ…ÿ„΅   ‰T$‰Υ‰ϋ‰Ψθόÿÿÿ‹V,‰ΩPD$θ+ÿÿÿZ…ΐt·Pfβ πfϊ 