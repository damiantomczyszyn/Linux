o another thread in
 * your thread group, or accelerate that thread toward the
 * processor it's on.
 * @p: target task
 * @preempt: whether task preemption is allowed or not
 *
 * It's the caller's job to ensure that the target task struct
 * can't go away on us before we can do any checks.
 *
 * Return:
 *	true (>0) if we indeed boosted the target task.
 *	false (0) if we failed to boost the target.
 *	-ESRCH if there's no task to yield to.
 */
int __sched yield_to(struct task_struct *p, bool preempt)
{
	struct task_struct *curr = current;
	struct rq *rq, *p_rq;
	unsigned long flags;
	int yielded = 0;

	local_irq_save(flags);
	rq = this_rq();

again:
	p_rq = task_rq(p);
	/*
	 * If we're the only runnable task on the rq and target rq also
	 * has only one task, there's absolutely no point in yielding.
	 */
	if (rq->nr_running == 1 && p_rq->nr_running == 1) {
		yielded = -ESRCH;
		goto out_irq;
	}

	double_rq_lock(rq, p_rq);
	if (task_rq(p) != p_rq) {
		double_rq_unlock(rq, p_rq);
		goto again;
	}

	if (!curr->sched_class->yield_to_task)
		goto out_unlock;

	if (curr->sched_class != p->sched_class)
		goto out_unlock;

	if (task_running(p_rq, p) || !task_is_running(p))
		goto out_unlock;

	yielded = curr->sched_class->yield_to_task(rq, p);
	if (yielded) {
		schedstat_inc(rq->yld_count);
		/*
		 * Make p's CPU reschedule; pick_next_entity takes care of
		 * fairness.
		 */
		if (preempt && rq != p_rq)
			resched_curr(p_rq);
	}

out_unlock:
	double_rq_unlock(rq, p_rq);
out_irq:
	local_irq_restore(flags);

	if (yielded > 0)
		schedule();

	return yielded;
}
EXPORT_SYMBOL_GPL(yield_to);

int io_schedule_prepare(void)
{
	int old_iowait = current->in_iowait;

	current->in_iowait = 1;
	blk_flush_plug(current->plug, true);
	return old_iowait;
}

void io_schedule_finish(int token)
{
	current->in_iowait = token;
}

/*
 * This task is about to go to sleep on IO. Increment rq->nr_iowait so
 * that process accounting knows that this is a task in IO wait state.
 */
long __sched io_schedule_timeout(long timeout)
{
	int token;
	long ret;

	token = io_schedule_prepare();
	ret = schedule_timeout(timeout);
	io_schedule_finish(token);

	return ret;
}
EXPORT_SYMBOL(io_schedule_timeout);

void __sched io_schedule(void)
{
	int token;

	token = io_schedule_prepare();
	schedule();
	io_schedule_finish(token);
}
EXPORT_SYMBOL(io_schedule);

/**
 * sys_sched_get_priority_max - return maximum RT priority.
 * @policy: scheduling class.
 *
 * Return: On success, this syscall returns the maximum
 * rt_priority that can be used by a given scheduling class.
 * On failure, a negative error code is returned.
 */
SYSCALL_DEFINE1(sched_get_priority_max, int, policy)
{
	int ret = -EINVAL;

	switch (policy) {
	case SCHED_FIFO:
	case SCHED_RR:
		ret = MAX_RT_PRIO-1;
		break;
	case SCHED_DEADLINE:
	case SCHED_NORMAL:
	case SCHED_BATCH:
	case SCHED_IDLE:
		ret = 0;
		break;
	}
	return ret;
}

/**
 * sys_sched_get_priority_min - return minimum RT priority.
 * @policy: scheduling class.
 *
 * Return: On success, this syscall returns the minimum
 * rt_priority that can be used by a given scheduling class.
 * On failure, a negative error code is returned.
 */
SYSCALL_DEFINE1(sched_get_priority_min, int, policy)
{
	int ret = -EINVAL;

	switch (policy) {
	case SCHED_FIFO:
	case SCHED_RR:
		ret = 1;
		break;
	case SCHED_DEADLINE:
	case SCHED_NORMAL:
	case SCHED_BATCH:
	case SCHED_IDLE:
		ret = 0;
	}
	return ret;
}

static int sched_rr_get_interval(pid_t pid, struct timespec64 *t)
{
	struct task_struct *p;
	unsigned int time_slice;
	struct rq_flags rf;
	struct rq *rq;
	int retval;

	if (pid < 0)
		return -EINVAL;

	retval = -ESRCH;
	rcu_read_lock();
	p = find_process_by_pid(pid);
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	rq = task_rq_lock(p, &rf);
	time_slice = 0;
	if (p->sched_class->get_rr_interval)
		time_slice = p->sched_class->get_rr_interval(rq, p);
	task_rq_unlock(rq, p, &rf);

	rcu_read_unlock();
	jiffies_to_timespec64(time_slice, t);
	return 0;

out_unlock:
	rcu_read_unlock();
	return retval;
}

/**
 * sys_sched_rr_get_interval - return the default timeslice of a process.
 * @pid: pid of the process.
 * @interval: userspace pointer to the timeslice value.
 *
 * this syscall writes the default timeslice value of a given process
 * into the user-space timespec buffer. A value of '0' means infinity.
 *
 * Return: On success, 0 and the timeslice is in @interval. Otherwise,
 * an error code.
 */
SYSCALL_DEFINE2(sched_rr_get_interval, pid_t, pid,
		struct __kernel_timespec __user *, interval)
{
	struct timespec64 t;
	int retval = sched_rr_get_interval(pid, &t);

	if (retval == 0)
		retval = put_timespec64(&t, interval);

	return retval;
}

#ifdef CONFIG_COMPAT_32BIT_TIME
SYSCALL_DEFINE2(sched_rr_get_interval_time32, pid_t, pid,
		struct old_timespec32 __user *, interval)
{
	struct timespec64 t;
	int retval = sched_rr_get_interval(pid, &t);

	if (retval == 0)
		retval = put_old_timespec32(&t, interval);
	return retval;
}
#endif

void sched_show_task(struct task_struct *p)
{
	unsigned long free = 0;
	int ppid;

	if (!try_get_task_stack(p))
		return;

	pr_info("task:%-15.15s state:%c", p->comm, task_state_to_char(p));

	if (task_is_running(p))
		pr_cont("  running task    ");
#ifdef CONFIG_DEBUG_STACK_USAGE
	free = stack_not_used(p);
#endif
	ppid = 0;
	rcu_read_lock();
	if (pid_alive(p))
		ppid = task_pid_nr(rcu_dereference(p->real_parent));
	rcu_read_unlock();
	pr_cont(" stack:%5lu pid:%5d ppid:%6d flags:0x%08lx\n",
		free, task_pid_nr(p), ppid,
		read_task_thread_flags(p));

	print_worker_info(KERN_INFO, p);
	print_stop_info(KERN_INFO, p);
	show_stack(p, NULL, KERN_INFO);
	put_task_stack(p);
}
EXPORT_SYMBOL_GPL(sched_show_task);

static inline bool
state_filter_match(unsigned long state_filter, struct task_struct *p)
{
	unsigned int state = READ_ONCE(p->__state);

	/* no filter, everything matches */
	if (!state_filter)
		return true;

	/* filter, but doesn't match */
	if (!(state & state_filter))
		return false;

	/*
	 * When looking for TASK_UNINTERRUPTIBLE skip TASK_IDLE (allows
	 * TASK_KILLABLE).
	 */
	if (state_filter == TASK_UNINTERRUPTIBLE && state == TASK_IDLE)
		return false;

	return true;
}


void show_state_filter(unsigned int state_filter)
{
	struct task_struct *g, *p;

	rcu_read_lock();
	for_each_process_thread(g, p) {
		/*
		 * reset the NMI-timeout, listing all files on a slow
		 * console might take a lot of time:
		 * Also, reset softlockup watchdogs on all CPUs, because
		 * another CPU might be blocked waiting for us to process
		 * an IPI.
		 */
		touch_nmi_watchdog();
		touch_all_softlockup_watchdogs();
		if (state_filter_match(state_filter, p))
			sched_show_task(p);
	}

#ifdef CONFIG_SCHED_DEBUG
	if (!state_filter)
		sysrq_sched_debug_show();
#endif
	rcu_read_unlock();
	/*
	 * Only show locks if all tasks are dumped:
	 */
	if (!state_filter)
		debug_show_all_locks();
}

/**
 * init_idle - set up an idle thread for a given CPU
 * @idle: task in question
 * @cpu: CPU the idle task belongs to
 *
 * NOTE: this function does not set the idle thread's NEED_RESCHED
 * flag, to make booting more robust.
 */
void __init init_idle(struct task_struct *idle, int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	unsigned long flags;

	__sched_fork(0, idle);

	raw_spin_lock_irqsave(&idle->pi_lock, flags);
	raw_spin_rq_lock(rq);

	idle->__state = TASK_RUNNING;
	idle->se.exec_start = sched_clock();
	/*
	 * PF_KTHREAD should already be set at this point; regardless, make it
	 * look like a proper per-CPU kthread.
	 */
	idle->flags |= PF_IDLE | PF_KTHREAD | PF_NO_SETAFFINITY;
	kthread_set_per_cpu(idle, cpu);

#ifdef CONFIG_SMP
	/*
	 * It's possible that init_idle() gets called multiple times on a task,
	 * in that case do_set_cpus_allowed() will not do the right thing.
	 *
	 * And since this is boot we can forgo the serialization.
	 */
	set_cpus_allowed_common(idle, cpumask_of(cpu), 0);
#endif
	/*
	 * We're having a chicken and egg problem, even though we are
	 * holding rq->lock, the CPU isn't yet set to this CPU so the
	 * lockdep check in task_group() will fail.
	 *
	 * Similar case to sched_fork(). / Alternatively we could
	 * use task_rq_lock() here and obtain the other rq->lock.
	 *
	 * Silence PROVE_RCU
	 */
	rcu_read_lock();
	__set_task_cpu(idle, cpu);
	rcu_read_unlock();

	rq->idle = idle;
	rcu_assign_pointer(rq->curr, idle);
	idle->on_rq = TASK_ON_RQ_QUEUED;
#ifdef CONFIG_SMP
	idle->on_cpu = 1;
#endif
	raw_spin_rq_unlock(rq);
	raw_spin_unlock_irqrestore(&idle->pi_lock, flags);

	/* Set the preempt count _outside_ the spinlocks! */
	init_idle_preempt_count(idle, cpu);

	/*
	 * The idle tasks have their own, simple scheduling class:
	 */
	idle->sched_class = &idle_sched_class;
	ftrace_graph_init_idle_task(idle, cpu);
	vtime_init_idle(idle, cpu);
#ifdef CONFIG_SMP
	sprintf(idle->comm, "%s/%d", INIT_TASK_COMM, cpu);
#endif
}

#ifdef CONFIG_SMP

int cpuset_cpumask_can_shrink(const struct cpumask *cur,
			      const struct cpumask *trial)
{
	int ret = 1;

	if (cpumask_empty(cur))
		return ret;

	ret = dl_cpuset_cpumask_can_shrink(cur, trial);

	return ret;
}

int task_can_attach(struct task_struct *p,
		    const struct cpumask *cs_cpus_allowed)
{
	int ret = 0;

	/*
	 * Kthreads which disallow setaffinity shouldn't be moved
	 * to a new cpuset; we don't want to change their CPU
	 * affinity and isolating such threads by their set of
	 * allowed nodes is unnecessary.  Thus, cpusets are not
	 * applicable for such threads.  This prevents checking for
	 * success of set_cpus_allowed_ptr() on all attached tasks
	 * before cpus_mask may be changed.
	 */
	if (p->flags & PF_NO_SETAFFINITY) {
		ret = -EINVAL;
		goto out;
	}

	if (dl_task(p) && !cpumask_intersects(task_rq(p)->rd->span,
					      cs_cpus_allowed)) {
		int cpu = cpumask_any_and(cpu_active_mask, cs_cpus_allowed);

		ret = dl_cpu_busy(cpu, p);
	}

out:
	return ret;
}

bool sched_smp_initialized __read_mostly;

#ifdef CONFIG_NUMA_BALANCING
/* Migrate current task p to target_cpu */
int migrate_task_to(struct task_struct *p, int target_cpu)
{
	struct migration_arg arg = { p, target_cpu };
	int curr_cpu = task_cpu(p);

	if (curr_cpu == target_cpu)
		return 0;

	if (!cpumask_test_cpu(target_cpu, p->cpus_ptr))
		return -EINVAL;

	/* TODO: This is not properly updating schedstats */

	trace_sched_move_numa(p, curr_cpu, target_cpu);
	return stop_one_cpu(curr_cpu, migration_cpu_stop, &arg);
}

/*
 * Requeue a task on a given node and accurately track the number of NUMA
 * tasks on the runqueues
 */
void sched_setnuma(struct task_struct *p, int nid)
{
	bool queued, running;
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(p, &rf);
	queued = task_on_rq_queued(p);
	running = task_current(rq, p);

	if (queued)
		dequeue_task(rq, p, DEQUEUE_SAVE);
	if (running)
		put_prev_task(rq, p);

	p->numa_preferred_nid = nid;

	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE | ENQUEUE_NOCLOCK);
	if (running)
		set_next_task(rq, p);
	task_rq_unlock(rq, p, &rf);
}
#endif /* CONFIG_NUMA_BALANCING */

#ifdef CONFIG_HOTPLUG_CPU
/*
 * Ensure that the idle task is using init_mm right before its CPU goes
 * offline.
 */
void idle_task_exit(void)
{
	struct mm_struct *mm = current->active_mm;

	BUG_ON(cpu_online(smp_processor_id()));
	BUG_ON(current != this_rq()->idle);

	if (mm != &init_mm) {
		switch_mm(mm, &init_mm, current);
		finish_arch_post_lock_switch();
	}

	/* finish_cpu(), as ran on the BP, will clean up the active_mm state */
}

static int __balance_push_cpu_stop(void *arg)
{
	struct task_struct *p = arg;
	struct rq *rq = this_rq();
	struct rq_flags rf;
	int cpu;

	raw_spin_lock_irq(&p->pi_lock);
	rq_lock(rq, &rf);

	update_rq_clock(rq);

	if (task_rq(p) == rq && task_on_rq_queued(p)) {
		cpu = select_fallback_rq(rq->cpu, p);
		rq = __migrate_task(rq, &rf, p, cpu);
	}

	rq_unlock(rq, &rf);
	raw_spin_unlock_irq(&p->pi_lock);

	put_task_struct(p);

	return 0;
}

static DEFINE_PER_CPU(struct cpu_stop_work, push_work);

/*
 * Ensure we only run per-cpu kthreads once the CPU goes !active.
 *
 * This is enabled below SCHED_AP_ACTIVE; when !cpu_active(), but only
 * effective when the hotplug motion is down.
 */
static void balance_push(struct rq *rq)
{
	struct task_struct *push_task = rq->curr;

	lockdep_assert_rq_held(rq);

	/*
	 * Ensure the thing is persistent until balance_push_set(.on = false);
	 */
	rq->balance_callback = &balance_push_callback;

	/*
	 * Only active while going offline and when invoked on the outgoing
	 * CPU.
	 */
	if (!cpu_dying(rq->cpu) || rq != this_rq())
		return;

	/*
	 * Both the cpu-hotplug and stop task are in this case and are
	 * required to complete the hotplug process.
	 */
	if (kthread_is_per_cpu(push_task) ||
	    is_migration_disabled(push_task)) {

		/*
		 * If this is the idle task on the outgoing CPU try to wake
		 * up the hotplug control thread which might wait for the
		 * last task to vanish. The rcuwait_active() check is
		 * accurate here because the waiter is pinned on this CPU
		 * and can't obviously be running in parallel.
		 *
		 * On RT kernels this also has to check whether there are
		 * pinned and scheduled out tasks on the runqueue. They
		 * need to leave the migrate disabled section first.
		 */
		if (!rq->nr_running && !rq_has_pinned_tasks(rq) &&
		    rcuwait_active(&rq->hotplug_wait)) {
			raw_spin_rq_unlock(rq);
			rcuwait_wake_up(&rq->hotplug_wait);
			raw_spin_rq_lock(rq);
		}
		return;
	}

	get_task_struct(push_task);
	/*
	 * Temporarily drop rq->lock such that we can wake-up the stop task.
	 * Both preemption and IRQs are still disabled.
	 */
	raw_spin_rq_unlock(rq);
	stop_one_cpu_nowait(rq->cpu, __balance_push_cpu_stop, push_task,
			    this_cpu_ptr(&push_work));
	/*
	 * At this point need_resched() is true and we'll take the loop in
	 * schedule(). The next pick is obviously going to be the stop task
	 * which kthread_is_per_cpu() and will push this task away.
	 */
	raw_spin_rq_lock(rq);
}

static void balance_push_set(int cpu, bool on)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	rq_lock_irqsave(rq, &rf);
	if (on) {
		WARN_ON_ONCE(rq->balance_callback);
		rq->balance_callback = &balance_push_callback;
	} else if (rq->balance_callback == &balance_push_callback) {
		rq->balance_callback = NULL;
	}
	rq_unlock_irqrestore(rq, &rf);
}

/*
 * Invoked from a CPUs hotplug control thread after the CPU has been marked
 * inactive. All tasks which are not per CPU kernel threads are either
 * pushed off this CPU now via balance_push() or placed on a different CPU
 * during wakeup. Wait until the CPU is quiescent.
 */
static void balance_hotplug_wait(void)
{
	struct rq *rq = this_rq();

	rcuwait_wait_event(&rq->hotplug_wait,
			   rq->nr_running == 1 && !rq_has_pinned_tasks(rq),
			   TASK_UNINTERRUPTIBLE);
}

#else

static inline void balance_push(struct rq *rq)
{
}

static inline void balance_push_set(int cpu, bool on)
{
}

static inline void balance_hotplug_wait(void)
{
}

#endif /* CONFIG_HOTPLUG_CPU */

void set_rq_online(struct rq *rq)
{
	if (!rq->online) {
		const struct sched_class *class;

		cpumask_set_cpu(rq->cpu, rq->rd->online);
		rq->online = 1;

		for_each_class(class) {
			if (class->rq_online)
				class->rq_online(rq);
		}
	}
}

void set_rq_offline(struct rq *rq)
{
	if (rq->online) {
		const struct sched_class *class;

		for_each_class(class) {
			if (class->rq_offline)
				class->rq_offline(rq);
		}

		cpumask_clear_cpu(rq->cpu, rq->rd->online);
		rq->online = 0;
	}
}

/*
 * used to mark begin/end of suspend/resume:
 */
static int num_cpus_frozen;

/*
 * Update cpusets according to cpu_active mask.  If cpusets are
 * disabled, cpuset_update_active_cpus() becomes a simple wrapper
 * around partition_sched_domains().
 *
 * If we come here as part of a suspend/resume, don't touch cpusets because we
 * want to restore it back to its original state upon resume anyway.
 */
static void cpuset_cpu_active(void)
{
	if (cpuhp_tasks_frozen) {
		/*
		 * num_cpus_frozen tracks how many CPUs are involved in suspend
		 * resume sequence. As long as this is not the last online
		 * operation in the resume sequence, just build a single sched
		 * domain, ignoring cpusets.
		 */
		partition_sched_domains(1, NULL, NULL);
		if (--num_cpus_frozen)
			return;
		/*
		 * This is the last CPU online operation. So fall through and
		 * restore the original sched domains by considering the
		 * cpuset configurations.
		 */
		cpuset_force_rebuild();
	}
	cpuset_update_active_cpus();
}

static int cpuset_cpu_inactive(unsigned int cpu)
{
	if (!cpuhp_tasks_frozen) {
		int ret = dl_cpu_busy(cpu, NULL);

		if (ret)
			return ret;
		cpuset_update_active_cpus();
	} else {
		num_cpus_frozen++;
		partition_sched_domains(1, NULL, NULL);
	}
	return 0;
}

int sched_cpu_activate(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	/*
	 * Clear the balance_push callback and prepare to schedule
	 * regular tasks.
	 */
	balance_push_set(cpu, false);

#ifdef CONFIG_SCHED_SMT
	/*
	 * When going up, increment the number of cores with SMT present.
	 */
	if (cpumask_weight(cpu_smt_mask(cpu)) == 2)
		static_branch_inc_cpuslocked(&sched_smt_present);
#endif
	set_cpu_active(cpu, true);

	if (sched_smp_initialized) {
		sched_update_numa(cpu, true);
		sched_domains_numa_masks_set(cpu);
		cpuset_cpu_active();
	}

	/*
	 * Put the rq online, if not already. This happens:
	 *
	 * 1) In the early boot process, because we build the real domains
	 *    after all CPUs have been brought up.
	 *
	 * 2) At runtime, if cpuset_cpu_active() fails to rebuild the
	 *    domains.
	 */
	rq_lock_irqsave(rq, &rf);
	if (rq->rd) {
		BUG_ON(!cpumask_test_cpu(cpu, rq->rd->span));
		set_rq_online(rq);
	}
	rq_unlock_irqrestore(rq, &rf);

	return 0;
}

int sched_cpu_deactivate(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;
	int ret;

	/*
	 * Remove CPU from nohz.idle_cpus_mask to prevent participating in
	 * load balancing when not active
	 */
	nohz_balance_exit_idle(rq);

	set_cpu_active(cpu, false);

	/*
	 * From this point forward, this CPU will refuse to run any task that
	 * is not: migrate_disable() or KTHREAD_IS_PER_CPU, and will actively
	 * push those tasks away until this gets cleared, see
	 * sched_cpu_dying().
	 */
	balance_push_set(cpu, true);

	/*
	 * We've cleared cpu_active_mask / set balance_push, wait for all
	 * preempt-disabled and RCU users of this state to go away such that
	 * all new such users will observe it.
	 *
	 * Specifically, we rely on ttwu to no longer target this CPU, see
	 * ttwu_queue_cond() and is_cpu_allowed().
	 *
	 * Do sync before park smpboot threads to take care the rcu boost case.
	 */
	synchronize_rcu();

	rq_lock_irqsave(rq, &rf);
	if (rq->rd) {
		update_rq_clock(rq);
		BUG_ON(!cpumask_test_cpu(cpu, rq->rd->span));
		set_rq_offline(rq);
	}
	rq_unlock_irqrestore(rq, &rf);

#ifdef CONFIG_SCHED_SMT
	/*
	 * When going down, decrement the number of cores with SMT present.
	 */
	if (cpumask_weight(cpu_smt_mask(cpu)) == 2)
		static_branch_dec_cpuslocked(&sched_smt_present);

	sched_core_cpu_deactivate(cpu);
#endif

	if (!sched_smp_initialized)
		return 0;

	sched_update_numa(cpu, false);
	ret = cpuset_cpu_inactive(cpu);
	if (ret) {
		balance_push_set(cpu, false);
		set_cpu_active(cpu, true);
		sched_update_numa(cpu, true);
		return ret;
	}
	sched_domains_numa_masks_clear(cpu);
	return 0;
}

static void sched_rq_cpu_starting(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	rq->calc_load_update = calc_load_update;
	update_max_interval();
}

int sched_cpu_starting(unsigned int cpu)
{
	sched_core_cpu_starting(cpu);
	sched_rq_cpu_starting(cpu);
	sched_tick_start(cpu);
	return 0;
}

#ifdef CONFIG_HOTPLUG_CPU

/*
 * Invoked immediately before the stopper thread is invoked to bring the
 * CPU down completely. At this point all per CPU kthreads except the
 * hotplug thread (current) and the stopper thread (inactive) have been
 * either parked or have been unbound from the outgoing CPU. Ensure that
 * any of those which might be on the way out are gone.
 *
 * If after this point a bound task is being woken on this CPU then the
 * responsible hotplug callback has failed to do it's job.
 * sched_cpu_dying() will catch it with the appropriate fireworks.
 */
int sched_cpu_wait_empty(unsigned int cpu)
{
	balance_hotplug_wait();
	return 0;
}

/*
 * Since this CPU is going 'away' for a while, fold any nr_active delta we
 * might have. Called from the CPU stopper task after ensuring that the
 * stopper is the last running task on the CPU, so nr_active count is
 * stable. We need to take the teardown thread which is calling this into
 * account, so we hand in adjust = 1 to the load calculation.
 *
 * Also see the comment "Global load-average calculations".
 */
static void calc_load_migrate(struct rq *rq)
{
	long delta = calc_load_fold_active(rq, 1);

	if (delta)
		atomic_long_add(delta, &calc_load_tasks);
}

static void dump_rq_tasks(struct rq *rq, const char *loglvl)
{
	struct task_struct *g, *p;
	int cpu = cpu_of(rq);

	lockdep_assert_rq_held(rq);

	printk("%sCPU%d enqueued tasks (%u total):\n", loglvl, cpu, rq->nr_running);
	for_each_process_thread(g, p) {
		if (task_cpu(p) != cpu)
			continue;

		if (!task_on_rq_queued(p))
			continue;

		printk("%s\tpid: %d, name: %s\n", loglvl, p->pid, p->comm);
	}
}

int sched_cpu_dying(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	/* Handle pending wakeups and then migrate everything off */
	sched_tick_stop(cpu);

	rq_lock_irqsave(rq, &rf);
	if (rq->nr_running != 1 || rq_has_pinned_tasks(rq)) {
		WARN(true, "Dying CPU not properly vacated!");
		dump_rq_tasks(rq, KERN_WARNING);
	}
	rq_unlock_irqrestore(rq, &rf);

	calc_load_migrate(rq);
	update_max_interval();
	hrtick_clear(rq);
	sched_core_cpu_dying(cpu);
	return 0;
}
#endif

void __init sched_init_smp(void)
{
	sched_init_numa(NUMA_NO_NODE);

	/*
	 * There's no userspace yet to cause hotplug operations; hence all the
	 * CPU masks are stable and all blatant races in the below code cannot
	 * happen.
	 */
	mutex_lock(&sched_domains_mutex);
	sched_init_domains(cpu_active_mask);
	mutex_unlock(&sched_domains_mutex);

	/* Move init over to a non-isolated CPU */
	if (set_cpus_allowed_ptr(current, housekeeping_cpumask(HK_TYPE_DOMAIN)) < 0)
		BUG();
	current->flags &= ~PF_NO_SETAFFINITY;
	sched_init_granularity();

	init_sched_rt_class();
	init_sched_dl_class();

	sched_smp_initialized = true;
}

static int __init migration_init(void)
{
	sched_cpu_starting(smp_processor_id());
	return 0;
}
early_initcall(migration_init);

#else
void __init sched_init_smp(void)
{
	sched_init_granularity();
}
#endif /* CONFIG_SMP */

int in_sched_functions(unsigned long addr)
{
	return in_lock_functions(addr) ||
		(addr >= (unsigned long)__sched_text_start
		&& addr < (unsigned long)__sched_text_end);
}

#ifdef CONFIG_CGROUP_SCHED
/*
 * Default task group.
 * Every task in system belongs to this group at bootup.
 */
struct task_group root_task_group;
LIST_HEAD(task_groups);

/* Cacheline aligned slab cache for task_group */
static struct kmem_cache *task_group_cache __read_mostly;
#endif

DECLARE_PER_CPU(cpumask_var_t, load_balance_mask);
DECLARE_PER_CPU(cpumask_var_t, select_idle_mask);

void __init sched_init(void)
{
	unsigned long ptr = 0;
	int i;

	/* Make sure the linker didn't screw up */
	BUG_ON(&idle_sched_class + 1 != &fair_sched_class ||
	       &fair_sched_class + 1 != &rt_sched_class ||
	       &rt_sched_class + 1   != &dl_sched_class);
#ifdef CONFIG_SMP
	BUG_ON(&dl_sched_class + 1 != &stop_sched_class);
#endif

	wait_bit_init();

#ifdef CONFIG_FAIR_GROUP_SCHED
	ptr += 2 * nr_cpu_ids * sizeof(void **);
#endif
#ifdef CONFIG_RT_GROUP_SCHED
	ptr += 2 * nr_cpu_ids * sizeof(void **);
#endif
	if (ptr) {
		ptr = (unsigned long)kzalloc(ptr, GFP_NOWAIT);

#ifdef CONFIG_FAIR_GROUP_SCHED
		root_task_group.se = (struct sched_entity **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.cfs_rq = (struct cfs_rq **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.shares = ROOT_TASK_GROUP_LOAD;
		init_cfs_bandwidth(&root_task_group.cfs_bandwidth);
#endif /* CONFIG_FAIR_GROUP_SCHED */
#ifdef CONFIG_RT_GROUP_SCHED
		root_task_group.rt_se = (struct sched_rt_entity **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.rt_rq = (struct rt_rq **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

#endif /* CONFIG_RT_GROUP_SCHED */
	}
#ifdef CONFIG_CPUMASK_OFFSTACK
	for_each_possible_cpu(i) {
		per_cpu(load_balance_mask, i) = (cpumask_var_t)kzalloc_node(
			cpumask_size(),3›·TŸq”zpªåDıömTIêr?âüÎ›ñ‚;·ïz¿ã—„yûçr¹”E,f7P$	l"Àx{¾¥È°íu®8î³¤KÔ^, ÷•;9ë˜rîQ‡8Ån¼‡”¬ıPp¾±Ç™’ëpƒ¨Ø„~_è/éSÇñõÆı<qøJP,Ğ'BŞ»º#İ…·2Êü]?h~÷IÂ›ÅLÇ(¶ã­‘’±,o›";É3,@6¹ŠÎÿkxÏƒ¡€Ä”.
h€lUdÒ#€ Š¡Â/P	PA#ƒäÖl½ô{şŒ‘=â¨ÃÓ(.ßğSi2*ë<„èÆüºZ`ù Tz€ò)¸-àdf1İrö ŠŠè³ìÃP”Š‰ÇycíGgå±¬-0ù??œ§¸—Áty<²ÙtÈ5!àÊc`¨ ¶±YÈÕo°Çı©œ9ƒ¶cE/HrnÑ{(vè¹H@¹€6›–©ùéc¸'„›r¤'¼¬^Ú»Ò*¯lL}ÖÑŸ&#ÆŞ7¯¨Rğ˜
M¼³|‡y¾Ÿ'ÆÓU;‰ˆLÓ*MÜóªœx¡æ"ÆnÆû+Naƒ3-İ»O
ÖÓ/ÙÒÊÿ
Èa
İW÷‘ì;ÁÅn(F^€›yæĞı»æ;O÷îNÁx1zqÔ¶Éƒ¿’dfnû¤™ÍÖó?ËšmrNŸ4spk¸ $HYß†<R¹¤nnrØğ¿\æä9ä2Sß¡"˜kâX\İ„óä>ˆn“«”6ƒ°ÈK2ç×”Ò(R¶î\Q2 iêéO>Ÿ¤µ2FiC;Š.{3¥ÀÒ!Ë(ğ4˜— c*h¾ÂJŠQß^*ÖÕq(¤bÀ,"pÆÜm¡©ôpë‚
M”<¾uï ©GIìFV¶&Q&»OÌá’ô$¹Å9D—Š.E­Éc†‚Ï¼úóğÌ4ŞzE&†	şùgàÖ¸ÔÀò,[Su÷Åğ^ñR±¹}J˜*S_ #Õ›ÀÆĞ³ã-©
¿cv?íŠ¸3ğ´5äRÎ‚@ÃÌ3æ#sœœ0+22±óà©)§Ûu#í;œ«•Çâ‚a(ÃÆqÀoÜA{) XN+7kV[ĞçÍãÎÔ—A‰¶2a!‰ÿj˜›¬‰¾VŸ+„À¹_YÅÅ¨)¾°d2ø¹Š^Ÿ´©árølTò4ÜÅvQp‡_çÕşv»hşYHÓ²®®ñEkáº=vrÇï’â!BSæL?ñâié@Ådë>L?ğxö“¬mÇõOúDÜêöB/'a å)­É§‚WÏ§Ø?w/Ñ‡R2ÀÉÑù¤S	ş‚áKLX­T8±ó|À‡°íU§º^F‘ÚLY¼O–¸áœúÓˆ­Ä‚oÎ5TUÇ¿İºÁ«Læa#Ó T˜±@½Ö#´Ëæ½<,Aõ+wyKçkß¡Yóæ5·¼p?œT—[ÿõÏİàÀ¶ÜÑOú7Qåã~â+„‰ƒ!%Í (yHŸS¥gÚ4ò˜ê¥1^œèMü:›İB¬é
F‚Lú$™ì‚Gt<¦BãÙZìªïn^¦¿8U~Æ[™¯Å'5
0ÓCs_ÙãUb¾’ÕÊÉ}KÖ…ŞË}Br+3õ2Ç—€p+…àÏ]-ŒÏµ÷qÚ˜ÌÚEr¥ïr"ôúË¾ª¼MrMkÍÅ
‡óÿ ıı#&düan4ƒ™Z¬#OòšÖÑüTôkŞ»¶Œ×G@)bkiYa º®X•ì&‡ÈÙjŸ\–Ó­ôŞ+¥?¯KoÛÿéwlñ©…5D—¢KÄŞPı÷:´èd™óåÖY~Å¬6¤Q~_|½çÅo°ĞK@BT-2êòÒ£Îâx«9g€Eó…Äùq&Tzö	(¸ øÊÈİ,JĞ‚ä"¿5ç‹x€À¬_ìQN±€Îºä:¿=ï–ZşY¬Ö,óëk'©¤"S¹)õíH`E¨™îJ Éü‰Ò€0™,D}v˜¹HzMDß¸è€QØÖgñíTfGtY|¦)*=.láÿXPä›ÿûæÑ…O'LĞyi>İUR£¾„èâğ­/šrz¡«—­ßåÏ…F%•/À>må·™&}&¢Èàğ	Í9±´²ƒF*:Aw\kÒU±^˜ïM“<ë×	²û¤¨ ’ä»y¶ÿ™¦1‰I—T{‚”dN¼ÇİÿæıøŞ~eÿ>>“\ƒ›ÖÉÃ6†ejgn‚­tÜgÙÆŒıáíàe^÷ºêîš«5®»½Nˆßå³‰.ş+üŞQØLèCÉ;
†O–“Ñë4ÛøEë1p„ú‰MulZŞQÊË„ÀVÓç¸ƒd
¼ñøÒƒÎ—¶`E“ºTÉ×Õ˜Y,æÌ‚k›¼ƒ/ÎsÕm#Ø­˜· ò9öAv”v˜ƒ=ñS® cPzßœTTŠ>Bò,bĞì´¶5ÓQm¥síäwyW£r7?!Ê“oxrÙeRŒB>æqæJ5
›=bµ¹¼6ˆNõpµY8éâÕF¾ÂöEYô6Àqx¡HˆTŠt‡_::Åd¸ kî¯êÎW£&ÅlUé<8”Äğ7xöµv6¾ä‚
©hL¨Gø5æËy’³ÿÌo„¾D=¾Z-{-áİi©7,ÉÄù‹S´92$ªÌt¼n«ËFôşÔly3!@À5ÃÙ¨% &íhë¾ÎÍ3Â{²Ï.…îÎÑ­9RÊT o%yq^c&cÇoÂÌz£n<aÉÿáœ¶ÉH<:Æü:rÈî7Û]7¡>ÂÆpö¾®wXÜ­ÀĞU2VÓ±İXõaÃ$ü†#2?Ö<	&"}z²Š?h¶‚¤ú}ö®:èUèÌÈf³]}[>-:>ME ‘îÀï}¹#ñ}ÈğålŠÅ†âP`qVş“ˆ>ß‘”Û¿‰Z•¯ù6_sEó¡Æ\42™—É$’ ğJ4cIš½N
ø³&fYKJ¡ qÍµ<öïäfÏğB_„KvÅezymJ:šZ«ÍüUzzIxÚ®ÅÈ7ğñÃÅöwKB_°—¨ş‰#ãsL\<5PD/î@ûå-/ªÆŸÒ6_Ëë›98|6ŸHcö]ñ™WÃ¹ÔwÖ'ö©Z9 †äª¶`uä.úøJ)]‹
1ì;/®yN5.Gp¸{b+« *,Í×³–.vÀ|˜é|«êIÂ™[.wµã`’(‡<>vpeá\n:ìS¤q¯ĞğÈLÀ¨®H
Œá»uKqb’¿.%.¯WÇéæ¢¼ıdÔŠl˜_ıütÙÊŒ²ê¹øµ»)wÉLÓˆñ§Ä(ÔxŞo?ËP‹4Í6öØuUæÙí»kun±m;¼°±å &\lş«']ë–0¶öSıd`m•XPà,p1ÊQ£ÆÛí‰:€ÚW;}ÅúŸv¯2¦é…^¶Âce
¿o³‹Œ³¨ù[) uÂÇ©QVÈ‘òÙ¶—½¾FÜ©¨;5ª$yf‘öUB¹ôŒ‘oD+ıÈ0@$‰@Š;˜øáD¹B@x¾3.¢]É>f-‡e—%¤T{êÉ¶ÅÅ—eşó*êa˜Ø„˜¹¼+a5ğ¶6óLoƒ8ûz_aESøÊÿÂZ6æå.T #ûCUŠ*Š
^¯¯W¯µ=ïÁAø(OĞ%ú‰EÏióø ZìÄòÊ&`¡ï Š«ïv,@]¥v‚»^Uæ="zM¨;úTùß¡k2Ä¸5ÚâK6Œåô	¹¹u>ºxˆc¹$–C³×1)bë5 aş‚ñêé9á.Ë ŠrÜ'„5eÂ<(,Œ¢¨~uºÇæÏBrëÑsf/xáö†.È?ôLCbo¶œÙJAÄ7¿s}$eQˆ·Næ:H¶ªÂƒÄˆş^k¿~79Û@kæsvÈ{3ëÉ‡yêTMª6uñÍô·¹,]ÙER+cûŠ+@³a ÕUõPÅ„<è“
oe½"su«ı@S‰ÆÛıÜu»>6†iy³tÈKiØ(+aòYöwÊWFÖ©òrŞi*,(ã)¾Eòøñ/!B¤¡zïóøŠ‰³¼åæ¹`Ê×æmqYD\3Å

‹AÚ>¶l‹äVZ;å"ôÎBü_”¶ZÃ>ôØV¬@ÿ}Ë½GÒ0S†Û§ÂÙQK,¡û-cŒ[ì Œrw(	ãÉv·Ó†~ nì†®nÔk‘¡ĞŠÿ»ñf)ón˜ÜÀ[1Õ/z|ïĞâb°:^°‘z"¯ôT¿GØÒ¹Ú¹¥8a:?h±Îœy½ƒÌ¨qì2áğ÷Iä N+ÆÉÔ€!ûÃÂ>MvFcÍF‚\ƒI¿'bÇ·hkjŸjî¬s7ÇOİ$O%à@ìstKhßx-’ÌòM›î×kX¨Ì}˜/2¨âì@û‘ÅÁÁ‡ç*W@¸LDóëÔ™Epä«0}†+¯w2LpöÈ2¿Pà,*w}RèÕ>İ¯°áŒ“$ÂÙüÉæÉ ¨)ñE=|tÓjé¿ilr&£0¡8_<9c"Ú=(§‚O!ç£JRN[£¶ğ²²úk#×Ş-ÏÕ¾“Zª²¤áÁP¹“Ìƒ9|·›7D°¤`(×p=ÎqšŸàÏ$2tvŞ£Ë´2r.ï}øú Tëâk<-ÔUzƒ®7‘ğ[¿ªÿım„@X	×Cnº´²nŞóq`})èéú„†ÚåwŞkW™¥¤juÃ¨¶¢¼oÊi1ëH6DèUB‡ú›Ø±Üd_§ŞtÛ¦­ŞçÌZs]¯6gÌ²ï.Ô¿R*‚Në]ŞØ0lR¿vÂ'$>k6åb¡Ï79C´5&1Š¼É?›°©Äa„}‰€Khæ
  -Åó§(Ü,ø#jv0^l–ù•ñ†¾i›'H0’.r#çTXgÃ‰Ó (4NŒ­qZ»V½£4–¬¡I›¸|nöº3ÀO·HQÇvƒ^J®üy)Ù–÷Ğ€xXô¤G|;ÏTĞö@­µÁ³«DU‰˜Õ‘˜r¡ûJÂJoSü¦Zùò,eßÎúdşÜ’‰ÌdÒ=*-Ğ–ØÖqº¤yPßã†-„…Rû…û8
T>?xZÎ–ûìäçFµ%\®NC÷VA×m¼òë×À[Y—,æÄä¶7ÂîW ößjÓköRçàµ®ãÜéÇEŸk>,,Qy·äû!_: ©.ºÄC[k¶üXf*qN¯RïÁP‚‡N
­0Š£n÷ÚtCáD›ı†AÚ5	}V’:ÜPº¦ñŠ¼¾¸jí¨x/¾ç5O‘ë\SŒ‡Ç‘%ÍŞà e£9»4g¢µ0µÑu’×"W´dLO¶¹sŠŒI5%î9^İ­2×b2ÃpGÅ	­ØŸdÚ(¹	ò„WNäÓ,$óäsèóµú/T‘Ÿéä…~*ùÀj&†EÊhKìVÊ3¸qpöGÙitF«	´Œ;‘¹-_Qˆoõıšm xÓ¡;Ÿhõ¾¢ƒÄY4\ƒÈú‚°ü%»€	×eL~7-SÄäı»ÒÓğÕ.ğå\‘ÿÍ“Ù¨£§úÀ‚`hc›£GÍ¢–c5şÕ¦C­F÷RõZ+ÆÓYó1@†ı£9ŞefxÒÃŒOå-òåHüüDåe/V£sT³f„Çêæá3ws‘Î¯ğ³P³£«]—^_ZÁ¹Ñ†!¾İŠü¥¦ÆÀš^œÒÿ§A‡pL8˜Alÿ=“Ÿ<>bnÇYÕ—³³7Ssn6áíí‘ÇËôó#^ïdcµm›‡LW2Ş|æ3lÃX^6S€5ñ:|úš[Â>H1À]ì Ë²oáMKM(/E.ÿ$xÜeÑ)O]h\èò{_±á#?Ú€HóIsR»
8/Ìj7¹ÎE"s<¸Âlœ­ß2jº”)ÒG”e„=ßùjì²ˆ®ÖĞÌf!4)Wn$WE;h¸ßİ¢²
”:qM8C’’H²#ÎæôUIîˆ°ädº„cÁ9jÒ”ğ!Û7‘4)íïñ¦ğE¸áş«üÖşd8jU¤üîºbsÄ¶je§$˜`¯ŞòCe˜<Ò=ÖŸ&ßÒMhŸ•?nÔ¢B¢Wœ!ÿßqZEæ «ôó)$s—ì’qH¶X ïÓ‹Ûı5k˜¦-Äm¼ÊÜ"Ş'K@àõ=»ıvàÂ§éGËÿ¢u•îøş´¾ªñGíÎİ¯:ÆıßŸ‚]š¥yO
Ó”ÿã½ß:91e“¤1&E47«· Ç‹) fïêî’°–Şò|ßSB²ñş#Õ=S”÷QÉ„‹qhş6jK;´DŞŸ"„”"@ş€D;üìÙq¾£-ªã£õK|!•ÍHó$+ª6ÕTò‡ìÍX›XUÒYÀ““…3ëHE÷Wı¬Ék€¿!T>ëJ’ç/ò4^gıöe&D¹Sçø$eU#¸å;.ã`ş–­é*{<Ü½'™¬¼O¤4¶õìgÌ·WPÕÍ2Ì˜&álû`|ä5”"r`5Ë+<¶'ÅñÚ‚]ÕHŞÒnÍ®mâıa°éZ‡µ =-˜¹ñuLl;éf7Ê±xsıÇqOÈº'Ö9¶*ŞAÊ½ê>ZQŸ{Ê4Åt†ßPÙµ—–ÃÆÕÀÙ(^³HL–ÈuÁfš… Íû;[´X;³–dË·İM×@õf7ÒÓŠ?C:;ˆrŒ|!Õ¯ä
xÑ‰^Ø¡Š¬êÆë÷¬i3	 Ô¸˜@<È>£œúg£Â}2ä&› ™˜`ÑQLû k29&ÂyàÂ¾;ò
& ‘7;6¦€§ÚßKĞóÿš¨&ŸQ6”({)Â¦§bkÖ:¥ªº«¦Ü<gÁó××‡‘wÈj~Øl'Ç«3óĞ²ìÕ_…e’²~Tº°ú¢Uå_ô»x­RJJÍı¬Ió½İ^#4‰V˜9Gü¨ÃÒx²{»0ˆ^d‚LÛqOªë^äÇd`>¥Õ†$'°ŒÄ„hLçá( ÁÁª707' moæ)EùC–¸P+w„@ÉéT“ îÚ_€…Ó+©‘…V²¿V©Ü-êÊWLÀ~OíÀ{R	èz6ısFoR‚Ûç]Ô3^³'ı¼(¡:5«Vd]l£„ˆ0‚ÕÆK_;Já;‚qìÖ WÆ»y¨‡™Ç\IÒt×råú”œo—”50µ¶dŞî÷ïÒ^º±°ƒU§.äÃ?‘fæ0û	†TÀãAæ?‹wHl: À‚ÎBŠ9Œ–:[]ññ`ÅŒ¹˜|qôm]Í'RÛ@71÷œ7HO¼*ä+R}*EM4°Âó%P-ÜW™,Ë¤Á*_™””uµNs…UÄijL‡¢¢Ç}ñFN¯›] ÿvfŞ°k&_QÂÇ†µDs§?óµìtÁÅ²)¨œ¦ÿ1<¯–ênæ»éÂÂÈ©ÈQĞ0¬H8/ì’\ò©¼£fõĞgõª„rmø¢må«z›—u”œñ>¾ÀÈQ’OãÄ¦ĞÑãRì¹åÆşb›g'C.‡"{õM€rA_°Of¬¢×È#îƒä»Õµeo•¥Ñœ>Ç¯»,Qq;hyéÌ(aÌğóo-$šÏî£bß%Ôé‚Ò‘-†Í­÷-´ğ|ùE2cfá¥eİŸ„-È
EÀ·0äìoq7«7„3,÷‚ €Q»½e€E¥ºóT¿uA$Ò§c×k?\Vıëÿøé‰2Ò´åğ4şÛüÇ€šèÎCåâ=Ê;(¹Ö¾­0Y¹‘§Ûø'•5|¦y7ŒÙÕOÅyˆ àr›&EŒè’‚óæ«ûkîĞq–ı^9ûyñ8‹‘D`ÿi°	`$`wmrÍ.¤œnx4sAèv¦şÓªö•Eu
¬!ı!«¡I¾Ç:¾„ Y®É Éu¾4EÈ¦uÅ/àÿ,Á¿UURÊ¼[Ç‘†)Eø/1üîÀİ&5¹Á-é±-'ª™å‹1Àé|ú2ÙPŸbáøæÜ¿¦•„ùi_"À6¥Îôü(ü›7ºâ­xÇ´D¿wÄSíQü·#uN}Cûa»]ø‹¡øÔ¥´°èI˜E¶öû\ıfÆ2:XØ¶Œ)Ò2äÙNÅ/ÓíŸà}Î¬1€ê¤›Ë"©µ¤KÒ”¼“ëÍV_Ç(Ö³Ã3óÑ³ôëtHö¹ã–íÅ·ñá"Yq-íÒuûM‡äQËÛèŠùàg½Şà:º>ªµf–’€T°= İ</ñY1Yµ™s!cT£ô,ø^®8{WËíÅ‚¶Á§ì§HDû~ğ{
sîç×x'B°Êiš ç+*¾à±pÚåùŒFúD•.Õ÷QªEô—rÇ‘ç¯ñ<±¼—ÄZ¶5·SâO8b¯ò(¿aTLêëkÄü1ÑK×øÓÁS®ˆWñ4`-f|¶Oô[°é[ôåY9ò<˜Õ\áªÎËö%£ê-‰3®8" ë	Ø$ÆFsV–Ê*³#(¾Ø>!EÍäB¾VÂMˆaVDH&a?'Äl+Ôêcd^.·së„€ô€]t
­´"ü!¿Í1í
Lãí¸Ägï™¹`äáR9İNïÆ0À7C‚l2f¶XG;qô¢Äß0ú¼¬à`Ò¿ˆ…úŞÔÂQ™~†Ï3úpzğ*p	Éä+	‹Îî7a÷=RİÃÏhØ)—Ö©f&‰ı¥."qõjiŒÍÌÀYŞè[û?ênå™‚¿e›sYÄcMÔB–kÖšn±yÏ²špºÑ›%Öxê¿[w.Ì1©¢
ì% )([`kBåû4‹Àñx•ÿÈ£öã›ğµŸ×ô,+ç®6~¨Ÿ)ÈuøûP}‰ß0P‹ÖÆÂ‡K‘2wK×oıÂCjÒö(]ÓD±Ú‹nÀ@ä…é"‚õ+,İñÜ’~X
¬”ï¢uô¼y]6+§UÔæšî¨h¿JºN~Iß°)ì™f!(÷Ï¼Çõ£ ğD²n[–RYÖÚ{buöÔÌ+'™¤ÍµXë£W	OòlLS¼š0"&^Xûœo9­2p0ñhõür²WKB‘8 ™l1µë,ÎƒÛo²PÍM	æ×ˆ»$ˆ8œvt°¹Ê&Ã¬ºÎ¿b!3#qàñÛ(¹‹†‡æã²T¨õ¹½ .‡ÔÏ"’:ßÑ”|aò$IÒö±“	æî0sàŠGÊMôÊHú5àˆı}b¨Ã'´oÔ/e0¿=ø™ñn!Šà:è9ƒ·Ö­åå€Êª-IÉÙæ·†>Ø´Jˆ:(³7KqU‰‡‹ñ$Œœ«Ü^)Ş©¬QÉ…/Œû	lÿR·XS›Dz¥#+†’áüd>HÜÈÒ_íâJÙ<Zè2uSF­ÖLó´#p¾pøQ†±nrÀØjäô³Äç—öõøô™`·C°ÅÎÙA ’C†[_@by…!B×˜38O™cƒ—iØÓZšÎ³Î÷¸¼PD›ÄJCóGùHIYâ’äWB8‘ ø‘,×W0º,O‘&L;·EÚ‘ÎN*qBcúÍ“ëæüÆé
Z·(T6ˆ
–)èF³b;Î)Kc±(—¶äõ¢óG…‰ŸÍq–‚'IŞ.= ‡; vîzg;õ€óÁäK!Xß¿¿á’ŞŒ4q’cİs
©°VP;Ô‘§-W#€cıCõNbHïİëÇ-g±âÃØÕì$êÚ\-¶ªñˆaƒ½³½ ëJŠÌ}ïò†ìWGàŞHYfM¦2ôs¦ÚÔû^²Ê‚ïÃtŒj,W¬mKTxk%ğå]vzm¼½ÔiŸR›Fé¦7‹ù’9¹Xœg¨÷(11.DÛõ¬âÀ"ÖæzÏ´¿³¾ÁdÍn1|C_¬ewÂpn¢øÿé¸—?Â¦•NaÛ&“¿^ÅÖ~•¾ÆıôÆÚ9¨„ş^ê&Êò¯H©'ô¼º¡A]ËK[šÍ¦ÓU©kõœ‰ÔYd—º8ÔDï–MVX¡¡~E_×†ŞœéÀ#a‘6u`/IŠhƒHôæFo’¡ª$´¼‰‡3¡%)ZŒQì) Øé*Wò—¼ f×ŠM[®ŞG.pgêTpƒ©9›É°+ ¥Koí,9¤QbÑwù<»Û9¥øüüÌı_Í>‡_=y©Zç+³„ÜW™Ûê!dUÌò=«ÅëØÒjµ¬l#1’ù­€@øE`,„ÄÇœï5ÉHU6‘¬ÎÆN»mVV{mãG[R|éj·„Œ1°)ÃTád«*àpn!•r.½YYkW{ô9Aï…ŠŠæ.·,Z ˆPıç<¡+3B§É(=!<çöZ$™'×,á[Õ~Ñà0Íƒ…½3vJu”u3ª²¯ØaØÙˆˆQVW5 ¹H¿3œˆªJÀÄ?B›Ûªº7ŒlÈõi!¼ ÿ*Öt¼ëÊŸp70¨d1µ~8EMXıœÚ ëŒÍö¼¦œÉ²Œœ@	YVºÂ6-ëqm¹ŸÇ…Y1×7Mb¥Ü®Æ’XÅ™¾Œå7qKX"¹pããÄ,Ó>ä& i•WÚH+†3Ş]6CƒâÜïüÛİRä(VAkÎ´ÕÛs¹šÉŞ¬Qà9 }}å¹;Å#x‰ü.Õä§¾şJBäx§rÔ>Ú*—ˆhIü5Úç+âQ·©¶ƒ‰°ä·••qù·@}yaj(íQUÇoG‹3y¤Àğê¶)ğç‚iZÇÆw[Å¿ôÉ@—¸ÌĞóîª¥<X©Òû!DÂÛGn±#Ê8Ï“ìÒx™Z"·{ÁmÙz£E>«¤\ïv{,/69¤p7ì^iÿ2²Ş­_åÙKŠêsÎç×Ô;dª¹UâNÕÈ—§
O¿zM""eõiÕ®ïV¸:Â…N20Èo-%TØc=ŸÁ’euDF¡ÀKNW;?â/4b<›=“¯ƒÂË­±¢ÃeÇ)©¸d6l¤h#Î_ô‰ƒ6£H2¢âwÕbnÏËu·n[ò2T^väŠ¥DŸ)	àÔÂË(r‹XuE15€~/ˆ|¸å†8tø°ª!\±u'm¯of¾­\â
ÊÁò–ºsB2AÃ¤=Ğ†Ùßfç‡Á=ÿ3lÛrk |>i\Ô
â2´™KöÕE_DŸÛ®»oqA¾´TH`_¸²¿4™_ã;{ ~ìÊ—!$œÿÃWºÄz…éP¾‚ÔÜ|@=ŸÇ½\a°àg7úîºDGÑf“¼¯î)c"{p^³ûÔŸÏ3ú'pÎ§ˆx'JpüòŸ€ıÔ3ä8. Gk÷O©#øfg3zm³‰ËxAÅœÒíçjù¿ŞÚÆ8ŞÕÆ‰\IHë·²~`cfŞèÎVâôñ»ˆ_¬Ï^±¿Sdi
Å\şmq-µæV{	XÕHÅ½8İÇˆ˜ @mĞrPæM(ŠÎ$ıQøú¼¯Ğ°¾ ÚÍ‰Ø¢Ü#…LÜÔÃÅøFô > M2¸çõoÔr æN4Î$ıš€((*sóÂWuzÕjéÉhàôÃH¥üòf×é¿6À§]ªK;GŠ©ô¶úÈsšrEYşyA¥•$İŒÚœUl^üü¾Ó,m—Ò9©ş»ïŠfzŠêó*º/ÙÌ]j’8lWJ„òˆt©$‡Ã¸’ µ~®Œ”`Äš[å¡"™‰Ú Œş´£1D›¨0ı‡r«Ô¥İ4îb6ÚİE\O0+;½n^ı•9ÒÅJô@*¥ã>m1AeZ§î¡k~ú>ğ¦Œo(åõg'.ßÜBï…18ÓÉÇ)S¾êjùš@©3¤+ÀõàW¤¥°k*¹ü\á§ÿù×ıÓ+)uÔú§ãµüëNM8ã·9!ùg‰ñX9¾˜·‡e4ÇG VM?fålc²¤ë·L,sµ¾×f¾İKëNšj’Õöó_¿
Uw•æòÕ­ö¥™g	Æ">}¢5Û=V±då^ù¾R^°«¼á Âéª>ÖHÜÎ9vÄpÂIa¦®(ÆB6uy.U¸ÌüaM3‡2úÓ…~r„à€9.ÛÀ1AGé$UÍ‰Æ™(İ¦uÀ‚Y!{™É§¹Ó
·»ĞO~Ìo8:A:K0›%`ù:6V€ÅaÔàKµ»73\hÂ]–¦t¶-©\á"-ì¿ºHß2³ºÎ´k‹2qíjeçåC8ßPÈq»²8ÍRÖ¤åø}°»"ì »ïá"˜øaò€b	(\ä'T/3Ş¨ÏÙ‹É¦·£bA/qª%…x¡³pgÓ:BE[„Z=´¾ë0€ €iS¬%ºa]Öâp‹F#à@pzÜnƒQ®Ö/•‚YÏ[ŞµHı38?z)Ä¤Ï’uN”$S·Ê0Ùš‹ŸßÕ³«Lï%Ó3ôÀø w~ÅK¸œQ<Fõ9MÂEGe`İûÔ?Vê’±ZLkÅ‡![4iå§âä¨«÷¹bµ,ÿXË[·È]¤kkZ„}M<¥¹„Ätß’íú.Ô•îñ,sQ g7…’«JóW83Ç<	Åİ|õ¨ø¾ìH’_ş<± ğÌ®Ì|aù¥6»`æœ6×p='úˆğP"î+ã¬U!$äëBJŞ8 ²´Wa’qÑƒ9İ"RAïßğÜXj˜¶`gùğS]ƒOlªÇNZ‘&}%Mz¹p1É)LCÆcÍCşŠ½‘z^ñ§[Î× \kycÅ‰×c4Ä_¾b‘Zœ•øHm”.)+?}f6œy!H#—d0Œ@ï`õ¯ê¼ˆ¦›ğŸùGâpdiB°éä"[‰’}„¡SĞ¬»5­ágD–zo5™èË-×.%á†3%èÉş¾¤­œÇ§\dor#ÎÅJNĞÿaD™½Ùµıå)ï
†şt©KÍ£ášøi¾æqå|_;™bª$;Ømä6À³ãÂÜ»´xŠ3" Ä–® ô»mK´ìüçi·ÈÏ·Ş¦ÀÔø.5@‰f¦§T™ü».‰?k§~ CôCv2*Òˆl/³¾ ´q­íC“Uı».D¬M€8ŸN$ïKBKšÎ°ÀrnÊ´WÁìœ¼¿GÎROeXnòwĞ[Åø‘ËUÀÛBÒ§¹Hb|f±Œá¼$^a"¤ƒF¶•÷]Ç%•Ô*lùƒåÿcr½p».(3Ïóíƒtÿ(ÇLœUCl¬ÏE YÏİÄÓ~³k¼™³nCx¹‹Íœ!<C÷¬ºÆ¿ü"}$Ìf¢Î;9¬¥Ièp\3Ï_±ÛüD±XU‰Æ_Lùı~Ãô›„ˆêöYcòÂ{kÿòi&–9¼å.lxpµ+ZgĞ)_ıDåD
ãI™*«|Ì/Kp-iVe!øÚ„+]¡±¬©‹•kõ¶C?0ğ`í-éâ•Aıy9óU¼=ˆŠêÍå‰,)f”DÒQå‚îtŸæMæwÅôJ0BŞ6Ç=WPS,‹ ,	N Up¥®}Dûa¿liİ8
æ;]aAv™I\ß¢ü„¸i 3Ë)»Ş™Õ?ó£¢ªuÑÔ0d{øµl{ğ]ê7eÖEQ]²­ìñüıÃ3` W\ã>Äz¸eMÒŠò.ŞÒO¬…!ì•×¿ukØ }Î!É	å{å×µ*DF—¼hÙÊ'EL¯õ†\©U¬*àºğlY¯ORNsnù¹l<B9t ÷LßûOë·[”myPh˜+ÚıÀMXsîa!¹ğÈĞèPhƒ¾nğ—Ó ”šÍ@êbí<£«k«LÈ4¬sç®”%[Å,è_ÑfÀ9ÏEªl®Ö’†ÕŸ¸G€*“œ:_0g•5şdSû€a }«Q8•¡áÕJï ha@©{ŠJ11r3E5iqxÄóÓÑìÚŞJÊ«D¨íË…Ì©“dÑé<¸ñ&…L®aÍ†£UèÜWYÔ½ÉÛ„rÚFÌòo˜ºğ`;zSg `4T'AñX³d2DåÄÊßZKb“|.¥²à½Fûâw>„‘d®M½³ê»AQçÜ^æ7Šş¶ç©ˆ`àDI¤ï{Xû`¦ş­’Âà
­”}KHú¼Œ<ıaº[ˆ~Wêï‹{EuÚ Œ:_N`æüµ+"áß9s?¦@1Kíª“ûŞWìmîÂl­¨ÿ¡	Ëks¸˜Òã,×k('áDÉş)™h@şŸ„‹I‘-±(ˆ5İ‹tp(
XB°¨¹n)©>GÀÅEi#
H•’ÉÙ!UÒ°´è±H2•¤!˜¸í&5WlšîÏ2À$tÁŠ ¼§b©«Hzãàê¬Í,Ğâk¤¬¿½ş%¡îü0hVİ«-ò z”4»îi²xS¡€n;h&”CúÁHÕÏÿ/(®
ıÆ°4rÂŠet}é¹I•2c©¢5‚ Ş‹[9;B–n6 ã/M¶Ôé»ñZaßó¹hÓºT")zPˆ{”sÁf‰F+ED1³«- ŒajI/0Bñ;^	´yıµ-o$ïRDáİÊ/æÛÅYÿØ=Eú	½G¿”ìì£7,¬F±^¬İhÌzıE‹ì	;§4ñÄÂ§8!ZŸÌØĞ‰â›åj>f’Gğ¹>Sæ›G5´fzõ¼x†g]¸ZLYr„’eÍ` /aB•'U§]Ú—z:A™Ò‘ì#â÷CùíÑ 5eÔåªVîWA¢ºÌçOš‹U”!¼Ób»<	‰c,×'–€là1n¯2ş_‘.Ã-´z‚ÆıIÕKåH‚%S¶İ¸Ï´‰Îâ–¯e/‹|ê¸Ü]Õï[:T×
Åù‹<a†t'P<fŠŒíùu ÆUvŸ¶¹7>³ıF2‚g›gZ‹ë®Á¢(XM¤ óÁÔ#*ô’”J™à®‰‘Ø|Íy~3 ¥¸ß²ÚvZ¿b|oLåCô8
ŒLç¾ñú^ewkÜággGîÑ+Ã±[‹­G¸§ÀñKú“B|wBÌÖ­¶áÔÀ¹O
»ÈlßSÆåŠ+şîøWü,Guğ>ö´HE€¼§iSC>˜QäıÆ#hÙìoéA»jQYAIûWåe,Cš ïJ+cİ”$¸œsêÿ\îÎçö Ÿ£8ó<oc‰‹7îüĞØ´E‘ŸÇBT,Ñ7ˆªá*.j.9Ri”‘ÓÕ€[ê`Æê¡2t×«±F™
eC‰¯z	:Ñd~aJ"Ş
¯ÜM8
Û¼ŠÃëÂ„–lL'Õ%dûuÀ$7å6ûÓ†#±m?Ì_é^;…ÔµàĞÈ7$Á„ßà½ów>$È•xz¾­R¬\bCkHxĞ¯],­D;#X+/°ˆë¥NIüiĞÖ"›ŠyÒGQœ¶
[ş¤&ö¢áœy˜ıÏånİagG!Ï°:nWèÿª!ùD32ß<¥Ìx†©OIx¨Ö™«;úÛ¦ÊĞı(3~ôpÊ¬«ŞÆ(¼Ü^³şÒä!VLŠ¿®Q
,”¼ıÕõÖL+Q‘+KdÙ€İÙgõ÷ú/(æGñŒ2=^…rC²uİîó‚1Á‡H)9aŠ±~)hè£™ÙÆ'Çe6y\TÕEÂ‡7Î9@•W‘Åa:Ò†ŸCšZm*óôÉH$Ïåb ÿğn\ro~.&¿·ş˜C~×f		Ó8$û4İÁããõ w,Æ[RËä0+Ñ¿²õºQ½&Ë’a£›‡¢PĞ²˜!Ï‹ Ò?3}OÁÃÁ'³yb£ÛEyE«wA~Û¿¡v*õVî«ëË‚ÈŒMıJ®@İchÈ†0HyûF²½éìğ~Äh¯ÂöÈLAt“C"Õ2n²‡ĞÙ	~ùgZ_ˆû%¢Z'ÙÛbœ×£ºd«n“nÚÒÓli@,•©5r®­~s¯NVöP|5¶ou(/Üı˜/oËpiïFÙ¶Ûêâüítü*q_];VÑ£ÀqmûU.n
!A[«æŠó†bšó-ŸŒª0âÌŒl´¡Š=!Š‰&Ş[G.\/M‘åø7Jğ±¤Ç­„›H‘…Æô‘hı©Wİœ»|Œ¹ÔbM!û’-nÃ·áğp¶‚hÿƒc ÈÚÆºÏ8_ãT³ş8SŞÓHfámû	é`²_bL¼.?‰¤¼âËq!p¼cK™#!Ä€À2Õ’Ôˆ¿3^D ôeœQ¼‚°§Òn³ÉÁ³½|B0/‘XZ	¡´ÔRŸ iºI ŞıjFM½¹ß€â¯Ÿ{ğ}}äÙö³62™§ÿwH*	1º¸Ä..N[ÜÔ_íè>¦”Èy$ûxÄËağÃ´É"`Ôªïhš…#ÿÆ£œ÷ğÛ•ùWb­Êz«ŒƒŒ,[&²h.°ò?Ô3•+«®%_5Ww å­´Í”“ëc±)Şº…ı*·²)‘86œ»sÖô,IÈÜÃ‰ö$ì1­ú˜‘ñdÄ(_¦¦¦A4šßEÛövÜ›ş Œ‡?@ÎHb* ³ÎÚbs¥+hˆ·VßŞ
ct#mˆÁ8ü’]j=$D×QÚñd–fb7-5]G®w
¦³&7§ï8`Ô”õÊy«ó™ïW'Ç¬^ğÑBt¡İ‡pÛoXõù‚t47—y‹áŠAP-RuÒç39eåßé±Úø•™3ãPîƒÛHÃø˜ì¹wî[õVLK­Y}³Ñ<½?QÓ‹Ãì°ïYd³¥E¶Ÿ&©hÌ(~b6_@µ‹¤—9v„Ğu,SİøB«çİâf÷!Ğì‰ML¯Úİ 9Ø‚Í¸RåóU¬kŒK+¡
ë®/ô:˜G½²IÄÓ£k®àÌ6ü¤¯œVSHÎŞC(WD§’Oc	ñ†@
Vbˆ»%äé{{ÒÃ&¿dÂÙ<",{tBºú»áVİ£Ïí`È³;/ÏiÉÆ‹ô‡â³&ªÌÆÇ£ 1-0‘4Ê½ã<–‹„%üiÈe@ÔòWõÔÃ#iB—ß$u\+&³v	í<"ªn}zXL˜+â^I*m1¥\k{eYÌIp§Úüï96`µ“ddàüQZ¸”ûQn…—ı¹^6áÄ'¤ºõ9æ3şèGa¼È°p·Ú «yv|¥%Ï\¢œY&.Æ)%MXäÂ{ÁTƒSMP“²äˆ¾¸AØ`T0[P‰Te(bB€8“#^¶?vM®‹V‚8ıEÖÄ6ü8Vôé” !úSÎ®×&êK'Ñı9ª¤¸i‹w~Xdn¬Éş³O]Q‚ÛG‘4Jšr4õØˆŸCº`¶şg‹(Ô>Áşñ8yØ¸ñë.h‘_O¬uõğ–î ıØG0ûì¤ÁïİÉ€7¡å '\Iñ£U]¯‰»e,¸ty–z`1 å‡œâSoË¥0¬öºûxNZFxÔ®ï Fô3Ó®IáÒä†£…+‚’ÂÃ¾Hyû#>;Èmà(BòÓ¶,çAI)ñ,ó‡z¯"é;2´©@Õs p“^J‘?„µ×ë+÷ƒ›¤»¤/»²HVØV&ªL f,5™ËS!k±¥©o2‰ÕÈÕkÓ
\›ö…H÷Ï{!²KÔ&M!F2%§(×t7÷I]İƒO	 d•¯—wƒAd*Ÿé$´r¼)aPu1}¸Øº‰Pï	òáHùj&#^ÍbûÈæ!+ "“*OpñFA±Ô–A¡V×Q1•,OZAÿŠbCX ğ‰ë§İ¤Ä[J‚ƒ–=ŠRºÚPsîçsQ’Ç…ïW–á˜©kl­G6ŞIMXz¸w•Ú…ÑµtxŸ ·ªÔìrÉ^úÇ)E;—Ğ§à.=Yv|¶Ñ/J
"/[`Œ,·°¥ûUtâ+k“¼%şØ‡—%C³ÑÖY¡Uø:À+ŠW‘°Bëƒ¼t©¦ÆÍ¶KÜ›à"ÏJ&6U!z²¿+2¸Íö»—m0N7Éıxd¼UğÔÜ$ï—›”IÚnÒ Æ™{÷šİÕ¯Q"ÇÎ¯AÃµlbÃh%¬zQİÍFènâ¢ìAÍi^7ğƒc‘Aö÷O»×I‚&âAõÊ¤zæBãKjÛé½GJ>Ü¥]6eæÆèézQ
“/Iª,\ôîÃ¹Â°Ò¢wŒê&/u$‰«ÃîFRÖ’¾bñÎŠßµ"—OíYa’oâñ(«»»NÎôâëÌ_bŸ­tğZd+oMÔ×†t€Ö¡™ƒ©¾QEÜ(["PL|eÍf­İ…æ®˜e|HµAËİpÉ¸ÀÖCAŒ)1±j5<˜bËbºÚ›w_[|±Bj”n¯¢+ÀÍõ X[1nSüh“i*6¡É19Ã°alæEâÂÅ8ˆÄOy³	å×>aºŸ(S¾‰öÕVÿGLÑüØÁSê=ÓıWâˆi_¢?õ$®
«ƒxy|MI¨›5A^IKmè	 ¡ˆŸVjØÇ‡ù{Æ…Òv:µ-]ñ{T$†7óë>[ ìdDX~üü
ûC:¢ús™+WçZ–+|—DÛ ÔÚré©iX›9®ÈÓ":%DşŠ“PYVF
Ó9Ÿy2ÉQÏ9>EB22Z;vRUgÿûmD-ÜM%ŒØö
Û Êƒ D¯¶z>ö)µ· A6h–MO?ßVæ B¢G5ô5Ç/J}N<èáÇ#‘CIò—)š=…OçY%ôÛDénêØ¾Û®#ëAˆ{&”YõºæNÇ|1	mµ!»N^¬üƒméŸş6úØùî¾bu®œßâ‡ä?æùÂ[Õ,~F¿j€HY'iU­|¢o<Šš«ˆ/	ÜOiu£ï$, 
Œ¥$»dr½àÁN³YßJŞDqF¹ãèò¦J¦ùı5fWY ÙIŠTÓ
ºÁ²»'0ë8Ò¹)¥yƒÈŠk£‚]Ù`¥„@GªS#ÌÊÒ|µ1"„a³ëù$ÄP½‡™+°Ù®Æª…‰s_‘eZ¡Û‚i‚lœÄeÊ¾·,ÊhR6[‡¿ËâŠ@Îˆ¾XGª—
&å(D&9£ªÖŠ]2«K6á÷~™†$Æ1$ı]*tã©gt;TlÔlM,]½ér{¹‡'Iº… ÜN-ŞÂ†³CCTä#}¿DJV`bÒÊ/”2¡-cßOš:jYÃEB#¹Úk2‡ÖSœÏb
×5
B~Á:q,é á¿˜KÍbN`à$I¦àJ%·S°ğ/åD7lÉ±f	`pvÖƒZZM+|ö"û–\eL“Ñ›.ÇÆúéÌ®y]˜Â‰ıéÈÓ´êÅ¬lMXcI—ÇG’Cw•Ed¤ûÛ´ÚËµğ:ÍúÀ^”3GA“ l˜p>5ç’¬L‹„£PzHPÅ}‹lëØî›èMİUšÔDíoãê<´9Ú3{¯N&°8T1ÆÑ5¼çÆ–òRr'ŞúSy~kbi÷‘nP8ÊPQªÆrBòÃ.¸I°îT|ĞœW(qMõ¿€’¨ŸmQtcqEs7UóV-©o"!cÀ
ó2ƒøP<´¨7‡V®Z˜/#ô6âˆ‰òÎÃS¦Ç…«B¥Æè³–±åÒ~Ükéµ_Ò·ÁÁõĞlVZ‰hÚC3[âí~¥^˜ƒ=r!$t_WÉIÏğSÙ¦º9ÁÒ·Õ¦ÄÚV9h\ë=wñ­<lT8J¯ı„–¤9¦ú1¯¹Â¯ãGÿÏ0”9¦òÊ~«!£\/ó;fíyQ—ÌÂ7¶¢Šyje=/nKt$Wh\Ã^«ºmù6
µĞe˜¡Å#àı¬°×y<D<j1XâJŒëoÃ;”(=İµ9¦pJ½÷ŞM>½Ëúü‘íÑİ#4wyôì|†Ö².? ËÉ>6-g»ñ– ´ÉMÏıÕ*æ7î°I.@ÁHÁ×|4b sú
c()WwÉşÂø>#"®WUZ'(¯‘]cÙÅ–Uê@t+UŠ’şYÄ›¦U…¨Ï}ÌÍÑÖHù†¤Ûyøxû«Eû(ŸŸ<û*äv5ÿ´o¹xKPs,Æ\ù6;j½åV÷×Â}1›ÔñøöÆLûpæOFF=ÅÎ5¦n€à+İ’ıµZ†Òğ	…RÖámOh7Nè.š”ÎXĞºŸf§F††÷€veZ„‡ĞPù¸—×•	ï.ÓEÙ§sà2Põ®~KsAj­ÿhdmÀâ\EB™l]rv¯·ádòÂƒHIa_HÊºu½¹A_[U“1ß¥®(yn>¬³.İ\}»´B·üœzƒ¢®ƒÌkN÷¬WM.Ş\^|`šš<VN‚Sr¢ÈÈ†W¨¿ &Q‚ª}®QáÎÇÒ[…—b§J lŞã´4’xµ¥a	¦o0E½Â¾.k	/Ø•Z¥İtÙ¾|ŒÜ7 §t^€÷7ü¾9‹mÄ¿¸è`'â6¬ë[Øô]òNYÊM0u3¦­1PÒKúÖÁĞH¨Oû²xö„¡©FOI¸Ü—_8	ï[d½uZ.ãJ*¤cµ|b¼Iÿåu÷N¶æò7&)} -.y5æ@LM§R€h×jı)/•é+ÚJq½¨›	‘DûuÖŸs_ìWìZ¹ö™){V‰‹äc[Œ!‡ĞÅÈ&ÙdfÎS]°àÚ‰¡®,»´}2
ô¤Ê+¦W¬å¿ZF§-Óq$O¶Å'-äJ+D‚è÷¬¿DoŸRùÃOrø©”|™£¿eu–$Ñ´R9ãT•ªzWN9q”EU
ÓhEãÑ‚ß’]HÑ6ò¸àjc£>")Ùcàx”},mÆ#´Çºš¿E™´[õ©=ÇÜ±&Œ¤¡ö6$Š}ÆYu¢ûÔítü°Ù”z‰] ÏNa5ğ«¦]š®ÍÁ„üô5Iñv´ÀricÕ!#JT´ŸÂß×Oüàû.­8ŸŒ2Ø·B7½q×np»g|zí!ˆ]HÔ)üİ	C)Aûù@MÿÚEÊ€û]©Â3®<k¤@ªÌÃ­fÏ|»x¼=Î‘Dí2Ò<ô»Y{t'DÅ8cif¶ıÚWS›ÌÏ4ªŒ-SXE]¦(xâ·©P ª×*Û øâDİëıüG+!Öa·ÒAì¾àõÖsÀfK+™¼]í+½×³>2í9Yk¨H¢^1¤üPvD!V+®Ù¡-´L3!{6°%0Q.¼z¹’€	»™õ—ˆ¨Œ &tx»x‹ô[
ÇÚ?|¢:K¼ ¿ìßƒ§ıhXhÌÆ<«‘F9NHá·`5ÑDeo¢ºğGƒ(úë2—¹†‹C–- Á~·ÌƒÏNt™Û6È´D*—u5uåT ÂÍÛQªûê'eCU4TÎ¥á0Ş1_!•€ö*  Şf<ÜOVËÍr"6ÉíğÙÙ•[T(níWM¯Š²ù÷Î‰ÓI¤‰L{-²­õ*å³Iş(¥¯%Yo},ûóoA&|
ÈX#,“BÂÉÉûB‡Ã^BÆıã<@şòBqdÉvÎY™Vç°şêâu‡n	F'/Ù+Â€pI¤´¯ø…gí¿wÙx…û4)/ êÏÌVÓx"6¤¥\©`-¦×Ó?é¤‚Vgû"dVR'"ñ”»äq¬ô·o<c¨u“òÉ¶j* YíWêÒ-.­û³z§…ÓşCÃwş}¬¼Ó([5ÑÈP¬cšdìV¢ÇXÇQ–¿Ë·YŒ¯pØ?Pš•á‡væád™šèã}¤*Uyv>ğÈ5†dğ¤Ö2ÓK`êù|ÌY$M¬Æˆ<;¥üoGıÛÑšÀ¼
ƒØï-(l;mi±¶é¾·tOgªìÂ¡Ï›ä/a7Û÷BR4Pà³;—/IÑE¯1Â_ÕJ¼5EW‚¯ÍsŠÂ…p,uİ<xêCpöoWıÔ[º`ÑJƒ57—ÎB%¾Áp#ÓNä”>#;CS3r?DÓ İŞÔ®ç‡µ&RAV/LKŸ©–î²ÊV©x"BœÔêé‘U«¨Ï6È•Ä¤ÔŠ¸¥êVšMªlhç|½lo],Hz0.µQğ HË#Îs¶ÓL««‹7Ğÿ¾í-OåQ#9|^¶{³ª¬l}×H)F»4ô’‰78Ö‘ÆKÔR KŞ@¸tîÌG±Ôú¹Bã6sgn·`Úİ"X«PÇuèò­ä¶NÆHZSAÖ ±">Â51ôÛÑø
Ó$}.êD±_#‡-”&heKgf•ˆ‡@‘òğœÚ|ğÇáÌuR*±‡s‰ñÙ³,Í·å\i3r†}A'¯*×£Â¶›
VKá™b…)¼~Å·}!]l=¯ëy¦nÛ{âöä“ÎkÕ•TæÄal9à-Ú´añ‡]b$(vëÔ@ø:Z9œ0,ÆÖÓGÅRty¹‰“úZÀnã«ÙDŸşdoÏşî\"D˜&ïñÔ0$”-	”)ÕÄuüÔ^6‰ã“XèîH%»äƒzyì5¨¦L“¸n¹M&¤nÈ#pEß‡ßx©I-Å¤ıäH±˜e %Ò~b5õ¶¨ÀİÍ÷û#Î§-JÍ¿]¥¦ø%{v
—Ïqı<ù¡Bølùu]