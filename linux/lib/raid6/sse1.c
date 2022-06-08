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
	