tirqs will be enabled:
 */
void lockdep_softirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c, avoids
	 * funny state and nesting things.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		debug_atomic_inc(redundant_softirqs_on);
		return;
	}

	lockdep_recursion_inc();
	/*
	 * We'll do an OFF -> ON transition:
	 */
	current->softirqs_enabled = 1;
	trace->softirq_enable_ip = ip;
	trace->softirq_enable_event = ++trace->irq_eve