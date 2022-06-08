tirqs will be enabled:
 */
void lockdep_softirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irq