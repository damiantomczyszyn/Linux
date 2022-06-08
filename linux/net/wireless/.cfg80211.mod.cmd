

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

	if (p->sched_clas