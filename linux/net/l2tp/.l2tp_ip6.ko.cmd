hed_uclamp_util_min_rt_default;
	uclamp_se_set(uc_se, default_util_min, false);
}

static void uclamp_update_util_min_rt_default(struct task_struct *p)
{
	struc