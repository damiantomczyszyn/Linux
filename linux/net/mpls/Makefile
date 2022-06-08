();
}

/*
 * Print out an error if an invalid bit is set:
 */
static inline int
valid_state(struct task_struct *curr, struct held_lock *this,
	    enum lock_usage_bit new_bit, enum lock_usage_bit bad_bit)
{
	if (unlikely(hloc