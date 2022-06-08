
			       &hlock_class(prev)->locks_after,
			       next->acquire_ip, distance,
			       calc_dep(prev, next),
			       *trace);

	if (!ret)
		return 0;

	ret = add_lock_to_list(hlock_class(prev), hlock_class(next),
			       &hlock_class(next)->locks_before,
			       next->acquire_ip, distance,
			       calc_depb(prev, next)