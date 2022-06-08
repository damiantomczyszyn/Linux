reempt_dynamic_undefined = -1,
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
	int retval = s