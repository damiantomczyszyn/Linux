k_struct *curr,
				 struct task_struct *next)
{
	if (static_branch_unlikely(&preempt_notifier_key))
		__fire_sched_out_preempt_notifiers(curr, next);
}

#else /* !CONFIG_PREEMPT_NOTIFIERS */

static inline void fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
}

static inline void
fire_sched_out_preempt_notifiers(struct task_struct *curr,
				 struct task_struct *next)
{
}

#endif /* CONFIG_PREEMPT_NOTIFIERS */

static inline void prepare_task(struct task_struct *next)
{
#ifdef CONFIG_SMP
	/*
	 * Claim the task as running, we do this before switching to it
	 * such that any running task will have this set.
	 *
	 * See the ttwu() WF_ON_CPU case and its ordering comment.
	 */
	WRITE_ONCE(next->on_cpu, 1);
#endif
}

static inline void finish_task(struct task_struct *prev)
{
#ifdef CONFIG_SMP
	/*
	 * This must be the very last reference to @prev from this CPU. After
	 * p->on_cpu is cleared, the task can be moved to a different CPU. We
	 * must ensure this doesn't happen until the switch is completely
	 * finished.
	 *
	 * In particular, the load of prev->state in finish_task_switch() must
	 * happen before this.
	 *
	 * Pairs with the smp_cond_load_acquire() in try_to_wake_up().
	 */
	smp_store_release(&prev->on_cpu, 0);
#endif
}

#ifdef CONFIG_SMP

static void do_balance_callbacks(struct rq *rq, struct callback_head *head)
{
	void (*func)(struct rq *rq);
	struct callback_head *next;

	lockdep_assert_rq_held(rq);

	while (head) {
		func = (void (*)(struct rq *))head->func;
		next = head->next;
		head->next = NULL;
		head = next;

		func(rq);
	}
}

static void balance_push(struct rq *rq);

struct callback_head balance_push_callback = {
	.next = NULL,
	.func = (void (*)(struct callback_head *))balance_push,
};

static inline struct callback_head *splice_balance_callbacks(struct rq *rq)
{
	struct callback_head *head = rq->balance_callback;

	lockdep_assert_rq_held(rq);
	if (head)
		rq->balance_callback = NULL;

	return head;
}

static void __balance_callbacks(struct rq *rq)
{
	do_balance_callbacks(rq, splice_balance_callbacks(rq));
}

static inline void balance_callbacks(struct rq *rq, struct callback_head *head)
{
	unsigned long flags;

	if (unlikely(head)) {
		raw_spin_rq_lock_irqsave(rq, flags);
		do_balance_callbacks(rq, head);
		raw_spin_rq_unlock_irqrestore(rq, flags);
	}
}

#else

static inline void __balance_callbacks(struct rq *rq)
{
}

static inline struct callback_head *splice_balance_callbacks(struct rq *rq)
{
	return NULL;
}

static inline void balance_callbacks(struct rq *rq, struct callback_head *head)
{
}

#endif

static inline void
prepare_lock_switch(struct rq *rq, struct task_struct *next, struct rq_flags *rf)
{
	/*
	 * Since the runqueue lock will be released by the next
	 * task (which is an invalid locking op but in the case
	 * of the scheduler it's an obvious special-case), so we
	 * do an early lockdep release here:
	 */
	rq_unpin_lock(rq, rf);
	spin_release(&__rq_lockp(rq)->dep_map, _THIS_IP_);
#ifdef CONFIG_DEBUG_SPINLOCK
	/* this is a valid case when another task releases the spinlock */
	rq_lockp(rq)->owner = next;
#endif
}

static inline void finish_lock_switch(struct rq *rq)
{
	/*
	 * If we are tracking spinlock dependencies then we have to
	 * fix up the runqueue lock - which gets 'carried over' from
	 * prev into current:
	 */
	spin_acquire(&__rq_lockp(rq)->dep_map, 0, 0, _THIS_IP_);
	__balance_callbacks(rq);
	raw_spin_rq_unlock_irq(rq);
}

/*
 * NOP if the arch has not defined these:
 */

#ifndef prepare_arch_switch
# define prepare_arch_switch(next)	do { } while (0)
#endif

#ifndef finish_arch_post_lock_switch
# define finish_arch_post_lock_switch()	do { } while (0)
#endif

static inline void kmap_local_sched_out(void)
{
#ifdef CONFIG_KMAP_LOCAL
	if (unlikely(current->kmap_ctrl.idx))
		__kmap_local_sched_out();
#endif
}

static inline void kmap_local_sched_in(void)
{
#ifdef CONFIG_KMAP_LOCAL
	if (unlikely(current->kmap_ctrl.idx))
		__kmap_local_sched_in();
#endif
}

/**
 * prepare_task_switch - prepare to switch tasks
 * @rq: the runqueue preparing to switch
 * @prev: the current task that is being switched out
 * @next: the task we are going to switch to.
 *
 * This is called with the rq lock held and interrupts off. It must
 * be paired with a subsequent finish_task_switch after the context
 * switch.
 *
 * prepare_task_switch sets up locking and calls architecture specific
 * hooks.
 */
static inline void
prepare_task_switch(struct rq *rq, struct task_struct *prev,
		    struct task_struct *next)
{
	kcov_prepare_switch(prev);
	sched_info_switch(rq, prev, next);
	perf_event_task_sched_out(prev, next);
	rseq_preempt(prev);
	fire_sched_out_preempt_notifiers(prev, next);
	kmap_local_sched_out();
	prepare_task(next);
	prepare_arch_switch(next);
}

/**
 * finish_task_switch - clean up after a task-switch
 * @prev: the thread we just switched away from.
 *
 * finish_task_switch must be called after the context switch, paired
 * with a prepare_task_switch call before the context switch.
 * finish_task_switch will reconcile locking set up by prepare_task_switch,
 * and do any other architecture-specific cleanup actions.
 *
 * Note that we may have delayed dropping an mm in context_switch(). If
 * so, we finish that here outside of the runqueue lock. (Doing it
 * with the lock held can cause deadlocks; see schedule() for
 * details.)
 *
 * The context switch have flipped the stack from under us and restored the
 * local variables which were saved when this task called schedule() in the
 * past. prev == current is still correct but we need to recalculate this_rq
 * because prev may have moved to another CPU.
 */
static struct rq *finish_task_switch(struct task_struct *prev)
	__releases(rq->lock)
{
	struct rq *rq = this_rq();
	struct mm_struct *mm = rq->prev_mm;
	unsigned int prev_state;

	/*
	 * The previous task will have left us with a preempt_count of 2
	 * because it left us after:
	 *
	 *	schedule()
	 *	  preempt_disable();			// 1
	 *	  __schedule()
	 *	    raw_spin_lock_irq(&rq->lock)	// 2
	 *
	 * Also, see FORK_PREEMPT_COUNT.
	 */
	if (WARN_ONCE(preempt_count() != 2*PREEMPT_DISABLE_OFFSET,
		      "corrupted preempt_count: %s/%d/0x%x\n",
		      current->comm, current->pid, preempt_count()))
		preempt_count_set(FORK_PREEMPT_COUNT);

	rq->prev_mm = NULL;

	/*
	 * A task struct has one reference for the use as "current".
	 * If a task dies, then it sets TASK_DEAD in tsk->state and calls
	 * schedule one last time. The schedule call will never return, and
	 * the scheduled task must drop that reference.
	 *
	 * We must observe prev->state before clearing prev->on_cpu (in
	 * finish_task), otherwise a concurrent wakeup can get prev
	 * running on another CPU and we could rave with its RUNNING -> DEAD
	 * transition, resulting in a double drop.
	 */
	prev_state = READ_ONCE(prev->__state);
	vtime_task_switch(prev);
	perf_event_task_sched_in(prev, current);
	finish_task(prev);
	tick_nohz_task_switch();
	finish_lock_switch(rq);
	finish_arch_post_lock_switch();
	kcov_finish_switch(current);
	/*
	 * kmap_local_sched_out() is invoked with rq::lock held and
	 * interrupts disabled. There is no requirement for that, but the
	 * sched out code does not have an interrupt enabled section.
	 * Restoring the maps on sched in does not require interrupts being
	 * disabled either.
	 */
	kmap_local_sched_in();

	fire_sched_in_preempt_notifiers(current);
	/*
	 * When switching through a kernel thread, the loop in
	 * membarrier_{private,global}_expedited() may have observed that
	 * kernel thread and not issued an IPI. It is therefore possible to
	 * schedule between user->kernel->user threads without passing though
	 * switch_mm(). Membarrier requires a barrier after storing to
	 * rq->curr, before returning to userspace, so provide them here:
	 *
	 * - a full memory barrier for {PRIVATE,GLOBAL}_EXPEDITED, implicitly
	 *   provided by mmdrop(),
	 * - a sync_core for SYNC_CORE.
	 */
	if (mm) {
		membarrier_mm_sync_core_before_usermode(mm);
		mmdrop_sched(mm);
	}
	if (unlikely(prev_state == TASK_DEAD)) {
		if (prev->sched_class->task_dead)
			prev->sched_class->task_dead(prev);

		/* Task is done with its stack. */
		put_task_stack(prev);

		put_task_struct_rcu_user(prev);
	}

	return rq;
}

/**
 * schedule_tail - first thing a freshly forked thread must call.
 * @prev: the thread we just switched away from.
 */
asmlinkage __visible void schedule_tail(struct task_struct *prev)
	__releases(rq->lock)
{
	/*
	 * New tasks start with FORK_PREEMPT_COUNT, see there and
	 * finish_task_switch() for details.
	 *
	 * finish_task_switch() will drop rq->lock() and lower preempt_count
	 * and the preempt_enable() will end up enabling preemption (on
	 * PREEMPT_COUNT kernels).
	 */

	finish_task_switch(prev);
	preempt_enable();

	if (current->set_child_tid)
		put_user(task_pid_vnr(current), current->set_child_tid);

	calculate_sigpending();
}

/*
 * context_switch - switch to the new MM and the new thread's register state.
 */
static __always_inline struct rq *
context_switch(struct rq *rq, struct task_struct *prev,
	       struct task_struct *next, struct rq_flags *rf)
{
	prepare_task_switch(rq, prev, next);

	/*
	 * For paravirt, this is coupled with an exit in switch_to to
	 * combine the page table reload and the switch backend into
	 * one hypercall.
	 */
	arch_start_context_switch(prev);

	/*
	 * kernel -> kernel   lazy + transfer active
	 *   user -> kernel   lazy + mmgrab() active
	 *
	 * kernel ->   user   switch + mmdrop() active
	 *   user ->   user   switch
	 */
	if (!next->mm) {                                // to kernel
		enter_lazy_tlb(prev->active_mm, next);

		next->active_mm = prev->active_mm;
		if (prev->mm)                           // from user
			mmgrab(prev->active_mm);
		else
			prev->active_mm = NULL;
	} else {                                        // to user
		membarrier_switch_mm(rq, prev->active_mm, next->mm);
		/*
		 * sys_membarrier() requires an smp_mb() between setting
		 * rq->curr / membarrier_switch_mm() and returning to userspace.
		 *
		 * The below provides this either through switch_mm(), or in
		 * case 'prev->active_mm == next->mm' through
		 * finish_task_switch()'s mmdrop().
		 */
		switch_mm_irqs_off(prev->active_mm, next->mm, next);

		if (!prev->mm) {                        // from kernel
			/* will mmdrop() in finish_task_switch(). */
			rq->prev_mm = prev->active_mm;
			prev->active_mm = NULL;
		}
	}

	rq->clock_update_flags &= ~(RQCF_ACT_SKIP|RQCF_REQ_SKIP);

	prepare_lock_switch(rq, next, rf);

	/* Here we just switch the register state and the stack. */
	switch_to(prev, next, prev);
	barrier();

	return finish_task_switch(prev);
}

/*
 * nr_running and nr_context_switches:
 *
 * externally visible scheduler statistics: current number of runnable
 * threads, total number of context switches performed since bootup.
 */
unsigned int nr_running(void)
{
	unsigned int i, sum = 0;

	for_each_online_cpu(i)
		sum += cpu_rq(i)->nr_running;

	return sum;
}

/*
 * Check if only the current task is running on the CPU.
 *
 * Caution: this function does not check that the caller has disabled
 * preemption, thus the result might have a time-of-check-to-time-of-use
 * race.  The caller is responsible to use it correctly, for example:
 *
 * - from a non-preemptible section (of course)
 *
 * - from a thread that is bound to a single CPU
 *
 * - in a loop with very short iterations (e.g. a polling loop)
 */
bool single_task_running(void)
{
	return raw_rq()->nr_running == 1;
}
EXPORT_SYMBOL(single_task_running);

unsigned long long nr_context_switches(void)
{
	int i;
	unsigned long long sum = 0;

	for_each_possible_cpu(i)
		sum += cpu_rq(i)->nr_switches;

	return sum;
}

/*
 * Consumers of these two interfaces, like for example the cpuidle menu
 * governor, are using nonsensical data. Preferring shallow idle state selection
 * for a CPU that has IO-wait which might not even end up running the task when
 * it does become runnable.
 */

unsigned int nr_iowait_cpu(int cpu)
{
	return atomic_read(&cpu_rq(cpu)->nr_iowait);
}

/*
 * IO-wait accounting, and how it's mostly bollocks (on SMP).
 *
 * The idea behind IO-wait account is to account the idle time that we could
 * have spend running if it were not for IO. That is, if we were to improve the
 * storage performance, we'd have a proportional reduction in IO-wait time.
 *
 * This all works nicely on UP, where, when a task blocks on IO, we account
 * idle time as IO-wait, because if the storage were faster, it could've been
 * running and we'd not be idle.
 *
 * This has been extended to SMP, by doing the same for each CPU. This however
 * is broken.
 *
 * Imagine for instance the case where two tasks block on one CPU, only the one
 * CPU will have IO-wait accounted, while the other has regular idle. Even
 * though, if the storage were faster, both could've ran at the same time,
 * utilising both CPUs.
 *
 * This means, that when looking globally, the current IO-wait accounting on
 * SMP is a lower bound, by reason of under accounting.
 *
 * Worse, since the numbers are provided per CPU, they are sometimes
 * interpreted per CPU, and that is nonsensical. A blocked task isn't strictly
 * associated with any one particular CPU, it can wake to another CPU than it
 * blocked on. This means the per CPU IO-wait number is meaningless.
 *
 * Task CPU affinities can make all that even more 'interesting'.
 */

unsigned int nr_iowait(void)
{
	unsigned int i, sum = 0;

	for_each_possible_cpu(i)
		sum += nr_iowait_cpu(i);

	return sum;
}

#ifdef CONFIG_SMP

/*
 * sched_exec - execve() is a valuable balancing opportunity, because at
 * this point the task has the smallest effective memory and cache footprint.
 */
void sched_exec(void)
{
	struct task_struct *p = current;
	unsigned long flags;
	int dest_cpu;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	dest_cpu = p->sched_class->select_task_rq(p, task_cpu(p), WF_EXEC);
	if (dest_cpu == smp_processor_id())
		goto unlock;

	if (likely(cpu_active(dest_cpu))) {
		struct migration_arg arg = { p, dest_cpu };

		raw_spin_unlock_irqrestore(&p->pi_lock, flags);
		stop_one_cpu(task_cpu(p), migration_cpu_stop, &arg);
		return;
	}
unlock:
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
}

#endif

DEFINE_PER_CPU(struct kernel_stat, kstat);
DEFINE_PER_CPU(struct kernel_cpustat, kernel_cpustat);

EXPORT_PER_CPU_SYMBOL(kstat);
EXPORT_PER_CPU_SYMBOL(kernel_cpustat);

/*
 * The function fair_sched_class.update_curr accesses the struct curr
 * and its field curr->exec_start; when called from task_sched_runtime(),
 * we observe a high rate of cache misses in practice.
 * Prefetching this data results in improved performance.
 */
static inline void prefetch_curr_exec_start(struct task_struct *p)
{
#ifdef CONFIG_FAIR_GROUP_SCHED
	struct sched_entity *curr = (&p->se)->cfs_rq->curr;
#else
	struct sched_entity *curr = (&task_rq(p)->cfs)->curr;
#endif
	prefetch(curr);
	prefetch(&curr->exec_start);
}

/*
 * Return accounted runtime for the task.
 * In case the task is currently running, return the runtime plus current's
 * pending runtime that have not been accounted yet.
 */
unsigned long long task_sched_runtime(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;
	u64 ns;

#if defined(CONFIG_64BIT) && defined(CONFIG_SMP)
	/*
	 * 64-bit doesn't need locks to atomically read a 64-bit value.
	 * So we have a optimization chance when the task's delta_exec is 0.
	 * Reading ->on_cpu is racy, but this is ok.
	 *
	 * If we race with it leaving CPU, we'll take a lock. So we're correct.
	 * If we race with it entering CPU, unaccounted time is 0. This is
	 * indistinguishable from the read occurring a few cycles earlier.
	 * If we see ->on_cpu without ->on_rq, the task is leaving, and has
	 * been accounted, so we're correct here as well.
	 */
	if (!p->on_cpu || !task_on_rq_queued(p))
		return p->se.sum_exec_runtime;
#endif

	rq = task_rq_lock(p, &rf);
	/*
	 * Must be ->curr _and_ ->on_rq.  If dequeued, we would
	 * project cycles that may never be accounted to this
	 * thread, breaking clock_gettime().
	 */
	if (task_current(rq, p) && task_on_rq_queued(p)) {
		prefetch_curr_exec_start(p);
		update_rq_clock(rq);
		p->sched_class->update_curr(rq);
	}
	ns = p->se.sum_exec_runtime;
	task_rq_unlock(rq, p, &rf);

	return ns;
}

#ifdef CONFIG_SCHED_DEBUG
static u64 cpu_resched_latency(struct rq *rq)
{
	int latency_warn_ms = READ_ONCE(sysctl_resched_latency_warn_ms);
	u64 resched_latency, now = rq_clock(rq);
	static bool warned_once;

	if (sysctl_resched_latency_warn_once && warned_once)
		return 0;

	if (!need_resched() || !latency_warn_ms)
		return 0;

	if (system_state == SYSTEM_BOOTING)
		return 0;

	if (!rq->last_seen_need_resched_ns) {
		rq->last_seen_need_resched_ns = now;
		rq->ticks_without_resched = 0;
		return 0;
	}

	rq->ticks_without_resched++;
	resched_latency = now - rq->last_seen_need_resched_ns;
	if (resched_latency <= latency_warn_ms * NSEC_PER_MSEC)
		return 0;

	warned_once = true;

	return resched_latency;
}

static int __init setup_resched_latency_warn_ms(char *str)
{
	long val;

	if ((kstrtol(str, 0, &val))) {
		pr_warn("Unable to set resched_latency_warn_ms\n");
		return 1;
	}

	sysctl_resched_latency_warn_ms = val;
	return 1;
}
__setup("resched_latency_warn_ms=", setup_resched_latency_warn_ms);
#else
static inline u64 cpu_resched_latency(struct rq *rq) { return 0; }
#endif /* CONFIG_SCHED_DEBUG */

/*
 * This function gets called by the timer code, with HZ frequency.
 * We call it with interrupts disabled.
 */
void scheduler_tick(void)
{
	int cpu = smp_processor_id();
	struct rq *rq = cpu_rq(cpu);
	struct task_struct *curr = rq->curr;
	struct rq_flags rf;
	unsigned long thermal_pressure;
	u64 resched_latency;

	arch_scale_freq_tick();
	sched_clock_tick();

	rq_lock(rq, &rf);

	update_rq_clock(rq);
	thermal_pressure = arch_scale_thermal_pressure(cpu_of(rq));
	update_thermal_load_avg(rq_clock_thermal(rq), rq, thermal_pressure);
	curr->sched_class->task_tick(rq, curr, 0);
	if (sched_feat(LATENCY_WARN))
		resched_latency = cpu_resched_latency(rq);
	calc_global_load_tick(rq);
	sched_core_tick(rq);

	rq_unlock(rq, &rf);

	if (sched_feat(LATENCY_WARN) && resched_latency)
		resched_latency_warn(cpu, resched_latency);

	perf_event_task_tick();

#ifdef CONFIG_SMP
	rq->idle_balance = idle_cpu(cpu);
	trigger_load_balance(rq);
#endif
}

#ifdef CONFIG_NO_HZ_FULL

struct tick_work {
	int			cpu;
	atomic_t		state;
	struct delayed_work	work;
};
/* Values for ->state, see diagram below. */
#define TICK_SCHED_REMOTE_OFFLINE	0
#define TICK_SCHED_REMOTE_OFFLINING	1
#define TICK_SCHED_REMOTE_RUNNING	2

/*
 * State diagram for ->state:
 *
 *
 *          TICK_SCHED_REMOTE_OFFLINE
 *                    |   ^
 *                    |   |
 *                    |   | sched_tick_remote()
 *                    |   |
 *                    |   |
 *                    +--TICK_SCHED_REMOTE_OFFLINING
 *                    |   ^
 *                    |   |
 * sched_tick_start() |   | sched_tick_stop()
 *                    |   |
 *                    V   |
 *          TICK_SCHED_REMOTE_RUNNING
 *
 *
 * Other transitions get WARN_ON_ONCE(), except that sched_tick_remote()
 * and sched_tick_start() are happy to leave the state in RUNNING.
 */

static struct tick_work __percpu *tick_work_cpu;

static void sched_tick_remote(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct tick_work *twork = container_of(dwork, struct tick_work, work);
	int cpu = twork->cpu;
	struct rq *rq = cpu_rq(cpu);
	struct task_struct *curr;
	struct rq_flags rf;
	u64 delta;
	int os;

	/*
	 * Handle the tick only if it appears the remote CPU is running in full
	 * dynticks mode. The check is racy by nature, but missing a tick or
	 * having one too much is no big deal because the scheduler tick updates
	 * statistics and checks timeslices in a time-independent way, regardless
	 * of when exactly it is running.
	 */
	if (!tick_nohz_tick_stopped_cpu(cpu))
		goto out_requeue;

	rq_lock_irq(rq, &rf);
	curr = rq->curr;
	if (cpu_is_offline(cpu))
		goto out_unlock;

	update_rq_clock(rq);

	if (!is_idle_task(curr)) {
		/*
		 * Make sure the next tick runs within a reasonable
		 * amount of time.
		 */
		delta = rq_clock_task(rq) - curr->se.exec_start;
		WARN_ON_ONCE(delta > (u64)NSEC_PER_SEC * 3);
	}
	curr->sched_class->task_tick(rq, curr, 0);

	calc_load_nohz_remote(rq);
out_unlock:
	rq_unlock_irq(rq, &rf);
out_requeue:

	/*
	 * Run the remote tick once per second (1Hz). This arbitrary
	 * frequency is large enough to avoid overload but short enough
	 * to keep scheduler internal stats reasonably up to date.  But
	 * first update state to reflect hotplug activity if required.
	 */
	os = atomic_fetch_add_unless(&twork->state, -1, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_OFFLINE);
	if (os == TICK_SCHED_REMOTE_RUNNING)
		queue_delayed_work(system_unbound_wq, dwork, HZ);
}

static void sched_tick_start(int cpu)
{
	int os;
	struct tick_work *twork;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_RUNNING);
	if (os == TICK_SCHED_REMOTE_OFFLINE) {
		twork->cpu = cpu;
		INIT_DELAYED_WORK(&twork->work, sched_tick_remote);
		queue_delayed_work(system_unbound_wq, &twork->work, HZ);
	}
}

#ifdef CONFIG_HOTPLUG_CPU
static void sched_tick_stop(int cpu)
{
	struct tick_work *twork;
	int os;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	/* There cannot be competing actions, but don't rely on stop-machine. */
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_OFFLINING);
	WARN_ON_ONCE(os != TICK_SCHED_REMOTE_RUNNING);
	/* Don't cancel, as this would mess up the state machine. */
}
#endif /* CONFIG_HOTPLUG_CPU */

int __init sched_tick_offload_init(void)
{
	tick_work_cpu = alloc_percpu(struct tick_work);
	BUG_ON(!tick_work_cpu);
	return 0;
}

#else /* !CONFIG_NO_HZ_FULL */
static inline void sched_tick_start(int cpu) { }
static inline void sched_tick_stop(int cpu) { }
#endif

#if defined(CONFIG_PREEMPTION) && (defined(CONFIG_DEBUG_PREEMPT) || \
				defined(CONFIG_TRACE_PREEMPT_TOGGLE))
/*
 * If the value passed in is equal to the current preempt count
 * then we just disabled preemption. Start timing the latency.
 */
static inline void preempt_latency_start(int val)
{
	if (preempt_count() == val) {
		unsigned long ip = get_lock_parent_ip();
#ifdef CONFIG_DEBUG_PREEMPT
		current->preempt_disable_ip = ip;
#endif
		trace_preempt_off(CALLER_ADDR0, ip);
	}
}

void preempt_count_add(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON((preempt_count() < 0)))
		return;
#endif
	__preempt_count_add(val);
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Spinlock count overflowing soon?
	 */
	DEBUG_LOCKS_WARN_ON((preempt_count() & PREEMPT_MASK) >=
				PREEMPT_MASK - 10);
#endif
	preempt_latency_start(val);
}
EXPORT_SYMBOL(preempt_count_add);
NOKPROBE_SYMBOL(preempt_count_add);

/*
 * If the value passed in equals to the current preempt count
 * then we just enabled preemption. Stop timing the latency.
 */
static inline void preempt_latency_stop(int val)
{
	if (preempt_count() == val)
		trace_preempt_on(CALLER_ADDR0, get_lock_parent_ip());
}

void preempt_count_sub(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON(val > preempt_count()))
		return;
	/*
	 * Is the spinlock portion underflowing?
	 */
	if (DEBUG_LOCKS_WARN_ON((val < PREEMPT_MASK) &&
			!(preempt_count() & PREEMPT_MASK)))
		return;
#endif

	preempt_latency_stop(val);
	__preempt_count_sub(val);
}
EXPORT_SYMBOL(preempt_count_sub);
NOKPROBE_SYMBOL(preempt_count_sub);

#else
static inline void preempt_latency_start(int val) { }
static inline void preempt_latency_stop(int val) { }
#endif

static inline unsigned long get_preempt_disable_ip(struct task_struct *p)
{
#ifdef CONFIG_DEBUG_PREEMPT
	return p->preempt_disable_ip;
#else
	return 0;
#endif
}

/*
 * Print scheduling while atomic bug:
 */
static noinline void __schedule_bug(struct task_struct *prev)
{
	/* Save this before calling printk(), since that will clobber it */
	unsigned long preempt_disable_ip = get_preempt_disable_ip(current);

	if (oops_in_progress)
		return;

	printk(KERN_ERR "BUG: scheduling while atomic: %s/%d/0x%08x\n",
		prev->comm, prev->pid, preempt_count());

	debug_show_held_locks(prev);
	pri΍��mo(:铌�M��I�����r�@�X[�qN�Z��:���B���B���/KF^� �����8^�5]�#�K�F`��Cy~!;+���X��,�4��������6r;3��/Uh���%� 0���40t�)���A�>�J-�=��DT�xz�A�4!d:�(*�!1H���h���uVA���F�V��7	�Z�j��M��u�q(G����@����͡є�!{�Q&!`�B����6�봍EԋRf���#����@0��v9�~／��-�>�/S��[|,3r�ߞO#���eM�����@	ӭ����'�c24W��y�0����8C�
�X�84�����9�4��W���%L�SfY���2�L���ÿ�v���b� aU}���x��Bf����H���?�K< ����K}�O�n��NN�B�A�rt��w���y�T���uU�_]SМhq��]��%�C�O܃�O���XS2J�A�I��BU����w<�1��s�����)�r_�m�'�`(���n{��(z��z���@�#䢋��q�/y��&*˗�e�[�N��Q��ç0qO�a�UC[8�0��~��J���H��r�rgk4cRHfz.=w)���ٻiFV�1B}i<���W��Z>��ׄRe��v�WW�e����#A�2�%r���Y��2��ޮ*\h�nݟ����"�	응����b?����po�#z�Ĕ���c�?���B�e�����	��k��"���~Bkbi0�R-T�<5z<.�3?_|���[�BTa8,�
d#�����e5Ӱ[d,���N+�؟o|���"��q�X�c0��J�4<0���������[�S��C�3���J�sW����mܑN)gc7�i���<5��U�V[�:?�D�Y���q��X%����[�2���)����ǡ��{�'e��&��z;ʔ�QWO����ؤ��#�uΝ���W=�b���y�kb{�_l�{��T�38&�Q�!�*�aw��)*F2|]I;$�:6��ʨ�+@�z�g]�8���KcE���z�5��ι�dTVx� ��@1ۓ#�3�ΧѤ�%�(7Ս�@��<(f	�M��h�R*�&��*4��6����l�7�,��~=������w"��n��|%р,�&�|���e��g~$5�O$�ޤQ��i�>�E>C�
K� � ш;l�^3��=����:2�a�eky�*�籀 ��r�����tl��M��Q�r�E���~������� ��3}�h�h�L��C#�
R�P�srd��`!e.,*(:� ��dV(,� �dg�Ü��:�>��F^EꄟA+!�/x��7t[gܣ���#�k���0~&&v;���z�y/���"�)�G���L#5�M^�&�ј�L�����E�nw8��՜<�uoqA��L&����G�E����sZ��IgSؠ��X��)����I�΃��ԕ'W�>�lP��Ý&�9i�j.WD� R��*��δ��]X�1���T-���D�q<�|ST����;���2R�1�;.pa-�.�rh�t"&�k����/zo�Y�Uy�J�J��w��s#O�׉q�a'��HҢ8�iy��~��w:_�2��i~0U�a'� QZ����k�\����I�q�ި�Ir^gg�)�蹀��
�: M6�ѭ1�zD��7-���Z���U��b�;��˭{}��Q�*����d�
r�W��a�m���tYMu�T����`t�K���u������R[F�͉o���;s���V��з}5j~
oש�)�����:�\�E*J���L7I@���z˾�8e��-^�����?�JI��D@�E��_�TwU��s�Ȑ��������,���#��m�Dn��)Pb۠Mj�8bٰ�
�
�q�[�.�wX]��n�2+����[�¶'��C�kR���Ϡ;��Rb�.	����)u϶��P^��0�xG��r����m���.=h"ᬮ|�[�D�=s��\�\U�4l�;��?l߉���v� ����)�Rw��D���qf^���IѪ�Ą����7K^Y:�C�,D`�d�R�J`��}dG�������y����?��܃̂�sr���v%�������8�*�lT�0�z�\�*az�8�{�g��_��a���+M�Κ�/�Kn�3;��/X�Z���T�'u�/�Ԗ��5�f��~��פx��61���h����`�\��1*�M�ʠ|1�Q��r�Iz�Y��եڃ܊��5�֨�[O�t�e`v���b�X���1,t��8��r`lY�� Ú�F_�(ley��$u6%'��!B�v�a�S`����Z��68Λ���k���ඖg���TSVʢ.�$5�䝚����	�T���(�S	yrhp���$H��;(�,���i$[���NH�\� x�l��e�3�����.rK�b�����y���nɖ� ��ͪ���Q���@�̓�QL�S��_�ai����-�&�-�r�F�i	�Z�s<9�̉�<"�3ݘ��3Eb"���*d�A�ݡ�!�]h���˒)��x���E��������ܵ�{N>('�V�1��J�GJf75�ue��1�� �Q��O[����d1ޗ,����@rve甞�Rh���(Yۗҿ�̿�[����$C��Y���M����$��@L�m�l���}��W�Bw�m�|�b��BF�,V�{�2B#(�"-iՏ�_�W ¹��"���S�����6�ﲀ����Hf�^Y (�s%VD���2�q�y�1�h㛦��I�&���=,t���bXQ�h�[�r�y/���KwvEN��Px@{�����L��<������B/�V����MF�!C�.{�~�t¨�XR��^i�>���	ۙ�]#�����y �9��:Kr���E���JTN� Fq��_q y�����]�J���.��~�@,�f�96�6~D�ug�Z�Ⱥ���yb/e���$��?vh������d��+����T�ׯ����W/��^Y3�Gn��%[fA-�o�2|S�t5�#�/����~6g�ūs���j�h�����e%��I�V1�@��6;��������+kQY#ʧI"��>`k�B9_J��b[�3#E�|���pފ�G�wh��g��+�R��r�j��H%�+�v������6ժ�"xshs���B�����U�%g�f�`ӎU����p���o�<>?��C�����&��"%�l�$bxm_!�1�A�q&��Ǐ�ÑsT��#��qN]ۋ�g��74B�h��Q��?��d��c�Ŧ:n�X:h�H5��O30Xvt��B�^m�)a�^}�5wz6��}����8]vU�ʻZ=$m�
�Y���B̙��0���/��u��������OB7�Ԃ_'�p-gaƍ���w۞T-88�h��i���9�wDB��[Ѭ�*�^#�S�g�Җ��R�	$��H�S���\���A�$Ú��8�M��y���n�P�&,�/X(Q~%CB���B�<�������T7�l��L�2`�P�>��]�������K���$͇鄤�ɫ����Ki�z��ޡ���|�U9���raUc,���.\B�&�S�%�"Y��X	����I �dV�8�]�ab�~p>��i�?!�B����%���6�\��6�~���8��%�
w���V�m/���)uKt��S]X=�%�%�(�j���ΔP����$�M!	IEH�i���D���,
�X��6^��"+>��&"�۪* ]�#)z�����l*��M�!#D�ao0�!�¯�h��d�<�{>��^M�OfV�s4��nm�8��:��)�S.O`2Qޓ6��Bu����6�q� �o@�gM��q!b@q�9&���/�M��e��9X�`��[���N�/զ��1L�U�G=/��~�1 ��N=�sd��&��f�b�	��j�'���j���X�= z;�k�9#�W4��
���??�8	a�`��,�~�L���z&�U�f���tLȊ��V��>N�.|�
ὥ,�-��Y����w�ڸw��̱A�v�E�f�ڽ���w���+�����u�<.Û:�����y퀷�SO��\o����g���`E��h�@�͘�-�d}J�ʕ�1� ��-�Tm5�0"��>��PӤ�渡b�U����]��]W�O�/�`j,*�9�G�B�w�%~(�2�����(�/�b�h�F�����%�5B���;L���o��Ж[�u��	g��x�#��I�a ����06_������
��^�p�){�q,d�ED�=�6�5������D�Y�1��U,$���K��#������7��or��I��(���/?Ě�J�M˭	[	�0Ĵ��2PM٥6Q�� ʮ��=�h����J����\��D7���w�Ӹ�f�p��Q�:�h�8�Cz�+���@���x��j@|�m����xlCY�-cp-�ƌl��?=���6��rM�KK�*����	���v"���;���������g�A�t���۽v�Q�=��� ėa����z�Y�.dG{e>IcrO�%�4�2���5�s�%hx��n�!�m��̀�ӈ-Y���q��	�em���h�ɫi�w�]�����#%�և��8�P �JD�xF,>�;���� �q���4��J���[�|i�ɼC��+u�·D�*U��W|.�<���S��ʗ���{�כ����z��-����H]���4��d����P�pw�����V/��B�īIV�# ��=j��8$Fv�y�;%�����xY,�^ꮰ��|��j�B'OvǞ�ݬj? �%���{��J�i�J῀���AW��O�ՠ���Sp+�"�;�9M�8�շ*M�Z���bo����L�ĸ,n�$�2��њ(yU:�u#2�*K�h:�zTVG��P|gg>�)��Wi>��S8UN�8�����)��<�f$��Y�,�@G��vo.�O��N�o����[��x��k<R`���
�t�|ߌ2�={��+��'��FZ��`��uz7���`���y��{��E!4d���l�� Db�.*C�����'�{v��t���:�%K�m�,D�/v�>��b�Ð �V��~���Ჯ�>�e��v�w�.q�]�̢��!-�͐�U ~
�+(�2p��\Y��U���y����[����[����Gs2��-e��_�����j���0U�ƀ΄���,��С=>�����m/�=߼�n�v� F��^Bsn�ٱ��%I⡙�.vf*P1ڬ�LOҁ���a��A�>̤CAO!�D�[����+��C�&�<7�@�Z���K�7��2���BmSD�Ctx?,�CÃٱ
��oZ�����i�X�<2ҡ�d�{��E����]�Й�%��u���antp�M
Jc�k&�A�Q_ʇ��D�O�G��)�d����g��,��'m�O�������\��y8Q��5��SD�<�p(��?�ݗ�F�{�~��{H/FL�O�Y���<���u{��^�u���s���4w���fE_�f���˻VC�/r�_��W��H?Mv"�a��$�aC%�)���"~����Y�����7)�n;$M���m�*A�p�Xȃ�/Hy����N2<�r�1�s���^<E�>����3�s�v�3������Q���	g-Km*.��Lh�Iif#� :����c܌!���ݥ�S����M�lg�e�EӤ��+�-�=A�iW
���iI����&��*#kQm�`?[�'��S���:˚=����׹i�G����נ2�Z�uK-��lھPj#]G}ҹ2�Y}D)D�'�ܕ�Aq�˳�C�F��!uij
l�x}��6;./��G��zlSU����b�@Ź�1v�C/��Hq�_n��܇���45S3+���P�#�mԭ(s�Hhs�UU�nD�C�����B�f��mx��+�m 3���J��(����\Q�p�vǲ�9A�c����u��C6[�]��b�s��_=��籧��[+�ܷu�ߐsu��R�x�Z>���� 8��3��J� +�˒0�Un��l�<�X��Fo�cA�\�wç-9�r��h(��MA
T��^$2"�`!�ᗷ��h��y�2ȕ����g2��e6T�J��ɇ��H�Wo����,�u�5a�� ��'����9=E;ܒ�H�r;J��vl�h���h�r�����wQm	C�k��W!��:)�I|Yʟh��1s>�|����U�6�ri�оeC�H�tﮃ����#�F�/C�+nQ,֦̅�w!�u��W�!��,�I�|*|42ʻ[Q��!��J��)���ƽ�����1����F����Z 	y�3'rŁ�ȨQ4F��$*�S�YxwY�F3���*ة�+=U���m�z
#���G���iN��Bu��@�\*� L�S�-��v6��QAlQ�C�Ŝk��1��)�M�,��ʽsy����X��<+���>��9��a���P���q��V�wO���S�R��1���;���+g�A�п��Awʁ-�Ч�C����ֈ��~&MZ݂������J�;�&Wo@�zut"���k�¸'Ѽ�e�UԔ��lǵ}0�}�]�~�9����8"ߌ��O7����K���R{ �X��Yz^���r� �ע�LwUOK�!`S����5�	U�do>|���a������r�/���2 ��e�q�ٮJ{��A�n�'������V�;Q�����^��������&",F���sP�E��1{�'��وU�|W��y��S�]^�����ժ��K���
�� s�5� .e*>�_�䶍�/X�'[]�ݾ+�C�S��{F�{�u���tqj5X�$ܰ��1��d��MLCkS����2�?9�>ᆒj��a���}#�k"����c���}������]C� ��VR�ݚ�kz�Hـ�0fM���J<-���$.6���j�4��Q����T}��_�DU$���](��������yr
ڡ�u�b4�$��NIFk�'���/��27.-�xW��ũ8Td�Ů�� ����%��s6m�fÿAr��Yإ�kE�������CM�'�
y��S�-�|�"@A{`˳�t4d�R���_��F��!��Tr�������h���.�D�A�	�KÃ)�"y�]�y�̭�>ְ����'�(p�F����zM����`� r��~�g��j�c"q:Y��v��Uk�吠����.��Ur��Tˁ����b�k 7j�g�H��&`�����\�1�t�`5=�`��Ȱ�i_��jܱT�$UK8%�f����&SG��&�u�
��e	��&u�����.5�	�E�{���j�xB��v��>��g�ZM�#K���yLY?�@�h-u��&T����>���
�3�!�� <�Iyi���NSQl���iG�*>��?���#��bB4:f�(5����"�e�z��-�5����9p���uu�DJ�o���]��e�WIkş8, �L/�k��Yԁ�=����v����
����!��4:HC��b������%Crbz�Ym�<7��O�<�cO׶���R�K�;o�S���+�.�I��'?�Q���}�TE62(=�|�E�E�*pQ� ��<�5^�D���*����.Q�=4��T*���:/�C#�j�R�;8��r�FЛ�V'��]�*� a�F��۴�$}�`1��H��VdO�'���CAM��ǖx��#��UE4cE�k$Lq��ꛂ���l�~�AXA&�Y���^��M�p�?�X&�+1�Ξq�-�� �����F���?z6]�l�<S�h�`'g�Q�{y���\�Z�X~�'��5 C�ŴΒ���n8�.�r�-�/�R�I�|W!��A��N,[<�C$�4rk�fhkl�Oy���^���Of�Ў�// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <media/v4l2-common.h>

static unsigned int i2c_debug;
module_param(i2c_debug, int, 0644);
MODULE_PARM_DESC(i2c_debug, "enable debug messages [i2c]");

static unsigned int i2c_scan;
module_param(i2c_scan, int, 0444);
MODULE_PARM_DESC(i2c_scan, "scan i2c bus at insmod time");

#define dprintk(level, fmt, arg...)\
	do { if (i2c_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: i2c:" fmt), \
			__func__, ##arg); \
	} while (0)

#define I2C_WAIT_DELAY 32
#define I2C_WAIT_RETRY 64

#define I2C_EXTEND  (1 << 3)
#define I2C_NOSTOP  (1 << 4)

static inline int i2c_slave_did_ack(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x01;
}

static inline int i2c_is_busy(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x02 ? 1 : 0;
}

static int i2c_wait_done(struct i2c_adapter *i2c_adap)
{
	int count;

	for (count = 0; count < I2C_WAIT_RETRY; count++) {
		if (!i2c_is_busy(i2c_adap))
			break;
		udelay(I2C_WAIT_DELAY);
	}

	if (I2C_WAIT_RETRY == count)
		return 0;

	return 1;
}

static int i2c_sendbytes(struct i2c_adapter *i2c_adap,
			 const struct i2c_msg *msg, int joined_rlen)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	u32 wdata, addr, ctrl;
	int retval, cnt;

	if (joined_rlen)
		dprintk(1, "%s(msg->wlen=%d, nextmsg->rlen=%d)\n", __func__,
			msg->len, joined_rlen);
	else
		dprintk(1, "%s(msg->len=%d)\n", __func__, msg->len);

	/* Deal with i2c probe functions with zero payload */
	if (msg->len == 0) {
		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, bus->i2c_period | (1 << 2));
		if (!i2c_wait_done(i2c_adap))
			return -EIO;
		if (!i2c_slave_did_ack(i2c_adap))
			return -ENXIO;

		dprintk(1, "%s() returns 0\n", __func__);
		return 0;
	}


	/* dev, reg + first byte */
	addr = (msg->addr << 25) | msg->buf[0];
	wdata = msg->buf[0];
	ctrl = bus->i2c_period | (1 << 12) | (1 << 2);

	if (msg->len > 1)
		ctrl |= I2C_NOSTOP | I2C_EXTEND;
	else if (joined_rlen)
		ctrl |= I2C_NOSTOP;

	cx_write(bus->reg_addr, addr);
	cx_write(bus->reg_wdata, wdata);
	cx_write(bus->reg_ctrl, ctrl);

	if (!i2c_wait_done(i2c_adap))
		goto eio;
	if (i2c_debug) {
		printk(KERN_DEBUG " <W %02x %02x", msg->addr << 1, msg->buf[0]);
		if (!(ctrl & I2C_NOSTOP))
			pr_cont(" >\n");
	}

	for (cnt = 1; cnt < msg->len; cnt++) {
		/* following bytes */
		wdata = msg->buf[cnt];
		ctrl = bus->i2c_period | (1 << 12) | (1 << 2);

		if (cnt < msg->len - 1)
			ctrl |= I2C_NOSTOP | I2C_EXTEND;
		else if (joined_rlen)
			ctrl |= I2C_NOSTOP;

		cx_write(bus->reg_addr, addr);
		cx_write(bus->reg_wdata, wdata);
		cx_write(bus->reg_ctrl, ctrl);

		if (!i2c_wait_done(i2c_adap))
			goto eio;
		if (i2c_debug) {
			pr_cont(" %02x", msg->buf[cnt]);
			if (!(ctrl & I2C_NOSTOP))
				pr_cont(" >\n");
		}
	}
	return msg->len;

 eio:
	retval = -EIO;
	if (i2c_debug)
		pr_err(" ERR: %d\n", retval);
	return retval;
}

static int i2c_readbytes(struct i2c_adapter *i2c_adap,
			 const struct i2c_msg *msg, int joined)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	u32 ctrl, cnt;
	int retval;


	if (i2c_debug && !joined)
		dprintk(1, "%s(msg->len=%d)\n", __func__, msg->len);

	/* Deal with i2c probe functions with zero payload */
	if (msg->len == 0) {
		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, bus->i2c_period | (1 << 2) | 1);
		if (!i2c_wait_done(i2c_adap))
			return -EIO;
		if (!i2c_slave_did_ack(i2c_adap))
			return -ENXIO;


		dprintk(1, "%s() returns 0\n", __func__);
		return 0;
	}

	if (i2c_debug) {
		if (joined)
			dprintk(1, " R");
		else
			dprintk(1, " <R %02x", (msg->addr << 1) + 1);
	}

	for (cnt = 0; cnt < msg->len; cnt++) {

		ctrl = bus->i2c_period | (1 << 12) | (1 << 2) | 1;

		if (cnt < msg->len - 1)
			ctrl |= I2C_NOSTOP | I2C_EXTEND;

		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, ctrl);

		if (!i2c_wait_done(i2c_adap))
			goto eio;
		msg->buf[cnt] = cx_read(bus->reg_rdata) & 0xff;
		if (i2c_debug) {
			dprintk(1, " %02x", msg->buf[cnt]);
			if (!(ctrl & I2C_NOSTOP))
				dprintk(1, " >\n");
		}
	}
	return msg->len;

 eio:
	retval = -EIO;
	if (i2c_debug)
		pr_err(" ERR: %d\n", retval);
	return retval;
}

static int i2c_xfer(struct i2c_adapter *i2c_adap,
		    struct i2c_msg *msgs, int num)
{
	int i, retval = 0;

	dprintk(1, "%s(num = %d)\n", __func__, num);

	for (i = 0 ; i < num; i++) {
		dprintk(1, "%s(num = %d) addr = 0x%02x  len = 0x%x\n",
			__func__, num, msgs[i].addr, msgs[i].len);
		if (msgs[i].flags & I2C_M_RD) {
			/* read */
			retval = i2c_readbytes(i2c_adap, &msgs[i], 0);
		} else if (i + 1 < num && (msgs[i + 1].flags & I2C_M_RD) &&
			   msgs[i].addr == msgs[i + 1].addr) {
			/* write then read from same address */
			retval = i2c_sendbytes(i2c_adap, &msgs[i],
					       msgs[i + 1].len);
			if (retval < 0)
				goto err;
			i++;
			retval = i2c_readbytes(i2c_adap, &msgs[i], 1);
		} else {
			/* write */
			retval = i2c_sendbytes(i2c_adap, &msgs[i], 0);
		}
		if (retval < 0)
			goto err;
	}
	return num;

 err:
	return retval;
}

static u32 cx23885_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_SMBUS_EMUL | I2C_FUNC_I2C;
}

static const struct i2c_algorithm cx23885_i2c_algo_template = {
	.master_xfer	= i2c_xfer,
	.functionality	= cx23885_functionality,
};

/* ----------------------------------------------------------------------- */

static const struct i2c_adapter cx23885_i2c_adap_template = {
	.name              = "cx23885",
	.owner             = THIS_MODULE,
	.algo              = &cx23885_i2c_algo_template,
};

static const struct i2c_client cx23885_i2c_client_template = {
	.name	= "cx23885 internal",
};

static char *i2c_devs[128] = {
	[0x10 >> 1] = "tda10048",
	[0x12 >> 1] = "dib7000pc",
	[0x1c >> 1] = "lgdt3303",
	[0x80 >> 1] = "cs3308",
	[0x82 >> 1] = "cs3308",
	[0x86 >> 1] = "tda9887",
	[0x32 >> 1] = "cx24227",
	[0x88 >> 1] = "cx25837",
	[0x84 >> 1] = "tda8295",
	[0x98 >> 1] = "flatiron",
	[0xa0 >> 1] = "eeprom",
	[0xc0 >> 1] = "tuner/mt2131/tda8275",
	[0xc2 >> 1] = "tuner/mt2131/tda8275/xc5000/xc3028",
	[0xc8 >> 1] = "tuner/xc3028L",
};

static void do_i2c_scan(char *name, struct i2c_client *c)
{
	unsigned char buf;
	int i, rc;

	for (i = 0; i < 128; i++) {
		c->addr = i;
		rc = i2c_master_recv(c, &buf, 0);
		if (rc < 0)
			continue;
		pr_info("%s: i2c scan: found device @ 0x%04x  [%s]\n",
		       name, i, i2c_devs[i] ? i2c_devs[i] : "???");
	}
}

/* init + register i2c adapter */
int cx23885_i2c_register(struct cx23885_i2c *bus)
{
	struct cx23885_dev *dev = bus->dev;

	dprintk(1, "%s(bus = %d)\n", __func__, bus->nr);

	bus->i2c_adap = cx23885_i2c_adap_template;
	bus->i2c_client = cx23885_i2c_client_template;
	bus->i2c_adap.dev.parent = &dev->pci->dev;

	strscpy(bus->i2c_adap.name, bus->dev->name,
		sizeof(bus->i2c_adap.name));

	bus->i2c_adap.algo_data = bus;
	i2c_set_adapdata(&bus->i2c_adap, &dev->v4l2_dev);
	i2c_add_adapter(&bus->i2c_adap);

	bus->i2c_client.adapter = &bus->i2c_adap;

	if (0 == bus->i2c_rc) {
		dprintk(1, "%s: i2c bus %d registered\n", dev->name, bus->nr);
		if (i2c_scan) {
			pr_info("%s: scan bus %d:\n",
					dev->name, bus->nr);
			do_i2c_scan(dev->name, &bus->i2c_client);
		}
	} else
		pr_warn("%s: i2c bus %d register FAILED\n",
			dev->name, bus->nr);

	/* Instantiate the IR receiver device, if present */
	if (0 == bus->i2c_rc) {
		struct i2c_board_info info;
		static const unsigned short addr_list[] = {
			0x6b, I2C_CLIENT_END
		};

		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "ir_video", I2C_NAME_SIZE);
		/* Use quick read command for probe, some IR chips don't
		 * support writes */
		i2c_new_scanned_device(&bus->i2c_adap, &info, addr_list,
				       i2c_probe_func_quick_read);
	}

	return bus->i2c_rc;
}

int cx23885_i2c_unregister(struct cx23885_i2c *bus)
{
	i2c_del_adapter(&bus->i2c_adap);
	return 0;
}

void cx23885_av_clk(struct cx23885_dev *dev, int enable)
{
	/* write 0 to bus 2 addr 0x144 via i2x_xfer() */
	char buffer[3];
	struct i2c_msg msg;
	dprintk(1, "%s(enabled = %d)\n", __func__, enable);

	/* Register 0x144 */
	buffer[0] = 0x01;
	buffer[1] = 0x44;
	if (enable == 1)
		buffer[2] = 0x05;
	else
		buffer[2] = 0x00;

	msg.addr = 0x44;
	msg.flags = I2C_M_TEN;
	msg.len = 3;
	msg.buf = buffer;

	i2c_xfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             �$�������q��d�S�����-~�����;������D�����k4�HI;���vۃe� =v\iqKd]��6%�&�x�&cT�P��0SRXp����o�Oq(��
l�{�g��3��On��q�HA�44�y��\�Hż�i���D�4s��WZ�:����s�j��㫋�|��~e��,�q*L�{�G�o�+V��u �v:Q��8�";��*�NFS#!�M�K���m[���")�&�c�p����2S����h���)q/�;K�3l�h��,�Ae�`䲸O��h�S)��ǩ60Z�>.��p�gu+�=-�+f�ә$��z�O|�
�^��o�V�E�>��d@d��Bl�TC�Kޙ��'��t��b* �JuG|�}Šr9��/�H�oa��+�v�n��+��|�Ă�|͋Yl��P�%�
����P�V�������T#�k#%,�i4i���\�%T˳�"��u���3Jr��8	�]��=2�,�(D������=RT>�Sb~�� Z��+l3�_�Z���H�N&�v�Cxu'�G�>�bW)��,�o��D�T��{*=����[M��;��&��P�,�ٮ���[:�.N!��q�Xe
��F��<0�K���	�古�*���s�܋|m�s&�r��6��g���"H�q�	��f͊�m-��B�絢X�VC�"i�*�m���ц�U�PN<�n��[k�Y�����uɗ��^�\r�"�-��߉ �[��ej-���&/���s�5�W4�I�bB��nʞ�C�̱t�Wk?����8��ǅ�2�:+��ސ8�̲�k6�]|
�2 ���gt�$�D���Z��  X��>\�Q!��N$pӠ�	)��J����m�L�\_�gV��:~/�D�YM��R��A�J

 9���!U��Y��/<{�o���}� ��E��9�����ۃ�G|�)Ɯ�X�@�3w�lM]�GN��M��Ax̝�CW�0Q�-F�i�˿d���hD��J��j�\Y�c<��y�A���+���M1�/�wg�J|K��"ה�&�F,��Vg=�"�19^�:�,+^���Q����3�����Cl<w�`�6��)��W���;ۋra��	����ߵ�I��$��oU� ���}��)ӊ�'9b�V�צM��a����np�ct�x����\Q��#L寜�<�`��"�������eb�|��z����ǜ���
W�( p^(L����XҪ�0��@���m���D�I@RzWA�Vr�x�: !�VL����#D��o�$��E�[�Na��؆ŴTZ��X����iG�-���C��јn��E,Jx���PwoN�s�R�-[���E����3b��1T�n����T���yMC��:Տ��h�mJāWd�,O�<��]�P�1��"��Dqh���������=��+��wI�=S�2���/�M�wV��n�mc1�:�� �� _\�T��C`����t��D�P3�yDR Q5�9��b_F��~��@�C��A���EI�N�5��p�,�0E�ט�ِ%�̯�������h�gz�p����۴-y���Ô�u�{��/"��]�O$��AOv�NX��U]�ص��kE���F�It�t�I6[cu7L)R��-�A}�g4q� ,�c5��o�hfz)ֱg`��h�G�֨U�'�V�(�_��l���5��z�_��^��ǌ]�O��i�bI�qi)�0w(���]��&������`a�g�h�/H��<ln1�;���,ա�_v�;oF�zu~5�_N=j�yU�[��N���{h�o����n�aY&*�h7��bg�0�Rrd�Ł`>4
x,��ΐ1���ל8w.��ߨ9h�K3c^��Yj#T�i�u3K"�_>�m��������>��.O z��������^�ԛ�1�p@��/�lc��ϯL����b��[O���_��n��f�BG�3�U}yTX�t6�,|�Wa6!�EG
L���h0S��󄑁���+p��l�x�B1�jO��o�D�V��~x����N1���#�������{�ݧ9��[�ʏ	�I�[���7
��w�ϔ׺�� ��^q�9{s�u��M�4���3��7{��0����z@W�B�1�J� �H��`痯�� ����s0���Ws����ů�=4�&�B�^�&C�&nK��ҧl2*G�+.���mػ�+z`ģ.
j7m(u��V�#��t�ې�w�����99����m'�|���f�����t�ǯ+���n��*�#-�,@?�DM��a��=aZb�]����*܂�r����ѳ뚧3h�a�N�2������Xl%x��za��uߋ|/���x�pE���	4g�%JN��[V"1��<!eA��F�^�7�v1�հ��Јj�(o��R�&��Z�ԭ0���{�}���~�JX_mw��n�##V�7U�I�Oԏ��:�$�/�ϔ�F0�>��4�ްnj	�c����
�d�gl�GUO9��H�+��(��|Ћ��&�28?XE�lf��Մs�z*���m��/��Xհ����l"U"�s��].�]��ŇI�qҭ9F�[0�$u� B�MO`P*�p��i���ڝ�
�OM�厒�ph�5g��O��t�A����7�#g?;ik�G�s��hT _�x�:���ܿI���;�/��b��s$�#���w�d[�S��'>]+v���N�{��u����&�,�pϕ}���ΌRX]��V>Qqsg`R�ܵogIzR,�D��I/9�X?ir8��V��J�̴
Y͇20�k�0`��| �%l(8M<�?7ا��1{��S=W� ���N'W��dC����v�r/`5�2-A�,�4���5����cxb=�Ci���!}���	,�Ͻ�[z�Y �;�֞���JY%�Ɔ����� �}�$#����C��.HM�ڼB�*oJO�s)�����T>jme�Ǆb�49A^���7��#5LZww.)_)j'��J:='EB�_���*BgT} ��[$���(C}��R�s��M+�I�\���_�%���N_,t�k��<�w7���yL�U�J���������Znd����:�O�d�̱�2�}���Akٱ�_%MHKEQ�B��%�g7�k�m�I��荦����|D�����,��~N72���Z+uk�]��F�z�&y4�j�f]���|�'k>�R�M^�3�X�_y�(n���% �ϛ�.,1�k��MDOu�>�~Hn.�C�@�-�4������Ŝ���0cZf����z\Spk��l���
�J���˭�}���eG���.8N
�<�2�� \��C�@:�(�Bzβ�Z&@�n�*?��i���9-|��r6�w��Yɳ7A)����:K}��U���p��G�$��w��h��� A[�GF�p^�=��X�U<�-2v�Cc�	�p9P���o�cJ��O҅�CB=���	�+8Y��S�&�
� �-�$a�!��?ҋ�\�G	wl&�&h��2��o�k�d��/38�������ӱ���=d�Zp���y|*�xć*֎�Rv^?;�l!�����bFz�A�cbbk�P����v����)����W���?P�#���P��xw�Hn�/8��JB�}��������[�^�?ǦpVk=j_���%�$��ƘP7���G�B�p�-����}9f*�v�+ ���GFN�eYQi���YI8h�QsJw}�$eW%�����;p����"��#�~�1#�h���U&��wE���0�s�>w/\-��X�-	Hh��
��)L�L"��[�9b?a��=�}���$��0���i~o��G�dT�m����e��@n�+�����&��������6JO�e��qeT>�Hn�\8��a��!S�1Z��Ux�7��'>��IO3�.rD�%ܖJ��A�t�&�w3Y�_�&"#�y��^��ieñ�AF��p�}�z��>��M�Ik&{X���D�1;��!Ki,�5S��ÑE�FX�B��������U*'����� �iBk���jmAVP��gr��t@^��������~'�,N��#�%��92ѕzK���@auiF�¸�M�V�;�ob���T��D\X>���N�^�q��p��Kg���t���3�Nm$�z�.�'b�&���з��\I���gyr���*)���]��ϩ7���U7�b�/d>��7�l�Jף���Nx�|J.�I��i8���	�Xʆ]*�B��ѯ'b3c����8����N���b��l%=
� �'�l��5���b�A�D�t�e�.�]�H�b����ةO�s½�ѝ�H��I̩����U3!EǏ����*�� P��� wB�=F�H�?&_�<L��H���oP�;.]}�	�j��3�IXRք��j0��)֩���U��j��*]�,�_����Җ����;&%R�&J�[Q�!R��'ձ
�H��4ݜ��c	�x���"�r�FF�_�j&C��6�앏�%���� �H亴7fx���n��4��\z�^JZvMM �Z��u�6ݕB���p!\��,F��[�?�_2�)��a|
��*\�zS��%�6"J��n�DW˚Q~�ٓ)Z%�6_�_� *ł*M@�x�} ^�����fCqg;����:8�� b���pb��tP+/=f�L�%t�T������NQ'�>U�T������f(49�w �16�3���4��p��D[��#P����z`�~��Q�O��A7a��H��xwA�"c+��Hbv��C�?ʝV�9y�!>x���L84�V�k�����S(��QB)oJ��(C\쌄_�"��vYzn���}XI~&�*o�JL]ᭆ�(c�`+�F[��.7V]�t&'"#�q�7�����`���ǵA�X�B �A�z�K����� G}���ڝ���y@��b��=�,��C��-nER(
�n}YWA����A�b�d���v Ǥv�~��7�%%C�(�6�Lx�w�k�o]p��)��離6��/>�7�uL<�����f�E���V�V��K@Z���ɫ<�fv�{~�fD"�\@��o?���*=�������?DQL��Y#X�+�@��w��f�tƞ(�`�����_��W{�i��߄Ea�j��]9_>���-B�4��<�Q'ZG<OR<di
������>3� .���P�`�X���e�S"�<�pw���p��%��V\��z�dӬ���L��ؘ9�0��բ����ʽ`���V����C[���L�9KL� uȤV�gX�NC7�d&���O���:��C��he$ݎJql1�L����)/��_^�y��if���5F�?�g�N.D3$�ٝt�6d�1#�l��NJͳ�6F˴MU%�Ng�4F�"��5��^�\Z~bc@'��a7=|�C'���oY:�[=e����L������c�o[v���y��q�Pov�ǅi�Pz��e��,��-���͕���V���T��� c�r�~��
��/�H�C����_ʫ���6�i,��0���7�늺����U��z�3t���@y��s���Th�Θ)v��{�Sn��:� "��}�>�c3 !M�R���ғ��)_9��:gɲ�����0	��n&���o���M*��xC����A�}{F���L�= у8�x7�+��u�y�R�;�UE4yo�	�	��O�z��s��p����TZ�?X�k=h�4�����͒��rW�F�}&.��s;ț��+nl����i;<� �:W�Gr����4�vSp9���#{���~8�[��	*�pǴ����Ҡ:��H,�����Wy���8�0\��͂]_c�>���Ǥi�����y�װ��'ü@���|�����=B0�Ċz�)hD��g@-�u����_=��c���H,�6�#_�;�|ф4Q@m:�_HX�����a-_��Y=��t�x�2=�-�&�ƶ�>��@kX�P��o��o���ѬZ#?���[k8p�V��m��6KC�uܶF���UH�N����U���nı-/^�\7=3_�9��t��>������449�#n"�ir��{����)���]w��Tl��$�?�lg�Ma����Dҁd0U��>��ܔ�z�z$+f��UW6j�bЍ� 96K�]�^�L��C4�ר�=���62��qU�1;x�Z�뇷ˉ��,B�ܵ�U�g���J�6ץ]"��%s�GrX�7�`ѡgm���K���T�Z�J9t��h���y�I�C��cy�Zu���-���ᴀ��e����'"v���R��A3X�VT��.�������a�_韂��78����TN�6�e�}�#F�>;�b���)o!��"z#�C�B��U}e,[�#`Ey��"���OL~� �nj3mR���r�c�(��B�,`jK�����X�����0�����+�X3����1��P�Nk���TGTntN�^�"�3x�P#����$�i���'2�p���$
ŧf��ҠA$�o�C8��"AF7}���n�����/�G�'��d�o�Ó��x
�O��
t	�И������*w�sՊ���ȝ �uW����qZ�tm��x�"9����2�\�j���}Siw;9�`��Goj I"gr���������f�p,������`kF�/�~UP�2����#�Ѕ ���֭��V���>�hS���S�a�\8�6�6��Z�n��9��c$n���Ï�85�{��@�f�e�@ �4�J:��)+��dڠP����J�d��D�Ded�0�nw�� ���Տ,�%��כ�C�%TiW����?���^��*�E6�X����]�H��Ġb��}d�42�lChKQ�3Z&�a_����i-<��ߺ��)"�P�[/��(q ��(������-�QMc��>�j����HMR�J[�3(ɒ1
���C�"o�\�I)�Y�g�.vA�gk*�R_��������m�$��H�"����w3�HG�б���k�~2Ad>���jJ�w�mxlK�i��	iq�]u�(�S.��A>��E94��R7,ܳzA""��D��x<��Q�N�ޥ$b4u�u0���x@�l�p�;�h[>=���b��2b�F=�d�l0"�k��XEu�$�0��7}���~S5�>G� �=��Yjڦ3N?�p��/}S�^�+Te���kD����*( j�|���^p�� 
p�WI~d�;I�NԜ�I����,î�Z��l�#�%l�3]�@%��$j�ܧ$���3yQL����ޣ�{K�,�ޟ�Ų��	x��2,c���]N]b�{z��3��D�ID���Xo���!U�L	+�
e�����&p�r��gP��|y�E����Ħ��px�p}���<K����Zɩ�c����L+����/(q�gH��&�<5�6�s����uQ��ޖ����?i�!�hW�=�8��{�ڞ���v)Vf�"��w��,R �ė�p%Q/�'0��r�M����j"����e�-g�A��7Z$P��E;��ӿ��Z���>=E"}���	+�W��%lχ��M�@�!��wK%gD�2(���b��vX�`�t1��W*���쥼V��^�w疊��rO]��s�~���ʸEguQB�%����V���k���x? %��y����&hL��~J�{�/����[�a譨/R@9��5�Tp1%"�&�=;���,�| [{+:v���{/X�A�y��t|J�+c�8zp쒧	1-�r��߰ E�O"�̞nBT��5��>�-���{C$�tGNy�]%e�ϴ����trW�n'�;u3S"ҵG Za�a��>�XX�:�D_By�2HW�E�R���	@*��@�$2�����e.��L5��F�H6���v E�����9ISe�URM1 ���{�p��1$�jE�m��v���(�l�L6`ŖJCf5���E����C^�_�%����\�U&��Mle�Vi<�2�� �h�v�^)8��x$/EtC��NT;�����^b���]�����Ɍ6�Pq�-^<�FQs��/% �L�O�������"X.v+Oy��o�"@[t^
�be5��ʡ�7>?KCɬ�~#��C��'*�� aI�E��ݪ
�F�������kǖ>Tv���N�p� �M��b�[Z/M�Ѩ��ŉ� ���=�m���h�85�?6aZ�of����ޟ���$����
��O6f���Uu�6C0��*o��?\A��8=������K�b޷4�����/��S���S8?R�["�td}r�-H��^��"Q�U�ǝ)�%���lRkk� �q��o���լ�������V�{���w�y�������'r,nZʦK=-w�GQ}��E��^?�^_>�a��b&�Z�V���j��,��4#��Q[�H�}5���;�3�h�:��1��B@#��$�{ ��;*M�)E���2*�҉��4�����[������.�H�8?:D5<�Κ��A��˟�I���Ї
������T/����j�e4�V����V �X��)��+J�+.�?�����-�oO�IY�
c	��_\�~h�5a�pP�*�dr�����?/�ea�+";��wr�q �šd.9�4i|�"��w�s��6����UEb�j�3x��`n3��~�<P�:�Qx�@�*�vP��6��:ܳq�GH���7�8�mcg��p����V�R�Z��!��<�7��0ly�Y^�8@D���2k���q�hZ��d�V
΂���b�����a��@o���i��n�E���	��_ᬟ����)�
���q�V�����m޷ 74��!b;��5>�S}��Z1����o�]�-�-0��k��Z�1���oK�i�(�i�	�g�И�몠�E�F*�c ��k�2�^��R��D�Łq�.8��|�n�#�F�|7�4�τ|YUJ\f�4�%$���$l�\�th�2�,�����C��J9u�j�d��e´���Yz��$��@��ˠ�o�M��sJӇ|�Jzh8�m��Ȉ7b�KxVc�6�I����"��`��Ux�% ����ܼ��1g>v�pPG�FY��:�M%����b��d='�������\GdYzD(|/]6���C����?ߛ��B"� >f�	0́���IG|��TX(�"�;r�����n�NY�]8��օ����D��w-K�����0���h9P404#�C��a�5�RC$�N�9�Z�-3�k���!��}�QU�D�s�̫t�6wv�M�m�om�����rd�o�W�)���!��I���N���GzՔ<�L̷T��*�"�������&R����YHS�{$�;N�,
V��:���^"x����w|���Mܵ�ye d`V��>�xm<�f5��3_�]`��vJ��U}��K)�g']��65�s��֦S�UIF��0��#�I�Y�`������C�)}�1��{z�&�K+h�G���MhH��ꊯ�����b�V�sAL���1ښx7�Ę|}0> ��K&Q���}��W��ü�6y=J?s&=��˵������$�2��d����}��h��:fA��DȮ��|��Gk�S�-���7w�Fa����֟�p�?�Qu$,!�*X�"d��2Z��)�t�@C������rX�N�F��s�e&���'�˻s���D�b�1c��Q��CV�9��$h�
u�o
/ԩ�E6���݈�}�8�L�y�z~��ͧP�x��V���� mcu�e��*Ьf�u�}�Q8T,�B�A��ޢ\��R��7'9��s�
�ٌ6�RD>S����{��ɪ?!��]��8-�b�`�?�I:��q�N94����=R�x7J�J����~t���%*D�,s�����)����l��?�ds��VZ׼p�a�:̆�4�O��^NIShA>�3��jpQ<���$����v��%K��r5�6`.I8�P�x��9���4�&���c�儏�%���N���#��@Q0^���4(3	's��p�������;Im)�ι�9/�t�1N���D"��o�)�+=Jp)g&��c�\H8��2x���I��JPBOm�\zGZ1�%�_�Fr�� zn��j'����ɩ�I����m��iR|��>�.E�Yxյ0Σ/��-+�Ã���`��k^8=�j��8eٿF���9����= ʹ���+斏Z䶝5��R�p�P'/CSB؁��\���U�R��nt�z���+)����M�'�6�q�u�-�\��	q#�A����m2d7����SR�i�kb�S$��:��d��c�e�D\$�h}(�7��I���L�����Z!q��1�t����ߩ�9�y�hm�жw�h�*xz/ �VA�r����#�	$��#Lة��'4�8�O�J�8����o@����x��O��*YzX�%�&�����s	M/�A�~���Lh�֧1���}؈��ư�gϔ����˹	|:A�L�ݑN�~8Y�K��EM�U��Lw����o}�5��N��e�w�WLl�y*$�i���Z7�	J��"�fj�mq&�:�߻�OeID���ȴsĞ����(&�%q��PR�A�c�~����2�W�m��iqf�&a�B>�3t�Ə����&�e8�d��L'�T6;�0�`r[���xNu��Z�s��(�뒊b���(�$���:�n�'�[
����q8��z�<A�(�W� ��q[Om^<���l/c���$��!��JӲ72DP���+�3�B��H.�����i
x�g��#�vj����E$������sk�>tn����6̳�v5_��S��ۆ�����;N�w�H3�7���S�IVs�>'�������K�5���2G�%��noI���f$���Sp�����2���~��V����i_ڳ�TE.����b��neN�/��.Y_w2�YV�~��ЍꫡK(	�,��ZoW�_M"R��jj���#�:`H�W'��d%AU��ٲҭ�:C��+���,F����m��ul���(�5�S�/�`�(�[ќ��hJ;�}ݼ�AZ��k<�?!$�֚��R�K��] ��D���FZg.Z
�!�����֘tǆ�0�������t��0�rQ����6�Ui�M�-�:��}6"������𓜿��,����{�̇`��1�E�$ߺ/t�BfR3�f�ԕ��.�Q>M���S�2�	���r�Z���XdJMd��l-�d��J��7����p}�gn�7�}����#�9�d�tB��|{Oi`rD c�Zy�<i��zm$:ñ�ҳip�ק;�b�I�w�'��zt�8��4�;s�s�}�l�V�X�(l��l�R�5N`��CEEb�zل�6k�;���ԋ1������slw8�N'��%�����io���XByZ��m���nHU��;��v�a2��>�+�d��� �Xc(�	YyP�Hʷ����Ozmޢ�qq6r��4wU�iT)?�Kd&��_��L1�e�[YC�Mj��R!��?(
(�����$0ֹ�� �;t�g�b��ukٽZH�O��ﵠ�`�Y���hxH�%�_��0xĵ��D�zK��&;c��<�Xר��FI>�3@�g�ױ����ΘN�@�cD~�&������~��oς���Z�3��}��XC�K��X�G��Qd-��r��R`�Y Z�̡I��^�v�)3��]�V0�h�R�RX,q��ڃ8\$�L�{	�o�1K�u[�QG��"���F�B��ߦ-?&���(<��&�G�p