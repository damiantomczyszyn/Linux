t));

	lockdep_print_held_locks(curr);

	pr_warn("\nthe dependencies between %s-irq-safe lock and the holding lock:\n", irqclass);
	print_shortest_lock_dependencies_backwards(backwards_entry, prev_root);

	pr_warn("\nthe dependencies between the lock to be acquired");
	pr_warn(" and %s-irq-unsafe lock:\n", irqclass);
	next_root->trace = save_trace();
	if (!next_root->trace)
		return;
	print_shortest_lock_dependencies(forwards_entry, next_root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static const char *state_names[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE),
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static const char *state_rnames[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE)"-READ",
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline const char *state_name(enum lock_usage_bit bit)
{
	if (bit & LOCK_USAGE_READ_MASK)
		return state_rnames[bit >> LOCK_USAGE_DIR_MASK];
	else
		return state_names[bit >> LOCK_USAGE_DIR_MASK];
}

/*
 * The bit number is encoded like:
 *
 *  bit0: 0 exclusive, 1 read lock
 *  bit1: 0 used in irq, 1 irq enabled
 *  bit2-n: state
 */
static int exclusive_bit(int new_bit)
{
	int state = new_bit & LOCK_USAGE_STATE_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * keep state, bit flip the direction and strip read.
	 */
	return state | (dir ^ LOCK_USAGE_DIR_MASK);
}

/*
 * Observe that when given a bitmask where each bitnr is encoded as above, a
 * right shift of the mask transforms the individual bitnrs as -1 and
 * conversely, a left shift transforms into +1 for the individual bitnrs.
 *
 * So for all bits whose number have LOCK_ENABLED_* set (bitnr1 == 1), we can
 * create the mask with those bit numbers using LOCK_USED_IN_* (bitnr1 == 0)
 * instead by subtracting the bit number by 2, or shifting the mask right by 2.
 *
 * Similarly, bitnr1 == 0 becomes bitnr1 == 1 by adding 2, or shifting left 2.
 *
 * So split the mask (note that LOCKF_ENABLED_IRQ_ALL|LOCKF_USED_IN_IRQ_ALL is
 * all bits set) and recompose with bitnr1 flipped.
 */
static unsigned long invert_dir_mask(unsigned long mask)
{
	unsigned long excl = 0;

	/* Invert dir */
	excl |= (mask & LOCKF_ENABLED_IRQ_ALL) >> LOCK_USAGE_DIR_MASK;
	excl |= (mask & LOCKF_USED_IN_IRQ_ALL) << LOCK_USAGE_DIR_MASK;

	return excl;
}

/*
 * Note that a LOCK_ENABLED_IRQ_*_READ usage and a LOCK_USED_IN_IRQ_*_READ
 * usage may cause deadlock too, for example:
 *
 * P1				P2
 * <irq disabled>
 * write_lock(l1);		<irq enabled>
 *				read_lock(l2);
 * write_lock(l2);
 * 				<in irq>
 * 				read_lock(l1);
 *
 * , in above case, l1 will be marked as LOCK_USED_IN_IRQ_HARDIRQ_READ and l2
 * will marked as LOCK_ENABLE_IRQ_HARDIRQ_READ, and this is a possible
 * deadlock.
 *
 * In fact, all of the following cases may cause deadlocks:
 *
 * 	 LOCK_USED_IN_IRQ_* -> LOCK_ENABLED_IRQ_*
 * 	 LOCK_USED_IN_IRQ_*_READ -> LOCK_ENABLED_IRQ_*
 * 	 LOCK_USED_IN_IRQ_* -> LOCK_ENABLED_IRQ_*_READ
 * 	 LOCK_USED_IN_IRQ_*_READ -> LOCK_ENABLED_IRQ_*_READ
 *
 * As a result, to calculate the "exclusive mask", first we invert the
 * direction (USED_IN/ENABLED) of the original mask, and 1) for all bits with
 * bitnr0 set (LOCK_*_READ), add those with bitnr0 cleared (LOCK_*). 2) for all
 * bits with bitnr0 cleared (LOCK_*_READ), add those with bitnr0 set (LOCK_*).
 */
static unsigned long exclusive_mask(unsigned long mask)
{
	unsigned long excl = invert_dir_mask(mask);

	excl |= (excl & LOCKF_IRQ_READ) >> LOCK_USAGE_READ_MASK;
	excl |= (excl & LOCKF_IRQ) << LOCK_USAGE_READ_MASK;

	return excl;
}

/*
 * Retrieve the _possible_ original mask to which @mask is
 * exclusive. Ie: this is the opposite of exclusive_mask().
 * Note that 2 possible original bits can match an exclusive
 * bit: one has LOCK_USAGE_READ_MASK set, the other has it
 * cleared. So both are returned for each exclusive bit.
 */
static unsigned long original_mask(unsigned long mask)
{
	unsigned long excl = invert_dir_mask(mask);

	/* Include read in existing usages */
	excl |= (excl & LOCKF_IRQ_READ) >> LOCK_USAGE_READ_MASK;
	excl |= (excl & LOCKF_IRQ) << LOCK_USAGE_READ_MASK;

	return excl;
}

/*
 * Find the first pair of bit match between an original
 * usage mask and an exclusive usage mask.
 */
static int find_exclusive_match(unsigned long mask,
				unsigned long excl_mask,
				enum lock_usage_bit *bitp,
				enum lock_usage_bit *excl_bitp)
{
	int bit, excl, excl_read;

	for_each_set_bit(bit, &mask, LOCK_USED) {
		/*
		 * exclusive_bit() strips the read bit, however,
		 * LOCK_ENABLED_IRQ_*_READ may cause deadlocks too, so we need
		 * to search excl | LOCK_USAGE_READ_MASK as well.
		 */
		excl = exclusive_bit(bit);
		excl_read = excl | LOCK_USAGE_READ_MASK;
		if (excl_mask & lock_flag(excl)) {
			*bitp = bit;
			*excl_bitp = excl;
			return 0;
		} else if (excl_mask & lock_flag(excl_read)) {
			*bitp = bit;
			*excl_bitp = excl_read;
			return 0;
		}
	}
	return -1;
}

/*
 * Prove that the new dependency does not connect a hardirq-safe(-read)
 * lock with a hardirq-unsafe lock - to achieve this we search
 * the backwards-subgraph starting at <prev>, and the
 * forwards-subgraph starting at <next>:
 */
static int check_irq_usage(struct task_struct *curr, struct held_lock *prev,
			   struct held_lock *next)
{
	unsigned long usage_mask = 0, forward_mask, backward_mask;
	enum lock_usage_bit forward_bit = 0, backward_bit = 0;
	struct lock_list *target_entry1;
	struct lock_list *target_entry;
	struct lock_list this, that;
	enum bfs_result ret;

	/*
	 * Step 1: gather all hard/soft IRQs usages backward in an
	 * accumulated usage mask.
	 */
	bfs_init_rootb(&this, prev);

	ret = __bfs_backwards(&this, &usage_mask, usage_accumulate, usage_skip, NULL);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}

	usage_mask &= LOCKF_USED_IN_IRQ_ALL;
	if (!usage_mask)
		return 1;

	/*
	 * Step 2: find exclusive uses forward that match the previous
	 * backward accumulated mask.
	 */
	forward_mask = exclusive_mask(usage_mask);

	bfs_init_root(&that, next);

	ret = find_usage_forwards(&that, forward_mask, &target_entry1);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/*
	 * Step 3: we found a bad match! Now retrieve a lock from the backward
	 * list whose usage mask matches the exclusive usage mask from the
	 * lock found on the forward list.
	 *
	 * Note, we should only keep the LOCKF_ENABLED_IRQ_ALL bits, considering
	 * the follow case:
	 *
	 * When trying to add A -> B to the graph, we find that there is a
	 * hardirq-safe L, that L -> ... -> A, and another hardirq-unsafe M,
	 * that B -> ... -> M. However M is **softirq-safe**, if we use exact
	 * invert bits of M's usage_mask, we will find another lock N that is
	 * **softirq-unsafe** and N -> ... -> A, however N -> .. -> M will not
	 * cause a inversion deadlock.
	 */
	backward_mask = original_mask(target_entry1->class->usage_mask & LOCKF_ENABLED_IRQ_ALL);

	ret = find_usage_backwards(&this, backward_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (DEBUG_LOCKS_WARN_ON(ret == BFS_RNOMATCH))
		return 1;

	/*
	 * Step 4: narrow down to a pair of incompatible usage bits
	 * and report it.
	 */
	ret = find_exclusive_match(target_entry->class->usage_mask,
				   target_entry1->class->usage_mask,
				   &backward_bit, &forward_bit);
	if (DEBUG_LOCKS_WARN_ON(ret == -1))
		return 1;

	print_bad_irq_dependency(curr, &this, &that,
				 target_entry, target_entry1,
				 prev, next,
				 backward_bit, forward_bit,
				 state_name(backward_bit));

	return 0;
}

#else

static inline int check_irq_usage(struct task_struct *curr,
				  struct held_lock *prev, struct held_lock *next)
{
	return 1;
}

static inline bool usage_skip(struct lock_list *entry, void *mask)
{
	return false;
}

#endif /* CONFIG_TRACE_IRQFLAGS */

#ifdef CONFIG_LOCKDEP_SMALL
/*
 * Check that the dependency graph starting at <src> can lead to
 * <target> or not. If it can, <src> -> <target> dependency is already
 * in the graph.
 *
 * Return BFS_RMATCH if it does, or BFS_RNOMATCH if it does not, return BFS_E* if
 * any error appears in the bfs search.
 */
static noinline enum bfs_result
check_redundant(struct held_lock *src, struct held_lock *target)
{
	enum bfs_result ret;
	struct lock_list *target_entry;
	struct lock_list src_entry;

	bfs_init_root(&src_entry, src);
	/*
	 * Special setup for check_redundant().
	 *
	 * To report redundant, we need to find a strong dependency path that
	 * is equal to or stronger than <src> -> <target>. So if <src> is E,
	 * we need to let __bfs() only search for a path starting at a -(E*)->,
	 * we achieve this by setting the initial node's ->only_xr to true in
	 * that case. And if <prev> is S, we set initial ->only_xr to false
	 * because both -(S*)-> (equal) and -(E*)-> (stronger) are redundant.
	 */
	src_entry.only_xr = src->read == 0;

	debug_atomic_inc(nr_redundant_checks);

	/*
	 * Note: we skip local_lock() for redundant check, because as the
	 * comment in usage_skip(), A -> local_lock() -> B and A -> B are not
	 * the same.
	 */
	ret = check_path(target, &src_entry, hlock_equal, usage_skip, &target_entry);

	if (ret == BFS_RMATCH)
		debug_atomic_inc(nr_redundant);

	return ret;
}

#else

static inline enum bfs_result
check_redundant(struct held_lock *src, struct held_lock *target)
{
	return BFS_RNOMATCH;
}

#endif

static void inc_chains(int irq_context)
{
	if (irq_context & LOCK_CHAIN_HARDIRQ_CONTEXT)
		nr_hardirq_chains++;
	else if (irq_context & LOCK_CHAIN_SOFTIRQ_CONTEXT)
		nr_softirq_chains++;
	else
		nr_process_chains++;
}

static void dec_chains(int irq_context)
{
	if (irq_context & LOCK_CHAIN_HARDIRQ_CONTEXT)
		nr_hardirq_chains--;
	else if (irq_context & LOCK_CHAIN_SOFTIRQ_CONTEXT)
		nr_softirq_chains--;
	else
		nr_process_chains--;
}

static void
print_deadlock_scenario(struct held_lock *nxt, struct held_lock *prv)
{
	struct lock_class *next = hlock_class(nxt);
	struct lock_class *prev = hlock_class(prv);

	printk(" Possible unsafe locking scenario:\n\n");
	printk("       CPU0\n");
	printk("       ----\n");
	printk("  lock(");
	__print_lock_name(prev);
	printk(KERN_CONT ");\n");
	printk("  lock(");
	__print_lock_name(next);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
	printk(" May be due to missing lock nesting notation\n\n");
}

static void
print_deadlock_bug(struct task_struct *curr, struct held_lock *prev,
		   struct held_lock *next)
{
	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("============================================\n");
	pr_warn("WARNING: possible recursive locking detected\n");
	print_kernel_ident();
	pr_warn("--------------------------------------------\n");
	pr_warn("%s/%d is trying to acquire lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(next);
	pr_warn("\nbut task is already holding lock:\n");
	print_lock(prev);

	pr_warn("\nother info that might help us debug this:\n");
	print_deadlock_scenario(next, prev);
	lo