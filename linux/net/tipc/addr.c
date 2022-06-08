e = "stat",
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
	e