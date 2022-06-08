v_state = exception_enter();

	do {
		preempt_disable();
		local_irq_enable();
		__schedule(SM_PREEMPT);
		local_ir