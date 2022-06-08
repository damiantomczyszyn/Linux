nt_modules();
	if (irqs_disabled())
		print_irqtrace_events(prev);
	if (IS_ENABLED(CONFIG_D