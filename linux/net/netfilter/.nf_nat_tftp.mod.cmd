_name(read_bit));
	}

	return 0;
}

void print_irqtrace_events(struct task_struct *curr)
{
	const struct irqtrace_e