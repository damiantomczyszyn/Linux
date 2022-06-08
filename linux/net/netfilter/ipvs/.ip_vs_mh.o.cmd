{
		if (dl_prio(oldprio))
			p->dl.pi_se = &p->dl;
		if (oldprio < prio)
			queue_flag |= ENQUEUE_HEAD;
	} else {
		if (dl_prio(oldprio))
			p->dl.pi_se = &p->dl;
		if (rt_prio(oldprio))
			p->rt.timeout = 0;
	}

	__setscheduler_prio(p, prio);

	if (queued)
		enqueue_task(rq, p, queue_flag);
	if (running)
		set_next_task(rq, p);

	check_class_changed(rq, p, prev_class, oldprio);
out_unlock:
	/* Avoid rq from going away on us: */
	preempt_disable();

	rq_unpin_lock(rq, &rf);
	__balance_callbacks(rq);
	raw_spin_rq_unlock(rq);

	preempt_enable();
}
#else
static inline int rt_effective_prio(struct task_struct *p, int prio)
{
	return prio;
}
#endif

void set_user_nice(struct task_struct *p, long nice)
{
	bool queued, running;
	int old_prio;
	struct rq_flags rf;
	struct rq *rq;

	if (task_nice(p) == nice || nice < MIN_NICE || nice > MAX_NICE)
		return;
	/*
	 * We have to be careful, if called from sys_setpriority(),
	 * the task might be in the middle of scheduling on another CPU.
	 */
	rq = task_rq_lock(p, &rf);
	update_rq_clock(rq);

	/*
	 * The RT priorities are set via sched_setscheduler(), but we still
	 * allow the 'normal' nice value to be set - but as expected
	 * it won't have any effect on scheduling until the task is
	 * SCHED_DEADLINE, SCHED_FIFO or SCHED_RR:
	 */
	if (task_has_dl_policy(p) || task_has_rt_policy(p)) {
		p->static_prio = NICE_TO_PRIO(nice);
		goto out_unlock;
	}
	queued = task_on_rq_queued(p);
	running = task_current(rq, p);
	if (queued)
		dequeue_task(rq, p, DEQUEUE_SAVE | DEQUEUE_NOCLOCK);
	if (running)
		put_prev_task(rq, p);

	p->static_prio = NICE_TO_PRIO(nice);
	set_load_weight(p, true);
	old_prio = p->prio;
	p->prio = effective_prio(p);

	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE | ENQUEUE_NOCLOCK);
	if (running)
		set_next_task(rq, p);

	/*
	 * If the task increased its priority or is running and
	 * lowered its priority, then reschedule its CPU:
	 */
	p->sched_class->prio_changed(rq, p, old_prio);

out_unlock:
	task_rq_unlock(rq, p, &rf);
}
EXPORT_SYMBOL(set_user_nice);

/*
 * can_nice - check if a task can reduce its nice value
 * @p: task
 * @nice: nice value
 */
int can_nice(const struct task_struct *p, const int nice)
{
	/* Convert nice value [19,-20] to rlimit style value [1,40]: */
	int nice_rlim = nice_to_rlimit(nice);

	return (nice_rlim <= task_rlimit(p, RLIMIT_NICE) ||
		capable(CAP_SYS_NICE));
}

#ifdef __ARCH_WANT_SYS_NICE

/*
 * sys_nice - change the priority of the current process.
 * @increment: priority increment
 *
 * sys_setpriority is a more generic, but much slower function that
 * does similar things.
 */
SYSCALL_DEFINE1(nice, int, increment)
{
	long nice, retval;

	/*
	 * Setpriority might change our priority at the same moment.
	 * We don't have to worry. Conceptually one call occurs first
	 * and we have a single winner.
	 */
	increment = clamp(increment, -NICE_WIDTH, NICE_WIDTH);
	nice = task_nice(current) + increment;

	nice = clamp_val(nice, MIN_NICE, MAX_NICE);
	if (increment < 0 && !can_nice(current, nice))
		return -EPERM;

	retval = security_task_setnice(current, nice);
	if (retval)
		return retval;

	set_user_nice(current, nice);
	return 0;
}

#endif

/**
 * task_prio - return the priority value of a given task.
 * @p: the task in question.
 *
 * Return: The priority value as seen by users in /proc.
 *
 * sched policy         return value   kernel prio    user prio/nice
 *
 * normal, batch, idle     [0 ... 39]  [100 ... 139]          0/[-20 ... 19]
 * fifo, rr             [-2 ... -100]     [98 ... 0]  [1 ... 99]
 * deadline                     -101             -1           0
 */
int task_prio(const struct task_struct *p)
{
	return p->prio - MAX_RT_PRIO;
}

/**
 * idle_cpu - is a given CPU idle currently?
 * @cpu: the processor in question.
 *
 * Return: 1 if the CPU is currently idle. 0 otherwise.
 */
int idle_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (rq->curr != rq->idle)
		return 0;

	if (rq->nr_running)
		return 0;

#ifdef CONFIG_SMP
	if (rq->ttwu_pending)
		return 0;
#endif

	return 1;
}

/**
 * available_idle_cpu - is a given CPU idle for enqueuing work.
 * @cpu: the CPU in question.
 *
 * Return: 1 if the CPU is currently idle. 0 otherwise.
 */
int available_idle_cpu(int cpu)
{
	if (!idle_cpu(cpu))
		return 0;

	if (vcpu_is_preempted(cpu))
		return 0;

	return 1;
}

/**
 * idle_task - return the idle task for a given CPU.
 * @cpu: the processor in question.
 *
 * Return: The idle task for the CPU @cpu.
 */
struct task_struct *idle_task(int cpu)
{
	return cpu_rq(cpu)->idle;
}

#ifdef CONFIG_SMP
/*
 * This function computes an effective utilization for the given CPU, to be
 * used for frequency selection given the linear relation: f = u * f_max.
 *
 * The scheduler tracks the following metrics:
 *
 *   cpu_util_{cfs,rt,dl,irq}()
 *   cpu_bw_dl()
 *
 * Where the cfs,rt and dl util numbers are tracked with the same metric and
 * synchronized windows and are thus directly comparable.
 *
 * The cfs,rt,dl utilization are the running times measured with rq->clock_task
 * which excludes things like IRQ and steal-time. These latter are then accrued
 * in the irq utilization.
 *
 * The DL bandwidth number otoh is not a measured metric but a value computed
 * based on the task model parameters and gives the minimal utilization
 * required to meet deadlines.
 */
unsigned long effective_cpu_util(int cpu, unsigned long util_cfs,
				 unsigned long max, enum cpu_util_type type,
				 struct task_struct *p)
{
	unsigned long dl_util, util, irq;
	struct rq *rq = cpu_rq(cpu);

	if (!uclamp_is_used() &&
	    type == FREQUENCY_UTIL && rt_rq_is_runnable(&rq->rt)) {
		return max;
	}

	/*
	 * Early check to see if IRQ/steal time saturates the CPU, can be
	 * because of inaccuracies in how we track these -- see
	 * update_irq_load_avg().
	 */
	irq = cpu_util_irq(rq);
	if (unlikely(irq >= max))
		return max;

	/*
	 * Because the time spend on RT/DL tasks is visible as 'lost' time to
	 * CFS tasks and we use the same metric to track the effective
	 * utilization (PELT windows are synchronized) we can directly add them
	 * to obtain the CPU's actual utilization.
	 *
	 * CFS and RT utilization can be boosted or capped, depending on
	 * utilization clamp constraints requested by currently RUNNABLE
	 * tasks.
	 * When there are no CFS RUNNABLE tasks, clamps are released and
	 * frequency will be gracefully reduced with the utilization decay.
	 */
	util = util_cfs + cpu_util_rt(rq);
	if (type == FREQUENCY_UTIL)
		util = uclamp_rq_util_with(rq, util, p);

	dl_util = cpu_util_dl(rq);

	/*
	 * For frequency selection we do not make cpu_util_dl() a permanent part
	 * of this sum because we want to use cpu_bw_dl() later on, but we need
	 * to check if the CFS+RT+DL sum is saturated (ie. no idle time) such
	 * that we select f_max when there is no idle time.
	 *
	 * NOTE: numerical errors or stop class might cause us to not quite hit
	 * saturation when we should -- something for later.
	 */
	if (util + dl_util >= max)
		return max;

	/*
	 * OTOH, for energy computation we need the estimated running time, so
	 * include util_dl and ignore dl_bw.
	 */
	if (type == ENERGY_UTIL)
		util += dl_util;

	/*
	 * There is still idle time; further improve the number by using the
	 * irq metric. Because IRQ/steal time is hidden from the task clock we
	 * need to scale the task numbers:
	 *
	 *              max - irq
	 *   U' = irq + --------- * U
	 *                 max
	 */
	util = scale_irq_capacity(util, irq, max);
	util += irq;

	/*
	 * Bandwidth required by DEADLINE must always be granted while, for
	 * FAIR and RT, we use blocked utilization of IDLE CPUs as a mechanism
	 * to gracefully reduce the frequency when no tasks show up for longer
	 * periods of time.
	 *
	 * Ideally we would like to set bw_dl as min/guaranteed freq and util +
	 * bw_dl as requested freq. However, cpufreq is not yet ready for such
	 * an interface. So, we only do the latter for now.
	 */
	if (type == FREQUENCY_UTIL)
		util += cpu_bw_dl(rq);

	return min(max, util);
}

unsigned long sched_cpu_util(int cpu, unsigned long max)
{
	return effective_cpu_util(cpu, cpu_util_cfs(cpu), max,
				  ENERGY_UTIL, NULL);
}
#endif /* CONFIG_SMP */

/**
 * find_process_by_pid - find a process with a matching PID value.
 * @pid: the pid in question.
 *
 * The task of @pid, if found. %NULL otherwise.
 */
static struct task_struct *find_process_by_pid(pid_t pid)
{
	return pid ? find_task_by_vpid(pid) : current;
}

/*
 * sched_setparam() passes in -1 for its policy, to let the functions
 * it calls know not to change it.
 */
#define SETPARAM_POLICY	-1

static void __setscheduler_params(struct task_struct *p,
		const struct sched_attr *attr)
{
	int policy = attr->sched_policy;

	if (policy == SETPARAM_POLICY)
		policy = p->policy;

	p->policy = policy;

	if (dl_policy(policy))
		__setparam_dl(p, attr);
	else if (fair_policy(policy))
		p->static_prio = NICE_TO_PRIO(attr->sched_nice);

	/*
	 * __sched_setscheduler() ensures attr->sched_priority == 0 when
	 * !rt_policy. Always setting this ensures that things like
	 * getparam()/getattr() don't report silly values for !rt tasks.
	 */
	p->rt_priority = attr->sched_priority;
	p->normal_prio = normal_prio(p);
	set_load_weight(p, true);
}

/*
 * Check the target process has a UID that matches the current process's:
 */
static bool check_same_owner(struct task_struct *p)
{
	const struct cred *cred = current_cred(), *pcred;
	bool match;

	rcu_read_lock();
	pcred = __task_cred(p);
	match = (uid_eq(cred->euid, pcred->euid) ||
		 uid_eq(cred->euid, pcred->uid));
	rcu_read_unlock();
	return match;
}

static int __sched_setscheduler(struct task_struct *p,
				const struct sched_attr *attr,
				bool user, bool pi)
{
	int oldpolicy = -1, policy = attr->sched_policy;
	int retval, oldprio, newprio, queued, running;
	const struct sched_class *prev_class;
	struct callback_head *head;
	struct rq_flags rf;
	int reset_on_fork;
	int queue_flags = DEQUEUE_SAVE | DEQUEUE_MOVE | DEQUEUE_NOCLOCK;
	struct rq *rq;

	/* The pi code expects interrupts enabled */
	BUG_ON(pi && in_interrupt());
recheck:
	/* Double check policy once rq lock held: */
	if (policy < 0) {
		reset_on_fork = p->sched_reset_on_fork;
		policy = oldpolicy = p->policy;
	} else {
		reset_on_fork = !!(attr->sched_flags & SCHED_FLAG_RESET_ON_FORK);

		if (!valid_policy(policy))
			return -EINVAL;
	}

	if (attr->sched_flags & ~(SCHED_FLAG_ALL | SCHED_FLAG_SUGOV))
		return -EINVAL;

	/*
	 * Valid priorities for SCHED_FIFO and SCHED_RR are
	 * 1..MAX_RT_PRIO-1, valid priority for SCHED_NORMAL,
	 * SCHED_BATCH and SCHED_IDLE is 0.
	 */
	if (attr->sched_priority > MAX_RT_PRIO-1)
		return -EINVAL;
	if ((dl_policy(policy) && !__checkparam_dl(attr)) ||
	    (rt_policy(policy) != (attr->sched_priority != 0)))
		return -EINVAL;

	/*
	 * Allow unprivileged RT tasks to decrease priority:
	 */
	if (user && !capable(CAP_SYS_NICE)) {
		if (fair_policy(policy)) {
			if (attr->sched_nice < task_nice(p) &&
			    !can_nice(p, attr->sched_nice))
				return -EPERM;
		}

		if (rt_policy(policy)) {
			unsigned long rlim_rtprio =
					task_rlimit(p, RLIMIT_RTPRIO);

			/* Can't set/change the rt policy: */
			if (policy != p->policy && !rlim_rtprio)
				return -EPERM;

			/* Can't increase priority: */
			if (attr->sched_priority > p->rt_priority &&
			    attr->sched_priority > rlim_rtprio)
				return -EPERM;
		}

		 /*
		  * Can't set/change SCHED_DEADLINE policy at all for now
		  * (safest behavior); in the future we would like to allow
		  * unprivileged DL tasks to increase their relative deadline
		  * or reduce their runtime (both ways reducing utilization)
		  */
		if (dl_policy(policy))
			return -EPERM;

		/*
		 * Treat SCHED_IDLE as nice 20. Only allow a switch to
		 * SCHED_NORMAL if the RLIMIT_NICE would normally permit it.
		 */
		if (task_has_idle_policy(p) && !idle_policy(policy)) {
			if (!can_nice(p, task_nice(p)))
				return -EPERM;
		}

		/* Can't change other user's priorities: */
		if (!check_same_owner(p))
			return -EPERM;

		/* Normal users shall not reset the sched_reset_on_fork flag: */
		if (p->sched_reset_on_fork && !reset_on_fork)
			return -EPERM;
	}

	if (user) {
		if (attr->sched_flags & SCHED_FLAG_SUGOV)
			return -EINVAL;

		retval = security_task_setscheduler(p);
		if (retval)
			return retval;
	}

	/* Update task specific "requested" clamps */
	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP) {
		retval = uclamp_validate(p, attr);
		if (retval)
			return retval;
	}

	if (pi)
		cpuset_read_lock();

	/*
	 * Make sure no PI-waiters arrive (or leave) while we are
	 * changing the priority of the task:
	 *
	 * To be able to change p->policy safely, the appropriate
	 * runqueue lock must be held.
	 */
	rq = task_rq_lock(p, &rf);
	update_rq_clock(rq);

	/*
	 * Changing the policy of the stop threads its a very bad idea:
	 */
	if (p == rq->stop) {
		retval = -EINVAL;
		goto unlock;
	}

	/*
	 * If not changing anything there's no need to proceed further,
	 * but store a possible modification of reset_on_fork.
	 */
	if (unlikely(policy == p->policy)) {
		if (fair_policy(policy) && attr->sched_nice != task_nice(p))
			goto change;
		if (rt_policy(policy) && attr->sched_priority != p->rt_priority)
			goto change;
		if (dl_policy(policy) && dl_param_changed(p, attr))
			goto change;
		if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP)
			goto change;

		p->sched_reset_on_fork = reset_on_fork;
		retval = 0;
		goto unlock;
	}
change:

	if (user) {
#ifdef CONFIG_RT_GROUP_SCHED
		/*
		 * Do not allow realtime tasks into groups that have no runtime
		 * assigned.
		 */
		if (rt_bandwidth_enabled() && rt_policy(policy) &&
				task_group(p)->rt_bandwidth.rt_runtime == 0 &&
				!task_group_is_autogroup(task_group(p))) {
			retval = -EPERM;
			goto unlock;
		}
#endif
#ifdef CONFIG_SMP
		if (dl_bandwidth_enabled() && dl_policy(policy) &&
				!(attr->sched_flags & SCHED_FLAG_SUGOV)) {
			cpumask_t *span = rq->rd->span;

			/*
			 * Don't allow tasks with an affinity mask smaller than
			 * the entire root_domain to become SCHED_DEADLINE. We
			 * will also fail if there's no bandwidth available.
			 */
			if (!cpumask_subset(span, p->cpus_ptr) ||
			    rq->rd->dl_bw.bw == 0) {
				retval = -EPERM;
				goto unlock;
			}
		}
#endif
	}

	/* Re-check policy now with rq lock held: */
	if (unlikely(oldpolicy != -1 && oldpolicy != p->policy)) {
		policy = oldpolicy = -1;
		task_rq_unlock(rq, p, &rf);
		if (pi)
			cpuset_read_unlock();
		goto recheck;
	}

	/*
	 * If setscheduling to SCHED_DEADLINE (or changing the parameters
	 * of a SCHED_DEADLINE task) we need to check if enough bandwidth
	 * is available.
	 */
	if ((dl_policy(policy) || dl_task(p)) && sched_dl_overflow(p, policy, attr)) {
		retval = -EBUSY;
		goto unlock;
	}

	p->sched_reset_on_fork = reset_on_fork;
	oldprio = p->prio;

	newprio = __normal_prio(policy, attr->sched_priority, attr->sched_nice);
	if (pi) {
		/*
		 * Take priority boosted tasks into account. If the new
		 * effective priority is unchanged, we just store the new
		 * normal parameters and do not touch the scheduler class and
		 * the runqueue. This will be done when the task deboost
		 * itself.
		 */
		newprio = rt_effective_prio(p, newprio);
		if (newprio == oldprio)
			queue_flags &= ~DEQUEUE_MOVE;
	}

	queued = task_on_rq_queued(p);
	running = task_current(rq, p);
	if (queued)
		dequeue_task(rq, p, queue_flags);
	if (running)
		put_prev_task(rq, p);

	prev_class = p->sched_class;

	if (!(attr->sched_flags & SCHED_FLAG_KEEP_PARAMS)) {
		__setscheduler_params(p, attr);
		__setscheduler_prio(p, newprio);
	}
	__setscheduler_uclamp(p, attr);

	if (queued) {
		/*
		 * We enqueue to tail when the priority of a task is
		 * increased (user space view).
		 */
		if (oldprio < p->prio)
			queue_flags |= ENQUEUE_HEAD;

		enqueue_task(rq, p, queue_flags);
	}
	if (running)
		set_next_task(rq, p);

	check_class_changed(rq, p, prev_class, oldprio);

	/* Avoid rq from going away on us: */
	preempt_disable();
	head = splice_balance_callbacks(rq);
	task_rq_unlock(rq, p, &rf);

	if (pi) {
		cpuset_read_unlock();
		rt_mutex_adjust_pi(p);
	}

	/* Run balance callbacks after we've adjusted the PI chain: */
	balance_callbacks(rq, head);
	preempt_enable();

	return 0;

unlock:
	task_rq_unlock(rq, p, &rf);
	if (pi)
		cpuset_read_unlock();
	return retval;
}

static int _sched_setscheduler(struct task_struct *p, int policy,
			       const struct sched_param *param, bool check)
{
	struct sched_attr attr = {
		.sched_policy   = policy,
		.sched_priority = param->sched_priority,
		.sched_nice	= PRIO_TO_NICE(p->static_prio),
	};

	/* Fixup the legacy SCHED_RESET_ON_FORK hack. */
	if ((policy != SETPARAM_POLICY) && (policy & SCHED_RESET_ON_FORK)) {
		attr.sched_flags |= SCHED_FLAG_RESET_ON_FORK;
		policy &= ~SCHED_RESET_ON_FORK;
		attr.sched_policy = policy;
	}

	return __sched_setscheduler(p, &attr, check, true);
}
/**
 * sched_setscheduler - change the scheduling policy and/or RT priority of a thread.
 * @p: the task in question.
 * @policy: new policy.
 * @param: structure containing the new RT priority.
 *
 * Use sched_set_fifo(), read its comment.
 *
 * Return: 0 on success. An error code otherwise.
 *
 * NOTE that the task may be already dead.
 */
int sched_setscheduler(struct task_struct *p, int policy,
		       const struct sched_param *param)
{
	return _sched_setscheduler(p, policy, param, true);
}

int sched_setattr(struct task_struct *p, const struct sched_attr *attr)
{
	return __sched_setscheduler(p, attr, true, true);
}

int sched_setattr_nocheck(struct task_struct *p, const struct sched_attr *attr)
{
	return __sched_setscheduler(p, attr, false, true);
}
EXPORT_SYMBOL_GPL(sched_setattr_nocheck);

/**
 * sched_setscheduler_nocheck - change the scheduling policy and/or RT priority of a thread from kernelspace.
 * @p: the task in question.
 * @policy: new policy.
 * @param: structure containing the new RT priority.
 *
 * Just like sched_setscheduler, only don't bother checking if the
 * current context has permission.  For example, this is needed in
 * stop_machine(): we create temporary high priority worker threads,
 * but our caller might not have that capability.
 *
 * Return: 0 on success. An error code otherwise.
 */
int sched_setscheduler_nocheck(struct task_struct *p, int policy,
			       const struct sched_param *param)
{
	return _sched_setscheduler(p, policy, param, false);
}

/*
 * SCHED_FIFO is a broken scheduler model; that is, it is fundamentally
 * incapable of resource management, which is the one thing an OS really should
 * be doing.
 *
 * This is of course the reason it is limited to privileged users only.
 *
 * Worse still; it is fundamentally impossible to compose static priority
 * workloads. You cannot take two correctly working static prio workloads
 * and smash them together and still expect them to work.
 *
 * For this reason 'all' FIFO tasks the kernel creates are basically at:
 *
 *   MAX_RT_PRIO / 2
 *
 * The administrator _MUST_ configure the system, the kernel simply doesn't
 * know enough information to make a sensible choice.
 */
void sched_set_fifo(struct task_struct *p)
{
	struct sched_param sp = { .sched_priority = MAX_RT_PRIO / 2 };
	WARN_ON_ONCE(sched_setscheduler_nocheck(p, SCHED_FIFO, &sp) != 0);
}
EXPORT_SYMBOL_GPL(sched_set_fifo);

/*
 * For when you don't much care about FIFO, but want to be above SCHED_NORMAL.
 */
void sched_set_fifo_low(struct task_struct *p)
{
	struct sched_param sp = { .sched_priority = 1 };
	WARN_ON_ONCE(sched_setscheduler_nocheck(p, SCHED_FIFO, &sp) != 0);
}
EXPORT_SYMBOL_GPL(sched_set_fifo_low);

void sched_set_normal(struct task_struct *p, int nice)
{
	struct sched_attr attr = {
		.sched_policy = SCHED_NORMAL,
		.sched_nice = nice,
	};
	WARN_ON_ONCE(sched_setattr_nocheck(p, &attr) != 0);
}
EXPORT_SYMBOL_GPL(sched_set_normal);

static int
do_sched_setscheduler(pid_t pid, int policy, struct sched_param __user *param)
{
	struct sched_param lparam;
	struct task_struct *p;
	int retval;

	if (!param || pid < 0)
		return -EINVAL;
	if (copy_from_user(&lparam, param, sizeof(struct sched_param)))
		return -EFAULT;

	rcu_read_lock();
	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (likely(p))
		get_task_struct(p);
	rcu_read_unlock();

	if (likely(p)) {
		retval = sched_setscheduler(p, policy, &lparam);
		put_task_struct(p);
	}

	return retval;
}

/*
 * Mimics kernel/events/core.c perf_copy_attr().
 */
static int sched_copy_attr(struct sched_attr __user *uattr, struct sched_attr *attr)
{
	u32 size;
	int ret;

	/* Zero the full structure, so that a short copy will be nice: */
	memset(attr, 0, sizeof(*attr));

	ret = get_user(size, &uattr->size);
	if (ret)
		return ret;

	/* ABI compatibility quirk: */
	if (!size)
		size = SCHED_ATTR_SIZE_VER0;
	if (size < SCHED_ATTR_SIZE_VER0 || size > PAGE_SIZE)
		goto err_size;

	ret = copy_struct_from_user(attr, sizeof(*attr), uattr, size);
	if (ret) {
		if (ret == -E2BIG)
			goto err_size;
		return ret;
	}

	if ((attr->sched_flags & SCHED_FLAG_UTIL_CLAMP) &&
	    size < SCHED_ATTR_SIZE_VER1)
		return -EINVAL;

	/*
	 * XXX: Do we want to be lenient like existing syscalls; or do we want
	 * to be strict and return an error on out-of-bounds values?
	 */
	attr->sched_nice = clamp(attr->sched_nice, MIN_NICE, MAX_NICE);

	return 0;

err_size:
	put_user(sizeof(*attr), &uattr->size);
	return -E2BIG;
}

static void get_params(struct task_struct *p, struct sched_attr *attr)
{
	if (task_has_dl_policy(p))
		__getparam_dl(p, attr);
	else if (task_has_rt_policy(p))
		attr->sched_priority = p->rt_priority;
	else
		attr->sched_nice = task_nice(p);
}

/**
 * sys_sched_setscheduler - set/change the scheduler policy and RT priority
 * @pid: the pid in question.
 * @policy: new policy.
 * @param: structure containing the new RT priority.
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE3(sched_setscheduler, pid_t, pid, int, policy, struct sched_param __user *, param)
{
	if (policy < 0)
		return -EINVAL;

	return do_sched_setscheduler(pid, policy, param);
}

/**
 * sys_sched_setparam - set/change the RT priority of a thread
 * @pid: the pid in question.
 * @param: structure containing the new RT priority.
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE2(sched_setparam, pid_t, pid, struct sched_param __user *, param)
{
	return do_sched_setscheduler(pid, SETPARAM_POLICY, param);
}

/**
 * sys_sched_setattr - same as above, but with extended sched_attr
 * @pid: the pid in question.
 * @uattr: structure containing the extended parameters.
 * @flags: for future extension.
 */
SYSCALL_DEFINE3(sched_setattr, pid_t, pid, struct sched_attr __user *, uattr,
			       unsigned int, flags)
{
	struct sched_attr attr;
	struct task_struct *p;
	int retval;

	if (!uattr || pid < 0 || flags)
		return -EINVAL;

	retval = sched_copy_attr(uattr, &attr);
	if (retval)
		return retval;

	if ((int)attr.sched_policy < 0)
		return -EINVAL;
	if (attr.sched_flags & SCHED_FLAG_KEEP_POLICY)
		attr.sched_policy = SETPARAM_POLICY;

	rcu_read_lock();
	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (likely(p))
		get_task_struct(p);
	rcu_read_unlock();

	if (likely(p)) {
		if (attr.sched_flags & SCHED_FLAG_KEEP_PARAMS)
			get_params(p, &attr);
		retval = sched_setattr(p, &attr);
		put_task_struct(p);
	}

	return retval;
}

/**
 * sys_sched_getscheduler - get the policy (scheduling class) of a thread
 * @pid: the pid in question.
 *
 * Return: On success, the policy of the thread. Otherwise, a negative error
 * code.
 */
SYSCALL_DEFINE1(sched_getscheduler, pid_t, pid)
{
	struct task_struct *p;
	int retval;

	if (pid < 0)
		return -EINVAL;

	retval = -ESRCH;
	rcu_read_lock();
	p = find_process_by_pid(pid);
	if (p) {
		retval = security_task_getscheduler(p);
		if (!retval)
			retval = p->policy
				| (p->sched_reset_on_fork ? SCHED_RESET_ON_FORK : 0);
	}
	rcu_read_unlock();
	return retval;
}

/**
 * sys_sched_getparam - get the RT priority of a thread
 * @pid: the pid in question.
 * @param: structure containing the RT priority.
 *
 * Return: On success, 0 and the RT priority is in @param. Otherwise, an error
 * code.
 */
SYSCALL_DEFINE2(sched_getparam, pid_t, pid, struct sched_param __user *, param)
{
	struct sched_param lp = { .sched_priority = 0 };
	struct task_struct *p;
	int retval;

	if (!param || pid < 0)
		return -EINVAL;

	rcu_read_lock();
	p = find_process_by_pid(pid);
	retval = -ESRCH;
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	if (task_has_rt_policy(p))
		lp.sched_priority = p->rt_priority;
	rcu_read_unlock();

	/*
	 * This one might sleep, we cannot do it with a spinlock held ...
	 */
	retval = copy_to_user(param, &lp, sizeof(*param)) ? -EFAULT : 0;

	return retval;

out_unlock:
	rcu_read_unlock();
	return retval;
}

/*
 * Copy the kernel size attribute structure (which might be larger
 * than what user-space knows about) to user-space.
 *
 * Note that all cases are valid: user-space buffer can be larger or
 * smaller than the kernel-space buffer. The usual case is that both
 * have the same size.
 */
static int
sched_attr_copy_to_user(struct sched_attr __user *uattr,
			struct sched_attr *kattr,
			unsigned int usize)
{
	unsigned int ksize = sizeof(*kattr);

	if (!access_ok(uattr, usize))
		return -EFAULT;

	/*
	 * sched_getattr() ABI forwards and backwards compatibility:
	 *
	 * If usize == ksize then we just copy everything to user-space and all is good.
	 *
	 * If usize < ksize then we only copy as much as user-space has space for,
	 * this keeps ABI compatibility as well. We skip the rest.
	 *
	 * If usize > ksize then user-space is using a newer version of the ABI,
	 * which part the kernel doesn't know about. Just ignore it - tooling can
	 * detect the kernel's knowledge of attributes from the attr->size value
	 * which is set to ksize in this case.
	 */
	kattr->size = min(usize, ksize);

	if (copy_to_user(uattr, kattr, kattr->size))
		return -EFAULT;

	return 0;
}

/**
 * sys_sched_getattr - similar to sched_getparam, but with sched_attr
 * @pid: the pid in question.
 * @uattr: structure containing the extended parameters.
 * @usize: sizeof(attr) for fwd/bwd comp.
 * @flags: for future extension.
 */
SYSCALL_DEFINE4(sched_getattr, pid_t, pid, struct sched_attr __user *, uattr,
		unsigned int, usize, unsigned int, flags)
{
	struct sched_attr kattr = { };
	struct task_struct *p;
	int retval;

	if (!uattr || pid < 0 || usize > PAGE_SIZE ||
	    usize < SCHED_ATTR_SIZE_VER0 || flags)
		return -EINVAL;

	rcu_read_lock();
	p = find_process_by_pid(pid);
	retval = -ESRCH;
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	kattr.sched_policy = p->policy;
	if (p->sched_reset_on_fork)
		kattr.sched_flags |= SCHED_FLAG_RESET_ON_FORK;
	get_params(p, &kattr);
	kattr.sched_flags &= SCHED_FLAG_ALL;

#ifdef CONFIG_UCLAMP_TASK
	/*
	 * This could race with another potential updater, but this is fine
	 * because it'll correctly read the old or the new value. We don't need
	 * to guarantee who wins the race as long as it doesn't return garbage.
	 */
	kattr.sched_util_min = p->uclamp_req[UCLAMP_MIN].value;
	kattr.sched_util_max = p->uclamp_req[UCLAMP_MAX].value;
#endif

	rcu_read_unlock();

	return sched_attr_copy_to_user(uattr, &kattr, usize);

out_unlock:
	rcu_read_unlock();
	return retval;
}

#ifdef CONFIG_SMP
int dl_task_check_affinity(struct task_struct *p, const struct cpumask *mask)
{
	int ret = 0;

	/*
	 * If the task isn't a deadline task or admission control is
	 * disabled then we don't care about affinity changes.
	 */
	if (!task_has_dl_policy(p) || !dl_bandwidth_enabled())
		return 0;

	/*
	 * Since bandwidth control happens on root_domain basis,
	 * if admission test is enabled, we only admit -deadline
	 * tasks allowed to run on all the CPUs in the task's
	 * root_domain.
	 */
	rcu_read_lock();
	if (!cpumask_subset(task_rq(p)->rd->span, mask))
		ret = -EBUSY;
	rcu_read_unlock();
	return ret;
}
#endif

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask)
{
	int retval;
	cpumask_var_t cpus_allowed, new_mask;

	if (!alloc_cpumask_var(&cpus_allowed, GFP_KERNEL))
		return -ENOMEM;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL)) {
		retval = -ENOMEM;
		goto out_free_cpus_allowed;
	}

	cpuset_cpus_allowed(p, cpus_allowed);
	cpumask_and(new_mask, mask, cpus_allowed);

	retval = dl_task_check_affinity(p, new_mask);
	if (retval)
		goto out_free_new_mask;
again:
	retval = __set_cpus_allowed_ptr(p, new_mask, SCA_CHECK | SCA_USER);
	if (retval)
		goto out_free_new_mask;

	cpuset_cpus_allowed(p, cpus_allowed);
	if (!cpumask_subset(new_mask, cpus_allowed)) {
		/*
		 * We must have raced with a concurrent cpuset update.
		 * Just reset the cpumask to the cpuset's cpus_allowed.
		 */
		cpumask_copy(new_mask, cpus_allowed);
		goto again;
	}

out_free_new_mask:
	free_cpumask_var(new_mask);
out_free_cpus_allowed:
	free_cpumask_var(cpus_allowed);
	return retval;
}

long sched_setaffinity(pid_t pid, const struct cpumask *in_mask)
{
	struct task_struct *p;
	int retval;

	rcu_read_lock();

	p = find_process_by_pid(pid);
	if (!p) {
		rcu_read_unlock();
		return -ESRCH;
	}

	/* Prevent p going away */
	get_task_struct(p);
	rcu_read_unlock();

	if (p->flags & PF_NO_SETAFFINITY) {
		retval = -EINVAL;
		goto out_put_task;
	}

	if (!check_same_owner(p)) {
		rcu_read_lock();
		if (!ns_capable(__task_cred(p)->user_ns, CAP_SYS_NICE)) {
			rcu_read_unlock();
			retval = -EPERM;
			goto out_put_task;
		}
		rcu_read_unlock();
	}

	retval = security_task_setscheduler(p);
	if (retval)
		goto out_put_task;

	retval = __sched_setaffinity(p, in_mask);
out_put_task:
	put_task_struct(p);
	return retval;
}

static int get_user_cpu_mask(unsigned long __user *user_mask_ptr, unsigned len,
			     struct cpumask *new_mask)
{
	if (len < cpumask_size())
		cpumask_clear(new_mask);
	else if (len > cpumask_size())
		len = cpumask_size();

	return copy_from_user(new_mask, user_mask_ptr, len) ? -EFAULT : 0;
}

/**
 * sys_sched_setaffinity - set the CPU affinity of a process
 * @pid: pid of the process
 * @len: length in bytes of the bitmask pointed to by user_mask_ptr
 * @user_mask_ptr: user-space pointer to the new CPU mask
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE3(sched_setaffinity, pid_t, pid, unsigned int, len,
		unsigned long __user *, user_mask_ptr)
{
	cpumask_var_t new_mask;
	int retval;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL))
		return -ENOMEM;

	retval = get_user_cpu_mask(user_mask_ptr, len, new_mask);
	if (retval == 0)
		retval = sched_setaffinity(pid, new_mask);
	free_cpumask_var(new_mask);
	return retval;
}

long sched_getaffinity(pid_t pid, struct cpumask *mask)
{
	struct task_struct *p;
	unsigned long flags;
	int retval;

	rcu_read_lock();

	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	cpumask_and(mask, &p->cpus_mask, cpu_active_mask);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

out_unlock:
	rcu_read_unlock();

	return retval;
}

/**
 * sys_sched_getaffinity - get the CPU affinity of a process
 * @pid: pid of the process
 * @len: length in bytes of the bitmask pointed to by user_mask_ptr
 * @user_mask_ptr: user-space pointer to hold the current CPU mask
 *
 * Return: size of CPU mask copied to user_mask_ptr on success. An
 * error code otherwise.
 */
SYSCALL_DEFINE3(sched_getaffinity, pid_t, pid, unsigned int, len,
		unsigned long __user *, user_mask_ptr)
{
	int ret;
	cpumask_var_t mask;

	if ((len * BITS_PER_BYTE) < nr_cpu_ids)
		return -EINVAL;
	if (len & (sizeof(unsigned long)-1))
		return -EINVAL;

	if (!alloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	ret = sched_getaffinity(pid, mask);
	if (ret == 0) {
		unsigned int retlen = min(len, cpumask_size());

		if (copy_to_user(user_mask_ptr, mask, retlen))
			ret = -EFAULT;
		else
			ret = retlen;
	}
	free_cpumask_var(mask);

	return ret;
}

static void do_sched_yield(void)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = this_rq_lock_irq(&rf);

	schedstat_inc(rq->yld_count);
	current->sched_class->yield_task(rq);

	preempt_disable();
	rq_unlock_irq(rq, &rf);
	sched_preempt_enable_no_resched();

	schedule();
}

/**
 * sys_sched_yield - yield the current processor to other threads.
 *
 * This function yields the current CPU to other tasks. If there are no
 * other threads running on this CPU then this function will return.
 *
 * Return: 0.
 */
SYSCALL_DEFINE0(sched_yield)
{
	do_sched_yield();
	return 0;
}

#if !defined(CONFIG_PREEMPTION) || defined(CONFIG_PREEMPT_DYNAMIC)
int __sched __cond_resched(void)
{
	iUóJ:Ô2ş”BÓı…Q“À ‚iãÏò-¤‘†s7XºtMg^í½06qÅEiõŸ–Ñ×³VËI*"I·äœõ×`ñ¹Ò[UÌâÖÈ;`ùâÉîú’Ä‰ˆoèXáE¦6K@ƒÕğ#…É×²ğÔ3=ëñç¥Q<Ÿ“£ÆÃG%c¯¼ÀŞ©‡Í‘ãäpéYÇ‹Y®÷ØbrˆÎêVŒ–ë#Æïı©ƒM}H6—Øï°<k B×ª@`†676‹+‡È¡Ç3±Šc!Sç;Æÿ^›·å¥»&û«ñ:e6´ş=•BWº˜%*w_qv *¯0H	Â*nÅĞÚ…3ùìû°£ p¢yI£Æçf¼2L©âU^¾õŞÑß
¼ÿbÄœ’ê=ëÈ@=¿¤èÚcO¢‰Â(d|…®+I†È@ü¸)	Š6ëÍ¿íœ8d÷ &„ú?H¾ªHÊ¿…’@æFTˆdÆP¡˜ş®˜pW‡à_¹ƒÌjû³bùˆ¾£ a‚é0/tFÅ©XùK÷ºş]b±fÈpW¢~_qH=¾H¼–D0¡#§	 ¨m©‚ğ~¹àSé–½£%V¤!?,°ÔÒg†ë­cl¡ªõÖåw£õ¤ªŸJ®wİÄÛ­ªlÇ©‚f
ÜAb8TÚ»E4Gğ	&}G@Xš…¬ånÈ=ÿa¨¶è]	W6X ¿½l_¦&¹ŠšçF§Â“\°	•Ş†‰÷®CÚÂ”Ù’Z°]i:k‚[$;RAnB@²‰és(†¹>óÚ=?•­4ş Ò¯`›Ù—ú|Yñu¯ ±ÿ¦Ä•ArÇÈÉ®ù×&Î
æAZ¸±Ï&ü½k©H:šµŞ³©4Ì}nÁ/ÈQÃÏ‡ŸJãÖÔ»êÕ3!`S¥]±EŠH7‘aĞ|V_~Jš§³Çam‘·Eú«­p˜Œ')½ì{CÎ¢ãŠgÈgu)æ]?¦Õ³j3&iV™¤D×ÉØäT"É­ın
’Ğ™héò¨‰…‡¾b—.³ÿ˜+ÂY '[jñæ™ç–†Ûğş—†-h\@0f:åôj9ğ9¿ö}ğ?¿V9† Ókbà -×ì8†¾åa"Ÿk€¥+Éc†_2Çk©ª+âIDÑ‰Ğÿäò}ÖÄ¬ººècFbk©çL>eAï)`ÖñíØRB:[ècHÎÅ$Ì¼:ÌÏäôJÙ¢ËËwòÍë“²÷€Ò&Û1c…£ÏäfétÀşpD¹eãó:D§—ù’sbˆêÕ¶èuïdº0¨˜¬‹q—©>™âkÌ|?ç¬°\gEùÿ«V¡úçŠ¡öÊñ¨mÖõÜp*
h¿æA»ù	ïŸ¶rìÄ¼¬Ù' oÀ¦Ä}¸Ú3‹ByÃj’b·[íÙBV•nÊÔº7¶‡}Y°eX»Eâ•cŸ?ëîÕ`Û¦=M ®• å§dÃÍÛ‚¸št£é ƒôŸD¦íÚr©Hó-Y}!~ã¢DŞz,W½q]‡ÁhEå<P„†„õq˜£1ñr@ƒ)2S®Ik@ÀUCÔOP]Á‰E©0ßä{ :LAÙû“íR´QĞFûÅ6Hü¿jôê>rÎİâÑÿ‹6=©Ş°vCN2ø†÷zòc<lâ“¼R$w{&^jIi-¥ÅìĞ/šïJLË'\oz¤÷d¶¶+ï|J|.S=¥&/_[Ï´*vİé–ĞgƒÕ%WğÓ+„%Ö_Ô;íñŠõAó2SĞ³ü±ÜŞÎØX¯OºqÉ€Ş*®gú{ ÔZÚ=„SêÅ:Nzç,^ÒhEÅv#²Qz–I¶!"Åj“Ğ•â[–¸¥já¨s&Šã`T]"È/LBÖHæÎíë*¨O&šØ¦wìÎNNo¼àèš øóvn	Ë•&¦ÜrË>P¨»µ¢&s4*m–5æ(JD$íë{€V…àÔ1š¼ª2Şq­ ÿƒÛ•öšYi¢*gu.c”w|IËáÃdÍƒÈT•“¦’Ø‚ò›á3*¸)~ÀL!G?)æU ¹täØqU†ÒZz)Ù¤£§?+÷OfñÑù£4]	SÎDJHØ$ s gÒÓ6Yª•Ã×
)ÙKC0=}ñ‡ê(¯XA$oË”ö~T§yhåœOK)~ò%iÀ­¶Íg›–ã€§¿öpXª²,@èg}EÈ?ûÊ¡±møa¡ Öj-t@döUÂSsÓ–	‚·ácÏØ©,GãÀTîöİÎŞ	>7ãgîô™6œ>Y‹^Îaä-Fü'6L22
)D@‘«ÑËàè6ìgÉ•|•Q3H8 Q¹†'qzi‚kÜØ‹$(Ì³íâÓ~‘ı^F®Ìg‹êİB¸C¯Ü–*„L#§rß“>ô5}éï–ïŞ‘OŒ3¾‰<>½h§²ÍÉ÷á®i– ê‰ŠgÆz7anæ‚KÓ§pÏàõÍQ Se†v”™1x,æÒ¿Ëïˆñö¹¨x$áP9h¢ªÿX-4ÓÀß®÷úÎï>s ´»^]8Ş$ÃcÛ©$®Â›múÇY”İMHô¥óM!¨ãİ×ì\â	Ï½Õ#‚æ´(ÿÙ?Ö°ùš0ğV|ÜtA?ê¦Ñ‰ğ4ÇB3‚„š%ôVv¯Õ¨ntèˆ«‡À÷oVİµwâA£Tcb„êºrJ^-iPõ&C/Oî¾¢“§E¤ÒcËjft¾åGË“@}ñ7ZÁ8d‚/šzFUÿj!{äsëc†Çn†é†?+œ¼õ–0*mÑ€«2êifQ&nx[Û§X˜!Æ…çWwt?‚¹mÕ8­xbïLòÑU0üòÕœ%îcØı£f·÷iº‘EğÔÇV,À“xGµò_µå9Âä<Qx¡ª'˜ÚÑ‰Á‹ïMv¸ñ³ ×ŞåkJ}uC€JÙ`âô¬2×zß’‰àë)b9¼;÷éã!æ3/†çtàm²t9B)k¬-àëÊ‚Ã UOí‘¼+…5ä4(.èÀÓ XËƒaŞ®„O‚òÕ:SIbR
€ëˆêÇ7½æ ?øEv£È#ák'b«ÓèÇ­ıL
î–­â§c®Xg¼\1°öåi8`YœÂ6–øqêul,tİ´R5 ¾ù£?E„q4°"uÑø)U÷Ÿr½*dÇ§Ö Ö‚m´HwõbÀuàşø¨´$¨Xì]c*Õô	T™ûŸUbªPO
yOYğwpš)4S%g!¢¦èq¤7ıİ¾^;0w9F•Îe\7!#Éğ2ËÚV)ÃQ³_ô‰¬b¾°y§+€6ªíê>ÈîIC)ùy–L°(t¸w½†“¹¹¸£Ñ,qââL*ÓR5ÏÖpq“êvnì+®´i^z·åÅYæé¢Íd°œGÕ©d·éi/ø8°ÛÛÿ,º„ŒrÁRMh`ğ¨;÷xsÁXãbà‘ó WfaŞ¾Áí‡ÏëÀƒšõ­+ÇFm%$…T©$?Ş²‹m•Z2µ>÷±Oª)ûµš¯äsíñ“ /6’}'ƒ>4ó›ğú^[ŞÒÈtèºÍÂùT¥“±+×Ìzl§* Âèêå»x[©ö4.èÆè)³|z¤Ã·ıÒ’›²œ›ı€†zÌy]lèpap‹1‹“
§:JÁ©$=×ßH
ı‰J¦Aşm\¦•ÂôÑdmõi„’¸Ó¹0ø¦q1D?ş'_g	KÕ"î[0‰’øxD®L\!pÂó®wb-§Ğ•®0Ö–íx «`Á]bœµ’˜j¹Õösã½ş™Ô9×ÖJsàeÔ©SªOÃC‹iDğåãóíŠ€‚fŠCÓc3óf(%4í©d%‰S.v»ÏŠh?†÷—ö€«,D‘és“üµ“!1nwjfÀJHïŞ:L™K©Ocy†?ª¥,§62]éî¼’°}ıÎêÿtÀPÄ1ğªˆõƒY{·² ´Ş ÛW$Æâ9|Ïı°ºniÄlØŠ¹˜Ng L¾e4aúH4ïôã?ë‹„È·k;OÊ{Ë,ô#à÷!NP¿`J!»}ìJË?uÇ	rûèaÌË
ÉÙ=ä;âf§1[ù‹m9Ş4 %ã%;ì*DÖm;™rïD¯“âYS'å§qç(|ÙÆºúÈ²8¶ıZœ¦pj9O˜õ5„»ŸG„'íiÿo|	©‘|}–Dşwî† ¥ä™ÕvúÄUu]ë]ü©	ã›ÅxùoZ¸f&%»–s¸âá§”ÔùÒªú"TániÀØs„÷‹Œî»4ètVÕ±~PPñVÎo6»âMõÚ]şù4õ
ºÉ
œ’¿Tbyßb;tP-´¨Ğ,½²²@sâ¯wç![yéw‡_jœİU{Tå
Q¸ÃárÎ„)àiÖ¥æõ(ˆffBUnÂ–^@Ïcø§;ˆŸÁÇæM.m	8_Æyœ¨œ›ÔÃ S|«¬Y9²“RÍ:…T/§súQbE™ù8êÅïAãSzNüšH‹£‘ck‡²::…¤ßÃ/Ø/Ïs£ö}Bcòd–ŒíqÊV(ïè·…öRÁÑû–´¬ôŠ+À©'™Ûv,ÿrÉÄÍ# ÕÚğÖW
Xæ¢E&JÇWe”q´üï¼i	´7FÕõu%õpo¡-‡z ixç—FøÑ$WÎĞÏ„Ã„£A£0-õ°<q2æ“M5s•úSÇÚª¬Qx×‰%}Æ‚‘òyšŸ—y)‡Â•ŞõËmó•BÍD8ã6Sd;Y1 :ˆÃ	³	H¯ö%Õ›AËÿdD“×lP]jŞ„¸Öt†ÒÓ´TtO@rª96Cî!Ø˜İiU·Y4™fn•è»w"G&Ü5ØÛ”t8Ân:®!'“
}G#¿1‹@Ó77ö4Y
5ïÎ›hKûéa7Üw³5,»À€$A×ÅÒÑ¯È©sˆÈ¢àÁv~!İ9u¿­…(ÉÃòìüï‹[Ê¤Ê¤öoJ4tHu˜€DßŒ"®LÖ£ÄS—êèSHJ™¸;'?”•UÏ˜¥QõUm¢Ø¾ OÕÁÑy|ªE”ˆOxYp‹9ìÄ’^KĞÛ„—”‹²_¯«×Êß”±™…&ü§Z°«-ú1‘·¦>Új1—MÈ¨Ì·®æŒ;0}ÀªV´„aRÃ&+ÇF„3‡­UÆÙìÈÕ]ÊL¢	j ë¦që×¶É Àâ_°Š¿Àº³E§ê£ËÊ§ ²-Ş`ãÅ— @­Ì3l‹^áx–tV³€Z&¿j?™-(¤Ãw¨_ìÈRÓòôyİï$ş+ûÙÕ‡<«ŸXà9¸	c ¸|$˜C’ ½–úoä:02i<Ğ~ıÿa¸š·`62XäšêÊ=æU{!›ŒŠÄåıã|ÜcõNó®¦'=dú9£`^a³8&ˆyx˜áµ²ëE%Ëù†©àô2ÇGz´V¢ßKEyğQü´bœ 8NœÅKúCŸNÆœ[Ã9¢İT÷ö6mÍ–«r‘…8Ãos\ög´]Û&'ö”×q	Ë¢»¦å>€KÛaV›ziKx—ò¯¼w—_-÷çà‰fvÎ×;V4}u„cÛøá.Šê«s–¨³úb‚“:üŸ¨ˆ×Ì|Ø´C3jüósÚ’Ã“=>½ÿo&æï¬@×J¸ƒUS§õš	JN³$‘t‹3LOåˆl*—<`È;nÓíîß¡>Í¡ûA>x{Ñ€8+kô˜5`\©•XºT¿Ú»ÚÖsùÛ@}^1ã¿j’åæüQw$ùf*9¿ z˜±éğë’^?²ËKĞr¶î/ì½˜¹q¾ŞÈàˆcn­YÏ7ïJùŸM£¾V­Y‹éÿMÀê{¢EÊz+(£*ÿÓfÃª‚õ^<òzáL¿‘Èß ¬û±šŞ”+s 46ì@íŒ0D³}»êñÉKB ù/l¬8\NåWÚ%­‰ ‡H7ó1 ğCÕ€M˜FüÍ3‡Ë!©Âªïã(‡A îĞËErÕ Î+n8Ò÷ÚòŠ’¥Ü µhÄ8G›£$>j9M)ÙEå¾ÚÆz][Q=¢ğxÃ´z¹á=.Ò/ê´ÊÎO(ä‡4‹É¶4ÅpW‘+	`´Å÷¾;w´fó¹J•B/Ğ[~Üíäœ3	Æı!—Ø2¶-Á×Ör 2·{”	)dóc“d4†]²Ÿß,b¡ñ¶ÁP½p³,¹ô§gQoâúÓşu_¾ü;·Ú…Éâ9vøFµùš×uÁp ]Ô4/œ™ôC—à«ÏEßQwhä—Ú	%QnÄM˜–îdñ¼Ú¨=ŸzHH&FŸØ¹cî ?xfÖ"_:$_·~ï#uîk•õÃç³9úŞF‰…íü|õ‚Œ¹XŸ?YWVMöZÇı!Íí?é¸q~›·y),Lî£aÿ¹°¸NĞQ&õüâ/—N%À½=À„¡óñªZ4êÎSwt{¢•¹–QŠf3ˆÀ³Ÿì·à`hR#2ş™Éš}L›Hùm`´W˜óÂ‘$µNóûİÚÔ¯œzE‚|’e}	Rn69wË©Mââ\> ³&#ffÅôŠªÑ}”nÆÙzŞĞ1o4\ÿÕ4‚vª ]ÒÀ]ƒ!¢Š—Pèz¡ŸØ›¸Ô©¥!Öt2¥’côuş@Ó1¿yTg>üì]iSwÉ"DÒ^!ZÁ»Ô4Ê¯ÖØsx9º7§5Ø.( u0$/Ô‘•×„p@®Æ(O—Î,ú^SŒB¤E¦‹™»¦e)û‚/",CBAÆŞc»4}J¾b ˆn¦´déæn¹J²)v‡Õ‡ÒÅ8ãi[ß”$2^³´hVÈ]éÊİ¸'ã‰åZ•è3BºİMSóÑZp•½—v!A50œdx_NA6|@P“±Ã=iŞ‡øñÈ½Äÿöe	ÒCò\5-D‡
 §;Ù v"A¼ ÈQŞ`éâŠa[Ç£€•7Ö,øó$béuÒ±ÜPÜÑµ7’ÚRÎÁºŒ»Zl]?X¯p%#k‘¯à >‚¦‰?È$L`]ChTÏË=%j1ÕRŸ6«(ÈVBƒZí{ad(ngÑ¯h
Á:û©í(÷C3n–aêr¬øS“ÇÂà>*<LåAÔBi“¤…å"ø¤F~÷Ôƒ W}HŒÌ‹²Ş
+ ò¯ñ]şq…¾´3u€;‡ë>Õ÷Á
5şu,€ârØ'|‚‘>§Ûs3$`ôRïUë3ì àaİ­6´¢hIRdsÜ›~tÚ¬{ÙoRX-ò¿…7cDu›áŠ×~„d„Ÿã\5öƒV@\ßówqwÌÏØÙ‡¢e/TÔEÙ‰*Höß§üÚÎ’‡LU’‡À—ÿ¬Lr©R
™ {ŒØ&|¶¸ªW]åa¡Ë£gÓâŸ#e€ /2ØÔØ~é"¹<y6Ã)
ÿ¢;{[Ù\o_ğíì¤ÍfÈ:‚Ç×¯	Şl*£6Ìú®×–‘—ªóFOıæŠ©0¬Ï¶‹ü¬¹“ÉAP‹X)‰¿°U*úÆü¡R»#éÔ9Ş7Ä ”k¯$2Ñ+Áã•µl×ap'†½hİ Ô¼O±×8Ì®Ñ°ÉÅUìĞ6î;-œâÿİûßyÁÔªA
60’Mj
ê•¿c·¨ÊÑƒª’YcÑC«Äö6Œé¨§¨)AkçÆfUù4?ã àP‘I:³= mÌ‚âï\ÀQèÇê’—ÂrEbjŠØ´"nºĞ;m7Ó¦Œ¤ƒ•óÿÚ˜¨ÅÈhQ Ì&¢®£wÊP¯êfê§ÿ‹>9ˆ$ÓÌaK›Yé.dƒŒ]é4$s?¬Ÿu»aŠ#ğ#Úì8^†°^E&€È3oˆ„ä°Îb|Şµ’VJF°lo_„,ñ$:#Îq¬˜pŞ×U¨œk‚…¥ylØÍ(çA+PĞ¼–].v¯¾@ÚA
²ÜÒÚ`  A†ü5×}Oîû¡E˜/?LµÂæOĞ–ìDÖ•²&~ü[C>\ú!Ëï÷»›­	[¯5íƒJ2ûÙ8cd‡î¿ÁÒ{ím¦*S®àuu#öş¬ÖdÆ‡‹ı_¯ºS\-›´ááıiOÂùi¶¯]úkBÁÜÍæ˜–Ã‡@à3òíNL2­•ÖÎR³°ÈÛ0J$åçËöo% Üò’ó^Uvúó”…-nz3#¶ÇSåYs”[ë³y	HÒÄ@—û9*LœöJúu ˆ¿4¼Hgƒ±ØóUlt¬û´…œWÃªªèg Äİrœ¿¡¬’B
yÙ{hÜ·ô9M´E,ó4	Ğ!ìığ z%¿ÀUØ—Í-c ¦†y“¡½¾8’˜»‘œÍ Çóz9ºªV—Š‘Cšî©FØu”‰Ç_£'F=Ïù¡æuHFu½ŞœúVö w²,Á§`)FÏ'?Â¶`6À¡Å¡ÂºĞŞMï/%Åj±Ô|ëRpª±ïG>ÌÄ¾ÁU“Y©±¼©Ï—
(9üWâ¸f’Ò§º< ¶*²}Ò5èó3»)lòµún .¤9eƒ:‰Î·ş–5ÏºĞõ:8®³á,‹.à4îÃÚ³*!òuG\‹åj©/7¥/LK~ê-Ëãù¢ì2›ÁGQ!ôßÈÕË4Á•;Ë†´fËÔM 6‚M`íRuî©ÀÆ-Å1ü„us¬{½‡5úiòzÌºv¶:e•E“x–Nä^lœêL.?ü,1ƒ–¨	~ç¤9ÎMš-¸V )¯ôt&ªsÕ¤ˆ5Î_h'H=ÉßhŒ©ÏH½¨úg°!ºŒ…±yòt¨Ï ç¦<²FTÈƒ›ë¯}H¨?.ªe6~E	ímmÿÜs1
v%¦˜!‘*b(®7Ma6aÌ%q¢‹”GrJ/şIÖGçÕ“]€M½:BWA>–@ñúø·kXÙÏ”/jà>d1>Š>QÚ	±ŞE.á@°=ÿúZui›õ„A…§Ï#M¥
”ryòÏ Q>#}6u˜V}¥ãcB³ß]ÒÚ®¹sCrL)BÀéØ?eÏóŞ5˜›„ñğhİxÕ¾Ih«U’ÄŒŸ
¨MÔ›K÷!|}Dó¡ù-	Ğ±ñ«œ…˜W‚@®e÷v~¦â–_‰°*ºL›<¾åÿ¹ú*6Rÿañ©±PĞ¦Awc!På#û±–v=’g@xhü‹íğ× b,øA}û€jáø*ìÇæ˜2
8^‡¦}ªÏğ‘¯TsƒÌnğBCßQ¢8#»2äİuŸ»“ş®ã M¬Ù\Ä¿¥­±µêMç³8º’rG˜ö·9[P§$Z	ËêÃŞ`ôFQ°‘œ2 ¹‡¬áÖ®ûŞpà5<‰.õCôÃ§Œ¶	zL©[£:j<H/
;E6Îâ¨øÜ°n=18røë„ÌQ=İ0ï¾\1”½ÿR¨èN[y
½vYq^Òìò2M;g¬!sb>|÷PÌ:¿a#ìûÛFQ€ğ0Š«%®‹í‹-Efwí-ÿñÈã#50¬N˜8’]Àáøş·cµ9ç¿%é-e)ãâf‚6 qP`»±¡²¿„eŸ&“–Wy›~¾\Ø~
ã£Bçw`PŞÇÚ¿Ù6/¾9-ß­Ñ+ò–±Œ«ë6`jpjè¡Vj\GJøÑ7&Pä«İ½°Æ­ÎR‡¥å!N bÑÈ´wğ}™C”Üü¸ÒU-(î‰ğ˜ ë†üJ6ÌáóYÉ‡&.‚×Büyä0Cæn:˜{œÀöqJScrÅÊï–7§òõPë[hußáèw¥4ùøöïİHU™
Øqş–¹iZÁ{Nëw÷éœê‰À¡¥q”)Ş5ãàC71°˜!ˆüH/w}$ªñbÌıhYÛåÿå€ Cc›œËíM•øY@.Â·;ÔÊX' mÛVÔÀHİk´†WÜìı)Mabƒ‰8£¦¾|ØÊdVMq'á§Rılù”gVŠ_•YÃ_½Ÿá6"z~õ“EL¢Ê™­¾²×V…Nÿ@HAqŒ´–'ÂLR0«±!,µ·yàp/?¡)€1UDö¨®Ü_cƒ›£æƒÉÕîÇRÔwå²ÍÁlâà¿i)~9´ºŞxÑv‹aWº÷›ö)¼©ÜØàì«¹¡‹~º¢ÏâÈâ9ãÓ®³ƒ.A¾s<£A“ã‰0sCİƒˆ2ÀúîÙÏ6\Ÿ.ÍtFùü5Hyiç1»—QÍ:É?¸š§ù)¬„İèH.]ëS’Fõ•UIã/[%äE«ôŒFŒ+Y%Æ…R _D—½û"NmÚ”ğ\ÚYp“h‘m0ñşßˆà¦)Õ†áV{#³D1#âƒSÁ‹ZÃ;ïC÷KòúßÎExÒÕôÓ)@Ëıêån¿¼Ã›t3ü@çöÙ¹…*üÂ—¹½CTJ<Ş<Vp¯Ë°êb0$ÔÛ…!÷é,¯dª>fÏ1V¿Xç!øâ¢èÙ½>·vÊœ;ŸNˆWaár%|¶`óÊÅìåÂ3î‰“ØVfv:ñZ­aìòÀÕ”ïb^L¾s¡vMPP°…iÍ«~Ğ©¼fËIWÒ=Z—B­;V/dƒw®yåå£§Í;Ÿdÿòkâä14O2Ij"$¤r ĞnàÙò*@M§=,R‘cØü¤Y$‚ƒ‰L../r¼ˆ9n%§l8‚İCÃM;a0å+3ç;?Ù\*T@{è1d%¿ş•I0Ö“¼ÁP³!Q»­½²$pö9!Óéˆı5{&¢/ª…JPÅœ©NJ2·ò£^¯åQóÿ™?üâ¶%›’d‹aÌ¬¿òÑj˜ßÜ.ïP{ZÎ4¤°şI×xåû±¬‘ŠË,DyëáŒæ«¯H¤î/İ‹
ÕkòÚG½^‚-›^øÕ@ŞÏ™ ´ØĞö{î¥Y"u”šŒlÁó{gB)P!Œ©lŞ$¹ardnµ¯jÔÖ‰¸wéà±cR8Ù_äìLX/¤®hË‡‘Ïq?[Œ†—g¦	:IP¨ò£ÜXf– µ æ[bHZŠFêÃë½±J­„ÛXÔQ˜nk¨‘ÛVx	5	ĞøÀõH!5‡ı«`öU¡'1‡=iC™¨M·=Ã'rÈ!ÀñıÎCê² äÂ©ñcZõ±òÈïü¥jŒ¸„
„‚oMİÁ7	#ŞÅ\vË›aáIb‘zÆ€£¸%Óş%,	ïªt·H:Ã4‰èI]ía(@&èŒm£QBDŒåstêv`%İÇX.«®°HC‰­ãœâz•Øµ&Ìf}ühêQŸ.¨]’à;D ĞsÔÎ–ø«[™Üg*|6Àı
VĞi0Ù„ Xz¶[ó&—ä¡zDW ?Çìù%-Â˜ÛN)Éİ¥8şû>Íÿø¹ÙŸ¾àç=ÅJéàı¸ëL&AAQ_äë½S¶C”U?”Fü‰Ö~vî{ü!5nW[+Âêâ³§bÂTü‡uuœyée<ë™-·(›?My7ßÌŸU-•	L¥„e˜oĞ@ü¹­¥­ŞRGN]\Õ»5´'jÆ5"§`FqÿÎµG[:Ù}Ò°ŒãˆË½ôy¿ADSX5¦TJÓf²Øİ>$}>—¸{¼ëé›Ì~×íO0#h "Õk&»ÇMÇ{ 7Tç÷ƒÓ¦éÆàÚnØ™!ojˆ®Ó©=0u3›õ‘öÅ6§×/ÚØ|Ø>(?\·Š¿~×¼¬Yœ_;$.R6á&+´Ì‡AÓJœ¬š6g‹ªZ…ÒHps0¢xzmQ“¥uë7ˆX\5;ñÒŠQÚÆ“_ÒÓÚ7¨û44”Ò­SÍZr*‚q g‡2!ğs~RÛš³2¢c[+äû©æ3æ$³¶8ÜšÅ÷y8×½=3I™øF„ >ú»}¤òıı£û‘$ìêËBË”àS9Ëë&PÆx²O@éÃ}SV›"ã_².4æşîs ß×ƒX4;¼0Ğ¹Ña×”B!/¨£JÎîÓlû9Şl\ë¾òya¿Ù<„@~eÏæø“ü¯Æ—¾>W,g^u†§`ùNh+½Ù’ÜÀ C„p¥èi .aÌ& ÆÌ¤?–.3eeé#}äÈ=ª>FÉ	ï]i¢`£>ğÄ;¸©â½+'…‡e¸ßÚüÇ›ÚCÅ¸˜C(™°Ë'ùèÌ0:ùÌ²¾QÅÙòW‡6ÔZ±?ˆt…Ä°¥{O€9~Vï¹oÕÂôQØ·‹Ç.D‡ö€ßö,'4îG?Æ ¸èôÖ™¬Øò3aµtîšò	VüÊXúŠ{~Uî[êr§ÛÅÎv«fBç;½çM¦~ŒŸ*=lodŠóÛ-b¢¸å•j±SÅ) Ìª|İô!Î›=€¥œú>¯°pçW|ßçzkÛ¦‚3Šï²¥¡ÇúÃ/o&s¹
@ÛÇ¿µ#*ÿåî@éåé«ãi­A^P>ãa¿“+*È£Œ#V­ğy.ÌÚ¤Ï©ÍŸ'‰Ş~Hu©ÙRina„ºáëwWJ[¬æq‘+u{‡jySØ:´ËƒÅñ—¤×soéØ‰0¤"ğ4Ï•µLl¸XIßÆ=\<Dï\ŸA—Õ'>:g@ÂŸ¼«ó¥&Å›¸êñ­(ÿ_®VÀƒ4~oÔšQµIÕØ¨úl>†p¤yÇ÷,,üšE¥
ägIµĞàt\OPCû¹—Á!ú¤D–ƒå\IÈ ÃÍÆ“ìIıµ2[Ï½—§Ë"Dz-²>Šıç›nÄ1½Z?âˆQüô`¸Ëˆõ5¿G ÃKy€³0,wUÕWbN{³“©FmŒVíSÌû;^vı\uòé.qîjD–š3 ‰Û#© –GÌâoßUsİÆÒvt™ÿ~^‡ÒÅšuEši½’¸§uVÙ	Õ›G¥U#ŸØMóªÚf&f˜Î´ó&f5b©0ŞŒßlP²S—*tâE¨²¤–î\c¬ª¿Pºh$rò'~8‘=«Œ©¤-‚¨¼çª£Šq¦xYó†cİ9Ò\‚»z„ÜJá}A´{Áşp˜c?¥;9n«´ÜwŞÂÀc³!e”dmï¢›Iıc2ÿ–ÖpõÕ· `½­ëŒÁÊIa|b`£ÒiÚ{²ÿºÔ•š¨5bôJ›:½»o OÓƒï%ƒ=åÌ÷ÅëÇ™^t.g"Êºº’}$ÉH7Nà¶jUqJ7NÒüo¼ˆæ3ße\Íµfwî«6iñC©ìX½µ™BÔ
Ò¯ ¼«ÛåÜNåÁö}§àMU–„4Ô•ë†£²ùµU
¯ÛÃÿ8t[V·§¨0_vr-lü½!ßÕ±nä‚µÄRçDA©;RzGÎ)í>'mæ^®›B)³ÆŸĞ@6™ü[ ø.}çßÜ§ç[D$Ñ=Dò)„“I6PÀõ`_·ƒ±QÓÿ Q®±¼3Sda+LPª²±®ë^Ğ·à|· ‡À²¤Ñ›ı¸¶—„SXÏ†‘–4âùøßFYşD¿DĞ?0Aê–wŸ×¬wÔØµm„¤2qt+bqİwïdiİ§7qˆÊ×|&¾Ú„@‚u‡Ó[C„†54¯ş4hsK·™|ıG±hkÅ;Òi¡Z¯ºDÚ_©ñ/«ßVzÿ¥˜G{ã"b•t _)êeÛù¡S…ä¹£QĞ®«|CÃÕoû,ö;—u¥!³åÒÒs÷·ít£ú¿ğBÕ¶ıÕîÂxHGí	O»¥éŞˆÙÿï¬vÛ(ƒB=zŞÈNÑßwŒêÙÉùÿ‘4«itñØ0TX ƒE«²ŠJÎ2›“€ÖŞuIW€pùØ¯_/ß„èT
v×U`[$Ç^ß€}»xijŠÂZañ¡J‹(–ƒ=a~•¡ Î¯åûDÅÇ•^³ı¨ËfıQljø2ÌK¹&k/Y+\©¦ àÄfå—âùŸh½Åœ@X'iH‘"z¦„”Ë&dª'üOAwC•_õŸçAbUÍ‚th'{c>4Iº¾D†èKŒR.¯F š©øÿç¿÷] ¸ŞîË\VªIÔ&/I³ã«I 4p°|Ñå²ØqÚ"Ònô½çL5‘všÒ°É&°‹°.æ—ç\>}qX]>ÅøuĞ›ZƒR=CN‰üzş¡,'¼+FƒĞÁœd£­C"êrjz$ĞFÀ|D3ÔèzN7XîŒ},õÓ…‚X’‰ª—×¨©*ş\VçF©¶¡[MÔw—anE¿§Cu½¢–Ëš[2p°¦+íNÅvÓÂ×q­V$¢ÉœDñ¯ÌZÔÌcnã=ö;~¥t™+‰¨˜ÖƒÇhÿ¸896)•ÌŸ@í~!#{‹ëá±ƒ6¡‹¶³ÔÀZôD>İõ ‘\šÖ&ÛtÇd Õ@1¾A»ù8Û&åÈÍñ-à"ä=ø–fÛÙ!F`]PùÖ¯ bê–sC(:õFÈÜ–í+u£PÑ¾¯ï<™áŠğ˜_‘VÁüo5§ÛêjnÒĞbk„•¶HvÆN¿YQhœ IÚÆ¨‚ØvĞñ%î^h£`A]áÕ H5Ê]ÙÚU0…9ìÅÔ¼ëàwÅ€9½2‰sd^-´f šoĞkƒMëË©]ÅîM®ıZ",®Y—°ÓD}c=¢èîs¡MÚ°U{;yÉ
Y]ÓÔìöÍ('7ÁÌ rµò‰C&^S"1­äE¥Z;ÈºpTXù:R+‡û÷G‡mYl.<Á÷Ô¤êŞúD$j¯@à;êµN¿ {&¾&N|òëQó®®?ÎB¢§°³f½ìÒ	)XƒQ·ßSksıÇècõÍ¦İæ5I-wI)¹¸^f¼|üªåiWî &©çjğDHÃĞK³{	©Çtm˜¡KÊŠÅÇ}XÁy›‰ÂÖê"0?Ô ŞÙş<bíONÄ£3úQH2úÁ{.ÅPºà©ƒ‡4E¨T¸›1 Àõ<½Î±Ç?–SQ›`WØõ†‚Øıß»49{)ı%åE”Ë0»O ¨2Ì r¬&x&‚OÑáã>ce¨RüÒ,•º°½$™¸.ğî“YnWÊn™UÒª‡Ş“‰E—ĞL²³¨úD¥È[0sv(ÄšSÎ\)3Q4*Êâ‘{AØ[;l˜‡Ûjì1½Ã„lÕl“í¹"2YÕ–ÅŠ´£Ä,ût¯´§±GÕ}£«{ü&àÈ?°Êúï±äˆ% ÑÜ$>Å*£ä½ò¨Èn–÷u¬Zâ¹u–)n¨iò:ß““å­Ui'i£Wå6—hã’‹1 r:u5n)T+I0Èéì€T2;èÍªO|ò_mqÑ”ËÕÇÂt0ïİI€D+tEô}Ì6Ó.æØ¤œ3÷ÂÃwW.5§vK¢hÙ¯şÛ¤_ òRK~mu3àÿüF„E·Å¾Ë¯F5“ì8ö05¤ÒÊnƒ«È$ˆŸ7º°)ÿTB:2Oü æÇ&îÓá¬=1ƒfç\°®Ñ§¶Ã:0Õ/1!dç†ô¨nS’È:-úi´*Gøißs.Ü ÙÚ?èD†œÏ
íÜ¸À	_¨;icÀ¹w½Ó¡â¬"è07Æ!*jÉxJÂ<Î€§Qİ{f\¬´ºŠU>Û’Å0ŠÉ,$6L²•rŞa™.7•ÿHÖÜgÀ½5ÕN5\îTu²]¬ºnÚz„5ğ´¤²¸òšş„tŒgê´ÍT<Š‘!ZY=ŞÊ1êõÕ2Cw9~ÛNÄ”ÕÓÉ™JE¤M×#ëèê1Û¹RbTJ`OˆX]!¯ó4ñ„!›¿£­,Ôu)6g"‚­¸cœ¯—»<K#0ïÕŞbsHÒ6 ¨8Wªt#àKòìôØ±ÙÜ–3ÔñkÛµ‰bGÄÂÓtú[Â S‚M	=‘¥ä9? 5©ÂÑ1™µ4ëË.sø‡2´ì¢@/ØÊ?é“S1×]¼°üúW(r›dIÀn®]ï²ºãºæ³ØÉdà°Û¢E…Ô&:ÜKÜúJpO¥­²ûÄ	\†[øÜ¬ÆÒ?Å”Q_’"&*„™aëŠËXaÎ)üH°ç“4¹“%)¶Ô¼ybççë üÒ*”MK?yQìs2Ó¥”à»ç4£ —Ûªëø$”k§ş¹µ‘Í’ü‘Ä'*°æú6S‘\µ§VÈ¨Ö®z×ƒ•¨±çàÏ!Õ›9øÕ]<H÷èJ Bß…Ã¢šVI§¢uöñE`ø@MxOÒÒğû}ô,˜ë²kù­M•]¸ÕGAraÖ»Şà8¢šY÷ëÏ´çÜj/_š³? kAxwˆ¢Ö¼+=÷z³™nÀ!Pë^§èEŞ†e°s^ÍĞÏÆ´ÑI­$ä_Æ¡ÚRz©¶/7„Ç¾ä=ª®±€¤ãõø1Òes©_ïmSyˆªÎ%ÕZJOéU kàÁÆëñL¤dğêi’/y®õï ç“M:>¯ğU×JH$jN„ÅŸ&Q¼ØÉ²Û —SÆ¤şğBf,aM/KfXBˆ8Æ ‡#†Aş«>q«=ÎT7úRŸø‰ó¼¯±¶O²iS¶»~=((dÏ|ó»nèLcÀÕF1H´›‡Ãlí„ ¡ò—dÊ‘üK¤ëoaê‘â¢M5¶£@]Ú\9gÚ#·¡€Ñà©Uí+!ÖfÑ›ÙZ‹šÛú >²TÃ«öˆ#Á˜¤:l«[ç|¤ˆ­|TºÇ40}/§×ùpÇy,‰)–>ÂE¾§“åÜ_öõóàæ¹£Â- ò)òOnN œ'—£üş%ß9è²¾à&Í$l*—iğ–»£­
aùê ©\ê‚n™«¦ŞE(˜¾0ßaô}RÊÂDw+”yÖæ°?jçU&¿ÌXNV„ò˜²Õ¡€æHŸ=Zª¯OHmQ”9Xzal
|–¤ğò˜³C3ê†›÷º)œÅÔÙfêÑ÷öä§ŠwÛÑ.øI%ıÁàœšd½ÿ»øŞ´*íà»´œ4¬ûÕ ôpÇ5/ÈIARœpÃÌ›ÖjL=AÀ«¼h¹Š“eÔé‚uûû„íw%-¦! #ö˜ß‘Ohâ^K)BE?¢ÿ¬&bzq¹»b†jÁh/á ;	9‚,Ñ¡u+@`³¯µOu·˜
´ø	qolV¿‘Ã”£€£‚RJù µ ™fd‚ü¹æ4ÒƒÅÓ´.J!S¶ tæœ4Å†aù[¢Ïˆ¯A£ü^M!>il²Øú3Jmß\OÖê²…+2A64ïÑV.á$ô¼KgßÂmı©7Â'e˜üU–‘`õbÕKîCş>*(Ó¦6ó•SÁö‘iòoáS¥­›(_—ÄÁ×ú3=ùíä…e\0†y¯
š§«Z¢¬N¨f@¹åNİÃ!€@ZOJ4;Æ(Ùãï­‚’>Ëì;G%Ù/·@
’XheÄX%r²Ù«aB6¼Ë¡°ñdñ[CÀë‰9—F¥ınv×ğ2c–‰pÛS:ıJ,ìı„f6cYŸƒ£,-œ`Ègóºe3d!.ød›[)Â¯xÍ÷JÏ'[e€âV‹SE©-µ÷ÏEff>>Ş náÿ~ªÊÛ:¼`P¨*€&H¥Fƒ&Ô„]B’aºwuÆ~[^#\•àÈô›MïÖî5¢ğbù(€Ã%U®ˆÚ2/!¯KĞµI§†::EÆŠ=Æ)²ûª’KF{ÙmíVò$;§=•:NKz¸°§¦Ó*€ÿùÇ9ÃJù€ñ÷Ìø„ ’P0İ‡Åc³¨‡¤o†:Ía!EƒL›Ó¯Ù7úZpóªÖCl2‰qÆ©"6+i	åa|7Ç‡˜çÅ«">ñÊÌäÅäNôÓ_Kaèb¹’®ÛóXı¬ÃP4½4õğIÓŠ†Ş¹#8îïÇü/?j©ˆ1³]
î£0ããi]	½Õ$XœlØw{rwps…¡• <u/ bÈ‹ç'n7&îl~ÖÙk³ÄqFX	°Â=ú¶¤å¯@×©IC¿s;Àa ÍR¾j²“N{¦Ø°°~«+áÕ˜ï”V­´ÛR–nF·Ò˜k}\ÕìB¤İÇ6ïgNxzËewQÓ‚îrà¡²;	a;†ç%+fU¿—SVĞ^‘ä4´ŒUåÛÀ9<ñæ½×•iŸ@åÓSWÖOÒW©š~â¢hÔÃ¯y2üTãC£Q
Àé4tlo¶:}PB×=~TÖR÷{u9—ç¨ağ×çı#‡×:wıF¯nªäE“„ÛÃ(+p{Æ†y³²ùù$N$ #sÉËj­î9¿~®ªœyeÃèW5wI9ú«ëJ4tíº	¢C„1?ÀÒü…whp§A&Oôûµ{ıN¿Nwwb#ñpLÇß¡Ro7·”Å[ŞVáP&ÖOĞ#2Û7K|h2r®P?0Î)l€-ÖnˆO HÉCç?[Úá;­ı&?Ë\€3C`K&\GÓ»-r…%Yíû­¸®œ×‹š²HÛí#«¢|·K&õ&T+y«[Ğ¶XvtHª%Ù¡¾ëWËìù&ERY‚2hÏíÛ€[ùÖğ×ÈÎSÆÉX1tĞwïšW]š¦ ÒûâìëàkkãÂ	º[øCßpÌ7·7¬ëŒEƒ—>àq­•7ú¹µİ[ŸT$’ë·ÌÛècvF0íN&ÒH¥²	Çğä)`hßd„^¸kˆ¤•ŠG¨Wj¥ù"5›]Gwh3h^XÒ–R¾ªúÉ×ğİ´óc.¤‰2ÄnaåŒÃÒĞq¨ñ3² /ñP(º³¡±Z§>ò­>j+{[´ÕµVwáy@EÅ'lóú¸1ƒ·ü½LXdˆrV%sè¶Û?ÑdÛ›ÓP|tè››·(ñuÀš9ş*ärX·KÕæÃ¤V|/²À(M´ûc½sVO¼Î ³œŸ±ùÇ7´Eİpœ@eÎpCXBpœG]šºV%ÛÖüŸ&òc%MD§(Pó†®Îæü&)ìZùÌŒ\Éš¨¶”Ÿ•÷İo0%ª£oƒ“b¸d¬…'¥&K€şWÜs'úÿbA lÇåC£ù“”—!Æ ×5¤êÈÿQ|§fd‘S®áâˆ©ßU:ÑÌ·¹¯H¢—ùRâĞ ¶ °*ğ{´n-æÒ`›dùÙŸaı;åÅXŸ¬ÍÁ0c?êµ÷Š{ŸQB˜ ¤£¼5¥Ø¼&yì AoÇÛZáÚUL¯pQÂyÊvF¡lœÎ¼;#½‚åÕ©Œ´¤E´„A²l
›á&ÍĞëHlÉ ù‡ƒßˆMešn†ø®jMÛ~ª›võ ".†	ı©IµwõıPLe(1plU'áÕ\ÆV4šâ*‡¸İı_½·=6·;¶2wü–)Í üBc»şp—ÏøUJg#+îípEU´¾¦|t¥àfIàgnuäÙîd@8Š«¦TKä<å)6/‡4KÍÃYùì­û¬9Ñ”•¯«¬ë£äNèí mM7wİ­$o–¹„>çêÓ£»
põİØÖÄP”s‘Píª+è=hfÖé#‹	ETÚ’©*ûZ«Ğm#‘:X’2t`8VcTŠŒ-„fBı¢^Ò"Asº„vØ!,ÏøxzÚı«EÆæ)¼ÊßüÙ¥ºüï¸ÿ<®UÇİ"’¥}!¼7ÈÅ8Ú»ÚôÅÙ	º§•’'“Ä¢0§Ã4\-Ö|<,@ùUZ`‹g¹EWé+CHÚŸãÒÅ¼³èıËñ–=X{>fit|l¾ÆeWÜÍ|=Æóà`ñtLš©‰Á Éé‘Ràx¬y^;¥ş“äOÕƒhÒXô¡ÀwäIÉsßÔUg˜8
·ßïD¶gÙé-pR!ì´WÓ×Êö}q.Ò¦mÇ¶ù™÷0{)´WÚ}ë´éVÒ¬²ûü×şcøìOz<¯­Û*ó>Š½Y°Ò`@=$Í§LLÒdşÅzNÌİæ¦Ÿ9ø0;y\„É#ö&Mt t¦iP†AÎêrİúoßÂN˜ˆ‰„¥ÛïâÙ®vˆÌU9Üæª—xQ?Íå/Œ!C!VëØ%wÚÒ-¥,%¨üxh={©‹rİà/_‹óGhÑ5ŠhºJ˜Uå*®=r§j4…Ççš jä¨OFâè5ã°`3|Ææ*´©ÈÆåJÍ6U&ş¹€Ğ@GæG|=C»å$)»‚ÑøJ~ÜM&îö”8Q]nZ®eÏ[0/ò²ŞÔ{ïİÇê©ÛÕê­ÑÍc'o»ÄoÇîç·³¬‚×Ù„ÌáO2û‰#ß­ãYîRV¸¶6VŸñö¨SÌØÂñÒíaÄ1ÓHùš\3hÑºŠ’ŞywPü\’¥†c÷—YóÍÄ8»á ãa{ÙÒ`eÆ¹¾õ’Öè}¹Ÿ(«4“M0›Iö‰‚ç&ôQ"MD¢LÁÎtq§X±ı ÉøRW­VËøN+4êÃ¥ä’ğG…ë¾Q‚ãzöWšæéëP¹×j8·ÜÅXİÄça.5ş}Få–î‘ò8°/'a™‚¾OŒÑªŠAVÓz#‹ü²Šé×a™ÙÕëp?…™) î¶|\ÎóÓ
Èjrñ@Ÿ·’â¯*Ò'Î€ôîº¨@=Ÿƒùç#á±E×ö¢ºAK©¹İñcå¥şÀ‚‘Uë}PJà¯ÇMêxÎ©ã8Q±‘Oˆaà¼-_Òu³LIux—£0&+6Ç3û<¼l~$*;¡#H›éf©)5%6IzLCKQG]O	P ‹âÿWiU ®ËåÁ@à:ú«û‚3™­2ÅC-Säé²ÚW
qk3noæSßÜ’†À«!àOA^c¥ŒàïH¬~V$ÔÄ•S£I¢“<#)å¦Šóc»@öî¤°*ü0i6Œ;kª‘‡§æLÛB:Ô¾Pî
±²uª6s}~„|Ğ€{¢º¤Øbä‰ÊZ»Zœİçå#ª&¨X·ıñy+Î\9UdŒ.æŸŸvb^B•{€¶Q%ê÷˜¶ÊdÄ&İ/²™ ÏpØŒ&‚Ã.Óetgâôo	~3è¤ ªN¸÷Äxzº²¡“QÜ'Çöôü®$f­ûÄ‰± ·â1Äu×Ó„90¬¨Nô‰INgš¦3Z!F¨$OOyÎ[İä†D*W‡åŞâ¥’(–€ªÜ€Dbâ[æƒªŸb¦=Rqân&Ìn²>(tâ¬LÈáNIâ¸äæsJ·7:&„R?6‡
´£ñb3 í³pR€Kê§E%”$¹˜[ŒtÔp\	M²> Ş‚pú3Ö½ÛµŞàÔı%r§a¼gk	û ­›9X÷¢ÒJ‘÷®ºj<h«í¿¡kä¶©¢~(¸ «a‰SgßÒçÖİ!KÔÏ’ogtY©€T,¬Ò¸ËÆ2Ş´ÂŸºd’–‰ç~h¨å¯˜r°$w4-Ó„ ‹Øí-9ÿ¼@˜Ïq"ôÜPW8Çß–%.0˜L±=Ä×¾´Ù¾½¦bğ£'6'ªŒ1ìd([#Xs§fDÇğ"G·ú;tusï¹|b¯
ä>T³MëÃ*Ê‹á½J˜Ü¢Tdã?&ÎÃû{Ï6Ìä\kgF¦ma»^ğh¼«ZÃ>šªºá9	OPÑ¨Ô6º:Í”‘^úf³<–IZkï1^l%™ßœú×Â:àÛ™òòy³;:’º·ú“€éN(Ü…˜_QÀü(0ÿ«ÃTÏ:Œã°Úm$‚3äÊ¼N¥ˆjĞ£‚[W°ôş!•Ğnò®™…²¹g¹§F£&6š4Ğ0·ÚĞÂ–UjNÚå_Æ&©hÎ‘rOJß2‚1±/y‰|…Zr–¥f9EŠM•6µÓî	È*®O($Ñm3¯yÛ3‡©;´ÓŒr	}3¡‚mÍ£N¤ëieIfFm*ÑP‰½#‰Jâ·F³Èñ-!•lıÿnÈòm`ü~‰·FÎG¸ÒÁ]QÜ´¥E˜e	
ì_¤ë"²MÖˆWÕ×ÃÀéŠ÷ÙYÅò Ÿ5Ò¾k{Ívº_É¿—ÃaºÇEí>ø’ÛKèĞ0ô·•"ˆ?,¸ãbÔly€£òVîã±JàTƒ†Òüü‰y›‹q÷@iR•Â@ i;ÆÚÌ˜PÌ#>ÎÁ¾¤ŸÂıĞ[adÁ1†ÃªÁxƒ¯YøıõÜ[À$hÏ*j„r>MEdòV[æ|ß¼ƒéF\tjEhJõ§ı«k¦¨Ö=Ğ9¾ÌåWÜbÿ`qËQ±eÙœ¬š“Ïi"N}Š¼ûo}ÂQ8Gî<|;71dôŒ4¾ \¶}OşÙç*fßUŠW"«^Á2åŸ¼H	ú«3Ê§Sêh”ËÀŒÃg‚AÊÌ	oq
Ênêp‚òš_âšSè½	Á¼ºêoLjåfeA2Œ{+||BCt*,„‰Òú™Ûˆ† }‘nº.˜F½çKİ%àv‹~ã=yš)	œY€¦¿ ²dï –h"(vw¢ìLÍ1ptLÜßu“$˜–æ¹Qœ9wËÀåœT$¼»{Õ]æÜ‰$qg€8#$ùü›*şã²§üvÔ—ÙéÈ|H¦vPy—cxĞïkë¡v
-Øx>Yjã[ÉQàæ<ññz<à¿ ëRdbÇ!6Æ;ïÈÑ|Gd?1œTõ˜+E®l¤n…´VcX¾´ˆXŠQ©-Q/$Ñ€9û8&Jô•KÛÅBş´ê…E&¹¶’Ñ'İ™0­æ7Wª æÓ¾	yé÷È÷Œ5GuŒƒ—¡^İ9Ä¦…6A½İnnCÆ#÷{~P·iR9M×1©5BÑ6TægÓÌTSºDÂÈŒè½³FXœlÓc ŸXF½pIÚ‰Ÿv*Ä¯wŞ[1ìdÚ,Fißh`àë(<OÒ­bR |.&%à¹sÓ
‹è)0ŞğœX§Ö¯§>ÃRGÊ#/Q.‚ÖÁÍsvãB´-5}U‹Ve¶ÇÏê{mÕ”¡z‘ooy;ôÔ³"m¿&Z€A³ë;æDl YØìÕ×«R{ÎiÁ*.XS„å2ğhŠ„‘&È÷ßé 8¼©…âíòBĞ«'iÅÄÒÁ-<‹Î¹lVlÍ7&ìÓ7æ'ÎÎ$6,äñö|r?éÙ€BYãRÊÃ/ÉT9É‘Zq'YlwÄuò}Hç×1ÈºC"¸ıí’¸	úçË%çj´õ$JweÜÚlSj‚ÿ¸·ø^aºÁ©xA=­ÇÄê°‘"°"Ø­rjäŸíØzä¸úÒŞ¯Zªó¨H,4tµ|±›059í­Ö§¿´"'n¨FP.0~§°ÖŸÁTíN²\»•yÙDbiµ{#lÿ‡¬ìÌ$¥üq][è¢Ê&FŸ'+‚S§CÙÈï¬^ğÏ]æº5h¶O±ª†Œ‡ ì;’·Ui{\­|ô‘÷½â	Q>1…ï Ç&K$~ñy{=Å¬4¡÷5¼’ñé^ÖÎ)GvRìŸ<7=hùŞ‰à¸—4x/½
¢=U—¶*Ÿµé½/ql‰¸}·]éÒVÅg¦U}˜ó0·“\§Â\ &e@ù€c{A -~Àÿ}°Njs³'Zs<Eí‰»&éCtçÇ#Ê­†½ÚU_V£©M°;RÉP–Áe?¿u¡ª÷Ï—Ói¯¤	PxãH²µÙr\»#óeÍøÒ‹>¬b·\âãŞ(V\ôt°¡ p’o=ûeU—%–¤N/#*eáŠÂVçfG3€”VÚ(Übª4È„!!&P[s‹¤9íPA«Úæ)£ígèI:·oŠÒ0®Õä5ñ‰[×•ó}sœvlŒ±jÙhÀşhù„æ—9ıË°d±9–ÀÂœÜw¦$è>­¾›F‘G“Æ=8”tÓ}
Ò‡¿¡0<‘iL;I7Ø¦üDr¨¸ƒÇ–°×Iø"J÷ˆ_“¨¢ß7'pIĞâœ­¾;l @ÿ\ÅJt¯LÕøz›Wuâ±—İ§ˆ¾`W²CÕç'üÕ*¼[AJô=%Š¨øÍÚãºq[çg.62oJ°­ ´nnPğè­XbT}—@ˆĞ´úöû~=PF!nëDÈ€C§ú!ƒ‡¨Ç´*HsNÙÑ‰Äsì\½ægÓ­…;ÎIbÆèi^QZ×¡=}QY"ŞU¨# d ¶sêUx!)4¥¡ñâ0›’Òj& ¿›±
l1@¹µì´p ö¨%ØÆo2;Ñ·Jjßÿ(Ätn»·4¨Bf¦d®Ä\Èn(q¨HnÜÇ@¿íkPvAoÔS¸Xöøã4­”çí§°«åîgàüâU¢°]úøv9Ÿt¦ôR C²Ÿ3ì°[¹³††~Çô¶KÁk±ë(ÏdIÌäÉ|+ò>õQGÌ^EãçÈ±¦¯(ŠN};Ón‚ÒAãËÀW|Ësş$ t³¡Û
%p Ä^v
W|²’¤3°e/[gud 3¿r5pcô½œ‚2ÀËÇŞ%Á›©è<(-O‘ó^ûûìé¦³tÀ±™2%ŠqDM&Vşp=BY›û$d•\acñöÚøHY“‡ÓéWÒN5í¥Ùd È;¢„#©ômÛGÆ	ÑyÆH½£õ*ô)Ót¤•vâØ.”QĞäşÅ#öş
;ı\ûGˆïÆ
X©ıô¶Ì÷EÀşèì)’1=Iíä ğ)Vı˜Aå¤Ù~¦æbç8÷êTÏ³­8²iãÜÖúğqCÔÆ*“Æ?bqÀn¡ap"–­	W·}
¤¦LIï2Ó¡[|Mt2]ĞŸ}¯®ÈÖüeu&º=“à]¢J+1¹ÿÂµR÷û481{á+T<–ª"ÕU&jÃº)C©À'
ÇĞ½õÆ•ï{îwc•W ôĞ'}–¼Äb	X?")€@H«á,«ëÌ€JÁlXÏ@CpQÚèÏ -Áf+A¦®õs•mÃ"yüİNäY•w;|Á÷jgÍ±*ÜáÄÑxï©İùÈmtö±ÍÉÚ5”k·V™p£È˜’Ô	D”zªˆm"6¨"µ¦W„®…›h'`Çnı6qÑ?¥œN™ÉÉ_MÅª¬±Bw˜ªj³ƒğKEéZ1/¶dGeÊÏ ˆZ<“SÔ­sji¬³(“bÓJv,ˆ™D‰U´gv¶ò‡0Ş8é¯º¦¶­ú³Ä7ş±ÙÑ9îH*¾BmnG^ Ú£ª6°ç2|G| ï]<(:z1Ş€ïÔ«ÒÇËš°¤€×	<—‰i‘Ğ§ÿ;¾‚‚?ø§îj­½+*YS*’Ræ‰Y$â[]UŞ™MqE’6q$ÿ‚ïş»	¿3ó¿V­‰õóD_ºªu›Xèr2Ÿ^R‰«îYşz”ªyèHZ6YwpäŞØÉë6ERYÅéƒßú¤—ó§,q?ï›à­>¿¢•[•ƒ3Ü>zfÕ¥CÍY¶‡«ºç]ã/YácsïEôÆ>gLô}X…‹|[_ï6’ßË$YîlDàN”±x#Ôü®òFÖâá£ÿµÑ2ÓÛ*/À…ÈAw|æÓv6rl‚ó¢á”t®d4O`_îxÊŠfÖ}J§ö!EO+—ìŞ§ºİOíÿì6Ğ5›çÏI3¾™ôGyÄ´1Â^+ñVd+\ÿwÿˆğæÄ›o¯\ğÔo`Âm¤`e‡ƒßòùºèDª­ foí2ÛÁà¢dŒ\j:ëíFH#ñ£uÊ±©'Y–7Vµzæ§­·ğ:ß`3&Œc=ùÇ`h¶¡ôØÿÑ8Ôi×xM^[šÊjiÄVppõ+=EŠ|Âd“Ê–_Qfí%ôi“À3d‚;û™Š/Dş@³
yéò1AéŸç¾8Òv#›Jè½¼/Å¥Çy±õĞ|-·b¶ Û;4Z«…±½ßŠ/°1Ï—Â©Å<¦2lw›œ-]|f~¹T6ºnüêQ0ñ#!bÏb*»j¶¥±uC-#WôEFGîŞädàöG!²óR÷$F­hŒqÁã¢¶Ç»e›+rª••.»]6X“Ëi£Ìµ×#v7/n!¢›OĞ¾"dI®5Ë«ÿá¢´Ân\[lw©¿2aå²˜a,Õ—¦Jè…S3½t@^û0p°±¦öÌafêˆc><ô=HüfÖÆ75¬—¹!™ç¤qã>ı›SqÒ;²Q9ƒÅ&H	™ÚFÈ‘?ÊÛÏŒAšµ¬A«ĞDs3àg~®N
µ"Ùigœ3NÖ/­²’'][¥¦ıÕbÇV¢÷+>sŒûÊ:ûq…û”ÏĞp'Py€QiSm©ªˆ?úÑçËê$c[ï¹©ÂÀX8’çÓÁ¿|îY©Iwå‘œÃ”NË,­«ôe\LÇWV›x£‡dMÓàDÓ_qÛü©50f«XĞ—3E‡<›ò›Ó‡|bï}Hßñ9ÆYÙcËÆ÷ñb"/„Ğ=_Ş©8²B©ò|ë3'É¹<ïÈğ#saùs,:b$KŠ0ÜÃ/åù»u•1–czÎ¨²Sš1úp‚C²>Ah¹\1@³BN¨ç™:ºï't’$j^Y™KgåÅ5C\Ä§mc	ÍñµIq·ÏäÛ;âû–§ÄšZ×©É@˜"uâÜœ;8šYw@GbŒÔıï‹Ê«²-•ÔÈ?uWT4ÿÇi›è&L0ësßI‰İÜœÉ£›ø
@ÇÜ šª\ûì_n†	¨DëÉÿÇ‚ìÌI¡d6X\ù2õ$&¸#glí/Ü% ş>®åİÉ¦Šç}ê•éß}ü
-8ÁªUh†BË
AÃÕ¨Ş4Püà“?ÒzïÂi‰[³ÔŸ4˜µh^hsSVÑç'"p.K<zıjå˜{Y]|<@îØ,•öÔ Á¼ë1Ÿ%cùÇ;f@"*ãş3»´¯ÀîMd§™£âÃ’LÀcvò¸•GÎ4nÍ3iö»wÚäÄv]¾`Æ»vğ-Za…¿î@£>’ŒŒÍ¨ë•Üì¸z¹i‚7 ƒE‹¬Y‹bQk ©] ËÄ@›«šâ|ĞSmô˜ÖÂ"ö	ü:ùVoü"	qÅÓ—ÊcY€òÁ1¡F3¢‰64Äx6ŠÇ4bcuDâÛ ËvXi©±¼‹ûº›ƒ}vi¶ÈhMÀÍãs…3z}ì1¨öü8q>ä*†ã8H‡İÄElDClÑb~­%¾-­şf>—béœDHƒ>9ú(r}é}Õì¸+1T‚Ü¡³t¹ô…óÔ#®Ggc±ñ®Oi5:Å5^Ä{¿S ¹Úø­£PŠPŠÅ
È/‚ƒ…Ü®+{°ÄÑ§l<M&Î3ğrµíRä:…—•4FŒü7`Ù(]d‹ÊÜªq&¬ Ì“5Ôx‡«cêeDÑÉ…0ı~#´ÿohÓ8ÿ»2é—.6¹® ùËğ”"ÍŠü(0ÛÇâxtÁJºúëµÁ¹-^0õZQ},¢‹Ó,¤#;ÖM9…\w¸–šs#ÖÌ)ÿí&ŒËRh=Î‘ŒÓ1œ!+W§F²8²~nzş6Ñ65Ì^»*niE=A%$®ÀÌ\Òô¶µ Ÿ•w)T³¨¡æ])@ÇÀ^®Vx©(¹Á•}Ñ(æGÌÿ%+ÇÓı4·ï½‰ÇC¥®%Kçı·fËáH`æv=UÕÙo„ãÌ©Œ`‘Ù6â[::p2Šæëu—tpxzÖ(ÑEÑÈ¤$±’]ê »ğX	3~Ç .*ÜãôÊ(ãó¦Õâ"ÒT€³IŞÎ4¡ùìªrÍ«j7”Hıío¤ÈªÜÃXVÒœÊW-Ééz!ë,v0QS¸ÊË”¥dŠŸÍ5j±™©Û^¯5´owçéRÉWÿó9<èÔ¿ÏM±Ú‡ —OşyçÆºJ`K¬õ@Î$û’©%\ƒIô2vä²ĞLjêÃú;mÌ"Qôš	hLÌÉ!î8à„7I):èB~èèoÎ í“½‹6±fb¹%¥O“KköÑqÕSkªYs 3hpPº(H™ƒÙ
iûÒTêº`	?ÄJıW#k„ùßƒ’`‹{ZiÕ$"-®qK²f’â³pMeçàÏ§ÀíÔx0ÆœŒ·NçÒ˜ä]2ıJxö:l‰ò\$‰fY˜ºãÆ´®@x&N>¥İ¶Ÿú&'ú™™µ¯nOš.ƒ¬;oÙË>µ´Í††‘qÀ^=WØÀ¼púr¤Ğ^ ÈÃt”â÷ğâM”İg¸I•Éş¾Oë#âäæ—YÚ“BŠd%'´$—%»¨Å«S_"œ)ßbq<·AìR=ö·ùÖFwÔÁBÁ—ôp®Lû*ü]ì@a‹/Mo*§LP x¥Ğ¢ü‚ËE‹Lµp±9é¹áû2/‰9®à®øå{)mxŒéâÇÆfË¶¡ĞFã+ô'2ctoÈvÊ(K“¯>ã0²v(*®<ÛW]K7ÛÍÜ]t!OkYj~(œëôîÂ‘ûáìÄ’yU¹ƒÃÇY¨kéíÈxÏ‚åv©Bèÿ<;&Š…~¦‚ùb$Mªñ[¹Qÿ 
2ÔmşÊ=kæa×vÂßá
 Wq	Êëb
Ï´ÕY¨ıÎË5ÛÏ„-ÌVc5Ñÿ¯»)¹„Ÿá‡3
Ÿš;ø)Ş7ŞÓ¦‡Ÿv‰’ÇO•4µÓ›±ÁÕE ê¥şË@2©
„j…0YV³‰ÙºÙ½˜&gz±¨ü¢%s%ì+âîV(®CJIô^Máå÷ê_©ğñêŒê2)v£ÜÑA(øÓWH0>N„¹ DäœÓãÿt$Iµe¬9†Ûê~©a•Ö_+w¹ËItJ^,D6HkE-ª»Ö‚·sõÉî¨«ÿk|=BÓJ¾È‡mıPu‚J”æĞ=dgdãLèkåº±-$OT³¬ôhå$^òG6z°Ù ªX¾ÂpïPvìÁëÀ=ÃZ-Ó¾8véÎ3èbv‹ÄVûlfğ (Ô›Î-¬j×PP{uH	“¬&Ë¯xåîÖî(9=êŞ>ÅÎ÷ï ıpbâ±à½=m_pŒ–f|˜İôPiHŒ\¶£Õ¸ë.£=Ue-ôÂµeêw/´Ê:–m˜G ëe×jÿx=¾Æ1È\Aò²Ïáf/©“ğò*äşqúEĞ ~Ü‰Øı*¼<V[°Ö–+.¯ÇÁwˆ*BU­ÈòZ²X¬ë/ÿ—*¢¾]“"şd;ÍÃû¸2‘mŸç9FkõãµçG²KÖWE¬h_kPà±3’Ñ`M²°4g&í‚áˆ°9'.ÏÒKƒ£“Da¼ûC‡mI·hêÂùÀøäô{å“@Ü&¶B4m<S;¿)4 	j‹º|£*ŠG‹Ä+ÓHõúŒ|áÇá€ÒÚ—Ñ£\é,ŠÄ<N~Ší‡×;¿G:ò7sO¤|äeá]™?mÛå›åìŠÍ)ĞVÖÄhP|Ää·¼Ua9¾ ùzã¯Û©  ‘èĞ§ ÿ1ğªáFTÏ›`ô¡¸¼w‘¡¢ô“‹M%evâ¬“"%#5>™#EÉçb	 -’j8çô`ÓìĞ®³×Lí*ñ §2@Su»æı8¤¶ô§45A†Íà–èÒ0VãİrsR*…÷VOÄÏ…ç–çÔ¡iÖ7(U™²Ú¼˜GV«û‘ßšÏ/V4DäMİ‰“İwÄ™O%T
³;ª8-«¦F2,ß2ôtRnšşSó-D;µ¥ì{ï$:Aôà#6dİo à²i¼Oi&·dv$…äº&Â1?º ¼çÇ’‹şBCw&FW_»3Ÿ^éÆòç÷¼0*û£dNXüÃ}kU§An—Óh‡¤UĞßÎ¨ut{†q€tA—p¸z¡§Je³.ÎTø7ñ†ªèqÂàDÏ²o¨ş°×Ş®Üeï¡{¾ïµãî•Ô†K“‰Èn)Öô~ ˜í…¼šu¸·.‚È”)é„éŒù§´™‚dk™q¨ûPÁÀ¡ï½ÏIÈk…e°ŸÑmøÄ*ÊQˆH{ªD*€ÓasÀ;:í)`ÙŒq¯¢šú­†âÁ¾G{™«÷Ü„œñ`´ÊŸÉVÜ‘ uè}œ’Gı²3İ”Æ ép¡âx“'”ùĞòÍSs»ÙÃãÿAÃ7ƒ”v‹¬Ç:òkÈ6cmXÊøÃ$£ôN¾Ü«–x“ëIãgŠ:#¾N_è$ò\Nèd°t`÷=ºÛÁfª¹îo½0ª¿J §‡OÕx;»ª®«ÙWÑ€ qƒ‚Üİ÷h.Âİ3r,»ùbù½·×ª¼†‘OúrYM\}|œÂ"b±“[5FlIV	ôı[¥G»®tÄF­e üEßµh]¨ôõ½¶ODÎè9şD6Ã6'ºÛ,|¿ô’ºZPñÇï¯]BÙ76Š¦2œè.ˆmd¸Â‹	¦!‰‘Ú1ŞøÆ1­j†Tıq~ğæ:ß{$Ÿ—ÊêQ5¨¬ü3±‚ı0Õo·*O=«Šİâ{yÌT ÈûÈ«\`Ô]~²šFPÙ]Z"ÜŞ¦
—‡¯Û%­“œ«ôI¸3jh Ğ¿1§g¡gD¿NÕİ¡‚©µr3ÛÓyõ·Ëƒ8	ït‘q$aô ©`¥¡iÕ×ëq%×âzMê,0HˆÑ'¤³}FÂ0Y©™Z¹PŒ1!"‘U÷”y“ü‚éWNH	FbÔ	¶aûò“¸¯SñÜPßÿ‘Kó‡FÒ‡N{wãtv5Ó Ûuè/tÊ­®±ìHNü1’Eîê‹¬½‚?„.ËORñv¥ŠTÎtDW4IõˆâñsåÓJÈ6á¢\«ó&:ªtÇ6+ÏMWÃ_¾™/XbR—ï)xªk½ñ¯|u+OØü¬ıû:çå³»]UÍÃÏkıD-·§ˆûÅ’ñ<FC£ÅCq¨Ø×»»¢<ï»‹¢ÑW«´u9>ÍáİNlré5¼JëVzÀL«ıÆ®r0¶íÌ›`¢¶6/Æ&İÙóå¥Ÿp†ˆ†¹ÁDäÙ! ÂNBlŸ¬PµÃ"Ü‹ÙŒá[Ñ,yuU{°L,Ÿd[j›œ3|×ï={cN-Ô1ÚCÖ9d’¦Ït€'ºë4øÉb–)I}XÿÊ½«£DÖ
PÈ
¬èèb‘şJ“öÈ¢Ó+±©°PøYIÆ’@ã„šfy'ÈV°ÀpÈ¨ÆœĞìBOrx…}³KJ…¥ú{i©›/ûª§}]!Û™X¶‹pÌœôtÆÍtÔÁÿ*0_ Îë#š#İ%˜wÂS¢ğQ…} G¢DÿÜrº»dêªáDR\*nB:soÉc÷Èçn’ó)m/ä5Á'r[¬ÃüÿQqÌ…œû`6ÒW{nh­ÛQ¯3åô/ôñåw‡K|Iâ»ı¶ºWÔ%Ÿi|–rİÒ{ °òš1ÔË"0/àÑ“¢¹]ÖŸmÌ	ÌPÍœ¥H
k0İvÃ@ãÿ :(æÜ¥v§Ì¤ÀrêÛw7Ş„// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/file.h>
#include <linux/suspend.h>

#include <media/v4l2-common.h>

#include <media/dvb_ca_en50221.h>
#include "s5h1409.h"
#include "s5h1411.h"
#include "mt2131.h"
#include "tda8290.h"
#include "tda18271.h"
#include "lgdt330x.h"
#include "xc4000.h"
#include "xc5000.h"
#include "max2165.h"
#include "tda10048.h"
#include "xc2028.h"
#include "tuner-simple.h"
#include "dib7000p.h"
#include "dib0070.h"
#include "dibx000_common.h"
#include "zl10353.h"
#include "stv0900.h"
#include "stv0900_reg.h"
#include "stv6110.h"
#include "lnbh24.h"
#include "cx24116.h"
#include "cx24117.h"
#include "cimax2.h"
#include "lgs8gxx.h"
#include "netup-eeprom.h"
#include "netup-init.h"
#include "lgdt3305.h"
#include "atbm8830.h"
#include "ts2020.h"
#include "ds3000.h"
#include "cx23885-f300.h"
#include "altera-ci.h"
#include "stv0367.h"
#include "drxk.h"
#include "mt2063.h"
#include "stv090x.h"
#include "stb6100.h"
#include "stb6100_cfg.h"
#include "tda10071.h"
#include "a8293.h"
#include "mb86a20s.h"
#include "si2165.h"
#include "si2168.h"
#include "si2157.h"
#include "sp2.h"
#include "m88ds3103.h"
#include "m88rs6000t.h"
#include "lgdt3306a.h"

static unsigned int debug;

#define dprintk(level, fmt, arg...)\
	do { if (debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s dvb: " fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

static unsigned int alt_tuner;
module_param(alt_tuner, int, 0644);
MODULE_PARM_DESC(alt_tuner, "Enable alternate tuner configuration");

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_tsport *port = q->drv_priv;

	port->ts_packet_size  = 188 * 4;
	port->ts_packet_count = 32;
	*num_planes = 1;
	sizes[0] = port->ts_packet_size * port->ts_packet_count;
	*num_buffers = 32;
	return 0;
}


static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);

	return cx23885_buf_prepare(buf, port);
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_dev *dev = port->dev;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(dev, buf);
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_tsport *port = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_buf_queue(port, buf);
}

static void cx23885_dvb_gate_ctrl(struct cx23885_tsport  *port, int open)
{
	struct vb2_dvb_frontends *f;
	struct vb2_dvb_frontend *fe;

	f = &port->frontends;

	if (f->gate <= 1) /* undefined or fe0 */
		fe = vb2_dvb_get_frontend(f, 1);
	else
		fe = vb2_dvb_get_frontend(f, f->gate);

	if (fe && fe->dvb.frontend && fe->dvb.frontend->ops.i2c_gate_ctrl)
		fe->dvb.frontend->ops.i2c_gate_ctrl(fe->dvb.frontend, open);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_tsport *port = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &port->mpegq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_dma(port, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_tsport *port = q->drv_priv;

	cx23885_cancel_buffers(port);
}

static const struct vb2_ops dvb_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_