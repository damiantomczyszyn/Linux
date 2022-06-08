tes, loff_t off)
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
		.write = cpu_u