f (should_resched(0)) {
		preempt_schedule_common();
		return 1;
	}
	/*
	 * In preemptible kernels, ->rcu_read_lock_nesting tells the tick
	 * whether t