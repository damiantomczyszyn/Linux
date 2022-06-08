ng tasks to offline CPUs is a fairly daft thing.
	 */
	WARN_ON_ONCE(!cpu_online(new_cpu));

	WARN_ON_ONCE(is_