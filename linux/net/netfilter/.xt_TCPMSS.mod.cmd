*pf;
	unsigned long flags;
	bool found = false;

	might_sleep();

	if (WARN_ON_ONCE(static_obj(key)))
		retur