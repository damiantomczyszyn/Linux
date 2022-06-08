me lock
		 * already?
		 */
		int ret = check_deadlock(curr, hlock);

		if (!ret)
			r