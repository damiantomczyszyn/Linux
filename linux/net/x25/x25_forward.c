E(uc_rq->value))
		WRITE_ONCE(uc_rq->value, uc_se->value);
}

/*
 * When a task is dequeued from a rq, the clamp bucket refcounted by the task
 * is released. If this is the last task reference counting the rq's max
 * active clamp value, then the rq's clamp value is updated.
 *
 * Both refcounted tasks and rq's cached clamp values are expected to be
 * always valid. If it's detected they are not, as defensive programming,
 * enforce the expected state and warn.
 */
static inline void uclamp_rq_dec_id(struct rq *rq, struct task_struct *p,
				    enum uclamp_id clamp_id)
{
	struct uclamp_rq *uc_rq = &rq->uclamp[clamp_id];
	struct uclamp_se *uc_se = &p->uclamp[clamp_id];
	struct uclamp_bucket *bucket;
	unsigned int bkt_clamp;
	unsigned int rq_clamp;

	lockdep_assert_rq_held(rq);

	/*
	 * If sched_uclamp_used was enabled after task @p was enqueued,
	 * we could end up with unbalanced call to uclamp_rq_dec_id().
	 *
	 * In this case the uc_se->active flag should be false since no uclamp
	 * accounting was performed at enqueue time and we can just return
	 * here.
	 *
	 * Need to be careful of the following enqueue/dequeue ordering
	 * problem too
	 *
	 *	enqueue(taskA)
	 *	// sched_uclamp_used gets enabled
	 *	enqueue(taskB)
	 *	dequeue(taskA)
	 *	// Must not decrement bucket->tasks here
	 *	dequeue(taskB)
	 *
	 * where we could end up with stale data in uc_se and
	 * bucket[uc_se->bucket_id].
	 *
	 * The following check here eliminates the possibility of such race.
	 */
	if (unlikely(!uc_se->active))
		return;

	bucket = &uc_rq->bucket[uc_se->bucket_id];

	SCHED_WARN_ON(!bucket->tasks);
	if (likely(bucket->tasks))
		bucket->tasks--;

	uc_se->active = false;

	/*
	 * Keep "local max aggregation" simple and accept to (possibly)
	 * overboost some RUNNABLE tasks in the same bucket.
	 * The rq clamp bucket value is reset to its base value whenever
	 * there are no more RUNNABLE tasks refcounting it.
	 */
	if (likely(bucket->tasks))
		return;

	rq_clamp = READ_ONCE(uc_rq->value);
	/*
	 * Defensive programming: this should never happen. If it happens,
	 * e.g. due to future modification, warn and fixup the expected value.
	 */
	SCHED_WARN_ON(bucket->value > rq_clamp);
	if (bucket->value >= rq_clamp) {
		bkt_clamp = uclamp_rq_max_value(rq, clamp_id, uc_se->value);
		WRITE_ONCE(uc_rq->value, bkt_clamp);
	}
}

static inline void uclamp_rq_inc(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_inc_id(rq, p, clamp_id);

	/* Reset clamp idle holding when there is one RUNNABLE task */
	if (rq->uclamp_flags & UCLAMP_FLAG_IDLE)
		rq->uclamp_flags &= ~UCLAMP_FLAG_IDLE;
}

static inline void uclamp_rq_dec(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_dec_id(rq, p, clamp_id);
}

static in