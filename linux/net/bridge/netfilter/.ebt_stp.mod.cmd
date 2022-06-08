FIG_RCU_EQS_DEBUG=y.
 */
noinstr void rcu_nmi_enter(void)
{
	long incby = 2;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data)