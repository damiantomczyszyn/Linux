 Push furthest requested GP to leaf node and rcu_data structure. */
	if (ULONG_CMP_LT(gp_seq_req, rnp->gp_seq_needed)) {
		WRITE_ONCE(rnp_start->gp_seq_needed, rnp->gp_seq_needed);
		WRITE_ONCE(rdp->gp_seq_needed, rnp->gp_seq_needed);
	}
	if (rnp != rnp_start)
		raw_spin_unlock_rcu_node(rnp);
	return ret;
}

/*
 * Clean up any old requests for the just-ended grace period.  Also return
 * whether any additional grace periods have been requested.
 */
static bool rcu_future_gp_cleanup(struct rcu_node *rnp)
{
	bool needmore;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);

	needmore = ULONG_CMP_LT(rnp->gp_seq, rnp->gp_seq_needed);
	if (!needmore)
		rn