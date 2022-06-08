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
		enqueue_task(rq, p, ENQ