

/*
 * period over which we measure -rt task CPU usage in us.
 * default: 1s
 */
unsigned int sysctl_sched_rt_period = 1000000;

__read_mostly int scheduler_running;

#ifdef CONFIG_SCHED_CORE

DEFINE_STATIC_KEY_FALSE(__sched_core_en