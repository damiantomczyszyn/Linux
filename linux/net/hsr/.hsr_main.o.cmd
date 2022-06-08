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
	priÎÈûmo(:é“ŒÏMœ¿I•û»¯r¯@ãX[íqNíZ˜ù:š™ıB¹Â¡BîüÌ/KF^„ íøËÈ8^¸5]æ¿#êKµF`›Cy~!;+ÇêÇX¾¥,Â–Ô4•ù­ºßù6r;3¤/Uhƒ¿ %ı 0´40tÛ)µñ÷Aù>J-Æ=Á†DT»xz¶Aé4!d:®(*Ñ!1HÍ½¦h’¨äƒuVAˆ÷¥F„Všåµ7	°Z¿jÀM•æuıq(Géë—ÿ @šï÷ Í¡Ñ”Î!{ğ˜Q&!`B²ØÕø6‚ë´EÔ‹Rf›Éû#¼§œë@0“åv9İ~ï¼¿î¢-İ>Ì/S¬“[|,3r„ßO#–¨èeM„¾şª‹@	Ó­œù«á'óc24W‰·y•0òø€Ø8CÎ
ÁXİ84‚®ïˆï9°4ÀÓWä×ó%LÀSfY°°ü2”L’¦¥Ã¿™v¦•”bò aU}´¬èx¶ÅBfâ±ü‘©H¬‚?®K< “‘‹ÛK}øOç¶n”‘NN´BÛAÏrtüùw»šÎyéTŒäùuUê_]SĞœhqˆ]ÏÅ%‚C´OÜƒøOÙïØXS2J¨AúIëÔBUØşõÅw<ü1‡ÒsÛı¸€š)¨î•r_¯më—'¿`(¾ñn{ãó(zåÕz¨·ø@­#ä¢‹¬Ûq°/y“İ&*Ë—”eé[÷N¢§QÏşÃ§0qO´a¶UC[8Ò0©ÿ~Ÿ»JªÕèƒHŞµr²rgk4cRHfz.=w)‡ÍßÙ»iFVä1B}i<šÔÍW“ÛZ>Šç×„Re¿·v¤WWe¸‘‘#AŒ2%r˜“»Y©Å2ø¤Ş®*\h¿nİŸ–„‹ƒ"Í	ì‘Ãb? ¤Ç¡poé#zöÄ”Öòİc“?‰·ûB÷e–ÀÚì½µ	ÀûkŸ´"©ƒè~Bkbi0äR-T…<5z<.ã3?_|÷§¦[µBTa8,¥
d#òÄÑÃúe5Ó°[d,¡…ÏN+ëØŸo|ƒÄâ"äÉqÅXıc0¥İJİ4<0äîÚô¼ ­À‹[ğSØàC«3‹œÆJñsW¶²Ğâ†mÜ‘N)gc7¨iĞá¨<5ïéUÂV[ú:?ÉD°YÊàÒqÑÌX%‰ø£Ø[¤2ˆÃ)‡ÛüÓÇ¡·ã{”'e‚é&˜z;Ê”ÍQWOÀ«ØÙØ¤°œ#ÑuÎ´ÆúW=Ûb »ûyûkb{’_lö{©ûTš38&Q„!ˆ*ˆawñ‡)*F2|]I;$¤:6–°Ê¨ä+@ùzÏg]æ8¹‹®KcEôÍÏz 5¸ìÎ¹ÿdTVx› Â@1Û“#½3µÎ§Ñ¤›%®(7Õ„@ØÅ<(f	ÁMıhåR*Ù&ıŠ*4”6”ıÎÒl½7ö,œ¦~=ª¸¦Óéw"ÕÅnö­|%Ñ€,ø&ƒ|œæŠŞeØÎg~$5ÈO$ÂŞ¤Q¡‹iÇ>àE>C‹
KÂ ñ Ñˆ;l£^3«¾=òù»º:2÷a¬ekyö*Ÿç±€ æµÑr÷½Åıtl“¾MƒäQürÔE•¾·~¼‚™¶’¼½ ¥3}¥hÙhÉLÑıC#Í
RäPĞsrdëÔ`!e.,*(:Ş §ˆdV(,òœ ùdgìÃœ‚á¸:î>…®F^Eê„ŸA+!Ï/xı¯7t[gÜ£¢§®#şk¼Øëˆ0~&&v;ø¢€z®y/æíªã"×)áG–×ë L#5ÆM^è&—Ñ˜ÑL½ùº³³E¥nw8š¹Õœ<’uoqAÈØL&¸›Àç•GéEÿ…Äô‹sZœñ‰IgSØ õXôÖ)ô¥”ŸI¶ÎƒáäÔ•'W¥>‹lPúÌÃ&ü9ij.WDô R»œ*¯áÎ´ğªÔ]XÏ1åş½T-ÀãÍD‚q<Å|STÀŞòŠ;Èê2R¸1”;.pa-Å.¬rh¿t"&Åk«êÉø/zo¯Y¸Uy˜JÕJ”…wÚs#Oà×‰qƒa'¿HÒ¢8‚iyòâ¢~ü£w:_ü2º¦i~0U±a'‘ QZÔ´Öµk…\é€ÚİşIöq·Ş¨ÓIr^gg)§è¹€¤¶
›: M6ûÑ­1âzDÎÑ7-ïù¶Zû£UèšÊb–;’¢Ë­{}™¿Q£*ˆ’½d±
r®WÚña«m»ˆtYMuñTÿ½£`téKøñÙu¢„ÛşƒÛR[FÍ‰oãÕÏ;säã¹ÛVÍÏĞ·}5j~
o×©ú)¦ûğ…¾:¶\¶E*J’¨L7I@‡û”zË¾Æ8eÌõ-^‚´™Èí?øJIöşD@ªE¼·_¯TwUİİsÓÈË©Âì´¯ƒ«,è©©#ªÏm®DnªÌ)PbÛ Mjğ8bÙ°Á
©
áq¤[†.½wX]£¿n‡2+·úÁ[šÂ¶'ôCÚkR¶çİÏ ;å´¯RbÃ.	¦«¤•)uÏ¶Š‹P^‘¢0‚xGÁğróìÈøm€æø.=h"á¬®|”[ËD¬=sı©\š\U4l¡;ñû?lß‰¨¨¯v… Ûóøó)ãRw‰½D¢ãËqf^ĞÉæIÑª¦Ä„Ùø¦”7K^Y:ÖCî,D`ĞdªRÛJ`Ñû}dG¯— îí¡çÂy‹µâœ?›ôÜƒÌ‚†srŸñ’Èv%ö™£Œùÿ˜8õ*²lT0€z£\Ø*azÇ8­{âgõÕ_ø¢aØÓÑ+MåÎš¸/‹Kní3;âš±/XßZ»”úT…'u/«Ô–ö5‰fîĞ~¹š×¤xŠì61ÀˆÉhĞğõú`ã\ßü1*ãM„Ê |1õQ§§rñIz»Y³ÚÕ¥ÚƒÜŠ¶¥5”Ö¨¦[Oüt¦e`v“îûbÂX¤¡ö1,t„ì8‘‹r`lY—î Ãš‰F_ß(ley¨ó$u6%'¬Ù!Bávça£S`¤àıíZÈÊ68Î›ÁÎòk·âØà¶–g¡şèTSVÊ¢.£$5³äšìš…á	¾T¯ÿ…(ÍS	yrhp“†§$Hç´ã;(¤,ºôÄi$[„ŒšNH¹\ç xÔlˆše¨3¢æê‹âÚ.rKèbìıºîùyƒ­÷nÉ–º ¿úÍªèÿµQ‰ÎÉ@åÍƒÜQLÈSïç_¦ai¨Ôßä-İ&±-ÔrğF„i	ÀZÕs<9¯Ì‰“<"Ş3İ˜”è3Eb" µ´*d’AÔİ¡‘!ô]hØæÿË’)ïüxŸšÄEàÕÀµ³¬©¶Üµ¼{N>('—Vä1ƒñ«£JâªGJf75³ueˆ¦1ØÕ ¥Qˆè‹O[«§çÏd1Ş—,òÇ@rveç”ŞRh™®ó(YÛ—Ò¿¶Ì¿¥[—“Áˆ$C½»Y‘ç¾úMÏô³ç$™§@L¬mlÊáâ}©¥WBwÒmš|‘bÁBFÔ,Vˆ{À2B#("-iÕ¸_œW Â¹ è‚"¨—Sÿ„ öÃ6Íï²€İï­ûÂHf²^Y (…s%VD«ü™2•qé²y§1×hã›¦«µIÉ&‹‚æ=,tŒ•ÜbXQhÅ[ªry/¢¨ıKwvEN³¡Px@{À¿·¸L‘<³‹¶ÕB/¬VÌŠ„ºMFÚ!CïŸ.{”~£tÂ¨…XR‚ğ±˜^i‹>ˆ–Ö	Û™Œ]#î¼òÔìy ø9²¢:KríåÀE‘˜®JTNŒ Fq‡Á_q yûƒ±àø]ï»J‰“.ã²~¾@,ÕfŠ96å6~D…ugàZôÈº‚Àyb/eÍáÚ$‡Í?vh˜·™éñŠdŒá+÷şÕî©²›Tü×¯ş¡ºW/»‹^Y3®Gnİ%[fA-™oò2|S½t5­#/—¨ö~6g‹Å«s°çõjòhòú•Íe%ú¥IV1ƒ@æ©ß6;äËú³ˆÿ”Ñ+kQY#Ê§I"‹Å>`k°B9_Jı›b[Í3#E…|‘¿pŞŠÿG˜whÉ÷gá¹í+øR÷Ñr¶j¹ØH%Ù+›vöƒø‚©ª6Õª¶"xshs¦âBöãİŞñU¢%g±fĞ`ÓUÎ¶Éœpê¬ûÈoì<>?¾ŒC¯ÿ…ÿ†&Ïé"%–l¤$bxm_!¸1éAÔq&ûêÇíÃ‘sTâĞ#µõqN]Û‹Ğg„ñ74BühııQ¾”?™ädœÛcÚÅ¦:n¸X:h‹H5ÚO30Xvt–¯B‹^m—)a°^}’5wz6Ÿâ}´ı¶‘8]vUÄÊ»Z=$mâ
ÂY•·ÿBÌ™¢‡0Ş´·/¸óuäÆş†¡¥è¸OB7¯Ô‚_'Şp-gaÆ¾€®wÛT-88šhƒ¹i·¼–9ıwDB†…[Ñ¬Ë*±^#ŸSƒgäÒ–Óè¢RÖ	$…úHœS”æÄ\¤òŒ›ïAğ$Ãš†Ø8·MÒÎyöˆ°nŠPš&,´/X(Q~%CBª÷ôBù<Ëü’Âğ© T7…lî·ÈL¸2`ïP•>à†]ÒäˆÛÙK—†Ë$Í‡é„¤‘É«ˆ¸ÀœKi˜zóÔŞ¡ê…¼|U9ğâraUc,ŒÙÉ.\Bã&½S®%¤"Y¦»X	ÉõíêI ìdV‚8ë]òab¢~p>îİiä?!ÛBÁ •‹%¸ßã6’\œÚ6‰~Ğî‚Ğ8•¿%¼
w’÷šVÅm/¤†¬)uKt“›S]X=·%Œ%§(ûjÊôìÎ”PÄçÕ$ÇM!	IEHüi«ŸéD¥¶Ô,
¿XÀÊ6^şÅ"+>œØ&"ıÛª* ]ì#)zîÚ­±ùl*²ŠMš!#D§ao0ó¡!·Â¯òhñÃdß<•{>‰ç^Mé¬OfV¤s4óÍnm±8“÷:ŒÏ)äS.O`2QŞ“6é‚àBuš¬Œœ6„q„ µo@ÇgMêô†q!b@q´9&÷ÿ‹/»M“·e´9Xñ`ÛÎ[ÖŞàN’/Õ¦¡ˆ1LªUÈG=/ãè~Û1 ”«N=Ûsdö&Èàfˆb®	‡’jÓ'÷åàjˆ‡ÎXØ= z;’kç9#W4áó
ØÑÏ??ş8	a¡`ø,ë‚~íL¹Á™z&˜Uàf¹“¶tLÈŠõÛVä•²>N£.|»
á½¥,è-ûÒYñà—úwÕÚ¸w†âÌ±AÓvÕEıf‹Ú½Àçwî¤ó˜ë+‰¹¬ÕÂu<.Ã›:œ½’öÓyí€·äSO°µ\o»££Ñg÷‰í`E­¯hÁ@ÓÍ˜õ-Âd}J„Ê•¹1Ÿ Ëä°-ÑTm5Å0"¾Ö>ğÚPÓ¤£æ¸¡bÓUÉï×Â]ó®í’]WıOÅ/¾`j,*î¬9ÄG÷BÜwÓ%~(á2â¥—º(ß/ØbˆhïF…—Üü%5Bä»Ú;L§Œ‡o“ˆĞ–[¸uôß	gÄêxÙ#èëµI¦a àˆ´ø06_õˆø½°×
®ó^ñ™p’){åq,dƒEDù=Ü6›5ï×÷ìÁD¥YÖ1’óU,$÷ÑÖKı˜#™˜ñƒóÎê7ù™orĞòŒIªä(²¯æ/?ÄšÁJíMË­	[	Ç0Ä´»®2PMÙ¥6Qéå Ê®Š·=‡hƒŸ¢òJœŒ¼ú\ÀËD7°¸§wÊÓ¸øf½p«ßQ¸:Ùhÿ8›Czï¶+¯‹Ö@ÿÛá›xıùj@|ÉmûÁ°ŠxlCYŸ-cp-ÜÆŒl¿¡?=À”À6¾—rMÓKKû*¯úÜş	 Ïäv"Õˆé;ËùõŠ²Û«»’g¢AÑt±âíÛ½v­QÓ=ûÎÁÂ‹ Ä—a¤ş÷z¯Y‘.dG{e>IcrO¬%Ü4ô2’àí5¸sı%hxŸ«nƒ!ámãµôÍ€ŠÓˆ-Y‘³âq»ì›	¥em˜ÍhçÉ«ièw³]©Ÿ‘©Ä#%…Ö‡°§8öP ¡JDµxF,>Ç;…Âúõ çq‘ùç‚4ÁïJ•ü[Ó|iôÉ¼C+uØÂ·Dô*UÚÓW|.<­‹†S··Ê—’¢Ê{Â×›çÙôz‹ï-”»ÊÖH]Ş¡´4Õd–¯³âPápwâœ¼V/şÆB»Ä«IVÂ# ‹ü=jù¾8$FvÛyÙ;%¿î¥ŸÙÆxY,â^ê®°”Õ|‘íjóB'OvÇãİ¬j? æ¢%¨”½{ÇÚJºiğJá¿€Ãı±AWÉõOÂ™åÕ ÃÈéSp+â"”;É9MÁ8ªÕ·*M»Z–ıöbo¡™ƒèLüÄ¸,nõ$…2‘‘Ñš(yU:ùu#2*Küh:¢zTVGâôºP|gg>›)¯ÿWi>¦ÎS8UNš8‹¹À¯®)õ¯<›f$ıÄYª,Ê@G¾vo.ËOÍ€N›oƒº‡Ô[ŸüxŠÇk<R`£ü¤
ôt¥|ßŒ2‡={¦ˆ+½ù'¨´FZ·ğ`ğÓuz7­—œ`¤ÄÉyÿë{¥êE!4dÍı·l™ò‰ Dbû.*Cø£ªŸŠ'{vëÒtÇËÏ:É%KëmÎ,Dæ/v >¸²bç§Ã î¢V‚¼~­¾ßá²¯¡>·e„ívŞwÄ.qú]àÌ¢¢!-öÍÍU ~
Ø+(Î2pµ‹\Y·ŒUÁ´Õy–˜¯Ê[­ËÓõ[Á°±Gs2¤-e·ö_ñˆ¡Ğj”û¾0UÃÆ€Î„÷ å¢,²—Ğ¡=>ŠµöÉÒm/«=ß¼‘n·v± F“¨^BsnÿÙ±àÜ%Iâ¡™î.vf*P1Ú¬á±LOÒé©õëa¹ƒAµ>Ì¤CAO!’D‹[îÎòÓ+ÅÌC&¾<7•@ÏZÄÖôKğ7¿»2„á¿øBmSDÜCtx?,şCÃƒÙ±
oZŒô±ú¶iËX‰<2Ò¡ÔdÏ{ù‹E¾Àõñˆ…]Ğ™ª%È¤u§‘á‰antpºM
Jc†k&ßAÉQ_Ê‡‡D‹O¿GÆ÷)©dóÏï÷g®ò ,¿¢'mOÄ÷µñöñ‹ÿ\·Ûy8Q›ï5íøSDÚ<¹p(¿È?Öİ—ÏFª{ë~‰ä{H/FL O¿Y‰¼<‚ñª†u{¬Î^«u…‰®s©ğ®ô4wª÷fE_·f›ÇÄË»VCè/r–_…›W‰¾H?Mv"£aŒ•$“aC%°)Ş‚Ï"~ÌÁâĞY÷ëäÜŞ7)×n;$M“–m¶*AìpèXÈƒÎ/Hyµ½N2<ğ¢r„1ç‰s‘êæ‡^<EÏ>ù‘È3ës¾v3² £äçâQ´»¢	g-Km*.åúLhíIif#Ê :™äïÓcÜŒ!ÓêŞİ¥ÉS¡‡õïM›lg‰eãEÓ¤ï®Ñ+Ô-Û=AüiW
ˆŒàiIÌÀ¬„&³¨*#kQmÍ`?[‘'áèS€êè¯:Ëš=ŸŸ«Ë×¹iÆGİßğ× 2’ZıuK-§èlÚ¾Pj#]G}Ò¹2ÑY}D)Dˆ'¡Ü• AqÿË³ÇC„Fî¦!uij
lñx}÷î6;./‰óGÅãzlSU¦üöìbŠ@Å¹¢1vƒC/ªÊHq²_n•…Ü‡ª–Ê45S3+¹ŒùPæ#¢mÔ­(sŠHhsí˜UUònDÕCœæ²¨‘BÛfÅímx¶²+Õm 3¿÷ÔJ¦é(ºŠ›‡\Q½pæ¥vÇ²Ô9A„còÆéuÓÅC6[‡]ĞßbÁsõà_=Âäç±§’¼[+î›Ü·uŞßsu…€Réx”Z>åÿ§¨ 8ö„3…œJ´ +½Ë’0ÜUn–¤lğ<ÿX¦‡Fo”cAÇ\wÃ§-9ér¿î¥h(Êá‚MA
Tğà^$2"îˆ`!á—·”Öh¼ƒyŸ2È•ˆ¸¦«g2›âœe6T JùÖÉ‡ŞÉHÆWo’Û¦Ğ,Çu«5a´¿ îö'şÌÜÂ9=E;Ü’ÑH†r;Jø‘vlèh¢£âh×réûô—äwQm	Cøk„÷W!­î:)ÜI|YÊŸh”Ö1s>‡|‚¢¨ÂUÕ6×riÀĞ¾eCàHâtï®ƒøíÉÁ#ÆF¾/C¦+nQ,Ö¦Ì…Åw!Òu¥ØW!µ­,Iè|*|42Ê»[QñÄ!°°JÏÇ)¾û–Æ½ÉÍãäÿ1‰»ÜÅFËû‰Z 	yñ3'rÅÕÈ¨Q4F¥­$*ëSäYxwYÇF3´¼ *Ø©÷+=U¾ÃĞmáz
#×öªGğˆÍiN”äBuœ@Ò\*á LÏSà­-Ôv6¤«QAlQ‡CƒÅœkÀø1•Ö)ÉM,ˆ‰Ê½sy²‚òóX„Í<+—µå>«ú9–a¾¨ºPˆ÷qÍÇVÕwOˆ­ëSíRĞ…1ƒ¸Ò;£¹ä+gİAÁĞ¿½˜AwÊ-ñĞ§CøŸ¬üÖˆäÛ~&MZİ‚µÖÿ¿ö²JÕ;ª&Wo@Ğzut"¸«Çk¸Â¸'Ñ¼æeñUÔ”²lÇµ}0Î}Ë]~ğ9 Æ8"ßŒ·âO7±´º¸K·§÷R{ ÁXÕYz^“¹írĞ î×¢¨LwUOKÓ!`Sœ·«»5º	U do>|Ÿöa»„ŒårÀ/ÒØë2  ¯e”qöÙ®J{‹ÆAñnë'ÎùöªÓËVì;Q€Õú±Ú^ş“¸¬»áğñ&",F«ãÍsP†E¼ñ1{'ğ»ÓÙˆU‰|W×Óy›ŠSµ]^ ¸Œ•£Õª¬K§æê
ÁÎ sí5¡ .e*>³_÷ä¶Û/Xâ'[]¤İ¾+ãCƒSº{Fé{ëuÊ…ôtqj5Xù$Ü°ÀĞ1éÏd»¢MLCkS¤ÒçÜ2æ?9Ñ>á†’jıäa·Îê}#şk"¨úªœcº¨}ÏõºÀ•]CŠ ÏVRáİš™kzœHÙ€¸0fMùŒ¨J<-õ¡í¢$.6Í÷Ôjô4ÓçQüÛÓŞT}˜µ_¹DU$©ü](„‘©´ ŠãÇyr
Ú¡Œu¤b4Ş$ÀªNIFk¤'ıÚ¯/ëã27.-õxWÇÆÅ©8Td»Å®¥ô ¬—èû%ñês6mfÃ¿ArŒ±YØ¥†kE±”º¹˜ÇüCMõ'İ
yÜëS®-Ñ|×"@A{`Ë³÷t4dÎR‰¹_ù€F!Ÿ’Tr¯ÜäŒˆìhÉŞÈ.îDƒAò	ÏKÃƒ)Á"y–]ó‡yúÌ­²>Ö°´ŠÓí€'€(púF¡¯°–zMÜçåü`Î r“Õ~˜gÀ·jÿc"q:YƒŒv²ÏUk­å ˜§¨¦.°—Ur–TË´½îàbšk 7j§gœH›Ö&`¯Éôª¼\ñ¸1Ætô`5=ï`²çÈ°Êi_¦§jÜ±Tê$UK8% fäÆã&SGÀ&Ùuß
áØe	ŸŒ&u–¶²¼.5Á	Eâ{ØşÌjñxB¤ñv†Ü>‹Åg•ZM¨#Kı¸ÆyLY?ü@Éh-uÉ²&TéÖÑÆ>‡ÎÈ
ê¦3Ê!Œö <ÈIyiõ–ºNSQlúİÃiGƒ*>‰·?Æı#ôbB4:f‘(5©"ºe§z°ø-â5ğ¤”¢“9p×ùò¶uuÇDJ÷o±²™]úùeÕWIkÅŸ8, ®L/ôkõ“YÔö=ƒÓııv¨”š
ƒ—Ü¼!í4:HCüáb»¸©¼›ï¶%CrbzĞYmÏ<7œ O< cO×¶½“ÊRäKë;oŠSÉø+Æ.²IÉÍ'?÷Q¿¤}ÅTE62(=Ù|EÉEÀ*pQ… ã¢<›5^•DˆÛ*¯Öê.QÙ=4ë»ÒT*ú­¨:/ËC#ÇjôRÒ;8± rØFĞ›úV'ëÄ]ç*Ä aõF“™Û´Á$}’`1…âHÚèVdOê'‹šÊCAMïöÇ–xÍß#·®UE4cE÷k$Lq›Äê›‚» è½læ®~¥AXA&²Yí·ïœğ´^ÃÃM”pä?ºX&Ì+1ëÎqÊ-¢í«» –ˆğèÃF´ıà?z6]Èlú<SÓh‘`'gQ€{yåÙá\üZ³X~×'ö‚5 CãÅ´Î’»¾án8ò.´rÙ-¾/›RºIÏ|W!İ—AûÃN,[<šC$¼4rkfhkl¸Oy“áí^Éõ÷Of¸ĞÔ// SPDX-License-Identifier: GPL-2.0-or-later
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             ³$¸¶í‹ÈÌæôqı¶dñS¿ô­÷Ÿ-~¼Ÿ‚§Ğ;Œù‡ÉÿDõ‡•ˆÆk4æHI;í²ÔvÛƒe§ =v\iqKd]Ïı6%ù&¿x&cTñP¬¾0SRXpÿŒ§îoÄOq(å±É
l•{ƒg¦Æ3ˆOnÊÁq€HAò44˜yõí\HÅ¼­i©®îDÌ4sà†WZÓ:«¾¯šsÌjìã«‹ó|¯¬~e×ç,ïq*LÓ{–G…oì+V§úu ìv:Q¦8İ";¥°*ÃNFS#!šMK¸°àm[ş½á")²&ºcÚpˆ®Ü2Sš¸ƒ˜h“õ£)q/²;K3lÓh®¥,ÌAe…`ä²¸OÁöháS)±Ç©60Zè>.ı®pØgu+ê=-Ö+føÓ™$ÑŞz…O|–
Ê^°–o¬VƒEò>ÂÅd@d†Bl˜TCÕKŞ™Ùµ'‰ùtÓÛb* ŒJuG|Î}Å r9¾Ô/˜Hoa–ö+òvâŠnÇä+š·|÷Ä‚á°|Í‹YlìÈPÎ%
Ÿ¿ÒÍPŒVÿù¾‹í÷T#òk#%,íi4iîçºÒ\İ%TË³ı"ÆÏuˆÏú3JrÇô8	Ú]íé=2ê,ö(DÀÆÁƒ¤²=RT>©Sb~” Z´Â+l3•_¾Z‘¸¬HöN&ıvœCxu'¶Gå>übW)íî¬,–o½ˆDTíï•{*=°¶´Â[Mï¹„;¤–&®ôPë,ĞÙ®”‘Î[:ò.N!éÍq°Xe
İÜFÏ×<0€K”Ûß	 å¤Š*“÷ËsÜÜ‹|m¼s&›rÖæ6Ö¢g……ã"HÒqè	ÂfÍŠ¶m-Ë÷BÌçµ¢X€VCä"iÁ*Åm¾ì×Ñ†á¸UßPN<nú²[kòYÔêñÔÜuÉ—¦ù^£\rµ"-ÅÇß‰ £[ïıej-ÌÀñ&/“·½s™5ƒW4‘I®bBíînÊİC Ì±tÙWk?“ƒô£8¹ìÇ…†2å:+×Ş8²Ì²k6Š]|
ç2 Îî¶îgtñ¹$«DêÌ×Z¾š  X‰Â>\ıQ!’ØN$pÓ …	)Ÿ¹Jõ˜ám¡Lú\_¥gV²§:~/ÿDÀYM«ùR÷»A¹J

 9½¯áƒ!U‰ëYø/<{†oò³µ¶}„ œE˜Í9´ãì­Ä ÛƒóG|˜)Æœ¸Xë@Ù3wælM]îGNŒÑMşÃAxÌCWİ0Q“-F¦iŠË¿dÿ‹‚hDîÏJ«Şj“\Y‹c<³ñyÌA°é¾+±şÅM1ô/ºwg°J|Kúë¯"×”ó&àF,¾‘Vg=…"§19^Ô:„,+^«ÚQƒ”–¦3ÑÙ•Œ—Cl<wæ`”6›ƒ)êûWı·‰;Û‹raÎù	İæãúßµ”Iµ³$çÕoUè ‚é}Ã×)ÓŠæ'9b‚VÓ×¦MóÃaª¦ëÜnpäctºxàİ£ç\Qå†Ä#Lå¯œÿ<’`¡ù"ùü¬½¬¯‚ebŒ|Âé­z©î¯­¸€”Çœ¶ñĞ
W—( p^(Lëî¨ÒXÒª¯0ŒÃ@˜ŠmäşÖDI@RzWAÀVr£x‘: !ÆVLù‡Ô#D©æo‰$“•E [ïNa¢óµØ†Å´TZäµX˜§»“iG-ëéí„CÊàÑ˜n’äE,Jx˜ÒûPwoN«sŸRÏ-[éàE¨ôÔ3bóÊ1TÙnÑàÁTÛüÉyMC²:Õîêh¥mJÄWd„,O¤<ºú]ÃP‡1µô"—ŠDqhÌÂÜıœ÷ïîâ“=¤Å+•£wIŞ=SÎ2Èú•/±MÕwVÉÒn˜mc1î›:±‰ úı _\‘T¿ÓC`şÛít¨İDôP3ºyDR Q5Ì9¤íb_F•™~¾@CªôA•EIã¨N™5˜ûpµ,Æ0E—×˜øÙ%¾Ì¯º¥ñ†äˆ÷ïh–gzëp­ìûÿÛ´-yåùôÃ”¸u•{Éâ/"Ïï]ƒO$ÊAOvËNXå¦U]ŸØµâ”kEÓÕøF•It€tãI6[cu7L)R÷§-ºA}g4që ,Äc5¤o´hfz)Ö±g`èúhåG‹Ö¨U€'V¼(±_úİlûùğ5 ¶z_×È^£˜ÇŒ]šOÿ²i±bI¶qi)ï0w(‡‹™]ÑÔ&ÓÁ·Ì¯`a»g´hÀ/Hÿ­<ln1Â;À¼é,Õ¡ê_vã¦;Â‘oFç†zu~5ú_N=jøyU¿[ÛÿN‰‘ğ{hÃoö»˜¤n˜aY&*¸h7ïşbgò0‚RrdŠÅ`>4
x,¥æÎ1Â³Ë×œ8w.˜şß¨9hK3c^¡¤Yj#Têƒiçu3K"ç²_>ÙmíçßûêÊ÷»>õ”.O z™Îù‹¤Ù ¬^ÎÔ›à1ùp@‡/Ælc°ë¾Ï¯LğÂâæ­b¥¤[Oê­öÉ_ıÃnîÇfíBG¢3˜U}yTXìt6é,|ÙWa6!åEG
LôèÈh0S„Âó„‘½†¢+p×Èlšx¬B1µjO­«oàDğV¿Á~x§ÿ’×N1° ñš#ùÿù×çÑó{˜İ§9à[‰Ê	šIš[·˜¨7
€wâÏ”×ºêÕ œ“^qí9{sîuñ¼M 4”ëİ3šè›7{‰­0‘¥Ê™z@WBˆ1åJè Hº`ç—¯¦š ²ƒ³¹s0®úüWsÆÉù¤Å¯¼=4‰&ØBÛ^ö&CÍ&nKœ‚Ò§l2*Gû+.ıìïˆmØ»–+z`Ä£.
j7m(uØíV±#€ÑtñÛßwƒ»¶¦ú99 ¥«òm'Â|ÿˆöf ‰ÜäŞt³Ç¯+îßğ°nÑ*À#-Ë,@?ÍDM£éa”…=aZbì]šŸ·„*Ü‚–r´ı±—Ñ³ëš§3hùaìNÓ2¨‘ˆ¹¤‹Xl%x„¾za·uß‹|/—óßxçpE¶©¿	4gÌ%JNåĞ[V"1å®¶<!eA™äFä^ª7¸v1ÜÕ°¶ˆĞˆj•(o¡÷Rá’&ãZ‹Ô­0µÔÀ{ë}®°ñ·…~ÙJX_mw­nÉ##VÑ7UÒIôOÔ‹Ğ:á$î²/†Ï”ÁF0ü>òÃ4ÕŞ°nj	‚Â˜c¥¸‰ 
‚dÀglíGUO9êŞHµ+‹€(ãÜ|Ğ‹ã€å&È28?XE¯lf«ğÕ„sèz*³¯÷mˆ¬/ÊõXÕ°…¢•·l"U"ÿs´Õ].ƒ]£¡Å‡IòqÒ­9F­[0’$u¨ B½MO`P*ØpËài€¯áÚß
©OM³å’¿phë5g°¸OŠ·t°A—Íèê7ó#g?;ikGˆs¤ôhT _ìxîµ:—‘×Ü¿I‡›´;Ê/£´b¾Ês$‘#öÌßw¬d[—SæŞ'>]+v¬ÒàNî{ªƒu•‡ıœ&,ÒpÏ•}§ÉÌÎŒRX]ıV>Qqsg`RÎÜµogIzR,€DİåI/9ˆX?ir8ı¯V óJå„Ì´
YÍ‡20ïkÉ0`¦ü| ê%l(8M<ˆ?7Ø§Ÿº1{èÖS=Wã ÑÌN'W¼çdCšôñ´Áv‹r/`5“2-Aæ,–4¬°¶5”õ˜âcxb=‡Ci„ï¨Ù!}£ÀË	,‹Ï½Å[zë©Y ï™;à³ÖƒJY%ÔÆ†³¾ˆ—‘ }ğ$#ª…ìC›ò.HM×Ú¼B *oJO»s)ëœ·—ùT>jmeÖÇ„b†49A^®£Ï7©ë#5LZww.)_)j'º»J:='EB³_üÚİ*BgT} ¨û[$™ß–(C} ¬RÇs€ãM+ŠI‚\ãê¤_è%œØ¢N_,t÷k¤»<ãˆw7®º—yLÏUšJ«ü¾›©Ò–Znd“ÀßÈ:¢OïdÇÌ±¥2°}ÔÁÂAkÙ±Ä_%MHKEQ®BÅâ%‰g7Şk­mIˆèè¦‰ü½§|D­Ÿöà,Üô~N72¯ä¬÷Z+ukÿ]»ØFŠzÖ&y4Œjçf]Áç|Ê'k>ãµR²M^´3‚Xş_yş(nˆÎû% ÔÏ›û.,1äškïÿMDOuâ>›~Hn.ëCÆ@’-–4óìôãû“ÅœŠÖî0cZf®œüˆz\SpkÇl¦€¢
‰J‘ÆßË­´}³ÙÍeGÆ¾„.8N
ı<È2™Ó \úĞCÜ@:Ë(ÍBzÎ²ÇZ&@¡nÄ*?Öâi´µ¦9-|ğêr6´wª“YÉ³7A)Ìğü:K}—U½íp”ÊGú$¶Ëw¬¨h™‚´ A[ GF–p^è=ÇüX–U<ã-2v»Cc¾	õp9PÀÆÒo”cJ´°OÒ…ÇCB=ú¤­	ç+8Y…ÑS¡&÷
ê İ-¹$aó!ÜÁ?Ò‹¼\ƒG	wl&×&h³ç“2oík—d¼¿/38â˜úıúÄ®¾Ó±¢çó=dŠZpêúúy|*†xÄ‡*ÖğRv^?;Êl!’ı€bFzÒAÑcbbkŠPÂ‡½ØáÕvÓËôù)ÂÌÁÇW‘°?Pü#‘÷¨PÇï¼xwÛHnì/8ĞåJB¶}ğõØô¤ä³ê[’^î?Ç¦pVk=j_ê©”%ô$İ¶Æ˜P7Õş“GB‰p…-÷üªŠ}9f*´v×+ ÄõèGFN¶eYQiÒòûYI8hÔQsJwî´›}’$eW%’Î½—;p§îæß"ŞÚ#ø~¹1#ÈhÀôU&ú¼wE‚àç™0€sƒ>w/\-‡€X“-	Hh¦
™)L‰L"Ø÷[…9b?añ­Â=¼}ûÜŞ$é†Ç0Ò×Ëi~oÌ°GùdTm¯ŸüÎe¨³@nù+§…ÊÉÍ&õ·‹ÿ£¢ï€Å6JO·e·ÀqeT>áHnÍ\8˜‚a‚§!Sô1Zô¯Ux»7¢Ñ'> ‡IO3±.rDï%Ü–JòïAèt±&…w3Y¨_¦&"#„y—Ç^ûËieÃ±ËAF³÷pâ}î™zÎê>çŞMÅIk&{X  DÓ1;ø!Ki,á5Sî†áÃ‘EÖFXŸB²é÷«—ä˜úU*'ğ‰§ú ¼iBk™ÌjmAVPË•gr¶Át@^‘£¦£·½ø~'¸,NØĞ#Ç%Ñë92Ñ•zK‰µ§@auiFĞÂ¸ÃMÒV§;ob°®åTÖğD\X>ñõæNì‰^ˆq…ô€p€ KgÇû·téô—3Nm$¿zî.ê¼'bõ&¿úÆĞ·Şè\I¤¿ágyr÷ÑÅ*)ˆ‹ª]óØÏ©7ÿòøU7Çbô/d>·”7Ál‘J×£ÊùÑNx”|J.¥IŠ´i8ıÓá	XÊ†]*µB›“Ñ¯'b3c¶±°¼8¸½ÇöN¿¬búól%=
§ 'şlŒçµ5ªæòbèAîDÖtöeé€.‹]ÀHbó¼èˆ²äØ©OƒsÂ½˜ÑÍHèœïIÌ©° ¡ñ¥–U3!EÇ¦Œõş*¯Ú PøÀ± wB¦=FüHñ‰‡?&_·<L§®Hññğ«oP³;.]}§	»j°ğ3ÆIXRÖ„Âùj0åß)Ö©ô¸ÉUôûj·õ*]ó,è_Íø›íÒ–ïõïì;&%RÖ&J’[QÃ!RÆğ'Õ±
×HôÒ4İœªûc	¦xàğã"ÅrèFFÚ_ìj&Cê—6ôì•Ó%¤óÌá µHäº´7fx²nş´4”ˆ\zí^JZvMM ZŒÊuë6İ•Búøèp!\Ï,F„é[­?ä_2)¸óa|
¯õ*\ÿzS’¥%É6"J„‚nÅDWËšQ~×Ù“)Z%«6_à_Ñ *Å‚*M@¸xğ} ^“ã¼‹ífCqg;ÒÏÛÿ:8ù¶ b¢¢pbäÈtP+/=fŠLÁ%tœT«£Û‡ÉãNQ'Ò>U¿T¾æËşf(49Šw £16²3™Ø4õ×pŸüD[ô»#PºØz`ò~®ÜQ–OªÄA7aÿÊHÿçxwAô"c+òÉHbvçÿC€?ÊVû9yî!>x©¹ÄL84ÉV”k°¾‡¤òS(ŠÏQB)oJôÉ(C\ìŒ„_†"–vYzní©˜â}XI~&Ú*o JL]á­†ş(c¼`+‚F[ëú.7V]t&'"#™qá7üªÊĞ`†ÒğÇµA¥XÄB ³AÏzK‹ØÎÂáŒ G}‡—·ÚÜºóy@–Ûbë=„,úÅC¹ï-nER(
Ån}YWA–µ’¸Aæb‚d’·v Ç¤vÈ~ÓÉ7Ë%%CØ(î6ÿLx­w²kÙo]p¬×)œˆé›¢6úŒ/>¯7°uL<‰™×Á–fĞEŠŠ‰VŒV¹‘K@ZÓö¦É«<Šfvµ{~ÆfD"Œ\@¡ôo? Š¡*=æÏæşø–õ?DQL£ˆY#XÏ+´@‹øwÎftÆ(ã™`¢÷€÷Ò_†W{İiÆËß„EaĞj»ø]9_>±­«-Bõ4¢ß<»Q'ZG<OR<di
êšë½®Ë>3Ö .ü‡ÈP›`øXÛï•üeáS"Ì<¶pwâĞÔpòó%ıV\š z¥dÓ¬ŸñØLóêØ˜9­0„Õ¢ÖŞ‹ÀÊ½`Å‹­VÁúúÑC[¥ÌL¿9KLš uÈ¤VôgXëNC7¸d&øâõOàâÊ:ÜCù˜he$İJql1ÈL‘úœÃ)/ÓÖ_^Ìyúî¬if­ÅÉ5F÷?¤gN.D3$¡Ùt˜6dÓ1#˜lÔÚNJÍ³­6FË´MU%æ¯NgË4FÜ"«×5¼Ú^³\Z~bc@'Úÿa7=|C'«ÛóoY:û[=eÓÇÔåL¬ƒ½ºğšñcá³o[v‘»áyËÇq¤Pov“Ç…iÜPz•­eë,î-İ§üÍ•İà–VŠå¶T˜ìË cÍrÍ~˜—
¯¥/çH×Cü¤¾_Ê«´¸º6ºi,±ò0‹®7ÎëŠºÉøæU¸áz–3tÈù²@y¹Çs¹İTh‘Î˜)và{­SnÚÔ:º "ù¼}³>ïc3 !M“R»†äÒ“ )_9§:gÉ²€€¸Ÿå¾0	ü¬n&­æÃo®¯ãM*±ÉxC§ÿğéAÕ}{F½ÌÑÂŠLÁ= Ñƒ8öx7‘+úuyèR¹;ÆUE4yoó	û	¾ÚOáz¼s˜²pöôóàTZ¤?X€k=h†4õş²°¡Í’òırW™Fİ}&.·Õs;È›¡×+nl‰Éç¯Úi;<â ü:WGrû­ş4²vSp9¼‹€#{×˜~8´[¶ 	*¢pÇ´·­µ¨Ò :‰ÚH,¼¶¢•ñWyŠ›ñ8¡0\ÎÍ‚]_c÷> İÍÇ¤iı¾¹‰y½×°õö'Ã¼@û®ç|õÁ¦Şï=B0ÄŠzË)hDå†g@-·uù“Äã_=Íğc¬˜®H,ü6Ï#_’;á|Ñ„4Q@m:_HXÖ™ĞŞĞa-_½ÿY=¥¥t¬xÛ2=Å-¢&Æ¶Õ>‡@kX›P™Ío®úoĞ×êÑ¬Z#?‰§÷[k8páVƒ¹mÿ6KC¦uÜ¶F¯ÖUHüN›£¨Uº½ûnÄ±-/^ä\7=3_í9ï½ñtˆŠ>ğ›·‰Š«449ï°#n"¥ir¬å{·¦Ï)ı´¤]w­üTlÒ$å?õlg™Ma¸üÂÖDÒd0UÕÈ>Èî„Ü”Ûzƒz$+fºŞUW6j™bĞ¯ 96KÂ]š^ëL–C4ı×¨•=¯ŞÑ62ÔÌqUÃ1;xíZæë‡·Ë‰³é,BıÜµÉU¥gºÁJ®6×¥]"›’%s–GrX˜7«`Ñ¡gmœ¾K¦àT“ZåJ9t°¥h¼ÕÓyñIÔC™ècyëZu³‚È-»˜¢á´€Œùe…éßÿ'"v’RÂÛA3X¢VT¦ñ.•Œ÷®à™şaò_éŸ‚Úâ78şÛø°TNò6†eˆ}Í#Fö>;Íbáıá)o!®Ë"z#ïCÔBÀåU}e,[æ¾#`EyÙô"ĞûOL~ ünj3mR§ì§äröc («¶BÃ,`jKƒúİûXı¶¹€å0öî’+¢X3Óç–ªÈ1†ıP¢NkÉªíTGTntNƒ^Ó"’3xÑP#Á§–û$ĞiÕéü'2ƒpÌñğ$
Å§f•ó®Ò A$Õo˜C8ìá"AF7}–å¡°n‰¯ó„¯/½GŞ'àdoıÃ“•Àx
ğOÒò
t	ÑĞ˜££´°¤×*wësÕŠ‹®‘È ÖuW¡†ìqZ¥tmÊŸxˆ"9×Æñğ2¸\ÍjÚã¨ö}Siw;9ò`¦µGoj I"gr¦…©™ °îÂfp,¯±’¯Å§`kFÔ/¾~UP’2½±¸â#ÙĞ… ¤²ÄÖ­‘VÉâù>èhSÒè×S•a¦\8ì¹6£6ìïZƒnßÍ9î¤c$n´İÕÃ®85™{Ï’@æ£f§eÀ@ Ã4ˆJ: ›)+±í’dÚ P‡±ÊJdú DDedŞ0¯nwÁï ¤º®Õ,”%‡Ã×›ÂCÛ%TiWÆôœÖ?¹ìğ^şÆ*İE6ÉXØöÉÉ]‚HÄ bÛÅ}d¤42lChKQš3Z&Áa_·Œàài-<œïßº¢Š)"¼P£[/‹»(q ÿŒ(†œ›¦Œí-—QMc¢¶>ójÊ©ÅÕHMR’J[à3(É’1
éš©Cò"o–\ûI)¿Y½gÈ.vAÈgk*ªR_“ƒÆÔğ´ÅıÜmÑ$ëÎHÜ"‰ë„ûæw3İHGûĞ±úÉÇkÈ~2Ad>œ—îjJ†wşmxlKßi‘	iqÿ]uÿ(¾S.ØA>øôE94ƒ›R7,Ü³zA""”ßD¶³x<çí‹QšNŞ¥$b4u¼u0Üëæx@ŞlÃpî¼;İh[>=‡´üb¯ü2bíF=’dˆl0"ûkÓXEu²$ë²0ŸÎ7}€À‚~S5±>GÛ ä=²ŸYjÚ¦3N?Æpè¶/}SÉ^ì¦+Teì¥ÇÆkD™àèó*( j×|§Àş^pí¶ 
p¸WI~dí›;IÜNÔœğIÉûª,Ã®ÓZĞ¾l’#³%l‡3]š@%Š¸$jİÜ§$™İì3yQL‘÷áŞ£æ{Kß,ÓŞŸÁÅ²·”	xÈÒ2,c§…]N]bÌ{z Ö3Ÿ­DûIDøüüXo¯†!UL	+ğ
eªØÏæ&pàr¿‡gP°Ÿ|yœEÌô³Ä¦‡Ñpx¿p}á±™<Kï—şò»ZÉ©©céÆÒöL+ú‚Ïó/(qİgH±Ò&®<5ö6¿sõñİÛuQ­ûŞ–‡Ë÷²?i“!˜hW®=ğ8Ìı{¡ÚïÒêv)VfÚ"šîwÁá,R ÂÄ—›p%Q/ú'0¢r®M¨íú¢j"ü™şÀeÌ-g‘A˜ó7Z$P¾ÚE;ÃÆÓ¿òõZ¥§ˆ>=E"}‡Äñ	+ŠW‚æ%lÏ‡ÑåšMÎ@ß!ÿÈwK%gDÂ2(“ú²b®°vXÁ`Òt1ıÏW*²ì¥¼Võâ^Êwç–ŠÃrO]÷²sá~‘„Ê¸EguQB´%©†”‚V°ƒ¢k·äx? %ÂÔy¤»„&hLÏ­~J¸{“/ÂÿÂı[’aè­¨/R@9ƒ¸5ç™Tp1%"Ú&Ê=;çÇŞ,¥| [{+:vİÙ‹{/XàA©y‡Ÿt|JÌ+cè8zpì’§	1-Òr×Ãß° EÁO"¼ÌnBT·à5¨´>õ-¬ş‰{C$…tGNyü]%eÏ´ÆÑ¢•trWán'î;u3S"ÒµG ZaıaØÖ>¶XXå:õD_By¦2HWãšEŸR¹İ	@*µÎ@•$2 Øü„Åe. ·L5àÄFøH6“ÊÃv EÊæˆˆ÷9ISeÃURM1 ¸Â{ÛpªÉ1$øjEÂm®ôvĞĞĞ(Şl¬L6`Å–JCf5ËíÍE¢™ÉßC^»_ˆ%şü”Ú\éU&ÌñMleàVi<ü2Ñü h¯v¡^)8‡x$/EtCì¹NT;šÌÕ÷ƒ^bö¦é˜]öÌó±ĞÉŒ6ÁPq-^<¨FQsñË/% ÎLîOÑÀÿµŸËçœ"X.v+OyÔçºoŒ"@[t^
Ïbe5İáˆÊ¡Õ7>?KCÉ¬ï~#âØC­İ'*¾Ë aI¼E€øİª
æFØëÔõº©kÇ–>TvÇÂäNàp˜ ùMøŠb§[Z/M¶Ñ¨ãºîÅ‰™ £±ò=…m¼Öşhù85ä?6aZ˜of­º¿ïŞŸ²µİ$ş¦¶
ƒO6f´ÏÀUu³6C0éà*oö£?\AÿÜ8=“¢ë®«KçbŞ·4²Éğã×/ÿ×Sñ”¿ÌS8?R›["Ûtd}rè-H‰ƒ^€È"Q¦UøÇ)²%ôïälRkkê ±qÀío¯£Õ¬‘Ú¨‰­ŒŠVÃ{¹¬wïyŠ½¿šğö'r,nZÊ¦K=-wğGQ}ˆEÃÃ^?ï³^_>¿aõõb&áZVÎÒ÷jÕÊ,õ§4#Š«Q[¼HØ}5÷µ;™3ï¶hÌ:¿1âæB@#´™$Á{ ´Â;*Mº)EÙÁ·2*ÎÒ‰Á‚4¼£¾¤[»ĞÂàèÃ.ôHŞ8?:D5<¨Îš§İA‚¢ËŸ¨Iœ¬‚Ğ‡
ÑûéÚÀT/­ø±İjïe4›V£Á¹¿V éX–¯)€¼+Jà+.„?Ê®Æùë-óoOÑIY¾
c	Õ_\ó~h‡5aØpPâ*èdrÒÖÍúø?/ò‹›eaŠ+";ÍÊwr¸q ™Å¡d.9ı4i|"†³w€sÄÒ6ÛÎüËUEbÆjº3x™ş`n3Ù~’<P¡:àQxì@é*ävPÕè6¾–:Ü³q÷GH¦ÿİ7ì8ªmcg«¨päàÒıVÏR†Zää!¸Ç<Î7Ùü0lyÑY^Ü8@Dú¾ê2k¤“İqhZŒêd»V
Î‚øğä»b½ŒÈÜaÂì@o’ê ÃiÓĞnìEÁŸ	¸ú_á¬Ÿş—ã×)Ü
¦™„q³V×ïÕÛ÷mŞ· 74‹à!b;˜ò‚5>ÔS}ÙZ1»òìêo¢]Á-‹-0ÕÅk»£Z·1ÜoK¨i(Üi™	gŠĞ˜•ëª ÉEñF*c ç ìk³2À^ÖúRŒàD‰Åq².8ëñ|ün#±Fò|7©4ÁÏ„|YUJ\fã4¾%$çêí$lç\ÚthÏ2¿,Å‚‹×CşùJ9uûj‡d–ŠeÂ´º¨âYzˆÀ$éû@œÇË ½o©M¡âsJÓ‡|’Jzh8²m½Èˆ7bKxVcğ6ÈIûœ—"éï`©¦UxÒ% ÓˆÎ–Ü¼™õ1g>všpPG©FY˜í:ŠM%õÂÛbÕd='½Ñƒªà¥\GdYzD(|/]6ƒ¤ C£õìæ?ß›éB"° >fŸ	0Í›ÿIG|ƒÓTX(û"ƒ;r¼¦ª‡•n‚NYô]8™”Ö…ÜÚìøD’‰w-K¸ËÃ0¬¤•h9P4ï¡²04#ıCÿÑa¼5½RC$¨NÀ9æ¤ZÎ-3ûkŠ¸›!‹¿}ÒQUâDœsÌ«tç6wvİMÎm¨omŠ°èÃÃrdµoÂW‡)üÙû!Á†IŒŸ„NúİĞGzÕ”<¹LÌ·T©˜*"©¤™…øô&RŠıû¾YHS{$;Nè±,
V’¸:Şù‚^"x£’×öw|¦è¤MÜµãye d`V‰Ù>¥xm<¿f5Øõ3_Â]`’ŠvJ¶U}ßÊK)‚g']éÔ65²s°°Ö¦SóUIFİâ0±¬#¼IÇY¨`ìèÃãßËCÅ)}Ç1Œ§{zï&ÏK+hÌGÀ¡¾MhHèÕêŠ¯©ö§‘ÜbÔVğ sALŒ®†1Úšx7ÃÄ˜|}0> øK&QíâÜ}ƒáWúñÃ¼Š6y=J?s&=ÕçËµ¼¾¯‰±ˆ$¿2Íød»ìÇ}×­h®”:fA½¹DÈ®ûô«|“•Gk·S”-ÛíŸ7w¨Fa“È÷ñÖŸ€pé?èQu$,!ó*Xİ"d«ç2Z€«)ßtá@C‹Ÿš¨ŸãrX¨NœFö¾sÚe&´ïÚ'¤Ë»s…ÂÙD¬b™1c†‹QŠşCVä9£í$h©
uÄo
/Ô©öE6Õóíİˆ÷}Â8ÑL¡yëz~¶ÉÍ§PÕx¯âVßİ÷ù mcu»eÕÌ*Ğ¬f÷uª}ØQ8T,âBõA¿¡Ş¢\¼óµRèè¹7'9Ëüs±
€ÙŒ6§RD>S®½ÍÄ{úÉª?!ğˆ]Î8-šb§`§?¸I:óûq”N94á“¾=Rôx7JÂJèæ‹åß~t’„Õ%*D¶,s‘‹³çË)µİûélÔş?ÌdsğµäVZ×¼pğaî:Ì†Ó4†O‡ô^NIShA>‡3ÂjpQ<“õä$¨ƒ‰êvù¬%KÓær5Ç6`.I8­P¦xÆæ9“ï4ˆ&ï®íòƒc“å„•%‡’ßNœöæ#–í@Q0^­ƒË4(3	's„pÿ®éƒÒÖ;Im)ßÎ¹½9/§tß1NâõªD"ñâ oÈ)ù+=Jp)g&˜ùcî\H8‰Ü2xúó–ÌIøJPBOm×\zGZ1è%”_–Fr¯— znŒğj'ö€âÆÉ©ªIüôÍm­½iR|µ„>¥.E³YxÕµ0Î£/ÿ-+µÃƒ¡¸Õ`‘İk^8=µjÈï8eÙ¿F¤À9›…Óù= Ê¹¤ªÆ+æ–Zä¶5¦ŠR¾p®P'/CSBØ¸¦\ğœ™ô¡UçR£ùnt¸z¹¹ş+)€ú·ĞMö'‡6ëquÛ-˜\°í	q#²AÊç²ñm2d7ÆŒıSRëi§kbS$‹ :Àd÷ğcÖe´D\$Ùh}(®7âƒIëùÔLû›ûøÁZ!q€€1ôtı•ö¬ß©Ú9×yøhm®Ğ¶wøh*xz/ ®VA¶rıßõ’#Û	$‰«#LØ©ñó¬'4çœ8°OóJ¬8™§Ño@– x£üOùò*YzX²%&ªıµºís	M/ÅAº~¹·LhìÖ§1‹Š­}Øˆ¯çÆ°œgÏ”õ½ßøË¹	|:A’L‚İ‘NÒ~8YåKßşEMÕUîĞLwºÿ€ o}€5ÎÛNÉáe’w›WLl­y*$èi‰µÿZ7Ì	Jöô"šfjšmq&Í:âß»´OeIDš’­È´sÄ‰¬¨°(&Ó%qğãPRäAÀc†~†§³»2ÇWè‡mŞÚiqfŞ&aĞB>ª3tèÆƒ”‘…&·e8ød­L'ğT6;ã0ô`r[ÿxNu½šZ•s¾‡(èë’Šbµø¾(à$„ó÷:únö'¶[
ßéñ¾ïq8ù¢zß<A¨(ÒW¸ ÃÀq[Om^<ô¶Ël/cî®ó$¼õÂˆ!‘ÎJÓ²72DPªªê+Î3†B‡ÉH.ìòÖŸi
xæg»Ÿ#ùvj›‚¯ŒE$ƒ¬ÚÂ¯sk»>tnŒ¼©6Ì³îv5_ª‘S±¿Û†°“ï¦”ó;NwàH3›7øÃÖSóIVsØ>'¦µÖı«‡’KÄ5ş„2GÄ%îænoI¤ÍËf$ş–Sp°·ù¼‹2êªëå~ ÃV¨ş‡Ùi_Ú³ğTE.¯¡Ób‰˜neNÌ/ò.Y_w2ÖYV°~…§Ğê«¡K(	­,œ ZoWİ_M"RÚÉjj¡Ê#ò:`H¾W'ˆ©d%AUØÙ²Ò­º:CªÜ+½¡€,FÇÉÿÁm›˜ul¼‚Ä(Œ5àS—/Ä`“(±[Ñœ¼±hJ;²}İ¼äAZÅ×k<‰?!$úÖš››RğK½Î] »ŠD²·»FZg.Z
Ì!ÔÄüñçÖ˜tÇ†‚0Ÿ­šê¦ûºÌtëÌ0©rQ€øÕ–6„UiˆMÁ-ş:àÚ}6"–Œÿª¹Ãğ“œ¿€•,èÑö·{şÌ‡`©Ğ1â€E‰$ßº/të©BfR3ªf¹Ô•­Ş.Q>Mâ¡ŞÿS’2´	˜ÌÆrÜZœ¸XdJMdŒ·l-Édù»Jä7²¯„Øp}ê™gn°7˜}„ßú×#éŠ9Ëd­tB’|{Oi`rD cZy©<i´ëzm$:Ã±§Ò³ip÷×§;ÁbÑI¢w'Éòzt8å4İ;sÉsã}ìlÀVÒX‡(láûl¢Rã5N`íÇCEEb‡zÙ„Œ6k¡;ÚËüÔ‹1’¨ª‚ËØslw8±N'Ôò%ØÇ°ã¨Êio­Á…XByZøşm§ nHU¹£;«v‚a2ÆÚ>½+êdšÆÀ ÖXc(×	YyP¦HÊ·ğ‚‰¶OzmŞ¢æqq6r”˜4wUÜiT)?«Kd&ã_ÊL1¯e[YCˆMjƒ–R!³ˆ?(
(—”ÑÒÉ$0Ö¹Ìã ¥;t†gåbĞõukÙ½ZHïˆúOœÈïµ `»Y•ÇÍhxHä’%ã_›ò0xÄµ¦âD™zK¹á&;cÉ<¯X×¨‹€FI>è3@øg×±ÁËğÎ˜NØ@åcD~¢&‚¸‚ÜÄê~ìæoÏ‚Ÿ®âZú3ªš}½§XCÆKêX GÕÑQd-—r”›R`†Y Z£Ì¡Iüò^ˆvş)3êÄ]V0êh°Rê•RX,q§–Úƒ8\$ÖLõ{	‚oñ1Kîu[öQG ¢"¥¸ôF–BâÅß¦-?&ŸØı(<ğÿ&Gºp