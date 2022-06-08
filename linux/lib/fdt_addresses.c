(likely(hlock_class(this)->usage_mask & new_mask))
		return 1;

	if (!graph_loc