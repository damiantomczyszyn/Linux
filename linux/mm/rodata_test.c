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
		 * ha