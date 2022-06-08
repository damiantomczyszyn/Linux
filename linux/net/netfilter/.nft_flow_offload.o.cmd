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
  �v ����   ����  ��������f����	  ����   ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ������&    �P   �    ��l  �   f��n  ��������  ����   �H܃�������   ����ƅ ��H  ��   �m�����&������H�   ��������������������&��  ǃ     ��ǃ      ǃ$     ������t& ���r���v�H�������뾍�&    �v���믅�t����   ����Ɔ<��   ��`  u]���������`  ���u����=�   �tK��   ����������  �������t& ���m����5   ���O����1����t& ��`  ���������   �������|������`  ��t������ǃ�     ǃ�     ǃ�     ������t& �ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ������&    ǃ�     ���   ǃ�     ǃ�       ǃ     ǃ      ǃ$     ���  ��   �'  ��  ��������     ������   �'  ��  1�������v ���     ������   �'  ��  ��������   ��  �:����������    ���������ȃ���!�	ȃ���   �'  ��  ���������Hu����     �������  �O�����&    f�ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     �5�����&    ǃ�     ǃ�     ǃ�     ������`   �{����D����؉\$�D$H   �D$    ��������   �    �������F  ���6  �1  ������������P(�   ���  ���  ���  �B�T$���1�Í�&    ��&    ������UWVS�Ã�� ���  �p(���  �����  �B���  �   p���  ���  ���B���  �@    ����������  ���  ��x  9�tS���  �M    ���  ���  ��x  ��|  �E �E���  �=x  �H��  ����[^_]������t& ���  ���  ��x  ��|  �E �=x  ��  ����[^_]�����f������UWVS�h(���   @  �������  ���  ��������Ƌ��  9�t@�v ���  �P�-x  �Q�
�   ǀx     ǀ|  "  ��������  9�u�[���^_]�������    �����VS�������(  �F��9�t$�X���Cd� ��t���t��������CL�X�9�u�1�[^Í�&    �t& �����WV��S�������(  ���  ��4t��8��wP�F   ���  ���F��X�9�t(�t& �Cd�@��t�H��t	���������CL�X�9�u�1�[^_Í�    ���  u�������捴&    ������WV��S�������(  ���  ��4t��8��wH���uK�G���X�9�t)�t& ��Cd�@��t�H��t	���������CL�X�9�u�1�[^_Í�    ���  u�������������WV��S�������(  ���  ��4t��8��wX���u[�F�   ��  ���������X�9�t(�t& �Cd�@��t�H��t	���������CL�X�9�u�1�[^_Í�    ���  u�������������S��������;��(  uH�C����w=�C    �C    �C�  ��x  % �  �����`�  �C1�[Í�&    f������[Í�&    f������V��S�\$�������(  ��x  %�� ��u(��҃�Ճ�6������σ�;�C1�[^Í�&    �������&    f������VS���������(  �x  ���  ��x  ��|  �1��S[^Í�&    f������VS���������(  ���  �C���  �K���  �s���  �6�s���  �B�C   ���ȉC1��K[^Ít& ��������u��I  �A,1�Í�&    �v �����Í�&    �v ������ �@(�������&    ��&    ������U1�WVS�Ã�� �p(��������k�ǋ��  �@���  �����  ����   ���  ����  ���  ����   wT���+  ����  ���   ���  ���  j P��j j����������=x  �6  1���[^_]Í�    ���  ���J  ���  ���   ���  ��Q��j P��j Q��������륍�&    �v ���  9ST�  �SP���  ���@����x  ��x   �  ti��    ��  ����   ���p  �$    �����   ���  �T$���  ��R��QQP�t$��D$��������	�����    ���   ���  ���  j Pj�j �����������������    ���  ���   ���  ��Q��j PQ�ɍ�&    �v �������[^_]Ív ����  �$��1��I�����������������VS��������5�I  ��(  9s��   ��x  �S% �  �����`�  ��t^��wy����   �   j �s��  j P�C�S�0   j P��j������C��I  �C   �����C�C�C1�[^Ð���   ��9Kw��   ��랍�&    f��J����   C�노����[^Í�&    �������j����t& ������U��WV��S�Ã�pd�    �D$l��������   ��(  �|$�D$   �$1��=x  ���  ���������D$�����U  �<$��   ���]  ��`  ���O  ���  ���A  ��I  9E��I  E��=x  ���  �E���  �U���  �M���  ��  �E�]0�D$$   �<$�u�M�U�D$(�D$�E(���t$ f�D$0f�D$�E,�L$,f�D$2f�D$��f�\$4��T$�\$��L;|$tY�t& �Cd�@��t�p��t�L$1҉�������CL�X�9�u؋D$(�T$�t$ �L$,�D$�D$0f�D$�D$2f�D$�D$4�U�T$�E0�$�U�T$�u�U(�T$�M�U,���  �E�D$ld+    u�D$��p[^_]��D$�������������&    �t& ������UWVS���������  �   ��(  ��������    i��    ��    ������������  �k���� t���� ��  ������������   ���   ��u���   P�C0h�  P������CT ����  ����4t��8��w�CT �1�[^_]Ít& ����  u�[1�^_]������W��V��S��@�P�V������������������������=x  �  ��������C�B���   �   �C"  [^_�������&    f������WVS�áx  ���+  ���wr�s�{1��C    ������F,    )���0���<��   �    �������������Q  �� t���� �G  ����������C$   1�[^_Í�&    �������퍴&    f���������T����t& �����S���������(  k�l  i��    ��,   �   ����v��p  ��x  ���[  ��[�������&    �t& �����U��WV��S�Ã��x  ���x  �������t]�ǹ�   �C󥉂�  ���  ǂ      ��0  i��    U��    ���  h�  j �T$P������T$����(  ����[^_]Í�&    ��&    ������V�x  S�X(���  ����  ���   @  ������d  �N�v����������   �   ��0  ǃ       �غ   ��������      �������      ��� ����   ��@  ��@  ���X1�[^Ít& ������S���������(  ��l  ��x  ����  1�[Ív ������Љʋ9�t	������t& Í�&    �����UWV��S�˹   ��d�$�|$d�    �D$`1��D$   �=x  �D$   ���6  ��|  �$3�x  1�	���   ��   ���  ��`  ����   ���  ����   �$�~��|  ��ǆ�  �  ��x  % �  ǆ�     �����`�  ���  �F�X�9���   �t$�v �Cd�@��t�p��t�$���������CL�X�9�uًt$���  ���  �F���  �L$�t$�X��T$9�t-�t& ��Cd�@��t�p��t�L$1҉�������CL�X�9�u�1��T$`d+    u8��d[^_]ø������׉Љ������PWh�  h�  h0  �����������������t& �����WVS�t$�|$�������(  �x  ����  �����[^_������&    �v �����S
  ��d�    �\$1ۈT$�   f�T$�T$�L$�   �T$���$L   ������T$d+    u��[�������������� 
  d�    �L$1ɈT$�T$�   �T$�   f�T$�T$f�L$�   �T$���D$ �$L   �D$L  ���������  �D$�T$d+    u�� ��������&    �v �����WVS�Ã�d�    �D$1��x  ����  ����   ����   �   ����������� �|$�T$�   ��
  �D$f�L$��   �D$�D$L   �|$������   �   f�D$�   �T$f�D$���D$L   �|$������x  ���$  �D$d+    u��[^_Ít& �   �����������[����������&    ��&    ������U�-x  WV��S�Å��o  ���  ��l  �B�у���   ��!��   ���  ����   �Hd�y���&  �?���  k��   i�  ��0   ���^  �)���T  j 1������_���  ����   �� ��   ��vl�B߃�w
�I�#��rk[^_]Í�    k�i�  ��,   �W����   ����������  ���  �х��C�����&    �� t}��tx��w���v�� ��s�f����  ��tV�Hd�I��tL�y��tEk��   i�  ��4   ��tc�i��t\j 1���������  Z�f��� t���� ���뭉�k��   i�  ��4   ��t�������   ��[^_]�;����v j 1���������  X븍�    j 1������Y����������S�Ã��x  ����  k�i��    ��4   t���غ   [���������غ   [����������V��S�������(  k�l  i��    ��,   ����v,���w=�x  ����  ��p  �؃��h���1�[^Ív �>������    [D�^Ít& �������ې�����UWV��S�Ã��x  ����  �>����   k�i��    ��,   ����   �F$   i��    ��,   �    �,�@  �F�$�������������  �� t���� �  �$��������F0�r k��F4    i��    ��,   �   ����w�F$   �   �V(9�l  t1���[^_]Ít& �C���x�9�t��8�Gd�@��t�H$��t	���������GL�x�9�u�뼸����뷍v �����VS���������(  �x  ���#  �ډ�[^�����t& �����S���������(  ��l  ��x  ���?  1�[Ív �����VS��������x  ��(  ���\  ��w9k�i��    ��,   ��t!�ډ��h��������O���1�[^Í�&    �v ������썴&    f������뉍�&    f������UWVS�˃�$d�    �T$ 1��������(  ���  ��:w��#��   �P�����   ��8���6  ����<  �C�		� �n���  �F\������ǅ��A  ������D$����  �F�p�9��8  f��Fd�@��t�H��t	�ډ�������FL�p�9�uܸd   ������D$�$    ����  �G�@�����1҉�������G�@������]  �v �Pܹ q ��r��4�4����#����t& ��K��x  �D$   ��|  �D$   ���  �D$�F\�T$�		� �L$������D$�$   ��t������$���k  �=x  ����  ��,  �   ���������   �@�D$���  �P�����   ��$��w� q ����   �D$����  ���  ����  �F�n�X�9�t?��&    �Cd�@��t�x��t��x  ��|  ��������CL�X�9�uҋD$���  �D$�T$������d   ������$����   �$    �D$ d+    �  �$��$[^_]Ív ���  ������v �$�����ȍ�&    ���  �D$�#�����&    f��F�D$   �p�9�������d   ������$    뀍�&    �|$�G�@������   ��������G�@������j����t& ��G�@������   ��������G�@������F�p�9��P����d   �����������|$�G�@�����1҉�������G�@����������������v ������f�����    �����WV��S���      � ����   ���   �Ӊ�$  ��  t+��   �  ����   ��t�=x  ��  �t& 1���u�ډ������[^�_Í�&    �v ���  ����������   ��   ���  ���  9�t����������   ������f�[1�^_Í�&    �v �=x  �L  1���t�뚍�&    �v ������x  S�Å��h  �غ   ��������  ��t���  ��uY�����ǃ�      ���  ��t���  ��u%�����ǃ�      ��0  ��t,��[������t& ������ٍ�&    f������륍�&    f�[Í�&    ��&    �����UWVS�Ã�@�x  d�    �D$<1����@  �@  ��F  ��   �  �   �͹   �������I  �   �=�I  YUYVǃx     E�ǃ|      ���  ���  ���  ���  ��X  ��X  ��\  ��ǃ�     ǃ�  �  ǃ�  �  ��������  t���  ����  ���  ��  ���������|  ��x  �������1҉������1҉����������������   ǃ     ǃ     �   ��,  ��  ��ǃ<     ǃ@     ǃ4  �  ǃ      ǃ$      ǃ8      ��  ��������  ���   ���  ��d  ǃd     �   ��x  ��l  ��ǃh     ǃ�     ǃ�     ǃ�  �  ǃ�      ǃ�      ǃ�      ���������   �K  ��F  ���=������  ���  ���  ǀ�    ���  ��4t��8���  ���  ���      ���  ���  ���  �� J  ���   �21�j�������XY����  �\  ��&    �؉$������$�D$<d+    ��  ��@��[^_]����  �s���^  �   ������(  i��  �  P��j ���   �����_]�ƅ���������  �D$    �D$    �D$���D$    �D$   �����f�D$�Fd�D$    �@��t*�H$��t#�   �T$����  �x$����  ����������  ����   ��*��   ��'�&������  �D$(    �D$,    �D$�D$ �D$0    �D$4    �D$8    �D$ <  �D$$@   �D$�Fd�@��������H(��������   �T$���!  �x(���  �������������&    �v i��  j �(  R���   ���������XZ������&    f����  ������������    ���  �D$(    �D$,    �T$�T$ �D$0    �D$4    �D$8    �D$ .  �D$$@   �T$�Vd�R��������J(��������   �T$��t0�x(��t)����������  ������&    ��������f����t& ����������  �h�����    ����������������f�f�f�f�f�f�f��������@(�T$��x  �   % �  ���%�C   �  �1�Í�&    ��&    ������UWVS�Í�x  ��� ���  �p(���  ����X  ���  �B���  �   p���  ���  ���B���  �@    ��X  9�tt���  �L$�   ��\  �$��������L$��\  ��\  ��x  ��|  �(��������$���  �=|  �@�P��  ��[^_]Í�&    f����$������$��\  ��\  ��x  ��|  �(��������=|  ��  ��[^_]Í�&    �t& �����UWVS�h(���   @  ���݉���  ��X  ��������Ƌ�X  9�t@�v ��X  �P�-x  �Q�
�   ǀx     ǀ|  "  �������X  9�u�[���^_]�������    ������ �@(�������&    ��&    ������W1�VS�Ë �p(��������S����x  % �  ���������u	[�����^_�i�@  �ST9�r�G׉SP���  ���   Pi��  j h�  P��j �	�����1���[^_Í�&    ��&    �����V�|  S�X(��X  ���  ��d  �v��  �P,����������   �   ���  ���   ��4  ǃ`      �غ   ��������      ���"����      ��� ����   ��@  ��@  ��"�X1�[^Í�    �����S���������(  �C����C�  �CGREY�C    �C$    ��x  ���t'�C
   �C  �C   �C    1�[Í�&    ��� t��C   1��C?  �C   �C    [Ð�������tFVS�á|  ���4  ���  ����������   ��$  ��X  ���������������   [^�1��f�f�f�f�f�f�������V���   S�É���������   ��  ���������[^������UWV��S�Ӄ��|$4 �T$ t�    p���@�    �@�    �ǃ��t�� � �����T$,����   �D$0�$    ���D$��    �K�l$$��t-�����&    f�)���������  �H9�v�l$$��)���$��ti�|$ tb1��t$0���W�%     ;L$$rRD$$   �s�w�G    �׋t$(�$�$�9D$,�k�������[^_]Í�&    f��   �W;L$$s�	ȉT$   �s�؉w�G    +k������T$�Ë@9�v5���؉Ӎv ��   ���K�P�S��C�    +h������H9�r؉ډÉ�z   ��C�B    �B�D�����    �H�l$$���������&    �v �����WVS���   �Ë�\  ��X  ��P  ��T  ��	�t
���� )��R����P�    ���������ZY��t|���  i�  ��   ti�  ��   ����   ����   i�  ��   ����   ��t7��
  �������|  ��������   ��������   [^_�����[^_Ít& �����������&    ����������  �o�����    ���  ��������  i�  ��   ���a�������������  �O�����&    ���   �����i��    ��   �<����������t7S�X`�� v@t��v@t[Ð9��  u��[�����9��  u�[�����Í�&    ��&    ��������4  t*S���   ���  ���   ���  ��t��u
[Í�&    Ë��   ���  ���   ���  ���   ��  ����   �� ����   ��  ����   �� �RQh�
  ���   �   P�������[Í�&    �v �����UW�   V��S���$�M��&    ���  ����  ��C���B��   ���   �C"  ������V�$)Ѕ�~O��~K�9�tE�������������������������F�P�V�������F��;$u��=�  v���  �t& ��[^_]Í�&    ������WV1�S�Ë��   ��   ���   ��   ���   ��@  ���   ��@ ���   ��@ ���   ��@  ���   ��@ ���   ��   ���   ����������   ��  ��1����������   ��   ���   ��0  ���   ��@  ���   ��P  ���   ��`  [^_Í�&    ��&    �����WV��S���   �_������������������������L  ���������������������GX��������������[^_������t& �����W���   V��S�É������	��   ���   ��  ��  	��[��^_�������&    ��&    ������W���   V�Ɖ�S���������#��   t���   ��  ��  	؉[��^_�������&    ��    �����W���   V�Ɖ�S����������   ��  ��  ��!É[��^_������v �����W���   V�Ɖ�S����������   ���Ѝ�  !��   ��  !ȉ[��^_�������&    �t& �����UW��VS�Ã��r��  ���  ���8	  �G��1҃���D$�G�L$�$��   9�FЉT$���>  ���   �  p�P�   ���   �P1ҋ��   �P1��D$    �t$�L$9$ro�|$1�f��=�  �]	  �������   �$�D$�x�$�8�F�����   ��F�����   ��F�����   �����l$9L$w��|$�G�W(�����   ���  �T$(��G1҃������   ��G�L$�������   ��G�t$�����������   �0�G�W�������   ��G(���G��   ���  ������   ��   1ɍt& ��GЃ����   �����Pu�G�W�����   ��G�L$�����   ��G$�����   �0�T$�G �������   ����=�  ��	  ��1�[^_]ú   �������t& ���	  �G1҃����   ��G�����   ��G$�����   ��G �����   ���1�[^_]Ít& ���   �����   �������    �����UWV��S�Ӂ�  �8��  ��t`�=�  �k  ���   ����������   �����   ����   ���������������   �����   ��   [^_]Í�&    ���u�[1�^_]Ít& ������U�hW��V�Ɖ�S������F�V9�t/�X����&    �v �CL�X�9�t9��   u���������[^_]É�1��������[^_]������UWV��1�S�l$�Ӌ|$$�����1҃|$����   ыT$T$ j ��h�  ���T�эK�TR��������ZY�C��tn���tj���j W�t$,�t$,j �������|$�t'1҃����R��j W�t$,�t$,h   �L$0�q������C+C1҃�;w[��^_]Í�&    �������荴&    �����U�   WV��S�Ӌ|$�T$�l$j �Kh�  �����T�R��������ZY�C��t:1҅���1�R��UWj �t$$j������1҉C+C����;w[��^_]Ít& ������썴&    �����UWV��1�S�l$�Ӌ|$$�����1҃|$����   ыT$T$ j ��h�  ���T�эK�TR��������ZY�C��tn���tj���j W�t$,�t$,j �/������|$�t'1҃����R��j W�t$,�t$,h   �L$0�������C+C1҃�;w[��^_]Í�&    �������荴&    �����S���  ���  ���   j ���  ���  �   ���������[Í�&    f������U��W��V��S����  ����q  ����������   �����   ����   ��!ʉkV,���   ���d  ���  ������=�  Z��  ���   ���   �����   �i��        �P��u�@���  ����  ��� ��������   ��t�����  ��   ����   ��  �����   ����   ��   �����   ����   ��  �����   ����   ���   �����   ���� ��������   �   �����   ��E    ���  i�  ��   u"���   L  � ������   ��L  ���  i�  ��   u"���   L  � ������   ��L  ���  i�  ��   uE���   L  � ������   ��
��L  ���   H  ���  ���E ���   ��,  ��h  =u  t-w  ���,  ��  ����  ���������   �����   ���   ����   �����   ���   ����   ���{��������   �����	��   �����������������   ��ut����������>������      ��� ����%���i��    ��   tb�=�  wi����������     � ��������������1���[^_]Ív ���   ��  �)	��u����1҉�������e���f��   ����������  ��&    �t& �����UWV��S�Ã����   �*���   1��������  ���!  �C��uC�������u(���   j �Ä  ���   ���   �
�������1�����[^_]Í�&    �v ;{Tw�{P뼸�����ݍ�&    ��&    ������UWV��S�Ӄ��8���  ���  ���Ǹ  �B���  �   p���  ���  ���B���  �@    ����������   ���   