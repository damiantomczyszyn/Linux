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
	iU�J:�2��B����Q��� �i���-���s7X�tMg^��06q�Ei����׳V�I*"I�����`���[U����;`�������ĉ�o�X�E�6K@����#��ײ��3=���Q<�����G%c���ީ�͑��p�YǋY���br���V���#�����M}H6���<k Bת@`�676�+����3��c!S�;��^����&���:e6��=�BW��%*w_qv *�0H	�*n��څ3����� p�yI���f�2L��U^�����
��b����=��@=����cO���(d|��+I��@��)	�6�Ϳ�8d��&��?H���Hʿ��@�FT�d�P������pW��_���j��b���� a��0/tFũX�K���]b�fȎpW�~_qH=�H��D0�#�	��m���~��S閽�%V�!?,���g��cl���֐�w�����J�w�����lǍ��f
�Ab8TڻE4G�	&}G@X����n�=�a���]	W6X���l_�&����F�\�	�ކ���C����ْZ�]i:k�[$;RAnB@���s(��>��=?��4� ү`�ٗ�|Y�u�����ĕAr���ɮ��&΁
�AZ���&��k�H:��޳�4�}n�/�Q�χ�J��Ի�Վ3!`S�]�E�H7�a�|V_~J����am��E���p��')��{C����g�gu)�]?�ճj3&iV��D����T"ɭ�n
�Йh�򨉅��b�.���+�Y '[j��疆�����-h\@0f:��j9�9��}�?�V9� �kb� -��8���a"�k���+�c�_2�k��+�IDщ����}�Ĭ���cF�bk��L�>eA�)`����RB:[�cH��$̼:����J٢��w��듲���&�1c����f�t��pD�e��:D����sb��ն�u�d�0����q��>��k�|?笰\gE���V��������m���p*
h��A��	��r�ļ��'�o���}��3�By�j�b�[��BV�n����7��}Y�eX�E�c�?���`ۦ=M �� �d��ۂ��t������D���r�H�-Y}!~�D�z,W�q]��hE�<P����q��1�r@�)2S�Ik@�UC��OP]��E�0��{�:LA����R�Q�F��6H��j��>r������6=�ްvCN2���z�c<l⓼R$w{&^jIi-����/��JL�'\oz��d��+�|J|.S=�&/_[��*v��Ўg��%W��+�%�_�;���A�2Sг�������X�O�qɀ�*�g�{��Z�=�S���:Nz�,^�hE�v#�Qz�I�!"�j�Е�[���j��s&��`T]"�/LB�H����*�O&�ئw��NNo��蚠��vn	��&��r�>P����&s4*m�5�(�JD$��{�V���1���2�q� ��ە��Yi�*gu.c�w|I���d̓�T����؂���3*�)~�L!G?)�U �t��qU��Zz)����?+�Of����4]	S�DJH�$ �s g��6Y����
�)ٍKC0=}��(�XA$o˔�~T�yh�OK)~�%i����g��〧���pX��,@�g}Eȏ?�ʡ�m�a����j-t@d�U�SsӖ	���c�ة,G��T������	>7�g���6�>Y�^�a�-F�'6L22
)D@������6�g��|�Q3H8 Q��'qzi�k�؋$(�����~��^F��g���B�C�ܖ*�L#�rߓ>�5}���ޑO�3���<>�h������i� ꉊg�z7an�Kӧp����Q� Se�v��1x,�ҿ�������x$�P9h���X-4��߮����>s ��^]8�$�c۩$��m��Y��MH���M!�����\��	Ͻ՝#��(��?ְ��0�V|�tA?��щ�4�B3���%�Vv�ըnt�����oVݵw�A�Tcb��rJ^-iP��&C/O������E��c�jft��G˓@}�7Z�8d��/�zFU�j!{�s�c��n��?+����0�*�mр�2�ifQ&nx[��X�!ƅ�Wwt?��m�8�xb�L��U0����%�c���f��i��E���V,��xG��_��9��<Qx��'���ю����Mv�� ���kJ}uC�J�`���2�zߒ���)b9�;���!�3/��t�m�t9B)k�-���ʂ� UO푼+�5�4(.��� X˃aޮ�O���:SIbR
����7��?�Ev��#�k'b���ǭ�L
���c�Xg�\1���i8`Y��6��q�u�l,tݴR5����?E�q4�"u��)U��r�*dǧ�֠ւm�Hw�b�u�����$�X�]c*��	T���Ub�PO
yOY�wp�)4S%�g!���q�7�ݾ^;0w9F��e\7!#��2��V)�Q�_�b���y�+�6���>��IC)�y�L�(t�w��������,q��L*�R5��p�q��vn�+��i^z���Y����d��Gթd��i/�8����,���r�RMh`�;�xs�X�b���Wfa޾����������+�Fm%$�T�$?޲�m�Z2�>��O�)�����s��/6�}'�>4���^[���t����T���+��zl�* �����x[��4�.���)�|z�Ð���Ғ�������z�y]l�pa�p�1��
�:J��$=��H
��J�A�m\�����dm�i���ӹ0��q1�D?�'_g	K�"�[0���xD�L\!p�wb-��Е�0���x��`�]b����j�՝�s���ԝ9��Js�eԩS�O�C�iD��������f�C�c�3�f(%4�d%�S.v�ϊh?�������,D��s����!1nwjf�JH��:L�K�Ocy�?��,�62]��}������t��P�1����Y{�� �ޞ� �W$��9|����ni�l؊��Ng L�e4a�H4���?닄ȷk;O�{�,�#��!NP�`J!��}�J�?u�	r���a��
��=��;�f�1[��m9�4 %�%;�*D�m;��r�D���YS'�q�(|�ƺ�Ȳ8��Z��pj9O��5����G�'�i�o|	���|}�D�w ���v��Uu]�]��	��x�oZ�f&%��s��᧔��Ҫ�"T�ni��s�����4�tVձ~PP�V�o6��M��]��4�
��
���Tby�b;tP-����,���@s�w�![y�w�_j��U{T��
Q���r΄)�i֥��(�ff�BUn^@�c��;�����M.m	8_�y�����àS|��Y9��R�:�T/�s�QbE��8���A�SzN��H���ck��::����/�/�s��}Bc�d���q�V�(�跅�R�������+��'��v,�r���# �����W
X�E&J�We�q���i	�7F��u%�po�-�z ix痍F��$W����Ä�A�0-��<q2�M5s��S�ڪ�Qx׉%}Ƃ��y���y)����m�B�D8�6Sd�;Y1 �:��	�	H��%՛A��dD���lP]�jގ���t���ӴTtO@r�96C�!ؘݎiU�Y�4�f�n��w"G&�5���t8�n:�!'�
}G#�1�@�77�4Y
5�ΛhK��a7�w�5,���$A���ѯȩs�Ȣ��v~!�9u����(������[ʤʤ�oJ4t�Hu��Dߌ"��L֣�S���SHJ���;'?��UϘ�Q�Um�ؾ O���y|�E��OxYp�9�Ē^K�ۄ����_����ߔ���&��Z��-�1���>�j1�MȨ̷��;0}���V��aR�&+�F�3��U�����]�L�	j ��q�׶� ��_������E���ʧ �-�`�ŗ�@��3l�^�x��tV��Z&�j?�-(��w�_��R���y��$�+��Շ<��X�9�	c �|$�C� ���o�:02i<��~��a���`62X���=�U�{!�������|�c��N�'�=d�9�`^a�8&�y�x�ᵲ�E%ˁ�����2�Gz��V�ߞKEy�Q��b� 8N��K�C�NƜ[�9��T���6m͖�r��8�os\�g�]�&'���׏q	ˢ���>�K�aV�ziKx���w�_-�����fv��;V4}u�c���.���s����b��:������|شC3j��sڒÓ=>��o&��@�J��US���	JN�$�t�3LO�l*�<`�;n���ߡ>͡�A>x{р8+k��5`\��X�T�ڻ��s��@}^1�j����Qw$�f*9��z�������^?��K�r��/콘�q�����cn�Y�7�J��M���V�Y���M��{�E�z+�(�*���fê��^<�z፞L���� ����ޔ+s 46��@�0D�}����KB �/l�8\N�W�%��� �H7�1��CՀM�F��3��!�����(�A����Er� �+n8���򊒥ܝ���h�8G��$>j9M)ٝE���z][Q=��xôz��=.�/���O(�4�ɶ4�pW�+�	`����;�w�f�J�B/�[~���3	��!��2�-���r 2�{�	)d�c�d4�]���,b����P�p�,���gQo����u_��;�څ��9v�F����u�p ]�4/���C���ύE�Qw�h��	%Qn�M���d�ڨ=�zHH&F�عc� ?xf�"_:$_�~�#u�k����9��F����|����X�?YWVM�Z��!��?�q~��y),L�a����N�Q&���/�N%��=�����Z4��S��wt{����Q�f3������`hR#�2��ɚ}L�H�m`�W��$�N����ԯ�zE�|�e}	Rn69w˩M��\>��&#ff�􊪝�}�n��z��1o4\��4��v��]��]��!����P�z��؍���ԩ�!�t2��c�u�@�1�yTg>��]iSw�"D�^!Z���4ʯ��sx9�7�5��.( u0�$/ԑ�ׄp@��(O��,�^S�B�E�����e)��/",CBA��c�4}J�b��n��d��n�J�)v�Շ��8�i[ߔ$2^��hV�]��ݸ'��Z���3B��MS��Zp���v!A50�dx�_NA6|@P���=iގ���Ƚ���e	�C�\5-D�
��;� v"A� �Q�`��a[ǣ��7�,��$b�uұ�P��ѵ7��R�����Zl]?X�p%#k��� >����?�$L`]ChT��=%j1�R�6�(��VB�Z�{a�d(ngѯh
�:���(�C3n�a�r��S����>*<L�A�Bi����"��F~�ԃ W}�H��̋��
+ ��]�q����3u�;��>���
5�u,��r�'|��>��s3$`�R�U�3� �a��6��hIRdsܛ~tڬ{�oRX-���7cDu���~�d���\5��V@\��wqw������e/T�Eى*H�ߧ��Β�LU������Lr�R
��{��&|���W]�a�ˣg��#e� /2���~�"�<y6�)
���;{[�\o_����f�:��ׯ	�l*�6̎��ז����FO�抩0����������AP�X)���U*����R�#��9�7� �k�$2�+����l�ap'��h� ԼO�׍8̮Ѱ��U��6�;-������y�ԪA
60�Mj
���c���у��Yc�C���6�騧�)Ak��fU�4?� �P�I:�= m̂��\�Q��꒗�rEbj�ش"n��;m7Ӧ����������ȝhQ �&���w�P��f���>9�$��aK�Y�.d��]�4$s?��u�a�#�#��8^���^�E&��3o����b|����VJF�lo_�,��$:#�q��p��U��k���yl��(�A+Pм�].v��@�A
����`  A��5�}O���E�/?L���OЖ�D֕�&~�[C>\�!������	[�5�J2��8cd����{�m�*S��uu#����dƇ��_��S\-������iO��i��]�kB��͞���Ç@�3��NL2��֐΁R����0J$����o%����^Uv��-nz�3#��S�Ys�[�y	Hҝ�@���9�*L���J�u ��4�Hg�����Ult������Wê��g ��r�����B
�y�{h���9�M�E,�4	�!��� z%��Uؗ�-c ��y�����8������ ��z9����V���C��F��u���_�'F=����uHFu�����V� w�,��`)F�'?�`6��šº��M�/%�j��|�Rp���G>�ľ�U�Y������
(�9�W�f�ҧ�< �*�}�5��3�)l��n .�9e�:�η���5Ϻ��:8���,�.�4��ڳ*!�uG\��j�/7�/LK~�-�����2��GQ!�����4��;ˍ��f��M�6�M`�Ru���-�1��us�{��5�i�z̺v�:e��E�x�N�^l��L�.?�,��1���	~�9�M�-�V� )��t�&�s���5�_h'H=��h���H���g�!����y�t�Ϡ�<�FTȃ��}H�?.�e6~E	�mm��s1
v%��!�*b(�7Ma6a̎%q���GrJ/�I�G�Փ]�M�:BWA>�@����kX�ϔ/j�>d1>�>Q��	��E.��@�=��Zui���A���#M�
�ry�ϠQ>#}6u�V}��cB��]�ڮ�sCrL)B���?e���5�����h�xվIh�U����
�M��K�!|}D��-	б񫜅�W��@�e�v~��_��*�L�<�����*6R�a�PЦAwc!P�#���v=�g@xh����� b,�A}��j���*���2
8^��}���Ts��n��BC�Q�8#�2�݁u������ M��\Ŀ�����M�8��rG���9[��P�$Z	����`�FQ����2 ����֮���p�5<�.�C�ç��	zL�[�:j<H/
;�E6����ܰn=18r���Q�=�0�\1���R��N[y
�vYq^���2M;g�!sb>|�P�:�a#���FQ��0��%���-Efw�-����#50�N�8�]������c�9�%�-e)��f�6 qP`������e�&��Wy�~�\�~
�B�w`P��ڿ�6/�9-߭�+򖱌��6`jpj�Vj\GJ��7&P�ݽ�����R���!N b��ȴw�}�C�����U-(�� ��J6����Y�ɇ&.��B�y�0C�n:�{���qJScr����7���P�[hu���w�4�����H�U�
�q���iZ�{N�w������q�)�5��C71��!��H/w}$��b��hY���� Cc����M��Y@.��;��X' m�V��H�k��W���)Mab��8���|؝�dVMq'�R�l��gV�_�Y�_���6"z~��EL�ʙ����V�N�@HAq����'�LR0��!,��y�p/?�)�1UD����_�c��������R�w���l��i)~9���x�v��aW����)�����쫹��~������9�Ӯ��.A�s<�A��0sC݃��2�����6\�.�tF��5Hyi�1��Q�:�?����)����H.]�S�F��UI�/[%�E��F�+Y%ƅR�_D���"Nmڔ�\�Y�p�h�m0�����)Ն�V{#��D1#��S��Z�;�C�K����Ex����)@����n��Ût3�@��ٹ�*���CTJ<�<Vp��˰�b0$���!��,��d�>�f�1V�X�!���ٽ>�vʜ;�N�Wa�r%|�`������3���Vfv:�Z�a���Ք�b^L�s��vMPP��i�ͫ~Щ�f�IW�=Z�B�;V/d�w�y�壧�;�d��k��14O2Ij"$�r��n���*@M�=,R�c؝��Y$���L../r��9�n%�l8��C�M;a0�+3�;?�\*T�@{�1d%���I0֓��P�!Q����$p�9!���5{&�/��JP���NJ2���^��Q���?��%��d�a̬���j���.�P{Z�4���I�x�������,Dy��櫯H��/݋
�k��G�^�-�^��@�ϙ�����{�Y"u���l��{gB)P!��l�$�ardn��j����w��cR8�_��LX/��hˇ��q?[���g�	:IP���Xf��� �[bHZ�F��뽱J���X�Q�nk���Vx	5	�����H!5����`�U�'1�=iC����M�=�'rȞ!����C��������cZ�������j���
��oM��7	#��\v˛a�Ib�zƀ��%��%,	�t�H:�4��I]�a(@&�m��QBD��st�v`%��X.���HC�����z�ص&�f}�h�Q�.�]��;�D �sԁΖ��[���g*|6��
V�i0ل X�z�[��&��zDW�?���%-�N)�ݥ8��>����ٟ����=�J�����L&AAQ_��S�C�U?�F����~v�{�!5nW[+��⳧b��T��uu�y�e<�-��(�?My7�̟U-�	L��e��o�@������RGN]\ջ5�'j�5"�`Fq�ε�G[:�}Ұ��˽�y�ADSX5�TJ�f���>$}>��{����~��O0#h "�k&���M�{ 7T���Ӧ����nؙ!oj��ө=0u�3�����6��/��|�>(?\���~׼�Y�_;$.R6�&+�̇A�J���6g��Z��H�ps0�xzmQ��u�7�X\5;�ҊQ���_���7��44�ҭS�Zr*�q g�2!�s�~Rۚ�2�c[+����3�$��8ܚ��y8׽=3I��F� >��}�������$����B˔�S9��&P�x�O@��}SV�"�_�.4���s �׃X4�;�0й�a��B!/��J���l�9�l\��ya��<�@~�e������Ɨ�>W,g^u��`�Nh+��ف��� C�p��i�.a�&��̤?�.3ee�#}��=�>F�	�]�i�`�>��;���+'��e����Ǜ�CŸ�C(���'���0:�̲�Q���W�6�Z�?�t�İ�{�O�9~V�o���Q���ǎ.D�����,�'4�G?� ���֙���3a�t��	V���X��{~U�[�r����v�fB�;��M�~��*=lod���-b���j�S�) ̪|݁�!Λ=����>��p�W|��zk���3�ﲥ����/o&s�
@��ǿ�#*���@��鍫�i��A^P>�a��+*ȣ�#V���y.�ڤϩ͟'��~Hu��Rina����wWJ[��q�+u{�jyS�:�������so�؉0��"�4ϕ�Ll�XI��=\<D�\�A��'>:g@���&ś���(�_�V��4~oԚQ�I�ب�l>�p�y��,,��E�
�gI���t\OPC����!��D���\IȠ��Ɠ�I��2[�����"Dz-�>���n�1��Z?�Q��`�ˈ�5�G �Ky��0,wU�WbN{���Fm�V�S���;^v�\u��.q�jD��3���#���G��o�Us���vt��~^��Ś�uE�i����uV�	՛G�U#��M��f&f�δ�&f5b�0ތ�lP�S�*t�E�����\c���P�h$r�'~8�=����-���窣�q�xY��c�9�\��z��J�}A�{��p�c?�;9n���w���c�!e�dmI�c2��֏p��� `�����Ia|�b`��i�{���ԍ���5b�J�:��o OӃ�%�=�����Ǚ^t.g"ʺ��}$�H7N��jUqJ7N��o����3�e\͵fw�6i�C��X���B�
ҏ� �����N���}��MU��4ԕ놣���U
����8t[V���0_vr�-l��!�ձn䂁��R�DA�;RzG�)�>'m�^��B)�Ɵ�@6��[��.}��ܧ�[D$�=�D�)��I6P��`_���Q���� Q���3Sda+LP�����^з�|� �����ћ������SXφ��4����FY�D�D�?0A��w�׬w�ؐ�m��2qt+bq�w�diݧ7q���|&�ڄ@�u��[C��54��4hsK��|�G�hk�;�i�Z���D�_��/��Vz�����G{�"�b�t _)�e���S����Q���|C��o�,�;��u�!����s���t����Bն����xHG�	O���ވ���v�(��B=z��N��w�������4�it��0TX��E���J�2�����uIW�p�د_/߄�T
v�U`[$�^߀}�x�i�j��Za�J�(��=a~�� ί��D���^����f�Qlj�2�K�&k/Y+\�� ��f�����h�Ŝ@X'iH�"z����&d�'�OAwC�_���AbU͂�th'{c>4I��D��K�R.�F ������]� ����\V�I�&/I�㞫I�4p�|���q�"�n���L5��v�����&���.��\>}qX]>��uЛZ�R=CN���z��,�'�+F����d��C"�rjz$�F�|D3��zN��7X�},�Ӆ�X����ר��*�\V�F���[M�w�anE��Cu���˚[2p��+�N��v���q�V$�ɜD���Z��cn�=�;~�t�+����փ�h��896)�̟@�~!#{��᱃6������Z�D>�� ��\��&�t�d �@1�A��8�&����-�"�=��f��!�F`]P�֯ b�sC(:�F�ܖ�+u�PѾ��<������_�V��o5���jn��bk���Hv�N�YQh� I�ƨ��v��%�^h�`A]��ՠH5�]���U0�9������wŀ9�2�sd^-�f��o�k�M�˩]��M��Z",��Y���D}c=���s�MڰU{;y�
Y]�����('7�� r��C&^S"1��E�Z;ȺpTX�:R�+���G�mYl.<�������D$j�@�;�N��{&�&N|��Q�?�B����f���	)X�Q�ߎSks���c�ͦ��5I-wI)��^f�|���iW� &��j�DH��K�{	���tm��K����}X�y������"�0?� ���<b�ONģ3�QH2���{.�P����4E�T��1���<����?��SQ�`W������߻49{)�%�E��0�O �2̞ r�&x&�O���>ce�R��,�����$��.��YnW��n�UҪ�ޓ�E��L����D��[0sv(ĚS�\)3Q4*��{A�[;l���j�1�Äl�l��"2YՖŊ����,�t����G�}��{�&��?�����% ��$>�*����n��u�Z➹u�)n�i�:ߓ���Ui'i�W�6�h㞒�1 r:u5n)T+I0���T2;�ͪO|�_mqє����t0��I�D+tE�}�6�.�ؤ�3���wW.5�vK��hٯ�ۤ_ �RK~mu3���F�E�ž˯F5��8�05���n���$��7��)�TB:2O� ��&���=1�f�\��ѧ��:0�/1!d���nS��:-�i�*G�i�s.ܠ��?�D���
�ܸ�	_�;ic��w���⁬"�07�!*j�xJ�<΀�Q�{f\����U>ے�0��,$6L��r�a�.7��H��g��5�N5\�Tu�]��n�z�5𴤲�����t�g��T<��!ZY=��1���2Cw9~�NĔ��Ɏ�JE�M�#���1۹RbTJ`O�X]!��4�!����,��u)6g"���c����<K#0���bsH�6��8W�t#�K���ر�ܖ3��k۵�bG���t��[�S�M	=���9? 5���1��4���.s��2��@/��?�S1�]����W(r�dI�n�]ﲺ����d���E��&:�K��JpO�����	\�[�ܬ��?ŔQ�_��"&*��a��Xa�)�H��4��%)����yb��� ��*�MK�?yQ�s2ӥ���4���۪��$�k����������'*���6S�\��VȨ֮z׃������!՛9��]<H��J B߅â�VI��u��E`���@MxO����}�,��k��M��]��GAraֻ��8��Y�����܁j/_��? kAxw���ּ+=�z��n�!P�^��Eކe�s^���ƴѝI�$�_ơ�Rz��/7�Ǿ�=��������1�es�_�mSy���%�ZJO�U k�����L�d��i�/y��� �M:>���U�JH$jN�ş&Q��ɏ�� �SƤ��Bf,aM/KfXB�8� �#�A��>q�=�T7�R��������O�iS��~=((d�|�n�Lc��F1H����l� ��d���K��oa��M5��@]�\9�g�#�����U�+!�fћ�Z������>�T�����#����:l�[�|���|�T��40}/���p�y,�)�>�E�����_����湣�- ��)�OnN �'����%�9����&�$l*�i𖻣�
a���\��n����E(��0�a�}R��Dw+�y�氞?j�U&��XNV�򁘲ա��H�=Z��OHmQ�9X�zal
|����C3�����)����f����䧊�w��.�I%������d����޴*�໴�4��� �p�5/��IAR�p�̛�jL=A���h���e��u����w%-�! #���ߑOh�^K)BE?����&bzq��b��j�h/�;	9�,ѡu+@`���Ou��
��	qolV��Ô�����RJ� ���fd����4���Ӵ.J!S���t��4ņa�[�ψ��A��^M!>il���3Jm�\O��겅+2A64��V.�$��Kg��m��7�'e��U��`�b�K�C�>*(Ӧ6�S����i�o�S����(_�����3=���e\0�y�
���Z��N��f@��N��!�@ZOJ4;��(����>��;G%��/�@�
�Xhe�X%r�٫aB6�ˡ��d�[C��9�F��nv��2c��p�S:�J,���f6cY���,-�`�g�e3�d!.�d�[)¯x��J�'[e��V�SE�-���Eff>>� n��~���:�`P�*�&H�F�&��]B�a�wu�~[^#\�����M���5��b�(��%U���2/!�K��I��::EƊ=�)����KF{�m�V�$;�=�:NKz�����*����9�J������� �P0݇��c�����o�:�a!E�L�ӯ�7�Zp��Cl2�qƩ"6+i	�a|7����ū">�������N��_Ka�b�����X���P�4�4��Iӊ�޹#8����/?j��1�]�
��0��i]	��$X�l�w{rwps��� <u/ bȝ��'n7&�l~��k��qFX	��=����@שIC�s;�a���R�j���N{�ذ�~�+�՘��V���R�nF�Ҙk}\��B���6�gNxz˞ewQӂ�rࡲ;	a;��%+fU��SV�^��4��U���9<��וi�@��SW�O�W��~�h�ïy�2��T�C�Q
��4tlo�:}PB�=~T�R�{u9��a����#��:w�F�n��E����(+p{Ɔy����$N$ #s��j��9�~���yeÐ�W5wI9���J4t��	�C�1?����w�hp�A&O���{�N�Nwwb#�p�L�ߡRo�7��ŏ[ލV�P&�O�#2�7K|h2r�P?0�)l�-�n�O H�C�?[��;��&?�\�3C`K&\G�ӻ-r�%Y������׋��H���#���|�K&�&T+y�[жXvtH�%١��W���&ERY�2h��ۀ[������S��X1t�w�W�]���������kk��	�[�C�p�7�7��E��>�q��7����[�T$�����cvF0�N&�H���	���)`h�d��^�k����G��Wj��"5�]Gwh3h^XҎ�R������ݴ�c.���2�na����q��3��/�P(����Z�>�>j+{[�յVw�y@E�'l���1����LXd��r�V%s���?�dۛ�P|�t����(�u��9�*�rX�K��Í��V|/��(M��c�sVO�� ������7�E�p��@e�pCXBp�G]��V%����&�c%MD�(P������&)�Z�̌\ɚ��������o0%��o��b�d��'�&K��W�s'���bA l��C�����!�� �5����Q|�fd�S��∩�U:ѝ̷��H���R�� ���*�{�n-��`�d���a�;��X����0c?���{�QB� ���5�ؼ&y� Ao��Z��UL��p�Q�y�vF�l�μ;#���թ���E��A�l
��&���Hl� �����Me�n���jM�~���v��".�	��I�w��PLe(1plU'��\�V4��*����_��=6��;�2w���)͠�Bc��p���UJg#+��pEU���|t��fI�gnu���d@8���TK�<�)6��/�4K͎�Y����9є�������N�� mM7wݭ$o���>��ӣ�
p�����P�s�P��+�=hf���#�	ETڒ�*�Z��m#�:X�2t`8VcT��-�fB��^�"As��v�!,��xz���E��)��ߏ�٥��︎�<�U��"��}!��7��8������	����'���0��4\-�|<,@�UZ`�g�EW�+CHڟ��ż�������=X{>fit|l��eW��|=���`�tL���� ��R�x�y^;����OՃh�X���w�I�s��Ug�8�
���D�g��-pR!�W����}q�.��mǐ����0{)�W�}��VҬ�����c��Oz<���*�>��Y��`@=$ͧLL�d��zN����9�0;y\��#�&Mt t�iP�A��r��o��N���������ٮv��U9�檗xQ?��/�!C!V��%w��-�,%��xh={��r��/_��Gh�5�h�J�U�*�=r�j4��皠j�OF��5�`3|��*�����J�6U&����@G�G|=C��$�)����J~�M&���8Q]nZ�e�[0/���{��������́c'o��o��緳���ل��O2��#߭�Y�RV��6�V�����S������a�1�H��\3hѺ���ywP�\���c��Y���8����a{��`eƹ�����}��(�4�M0�I����&�Q"MD�L��tq�X�� ���RW�V��N+4�å��G��Q��z�W����P��j8����X���a.5�}F��8�/'a����O�Ѫ�AV�z#������a����p?��) �|\���
�jr�@�����*�'���@=����#�E����AK����c�����U�}PJ��M�xΩ�8Q���O�a���-_��u��LIux��0&+6�3�<�l~$*;�#H��f�)5%6IzLCKQG]O	P ���WiU�����@�:����3��2�C-S���W
qk3no�S�ܒ���!�OA^c����H�~V$�ĕS�I��<#)妊�c�@�*�0i6�;k�����L�B:ԾP�
��u��6s}~�|Ѐ{�����b��Z��Z����#�&�X���y+�\9Ud�.柟vb^B�{��Q%�����d�&�/�� �p،&��.�etg��o	~3� �N���xz����Q�'�������$f��ĉ� ��1�u�ӄ90��N�INg��3Z!F�$OOy�[��D*W���⥒(���܀D�b�[���b�=Rq�n&�n��>(t�L��NI���sJ�7:&�R?6�
���b3 �pR�K�E%�$��[�t�p\	�M�> ނp�3ֽ������%r�a�gk	����9X���J����j<h���k����~(� �a�Sg�����!K�ϒogtY��T,�Ҹ��2޴�d����~h���r�$w4-ӄ �؝�-9��@��q"��PW8�ߖ%.0�L�=�����پ��b�'6'��1�d([#Xs�f�D��"G���;tus�|b�
�>T�M��*ʋ�J�ܢTd�?&Ξ��{�6��\kgF�ma�^��h��Z�>����9	OPѨ�6�:͔�^�f�<�IZk�1^l%�ߜ���:�ۙ��y�;:�������N(܅�_Q��(0���T�:���m$�3�ʼN��j���[W���!��n򮙅��g��F�&6�4�0���UjN��_�&�hΑrOJ�2�1�/y�|��Zr��f9E��M�6���	�*�O($�m3�y�3��;�ӌr	}3��mͣN��ieIfFm*�P��#�J�F���-!�l��n��m`�~��F�G���]Q�ܴ��E�e	
�_��"���MֈW�������Y��5Ҿk{�v�_ɿ��a�ǐE�>��ێK��0���"�?,��b�ly���V��J�T������y��q�@iR��@�i;��̘P�#>��������[ad�1�ê�x��Y����[�$h�*j�r>MEd�V[�|�߼��F\tjEhJ����k���=�9���W�b��`q�Q�eٜ���ρi"N}���o}�Q8G�<|;71d�4��\�}O����*f�U�W"�^�2埼H	��3ʧS�h�����g�A��	oq
�n�p��_�S�	����oLj�feA�2�{+||BCt*,������ۈ� }�n�.�F��K�%�v�~�=y�)	�Y��� �dh"(vw��L�1ptL��u�$���Q�9w���T$��{�]���$qg�8#�$���*�㲧�vԗ���|H�v�Py��cx��k�v
-�x>Yj�[�Q��<��z<࿠�R�db�!6�;��ў|Gd?1��T��+E�l�n��VcX����X�Q�-Q/$��9�8&J��K��B����E&����'��0��7W� �Ӿ	y�����5Gu����^�9���6A��nnC�#��{~P�iR9M�1�5B��6T�gӝ�TS�D���轳FX��l�c��XF�pIډ�v*įw�[1�dڐ,Fi�h`���(<OҭbR |.�&%�s�
��)0��X�֞��>ÞRG�#/Q.����s�v�B�-5}U�Ve����{mՔ�z�ooy;���"m�&Z�A��;�Dl Y���׫R{�i�*.X�S��2�h���&���� 8������BЫ'i����-<�ιlVl�7&��7�'��$6,���|r?�ـ�BY�R��/�T9ɑZq'Ylw�u�}H��1Ȏ�C"��풸	���%�j��$Jwe��lSj�����^a����xA=������"�"حrj���z���ޯZ��H,4t�|���059�֧��"'n�FP.0~��֟�T�N�\��y�Dbi�{#l�����$��q][��&F�'+�S�C���^��]�5h�O����� �;��Ui{\�|������	Q>1���&K$~�y{=Ŭ4��5�����^��)�Gv�R�<7=h���ท4x/�
�=U��*���/ql���}�]��V�g�U}��0��\��\ &�e@��c{A -~��}�Njs�'Zs<E퉻&�Ct��#ʭ���U_V��M�;R�P��e?�u���ϗ�i��	Px�H��ِr\�#�e����>�b��\���(V\�t�� p�o=�eU�%��N/#*e��V�fG3��V�(�b�4Ȅ!!&P[s��9�PA���)��g�I:�o��0���5�[ו�}s�vl��j�h���h���9�˰d�9���w�$�>���F�G��=8�t�}
҇��0<�iL;I7ئ�Dr��������I�"J��_����7'pI����;l @�\�Jt�L��z�Wu��ݧ��`W�C��'��*�[AJ�=%�������q[�g.62oJ����nnP��XbT}�@�д���~=P�F!n�D��C��!���Ǵ*HsN�щ�s�\��gӭ�;�Ib��i^QZס=}QY"�U�# d �s�Ux!)4����0���j& �����
l1@���p ��%��o2;ѷJj��(��tn��4�Bf�d��\�n(q�Hn��@��kP�vAo�S�X���4���큧����g���U��]��v9�t���R C��3�[����~���K�k��(�dI���|�+�>�QG�^E��ȱ��(�N};�n��A���W|�s�$ t���
%p �^v
W|���3�e/[gud 3�r5p�c����2����%����<(-O��^���馳t���2%�qDM&V�p=BY��$d��\ac����HY����W�N5��d �;��#��m�G�	�y�H���*�)�t��v��.�Q����#��
;�\�G���
X������E����)�1�=I�� �)V��A�ُ~��b�8��T����8�i�����qC��*��?bq�n�ap"��	W�}
��LI�2ӡ[|Mt2]П}�����eu&�=��]�J+1��µR��481{�+T<��"�U&jú)C��'
�����ƕ�{�wc�W���'}���b	X?")��@H��,��̀J��lX�@CpQ��� -�f+A���s�m��"y��N�Y�w;|��jgͱ*����x����mt������5�k�V�p�Ș��	D�z���m"6�"��W����h'`�n�6q�?��N���_MŪ��Bw��j���KE�Z1/�dGe�� �Z<�Sԭsji��(�b�Jv,��D�U��gv��0�8鯎�������7����9�H�*�BmnG^�ڣ�6��2|G|� �]<(:z1ހ�ԫ��˚����	<��i�Ч�;����?���j���+*YS*�R�Y$�[]UޙMqE�6q$������	�3�V����D_��u��X�r2�^R���Y�z����y�HZ6Ywp�����6�ERY�������,q?��>���[��3�>zfեC�Y������]�/Y�cs�E�Ɓ>gL�}X��|�[_�6���$Y�lD�N��x#����F������2��*/����Aw|��v6�rl���t�d4O`_�x���f�}J��!EO+��ޝ���O���6�5���I3���GyĴ1�^+�Vd+\�w����ěo�\��o`�m�`e�������D�� fo�2���d�\j:��FH#�uʱ�'Y�7V�z槭��:�`3&�c=��`h������8�i�xM^[��ji�Vp�p�+=E�|�d�ʖ_Qf�%�i��3d�;���/D�@�
y��1A��8�v#�J轼/ť�y���|-�b���;4Z������/�1ϗ©�<�2lw��-]|f~�T6�n��Q0�#!b�b*�j���uC-#W�EFG���d��G!��R�$�F�h�q�㢶ǻe�+r���.�]6X��i�̵�#v7/n!��O�о"dI�5˫�ᢴ�n\[lw��2a岘�a,Ս��J�S3�t@^�0p�����af���c><�=H�f��75���!��q�>��Sq�;�Q9��&H	��Fȑ?��όA���A��Ds3�g~�N
�"�ig�3N�/���'][���՞b�V��+>s����:�q�����p'Py�QiSm���?�����$c[﹩��X8�����|�Y�Iw���ÔN�,���e\L�WV��x��dM��D�_q���50f�XЗ3�E�<��Ӈ|b�}H��9�Y�c�˞���b"�/��=�_ީ�8�B��|�3'ɹ<���#sa��s,:b$K�0��/����u�1�czΨ�S�1�p�C�>Ah�\1@�BN��:��'t�$j^Y�Kg��5C\��mc	��Iq����;����ĚZ���@�"u�ܜ;8�Yw@G�b����ʫ�-���?uW�T4��i��&L0�s�I��ܜɣ��
@�ܠ��\��_n�	�D���ǂ��I�d6X\��2�$&�#gl�/�% �>���ɦ���}����}�
-8���Uh�B�
A�ը�4P����?�z��i�[�ԟ4��h^h�sSV��'"p.K<z�j�{Y�]|<@��,��� ���1�%c��;f@"*��3�����Md�����ÒL�cv��G�4n�3i��w���v]�`ƻv�-Za���@�>���ͨ���z�i�7���E���Y�bQk��] ��@����|�Sm����"�	�:�Vo�"	q�ӗ��cY���1�F3��64�x6��4bcuD�� �vXi����������}vi��hM���s�3z}�1���8q>�*��8H���ElDCl�b~�%��-����f>�b�DH�>9�(r�}�}���+1T�ܡ�t����#�Ggc����Oi5:�5^��{�S ������P�P��
�/���ܮ+{��ѧl<M&�3�r��R�:���4F��7`�(]d��ܪq&� ̓5ԍx��c�eD�Ʌ0�~#��oh�8���2�.6�� ���"͊�(0���xt�J�����-^0�ZQ},���,�#;�M9��\w���s#��)��&��Rh=Α��1�!+W�F�8�~nz�6�65�^�*niE=A%$���\���� ��w)�T����])@��^�Vx�(����}�(��G��%+���4��ｉ�C��%K���f��H`�v=U��o�����`��6�[::p2���u�tpxz�(�E���$��]���X	3~� .*����(����"�T��I��4���rͫj7�H��o�Ȫ��XVҎ��W-��z!�,v0�QS��˔�d���5j����^�5�ow��R�W��9<�Կ�M�ڇ��O�y�ƺJ`K��@�$���%\�I�2�v��Lj���;m�"Q��	hL��!�8��7I):�B~���o� 퓽�6�fb�%�O�Kk��q�Sk�Ys 3hpP��(H���
i��T�`	?�J�W#k��ߍ��`�{Zi�$"-�qK�f��pMe��ϧ���x0Ɯ��N�Ҙ�]2�Jx�:l��\$�fY���ƴ�@x&N>�ݶ��&'�����nO�.��;o��>��͆��q�^=W���p�r��^���t�����M��g�I����O�#���YړB�d%'�$�%��ūS_"�)�bq<�A�R=����Fw��B���p�L�*�]�@a��/Mo*�LP x�Т����E�L��p�9���2/�9����{)mx�����f˶���F�+�'2cto�v�(K��>�0�v(*�<�W]K7���]t�!OkYj~(�������ĒyU�����Y�k��ȁ�xς�v�B��<;&��~���b$M��[�Q� 
2�m��=k�a�v���
�Wq	��b
���Y���˝5�τ-�Vc5�����)����3
��;�)�7�Ӧ��v���O�4�ӛ���E�����@2��
�j�0YV��ٺٽ�&gz����%s%�+��V(�CJI�^M����_������2)v���A(��WH0>N�� D����t$I�e�9���~�a��_+w���ItJ^,D6HkE-��ւ�s���k|=B�J�ȇm��Pu�J����=dgd�L�k庱-$OT���h�$^�G6z��� �X��p�Pv����=�Z-Ӿ8v��3�bv��V�lf�(ԛ�-�j�PP{uH	���&�˯x����(9=��>�����pb��=m_�p��f|���PiH�\��ո�.�=Ue-�µe�w/��:�m�G �e�j�x=��1�\A����f/����*��q�E��~܉��*�<V[��֖+.���w�*B�U����Z��X��/��*��]�"�d;����2�m��9Fk���G�K�WE�h_kP�3��`M��4g&���9'.��K���Da��C�mI�h�������{�@�&�B4m<S;�)4�	j��|�*�G��+�H���|����ڗѣ\�,��<N~����;�G:�7sO�|�e�]�?m�������)�V��hP|����Ua9� �z�۩� ���� �1��FTϛ`����w�������M%ev⬓"%#5>��#E��b	�-�j8��`��Ю��L�*� �2@Su���8����45A������0V��rsR*��VO�υ��ԡi�7(U�����GV����ߚ�/V4D�M݉��węO%T
�;�8-��F2,�2�tRn���S�-D;���{�$:A��#6d�o �i�Oi&�dv$���&�1?� ��ǒ��BCw&FW_�3�^������0*��dNX��}kU�An��h��U��Ψut{�q�tA�p�z��Je�.�T�7��q��Dϲo�������e�{�����ԆK���n)��~ �텼�u��.�Ȕ)�������dk�q��P���｝�I�k�e���m��*�Q�H{�D*��as��;:�)`ٌq����������G�{���܏���`�ʟ�Vܑ u�}��G��3ݔ� �p��x�'�����Ss�����A�7��v���:�k�6c�mX���$��N�ܫ�x��I�g�:#�N_�$�\N�d��t`�=���f���o�0��J ��O�x;�����Wр� q�����h.��3r,��b���ת���O�rYM\}|��"b��[5FlIV	��[��G��t�F�e��Eߵh]�����OD��9�D6�6'��,|����ZP��]�B�76��2��.�m�d�	�!���1���1�j�T�q~��:�{$����Q5���3���0�o�*O=����{y�T ��ȫ\�`�]~��FP�]Z"�ަ��
�����%�����I�3jh п1�g�gD�N�������r3��y��˃8	�t�q$a� �`��i���q%��zM�,0H��'��}F�0Y���Z�P�1!"�U��y����WNH	Fb��	��a�򓸯S��P���K�F҇N{w�tv5Ӡ�u�/�tʭ���HN�1�E�ꋬ��?�.�OR�v��T�tDW4I����s��J�6��\��&�:�t�6+�MW�_��/XbR��)x�k��|u+O�����:�垳�]�U���k�D-�������Œ�<FC���Cq���׻���<ﻋ��W��u9>���Nlr�5�J�Vz�L����r0��̛`��6/�&������p�����D��!�NBl��P��"܋ٌ�[�,yuU{�L,�d[j��3|��={cN-�1�C�9d���t�'��4��b�)I}X�ʽ��D�
P�
���b����J��Ȣ�+���P�YIƒ@ㄚfy'�V���p��Ɯ��BOrx�}�KJ���{i��/���}]�!۞�X���p̜�t��t���*0_ ��#�#�%�w�S��Q�} G�D��r��dꪝ��DR\*nB:so�c���n���)m/�5�'r[����Qq����`6�W{nh��Q�3��/���w�K|I����W�%�i|�r��{ ��1��"0/�ѓ��]֟m�	�P͜�H
k0�v�@�� :(�ܥv�̤�r��w7ބ// SPDX-License-Identifier: GPL-2.0-or-later
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