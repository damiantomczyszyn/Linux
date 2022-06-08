_irqsave(&task_group_lock, flags);
	list_add_rcu(&tg->list, &task_groups);

	/* Root should already exist: */
	WARN_ON(!parent);

	tg->parent = parent;
	INIT_LIST_HEAD(&tg->children);
	list_add_rcu(&tg->siblings, &parent->children);
	spin_unlock_irqrestore(&task_group_lock, flags);

	online_fair_sched_group(tg);
}

/* rcu callback to free various structures associated with a task group */
static void sched_unregister_group_rcu(struct rcu_head *rhp)
{
	/* Now it should be safe to free those cfs_rqs: */
	sched_unregister_group(container_of(rhp, struct task_group, rcu));
}

void sched_destroy_group(struct task_group *tg)
{
	/* Wait for possible concurrent references to cfs_rqs complete: */
	call_rcu(&tg->rcu, sched_unregister_group_rcu);
}

void sched_release_group(struct task_group *tg)
{
	unsigned long flags;

	/*
	 * Unlink first, to avoid walk_tg_tree_from() from finding us (via
	 * sched_cfs_period_timer()).
	 *
	 * For this to be effective, we have to wait for all pending users of
	 * this task group to leave their RCU critical section to ensure no new
	 * user will see our dying task group any more. Specifically ensure
	 * that tg_unthrottle_up() won't add decayed cfs_rq's to it.
	 *
	 * We therefore defer calling unregister_fair_sched_group() to
	 * sched_unregister_group() which is guarantied to get called only after the
	 * current RCU grace period has expired.
	 */
	spin_lock_irqsave(&task_group_lock, flags);
	list_del_rcu(&tg->list);
	list_del_rcu(&tg->siblings);
	spin_unlock_irqrestore(&task_group_lock, flags);
}

static void sched_change_group(struct task_struct *tsk, int type)
{
	struct task_group *tg;

	/*
	 * All callers are synchronized by task_rq_lock(); we do not use RCU
	 * which is pointless here. Thus, we pass "true" to task_css_check()
	 * to prevent lockdep warnings.
	 */
	tg = container_of(task_css_check(tsk, cpu_cgrp_id, true),
			  struct task_group, css);
	tg = autogroup_task_group(tsk, tg);
	tsk->sched_task_group = tg;

#ifdef CONFIG_FAIR_GROUP_SCHED
	if (tsk->sched_class->task_change_group)
		tsk->sched_class->task_change_group(tsk, type);
	else
#endif
		set_task_rq(tsk, task_cpu(tsk));
}

/*
 * Change task's runqueue when it moves between groups.
 *
 * The caller of this function should have put the task in its new group by
 * now. This function just updates tsk->se.cfs_rq and tsk->se.parent to reflect
 * its new group.
 */
void sched_move_task(struct task_struct *tsk)
{
	int queued, running, queue_flags =
		DEQUEUE_SAVE | DEQUEUE_MOVE | DEQUEUE_NOCLOCK;
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(tsk, &rf);
	update_rq_clock(rq);

	running = task_current(rq, tsk);
	queued = task_on_rq_queued(tsk);

	if (queued)
		dequeue_task(rq, tsk, queue_flags);
	if (running)
		put_prev_task(rq, tsk);

	sched_change_group(tsk, TASK_MOVE_GROUP);

	if (queued)
		enqueue_task(rq, tsk, queue_flags);
	if (running) {
		set_next_task(rq, tsk);
		/*
		 * After changing group, the running task may have joined a
		 * throttled one but it's still the running task. Trigger a
		 * resched to make sure that task can still run.
		 */
		resched_curr(rq);
	}

	task_rq_unlock(rq, tsk, &rf);
}

static inline struct task_group *css_tg(struct cgroup_subsys_state *css)
{
	return css ? container_of(css, struct task_group, css) : NULL;
}

static struct cgroup_subsys_state *
cpu_cgroup_css_alloc(struct cgroup_subsys_state *parent_css)
{
	struct task_group *parent = css_tg(parent_css);
	struct task_group *tg;

	if (!parent) {
		/* This is early initialization for the top cgroup */
		return &root_task_group.css;
	}

	tg = sched_create_group(parent);
	if (IS_ERR(tg))
		return ERR_PTR(-ENOMEM);

	return &tg->css;
}

/* Expose task group only after completing cgroup initialization */
static int cpu_cgroup_css_online(struct cgroup_subsys_state *css)
{
	struct task_group *tg = css_tg(css);
	struct task_group *parent = css_tg(css->parent);

	if (parent)
		sched_online_group(tg, parent);

#ifdef CONFIG_UCLAMP_TASK_GROUP
	/* Propagate the effective uclamp value for the new group */
	mutex_lock(&uclamp_mutex);
	rcu_read_lock();
	cpu_util_update_eff(css);
	rcu_read_unlock();
	mutex_unlock(&uclamp_mutex);
#endif

	return 0;
}

static void cpu_cgroup_css_released(struct cgroup_subsys_state *css)
{
	struct task_group *tg = css_tg(css);

	sched_release_group(tg);
}

static void cpu_cgroup_css_free(struct cgroup_subsys_state *css)
{
	struct task_group *tg = css_tg(css);

	/*
	 * Relies on the RCU grace period between css_released() and this.
	 */
	sched_unregister_group(tg);
}

/*
 * This is called before wake_up_new_task(), therefore we really only
 * have to set its group bits, all the other stuff does not apply.
 */
static void cpu_cgroup_fork(struct task_struct *task)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(task, &rf);

	update_rq_clock(rq);
	sched_change_group(task, TASK_SET_GROUP);

	task_rq_unlock(rq, task, &rf);
}

static int cpu_cgroup_can_attach(struct cgroup_taskset *tset)
{
	struct task_struct *task;
	struct cgroup_subsys_state *css;
	int ret = 0;

	cgroup_taskset_for_each(task, css, tset) {
#ifdef CONFIG_RT_GROUP_SCHED
		if (!sched_rt_can_attach(css_tg(css), task))
			return -EINVAL;
#endif
		/*
		 * Serialize against wake_up_new_task() such that if it's
		 * running, we're sure to observe its full state.
		 */
		raw_spin_lock_irq(&task->pi_lock);
		/*
		 * Avoid calling sched_move_task() before wake_up_new_task()
		 * has happened. This would lead to problems with PELT, due to
		 * move wanting to detach+attach while we're not attached yet.
		 */
		if (READ_ONCE(task->__state) == TASK_NEW)
			ret = -EINVAL;
		raw_spin_unlock_irq(&task->pi_lock);

		if (ret)
			break;
	}
	return ret;
}

static void cpu_cgroup_attach(struct cgroup_taskset *tset)
{
	struct task_struct *task;
	struct cgroup_subsys_state *css;

	cgroup_taskset_for_each(task, css, tset)
		sched_move_task(task);
}

#ifdef CONFIG_UCLAMP_TASK_GROUP
static void cpu_util_update_eff(struct cgroup_subsys_state *css)
{
	struct cgroup_subsys_state *top_css = css;
	struct uclamp_se *uc_parent = NULL;
	struct uclamp_se *uc_se = NULL;
	unsigned int eff[UCLAMP_CNT];
	enum uclamp_id clamp_id;
	unsigned int clamps;

	lockdep_assert_held(&uclamp_mutex);
	SCHED_WARN_ON(!rcu_read_lock_held());

	css_for_each_descendant_pre(css, top_css) {
		uc_parent = css_tg(css)->parent
			? css_tg(css)->parent->uclamp : NULL;

		for_each_clamp_id(clamp_id) {
			/* Assume effective clamps matches requested clamps */
			eff[clamp_id] = css_tg(css)->uclamp_req[clamp_id].value;
			/* Cap effective clamps with parent's effective clamps */
			if (uc_parent &&
			    eff[clamp_id] > uc_parent[clamp_id].value) {
				eff[clamp_id] = uc_parent[clamp_id].value;
			}
		}
		/* Ensure protection is always capped by limit */
		eff[UCLAMP_MIN] = min(eff[UCLAMP_MIN], eff[UCLAMP_MAX]);

		/* Propagate most restrictive effective clamps */
		clamps = 0x0;
		uc_se = css_tg(css)->uclamp;
		for_each_clamp_id(clamp_id) {
			if (eff[clamp_id] == uc_se[clamp_id].value)
				continue;
			uc_se[clamp_id].value = eff[clamp_id];
			uc_se[clamp_id].bucket_id = uclamp_bucket_id(eff[clamp_id]);
			clamps |= (0x1 << clamp_id);
		}
		if (!clamps) {
			css = css_rightmost_descendant(css);
			continue;
		}

		/* Immediately update descendants RUNNABLE tasks */
		uclamp_update_active_tasks(css);
	}
}

/*
 * Integer 10^N with a given N exponent by casting to integer the literal "1eN"
 * C expression. Since there is no way to convert a macro argument (N) into a
 * character constant, use two levels of macros.
 */
#define _POW10(exp) ((unsigned int)1e##exp)
#define POW10(exp) _POW10(exp)

struct uclamp_request {
#define UCLAMP_PERCENT_SHIFT	2
#define UCLAMP_PERCENT_SCALE	(100 * POW10(UCLAMP_PERCENT_SHIFT))
	s64 percent;
	u64 util;
	int ret;
};

static inline struct uclamp_request
capacity_from_percent(char *buf)
{
	struct uclamp_request req = {
		.percent = UCLAMP_PERCENT_SCALE,
		.util = SCHED_CAPACITY_SCALE,
		.ret = 0,
	};

	buf = strim(buf);
	if (strcmp(buf, "max")) {
		req.ret = cgroup_parse_float(buf, UCLAMP_PERCENT_SHIFT,
					     &req.percent);
		if (req.ret)
			return req;
		if ((u64)req.percent > UCLAMP_PERCENT_SCALE) {
			req.ret = -ERANGE;
			return req;
		}

		req.util = req.percent << SCHED_CAPACITY_SHIFT;
		req.util = DIV_ROUND_CLOSEST_ULL(req.util, UCLAMP_PERCENT_SCALE);
	}

	return req;
}

static ssize_t cpu_uclamp_write(struct kernfs_open_file *of, char *buf,
				size_t nbytes, loff_t off,
				enum uclamp_id clamp_id)
{
	struct uclamp_request req;
	struct task_group *tg;

	req = capacity_from_percent(buf);
	if (req.ret)
		return req.ret;

	static_branch_enable(&sched_uclamp_used);

	mutex_lock(&uclamp_mutex);
	rcu_read_lock();

	tg = css_tg(of_css(of));
	if (tg->uclamp_req[clamp_id].value != req.util)
		uclamp_se_set(&tg->uclamp_req[clamp_id], req.util, false);

	/*
	 * Because of not recoverable conversion rounding we keep track of the
	 * exact requested value
	 */
	tg->uclamp_pct[clamp_id] = req.percent;

	/* Update effective clamps to track the most restrictive value */
	cpu_util_update_eff(of_css(of));

	rcu_read_unlock();
	mutex_unlock(&uclamp_mutex);

	return nbytes;
}

static ssize_t cpu_uclamp_min_write(struct kernfs_open_file *of,
				    char *buf, size_t nbytes,
				    loff_t off)
{
	return cpu_uclamp_write(of, buf, nbytes, off, UCLAMP_MIN);
}

static ssize_t cpu_uclamp_max_write(struct kernfs_open_file *of,
				    char *buf, size_t nbytes,
				    loff_t off)
{
	return cpu_uclamp_write(of, buf, nbytes, off, UCLAMP_MAX);
}

static inline void cpu_uclamp_print(struct seq_file *sf,
				    enum uclamp_id clamp_id)
{
	struct task_group *tg;
	u64 util_clamp;
	u64 percent;
	u32 rem;

	rcu_read_lock();
	tg = css_tg(seq_css(sf));
	util_clamp = tg->uclamp_req[clamp_id].value;
	rcu_read_unlock();

	if (util_clamp == SCHED_CAPACITY_SCALE) {
		seq_puts(sf, "max\n");
		return;
	}

	percent = tg->uclamp_pct[clamp_id];
	percent = div_u64_rem(percent, POW10(UCLAMP_PERCENT_SHIFT), &rem);
	seq_printf(sf, "%llu.%0*u\n", percent, UCLAMP_PERCENT_SHIFT, rem);
}

static int cpu_uclamp_min_show(struct seq_file *sf, void *v)
{
	cpu_uclamp_print(sf, UCLAMP_MIN);
	return 0;
}

static int cpu_uclamp_max_show(struct seq_file *sf, void *v)
{
	cpu_uclamp_print(sf, UCLAMP_MAX);
	return 0;
}
#endif /* CONFIG_UCLAMP_TASK_GROUP */

#ifdef CONFIG_FAIR_GROUP_SCHED
static int cpu_shares_write_u64(struct cgroup_subsys_state *css,
				struct cftype *cftype, u64 shareval)
{
	if (shareval > scale_load_down(ULONG_MAX))
		shareval = MAX_SHARES;
	return sched_group_set_shares(css_tg(css), scale_load(shareval));
}

static u64 cpu_shares_read_u64(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	struct task_group *tg = css_tg(css);

	return (u64) scale_load_down(tg->shares);
}

#ifdef CONFIG_CFS_BANDWIDTH
static DEFINE_MUTEX(cfs_constraints_mutex);

const u64 max_cfs_quota_period = 1 * NSEC_PER_SEC; /* 1s */
static const u64 min_cfs_quota_period = 1 * NSEC_PER_MSEC; /* 1ms */
/* More than 203 days if BW_SHIFT equals 20. */
static const u64 max_cfs_runtime = MAX_BW * NSEC_PER_USEC;

static int __cfs_schedulable(struct task_group *tg, u64 period, u64 runtime);

static int tg_set_cfs_bandwidth(struct task_group *tg, u64 period, u64 quota,
				u64 burst)
{
	int i, ret = 0, runtime_enabled, runtime_was_enabled;
	struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;

	if (tg == &root_task_group)
		return -EINVAL;

	/*
	 * Ensure we have at some amount of bandwidth every period.  This is
	 * to prevent reaching a state of large arrears when throttled via
	 * entity_tick() resulting in prolonged exit starvation.
	 */
	if (quota < min_cfs_quota_period || period < min_cfs_quota_period)
		return -EINVAL;

	/*
	 * Likewise, bound things on the other side by preventing insane quota
	 * periods.  This also allows us to normalize in computing quota
	 * feasibility.
	 */
	if (period > max_cfs_quota_period)
		return -EINVAL;

	/*
	 * Bound quota to defend quota against overflow during bandwidth shift.
	 */
	if (quota != RUNTIME_INF && quota > max_cfs_runtime)
		return -EINVAL;

	if (quota != RUNTIME_INF && (burst > quota ||
				     burst + quota > max_cfs_runtime))
		return -EINVAL;

	/*
	 * Prevent race between setting of cfs_rq->runtime_enabled and
	 * unthrottle_offline_cfs_rqs().
	 */
	cpus_read_lock();
	mutex_lock(&cfs_constraints_mutex);
	ret = __cfs_schedulable(tg, period, quota);
	if (ret)
		goto out_unlock;

	runtime_enabled = quota != RUNTIME_INF;
	runtime_was_enabled = cfs_b->quota != RUNTIME_INF;
	/*
	 * If we need to toggle cfs_bandwidth_used, off->on must occur
	 * before making related changes, and on->off must occur afterwards
	 */
	if (runtime_enabled && !runtime_was_enabled)
		cfs_bandwidth_usage_inc();
	raw_spin_lock_irq(&cfs_b->lock);
	cfs_b->period = ns_to_ktime(period);
	cfs_b->quota = quota;
	cfs_b->burst = burst;

	__refill_cfs_bandwidth_runtime(cfs_b);

	/* Restart the period timer (if active) to handle new period expiry: */
	if (runtime_enabled)
		start_cfs_bandwidth(cfs_b);

	raw_spin_unlock_irq(&cfs_b->lock);

	for_each_online_cpu(i) {
		struct cfs_rq *cfs_rq = tg->cfs_rq[i];
		struct rq *rq = cfs_rq->rq;
		struct rq_flags rf;

		rq_lock_irq(rq, &rf);
		cfs_rq->runtime_enabled = runtime_enabled;
		cfs_rq->runtime_remaining = 0;

		if (cfs_rq->throttled)
			unthrottle_cfs_rq(cfs_rq);
		rq_unlock_irq(rq, &rf);
	}
	if (runtime_was_enabled && !runtime_enabled)
		cfs_bandwidth_usage_dec();
out_unlock:
	mutex_unlock(&cfs_constraints_mutex);
	cpus_read_unlock();

	return ret;
}

static int tg_set_cfs_quota(struct task_group *tg, long cfs_quota_us)
{
	u64 quota, period, burst;

	period = ktime_to_ns(tg->cfs_bandwidth.period);
	burst = tg->cfs_bandwidth.burst;
	if (cfs_quota_us < 0)
		quota = RUNTIME_INF;
	else if ((u64)cfs_quota_us <= U64_MAX / NSEC_PER_USEC)
		quota = (u64)cfs_quota_us * NSEC_PER_USEC;
	else
		return -EINVAL;

	return tg_set_cfs_bandwidth(tg, period, quota, burst);
}

static long tg_get_cfs_quota(struct task_group *tg)
{
	u64 quota_us;

	if (tg->cfs_bandwidth.quota == RUNTIME_INF)
		return -1;

	quota_us = tg->cfs_bandwidth.quota;
	do_div(quota_us, NSEC_PER_USEC);

	return quota_us;
}

static int tg_set_cfs_period(struct task_group *tg, long cfs_period_us)
{
	u64 quota, period, burst;

	if ((u64)cfs_period_us > U64_MAX / NSEC_PER_USEC)
		return -EINVAL;

	period = (u64)cfs_period_us * NSEC_PER_USEC;
	quota = tg->cfs_bandwidth.quota;
	burst = tg->cfs_bandwidth.burst;

	return tg_set_cfs_bandwidth(tg, period, quota, burst);
}

static long tg_get_cfs_period(struct task_group *tg)
{
	u64 cfs_period_us;

	cfs_period_us = ktime_to_ns(tg->cfs_bandwidth.period);
	do_div(cfs_period_us, NSEC_PER_USEC);

	return cfs_period_us;
}

static int tg_set_cfs_burst(struct task_group *tg, long cfs_burst_us)
{
	u64 quota, period, burst;

	if ((u64)cfs_burst_us > U64_MAX / NSEC_PER_USEC)
		return -EINVAL;

	burst = (u64)cfs_burst_us * NSEC_PER_USEC;
	period = ktime_to_ns(tg->cfs_bandwidth.period);
	quota = tg->cfs_bandwidth.quota;

	return tg_set_cfs_bandwidth(tg, period, quota, burst);
}

static long tg_get_cfs_burst(struct task_group *tg)
{
	u64 burst_us;

	burst_us = tg->cfs_bandwidth.burst;
	do_div(burst_us, NSEC_PER_USEC);

	return burst_us;
}

static s64 cpu_cfs_quota_read_s64(struct cgroup_subsys_state *css,
				  struct cftype *cft)
{
	return tg_get_cfs_quota(css_tg(css));
}

static int cpu_cfs_quota_write_s64(struct cgroup_subsys_state *css,
				   struct cftype *cftype, s64 cfs_quota_us)
{
	return tg_set_cfs_quota(css_tg(css), cfs_quota_us);
}

static u64 cpu_cfs_period_read_u64(struct cgroup_subsys_state *css,
				   struct cftype *cft)
{
	return tg_get_cfs_period(css_tg(css));
}

static int cpu_cfs_period_write_u64(struct cgroup_subsys_state *css,
				    struct cftype *cftype, u64 cfs_period_us)
{
	return tg_set_cfs_period(css_tg(css), cfs_period_us);
}

static u64 cpu_cfs_burst_read_u64(struct cgroup_subsys_state *css,
				  struct cftype *cft)
{
	return tg_get_cfs_burst(css_tg(css));
}

static int cpu_cfs_burst_write_u64(struct cgroup_subsys_state *css,
				   struct cftype *cftype, u64 cfs_burst_us)
{
	return tg_set_cfs_burst(css_tg(css), cfs_burst_us);
}

struct cfs_schedulable_data {
	struct task_group *tg;
	u64 period, quota;
};

/*
 * normalize group quota/period to be quota/max_period
 * note: units are usecs
 */
static u64 normalize_cfs_quota(struct task_group *tg,
			       struct cfs_schedulable_data *d)
{
	u64 quota, period;

	if (tg == d->tg) {
		period = d->period;
		quota = d->quota;
	} else {
		period = tg_get_cfs_period(tg);
		quota = tg_get_cfs_quota(tg);
	}

	/* note: these should typically be equivalent */
	if (quota == RUNTIME_INF || quota == -1)
		return RUNTIME_INF;

	return to_ratio(period, quota);
}

static int tg_cfs_schedulable_down(struct task_group *tg, void *data)
{
	struct cfs_schedulable_data *d = data;
	struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;
	s64 quota = 0, parent_quota = -1;

	if (!tg->parent) {
		quota = RUNTIME_INF;
	} else {
		struct cfs_bandwidth *parent_b = &tg->parent->cfs_bandwidth;

		quota = normalize_cfs_quota(tg, d);
		parent_quota = parent_b->hierarchical_quota;

		/*
		 * Ensure max(child_quota) <= parent_quota.  On cgroup2,
		 * always take the min.  On cgroup1, only inherit when no
		 * limit is set:
		 */
		if (cgroup_subsys_on_dfl(cpu_cgrp_subsys)) {
			quota = min(quota, parent_quota);
		} else {
			if (quota == RUNTIME_INF)
				quota = parent_quota;
			else if (parent_quota != RUNTIME_INF && quota > parent_quota)
				return -EINVAL;
		}
	}
	cfs_b->hierarchical_quota = quota;

	return 0;
}

static int __cfs_schedulable(struct task_group *tg, u64 period, u64 quota)
{
	int ret;
	struct cfs_schedulable_data data = {
		.tg = tg,
		.period = period,
		.quota = quota,
	};

	if (quota != RUNTIME_INF) {
		do_div(data.period, NSEC_PER_USEC);
		do_div(data.quota, NSEC_PER_USEC);
	}

	rcu_read_lock();
	ret = walk_tg_tree(tg_cfs_schedulable_down, tg_nop, &data);
	rcu_read_unlock();

	return ret;
}

static int cpu_cfs_stat_show(struct seq_file *sf, void *v)
{
	struct task_group *tg = css_tg(seq_css(sf));
	struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;

	seq_printf(sf, "nr_periods %d\n", cfs_b->nr_periods);
	seq_printf(sf, "nr_throttled %d\n", cfs_b->nr_throttled);
	seq_printf(sf, "throttled_time %llu\n", cfs_b->throttled_time);

	if (schedstat_enabled() && tg != &root_task_group) {
		struct sched_statistics *stats;
		u64 ws = 0;
		int i;

		for_each_possible_cpu(i) {
			stats = __schedstats_from_se(tg->se[i]);
			ws += schedstat_val(stats->wait_sum);
		}

		seq_printf(sf, "wait_sum %llu\n", ws);
	}

	seq_printf(sf, "nr_bursts %d\n", cfs_b->nr_burst);
	seq_printf(sf, "burst_time %llu\n", cfs_b->burst_time);

	return 0;
}
#endif /* CONFIG_CFS_BANDWIDTH */
#endif /* CONFIG_FAIR_GROUP_SCHED */

#ifdef CONFIG_RT_GROUP_SCHED
static int cpu_rt_runtime_write(struct cgroup_subsys_state *css,
				struct cftype *cft, s64 val)
{
	return sched_group_set_rt_runtime(css_tg(css), val);
}

static s64 cpu_rt_runtime_read(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	return sched_group_rt_runtime(css_tg(css));
}

static int cpu_rt_period_write_uint(struct cgroup_subsys_state *css,
				    struct cftype *cftype, u64 rt_period_us)
{
	return sched_group_set_rt_period(css_tg(css), rt_period_us);
}

static u64 cpu_rt_period_read_uint(struct cgroup_subsys_state *css,
				   struct cftype *cft)
{
	return sched_group_rt_period(css_tg(css));
}
#endif /* CONFIG_RT_GROUP_SCHED */

#ifdef CONFIG_FAIR_GROUP_SCHED
static s64 cpu_idle_read_s64(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	return css_tg(css)->idle;
}

static int cpu_idle_write_s64(struct cgroup_subsys_state *css,
				struct cftype *cft, s64 idle)
{
	return sched_group_set_idle(css_tg(css), idle);
}
#endif

static struct cftype cpu_legacy_files[] = {
#ifdef CONFIG_FAIR_GROUP_SCHED
	{
		.name = "shares",
		.read_u64 = cpu_shares_read_u64,
		.write_u64 = cpu_shares_write_u64,
	},
	{
		.name = "idle",
		.read_s64 = cpu_idle_read_s64,
		.write_s64 = cpu_idle_write_s64,
	},
#endif
#ifdef CONFIG_CFS_BANDWIDTH
	{
		.name = "cfs_quota_us",
		.read_s64 = cpu_cfs_quota_read_s64,
		.write_s64 = cpu_cfs_quota_write_s64,
	},
	{
		.name = "cfs_period_us",
		.read_u64 = cpu_cfs_period_read_u64,
		.write_u64 = cpu_cfs_period_write_u64,
	},
	{
		.name = "cfs_burst_us",
		.read_u64 = cpu_cfs_burst_read_u64,
		.write_u64 = cpu_cfs_burst_write_u64,
	},
	{
		.namELF                      p'      4     (                èüÿÿÿ…Àt?S‰Ã‹€ˆ  …Àt‹…Òt
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
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           ¤ÿ     À           ¤ÿ    ×    s           GNU  À       À               GNU p¢7ğòœ°J©ö=O"í&ù•        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 ñÿ       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 ‰            ˜   U   ?     §            ¾           Õ      #     ğ   =          O   Y       ¨        4           B  8       	 T           ñÿd          y  À         Ğ  %     Š          £    À     s   d       	 Ä  °  R     ß    ©    ù  €   @    	   Ä  0     +  H       	 6  @       	 A  P       	 L  »   -     e  è        ‚          ”  À   
    	 ª           ñÿº  ÿ   #     Ó  "       é  ;         G  	       P       )  _  7     @           H          P             \           j             s                          ‘             £             ±  U   ?     À             Ç             Í             ä             û             	                                      >             F             \             m             u             ‚                                       ¯             Ã             ß             ş                          9             A             U             c             p             ‡           —     £    ³             Ê             Ø  À  Ğ     ì             ù              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q          «   B  °   U  »     À   i  Ò   L  Ü     è     û       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  ‚  c  Š      S  ”    ¡  N  Á  N  Ñ  N  à  K  ì  Z    N          ™  I     _  Ü  R  â    ê      I  @  I  R    Z    x  R  †  e  ’    Ÿ    ±  N  Ê  V  Ó  \  â  Y  ø  a    N  #  E  C  V  m  X  ©  a  ¸  E  U  ]  o  d  €  P    a  ¦  a  µ  S  Á  N  É    Ù  E  ì  D  ù          W  &  [  I  T  V    g    p  ^  {  E  Œ  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  ‡     Œ   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     ƒ     ˆ   Q  •     ›         Q  ¨     »     Á     Æ   Q  Ó     Ø     İ   Q  ä     é     î     ó     ø   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    €  Q  ‡    Œ    ‘    –  Q      £    ª    ¯    ´    ¹  Q  À    Å    Ê  M  Ï    Ô  M  İ    ã    è  Q  ğ       N          Q     `       €     „     ”     ˜                                                          $              B     f     c          B     f  $                     Ì   O  ô  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @                       	   @       ä  ˆ              )             Ğ  ”                  %   	   @       l"  ˆ               8             d  ô                 4   	   @       ô"  H              K             X	                    G   	   @       <%  (               Z             €	  Ê                   V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       Œ%  P               ‚      2       }  +                 ‘             ¨  –                              @  (                  š   	   @       Ü%  @               ª             h                    ¦   	   @       &                 µ             t                     »             t  (                  Î             œ  $                  á             À  0                  ñ                           @       í   	   @       4&                                                       0          9                              9                                   <        A         	              Ü                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 âÅÎ°gjşD~æÄ
ÁCè÷WÛÂjGø4<*1¯ÉCvV­ŠÙïZhTpzÆŸ'ïXŸµ»Î¿ÒÕs~oå'[¢©\¡^=Å$¯òî‘0Êï˜ÅÈa`>ŸÚ
¿·™<ÿ–*f—†±,½@î¬­¤Ñf)ŒĞ–8xJµUuKwt¬†æ–w•)éifÚ“ äÈdFÎ`“[Ë¨»E¯’ ®Â¤€óÖó!L„Íÿ4rÿÆi1gºæ)-Íİst0ÑànÕó°zi=?>Øğ!ÜŞ,8Zv=Œ®t#Lb6‘®*¨”«EÒ:M®;å¥ÊK©±FVûTO<'oˆ[ÖŸö‚@ªÎ£]-3ıpïtÉ¯İ0`ÄÓ¤Dâ›Nr(Öç¥KAÔ¬JNçãu«†ÚF>Wš,©ˆ|í-O|U8SÙ	¯lÙ#ÿ¦MİªxSJZlü®JöÚ{xpôh† <É«R¸ë¿Zïÿ6•è°gM»‹“³2ìŸ‰•É”+BıòX*%ºyÀyc{Ä¤î•Fñ‡3U8ïºq¼½K#V…1u3yŠ}§p‡´=µüFaÒ´¸6&*ÎX±®u·µftˆYáÕ¾Gë`Å}¡Œ‰Îó$W¸Å”Œ³°zxÍCìÅûê.H­\íÑsTZÆ¦]Ú¢Éšk¹•UÔèXÎÜ`èTñ®º—€w®u£c:ªu‘Š-¥á2w6ú•›=Î9UÈÒÈçvÃõsŸ—”Ô.Q’³¢³w?·(¹Â(Èÿ	:Ø+KŒÍ1ez[¹L©¢EpWŸszlå›r/ƒ•äE'ó '
nıS>^š\XÆ¹ùO‡>û¿:¾°C—ôèP–ïÛ(÷(k"Àò›%íïĞ$¬ûqîªÈI-‡A7nŸ•å!·Ûš÷š@ÊÂ*½†¸ÌêÂ§0ädHß×|ÜÑ£Äqë/ÜÏ”İ¡{«io½ïíÏ(ëÌ0:ëé§0¾ˆ+kFáÆÜkWİænê¼¸“ßz­„)9¹†«ÚÇöá7x÷„pÚñ!WpÊ¹êï#Dìw#,–“ dÌk31ÈˆKPN)—IÖÈğæ~<­)’@ôOäıu%ÔNõpùœÄªˆ*,{Œ"W¨¡rø+İéÂ/*JK”fıõæ¨ Ó›d; 	´¦dq+äÕSëm æxóœ;:Ø.]¢ƒUò¿(@õ\vNÙè¸ä2Ä<>ÏlƒU D }:‘ŞDµëşşÙáçÉLWş)‘	óˆˆ)<hpQ@cœ@9P[Îšî¯àOV¨4â¶FoXJcÛÂ õ7mß‰ôU:ïnh½øA(ñ¢£gåM7¡ìÇp…äz<]‹dæØ9yNHv/ÁŒoD®0kØ™Fƒ˜çÿ²°LCÆËö¢ãÛ$Ğ'ÎDŒ.©l»ààÂGğ%}ŒûŒ" £/”ÕGúx5yIgİuà¨RâÔ}İä¬¿OÿÔÀÏM”‰ÄÉÃ· MêWéï!|1¾&/æÔŒ×‡Pİ™¾Äå®r@÷È÷¹ÂÿN!l,n‚ß™’tónóÅ¢ş@Êº ØÁG›@PÉ]İùû’úl&O'uÍîğ’Î+ÈÍØ]¼wB`š'ÄdƒûâØ¹²áh¾b.ğFÄTrè ±Ò5¡^’h¬ÃˆÒÃvS¶IÏ‘øˆµ<w³õº¯ÅÙ'í;9ÀÒş”r¶¯víôVÆšaIYJ”~3€ıIœÔÈy#ìA jéˆD¿PFòÇKéÄ²Ï%ÁÌOSÆôFK
AÜhVÁ y^Ü4Ş*’¾SàÈ7ö”ÎÛ·¦É­Õóü"¦¼âšâBêí+¸€§
½…Ş *¤[¥ï[ŒbøxŠ©–Ê)v2zDï‘)9,¯=öÜ|¾(Ô0¡@8ĞäŒ<8]oíO†E¨Â‚´í•Š°,Õ#-_tš—VàûdÖ&ädA„í.ëİ7Rx)dİîwËQiçQR^}î³j­¡Óô÷´ s‚q=OÕ 4ğ›Û˜ş“ü"[pßƒ8—A‡…Ñ:UrYkÀÀ°œF
´˜şRQØ`æÏòQhZfÑŠ‡“}ín&‹bL'“<§?ıjŠÄ›ñÊHÎQwfaDòjÒx:°åºÇZIª}¡,mCQXN,Ñõ¤ES×ÿdu]C0_¹h8\vz•oı%M@ggj4–ÎèazÑw5ÚÈ‰½ÏÒà˜Ÿá¦dÉµÊ~>LcÏIT,ó„œn›rò<?äGD·y£¯—İÛ;;O1I1Ù0×yf<ßÄtÖ#ÂIÀ‘3ÏßÜeH¹HiÍhŞéştÙ5¹¥ø:ŠDOÈãË`7˜`™Š³¯Ûñ­W÷\5È@Ğ»EÔËÑJZd™->d¿Ü0Ç¥4Ê®Ü¼¡¶­6ÌrÅ%)ee{ÉhüJÄû>k4"$ÃW÷jd[/ÛN¯…‰ÇLI&C73§;ßõ“a²/|êµŸàØdàûÖ7mR÷Çdò?–âT'ÂsŞåÕ‘wR|‹‚Péƒ¥ËÜ‡*(®gÆyläD÷
;t=0?À¹µk§€ uæÓ\Tv½!Ê¦â®öş"nP’óË.Z‚Ÿñï¦p…†!âƒÆ˜ğ°c¯Ov»_–€§—®Ò_=À×ÿÍS#Ó†d•&»yˆlª¥Ï†q°f‚™sùÖQş(.ìñîE£ÂXV~ˆTïzéL2Óéı†Ël™'fÜI@h4M ú¾>Qs4¶ó‹Eè²Tq¹@¢ŸÖ’.iœ4“BhæÄ…Qròëšk,Wvƒ0/FÊ2YD?“‰È±‡µ´1-¯Š]¼sìo8³^Š MR˜m“Ë)Ğßšû9C]J^E,˜Ò‡İC³]
âœòÃ«ã¤‚ÖêÔíf»Ó»µá10:Ê.ˆ;5ú×«{¨ÌáÖõş#—m§ùÏÑö4–‹ö6Çğ¨¬c7ÌÂ2R!Àû“Ë˜Éš¢”‘tŒ?Áª‡5…òßaá/ãQn¸Fo]Ÿ<¡ŒğÄ7RŒ0Õõg¾Hü*x¹´°¨öÜvW±_£ØLÖJ¤ˆ:LÇ2LM©3ÎA/ é-Î»øò®ÈãGC×‹U=Ã5wèÓq®qkôûc^qÔ9L‚ò=‹™W¯:S.R‘¨düÌ½—_èŒ… [£»]Õøq„n!¸‡´Ê 	Ú>àCÛ¨Û$j¨ÙY¦ĞŠ©:s+~‚zö®N0ÁQ² RJµ3Ú]Ğ°…ZRbÙ-vÕ‚jŠq89ÖaMÚ„—¢W¡=o@EcJRí÷ÁŠè›¶Y¬Í]©ÚDpx¬Y”ÊGübJå#Å°¤{¾"¶Û#˜È.¢Ã’äu~b†™Ü¤
¾Ñ»ªØÏÛóC^­£Ñ¦)>«z`‰bgYz§·ãØ±Ğ™é‰…Šò:‚˜$x€¦óò›]Òë¨E–Ç|Qp?ø‚üªƒôè Éš,éå¶ç?âZ²d+šÃ“TÚœñ;âL/8ó!é8óÒ‚/•ã…¶ö×ØÍÑlº_›¤Ô¤¿@½›uH“(¤<RÂ§u^éƒ~±0æ )ƒÙ[iöÔÔ	TsH±*f†ÀÍšu¸aıoÆ¸çhG¸^"šQ™…„%œ‘%]Â€zá`ûÊeF!ïœ=? Ñmv†Â¥£n+^oe½Ò‰Vª¿AÖ ‹pñn…HÖ>Lh}Ã‰ñÍ‰8Œı¿¿Ü‡«Ïç±³8ÃÔX%¹m8E¾·«i÷r÷¥¥$Q`ñPó±„+Ïº} /ó¯’%­[Æè0µ`DıŸ×§®KUÉâä~ØñĞ7JzıíœG¤WP1 `yD_]£D³ÖÉ| `Eô¿ffíÍ ­q6Îy|t}ç]ç^'™öjŠYÇXåMdN@º€2ñHôıÏš" ÖœÏ¦õ*{BĞ0øW;CÕØPœK|Í/àÙèŸ—FƒèGyÙìtÀ(íïò¯ ÌŞr3àöj.'6?ñ@Í+«·™í„ã²”·ªõY‡İ6ç©ğl—òËßwyõÕá4‰®N[ùW›÷(/ã“ÈÈ¼ÿ¬¤Ú"ü¯E=…çudf:éwµµÜª"=C}Ü(÷Wê=Ã4H9DJ:h1|Ä¾l/Ú_ÁJÙ1}­8fLÈ•’øşOˆì[*$eC^†Yuu&À›ÀÏ]i-¨WA/|Kw¶sÖêT“ë„—>\Hš˜äÚè¿ÅŠÜNÌdcÎâ];AùÖa-Ÿîƒd)Qlg®R¢$$°¬uH”yåªòG*­ì×BBˆ´`pNçƒŠe+ÊÆ¶4*Ô˜4ŠL‘ ­>¹<ˆJrÃ,]S¢û•­‹ÿ’Ù%¡ó+º)7~dXêõË‚š•üpeúXacg¡‰Ô7KîÚÕ¾À.—$Kìmxö¼!ª
¤zĞƒTMzdÖ¯u’`SÍ¶GŒ»ºáb•7I#iîuöÕ‘ øÜ‰mˆşX‹ë–Déõf!Wš5*ˆÇ2o/¤v‚V´J¢ûQµú˜Q
·²*±v±ÆÆ¯¸‘øº~Í Àz^ ûa¡Nl¹9`“cï¸QNüŸØI³7å¢XŞÈo­e|F!¼³b@¸sŒa¯Z¦Š?í(RÂw$5®›m.aÒÈàEç‹|s%†¸iÜ$Òç(åD\çuŠ$"N~³3˜›.åÜÍ¾o sr”ÿÌ¡1MIË¡.cõWb.‡“¦Næ¡IìIÕ8¶şŒèÓm3—ş®í8m'\²D_ñŞ}6Uw_…ğ”½Jz)Õ¸m1¾¿Šo8É¹q1¶@G{z-É\•†Ô‹‚©æ•Ô›F°ç7$eŸñ7³~ñÂ¥€¡ë#°’Sãêç,O"_šM|½q1ózr–¥qøµ=k‚¡hK¶v&s*Ë8=ÛëzÕë‰‚¶÷jÃşÚŞpë;âáÆ¿a=1êÃ˜`Ób¶AZñ•~`é^¹»™E5c-È &%FÅ“ğvn„ŞPôÓ¯ŒF„¦~/°ÍŞ‰™şKóà«†tİíÌMBä[i*³(˜ŠşÚp›¥p¿
®@*•ğ,Òu€Üïrù]PöŒ|ÁçÓ²ß¦¹@W€ÁÎ€bLòÈ@‡ÇŸŒn 8×s†O§èÙ–x[}I3i:K‰ıÃÍh8JBIè´WˆŸ+ùÜ£‰úT%ƒyá% @}Xiª-ÃŠœí¢şÏ±8<°& eE“U§// SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/export.h>
#include <linux/irqflags.h>

noinstr void warn_bogus_irq_restore(void)
{
	instrumentation_begin();
	WARN_ONCE(1, "raw_local_irq_restore() called with IRQs enabled\n");
	instrumentation_end();
}
EXPORT_SYMBOL(warn_bogus_irq_restore);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            // SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/export.h>
#include <linux/irqflags.h>

noinstr void warn_bogus_irq_restore(void)
{
	instrumentation_begin();
	WARN_ONCE(1, "raw_local_irq_restore() called with IRQs enabled\n");
	instrumentation_end();
}
EXPORT_SYMBOL(warn_bogus_irq_restore);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <waiman.long@hpe.com>
 */

/*
 * Collect locking event counts
 */
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/fs.h>

#include "lock_events.h"

#undef  LOCK_EVENT
#define LOCK_EVENT(name)	[LOCKEVENT_ ## name] = #name,

#define LOCK_EVENTS_DIR		"lock_event_counts"

/*
 * When CONFIG_LOCK_EVENT_COUNTS is enabled, event counts of different
 * types of locks will be reported under the <debugfs>/lock_event_counts/
 * directory. See lock_events_list.h for the list of available locking
 * events.
 *
 * Writing to the special ".reset_counts" file will reset all the above
 * locking event counts. This is a very slow operation and so should not
 * be done frequently.
 *
 * These event counts are implemented as per-cpu variables which are
 * summed and computed whenever the corresponding debugfs files are read. This
 * minimizes added overhead making the counts usable even in a production
 * environment.
 */
static const char * const lockevent_names[lockevent_num + 1] = {

#include "lock_events_list.h"

	[LOCKEVENT_reset_cnts] = ".reset_counts",
};

/*
 * Per-cpu counts
 */
DEFINE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * The lockevent_read() function can be overridden.
 */
ssize_t __weak lockevent_read(struct file *file, char __user *user_buf,
			      size_t count, loff_t *ppos)
{
	char buf[64];
	int cpu, id, len;
	u64 sum = 0;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	id = (long)file_inode(file)->i_private;

	if (id >= lockevent_num)
		return -EBADF;

	for_each_possible_cpu(cpu)
		sum += per_cpu(lockevents[id], cpu);
	len = snprintf(buf, sizeof(buf) - 1, "%llu\n", sum);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

/*
 * Function to handle write request
 *
 * When idx = reset_cnts, reset all the counts.
 */
static ssize_t lockevent_write(struct file *file, const char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	int cpu;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	if ((long)file_inode(file)->i_private != LOCKEVENT_reset_cnts)
		return count;

	for_each_possible_cpu(cpu) {
		int i;
		unsigned long *ptr = per_cpu_ptr(lockevents, cpu);

		for (i = 0 ; i < lockevent_num; i++)
			WRITE_ONCE(ptr[i], 0);
	}
	return count;
}

/*
 * Debugfs data structures
 */
static const struct file_operations fops_lockevent = {
	.read = lockevent_read,
	.write = lockevent_write,
	.llseek = default_llseek,
};

#ifdef CONFIG_PARAVIRT_SPINLOCKS
#include <asm/paravirt.h>

static bool __init skip_lockevent(const char *name)
{
	static int pv_on __initdata = -1;

	if (pv_on < 0)
		pv_on = !pv_is_native_spin_unlock();
	/*
	 * Skip PV qspinlock events on bare metal.
	 */
	if (!pv_on && !memcmp(name, "pv_", 3))
		return true;
	return false;
}
#else
static inline bool skip_lockevent(const char *name)
{
	return false;
}
#endif

/*
 * Initialize debugfs for the locking event counts.
 */
static int __init init_lockevent_counts(void)
{
	struct dentry *d_counts = debugfs_create_dir(LOCK_EVENTS_DIR, NULL);
	int i;

	if (!d_counts)
		goto out;

	/*
	 * Create the debugfs files
	 *
	 * As reading from and writing to the stat files can be slow, only
	 * root is allowed to do the read/write to limit impact to system
	 * performance.
	 */
	for (i = 0; i < lockevent_num; i++) {
		if (skip_lockevent(lockevent_names[i]))
			continue;
		if (!debugfs_create_file(lockevent_names[i], 0400, d_counts,
					 (void *)(long)i, &fops_lockevent))
			goto fail_undo;
	}

	if (!debugfs_create_file(lockevent_names[LOCKEVENT_reset_cnts], 0200,
				 d_counts, (void *)(long)LOCKEVENT_reset_cnts,
				 &fops_lockevent))
		goto fail_undo;

	return 0;
fail_undo:
	debugfs_remove_recursive(d_counts);
out:
	pr_warn("Could not create '%s' debugfs entries\n", LOCK_EVENTS_DIR);
	return -ENOMEM;
}
fs_initcall(init_lockevent_counts);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ELF                      (W     4     ( ' &             èüÿÿÿS‰Ãh    jèüÿÿÿ‹    XY…ÒtƒP¦ÿÿ[éüÿÿÿt& [Ã´&    ´&    èüÿÿÿUWVSƒì‹°È   F~ø‰$ö    …s   †¨Y  èüÿÿÿö    …_   ‹†4&  …À~‰øèüÿÿÿº   ‰ø®Ü1  èüÿÿÿ”Q  ´&    ‰èƒÅtèüÿÿÿ9İuò®d  ô)  ¶    ‹…Üşÿÿ…Àt‰èèüÿÿÿÅğ  9İuåºˆ   ‰øèüÿÿÿ‰øèüÿÿÿ‹†Ì1  èüÿÿÿº   ‰øèüÿÿÿ‰øèüÿÿÿ‹Fü‰ú‹€L  èüÿÿÿ…ÿt&‹†L&  …Àtö    …‡   èüÿÿÿÇ†L&      ‹†@&  ‹–D&  j h   ‰Ñ‰Â¸    èüÿÿÿ‹Füèüÿÿÿ‹†80  ZY…Àt$®<0  ¸0  ë´&    ‹E ƒÅèüÿÿÿ9İuñ†h  èüÿÿÿÿvdÿt$hY   èüÿÿÿ‰ğèüÿÿÿ‰øƒÄ[^_]éüÿÿÿèüÿÿÿWVS‰Ó‰Şèüÿÿÿ÷ŞöƒÆd‹=    ë´&    …Àt4„Ét0‰ò‡Wèüÿÿÿ1Ò¹   …Ûtã‹â   u!‹Áêƒâ‰Ñƒñ…ÀuÌ[‰Ğ^_Ã´&    v º   [^‰Ğ_Ãt& èüÿÿÿU‰Å‰ÑWzVƒçü‰ÖS)ùƒÁDÁéì  d¡    ‰„$   1ÀÇ    ÇB@    ºÀ  ó«¹ô  ¡$   èüÿÿÿ…ÀtR‰Ã¹   @ºk   èüÿÿÿ…œR  ¹   ‰C¸P   ‰âf‰C‰Øèüÿÿÿ…Àu‹Eh‹ ƒø	u5‰â‰ğèüÿÿÿ‰Øèüÿÿÿ‹„$   d+    u-Ä  [^_]Ã´&    v «   ƒø¡   …Ày¸é«   èüÿÿÿv èüÿÿÿUWV¾   Sƒì|‰$d¡    ‰D$x1ÀğÁ5    ƒş3  ¹   ºÀ  ¸ÀY  èüÿÿÿ‰Ã…À„–
  ‹$‰3S‰T$‰Cˆ   ‰D$èüÿÿÿ‰D$…À…‚  C³<  ‰D$‰Çÿ3h}   j$Pèüÿÿÿÿ3W1ÿh…   èüÿÿÿ¡ü   ‹-Ì   ÇƒT      Çƒ|      ƒÄ‰ƒ@  ¡ø   ‰«d  ‰ƒ<  ¡ô   ‰ƒP  ¡ğ   ‰ƒD  ¡ì   ‰ƒH  ¡è   ‰ƒL  ¡Ğ   ‰ƒh  ¡È   ‰ƒx  ¡Ä   ‰ƒl  ¡À   ‰ƒp  ¡¼   ‰ƒt  ¡ä   ‰ƒ„  ¡à   ‰ƒ€  ¡Ü   ‰ƒ”  ¡Ø   ÇƒŒ  Ë  ‰ƒˆ  ¡Ô   Çƒ˜      ‰ƒ  ‰\$ëo¶    ‹…Û~g‹ND…É~`ƒÿ„²  ƒÿ„©  ¸,   £ø‚Ï  …íˆk  ¯é…í…ÿ  IÅÁø
‰Áá
‰NDö    …„  ƒÇƒÆƒÿt&‹n(…íu“ƒÇÇ    ƒÆÇF$    ÇF@    ƒÿuÚ‹\$¶  ‹‹4…    ‹…   ‹…   ‰³X  ‰“\  ‰ƒ`  €ùM„ÿ  9  €ùB„ë  B  ¸   1Ò€ùHtô  ¸   1Ò€ùI…Ï  ´&    ‰ƒ Y  ‰“¤Y  ƒşÿ„o  Fÿ·Àèüÿÿÿ‰Ch…À…š  ‹«X  …í…å  ‹Cfƒx$p„Ï  ‹t$·Æèüÿÿÿ‰Ch‰Á…À„Œ  ‹à   …ÀtN·f…ÒtF‹kƒÀ·}"ëf·ƒÀf…Òt-f9×uğ·Püf9U$uæ·Pşf9U&„v  ·ƒÀf…ÒuØt& ƒÆëv Áá
ºIÖ¹ò‰È÷ê
Áùº ‡  Áø)È‹L$iÀ ‡  9ĞLÂ‰ˆ  ‹ND…íˆ´  ¯Í…Éÿÿ IÁÁø‰é2şÿÿt& €ù-u{¶  €ùL„-    €ùB„ö  >  €ù-…·  ¶   €ùM„q  w  ¸    1Ò€ùJ„şÿÿ¸ €  1Ò€ùK„şÿÿ€ù-…‚  1À1Òémşÿÿt& €ù6…Ê  ¸   1ÒéSşÿÿ€ùNtn€ùb…V  ¸   1Òé9şÿÿ€ùK…–  ¸à   1Òé$şÿÿ¸à   1Ò€ùD„şÿÿ€ùGtÊéş   ´&    v €ùi„5  ~E¸   1Ò€ùm„åıÿÿ€ùnud¶  ¸   1Òƒáß€ùC…Æıÿÿ¸   1Òéºıÿÿ´&    v €ùd„wÿÿÿNÿÿÿ¸   1Ò€ùg„‘ıÿÿ€ùh…È  ¸   1Òé|ıÿÿ€ùk„Cÿÿÿéş   ¶    ‰ØÁà
™÷ù‰F(é˜üÿÿ´&    ‰ØÁà™÷ù‰F(é†üÿÿÁá
¸«ªªª÷á‹L$Áê
Rº   Áà	9ĞBÂ‰”  éşÿÿ1Àèüÿÿÿ‰Ch‹C·P"R·@ P‹|$Whø  èüÿÿÿ‹C·P&R·@$PWh(  èüÿÿÿ‹ChƒÄ ÿpWhî   èüÿÿÿWhd  èüÿÿÿWh¼  èüÿÿÿWh  èüÿÿÿ‹ChƒÄ$‹P‰Sx‹P‰Sl‹€Ä   ‰Cpƒ»X  ÿ„P  ‹C¹   ³Ø1  ‹T  ‹€P  ‰“L&  º  ‰ƒH&  ƒ(  èüÿÿÿƒX  ¹   º  èüÿÿÿƒˆ   ¹   º.  èüÿÿÿ¹   ºC  ƒœ   èüÿÿÿÿt$hX  jVèüÿÿÿVjh 
 h^  èüÿÿÿ‰ƒÔ1  ƒÄ …À„i  ‹œQ  ƒè1  “ Q  ¶    ‰X‰HlÇ@üàÿÿÿ‰ ‰@ƒÀtÇ@”    9Âuàƒœ  «  ÇƒD&     º2   ‰ƒ”  ‰èÇƒ      Çƒ     Çƒœ      Çƒ˜      èüÿÿÿ‰D$…À…X  ƒ$  ‹“ğ  ¹   ‰CD‹ƒ   ‹€€   ‰ƒ  ‹ƒü  ‹€€   ‰ƒ  ‹ƒì  ‹’€   ‹€€   ÁâÀ	Ğ‹“è  ‚€   ºa  ‰ƒ  ƒ°1  èüÿÿÿƒ˜1  ¹   ºl  èüÿÿÿƒ¤1  ¹   º~  èüÿÿÿƒ¼1  º  ¹   èüÿÿÿƒ`&  “D1  Çƒ\&     ‰ƒ('  ƒl1  ‰ƒl1  ‰“H1  ‰“D1  “X1  ‰“X1  ‰ƒ\1  ‰“l1  ‰“p1  ‰ƒX1  ö    …†  ‹$èüÿÿÿ…À…Å  ‹D$1Éºÿÿÿÿèüÿÿÿ…À…ÿ  j ¸    hŸ  j h   ‹“H&  ‹‹L&  èüÿÿÿƒÄ…À„£  ‹<$L$ º   ‰øèüÿÿÿ·L$ º   ‰øƒÉf‰L$ ·Éèüÿÿÿ¶G,º   L$ˆƒP&  ‰øèüÿÿÿ¶D$<?w‹¸   …Ò…Ã  ‹ƒH&  ‹“L&  ö    …  ¹   èüÿÿÿ‰ƒT&  …À„6     ‰“X&  ‹( Ç‰Ğ%   ÿ=   „  =   ÿ„œ  Rÿt$h˜  èüÿÿÿƒÄé®  ´&    ¸  2 1Ò€ùd„àøÿÿ€   ƒég€ù‡(  ¸   1ÒéÂøÿÿ¸   1Òé¶øÿÿ€ùktK~¿€ùl…(  ¶  ¸  @ 1Òƒáß€ùC…Œøÿÿ¸  € 1Òé€øÿÿ€ùH+¸   1Ò€ùFkøÿÿ€ùDum¸  2 1ÒéZøÿÿ€ùbt‡é(  €ùKtåé(  ¶    ¸   1Òé4øÿÿ¸   1Òé(øÿÿ¸ €  1Ò€ùk„øÿÿ€ùmtß€ùj…  ¸    1Òéş÷ÿÿé(  é(  é  éş   1Àèüÿÿÿ‰Chÿt$hÔ  èüÿÿÿ‹ChZY…À„øÿÿé%ûÿÿéş   ÇD$ôÿÿÿ‹D$xd+    u‹D$ƒÄ|[^_]Ãéş   éş   èüÿÿÿ´&    t& èüÿÿÿS‰ÃìŒ   d¡    ‰„$ˆ   1À‰\$X‹ƒ4&  ©  @ …¸  ğº«4&  ¸    ‚k  ‰Øèüÿÿÿ…À…Æ  ğ€‹4&  j h¹   jh  Sèüÿÿÿj h  SèüÿÿÿƒÄ j jh  Sèüÿÿÿ‰ØèüÿÿÿƒÄ…À…™  j h¹   jh  Sèüÿÿÿj h  SèüÿÿÿƒÄ j jh  Sèüÿÿÿ‹C`ƒÄ…Àt1‹Pd‹…Òt(‹R…Òt!‹    …É„ã   ‹I…É„Ø   èüÿÿÿv ‹“ Y  ÇD$\    ÇD$`   ‰ĞÇD$d   €ô ƒ¤Y  „‘   €ætÇD$d4  ‹‹œY  ‰âA‰ƒœY  1Àèüÿÿÿ‹ƒ¨Y  ‹“¬Y  ƒƒ Y  ‰Áƒ“¤Y   ñÿÿÿ 	Ñu¸   1ÒRP1ÀT$èüÿÿÿ1ÀL$dT$èüÿÿÿ[X1À‹”$ˆ   d+    u0ÄŒ   [Ã´&    ÇD$d´  éoÿÿÿv èüÿÿÿé&ÿÿÿ¸úÿÿÿëÀèüÿÿÿfffffèüÿÿÿfƒø	w·À‹…`  Ã´&    v 1ÀÃ´&    ¶    èüÿÿÿUWV‰Æ‰Ğ·ÒS‰Ëƒì‹Nh|Ñ¶Nvf9Á†§   ‰¶º    ‹,…8  C‰$‰èèüÿÿÿ‰Áƒøÿ„  ƒø tƒÁƒù ‡ü  ‹$‰êèüÿÿÿ€?t6ÇC$   ¶Nw¸   1ÒÓàƒè‰C(¸ÿÿÿ ‰S4‰C01ÀƒÄ[^_]Ã´&    ÇC$   ¶Nw¸   Óàƒè‰C(‹†¨Y  ‹–¬Y  ëÃf¸êÿÿÿëÂ´&    fèüÿÿÿU·êWV‰ÆS‰ËLm ƒì‹@hÇ    ÇC0    DˆL‰Ù‰$C‰Ç‰D$1Àƒçü)ùƒÁ4Áéó«¶Fwf9ĞvW‹$º    ¶ ‹4…  ‰ğèüÿÿÿ‰Áƒøÿ„  ƒø tƒÁƒù ‡  ‹D$‰òèüÿÿÿ‰+1ÀÇC$   ƒÄ[^_]Ãt& ¸êÿÿÿëíèüÿÿÿSƒx‰Ñ‹À% ÿşÿ‹“X&   Qò Â‹…Ét(ƒÊƒX&  ¹
   ´&    v ‰‹9Útƒéuó[ÃƒâşƒX&  ¹
   f‰‹9Útåƒéuó[ÃèüÿÿÿSƒx‰Ñ‹À% ÿşÿ‹“X&   Qò Â‹…Ét(ƒÊƒX&  ¹
   ´&    v ‰‹9Útƒéuó[ÃƒâıƒX&  ¹
   f‰‹9Útåƒéuó[Ãèüÿÿÿ‹ƒxÀ‹’X&  % ÿşÿ„Qò ‹ ƒàÃ´&    t& èüÿÿÿ‹ƒxÀ‹’X&  % ÿşÿ„Qò ‹ ƒàÃ´&    t& èüÿÿÿU‰ÑW¿   VÓçS‰ÃƒìD‹4•à  d¡    ‰D$@1À¶‚   iÀp  ¬œR  ƒÿt]¶‚  ÿ€   „Â   „Àt>j ƒÃ‰ñ‰êP‰ØèüÿÿÿZY…Àt'‰¸   1À‹T$@d+    …B  ƒÄD[^_]Ã¶    ¸ÿÿÿÿëÚÿsp{‰ñ‰êj ‰øèüÿÿÿYZ…Àt
Ç€      ‹Cp‰ñ‰êƒÀP‰øj èüÿÿÿYZ…Àt
Ç€      ‹Cp‰ê‰ñƒÀ
P‰øj èüÿÿÿ]Z…Àt—Ç€      1Àégÿÿÿ¶    ¿şÿÿÿf‰D$1À¹   f‰|$|$“ÜW  ó«‰$‰ğº   èüÿÿÿ‰Áƒøÿ„.  ƒøtƒÁƒù‡$  ‰òD$èüÿÿÿ‹ClÇƒÜW  l  ÇƒôW     ‰ƒàW  ‹$ÇƒäW    ÇƒèW      ‰D$$‰èj L$T$èüÿÿÿ[= ğÿÿ—À¶À÷Øé­şÿÿèüÿÿÿ´&    ´&    èüÿÿÿUhW‰×V‰Æ‰èSèüÿÿÿ‹FV9Ât/X´ë´&    v ‹CLX´9Ât9»   uî‰èèüÿÿÿ‰Ø[^_]Ã‰è1Ûèüÿÿÿ‰Ø[^_]ÃèüÿÿÿUWV‰ÆSƒìö    @…B  †ÌW  Ç$    ®|W  ‰D$F T  ‰D$‹$¹
   ’¼Æ|W  1Àó«iÂp  ‹|$ÇE   ÇE0  ¹œ   ÇE@  ÇE  ÇE   ÇE Ğ  ‰Wº0   ‰7‰} ¼œR  1Àó«Çcx18‰ØÇƒüıÿÿ    ÇC i2cÇC driÇCver ÇC    ÇC    ÇC    ÇC    ÇC     ÇC$    ÇC(    ÇC,    ‰«şÿÿ‹>èüÿÿÿƒø0‡8  „W  Øÿ4$ƒÅ(Ãp  Wh  Pèüÿÿÿ‹|$ƒD$ƒÄ‰»üÿÿ‹~‡ˆ   ‰ƒìûÿÿƒ<$…  ‹†X&   Qò ‹ =/À …  ‹†X&  »  À ˆ Ç ¸
   t& ‰‹ƒúÿtâÀ À tƒèuê»
   t& ¸X‰A èüÿÿÿƒëuñ‹†X&  º
   »À À ˆ Ç v ‰‹ƒøÿt%À À =À   tƒêuæ»
   ¸X‰A èüÿÿÿƒëuñ‹†X&  »  À ˆ Ç ¸
   v ‰‹ƒúÿtâÀ À tƒèuê»
   t& ¸X‰A èüÿÿÿƒëuñ‹†X&  »  À È0Ç ¸
   v ‰‹
ù  À tƒèuï‹†X&  »  À ˆÄ0Ç ¸
   ´&    f‰‹ƒúÿtâ  À tƒèuê‹†X&  »  Pñ ¸
   t& ‰‹
ù tƒèuïƒ¾ĞW  ‹–ÌW  À‹ŠX&  % ÿşÿ Qò Á‹	‚X&  ƒÉº
   ¶    ‰‹9Ùtƒêuóƒ¾ĞW  ‹–ÌW  À‹ŠX&  % ÿşÿ Qò Á‹	‚X&  ƒÉº
   ´&    v ‰‹9Ùtƒêuó‹†X&  »  Qò ¸
   t& ‰‹
ù tƒèuïƒ¾ØW  ‹–ÔW  À‹ŠX&  % ÿşÿ Qò Á‹	‚X&  ƒÉº
   ¶    ‰‹9Ùtƒêuóƒ¾ØW  ‹–ÔW  À‹ŠX&  % ÿşÿ Qò Á‹	‚X&  ƒÉº
   f‰‹9Ùtƒêuó‹F~X´9Çt2¶    öƒ   @t‹Cd‹ …Àt‹H…Ét	1Ò‰Øèüÿÿÿ‹CLX´9ÇuÔ¾œR  ‰øèüÿÿÿ‰Ã…À„•   ƒÄ‰Ø[^_]Ãv Ç$   é³ûÿÿt& ‹†X&  »   ˆÇ ¸
   ´&    v ‰‹ƒúÿtâ  tƒèuê‹†X&  º
   »  ˆ$Ç t& ‰‹ƒøÿt%  =   „…üÿÿƒêuâé{üÿÿt& †U  èüÿÿÿ‰Ã…À„Vÿÿÿ‰øèüÿÿÿéJÿÿÿ´&    èüÿÿÿVS‰Ãö    @…a  ‹ƒX&  ‹ˆ Pñ ‹ƒX&  ƒÉ Pñ ¸
   ´&    v ‰
‹29ñtƒèuó‹ƒX&  ‹ˆ Qò ‹ƒX&  ƒÉ Qò ¸
   v ‰
‹29ñtƒèuóƒœR  èüÿÿÿƒU  [^éüÿÿÿfffffèüÿÿÿV‰ÆSö    …v  ‹†X&  º
   »  ˆ Ç t& ‰‹ƒøÿt
%  ƒøtƒêuè‹X&  º
   »  Á Ç f‰‹ƒøÿt
%  ƒøtƒêuè[^Ãt& èüÿÿÿW‰Á¿   VS‹€X&  ‰Óˆ€Ç ¸
   ¶    ‰:‹2ƒştƒèuò‹X&  ¿   ° Ç ¸
   t& ‰>‹ƒúÿtâ  tƒèuêƒû‹X&  Òƒâ° €Ç ¸
   ƒÂ¶    ‰‹>9útƒèuóƒû‹X&  Òâ $Iş°€Ç ¸
   Â7¿ï´&    f‰‹>9útƒèuó‹X&  ¿   €Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ¿   €Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ¿   €Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ƒû¾   ƒŞÿ€Ç ¸
   ¶    ‰2‹:9ştƒèuóƒû‹X&  Òâ;Ïó °€Ç ¸
   ÂDÃ0 ´&    f‰‹>9útƒèuó‹X&  ¿   €Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ¿   @€Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ¿şâ+ D€Ç ¸
   t& ‰:‹2şşâ+ tƒèuï‹X&  ¿   H€Ç ¸
   ‰:‹2ƒştƒèuò‹X&   Ç …Û„÷   º
   ¾  ÿÿt& ‰0‹ƒû tƒêuò‹X&  ¾ ÿÿÇ ¸
   t& ‰2‹ƒûtƒèuò‹X&  ¾ ÿÿÇ ¸
   t& ‰2‹ƒûtƒèuò‹X&  ¾ÿÿÇ ¸
   t& ‰2‹û  tƒèuï‹X&  ¾&ÿÿ Ç ¸
   ‰2‹û&  tƒèuï‹‘X&  ¸
   »1ÿÿÂ$Ç ´&    f‰‹
ù1  tƒèuï[^_Ãv »
   ¾  ¶    ‰0‹ƒúÿtâ  ƒútƒëuç‹X&  º
   ¾  ˜Ç ´&    f‰3‹ƒøÿt%  ƒø„ïşÿÿƒêuäéåşÿÿ´&    ´&    èüÿÿÿV1Ò¾   S‰Ã¸
   èüÿÿÿ‹ƒX&  ˆ Ç ¸
   t& ‰1‹ƒúÿtâ  tƒèuê1Ò¸
   èüÿÿÿ‹Ch‹ˆÈ   ‹ƒX&   È ¸
   t& ‰
‹29ñtƒèuó1Ò¸
   èüÿÿÿ‹Ch‹ˆÌ   ‹ƒX&   È ¸
   t& ‰
‹29ñtƒèuó‹Ch‹ˆĞ   ‹ƒX&   È ¸
   ‰
‹29ñtƒèuó‹Ch‹ˆÔ   ‹ƒX&   È ¸
   ´&    f‰
‹29ñtƒèuó1Ò¸
   èüÿÿÿ‹Ch‹ˆØ   ‹ƒX&  H È ¸
   t& ‰
‹29ñtƒèuó‹Ch‹ˆÜ   ‹ƒX&  T È ¸
   ‰
‹29ñtƒèuó1Ò¸
   ¾   èüÿÿÿ‹ƒX&  ˆ Ç ¸
   ‰1‹ƒúÿtâ  tƒèuê1Ò¸
   ¾   èüÿÿÿ‹ƒX&   È ¸
   ´&    ‰2‹
ƒùtƒèuò‹ƒX&  º
   ¾  ˆ$ Ç t& ‰1‹ƒøÿt
%  ƒøtƒêuè‹ƒX&  ¾H   œ È ¸
   ´&    v ‰2‹
ƒùHtƒèuò‹ƒX&  ¾   üÈ ¸
   t& ‰2‹
ù   tƒèuï‹ƒX&  ¾  É ¸
   ‰2‹
ù  tƒèuï‹ƒX&  ¾  $É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  (É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  0É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  4É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  ,É ¸
   ´&    f‰2‹
ù  tƒèuï‹“X&  ¸
   »  Â8É ´&    f‰‹
ù  tƒèuï[^Ã´&    t& èüÿÿÿUW‰ÇVS»   ƒìP‰D$d¡    ‰D$L‰ø‹€X&  ‰$L Ğ ¸
   ‰‹
ƒùtƒèuò‹D$º
   »  ‹€X&  ‰$ˆ Ç ‰‹ƒøÿt
%  ƒøtƒêuè1Ò¸   èüÿÿÿ‹D$‹€X&  ‰$ Ç ‹ ¨„Ù  ‹|$º   ‰øèüÿÿÿ‰øºˆ   èüÿÿÿ‹Gº  ‹ŸT&  ÇD$0    ‰$ˆˆ   D$0èüÿÿÿ‰Ç…À…Û  ‹D$0‹‹p‰$…É„s  ‹D$‰ú‰ıèüÿÿÿ‹D$0‰|$Ç   ‰D$‹ ‰$9Ås9t& 9ı}1‹¸
   t& ‰‹9Êtƒèuó‹9Â…¦  ƒÃƒÆƒÅ;,$rË;<$rš‹D$‹€4&  ¨„‹  ‹D$èüÿÿÿ‹t$º  Ü ‰ğèüÿÿÿ‹<$…ÿ°  ‰ğèüÿÿÿ‹F‹¾T&  º3  ÇD$0    ‰$ˆˆ   D$0‰|$ èüÿÿÿ…À…ı  ‹T$0‹z‹2‰Ğ¶_‰|$‰t$ˆ\$$¶_ˆ\$*¶_0ˆ\$+ƒş†™  ‹D$ÇD$   ÇD$    ƒÀ‰D$,ë‹D$‹:ƒÀ‰Æ‰D$‰Ğ9şƒâ   ‹D$‹t$ƒàüD$‹x‰|$‹xş8Segm‰<$‰t$u½xHedru´ö    …
  ‹|$9|$‚   ‹$…Ét~‹D$ ‹t$1ÿğ‰D$‹D$‰û‰ı8‹D$Ç   èüÿÿÿ9$vJ9û}Fv ‹D$‹\$(‹D$4(‰ğƒàü‹¸
   ‰‹9Útƒèuó‹9Â…t  ƒÅ9,$v9ı|½;<$r’‹T$0éÿÿÿ‹D$0‹8‹t$‹–4&  ƒâ„5  èüÿÿÿ‹t$1Ò‰ğèüÿÿÿ…ÿ‹D$Ld+    …=  ƒÄP‰ø[^_]Ã‹†X&  »   ‰$ Ç ¸
   ‰‹
ƒùÿtá  tƒèuê‹t$»2   ëv 1Ò¸
   èüÿÿÿƒë„†   ‹†X&   Ç ‹ ¨uÚ1Ò¸È   èüÿÿÿ‹t$ºˆ   ‰ğèüÿÿÿj jh   D$@PVèüÿÿÿ‰ÇƒÄ…ÀˆEÿÿÿ‹†X&  ¹
   »  Ç ‰‹ƒúÿtâ  ú   t9ƒéuä1ÿéÿÿÿ1Ò¸È   èüÿÿÿ‹D$‹€X&  ‰$ Ç ‹ ¨„oÿÿÿé»  1ÿéÙşÿÿ‹t$‹–4&  ƒât+èüÿÿÿ‹D$º  Ü èüÿÿÿé²şÿÿèüÿÿÿt& ‹|$évşÿÿé‹  fffffèüÿÿÿWVS‹°”   ‰ÃƒÃp¾X  ‰øèüÿÿÿÿ¶ X  ÿ¶üW  Sh¬  èüÿÿÿ‰øèüÿÿÿ1ÀƒÄ[^_Ã´&    fèüÿÿÿVS‰Ã‹@`‹ˆ˜   …ÉtJö    …-  ‰ÙÁĞX  t3‹ƒ4Y  ‹ …Àt'‹X…Ût ¡    º   …Àt‹p…öt‰È[‰ñ^éüÿÿÿ[^Ãt& ‰È‰Ù[^éüÿÿÿ´&    ¶    èüÿÿÿWVS‹°”   ‰ÃƒÃp¾X  ‰øèüÿÿÿÿ¶ X  ÿ¶üW  Sh¬  èüÿÿÿ‰øèüÿÿÿ1ÀƒÄ[^_Ã´&    fèüÿÿÿU‰ÅWVSƒì‹üW  ‹€ X  ·ğÁè‰$‰Ó‰D$‹…X&  ·úÁã‰t$‰ÖˆÇ Áî¸
   ´&    ‰‹ƒúÿt…×tƒèuî‹D$‹X&  ‰t$º
   	ÃÁ Ç ‰Æ‰‹ƒøÿt1ğ…øtƒêuì‹…X&  ‹$‹t$˜Ç f1Ò¸
   ‰‹ƒùÿt…Îtƒèuî‹|$‹X&  ¹
   	úÃÇ ´&    ‰‹ƒøÿt1ø…ğtƒéuìƒÄ[^_]Ãt& èüÿÿÿUWVS‹˜”   ‹Ch‹¨œ   …ít5»X  ‹°¨   ‰øèüÿÿÿ3³ X  ‰Ø!î3³ X  ‰³ X  è¶şÿÿ‰øèüÿÿÿ[1À^_]Ã´&    èüÿÿÿUWVS‹°”   ‹Vh‹˜Y  ‹ªœ   I‹‚¤   fƒ|ŠTE‚    …ít3¾X  ‰Ã‰øèüÿÿÿ‹† X  1Ø!è3† X  ‰† X  ‰ğè<şÿÿ‰øèüÿÿÿ[1À^_]Ã´&    ´&    èüÿÿÿUW‰ÇV‰ÖS‰ğ‰Ëƒì	Ø‹L$ ‹T$$‰$‰L$‰T$uƒÄ[^_]Ãt& ¯X  ‰èèüÿÿÿ3· X  ‰ø÷Ö#4$3· X  ‰· X  è½ıÿÿ‰èèüÿÿÿ‹D$èüÿÿÿèüÿÿÿ‰èèüÿÿÿ3Ÿ X  ‰ø÷Ó#$3Ÿ X  ‰Ÿ X  èƒıÿÿ‰èèüÿÿÿ‹D$èüÿÿÿƒÄ[^_]éüÿÿÿ´&    èüÿÿÿS‹˜”   ‹Chƒútƒút_…Òt31À[Ã´&    ‹˜   ÿ°”   1Éÿ°   ‰Øèışÿÿ1ÀY[[Ã´&    ‹ˆ   ‹ˆŒ   ÿ°”   ÿ°   ‰ØèÑşÿÿX1ÀZ[Ãv ƒ¸´   Guœ¶H|jº   ‰ØjÓâ1Éè§şÿÿXZë€v èüÿÿÿUWVSƒì‹°”   ƒútƒút|¿êÿÿÿ…ÒtcƒÄ‰ø[^_]Ã‹Fh‹˜¤   ‹œ   1ÿ…Ò‰$tİ®X  ‰èèüÿÿÿ‹$3 X  ‰ğ!Ó3 X  ‰ X  èMüÿÿ‰èèüÿÿÿ‰øƒÄ[^_]Ãt& ‹Fh‹˜    ë¦t& ‹Fh‹˜¨   ë–´&    ¶    èüÿÿÿUW¸X  VS‰Ã‰øèüÿÿÿ‹Ch‹€   ‰“üW  ‹°„   ‰³ X  ƒ¸´   Gu ¶H|½   Óå	ê‰“üW  ¶@|«Æ‰³ X  …Òtö    u‰Øè ûÿÿ[‰ø^_]éüÿÿÿ‹ƒX&  ‹Ç ‹ƒX&  ‹ˆ Ç ‹ƒX&  ‹°Ç ‹ƒX&  Ç ‹ RQVPCPh  èüÿÿÿƒÄë¦´&    ¶    èüÿÿÿUWV‰ÖS‰Ãƒú tƒú@ta[¸êÿÿÿ^_]Ã´&    ¸X  ½F  º   ‰øèüÿÿÿC‰Ÿ”   UPGph_  j Pèüÿÿÿ‰·   C‰úƒÄ[^_]éüÿÿÿ´&    ¸ØX  ½O  º@  ë®¶    èüÿÿÿ…ÉugVS‹ ‹‹Chƒ¸´   GuMö    …B  ‰ŞÆØX  tV‹ƒ<Y  ‹ …ÀtC‹H…Ét<¡    º   …Àt%‹X…Ût‰ğ‰Ù[^éüÿÿÿv 1À[^Ãv 1ÀÃt& ‰ğ[^éüÿÿÿ¸ııÿÿëâ¸íÿÿÿëÛèüÿÿÿUWxV‰ÖS‰Ã‰øƒìèüÿÿÿF‰$èüÿÿÿ‹9ÃtKnf‹‹P‹‰Q‰
‹V‰F‰0‰P‰Ç@     Ç@$    Ç@    Ç@    Ç@    ğÿE ‹9Øuº‰‰ø‰[ÇC    ÇC    èüÿÿÿ‹$ƒÄ[^_]éüÿÿÿèüÿÿÿ‹P…ÒtS1Ò‹XÓƒÂ‹É‰9Pwí[Ãv Ã´&    èüÿÿÿVpS‹X9ót0´&    ‹C…Àt"1À´&    ‹KÁƒÀ‹Ê‰9Cwí‹9óu×[^Ã´&    t& èüÿÿÿ‰ ‰@Ç@    Ç@    Ã´&    èüÿÿÿUWV‰ÖS‰Ã¨  9Át#ÇB     ÇB$    ÇB    ÇB    ÇB    ƒ”  9ÁtM‰Ï‰Ëk‰èèüÿÿÿ‹D$…Àt'‹‰p‰‰~‰3‹F +F$CğÿC‰èèüÿÿÿ‰Ø[^_]Ã‹C‰s‰>‰F‰0ëÖ‹Aƒø>~«Ã€  ‰ßë¥´&    ´&    èüÿÿÿWzV‰øS‰Óèüÿÿÿ‹9Ãt6‹3‹F‹‰B‰‹F$‰6‰vC+F ‰CÇF    ğÿK‰øèüÿÿÿ‰ğ[^_Ã‰ø1öèüÿÿÿ‰ğ[^_ÃèüÿÿÿUWVS‰Ã³¤  ƒì$‰L$‰$d¡    ‰D$ ‹ƒ8  ‰D$D$‰D$‰D$‰ğèüÿÿÿ‹«”  ƒ”  ‰D$‹M 9è„¸  ‹D$‹U‰t$‰ïƒÀ‰D$;$t4‹EƒÀ‰E‹³œ  ƒî9ğƒW  ‹‰Ï‰Í;L$„q  ‹U‰Á;$uÌ‹U ‹E‹t$‰B‰‰} ‰}ğÿ‹œ  ‰ğèüÿÿÿ…í„   ‹D$‹uÇE    ‰E E‰$‰Ç‹E9Çt	9u„Ò   ‹»t  ;4$t[‰l$‰ı‹|$ë8v )ï‰è‹V‹N‰F‹ƒ8  ‹@ÿ³X  ÿ³t  ˆ   èüÿÿÿ‹6XZ94$tÇF    9ıvÀ‰ø1ÿë¾f‹l$ƒ»@  tğ€M‹T$L$»€  ‹2‰Ğ9Êu
ë/¶    ‰Î‹J‰N‰1‰ù‰‰B‰Øj èüÿÿÿ‹‰ğ‰ò^t$9ğu×‹D$ d+    u]ƒÄ$‰è[^_]Ã‹D$‹VÇF    ‰F‹‹8  ‹F‹qÿ³X  ‰Ñÿ³t  ‰ÂÆˆ   ‰ğèüÿÿÿY^éSÿÿÿ‹t$‰ğ1íèüÿÿÿéOÿÿÿèüÿÿÿ´&    èüÿÿÿV°€  S‰ò‰Ã€”  èdûÿÿƒ¨  ‰ò[^éUûÿÿt& èüÿÿÿUW¸¼  V‰ú‰Æ”  S®h  ƒì‰<$è'ûÿÿ†¨  ‰úèûÿÿ†€  ‰úèûÿÿ†Ì  ‰D$èüÿÿÿ‹¼  9ßtYt& ‹C{9øt;¶    ‹C‹P‹‰Q‰
‹–l  ‰†l  ‰(‰P‰Ç@    Ç@    ‹C9øuË‹†L  ‰C‹9$u«‹D$ƒÄ[^_]éüÿÿÿ´&    t& èüÿÿÿUWVS‰Ãƒì‹¸8  ‹€L  ‰D$ƒ¼  ‰Â‰D$‰Øèüÿÿÿ“€  ‰D$‰Æ‹ƒx  ‰T$“h  ‰$‰T$…ö„/  9$…&  v ‹D$‹t$‰p‹‹x  …É„m  ƒÀÇ$    4õ´  ‰D$´&    v ‹ƒh  ;D$„ã   ‹ƒh  ‹‹P‰Q‰
‹L$‹Q‰A‹L$‰P‰‰‹—„   L2ü‹P¸
   ´&    f‰‹)9êtƒèuó‹„   ‹“t  ¸
   ñ‰‹)9êtƒèuóƒ$‹ƒx  ƒÆ‹$9È‡kÿÿÿ‹t$Î‰õ9Èu[‹ƒ|  1Ò÷³t  …Òuaj ‹T$‰Ø‹L$èüÿÿÿZ‰l$‹T$‰Øèüÿÿÿ‰D$…Àt‹ƒx  9$„İşÿÿƒÄ[^_]Ã‹$‹t$Æj‹L$‰Ø‹T$èüÿÿÿ‰t$Xëµ‹$‹t$Œ0•  ‹‡„   ¾
   DÈ´&    ‰‹9Ê„oÿÿÿƒîuïéeÿÿÿÇ$    ‹l$éCÿÿÿfèüÿÿÿUWVSƒì‹¨X  ‹¸t  ‹€8  ‹Z‹pB‰$Æˆ   9Ãt t& ‹C‹SUW‰Ñ‰Â‰ğèüÿÿÿ‹XZ;$uäƒÄ[^_]Ãt& èüÿÿÿWV‰ÆSö    …£  ‰ğ¼  èüÿÿÿë´&    v èüÿÿÿ‰Ú‰ğèüÿÿÿ…Àuî‹†h  ¾h  9Çt^¶    ‹h  ‹‹C‰B‰‹S‰‹K‰[‹†8  ‹@j ÿ¶X  ÿ¶t  ˆ   èüÿÿÿ‹Cèüÿÿÿ‰Øèüÿÿÿ‹†h  ƒÄ9øu¨[^_Ã´&    t& èüÿÿÿ‹ˆp  …É„  U‰ÅWVSƒì‹€8  ‰D$ö    …   ‹D$‹€   „–  Áà=   ñ  ‰•L  …¼  1ÿ‰D$é›   ´&    ‚   @j âÿ  ‰ÁÁèÿt$Áéÿt$€‹Í    ƒáàÁ‰Ñ‰Â‰ØèüÿÿÿƒÄ‹8  ‰FƒÇ‰V‹YÿµX  ‰Ñ‰Âÿµt  Ãˆ   ‰Øèüÿÿÿ‹…l  •h  ‰µl  ‰‰F‰0‹…p  ZY9ø†8  ¡   ¹(   ºÀ-  èüÿÿÿ‰Ã…À„>  ¡   ¹   ºÀ-  èüÿÿÿ‰Æ…À„(  ‹…t  ºÀ,  èüÿÿÿ‰F…À„Í   C‰‰Ú‰C‰C‹…L  ‰[‰C‰èj ‹L$èüÿÿÿ‹…X  ‹V‰6‰v‰D$‹…t  ‰T$‰D$‹…8  ‹H‰Ğ‰L$™ˆ   èüÿÿÿZ‹$„À‹L$„¥şÿÿ€=     ¸ÿÿÿÿºÿÿÿÿ…Êşÿÿ‹‘´   Æ    …Òte‰Ø‰$èüÿÿÿ‹$RPhd  èüÿÿÿ¸ÿÿÿÿƒÄºÿÿÿÿéŒşÿÿ1ÀÃt& ‰Øèüÿÿÿ‰ğèüÿÿÿ‹…p  9Ç…Å  ‹|$‡€   1ÀƒÄ[^_]Ã‹‘ˆ   ë“‹…p  ëÔ‰Øèüÿÿÿ‹…p  ëÅffffèüÿÿÿV‰ÆS‹€Ø   ‰Ó‹€8  ·  ·€  ¯ĞÒ‰‹…ÀuÇ    ‹¸    ¯Ğƒèú  évv ‰‹¯Ğƒèú  éwîÇF4   1À[ÇF8   ^Ã´&    t& èüÿÿÿS‹€Ø   Z(ÇB   ‹ˆğ  ‰˜ğ  ì  ‰B(‰J,‰[Ã´&    ´&    èüÿÿÿU¨”  W¸€  VS‰Ãƒì¶    ‰ú‰Øèüÿÿÿ‰Æ…Àt?‹ƒ8  ƒ»D  ÿ‰$t?‹ƒT  öÄu4‹ƒT  ¨t*j ‰é‰ò‰ØèüÿÿÿY9Åt8‹ƒœ  ƒÄ[^_]Ã´&    v j ‰ò‰ù‰Øèüÿÿÿ‹“œ  ^ƒú>Õ9ètƒëÏ‹VF9Ât‹F;Ft^‰ò‰Øèüÿÿÿ‹Fÿ³|  Pÿ³x  ‹L$‹‘„   „Â°  +T&  Pÿ³D  jh  Qèüÿÿÿ‹ƒœ  ƒÄ ƒø>ÿÿÿéaÿÿÿv ‹P‹H‹ƒ8  ‹@ÿ³X  ÿ³t  ˆ   èüÿÿÿXZë‚´&    èüÿÿÿ‹P¸…Òt‹PÌƒú>-Ğ  é¢şÿÿfÃ´&    ´&    èüÿÿÿS1É‰Ój èüÿÿÿ‰ØèüÿÿÿÇC    X[Ã´&    ´&    èüÿÿÿUWVSƒì‹°Ø   ‰D$‹®8  ‹†à  ‰L$·  ·½  ‰D$ÇBl    ‰$ƒÿ/†>  ƒû†5  ‹$‹BT¯ß‰ÙÉ‰J…À…  ;z„  ‹$‰z‰B‹D$‰B‹D$‹ Y  ‹¤Y  ‰Jd‰Zh‰†à  =HM12„4  i$   ‰†ä  1Éj ‹D$‰T$èüÿÿÿ‹D$èüÿÿÿ‹T$‹BTÇB    Y…Àt‹B9BL‚—   ‹Z…ÛuÇB   ‹J…Éu_‹$‰z‰B‹D$‰B‹D$‹½ Y  ‹­¤Y  ‰zd‰jh‰†à  =HM12„â   i$   ‰ä  ‹D$1É‰$èüÿÿÿ‹$…À‰Ã…Š   ÇB   1ÛƒÄ‰Ø[^_]Ã´&    ;JL†ãşÿÿ»êÿÿÿëß‹$;J…Ùşÿÿ‹\$;Z…Ìşÿÿ‹\$;à  …¼şÿÿ‹Jh‹Zd3¤Y  3 Y  	Ù…¢şÿÿé
ÿÿÿ´&    i$p  ÑèéÅşÿÿfj ‹D$1Éèüÿÿÿ‹D$èüÿÿÿ‹T$ÇB    XéXÿÿÿ´&    fi$p  ÑëéÿÿÿfèüÿÿÿW‰×VS‰Ã‹°8  ‹€¬  …À„8  ö    …a  ‹†<&  …À„ë   ğ€‹U  ‹ƒ@  …À„ß   ÿ³D  jh  VèüÿÿÿƒÄ‹ƒ@  …ÿt…À„‘  ƒøtğÿ8&  ğÿ<&  ğ€£T  ïÿ³D  jh  Vèüÿÿÿÿ³D  jh  @VèüÿÿÿÇƒD  ÿÿÿÿğ€£U  ı‹†<&  ƒÄ …ÀM†  1Òèüÿÿÿ‹–X&  ¸
   ¾   ÂL Ğ t& ‰2‹
ƒùtƒèuòj ƒ\  ¹   º   èüÿÿÿX1À[^_Ãt& 1À…ÿ”ÀPÿ³D  jh  VèüÿÿÿƒÄéÿÿÿt& ‹ƒ4  …Àt‹€ü  …À…°şÿÿƒ»@  ¸êÿÿÿuª‹x  …É…”şÿÿëšé¬  ´&    ¶    èüÿÿÿUWVS‰Ãƒìp‹³8  d¡    ‰D$l1À‹ƒ¬  …À„²  ö    …d  ‹»@  ƒÿ„¦  P  ƒÿtƒÿ…b  ¿   ğ€£T  h   jh  @l$ UVèüÿÿÿ‹D$(‰ƒD  WPjh  VèüÿÿÿƒÄ(ƒÿ…¹  ‹†<&  …À„Ó  ‹ƒ@  ‹®„   ‹–T&  ÁàŒH  „@  )Ñ)ĞQPÿ³D  jh  Vèüÿÿÿ‰Øèüÿÿÿ‹ƒ@  ƒÄƒø„  ƒø„  Çƒx     ‹ƒt  ‰ƒ|  ‰Øèüÿÿÿ‹ƒˆ  …Àt‹ƒœ  ƒø>‰ØèDùÿÿÿ³D  jh  VèüÿÿÿƒÄ…À…P  ƒÿtğÿ†8&  ğÿ†<&  ‹T$ld+    …  ƒÄp[^_]Ã´&    v …ÿ„è  ƒÿ…_  ¿   é¨şÿÿt& ƒÿ…G  ¾Ä0  À0  1ÀÇ†8'      Ç†@1      ƒçüÇ†À0      )ùÇ†<1      ƒé€Áéó«¿   éMşÿÿ´&    v Çƒx     ‹ƒ8  ƒ¸\&  „3  €xuÀ%
  @&  ‰ƒ|  éÑşÿÿ´&    j ÿ³D  jh  Vèüÿÿÿjjÿ³D  jh  VèüÿÿÿƒÄ,j jÿ³D  jh  Vèüÿÿÿjjÿ³D  jh  Vèüÿÿÿ‹†8&  ƒÄ0…À„2  h9  h8  ÿ³D  jh  VèüÿÿÿƒÄöFx…(  ‹ø  ¸   …É„k  Pÿ³D  jh  UVèüÿÿÿ‰”  †  èüÿÿÿ‹®à  ƒÄ…í„~  ƒÿ…\ıÿÿ»à  UYVY‹ƒD  „·  j Pjh#  Vèüÿÿÿ‹†<&  ƒÄ…À…3ıÿÿ¶    †  º   èüÿÿÿğ€¦4&  ï‹†X&  ½   L Ğ ¸
   ´&    v ‰*‹
ƒù„èüÿÿƒèuîéŞüÿÿ´&    f‹ƒ4  …Àt‹¨ü  …í…6üÿÿƒ»@  u‹¾x  …ÿ…ñ  ¸êÿÿÿécıÿÿ¶    »à  HM12‹ƒ8  ·€  t'iÀ   1Ò‰ƒ|  ÷³t  ƒúƒØÿ‰ƒx  éËüÿÿfiÀp  ÑøëÕ¶    €xuÀ%°C  `‡  ‰ƒ|  éüÿÿt& ¡    ¿   Ç†1      Ç†”1      ‰†   ‹†°  Ç†ˆ1      Ç†Œ1      èüÿÿÿÇ†@&      ‰†$  é{ûÿÿ´&    ¿   éiûÿÿ¶    jÿ³D  jh  VèüÿÿÿƒÄé±ıÿÿv ‹‹8  ¶Au‰Ê‰L$<ˆ$ÉˆD$ƒáƒÁ<À‰Š,'  ‰ÑƒàüƒÀ
‰D$¶$‰$‹T$<À‰‘0'  ƒà-  ƒ¹\&  ‰4'  ‹A`„–  …ÀtR‹Pd‹R…ÒtH‹R‰$…Òt>‹T$‹   Â`&  …É„ç  ‹   ‹I…É„Ö  èüÿÿÿ‹T$¶BuˆD$t& ¶D$‹‹D  ÇD$ğ°ğ°Ç$à à <‰L$Ò‰L$,â  Â  <À‰T$0ƒà  ‹L$‰D$4ÇD$8   ‰L$<‹$‰L$@ö    …Ö  D$,¹   º  P‹D$èüÿÿÿ‹ø  X¸   …É…•üÿÿ‹–€  …Òt‹’ü  …Ò…}üÿÿ1Àƒ¾Œ  …nüÿÿ‹†„  º   ‹€x  …ÀEÂéSüÿÿ´&    ‹†4&  ¨ „tüÿÿ‹†ä  èüÿÿÿÁàƒÈPÿ³D  jh  VèüÿÿÿƒÄéGüÿÿt& ö    „Qùÿÿéÿ  ¶    jéDüÿÿ…Àt8‹Pd‹R…Òt.‹R‰$…Òt$‰Ê‹   Â`&  …ÉtR‹   ‹I…ÉtEèüÿÿÿ‹D$‹“D  ÇD$` ` Ç$Ğp0‹€,'  ‰T$‰T$,À‰ÂÁàÑú	Â¸¤  ‰T$0é†şÿÿ‹$ë¶‹$èüÿÿÿ‹D$¶@uˆD$é&şÿÿö    …Á  ğ€‹U  ‹ƒ@  …À…¢   jÿ³D  jh  VèüÿÿÿƒÄğ€£T  ïÿ³D  jh  Vèüÿÿÿÿ³D  jh  @VèüÿÿÿÇƒD  ÿÿÿÿğ€£U  ı‹†<&  ƒÄ …À…Êûÿÿğ€4&  ‹–X&  ¸
   »   ÂL Ğ t& ‰‹
ƒù„›ûÿÿƒèuîé‘ûÿÿÿ³D  jh  VèüÿÿÿƒÄé[ÿÿÿèüÿÿÿéÿ  ´&    t& èüÿÿÿU¨œ  WV‰Æ€ˆ  S1Ûƒì‰D$†L  ‰$‰D$ë5ƒûtk‹…¬  …Àtƒû„˜   ‰èèüÿÿÿ‰èèüÿÿÿƒûtuƒÃÅğ  ƒûuÇ‹†À  …Àtè‹$¾Œ  …Òupèüÿÿÿ‰øÇ†À      èüÿÿÿëÃ´&    ‹x  …Ét±Ç†x      ‹–¼  …Òt‹D$èüÿÿÿë’´&    ƒÄ[^_]Ã´&    ‹D$èüÿÿÿéZÿÿÿf‰øèüÿÿÿ‹†À  ëèüÿÿÿU¨œ  ºµ  WV‰Æ@S1ÛƒìÇ$Ì  ÇD$    ‰D$‹>‹Fx‰|$‹=    Ç…¬      ‰|$Ç…4      ‰µ8  ‰@  ‰•<  ƒû„m  ƒû„¤  ‹$‹ ‰D$ƒøt‹Œd  …É„  }‰é1ÀÇE     Ç…ì      ƒçü)ùÁğ  Áéó«‰µ8  ¹¤   Ç…4      ‹D$‰@  ‰•<  ‰…X  ‹$Ç…D  ÿÿÿÿ‹@‰…H  ‹„d  ‰…p  ‹”€  …h  ‰•t  ‰ ‰…l  …\  ‰•|  ºÜ  Ç…x     èüÿÿÿ…€  Ç…P  ÿÿÿÿÇ…      èüÿÿÿ…”  Ç…¤      èüÿÿÿ…¨  Ç…¸      èüÿÿÿ…¼  Ç…Ì      èüÿÿÿ…Ô  º    1ÉÇ…Ğ  àÿÿÿ‰ ‰…Ø  …ì  Ç…Ü      ‰ ‰…ğ  …ø  j j èüÿÿÿÇ…ô      XZƒû„m  ƒû„D  ‹|$ƒÿÿ„‰   …À  ÿµ<  ÿt$hæ  j Pèüÿÿÿ‹D$ D$$1ÒÇ…à   `  øƒÄf‰…ì  F‰…¬  ¸ÿÿÿ Ç…      ‹Nh€yu‹†¨Y  ‹–¬Y  ‰…  †(  ‰…0  ‰è‰•  èüÿÿÿ‰èèüÿÿÿ…Àx[C‰ÂƒøtA’ƒ$Åğ  ‹•È  ‹•À  ‰\$‰ÃéTıÿÿv ©   …‘ıÿÿ‰èèüÿÿÿ‰Ç…ÀxƒÄ1ÿ[‰ø^_]Ãt& ‰Ç‰ğ1Òèüÿÿÿ‰øƒÄ[^_]Ãv ¨P…Tıÿÿ‰èèüÿÿÿ…ÀˆÙ   º   ¸   éqÿÿÿ´&    v ‹Fhö@…‹   Ç†ü      é şÿÿt& –(  ‹FÇ†d      R–d  Uˆˆ   †ˆ  jpÇ†h     jjRº   èüÿÿÿ·†  Ç†\  HM12Ç†d  Ğ  ƒÄiÀp  Ñø‰†`  é)şÿÿt& ¡,   ¹  ºÀ  èüÿÿÿ‰†À  …À…şÿÿé2  ‰Çéóşÿÿ´&    fèüÿÿÿU‰ÅW1ÿV¾   S˜œ  @ƒìÇD$    ‰D$‰,$éœ   ¶    ‹“¬  …Òtn‹“H  ·«ì  ‰“ä   …À…¶   ‹ƒà   ‰›(  ‰é‰úÿ0‰Øj èüÿÿÿY‰ÂX…Òˆë  ‹‹  …Éu‹‹è   ƒÿ„}  …ÿ„š  ƒÿ„Â   ‰ğƒş„…   €ƒÆÃğ  ‹<…Ä  Fÿƒş…^ÿÿÿ‹$‹¨À  …ít‰Øèüÿÿÿ…ÀˆS  ‹ƒ¬  …Àth‹ƒH  ·«ì  ‰ƒä   ‹$‹H  …À„9ÿÿÿ¶·‰ˆ  …´  ‰Íé!ÿÿÿt& ‹T$‹,$…Òtº   ‰èèüÿÿÿ‹D$ƒÄ[^_]Ãt& ¸   éIÿÿÿé  ´&    fèüÿÿÿVS‹°ø  ‰Ã…ötM‹“Ô  ‹ƒè  Ğƒø4‹ƒü  ƒø~)³L  “ô  ‰ğèüÿÿÿ‰Â…Àtj ‹Ì  ‰ğèüÿÿÿX[^Ãv ‹€€  …Àt
‹ˆü  …ÉuŸƒ»Œ  uİ‹ƒ„  ‹x  …Òu†ëËèüÿÿÿW‰ÇV¾   S˜<!  ë)´&    fiÆğ  ø‹€ğ  ¨uNƒîëğ  ƒşÿtW‹‹¬  …ÉuÖ‹ƒ4  …Àt
‹ü  …ÒuÂƒ»@  uË‹ƒ8  ‹€x  …Àu©ë¹´&    ‰Ø1Òƒîëğ  èüÿÿÿƒşÿu©[^_Ãt& èüÿÿÿH  Ø(  ‹
…Ét‹Š˜   ƒùÿuÂğ  9Âuå¹ÿÿÿÿ‰ÈÃ´&    ¶    èüÿÿÿVSƒúÿt^ˆœ  ,&  ´&    f‰Ë9‘D  u6‹±¬  …öt‰Ø[^Ã´&    ‹±4  …öt
‹¶ü  …öuàƒ¹@  tv Áğ  9Áu¶1Û‰Ø[^Ã´&    ‹±8  ‹¶x  …öu¬ëÖfffèüÿÿÿVS‰Ã‹pXƒú„€  iÊğ  ‰Ğ”ğ  ğº*sE‹Œì  ‹[P9Ùt)‹    ƒâƒø…­   ƒùÿ…¤   ‰¼  …Ò…i  1À[^Ã´&    f‹SP‰”ì  …Àuä‹–<'  †L  …Òt*ƒ¾\&  t!†l  ğ€ˆT  ğ€ˆT   1À[^Ã¶    ‹ø  …ÛuÛ‹–€  …Òt
‹Šü  …ÉuÇƒ¾Œ  uƒ‹–„  ‹’x  …Ò„oÿÿÿë¨v …Ò…š  ¸ğÿÿÿéZÿÿÿ¶    èüÿÿÿVS‰Ã‹°8  Ç€P  ÿÿÿÿ‹€@  ƒøtƒøtoğº³T  rö    …¶  [^Ãt& ‰Øèüÿÿÿ‹ƒ@  …Àuè†L  ğº¶   r>†l  ğº¶À  sÆƒ¾¼  ÿu½ğ€¦À  ÷[^éüÿÿÿt& ‹ƒT  ¨ t‡ët& ğ€¦   ÷èüÿÿÿë³´&    fèüÿÿÿUWV‰ÆSƒì\d¡    ‰D$X‹†°  èüÿÿÿ¹   l$‰$1À‰ïó«‹F~9ÇtEX´¶    ‹Cd‹@…Àt‹H…Ét	‰ê‰Øèüÿÿÿ‹CLX´9ÇuÜƒ|$<uöD$8¸   D$‰$‹†$  ;$t9ö    …Ò  ‹¾°  ‹G‹@èüÿÿÿ‹$‰øèüÿÿÿ‰Ã‹G‹@èüÿÿÿ…Ûu‹D$Xd+    uƒÄ\[^_]Ãv ö    tßéì  èüÿÿÿ´&    ¶    èüÿÿÿUWVSƒì‹HT‹XXiéğ  ‰$<+‹—Ü  ƒú„«  èüÿÿÿ‰Â…À…¬  ‹‡ğ  ´+ğ  „Àˆ§   ğº.‚œ   ƒl  ¬+œ  ‰D$ƒL  ‰D$‹‡Ü  …Àu#‹ƒ   ¨ tğº«   ƒ¦   ‹ƒÀ  ¨ ul‰èèüÿÿÿ‰Â…À…Ã   ğ€Nğº³4&  s8i<$ğ  ‰D$ÿ´;à  jh  Sèüÿÿÿ‹T$ƒÄƒÄ‰Ğ[^_]Ãv ğ€NƒÄ‰Ğ[^_]Ã´&    fğº«À  r‰‹D$èüÿÿÿ…À…ğ   ö    „kÿÿÿéO  f‹D$èüÿÿÿ…Àuö    „@ÿÿÿé%  ö    …d  ğ€£   ït& ö    …  i<$ğ  ‹„;Ü  …Àu)‹ƒ   ¨uQ‹ƒÀ  ¨t‹ƒÀ  öÄtQ´&    v ğ€&ï‰èèüÿÿÿºûÿÿÿé(ÿÿÿv ºÿÿÿÿéÿÿÿ¶    ºğÿÿÿéÿÿÿ‹D$1Òèüÿÿÿğ€£   ïëš‹D$1Òèüÿÿÿğ€£À  ïë¤ö    …:  ğ€£À  ïéBÿÿÿ´&    v èüÿÿÿUWVS‰Ãìˆ   ‹³Œ   ‹„$œ   ‰T$,‰L$0‰D$`d¡    ‰„$„   1À‹FX‰D$8‹FT‰D$H¡    %   =   „E  ‹D$8¸(  ‰øèüÿÿÿ‰ğèüÿÿÿ‰Å‰D$L‰øèüÿÿÿ…íudi|$Hğ  ‹[ ‹D$8ã   ƒ¼8ˆ  ‰|$d‰\$ u
ƒ~T„’  iT$Hğ  T$8‹‚Ô  ‹L$0‰D$D¡    …Éu3%   =   „¢  ‹„$„   d+    …ö  ‹D$LÄˆ   [^_]Ã‹\$D‹‹8&  …Éuƒºì  ÿ„·  iD$Hğ  ‹\$8ÇD$\    ‹„Ü  ƒø„«  ‹\$8‹t$dÇD$$    iT$Hğ  ¼3œ  ‰|$@¼3D  ‰|$¼3ø  Ú‰|$X¼3  ´3l  ‰t$l‰T$‰|$h‹|$Dt& \$|·l  ÇD$p    ‰\$|‰õ‹t$@‰œ$€   ‹\$Xd‹    ÇD$x    ‰T$të7t& T$p‰Øèüÿÿÿd¡    ‹â   …±  ‹ ¨…§  ‹D$‹€Ü  …ÀuD‹    ¸è  )Ğ‡   ˆˆ   ‹‡À  ¨ t‹‡À  öÄ„   ‹‡d1  9‡h1  …  ‹T$‰ğèüÿÿÿ…À…š  ‹†T  ¨„Ü  ‹D$ …À…0  ‰Ø¹   T$pèüÿÿÿ‹†°  …À…:ÿÿÿèüÿÿÿé0ÿÿÿ´&    ¡    ‰‡   ‰øèîùÿÿéaÿÿÿ´&    fì  ‡  ‰t$‰L$<  ‰\$‰Î‰Ãë8t& ‹¬  ‰ø‰$èüÿÿÿ‰èj ‹L$‹T$èüÿÿÿ‹    ‰ñ¸    èüÿÿÿX‰Ú‰èèüÿÿÿ‰Â…Àu½‹t$‹\$‹‡d1  9‡h1  „şşÿÿv ‰ıÅD1  „  ‡X1  ‰D$‹u‹D$0+D$$‰D$…ö„Ù  ‹D$DÇ$    D1  ‰D$<;t$u!éú  ´&    v ‹E‹0‰u;t$„.  ‹V‹F9Âsä‹$‹L$$)Ğ‹|$ÙL$,‰L$‹Ô  ‹|$‰L$()ß‹^9ÇGø‹<'  Ú‰\$‰T$1Û…À„Û   ‹D$‹€Ü  …À…É   ƒ¹\&  „¼   l1  9Æ„®   ƒ¹@&  BÉƒá&ƒéFˆL$7:9Á†   ‰|$T‰Ë‹|$‰l$PëQt& ‹Nj@ù9Ís9€z u3€zu-¶L$78Ju#‹L$(‹‰@&  …É…Á  ¶JƒáÀ€ù€„‘  9Ãv‰Ù1Ò)Áèüÿÿÿ‰Â…Àu£‰øF‹l$P1Û‰D$‹|$T´&    …ÿˆ   ‹D$¹   ‰úèüÿÿÿ‹T$‹D$‰ùèüÿÿÿ…À…Û   ‹D$~‹€Ü  …Àu‹L$(l1  9Æt1Ò¹ˆ1  ‘Œ1  ‹U$<$‹$ú‰U$9D$†|  „Û…t  ‹E‹X9X‚U  ;U ‚/şÿÿ‹$‹E ;l$<„Œ   t& 9E$„  j‹L$ ‰ê‹D$Dèüÿÿÿ^‹D$‹¸Ô  …ÛˆP  ‹$\$$‹D$$9D$0„  ‹\$\…Û…‚  ‹D$‹€Ü  é£ûÿÿv ö    …  Ç$òÿÿÿ‹E »òÿÿÿ;l$<…xÿÿÿ9E$u‹t$D‹†@1  ‰ÂƒÀƒâÇ„–À0      1Ò‰†@1  ‹E †1  –”1  éUÿÿÿf‹u‰uéıÿÿt& ¶J‰Íƒåƒát1É€|
ÿ…5şÿÿƒÁ9Íuî¶JƒáÄ€ùD…şÿÿ¶Jƒá€ù…şÿÿ€|* …şÿÿ€|* …øıÿÿ€|*…íıÿÿ‹\$(‰×‹l$P+|$Çƒ@&      ‹ƒ@1  ƒà‹”ƒ@0  ‰“|1  ‹„ƒÀ0  Çƒ„1      ‰ƒ€1  Çƒ`1      ‰ƒd1  Çƒh1      ‹F»   F‰D$éªıÿÿ´&    v ¶JƒáÀ€ù€„   €ùÀ…Wıÿÿ¶J	ƒáğ€ù0…Gıÿÿ‹D$(ÆD$7ºÇ€@&     B	‹~é)ıÿÿ´&    f‹E 1Û‰E$;l$<„sşÿÿj ‹L$l‰ê‹D$Dèüÿÿÿ‹    ‹L$p¸    èüÿÿÿ_éÖıÿÿv ‹$éŸıÿÿ´&    ‹$éıÿÿ´&    ‰Åğºp ]’À‰\$„À„Vûÿÿ‹D$‹ˆÜ  …É…¬   ‹E9Øt‹E;E„#  ‰èèüÿÿÿé%ûÿÿt& ö    …y  ƒ|$$ „  ¡    ‹\$$%   ‰\$L=   „c  ‹\$L…ÛHøÿÿ‰Ø‹\$`™Sé7øÿÿ¶    ¡    ¨…æ  ‹L$$…Éu°ÇD$Lüÿÿÿéÿ÷ÿÿ¶    ‹$é¯üÿÿ‰ê‰øèüÿÿÿéŠúÿÿ‹\$Dƒ»\&  •Ã¶Û‰\$\é;øÿÿ¶J	ƒáğ€ù …°ûÿÿédşÿÿ¡    ‰\$L%   =   …«÷ÿÿé¢  t& ÿt$ „8¨  j ÿt$h‹L$<‹T$8èüÿÿÿ‰D$XƒÄéw÷ÿÿèüÿÿÿéúÿÿƒ|$$ ¡    …ñşÿÿ‹D$8‹\$d„ğ  ğ€ ‹D$@ğ€ U  şèüÿÿÿ¡    é÷ÿÿ¶    ‹T$$¡    …Ò…¬şÿÿÇD$LõÿÿÿéûöÿÿÇD$Lûÿÿÿ¨„ëöÿÿér  èüÿÿÿé¢  v èüÿÿÿU‰ÑWVSƒì‰$…Ò„—  ‹B‹<$‹·Œ   ‹^T‹~Xiëğ  ı‹•ğ  Áê‰T$ƒâ‰T$u‹•ğ  ƒâttiÛğ  ƒ¼ˆ  „¸   ¡    %   =   „´  ‰ğ‰L$¾   èüÿÿÿ‹L$…À„ß   „L  ‹ …À…¾   ‹T$‰ğƒÈ…ÒEğƒÄ‰ğ[^_]Ã¶    ¨Atˆ—(  ‰L$‰Ğ‰T$èüÿÿÿ‰ğèüÿÿÿ‰D$‹D$èüÿÿÿ‹L$…É‹L$…Ë   ö     „Bÿÿÿé—  ´&    fƒ~T…>ÿÿÿ‹$”¨  èüÿÿÿ‰Ã‰ğèüÿÿÿ…À•À¶À4 ƒûunöD$tgƒÎéWÿÿÿ´&    ƒÄƒÎA[‰ğ^_]Ãv 1ö…É„ÿÿÿ‹)”ø  …ít…Òt‹$èüÿÿÿéùşÿÿ1öéòşÿÿ´&    f¸ÿÿÿÿébşÿÿ¶    	Şéñşÿÿ´&    fö    …Ñ  ¾   éÑşÿÿ´&    fèüÿÿÿUWV‰ÖS‹¸Œ   ‹OT‹_XiÑğ  Ú‹‚ğ  Áèƒàƒºˆ  uiƒùud…Àu
‹ƒĞ  ¨tƒˆ  ‰ò[^_]éüÿÿÿ´&    «(  ‰èèüÿÿÿ‰øèüÿÿÿ‰Ç‰èèüÿÿÿ…ÿuö     t½éö  ö    …  [¸êÿÿÿ^_]Ã´&    fèüÿÿÿUhüWVS‰Ã‰èsôèüÿÿÿ‰Ç‹Cô9ÆtE´&    ‹Cô‹‹PƒÀ‰Q‰
º   ¹   Ç@ø   Ç@ü"  Ç@ì   j èüÿÿÿ‹CôZ9ğuÂ[‰ú^‰è_]éüÿÿÿ´&    ¶    èüÿÿÿU‰ÕWVS‰Ãƒì‹pT‹xXiÎğ  „œ  ‰$ö    …  iÆğ  ø‹€ğ  ¨t9ö    …b  ‹CT…Àt7ƒø…~   iöğ  ş‹†ğ  ¨ tlÇ†ì  ÿÿÿÿ…íttƒÄ[^_]Ã¶    ‹‡À  ¨t~‹‡À  öÄusö    …(  1Ò‡l  ‰L$èüÿÿÿ‹‡   ‹L$¨uQ‹CTëƒ´&    v ‹$‰ê‰L$èüÿÿÿ‹L$…íuŒ„ğ  ğ€`şğ€ ‹$ƒÄ[^_]éËíÿÿv ‹‡   ¨t¾ö    …E  ‡L  1Ò‰L$èüÿÿÿ‹CT‹L$éÿÿÿt& èüÿÿÿS‰Ã‹€8&  …Àuö    …¹  [Ã´&    ‰Øèüÿÿÿƒøÿ„¤  jPjh  SèüÿÿÿƒÄëÅv èüÿÿÿVS‰Ã‹€8&  …Àuö    …ã  [^Ãt& ‰Øèüÿÿÿ‰Æƒøÿ„Î  1Ò¸d   èüÿÿÿjVjh  Sèüÿÿÿj Vjh  SèüÿÿÿƒÄ(ë¨t& èüÿÿÿU‰ÅWVSƒì‹˜Œ   ‹CT‹sX‰$ö    …ø  ¾(  ‰øèüÿÿÿƒ{TtC‰Øèüÿÿÿ‰Øèüÿÿÿi$ğ  ‹KP9Œì  „®   ‰Øèüÿÿÿ‰øèüÿÿÿ1ÀƒÄ[^_]Ã¶    ‹…Œ   èüÿÿÿ…Àt®‰ğèüÿÿÿğ€¦4&  ß‹FVh´9ÂtC‰\$‰|$‰×v ‹Ed‹@…Àt‹X…Ût‹– Y  ‹¤Y  ‰èèüÿÿÿ‹ELh´9ÇuÒ‹|$‹\$‰ğèüÿÿÿ‹†8&  …À!‰ğèüÿÿÿé2ÿÿÿt& 1Ò‰ØèüÿÿÿéDÿÿÿf‹†à  èüÿÿÿ‰Å‹†ä  èüÿÿÿÁà	èPiD$ğ  ÿ´à  jh  VèüÿÿÿƒÄë èüÿÿÿUWVS‰Ãƒìèüÿÿÿ‹°(  ‰Å‹¾8  ‡(  ‰D$èüÿÿÿ‰øèüÿÿÿ‰$…À…8  ‹¾8  ö     …  ¡   ¹\   ºÀ  èüÿÿÿ‰Å…À„ã   ‰òèüÿÿÿ‰}X‹†@  ‰ET‹‡D&  P‰—D&  ‰EP‰è‰«Œ   èüÿÿÿƒ}Tt‹D$èüÿÿÿ‹$ƒÄ[^_]Ã‹ƒŒ   èüÿÿÿ…ÀtÜ‹‡4&  ¨ u
‹‡8&  …ÀRğ€4&   ‰øwèüÿÿÿ‹GX´9Æt#‹Cd‹@…Àt‹P…Òt‰Øèüÿÿÿ‹CLX´9ÆuŞ‰øèüÿÿÿ‰øèüÿÿÿévÿÿÿ‰èèüÿÿÿ‰èèüÿÿÿ‰èèüÿÿÿÇ$ğÿÿÿéUÿÿÿÇ$ôÿÿÿö    „Aÿÿÿéi  èüÿÿÿV‰ÈS‹JX‹ZT·±  ‰p·±  Ç@   ‰pÇ@   ƒzTt1ÒÇ@MPEGÇ@   ‰P1À[^ÃiÛğ  Ù‹‘|  ‰P‹‘€  ‰P‹‘„  ‰P1À[^Ã´&    èüÿÿÿ‹BXÇAÀü›ÇAø   ÇA   ÇAGREY‹0'  ‰Q‹4'  ‰Q‹€,'  ÇA$    ‰A ‰A1ÀÇA(    ÇA,    Ã¶    èüÿÿÿW‹AV¾Ğ  S‰Ó‹zX‹Q9òOÖ¾   9òLÖƒ{Tuƒàà¾    €t‰QÛƒã Ã@  9ØOÃ[9ğLÆ^_‰A1ÀÃ¶    èüÿÿÿUW‰×VS‹rX‰Ëƒ¾\&  t‹†8&  …Àã   ‹F`…À„Î   ‹Pd‹R…Ò„°   ‹J…É„¥   ‹-   S…í„„   ‹m…ít}èüÿÿÿ…Àug‹–('  1Éf‰
Ç†\&     ‹WXÇCÀü›ÇCø   ÇC   ÇCGREY‹Š0'  ‰K‹Š4'  ‰K‹’,'  ÇC$    ‰S ‰SÇC(    ÇC,    [^_]Ã´&    èüÿÿÿë´&    f[¸ııÿÿ^_]Ã¶    [¸íÿÿÿ^_]Ã¸ğÿÿÿëÂ´&    ´&    èüÿÿÿS‰È‰Ó‹T$‹KXƒøu*€ytÀƒàÕƒÀ6‰€ytÀƒàÏƒÀ;‰B1À[Ã´&    f¸êÿÿÿ[Ã´&    fèüÿÿÿƒ9‹RXu;‹Aƒèƒøw0ÇA    ÇA    ÇAĞ  €ztÀƒà @  ‰A1ÀÃ´&    ¸êÿÿÿÃfèüÿÿÿ‹‰ÈƒúwrÁâ‰Ñ’   ‹‰   ‰‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J ‰H ‹J$‰H$‹J(‰H(‹J,‰H,‹J0‰H0‹J4‰H4‹J8‹R<‰H8‰P<1ÀÃf¸êÿÿÿÃ´&    v èüÿÿÿ‹BX‹€œY  ‰1ÀÃ´&    ¶    èüÿÿÿWVS‹‹rX…Àu9‹FƒÆ‰ÏX´9Æt$‹Cd‹@…Àt‹H…Ét	‰ú‰Øèüÿÿÿ‹CLX´9ÆuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿ‹BX‹¤Y  ‹€ Y  ‰Q‰1ÀÃt& èüÿÿÿWVS‹‹rX…Àu9‹FƒÆ‰ÏX´9Æt$‹Cd‹@…Àt‹H…Ét	‰ú‰Øèüÿÿÿ‹CLX´9ÆuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿVS|$fd@u]‹D$‹ …ÀuM‹rX‹FƒÆX´9Æt4t& öƒ   @t‹Cd‹ …Àt‹H…Étº   ‰Øèüÿÿÿ‹CLX´9ÆuÑ1À[^Ãt& ¨u¯ëñ¸çÿÿÿëì´&    t& èüÿÿÿS‰Ë‹JX‹‹    ƒâƒøtMw…Àt/…Ò…Ì  ƒc1À[Ã´&    ƒøuC…Ò…á  ÇC    1À[Ã…Ò…†  ÇC    1Àëët& …Ò…›  ÇC    1ÀëÓt& …Ò…°  ¸êÿÿÿ[Ãèüÿÿÿ‹BX‹¶Hw9Ês‰˜Y  èüÿÿÿ1ÀÃ¸êÿÿÿÃ´&    v èüÿÿÿWVS‹ZX‹Sh¶Cv|Ê9È†¢   ¡    ‰Î‹‹œY  ƒà9ñt~…À…ö  ‰³œY  ¶Dò1Ò‰ƒ˜Y  €?¸ÿÿÿ u‹ƒ¨Y  ‹“¬Y  ‰“¨  ‰“ˆ  ‰“x  ‰ƒ¤  ‰ƒ„  ‰ƒt  ‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèüÿÿÿ1À[^_Ã´&    …À…  [^_Ãt& ¸êÿÿÿëŞ´&    fèüÿÿÿUWVS‹‹zX…ÀuX‰ø‰Îèüÿÿÿö    …(  ‹GoX´9Åt'v ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÅuÜ‰øèüÿÿÿ1À[^_]Ã¸êÿÿÿëô´&    ´&    èüÿÿÿUWVS‹D$‹L$‹rX©ÿÿÿ „  ‹–¤Y  ‹ Y  1Ê1Ã	Ú„ç   ‹–4&  ƒâ …ß   ‹–8&  …ÒÑ   öÄù‰† Y  †  ”Â‰¤Y  ˆVt¶Ò•Fuèüÿÿÿ€~t¸Ğ  Ûf‰†  ƒã fÃ@€~tÉf‰  ƒáúƒÁ€~tÒ‰,'  ƒâƒÂ€~tÀ‰–0'  ƒàÓ>  ‰†4'  ö    …A  ‹F~X´9Çt0f‹Cd‹@…Àt‹h…ít‹– Y  ‹¤Y  ‰Øèüÿÿÿ‹CLX´9ÇuÒ1À[^_]Ã¸ğÿÿÿëô¸êÿÿÿëí´&    ¶    èüÿÿÿ‰Ğ·‹@Xéüÿÿÿ´&    ´&    èüÿÿÿ‹BX‹˜Y  ‰·Òéüÿÿÿ´&    èüÿÿÿ‰Ğ·‹@Xéüÿÿÿ´&    ´&    èüÿÿÿWVS‹‹rX…Àul‹FƒÆ‰ÏX´9Æt$‹Cd‹@…Àt‹H…Ét	‰ú‰Øèüÿÿÿ‹CLX´9ÆuÜƒ$Gt¹   º  èüÿÿÿ1À[^_Ã¶    ¹   º÷  èüÿÿÿ1À[^_Ã¸êÿÿÿëÚ´&    t& èüÿÿÿWV‰ÖSƒì‹zX‹‹    ƒâƒø„Ä   w*…À„š   …Ò…]  ‹Q‰ğƒâ‰Qèüÿÿÿ1ÀƒÄ[^_Ãƒøu[…Ò…x  ÇA    ‹‡8&  …À„×   ğº·4&  ƒ¿   ‰øèüÿÿÿƒøÿ„)  Pjh  Wèüÿÿÿ‰øèüÿÿÿ1ÀƒÄë›v …Ò…“  ƒÄ¸êÿÿÿ[^_Ãt& …Ò…¯  ÇA    ƒÄ‰ğ[^_éüÿÿÿt& …Ò…Ê  ÇA    ‹‡8&  …ÀtHğº¯4&  r4‰øèüÿÿÿ‰Ãƒøÿ„C  ‰øèüÿÿÿSjh  Wèüÿÿÿ1ÀƒÄéÿÿÿv ƒÄ1À[^_Ã¸ÿÿÿÿéõşÿÿ´&    ¶    èüÿÿÿWVS‹Œ   ‹JT‹zXiÑğ  ‹”ˆ  ƒú•Ãƒú‰Ş•Ã‰ğ„Øu#ƒùu‡ˆ  ƒúº    [EÂ^_éüÿÿÿt& [¸êÿÿÿ^_Ã´&    ´&    èüÿÿÿWVS‹˜Œ   ‹KT‹{XiÑğ  ‹”ˆ  ƒú•Àƒú•Â„ĞuGƒùuB‹    ‡t  ÂĞ  èüÿÿÿiSTğ  ‹KX1Àƒ¼ˆ  u„¨  [^_éüÿÿÿ¶    [¸êÿÿÿ^_Ã´&    ´&    èüÿÿÿW‰ÊVS‹ˆŒ   ‹YT‹yXiËğ  ‹´ˆ  ƒştƒşu+‹H 1Àá   ƒşuiÛğ  „¨  [^_éüÿÿÿ¶    [¸êÿÿÿ^_Ã´&    ´&    èüÿÿÿV‰ÊS‹€Œ   ‹XX‹@TiÈğ  ‹Œˆ  ƒùtƒùu$1öƒùuiÀğ  ´¨  ‰ğ[^éüÿÿÿ´&    ¸êÿÿÿ[^Ã´&    èüÿÿÿV‰ÊS‹€Œ   ‹XX‹@TiÈğ  ‹Œˆ  ƒùtƒùu$1öƒùuiÀğ  ´¨  ‰ğ[^éüÿÿÿ´&    ¸êÿÿÿ[^Ã´&    èüÿÿÿV‰ÊS‹€Œ   ‹XX‹@TiÈğ  ‹Œˆ  ƒùtƒùu$1öƒùuiÀğ  ´¨  ‰ğ[^éüÿÿÿ´&    ¸êÿÿÿ[^Ã´&    èüÿÿÿUWVS‰Ë‹rX¹   º  ‰Øèüÿÿÿ‹~lº    ‰øèüÿÿÿƒøÿ„ï  k‰Áƒø tƒÁƒù ‡å  ‰ú‰èèüÿÿÿ‹V‹‚´   …Àu‹‚ˆ   PC0h  j Pèüÿÿÿ‹FxƒÄ   €‰CT[1À^_]ÃèüÿÿÿS‹\$…ÛtR‰Ã‰Èf%Dt<ƒú~S…Ûugƒúbƒúuk€åus·À1Éë´&    fƒÁƒù t(£Èsó¸   Óà[Ã´&    v ‰Èf% tìJöƒùv1À[Ãt& ƒúu³¸   [Ãt& ƒûuãƒú~1ÀëÜƒúu•‰È[f% @Ã¸   [Ã¶    èüÿÿÿU‰ÍWVSƒì ‹BX‹°ø  ˜L  ‰D$‰\$…ö„W  ‹\$‹ƒÄ  @‹ƒ¼  ‰$)Ğº   …ÀNÂ¯ƒÀ  º«ªªªÇE     ÇE    ÇE    ÷âƒô  ÇE    ‰D$ƒÌ  ÇE    Áê‰D$‰U¶    ‹T$‹D$èüÿÿÿ‰D$…À„e  ‹\$ƒ»Œ  …ı  ‹@‰D$…À„‰  ‹D$ƒÀ‰D$9D$u$é†  ‹D$‹@‹ ‹\$‰D$‰C9D$„4  ‹D$‹X‹x9ûsÔ‹M ‹t$)ßÇ$    ‹V‰ÈÁàDÚƒÿwlëv´&    ‹J‹ZƒïƒÂƒ$‰X1Û‰‹rü‹Jøƒæ	Ş‰H‰p‹Jè‰H‹JôÇ@    ƒáÇ@    ‹`  ‰H‹E H‰È‰M ÁàDƒÿvƒù?v˜‹D$‹X‹4$Gÿ÷ƒøCşû‰ø‹|$‰_‹|$G$‰G$ƒ} ?‰Áwc‹G‹W ‹X9XrU9Ê‡ÿşÿÿ´&    j ‹L$ ‹T$‹D$èüÿÿÿƒ} ?X†‚şÿÿ‹L$‹    ¸    Á   èüÿÿÿ1ÀƒÄ [^_]Ã‹K$‹D$;H s²j‰Â‹L$‹D$èüÿÿÿZë¼‹D$‹X‰\$‰Xédşÿÿ‹\$‹C ‰C$é|ÿÿÿ‹€€  …Àt‹˜ü  …Û…‘ıÿÿ‹\$¸êÿÿÿƒ»Œ  uŒ‹“„  ‹Šx  …É…kıÿÿésÿÿÿ‹D$‹H$‹D$;H ƒ%ÿÿÿénÿÿÿèüÿÿÿ‹BXÇAÀü›ÇAø   ÇA   ÇAGREY‹0'  ‰Q‹4'  ‰Q‹€,'  ÇA$    ‰A ‰A1ÀÇA(    ÇA,    Ã¶    èüÿÿÿUWVSƒì‹zX€töæÿËÿÿÆD  ƒyd…Œ   Ç$    1Ûf‰‰Ë1Àëv ƒ<$uƒø)f‰tCƒÀf	1ƒøt(¶Wt…ÒtƒøØhöƒıw…Òt×1Òf‰TCƒÀƒøuØƒÃ0ƒ<$tÇ$   ë¤v ÇAh    1ÀÇAl    ÇAp    ƒÄ[^_]Ã¸êÿÿÿëñfèüÿÿÿU1ÀWy
Vƒçü‰ÎSY‹jXQÇAl    ÇAp    ÇAh 	  ÇA    ‰Ù)ùÇC\    ƒÁ`Áéó«1Àf‰F‹E`…Àtm‹Hd‹I…Étc‹I…Ét\‹=   …ÿt‹…ÿtèüÿÿÿ‰Áët& èüÿÿÿ‰Á…Éu2V61À´&    fƒÃ9ÓuöVfv fƒÃ9Úuöf‰F[‰È^_]Ã¹êÿÿÿëòt& èüÿÿÿUWV‰ÎSƒì‹BX·YÇAh 	  ÇAl    ‰$ÇAp    f…Û…¢   ‹$1Û1ÿ¶ht´&    ·L~U‰ú1Àè)úÿÿZf‰D~ƒÇ	Ãƒÿuá1Ò‰4$~0‰Ö´&    ·LwU‰ò¸   èöùÿÿZf‰DwƒÆ	ÃƒşuŞ‹4$f…Ût,NF61Ò¶    fƒÁ9ÈuöNfv fƒÀ9Èuöf‰VƒÄ1À[^_]Ã‹$1É1í¶xtf‰N´&    fW‰ê‰Ù1Àè„ùÿÿZf‰DnƒÅƒıuæ1ít& W‰Ù‰ê¸   èaùÿÿYf‰Dn6ƒÅƒıuãéÿÿÿ´&    ´&    èüÿÿÿUW‰ÏVS‹ZXè­şÿÿƒ»\&  „ˆ   ‹C`…À„¥   ‹Pd‹R…Ò„‡   ‹J…É„|   ‹   w…ÒtG‹j…ít@‰òèüÿÿÿ…Àu-Çƒ\&     ‹‹('  ‹Wy‰‹Vlƒçü‰Ql)ù)ÎƒÁpÁéó¥[^_]Ãv ‰òèüÿÿÿë¾´&    ‹ƒ8&  …Àjÿÿÿ¸ğÿÿÿëÓv [¸ııÿÿ^_]Ã¶    [¸íÿÿÿ^_]Ã¶    èüÿÿÿU½   WV‰ÖS‰Ë¹   ƒìh‹RXd¡    ‰D$d1À|$ÇD$   ó«‹{¹Ğ  ‹FT9Ï‰$‹COù¹   9ÏLùƒ<$uƒàà½    €zt‰{Éƒá Á@  9ÈOÁ9èLÅ‰C·Š  9ù„=  ‹Š8&  …É›  ‹KùHM12„   iè   ÇD$   ‰l$i,$ğ  ‰D$‰|$ÇD$   Õ‰|  ‹L$‰€  ‹L$‰„  f‰‚  ‹B`f‰º  …Àt6‹Pd‹R…Òt,‹z…ÿt%‹   L$…Ò„  ‹j…í„ø   1Òèüÿÿÿ‹FX‹VT·ˆ  ‰K·ˆ  ÇC   ‰KÇC   ƒ~T„   ÇCMPEGÇC   ÇC    ÇD$    ‹D$dd+    …©   ‹D$ƒÄh[^_]Ãt& ièp  ÇD$Ğ  Ñı‰l$éùşÿÿ´&    ·Š  9Á…´şÿÿi,$ğ  ‹KÇD$    9Œ*|  …•şÿÿëiÒğ  ÇD$    Ğ‹|  ‰S‹€  ‰S‹€„  ‰Cé_ÿÿÿ1ÒèüÿÿÿéÿÿÿÇD$ğÿÿÿéFÿÿÿèüÿÿÿfèüÿÿÿ‰Â=   t2 ¸   ƒút+1Àú   º   DÂÃt& 1Àú @  ”À€Ã¸   Ã´&    v èüÿÿÿU‰ÕWV‰ÆS1Û·8f‰1Û´&    fU‰ù‰Ú1Àè”õÿÿYf‰D^ƒÃƒûuæ1Ût& U‰Ú‰ù¸   èqõÿÿZf‰D^2ƒÃƒûuã[^_]Ã´&    ´&    èüÿÿÿS‰ÁPX21ÀfƒÂ9ÚuöƒÁbv fƒÂ9Ñuö[Ãt& èüÿÿÿÇ€     ÃèüÿÿÿW¹   S‰Ãƒì\d¡    ‰D$X1À|$Ç$   ó«·CÇD$   ‰ÁÑù…ÒDÁ‰D$·C
‰D$‹ƒPşÿÿ…Àt+‹Pd‹R…Òt!‹Z…Ût‹   ‰á…Òt&‹z…ÿt1Òèüÿÿÿ‹D$Xd+    uƒÄ\1À[_Ãv 1Òèüÿÿÿëßèüÿÿÿ´&    v èüÿÿÿ‰  1ÀÃfèüÿÿÿWV‰ÆS‹€¼   ‹H|‹†($  …À  ƒúuZAıƒàıt…ÉuN‹¾0.  …ÿ„   Ç†,%     ö    …ë   ‹†%  èüÿÿÿf…Àt9´&    f1À[^_Ã´&    v Ç†,%      ö    tİéù  ´&    €¾eşÿÿ‹–%  Àf% 0f f‰¶–dşÿÿ‹†%  èüÿÿÿë¢f1Ûët& ƒÃƒû „\ÿÿÿ¡,   ¹8  ºÀ  èüÿÿÿ‰„0.  …ÀuÕ{ÿ…Û„  f‹„¾0.  èüÿÿÿÇ„¾0.      ƒïsäé  ´&    ¸ğÿÿÿé0ÿÿÿé:  ´&    fèüÿÿÿƒúwVW‰×V° şÿÿS‹ şÿÿZ´9Öt.¶    ‹Cd‹@…Àt‹…Ét‹½|  ‰Øèüÿÿÿ‹CLX´9ÆuØ[1À^_Ã´&    v 1ÀÃffffffèüÿÿÿU‰ÍWVS…Ò~A‰Æ’‰Ë<BÏt& ÿ6ƒÆh"  jSƒÃèüÿÿÿƒÄ9ûuäÆ ‰è[^_]Ã´&    v ‰Ï‰èÆ [^_]Ãt& èüÿÿÿUW‰×VS1Ûìœ   ‰D$‹„$°   ‰L$‰D$d¡    ‰„$˜   1À¸  ët& ƒÃ[‹…   …À„Å  9øuçd¡    ‰D$0D$8‰D$8‰D$<¡    ÇD$,    ÇD$4    ¨t!ÿ  …´  öÄt%  =  „  [¶…¥  „À…w  ‹D$ÇD$ €   °˜1  ‰4$°ˆ   ‹€„   ‰t$ 	  ‰D$‹D$èüÿÿÿ‹t$‹.‰l$‰l$èüÿÿÿ‰ğv‹@9Å…ò  ‹L$‰Èƒàşƒøş„~  ƒÁ‰L$‹D$P ¸
   t& ‰:‹
9Ïtƒèuó‹D$1íP$‹D$…À~A‰\$(‹|$‰t$$‹t$f‹¯¸
   ´&    ‰
‹9ÙtƒèuóƒÅƒÂ9îuÙ‹t$$‹\$(‹D$1Éx<¸
   v ‰‹…Òtƒèuó‹T$‹L$¸
   ¶    ‰
‹*9étƒèuó‹T$¸
   ´&    v ‰‹9Êtƒèuó[¶…¤  ƒà<Àƒà
ƒÀ
èüÿÿÿ‰Å¡    %€  =€  „R  ‹$¹   T$,èüÿÿÿ‹D$º
   ‰t$‹t$ ‹€X&  ˆ 1Ç ‰1‹ƒøÿt!ğ9Ætƒêuì‹t$‹    ‹9D$…  ¡    )ĞT$,‰D$‹$èüÿÿÿ¡    ;l$†¾   %  =  „	  ‹t$1ÀN$‹t$v ‹‰ƒÀƒøuğ‹7‹D$èüÿÿÿ[ö…¤  …¿  1À…ötö    …™  ¸ûÿÿÿ‹”$˜   d+    …œ  Äœ   [^_]Ã<…C  ‹D$ÇD$    °¤1  ‰4$°œ   ‹€„   ‰t$   ‰D$é|ıÿÿ¨„Jÿÿÿ‹D$èüÿÿÿPkÃÿ°¨  ‹D$ƒÀPh  èüÿÿÿƒÄéÿÿÿt& ‰è‰T$èüÿÿÿ¡    ‹T$)Ğ‰D$‹6‹$T$,èüÿÿÿ9t$„Çşÿÿ‹D$èüÿÿÿ9l$‚ß   ö    …å   ¸êÿÿÿéÿÿÿt& 1ÒD$@½
   èüÿÿÿ´&    ‹$¹   T$@èüÿÿÿ‹9L$”À…íuR„À„=  ‹$T$@½   èüÿÿÿö    „²üÿÿ¸
   )èèüÿÿÿP‹D$ƒÀPhÈ  èüÿÿÿƒÄé‹üÿÿt& „À…ë   ‰èèüÿÿÿ‰ÅézÿÿÿÇD$    ÇD$   étüÿÿ1Ò¸,  èüÿÿÿé0şÿÿö    u<¸êÿÿÿé5şÿÿkÛ‹D$èüÿÿÿPÿ³¨  ‹D$ƒÀPh4  èüÿÿÿ¸êÿÿÿƒÄéşÿÿkÛ‹D$èüÿÿÿVÿt$Pÿ³¨  ‹D$ƒÀPh„  èüÿÿÿ¸êÿÿÿƒÄéÌıÿÿ‹T$‹D$L$UèGúÿÿPWkÃÿ°¨  ‹D$ƒÀPh  èüÿÿÿƒÄé+ûÿÿéy  ‹$T$@‰L$$èüÿÿÿ‹L$$9L$uZƒı
„pûÿÿé¬şÿÿ‹T$‹D$L$UèäùÿÿPh  ë—èüÿÿÿkÃ‹°¨  ‹D$èüÿÿÿVP‹D$ƒÀPh„  èüÿÿÿƒÄéÉüÿÿ¸
   ëfƒè„Â  ‹L$‰‹9ÑuëéÂ  d¡    ‰D$0D$8‰D$8‰D$<ÇD$,    ÇD$4    W‹D$ƒÀPh*  èüÿÿÿ¸êÿÿÿƒÄé´üÿÿèüÿÿÿW‰ÇVS‹B‰Ó…À…   ‹‡„   ¹   €  ‹S ‹09òuPp‹@9ÂtF¸
   ‰‹9Útƒèuó‹ŸX&  º
   Ã@1Ç f‰‹ƒøÿt!È9Átƒêuì[^_Ã´&    ö    u.ƒK[^_Ã´&    ƒøu‹‡„   ¹   @  éuÿÿÿéå  éÿ  t& èüÿÿÿU‰ÅWVSƒì,‹@‰D$‹E…À„  ƒøtÇE    ƒÄ,[^_]Ãt& ‹E@=  u>‹]pö    …@  º.   ‰Øèüÿÿÿ‹|$‹—4&  ƒâu¸…Àt´9Ãs°é"  ¶    = …8  ‹uD‹D$‰òèüÿÿÿ‰Ã…À„^  ‹T$‹EL1ÿƒÂ‰D$‰T$“¨  ‰T$…À„J  ‰,$ë5fƒø„  j ‹L$‰ê‰Øèüÿÿÿƒ»@  Y„  ƒÇ;|$„  ‹$‹tø`ö@t‹ƒL  9Æ‚  ƒp  9Æƒ  ‹$‰ò‹Lød‰Øèüÿÿÿ‰Å¡    %  =  „«  …í„K  ‹ƒ@  ƒø„  ƒø…]ÿÿÿ‹D$‹€0&  ‰D$…À„W  ‹E …Àt]E‹u‰Â‹E9Ât	9u„Ø  9ÖtA‰l$‰Õ‰\$‹\$‰|$ë‹V‹ƒ,&  ‹»0&  èüÿÿÿ‹69õt‹N…Éuß‹\$‹l$‹|$j ‹€  ‰ê‰ØƒÇèüÿÿÿX;|$…÷şÿÿt& ‹,$‹    ‹Ğ  ¸    èüÿÿÿ‹D$º   ¹   j ¼1  èüÿÿÿƒ»P  ÿZ„Ôıÿÿj ƒ\  ¹   º   èüÿÿÿXé·ıÿÿf‹ƒ4  …ÀtZ‹°ü  …ötP‹M …ÉtIU‹u‰Ñ‹U9Ñt	9u„½  9ñt-‰l$‰Íë‹ƒ4  ‹V  èüÿÿÿ‹69õt‹N…Éuà‹l$j ‰ê‹€  ‰ØèüÿÿÿZéşÿÿt& ‹E …À„Ë   ƒô  ‰D$èüÿÿÿ‹“ì  ƒì  9Â„Ÿ   ‹³ì  FØ‰t$èüÿÿÿ‰D$…À„   ‹VD‰ğ‹uM9ÎtS‰\$ ‰Ó‰l$$‰Í‰|$(‰Çë%;G$w‹D$‹VØèüÿÿÿ‹FGDÃ‹69õt‹N…ÉuÕ‹D$‹\$ ‹l$$‹|$(‹PD;“ä  sj‹    ƒø  ÂĞ  èüÿÿÿ‹D$èüÿÿÿj ‹€  ‰ê‰Øèüÿÿÿ^éıÿÿv j ‹L$‰ê‰ØèüÿÿÿXéıÿÿv ‹D$èüÿÿÿéğüÿÿ´&    v ‹t$ÇFD    èüÿÿÿ¹   ‰F‹F‰V‹‰B‰Fº   Ç   ÇF"  ÇFô   j èüÿÿÿXéHÿÿÿf‹N…É„işÿÿ‹V  èüÿÿÿéWşÿÿv ‹N…É„bıÿÿ‹D$‹V‹t$‹€,&  èüÿÿÿéGıÿÿéû  t& èüÿÿÿU‰ÕW‰ÇVSƒìTd¡    ‰D$P1À…Ò„ì  ƒú…£  ‹‡„   @  ‰D$1ö1Ûô1  ë´&    ƒÃƒÆtƒûF„J  1‹…ÒuèŒ7ä1  Ç    ‰$…É„)  kÃt‹L$‰\$ƒÁ øÇ€ü1      ‰¨ 2  1À‹‹$‰T@ƒÀƒøuì‹\$‹T$‹‹$‰A ‹B‹Q ‰A$9Â„  këtı‹… 2  …À„j  ƒø…İ   ‹…$2  =  „¤   = …S  ‹,$‹EL‹MHPÿƒú‡  ƒ}Dÿ„ú   ƒø´7D2  •À¶À,Å   1À´&    f—T&  ‹‰ƒÀ9èrëkÛtö„ü1  „  ‹D$Pd+    …  ‹—Ô1  ‹$ƒÄT¸    [^_]éüÿÿÿ´&    ‹T2  Æ ‹µ,2  …ö…™   ö…ü1  tm´&    ‹D$Pd+    …Ã   ƒÄT[^_]Ã´&    ö    …†  kÃtÇ„ü1     éÇşÿÿ‹‡„   €  ‰D$éşÿÿt& kÛtö„ü1  u›‹D$Pd+    ub‹$ƒÄT‰ø[^_]é=øÿÿt& ‰ò‰øèüÿÿÿ·T&  ‰Ø¹ü   ‰òèüÿÿÿÆƒü    º  Ü ‰øèüÿÿÿö…ü1  …áşÿÿ‹$‰øèñ÷ÿÿéÒşÿÿèüÿÿÿGPh$  èüÿÿÿXZéÿÿÿfèüÿÿÿéfñÿÿ¶    èüÿÿÿWVS‹L$‹|$…É~D$$‰úˆv ‹püƒÀƒÂ‰rü9ØuğW‹T$‹D$è"ñÿÿZ[^_Ã´&    ¶    èüÿÿÿUWVSƒìd¡    ‰D$1À‹\$0‹t$4‹L$8…Û„  ƒùó  T$<‰å…É~"ƒÂ1À‰å´&    v ‹züƒÂ‰|… ƒÀ9È|ïU‰ò‰Øè¥ğÿÿZ‹T$d+    uƒÄ[^_]Ãèüÿÿÿ´&    v èüÿÿÿV‰ÆS‹˜8  ¸   ‹“  öƒ  u1À…Ò•ÀRPjÿ¶D  jh	  Sèüÿÿÿº   ƒÄöƒ  u‹‹  1Ò…É•Â…Àt[^Ã´&    v ÿ³  Rj ÿ¶D  jh	  SèüÿÿÿƒÄ…ÀuĞj ‹ƒ  ÁèPjÿ¶D  jh	  SèüÿÿÿƒÄ[^Ã¶    èüÿÿÿS‹L$‹˜8  ú¡   „ä  ‡¾   ú™   „ò  wpú•   „  v2ú—   …  ÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã´&    fú   …´   ÿ1j j j j ÿ°D  jh  SèüÿÿÿƒÄ$[Ãfú   „¼  úŸ   …¨   ÿqÿqÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ [Ãt& úĞ   „”  †¾   úÚ   „¢  úÜ   …~   ÿqÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã¶    ú‘   …­  ÿ1ÿqÿ°D  jh  SèüÿÿÿƒÄ[Ã´&    ú›   …x  ‹‹IƒâÁá	Ê‰“  [é»ıÿÿv úÙ   …Z  ÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ãfú½   „  vúÅ   …  1À[Ãú¹   uÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ãú»   tÒé*  v ÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ãv ÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã¶    ÿqÿqÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ [Ãt& ‹‰“  ‹Q‰“  [éÁüÿÿÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã¶    ÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã¶    ÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ãé*  é*  é*  é*  é*  ffffffèüÿÿÿUWVSƒì<‰$‰T$d¡    ‰D$81Àƒù…ˆ  ‹z‰ĞƒÀ‰|$‰D$ 9ø„p  ‹D$‹h…í„a  ‹x‰|$èüÿÿÿ‹$ƒ¸\&  „W  ‹D$ÇD$    8?ÿÿÿu	‹@È‰D$‹$€xu‹D$ÿƒçXEÇ  ƒı†E  º   ët& ƒÂƒÃ9Õ„
  €;ÿuí€{ uç€{ uá¶Kƒá¿€ù°uÕ)Ğ9Ç‡ğ  v 1ÒÇD$    ƒÃ1ö÷÷‰l$‰Å´&    f€{üÿ…  €{ı …ü   €{ş …ò   ¶Cÿƒà¿<°…ã   ‹$‰\$,‹@`…ÀtE‹Pd‹R…Òt;‹
…Ét5‹   …Ò„K  ‹   ‹‰T$…Ò„7  ‹L$T$(èüÿÿÿ¶    ‹T$4…Ò„„   ‹L$‰ÈÁà$‰@'  ‹T$(‰D'  ‹T$0‰H'  ‹$‰ÈÁà„P'  ‹T$,‹
‰‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J ‰H ‹J$‰H$·R(f‰P(ƒD$t& ƒÆû9õ‡ãşÿÿ‹D$‹<$‹l$Ÿ@'  ‰D$…À„œ   ‹L$‹D$‰ÚÁá‰H‹D$èüÿÿÿ‹$‹€<'  …À…Á  ‹$‹€8'  ‰D$ƒÀ‹<$‹L$‰‡8'  ‹|$‹GA )è‰A ‹‰D$;D$ …ıÿÿ‹D$8d+    …8  ƒÄ<[^_]Ã´&    ¸   9Ç†şÿÿ‹$˜@'  ‹<$‹D$‹t$Ç‡@'      Ç‡H'      Ç‡D'      Ç@@   ‹‰‹C‰F‹C‰F‹C‰F‹C‰F‹C‰F‹C‰F‹C‰F‹C ‰F ‹C$‰F$‹C(‰F(‹C,‰F,‹C0‰F0‹C4‰F4‹C8‰F8‹C<‰F<‹—<'  …Ò„òşÿÿ‹<$ÇD$    ÇD$    ‹‡8'  ÇD$   ƒà‰D$$‹´‡@0  1À‰l$ƒÃ1ÿ‰Åëp‹L$«Á‰L$‰Ğèüÿÿÿ‰ÂkÅ+ƒÅˆT,‹D-‰‹S‰P‹S‰P‹S‰P‹S‰P‹S‰P‹S‰P‹S‰P‹S ‰P ‹S$‰P$·S(f‰P(ƒÇƒÃ@9|$~8‹Sğ…Òtí‹Køƒ{ô AuAúƒøqÿÿÿ‹L$ƒè «Á‰L$éiÿÿÿ¶    ‰èÇ  º‹l$ÇFD fÇF$ÑÇFÓúÿÿÇF  ½ÇF „€ÇF! ]ÇFc§ÿÿƒø$„  ‰Á€‹T$‹|$‰ËƒDAƒàüƒÀ·ØƒÀ
K
ƒÃ ¶ÍÇF itv0‰V$‰~(‹|$ˆFˆN‰øÁèƒàƒÈ!ˆF‰øÁèˆF‰øÁèƒÈˆF‰øÁèˆF‰ø‹|$$D ˆF‹$‰œ¸À0  é?ıÿÿ‹<$ÇD$    ÇD$    ‹‡8'  ƒà‰D$$‹´‡@0  ‹D$…ÀIşÿÿÇ  º»,   1É1ÒÇFD f¸   1ÿÇF$ÑÇFÓúÿÿÇF  ½ÇF „€ÇF! ]ÇFc§ÿÿé%ÿÿÿT$(èüÿÿÿéÎûÿÿ‹¸,'  ‹T$1Û‰|$ÿB‰|$„´   ƒï‰l$‰ıë=t& ‹ˆœ  z‰Æƒçü‰Šœ  ‰ÑÂ   )ù)ÎÁ   Áéó¥ƒÃ¤  ;\$th€xüÿub€xı u\€xş uV¶Hÿƒá¿€ù uJ‹‰
9İu¡‹ˆ  z‰Æƒçü‰Š  ‰ÑÂ”  )ù)ÎÁ”  Áéó¥¶ˆ“  iÉ‰
‰J‰JëŠ‹l$iD$@  ‹|$‹t$‰G‹<$‹—8'  ‰Tü‹‡8'  ‰D$ƒÀé½ûÿÿ¹  V,F$»0  ÇF ITV0èüÿÿÿ¸   ¹   éôıÿÿ‰èéìùÿÿèüÿÿÿfèüÿÿÿUWV‰ÆSƒì‹€4&  ‹Vh¨ „1  Bp‰$‹Fd…ÀtD‹Pd‹R…Òt:‹Z…Ût3‹<$‹   ·W…É„n  ‹y…ÿ„c  j 1ÉèüÿÿÿY´&    f‹Fn1ÒX´9Åuë_´&    ‹CLX´9ÅtN‹Fh‹@…Àt#ƒ   t%‹Cd‹@…Àt‹x…ÿt‹$1É‹Pj ‰Øèüÿÿÿ‰ÂX…Òt»úııÿÿt³ƒÄ‰Ğ[^_]Ã‹†X&  ‹˜ Ç ‹$‰ßƒçÏ‹@…À„±   ƒøtd‰øƒÈ 9Øtg‰Ã‹X&  ‰ßº
   Ï   Á Ç ´&    ‰9‹ƒøÿt1Ø¨0tƒêuìƒÄ1Ò[‰Ğ^_]Ã‹†˜Y  @D‚L‰$éÀşÿÿv ‰øƒÈ9ØuœƒÏ ‹†X&  ‰ıº
   Í   ˆ Ç v ‰)‹ƒøÿt
1ø¨0„lÿÿÿƒêuèébÿÿÿv 9ßt½‰ûéTÿÿÿt& j 1ÉèüÿÿÿZé¡şÿÿèüÿÿÿWVS‹X`…Ût9‹Sd‹R…Òt/‹2…öt)‹Hh‹=   ‹€œY  ‹TÁ …ÿt‹?…ÿtj 1É‰ØèüÿÿÿZ[^_Ãt& j ‰Ø1ÉèüÿÿÿX[^_ÃèüÿÿÿUWVS‰Óƒì‹‚X&  ‹°1Ç ‹‚X&  #²È1  ‹¸D1Ç ‹‚X&  #ºÌ1  Ä0Ç ‹ #‚Ğ1  …öu7‰ò	ú‰$…ÿuc…À…Œ   ‹,$…í…Z  ‹$1À…Ò•ÀƒÄ[^_]Ã´&    ‹’X&  ª1Ç º
   ´&    ‰u ‹M ƒùÿt…Ît¢ƒêuì‰ò	ú‰$…ÿt‹“X&  ªD1Ç º
   ´&    ‰} ‹M ƒùÿt…Ïtƒêuì…Àt/‹‹X&  º
   ©Ä0Ç ´&    ‰E ‹M ƒùÿt…Ètƒêuì	$‹    â€  ú€  „C  …öt÷Æ   uuæ   u]…ÿ„ÿÿÿ÷Ç   u-ç€   „ÿÿÿj ¹   ƒ˜1  º   èüÿÿÿYéëşÿÿt& j ƒ¤1  ¹   º   èüÿÿÿ^ë¹¶    1Ò‰Øèüÿÿÿë˜t& º   ‰Øèüÿÿÿézÿÿÿ‹    â€  ú€  …ZÿÿÿéC  fèüÿÿÿ‰Á‰Ğ‹‘4  …Òu}V‹±”   S‹X&  ÃÄ ‹ã    »ÿÿ  EÓƒH(p‰P<‹–X&  ‹šÄ ‰Úsşƒâƒú”Â¶ÒƒÂæı   ¾   DÖ‰ÖƒÎƒãEÖ‰P4‹‘L  ‰P81À[^Ã´&    1ÀÃ´&    ¶    èüÿÿÿÇ€4     1ÀÃ´&    ´&    èüÿÿÿUWVSƒì‹¨”   ‹…X&  ‹° Ä ‹…X&  ‹¸Ä ‹…X&  Ä ‹ ¹H  ©    ¸G  DÁ‹ğ     ƒæ‰$PSh    èüÿÿÿ‰ø¶Ôƒâÿ4•   ShL   èüÿÿÿ‰ğƒÄ„À„\  é"  ´&    ¶    èüÿÿÿV‰Á¾S‹€X&  Ä ¸
   ‰2‹ûtƒèuï‹X&  ¾şâ+ Ä ¸
   ´&    f‰2‹ûşâ+ tƒèuï‹X&  ¾­'R Ä ¸
   ´&    f‰2‹û­'R tƒèuï‹X&  ‹$Ä ‹‰X&  âÿÿÿ ¸
   Ê   VÁ$Ä ¶    ‰‹9Útƒèuó[^ÃèüÿÿÿVSƒì‹˜”   ö    …×#  ‹ƒX&  Ä …Òtl‹‹ƒX&  0ö€ÎŒˆÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹Ä ‹‹X&  âÿÿ ÿ¸
   Ê   ÁÄ f‰‹9ÚtƒèuóƒÄ1À[^Ãv ‹‹ƒX&  0íÄ ¸
   v ‰
‹29ñtƒèuó‹ƒX&  ‹ˆÄ ‹“X&  áÿÿ ÿ¸
   ÂÄ ‰
‹9Ùt¥ƒèuóƒÄ1À[^Ã´&    t& èüÿÿÿUWVSƒì‹˜”   ‹q‰D$‹€8  ‰Å‰$å ù  …ö…œ  ‹q‰Êƒş…‡  ÇA   ÇA   ‹ƒX&  ‹¸tÄ Áï‹ƒX&  çğ  ‹ˆtÄ Áé‹ƒX&  ƒá	ù‹¸pÄ ‹ƒX&  Áïçğ  pÄ ‹ Áèƒà	ø‹z…í‰<$¿   D÷‹z4$‰ıÁå9Å‚$  9Ç‡$  ,õ    9ÍŒ$  9ñŒ$  ‹…Ò„   Áà1Ò÷÷¨  ğÿ‰ÈÁà	™÷ş¾   ‰D$¸  +D$%ÿ  ‰D$ÿ€  †w  ö    …'$  ‹ƒX&  Ä ‹ ‰é0À¶Ñ	Â‹ƒX&  ˆÄ ¸
   v ‰‹99útƒèuó‹ƒX&  ‹Ä ‰è0ö% ÿ  	Â‹ƒX&  ˆÄ ¸
   v ‰‹99útƒèuó‹ƒX&  ‹Ä ‰èâÿÿ ÿ%  ÿ 	Â‹ƒX&  ‰ÕÄ ¸
   t& ‰*‹
9Ítƒèuó‹ƒX&  ‹Ä ‹D$‹»X&  0Ò÷ØÄ ¶À	Ğº
   ‰‹99øtƒêuó‹ƒX&  ‹Ä ‹D$0ö0À	Â‹ƒX&  ˆÄ ¸
   t& ‰‹99útƒèuó‹ƒX&  ‹Ä ‹‹X&  âÿÿ ÿ¸
   	òÁÄ ¶    ‰‹9Útƒèuó1ÀƒÄ[^_]Ã´&    f¾  	 ÿÀ   ‡xşÿÿƒÿaöæ   Æ  
 ébşÿÿ¸êÿÿÿëÁ¸êÿÿÿëº´&    t& èüÿÿÿWVS‹P‹JÔ‹P(ú	˜ „  waú 	˜ „µ   ú	˜ …™   ‹@|‹±X&  À‹–Ä ‹‰X&  ¶À0öÁà	ÂÁÄ ¸
   t& ‰‹9Útƒèuó[1À^_Ãt& ú	˜ uH‹@|‹¹X&  ‹— Ä ‹‰X&  Áàâÿÿ ÿ%  ÿ 	ĞÁ Ä º
   ´&    ‰‹9Øt­ƒêuóë¦[¸êÿÿÿ^_Ã´&    ‹@|‹¹X&  ƒÀ€‹—Ä ‹‰X&  ¶À0Ò	Â¸
   ÁÄ ¶    ‰‹9Ú„YÿÿÿƒèuïéOÿÿÿ´&    v ¶x|‹±X&  ?‹ Ä ‹¹X&  ¶Ò0Û	Óº
   · Ä t& ‰‹>9ûtƒêuó‹@|‹±X&  À‹– Ä ‹‰X&  ¶À0öÁà	ÂÁ Ä ¸
   ´&    ‰‹9Ú„Éşÿÿƒèuïé¿şÿÿ´&    v èüÿÿÿUWV¾   Sƒì‹˜”   ‰$‰Øèüÿÿÿ‹ƒX&  º
   ˆ Ä ¶    ‰1‹ƒøÿt%   =   tƒêuæ‹ƒX&  ‹¨  Ä ‹ƒX&  ‰ï‰îº
   ƒÏæşÿ  ˆ  Ä t& ‰9‹ƒøÿt	1è©şÿ  tƒêué‹ƒX&  º
   ˆ  Ä ´&    ‰1‹ƒøÿt·À9ğtƒêuë‹ƒX&  XÄ ‹ ‹»X&  %ÿşÿáº
   XÄ ´&    ‰‹19ğtƒêuó‹»X&    º
   XÄ t& ‰‹19ğtƒêuó‹ƒX&  \Ä ‹ ‹»X&  %ÿşÿáº
   \Ä ´&    ‰‹19ğtƒêuó‹»X&    º
   \Ä t& ‰‹19ğtƒêuó‹ƒX&  ¾
 4Ä ¸
   t& ‰2‹
ù
 tƒèuï‹ƒX&  ‹°<Ä ‹ƒX&  ‰õ‰÷º
   åŸ ƒÏˆ<Ä ƒÍf‰9‹ƒøÿt	%Ÿ 9Åtƒêué‹ƒX&  ‰÷º
   æŸ ƒçşˆ<Ä t& ‰9‹ƒøÿt	%Ÿ 9Ætƒêué‹ƒX&  ‹ˆÄ ‹ƒX&  É  Ä ¸
   ¶    ‰
‹29ñtƒèuó‹ƒX&  ‹ˆÄ ‹ƒX&  ƒÉÄ ¸
   v ‰
‹29ñtƒèuó‹ƒX&  ¾ €  ¤Ä ¸
   t& ‰2‹
ù €  tƒèuï‹ƒX&  1ö¤Ä ¸
   t& ‰2‹
…Étƒèuó‹ƒX&  ‹ Ä ‹ƒX&  âÿÿûÿÊ   ˆ Ä ¸
   f‰‹19òtƒèuó‰Øèlöÿÿ‹ƒX&  ‹ Ä ‹ƒX&  âğç÷ÿÊ ˆ Ä ¸
   v ‰‹19òtƒèuó‹ƒX&  ‹ˆ Ä ‹ƒX&  É  P  Ä ¸
   ‰
‹29ñtƒèuó‹ƒX&  ‹ŒÄ ‹ƒX&  0ö€Î ˆŒÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ¾.%@Ä ¸
   t& ‰2‹
ù.%@tƒèuï‹ƒX&  ‹Ä ‹ƒX&  â   ÿ€Î]ˆÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ¾(fxÄ ¸
   t& ‰2‹
ù(ftƒèuï‹ƒX&  ÔÄ ‹ ¶Ğ<ä‡—   ƒúvJ¸ä   )ĞÑøƒÀÁà	‰Ã‰ÁÁû‹<$‹—0  ‰JD‰ZH‹—0  ‰‚€   ƒÄ‡À   [^_]éüÿÿÿ´&    ‹ƒX&  ‹ÔÄ ‹‹X&  0Ò¸
   ƒÊÁÔÄ ¶    ‰‹9Útƒèuó¹ ş  1Û¸ ş  ë‰t& ‹ƒX&  ‹ÔÄ ‹‹X&  0Ò¸
   €ÊäÁÔÄ ¶    ‰‹9Útƒèuó¹ .  1Û¸ .  é>ÿÿÿfèüÿÿÿè¶úÿÿ1ÀÃv èüÿÿÿ‹T  …Òt	1ÀÃ¶    Ç€T     è‰úÿÿ1ÀÃ¶    èüÿÿÿWV‰ÎS‰Ã‰Ğâüÿ  ƒàÂ  Ä Å    ‹ƒX&  Ğ‹8¸ÿ   “X&  Óà÷Ğ!Ç‰ğ¶ğ‰øÓæ	ğ‰Á¸
   ´&    ‰
‹9Ùtƒèuó[1À^_Ãt& èüÿÿÿUW‰×‰ÊV‰ÆSƒì‹D$‹\$‰$‰øƒç%üÿ  ı    ‹¾X&    Ä Ç‹/¿ÿ   ¶Ò¶Û†X&  ÓçÓâ¾
   ÷×Óã!ï	ú¶<$Óçv ‰‹ƒùÿt1ù…ÙtƒîuìƒÄ1À[^_]Ã´&    v èüÿÿÿWVS‹¸4  …ÿu^‰Ã‹€”   ‰Ö‹ˆX&  ‹‰Ä ‹R8ÁéƒáğƒútRw(…ÒuD¶Éhÿ   º	  Qèüÿÿÿ‹F8‰ƒL  XZ‰ø[^_Ãƒútƒúu#ƒÉëÍ1ÿ[^‰ø_Ã´&    ƒÉë·v ƒÉë¯¿êÿÿÿëÆ´&    t& èüÿÿÿUWVS‹°è  ‰Ã‹¸ì  ‹“X&  ‰ğ% °  ƒøÀÂœÄ %   ı   ‹âÿÿÿ 	Â‹ƒX&  ˆœÄ ¸
   t& ‰‹)9êtƒèuó‹ƒX&  ‹ˆ Ä Áéº  ‰ØáŸ   èüÿÿÿ‹ƒX&  ‹ˆ Ä Áéº  ‰ØƒÉ`¶Éèüÿÿÿ‹ƒX&  ÷Æ ù  „¯   ‰òÄ €ö 	ú„¥  ö €  	ş„  ‹0‹ƒX&  æ ÿÿÿÎö   ˆÄ ¸
   ´&    ‰1‹ƒúÿt€úötƒèuí‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ t& ‰1‹ƒøÿt%   ?=   tƒêuæ‹ƒX&  ë¶    ‰ò„Òujæ  ÿ …   Ä ‹ ‰ÁÁé©   u[^_]Ã¶    ‰Îáï   jº  Qƒæï‰Øèüÿÿÿ‰ñ‰Øº  ƒÉj¶ÉQèüÿÿÿƒÄ[^_]Ã´&    ‹°Ä ‹ƒX&  Îÿ   º
   ˆÄ v ‰1‹ƒøÿt
¶À=ÿ   tƒêuè‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ ´&    ‰1‹ƒøÿt%   ?=   „ÿÿÿƒêuâ‹ƒX&  éÿÿÿ´&    ‹0‹ƒX&  æ ÿÿÿÎ÷   ˆÄ ¸
   ‰1‹ƒúÿt€ú÷tƒèuí‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ t& ‰1‹ƒøÿt%   ?=   „qşÿÿƒêuâégşÿÿt& ‹0‹ƒX&  æ ÿÿÿÎø   ˆÄ ¸
   ´&    f‰1‹ƒúÿt€úøtƒèuí‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ t& ‰1‹ƒøÿt%   ?=   „áıÿÿƒêuâé×ıÿÿt& ‹°Ä ‹ƒX&  Îÿ   º
   ˆÄ v ‰1‹ƒøÿt
¶À=ÿ   tƒêuè‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ ´&    ‰1‹ƒøÿt%   ?=   „QıÿÿƒêuâéGıÿÿ´&    ¶    èüÿÿÿ‹€”   èĞûÿÿ1ÀÃ´&    ¶    èüÿÿÿUW‰×V‰ÎS‰Ãƒìd¡    ‰D$1ÀÇ$    ÇD$    ÇD$    ö    …K$  Gÿƒø‡Š  Ç$   Oï1íƒş„
  ‡7  ƒş„.  ƒş…  ÇD$   ƒáÏƒÉ´&    ¶Éh÷   º  ‰ØQèüÿÿÿ‹ƒX&  ‹ˆ Ä Áéº  ‰Øƒáù	é¶Éèüÿÿÿ‹ƒX&  ‹ˆ Ä XZÁé‹l$…í„›  ‹D$…À„  ƒÉ¶Éjº  ‰ØQèüÿÿÿ‹ƒX&  Ä ‹ ]%   ÿY‹l$‰Â€ÎP…ít‹L$…Ét0P  ‰Â1É½   ‹Œƒøt2‡½  ƒø„\  …À„¬  …Ét€Î ƒù„«  ƒÁ‹ŒƒøuÎ¸@  Óà	Â…Éuß€æ¿ëãƒş†şşÿÿéf$  ´&    fƒşuÇD$   ƒá?éÛşÿÿv ƒş…
  ƒá?ÇD$   ƒÉ@é¼şÿÿt& ÿÿ  †¬   ‰ú‰ù‰øâ ğ  á   %  ğ ÷Çÿ ÿ……$  ê   ú p  ‡…$  é   ù   ‡…$  -  p =   ‡…$  ÁùÁúÇ$   ½   	ÑÁøÇD$   ÇD$   	Áƒş…öıÿÿƒáÏÇD$   ƒÉ éşÿÿƒáıƒáûéişÿÿt& ‰ø‰ú%ğ   â   ÷Çğÿÿ……$  ƒèƒøp‡…$  ª üÿÿı   ‡…$  ÁøÇ$   Hğúÿ     ê   ƒá?½   ÇD$   Áú	ÑéXıÿÿ¶    ‰èÓà	Âƒù…Uşÿÿ‹ƒX&  ˆÄ ¸
   ´&    ‰‹)9êtƒèuó‰»ğ  ‰Ø‰³ô  èüÿÿÿ‰Øèiøÿÿ1À‹T$d+    u`ƒÄ[^_]ÃfƒÉéÿÿÿ´&    ÁıƒáÏÇD$   	é½   éÀüÿÿ¶    ÇD$   ƒáÏéàüÿÿ‰èÓà	Â…É…¥ıÿÿ€æïé¦ıÿÿèüÿÿÿéf$  èüÿÿÿ‹ˆD  ‹€”   éüÿÿ´&    v èüÿÿÿ‰Ñ‹@  ‹€”   éøûÿÿ´&    èüÿÿÿS‹€X&  ·Ò”  Ä ¸
   t& ‰
‹9Ùtƒèuó1À[Ã´&    ´&    èüÿÿÿU·ÒWVS‰Ëƒì‹€X&  ‹|$‹t$Ç$
   Œ  Ä !şƒşÿ•À‰Å´&    f‰‹ƒøÿu‰ê„Òu!ø9Ætƒ,$uåƒÄ1À[^_]Ã´&    t& èüÿÿÿ‹€X&  ·Ò„  Ä ‰1ÀÃ¶    èüÿÿÿ‰Á‰Ğ‹‰X&  %üÿ  „  Ä ‹ ƒâÕ    ÓèÃ¶    èüÿÿÿ‹€X&  ·Ò„  Ä ‹ Ã´&    èüÿÿÿU‰Õ·ÒW‰ÏVS‰Ã‰è‹t$‹‹X&  %üÿ  „  Ä ‹ ƒåí    Óè‰Á‰Ø[!ù	ñ^_¶É]é¤ôÿÿt& èüÿÿÿS·Ò‹˜X&  Â  Ä Ó‹!ÙX&  L$¸
   ¶    ‰
‹9Ùtƒèuó1À[Ã´&    ´&    èüÿÿÿUWVS‰Ãƒì\‹€è  ‹³ì  ‹»X&  ‰Á€åO—œÄ 	ñ„í   ‹
‹»X&  áÿÿÿ º
   É   ÇœÄ ¶    ‰‹/9étƒêuó‰Ççÿÿ ‰|$(„ì   „À„ù  ÇD$„   ½ ‚  ¹<‚
 ÇD$   ]ÇD$$&   ÇD$ 0  ÇD$  $ ÇD$   0ÇD$   ÇD$    ÇD$ <   Ç$  
 ÇD$H    ÇD$D   ÇD$@   ÇD$<0   ÇD$8C  ÇD$4&   ÇD$0]   ÇD$,„   é8  v ‹
‹»X&  áÿÿÿ º
   É   ÇœÄ ‰‹/9é„ÿÿÿƒêuï‰Ççÿÿ ‰|$(…ÿÿÿ‰Â€ö	ò„ğ  €ôÇD$z   	ğÇD$$   ƒøÇD$   ÿÇD$   ç   ÇD$   &Ç   ZƒøÇD$  P ‰|$ÿç   ºÇ$   Ç   fƒøÇD$D   ‰|$ÿç   ƒøÇD$@   í‰|$ å ıÿÿÇD$<&   Å |  ƒøÇD$8á  ÿÇD$4   ƒçºÇD$,z   ƒÇfƒøÉ‰|$HáıÿÿÁ | ƒøÀƒàƒÀZ‰D$0‹ƒX&  ‹Ä ‹ƒX&  ¶Ò‹¸Ä ‹ƒX&  çÿÿÿÄ ‹ ¶ôö    …%  …ötO¸Èô´÷é‰Ç‰Ö…Òt@ÇD$L    ‰ğº¹ˆbñ÷â‰T$PÁê	iÂ  )Æ‰ò‹t$PÁî	‰ø¿  ÷÷‰ò‹t$L…ö…¤$  ‹ƒX&  ‹pÄ ‹ƒX&  0ÒT$ˆpÄ ¸
   ´&    ‰‹19òtƒèuó‹ƒX&  ‹ˆpÄ ‹ƒX&  0ípÄ ¸
   t& ‰
‹29ñtƒèuó‹ƒX&  ‹pÄ ‹ƒX&  âÿÿ ÿÊ  - ˆpÄ ¸
   ´&    v ‰‹19òtƒèuó‹ƒX&  ‹pÄ ‹ƒX&  âÿÿÿ T$ˆpÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹tÄ ‹ƒX&  0ÒT$$ˆtÄ ¸
   ´&    ‰‹19òtƒèuó‹ƒX&  ‹tÄ ‹ƒX&  0öT$ˆtÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ‹tÄ ‹ƒX&  âÿÿ ÿT$ˆtÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹tÄ ‹ƒX&  âÿÿÿ T$ˆtÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹xÄ ‹ƒX&  0ÒƒÊˆxÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ‹xÄ ‹ƒX&  0ö€ÎˆxÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ‹xÄ ‹ƒX&  âÿÿ ÿT$ˆxÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹xÄ ‹ƒX&  âÿÿÿ T$ˆxÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹|Ä ‹ƒX&  0ÒT$ ˆ|Ä ¸
   ‰‹19òtƒèuó‹ƒX&  ‹|Ä ‹ƒX&  0ö	êˆ|Ä ¸
   f‰‹19òtƒèuó‹ƒX&  ‹|Ä ‹ƒX&  âÿÿ ÿ$ˆ|Ä ¸
   t& ‰‹19òtƒèuó‹|$(ƒÿÒƒÂƒÿÀ‰“  ƒàƒÀ÷ßÉ‰ƒ  ‹ƒX&  á   ‹|Ä âÿÿÿ ¸
   	Ê‹‹X&  Á|Ä ´&    f‰‹9ÚtƒèuóƒÄ\[^_]Ãv €ôÇD$„   	ğÇD$   ]ƒøÇD$$&   ÿÇD$ 0  ç   ÇD$  $ Ç  € ƒøÇD$   0‰|$ÿç    ƒøÇD$@   ‰|$ÿƒçLÇD$<0   ƒÇ:ƒøÇD$8C  í‰|$ å ;  ÇD$4&   Å B  ƒøÇD$0]   ÿÇD$,„   ç  şÿÇ  
 ƒø‰<$ÿ1Éƒç ƒø’Á‰|$H‰L$DÉáL;şÿÁ:B
 éõúÿÿÇD$z   ½ ‚  ¹<‚
 ÇD$   ZÇD$$   ÇD$   ÇD$   ÇD$   &ÇD$   ÇD$    ÇD$ <   Ç$  
 ÇD$H    ÇD$D   ÇD$@   ÇD$<&   ÇD$8á  ÇD$4   ÇD$0Z   ÇD$,z   éWúÿÿfèüÿÿÿWVS‹°4  ‹˜”   …öu‹°<  ‹¸8  1Î1×	ş„   ‰Ö‰8  ö    ‰ˆ<  Ç€4      	Î„ê   ‰Öö @  	Îtf‰Öö   	Î„>  ‰Öö   	Î„Î   ‰Öö   	Î„.  ‰Öö   	Î„>  öÆ°…%  „Ò…=  â  ÿ ÷ÚÒ‰×ƒçë	t& ¿   ö    …;&  1ö‹ƒX&  ‹ˆ Ä ƒáĞº   ‰Ø	ùƒÉ ¶Éèüÿÿÿ‹ƒX&  ‹ˆ Ä Áéº  ‰Øƒáü	ñ¶Éèüÿÿÿ‰Øèüÿÿÿ‰ØèÀìÿÿ[1À^_Ã´&    v ¿   ë†´&    f1ö¿   ö    …Y&  ‹ƒX&  ‹ˆ Ä áğ   º   ‰ØƒÉèüÿÿÿ‹ƒX&  ‹ˆxÄ Áéº{  ‰Øáù   èüÿÿÿé1ÿÿÿt& ¾   ¿   ë›t& 1ö¿   ë´&    ¿   éóşÿÿ¶    ¿   éãşÿÿ¶    1ö¿   é[ÿÿÿt& èüÿÿÿUWVS‰Ã‹€X&   Ä ‹ %ÿÿ  «°   Çƒğ     ºà  ‰ƒ   ‰è³p  Çƒô     Çƒø  €»  Çƒü     Çƒ      Çƒ     èüÿÿÿ‹ƒ   ‰›D  ÁèPCPƒ   hñ  j Pèüÿÿÿ1Éº	   ‰ğj Çƒ@     èüÿÿÿ¹ 	˜ º0  ‰ğj h€   j jj hÿ   j j èüÿÿÿ¹	˜ º0  ‰ğƒÄ8j j@j jj jj j èüÿÿÿ¹	˜ º0  ‰ğƒÄ j j@j jj jj j èüÿÿÿ¹	˜ º0  ‰ğƒÄ j j j jj jjÿj€èüÿÿÿ¹	˜ º    ‰ğƒÄ j j j h  j hÿÿ  j j èüÿÿÿ¹		˜ º    ƒÄ ‰ƒà  ‰ğj j j jj jj j èüÿÿÿ¹	˜ º    ‰ğƒÄ j h €  j h  j hÿÿ  j j èüÿÿÿ¹	˜ º    ‰ğƒÄ j h €  j h  j hÿÿ  j j èüÿÿÿ¹	˜ º    ‰ğƒÄ j h €  j h  j hÿÿ  j j èüÿÿÿ‹»¬  ‰³  ƒÄ …ÿuC‰êèüÿÿÿ‰Ç…Àt‰ğèüÿÿÿ‰ø[^_]Ã´&    f‰ØèùÛÿÿ‰ø[^_]ÃfèüÿÿÿVú }  ‰Ö•ÂşD¬  S‰Ã•À„Âtş€»  …ı  ƒ»ô  v2şD¬  „  ş€»  „r  ş }  „Æ   ‰³ø  1À[^Ãv şD¬  „$  ş€»  „€  ş }  uÒ¹0º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹tvº  ‰Øèüÿÿÿ¹  ºø  ‰Øèüÿÿÿ¹  º 	  ‰Øèüÿÿÿ¹  º	  ‰Øèüÿÿÿ¹  º	  ‰Øèüÿÿÿ¹p   º'  ‰Øèüÿÿÿ¹ÿ é‚   ¶    ¹ º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹tvº  ‰Øèüÿÿÿ¹÷º 	  ‰Øèüÿÿÿ¹÷º	  ‰Øèüÿÿÿ¹÷º	  ‰Øèüÿÿÿ¹`   º'  ‰Øèüÿÿÿ¹ÿ/ º,  ‰Øèüÿÿÿ¹ø. º(  ‰Øèüÿÿÿéœşÿÿf¹º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹­'R º  ‰Øèüÿÿÿ¹ªOº 	  ‰Øèüÿÿÿ¹ªOº	  ‰Øèüÿÿÿ¹ªOº	  ‰Øèüÿÿÿ¹V   º'  ‰Øèüÿÿÿ¹ÿ_ º,  ‰Øèüÿÿÿ¹ø“ º(  ‰Øèüÿÿÿéüıÿÿf¹º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹ò¡b º  ‰Øèüÿÿÿ¹Ymº 	  ‰Øèüÿÿÿ¹Ymº	  ‰Øèüÿÿÿ¹Ymº	  ‰Øèüÿÿÿ¹X   º'  ‰Øèüÿÿÿ¹ÿ’ º,  ‰Øèüÿÿÿ¹øK º(  ‰Øèüÿÿÿé\ıÿÿf¹ º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹tvº  ‰Øèüÿÿÿ¹ €ºø  ‰Øèüÿÿÿ¹UUº 	  ‰Øèüÿÿÿ¹UUº	  ‰Øèüÿÿÿ¹UUº	  ‰Øèüÿÿÿ¹`   º'  ‰Øèüÿÿÿ¹ÿ? éªşÿÿ¶    ¹$º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹ò¡b º  ‰Øèüÿÿÿ¹Í`ºø  ‰Øèüÿÿÿ¹…sº 	  ‰Øèüÿÿÿ¹…sº	  ‰Øèüÿÿÿ¹…sº	  ‰Øèüÿÿÿ¹d   º'  ‰Øèüÿÿÿ¹ÿa é²şÿÿ¸êÿÿÿé.üÿÿ´&    t& èüÿÿÿVS‹P‹ZÔ‹P(ú	˜ „œ  vRú	˜ „N  ú		˜ …Ş  ƒ»ô  ‹p|‡²   1À…ö¹ıÿÿÿºÓ  •À1öÀP‰ØèüÿÿÿX‰ğ[^Ã´&    ú	˜ „Ä   ú	˜ …Œ  ‹@|‰ÆÁş=ÿ€  ~9h€   ¹   ‰ØƒæºÕ  èüÿÿÿVºÕ  ‰Ø1ö¹€ÿÿÿèüÿÿÿX‰ğZ[^Ãt& j ¹   ºÕ  ‰Øèüÿÿÿ¸€ÿÿÿ‰ñ)È¶ÀPë¿¶    º  ‰Øèüÿÿÿ…ö„ì   ‰Ájº  ‰Øáï   Qèüÿÿÿ‰Ø¹   ºÓ  èüÿÿÿ[^1öé'ÿÿÿv ‹@|¹ä   ‰ÂÁú	=ÿ/  ~¹‰   )ÑÉ¶É‰Ø1öºÔ  èüÿÿÿ‰ğ[^Ãt& ‹@|º€ €1ö@Áá‰È÷êÊÁùÁú)ÑºÛ  A0¹Àÿÿÿ¶ÀP‰Øèüÿÿÿ‰ğY[^Ãt& ‹@|º€ €1ö@Áá‰È÷êÊÁùÁú)ÑºÙ  A0¹Àÿÿÿ¶ÀP‰Øèüÿÿÿ‰ğ[[^Ãt& ƒÈjº  ¶È‰ØQèüÿÿÿZYéPşÿÿ¾êÿÿÿéFşÿÿfèüÿÿÿSº  ‰Ãèüÿÿÿº  ‰Áj‰Øáï   Qèüÿÿÿº  ‰Øèüÿÿÿº  ƒÈj¶È‰ØQèüÿÿÿ¹   ºÓ  ‰ØèüÿÿÿƒÄƒ»ô  vy¹p8ºĞ  ‰Øèüÿÿÿ‹“ø  ‰Øèùÿÿº  ‰Øèüÿÿÿº  ‰Áj‰Øáş   Qèüÿÿÿƒ»ô  YXw[Ãv º  ‰Øèüÿÿÿº  ƒÈj¶È‰ØQèüÿÿÿXZ[Ãv ¹ºĞ  ‰Øèüÿÿÿë…´&    ¶    èüÿÿÿV‰ÖS‹˜”   ƒ»ô  v6º  ‰Øèüÿÿÿº  ‰Áj‰Øáï   Qèüÿÿÿ¹   ºÓ  ‰ØèüÿÿÿYXº  ‰Øèüÿÿÿº  ƒÈj¶È‰ØQèüÿÿÿ‰ò‰Øè(øÿÿº  ‰Æ‰Øèüÿÿÿº  ‰Áj‰Øáş   QèüÿÿÿƒÄƒ»ô  v#º  ‰Øèüÿÿÿº  ƒÈj¶È‰ØQèüÿÿÿXZ‰ğ[^ÃfffffffèüÿÿÿUºc  WV‰ÆS¾   ƒìd¡    ‰D$1À‹FÇD$    ˆˆ   D$èüÿÿÿ‰D$‰|$…À…ƒ(  ÇD$   jÿ¹   º   ‰ğh0„  èüÿÿÿ¹ö   º  ‰ğhÿ   hö   èüÿÿÿ¹   º   ‰ğjÿh0„  èüÿÿÿ¹   º   ‰ğèüÿÿÿ‹D$,‹x‹ ‰|$‰D$ƒÄ…À„©   1ÿ´&    ‹$1Û¶8Áá	ùÉ   ‰Ífº   ‰é‰ğèüÿÿÿ¸Æ§  èüÿÿÿº   ‰ğèüÿÿÿ9ÅtH1èöÄ?uƒÃƒû
uÉƒl$…%ÿÿÿhc  ÿt$h'  èüÿÿÿ‹D$ èüÿÿÿƒÄÇD$ûÿÿÿéâ'  fƒÇ;|$…kÿÿÿ‹D$‹ëv 1Éh   É   º   ‰ğh   èüÿÿÿÿt$hc  ÿt$h4'  èüÿÿÿº   ‰ğ‹\$,èüÿÿÿƒÄ%ÿÿÿ º      ‰Á‰ğèüÿÿÿº   ‰ğèüÿÿÿ‰Ââ   ÿú   uÌ‰Â©ÿ?  tv º   ‰ğèüÿÿÿ©ÿ?  uí‰Â‹{‹…Û„u&  ¶€æ?Áà   9Âu:1íë´&    ¶/€ä?Áá	éÉ   9Èuº   ‰ğƒÅèüÿÿÿ9İu×éu&  éÿ'  ‰Â‰Èéÿ'  ffèüÿÿÿUJWzƒçüV)ùS‹°”   1ÀƒÁ`‰ÓÁé‹®è  ÇB    ÇB^    ó«1Àf‰º  ‰ğèüÿÿÿ¨tUå ù  ¿$  uTt& ‰ú‰ğèüÿÿÿ‰ÂƒàÀê·„ `  ¶Ò·”`  f‰„{ø÷ÿÿf‰”{È÷ÿÿƒÇ	Âf	ÿ5  u¼[1À^_]Ãt& ‰ú‰ğèüÿÿÿ‰ÂƒàÀê·„ `  ¶Ò·”`  f‰„{ş÷ÿÿf‰”{Î÷ÿÿƒÇ	Âf	ÿ0  u¼ë®´&    v èüÿÿÿS‹˜”   ‰Øèüÿÿÿ¶‹  º  ‰Øèüÿÿÿ‰Ø¹.   º  èüÿÿÿ1À[Ãt& èüÿÿÿU‰ÕWVSƒì‹°”   d‹    ‰T$1Ò‰à\$‹¾è  ç ù  t& Æ  ƒÀ9Øuö‰ğèüÿÿÿ¹2   º  ‰ğèüÿÿÿ¹   º  ‰ğèüÿÿÿ¶  º  ‰ğèüÿÿÿE…ÿ…®  ƒÅt& 1Ò1ÉƒÀf‰P.f‰Hş9èuíD$ë´&    fú u€ƒÀƒÅ9ØtQ·U0fú „  ‡ß   fƒú„ı   fú u€	·U fú „Ü   ‡   fƒúu¨€ƒÀƒÅ9Øu¯»$  …ÿ„Ş   ¶    ¶Œæûÿÿ‰Ú‰ğƒÃèüÿÿÿû1  uä‰Ú1É‰ğƒÃèüÿÿÿû5  uê¹   º<  ‰ğèüÿÿÿ¹   ºt  ‰ğèüÿÿÿ‹D$d+    …Ü   ƒÄ1À[^_]Ã´&    ffú @…ÿÿÿ€@éÿÿÿt& fú @…*ÿÿÿ·U €fú …'ÿÿÿv €`éÚşÿÿ€éÿÿÿ´&    €éõşÿÿ´&    ¶Œãûÿÿ‰Ú‰ğƒÃèüÿÿÿû5  uä¹   º<  ‰ğèüÿÿÿ¹&   é9ÿÿÿU¶    1ÉƒÀf‰H.1Éf‰Hş9ĞuíÇE^    ƒÅÇE    é:şÿÿèüÿÿÿfèüÿÿÿU‰Á‰ĞWVSƒì‹R‹±”   ¶
„Éu€zÿtÇ@    Ç@    ƒÄ1À[^_]Ãf€zÿuà¶Zˆ\$€ûUt€û‘uÎ¶Z‹¾  ƒã‰|$z‰İ¶Z‰<$ˆ\$‰ë€û„Ì   wJ€û„±   €û…˜   º @  ‰P‹T$1Éƒâ?T$€|$U”Á‹<$‰P‰‰xƒÄ1À[^_]Ã¶    1ö€û	uY‰D$‹$¶\2¶|2	¶›`  ¶¿`  ‰İÁã	ıƒç	é‰õ	ûƒÆÑıˆ(ƒşuËáğ   ‹D$º   „uÿÿÿ´&    Ç@    1Ò1Ééuÿÿÿº   éSÿÿÿ¶    ¶Z‰ÙÀé1Ë‰ÙÀé1Ù‰ËĞë1ÙƒátÃ¶J	‰ÊÀê1Ê‰ÑÀé1Ê‰ÑĞé1Êƒât¦º   é	ÿÿÿffffffèüÿÿÿVº  Ü ¾   S‰Ãèüÿÿÿ‹“„   ‰Ø1Éh   èüÿÿÿ‹ƒ„   „   X¸
   f‰2‹
ƒùtƒèuò‹ƒ„   ¾   ˆ   ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   Ä   ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   È   ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾     ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾     ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   D  ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   H  ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   Ä  ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   È  ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾      ¸
   t& ‰2‹
ƒù tƒèuò‹ƒ„   ¾      ¸
   t& ‰2‹
ƒù tƒèuò‹ƒ„   ¾@   D  ¸
   t& ‰2‹
ƒù@tƒèuò‹ƒ„   ¾@   H  ¸
   t& ‰2‹
ƒù@tƒèuò‹ƒ„   ¾€   „  ¸
   t& ‰2‹
ù€   tƒèuï‹ƒ„   ¾€   ˆ  ¸
   ´&    f‰2‹
ù€   tƒèuï‹ƒ„   ¾     ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾     ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   D  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   H  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   „  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   ˆ  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   Ä  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   È  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   D  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   H  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾    „  ¸
   ´&    f‰2‹
ù    tƒèuï‹ƒ„   ¾    ˆ  ¸
   ´&    f‰2‹
ù    tƒèuï‹ƒ„   ¾ @  Ä  ¸
   ´&    f‰2‹
ù @  tƒèuï‹ƒ„   ¾ @  È  ¸
   ´&    f‰2‹
ù @  tƒèuï‹ƒ„   ¾ €    ¸
   ´&    f‰2‹
ù €  tƒèuï‹ƒ„   ¾ €    ¸
   ´&    f‰2‹
ù €  tƒèuï‹ƒ„   ¾   „  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   ˆ  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   Ä  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   È  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾     ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾     ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   D  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   H  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾@Ü €   ¸
   ´&    f‰2‹
ù@Ü tƒèuï‹ƒ„   ¾€Ü À   ¸
   ´&    f‰2‹
ù€Ü tƒèuï‹ƒ„   ¾ÀÜ    ¸
   ´&    f‰2‹
ùÀÜ tƒèuï‹ƒ„   ¾ Ü @  ¸
   ´&    f‰2‹
ù Ü tƒèuï‹ƒ„   ¾@Ü À  ¸
   ´&    f‰2‹
ù@Ü tƒèuï‹ƒ„   ¾€Ü    ¸
   ´&    f‰2‹
ù€Ü tƒèuï‹ƒ„   ¾ÀÜ @  ¸
   ´&    f‰2‹
ùÀÜ tƒèuï‹ƒ„   ¾ 	Ü €  ¸
   ´&    f‰2‹
ù 	Ü tƒèuï‹ƒ„   ¾@	Ü    ¸
   ´&    f‰2‹
ù@	Ü tƒèuï‹ƒ„   ¾€	Ü @  ¸
   ´&    f‰2‹
ù€	Ü tƒèuï‹ƒ„   ¾À	Ü €  ¸
   ´&    f‰2‹
ùÀ	Ü tƒèuï‹ƒ„   ¾ 
Ü À  ¸
   ´&    f‰2‹
ù 
Ü tƒèuï‹ƒ„   ¾@
Ü @  ¸
   ´&    f‰2‹
ù@
Ü tƒèuï‹ƒ„   ¾€
Ü €  ¸
   ´&    f‰2‹
ù€
Ü tƒèuï‹ƒ„   ¾À
Ü À  ¸
   ´&    f‰2‹
ùÀ
Ü tƒèuï‹ƒ„   ¾ Ü    ¸
   ´&    f‰2‹
ù Ü tƒèuï‹ƒ„   ¾@Ü €  ¸
   ´&    f‰2‹
ù@Ü tƒèuï‹ƒ„   ¾€Ü À  ¸
   ´&    f‰2‹
ù€Ü tƒèuï‹ƒ„   ¾ÀÜ    ¸
   ´&    f‰2‹
ùÀÜ tƒèuï‹ƒ„   ¾ Ü @  ¸
   ´&    f‰2‹
ù Ü tƒèuï‹“„   ¸
   ¾` Ü ´&    ‰2‹
ù` Ü tƒèuï‹“„   ¸
   »   Â`  ‰‹
ƒùtƒèuò[^ÃfffffffèüÿÿÿWVS‹ô  ‹ZD…Ûty‹³8  ö    …«(  ‹ƒ4  ¿      èüÿÿÿ‹“4  ƒª   u,ö    …Ï(  Æ(  ‰ğèüÿÿÿ1Ò‰Øèüÿÿÿ‰Ç‰ğèüÿÿÿ‹ƒ4    èüÿÿÿ‰ø[^_Ã¿êÿÿÿëó´&    t& èüÿÿÿUWVSƒì‹°ô  ‹^D‰t$…Û„j  ‹»8  ö    …ä(  ·(  ‰ğèüÿÿÿ‰øèüÿÿÿ‰Å‰$‰ğèüÿÿÿ…í…)  ‹Gh‹ ƒø–   …À‰—   ‹D$‹@…À„
  ‹ƒ4    èüÿÿÿ‹“4  ‹‚   H‰Š   …Àu1ö    …)  ‰ğèüÿÿÿğ€‹T  ‰Øèüÿÿÿ‰$…Àx|‰ğèüÿÿÿ‹ƒ4    èüÿÿÿ‹$ƒÄ[^_]Ã´&    v ƒø	…iÿÿÿ‹‡X&  ‹ˆ  Õ ‹‡X&  É  O   Õ ¸
   t& ‰
‹*9é„6ÿÿÿƒèuïé,ÿÿÿ´&    v ö    …2)  ‹ƒ4  ƒ¨   ‹ƒ4  ‹€   …À…Vÿÿÿğ€£T  ïéIÿÿÿÇ$êÿÿÿéTÿÿÿ´&    fèüÿÿÿUºv  W‰ÇVSƒìd¡    ‰D$‹‡$  ‹€ØıÿÿÇ$    ‹˜8  ‹Cˆˆ   ‰àèüÿÿÿ‰Å…À…)  ‹$‹Hşƒù>‡G)  ¨…G)  ‹·È   1Ûë=t& ¸rI  f‰D$…öt¹   T$‰øèüÿÿÿ‹·È   ‹$ƒÃ‰Ğ9v*´&    ‹J¶ˆD$<VtqwO<Tt³<UuKƒÃ‰Ğ9wŞº]  f‰T$…öt¹   T$‰øèüÿÿÿ‹$èüÿÿÿ‹D$d+    uBƒÄ‰è[^_]Ã<Wt‹¶DˆD$…ö„zÿÿÿ¹   é\ÿÿÿ¹1Àÿÿf‰L$…ö…DÿÿÿéXÿÿÿèüÿÿÿt& èüÿÿÿUW‰ÇVSƒì0‹Ÿ4  d¡    ‰D$,‹‡8  ‰D$…Û„%  ‰;«(  ¹    º  Çƒü      hÀ  ‹@‰D$ˆ   P‰èèüÿÿÿY‰ÆX…öˆ  ‰»Ô  ƒ  ÇƒØ     ÇƒÜ     Çƒà  ğä  Çƒä  Pä  Çƒ     ‰$èüÿÿÿ‰Æ…Àˆ«  ‹$SÇC   ÇC    ‰Ñ‰C(‰ÈÇC,   ‰T$‰êèüÿÿÿ‰Æ…Àˆl  S‹‹¸  ‹$‰T$èüÿÿÿ‰Æ…ÀˆD  K‹$‰Î‰L$‹‹¸  ‰òèüÿÿÿ‰Æ…Àˆ  ‹‹Ä  ‹T$‹$èüÿÿÿ‰Æ…Àˆá  ‹‡4  ‰D$‹‡8  ‰D$‹@h‹ ƒø„H    ƒø„I  ƒø	…€   ¸à  èüÿÿÿ‰Á…À„Å  ‹D$°œR  ¸|  ‰òèüÿÿÿ…À„¬  ‹L$‰ˆ  ¸  èüÿÿÿ‰D$…À„¾  ‹D$‰ñº`   ‹€ˆ  h  ‹t$ èüÿÿÿZ…Àu
¸  èüÿÿÿ‹T$‹‚ˆ  …À„d  Ç€      ‰Ğ‹’ˆ  (  èüÿÿÿ‰Æ…ÀˆÈ   ‹D$º
   ‹ˆX&  Á$Ç v ¸€ € ‰‹ƒøÿt%€ € =€   tƒêuá‹$“Œ  ‰èèüÿÿÿ‹l$ƒÅUhÀ*  èüÿÿÿk‡t  d¹d   1ÒÁè
÷ñ‹‡t  Áè
RP‹‡4  ÿ·p  ÿ·<  ÿ°(  Uhà*  èüÿÿÿƒ  ¹¤   º•  ƒÄ$èüÿÿÿÇƒü     ëm´&    v ‹D$‹€ˆ  ‹°   …Òtèüÿÿÿ‹“È  ‹$èüÿÿÿ‹‹¼  ‹T$‹$èüÿÿÿ‹‹¼  ‹T$‹$èüÿÿÿ‹D$èüÿÿÿ‹$èüÿÿÿ‰èèüÿÿÿ‹D$,d+    …l  ƒÄ0‰ğ[^_]Ãfƒø§   ƒø…nşÿÿ¸t  èüÿÿÿ‰Á…À„Ú  ‹D$°U  ¸ä  ‰òèüÿÿÿ…À„d  ‹L$‰ˆ  ¸Q  ‰t$ ÆD$$aÇD$(    èüÿÿÿ‰Á…À„=  ‹D$T$ ‹€ˆ  èüÿÿÿ…À„A  ‹ˆØ  …É„äıÿÿº|  èüÿÿÿéÕıÿÿt& …ÀˆÈıÿÿ¸”  èüÿÿÿ‰Á…À„±  ‹D$°œR  ¸ˆ  ‰òèüÿÿÿ…À„v  ‹L$‰ˆ  ¸¹  èüÿÿÿ‰D$…À„v  ‹D$‰ò¹   ‹t$‹€ˆ  èüÿÿÿ…À…Tıÿÿ¸¹  èüÿÿÿéEıÿÿt& ¸,  èüÿÿÿ‰Á…À„Ş  ‹D$°U  ¸´  ‰òèüÿÿÿ…À„¦  ‹L$‰ˆ  ¸Q  ‰t$ ÆD$$aÇD$(    èüÿÿÿ‰Á…À„?  ‹D$T$ ‹€ˆ  èüÿÿÿ…À„  ‹ˆØ  …É„¶üÿÿº`  èüÿÿÿé§üÿÿhï  jèüÿÿÿ¸à  èüÿÿÿ^‰ÁX…É…üÿÿéĞ)  h  jèüÿÿÿ¸t  èüÿÿÿ^‰ÁX…É…şÿÿéî)  ´&    f¸,  èüÿÿÿ‰Á…À„è  ‹D$°U  ¸ø  ‰òèüÿÿÿ…À„­  ‹L$‰ˆ  ¸Q  ‰t$ ÆD$$aÇD$(    èüÿÿÿ‰Á…À„Ç  ‹D$T$ ‹€ˆ  èüÿÿÿ…Àt'‹ˆØ  …É„Êûÿÿº˜  èüÿÿÿé»ûÿÿ¾êÿÿÿéıÿÿ¸Q  èüÿÿÿé¢ûÿÿ¸à  èüÿÿÿ‹D$Ç€ˆ      ‹D$ƒÎÿƒÀPh+  èüÿÿÿXZé‡üÿÿ¶    ¸,  èüÿÿÿ‹D$Ç€ˆ      ëÄh;  jèüÿÿÿ¸,  èüÿÿÿ‰ÁXZ…É… şÿÿé,*  ´&    ¸”  èüÿÿÿ‹D$Ç€ˆ      éyÿÿÿh£  jèüÿÿÿ¸”  èüÿÿÿ‰ÁXZ…É…-ıÿÿéş)  t& ¸t  èüÿÿÿ‹D$Ç€ˆ      ¸,  èüÿÿÿ‰Á…À„e  ‹D$°U  ¸Ì  ‰òèüÿÿÿ…À…Rüÿÿ¸,  èüÿÿÿ‹D$Ç€ˆ      éçşÿÿ¸,  èüÿÿÿ‹D$Ç€ˆ      éÊşÿÿh;  jèüÿÿÿ¸,  èüÿÿÿ^‰ÁX…É…öıÿÿéZ#)´K\´£èVåÚí%Ö§«Ì]ºùò—Øq«‚ò4ÄÔgV'5>DŞÖ5R§tIOÊÙ»•QU|1_üy•/èŞa¾ ÿèDl¨czÿIE?JãP‰\„ü/ñvˆhIhcø2öWÇ¼ÁÅÊD†mFÄS	YÈ®Ø
$½Á˜SÃnmUøÊrWQ°óNPHÎ)`Ñ!ÍÆMr L=\–5Á<¡H?T¡KêmÑ¨¨SKøx»çğ²ã"è½ŸÁ">zjËó¯v”³îä:â• ;¿!&™MD¸
Ÿm°dÙšİ½K›¿Æce]‹8…B=Eï8MÀ2m™ÙSMDê÷ÌZP(÷#ˆC¡ûÊ"ÁN|şô$q®L!b¹ƒl‘1?N³DCÈ*9…‹k3W k7aÿ~"ÆQ&x˜Âş ’N¬k!"w¦*90Ã¸;Ë›jå|Ô2²´ëŸõoü[÷0ÉRÁñSà1ĞÓÊ÷ã+ä	‘¶ÛÙV‰2ÊO/ÁiÈv@•Mü}un:Ø¥„ÕcœæÍ}!û{şxRŸı=©KRm@]à²»€m7<ãÌ›èÏl#ùyKƒÌw0?µ>Céë#S•áºæ`f ÷M÷a™’î¥”À…õO(ùKZú›*oQã‡Àí}_k:3_‡Î¼™öI>
ØãWÂ(‹øğºAÏ?äè²,›ñH$€û.ª4·0İ¨­-n$9›±ióy½Œ,¤okËÑ]–röá6r<L’_Ê5f0r"Î2§ÉØK–ÚT¤#WîEkÍhÔ:'ÎVÊ´ªÇW>¥YƒÙÛâï—Ú=ˆì³/ ÓíÖÍ}ş-¯#[0Åé7S#hJã½T&¡³Ù¡ğ^q '‰ÊÜMs(™^HF%XùSï *{¢¯EüÊmï™?J™[ïÍh½Rg§ÌCĞ¿#½ù{jrƒşÒíÚ í[˜¤İ*¨$,…ş¿¬,óÊ®âØWŸ7¨V’mÿ~¸V»òXşu´¤6}ïBotåÁ±ÔÙ‰õİ)şÈÅÈßò°uÀ•ÿ÷¢¸nm’Í6b¸ùhö$P˜·Âÿ¾­—1¤Bq2
Lê” ¢(‡5CêL“§6µRmÁRIvñ¡¤ğŠøğöÅöÛººZpËLæÆÚ“ÃxD	´¿8œòLF6ãe¥E©¼Ğ˜Ò ¦°2¡?É}9Ê‰Uº*­yÌ'ÑB•óeS¢Æa¹y¹ğ¦”¢Ø&•õÁL\£èĞöóÆËı!‚,‚ ãü âşh™ÛVŠ}C#°ÿjáñpN(IQæj%ÍNİÔ' !ƒ½ï¼Ö]Sp^KM„Æ×aŞ»ùæù©ªˆªÖ†Û:§ÚÌñ½8÷]Šh×:·§uúrWñh£~t¤È‡W–'Ï¦2Æ¹ë¾˜U´d	İ•a¼Úui”yDğÛğTã,éè¦}7®G]B0§È#Ms"vlç¢•íFÎæÜQÑfLÜVrFû7‰´Ïæ759ÂZÚ“6\ıPäYÔK¢*oĞì2Ac"´$PäMøñ•aŠj†qì7‡±QqkÎ)š¯D;­=¢*Ì!Xj±Ü5PØ&¥HB‘µ5}w‹[j¯ÍÁ¶œ×ÏŞîËTN¤äpŸ›"Ê;¼U& „}Ê…™CŠ«V6pG"gfûq‚¯Wc1Ÿ¢ÒÆ–=ğ²Ç¸†Ì,DóÄşi8èCİA§2“÷Õ@†ŒÍ7çxÖoË†ûHâ}UĞIÿú1§ã5vİ¾UĞiß*òÁöüìc9f]›í‘aèŞÑë-ŞƒLBº¡µK´D@.Ë	ˆ}WÁuXøÎ^ê™/¦ŠZ_]öĞ¤zd¸‰8òjôˆ¼S¿PJ†å”æs³[Ã5~÷¡àµæa=XÃfu÷re*(„ïJğ à¹vn§¿ùcÉ™_,Ğb¢3×6_Oym°Gx®jàu›ÃÔ‹9ÁÌ’’›øÂú1w'“€yø©†j }w¨İv‡«	 _™_Qt7µ‚PV 9}4”–6·ÖèV­bp*ãÖU¡ZEëÎªvn-æÔ 6XœºĞa,®$(á_Ç}¯ãjŒP~(ìöP ‹±	¡ğ³ps‰áœäkÂ²V®ûÄZšº9÷â1&ƒÍ‘m9rÃ&ğ72õ"ò×Ö³ë£®Y:§q@©”Kù2‘kN©Õn²¬Ü)ß¼-sr¯Ç²¼Z$‡µ±­ÿt	¡9tEÕúP\Ö5o”¡/,_1{+aåÇ†®¢j AÑñoÙd½B8Í²W;ÙùtòQ}}ì‡ƒ
ÙDùJl¾‚€¢”§£“M{Éƒù!²ËÃñé@³b¡ğıP„:ÜkÍáê¢¨¬”ë{#‡ZÓ–ßCo ~½öJòÒÉëUà2S%b"1-ş®å û6ĞDK şÀs	Åá,õà½O@vò­Ô<.äC>Û¹jtÁÙC&–._û›}Z8	rSÀüDó£;½z)äp”°1º:uáuèqb<ïg*hi÷ù:¤ì‚æ!g)#—b<ÔL\Q=?ãÕ^^K©K¬÷3xß¬°é{Uë°«n‹:	á™…Ş%°š?\ä_9‹`e×ñQu;Ô2FŒÿ²Lïˆsí(—Õ]î@€ceŞŠ‘ŒFıôW?øé•K&Í¥•f0=‘9Ù*îøŸ	Ñ,ê²E`ß—©šçI4£‹XÓ1;aˆÔ-ŸçSé”öñ¬„\²lCqÜt&Äùa0Âz´×ü½˜c_oÄdéw)o ³–ç™·hºÊî¥âÀä¤„ˆ€¨ÕiûŠ*¤™+™WìÄöOK`ò€óî³›‚ ’EùìV9QI\–]tL›Ê¹›Ù®–G›Xßœ]’¿(DÿJ‚“/“E›]&©‰x*;ÌKZ^ûø[ïgp£Ia1Fÿú@on}ßØ’ï.fxAPş¾Çÿ³«1‡a&è¡Š*fŸÊÂáº
âf)¶8>n™Ïìo	Z4\ÈÿZHX]0õİıp‘“{_CD–[!²Y‚m8ˆÖWœ4Í…÷Şù˜òm©¢xåñS,kªğ29ÕX<F$]©Ï{ƒ>yÖ{‡ówÌ‘m’İ†“Ğ‡Áëƒv¨a5Ä½Yyz7Æ€¾à€ `ÜõÂ[X
£&±Í›é½ÿ!¨É±• œÊD«}oG¨Ü’ Äjš³RÆEH/Õ®_÷ÍáêwB¾Op*ê¤2t.%Ûnº…Å©NkŞ•\!‚íû¨,™S +Ÿûü½ã‚?¸C´Øy5Sí›÷!–*cF#3WæÙ„àúLLÅ«* xJµ{És ”’%FÎ×nDˆõİ2Ó¤=².ÀD/§3yï›Ä‡2³¥ÃÜŠ1ÏƒzâŒ.ğºgÿV˜"şú9 BcMpŸ|Ú»%¹ÒV0ä¬cúJ:Iã®"H<œvr<®-ÙĞ5ßú% Lq“ôQ9ËnmŒz®ú©á¹¾³j“,Ñ
 9´;NÚ-Ë¬ÓB\(Zõ’ã¼Ä¬ŠõR¸Œ½rH(zºÊ˜T–=PŒ!ifõC])Rª|îOG³²0SuªÄ9@%ßÂ¡õx¾%{à¨!PÒŸìUøÙÔÛyi$Ó{çC«’÷WÛd§¸7ø.†
V"ßÄCØÑÜ»¥¾óØQÃ	Í¬Ä Ã³:¶!‚èõXdÈKøÍiïOvİ½°ñ<-€PA‚?Éc¬_y.ÉfDêr˜˜iêJ“©§zhJUÌ2ŠÍ<üöz¤•Iye`ÒGneôy2íìÙpğ¯İ÷6³} –(™Ö¬lõ}ZLUb`¯hdST )7¼:Êpb‹°s2¿#•~§x“ò¤Ã»k}Àşù]’ÁİÔ‚ö‰Ô>±wZõB·e1jìº|ÙFZ®NöMM…[“|¶ÒåüşÙŞ¤BM§&/ùºYPÇ´QÕ§m1^u
Ç/Îò
iæèï*Ì³3~ß45ûÖ4Aâ{/ 
JAŠ! ¿b>‘9¹WÅ*­Ó8-¶JŠ²ŞùÁHòeüp)û•„7Îé<»­²5o·çpÌp22²»2AªhxãËÉYº2§$ªòúø8ä,
Y¹ÁS€t¯Áéß!™T¢²»ÌH.»î¦gÔ€ë…ªæY	÷~Ûğ(¯ƒÑm#ä¾P¶´#ıÃo¹_K]ÕæÒ½9çdˆ˜!Ia§¢x‚ ¬&Œ›ğ1ò@ra0ÆdŠ·Kïv¦ÚùfCà’7Ò°l+BxİûªÆ¤~*Oêxñ¦j;è¤HI×¡N%”÷A]è'^ãÄrH bdî%ØNXè×)*©ù8Õ‚ EûÛÑwå¨ùøÙÄéÌ‘}ÔG{ùÓ‘µ°¹„ †k©ÑÇìıÃö£`)X_à;Cn\-uº!Sëb(öA•H¤ÅŠU3ñtt½r,açh ÍúšóÎ´ÖwğïÒÃÁC&ĞŠZkà{k]Uç	›$A–>)9¾"uŸè(ìÁ'­‹Òì"É3Àzlğ\L/6¿ª/8¦Î„”|²Ù
÷½¦+Ò®xöx8¼E‰f[/?dwF\,ú8˜€?”ö áôSå²á5w±ômu|•?ºfvÏG¢fêPÅ¦:–”‡w¶¶MëMìb>]­ÂÁèVÀæó«İ üpÄÕZÖØ
²>î-©pqî“Ä²dîí¥ëyÆ¸?¯)‘˜QÃé
Ÿ•¯›N00ÓşŠÓ{3G^8uzhÅ‚ñÅÓ+PìØç6^O•—‘&¯˜ËNğ%²ÕGR–\w$ĞäŸb? …'ïTµäQWäŞ!µ\[†u#tNSÄU¦Ñ1kÜú!Ñ	ª¾À×²·€i>Û±aÏ¬óóLëÈÌ{3´øøVî¢;aåáVÊŸö=~Ğ„Pˆ\ãwÓqª}P³9ûi©sF·O³q—µûIÒx	¸š*—mç­õ(çÊ ÇaÊ…­Ín§sû«;îSˆpGç©y·¨óáX„­Æìë?'*-CY­#’óvÎÊ$kõ0oŸ8ü„›×»îaƒA§Âë+Ô3õtÒ#	/ø¡–£ £ÎD.[Ê‹jUïŠğ}¹:?s¶Ÿ]Ş|¼y¦¦üN9ZGıwhï~é¿ù~êÚ´e|IïC„zµÅ±öÊu}®¾‹ì_Ö•qÂÇUòîwÈ}¸©Š:ô{ÂÄà>PĞONMŒ<ŸöYFğô4{™P	ÖI×ªÒx…Àt Pÿt$h	  èüÿÿÿº   ‰ØèüÿÿÿƒÄë¼ÿslÿt$hâ  èüÿÿÿƒ´Y  Çƒ°Y  àÿÿÿ‹    ‰ƒ´Y  ‹°Y  ‰ƒ¸Y  ¸    Çƒ¼Y      èüÿÿÿƒÄé9  ÿt$h€  èüÿÿÿY_éùúÿÿ‹Fd‹@…Àt‹H(…ÉtT$‰ğèüÿÿÿ‹vLƒîLéÊşÿÿÿt$hè  èüÿÿÿY_é[úÿÿ‹D$@‰ƒ\  éYúÿÿ1Àƒ|$4 •À‰ƒ`  éQúÿÿö†   @t‹Fd‹ …Àt‹H…Ét	1Ò‰ğèüÿÿÿ‹vLƒîLéúÿÿ„Òt-ö    tÿt$h<  èüÿÿÿ‹ƒ¤Y  Y_‹“ Y  ƒÊé/úÿÿöÆ°t-ö    tÿt$h`  èüÿÿÿX‹ƒ¤Y  Z‹“ Y  €Îéıùÿÿâ  ÿ t0ö    tÿt$h„  èüÿÿÿ_X‹ƒ¤Y  ‹“ Y  Ê  @ éÅùÿÿÿt$h¨  èüÿÿÿ‹“ Y  _‹ƒ¤Y  Y€Îé¡ùÿÿÿt$h  èüÿÿÿXZé…ùÿÿ¸	   èüÿÿÿéòøÿÿÿt$h¬  èüÿÿÿXZétùÿÿCPhş  èüÿÿÿ‰ØèüÿÿÿZY…ÀtCğ€‹6&  @¸úÿÿÿé  CPhş  èüÿÿÿ‰ØèüÿÿÿZY…À„Á  ğ€‹6&  @¸úÿÿÿé  é  ¸4  èüÿÿÿ¸,  èüÿÿÿ¸4  èüÿÿÿ¸,  èüÿÿÿ¸¼  èüÿÿÿ¸´  èüÿÿÿ¸´  èüÿÿÿ@Phy  èüÿÿÿXZéÇ  ¸   èüÿÿÿ@Ph—  èüÿÿÿXZéò  @Phè  èüÿÿÿXZé’  ÿ4$h  ‹D$ƒÀPh¬  èüÿÿÿ‹D$@‰D$‹ ‰D$ƒÄé˜%  ‹D$¿ûÿÿÿƒÀPhŒ  èüÿÿÿXZél'  h  ‹D$¿ûÿÿÿƒÀPh  èüÿÿÿƒÄél'  ‹|$h3  _ShÔ  èüÿÿÿShø  èüÿÿÿ‰ø1Ò¿ôÿÿÿèüÿÿÿƒÄél'  W‹D$(¶T$.ÁàÁâ	Ğ¶T$/	ĞPh3  ‹D$ƒÀPh`  èüÿÿÿ‹D$DƒÄ‹8éV'  V‹|$GPh  èüÿÿÿ‹D$<èüÿÿÿ‰ø1Ò¿ûÿÿÿèüÿÿÿƒÄél'  ÿt$‹|$GPh  èüÿÿÿ‹D$<èüÿÿÿ‰ø1Ò¿ûÿÿÿèüÿÿÿƒÄél'  ‹\$h  ¿ôÿÿÿƒÃShD  èüÿÿÿShh  èüÿÿÿƒÄél'  ‹|$‹$DÿPWÿt$4h<  èüÿÿÿƒÄé¡&  CPhÜ  èüÿÿÿXZéO)  CPhD  èüÿÿÿXZét/  ‰L$PRÿ³<  ÿt$hl  èüÿÿÿ‹E‹U L$,‰B‰‹D$0‰|$0‰M ‰E‰8ğÿ‹œ  ƒÄ‹L$é¼2  ÿ°<  ‹€8  ƒÀPh¸  èüÿÿÿƒÄéÓ7  ÿµ<  ‹D$ƒÀPh  èüÿÿÿ‰èèüÿÿÿ¸ôÿÿÿƒÄéÌ:  ‹D$XSh4  èüÿÿÿhj  She  èüÿÿÿ¸ôÿÿÿƒÄéÌ:  ‹½t  ‰Ë¾d   1Ò¯ßkÃdÁë
Áè
÷öRSWQÿµ<  ‹|$(_Shì  èüÿÿÿ‹€   ‹…p  ƒÄ”–  Áâú   zÿÿÿ‰L  …À„À:  éÔ8  FPhÀ  èüÿÿÿ‹ƒ@  ZYé@  FPhš  èüÿÿÿXZé¹?  FPhd  èüÿÿÿZYéóH  ÿ4$ÿt$jPRÿt$$‹D$ƒÀPh(  èüÿÿÿƒÄ évG  Fÿ³<  Ph   èüÿÿÿƒÄégA  Rÿt$hÂ  èüÿÿÿƒÄé~M  Fÿ¶È  ¿ôÿÿÿPh  èüÿÿÿƒÄéæM  ‰D$ÿt$hì  èüÿÿÿY_‹T$‹D$…ÀEĞ‰T$é¬O  ÿ³<  Qÿt$h@  èüÿÿÿƒÄé¬O  ‹$¿   ‹¬°|  kÅd™÷ÿ¿d   ™÷ÿ‰è¿   R™÷ÿP‹D$ÿ´°`  ÿ³<  Qÿt$h  èüÿÿÿƒÄé¬O  ‰T$Uÿ³<  ÿt$hÄ  èüÿÿÿ‹T$Çƒ¬      ƒÄéQÿÿÿ‹$‹”°`  ‹ƒ<  …Òt#‹<$ÿ´·|  RPQÿt$hd  èüÿÿÿƒÄé¬O  PQÿt$h@  èüÿÿÿƒÄé¬O  ƒÆVhQ  èüÿÿÿ1ÀY[éàR  ƒÆVh˜  èüÿÿÿ^X¸êÿÿÿéàR  ƒÆPVhä  èüÿÿÿ¸ğÿÿÿƒÄéàR  ƒÆÿ³<  Vh  èüÿÿÿƒÄéÍS  ÿ4$PFPh4  èüÿÿÿƒÄéèT  ƒÆVht  èüÿÿÿXZéU  i$ğ  ÿ´Ø  CPhL  èüÿÿÿƒÄéÑV  CPhØ  èüÿÿÿXZéæU  CPhü  èüÿÿÿ_XéuW  CPh(  èüÿÿÿZYéğU  CPh¬  èüÿÿÿY_é¸V  iD$Hğ  ‹\$8ÿ´Ø  GPhn  èüÿÿÿƒÄéù_  iD$Hğ  ‹\$8‹„Ô  ÿt$LiT$Lğ  ‹\$<ƒÀÿ´Ø  ÿt$8Ph   èüÿÿÿƒÄé!`  iD$Hğ  ‹\$8ÿ´Ø  GPhƒ  èüÿÿÿ¡    ƒÄéQ`  ‹D$»òÿÿÿÿ°Ø  W‹D$0ƒÀPhè  èüÿÿÿƒÄÇ$òÿÿÿé#]  iD$Hğ  ‹|$8ÿ´Ø  Gÿt$4Phˆ  èüÿÿÿƒÄéèW  ÿ²Ø  ‹D$HƒÀPh°  èüÿÿÿ¡    ƒÄéeX  G‰L$Ph|  èüÿÿÿXZ‹L$éàa  G‰L$Ph½  èüÿÿÿY]‹L$é	b  ƒÇÿt$¾   ÿµØ  WhH  èüÿÿÿƒÄéDb  CPh¨  èüÿÿÿXZéÃc  WƒÃÿ³œ  ShH  èüÿÿÿƒÄéd  G‰L$Phü  èüÿÿÿYX‹L$é^e  G‰L$Ph0  èüÿÿÿXZ‹L$éÛe  G‰L$PhÔ  èüÿÿÿXZ‹L$éşd  ÿ´Ø  G‰L$PhØ  èüÿÿÿ‹L$ƒÄéßd  CPh\  èüÿÿÿYXéf  ƒÃShõ  èüÿÿÿXZéf  CPhŒ  èüÿÿÿY^é_f  ƒÃSh  èüÿÿÿXZélf  i$ğ  ÿ´Ø  FPhØ  èüÿÿÿƒÄéæf  Gÿ¶<  Ph  èüÿÿÿƒÄéqh  ‹…  …Àt<ƒÇPWh¼  èüÿÿÿ‹D$èüÿÿÿƒÄÇ$úÿÿÿéĞh  ƒÇWhà  èüÿÿÿXZéÇh  ‹…è   ë¼ƒÁQh  èüÿÿÿYXéán  ƒÁQhL  èüÿÿÿYXéán  ƒÁPQh3  èüÿÿÿ¸êÿÿÿƒÄéËn  ƒÁQh(  èüÿÿÿXZéÅn  ƒÁQhp  èüÿÿÿXZéán  CVQPh”  èüÿÿÿ‹ShƒÄé–o  ƒÃShQ  èüÿÿÿX1ÀZép  GÿvPhÀ  èüÿÿÿƒÄéTp  Fÿ¶¤Y  ÿ¶ Y  Phì  èüÿÿÿƒÄéq  èüÿÿÿUWVSì„   ‹ZXd¡    ‰„$€   1À{ÿslhx  Wh~  èüÿÿÿƒÄöC|tT$4‰Øèüÿÿÿ‹Ckp´FL9èt‹Fd‹ …Àt‹…Òt‰ğèüÿÿÿ‹vLƒîLëÛ·“œY  L$4‰Ø³X  «Ü)  èüÿÿÿ·“˜Y  ‰Ø‰áèüÿÿÿD$8PWh›  èüÿÿÿD$PWh²  èüÿÿÿ‰ğèüÿÿÿÿ³ X  ÿ³üW  Wh  èüÿÿÿ‰ğ³L  ƒÄ(èüÿÿÿ‹ƒ4&  ºu  ¨ ¸o  DÂPWhÉ  èüÿÿÿƒ$  ‰úèüÿÿÿÿ³4&  WhÚ  èüÿÿÿƒÄƒ¾üşÿÿ t4‹NÀ…Ét-‹Q‹VÄ¯ÑÁê
R¯FÈ1ÒkÀd÷ñPÿv¤ÿvŒWhH  èüÿÿÿƒÄÆğ  9õu¹ÿ³”1  ÿ³1  ÿ³Œ1  ÿ³ˆ1  Wh  èüÿÿÿƒÄ‹„$€   d+    tèüÿÿÿÄ„   1À[^_]ÃGPhô  èüÿÿÿX¸³ÿÿÿZé$s  GPh¸  èüÿÿÿ¸³ÿÿÿ^_é$s  G‰$Ph(  èüÿÿÿY[‹$és  G‰$Php  èüÿÿÿY[‹$é9s  PGPh3  èüÿÿÿ¸êÿÿÿƒÄé$s  G‰$Ph  èüÿÿÿ_X‹$é¬s  G‰$PhL  èüÿÿÿXZ‹$éÌs  ¸ˆ  èüÿÿÿ¸€  èüÿÿÿîô  Vh0  èüÿÿÿX1ÀZé~ƒ  Ç†,%      îô  Vh|  èüÿÿÿ¸ôÿÿÿY[é~ƒ  †şÿÿPh¼  èüÿÿÿXZécƒ  kÃÿ°¨  ÿt$$‹D$ƒÀPh   èüÿÿÿƒÄé]‡  P‹D$ƒÀPh@øŒk„c.ZŞ¶Dp0 :€GÂØú"Eá"•?:­ˆÛSB1Î‰¿ƒşÍhJõÊRÄËƒìÇjÇÄ"r†Ä	Aä¦İÔéhSoÇÍ?dJ)ˆX¢£ØQ6î`d¦t+‚„;F‹QòPª›ô^úw³Æ¸‹ÖÏí>UW=¶ØWÓ}‰IéïöpWŸç\mÜûÑµP¸]Õ4VÂPÛûÎöS¬øÛLÏqÄBG¥co<üçªyY|Ê†–¸ËùœLíaŠ#ø_Nª÷k%æ¨E„­SÖ‰#„ø%­,¢tceŸ`ªê&	ñé®ÔP>³òV7%Í2±È÷¦±ç<Æf¸*P³¼ŸfŠ”Ÿ}ô¦¿–õôÚ
0Åß‘vøõ·`5¦&´*­=ßª‹Ø[ß/†‡%|x]ç8ÑmÜwhu¼ğË¦júMŸôhê÷îCûÿ+¹I¬[v.,=&ñİ‘v{†êu€);ÉqrÒnT°Í<Ûµü„NzÒÉ!¢Âÿ’³±ëŠL¸Ìªt û!›m|ì–îµ!g~säWÓ¨5 ¾›Ï'yV¿-b÷¾¿Êú‚%…towŒğŸƒºxš÷äğ®-†2: g¶îLÇ{5[Q‚Ãì56Dÿ\O¶{ùß5âx*t¼êŸ¾ñµ: $‡‰ŞeFô,TYnú`ìÏ •S…9Ur`'†Vù!¡§Á77x¦Ùæß*€¼ƒğ?EÖzBŞ1ÿæ;du$JF%Cd¿ùÑˆ‹’ÁÍæà+Gì[‚1oê¼•ÏcÀÙìÏO“c¾{%3vjö·~	¹é„éİ‘Át‡"L¾*D_W+®üîİÒ§i<déuXÖöîªô³ñ
ÙàZ®!uhŠ=åÁê•wİ
©0$w
”Ê–3!f7ãæú;ûMŸ';©Èß„uié¶k'‹W©^àdMõSyìeî½kÃŒ¾h›¯£c²Ù<êËyï,0Å[%)ÃåÖì³HHe¼¤2ñ~1?CÉvÛ‰d¿VÁMùÀ°¸÷M„)ÖM)(Eı…3İ–èódÍyHæW˜|^˜Jqš¦×ÈŞg¦qLÓ÷†Y´É .î]Ğ>ºØ†¾æ0ßfÏ 9cV˜¬7Ÿ…)áÁ4ÁdÑ
S£ï•n(w|Ğ|)|íäã’h—™ší}¨gÄ…´¦(˜‡t–Êzv46t î“€crFKùß†¿»¿À+ÖpXŸ$Ê‹VAÏ÷ûì8.µ×L2œ-í›«—•”;ZAÄÇ¢€IGJ[„¶/Ïe¹"ò# üOs¡¹,fœ)~Ö†ÁÅÖk¸^b#ëX³øÒÁæû§ŸXw
+pGêÖœg%ÊÎ™ëûÅJbÓ]Gn6È*M³fnjÛB­
Ÿ&jõ_£tË¯ã`˜PGu8ê *@L¾1\nÇ×qVZÃù!m_7‰õ‹äÁ+×P‰9>ÚsŒ„ÀµmTàMš\âÜxû)×YÙZ²…òÕ€"nŠ¿êõOËôX®q$TÚmÕÇŒßi—ØMh7‘ß„5Zºx$à|5ùf^ù»êõ-¬‚•K4òjV=MWõ®—+AİG‹çiÎMß¡‚£7Ç‰ğU¼»Eş¢¤’Jój‹Â{' /›{üAé’ÒŠjšw€í+Ã°f`˜W&ukğt¼Ÿfø¼¢¸•C›?c.¾ïX+3üf»‘àe¾ë}¾ÁiåœÊÓ½ù)>`rbmV·v©y¦ ½æºÃ²I1×áÇœŞ¾^4êb©†şù·¯ôš)+ï³ìÙë}Ç¯xŒ÷Y\cT®FµŒªk¶õxêˆS£‚JA1 ©YÖx·çƒm"•6IGáBé¢€»…´¥bPƒ´*ó2‡é9ºFx_øıAT=r63>‹–¿ l{ÎƒymÔĞ —½P#ûW7Ô>Kõ§÷³‚9ÄÀª-"½}xv|èQÏ˜(RzßN>~gÌ»ÙeN´;º§Æåî·&¹~1‰G—îLR‡Û=7·tÃ¬ëÑÕ@kó’»wM*åºÄCy¡ ^AŸjéÑ›ãú<[»g÷wâx>3$Pg®+ŸÈk4ÑšÒ·ëx,ÏDø™æ r”•º’#–ôÛñ–]µ´Z<áL*&kƒÃJ	f7?Ã{õOÅõ¶_ƒ\5®IæÏ¦ÈÉÎòàŠ(‡gê“ñ+ğñ	Š*Z[È¶–æ8Ì“¢pávÕ$Ág¯Šş—qÓ&–Û×pìUŞÿ€¼ßää™öàŸ"j"¼q
Jñe×ëm·tYÙF$tõˆS‹To:oüºQhå›ôï¨‚¥bÉzqî7>ä–ì³;Ö(2;¡x^b­Ú=ˆEæÑ¾Â0,'t±¦©>Ód0aÉœ~DL“ÄãÎ}°÷0;¾ÅDZm¾%Y’£y `VâwJ€™zw²ã´-ŠZ«ø9•–âÆ¡|[\´|éÿùv¼Ê¿:AF‘HíÁ]¥–O_˜]¥±œÜ×Ò8‰%× {Öûˆ;ŞˆO)ò“¹,„ˆÚ«à‡@ÉõÏ"t5Gzµ4>^LDYD,›Yi·¼{tä5ûÃ"˜ÍhÑLLÒ“SÔÅ O·º!­E×P`"÷$2ÕÜcµ73—•/@uİñ¦®dì¼>…üÛa(|»Ï×§•å“ˆıìâ§u¥ã:ŒÎkW…g¼€JP>µ¶Î­…¨]úà±—2<€:ÙÎã{ß¢ìíÑˆIŒêÄú6`ÿ†ghQqèÂ¯o_q@:Çuqsıh;K)LçY]qä³ËœGÒ¥T3IAt^dm˜¸ˆ©%¬ÑÀˆzºİZG¶Ÿ¤Š¯P7
ä£LÅ²¤·ºŸ»÷a0¨fÏ^ C]LŠ1—uÕÎ)N÷³oY‚£)õC*‰:&…..8Í$6cöù8J›“ÕÅPzòšÛeøe÷½ÅÓôÂHk”^K°P'-Àî6#Àb^<DŞ„ógá³‚WDtÄşöÁ¶ã•²ã÷[V·@\û]Ü)«çô‰¡
ŸÍ%µ!Æ¹©IÊŞ^AÀ|ÉSÀôˆøƒX$¬ä	µö©EAf`<„uÔ"Üì¦5ÏíÊn;ŒÄC¿2œrGèÔI'aj9?Ò7õ8/\‹ŞÃèpîãVTçÕşĞá%hƒó½ŒTÌkT&Æ7k†€	ë(¶ôN×Z- L%Ñ)iÖ,\4¶œ# M‘ƒÀ^çÕ3B¦¿;RÈ„éÒË³J)h3]Ù¾ëâvÒˆ’ğC÷(1#~Ê™È”SİqÕŸoğN$<<×U2Ê—:Ú™pÈ;$ŞËŒ+KÍB´.‡‘z’ÒŠp¨ú½ÃÇ¡ˆŠš8åGŒ¥áË±êyp&İ}èLÒPeY	İZD3[Ì±Æ×_-‹ËÛÄ™¹ë©©[Ë@®wv,M Ÿv”ş?û§pÂ7ıxâwÔÊóakæš”e=LR³òãØìĞgáB•¢ôíÛ‰D\hL-ò£Ûs×8`ZHÈ
ÔuëxFaE6ğ çY=XÕó¤²‡£ÑíŒs°éJê.à—à4Åh†µKÆ'!ÿzöš™Åda·´4Ÿd&G’…RÔIF<„B?§	øc27á1ºÙ^oÀ|è@ñ=Jå°Âê›1Ÿ½à‡îOTGÚw¾YY2^XgævÀ«ë
 âßnr/ÈG€ŒÇs×„ã1[›–³™Pc^Œ${?ãSÈÑQŠ£ıü)zÜçï´ğC­²ÄŒB<À²†ùqÉ]Cl-Êù‰hØšı1&İ¿Øt(şUÎGG® &±×ça°™ÑŸ£]àf1Ÿ-á* ò~ÊĞ^ìÌäRñHvx—ñC®ÚÔÃåËñ²ZÂ÷Ú ì ÏÖ±¹ƒJİ‚(»IZ1ıïŠºWª‚òF#aR¶¦GT2åJ ê¡©ß$Ö×Ö¿¹¶)ğG,ÉQ^³Ôrôùç;Å&Û»M1‘Stãzw´vÊğ†âøsŠ”ZÑx[«|ŒœyÕÔ:/ï~æZÕ%ô…ôôD@"Û¼ úÖ'd«4©[Û&oª=VÍúÀÜ RÿBê¤áÕİ¿U‡…ñšşÜ
ş½ú¹Şs 0áëÓ49€O¨ÃJï-ör{ÌUf‘Ô'sú~Ñ=Uú€Q/ôşÇbJenÆ[ÙueİİıíYí-xœİ0†ÏÔ±ãîœØr—6à†Ta4”vK:eC-‡a‚ió›)¼„Ç<<&% Ô]·‚ØÏ—-¹Ê©èÃß9oR}¢bBrâz·;AàĞç•¯—¬cJs´®Ù¿l…ıN…á$Ñs¼f…–Pá8
yÿ‹®_¯Jƒè^Ú½(wÆÙ¼³foYH2ZIB;@ğ#k Æèçfb`Ä&¤'´[dMr«³Å×¿#¾¬†Ò›c€[|Îå<^ÒKOš.ã“Œ¢EimK‡|#8»WŸ-Ÿ._…fÜUkºÀA†çM©.upÿ«;ª\:0Ÿ?oå›bè@$qâ>›b\P6éÓ^:áMSÖ·4å‘{¯`º#G\Mÿ'ÉyÂXñá=fOlh˜²T“µˆš‚FwñÚƒ'ƒ‡²Ób›OºÅ•*:XªyË®Ë6<ˆ„oÍÎĞbXÖ˜iÕ	¢Xh³@ CæiÄ»ín3¯ış$êøXÏö?MUÃC®*ï{am¸ë€ôJßˆF–ùgB?Ôœ·"F°`¿:$ä¶%|¡×£)èíË¨Ïş(ÙSu¯J˜bÂr¤¨Ğ ùŞpŞk‚[œ8*¹òLM_¯#bf#r}sÛ@íËuÔ€ÁlmÏMoKW{öFñ’Ğ•Å1A£Ö¿·BÄƒ>"IÖ=JjèuËTšh¤=ø*ı·‘Y'¾6èÑxí‹èİîjqtcÜ>òÁc¢WÑ.º´Øfulpğ_=/ÛŸ¶üÄL¥Z*oNéİû*ûÿ!
˜Şj4«¯õ{Æ¨än*Ä=Ì/K÷¯wª-Tı…æ2)LEé¾áæ¨•ÚtÌ7ópa´CˆrTî÷Š×:ªdßüoYê—°> )QÔA¯ ¸Óê•x§ÏC:>é¶×Ñ^¢Êyt!âŞJNH`¦#’…5îğèW„ÄFhİÑbÕÃµk£å5ÁpÑ*¶9AÌÊ[®x©^¢À›ğùî ç‹İØùhw°-q))¬<i’d­zy£œš_nÙŸzìÓª'	»[Åïˆv¦2«àVÔÚ¥şÓ|¸¶F™aíÀ4¦š³h[6bw77ß-p¸:èÓ—©º(ÓxÏ	1BğÕê‘NÒF`r°Àæ·*H(Ô|çÍ]w^piÿ9ˆ‰ŒYˆ<ËH¯˜ø3×J÷Ùø;ˆJ 5mQH—÷5
Üe[ `NBÂby˜N$.6_ù„DË®ò«ä¤ê€—-÷ÀÖ§#O/W÷ºìLÈı%ûÇÙ#ìü´¯Ø>VøÂ_b›é)§ıîì–À„…gKUV­ìWh0(İ÷æv¿Yfª°yù™[*òw®y”€ih ÀüåÅéæËë¦H€¨rT-é8,¿7¥ˆòb>ãÍwÔOv2ªP€İåôï|ï€¥±‚ÃFåG![=Õªš(à‡ƒâ.mVğ·#IÅó>Ä;¡˜$C€i×A;Û!…Fş_ê;ímÃ;³õì5„W’Š†go]òcÿ™½qş¤gÄàÕ$,ô†¾”,2°ı=ğ|>½ÿ:Ğ^LŸl¼f€{JŸ;`Ævîšúe:Bê¢û1*ï{4m¶T;N²ÿ¸a²í=YÈáş„õôÄê“$ÍÌNõ}ëİr•'ˆä\On%=¾½$5iĞ8wy´#ÖßBÌ'5[À]ğ½U§ı¿¨!J¬í›[+>{Š…
JÓ(j4Bs~jnkdV¤ĞÛ «x£Ø4Ä¸ BÙ3LJâ§ ÇçĞ ¨tªş†}ˆ¡Mæñì|UXÖmÕâ«9ÂK§	ål×9«Š 0nÓ§-”<“äîò¡ƒÈªÚ-oYÏ}Ì)-¢­YÒ2º5\ğŠ5Û‰ÃVé=ÄØno†"‚ÍÑ7.tûƒcØ¼ªRbù=™ıT»XJ]5•U*ÿdNÜf 3¦ô8NäGÁ’H§Z¨$b†K›’åñ¤X†SZ.Ò¢ÿ}®œ‚yØcĞM¡[ül\>{l³ÛE›Õ„†¦¡¯MN3$Òö_ßˆ×#­şL5èˆª}»7•€‘ÃÓ]Âm¬BÕ=Ì \["f¢s¥´ôª¹-íGŠâ}¶ö½f¾¨;3Lšå±Ú §K]$úÂÇñYgO'ùï[Á"²Ñ½Ş’&Hsò»ùV¯‘hDæ72Â£t[İìâ˜LŸ»(X€Á@ÔS.Mâl{e[jK¶³i¯c}Hâ< pÀ–ÇÍkú)ôˆf£ñ¹³m3¶^zXî¡bÿ°Zÿ§…z%Ã Ôkñ71cYq¦ÃT†dZı©34õÛæ·Öu·uƒ·~'&iq´Ô®†È2¤ÜŒ±IÎû+’®XÖjQ¡Cğ½½ßY¯Ëöôx’g/xL@Ú×ê”ÅºÊq‚îk¡A¤ÃG­²¶Ty9ÑY­ŸoE¿æĞg(­†@œìwş·UAeû°ì0ÀÈø§áaş¼¼(g^½nàmº˜–¿
™•”ï+9†*Ò°ü•dÄûkµ€ôÈŞ*êİPşÜ§Ñ?Gß	,l»üöL†Ü?Eƒ/\1'È3ğ˜õÉ…X`—®°‚ÎR±
RDè®‘«È/Pôü˜x/w&Œ¯¥Y¹ÓO	í~ê‡<;K­<)jÉZ\ıwß<`C$ïÌ ô‚Š[¨wUe–i/Z¬š²Õ ›>O óüöÑøû{ıëå=ÓLœQ‰	Y©N–|mñÍãu×ÉX‚ğx°KíÆª¨“vD[ãwÉ¸=ÇXvKgd˜3eS¸IN¿ûhÑd0`Y›Oô‰ìùåÈİ!PñŠ`'ª„×/d‡Zf5« ô¨«!’Ë¥Œ¨1¨‘RÁÕ‰ss8Y •¦hš_ÈW¡Î¿:»¥å£B£F	hÌŸhsÍ‡í×}ì{Ç†ğ…ºÈ¡—@å¤©©kCú{èt&0„¥vÅ:?Ÿâ³ùšèpˆ×æµë{SÓ"a[„2ä¼j -XÙÀ;X‘¬ºz’ü«â%`(íPDL	ÏÀºŸ‰ŠVî67yÊø¤ág¨¹ÿÂß¤„qÙtëò‰b¹:¬Ñ˜™F[!à¬œa~(¾@ØÆiÔ‘ù¥9e:U«¦Şˆ„òhÿVø°.ÌOöeÉ«'¨wƒ à	7ì¼ÖaÖV\Meß‰>äÂêŒ‘PÓÄ6Şİ@í~®ì4uƒõÌ¬‘Ì+WCM ƒ÷†Üs}#;;•9+Õ·ÍCQ;ë‰ë‹Gü0ÙÁÏ7Ö¢õ2¼Úçrî'T¸ï¾Ÿ‚şF”¬<¼S^jƒ=f
…0TF@LT‡²¿ÃaPGÜ`®Ï_|$ºB)@¶PZÿËò*e¹ˆu•]I\Hm9#Âè^å¡'hú]–à¶²r;"É·ËONPs»ŞTlfl‡”»ºó‹'ÓgŒ"¶ğíDD§ÄH£Ú§yœçaîÜT÷nY5ºíãx8+JÂààİ¢­+“|~oìBaË²}±a´O ¹~YOÁ§Í¦I#¦æ’)ÆãQv×Z& ™f»”¿Táî÷d;İvê–àºOæÖ–½}/:wÏÛê¦ğO.E&E'0 Ş—SûÆ~†}¨—Ö¬A
¤O+lTöã]¼YÅ%MUÊ·5Ğpô½Ô¹©nıOåÂSÎY±ÜŸıûÎ(Ø àÄ Slûv¥Ää<¾P$‹Jİ=Qè+’ç³%½²`A]p¿V„7½/¯Q ‚zc¤Ø¦-m'=™Ö´«nÀœìí›œ1ê³ó›K=»èrC=´G¸úc]Ò mMzÂcäB£bšşï‹_¦'D¨="]ØßÜïà­NN¢›ğ®Úª|xgMd§1CäP xÙ!ÔÙïC¬ÌåŸ =}qTVø~·cqŒ.øŒğ?ÓF¯ÒD’âI¤ÕI÷eÉ4K¨ÌÉnúÀf²ÿ–ƒy–òº¼½ÉcxêHí¼Í›Ø:x¦.¾¾Œêİ_+ï;ÀR¼Æe“X@¼Š¨¡Æˆìu"‚hñv4¤yP¨QÙKM›qk=:Bç+v-.¶ÓÂı†öMd¤¦dªO]SN¦ •ö>Àqmÿ¾ëÆĞ^İëçö……¿`ÿªçE+VÑĞ’ÇWâ.`™ê_áLo™ÓÏÔÊ?²ås‘12üêÙã*p·‹ì
;§È‰Mn†©Ì}™ú±x>oÛ­ïõlÖf “@ßÈ¼ñdêÍ»º4NÁ½™…/à¹D»¤Õ¿üI¬/Œ•üÏê>¤*éúãXr¤÷ˆª²„¢Í-0%¸ÍoéŒÿ´©y±Äù‚J|áÀ,(’… À?\R<r¤“Ê¦¬dV#z£úòêòšß³*òS‹)–ügµ`Yu)‰‹Ù¯Ëî¡ª|@¸'_¬×OÊdYA9kµ‡Dî”ñèL"SCOÖõ%Æƒò˜‘kª²àÍt÷JïOMT¸UÄ„x†)Î
5ªË=¤'ÙP8<Y²·Fè#§çğ—\ÈMcn1˜ï4ˆ&Ş YøçhV…(Èä¸IüJº©¯ÍËÌ(ÎŒx…³ëÔŒá  EÏ‘9HÏ) újóé|QçƒtÊ’*ö6å¹
à…£à@Okz]n»¥O:%y'±mæS|t×k³\PñùøVı0­dsªÀt¤fP3N^©‹ÇÅïğ¤–*9ì¥¦[[—ŞµÚá¹QÒeÃU{x<l¯UtĞË%wE`bã£ã£­Lş¥×¶xIvŒë*o—,v©úLñ‡‡§ïåúÔÊı%–ßr‚ÙiÕ}KÈ)N—eVÕØÖ2}‹ÁBs#³Rß;´
™ÙXy (‚kFÏŞÜKdf¡ÿÈH4Ó*9Uó’IRp<3-³iš
<ŠÄ‚79r,„2¢s¶c~l¯Ö)ÿ~…©öèñ¥[ıÍJø{ìåÃrñ±é%7Ÿ[(JL•UNÏ˜IĞÉì2š9ñÌdl¦{t—+…HøÚ…XûHÊÆ	}#Ï˜E/èy2¤¦UA)CÜÚe†tàïø`U†cC%nòÇ`ôn¢shjpÔ"ÛÔ·yıŒ‰è§ËS)[÷ş ,ªö%¬Uõà>l“ *PW9PLoñ‡Nÿ3fÃ]¨¥bH]ÕgÄ-Sk;kì°1œ†(±À&´úB–Ó³¾8®‰#‡BR1;3Tô«º*€Ë—Æ¢º4>ØPäÈ#uo³¾n1Å­]æW=)ååšüs¿ËGÕ	~)½‡U˜"xÀv	Ëõ8C³·V6ïÃ.KÜ˜fË—>…BÀTî¿‡vÖ)«âm]sÅ×h¨Ş¿¨eİX4‡xÚ+·)‚˜? ¾İä­ê¢XÜ¸ÆÎ·(~ÔÓT 	5V8<]Ô@n+ –ú6;~)Ñ/”áf2\v¼ğUÎE¾¢Å%È[ÀãöÃì@˜È<Iÿ;\ÄOÁô2[ZÑÉ)“AÏ¿d#F=¡Í””Í m³.ù:° 
$–¡7,“RFŒ4g6¦–èy¾]«¾]°‘rD–´´wRò!š
T—ëæÈ´~Y¾X¡µiñÔéÅ–FÔLÔê•ø¡"Q'ZË$ŸD_ÓÚk—B–Ü8Ñ#‘ZìG;î°±§»3R7WkWŠjTş”CI\ ÑìÎ†ãHª£»eÆgrş¨–œÑ=úæÒ°cŒ<+»ÿàŞìh¿Q‡ıMš(´€5w?úüélÉxÓLhŠf,okì6²¿úÕØsâ²×R¼½½uh²ÖO pcUä+AÀ˜·ô’43/lZüSkPjj®ÚbGÙ¥vŒ˜Q%T1.•ÿt%H‘h6ï9..-— qNèVöëhL+ƒ¿Œ.\;EX•»@#ÑŠRÆo¯âÆ}/A ä«+„	PM@>~ØN; Ë}é$êÏ³²‚©¼f˜«Åüİ¶ÏcŸmı,tÖÅT2ÄÙA§wïí¹™†T¾>Å-Z/ğWóKÓåøq*¾ÀÔ¬ëaxs?Á;Ëaì¾¼û#Dhq÷Ïl_Ô»äîP:Å¹/†;€ñAd¯$AE®8¨™:È¢0MHT™Öh˜øò\¾öçÏÄo¬™ÔpHá%¡m±kº­*éÅ¸!â€ '¢,úYiOBCjÑÅœwÆ`Ôv’µšÛx¢"ñüiô)/-8h–2ø ³C·m(jTK¾Â‹?¿ñìl÷‡Ä²ÂGFî‡Û¡b	æ"¦XÒx6N½ÓIRÁÄøé·G4c-!úŠIµ"eÀQ…(r¹0’2ÕCÈ­4N\õŠâ6ˆ¶]«d°ìˆ&6X/§Ö¦A±‡ßşè¸Ï0 «|"€€>ú|°;ä¾_a7¥‘qFÑíÙ©<¹²*îùo@jå–çüİ²Ü‰>;~f
³Ä$08#‚QğóÏnÏÊÄ°ç:Nß_’¸H´QÒ`Ï¢;/Ã¿ğû B*`-õ»Ï÷`ñT`&Ö-ë'Ü¹÷WïóÂ ¯ˆ‰~£›”bO"¤nã.¥È[)ÀŠQ¤Ã•È'øÛ'“±²ã=<<!ö›EÁ™sTl?lÉ¨î’b&—ÔGÆùf²Zuú•ÙÀáßø•ë|tï¶qÁÌÕ¥3íòrÆßêâÁu#
"'ÓŒ8°Ì°‘©€c„ôsq8ÈæW<…1Ğı<qtš%=ÒqâÒÔ9FéU:¨ç‚Ö½îdPA¥2‰:+ôB¯Ú»àİcD‹¢g”'z2y˜t†0C)Wòã9V]ZF.Èğ'm
¾~vêDrÅŞêd­'‹¾>Ê¥²H›*öR5®íìcğ†İ±»…F¦Y'Ç¶X‰ÉqÁéï|âÑ&ÛgX<QTe4èT±¦ŞaŠ¤}ówÜ¿½»òÔ(Ó“2Ğ
šïöYcrYJH¡Ë¤€£8=ñ`½ZÔ˜{¡²W'‡aÈØ*_f-I¼¥/§ñü¡BeÁã}-8Ğ‘¶‰*’wO=ŠX¦o	w°™:" İR/S@lú¿V¶™Ê¦Ï‰6™W©+'•=)×œ;Å!ÛÙ[Îg]¯¶ó#ß³Ô0íåD®³Ò9KhÊâ1}‚Ê“>
œIéêã‹%€'nËEĞ3C2]>ğjŸ8U¹s1êP²>-d:x&á ©Ê=—„Z>Vªá¬b«-ğßßå*õ˜<øÁoO<:|-si’ÌdóÁc!cÙ¤9õwå«x ^ºÅºÄ­”ŠÃyÖÍÜ\ÅeÕÓÛ0°]
¯èEÌnfÁ“÷½»hÄA
ã‹‚ì• ¬Ïƒ?XÈK²sr‘­x/í{É»¿1~R%B0Şkj.`ê*ó(ØÙœP™~ë‡ËæÖË£~‰ü¶}ésÑàæÑ–ˆÔN†
GŒ«Z- t%Ø¦&#îƒ’xëìü¤x%.sÌ'œÊ$¢A™d8ÔVÊ	aGyÂíBMYÊ?W‡€ÓV&º9aTèíœ^gì·ÇŒœWŸÈ€²“µ¥ï‡ªsâ…¯\wÜ…#fj×V4<¢<O]ûó]!Õ\fŠØX#­ŞØ-Ímš¯uæõ³eQ33†¬­AKÛ[Ëâ¸qæ,f9ª~MÓ[Áwã¢¡ù°6•qŸÉdfõ‡¢¡}:3³¿¡ÑôƒlÄ€ÍßÙ|“­Ê¶YĞ¹éÂĞ¿¬½À¶òò¢Š“FgãV"ÂÂ!Ú#Û±¹5@]bà´Å® ”¼Û"¸ïQüúûÈ#ã€MòÏ8?­lšÕa#àS~JuBù‚­4¸®ët2øÜ©D^h;üô{P<9‰iÜ(óåÎXÛ°¸ıØDù3:×X6õªmÁ}˜ıt¸$ ,;ëÉš¥ß_JòöÓîÌGƒ›»-‘ë¶ökû•H™‰T•¾2lÕˆƒ²8”Y¨‘z™Ø†‹3Ïvh¿Äc!¦-Aº¸ŞÀÈ0ÈFF*ê’åÖ|6Ä(È‹°ğ¶·8ë?Xƒ7œİÇBù`ŞòÏ;RÁ*u~~"¯A_gïe
òpëùD–nÕ£j§Êë¼·şÖq=¦VÍpşøóŠD‹É‰¡I¬%æÏ`ÿ¿¼~W(»‹£‹!;$£zœƒÇ'—ÂBôzÖš<ÿwòFñ†¿ã¼¥äÚawiWß\>@^0Å£ŒÎ•€Í*?Œ*ö¿TŒĞ¼l8hÛZÚ#"ÌÛ+d•Ix_€¨Ü™½xhŞF+Fx‡œXuªô˜×ÛB/¤(üHD£¸cHèş†?¬à´×“;OúÉÓS¦´ ¦¿gÕgNÇâRj8ÃéÛ.O=bÊ©­ä
2/Xs#}-6¹«©QgeĞéàáÓv6Ü¹w»ÀBÍ
Yè×ÒShR–®6Ù\²}Óß(ğªCÛÁ<™Êî ì•I=‰ˆH…µ–±l¤N|ò¾,C²/œ¾`;œà~DaÊÎFòÕKÚ8×ÙE„S†ËË‡ª&N‡eW–cE‚JÜWÃ±ÑñN;ŸËözŸ¨« ¹ Æ*ı³Ú°wÖÎá–fä5µh¸–®`™™}¤ VXÊÚíAR‡İñTè–®øí\Òj6mûKØ„læºQêöp›9®çòºËk&¹+•¶’óaú‘Mïv¹|àkueºájEC^ÌlDEwì· à²”VÌ­@Ğ¨u?Ô'"ÌšñE™ë€ğ	%üL1ù»–Á#sUcÑ–ÔçNŞ°é­ë‹ĞTÊä–ş6Ì¶u–A&T$Ğ[©é†>ÄÎK]6±¡EË/Ğí˜½oünâiœYªÊ
g¨Cø~&Şîl]OÃ0^’q‘Ø$E¬èÊIºObûŒénÎ_·ó pì®©8ITĞ€;³ç¡“Õãìß
î… ZË½lÔ¦EO¨µÑ(=LÏJŞé”ù'bÖf„şV”Y+hîÙ.ZğG…Ì8øóÅ£˜Ëç½¯#h°¶Ú*º©;š»ëÿØ{Õ<zÅ¯_ãÎXzHmQcÉórß2&ë–‹–*˜/½Æò˜uH¸†9z—Fü²„µ-{¢{Æº&«©È U£şg´«ur56i‡‚Î	mÿ¤:¢<˜¯Æïs­Ö>ÓôI%IúA¦óÄw€‚§[€_ŞE±k"¥Ü*}mş1ç`WNkE0(=M'rÄ¸û’â·rdĞ§H)håZÁ.6âyf! 8Âui i¯ê‘ïí‚ÔãÊ»ñHc a}Åu ¬4¶×2;“qkÌÌL¬Ô<òøÃéŠ„[Rœéş>-üzïßœ‚ZlŞD'üÇ¸n=³NT€ùŸùúÀZ5a’i\Ò¿·@˜N˜$ŸBªjÔ0@rOÇ­l»ãñ ÏİšgÉ×ÿ8v¬”ƒywIÆĞBg[+R’œ3+Ì
ùIoñSVŠI°~ûwıyäz9ı.‹ËÜëÊK{ÿë`ª¤3'ÀNYXI"êç¢TµiÔo†D†	œ!á]š^|kÖÚŸO¶šœ×j†Î…tæá^G¨óÛğS’v1©$áBÓåéŸÀJELvÁ„Rp'ÀaÁn¦"Içu—®Õ/!² ;5é»]@
,Ñç3ZjÓ³Ë|n6¼ªĞ%‡>O€†ı¿jqxrvŠ¹q¹];î·ñ©LŠc€XxÀï{£Qœ¸û²KˆWM:éAõH(AÅß'õ‘râùÈî™ŠYÅ¨ä×f%LíŒ_F‘í†r¶OË{Z+RÙ{;õ£y™üÏòCI[´OŠ»Éöê•Sª˜=î<‹öK®{V«şŸÕT q•jÃ8 «É‘¡ZP¥ÑÖC^¡7˜Ë5Ğ>sjmëôv†ğÃº E:ös-X\ÑHv©ıˆìã¼¸KƒóTN²BûÖ¼ã-@= ”DûƒyÆîÇ(à ¥—~?ÿìbÙ)½_o¥£Ù‚ë·sEQÊlÎg;tC¤Ö>ˆXœ·•_ß…«9¨#”+lëóMÃhÍeîMÿ>ı€(ÆY„d_«ï_îî%ey[ˆ=uÛò\Ö*gû”ÃÃÇbzêlÙÆ.±ö ëá´L¸­Åšê¾\èÊÂ4wI¿’ ¶6cX_ğ=ñS·pƒåƒÅUFØ÷îIl–%—Ânõ×‹ßš 6˜,ÚuÑ‰åÍ?’44ãKu+ƒ¨„m=E¿¼l:à¶Çˆeã7ª“Œ†#ı=Ÿtğ|STnÀûòí¨
¼Î2¬˜]¬‚ÈÇN&‡@Œ³z1ñ:5ZúåV£"dn_'yÑ5ŞsùÍ[|/â’pŒ'8pBed(ğ®7°ˆ^”ï]Öò¥xíÏGU—Ü|'‚ÉMşÇ@Ô.äÿ¦R±PTãñèüTb3æ­2íÄAX{Ú¿!"”‘EXwkßNn8YÁÜœo"¶ ^ŠSTµFÇG¹Í1.„,£cÏ|R]ÈÄÿ Õ¸»û`PqkÛèq€(pqZÚ¼ ì°ß³™&ä9C³Ïl`=İ›¡ßöäï#,.9Š•gÔY­ RÊ+¾a¸Œr¶î3+¨#¼a©e1”­ı,ßaJÏ°XÔåÖ•HM¥f¾ì%—el®>Ş”™•ª,ÊÇÖ7Ò•#3áß¢	B5&q+
Ö’ù6ŞcúSZæ×È~'såbƒ(ÎšÍ œ™	RËˆí¢]øjÑõ0ıâòâV2sxd¯D4B'±ª|F®t}x'_–nÇ-†yrÜ=š½N¢…·³¢l@Û,‹¹•+wi¨?q°q¾Q^òp¿¤ãHU
ìÆ«¡<I„SN¢;CÄ1íüÃFMÀ'—·"|ak}«¦i¯‘Ç
€ŠşŸ„y†.˜:ãæƒ`eê«üööŞ9ˆ½7x0€kúéÅó×š‚CÚõ‚È=®ËS„dK>°)°ŞyÅéU[· Xvaêoğ 4ì¿Ÿ¢§~¿C?áÏ;·uL—•!í“àƒ8RóÕ9=c%#]y„K"aI÷ Ô¶æ¶ø­İû‚#€ùÿØ¶%DwÍkr2Ş9š†#GöşŒ öf•¹ly©%@”l ,ODô{OË©˜`\F*yMßöÙ$Œ¹L4)rLÀ‰t]‡'Ãïbcn‡°“¾È¾%•Ÿ¥{0”¾ôcømSÅ;6Í~¤÷{Ò¿Fşó`x‰­xSÌJÊ¢2jşQièıMC(öª($ø!ëuê/Áã°-Ÿš³qÀÙ×ô"[	‚bK„Ğp‡tôwÛ¸®¬ĞGíîéDm®øy$EŠŞ'a9‘n}ú?wu| 9qc2;ïqŸó#ş·W~ıóP/Î3Â:I×“.9ÁÅI[qGÎ>£ `<×G(¬ÈfÖ~/}Ÿş®85Ÿà\`O¤×ê×jOø 2lTãÀ¶Ÿ¢h_M·å“Ø»Ò”LV`TÉŠ„ÜU^³ö*o¸&gjZªÉX Ë¿PyMPGAÅÑv¡¨;…Çõ¦åú«	•=’(ĞO=-RŒI÷¼nt));
	WRITE_ONCE(rdp->dynticks_nesting, 1);
	WARN_ON_ONCE(rdp->dynticks_nmi_nesting);
	WRITE_ONCE(rdp->dynticks_nmi_nesting, DYNTICK_IRQ_NONIDLE);
	instrumentation_end();
}

/**
 * rcu_idle_exit - inform RCU that current CPU is leaving idle
 *
 * Exit idle mode, in other words, -enter- the mode in which RCU
 * read-side critical sections can occur.
 *
 * If you add or remove a call to rcu_idle_exit(), be sure to test with
 * CONFIG_RCU_EQS_DEBUG=y.
 */
void rcu_idle_exit(void)
{
	unsigned long flags;

	local_irq_save(flags);
	rcu_eqs_exit(false);
	local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(rcu_idle_exit);

#ifdef CONFIG_NO_HZ_FULL
/**
 * rcu_user_exit - inform RCU that we are exiting userspace.
 *
 * Exit RCU idle mode while entering the kernel because it can
 * run a RCU read side critical section anytime.
 *
 * If you add or remove a call to rcu_user_exit(), be sure to test with
 * CONFIG_RCU_EQS_DEBUG=y.
 */
void noinstr rcu_user_exit(void)
{
	rcu_eqs_exit(true);
}

/**
 * __rcu_irq_enter_check_tick - Enable scheduler tick on CPU if RCU needs it.
 *
 * The scheduler tick is not normally enabled when CPUs enter the kernel
 * from nohz_full userspace execution.  After all, nohz_full userspace
 * execution is an RCU quiescent state and the time executing in the kernel
 * is quite short.  Except of course when it isn't.  And it is not hard to
 * cause a large system to spend tens of seconds or even minutes looping
 * in the kernel, which can cause a number of problems, include RCU CPU
 * stall warnings.
 *
 * Therefore, if a nohz_full CPU fails to report a quiescent state
 * in a timely manner, the RCU grace-period kthread sets that CPU's
 * ->rcu_urgent_qs flag with the expectation that the next interrupt or
 * exception will invoke this function, which will turn on the scheduler
 * tick, which will enable RCU to detect that CPU's quiescent states,
 * for example, due to cond_resched() calls in CONFIG_PREEMPT=n kernels.
 * The tick will be disabled once a quiescent state is reported for
 * this CPU.
 *
 * Of course, in carefully tuned systems, there might never be an
 * interrupt or exception.  In that case, the RCU grace-period kthread
 * will eventually cause one to happen.  However, in less carefully
 * controlled environments, this function allows RCU to get what it
 * needs without creating otherwise useless interruptions.
 */
void __rcu_irq_enter_check_tick(void)
{
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);

	// If we're here from NMI there's nothing to do.
	if (in_nmi())
		return;

	RCU_LOCKDEP_WARN(rcu_dynticks_curr_cpu_in_eqs(),
			 "Illegal rcu_irq_enter_check_tick() from extended quiescent state");

	if (!tick_nohz_full_cpu(rdp->cpu) ||
	    !READ_ONCE(rdp->rcu_urgent_qs) ||
	    READ_ONCE(rdp->rcu_forced_tick)) {
		// RCU doesn't need nohz_full help from this CPU, or it is
		// already getting that help.
		return;
	}

	// We get here only when not in an extended quiescent state and
	// from interrupts (as opposed to NMIs).  Therefore, (1) RCU is
	// already watching and (2) The fact that we are in an interrupt
	// handler and that the rcu_node lock is an irq-disabled lock
	// prevents self-deadlock.  So we can safely recheck under the lock.
	// Note that the nohz_full state currently cannot change.
	raw_spin_lock_rcu_node(rdp->mynode);
	if (rdp->rcu_urgent_qs && !rdp->rcu_forced_tick) {
		// A nohz_full CPU is in the kernel and RCU needs a
		// quiescent state.  Turn on the tick!
		WRITE_ONCE(rdp->rcu_forced_tick, true);
		tick_dep_set_cpu(rdp->cpu, TICK_DEP_BIT_RCU);
	}
	raw_spin_unlock_rcu_node(rdp->mynode);
}
#endif /* CONFIG_NO_HZ_FULL */

/**
 * rcu_nmi_enter - inform RCU of entry to NMI context
 *
 * If the CPU was idle from RCU's viewpoint, update rdp->dynticks and
 * rdp->dynticks_nmi_nesting to let the RCU grace-period handling know
 * that the CPU is active.  This implementation permits nested NMIs, as
 * long as the nesting level does not overflow an int.  (You will probably
 * run out of stack space first.)
 *
 * If you add or remove a call to rcu_nmi_enter(), be sure to test
 * with CONFIG_RCU_EQS_DEBUG=y.
 */
noinstr void rcu_nmi_enter(void)
{
	long incby = 2;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);

	/* Complain about underflow. */
	WARN_ON_ONCE(rdp->dynticks_nmi_nesting < 0);

	/*
	 * If idle from RCU viewpoint, atomically increment ->dynticks
	 * to mark non-idle and increment ->dynticks_nmi_nesting by one.
	 * Otherwise, increment ->dynticks_nmi_nesting by two.  This means
	 * if ->dynticks_nmi_nesting is equal to one, we are guaranteed
	 * to be in the outermost NMI handler that interrupted an RCU-idle
	 * period (observation due to Andy Lutomirski).
	 */
	if (rcu_dynticks_curr_cpu_in_eqs()) {

		if (!in_nmi())
			rcu_dynticks_task_exit();

		// RCU is not watching here ...
		rcu_dynticks_eqs_exit();
		// ... but is watching here.

		instrumentation_begin();
		// instrumentation for the noinstr rcu_dynticks_curr_cpu_in_eqs()
		instrument_atomic_read(&rdp->dynticks, sizeof(rdp->dynticks));
		// instrumentation for the noinstr rcu_dynticks_eqs_exit()
		instrument_atomic_write(&rdp->dynticks, sizeof(rdp->dynticks));

		incby = 1;
	} else if (!in_nmi()) {
		instrumentation_begin();
		rcu_irq_enter_check_tick();
	} else  {
		instrumentation_begin();
	}

	trace_rcu_dyntick(incby == 1 ? TPS("Endirq") : TPS("++="),
			  rdp->dynticks_nmi_nesting,
			  rdp->dynticks_nmi_nesting + incby, atomic_read(&rdp->dynticks));
	instrumentation_end();
	WRITE_ONCE(rdp->dynticks_nmi_nesting, /* Prevent store tearing. */
		   rdp->dynticks_nmi_nesting + incby);
	barrier();
}

/**
 * rcu_irq_enter - inform RCU that current CPU is entering irq away from idle
 *
 * Enter an interrupt handler, which might possibly result in exiting
 * idle mode, in other words, entering the mode in which read-side critical
 * sections can occur.  The caller must have disabled interrupts.
 *
 * Note that the Linux kernel is fully capable of entering an interrupt
 * handler that it never exits, for example when doing upcalls to user mode!
 * This code assumes that the idle loop never does upcalls to user mode.
 * If your architecture's idle loop does do upcalls to user mode (or does
 * anything else that results in unbalanced calls to the irq_enter() and
 * irq_exit() functions), RCU will give you what you deserve, good and hard.
 * But very infrequently and irreproducibly.
 *
 * Use things like work queues to work around this limitation.
 *
 * You have been warned.
 *
 * If you add or remove a call to rcu_irq_enter(), be sure to test with
 * CONFIG_RCU_EQS_DEBUG=y.
 */
noinstr void rcu_irq_enter(void)
{
	lockdep_assert_irqs_disabled();
	rcu_nmi_enter();
}

/*
 * Wrapper for rcu_irq_enter() where interrupts are enabled.
 *
 * If you add or remove a call to rcu_irq_enter_irqson(), be sure to test
 * with CONFIG_RCU_EQS_DEBUG=y.
 */
void rcu_irq_enter_irqson(void)
{
	unsigned long flags;

	local_irq_save(flags);
	rcu_irq_enter();
	local_irq_restore(flags);
}

/*
 * Check to see if any future non-offloaded RCU-related work will need
 * to be done by the current CPU, even if none need be done immediately,
 * returning 1 if so.  This function is part of the RCU implementation;
 * it is -not- an exported member of the RCU API.  This is used by
 * the idle-entry code to figure out whether it is safe to disable the
 * scheduler-clock interrupt.
 *
 * Just check whether or not this CPU has non-offloaded RCU callbacks
 * queued.
 */
int rcu_needs_cpu(void)
{
	return !rcu_segcblist_empty(&this_cpu_ptr(&rcu_data)->cblist) &&
		!rcu_rdp_is_offloaded(this_cpu_ptr(&rcu_data));
}

/*
 * If any sort of urgency was applied to the current CPU (for example,
 * the scheduler-clock interrupt was enabled on a nohz_full CPU) in order
 * to get to a quiescent state, disable it.
 */
static void rcu_disable_urgency_upon_qs(struct rcu_data *rdp)
{
	raw_lockdep_assert_held_rcu_node(rdp->mynode);
	WRITE_ONCE(rdp->rcu_urgent_qs, false);
	WRITE_ONCE(rdp->rcu_need_heavy_qs, false);
	if (tick_nohz_full_cpu(rdp->cpu) && rdp->rcu_forced_tick) {
		tick_dep_clear_cpu(rdp->cpu, TICK_DEP_BIT_RCU);
		WRITE_ONCE(rdp->rcu_forced_tick, false);
	}
}

/**
 * rcu_is_watching - see if RCU thinks that the current CPU is not idle
 *
 * Return true if RCU is watching the running CPU, which means that this
 * CPU can safely enter RCU read-side critical sections.  In other words,
 * if the current CPU is not in its idle loop or is in an interrupt or
 * NMI handler, return true.
 *
 * Make notrace because it can be called by the internal functions of
 * ftrace, and making this notrace removes unnecessary recursion calls.
 */
notrace bool rcu_is_watching(void)
{
	bool ret;

	preempt_disable_notrace();
	ret = !rcu_dynticks_curr_cpu_in_eqs();
	preempt_enable_notrace();
	return ret;
}
EXPORT_SYMBOL_GPL(rcu_is_watching);

/*
 * If a holdout task is actually running, request an urgent quiescent
 * state from its CPU.  This is unsynchronized, so migrations can cause
 * the request to go to the wrong CPU.  Which is OK, all that will happen
 * is that the CPU's next context switch will be a bit slower and next
 * time around this task will generate another request.
 */
void rcu_request_urgent_qs_task(struct task_struct *t)
{
	int cpu;

	barrier();
	cpu = task_cpu(t);
	if (!task_curr(t))
		return; /* This task is not running on that CPU. */
	smp_store_release(per_cpu_ptr(&rcu_data.rcu_urgent_qs, cpu), true);
}

#if defined(CONFIG_PROVE_RCU) && defined(CONFIG_HOTPLUG_CPU)

/*
 * Is the current CPU online as far as RCU is concerned?
 *
 * Disable preemption to avoid false positives that could otherwise
 * happen due to the current CPU number being sampled, this task being
 * preempted, its old CPU being taken offline, resuming on some other CPU,
 * then determining that its old CPU is now offline.
 *
 * Disable checking if in an NMI handler because we cannot safely
 * report errors from NMI handlers anyway.  In addition, it is OK to use
 * RCU on an offline processor during initial boot, hence the check for
 * rcu_scheduler_fully_active.
 */
bool rcu_lockdep_current_cpu_online(void)
{
	struct rcu_data *rdp;
	bool ret = false;

	if (in_nmi() || !rcu_scheduler_fully_active)
		return true;
	preempt_disable_notrace();
	rdp = this_cpu_ptr(&rcu_data);
	/*
	 * Strictly, we care here about the case where the current CPU is
	 * in rcu_cpu_starting() and thus has an excuse for rdp->grpmask
	 * not being up to date. So arch_spin_is_locked() might have a
	 * false positive if it's held by some *other* CPU, but that's
	 * OK because that just means a false *negative* on the warning.
	 */
	if (rcu_rdp_cpu_online(rdp) || arch_spin_is_locked(&rcu_state.ofl_lock))
		ret = true;
	preempt_enable_notrace();
	return ret;
}
EXPORT_SYMBOL_GPL(rcu_lockdep_current_cpu_online);

#endif /* #if defined(CONFIG_PROVE_RCU) && defined(CONFIG_HOTPLUG_CPU) */

/*
 * When trying to report a quiescent state on behalf of some other CPU,
 * it is our responsibility to check for and handle potential overflow
 * of the rcu_node ->gp_seq counter with respect to the rcu_data counters.
 * After all, the CPU might be in deep idle state, and thus executing no
 * code whatsoever.
 */
static void rcu_gpnum_ovf(struct rcu_node *rnp, struct rcu_data *rdp)
{
	raw_lockdep_assert_held_rcu_node(rnp);
	if (ULONG_CMP_LT(rcu_seq_current(&rdp->gp_seq) + ULONG_MAX / 4,
			 rnp->gp_seq))
		WRITE_ONCE(rdp->gpwrap, true);
	if (ULONG_CMP_LT(rdp->rcu_iw_gp_seq + ULONG_MAX / 4, rnp->gp_seq))
		rdp->rcu_iw_gp_seq = rnp->gp_seq + ULONG_MAX / 4;
}

/*
 * Snapshot the specified CPU's dynticks counter so that we can later
 * credit them with an implicit quiescent state.  Return 1 if this CPU
 * is in dynticks idle mode, which is an extended quiescent state.
 */
static int dyntick_save_progress_counter(struct rcu_data *rdp)
{
	rdp->dynticks_snap = rcu_dynticks_snap(rdp);
	if (rcu_dynticks_in_eqs(rdp->dynticks_snap)) {
		trace_rcu_fqs(rcu_state.name, rdp->gp_seq, rdp->cpu, TPS("dti"));
		rcu_gpnum_ovf(rdp->mynode, rdp);
		return 1;
	}
	return 0;
}

/*
 * Return true if the specified CPU has passed through a quiescent
 * state by virtue of being in or having passed through an dynticks
 * idle state since the last call to dyntick_save_progress_counter()
 * for this same CPU, or by virtue of having been offline.
 */
static int rcu_implicit_dynticks_qs(struct rcu_data *rdp)
{
	unsigned long jtsq;
	struct rcu_node *rnp = rdp->mynode;

	/*
	 * If the CPU passed through or entered a dynticks idle phase with
	 * no active irq/NMI handlers, then we can safely pretend that the CPU
	 * already acknowledged the request to pass through a quiescent
	 * state.  Either way, that CPU cannot possibly be in an RCU
	 * read-side critical section that started before the beginning
	 * of the current RCU grace period.
	 */
	if (rcu_dynticks_in_eqs_since(rdp, rdp->dynticks_snap)) {
		trace_rcu_fqs(rcu_state.name, rdp->gp_seq, rdp->cpu, TPS("dti"));
		rcu_gpnum_ovf(rnp, rdp);
		return 1;
	}

	/*
	 * Complain if a CPU that is considered to be offline from RCU's
	 * perspective has not yet reported a quiescent state.  After all,
	 * the offline CPU should have reported a quiescent state during
	 * the CPU-offline process, or, failing that, by rcu_gp_init()
	 * if it ran concurrently with either the CPU going offline or the
	 * last task on a leaf rcu_node structure exiting its RCU read-side
	 * critical section while all CPUs corresponding to that structure
	 * are offline.  This added warning detects bugs in any of these
	 * code paths.
	 *
	 * The rcu_node structure's ->lock is held here, which excludes
	 * the relevant portions the CPU-hotplug code, the grace-period
	 * initialization code, and the rcu_read_unlock() code paths.
	 *
	 * For more detail, please refer to the "Hotplug CPU" section
	 * of RCU's Requirements documentation.
	 */
	if (WARN_ON_ONCE(!rcu_rdp_cpu_online(rdp))) {
		struct rcu_node *rnp1;

		pr_info("%s: grp: %d-%d level: %d ->gp_seq %ld ->completedqs %ld\n",
			__func__, rnp->grplo, rnp->grphi, rnp->level,
			(long)rnp->gp_seq, (long)rnp->completedqs);
		for (rnp1 = rnp; rnp1; rnp1 = rnp1->parent)
			pr_info("%s: %d:%d ->qsmask %#lx ->qsmaskinit %#lx ->qsmaskinitnext %#lx ->rcu_gp_init_mask %#lx\n",
				__func__, rnp1->grplo, rnp1->grphi, rnp1->qsmask, rnp1->qsmaskinit, rnp1->qsmaskinitnext, rnp1->rcu_gp_init_mask);
		pr_info("%s %d: %c online: %ld(%d) offline: %ld(%d)\n",
			__func__, rdp->cpu, ".o"[rcu_rdp_cpu_online(rdp)],
			(long)rdp->rcu_onl_gp_seq, rdp->rcu_onl_gp_flags,
			(long)rdp->rcu_ofl_gp_seq, rdp->rcu_ofl_gp_flags);
		return 1; /* Break things loose after complaining. */
	}

	/*
	 * A CPU running for an extended time within the kernel can
	 * delay RCU grace periods: (1) At age jiffies_to_sched_qs,
	 * set .rcu_urgent_qs, (2) At age 2*jiffies_to_sched_qs, set
	 * both .rcu_need_heavy_qs and .rcu_urgent_qs.  Note that the
	 * unsynchronized assignments to the per-CPU rcu_need_heavy_qs
	 * variable are safe because the assignments are repeated if this
	 * CPU failed to pass through a quiescent state.  This code
	 * also checks .jiffies_resched in case jiffies_to_sched_qs
	 * is set way high.
	 */
	jtsq = READ_ONCE(jiffies_to_sched_qs);
	if (!READ_ONCE(rdp->rcu_need_heavy_qs) &&
	    (time_after(jiffies, rcu_state.gp_start + jtsq * 2) ||
	     time_after(jiffies, rcu_state.jiffies_resched) ||
	     rcu_state.cbovld)) {
		WRITE_ONCE(rdp->rcu_need_heavy_qs, true);
		/* Store rcu_need_heavy_qs before rcu_urgent_qs. */
		smp_store_release(&rdp->rcu_urgent_qs, true);
	} else if (time_after(jiffies, rcu_state.gp_start + jtsq)) {
		WRITE_ONCE(rdp->rcu_urgent_qs, true);
	}

	/*
	 * NO_HZ_FULL CPUs can run in-kernel without rcu_sched_clock_irq!
	 * The above code handles this, but only for straight cond_resched().
	 * And some in-kernel loops check need_resched() before calling
	 * cond_resched(), which defeats the above code for CPUs that are
	 * running in-kernel with scheduling-clock interrupts disabled.
	 * So hit them over the head with the resched_cpu() hammer!
	 */
	if (tick_nohz_full_cpu(rdp->cpu) &&
	    (time_after(jiffies, READ_ONCE(rdp->last_fqs_resched) + jtsq * 3) ||
	     rcu_state.cbovld)) {
		WRITE_ONCE(rdp->rcu_urgent_qs, true);
		resched_cpu(rdp->cpu);
		WRITE_ONCE(rdp->last_fqs_resched, jiffies);
	}

	/*
	 * If more than halfway to RCU CPU stall-warning time, invoke
	 * resched_e = "stat",
		.seq_show = cpu_cfs_stat_show,
	},
#endif
#ifdef CONFIG_RT_GROUP_SCHED
	{
		.name = "rt_runtime_us",
		.read_s64 = cpu_rt_runtime_read,
		.write_s64 = cpu_rt_runtime_write,
	},
	{
		.name = "rt_period_us",
		.read_u64 = cpu_rt_period_read_uint,
		.write_u64 = cpu_rt_period_write_uint,
	},
#endif
#ifdef CONFIG_UCLAMP_TASK_GROUP
	{
		.name = "uclamp.min",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_uclamp_min_show,
		.write = cpu_uclamp_min_write,
	},
	{
		.name = "uclamp.max",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_uclamp_max_show,
		.write = cpu_uclamp_max_write,
	},
#endif
	{ }	/* Terminate */
};

static int cpu_extra_stat_show(struct seq_file *sf,
			       struct cgroup_subsys_state *css)
{
#ifdef CONFIG_CFS_BANDWIDTH
	{
		struct task_group *tg = css_tg(css);
		struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;
		u64 throttled_usec, burst_usec;

		throttled_usec = cfs_b->throttled_time;
		do_div(throttled_usec, NSEC_PER_USEC);
		burst_usec = cfs_b->burst_time;
		do_div(burst_usec, NSEC_PER_USEC);

		seq_printf(sf, "nr_periods %d\n"
			   "nr_throttled %d\n"
			   "throttled_usec %llu\n"
			   "nr_bursts %d\n"
			   "burst_usec %llu\n",
			   cfs_b->nr_periods, cfs_b->nr_throttled,
			   throttled_usec, cfs_b->nr_burst, burst_usec);
	}
#endif
	return 0;
}

#ifdef CONFIG_FAIR_GROUP_SCHED
static u64 cpu_weight_read_u64(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	struct task_group *tg = css_tg(css);
	u64 weight = scale_load_down(tg->shares);

	return DIV_ROUND_CLOSEST_ULL(weight * CGROUP_WEIGHT_DFL, 1024);
}

static int cpu_weight_write_u64(struct cgroup_subsys_state *css,
				struct cftype *cft, u64 weight)
{
	/*
	 * cgroup weight knobs should use the common MIN, DFL and MAX
	 * values which are 1, 100 and 10000 respectively.  While it loses
	 * a bit of range on both ends, it maps pretty well onto the shares
	 * value used by scheduler and the round-trip conversions preserve
	 * the original value over the entire range.
	 */
	if (weight < CGROUP_WEIGHT_MIN || weight > CGROUP_WEIGHT_MAX)
		return -ERANGE;

	weight = DIV_ROUND_CLOSEST_ULL(weight * 1024, CGROUP_WEIGHT_DFL);

	return sched_group_set_shares(css_tg(css), scale_load(weight));
}

static s64 cpu_weight_nice_read_s64(struct cgroup_subsys_state *css,
				    struct cftype *cft)
{
	unsigned long weight = scale_load_down(css_tg(css)->shares);
	int last_delta = INT_MAX;
	int prio, delta;

	/* find the closest nice value to the current weight */
	for (prio = 0; prio < ARRAY_SIZE(sched_prio_to_weight); prio++) {
		delta = abs(sched_prio_to_weight[prio] - weight);
		if (delta >= last_delta)
			break;
		last_delta = delta;
	}

	return PRIO_TO_NICE(prio - 1 + MAX_RT_PRIO);
}

static int cpu_weight_nice_write_s64(struct cgroup_subsys_state *css,
				     struct cftype *cft, s64 nice)
{
	unsigned long weight;
	int idx;

	if (nice < MIN_NICE || nice > MAX_NICE)
		return -ERANGE;

	idx = NICE_TO_PRIO(nice) - MAX_RT_PRIO;
	idx = array_index_nospec(idx, 40);
	weight = sched_prio_to_weight[idx];

	return sched_group_set_shares(css_tg(css), scale_load(weight));
}
#endif

static void __maybe_unused cpu_period_quota_print(struct seq_file *sf,
						  long period, long quota)
{
	if (quota < 0)
		seq_puts(sf, "max");
	else
		seq_printf(sf, "%ld", quota);

	seq_printf(sf, " %ld\n", period);
}

/* caller should put the current value in *@periodp before calling */
static int __maybe_unused cpu_period_quota_parse(char *buf,
						 u64 *periodp, u64 *quotap)
{
	char tok[21];	/* U64_MAX */

	if (sscanf(buf, "%20s %llu", tok, periodp) < 1)
		return -EINVAL;

	*periodp *= NSEC_PER_USEC;

	if (sscanf(tok, "%llu", quotap))
		*quotap *= NSEC_PER_USEC;
	else if (!strcmp(tok, "max"))
		*quotap = RUNTIME_INF;
	else
		return -EINVAL;

	return 0;
}

#ifdef CONFIG_CFS_BANDWIDTH
static int cpu_max_show(struct seq_file *sf, void *v)
{
	struct task_group *tg = css_tg(seq_css(sf));

	cpu_period_quota_print(sf, tg_get_cfs_period(tg), tg_get_cfs_quota(tg));
	return 0;
}

static ssize_t cpu_max_write(struct kernfs_open_file *of,
			     char *buf, size_t nbytes, loff_t off)
{
	struct task_group *tg = css_tg(of_css(of));
	u64 period = tg_get_cfs_period(tg);
	u64 burst = tg_get_cfs_burst(tg);
	u64 quota;
	int ret;

	ret = cpu_period_quota_parse(buf, &period, &quota);
	if (!ret)
		ret = tg_set_cfs_bandwidth(tg, period, quota, burst);
	return ret ?: nbytes;
}
#endif

static struct cftype cpu_files[] = {
#ifdef CONFIG_FAIR_GROUP_SCHED
	{
		.name = "weight",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_u64 = cpu_weight_read_u64,
		.write_u64 = cpu_weight_write_u64,
	},
	{
		.name = "weight.nice",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_s64 = cpu_weight_nice_read_s64,
		.write_s64 = cpu_weight_nice_write_s64,
	},
	{
		.name = "idle",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_s64 = cpu_idle_read_s64,
		.write_s64 = cpu_idle_write_s64,
	},
#endif
#ifdef CONFIG_CFS_BANDWIDTH
	{
		.name = "max",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_max_show,
		.write = cpu_max_write,
	},
	{
		.name = "max.burst",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_u64 = cpu_cfs_burst_read_u64,
		.write_u64 = cpu_cfs_burst_write_u64,
	},
#endif
#ifdef CONFIG_UCLAMP_TASK_GROUP
	{
		.name = "uclamp.min",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_uclamp_min_show,
		.write = cpu_uclamp_min_write,
	},
	{
		.name = "uclamp.max",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_uclamp_max_show,
		.write = cpu_uclamp_max_write,
	},
#endif
	{ }	/* terminate */
};

struct cgroup_subsys cpu_cgrp_subsys = {
	.css_alloc	= cpu_cgroup_css_alloc,
	.css_online	= cpu_cgroup_css_online,
	.css_released	= cpu_cgroup_css_released,
	.css_free	= cpu_cgroup_css_free,
	.css_extra_stat_show = cpu_extra_stat_show,
	.fork		= cpu_cgroup_fork,
	.can_attach	= cpu_cgroup_can_attach,
	.attach		= cpu_cgroup_attach,
	.legacy_cftypes	= cpu_legacy_files,
	.dfl_cftypes	= cpu_files,
	.early_init	= true,
	.threaded	= true,
};

#endif	/* CONFIG_CGROUP_SCHED */

void dump_cpu_task(int cpu)
{
	pr_info("Task dump for CPU %d:\n", cpu);
	sched_show_task(cpu_curr(cpu));
}

/*
 * Nice levels are multiplicative, with a gentle 10% change for every
 * nice level changed. I.e. when a CPU-bound task goes from nice 0 to
 * nice 1, it will get ~10% less CPU time than another CPU-bound task
 * that remained on nice 0.
 *
 * The "10% effect" is relative and cumulative: from _any_ nice level,
 * if you go up 1 level, it's -10% CPU usage, if you go down 1 level
 * it's +10% CPU usage. (to achieve that we use a multiplier of 1.25.
 * If a task goes up by ~10% and another task goes down by ~10% then
 * the relative distance between them is ~25%.)
 */
const int sched_prio_to_weight[40] = {
 /* -20 */     88761,     71755,     56483,     46273,     36291,
 /* -15 */     29154,     23254,     18705,     14949,     11916,
 /* -10 */      9548,      7620,      6100,      4904,      3906,
 /*  -5 */      3121,      2501,      1991,      1586,      1277,
 /*   0 */      1024,       820,       655,       526,       423,
 /*   5 */       335,       272,       215,       172,       137,
 /*  10 */       110,        87,        70,        56,        45,
 /*  15 */        36,        29,        23,        18,        15,
};

/*
 * Inverse (2^32/x) values of the sched_prio_to_weight[] array, precalculated.
 *
 * In cases where the weight does not change often, we can use the
 * precalculated inverse to speed up arithmetics by turning divisions
 * into multiplications:
 */
const u32 sched_prio_to_wmult[40] = {
 /* -20 */     48388,     59856,     76040,     92818,    118348,
 /* -15 */    147320,    184698,    229616,    287308,    360437,
 /* -10 */    449829,    563644,    704093,    875809,   1099582,
 /*  -5 */   1376151,   1717300,   2157191,   2708050,   3363326,
 /*   0 */   4194304,   5237765,   6557202,   8165337,  10153587,
 /*   5 */  12820798,  15790321,  19976592,  24970740,  31350126,
 /*  10 */  39045157,  49367440,  61356676,  76695844,  95443717,
 /*  15 */ 119304647, 148102320, 186737708, 238609294, 286331153,
};

void call_trace_sched_update_nr_running(struct rq *rq, int count)
{
        trace_sched_update_nr_running_tp(rq, count);
}
                               // SPDX-License-Identifier: GPL-2.0-only
/*
 *  kernel/sched/core.c
 *
 *  Core kernel scheduler code and related syscalls
 *
 *  Copyright (C) 1991-2002  Linus Torvalds
 */
#include <linux/highmem.h>
#include <linux/hrtimer_api.h>
#include <linux/ktime_api.h>
#include <linux/sched/signal.h>
#include <linux/syscalls_api.h>
#include <linux/debug_locks.h>
#include <linux/prefetch.h>
#include <linux/capability.h>
#include <linux/pgtable_api.h>
#include <linux/wait_bit.h>
#include <linux/jiffies.h>
#include <linux/spinlock_api.h>
#include <linux/cpumask_api.h>
#include <linux/lockdep_api.h>
#include <linux/hardirq.h>
#include <linux/softirq.h>
#include <linux/refcount_api.h>
#include <linux/topology.h>
#include <linux/sched/clock.h>
#include <linux/sched/cond_resched.h>
#include <linux/sched/debug.h>
#include <linux/sched/isolation.h>
#include <linux/sched/loadavg.h>
#include <linux/sched/mm.h>
#include <linux/sched/nohz.h>
#include <linux/sched/rseq_api.h>
#include <linux/sched/rt.h>

#include <linux/blkdev.h>
#include <linux/context_tracking.h>
#include <linux/cpuset.h>
#include <linux/delayacct.h>
#include <linux/init_task.h>
#include <linux/interrupt.h>
#include <linux/ioprio.h>
#include <linux/kallsyms.h>
#include <linux/kcov.h>
#include <linux/kprobes.h>
#include <linux/llist_api.h>
#include <linux/mmu_context.h>
#include <linux/mmzone.h>
#include <linux/mutex_api.h>
#include <linux/nmi.h>
#include <linux/nospec.h>
#include <linux/perf_event_api.h>
#include <linux/profile.h>
#include <linux/psi.h>
#include <linux/rcuwait_api.h>
#include <linux/sched/wake_q.h>
#include <linux/scs.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/vtime.h>
#include <linux/wait_api.h>
#include <linux/workqueue_api.h>

#ifdef CONFIG_PREEMPT_DYNAMIC
# ifdef CONFIG_GENERIC_ENTRY
#  include <linux/entry-common.h>
# endif
#endif

#include <uapi/linux/sched/types.h>

#include <asm/switch_to.h>
#include <asm/tlb.h>

#define CREATE_TRACE_POINTS
#include <linux/sched/rseq_api.h>
#include <trace/events/sched.h>
#undef CREATE_TRACE_POINTS

#include "sched.h"
#include "stats.h"
#include "autogroup.h"

#include "autogroup.h"
#include "pelt.h"
#include "smp.h"
#include "stats.h"

#include "../workqueue_internal.h"
#include "../../fs/io-wq.h"
#include "../smpboot.h"

/*
 * Export tracepoints that act as a bare tracehook (ie: have no trace event
 * associated with them) to allow external modules to probe them.
 */
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_cfs_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_rt_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_dl_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_irq_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_se_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_thermal_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_cpu_capacity_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_overutilized_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_util_est_cfs_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_util_est_se_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_update_nr_running_tp);

DEFINE_PER_CPU_SHARED_ALIGNED(struct rq, runqueues);

#ifdef CONFIG_SCHED_DEBUG
/*
 * Debugging: various feature bits
 *
 * If SCHED_DEBUG is disabled, each compilation unit has its own copy of
 * sysctl_sched_features, defined in sched.h, to allow constants propagation
 * at compile time and compiler optimization based on features default.
 */
#define SCHED_FEAT(name, enabled)	\
	(1UL << __SCHED_FEAT_##name) * enabled |
const_debug unsigned int sysctl_sched_features =
#include "features.h"
	0;
#undef SCHED_FEAT

/*
 * Print a warning if need_resched is set for the given duration (if
 * LATENCY_WARN is enabled).
 *
 * If sysctl_resched_latency_warn_once is set, only one warning will be shown
 * per boot.
 */
__read_mostly int sysctl_resched_latency_warn_ms = 100;
__read_mostly int sysctl_resched_latency_warn_once = 1;
#endif /* CONFIG_SCHED_DEBUG */

/*
 * Number of tasks to iterate in a single balance run.
 * Limited because this is done with IRQs disabled.
 */
#ifdef CONFIG_PREEMPT_RT
const_debug unsigned int sysctl_sched_nr_migrate = 8;
#else
const_debug unsigned int sysctl_sched_nr_migrate = 32;
#endif

/*
 * period over which we measure -rt task CPU usage in us.
 * default: 1s
 */
unsigned int sysctl_sched_rt_period = 1000000;

__read_mostly int scheduler_running;

#ifdef CONFIG_SCHED_CORE

DEFINE_STATIC_KEY_FALSE(__sched_core_enabled);

/* kernel prio, less is more */
static inline int __task_prio(struct task_struct *p)
{
	if (p->sched_class == &stop_sched_class) /* trumps deadline */
		return -2;

	if (rt_prio(p->prio)) /* includes deadline */
		return p->prio; /* [-1, 99] */

	if (p->sched_class == &idle_sched_class)
		return MAX_RT_PRIO + NICE_WIDTH; /* 140 */

	return MAX_RT_PRIO + MAX_NICE; /* 120, squash fair */
}

/*
 * l(a,b)
 * le(a,b) := !l(b,a)
 * g(a,b)  := l(b,a)
 * ge(a,b) := !l(a,b)
 */

/* real prio, less is less */
static inline bool prio_less(struct task_struct *a, struct task_struct *b, bool in_fi)
{

	int pa = __task_prio(a), pb = __task_prio(b);

	if (-pa < -pb)
		return true;

	if (-pb < -pa)
		return false;

	if (pa == -1) /* dl_prio() doesn't work because of stop_class above */
		return !dl_time_before(a->dl.deadline, b->dl.deadline);

	if (pa == MAX_RT_PRIO + MAX_NICE)	/* fair */
		return cfs_prio_less(a, b, in_fi);

	return false;
}

static inline bool __sched_core_less(struct task_struct *a, struct task_struct *b)
{
	if (a->core_cookie < b->core_cookie)
		return true;

	if (a->core_cookie > b->core_cookie)
		return false;

	/* flip prio, so high prio is leftmost */
	if (prio_less(b, a, !!task_rq(a)->core->core_forceidle_count))
		return true;

	return false;
}

#define __node_2_sc(node) rb_entry((node), struct task_struct, core_node)

static inline bool rb_sched_core_less(struct rb_node *a, const struct rb_node *b)
{
	return __sched_core_less(__node_2_sc(a), __node_2_sc(b));
}

static inline int rb_sched_core_cmp(const void *key, const struct rb_node *node)
{
	const struct task_struct *p = __node_2_sc(node);
	unsigned long cookie = (unsigned long)key;

	if (cookie < p->core_cookie)
		return -1;

	if (cookie > p->core_cookie)
		return 1;

	return 0;
}

void sched_core_enqueue(struct rq *rq, struct task_struct *p)
{
	rq->core->core_task_seq++;

	if (!p->core_cookie)
		return;

	rb_add(&p->core_node, &rq->core_tree, rb_sched_core_less);
}

void sched_core_dequeue(struct rq *rq, struct task_struct *p, int flags)
{
	rq->core->core_task_seq++;

	if (sched_core_enqueued(p)) {
		rb_erase(&p->core_node, &rq->core_tree);
		RB_CLEAR_NODE(&p->core_node);
	}

	/*
	 * Migrating the last task off the cpu, with the cpu in forced idle
	 * state. Reschedule to create an accounting edge for forced idle,
	 * and re-examine whether the core is still in forced idle state.
	 */
	if (!(flags & DEQUEUE_SAVE) && rq->nr_running == 1 &&
	    rq->core->core_forceidle_count && rq->curr == rq->idle)
		resched_curr(rq);
}

/*
 * Find left-most (aka, highest priority) task matching @cookie.
 */
static struct task_struct *sched_core_find(struct rq *rq, unsigned long cookie)
{
	struct rb_node *node;

	node = rb_find_first((void *)cookie, &rq->core_tree, rb_sched_core_cmp);
	/*
	 * The idle task always matches any cookie!
	 */
	if (!node)
		return idle_sched_class.pick_task(rq);

	return __node_2_sc(node);
}

static struct task_struct *sched_core_next(struct task_struct *p, unsigned long cookie)
{
	struct rb_node *node = &p->core_node;

	node = rb_next(node);
	if (!node)
		return NULL;

	p = container_of(node, struct task_struct, core_node);
	if (p->core_cookie != cookie)
		return NULL;

	return p;
}

/*
 * Magic required such that:
 *
 *	raw_spin_rq_lock(rq);
 *	...
 *	raw_spin_rq_unlock(rq);
 *
 * ends up locking and unlocking the _same_ lock, and all CPUs
 * always agree on what rq has what lock.
 *
 * XXX entirely possible to selectively enable cores, don't bother for now.
 */

static DEFINE_MUTEX(sched_core_mutex);
static atomic_t sched_core_count;
static struct cpumask sched_core_mask;

static void sched_core_lock(int cpu, unsigned long *flags)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	int t, i = 0;

	local_irq_save(*flags);
	for_each_cpu(t, smt_mask)
		raw_spin_lock_nested(&cpu_rq(t)->__lock, i++);
}

static void sched_core_unlock(int cpu, unsigned long *flags)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	int t;

	for_each_cpu(t, smt_mask)
		raw_spin_unlock(&cpu_rq(t)->__lock);
	local_irq_restore(*flags);
}

static void __sched_core_flip(bool enabled)
{
	unsigned long flags;
	int cpu, t;

	cpus_read_lock();

	/*
	 * Toggle the online cores, one by one.
	 */
	cpumask_copy(&sched_core_mask, cpu_online_mask);
	for_each_cpu(cpu, &sched_core_mask) {
		const struct cpumask *smt_mask = cpu_smt_mask(cpu);

		sched_core_lock(cpu, &flags);

		for_each_cpu(t, smt_mask)
			cpu_rq(t)->core_enabled = enabled;

		cpu_rq(cpu)->core->core_forceidle_start = 0;

		sched_core_unlock(cpu, &flags);

		cpumask_andnot(&sched_core_mask, &sched_core_mask, smt_mask);
	}

	/*
	 * Toggle the offline CPUs.
	 */
	cpumask_copy(&sched_core_mask, cpu_possible_mask);
	cpumask_andnot(&sched_core_mask, &sched_core_mask, cpu_online_mask);

	for_each_cpu(cpu, &sched_core_mask)
		cpu_rq(cpu)->core_enabled = enabled;

	cpus_read_unlock();
}

static void sched_core_assert_empty(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		WARN_ON_ONCE(!RB_EMPTY_ROOT(&cpu_rq(cpu)->core_tree));
}

static void __sched_core_enable(void)
{
	static_branch_enable(&__sched_core_enabled);
	/*
	 * Ensure all previous instances of raw_spin_rq_*lock() have finished
	 * and future ones will observe !sched_core_disabled().
	 */
	synchronize_rcu();
	__sched_core_flip(true);
	sched_core_assert_empty();
}

static void __sched_core_disable(void)
{
	sched_core_assert_empty();
	__sched_core_flip(false);
	static_branch_disable(&__sched_core_enabled);
}

void sched_core_get(void)
{
	if (atomic_inc_not_zero(&sched_core_count))
		return;

	mutex_lock(&sched_core_mutex);
	if (!atomic_read(&sched_core_count))
		__sched_core_enable();

	smp_mb__before_atomic();
	atomic_inc(&sched_core_count);
	mutex_unlock(&sched_core_mutex);
}

static void __sched_core_put(struct work_struct *work)
{
	if (atomic_dec_and_mutex_lock(&sched_core_count, &sched_core_mutex)) {
		__sched_core_disable();
		mutex_unlock(&sched_core_mutex);
	}
}

void sched_core_put(void)
{
	static DECLARE_WORK(_work, __sched_core_put);

	/*
	 * "There can be only one"
	 *
	 * Either this is the last one, or we don't actually need to do any
	 * 'work'. If it is the last *again*, we rely on
	 * WORK_STRUCT_PENDING_BIT.
	 */
	if (!atomic_add_unless(&sched_core_count, -1, 1))
		schedule_work(&_work);
}

#else /* !CONFIG_SCHED_CORE */

static inline void sched_core_enqueue(struct rq *rq, struct task_struct *p) { }
static inline void
sched_core_dequeue(struct rq *rq, struct task_struct *p, int flags) { }

#endif /* CONFIG_SCHED_CORE */

/*
 * part of the period that we allow rt tasks to run in us.
 * default: 0.95s
 */
int sysctl_sched_rt_runtime = 950000;


/*
 * Serialization rules:
 *
 * Lock order:
 *
 *   p->pi_lock
 *     rq->lock
 *       hrtimer_cpu_base->lock (hrtimer_start() for bandwidth controls)
 *
 *  rq1->lock
 *    rq2->lock  where: rq1 < rq2
 *
 * Regular state:
 *
 * Normal scheduling state is serialized by rq->lock. __schedule() takes the
 * local CPU's rq->lock, it optionally removes the task from the runqueue and
 * always looks at the local rq data structures to find the most eligible task
 * to run next.
 *
 * Task enqueue is also under rq->lock, possibly taken from another CPU.
 * Wakeups from another LLC domain might use an IPI to transfer the enqueue to
 * the local CPU to avoid bouncing the runqueue state around [ see
 * ttwu_queue_wakelist() ]
 *
 * Task wakeup, specifically wakeups that involve migration, are horribly
 * complicated to avoid having to take two rq->locks.
 *
 * Special state:
 *
 * System-calls and anything external will use task_rq_lock() which acquires
 * both p->pi_lock and rq->lock. As a consequence the state they change is
 * stable while holding either lock:
 *
 *  - sched_setaffinity()/
 *    set_cpus_allowed_ptr():	p->cpus_ptr, p->nr_cpus_allowed
 *  - set_user_nice():		p->se.load, p->*prio
 *  - __sched_setscheduler():	p->sched_class, p->policy, p->*prio,
 *				p->se.load, p->rt_priority,
 *				p->dl.dl_{runtime, deadline, period, flags, bw, density}
 *  - sched_setnuma():		p->numa_preferred_nid
 *  - sched_move_task()/
 *    cpu_cgroup_fork():	p->sched_task_group
 *  - uclamp_update_active()	p->uclamp*
 *
 * p->state <- TASK_*:
 *
 *   is changed locklessly using set_current_state(), __set_current_state() or
 *   set_special_state(), see their respective comments, or by
 *   try_to_wake_up(). This latter uses p->pi_lock to serialize against
 *   concurrent self.
 *
 * p->on_rq <- { 0, 1 = TASK_ON_RQ_QUEUED, 2 = TASK_ON_RQ_MIGRATING }:
 *
 *   is set by activate_task() and cleared by deactivate_task(), under
 *   rq->lock. Non-zero indicates the task is runnable, the special
 *   ON_RQ_MIGRATING state is used for migration without holding both
 *   rq->locks. It indicates task_cpu() is not stable, see task_rq_lock().
 *
 * p->on_cpu <- { 0, 1 }:
 *
 *   is set by prepare_task() and cleared by finish_task() such that it will be
 *   set before p is scheduled-in and cleared after p is scheduled-out, both
 *   under rq->lock. Non-zero indicates the task is running on its CPU.
 *
 *   [ The astute reader will observe that it is possible for two tasks on one
 *     CPU to have ->on_cpu = 1 at the same time. ]
 *
 * task_cpu(p): is changed by set_task_cpu(), the rules are:
 *
 *  - Don't call set_task_cpu() on a blocked task:
 *
 *    We don't care what CPU we're not running on, this simplifies hotplug,
 *    the CPU assignment of blocked tasks isn't required to be valid.
 *
 *  - for try_to_wake_up(), called under p->pi_lock:
 *
 *    This allows try_to_wake_up() to only take one rq->lock, see its comment.
 *
 *  - for migration called under rq->lock:
 *    [ see task_on_rq_migrating() in task_rq_lock() ]
 *
 *    o move_queued_task()
 *    o detach_task()
 *
 *  - for migration called under double_rq_lock():
 *
 *    o __migrate_swap_task()
 *    o push_rt_task() / pull_rt_task()
 *    o push_dl_task() / pull_dl_task()
 *    o dl_task_offline_migration()
 *
 */

void raw_spin_rq_lock_nested(struct rq *rq, int subclass)
{
	raw_spinlock_t *lock;

	/* Matches synchronize_rcu() in __sched_core_enable() */
	preempt_disable();
	if (sched_core_disabled()) {
		raw_spin_lock_nested(&rq->__lock, subclass);
		/* preempt_count *MUST* be > 1 */
		preempt_enable_no_resched();
		return;
	}

	for (;;) {
		lock = __rq_lockp(rq);
		raw_spin_lock_nested(lock, subclass);
		if (likely(lock == __rq_lockp(rq))) {
			/* preempt_count *MUST* be > 1 */
			preempt_enable_no_resched();
			return;
		}
		raw_spin_unlock(lock);
	}
}

bool raw_spin_rq_trylock(struct rq *rq)
{
	raw_spinlock_t *lock;
	bool ret;

	/* Matches synchronize_rcu() in __sched_core_enable() */
	preempt_disable();
	if (sched_core_disabled()) {
		ret = raw_spin_trylock(&rq->__lock);
		preempt_enable();
		return ret;
	}

	for (;;) {
		lock = __rq_lockp(rq);
		ret = raw_spin_trylock(lock);
		if (!ret || (likely(lock == __rq_lockp(rq)))) {
			preempt_enable();
			return ret;
		}
		raw_spin_unlock(lock);
	}
}

void raw_spin_rq_unlock(struct rq *rq)
{
	raw_spin_unlock(rq_lockp(rq));
}

#ifdef CONFIG_SMP
/*
 * double_rq_lock - safely lock two runqueues
 */
void double_rq_lock(struct rq *rq1, struct rq *rq2)
{
	lockdep_assert_irqs_disabled();

	if (rq_order_less(rq2, rq1))
		swap(rq1, rq2);

	raw_spin_rq_lock(rq1);
	if (__rq_lockp(rq1) == __rq_lockp(rq2))
		return;

	raw_spin_rq_lock_nested(rq2, SINGLE_DEPTH_NESTING);
}
#endif

/*
 * __task_rq_lock - lock the rq @p resides on.
 */
struct rq *__task_rq_lock(struct task_struct *p, struct rq_flags *rf)
	__acquires(rq->lock)
{
	struct rq *rq;

	lockdep_assert_held(&p->pi_lock);

	for (;;) {
		rq = task_rq(p);
		raw_spin_rq_lock(rq);
		if (likely(rq == task_rq(p) && !task_on_rq_migrating(p))) {
			rq_pin_lock(rq, rf);
			return rq;
		}
		raw_spin_rq_unlock(rq);

		while (unlikely(task_on_rq_migrating(p)))
			cpu_relax();
	}
}

/*
 * task_rq_lock - lock p->pi_lock and lock the rq @p resides on.
 */
struct rq *task_rq_lock(struct task_struct *p, struct rq_flags *rf)
	__acquires(p->pi_lock)
	__acquires(rq->lock)
{
	struct rq *rq;

	for (;;) {
		raw_spin_lock_irqsave(&p->pi_lock, rf->flags);
		rq = task_rq(p);
		raw_spin_rq_lock(rq);
		/*
		 *	move_queued_task()		task_rq_lock()
		 *
		 *	ACQUIRE (rq->lock)
		 *	[S] ->on_rq = MIGRATING		[L] rq = task_rq()
		 *	WMB (__set_task_cpu())		ACQUIRE (rq->lock);
		 *	[S] ->cpu = new_cpu		[L] task_rq()
		 *					[L] ->on_rq
		 *	RELEASE (rq->lock)
		 *
		 * If we observe the old CPU in task_rq_lock(), the acquire of
		 * the old rq->lock will fully serialize against the stores.
		 *
		 * If we observe the new CPU in task_rq_lock(), the address
		 * dependency headed by '[L] rq = task_rq()' and the acquire
		 * will pair with the WMB to ensure we then also see migrating.
		 */
		if (likely(rq == task_rq(p) && !task_on_rq_migrating(p))) {
			rq_pin_lock(rq, rf);
			return rq;
		}
		raw_spin_rq_unlock(rq);
		raw_spin_unlock_irqrestore(&p->pi_lock, rf->flags);

		while (unlikely(task_on_rq_migrating(p)))
			cpu_relax();
	}
}

/*
 * RQ-clock updating methods:
 */

static void update_rq_clock_task(struct rq *rq, s64 delta)
{
/*
 * In theory, the compile should just see 0 here, and optimize out the call
 * to sched_rt_avg_update. But I don't trust it...
 */
	s64 __maybe_unused steal = 0, irq_delta = 0;

#ifdef CONFIG_IRQ_TIME_ACCOUNTING
	irq_delta = irq_time_read(cpu_of(rq)) - rq->prev_irq_time;

	/*
	 * Since irq_time is only updated on {soft,}irq_exit, we might run into
	 * this case when a previous update_rq_clock() happened inside a
	 * {soft,}irq region.
	 *
	 * When this happens, we stop ->clock_task and only update the
	 * prev_irq_time stamp to account for the part that fit, so that a next
	 * update will consume the rest. This ensures ->clock_task is
	 * monotonic.
	 *
	 * It does however cause some slight miss-attribution of {soft,}irq
	 * time, a more accurate solution would be to update the irq_time using
	 * the current rq->clock timestamp, except that would require using
	 * atomic ops.
	 */
	if (irq_delta > delta)
		irq_delta = delta;

	rq->prev_irq_time += irq_delta;
	delta -= irq_delta;
#endif
#ifdef CONFIG_PARAVIRT_TIME_ACCOUNTING
	if (static_key_false((&paravirt_steal_rq_enabled))) {
		steal = paravirt_steal_clock(cpu_of(rq));
		steal -= rq->prev_steal_time_rq;

		if (unlikely(steal > delta))
			steal = delta;

		rq->prev_steal_time_rq += steal;
		delta -= steal;
	}
#endif

	rq->clock_task += delta;

#ifdef CONFIG_HAVE_SCHED_AVG_IRQ
	if ((irq_delta + steal) && sched_feat(NONTASK_CAPACITY))
		update_irq_load_avg(rq, irq_delta + steal);
#endif
	update_rq_clock_pelt(rq, delta);
}

void update_rq_clock(struct rq *rq)
{
	s64 delta;

	lockdep_assert_rq_held(rq);

	if (rq->clock_update_flags & RQCF_ACT_SKIP)
		return;

#ifdef CONFIG_SCHED_DEBUG
	if (sched_feat(WARN_DOUBLE_CLOCK))
		SCHED_WARN_ON(rq->clock_update_flags & RQCF_UPDATED);
	rq->clock_update_flags |= RQCF_UPDATED;
#endif

	delta = sched_clock_cpu(cpu_of(rq)) - rq->clock;
	if (delta < 0)
		return;
	rq->clock += delta;
	update_rq_clock_task(rq, delta);
}

#ifdef CONFIG_SCHED_HRTICK
/*
 * Use HR-timers to deliver accurate preemption points.
 */

static void hrtick_clear(struct rq *rq)
{
	if (hrtimer_active(&rq->hrtick_timer))
		hrtimer_cancel(&rq->hrtick_timer);
}

/*
 * High-resolution timer tick.
 * Runs from hardirq context with interrupts disabled.
 */
static enum hrtimer_restart hrtick(struct hrtimer *timer)
{
	struct rq *rq = container_of(timer, struct rq, hrtick_timer);
	struct rq_flags rf;

	WARN_ON_ONCE(cpu_of(rq) != smp_processor_id());

	rq_lock(rq, &rf);
	update_rq_clock(rq);
	rq->curr->sched_class->task_tick(rq, rq->curr, 1);
	rq_unlock(rq, &rf);

	return HRTIMER_NORESTART;
}

#ifdef CONFIG_SMP

static void __hrtick_restart(struct rq *rq)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	ktime_t time = rq->hrtick_time;

	hrtimer_start(timer, time, HRTIMER_MODE_ABS_PINNED_HARD);
}

/*
 * called from hardirq (IPI) context
 */
static void __hrtick_start(void *arg)
{
	struct rq *rq = arg;
	struct rq_flags rf;

	rq_lock(rq, &rf);
	__hrtick_restart(rq);
	rq_unlock(rq, &rf);
}

/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	s64 delta;

	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense and can cause timer DoS.
	 */
	delta = max_t(s64, delay, 10000LL);
	rq->hrtick_time = ktime_add_ns(timer->base->get_time(), delta);

	if (rq == this_rq())
		__hrtick_restart(rq);
	else
		smp_call_function_single_async(cpu_of(rq), &rq->hrtick_csd);
}

#else
/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense. Rely on vruntime for fair