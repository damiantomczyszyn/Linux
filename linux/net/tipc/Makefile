ue of having been offline.
 */
static int rcu_implicit_dynticks_qs(struct rcu_data *rdp)
{
	unsigned long jtsq;
	struct rcu_node *rnp = rdp->mynode;

	/*
	 * If the CPU passed through or entered a dynticks idle phase with
	 * no active irq/NMI handlers, then we can safely pretend that the CPU
	 * already acknowledged the request to pass through a quiescent
	 * state.  Either way, that CPU cannot possibly be in an RCU
	 * read-side critical section that started before the beginning
	 * of the current RCU grace period.
	 */
	if (rcu_dynticks_in_eqs_sin