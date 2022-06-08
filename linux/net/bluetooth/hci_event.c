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
		.namELF                      p'      4     (                �������t?S�Ë��  ��t���t
ǂ$&      �����ǃ�      ǃ�      [Í�&    Í�&    �����UWV��S����d�    �D$1�����   ��  ����   ��,&  ����   �F�$    1ɺ����Tj �   h    ����������t   �   �   ��  �,$������Å��Z   �8�   �
   �h���  ǅ�      �x�(�G������G$�u���Dh   j P������ud�u�h@  jPW������؃�$��������   ��,&  �$���������   �    u-�D$d+    u��1�[^_]Ív �    t�������������   f�f�f�f�������@�@    �@    1�Í�&    �����1�Í�&    ������VS�p�^��������v���������[^Í�&    �t& �����UWVS�Ã��5   �$�ʅ��2  �h����   �}|����   �w`����   9��  ��4  ���7  ��1����S�D$�GT�D$����ȉщD$�D$ЉD$9�sv�t$�$�D$���������������C�t$��C�OT��)�9�C�s�s��C�GL9���   ����[^_]������-   ���  ��[^_]Í�&    �v �L$�D$)ʉы$�ΉL$������D$�L$+L$�$ȋL$��ʉ���4  ������T�����&    �   ���   ��[^_]Í�&    �v )���K��������[^_]�������    �   ���P����  ��&    ��    �����WVS�p���   ��T  �����1҉����������  �������ǃ(&      �   �����1�[^_Í�&    ��&    �����U��WVS��l�u|d�    �D$h�E��$�C��D$�Ǎ�   �������<&  ���  �|$d�H�D$\�D$��<&  �T$`��������"  ���  ��x��T  �D$����  s6�$�T$�    ������T$�D$hd+    ��   ��l��[^_]Ít& �Ѝ��   �   �   ��ǆ�    ǆ�      ǆ       ǆ  �   ǆ  ��  ǆ  ��  ǆ     ǆ     ǆ  � @ǆ!  1  ǆ(  b   ������<$�L$�o���   ǃ(&      ����  �D$������$�   ������$�����$�    ����������������������&    �����U1ɺ   W��VS���/d�    �D$1��GTjj ������ƃ�����  �G    �$��   �   �����1ɺ   j j �D$������\$�P   �C    ��X��`  �}d���������XZ�����  ��Pt����P��  ����������D$d+    u
����[^_]������S�@@��uh    h    �����XZ�6��$&  ��u��h    Ph0   ����������B�����ǃ$&      1�[�hh  ������    �   �����1�1�h    ������        h�  ��������h(   h    h`   ��������b  ��Ph   Vhl  ��������$��t������������h(   Vh  ��������b  ��j�h   Vh  �������뻃�Ph   Vh�   �������뵃�h(   Vh�   ��������b  ǆ,&      ��Ph   Vh�  ��������a���h(   h    �����Y[�b  h(   h    h�  ��������b  hd   hd   hp  ���������  hd   hL  �����^_��  Q�L$�t$Phd   h�  ������k�T$����tD�}|���3  �=    ��  hd   h$  �����XZ��  hd   h�  �����XZ��  �=    ��  hd   h�  �����Y[��  �H   ������@   �������PhP   Uh�  ��������p  �����hH  �����X1��    P   �           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   �              �  �      �                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
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
     P       �  �  �     �    �  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           ��     �           ��    �  �  s           GNU  �       �               GNU p�7���J��=O"��&��        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 ��       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 �            �   U   ?     �            �           �      #     �   =          O   Y       �        4           B  8       	 T           ��d  �       y  �       �  �  %     �          �    �     s   d       	 �  �  R     �    �    �  �   @    	   �  0     +  H       	 6  @       	 A  P       	 L  �   -     e  �        �          �  �   
    	 �           ���  �   #     �  "       �  ;         G  	       P       )  _  7     @           H          P             \           j             s                          �             �             �  U   ?     �             �             �             �             �             	                                      >             F             \             m             u             �             �             �             �             �             �             �                          9             A             U             c             p             �           �     �    �             �             �  �  �     �             �              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q     �     �   B  �   U  �     �   i  �   L  �     �     �       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  �  c  �    �  S  �    �  N  �  N  �  N  �  K  �  Z    N          �  I  �  _  �  R  �    �      I  @  I  R    Z    x  R  �  e  �    �    �  N  �  V  �  \  �  Y  �  a    N  #  E  C  V  m  X  �  a  �  E  U  ]  o  d  �  P  �  a  �  a  �  S  �  N  �    �  E  �  D  �          W  &  [  I  T  V    g    p  ^  {  E  �  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  �     �   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     �     �   Q  �     �     �   Q  �     �     �     �   Q  �     �     �   Q  �     �     �     �     �   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    �  Q  �    �    �    �  Q  �    �    �    �    �    �  Q  �    �    �  M  �    �  M  �    �    �  Q  �       N          Q     `       �     �     �     �     �                                                    $              B     f     c          B     f  $                     �   O  �  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   �                    	   @       �  �              )             �  �                  %   	   @       l"  �               8             d  �                 4   	   @       �"  H              K             X	                    G   	   @       <%  (               Z             �	  �                   V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       �%  P               �      2       }  +                 �             �  �                 �             @  (                  �   	   @       �%  @               �             h                    �   	   @       &                 �             t                     �             t  (                  �             �  $                  �             �  0                  �                           @       �   	   @       4&                                                       0          9                              9                                   <  �     A         	              �                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ���ΰgj�D~��
�C��W���jG�4<�*1���CvV����ZhTpzƟ'�X����ο��s~o�'[��\�^=�$���0����a`>��
���<���*f���,�@��f)���8xJ�UuKwt���w�)�ifړ���dF�`�[˨�E����¤����!L����4r��i1g��)-��st0��n��zi=�?>��!��,8Zv=��t#Lb�6���*���E�:M�;��K��F�V�TO<'o�[֟��@�Σ]-3�p�tɯ�0`�ӤD�Nr(��KAԬJN��u���F>W�,��|�-O|U8S�	�l�#��M��xSJ�Zl��J��{xp�h��<ɫR��Z��6��gM����2쟉�ɔ+B��X*%�y�yc{Ĥ�F�3U8�q��K#V�1u3y�}�p���=��FaҴ�6&*�X��u��ft�Y�վG�`�}�����$W�Ŕ���zx�C����.H�\��sTZ�Ʀ]ڢɚk��U��Xΐ�`�T����w�u�c:�u���-��2w6���=�9U����v��s����.Q����w?�(��(��	:�+K���1ez[�L��E�pW�szl�r/���E'�� '
n�S>^�\X���O�>��:��C���P��ۏ(�(k"��%���$��q��I-�A7n���!�ۚ���@��*�����§0�dH����|�ѣ�q�/�ϔ��{�io����(��0:��0��+kF���kW��n꼸��z��)9��������7x��p��!Wpʹ��#D�w#,�� d�k31ȈKPN)�I����~<�)�@�O��u%�N�p��Ī�*,{�"W��r�+���/*JK�f�����ӛd;�	��dq+��S�m �x���;:�.]��U�(@�\vN���2�<>�l�U D }:��D��������LW�)�	�)<hpQ@c�@9P[Κ���OV�4�FoXJc�� �7m߉�U:�nh��A(�g�M7���p��z<]�d��9yNHv/��oD�0k�ؙF������LC������$�'�D�.�l����G�%}���" �/��G�x5yIg�u�R��}�䬿O����M����Á��M�W��!|1�&/�ԌׇPݙ���r@������N!l,�n�ߙ�t�n����@ʺ ���G�@P�]�����l&O'u����+���]�wB`�'�d����ع��h�b.�F�Tr� ��5�^�h�È��vS�Iϑ���<�w������'�;9����r��v��VƚaIYJ��~3��I���y#�A��j�D�PF��K����%��OS��FK
A�hV� y^�4��*��S��7���۷�ɭ���"��⚁�B��+���
��� *�[��[�b�x�����)v2zD�)9,�=��|�(�0�@8��<8]o�O�E��핊�,�#-_t��V��d�&�dA��.��7Rx)d��w��Qi�QR^}�j��������s�q=O� 4�ۘ���"[p��8�A���:UrYk����F
�����RQ�`���QhZfъ��}�n&�bL'�<�?�j�����H�QwfaD�j�x:���ZI�}�,�mCQXN,���ES��du]C0_�h8\vz�o�%M@ggj4���az�w5�ȉ��������dɵ�~>Lc�IT,�n�r�<?�GD�y�����;;O1I1�0�yf<��t�#�I��3���eH�Hi�h���t�5���:��DO���`7�`������W�\5�@лE���JZd�->d��0ǥ4ʮܼ���6�r�%)ee{�h�J��>k4"$�W�jd[/�N����LI&C73�;����a�/|������d���7mR��d�?��T'�s��ՑwR|��P郥�܇�*(�g�yl�D�
;t=0?���k���u��\Tv�!ʦ���"nP���.Z�����p��!�Ƙ��c�Ov�_������ҍ_=�����S#ӆd�&�y�l��φq�f���s��Q�(.���E���XV~�T�z�L2�����l�'f�I@h4M ��>Qs4��E�Tq�@�����.i�4�Bh�ąQr���k,W�v�0/F�2YD?��������1-��]�s�o8�^� MR�m��)�ߐ��9C]J^�E,�҇�C�]
��ë�������f�ӻ��1�0�:�.�;5�׫{������#�m�����4���6�𨬁c7��2R!����ˁ�ɚ���t�?����5���a�/�Qn�Fo]�<����7R�0��g�H�*x������vW�_��L֍J��:L�2LM�3�A/ �-λ����GC׋U=�5w��q�qk��c^q�9L��=��W�:S.R��d�����_茅 [��]���q�n!���� 	�>�Cۨ�$j��Y��Њ�:s+~�z��N0�Q� RJ�3�]а�ZRb�-v��j�q89�aMڄ��W�=o@EcJR����蛶Y��]��Dpx�Y��G�bJ�#Ű�{��"��#��.�Ò�u~b��ܤ
�ѻ�����C^��Ѧ)>�z`�bgYz�����Й鉅���:��$x����]��E���|Qp?�������� ɚ,���?�Z�d+���Tڜ�;�L/8�!�8�҂/���������l�_��Ԥ�@���uH�(�<R��u^�~�0� )���[i���	TsH�*f��͚u�a�oƸ�hG�^"�Q���%��%]z�`�ʐeF!=?��mv�¥�n+^oe�҉V��A� �p�n�H�>Lh}���͉8��������米8��X%�m8E���i�r���$Q`�P�+Ϻ} /󯎒%�[��0�`D��ק�KU�ɞ��~���7Jz��G�WP1 `yD_]�D���| `�E��ff�� �q6�y|t}�]�^'��j�Y�X�MdN@���2�H��Ϛ"�֜Ϧ�*{B�0�W;�C�؁P�K|�/��蟗F��Gy��t��(��� ��r3��j.'6?�@�+�����������Y��6���l����wy���4��N[�W��(/���ȼ����"��E=��udf:�w��ܪ"=�C}�(�W�=�4H9DJ:h1|ľl/�_�J�1}�8fLȕ���O��[*$eC^�Yuu&����]i-�WA/|Kw�s��T��넗>\H������Ŋ�N�dc��];A��a-��d)�Qlg�R�$$��uH�y媞�G*���BB��`pN烊e+�ƶ4*Ԙ4�L� �>�<�Jr�,]S��������%��+�)7~dX�������pe�Xacg���7K��վ�.�$K�mx��!�
�zЃTMzd֯u�`SͶG����b�7I#i�u�Ց �܉m��X��D��f!W�5*��2o/�v���V�J��Q���Q
���*�v��Ư����~� �z^ �a�Nl�9`��c�QN���I�7��X��o�e|F!��b@�s�a�Z��?�(R�w$5��m.�a���E���|s%��i�$��(�D\�u�$"N~�3��.��;o sr��̡1MIˡ.c�Wb.���N��I�I�8�����m3����8m'\�D_��}6U�w_��Jz)ոm1����o8ɹq1�@G{z-�\��ԋ���ԛF��7$e��7�~�¥���#��S���,O"_�M|�q1�zr��q���=k��hK�v&s*�8=��z�뎉���j����p�;�����a=1���`�b�AZ�~`�^���E5c-Ƞ&%Fœ�vn��P��ӯ�F��~/������K����t���M�B�[i*��(����p���p��
�@*��,�u���r�]P��|��Ӳߦ�@W��΀bL��@�ǟ�n 8�s�O��ٖx[}I3i:K����h�8JBI�W��+�ܣ��T%�y�% @}Xi�-Ê���ϱ8<�& eE�U�// SPDX-License-Identifier: GPL-2.0-only

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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ELF                      (W     4     ( ' &             �����S��h    j������    XY��t��P���[������t& [Í�&    ��&    �����UWVS�����   �F�~��$�    �s   ���Y  ������    �_   ��4&  ��~��������   �����1  ��������Q  ��&    ���t�����9�u�d  ���)  ��    ��������t����������  9�u庈   �����������������1  ������   ���������������F�����L  �������t&��L&  ��t�    ��   �����ǆL&      ��@&  ��D&  j h   �щ¸    ������F��������80  ZY��t$��<0  ���0  ���&    �E �������9�u�h  ������vd�t$hY   ����������������[^_]����������WVS�Ӊ�����������d�=    ���&    ��t4��t0��W�����1ҹ   ��t���   u!������у���u�[��^_Í�&    �v �   [^��_Ít& ������U�ŉ�W�zV�����S)���D����  d�    ��$   1��    �B@    ��  ��  �$   �������tR�ù   �@�k   ��������R  �   �C�P   ��f�C���������u�Eh� ��	u5�����������������$   d+    u-��  [^_]Í�&    �v ��   ����   ��y��   ������v �����UWV�   S��|�$d�    �D$x1���5�   ���3  �   ��  ��Y  ������Å���
  �$�3�S�T$�C�   �D$������D$����  �C��<  �D$���3h}   j$P������3W1�h�   �������   �-�   ǃT      ǃ|      ����@  ��   ��d  ��<  ��   ��P  ��   ��D  ��   ��H  ��   ��L  ��   ��h  ��   ��x  ��   ��l  ��   ��p  ��   ��t  ��   ���  ��   ���  ��   ���  ��   ǃ�  �  ���  ��   ǃ�      ���  �\$�o��    ���~g�ND��~`����  ����  �,   ����  ���k  ��퍅�  I���
���
�ND�    ��  ������t&�n(��u����    ���F$    �F@    ��uڋ\$�  ��4�    ���  ��   ��X  ��\  ��`  ��M��  �9  ��B��  �B  �   1Ҁ�Ht��  �   1Ҁ�I��  ��&    ����Y  ���Y  ����o  �F���������Ch����  ��X  ����  �Cf�x$p��  �t$��������Ch������  ���   ��tN�f��tF�k���}"�f����f��t-f9�u��P�f9U$u��P�f9U&�v  ���f��u؍t& ���뎍v ��
�Iֹ����
��� �  ��)ȋL$i� �  9�L��  �ND����  �ͅɍ��� I�����2����t& ��-u{�  ��L�-  �  ��B��  �>  ��-��  �   ��M�q  �w  �    1Ҁ�J������ �  1Ҁ�K������-��  1�1��m����t& ���6��  �   1��S�����Ntn��b�V  �   1��9�����K��  ��   1��$�����   1Ҁ�D������Gt���   ��&    �v ��i�5  ~E�   1Ҁ�m�������nud�  �   1҃�߀�C������   1�������&    �v ��d�w����N����   1Ҁ�g�������h��  �   1��|�����k�C�����   ��    ����
����F(������&    ���������F(������
�������L$��
�R�   ��	9�B��  ����1�������Ch�C�P"R�@ P�|$Wh�  ������C�P&R�@$PWh(  ������Ch�� �pWh�   �����Whd  �����Wh�  �����Wh  ������Ch��$�P�Sx�P�Sl���   �Cp��X  ��P  �C�   ���1  ��T  ��P  ��L&  �  ��H&  ��(  �������X  �   �  ��������   �   �.  ������   �C  ���   ������t$hX  jV�����Vjh 
 h^  ��������1  �� ���i  ���Q  ���1  ���Q  ��    �X�Hl�@������ �@��t�@�    9�u����  ��  ǃD&     �2   ���  ��ǃ      ǃ     ǃ�      ǃ�      ������D$���X  ��$  ���  �   �CD��   ���   ��  ���  ���   ��  ���  ���   ���   ���	Ћ��  ��   �a  ��  ���1  ��������1  �   �l  ��������1  �   �~  ��������1  ��  �   �������`&  ��D1  ǃ\&     ��('  ��l1  ��l1  ��H1  ��D1  ��X1  ��X1  ��\1  ��l1  ��p1  ��X1  �    ��  �$���������  �D$1ɺ�������������  j �    h�  j h   ��H&  ��L&  �����������  �<$�L$ �   ��������L$ �   ����f�L$ ��������G,�   �L$��P&  ��������D$<?w��   ����  ��H&  ��L&  �    �  �   �������T&  ���6  ��   ��X&  ��( ���%   �=   �  =   ���  R�t$h�  ��������  ��&    �  2 1Ҁ�d�������   ��g���(  �   1�������   1�������ktK~���l�(  �  �  @ 1҃�߀�C������  � 1�������H+�   1Ҁ�F�k�����Dum�  2 1��Z�����bt��(  ��Kt��(  ��    �   1��4����   1��(���� �  1Ҁ�k������mt߀�j�  �    1�������(  �(  �  ��   1�������Ch�t$h�  ������ChZY�������%�����   �D$�����D$xd+    u�D$��|[^_]���   ��   �������&    �t& �����S�Á�   d�    ��$�   1��\$X��4&  �  @ ��  ���4&  �    �k  �����������  ���4&  j h�   jh  S�����j h  S������� j jh  S������������������  j h�   jh  S�����j h  S������� j jh  S������C`����t1�Pd���t(�R��t!�    ����   �I����   ������v ���Y  �D$\    �D$`   ���D$d   �� ��Y  ��   ��t�D$d4  ���Y  ��A���Y  1���������Y  ���Y  ���Y  �����Y   ����� 	�u�   1�RP1��T$�����1��L$d�T$�����[X1���$�   d+    u0�Č   [Í�&    �D$d�  �o����v ������&���������������f�f�f�f�f������f��	w����`  Í�&    �v 1�Í�&    ��    �����UWV�Ɖ���S�˃��Nh�|��Nvf9���   ���    �,�8  �C�$�������������  �� t���� ��  �$��������?t6�C$   �Nw�   1������C(���� �S4�C01���[^_]Í�&    �C$   �Nw�   �����C(���Y  ���Y  ��f��������&    f������U��WV��S�ˍLm ���@h�    �C0    �D�L�ى$�C�ǉD$1����)���4����Fwf9�vW�$�    � �4�  �������������  �� t���� �  �D$��������+1��C$   ��[^_]Ít& ������������S�x�ы�% �����X&   Q� ��t(���X&  �
   ��&    �v ��9�t��u�[Ð����X&  �
   f���9�t��u�[Ð�����S�x�ы�% �����X&   Q� ��t(���X&  �
   ��&    �v ��9�t��u�[Ð����X&  �
   f���9�t��u�[Ð�������x���X&  % �����Q� � ��Í�&    �t& �������x���X&  % �����Q� � ��Í�&    �t& �����U��W�   V��S�Ã�D�4��  d�    �D$@1���   i�p  ���R  ��t]��  ���   ��   ��t>j �����P�������ZY��t'���   1��T$@d+    �B  ��D[^_]Í�    ������ڐ�sp�{���j �������YZ��t
ǀ�      �Cp����P��j �����YZ��t
ǀ�      �Cp����
P��j �����]Z��t�ǀ�      1��g�����    �����f�D$1��   f�|$�|$���W  �$��   �����������.  ��t�����$  ��D$������Clǃ�W  l  ǃ�W     ���W  �$ǃ�W    ǃ�W      �D$$��j �L$�T$�����[= ��������������������&    ��&    ������U�hW��V�Ɖ�S������F�V9�t/�X����&    �v �CL�X�9�t9��   u���������[^_]É�1��������[^_]������UWV��S���    @�B  ���W  �$    ��|W  �D$�F���T  �D$�$�
   �����|W  1��i�p  �|$�E�  �E0  ��   �E@  �E  �E   �E �  �W�0   �7�} ���R  1���cx18��ǃ����    �C i2c�C dri�Cver �C    �C    �C    �C    �C     �C$    �C(    �C,    ������>�������0�8  �W  ��4$��(��p  Wh�  P������|$�D$��������~���   �������<$�  ��X&   Q� � =/� �  ��X&  �  � �� � �
   �t& �����t��� � t��u�
   �t& ��X�A �������u�X&  �
   �� � �� � �v �����t%� � =�   t��u�
   ��X�A �������u�X&  �  � �� � �
   �v �����t��� � t��u�
   �t& ��X�A �������u�X&  �  � ���0� �
   �v ��
��  � t��uX&  �  � ���0� �
   ��&    f������t��  � t��uꋆX&  � �� P� �
   �t& ��
�� t��u�W  ���W  ���X&  % ��� Q� ��	�X&  ���
   ��    ��9�t��u��W  ���W  ���X&  % ��� Q� ��	�X&  ���
   ��&    �v ��9�t��u�X&  � �� Q� �
   �t& ���
�� t��u�W  ���W  ���X&  % ��� Q� ��	�X&  ���
   ��    ��9�t��u��W  ���W  ���X&  % ��� Q� ��	�X&  ���
   f���9�t��u�F�~�X�9�t2��    ���   @t�Cd� ��t�H��t	1҉�������CL�X�9�uԍ��R  ��������Å���   ����[^_]Ív �$   �����t& ��X&  �   ��� �
   ��&    �v �����t��  t��uꋆX&  �
   �  ��$� �t& �����t%  =   �������u��{����t& ���U  ������Å��V�����������J�����&    �����VS���    @�a  ��X&  �� P� ��X&  ���� P� �
   ��&    �v �
�29�t��u�X&  �� Q� ��X&  ���� Q� �
   �v �
�29�t��u󍃜R  �������U  [^�����f�f�f�f�f�������V��S�    �v  ��X&  �
   �  �� � �t& �����t
%  ��t��u苎X&  �
   �  �� �� f������t
%  ��t��u�[^Ít& ������W���   VS��X&  �Ӎ���� �
   ��    �:�2��t��u�X&  �   �� �� �
   �t& �>����t��  t��u����X&  ҃��� �� �
   ����    ��>9�t��u����X&  ҁ�$I����� �
   ��7����&    f���>9�t��u�X&  �   ���� �
   �t& ��:�2��t��u�X&  �   ���� �
   �t& �:�2��t��u�X&  �   ���� �
   �t& �:�2��t��u�X&  ���   ������� �
   ��    �2�:9�t��u����X&  ҁ�;�� ���� �
   ��D�0 ��&    f���>9�t��u�X&  �   ���� �
   �t& ��:�2��t��u�X&  �   ��@�� �
   �t& �:�2��t��u�X&  ���+ ��D�� �
   �t& �:�2����+ t��uX&  �   ��H�� �
   ��:�2��t��u�X&   � ����   �
   �  ���t& ��0��� t��u�X&  � ����� �
   �t& �2���t��u�X&  � ����� �
   �t& �2���t��u�X&  ������ �
   �t& �2���  t��uX&  �&����� � �
   ��2���&�  t��uX&  �
   �1����$� ��&    f���
��1  t��u�[^_Ív �
   �  ��    �0����t��  ��t��u狁X&  �
   �  ��� ��&    f��3����t%  ���������u��������&    ��&    ������V1Ҿ   S�ø
   �������X&  �� � �
   �t& ��1����t��  t��u�1Ҹ
   ������Ch���   ��X&  �� � �
   �t& �
�29�t��u�1Ҹ
   ������Ch���   ��X&  �� � �
   �t& ��
�29�t��u�Ch���   ��X&  �� � �
   ��
�29�t��u�Ch���   ��X&  �� � �
   ��&    f��
�29�t��u�1Ҹ
   ������Ch���   ��X&  ��H � �
   �t& ��
�29�t��u�Ch���   ��X&  ��T � �
   ��
�29�t��u�1Ҹ
   �   �������X&  �� � �
   ��1����t��  t��u�1Ҹ
   �   �������X&  �� � �
   ��&    ��2�
��t��u�X&  �
   �  ��$ � �t& �1����t
%  ��t��u苃X&  �H   ��� � �
   ��&    �v �2�
��Ht��u�X&  �   ���� �
   �t& �2�
��   t��uX&  �  ��� �
   ��2�
��  t��uX&  �  ��$� �
   ��&    f��2�
��  t��uX&  �  ��� �
   ��&    f��2�
��  t��uX&  �  ��� �
   ��&    f��2�
��  t��uX&  �  ��� �
   ��&    f��2�
��  t��uX&  �  ��(� �
   ��&    f��2�
��  t��uX&  �  ��0� �
   ��&    f��2�
��  t��uX&  �  ��4� �
   ��&    f��2�
��  t��uX&  �  ��,� �
   ��&    f��2�
��  t��uX&  �
   �  ��8� ��&    f���
��  t��u�[^Í�&    �t& ������UW��VS�   ��P�D$d�    �D$L����X&  �$��L � �
   ��
��t��u�D$�
   �  ��X&  �$�� � �����t
%  ��t��u�1Ҹ   ������D$��X&  �$ � � ���  �|$�   �����������   ������G�  ��T&  �D$0    �$���   �D$0������ǅ���  �D$0��p�$���s  �D$����������D$0�|$��   �D$� �$9�s9�t& 9�}1��
   �t& ���9�t��u�9���  ������;,$r�;<$r��D$��4&  ���  �D$������t$�  � ��������<$����  ��������F��T&  �3  �D$0    �$���   �D$0�|$ ���������  �T$0�z�2���_�|$�t$�\$$�_�\$*�_0�\$+����  �D$�D$   �D$    ���D$,��D$�:���ƉD$��9���   �D$�t$���D$�x�|$�x��8Segm�<$�t$u��xHedru��    �
  �|$9|$��   �$��t~�D$ �t$1���D$�D$�����8�D$��   �����9$vJ9�}F�v �D$�\$�(�D$�4(�������
   ���9�t��u�9��t  ��9,$v9�|�;<$r��T$0�����D$0�8�t$��4&  ���5  ������t$1҉���������D$Ld+    �=  ��P��[^_]Ë�X&  �   �$�� � �
   ��
���t��  t��u�t$�2   ��v 1Ҹ
   ���������   ��X&   � � �u�1Ҹ�   ������t$��   �������j jh   �D$@PV������ǃ����E�����X&  �
   �  �� �����t��  ��   t9��u�1�����1Ҹ�   ������D$��X&  �$ � � ��o����  1�������t$��4&  ��t+������D$�  � ���������������t& ��|$�v����  f�f�f�f�f������WVS���   �Ã�p��X  ��������� X  ���W  Sh�  ������������1���[^_Í�&    f������VS�Ë@`���   ��tJ�    �-  �ف��X  t3��4Y  � ��t'�X��t �    �   ��t�p��t��[��^�����[^Ít& ��ȉ�[^�������&    ��    �����WVS���   �Ã�p��X  ��������� X  ���W  Sh�  ������������1���[^_Í�&    f������U��WVS�����W  �� X  �����$�ӉD$��X&  �����t$�֍��� ���
   ��&    �����t��t��u�D$��X&  �t$�
   	Á� �� �Ɛ�����t1���t��u싅X&  �$�t$���� f1Ҹ
   ������t��t��u�|$��X&  �
   	����� ��&    �����t1���t��u��[^_]Ít& �����UWVS���   �Ch���   ��t5��X  ���   �������3� X  ��!�3� X  �� X  �����������[1�^_]Í�&    ������UWVS���   �Vh���Y  ���   �I���   f�|�TE��   ��t3��X  �É�������� X  1�!�3� X  �� X  ���<����������[1�^_]Í�&    ��&    �����UW��V��S���˃�	؋L$ �T$$�$�L$�T$u��[^_]Ít& ���X  �������3� X  ����#4$3� X  �� X  ������������D$�����������������3� X  ����#$3� X  �� X  ������������D$�������[^_]�������&    �����S���   �Ch��t��t_��t31�[Í�&    ���   ���   1����   �������1�Y[[Í�&    ���   ���   ���   ���   �������X1�Z[Ív ���   Gu��H|j�   ��j��1�����XZ뀍v �����UWVS�����   ��t��t|�������tc����[^_]Ð�Fh���   ���   1��҉$tݍ�X  ��������$3� X  ��!�3� X  �� X  �M��������������[^_]Ít& �Fh���   릍t& ��Fh���   떍�&    ��    �����UW��X  VS�É�������Ch���   ���W  ���   �� X  ���   Gu �H|�   ��	ꉓ�W  �@|�Ɖ� X  ��t�    u������[��^_]�������X&  ���� ��X&  �� �� ��X&  ���� ��X&  �� � RQVP�CPh  �������릍�&    ��    �����UWV��S�Ã� t��@ta[�����^_]Í�&    ��X  �F  ��  ��������C���   UP�Gph_  j P��������   �C����[^_]�������&    ���X  �O  �@  뮍�    �������ugVS� ��Ch���   GuM�    �B  �ށ��X  tV��<Y  � ��tC�H��t<�    �   ��t%�X��t����[^������v 1�[^Ív 1�Ít& ���[^�����������������ې�����UW�xV��S�É���������F�$������9�tK�nf���P��Q�
�V�F�0�P��@     �@$    �@    �@    �@    ��E �9�u�����[�C    �C    ������$��[^_]������������P��tS1Ґ�XӃ��ɉ9Pw�[Ív Í�&    �����V�pS�X9�t0��&    �C��t"1���&    �K����ʉ9Cw�9�u�[^Í�&    �t& ������� �@�@    �@    Í�&    �����UWV��S���  9�t#�B     �B$    �B    �B    �B    ���  9�tM�ωˍk��������D$��t'��p��~�3�F +F$C��C���������[^_]ËC�s�>�F�0�֐�A��>~��À  ��륍�&    ��&    �����W�zV��S��������9�t6�3�F��B��F$�6�vC+F �C�F    ��K���������[^_Ð��1��������[^_Ð�����UWVS�Í��  ��$�L$�$d�    �D$ ��8  �D$�D$�D$�D$����������  ���  �D$�M 9���  �D$�U�t$����D$;$t4�E���E���  ��9��W  ��ω�;L$�q  �U��;$űU �E�t$�B��} �}����  �����������   �D$�u�E    �E �E�$�ǋE9�t	9u��   ��t  ;4$t[�l$���|$�8�v )��V�N�F��8  �@��X  ��t  �   ������6XZ94$t�F    9�v���1��f��l$��@  t��M�T$�L$���  �2��9�u
�/��    �΋J�N�1����B��j ����������^�t$9�u׋D$ d+    u]��$��[^_]ËD$�V�F    �F��8  �F�q��X  ����t  �ƈ   �������Y^�S����t$��1�������O����������&    ������V���  S��Í��  �d������  ��[^�U����t& ������UW���  V�����  S��h  ���<$�'������  ���������  ���������  �D$��������  9�tY�t& �C�{9�t;��    �C�P��Q�
��l  ��l  �(�P��@    �@    �C9�uˋ�L  �C�9$u��D$��[^_]�������&    �t& �����UWVS�Ã���8  ��L  �D$���  �D$����������  �D$�Ƌ�x  �T$��h  �$�T$���/  9$�&  �v �D$�t$�p��x  ���m  ���$    �4��  �D$��&    �v ��h  ;D$��   ��h  ��P�Q�
�L$�Q�A�L$�P�����   �L2��P�
   ��&    f���)9�t��u󋏄   ��t  �
   ��)9�t��u�$��x  ���$9��k����t$Ή�9�u[��|  1���t  ��uaj �T$�؋L$�����Z�l$�T$��������D$��t��x  9$�������[^_]Ë$�t$�j�L$�؋T$������t$X뵋$�t$��0�  ���   �
   �D���&    ��9��o�����u��e����$    �l$�C���f������UWVS����X  ��t  ��8  �Z�p�B�$�ƈ   9�t �t& �C�SUW�щ�������XZ;$u��[^_]Ít& �����WV��S�    ��  �����  ��������&    �v ������ډ��������uh  ��h  9�t^��    ��h  ��C�B��S��K�[��8  �@j ��X  ��t  �   ������C��������������h  ��9�u�[^_Í�&    �t& ��������p  ���  U��WVS����8  �D$�    �   �D$���   ���  ��=   ��  ��L  ���  1��D$�   ��&    ��   @j ���  �����t$���t$����    ������щ����������8  �F���V�Y��X  �щ���t  �È   ���������l  ��h  ��l  ��F�0��p  ZY9��8  �   �(   ��-  ������Å��>  �   �   ��-  ������ƅ��(  ��t  ��,  ������F����   �C��ډC�C��L  �[�C��j �L$�������X  �V�6�v�D$��t  �T$�D$��8  �H�ЉL$���   �����Z�$���L$������=     ������������������   �    ��te�؉$������$RPhd  ���������������������1�Ít& �����������������p  9���  �|$��   1���[^_]Ë��   듋�p  �ԉ��������p  ��f�f�f�f�������V��S���   �Ӌ�8  ��  ��  ��҉���u�    ��    �Ѓ���  �v�v ���Ѓ���  �w��F4   1�[�F8   ^Í�&    �t& �����S���   �Z(�B   ���  ���  �  �B(�J,�[Í�&    ��&    ������U���  W���  VS�Ã���    ����������ƅ�t?��8  ��D  ��$t?��T  ��u4��T  �t*j ���������Y9�t8���  ��[^_]Í�&    �v j �������������  ^��>�9�t��ϐ�V�F9�t�F;Ft^���������F��|  P��x  �L$���   ��°  +�T&  P��D  jh  Q��������  �� ��>�����a����v �P�H��8  �@��X  ��t  �   �����XZ낍�&    ������P���t�P̃�>-�  ����f�Í�&    ��&    ������S1ɉ�j �������������C    X[Í�&    ��&    �����UWVS�����   �D$��8  ���  �L$��  ��  �D$�Bl    �$��/�>  ���5  �$�BT�߉�ɉJ���  ;z�  �$�z�B�D$�B�D$���Y  ���Y  �Jd�Zh���  =HM12�4  i$�  ���  1�j �D$�T$������D$������T$�BT�B    Y��t�B9BL��   �Z��u�B   �J��u_�$�z�B�D$�B�D$���Y  ���Y  �zd�jh���  =HM12��   i$�  ���  �D$1ɉ$������$������   �B   1ۃ���[^_]Í�&    ;JL�����������ߋ$;J������\$;Z������\$;��  ������Jh�Zd3��Y  3��Y  	�������
�����&    �i$p  �������f�j �D$1�������D$������T$�B    X�X�����&    f�i$p  ������f������W��VS�Ë�8  ���  ���8  �    �a  ��<&  ����   ���U  ��@  ����   ��D  jh  V���������@  ��t����  ��t���8&  ���<&  ���T  ���D  jh  V�������D  jh  @V�����ǃD  �������U  ���<&  �� ��M��  1��������X&  �
   �   ��L � �t& �2�
��t��u�j ��\  �   �   �����X1�[^_Ít& 1�����P��D  jh  V������������t& ���4  ��t���  ���������@  �����u���x  ���������  ��&    ��    �����UWVS�Ã�p��8  d�    �D$l1����  ����  �    �d  ��@  ����  �P  ��t���b  �   ���T  h   jh  @�l$ UV������D$(��D  WPjh  V�������(����  ��<&  ����  ��@  ���   ��T&  ����H  ��@  )�)�QP��D  jh  V��������������@  �����  ���  ǃx     ��t  ��|  ����������  ��t���  ��>���D�����D  jh  V����������P  ��t���8&  ���<&  �T$ld+    �  ��p[^_]Í�&    �v ����  ���_  �   �����t& ����G  ���0  ���0  1�ǆ8'      ǆ@1      ���ǆ�0      )�ǆ<1      �����   �M�����&    �v ǃx     ��8  ��\&  �3  �xu�%�
  @&  ��|  �������&    �j ��D  jh  V�����jj��D  jh  V�������,j j��D  jh  V�����jj��D  jh  V�������8&  ��0���2  h9  h8  ��D  jh  V��������Fx�(  ���  �   ���k  P��D  jh  UV��������  ��  ��������  �����~  ���\������  UYVY��D  ��  j Pjh#  V�������<&  �����3�����    ��  �   ��������4&  X&  �   ��L � �
   ��&    �v �*�
���������u��������&    f���4  ��t���  ���6�����@  u��x  ����  �������c�����    ���  HM12��8  ��  t'i��  1҉�|  ��t  �������x  �����f�i�p  ���Ս�    �xu�%�C  `�  ��|  �����t& ��    �   ǆ�1      ǆ�1      ��   ���  ǆ�1      ǆ�1      �����ǆ@&      ��$  �{�����&    ��   �i�����    j��D  jh  V������������v ��8  �Au�ʉL$<�$ɈD$����<���,'  �у����
�D$�$�$�T$<���0'  ��-  ��\&  ��4'  �A`��  ��tR�Pd�R��tH�R�$��t>�T$�   ��`&  ����  �   �I����  ������T$�Bu�D$�t& �D$��D  �D$���$��<�L$҉L$,��  ��  <��T$0��  �L$�D$4�D$8   �L$<�$�L$@�    ��  �D$,�   �  P�D$��������  X�   ����������  ��t���  ���}���1����  �n������  �   ��x  ��E��S�����&    ���4&  � �t������  ���������P��D  jh  V��������G����t& ��    �Q�����  ��    j�D������t8�Pd�R��t.�R�$��t$�ʋ   ��`&  ��tR�   �I��tE������D$��D  �D$` ` �$Аp0��,'  �T$�T$,�������	¸�  �T$0�����$붋$������D$�@u�D$�&����    ��  ���U  ��@  ����   j��D  jh  V����������T  ���D  jh  V�������D  jh  @V�����ǃD  �������U  ���<&  �� ����������4&  ��X&  �
   �   ��L � �t& ��
���������u�������D  jh  V��������[����������  ��&    �t& ������U���  WV�ƍ��  S1ۃ��D$��L  �$�D$�5���tk���  ��t����   ����������������tu�����  ��uǋ��  ��t�$���  ��up�������ǆ�      ������Í�&    ���x  ��t�ǆx      ���  ��t��D$�����뒍�&    ��[^_]Í�&    ��D$������Z���f�����������  끐�����U���  ��  WV�ƍ@S1ۃ��$�  �D$    �D$�>�Fx�|$�=    ǅ�      �|$ǅ4      ��8  ��@  ��<  ���m  ����  �$� �D$��t���d  ���  �}��1��E     ǅ�      ���)����  ��󫉵8  ��   ǅ4      �D$��@  ��<  ��X  �$ǅD  �����@��H  ���d  ��p  ����  ��h  ��t  � ��l  ��\  ��|  ��  ǅx     ��������  ǅP  ����ǅ�      ��������  ǅ�      ��������  ǅ�      ��������  ǅ�      ��������  �    1�ǅ�  ����� ���  ���  ǅ�      � ���  ���  j j �����ǅ�      XZ���m  ���D  �|$�����   ���  ��<  �t$h�  j P������D$ D$$1�ǅ�   `  ���f���  �F���  ���� ǅ      �Nh�yu���Y  ���Y  ��  ��(  ��0  �艕  ��������������x[�C��tA���$���  ���  ���  �\$���T����v �   �������������ǅ�x��1�[��^_]Ít& �ǉ�1����������[^_]Ív �P�T��������������   �   �   �q�����&    �v �Fh�@��   ǆ�      �����t& ��(  �Fǆd      R��d  U���   ���  jpǆh     jjR��  �������  ǆ\  HM12ǆd  �  ��i�p  ����`  �)����t& ��,   �  ��  ��������  �������2  ����������&    f������U��W1�V�   S���  �@���D$    �D$�,$�   ��    ���  ��tn��H  ���  ���   ����   ���   ��(  ����0��j �����Y��X����  ��  ��u���   ���}  ����  ����   ������   �������  �<��  �F����^����$���  ��t����������S  ���  ��th��H  ���  ���   �$��H  ���9��������  ��  ���!����t& �T$�,$��t�   ��������D$��[^_]Ít& ��   �I����  ��&    f������VS���  �Å�tM���  ���  Ѓ�4���  ��~)��L  ���  ���������tj ���  �������X[^Ív ���  ��t
���  ��u����  u݋��  ��x  ��u��ː�����W��V�   S��<!  �)��&    f�i��  ����  �uN�����  ���tW���  ��u֋�4  ��t
���  ��u�@  uˋ�8  ��x  ��u�빍�&    ��1҃����  ��������u�[^_Ít& ��������H  �(  �
��t���   ���u���  9�u�������Í�&    ��    �����VS���t^���  ,&  ��&    f���9�D  u6���  ��t��[^Í�&    ��4  ��t
���  ��u���@  t�v ���  9�u�1ۉ�[^Í�&    ��8  ��x  ��u���f�f�f������VS�ËpX����  i��  �Ѝ��  ��*sE���  �[P9�t)�    ������   �����   ���  ���i  1�[^Í�&    f��SP���  ��u䋖<'  ��L  ��t*��\&  t!��l  ���T  ���T   1�[^Í�    ���  ��uۋ��  ��t
���  ��uǃ��  u����  ��x  ���o���먍v ����  ������Z�����    �����VS�Ë�8  ǀP  ������@  ��t��to���T  r�    ��  [^Ít& ���������@  ��u荆L  ����  r>��l  ����  sƃ��  �u�����  �[^������t& ���T  � t�띍t& ����  ������볍�&    f������UWV��S��\d�    �D$X���  ������   �l$�$1����F�~9�tE�X���    �Cd�@��t�H��t	���������CL�X�9�u܃|$<u�D$8�   D$�$��$  ;$t9�    ��  ���  �G�@������$��������ËG�@�������u�D$Xd+    u��\[^_]Ív �    t���  �������&    ��    �����UWVS���HT�XXi��  �$�<+���  ����  ���������  ���  ��+�  ����   ��.��   ��l  ��+�  �D$��L  �D$���  ��u#���  � t����  ��   ���  � ul�����������   ��N���4&  s8i<$�  �D$��;�  jh  S������T$������[^_]Ív ��N����[^_]Í�&    f�����  r��D$���������   �    �k����O  f��D$�������u�    �@����%  �    �d  ����  �t& �    �  i<$�  ��;�  ��u)���  �uQ���  �t���  ��tQ��&    �v ��&�������������(����v �����������    ����������D$1����������  �뚋D$1����������  ���    �:  ����  ��B�����&    �v �����UWVS�Á�   ���   ��$�   �T$,�L$0�D$`d�    ��$�   1��FX�D$8�FT�D$H�    %   =   �E  �D$8��(  ���������������ŉD$L���������udi|$H�  �[ �D$8��   ��8�  �|$d�\$ u
�~T��  iT$H�  T$8���  �L$0�D$D�    ��u3%   =   ��  ��$�   d+    ��  �D$L�Ĉ   [^_]Ë\$D��8&  ��u���  ���  iD$H�  �\$8�D$\    ���  ����  �\$8�t$d�D$$    iT$H�  ��3�  �|$@��3D  �|$��3�  ډ|$X��3  ��3l  �t$l�T$�|$h�|$D�t& �\$|��l  �D$p    �\$|���t$@��$�   �\$Xd�    �D$x    �T$t�7�t& �T$p�������d�    ���   ��  � ���  �D$���  ��uD�    ��  )��   ��   ���  � t���  ����   ��d1  9�h1  �  �T$�����������  ��T  ���  �D$ ���0  �ع   �T$p��������  ���:���������0�����&    ��    ��   ��������a�����&    f����  ��  �t$�L$��<  �\$�Ή��8�t& ���  ���$�������j �L$�T$������    ��    �����X�ډ��������u��t$�\$��d1  9�h1  ������v ����D1  �  ��X1  �D$�u�D$0+D$$�D$����  �D$D�$    D1  �D$<;t$u!��  ��&    �v �E�0�u;t$�.  �V�F9�s�$�L$$)Ћ|$�L$,�L$���  �|$�L$()ߋ^9�G���<'  ډ\$�T$1ۅ���   �D$���  ����   ��\&  ��   ��l1  9���   ��@&  �BɃ�&��F�L$7�:9���   �|$T�ˋ|$�l$P�Q�t& ��N�j�@�9�s9�z u3�zu-�L$78Ju#�L$(��@&  ����  �J��������  �9�v��1�)��������u���F�l$P1ۉD$�|$T��&    ����   �D$�   ��������T$�D$�����������   �D$~���  ��u�L$(��l1  9�t1���1  ��1  �U$<$�$��U$9D$�|  ���t  �E�X9X�U  ;U �/����$�E ;l$<��   �t& 9E$�  j�L$ ��D$D�����^�D$���  ���P  �$\$$�D$$9D$0��  �\$\����  �D$���  �����v �    �  �$�����E �����;l$<�x���9E$u��t$D��@1  ����Ǆ��0      1҉�@1  �E ��1  ��1  �U���f��u�u�����t& ��J�̓���t1ɀ|
��5�����9�u��J��Ā�D�����J���������|* �����|* ������|*������\$(�׋l$P+|$ǃ@&      ��@1  �����@0  ��|1  ����0  ǃ�1      ���1  ǃ`1      ��d1  ǃh1      �F�   F�D$������&    �v �J��������  ����W����J	�����0�G����D$(�D$7�ǀ@&     �B	�~�)�����&    f��E 1ۉE$;l$<�s���j �L$l��D$D������    �L$p�    �����_������v �$������&    ��$������&    �����p �]���\$���V����D$���  ����   �E9�t�E;E�#  ��������%����t& ��    �y  �|$$ �  �    �\$$%   �\$L=   �c  �\$L���H����؋\$`�S�7�����    �    ���  �L$$��u��D$L�����������    �$�����������������\$D��\&  ���ۉ\$\�;����J	����� ������d����    �\$L%   =   ������  �t& ��t$ ��8�  j �t$h�L$<�T$8������D$X���w�������������|$$ �    ������D$8�\$d���  �� �D$@���U  �������    ������    �T$$�    ��������D$L����������D$L�����������r  ������  �v �����U��WVS���$����  �B�<$���   �^T�~Xi��  ����  ���T$���T$u���  ��tti��  ���  ��   �    %   =   ��  ���L$�   ������L$����   ��L  � ����   �T$������E�����[^_]Í�    �At���(  �L$�ЉT$�������������D$�D$������L$�ɋL$��   �     �B����  ��&    f��~T�>����$���  ������É�������������4 ��un�D$tg���W�����&    ����A[��^_]Ív 1��������)���  ��t��t�$����������1��������&    f�������b�����    	��������&    f��    ��  �   �������&    f������UWV��S���   �OT�_Xi��  ڋ��  �������  ui��ud��u
���  �t���  ��[^_]�������&    ���(  ���������������ǉ��������u�     t���  ��    �  [�����^_]Í�&    f������U�h�WVS�É�s�������ǋC�9�tE��&    �C��P���Q�
�   �   �@�   �@�"  �@�   j ������C�Z9�u�[��^��_]�������&    ��    �����U��WVS�Ã��pT�xXi��  ���  �$�    �  i��  ����  �t9�    �b  �CT��t7���~   i��  ����  � tlǆ�  ������tt��[^_]Í�    ���  �t~���  ��us�    �(  1ҍ�l  �L$��������  �L$�uQ�CT냍�&    �v �$��L$������L$��u����  ��`��� �$��[^_]������v ���  �t��    �E  ��L  1҉L$������CT�L$�����t& �����S�Ë�8&  ��u�    ��  [Í�&    ������������  jPjh  S��������ōv �����VS�Ë�8&  ��u�    ��  [^Ít& ���������ƃ����  1Ҹd   �����jVjh  S�����j Vjh  S�������(먍t& ������U��WVS�����   �CT�sX�$�    ��  ��(  ��������{TtC��������������i$�  �KP9��  ��   ��������������1���[^_]Í�    ���   �������t�����������4&  ߋF�V�h�9�tC�\$�|$�׍v �Ed�@��t�X��t���Y  ���Y  ��������EL�h�9�uҋ|$�\$���������8&  ��!��������2����t& �1҉�������D���f����  ������ŋ��  �������	�PiD$�  ���  jh  V�������렐�����UWVS�Ã��������(  �ŋ�8  ��(  �D$�������������$���8  ��8  �     �  �   �\   ��  ������Ņ���   ��������}X��@  �ET��D&  �P��D&  �EP�艫�   ������}Tt�D$������$��[^_]Ð���   �������t܋�4&  � u
��8&  ��R���4&   ���w������G�X�9�t#��Cd�@��t�P��t��������CL�X�9�uމ��������������v�������������������������$�����U����$�����    �A����i  ������V��S�JX�ZT��  �p��  �@   �p�@   �zTt1��@MPEG�@   �P1�[^Ði��  ً�|  �P���  �P���  �P1�[^Í�&    �������BX�A����A�   �A�  �AGREY��0'  �Q��4'  �Q��,'  �A$    �A �A1��A(    �A,    Í�    �����W�AV��  S�ӋzX�Q9�O־   9�Lփ{Tu���    �t�Qۃ㠁�@  9�O�[9�L�^_�A1�Í�    �����UW��VS�rX�˃�\&  t��8&  ����   �F`����   �Pd�R����   �J����   �-   �S����   �m��t}�������ug��('  1�f�
ǆ\&     �WX�C����C�   �C�  �CGREY��0'  �K��4'  �K��,'  �C$    �S �S�C(    �C,    [^_]Í�&    ������끍�&    f�[�����^_]Í�    [�����^_]ø������&    ��&    ������S�ȉӋT$�KX��u*�yt���Ճ�6��yt���σ�;�B1�[Í�&    f������[Í�&    f�������9�RXu;�A����w0�A    �A    �A�  �zt���@  �A1�Í�&    ������f��������ȃ�wr���э��  ���  ��J�H�J�H�J�H�J�H�J�H�J�H�J�H�J �H �J$�H$�J(�H(�J,�H,�J0�H0�J4�H4�J8�R<�H8�P<1��f������Í�&    �v ������BX���Y  �1�Í�&    ��    �����WVS��rX��u9�F���ύX�9�t$�Cd�@��t�H��t	����������CL�X�9�u�1�[^_ø��������&    ��&    �������BX���Y  ���Y  �Q�1�Ít& �����WVS��rX��u9�F���ύX�9�t$�Cd�@��t�H��t	����������CL�X�9�u�1�[^_ø��������&    ��&    ������VS�|$fd@u]�D$� ��uM�rX�F���X�9�t4�t& ����   @t�Cd� ��t�H��t�   ��������CL�X�9�u�1�[^Ít& �u��������썴&    �t& �����S�ˋJX��    ����tMw��t/����  �c1�[Í�&    ��uC����  �C    1�[Å���  �C    1���t& �����  �C    1��Ӎt& �����  �����[Ð������BX��Hw9�s���Y  �����1�ø����Í�&    �v �����WVS�ZX�Sh�Cv�|�9���   �    �΋��Y  ��9�t~����  ���Y  �D�1҉��Y  �?���� u���Y  ���Y  ���  ���  ��x  ���  ���  ��t  ����������������������������1�[^_Í�&    ���  [^_Ít& ������ލ�&    f������UWVS��zX��uX����������    �(  �G�o�X�9�t'�v �Cd�@��t�H��t	���������CL�X�9�u܉������1�[^_]ø������&    ��&    ������UWVS�D$�L$�rX���� �  ���Y  ���Y  1�1�	���   ��4&  �� ��   ��8&  ����   ������Y  ��  ���Y  �Vt���Fu������~t��  �f��  ��f��@�~t�f��  ������~t҉�,'  �����~t���0'  ���>  ��4'  �    �A  �F�~�X�9�t0f��Cd�@��t�h��t���Y  ���Y  ��������CL�X�9�u�1�[^_]ø������������퍴&    ��    ���������@X�������&    ��&    ������BX���Y  ����������&    ����������@X�������&    ��&    �����WVS��rX��ul�F���ύX�9�t$�Cd�@��t�H��t	����������CL�X�9�u܃$�Gt�   �  �����1�[^_Í�    �   ��  �����1�[^_ø�����ڍ�&    �t& ������WV��S���zX��    ������   w*����   ���]  �Q�����Q�����1���[^_Ð��u[���x  �A    ��8&  ����   ���4&  ��   �����������)  Pjh  W������������1���뛍v ����  �������[^_Ít& ����  �A    ����[^_������t& ����  �A    ��8&  ��tH���4&  r4��������Ã���C  �������Sjh  W�����1��������v ��1�[^_ø�����������&    ��    �����WVS���   �JT�zXi��  ���  ���Ã����É���u#��u���  ���    [E�^_������t& �[�����^_Í�&    ��&    ������WVS���   �KT�{Xi��  ���  ��������uG��uB�    ��t  ���  �����iST�  �KX1����  u���  [^_�������    [�����^_Í�&    ��&    ������W��VS���   �YT�yXi��  ���  ��t��u+�H 1���   ��ui��  ���  [^_�������    [�����^_Í�&    ��&    ������V��S���   �XX�@Ti��  ���  ��t��u$1���ui��  ���  ��[^�������&    �����[^Í�&    ������V��S���   �XX�@Ti��  ���  ��t��u$1���ui��  ���  ��[^�������&    �����[^Í�&    ������V��S���   �XX�@Ti��  ���  ��t��u$1���ui��  ���  ��[^�������&    �����[^Í�&    ������UWVS�ˋrX�   �  ��������~l�    ������������  �k���� t���� ��  ����������V���   ��u���   P�C0h  j P������Fx��   ��CT[1�^_]Ð�����S�\$��tR�É�f%Dt<��~S��ug��b��uk��us��1����&    f����� t(��s�   ��[Í�&    �v ��f% t�J���v1�[Ít& ��u��   [Ít& ��u��~�1��܃�u���[f% @ø   [Í�    �����U��WVS�� �BX���  ��L  �D$�\$���W  �\$���  �@���  �$)к   ��N����  ������E     �E    �E    �⍃�  �E    �D$���  �E    ���D$�U��    �T$�D$������D$���e  �\$���  ��  �@�D$����  �D$���D$9D$u$�  ��D$�@� �\$�D$�C9D$�4  �D$�X�x9�sԋM �t$)��$    �V�����Dڃ�wl�v��&    �J�Z�����$�X1ۉ�r��J���	މH�p�J�H�J��@    ���@    ��`  �H�E �H�ȉM ���D��v��?v��D$�X�4$�G����C�����|$�_�|$G$�G$�} ?��wc�G�W �X9XrU9��������&    �j �L$ �T$�D$������} ?X������L$�    �    ��   �����1��� [^_]ËK$�D$;H s�j�L$�D$�����Z뼋D$�X�\$�X�d����\$�C �C$�|������  ��t���  ��������\$��������  u����  ��x  ���k����s����D$�H$�D$;H �%����n���������BX�A����A�   �A�  �AGREY��0'  �Q��4'  �Q��,'  �A$    �A �A1��A(    �A,    Í�    �����UWVS���zX�t���������D  �yd��   �$    1�f���1���v �<$u��)f�tC��f	1��t(�Wt��t��؍h���w��t�1�f�TC����u؃�0�<$t�$   뤍v �Ah    1��Al    �Ap    ��[^_]ø������f������U1�W�y
V�����S�Y�jX�Q�Al    �Ap    �Ah 	  �A    ��)��C\    ��`���1�f�F�E`��tm�Hd�I��tc�I��t\�=   ��t���t���������t& ����������u2�V61���&    �f��9�u��Vf�v f��9�u�f�F[��^_]ù������t& �����UWV��S���BX�Y�Ah 	  �Al    �$�Ap    f����   �$1�1��ht��&    �L~U��1��)���Zf�D~��	Ã�u�1҉4$�~0�֍�&    �LwU��   �����Zf�Dw��	Ã�uދ4$f��t,�N�F61ҍ�    f��9�u��Nf�v f��9�u�f�V��1�[^_]Ë$1�1��xtf�N��&    f�W���1�����Zf�Dn����u�1�t& W�ى�   �a���Yf�Dn6����u�������&    ��&    �����UW��VS�ZX������\&  ��   �C`����   �Pd�R����   �J���|   �   �w��tG�j��t@���������u-ǃ\&     ��('  �W�y��Vl����Ql)�)΃�p���[^_]Ív �������뾍�&    ��8&  ���j���������Ӎv [�����^_]Í�    [�����^_]Í�    �����U�   WV��S�˹   ��h�RXd�    �D$d1��|$�D$   �{��  �FT9ω$�CO��   9�L��<$u���    �zt�{Ƀ᠁�@  9�O�9�LŉC��  9��=  ��8&  ����  �K��HM12�   i�  �D$�  �l$i,$�  �D$�|$�D$   Չ�|  �L$���  �L$���  f��  �B`f��  ��t6�Pd�R��t,�z��t%�   �L$���  �j����   1��������FX�VT��  �K��  �C   �K�C   �~T��   �CMPEG�C   �C    �D$    �D$dd+    ��   �D$��h[^_]Ít& i�p  �D$�  ���l$�������&    ��  9������i,$�  �K�D$    9�*|  ������i��  �D$    Ћ�|  �S���  �S���  �C�_���1�����������D$�����F��������f��������=   t2 �   ��t+1���   �   D�Ít& �1��� @  ����Ð�   Í�&    �v �����U��WV��S1��8f�1ۍ�&    f�U����1�����Yf�D^����u�1ۍt& U�ډ��   �q���Zf�D^2����u�[^_]Í�&    ��&    �����S���P�X21�f��9�u���b�v f��9�u�[Ít& �����ǀ  �  ������W�   S�Ã�\d�    �D$X1��|$�$   ��C�D$   ������D��D$�C
�D$��P�����t+�Pd�R��t!�Z��t�   ���t&�z��t1�������D$Xd+    u��\1�[_Ív 1���������������&    �v �������  1��f������WV��S���   �H|��($  ���  ��uZ�A����t��uN��0.  ����   ǆ,%     �    ��   ��%  �����f��t9��&    f�1�[^_Í�&    �v ǆ,%      �    t���  ��&    ���e�����%  �f% 0f f���d�����%  ������f�1���t& ���� �\����,   �8  ��  ��������0.  ��uՍ{����  f����0.  �����Ǆ�0.      ��s��  ��&    ������0����:  ��&    f��������wVW��V�� ���S�� ����Z�9�t.��    �Cd�@��t���t��|  ��������CL�X�9�u�[1�^_Í�&    �v 1��f�f�f�f�f�f�������U��WVS��~A�ƍ��ˍ<Bύt& ��6��h"  jS���������9�u�� ��[^_]Í�&    �v �ω�� [^_]Ít& �����UW��VS1ہ�   �D$��$�   �L$�D$d�    ��$�   1��  ��t& ���[���  ����  9�u�d�    �D$0�D$8�D$8�D$<�    �D$,    �D$4    �t!��  ��  ��t%  =  �  �[���  ���w  �D$�D$ �   ���1  �4$���   ���   �t$ 	  �D$�D$������t$�.�l$�l$��������v�@9���  �L$�ȃ������~  ���L$�D$�P �
   �t& ��:�
9�t��u�D$1�P$�D$��~A�\$(�|$�t$$�t$f����
   ��&    ��
�9�t��u����9�uًt$$�\$(�D$1ɍx<�
   �v ����t��u�T$�L$�
   ��    �
�*9�t��u�T$�
   ��&    �v ��9�t��u�[���  ��<���
��
������š    %�  =�  �R  �$�   �T$,������D$�
   �t$�t$ ��X&  �� 1� ��1����t!�9�t��u�t$�    �9D$�  �    )ЍT$,�D$�$������    ;l$��   %  =  �	  �t$1��N$�t$�v �������u��7�D$������[���  ��  1���t�    ��  �������$�   d+    ��  �Ĝ   [^_]�<�C  �D$�D$    ���1  �4$���   ���   �t$   �D$�|�����J����D$�����Pk����  �D$��Ph  ������������t& ���T$������    �T$)ЉD$�6�$�T$,�����9t$������D$�����9l$��   �    ��   ����������t& 1ҍD$@�
   �������&    ��$�   �T$@������9L$����uR���=  �$�T$@�   ������    ������
   )������P�D$��Ph�  ������������t& �����   ����������z����D$    �D$   �t���1Ҹ,  ������0����    u<������5���k��D$�����P���  �D$��Ph4  ����������������k��D$�����V�t$P���  �D$��Ph�  ������������������T$�D$�L$U�G���PWk����  �D$��Ph  ��������+����y  �$�T$@�L$$������L$$9L$uZ��
�p��������T$�D$�L$U�����Ph  ������k����  �D$�����VP�D$��Ph�  �������������
   �f�����  �L$��9�u���  d�    �D$0�D$8�D$8�D$<�D$,    �D$4    W�D$��Ph*  ���������������������W��VS�B�Ӆ���   ���   �   �  �S �09�uP�p�@9�tF�
   ���9�t��u�X&  �
   ��@1� f������t!�9�t��u�[^_Í�&    ��    u.�K[^_Í�&    ��u���   �   @  �u�����  ��  �t& �����U��WVS��,�@�D$�E����  ��t�E    ��,[^_]Ít& �E@=  u>�]p�    �@  �.   ��������|$��4&  ��u���t�9�s��"  ��    = �8  �uD�D$��������Å��^  �T$�EL1����D$�T$���  �T$���J  �,$�5f����  j �L$����������@  Y�  ��;|$�  �$�t�`�@t��L  9��  �p  9��  �$��L�d��������š    %  =  ��  ���K  ��@  ���  ���]����D$��0&  �D$���W  �E ��t]�E�u�E9�t	9u��  9�tA�l$�Չ\$�\$�|$��V��,&  ��0&  ������69�t�N��uߋ\$�l$�|$j ���  ��؃������X;|$������t& �,$�    ���  �    ������D$�   �   j �1  �������P  �Z�����j ��\  �   �   �����X����f���4  ��tZ���  ��tP�M ��tI�U�u�ыU9�t	9u��  9�t-�l$������4  �V�  ������69�t�N��u��l$j �ꍋ�  �������Z�����t& ��E ����   ���  �D$��������  ���  9���   ���  �F؉t$������D$����   �VD���u�M9�tS�\$ �Ӊl$$�͉|$(���%��;G$w�D$�V�������FGDË69�t�N��uՋD$�\$ �l$$�|$(�PD;��  sj�    ���  ���  ������D$�����j ���  ��������^�����v j �L$��������X�����v �D$������������&    �v �t$�FD    ������   �F�F�V��B��F�   �   �F"  �F�   j �����X�H���f��N���i����V�  ������W����v �N���b����D$�V�t$��,&  ������G�����  �t& ������U��W��VS��Td�    �D$P1�����  ����  ���   @  �D$1�1ۍ��1  ���&    �����t��F�J  �1���u荌7�1  �    �$���)  k�t�L$�\$�� �ǀ�1      �� 2  1����$�T@����u�\$�T$��$�A �B�Q �A$9��  k�t��� 2  ���j  ����   ��$2  =  ��   = �S  �,$�EL�MH�P����  �}D���   ����7D2  �����,�   1���&    f���T&  ����9�r�k�t���1  �  �D$Pd+    �  ���1  �$��T�    [^_]�������&    ��T2  � ��,2  ����   ���1  tm��&    ��D$Pd+    ��   ��T[^_]Í�&    �    ��  k�tǄ�1     ��������   �  �D$�����t& k�t���1  u��D$Pd+    ub�$��T��[^_]�=����t& ����������T&  �ع�   �������ƃ�    �  � ����������1  ������$������������������GPh$  �����XZ����f�������f�����    �����WVS�L$�|$��~�D$$�����v �p������r�9�u�W�T$�D$�"���Z[^_Í�&    ��    �����UWVS��d�    �D$1��\$0�t$4�L$8���  ����  �T$<���~"��1��却&    �v �z����|� ��9�|�U�������Z�T$d+    u��[^_]��������&    �v �����V��S��8  �   ��  ��  u1�����RPj��D  jh	  S������   ����  u��  1҅���t[^Í�&    �v ��  Rj ��D  jh	  S���������u�j ��  ��Pj��D  jh	  S�������[^Í�    �����S�L$��8  ���   ��  ��   ���   ��  wp���   �  v2���   ��  �q�1��D  jh  S�������[Í�&    f����   ��   �1j j j j ��D  jh  S�������$[�f����   ��  ���   ��   �q�q�q�1��D  jh  S������� [Ít& ����   ��  ��   ���   ��  ���   �~   �q�q�1��D  jh  S�������[Í�    ���   ��  �1�q��D  jh  S�������[Í�&    ���   �x  ��I����	ʉ�  [�����v ���   �Z  �1��D  jh  S�������[�f����   �  v���   �  1�[Á��   u�1��D  jh  S�������[Á��   t��*  �v �q�1��D  jh  S�������[Ív �1��D  jh  S�������[Í�    �q�q�q�1��D  jh  S������� [Ít& ����  �Q��  [�������1��D  jh  S�������[Í�    �1��D  jh  S�������[Í�    �1��D  jh  S�������[��*  �*  �*  �*  �*  f�f�f�f�f�f�������UWVS��<�$�T$d�    �D$81�����  �z�Ѓ��|$�D$ 9��p  �D$�h���a  �x�|$������$��\&  �W  �D$�D$    �8?���u	�@ȉD$�$�xu�D$����X�E��  ���E  �   ��t& ����9��
  �;�u�{ u�{ u��K�῀��u�)�9���  �v 1��D$    ��1����l$�ō�&    f��{���  �{� ��   �{� ��   �C���<���   �$�\$,�@`��tE�Pd�R��t;�
��t5�   ���K  �   ��T$���7  �L$�T$(�������    �T$4����   �L$����$��@'  �T$(��D'  �T$0��H'  �$������P'  �T$,�
��J�H�J�H�J�H�J�H�J�H�J�H�J�H�J �H �J$�H$�R(f�P(�D$�t& ���9�������D$�<$�l$��@'  �D$����   �L$�D$�����H�D$������$��<'  ����  �$��8'  �D$���<$�L$��8'  �|$�GA )�A ��D$;D$ ������D$8d+    �8  ��<[^_]Í�&    �   9������$��@'  �<$�D$�t$Ǉ@'      ǇH'      ǇD'      �@@   ���C�F�C�F�C�F�C�F�C�F�C�F�C�F�C �F �C$�F$�C(�F(�C,�F,�C0�F0�C4�F4�C8�F8�C<�F<��<'  ��������<$�D$    �D$    ��8'  �D$   ���D$$���@0  1��l$��1����p�L$���L$���������k�+���T,��D-��S�P�S�P�S�P�S�P�S�P�S�P�S�P�S �P �S$�P$�S(f�P(����@9|$~8�S���t�K��{� �Au�A����q����L$�� ���L$�i�����    ���  ��l$�FD f�F$��F�����F  ��F ���F! ]�Fc�����$�  �����T$�|$�ˍ��DA������؃�
�K
�� ���F itv0�V$�~(�|$�F�N��������!�F�����F�������F�����F���|$$�D �F�$����0  �?����<$�D$    �D$    ��8'  ���D$$���@0  �D$���I����  ��,   1�1��FD f�   1��F$��F�����F  ��F ���F! ]�Fc����%����T$(������������,'  �T$1ۉ|$��B�|$��   ���l$���=�t& ���  �z�ƃ�����  �с   )�)΁��  �����  ;\$th�x��ub�x� u\�x� uV�H��῀� uJ��
9�u����  �z�ƃ�����  �с  )�)΁��  ������  i��
�J�J늋l$iD$@  �|$�t$�G�<$��8'  �T���8'  �D$�������  �V,�F$�0  �F ITV0������   �   �����������������f������UWV��S����4&  �Vh� �1  �Bp�$�Fd��tD�Pd�R��t:�Z��t3�<$�   �W���n  �y���c  j 1������Y��&    f��F�n1ҍX�9�u�_��&    �CL�X�9�tN�Fh�@��t#��   t%�Cd�@��t�x��t�$1ɋPj ���������X��t�������t�����[^_]Ð��X&  �� � �$�߃�ϋ@����   ��td���� 9�tg�Ë�X&  �ߺ
   ��   �� � ��&    ��9����t1ب0t��u��1�[��^_]Ë��Y  �@�D�L�$������v ����9�u��� ��X&  ���
   ��   �� � �v �)����t
1��0�l�����u��b����v 9�t����T����t& �j 1������Z����������WVS�X`��t9�Sd�R��t/�2��t)�Hh�=   ���Y  �T� ��t�?��tj 1ɉ������Z[^_Ít& j ��1������X[^_������UWVS�Ӄ���X&  ��1� ��X&  #��1  ��D1� ��X&  #��1  �0� � #��1  ��u7��	��$��uc����   �,$���Z  �$1�������[^_]Í�&    ��X&  ��1� �
   ��&    �u �M ���t��t���u��	��$��t���X&  ��D1� �
   ��&    ��} �M ���t��t��u��t/��X&  �
   ���0� ��&    �E �M ���t��t��u�	$�    ��  ���  �C  ��t��   uu��   u]��������   u-��   ����j �   ���1  �   �����Y������t& j ���1  �   �   �����^빍�    1҉������똍t& ��   ��������z����    ��  ���  �Z����C  f���������Ћ�4  ��u}V���   S��X&  ��� ���    ���  EӃH(p�P<��X&  ��� �ڍs��������҃����   �   D։փ���E։P4��L  �P81�[^Í�&    �1�Í�&    ��    �����ǀ4     1�Í�&    ��&    �����UWVS�����   ��X&  �� � ��X&  ��� ��X&  � � �H  �    �G  D����  ��   ���$PSh    ��������ԃ��4��  ShL   ������������\  �"  ��&    ��    �����V���S��X&  ��� �
   ��2���t��uX&  ���+ ��� �
   ��&    f��2�����+ t��uX&  ��'R ��� �
   ��&    f��2����'R t��uX&  ��$� ��X&  ����� �
   ��   V��$� ��    ��9�t��u�[^������VS�����   �    ��#  ��X&  � ��tl���X&  0��Ό��� �
   �t& ��19�t��u�X&  ��� ��X&  ���� ��
   ��   ��� f���9�t��u��1�[^Ív ���X&  0퍐� �
   �v �
�29�t��u�X&  ��� ��X&  ���� ��
   ��� �
�9�t���u��1�[^Í�&    �t& �����UWVS�����   �q�D$��8  �ŉ$�� �  ����  �q�ʃ���  �A   �A   ��X&  ��t� ����X&  ���  ��t� ����X&  ��	���p� ��X&  �����  p� � ����	��z��<$�   D��z4$����9��$  9��$  �,�    9��$  9��$  �����  ��1�����  ������	����   �D$�  +D$%�  �D$���  �w  �    �'$  ��X&  � � ��0���	�X&  ��� �
   �v ��99�t��u�X&  ��� ��0�% �  	�X&  ��� �
   �v ��99�t��u�X&  ��� ����� �%  � 	�X&  �Ս�� �
   �t& ��*�
9�t��u�X&  ��� �D$��X&  0��؍�� ��	к
   ���99�t��u�X&  ��� �D$0�0�	�X&  ��� �
   �t& ��99�t��u�X&  ��� ��X&  ���� ��
   	��� ��    ��9�t��u�1���[^_]Í�&    f��  	 ���   �x�����a���   ��  
 �b���������������뺍�&    �t& �����WVS�P�JԋP(��	� �  wa�� 	� ��   ��	� ��   �@|��X&  ���� ��X&  ��0���	�� �
   �t& ���9�t��u�[1�^_Ít& ���	� uH�@|��X&  �� � ��X&  ������ �%  � 	Ё� � �
   ��&    ���9�t���u�릐[�����^_Í�&    �@|��X&  ������ ��X&  ��0�	¸
   ��� ��    ��9��Y�����u��O�����&    �v �x|��X&  �?�� � ��X&  ��0�	Ӻ
   �� � �t& ���>9�t��u�@|��X&  ��� � ��X&  ��0���	� � �
   ��&    ��9��������u�������&    �v �����UWV�   S�����   �$���������X&  �
   �� � ��    �1����t%   =   t��u拃X&  ��  � ��X&  ���
   ������  ��  � �t& �9����t	1���  t��u鋃X&  �
   ��  � ��&    ��1����t��9�t��u닃X&  X� � ��X&  %����
   ��X� ��&    ���19�t��u�X&    �
   ��X� �t& ���19�t��u�X&  \� � ��X&  %����
   ��\� ��&    ���19�t��u�X&    �
   ��\� �t& ���19�t��u�X&  �
 ��4� �
   �t& ��2�
��
 t��uX&  ��<� ��X&  �����
   ��� ����<� ��f��9����t	%� 9�t��u鋃X&  ���
   ��� �����<� �t& ��9����t	%� 9�t��u鋃X&  ��� ��X&  ��  ��� �
   ��    �
�29�t��u�X&  ��� ��X&  ����� �
   �v �
�29�t��u�X&  � �  ���� �
   �t& ��2�
�� �  t��uX&  1����� �
   �t& �2�
��t��u�X&  �� � ��X&  ��������   �� � �
   f���19�t��u���l�����X&  �� � ��X&  �������� �� � �
   �v ��19�t��u�X&  ���� ��X&  ��  P ���� �
   �
�29�t��u�X&  ���� ��X&  0��� ���� �
   ���19�t��u�X&  �.%@��� �
   �t& ��2�
��.%@t��uX&  ��� ��X&  ��   ���]��� �
   ���19�t��u�X&  �(f��x� �
   �t& ��2�
��(ft��uX&  �� � ��<���   ��vJ��   )�������	�É����<$��0  �JD�ZH��0  ���   �����   [^_]�������&    ��X&  ���� ��X&  0Ҹ
   ������ ��    ��9�t��u� �  1۸ �  뉍t& ���X&  ���� ��X&  0Ҹ
   ������ ��    ��9�t��u� .  1۸ .  �>���f����������1�Ív �������T  ��t	1�Í�    ǀT     ����1�Í�    �����WV��S�ÉЁ���  ����  � ��    ��X&  Ћ8��   �X&  ����!ǉ�������	����
   ��&    �
�9�t��u�[1�^_Ít& ������UW�׉�V��S���D$�\$�$����%��  ��    ��X&    � ǋ/��   �����X&  ����
   ����!�	��<$��v �����t1���t��u��1�[^_]Í�&    �v �����WVS��4  ��u^�Ë��   �֋�X&  ��� �R8�������tRw(��uD��h�   �	  Q������F8��L  XZ��[^_Ã�t��u#���͐1�[^��_Í�&    ���뷍v ��믿�����ƍ�&    �t& ������UWVS���  �Ë��  ��X&  ��% �  ����� %   �   ������ 	�X&  ���� �
   �t& ��)9�t��u�X&  �� � ���  �؁�   �������X&  �� � ���  �؃�`���������X&  �� �  ��   ��� �� 	���  �� �  	��  �0��X&  �� ������   ��� �
   ��&    ��1����t���t��u틃X&  ��� ��X&  ����� �
   ��   ��� �t& �1����t%   ?=   t��u拃X&  ���    ���uj��  � �   � � �����   u[^_]Í�    �΁��   j�  Q�����������غ  ��j��Q�������[^_]Í�&    ��� ��X&  ���   �
   ��� �v �1����t
��=�   t��u苃X&  ��� ��X&  ����� �
   ��   ��� ��&    �1����t%   ?=   ������u⋃X&  ������&    �0��X&  �� ������   ��� �
   ��1����t���t��u틃X&  ��� ��X&  ����� �
   ��   ��� �t& �1����t%   ?=   �q�����u��g����t& ��0��X&  �� ������   ��� �
   ��&    f��1����t���t��u틃X&  ��� ��X&  ����� �
   ��   ��� �t& �1����t%   ?=   �������u�������t& ���� ��X&  ���   �
   ��� �v �1����t
��=�   t��u苃X&  ��� ��X&  ����� �
   ��   ��� ��&    �1����t%   ?=   �Q�����u��G�����&    ��    ��������   �����1�Í�&    ��    �����UW��V��S�Ã�d�    �D$1��$    �D$    �D$    �    �K$  �G�����  �$   �O�1���
  �7  ���.  ���  �D$   ��σ���&    ��h�   �  ��Q�������X&  �� � ���  �؃��	����������X&  �� � XZ���l$����  �D$���  ����j�  ��Q�������X&  � � ]%   �Y�l$��P��t�L$��t0P  ��1ɽ   ����t2��  ���\  ����  ��t�� ����  ������uθ@  ��	�u߀����������f$  ��&    f���u�D$   ��?������v ���
  ��?�D$   ��@�����t& ���  ��   �������� �  ��   %  � ��� ���$  ��   �� p  ��$  ��   ��   ��$  -  p =   ��$  �����$   �   	����D$   �D$   	������������D$   �� �����������i����t& �����%�   ��   �������$  ����p��$  �� �����   ��$  ���$   �H����  ��   ��   ��?�   �D$   ��	��X�����    ����	��U�����X&  ��� �
   ��&    ���)9�t��u��  �؉��  ��������i���1��T$d+    u`��[^_]�f���������&    �������D$   	�   �������    �D$   ������������	�������������������f$  �������D  ���   ������&    �v ������ы�@  ���   �������&    ������S��X&  �ҍ�  � �
   �t& ��
�9�t��u�1�[Í�&    ��&    ������U��WVS�˃���X&  �|$�t$�$
   ��  � !�������ō�&    f������u���u!�9�t�,$u��1�[^_]Í�&    �t& �������X&  �ҍ�  � �1�Í�    ��������Ћ�X&  %��  ��  � � ����    ��Í�    �������X&  �ҍ�  � � Í�&    ������U����W��VS�É�t$��X&  %��  ��  � � ����    �����[!�	�^_��]�����t& �����S�ҋ�X&  ��  � Ӌ!��X&  L$�
   ��    �
�9�t��u�1�[Í�&    ��&    ������UWVS�Ã�\���  ���  ��X&  ����O���� 	���   �
��X&  ����� �
   ��   �ǜ� ��    ��/9�t��u�ǁ��� �|$(��   ����  �D$�   � �  �<�
 �D$   ]�D$$&   �D$ 0  �D$  $ �D$   0�D$  � �D$    �D$ <   �$  
 �D$H    �D$D   �D$@   �D$<0   �D$8C  �D$4&   �D$0]   �D$,�   �8  �v �
��X&  ����� �
   ��   �ǜ� ���/9�������u�ǁ��� �|$(������	���  ���D$z   	��D$$   ���D$   ��D$   ��   �D$   &��   Z���D$  P �|$���   ��$   ��   f���D$D   �|$���   ���D$@   �|$ �� ����D$<&   �� |  ���D$8�  ��D$4   ���D$,z   ��f��ɉ|$H������� | �������Z�D$0��X&  ��� ��X&  �ҋ�� ��X&  ������ � ���    �%  ��tO������ǉօ�t@�D$L    �𺹈b���T$P��	i�  )Ɖ�t$P��	���  ����t$L����$  ��X&  ��p� ��X&  0�T$��p� �
   ��&    ��19�t��u�X&  ��p� ��X&  0퍐p� �
   �t& �
�29�t��u�X&  ��p� ��X&  ���� ���  - ��p� �
   ��&    �v ��19�t��u�X&  ��p� ��X&  ����� T$��p� �
   �t& ��19�t��u�X&  ��t� ��X&  0�T$$��t� �
   ��&    ���19�t��u�X&  ��t� ��X&  0�T$��t� �
   ��19�t��u�X&  ��t� ��X&  ���� �T$��t� �
   �t& ��19�t��u�X&  ��t� ��X&  ����� T$��t� �
   �t& ��19�t��u�X&  ��x� ��X&  0҃���x� �
   ���19�t��u�X&  ��x� ��X&  0�����x� �
   ���19�t��u�X&  ��x� ��X&  ���� �T$��x� �
   �t& ��19�t��u�X&  ��x� ��X&  ����� T$��x� �
   �t& ��19�t��u�X&  ��|� ��X&  0�T$ ��|� �
   ��19�t��u�X&  ��|� ��X&  0�	ꍈ|� �
   f���19�t��u�X&  ��|� ��X&  ���� �$��|� �
   �t& ���19�t��u�|$(��҃������  ������ɉ�  ��X&  ��   ��|� ����� �
   	ʋ�X&  ��|� ��&    f���9�t��u��\[^_]Ív ���D$�   	��D$   ]���D$$&   ��D$ 0  ��   �D$  $ ��  � ���D$   0�|$���    ���D$@   �|$���L�D$<0   ��:���D$8C  �|$ �� ;  �D$4&   �� B  ���D$0]   ��D$,�   ��  ����  
 ���<$�1Ƀ� �����|$H�L$DɁ�L;����:B
 ������D$z   � �  �<�
 �D$   Z�D$$   �D$   �D$   �D$   &�D$  � �D$    �D$ <   �$  
 �D$H    �D$D   �D$@   �D$<&   �D$8�  �D$4   �D$0Z   �D$,z   �W���f������WVS��4  ���   ��u��<  ��8  1�1�	��   �։�8  ��    ��<  ǀ4      	���   �ց� @  	�tf�ց�   	��>  �ց�   	���   �ց�   	��.  �ց�   	��>  �ư�%  ���=  ��  � ��҉׃��	�t& �   �    �;&  1���X&  �� � ��к   ��	��� ���������X&  �� � ���  �؃��	����������������������[1�^_Í�&    �v �   농�&    f�1��   �    �Y&  ��X&  �� � ���   �   �؃��������X&  ��x� ���{  �؁��   ������1����t& ��   �   뛍t& 1��   뎍�&    �   �������    �   �������    1��   �[����t& �����UWVS�Ë�X&   � � %��  ���   ǃ�     ��  ��   �荳p  ǃ�     ǃ�  ��  ǃ�     ǃ      ǃ     �������   ��D  ��P�CP��   h�  j P�����1ɺ	   ��j ǃ@     ������ 	� �0  ��j h�   j jj h�   j j ������	� �0  ����8j j@j jj jj j ������	� �0  ���� j j@j jj jj j ������	� �0  ���� j j j jj jj�j�������	� �    ���� j j j h�  j h��  j j ������		� �    �� ���  ��j j j jj jj j ������	� �    ���� j h �  j h�  j h��  j j ������	� �    ���� j h �  j h�  j h��  j j ������	� �    ���� j h �  j h�  j h��  j j ��������  ��  �� ��u�C��������ǅ�t���������[^_]Í�&    f����������[^_]�f������V�� }  ����D�  S������t����  ��  ���  v2��D�  �  ����  �r  �� }  ��   ���  1�[^Ív ��D�  �$  ����  ��  �� }  uҹ0�  ����������+ �  ��������tv�  ��������  ��  ��������  � 	  ��������  �	  ��������  �	  ��������p   �'  ��������� �   ��    � �  ����������+ �  ��������tv�  ���������� 	  ����������	  ����������	  ��������`   �'  ���������/ �,  ���������.��(  �����������f���  ����������+ �  ���������'R �  ���������O� 	  ���������O�	  ���������O�	  ��������V   �'  ���������_ �,  ������������(  ������������f���  ����������+ �  ���������b �  ��������Ym� 	  ��������Ym�	  ��������Ym�	  ��������X   �'  ���������� �,  ���������K��(  ��������\���f�� �  ����������+ �  ��������tv�  �������� ���  ��������UU� 	  ��������UU�	  ��������UU�	  ��������`   �'  ���������? ������    �$�  ����������+ �  ���������b �  ���������`��  ���������s� 	  ���������s�	  ���������s�	  ��������d   �'  ���������a ����������.�����&    �t& ������VS�P�ZԋP(��	� ��  vR��	� �N  ��		� ��  ���  �p|��   1����������  ��1��P�������X��[^Í�&    ��	� ��   ��	� ��  �@|����=��  ~9h�   �   �؃���  �����V��  ��1�����������X��Z[^Ít& �j �   ��  ��������������)���P뿍�    �  �����������   ��j�  �؁��   Q������ع   ��  �����[^1��'����v �@|��   ����	=�/  ~��   )���ɉ�1���  �������[^Ít& ��@|�� �1��@�����������)Ѻ�  �A0�������P���������Y[^Ít& �@|�� �1��@�����������)Ѻ�  �A0�������P���������[[^Ít& ��j�  �ȉ�Q�����ZY�P���������F���f������S�  ��������  ��j�؁��   Q������  ��������  ��j�ȉ�Q������   ��  ������������  vy�p8��  ����������  �������  ��������  ��j�؁��   Q��������  YXw[Ív �  ��������  ��j�ȉ�Q�����XZ[Ív ���  �������녍�&    ��    �����V��S���   ���  v6�  ��������  ��j�؁��   Q������   ��  �������YX�  ��������  ��j�ȉ�Q���������(����  �Ɖ�������  ��j�؁��   Q����������  v#�  ��������  ��j�ȉ�Q�����XZ��[^�f�f�f�f�f�f�f�������U�c  WV��S��   ��d�    �D$1��F�D$    ���   �D$������D$�|$����(  �D$   j��   �   ��h0�  �������   �  ��h�   h�   ������   � �  ��j�h0�  ������   �   ��������D$,�x� �|$�D$������   1���&    �$1��8��	���   ��f��   ���������Ƨ  ������   �������9�tH1���?u����
uɃl$�%���hc  �t$h'  ������D$ ��������D$������'  f���;|$�k����D$���v 1�h   ��   �   ��h   ������t$hc  �t$h4'  ������   ���\$,�������%��� �      ����������   ���������   ���   ủ©�?  t�v �   ���������?  u�{����u&  ���?��   9�u:1����&    ��/��?��	��   9�u�   ���������9�u��u&  ��'  ����'  f�f������U�JW�z���V)�S���   1���`�������  �B    �B^    �1�f��  ��������tU�� �  �$  uT�t& ��������������� `  ����`  f��{����f��{������	�f	��5  u�[1�^_]Ít& ���������������� `  ����`  f��{����f��{������	�f	��0  u�뮍�&    �v �����S���   ���������  �  ��������ع.   �  �����1�[Ít& ������U��WVS�����   d�    �T$1҉��\$���  �� �  �t& ��  ��9�u���������2   �  ��������   �  ���������  �  ��������E����  ���t& 1�1Ƀ�f�P.f�H�9�u�D$���&    f�� u������9�tQ�U0f�� �  ��   f����   f�� u�	�U f�� ��   ��   f��u������9�u��$  ����   ��    �������ډ����������1  u��1ɉ����������5  u�   �<  ��������   �t  ��������D$d+    ��   ��1�[^_]Í�&    f�f�� @�����@�����t& �f�� @�*����U �f�� �'����v �`������������&    ���������&    ��������ډ����������5  u�   �<  ��������&   �9����U��    1Ƀ�f�H.1�f�H�9�u��E^    ���E    �:��������f������U����WVS���R���   �
��u�z�t�@    �@    ��1�[^_]�f��z�u��Z�\$��Ut���u��Z��  ���|$�z���Z�<$�\$�����   wJ����   ����   � @  �P�T$1Ƀ�?T$�|$U���<$�P��x��1�[^_]Í�    1���	uY�D$�$�\2�|2	��`  ��`  ����	���	��	������(��uˁ��   �D$�   �u�����&    ��@    1�1��u����   �S�����    �Z����1ˉ���1ى���1ك�t��J	����1ʉ���1ʉ���1ʃ�t��   �	���f�f�f�f�f�f������V�  � �   S����������   ��1�h   ��������   ���   X�
   f��2�
��t��u򋃄   �   ���   �
   �t& �2�
��t��u򋃄   �   ���   �
   �t& �2�
��t��u򋃄   �   ���   �
   �t& �2�
��t��u򋃄   �   ��  �
   �t& �2�
��t��u򋃄   �   ��  �
   �t& �2�
��t��u򋃄   �   ��D  �
   �t& �2�
��t��u򋃄   �   ��H  �
   �t& �2�
��t��u򋃄   �   ���  �
   �t& �2�
��t��u򋃄   �   ���  �
   �t& �2�
��t��u򋃄   �    ��  �
   �t& �2�
�� t��u򋃄   �    ��  �
   �t& �2�
�� t��u򋃄   �@   ��D  �
   �t& �2�
��@t��u򋃄   �@   ��H  �
   �t& �2�
��@t��u򋃄   ��   ���  �
   �t& �2�
���   t��u�   ��   ���  �
   ��&    f��2�
���   t��u�   �   ��  �
   ��&    f��2�
��   t��u�   �   ��  �
   ��&    f��2�
��   t��u�   �   ��D  �
   ��&    f��2�
��   t��u�   �   ��H  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ��D  �
   ��&    f��2�
��   t��u�   �   ��H  �
   ��&    f��2�
��   t��u�   �    ���  �
   ��&    f��2�
��    t��u�   �    ���  �
   ��&    f��2�
��    t��u�   � @  ���  �
   ��&    f��2�
�� @  t��u�   � @  ���  �
   ��&    f��2�
�� @  t��u�   � �  ��  �
   ��&    f��2�
�� �  t��u�   � �  ��  �
   ��&    f��2�
�� �  t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ��  �
   ��&    f��2�
��   t��u�   �   ��  �
   ��&    f��2�
��   t��u�   �   ��D  �
   ��&    f��2�
��   t��u�   �   ��H  �
   ��&    f��2�
��   t��u�   �@� ���   �
   ��&    f��2�
��@� t��u�   ��� ���   �
   ��&    f��2�
���� t��u�   ��� ��   �
   ��&    f��2�
���� t��u�   � � ��@  �
   ��&    f��2�
�� � t��u�   �@� ���  �
   ��&    f��2�
��@� t��u�   ��� ��   �
   ��&    f��2�
���� t��u�   ��� ��@  �
   ��&    f��2�
���� t��u�   � 	� ���  �
   ��&    f��2�
�� 	� t��u�   �@	� ��   �
   ��&    f��2�
��@	� t��u�   ��	� ��@  �
   ��&    f��2�
���	� t��u�   ��	� ���  �
   ��&    f��2�
���	� t��u�   � 
� ���  �
   ��&    f��2�
�� 
� t��u�   �@
� ��@  �
   ��&    f��2�
��@
� t��u�   ��
� ���  �
   ��&    f��2�
���
� t��u�   ��
� ���  �
   ��&    f��2�
���
� t��u�   � � ��   �
   ��&    f��2�
�� � t��u�   �@� ���  �
   ��&    f��2�
��@� t��u�   ��� ���  �
   ��&    f��2�
���� t��u�   ��� ��   �
   ��&    f��2�
���� t��u�   � � ��@  �
   ��&    f��2�
�� � t��u�   �
   �` � ��&    �2�
��` � t��u�   �
   �   ��`  ���
��t��u�[^�f�f�f�f�f�f�f�������WVS���  �ZD��ty��8  �    ��(  ��4  �      �������4  ��   u,�    ��(  ��(  �������1҉�������ǉ��������4    �������[^_ÿ������&    �t& �����UWVS�����  �^D�t$���j  ��8  �    ��(  ��(  ���������������ŉ$����������)  �Gh� ����   ����   �D$�@���
  ��4    �������4  ��   �H��   ��u1�    �)  ����������T  ��������$��x|���������4    ������$��[^_]Í�&    �v ��	�i�����X&  �� �� ��X&  ��  O �� �� �
   �t& �
�*9��6�����u��,�����&    �v �    �2)  ��4  ��   ��4  ��   ���V������T  ��I����$�����T�����&    f������U�v  W��VS��d�    �D$��$  �������$    ��8  �C���   ��������Ņ���)  �$��H���>�G)  ��G)  ���   1��=�t& ��rI  f�D$��t�   �T$����������   �$����9v*��&    ��J��D$<VtqwO<Tt�<UuK����9w޺]  f�T$��t�   �T$��������$������D$d+    uB����[^_]Ð<Wt��D�D$���z����   �\�����1���f�L$���D����X���������t& �����UW��VS��0��4  d�    �D$,��8  �D$���%  �;��(  �    ��  ǃ�      h�  �@�D$�   P�������Y��X���  ���  ���  ǃ�     ǃ�     ǃ�  ��  ǃ�  P�  ǃ�     �$������ƅ���  �$�S�C   �C    �щC(���C,   �T$��������ƅ��l  �S���  �$�T$������ƅ��D  �K�$�ΉL$���  ��������ƅ��  ���  �T$�$������ƅ���  ��4  �D$��8  �D$�@h� ���H  �  ���I  ��	��   ��  �����������  �D$���R  �|  �����������  �L$���  �  ������D$����  �D$��`   ���  h  �t$ �����Z��u
�  ������T$���  ���d  ǀ      �Ћ��  (  ������ƅ���   �D$�
   ��X&  ��$� �v �� � �����t%� � =�   t��u�$���  ��������l$��Uh�*  �����k�t  d�d   1���
��t  ��
RP��4  ��p  ��<  ��(  Uh�*  �������  ��   ��  ��$�����ǃ�     �m��&    �v �D$���  ���   ��t��������  �$��������  �T$�$��������  �T$�$������D$������$�������������D$,d+    �l  ��0��[^_]�f�����   ���n����t  �����������  �D$��U  ��  ����������d  �L$���  �Q  �t$ �D$$a�D$(    ����������=  �D$�T$ ���  ��������A  ���  ��������|  �����������t& ����������  �����������  �D$���R  ��  ����������v  �L$���  ��  ������D$���v  �D$��  �t$���  ��������T�����  ������E����t& ��,  �����������  �D$��U  ��  �����������  �L$���  �Q  �t$ �D$$a�D$(    ����������?  �D$�T$ ���  ��������  ���  ��������`  ���������h�  j�������  �����^��X��������)  h�  j������t  �����^��X��������)  ��&    f��,  �����������  �D$��U  ��  �����������  �L$���  �Q  �t$ �D$$a�D$(    �����������  �D$�T$ ���  �������t'���  ���������  �������������������Q  �����������  ������D$ǀ�      �D$�����Ph+  �����XZ������    �,  ������D$ǀ�      ��h;  j������,  �������XZ��� ����,*  ��&    ��  ������D$ǀ�      �y���h�  j�������  �������XZ���-�����)  �t& �t  ������D$ǀ�      �,  ����������e  �D$��U  ��  ����������R����,  ������D$ǀ�      ������,  ������D$ǀ�      �����h;  j������,  �����^��X��������Z#)�K\����V���%֧���]����q�����4��gV'5>D��5R�tIO�ٻ�QU|1_�y�/�ގa� ��Dl�cz�IE?J�P�\���/�v�hIhc�2�WǼ����D�mF�S	YȮ�
$���S�nmU��rWQ���NPH�)`�!��Mr L=\�5�<�H?T�K�mѨ�SK�x����"轟�">zj��v����:� ;�!&�MD��
�m�dٚ��K���ce]�8�B=E�8M�2m��SMD���ZP(�#�C���"�N|��$q�L!b��l��1?N�DC�*9��k3W k7a�~"�Q&x������N�k!"w�*90Á�;˛j�|�2�����o�[�0�R��S�1�����+�	����V�2�O/�i�v@�M�}un:إ��c����}!�{�xR��=��KRm@]����m7<�̛��l#�yK��w0?�>C��#S�Ꮊ�`f �M��a�����O(�KZ��*oQ���}_k:3_�μ��I�>
��W�(���A�?��,��H$��.�4�0ݨ�-n$9��i�y��,�ok���]�r��6r<L�_�5�f0r"�2���K��T�#W�Ek�h�:'�Vʴ��W>�Y�����=��/ ����}�-�#[0��7S#hJ�T&��١�^q '���Ms(�^HF%X�S� *{��E�ʝm�?J�[��h�Rg��C��#��{jr����ڠ�[���*�$,����,��ʮ��W�7�V�m�~�V��X�u��6}�Bot����ى��)�����򰁝u������nm��6b��h�$P�������1�Bq2
Lꔠ�(�5C�L��6��Rm�RIv��������ۺ�Zp�L��ړ�xD	��8��LF6�e�E��И� ��2�?�}9ʉU�*�y�'�B��eS��a�y������&���L\��������!�,�������h��V�}C#���j��pN(IQ�j%�N��' !����]Sp^KM���a޻�������ֆ�:�����8�]�h�:��u�rW��h�~t�ȇW�'Ϧ2ƹ뾘U���d	ݕa��ui�yD����T�,��}7�G]B0��#Ms"vl碕�F���Q�fL�VrF�7����759�Zړ6\�P�Y�K�*oЏ�2Ac"�$P�M��a�j�q�7��Qqk�)��D;�=�*�!Xj��5P�&�HB��5}w�[j����������TN��p��"�;�U& �}ʅ�C��V6pG"gf�q��Wc1����Ɩ=�Ǹ��,D���i�8�C�A�2����@��͏7�x�oˆ�H�}U�I��1��5vݾU�i�*�����c9f]���a����-��LB���K�D@.�	�}�W�uX��^�/��Z_]�Фzd��8�j�S��PJ���s�[Ð�5~�����a=X�fu�re*(��J���vn���cə_,�b�3�6_�Oym�Gx�j�u��ԋ9�̒�����1w'��y���j }w��v��	 _�_Qt7��PV 9}4��6�֏�V�bp*��U�ZE�Ϊvn-�� 6�X��Ўa�,�$(�_�}��j�P~(��P ��	���ps���k²V���Z��9��1&�͑m9r�&�72�"��ֳ���Y:�q@��K�2�kN��n���)߼-sr����Z$�����t	�9tE��P\�5o��/,_1{+a�ǆ��j A��o�d�B8��W;��t�Q}}쇃
�D�Jl���������M{Ƀ�!�����@�b���P�:�k��ꢨ���{#�ZӖ�Co ~��J����U�2S%b"1-��� �6�DK� ��s	��,��O@v���<.�C>۹jt��C&�.�_��}Z8	rS��D�;�z)�p��1�:u�u�qb<�g*hi��:���!g)#��b�<�L\Q=?��^^K�K��3x߬��{U밫n�:	�ᙅ�%��?\�_9�`e��Qu;�2F���L�s�(��]�@�ceފ��F��W?��K&ͥ�f0=�9�*���	�,�E`ߗ���I4��X�1;a��-��S���\�lCq�t&��a0�z�����c_o�d�w)o ��癷h������������i��*��+�W���OK`���� �E��V9QI\�]tL�ʹ�ٮ�G�Xߜ]��(D�J��/�E��]&��x*;�KZ^��[�g�p�Ia1F��@on�}�ؒ�.fxAP��Ǐ���1�a&衊*f����
�f)�8>n�ρ�o	Z�4\��ZHX]0���p��{_CD�[!�Y�m8��W�4ͅ�����m��x��S,k��29�X<F$]��{�>y�{��w̑m�݆�Ї��v�a5ĽYy�z7ƀ���� `���[X
�&������!�ɱ� ��D�}oG�ܒ �j��R�EH/ծ_����wB��Op*��2t.%�n��ũNk�ޕ\!����,�S +����ゐ?�C��y5S���!�*cF#3W�����LLū* xJ�{�s���%F��nD���2Ӥ=�.�D/��3y��ć2���܊1σz�.��g�V�"��9�BcM�p�|ڻ%��V0䏬c�J:I�"H<�vr<�-��5��%�Lq��Q9�nm�z�������j�,�
 9�;N�-ˬ�B\(Z���Ĭ��R���rH(z�ʘT�=P�!if�C])R�|�OG��0Su��9@%�¡�x�%{�!Pҟ�U����yi$�{�C���W��d��7�.�
V"��C��ܻ�����Q�	ͬ� ó:�!���Xd�K��i�Ovݽ��<-�PA�?�c�_y.�fD�r���i��J���zhJU�2��<��z��Iye`�Gne�y2���p����6�}��(�֬l�}ZLUb`�hdST )7�:�pb��s2�#�~��x��ûk}���]���Ԃ���>�wZ�B�e1j�|�FZ�N�MM�[�|������ޤBM�&/��YPǴQ՝�m1^u
�/��
i���*̳3~�45��4A�{/�
JA�!��b>�9�W�*��8-�J�����H�e�p)���7��<����5o��p�p22��2A�hx���Y�2��$�����8�,
Y��S�t����!�T����H.��gԀ녪�Y	�~��(���m#�P��#��o�_K]���ҽ9�d���!Ia��x� �&���1�@ra0�d��K�v���fC��7Ұl+Bx���Ƥ~*O�x�j;�HIסN�%��A]�'^��rH bd�%�NX��)*��8Ղ E���w������̑}�G{�ӑ���� �k��������`)X_�;Cn\-u�!�S�b(�A�H�ō�U3�tt�r,a�h�����δ�w�����C&ЊZk�{k]U�	�$A�>)9�"u���(��'�����"�3�zl�\L/6��/8�΄�|��
���+�Үx�x8�E�f[/?dwF\,�8��?�����S坲�5w��mu|�?�fv�G�f�PŦ:���w��M�M�b>]����V���� �p��Z��
�>�-�pq�Ĳd���yƸ?�)��Q��
����N00����{3G^8uzhł���+P���6^O���&���N�%��GR�\w$��b? �'�T��QW��!�\[��u#tNS�U��1k��!�	���ײ��i>۱aϬ��L���{3���V�;a��V���=~ЄP�\�w�q�}P�9�i�sF�O�q���I�x	��*�m��(�ʝ �aʅ��n�s��;�S�pG�y����X�����?'*-CY�#��v��$k�0o�8������a�A���+�3�t�#	/���� ��D.[ʋjU��}�:?s��]�|�y���N9ZG�wh�~���~�ڴe|I�C�z�ű��u}����_֏�q��U��w�}���:�{���>P�ONM�<��YF��4{�P	�Iת�x��t P�t$h	  ������   �����������sl�t$h�  ��������Y  ǃ�Y  �����    ���Y  ���Y  ���Y  �    ǃ�Y      ��������9  �t$h�  �����Y_������Fd�@��t�H(��t�T$��������vL��L������t$h�  �����Y_�[����D$@��\  �Y���1��|$4 ����`  �Q������   @t�Fd� ��t�H��t	1҉�������vL��L������t-�    t�t$h<  ��������Y  Y_���Y  ���/����ưt-�    t�t$h`  �����X���Y  Z���Y  ���������  � t0�    t�t$h�  �����_X���Y  ���Y  ��  @ ������t$h�  ��������Y  _���Y  Y�������t$h  �����XZ�����	   �����������t$h�  �����XZ�t����CPh�  ������������ZY��tC���6&  @������  �CPh�  ������������ZY����  ���6&  @������  �  �4  ������,  ������4  ������,  �������  �������  �������  ������@Phy  �����XZ��  ��  ������@Ph�  �����XZ��  �@Ph�  �����XZ�  �4$h  �D$��Ph�  ������D$@�D$� �D$���%  �D$�������Ph�  �����XZ�l'  h  �D$�������Ph  ��������l'  �|$h3  �_Sh�  �����Sh�  �������1ҿ������������l'  W�D$(�T$.����	��T$/	�Ph3  �D$��Ph`  ������D$D���8�V'  V�|$�GPh  ������D$<�������1ҿ������������l'  �t$�|$�GPh  ������D$<�������1ҿ������������l'  �\$h  �������ShD  �����Shh  ��������l'  �|$�$�D�PW�t$4h<  ��������&  �CPh�  �����XZ�O)  �CPhD  �����XZ�t/  �L$PR��<  �t$hl  ������E�U �L$,�B��D$0�|$0�M �E�8����  ���L$�2  ��<  ��8  ��Ph�  ���������7  ��<  �D$��Ph�  ���������������������:  �D$�XSh4  �����hj  She  ��������������:  ��t  �˾d   1���k�d��
��
��RSWQ��<  �|$(�_Sh�  ��������   ��p  �����  ����   �z�����L  ����:  ��8  �FPh�  �������@  ZY�@  �FPh�  �����XZ�?  �FPhd  �����ZY��H  �4$�t$jPR�t$$�D$��Ph(  ������� �vG  �F��<  Ph   ��������gA  R�t$h�  ��������~M  �F���  �����Ph�  ���������M  �D$�t$h�  �����Y_�T$�D$��EЉT$�O  ��<  Q�t$h@  ��������O  �$�   ���|  k�d����d   �����   R���P�D$���`  ��<  Q�t$h  ��������O  �T$U��<  �t$h�  ������T$ǃ�      ���Q����$���`  ��<  ��t#�<$���|  RPQ�t$hd  ��������O  PQ�t$h@  ��������O  ��VhQ  �����1�Y[��R  ��Vh�  �����^X�������R  ��PVh�  ��������������R  ����<  Vh  ���������S  �4$P�FPh4  ���������T  ��Vht  �����XZ�U  i$�  ���  �CPhL  ���������V  �CPh�  �����XZ��U  �CPh�  �����_X�uW  �CPh(  �����ZY��U  �CPh�  �����Y_�V  iD$H�  �\$8���  �GPhn  ���������_  iD$H�  �\$8���  �t$LiT$L�  �\$<�����  �t$8Ph   ��������!`  iD$H�  �\$8���  �GPh�  ������    ���Q`  �D$��������  W�D$0��Ph�  ��������$�����#]  iD$H�  �|$8���  �G�t$4Ph�  ���������W  ���  �D$H��Ph�  ������    ���eX  �G�L$Ph|  �����XZ�L$��a  �G�L$Ph�  �����Y]�L$�	b  ���t$�   ���  WhH  ��������Db  �CPh�  �����XZ��c  W�����  ShH  ��������d  �G�L$Ph�  �����YX�L$�^e  �G�L$Ph0  �����XZ�L$��e  �G�L$Ph�  �����XZ�L$��d  ���  �G�L$Ph�  ������L$����d  �CPh\  �����YX�f  ��Sh�  �����XZ�f  �CPh�  �����Y^�_f  ��Sh  �����XZ�lf  i$�  ���  �FPh�  ���������f  �G��<  Ph  ��������qh  ��  ��t<��PWh�  ������D$��������$������h  ��Wh�  �����XZ��h  ���   뼃�Qh  �����YX��n  ��QhL  �����YX��n  ��PQh3  ��������������n  ��Qh(  �����XZ��n  ��Qhp  �����XZ��n  �CVQPh�  ������Sh���o  ��ShQ  �����X1�Z�p  �G�vPh�  ��������Tp  �F���Y  ���Y  Ph�  ��������q  �����UWVS��   �ZXd�    ��$�   1��{�slhx  Wh~  ��������C|t�T$4��������C�k�p��FL9�t�Fd� ��t���t��������vL��L�����Y  �L$4�؍�X  ���)  ��������Y  �؉�������D$8PWh�  ������D$PWh�  �������������� X  ���W  Wh  ���������L  ��(�������4&  �u  � �o  D�PWh�  �������$  ���������4&  Wh�  ������������� t4�N���t-�Q�V�����
R�F�1�k�d��P�v��v�WhH  ����������  9�u����1  ���1  ���1  ���1  Wh�  ���������$�   d+    t������Ą   1�[^_]ÍGPh�  �����X�����Z�$s  �GPh�  ����������^_�$s  �G�$Ph(  �����Y[�$�s  �G�$Php  �����Y[�$�9s  P�GPh3  �������������$s  �G�$Ph  �����_X�$�s  �G�$PhL  �����XZ�$��s  ��  �������  ��������  Vh0  �����X1�Z�~�  ǆ,%      ���  Vh|  ����������Y[�~�  �����Ph�  �����XZ�c�  k����  �t$$�D$��Ph   ��������]�  P�D$��Ph@��k�c.Z��Dp0 :�G���"E�"�?:���SB1Ή����hJ��R�˃��j��"r��	A����hSo��?dJ)�X���Q6��`d�t+��;F�Q�P���^�w�Ƹ�֞��>UW=��W�}�I���pW��\m��ѵP�]�4�V��P����S���L�q�BG�co<��yY|ʆ������L�a�#���_N��k%樎E��S։#��%�,�tce�`��&	���P>��V7%�2������<�f�*P����f���}�������
0�ߑv���`5�&�*�=ߪ��[��/���%|x]�8�m�whu��˦j�M��h���C��+�I�[v.,=&�ݑv{��u�);�qr�nT��<۵��Nz��!�������L�̪t��!�m|���!g~s�WӨ5 ���'yV�-b������%�tow�🃍�x�����-�2:�g��L�{5[Q���56D�\O�{��5�x*t�ꟾ�: $���eF�,TYn�`�� �S�9Ur`'�V�!���77x����*����?E�zB�1��;du$JF%Cd��ш������+G�[�1o꼕�c����O�c�{%3vj��~	���ݑ�t�"L��*D_W+����ҧi<d�uX������
��Z�!uh�=���w�
�0$w
�ʖ3!f7���;�M�';��߄ui�k'�W�^�dM�Sy�e�k���h���c��<��y�,0�[%)�����HHe��2�~1?C�v��d�V�M�����M�)�M)(E��3ݖ��d�yH�W�|^�Jq�����g�qL���Y�ɠ.�]�>�؆��0�f� 9cV��7��)��4�d�
S��n(w|�|)|���h����}�gą���(��t��zv46t����crFK��߆�����+�pX�$ʋVA����8.��L2�-������;ZA�Ǣ�IGJ[��/�e�"�#��Os��,f�)~ֆ���k�^b#�X��������Xw
+pG�֜g%�Ι���Jb�]Gn6�*M�fnj�B�
�&j�_�t˯�`�PGu8�*@�L�1\n��qVZÝ�!m_7������+�P�9>�s����mT�M�\��x�)�Y�Z����Հ�"n����O��X�q$T�m�ǌ�i�؎Mh7���5Z�x$�|5�f^����-���K4�jV=MW���+A�G��i�Mߡ��7���U��E����J�j��{' /�{�A�Ҋj�w��+ðf`�W&uk�t��f�����C�?c.��X+3�f���e��}��i���ӽ�)>`rbmV�v�y� ��òI1��ǜ޾^4�b��������)+����}ǯx��Y\cT�F���k��xꝈ�S��JA1 �Y�x��m"�6IG�B�������bP��*�2��9�Fx_��AT=r63>����l�{΃ym�� ���P#�W7�>K�����9���-"�}xv|�QϘ(Rz�N>~g̻�eN�;�������&�~1�G��LR��=7�tì���@k�wM*���Cy��^A�j�ћ��<[�g�w�x>3$Pg�+��k4њҷ�x,�D���� r�����#����]��Z<�L*&k��J	f7?�{�O���_�\5�I�Ϧ������(�g��+��	�*Z[ȶ��8̓�p�v�$�g�����q�&���p�U����������"j"�q
J�e��m�tY�F$t��S��To:o��Qh����b�zq�7>��;�(2;�x^b��=�E����0,'t���>�d0aɜ~DL����}��0;��DZm�%Y��y `V�wJ��zw��-�Z��9����ơ|[\�|���v�ʿ:AF�H��]��O_�]������8�%� {�����;ވO�)�,��ګ��@���"t5Gz�4>^LDYD,�Yi��{t�5��"��h�LLғS��Š�O��!�E�P`"�$2��c�73��/@u��d�>���a(�|��ק�������u��:��kW�g��JP>��έ��]�౗2<�:���{ߢ��шI����6`��ghQq���o_q@�:�uqs�h;K)L�Y]q�˜GҥT3I�At^dm����%����z��ZG�����P7
�LŲ������a0�f�^ C]L�1�u��)N��oY��)�C*�:&�..8�$6c��8J��Ձ�Pz��e�e������Hk�^�K�P'-��6#�b^<Dބ�g᳂WDt���������[V�@\�]�)���
��%�!ƹ���I��^A�|�S����X$��	����EAf`<��u�"��5���n;��C�2�rG��I'aj9?�7�8/\����p��VT�����%h��T�kT&�7k��	�(��N�Z- L%�)i�,\4��#�M���^��3�B��;RȄ��˳J)h3]پ��v҈���C�(1#~ʙ��S�q՟o�N$<<��U2ʗ:ڙp�;$�ˌ+K�B�.���z���p����ǡ���8�G���˱�yp&��}�L�PeY	�ZD3[̱��_-���ę�멩[��@�wv,M �v��?��p�7�x�w���ak暔e�=LR������g�B�����ۉD\hL-���sׁ8`ZH��
�u�xFaE6��Y=X�󤲇���s��J�.���4ŏh��K��'!�z���ŝda��4�d&G��R�IF<�B?�	�c27�1��^o�|�@�=J��ꛁ1�����OTG�w�YY2^Xg�v���
 ��nr/�G���sׄ�1[����Pc^�${?�S��Q����)z����C��Č�B<�����q�]Cl-����hؚ�1&ݿ�t(�U�GG� &���a��џ�]�f1�-�* �~��^���R�Hvx��C�������Z��� � �ֱ��J݂(�IZ1�W���F#aR��GT2�J�����$��ֿ��)�G,�Q^��r���;�&ۻM1�St�zw��v�����s��Z�x[�|��y��:/�~�Z�%���D@"ۼ���'d�4�[��&o�=V͍��� �R�B���ݿU�����
�����s 0���49�O��J�-�r{�Uf��'s�~�=U��Q/���bJen�[�ue����Y�-x��0�������r�6��Ta4�vK:eC-�a�i�)���<<&% �]���ϗ-�ʩ���9oR}�bBr�z�;A��畯��cJs��ٿl��N��$�s�f���P�8
y���_�J���^ڽ(w�ټ�foYH2ZIB;@�#k ���fb`�&�'�[dMr�����#�����c�[|��<^�KO�.㎓��EimK�|#8�W�-�._��f�Uk��A��M�.up��;�\:0�?o�b�@$q�>�b\P6��^:�MSַ4�{�`�#G\M�'�y�X���=fOlh��T������Fw�ڃ'�����b��O�ŕ*:X�yˮ�6<��o���bX֘i�	�Xh�@�C�i�Ļ�n3���$��X��?MU�C�*�{am���J߈F��gB?Ԝ�"F�`�:$�%|�ף)��˨��(�S�u�J�b�r����� ��p�k�[�8*��LM_�#bf#r}sۍ@��uԀ�lm�MoKW{�F�Е�1A�ֿ�BĎ�>"I�=Jj�u�T�h�=�*���Y'�6��x����jqtc�>��c�W�.���fulp�_=/۟���L�Z*oN���*��!
��j4���{ƨ�n*�=�/K��w�-T���2)LE靾�樕�t�7�pa��C�rT����:�d��oYꗰ> )Q�A� ����x��C:>���^��yt!��JNH`�#���5���W��Fh��b�õk��5�p�*�9A��[�x�^������ �����hw�-q))�<i�d�zy���_nٟz�Ӫ'	�[��v�2��V���ڥ��|���F�a��4���h[6bw77�-p�:�ӗ��(�x�	1B���N�F`r����*H(�|��]w^pi�9���Y�<ˁH���3�J���;�J 5mQH��5
�e[ `NB�by�N$�.6_��Dˮ��ꀗ-��֧#O/W���L��%���#�����>V��_b��)��������gKUV��Wh0�(���v�Yf��y��[*�w�y��ih ���������H��rT-�8,�7���b>��w�O�v2�P�����|���F�G![�=ժ�(����.mV�#I��>�;��$C�i�A�;�!�F�_�;�m�;���5�W���go]�c���q��g���$,􆾔,2��=�|>��:�^L��l��f��{J���;`�v��e:B��1*�{4m�T�;N���a��=Y��������$��N�}��r�'��\On%=��$5i�8wy�#��B�'5[�]�U����!J��[+>{��
J�(j4Bs~jnkdV��� �x��4�� B�3LJ� ��� �t���}��M���|UX�m��9�K�	�l�9�� �0nӧ-�<����Ȫ�-oY�}�)-��Yҏ2�5\��5ۉ�V�=��no�"��ў7.t��cؼ�Rb�=����T�XJ]5�U*�dN�f 3��8N�G��H�Z�$b�K������X�SZ.���}���y�c�M��[�l\>{l��E�Մ����MN3�$��_߈ם#��L5航}�7�����]�m�B�=� \["f�s�����-�G��}����f��;3L��ڠ�K]$����YgO'��[�"�ѽޒ&Hs��V��hD�72£t[���L��(X��@�S.M�l{e[jK��i�c}H�< p����k�)��f�񹝳m3�^z�X�b��Z���z%à�k�71cYq��T�d�Z���34�����u�u��~'&iq��Ԯ��2�܌�I��+��X�jQ�C��Y����x�g/xL@����ź�q��k�A��G���Ty9�Y��oE���g(��@��w��UAe���0�����a���(g^�n�m����
����+9�*Ұ��d��k�����*��P�ܧ�?G�	,l���L��?E�/\1'�3����X`�����R��
RD讑��/P����x/w&���Y��O	�~�<;K��<)j�Z\�w�<`C�$�� �[�wUe�i/Z���ՠ�>O�������{����=�L�Q�	Y�N��|m���u��X��x�K�ƪ��vD[�wɸ=�XvKgd�3eS�IN��h�d0`Y�O������!P��`'��א/d�Zf5� ����!�˥��1��R�Չss8Y����h�_�W�ο:���B�F	h��hs͇��}�{ǆ����ȡ�@���kC�{�t&0��v�:?�����p����{S�"a[�2�j -X��;X���z����%`(�PDL	�������V�67y����g��������q�t�򏉐b�:�ј�F[!ଜa�~(�@���iԑ��9�e:U��ވ��h�V��.�O�eɫ'�w���	7��a�V\Meߝ�>��ꌑPӎ��6��@�~��4u��̬��+WCM�����s}#;;�9+շ�CQ;��G�0����7֢�2���r�'T�ﾟ��F��<�S^j�=f
�0TF@LT����aPG�`��_|$��B�)�@�PZ���*e��u�]I\Hm9#��^�'h�]�඲r;"ɷ�ONPs��Tlfl����󐋞'�g��"���DD��H�ڧy��a���T�nY5����x8+J���ݢ�+��|~o�Ba˲}�a�O �~YO��ͦI#��)��Qv�Z&��f���T���d;�v��O�ց��}/:w����O.E&E'0 ޗS��~�}��֬A
�O+lT��]�Y�%MUʷ5�p��Թ�n�O��S�Y�ܟ���(� �� Sl�v���<�P$�J�=Q�+��%��`A]p�V�7�/�Q �zc�ئ-m'=�ִ�n���훜1��K=��rC=�G��c]� mMz�c�B�b���_�'D�="�]�����NN����ڪ|xgMd�1C�P x�!���C��埠=}qTV�~�cq�.���?�F��D��I��I�e�4K���n��f����y�򺼽�cx�H�͛�:x�.�����_+�;�R��e�X@����ƈ�u"�h�v4�yP�Q�KM�qk=:B�+v-.������Md���d�O]SN����>�qm�����^�������`�����E+V�В�W�.`��_�Lo�����?��s�12����*p���
;�ȉMn���}���x>oۭ���l�f �@�ȼ�d�ͻ�4N�����/�D��տ�I�/�����>�*���Xr��������-0%��o����y����J|��,(�� �?\R<r��ʦ�dV#z�����߳*�S�)��g�`Yu)��ٯ�|@�'_��O�dYA9k��D�����L"SC�O��%ƃ����k����t�J�OMT�UĄx�)�
5��=�'�P8<Y��F�#���\�Mcn1��4�&ޠY��hV�(���I�J������(Όx���Ԍ� �Eϑ�9H�) �j��|Q��tʒ*�6�
�����@Okz]n���O:%y'�m�S|t�k�\P���V�0��ds��t�fP3�N^������*9쥦[[�޵���Q�e�U{x<l�Ut��%wE`b����L��׶xIv��*o�,v��L񇇧������%��r��i�}K�)N�eV���2}��Bs#�R�;�
��Xy�(�kF���Kdf���H4�*9U�IRp<3-�i�
<���79r,�2�s�c~l���)�~�����[��J�{���r��%7�[(JL�UN��I���2�9��dl�{t�+�H�څX�H��	}#ϘE/�y2��UA)�C��e�t���`U�cC%n��`�n�shjp�"���y�����S)[�� ,��%�U��>l� *PW9PLo�N�3f�]��b�H]�g�-Sk;k�1��(��&��B����8��#�BR1;3T���*����Ƣ�4>�P�Ȑ#uo��n1ŭ]�W=)���s���G�	~)��U��"x�v	��8C��V6��.Kܘf��>�B�Tv�)��m]s��h�ސ��e�X4�x�+�)��?���䭐�Xܸ�η(~��T 	5V8<]�@n+���6;~)�/��f2\v��U�E���%ȍ[�����@��<I��;\�O��2[Z��)�AϿd#F=�͔�͠m�.�:� 
$��7,�RF�4g6����y�]��]��rD���wR�!�
T���ȴ~Y�X��i���ŖF�L����"Q'Z�$�D_��k�B��8�#�Z�G;��3R7WkW�jT��CI\ �����H���e�g�r�����=��Ұc�<+�����h�Q��M�(��5w?���l�x�Lh�f,ok�6�����s��R���uh��O pcU��+�A�����43/lZ�SkPjj��bG�٥v��Q%T1�.��t%H�h6�9..-� qN�V��hL+���.\��;EX��@#ъR�o���}/A��+�	PM@>~�N;�˝}�$�ϳ����f����ݶ�c�m�,t��T2���A��w���T��>�-Z�/�W�K���q*��Ԭ�axs?��;�a����#Dhq��l_Ի���P:Ź/�;��Ad�$AE�8��:����0MHT��h���\�����o���pH�%�m���k��*����!� '�,�YiOBCj�Ŝw�`�v����x�"��i��)/-8h�2���C�m(jTK�?���l��Ĳ�GF���ۡb	�"�X�x�6N��IR������G4c-!��I��"e�Q�(r�0�2�Cȭ4N\����6���]�d��&6X�/�֦A����踝�0 �|"��>�|�;�_a7��qF��٩<��*��o@j�����܉>;~f
��$08#�Q���n��İ�:N�_���H�Q�`Ϣ;/ÿ�� B*`-����`�T`&�-�'ܹ�W��� ���~���bO"�n�.���[)��Q�Õ�'��'����=<<!��E���s�Tl?lɨ�b&��G��f�Zu���������|t�q��ե�3��r�����u#
"'ӌ8�̰���c��sq8��W<�1��<qt�%=�q���9F�U:���ֽ�dPA�2�:+�B�ڻ��cD��g�'z2�y�t�0C)W��9V�]ZF.��'m
�~v��Dr���d�'��>ʥ�H�*�R5���c��ݱ��F�Y'ǶX��q���|��&�gX<QTe4�T���a��}�wܿ����(ӓ2�
���YcrYJH�ˤ��8=�`�ZԘ{��W'�a��*_f-I���/����Be��}-8�Б��*�wO=�X�o	w��:"���R/S@l��V��ʦω6�W�+'�=)ל;�!��[�g]���#߳��0��D���9Kh��1}�ʓ>
�I���%�'n�E�3C2]>�j�8U�s1�P�>-d:x&ᠩ�=��Z>V��b�-����*��<��oO<:|-si��d��c!c٤9�w�x ^�źĭ���y���\�e���0�]
��E�nf�����h�A
㋂� �σ?X�K�sr��x/�{ɻ�1~R%B0�kj.`�*�(�ِ�P�~����ˣ~���}�s�������N�
G��Z-�t%ئ&#x����x%.s�'��$�A�d8�V�	aGy��BMY�?W���V&�9aT��^g�ǌ�W�ȝ�����sⅯ\w܅#fj�V4<�<O]��]!Ր\f��X#��ؐ-�m��u���eQ33���AK�[��q�,f9�~M�[�w㢡��6�q����df����}:3�����lĀ���|��ʶYй��п�����򢊓Fg�V"��!��#۱�5@]b�Ů ���"��Q����#�M��8�?�l��a#�S~JuB���4���t2�ܩD^h;��{P<9��i�(���X۰���D�3:�X6��m�}��t�$ ,;�ɚ��_J�����G���-���k��H��T��2lՈ��8�Y��z�؆�3ρvh��c!�-A�����0�FF*���|6�(ȋ����8�?X�7���B�`���;R�*u~~"�A_g�e
�p��D�nգj��뼷��q=�V�p���D�ɉ�I�%��`���~W(����!;$�z���'��B�z֚<�w�F������awiW�\>@^0���Ε��*?�*��T�мl8h�Z�#"��+d�Ix_��ܙ�xh�F+Fx��Xu�����B/�(�HD��cH���?��ד;O���S�� ��g�gN��Rj8���.O=bʩ��
2/Xs#}-6���Qge�����v6ܹw��B�
Y���ShR���6�\��}��(�C��<����I=��H����l�N|�,C�/��`;��~Da�ΎF��K�8��E�S��ˇ�&N�eW�cE�J�Wñ��N;���z����� �*����w���f�5�h���`��}� VX���AR���T�����\�j6�m�K؄l�Q��p�9�����k&�+����a��M�v�|�kue��jEC^�lD�Ew췍� ಔV̭@�Шu?�'"̚�E����	%�L1����#sUcі��Nް��뎋�T����6̶u�A&T$�[���>��K]6��E�/�혽o�n�i�Y��
g��C�~&��l]O�0^�q��$E���I�Ob���n�_��p쮩8ITЀ;�硓����
�� Z˽lԦEO����(=L�J���'b�f��V�Y+�h��.Z�G��8��ţ���罯#h���*��;�����{�<zů_��XzHmQc��r�2&떋��*�/���uH��9z�F����-{�{ƺ&��ȠU��g��ur56i����	m���:�<�����s��>��I%I�A���w���[�_�E�k"��*}m�1�`WNkE0(=M'r�����rdЧH)h�Z�.6�yf!�8�ui i�������ʻ�Hc a}�u �4��2;�qk��L��<���銄[R���>-�z�ߜ�Zl�D'���n=�NT�������Z5a�i\ҿ�@��N�$��B�j�0@rOǭl��� �ݚg���8v���ywI��Bg[�+R��3+�
�Io�SV�I�~�w�y�z9�.��܎��K{���`��3'�NYXI"��T�i�o��D�	�!�]�^|k�ڟO����j���t��^G����S�v1�$�B����JELv��Rp'�a�n�"�I�u���/!��;5��]@
,э�3Zjӳ�|n6���%�>O����jqxrv��q�];���L�c�Xx��{�Q����K�WM:�A�H(A��'��r������YŨ��f%L�_F���r�O�{Z+R�{;���y����CI[�O�����S���=�<���K�{V����T q�j�8��ɑ�ZP���C^�7��5�>sjm��v��ú�E:�s-X\�Hv������K��TN�B����-@=��D����y���(� ��~?��b�)�_o��ق�sEQʁl�g;tC��>�X���_߅�9�#�+l��M�h�e�M�>��(�Y�d_��_��%ey[�=u��\�*g�����bz�l��.�����L���Ś�\���4wI�� �6cX_�=�S�p���UF���Il��%�n�׋ߚ 6�,�uщ��?�44�K�u+���m=E��l:���e�7����#�=�t�|STn������
��2��]����N�&�@��z1�:5Z��V�"dn_'y�5�s��[|/➒p�'8pBed(�7��^��]���x��GU��|'��M��@�.���R�PT����Tb3�2��AX{ڿ!"��EXwk�Nn8Y�ܜ�o"�� ^�ST�F�G��1.�,�c�|R]��� ո��`Pqk��q�(pqZڼ���߳�&�9C��l`=ݛ�����#,.9��g�Y� R�+�a��r��3+�#�a�e1���,��aJϰX����HM�f��%�el�>ޔ���,���7ҕ#3�ߢ	B5&q+
֒�6�c�SZ���~'s�b�(Κ͝���	Rˈ�]�j��0����V2sxd�D4B'��|F��t}x'_�n�-�yr�=���N�����l@�,���+wi�?q�q��Q^�p����HU
�ƫ�<I�SN�;C�1���FM�'��"|ak}��i���
�����y�.�:��`e������9��7x0�k����ך�C����=��S�dK>�)��y��U[��Xva�o� 4쿟��~�C?��;�uL��!����8R��9=c%#]y�K"aI� ��Զ�������#���ض%Dw�kr�2�9��#G�����f��ly�%@�l�,OD�{O���`\F*yM���$��L4�)rL��t]�'��bcn������%����{�0���c�mS��;6�~��{ҿF��`x��xS�Jʢ2j�Qi��MC(��($�!�u�/��-���q����"[�	�bK��p�t�w۸���G���Dm��y$E��'a9�n}�?wu| 9qc2;�q��#��W~��P/��3�:Iד.9��I[qG�>���`<�G(��f�~/}���85��\`O����jO��2lT�����h_M��ػ��LV`�TɊ��U^��*o�&gjZ��X�˿PyMPGA��v��;�������	�=�(�O=-R�I��nt));
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