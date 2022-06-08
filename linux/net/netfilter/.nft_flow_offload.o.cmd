q;

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
	/* Reset on sched cl®+v_µψ-πθkΕθs6-λtΫιΞ•7%“p"8ρ²Ρ6έ“!J]3’FΕΗ;™γhΓαfrLρή²KlΜ©OcΰΡBΡοnφ9γΝΜυu4έ!0³!ρΊ¤ƒ.μQ£y4ÒΦμ¤C^Ί(Σ•ηξΑ^v:Θιώ”`hdμΌn}‰μΒcέΜΗ΄LΈβ.ΐ|nyξu£‡&@χ(γ‡]ΐ¬΅0| ’½Wόbg~UΞXI­ρ'¤7φ]%~OxζΛ7z Zwª,7α@ΰxbDΛ΅ ϋΖm£yλ
 ƒσ¤ρΩζ«‰lt4Γ~Ή½³΄XU¤uI!<­sΠIq:EriΫτΙ™Z•p'a*ωEs.΄,™	Λ?=π/\οi"£άcΨ[#yWΞ2ρ&σΐ!GyΣB$G2ϊO4λƒ&ζuWHΌT7» _B_Ω\xΧZ)ώlψI{χ§ι¨G)ίaΠψw€—ρΊª3ΚυΘμlJG¦‹ZQΧ—Γςƒ”¨ΐRMΛ±®ϋ•ε
Π8ΚΙ&
…8RςE7%¦b]rmE~Ci9Α¬†c°ψ“DJVYkW΅—WΉτΚ#ωHΰΖ~—”γO5S@ΟXj²Ν_“f4¶Ή™^Ζ Β5…ΰ†¤³2bύ΅–2ΆΫλ]vjνηΑΘ¦ηeΘ“¨”Ν΄ι'IφQΎ¬ …’¤\ΌυΆ=eoΞ{ρ*/&Φ=>†„Θ—Xπτ%m9Ξ{b/:Ά›O’ `(ª%α΄“υXΜΝ.(φζωCΗBxZeω’Cην}„¨«λΥRΏΛWφsχ«²pÒη”wη$§MAΚ
ZΆ¥VϋΜ‚fe £}Θ²]ζxΘ«―ςΒ†a*έ°y¤Οε™Sχ°'N‡Η»ΰ$ς-Y°µ¦ Δ~΅:ΙΈ^ρBrΞρΔβξ«ag²οk‡ΣΩΎ»ΆΌΫΤf›0f¶³¤θΟπl"„¥υ†bΘΣYμΉ-SΩ 'αΡΰΦό©“f}@Xλά~ψg«ώG°0§=0=NF+3'K΅ϊΘ”³‹j«ξ„7Be•ΈΨΆIΊΈΝssΨyQ½m›~i™aθ0dΥσχΨY€‡0Φ]Uνε¦Bd*rƒψΘα:β%”°ςΕ_(λft0ϋG’»6,—¨\Ή¦0B Τ§ΕάΓν	`j«¶PXβCw^µ³θφJ”ΧXVηΕν=rΕC.Π6’η¬_Tφ"άςW»ƒδδ$ÒnDE©ªΤΒ[n{—3,»obφtΊ7ΙUKΥ―Η^ΊrΗ8Ά¬Kd
ΐΚωΕυήΩΆέ–Ύ·cεπΨ)M>­@pj‡u!»χ.ώ™Ί'"dT¦'®rF/ΘζAVμMΈΦ­±]‹oξ]χ"³!ÿπ—zΤ–_EBΪZ«w)~ξ’:ξΐPyG•OiJ•6ΈΕα–&cƒ|I!GΔφP’\»kb££΄Ο
©Ψήϊ5―iªYy~“΅½.N¥z ΙΩπξΖξ—?³fΉ0y!ΰ{ÿ›Τ8±Ο^lΏ²=ΦaζVδm Ά―Πλώ.έΡό–	Ζ("lϊΪ¦.4¦$ΆO½Iιl	xvT¬z»i΄pΟ¶I±P­έ8bC]·ρΰγϋH6ΰ5N$…xe­(Ϋ†DÿΉk\βFT΄vα‡¨fόJΓQΥΤU([wBσcΙwΊ²κμ9±ξπ~(b3€ΐ4 Yƒ;TB(Φ™2oµ©‘°
εsµAγQL¤gΟΙ™­οaZέ?ςHG|9mEFqA2qxϋ%dNΑξr΄ε~{VCp] p^¬ ώώ_†­b¦cύ\&Σς΅8	υSιΰοΨd{;Ψ'	r‹μ^*LΒFD<gΦΠα¬#Άÿh½β„y‹γαBΗ®ζÿϊ’Sψ)KU!Ύίύy
ΤΒc2r¶ϋλ±΄l¬εηxΚδ
¨MHψªenMΪ² ±{Ήζ{™ΰ#α\θΛ±ΚπU_;††ΦΦB™κΊz0sζUb.Δ—;0ψ`l―=~4cΛA5ΚΛqί‰@ª½Aε“j|΄‘GD-με«N»hψs<sC¬βHΤ\X(CSKC΅½ΙJφνoΥόp	*1uE]+ί7(77§l4›D	ΈoΠE"qγΌ(η΄)υ¥¶}«C3@4E»ήΙ73Άοϋ‰wQ_ΣfϊF€(^T¦>―ΤΝuζ²tΕ=]…ιφ€π_1ώφ]ρά—"Τΰs‘†®hό‰{~Λζthί£„cJώ<6–΄A·k—Ί~³Z²χ‡¤α®Ά8΄Ώ° )L®Ίk«ΞΤ(ΛΌV|¬κ/φΥ‡$bπNΛ‘JφΎΑΚΤNΪρ΅'|OPΖ„S LξΔήάσMWς49Η…Ηª„™}²µ9y"<G%·9mΔ€°u
e”&Ϋn_›ΡΘ΅R]h(ίªΡ»ΩuΚyΩθϋfκ!j±_NΎ‹“‘ϋ³¶uΈ{P[d¦=7P„UδεΩj½ζΠ4	λϊF5±q	`Τ®8Ψqv«­ϋJΒ})TjΗs_κ•jsΔ@U;»οµΌξΠJd.€GΏN½?τBUtΑω:XΖpY0®eV° ΒsΗDOjy£Eλ%;#ΚΌΆν‰ ο6‡γfI?0LΛ‚¥‘ccVdΠIµΏτΔς]{Bj^†oU&ΰV+UΒO8YΑτoªύ•Έ“­~‰πΐF^;α7;"(χU…ϊΘ†ªΊ¶€)oΝΞ/‹ΊνSlΞ"eP^–1>v«VΟ‡ώΉ=Ψd³$„5Ε3iγ
b©Jχsπ»™pÒ-‡@τ²κM8	¬tΥ‰OÿGΣD!ϋΔΕp¤Άq§xGΌΤϊixSΈγΊ
¬μ•B;tƒt¥σ’Kgΰ„³Ί›Θ‘iΘηαq?εG†υ[|ΈÒΧ £Ίφε #¤|ζΌθµ0ΧΪ•®?|UΏQά~ μ 1Δ6­+uλ·™¨ω'”X‹Φ %e‰ήFζΟ§c||α>ήIULe"vO“tHιΥΦ»Υ ΰMΩqc‘/Δψ+όΓτΝ#.ΎΞh¬fEzΧBN»®VΧ€½F³[€<€UnauΟpdδσώDξV¦ρP&΄ÿ¶ΧΘ%LP¬DTIμtnσ[>Ν-]9ÿªNιyH-HωlΊλςYNTζ€p3&κÿLΘtI^(Cυ7ς…ΌΚΜMqPπ¥Zο£Y-ΘΞaSϊΑƒyΨ™F•µ΄Qϋ>!η.ZP'!Y'Zΰβώ'‚Β²3+£•“ T[}ΉIªlΌE—W+—ΛEΌu-θηiÿ^ª+gIπ-ΈwFίXL(‹β—ρΕΒ"Ί€υΘόί“Ό¤g³7δΎ|¥>Yo2Ιξς1-•Χ…9w4®ΏΫ­$‚”ΊΫ<Ψ:xXχs*ΌμJQα}ΜΚ½\'Α>e­2ωΐ?ΎιpΎ»#@δL	o`ΗίΜ—†68Γwqm€®£γ™φλV®ψ[l¶fYΓMK£RsκΥ^ζ•ΖΔhΧ31ά3ΝiΐοΥΘτkΫ’εΈQP;DrύY•n%Ά#©V—ΒX=DύkΌpέC•>dH(0PMÿ=v|―ΌΚ}α‹4ώόdkM§®u>?“ϊΉκ1O!θ!:;_ή7πB© <KβΨ«μwq$ΡΙΦ¬η+Ρ¨<ΆΖ“Ι;h©L–βντυ]HWσΞΫh[§Έ^Γώι–υW~1_•ΡΣ3Ze`ρ7Ψ% }‹Gª@H8bHσ!ώ
ζΣTν>eΓ_πΩΆ¨W~'Π/.΄Q ς±>Y·GqόTοe­λ£©XΌΥB[ 3%\ΫΐΔίCϋ ¬cI<Ζ\ΤxΥ	bίΨUΏΈγ…jQΜφζ3C χϋ―.•5f…τ9n¶46Gk―ΑM$‰§ΈZ\X`ύƒΙθ`ΤΚ&1ο;€®"―αΨÿπ†Ο_π.cπΛγw¬`³ΫΕχΔΰnτΖλ\λV'”"tΡζύvΊ0μ€™ΪΊ¥µΊ΅ΒyE]λ¨κXϊΥ®±½HΌ mί£ π“φz[QΑΡ>6?±υ`§ΡT™Ρk½ΧΊr«•£έnψνςΐι€Άζ¶„”ύyυP¤ΈÒI—¨qΊZ¬/‡§κiξ6"=²)›¶κ_τ3F½cΨt=$³JΞ4{
γVθT,&¶“υr¨Θ…%ν’‚!pΆυx+ΕL¶‹sοU‰ψΛΞ’tπ‹¬x!Ζχθ³Ò|OKΜ{΅^ς!uµΐ)««?>™γA¶.©γ`Θ‘I¤ υ!ZeΦ.³xxX§‰WΛNC¨~ζe5Φύ"$4[J¥QD£ΆIΎΝ«BΤΦξ3^‘S‘7)£#Ò]sdπΩθkecΩnJΏΑy£¥Π™Hξƒz&Ò«”ς½W0Òd“L™ÿΏ]
laΒΑh„n4PφD‰m5―ψ$ΐª-ΈC£ξΖ›
d Αε¨«Έν	Òτ²‚(ίσ—αγ,οHgb2Ώ7ϊN¥ϋ4ªΞ¤ <©%ύ)+—vY	—ι[F'ΫMΒ”ijΰl³tS"γ»Ή:ΰ‹ΈPb5]―5|ZªLTNs¨ζΨ=r,<ΚΝ[εn•yΊzΟΟX†³;tφϊ‘?WwB΄#Π‘τΎc9Utάσ£?¤ΤwΈ1‡Π€OδκζΉxΟΧDwΉ2 Ζ!¤H†μ_eθΙ¨ΤϋT,GέΛσΰw}»’£Ò­θUΩΤ,¤[ƒΗ•¥]o6HγΪi9ΐ» ―ζ y€9tΫ`Ύx²":¤Ο‘\J:ΪG=‘μ›|ÿyR΄@ά|gT›θΏ%	θΆ›tΤK_ΆeBΜxΕβΜA°’Έ΄Ω›b6Ήϋ’#GμmV?|NΝ”lΊΨΘCJϊαάς45NΤ'Fq\ªZω¤ίιΜ|Ά°U£Β‹ !Ν£Τ…uaQΈ'yFN³΄•Ϋ;XVL:”ΛUίU‚«Yα›L"uu=`Sΰ_²z‚[©H®«ÿ0ίδ–οΚ( 7Θ8ΠÒ{Ζψ9ά§ν{ΙρXZΛ@β0Δϊ”ήΛ6LόέΒa>ά«Ώµ_ΥκNkSψI λ—•)sΖq*n„ωfγÒΓJΝh4>γβfσρΕF{ΕτJθ%ω]^cΉ@-0Ώ!ÿ>’}@Ιϊ‹ΕΤΔ…ησ'y`εj“0ΝΆXή°S¨ρ%²ώΥ6"ΫΙvΉWΰ'g‡bΏΈη^ΘΊ!ΚBΜ€τ	}ηΛζ!¤ύYτύ»‡ςƒ6ξ“η~9Ά#rΜz‘μΓB¦§τ―u™Η#­ξn%Ϋα―2Lμ²σh,Ρ™S«Ύ¤
²e”oC,	°^΅H(ΐTKµuUΒk%›τΖΌ²#|―”YBΧ7pdtEIβrFΦΏSΣ3.…Γύvq!ΠΨΆ“ά‚tzΛΙΙF²Bª¶®@|?©OΓ©hψq»Θάs)…a–µG]x6W›[–‡›aΘ‘]“3Κ-²Θσ#ws@―qτ®³έNµƒLf’ ZΥτ5!BΜ—›”iHΈν75΄6Ηή‘G€Y$VΧ¨ΞOEω°―"hΉΔ>Ϊ)+‘¥ΛC–Ί|=ΠΥΣEqΊEΦ"¬,9¤ο΅
Zt”Òaoχ‚sm[™NfT5χόFτMΥ[‰Γ”ik®ÿ)cE;}Ν½Ό³σιKβ[3ÒH8Ιa@@w!ΨqϊE_έ«¥ζV]δT¥Π`λP†6Kπόsφ{^XΆ ,EεΠΞ―aP…qgsάΩ%1zF,¬Η%5q<χωθλ΅)Ί
?¬–ªεΚ`"4ªFΣθ.όηgΥ66	οώ9\Ϊ‹Ϋ=®ϋ¶ΨmIέε
ΘΉω?©Κ¥φVWp•Sÿ¦C.ƒλÒζQ!N°1‹Ά"Yαo_b[Δ?π#ΜpB)Κ~¦®|\ΰpΌ!‰cΦ9τ®ΰIρgx«Ιίθ8fwξKX¤™LG_]F€ÿ3*oz\ο&$a¶"k2~«¶-ήΐJΕNMµG#j!s	ϋd‰?ZO	π΅ώφmf5ύ²n‹?Qi-κEΆ}9l–QYς¦yΠ®―jIτb«;*!’‚ÿEΧ’…ΰFΜyÒcGocβ‰$ QF;ξΠZrΌ›ra΅εύ_ΘCEΪhΖ
πk[0χ€H2#ο~ω6½ΎlΕγχΫ»uγ¥«V¦€b`F5¦gίΎ."TΜκ½°ΛRύtΪQ9_vαυm}ρτÿlβΊΪ‹Φ3cΟ=v‘#‡Ζ`8ΎόtzKΑrηz¦ΟΚΏβ«μ0ΐηΣΑτΏP#ςΤ82ω8w—ÿ„½lρBMωφPίξ…`	Zϋί·Μ8ªΙD€ΥLXcύ€ξ‰ϋ–1ΥUλ.`)¤ΛψΩής΅\hF<YαΪεύΡΤo…dΒφΖSώΊEtF`vw:βµ“”CVO„‘ψ`b³]·{ά.Ql,g―ηΗS”P ­8―*ωcΫΰJ‰ρΦzϊΉ,ύσ®ΞϋƒV'IΆ£νδ+δσ±"½+]xΊΓΤγΚΌd΅'ϋςkN1λ®ΞςΤg.¶Β΄•΄δ ώyΕρ Mr;’ϋt82RÒΆιM1„ Θ΄q\ΐ·G¦H‡ %Η·m,C><te3¦µΔτσoΞ΅}3τM	f¬o σuυnπΆF¶Τ¤Κ„ÿ±θψηώrΨ
~g¤¨άξA[»Δlβ?ne‹σ(^;5.7Z/1«©1hm>bluGΪT¤#·΄Ϊ>D~h>Y±βp΄„Ν‘ξ¨DqλΠnσ'jυΓoω<“ΰVΞΛ%†i)¥‚[>W]|<-Μ]"Mγ¥ΞωΕ―σeFg¦Π@©|_΅LοG|N%ΈΑ8H3Ζ~Φn‹_©χ)<ΘΙXjΈƒ~wj^I Πm‰›:I+@ε‹=7gΖτ“ΫηwP3H¤ήΖ–υv~φ„LθT
ΑΛZ	/ΰ›&ΆΘςξΪ±¦dΖ`‰Ήζΰ6Χ‰;ϋ›π…ε½$Ύδcl(³Jϊΐi©9θ«ƒ΅P/ίe5θ’ΛΥ]ΪΖ#2ΘΒ΅Οα(^Rw^bΒK$|==±ρΨDµ—ΤηύΦP±*“>fΫr«"³Q[>ΤM’Ί±ΌπΪέSÒμGθ~ύΦ‰υ(Ψ#%8Έλ©ΐΖ^£“t0*©;%―0ίM¶‹‘J$8Ηϊx@)ΐ+3O?~1―dFίΊÿ_ξ ‡ώ"RL¨5κ2x#¤ΰ†nό€w9Φό`Η(
+ίl‡u*Φt@ψΙρΪ%"ΨΐλnοtΒΫ‡¤—μECϋ'oRΉw Uδ±–Z85sUBπ°ϋΝ9³²Κ»ν	,ή1C{ΩΒΛΑχΗΦφΩn’τ’τAgzέΜ•aDb)ΗΦP1” 2γ®‚Ο=ΌΡΫν”ΰή!‡zb¶UAη–Y–	MkXI(LαN~ϋ²&„±p1v¤Jxΐθo²άnc‘Όή2^ϋ;Ή#ϋsJ~Ϊc5Νΰυpkζ΅“£§Zη	zρΌc?jάΉ΅Zξας	¦*$ψ(»όΩΒU5ΕθωqρoΞ°ι]Χ>ϋ-?r¥µ:ϋί ΜaA%“Zyέ¤6#΅Σ@o•KrF1cΏ1Q¥£²*bϊΟ Ε»t.°™βu^―,ψβΑΧ5ª0ΣΑT;ΊήG=°rάμ•:Ζ…~ή΅·„L΅ωθU…ιΨ«+λ›‘wψ<&¨Y±΅Y"οΩΧÿPΪ)Υ*‰ΗGγΣ› aZaτ}+Dιu«!τ°FΈ®p$L\rmσΖςO!΅†Έƒξ>ς<v]Έΰ­XκVΙφ_ Μ·ΡαγΠσZΆ¶ΜJÒ&5PoςAD_²gs΅2M¦ψ©PΌ>£Πd)ΣΈ%ΡρΟW<‰e 	!n)γαψΤ¬vuπ#9ΤHΥT€Vhr	›ΐ.*υXήpnΞ-F·ΎΘεtήu¨uνuΦ3‘v°Ε»ΩdRφ:-?	ΊήαζΕ›R]$€έΚ+:‚ªΔo1— WDφ
c-΅C.|yΣ7―‰"ΘϋκÿΠG§yύΑη<_γ‘QήhΠμ“fΔ16ύ†™epΰϊ.ήτΞ=φΓΨδ© m €FκΐΒάJ©0¶kLΣZPf:)9ώ–#¶¨=q [ΦΌσµ3°Ρ‰DXzJ¨–Έ¨·„‹\μεcά)¬…~xΉ§—κ ¥+[yÒeΰΊζ5ΦρxΓ<nsKΛφ$N–Κ
<[S=Ζ€Qή“R—ΐD­YlπX±γ#>ΧΞάUdEKόη―τ»J‡―V¤½ΎEΕα&›>’H6Mh]‡θs	ηΗίυΧ(–Δl7Τβ%‹β4M†eΦ«—$D&®a‹
Ψ^=ΞK	FoοY]K™§ΡθNΙ:-¤δΉ`n£ΔwZ¨+‡όΩΒ±ΡσώΒΣήyήωΤξ'³C“Ή.ρªΩ=β¦#φ}gJΆ
^[gάΔv$wΏΫ‰ΕOvl}θ?”Γ²b°#'ϊtblµCσH½Κª§J6Ιa[µ¨—s"–χφήJ?Η+½	r Gwϊ‡‡¤\Ο€2£
²ύ?ω¬eψjW‘†uΚΑ{<‘Ί"‹ξ|~°O+¦
ύφι7Υ²!c&€Nβηπ Ir¤Ò‡}Φ$ΉA±ΌVD8}:£ΠT?1T¨9π‡|ξ'i`pρώ~υΰ΄@	πΑu4=<\΄ΧπώOόx'xeCµΏΆX£,&wF—αέ›<¥Ϋ0Kγλ„Ύά†‚<I$¶Ω|ΚΞjΨ„―A–[[ηHoΚj!θόIθ
ΙΆϊςφλόΆ§•hέ`ώ>^1CΦιVΣhΘa‚ϋ*$-{γ€±†ΥΓyi"φ<ή™Τ%D`“χƒW^CΡ·;ε‹Y< —LΘΗ¨γ©²ΨάrCJυΐOQ~c±JƒΝ‰¦ÿςZ[~
IατγΛΐc–QXg¤ΰσw1½θ΄tύΡ―xomAsΣ€RηΕΎ’QΖ¦·tζr°Ξ³f ±ύr|…ΪmFφΟOτΫιθρ”‚>³"-y\mΪv‘f7IνEe>…«=gD/"§nKSy±jγ(―o|tϊFΉm›4Έ²{Ω2ψ–­¥NΥ7Κ“όΎ6­ ¤,ΗFό)ΔB6NΟίωΖ\kxφύ·@Φ'{,YΘQη³’¤0χΡϊΠjΣ5§mΔΆσ μC‹MΑBp­;κΌ—ϋΚάl`2;eCwκχΉ΅¬ΕΫ‡¨ςW›ό­‹.έΠύϊΌ΄ΕFh;Ν%ΚΦς;,Ψ£Q—_§"¶§η3C΄Ά‹“έQ―¬Ύ°QτL7ΊΥm©7·zΐ΄|WβκΗΞhY»Οª)Ά (θMh8
gB@;],_G@:+IάÿNο%τδnΘXTnÒ6ΛΩ9Τ(:λ@ΕO·vΙ;K27α‚σΤlw‘ΕΒ]έ1Ί²ηΡ} wέσΤ@Φ±WGW%aN›p γl"πRμ7VΉ{2;Π;8p…Όω©h\*„m”­ ΩΘθl£ ΛqΝΝΉWαhhrέΜξ&NcεWόqί·ϋΒ³ωg$…%½ΥΫ*§(ηOρ{c¦–	»ZΟλγ‡6RψςKƒ@VlC.τ$wΥίφ®Ε@λ%DΑ¥” *¦ΗΎι·΄ΥCλ ΟdwΑΫΉÿv Α§“¦KΎƒδ2=,6χ0%σTG‡ξFθU[r-\@"…ΟcΞιQ ύ+'Ο`Νiο'B γ_Κ™ΒUM;ΉΒιJD­«_°ό¤ RχΩ$…*A΅ρKU¬gXzςX°θ»’-r:½'Ά	Εk’ª?ΦΒιΧjβόόtN.w*ίR¬²5?ΟÒ	ZΟ!¦Ε=…O7†€$ν¦V<V8τ4’B‰όΡtδRήx¤/ΐm’\r=ά9r,²ΥΕAΗlM}§ξE§H$τ™ve&υΤKΟ‚λ_9πχc‡ώ«5γ‡%Θ[«fΙ©,`suPΝ¥Σ€TVB†½ΗmWK½ΆυSΌΆoφ«λ³Hα·jρ¥a?θΘΔΕv©Θcƒ³K‘f#/A †ΐ:C_ΪΏΩI€9wy!£Θ(νS0δΕR=φηξω‡VΛάBZ§ΔA…φ1~37σÿΎ9θ9ε‡ΐm€“vgw>›$λΧ&Ò4όΝ3 ηIa:(ΰ Φ1:‡Rx<ς;µ‰;ELF                      ¬2     4     (            GNU  ΐ       ΐ          θόÿÿÿWV‰Ζ‰Π‰ΚS‹ά   ‹|$‹™  ƒγύƒψ‹ά   ΐƒΰώ  	Ψ‰  ‹†ά      ‹ 1Ϋ€δ_…Ò•ΓΑγ	Γ‹†ά   ‰   ‰Ψ‰ΪΑψƒΰ…ÿEψ‹†ά   €Ξ ‰   ΈΗ  θόÿÿÿ‹†ά   ‰   ‰ψ[^_ΓfθόÿÿÿS‰ΓΓΌ  ƒμHd΅    ‰D$D‰ΰθόÿÿÿ‹D$,=9‰ ‡L  =7‰ ‡   =Υ ‡6  =Σ ‡   =Q… ‡   =O… ‡   =ν„ ‡
  =λ„ ‡   =Wu „   ‡ξ  =9K „   ‡έ  =ΕΨ „   ‡Μ  =»Ψ „   ‡»  =ΩN „   ‡ª  =L „   ‡™  =o> „   ‡  =e> „   ‡w  =“8 „   ‡f  =77 „   ‡U  =Σ6 „   ‡D  =Ι6 „   ‡3  =6 „   ‡"  =ύ4 „   ‡  =™4 „   ‡   ='3 „   =Γ2 ‡κ   =¤2 ‡?   =»0 „   ‡Ξ   =ϋ, „   ‡½   =ρ, „   ‡¬   =Σ, „   ‡›   =Ι, „   ‡   =9+ „   w}=? „   wp=+ „   = w^= ‡W   = „   wF= „   w9=Ώ „   w,=Ό „   w=a „   w=V  ‡όÿÿÿ=πU  wPSh    θόÿÿÿƒΔι   ιo   ΄&    v θόÿÿÿQÿUWVSƒϊ†Σ   ‹8‰Ξ…Ι…ν   ‹Έ  ƒωwƒωws[‰π^_]Γƒω„Ν   ƒω,„®   wεƒω*uΈϋÿÿÿ½   ‹—ά   Β  ‹
!Θ‰»Θ   ¶    ΈX‰A θόÿÿÿƒλuρ‹‡ά     ‹€  	Ε‰*[‰π^_]ΓΊ   ΣβχΒ`2 u€ζu@ƒω…pÿÿÿΈύÿÿÿ½   λv ƒιƒαχ„uÿÿÿιPÿÿÿ΄&    1φ[‰π^_]Γ΄&    ‹hƒύtƒύ…(ÿÿÿιCÿÿÿ‹P‰ψθόÿÿÿ‰π[^_]ΓΈώÿÿÿι2ÿÿÿ΄&    θόÿÿÿS‰Γ‹€Έ  ƒψ„	  wwƒψ	„#  †   ƒψ„'  †Q  ƒψt†  ƒψ„½  Ή   ‰ΨΊ `  θόÿÿÿΊ `  ‰ΨθόÿÿÿΈd   θόÿÿÿ‰ΨΊ @  θόÿÿÿΈd   [ιόÿÿÿ΄&    ƒψ)‡  ƒψ'‡  ƒψ!„Γ  †G  ƒψ&„r  ƒψ'…ι  ‹ƒά   (  ‹ƒβώ‰‹ƒά     ‹Κ   ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹Κ  ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹βώÿώÿ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹Κ  ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹βόÿόÿ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹Κ  ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹Κ  ‰Ή   Ίψ*  Έ'  θόÿÿÿ‹ƒά     ‹βύÿύÿ‰‹ƒά     ‹Κ  ‰‹ƒά     ‹βϋÿϋÿ‰‹ƒά     ‹€    ‰Έ<   [ιόÿÿÿ΄&    fƒψvPƒψ„ξ   Ή   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈd   θόÿÿÿΊ   ‰ΨθόÿÿÿΈd   [ιόÿÿÿƒψ$…[  Ή   ‰ΨΊ`  θόÿÿÿ‰ΨΊ`  θόÿÿÿΊ   ‰ΨθόÿÿÿΈ   θόÿÿÿ‰ΨΊ   [ιόÿÿÿv ƒψ„/  v]ƒψuT‹ƒά     ‹Κ   ‰Έ   θόÿÿÿ‹ƒά     ‹ƒβϋ‰Έ   θόÿÿÿ‹ƒά     ‹€    ‰[Γfƒψt£ƒψuς‹ƒά     ‹€    ‰[Γv ƒψ„  †  ƒψ†hÿÿÿƒψu·‹ƒά   Ί6   ‰(  Ί   ‹ƒά   ‰$  ‹ƒά      ‹ƒΚ‰ΈΘ   θόÿÿÿ‹ƒά      ‹€ζχ‰ΈΘ   θόÿÿÿ‹ƒά      ‹€   €Μ‰ΈΘ   [ιόÿÿÿ΄&    ƒψ„·  †Ι   ƒψt	ƒψ…ÿÿÿ‹ƒά     ‹Κ   ‰Έ   θόÿÿÿ‹ƒά     ‹ƒβϊ‰Έ   θόÿÿÿ‹ƒά     ‹€    ‰[Γƒψ
…Έώÿÿ‹ƒά     ‹Κ   ‰Έ   θόÿÿÿ‹ƒά     ‹ƒβπ‰Έ   θόÿÿÿ‹ƒά     ‹€    ‰[Γ¶    ƒψ„aώÿÿƒψ„nÿÿÿƒψ…AώÿÿΉ   ‰ΨΊ ΐ  θόÿÿÿΊ ΐ  ‰ΨθόÿÿÿΈd   θόÿÿÿΊ ΐ  ‰ΨθόÿÿÿΈd   θόÿÿÿΉ   ‰ΨΊ   θόÿÿÿΊ   ‰ΨθόÿÿÿΈ   θόÿÿÿΊ   ‰ΨθόÿÿÿΈ   θόÿÿÿ‰ΨΊ   θόÿÿÿΈ   [ιόÿÿÿt& ƒψ1„χ  †   ƒψ5w\ƒψ3wc‹ƒά   Ί7   ‰(  Ή   ‰ΨΊ  θόÿÿÿΊ  ‰ΨθόÿÿÿΈd   θόÿÿÿ‰ΨΊ  [ιόÿÿÿfƒψ%„ύÿÿι>ύÿÿfƒθ8ƒψ‡0ύÿÿΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈd   θόÿÿÿΊ   ι=όÿÿƒψ-„cÿÿÿ†ª   ƒψ1„ηόÿÿ‹ƒά     ‹Κ  ‰‹ƒά     ‹βϋÿώÿ‰Έd   θόÿÿÿ‹ƒά     ‹Κ  ‰‹ƒά     ‹βύÿώÿ‰Ί7   ‹ƒά   ‰(  Ί   ‹ƒά   ‰$  ‹ƒά   Ί Γ  ‰   1Ò‹ƒά   ‰  [Γƒψ*„ιϋÿÿƒψ,„|ύÿÿ[Γ¶    ‹ƒά     ‹Κ   ‰‹ƒά     ‹βωÿώÿ‰Έd   θόÿÿÿ‹ƒά     ‹ƒΚ‰Ί7   ‹ƒά   ‰(  Ί P  ‹ƒά   ‰$  Ί   ‹ƒά   ιXÿÿÿ΄&    Ή   ‰ΨΊ   θόÿÿÿΊ   ‰ΨθόÿÿÿΈd   θόÿÿÿ‰ΨΊ   [ιόÿÿÿt& ‹ƒά     ‹Κ   ‰‹ƒά     ‹ƒβϊ‰Έ   θόÿÿÿιbόÿÿfΉ   Ί   ‰ΨθόÿÿÿΊ   ‰ΨθόÿÿÿΈd   θόÿÿÿΊ   ιIϊÿÿ΄&    ‹ƒά     ‹Κ   ‰‹ƒά     ‹βϋÿόÿ‰Έd   θόÿÿÿ‹ƒά     ‹Κ  ι-ώÿÿ΄&    t& θόÿÿÿUWVS‰Γƒμ0‹‹Έ  d΅    ‰D$,1ΐƒω„w  vEƒιƒωwΈ   Σΰ© γud¨…Χ  ƒωt71φ‹D$,d+    …±  ƒΔ0‰π[^_]Γt& ƒω„§   1φƒωvΠƒωuh    1φjθόÿÿÿXZλΉ΄&    fƒωu«‹5   …φtΊ   ‰Ψθόÿÿÿ‰ƒ¨  …ΐ„?  ‹ƒ”  …ΐ„wÿÿÿ‹Pd‹…Ò„jÿÿÿ‹Z…Ϋ„_ÿÿÿ‹    Ή   …Ò„ζ  ‹r…φ„Ϋ  Ί   θόÿÿÿ1φι2ÿÿÿf‹   …Ι„ ÿÿÿΊ   ‰Ψθόÿÿÿ‰ƒ¨  …ΐ„ΐ  ‹ƒ”  …ΐ„ψώÿÿ‹Pd‹…Ò„λώÿÿ‹Z…Ϋ„ΰώÿÿ‹    Ή    …Ò„x  ‹r…φ„m  Ί   θόÿÿÿ1φι³ώÿÿv ‰Ψθόÿÿÿ‰Ζ…ΐ…ώÿÿΊ   ‰Ψθόÿÿÿ‰ƒ¨  ‹ƒ”  …ΐ„}ώÿÿ‹Pd‹…Ò„pώÿÿ‹Z…Ϋ„eώÿÿ‹    Ή   …Ò„έ  ‹z…ÿ„Ò  Ί   θόÿÿÿι:ώÿÿ΄&    v ‰Ψθόÿÿÿ‰Ζ…ΐ…ώÿÿΊ   ‰Ψθόÿÿÿ‰ƒ¨  ‰Α‹ƒ”  …ΐtA‹Pd‹…Òt8‹z…ÿt1‹    Ή    …Ò„@  ‹j…ν„5  Ί   θόÿÿÿ‹‹¨  v …Ι„¶ύÿÿ‹Ad‹@…ΐ„Ϊ   ‹x…ÿ„Ο   ΅   ‰β…ΐ„Τ   ‹h…ν„Ι   ‰Θθόÿÿÿ‹‹¨  ΖD$ ΖD$
 ΖD$…Ι„_ύÿÿ‹Ad‹@…ΐtw‹x…ÿtp΅   ‰β…ΐ„Έ   ‹h…ν„­   ‰Θθόÿÿÿ‹‹¨  ΖD$
…Ι„ύÿÿ‹Ad‹@…ΐ„ύÿÿ‹X…Ϋ„ύÿÿ΅   ‰β…ΐtN‹x…ÿtG‰Θθόÿÿÿιεόÿÿt& ΖD$
λ½΄&    fΖD$ ΖD$
 ΖD$ι[ÿÿÿ‰Θθόÿÿÿ‹‹¨  ι2ÿÿÿ‰ΘθόÿÿÿιόÿÿΊ   θόÿÿÿ‹‹¨  ιΙώÿÿ‰Θθόÿÿÿ‹‹¨  ιNÿÿÿΊ   θόÿÿÿιhόÿÿΊ   1φθόÿÿÿιWόÿÿΊ   θόÿÿÿιFόÿÿΎνÿÿÿι>όÿÿθόÿÿÿ΄&    fθόÿÿÿ‹Έ  ƒω3wpS‰ΓƒωvƒιΈ   Σΰ© γu¨u3[Γƒωt
ƒωt&ƒωuξ‰ΨΊ   θόÿÿÿΗƒ¨      [Γ΄&    f‰ΨΊ   θόÿÿÿ‰ΨθόÿÿÿΗƒ¨      [ΓΓ΄&    ΄&    θόÿÿÿ‹Έ  ƒω3wƒωvƒιΊ   ΣβχΒ γuƒβu,Γv ƒωt
ƒωtƒωuν‹¨  …ÒtγΊ   ιόÿÿÿt& ‹¨  …ΙtΚΊ   ιόÿÿÿt& θόÿÿÿWVS‰Γƒμ‹»`  d΅    ‰D$1ΐ…ÿ„9  ‹ƒΈ  ‰Βƒψ	t‡ζ  ƒψ„¬  Hϋƒω‡1  ‰Βƒψ&†  ƒψ2„µ  †η  ƒψ5„f  †Ψ  ƒψ;„  †¶  HΔƒω‡\  ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ƒψ"‡½  JϊΈ   Σΰ©ΕXί„³  j “
  Ή9   CjDθόÿÿÿ‰ƒ”  ZY…ΐ„Τ  “π   ‰   ‹ƒ”  Η€      ‹ƒ”  …ΐ„ª  ‹Pd‹…Ò„  ‹R…Ò„’  ‹    …Ι„|  ‹I…Ι„q  θόÿÿÿ‹“Έ  ιo  ƒψt3†Ϋ   Hφƒω‡X  Ηƒ     Ηƒ      Ηƒ$     v ΗƒΨ     Ηƒά     Ηƒΰ     ƒϊ"‡¶  ƒϊ‡πώÿÿBÿƒψ†ωώÿÿƒϊ„"  ƒϊ„  ‹D$d+    …3  ƒΔ[^_ΓfHΚƒω‡%ώÿÿΎP   ƒτ  Ή   Ί    f‰³φ  θόÿÿÿ‹»`  …ÿ„
  ‹ƒΈ  ιιύÿÿƒψ…€  ΗƒΨ    ‹ƒά   Ί   Ηƒά     Ηƒΰ     Ηƒδ      Ηƒθ  ΜG ‰„ Ηƒ     ‹“Έ  Ηƒ      Ηƒ$     ιϋώÿÿt& θόÿÿÿv ‹“Έ  ƒϊ6…Α   j “ό   CΉA   jAθόÿÿÿ‹“Έ  _Xƒϊ…ήώÿÿ‹D$d+    …  ƒΔ‰Ψ[^_ιόÿÿÿHΘƒω†Jύÿÿι΅  fƒψ3…§   ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     Bάƒψ‡MÿÿÿΉωώ}£Α‚>ύÿÿƒϊ6„?ÿÿÿƒϊ7…Uÿÿÿj »ό   sΉA   j@‰ϊ‰πθόÿÿÿ‰ϊΉA   ‰πj jAθόÿÿÿ‹“Έ  ƒΔιÿÿÿt& ƒψ4…ζ  ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ιKÿÿÿ¶    ƒψ)wKƒψ'…  ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ    Ηƒ      Ηƒ$     ιNόÿÿƒψ-„°ώÿÿv,ƒψ2„;  ΗƒΨ     Ηƒά     Ηƒΰ     ιψϋÿÿƒψ,„·όÿÿι
  v ƒψ‡Χ   ƒψ„φ  †€όÿÿƒψ„fύÿÿ†	  ƒψ…Χ   ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ι­όÿÿ΄&    ΎP   Ί    ƒl  Ή   f‰³n  θόÿÿÿ‹ƒΈ  ‰Βƒψ†®   Hάƒω‡³ϊÿÿΎ   Σζ‰ρχΖ… σ…H  α   …mόÿÿƒψ&‡’ϊÿÿHβΎ   Σζ‰ρφΑ¨…Χώÿÿƒα…­ϊÿÿƒψ&„΅  Ηƒ     ‰ΒΗƒ      Ηƒ$     ιΒϋÿÿt& ƒψ„rϊÿÿvHκƒω†…ώÿÿλΎ΄&    …vώÿÿλ―…ΐt«‰ΑΎ   Σζ‰ρχΖ†<……   α`  u]ƒψ……ώÿÿ‹‹`  …Ι…uÿÿÿ€=    „tKΊΰ   ‰ΨθΡηÿÿ‹ƒΈ  ‰Βιµωÿÿt& ƒϊ…mόÿÿ‹5   …φ„Oϋÿÿι1ϊÿÿt& ‹“`  …Ò…ωÿÿΊ    ‰Ψθ†ηÿÿι|ϋÿÿ‹‹`  …Ιt™ιώÿÿΗƒΨ     Ηƒά     Ηƒΰ     ισϊÿÿt& ΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ι”ωÿÿ΄&    ΗƒΨ     ‹ƒά   Ηƒά     Ηƒΰ       Ηƒ     Ηƒ      Ηƒ$     ‹Κ  ‰Ή   Ί'  Έθ  θόÿÿÿ‹ƒά     ‹ƒβύ‰Ή   Ί'  Έθ  1φθόÿÿÿv ‹ƒά     ‹ƒβψ‰Ή   Ί'  Έθ  θόÿÿÿ‹ƒά     ‹:‰π‰ρΑψχΡ¶€    ƒαΣψ‰Αƒα‰ΘƒΘχΠ!ψ	ΘƒΘ‰Ή   Ί'  Έθ  ƒΖθόÿÿÿƒώHu†‹ƒά     ‹ƒΚ‰‹“Έ  ιOωÿÿ΄&    fΗƒΨ     Ηƒά     Ηƒΰ     Ηƒ     Ηƒ      Ηƒ$     ι5ωÿÿ΄&    ΗƒΨ     Ηƒά     Ηƒΰ     ιωÿÿƒ»`   „{ύÿÿιDχÿÿ‰Ψ‰\$ΗD$H   ΗD$    θόÿÿÿƒό   Ί    θόÿÿÿ΅F  …ΐ„6  ι1  θόÿÿÿθόÿÿÿ‹P(Η   ‹‚μ  ―‚π  ‹’θ  ―B‹T$Αθ‰1ΐΓ΄&    ΄&    θόÿÿÿUWVS‰Γƒμ‹ ‹“  ‹p(‹ƒ  ƒΐΎΈ  ‰B‹ƒ  Η   p‹ƒ  ‹“  ƒΐ‰B‹ƒ  Η@    ‰ψθόÿÿÿ‹®ψ  ψ  ‰Βƒx  9ιtS‹«  M    ‹®ό  ‰†ό  ‰‹x  ‰«|  ‰E ‹E‹‹  ƒ=x  ‰H‡ψ  ƒΔ‰ψ[^_]ιόÿÿÿt& ‹®ό  ‰†ό  ‰‹x  ‰«|  ‰E ƒ=x  ‡Φ  ƒΔ‰ψ[^_]ιόÿÿÿfθόÿÿÿUWVS‹h(‹…ά   @  ‹ƒβξ‰½Έ  ψ  ‰ψθόÿÿÿ‰Ζ‹…ψ  9Γt@v ‹…ψ  ‹P‹-x  ‰Q‰
Ί   Η€x     Η€|  "  θόÿÿÿ‹…ψ  9ΨuΓ[‰ς‰ψ^_]ιόÿÿÿ¶    θόÿÿÿVSθόÿÿÿ‹°(  ‹FƒΖ9Ζt$X΄‹Cd‹ …ΐt‹…Òt‰Ψθόÿÿÿ‹CLX΄9Ζuΰ1ΐ[^Γ΄&    t& θόÿÿÿWV‰ΞSθόÿÿÿ‹Έ(  ‹‡Έ  ƒψ4tƒθ8ƒψwPΗF   ‹‡ά  ƒΗ‰F‹X΄9Ηt(t& ‹Cd‹@…ΐt‹H…Ιt	‰ς‰Ψθόÿÿÿ‹CLX΄9Ηuά1ΐ[^_Γ¶    ƒΏ€  u§Έκÿÿÿλζ΄&    θόÿÿÿWV‰ΞSθόÿÿÿ‹Έ(  ‹‡Έ  ƒψ4tƒθ8ƒψwH‹…ΐuK‹GƒΗX΄9Ηt)t& ‹Cd‹@…ΐt‹H…Ιt	‰ς‰Ψθόÿÿÿ‹CLX΄9Ηuά1ΐ[^_Γ¶    ƒΏ€  u―ΈκÿÿÿλζθόÿÿÿWV‰ΞSθόÿÿÿ‹Έ(  ‹‡Έ  ƒψ4tƒθ8ƒψwX‹…ΐu[FΉ   Ίª  ƒΗθόÿÿÿ‹X΄9Ηt(t& ‹Cd‹@…ΐt‹H…Ιt	‰ς‰Ψθόÿÿÿ‹CLX΄9Ηuά1ΐ[^_Γ¶    ƒΏ€  uΈκÿÿÿλζθόÿÿÿS‰Λθόÿÿÿƒ;‹(  uH‹Cƒθƒψw=ΗC    ΗC    ΗCΠ  ‹‚x  % ω  ƒψΐƒΰ`ΰ  ‰C1ΐ[Γ΄&    fΈκÿÿÿ[Γ΄&    fθόÿÿÿV‰ΞS‹\$θόÿÿÿ‹€(  ‹€x  %ÿÿ ƒώu(ƒψÒƒβΥƒΒ6ƒψΐ‰ƒΰΟƒΐ;‰C1ΐ[^Γ΄&    Έκÿÿÿλξ΄&    fθόÿÿÿVS‰Λθόÿÿÿ‹°(  ΅x  …ΐ…  ‹†x  ‹–|  ‰1ΐ‰S[^Γ΄&    fθόÿÿÿVS‰Λθόÿÿÿ‹(  ‹‚μ  ‰C‹π  ‰K‹²τ  ‰s‹²θ  ‹6‰s‹’θ  ―BΗC   Αθ―Θ‰C1ΐ‰K[^Γt& θόÿÿÿ‹…ΐu΅ΤI  ‰A,1ΐΓ΄&    v ΈκÿÿÿΓ΄&    v θόÿÿÿ‰Β‹ ‹@(ιόÿÿÿ΄&    ΄&    θόÿÿÿU1ÒWVS‰Γƒμ‹ ‹p(‰Ψθόÿÿÿ‹k‰Η‹†θ  ‹@―†μ  Αθ‰ƒ€  ‰Β…ν…Ό   ―–π  …Ò…  ‹–τ  ƒϊ„ΐ   wTƒϊ„+  ƒϊ…  ‹®Μ   ÿ¶π  “„  j P‰θj jÿ‹θόÿÿÿƒΔƒ=x  ‡6  1ΐƒΔ[^_]Γ¶    ƒϊ„  ƒϊ…J  ‹π  ‹®Μ   “„  ΡιQ―Θj P‰θj Q‹θόÿÿÿƒΔλ¥΄&    v ―–π  9ST‚  ‰SP‹–τ  ƒϊ…@ÿÿÿ‹x  χ†x   ω  tiΈ    ‹‰  …Ι„΄   …Ò…p  Η$    ‰Α‹–Μ   «„  ‰T$‹–π  ΡκR‰κQQPÿt$‹‹D$θόÿÿÿƒΔι	ÿÿÿ¶    ‹®Μ   ÿ¶π  “„  j Pjÿj ‹‰θθόÿÿÿƒΔιΩώÿÿ¶    ‹π  ‹®Μ   “„  ΡιQ―Θj PQλΙ΄&    v ƒΔΈκÿÿÿ[^_]Γv …Ò…’  ‰$‰Α1ΐιIÿÿÿΈκÿÿÿιώÿÿθόÿÿÿVS‰Λθόÿÿÿ‹5ΤI  ‹€(  9s…­   ‹€x  ‹S% ω  ƒψΐƒΰ`ΰ  …Òt^ƒϊwyƒϊ…   Ί   j sΉΠ  j PC‰SΊ0   j P‰πjθόÿÿÿ‹C―ΨI  ΗC   ƒΔΑθ‰C―C‰C1ΐ[^Γ‰ΑΊ   Ρι9Kw§Ί   ‰Θλ΄&    fJόƒωΉ   CΡλ…Έκÿÿÿ[^Γ΄&    Ρθ‰Α‰Θιjÿÿÿt& θόÿÿÿU‰ΝWV‰ΦS‰Γƒμpd΅    ‰D$l‰ΨθόÿÿÿΉ   ‹€(  |$ΗD$   ‰$1ΐƒ=x  σ«‡΄  ‰ι‰ς‰Ψθ·ώÿÿ‰D$‰Α…ΐ…U  ‹<$‹   …Ϋ…]  ‹—`  …Ò…O  ‹‡Δ  …ΐ…A  ΅ΤI  9EΈΤI  EΑƒ=x  ‰‡θ  ‹E‰‡μ  ‹U‰—π  ‹M‰τ  ‡Π  ‹E‹]0ΗD$$   ‹<$‹u‹M‹U‰D$(‰D$‹E(ƒΗ‰t$ f‰D$0f‰D$‹E,‰L$,f‰D$2f‰D$·Γf‰\$4‹‰T$‰\$ƒλL;|$tYt& ‹Cd‹@…ΐt‹p…φtL$1Ò‰Ψθόÿÿÿ‹CLX΄9ΗuΨ‹D$(‹T$‹t$ ‹L$,‰D$·D$0f‰D$·D$2f‰D$·D$4‰U‹T$‰E0‹$‰U·T$‰u‰U(·T$‰M‰U,‹€τ  ‰E‹D$ld+    u‹D$ƒΔp[^_]ΓΗD$πÿÿÿλέθόÿÿÿ΄&    t& θόÿÿÿUWVS‰ΛθόÿÿÿΊΠ  Ή   ‹°(  ‰ΨθόÿÿÿΊ    i†Έ    ‹Έ    ‰ψθόÿÿÿƒψÿ„ω  k‰Αƒψ tƒΑƒω ‡ο  ‰ϊ‰θθόÿÿÿ‹–Μ   ‹‚΄   …ΐu‹‚   PC0hΨ  PθόÿÿÿΗCT …‹†Έ  ƒΔƒψ4tƒθ8ƒψwΗCT …1ΐ[^_]Γt& ƒΎ€  uδ[1ΐ^_]ΓθόÿÿÿW‰ΧV‰ΖS‹‹@P‰V‰ƒτώÿÿθόÿÿÿ‰ƒύÿÿ‰“ ύÿÿƒ=x  ‡  ³ύÿÿ‹‹C‰B‰‰πΊ   Η   ΗC"  [^_ιόÿÿÿ΄&    fθόÿÿÿWVS‰Γ΅x  …ΐ…+  ‹ƒϊwrs{1ΐΗC    ƒηό‰ρΗF,    )ωƒΑ0Αισ«‹<•ΐ   Ί    ‰ψθόÿÿÿ‰Αƒψÿ„Q  ƒψ tƒΑƒω ‡G  ‰ϊ‰πθόÿÿÿΗC$   1ΐ[^_Γ΄&    Έκÿÿÿλν΄&    fθόÿÿÿ‰ΘιTÿÿÿt& θόÿÿÿS‰Λθόÿÿÿ‹(  kl  i‚Έ    ‹„,   Ή   ƒθƒψv‹p  ‰΅x  …ΐ…[  ‰Ψ[ιϋώÿÿ΄&    t& θόÿÿÿU‰ΝWV‰ΦS‰Γƒμ΅x  …ΐ…x  θόÿÿÿ‰Β…ΐt]‰ΗΉΝ   Cσ¥‰‚¬  ƒ  Η‚      ‰‚0  iƒΈ    Uÿ°    ‚ΐ  hΰ  j ‰T$Pθόÿÿÿ‹T$ƒΔ‰(  ƒΔ‰Π[^_]Γ΄&    ΄&    θόÿÿÿV‹x  S‹X(‹³ψ  …Ò…”  ‹ƒά   @  ‹ƒβξ‰‹“d  ‹Nÿv‰Ψθόÿÿÿ‹ƒά   Ί   ‰0  Ηƒ       ‰ΨΊ   θόÿÿÿ‹ƒά      ‹ƒΚ‰‹ƒά      ‹ƒΚ ‰‹ƒά   @  ‹€@  ƒΘ‰X1ΐ[^Γt& θόÿÿÿS‰Λθόÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…°  1ΐ[Γv θόÿÿÿ‰Π‰Κ‹9Θt	ιμόÿÿt& Γ΄&    θόÿÿÿUWV‰ΖS‰ΛΉ   ƒμd‰$|$d΅    ‰D$`1ΐΗD$   σ«‹=x  ΗD$   …ÿ…6  ‹†|  ‹$3–x  1Ψ	Π„ÿ   ‹   …Ι…  ‹–`  …Ò…ϊ   ‹†Δ  …ΐ…μ   ‹$~‰|  ‰έΗ†μ  Π  ‰†x  % ω  Η†τ     ƒψΐƒΰ`ΰ  ‰†π  ‹FX΄9Η„‡   ‰t$v ‹Cd‹@…ΐt‹p…φt‹$‰ι‰Ψθόÿÿÿ‹CLX΄9ΗuΩ‹t$‹π  ‹–τ  ‹F‹¶μ  ‰L$‰t$X΄‰T$9Ηt-t& ‹Cd‹@…ΐt‹p…φtL$1Ò‰Ψθόÿÿÿ‹CLX΄9ΗuΨ1ΐ‹T$`d+    u8ƒΔd[^_]ΓΈπÿÿÿλδ‰Χ‰Π‰ΪθόÿÿÿPWh  h  h0  θόÿÿÿƒΔι΅ώÿÿθόÿÿÿt& θόÿÿÿWVS‹t$‹|$θόÿÿÿ‹(  ΅x  …ΐ…Ν  ‰ς‰ω‰Ψ[^_ιώÿÿ΄&    v θόÿÿÿS
  ƒμd‹    ‰\$1ΫT$Ί   f‰T$T$L$Ή   ‰T$‰βΗ$L   θόÿÿÿ‹T$d+    uƒΔ[Γθόÿÿÿθόÿÿÿƒμ 
  d‹    ‰L$1ΙT$T$Ή   ‰T$Ί   f‰T$T$f‰L$Ή   ‰T$‰βΖD$ Η$L   ΗD$L  θόÿÿÿƒψ…ι  ¶D$‹T$d+    uƒΔ Γθόÿÿÿ΄&    v θόÿÿÿWVS‰Γƒμd΅    ‰D$1ΐ΅x  …ΐ…ÿ  ƒϊ„©   ƒϊ…   Ί   ‰ΨθόÿÿÿƒΘƒΘ |$T$Ή   ³
  D$f‰L$‰πΉ   ΖD$ΗD$L   ‰|$θόÿÿÿΈ   Ή   f‰D$Έ   T$f‰D$‰πΗD$L   ‰|$θόÿÿÿ΅x  …ΐ…$  ‹D$d+    uƒΔ[^_Γt& Ί   ‰Ψθόÿÿÿƒΰχι[ÿÿÿθόÿÿÿ΄&    ΄&    θόÿÿÿU‹-x  WV‰ΦS‰Γ…ν…o  ‹“Έ  ‰³l  Bκ‰Ρƒψ†›   ƒϊ!„’   ‹ƒ”  …ΐ„Δ   ‹Hd‹y…ÿ„&  ‹?…ÿ„  kξ‹   iÒ  ‹”0   …Ι„^  ‹)…ν„T  j 1Ιθόÿÿÿ_‹“Έ  ƒϊ„‹   ƒϊ „‚   ƒϊvlBίƒψw
ΉIθ#£Αrk[^_]Γ¶    kώiΒ  ƒΌ,   …WÿÿÿΊ   ‰Ψθόÿÿÿ‹“Έ  ‹ƒ”  ‰Ρ…ΐ…Cÿÿÿ΄&    ƒϊ t}ƒϊtxƒϊw”ƒϊv΅Έ £Πs—f‹ƒ”  …ΐtV‹Hd‹I…ΙtL‹y…ÿtEkξ‹   iÒ  ‹”4   …Ιtc‹i…νt\j 1Ιθόÿÿÿ‹‹Έ  Zλfƒϊ tΈƒϊ… ÿÿÿλ­‰ΡkφΊ   iΙ  ‹„4   ƒψtƒψ…ÿÿÿΊ   ‰Ψ[^_]ι;ύÿÿv j 1Ιθόÿÿÿ‹‹Έ  XλΈ¶    j 1ΙθόÿÿÿYι§ώÿÿθόÿÿÿS‰Γƒμ΅x  …ΐ…―  kÒiƒΈ    ƒΌ4   tƒΔ‰ΨΊ   [ιΡόÿÿƒΔ‰ΨΊ   [ιΐόÿÿθόÿÿÿV‰ΞSθόÿÿÿ‹(  k“l  iƒΈ    ‹„,   ƒθƒψv,‹ƒϊw=΅x  …ΐ…Τ  ‰“p  ‰ΨƒΒθhόÿÿ1ΐ[^Γv ƒ>ΈκÿÿÿΊ    [DΒ^Γt& ΈκÿÿÿλΫθόÿÿÿUWV‰ΦS‰Γƒμ‹x  …Ι…σ  ‹>ƒÿ‡ύ   kΗi“Έ    ‹”,   …Ò„α   ΗF$   i“Έ    ‹„,   Ί    ‹,…@  F‰$‰θθόÿÿÿ‰Αƒψÿ„  ƒψ tƒΑƒω ‡  ‹$‰κθόÿÿÿΗF0χr kΧΗF4    iƒΈ    ‹„,   Ί   ƒθƒψwΗF$   Ί   ‰V(9»l  t1ΐƒΔ[^_]Γt& ‹CƒΓx΄9ΓtεƒΖ8‹Gd‹@…ΐt‹H$…Ιt	‰ς‰ψθόÿÿÿ‹GLx΄9ΓuάλΌΈκÿÿÿλ·v θόÿÿÿVS‰Λθόÿÿÿ‹°(  ΅x  …ΐ…#  ‰Ϊ‰π[^ι¤ώÿÿt& θόÿÿÿS‰Λθόÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…?  1ΐ[Γv θόÿÿÿVS‰Λθόÿÿÿ‹x  ‹°(  …Ò…\  ƒϋw9kΣi†Έ    ‹„,   …ΐt!‰Ϊ‰πθhϋÿÿ‰π‰ΪθOύÿÿ1ΐ[^Γ΄&    v Έκÿÿÿλμ΄&    fθόÿÿÿλ‰΄&    fθόÿÿÿUWVS‰Λƒμ$d‹    ‰T$ 1Òθόÿÿÿ‹°(  ‹†Έ  ƒψ:wƒψ#‡Ξ   Pμƒβϋ„β   ƒθ8ƒψ‡6  ‹…Ò…<  ‹CΊ		 n‰†ά  F\θόÿÿÿ‰Η…ΐ„A  θόÿÿÿ‰D$…ΐ„  ‹Fp΄9Ε„8  f‹Fd‹@…ΐt‹H…Ιt	‰Ϊ‰πθόÿÿÿ‹FLp΄9ΕuάΈd   θόÿÿÿ‹D$Η$    …ΐ…  ‹G‹@θόÿÿÿ1Ò‰ψθόÿÿÿ‹G‹@θόÿÿÿι]  v PάΉ q £Ρrƒψ4„4ÿÿÿι#ÿÿÿt& ‹K‹†x  ΗD$   ‹–|  ΗD$   ‰ά  ‰D$F\‰T$Ί		 ‰L$θόÿÿÿ‰D$Η$   …ΐtθόÿÿÿ‰$…ΐ„k  ‹=x  …ÿ…«  †,  Ί   θόÿÿÿ…ΐ„ν   ‹@‰D$‹†Έ  Pμƒβϋ„δ   ƒθ$ƒψwΊ q £Β‚Ξ   ‹D$…ΐ„Υ  ‹Τ  …Ι„Υ  ‹FnX΄9Εt?΄&    ‹Cd‹@…ΐt‹x…ÿt‹–x  ‹|  ‰Ψθόÿÿÿ‹CLX΄9ΕuÒ‹D$‹Τ  ‹D$T$θόÿÿÿΈd   θόÿÿÿ‹$…Ι„ώ   Η$    ‹D$ d+    …  ‹$ƒΔ$[^_]Γv ƒΎ€  …½ύÿÿv Η$κÿÿÿλΘ΄&    †ψ  ‰D$ι#ÿÿÿ΄&    f‹FΗD$   p΄9Ε…ΚύÿÿΈd   θόÿÿÿΗ$    λ€΄&    ‹|$‹G‹@θόÿÿÿΊ   ‰ψθόÿÿÿ‹G‹@θόÿÿÿιjώÿÿt& ‹G‹@θόÿÿÿΊ   ‰ψθόÿÿÿ‹G‹@θόÿÿÿ‹Fp΄9Ε…PύÿÿΈd   θόÿÿÿι‚ύÿÿ‹|$‹G‹@θόÿÿÿ1Ò‰ψθόÿÿÿ‹G‹@θόÿÿÿιαώÿÿθόÿÿÿv θόÿÿÿιfόÿÿ¶    θόÿÿÿWV‰ΖS‹€ά      ‹ …Β„΅   ‹†ά   ‰Σ‰$  χΒ  t+χΒ   …  φΗ…†   φΗtƒ=x  ‡λ  t& 1ÿφΓu‰Ϊ‰πθόÿÿÿ[^ψ_Γ΄&    v ΎΈ  ‰ψθόÿÿÿ‹†ά   ‹   ‹–ψ  †ψ  9Πt‰Κθΰοÿÿ‰ψΏ   θόÿÿÿλ©f[1ΐ^_Γ΄&    v ƒ=x  ‡L  1ÿφΓtƒλ΄&    v θόÿÿÿ‹x  S‰Γ…Ò…h  ‰ΨΊ   θόÿÿÿ‹ƒδ  …ΐt‹π  ƒβuYθόÿÿÿΗƒδ      ‹ƒΰ  …ΐt‹π  ƒβu%θόÿÿÿΗƒΰ      ‹ƒ0  …ΐt,‰Ψ[ιόÿÿÿt& θόÿÿÿλΩ΄&    fθόÿÿÿλ¥΄&    f[Γ΄&    ΄&    θόÿÿÿUWVS‰Γƒμ@‹x  d΅    ‰D$<1ΐ…Ι…@  Ώ@  Ύ F  ΉΝ   Ί  σ¥Έ   ‰ΝΉ   θόÿÿÿΈΤI  Ί   =ΤI  YUYVΗƒx     EΕΗƒ|      ‰ƒθ  ƒψ  ‰ƒψ  ‰ƒό  ƒX  ‰ƒX  ‰ƒ\  ‰ΨΗƒτ     Ηƒμ  Π  Ηƒπ  ΰ  θόÿÿÿƒ»€  t‹ƒ  ƒψ†ÿ  ³  »  ‰πθόÿÿÿ‹‹|  ‹“x  ‰Ψθόÿÿÿ1Ò‰Ψθΰτÿÿ1Ò‰ΨθΗφÿÿ‰πθόÿÿÿ‹ƒΜ   Ηƒ     Ηƒ        ‰›,  ‰ƒ  ‰ψΗƒ<     Ηƒ@     Ηƒ4     Ηƒ      Ηƒ$      Ηƒ8      ‰³  θόÿÿÿ‰Β…ΐ  ‹ƒΜ   ‰›  «d  Ηƒd        ‰³x  ‰ƒl  ‰θΗƒh     Ηƒ     Ηƒ      Ηƒ”     Ηƒ€      Ηƒ„      Ηƒ      θόÿÿÿ‰Β…ΐ–   ΉK  Ί F  ‰Ψθ=ξÿÿ‰ƒΰ  ‰ΈΈ  ‹ƒΰ  Η€δ    ‹ƒΈ  ƒψ4tƒθ8ƒψ‡  ‹ƒΰ  δ      ‹“  ‹ƒΰ  ³Ό  ‹• J  ‹ΰ   ÿ21Òjθόÿÿÿ‰ΒXY…Ò‰„  ι\  ΄&    ‰Ψ‰$θόÿÿÿ‹$‹D$<d+    …†  ƒΔ@‰Π[^_]Γ¶“„  s„Ò…^  Έ   θόÿÿÿΉ(  i“    P‰πj ”ό   θόÿÿÿ_]‰Ζ…φ„Ήύÿÿ‹ƒ€  ΗD$    ΗD$    ‰D$‰πΗD$    ΗD$   θόÿÿÿf‰D$‹FdΗD$    ‹@…ΐt*‹H$…Ιt#΅   T$…ΐ„   ‹x$…ÿ„•  ‰πθόÿÿÿ‹ƒΈ  ƒψ„ο   ƒψ*„ζ   ƒψ'…&ύÿÿ‹ƒ€  ΗD$(    ΗD$,    ‰D$D$ ΗD$0    ΗD$4    ΗD$8    ΗD$ <  ΗD$$@   ‰D$‹Fd‹@…ΐ„Ξόÿÿ‹H(…Ι„Γόÿÿ΅   T$…ΐ„!  ‹x(…ÿ„  ‰πθόÿÿÿι›όÿÿ΄&    v iΐ  j Ή(  R„ό   ‰Β‰πθόÿÿÿ‰ΖXZι©ώÿÿ΄&    fƒ»€  …Ωύÿÿιδύÿÿ¶    ‹“€  ΗD$(    ΗD$,    ‰T$T$ ΗD$0    ΗD$4    ΗD$8    ΗD$ .  ΗD$$@   ‰T$‹Vd‹R…Ò„Βώÿÿ‹J(…Ι„·ώÿÿ΅   T$…ΐt0‹x(…ÿt)‰πθόÿÿÿ‹ƒΈ  ι‘ώÿÿ΄&    ‰πθόÿÿÿιfώÿÿt& ‰πθόÿÿÿ‹ƒΈ  ιhώÿÿ¶    ‰πθόÿÿÿι…ϋÿÿθόÿÿÿfffffffθόÿÿÿ‹@(‹T$‹€x  Η   % ω  ƒψΐ%€C   ‡  ‰1ΐΓ΄&    ΄&    θόÿÿÿUWVS‰Γ«x  ƒμ‹ ‹“  ‹p(‹ƒ  ƒΐX  ΎΈ  ‰B‹ƒ  Η   p‹ƒ  ‹“  ƒΐ‰B‹ƒ  Η@    ‹†X  9Αtt‹ƒ  ‰L$   ‹†\  ‰$‰ψθόÿÿÿ‹L$‰Β‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰ψθόÿÿÿ‹$‹“  ƒ=|  ‹@‰P‡ύ  ƒΔ[^_]Γ΄&    f‰ψ‰$θόÿÿÿ‹$‰Β‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰ψθόÿÿÿƒ=|  ‡β  ƒΔ[^_]Γ΄&    t& θόÿÿÿUWVS‹h(‹…ά   @  ‹ƒβέ‰½Έ  X  ‰ψθόÿÿÿ‰Ζ‹…X  9Γt@v ‹…X  ‹P‹-x  ‰Q‰
Ί   Η€x     Η€|  "  θόÿÿÿ‹…X  9ΨuΓ[‰ς‰ψ^_]ιόÿÿÿ¶    θόÿÿÿ‰Β‹ ‹@(ιόÿÿÿ΄&    ΄&    θόÿÿÿW1ÒVS‰Γ‹ ‹p(‰Ψθόÿÿÿ‹S‰Α‹†x  % ω  ƒψΐƒΰƒΐ…Òu	[Έκÿÿÿ^_Γiψ@  ‹ST9ϊrκGΧ‰SP“„  ‹Μ   Piΐ   j h   P‰Ψj ‹	θόÿÿÿ1ΐƒΔ[^_Γ΄&    ΄&    θόÿÿÿV‹|  S‹X(‹³X  …Ò…  ‹ƒd  ÿvΉ   P,‰Ψθόÿÿÿ‹ƒά   Ί   ‰  ‹ƒά   ‰4  Ηƒ`      ‰ΨΊ   θόÿÿÿ‹ƒά      ‹ƒΚ"‰‹ƒά      ‹ƒΚ ‰‹ƒά   @  ‹€@  ƒΘ"‰X1ΐ[^Γ¶    θόÿÿÿS‰Λθόÿÿÿ‹€(  ΗCΐό›ΗC   ΗCGREYΗC    ΗC$    ‹€x  φΔωt'ΗC
   ΗC  ΗC   ΗC    1ΐ[Γ΄&    ©ÿÿ tξΗC   1ΐΗC?  ΗC   ΗC    [ΓθόÿÿÿƒβtFVS‰Γ΅|  …ΐ…4  ³Έ  ‰πθόÿÿÿ‹ƒά   ‹$  “X  ‰Ψθόÿÿÿ‰πθόÿÿÿΈ   [^Γ1ΐΓffffffθόÿÿÿV°θ   S‰Γ‰πθόÿÿÿ‰Β‹ƒά   ‹  ‰πθόÿÿÿ‰Ψ[^ΓθόÿÿÿUWV‰ΞS‰Σƒμ€|$4 ‹T$ tΗ    pƒΐΗ@ψ    Η@ό    ‰ΗƒϊÿtΚ € €ƒΗ‰‹T$,…Ò„­   ‹D$0Η$    …ΐ•D$¶    ‹K‹l$$…φt-‰Ψλ΄&    f)Ξθόÿÿÿ…φ„  ‹H9ρvκ‹l$$‰Γ)ρυ‹$…ΐti€|$ tb1Òχt$0ƒϊWΐ%     ;L$$rRD$$   ‰s‰wΗG    ‰Χ‹t$(ƒ$‹$ξ9D$,…kÿÿÿƒΔ‰ψ[^_]Γ΄&    fΈ   W;L$$s®	Θ‰T$   ‰s‰Ψ‰wΗG    +kθόÿÿÿ‹T$‰Γ‹@9Εv5‰Α‰Ψ‰Σv Ι   ƒΓ‰Kτ‹P‰SψΗCό    +hθόÿÿÿ‹H9ιrΨ‰Ϊ‰Γ‰θz   ‰‹CΗB    ‰BιDÿÿÿ¶    ‹H‹l$$‰Γικώÿÿ΄&    v θόÿÿÿWVS‹Μ   ‰Γ‹‘\  ‹X  ‹±P  ‹ΉT  ‰Ρ	Αt
ƒΐƒÒ )πϊR‰ω‰ςPΈ    θόÿÿÿπÿ”ΐZY„ΐt|‹ƒΈ  iΠ  ƒΊ   tiΠ  ‹’   ƒϊ„‚   ƒϊ„   iΐ  ‹€   ƒψ„¤   ƒψt7ƒ
  θόÿÿÿƒ|  θόÿÿÿƒτ   θόÿÿÿ‹ƒά   [^_ιόÿÿÿ[^_Γt& ‰Ψθόÿÿÿλΐ΄&    ‰Ψθόÿÿÿ‹ƒΈ  ιoÿÿÿ¶    ƒά  θόÿÿÿ‹ƒΈ  iΠ  ‹’   ƒϊ…aÿÿÿ‰Ψθόÿÿÿ‹ƒΈ  ιOÿÿÿ΄&    ƒ   θόÿÿÿiƒΈ    ‹€   ι<ÿÿÿθόÿÿÿ…ΐt7S‹X`ϊ v@tϊv@t[Γ9ƒ¤  uυ‹[ιόÿÿÿ9ƒ¤  uε‹[ιόÿÿÿΓ΄&    ΄&    θόÿÿÿφ€4  t*S‹ά   ‹‹  ‹ά   ‹“”  …Ιt…Òu
[Γ΄&    Γ‹ά   ‰‹  ‹ά   ‰“”  ‹ά   Γ  ‹‹ά   Γ ‹‹ά   Γ  ‹‹ά   Γ ‹RQhθ
  ‹€Μ      PθόÿÿÿƒΔ[Γ΄&    v θόÿÿÿUWΏ   V‰ΖSƒμ‰$λM΄&    ‹„  …Ò…Τ  ‹‹Cƒο‰B‰Ί   ‰θΗ   ΗC"  θόÿÿÿ·V‹$)Π…ΐ~O…ÿ~K‹9πtE‹θόÿÿÿ‰“ ύÿÿ«ύÿÿ‰ƒύÿÿ‹FP‰V‰ƒτώÿÿ‹F·Π;$u‚ƒ=„  v‡ιχ  t& ƒΔ[^_]Γ΄&    θόÿÿÿWV1φS‰Γ‹€ά   ‰°   ‹ƒά   ‰°   ‹ƒά   ‰°@  ‹ƒά   ‰°@ ‹ƒά   ‰°@ ‹ƒά   ‰°@  ‹ƒά   ‰°@ ‹ƒά   ‰°   »θ   ‰ψθόÿÿÿ‰Β‹ƒά     ‹‰1‰ψθόÿÿÿ‹ƒά   ‰°   ‹ƒά   ‰°0  ‹ƒά   ‰°@  ‹ƒά   ‰°P  ‹ƒά   ‰°`  [^_Γ΄&    ΄&    θόÿÿÿWV‰ΖS‹ΈΘ   _ό‰Ψθόÿÿÿ‰Ψθόÿÿÿ‰Ψθψώÿÿ‹†L  ‰Ϊθόÿÿÿ‰πθόÿÿÿ‰Ψθ½ϋÿÿGXθόÿÿÿ‰ψθόÿÿÿ‰Ψ[^_ιόÿÿÿt& θόÿÿÿWΈθ   V‰ΦS‰Γ‰ψθόÿÿÿ	³δ   ‰Β‹ƒά     ‹€  	π‰[‰ψ^_ιόÿÿÿ΄&    ΄&    θόÿÿÿWΈθ   V‰Ζ‰ψS‰Σθόÿÿÿ‰Β#δ   t‹†ά     ‹€  	Ψ‰[‰ψ^_ιόÿÿÿ΄&    ¶    θόÿÿÿWΈθ   V‰Ζ‰ψS‰Σθόÿÿÿ‰Β‹†ά     ‹€  χΣ!Γ‰[‰ψ^_ιόÿÿÿv θόÿÿÿWΈθ   V‰Ζ‰ψS‰Σθόÿÿÿ‹ά   ‰Β‰ΨχΠ™  !†δ   ‹‰  !Θ‰[‰ψ^_ιόÿÿÿ΄&    t& θόÿÿÿUW‰ΧVS‰Γƒμ‹r΅„  …φ„  …ΐ…8	  ‹GƒΑ1Òƒαψ‰D$‹G‰L$‰$χρΊ   9ΠFΠ‰T$ƒψ†>  ‹ƒά   Ί  p‰PΊ   ‹ƒά   ‰P1Ò‹ƒά   ‰P1νΗD$    ‹t$‹L$9$ro‰|$1Ιfƒ=„  ‡]	  ‰πƒΰόƒά   ‰$‹D$‹x‹$ο‰8Fƒΰόƒά   ‰Fƒΰόƒά   ‰Fƒΰόƒά   ‰ƒΑƒΖl$9L$w‹|$‹G‹W(ƒΰόƒά   …Ò…  ‹T$(‰‹G1Òƒΐƒΰόƒά   ‰‹G‹L$ƒΐƒΰόƒά   ‰‹G‹t$ƒΐΑζƒΰόΑξƒά   ‰0‹G‹Wƒΐƒΰόƒά   ‰‹G(…ΐ‹G„χ   ƒΐΊ  €ƒΰόƒά   ‰Ί   1Ιt& ‹GΠƒΰόƒά   ‰ƒΒƒϊPuθ‹G‹Wƒΰόƒά   ‰‹G‹L$ƒΰόƒά   ‰‹G$ƒΰόƒά   ‰0‹T$‹G Ακƒΰόƒά   ƒκ‰ƒ=„  ‡	  ƒΔ1ΐ[^_]ΓΊ   ‰ιψώÿÿt& …ΐ…	  ‹G1Òƒΰόƒά   ‰‹Gƒΰόƒά   ‰‹G$ƒΰόƒά   ‰‹G ƒΰόƒά   ‰ƒΔ1ΐ[^_]Γt& ƒΐΊ   ƒΰόƒά   ‰ιÿÿÿ¶    θόÿÿÿUWV‰ΖS‰Σβ  ‹8…€  φΓt`ƒ=„  ‡k  ®΄   ‰θθόÿÿÿ‹†Έ   ƒΰό‡ά   ‹†   θΪωÿÿ‰θθόÿÿÿ‹†θ   ƒΰό‡ά   ‰Έ   [^_]Γ΄&    …ΫuΩ[1ΐ^_]Γt& θόÿÿÿUhW‰ΧV‰Ζ‰θSθόÿÿÿ‹FV9Βt/X΄λ΄&    v ‹CLX΄9Βt9»   uξ‰θθόÿÿÿ‰Ψ[^_]Γ‰θ1Ϋθόÿÿÿ‰Ψ[^_]ΓθόÿÿÿUWV‰Ξ1ΙS‹l$‰Σ‹|$$ƒύÿ•Α1Òƒ|$ÿ•Β   Ρ‹T$T$ j ―Χhΐ  ΑκT―ΡKTRΑβ‰θόÿÿÿZY‰C…ΐtnƒύÿtj‰ι‰ςj Wÿt$,ÿt$,j θτÿÿƒΔƒ|$ÿt'1Òƒύÿ”ΒR‰ςj Wÿt$,ÿt$,h   ‹L$0θqτÿÿƒΔ‰C+C1Òƒΐ;w[‰Π^_]Γ΄&    Ίτÿÿÿλθ΄&    θόÿÿÿU   WV‰ΞS‰Σ‹|$‹T$‹l$j Khΐ  ―ΧΑκTRΑβ‰θόÿÿÿZY‰C…ΐt:1Ò…ν”Β1ΙR‰ςUWj ÿt$$jÿθέσÿÿ1Ò‰C+CƒΔƒΐ;w[‰Π^_]Γt& Ίτÿÿÿλμ΄&    θόÿÿÿUWV‰Ξ1ΙS‹l$‰Σ‹|$$ƒύÿ•Α1Òƒ|$ÿ•Β   Ρ‹T$T$ j ―Χhΐ  ΑκT―ΡKTRΑβ‰θόÿÿÿZY‰C…ΐtnƒύÿtj‰ι‰ςj Wÿt$,ÿt$,j θ/σÿÿƒΔƒ|$ÿt'1Òƒύÿ”ΒR‰ςj Wÿt$,ÿt$,h   ‹L$0θσÿÿƒΔ‰C+C1Òƒΐ;w[‰Π^_]Γ΄&    Ίτÿÿÿλθ΄&    θόÿÿÿS‹„  ‹  ‹€Μ   j ÿ²”  ÿ²     ‰ΪθόÿÿÿƒΔ[Γ΄&    fθόÿÿÿU‰ΥW‰ΟV‰ΖSƒμ‹„  ‹…Ι…q  ‰ΨθΧυÿÿ‹†ΐ   ƒΰόƒά   ‹‹–τ   χÒ!Κ‰kV,‹¤   ‰Ψ“d  ÿ·  θόÿÿÿƒ=„  Z‡²  ‹†Δ   ‹–¤   ƒΰόƒά   ‰iƒΈ        ‹PφΒu‹@¨„  ƒϊ„  ΈΌ θόÿÿÿ‹†μ   …ΐtƒΰό‹–  ƒά   ‰‹†Θ   ‹–  ƒΰόƒά   ‰‹†ΰ   ‹–   ƒΰόƒά   ‰‹†ά   ‹–  ƒΰόƒά   ‰‹†Μ   ‹–ό   ƒΰόƒά   ‰ΈΌ θόÿÿÿ‹†Ό   Ί   ƒΰόƒά   ‰ΗE    ‹ƒΈ  iΠ  ƒΊ   u"‹ƒά   L  ‹ ƒΰό‹»ά   ‰‡L  ‹ƒΈ  iΠ  ƒΊ   u"‹ƒά   L  ‹ ƒΰϋ‹»ά   ‰‡L  ‹ƒΈ  iΐ  ƒΈ   uE‹ƒά   L  ‹ ƒΰώ‹»ά   ƒΘ
‰‡L  ‹ƒά   H  ‹Κ  €‰ΊE ‹ƒά   ‰,  ‹ƒh  =u  t-w  ƒψ‡,  ΅„  …ΐ…–  ‰ΨθΊσÿÿ‹†δ   ƒΰόƒά   ‹–ψ   ‰‹†ΐ   ƒΰόƒά   ‹–τ   ‰»θ   ‰Ψθ{σÿÿ‰ψ‹®π   θόÿÿÿ	«δ   ‰Β‰ψθόÿÿÿ‰ψθόÿÿÿ‰Β‹ƒδ   …ΐut‰ψθόÿÿÿ‰Ψθ>σÿÿ‹ƒά      ‹ƒΚ ‰‰Ψθ%σÿÿiƒΈ    ƒΈ   tbƒ=„  wi‰ψθόÿÿÿ‰Β‹ƒά     ‹ ‰ψθόÿÿÿ‰Ψθεςÿÿ1ΐƒΔ[^_]Γv ‹‹ά   Α  ‹)	θ‰ιuÿÿÿ1Ò‰ΨθόÿÿÿιeύÿÿfΊ   ‰ΨθόÿÿÿλιΚ  ΄&    t& θόÿÿÿUWV‰ΦS‰Γƒμ‹Ί¤   ‹*―Ί¨   1Òθόÿÿÿ‹„  ‰Β…Ι…!  ‹C…ΐuCΈκÿÿÿ…ÿu(‹…Μ   j Γ„  ÿ¶¨   ÿ¶¤   ‹
‰Ϊθόÿÿÿ1ΐƒΔƒΔ[^_]Γ΄&    v ;{Tw‰{PλΌΈκÿÿÿλέ΄&    ΄&    θόÿÿÿUWV‰ΖS‰Σƒμ‹8‹ƒ  ‹’  ƒΐΗΈ  ‰B‹ƒ  Η   p‹ƒ  ‹“  ƒΐ‰B‹ƒ  Η@    ‰ψθόÿÿÿ‹®      