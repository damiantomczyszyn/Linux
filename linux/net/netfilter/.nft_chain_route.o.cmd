f (should_resched(0)) {
		preempt_schedule_common();
		return 1;
	}
	/*
	 * In preemptible kernels, ->rcu_read_lock_nesting tells the tick
	 * whether the current CPU is in an RCU read-side critical section,
	 * so the tick can report quiescent states even for CPUs looping
	 * in kernel context.  In contrast, in non-preemptible kernels,
	 * RCU readers leave no in-memory hints, which means that CPU-bound
	 * processes executing in kernel context might never report an
	 * RCU quiescent state.  Therefore, the following code causes
	 * cond_resched() to report a quiescent state, but only when RCU
	 * is in urgent need of one.
	 */
#ifndef CONFIG_PREEMPT_RCU
	rcu_all_qs();
#endif
	return 0;
}
EXPORT_SYMBOL(__cond_resched);
#endif

#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#define cond_resched_dynamic_enabled	__cond_resched
#define cond_resched_dynamic_disabled	((void *)&__static_call_return0)
DEFINE_STATIC_CALL_RET0(cond_resched, __cond_resched);
EXPORT_STATIC_CALL_TRAMP(cond_resched);

#define might_resched_dynamic_enabled	__cond_resched
#define might_resched_dynamic_disabled	((void *)&__static_call_return0)
DEFINE_STATIC_CALL_RET0(might_resched, __cond_resched);
EXPORT_STATIC_CALL_TRAMP(might_resched);
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
static DEFINE_STATIC_KEY_FALSE(sk_dynamic_cond_resched);
int __sched dynamic_cond_resched(void)
{
	if (!static_branch_unlikely(&sk_dynamic_cond_resched))
		return 0;
	return __cond_resched();
}
EXPORT_SYMBOL(dynamic_cond_resched);

static DEFINE_STATIC_KEY_FALSE(sk_dynamic_might_resched);
int __sched dynamic_might_resched(void)
{
	if (!static_branch_unlikely(&sk_dynamic_might_resched))
		return 0;
	return __cond_resched();
}
EXPORT_SYMBOL(dynamic_might_resched);
#endif
#endif

/*
 * __cond_resched_lock() - if a reschedule is pending, drop the given lock,
 * call schedule, and on return reacquire the lock.
 *
 * This works OK both with and without CONFIG_PREEMPTION. We do strange low-level
 * operations here to prevent schedule() from being called twice (once via
 * spin_unlock(), once by hand).
 */
int __cond_resched_lock(spinlock_t *lock)
{
	int resched = should_resched(PREEMPT_LOCK_OFFSET);
	int ret = 0;

	lockdep_assert_held(lock);

	if (spin_needbreak(lock) || resched) {
		spin_unlock(lock);
		if (!_cond_resched())
			cpu_relax();
		ret = 1;
		spin_lock(lock);
	}
	return ret;
}
EXPORT_SYMBOL(__cond_resched_lock);

int __cond_resched_rwlock_read(rwlock_t *lock)
{
	int resched = should_resched(PREEMPT_LOCK_OFFSET);
	int ret = 0;

	lockdep_assert_held_read(lock);

	if (rwlock_needbreak(lock) || resched) {
		read_unlock(lock);
		if (!_cond_resched())
			cpu_relax();
		ret = 1;
		read_lock(lock);
	}
	return ret;
}
EXPORT_SYMBOL(__cond_resched_rwlock_read);

int __cond_resched_rwlock_write(rwlock_t *lock)
{
	int resched = should_resched(PREEMPT_LOCK_OFFSET);
	int ret = 0;

	lockdep_assert_held_write(lock);

	if (rwlock_needbreak(lock) || resched) {
		write_unlock(lock);
		if (!_cond_resched())
			cpu_relax();
		ret = 1;
		write_lock(lock);
	}
	return ret;
}
EXPORT_SYMBOL(__cond_resched_rwlock_write);

#ifdef CONFIG_PREEMPT_DYNAMIC

#ifdef CONFIG_GENERIC_ENTRY
#include <linux/entry-common.h>
#endif

/*
 * SC:cond_resched
 * SC:might_resched
 * SC:preempt_schedule
 * SC:preempt_schedule_notrace
 * SC:irqentry_exit_cond_resched
 *
 *
 * NONE:
 *   cond_resched               <- __cond_resched
 *   might_resched              <- RET0
 *   preempt_schedule           <- NOP
 *   preempt_schedule_notrace   <- NOP
 *   irqentry_exit_cond_resched <- NOP
 *
 * VOLUNTARY:
 *   cond_resched               <- __cond_resched
 *   might_resched              <- __cond_resched
 *   preempt_schedule           <- NOP
 *   preempt_schedule_notrace   <- NOP
 *   irqentry_exit_cond_resched <- NOP
 *
 * FULL:
 *   cond_resched               <- RET0
 *   might_resched              <- RET0
 *   preempt_schedule           <- preempt_schedule
 *   preempt_schedule_notrace   <- preempt_schedule_notrace
 *   irqentry_exit_cond_resched <- irqentry_exit_cond_resched
 */

enum {
	preempt_dynamic_undefined = -1,
	preempt_dynamic_none,
	preempt_dynamic_voluntary,
	preempt_dynamic_full,
};

int preempt_dynamic_mode = preempt_dynamic_undefined;

int sched_dynamic_mode(const char *str)
{
	if (!strcmp(str, "none"))
		return preempt_dynamic_none;

	if (!strcmp(str, "voluntary"))
		return preempt_dynamic_voluntary;

	if (!strcmp(str, "full"))
		return preempt_dynamic_full;

	return -EINVAL;
}

#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#define preempt_dynamic_enable(f)	static_call_update(f, f##_dynamic_enabled)
#define preempt_dynamic_disable(f)	static_call_update(f, f##_dynamic_disabled)
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
#define preempt_dynamic_enable(f)	static_key_enable(&sk_dynamic_##f.key)
#define preempt_dynamic_disable(f)	static_key_disable(&sk_dynamic_##f.key)
#else
#error "Unsupported PREEMPT_DYNAMIC mechanism"
#endif

void sched_dynamic_update(int mode)
{
	/*
	 * Avoid {NONE,VOLUNTARY} -> FULL transitions from ever ending up in
	 * the ZERO state, which is invalid.
	 */
	preempt_dynamic_enable(cond_resched);
	preempt_dynamic_enable(might_resched);
	preempt_dynamic_enable(preempt_schedule);
	preempt_dynamic_enable(preempt_schedule_notrace);
	preempt_dynamic_enable(irqentry_exit_cond_resched);

	switch (mode) {
	case preempt_dynamic_none:
		preempt_dynamic_enable(cond_resched);
		preempt_dynamic_disable(might_resched);
		preempt_dynamic_disable(preempt_schedule);
		preempt_dynamic_disable(preempt_schedule_notrace);
		preempt_dynamic_disable(irqentry_exit_cond_resched);
		pr_info("Dynamic Preempt: none\n");
		break;

	case preempt_dynamic_voluntary:
		preempt_dynamic_enable(cond_resched);
		preempt_dynamic_enable(might_resched);
		preempt_dynamic_disable(preempt_schedule);
		preempt_dynamic_disable(preempt_schedule_notrace);
		preempt_dynamic_disable(irqentry_exit_cond_resched);
		pr_info("Dynamic Preempt: voluntary\n");
		break;

	case preempt_dynamic_full:
		preempt_dynamic_disable(cond_resched);
		preempt_dynamic_disable(might_resched);
		preempt_dynamic_enable(preempt_schedule);
		preempt_dynamic_enable(preempt_schedule_notrace);
		preempt_dynamic_enable(irqentry_exit_cond_resched);
		pr_info("Dynamic Preempt: full\n");
		break;
	}

	preempt_dynamic_mode = mode;
}

static int __init setup_preempt_mode(char *str)
{
	int mode = sched_dynamic_mode(str);
	if (mode < 0) {
		pr_warn("Dynamic Preempt: unsupported mode: %s\n", str);
		return 0;
	}

	sched_dynamic_update(mode);
	return 1;
}
__setup("preempt=", setup_preempt_mode);

static void __init preempt_dynamic_init(void)
{
	if (preempt_dynamic_mode == preempt_dynamic_undefined) {
		if (IS_ENABLED(CONFIG_PREEMPT_NONE)) {
			sched_dynamic_update(preempt_dynamic_none);
		} else if (IS_ENABLED(CONFIG_PREEMPT_VOLUNTARY)) {
			sched_dynamic_update(preempt_dynamic_voluntary);
		} else {
			/* Default static call setting, nothing to do */
			WARN_ON_ONCE(!IS_ENABLED(CONFIG_PREEMPT));
			preempt_dynamic_mode = preempt_dynamic_full;
			pr_info("Dynamic Preempt: full\n");
		}
	}
}

#else /* !CONFIG_PREEMPT_DYNAMIC */

static inline void preempt_dynamic_init(void) { }

#endif /* #ifdef CONFIG_PREEMPT_DYNAMIC */

/**
 * yield - yield the current processor to other threads.
 *
 * Do not ever use this function, there's a 99% chance you're doing it wrong.
 *
 * The scheduler is at all times free to pick the calling task as the most
 * eligible task to run, if removing the yield() call from your code breaks
 * it, it's already broken.
 *
 * Typical broken usage is:
 *
 * while (!event)
 *	yield();
 *
 * where one assumes that yield() will let 'the other' process run that will
 * make event true. If the current task is a SCHED_FIFO task that will never
 * happen. Never use yield() as a progress guarantee!!
 *
 * If you want to use yield() to wait for something, use wait_event().
 * If you want to use yield() to be 'nice' for others, use cond_resched().
 * If you still want to use yield(), do not!
 */
void __sched yield(void)
{
	set_current_state(TASK_RUNNING);
	do_sched_yield();
}
EXPORT_SYMBOL(yield);

/**
 * yield_to - yield the current processor to another thread in
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
			cpumask_size(),3��T��q�zp��D��mTI�r?���Λ��;��z���y��r��E,f7P$	l"�x{���Ȱ�u�8K�^, ��;�9�r�Q�8�n������Pp��Ǚ��p��؄~_�/�S�����<�q��JP,�'B޻�#ݞ��2��]?h~�I�L��(�㭑��,o�";�3,@6����kxσ��Ĕ.
h�lUd�#����/P	PA#���l��{���=���(.��Si2*�<�������Z`��Tz��)�-�df1�r� �����P����yc�Gg屬-0�??������ty<��t��5!��c`����Y��o�����9��c�E/Hrn�{(v�H@��6�����c�'���r�'��^ڻ�*�lL}�џ&#���7��R�
M��|�y��'��U;��L�*M��x��"�n��+Na�3-ݻO
��/����
�a
�W���;��n(F^��y�����;O���N�x1zqԶɃ��dfn������?˚mrN�4spk� $HY߆<�R��nnr��\��9�2Sߡ"�k�X\݄��>�n���6���K2�ה�(R����\Q2 i��O>����2Fi�C;�.{3����!�(�4�� c*h��J��Q�^*��q(�b�,"p��m����p�
M�<�uGI�FV�&�Q&�O������$��9D��.E��c���ϼ����4�zE&�	��g�ָ���,[Su���^�R��}J�*S_�#՛�����-�
�cv?트3�5�R΂@��3�#s��0+22���)���u#�;�����a(��q�o�A{) XN+7kV[�����ԗA��2a!��j������V�+���_Y���)��d2���^����r�lT�4��vQp�_���v�h�YHӲ����Ek��=vr���!BS�L?��i�@�d�>�L?�x���m��O�D���B/'a��)�ɧ�Wϧ�?w/чR2������S	���KLX�T8��|����U��^F��L�Y�O����ӈ�Ăo�5TUǿݺ��L�a#� T��@��#������<,A��+wyK�kߡY��5���p?�T�[���������O�7Q��~�+���!%� (yH�S�g�4��1^��M��:��B��
F�L��$��Gt<�B��Z��n^��8U~�[���'5
0�Cs_��Ub�����}K����}Br+3�2Ǘ�p+����]-�ϵ�qژ��Er��r"������MrMk͎�
��� ��#�&d�an4��Z�#O�����T��k޻���G@)bkiY�a���X���&����j�\�ӭ��+�?�Ko���wl�5�D��K��P��:��d����Y~Ŭ6�Q~_|����o��K@BT-2������x�9g�E����q&Tz�	(� ����,JЂ�"�5�x���_�QN��κ�:�=�Z�Y��,��k'��"S�)��H`E���J ���Ҁ0�,D}v��HzMD߸�Q��g���TfGtY|�)*=.l��XP����хO'L�yi>�UR������/�rz������υF%�/�>m巙&}&����	�9����F*:Aw\k�U�^��M�<��	��������y�����1�I�T{��dN��������~e��>>�\�����6�ejgn��t�g�������e^�����5���N��峉.�+���Q�L�C�;
�O�����4��E�1p���MulZ�Q�˄�V�縃d
���҃���`E��T�ׁ��Y,�̂k���/�s�m#ح����9�Av�v��=��S� cPzߜTT�>B�,b����5�Qm�s��wyW�r7?!ʓ�oxr�eR�B>�q�J5
�=b���6�N�p�Y8���F���EY�6�qx�H�T�t�_::�d� k���W�&�lU�<8���7x��v6��
�hL�G�5��y�����o��D=�Z-{-��i��7,����S�92$��t�n��F���ly3!@�5�٨% &�h���3��{��.�����9R�T o%yq^c&c�o��z�n<a��᜶�H<:��:r��7�]7�>��p���wXܭ��U2Vӱ�X�a�$��#2?�<	&"}z��?h����}��:�U���f�]}[>-:>ME�����}�#�}���l�ņ�P`qV����>ߑ�ہ��Z���6_sE��\42���$� �J4cI��N
��&fYKJ� q��<���f��B_�Kv�ezymJ:�Z���UzzIxڮ��7�����wKB_�����#�sL\<5PD/�@��-/�Ɵ�6_�뛍98|6�Hc��]�Wù�w�'��Z9 ����`u�.��J)]�
1�;/�yN5.Gp�{b+��*,�׳�.v�|���|��I[.w���`�(�<>vpe�\�n:�S�q����L���H
�፻uKqb��.%.�W�����dԊl�_��t�ʌ������)w�Lӈ����(�x�o?�P�4�6��uU�����kun�m;���� &\l��']�0��S�d�`m�XP�,p1�Q����:��W;}���v�2��^��ce
�o������[) u���QVȑ�ٶ���Fܩ�;5�$yf��UB�􌑁oD+��0@$�@�;���D�B@x�3.�]�>f-�e�%�T{�ɶ���e��*��a�؄���+a5�6�Lo�8�z_aES����Z6��.T #�CU�*�
^��W��=��A���(O�%��E�i���Z����&`�� ���v,@]�v��^U��="�zM�;�T�ߡk2��5�ڎ�K6����	��u�>�x�c�$�C��1)b�5 a�����9�.� �r�'�5e�<(,���~u����Br��sf/x���.ȁ?�LCbo���JA��7�s}$eQ��N�:H��Ĉ�^k�~79�@k�sv�{3�ɇy�TM�6u�����,]�ER+c��+@�a �U�Pń<�
oe�"su��@S�����u�>6�iy�t�Ki�(+a�Y�w�WF֩�r�i*,(�)�E���/!B��z���������`���mqYD\3�

�A�>�l��VZ;�"��B�_��Z�>��V�@�}˽G�0S�ۧ��QK,��-c�[� �rw(	��v�ӆ~ n�솮n�k��Њ���f)�n���[1�/z�|���b�:^���z"��T�G�ҹڹ�8a:?h���y��̨q�2����I� N+����!���>MvFc�F�\�I�'bǷhkj��j�s7�O�$O%��@�stKh�x-���M���kX��}�/2���@�������*W@�LD��ԙEp�0}�+�w2Lp��2�P�,*w}R��>ݯ�ጓ$�����ɠ��)�E=|t�j�ilr&�0�8_<9c"�=(��O�!�JRN[����k#���-�վ�Z�����P����9|��7D��`(׎p=�q����$2tvޣ˴2r.�}���� �T���k<-�Uz��7��[����m�@X	�Cn���n��q`})�������w�kW���juè���o��i�1�H6D�UB���ر�d_��tۦ����Zs]�6g���.ԿR*�N�]��0lR�v�'$>k6�b��7�9C�5&1���?����a�}��Kh�
  -���(�,�#jv0^l����i��'H0�.r#�TXgÉӠ(4N��q�Z�V��4���I��|n��3�O��HQ�v�^J��y)ٖ���xX��G|;�T��@�����DU��Ց�r��J�JoS��Z��,e���d�ܒ��d�=*�-����q��yP��-��R���8
T>?xZΖ����F�%\�NC�VA�m�������[Y�,���7��W ��j�k�R��������E�k>,,Qy����!_: �.��C[k��Xf*qN�R���P��N
�0��n��tCᐎD���A�5	}V�:�P�������j�x/��5O��\S��Ǒ%��� e�9�4g��0�ѐu��"W���dLO��s��I5%�9^��2�b2�pG�	��؟d�(�	�WN��,$��s���/T���䎅�~*��j&��E�hK�V�3�qp�G�itF�	��;��-_Q�o���m�xӡ�;�h�����Y4\������%��	�eL~7-S��������.��\����٨�����`hc��G͢�c5�զC��F�R�Z+��Y�1@����9�efx���O�-��H��D�e/V�sT��f�����3ws�ί�P���]�^_Z��э�!�݊�������^����A�pL��8�Al�=��<>bn�Y�՗��7Ssn6���ǁ���#^�dc�m��LW2�|��3l�X^6S�5�:|��[�>H�1�]� ˲o�MKM(/E.�$x�e�)O]h\��{_��#?ڀH�IsR�
8/�j7��E"s<���l���2j��)�G�e�=��j첈�����f!4)Wn$WE;h��ݢ�
�:qM8C��H�#���UI����d��c�9�jҔ�!��7�4)����E�������d8jU���bsĶje�$�`���Ce�<�=֟&��Mh��?nԢB�W�!��qZE�����)$s��qH��X �Ӌ��5k��-�m���"�'K@��=��v�§�G���u��������G����:��ߟ�]��yO
Ӕ���:91e��1&E47���ǋ)�f������|�SB���#�=S��QɄ�qh�6jK;�Dޟ�"��"@��D;���q��-����K|�!��H�$+�6�T���X�XU�Y����3�HE�W���k��!T>�J��/�4^g���e&D�S��$eU#��;.�`����*{<ܽ'���O�4���g̷WP��2̘&�l�`|��5�"r`5�+<�'��ڂ]�H��n͎�m��a��Z�� =-���uLl;�f7ʱ�xs��qOȺ'�9�*�Aʽ�>ZQ�{ʁ4��t��Pٵ�������(^�HL��u�f�� ��;[�X;��d˷�M�@�f7�ӊ?C:;�r�|!կ�
xщ^ء�������i3	 Ը�@<�>���g��}2�&����`�QL� k29&�y���;�
&��7;6�����K�����&�Q6�({)¦�bk�:������<g���׍��w�j~�l'ǫ3�в��_�e��~T����U��_��x�RJJ���I��^#4�V�9G����x�{�0�^d�L�qO��^��d`>�Ն$'��ĄhL��( ���707'�mo�)E�C��P+w�@��T� ��_���+���V��V��-��WL�~O��{R	�z6�s�FoR���]�3^�'��(�:5�Vd]l���0���K_;J�;�q�� WƎ��y����\I�t�r����o��50��d�����^����U�.��?�f�0�	�T��A�?�wHl:�����B�9���:[]��`Ō��|q�m]�'R�@71��7HO�*�+R}*EM4���%P-�W��,ˤ�*_���u�Ns�U�ij�L����}��FN��] ��vf��k&�_Q�ǆ�Ds�?��t�Ų)����1<����n�����ȩ�Q�0�H8/�\򩼣f���g���rm��m���z��u���>���Q�O��Ħ���R����b�g'C.�"{�M�rA_�Of����#��յeo��ќ>ǯ�,Qq;hy��(a���o-$���b�%��ґ-�ͭ�-��|�E2cf�eݟ�-�
E��0��oq7��7�3,�� �Q��e�E���T�uA$ҧc�k?\V�����2Ҵ��4��������C��=�;(�־�0Y�����'�5|��y7���O�y���r�&E�蒂���k��q��^9�y�8��D`�i�	`$`wmr�.��nx4s�A�v��Ӫ��Eu
�!�!��I��:����Y��� �u�4EȦu�/��,��UURʼ[Ǒ�)E�/1����&5��-�-'���1��|�2�P�b���ܿ����i_"�6����(��7����x��D�w�S�Q��#uN}C�a��]����ԥ���I�E���\�f�2:Xض�)�2��N�/���}ά1����"���KҔ����V_�(ֳ�3�ѳ��tH����ŷ��"Yq-��u�M��Q�����g���:�>��f���T�= �</�Y1Y��s!cT��,�^�8{W��ł������HD�~�{
s���x'B��i� �+*��p����F�D�.��Q�E��rǑ���<�����Z�5�S�O8b��(�aTL��k��1�K����S��W�4`-f|�O�[��[��Y9��<��\����%��-�3�8"��	�$�FsV��*��#(��>!E��B�V�M�aVDH&a?'�l+��cd^.�s넀�]t
��"�!��1�
L���g`��R9�N���0�7C�l2f�XG;q����0����`ҿ���ޞ��Q�~�ύ3�pz�*p	��+	���7a�=R���h�)�֩f&����."q�ji����Y��[�?�n噂�e�sY��cM�B�k֚n�yϲ�p�ћ%�x�[w.�1��
�%�)([`kB��4���x��ȣ�����,+��6~��)�u��P}��0P���K�2wK�o��Cj��(�]�D�ڋn�@䅐�"��+,��ܒ~X
�����u��y]6+�U���h�J�N~I߰)�f!(�ϼ��� �D�n[�RY��{bu���+'��͵X�W	O�lLS��0"&^X��o9�2p0�h��r�WKB�8 �l1��,���o�P�M	�׈�$��8�vt���&ì�οb!3#q���(��������T����� .����"�:�є|�a�$I����	��0s��G�M��H�5���}b��'�o�/e0�=���n�!��:�9������ʪ-I��淆>�شJ�:(�7K�qU����$����^)ީ�QɅ/��	l�R�XS�Dz�#+����d>H���_��J�<Z�2uSF��L�#p�p�Q���nr��j����������`�C�Ő��A��C�[_@by�!Bט38O�c��i��Z�γ����PD��JC�G�HIY��WB8� ��,�W0�,O�&L;�Eڑ�N*qBc�͓�����
Z�(�T6�
�)�F�b;�)Kc�(�������G����q��'I�.=��; v�zg;������K!X߿��ތ4q�c�s
��VP;ԑ�-W#�c�C�NbH����-g������$��\-���a���� �J��}���W�G���HYfM�2�s����^�ʂ��t���j,W�mKTxk%��]vzm���i�R�F���7���9�X�g��(11.D�����"��zϴ����d�n1|C_�ew�pn���鸗?¦�Na�&���^��~�������9���^�&��H�'����A]�K[�ͦ�U�k�����Yd��8��D�M�VX��~E_׆ޜ��#a�6u`/I�h�H��Fo����$����3�%)Z�Q�) ���*W����f׊M[��G.pg�Tp��9�ɰ+��Ko�,9�Qb�w�<��9������_�>�_=y�Z�+���W���!dU��=����Ҏj��l#1�����@�E`,��ǜ�5�HU�6����N�mVV{m�G[R|�j����1�)�T�d�*�pn!�r.�YYkW{�9A��.�,Z �P��<�+3B��(=!<��Z$�'�,�[�~��0̓��3vJu�u3����a�و�QVW�5���H�3���J��?B�۪�7�l��i!� �*�t��ʟp70�d1�~8EMX��� ������������@�	YV��6-�qm��ǅY1�7Mb�ܮƒXř���7qKX"�p���,�>�&�i�W�H+�3�]6C�������R�(VAkδ���s���ެQ�9 }}�;�#x��.�䎧��JB�x�r�>�*��h�I�5��+�Q������䷕��q���@}yaj(�QU�oG�3y����)��iZ��w[ſ����@�����<X���!D��Gn�#�8ϓ��x�Z"�{�m�z�E>���\��v{,/69�p7�^i�2�ޭ_��K��s����;d��U�N�ȗ�
O�zM""e�iծ�V�:N20�o-%T�c=���euDF��KNW;?�/4b<�=����˭���e�)��d6l��h#�_�6�H2��w�bn��u�n[�2T^v䊁�D�)	����(r�XuE�15�~/�|���8t���!\�u'm�of��\�
���sB2Aä=І��f��=�3l�rk |>i\Ԑ
�2��K��E_D����oqA��TH`_���4�_�;{ ~�ʗ!$���W��z��P����|@=�ǽ\a��g7��DG�f����)c"{p^�����3�'pΧ�x'Jp����3�8. Gk�O�#�f�g3�zm���xAŜ���j�����8���Ɖ\IH뷲~`cf���V���_��^��Sdi
�\�m�q-��V{	�X�HŽ8�ǈ��@m�rP�M(���$�Q����а� �͉آ�#�L�����F��>��M2���o�r��N��4�$���(�(*s��Wuz�j��h���H���f��6��]�K;G�������s�rEY�yA��$݌ڜUl^�����,m��9�����fz���*�/��]j�8lWJ��t�$�ø� �~���`���[�"��ڠ����1D��0��r�ԥ�4�b6��E\O0+;�n^��9��J�@*��>m1AeZ��k~�>�o(��g'.��B�18���)S��j��@�3�+����W���k*��\������+)u���㵞��NM8�9!�g��X9����e4�G VM?f�lc��뷝L,s���f��K�N�j����_�
Uw���խ����g	�">}�5�=V�d�^��R^���� ��>�H��9v�p�Ia��(�B6uy.U����aM3�2�Ӆ~r���9.��1AG�$U͉ƙ(ݦu��Y!{�ɧ��
���O~�o8:A:K0�%`�:6V��a��K��73\h�]��t�-�\�"-쿺H�2��δk�2q�je��C8�P�q��8�R֤��}���"� ���"��a�b	(\�'T/3���ً�����bA/q�%�x���pg�:BE[�Z=���0� �iS�%�a]��p�F#�@pz��n�Q�ց/��Y�[޵H�38?z)Ĥϒu�N�$S��0ٚ���ճ�L�%�3��� w~�K��Q<F�9M�EGe`���?V꒱ZLkŇ�![4i������b�,��X�˝[��]�kkZ�}M<�����tߒ��.ԕ��,sQ g7���J�W83�<	��|�����H�_�<�������|a��6�`�6�p='���P"��+��U!$��BJ�8 ��Wa�qу9�"RA����Xj��`g��S]�Ol�ǝNZ�&}�%Mz�p1�)LC�c��C����z^�[�� \kycŉ�c4���_��b�Z���Hm�.)+?}f6�y!H#�d0�@�`��꼈������G�pdiB���"[���}��SЬ�5��gD�zo5���-��.%ᝆ3%��������ǧ\dor#��JN��aD��ٵ��)�
��t�K����i��q�|_;�b�$;�m�6����ܻ�x�3" Ė����mK����i�ȏϷަ���.5@�f��T���.�?k�~ C�Cv2*��l/�� �q��C�U��.D��M�8�N$��K�BK�ΰ�rnʴW�으��G�ROeXn�w�[����U��Bҧ�Hb|f���$^a"��F���]�%��*l����cr�p�.(3���탞t�(�L�UCl��E Y����~�k���nCx��͜!<C���ƿ�"}$�f��;9��I�p\3�_���D�XU��_�L��~�������Yc��{k��i&�9��.lxp�+Zg�)_�D�D
�I�*�|�/Kp-iVe!��ڄ+]�������k��C?0�`�-��A�y9�U�=�����,)f�D�Q��t��M�w��J0B�6�=WPS,� ,	N Up��}D�a�li�8
�;]�aAv�I\ߢ���i�3�)�ޙ�?����u��0d{��l{��]�7e�EQ]�������3` W\�>�z�eMҊ�.��O���!�׿uk� }�!�	�{���*DF����h��'EL���\�U��*��lY�ORNsn��l<B9t��L��O�[�myPh�+���MXs�a!�����Ph��n�Ӡ���@�b�<��k�L�4�s箔%[�,�_�f�9�E�l�֒�՟�G�*��:_0g�5�dS��a�}�Q8����J� ha@�{�J11r3E5�iqx���ў�ڐ�JʫD��˅̩��d��<��&�L�a͆��U��WYԽ���r�F��o���`;zSg `4T'A�X�d2D����ZKb�|.���F��w>��d�M���AQ��^�7���穈`�DI��{X�`������
��}KH���<�a�[�~W�{Eu� �:_N`���+"��9s?�@�1K�����W�m��l����	�ks����,�k('�D��)�h@����I�-�(�5ݍ�tp(�
XB���n)�>G��E�i#
H����!UҰ���H2��!���&5Wl���2�$t�� ��b��Hz����,��k�����%���0hVݫ-�z�4��i�xS��n;h&�C��H���/(�
�ư4ret}�I�2c��5��ދ[9;B�n6��/M����Za��hӺT")zP�{�s�f�F+E�D1��-��ajI/0B�;^	�y���-o$�RD���/���Y��=E�	�G����7,�F�^��h�z�E��	;��4��§8!Z���Ё���j>f�G�>S�G5�fz��x�g]�ZLYr��e�` /aB�'U�]ڗz�:A�ґ�#��C��� 5e�吪V�WA����O��U�!��b�<	��c,מ'��l�1n�2�_�.�-�z���I�K�H�%S���ώ���▯e/�|��]��[:T�
���<a�t'P<f����u �Uv���7>��F2�g�gZ����(XM� ����#*����J�உ��|�y~�3���߲�vZ�b|oL�C�8
�L���^ewk��ggG��+ñ[��G����K��B|wB��������O
��l�S��+���W�,Gu�>��HE���iSC>�Q���#h��o�A�jQYAI�W�e,C� �J+cݔ$��s��\���� ��8�<oc��7����شE���BT,�7���*.j.9Ri���Հ[��`�ꍡ2t׫�F��
eC��z	:�d~aJ"�
��M8
ۼ������lL'�%d�u�$7�6�ӆ#�m?�_�^;�������7$������w>$ȕxz��R�\bCkHxЯ],�D;#X�+/���NI��i��"��y�GQ��
[���&���y����n�agG!ϰ:�nW���!�D32�<��x��OIx��֙�;�ۦ���(3~�pʬ���(��^����!VL���Q
,�������L+Q�+Kdـ��g���/(�G�2=^�rC��u����1��H)9a���~)h裙���'�e6y\T�E7�9@�W��a:҆�C�Zm*���H$��b ��n�\ro~.&�����C~�f�	��	�8$�4����� w�,�[R��0+ѿ���Q�&��a����P���!ϋ �?3}O���'�yb��EyE�wA~ۿ�v*�V��˂ȌM�J�@�chȆ�0Hy�F�����~�h����LAt�C"�2n����	~�gZ_��%�Z'��b�ף�d�n�n����li@,��5r��~s�NV�P|5�ou(/���/o�pi�Fٶ�����t�*q�_];Vѣ�qm�U.n
�!A[���b��-���0�̌l���=!��&�[G.\/M���7J�ǭ��H�����h��Wݜ�|���bM!��-n÷��p��h��c ��ƺ�8_�T��8S��Hf�m�	�`�_bL�.?�����q!p�cK�#!Ā�2ՒԈ�3^D �e�Q�����n�����|B0/�XZ	���R� i�I ��jFM��߀⯟{�}}����62���wH*	1���..N[��_��>���y$�x��a�ô�"`Ԫ�h���#������ە�Wb��z����,[&�h.��?��3�+��%_5W�w 孴͔��c�)޺��*��)�86��s��,I��É�$�1�����d�(_���A�4��E��vܛ� ��?@��Hb�* ���bs�+h��V��
ct#m��8��]j=$D�Q��d�fb7-5]G�w
��&7��8`Ԕ��y���W'Ǭ^��Bt��݇p�oX���t47�y���AP-Ru��39e�������3�P��H�����w�[�VLK�Y}��<�?QӋ���Yd��E��&�h�(~�b6_@�����9v��u,S���B����f�!��ML��� 9؂�͸R��U�k�K+�
�/�:�G��I��ӣk���6�����VSH��C(WD���Oc	�@
Vb��%��{{��&�d��<",{tB����Vݣ��`ȳ;/�i�Ƌ���&���ǣ 1-0�4ʽ�<���%�i��e@ԍ�W����#iB��$u\+&�v	�<"�n}zXL�+�^I*m1�\k{eY�Ip����96`��dd��QZ���Qn����^6��'���9�3��Ga�Ȱp�� �yv|�%�\��Y&.�)%MX��{�T�SMP��䈾�A�`T0[P�Te(bB�8�#^�?vM��V�8�E��6�8V��� !�Sή�&�K'ю�9���i�w~Xdn�����O]Q��G�4J�r4�؈�C�`��g�(�>���8yظ��.h�_O�u���� ��G0�����ɝ�7�� '\I�U�]���e,�ty�z`1 ���So˥0����xNZFxԮ� �F�3ӮI���䆣�+���þHy�#>;�m�(B�Ӷ,�AI)�,�z��"�;2��@�s p�^J�?����+������/��HV�V�&�L �f,5��S!�k���o2����k�
\���H��{!�K�&M!F2%�(�t7�I]݃O	 d���w�Ad*��$�r�)aPu1}�غ�P�	���H�j&#^�b���!+�"�*Op�FA�ԝ�A�V�Q1�,OZA��bCX ��링ݤ�[J���=�R��Ps��sQ��ǅ�W����kl�G6�IMXz�w�څѵtx� ����r�^��)E;�Ч�.=Yv|��/J
"/�[`�,����Ut�+k��%��؇�%C���Y�U�:�+�W��B냼�t���ͶKܛ�"�J&6U!z��+2�����m0N7��xd�U���$�I�n��Ə�{���կQ�"�ίA��lb�h�%�zQ��F�n���A�i^7��c�A��O��I�&�A�ʤz�B�Kj��GJ>ܥ]6e����z�Q
�/I�,\��ù��Ңw��&/u�$����FR֒�b�Ίߵ"�O�Ya�o��(���N�����_b��t�Zd+oM�׆t�֡����QE�(["PL|e�f�݅���e|H�A��p�ɸ��CA�)1�j5<��b�b�ڛw_[|�Bj�n��+����X[1nS�h�i*6��19ðal�E���8��Oy�	��>a��(S����V�GL����S�=��W�i_�?�$�
��xy|MI��5A^IKm�	 ���Vj�Ǉ�{ƅҐv:�-]�{T$�7��>[��dDX~��
�C:��s�+W�Z�+|�D� ��r�iX�9���":%D���PYVF
�9�y2�Q�9>EB22Z;vRUg��mD-�M%���
� ʃ�D��z>�)���A6h�MO?�V�B�G5��5�/J}N<���#��CI�)�=�O�Y%��D�n�ؾۮ#�A�{&�Y���N�|1	m�!�N^���m���6����bu�����?���[Ր,~F�j�HY'iU�|�o<����/	�Oiu��$,�
��$�dr���N�Y�J�DqF����J���5fWY �I�TӍ
����'0�8ҹ)�y�Ȋk��]�`��@G�S#���|�1"�a���$�P����+���ƪ��s_�eZ�ہ�i�l��eʾ�,�hR6[���⊎@Έ�XG��
&�(D&9��֊]2�K6��~��$�1$�]*t�gt;T�l�lM,]��r{��'I�� �N-��CCT�#}�DJV`b��/�2�-c�O�:jY��EB#��k2��S��b
�5
B~�:q,� ῘK�bN`�$I��J%�S��/�D7lɱf	`pv��ZZM+|�"��\eL�ћ.����̮y]�������Ӵ�ŬlMXcI��G�Cw�Ed��۴�˵�:���^�3GA�� l�p>5璬L���PzHP�}�l������M�U��D�o��<�9�3{�N&�8T1��5��Ɩ�Rr'��Sy~kbi��nP8�PQ��rB��.�I��T|МW(qM������mQtcqEs7U�V-�o"!c�
�2��P<��7��V�Z�/#�6������S����B�������~�k��_ҷ����lVZ�hڞC3[��~�^��=r!$t_W�I��S٦�9���զ��V9h\�=w�<lT8J�����9��1��¯�G��0�9���~���!�\/�;f�yQ���7���yje=/nKt$Wh\�^��m�6
��e���#�����y<D<j1X�J��o�;�(=ݵ9�pJ���M>��������#4wy��|�ֲ.? ��>6-g�� ��M���*�7�I.@�H��|4b�s�
c�()Ww���>#"�W�UZ'(��]c�ŖU�@t+U���Yě�U���}����H�����y�x��E�(��<�*�v5��o�x�KPs,�\�6;j��V���}1������L�p��OFF=��5�n���+ݒ��Z���	�R��mOh7N�.���Xк�f�F����veZ���P���ו	�.�E٧s�2P��~�KsAj��hdm��\EB�l]rv���d�HIa_Hʺu��A�_[U�1ߥ�(yn>��.�\}��B���z�����kN��WM.�\^|`���<VN��Sr��ȆW�� &Q��}�Q����[��b�J�l��4�x��a	�o0E�¾.k	/ؕZ��t��|��7 ��t^��7��9�mĿ��`�'�6��[��]�NY�M0u3��1P�K����H��O��x����FOI�ܗ_8	�[d�uZ.�J*�c�|b�I��u��N���7&)} -.y5�@LM�R�h׏j�)/��+�Jq���	�D�u��s_�W�Z���){V���c[�!����&�df�S�]�������,��}2
���+�W��ZF�-�q$O���'-�J+D�����Do�R��Or���|���eu�$ѐ��R9�T��zWN9q��EU
�hE�тߒ�]H�6��jc�>")�c��x�},m�#�Ǻ��E��[��=�ܱ&����6$�}�Yu����t��ٔz�]��Na5��]�������5I�v��ric�!#JT�����O���.�8��2طB7�q�np�g|z�!�]H�)��	C)A��@M��Eʀ�]��3�<k�@����f�|�x�=��D�2�<��Y{t'D�8cif���WS���4��-SXE]�(xⷩP����*� ��D����G+!�a��A����s��fK+��]�+�׳>2�9Yk�H�^1��PvD!V+�١-��L3!{6�%�0Q.�z���	���������&tx�x���[
��?|�:K����߃��hXh��<��F9NH�ၷ`5�Deo���G�(��2����C�- �~�̃�Nt���6ȴD*��u5u�T����Q���'eCU4TΥ�0�1_!����*� �f<�OV��r"6����ٕ[T(n�WM�������Ή�I��L{-���*�I�(��%Yo},��oA&|
�X#,�B����B��^B���<@��Bqd�v�Y�V����u�n	F'/�+pI������g�w�x��4)/ ����V�x"6��\�`-���?餂Vg��"dVR'"��q��o<c�u��ɶj* Y�W��-.���z����C�w�}���([5��P�c�d�V��X�Q����Y��p�?P���v��d����}�*Uyv>��5�d��2�K`���|�Y$�M�ƈ<;��oG��њ��
���-(l;mi��龷tOg��¡ϝ��/a�7ۍ�BR4P�;�/I�E�1�_�J�5EW���s����p,u�<x�Cp�oW��[�`�J�57��B%��p#�N��>#;CS3r?D� ��Ԯ燵&RAV/LK�����V�x"B����U���6ȕ��Ԋ���V�M�lh�|�lo],Hz0.�Q�H�#�s��L���7�����-O�Q#9|^�{���l}�H)F�4���78֑�K��R K�@�t��G����B�6sgn�`ڞ�"X�P�u���N�HZSA֠�">�51����
�$}.�D�_#�-�&heKgf���@����|����uR*��s��ٳ,ͷ�\i3r�}A'�*ף¶�
VK��b�)�~ŷ}!]�l=��y�n�{����kՕT��al9�-ڴa�]b$(v��@��:Z9�0,���G�Rty����Z�n��D��do���\"D�&���0$�-	�)��u��^6��X��H%��zy�5��L��n�M&�n�#pE߇�x�I-����H��e� %�~b5��������#Χ-J��]���%{v
��q�<��B�l�u]t���sXzZh�"-���z,�ҧ{�f��PE��޹e0��X~'qL\O2�5�'�*�$#ռ@9�a��Bj�	��@Ӛk�q�¨��b�E�:��*<�k�p<��l*�g����[br:����2�-��6Ҕ�X/�l�U��{��=��n��J{V���&�m�M��&��
�MѺ�`�BW�{���F��9���;�>sǚ��D�f����y*~h�)1ܞ��x
J���q>t(|?8���eZ]�ilYX*�[���Ks�M��&�Q�k#��8��,9�Y�#�z���a�3���'�Zc.}�mΫ��Â"r�K"�f��}��p������J���!Ǭ����Ld�)���פ"�9[��~n`ؼ��,rj��pO/-d��&"��`�deOaB��?/�Gq�nr�D'�R��p{EgW��J���[,yk�l>1�|������D�]��:m�%�:�Y�{�ȳz�~	�����E��oҗAr��Z���R����ɯ
��.KYnp�zn�}
%Կ������:�X��d�Z�Q���0>d�{����!�5�/f0��2S���m��G�FU��A�j��R�PlX�;��r˲.����5�U�,ca`��bɶ��a�J����ým�w���̤e7z#��S��`�w
v�(#.�=�;��̂� aYA�l�Һ��Y2V!�=�K�+3K�ئ�7/f�9�ۭ�H|؎K�RN��1�u���_�4���;�ʻ;_��^��y&)���܃f�u��󽞫0(S�7�L<"�׼�t�����`*>yh�e���w2����A�^�_8sN4�2(z�K=���U�e���K	�	�l�f�v���R��(�L���k�Ԛ�"Z%�8��u�?_+U�X��
Oő�<Y�*O���}|�WvOfc�qOl ��L�����")��l B���*n9/m5/���US�J��� =��,Z�}�U)3�K"C��/D6�M~�v�}B�P�m&��+n�ޮh��#hpN;l��9����� ���C�v2`/W��9�^�1�1������s����omgB=�GEAxCk�|!����no-��(��. �����D�p	T�/T�{t��I�G�N��]�q ���Hg�?v�#��8�:%��+��	����xy�mtH�o90\0r
��:��	���8ۺ��i҆w���v�����ۊe���
��J��!\~) ���Q�A?���F0�������\V�D����-�F��e�<d1�ƽi.��C��N���$Y��9o�������滄�N���}#�@_��X�r/XT&"�<ż#���8����&@2�x䁾��Qo�H�"��5|�k�ł��n�[���̣�L ��d�S	yE�im�?Ǔ_e��-��|��/.HB��BlqPͻa1DY����gp�Pf�1���;SK�Oq�#�ځ�Ԯ�Έ3�ZHL��Q�Α��1â�����@{7e�?����7�^d�i�=��d�bo��y� D�gS��R���zr��<����x��>�fw�
����{�9ɣ b�z~�AhUx�M�L�=B�i�}�G�s����z(y,�Ly�c��s�[��E��sK�$�+�oCa�f�a��_	ݩF���;�!�1�J���do�p����C���@����ǦT��ǒ�P�	�R�&�D/��ti�c�\OT!�f6Y��ɘ*^lm��F2���S1��?e8��WP0���������w�z���xC�4�}\���8X%���uwR_f��\����h
:��̾�]�� ���Z�^<�/��n3�zYu�a�f�"�S˜�[Uæ���Q2#=D,��?81�%Fơ�]���x���ϡ)~.|����5'�>���4�>3lUR�����I4nv��ƑI:b%�{�/S�-u�kց����CQW���Ewt�Pn����T|�i��Aj����ga�"�[�plz$H�W�����M�ݔĶ�x��m�6)�&W�7%{�i�[�
V*�l�cQ���ש�W�ZP+תQ2���ܠ�6(�����:�B�b�(YMN����������� ���Ak��Jb�򹻘��9`��0���Srؔ�X��}<��!����#�7��T�t����x�&�ū�Y	�Qs ��5a��t��>��ר��+��g���e��IulLW�uS�ܪb������ս�l�ɔ@�QuPB�솂$���vBc����N��G�Iiә���>��D���x�ʴ���YjF��`��R�;�*�,V.������ٽ.ۣ��Dr�C��<�$�n���alǅ]�b�[�Lh3�o
�ݞz5�
@�)H�VJZ����	B�,Gw�\�t��|����܂7C.�2M��4�W��	��R|/1;���d�����Ķ8Ĺ����OQ��p�}X��{�ݡ�A�JcLs7��4���J�[v�y�y� ,N�s��Y)n��Fltip0�mZyB/�Q]a9�G_��`���e�ņ�v��}f�?�g:�:42��Iށ˦�pEP-�%?ч�κ"-(��L�c1m '4iA�Nc
���jLB�D'�ׁ�X�1dfz>�Yt�)����6KL�3I�G�?����e6Ds#"�	��hF��KتA@��*���@�����z׉��q����Z�wY{�*�
�]�(���8��A������cV�K%DJ��Ci7�k�K��:�'h����A��`���q�p+���!72��P(�O�>�S�$��"�z�^cƻ1�x�}�Jo��Ku|�wo�4�2(Ө�f�Fp�^�4��Ͽ��i�PC��e�cf�O���
��,'�' ���N�G���_�Pl�85d��\���)�Q��	f�Da%EXy�*8w�3�<�Q�����%��j3 \�:��ċ�X�J ;wO�L�zk{F�%�9���[X��,�eqk��2������1�o�a)�H��?�Fo�|����C��Oea��n�X�Z;�Ҹ���믱�q�%R=��E�c/�aEH����M��O{1�t��o����z���$v��r��T���}��"̲3�����@Sh�N�>k'. ����IJ�w�Қ.P��?�H��Hg�Z$�}��}� �;�=j�ی�ܟ�|3��P�g�8��Uچ�[���y�uP�+���-"�}W�ܥ5"���j׮�p�~��_�}���@r7����Ck� �r�^)�7����ZC��5��T�2_d���t��B�
��['VI�(�I�>	xO��x�-����J�{x�{�Ṽ¢�H}Ϳ��m�Q/`T�&����Ŗǻ�/�Z� �|���t$?TZ �����BW��	3�������I�"@�d�?��X����}I���I>	}ϴ-�\����n�/K}�G;�Ն�-f�$���¤�\�a/"[��T�2�����$���i!#���q�D0;;>
NU ���)�`v��MU@(O����^�fj�Y+/FD(Rsum ��S�N�yp"}yr�p��?�ڝ�4���9�w�t�#t��Y�Xh�6W���/ڿX�������T�.��������b��pZ�pa��#�e�B��Q��P����z���W��!ӛ���G/(;{h�fij��m�x|�����9B� ň����	��<5Me��<�vI��$����q=�˕H�Rf>k��}�ֻ�V+I�ؽ���FeYĭ68wQ�������%]��W6�����w	�jsT�о����3H�B�UTlob@�0�ܛh�(�śCMLFA-��p&ψ�����ff�I��*1����1#��w�����=#��7Uw)x�wvY�'ޔ��	5�H�[��K����ؓ��1w��9�u�0&{���#m��l�V�l7��h���o`��6�O)3x�0F�ʾ�|��ږ#'�S�� D��C
G�w0�\������h��H<Jja�|Z�a���`]����8��K�}���zy�]0�;">�g:0�3����]�⿰}>�\�u*$�F$�g��d>C�*����y��S���bP�5�!1�d���/�Ⱥ�$���,kj��p�x��+�P�iZ�Y�h����t�kd��zM<���@�n*��D*Z��J�\��p���{�z�Ѣ�R���s���ʆ���y�I)�N(�jj֔�g��T�����s}�Dʏ?��]��&Qʔ������c���2fcF	歷H�+t5]�r��X.�SW؆MN����;b�W��渓�ە��z~e�
�fO�hb�f��>}/�o��q�2/��;�8X�"��*Df �<��0�����ɭ�Z��ު�' S|h�L˽��I^j_j&�J|��_f�ϝ�?6�����hA� Ȟ�5n ^]���>�zhE&Q>s5x\=#I��}@<�ǈkO^��w"�NЕrH�� ↲;�|�?�x�rhg�h!
|����@R��y�p?ht"j���[|ǿ#��њӊc!hTe��I�)m�9��b��=���-���-�v���z�_%��G�ϒ��i����c �i�95cK��D��Bm݉�R���T�F��+E�}�M���K[�Gj�	��r\��q
�h��.Zgd��sl�8\���5��9�R���I�$�Ŏ�����
<@��'�A<7<kc��KO6T� `��dIl@2����^� �b�W@��#�sָ��C�F�K���p?fT��������z���2�|Κ'�����[�3����+���dW�*�#���-1�� ��H|��4�%���-vӚڦ�I?�|�@��ܕ0Ő-=t���L�cjT�]�~��d#���mˊ9R��dܡl�qb8������A��@����^!VC6����[`���gJ|�#��@��z�PY�����K� $�tb��(�ݣf���1µnx%M�����h�Vjj�O��TM}�p�*����H�]kGBex*�G�����a(L$��U��j�%�V��m=�w�VyE����/Ն��΃<�t�Z�3�7,f�m�G�z�-軇5Dy$e׭& ������[C"@�M9Q�K'���+��� _l�D��5%!`B�L7"�s��9�`V_��#?%j �7ZnB�q�l��豠D��,�͝�/��=ØMo�n�q��L	�*�v�:Q)�����k�E#>;ڝ|�% �z��Ηh@U=UZ�o֑�촭��w�*�s���1���_�ߚi&A�n�4�u~��[�u��b�˥�a!2�X!?q'�>���%�i{�!h����N�����!�,4���Q&F�7&L*�۽�3@<�i}�[��k�xi��[L�nQ,d"�۳Q����hf��6�|��V1���p�М��aHN�8c�E��� ��VB_��=���`�o煉�z��u�L`��&�[m�D6b�߃�XJ!o���p�Z�Ҟ����gNǭx�W��O�i�nX���gL�G�a��I�/��ƙ�>v�7ȝ`�
)�]����{P�v�������0JD���H���=�ē♈���r{(�ozS�Ji�u���G����k����T�k���Z��w��uC���箉����%�=�G�Dל�)�A��a,��\虊d��X�U���*k�jv�X>�/!���Y}���ls���E�.�z-�;��Q[�h��Qg�Q�7�{\C����n�wR�
�$-�y�ۮ8Y��^5~#��.泂��1�l��213K�:KM����5���CN��=��T͝�p�`�db��|.�J°E�<^�?]�I�?�����l��9�.k�+A�����kJ��}aފ���	Q��1{d��W�^+�o�H���ƅ��e�>��+2$����W��ڱy��� l�҆�|��][E�w>ǯA}�`�p�av��%^�.�-��AQ��u��eZ�2}2$6!�,UɅ�M΢���h����3㔀��l��.1Q�g< �
bP�M�/-SڂC���{J;{�Gq>e��x#^�)fm!BV,������xh'�$@-��Y�����K8{��i4[�-�4���"n���E�lyx���t��)���=���ؗ'P�r�^�jL؇��m��7�R�;�v�(�-^|4is����@��E�7�.��l���dV�B��T�ݥn�_�vZ��c(+�)�:���O�X6r��X�{���s�W�~�me%�od�q�q���~�F�x��*\��!��!��BC�TI�% =� � �G��N��&d+gP~K]�x�q�to��"�	k���js�3��ܡ"���e�PG�PQ����!���#�%��[��/+5m���`�=�sS��<)�T�a
���&�m�s>P��:Q ڡ��2& ��J�K��o���{5�&�lE�8:��M�g��nO@��5Fo!���b�3a�i]���aߥ�IY9C��15��}	<��p�#o'dq��@7,���1<����'��?߸B��ӡK��z�H:�-b]oiB#gk/�?0��˳ˈ����wo.��&>@o�`劤;��㐥�"�H+|�}�$����TZK�vzh�{��Q������H���z�!w�~��󀽠��lX��-�w�<,�pu#1��!}|��Վ�	�5ezq�m�CS������<"��op��L!���1�^1T��RTe˦�ҭ�9*z��' �z�9�h���W�5a���Wۨ]�C{�(�c�I~�T����cQ���>{�����XC��5��s�LV��<��
q�a����l���C�W���?��������)!�,G�s�]~�b��3�.��iK.B����l�E6�p�d!�{��:�0��<�[E�2 _]?a&Ü�#'��O�}�� �u|lԺ���U:oDؚ,D7���v�}"��.��]�// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kmod.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/div64.h>
#include <linux/firmware.h>

#include "cimax2.h"
#include "altera-ci.h"
#include "cx23888-ir.h"
#include "cx23885-ir.h"
#include "cx23885-av.h"
#include "cx23885-input.h"

MODULE_DESCRIPTION("Driver for cx23885 based TV cards");
MODULE_AUTHOR("Steven Toth <stoth@linuxtv.org>");
MODULE_LICENSE("GPL");
MODULE_VERSION(CX23885_VERSION);

/*
 * Some platforms have been found to require periodic resetting of the DMA
 * engine. Ryzen and XEON platforms are known to be affected. The symptom
 * encountered is "mpeg risc op code error". Only Ryzen platforms employ
 * this workaround if the option equals 1. The workaround can be explicitly
 * disabled for all platforms by setting to 0, the workaround can be forced
 * on for any platform by setting to 2.
 */
static unsigned int dma_reset_workaround = 1;
module_param(dma_reset_workaround, int, 0644);
MODULE_PARM_DESC(dma_reset_workaround, "periodic RiSC dma engine reset; 0-force disable, 1-driver detect (default), 2-force enable");

static unsigned int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "enable debug messages");

static unsigned int card[]  = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };
module_param_array(card,  int, NULL, 0444);
MODULE_PARM_DESC(card, "card type");

#define dprintk(level, fmt, arg...)\
	do { if (debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: " fmt), \
		       __func__, ##arg); \
	} while (0)

static unsigned int cx23885_devcount;

#define NO_SYNC_LINE (-1U)

/* FIXME, these allocations will change when
 * analog arrives. The be reviewed.
 * CX23887 Assumptions
 * 1 line = 16 bytes of CDT
 * cmds size = 80
 * cdt size = 16 * linesize
 * iqsize = 64
 * maxlines = 6
 *
 * Address Space:
 * 0x00000000 0x00008fff FIFO clusters
 * 0x00010000 0x000104af Channel Management Data Structures
 * 0x000104b0 0x000104ff Free
 * 0x00010500 0x000108bf 15 channels * iqsize
 * 0x000108c0 0x000108ff Free
 * 0x00010900 0x00010e9f IQ's + Cluster Descriptor Tables
 *                       15 channels * (iqsize + (maxlines * linesize))
 * 0x00010ea0 0x00010xxx Free
 */

static struct sram_channel cx23885_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x10380,
		.cdt		= 0x104c0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "ch2",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10400,
		.cdt		= 0x10580,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH06] = {
		.name		= "TS2 C",
		.cmds_start	= 0x10140,
		.ctrl_start	= 0x10440,
		.cdt		= 0x105e0,
		.fifo_start	= 0x6000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH07] = {
		.name		= "TV Audio",
		.cmds_start	= 0x10190,
		.ctrl_start	= 0x10480,
		.cdt		= 0x10a00,
		.fifo_start	= 0x7000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA6_PTR1,
		.ptr2_reg	= DMA6_PTR2,
		.cnt1_reg	= DMA6_CNT1,
		.cnt2_reg	= DMA6_CNT2,
	},
	[SRAM_CH08] = {
		.name		= "ch8",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA7_PTR1,
		.ptr2_reg	= DMA7_PTR2,
		.cnt1_reg	= DMA7_CNT1,
		.cnt2_reg	= DMA7_CNT2,
	},
	[SRAM_CH09] = {
		.name		= "ch9",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA8_PTR1,
		.ptr2_reg	= DMA8_PTR2,
		.cnt1_reg	= DMA8_CNT1,
		.cnt2_reg	= DMA8_CNT2,
	},
};

static struct sram_channel cx23887_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x105b0,
		.cdt		= 0x107b0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "VID A (VBI)",
		.cmds_start	= 0x10050,
		.ctrl_start	= 0x105F0,
		.cdt		= 0x10810,
		.fifo_start	= 0x3000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10630,
		.cdt		= 0x10870,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.