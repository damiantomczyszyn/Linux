_lock_class(lock, 0);

		/*
		 * If look_up_lock_class() failed to find a class, we're trying
		 * to test if we hold a lock that has never yet been acquired.
		 * Clearly if the lock hasn't been acquired _ever_, we're not
		 * holding it either, so report failure.
		 */
		if (!class)
			return 0;

		/*
		 * References, but not a lock we're actually ref-counting?
		 * State got messed up, follow the sites that change ->references
		 * and try to make sense of it.
		 */
		if (DEBUG_LOCKS_WARN_ON(!hlock->nest_lock))
			return 0;

		if (hlock->class_idx == class - lock_classes)
			return 1;
	}

	return 0;
}

/* @depth must not be zero */
static struct held_lock *find_held_lo