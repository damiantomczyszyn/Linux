k_struct *curr,
				 struct task_struct *next)
{
	if (static_branch_unlikely(&preempt_notifier_