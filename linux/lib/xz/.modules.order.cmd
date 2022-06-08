 *rq_i;
	bool need_sync;

	if (!sched_core_enabled(rq))
		return __pick_next_ta