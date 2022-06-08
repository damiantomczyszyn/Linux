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
	 * Clearly, migrating tasks to offline CPUs is a fairly daft thing.
	 */
	WARN_ON_ONCE(!cpu_online(new_cpu));

	WARN_ON_ONCE(is_migration_disabled(p));
#endif

	trace_sched_migrate_task(p, new_cpu);

	if (task_cpu(p) != new_cpu) {
		if (p->sched_class->migrate_task_rq)
			p->sched_class->migrate_task_rq(p, new_cpu);
		p->se.nr_migrations++;
		rseq_migrate(p);
		perf_event_task_migrate(p);
	}

	__set_task_cpu(p, new_cpu);
}

#ifdef CONFIG_NUMA_BALANCING
static void __migrate_swap_task(struct task_struct *p, int cpu)
{
	if (task_on_rq_queued(p)) {
		struct rq *src_rq, *dst_rq;
		struct rq_flags srf, drf;

		src_rq = task_rq(p);
		dst_rq = cpu_rq(cpu);

		rq_pin_lock(src_rq, &srf);
		rq_pin_lock(dst_rq, &drf);

		deactivate_task(src_rq, p, 0);
		set_task_cpu(p, cpu);
		activate_task(dst_rq, p, 0);
		check_preempt_curr(dst_rq, p, 0);

		rq_unpin_lock(dst_rq, &drf);
		rq_unpin_lock(src_rq, &srf);

	} else {
		/*
		 * Task isn't running anymore; make it appear like we migrated
		 * it before it went to sleep. This means on wakeup we make the
		 * previous CPU our target instead of where it really is.
		 */
		p->wake_cpu = cpu;
	}
}

struct migration_swap_arg {
	struct task_struct *src_task, *dst_task;
	int src_cpu, dst_cpu;
};

static int migrate_swap_stop(void *data)
{
	struct migration_swap_arg *arg = data;
	struct rq *src_rq, *dst_rq;
	int ret = -EAGAIN;

	if (!cpu_active(arg->src_cpu) || !cpu_active(arg->dst_cpu))
		return -EAGAIN;

	src_rq = cpu_rq(arg->src_cpu);
	dst_rq = cpu_rq(arg->dst_cpu);

	double_raw_lock(&arg->src_task->pi_lock,
			&arg->dst_task->pi_lock);
	double_rq_lock(src_rq, dst_rq);

	if (task_cpu(arg->dst_task) != arg->dst_cpu)
		goto unlock;

	if (task_cpu(arg->src_task) != arg->src_cpu)
		goto unlock;

	if (!cpumask_test_cpu(arg->dst_cpu, arg->src_task->cpus_ptr))
		goto unlock;

	if (!cpumask_test_cpu(arg->src_cpu, arg->dst_task->cpus_ptr))
		goto unlock;

	__migrate_swap_task(arg->src_task, arg->dst_cpu);
	__migrate_swap_task(arg->dst_task, arg->src_cpu);

	ret = 0;

unlock:
	double_rq_unlock(src_rq, dst_rq);
	raw_spin_unlock(&arg->dst_task->pi_lock);
	raw_spin_unlock(&arg->src_task->pi_lock);

	return ret;
}

/*
 * Cross migrate two tasks
 */
int migrate_swap(struct task_struct *cur, struct task_struct *p,
		int target_cpu, int curr_cpu)
{
	struct migration_swap_arg arg;
	int ret = -EINVAL;

	arg = (struct migration_swap_arg){
		.src_task = cur,
		.src_cpu = curr_cpu,
		.dst_task = p,
		.dst_cpu = target_cpu,
	};

	if (arg.src_cpu == arg.dst_cpu)
		goto out;

	/*
	 * These three tests are all lockless; this is OK since all of them
	 * will be re-checked with proper locks held further down the line.
	 */
	if (!cpu_active(arg.src_cpu) || !cpu_active(arg.dst_cpu))
		goto out;

	if (!cpumask_test_cpu(arg.dst_cpu, arg.src_task->cpus_ptr))
		goto out;

	if (!cpumask_test_cpu(arg.src_cpu, arg.dst_task->cpus_ptr))
		goto out;

	trace_sched_swap_numa(cur, arg.src_cpu, p, arg.dst_cpu);
	ret = stop_two_cpus(arg.dst_cpu, arg.src_cpu, migrate_swap_stop, &arg);

out:
	return ret;
}
#endif /* CONFIG_NUMA_BALANCING */

/*
 * wait_task_inactive - wait for a thread to unschedule.
 *
 * If @match_state is nonzero, it's the @p->state value just checked and
 * not expected to change.  If it changes, i.e. @p might have woken up,
 * then return zero.  When we succeed in waiting for @p to be off its CPU,
 * we return a positive number (its total switch count).  If a second call
 * a short while later returns the same number, the caller can be sure that
 * @p has remained unscheduled the whole time.
 *
 * The caller must ensure that the task *will* unschedule sometime soon,
 * else this function might spin for a *long* time. This function can't
 * be called with interrupts off, or it may introduce deadlock with
 * smp_call_function() if an IPI is sent by the same process we are
 * waiting to become inactive.
 */
unsigned long wait_task_inactive(struct task_struct *p, unsigned int match_state)
{
	int running, queued;
	struct rq_flags rf;
	unsigned long ncsw;
	struct rq *rq;

	for (;;) {
		/*
		 * We do the initial early heuristics without holding
		 * any task-queue locks at all. We'll only try to get
		 * the runqueue lock when things look like they will
		 * work out!
		 */
		rq = task_rq(p);

		/*
		 * If the task is actively running on another CPU
		 * still, just relax and busy-wait without holding
		 * any locks.
		 *
		 * NOTE! Since we don't hold any locks, it's not
		 * even sure that "rq" stays as the right runqueue!
		 * But we don't care, since "task_running()" will
		 * return false if the runqueue has changed and p
		 * is actually now running somewhere else!
		 */
		while (task_running(rq, p)) {
			if (match_state && unlikely(READ_ONCE(p->__state) != match_state))
				return 0;
			cpu_relax();
		}

		/*
		 * Ok, time to look more closely! We need the rq
		 * lock now, to be *sure*. If we're wrong, we'll
		 * just go back and repeat.
		 */
		rq = task_rq_lock(p, &rf);
		trace_sched_wait_task(p);
		running = task_running(rq, p);
		queued = task_on_rq_queued(p);
		ncsw = 0;
		if (!match_state || READ_ONCE(p->__state) == match_state)
			ncsw = p->nvcsw | LONG_MIN; /* sets MSB */
		task_rq_unlock(rq, p, &rf);

		/*
		 * If it changed from the expected state, bail out now.
		 */
		if (unlikely(!ncsw))
			break;

		/*
		 * Was it really running after all now that we
		 * checked with the proper locks actually held?
		 *
		 * Oops. Go back and try again..
		 */
		if (unlikely(running)) {
			cpu_relax();
			continue;
		}

		/*
		 * It's not enough that it's not actively running,
		 * it must be off the runqueue _entirely_, and not
		 * preempted!
		 *
		 * So if it was still runnable (but just not actively
		 * running right now), it's preempted, and we should
		 * yield - it could be a while.
		 */
		if (unlikely(queued)) {
			ktime_t to = NSEC_PER_SEC / HZ;

			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_hrtimeout(&to, HRTIMER_MODE_REL_HARD);
			continue;
		}

		/*
		 * Ahh, all good. It wasn't running, and it wasn't
		 * runnable, which means that it will never become
		 * running in the future either. We're all done!
		 */
		break;
	}

	return ncsw;
}

/***
 * kick_process - kick a running thread to enter/exit the kernel
 * @p: the to-be-kicked thread
 *
 * Cause a process which is running on another CPU to enter
 * kernel-mode, without any delay. (to get signals handled.)
 *
 * NOTE: this function doesn't have to take the runqueue lock,
 * because all it wants to ensure is that the remote task enters
 * the kernel. If the IPI races and the task has been migrated
 * to another CPU then no harm is done and the purpose has been
 * achieved as well.
 */
void kick_process(struct task_struct *p)
{
	int cpu;

	preempt_disable();
	cpu = task_cpu(p);
	if ((cpu != smp_processor_id()) && task_curr(p))
		smp_send_reschedule(cpu);
	preempt_enable();
}
EXPORT_SYMBOL_GPL(kick_process);

/*
 * ->cpus_ptr is protected by both rq->lock and p->pi_lock
 *
 * A few notes on cpu_active vs cpu_online:
 *
 *  - cpu_active must be a subset of cpu_online
 *
 *  - on CPU-up we allow per-CPU kthreads on the online && !active CPU,
 *    see __set_cpus_allowed_ptr(). At this point the newly online
 *    CPU isn't yet part of the sched domains, and balancing will not
 *    see it.
 *
 *  - on CPU-down we clear cpu_active() to mask the sched domains and
 *    avoid the load balancer to place new tasks on the to be removed
 *    CPU. Existing tasks will remain running there and will be taken
 *    off.
 *
 * This means that fallback selection must not select !active CPUs.
 * And can assume that any active CPU must be online. Conversely
 * select_task_rq() below may allow selection of !active CPUs in order
 * to satisfy the above rules.
 */
static int select_fallback_rq(int cpu, struct task_struct *p)
{
	int nid = cpu_to_node(cpu);
	const struct cpumask *nodemask = NULL;
	enum { cpuset, possible, fail } state = cpuset;
	int dest_cpu;

	/*
	 * If the node that the CPU is on has been offlined, cpu_to_node()
	 * will return -1. There is no CPU on the node, and we should
	 * select the CPU on the other node.
	 */
	if (nid != -1) {
		nodemask = cpumask_of_node(nid);

		/* Look foRRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (in_list(e, &class->locks_after) ||
		    in_list(e, &class->locks_before))
			return true;
	}
	return false;
}

static bool class_lock_list_valid(struct lock_class *c, struct list_head *h)
{
	struct lock_list *e;

	list_for_each_entry(e, h, entry) {
		if (e->links_to != c) {
			printk(KERN_INFO "class %s: mismatch for lock entry %ld; class %s <> %s",
			       c->name ? : "(?)",
			       (unsigned long)(e - list_entries),
			       e->links_to && e->links_to->name ?
			       e->links_to->name : "(?)",
			       e->class && e->class->name ? e->class->name :
			       "(?)");
			return false;
		}
	}
	return true;
}

#ifdef CONFIG_PROVE_LOCKING
static u16 chain_hlocks[MAX_LOCKDEP_CHAIN_HLOCKS];
#endif

static bool check_lock_chain_key(struct lock_chain *chain)
{
#ifdef CONFIG_PROVE_LOCKING
	u64 chain_key = INITIAL_CHAIN_KEY;
	int i;

	for (i = chain->base; i < chain->base + chain->depth; i++)
		chain_key = iterate_chain_key(chain_key, chain_hlocks[i]);
	/*
	 * The 'unsigned long long' casts avoid that a compiler warning
	 * is reported when building tools/lib/lockdep.
	 */
	if (chain->chain_key != chain_key) {
		printk(KERN_INFO "chain %lld: key %#llx <> %#llx\n",
		       (unsigned long long)(chain - lock_chains),
		       (unsigned long long)chain->chain_key,
		       (unsigned long long)chain_key);
		return false;
	}
#endif
	return true;
}

static bool in_any_zapped_class_list(struct lock_class *class)
{
	struct pending_free *pf;
	int i;

	for (i = 0, pf = delayed_free.pf; i < ARRAY_SIZE(delayed_free.pf); i++, pf++) {
		if (in_list(&class->lock_entry, &pf->zapped))
			return true;
	}

	return false;
}

static bool __check_data_structures(void)
{
	struct lock_class *class;
	struct lock_chain *chain;
	struct hlist_head *head;
	struct lock_list *e;
	int i;

	/* Check whether all classes occur in a lock list. */
	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (!in_list(&class->lock_entry, &all_lock_classes) &&
		    !in_list(&class->lock_entry, &free_lock_classes) &&
		    !in_any_zapped_class_list(class)) {
			printk(KERN_INFO "class %px/%s is not in any class list\n",
			       class, class->name ? : "(?)");
			return false;
		}
	}

	/* Check whether all classes have valid lock lists. */
	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (!class_lock_list_valid(class, &class->locks_before))
			return false;
		if (!class_lock_list_valid(class, &class->locks_after))
			return false;
	}

	/* Check the chain_key of all lock chains. */
	for (i = 0; i < ARRAY_SIZE(chainhash_table); i++) {
		head = chainhash_table + i;
		hlist_for_each_entry_rcu(chain, head, entry) {
			if (!check_lock_chain_key(chain))
				return false;
		}
	}

	/*
	 * Check whether all list entries that are in use occur in a class
	 * lock list.
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		e = list_entries + i;
		if (!in_any_class_list(&e->entry)) {
			printk(KERN_INFO "list entry %d is not in any class list; class %s <> %s\n",
			       (unsigned int)(e - list_entries),
			       e->class->name ? : "(?)",
			       e->links_to->name ? : "(?)");
			return false;
		}
	}

	/*
	 * Check whether all list entries that are not in use do not occur in
	 * a class lock list.
	 */
	for_each_clear_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		e = list_entries + i;
		if (in_any_class_list(&e->entry)) {
			printk(KERN_INFO "list entry %d occurs in a class list; class %s <> %s\n",
			       (unsigned int)(e - list_entries),
			       e->class && e->class->name ? e->class->name :
			       "(?)",
			       e->links_to && e->links_to->name ?
			       e->links_to->name : "(?)");
			return false;
		}
	}

	return true;
}

int check_consistency = 0;
module_param(check_consistency, int, 0644);

static void check_data_structures(void)
{
	static bool once = false;

	if (check_consistency && !once) {
		if (!__check_data_structures()) {
			once = true;
			WARN_ON(once);
		}
	}
}

#else /* CONFIG_DEBUG_LOCKDEP */

static inline void check_data_structures(void) { }

#endif /* CONFIG_DEBUG_LOCKDEP */

static void init_chain_block_buckets(void);

/*
 * Initialize the lock_classes[] array elements, the free_lock_classes list
 * and also the delayed_free structure.
 */
static void init_data_structures_once(void)
{
	static bool __read_mostly ds_initialized, rcu_head_initialized;
	int i;

	if (likely(rcu_head_initialized))
		return;

	if (system_state >= SYSTEM_SCHEDULING) {
		init_rcu_head(&delayed_free.rcu_head);
		rcu_head_initialized = true;
	}

	if (ds_initialized)
		return;

	ds_initialized = true;

	INIT_LIST_HEAD(&delayed_free.pf[0].zapped);
	INIT_LIST_HEAD(&delayed_free.pf[1].zapped);

	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		list_add_tail(&lock_classes[i].lock_entry, &free_lock_classes);
		INIT_LIST_HEAD(&lock_classes[i].locks_after);
		INIT_LIST_HEAD(&lock_classes[i].locks_before);
	}
	init_chain_block_buckets();
}

static inline struct hlist_head *keyhashentry(const struct lock_class_key *key)
{
	unsigned long hash = hash_long((uintptr_t)key, KEYHASH_BITS);

	return lock_keys_hash + hash;
}

/* Register a dynamically allocated key. */
void lockdep_register_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head;
	struct lock_class_key *k;
	unsigned long flags;

	if (WARN_ON_ONCE(static_obj(key)))
		return;
	hash_head = keyhashentry(key);

	raw_local_irq_save(flags);
	if (!graph_lock())
		goto restore_irqs;
	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (WARN_ON_ONCE(k == key))
			goto out_unlock;
	}
	hlist_add_head_rcu(&key->hash_entry, hash_head);
out_unlock:
	graph_unlock();
restore_irqs:
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lockdep_register_key);

/* Check whether a key has been registered as a dynamic key. */
static bool is_dynamic_key(const struct lock_class_key *key)
{
	struct hlist_head *hash_head;
	struct lock_class_key *k;
	bool found = false;

	if (WARN_ON_ONCE(static_obj(key)))
		return false;

	/*
	 * If lock debugging is disabled lock_keys_hash[] may contain
	 * pointers to memory that has already been freed. Avoid triggering
	 * a use-after-free in that case by returning early.
	 */
	if (!debug_locks)
		return true;

	hash_head = keyhashentry(key);

	rcu_read_lock();
	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (k == key) {
			found = true;
			break;
		}
	}
	rcu_read_unlock();

	return found;
}

/*
 * Register a lock's class in the hash-table, if the class is not present
 * yet. Otherwise we look it up. We cache the result in the lock object
 * itself, so actual lookup of the hash should be once per lock object.
 */
static struct lock_class *
register_lock_class(struct lockdep_map *lock, unsigned int subclass, int force)
{
	struct lockdep_subclass_key *key;
	struct hlist_head *hash_head;
	struct lock_class *class;
	int idx;

	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	class = look_up_lock_class(lock, subclass);
	if (likely(class))
		goto out_set_class_cache;

	if (!lock->key) {
		if (!assign_lock_key(lock))
			return NULL;
	} else if (!static_obj(lock->key) && !is_dynamic_key(lock->key)) {
		return NULL;
	}

	key = lock->key->subkeys + subclass;
	hash_head = classhashentry(key);

	if (!graph_lock()) {
		return NULL;
	}
	/*
	 * We have to do the hash-walk again, to avoid races
	 * with another CPU:
	 */
	hlist_for_each_entry_rcu(class, hash_head, hash_entry) {
		if (class->key == key)
			goto out_unlock_set;
	}

	init_data_structures_once();

	/* Allocate a new lock class and add it to the hash. */
	class = list_first_entry_or_null(&free_lock_classes, typeof(*class),
					 lock_entry);
	if (!class) {
		if (!debug_locks_off_graph_unlock()) {
			return NULL;
		}

		print_lockdep_off("BUG: MAX_LOCKDEP_KEYS too low!");
		dump_stack();
		return NULL;
	}
	nr_lock_classes++;
	__set_bit(class - lock_classes, lock_classes_in_use);
	debug_atomic_inc(nr_unused_locks);
	class->key = key;
	class->name = lock->name;
	class->subclass = subclass;
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	class->name_version = count_matching_names(class);
	class->wait_type_inner = lock->wait_type_inner;
	class->wait_type_outer = lock->wait_type_outer;
	class->lock_type = lock->lock_type;
	/*
	 * We use RCU's safe list-add method to make
	 * parallel walking of the hash-list safe:
	 */
	hlist_add_head_rcu(&class->hash_entry, hash_head);
	/*
	 * Remove the class from the free list and add it to the global list
	 * of classes.
	 */
	list_move_tail(&class->lock_entry, &all_lock_classes);
	idx = class - lock_classes;
	if (idx > max_lock_class_idx)
		max_lock_class_idx = idx;

	if (verbose(class)) {
		graph_unlock();

		printk("\nnew class %px: %s", class->key, class->name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		printk(KERN_CONT "\n");
		dump_stack();

		if (!graph_lock()) {
			return NULL;
		}
	}
out_unlock_set:
	graph_unlock();

out_set_class_cache:
	if (!subclass || force)
		lock->class_cache[0] = class;
	else if (subclass < NR_LOCKDEP_CACHING_CLASSES)
		lock->class_cache[subclass] = class;

	/*
	 * Hash collision, did we smoke some? We found a class with a matching
	 * hash but the subclass -- which is hashed in -- didn't match.
	 */
	if (DEBUG_LOCKS_WARN_ON(class->subclass != subclass))
		return NULL;

	return class;
}

#ifdef CONFIG_PROVE_LOCKING
/*
 * Allocate a lockdep entry. (assumes the graph_lock held, returns
 * with NULL on failure)
 */
static struct lock_list *alloc_list_entry(void)
{
	int idx = find_first_zero_bit(list_entries_in_use,
				      ARRAY_SIZE(list_entries));

	if (idx >= ARRAY_SIZE(list_entries)) {
		if (!debug_locks_off_graph_unlock())
			return NULL;

		print_lockdep_off("BUG: MAX_LOCKDEP_ENTRIES too low!");
		dump_stack();
		return NULL;
	}
	nr_list_entries++;
	__set_bit(idx, list_entries_in_use);
	return list_entries + idx;
}

/*
 * Add a new dependency to the head of the list:
 */
static int add_lock_to_list(struct lock_class *this,
			    struct lock_class *links_to, struct list_head *head,
			    unsigned long ip, u16 distance, u8 dep,
			    const struct lock_trace *trace)
{
	struct lock_list *entry;
	/*
	 * Lock not present yet - get a new dependency struct and
	 * add it to the list:
	 */
	entry = alloc_list_entry();
	if (!entry)
		return 0;

	entry->class = this;
	entry->links_to = links_to;
	entry->dep = dep;
	entry->distance = distance;
	entry->trace = trace;
	/*
	 * Both allocation and removal are done under the graph lock; but
	 * iteration is under RCU-sched; see look_up_lock_class() and
	 * lockdep_free_key_range().
	 */
	list_add_tail_rcu(&entry->entry, head);

	return 1;
}

/*
 * For good efficiency of modular, we use power of 2
 */
#define MAX_CIRCULAR_QUEUE_SIZE		(1UL << CONFIG_LOCKDEP_CIRCULAR_QUEUE_BITS)
#define CQ_MASK				(MAX_CIRCULAR_QUEUE_SIZE-1)

/*
 * The circular_queue and helpers are used to implement graph
 * breadth-first search (BFS) algorithm, by which we can determine
 * whether there is a path from a lock to another. In deadlock checks,
 * a path from the next lock to be acquired to a previous held lock
 * indicates that adding the <prev> -> <next> lock dependency will
 * produce a circle in the graph. Breadth-first search instead of
 * depth-first search is used in order to find the shortest (circular)
 * path.
 */
struct circular_queue {
	struct lock_list *element[MAX_CIRCULAR_QUEUE_SIZE];
	unsigned int  front, rear;
};

static struct circular_queue lock_cq;

unsigned int max_bfs_queue_depth;

static unsigned int lockdep_dependency_gen_id;

static inline void __cq_init(struct circular_queue *cq)
{
	cq->front = cq->rear = 0;
	lockdep_dependency_gen_id++;
}

static inline int __cq_empty(struct circular_queue *cq)
{
	return (cq->front == cq->rear);
}

static inline int __cq_full(struct circular_queue *cq)
{
	return ((cq->rear + 1) & CQ_MASK) == cq->front;
}

static inline int __cq_enqueue(struct circular_queue *cq, struct lock_list *elem)
{
	if (__cq_full(cq))
		return -1;

	cq->element[cq->rear] = elem;
	cq->rear = (cq->rear + 1) & CQ_MASK;
	return 0;
}

/*
 * Dequeue an element from the circular_queue, return a lock_list if
 * the queue is not empty, or NULL if otherwise.
 */
static inline struct lock_list * __cq_dequeue(struct circular_queue *cq)
{
	struct lock_list * lock;

	if (__cq_empty(cq))
		return NULL;

	lock = cq->element[cq->front];
	cq->front = (cq->front + 1) & CQ_MASK;

	return lock;
}

static inline unsigned int  __cq_get_elem_count(struct circular_queue *cq)
{
	return (cq->rear - cq->front) & CQ_MASK;
}

static inline void mark_lock_accessed(struct lock_list *lock)
{
	lock->class->dep_gen_id = lockdep_dependency_gen_id;
}

static inline void visit_lock_entry(struct lock_list *lock,
				    struct lock_list *parent)
{
	lock->parent = parent;
}

static inline unsigned long lock_accessed(struct lock_list *lock)
{
	return lock->class->dep_gen_id == lockdep_dependency_gen_id;
}

static inline struct lock_list *get_lock_parent(struct lock_list *child)
{
	return child->parent;
}

static inline int get_lock_depth(struct lock_list *child)
{
	int depth = 0;
	struct lock_list *parent;

	while ((parent = get_lock_parent(child))) {
		child = parent;
		depth++;
	}
	return depth;
}

/*
 * Return the forward or backward dependency list.
 *
 * @lock:   the lock_list to get its class's dependency list
 * @offset: the offset to struct lock_class to determine whether it is
 *          locks_after or locks_before
 */
static inline struct list_head *get_dep_list(struct lock_list *lock, int offset)
{
	void *lock_class = lock->class;

	return lock_class + offset;
}
/*
 * Return values of a bfs search:
 *
 * BFS_E* indicates an error
 * BFS_R* indicates a result (match or not)
 *
 * BFS_EINVALIDNODE: Find a invalid node in the graph.
 *
 * BFS_EQUEUEFULL: The queue is full while doing the bfs.
 *
 * BFS_RMATCH: Find the matched node in the graph, and put that node into
 *             *@target_entry.
 *
 * BFS_RNOMATCH: Haven't found the matched node and keep *@target_entry
 *               _unchanged_.
 */
enum bfs_result {
	BFS_EINVALIDNODE = -2,
	BFS_EQUEUEFULL = -1,
	BFS_RMATCH = 0,
	BFS_RNOMATCH = 1,
};

/*
 * bfs_result < 0 means error
 */
static inline bool bfs_error(enum bfs_result res)
{
	return res < 0;
}

/*
 * DEP_*_BIT in lock_list::dep
 *
 * For dependency @prev -> @next:
 *
 *   SR: @prev is shared reader (->read != 0) and @next is recursive reader
 *       (->read == 2)
 *   ER: @prev is exclusive locker (->read == 0) and @next is recursive reader
 *   SN: @prev is shared reader and @next is non-recursive locker (->read != 2)
 *   EN: @prev is exclusive locker and @next is non-recursive locker
 *
 * Note that we define the value of DEP_*_BITs so that:
 *   bit0 is prev->read == 0
 *   bit1 is next->read != 2
 */
#define DEP_SR_BIT (0 + (0 << 1)) /* 0 */
#define DEP_ER_BIT (1 + (0 << 1)) /* 1 */
#define DEP_SN_BIT (0 + (1 << 1)) /* 2 */
#define DEP_EN_BIT (1 + (1 << 1)) /* 3 */

#define DEP_SR_MASK (1U << (DEP_SR_BIT))
#define DEP_ER_MASK (1U << (DEP_ER_BIT))
#define DEP_SN_MASK (1U << (DEP_SN_BIT))
#define DEP_EN_MASK (1U << (DEP_EN_BIT))

static inline unsigned int
__calc_dep_bit(struct held_lock *prev, struct held_lock *next)
{
	return (prev->read == 0) + ((next->read != 2) << 1);
}

static inline u8 calc_dep(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bit(prev, next);
}

/*
 * calculate the dep_bit for backwards edges. We care about whether @prev is
 * shared and whether @next is recursive.
 */
static inline unsigned int
__calc_dep_bitb(struct held_lock *prev, struct held_lock *next)
{
	return (next->read != 2) + ((prev->read == 0) << 1);
}

static inline u8 calc_depb(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bitb(prev, next);
}

/*
 * Initialize a lock_list entry @lock belonging to @class as the root for a BFS
 * search.
 */
static inline void __bfs_init_root(struct lock_list *lock,
				   struct lock_class *class)
{
	lock->class = class;
	lock->parent = NULL;
	lock->only_xr = 0;
}

/*
 * Initialize a lock_list entry @lock based on a lock acquisition @hlock as the
 * root for a BFS search.
 *
 * ->only_xr of the initial lock node is set to @hlock->read == 2, to make sure
 * that <prev>ELF                      ¤"      4     (            GNU  À       À          èüÿÿÿ…Àt?S‰Ã‹€ˆ  …Àt‹…Òt
Ç‚$&      èüÿÿÿÇƒˆ      ÇƒŒ      [Ã´&    Ã´&    èüÿÿÿUWV‰ÆS‰÷ƒìd¡    ‰D$1ÀƒÇ„Î   ‹–  …Ò„ı   ‹†,&  …À…   ‹FÇ$    1ÉºÿÿÿÿTj ˆ   h    èüÿÿÿƒÄ…À…t   ¡   ¹   ºÀ  ‹,$èüÿÿÿ‰Ã…À„Z   ‰8¹   º
   ‰h‰…ˆ  Ç…Œ      ‹x‹(GèüÿÿÿG$ÿuøƒÇDh   j Pèüÿÿÿÿudÿuøh@  jPWèüÿÿÿ‰ØƒÄ$èüÿÿÿ…À…   ‰,&  ‹$èüÿÿÿ…À…¨   ö    u-‹D$d+    uƒÄ1À[^_]Ãv ö    tİéüÿÿÿèüÿÿÿéä   ffffèüÿÿÿ‹@Ç@    Ç@    1ÀÃ´&    èüÿÿÿ1ÀÃ´&    èüÿÿÿVS‹p^‰Øèüÿÿÿ‹v‰Â‰Øèüÿÿÿ‰ğ[^Ã´&    t& èüÿÿÿUWVS‰Ãƒì‹5   ‰$‰Ê…ö…2  ‹h…í„Â   ‹}|…ÿ„·   ‹w`Áî„   9ò‚  ‹4  …É„7  ‰Ğ1Ò÷ö‹S‰D$‹GT‰D$‰Ğ¯ÆÈ‰Ñ‰D$‹D$Ğ‰Â‹D$9Âsv¯t$‹$‹D$‰ñèüÿÿÿ‰èèüÿÿÿ‹C‹t$ğ‰C‹OT‰Â)Ê9ÈCÂs‰s‰ñ‰C‹GL9Æƒ   ƒÄ‰è[^_]éüÿÿÿ‹-   …í…  ƒÄ[^_]Ã´&    v ‰Â‰L$‹D$)Ê‰Ñ‹$¯Î‰L$èüÿÿÿ‹D$‹L$+L$‹$È‹L$¯ÆÊ‰Á‹‡4  èüÿÿÿéTÿÿÿ´&    ‹   …Û…   ƒÄ[^_]Ã´&    v )Á‰è‰Kèüÿÿÿ‰èƒÄ[^_]éüÿÿÿ¶    ‹   …É„Pÿÿÿé‡  ´&    ¶    èüÿÿÿWVS‹p‹ƒ   »T  èüÿÿÿ1Ò‰øèüÿÿÿğ€£¨  ï‰øèüÿÿÿÇƒ(&      ‹   èüÿÿÿ1À[^_Ã´&    ´&    èüÿÿÿU‰ÅWVSƒìl‹u|d¡    ‰D$h‹E‹‰$Cø‰D$‰Çƒ   èüÿÿÿ‹ƒ<&  ‹“”  ‰|$dH‰D$\D$‰‹<&  ‰T$`èüÿÿÿ…À…"  ‰Â‹ƒ¨  „ÀxƒT  ‰D$ğº«¨  s6‹$‰T$‹    èüÿÿÿ‹T$‹D$hd+    …ò   ƒÄl‰Ğ[^_]Ãt& ‰Ğ¾ø   ¹   º   ó«‰ğÇ†ø    Ç†ü      Ç†       Ç†  €   Ç†  €»  Ç†  €»  Ç†     Ç†     Ç†  ¨ @Ç†!  1  Ç†(  b   èüÿÿÿ‹<$‹L$‰o‰ğ   Çƒ(&      ğ€‹¨  ‹D$èüÿÿÿ‰$‹   èüÿÿÿ‹$éÿÿÿ‹$‹    èüÿÿÿºğÿÿÿéışÿÿèüÿÿÿ´&    èüÿÿÿU1Éº   W‰ÇVSƒì‹/d¡    ‰D$1À‹GTjj èüÿÿÿ‰ÆƒÄ…À…Ô  ÇG    ‹$¹€   º   èüÿÿÿ1Éº   j j ‹D$èüÿÿÿ‹\$ºP   ÇC    ƒÃX‰»`  ‹}d‰øèüÿÿÿ‰ÁXZƒùÿ„Ê  ƒùPtƒÁƒùP‡À  ‰ú‰Øèüÿÿÿ‹D$d+    u
ƒÄ‰ğ[^_]ÃèüÿÿÿS‹@@…Àuh    h    èüÿÿÿXZë6‹$&  …ÒuƒÀh    Ph0   èüÿÿÿƒÄë‹‹BèüÿÿÿÇƒ$&      1À[Ãhh  èüÿÿÿº    ¸   èüÿÿÿ1É1Òh    èüÿÿÿÇ        hŒ  èüÿÿÿƒÄÃh(   h    h`   èüÿÿÿƒÄéb  ƒÆPh   Vhl  èüÿÿÿƒÄ‹$…Àtèüÿÿÿ‰Øèüÿÿÿh(   Vh  èüÿÿÿƒÄéb  ƒÆjôh   Vh  èüÿÿÿƒÄë»ƒÆPh   VhĞ   èüÿÿÿƒÄëµƒÆh(   Vh˜   èüÿÿÿƒÄéb  Ç†,&      ƒÆPh   Vh¨  èüÿÿÿƒÄéaÿÿÿh(   h    èüÿÿÿY[éb  h(   h    hà  èüÿÿÿƒÄéb  hd   hd   hp  èüÿÿÿƒÄéê  hd   hL  èüÿÿÿ^_éê  Q‰L$ÿt$Phd   h´  èüÿÿÿ‹k‹T$ƒÄ…ítD‹}|…ÿ…3  ƒ=    „ê  hd   h$  èüÿÿÿXZéê  hd   hœ  èüÿÿÿXZéê  ƒ=    „ê  hd   hü  èüÿÿÿY[éê  ¸H   èüÿÿÿ¸@   èüÿÿÿƒÅPhP   UhĞ  èüÿÿÿƒÄép  èüÿÿÿhH  èüÿÿÿX1ÀÇ    P   Ã           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   °                 À      Ğ                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P          À  Ğ     °    À  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           ¤ÿ     À           ¤ÿ    ×    s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ñÿ       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 ‰            ˜   U   ?     §            ¾           Õ      #     ğ   =          O   Y       ¨        4           B  8       
 T           ñÿd          y  À         Ğ  %     Š          £    À     s   d       
 Ä  °  R     ß    ©    ù  €   @    
   Ä  0     +  H       
 6  @       
 A  P       
 L  »   -     e  è        ‚          ”  À   
    
 ª             ¶             Ä             Ğ             Ù             ë             ı               U   ?                  !             '             >             U             c             n           z             ˜                           ¶             Ç             Ï             Ü             ç             ÷             	                          9             X             u             “             ›             ¯             ½             Ê             á           ñ     £                 $             2  À  Ğ     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q          «   6  °   I  »     À   ]  Ò   @  Ü     è   	  û       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  ‚  W  Š      G  ”    ¡  B  Á  B  Ñ  B  à  ?  ì  N    B          ™  =     S  Ü  F  â    ê      =  @  =  R    Z    x  F  †  Y  ’    Ÿ    ±  B  Ê  J  Ó  P  â  M  ø  U    B  #  9  C  J  m  L  ©  U  ¸  9  U  Q  o  X  €  D    U  ¦  U  µ  G  Á  B  É  	  Ù  9  ì  7  ù          K  &  O  I  H  V    g    p  R  {  9  Œ  G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  ‡     Œ   E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     ƒ     ˆ   E  •     ›         E  ¨     »     Á     Æ   E  Ó     Ø     İ   E  ä     é     î   	  ó     ø   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    €  E  ‡    Œ    ‘    –  E      £    ª    ¯    ´    ¹  E  À    Å    Ê  A  Ï    Ô  A  İ    ã    è  E  ğ       B          E     T       €     „     ”     ˜                                                          $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `                    .   	   @       h  ˆ              <             ğ  ”                  8   	   @       ğ  ˆ               K             „  ô                 G   	   @       x  H              ^             x	                    Z   	   @       À   (               m              	  Ê                   i   	   @       è   (      
         u      2       l
  	                ˆ             u  (                  „   	   @       !  P               •      2         +                 ¤             È  ÿ                  ±             È  (                  ­   	   @       `!  @               ½             ğ                    ¹   	   @        !                 È             ü                     Î             ü                    Ó      0       ü  &                 Ü              "                                   $  à     5         	                b                               ¸!  ì                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              êÈÄ—‡IôA§92=õånxsw(ÄŸuzÎá¼ã*¤Óãrù?/WÏ
Ğóc^|ï–‰Bƒ¬ÎëÌ0×bí6[ĞÀœGvŸ{z—=—àÌQà5ŒÙ(Ñ…ÅËú£İš™J¥÷?£8˜ÖØß†Á®Vr:9x·SsØ?ÀN3Ê)±Cs‹¼è’/#´²ÓÏç#”µ"U 4ËF?óHÑÂ5ÓV¨5ÙÍc‚Ã×Ñ×ĞöûÀA×6€áB-\É+É×J'‡1ãrx:x×ŞÖê™!…ÌO	lÀs…Cƒƒõc¶	ß¹VeçóŸ.°ë9ãñCåòÖ¹Qr7¢Ê'Äap«=[õmñø®Ó‡¢‚unRÔ^Ş¦3Âì[À-ÙáqÅ²Å=ÅLl:0èBúwZiTX”¶¡\÷!|*"ò€›¹çğÕñÇ*Ão°ü£A'ÄG~GÀÅ"‡’ã`g	’ØĞÁ›ÓH¦òeQ|k
“m¿UPA´˜!ö"‹pš†Üà+D(=PiÀ0:8:–äÍ®ø³¤–&EbmîD>‚•…Ş®ş‹ì»2¤eÇo·˜yŸT>w,ˆ÷²l.V©u{/v‹»MÃ"¶-Ë ÎÔ«æ&[GÛSi@?Äçı[Bc+BU¦5ÿ“£{âüêÕ7İÆ!PÄ¦é_D²-«_\­. =w½µJf`Šˆİ
ÉY?[+ŞG½E®C ˜, ²Í¡ªcW	pD4ŒVtÍş*ÿÚnÄ. QÀ±m,sÍ8¾£J¯…î`u£L Ì[&„h¤Q¶gìJEæ²îJèÕ!—Wºíñx)³ü„»¸6 ­şòı8kÔ±æÓ¾‘¸ÀÎObGG¡yåP·¤AÛúß®ìØxóé0G$ÆüòO ‚S³ŒÔP4w\@n Çë¨âoÖEQœN1"z’ë|á'z‘¬iÍ¸ï#yQ‡Ñ„¼8€âWñºl—ÔÕöôÈXmÇğNïÙdÑ1å~¶æ^Û×Ğ¼ô’n }¾ËL 
¯8b*ğtÉWgâ… XŠ6´nş‘Ïªğ4yR…—°Ş˜µ¼Eï¥«NLkHçÈ¨Îç(³°AĞì˜”)Ë/Wì^Q¹´Åµuõ=6ä"wKâjôÉa7½È€ª÷oF#±Ü-—å2Vu}E|™ÖìáuŠî,ùÌhY/ˆ©%|ùTMx¶‘‡Ù—‰qÂ:±=ûë{RtìjSÀ“CÚäë»1‰5ÉÓ2¤¡Û<Lì»ñ”pî˜äÌK‘S®¢´À K£—ŞİR#¢'íÛkÍFÏ”…F¸‘´ÜHJÛ/!ŠI´ C;½ºsBÆL_
ßÃ÷Ûi]*Çœc·ª†wmÄñ€Ò1ıªwPUúö»”+4ÎåÙ’À/Jü‹…,|İİÜîİ …[ Œ‘#mğˆ€#È$Æ¾ß<Ë&şI'MèT¸ï“Y!$_Ô?§×ÎB¨-AY^WÊñ:êW°cÚˆN&à±`ÛFİ ÓRVò¶û<†v‡İ#tƒ²ƒÙ&£ù@ËÌC+ÖÊ}:šà=Bİ(ˆÃ²(f+OŞ‚¡¼43ÏÌ%Çê}cÈ£¥G"¿ì­üi¾T4<vÀG­ÖªîXú …
V;
Ç…§q"¼^Å©ãkQüÂ $U½¦8Š}º;×â3è~,Æ¦wWâÛàº¿ü1…Ít¾p9„ƒ-–…ç ÿ"3æ*u”\—½,B55V÷îQÃ€ÉÑ?6º³¹F7Ãuk†0œOs,ÉàÇ´K åàgÊ[W™/Tê5íKø(jPkKşiİ%Ë)— J_ˆj©öçÊ¨ÄÄ›ì1ƒûÌHsS½£?9g-ÉÊÙ‘BT,Y€´é–#ÖôJËğ xAÂÌ[”ƒV/ÍñúzÅ–ëUjÊÕ6oLcÈÓƒ<[zqÎvˆ„s¿kŒ¯9İkµËgĞå©C„a®ˆQl\GÃç*Ã"]Hw˜‘“¹xbèæ]â¢ës8_|S5êİùˆ×Zš¶O¿!FóØóà–JvG©’€‹,Wvñ×ÑÒ¡éhÉe+ß™öW š«ÍÉ¸ô	w?*éˆeÌ)\–»éËKİdëJØNo5HÛ £×¼iú)ˆºyì!¾ (»¾K íP«6ÛcUÑ3$31gJ®TÌy‘DãiÑ.îCü×ã+Ô ^Äècı™¨õê8Ak[É[ÓLº\¸&HÙÄĞ6àõ³­4?œ&6,¸ãùì„aZÀĞó¥\ĞË!‡xpÆóæSÖZøü{±BßÉ7&Ø‹‰ˆ¢TÒÃjĞym"GßĞÚ¼Š'¥É‚BcßƒGi­ ª;éÀ£ßcõS({­zª,İ@S¤}EeÏjº¡ï©]p¼è¼Ò_Ì:bËÒáÉ©‹¤ôåÚ8}§ÕÕ²ÁŠ„« Dz¸õ¿C0ëò!Š9f_’í¹ç_ö¯B2,ª*enƒÌ‰6€’òcÎe=™Á•şÛ÷]DLù¼•rùºRë”ç4Ğè_"àÓ7Sœ› êŒØ+f’ÿ>à"ô}èı¶~I¯ÃiíÜëºár¯´4¸™UXs¼g|CŞI¤O–Ì(â…_¯¿XN~ëjäUN+Ø"&uĞ<-ö—úç>ÜNì+®ÀÎŞû¬ÜµôH^›¯’ª°'¢EjC³­8M÷ğZ”,ìïmáÏµ¡m…„P‚ñ"Á**‰CTö«Âµ)d÷¶_FÑü¨ú=ïDwY…6ìøúƒbÎd’õ,¶q•ÙUôú¦xC>ò5şX[Š‹ëÿ—VÎ…7Ñ„Ìà–F%©H Sp„²0ÅÚ±ŞqÇÈêEÿù³¯âûÙ˜K¯M-âOE§@2¡ùw!RÏ©æRÿ²­	k9˜QP(å®ŸŸ`™ú-ãôìãô›˜é`ı»Û”ıqFI$eç?´ñd‚ÅŒ_©Œ›SSµ±d«½vº*Ï«øw»°â–#re]á#uáhta{ï˜vh˜´†â«*õË@—UáFq-&¨ƒ@…š€[gûØÂgÓ‹ã5íîYÛ]©ÜbŞ$·+ºAİJX-#t¨¼,şç÷uçàÍeÆXPAî*İ%Pá§İn?˜«C¤Ò¯ÑfZ7±ÍJTRÛ’²’×<@
öé
4KÆ¼ñØ1HÍgÑ3£¿­åñ6w•êwîÉ¹1Jîa‰õ§±–1¨%åï5˜`JéúèB/pz3ÒË0¿¸
`H·š¢5yxÏvÇ?Oëç'İñğ¾:Bb²ôdLŞÛäØü.ÄæGÓ 9·ıi··B?œŸÀı ¬ÃÂ¯Ÿ¢äk¬QG"®iï›;Z1¯TT2V‡Âã°áÇÖ¥¶Ö£d!;ĞbÉÏ…òÕ·?Ÿé¼ïEÌ³äM65ìŞõàÀmct© ¿tgN{İMeZDä¡{UTT¥õ^	¾XNCèÉ—AûË†ÁûÈÅşL–à›üÿG–2K¸„±¶fAçxŠOÀó‡ éÍ¹Æ¢u¾mîuvn/ºÄHæ‡òp·çÉÕ­>—T8dj®~UÉ#:Îœ(¯cŠ‘%üây8Gi§¾åXø÷ï©ßœoúiİ•1ÎõHéì«ÌäæÀB”'U\û70‰Ë{w·,¨­•'š’«Óÿ+I5´6¬1ÉJUöÅdª¨./ñzãAt(âìøQÂÎ©ÕÍ%±Ö"f¤ÙæaÇ»c+Ó š›şpP€~ 
(SÆ:ô™7Šàø›£O2IRç_ÖN³NØyV8‚}ï…AAÚfßŠQLİŞ‘’Ó‚ò³s`ºÙle©9EŸJZĞHˆV"X«ûrk‹o›V:ªÜÿVNñş50Š®a¾×ÌğVÁ#ßÍ\öq@«Õ¶¨9õ—İ“kñ½ÿPvİİımïÆN^“¼Ì“7L~Î„'µ½”^ù¢9hèôó Ö#D²ÈqyÅI€OYq¾1Œ(À¢–Æè¦ëƒ	£¼ñ«æîP8‹æ”c²Æ:?nPGN/õA5Ê!…u"ÖÌ»î~ù1îó0šöÇ…¬1º:#ÂïÿÔê<…æd’_‰Õ p8ÚVFç¤kÏÌ/™Ñ ëYnïTÙÒ¹æQ1éÒQÁÊ?·>‡
ÕægÕB’XGÑ\Ú¼~XûœÒ1ğíy2~lpQPæ^x° »‹ş ^¦göZi©2\†·Y8lµƒ®æGˆÒ+®q·Ñu‡%¯ÄP±Y¥®DøUŒÛˆ6oòIÓ"u€9xqÃÑòU
4º‹¦Š\tÎ7l'/™•9)Ña#Y;IİICV?ïD¢òÛôg¯uO{
½¨ÌÖ<ØşI§Lv%V/œÊòB÷ï(ØÈı¿ÙW¹=°N„ÖZŸ ì©‹“èÕIÑpHˆZ"±ÖGÅ3z¤ìo –v„µÓ s¾‘{8@Ş&?\‘ªéó*ÜqÜìxÜ=v¬×ÿAìãÂò‰Ù6½ğwíR£=kkjâ<@º Æ!%ÛHÚ·¶„…öiŸD™Ö&"kåV¹ÁEf8öXAHx˜\¾Ç½Øò¥#1Qk_8dnyÔör3U·Ô½‘¤•wbmf ÿú“±?&€Y7¡Ú½K&µdá½«|ß­b^,¸EŒµªhëş•6k*x¹É­ê‰è1ü_%"]7Úmı‡ä.ê,lZò‡ÇC‘¬o„‚8¨ü/îØ»0V©Ğªİ–5öâşÑ¿ +JE‡“Yi³7~»¯Oc¡êkrŸé§`V'|KŞ3*hA<º}˜ŞùLå×sÅˆºÿ=éŒã%™ˆZ@¾‘™BpÁv8§ØRÌ|+vÌ½]µ·ñoª‘2ÿ}yùÙ€XÍ0Òİ-å.PØ<Ç®\0)	}»E´NG<Tl(¸èù?¬[¬î¡Ú™Ç¨TË=7T6$å´yèúnÕB,àËh´pÚĞç‡ñ¶$a+E—¹Dd‘RöIÜÙ:<`æŒâì]Ék›>—-¸ÚĞ_xw•û£á6˜G™wÉé¸P>¾èIâ<Ët­~~Qh½Ô§r²‡Vi/G»ÇÏ”ôzõæT×	T àŠ^{ã’Ò`xíŸcP´Í{©GğZé–äS‹;Œ£NÑÒİC;ªbuÎÁ,T@E­–ë3İÊN÷Xûì kË(D0Mù~Ş]ié-'NKİ¤@†U>ä´|;d$ãøÖÃABúHíi`Îd‰Ãv~ÑDĞƒÂ¸?   ‹t$ÇD$$    )Ğ‹u 9Ø‰÷NØ¡ u ‰T$‰\$‹\$‰D$9Ã×‚Ï  ´&    v ‹\$‹D$1ö‹T$‹|$‰Ù‹l$¥ÂÓàöÃ EĞEÆ‰é9ÇÑ‚B  )Ç‹5u ‰Ù‰|$‹=u Õ‰t$Lt ‰|$ Pt €t ¸ Êš;„t ¥÷1ÛÓæöÁ ‹Xt ‰l$EşEó5\t =`t 1Ò1í‰5\t ¥ÂÓàöÁ ‰ùEĞEÅ‰=`t 9Æ‰$Ñ‰T$‚   1í´&    v ¡¨t ‹¬t +4$‹8u |$ƒÀ‰5\t ƒÒ ‰=`t £¨t ‰¬t …É…Ã  èüÿÿÿ‰Ã…À…Ä  ‹5\t ‹=`t ‹$‹T$‰û9ÆÓs™¡u ‹u 	l$$‰D$¡ u ‰T$ ‹u ‰D$‰T$¶L$¡u 1Û‹u ¥ÂÓàöÁ ¶Œt EĞEÃt 