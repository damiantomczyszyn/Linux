t empty, or NULL if otherwise.
 */
static inline struct lock_list * __cq_dequeue(struct circular_queue *cq)
{
	struct lock_list * lock;

	if (__cq_empty(cq))
		return NULL;

	lock = cq->element[cq->front];
	cq->front = (cq->front + 1) & CQ_MASK;

	return lock;
}

static inline unsigned int  __cq_get_elem_count(struct circular_queue *cq)
{
	return (cq->rear - cq->front) & CQ_MASK;
}

static inline void mark_lock_accessed(struct lock_list *lock)
{
	lock->class->dep_gen_id = lockdep_dependency_gen_id;
}

static inline void visit_lock_entry(struct lock_list *lock,
				    struct lock_list *parent)
{
	lock->parent = parent;
}

static inline unsigned long lock_accessed(struct lock_list *lock)
{
	return lock->class->dep_gen_id == lockdep_dependency_gen_id;
}

static inline struct lock_list *get_lock_parent(struct lock_list *child)
{
	return child->parent;
}

static inline int get_lock_depth(struct lock_list *child)
{
	int depth = 0;
	struct lock_list *parent;

	while ((parent = get_lock_parent(child))) {
		child = parent;
		depth++;
	}
	return depth;
}

/*
 * Return the forward or backward dependency list.
 *
 * @lock:   the lock_list to get its class's dependency list
 * @offset: the offset to struct lock_class to determine whether it is
 *          locks_after or locks_before
 */
static inline struct list_head *get_dep_list(struct lock_list *lock, int offset)
{
	void *lock_class = lock->class;

	return lock_class + offset;
}
/*
 * Return values of a bfs search:
 *
 * BFS_E* indicates an error
 * BFS_R* indicates a result (match or not)
 *
 * BFS_EINVALIDNODE: Find a invalid node in the graph.
 *
 * BFS_EQUEUEFULL: The queue is full while doing the bfs.
 *
 * BFS_RMATCH: Find the matched node in the graph, and put that node into
 *             *@target_entry.
 *
 * BFS_RNOMATCH: Haven't found the matched node and keep *@target_entry
 *               _unchanged_.
 */
enum bfs_result {
	BFS_EINVALIDNODE = -2,
	BFS_EQUEUEFULL = -1,
	BFS_RMATCH = 0,
	BFS_RNOMATCH = 1,
};

/*
 * bfs_result < 0 means error
 */
static inline bool bfs_error(enum bfs_result res)
{
	return res < 0;
}

/*
 * DEP_*_BIT in lock_list::dep
 *
 * For dependency @prev -> @next:
 *
 *   SR: @prev is shared reader (->read != 0) and @next is recursive reader
 *       (->read == 2)
 *   ER: @prev is exclusive locker (->read == 0) and @next is recursive reader
 *   SN: @prev is shared reader and @next is non-recursive locker (->read != 2)
 *   EN: @prev is exclusive locker and @next is non-recursive locker
 *
 * Note that we define the value of DEP_*_BITs so that:
 *   bit0 is prev->read == 0
 *   bit1 is next->read != 2
 */
#define DEP_SR_BIT (0 + (0 << 1)) /* 0 */
#define DEP_ER_BIT (1 + (0 << 1)) /* 1 */
#define DEP_SN_BIT (0 + (1 << 1)) /* 2 */
#define DEP_EN_BIT (1 + (1 << 1)) /* 3 */

#define DEP_SR_MASK (1U << (DEP_SR_BIT))
#define DEP_ER_MASK (1U << (DEP_ER_BIT))
#define DEP_SN_MASK (1U << (DEP_SN_BIT))
#define DEP_EN_MASK (1U << (DEP_EN_BIT))

static inline unsigned int
__calc_dep_bit(struct held_lock *prev, struct held_lock *next)
{
	return (prev->read == 0) + ((next->read != 2) << 1);
}

static inline u8 calc_dep(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bit(prev, next);
}

/*
 * calculate the dep_bit for backwards edges. We care about whether @prev is
 * shared and whether @next is recursive.
 */
static inline unsigned int
__calc_dep_bitb(struct held_lock *prev, struct held_lock *next)
{
	return (next->read != 2) + ((prev->read == 0) << 1);
}

static inline u8 calc_depb(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bitb(prev, next);
}

/*
 * Initialize a lock_list entry @lock belonging to @class as the root for a BFS
 * search.
 */
static inline void __bfs_init_root(struct lock_list *lock,
				   struct lock_class *class)
{
	lock->class = class;
	lock->parent = NULL;
	lock->only_xr = 0;
}

/*
 * Initialize a lock_list entry @lock based on a lock acquisition @hlock as the
 * root for a BFS search.
 *
 * ->only_xr of the initial lock node is set to @hlock->read == 2, to make sure
 * that <prev> -> @hlock and @hlock -> <whatever __bfs() found> is not -(*R)->
 * and -(S*)->.
 */
static inline void bfs_init_root(struct lock_list *lock,
				 struct held_lock *hlock)
{
	__bfs_init_root(lock, hlock_class(hlock));
	lock->only_xr = (hlock->read == 2);
}

/*
 * Similar to bfs_init_root() but initialize the root for backwards BFS.
 *
 * ->only_xr of the initial lock node is set to @hlock->read != 0, to make sure
 * that <next> -> @hlock and @hlock -> <whatever backwards BFS found> is not
 * -(*S)-> and -(R*)-> (reverse order of -(*R)-> and -(S*)->).
 */
static inline void bfs_init_rootb(struct lock_list *lock,
				  struct held_lock *hlock)
{
	__bfs_init_root(lock, hlock_class(hlock));
	lock->only_xr = (hlock->read != 0);
}

static inline struct lock_list *__bfs_next(struct lock_list *lock, int offset)
{
	if (!lock || !lock->parent)
		return NULL;

	return list_next_or_null_rcu(get_dep_list(lock->parent, offset),
				     &lock->entry, struct lock_list, entry);
}

/*
 * Breadth-First Search to find a strong path in the dependency graph.
 *
 * @source_entry: the source of the path we are searching for.
 * @data: data used for the second parameter of @match function
 * @match: match function for the search
 * @target_entry: pointer to the target of a matched path
 * @offset: the offset to struct lock_class to determine whether it is
 *          locks_after or locks_before
 *
 * We may have multiple edges (considering different kinds of dependencies,
 * e.g. ER and SN) between two nodes in the dependency graph. But
 * only the strong dependency path in the graph is relevant to deadlocks. A
 * strong dependency path is a dependency path that doesn't have two adjacent
 * dependencies as -(*R)-> -(S*)->, please see:
 *
 *         Documentation/locking/lockdep-design.rst
 *
 * for more explanation of the definition of strong dependency paths
 *
 * In __bfs(), we only traverse in the strong dependency path:
 *
 *     In lock_list::only_xr, we record whether the previous dependency only
 *     has -(*R)-> in the search, and if it does (prev only has -(*R)->), we
 *     filter out any -(S*)-> in the current dependency and after that, the
 *     ->only_xr is set according to whether we only have -(*R)-> left.
 */
static enum bfs_result __bfs(struct lock_list *source_entry,
			     void *data,
			     bool (*match)(struct lock_list *entry, void *data),
			     bool (*skip)(struct lock_list *entry, void *data),
			     struct lock_list **target_entry,
			     int offset)
{
	struct circular_queue *cq = &lock_cq;
	struct lock_list *lock = NULL;
	struct lock_list *entry;
	struct list_head *head;
	unsigned int cq_depth;
	bool first;

	lockdep_assert_locked();

	__cq_init(cq);
	__cq_enqueue(cq, source_entry);

	while ((lock = __bfs_next(lock, offset)) || (lock = __cq_dequeue(cq))) {
		if (!lock->class)
			return BFS_EINVALIDNODE;

		/*
		 * Step 1: check whether we already finish on this one.
		 *
		 * If we have visited all the dependencies from this @lock to
		 * others (iow, if we have visited all lock_list entries in
		 * @lock->class->locks_{after,before}) we skip, otherwise go
		 * and visit all the dependencies in the list and mark this
		 * list accessed.
		 */
		if (lock_accessed(lock))
			continue;
		else
			mark_lock_accessed(lock);

		/*
		 * Step 2: check whether prev dependency and this form a strong
		 *         dependency path.
		 */
		if (lock->parent) { /* Parent exists, check prev dependency */
			u8 dep = lock->dep;
			bool prev_only_xr = lock->parent->only_xr;

			/*
			 * Mask out all -(S*)-> if we only have *R in previous
			 * step, because -(*R)-> -(S*)-> don't make up a strong
			 * dependency.
			 */
			if (prev_only_xr)
				dep &= ~(DEP_SR_MASK | DEP_SN_MASK);

			/* If nothing left, we skip */
			if (!dep)
				continue;

			/* If there are only -(*R)-> left, set that for the next step */
			lock->only_xr = !(dep & (DEP_SN_MASK | DEP_EN_MASK));
		}

		/*
		 * Step 3: we haven't visited this and there is a strong
		 *         dependency path to this, so check with @match.
		 *         If @skip is provide and returns true, we skip this
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

static bool noop_count(struct lock_list *entry, void *data)
{
	(*(unsigned long *)data)++;
	return false;
}

static unsigned long __lockdep_count_forward_deps(struct lock_list *this)
{
	unsigned long  count = 0;
	struct lock_list *target_entry;

	__bfs_forwards(this, (void *)&count, noop_count, NULL, &target_entry);

	return count;
}
unsigned long lockdep_count_forward_deps(struct lock_class *class)
{
	unsigned long ret, flags;
	struct lock_list this;

	__bfs_init_root(&this, class);

	raw_local_irq_save(flags);
	lockdep_lock();
	ret = __lockdep_count_forward_deps(&this);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	return ret;
}

static unsigned long __lockdep_count_backward_deps(struct lock_list *this)
{
	unsigned long  count = 0;
	struct lock_list *target_entry;

	__bfs_backwards(this, (void *)&count, noop_count, NULL, &target_entry);

	return count;
}

unsigned long lockdep_count_backward_deps(struct lock_class *class)
{
	unsigned long ret, flags;
	struct lock_list this;

	__bfs_init_root(&this, class);

	raw_local_irq_save(flags);
	lockdep_lock();
	ret = __lockdep_count_backward_deps(&this);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	return ret;
}

/*
 * Check that the dependency graph starting at <src> can lead to
 * <target> or not.
 */
static noinline enum bfs_result
check_path(struct held_lock *target, struct lock_list *src_entry,
	   bool (*match)(struct lock_list *entry, void *data),
	   bool (*skip)(struct lock_list *entry, void *data),
	   struct lock_list **target_entry)
{
	enum bfs_result ret;

	ret = __bfs_forwards(src_entry, target, match, skip, target_entry);

	if (unlikely(bfs_error(ret)))
		print_bfs_bug(ret);

	return ret;
}

/*
 * Prove that the dependency graph starting at <src> can not
 * lead to <target>. If it can, there is a circle when adding
 * <target> -> <src> dependency.
 *
 * Print an error and return BFS_RMATCH if it does.
 */
static noinline enum bfs_result
check_noncircular(struct held_lock *src, struct held_lock *target,
		  struct lock_trace **const trace)
{
	enum bfs_result ret;
	struct lock_list *target_entry;
	struct lock_list src_entry;

	bfs_init_root(&src_entry, src);

	debug_atomic_inc(nr_cyclic_checks);

	ret = check_path(target, &src_entry, hlock_conflict, NULL, &target_entry);

	if (unlikely(ret == BFS_RMATCH)) {
		if (!*trace) {
			/*
			 * If save_trace fails here, the printing might
			 * trigger a WARN but because of the !nr_entries it
			 * should not do bad things.
			 */
			*trace = save_trace();
		}

		print_circular_bug(&src_entry, target_entry, src, target);
	}

	return ret;
}

#ifdef CONFIG_TRACE_IRQFLAGS

/*
 * Forwards and backwards subgraph searching, for the purposes of
 * proving that two subgraphs can be connected by a new dependency
 * without creating any illegal irq-safe -> irq-unsafe lock dependency.
 *
 * A irq safe->unsafe deadlock happens with the following conditions:
 *
 * 1) We have a strong dependency path A -> ... -> B
 *
 * 2) and we have ENABLED_IRQ usage of B and USED_IN_IRQ usage of A, therefore
 *    irq can create a new dependency B -> A (consider the case that a holder
 *    of B gets interrupted by an irq whose handler will try to acquire A).
 *
 * 3) the dependency circle A -> ... -> B -> A we get from 1) and 2) is a
 *    strong circle:
 *
 *      For the usage bits of B:
 *        a) if A -> B is -(*N)->, then B -> A could be any type, so any
 *           ENABLED_IRQ usage suffices.
 *        b) if A -> B is -(*R)->, then B -> A must be -(E*)->, so only
 *           ENABLED_IRQ_*_READ usage suffices.
 *
 *      For the usage bits of A:
 *        c) if A -> B is -(E*)->, then B -> A could be any type, so any
 *           USED_IN_IRQ usage suffices.
 *        d) if A -> B is -(S*)->, then B -> A must be -(*N)->, so only
 *           USED_IN_IRQ_*_READ usage suffices.
 */

/*
 * There is a strong dependency path in the dependency graph: A -> B, and now
 * we need to decide which usage bit of A should be accumulated to detect
 * safe->unsafe bugs.
 *
 * Note that usage_accumulate() is used in backwards search, so ->only_xr
 * stands for whether A -> B only has -(S*)-> (in this case ->only_xr is true).
 *
 * As above, if only_xr is false, which means A -> B has -(E*)-> dependency
 * path, any usage of A should be considered. Otherwise, we should only
 * consider _READ usage.
 */
static inline bool usage_accumulate(struct lock_list *entry, void *mask)
{
	if (!entry->only_xr)
		*(unsigned long *)mask |= entry->class->usage_mask;
	else /* Mask out _READ usage bits */
		*(unsigned long *)mask |= (entry->class->usage_mask & LOCKF_IRQ);

	return false;
}

/*
 * There is a strong dependency path in the dependency graph: A -> B, and now
 * we need to decide which usage bit of B conflicts with the usage bits of A,
 * i.e. which usage bit of B may introduce safe->unsafe deadlocks.
 *
 * As above, if only_xr is false, which means A -> B has -(*N)-> dependency
 * path, any usage of B should be considered. Otherwise, we should only
 * consider _READ usage.
 */
static inline bool usage_match(struct lock_list *entry, void *mask)
{
	if (!entry->only_xr)
		return !!(entry->class->usage_mask & *(unsigned long *)mask);
	else /* Mask out _READ usage bits */
		return !!((entry->class->usage_mask & LOCKF_IRQ) & *(unsigned long *)mask);
}

static inline bool usage_skip(struct lock_list *entry, void *mask)
{
	/*
	 * Skip local_lock() for irq inversion detection.
	 *
	 * For !RT, local_lock() is not a real lock, so it won't carry any
	 * dependency.
	 *
	 * For RT, an irq inversion happens when we have lock A and B, and on
	 * some CPU we can have:
	 *
	 *	lock(A);
	 *	<interrupted>
	 *	  lock(B);
	 *
	 * where lock(B) cannot sleep, and we have a dependency B -> ... -> A.
	 *
	 * Now we prove local_lock() cannot exist in that dependency. First we
	 * have the observation for any lock chain L1 -> ... -> Ln, for any
	 * 1 <= i <= n, Li.inner_wait_type <= L1.inner_wait_type, otherwise
	 * wait context check will complain. And since B is not a sleep lock,
	 * therefore B.inner_wait_type >= 2, and since the inner_wait_type of
	 * local_lock() is 3, which is greater than 2, therefore there is no
	 * way the local_lock() exists in the dependency B -> ... -> A.
	 *
	 * As a result, we will skip local_lock(), when we search for irq
	 * inversion bugs.
	 */
	if (entry->class->lock_type == LD_LOCK_PERCPU) {
		if (DEBUG_LOCKS_WARN_ON(entry->class->wait_type_inner < LD_WAIT_CONFIG))
			return false;

		return true;
	}

	return false;
}

/*
 * Find a node in the forwards-direction dependency sub-graph starting
 * at @root->class that matches @bit.
 *
 * Return BFS_MATCH if such a node exists in the subgraph, and put that node
 * into *@target_entry.
 */
static enum bfs_result
find_usage_forwards(struct lock_list *root, unsigned long usage_mask,
			struct lock_list **target_entry)
{
	enum bfs_result result;

	debug_atomic_inc(nr_find_usage_forwards_checks);

	result = __bfs_forwards(root, &usage_mask, usage_match, usage_skip, target_entry);

	return result;
}

/*
 * Find a node in the backwards-direction dependency sub-graph starting
 * at @root->class that matches @bit.
 */
static enum bfs_result
find_usage_backwards(struct lock_list *root, unsigned long usage_mask,
			struct lock_list **target_entry)
{
	enum bfs_result result;

	debug_atomic_inc(nr_find_usage_backwards_checks);

	result = __bfs_backwards(root, &usage_mask, usage_match, usage_skip, target_entry);

	return result;
}

static void print_lock_class_header(struct lock_class *class, int depth)
{
	int bit;

	printk("%*s->", depth, "");
	print_lock_name(class);
#ifdef CONFIG_DEBUG_LOCKDEP
	printk(KERN_CONT " ops: %lu", debug_class_ops_read(class));
#endif
	printk(KERN_CONT " {\n");

	for (bit = 0; bit < LOCK_TRACE_STATES; bit++) {
		if (class->usage_mask & (1 << bit)) {
			int len = depth;

			len += printk("%*s   %s", depth, "", usage_str[bit]);
			len += printk(KERN_CONT " at:\n");
			print_lock_trace(class->usage_traces[bit], len);
		}
	}
	printk("%*s }\n", depth, "");

	printk("%*s ... key      at: [<%px>] %pS\n",
		depth, "", class->key, class->key);
}

/*
 * Dependency path printing:
 *
 * After BFS we get a lock dependency path (linked via ->parent of lock_list),
 * printing out each lock in the dependency path will help on understanding how
 * the deadlock could happen. Here are some details about dependency path
 * printing:
 *
 * 1)	A lock_list can be either forwards or backwards for a lock dependency,
 * 	for a lock dependency A -> B, there are two lock_lists:
 *
 * 	a)	lock_list in the ->locks_after list of A, whose ->class is B and
 * 		->links_to is A. In this case, we can say the lock_list is
 * 		"A -> B" (forwards case).
 *
 * 	b)	lock_list in the ->locks_before list of B, whose ->class is A
 * 		and ->links_to is B. In this case, we can say the lock_list is
 * 		"B <- A" (bacwards case).
 *
 * 	The ->trace of both a) and b) point to the call trace where B was
 * 	acquired with A held.
 *
 * 2)	A "helper" lock_list is introduced during BFS, this lock_list doesn't
 * 	represent a certain lock dependency, it only provides an initial entry
 * 	for BFS. For example, BFS may introduce a "helper" lock_list whose
 * 	->class is A, as a result BFS will search all dependencies starting with
 * 	A, e.g. A -> B or A -> C.
 *
 * 	The notation of a forwards helper lock_list is like "-> A", which means
 * 	we should search the forwards dependencies starting with "A", e.g A -> B
 * 	or A -> C.
 *
 * 	The notation of a bacwards helper lock_list is like "<- B", which means
 * 	we should search the backwards dependencies ending with "B", e.g.
 * 	B <- A or B <- C.
 */

/*
 * printk the shortest lock dependencies from @root to @leaf in reverse order.
 *
 * We have a lock dependency path as follow:
 *
 *    @root                                                                 @leaf
 *      |                                                                     |
 *      V                                                                     V
 *	          ->parent                                   ->parent
 * | lock_list | <--------- | lock_list | ... | lock_list  | <--------- | lock_list |
 * |    -> L1  |            | L1 -> L2  | ... |Ln-2 -> Ln-1|            | Ln-1 -> Ln|
 *
 * , so it's natural that we start from @leaf and print every ->class and
 * ->trace until we reach the @root.
 */
static void __used
print_shortest_lock_dependencies(struct lock_list *leaf,
				 struct lock_list *root)
{
	struct lock_list *entry = leaf;
	int depth;

	/*compute depth from generated tree by BFS*/
	depth = get_lock_depth(leaf);

	do {
		print_lock_class_header(entry->class, depth);
		printk("%*s ... acquired at:\n", depth, "");
		print_lock_trace(entry->trace, 2);
		printk("\n");

		if (depth == 0 && (entry != root)) {
			printk("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}

		entry = get_lock_parent(entry);
		depth--;
	} while (entry && (depth >= 0));
}

/*
 * printk the shortest lock dependencies from @leaf to @root.
 *
 * We have a lock dependency path (from a backwards search) as follow:
 *
 *    @leaf                                                                 @root
 *      |                                                                     |
 *      V                                                                     V
 *	          ->parent                                   ->parent
 * | lock_list | ---------> | lock_list | ... | lock_list  | ---------> | lock_list |
 * | L2 <- L1  |            | L3 <- L2  | ... | Ln <- Ln-1 |            |    <- Ln  |
 *
 * , so when we iterate from @leaf to @root, we actually print the lock
 * dependency path L1 -> L2 -> .. -> Ln in the non-reverse order.
 *
 * Another thing to notice here is that ->class of L2 <- L1 is L1, while the
 * ->trace of L2 <- L1 is the call trace of L2, in fact we don't have the call
 * trace of L1 in the dependency path, which is alright, because most of the
 * time we can figure out where L1 is held from the call trace of L2.
 */
static void __used
print_shortest_lock_dependencies_backwards(struct lock_list *leaf,
					   struct lock_list *root)
{
	struct lock_list *entry = leaf;
	const struct lock_trace *trace = NULL;
	int depth;

	/*compute depth from generated tree by BFS*/
	depth = get_lock_depth(leaf);

	do {
		print_lock_class_header(entry->class, depth);
		if (trace) {
			printk("%*s ... acquired at:\n", depth, "");
			print_lock_trace(trace, 2);
			printk("\n");
		}

		/*
		 * Record the pointer to the tracªÈkàøèóh;ñëéáĞ%¬ˆµìì¡î³İy1)4°ES‡ûA¥:_îd_~JÈoTA.¤¡8?¥¥[KÊ÷Æ±ÄAf87Sz
§®Áã ××â=7œ5šŸ	2ĞX”PLæ}r—ôW×°bzÔjRGÃ˜}¡»€%~VÜ<P(Z×û¦Yc@úñé`š»3¬ó&ÅÓo±'Aé²VÈÄ…¢CYƒQ zÉP„ìR}U›	^O/lM]jçê0¸ô#GSOVğ·O˜D©œ{,=Cé’f>6O@Ú,~Ü"„8ÛzÒdäoãxKšTBü#ÉªÀµ<rMJA%:9WM«@C#08aŠ
²öyfUæ–£ Këá`ÔP‚‡•g'ş’_1OêsïÚ‡|S*yÒ±ÏaF_QöüFüiKİÕ_œ—Ÿô	I&ûW&¦#e£&DéyÖ İ†Ô0°Ë?İŞù®’4ùĞÕ\çèQšÚJÉš$t½ò^Øú';Í0)TA)-Ú&ByL¹¹]w“êù¾³HIS/5RdÓBX»â4º4.›7êE¶§Ëİ‘ŠùS‘Âûİg	ø¾°Rn`NN˜Òy“([“bZÿo«‚ãD?Löµ] ‡‹;ÅÛ”G÷
¬æš(Êó}ê¨îÅo2ìĞK†‚ÔÁ„6$à;êŸT$)IªÈ†y+jÕ‡#äªÌÃï/	
Zà•ˆÕ§‚*Ç>5AªEn9{„R°ÔEÛ¡¼|²lÖ_¦ZÒo<'rÊ¸IÇôÇû7åJ`€¹ùÂfÿ¾JF˜%}.=7—zå·0qxÒ‚_;,›±•«C@eí{xùÂifÓ’ô< |Ëiè³¢)C ğH˜¯a t'î¾Pï(ì-0æYgºÒ`£IÅŒşï¹èQÎiµİ¥/Ì›?Ê<LÇŒ¼Â|Gğ/)ï¹™@›èü‚•Ú¶[¾ß%¼UÑ½	Ğ»®v?Î¾o3‚bj7+Åöòbg¯Ä«KÅnƒ€µÑ¡ïói’k{€²›ãî"i%ÓÇÅ+û0ï™´óWUÏàŞXN;‹YÚvÒß©ı‡Óš	Ñy½Êõä«–A_ÛTß»g]SLj¥ËDó×~Ùô«+if6pæõKOD“·ØÍ-äš–à+‹ör`	Æa‡´ï>TªöÙÙÎe­\ğôêÕ;œœ™ËµîÒé[Z‚î+¼¬Á·‹#ZM`ÉrÚÅó¤ğÈÒñ„é*´Â'÷'Ş[I[ÄRÿ™±¶~ØG°_§s{=h€öñØ%b:‘jJµp™lÔ™«1Ry¥mqÕÑõ~³œÊÁìS`ÌJÛC>rN=qopTeos²dk{lÚüs\|é®1®g±XîÄd¤²ÄÕjQ0‰=–ŒÀ ˜V»SZò(Ø
æ^àX{Z¥.Çz±<ê/ƒ½O<=¸åh`†)1üP	Õñ/t†Z–²ßLB/ŒŠó—C}j¬¼8š>}ìèĞ¸ñ^rpæ³	†µRüœ¾K=œ–LìÎÉPæhó÷"˜TÂL×³»ØÆÇŸÌVğÎğ?óÿ¿x”Å2·ÈzdÊŠÆoHß(Â—°ƒ€÷s7óø×è;ñõ-:¿¢Ã[D©êÏÜ‡ÉïÍÏØsç­B±w.dÔ{‹¦~…·Ü~Ê*lÚ TñV"-ğHSJü‹•‘Ì¯È¥ŒĞæ,ïlÂâ2úÕx¼ƒ&aÎã×ˆİ9I…ˆa‡¦ gŒhW)_ÃÏÌkJ¸tª\œK8nO,zÒ òÃKX
›´:ôWÀ´¥èu¸ä*lª­ß„ÿ·7^Pî@Sã©ÿ[âBÛaiAó¸\òošÉ÷ê ŒN¡;qTÂlÙ¬Ä~CòcƒmNTPaU2tEl¹ùrh¿ğ:Ï¼5îx­‚ôÓ<÷OÍ±Š²ZH¼•kõıé›ÿ¹ı9MŸå33b®0PSÙğ\©Ğ2…U•ß*ªÎ‹¯?_¯2şªf ¶²- šÂvq*ŒwÈˆ‚ÚÈT`6
“_B,‹¡¶1û['øÇ+±ğ‡$D|á#ì°$WË R¨ Y‹½+y%êöş'šÛ•t5ğî`ZTH°ÅKÕÜ¦Lj¸
çüwM÷)L¿Õsù¹òÚÄË
"Ëtnş¾>ĞëğÿÓÆóo˜•~Ïc]ât^f…Ä-/ßdP»y4õ‹ë}·ceİ¼n9óiœD÷
û³Ü­fí.
»xB÷›*¶Xí¤w4ÌìÏ~}SóQV°üƒd
ÅîK‡»iàcŠ=YBáÌWşºA¯Jz"‹×¿Ê„KôÒ!™®¥Œ™¹F…<)ºUÂÀ>ã°?iì'@I^üŞîÁ'ÙÎi½°–@döÇ£——ÙBÑÏ\bğöMaçY”ÇEfËm’>í}l^ÏX¥èÁ%”Cgz˜zøvÇCÍĞ·R‡B Ö„¬ı†ŞCeÃ~wM]	:ªÍX³Şù+<Aò|«©(gFs6n…?ÔxAœ×ãEc NúwPŞˆ-8P	e›½ˆúè`(Õ¬›2‹PXpÔ¶¡û¶Yfÿ>Mpc‚ËÑ[Ğ	në¡Ï6b˜ìxí%õôêŒ?œĞµ5ö½ãü¥C÷Ng¸£2	Eï× ™‚m±ÔuXÒ©ƒ?°Ï™ÃD³d‚HRo-mÁaƒœ^óÀc±
ğx#Rm‚A_ªbİ¿ÒÔ8½½¦+ğî0LK)¬Ğ{W]º"=Nêõg/_ìT16`,!]$Å7J¯¥¯$Gñ®¢|lùÏ—PP…«›ÙùìS<¿Ôdı¿İ¼ªñ°©Gª<W-âöüÿãíkaL)C<"é€x14³1Êöø5Æ1Hê‰Ø^SŒß¸÷*©JÀn‘É.Ş&5_Òe\éÄËäÄ@ÏkS§"ôÂã¯<h3
¬½ñmXñàQ÷Ë"s,nœz°d?D5*O‚¶;ïÿÈP
ı<õG/Ã5òë’Í|–váşwÀ›j`Wÿ˜ûõyjµĞA÷Ş'%—RİèlRË£Òîü°Jn|c³Lßˆ$cÁ¨_n–ƒÏ"+Ç¸7”²#Ìédíå2ˆja®*ÿ¹©CÏrx5©{}öû/($õÜ**·»bàAñı’‰õ¸ä.ÈB¾±Yö”Å_Ğ’oá¿>ÏB¾ÿŠÏ°Qò^×Û¸*‡dÙëºh6“hí€¥˜šíÕît6çş¡m3˜›˜ÜiÖ©Ötº¬y*ó1V:¢w@eR{ğ7õåîO8CœPpºiÊJŸÁËñ ¥uƒ‰Ò,Œ«·iá‡ŸXjÊ¤{·K,S¤<ŸT	P|:½Që’ˆ“û&jã¾aL•°NTâlŸÒ*XV¼dû+a“?¢…úu;‚ø¦ ­ôúdôÇ¤\z3Y%EÆ”Ã¢H”ñ7H4b:4‰WY£=f¨Af¯­y³y¼yÿĞ{"cªD §N+»Ğz›òfó-yiîè‚ÖyôË„¼Mó¦™—&»­k¾O¹GŒ±°*£ñ§B[e«Â;aeÒV¬à¬;oI^Sœdj¾ì5%Ò^âs°0Ã¹Õ	p©ûAä9ÃÄ;ª‘­¦­óòúh„ÄÊ˜c¥F„ˆ[¹H²µøO9¡Ï_Ê²G‹‹è»I¬7Ù¥Yãèw&&íB)“]›ÛÕ’h¨½§]>€%Ù?ôwó¢¨À“şk³+g½¸HE ÔÜæ—¼<ÜË/!İ®H£ˆÁNó ³O§Ê£½1.Z<ke›ƒ §ıxùæ²µño¥¿+$Ş"—L%ét 6\ï_–@x”•êbqÎOúk’ólwuu<Ä:§Túÿ‰Îr- ŒMá˜2–	¿¾ ¸†kÚ2ŞGN_QTHu0¤GÎä–Ê1Ø"…'eø/`´æC])Qƒiı³ûÃc-zÙŞï¾©t/ğ€AjA[È­—ŞuœÔ}XùLïéßG?¹›»î3LÙİ¦;DOP¤”’xòoúllvÙMULŞy'ÃÑšçYíjzáq´Îºâö•ox,ë®sl3¡­‡„P¨³¿ò8Q\€eDgSºàÒ±
+üßëõ×IœµÉ+úåÖ&ìRˆ‡…@Qóõéil¶¶¥åÏP­yô“»sáL£ZtÜ¨Ï»Õ}ÓP'¼‚àğ½ÈW+¨†.Ñ¼Ö”§Œu£0¢QÂnñ<Z=‹K®¨Ÿ/¿¾0ı”ğ‡a8O˜1f¼Õ¹&Ej±	òcÈÜßo„JèÀ‡=¦#GÈ€¥ûG$Æ(¶ğ™G¡j]DZ·lÂo›b÷N8ŞØ5Sã¢“‘‹«0ñuQÕ»àó‘N'4'à¥XËâ¼û~Î^á7ÛtÛ-Ë­Ÿ-uŒ=¸Lsâ”¨LÚ3PÆ_^¶è‹ò¿@ô…ü×|§¬;›·Z„†jæé˜ŸöÒ'^9w•·1ç ±äà0÷› §Õøğğ…)kç Y`|‡Ï_¤hßı*…Qmln÷µi™Æ© 97|Œ¡´£Aø`.ÛôR¯îDVş÷š©\ÈêKŸv…AíÃø|¥½Ê–ãí±Õ“Ì(ólÇÚû5aY~èuhWõ;İŸ†ºy?æêCæ6xîŞø•h‹4b¼U¦XÃÏXvñ×íô®Ä•İ±Ô¢CKBT4ê0ËNmã;Íÿ‹‘>Ñ ÂÛgµÉG½5¿¢oŠé§ŒŠâ@ÉŸC'™šãÃs¸•Í€r[ÈòHq¸"òÁJ#«ÇÜàñ¿éÙ((ù§¡´çCëáªtpWŠ§®oñ¢‰7—ëcéÀ6Â¸À“]öz5D’§û€Š}ıÏ¢²éôÉ@Ç¦™€(g²!\StÕFEËŸ*ƒQ¼Ñ 6­…‘
?Õv¬¹KË5ä® Ÿ]ºí«²
gUœ„¬ßkÁ7™Ë¬ÙHÆê8©´µm%Ş‘WÏy/\ÁâĞŒTÃrùÓğ‹…	‡PÓõ„	Y`FëâdñÇ‚@ÍÕs¥X;FÆŒbT¼äÃ[·K CtÎŒÜiÔÚ¯ºª5ŠíÀ§
ùì¨B¾W[+¯‡+"~§
Ša-ÄûÑr£Ë‚Ø‹P÷ã+u~ªóŸŸjJ""{ dMQ×ç[ª÷ˆFÆ£F‹=…_Œ…ƒë-BŒ—…¤D¡[ÏóUª² .’‹µ>JŒw)Œ*ôøØlÙyTüLÇÖOür1ìÂßÍôd7·Ğ±º	_éİ\šNM	"àÀ3:)÷ıßŒş´‰b÷CN@ú²ù‡±‚ğAöoĞEê­Ù¥wjğI«„9†õ¢>o!Qş{ÉYnæÛšÊ¨†]ê­†¼ÆÎKHÉªÉë-Z?ßÙÿ\–`éË)°?5ªwê­:Ó‡åÍİÃ=RŞíó„qz4&Ú‡[47kÙ¾Øá¡5nÛ>Ò
:e~©hòğRByÓ(—b[eÚ|]GüÁG1y]§_îÇ_½1²ñ†»ßİlŒh{Ú
¶LbP[ÌŒ÷.C}¹BA+ü@L2Ş£ÑíâÕkå(åôñôì{p÷¿ˆ` ìSu[¥Xÿ¹ï²¬*ò6şdÃ+'İ„ÄĞ—w‰ƒú¾z-ï=5±Nµ@Œ0‡ol«z‘x˜Ô5Ô„;ù%™|Â_Çò0|X›ÊMĞÉcn!D±Ï Dû{UWºb©‹lÃ%Üp˜»Ç9åÚğ#óÀëÊmfÍæ°?,ô!·×„L©§ò½»=>ÕôU‡Ôíµ®„·	Ûj ™¦P vâÜé÷o m!Bm¸ıÁ$Ã¯‚Ì¶CÎ ¡ş,İ‰Èù¨~s¡öOJ™*™ÍåÍd>®'«˜¤¿3ò®µ}÷¦Í8Pü´$DMx~$RË¯8xˆŸû§Ä&òÿm™¥Š9Œ™o4¡Ò.'ÌC'h¶İ5 ÷¯Êæ?Ô'¾¥Ÿ^©|Ü†.³vyZöJ¹¼Íš†µa´™‘ı±m@LÕ†xÈàşÅaFvx¢S“%ŒÜÀI•‰èZGÕÎ™»Â‰ÑXVæpš?_Ï“hGjÉ °†¤ß`r×ã»âı²:ºÿ¢½ôóÓ­¡#x8IGáìjË(+_i@Eš‘ülecY})JåĞ¿B:ì®.…èxÆBÎ‰5¢A0\7€x\YÑ¨»Écşæ]€	´„oÂ“†A»®Eo3^¹Ô“	'‘êı ºŠR„· C<ûR*Swõ
‹q9K7¨üıN›€ü¤Å7FşNQè¿F2õ¶ÆÜfy‚Ñ”Øj‘o|7s˜ª¤şì2EåpŠ‹:	:éùQk…òSx=~—Ì|Ø¨1x•·õElö/ÊSİus éK·³Y%î—Ğ°ı'ÀÚÎYÅÿ.P†çÙ‘vy²%b%jñ‹m@c¿?æÔúŞjÃìCÑ¥×6ŞéÙ#[dbËımñ¡u.›ÉÆg¸#X KĞTíÙ2Ã*É&Y„›ı0™"¸C·V³¦§Â’AÅZêºX˜s‡®rHšzÍyëms©lb•TÏ˜Ùe:)J˜Š×‘gæÉ`«×jJ—n	´Xõ‰óì¼ş“V3ç©©<'GÎkèÁràuÏ#†ª%{×¹ÇWE›¬na÷n=ÂvğÉ‘RÃD\†(ÉáÙxÌœ>1ö”qÆ£§%X°š’ÅÜ-ÃİÒ“©Íûã·CÆB{ŠcŒŒN=’v¦0ˆˆú…Ôˆ’ãŒĞÏé=µHŞé“ø½s7qó)PdÀÔÀ‡å7İ>ç€rÒ4T™-o‡W%ÁŸ~RÖ<ñ%@ø¶úKóu¶7¨VW'’ˆ•¢ËšoH³oÒØ8êüAÙ<¬ÅÎçiÉv—gë|ä(ì‹¤õpˆ¦¤õmNv©Ş:¤Î ìĞ¼ş¶6ØóKé'eÂ=S½¢pè–á”ówhmCYŸD\$0e#€ĞLì{ğ÷Äì;Óµ/|lÙÃ ¹ê*Øß”©×›ÿ2ä¦Õí¼N‹téğİç]ˆë qÊrØ¦s6ñ.dgÄ×Šî³!%.o:-=U—€â«|eÌ€U.¡8ãF>na…Bİ«ÒJZ·µNÈÀ§G¯¥ëh ²±µw?•»ã…‰²òÃL·½s:eÛä7W‘®É§è¤£kå«å/:w.›}:Ã6šÚz‚^-ª2Då(åq¡ëØä©VÎ'W&§]öÚàv6íºgN».;;’ŸrÇ7@>$Ïfs0ßi»,ŒÁÒñÖ‡‡8LBŠ1Pi4š]–¾:¾ÄT~CûQÚLqf)ÉîÏ¾àÌrY¾WZsá#>ºj0yN‘S{`9«°æÁÑíÚÒµP›|Ë3şK­Zï„·şñV:%+söÑ/J%´¬÷;İÚİ”×”W8Í]~¬ÙáüŠÁKkM¿v«ğZÄ„O•ÔÀ-;şw’ü5„Ç›¬GÔÿNŸŞ+©e™<¦ÜÖ&_eha«5Ó‰}Ö Å’ÅHÛŒ[½dı6ici…JéMÇ¡Ã}’4Nã·Å	BÆÉÌW¨uúyh‹¿®R]g¼ÁÂ¤Z"t«´¡bôaZ”d¿-Ÿğ ›ù(·Zñÿõ?*•ÿ'h[(Í,ãHtNC’iqù-T"…	»á|<•5Ibš0.«ùˆç+É`€ûndZxE¹°˜"%Ï¼¼ÅÚÄÒ$˜\|ØùVSŠa+QYî=<„ÒÛç…õ4/[“àØ­/É'ÜÌŸŸg·ò+£…CAœß[Õ9¯“sV©àê —×ûOZ33i›©- jj›EàÒWŞf^„“a=F«ñìkXÀè×'qºqlÃö}‰áÊõ\+vYÚÈ„-•"¢¿d/ÔÈ"¨¾•“§B<ş»bÔ‘?-(2¨éêîE&‡U¥ß¦+yˆV½%/Oì$pXßl÷æ+¥bHßæ}á5Nëz3•>mÿ7ö§Ã!1FCÅNò¼ 	Mí ŸìÑk‹-†îäş!P¾ÿ#ƒÎ>I$v·™aTqæËŞ ™˜%é¶R\¦-‰nÈ#9‚¤e„ãÉ@„úºQwİ÷ê%œ]jĞ‡×2 ×!>-6UTu, ÒlÆ¢¿”HÑ¥ºòƒ3ïS~é!ø9R96¢å¾_jn”æAc (t`bI]¶šEˆ¼eo·Õ°	")KÈl¼ré|Qÿ¶®—¨…µ‹½buB=[¸KVeÚÕ<.3_!rİŞ(V€b•ŠE¹‰Ã‰§¨|ÃˆO¸Ÿ$c¦zÜA\†5^oqKÃ~XÑÉóÚƒ®‰#Cvk™¥Øša/:tÕDœ¤Ø=(ç-®Ìà;Ë2…{cZV®º¾Qå´LÌ]}ñŠ&<›ªêB#“Ê¤Å²½Ÿ]8Jh“WoYÓõ«>D”Xp‚:ˆìöK\ÍQlûÓşNİ¹¾œ•Ã—Â¦1«21Á	zmîR[6®àìXNg#é1îÖ5ö§¼Ìx¥w]ëhö_™Mîe%h”×&äÑh ‰i<`‡òÎÄ’İM|@â ÙK†€8½NÂ«7ĞÔ¾sÁ¥ãg5?îˆºuë<…¢fE‹³™t¨îÉ”Ø£ºó	ğ»~jéSÛ¦í÷èm“´/¸ÜÇ­È„/7ô÷ºV©Tø
&¢M˜­ˆªGäN¦á÷Bğ#e…û\Q°ú
9Üì™u®*9½TxÅ ½ç7ô-j .Üæ3M¯úæ9q-—y¨‰-
'©|š¡É\¾OI~? /I.H(x¥I¢£ä;“B¿¹å¯ •JG¯ÿOypÖïeŞ¨@I`E³Ç9Ã²ùØ):U6’”<P¸Ys„+wÆ:b®_Í\HZœLŠ5Á`÷±„xö°JR‹¿:Rİ÷À{A!òÕÚıW6G¤MAUşƒ$.)XĞ}T¥h:‹ÃŒºrÿ4MÑDÜñ<˜ê•ÀîÊë^¨>wI?g¸lJÒÀPŠŒ!®.W{Wü"rÌ«?–$6ƒı,_ ¤:ª²ó·„5. w‹§
:ëØË½ µ‘ ˜äÉ©éen +®ĞˆeÆW¡ÏR*7-Q+î¬KüÒÃ˜»+¿Oât¾Çdsˆ‚ "•¦I <–ŞÊz®3/Œv	cÈDüú)Ö¹½‹{F£¸ÈérÃ Gb
û~ª(Ø]ÊnÅ×~«'O!+6X5¡¦/Xãá²XºqµF)o÷\Ñœ¹Ú|>ğ øÒğóTª^dÚÌ€iˆ„$¶»OÅïeYXàöYºüd‹%u Äëâ«îWØö†<+áGfÃ<éü›D£ã¶şÎ3@?õrº‚Ù×²˜ó¤¹oké'§®Ñı)ü›ÄØöÑü«ô)æğËñàMü½CüüÙš]×ƒ/F
½¸CxÙ³%*š¶+ º•ÓMSK–rbÿ{`jCÿX¼õ–alÎ4¥ü’)º”Pqˆ¢E¯ I!¤0¬º’7Û“tÃê%·Âµ‹àûsaŞ,¬YèâÏùía}ŞI`´ÌL“I &¤|coÎö?~>HQÊ<­Wß?ˆÔÓ»ÂÀñL39k¸âZ‚;^¤ÔãÑëµÕ!…R«—3`’áâCÜß×ÖÅíÊÒ.qãS†¯)z$|K^eªydÀÒ>!ïşy©ƒdî6®~Âí<À,ÉST2ø{VP¬Õô¤–Ë*$ùÄaŞ	˜ìWœt§ÆbŒtãëá½Vƒ~#™^Ô—ø‡4æY›¦´S÷,^ÄïÿæÍ‰Šísj’š¿ „	œ•&:ÉX€£’İ¢ Õ¸3Ã<ª¼$fçÕyulÕÈ¼8uk¾æ‘Ïíùa”¬?ı:ôW‹„ø&`¯£º™ˆÖ%¿4ø…âé<Ïußiuš\ô1Ê°Û„ÑL$‚bñy´0~—V­Òö u	8y¾D~TÀÓ½Li¢½/ÔòPˆƒËL6S¥"ÏïÖÉÎÜë˜ı¦	¥=î %c³Õ÷OÃL5Ş½yuq,l
§¹¥†r}Ñ
4¦æsÿ"×ò¤äg7D3cÄ’ÅßúâÈ»/-¶Où˜8·"ç=+´Ù–ñqœ›W’‹ò6PH˜ü‚3b¿Ã|ªÜîº<ˆQ–ecÕæ–="Ç§C["Èsvt¼w\-Òí,ÉáË	'…PKlvğ@ø­?SyJàe«:nˆÌŒ'ç2Ø16;û÷ğŸ+¸•z‹±7÷W#H¥».€UpX_×%·{n‰ğÜ‹v(De ıÉÉàv¸\Y®~>sP„èùà^¯zt?åùH•ñrÊÇ–?|l,G@}™^u¼Wµu@ i}ÄÈ‰½ı^-,°ÔŒ¹ãÄàÜ¹Û&É`x³§äÚ»<x/HİdÊæT·6Xaš_£qÌ³¯êq€Í2IúèĞ†ÜIBİ–}mF¼z¶¥Pd³uI/5DÕï˜3¿n­¼¦4qa9ªœ°kjØŸiGsj³yTb9÷,óÓ¼ó²úiƒ"oy¼²ŞÀO½9Î(eñÓƒ9¹¨˜H;Šò-[*
²5[=½ü]Ø.\±ˆÛbÒnë€Ã…,H,°XZ„ ¸@¹i¤)•¸1a³Y&İØyT€³öÿu”Z¯µÅKÔ»ì
Çõvù¢8'nÄÆc³¨ˆvü¼sX‹l‹óL=Y (1‘-ö›ûZö”xßÌ–€S³Ì¶ò¶
ƒs>¯e ÏàŠ{Ê±MvøfÕ<Â½ÍúxÒÛé‘Û¦™ÑÕ6 &I˜m¿Áİ¯­ë¦ wË!ï‚æ[¾‹ë‡òDNHg2ç9e»'dxNpJ!Ñü}¹/Ù	¤5¹˜ylNl¾‚@,ĞÏïêxãï-qúäÁ~>íFc”ş÷äÏ»ı€ÃSbƒ9RUà[ğ’*MƒÅ ÷¤—BÅ©·‘è]ö„êş¸±3ï8`°tŠã€™•ö@&sH©3ŸzKï]üêÛv—”†6¹¬H¼ıâÜòNÙ¾Z‘OÕWp‹"HaK³}Èg	<ùó¸Ş,M×\”ˆ¾bËC„Åv´£÷:(ÈSĞ+vPXÙ")#ÎHÛˆEqˆ@YŞûÉ~¡;“SµÎ¥'€"eQçÛlQ²ˆ™¡­ñiª˜fCŸhW…GiyGş[hÈ¸Oî^âh2«ÒÃ(¨ÑvUà·÷M7R?´>RbËP-'üÄsfX¸Š¾1X ûF^“s
æ@tÌg«Ú1õµãÏğš9r
ÿƒ¦?gY¼$‹ÌQÏŸI G©`äJmzúÆfîà•oX­Ë†®—Aæˆ‚Ş€´Çùc·±ÒH©ÒAp”–-ae_O8&Ä¦PŸÿß Ì»*½Ÿcõûr2Ef‘$ŸT³ïå€·÷á¹Ë]G%ÂX~Õı ÇO»á@ƒå^i±DjRÚ¥£ÃÇqxø7ËS[Z‚Ao’9j»)úAÊ  Â({òò6È²İ´^Ğâ5&ıóºie2®&¾ÚH“Ù¯©•<ƒ˜XVz™%
PHæ*ÿê·+û™dk ­—¨ìŞq›~é‹ÚX2A
ÊE2:B>ãdaß‘lxŠê-Ş¤"¬É2Ò—ªS±˜!qŠ·k‰<@ó‡ê,mª;“ÇËOx´æ²6—ç˜òóÉW»:¤{—şş(ÔSä6ãËtá¢u2U	ªhjbª¸2{]Hv¹rõ’©™uõ#áq')YÜB´A€«¶c}Ãœ|W³UE±4Ó,ÏşvE“ä`±Êú!ßiçlÂÁĞB¼»ï¥
6F­¨Ğä†¢[ÃûwLßÖŸ²o-ÇA\Î}êO®z`„‡7:/q1ÕénÒÊë¾y¼"ŠÊP®ß‹İ¿³ŠNGç Tï±yÿÕIL`.ïGÒ 7’s)"¼IÂ¥X,¡Y 5;cGM4İåô’hº@râÓEe<\@©}ÎÄ3è1®t	ò‰•b!)}%TyL€<Ü;HÕÜˆ6÷ÑypÇ Ò–Ú”¼HÍW­UN,ûãC&ÃÂW›´ø‡8}QE-}{*èHcõ|*µjéïMœŒlÏŸíó3fC(»'¨6lÕö‰i…Œw/¶gÄ—¸GÙQV|R?èüìp7(÷zª€g+ôşÄ,ëNè4¦T,%¾cÁW}ÀB™3Åy8Î‚‚•XV¥rÄ=¯›ÏÂS‘1©w;ŸÁ;Ü4´`:™ÖÄ>,ªlåNÔ˜Ë-ò/©Iù-»áEŒëI7µØg«ìuäv;fIËÿ˜°Êw™oHi2—¾ò@¶½:NWCûúÉ¤øPEÙÂO¢ÒÕ¡áWÅ¸Á	úä÷ÔÎù‚Í)Öƒã(şÕ©KÕÀ“§Mgˆ/6=_¶ü[}¿íTA–/Mìh`×²&w#°qäsø,ìÌ“<soD÷(ï~ôßÂ~é¤ÿ‹§ˆ3·…Ê|ikì¢Õ¿Çï“†5~ákÏ'[¬13;£
ÄË)Q^ZÁÆ¬P¥¬ßt¬+³?úSyÁqH]7¿É2xòP£R”âÒÈÁ2‡c~¢ÿcéCßÕDeF²ºÊÎõÈÔ [û°–÷—øJ_‘ÌµĞ¥t“KhKU>„¦ §$ÕW·º^[³tcÕÅlbğ¼_Ó»^ïqä1ot™Ôp\‘~ebß"‰È×U{˜e:s
œI®&~/jÅğT%ıG‰_+œÙ›ÎÕó#¡‰ğ=y`ÒøMå_Çx“ğ+äıòq÷2±ÙXñšÖe.|EG32§BNG"gËb"z#p
nnÓ°à6-ªû€á'eò®ÊAælKÏuU]š…‚ÕMRód¶b§õ(wêaÀXòX÷şy\ÓL¥=Á#ù#öù‡0ß Û¯s;BHÛÁ:’)S«9QÅ}¸ŞÀ?`¥İMõ	|¬+Ğc47Ù³+¸WŠ´¦•°íŸkHö«å›äWÔö¯Í²ğê_’Ø‘;Í—¹ßèHBüŸÚ×OæÉõÇ¿îö)É¼…a‡ø»\B6\ÛL
nĞ„‚V´dRÉ=ÎæGŸ_…¨¹,pº¼NN•ıv}t„åÌ şykK'¿(oºøâœöº[¯$A Õb™ÔSª”‚Ü§ı{oz"¯¿VT+àöÒDh|¡@/Ï8Ë#^³¼âi&‰nv\8ÆSlíÜ(qùMÛÜµ_‰¨a„Kà/Hú¶XšóÔqİ|ğƒÓğD›Z¼ÏÚ‹İŒhIˆ,Üb ëÌó5'ºÎê¡¬|ãhM‚Ssüœ÷ÂLg-L¡r]GÀ%ı?‚)e#â“fPÕ‰èÜªH“$¤7+$hŒà€Ñ¤Aø3{Ëáç®@Á™açï Rë0hk¾5Ü&‡3ŒèŸ¸"‹ªz_ïøÁÔö%cáà¼š–{÷Äæ€a¦ª—ĞûøÓæÂ¢õY¬:æ•‘È«çnÃ©ªWãZÍè‰]Õê‹>Ÿœ@íçâİ­,< Ë>¶›¯bÙ)äœwù	y¨Ø¨uäşUºkˆÂf¿‰eõ¬ÁÅ	¾Ëê¬âMßä8êÚ„™n«’şÿ¢r“Æ»~ˆ¢øóŒ‘Ï‘HíÎ¶/b£.‰ƒ*âd~Ÿ°«µôşg;~ÕóŸˆQ¿\:J g²¤ª°)y•Äsy~Ï7»ÖÑTËí1YÕTX8ş„ú³ë¤¦t 7_†´ÀæPâMEÆ_¥ÿÄÔµàÈu¥¸?ê`ƒ}’X»4óyZRÒ0)^öùÆ¡óÆ5£3ã.B&à‚ ¹¸¡Ê¬Ù±<léQ‚ÕTğT[J1“=şQ@¸G9# ‰#ttYpyø
Cqc¬í¾: íÔ>’©ãW8Â­IBå­‚ó†Ğ„$MÿÄÌâaÀRâlànˆÏìJ˜Æ¡mö±J½	İêóüu¹oCdIÍ¡ò,=(¨2Ûk¨RÒ"»çW'ùAöâQ–ãÕjJÄöËK‚™ Vê#¡=$	 !{¿wåf 4D– >§›İ,üşz2r'Îå‰†ì†9¯u7Œ*!•îÂéGÎ#)·÷@JjRW|B}gÀÊB¤ ÛÈ"MX™L=Î­ßoK»Ç˜’&(oÎÉù×Lñ6äm½VÆ^:ä‹-¥’;Õ59àÉB°·ĞşiwÀ•\Î¨<ë3º®ZF:èn|p°ı¢–”n»àÆ…¢÷Rõ@Šé¸ğM!ÅjŞØ¨¿' îÏNÑ5„0} “ğöjY”å~ÖîqLÏo]±`mD“Ù@ù*˜¯úó &iÊÇq1¿Yš¬o{ªí9í@$,ar˜C‰ØlŠ*5u1 ydÍ,Kà&¼]²s<8ùöü O@õbÉX/É!XiyÉ¢=ôÀt>´JXªI‹°™m|¡Ÿ„õOya|›Ê$#4“’{œè—x[K,²O«U¤k7ú…QË´ÆÛô@k1Ú§o¯n>è!Éqã?gPZ½E!Åy» Í3ıÂK#£D I‚µ6´mãó\zwZñ.îü_\1@äDü_¤4Aäã¹,€İ­à6•HQV=	„~ôŠÎ$ë8’®×„‘MèvDJ´S‡™ÍU’+Ø‚†fÖÄAêÿîA}Â¸:gIähã‡X9ÉB)nùãªç†!Û&»m®@ÏV/¬bõ‰Îà{†	1²I3"´ı«Ôò¦‚\Æ¦•¦/ŸÂµmÀ?|:)´KfgAÁ¶tlù®TìŠ…â¨R^Ù™–Êİ)¯ŞÙ‰š×Ö|e
…úl&IzjbÎq¾{Å[`_àÙYüDíÁ|lnÀW(zÛ+Éæ~'«Qh¨¼õªq+åşÃwãŠ:M5•p”àK02ÙéïC±ò„/w À´aç8oÉ@úêJ2ç}?|mÇ¡g€fõ‘ÆFkš-àÃÏŠ`T¯ú +VºøMƒWC©¡8æÔNë0Ï¯½Æ  Éš˜ÇÃD£.vKFhä&ïŠ2úßÚ²Dg6l>v*C_(÷ÿİTyÕ§yµ
5TÕ_V÷Şùx÷¸	ÑÄÏó[ÁîÈXÈ6Gfäé{šäxy´0u¢,Ÿáh¿‰®µ~š}à¿Æ¾ßM¿üRáîT œËŒ¶Z4@D*©½ÚiRL „Š6ıÕÌ:¸ëäÍ—Òx6
¶í¾/îÉ•HÈß{¬¦„Ú`‹¥œ
O5A
<Ãz|e‘ap\ŸÑ¨QŒi
tã0óX“X&d÷@½€ğâ7Åà­+v¡vxÏ±³(~”ä52YÃ×‚ªwj( Oáì©Ì$ë&şó8	:ŒÜ>Êûh^;2KÎ³&NßT”q:–”!çšŠùşt]—>Ë2/5Öî~$.ÏÉÿJò¿“Í;˜€¹ÅÑ\^6üÃ€wŠëRºP›’©7r }“¤ö­ˆ´lC×Y&¼«è§r\[ÑQ€:R…G‹qj2$pÂ¾lÁ9ñ„ß°ñ1s™Dós´<.ş£c_íó*GZ“p_ú5eRÄ»›ñÈaåtóAn_ÜÛÀyéÃzW[§pùáê×t¯ã;M}ò£±w„ošê[Lll[¾9{6|xï{1S(*F§‚‰BnQp—ß|ıÓø«`ßÑf€5Û`'nAwÜŒBº˜Èøµ2¥ª…Bê½/»Şã{]/ÂŠu‡’Øi1ˆ!–şşZüpœWI…tSê.š0„Ñ¾ƒÂl>™jÆ½Ÿiı[~ÂŒ 5q®7šİî”=Åhà·éŸ:aÆ€‹…ÏÑÃ;7s:ì›ÒpÊl˜ Š¥#"ŠÀâÈøúü#u	;Î˜‡Àü/NfâÛ")!Ü‚¨®Â+Şi{jWS6òğ™İarhğ„ëh|sÍÑ%	uÅ\ŸKˆ4! ,X“¶/²h¤Ÿ›Q4€¯ûK€dªzP•ch
)¿!­r!ãèPûè%Z!yU$6dü(Ñ,âõ	&º¤ÁÓ±06–6œVÑYóƒ=‹Ö1Ô‡İ˜µ|M|cI‹ˆM…’a_f’$s_|s÷È/ZP^ö¡+Â
ı™şCSş™%~4ÚŞÀu¡NT)ÊP—?×A#ñÿ¹4!¢»ŞÃGßxƒbg
wB'¨ÄÙF5bô(&Œ„¼ztœœkR#œ½Âtd(‰®9Ø‰/=H “µxøG9¯€MÇ¥©gbŞúóõ¦›’ ğQ‰C±âpÔş}‰ŠC¯qV)Bwá!H¨‘XèV?ü(FO¿%fqÛŸ)ÀDíW;%›ì¯»Äõf sWDd¨ÇÊÌëBöÀ]Â~xgj_l +Y^M¶mOòK·V@ù€ËaFC0XÅ–Wr-í
ƒ¯_ìƒÙ1upO!¹&=üè.G³ôŸÈªœpqsÜ™±f›Ö”	óú²5|UyÓºPâ)mYh´]	f7v5µ¨²ªµ0X‘vf¥ä.İ.ˆ»!CâÔ!ÄãkS{ŒØ±ô¡8'!N‹è‚´yÙEç‹sDbºø^J¢Ip³…OŒıäœªèÈñÿ(šÈL_Şh‘Bg\Eß`$kDE{[çOYTúj]Ãñ1°š"MsPÒ™¡Jj[)‹Œ,n~5?1H­v¹šnI‡nß•A¶ÂÔ‡Òù¿îK–Ìxg@u‡P»£fŠÃ^æ¦*’šê‘“<}-7€hIy‚ÓŞ®œaùÖZ’lF?ÏuèĞ‡®ÑÄ½
®ş‡IO¤bzàW4âùmï×´“ZY8¬‹Ô±éõ®©ÍøÛ\tşÆœq%¯Ï‚§èÛ¥|p»R‹ˆdƒÄXÌ™rù«/ë4*ËP]Ç!\G¢„1›HÌ±—D*!~ŒjÈA‘€óÜOxº½äIP×ŞoH½“oë¦¼ŠÌ‘KÌŸ
»å*j6c¾0NP:’ÓY3°KçnîPÍˆ9FUJîp·äñ³hÚ”—A*¶©E„jğZşWV©„éÖXImÀFE÷£­”u­²w»Ù	Ü‚ıŞƒÁ­Íú ƒŞÚ(IİÔ”GÕö8?æ¡îî{~óÓ6 îˆoÕµ•¶ZG`t(§Sß•²ˆ&A¹½à”Á}×Ï_è¼¬¾ZD¤ª$˜ŒÕòÛø	íòuYûáˆ­“L—¢òÂ;ËóÈ]öóíUë´6¯µíEBj•µ l€ÿÀ0–¡¡jÀi¸?‡Ÿ!ÂZîœÆîû‡ïÁš‹±gô`0 wëê»€}±ÒŒ?Ú¾$Jv`…¾X7Rğ#÷ĞİFÇ%|ÓºÛîtÅÃ x¶¨IlNq—î8(FYƒ/•îŠáÀ¦SÒ=°Ùº ²È*6xYnÉ†$è%kûŒOá4*ó|¹ï)h(ş9'K‡E›#,ĞÚO¸!+«
ÒmÇFúSÁ½ÉÖ>±{é­—:ÿñÑ.v6 ÍbìczaõíV)·±—T¥ÜXI1ù0¿ú\ıƒqëévñ–Õ£æ  R2R£Â_B59«J:IƒÍİÓJiÚ%š¹ˆ¨vâà ]2œùƒëç¬ùï-&™/ñ_¿Zm%à…ë j›™'Ü;­Ì§Î–?÷R×‘Ù@†vaë=jQ4c´Àfñó™h¨›E_[4Ø=r` sÕg—Úƒñî$G	‡ÚÀº£oˆ÷ƒˆcÔí72ê6Gt¾Â³O¦]Åœı{š†£²×2ğc+!cK'Ú{pF®´ùÒfbœÊE¦FB=Th*Dàãî•ÇVÉ”Bf”î¤¹^Ê–‡n;u_½íPYœÜ·Òµ°f¦;U—“me[cdÆ¿v«¸» ¯ù5±¸qós…âÕ²‡úIÑQåXáDĞJ d@Naı=f;3êÅ‘wæÌSŠ¢¹™,C§ÌM>ÉŸ7¶q±ìôÈïÅÆî®kjsÜN$oXA½/œ^º6©1U¾‚|D{
ø
Õah˜M’â~³?ë‚“´Ãâ}ï1&8ªJ®ôI™Iõ—0S½^mbtüx¾„Y´?WVv:äşhA¶Ê9[ÕæÑëµm¨í	Ôé•şİ{0Ş;ø8R…ûÃ}l«4
ş™ús¥ÛœîS&­PboüQ¬{Ïø’³W¦=ºÁ´–'ª4í{	Âûâ¢å³«JÜõ3ñó¯
¼šÀ‡9¯’—Õ!L‘®¬å¶,ªz¨f(I¯4xñ¸F@é¢2Œlç^®ÓšËIÒ#¿“ö´= »à« š±íÃ:Íò@VSì£ÚÃq‰‚¥î‰­ö÷¸ªsñ|ÈhzFVYõfI0‹ã?ş:JøÃ4™ÙØìÄo¶Ü_g¬e7gÖ—ŒÑÅu²“¥Y°Ìk3g»;Ç{ìKB?Ä”>XS˜Şlq> Ô]«mò‘°öèÙO¢†Ò×ŒéÚ¸ÖŒä@7`Î×Wë[×sI¦W  ş¢Ihf‰'øÊYq|+zlZpzÃ»,4’1‚ñÈr(í³šÉáÓÔ>Sáx¤‡J¦cG.3öË.ÓkƒØÑË;’‘6»«ÉE³m¢ÑYøÿ¬hXŒgcš³Ê~yğ‚H“ó‘ûĞz§?fqN´8\ªòÚù1H8æ“ÎçVZõ+
áË²cz;úË±Sõ¸²/U5óè¤ Ê<°xŞu Rgéf™}ë®RØd Ù7]üïõFàD¦?fô­´ÚñÌI¢a]«+«©søpÇx·d|ÀƒÛä#tU.Ü®‚l[¨47@ú8s—7äìËv&ŸeÏÖ°´ Õ6€t6¯Nº±¸+^Å3LÇ#óxûv…Ê “eƒju13p¶ı"åáy&ä¿ğ«u]H4K•ÚíF4*KM6¨Ãú^2/­ÅP›^“ìĞ|>Œ°w+G˜‚ŒNå	Õ%U÷C‚ÁF‡°M…î²gOÑÑ\FÂU„%ŠğŒ¸kkº÷!4(—ôiª7H¾„øœ†¨õÛñèµ•ÿj–‹—à<Äxâ!§E{ƒ¶—ÇcmíğWÁÍ¿Œ3úšcŒ˜5Cİ¡¥Ü•Ï‚ ‰¥Ò¶3XÖ=¸É”Ú¶òlwÈÈg™MîÃ
S±(ehX­w¼4cäÖc¸#Å0â£Ç¿îÏ`¼<T£Å]w@aÎn²ö¯ïè¿«Ÿ1(šƒ?AK‘ª?rû ïùmO
pKË×œZÆ‘P‰„#B%¼¿}óóuØïYq·¸J½d—×Â¹”B`UM‚Üû6gT©*˜¢ÀrD¼äÃrnî-;AÎ¨	mNuäü E)ê"=1í»+ïwÚ+Kh-ÒÂ%6ÈE4æÜøµÖ†‘ÁF-×Áş‚êƒ¿5 @xs°«Ë‚Ñ×ü›µìY£ŸXJï ñÚRÓ§2µ§kÙ“½¾Ìd•Š:xŞO(kÊËq´Ì¤›ÃlyÀ]Áÿ—ºİtLÀ›ó~šmyËaÕCU¾Òw™XlT¹`õµ8lh®Ä÷ÛùGa<mn‚,cë÷µĞ ^€Ã—ë2Å@¿â§´â¿w[åîJ4È)_ºÑ¦EşA¤3&=_>ˆbî”-J&òP²oQÑ'r>²!Ğ‰Æƒ.8ÿàÒ†Y­	ãbÀó€'™œe=gŒ,mo€Fánn1u²ó³-å-Á¸?K]Ì&Â—‚ùÎ$ÌZÅ&õ+£ºËg´çù‰ÛEr¢ÌÄ®³Ñ?‰±*¤ÌşPÎx&ÃÑãDtLe´_&h¾Å/5uPíòõX#=8û¢ï¯ûâÚsT°Â±?…bU›•cÉ3Ê³˜›2‰£Ò½ÛšR(§XÊÊæv—Ìı¾JüGÖÆ6ÛËÓ|Î1`—Ö†	¬Ó‡¢ºWl¬â†åë&ı2Ÿ0= ‚—‹‚ñâØ—×X¿C¦<¨ï²uÓç4¡:¢†lÈïÅƒû¤“SyÖ÷9‘ı2äY&ìF®ü¢hÖ²Hr½ÜßY‡Àé†¨:x¤6ÿMDYË/°sÊ‘¡¡ü-·m»µÍ|²•\İ¶œIAhBXûË¾À*Í‹s$tè{ı#¶IÜqÂ†q…«œ' '}EN”'²üy°™»¶ÁÇZƒ~f—/GE‘7$4íšN1 aªÀŒb¬4¦‚#ÔzÈŸ$ãì‚'¡p`Ğjİb–Cšì;¦¦y«Œ§hql–Ü«£‰Po ŒRĞİOòƒ0ó'°iºk6Ğª¨(ïËåäÛ2‰‘%;Äï›T–“.äéUP®‹ç¿Ÿd¢ëjUaùT^ôÆ±Ÿ®ì„æ¢
›Ğ[¿*ü+€5Ş÷Áæ3zL©ŒÊœLğ¸WêMZ;J‰~„§vJUr¬uuVˆµŠÑƒæ¨Ÿ„÷²èSÁ.Ş	@²ÁÇıE'——áÄ-ËMÌÙùI~#âd{Dävˆ}p ¢ôÑIØŸ2“I0å'OR'd&36dUMøã¯~–íµ‰ŠO3ºÊ¨"=ÎT¸#ÉNIÛ_ÁË+á½¦¥¢Wí‰ğ¢MN×¡ŞM,š¸ç¥D*ÍRO9öJGæ=Æ¶8°÷G ]<Œ‹„€÷çO­rÕØcL®!ö~a|›òÜâD¾p¦æ—jsÿ`Í%Î¹¢4Ÿ*î0Îíò[ß²mëïízä¹aÏŠÎBÖÔÂªZ0ÅîÛ®mØ¢i„“Õ%]û »:7Â”p……›Ïa¶£íCU%fÕg~R[	c¢"ówœeÇ¬¸Ûm^JKz$C»'VİÎ¢ÊT÷îV$Zõ„-©Øô1q^^0"g¸¾^ğnÈdG4Z¿fÂ²DÌ¹7Ê^[fœwîF>Z°$df…Ä‡ôõ©›Ûp.ÁyzèåWì#k€ÀpS
å~v/hİğH…<¿_ÔDÎK›ÁéBwŸkÕ¨Ü«ınM‘‹Qõxiìîó·³‡†aĞß”Ouõ¡½W»+T³ÛÅ3ÏE Û^0¦áLm:N d include/config/NO_HZ_FULL) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/DEBUG_MUTEXES) \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/COMPAT) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/UPROBES) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/PREEMPTION) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/asm-generic/percpu.h \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/rcupdate.h \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/irqflags.h \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
    $(wildcard include/config/DEBUG_ENTRY) \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/processor-flags.h \
    $(wildcard include/config/VM86) \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  inclcmd_drivers/media/i2c/uda1342.o := gcc -Wp,-MMD,drivers/media/i2c/.uda1342.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"uda1342"' -DKBUILD_MODNAME='"uda1342"' -D__KBUILD_MODNAME=kmod_uda1342 -c -o drivers/media/i2c/uda1342.o drivers/media/i2c/uda1342.c 

source_drivers/media/i2c/uda1342.o := drivers/media/i2c/uda1342.c

deps_drivers/media/i2c/uda1342.o := \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/RETPOLINE) \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/vdso/math64.h \
  include/linux/time64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/scree