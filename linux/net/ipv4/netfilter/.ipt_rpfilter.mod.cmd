d domains by considering the
		 * cpuset configurations.
		 */
		cpuset_force_rebuild();
	}
	cpuset_update_active_cpus();
}

static i