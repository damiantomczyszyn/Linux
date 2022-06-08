p_assert_rq_held(rq);

	/*
	 * Ensure the thing is persistent until balance_push_set(.on = false);
	 */
	rq->balance_callback = &balance_push_callback;

	/*
	 * Only active while going offline and when invoked on the outgoing
	 * CPU.
	 */
	if (!cpu_dying(rq->cpu) || rq != this_rq())
		return;

	/*
	 * Both the cpu-hotplug and stop task are in this case and are
	 * required to complete the hotplug process.
	 */
	if (kthread_is_per_cpu(push_task) ||
	    is_migration_disabled(push_task)) {

		/*
		 * If this is the idle task on the outgoing CPU try to wake
		 * up the hotplug control thread which might wait for the
		 * last task to vanish. The rcuwait_active() check i