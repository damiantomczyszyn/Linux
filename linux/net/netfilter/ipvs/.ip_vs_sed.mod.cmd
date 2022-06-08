just need the target to call irq_exit() and re-evaluate
	 * the next tick. The nohz full kick at least implies that.
	 * If 