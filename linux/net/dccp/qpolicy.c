ass change for a non user-defined clamp value. */
	if (likely(!(attr->sched_flags & SCHED_FLAG_UTIL_CLAMP)) &&
	    !uc_se->user_defined)
		return true;

	/* Reset on sched_util_{min,max} == -1. */
	if (clamp_id == UCLAMP_MIN &&
	    attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN &&
	    attr->sched_util_min == -1) {
		return true;
	}

	if (clamp_id == UCLAMP_MAX &&
	    attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX &&
	    attr->sched_util_max == -1) {
		return true;
	}

	return false;
}

static void __setscheduler_uclamp(struct task_struct *p,
				  const struct sched_attr *attr)
{
	enum uclamp_id clamp_id;

	for_each_clamp_id(clamp_id) {
		struct uclamp_se *uc_se = &p->uclamp_req[clamp_id];
		unsigned int value;

		if (!uclamp_reset(attr, clamp_id, uc_se))
			continue;

		/*
		 * RT by default have a 100% boost value that could be modified
		 * at runtime.
		 */
		if (unlikely(rt_task(p) && clamp_id == UCLAMP_MIN))
			value = sysctl_sched_uclamp_util_min_rt_default;
		else
			value = uclamp_none(clamp_id);

		uclamp_se_set(uc_se, value, false);

	}

	if (likely(!(attr->sched_flags & SCHED_FLAG_UTIL_CLAMP)))
		return;

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN &&
	    attr->sched_util_min != -1) {
		uclamp_se_set(&p->uclamp_req[UCLAMP_MIN],
			      attr->sched_util_min, true);
	}

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX &&
	    attr->sched_util_max != -1) {
		uclamp_se_set(&p->uclamp_req[UCLAMP_MAX],
			      attr->sched_util_max, true);
	}
}

static void uclamp_fork(struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * We don't need to hold task_rq_lock() when updating p->uclamp_* here
	 * as the task is still at its early fork stages.
	 */
	for_each_clamp_id(clamp_id)
		p->uclamp[clamp_id].active = false;

	if (likely(!p->sched_reset_on_fork))
		return;

	for_each_clamp_id(clamp_id) {
		uclamp_se_set(&p->uclamp_req[clamp_id],
			      uclamp_none(clamp_id), false);
	}
}

static void uclamp_post_fork(struct task_struct *p)
{
	uclamp_update_util_min_rt_default(p);
}

static void __init init_uclamp_rq(struct rq *rq)
{
	enum uclamp_id clamp_id;
	struct uclamp_rq *uc_rq = rq->uclamp;

	for_each_clamp_id(clamp_id) {
		uc_rq[clamp_id] = (struct uclamp_rq) {
			.value = uclamp_none(clamp_id)
		};
	}

	rq->uclamp_flags = UCLAMP_FLAG_IDLE;
}

static void __init init_uclamp(void)
{
	struct uclamp_se uc_max = {};
	enum uclamp_id clamp_id;
	int cpu;

	for_each_possible_cpu(cpu)
		init_uclamp_rq(cpu_rq(cpu));

	for_each_clamp_id(clamp_id) {
		uclamp_se_set(&init_task.uclamp_req[clamp_id],
			      uclamp_none(clamp_id), false);
	}

	/* System defaults allow max clamp values for both indexes */
	uclamp_se_set(&uc_max, uclamp_none(UCLAMP_MAX), false);
	for_each_clamp_id(clamp_id) {
		uclamp_default[clamp_id] = uc_max;
#ifdef CONFIG_UCLAMP_TASK_GROUP
		root_task_group.uclamp_req[clamp_id] = uc_max;
		root_task_group.uclamp[clamp_id] = uc_max;
#endif
	}
}

#else /* CONFIG_UCLAMP_TASK */
static inline void uclamp_rq_inc(struct rq *rq, struct task_struct *p) { }
static inline void uclamp_rq_dec(struct rq *rq, struct task_struct *p) { }
static inline int uclamp_validate(struct task_struct *p,
				  const struct sched_attr *attr)
{
	return -EOPNOTSUPP;
}
static void __setscheduler_uclamp(struct task_struct *p,
				  const struct sched_attr *attr) { }
static inline void uclamp_fork(struct task_struc