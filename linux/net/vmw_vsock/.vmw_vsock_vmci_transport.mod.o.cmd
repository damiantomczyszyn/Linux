FIG_SMP */

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
½vYq^Òì