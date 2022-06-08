if (__wake_q_add(head, task))
		get_task_struct(task);
}

/**
 * wake_q_add_safe(