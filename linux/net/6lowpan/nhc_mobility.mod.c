if
}

DEFINE_STATIC_KEY_FALSE(sched_numa_balancing);

#ifdef CONFIG_NUMA_BALANCING

int sysctl_numa_balancing_mode;

static void __set_numabalancing_state(bool enabled)
{
	if (enabled)
		static_branch_enable(&sched_numa_balancing);
	else
		static_branch_disable(&sched_numa_balancing);
}

void set_numabalancing_state(bool enabled)
{
	if (enabled)
		sysctl_numa_balancing_mode = NUMA_BALANCING_NORMAL;
	else
		sysctl_numa_balancing_mode = NUMA_BALANCING_DISABLED;
	__set_numabalancing_state(enabled);
}

#ifdef CONFIG_PROC_SYSCTL
int sysctl_numa_balancing(struct ctl_table *table, int write,
			  void *buffer, size