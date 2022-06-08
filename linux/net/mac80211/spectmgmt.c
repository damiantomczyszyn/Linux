turns true, we skip this
		 *         lock (and any path this lock is in).
		 */
		if (skip && skip(lock, data))
			continue;

		if (match(lock, data)) {
			*target_entry = lock;
			return BFS_RMATCH;
		}

		/*
		 * Step 4: if not match, expand the path by adding the
		 *         forward or backwards dependencies in the search
		 *
		 */
		first = true;
		head = get_dep_list(lock, offset);
		list_for_each_entry_rcu(entry, head, entry) {
			visit_lock_entry(entry, lock);

			/*
			 * Note we only enqueue the first of the list into the
			 * queue, because we can always find a sibling
			 * dependency from one (see __bfs_next()), as a result
			 * the space of queue is saved.
			 */
			if (!first)
				continue;

			first = false;

			if (__cq_enqueue(cq, entry))
				return BFS_EQUEUEFULL;

			cq_depth = __cq_get_elem_count(cq);
			if (max_bfs_queue_depth < cq_depth)
				max_bfs_queue_depth = cq_depth;
		}
	}

	return BFS_RNOMATCH;
}

static inline enum bfs_result
__bfs_forwards(struct lock_list *src_entry,
	       void *data,
	       bool (*match)(struct lock_list *entry, void *data),
	       bool (*skip)(struct lock_list *entry, void *data),
	       struct lock_list **target_entry)
{
	return __bfs(src_entry, data, match, skip, target_entry,
		     offsetof(struct lock_class, locks_after));

}

static inline enum bfs_result
__bfs_backwards(struct lock_list *src_entry,
		void *data,
		bool (*match)(struct lock_list *entry, void *data),
	       bool (*skip)(struct lock_list *entry, void *data),
		struct lock_list **target_entry)
{
	return __bfs(src_entry, data, match, skip, target_entry,
		     offsetof(struct lock_class, locks_before));

}

static void print_lock_trace(const struct lock_trace *trace,
			     unsigned int spaces)
{
	stack_trace_print(trace->entries, trace->nr_entries, spaces);
}

/*
 * Print a dependency chain entry (this is only done when a deadlock
 * has been detected):
 */
static noinline void
print_circular_bug_entry(struct lock_list *target, int depth)
{
	if (debug_locks_silent)
		return;
	printk("\n-> #%u", depth);
	print_lock_name(target->class);
	printk(KERN_CONT ":\n");
	print_lock_trace(target->trace, 6);
}

static void
print_circular_lock_scenario(struct held_lock *src,
			     struct held_lock *tgt,
			     struct lock_list *prt)
{
	struct lock_class *source = hlock_class(src);
	struct lock_class *target = hlock_class(tgt);
	struct lock_class *parent = prt->class;

	/*
	 * A direct locking problem where unsafe_class lock is taken
	 * directly by safe_class lock, then all we need to show
	 * is the deadlock scenario, as it is obvious that the
	 * unsafe lock is taken under the safe lock.
	 *
	 * But if there is a chain instead, where the safe lock takes
	 * an intermediate lock (middle_class) where this lock is
	 * not the same as the safe lock, then the lock chain is
	 * used to describe the problem. Otherwise we would need
	 * to show a different CPU case for each link in the chain
	 * from the safe_class lock to the unsafe_class lock.
	 */
	if (parent != source) {
		printk("Chain exists of:\n  ");
		__print_lock_name(source);
		printk(KERN_CONT " --> ");
		__print_lock_name(parent);
		printk(KERN_CONT " --> ");
		__print_lock_name(target);
		printk(KERN_CONT "\n\n");
	}

	printk(" Possible unsafe locking scenario:\n\n");
	printk("       CPU0                    CPU1\n");
	printk("       ----                    ----\n");
	printk("  lock(");
	__print_lock_name(target);
	printk(KERN_CONT ");\n");
	printk("                               lock(");
	__print_lock_name(parent);
	printk(KERN_CONT ");\n");
	printk("                               lock(");
	__print_lock_name(target);
	printk(KERN_CONT ");\n");
	printk("  lock(");
	__print_lock_name(source);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

/*
 * When a circular dependency is detected, print the
 * header first:
 */
static noinline void
print_circular_bug_header(struct lock_list *entry, unsigned int depth,
			struct held_lock *check_src,
			struct held_lock *check_tgt)
{
	struct task_struct *curr = current;

	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("======================================================\n");
	pr_warn("WARNING: possible circular locking dependency detected\n");
	print_kernel_ident();
	pr_warn("------------------------------------------------------\n");
	pr_warn("%s/%d is trying to acquire lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(check_src);

	pr_warn("\nbut task is already holding lock:\n");

	print_lock(check_tgt);
	pr_warn("\nwhich lock already depends on the new lock.\n\n");
	pr_warn("\nthe existing dependency chain (in reverse order) is:\n");

	print_circular_bug_entry(entry, depth);
}

/*
 * We are about to add A -> B into the dependency graph, and in __bfs() a
 * strong dependency path A -> .. -> B is found: hlock_class equals
 * entry->class.
 *
 * If A -> .. -> B can replace A -> B in any __bfs() search (means the former
 * is _stronger_ than or equal to the latter), we consider A -> B as redundant.
 * For example if A -> .. -> B is -(EN)-> (i.e. A -(E*)-> .. -(*N)-> B), and A
 * -> B is -(ER)-> or -(EN)->, then we don't need to add A -> B into the
 * dependency graph, as any strong path ..-> A -> B ->.. we can get with
 * having dependency A -> B, we could already get a equivalent path ..-> A ->
 * .. -> B -> .. with A -> .. -> B. Therefore A -> B is redundant.
 *
 * We need to make sure both the start and the end of A -> .. -> B is not
 * weaker than A -> B. For the start part, please see the comment in
 * check_redundant(). For the end part, we need:
 *
 * Either
 *
 *     a) A -> B is -(*R)-> (everything is not weaker than that)
 *
 * or
 *
 *     b) A -> .. -> B is -(*N)-> (nothing is stronger than this)
 *
 */
static inline bool hlock_equal(struct lock_list *entry, void *data)
{
	struct held_lock *hlock = (struct held_lock *)data;

	return hlock_class(hlock) == entry->class && /* Found A -> .. -> B */
	       (hlock->read == 2 ||  /* A -> B is -(*R)-> */
		!entry->only_xr); /* A -> .. -> B is -(*N)-> */
}

/*
 * We are about to add B -> A into the dependency graph, and in __bfs() a
 * strong dependency path A -> .. -> B is found: hlock_class equals
 * entry->class.
 *
 * We will have a deadlock case (conflict) if A -> .. -> B -> A is a strong
 * dependency cycle, that means:
 *
 * Either
 *
 *     a) B -> A is -(E*)->
 *
 * or
 *
 *     b) A -> .. -> B is -(*N)-> (i.e. A -> .. -(*N)-> B)
 *
 * as then we don't have -(*R)-> -(S*)-> in the cycle.
 */
static inline bool hlock_conflict(struct lock_list *entry, void *data)
{
	struct held_lock *hlock = (struct held_lock *)data;

	return hlock_class(hlock) == entry->class && /* Found A -> .. -> B */
	       (hlock->read == 0 || /* B -> A is -(E*)-> */
		!entry->only_xr); /* A -> .. -> B is -(*N)-> */
}

static noinline void print_circular_bug(struct lock_list *this,
				struct lock_list *target,
				struct held_lock *check_src,
				struct held_lock *check_tgt)
{
	struct task_struct *curr = current;
	struct lock_list *parent;
	struct lock_list *first_parent;
	int depth;

	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	this->trace = save_trace();
	if (!this->trace)
		return;

	depth = get_lock_depth(target);

	print_circular_bug_header(target, depth, check_src, check_tgt);

	parent = get_lock_parent(target);
	first_parent = parent;

	while (parent) {
		print_circular_bug_entry(parent, --depth);
		parent = get_lock_parent(parent);
	}

	printk("\nother info that might help us debug this:\n\n");
	print_circular_lock_scenario(check_src, check_tgt,
				     first_parent);

	lockdep_print_held_locks(curr);

	printk("\nstack backtrace:\n");
	dump_stack();
}

static noinline void print_bfs_bug(int ret)
{
	if (!debug_locks_off_graph_unlock())
		return;

	/*
	 * Breadth-first-search failed, graph got corrupted?
	 */
	WARN(1, "lockdep bfs error:%d\n", ret);
}

static bool noop_count(struct lock_list *entry, 