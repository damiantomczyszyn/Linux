_do_set_cpus_allowed(p, cpumask_of(rq->cpu), SCA_MIGRATE_DISABLE);
}

void migrate_disable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled) {
		p->migration_disabled++;
		return;
	}

	preempt_disable();
	this_rq()->nr_pinned++;
	p->migration_disabled = 1;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_disable);

void migrate_enable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled > 1) {
		p->migration_disabled--;
		return;
	}

	if (WARN_ON_ONCE(!p->migration_disabled))
		return;

	/*
	 * Ensure stop_task runs either before or after this, and that
	 * __set_cpus_allowed_ptr(SCA_MIGRATE_ENABLE) doesn't schedule().
	 */
	preempt_disable();
	if (p->cpus_ptr != &p->cpus_mask)
		__set_cpus_allowed_ptr(p, &p->cpus_mask, SCA_MIGRATE_ENABLE);
	/*
	 * Mustn't clear migration_disabled() until cpus_ptr points back at the
	 * regular cpus_mask, otherwise things that race (eg.
	 * select_fallback_rq) get confused.
	 */
	barrier();
	p->migration_disabled = 0;
	this_rq()->nr_pinned--;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_enable);

static inline bool rq_has_pinned_tasks(struct rq *rq)
{
	return rq->nr_pinned;
}

/*
 * Per-CPU kthreads are allowed to run on !active && online CPUs, see
 * __set_cpus_allowed_ptr() and select_fallback_rq().
 */
static inline bool is_cpu_allowed(struct task_struct *p, int cpu)
{
	/* When not in the task's cpumask, no point in looking further. */
	if (!cpumask_test_cpu(cpu, p->cpus_ptr))
		return false;

	/* migrate_disabled() must be allowed to finish. */
	if (is_migration_disabled(p))
		return cpu_online(cpu);

	/* Non kernel threads are not allowed during either online or offline. */
	if (!(p->flags & PF_KTHREAD))
		return cpu_active(cpu) && task_cpu_possible(cpu, p);

	/* KTHREAD_IS_PER_CPU is always allowed. */
	if (kthread_is_per_cpu(p))
		return cpu_online(cpu);

	/* Regular kernel threads don't get to stay during offline. */
	if (cpu_dying(cpu))
		return false;

	/* But are allowed during online. */
	return cpu_online(cpu);
}

/*
 * This is how migration works:
 *
 * 1) we invoke migration_cpu_stop() on the target CPU using
 *    stop_one_cpu().
 * 2) stopper starts to run (implicitly forcing the migrated thread
 *    off the CPU)
 * 3) it checks whether the migrated task is still in the wrong runqueue.
 * 4) if it's in the wrong runqueue then the migration thread removes
 *    it and puts it into the right queue.
 * 5) stopper completes and stop_one_cpu() returns and the migration
 *    is done.
 */

/*
 * move_queued_task - move a queued task to new rq.
 *
 * Returns (locked) new rq. Old rq's lock is released.
 */
static struct rq *move_queued_task(struct rq *rq, struct rq_flags *rf,
				   struct task_struct *p, int new_cpu)
{
	lockdep_assert_rq_held(rq);

	deactivate_task(rq, p, DEQUEUE_NOCLOCK);
	set_task_cpu(p, new_cpu);
	rq_unlock(rq, rf);

	rq = cpu_rq(new_cpu);

	rq_lock(rq, rf);
	BUG_ON(task_cpu(p) != new_cpu);
	activate_task(rq, p, 0);
	check_preempt_curr(rq, p, 0);

	return rq;
}

struct migration_arg {
	struct task_struct		*task;
	int				dest_cpu;
	struct set_affinity_pending	*pending;
};

/*
 * @refs: number of wait_for_completion()
 * @stop_pending: is @stop_work in use
 */
struct set_affinity_pending {
	refcount_t		refs;
	unsigned int		stop_pending;
	struct completion	done;
	struct cpu_stop_work	stop_work;
	struct migration_arg	arg;
};

/*
 * Move (not current) task off this CPU, onto the destination CPU. We're doing
 * this because either it can't run here any more (set_cpus_allowed()
 * away from this CPU, or CPU going down), or because we're
 * attempting to rebalance this task on exec (sched_exec).
 *
 * So we race with normal scheduler movements, but that's OK, as long
 * as the task is no longer on this CPU.
 */
static struct rq *__migrate_task(struct rq *rq, struct rq_flags *rf,
				 struct task_struct *p, int dest_cpu)
{
	/* Affinity changed (again). */
	if (!is_cpu_allowed(p, dest_cpu))
		return rq;

	update_rq_clock(rq);
	rq = move_queued_task(rq, rf, p, dest_cpu);

	return rq;
}

/*
 * migration_cpu_stop - this will be executed by a highprio stopper thread
 * and performs thread migration by bumping thread off CPU then
 * 'pushing' onto another runqueue.
 */
static int migration_cpu_stop(void *data)
{
	struct migration_arg *arg = data;
	struct set_affinity_pending *pending = arg->pending;
	struct task_struct *p = arg->task;
	struct rq *rq = this_rq();
	bool complete = false;
	struct rq_flags rf;

	/*
	 * The original target CPU might have gone down and we might
	 * be on another CPU but it doesn't matter.
	 */
	local_irq_save(rf.flags);
	/*
	 * We need to explicitly wake pending tasks before running
	 * __migrate_task() such that we will not miss enforcing cpus_ptr
	 * during wakeups, see set_cpus_allowed_ptr()'s TASK_WAKING test.
	 */
	flush_smp_call_function_from_idle();

	raw_spin_lock(&p->pi_lock);
	rq_lock(rq, &rf);

	/*
	 * If we were passed a pending, then ->stop_pending was set, thus
	 * p->migration_pending must have remained stable.
	 */
	WARN_ON_ONCE(pending && pending != p->migration_pending);

	/*
	 * If task_rq(p) != rq, it cannot be migrated here, because we're
	 * holding rq->lock, if p->on_rq == 0 it cannot get enqueued because
	 * we're holding p->pi_lock.
	 */
	if (task_rq(p) == rq) {
		if (is_migration_disabled(p))
			goto out;

		if (pending) {
			p->migration_pending = NULL;
			complete = true;

			if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask))
				goto out;
		}

		if (task_on_rq_queued(p))
			rq = __migrate_task(rq, &rf, p, arg->dest_cpu);
		else
			p->wake_cpu = arg->dest_cpu;

		/*
		 * XXX __migrate_task() can fail, at which point we might end
		 * up running on a dodgy CPU, AFAICT this can only happen
		 * during CPU hotplug, at which point we'll get pushed out
		 * anyway, so it's probably not a big deal.
		 */

	} else if (pending) {
		/*
		 * This happens when we get migrated between migrate_enable()'s
		 * preempt_enable() and scheduling the stopper task. At that
		 * point we're a regular task again and not current anymore.
		 *
		 * A !PREEMPT kernel has a giant hole here, which makes it far
		 * more likely.
		 */

		/*
		 * The task moved before the stopper got to run. We're holding
		 * ->pi_lock, so the allowed mask is stable - if it got
		 * somewhere allowed, we're done.
		 */
		if (cpumask_test_cpu(task_cpu(p), p->cpus_ptr)) {
			p->migration_pending = NULL;
			complete = true;
			goto out;
		}

		/*
		 * When migrate_enable() hits a rq mis-match we can't reliably
		 * determine is_migration_disabled() and so have to chase after
		 * it.
		 */
		WARN_ON_ONCE(!pending->stop_pending);
		task_rq_unlock(rq, p, &rf);
		stop_one_cpu_nowait(task_cpu(p), migration_cpu_stop,
				    &pending->arg, &pending->stop_work);
		return 0;
	}
out:
	if (pending)
		pending->stop_pending = false;
	task_rq_unlock(rq, p, &rf);

	if (complete)
		complete_all(&pending->done);

	return 0;
}

int push_cpu_stop(void *arg)
{
	struct rq *lowest_rq = NULL, *rq = this_rq();
	struct task_struct *p = arg;

	raw_spin_lock_irq(&p->pi_lock);
	raw_spin_rq_lock(rq);

	if (task_rq(p) != rq)
		goto out_unlock;

	if (is_migration_disabled(p)) {
		p->migration_flags |= MDF_PUSH;
		goto out_unlock;
	}

	p->migration_flags &= ~MDF_PUSH;

	if (p->sched_class->find_lock_rq)
		lowest_rq = p->sched_class->find_lock_rq(p, rq);

	if (!lowest_rq)
		goto out_unlock;

	// XXX validate p is still the highest prio task
	if (task_rq(p) == rq) {
		deactivate_task(rq, p, 0);
		set_task_cpu(p, lowest_rq->cpu);
		activate_task(lowest_rq, p, 0);
		resched_curr(lowest_rq);
	}

	double_unlock_balance(rq, lowest_rq);

out_unlock:
	rq->push_busy = false;
	raw_spin_rq_unlock(rq);
	raw_spin_unlock_irq(&p->pi_lock);

	put_task_struct(p);
	return 0;
}

/*
 * sched_class::set_cpus_allowed must do the below, but is not required to
 * actually call this function.
 */
void set_cpus_allowed_common(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	if (flags & (SCA_MIGRATE_ENABLE | SCA_MIGRATE_DISABLE)) {
		p->cpus_ptr = new_mask;
		return;
	}

	cpumask_copy(&p->cpus_mask, new_mask);
	p->nr_cpus_allowed = cpumask_weight(new_mask);
}

static void
__do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	struct rq *rq = task_rq(p);
	bool queued, running;

	/*
	 * This here violates the locking rules for affinity, since we're only
	 * supposed to change these variables while holding both rq->lock and
	 * p->pi_lock.
	 *
	 * HOWEVER, it magically works, because ttwu() is the only code that
	 * accesses these variables under p->pi_lock and only does so after
	 * smp_cond_load_acquire(&p->on_cpu, !VAL), and we're in __schedule()
	 * before finish_task().
	 *
	 * XXX do further audits, this smells like something putrid.
	 */
	if (flags & SCA_MIGRATE_DISABLE)
		SCHED_WARN_ON(!p->on_cpu);
	else
		lockdep_assert_held(&p->pi_lock);

	queued = task_on_rq_queued(p);
	running = task_current(rq, p);

	if (queued) {
		/*
		 * Because __kthread_bind() calls this on blocked tasks without
		 * holding rq->lock.
		 */
		lockdep_assert_rq_held(rq);
		dequeue_task(rq, p, DEQUEUE_SAVE | DEQUEUE_NOCLOCK);
	}
	if (running)
		put_prev_task(rq, p);

	p->sched_class->set_cpus_allowed(p, new_mask, flags);

	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE | ENQUEUE_NOCLOCK);
	if (running)
		set_next_task(rq, p);
}

void do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask)
{
	__do_set_cpus_allowed(p, new_mask, 0);
}

int dup_user_cpus_ptr(struct task_struct *dst, struct task_struct *src,
		      int node)
{
	if (!src->user_cpus_ptr)
		return 0;

	dst->user_cpus_ptr = kmalloc_node(cpumask_size(), GFP_KERNEL, node);
	if (!dst->user_cpus_ptr)
		return -ENOMEM;

	cpumask_copy(dst->user_cpus_ptr, src->user_cpus_ptr);
	return 0;
}

static inline struct cpumask *clear_user_cpus_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = NULL;

	swap(p->user_cpus_ptr, user_mask);

	return user_mask;
}

void release_user_cpus_ptr(struct task_struct *p)
{
	kfree(clear_user_cpus_ptr(p));
}

/*
 * This function is wildly self concurrent; here be dragons.
 *
 *
 * When given a valid mask, __set_cpus_allowed_ptr() must block until the
 * designated task is enqueued on an allowed CPU. If that task is currently
 * running, we have to kick it out using the CPU stopper.
 *
 * Migrate-Disable comes along and tramples all over our nice sandcastle.
 * Consider:
 *
 *     Initial conditions: P0->cpus_mask = [0, 1]
 *
 *     P0@CPU0                  P1
 *
 *     migrate_disable();
 *     <preempted>
 *                              set_cpus_allowed_ptr(P0, [1]);
 *
 * P1 *cannot* return from this set_cpus_allowed_ptr() call until P0 executes
 * its outermost migrate_enable() (i.e. it exits its Migrate-Disable region).
 * This means we need the following scheme:
 *
 *     P0@CPU0                  P1
 *
 *     migrate_disable();
 *     <preempted>
 *                              set_cpus_allowed_ptr(P0, [1]);
 *                                <blocks>
 *     <resumes>
 *     migrate_enable();
 *       __set_cpus_allowed_ptr();
 *       <wakes local stopper>
 *                         `--> <woken on migration completion>
 *
 * Now the fun stuff: there may be several P1-like tasks, i.e. multiple
 * concurrent set_cpus_allowed_ptr(P0, [*]) calls. CPU affinity changes of any
 * task p are serialized by p->pi_lock, which we can leverage: the one that
 * should come into effect at the end of the Migrate-Disable region is the last
 * one. This means we only need to track a single cpumask (i.e. p->cpus_mask),
 * but we still need to properly signal those waiting tasks at the appropriate
 * moment.
 *
 * This is implemented using struct set_affinity_pending. The first
 * __set_cpus_allowed_ptr() caller within a given Migrate-Disable region will
 * setup an instance of that struct and install it on the targeted task_struct.
 * Any and all further callers will reuse that instance. Those then wait for
 * a completion signaled at the tail of the CPU stopper callback (1), triggered
 * on the end of the Migrate-Disable region (i.e. outermost migrate_enable()).
 *
 *
 * (1) In the cases covered above. There is one more where the completion is
 * signaled within affine_move_task() itself: when a subsequent affinity request
 * occurs after the stopper bailed out due to the targeted task still being
 * Migrate-Disable. Consider:
 *
 *     Initial conditions: P0->cpus_mask = [0, 1]
 *
 *     CPU0		  P1				P2
 *     <P0>
 *       migrate_disable();
 *       <preempted>
 *                        set_cpus_allowed_ptr(P0, [1]);
 *                          <blocks>
 *     <migration/0>
 *       migration_cpu_stop()
 *         is_migration_disabled()
 *           <bails>
 *                                                       set_cpus_allowed_ptr(P0, [0, 1]);
 *                                                         <signal completion>
 *                          <awakes>
 *
 * Note that the above is safe vs a concurrent migrate_enable(), as any
 * pending affinity completion is preceded by an uninstallation of
 * p->migration_pending done with p->pi_lock held.
 */
static int affine_move_task(struct rq *rq, struct task_struct *p, struct rq_flags *rf,
			    int dest_cpu, unsigned int flags)
{
	struct set_affinity_pending my_pending = { }, *pending = NULL;
	bool stop_pending, complete = false;

	/* Can the task run on the task's current CPU? If so, we're done */
	if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask)) {
		struct task_struct *push_task = NULL;

		if ((flags & SCA_MIGRATE_ENABLE) &&
		    (p->migration_flags & MDF_PUSH) && !rq->push_busy) {
			rq->push_busy = true;
			push_task = get_task_struct(p);
		}

		/*
		 * If there are pending waiters, but no pending stop_work,
		 * then complete now.
		 */
		pending = p->migration_pending;
		if (pending && !pending->stop_pending) {
			p->migration_pending = NULL;
			complete = true;
		}

		task_rq_unlock(rq, p, rf);

		if (push_task) {
			stop_one_cpu_nowait(rq->cpu, push_cpu_stop,
					    p, &rq->push_work);
		}

		if (complete)
			complete_all(&pending->done);

		return 0;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		/* serialized by p->pi_lock */
		if (!p->migration_pending) {
			/* Install the request */
			refcount_set(&my_pending.refs, 1);
			init_completion(&my_pending.done);
			my_pending.arg = (struct migration_arg) {
				.task = p,
				.dest_cpu = dest_cpu,
				.pending = &my_pending,
			};

			p->migration_pending = &my_pending;
		} else {
			pending = p->migration_pending;
			refcount_inc(&pending->refs);
			/*
			 * Affinity has changed, but we've already installed a
			 * pending. migration_cpu_stop() *must* see this, else
			 * we risk a completion of the pending despite having a
			 * task on a disallowed CPU.
			 *
			 * Serialized by p->pi_lock, so this is safe.
			 */
			pending->arg.dest_cpu = dest_cpu;
		}
	}
	pending = p->migration_pending;
	/*
	 * - !MIGRATE_ENABLE:
	 *   we'll have installed a pending if there wasn't one already.
	 *
	 * - MIGRATE_ENABLE:
	 *   we're here because the current CPU isn't matching anymore,
	 *   the only way that can happen is because of a concurrent
	 *   set_cpus_allowed_ptr() call, which should then still be
	 *   pending completion.
	 *
	 * Either way, we really should have a @pending here.
	 */
	if (WARN_ON_ONCE(!pending)) {
		task_rq_unlock(rq, p, rf);
		return -EINVAL;
	}

	if (task_running(rq, p) || READ_ONCE(p->__state) == TASK_WAKING) {
		/*
		 * MIGRATE_ENABLE gets here because 'p == current', but for
		 * anything else we cannot do is_migration_disabled(), punt
		 * and have the stopper function handle it all race-free.
		 */
		stop_pending = pending->stop_pending;
		if (!stop_pending)
			pending->stop_pending = true;

		if (flags & SCA_MIGRATE_ENABLE)
			p->migration_flags &= ~MDF_PUSH;

		task_rq_unlock(rq, p, rf);

		if (!stop_pending) {
			stop_one_cpu_nowait(cpu_of(rq), migration_cpu_stop,
					    &pending->arg, &pending->stop_work);
		}

		if (flags & SCA_MIGRATE_ENABLE)
			return 0;
	} else {

		if (!is_migration_disabled(p)) {
			if (task_on_rq_queued(p))
				rq = move_queued_task(rq, rf, p, dest_cpu);

			if (!pending->stop_pending) {
				p->migration_pending = NULL;
				complete = true;
			}
		}
		task_rq_unlock(rq, p, rf);

		if (complete)
			complete_all(&pending->done);
	}

	wait_for_completion(&pending->done);

	if (refcount_dec_and_test(&pending->refs))
		wake_up_var(&pending->refs); /* No UaF, just an address */

	/*
	 * Block the original owner of &pending until all subsequent callers
	 * have seen the completion and decremented the refcount
	 */
	wait_var_event(&my_pending.refs, !refcount_read(&my_pending.refs));

	/* ARGH */
	WARN_ON_ONCE(my_pending.stop_pending);

	return 0;
}

/*
 * Called with both p->pi_lock and rq->lock held; drops both before returning.
 */
static int __set_cpus_allowed_ptr_locked(struct task_struct *p,
					 const struct cpumask *new_mask,
					 u32 flags,
					 struct rq *rq,
					 struct rq_flags *rf)
	__releases(rq->lock)
	__releases(p->pi_lock)
{
	const struct cpumask *cpu_allowed_mask = task_cpu_possible_mask(p);
	const struct cpumask *cpu_valid_mask = cpu_active_mask;
	bool kthread = p->flags & PF_KTHREAD;
	struct cpumask *user_mask = NULL;
	unsigned int dest_cpu;
	int ret = 0;

	update_rq_clock(rq);

	if (kthread || is_migration_disabled(p)) {
		/*
		 * Kernel threads are allowed on online && !active CPUs,
		 * however, during cpu-hot-unplug, even these might get pushed
		 * away if not KTHREAD_IS_PER_CPU.
		 *
		 * Specifically, migration_disabled() tasks must not fail the
		 * cpumask_any_and_distribute() pick below, esp. so on
		 * SCA_MIGRATE_ENABLE, otherwise we'll not call
		 * set_cpus_allowed_common() and actually reset p->cpus_ptr.
		 */
		cpu_valid_mask = cpu_online_mask;
	}

	if (!kthread && !cpumask_subset(new_mask, cpu_allowed_mask)) {
		ret = -EINVAL;
		goto out;
	}

	/*
	 * Must re-check here, to close a race against __kthread_bind(),
	 * sched_setaffinity() is not guaranteed to observe the flag.
	 */
	if ((flags & SCA_CHECK) && (p->flags & PF_NO_SETAFFINITY)) {
		ret = -EINVAL;
		goto out;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		if (cpumask_equal(&p->cpus_mask, new_mask))
			goto out;

		if (WARN_ON_ONCE(p == current &&
				 is_migration_disabled(p) &&
				 !cpumask_test_cpu(task_cpu(p), new_mask))) {
			ret = -EBUSY;
			goto out;
		}
	}

	/*
	 * Picking a ~random cpu helps in cases where we are changing affinity
	 * for groups of tasks (ie. cpuset), so that load balancing is not
	 * immediately required to distribute the tasks within their new mask.
	 */
	dest_cpu = cpumask_any_and_distribute(cpu_valid_mask, new_mask);
	if (dest_cpu >= nr_cpu_ids) {
		ret = -EINVAL;
		goto out;
	}

	__do_set_cpus_allowed(p, new_mask, flags);

	if (flags & SCA_USER)
		user_mask = clear_user_cpus_ptr(p);

	ret = affine_move_task(rq, p, rf, dest_cpu, flags);

	kfree(user_mask);

	return ret;

out:
	task_rq_unlock(rq, p, rf);

	return ret;
}

/*
 * Change a given task's CPU affinity. Migrate the thread to a
 * proper CPU and schedule it away if the CPU it's executing on
 * is removed from the allowed bitmask.
 *
 * NOTE: the caller must have a valid reference to the task, the
 * task must not exit() & deallocate itself prematurely. The
 * call is not atomic; no spinlocks may be held.
 */
static int __set_cpus_allowed_ptr(struct task_struct *p,
				  const struct cpumask *new_mask, u32 flags)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(p, &rf);
	return __set_cpus_allowed_ptr_locked(p, new_mask, flags, rq, &rf);
}

int set_cpus_allowed_ptr(struct task_struct *p, const struct cpumask *new_mask)
{
	return __set_cpus_allowed_ptr(p, new_mask, 0);
}
EXPORT_SYMBOL_GPL(set_cpus_allowed_ptr);

/*
 * Change a given task's CPU affinity to the intersection of its current
 * affinity mask and @subset_mask, writing the resulting mask to @new_mask
 * and pointing @p->user_cpus_ptr to a copy of the old mask.
 * If the resulting mask is empty, leave the affinity unchanged and return
 * -EINVAL.
 */
static int restrict_cpus_allowed_ptr(struct task_struct *p,
				     struct cpumask *new_mask,
				     const struct cpumask *subset_mask)
{
	struct cpumask *user_mask = NULL;
	struct rq_flags rf;
	struct rq *rq;
	int err;

	if (!p->user_cpus_ptr) {
		user_mask = kmalloc(cpumask_size(), GFP_KERNEL);
		if (!user_mask)
			return -ENOMEM;
	}

	rq = task_rq_lock(p, &rf);

	/*
	 * Forcefully restricting the affinity of a deadline task is
	 * likely to cause problems, so fail and noisily override the
	 * mask entirely.
	 */
	if (task_has_dl_policy(p) && dl_bandwidth_enabled()) {
		err = -EPERM;
		goto err_unlock;
	}

	if (!cpumask_and(new_mask, &p->cpus_mask, subset_mask)) {
		err = -EINVAL;
		goto err_unlock;
	}

	/*
	 * We're about to butcher the task affinity, so keep track of what
	 * the user asked for in case we're able to restore it later on.
	 */
	if (user_mask) {
		cpumask_copy(user_mask, p->cpus_ptr);
		p->user_cpus_ptr = user_mask;
	}

	return __set_cpus_allowed_ptr_locked(p, new_mask, 0, rq, &rf);

err_unlock:
	task_rq_unlock(rq, p, &rf);
	kfree(user_mask);
	return err;
}

/*
 * Restrict the CPU affinity of task @p so that it is a subset of
 * task_cpu_possible_mask() and point @p->user_cpu_ptr to a copy of the
 * old affinity mask. If the resulting mask is empty, we warn and walk
 * up the cpuset hierarchy until we find a suitable mask.
 */
void force_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	cpumask_var_t new_mask;
	const struct cpumask *override_mask = task_cpu_possible_mask(p);

	alloc_cpumask_var(&new_mask, GFP_KERNEL);

	/*
	 * __migrate_task() can fail silently in the face of concurrent
	 * offlining of the chosen destination CPU, so take the hotplug
	 * lock to ensure that the migration succeeds.
	 */
	cpus_read_lock();
	if (!cpumask_available(new_mask))
		goto out_set_mask;

	if (!restrict_cpus_allowed_ptr(p, new_mask, override_mask))
		goto out_free_mask;

	/*
	 * We failed to find a valid subset of the affinity mask for the
	 * task, so override it based on its cpuset hierarchy.
	 */
	cpuset_cpus_allowed(p, new_mask);
	override_mask = new_mask;

out_set_mask:
	if (printk_ratelimit()) {
		printk_deferred("Overriding affinity for process %d (%s) to CPUs %*pbl\n",
				task_pid_nr(p), p->comm,
				cpumask_pr_args(override_mask));
	}

	WARN_ON(set_cpus_allowed_ptr(p, override_mask));
out_free_mask:
	cpus_read_unlock();
	free_cpumask_var(new_mask);
}

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask);

/*
 * Restore the affinity of a task @p which was previously restricted by a
 * call to force_compatible_cpus_allowed_ptr(). This will clear (and free)
 * @p->user_cpus_ptr.
 *
 * It is the caller's responsibility to serialise this with any calls to
 * force_compatible_cpus_allowed_ptr(@p).
 */
void relax_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = p->user_cpus_ptr;
	unsigned long flags;

	/*
	 * Try to restore the old affinity mask. If this fails, then
	 * we free the mask explicitly to avoid it being inherited across
	 * a subsequent fork().
	 */
	if (!user_mask || !__sched_setaffinity(p, user_mask))
		return;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	user_mask = clear_user_cpus_ptr(p);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

	kfree(user_mask);
}

void set_task_cpu(struct task_struct *p, unsigned int new_cpu)
{
#ifdef CONFIG_SCHED_DEBUG
	unsigned int state = READ_ONCE(p->__state);

	/*
	 * We should never call set_task_cpu() on a blocked task,
	 * ttwu() will sort out the placement.
	 */
	WARN_ON_ONCE(state != TASK_RUNNING && state != TASK_WAKING && !p->on_rq);

	/*
	 * Migrating fair class task must have p->on_rq = TASK_ON_RQ_MIGRATING,
	 * because schedstat_wait_{start,end} rebase migrating task's wait_start
	 * time relying on p->on_rq.
	 */
	WARN_ON_ONCE(state == TASK_RUNNING &&
		     p->sched_class == &fair_sched_class &&
		     (p->on_rq && !task_on_rq_migrating(p)));

#ifdef CONFIG_LOCKDEP
	/*
	 * The caller should hold either p->pi_lock or rq->lock, when changing
	 * a task's CPU. ->pi_lock for waking tasks, rq->lock for runnable tasks.
	 *
	 * sched_move_task() holds both and thus holding either pins the cgroup,
	 * see task_group().
	 *
	 * Furthermore, all task_rq users should acquire both locks, see
	 * task_rq_lock().
	 */
	WARN_ON_ONCE(debug_locks && !(lockdep_is_held(&p->pi_lock) ||
				      lockdep_is_held(__rq_lockp(task_rq(p)))));
#endif
	/*
	 * Clearly, migrati«èæ¬Üî>ŒĞ´_ÒˆÖ4–ÏÅœgl¦T²Øœh\
wùåı%&|²çí°ZÑYEİ,òíÕQémê¸'%-KhĞª¥zPêªí7óŠ‡L[ãlQåš^{rĞ¹P^ëıé—,ÔéŞ¯{Ê±´¯Ø«¡‘x³hÏ¸gA%çˆ=>¼ßpûGŞßhkùülòû”U—Oï-˜ŠÜS%ôáJßT®F¹3ÒnÉÍüÊ®LİdÕš`l²BÇ|jÿ×ßæ›ÔHvò˜ê/«^7ªÇÒv’v²¨¶s±€ÀÍAï†ÆåB;NŒ”#v¨Ş£‡tÿtÖêßu Gô–Q,YR)ß³½Ş%+¤—kÖ0dA})$áloÁ]ÌSlŸIÃ'+ Tdw’{‘VHèııiÖ¢å§Ë‡°©dAõì83ÜXREhBü†¥;ø¡cbÖ.^Ú?|h0uğPĞÁå@¨“Îaã³Ò1]N45à´ˆFkïb_<u¤ 1Âƒ1‚Htİ—.ü£oLÖÌæEÍï¤í°Ö Æ([œ¹èfÜ€¿ğƒ[½]I4ô,J–8oc ÀMBcüö“/<ÿ—;Mo¥hw ÔÉ6àiŞ»J/¶ñ	íñÕx†ó„ {•'}éù{SÅ%µ¢9Ü¯'v–çtÄ* Iü“Å*Ë7åÈ5Ş){„Xšñõ¯i¶’Ç,qà½ğzı4È5æ«L…Ç»«Z*ÅÖğË]óÖÁ@jXFµ.Rø:ùãöŒ}¹gâ£Õ¢¼hØÓÌì22ÛTøNÉÁrLa›ôõq³%[ÈÏÌ@ôå½ùˆƒÀsÉïó±äúæO WÓ2jÚl®Ü>òú
¤í†êñ|KäÌÿ¿¤©é)å†væ’'Eí\Ù#(Ø(ÿ9¥ÑWPmEuÔ"v‘,SõÜ5¢\æ£ÁBZğ6Y¯ì”%-l’
z›^ÏYÓD¦…e¢Õ,S"¿ ÊuYÏ£a5½Úª]u|¸zş	¯L±Íq^(©APô$ÇNıM±Ô¦‡¼XxHÌ*Ğ?‹¦X/-€ƒ”õş³³`Œ2&Á:h) l°D¿@Bî€¼‚k¯³ƒ2Á R[,.²ğîÇİp”îèÑíA^›Æ%Ôİ»ŒôEsŞ€ƒ¯á—T—Spß¾q¥\D}¡ñs—SAØÉ50Öke?Òqë<ÙóN0M¢NŒx­
¡,S~©*­”—ÓeOŠ‡™ÂÕøn’}¥Ê™ñÅ‹~w8û:´|Ë×›gÑ×%¦2½–8eÎkíëÖÑ/µ^‰Å i¢*zø~LSñ4\WÅ™W¥UöP< c‘wà™»¾­¥áNêŠ¯;ÍÚ½HLAû¡Ö‰ş"æ^V›’Šsßk¡KµW~Xµfœâ¡T«'>èª˜Vî‡Œ×oõ'<>`-¥²4XÔT;p`Òö§#+ÙaŒ$üo ºñ+®%4üSSî›à_qãŞò¾zù$.{§
.‹lQaÀS.,Æ.Sq]i\ƒï®ƒşµOX=x°ˆÔğ—>ıh™û~xô¬Èìq2b?p{w*ó¶A4œ0J,c}.2©.­MÄ\°A³³T2Æ ğ±9nhÎåîßz¨‘äÈrñRlÜÑ\Ñòã#ÿ²½ÀúÛ×JM9)šCïJø-ŞÑÍè# ”O2ÈH;¦OHŠ,Ğ°o9Ëg!Ï»ª˜O_d’24ˆç4-u+±æUËæCq:Uº5LdHÓ!·Ã	LÉÉj` 9ÄËøÛeÑÕ'ß¤Ën/ucU6Ã“RíoN~â
ºSøjÄ†Øø¢tˆÛµ9õ2›+^ê¥„Z§Au{é:¥böÚ»aP÷ÄŠöÚëÉeÓ”Ğa3²¢9—b†Tˆòú_7:f¯HŸ~é4„ÌËû+íaBÔxhÎ½«ƒS»òĞº¤LñPx»Xëb¡ÃÁöƒúç!ş%¡ı6SÍ–™€f®xÊ”x>6R½<±®s~–Ğµ”ıÆb<ue‚0öùVi4§Ò%‚=Óİ3ş1˜æ/¢§ĞtF¿ØaŞå`İ­+Áõ†nŠg H2€G Cê+(“v®IóÈ5/À¸¥î¡Óë6gg‹YÚ=«w÷WE·Cìğ`êí4s6¥Ú·Õğ¶£ğ‘òkrf”•ÈúÕ·á¶{p/ ˆ	S¿no°h¶û²Ñ¡†z³x&î*xÈæ£ÄùcgÒ›ÕG8¯è9#ùJ‡HöòÄf`cÌ°Ş¯á¶¾öÍ¢òG5¥%*5¦Øı=xÛÈ¶ªÔw/xz­\#‹éV–Ô†ß³QˆM¤n­oÂDûÔûÍ]xÁ°[%uL8‹mæ¢ìº÷¼§Š™¨#¬Dõ{é İOft­Õ«ŠDúqy‚g2Ì¥TğÙ/Ù¦1´Hä†–-ÆŠ…é/ÁşBÔ„¬Ó¥¤™¿F$¶XAéô»dJPÄ)åò¬ğø7îûb'ÅÏWU!Ş.•gİ)}«ÏÓ½ìèÚ‘{¥µ?¥¦[A®Êş]oE6ÕØ(ŞELv
“!ßn¼À÷„ıÃ”Ú{Éj çÖç}Â[ú¤‚¯>L3#£ëxÂİaÏ‹ÖvtÑkª>8ç!¬’ah2½s÷Ø,°—ÛpœC_ñî»GÆ2>!:‡YO_û2ÈÅ.ÑÄù¸5w’PO±Zlc5Ë/Om®Ä~¾ÁaËxOú„ãå–NŒİ[%Ac"I,Ü@Ü²¡…Æ¥W´¢Îë(Œ{l× H?@„B/zX+;N3úmwØ@=Ê²÷°$tı }-³’“Nšï½†àHlPâ#Í†°^,!ëô”ƒ•×V‹N/ı–ÕYŒrË<ÍuÜÚú7G ¹u)rS#›†8ìS65gRŸÅWc_Tï2Ér”.Ø„/¬©^U}å2dÇ¦JÂ[óŞ—üÆH	>Ê¦L}Ï:•–±Ì8Õ†œ%€ë
E˜4j—É´zA¥+Ğ™…&}U>Õ´~Ÿ3w×gÿÕÀoÎá}ãîè|ê8àc
Şœ8ÈNAè¯ñş&jËµÃt}@›âŠMó0;úÉ‚§™(‰¹‹s§Õ	ªTˆ7]êš&6kIÄö¬	[R«(ò˜ÇÃ°‡&|sõZî±lVÈÏd,ê°e¤¬õ°ªv ŒcI@û*ÆáøCG 9şúÓH7˜m”…ò!)Œ‰ÔüvoYåiÎ»_n…»_NChşŞ1c-Q"G+œ8Ñ4HgğpfVÊKµäŒèá³hÜ1•Ùl3ÜJ~YØ¸ûEİéÊ7u#®uXã¨ÍíÕ¶W©mÑ¸e6ÅR_¤¤$­‘âíÒ%ºNv+Ş¼Š¿í+‘Ü/s1&*k™ş²—W2
$îïS©TmÑ!Ó¸<vI'2«ÒÊ[5–šÊºä6²YƒÂu™¤Qy,—7Ø*µD”é¥øGá¥·:î‹æ:5XC×ŸSÇx­Zdeğ¿ğ
¥aÛõ"73ĞW
ÿtØp
&m°ïçõÅ4RØ–>«e<§®‹µLıDøë„Ş”Ö=Ù÷ƒğ
Ãx$».jIxRUYT{BòCz;ïŞ}
&ŠËb`ÚÀ"gJií¼­µ>]?vŠ­ê¶ÃÎ)î ~tXR"z«´·í 5'å%Óí1›Çñ‚ {À”rÛöu‚:[ğR
áé#Ãì†œ2w=mÃ‡Ü€`ygHÕxãhf£×@•ea˜Ú—)‡{]"ƒtÂĞJÒö-àÅæ‹å°’ŞòE—¿ğ6¥‘)
M”ãk‡Qq*>ÇGŒökâNFÇ›Mœ‹ŒÒbh³7N{î~–W8ïÓ”¼8ÅĞËR”_¡o›n‰ÃÀUÑşÒg¯B:{¾–l*»éâÑMüãzÅ{Çn…ç{`ÑKiµ]P+ĞáÖ[&ûCn—kNcÎD>‹;öA^*h‹âæ˜¨õMÈ”MAÙà
fQgü¬v hĞ–¥†0s;[I¹;[fæŞ„ÿ±®.$…„&ARµ•¤t‡-[ÂRãyÖ´PéûÜ¾¸®6(âd(´3Ï’”yH6H,VôÉÚè¼ç‘M%MWœ¡)¢ªCşMXâ‚†n7äÖ5ÒïvsÍiaG}ş-WÄ™ÎŞÌ[|°ä¤!†îñ£ d}õ)ŞödÚöo©µg°à€ö•ƒ[[!f/÷g©]Í:'&S‘ÕıT%õ†9í¸°}r¥YØ2¯ç½T1ƒBóiSŞŒ(\î×Î·îzgÍú¾Mø[¤)8óC¤‚ MnNÿó°R’W‘Hƒ	ˆ²ŸññŞÀıp´U-©Ç«Y^*+”¢¯	å{¥†8Ò)‘Şœ¡pÚL¼ñy6Tæê²Å—Öƒª],m2E—½FS™k·
”mˆØÚêHÂJ‡o…2h
Da_f”©½ïåÌô\WOd+¹;[ı1¹7]˜¡ÌF‘€Ô÷©Ã"Hº/PÃWwjë!–WŠ†*0ı¯hâŞ(W±H»pZ¼~.®®‰y>ÙAŞºmOô#™7á=i¸•À€*=ÚFFæ(#Äæ¹œDVcsœ]†¨a7	›
`k¨LŸûâaıÒü ,Ï}¯óTÚUçÚ®rWb7,¦…p6ö‘.8œ÷N]Ç‹;(‚'è{JÖş(gÿ4¬âB7#ë*(§^ãXb#2BŒ²‘ã¼¯«ÿ´+ˆ‘g[U;ªœçc£ú¢©po8ŸWz¾¹vçN]ş½qYádÂ¥™6m¯{¥aIyµœLu,fvJİ«(ø\ÆšùWk;4»»rÿÚ$G;€K™Ï|
k$à±œáH™Èeà±ÙÅ8ˆqgªô·9Æµ¼3“m±„*EÃÈA° XéØ=Å®µ]“7ª$¬ÌèDÏÃÍ8	]ŸÏãä7…ğ.b¯†Î‰“°l­ÖÔl¬­ëáJ

P r­ë]¶GÂÔÊĞ1ÌĞ¹Ğp¶™yY=XY@V2:<øĞ3ë*êĞZî´¢ôK%éx6CFO&¼´¶\(ÓxKƒ˜KéÃ™Å†¯ÛÜ’‚ÀıÂğ¨öÿ·„ |	ŞÖĞáy›×~®å©·«š‹“Qzíãk…BY¯„Y–€¿…Ùhï©­ï&%Sh"÷
h4’“N#Â‘0N9gâjäÇ­'oê‰ê‰GfôfŠÜÍÄfÌj`Kh™5Ö**À´|hñÙšßj61ËşÛsVÏ„ı@™_kıô!`0.‡èªQÇ-é@¢)#dê-<œªù×Äœ=&ôä¸šiĞ¸:vœ²€Óç1¿%¤(Ø'æùÛx‚üòfÆßa§äSo:ÅÄ®÷
îÿÊ!¼2¯‰†úÕ~Pm1¿ÕûÕÇ[E!C«·<3×?E®¡c´$#¸ÃbfeGgÆş÷ˆ°ÍHÏ|ªâ»ãñ.ÀrU9µ•­ï½³ZÁşÏbå´ã4ñqà+å7èÀÍ³pQS,˜ŒŸ°£¡£ŠÛ~¡÷¶±×g¿U­YR’%Ğßk#¬üê˜Ï!RI(V?Ø=rO<XXTZÑ
^ê`c&h(¡Í°åRzZùvĞA0eÚİ‘ÜgâÙ8?ÈÊ1¢C3VÈoÛ,! ¦H4å¯^ha ±:r´#•kæúdü^ñÊUÃ2L²¥[ú—á‚üw#hƒ?¸({™$9(»hKÏpdBÅƒG’6˜*	å1ì÷5«õÙ«F(;ÄB2Â²5ÕÅ®+cÂ†îæXı/315°*y†Ò!æz­3Z0ÚÇ8xR/¼êùy
h©O³ÕÌk)¢½l<‘EÇ¤o@#vWÓim%ûq3KLRŸ|_vV3j(Ÿ¶ùúù¹XFò°{´L¹VØ¾kÃÆÁ¡ŒàFÑ!gheKIx³<<{»³‰ÇPŸZÍçYS¥/XÇ£A>íTó˜ÆàİŒ ñúùê±WÓÚØÊ-â}É›ŠqlIòêÎŞğNí²-LE¥óHW_¸IŠİuæI•·|ÈbÏWL44êˆHV
$¹U¦¾ı¾Qéùtİß‡X¼iìÁ§#àÙ2{”ûögc.ş=™I¢âgÃÀ÷)gŸ\¢à
röã¦t·7 S«MwÒ4¾mRìÂûb÷t3Ç…Râj”._+6­ŒKOMàyğ“c@º›Ñs0V¿ˆ:>ƒ ƒÕ¸á¢¥áåQ» ÎDŒ„'3<® |? ÏÁ_?Ş’âïM¸³³BÆ]L­r&äëií‡æYnq°<Ueª§h}­éè£¼µí#`å>şEuF¬Ò+Pf,„ıß”Kuò8±Hc—İÔ®qƒ^@	Y/’Ÿêq¤Å$íS·ŠÁx¡ÄÌSíp
ÒÙTÁ ¾Ëìe»[l¯G0cïQpn¯Wfkm[Êı=1ÂË…Gm&z–ìër•ª†_Øc¹ !ÄúÿÁ“¬ËÀ<ï8]­Œ=œÄyœˆ†úd=n’±¯Rœ7¿qCù6•Sá~ãbÚçW«tŸC´ä2	·åuëƒ4çÕ¶\2"2ªrşßïDøœîÈ=xlØmu[‘•º@¹İ¬8Ê´Ÿ4&c
Ytå5ÂSÈÄDgÌ‡ü&@änÓìJöì?nkúŸm´[÷UÈÆVè
%{–Ş¸K{ê¯™€xÄUší®‹€ùTéX ÅDmÄÃ±ßkƒ&§@Ö;?„‘õ’pğEs‰ÚPÆ6~M}M½ FÚ¶éÄC´Bì/AHã2«ÔdŠ%øXS6pUUï¶¬ÅDâ#>Üm)•fîÉùÒfV¿ÉP‚6R˜pşàát—Ô
¾Í? ,E˜åø‰œÜùKßÜmÁ1ÛŸ¯ª7kêZ³ˆSvôóxèB‘Ç	yÏ*4hœµHR]¼y1U·Uc36Ó€Sğ÷îºIP:œ{Û[¡ş€_éË9ÌÎiq²ß“r9Jp,ÇD˜g½#Õ‘vMÓwÑ8ùd7hŞÈW¸­÷o§'³åí©L…Ôú¡¦¨ÓÚÜŠÙœ»—Ê=o…Iÿ£ğÎ]Nâ¶î]#`1ïW¶ÅÊEì•|ì†É’m«}ê
¶Ù,N·u‹ğŠ¤Tƒèeò×ÂñÏâië`EóÂ…³Î_’ôfÅwp»²p)Xøh§äSZ¬¨áı¯ôÖŸâ?šÊAƒÈ°0E×g0\ßİ• é”ÕŸÛh¤TÕæ<TÓWŸ¬PZ ãuÃ.'4d¢QÆ{½Ù‚"Ì“Mlå†¡^´-`Å//âÌâM¹ÉÁ©ómJsˆÌ\‹óÍuáQšW¤ô¬ûì>€Çx%ta{ÄéDÔPQÍÛÄ²âÔúæ’'ó Ë&NÅ#]¤ 2Jz²THªäe®Jé<·êˆtBÏS²aky -Své·CsÓ´°b!­'äén¤v”QĞCR‹.ì [˜…¨ËX/É=ğØ13y¿bNä˜šÿÄlšñßì”8	·g“•±µCœ
øşHñm5úµ*ŒÃË>õSV´¿åÈ¸6‰Q—ŒV‚L|ù2á~¡–p¤È£÷ŸKD1x¤¨‚,68ŠZf­72 }2ßT×âs_¬»’¬vJÍPâÌ4¼"–k6˜Ç,6ÙdropË´¸¯Dş7B4ª\¢Éİ¸ bn'´şµ!’›&¨Ag°kHßÓ*xa]51UÚ8ÂÜ‹İÊƒ¿·RïÄ¬"õÑ„GøÖ—k@c=fL™Ÿ\Jü³w8®ûÖÖän /N–÷ÙLÕrÊé?¦ªY%’ÕÒR"‘ÊÖ‰Ò¯IüI­ÔR¶-£ë…L#·ŠÔ‡ªéOÂ@„;]™›±(YUÜµ­âYôšÉ”2‚·aÂ‰ïû0ÂcóŸß\rDºUøŸ÷§Ú¾ÉÈ{«æ3€«Û%3ÿİ¶$QmÒ¬òSå‹º5—i°È™ì(‰)8¥ŞIb½g,÷4#¹)ä}hÖ1´Bÿ‹âëCïÃ³.ôœ?–À°ŞÕâhÆ’æmœÙêDÎr˜Î
)Äs¼’SF?›ô1ÎgÇAi±R>”TŞ,ÛºBxy°t³LÊÃ1ÿ£È /ËÀ.¼ø+“Şõ_TŞ²²w2"¤ñ¸õ¤wg+BjÚïÙlj‹¦^%º§—¡Z6$Ôş¢E}î+äFÌÙº€a+†¾Â¡hG/!üaI ÃIì—®«ñtX‡±}nl  pa—š:Áï–·JñF/íˆ¶ãH.÷lÅÍ@§aËŸ¾îi­e·dk˜
qO”İÃÑGÆ'&±›â(@ò=èQP×¬A†û©˜ÌÎì³xfâÙÎ°a İí/2Xõ;ãFHÓ8&¦y¸“{,ïCê ¤ê¿)úÜ+é'ÍàE.iğÂ¾ú˜Tî)9‹œ™xÕñ¬?F#*LÄ#Â\1¥‹×	µ^Ò¨Ûë ß_ç”“Î0ÜãDÕ’¿‹É_Gé‡uà+Ø`—Í‰aäæ(1¬j´„)AYùXSäò|R±	Ÿ¯6R“aG7„*?â2Œs¶ÊYøTùşnw‡ÎògvºØúàˆ™¿>ˆI]°±(0{¹€>æø…ÊVeóâ!J„­Ïñ‹DáI±Ê½ Ót&²at˜ƒ7dr¬~¸íeüµÚ­¶~4ÁŒ½ƒxY0,tTÂ¬™T¡;:íœü>Ü!ˆŒ*´Œÿ ¿øI ¿‹æ-4j)rÁ¶}Ëò±{ÿ4#ÆàBÚµv<·sÒjØ²ıÏÿì ô
Töt'"Çn=ObÈœ÷Arü«-xÙ¸íh’ÏÓ­ë-QVHçW[qzÍP°éèâgÖÎË[ì±¥ŒÊ5;?Í‚€h³jãé:%~¢$3š)UHtƒÅSìhç(Íø¢7•=m-GÚYÃÖ²4ö9µˆ¡$1ä &ı–•‹yFIx}ÁÁO<+§>	ã®uá(vìü”ËcvG.ç*ŒöÒHVä‹¢Øçhµ9&ÛÒ×ç!@v\åTai?Ÿ¯®ô“pkvŸv,AµxFSŞÿ%Ú¦×O
ïYwn [·ÎËQ¿¤{ÙÃvË´g=õüîO1Rû„6Gnc²	¡ï¸pCxÂÕû{Çğ›±¼ÓÒUb)tƒN8ĞÍˆÉgoÎ@¨ä#’Öh]€ï=Ñ¢¨ÈèCCëE	ŒªzWa¶7¦*“TŒ÷ƒ3Ê_ÉZ6.ûœâg€Y=
¿¼†É<T?œ&étªvxå¹–e!kó¢	¨£4Iå½Ñ—ÕA¤e2€!M…Pg§•òÖ„ÍzçÍl>ùJ³/¿^¦_ñ+Jüké!«ˆ†Z:îÒ–õÛ¤ñ”ÌŠg/¶q	
HkÖ¾ñFÿ ·şåï;b†©¸ôwO‚Ú£:!pDÎ¹=x±nc7İ—šÌ!Ÿáy¹f.Ûí*Éšrs•-r¸Ø	¸Õ³q‡À©éF‡.ğ¸u˜vpz|ßp>cñx €ÉDÒ½/WdGÖOÖÏ®g`Aè0è‚}@P­s2w$<ğ¶|Ÿè@J¼²t†p©òO… x+0¥ÑùíÁ¸áL¿>&  sĞûË3¶€J×gwÁ\øòN+¾vöÍ…ÃLw€„Ş¹Ñì^÷[h¯òEFÿ@S'Ù"Hö¬İ¯Íe_5‡`^¼ŠS9Y°ÈºÒù):Øƒq§O#ÿê6ÀİYÎâÏ	Ãç™yÀŠ‡qì“ÜoîŞ(å3Tğ´C_\:Zå‹Ş­EîéÏØyÑZ©¶•Ñ¯ò`RÉ5°ÿW/¶ã¢¾J&G(Öá
„ÄJC)•Ò:TINßĞİĞ€W¦¼HìæË­@Ã¹M…‚N5B,q‘M›ø¹8 ¹¹©²0Ê?¨VV½³8İ2³	rımâ§ĞÆƒ^k£ğ† ŸJ6ÜÌF˜ê$ÖqtÙë÷Õô–>K,?I+è¥ı»óÔ‰€®:ÓV>N7r­j Ò™İˆ£Hkä¯tV5'ãÚFĞ©.Î ›[Ø1'ĞÓüÉû·r™ÂÅÇùQ§võ´pì¼g=+Ù’§VD¿—¨Oğ*öUñÛÅ!/>b2å82ÊÜoA<üh#à¾ç¨WX•PL¨×OîA%ï»]=«ëZÆ0+NVx¼Î¨E¸í¤µGu™A¬NÕuŸeYüQ£Ğ«Ù]‹£†ß˜¬¾ÚƒÊíAaÜNán€5Å¦¢½&³û>·q§µ‹µí‘:ê&¦½®^-sQäU`Dájì/P£½TÎÊÉİ™µÕA*œ¥åo¯§¦”„ï/úîÆâB¤FĞo%ÑOò@ç¤ôz–šbÜ…0 9ÂWeŠ«±7ñ;B÷ø¦ö.*-3»%
ç °¨»“Šù“(ë|
çƒG{5«Æß£øg¯Î›€.F*,™Ö˜)7_=?¹«¼÷	Kßß"ûÑ¾;G¦F…[^-o•æt¿ fÜÈS†U`édrMÇ9uKâoĞˆÌó(	O/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/pci.h>
#include <linux/i2c.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ctrls.h>
#include <media/tuner.h>
#include <media/tveeprom.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dvb.h>
#include <media/rc-core.h>

#include "cx23885-reg.h"
#include "media/drv-intf/cx2341x.h"

#include <linux/mutex.h>

#define CX23885_VERSION "0.0.4"

#define UNSET (-1U)

#define CX23885_MAXBOARDS 8

/* Max number of inputs by card */
#define MAX_CX23885_INPUT 8
#define INPUT(nr) (&cx23885_boards[dev->board].input[nr])

#define BUFFER_TIMEOUT     (HZ)  /* 0.5 seconds */

#define CX23885_BOARD_NOAUTO               UNSET
#define CX23885_BOARD_UNKNOWN                  0
#define CX23885_BOARD_HAUPPAUGE_HVR1800lp      1
#define CX23885_BOARD_HAUPPAUGE_HVR1800        2
#define CX23885_BOARD_HAUPPAUGE_HVR1250        3
#define CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP   4
#define CX23885_BOARD_HAUPPAUGE_HVR1500Q       5
#define CX23885_BOARD_HAUPPAUGE_HVR1500        6
#define CX23885_BOARD_HAUPPAUGE_HVR1200        7
#define CX23885_BOARD_HAUPPAUGE_HVR1700        8
#define CX23885_BOARD_HAUPPAUGE_HVR1400        9
#define CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP 10
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP 11
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H 12
#define CX23885_BOARD_COMPRO_VIDEOMATE_E650F   13
#define CX23885_BOARD_TBS_6920                 14
#define CX23885_BOARD_TEVII_S470               15
#define CX23885_BOARD_DVBWORLD_2005            16
#define CX23885_BOARD_NETUP_DUAL_DVBS2_CI      17
#define CX23885_BOARD_HAUPPAUGE_HVR1270        18
#define CX23885_BOARD_HAUPPAUGE_HVR1275        19
#define CX23885_BOARD_HAUPPAUGE_HVR1255        20
#define CX23885_BOARD_HAUPPAUGE_HVR1210        21
#define CX23885_BOARD_MYGICA_X8506             22
#define CX23885_BOARD_MAGICPRO_PROHDTVE2       23
#define CX23885_BOARD_HAUPPAUGE_HVR1850        24
#define CX23885_BOARD_COMPRO_VIDEOMATE_E800    25
#define CX23885_BOARD_HAUPPAUGE_HVR1290        26
#define CX23885_BOARD_MYGICA_X8558PRO          27
#define CX23885_BOARD_LEADTEK_WINFAST_PXTV1200 28
#define CX23885_BOARD_GOTVIEW_X5_3D_HYBRID     29
#define CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF 30
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000 31
#define CX23885_BOARD_MPX885                   32
#define CX23885_BOARD_MYGICA_X8507             33
#define CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL 34
#define CX23885_BOARD_TEVII_S471               35
#define CX23885_BOARD_HAUPPAUGE_HVR1255_22111  36
#define CX23885_BOARD_PROF_8000                37
#define CX23885_BOARD_HAUPPAUGE_HVR4400        38
#define CX23885_BOARD_AVERMEDIA_HC81R          39
#define CX23885_BOARD_TBS_6981                 40
#define CX23885_BOARD_TBS_6980                 41
#define CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200 42
#define CX23885_BOARD_HAUPPAUGE_IMPACTVCBE     43
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2 44
#define CX23885_BOARD_DVBSKY_T9580             45
#define CX23885_BOARD_DVBSKY_T980C             46
#define CX23885_BOARD_DVBSKY_S950C             47
#define CX23885_BOARD_TT_CT2_4500_CI           48
#define CX23885_BOARD_DVBSKY_S950              49
#define CX23885_BOARD_DVBSKY_S952              50
#define CX23885_BOARD_DVBSKY_T982              51
#define CX23885_BOARD_HAUPPAUGE_HVR5525        52
#define CX23885_BOARD_HAUPPAUGE_STARBURST      53
#define CX23885_BOARD_VIEWCAST_260E            54
#define CX23885_BOARD_VIEWCAST_460E            55
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB     56
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC    57
#define CX23885_BOARD_HAUPPAUGE_HVR1265_K4     58
#define CX23885_BOARD_HAUPPAUGE_STARBURST2     59
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885 60
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885 61
#define CX23885_BOARD_AVERMEDIA_CE310B         62

#define GPIO_0 0x00000001
#define GPIO_1 0x00000002
#define GPIO_2 0x00000004
#define GPIO_3 0x00000008
#define GPIO_4 0x00000010
#define GPIO_5 0x00000020
#define GPIO_6 0x00000040
#define GPIO_7 0x00000080
#define GPIO_8 0x00000100
#define GPIO_9 0x00000200
#define GPIO_10 0x00000400
#define GPIO_11 0x00000800
#define GPIO_12 0x00001000
#define GPIO_13 0x00002000
#define GPIO_14 0x00004000
#define GPIO_15 0x00008000

/* Currently unsupported by the driver: PAL/H, NTSC/Kr, SECAM B/G/H/LC */
#define CX23885_NORMS (\
	V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_443 | \
	V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK    |  V4L2_STD_PAL_I    | \
	V4L2_STD_PAL_M  |  V4L2_STD_PAL_N     |  V4L2_STD_PAL_Nc   | \
	V4L2_STD_PAL_60 |  V4L2_STD_SECAM_L   |  V4L2_STD_SECAM_DK)

struct cx23885_fmt {
	u32   fourcc;          /* v4l2 format id */
	int   depth;
	int   flags;
	u32   cxformat;
};

struct cx23885_tvnorm {
	char		*name;
	v4l2_std_id	id;
	u32		cxiformat;
	u32		cxoformat;
};

enum cx23885_itype {
	CX23885_VMUX_COMPOSITE1 = 1,
	CX23885_VMUX_COMPOSITE2,
	CX23885_VMUX_COMPOSITE3,
	CX23885_VMUX_COMPOSITE4,
	CX23885_VMUX_SVIDEO,
	CX23885_VMUX_COMPONENT,
	CX23885_VMUX_TELEVISION,
	CX23885_VMUX_CABLE,
	CX23885_VMUX_DVB,
	CX23885_VMUX_DEBUG,
	CX23885_RADIO,
};

enum cx23885_src_sel_type {
	CX23885_SRC_SEL_EXT_656_VIDEO = 0,
	CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO
};

struct cx23885_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

/* buffer for one video frame */
struct cx23885_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head queue;

	/* cx23885 specific */
	unsigned int           bpl;
	struct cx23885_riscmem risc;
	struct cx23885_fmt     *fmt;
	u32                    count;
};

struct cx23885_input {
	enum cx23885_itype type;
	unsigned int    vmux;
	unsigned int    amux;
	u32             gpio0, gpio1, gpio2, gpio3;
};

typedef enum {
	CX23885_MPEG_UNDEFINED = 0,
	CX23885_MPEG_DVB,
	CX23885_ANALOG_VIDEO,
	CX23885_MPEG_ENCODER,
} port_t;

struct cx23885_board {
	char                    *name;
	port_t			porta, portb, portc;
	int		num_fds_portb, num_fds_portc;
	unsigned int		tuner_type;
	unsigned int		radio_type;
	unsigned char		tuner_addr;
	unsigned char		radio_addr;
	unsigned int		tuner_bus;

	/* Vendors can and do run the PCIe bridge at different
	 * clock rates, driven physically by crystals on the PCBs.
	 * The core has to accommodate this. This allows the user
	 * to add new boards with new frequencys. The value is
	 * expressed in Hz.
	 *
	 * The core framework will default this value based on
	 * current designs, but it can vary.
	 */
	u32			clk_freq;
	struct cx23885_input    input[MAX_CX23885_INPUT];
	int			ci_type; /* for NetUP */
	/* Force bottom field first during DMA (888 workaround) */
	u32                     force_bff;
};

struct cx23885_subid {
	u16     subvendor;
	u16     subdevice;
	u32     card;
};

struct cx23885_i2c {
	struct cx23885_dev *dev;

	int                        nr;

	/* i2c i/o */
	struct i2c_adapter         i2c_adap;
	struct i2c_client          i2c_client;
	u32                        i2c_rc;

	/* 885 registers used for raw address */
	u32                        i2c_period;
	u32                        reg_ctrl;
	u32                        reg_stat;
	u32                        reg_addr;
	u32                        reg_rdata;
	u32                        reg_wdata;
};

struct cx23885_dmaqueue {
	struct list_head       active;
	u32                    count;
};

struct cx23885_tsport {
	struct cx23885_dev *dev;

	unsigned                   nr;
	int                        sram_chno;

	struct vb2_dvb_frontends   frontends;

	/* dma queues */
	struct cx23885_dmaqueue    mpegq;
	u32                        ts_packet_size;
	u32                        ts_packet_count;

	int                        width;
	int                        height;

	spinlock_t                 slock;

	/* registers */
	u32                        reg_gpcnt;
	u32                        reg_gpcnt_ctl;
	u32                        reg_dma_ctl;
	u32                        reg_lngth;
	u32                        reg_hw_sop_ctrl;
	u32                        reg_gen_ctrl;
	u32                        reg_bd_pkt_status;
	u32                        reg_sop_status;
	u32                        reg_fifo_ovfl_stat;
	u32                        reg_vld_misc;
	u32                        reg_ts_clk_en;
	u32                        reg_ts_int_msk;
	u32                        reg_ts_int_stat;
	u32                        reg_src_sel;

	/* Default register vals */
	int                        pci_irqmask;
	u32                        dma_ctl_val;
	u32                        ts_int_msk_val;
	u32                        gen_ctrl_val;
	u32                        ts_clk_en_val;
	u32                        src_sel_val;
	u32                        vld_misc_val;
	u32                        hw_sop_ctrl_val;

	/* Allow a single tsport to have multiple frontends */
	u32                        num_frontends;
	void                (*gate_ctrl)(struct cx23885_tsport *port, int open);
	void                       *port_priv;

	/* Workaround for a temp dvb_frontend that the tuner can attached to */
	struct dvb_frontend analog_fe;

	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	struct i2c_client *i2c_client_sec;
	struct i2c_client *i2c_client_ci;

	int (*set_frontend)(struct dvb_frontend *fe);
	int (*fe_set_voltage)(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage);
};

struct cx23885_kernel_ir {
	struct cx23885_dev	*cx;
	char			*name;
	char			*phys;

	struct rc_dev		*rc;
};

struct cx23885_audio_buffer {
	unsigned int		bpl;
	struct cx23885_riscmem	risc;
	void			*vaddr;
	struct scatterlist	*sglist;
	int			sglen;
	unsigned long		nr_pages;
};

struct cx23885_audio_dev {
	struct cx23885_dev	*dev;

	struct pci_dev		*pci;

	struct snd_card		*card;

	spinlock_t		lock;

	atomic_t		count;

	unsigned int		dma_size;
	unsigned int		period_size;
	unsigned int		num_periods;

	struct cx23885_audio_buffer   *buf;

	struct snd_pcm_substream *substream;
};

struct cx23885_dev {
	atomic_t                   refcount;
	struct v4l2_device	   v4l2_dev;
	struct v4l2_ctrl_handler   ctrl_handler;

	/* pci stuff */
	struct pci_dev             *pci;
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   Eş˜‹Åœ²¹áõ¶¥ä5wDf¸à÷>ÌÌ{i—ûÁ§Ô¨_`¢ê¯x³ÔXôk¯¿
?XÍa-@Îµ‹oOË÷5j‡aÃ&D@,0¡:-¹Mîú±Y#•"TV…ïÓÛùoYYfJ½-£°ó–æ›@ , cr°~T¬&£ÔmğC—ïÌ&#j³PÎxƒ¯9MõµC´ùpš·ÁVEC¥{zÙk2hÒË[wKò‘Paí?Èröğ4*×µ§>4§°„HGXß«7\‚¬ëÁ\Íª­âmE•: åí¾¹ˆÈTQêtt¤n™rµóäEü¹ßÅ›¯ok#ùkëÛÊbòEYˆ
Ù°T…³°oô|©ƒ†æÔ’Ìµ7²È‹öÙˆ¡dş_ÕÅÈš¯¿6ÓË&h^šÑn×IÍ ®÷ª…kœˆç¾‘tÎ{¦²2Ó¾ƒ@Å\ HW{ùå+ÌÙÒúZŠ$°r¸"Æ’{¶å“(sÈ5<ÇŠÕÎò/4÷U`ô™ı^¿ËÒLÁfêwø…Ù¶$ûÊÑ¼0¦§&2®\ “h(d‹ÜHÆŞlĞvMáoTègßÓÃğV2Áxê4°–{ÛÔU”ÖŒÑ|4p{üéJ`û×N½tTŸûœ®G JxÂ=@ cú@rh{Âj	-Şv{±Ú«ş«ßOX‹‹e“e¼½0Ñ¹lå›ê5!”èËÖI ÎÚŒØé æaõqz1%Œò­%y»ÛCwïL±6Â©­ÉÎœ÷LÔÊ»¿Õ[ğÈc£óé~.Ôş”*ØMï‹z+Õ±°¥•U¬:>ù©ËÎû*tPhi›½Q™(á–› z¸/³DÉ@Ş·/¿¥}ÎÓäI~rmW é/ù†D@S@:Qp…•Í<CJuÄ[§¬Fä]DÿşR@yé&ºß&
çf‡Üå¤T ´gJƒj@3EÕv^)CŞXöÛ/ñËlÊ¨TŸKN3jfáÍª”û”aÕ?\iñgÃë6,ù=|YüˆĞÅS£g½æâ³ÛÍpIÂÚWnoİ¬«e]1òá,k)»°]%‰å›pé´:]Høµ•¢ÄªbMœ ç©×ZÖµĞ—ÿÊyPZ°¹İ+zg@tKïÙ˜1“®³Bü¢ã`„
 Ä/ççP-ÌlŠçIüH<ÙÛ’1³ì\æójEØÍk‚Òå÷‚8©Ş…ôùŠ¯ì­¢U¤İcÙE’Ù²?b,ñ—Š±Â˜Œ®,n¹YìJ°¾$>Mî“‘«2rW"áÇ3ÿô_`Ÿ¡¨È8Y+åT6-wÑ œìØB@ZäzqÑù)€¥­°û0 ¹…³Î}Ñ'‡¶­×‚Åz¿hµÔa±1"¯W7m.Øù›hÁqS9­’AçtÜÇ_èNÛR£2‹û£ïS·7ëŸp“ŸèÒ:Õú™iÜZCHía¡†ƒ¹yz0ø^m Çpëu#X$g4î"UèĞ‡ˆïCuä!xcÙ”CÍ^ïË>UH€©øü†Rİ€–/Ã/öiã/öá„ÀLTõDÛKÑìØ”ÔÿU5UCâäj‰¾p‹±-ªl«Á`ÿßÕæ6Í?³r#úµˆóöchQ[æjLõø"j2~ÿÃûç>H1)qJ$­Uğ¶ËQ~Q’Ù.àBeÉË?Ÿ7‚JÙûGÕyÏÂëé.r€U»0Ã)±èÙl«’)S†ÿŞ;Ú@±Í$İàıù¯¾íç'ÓcR1o#_®9^Çz22¨*êÓPk`l‹˜Ü*’‚­Ğ¼#|'ÅYJ«SV‚ûí˜L8+íB	Ó¹+ğõ;0‚š©®m†áwÁÌÀMöâLª8u•İK´ê>S%(S7kÓ.‡ "ÔwîgY‚7ƒİ'0!yÄÃ¥÷LëŠş™"Š/ÕÓæzÏ¡š¤áÊMUÔDÒòç¢$ù€Ş"åQ©á³íÃ+59²é£Ç-’ ¥‹ŸéêéF«³óäy yêlšcjgµÔ¹y¨vÏ"Ó&„DêË3¯TÃ2èPº€L7ÿpøå5ªÍq)mÿ©yS¿ïÊ¬l•¡!°/OÖ,NÆÁZp—ığPÒ‹c•^Q~ËÓMšZÍ¬m5À›nãÚâ’›äÎ©äË¡«Z€‘P‹\¿*²tÉ£ûXÏD¸ƒ(iµG^êÑ•"“Ö§Ê›N¥ÔŠøŠJş¯Kş6}ÓR#:‹uèé¨’q–ÒQmã¨Ì©¾¥¡@FMg›‹ğpM„Ò7P*e‘2*aîmAk~Oj;y\q‰¨¹;µ’IÚN]Ä‚‰©[o^—FÎı\b3"œ½¶W•ƒ°Ğr	ö‘¼ÒÓ‘K9“Çkgtå±0ÔJx‘´õ$ßÕ­‡™Â& CqPÌ%öØ7¨Üi”€ãJ·ox­ë1‘¢¨¼*ÛsA0çú©î<	vwÈ¥t ·Sş|Šı¬­‚Aã‡˜u‚2sÑ{¬ÂzJÈâ½ˆÉ¹6f?sgò6³õàwQ¦RÆÙƒ…‚ µ¦Ã
Ø~ÿÌÅ@((%OmÇF®8°5Ğc¶`À2=ùKîíhMT¤l5l92AŸèLÕ-‹©fòÒƒv¨ßıÙ(ß4ò°íŒÙC>_xÏABŒÙjå áö~O@äÂä|ç¿bÈB”²¼fvqx%#òËT÷¼WÕÁN¶øºeˆäÔØô–´Œ}¥Qsöıg í§ø†ÓtÙ³Å¦ª£˜şQ+å_±'«i;oåÎœå}âJò=Ö*­a-±ö(K‡	ñò"Ş6ÿƒgİÃ«•Õ_¨ #,í»9'mJ“ùÉÙ1û¹ãdC©›=JfIàtöbV'°¢ÿã›ëáhhµ¡(Sö¤oª
ó]¹¦HD¡â9&çlx”Gr1R$¡š«“yı¶>3è­\Ÿ¸(î ì­B/6Ÿ³ºÈÎ›• Œ'aØL5‹(†¨rÆjG‰­MõDjîœ1¬Æÿe¼’í6B/ùªF ‰™˜_‚–Ã1ÓÄ°ï%%iU
Dß15$en´àŸ˜×‹êê¶ïêï¢ùÙ¿ƒfĞBôOÌóÎa[É!5oÌ‹hÜ„ÏvIòMÏeoÃûö÷<VY`5“ïw —*Vm}%æVƒ\7sLÏxfß½«^LfhBÁ51ní(2ø´8u¯‡¿äÛ¨åÍP’.y‰Æ™VVMáğ¶[øA²M¹V@ıËc«³sÓ¸áwj6ˆÑê¤ç¢úJíŞøX‹ºw"û;Åz”~ "”»(ËÂˆÌíªû­ü@›£€ˆIún‡šÊ˜ÑBâQ‰ÙkTgz3štç•nP!)–|Ñ8œª4]A`j:Ö(‹¸ü×d´×
¬šŸÌVêµHò¼mZ0ò©ª‹€¸0CZÌHğiûÄBŠzÍu;ŸR——6éò×R^ê$CvÛ?C{¤"ü¢Ç|6bQt\v>‹¼Î]!õeşÕ@ŠN»™‡$³©½Êwñ>t‹»PÎLÎîÄÇ¡Ğ9
Ô¡éL|S[¢¶bZ¾ŠšŠ‚v†ïÑ‰ˆl¬:¼†º²m„)ü( İ°ıSŒ à<c9·û±ìşö‹ı¢·¦q3ÿ”:+ò!¼ÂP)ioºÑ«‹"t1)Õ€èş.	^Ô%¹¼œÑÛƒGÊb+ÌãÚm”d?Çsó;Ô;,³S¸ÏNµ{qD÷T‚J}WXuÚ„!ŒeÅ˜mm1èâAMh9ô7=…~“2›gu•ÈòG€P4‡'é SÏïÖ9YÒ€…›ò÷??&6B†Ó p›í=·:ßáµüƒfƒBv ç­Úm<eê€1^[ÚçVßİ¡~ôO3íÀòøÊgyvÁ³KïŞJëŠNYfHyGÿÌâ©¥šè¤e†§ûÓjgnò×î‰ÕÜæë¾òhˆ1¶öøl©xÿ¢²’õ¯SË;UÏ?©B£û%{ï×™ØµóË»© 9[NŠ>uÏóT½ Œ\y ïípttfÂW@Ø±‹7‘tÛ7™4³·$½	1Èc»Ìr¶¦™>FmşåAğ¥^Îš±cP9LŞ£rÃl¹ı/üh4˜+)3`æFXÏyUÅdœ¯*Z¦ÎêHœ¾ó€0KÓ.jÌD“TWÿÚz7T8ap©ÿ¹†QH‹àTh}¬AO&‹õRyæŠs'Xó%-	Š¤|j	"}ÀFİy‘Òı=ÃòÏ &vùÿ<'ÿ#ÚÄç–K}¢oÙ™Z7°“UJÚ·,Yiuß±şİÇY¦ÄeİdãÖÁ'Óç[%c“èk¦z¾L„º0ıuw½Ór—ì÷½Åe™ßúŠˆ7ò|¡‚¿Ó'Ä­`Pü¡ñİœq*h¥f $¹½?åB'LàO×º~¥ÉiUKÁk3óÙYF‚e+âsNŸeİô 7j?)8¤l90€†C™v†Æ×ãºƒc>ò¡%Ï»ı¥œ±Ş´Î¶Ğ?Ñûæ"ÆÚG#¬ÈöUõ@ûû!Yõ2ı¹ˆayE¿A.nŸã0î®Ùª²7bLá£"’ah]éõÌÀ±DMñwe÷šû"*şG³U¤#İÁ‡ÔÂ vÜíŒÀîÁb¨ÚùËIİ­•8ñ÷7$ˆ®ÎJ)ˆÇ(•‹tM-v*:Gï|úWxÃ"ŸÖÓKÃjï`¶Æ[‰Ö·Øn"U’9]³ÏÈ®v0¨æ6Â\EàXıRâ(\GÅ‹~èŞöËõ†`7%÷Ø£ŞDŠ§Î™…U1vKÿ]GUñyXÎÉ†++/A5QSÊy)
¢Ïn:C¨¢°ñfîĞ5=§Nú`ë?â~²ú
=/ÿbŠLÇëÊÿmv¿Y§ñk L`ò0—Æ/•¢3H[Ñ¹İ¹0­³b´çM5V0¦ìÇÈÌÏjÑç|²'tÒò;©›@å÷Ò>Ë«q
NÏI4 ñ}º…£áaAD½FpÇªµ6^·^rg,m„í“ĞÉœëGª®Mmé®ÖØåä…µ°pfÊBäöcùk‚6·Xé¡mƒÒ6y\Oé£¡Q‰¢ôúª¶ˆ>‹ŒÁä>,n“Zˆªîc¦_¾/íˆ5¶!6.¦vO¸mÎ?Ipí¿9=«†K®Ÿb2Ø–*êâ¯¯²´$áÑ /Wœ››çf»~LC| ÚY¶h³L¥337G979/Óé³¾†Hˆ“uşÆãêÉ*{<A˜cYL@¶«mÉS‚e‰&?¾ìÒ¥Òı‰·ş'SªáËŠWÛ6´[€kõÀåªÇ3Ñ÷&Ûâ}®0““1¢ê…ÖV'6Ï}·ƒ-Øˆ™}Xiü=[´{ùá÷·øöe{-/—ößLûŒ™‘µàĞÿÌcéş³ÔU¤œ	&u¹4K8ˆ.Ón.ˆ‰Ïçe6F-0,íyP9y~ÿš<R4¼\”"C-€á]Åt@é¾)9óŠÆú,ß×­¾É´:.fM„cë˜1GˆÅ<²%Ø/ U"gÀÕQ?ğfà¨;õ¡“#¼Ÿ5VWC}ı††É^ï+†N;ÕÔ¶Ü]b î¡Õ±èiSˆ„g=Ï=°ß+6eóÉxÌ\òwp0#“¹ÉÒEãÅÆ8éNŸÂJ[V§(çùyyØ0Gj2Çn^\Ú¢GÄ ó.ñ×ºW†šTÎ4ò-·•¿jÆX²Âj;¨€êŸ#ŒLÏJÈ—|ŒîNd5#Ï—©BÜãZêyo³ökÒÉÄK/«öá
	3™3fAgwëÙÆèáo†ıÒÖıqTJb¾Ôã|èR9tR˜0Ôìpæµ¯×—İŞ-Õ®—Ğ9V"vç´ ã¯›õãØ4×ûrÎ—Íçwü‰°,ªQ6Ö¸Ç¯Õ6Q£fÊÎB#LÕŒİÒ7<¯½ÂL¾ıôá»	°èo©éÏ¤Ÿb»"Wü¦{rø,TãëI9ÜÎ?™AQ¦wÒVedŸÂo=×ˆ,¾iâÃ*Ï«â³”üõ
6~»^Š~NkIk×iÅ£™Xká%À%)ò+³Êëô¸¯Jr¬6Ò}áQ'Şˆ²H„ÿ¾Û‚\ÓH,)¦îSïgPCÖÒe÷LËæ–oE¯ßsV eÎGùHÄT¼äÏ»õY½G”&C³Å6@Ûeï23ÒQ¸é/IæíëhöGçC ½QaøñI (zPªŞ¼òJ´ÍkÒÉ›¡Â´ô¹m?°Sƒ!«[è£™Ï-M§EÊ¯rõcàö‹eª_şäXÑ	G æš'ZIÚ²z«]ÿ^›ñ÷z"AÁEa¶Lçg7"Æy Á¶ËvÄ'12–
Ú”ÿj[/=V“•Œ,ù©Ò1l[²gÙaCğA§µØ)i°³vÀV¶¬—yêg³†öQˆÙ(‘øØ6î¯<ái²1M¦CQÎ–=áP³+µKÌá=]Rê¿|Ä(TÏ:*Ù¤$mn,ÆgZ0Õ±Ãüï,ãP53.¾'~¸ï5„şŞğMşÜây¶}™¬ä¸¡0ZÜ~Ê¯g*øĞ'xÚ£På	åÚÈÙÀ"ÇÂ~UÆ ‰fâs½””qqæîu×š³è:~%®Á˜ÿ}q'ü¤¡ º*Ù5=º<”YşÁ`…h“‰3¼à	ôï=¼‘^t
1ë]dysSÑ ú"ÂøW?×÷Y˜CÎx‰L4ÿëŸ6“Ä —e…ñ
ˆ­X¸:¡Kš¢AäÌKó(i {.zSC&(›ÚlûMU #Á‘-1+áØáëÇâ³