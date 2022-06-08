 (!cpumask_subset(new_mask, cpus_allowed)) {
		/*
		 * We must have raced with a concurrent cpuset update.
		