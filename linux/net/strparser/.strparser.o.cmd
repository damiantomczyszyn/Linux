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
   èüÿÿÿƒë„†   ‹†X&   Ç ‹ ¨uÚ1Ò¸È   èüÿÿÿ‹t$ºˆ   ‰ğèüÿÿÿj jh   D$@PVèüÿÿÿ‰ÇƒÄ…ÀˆEÿÿÿ