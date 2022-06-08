nt));
	WRITE_ONCE(rdp->dynticks_nesting, 1);
	WARN_ON_ONCE(rdp->dynticks_nmi_nesting);
	WRITE_ONCE(rdp->dynticks_nmi_nesting, DYNTICK_IRQ_NONIDLE);
	instrumentation_end();
}

/**
 * rcu_idle_exit - inform RCU that current CPU is leaving idle
 *
 * Exit idle mode, in other words, -enter- the mode in which RCU
 * read-side critical sections can occur.
 *
 * If you add or remove a call to rcu_idle_exit(), be sure to test with
 * CONFIG_RCU_EQS_DEBUG=y.
 */
void rcu_idle_exit(void)
{
	unsigned long flags;

	local_irq_save(flags);
	rcu_eqs_exit(false);
	local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(rcu_idle_exit);

#ifdef CONFIG_NO_HZ_FULL
/**
 * rcu_user_exit - inform RCU that we are exiting userspace.
 *
 * Exit RCU idle mode while entering the kernel because it can
 * run a RCU read side critical section anytime.
 *
 * If you add or remove a call to rcu_user_exit(), be sure to test with
 * CONFIG_RCU_EQS_DEBUG=y.
 */
void noinstr rcu_user_exit(void)
{
	rcu_eqs_exit(true);
}

/**
 * 