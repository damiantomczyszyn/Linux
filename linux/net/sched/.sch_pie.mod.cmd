f (should_resched(0)) {
		preempt_schedule_common();
		return 1;
	}
	/*
	 * In preemptible 