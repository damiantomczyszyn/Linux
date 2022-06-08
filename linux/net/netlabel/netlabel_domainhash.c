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
ÂY•·ÿBÌ™¢‡0Ş´·/¸óuäÆş†¡¥è¸OB7¯Ô‚_'Şp-gaÆ¾€®wÛT-88šhƒ¹i·¼