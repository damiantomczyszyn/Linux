IRQ) << LOCK_USAGE_READ_MASK;

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
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Check whether we are holding such a class already.
 *
 * (Note that this has to be done separately, because the graph cannot
 * detect such classes of deadlocks.)
 *
 * Returns: 0 on deadlock detected, 1 on OK, 2 if another lock with the same
 * lock class is held but nest_lock is also held, i.e. we rely on the
 * nest_lock to avoid the deadlock.
 */
static int
check_deadlock(struct task_struct *curr, struct held_lock *next)
{
	struct held_lock *prev;
	struct held_lock *nest = NULL;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		prev = curr->held_locks + i;

		if (prev->instance == next->nest_lock)
			nest = prev;

		if (hlock_class(prev) != hlock_class(next))
			continue;

		/*
		 * Allow read-after-read recursion of the same
		 * lock class (i.e. read_lock(lock)+read_lock(lock)):
		 */
		if ((next->read == 2) && prev->read)
			continue;

		/*
		 * We're holding the nest_lock, which serializes this lock's
		 * nesting behaviour.
		 */
		if (nest)
			return 2;

		print_deadlock_bug(curr, prev, next);
		return 0;
	}
	return 1;
}

/*
 * There was a chain-cache miss, and we are about to add a new dependency
 * to a previous lock. We validate the following rules:
 *
 *  - would the adding of the <prev> -> <next> dependency create a
 *    circular dependency in the graph? [== circular deadlock]
 *
 *  - does the new prev->next dependency connect any hardirq-safe lock
 *    (in the full backwards-subgraph starting at <prev>) with any
 *    hardirq-unsafe lock (in the full forwards-subgraph starting at
 *    <next>)? [== illegal lock inversion with hardirq contexts]
 *
 *  - does the new prev->next dependency connect any softirq-safe lock
 *    (in the full backwards-subgraph starting at <prev>) with any
 *    softirq-unsafe lock (in the full forwards-subgraph starting at
 *    <next>)? [== illegal lock inversion with softirq contexts]
 *
 * any of these scenarios could lead to a deadlock.
 *
 * Then if all the validations pass, we add the forwards and backwards
 * dependency.
 */
static int
check_prev_add(struct task_struct *curr, struct held_lock *prev,
	       struct held_lock *next, u16 distance,
	       struct lock_trace **const trace)
{
	struct lock_list *entry;
	enum bfs_result ret;

	if (!hlock_class(prev)->key || !hlock_class(next)->key) {
		/*
		 * The warning statements below may trigger a use-after-free
		 * of the class name. It is better to trigger a use-after free
		 * and to have the class name most of the time instead of not
		 * having the class name available.
		 */
		WARN_ONCE(!debug_locks_silent && !hlock_class(prev)->key,
			  "Detected use-after-free of lock class %px/%s\n",
			  hlock_class(prev),
			  hlock_class(prev)->name);
		WARN_ONCE(!debug_locks_silent && !hlock_class(next)->key,
			  "Detected use-after-free of lock class %px/%s\n",
			  hlock_class(next),
			  hlock_class(next)->name);
		return 2;
	}

	/*
	 * Prove that the new <prev> -> <next> dependency would not
	 * create a circular dependency in the graph. (We do this by
	 * a breadth-first search into the graph starting at <next>,
	 * and check whether we can reach <prev>.)
	 *
	 * The search is limited by the size of the circular queue (i.e.,
	 * MAX_CIRCULAR_QUEUE_SIZE) which keeps track of a breadth of nodes
	 * in the graph whose neighbours are to be checked.
	 */
	ret = check_noncircular(next, prev, trace);
	if (unlikely(bfs_error(ret) || ret == BFS_RMATCH))
		return 0;

	if (!check_irq_usage(curr, prev, next))
		return 0;

	/*
	 * Is the <prev> -> <next> dependency already present?
	 *
	 * (this may occur even though this is a new chain: consider
	 *  e.g. the L1 -> L2 -> L3 -> L4 and the L5 -> L1 -> L2 -> L3
	 *  chains - the second one will be new, but L1 already has
	 *  L2 added to its dependency list, due to the first chain.)
	 */
	list_for_each_entry(entry, &hlock_class(prev)->locks_after, entry) {
		if (entry->class == hlock_class(next)) {
			if (distance == 1)
				entry->distance = 1;
			entry->dep |= calc_dep(prev, next);

			/*
			 * Also, update the reverse dependency in @next's
			 * ->locks_before list.
			 *
			 *  Here we reuse @entry as the cursor, which is fine
			 *  because we won't go to the next iteration of the
			 *  outer loop:
			 *
			 *  For normal cases, we return in the inner loop.
			 *
