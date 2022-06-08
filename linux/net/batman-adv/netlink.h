FIG_RCU_EQS_DEBUG=y.
 */
noinstr void rcu_nmi_enter(void)
{
	long incby = 2;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);

	/* Complain about underflow. */
	WARN_ON_ONCE(rdp->dynticks_nmi_nesting < 0);

	/*
	 * If idle from RCU viewpoint, atomically increment ->dynticks
	 * to mark non-idle and increment ->dynticks_nmi_nesting by one.
	 * Otherwise, increment ->dynticks_nmi_nesting by two.  This means
	 * if ->dynticks_nmi_nesting is equal to one, we are guaranteed
	 * to be in the outermost NMI handler that interrupted an RCU-idle
	 * period (observation due to Andy Lutomirski).
	 */
	if (rcu_dynticks_curr_cpu_in_eqs()) {

		if (!in_nmi())
			rcu_dynticks_task_exit();

		// RCU is not watching here ...
		rcu_dynticks_eqs_exit();
		// ... but is watching here.

		instrumentation_begin();
		// instrumentation for the noinstr rcu_dynticks_curr_cpu_in_eqs()
		instrument_atomic_read(&rdp->dynticks, sizeof(rdp->dynticks));
		// ins