me lock
		 * already?
		 */
		int ret = check_deadlock(curr, hlock);

		if (!ret)
			return 0;
		/*
		 * Add dependency only if this lock is not the head
		 * of the chain, and if the new lock introduces no more
		 * lock dependency (because we already hold a lock with the
		 * same lock class) nor deadlock (because the nest_lock
		 * serializes nesting locks), see the comments for
		 * check_deadlock().
		 */
		if (!chain_head && ret != 2) {
			if (!check_prevs_add(curr, hlock))
				return 0;
		}

		graph_unlock();
	} else {
		/* after lookup_chain_cache_add(): */
		if (unlikely(!debug_locks))
			return 0;
	}

	return 1;
}
#else
static inline int validate_chain(struct task_struct *curr,
				 struct held_lock *hlock,
				 int chain_head, u64 chain_key)
{
	return 1;
}

static void init_chain_block_buckets(void)	{ }
#endif /* CONFIG_PROVE_LOCKING */

/*
 * We are building curr_chain_key incrementally, so double-check
 * it from scratch, to make sure that it's done correctly:
 */
static void check_chain_key(struct task_struct *curr)
{
#ifdef CONFIG_DEBUG_LOCKDEP
	struct held_lock *hlock, *prev_hlock = NULL;
	unsigned int i;
	u64 chain_key = INITIAL_CHAIN_KEY;

	for (i = 0; i < curr->lockdep_depth; i++) {
		hlock = curr->held_locks + i;
		if (chain_key != hlock->prev_chain_key) {
			debug_locks_off();
			/*
			 * We got mighty confused, our chain keys don't match
			 * with what we expect, someone trample on our task state?
			 */
			WARN(1, "hm#1, depth: %u [%u], %016Lx != %016Lx\n",
				curr->lockdep_depth, i,
				(unsigned long long)chain_key,
				(unsigned long long)hlock->prev_chain_key);
			return;
		}

		/*
		 * hlock->class_idx can't go beyond MAX_LOCKDEP_KEYS, but is
		 * it registered lock class index?
		 */
		if (DEBUG_LOCKS_WARN_ON(!test_bit(hlock->class_idx, lock_classes_in_use)))
			return;

		if (prev_hlock && (prev_hlock->irq_context !=
							hlock->irq_context))
			chain_key = INITIAL_CHAIN_KEY;
		chain_key = iterate_chain_key(chain_key, hlock_id(hlock));
		prev_hlock = hlock;
	}
	if (chain_key != curr->curr_chain_key) {
		debug_locks_off();
		/*
		 * More smoking hash instead of calculating it, damn see these
		 * numbers float.. I bet that a pink elephant stepped on my memory.
		 */
		WARN(1, "hm#2, depth: %u [%u], %016Lx != %016Lx\n",
			curr->lockdep_depth, i,
			(unsigned long long)chain_key,
			(unsigned long long)curr->curr_chain_key);
	}
#endif
}

#ifdef CONFIG_PROVE_LOCKING
static int mark_lock(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit new_bit);

static void print_usage_bug_scenario(struct held_lock *lock)
{
	struct lock_class *class = hlock_class(lock);

	printk(" Possible unsafe locking scenario:\n\n");
	printk("       CPU0\n");
	printk("       ----\n");
	printk("  lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_usage_bug(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit prev_bit, enum lock_usage_bit new_bit)
{
	if (!debug_locks_off() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("================================\n");
	pr_warn("WARNING: inconsistent lock state\n");
	print_kernel_ident();
	pr_warn("--------------------------------\n");

	pr_warn("inconsistent {%s} -> {%s} usage.\n",
		usage_str[prev_bit], usage_str[new_bit]);

	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] takes:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		lockdep_softirq_context(curr), softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		lockdep_softirqs_enabled(curr));
	print_lock(this);

	pr_warn("{%s} state was registered at:\n", usage_str[prev_bit]);
	print_lock_trace(hlock_class(this)->usage_traces[prev_bit], 1);

	print_irqtrace_events(curr);
	pr_warn("\nother info that might help us debug this:\n");
	print_usage_bug_scenario(this);

	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Print out an error if an invalid bit is set:
 */
static inline int
valid_state(struct task_struct *curr, struct held_lock *this,
	    enum lock_usage_bit new_bit, enum lock_usage_bit bad_bit)
{
	if (unlikely(hlock_class(this)->usage_mask & (1 << bad_bit))) {
		graph_unlock();
		print_usage_bug(curr, this, bad_bit, new_bit);
		return 0;
	}
	return 1;
}


/*
 * print irq inversion bug:
 */
static void
print_irq_inversion_bug(struct task_struct *curr,
			struct lock_list *root, struct lock_list *other,
			struct held_lock *this, int forwards,
			const char *irqclass)
{
	struct lock_list *entry = other;
	struct lock_list *middle = NULL;
	int depth;

	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("========================================================\n");
	pr_warn("WARNING: possible irq lock inversion dependency detected\n");
	print_kernel_ident();
	pr_warn("--------------------------------------------------------\n");
	pr_warn("%s/%d just changed the state of lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(this);
	if (forwards)
		pr_warn("but this lock took another, %s-unsafe lock in the past:\n", irqclass);
	else
		pr_warn("but this lock was taken by another, %s-safe lock in the past:\n", irqclass);
	print_lock_name(other->class);
	pr_warn("\n\nand interrupts could create inverse lock ordering between them.\n\n");

	pr_warn("\nother info that might help us debug this:\n");

	/* Find a middle lock (if one exists) */
	depth = get_lock_depth(other);
	do {
		if (depth == 0 && (entry != root)) {
			pr_warn("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}
		middle = entry;
		entry = get_lock_parent(entry);
		depth--;
	} while (entry && entry != root && (depth >= 0));
	if (forwards)
		print_irq_lock_scenario(root, other,
			middle ? middle->class : root->class, other->class);
	else
		print_irq_lock_scenario(other, root,
			middle ? middle->class : other->class, root->class);

	lockdep_print_held_locks(curr);

	pr_warn("\nthe shortest dependencies between 2nd lock and 1st lock:\n");
	root->trace = save_trace();
	if (!root->trace)
		return;
	print_shortest_lock_dependencies(other, root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Prove that in the forwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_forwards(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_root(&root, this);
	ret = find_usage_forwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(read_bit));
	}

	return 0;
}

/*
 * Prove that in the backwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_backwards(struct task_struct *curr, struct held_lock *this,
		      enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_rootb(&root, this);
	ret = find_usage_backwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(read_bit));
	}

	return 0;
}

void print_irqtrace_events(struct task_struct *curr)
{
	const struct irqtrace_events *trace = &curr->irqtrace;

	printk("irq event stamp: %u\n", trace->irq_events);
	printk("hardirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->hardirq_enable_event, (void *)trace->hardirq_enable_ip,
		(void *)trace->hardirq_enable_ip);
	printk("hardirqs last disabled at (%u): [<%px>] %pS\n",
		trace->hardirq_disable_event, (void *)trace->hardirq_disable_ip,
		(void *)trace->hardirq_disable_ip);
	printk("softirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->softirq_enable_event, (void *)trace->softirq_enable_ip,
		(void *)trace->softirq_enable_ip);
	printk("softirqs last disabled at (%u): [<%px>] %pS\n",
		trace->softirq_d