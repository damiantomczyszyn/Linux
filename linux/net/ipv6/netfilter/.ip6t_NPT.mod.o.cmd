de/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/linux/kstrtox.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/kern_levels.h \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/ratelimit_types.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  include/linux/instruction_pointer.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/bug.h \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/TINY_RCU) \
  include/linux/context_tracking_state.h \ount_matching_names(class);
	class->wait_type_inner = lock->wait_type_inner;
	class->wait_type_outer = lock->wait_type_outer;
	class->lock_type = lock->lock_type;
	/*
	 * We use RCU's safe list-add method to make
	 * parallel walking of the hash-list safe:
	 */
	hlist_add_head_rcu(&class->hash_entry, hash_head);
	/*
	 * Remove the class from the free list and add it to the global list
	 * of classes.
	 */
	list_move_tail(&class->lock_entry, &all_lock_classes);
	idx = class - lock_classes;
	if (idx > max_lock_class_idx)
		max_lock_class_idx = idx;

	if (verbose(class)) {
		graph_unlock();

		printk("\nnew class %px: %s", class->key, class->name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		printk(KERN_CONT "\n");
		dump_stack();

		if (!graph_lock()) {
			return NULL;
		}
	}
out_unlock_set:
	graph_unlock();

out_set_class_cache:
	if (!subclass || force)
		lock->class_cache[0] = class;
	else if (subclass < NR_LOCKDEP_CACHING_CLASSES)
		lock->class_cache[subclass] = class;

	/*
	 * Hash collision, did we smoke some? We found a class with a matching
	 * hash but the subclass -- which is hashed in -- didn't match.
	 */
	if (DEBUG_LOCKS_WARN_ON(class->subclass != subclass))
		return NULL;

	return class;
}

#ifdef CONFIG_PROVE_LOCKING
/*
 * Allocate a lockdep entry. (assumes the graph_lock held, returns
 * with NULL on failure)
 */
static struct lock_list *alloc_list_entry(void)
{
	int idx = find_first_zero_bit(list_entries_in_use,
				      ARRAY_SIZE(list_entries));

	if (idx >= ARRAY_SIZE(list_entries)) {
		if (!debug_locks_off_graph_unlock())
			return NULL;

		print_lockdep_off("BUG: MAX_LOCKDEP_ENTRIES too low!");
		dump_stack();
		return NULL;
	}
	nr_list_entries++;
	__set_bit(idx, list_entries_in_use);
	return list_entries + idx;
}

/*
 * Add a new dependency to the head of the list:
 */
static int add_lock_to_list(struct lock_class *this,
			    struct lock_class *links_to, struct list_head *head,
			    unsigned long ip, u16 distance, u8 dep,
			    const struct lock_trace *trace)
{
	struct lock_list *entry;
	/*
	 * Lock not present yet - get a new dependency struct and
	 * add it to the list:
	 */
	entry = alloc_list_entry();
	if (!entry)
		return 0;

	entry->class = this;
	entry->links_to = links_to;
	entry->dep = dep;
	entry->distance = distance;
	entry->trace = trace;
	/*
	 * Both allocation and removal are done under the graph lock; but
	 * iteration is under RCU-sched; see look_up_lock_class() and
	 * lockdep_free_key_range().
	 */
	list_add_tail_rcu(&entry->entry, head);

	return 1;
}

/*
 * For good efficiency of modular, we use power of 2
 */
#define MAX_CIRCULAR_QUEUE_SIZE		(1UL << CONFIG_LOCKDEP_CIRCULAR_QUEUE_BITS)
#define CQ_MASK				(MAX_CIRCULAR_QUEUE_SIZE-1)

/*
 * The circular_queue and helpers are used to implement graph
 * breadth-first search (BFS) algorithm, by which we can determine
 * whether there is a path from a lock to another. In deadlock checks,
 * a path from the next lock to be acquired to a previous held lock
 * indicates that adding the <prev> -> <next> lock dependency will
 * produce a circle in the graph. Breadth-first search instead of
 * depth-first search is used in order to find the shortest (circular)
 * path.
 */
struct circular_queue {
	struct lock_list *element[MAX_CIRCULAR_QUEUE_SIZE];
	unsigned int  front, rear;
};

static struct circular_queue lock_cq;

unsigned int max_bfs_queue_depth;

static unsigned int lockdep_dependency_gen_id;

static inline void __cq_init(struct circular_queue *cq)
{
	cq->front = cq->rear = 0;
	lockdep_dependency_gen_id++;
}

static inline int __cq_empty(struct circular_queue *cq)
{
	return (cq->front == cq->rear);
}

static inline int __cq_full(struct circular_queue *cq)
{
	return ((cq->rear + 1) & CQ_MASK) == cq->front;
}

static inline int __cq_enqueue(struct circular_queue *cq, struct lock_list *elem)
{
	if (__cq_full(cq))
		return -1;

	cq->element[cq->rear] = elem;
	cq->rear = (cq->rear + 1) & CQ_MASK;
	return 0;
}

/*
 * Dequeue an element from the circular_queue, return a lock_list if
 * the queue is not empty, or NULL if otherwise.
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

static void print_lock_class