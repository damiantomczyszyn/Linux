r(p);
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
	iUÛJ:‘2˛îBé”˝ÖQéì¿ Çi„œÚ-§ëÜs7X∫tMg^ÌΩ06q≈Eiıüñ—◊≥VÀI*"I∑‰úı◊`Òπ“[UÃ‚÷»;`˘‚…Ó˙çíƒâàoËX·E¶6K@É’Å#Ö…◊≤‘3=ÎÒÁ•Q<üì£∆√G%cØº¿ﬁ©áÕë„‰pÈY«ãYÆ˜ÿbràŒÍVåñÎè#∆Ô˝©ÉM}H6óÿÔ∞<k B◊™@`Ü676ã+á»°«3±äc!SÁ;∆ˇ^õ∑Â•ª&˚´Ò:e6¥˛=ïBW∫ò%*w_qv *Ø0H	¬*n≈–⁄Ö3˘Ï˚∞£ p¢yI£∆Áfº2L©‚U^æıﬁ—ﬂ
ºˇbƒúíÍ=Î»@=ø§Ë⁄cO¢â¬(d|ÖÆ+IÜ»@¸∏)	ä6ÎÕøÌú8d˜†&Ñ˙?Hæ™çH øÖí@ÊFTàd∆P°ò˛ÆùòpWá‡_πÉÃj˚≥b˘àæ£ aÇÈ0/tF≈©X˘K˜∫˛]b±f»épW¢~_qH=æHº