t this point we're no longer running the old task and
	 * sending an IPI (below) ensures the sibling will no longer be running
	 * their task. This ensures there is no inter-sibling overlap between
	 * non-matching user state.
	 */
	for_each_cpu(i, smt_mask) {
		rq_i = cpu_rq(i);

		/*
		 * An online sibling might 