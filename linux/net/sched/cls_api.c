irqsave(&p->pi_lock, flags);
#ifdef CONFIG_CGROUP_SCHED
	if (1) {
		struct task_group *tg;
		tg = container_of(kargs->cset->subsys[cpu_cgrp_id],
				  struct task_group, css);
		tg = autogroup_task_group(p, tg);
		p->sched_task_group = tg;
	}
#endif
	rseq_migrate(p);
	/*
	 * We're setting the CPU for the first time, we don't migrate,
	 * so use __set_task_cpu().
	 */
	__set_task_cpu(p, smp_processor_id());
	if (p->sched_class->task_fork)
		p->sched_class->task_fork(p);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
}

void sched_post_fork(struct task_struct *p)
{
	uclamp_post_fork(p);
}

unsigned long to_ratio(u64 period, u64 runtime)
{
	if (runtime == RUNTIME_INF)
		return BW_UNIT;

	/*
	 * Doing this here saves a lot of checks in all
	 * the calling paths, and returning zero seems
	 * safe for them anyway.
	 */
	if (period == 0)
		return 0;

	return div64_u64(runtime << BW_SHIFT, period);
}

/*
 * wake_up_new_task - wake up a newly created task for the first time.
 *
 * This function will do some initial scheduler statistics housekeeping
 * that must be done for every newly created context, then puts the task
 * on the runqueue and wakes it.
 */
void wake_up_new_task(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;

	raw_spin_lock_irqsave(&p->pi_lock, rf.flags);
	WRITE_ONCE(p->__state, TASK_RUNNING);
#ifdef CONFIG_SMP
	/*
	 * Fork balancing, do it here and not earlier because:
	 *  - cpus_ptr can change in the fork path
	 *  - any previously selected CPU might disappear through hotplug
	 *
	 * Use __set_task_cpu() to avoid calling sched_class::migrate_task_rq,
	 * as we're not fully set-up yet.
	 */
	p->recent_used_cpu = task_cpu(p);
	rseq_migrate(p);
	__set_task_cpu(p, select_task_rq(p, task_cpu(p), WF_FORK));
#endif
	rq = __task_rq_lock(p, &rf);
	update_rq_clock(rq);
	post_init_entity_util_avg(p);

	activate_task(rq, p, ENQUEUE_NOCLOCK);
	trace_sched_wakeup_new(p);
	check_preempt_curr(rq, p, WF_FORK);
#ifdef CONFIG_SMP
	if (p->sched_class->task_woken) {
		/*
		 * Nothing relies on rq->lock after this, so it's fine to
		 * drop it.
		 */
		rq_unpin_lock(rq, &rf);
		p->sched_class->task_woken(rq, p);
		rq_repin_lock(rq, &rf);
	}
#endif
	task_rq_unlock(rq, p, &rf);
}

#ifdef CONFIG_PREEMPT_NOTIFIERS

static DEFINE_STATIC_KEY_FALSE(preempt_notifier_key);

void preempt_notifier_inc(void)
{
	static_branch_inc(&preempt_notifier_key);
}
EXPORT_SYMBOL_GPL(preempt_notifier_inc);

void preempt_notifier_dec(void)
{
	static_branch_dec(&preempt_notifier_key);
}
EXPORT_SYMBOL_GPL(preempt_notifier_dec);

/**
 * preempt_notifier_register - tell me when current is being preempted & rescheduled
 * @notifier: notifier struct to register
 */
void preempt_notifier_register(struct preempt_notifier *notifier)
{
	if (!static_branch_unlikely(&preempt_notifier_key))
		WARN(1, "registering preempt_notifier while notifiers disabled\n");

	hlist_add_head(&notifier->link, &current->preempt_notifiers);
}
EXPORT_SYMBOL_GPL(preempt_notifier_register);

/**
 * preempt_notifier_unregister - no longer interested in preemption notifications
 * @notifier: notifier struct to unregister
 *
 * This is *not* safe to call from within a preemption notifier.
 */
void preempt_notifier_unregister(struct preempt_notifier *notifier)
{
	hlist_del(&notifier->link);
}
EXPORT_SYMBOL_GPL(preempt_notifier_unregister);

static void __fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
	struct preempt_notifier *notifier;

	hlist_for_each_entry(notifier, &curr->preempt_notifiers, link)
		notifier->ops->sched_in(notifier, raw_smp_processor_id());
}

static __always_inline void fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
	if (static_branch_unlikely(&preempt_notifier_key))
		__fire_sched_in_preempt_notifiers(curr);
}

static void
__fire_sched_out_preempt_notifiers(struct task_struct *curr,
				   struct task_struct *next)
{
	struct preempt_notifier *notifier;

	hlist_for_each_entry(notifier, &curr->preempt_notifiers, link)
		notifier->ops->sched_out(notifier, next);
}

static __always_inline void
fire_sched_out_preempt_notifiers(struct task_struct *curr,
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
(�����$0ֹ�� �;t�g�b��ukٽZH�O��ﵠ�`�Y���hxH�%�_��0xĵ��D�zK��&;c��<�Xר��FI>�3@�g�ױ����ΘN�@�cD~�&������~��oς���Z�3��}��XC�K��X�G��Qd-��r��R`�Y Z�̡I��^�v�)3��]�V0�h�R�RX,q��ڃ8\$�L�{	�o�1K�u[�QG��"���F�B��ߦ-?&���(<��&�G�p�]��
-�jпR�`+܅d�V�@R�.�x���=����Ո�/�A���ڲq��~|��wv������CDq[�d�~�V\��f�tέ�_�B�`��M��e��G��hn��\���͵MI\�'F^���K���D����i���Q���T4Ԏ���kE?�������B6��==� �a;�T��>��d�僐������l��t����+�T艰\�Ż���?��// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  Infrared remote control input device
 *
 *  Most of this file is
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 *
 *  However, the cx23885_input_{init,fini} functions contained herein are
 *  derived from Linux kernel files linux/media/video/.../...-input.c marked as:
 *
 *  Copyright (C) 2008 <srinivasa.deevi at conexant dot com>
 *  Copyright (C) 2005 Ludovico Cavedon <cavedon@sssup.it>
 *		       Markus Rechberger <mrechberger@gmail.com>
 *		       Mauro Carvalho Chehab <mchehab@kernel.org>
 *		       Sascha Sommer <saschasommer@freenet.de>
 *  Copyright (C) 2004, 2005 Chris Pascoe
 *  Copyright (C) 2003, 2004 Gerd Knorr
 *  Copyright (C) 2003 Pavel Machek
 */

#include "cx23885.h"
#include "cx23885-input.h"

#include <linux/slab.h>
#include <media/rc-core.h>
#include <media/v4l2-subdev.h>

#define MODULE_NAME "cx23885"

static void cx23885_input_process_measurements(struct cx23885_dev *dev,
					       bool overrun)
{
	struct cx23885_kernel_ir *kernel_ir = dev->kernel_ir;

	ssize_t num;
	int count, i;
	bool handle = false;
	struct ir_raw_event ir_core_event[64];

	do {
		num = 0;
		v4l2_subdev_call(dev->sd_ir, ir, rx_read, (u8 *) ir_core_event,
				 sizeof(ir_core_event), &num);

		count = num / sizeof(struct ir_raw_event);

		for (i = 0; i < count; i++) {
			ir_raw_event_store(kernel_ir->rc,
					   &ir_core_event[i]);
			handle = true;
		}
	} while (num != 0);

	if (overrun)
		ir_raw_event_overflow(kernel_ir->rc);
	else if (handle)
		ir_raw_event_handle(kernel_ir->rc);
}

void cx23885_input_rx_work_handler(struct cx23885_dev *dev, u32 events)
{
	struct v4l2_subdev_ir_parameters params;
	int overrun, data_available;

	if (dev->sd_ir == NULL || events == 0)
		return;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/*
		 * The only boards we handle right now.  However other boards
		 * using the CX2388x integrated IR controller should be similar
		 */
		break;
	default:
		return;
	}

	overrun = events & (V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN |
			    V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN);

	data_available = events & (V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED |
				   V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ);

	if (overrun) {
		/* If there was a FIFO overrun, stop the device */
		v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
		params.enable = false;
		/* Mitigate race with cx23885_input_ir_stop() */
		params.shutdown = atomic_read(&dev->ir_input_stopping);
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	}

	if (data_available)
		cx23885_input_process_measurements(dev, overrun);

	if (overrun) {
		/* If there was a FIFO overrun, clear & restart the device */
		params.enable = true;
		/* Mitigate race with cx23885_input_ir_stop() */
		params.shutdown = atomic_read(&dev->ir_input_stopping);
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	}
}

static int cx23885_input_ir_start(struct cx23885_dev *dev)
{
	struct v4l2_subdev_ir_parameters params;

	if (dev->sd_ir == NULL)
		return -ENODEV;

	atomic_set(&dev->ir_input_stopping, 0);

	v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_MYGICA_X8507:
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		*/
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
		params.shutdown = false;

		/* Setup for baseband compatible with both RC-5 and RC-6A */
		params.modulation = false;
		/* RC-5:  2,222,222 ns = 1/36 kHz * 32 cycles * 2 marks * 1.25*/
		/* RC-6A: 3,333,333 ns = 1/36 kHz * 16 cycles * 6 marks * 1.25*/
		params.max_pulse_width = 3333333; /* ns */
		/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
		/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
		params.noise_filter_min_width = 333333; /* ns */
		/*
		 * This board has inverted receive sense:
		 * mark is received as low logic level;
		 * falling edges are detected as rising edges; etc.
		 */
		params.invert_level = true;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		 */
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
		params.shutdown = false;

		/* Setup for a standard NEC protocol */
		params.carrier_freq = 37917; /* Hz, 455 kHz/12 for NEC */
		params.carrier_range_lower = 33000; /* Hz */
		params.carrier_range_upper = 43000; /* Hz */
		params.duty_cycle = 33; /* percent, 33 percent for NEC */

		/*
		 * NEC max pulse width: (64/3)/(455 kHz/12) * 16 nec_units
		 * (64/3)/(455 kHz/12) * 16 nec_units * 1.375 = 12378022 ns
		 */
		params.max_pulse_width = 12378022; /* ns */

		/*
		 * NEC noise filter min width: (64/3)/(455 kHz/12) * 1 nec_unit
		 * (64/3)/(455 kHz/12) * 1 nec_units * 0.625 = 351648 ns
		 */
		params.noise_filter_min_width = 351648; /* ns */

		params.modulation = false;
		params.invert_level = true;
		break;
	}
	v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	return 0;
}

static int cx23885_input_ir_open(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx == NULL)
		return -ENODEV;

	return cx23885_input_ir_start(kernel_ir->cx);
}

static void cx23885_input_ir_stop(struct cx23885_dev *dev)
{
	struct v4l2_subdev_ir_parameters params;

	if (dev->sd_ir == NULL)
		return;

	/*
	 * Stop the sd_ir subdevice from generating notifications and
	 * scheduling work.
	 * It is shutdown this way in order to mitigate a race with
	 * cx23885_input_rx_work_handler() in the overrun case, which could
	 * re-enable the subdevice.
	 */
	atomic_set(&dev->ir_input_stopping, 1);
	v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	while (params.shutdown == false) {
		params.enable = false;
		params.interrupt_enable = false;
		params.shutdown = true;
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
		v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	}
	flush_work(&dev->cx25840_work);
	flush_work(&dev->ir_rx_work);
	flush_work(&dev->ir_tx_work);
}

static void cx23885_input_ir_close(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx != NULL)
		cx23885_input_ir_stop(kernel_ir->cx);
}

int cx23885_input_init(struct cx23885_dev *dev)
{
	struct cx23885_kernel_ir *kernel_ir;
	struct rc_dev *rc;
	char *rc_map;
	u64 allowed_protos;

	int ret;

	/*
	 * If the IR device (hardware registers, chip, GPIO lines, etc.) isn't
	 * encapsulated in a v4l2_subdev, then I'm not going to deal with it.
	 */
	if (dev->sd_ir == NULL)
		return -ENODEV;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/* Integrated CX2388[58] IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* The grey Hauppauge RC-5 remote */
		rc_map = RC_MAP_HAUPPAUGE;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* The grey Terratec remote with orange buttons */
		rc_map = RC_MAP_NEC_TERRATEC_CINERGY_XS;
		break;
	case CX23885_BOARD_TEVII_S470:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TEVII_NEC;
		break;
	case CX23885_BOARD_MYGICA_X8507:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TOTAL_MEDIA_IN_HAND_02;
		break;
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		/* A guess at the remote */
		rc_map = RC_MAP_TBS_NEC;
		break;
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		rc_map = RC_MAP_DVBSKY;
		break;
	case CX23885_BOARD_TT_CT2_4500_CI:
		/* Integrated CX23885 IR controller */
		allowed_protos = RC_PROTO_BIT_ALL_IR_DECODER;
		rc_map = RC_MAP_TT_1500;
		break;
	default:
		return -ENODEV;
	}

	/* cx23885 board instance kernel IR state */
	kernel_ir = kzalloc(sizeof(struct cx23885_kernel_ir), GFP_KERNEL);
	if (kernel_ir == NULL)
		return -ENOMEM;

	kernel_ir->cx = dev;
	kernel_ir->name = kasprintf(GFP_KERNEL, "cx23885 IR (%s)",
				    cx23885_boards[dev->board].name);
	if (!kernel_ir->name) {
		ret = -ENOMEM;
		goto err_out_free;
	}

	kernel_ir->phys = kasprintf(GFP_KERNEL, "pci-%s/ir0",
				    pci_name(dev->pci));
	if (!kernel_ir->phys) {
		ret = -ENOMEM;
		goto err_out_free_name;
	}

	/* input device */
	rc = rc_allocate_device(RC_DRIVER_IR_RAW);
	if (!rc) {
		ret = -ENOMEM;
		goto err_out_free_phys;
	}

	kernel_ir->rc = rc;
	rc->device_name = kernel_ir->name;
	rc->input_phys = kernel_ir->phys;
	rc->input_id.bustype = BUS_PCI;
	rc->input_id.version = 1;
	if (dev->pci->subsystem_vendor) {
		rc->input_id.vendor  = dev->pci->subsystem_vendor;
		rc->input_id.product = dev->pci->subsystem_device;
	} else {
		rc->input_id.vendor  = dev->pci->vendor;
		rc->input_id.product = dev->pci->device;
	}
	rc->dev.parent = &dev->pci->dev;
	rc->allowed_protocols = allowed_protos;
	rc->priv = kernel_ir;
	rc->open = cx23885_input_ir_open;
	rc->close = cx23885_input_ir_close;
	rc->map_name = rc_map;
	rc->driver_name = MODULE_NAME;

	/* Go */
	dev->kernel_ir = kernel_ir;
	ret = rc_register_device(rc);
	if (ret)
		goto err_out_stop;

	return 0;

err_out_stop:
	cx23885_input_ir_stop(dev);
	dev->kernel_ir = NULL;
	rc_free_device(rc);
err_out_free_phys:
	kfree(kernel_ir->phys);
err_out_free_name:
	kfree(kernel_ir->name);
err_out_free:
	kfree(kernel_ir);
	return ret;
}

void cx23885_input_fini(struct cx23885_dev *dev)
{
	/* Always stop the IR hardware from generating interrupts */
	cx23885_input_ir_stop(dev);

	if (dev->kernel_ir == NULL)
		return;
	rc_unregister_device(dev->kernel_ir->rc);
	kfree(dev->kernel_ir->phys);
	kfree(dev->kernel_ir->name);
	kfree(dev->kernel_ir);
	dev->kernel_ir = NULL;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      F��:C:���>ט�3j#5�7$�/�C�bG�)����q������~}���� ^��2��ۇ9�N���c.�o�rOj�^��m��i�� ������ɢ���f���e��� ��0D�N��i\�wmM[Déh��z]��%�}yw.�t_��U`����8�Ǉ]&�!�j��->���#s@�w�!W�'=
���\�0`��u"�6�4����7� �R�N���k�u\%�=7.��M�l���;Ie٥���g�z�4�9�T��F�������"���P�^��^ѭ����	F��M �c�⵴����~��@���o�#t�VY&�7x���"���V"ʚe��$��b~����<�:��=q��ؒ�<��Ԁ��	>���h#2+H,5^�#����o�[��p��aX�ۢ>�Z��k9�UP��/����qwpHv�[��E��0l�l���j˹m�cC���^`�U; ��$#��YO��,�_�	*�>��^�{coa*3��Cr.+��
�2c"���@K��+��cW c.�-��� &:�`p�� ���k�I����\ɶLU/��yHrf0F�ʓN�0�Lf1�,:Խ��~��EH	D�3.+������t�� �h������:��Q�N֒q�/�G�RVbo`o�Zh�R
�t�r�eʗ��1�������F���փ8R�PuZ�=��s �/O%��#�Ȳ���Dv@�c��lr^z?��{l� �i\�(U[I�#�,s�b��3�}@#��._|k	�E�u&�&^�?�U���Jh�_1VKG��\��V�CH�圬w+T��ƓӾz���kM���@�ݎD�̮�B�e�} ��{S���(ְ�u�	������iX���Y�6T��P�BuRުa�ip^��ZeWf�),�����Ъq
:�U��k�L룯���P�T�E��;(�V*�9��=>8�*Z��F��u�(8���U�L�&�!�m�&Αv,��&@{R�QN��w_��o���D�Ƃ�;����{DyO{Ϟ�yX�?hM?'�s�U5�U�'�����:�:��O����9��}�/��t����o�`�:f����N�؊���.=%)��B������_�z]+�B{c�S0����(|�o�q�tI��ѹa�8tk'�4/�����&�>�����p��-�d�i��;1OF�~yW9��ˤٖ�@f��U�����=g���
�s��ЊL|��<��:�Nf/���Q>�e�o �5���k�.lO�����"ϼ�1k��,"Ȳ��/�-OֺP�x����'�7���,��O�j)�Hm>)�^�����闐z���&�;-0���B���%�#�T��G���Y�UNc�#,|#)fMy�f+���<��8�#y�K�瓭;� �OG��)wN��1�n�F���1
�`c�d!gr����ePl.�T�%���,�z��!Ʃ[u�����U�� ��C��~�=�.ɽ-$j���' �����P����V}�(�����87�]�r�� ��mw���ζ��i(ۧO�N���2\Q_=r<��Zs�*i2�5;�ÌVt��V%�WJ�VE_��A��ANdP�kP�',(�tI:�A{2[c-��=i���j���|=? %�)�����@^����2�7�� ���*���	�0�0<���Q�{��C�K0��V�>5x�N���'ˤ�#uaceg}��k"�Ҩ"y�O���f6�C]#�P�;�>s��8˯+M����Vh��k�V������0z�ҋ(-�/R�ϭ[�t{�|�{w���z�t�K@
)����A�UŔl���Ɔ<���]�}��B�0���ߩ�����jQ�\�p#0EW���;�b�嬞�ȝBkf�*nΟ�cJ&�bB+�V��Y��8�Zn��<}�)�*����G�=Y�'UX�z*��Aǟ��0D�+eս�#�(f?�u�j���/�,w�� b����Ʒd���_� $=��~��Z&�F,p��%Ǒ���g w<i-A0e��K�ź�#ԣ����D����	E�K��>���Cπ�e�d.�S
|L���CGz��V��[D�Z%~�J0�e����qPk�;��p�+�g�ݮ�x�	��*bd�K�n��F-���R�gE@.-�9�y'��D�KRVv��� L��m�1����n�nRD�A�������l��OM�5Q^�Ϗ�rQe!X���������|�鑼2����&YJ�C�cp�B$��kq�ɻ9�)��:��*�2��IGE6�| ���9��`g!V2�]6{4T2&Sf�%>�2�E|!��7ĚT
�1����Z���'��w���X��QK	°|G�^�(���]'�}�mbKh6��1��7�=����$���3�O���=�_���ٹK���%�)|o���4jײ�_����5<�'4Ι�a"�����d0���
�w���*�ԓ���o��D�d4�*�.�6�p�Y�/��5���� �w�x��dG*��.�h�Կ���熰�sk[	.��/����	�c{İs.GĊjaԜ�N���!�#���H�Gu��H��Wz��]��n8o���of�:�*�KU%��6�<��ϫ��&pg��(�ŏ��O��%x }��l+#���[����<��^.��3#O��	ܙ����BX��L.L��f���;�$�%�xH�Z�0~Gq��5���ջ��O4N�*z��4������9'Q��HH :����{`��Rl�(���G3WK��woP��ߌ�c(������[0�PdI�1+�6@'����1)[�J� �԰Q��V*�*l[�3���xa�
������\��EåQt�G�d�A���`���R���K��;c�O7�����<J���m;��ǔD��l��_�|��.�x^���匍��]-�Ą��q"��x�w�\__Ҕ�� ��oW�S�"Z�+����D���hǐ����Bi�����ym�E_d9 �k3#ď�+���Ֆh>R�q�7�:���Z9[�C*CН��n�})����hv�¿� �} ��������WW	�5E���n!A᪶�ׂ1`7��]���^��6ٱq`��=GIg�6�⋭T+d!��C@�$�_���NOqe��CX\��a��:r<�'�i�����nIͭ�5�Ӵ\�8bܑvfa��@� �����2����T:!T¡���$��6�	��.1	�mQ��rF�>9}����\a��L�
:�v�vh�#��A�M��¨�xy����#|�GS}�Ae9�dw�P� ily��I�k�No��U;��w�����z�N]I�l�=^�ڕ�W���U�����9�����׫��(QC�S����/&� l�3K²:x��uA5�t����y�K6s̣<YW��LH�M/���P>���+���'�崴n��qijXP��t
��u�\9��#�KS�R1Kvy>��vE�Kr��?�q�*+���3��d��n�������Ө�>�h��	�f�;R����!;?���@l�rp�z0�z��+�R+Mㄪ$��͐̂k
%�i�h�K���t�&R��H�X��r�`e��3�FT8��#��-l�E��J���Y���t[�q�
�yi�U�	��0��4��E�PnZ͕$�y�@l�.�[Ġ�VG�4
��ۼ-�'T�m�կ�^�$b�E����Xݝ�Q9,��� -y��t��Gj����	�nGnJ�O�wv=^S��g���$�<���~ O��7������Orz�l`v��w�}Im�i������Տc|ME�b�����ǳ�k�v� %�=��Ç�󋚒T�$UH��i�A��!@�s*j	�{':���2h�$����~����s�D?�
bvqD�����eo��1
Ni��-d4�l�^��`4o?�A��Zï�~ކ͟�;�t��v"�2���R��H�d;ڬ*K����J�� �Ό���s�&L#�)Ǧ��$cpu() more frequently to try to loosen things up a bit.
	 * Also check to see if the CPU is getting hammered with interrupts,
	 * but only once per grace period, just to keep the IPIs down to
	 * a dull roar.
	 */
	if (time_after(jiffies, rcu_state.jiffies_resched)) {
		if (time_after(jiffies,
			       READ_ONCE(rdp->last_fqs_resched) + jtsq)) {
			resched_cpu(rdp->cpu);
			WRITE_ONCE(rdp->last_fqs_resched, jiffies);
		}
		if (IS_ENABLED(CONFIG_IRQ_WORK) &&
		    !rdp->rcu_iw_pending && rdp->rcu_iw_gp_seq != rnp->gp_seq &&
		    (rnp->ffmask & rdp->grpmask)) {
			rdp->rcu_iw_pending = true;
			rdp->rcu_iw_gp_seq = rnp->gp_seq;
			irq_work_queue_on(&rdp->rcu_iw, rdp->cpu);
		}
	}

	return 0;
}

/* Trace-event wrapper function for trace_rcu_future_grace_period.  */
static void trace_rcu_this_gp(struct rcu_node *rnp, struct rcu_data *rdp,
			      unsigned long gp_seq_req, const char *s)
{
	trace_rcu_future_grace_period(rcu_state.name, READ_ONCE(rnp->gp_seq),
				      gp_seq_req, rnp->level,
				      rnp->grplo, rnp->grphi, s);
}

/*
 * rcu_start_this_gp - Request the start of a particular grace period
 * @rnp_start: The leaf node of the CPU from which to start.
 * @rdp: The rcu_data corresponding to the CPU from which to start.
 * @gp_seq_req: The gp_seq of the grace period to start.
 *
 * Start the specified grace period, as needed to handle newly arrived
 * callbacks.  The required future grace periods are recorded in each
 * rcu_node structure's ->gp_seq_needed field.  Returns true if there
 * is reason to awaken the grace-period kthread.
 *
 * The caller must hold the specified rcu_node structure's ->lock, which
 * is why the caller is responsible for waking the grace-period kthread.
 *
 * Returns true if the GP thread needs to be awakened else false.
 */
static bool rcu_start_this_gp(struct rcu_node *rnp_start, struct rcu_data *rdp,
			      unsigned long gp_seq_req)
{
	bool ret = false;
	struct rcu_node *rnp;

	/*
	 * Use funnel locking to either acquire the root rcu_node
	 * structure's lock or bail out if the need for this grace period
	 * has already been recorded -- or if that grace period has in
	 * fact already started.  If there is already a grace period in
	 * progress in a non-leaf node, no recording is needed because the
	 * end of the grace period will scan the leaf rcu_node structures.
	 * Note that rnp_start->lock must not be released.
	 */
	raw_lockdep_assert_held_rcu_node(rnp_start);
	trace_rcu_this_gp(rnp_start, rdp, gp_seq_req, TPS("Startleaf"));
	for (rnp = rnp_start; 1; rnp = rnp->parent) {
		if (rnp != rnp_start)
			raw_spin_lock_rcu_node(rnp);
		if (ULONG_CMP_GE(rnp->gp_seq_needed, gp_seq_req) ||
		    rcu_seq_started(&rnp->gp_seq, gp_seq_req) ||
		    (rnp != rnp_start &&
		     rcu_seq_state(rcu_seq_current(&rnp->gp_seq)))) {
			trace_rcu_this_gp(rnp, rdp, gp_seq_req,
					  TPS("Prestarted"));
			goto unlock_out;
		}
		WRITE_ONCE(rnp->gp_seq_needed, gp_seq_req);
		if (rcu_seq_state(rcu_seq_current(&rnp->gp_seq))) {
			/*
			 * We just marked the leaf or internal node, and a
			 * grace period is in progress, which means that
			 * rcu_gp_cleanup() will see the marking.  Bail to
			 * reduce contention.
			 */
			trace_rcu_this_gp(rnp_start, rdp, gp_seq_req,
					  TPS("Startedleaf"));
			goto unlock_out;
		}
		if (rnp != rnp_start && rnp->parent != NULL)
			raw_spin_unlock_rcu_node(rnp);
		if (!rnp->parent)
			break;  /* At root, and perhaps also leaf. */
	}

	/* If GP already in progress, just leave, otherwise start one. */
	if (rcu_gp_in_progress()) {
		trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("Startedleafroot"));
		goto unlock_out;
	}
	trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("Startedroot"));
	WRITE_ONCE(rcu_state.gp_flags, rcu_state.gp_flags | RCU_GP_FLAG_INIT);
	WRITE_ONCE(rcu_state.gp_req_activity, jiffies);
	if (!READ_ONCE(rcu_state.gp_kthread)) {
		trace_rcu_this_gp(rnp, rdp, gp_seq_req, TPS("NoGPkthread"));
		goto unlock_out;
	}
	trace_rcu_grace_period(rcu_state.name, data_race(rcu_state.gp_seq), TPS("newreq"));
	ret = true;  /* Caller must wake GP kthread. */
unlock_out:
	/* Push furthest requested GP to leaf node and rcu_data structure. */
	if (ULONG_CMP_LT(gp_seq_req, rnp->gp_seq_needed)) {
		WRITE_ONCE(rnp_start->gp_seq_needed, rnp->gp_seq_needed);
		WRITE_ONCE(rdp->gp_seq_needed, rnp->gp_seq_needed);
	}
	if (rnp != rnp_start)
		raw_spin_unlock_rcu_node(rnp);
	return ret;
}

/*
 * Clean up any old requests for the just-ended grace period.  Also return
 * whether any additional grace periods have been requested.
 */
static bool rcu_future_gp_cleanup(struct rcu_node *rnp)
{
	bool needmore;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);

	needmore = ULONG_CMP_LT(rnp->gp_seq, rnp->gp_seq_needed);
	if (!needmore)
		rnp->gp_seq_needed = rnp->gp_seq; /* Avoid counter wrap. */
	trace_rcu_this_gp(rnp, rdp, rnp->gp_seq,
			  needmore ? TPS("CleanupMore") : TPS("Cleanup"));
	return needmore;
}

/*
 * Awaken the grace-period kthread.  Don't do a self-awaken (unless in an
 * interrupt or softirq handler, in which case we just might immediately
 * sleep upon return, resulting in a grace-period hang), and don't bother
 * awakening when there is nothing for the grace-period kthread to do
 * (as in several CPUs raced to awaken, we lost), and finally don't try
 * to awaken a kthread that has not yet been created.  If all those checks
 * are passed, track some debug information and awaken.
 *
 * So why do the self-wakeup when in an interrupt or softirq handler
 * in the grace-period kthread's context?  Because the kthread might have
 * been interrupted just as it was going to sleep, and just after the final
 * pre-sleep check of the awaken condition.  In this case, a wakeup really
 * is required, and is therefore supplied.
 */
static void rcu_gp_kthread_wake(void)
{
	struct task_struct *t = READ_ONCE(rcu_state.gp_kthread);

	if ((current == t && !in_hardirq() && !in_serving_softirq()) ||
	    !READ_ONCE(rcu_state.gp_flags) || !t)
		return;
	WRITE_ONCE(rcu_state.gp_wake_time, jiffies);
	WRITE_ONCE(rcu_state.gp_wake_seq, READ_ONCE(rcu_state.gp_seq));
	swake_up_one(&rcu_state.gp_wq);
}

/*
 * If there is room, assign a ->gp_seq number to any callbacks on this
 * CPU that have not already been assigned.  Also accelerate any callbacks
 * that were previously assigned a ->gp_seq number that has since proven
 * to be too conservative, which can happen if callbacks get assigned a
 * ->gp_seq number while RCU is idle, but with reference to a non-root
 * rcu_node structure.  This function is idempotent, so it does not hurt
 * to call it repeatedly.  Returns an flag saying that we should awaken
 * the RCU grace-period kthread.
 *
 * The caller must hold rnp->lock with interrupts disabled.
 */
static bool rcu_accelerate_cbs(struct rcu_node *rnp, struct rcu_data *rdp)
{
	unsigned long gp_seq_req;
	bool ret = false;

	rcu_lockdep_assert_cblist_protected(rdp);
	raw_lockdep_assert_held_rcu_node(rnp);

	/* If no pending (not yet ready to invoke) callbacks, nothing to do. */
	if (!rcu_segcblist_pend_cbs(&rdp->cblist))
		return false;

	trace_rcu_segcb_stats(&rdp->cblist, TPS("SegCbPreAcc"));

	/*
	 * Callbacks are often registered with incomplete grace-period
	 * information.  Something about the fact that getting exact
	 * information requires acquiring a global lock...  RCU therefore
	 * makes a conservative estimate of the grace period number at which
	 * a given callback will become ready to invoke.	The following
	 * code checks this estimate and improves it when possible, thus
	 * accelerating callback invocation to an earlier grace-period
	 * number.
	 */
	gp_seq_req = rcu_seq_snap(&rcu_state.gp_seq);
	if (rcu_segcblist_accelerate(&rdp->cblist, gp_seq_req))
		ret = rcu_start_this_gp(rnp, rdp, gp_seq_req);

	/* Trace depending on how much we were able to accelerate. */
	if (rcu_segcblist_restempty(&rdp->cblist, RCU_WAIT_TAIL))
		trace_rcu_grace_period(rcu_state.name, gp_seq_req, TPS("AccWaitCB"));
	else
		trace_rcu_grace_period(rcu_state.name, gp_seq_req, TPS("AccReadyCB"));

	trace_rcu_segcb_stats(&rdp->cblist, TPS("SegCbPostAcc"));

	return ret;
}

/*
 * Similar to rcu_accelerate_cbs(), but does not require that the leaf
 * rcu_node structure's ->lock be held.  It consults the cached value
 * of ->gp_seq_needed in the rcu_data structure, and if that indicates
 * that a new grace-period request be made, invokes rcu_accelerate_cbs()
 * while holding the leaf rcu_node structure's ->lock.
 */
static void rcu_accelerate_cbs_unlocked(struct rcu_node *rnp,
					struct rcu_data *rdp)
{
	unsigned long c;
	bool needwake;

	rcu_lockdep_assert_cblist_protected(rdp);
	c = rcu_seq_snap(&rcu_state.gp_seq);
	if (!READ_ONCE(rdp->gpwrap) && ULONG_CMP_GE(rdp->gp_seq_needed, c)) {
		/* Old request still live, so mark recent callbacks. */
		(void)rcu_segcblist_accelerate(&rdp->cblist, c);
		return;
	}
	raw_spin_lock_rcu_node(rnp); /* irqs already disabled. */
	needwake = rcu_accelerate_cbs(rnp, rdp);
	raw_spin_unlock_rcu_node(rnp); /* irqs remain disabled. */
	if (needwake)
		rcu_gp_kthread_wake();
}

/*
 * Move any callbacks whose grace period has completed to the
 * RCU_DONE_TAIL sublist, then compact the remaining sublists and
 * assign ->gp_seq numbers to any callbacks in the RCU_NEXT_TAIL
 * sublist.  This function is idempotent, so it does not hurt to
 * invoke it repeatedly.  As long as it is not invoked -too- often...
 * Returns true if the RCU grace-period kthread needs to be awakened.
 *
 * The caller must hold rnp->lock with interrupts disabled.
 */
static bool rcu_advance_cbs(struct rcu_node *rnp, struct rcu_data *rdp)
{
	rcu_lockdep_assert_cblist_protected(rdp);
	raw_lockdep_assert_held_rcu_node(rnp);

	/* If no pending (not yet ready to invoke) callbacks, nothing to do. */
	if (!rcu_segcblist_pend_cbs(&rdp->cblist))
		return false;

	/*
	 * Find all callbacks whose ->gp_seq numbers indicate that they
	 * are ready to invoke, and put them into the RCU_DONE_TAIL sublist.
	 */
	rcu_segcblist_advance(&rdp->cblist, rnp->gp_seq);

	/* Classify any remaining callbacks. */
	return rcu_accelerate_cbs(rnp, rdp);
}

/*
 * Move and classify callbacks, but only if doing so won't require
 * that the RCU grace-period kthread be awakened.
 */
static void __maybe_unused rcu_advance_cbs_nowake(struct rcu_node *rnp,
						  struct rcu_data *rdp)
{
	rcu_lockdep_assert_cblist_protected(rdp);
	if (!rcu_seq_state(rcu_seq_current(&rnp->gp_seq)) || !raw_spin_trylock_rcu_node(rnp))
		return;
	// The grace period cannot end while we hold the rcu_node lock.
	if (rcu_seq_state(rcu_seq_current(&rnp->gp_seq)))
		WARN_ON_ONCE(rcu_advance_cbs(rnp, rdp));
	raw_spin_unlock_rcu_node(rnp);
}

/*
 * In CONFIG_RCU_STRICT_GRACE_PERIOD=y kernels, attempt to generate a
 * quiescent state.  This is intended to be invoked when the CPU notices
 * a new grace period.
 */
static void rcu_strict_gp_check_qs(void)
{
	if (IS_ENABLED(CONFIG_RCU_STRICT_GRACE_PERIOD)) {
		rcu_read_lock();
		rcu_read_unlock();
	}
}

/*
 * Update CPU-local rcu_data state to record the beginnings and ends of
 * grace periods.  The caller must hold the ->lock of the leaf rcu_node
 * structure corresponding to the current CPU, and must have irqs disabled.
 * Returns true if the grace-period kthread needs to be awakened.
 */
static bool __note_gp_changes(struct rcu_node *rnp, struct rcu_data *rdp)
{
	bool ret = false;
	bool need_qs;
	const bool offloaded = rcu_rdp_is_offloaded(rdp);

	raw_lockdep_assert_held_rcu_node(rnp);

	if (rdp->gp_seq == rnp->gp_seq)
		return false; /* Nothing to do. */

	/* Handle the ends of any preceding grace periods first. */
	if (rcu_seq_completed_gp(rdp->gp_seq, rnp->gp_seq) ||
	    unlikely(READ_ONCE(rdp->gpwrap))) {
		if (!offloaded)
			ret = rcu_advance_cbs(rnp, rdp); /* Advance CBs. */
		rdp->core_needs_qs = false;
		trace_rcu_grace_period(rcu_state.name, rdp->gp_seq, TPS("cpuend"));
	} else {
		if (!offloaded)
			ret = rcu_accelerate_cbs(rnp, rdp); /* Recent CBs. */
		if (rdp->core_needs_qs)
			rdp->core_needs_qs = !!(rnp->qsmask & rdp->grpmask);
	}

	/* Now handle the beginnings of any new-to-this-CPU grace periods. */
	if (rcu_seq_new_gp(rdp->gp_seq, rnp->gp_seq) ||
	    unlikely(READ_ONCE(rdp->gpwrap))) {
		/*
		 * If the current grace period is waiting for this CPU,
		 * set up to detect a quiescent state, otherwise don't
		 * go looking for one.
		 */
		trace_rcu_grace_period(rcu_state.name, rnp->gp_seq, TPS("cpustart"));
		need_qs = !!(rnp->qsmask & rdp->grpmask);
		rdp->cpu_no_qs.b.norm = need_qs;
		rdp->core_needs_qs = need_qs;
		zero_cpu_stall_ticks(rdp);
	}
	rdp->gp_seq = rnp->gp_seq;  /* Remember new grace-period state. */
	if (ULONG_CMP_LT(rdp->gp_seq_needed, rnp->gp_seq_needed) || rdp->gpwrap)
		WRITE_ONCE(rdp->gp_seq_needed, rnp->gp_seq_needed);
	WRITE_ONCE(rdp->gpwrap, false);
	rcu_gpnum_ovf(rnp, rdp);
	return ret;
}

static void note_gp_changes(struct rcu_data *rdp)
{
	unsigned long flags;
	bool needwake;
	struct rcu_node *rnp;

	local_irq_save(flags);
	rnp = rdp->mynode;
	if ((rdp->gp_seq == rcu_seq_current(&rnp->gp_seq) &&
	     !unlikely(READ_ONCE(rdp->gpwrap))) || /* w/out lock. */
	    !raw_spin_trylock_rcu_node(rnp)) { /* irqs already off, so later. */
		local_irq_restore(flags);
		return;
	}
	needwake = __note_gp_changes(rnp, rdp);
	raw_spin_unlock_irqrestore_rcu_node(rnp, flags);
	rcu_strict_gp_check_qs();
	if (needwake)
		rcu_gp_kthread_wake();
}

static void rcu_gp_slow(int delay)
{
	if (delay > 0 &&
	    !(rcu_seq_ctr(rcu_state.gp_seq) %
	      (rcu_num_nodes * PER_RCU_NODE_PERIOD * delay)))
		schedule_timeout_idle(delay);
}

static unsigned long sleep_duration;

/* Allow rcutorture to stall the grace-period kthread. */
void rcu_gp_set_torture_wait(int duration)
{
	if (IS_ENABLED(CONFIG_RCU_TORTURE_TEST) && duration > 0)
		WRITE_ONCE(sleep_duration, duration);
}
EXPORT_SYMBOL_GPL(rcu_gp_set_torture_wait);

/* Actually implement the aforementioned wait. */
static void rcu_gp_torture_wait(void)
{
	unsigned long duration;

	if (!IS_ENABLED(CONFIG_RCU_TORTURE_TEST))
		return;
	duration = xchg(&sleep_duration, 0UL);
	if (duration > 0) {
		pr_alert("%s: Waiting %lu jiffies\n", __func__, duration);
		schedule_timeout_idle(duration);
		pr_alert("%s: Wait complete\n", __func__);
	}
}

/*
 * Handler for on_each_cpu() to invoke the target CPU's RCU core
 * processing.
 */
static void rcu_strict_gp_boundary(void *unused)
{
	invoke_rcu_core();
}

/*
 * Initialize a new grace period.  Return false if no grace period required.
 */
static noinline_for_stack bool rcu_gp_init(void)
{
	unsigned long flags;
	unsigned long oldmask;
	unsigned long mask;
	struct rcu_data *rdp;
	struct rcu_node *rnp = rcu_get_root();

	WRITE_ONCE(rcu_state.gp_activity, jiffies);
	raw_spin_lock_irq_rcu_node(rnp);
	if (!READ_ONCE(rcu_state.gp_flags)) {
		/* Spurious wakeup, tell caller to go back to sleep.  */
		raw_spin_unlock_irq_rcu_node(rnp);
		return false;
	}
	WRITE_ONCE(rcu_state.gp_flags, 0); /* Clear all flags: New GP. */

	if (WARN_ON_ONCE(rcu_gp_in_progress())) {
		/*
		 * Grace period already in progress, don't start another.
		 * Not supposed to be able to happen.
		 */
		raw_spin_unlock_irq_rcu_node(rnp);
		return false;
	}

	/* Advance to a new grace period and initialize state. */
	record_gp_stall_check_time();
	/* Record GP times before starting GP, hence rcu_seq_start(). */
	rcu_seq_start(&rcu_state.gp_seq);
	ASSERT_EXCLUSIVE_WRITER(rcu_state.gp_seq);
	trace_rcu_grace_period(rcu_state.name, rcu_state.gp_seq, TPS("start"));
	raw_spin_unlock_irq_rcu_node(rnp);

	/*
	 * Apply per-leaf buffered online and offline operations to
	 * the rcu_node tree. Note that this new grace period need not
	 * wait