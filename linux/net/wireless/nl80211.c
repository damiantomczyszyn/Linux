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
			 *  If we fail to return, we have inconsistency, i.e.
			 *  <prev>::locks_after contains <next> while
			 *  <next>::locks_before doesn't contain <prev>. In
			 *  that case, we return after the inner and indicate
			 *  something is wrong.
			 */
			list_for_each_entry(entry, &hlock_class(next)->locks_before, entry) {
				if (entry->class == hlock_class(prev)) {
					if (distance == 1)
						entry->distance = 1;
					entry->dep |= calc_depb(prev, next);
					return 1;
				}
			}

			/* <prev> is not found in <next>::locks_before */
			return 0;
		}
	}

	/*
	 * Is the <prev> -> <next> link redundant?
	 */
	ret = check_redundant(prev, next);
	if (bfs_error(ret))
		return 0;
	else if (ret == BFS_RMATCH)
		return 2;

	if (!*trace) {
		*trace = save_trace();
		if (!*trace)
			return 0;
	}

	/*
	 * Ok, all validations passed, add the new lock
	 * to the previous lock's dependency list:
	 */
	ret = add_lock_to_list(hlock_class(next), hlock_class(prev),
			       &hlock_class(prev)->locks_after,
			       next->acquire_ip, distance,
			       calc_dep(prev, next),
			       *trace);

	if (!ret)
		return 0;

	ret = add_lock_to_list(hlock_class(prev), hlock_class(next),
			       &hlock_class(next)->locks_before,
			       next->acquire_ip, distance,
			       calc_depb(prev, next),
			       *trace);
	if (!ret)
		return 0;

	return 2;
}

/*
 * Add the dependency to all directly-previous locks that are 'relevant'.
 * The ones that are relevant are (in increasing distance from curr):
 * all consecutive trylock entries and the final non-trylock entry - or
 * the end of this context's lock-chain - whichever comes first.
 */
static int
check_prevs_add(struct task_struct *curr, struct held_lock *next)
{
	struct lock_trace *trace = NULL;
	int depth = curr->lockdep_depth;
	struct held_lock *hlock;

	/*
	 * Debugging checks.
	 *
	 * Depth must not be zero for a non-head lock:
	 */
	if (!depth)
		goto out_bug;
	/*
	 * At least two relevant locks must exist for this
	 * to be a head:
	 */
	if (curr->held_locks[depth].irq_context !=
			curr->held_locks[depth-1].irq_context)
		goto out_bug;

	for (;;) {
		u16 distance = curr->lockdep_depth - depth + 1;
		hlock = curr->held_locks + depth - 1;

		if (hlock->check) {
			int ret = check_prev_add(curr, hlock, next, distance, &trace);
			if (!ret)
				return 0;

			/*
			 * Stop after the first non-trylock entry,
			 * as non-trylock entries have added their
			 * own direct dependencies already, so this
			 * lock is connected to them indirectly:
			 */
			if (!hlock->trylock)
				break;
		}

		depth--;
		/*
		 * End of lock-stack?
		 */
		if (!depth)
			break;
		/*
		 * Stop the search if we cross into another context:
		 */
		if (curr->held_locks[depth].irq_context !=
				curr->held_locks[depth-1].irq_context)
			break;
	}
	return 1;
out_bug:
	if (!debug_locks_off_graph_unlock())
		return 0;

	/*
	 * Clearly we all shouldn't be here, but since we made it we
	 * can reliable say we messed up our state. See the above two
	 * gotos for reasons why we could possibly end up here.
	 */
	WARN_ON(1);

	return 0;
}

struct lock_chain lock_chains[MAX_LOCKDEP_CHAINS];
static DECLARE_BITMAP(lock_chains_in_use, MAX_LOCKDEP_CHAINS);
static u16 chain_hlocks[MAX_LOCKDEP_CHAIN_HLOCKS];
unsigned long nr_zapped_lock_chains;
unsigned int nr_free_chain_hlocks;	/* Free chain_hlocks in buckets */
unsigned int nr_lost_chain_hlocks;	/* Lost chain_hlocks */
unsigned int nr_large_chain_blocks;	/* size > MAX_CHAIN_BUCKETS */

/*
 * The first 2 chain_hlocks entries in the chain block in the bucket
 * list contains the following meta data:
 *
 *   entry[0]:
 *     Bit    15 - always set to 1 (it is not a class index)
 *     Bits 0-14 - upper 15 bits of the next block index
 *   entry[1]    - lower 16 bits of next block index
 *
 * A next block index of all 1 bits means it is the end of the list.
 *
 * On the unsized bucket (bucket-0), the 3rd and 4th entries contain
 * the chain block size:
 *
 *   entry[2] - upper 16 bits of the chain block size
 *   entry[3] - lower 16 bits of the chain block size
 */
#define MAX_CHAIN_BUCKETS	16
#define CHAIN_BLK_FLAG		(1U << 15)
#define CHAIN_BLK_LIST_END	0xFFFFU

static int chain_block_buckets[MAX_CHAIN_BUCKETS];

static inline int size_to_bucket(int size)
{
	if (size > MAX_CHAIN_BUCKETS)
		return 0;

	return size - 1;
}

/*
 * Iterate all the chain blocks in a bucket.
 */
#define for_each_chain_block(bucket, prev, curr)		\
	for ((prev) = -1, (curr) = chain_block_buckets[bucket];	\
	     (curr) >= 0;					\
	     (prev) = (curr), (curr) = chain_block_next(curr))

/*
 * next block or -1
 */
static inline int chain_block_next(int offset)
{
	int next = chain_hlocks[offset];

	WARN_ON_ONCE(!(next & CHAIN_BLK_FLAG));

	if (next == CHAIN_BLK_LIST_END)
		return -1;

	next &= ~CHAIN_BLK_FLAG;
	next <<= 16;
	next |= chain_hlocks[offset + 1];

	return next;
}

/*
 * bucket-0 only
 */
static inline int chain_block_size(int offset)
{
	return (chain_hlocks[offset + 2] << 16) | chain_hlocks[offset + 3];
}

static inline void init_chain_block(int offset, int next, int bucket, int size)
{
	chain_hlocks[offset] = (next >> 16) | CHAIN_BLK_FLAG;
	chain_hlocks[offset + 1] = (u16)next;

	if (size && !bucket) {
		chain_hlocks[offset + 2] = size >> 16;
		chain_hlocks[offset + 3] = (u16)size;
	}
}

static inline void add_chain_block(int offset, int size)
{
	int bucket = size_to_bucket(size);
	int next = chain_block_buckets[bucket];
	int prev, curr;

	if (unlikely(size < 2)) {
		/*
		 * We can't store single entries on the freelist. Leak them.
		 *
		 * One possible way out would be to uniquely mark them, other
		 * than with CHAIN_BLK_FLAG, such that we can recover them when
		 * the block before it is re-added.
		 */
		if (size)
			nr_lost_chain_hlocks++;
		return;
	}

	nr_free_chain_hlocks += size;
	if (!bucket) {
		nr_large_chain_blocks++;

		/*
		 * Variable sized, sort large to small.
		 */
		for_each_chain_block(0, prev, curr) {
			if (size >= chain_block_size(curr))
				break;
		}
		init_chain_block(offset, curr, 0, size);
		if (prev < 0)
			chain_block_buckets[0] = offset;
		else
			init_chain_block(prev, offset, 0, 0);
		return;
	}
	/*
	 * Fixed size, add to head.
	 */
	init_chain_block(offset, next, bucket, size);
	chain_block_buckets[bucket] = offset;
}

/*
 * Only the first block in the list can be deleted.
 *
 * For the variable size bucket[0], the first block (the largest one) is
 * returned, broken up and put back into the pool. So if a chain block of
 * length > MAX_CHAIN_BUCKETS is ever used and zapped, it will just be
 * queued up after the primordial chain block and never be used until the
 * hlock entries in the primordial chain block is almost used up. That
 * causes fragmentation and reduce allocation efficiency. That can be
 * monitored by looking at the "large chain blocks" number in lockdep_stats.
 */
static inline void del_chain_block(int bucket, int size, int next)
{
	nr_free_chain_hlocks -= size;
	chain_block_buckets[bucket] = next;

	if (!bucket)
		nr_large_chain_blocks--;
}

static void init_chain_block_buckets(void)
{
	int i;

	for (i = 0; i < MAX_CHAIN_BUCKETS; i++)
		chain_block_buckets[i] = -1;

	add_chain_block(0, ARRAY_SIZE(chain_hlocks));
}

/*
 * Return offset of a chain block of the right size or -1 if not found.
 *
 * Fairly simple worst-fit allocator with the addition of a number of size
 * specific free lists.
 */
static int alloc_chain_hlocks(int req)
{
	int bucket, curr, size;

	/*
	 * We rely on the MSB to act as an escape bit to denote freelist
	 * pointers. Make sure this bit isn't set in 'normal' class_idx usage.
	 */
	BUILD_BUG_ON((MAX_LOCKDEP_KEYS-1) & CHAIN_BLK_FLAG);

	init_data_structures_once();

	if (nr_free_chain_hlocks < req)
		return -1;

	/*
	 * We require a minimum of 2 (u16) entries to encode a freelist
	 * 'pointer'.
	 */
	req = max(req, 2);
	bucket = size_to_bucket(req);
	curr = chain_block_buckets[bucket];

	if (bucket) {
		if (curr >= 0) {
			del_chain_block(bucket, req, chain_block_next(curr));
			return curr;
		}
		/* Try bucket 0 */
		curr = chain_block_buckets[0];
	}

	/*
	 * The variable sized freelist is sorted by size; the first entry is
	 * the largest. Use it if it fits.
	 */
	if (curr >= 0) {
		size = chain_block_size(curr);
		if (likely(size >= req)) {
			del_chain_block(0, size, chain_block_next(curr));
			add_chain_block(curr + req, size - req);
			return curr;
		}
	}

	/*
	 * Last resort, split a block in a larger sized bucket.
	 */
	for (size = MAX_CHAIN_BUCKETS; size > req; size--) {
		bucket = size_to_bucket(size);
		curr = chain_block_buckets[bucket];
		if (curr < 0)
			continue;

		del_chain_block(bucket, size, chain_block_next(curr));
		add_chain_block(curr + req, size - req);
		return curr;
	}

	return -1;
}

static inline void free_chain_hlocks(int base, int size)
{
	add_chain_block(base, max(size, 2));
}

struct lock_class *lock_chain_get_class(struct lock_chain *chain, int i)
{
	u16 chain_hlock = chain_hlocks[chain->base + i];
	unsigned int class_idx = chain_hlock_class_idx(chain_hlock);

	return lock_classes + class_idx;
}

/*
 * Returns the index of the first held_lock of the current chain
 */
static inline int get_first_held_lock(struct task_struct *curr,
					struct held_lock *hlock)
{
	int i;
	struct held_lock *hlock_curr;

	for (i = curr->lockdep_depth - 1; i >= 0; i--) {
		hlock_curr = curr->held_locks + i;
		if (hlock_curr->irq_context != hlock->irq_context)
			break;

	}

	return ++i;
}

#ifdef CONFIG_DEBUG_LOCKDEP
/*
 * Returns the next chain_key iteration
 */
static u64 print_chain_key_iteration(u16 hlock_id, u64 chain_key)
{
	u64 new_chain_key = iterate_chain_key(chain_key, hlock_id);

	printk(" hlock_id:%d -> chain_key:%016Lx",
		(unsigned int)hlock_id,
		(unsigned long long)new_chain_key);
	return new_chain_key;
}

static void
print_chain_keys_held_locks(struct task_struct *curr, struct held_lock *hlock_next)
{
	struct held_lock *hlock;
	u64 chain_key = INITIAL_CHAIN_KEY;
	int depth = curr->lockdep_depth;
	int i = get_first_held_lock(curr, hlock_next);

	printk("depth: %u (irq_context %u)\n", depth - i + 1,
		hlock_next->irq_context);
	for (; i < depth; i++) {
		hlock = curr->held_locks + i;
		chain_key = print_chain_key_iteration(hlock_id(hlock), chain_key);

		print_lock(hlock);
	}

	print_chain_key_iteration(hlock_id(hlock_next), chain_key);
	print_lock(hlock_next);
}

static void print_chain_keys_chain(struct lock_chain *chain)
{
	int i;
	u64 chain_key = INITIAL_CHAIN_KEY;
	u16 hlock_id;

	printk("depth: %u\n", chain->depth);
	for (i = 0; i < chain->depth; i++) {
		hlock_id = chain_hlocks[chain->base + i];
		chain_key = print_chain_key_iteration(hlock_id, chain_key);

		print_lock_name(lock_classes + chain_hlock_class_idx(hlock_id));
		printk("\n");
	}
}

static void print_collision(struct task_struct *curr,
			struct held_lock *hlock_next,
			struct lock_chain *chain)
{
	pr_warn("\n");
	pr_warn("============================\n");
	pr_warn("WARNING: chain_key collision\n");
	print_kernel_ident();
	pr_warn("----------------------------\n");
	pr_warn("%s/%d: ", current->comm, task_pid_nr(current));
	pr_warn("Hash chain already cached but the contents don't match!\n");

	pr_warn("Held locks:");
	print_chain_keys_held_locks(curr, hlock_next);

	pr_warn("Locks in cached chain:");
	print_chain_keys_chain(chain);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}
#endif

/*
 * Checks whether the chain and the current held locks are consistent
 * in depth and also in content. If they are not it most likely means
 * that there was a collision during the calculation of the chain_key.
 * Returns: 0 not passed, 1 passed
 */
static int check_no_collision(struct task_struct *curr,
			struct held_lock *hlock,
			struct lock_chain *chain)
{
#ifdef CONFIG_DEBUG_LOCKDEP
	int i, j, id;

	i = get_first_held_lock(curr, hlock);

	if (DEBUG_LOCKS_WARN_ON(chain->depth != curr->lockdep_depth - (i - 1))) {
		print_collision(curr, hlock, chain);
		return 0;
	}

	for (j = 0; j < chain->depth - 1; j++, i++) {
		id = hlock_id(&curr->held_locks[i]);

		if (DEBUG_LOCKS_WARN_ON(chain_hlocks[chain->base + j] != id)) {
			print_collision(curr, hlock, chain);
			return 0;
		}
	}
#endif
	return 1;
}

/*
 * Given an index that is >= -1, return the index of the next lock chain.
 * Return -2 if there is no next lock chain.
 */
long lockdep_next_lockchain(long i)
{
	i = find_next_bit(lock_chains_in_use, ARRAY_SIZE(lock_chains), i + 1);
	return i < ARRAY_SIZE(lock_chains) ? i : -2;
}

unsigned long lock_chain_count(void)
{
	return bitmap_weight(lock_chains_in_use, ARRAY_SIZE(lock_chains));
}

/* Must be called with the graph lock held. */
static struct lock_chain *alloc_lock_chain(void)
{
	int idx = find_first_zero_bit(lock_chains_in_use,
				      ARRAY_SIZE(lock_chains));

	if (unlikely(idx >= ARRAY_SIZE(lock_chains)))
		return NULL;
	__set_bit(idx, lock_chains_in_use);
	return lock_chains + idx;
}

/*
 * Adds a dependency chain into chain hashtable. And must be called with
 * graph_lock held.
 *
 * Return 0 if fail, and graph_lock is released.
 * Return 1 if succeed, with graph_lock held.
 */
static inline int add_chain_cache(struct task_struct *curr,
				  struct held_lock *hlock,
				  u64 chain_key)
{
	struct hlist_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;
	int i, j;

	/*
	 * The caller must hold the graph lock, ensure we've got IRQs
	 * disabled to make this an IRQ-safe lock.. for recursion reasons
	 * lockdep won't complain about its own locking errors.
	 */
	if (lockdep_assert_locked())
		return 0;

	chain = alloc_lock_chain();
	if (!chain) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAINS too low!");
		dump_stack();
		return 0;
	}
	chain->chain_key = chain_key;
	chain->irq_context = hlock->irq_context;
	i = get_first_held_lock(curr, hlock);
	chain->depth = curr->lockdep_depth + 1 - i;

	BUILD_BUG_ON((1UL << 24) <= ARRAY_SIZE(chain_hlocks));
	BUILD_BUG_ON((1UL << 6)  <= ARRAY_SIZE(curr->held_locks));
	BUILD_BUG_ON((1UL << 8*sizeof(chain_hlocks[0])) <= ARRAY_SIZE(lock_classes));

	j = alloc_chain_hlocks(chain->depth);
	if (j < 0) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAIN_HLOCKS too low!");
		dump_stack();
		return 0;
	}

	chain->base = j;
	for (j = 0; j < chain->depth - 1; j++, i++) {
		int lock_id = hlock_id(curr->held_locks + i);

		chain_hlocks[chain->base + j] = lock_id;
	}
	chain_hlocks[chain->base + j] = hlock_id(hlock);
	hlist_add_head_rcu(&chain->entry, hash_head);
	debug_atomic_inc(chain_lookup_misses);
	inc_chains(chain->irq_context);

	return 1;
}

/*
 * Look up a dependency chain. Must be called with either the graph lock or
 * the RCU read lock held.
 */
static inline struct lock_chain *lookup_chain_cache(u64 chain_key)
{
	struct hlist_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;

	hlist_for_each_entry_rcu(chain, hash_head, entry) {
		if (READ_ONCE(chain->chain_key) == chain_key) {
			debug_atomic_inc(chain_lookup_hits);
			return chain;
		}
	}
	return NULL;
}

/*
 * If the key is not present yet in dependency chain cache then
 * add it and return 1 - in this case the new dependency chain is
 * validated. If the key is already hashed, return 0.
 * (On return with 1 graph_lock is held.)
 */
static inline int lookup_chain_cache_add(struct task_struct *curr,
					 struct held_lock *hlock,
					 u64 chain_key)
{
	struct lock_class *class = hlock_class(hlock);
	struct lock_chain *chain = lookup_chain_cache(chain_key);

	if (chain) {
cache_hit:
		if (!check_no_collision(curr, hlock, chain))
			return 0;

		if (very_verbose(class)) {
			printk("\nhash chain already cached, key: "
					"%016Lx tail class: [%px] %s\n",
					(unsigned long long)chain_key,
					class->key, class->name);
		}

		return 0;
	}

	if (very_verbose(class)) {
		printk("\nnew hash chain, key: %016Lx tail class: [%px] %s\n",
			(unsigned long long)chain_key, class->key, class->name);
	}

	if (!graph_lock())
		return 0;

	/*
	 * We have to walk the chain again locked - to avoid duplicates:
	 */
	chain = lookup_chain_cache(chain_key);
	if (chain) {
		graph_unlock();
		goto cache_hit;
	}

	if (!add_chain_cache(curr, hlock, chain_key))
		return 0;

	return 1;
}

static int validate_chain(struct task_struct *curr,
			  struct held_lock *hlock,
			  int chain_head, u64 chain_key)
{
	/*
	 * Trylock needs to maintain the stack of held locks, but it
	 * does not add new dependencies, because trylock can be done
	 * in any order.
	 *
	 * We look up the chain_key and do the O(N^2) check and update of
	 * the dependencies only if this is a new dependency chain.
	 * (If lookup_chain_cache_add() return with 1 it acquires
	 * graph_lock for us)
	 */
	if (!hlock->trylock && hlock->check &&
	    lookup_chain_cache_add(curr, hlock, chain_key)) {
		/*
		 * Check whether last held lock:
		 *
		 * - is irq-safe, if this lock is irq-unsafe
		 * - is softirq-safe, if this lock is hardirq-unsafe
		 *
		 * And check whether the new lock's dependency graph
		 * could lead back to the previous lock:
		 *
		 * - within the current held-lock stack
		 * - across our accumulated lock dependency records
		 *
		 * any of these scenarios could lead to a deadlock.
		 */
		/*
		 * The simple case: does the current hold the same lock
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
	dump_stackÉ'  w  ú'  w  ±'  F  ø'  ã  ‘'  å  ﬁ'  L  Î'  M  ˛'  L  (  N  (  r  !(  `  0(  a  Q(  M  d(  L  i(  N  É(  w  ë(  F  £(  R  ª(  ã  Õ(  Ç  ‹(  É  Ï(  :  ˆ(  è  
)  ê  )  L  )  M  4)  L  9)  N  G)  ë  T)  R  k)  K  v)  `  Ö)  a  ë)  í  √)  w  —)  Y  ·)  F  Ú)  R  ˝)  ã  *  L  *  M  .*  :  w*  î  å*  ï  •*  ï  &+  ï  h+  L  m+  N  ~+  R  ô+  `  ¨+  a  g,  w  {,  ñ  à,  Y  ë,  F  £,  R  ∆,  G  À,  `  Ï,  _  Ò,  `  ,-  T  X-  G  c-  a  h-  b  q-  a  |-  R  ë-  U  ¢-  V  ¨-  W  ≥-  X  »-  U  Ó-  Y  .  F  -.  R  t.  ô  ò.  ö  √.  `  Œ.  G  Î.  _  .  `  ,/  T  X/  G  c/  a  h/  b  q/  a  z/  K  î/  K  ü/  R  …/  U  ⁄/  V  ‰/  W  Î/  X   0  U  /0  K  _0  ô  ·0  Y  Ò0  F  11  R  ∂1  ú  ¡1  ù  ‚1  K  Ì1  R  2  û  "2  _  *2  `  02  G  d2  T  ê2  G  õ2  a  †2  b  ≠2  ü  !3  U  23  V  <3  W  C3  X  X3  U  ∫3  o  Õ3  [  Â3  Y  4  F  64  R  r4  G  •4  °  ”4  ú  ‡4  ù  Û4  û  5  _  
5  `  5  G  D5  T  p5  G  {5  a  Ä5  b  ç5  ü  º5  ú   5  ï  €5  ¢  5  K  ˚5  R  16  U  B6  V  L6  W  S6  X  h6  U  ö6  Y  °6  F  ø6  R  7  G  7  _  7  `  T7  T  Ä7  G  ã7  a  ê7  b  ∑7  R  …7  U  ⁄7  V  ‰7  W  Î7  X   8  U  78  Y  f  K  ˚  P  (    ≥  k  ˚  k  r  a  à  N  …  k  ‘  a  h$  á  é$  á      @                A                B     	     
      C                D                                                           $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     Ä     Ñ     à     å     ê     î     ò     ú     †     §     ®     ¨     ∞     ¥     ∏                   S               S                S  $     (     ,   S  0     4     8   S  <     @     D   S  H     L     P   S  T     X     \   S  `     d     h   S                                 $     (     0     4        :     e                                                 $     (     ,     0     4     8     @     `   I  †     §     ∞     ¿   Z     ^  @  ]  Ä  f  º    ¿    ‡  H     E  \    `    d    h    l    p        D     C     B     A     @   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab+cgroup_bpf_enabled_key .rel___ksymtab+__cgroup_bpf_run_filter_skb .rel___ksymtab+__cgroup_bpf_run_filter_sk .rel___ksymtab+__cgroup_bpf_run_filter_sock_addr .rel___ksymtab+__cgroup_bpf_run_filter_sock_ops .rel__mcount_loc .rodata.str1.4 .rel__jump_table .rodata.str1.1 .rel__bug_table .rel.text.unlikely .rel.smp_locks .rel.rodata .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                                         @   ;8                    	   @       pW    #            %             {8                     +             Ä8  ∏                   0      2       Ä8  ñ                 F             9                    B   	   @       `e     #            l             $9                    h   	   @       xe     #            ó             09                    ì   	   @       êe     #   
         ¡             <9                    Ω   	   @       ®e     #            Ú             H9                    Ó   	   @       ¿e     #            "            T9  º                    	   @       ÿe  x  #            /     2       :  !                 B            4:  l                  >  	   @       Pg  ÿ   #            O     2       †:  I                 b            È:  <                  ^  	   @       (h  P   #            r            %;  
                  n  	   @       xh     #            Ö            0;  0                  Å  	   @       àh  `   #         ELF                      "      4     (               Ë¸ˇˇˇSãX\âÿË¸ˇˇˇçÉ¿   Ë¸ˇˇˇ1¿[√êË¸ˇˇˇWVâ∆Sª
   dã=    ç¥&    çv ãÜî   ∫)   Ë¸ˇˇˇÉ‡t∏∆ß  Ë¸ˇˇˇã®uÉÎu÷∏ˇˇˇˇ[^_√çt& êË¸ˇˇˇÎÊêË¸ˇˇˇVf¡¬Sã∞î   â√∑ ∫&   âË¸ˇˇˇÉ¯ˇt)âÿËrˇˇˇÖ¿x+∫(   âË¸ˇˇˇfÉ¯ˇtf¡¿∑¿[^√fê°    Ö¿è¸ˇˇˇ∏ˇˇˇˇÎÁç¥&    çt& êË¸ˇˇˇUWâ«Vâ÷SâÀÉÏÖ…Ññ   ãAã)â$∫Û   â¯Ëbˇˇˇâ¡°    Ö¿è   Ö…x)â»É‡ t0«$    1Ì∏   Ö€tã<$â+â{1…ÖˆtâÉƒâ»[^_]√çt& É·∫   ”‚˜¬ uVÅ‚  u6É˘uø«$    ÅÂ  ˇ Î∞ç¥&    çv «$    Ωˇˇˇ Èaˇˇˇç¥&    «$    ÅÂ ∞  È|ˇˇˇç∂    âÍ«$    ∂ÍÈeˇˇˇç¥&    ç¥&    êË¸ˇˇˇSâ√ÉÏã    Ö…è9   Éƒâÿ1…[È⁄˛ˇˇç¥&    çv Ë¸ˇˇˇSâ√â—ÉÏ°    Ö¿èT   Éƒâÿ1“[È©˛ˇˇç¥&    fêË¸ˇˇˇWf¡¬Vâ∆Sã∏î   âÀâ—Å· ˇ  ∫'   â¯Ë¸ˇˇˇÉ¯ˇt1âËö˝ˇˇÖ¿xf¡√∫(   â¯∑ÀË¸ˇˇˇÉ¯ˇt[^_√ç¥&    çv °    Ö¿èo   Î„êË¸ˇˇˇVâ∆â—SÉÏ°    Ö¿èä   1€Ö…t
ª   π   ãÜî   ãP\àö"  ∫Ú   Ë¸ˇˇˇ1¿Éƒ[^√çt& Ë¸ˇˇˇÉ˙wvVâ÷Sâ√°    Ö¿è∑   ãÉî   ãı    ãP\àäc  ∫3   ∂…Ë¸ˇˇˇ∫Ú   âÿË$˝ˇˇÉ‡ﬂx*ãı   ∫Ú   ¡·	¡âÿÉ…∑…Ë—˛ˇˇ∏∆ß  Ë¸ˇˇˇ1¿[^√fê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇVSãXãP(ç≥@ˇˇˇÅ˙	ò Ñâ   w7Å˙ 	ò tWÅ˙	ò uGãH|ãC‘ãP\ÉÈ@àä  ∂…∫Á   Ë¸ˇˇˇ1¿[^√çv Å˙	ò u∑H|∫   âË@˛ˇˇ1¿[^√çv ∏Íˇˇˇ[^√ãH|ãC‘ãP\àä  ∂…∫Ê   Ë¸ˇˇˇ1¿[^√ç¥&    ∑H|∫†   âË¯˝ˇˇ1¿[^√çv Ë¸ˇˇˇUâÕWâ◊∫Ú   Vâ∆SÉÏË¸ˇˇã    â$Ö€è  â¯% ∞  Öﬁ   â¯Ñ¿Öå   â¯%  ˇ Ñ  ∏   ∫à   ª‡  ∑»Î
çv ∑∑KÉ√∂“âËt˝ˇˇÅ˚  u‰ã    Ö…è‘   âæ0  ∑<$∫Ú   ââÆ4  Éœ∑œË<˝ˇˇ∏∆ß  Ë¸ˇˇˇ1¿Éƒ[^_]√ç¥&    ê∏   ∫à   ª   ∑»Îçt& ∑∑KÉ√∂“âËÙ¸ˇˇÅ˚H  u‰°    Ö¿~ÖÈÈ   ç∂    1…∫   ª`  Îç¥&    çv ∑∑KÉ√∂“âË¨¸ˇˇÅ˚å  u‰ã    Ö…é8ˇˇˇÈ˛   ê∏ÍˇˇˇÈVˇˇˇç∂    Ë¸ˇˇˇW1…∫Ú   Vâ∆Sª†  Îç¥&    ê∂∂KãÜî   É√ãx\àå0  Ë¸ˇˇˇÖ¿xÅ˚ƒ  u’1…∫Y   ª`  Îç¥&    ∑∑KÉ√∂“âË¸ˇˇÅ˚à  u‰ãÜ0  ˆƒ∞Ö}   π   ∫à   Ñ¿u?©  ˇ Ñ§   ª‡  Îçt& ê∑∑KÉ√∂“âËº˚ˇˇÅ˚  u‰[1¿^_√ç∂    ª   Îê∑∑KÉ√∂“âËå˚ˇˇÅ˚H  u‰[1¿^_√ç∂    1…∫   ª`  Îç¥&    çv ∑∑KÉ√∂“âËL˚ˇˇÅ˚å  u‰[1¿^_√ç∂    ª   Îê∑∑KÉ√∂“âË˚ˇˇÅ˚H  u‰[1¿^_√ç¥&    ç¥&    Ë¸ˇˇˇUWVâ∆SÉÏã@ãPãRË¸ˇˇˇ%  x =  x Öd  çFπ¿  ∫@  Ë¸ˇˇˇâ√Ö¿ÑO  π†   âÚçª¿   Ë¸ˇˇˇ1…∫   â¯j «É0  ˇ   «É4      «É8      «É<     Ë¸ˇˇˇπ 	ò ∫P  â¯j j j jj jjˇjÄË¸ˇˇˇπ	ò ∫P  â¯Éƒ$j j j jj j?j j Ë¸ˇˇˇπ	ò ∫P  â¯Éƒ j h   j jj hˇ  j j Ë¸ˇˇˇπ	ò ∫P  â¯Éƒ j j j jj hˇ  jˇh ˛ˇˇË¸ˇˇˇã´¸   â{lâ¯Éƒ ÖÌuCË¸ˇˇˇ1“âË¸ˇˇˇ∫   â$âË¸ˇˇˇ∫   â«âË¸ˇˇˇ¡Áâ¬â¯–Ä<$ÏÖP  È+  Ë¸ˇˇˇÉƒâË[^_]√êΩÌˇˇˇÎÓΩÙˇˇˇÎÁ                      P                      $                                                                          @                               Ä           P  ¿    †  `  –  P      7%s: %s: failed
 7%s: status: 0x%04x
 7%s: g_input_status
 7%s: querystd
 on off 7%s: s_stream %s
 7%s: input switched to %s
 7%s: s_std %llx
 7%s: norm switched to NTSC
 7%s: norm switched to PAL
 7%s: norm switched to SECAM
 vpx3214c vpx3216b vpx3220a 6%s: %s found @ 0x%x (%s)
 vpx3220 internal composite svideo É√ph   Sh    Ë¸ˇˇˇÉ»ˇÉƒÈ«   QÉ«pâL$Wh   Ë¸ˇˇˇãL$ÉƒÈ)  ç@pâ$Ph(   Ë¸ˇˇˇXZã$È  çCpâ$Ph>   Ë¸ˇˇˇXZã$È6  É∆ph(   Vh    Ë¸ˇˇˇÉƒÈû  â$Ö“∏N   ∫Q   D¬PçFpPhU   Ë¸ˇˇˇãL$ÉƒÈ◊  çCpˇ4ïå  Phh   Ë¸ˇˇˇÉƒÈ)  çFpPhœ   Ë¸ˇˇˇXZÈÓ  çFpPh≥   Ë¸ˇˇˇ[XÈÓ  çFpPhñ   Ë¸ˇˇˇXZÈÓ  çFpUWPhÑ   Ë¸ˇˇˇÉƒÈâ  f=ÄFÑ≥   f=`BÑ‚   ∫Ì   f=ÄBÑù   ãN∑¿çë  R∑V1ˆ“RP∂|$çCpWPh    Ë¸ˇˇˇÉƒãÉî   ∂é°  ∂ñ†  ãx\àå0  Ë¸ˇˇˇÖ¿yq1ˆ∑éb  ∂ñ`  âÿÉ∆ËL  É˛(u„1ˆ∑é"  ∂ñ   âÿÉ∆ËL  É˛(u„È—  ∫ˇ   ãF  P∑F1ˆ¿PçCpRPh  Ë¸ˇˇˇÉƒÈhˇˇˇÉ∆É˛$táÈ[ˇˇˇ∫ˆ   Î√6%s: chip (%02x:%04x) found @ 0x%x (%s)
 Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇ                                vpx3220_fp_read vpx3220_fp_write        vpx3220a                vpx3216b                vpx3214c                                                   ¿                             `                       ¿                                                              –                                              †      Y   † £   §   ®  ≤  æ  X   &   K ò                        Ú 3ÿ® ‡ˇ· ‚„Ä‰ÂÄÊ Á‡Ë¯ÍäÒ¯˘$                            à  â  ä  ã  å  ç è   w Ú ’Á A                        à  â  ä  ã  å  ç è   w Ú —Á A                           à  â  ä  ã   å Äç Äè   s Ú  Á ·,  5  ?  debug parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=Laurent Pinchart description=vpx3220a/vpx3216b/vpx3214c video decoder driver   ò            ˇ      GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                                                             _                                	 ,   Ä   d     ≈           <           	 Q           \      Ò     k          	      &     ñ   =       	 ≤      '     √   X       	 Ÿ   P  o     Ò   s       	   (          ¿  L     *  é   -    	 @    Ü     R           _  ª       	 v  å       }  †  Ω     å  `  j    ö  ‡  (     •     (     Æ  `  ,     ∏  ÿ   W    	 À  –  r    ÿ  †  $     ‰  `  (                   Ï  P  û    ˙  †           P         /  ˇ    	               *           >      Ä     M      
                   a  @   `     l     0     }  ¿   P     è           §          Ω           À  ò       ›  0        Ù  <        
  T   <     %             0             N             e             r             ã             ö             Æ             »             ·             È                                       )             >             [             m             Ö             ì             ß           ≥             ¬      
     —  @   `     Ù              vpx3220.c vpx3220_remove vpx3220_fp_status vpx3220_fp_read vpx3220_fp_read.cold __func__.2 vpx3220_status vpx3220_status.cold vpx3220_g_input_status vpx3220_g_input_status.cold vpx3220_querystd vpx3220_querystd.cold vpx3220_fp_write.isra.0 vpx3220_fp_write.isra.0.cold __func__.1 vpx3220_s_stream vpx3220_s_stream.cold vpx3220_s_routing input_vals.3 vpx3220_s_routing.cold inputs vpx3220_s_ctrl vpx3220_s_std init_secam init_pal init_ntsc vpx3220_s_std.cold vpx3220_init init_common init_fp vpx3220_probe vpx3220_ops vpx3220_ctrl_ops vpx3220_probe.cold vpx3220_driver_init vpx3220_driver vpx3220_driver_exit vpx3220_id vpx3220_core_ops vpx3220_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free current_task i2c_smbus_read_byte_data __const_udelay __SCT__cond_resched i2c_smbus_write_word_data i2c_smbus_read_word_data _printk i2c_smbus_write_byte_data __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vpx3220_id_device_table param_ops_int      9     :     ;  !   9  2   <  L   =  [   >  y   ?  Å   9  û   @  ∫   A  —     Ò   9  !    Ò  9  ˝    !  9  .    Q  9  v  @  ô  @  ±    ¡  9  œ       C    9  !    6  .  L  C  d  .  Ö  >  °  9  Ï  C  8  C  a  9  ~    º  .  ﬁ  .  Ê      >  ;  .  ^  .  e    Ä  .  ¶  .  Æ    —  9  ‚  .    C    .  #  .  F  .  u  .  ñ  .  ©  .  ∆  .  ‡  .    .    .  6  .  Q  9  h  D  ä  E  ô  .  ¶  F  ﬁ  G  Ë  .  ˇ  H  	  .  #  H  -  .  M  H  W  .  w  H  é  I  ó  =  ¶  =  ¥  =  —  ;  Ÿ     )        6    π    ◊    )    â    Ó    n    ª    «    Ã                   h   .                                                 $     (     ,     0     4   *     .  
        B  (     -   B  E     J   B  `     e   B  w   .  }     Ç   B  î     ô     ¶     ´   B  ¡   .  «     Ã   B  ›     ‚   B  Ú     ˜   B        B      #  B  F    v  %  {  B  ã  .  í  .  °  C  Æ  .  µ  .  ø    Õ  .  ‘  .  ﬁ    Ì    	      B  (         9     T     o     ä     ∑     ‘     È     ˛         +    Ë       9          J     K          M  †   .  ¨   .  ¿     Ã     ÿ     ‰     Ë     (    X    å    ê    î        .     J     P        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   Ó                    	   @       T  ‡              )             @	  Ä                   %   	   @       4                  /             ¿	                    8             ¿	  8                  4   	   @       T  p               E      2       ¯	  F                X             >  .                 T   	   @       ƒ  ∏     	         g      2       l  *                 z             ñ                    v   	   @       |                   â             ™  
                  Ö   	   @       ú                  ò             ¿  û                  î   	   @       ¨   `               †             ^  ê                  ≠                                 ©   	   @       !                  µ      0                          æ                                   Œ               (                                @       9         	              P                                 ,!  ·                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  include/asm-generic/fixmap.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/sched/clock.h \
    $(wildcard include/config/HAVE_UNSTABLE_SCHED_CLOCK) \
  kernel/irq/debug.h \
  kernel/irq/settings.h \

kernel/irq/migration.o: $(deps_kernel/irq/migration.o)

$(deps_kernel/irq/migration.o):
                                                                                                                                                                                                                   ELF                      <      4     (               Ë¸ˇˇˇU1¿WVSÉÏË¸ˇˇˇ;    Éé  â≈ç¥&    âËË¸ˇˇˇçplâ√âË¸ˇˇˇãS$ã{(â$ã
ÖˇÑB  ãG(Ö¿Ñ7  ˆ≈ÖV  ˜¡  @ âL$ÑF  Ë¸ˇˇˇã$âD$ãJÖ…ãL$Ñ;  çBâ$ãT$ã$âL$Ë¸ˇˇˇã    ãL$9–s'ã$«D$    ã #    tÛº¿âD$;T$Ü   ã$ãT$£É–   âÿâ$Ë¸ˇˇˇ∫   âÿË¸ˇˇˇçìÄ   ã$Ñ¿uãC$çPçCÅ· Ä â$âL$uãOÖ…tâT$Ë¸ˇˇˇãT$ãπ    #    tÛº¿â¡9    wãK$∫    ã©    Öü   ã$1…Ë¸ˇˇˇÖ¿uiãD$Ö¿uãW Ö“t
ã$Ë¸ˇˇˇfêâË¸ˇˇˇçEË¸ˇˇˇâ≈9    á{˛ˇˇÉƒ[^_]√çv 1“âÿË¸ˇˇˇÎÕçt& êãC$É¿â$ÈΩ˛ˇˇç¥&    çv âD$∫    ∏    Ë¸ˇˇˇãL$Ö¿ÑxˇˇˇÈ¸ˇˇˇçt&   Ä ââÿË¸ˇˇˇÈuˇˇˇçt& êË¸ˇˇˇUâ≈WVSÉÏË¸ˇˇˇ1¿Ë¸ˇˇˇ;    sâ√çt& âÿË¸ˇˇˇçxlâ∆â¯Ë¸ˇˇˇãF$˜     tBãN@Ö…t;ãV(Ö“t4çP£hs+ãF$ã ©  Ä uw©   t`∏   â$Ë¸ˇˇˇÑ¿u/ç¥&    â¯Ë¸ˇˇˇçCË¸ˇˇˇâ√9    wáË¸ˇˇˇ1¿Éƒ[^_]√∏   Ë¸ˇˇˇãV$â¡ã ˜–#Bt¿£)ã$s∏çF1…Ë¸ˇˇˇÎ¨çt& 1…∫   âË¸ˇˇˇÎò    à  
                   4Eff. affinity %*pbl of IRQ %u contains only offline CPUs after offlining CPU %u
  4IRQ%u: set affinity failed(%d).
 QˇshT   Ë¸ˇˇˇÉƒÈs  âL$ˇt$ˇsˇt$Rh    Ë¸ˇˇˇãL$ÉƒÈ‰           migrate_one_irq  GCC: (GNU) 11.2.0           GNU  ¿        ¿                                  Òˇ                                                                   
                    <                  
 A           ^              i              z              Ç              é              ù              ¥              ƒ              œ              ·              ˘                           )             =             V             g             t             ê             ò          ∞             ¿             ”             Ë             ˝                          $             <              cpuhotplug.c __func__.0 _rs.1 irq_migrate_all_off_this_cpu.cold irq_migrate_all_off_this_cpu __fentry__ irq_get_next_irq nr_irqs irq_to_desc _raw_spin_lock debug_smp_processor_id cpumask_any_but nr_cpu_ids __cpu_online_mask irq_force_complete_move irq_fixup_move_pending __x86_indirect_thunk_ecx irq_do_set_affinity __x86_indirect_thunk_edx _raw_spin_unlock ___ratelimit irq_shutdown_and_deactivate _printk irq_affinity_online_cpu irq_lock_sparse _raw_spin_lock_irq housekeeping_enabled _raw_spin_unlock_irq irq_unlock_sparse housekeeping_cpumask irq_set_affinity_locked irq_startup                +     7     s     ü     •     ¿     Ó     ˙     /    @    N    X    o    ä    ì    õ    £    Ω    Â  	  Í    Ô        !    /    6    <    K    W    ô     ´  !  ≥    ª    ¬  "  ÷  #  ˆ  $  
  %  ‘               
     '     ,          8                 .symtab .strtab .shstrtab .rel.text .data .bss .rodata.str1.4 .rel.text.unlikely .rel__mcount_loc .rodata .comment .note.GNU-stack .note.gnu.property                                                       @                       	   @       	  H              %             P                    +             l                     0      2       l  w                 C             „  <                  ?   	   @       d
  0               V                                 R   	   @       î
                 c             (                    k      0       8                   t              K                     Ñ             L  (                                t  `     
         	              ‘  H                               §
  ó                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 î            `;  t                  ê  	   @       Ëh  ∞   #            †            ‘=                    ú  	   @       òi  (   #            µ     0       Ë=                   æ             ˚=                     Œ            ¸=  (                                $>  †
  $   @         	              ƒH  ™                               ¿i  ·                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_drivers/media/i2c/vpx3220.o := gcc -Wp,-MMD,drivers/media/i2c/.vpx3220.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"vpx3220"' -DKBUILD_MODNAME='"vpx3220"' -D__KBUILD_MODNAME=kmod_vpx3220 -c -o drivers/media/i2c/vpx3220.o drivers/media/i2c/vpx3220.c 

source_drivers/media/i2c/vpx3220.o := drivers/media/i2c/vpx3220.c

deps_drivers/media/i2c/vpx3220.o := \
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
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/ratelimit_types.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/msr.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
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
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/COMPAT) \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/osq_lock.h \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/debug_locks.h \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/generated/bounds.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/delay.h \
  include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/rhashtable-types.h \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/ACPI) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/ioport.h \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/irqhandler.h \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/fwnode.h \
  include/linux/resource_ext.h \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  include/linux/ctype.h \
  arch/x86/include/asm/acenv.h \
  include/acpi/acnames.h \
  include/acpi/actypes.h \
  include/acpi/acexcep.h \
  include/acpi/actbl.h \
  include/acpi/actbl1.h \
  include/acpi/actbl2.h \
  include/acpi/actbl3.h \
  include/acpi/acrestyp.h \
  include/acpi/platform/acenvex.h \
  include/acpi/platform/aclinuxex.h \
  include/acpi/platform/acgccex.h \
  include/acpi/acoutput.h \
  include/acpi/acpiosxf.h \
  include/acpi/acpixf.h \
  include/acpi/acconfig.h \
  include/acpi/acbuffer.h \
  include/linux/dynamic_debug.h \
  include/acpi/acpi_bus.h \
    $(wildcard include/config/X86_ANDROID_TABLETS) \
    $(wildcard include/config/ACPI_SYSTEM_POWER_STATES_SUPPORT) \
    $(wildcard include/config/ACPI_SLEEP) \
  include/acpi/acpi_drivers.h \
    $(wildcard include/config/ACPI_DOCK) \
  include/acpi/acpi_numa.h \
    $(wildcard include/config/ACPI_HMAT) \
  include/acpi/acpi_io.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/acpi.h \
    $(wildcard include/config/ACPI_APEI) \
  include/acpi/pdc_intel.h \
  arch/x86/include/asm/numa.h \
    $(wildcard include/config/NUMA_EMU) \
  arch/x86/include/asm/numa_32.h \
  include/linux/regulator/consumer.h \
    $(wildcard include/config/REGULATOR) \
  include/linux/suspend.h \
    $(wildcard include/config/VT) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/HIBERNATION_SNAPSHOT_DEV) \
    $(wildcard include/config/PM_SLEEP_DEBUG) \
    $(wildcard include/config/PM_AUTOSLEEP) \
  include/linux/swap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
    $(wildcard include/config/CGROUP_WRITEBACK) \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/list_lru.h \
  include/linux/shrinker.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/page_counter.h \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/sizes.h \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bvec.h \
  include/linux/highmem.h \
  include/linux/cacheflush.h \
  arch/x86/include/asm/cacheflush.h \
  include/asm-generic/cacheflush.h \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  include/linux/pagemap.h \
  include/linux/hugetlb_inline.h \
  include/uapi/linux/mempolicy.h \
  include/linux/freezer.h \
  include/uapi/linux/i2c.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
  include/media/media-devnode.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/v4l2-subdev.h \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/vpx3220.o: $(deps_drivers/media/i2c/vpx3220.o)

$(deps_drivers/media/i2c/vpx3220.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ELF                      §k      4     ( & %             Ë¸ˇˇˇUd°    ãÄp  âÂ]ã@ô√ç∂    Ë¸ˇˇˇUâ¬d°    ãÄp  âÂâP1¿1“]√êË¸ˇˇˇWVSâ”â ∑KfÖ…Ñá   fÉ˘uq∂KâŒ¿ÈÉÊÉ¯Ñ§   ¡·∏    ∆a	ÒfâBçBàJ«B    ∂K∂«B    É„ÉÀaàZâÀÉ·¡„	ŸàJ	1…fâJ
ã|$)–¡¯«   [^_√ê[1¿^_√ç¥&    çv ∂ã|$∂KÉ‡«   [É»a^_àJà∏   fâB∏   «B    √çv Ä˘	Ñø   â<	Ñù   π	   â∂¯çAˇÉ‡9œD»â»∆{¡‡«B    	æ$   fâr∂ÒàB∂C¡Êâ˜«B    ÅŒy $ É‡Åœa   ¡‡	¯1ˇâB∂C∆BcÉ‡fâz	»«B    àB∂KçB «B    É·¡·	ÒâJÈ˙˛ˇˇç∂    Ä˘ï¡É¡Èjˇˇˇç¥&    çv π   ÈFˇˇˇç∂    Ë¸ˇˇˇUâÂÖ¿tã ]ã@$ãê  ãÄ   √ê∏    ]ãê  ãÄ   √ç¥&    ç∂    Ë¸ˇˇˇUWVâ∆Sâ”â ∑CfÉ¯Ñì  ifÉ¯Ñ«  fÉ¯u)∂C∆a«A    àA∏   fâA∏   [^_]√çt& êfÖ¿Öˇ   ∂C∆aàA1…[∏   ^_fâJ«B    ]√ç∂    fÉ¯ Ñﬁ   fÉ¯$ÖÑ   ∂Câ¡¿ËÉ·É˛Ño  ¡‡æ   ∆aøp  	»fârΩ   àB«B    ∂C∆Baâ¡É‡fâz
¡·«B    	»àB	∂C∆Baâ¡É‡fâj¡·«B    	»àB∏   È.ˇˇˇfÉ¯u:∂Câ¡¿ËÉ·É˛Ñ›   ∆a¡‡[^«B    	¡∏   _]fâB∏   àJ√[1¿^_]√ç¥&    fê∂Câ¡¿ËÉ·É˛Ñk  ∆a¡‡[^«B    	¡∏   _]fâB∏   àJ√ç∂    ∂Câ¡¿ËÉ·É˛Ñ#  ∆a¡‡[^«B    	¡∏   _]fâB∏   àJ√ç∂    ∂C∆aàAπ   [∏   ^_fâJ«B    ]√çt& ∆cÈˇˇˇ<	Ñÿ   æ	   Ä˘	Ñ‘   çFˇ∂˘É‡9˜Dâ∆{¡‡«B    	»âÒàB∂˘∏   fâBâ˘∂C¡Á¡·«B    âÕÉ‡«B    ¡‡ÅÕa  	ËâBâ»Å…y  a p	¯âB∂C∆BcÉ‡«B    	àB∏   fâB∂C«B$    É‡¡‡	»âB ∏   Èi˝ˇˇçv ∆cÈÿ˛ˇˇ∆cÈê˛ˇˇç¥&    êæ   È,ˇˇˇ<ï¿çpÈ-ˇˇˇç¥&    fêË¸ˇˇˇ1¿√ç¥&    êË¸ˇˇˇÈ¸ˇˇˇç∂    Ë¸ˇˇˇUWVâ∆∏    Sâ”Ë¸ˇˇˇãF(Ö¿t2ãÄî   ãx<ãh@∏    Ë¸ˇˇˇâ{âkãF,âC[1¿^_]√ç¥&    fê1ˇ1ÌÎ‘ç¥&    çv Ë¸ˇˇˇçP«@‡ˇˇˇçHâPâPã    «@  ∏    È¸ˇˇˇêË¸ˇˇˇUWVâ÷Sâ√∏    Ë¸ˇˇˇãC(Ö¿t2ãÄî   ãx<ãh@∏    Ë¸ˇˇˇˇs,UWh    VË¸ˇˇˇÉƒ[^_]√çt& 1ˇ1ÌÎ‘ç¥&    çv UWâ◊VSâ√ÉÏ d°    âD$1¿ˆCÖß   >çt& ãK$çS0â¯Ë¸ˇˇˇâ≈ˆCÖŸ   ãD$d+    Ö   Éƒ âË[^_]√ç∂    Ë¸ˇˇˇãs$çS0âD$â¯Ë¸ˇˇˇãsâ≈Ë¸ˇˇˇ4Ö    úè$˙ÉFÉÉV Ë¸ˇˇˇãL$1“)»FVÉF˜$   Ñzˇˇˇ˚Ètˇˇˇç¥&    ãB «B     âD$ãB$«B$    âD$ãB(«B(    âD$ãB,«B,    âD$ãB0«B0    âD$Èˇˇˇçt& êãD$âG ãD$âG$ãD$âG(ãD$âG,ãD$âG0Èˇ˛ˇˇË¸ˇˇˇç¥&    çt& Ë¸ˇˇˇUâÂWVSÉÏãPãMãuÖ“t]ãxâ√∫ˇˇˇˇ∏ÍˇˇˇÖˇt=É{ t7â$Ö…t=Öˆt9∏˘ˇˇˇ∫ˇˇˇˇÅ˛ˇ  wã$âÒâ¯Ë¸ˇˇˇâs1¿1“«C   Éƒ[^_]√çt& êÉƒ∏Íˇˇˇ∫ˇˇˇˇ[^_]√ç∂    Ë¸ˇˇˇWVSãt$Ö¿tbâ”Ö“tBâ«â Ö…tBÖˆt>9ÛâÒFÀË¸ˇˇˇ9Ûw∆Dˇ ∏˘ˇˇˇ[^_√)Ûç71“âŸË¸ˇˇˇâ[^_√çt& ∏˘ˇˇˇÎ›êâŸ1“â¯Ë¸ˇˇˇ∏ÍˇˇˇÎ ∏ÍˇˇˇÎ√ç¥&    Ë¸ˇˇˇUâÂWç}SãHããWˇpâÿËaˇˇˇYçe¯[ô_]√ç¥&    êË¸ˇˇˇUâÂWç}SããWãxÖˇu?Ö€t#Ö“tâ—âÿ1“Ë¸ˇˇˇçe¯∏Íˇˇˇ∫ˇˇˇˇ[_]√çv çe¯∏Íˇˇˇ∫ˇˇˇˇ[_]√ç¥&    ãHˇpâÿËÎ˛ˇˇYçe¯[ô_]√fêË¸ˇˇˇUWVâ∆Sâ”ÉÏã∫§   d°    âD$1¿ãÇ†   ∑íî   â$ÖˆÑ6  âÕ∂N∏   ”‡%øÔˇˇÑ˚   ∑FÉ‡˜fÉ¯Ö  ãK$)◊ãÜ‰  âsâL$ãKTãs8âì§   ˘âKT+KX â4$âS8É˝ÑÛ   ãÑ®ÿ  «D$    âD$Ë¸ˇˇˇË¸ˇˇˇãD$çL$dã    çhã≤p  âäp  ã@Ö¿t,êâ⁄âl$Ë¸ˇˇÖ¿uÅ|$ ˇˇw«D$ˇˇˇˇãEÉ≈Ö¿u’d°    â∞p  Ë¸ˇˇˇË¸ˇˇˇãD$Ö¿uOãSTã$)˙âK8âST;SXÇ  ª§   ã|$â{ãT$d+    ÖÙ   Éƒ[^_]√ç¥&    çv 1¿ÎŸçt& = ˇˇ∫ÚˇˇˇF¬Î¢ç¥&    fêã®‹  «D$    Ë¸ˇˇˇË¸ˇˇˇçud°    ãàp  âL$çL$âàp  ãE1ÌÖ¿t3fêâ⁄ât$Ë%˚ˇˇ®uÅ|$ ˇˇw«D$ˇˇˇˇ—ËÉ∆	≈ãÖ¿u“ÉÂãL$d°    âàp  Ë¸ˇˇˇË¸ˇˇˇãT$çD- Ö“Ñ˛˛ˇˇÅ˙ ˇˇ∏ÚˇˇˇF–ÖÌ∏   D¬È·˛ˇˇç¥&    êË¸ˇˇˇç¥&    fêË¸ˇˇˇUWâœVâ÷Sâ√ÉÏã@ Ö¿tË‚ˇˇˇâ≈Ö¿x|ããããË¸ˇˇˇâ√Ö¿xSã)¡âÎtG∫   ∏   â$ã.9—F—Ë¸ˇˇˇã$É¯v9¡Ö¸ˇˇˇçP9¡âËE ∫   Ë¸ˇˇˇÖ¿x,√)Éƒâÿ[^_]√çt& 1ÌÎàçt& Éƒâ√âÿ[^_]√çt& Éƒâ√âÿ[^_]√çt& Ë¸ˇˇˇUâÂVâ∆SÉÏãMãUd°    âD$1¿ãEâL$â$Ö¿tYÉ‚t,1€ãVãË¸ˇˇˇ√Ö¿I√ôãL$d+    u?Éƒ[^]√çt& ãÖ¿t"ã@ â‚çL$ËÃ˛ˇˇâ√ôÖ¿xÕãL$ã$Î≤çt& ∏Íˇˇˇ∫ˇˇˇˇÎ¥Ë¸ˇˇˇç¥&    Ë¸ˇˇˇÖ¿xOÉ˘tJVâ÷Sâ√çÉ¯w4âÿô˜˛Ö“u+É˚6ãD$«@   É˛wçVˇ∏   ÖÚtç¥&    çv 1¿[^√çv 1¿√çt& êÉ˛[^î¿√ç¥&    Ë¸ˇˇˇÖ¿x?Wâœ1…Vâ÷Sâ√çÉ¯w!âÿô˜˛Ö“uÉ˚~&ÉÎÉ˚wÉ˛î¡Éˇt3[â»^_√çt& ê1¿√çt& êÉˇuËãD$«@   É˛wÿÎç¥&    fêãD$1…«@   É˛wªçFˇÖî¡Î±çt& Ë¸ˇˇˇÉ¯'wVVâ÷ôSâ√˜˛1¿Ö“u"É˘trÉ˚Ñ°   Ö€t;É˚uNÉ˛Ñ≠   [^√fêÉ˚$u;1¿É˛uÔãD$Éxî¿Î‚ç¥&    1¿√çt& êÉ˛uŒãD$«    ∏   ÎΩfêÉ˛î¿Î≥ç¥&    êÉ˚ tÎ!É„˚É˚uúÉ˛uóãD$Éxî¿Îäç¥&    É˚$tàÈyˇˇˇç∂    É˛ÖjˇˇˇãD$« 	   ∏   ÈVˇˇˇçv ãD$«    ∏   È?ˇˇˇçt& Ë¸ˇˇˇÉ¯hÑ≤   w É¯fÑó   ∫   É¯guEâ–√ç¥&    çv É¯jtk∫    =†   t‚∫    É¯itÿ=∫   t~=ª   u9∫‡  â–√ç¥&    ∫Ä  É¯et±É¯Qtyw“É¯tb∫    É¯Ptõ∫    É¯tëÈ¸ˇˇˇç¥&    fêde/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  include/asm-generic/bug.h \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  include/linux/restart_block.h \
  include/linux/time64.h \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/vdso/math64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/COMPAT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
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
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/TRACEPOINTS) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wil/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <waiman.long@hpe.com>
 */

/*
 * Collect locking event counts
 */
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/fs.h>

#include "lock_events.h"

#undef  LOCK_EVENT
#define LOCK_EVENT(name)	[LOCKEVENT_ ## name] = #name,

#define LOCK_EVENTS_DIR		"lock_event_counts"

/*
 * When CONFIG_LOCK_EVENT_COUNTS is enabled, event counts of different
 * types of locks will be reported under the <debugfs>/lock_event_counts/
 * directory. See lock_events_list.h for the list of available locking
 * events.
 *
 * Writing to the special ".reset_counts" file will reset all the above
 * locking event counts. This is a very slow operation and so should not
 * be done frequently.
 *
 * These event counts are implemented as per-cpu variables which are
 * summed and computed whenever the corresponding debugfs files are read. This
 * minimizes added overhead making the counts usable even in a production
 * environment.
 */
static const char * const lockevent_names[lockevent_num + 1] = {

#include "lock_events_list.h"

	[LOCKEVENT_reset_cnts] = ".reset_counts",
};

/*
 * Per-cpu counts
 */
DEFINE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * The lockevent_read() function can be overridden.
 */
ssize_t __weak lockevent_read(struct file *file, char __user *user_buf,
			      size_t count, loff_t *ppos)
{
	char buf[64];
	int cpu, id, len;
	u64 sum = 0;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	id = (long)file_inode(file)->i_private;

	if (id >= lockevent_num)
		return -EBADF;

	for_each_possible_cpu(cpu)
		sum += per_cpu(lockevents[id], cpu);
	len = snprintf(buf, sizeof(buf) - 1, "%llu\n", sum);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

/*
 * Function to handle write request
 *
 * When idx = reset_cnts, reset all the counts.
 */
static ssize_t lockevent_write(struct file *file, const char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	int cpu;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	if ((long)file_inode(file)->i_private != LOCKEVENT_reset_cnts)
		return count;

	for_each_possible_cpu(cpu) {
		int i;
		unsigned long *ptr = per_cpu_ptr(lockevents, cpu);

		for (i = 0 ; i < lockevent_num; i++)
			WRITE_ONCE(ptr[i], 0);
	}
	return count;
}

/*
 * Debugfs data structures
 */
static const struct file_operations fops_lockevent = {
	.read = lockevent_read,
	.write = lockevent_write,
	.llseek = default_llseek,
};

#ifdef CONFIG_PARAVIRT_SPINLOCKS
#include <asm/paravirt.h>

static bool __init skip_lockevent(const char *name)
{
	static int pv_on __initdata = -1;

	if (pv_on < 0)
		pv_on = !pv_is_native_spin_unlock();
	/*
	 * Skip PV qspinlock events on bare metal.
	 */
	if (!pv_on && !memcmp(name, "pv_", 3))
		return true;
	return false;
}
#else
static inline bool skip_lockevent(const char *name)
{
	return false;
}
#endif

/*
 * Initialize debugfs for the locking event counts.
 */
static int __init init_lockevent_counts(void)
{
	struct dentry *d_counts = debugfs_create_dir(LOCK_EVENTS_DIR, NULL);
	int i;

	if (!d_counts)
		goto out;

	/*
	 * Create the debugfs files
	 *
	 * As reading from and writing to the stat files can be slow, only
	 * root is allowed to do the read/write to limit impact to system
	 * performance.
	 */
	for (i = 0; i < lockevent_num; i++) {
		if (skip_lockevent(lockevent_names[i]))
			continue;
		if (!debugfs_create_file(lockevent_names[i], 0400, d_counts,
					 (void *)(long)i, &fops_lockevent))
			goto fail_undo;
	}

	if (!debugfs_create_file(lockevent_names[LOCKEVENT_reset_cnts], 0200,
				 d_counts, (void *)(long)LOCKEVENT_reset_cnts,
				 &fops_lockevent))
		goto fail_undo;

	return 0;
fail_undo:
	debugfs_remove_recursive(d_counts);
out:
	pr_warn("Could not create '%s' debugfs entries\n", LOCK_EVENTS_DIR);
	return -ENOMEM;
}
fs_initcall(init_lockevent_counts);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef __LOCKING_LOCK_EVENTS_H
#define __LOCKING_LOCK_EVENTS_H

enum lock_events {

#include "lock_events_list.h"

	lockevent_num,	/* Total number of lock event counts */
	LOCKEVENT_reset_cnts = lockevent_num,
};

#ifdef CONFIG_LOCK_EVENT_COUNTS
/*
 * Per-cpu counters
 */
DECLARE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * Increment the statistical counters. use raw_cpu_inc() because of lower
 * overhead and we don't care if we loose the occasional update.
 */
static inline void __lockevent_inc(enum lock_events event, bool cond)
{
	if (cond)
		raw_cpu_inc(lockevents[event]);
}

#define lockevent_inc(ev)	  __lockevent_inc(LOCKEVENT_ ##ev, true)
#define lockevent_cond_inc(ev, c) __lockevent_inc(LOCKEVENT_ ##ev, c)

static inline void __lockevent_add(enum lock_events event, int inc)
{
	raw_cpu_add(lockevents[event], inc);
}

#define lockevent_add(ev, c)	__lockevent_add(LOCKEVENT_ ##ev, c)

#else  /* CONFIG_LOCK_EVENT_COUNTS */

#define lockevent_inc(ev)
#define lockevent_add(ev, c)
#define lockevent_cond_inc(ev, c)

#endif /* CONFIG_LOCK_EVENT_COUNTS */
#endif /* __LOCKING_LOCK_EVENTS_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef __LOCKING_LOCK_EVENTS_H
#define __LOCKING_LOCK_EVENTS_H

enum lock_events {

#include "lock_events_list.h"

	lockevent_num,	/* Total number of lock event counts */
	LOCKEVENT_reset_cnts = lockevent_num,
};

#ifdef CONFIG_LOCK_EVENT_COUNTS
/*
 * Per-cpu counters
 */
DECLARE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * Increment the statistical counters. use raw_cpu_inc() because of lower
 * overhead and we don't care if we loose the occasional update.
 */
static inline void __lockevent_inc(enum lock_events event, bool cond)
{
	if (cond)
		raw_cpu_inc(lockevents[event]);
}

#define lockevent_inc(ev)	  __lockevent_inc(LOCKEVENT_ ##ev, true)
#define lockevent_cond_inc(ev, c) __lockevent_inc(LOCKEVENT_ ##ev, c)

static inline void __lockevent_add(enum lock_events event, int inc)
{
	raw_cpu_add(lockevents[event], inc);
}

#define lockevent_add(ev, c)	__lockevent_add(LOCKEVENT_ ##ev, c)

#else  /* CONFIG_LOCK_EVENT_COUNTS */

#define lockevent_inc(ev)
#define lockevent_add(ev, c)
#define lockevent_cond_inc(ev, c)

#endif /* CONFIG_LOCK_EVENT_COUNTS */
#endif /* __LOCKING_LOCK_EVENTS_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef LOCK_EVENT
#define LOCK_EVENT(name)	LOCKEVENT_ ## name,
#endif

#ifdef CONFIG_QUEUED_SPINLOCKS
#ifdef CONFIG_PARAVIRT_SPINLOCKS
/*
 * Locking events for PV qspinlock.
 */
LOCK_EVENT(pv_hash_hops)	/* Average # of hops per hashing operation */
LOCK_EVENT(pv_kick_unlock)	/* # of vCPU kicks issued at unlock time   */
LOCK_EVENT(pv_kick_wake)	/* # of vCPU kicks for pv_latency_wake	   */
LOCK_EVENT(pv_latency_kick)	/* Average latency (ns) of vCPU kick	   */
LOCK_EVENT(pv_latency_wake)	/* Average latency (ns) of kick-to-wakeup  */
LOCK_EVENT(pv_lock_stealing)	/* # of lock stealing operations	   */
LOCK_EVENT(pv_spurious_wakeup)	/* # of spurious wakeups in non-head vCPUs */
LOCK_EVENT(pv_wait_again)	/* # of wait's after queue head vCPU kick  */
LOCK_EVENT(pv_wait_early)	/* # of early vCPU wait's		   */
LOCK_EVENT(pv_wait_head)	/* # of vCPU wait's at the queue head	   */
LOCK_EVENT(pv_wait_node)	/* # of vCPU wait's at non-head queue node */
#endif /* CONFIG_PARAVIRT_SPINLOCKS */

/*
 * Locking events for qspinlock
 *
 * Subtracting lock_use_node[234] from lock_slowpath will give you
 * lock_use_node1.
 */
LOCK_EVENT(lock_pending)	/* # of locking ops via pending code	     */
LOCK_EVENT(lock_slowpath)	/* # of locking ops via MCS lock queue	     */
LOCK_EVENT(lock_use_node2)	/* # of locking ops that use 2nd percpu node */
LOCK_EVENT(lock_use_node3)	/* # of locking ops that use 3rd percpu node */
LOCK_EVENT(lock_use_node4)	/* # of locking ops that use 4th percpu node */
LOCK_EVENT(lock_no_node)	/* # of locking ops w/o using percpu node    */
#endif /* CONFIG_QUEUED_SPINLOCKS */

/*
 * Locking events for rwsem
 */
LOCK_EVENT(rwsem_sleep_reader)	/* # of reader sleeps			*/
LOCK_EVENT(rwsem_sleep_writer)	/* # of writer sleeps			*/
LOCK_EVENT(rwsem_wake_reader)	/* # of reader wakeups			*/
LOCK_EVENT(rwsem_wake_writer)	/* # of writer wakeups			*/
LOCK_EVENT(rwsem_opt_lock)	/* # of opt-acquired write locks	*/
LOCK_EVENT(rwsem_opt_fail)	/* # of failed optspins			*/
LOCK_EVENT(rwsem_opt_nospin)	/* # of disabled optspins		*/
LOCK_EVENT(rwsem_rlock)		/* # of read locks acquired		*/
LOCK_EVENT(rwsem_rlock_steal)	/* # of read locks by lock stealing	*/
LOCK_EVENT(rwsem_rlock_fast)	/* # of fast read locks acquired	*/
LOCK_EVENT(rwsem_rlock_fail)	/* # of failed read lock acquisitions	*/
LOCK_EVENT(rwsem_rlock_handoff)	/* # of read lock handoffs		*/
LOCK_EVENT(rwsem_wlock)		/* # of write locks acquired		*/
LOCK_EVENT(rwsem_wlock_fail)	/* # of failed write lock acquisitions	*/
LOCK_EVENT(rwsem_wlock_handoff)	/* # of write lock handoffs		*/
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef LOCK_EVENT
#define LOCK_EVENT(name)	LOCKEVENT_ ## name,
#endif

#ifdef CONFIG_QUEUED_SPINLOCKS
#ifdef CONFIG_PARAVIRT_SPINLOCKS
/*
 * Locking events for PV qspinlock.
 */
LOCK_EVENT(pv_hash_hops)	/* Average # of hops per hashing operation */
LOCK_EVENT(pv_kick_unlock)	/* # of vCPU kicks issued at unlock time   */
LOCK_EVENT(pv_kick_wake)	/* # of vCPU kicks for pv_latency_wake	   */
LOCK_EVENT(pv_latency_kick)	/* Average latency (ns) of vCPU kick	   */
LOCK_EVENT(pv_latency_wake)	/* Average latency (ns) of kick-to-wakeup  */
LOCK_EVENT(pv_lock_stealing)	/* # of lock stealing operations	   */
LOCK_EVENT(pv_spurious_wakeup)	/* # of spurious wakeups in non-head vCPUs */
LOCK_EVENT(pv_wait_again)	/* # of wait's after queue head vCPU kick  */
LOCK_EVENT(pv_wait_early)	/* # of early vCPU wait's		   */
LOCK_EVENT(pv_wait_head)	/* # of vCPU wait's at the queue head	   */
LOCK_EVENT(pv_wait_node)	/* # of vCPU wait's at non-head queue node */
#endif /* CONFIG_PARAVIRT_SPINLOCKS */

/*
 * Locking events for qspinlock
 *
 * Subtracting lock_use_node[234] from lock_slowpath will give you
 * lock_use_node1.
 */
LOCK_EVENT(lock_pending)	/* # of locking ops via pending code	     */
LOCK_EVENT(lock_slowpath)	/* # of locking ops via MCS lock queue	     */
LOCK_EVENT(lock_use_node2)	/* # of locking ops that use 2nd percpu node */
LOCK_EVENT(lock_use_node3)	/* # of locking ops that use 3rd percpu node */
LOCK_EVENT(lock_use_node4)	/* # of locking ops that use 4th percpu node */
LOCK_EVENT(lock_no_node)	/* # of locking ops w/o using percpu node    */
#endif /* CONFIG_QUEUED_SPINLOCKS */

/*
 * Locking events for rwsem
 */
LOCK_EVENT(rwsem_sleep_reader)	/* # of reader sleeps			*/
LOCK_EVENT(rwsem_sleep_writer)	/* # of writer sleeps			*/
LOCK_EVENT(rwsem_wake_reader)	/* # of reader wakeups			*/
LOCK_EVENT(rwsem_wake_writer)	/* # of writer wakeups			*/
LOCK_EVENT(rwsem_opt_lock)	/* # of opt-acquired write locks	*/
LOCK_EVENT(rwsem_opt_fail)	/* # of failed optspins			*/
LOCK_EVENT(rwsem_opt_nospin)	/* # of disabled optspins		*/
LOCK_EVENT(rwsem_rlock)		/* # of read locks acquired		*/
LOCK_EVENT(rwsem_rlock_steal)	/* # of read locks by lock stealing	*/
LOCK_EVENT(rwsem_rlock_fast)	/* # of fast read locks acquired	*/
LOCK_EVENT(rwsem_rlock_fail)	/* # of failed read lock acquisitions	*/
LOCK_EVENT(rwsem_rlock_handoff)	/* # of read lock handoffs		*/
LOCK_EVENT(rwsem_wlock)		/* # of write locks acquired		*/
LOCK_EVENT(rwsem_wlock_fail)	/* # of failed write lock acquisitions	*/
LOCK_EVENT(rwsem_wlock_handoff)	/* # of write lock handoffs		*/
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/lockdep.c
 *
 * Runtime locking correctness validator
 *
 * Started by Ingo Molnar:
 *
 *  Copyright (C) 2006,2007 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2007 Red Hat, Inc., Peter Zijlstra
 *
 * this code maps all the lock dependencies as they occur in a live kernel
 * and will warn about the following classes of locking bugs:
 *
 * - lock inversion scenarios
 * - circular lock dependencies
 * - hardirq/softirq safe/unsafe locking bugs
 *
 * Bugs are reported even if the current locking scenario does not cause
 * any deadlock at this point.
 *
 * I.e. if anytime in the past two locks were taken in a different order,
 * even if it happened for another task, even if those were different
 * locks (but of the same class as this lock), this code will detect it.
 *
 * Thanks to Arjan van de Ven for coming up with the initial idea of
 * mapping lock dependencies runtime.
 */
#define DISABLE_BRANCH_PROFILING
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/interrupt.h>
#include <linux/stacktrace.h>
#include <linux/debug_locks.h>
#include <linux/irqflags.h>
#include <linux/utsname.h>
#include <linux/hash.h>
#include <linux/ftrace.h>
#include <linux/stringify.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/gfp.h>
#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/nmi.h>
#include <linux/rcupdate.h>
#include <linux/kprobes.h>
#include <linux/lockdep.h>

#include <asm/sections.h>

#include "lockdep_internals.h"

#define CREATE_TRACE_POINTS
#include <trace/events/lock.h>

#ifdef CONFIG_PROVE_LOCKING
int prove_locking = 1;
module_param(prove_locking, int, 0644);
#else
#define prove_locking 0
#endif

#ifdef CONFIG_LOCK_STAT
int lock_stat = 1;
module_param(lock_stat, int, 0644);
#else
#define lock_stat 0
#endif

DEFINE_PER_CPU(unsigned int, lockdep_recursion);
EXPORT_PER_CPU_SYMBOL_GPL(lockdep_recursion);

static __always_inline bool lockdep_enabled(void)
{
	if (!debug_locks)
		return false;

	if (this_cpu_read(lockdep_recursion))
		return false;

	if (current->lockdep_recursion)
		return false;

	return true;
}

/*
 * lockdep_lock: protects the lockdep graph, the hashes and the
 *               class/list/hash allocators.
 *
 * This is one of the rare exceptions where it's justified
 * to use a raw spinlock - we really dont want the spinlock
 * code to recurse back into the lockdep code...
 */
static arch_spinlock_t __lock = (arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED;
static struct task_struct *__owner;

static inline void lockdep_lock(void)
{
	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	__this_cpu_inc(lockdep_recursion);
	arch_spin_lock(&__lock);
	__owner = current;
}

static inline void lockdep_unlock(void)
{
	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	if (debug_locks && DEBUG_LOCKS_WARN_ON(__owner != current))
		return;

	__owner = NULL;
	arch_spin_unlock(&__lock);
	__this_cpu_dec(lockdep_recursion);
}

static inline bool lockdep_assert_locked(void)
{
	return DEBUG_LOCKS_WARN_ON(__owner != current);
}

static struct task_struct *lockdep_selftest_task_struct;


static int graph_lock(void)
{
	lockdep_lock();
	/*
	 * Make sure that if another CPU detected a bug while
	 * walking the graph we dont change it (while the other
	 * CPU is busy printing out stuff with the graph lock
	 * dropped already)
	 */
	if (!debug_locks) {
		lockdep_unlock();
		return 0;
	}
	return 1;
}

static inline void graph_unlock(void)
{
	lockdep_unlock();
}

/*
 * Turn lock debugging off and return with 0 if it was off already,
 * and also release the graph lock:
 */
static inline int debug_locks_off_graph_unlock(void)
{
	int ret = debug_locks_off();

	lockdep_unlock();

	return ret;
}

unsigned long nr_list_entries;
static struct lock_list list_entries[MAX_LOCKDEP_ENTRIES];
static DECLARE_BITMAP(list_entries_in_use, MAX_LOCKDEP_ENTRIES);

/*
 * All data structures here are protected by the global debug_lock.
 *
 * nr_lock_classes is the number of elements of lock_classes[] that is
 * in use.
 */
#define KEYHASH_BITS		(MAX_LOCKDEP_KEYS_BITS - 1)
#define KEYHASH_SIZE		(1UL << KEYHASH_BITS)
static struct hlist_head lock_keys_hash[KEYHASH_SIZE];
unsigned long nr_lock_classes;
unsigned long nr_zapped_classes;
unsigned long max_lock_class_idx;
struct lock_class lock_classes[MAX_LOCKDEP_KEYS];
DECLARE_BITMAP(lock_classes_in_use, MAX_LOCKDEP_KEYS);

static inline struct lock_class *hlock_class(struct held_lock *hlock)
{
	unsigned int class_idx = hlock->class_idx;

	/* Don't re-read hlock->class_idx, can't use READ_ONCE() on bitfield */
	barrier();

	if (!test_bit(class_idx, lock_classes_in_use)) {
		/*
		 * Someone passed in garbage, we give up.
		 */
		DEBUG_LOCKS_WARN_ON(1);
		return NULL;
	}

	/*
	 * At this point, if the passed hlock->class_idx is still garbage,
	 * we just have to live with it
	 */
	return lock_classes + class_idx;
}

#ifdef CONFIG_LOCK_STAT
static DEFINE_PER_CPU(struct lock_class_stats[MAX_LOCKDEP_KEYS], cpu_lock_stats);

static inline u64 lockstat_clock(void)
{
	return local_clock();
}

static int lock_point(unsigned long points[], unsigned long ip)
{
	int i;

	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		if (points[i] == 0) {
			points[i] = ip;
			break;
		}
		if (points[i] == ip)
			break;
	}

	return i;
}

static void lock_time_inc(struct lock_time *lt, u64 time)
{
	if (time > lt->max)
		lt->max = time;

	if (time < lt->min || !lt->nr)
		lt->min = time;

	lt->total += time;
	lt->nr++;
}

static inline void lock_time_add(struct lock_time *src, struct lock_time *dst)
{
	if (!src->nr)
		return;

	if (src->max > dst->max)
		dst->max = src->max;

	if (src->min < dst->min || !dst->nr)
		dst->min = src->min;

	dst->total += src->total;
	dst->nr += src->nr;
}

struct lock_class_stats lock_stats(struct lock_class *class)
{
	struct lock_class_stats stats;
	int cpu, i;

	memset(&stats, 0, sizeof(struct lock_class_stats));
	for_each_possible_cpu(cpu) {
		struct lock_class_stats *pcs =
			&per_cpu(cpu_lock_stats, cpu)[class - lock_classes];

		for (i = 0; i < ARRAY_SIZE(stats.contention_point); i++)
			stats.contention_point[i] += pcs->contention_point[i];

		for (i = 0; i < ARRAY_SIZE(stats.contending_point); i++)
			stats.contending_point[i] += pcs->contending_point[i];

		lock_time_add(&pcs->read_waittime, &stats.read_waittime);
		lock_time_add(&pcs->write_waittime, &stats.write_waittime);

		lock_time_add(&pcs->read_holdtime, &stats.read_holdtime);
		lock_time_add(&pcs->write_holdtime, &stats.write_holdtime);

		for (i = 0; i < ARRAY_SIZE(stats.bounces); i++)
			stats.bounces[i] += pcs->bounces[i];
	}

	return stats;
}

void clear_lock_stats(struct lock_class *class)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct lock_class_stats *cpu_stats =
			&per_cpu(cpu_lock_stats, cpu)[class - lock_classes];

		memset(cpu_stats, 0, sizeof(struct lock_class_stats));
	}
	memset(class->contention_point, 0, sizeof(class->contention_point));
	memset(class->contending_point, 0, sizeof(class->contending_point));
}

static struct lock_class_stats *get_lock_stats(struct lock_class *class)
{
	return &this_cpu_ptr(cpu_lock_stats)[class - lock_classes];
}

static void lock_release_holdtime(struct held_lock *hlock)
{
	struct lock_class_stats *stats;
	u64 holdtime;

	if (!lock_stat)
		return;

	holdtime = lockstat_clock() - hlock->holdtime_stamp;

	stats = get_lock_stats(hlock_class(hlock));
	if (hlock->read)
		lock_time_inc(&stats->read_holdtime, holdtime);
	else
		lock_time_inc(&stats->write_holdtime, holdtime);
}
#else
static inline void lock_release_holdtime(struct held_lock *hlock)
{
}
#endif

/*
 * We keep a global list of all lock classes. The list is only accessed with
 * the lockdep spinlock lock held. free_lock_classes is a list with free
 * elements. These elements are linked together by the lock_entry member in
 * struct lock_class.
 */
static LIST_HEAD(all_lock_classes);
static LIST_HEAD(free_lock_classedcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/irqhandler.h \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/CMA) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/linux/seqlock.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/osq_lock.h \
  include/linux/debug_locks.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  include/linux/rbtree_types.h \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/generated/bounds.h \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/tsc.h \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
    $(wildcard include/config/PCI) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/fwnode.h \
  include/linux/vmalloc.h \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  include/linux/overflow.h \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  arch/x8
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/socket.h \
  arch/x86/include/generated/uapi/asm/socket.h \
  include/uapi/asm-generic/socket.h \
  arch/x86/include/generated/uapi/asm/sockios.h \
  include/uapi/asm-generic/sockios.h \
  include/uapi/linux/sockios.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/uapi/linux/socket.h \
  include/linux/net.h \
  include/linux/random.h \
    $(wildcard include/config/VMGENID) \
    $(wildcard include/config/ARCH_RANDOM) \
  include/linux/once.h \
  include/uapi/linux/random.h \
  include/linux/prandom.h \
  include/linux/siphash.h \
  arch/x86/include/asm/archrandom.h \
  include/uapi/linux/net.h \
  include/linux/textsearch.h \
  include/net/checksum.h \
  arch/x86/include/asm/checksum.h \
  arch/x86/include/asm/checksum_32.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/uapi/linux/libc-compat.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/DMA_API_DEBUG) \
    $(wildcard include/config/HAS_DMA) \
    $(wildcard include/config/NEED_DMA_MAP_STATE) \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/dma-direction.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  arch/x86/include/asm/io.h \
    $(wildcard includeELF                      L"      4     (               Ë¸ˇˇˇãÖ¿uãAÉ¯wã≈¨  âA1¿√∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇâ¬â»ãIÖ…upãä8  âHãä<  âHãä@  âHãäD  âHãäH  âHãäL  âHãäP  âH ãäT  âH$ãäX  âH(ãä\  âH,ãä`  âH0ãíd  âP41¿√ê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇãà4  âJãÄ0  âB1¿√ç∂    Ë¸ˇˇˇSãX\âÿË¸ˇˇˇãClË¸ˇˇˇ1¿[√çt& Ë¸ˇˇˇÉÏãÄî   dã    âL$1…πÄˇˇfâL$Ö“t7∆D$j π   çT$Ë¸ˇˇˇX∏ºç Ë¸ˇˇˇãD$d+    u1¿Éƒ√çt& ∆D$Î«Ë¸ˇˇˇçt& Ë¸ˇˇˇSâ√ÉÏãJd°    âD$1¿Ö…tãBÖ¿Öﬂ   «B   ∏   «B   âÉ0  ãB∫Äˇˇπ   fâT$âÉ4  ãÉî   ∆D$ j çT$Ë¸ˇˇˇãì0  πÅˇˇãÉî   fâL$π   àt$j çT$	Ë¸ˇˇˇãì0  πÇˇˇãÉî   fâL$	π   àT$j çT$Ë¸ˇˇˇãì4  ãÉî   ªÑˇˇfâ\$π   àT$j çT$Ë¸ˇˇˇÉƒãD$d+    uÉƒ1¿[√çv k…9»Ü)ˇˇˇÈˇˇˇË¸ˇˇˇçv Ë¸ˇˇˇUâ≈WVSÉÏd°    âD$ãEãPãRË¸ˇˇˇ®Ñ)  ãEXâ$Ö¿Ñ"  h    çuãπ   âË¸ˇˇˇâ√XÖ€Ö¸ˇˇˇ∏d   Ë¸ˇˇˇπ¿  ∫l  âË¸ˇˇˇâ√Ö¿Ñ„  π@   âÍæ†  Ë¸ˇˇˇπ   ∫ÅˇˇÎê∑Nf¡¬àL$ãÉî   É∆fâT$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÕ∫¿  ∆D$ãÉî   π   fâT$j çT$	Ë¸ˇˇˇπ¿D  ãÉî   fâL$	π   ∆D$j çT$Ë¸ˇˇˇπ   ∫¯*  ∏'  Ë¸ˇˇˇπ   ^∫oÄˇˇ_æ|  Îçt& ∑Nf¡¬àL$ãÉî   É∆fâT$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÕ∫Ñ   æ¥  ø@  Îçt& ∑Wf¡∆àT$É«π   fât$ãÉî   j çT$	Ë¸ˇˇˇ∑7XfÖˆuÕ∫%Üˇˇ∆D$ãÉî   π   fâT$ø‡  j çT$	Ë¸ˇˇˇ∫  YÎçv ∑wf¡¬âÒãÉî   É«fâT$àL$π   j çT$	Ë¸ˇˇˇ∑XfÖ“uÀ∫Äˇˇ∆D$ π   ãÉî   fâT$æÇˇˇøÑˇˇ«É0     «É4     j çT$	Ë¸ˇˇˇãì0  πÅˇˇãÉî   fâL$	π   àt$j çT$Ë¸ˇˇˇãì0  ãÉî   π   fât$ç≥¿   àT$j çT$Ë¸ˇˇˇãì4  ãÉî   π   fâ|$çª8  àT$j çT$Ë¸ˇˇˇ1¿π   ∑UÛ´ãD$«É8  Ä  «É<  ‡  «É@     «ÉD     «ÉH     ã âÉh  ãEçà  QçQRˇ∞   ãETˇ0h,   Ë¸ˇˇˇ1…∫   âÉƒ(j Ë¸ˇˇˇπ	ò ∫0  âj há   j jj hˇ   j j Ë¸ˇˇˇπ	ò ∫0  âÉƒ$j jxj jj hˇ   j j Ë¸ˇˇˇπ	ò ∫0  âÉƒ j j j jj jj j Ë¸ˇˇˇπ	ò ∫0  âÉƒ j j j jj jj j Ë¸ˇˇˇâslãõ¸   âÉƒ Ö€u6Ë¸ˇˇˇâ√Ö¿uãD$d+    u:Éƒâÿ[^_]√çt& âË¸ˇˇˇÎ‹ç¥&    Ë¸ˇˇˇÎŒª˚ˇˇˇÎ«ªÍˇˇˇÎ¿ªÙˇˇˇÎπË¸ˇˇˇç¥&    ç¥&    êË¸ˇˇˇSÉÏãHdã    âT$1“ãP(Å˙	ò Ñê   wNÅ˙	ò tnÅ˙	ò u^∫∂ˇˇãY‘ã@|fâT$àD$π   âÿj çT$Ë¸ˇˇˇX1¿ãT$d+    uZÉƒ[√çv Å˙	ò u∫ºˇˇãY‘ã@|fâT$Î∏ç∂    ∏ÍˇˇˇÎƒêãY‘π¥ˇˇã@|fâL$Îòç∂    ãY‘π∫ˇˇã@|fâL$ÎÄË¸ˇˇˇêË¸ˇˇˇUWVâ∆SÉÏd°    âD$1¿ãAÖ¿Öé   ãAâ◊âÀçê˙ﬂˇˇÉ‚˝Ñ%  =  Ñ*  «A   π   ãCãk«C   âKâ¬É‚¸=Å  ∏Ä  C–âËÉ‡¸Å˝·  Ω‡  C≈ã+âSâCÖÌtEãC=   Ñó  =   ÑD  =  tu∏ÍˇˇˇãT$d+    Ö¬  Éƒ[^_]√ç¥&    ãâãSâPãSâPãSâPãSâPãSâPãS âPãS$âPãS(âP ãS,âP$ãS0âP(ãS4âP,1¿Îñç∂    ∫∞ˇˇç|$∆D$ãÜî   fâT$π   â˙j Ë¸ˇˇˇπ%îˇˇãÜî   fâL$∆D$ j π   â˙Ë¸ˇˇˇãCY]ãÆî   =Ä  Ñ∏  =@  ÖÌ  Å{   Ñ⁄  ∏Äˇˇ∆D$π   â˙fâD$âËΩÑˇˇj Ë¸ˇˇˇãSπÉˇˇãÜî   fâL$π   àt$â˙j Ë¸ˇˇˇãSãÜî   π   fâl$	ΩàˇˇàT$â˙j Ë¸ˇˇˇãSπáˇˇãÜî   fâL$π   àt$â˙j Ë¸ˇˇˇãSãÜî   π   fâl$àT$â˙j Ë¸ˇˇˇ∫ûˇˇãÜî   π   fâT$â˙∆D$j Ë¸ˇˇˇÉƒãCâÜ8  ãCâÜ<  ãCâÜ@  ãCâÜD  ãCâÜH  ãCâÜL  ãC âÜP  ãC$âÜT  ãC(âÜX  ãC,âÜ\  ãC0âÜ`  ãC4âÜd  1¿ÈÃ˝ˇˇçt& ø∞ˇˇ∆D$ ãÜî   π   fâ|$ç|$Ω%ñˇˇj â˙Ë¸ˇˇˇãÜî   fâl$∆D$È3˛ˇˇç¥&    ∫∞ˇˇç|$∆D$ ãÜî   fâT$π   â˙j Ë¸ˇˇˇπ%ñˇˇãÜî   fâL$∆D$ÈÎ˝ˇˇç¥&    =†   u!É{xÖ˛ˇˇ∏Äˇˇ∆D$fâD$Î~ç∂    =`  u!Å{   Ö‰˝ˇˇ∏Äˇˇ∆D$fâD$ÎSçv =∞   u!Å{ê   Öº˝ˇˇ∏Äˇˇ∆D$fâD$Î+çv É¯XÖü˝ˇˇÉ{HÖï˝ˇˇπÄˇˇ∆D$fâL$ç∂    j π   â˙âËË¸ˇˇˇXÈC˛ˇˇç¥&    çv Å{‡  ÖS˝ˇˇ∫Äˇˇ∆D$fâT$Î¬fêπ   ÈË˚ˇˇç∂    π   Èÿ˚ˇˇ∏Äˇˇ∆D$fâD$ÎïË¸ˇˇˇ                    ê  ‡                                                                                                                          0   ¿   ‡      p  ê    ‡      VS6624 Chip Enable vs6624   3%s %d-%04x: failed to request GPIO %d
    6%s %d-%04x: chip found @ 0x%02x (%s)
 ã$ˇ0∑EPãEˇ∞   ãETˇ0h    Ë¸ˇˇˇÉƒÈ±  Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇlicense=GPL v2 author=Scott Jiang <Scott.Jiang.Linux@gmail.com> description=VS6624 sensor driver                                vs6624                                                                     †               `                   0   ‡                                                                                         ¿   p                                                                                                        ¥Ñ ∂u ∏ 4Ñ 6u 8 ÇÄ ÑÄ ÜÄ Ä ê˛ Ç  Äd     ( ∫  º  ã  å è  ê ï  ñ ô  ö  Ñ%! Ä  Ñ Ç Ç  ∞  ñ% Ä                            <  f e — f b      &  & &c &— &h &› &: &  Ä$  ä0 ëb íJ ïe ñ °: ¢∏  

 :  ; Ö 	; 
Ö :  0 è      oÄ å                             Å Å  … …G … …Ä …: :ê ;êG <ê …1 	…‹ 
…Ä …D Dê Eê1 Fê‚ … …‡ …Ä …G Gêê HêÉ IêÅ Jê‡ Kê` Lê Mêê Nê¿ OêC Pêt Qê Rê SêÄ Tê Uê‰ Vêê Wê¿ XêC Yê Zê [ê \êÏ …] …  …Ä …] ]ê£ ^ê _ê `ê£ aê bê cê" …r …í …Ä …d dêt eê fê gêr hêï …G …Ú …Å …i iët jë kë lëÏ më¥ në oë
 pëê qëÄ rë së‡ tëp uë vëê wë” xëƒ yë zë" …
 …æ …Ä …s sê¸ tê£ uê‡ vêı wêÇ xêå yêÉ zê£ {ê£ |ê‡ }ê¸ ~ê£ ê‡ Äê√ Åêü Çêˇ ÉêÏ Ñêû Öê˛ Üê áê
 àêÍ  …G !…8 "…Ä #…â âêÏ äê” ãêî åê  çê@ éê èê êêê ëê” íê‘ ìêÏ îê ïê ñêG óê= $…E %…  &…Ä '…ò òê ôêw öê÷ õê úêE ùêÕ (…  )…’ *…Ä +…û ûêê üêÇ †ê °ê‡ ¢ê¥ £ê §ê •êê ¶êÉ ßêø ®ê‡ ©ê` ™ê ´êê ¨êÅ ≠ê¸ Æê‡ Øêˇ ∞ê√ ±ê ≤ê ≥êê ¥êÅ µê¸ ∂ê‡ ∑êˇ ∏ê πê  ∫ê⁄ ,…p -…º .…Ä /…ª ªêê ºêÇ Ωê æê‡ øê¥ ¿ê ¡ê ¬êê √ê¡ ƒê ≈êt ∆ê «ê »êê …ê”  ê† Àê Ãêp Õêø 0…r 1…! 2…Å 3…; ;ë} <ë =ë >ë{ ?ë @ër Aë% 4…( 5…Æ 6…Ä 7…“ “ê ”êê ‘ê“ ’ê
 ÷ê ◊ê( ÿê¥ 8…( 9…± :…Ä ;…Ÿ Ÿêê ⁄êÉ €ê∫ ‹ê‡ ›êˇ ﬁêê ﬂê“ ‡ê ·ê‡ ‚ê‰ „êÔ ‰ê Âê£ Êê‡ Áêt Ëêˇ Èê Íêê Îê“ Ïê
 Ìê Óê( Ôê¥ <…) =…y >…Ä ?… ê Òêê Úê“ Ûê Ùê ıê) ˆê @…) A…| B…Ä C…˜ ˜êê ¯êÉ ˘ê∫ ˙ê‡ ˚êˇ ¸êê ˝ê“ ˛ê ˇê‡  ë‰ ëÔ ë ë£ ë‡ ët ëˇ ë ëê 	ë“ 
ë ë ë) ë D…* E…B F…Å G… ë ëê ë“ ë ë ë* ëH H…* I…E J…Å K… ëê ëÉ ë∫ ë‡ ëˇ ëê ë“ ë ë‡ ë‰ ëÔ  ë !ë£ "ë‡ #ët $ëˇ %ë &ëê 'ë“ (ë )ë *ë* +ëH  …                       GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                            
       &         ¨       /   0   Ü     >   ¿        V   ‡        d      l     t   p                                 	              
 å   ê  q    ô   @         §   †      Æ   |       ∏   @  å     «   ‡  ú     ÿ   0       Ë       *    
 ˙     œ       ‡  ê                             *      Ä     8      
     K           b     1     x  @   !     ì      0                   ù     0     ≠  †   P     æ  `   @     Õ             ÿ             ˆ                                       9             H             Y             r             à             è             ú             ±             ƒ             Ã             È             ˚                          !             5           A             P      
     _      0      vs6624.c vs6624_enum_mbus_code vs6624_formats vs6624_get_fmt vs6624_g_frame_interval vs6624_remove vs6624_s_stream vs6624_s_frame_interval vs6624_probe vs6624_ops vs6624_p1 vs6624_p2 vs6624_default vs6624_run_setup vs6624_ctrl_ops vs6624_probe.cold vs6624_s_ctrl vs6624_set_fmt vs6624_driver_init vs6624_driver vs6624_driver_exit __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 vs6624_id vs6624_core_ops vs6624_video_ops vs6624_pad_ops __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free __stack_chk_guard i2c_transfer_buffer_flags __const_udelay __stack_chk_fail __x86_indirect_thunk_edx devm_gpio_request_one msleep devm_kmalloc v4l2_i2c_subdev_init usleep_range_state _printk v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vs6624_id_device_table       $        1   $  ¡   $  ·   $  Ï   %  Ù   &    $    '  :  (  E  )  P  '  h  *  q  $  Ä  '  ‹  (    (  0  (  Z  (  h  '  â  *  ë  $  †  '  ≤  +  Õ    ﬁ  ,  Û  -    .              /  V  (  Ñ  (  ©  (  Ω  0  Œ     ˛  (       F  (  n     y  (  ∞  (  ¸  (  &  (  Q  (  |  (  Ê    Î  1  ˛  2       %  3  /     L  3  V     p  3  z     î  3  ´  4  º  '  ”  &  ·  &  ˝  *    $    '  l  (  z  '  €  *  ·  $    '  ü  '  +	  (  N	  (  †	  (  ≈	  (  Í	  (  
  (  /
  (  R
  (  ¯
  (  ;  (    (  l  *  È                   h                                                     $             1  &        $          5     6          8  @      L      \      d     p     t     »     –     ‘     8     .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rodata.str1.4 .rel.text.unlikely .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   p                    	   @       ‘  ò              )             ¿  Ä                   %   	   @       l                   /             @                     8             @  (                  4   	   @       å   P               E      2       h                   T      2       Ñ  T                 g             ÿ  *                  c   	   @       ‹         
         z                                 v   	   @       Ù                   â               
                  Ö   	   @       !                 î                a                  °             †  ƒ                  ù   	   @       $!  P               ©      0       d                   ≤              w                     ¬             x  (                                †  ∞     $         	              P  Å                               t!  ’                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              6/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/gfp.h \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  arch/x86/include/asm/irq.h \
    $(wildcard include/config/HAVE_KVM) \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/generated/asm/irq_regs.h \
  include/asm-generic/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/SPARSE_IRQ) \
    $(wildcard include/config/IRQ_DOMAIN) \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/kobject_ns.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  arch/x86/include/asm/hw_irq.h \
  include/linux/profile.h \
    $(wildcard include/config/PROFILING) \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/BPF_JIT) \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/rhashtable-types.h \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  kernel/irq/internals.h \
    $(wildcard include/config/GENERIC_IRQ_CHIP) \
  include/linux/kernel_stat.h \
  include/linux/pm_runtime.h \
    $(wildcard include/config/PM) \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ)/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/fwnode.h \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
  include/linux/netdev_features.h \
  include/linux/sched/clock.h \
    $(wildcard include/config/HAVE_UNSTABLE_SCHED_CLOCK) \
  include/net/flow_dissector.h \
  include/uapi/linux/if_ether.h \
  include/linux/splice.h \
  include/linux/pipe_fs_i.h \
  include/uapi/linux/if_packet.h \
  include/net/flow.h \
  include/net/page_pool.h \
    $(wildcard include/config/PAGE_POOL_STATS) \
  include/linux/ptr_ring.h \
  include/linux/netfilter/nf_conntrack_common.h \
  include/uapi/linux/netfilter/nf_conntrack_common.h \
  include/net/scm.h \
    $(wildcard include/config/SECURITY_NETWORK) \
  include/linux/security.h \
    $(wildcard include/config/SECURITY_INFINIBAND) \
    $(wildcard include/config/SECURITY_NETWORK_XFRM) \
    $(wildcard include/config/SECURITY_PATH) \
    $(wildcard include/config/SECURITYFS) \
  include/linux/kernel_read_file.h \
  include/linux/nsproxy.h \
  include/uapi/linux/netlink.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/ns_common.h \
  include/net/net_namespace.h \
    $(wildcard include/config/IPV6) \
    $(wildcard include/config/IEEE802154_6LOWPAN) \
    $(wildcard include/config/IP_SCTP) \
    $(wildcard include/config/NETFILTER) \
    $(wildcard include/config/WEXT_CORE) \
    $(wildcard include/config/MPLS) \
    $(wildcard include/config/CAN) \
    $(wildcard include/config/MCTP) \
    $(wildcard include/config/CRYPTO_USER) \
    $(wildcard include/config/SMC) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/NET_NS_REFCNT_TRACKER) \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/XFRM_STATISTICS) \
    $(wildcard include/config/TLS) \
  include/net/snmp.h \
  include/uapi/linux/snmp.h \
  include/linux/u64_stats_sync.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/IP_MULTIPLE_TABLES) \
    $(wildcard include/config/NET_L3_MASTER_DEV) \
    $(wildcard include/config/IP_MROUTE) \
    $(wildcard include/config/IP_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_MULTIPATH) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/IPV6_MULTIPLE_TABLES) \
    $(wildcard include/config/IPV6_SUBTREES) \
    $(wildcard include/config/IPV6_MROUTE) \
    $(wildcard include/config/IPV6_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/NF_DEFRAG_IPV6) \
  include/net/dst_ops.h \
  include/uapi/linux/icmpv6.h \
  include/net/netns/nexthop.h \
  include/net/netns/ieee802154_6lowpan.h \
  include/net/netns/sctp.h \
  include/net/netns/netfilter.h \
    $(wildcard include/config/NETFILTER_FAMILY_ARP) \
    $(wildcard include/config/NETFILTER_FAMILY_BRIDGE) \
    $(wildcard include/config/DECNET) \
    $(wildcard include/config/NF_DEFRAG_IPV4) \
  include/linux/netfilter_defs.h \
  include/uapi/linux/netfilter.h \
  include/linux/in.h \
  include/uapi/linux/in.h \
  include/net/netns/conntrack.h \
    $(wildcard include/config/NF_CT_PROTO_DCCP) \
    $(wildcard include/confcmd_drivers/media/i2c/vs6624.o := gcc -Wp,-MMD,drivers/media/i2c/.vs6624.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"vs6624"' -DKBUILD_MODNAME='"vs6624"' -D__KBUILD_MODNAME=kmod_vs6624 -c -o drivers/media/i2c/vs6624.o drivers/media/i2c/vs6624.c 

source_drivers/media/i2c/vs6624.o := drivers/media/i2c/vs6624.c

deps_drivers/media/i2c/vs6624.o := \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
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
  include/linux/delay.h \
  include/linux/math.h \
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
  include/linux/compiler_types.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/vdso/bits.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
    $(wildcard include/config/SMP) \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
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
  include/linux/sched.h \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/IOMMU_SVA) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/NO_HZ_FULL) \
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
    $(wildcard include/config/LOCKDEP) \
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
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/UPROBES) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/MMU) \
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
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/kstrtox.h \
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
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
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
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/segment.h \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
    $(wildcard include/config/BUG) \
  include/linux/bug.h \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  include/linux/restart_block.h \
  include/linux/time64.h \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/vdso/math64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/TRACEPOINTS) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  include/linux/rcutree.h \
  include/linux/wait.h \
  include/linux/spinlock.h \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/uapi/linux/wait.h \
  include/linux/refcount.h \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/rhashtable-types.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
  include/linux/osq_lock.h \
  include/linux/debug_locks.h \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/tsc.h \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/nodemask.h \
    $(wildcard include/config/HIGHMEM) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/mm_types_task.h \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/gpio.h \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ARCH_HAVE_CUSTOM_GPIO_H) \
  include/asm-generic/gpio.h \
    $(wildcard include/config/ARCH_NR_GPIO) \
  include/linux/gpio/driver.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GPIO_GENERIC) \
    $(wildcard include/config/GPIOLIB_IRQCHIP) \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/PINCTRL) \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
  include/linux/energy_model.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/gfp.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags-layout.h \
  include/generated/bounds.h \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/uprobes.h \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/overflow.h \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/irq.h \
    $(wildcard include/config/GENERIC_IRQ_EFFECTIVE_AFF_MASK) \
    $(wildcard include/config/GENERIC_IRQ_IPI) \
    $(wildcard include/config/DEPRECATED_IRQ_CPU_ONOFFLINE) \
    $(wildcard include/config/GENERIC_IRQ_MIGRATION) \
    $(wildcard include/config/GENERIC_PENDING_IRQ) \
    $(wildcard include/config/HARDIRQS_SW_RESEND) \
    $(wildcard include/config/GENERIC_IRQ_LEGACY) \
    $(wildcard include/config/GENERIC_IRQ_MULTI_HANDLER) \
  include/linux/irqhandler.h \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
    $(wildcard include/config/PCI) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/fwnode.h \
  include/linux/vmalloc.h \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  arch/x86/include/asm/irq.h \
  arch/x86/include/generated/asm/irq_regs.h \
  include/asm-generic/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/SPARSE_IRQ) \
    $(wildcard include/config/IRQ_DOMAIN) \
  arch/x86/include/asm/hw_irq.h \
  include/linux/profile.h \
    $(wildcard include/config/PROFILING) \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/linux/irqchip/chained_irq.h \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/pinctrl/pinctrl.h \
    $(wildcard include/config/GENERIC_PINCONF) \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ctype.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/CGROUP_WRITEBACK) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
    $(wildcard include/config/MIGRATION) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/list_lru.h \
  include/linux/shrinker.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/pinctrl/pinctrl-state.h \
  include/linux/pinctrl/devinfo.h \
  include/linux/pinctrl/consumer.h \
  include/linux/pinctrl/pinconf-generic.h \
  include/linux/pinctrl/machine.h \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/GPIO_SYSFS) \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/resource_ext.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  arch/x86/include/asm/acenv.h \
  include/acpi/acnames.h \
  include/acpi/actypes.h \
  include/acpi/acexcep.h \
  include/acpi/actbl.h \
  include/acpi/actbl1.h \
  include/acpi/actbl2.h \
  include/acpi/actbl3.h \
  include/acpi/acrestyp.h \
  include/acpi/platform/acenvex.h \
  include/acpi/platform/aclinuxex.h \
  include/acpi/platform/acgccex.h \
  include/acpi/acoutput.h \
  include/acpi/acpiosxf.h \
  include/acpi/acpixf.h \
  include/acpi/acconfig.h \
  include/acpi/acbuffer.h \
  include/linux/dynamic_debug.h \
  include/acpi/acpi_bus.h \
    $(wildcard include/config/X86_ANDROID_TABLETS) \
    $(wildcard include/config/ACPI_SYSTEM_POWER_STATES_SUPPORT) \
    $(wildcard include/config/ACPI_SLEEP) \
  include/acpi/acpi_drivers.h \
    $(wildcard include/config/ACPI_DOCK) \
  include/acpi/acpi_numa.h \
    $(wildcard include/config/ACPI_HMAT) \
  include/acpi/acpi_io.h \
  arch/x86/include/asm/acpi.h \
    $(wildcard include/config/ACPI_APEI) \
  include/acpi/pdc_intel.h \
  arch/x86/include/asm/numa.h \
    $(wildcard include/config/NUMA_EMU) \
  arch/x86/include/asm/numa_32.h \
  include/linux/regulator/consumer.h \
    $(wildcard include/config/REGULATOR) \
  include/linux/suspend.h \
    $(wildcard include/config/VT) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/HIBERNATION_SNAPSHOT_DEV) \
    $(wildcard include/config/PM_SLEEP_DEBUG) \
    $(wildcard include/config/PM_AUTOSLEEP) \
  include/linux/swap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/page_counter.h \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/sizes.h \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bvec.h \
  include/linux/highmem.h \
  include/linux/cacheflush.h \
  arch/x86/include/asm/cacheflush.h \
  include/asm-generic/cacheflush.h \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  include/linux/pagemap.h \
  include/linux/hugetlb_inline.h \
  include/uapi/linux/mempolicy.h \
  include/linux/freezer.h \
  include/uapi/linux/i2c.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
  include/media/media-device.h \
  include/media/media-devnode.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/hevc-ctrls.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/v4l2-subdev.h \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-image-sizes.h \
  drivers/media/i2c/vs6624_regs.h \

drivers/media/i2c/vs6624.o: $(deps_drivers/media/i2c/vs6624.o)

$(deps_drivers/media/i2c/vs6624.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ig/NF_CT_PROTO_SCTP) \
    $(wildcard include/config/NF_FLOW_TABLE) \
    $(wildcard include/config/NF_CT_PROTO_GRE) \
    $(wildcard include/config/NF_CONNTRACK_EVENTS) \
    $(wildcard include/config/NF_CONNTRACK_LABELS) \
  include/linux/list_nulls.h \
  include/linux/netfilter/nf_conntrack_tcp.h \
  include/uapi/linux/netfilter/nf_conntrack_tcp.h \
  include/linux/netfilter/nf_conntrack_dccp.h \
  include/uapi/linux/netfilter/nf_conntrack_tuple_common.h \
  include/linux/netfilter/nf_conntrack_sctp.h \
  include/uapi/linux/netfilter/nf_conntrack_sctp.h \
  include/net/netns/nftables.h \
  include/net/netns/xfrm.h \
  include/uapi/linux/xfrm.h \
  include/net/netns/mpls.h \
  include/net/netns/can.h \
  include/net/netns/xdp.h \
  include/net/netns/smc.h \
  include/net/netns/bpf.h \
  include/net/netns/mctp.h \
  include/net/net_trackers.h \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
  include/linux/ref_tracker.h \
    $(wildcard include/config/REF_TRACKER) \
  include/linux/seq_file_net.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ctype.h \
  include/net/sock.h \
    $(wildcard include/config/SOCK_RX_QUEUE_MAPPING) \
    $(wildcard include/config/SOCK_VALIDATE_XMIT) \
    $(wildcard include/config/RPS) \
  include/linux/netdevice.h \
    $(wildcard include/config/DCB) \
    $(wildcard include/config/HYPERV_NET) \
    $(wildcard include/config/WLAN) \
    $(wildcard include/config/AX25) \
    $(wildcard include/config/MAC80211_MESH) \
    $(wildcard include/config/NET_IPIP) \
    $(wildcard include/config/NET_IPGRE) \
    $(wildcard include/config/IPV6_SIT) \
    $(wildcard include/config/IPV6_TUNNEL) \
    $(wildcard include/config/NETPOLL) \
    $(wildcard include/config/BQL) \
    $(wildcard include/config/RFS_ACCEL) \
    $(wildcard include/config/FCOE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/NET_POLL_CONTROLLER) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/VLAN_8021Q) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/netprio_cgroup.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/CGROUP_DATA) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/kernel_stat.h \
  include/linux/cgroup-defs.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/uapi/linux/netdevice.h \
  include/uapi/linux/if.h \
  include/uapi/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
  include/uapi/linux/if_bonding.h \
  include/uapi/linux/pkt_cls.h \
  include/uapi/linux/pkt_sched.h \
  include/linux/hashtable.h \
  include/linux/page_counter.h \
  s);

/**
 * struct pending_free - information about data structures about to be freed
 * @zapped: Head of a list with struct lock_class elements.
 * @lock_chains_being_freed: Bitmap that indicates which lock_chains[] elements
 *	are about to be freed.
 */
struct pending_free {
	struct list_head zapped;
	DECLARE_BITMAP(lock_chains_being_freed, MAX_LOCKDEP_CHAINS);
};

/**
 * struct delayed_free - data structures used for delayed freeing
 *
 * A data structure for delayed freeing of data structures that may be
 * accessed by RCU readers at the time these were freed.
 *
 * @rcu_head:  Used to schedule an RCU callback for freeing data structures.
 * @index:     Index of @pf to which freed data structures are added.
 * @scheduled: Whether or not an RCU callback has been scheduled.
 * @pf:        Array with information about data structures about to be freed.
 */
static struct delayed_free {
	struct rcu_head		rcu_head;
	int			index;
	int			scheduled;
	struct pending_free	pf[2];
} delayed_free;

/*
 * The lockdep classes are in a hash-table as well, for fast lookup:
 */
#define CLASSHASH_BITS		(MAX_LOCKDEP_KEYS_BITS - 1)
#define CLASSHASH_SIZE		(1UL << CLASSHASH_BITS)
#define __classhashfn(key)	hash_long((unsigned long)key, CLASSHASH_BITS)
#define classhashentry(key)	(classhash_table + __classhashfn((key)))

static struct hlist_head classhash_table[CLASSHASH_SIZE];

/*
 * We put the lock dependency chains into a hash-table as well, to cache
 * their existence:
 */
#define CHAINHASH_BITS		(MAX_LOCKDEP_CHAINS_BITS-1)
#define CHAINHASH_SIZE		(1UL << CHAINHASH_BITS)
#define __chainhashfn(chain)	hash_long(chain, CHAINHASH_BITS)
#define chainhashentry(chain)	(chainhash_table + __chainhashfn((chain)))

static struct hlist_head chainhash_table[CHAINHASH_SIZE];

/*
 * the id of held_lock
 */
static inline u16 hlock_id(struct held_lock *hlock)
{
	BUILD_BUG_ON(MAX_LOCKDEP_KEYS_BITS + 2 > 16);

	return (hlock->class_idx | (hlock->read << MAX_LOCKDEP_KEYS_BITS));
}

static inline unsigned int chain_hlock_class_idx(u16 hlock_id)
{
	return hlock_id & (MAX_LOCKDEP_KEYS - 1);
}

/*
 * The hash key of the lock dependency chains is a hash itself too:
 * it's a hash of all locks taken up to that lock, including that lock.
 * It's a 64-bit hash, because it's important for the keys to be
 * unique.
 */
static inline u64 iterate_chain_key(u64 key, u32 idx)
{
	u32 k0 = key, k1 = key >> 32;

	__jhash_mix(idx, k0, k1); /* Macro that modifies arguments! */

	return k0 | (u64)k1 << 32;
}

void lockdep_init_task(struct task_struct *task)
{
	task->lockdep_depth = 0; /* no locks held yet */
	task->curr_chain_key = INITIAL_CHAIN_KEY;
	task->lockdep_recursion = 0;
}

static __always_inline void lockdep_recursion_inc(void)
{
	__this_cpu_inc(lockdep_recursion);
}

static __always_inline void lockdep_recursion_finish(void)
{
	if (WARN_ON_ONCE(__this_cpu_dec_return(lockdep_recursion)))
		__this_cpu_write(lockdep_recursion, 0);
}

void lockdep_set_selftest_task(struct task_struct *task)
{
	lockdep_selftest_task_struct = task;
}

/*
 * Debugging switches:
 */

#define VERBOSE			0
#define VERY_VERBOSE		0

#if VERBOSE
# define HARDIRQ_VERBOSE	1
# define SOFTIRQ_VERBOSE	1
#else
# define HARDIRQ_VERBOSE	0
# define SOFTIRQ_VERBOSE	0
#endif

#if VERBOSE || HARDIRQ_VERBOSE || SOFTIRQ_VERBOSE
/*
 * Quick filtering for interesting events:
 */
static int class_filter(struct lock_class *class)
{
#if 0
	/* Example */
	if (class->name_version == 1 &&
			!strcmp(class->name, "lockname"))
		return 1;
	if (class->name_version == 1 &&
			!strcmp(class->name, "&struct->lockfield"))
		return 1;
#endif
	/* Filter everything else. 1 would be to allow everything else */
	return 0;
}
#endif

static int verbose(struct lock_class *class)
{
#if VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static void print_lockdep_off(const char *bug_msg)
{
	printk(KERN_DEBUG "%s\n", bug_msg);
	printk(KERN_DEBUG "turning off the locking correctness validator.\n");
#ifdef CONFIG_LOCK_STAT
	printk(KERN_DEBUG "Please attach the output of /proc/lock_stat to the bug report\n");
#endif
}

unsigned long nr_stack_trace_entries;

#ifdef CONFIG_PROVE_LOCKING
/**
 * struct lock_trace - single stack backtrace
 * @hash_entry:	Entry in a stack_trace_hash[] list.
 * @hash:	jhash() of @entries.
 * @nr_entries:	Number of entries in @entries.
 * @entries:	Actual stack backtrace.
 */
struct lock_trace {
	struct hlist_node	hash_entry;
	u32			hash;
	u32			nr_entries;
	unsigned long		entries[] __aligned(sizeof(unsigned long));
};
#define LOCK_TRACE_SIZE_IN_LONGS				\
	(sizeof(struct lock_trace) / sizeof(unsigned long))
/*
 * Stack-trace: sequence of lock_trace structures. Protected by the graph_lock.
 */
static unsigned long stack_trace[MAX_STACK_TRACE_ENTRIES];
static struct hlist_head stack_trace_hash[STACK_TRACE_HASH_SIZE];

static bool traces_identical(struct lock_trace *t1, struct lock_trace *t2)
{
	return t1->hash == t2->hash && t1->nr_entries == t2->nr_entries &&
		memcmp(t1->entries, t2->entries,
		       t1->nr_entries * sizeof(t1->entries[0])) == 0;
}

static struct lock_trace *save_trace(void)
{
	struct lock_trace *trace, *t2;
	struct hlist_head *hash_head;
	u32 hash;
	int max_entries;

	BUILD_BUG_ON_NOT_POWER_OF_2(STACK_TRACE_HASH_SIZE);
	BUILD_BUG_ON(LOCK_TRACE_SIZE_IN_LONGS >= MAX_STACK_TRACE_ENTRIES);

	trace = (struct lock_trace *)(stack_trace + nr_stack_trace_entries);
	max_entries = MAX_STACK_TRACE_ENTRIES - nr_stack_trace_entries -
		LOCK_TRACE_SIZE_IN_LONGS;

	if (max_entries <= 0) {
		if (!debug_locks_off_graph_unlock())
			return NULL;

		print_lockdep_off("BUG: MAX_STACK_TRACE_ENTRIES too low!");
		dump_stack();

		return NULL;
	}
	trace->nr_entries = stack_trace_save(trace->entries, max_entries, 3);

	hash = jhash(trace->entries, trace->nr_entries *
		     sizeof(trace->entries[0]), 0);
	trace->hash = hash;
	hash_head = stack_trace_hash + (hash & (STACK_TRACE_HASH_SIZE - 1));
	hlist_for_each_entry(t2, hash_head, hash_entry) {
		if (traces_identical(trace, t2))
			return t2;
	}
	nr_stack_trace_entries += LOCK_TRACE_SIZE_IN_LONGS + trace->nr_entries;
	hlist_add_head(&trace->hash_entry, hash_head);

	return trace;
}

/* Return the number of stack traces in the stack_trace[] array. */
u64 lockdep_stack_trace_count(void)
{
	struct lock_trace *trace;
	u64 c = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(stack_trace_hash); i++) {
		hlist_for_each_entry(trace, &stack_trace_hash[i], hash_entry) {
			c++;
		}
	}

	return c;
}

/* Return the number of stack hash chains that have at least one stack trace. */
u64 lockdep_stack_hash_count(void)
{
	u64 c = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(stack_trace_hash); i++)
		if (!hlist_empty(&stack_trace_hash[i]))
			c++;

	return c;
}
#endif

unsigned int nr_hardirq_chains;
unsigned int nr_softirq_chains;
unsigned int nr_process_chains;
unsigned int max_lockdep_depth;

#ifdef CONFIG_DEBUG_LOCKDEP
/*
 * Various lockdep statistics:
 */
DEFINE_PER_CPU(struct lockdep_stats, lockdep_stats);
#endif

#ifdef CONFIG_PROVE_LOCKING
/*
 * Locking printouts:
 */

#define __USAGE(__STATE)						\
	[LOCK_USED_IN_##__STATE] = "IN-"__stringify(__STATE)"-W",	\
	[LOCK_ENABLED_##__STATE] = __stringify(__STATE)"-ON-W",		\
	[LOCK_USED_IN_##__STATE##_READ] = "IN-"__stringify(__STATE)"-R",\
	[LOCK_ENABLED_##__STATE##_READ] = __stringify(__STATE)"-ON-R",

static const char *usage_str[] =
{
#define LOCKDEP_STATE(__STATE) __USAGE(__STATE)
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	[LOCK_USED] = "INITIAL USE",
	[LOCK_USED_READ] = "INITIAL READ USE",
	/* abused as string storage for verify_lock_unused() */
	[LOCK_USAGE_STATES] = "IN-NMI",
};
#endif

const char *__get_key_name(const struct lockdep_subclass_key *key, char *str)
{
	return kallsyms_lookup((unsigned long)key, NULL, NULL, NULL, str);
}

static inline unsigned long lock_flag(enum lock_usage_bit bit)
{
	return 1UL << bit;
}

static char get_usage_char(struct lock_class *class, enum lock_usage_bit bit)
{
	/*
	 * The usage character defaults to '.' (i.e., irqs disabled and not in
	 * irq context), which is the safest usage category.
	 */
	char c = '.';

	/*
	 * The order of the following usage checks matters, which will
	 * result in the outcome character as follows:
	 *
	 * - '+': irq is enabled and not in irq context
	 * - '-': in irq context and irq is disabled
	 * - '?': in irq context and irq is enabled
	 */
	if (class->usage_mask & lock_flag(bit + LOCK_USAGE_DIR_MASK)) {
		c = '+';
		if (class->usage_mask & lock_flag(bit))
			c = '?';
	} else if (class->usage_mask & lock_flag(bit))
		c = '-';

	return c;
}

void get_usage_chars(struct lock_class *class, char usage[LOCK_USAGE_CHARS])
{
	int i = 0;

#define LOCKDEP_STATE(__STATE) 						\
	usage[i++] = get_usage_char(class, LOCK_USED_IN_##__STATE);	\
	usage[i++] = get_usage_char(class, LOCK_USED_IN_##__STATE##_READ);
#include "lockdep_states.h"
#undef LOCKDEP_STATE

	usage[i] = '\0';
}

static void __print_lock_name(struct lock_class *class)
{
	char str[KSYM_NAME_LEN];
	const char *name;

	name = class->name;
	if (!name) {
		name = __get_key_name(class->key, str);
		printk(KERN_CONT "%s", name);
	} else {
		printk(KERN_CONT "%s", name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		if (class->subclass)
			printk(KERN_CONT "/%d", class->subclass);
	}
}

static void print_lock_name(struct lock_class *class)
{
	char usage[LOCK_USAGE_CHARS];

	get_usage_chars(class, usage);

	printk(KERN_CONT " (");
	__print_lock_name(class);
	printk(KERN_CONT "){%s}-{%d:%d}", usage,
			class->wait_type_outer ?: class->wait_type_inner,
			class->wait_type_inner);
}

static void print_lockdep_cache(struct lockdep_map *lock)
{
	const char *name;
	char str[KSYM_NAME_LEN];

	name = lock->name;
	if (!name)
		name = __get_key_name(lock->key->subkeys, str);

	printk(KERN_CONT "%s", name);
}

static void print_lock(struct held_lock *hlock)
{
	/*
	 * We can be called locklessly through debug_show_all_locks() so be
	 * extra careful, the hlock might have been released and cleared.
	 *
	 * If this indeed happens, lets pretend it does not hurt to continue
	 * to print the lock unless the hlock class_idx does not point to a
	 * registered class. The rationale here is: since we don't attempt
	 * to distinguish whether we are in this situation, if it just
	 * happened we can't count on class_idx to tell either.
	 */
	struct lock_class *lock = hlock_class(hlock);

	if (!lock) {
		printk(KERN_CONT "<RELEASED>\n");
		return;
	}

	printk(KERN_CONT "%px", hlock->instance);
	print_lock_name(lock);
	printk(KERN_CONT ", at: %pS\n", (void *)hlock->acquire_ip);
}

static void lockdep_print_held_locks(struct task_struct *p)
{
	int i, depth = READ_ONCE(p->lockdep_depth);

	if (!depth)
		printk("no locks held by %s/%d.\n", p->comm, task_pid_nr(p));
	else
		printk("%d lock%s held by %s/%d:\n", depth,
		       depth > 1 ? "s" : "", p->comm, task_pid_nr(p));
	/*
	 * It's not reliable to print a task's held locks if it's not sleeping
	 * and it's not the current task.
	 */
	if (p != current && task_is_running(p))
		return;
	for (i = 0; i < depth; i++) {
		printk(" #%d: ", i);
		print_lock(p->held_locks + i);
	}
}

static void print_kernel_ident(void)
{
	printk("%s %.*s %s\n", init_utsname()->release,
		(int)strcspn(init_utsname()->version, " "),
		init_utsname()->version,
		print_tainted());
}

static int very_verbose(struct lock_class *class)
{
#if VERY_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

/*
 * Is this the address of a static object:
 */
#ifdef __KERNEL__
/*
 * Check if an address is part of freed initmem. After initmem is freed,
 * memory can be allocated from it, and such allocations would then have
 * addresses within the range [_stext, _end].
 */
#ifndef arch_is_kernel_initmem_freed
static int arch_is_kernel_initmem_freed(unsigned long addr)
{
	if (system_state < SYSTEM_FREEING_INITMEM)
		return 0;

	return init_section_contains((void *)addr, 1);
}
#endif

static int static_obj(const void *obj)
{
	unsigned long start = (unsigned long) &_stext,
		      end   = (unsigned long) &_end,
		      addr  = (unsigned long) obj;

	if (arch_is_kernel_initmem_freed(addr))
		return 0;

	/*
	 * static variable?
	 */
	if ((addr >= start) && (addr < end))
		return 1;

	/*
	 * in-kernel percpu var?
	 */
	if (is_kernel_percpu_address(addr))
		return 1;

	/*
	 * module static or percpu var?
	 */
	return is_module_address(addr) || is_module_percpu_address(addr);
}
#endif

/*
 * To make lock name printouts unique, we calculate a unique
 * class->name_version generation counter. The caller must hold the graph
 * lock.
 */
static int count_matching_names(struct lock_class *new_class)
{
	struct lock_class *class;
	int count = 0;

	if (!new_class->name)
		return 0;

	list_for_each_entry(class, &all_lock_classes, lock_entry) {
		if (new_class->key - new_class->subclass == class->key)
			return class->name_version;
		if (class->name && !strcmp(class->name, new_class->name))
			count = max(count, class->name_version);
	}

	return count + 1;
}

/* used from NMI context -- must be lockless */
static noinstr struct lock_class *
look_up_lock_class(const struct lockdep_map *lock, unsigned int subclass)
{
	struct lockdep_subclass_key *key;
	struct hlist_head *hash_head;
	struct lock_class *class;

	if (unlikely(subclass >= MAX_LOCKDEP_SUBCLASSES)) {
		instrumentation_begin();
		debug_locks_off();
		printk(KERN_ERR
			"BUG: looking up invalid subclass: %u\n", subclass);
		printk(KERN_ERR
			"turning off the locking correctness validator.\n");
		dump_stack();
		instrumentation_end();
		return NULL;
	}

	/*
	 * If it is not initialised then it has never been locked,
	 * so it won't be present in the hash table.
	 */
	if (unlikely(!lock->key))
		return NULL;

	/*
	 * NOTE: the class-key must be unique. For dynamic locks, a static
	 * lock_class_key variable is passed in through the mutex_init()
	 * (or spin_lock_init()) call - which acts as the key. For static
	 * locks we use the lock object itself as the key.
	 */
	BUILD_BUG_ON(sizeof(struct lock_class_key) >
			sizeof(struct lockdep_map));

	key = lock->key->subkeys + subclass;

	hash_head = classhashentry(key);

	/*
	 * We do an RCU walk of the hash, see lockdep_free_key_range().
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return NULL;

	hlist_for_each_entry_rcu_notrace(class, hash_head, hash_entry) {
		if (class->key == key) {
			/*
			 * Huh! same key, different name? Did someone trample
			 * on some memory? We're most confused.
			 */
			WARN_ON_ONCE(class->name != lock->name &&
				     lock->key != &__lockdep_no_validate__);
			return class;
		}
	}

	return NULL;
}

/*
 * Static locks do not have their class-keys yet - for them the key is
 * the lock object itself. If the lock is in the per cpu area, the
 * canonical address of the lock (per cpu offset removed) is used.
 */
static bool assign_lock_key(struct lockdep_map *lock)
{
	unsigned long can_addr, addr = (unsigned long)lock;

#ifdef __KERNEL__
	/*
	 * lockdep_free_key_range() assumes that struct lock_class_key
	 * objects do not overlap. Since we use the address of lock
	 * objects as class key for static objects, check whether the
	 * size of lock_class_key objects does not exceed the size of
	 * the smallest lock object.
	 */
	BUILD_BUG_ON(sizeof(struct lock_class_key) > sizeof(raw_spinlock_t));
#endif

	if (__is_kernel_percpu_address(addr, &can_addr))
		lock->key = (void *)can_addr;
	else if (__is_module_percpu_address(addr, &can_addr))
		lock->key = (void *)can_addr;
	else if (static_obj(lock))
		lock->key = (void *)lock;
	else {
		/* Debug-check: all keys must be persistent! */
		debug_locks_off();
		pr_err("INFO: trying to register non-static key.\n");
		pr_err("The code is fine but needs lockdep annotation, or maybe\n");
		pr_err("you didn't initialize this object before use?\n");
		pr_err("turning off the locking correctness validator.\n");
		dump_stack();
		return false;
	}

	return true;
}

#ifdef CONFIG_DEBUG_LOCKDEP

/* Check whether element @e occurs in list @h */
static bool in_list(struct list_head *e, struct list_head *h)
{
	struct list_head *f;

	list_for_each(f, h) {
		if (e == f)
			return true;
	}

	return false;
}

/*
 * Check whether entry @e occurs in any of the locks_after or locks_before
 * lists.
 */
static bool in_any_class_list(struct list_head *e)
{
	struct lock_class *class;
	int i;

	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (in_list(e, &class->locks_after) ||
		    in_list(e, &class->locks_before))
			return true;
	}
	return false;
}

static bool class_lock_list_valid(struct lock_class *c, struct list_head *h)
{
	struct lock_list *e;

	list_for_each_entry(e, h, entry) {
		if (e->links_to != c) {
			printk(KERN_INFO "class %s: mismatch for lock entry %ld; class %s <> %s",
			       c->name ? : "(?)",
			       (unsigned long)(e - list_entries),
			       e->links_to && e->links_to->name ?
			       e->links_to->name : "(?)",
			       e->class && e->class->name ? e->class->name :
			       "(?)");
			return false;
		}
	}
	return true;
}

#ifdef CONFIG_PROVE_LOCKING
static u16 chain_hlocks[MAX_LOCKDEP_CHAIN_HLOCKS];
#endif

static bool check_lock_chain_key(struct lock_chain *chain)
{
#ifdef CONFIG_PROVE_LOCKING
	u64 chain_key = INITIAL_CHAIN_KEY;
	int i;

	for (i = chain->base; i < chain->base + chain->depth; i++)
		chain_key = iterate_chain_key(chain_key, chain_hlocks[i]);
	/*
	 * The 'unsigned long long' casts avoid that a compiler warning
	 * is reported when building tools/lib/lockdep.
	 */
	if (chain->chain_key != chain_key) {
		printk(KERN_INFO "chain %lld: key %#llx <> %#llx\n",
		       (unsigned long long)(chain - lock_chains),
		       (unsigned long long)chain->chain_key,
		       (unsigned long long)chain_key);
		return false;
	}
#endif
	return true;
}

static bool in_any_zapped_class_list(struct lock_class *class)
{
	struct pending_free *pf;
	int i;

	for (i = 0, pf = delayed_free.pf; i < ARRAY_SIZE(delayed_free.pf); i++, pf++) {
		if (in_list(&class->lock_entry, &pf->zapped))
			return true;
	}

	return false;
}

static bool __check_data_structures(void)
{
	struct lock_class *class;
	struct lock_chain *chain;
	struct hlist_head *head;
	struct lock_list *e;
	int i;

	/* Check whether all classes occur in a lock list. */
	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (!in_list(&class->lock_entry, &all_lock_classes) &&
		    !in_list(&class->lock_entry, &free_lock_classes) &&
		    !in_any_zapped_class_list(class)) {
			printk(KERN_INFO "class %px/%s is not in any class list\n",
			       class, class->name ? : "(?)");
			return false;
		}
	}

	/* Check whether all classes have valid lock lists. */
	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		class = &lock_classes[i];
		if (!class_lock_list_valid(class, &class->locks_before))
			return false;
		if (!class_lock_list_valid(class, &class->locks_after))
			return false;
	}

	/* Check the chain_key of all lock chains. */
	for (i = 0; i < ARRAY_SIZE(chainhash_table); i++) {
		head = chainhash_table + i;
		hlist_for_each_entry_rcu(chain, head, entry) {
			if (!check_lock_chain_key(chain))
				return false;
		}
	}

	/*
	 * Check whether all list entries that are in use occur in a class
	 * lock list.
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		e = list_entries + i;
		if (!in_any_class_list(&e->entry)) {
			printk(KERN_INFO "list entry %d is not in any class list; class %s <> %s\n",
			       (unsigned int)(e - list_entries),
			       e->class->name ? : "(?)",
			       e->links_to->name ? : "(?)");
			return false;
		}
	}

	/*
	 * Check whether all list entries that are not in use do not occur in
	 * a class lock list.
	 */
	for_each_clear_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		e = list_entries + i;
		if (in_any_class_list(&e->entry)) {
			printk(KERN_INFO "list entry %d occurs in a class list; class %s <> %s\n",
			       (unsigned int)(e - list_entries),
			       e->class && e->class->name ? e->class->name :
			       "(?)",
			       e->links_to && e->links_to->name ?
			       e->links_to->name : "(?)");
			return false;
		}
	}

	return true;
}

int check_consistency = 0;
module_param(check_consistency, int, 0644);

static void check_data_structures(void)
{
	static bool once = false;

	if (check_consistency && !once) {
		if (!__check_data_structures()) {
			once = true;
			WARN_ON(once);
		}
	}
}

#else /* CONFIG_DEBUG_LOCKDEP */

static inline void check_data_structures(void) { }

#endif /* CONFIG_DEBUG_LOCKDEP */

static void init_chain_block_buckets(void);

/*
 * Initialize the lock_classes[] array elements, the free_lock_classes list
 * and also the delayed_free structure.
 */
static void init_data_structures_once(void)
{
	static bool __read_mostly ds_initialized, rcu_head_initialized;
	int i;

	if (likely(rcu_head_initialized))
		return;

	if (system_state >= SYSTEM_SCHEDULING) {
		init_rcu_head(&delayed_free.rcu_head);
		rcu_head_initialized = true;
	}

	if (ds_initialized)
		return;

	ds_initialized = true;

	INIT_LIST_HEAD(&delayed_free.pf[0].zapped);
	INIT_LIST_HEAD(&delayed_free.pf[1].zapped);

	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
		list_add_tail(&lock_classes[i].lock_entry, &free_lock_classes);
		INIT_LIST_HEAD(&lock_classes[i].locks_after);
		INIT_LIST_HEAD(&lock_classes[i].locks_before);
	}
	init_chain_block_buckets();
}

static inline struct hlist_head *keyhashentry(const struct lock_class_key *key)
{
	unsigned long hash = hash_long((uintptr_t)key, KEYHASH_BITS);

	return lock_keys_hash + hash;
}

/* Register a dynamically allocated key. */
void lockdep_register_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head;
	struct lock_class_key *k;
	unsigned long flags;

	if (WARN_ON_ONCE(static_obj(key)))
		return;
	hash_head = keyhashentry(key);

	raw_local_irq_save(flags);
	if (!graph_lock())
		goto restore_irqs;
	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (WARN_ON_ONCE(k == key))
			goto out_unlock;
	}
	hlist_add_head_rcu(&key->hash_entry, hash_head);
out_unlock:
	graph_unlock();
restore_irqs:
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lockdep_register_key);

/* Check whether a key has been registered as a dynamic key. */
static bool is_dynamic_key(const struct lock_class_key *key)
{
	struct hlist_head *hash_head;
	struct lock_class_key *k;
	bool found = false;

	if (WARN_ON_ONCE(static_obj(key)))
		return false;

	/*
	 * If lock debugging is disabled lock_keys_hash[] may contain
	 * pointers to memory that has already been freed. Avoid triggering
	 * a use-after-free in that case by returning early.
	 */
	if (!debug_locks)
		return true;

	hash_head = keyhashentry(key);

	rcu_read_lock();
	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (k == key) {
			found = true;
			break;
		}
	}
	rcu_read_unlock();

	return found;
}

/*
 * Register a lock's class in the hash-table, if the class is not present
 * yet. Otherwise we look it up. We cache the result in the lock object
 * itself, so actual lookup of the hash should be once per lock object.
 */
static struct lock_class *
register_lock_class(struct lockdep_map *lock, unsigned int subclass, int force)
{
	struct lockdep_subclass_key *key;
	struct hlist_head *hash_head;
	struct lock_class *class;
	int idx;

	DEBUG_LOCKS_WARN_ON(!irqs_disabled());

	class = look_up_lock_class(lock, subclass);
	if (likely(class))
		goto out_set_class_cache;

	if (!lock->key) {
		if (!assign_lock_key(lock))
			return NULL;
	} else if (!static_obj(lock->key) && !is_dynamic_key(lock->key)) {
		return NULL;
	}

	key = lock->key->subkeys + subclass;
	hash_head = classhashentry(key);

	if (!graph_lock()) {
		return NULL;
	}
	/*
	 * We have to do the hash-walk again, to avoid races
	 * with another CPU:
	 */
	hlist_for_each_entry_rcu(class, hash_head, hash_entry) {
		if (class->key == key)
			goto out_unlock_set;
	}

	init_data_structures_once();

	/* Allocate a new lock class and add it to the hash. */
	class = list_first_entry_or_null(&free_lock_classes, typeof(*class),
					 lock_entry);
	if (!class) {
		if (!debug_locks_off_graph_unlock()) {
			return NULL;
		}

		print_lockdep_off("BUG: MAX_LOCKDEP_KEYS too low!");
		dump_stack();
		return NULL;
	}
	nr_lock_classes++;
	__set_bit(class - lock_classes, lock_classes_in_use);
	debug_atomic_inc(nr_unused_locks);
	class->key = key;
	class->name = lock->name;
	class->subclass = subclass;
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	class->name_version = count_matching_names(class);
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
 * that <prev>˙ÀqÏÈü>∏a ¥ëaÕ BÈ\…mÿ•‘cÈäŸãÑU…ı\r∫8à∞ß›`Ç|ﬂâ¸‚	Yùπ3@zB=;`9Ïî,ì˝`_πäÃ»©ÅÇ7‹ƒ…ﬁÀk[Ù©„„ãCÍtZ®*Ù´∞ÓíπEj7(`!SP?‰_e ƒËŒÄ¯˜6Ëó±/çP-Q¯õÚ√eÿÇ4∑[Xx˙Ï˛u;é,+ÎÖ#„ÈE¡Q9TY}Kv≠åîÀ4ıñçœá∂Ê÷ﬁ√^˜p¥ÙQ:u?ç’ﬁ©Ì~—(6¨Åv“ï!=k\ÂÓ‰¿ø^í˝ö≥Ùws/ò3;¯zußÎL≈K81:ˇïàjh∆•Ÿ√dgÉ‰<YÚµÜ˘ö‚˚»»\@0˙ﬁÿé…HxWÒÆúü"ºo-p[ç˚bÜ.<§Ì K$!¢C‡ïWp‘∞áÎìuµ{YÄÑa§Í’O ∏è˚√«YπXÑ+ü.⁄€*¶‘ÎËèÜµıªcclèâZªƒ°]uQw< Í ◊Å«é» Fk±¸ùÌfŸÊû…¯i ∏ƒ@2g—Äeæ†|I–x∏(0,bz
¿u)Fw˛`ñ|ú;∞Nlã%Æã4 —3”£ÛÓ˙ﬂ{ÚqjLT∑zÿm38ê••í6s:ÕÈÙ®M∂qEÆ .eÎ¶/W˘n∏t£/À<y‚ı…ã‚I‚Ñ‰P–°§…2[Bòó÷Ú≈È{˜v—òR\X…ín: ”MÍ6¯Åõªûi3¸ˇ;æ6±'cZõ‰Ù|SníˇD˘ù´
tÜ–å–Vá<ÏUUœÚne0Ôßæ6ãéÓ÷∫íy¬ãÈö66@Ô©Js`üS≈í¨˜·®Ñj˙UMf“è5‹Iœ1bxOnà!_Ûj‚ «h8ﬂ∏M¥/^çŒœ¬fy%anÄï>i_ˆú<éµ‘Ç√"ÑrO1( ›0f´Ü^“(/¬j;Q6Ôtê3òPƒÎ&¿©„ä>ÑŸ0{Œ5e&Â≤ ÉÊaˇÌxˇ´Çuü*™|≠Ø|“•oK®µ
ª˛7'¨›9F»9%àkÿ:%»a,˝8ÑÀˆÏy‘ÛywÓŸJ÷V¡ôN˛„P?—¿?Êt∫t)9∫Ì¸Wgd7ÙÎﬂ® í‚*”ßÒ∑µçIÌ˝GıQ«nL~+Ñ]òS_õ'“è∑û+Ú\πY8Óe˜M;’¨€µ…;©%ÆCÚú’PmêïùÏ∫œìzŸåxyg,ËŸ‡˛¿Läã◊ôµÂwë~üõníR@ﬁ¨√(F†Ò@TN¯‘PñUà—bSÃZOHãÙ}d
>„+¿wVmÿΩC*íÆu§ ™òf/'©©ß±±–À0ƒ˙Íj=!`QÃ r˝Yêyﬂ|?”ËŸïõüõé è:7ìt"¥ïkùÉ%+“Ø2Jó~˘å|sMN„Å€Õ÷4QS¨ø9√¯—îÆn –yˇÓyÁ7‹’Áµ
≠dk,W˘Ëpçªx'∑l7£ç,ÜÔ≈ÿïÚw(óy¬†Ò|(Î_ÇS)‡†Z¢BÖ˛ VwL’7GÕÎ¢giÁÀ9:pU¡KpXr(‡ÄRX˙m© í'‚L#N1≈L„8ëZi@UoK\¶kJÙ¶ÉÚéÄ^ä|æNΩ(7wˇ∫∑c; M`ˇtΩ,2@–MÖ7Yué¶æqÒÀ'( C.ﬁ-Yaà  ∂Ëˆ]ê≈KaÑ˜;æ9$N=ç≥ö
™øÕ‡–3Pª©ÛWºœå7:ZË¡dõÍ›≤pHRËV^>"∫fﬁ·Zöø
•—∑“0qˇ8ÔÿÜtiüI;âfqÅ≤ÆÆØ)ô>'Ì‰Éı‘PTúÃ^¿=j77œô2·Öød˙T+}YËã¬WÜÁbﬂä7P¨rÁ#Ó†d9˜FoMµàÔjLÅπ’Ô}∫(ÔJp–˛ìÅ™‰A¶ó¸"Ù¨„gqYO∫¸ä⁄KS3¢yÏ"(W‡U˙æ©ŸZ8]TΩL ¸$TpèΩ|;¿¬È√b&Œ=s‹
Ù'x—®O˙S°NÆmX†ç¿bÒ8$ó`”'¸KÆ&è˝≤{£Üˆêq()»Ä\3óÁ√o8©=ﬂC5›Q!„aH∏‘ﬂû¥EÆTΩs@tvÓà2¥ÓõÓ{ 7ê€zTmÇ8kæÈ5∆íû™+ˇVÖ≈Q1¬zÎ0aÇly”Å“ÔÂK©2ÉpÏ·»)Çt¡Py⁄≈ªNù–VZÅñ Xb€mÊˆòàÁm@Î† 0›•??ﬁv 3¨ùp˚!≥Õåòˇ¢·ÃæÛ@Ä"˜y§ë√Ê=‚Cj(Œ—¸’x°$Â÷·Tg!˜ªZás>cØsø?fËÔ?µ∞”h”—É¯Æ¥pÑZc{¸ÙyÓ•s†ä€à√Ëπwk‰ûú5Jéx·ıµ_a≠LR—¬«©È€ºåÄ'›dÛ’Ë3∫—Cë$tñUt}∏ƒ6˛≠©T€¡ƒ¨QŒd,Ì¶È¨›ÜÀ√F&“Ù◊çP§é¬·5møπ⁄‡w8ﬁÍS{Ó ıç◊`j∞vàÔ»	Pﬁ<„¢ƒÄR\—™Ω∂xî◊ì]™_ôÄç—g%DaÙ‘s“≈52	¨˜˘ül#¯í˘…‰’sRΩE”BÓÂTYì5É0-SØÕp ™Ó\À3z∑t“DπHYâÄ,@Ω87<Gºœ¸îAç
ÿä®îß-jµ_¥uR¡∂ûas∆zÈ˘Cù
´—r:.^…MW&»$π+i·u„HWå=uîç{Üc? ﬁ¢L≤/≈t‹»_≥ÙÕ7 AñW,<Ô€ÇõÆ~^S(>d¥≥;ÁÉuîÁyKﬂPm‘˙K÷[T:Uaª‹i©`≠û›√‰<«yπ¶s˜WÿÜ•ÇéC&ÖXŒÀÎ√<Ía~ñç´1Å££«sn˛o	æ4íS¢`ßwn—àÄøÆ|`ÛÕib7Ö´Û/Ót Ëj‹{Ïﬁ6ü4“3ÈÔ:ˇ›¶L∂©«B|V›}ìŒOog4@Æ–mXŒzÓ&≈€◊§ï¥Nµä≤cÔÓIDÑ.ç‰†±¥RáΩ‘F˚G'Îxõ›˛bkäç<Èü{[â prìNÖ"›>ˆ4WL—%≤π“AE3OÕ
èc‘Œïƒ<âOÿ|˘£˛üTÀæ.Å`2Ü∑lÍØÖ√¢#@ì„˛'Vù⁄µnâ˜ºöY-¶X€’V-‚3‚˜ëEj4˘ÒÑÁç≈@nD‡·°Ë~≠=’≥Fx13ˇÅΩ í√?ù¯}M-
óÛù˛ ©ÎLUÒ√É˜B2bpOcA∫Êª/FH5Âò˝}¸Œ†wH/∂Hà¶¡ã/”‘o§‰[g]∏Ä˙`J3Ä˜±ÌR€kK®∞@+®ﬁÆÙ–7∂Øã;R∞°rÜﬁåîŒLkÕ‘¯ì „}≥·∑¸∆∞~ª‹˚ÆÆàäõVÃ∫Ø‰óºY‚Ω9’˜‘vŸ•æ·Öﬂ%\ÁØƒ°˜zæÕΩΩ˜ÎIÈ†~ à:£>*l‚®ﬂ˜É`E43¡û±_∂c^Ëâ
îrëΩ.Í∆S8ß	Ê]wÛZ4i›Í”îÇkFD†wûtg?ˇ+Í˝´+æÈØo7b·¨˝\øk;°†O-Ed[éB›gÑêü≤3∏â∑ßŒÇ˛∫O∆›Y¯∆©LÕ}
j‹PuÊü|òü»T
^±LƒΩ!°…7F3l A÷tN |›§≈:ÕÏ{Ëí(∞RQd–Ñ@€°(¬ÈOﬁ˝ô‹£C◊l:Ó3É4ÔTthè+kpˆí∆˙9ÀÈ¶æ3™oQr
‘EØ:ûÓ=¬	7óEO~ ˛˘º˙·Ãán[$>|„≥ƒâèí8¶©Gù˚·UÇYÀSw<ß÷‚ƒî€œäaË”¿∞≠ånlÅP†ë<]~ÇZõ.äÓÉÜK˚Èu÷RŸ9å©Yg<ÎÜ◊ú¨eÁÜÏâÎ‚˙ïB©Aª¶±˛¨ûDn°6Ê]êz&=-ÛlÛ›ı7q'‚ÊZ©í°˙∂˝ü‡W~îçZyb4%’wøDµá≥’/Y∂⁄'-ÑT?*∑∞ºÓÚ~›à".®gr$üãë≤ûøõ&é◊}%JM™Ø)µ.*”ûiıÓ;‰/:)bWô0⁄˛’∫÷hqNfÍUb‚øé‰‚Yd˛∏I—0xÂΩµ´ˇO&ï›√%l˘˜yBœ≤&' ∞v¢å_}-Ÿ6π≈`n°r°4Åñø€vz¯¿™]˛llÚ@ÕRf—†¿F/>≈rRg¡˚d;yoÈ
RÁ(}˙Œ,4o—Éa(∂Ç‡/õûºúa∫ﬁ $éx.˜jå’~≈ıxUÁçJ	∏˚õº‘ËPs˘Lª#‚Ñ	 ƒ∏Z◊ãá•≤‚ √.D~≤Û*˜∏zª‹≤ÜÀ‹™sÖßØV∑= Hœ4.—êß˙„(m∑Œ%ÔƒÙz&ı{·bÀ€;4÷9,6O≤wœ˛◊ú„B‹™}íoGà~øµq¯µP‡E
cÑ‡¿∏øÛéÉ·Oé”RÛ¸„ÆyÓ04⁄<Ω	ôÔ¶˝êF’æõ≥ÂåèQB- ≠Ê–¢={ í†à“bè¯«¡r+Ëá‘DG’F4 Ö\d∆ÊÌ√’yä*˙jæ>Dªﬂùf∏Ö`ágØ>=≥}!*∫L!ôwnÜ!”!% <>ËÛ&ª†ŒÑxü´Øî\•(©`>◊Ä?Û•	€’`¸jÖá)g≈}Ä˚ö^µÎŸ:ßÕ⁄'œ¢ld5ìR–)u∂Ü)©N›k“%ÈPºº∏∫
_ø“Ø§≠¢ ¡¢¸a°fﬁ17·YKf˜%´ˇDm¯MøÈ{ì¢ÇÕÓÈ[º–`ïü(m⁄_(°∞é%§)»ñåvB:›yÇ„LAsW™Ô6TQ¡ó∏RË‹bjŸèI ∑„"'›Ò&üê,≤XR”%ôâ∆˙v¯†ÏŒid{jîòî“§ÇÔP»hIùÀÕYß◊≥tßöY|7Ey
=û—¡*ÿr~—¸5l‘&!îD87zuë¿tq|Ü5	ñTòÉÍ≈Ç∂bú3|o5â.◊Àà±*ﬁ7?Kõ…≤ì#6¡BEﬁØΩ“1*«ŸÑ+qÌ7éjˇªé∞ΩAJäk};ûf;*v~êl¥%åÓ«Âf‡ˆEGﬁÅ"j˛›A– œcHUO˝œìä)˙vmXπ–‚øâOŒ™0i®´åó’47ê{®zf< ≥„W–2q∂æÏ +1t$iÌ]Lf~˜¯d˘£å9ÕËtw∆>Ùv|	ÒU˝ÂÎŒ∏/)æs√Èã6ú
˘æåmå≈W#	<˘mÔÓ„∂ü”ç`|vVª @•‚π–à]Ø#Ø÷‚»£øÏ˚ò <ïóm,[m‡ßè√l]¥M~‘÷>@ªâ¬SÖÄiö7¿ Ã‚ñŒDºã1‹UÁ2¯%BÑ,— :∏˚µBX}I?´ƒx8nê]Å3>.ÿ_„ê∏ˆ	∫i’S*fïï›#ﬁŸ≥åè’œΩgè0$4î‡?†íùÇ¿ÅAﬁC¬ay÷ÔY˙%%˘@Äh•o·ˇ˛≤Èˇë©»’|w∞r[>˙k_òDTÎ—∆ç»0ÔïÈ'RÓÆàƒ¬T7ƒörw∆…¨9âàzàıñ˝+“∂ÿ6˝‘ïl2*`‘4‚9Ízîi8•¶ˇÙR≠ñ˚;LK˜π«åΩP`1ÎÁq•Ω•˚§–ƒxÕ¬ºI¥(Ò·ÖÁOå¡IÌé~~¶oú∑Âø(!ëÓÀ¨üâ“Ø•∏‘Ë+íÂe∏nK$ ﬁ+Ï⁄´r@»_1##˙9Çl]®åÆ√…I-0O∏yw’√áÊ¢÷¨ÜŒııﬂ∏õf˘˜YFV:Í…ØÔô@õ€"mÕª.Å~Ü∫(Õô9éô/ÀçnlOïÂY® ¢b÷—Yó‘? M©Øâ0≤U‹‰0o)¥¶)Í&”¯˜Ïñß>bˆıYCˆx™GÄÂ_õ~$p≈èç¶%V8LÆØ∑‘hbpHíZ)-ÆÍmç[tdÌT1˜ÿéã˜‚A‰‰]”lÄÖ‚[Ω‘ö˚ÜO+˚
VÊAX˜°r8oı›¶5˛n+∞5´(ºiFB!Ω;{˘/äÏ‡$ƒyà5<Ï-P~Äãó°d‹_„ß´ iO{Vxz»^7î¸’gÂIW=)ò`cÍ=¨q f+•.(‹∂•SU.WI÷„zµ˚	Ô©∆ÚŒ˘Óæ‚HZ˙UbÎ!S≈“≠‚=‰r&¢tƒ
nÇÒ…#]åê»’)ÕÿY{3!ùêÓ≥ÎŒ§T©¶ìP√ÂÂb˘l~¡5§»Ô^»Ã;n ‚º2¢ TŸ]ïıò‹P±‰@4d©iYùÓ¡/EhÆD∞o\5à0HæU¸/€zZ››oy˜æÂ=øƒã~KA,XµÁ/J,Ìÿ`õ∂‰Nƒ
B¯wnQÕÊÿ∫tÅ‘w[ñQÉ+Eêvm≈&pH› ûÙÒ°£&JhJÔv~X]~˚„5v⁄õ)qº_Ä¢Rö¯öÄèköıávàW#Ÿ÷d>Áº7åÉÔDBZ“îP·√$œ+Úâ÷™ 6±˘#9±÷÷W7åjø?M˜∂éç∫u/Øª´≥rïÇfª≥◊+¯i˚≠Ö¢`HË∑˘·AÉ&ÀÇâx˝·Û´m•´¿Ad	Áº¬¶Ω”≥2Arê!*UwiìVìHkÆ∂[—Ÿπ3p+ïÁbŸ‘ã≈–ÌmXÁ[’RÎï^UÑPC•W ˘t√ºœ<ö7{œ˙ú
Ï6)ãﬁ9â,P<u◊Z8)Nóƒ∑« yÜ√˝ƒ=ØE6ë`µÏQáb” @ J Èı•¶ÑLUü5√…_2»WJ^·>¢á-<Ñô∞pã?shÿ"”ü÷˜àøDã◊rŸçè†fÉ«±“‹ZhpñNZH∞ï]ÜÍÙ,”Õm„ÀzlËØÊè4Ï$´ "tt∆‘≈¿].?g°¸IÖ0í∆®’?j0“@(,ã.H¥Œiı[ÇÔñö¿ÄÊˇ0tæ)~Á#Q-9ZëS∫®÷¶ßWxK~◊Á^VnÂ-µS.h%¯æ≤&≥§dthı≈≈•z®)¥·Õ1˜`zóß˝g"Sã∞U˜vÙI"oÌáSˇñR7É!ùqT/Ú∏«Xü˛«CN—-∫≠∆P¸„W≈Ç´ó«ìÚßîv˚ÓW<ø*'¨k\_‡B6¯/	lã¬Ju∂ Î¯@aü/€i}'h1¿F›HÓ+l„≥ï¿√å^Ê" „{°˜CYë‡:(‹Kˆ‡,Sè’&¶ÎBy•åjõ≠=ªG˜±K“…WêMò˙ßXDÅˇPg8∂î˚Àÿâ†K+1?Ô∂Ÿ>ìº∂oU¥RÔfHfÔ∂o)mnﬁKBºÎœ±5é-Ø^∞ÆE1ØK 0d±_ÔR0érqÀMÕFã‹îûÌvë·Ø|ﬂqÈjö™3APJòïR	ê“éêÎ·ö'Kñµπ(ßS’ˇïydÚƒ«6áÙæàFß/ Ú5ZÒf&A˘ëGí&µÄ∂*oêë“	HM‚$A…∞&dD·Úó{¬drcQ0˘`ÈIÇÆl‰’r\6≠∂2œÍ5è«j¢xQŒ∏Q‰ÉŸF`GÁ∞ÈäEÍ9^√!ªWPï€Ç-≤À“0¿Úd÷£◊Ä∞ˇÍäú:ø^Zﬂ8õ:’áÆI[∑±¡∑Fñfj0/ôÕ”`◊˙‹∞å&7'Ä§}-ÙƒaOË—Ï.ƒ€…¯Tõ≥¥•\Z∑´!X—W°∆Ç´Ÿ3À¸c∏≈Ô∂#DTÆ Ó≠”zß(yñÇ∂˙’∂-ìLHÀGçëNbF„aMˇŒSi=ívÀÚ.JÀMêø√S÷úa£·4F¥>›ÔÀ˛Ñ=qÖΩª3wΩ„∞Ç´‰iå	àQπÈÖçRÖiåC4£x‚*ˆF„∏ûÏ(W¢ZY
|˘Òßˇ%‹⁄§∏"⁄=¥ØmÜ){;«€™áü¬3†ßãÈ)YÛ≤kwoD%fvá∂É≤zﬂy*“-»&3èºÂ-gk√%mGÍ4è‡ìó⁄ò¢4UJó¬\PA+Õ„∆nøËOXs◊ê¥n!Áz}”g¨˚.˝Ωœd,r4w	Ü¶(â;Æ›ówªé¸ŒÓ†e>ÄR}ç˚€>œS≤ïª≥/-BN åﬂnˇ£àö®[πHV!;ıW””ªF!Â1h§¯∑öVT¯{ïLÚ’%›/Î∑≤úrΩ»…8{Mﬂ‹ÄâE∑/ob√gØ©MuÀ¨<ü$ƒà_•ƒ÷ÔŒÁ˚ˆW¢ÄËß9ûyÃó)≥ÒÜ∑òrPüuœP>ë!ç&Y≈@†±|ÖO˙Ïx=Z…}R~û—rÊ”qÍëdKA¢êz7Ãèœˆ,'ßK	Á+’æô67ºRÎ˙8?s&q(’.ä¢ gø'êø≈#Äﬁe+®!pızB∏Ô'p\å˝Î©îµÎì¶J~ìæ~1∞±¢CUaFåmP˘Yäî“2T}aôö8ûJÆ}sàÚÖJ∑[‘¯¨ı∑‘ÅjôK˛ »∫tÿπH{±◊¥¸Ey0˚ø  5(±nË~	/Iar˚Ø_◊@z¢»îπ	i Â/˝çpc!_∏£àÊãº…nQpK¯o¨hïëNë7%÷ŸóËì:Íπ◊ı+CtN°˘ÒM8ˆ≠Îí9~ÛHP{WÛå¡ô÷_ƒtÀà1!Rß\8ªOÛ˝ƒñ~ùvò˘f;e;ìˇ}{∫c˜†Ô≥'Ápì`≠ßà¸‘µ|\{xKªr≤‡Ky´…ÊN?	3 ŸMüÛWº!◊]C•ìY k∫»4Â?°—Äîd“LÃ+<NﬁuöÃ+Ö;a»\gÍí_©µ)M“ø‡ªé€’ãô‹”¡∆ÿAóaÃ%ú«!–’7h#˙ËÔÆ*≠ÀÂ∏RŒ9˘bï≥ﬁá¯£b˚}Âr≥YíÛXGhÎ”–Ü9Qù˜
ïdM”1÷ø[Ωû!(±™—ˆ°‡¸ò∆vnÈ\V\n·Óó∫YBÓ*M8œYê?∆˜ÄC›ﬂ1óñÍG1ÅÒû-ÿ'd–R_ïÔ√ã†£´≈}ﬁ“\äZ|ˇ÷ÎæI≠"Œ¯`¢Ã)˜ ÿÍVˇ‹h08l1™11≥¢π+£¯U6ÜN¨∏˚É˚PŸûÔK.L|⁄H`Ω∑'ÕÃ¯sk/|>uz©™í+}*ƒÀCYÑπg¡«ƒ∆ç≥•≤ˆ5}p5vø)«LπÒ2$’ß®Å'	¡Œ_xMŒ∫,£Ò∏◊∑.Œ§¬snøÓL-J˛mÖE¥Ÿ›æmüC”0åÚ\¸“öîZ%’JA^{
ûdl¿c{(ÏvtÕ¨liÂeiºà∞ﬁo1¯–c¨Ê Ç¡c…≥ÖÓ€Ñò.d]„-ﬁ/Ö√:X“,÷·n,j£ù¥ÿR„Ä3„D)é¿çäC.ÃRÛxÀ–—wòÁ4r2<¬,–ôTπÍ∆ÎO√πK\aí$M„æÇ‚FÜ⁄ÀøpÈ≈∞~´¿xë¥mª≥á™\ªã˛ÌèLgo‘tΩ™2ÄåF1\YÄãg·Wøî| ‘oÍ„ïª«.◊äeŒGtw%*äƒ^Êt.Õﬁ¬ˇˆ_Ëîå‡Ã9g˜®n.è,›ŸÀìs´nπK*ä#	r-•O¥çû@`†ïøô2å„x   }ñZÖwˇ$÷˘ë¿∫ä,=QÁîQ‘ÇÇ3–Ír‹‘¸´¬JN*N°ëÇ7 "Çi	scr≤˘ÿ¥7qˇ-ÑÅ«áöq)·R:<waÈf f¸u5/ö⁄˜êŒ'\7˜ÄÔÎ^''Ùpe›¢x ä§ˆúÓ⁄'ˆ∂$ugiÍõõw0≈⁄˛=eÙvÜ}Ä
Üäçâ[öÜkπ0¡≤8N´dﬂ◊D^Ÿà√Bjâ|=œÄ„´röÈ^!ó≈2ùÚ=jz™G‰êA*+D1+Ù“˜@
ûŸi9x£ø+`Y.1R(‰∏Õk·îoú∫1Œ=V3
& .ΩÖˆŸ>t¯í∞®»+93ÿ6Sﬂ=}ÏÔ <ıjZ!è1ºtóÌwº9«x›ª9ã¸îúè “hΩóFà“Û fﬂÈà$ﬁ:qgÍΩéÔD2‡&W,<îFÜ‡FµìHÑôñN†ìvPö]∞dÇåê—»˝gºR0:uˆ˚MW˘öÚã0\*(Ì‚
…Ïƒ.Éù–÷ùip-izn$√Pöç[M‹À˙5ÇãÎ0(OÌ{∏ ÌÈæEkr’†®+Uƒ3ms'1îúÏ°ê0T"t∑±#—∞W~¿)›7mzÜK	v‹?p^:-N˚`öWpuÕÖ<\Ôy$$Ó1“˛ÙM◊iYçÒÏW/@´±_"GP_—ó¯6ë∞›”ƒClcHô<∞a¥zVŒ„âJ_ræA(ïé·ì`‚fAVx‘ö]&1ê◊ï∏∞tn)ÖDÈÃÎm°ÅkQaôE°Ú›·Å@XJ,6ˆfj£ ≈∞≤$‚–—∏:ÅÓ{˜íªä$˙~ÍÙk.⁄:°ırßëY∑˘ R.¥U @G‡ÍÆ	Öm™Z∫Å¢Jp°:Lv_ﬂ>Ùxx •]3ë–hﬂ¢=∂Bk‡®oØ<πŸ>nå†”¢Md5ÏH›?É∑–($ì	ﬂ|ï Ü°!£Ëw◊å8Eo.¬≈ıı≤qm –P
«ÙÒ?daùèf√˘BBaü6qÆ≈R=Äôr$]™±˜ùÍ´r˛3K„*µXÎ…ß…][›
±Àääÿ‹èæ-Ôg$pﬂ-ßW9ˆ˝Ï˘µ	c≥≤˝â^Ér ë1soﬁ™æÉΩ÷L3˙gk∞08ıœu…8qqdµx*q"ÚÀ√ÀkÓ(Ö–Ò∆\ø™ÚB0]8º?πH˚d¿Å∆ﬂÛè†»<≈¥ÑAà¶ÌIfπñ”~Vhévnè˙Z1¬EµlX‚˙ªâ`…Tç‡Ñ¿ΩdM>}/3Q?åÕ÷0Ø"È§åÅÏâÆ®Z6y:Õúî∂–∏…/Mu◊∑π<“så£Kómö˚˜´‹èô<¡81§ÖÍPllI{\üê	mríQ†ú"¨E”:¬|æ·’T++l—ZFàC≥G—Œ"ç≤ÈÙ¡∫£ZÆ Å0ñ^ı£Á‹F„c`˙«_FÇC6hMPôvÌ‹‘	Mó|'ŒÑ.ƒ¥∂<Vπôwœ¢4ØHV:,Ã{-∂
8e$Bã•»Ω•ÉaEk ®€xw/°P¿ì‹ªø}d]◊oÉfÆçlWN◊¸Ñç‰i)EûJN¬ÇÇPà†<cÏsﬂf’;übÛ˙ˇåx)nY ΩÂÒ{6¬¯á7»a¯amü§|ö5˛(—NŒ4{d2!{≠UPoSµY˙∫y?∂ùÄ¡`¨aœ«â‡åﬂÜáuY–u÷p™‹±?ñã⁄¸äˇÊ$‘SóΩ™Æu˙≤–¢íß‡SSIhúœ|ÕÙêÈOPá≥'pÿ9“&
ªÕ∂âﬁÑ∑3¬'K)OãArùyÉÖº˜V™&/Dëª.gor≠2gqÓn∏_ª‹û9œ˚mj—ÿäT¢∫vz3Èz9ë≠}¿xÜ≤∏Èn◊æ-ÃòÄ¶ô‡ÀÃÿ˝K∑:pX‘â	qLs¶ˆf^˛ê≤_≠D€´Ω´.ø›SïWfJÔ®ù“ÁìÉâMC√ìø	ˇÇ±©*k$~lJv˝íÊZ∆ïpÛ°—˝gÖ*^Ä∂§[¶îò±7Ø≈>;WM&ÀéﬂT#«ïÒ©J}NLtº? åœ\ı=U›√Ó?◊5…ÕBçœ@ìCJ=è7ΩwÊé ¢\p¥m∏]'–úáËªc˚t c)˚ÎK9Ãr„"Ùy›ìK3ïeëº1k‰îÀyßíª©Äê#*È˛õ≥µ°áÓ¬R·‹`0uÔ##É	ä©:}ÆÓw„ïKH?∂≈Å)•ràe¨Ö¡Ñ∞2¸˝FëÜvEﬁÕ≤»î1]†x≥ah‚·˘E≥Ù∑oe_S6ÄF·kˆ wŸeïD˝‡º¸3ENEÑ4ãHWÅÉBÜÁ.üuÈtÖ+(ÂT:Ss¥©(ù⁄ê† )¸™óÀ*aäÎ\´Ä£ı+¿ˆèÑò Q¸¯(–öÒïÛû†°ΩÄ©wèY’UÌ¸~ãÍ¥*’`-Êºb”E}˜ˇ∫˛ÖGôcZl97	ìr°¬3∆ènàaÕn¶üêxØ∏yµê∏6¡Ê9J
˚-∂ÄÑ^˛MπeíÆ˚õˆ5ü|Æ9PP,V≠µJúœ‹æòz∑ˇIbÉÆ54Îê}©EA∫”¸"èˆÜ∫_ÕHû„48j˚(2¿uT_ÅâR7ΩiÊZoöK}w	∫π6ûÄ®—√m˙7A¡tCò]Øç˜iô=¶“X“Vƒ™aîƒ{èGFjAŸT0‘/Z√”Ûıw%«%◊ìã≤Í§˙˜'¿ªÅüAJ3ÉHÖ:=∞ pL´r§9JJ–çö„0ıOI–p3ŒUí®¯	ªñŸ„côô|4ﬂ GSPπƒıQCƒ∏Ω˛øC{¸4ßFÎcà/8†T&ƒõ•∫°1ﬁTScÑ¨·”ò\# d.Ó>ñ#7Û%ùÃáŸò˜EràZ∫æ…YœYJ∑N-À∂Å„¶òçc4¸¶b{ß∂ „–Cãﬁ7f8u?È«:!ÓÂQ‹{Øö
±Çy∏‡ﬂ¸˜q´Å∏q—rÜ°>ç€W˘ıµU≠âß—G.í&±"¶0I∂Ò,æ`—'Ω≤)k
ïk´Ìÿ
ÌDÎü∆-í~ıc∫Ôõêo+êùtRvŸæuÍ∂£∞1$‚ „ÍèÆ®∫&ÅmÏ÷5gr¸ß§j≈WY®/fqﬂ'\∏££DﬁWZE˚“=C±5™∂Gyrä…e«È¢SMºV÷˜˝Úıå$‹~ø–ÍÕQ–y]ó/é)Ó…œÆÃ∫‘—π2Ú5˜Wç~˘<ì´3Wwa6œÊûô™f˙;^≠M4Í„£õÊ∫f`$B\K3Ãßefª{çwáÒÜqYN˙ÁpÑ∞ZÑ@v•ç+)úH9xŒ œ˚µ:¥ê¡çK•LHÙˇ˝g/çˇËüñ4Kƒô®íDÒ√°?ﬂnÆËcÕ·ú‘ÿ‰‡!ÖÙáw[,¥¯*n"≠Ø‚(˛±Æ’§ıéM‡Ûç_∂=I†»ã–J EÁCØÌà[3{9knl’fbÈ(Éë*äis:Ä"Ö]}ﬁUù?äX’L≈[GÓÇ•ı9p‹\û¥lè}…ù¸‡ê‡Ω}Ωì∆4ºé∑ÒHËºY'–£/ÁóçÚ˘!7.yégb ∂ïÎ‰ÁbJdÀê‚·Â∂s¥œ‹±`Âº† ÿ+€(‹≠«UŸıØ<öÎxâ‘'8ﬁõ`ÕíØ¢√ît°¬ätÛHâ{mUp»?£¸∫GåwÍS∂“⁄©ˆR?ql9#n≈guY	≤Æ÷ØH]îUÂÒ¡']tw(â.u/6•ß≤7
ßëÊå>ryp|WÿºÚÌ·&êÃ4∆0]ÜÙq∏ŒóY_íùe4i¬Ω\iä´h»&‡Ü"~»ÅÂMêÇe»ì>%NågÄÁ±ºıôÑ∆sè’†Z"∞+«âZ¿@Írñ	5VÛ´7cBK¢ ôdÊ¡CQ\&Ê˚I§D!˙À~¿\Ö&.@ÊØ`Hb¶)¿=ë8E
éŸ≥Ä•¥˙l,øÆ(Jr>P’CxÊè∆3ñäÄjwÅû0◊(ôcUˆ¨êT÷©\ò6∫€Yr 9h!1òµ¬~’CLˇ⁄˛i2öã z…;,Ío ˝Ç©√≠±—t∂'π∏j<ôë!mQ{]	xù*(â°›ÇíåÏ¶ﬂ
íåg íˆ¨FØ[ÎOÃ»dØ®F&º}ß!!ìnéxáµ≠z%ª]Å9›±áåM‰Ó>~ûB”fÂÒÎg¯Õœµ◊º«4â›∏GhïhR°È „∑Ü˚•'êã©.Mìq"oÜˆ‰·∂∏=¬âãºH3ôÙ†géÕ£#Éæ}.fﬂNŒÏNgkC”Èrµâè◊ãW◊—ÆH„◊'±åç¥±eˇ’§$ˇÇÓÆ]CJ›◊bìúˆ·kÕÀ
¨åtz„d'€wB¯P„´'Ú”œ”¿π˘é —?p»˛AÖ®IﬁÉò∑Kÿ≥hè/ULfF˝h-·±êm§Üi	oE’ÏsvìÅHã¶Î‹	Ü…Ÿ#›Áá1Hó~`±ΩEà{Rjaü˙`T˘+ÄíÕZd≠™æ‰vô0∏Cu*^√îzÛVV{XΩÃßoT˚PÔBıYH$ã˙¶·∑Bj˜HI5\çxÉµsÄŒÈ4§bg1rÓ∏ÁZﬂCoÇU£CÍÍ ≈(°'—SıÎ\ñ-3≠{‚@≤ﬁ˛ñÓì
ÉÉÍ„©ΩÎp\ƒÄ1'MBØ∏F kˆ≠~0 £È¸∫∫3 xh¯êX`u´î/«D” ¯oôQX5@?¸´Øâ‡ö{ñnÔ˝5 ƒ\◊KzÉmôt‰˛ˆG‰@…˙t£UÊ≈ ◊xÚ}Cø>≠*∏ﬁ∏†ñ7ıª≥â˝ùæ⁄ˆ\’hä1∆--1Ùô¥U^Ï"îkòΩ#'pgÑ8Õ\Awp…í±ÕL¯ÃÙ™j}üπ&4äkiƒÙ¬«onèËY<®µSërù%@>Îì»ÿ≤Ô<∏.j–Òπ˘∏ˇQÊ˙–pØÿÍpa°hAt0·¿i∆îë©≠@ØÜsJgÉ⁄u/ΩXà\i%÷WRÿÂ+ëÍãw2VLŸo/¿#rÆıà˜6ü>€÷^øƒ‹Ebsi˝uf™∑l¯(√ª±8?È±cˇ/oØùß
2Ü,ëOÕÕH'¯Í›±≠8ﬁJ0Öä˘6ƒ\~6ã/^:ìQÁî5ÕS-8\Íî¢e›ü˜äAiπñ#àêIxdµµÛÚ‘≈"fzê’ùr¬‰˝”;Í∑Q+ }ÇP®±∂F‰%gr≈G°;;ˆD˜_!¸ôv%gvZˇç±N GÛÇÕ™ôÆ˘ºVN˚(}GF√≈Içú”ﬂ™ •n&ãl8'çµ?m
L–"4ñfÌ◊ìÛÉZU*>V·-[¬6≠¬A∫—îÖNxôßÿ#ú¡Uâ[ãa:§®∑ª3æ2-®ÍWô'5‹É)Â¶sÿ}¢é°å≠	_∆AÙoDnf:jt$£\wJ¥êÃiÕö∏¸Ô¨¡=ï¿AØÿ/∑ì„G+÷V_ÌÖ…¬34ÀÿË|és£è·y±y=Ú8ÛŸ˙45∆3⁄˘#%≤±œ[*zﬂ∞4jß†VúáZfÁÕw9KÅ‹Üe‡Îlü±Êü‡6„Bkm«Á{dÇËÇ˚∞6Ë∞1`‰™W¡ ãæ&ﬁﬂ á«Ûjºm"˛{vi<U?’Ÿ˚Ïxﬂô$ø°t+zLˇy˚D‚sŸ≈¸æ‰%U‰2¯˚&˘ıìÕéÅ*¬;ÀÂ™õ·9œY÷Ìµ‹GËo˝c{'cÚa9¡‰»ô´3!ÔnPûí“éoÅõZvŸ§Ω@¢MÁ‘[¿P„˚~y)k\yØ¶7ÚMYÜ)€â1«óc’Ñ´ﬂô÷∏iU¯§0"1’¿∑Ù€ŸU‘á—ˆ*◊¢Í,ã˛¨í:®iIÃ	nZœ˙˛›Ω*¬	˛Ñ◊á‡âA%|≠?«≤óénI·4.<^˚ø¯È≈¬{ò JÜBR°E`E2Óu1∞’ëè”÷I°_CAUùH;"DµTîSóf€?,T5MÎ´2Ìb+uÃé7}§/j~KÍIévX©+©≠á'∂/GuôYR§“‡hŒ»u¨/ñmı‹°`?ÙÜ∆€ãG#©^5û¬ıa–ïπ‡nMª˜¬îÚŸ’‚n#_Qíö71Rt—î$%öøµXe∫Ω 	>¯IΩ}ÖUó"Ûª‚ØãÓ¶ö≈Äwˆ˚{Ë†≥lQÕ£>≥ÿ˘Õ«îÏjIÈ˘€_AtP–H4#	b‡ã˘/AÒ◊:K1±WzkL‘˜ó_%uõT∏4N=ËuÆôecµ¬j“Ql˘¨…ıç“ï√≈gƒÚa!ÈŸ•Ëdøêê!GçCÓÌ´¬öëé¢PL}¸Cnyõ~Ô¯Ûw}OÌC∑WhÀ›ΩL‡%†?(ı¿˘Vπ∆0O—‰Â%-Dı	
vkw7∫∑\‰n‚C-ùú‘0XÔø`Àd“≈}~d.§ÎÙâpHXx1ÄÃñË8_±¯´_Ù{Eê≤K∞[∏;˚V)´=pTGä Q8ndÚ¥ïÌò>à"»1»¨`˘N<X≈‚ö˜D%b≥Z‘ê˘æıeßäÅtBüç:e≠5öè‚ˆ>j'sK	è⁄k†,É˘	2–ÂgCÍët˚òxq0)˘\#•∏´˘{ß∞»Oo0êåâü©Íî‚P>4ú-œS#È –éÁ=ƒéVC\-ñ◊LÇéÖ)–K:ˇ®ô„ÎÔ·	7‹gõJQ:ty‚<ónç¢`∏)¸Ÿ¢°œFπUî¸:û¡ë¥·Êåècë@…HC.K‚?\ó◊–Í>’≥w≤¿˜V·{í’7AHE9~™¸7≈•ÿqA[ ö^;éY¶˝ﬂ’µ{h–Ú·Ö Àg¡ä∆(v…Êë\.Nÿj°û±h“RHh=ë≠≥íZ+@ˆµõˇRœ:!1@r§ÄAb†lƒyúË	¬(¸±ÆﬁãgáÿO-ä÷tÄ0 ∂åx8ç8Ç^ÔÚ;¡Èì	√§:y	"üTÅ’Û„,w4¡≥2CÎπä’rÜÌpè*ò˝ÇiÎGΩR«ˇT⁄®u^:|”‚ÈG0πQõg„3≈°›∞ı6tÙxµt“*JÈY
 8ºòõ/ÿy÷z`wRò≥€3òxænÉ—]d>v&ìqW˜ö◊ﬂ?Ï`¬Ä˝KÃûÔ 0†ÙyF…5YZ‡QY≥@0]ö·~ÿí¬sD Eüáo‹5xÃÔ`M∆…TóõÉUÚÙÛ˝V&U@R2~√àoºﬂó-Â '5˚¯|ÕaØYø¿D“Ñî5Ñ†ó?ËÉ˙“Õ.¡Éòù}Ü‹ò+6h`¨Á”∞ÜYÀït¨_<ØHQŸSƒôf	6§ ˛¥Jhç6x—ÓŸ#wÎ+Õ+f√js˜ù™Gzû%¬ÔÑÊ·ƒ.ÖúNQÎï:©omOoaËJ4ÖÈUXlÛ¨O‘K.˚ç‹›¶®nAúSXUÑÓù°iÊd~˝ÌH™Ωh∆,ÿö¢ªΩ}ÕD˘}eORä£v~Í/]Abõ\2
µ1◊®EAk~a{z⁄‚tôZ”£«⁄SÊÅ_ÌÌ8µ9$åè…ßÇÆzÎKÿz_€ä◊Å`Z¿ rÔ4…ÅÓ¸ØJ<`’;H¸ŸﬂItã°¨Ä§™èÆ≠Œƒ«:3ÓªGô¿ºΩaæç{üˇá§ /O ¢$ïàõöä÷'Æ?Á†úÙy
 D”˝èm1î%ƒHezSs‰^-®AîÌı•m´¨œUôhﬂQ2ÀWø–9oW
‚çQÜ…cU∂ÙmûÃK±B«⁄ÕÂKØºMl≠ªh2B∂1â”‰÷ıE CPNì—3bz#XΩ˛Ö·xu≈±)´VLµÁqå™{«üL≠!◊◊%@˙µﬂ≈¸ío.‹Ö~/˛#Ç&÷·b¬†9oi'±WN}Ì”oãü∑øvƒñ (ﬁÒ≥Ú*Y`jô†óï„$§ïRã)æŸDN¢à§ñ≤}˘
Ùj∫ì!êr]O ,Ö>Ω∑r≤ƒÉﬂ5ImÜ∂›	¢Hú(˛¯z´…ùù)ÑûÉÍk™m∑ äœ4S« Ôä3Lú¿7™≠;r¶=ÖäEµÃŸX>c≥≤≥ü˜D£Ç¸w”P	Ü •˚†Ìz]Ãº#QçÎ-HDO‚≠QLÜ£_e®xe•Ç©qa—´∞á
€äó˙ò„∞(≤Äâ´C<”≤õyovÂY√Nô[xÊQOæ>´%œcq>yföËTÔg†	›ÈˇeŸ.Ü8Ÿ`¬¥î3å‚ÌΩ5≤Í¯ÜZ}á÷ùR•“?©ÂÄ°È3ÂO…ñ%´æ«ºõ¿ÍÕw«Ê<~grG†ŒÈt@l—î>Öç`6â<ò@SüGO3Ãh©s[ÀC*éQ?M·ò¢£¬≠üz-"Õê@∫Æ±G5«ˇΩ˙Ìˇ{/
ù∆õ_[ì)vûŸ‚ïLO3ÙZÒ …ç∆¸ÖÉ@HË„ææ”^⁄5x=À79Ω(œ\ˆßŒ◊1…·%røóŸËÛ˜ßO¶•(õƒ3>∫*¥·@õE®*X•^ê˝)ué)ÀxDRù_°——etÓÂÆÖ0b+ª£œL)ü´^’O∆J8#∞S‘∞]‹¶·•∞Sà„eÆc⁄‡(¯.*£=JW@¨Ø1]ˆ¶ÍΩG:~øÑqFüEP›í)]'ò}QæCl8ì√iô‹-©˜îrK∞V±¬›!!œK≠õ.Ã…C®Çñ˝hü5Hú˚vªpØÉVŸÆŸ6=« Ç§`|* *π¨ÿ›∆	T…Dáâ?C“Ùú ,´∑GÛÁu)ko›Ö2p÷Âï.”òﬂ˘ëlª”Iƒ	Àù3çRyÓ|≈˚ÿ¡ÑÍI|oLGLª_òüÁu∆Â÷¡∏îÙ6o`¯ÆÏÇQo˙‘aQµ»Oy¿.ô…¶ΩIÖˆ„Üxüπ©Ö‚*!µ7~÷ãˇtuÅ≈Ûh«v5r·†Aﬁ◊ì%7êîÍbM—5Oe"¢ì‘eZzR÷⁄zt≤E Ñxµ?_ÕÉ¿ï∂≤	J(3øô>«S°"4êœÒ…€Ê}√∫y_πR8T¡æ† ı‚‘ï∑æ≈¿BV˚PÀ4/∆˙4à∫æK^>D‹T∫ïo}ªP∑¨ˇ;Å&¨O:•ùœÈ%PÒ™&Ùr0ÀOÓ lz≠µl7Y≤qF85¸Fó…ﬂ!∞√›
O∆ﬁu¥3êX^≠2ua›~·…Í*æ^LÃh]RL}≥ uÖëâ‡UÚãŒ∂ûÁ¢0P#A¶L}Ω`å	†i≥NÍ“dKüü¯äÙ®Â·+ÃÑ°õ«3¯öµ»˙¥¶«Øß2T¯éÄw∫YöÒŒªπ$ÎÅç˙j‘ƒ¯∆Ÿbµ"xÅNïDç—$Xbbö
Ÿk∑gõkõ‡$Cvœ∫o«~í˜.,Ï¢¯˛Ï6 º÷à˙«T÷ªÁÉÊù∫ªÍ⁄
ö^k :/ËyØ23)_ßõë}Ï,ZÚp
—‹‚›∆<®T•»HØJö·r(·œÉ∏zÆv'ßçe€Y»îpÛ°›Ó@Që3ÔBNÅ†)-mw˝=Zgr™⁄ZÚd±‡
¥ﬂôáZÒªF‹¯*õ∞ÖkDå=3õ%Cû‡Ì`í¿«R9∑â…'3ÄpÕuÑ‡ﬁÄzßNg]†‹Q%˘£\ˇ†9âµ–G
(}»Ú∫◊®ÌÉpıä5mZÚ9hÖ,áôÃu"Y–k◊S|{Â≥Ÿ¸ªE"^T/†\Ô7‚òò˙Ÿ¶°Ûˇ¬Ë
Sà,”≤7N`ïqhﬁ˛dˇ÷üˇñ¨˛µø∆«»∞"¬Tª1láçéÍƒ∑«7àÌ™üç]‡n¿Ø∞Æ«Ó@⁄◊"€	0ãÍ'ä—˙€TÔç#Bâ˝ ë°ØÖ◊¯‘´<Ó’oˇf‡;ÈÁ˘éL”8ç1;˚NÊF»˙“{Ø©≥∞rÖöO-HÒpÒéA0Ñ;˘Ls_)y=$‡V±‚m©@(úø≈›Ω/DÏ—Ú—GÅÌΩ‰7üéür≥zCâ√{™Ìé/˜¬© ⁄Mè¢U{º‚AƒX™'okﬁ“ò∆õf„Hùp€ÚÿÏLπM◊¡\‘ùâ˚ºe˜G•sW¬Zo
7\¢ìÖq[j~N©[«˛_π≥YØÙ9¸Ç%DÍ·Jù1QìÏÍ’ˆyVISÙT$8+–cïL\K-ôBﬁt$‘La∆¸*.n30îF
∂zxCY©øZ˛î¬4ˇDÁ“n¥ú7¯ü°k_á”ºq<¸+÷X{ÆLØ_?ÉÁ_‡Oû™∏Å`Ze2Q›ŸÏóú©0·rπ{¸+ye6á∑…aËv2ƒ˘%ÂËÕFNi},sool“Œç›xˇyÃËﬁw/≈5Yã˝ûû•˙FÀ9dAs‚ñødú{e pe´á√≤NâÎOÍJ  9ÛÉËÈ≤7w^T´ˆßEÛÏc'cïœ≤“ëÄàÖ"~ÃÉöC‡ûºOXóP˝ùG¡MÄê€ìX{.V©»3„&Ù⁄™q%ΩuÇª›ÔYäÍc8+'@ô	#¢]‚≠ΩøÃ4Íw@xØfçñhfOk’4©:r1µÅcx &)˚∑ç„S„fCŒÛtW¸ﬂîC»á§7œ~Ít?£‚Èﬂ°ﬁ¢úõhoäƒ“åä±⁄ª‰Ûd… !¬ùÍÔ≠å±£¶:Q6¸LXV∂ı£@©–Éw‘à_∫Öõƒ›òˇsMqb@à∑XˆCùºÔ,R˝ù7üæÎ∏˙yùS|WW ﬁZH≤"TI9y?]‹/”@ﬂ≥öÁÊõN¬¬ŒCml<OKó§11≥q⁄ªÅ>Ìy9ƒø‹¨¥mmå ∆sªUû…Ñ€˙Í√º“ëÖ√ƒ úÎ@y∂˛/\ﬂlûGÁ¢Ã`H˙∏áfı[b¸‚n
Åy5xv±4∑ëå2F≤ñí∆
Fr|G¸—uM®Q–?	9]®—É˙/Ñ;…›zOÎ5Ä/3ˆ0±ÖÁ‚Mº˜ªænDœ›ãn4K˛z¿ÌGˆulcÔ†èΩ|ú£oI^Öè∏+îı,¶c √[¡dÎ±™ÿÓÕS˘ßçì'·XLï“}«Ó‘ã¶é®Zs|√Ü»˝T~π.4‹˙x7D9kUˇÿxù‰¡™.Ë’)µí¢Ç8∞$´€¶ÿ*å—“ªÇ0=ªX⁄„h≠òqµi≠W^~™›ÃÅ7JOÎQ!É8Û◊5…ƒ ∏-–%4Rûµ◊Ê≠"yﬂ_Ê≈(3p†·Võ„ˆ"æZ˛†øµ/2c¸ø˝reÁY—`—›Ë≥≈w0t¶|0R«Ôo—‚°˜B
ƒÀïA∫ÃHOX‹˜p\}£/ÆGÏ‘"m=ıöÅõÏ6Q5Û™‰‰ä|GÒ.ÇpÁ`ˆVûËØ¨"6msBîcÃavÁ!CWtòCqÁﬁA˝?Eûj1Ìdøì »©êEÂ˝]œÙÅÚ∏Zƒh¯@ÏÊæV‹œ¯}+∆ê ı∫Näôaƒé)«jÿﬁ£CÙN’]0TŒ8ZÜî•È»mè≤rl¸Ï}ñß»˝;3ú∏8m—EF:-àÜ¸sp@:]Imü6€∑∞Ìƒöí`2∏ûég¯Ãçÿ ü+èú%6[yüï´Ÿ…rÚÎ|’3óä32∏,ùÖêGt’vu›Iñ§RÓ±ÌÜ“&Ûa¸Â¡gó•∑x|Â¯'RÿºY}=include/linux/memcontrol.h \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/rculist_nulls.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/indirect_call_wrapper.h \
  include/net/dst.h \
  include/linux/rtnetlink.h \
    $(wildcard include/config/NET_INGRESS) \
    $(wildcard include/config/NET_EGRESS) \
  include/uapi/linux/rtnetlink.h \
  include/uapi/linux/if_addr.h \
  include/net/neighbour.h \
  include/net/rtnetlink.h \
  include/net/netlink.h \
  include/net/tcp_states.h \
  include/uapi/linux/net_tstamp.h \
  include/net/l3mdev.h \
  include/net/fib_rules.h \
  include/uapi/linux/fib_rules.h \
  include/net/fib_notifier.h \
  include/uapi/linux/sock_diag.h \
  include/net/sock_reuseport.h \
  include/linux/filter.h \
    $(wildcard include/config/HAVE_EBPF_JIT) \
  include/linux/compat.h \
    $(wildcard include/config/ARCH_HAS_SYSCALL_WRAPPER) \
    $(wildcard include/config/COMPAT_OLD_SIGACTION) \
    $(wildcard include/config/ODD_RT_SIGACTION) \
  include/uapi/linux/aio_abi.h \
  arch/x86/include/asm/compat.h \
  include/linux/sched/task_stack.h \
    $(wildcard include/config/DEBUG_STACK_USAGE) \
  include/uapi/linux/magic.h \
  arch/x86/include/asm/user32.h \
  include/asm-generic/compat.h \
    $(wildcard include/config/COMPAT_FOR_U64_ALIGNMENT) \
  arch/x86/include/asm/syscall_wrapper.h \
  include/linux/set_memory.h \
    $(wildcard include/config/ARCH_HAS_SET_MEMORY) \
    $(wildcard include/config/ARCH_HAS_SET_DIRECT_MAP) \
  arch/x86/include/asm/set_memory.h \
  include/asm-generic/set_memory.h \
  include/linux/if_vlan.h \
  include/linux/etherdevice.h \
  include/linux/crc32.h \
  include/linux/bitrev.h \
    $(wildcard include/config/HAVE_ARCH_BITREVERSE) \
  arch/x86/include/generated/asm/unaligned.h \
  include/asm-generic/unaligned.h \
  include/linux/unaligned/packed_struct.h \
  include/uapi/linux/if_vlan.h \
  include/crypto/sha1.h \
  include/net/sch_generic.h \
  include/uapi/linux/pkt_cls.h \
  include/net/gen_stats.h \
  include/uapi/linux/gen_stats.h \
  include/net/flow_offload.h \
  include/uapi/linux/filter.h \

kernel/bpf/reuseport_array.o: $(deps_kernel/bpf/reuseport_array.o)

$(deps_kernel/bpf/reuseport_array.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      ELF                      T      4     (               Ë¸ˇˇˇSãX\âÿË¸ˇˇˇçÉ¿   Ë¸ˇˇˇ1¿[√êË¸ˇˇˇUWV∑ÒSÉÏã®î   âD$°    â$Ö¿è.   ã$âÒª   ç< âËâ˙Ë¸ˇˇˇÖ¿tÉÎÑM   âÒâ˙âËË¸ˇˇˇÖ¿uËÉƒ[^_]√ç¥&    ç∂    Ë¸ˇˇˇV1…â∆Sâ”âê<  ∫	   ËsˇˇˇÅ˚D¨  t+Å˚Äª  tSÅ˚ }  t3âπ   ∫	   ËJˇˇˇ1¿[^√çt& êπ    ∫   âË/ˇˇˇÎ“çt& êπ   ∫   âËˇˇˇÎ∫çt& ê1…∫   âËˇˇˇÎ•Ë¸ˇˇˇÅx(	ò ãPÖ€   UWç∫@ˇˇˇVSªÄ ÄÉÏãBxãp|ãBpãh|ãBt∫ Ä  ã@|Ö¿ï¿∂¿¡‡fâD$∏   )9–O¬Ø≈Ö¿çêˇ  H¬¡¯â¡¡·)¡â»˜Îâ¯ ¡˘¡˙) â—1“É·fL$∑…Ëj˛ˇˇ∏ Ä  9∆OØıÖˆçÜˇ  I∆¡¯â∆¡Ê)∆â˜Îâ¯ç2¡˛∫   ¡˘)ÒÉ·fL$∑…Ë"˛ˇˇ1¿Éƒ[^_]√ç¥&    ê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇWVSâ√ã@ãPãRË¸ˇˇˇ%   =   Ñk   ª˚ˇˇˇâÿ[^_√                                        0                                                                                                               ê             6%s: Frequency: %u Hz
 7%s: write: %02x %02x
 wm8739 Ë¸ˇˇˇVçppSâ√ˇ∞<  Vh    Ë¸ˇˇˇçÉ¿   âÚË¸ˇˇˇ1¿Éƒ[^√Vˇt$ãD$É¿pPh   Ë¸ˇˇˇÉƒÈE   ˇ4$VãD$É¿pPh    Ë¸ˇˇˇÉƒÈw   ãS∑Cçä  Qç QPˇ≤   ãCTˇ0h0   Ë¸ˇˇˇçCπ¿  ∫@  Ë¸ˇˇˇâ∆ÉƒÖ¿u
ªÙˇˇˇÈI  â⁄π@   çæ¿   Ë¸ˇˇˇ1…∫   â¯j Ë¸ˇˇˇπ	ò ∫∞   â¯j h0∆  j hè  j hˇˇ  j j Ë¸ˇˇˇπ		ò ∫∞   Éƒ$âÜ0  â¯j j j jj jj j Ë¸ˇˇˇπ	ò ∫∞   Éƒ âÜ4  â¯j h Ä  j hè  j hˇˇ  j j Ë¸ˇˇˇãû¸   â~lâÜ8  Éƒ Ö€tâ¯Ë¸ˇˇˇÈI  çñ0  ∏   Ë¸ˇˇˇ1…â∫   «Ü<  Äª  Ë   1…â∫   Ë   1…â∫   Ë   âπI   ∫   Ë   1…â∫   Ë   âπ   ∫	   Ë   â¯Ë¸ˇˇˇÈI    3%s: I2C: cannot write %03x to register R%d
   6%s %d-%04x: chip found @ 0x%x (%s)
 Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇ                wm8739                                                          Ä       `                       ê                                                                                          debug parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=T. Adachi, Hans Verkuil description=wm8739 driver  º           §ˇ      GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                                          
                                       	        2    	               *       c     H           >   2   =    	 W   ê   ê     k      ˆ     y      3     Ü   o   ò   	 ò   @         £   ∞                      ≥            ∆       Ä     ‘       
     Á       0                   Ò   Ä   0       `                   '          @           N  º        `  0        w  <        ç  [        ®             ≥             —             Ë                                       '             @             M             b                          ë             £             ª             …             ›           È             ¯      
           0     )              wm8739.c wm8739_remove wm8739_log_status wm8739_write.isra.0 wm8739_write.isra.0.cold wm8739_s_clock_freq wm8739_s_ctrl wm8739_probe wm8739_probe.cold wm8739_ops wm8739_ctrl_ops wm8739_driver_init wm8739_driver wm8739_driver_exit wm8739_id wm8739_core_ops wm8739_audio_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free _printk v4l2_ctrl_handler_log_status i2c_smbus_write_byte_data __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_cluster v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__wm8739_id_device_table param_ops_int     "     #     $  !   "  :     [   '  s   '  ë   "  !  "  !  "  4  (  E     h     D                   h                                            "          %  &   &  @     E   %  ^   	  c   %  é   	  ì   %  •   )  ø         *  ⁄   +  ‰       ,      .  ,  8    a  ,  ~  $  ì  -  ´    π    «    ÿ    Ê    ˜    ˛  .  M     k     ∏     É           "          /     0          2  @     H     `     Ä     ∏              /     5        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   S                    	   @          p               )             †  Ä                   %   	   @       ê                  /                                  8                                  4   	   @       ∞  8               E      2       <  7                 X             s                   T   	   @       Ë       	         g      2       |  V                 z             “                    v   	   @       ¯                  â             Ê  
                  Ö   	   @                        ò                ¬                   î   	   @       (  (               †             ¬  u                  ≠             8                    ©   	   @       P                  µ      0       L                   æ              _                     Œ             `  (                                à  `     "         	              Ë
  7                               p  ·                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      ELF                      §k      4     ( & %             Ë¸ˇˇˇUd°    ãÄp  âÂ]ã@ô√ç∂    Ë¸ˇˇˇUâ¬d°    ãÄp  âÂâP1¿1“]√êË¸ˇˇˇWVSâ”â ∑KfÖ…Ñá   fÉ˘uq∂KâŒ¿ÈÉÊÉ¯Ñ§   ¡·∏    ∆a	ÒfâBçBàJ«B    ∂K∂«B    É„ÉÀaàZâÀÉ·¡„	ŸàJ	1…fâJ
ã|$)–¡¯«   [^_√ê[1¿^_√ç¥&    çv ∂ã|$∂KÉ‡«   [É»a^_àJà∏   fâB∏   «B    √çv Ä˘	Ñø   â<	Ñù   π	   â∂¯çAˇÉ‡9œD»â»∆{¡‡«B    	æ$   fâr∂ÒàB∂C¡Êâ˜«B    ÅŒy $ É‡Åœa   ¡‡	¯1ˇâB∂C∆BcÉ‡fâz	»«B    àB∂KçB «B    É·¡·	ÒâJÈ˙˛ˇˇç∂    Ä˘ï¡É¡Èjˇˇˇç¥&    çv π   ÈFˇˇˇç∂    Ë¸ˇˇˇUâÂÖ¿tã ]ã@$ãê  ãÄ   √ê∏    ]ãê  ãÄ   √ç¥&    ç∂    Ë¸ˇˇˇUWVâ∆Sâ”â ∑CfÉ¯Ñì  ifÉ¯Ñ«  fÉ¯u)∂C∆a«A    àA∏   fâA∏   [^_]√çt& êfÖ¿Öˇ   ∂C∆aàA1…[∏   ^_fâJ«B    ]√ç∂    fÉ¯ Ñﬁ   fÉ¯$ÖÑ   ∂Câ¡¿ËÉ·É˛Ño  ¡‡æ   ∆aøp  	»fârΩ   àB«B    ∂C∆Baâ¡É‡fâz
¡·«B    	»àB	∂C∆Baâ¡É‡fâj¡·«B    	»àB∏   È.ˇˇˇfÉ¯u:∂Câ¡¿ËÉ·É˛Ñ›   ∆a¡‡[^«B    	¡∏   _]fâB∏   àJ√[1¿^_]√ç¥&    fê∂Câ¡¿ËÉ·É˛Ñk  ∆a¡‡[^«B    	¡∏   _]fâB∏   àJ√ç∂    ∂Câ¡¿ËÉ·É˛Ñ#  ∆a¡‡[^«B    	¡∏   _]fâB∏   àJ√ç∂    ∂C∆aàAπ   [∏   ^_fâJ«B    ]√çt& ∆cÈˇˇˇ<	Ñÿ   æ	   Ä˘	Ñ‘   çFˇ∂˘É‡9˜Dâ∆{¡‡«B    	»âÒàB∂˘∏   fâBâ˘∂C¡Á¡·«B    âÕÉ‡«B    ¡‡ÅÕa  	ËâBâ»Å…y  a p	¯âB∂C∆BcÉ‡«B    	àB∏   fâB∂C«B$    É‡¡‡	»âB ∏   Èi˝ˇˇçv ∆cÈÿ˛ˇˇ∆cÈê˛ˇˇç¥&    êæ   È,ˇˇˇ<ï¿çpÈ-ˇˇˇç¥&    fêË¸ˇˇˇ1¿√ç¥&    êË¸ˇˇˇÈ¸ˇˇˇç∂    Ë¸ˇˇˇUWVâ∆∏    Sâ”Ë¸ˇˇˇãF(Ö¿t2ãÄî   ãx<ãh@∏    Ë¸ˇˇˇâ{âkãF,âC[1¿^_]√ç¥&    fê1ˇ1ÌÎ‘ç¥&    çv Ë¸ˇˇˇçP«@‡ˇˇˇçHâPâPã    «@  ∏    È¸ˇˇˇêË¸ˇˇˇUWVâ÷Sâ√∏    Ë¸ˇˇˇãC(Ö¿t2ãÄî   ãx<ãh@∏    Ë¸ˇˇˇˇs,UWh    VË¸ˇˇˇÉƒ[^_]√çt& 1ˇ1ÌÎ‘ç¥&    çv UWâ◊VSâ√ÉÏ d°    âD$1¿ˆCÖß   >çt& ãK$çS0â¯Ë¸ˇˇˇâ≈ˆCÖŸ   ãD$d+    Ö   Éƒ âË[^_]√ç∂    Ë¸ˇˇˇãs$çS0âD$â¯Ë¸ˇˇˇãsâ≈Ë¸ˇˇˇ4Ö    úè$˙ÉFÉÉV Ë¸ˇˇˇãL$1“)»FVÉF˜$   Ñzˇˇˇ˚Ètˇˇˇç¥&    ãB «B     âD$ãB$«B$    âD$ãB(«B(    âD$ãB,«B,    âD$ãB0«B0    âD$Èˇˇˇçt& êãD$âG ãD$âG$ãD$âG(ãD$âG,ãD$âG0Èˇ˛ˇˇË¸ˇˇˇç¥&    çt& Ë¸ˇˇˇUâÂWVSÉÏãPãMãuÖ“t]ãxâ√∫ˇˇˇˇ∏ÍˇˇˇÖˇt=É{ t7â$Ö…t=Öˆt9∏˘ˇˇˇ∫ˇˇˇˇÅ˛ˇ  wã$âÒâ¯Ë¸ˇˇˇâs1¿1“«C   Éƒ[^_]√çt& êÉƒ∏Íˇˇˇ∫ˇˇˇˇ[^_]√ç∂    Ë¸ˇˇˇWVSãt$Ö¿tbâ”Ö“tBâ«â Ö…tBÖˆt>9ÛâÒFÀË¸ˇˇˇ9Ûw∆Dˇ ∏˘ˇˇˇ[^_√)Ûç71“âŸË¸ˇˇˇâ[^_√çt& ∏˘ˇˇˇÎ›êâŸ1“â¯Ë¸ˇˇˇ∏ÍˇˇˇÎ ∏ÍˇˇˇÎ√ç¥&    Ë¸ˇˇˇUâÂWç}SãHããWˇpâÿËaˇˇˇYçe¯[ô_]√ç¥&    êË¸ˇˇˇUâÂWç}SããWãxÖˇu?Ö€t#Ö“tâ—âÿ1“Ë¸ˇˇˇçe¯∏Íˇˇˇ∫ˇˇˇˇ[_]√çv çe¯∏Íˇˇˇ∫ˇˇˇˇ[_]√ç¥&    ãHˇpâÿËÎ˛ˇˇYçe¯[ô_]√fêË¸ˇˇˇUWVâ∆Sâ”ÉÏã∫§   d°    âD$1¿ãÇ†   ∑íî   â$ÖˆÑ6  âÕ∂N∏   ”‡%øÔˇˇÑ˚   ∑FÉ‡˜fÉ¯Ö  ãK$)◊ãÜ‰  âsâL$ãKTãs8âì§   ˘âKT+KX â4$âS8É˝ÑÛ   ãÑ®ÿ  «D$    âD$Ë¸ˇˇˇË¸ˇˇˇãD$çL$dã    çhã≤p  âäp  ã@Ö¿t,êâ⁄âl$Ë¸ˇˇÖ¿uÅ|$ ˇˇw«D$ˇˇˇˇãEÉ≈Ö¿u’d°    â∞p  Ë¸ˇˇˇË¸ˇˇˇãD$Ö¿uOãSTã$)˙âK8âST;SXÇ  ª§   ã|$â{ãT$d+    ÖÙ   Éƒ[^_]√ç¥&    çv 1¿ÎŸçt& = ˇˇ∫ÚˇˇˇF¬Î¢ç¥&    fêã®‹  «D$    Ë¸ˇˇˇË¸ˇˇˇçud°    ãàp  âL$çL$âàp  ãE1ÌÖ¿t3fêâ⁄ât$Ë%˚ˇˇ®uÅ|$ ˇˇw«D$ˇˇˇˇ—ËÉ∆	≈ãÖ¿u“ÉÂãL$d°    âàp  Ë¸ˇˇˇË¸ˇˇˇãT$çD- Ö“Ñ˛˛ˇˇÅ˙ ˇˇ∏ÚˇˇˇF–ÖÌ∏   D¬È·˛ˇˇç¥&    êË¸ˇˇˇç¥&    fêË¸ˇˇˇUWâœVâ÷Sâ√ÉÏã@ Ö¿tË‚ˇˇˇâ≈Ö¿x|ããããË¸ˇˇˇâ√Ö¿xSã)¡âÎtG∫   ∏   â$ã.9—F—Ë¸ˇˇˇã$É¯v9¡Ö¸ˇˇˇçP9¡âËE ∫   Ë¸ˇˇˇÖ¿x,√)Éƒâÿ[^_]√çt& 1ÌÎàçt& Éƒâ√âÿ[^_]√çt& Éƒâ√âÿ[^_]√çt& Ë¸ˇˇˇUâÂVâ∆SÉÏãMãUd°    âD$1¿ãEâL$â$Ö¿tYÉ‚t,1€ãVãË¸ˇˇˇ√Ö¿I√ôãL$d+    u?Éƒ[^]√çt& ãÖ¿t"ã@ â‚çL$ËÃ˛ˇˇâ√ôÖ¿xÕãL$ã$Î≤çt& ∏Íˇˇˇ∫ˇˇˇˇÎ¥Ë¸ˇˇˇç¥&    Ë¸ˇˇˇÖ¿xOÉ˘tJVâ÷Sâ√çÉ¯w4âÿô˜˛Ö“u+É˚6ãD$«@   É˛wçVˇ∏   ÖÚtç¥&    çv 1¿[^√çv 1¿√çt& êÉ˛[^î¿√ç¥&    Ë¸ˇˇˇÖ¿x?Wâœ1…Vâ÷Sâ√çÉ¯w!âÿô˜˛Ö“uÉ˚~&ÉÎÉ˚wÉ˛î¡Éˇt3[â»^_√çt& ê1¿√çt& êÉˇuËãD$«@   É˛wÿÎç¥&    fêãD$1…«@   É˛wªçFˇÖî¡Î±çt& Ë¸ˇˇˇÉ¯'wVVâ÷ôSâ√˜˛1¿Ö“u"É˘trÉ˚Ñ°   Ö€t;É˚uNÉ˛Ñ≠   [^√fêÉ˚$u;1¿É˛uÔãD$Éxî¿Î‚ç¥&    1¿√çt& êÉ˛uŒãD$«    ∏   ÎΩfêÉ˛î¿Î≥ç¥&    êÉ˚ tÎ!É„˚É˚uúÉ˛uóãD$Éxî¿Îäç¥&    É˚$tàÈyˇˇˇç∂    É˛ÖjˇˇˇãD$« 	   ∏   ÈVˇˇˇçv ãD$«    ∏   È?ˇˇˇçt& Ë¸ˇˇˇÉ¯hÑ≤   w É¯fÑó   ∫   É¯guEâ–√ç¥&    çv É¯jtk∫    =†   t‚∫    É¯itÿ=∫   t~=ª   u9∫‡  â–√ç¥&    ∫Ä  É¯et±É¯Qtyw“É¯tb∫    É¯Ptõ∫    É¯tëÈ¸ˇˇˇç¥&    fê∫    â–√ç¥&    ê∫@  â–√ç¥&    ê∫¿   â–√ç¥&    ê∫   ÈIˇˇˇç∂    ∫    È9ˇˇˇç∂    ∫    È)ˇˇˇç∂    Ë¸ˇˇˇWVSÖ“xTâ∆Å˙   â◊â”É˙ âN—â”âNâÿ[^_√êª   ø   ∫¿ â¯Ë¸ˇˇˇâFÖ¿t¯âFâÿ[^_√ç¥&    êªÍˇˇˇâÿ[^_√ªÙˇˇˇÎ∑ç¥&    ç¥&    Ë¸ˇˇˇUWâœVâ÷Sâ√∏    ÉÏË¸ˇˇˇãC(â$Ö¿Ñô  Öˇt	9{ÖÇ  ãC,É¯"áÕ   æÄ    âD$Ö¿à∫   ã<$çàÑ   ç≈4  ãnâD$â˙âL$¬ãC9hÖè   ãDœ9¬uÈ!  fêã 9¬Ñ  ;XuÒâáCã$âD$1¿Ë¸ˇˇˇâ∆ãD$8  âD$ÖˆuÈ’   ç∂    ã$âË¸ˇˇˇâ∆Ö¿Ñª   çÜT  Ë¸ˇˇˇÑ¿u›âı1…Ö…t5ãD$ˆDÖu*ãmhÖÌuÍêªÍˇˇˇ∏    Ë¸ˇˇˇâÿÉƒ[^_]√ç¥&    ãD$ç| ãD$ãD≈9«uÎ¿ç¥&    çv 9”t É¡ã 9«t©Éx ãPuÍÖ“tÌÉz tÁ9”u‡ã|$ãCçI¡‚îæÿ  âBã$âË¸ˇˇˇâ∆Ö¿ÖEˇˇˇãD$1€Ë¸ˇˇˇÈfˇˇˇª˛ˇˇˇÈ\ˇˇˇªˇˇˇˇÈRˇˇˇªΩˇˇˇÈHˇˇˇçv Ë¸ˇˇˇUâ’WVSÉÏãÄ‰  dã    âT$1“«D$    ãúàÿ  Ë¸ˇˇˇË¸ˇˇˇç{d°    ã∞p  ât$çt$â∞p  ã[Ö€t:fêâ|$>çt& ãK$çS0âËË¸ˇˇˇâ∆ÖˆuÅ|$ ˇˇw«D$ˇˇˇˇã_É«Ö€u»ã|$d°    â∏p  Ë¸ˇˇˇË¸ˇˇˇãD$ãT$d+    unÉƒ[^_]√çt& êË¸ˇˇˇçS0ãs$âD$âËË¸ˇˇˇã[â∆Ë¸ˇˇˇÖ    úè$˙ÉCÉÉS Ë¸ˇˇˇãL$1“)»CSÉC˜$   ÑMˇˇˇ˚ÈGˇˇˇË¸ˇˇˇç¥&    çv Ë¸ˇˇˇUâ≈WVSÉÏd°    âD$ãÖ‰  «D$    ãúêÿ  Ë¸ˇˇˇË¸ˇˇˇç{d°    ã∞p  ât$çt$â∞p  ã[Ö€t=çt& êâ|$>çt& ãK$çS0âËË¸ˇˇˇâ∆ÖˆuÅ|$ ˇˇw«D$ˇˇˇˇã_É«Ö€u»ã|$d°    â∏p  Ë¸ˇˇˇË¸ˇˇˇãD$ãT$d+    unÉƒ[^_]√çt& êË¸ˇˇˇçS0ãs$âD$âËË¸ˇˇˇã[â∆Ë¸ˇˇˇÖ    úè$˙ÉCÉÉS Ë¸ˇˇˇãL$1“)»CSÉC˜$   ÑMˇˇˇ˚ÈGˇˇˇË¸ˇˇˇç¥&    çv Ë¸ˇˇˇUWV1ˆSâ√â–â ÅÏ¨   dã    âå$®   1…ãå$¿   «D$    «D$     ã¨$ƒ   âL$$∑Kâ\$É·˜âD$fÉ˘Ö´   Ö¿Ñ.  ãÉ‰  «D$    ãúêÿ  Ë¸ˇˇˇË¸ˇˇˇç{d°    ãêp  âT$çT$âêp  ãsÖˆtBçv â|$>çt& ãN$çV0çD$Ë¸ˇˇˇâ√ˆ√uÅ|$ ˇˇw«D$ˇˇˇˇÉ«—Î	] ã7Öˆu¡ã|$d°    â∏p  Ë¸ˇˇˇË¸ˇˇˇãt$ãÑ$®   d+    Öç   Åƒ¨   â[^_]√fêË¸ˇˇˇçV0ã^$âD$çD$Ë¸ˇˇˇãvâ√Ë¸ˇˇˇ4Ö    úè$˙ÉFÉÉV Ë¸ˇˇˇãL$1“)»FVÉF˜$   Ñ=ˇˇˇ˚È7ˇˇˇçt& êçt$(π    â˜ât$Û´Èº˛ˇˇË¸ˇˇˇç¥&    ç∂    Ë¸ˇˇˇÉ¯QtfwÉ¯tO∫    É¯Pu%â–√fê=∫   t)=ª   u∫‡  â–√ç¥&    çv ∫    É¯t—È¸ˇˇˇê∫   Îƒç¥&    fê∫    Î¥ç¥&    fê∫    Î§ç¥&    fêË¸ˇˇˇU1ÌWVç≤Ñ   Sâ√â–8  âﬂÉÏât$ç4’4  âT$âL$â4$âD$âËÖÌtãt$ˆD∑tDã$1“ç4ãD$ãD«9∆uÎ(É¬ã 9∆tãHÖ…uãHÖ…tÏãIÖ…u‚ã 9∆uÂçt& çD â≈ãhÖˇu§∫¿  Ë¸ˇˇˇâ∆Ö¿Ñá   ãD$1“8  âD$Ö“~$ãD$ˆDÉuã[hÖ€uÍãD$â01¿Éƒ[^_]√fêãD$ã<$ãD√ç,;9≈u'Î”çt& ç<RÉ¬âLæçRãxçéâ9ãxâyã 9ËtÆãHÖ…uÿãHÖ…tÏãIÖ…tÂÎ»∏ÙˇˇˇÎüç¥&    fêË¸ˇˇˇUWâ◊â¬Vâ∆1¿SË¸ˇˇˇÖ¿t=â√çt& çÉT  Ë¸ˇˇˇÑ¿uçãP  â˙âÿËÇ˛ˇˇâ≈Ö¿Öà   âÿâÚË¸ˇˇˇâ√Ö¿u…âÚ1¿Å«Ù   Ë¸ˇˇˇâ√Ö¿uÎVfêãÉP  Ö¿u,âÿâÚË¸ˇˇˇâ√Ö¿t;çÉT  Ë¸ˇˇˇÑ¿uÿãìP  ãDªâTªË¸ˇˇˇâÿâÚ«ÉP      Ë¸ˇˇˇâ√Ö¿u≈1Ì[âË^_]√çv âÚ1¿Ë¸ˇˇˇâ√Ö¿tÁêãÉP  Ë¸ˇˇˇâÿâÚ«ÉP      Ë¸ˇˇˇâ√Ö¿u‹[âË^_]√çt& êË¸ˇˇˇUWVâ∆â–SÉÏãT$(â$É˙"á†   âÀæä    Ö…àè   çπÑ   çë8  â|$¡Áâ|$ç|>âT$ãTñÖ¿tÖ€udÉ‚uoãT$Ω˛ˇˇˇãD÷9«tSã\÷Å˚ ˇˇá¸   ãC«C    â «C    âD$âËP˛ˇˇâ≈Ö¿tzãD$âCã$âCÉƒâË[^_]√çv ΩÍˇˇˇÉƒâË[^_]√êã$ΩÍˇˇˇâ”	√tÁã\$ã\ﬁ9ﬂuÎçt& ã9ﬂt;Cuı;SÑrˇˇˇã9ﬂuÏÉƒΩ˛ˇˇˇ[âË^_]√ç¥&    ãCãâAââÿ«   «C"  Ë¸ˇˇˇãD$ãD∆9«t&ãD$Ö¿tË¸ˇˇˇãD$‡˚ˇˇË¸ˇˇˇâËÉƒ[^_]√êãD$«DÜ    ÎÃâ›È@ˇˇˇç¥&    çt& Ë¸ˇˇˇVSâ√∏    Ë¸ˇˇˇãC(Ö¿tVˇs,1“âŸËJ˛ˇˇZÖ¿uUãs(«C(    ∏    Ë¸ˇˇˇˆF,t[^√ç¥&    êË¸ˇˇˇãF®u,dˇ[^È¸ˇˇˇç¥&    çv [∏    ^È¸ˇˇˇçt& Îßçt& ãFÉ(uŒãVçFãRË¸ˇˇˇÎæç¥&    ç¥&    êË¸ˇˇˇãP(Ö“tÈ?ˇˇˇç¥&    √ç¥&    Ë¸ˇˇˇãP(Ö“tËˇˇˇ1¿√ç¥&    çt& êË¸ˇˇˇÉ¯kÑ¬   wÉ¯9t[π    É¯`u!â»√ç∂    É¯lt3É¯zuVπ`   â»√ç∂    É¯1u[Ézπ    ∏    E»Î«ç¥&    êπ    Î∏ç¥&    fêÉzπ    ∏    E»Îúçt& ê=∫   tQ=ª   u2π‡  ÎÇçv É¯Qt[w·É¯tDπ    É¯PÑeˇˇˇπ    É¯ÑWˇˇˇÈ¸ˇˇˇçv π    ÈEˇˇˇç∂    π   È5ˇˇˇç∂    π    È%ˇˇˇç∂    π    Èˇˇˇç∂    Ë¸ˇˇˇUΩ    Wâ«VSÉÏâD$ç@ÏâD$∏    Ë¸ˇˇˇâ¯çøÿ˛ˇˇ-Ñ  âD$ç¥&    ãã39˚u'ÈÉ   çv «B(    âÿâÛË¸ˇˇˇâËË¸ˇˇˇã9˛tcâ∆ãCâFâ0ãC«   «C"  Ö¿tË¸ˇˇˇãSÖ“t¿ãJ(ˆA,u∞âL$â$Ë¸ˇˇˇãL$ã$ãA®Ö¬   dˇâ$Ë¸ˇˇˇã$ÎÇfêãt$É≈É«ãË¸ˇˇˇâÉ¿âD$Å˝∏   ÖJˇˇˇãD$ã|$ã@ÏãçX‰çq‰9D$têâÿË¸ˇˇˇâÿâÛË¸ˇˇˇãFçp‰çC9¯u„∏    Ë¸ˇˇˇãD$ãò˚ˇˇÖ€tË¸ˇˇˇãÉT  ®utdˇË¸ˇˇˇã[hÖ€u‚ã|$çG¯Ë¸ˇˇˇˆá–˙ˇˇt.Éƒ[^_]√ãAÉ(Ö4ˇˇˇçAãIâ$ãIË¸ˇˇˇã$ÈˇˇˇË¸ˇˇˇãD$ãÄ¨˙ˇˇ®u4dˇÉƒ[^_]È¸ˇˇˇãÉX  É(uÉãìX  çÉT  ãRË¸ˇˇˇÈjˇˇˇãD$ãÄ∞˙ˇˇÉ(uøãL$ãë∞˙ˇˇçÅ¨˙ˇˇãRË¸ˇˇˇÎ•ç¥&    fêË¸ˇˇˇUWVSÉÏHãt$\â$âL$ât$d°    âD$DãD$d«D$4    «D$8    «D$<    «D$@    É‡âD$É¯Ñ_  ãD$dÉ‡É¯ÑO  â◊Öˆt	—ÖA  ãD$dãt$¡ËÉ‡Öˆï¬8–Ö&cmd_drivers/media/i2c/wm8739.o := gcc -Wp,-MMD,drivers/media/i2c/.wm8739.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"wm8739"' -DKBUILD_MODNAME='"wm8739"' -D__KBUILD_MODNAME=kmod_wm8739 -c -o drivers/media/i2c/wm8739.o drivers/media/i2c/wm8739.c 

source_drivers/media/i2c/wm8739.o := drivers/media/i2c/wm8739.c

deps_drivers/media/i2c/wm8739.o := \
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
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/ratelimit_types.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/msr.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
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
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/COMPAT) \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/osq_lock.h \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/debug_locks.h \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/generated/bounds.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/rhashtable-types.h \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/ACPI) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/ioport.h \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/irqhandler.h \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/fwnode.h \
  include/linux/resource_ext.h \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  include/linux/ctype.h \
  arch/x86/include/asm/acenv.h \
  include/acpi/acnames.h \
  include/acpi/actypes.h \
  include/acpi/acexcep.h \
  include/acpi/actbl.h \
  include/acpi/actbl1.h \
  include/acpi/actbl2.h \
  include/acpi/actbl3.h \
  include/acpi/acrestyp.h \
  include/acpi/platform/acenvex.h \
  include/acpi/platform/aclinuxex.h \
  include/acpi/platform/acgccex.h \
  include/acpi/acoutput.h \
  include/acpi/acpiosxf.h \
  include/acpi/acpixf.h \
  include/acpi/acconfig.h \
  include/acpi/acbuffer.h \
  include/linux/dynamic_debug.h \
  include/acpi/acpi_bus.h \
    $(wildcard include/config/X86_ANDROID_TABLETS) \
    $(wildcard include/config/ACPI_SYSTEM_POWER_STATES_SUPPORT) \
    $(wildcard include/config/ACPI_SLEEP) \
  include/acpi/acpi_drivers.h \
    $(wildcard include/config/ACPI_DOCK) \
  include/acpi/acpi_numa.h \
    $(wildcard include/config/ACPI_HMAT) \
  include/acpi/acpi_io.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/acpi.h \
    $(wildcard include/config/ACPI_APEI) \
  include/acpi/pdc_intel.h \
  arch/x86/include/asm/numa.h \
    $(wildcard include/config/NUMA_EMU) \
  arch/x86/include/asm/numa_32.h \
  include/linux/regulator/consumer.h \
    $(wildcard include/config/REGULATOR) \
  include/linux/suspend.h \
    $(wildcard include/config/VT) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/HIBERNATION_SNAPSHOT_DEV) \
    $(wildcard include/config/PM_SLEEP_DEBUG) \
    $(wildcard include/config/PM_AUTOSLEEP) \
  include/linux/swap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
    $(wildcard include/config/CGROUP_WRITEBACK) \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/list_lru.h \
  include/linux/shrinker.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/page_counter.h \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/sizes.h \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bvec.h \
  include/linux/highmem.h \
  include/linux/cacheflush.h \
  arch/x86/include/asm/cacheflush.h \
  include/asm-generic/cacheflush.h \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  include/linux/pagemap.h \
  include/linux/hugetlb_inline.h \
  include/uapi/linux/mempolicy.h \
  include/linux/freezer.h \
  include/uapi/linux/i2c.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
  include/media/media-devnode.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/v4l2-subdev.h \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/wm8739.o: $(deps_drivers/media/i2c/wm8739.o)

$(deps_drivers/media/i2c/wm8739.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/refcount.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/generated/bounds.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.ho build the kernel author=Joel Fernandes license=GPL v2 depends= retpoline=Y intree=Y name=kheaders vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII  kheaders.tar.xz           GNU  ¿       ¿                GNU ])u)Uﬁo«ü⁄C	a∑¢º•        Linux                Linux                                   $                                                                      kheaders                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         Òˇ       #            #     *             8            J       F    
 d   F       
 y   \       
 è            Òˇû   k   	    
 ¥   t       
 Ã   Ä   	    
 ·   â       
 Ù   ó   7    
                                 )             ?           N             U             a             l      #     x            å  0?;       §              kheaders.c ikheaders_read ikheaders_init kheaders_attr ikheaders_cleanup __UNIQUE_ID_description96 __UNIQUE_ID_author95 __UNIQUE_ID_license94 kheaders.mod.c __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module sysfs_create_bin_file cleanup_module memcpy kernel_kobj __fentry__ init_module kernel_headers_data kernel_headers_data_end sysfs_remove_bin_file      &     (     $     &     )          (          %     "     %          *                        Ã   '  Ù  #   .symtab .strtab .shstrtab .rel.text .rel.init.text .rel.exit.text .rodata .rel__mcount_loc .modinfo .rodata.str1.1 .note.gnu.property .note.gnu.build-id .note.Linux .data .rel.data..ro_after_init .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   #                     	   @       H;                )             c   #                  %   	   @       ,H; 8               8             Ü                     4   	   @       dH;                C             ï   0?;                O             ≈?;                   K   	   @       |H;                \             Õ?; Œ                  e      2       õ@;                  t             ¨@; (                  á             ‘@; $                  ö             ¯@; 0                  ¶             (A;                    ∞             @A;                     ¨   	   @       åH;                …             ÄA;            @       ≈   	   @       úH;                „             ÄC;                    Ë      0       ÄC; &                 Ò              ¶C;                                  ®C; ∞     !         	              XF; ∫                               ¨H;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  cmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        cmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        # SPDX-License-Identifier: GPL-2.0
# Any varying coverage in these files is non-deterministic
# and is generally not a function of system call inputs.
KCOV_INSTRUMENT		:= n

obj-y += mutex.o semaphore.o rwsem.o percpu-rwsem.o

# Avoid recursion lockdep -> KCSAN -> ... -> lockdep.
KCSAN_SANITIZE_lockdep.o := n

ifdef CONFIG_FUNCTION_TRACER
CFLAGS_REMOVE_lockdep.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_lockdep_proc.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_mutex-debug.o = $(CC_FLAGS_FTRACE)
endif

obj-$(CONFIG_DEBUG_IRQFLAGS) += irqflag-debug.o
obj-$(CONFIG_DEBUG_MUTEXES) += mutex-debug.o
obj-$(CONFIG_LOCKDEP) += lockdep.o
ifeq ($(CONFIG_PROC_FS),y)
obj-$(CONFIG_LOCKDEP) += lockdep_proc.o
endif
obj-$(CONFIG_SMP) += spinlock.o
obj-$(CONFIG_LOCK_SPIN_ON_OWNER) += osq_lock.o
obj-$(CONFIG_PROVE_LOCKING) += spinlock.o
obj-$(CONFIG_QUEUED_SPINLOCKS) += qspinlock.o
obj-$(CONFIG_RT_MUTEXES) += rtmutex_api.o
obj-$(CONFIG_PREEMPT_RT) += spinlock_rt.o ww_rt_mutex.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock_debug.o
obj-$(CONFIG_QUEUED_RWLOCKS) += qrwlock.o
obj-$(CONFIG_LOCK_TORTURE_TEST) += locktorture.o
obj-$(CONFIG_WW_MUTEX_SELFTEST) += test-ww_mutex.o
obj-$(CONFIG_LOCK_EVENT_COUNTS) += lock_events.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       // SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/export.h>
#include <linux/irqflags.h>

noinstr void warn_bogus_irq_restore(void)
{
	instrumentation_begin();
	WARN_ONCE(1, "raw_local_irq_restore() called with IRQs enabled\n");
	instrumentation_end();
}
EXPORT_SYMBOL(warn_bogus_irq_restore);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            // SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/export.h>
#include <linux/irqflags.h>

noinstr void warn_bogus_irq_restore(void)
{
	instrumentation_begin();
	WARN_ONCE(1, "raw_local_irq_restore() called with IRQs enabled\n");
	instrumentation_end();
}
EXPORT_SYMBOL(warn_bogus_irq_restore);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <waiman.long@hpe.com>
 */

/*
 * Collect locking event counts
 */
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/fs.h>

#include "lock_events.h"

#undef  LOCK_EVENT
#define LOCK_EVENT(name)	[LOCKEVENT_ ## name] = #name,

#define LOCK_EVENTS_DIR		"lock_event_counts"

/*
 * When CONFIG_LOCK_EVENT_COUNTS is enabled, event counts of different
 * types of locks will be reported under the <debugfs>/lock_event_counts/
 * directory. See lock_events_list.h for the list of available locking
 * events.
 *
 * Writing to the special ".reset_counts" file will reset all the above
 * locking event counts. This is a very slow operation and so should not
 * be done frequently.
 *
 * These event counts are implemented as per-cpu variables which are
 * summed and computed whenever the corresponding debugfs files are read. This
 * minimizes added overhead making the counts usable even in a production
 * environment.
 */
static const char * const lockevent_names[lockevent_num + 1] = {

#include "lock_events_list.h"

	[LOCKEVENT_reset_cnts] = ".reset_counts",
};

/*
 * Per-cpu counts
 */
DEFINE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * The lockevent_read() function can be overridden.
 */
ssize_t __weak lockevent_read(struct file *file, char __user *user_buf,
			      size_t count, loff_t *ppos)
{
	char buf[64];
	int cpu, id, len;
	u64 sum = 0;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	id = (long)file_inode(file)->i_private;

	if (id >= lockevent_num)
		return -EBADF;

	for_each_possible_cpu(cpu)
		sum += per_cpu(lockevents[id], cpu);
	len = snprintf(buf, sizeof(buf) - 1, "%llu\n", sum);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

/*
 * Function to handle write request
 *
 * When idx = reset_cnts, reset all the counts.
 */
static ssize_t lockevent_write(struct file *file, const char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	int cpu;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	if ((long)file_inode(file)->i_private != LOCKEVENT_reset_cnts)
		return count;

	for_each_possible_cpu(cpu) {
		int i;
		unsigned long *ptr = per_cpu_ptr(lockevents, cpu);

		for (i = 0 ; i < lockevent_num; i++)
			WRITE_ONCE(ptr[i], 0);
	}
	return count;
}

/*
 * Debugfs data structures
 */
static const struct file_operations fops_lockevent = {
	.read = lockevent_read,
	.write = lockevent_write,
	.llseek = default_llseek,
};

#ifdef CONFIG_PARAVIRT_SPINLOCKS
#include <asm/paravirt.h>

static bool __init skip_lockevent(const char *name)
{
	static int pv_on __initdata = -1;

	if (pv_on < 0)
		pv_on = !pv_is_native_spin_unlock();
	/*
	 * Skip PV qspinlock events on bare metal.
	 */
	if (!pv_on && !memcmp(name, "pv_", 3))
		return true;
	return false;
}
#else
static inline bool skip_lockevent(const char *name)
{
	return false;
}
#endif

/*
 * Initialize debugfs for the locking event counts.
 */
static int __init init_lockevent_counts(void)
{
	struct dentry *d_counts = debugfs_create_dir(LOCK_EVENTS_DIR, NULL);
	int i;

	if (!d_counts)
		goto out;

	/*
	 * Create the debugfs files
	 *
	 * As reading from and writing to the stat files can be slow, only
	 * root is allowed to do the read/write to limit impact to system
	 * performance.
	 */
	for (i = 0; i < lockevent_num; i++) {
		if (skip_lockevent(lockevent_names[i]))
			continue;
		if (!debugfs_create_file(lockevent_names[i], 0400, d_counts,
					 (void *)(long)i, &fops_lockevent))
			goto fail_undo;
	}

	if (!debugfs_create_file(lockevent_names[LOCKEVENT_reset_cnts], 0200,
				 d_counts, (void *)(long)LOCKEVENT_reset_cnts,
				 &fops_lockevent))
		goto fail_undo;

	return 0;
fail_undo:
	debugfs_remove_recursive(d_counts);
out:
	pr_warn("Could not create '%s' debugfs entries\n", LOCK_EVENTS_DIR);
	return -ENOMEM;
}
fs_initcall(init_lockevent_counts);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     Ÿ~$!Hçßûqö⁄Hºµí{rËMP–û"‹éÏ4ûEj· ÿ–TòÜr*6Ù3fVTªû#‡ÃÎ∂ø≥#y°Å´ƒÈø\◊ˇ∆ãdƒV#Lï∏¯¬	gÓAÁ90-;Í‚a∂ßé“ZÒ“Vá&EÅb‘FÅŸ†„Ü>F∞<ï°ãí¢àﬂÃIT∞nUÈ–+Àò—)Æ%?∫Z∂óΩ%%”Dî¶d@ˆˇí-ÿláà¸qúıM∂%pc»·Ç ô8w"ÛöˆÛ˜S≤≤ecxÂÍb‘ù-ORû0\<m≈ùâ/◊F‚ÍÏê&›0]ÃÿûwÖ€Ï'ã:ª†MìT;2››Ñr¨†ø,˚l⁄ŒT0cB…€fS§§ÙtLCF8Í<|ıhΩÑ–yü¶„_=*¶OQ©°¶Õwq0ŒÖÈ\®ëÍÁ÷/˝Üñ›ëo?´€,É›'vÄèË°7	º.5ÖZ<ëfwÙ2TôKèﬁZ~E0Q€9è#ÜêëDêÀ“1fùS$F—U5HÑZNŒãW´›Ãæ ∑¨•ï:z9ÖÔCyúZ=‚ˇﬁ\Ë∆‹^Ù˝>|”q5M¯ÛÆûDøX˚úÔçS±@T2ª4P¬ ¨8†^/Z];N∑≤∏Üª“•ÍHbÇÉ∑!∞—”dj H’e„ŒÀ˘	VÎêî±•C›w⁄“∏ÂmÄ®'Z‘iﬁ3 ÌÏ˜i–^ë{Ë∆?s…téç‚ÌQC¡e`i`x∏)©\Ùª”nßcHö	ESEØF±7 êñ‘◊îø ˙M—˚A´ñÔü∆b$	 y∑◊ÇWß4‡Ë“L†;ô*Èp@€ö„˛Ö∏˜Öä£¨F∞HŸeÿ¯u6ŸVèöé-xxL=}Ül˙SÂ@µxÉj?∆¯`ëπ˙ø
'‡Ò≈
&lœN|qô∂fÁÊyΩ‡ÄV¬“SÚ≠µKˇÔ”ñRadB/Oø`&.|ﬂt∆0RÉ†nx«"‹Aû#~ü’y9π}≥ﬁ–J»Ñıt\U†.SÒo®[:JÄ1Ù≠CÕ…çj|x©Rq‘ÜtÌZÅ0(cÃıøp<R™≠È-HÈ….Î≈ñˆs>≈?íÿbd$¿D^E”,‚î@6ÀjµpØóhL∫©÷˙RéÃ´<Kà+éO%›‰ﬁ≠Á ¬ ˛-ˆ,7l#ÌPÂ’–πü∫˝Õxbapæ)ñ‹∞µ©CÁ.zmv$d‰B¡ëÁjöV›rÀÀµ†äÏπ}≤:]R≠-÷$πúﬂaﬁ9Ô”ª#¸îØV;Pl≤*ÿÄ[‹\+w;Q€ÚÁ”ÓÍ†4Ìú÷3«„[œG˙kËm¢%¯‰I%À5µÒ_YJiÀXËÖ¿©f7m‚•Bâl∏;ıØc†œ–øô32XÓôù]ˆömáÕπdÿøË]ÍA’.káTvH∆?^*;˘…ªU#øz⁄∑<◊A…q2/ÅÑèy•+æ[e©-ãÚ∫µ‰6‚ßß%‚ŸJ2K–i‹pﬁ0«f@+0è~ﬂ–ÍùÎ	LwŸ.yô+gÌß=Öìﬂ¡ÙKQb˝–«Ò_Hk§GDâÃÊU¬—#‚Új;aﬂ\¡j·w\Äÿ8Æ#™Dº—ˇÛPA«Hø›],Ø3l˘,Fç”≈ı{Xˆ¿Àô¯´gÕ–Má‚ÆﬂƒÈA‘p*A¶˚,GÍ AÓñM-∑r	·∆Õ„[˜◊[”%Ü‚ØÔXÃ1
u8¥/ôÃIfá€¶Àúã‡≥Lh˚Ow%≤( *ñ‰	±>V˝RFì6J~Jk¸FŒÀÓªÈJ≈JÕ>πÚR	ôcÏJ-©è1˛Yªï-˚c•9ûÁ≥Oã∏x9RCèÙ˚∞À∑ª∂Î1Üπ)q=wÙ@xßê	'y”€;kˆA0@˛Ò˜˘'Ÿ_›|~NJÇœî¡Ó¶ÅL≥ü â)óÇÙÖg˛$ësè∑s'I∑xéC0ŸPYﬁ˛üÿcƒ<‚:*ˇ√ ÃÙ5«®,fÜA6¿<MXL õœÓx–ÎiÓåè%Ò›|Gª∞Ú:∂ª
d|5◊JóˇÕyévÁJ0ÕÔ9Up¨oî·+ä-:≤≠R„≠*x3
±^ñõëÚ¬±gπGx(°œX€2üîÎﬂ°´ﬂ…ü≠û˜zîUó¯°G0pV¨ysDÌñ:€h°’KÅ B)…æåÃoWÕw”dÒ #páL^ôÌºmøQˆqƒ!≠ú•r∆?àSΩ·ı`Ô;YÂñ
º¬—“ÁÚÈ(ÅçÅ`S #5¶†]+®3™yGcÎÕKçÄ‰È	ß¿X˙Öƒñh˜îg+ñg1∞áøy¡Iylid©XVË‹ì<ÛS˘g±=léı˜«Æ¥§{Ò¸O+∏V>i

ı¿WJﬂ˝&zë¯⁄c´Û¡a=>•9øö<0ñ^JM(*Bo	@)TŒ-+?J4R‡∫°, 1$xE;r#Â!/Æ¢xÑ—çŸê‰®<Ω˜ˇ∞üÆ,étÈ˚ı¢»tºëñ◊mÜ´)PÄœ“‚£éùµ˘";9´¬¯µé¡_q†`-ã¸˛õÔ72xË±ΩäPº……aó£ìly‚ü`á˘g*≈â±ûa¨õÑ‰
Eª¯«/p˝	√RñÄÈ©N≠Õí5jÅï›ı‘‡H‰öâÓÎ^ª[1[Ö‚l‰∂ÅÈ¨1ÖÔBNOâ˚SÑ=L%˘π÷2*‰âc4lÍ˛«òJ⁄Ì>“úïâ6u≠sÏk÷`1˘¨ÎEË|áñëmå¶[c≈˝†‰â'e`˘∞EajDeAÃÊ
¬DlÓå>ÈµM· ˚∏Nà '”æ‘	sﬂr/πßA¨*ËößyD> bC'¸	Ê(&Ê&^ Ÿ
ÇÊbÈ“¸vÜ'@)ƒﬂQ±ëŒÁ ¬πè±E∑ª¶ã˚ô·Ã<˛Æ~TK∂—ıfÉU#Òø'7æï1få±ÑŒœ.LL>.çsÓA“∂á	e◊){É◊ùÊ]Ä¥¥>~xSqá t∫≤Zb	q$⁄^Zæ˝Wë·|-1>‹'F“/[íì$ M°iπ´4*Çl˙ì˚r§l.n˙Üè1Ï„pbÍq,ıkzŸè≤˚A:°0Pë„o∫Iª3RèŸ§U6”0v~‚`™[ﬁ=}©kIz>€u7ˇ≠Pê9_Y∏‚=€n¬e∫°rÛ%Ñ‚≥ÊÀ¥t}Çæ≠√sÁ%:°(Úo°~w\”´ÁNﬁ•Xªõ#±Ùã∫’Â™Wø äq◊F,‘ç9∏Ω∫õGv“ºÍb“	àzπ±¸˝0ñ*ÃGüÿ4`£mΩÁO∫’õ‚(N†h†’ä„˙†=œ˜d˛b·|â¢àk·I„÷<ﬁåbû6oºnÃ¶‹≥ÍwGÇ%J“ªxWG≤FÉoí‚ª¥*>)†ƒç˘óä:ú§ÔÔ≠g®ç'3ÁëÜ‡%î)ı42	±#&ÅwLÍf≥s¡»£[ë3ôòÓ¡•˜Ò˜7Ï8Æ‚(Ãm$LÂ{j#OÕV!m†#-aënvoEâDâçÙyÇHzøº7˙«hÜÅµ9®{ù?º@ŒÙQ˚·T≈£%*˛Hˇ0·€n’ùûﬂYaÊÚ¸üIYôFwˇ´ûo0É¸=•mU
Í@ÑOâ?à,Ïiÿ*<6ÕÒPtÙó?õ4ÈˇLôõÏø·ˆNyˇ
ÁcÚ™Ì[_¶«·¿y∆Çﬁn
V˚GG—dòà˚Ü3]%ÿáÄCäƒmÿPh7ß y$*A-¶#·pá≠¥W≈^L¢ÈWçÑ2†ÁÈl´<Ó@ÉE¸b]œ«Y|!É¨6PÆâRi˜–ªrì∆øt∞%á&W&L£ÌÖjY bìCB/£—0@Çí©§Æ¿§KGnü˚cÓò{ZÒCÄv,æ,L
Ç≤¶ı«“•´y≤ö§6âÇHçÁBê≥Å¯‘¬+G≈qyÊ¿ÊÏVaCîó1öHvÁÚ¯Ø;\π˝‘fZV&©ÍÔwΩâæ˛·$˙#ΩxˇFeAå’4⁄(	‚fπû»Ä2±æåÉè%‚ÕÉ√*¥§~’¯ΩÄ¸X¡k£º7ÿù]ÉF)Kß±=√˚.Œ±âΩ>ì„í\”ÁÌXÂ"∏Ik—Éß…∞UêF î[√`“7π;Kqõqa‰ËjΩô∏[≤Ö’”¬Mﬁ»◊3=◊ÇåñmET‰¶#å˜∑¥™û∆õVnÕ.!ö †F[ß–(Ó{⁄p∞`ÛÊˆ:ª54r1L]£üç·B˝9E{ƒ 'I«ˆO}T™y9ƒ+[G¥üäœ@¥/ˇÎâ c›¸o 3*—	\Íï2◊4Œñ *ÆÀ“‹'≠Â≈S™3ÈdeY•©ÈÊ@‘Ÿ˚$« XxmÎa?ï˛ºôO\O˛T„›VúLEªP´≈˜ˆÇW iáêﬁÒπ*4'∆~÷Óg^ïÒÎ6Z∫+ÏfÒç[´B^üyxi„∑átåüﬂ—ﬂ –,Èq†b±B%ÿÓä§°ë]d∞äŸCÀS
1•˚ïÙsÈÉi`5hµ â.=*◊f´Ì◊eï§™∑|?tQ⁄ƒ7?¬E±¬Å¯(H§„¿ó ÿŒ™fT´2Ë7Dıw£˛Ê£Ñ˛ª„ßwãﬁ5<ÖMÌó–Ü\@ˇ∞eïbı25Àsóë8A⁄ÒdW—6ÒáÄù"Ö &ªLÜ.∆bèÄ∫Øß) sk}¿Ö∞ˆízÚ∫¨y
ÔdÇÛu◊ÙúoÌìÖqdpp061∑\ÒjßiWÁÆ´X⁄˚0v◊ﬂó∆’6àÊÓîƒ”:w)\µœdÇ?Ñä:˝Œk<—ô$@”3Éø7@L#„êˇ•è«6)¿äÚmJ÷(ûGëòÕ(^Qå]±ó; ÜR˜Ë5ùávÎI›òx|⁄›ÉÜ 6‰ #8◊Ñèf57ËÙM“=ìπ»VRÏzrwfû÷R†Xv9„vÉ’ÍùËáÊåw<æ%ﬁıh@ÅÛ=0sL∏´,h>8Å;H/¶6ÒÁ˘`%qW®≈¿£§h¡O‰-◊«“√; c˙0tm0Ó∏ 2!>Ùòoãµ%èn@‰†÷oqyrÍ°N∞ı0}Tﬁ≠0‹$â:Òa˚q $:$hÕˆoR◊»qy)†>ı™∂ÄWã5mANâÈv®F>ﬂßƒû÷Ez˛ñw≠BviµÂ©ŸJ5ùë[¸vˇPzfZÔñî!`„2Ö Éh≥((–âO≥`£,¶îÊq£˚·.À%}Hdl*˛H >ÍΩÂÌÈÂ¯Xf’ª 6|hecƒñÜ!æ÷w≈˛˚¢o˙∞Ÿ}wQQÄ∞µUªÿ˜¯3'ÉÄ⁄î¡Æü‰P“ñî€ΩaGÉ§!©á¥∏Ü÷ \M(6$ÆF‹s3∏Ü.&zæÑœ÷,àÌQ; Ïr™)„BB)«#ÃÊ˙Pf¡‡W>√;m:dnÈä%#,+é[ÑBÇº5?dd–ÌÌ ü%ê>P˝1R5æà≈I#â—3+>xE´(ˆ†;{‰.≈Æ"Q≥¿úÊªØ¨ÿ@}Èû –fQ¥nFÂﬁcS∑Ã^∂«≠ı#Uh*E˛©{ı§≤I˝Ké’ÀÙO°.]Eï™—Öxˇ≤˛“ÿµêló!0GÜ«:R)≥◊J’|∫_≈µ‹Îr?œ¯Nl°*œÔÀ¯πGıg»91^~qœ'‘∏í˙#Ãoy¬/Ùt—∫j á®ä˘ì…>’∏„H∑ãœNêacO7°î^-£≠ës©°M/s0í=≠n4[¿Ë0Ÿ_!âGò•ˆﬁ2t|∏qÁKZ≠◊ êÅÅ√˘ÏPeÙÆ_ˆ†@è˜S2ËSøµ‹ ˇ£ı{Ÿ©ªnäµ·£:∫ób,	CqTGÁãÊ˙;f]ë‡©—JsGÊ%+yæÛF‘DXËl”Ê-;<Òg2◊Ö»E)vÑÙâêáéÁîÆoﬂ'¯ÚâÖÍdÚñÂ√ß€D@ÉgÖéS∑?B˝<◊vUº’Oﬂˇ˜zÆ·œvøyÿ˙¨íZ¬Î"Y®2"ê÷;ÎÁ]X„@úˆ“
ÜU∞,[/pIÃ5sëg	ˆ>˚ZV`¯ πÆƒ)4ü˛ÄX#Ÿ® ß‹M·ÚD∂‹√ÿBv¶8.‹Æ)í8Ò(Äq?≥õ√Hm;ƒ Æì·	ÅÅﬁ‚Ùx¶—G∏˛Èæ%}Ÿ˛Ë’àøiŒ$ÀñïG¢gªyJkVíÌ6o	à·ïÍ¸LÅﬂ’éŸ‹a‡s“?#"¬CÎ$;ÊGôiò°tAö.F@ΩƒŸ¨ÒHt√ÕΩ≈kà0Q{9ïø´ı{µiÔ∑˝Õ2(“POÙp"‰/“µ‰u%\==]A∂∞°Mx¸.èÆ˜Òøîbè‹HÀ÷j9KÑ˛‡7ÒP [Ú€´ﬁGÈ·#jß—5;$|†ÆAA∫∑¢õc^é78]…r«ÎπØfu¶ÈqS’ûRy¬PfU¿‡»†—-ÃqÊ πDj˙ˇI\9W	Ö
¯ÜP¢ÓéÖ»;'íœ∑‘à«á.e•¸E¯ãc÷§°™Y2¡vÏÈHSM°xÕ˝˝êN∂¢ñbUzª¶iaä¥õ»q∏ıdΩ“¶¯z$}–Ü+JdLalä& C†±{~T«{áÙQpæGO·ÈXÇ;≤–ﬂöÛÚ`≤2èi£'¢ﬁ|3å_≈™ÒøBt»˜Ó‹ÎË7O˝·¸éº`S'V_∏7√Õ˙;®Q‹1œœäÊtôQ	l˛ì}πÁhß^,ˆ’Ωï<À5Cx:î`EºVedÈº ΩÛø.˙dsπÓ¬‡ÔÂ'‡œ&UMëSıÜ…sÆ∂Ç.-Ü[ÓmÏ∑öx«v,¨VÈA*¢Gz£O¢#^øb∂6\»>lı≥ù‹x0'ÎN€Ë3Ç&œ},ì/g5—f$∑>|˘Ñ%õŸg›iSöì—DΩ_m€=tDqj	ÿ:^*∏ Öæ˘Ll‘TW›≥Áy†πè´∆˝‚•Ë0j£±Õ‘áŸπef˝´„˛€˝>Ø¥üÏ÷LO¯S£Ù_ﬁÄá˙Ô´ÒÉú¥Lﬂ£€NôoH/‹˝hº≥æª?4ÿƒp√Îiû⁄(Ö∑œ¬åïnoæ¶§\”óNhöè¿ñF(_tAäÅÅÈ©´¿b&,=SâhÌ]ìKX{πd˙M7Ü3)–<•É“êﬂxóä.ËkcÏ†õπ∏≥—◊≥_wí‘¬F≥ÉxL7ö¯Ö£∂˛√ÑÒivéúwß@2—G˙·ˇ|ñî¯ùÎ0Ú¡)}›^ÙÌë˜ëá@I∫RÔn>q’*î˜ÃKá)Ÿ≥¨>ÏQ™¨∂‹q¢ÑrƒZÜDF”l≈–ñÜ√ Œ€NdÁûπ?l|I)∂j›&(Cñá˝	O∫<	LoÊäj˜fπd9MÔËÛ!Á©?o ı»î´€ˇˆÂI∂ÁÎ∑“s¯f<—±ÈŒa÷€—úÉú/pK¸Ë¡zö–∂^°ÃØ)å}ç.{˜h>éõU(≠oMR6Zè8êËí¡Ma»;“Ä2@è≤Kºå˛úQAŸ’Ω¢t&SÃe^ÀT6†VïKÖ^ïÈ^[¢dŒÛösñ‚™∂Òl¶e+≈Tw‹öil	"W±⁄»ÿt¢l’ò+ŒzReQPÂ”π\ƒI`p1›X˘iîœƒ® Âœ¢¨;”‹ZsL(√7Ls'†S0«—*ìÁ‘ê®¯∆ò%!∫ñ˘!Ebnª32‘3˛Æ∂Í•ÙvŸÒz∫_/z.<≠^]ë¶ËFâjM!WMØáEÕØ!æè°t˚¿=Ö˝ÊT˝RA›G\iàK3ŸÍ^]"´2À5"À ®<‚ì@®_í◊ê&‚‘–Äÿ_î˙xù0Œê∂¯;Çw—–⁄£Á◊≥‹ßNi66h¢ódEıÒÅÀ„√•=k)‘ﬂÛû‰|èñÆ∏◊DÚ»åùﬁßﬁU‘RH˛›-z‚%˚ÍÛãü¯ˆAÎe´dÉò˝N.k?üx<‘kN◊"Úõ©Ê& √´"Æ\g»B⁄ÜEﬁVŸXT`uˇ¨@ÚÓ«ˆ†GD
k–∂Íb˘OèÅ§|a6ÕyÇΩƒÙA)3!ÈÇ]Ê.ÏTÛaÑ$à:öÆ3B¨‹v%„gkUI~G™òÖ¨X∑ÚZ—]önhÊ&†ö+B¡Ω	 πª°•∑kr} ›_ye≠ÃìËÃQ[‚©º4}⁄Ê^ÊØ2ëÄíqŸçÜù:4…¿4é)‡πSm„¬zkw?Ù4·|m™ÍmÖtÑÂ¸ƒ„Èô@:ÔèZ·` πÿK$/!˝‘4h=¢‰Â:xe¡ “ÃNÅN^ßˆMOW≥`køÒ!"S»wëP'T4‡6YNØ3Ø˙e°¡*…=X±{aÁÚªÚg†‘'`Ë	Ë÷'mÔåÿ˛VJK·Çπoﬁî∫YcÅì®ññBO†"zÎ=?PπÒ!ym⁄ëì√ìè{ˇûnËíÙø‰ô≥ÍÁÚœë[ê4Áù÷Œ-p9é¢/¡ÓÉÂﬁ£6◊¨C¢ÄÌ ®∑\Ûô»KÄó¬aÆ◊á„R<≤>ô^ˇ-’wkìêéØuÀtÅ–m“Êy.œIÖgì)qEŒπh§´…W%è%0$Çè>æ?ìãdπ.8g≤|XoæúmùŒ˜Èt¯f“x≥µﬁaÃ…u≈îÛ„lS’€tX=-0µªÉÒ¬Ô„àù-@Á∑Ôú›∂ï—c(=s•…8qzÜƒr€&_3ÕÁDvjÍ%J fFAµ2Æ)l!ZkÏdã¢√ï´•~•Û—∆{÷’8â/»"ÅI±¡fGŒÀJm&–÷øÏfCòa^mÃ‰·pS£œ[˜@5˙Kã˘ALïø≠ßVi˚!\µ:∞Û¬l"Í’'ÀÌ
 S‰"%‹èˇ⁄”ÍE⁄3÷øwﬁÍCx
›r%ˇà¬0û°œ‡LI_:(â®™]˙hó…»Hã/Ü~d∂Ln ¬<πWé|
gt;+u$ ÕæØ®¯·Ÿ2Åx,]E£cµ»\ö≠j y‹lp8I%úxML*›≠à¸]ÿiûaßïöY5›1—∞ÆX%UÀ$Û}õ§N÷-≈rÈ´*[πÛj˝oà‘.!zÏâJêÇ…ˆ†¬®⁄Kû¯ªœugA(‰g⁄€ ®=¥êj∂h«j¸0Dı:ã$~d^#˛‹piÈ˚åo#OŸâpr√Gy;nèŸj¨ø#=«N~¿3}I‰ê@îÊ‚\˜¥YxÍπ·≤y"àÖ¿<€¨^Eﬁ ïwéöÑO@À˜≠L®ÎÂ¡u/qTk∂∆˘ˇD[Õÿ™¯ü¡«π^Ü1›9·˝x Ù{zJIn˝õwYÏH˛9®—âÇÑßñ;˘ëu7™u¢˛kåÉ*ï˙v«7∑•õqá¯≈¯– Ô∏T$“èÅ}•qoÂú$Õ≥PŸH√\+u˙P‡J¡ä∞
:íˆéjÄ"ú-p:`¯7 zSπÚ•ª¥-}‚hÙÀZ©BÚ2vßùOÇx:,âJó!'∫0õi˘Ì¥aN≤-lÇ◊ˆ–u.◊]#8$¢Î"_9¸+‚›	ÕÁÏzÓ3∫sy%ËQíävg∫˜KTπ?*âi,µ79‹Ä¿'∏Úz®$…‰AΩ˙ﬂp9øëÍ•Ã1<.òÃF®è}SÑsxü;=‚K?\6/«3∞∏Qz~ÇnúvØ∏øÍøó)áTnìƒÄt‰?Ä∆9“—≈>6Ñ<.Œµî	jædZ`„_ã/U„º¥≥'Èº=BÄr≤`¡Flin[í©·Æ34ôN‡Ù¬ÑEî˘ÛJt;hy!˙¡âä≥y≤˛ú^ÃÑwRê>xîËo,¥ºåÑa«"}´ãéÃ“>ó’“M ∏◊ÊWE6Ò_RV©ﬁ¬Ò?i˚Ùﬁ—óì¨˛s‚Kå¯ª
´af–F§:í`Ã»6*As€êòÔ)â¡ù%ÏêÕ{ƒgææß~+∞=J@ıÇøÇQwè1‚ùK nV#Gúﬁ
*–v–oÁ}Ïú‹ÛÕ@eÖ 6:{Ò“¬fU[oøœ6•Á°øÎ‹$ë(“1ﬂ…√¸zñ‰÷õCtP}ì1ß˘MfbEëB]Ôhãåôﬂ»w	Ù}zLwŒW›E˙‹±6≤9¸–ó√Æpü ÷)∑=û⁄ºÑôI g?À1ù9ê‹∞[VHñ_UzﬁÔ.oÊËE_Iox?±ÖÁÈ†ÿÊ–-Tu™Û"k]zHÚRè[ﬁŸi∑åìQ«u ¶/,˘$ä≈-&·…TPﬁÀ|´ø~€¨óºÕä	'§ùLïwÊ¸G(Ñ£†ÙnΩ`™™„ó?c≥Ä7Ú™ª
˚Guj§[5ºNa±Û”"˚à1%ÔÌ/€àŸLhoUM¥.`«Òï„ÜcàIûIKìÜ ˛:)E™å5_ ¿/ÌÕ›»|ebAW„¬ñ“XH.H≠3≈Ÿ•√àBûÍÒÆ-WpMIÃMˆı”∫\ò»ÂÑ¥oo∂ÂlùOÁâr≥àr˜‹‰©≥	£&z›nπg&ñ9y€óö><ïüÿΩñ#ùˇY≥–uN+ô(¥≥ﬁÒÌ‘ªÇ¡(Ë\\ÏÀ,˜ÙT^¯•∞nøiÃùª1T≥Ç~ÖlÖ˘˙M6§|.∆ák/√©÷då◊FG‹ë≥áŒ,”!	ãjNLœy‚ã›TmÃÂ–§˜£‘TÎ8¯§C¢#£çh{%?cQ‚—Sdç)—©ò8P&¢À]`=ï¬€·¥¥9>∏ñD˝˘vç¨v◊Mºä*ï€èJ3‰◊1Ì∂Ω¸q=€“§¥M9Í?˝‚/K° Pπº¿+º;R`Û´πt‰Ã∂¡Â„Ïa®á{∞ Ø”Ó®ﬂ≈∫Ω7ß€ˇ_‘-vÑ¶&ƒúª◊"ô±ÖÌøœmÑÙµÿ· 6hÚl“ÏÁÄΩWÆHD8b ˜≈ÆH6¿ÅË˜¥A€-ö¥∑˜_†Ë∂æär|jÊ*ÿˆ2u·Wµ÷!ä˛O‘¢+Ωπ
	Õ˛PKÇñÛm~Y'o◊#%öi¢SìËb˝¨·hX◊£NÜÑ¡ë^°5•ë‹«6&ù|∞ÛkΩÀk«>mZÅ;∏_(AÖ≥$k2¥!ÈiÇMÉNØ™¬¡/Ùl/ÑÆ∆X·›‡…n≤&°ö¯JW–úÛê]Sø áèÕ˜∞%Àyp4Ë–o∂Óû5Ö7õVOœÀs4ÈáŒ>¿l¨'©ﬂMA%{t'O#™èºNˇ˛J‹Ü1n(¶≤õŒ »Ωô^¡~˚jj~Z
3WúéÊ ^ïTâ%ëÄóÄ+7‹„c{5≈ùïÑ¥Ω°u˛K|XﬂHdÕh‘√NÔH
j)?¿oàü0QD$Ä÷õµQë=P+vö7¶&ÕkÇG…üÃq[¢L%vœt÷¯Â"yoO¬¿Z¡∑B;\ÃO· vÖ]îpUíQävi∑<„,”R˚ WGÎª£Ô*Wëÿs¥49e0|>óvﬁch‚?Çåììà:=6’Ç#'/ )€Ê˙ÊôØr]± 6˙ö~~N˙í€R∆NM°ﬁ—Éwﬁ-¬ìX(rÜß-0MÂ¶âoÕ}aÂ‚p¿üomì»‘9ÿ‰ÁûÉX6Un'vÜn\¡¿≥´TáÊÀÔ∏Ú0˛MtÕ’¿l?ì›¬‡©ÄF"_j©Í3Æ?Emb€îS† ∑y-ßä:{£®cC=ƒ Ø¶‰Î]ª¿
∑Å≥d√÷ds0◊bDiñ°~äBj…]äÓÆb€ß%6=£ÊhòGÙô´πÔ∂ÂT	¡ÔπÏ•Q|Kà üÕ±¨\Öi[´∆YEb}0Z¨D8…^|95¿Qóñ‡∆…ì‚®ëÆ2ÁSó√{ÿpü¯Ñ˜yln◊Tö÷,ÙAyqÃN•bmÉï’eú…≤;\Á@•ä∞oòQE˛π˚ †√[ñÇ»õ·∫“MPGóâçŒ-æ√›ΩıÙF€-}0#K®õ $éu⁄ÂÃŸ¨Ä ÷oƒ∂¿'’@D°R◊≠≠∞x:TnÂi’´∫Å˛5É´∑x*Ê„
˙óÌˆÓ]‡ú$Ï2ö˘∑&`ˇŒò{#=w'Óñ¸∏ƒÿ‰ëÁ5´.BÕØÁ√=
å%o5¢≤ÛÑ~ãM[WëG3öˆ	]ÍÀ8√∂m∫ÓœAÛπ≤∫ÛÉ0~€CËúÔ}£œÿÍ+•.L‚˜= ˚ûF¯≤x◊	kÛﬁ!ßG¿?…T£q‡«˜∂çQ"âDÕD‰A$@˚‚≠Œ}§]PFQˇ$qcÜVÒƒ*Ûp~6û¸ã-˙¯Ønº0wìÀ	Û x-ÿÃÒﬂŸB0)DLƒDâœù‹ÿ#|›dÙÆÀ&sB|n€®GNÍê'˙ÏÛ“ˆ{ëî}ÒÈ∆ŸX`B⁄óËmîtˇ˘∫+ßú˝É†R∞ÑK1πù±≥∫kª˜â+ï‹¸˘ÃsíîJ]˙ÚÎyîNPø≥7v¡D_ø)iùq¬ﬁ≈Í3Ω–•5Ø¯	√…e:
∂“ø¸MÕif"~TFöùèA¬àíä¢	-´®4$ÍÚ·“]ˇ¶>ù7n#0˜À”{ƒ-æ@´%ïÁ©D¿0¯dërúö!+8ﬂHsëîr/?5ä‹Ô◊^¨ÛÃGπÔVÌ_]’úcRV#˛ô¶Ü|+ìâ∑PÎ-ºKÉ.]Ël∆ˇ\FÉQ√ƒkÕGÛ“≤t˛S–[“·Õr”‹ójth≈˛UÌx÷{ú’'ÄÓ(’,it{Fa;ﬂtîúŸî2√µˇ‘7Å"W“‰ÃÜ+ù∆î∂Q*Ô_¨2x„∫Û∆<ä’{5 7]õÙcß¢q)˘∞Xúh^“Çn∫óáÑò…Ìa∫ı¨◊÷Kmnß…ıà∑√øGu˙Úc‘œäÀ€£Í§™JöM!RüEe–ÈâπÆ;8 `¥‰ux8{–õ∫≤…áç€∞ÌÁò2NÈÔzL≈‚Ç=“g,›2§B%˛≠ZQ¶˘g&ÄBÂ_˚3‡∂Q·‘≠¶∂ŸŸÏÙb√mMá9EÌXx)õÊAº*&”9ÒΩ”äê(l}3˙ú∞∑J˜"πÓYW|7≥Û‚µõ*ô”km⁄âÑÃ‚b‹DSÄ˚·âá4ù„Íp∑´~/Ï‚|¶æc:≥˜<wÜq¶æg∞-Mà¯+é|‹I∏ÇÄY¡‘¿§BæÏ˛GM¥o»ÄNÇGl}ÉÊ^1⁄4ù≤åó∂pyÛ–}W∂îŒñªJ™xÂì˝ ´6Z≠r·ŸÇy∆Ù˛>i√≠O(CµÛ˚±ËùÖè§ÿ’ºNr£Oúß>≤iÀø“sº¥ùßÒR™2™Hk+GiøZó¡Ojé˘Çˇ.74¯*Ï√çB˛c˛∫Ω…[ìê	_πßóJ(”Õõ*æÊSê„L¬IÿÒ©:‹ÈlqÀA∂CiQÆ’ IF=ô(§Û˜≈+}∆Ì^nhw‹[PÜ8π∆“Ã=6¿ﬁ€∏·¶¸9K‡pâ∫gπ¶êRı˝“Ü>#¨^!äËrÄdµéxìû|ª†H§»¸»q2ªÇ@&Ú1´WI⁄=~ÍªKHpIÆ˜≥ó»ˇ∂«8<—ù˛Êl~’"J|*Öf˚÷∏`Pl¨Vµ+¶∏Ïkó¥õl^jC;ˇª}ˇb»X(Ößâ–¡¢óÅ„ áxÒ8«Ë‘Z‡‹ÍH`∑ÔJÌwØﬂB≤â;∂G8[,¬jL;@gÒÌ¥≈%!á>Q¢¶Ú®˚gB.Lä¢Ωc≠&∫ô ~‘Í,˙_!=®0¡¨◊?©˚Ÿ\
r∫;¥7"éÉ3¸Z¥î“‘ô †àª˝˝ñó v "πOÚÂ[„KÚ ±X4‹4“/:ÔÍ"›¬LÌbÉ‚@>É˚s0“â’K&ÕâVﬂ
'*}‰3©ñWJõŸ§™)P,⁄ZÖ^/¥ åôÅ\QI∆ûj√BŸNäÓüÂLÇ∞á:å„L˙z eITü¬œò0OÉ¬O1û∞~hRÃ’e 0+üh±swà-ÊP[ç˘´(≥≈ò‚Øú@hRÇ–÷W/T´N&ËOdy«Ç√E_ ——ËÖxê$Ÿ#Î…àﬁ]	U∞"ﬂ3∞ß¶Í≠n™üNá_æ‡C¶b(ƒ¯©÷Û∑ukÆ≠"∫öã•û‚o⁄é'6 oÓ•UWAL™k∞2ÊÃKW‚FKÚñ'˚s¡=@m’·˙ªŒ‹-µn˜ıÒìtÛû„T∏µ#»o¶∏œy!◊u üå»∑îc€LÒÏxYÓ*e“Lp†˙U9nv™ÿ†U3.”bèÜ˜åãq˚oÓüπ‰¥®g›ô¿…©îF ó3yRG≥pπz$ñØ£–PF±£VûcèpÆŸr†ˇj≈Àéq–ÖÖU≠›>9à"V/-%ÈÆå6#ıù¡◊÷ñtŒGÍw<sÊ”¯ãôür¸R∆å+˜K¢ñ–®∑N•êØÍ‰F:ƒ»0@yπìr›øIµ∫N·p»ìùy|D.ÖÑDìuäËÅIIáﬁcTUriÓæSh¢‚Ωì>«2í.ûÉãÔîÒdç°
Æ7«äraA[nVbÇv $WóQ’˜®&´/†xcÁŒLÎ)©IÒÈÕ…⁄a8¨πèXıãC:«@ùÒàL=û‹Ω^»ÁC”>XëËœA	vÅtknÑ…=§hSeé1VÛÿõ2»ob◊òÛTäñúùZìmçvúø&	 @ÀÚX#/tZüiE é$a!O¨xîxŸ$„ù\a jÉKﬂLÀØR®•#€9≠`ûDòF<J*=XSÖß°9∏D7m…’±Ä£äm*ﬂæÚîÖ§fò¡ÄdÔË äip_ZñTç◊®æ˚˙#‚€ÄËÛ?zÏ)dú…r∆©Úˆ%¥±rU#˘^Ω»îñ‰¸π˝÷Œ∏±≠/˘Z¢ÈëyöatïÌ]çﬁj¶’∑∆iL
˜ÑÙL%‚CS(é;æŒï≤∑!é |Êu(¨Ìd‡„7˛`˙Æt°ˇ«°˝¨ 4ÕXoùEpÑëùjub£¢NRç}v|ØBV¯3Í£b/jT”VhCÇ
ÙØu÷•ÌÊ7p…T âS˝e⁄9QßFPD„B∫:®‘›l{ëú9Ü(£&eEÌáL˝ÔÜ’
íØΩôw^$Œm…Èöpl¸$0§∫	®®ù„4éä÷˝ÖçïÒ‚QI≈ˆô§ê˙’#⁄’˜§Œ·d v{óvkéØÎ5˝2æ≥Ÿ(ÉnáÑÅœêVçtbûÃ>Ñ[v◊]ud˙‚¡ÍJ≤ê¯äÛ4¥˜ÃAj§^∑ÌºU/zªëvd[“@5…É∆≈ûóéÃŒ≈)´´%Ì®ùr≠√ûÚäw+ì©…˚öŸfe¶[@}ıî(ÿU¨g1Ä1$íIñh EØ∏–⁄"#®|æ“æ>XzQLª¸OjR›˘Ö2õ1ã"-:3BÛPÜÌ`∂î÷‚œ:*ñ–Í∏AÉ˛ÛØjñ+¬"¬‹Ç]˜Boèc%˝0WxsßùÅÙd””íÿA›∑RH24&Ùí“Î≈xm‹_z—À8|÷ÍœTæ'vx2=]uõ=7ˇJhãx˜]s4äÖWÓb∞"a‘)íTßï°°Ù_∂¥Ø≈ /;	PÔpÕ€ÔXâÈoˆT —¶F˙Êæ≈î=Pò∑Ω¸q¨Ç)¨JùœÜ[ÆTaÁ·¬˜≤©]^ì¿í∫‹á¡'JÂí•O¯◊7T˜EˆãÆÚ?†ÂS$”Ìpö∏ *5 ØêOI§∆Õ2ÔãWõ)›]∂Ô4öÿu5#ˆ‘˝úR®ˇ§¸µÚ’õ÷SxıÚõîûŸ®kòm±
ws®;QjV# ;ëbŸá~ ∏€„µà9€⁄Íë¡∂*,%a!GÕÄª1c:uÇÄ-f'+Ú}°r†˚¬›†u1‚T/X	ø¶JHÑ,Âí∫ı˚÷—õ÷ıª”beÛ◊Ô∆}BŸÊì˝„ú,‹'T—è≠t^Ó’ÎïôúhT})≈Ù«‹µòﬂ=ÊæcPÕúÊŸ˛Ó«5√í‡Ωó:@.ƒoÛ◊Û≈‚ü≥tµuQ¿x}Wºw¶!jl√˝∞ê¸Œ?^M£ﬂÒ°ÜAFˆı(ç^‘X—vP¸n™úWkµé˙Of 2ÀªBû≈L (/:;˛[¨vzïnä°‰”ßÆF#]û/m¬±¥K±ñm∞_cÁ5ßEIGÇõw≤Ë@óI2áH'z˘ÛñGã‡Ö –qÆ¿å!¿˚»¥ ÜzÔˇ ⁄[w‘5#±][eˇ›Œ\Y∞dï.Ö9+öäÂÇ∫∑sû±cç≥Ü1À’‡˝~Å=	ˆ≠4{á#[#Ÿ∑sπ†NS≤D§ˆ"€q°öÜËÄ*ˆ‹Äò»’√4
Úx˙ÌâõíQ4úB29˝˙À-÷ûí˙â~´^Ûﬁ¥`’ÒŸõÔUå˜Ö”4tÇ,%	#¯∑ˇùˆ˜7⁄‹l?ÆºF¬X5&∫¡
Ÿlã8rw≈◊%$™˘3&Dø+ñ≠*˝◊Iâl(“L:ô“Ø¢º¨¨Òœ[È9qM·˛ŒUÒÂ”V¬“Yƒ¶Èå˝KÕßî'4”á(%À¥ülp≤›)ﬁ¯⁄¶ú…njôæ_zÙ°#ÄR‹|s°§T\|f£u©í¿˜nô±Œg’ﬁá∞Õ=∞=îû{o¡©ÂÁU‰»Ëø
NÏÉƒaÏ$ÎT"äS˚œmÓ`z?∞gwX=4a\Oâ·%üô]wL?ÚG—èÃ˛mò›*Jyh»	?î}Ißª}\ÅW≠r¬úπ˘∆¬∏iQÇ˜_RåœZ[Pm£V	÷¬%+‹é¸ouù2Û[2q·^ñ4]ÀD"‰VXßP°Jå‘·ºr¸˚kzı÷Ó*¯®N∫X]Mœà~∂µ∏m‹ŒáØQ¡	«:‘õ∂7øjëïMYM9ô”`t$∏å’“ıë ‰«·¯=√9	âO+ ˝Õnwshõh\ G«Îv1®ëy—Œkj˘_¿&Œö∆˛∫««ﬁ!ΩL…5$^’ÿZ∞-ó∑∑≥·æ)BeK“~¸l*cOBëÅ∫1	˘üZD:1bE
ııA!"]™›q Ò$Œ∂•Œπ*ûã:V4xé¶`LÉ≠∞ßmH’ågoÇh≥ê≠∂(g]•S˘5Øè≠®PœÚü¡ô5„˝‘ˆ'	¸®—ﬂhÚû'$ÅÊ¯ÈAáM‚^∏Ñênﬁ®<‘Ï,d+ÜÆ)fUÇØ^Ê◊~÷ÎMYLÛèéUØã¡'ΩG˚}ao›¥–XeëÂ≠ó]ëêî‰**7R•cPÅ¥Är≈†å+[2ì9fZHóí	ﬁ…é\ã_¬e¯tûÉÔ_ë~°Ωt†ê€◊|¡í'ß£	ñØÄ¨Ø!GÌvº)ºù{›Ãv	iiú[⁄π7∞Lz≈BllÜÇ≈≥të6XÜO§ È—˝Ÿ´QêÈÊyt˚{ç˛§òÖ⁄ „~K¢˝∂˝◊\Ã∫òBÌ÷v©ÕÖ¬(«F7FºW9e˘#¨÷˝te)o4í˛ÒdºÇGê{∑ÑánënnbtËbJﬁu'^óÅÁ˛ ç6ÏWxﬁÿ-v ÈqÑàúägìÙÊÃjûÒv0≤ïÇ^’°Ä4é‚h›ÑEÉÀU◊™_ƒŒ∂•µ‚õ⁄CdY∫§\'ìí»\ùcî,˝ˆöèÈíƒ <[VCPÚje7ÃNMŸÉÆ@ï.…zﬂ¥X∫¯1iø&úÆJ‡ÑõjHUÁ–ß∑n(ﬁ&VK∑óT2Û$UäË_÷™#ÛÕí©ü#IÉÏ>{ÁM'mR-ûb»°gøi“ÉP`ﬁ-†ƒíö)F7E¢X¿Îu7rÏ†x‰®n~ˇ "Y4úæ/x«œùd5è˚l™_§’ç‡ﬁ/@€˘bAB_Øà–êlÉbö[j˚¶7*ˇI⁄Öú∑¸ºƒvÑÓ‚‡gg’:9·ºá"†¿ó[çØ∂»oëk≤2ÍN‹B5¥J¨≈0FyÕVÜ‹Z≤‚îÈ|tc çR;g1ôÄœ’È0“2 "ΩN$$*A∫o´)H¢{√V¬ö$°ª—n>…©IV£á‹da7?8≤ìà<mU:V©œ¯7jQÓãç◊£Óê>fÔ%©ù+Çú<ŸYUì6ò®O˚€ÁKiπB;=?µÓæÀ¥`ﬂÏ∂_Bf‰@™r,iuteg_Xê‡©W9ÇÁ∑`π1]õBÕk	¶n ∏…!Êõÿ&	¶éó∏)ΩÃD≠t^ô¶æπ◊-~˛•MHπQïΩ‘ŒôM_äEËA‘◊·ÕdH ÉØzÕ»'`¥ÄÕ¯]˛ÊÅÈ∑’∆≥fOˆF8oMs^± ßÔ§ìÔ™˛kú˜€‚∏Ô¬¨’vS„;
 nY√x©õ<{Ô£]ef;d'N“Cø¯u«⁄∂}Ã3„CgØÖD˘¯ó.Aﬂ6¶ù1{Ê≈WGfv˘qiù˜Öá[£Ω[Sep;wÈÓD∂>∂$îÆ¯ÆeACOZ…iÕ(¢ƒq:6&ìú\™Î-’üÉÃ≤Y.©·˛NﬁœÖ!~≤ç¶íãt±’ßáDÇ0¶¸?ã¯¿gÊYÅøX@åΩsÑú™‚ó_›/√T–N≤÷yé$éIºŸ∆©ù…
áï5ç’ıpgö_ãpÃπ3N9¯n$O±k˙%	:e√â§¡LW˝C¢@Ù wÃ5! ˙è≥èV∏®L⁄£“)•≠K•ÒyWªåzŒ◊£]U√˚)?"a[»TãråLOø°’5
TÂrèÎ=4∫Hëfk™Ã∆ı“/∂/"\∑Um0ˇ—∞√T≠¨Ñ≠|OÅÈO2¨s‚z÷z_/˚èÅï»1 Ì⁄gJÏQÎva6}YæÙºÛy–™E¬˙s‰Ëm÷Sy4nÉ¨ÈG≤~ÚüØuòÂó´”Q<√Ù‹ ›´Ô|‚≤y„}óW±®‡ÿ”Ù¥6Òœ®≠y€˘%
Oè%æ4‚-‚ÎäõΩı5®FAöM® ˘„∏_õry!Æ‹÷§“ÄynˇMGÈi∞+ˆû µ-∏p;‘†LôíAy_èòE’q£Åëˇäπqb˙ÚŸâãl!9g >$ê,∞Ω@Ÿ|GôêòïP$*@œR[˜ªHUe	›3ê{"R£ÿÅ&R~ãóÕ©ßjs¶Á˘+(¶gb⁄øñ©0æ—B‚U4”]9tPÑ∫⁄„HüÔ|â\&-B;˝CH®w_¢ıéx	òÓ≥.6¶®]ç__%˘∆ı´m4[º”mÏàHrr¢XÚ˜Q¬X#⁄†Ö’)rÅLm∑ÀW÷Ã1M… ∑~@…ÚE‡mkGKó◊ç¶mhE°–o:a’{õˆSbP:π•Å‡`º“%®ôGÀÓ:PÆŒÎMä∏ÙHµô(Ü÷ÄıûmS*fb#b.\”åî•´O‹ì∞≠yG/¸	vRÅ¿eQEê)Û…£d}C¸k”ˆ∫)çvûîhù“ ¨Ø©¥±©∫w∏Ì*•§#¡+]x6ÒP√É`¬^Ï= ÊÄõÒqè∂"a»èkãøbb5ﬁ5Èq≥ø”XG}ÔÃÍœ¯AC≠íÕxπó˚Ñ+•¨÷TÉWOÏË…ôMı„(ªfﬁO◊óe=$O´My
√∞Rmj–±∆Õïô≈ 1s`ñG¢÷s∫Œß;¥≤83'Üì‰m®‡Zh›˛d‹€Õ"ÉÊΩ‚ı≤…v™?vKS0Ú‹˛úçøø:r•¿Gëc%Xﬁ1£ûë!Ërƒ?ÂPpò∏ÙË√Kü@◊+úæ™ÊR…¨«PΩÁQ
7—Q˝Ω&"t%˘/3…g—ôgÃº@∆oTìŒv^àôhÕ ©CéÎ¿ ç§˙œ˘ZêÇ„Hûlˆ!B¥Û Äß—≥ˆ`ÿzˆø»˘™˙%•∞}∞g˛ÜÃZ:ßèI!X.Rs≤[»YDÜ7e62™JWñÆÙnI‚ƒápâÔ
éFl˛De‡à⁄z
≤#æ8]ªNÄö¨›ÛπÕ¯lAvJBÉ^dJPµ¸»K_∏eMˆˆπH∆Â¢fº† gßò÷ıÛß}Ä^æœîµ&K˚‡õ#Û„ı› ≈[¥ér√”…∫’ÆöV‰Í√ÖÎcIÁÁŸ-)ÑıU÷Ëû}ê6ÊÅº®oˇ∆gTd¶<íGÀÜæ^–J‘≈_Å™H“≈mß´B•@C@∞©[√Æ‰Ã≥t—sFÖ≠<¶]ƒp ;:25Ô-Ü!ì î∂°∏5I‚øÒSÎêG¬=B”ÁlV…lQK”
1PG@pwêãˇW“QUË√Ce65a|óäìM·á^K)[√˛0ë⁄(Îı11€N5™»®∆ÕHÁÏ≥J Å£ø	‘^&'$di¨@¶siB∫ˇ”Çõ'H≤q
H;ÃcxûÌ˙7ãdw.Î÷+œ_Hæ-òÉ‹‚õ$|Òg∞µ`˘V»„Ó˙._∫QYU…â?†ü∑C)KuUÔh8Ãe0ºÇdÈ˜êı§o≤ıfp◊Éß$ o≈BÄLËˇG™ﬁŒ°oBOwI˝ñÉJ5 ≤v≠Î∞úIa€Ø|l¢¯∫˛p±WÛÆq™+∏VK Cıœ{6»*Ö-/smRòÓºíÊ¿kõ-B∏–n{5q‚å~/4QaÓ}ô≠;kÜ=‚<âÜ≠i2è5Ø-¡∑¥ÅGú q≤ƒëıWè“yôÛ˘y‘>$3nÖƒ”æ/∞œë¸˚Ceî ı7®¸Rpm∏∫ıÄ˛êœdÔÉÃ∆¶«—˜˛™ﬂ™x“^PÖÏvÖ¬{››ƒwº^»±sí£Fo?"Ü]fÕ˜ ¸_MŒF˝∂‰_ò∫%‘˘ëñ“z¿%cW√ Ö3#f«ˇd¥ßïgU±˙bsRŒ∆d¥ëß%∑Å“uüïÔ=â¡µgÀ¥…hcÇ…güE˛S˘˝˝∞Ñ0±Í¶bsu≥_ªÁyıw®,:mÙhúeˇgÍX]#k pì∫Ùﬁ⁄¿ö§u]
ÀÿÛCÑqÚéπ'⁄¶D‡<¡ÍGæ¿Ø⁄í´¨‡Ü[cül≥GπÃ%sW;Á‹AüêëÒjA4’–UØ¯ä{KmCUˇ ¡Ÿâ.SÊè·ÿrLiºss"‰b•9@ß≠\åzçsÊ?;…íiÌüØà¡s§T!æp≈Q◊QØß#[‚¢‡£eÇ÷‰;[Ò‹ùÍò«ñ5õ·Ö7˚—DÎ\œHIS¶∂ÒNØ/.Ò™Â›âö|JÉÈ‚0⁄À6MQd"W}≠CEì0Là‰≈Î¯¶2ÈÈlú±9·ΩéÎ¡‡Kıuãùôµæ˚sû!„çµÊ¿∫ÙâLq¡øë!!¸Øf˜~£Ô¥∞<ñ∫uΩpO∆ÿGÿ«g7uZˆ∏wM,›#nñáµ&V:¬yÚ†ßí√ <\ô’&P)¶Í€{&ÄªÃfIùmÏrπZfÀŒtÚaè[˜a.°\ÒÃ◊2ˆØû_¿"P% \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/kallsyms.h \
    $(wildcard include/config/KALLSYMS_ALL) \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/STACK_GROWSUP) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/percpu-refcount.h \
  include/linux/bit_spinlock.h \
  include/linux/shrinker.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generatELF                            4     (               Ë¸ˇˇˇSãX\âÿË¸ˇˇˇçÉ¿   Ë¸ˇˇˇ1¿[√êË¸ˇˇˇUW∂˘Vç4Sª   ÉÏã®î   â$â»f¡ËâT$	âL$â˘æâËâÚË¸ˇˇˇÖ¿tÉÎÑ0   â˘âÚâËË¸ˇˇˇÖ¿uËÉƒ[^_]√ç¥&    ç¥&    Ë¸ˇˇˇUWVâ∆Sª   ÉÏãÄ0  ãé4  ãh|ãÜ8  ∑I|ã@|â,$∑¯)˚ø Ä  9˚Oﬂø ÄˇˇØŸ¡˚f9¯G«ãæî   ∑¿Ø¡¡¯âD$1¿ÖÌï¿	–Ö|   âÿΩ   ∂€ÄÃfâD$âŸ∫   â¯Ë¸ˇˇˇÖ¿tÉÌuÈÈÆ   ç∂    ãT$ãÆî   ª   â–∂˙ÄÃfâD$â˘∫   âËË¸ˇˇˇÖ¿t
ÉÎuÈÈê   ã$Ö¿tAÉƒ[^_]√ê∂Ü@  â\$É»¿∂Ë∏   â√âÈ∫*   â¯Ë¸ˇˇˇÖ¿tFÉÎuÈÈT   çt& ∂æ@  ãÆî   ª   â˘∫*   âËË¸ˇˇˇÖ¿tõÉÎuÈÈw   ç¥&    çv ã\$ãæî   ÈˇˇˇêË¸ˇˇˇVâ∆SÉ˙áÃ   àê@  ãÄ0  Ë¸ˇˇˇâ√Ö¿t1€âÿ[^√ãÜ4  Ë¸ˇˇˇÖ¿tÍâ∫   ËE˛ˇˇâÿ[^√Ë¸ˇˇˇ1“Ë4˛ˇˇ1¿√êË¸ˇˇˇUWV1ˆSâ√ã@XÖ¿t∂0ãCãPãRË¸ˇˇˇ%   =   ÑË   ª˚ˇˇˇâÿ[^_]√ç¥&    ç¥&    êË¸ˇˇˇUWVSÉÏãP(ãpÅ˙		ò t^vLÅ˙
	ò ulÉx|ãn‘ª   ˇÉ«#Éx|¿0¿Ö  â$πÖ   â˙âËË¸ˇˇˇÖ¿t0ÉÎuÈÈâ  ç¥&    ÅÍ	ò ∏ÍˇˇˇÉ˙wçÜ@ˇˇˇ1“Ë[˝ˇˇ1¿Éƒ[^_]√êÉƒ∏Íˇˇˇ[^_]√                       `                      *                                                                                                               ê      P  `  ∞      6%s: Input: %d
 3%s: Invalid input %d.
 wm8775 Ë¸ˇˇˇVçppSâ√∂Ä@  PVh    Ë¸ˇˇˇçÉ¿   âÚË¸ˇˇˇ1¿Éƒ[^√ˇt$∑D$PãD$É¿pPh    Ë¸ˇˇˇÉƒÈv   çFpã\$jUPh    Ë¸ˇˇˇãæî   ÉƒÈ   É∆pjWVh    Ë¸ˇˇˇÉƒÈs  jøD$PçFpPh    Ë¸ˇˇˇÉƒÈl  jøD$PçFpPh    Ë¸ˇˇˇÉƒÈ4  É∆pRªÍˇˇˇVh   Ë¸ˇˇˇÉƒÈ'  ãS∑Cçä  Qç QPˇ≤   ãCTˇ0h0   Ë¸ˇˇˇçCπ¿  ∫D  Ë¸ˇˇˇâ«ÉƒÖ¿u
ªÙˇˇˇÈñ  â⁄π@   çØ¿   Ë¸ˇˇˇ1…∫   âËj ∆á@  Ë¸ˇˇˇπ		ò ∫   âËj j j jj jj j Ë¸ˇˇˇπ	ò ∫   Éƒ$âá0  âËj h œ  j hê  j hˇˇ  j j Ë¸ˇˇˇπ	ò ∫   Éƒ âá4  âËj h Ä  j hê  j hˇˇ  j j Ë¸ˇˇˇπ
	ò ∫   Éƒ âá8  âËj jj jj jj j Ë¸ˇˇˇãü¸   âolâá<  Éƒ Ö€tâËË¸ˇˇˇÈñ  1…∫   â¯Ë   1…∫   â¯Ë   π!   ∫   â¯Ë   π  ∫   â¯Ë   â¯1…∫   Ë   âÑ¿Öç   â¯π‘  ∫   Ë   â¯π‘  ∫   Ë   â¯πø  ∫   Ë   â¯πÖ  ∫   Ë   â¯π¢   ∫   Ë   â¯π   ∫   Ë   â¯πz   ∫   Ë   π  ∫   â¯Ë   Èñ  â¯πª  ∫   Ë   ãá<  ∫   Éx|â¯…0…Å¡Ö  Ë   â¯π¢   ∫   Ë   â¯π   ∫   Ë   â¯π˚   ∫   Ë   ∫   â¯Ëå   Èñ  ÉÓPjˇt$Vh    Ë¸ˇˇˇ1¿ÉƒÈ3  3%s: I2C: cannot write %03x to register R%d
   6%s %d-%04x: chip found @ 0x%02x (%s)
 Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇ          wm8775                                                          ¿   Ä   `                                                              P                                                                                                              ∞  license=GPL author=Ulf Eklund, Hans Verkuil description=wm8775 driver  GCC: (GNU) 11.2.0             GNU  ¿       ¿                                  Òˇ                            
                                       	        4    	               *       b     >   4   $    	 W   ê   o    h   X   x    	 ~      P     è   –       	 •   P       ∏   `  A     ≈   Ï   °   	 ◊   @         ‚           Ú   ∞  ù        ç      	                          &      Ä     4      
     G      0                   Q  ¿   0     a  Ä   ,     r  `        É           ö           ∞  ,        À             ÷             Ù                                       0             J             [             t             Å             ñ             ≥             ≈             ”             Á           Û                   
           0      wm8775.c wm8775_remove wm8775_log_status wm8775_write.isra.0 wm8775_write.isra.0.cold wm8775_set_audio wm8775_set_audio.cold wm8775_s_routing wm8775_s_routing.cold wm8775_s_frequency wm8775_probe wm8775_probe.cold wm8775_ops wm8775_ctrl_ops wm8775_s_ctrl wm8775_s_ctrl.cold wm8775_driver_init wm8775_driver wm8775_driver_exit wm8775_id wm8775_core_ops wm8775_tuner_ops wm8775_audio_ops __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free _printk v4l2_ctrl_handler_log_status i2c_smbus_write_byte_data v4l2_ctrl_g_ctrl __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__wm8775_id_device_table     "     #     $  !   "  Z   '  r   '  ë   "     '  ^  '  ¢  '  ‘  '    "    (  7  (  Q  "  a  "  Å  )  ±  "  ˇ  '  g     .    l    ∞    ‚        ë                       h                                                       "          %  (   &  G     L   %  d     i   %  É     à   %  °     ¶   %  ø     ƒ   %  €     ‡   %        %  "  *  <    G  +  ^  ,  h      -  â    ≤  -  º    Â  -  Ô      -  ,  $  ?    M    ^    o    }    ò    ©    ∫    À    ‹    Ì    ˛        %    E    V    g    x    Ñ    ò    ù  %  T     w     ê     Æ     Ã     Ë     5    1        â    ß       "          .     /          1  @     D     H     h     à     ¿     ¯      .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .comment .note.GNU-stack .note.gnu.property                                                         @   M                    	   @       î  ÿ               )             †  Ä                   %   	   @       l                  /                                   8                $                  4   	   @       å  H               E      2       D  1                 X             u  ´                 T   	   @       ‘  ¯     	         g      2          X                 z             x                    v   	   @       Ã                  â             å  
                  Ö   	   @       Ï                 ò             †  ¸                   î   	   @       ¸  8               †             ú	  F                  ©      0       ‚	                   ≤              ı	                     ¬             ¯	  (                                 
  @     "         	              `  3                               4  ’                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ed/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/overflow.h \
  include/linux/sizes.h \
  include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/rhashtable-types.h \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/ae/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/rhashtable-types.h \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/ACPI) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/ioport.h \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/irqhandler.h \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/fwnode.h \
  include/linux/resource_ext.h \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  include/linux/ctype.h \
  arch/x86/include/asm/acenv.h \
  include/acpi/acnames.h \
  include/acpi/actypes.h \
  include/acpi/acexcep.h \
  include/acpi/actbl.h \
  include/acpi/actbl1.h \
  include/acpi/actbl2.h \
  include/acpi/actbl3.h \
  include/acpi/acrestyp.h \
  include/acpi/platform/acenvex.h \
  include/acpi/platform/aclinuxex.h \
  include/acpi/platform/acgccex.h \
  include/acpi/acoutput.h \
  include/acpi/acpiosxf.h \
  include/acpi/acpixf.h \
  include/acpi/acconfig.h \
  include/acpi/acbuffer.h \
  include/linux/dynamic_debug.h \
  include/acpi/acpi_bus.h \
    $(wildcard include/config/X86_ANDROID_TABLETS) \
    $(wildcard include/config/ACPI_SYSTEM_POWER_STATES_SUPPORT) \
    $(wildcard include/config/ACPI_SLEEP) \
  include/acpi/acpi_drivers.h \
    $(wildcard include/config/ACPI_DOCK) \
  include/acpi/acpi_numa.h \
    $(wildcard include/config/ACPI_HMAT) \
  include/acpi/acpi_io.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/acpi.h \
    $(wildcard include/config/ACPI_APEI) \
  include/acpi/pdc_intel.h \
  arch/x86/include/asm/numa.h \
    $(wildcard include/config/NUMA_EMU) \
  arch/x86/include/asm/numa_32.h \
  include/linux/regulator/consumer.h \
    $(wildcard include/config/REGULATOR) \
  include/linux/suspend.h \
    $(wildcard include/config/VT) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/HIBERNATION_SNAPSHOT_DEV) \
    $(wildcard include/config/PM_SLEEP_DEBUG) \
    $(wildcard include/config/PM_AUTOSLEEP) \
  include/linux/swap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
    $(wildcard include/config/CGROUP_WRITEBACK) \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/list_lru.h \
  include/linux/shrinker.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/page_counter.h \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/sizes.h \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bvec.h \
  include/linux/highmem.h \
  include/linux/cacheflush.h \
  arch/x86/include/asm/cacheflush.h \
  include/asm-generic/cacheflush.h \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  include/linux/pagemap.h \
  include/linux/hugetlb_inline.h \
  include/uapi/linux/mempolicy.h \
  include/linux/freezer.h \
  include/uapi/linux/i2c.h \
  include/media/i2c/tvaudio.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
  include/media/media-devnode.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/v4l2-subdev.h \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/tvaudio.o: $(deps_drivers/media/i2c/tvaudio.o)

$(deps_drivers/media/i2c/tvaudio.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/dma-direction.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/fwnode.h \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
  include/linux/netdev_features.h \
  include/linux/sched/clock.h \
    $(wildcard include/config/HAVE_UNSTABLE_SCHED_CLOCK) \
  include/net/flow_dissector.h \
  include/uapi/linux/if_ether.h \
  include/linux/splice.h \
  include/linux/pipe_fs_i.h \
  include/uapi/linux/if_packet.h \
  include/net/flow.h \
  include/net/page_pool.h \
    $(wildcard include/config/PAGE_POOL_STATS) \
  include/linux/ptr_ring.h \
  include/linux/netfilter/nf_conntrack_common.h \
  include/uapi/linux/netfilter/nf_conntrack_common.h \
  include/linux/set_memory.h \
    $(wildcard include/config/ARCH_HAS_SET_MEMORY) \
    $(wildcard include/config/ARCH_HAS_SET_DIRECT_MAP) \
  arch/x86/include/asm/set_memory.h \
  include/asm-generic/set_memory.h \
  include/linux/if_vlan.h \
    $(wildcard include/config/VLAN_8021Q) \
    $(wildcard include/config/NET_POLL_CONTROLLER) \
  include/linux/netdevice.h \
    $(wildcard include/config/DCB) \
    $(wildcard include/config/HYPERV_NET) \
    $(wildcard include/config/WLAN) \
    $(wildcard include/config/AX25) \
    $(wildcard include/config/MAC80211_MESH) \
    $(wildcard include/config/NET_IPIP) \
    $(wildcard include/config/NET_IPGRE) \
    $(wildcard include/config/IPV6_SIT) \
    $(wildcard include/config/IPV6_TUNNEL) \
    $(wildcard include/config/RPS) \
    $(wildcard include/config/NETPOLL) \
    $(wildcard include/config/BQL) \
    $(wildcard include/config/RFS_ACCEL) \
    $(wildcard include/config/Fit(void)
{
	int i, j;
	int firsterr = 0;
	static struct lock_torture_ops *torture_ops[] = {
		&lock_busted_ops,
		&spin_lock_ops, &spin_lock_irq_ops,
		&rw_lock_ops, &rw_lock_irq_ops,
		&mutex_lock_ops,
		&ww_mutex_lock_ops,
#ifdef CONFIG_RT_MUTEXES
		&rtmutex_lock_ops,
#endif
		&rwsem_lock_ops,
		&percpu_rwsem_lock_ops,
	};

	if (!torture_init_begin(torture_type, verbose))
		return -EBUSY;

	/* Process args and tell the world that the torturer is on the job. */
	for (i = 0; i < ARRAY_SIZE(torture_ops); i++) {
		cxt.cur_ops = torture_ops[i];
		if (strcmp(torture_type, cxt.cur_ops->name) == 0)
			break;
	}
	if (i == ARRAY_SIZE(torture_ops)) {
		pr_alert("lock-torture: invalid torture type: \"%s\"\n",
			 torture_type);
		pr_alert("lock-torture types:");
		for (i = 0; i < ARRAY_SIZE(torture_ops); i++)
			pr_alert(" %s", torture_ops[i]->name);
		pr_alert("\n");
		firsterr = -EINVAL;
		goto unwind;
	}

	if (nwriters_stress == 0 &&
	    (!cxt.cur_ops->readlock || nreaders_stress == 0)) {
		pr_alert("lock-torture: must run at least one locking thread\n");
		firsterr = -EINVAL;
		goto unwind;
	}

	if (nwriters_stress >= 0)
		cxt.nrealwriters_stress = nwriters_stress;
	else
		cxt.nrealwriters_stress = 2 * num_online_cpus();

	if (cxt.cur_ops->init) {
		cxt.cur_ops->init();
		cxt.init_called = true;
	}

#ifdef CONFIG_DEBUG_MUTEXES
	if (str_has_prefix(torture_type, "mutex"))
		cxt.debug_lock = true;
#endif
#ifdef CONFIG_DEBUG_RT_MUTEXES
	if (str_has_prefix(torture_type, "rtmutex"))
		cxt.debug_lock = true;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
	if ((str_has_prefix(torture_type, "spin")) ||
	    (str_has_prefix(torture_type, "rw_lock")))
		cxt.debug_lock = true;
#endif

	/* Initialize the statistics so that each run gets its own numbers. */
	if (nwriters_stress) {
		lock_is_write_held = false;
		cxt.lwsa = kmalloc_array(cxt.nrealwriters_stress,
					 sizeof(*cxt.lwsa),
					 GFP_KERNEL);
		if (cxt.lwsa == NULL) {
			VERBOSE_TOROUT_STRING("cxt.lwsa: Out of memory");
			firsterr = -ENOMEM;
			goto unwind;
		}

		for (i = 0; i < cxt.nrealwriters_stress; i++) {
			cxt.lwsa[i].n_lock_fail = 0;
			cxt.lwsa[i].n_lock_acquired = 0;
		}
	}

	if (cxt.cur_ops->readlock) {
		if (nreaders_stress >= 0)
			cxt.nrealreaders_stress = nreaders_stress;
		else {
			/*
			 * By default distribute evenly the number of
			 * readers and writers. We still run the same number
			 * of threads as the writer-only locks default.
			 */
			if (nwriters_stress < 0) /* user doesn't care */
				cxt.nrealwriters_stress = num_online_cpus();
			cxt.nrealreaders_stress = cxt.nrealwriters_stress;
		}

		if (nreaders_stress) {
			cxt.lrsa = kmalloc_array(cxt.nrealreaders_stress,
						 sizeof(*cxt.lrsa),
						 GFP_KERNEL);
			if (cxt.lrsa == NULL) {
				VERBOSE_TOROUT_STRING("cxt.lrsa: Out of memory");
				firsterr = -ENOMEM;
				kfree(cxt.lwsa);
				cxt.lwsa = NULL;
				goto unwind;
			}

			for (i = 0; i < cxt.nrealreaders_stress; i++) {
				cxt.lrsa[i].n_lock_fail = 0;
				cxt.lrsa[i].n_lock_acquired = 0;
			}
		}
	}

	lock_torture_print_module_parms(cxt.cur_ops, "Start of test");

	/* Prepare torture context. */
	if (onoff_interval > 0) {
		firsterr = torture_onoff_init(onoff_holdoff * HZ,
					      onoff_interval * HZ, NULL);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (shuffle_interval > 0) {
		firsterr = torture_shuffle_init(shuffle_interval);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (shutdown_secs > 0) {
		firsterr = torture_shutdown_init(shutdown_secs,
						 lock_torture_cleanup);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (stutter > 0) {
		firsterr = torture_stutter_init(stutter, stutter);
		if (torture_init_error(firsterr))
			goto unwind;
	}

	if (nwriters_stress) {
		writer_tasks = kcalloc(cxt.nrealwriters_stress,
				       sizeof(writer_tasks[0]),
				       GFP_KERNEL);
		if (writer_tasks == NULL) {
			TOROUT_ERRSTRING("writer_tasks: Out of memory");
			firsterr = -ENOMEM;
			goto unwind;
		}
	}

	if (cxt.cur_ops->readlock) {
		reader_tasks = kcalloc(cxt.nrealreaders_stress,
				       sizeof(reader_tasks[0]),
				       GFP_KERNEL);
		if (reader_tasks == NULL) {
			TOROUT_ERRSTRING("reader_tasks: Out of memory");
			kfree(writer_tasks);
			writer_tasks = NULL;
			firsterr = -ENOMEM;
			goto unwind;
		}
	}

	/*
	 * Create the kthreads and start torturing (oh, those poor little locks).
	 *
	 * TODO: Note that we interleave writers with readers, giving writers a
	 * slight advantage, by creating its kthread first. This can be modified
	 * for very specific needs, or even let the user choose the policy, if
	 * ever wanted.
	 */
	for (i = 0, j = 0; i < cxt.nrealwriters_stress ||
		    j < cxt.nrealreaders_stress; i++, j++) {
		if (i >= cxt.nrealwriters_stress)
			goto create_reader;

		/* Create writer. */
		firsterr = torture_create_kthread(lock_torture_writer, &cxt.lwsa[i],
						  writer_tasks[i]);
		if (torture_init_error(firsterr))
			goto unwind;

	create_reader:
		if (cxt.cur_ops->readlock == NULL || (j >= cxt.nrealreaders_stress))
			continue;
		/* Create reader. */
		firsterr = torture_create_kthread(lock_torture_reader, &cxt.lrsa[j],
						  reader_tasks[j]);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (stat_interval > 0) {
		firsterr = torture_create_kthread(lock_torture_stats, NULL,
						  stats_task);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	torture_init_end();
	return 0;

unwind:
	torture_init_end();
	lock_torture_cleanup();
	if (shutdown_secs) {
		WARN_ON(!IS_MODULE(CONFIG_LOCK_TORTURE_TEST));
		kernel_power_off();
	}
	return firsterr;
}

module_init(lock_torture_init);
module_exit(lock_torture_cleanup);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           // SPDX-License-Identifier: GPL-2.0+
/*
 * Module-based torture test facility for locking
 *
 * Copyright (C) IBM Corporation, 2014
 *
 * Authors: Paul E. McKenney <paulmck@linux.ibm.com>
 *          Davidlohr Bueso <dave@stgolabs.net>
 *	Based on kernel/rcu/torture.c.
 */

#define pr_fmt(fmt) fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched/rt.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>
#include <linux/rtmutex.h>
#include <linux/atomic.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/torture.h>
#include <linux/reboot.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul E. McKenney <paulmck@linux.ibm.com>");

torture_param(int, nwriters_stress, -1,
	     "Number of write-locking stress-test threads");
torture_param(int, nreaders_stress, -1,
	     "Number of read-locking stress-test threads");
torture_param(int, onoff_holdoff, 0, "Time after boot before CPU hotplugs (s)");
torture_param(int, onoff_interval, 0,
	     "Time between CPU hotplugs (s), 0=disable");
torture_param(int, shuffle_interval, 3,
	     "Number of jiffies between shuffles, 0=disable");
torture_param(int, shutdown_secs, 0, "Shutdown time (j), <= zero to disable.");
torture_param(int, stat_interval, 60,
	     "Number of seconds between stats printk()s");
torture_param(int, stutter, 5, "Number of jiffies to run/halt test, 0=disable");
torture_param(int, verbose, 1,
	     "Enable verbose debugging printk()s");

static char *torture_type = "spin_lock";
module_param(torture_type, charp, 0444);
MODULE_PARM_DESC(torture_type,
		 "Type of lock to torture (spin_lock, spin_lock_irq, mutex_lock, ...)");

static struct task_struct *stats_task;
static struct task_struct **writer_tasks;
static struct task_struct **reader_tasks;

static bool lock_is_write_held;
static atomic_t lock_is_read_held;
static unsigned long last_lock_release;

struct lock_stress_stats {
	long n_lock_fail;
	long n_lock_acquired;
};

/* Forward reference. */
static void lock_torture_cleanup(void);

/*
 * Operations vector for selecting different types of tests.
 */
struct lock_torture_ops {
	void (*init)(void);
	void (*exit)(void);
	int (*writelock)(int tid);
	void (*write_delay)(struct torture_random_state *trsp);
	void (*task_boost)(struct torture_random_state *trsp);
	void (*writeunlock)(int tid);
	int (*readlock)(int tid);
	void (*read_delay)(struct torture_random_state *trsp);
	void (*readunlock)(int tid);

	unsigned long flags; /* for irq spinlocks */
	const char *name;
};

struct lock_torture_cxt {
	int nrealwriters_stress;
	int nrealreaders_stress;
	bool debug_lock;
	bool init_called;
	atomic_t n_lock_torture_errors;
	struct lock_torture_ops *cur_ops;
	struct lock_stress_stats *lwsa; /* writer statistics */
	struct lock_stress_stats *lrsa; /* reader statistics */
};
static struct lock_torture_cxt cxt = { 0, 0, false, false,
				       ATOMIC_INIT(0),
				       NULL, NULL};
/*
 * Definitions for lock torture testing.
 */

static int torture_lock_busted_write_lock(int tid __maybe_unused)
{
	return 0;  /* BUGGY, do not use in real life!!! */
}

static void torture_lock_busted_write_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_lock_busted_write_unlock(int tid __maybe_unused)
{
	  /* BUGGY, do not use in real life!!! */
}

static void torture_boost_dummy(struct torture_random_state *trsp)
{
	/* Only rtmutexes care about priority */
}

static struct lock_torture_ops lock_busted_ops = {
	.writelock	= torture_lock_busted_write_lock,
	.write_delay	= torture_lock_busted_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_lock_busted_write_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "lock_busted"
};

static DEFINE_SPINLOCK(torture_spinlock);

static int torture_spin_lock_write_lock(int tid __maybe_unused)
__acquires(torture_spinlock)
{
	spin_lock(&torture_spinlock);
	return 0;
}

static void torture_spin_lock_write_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/* We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2 * shortdelay_us)))
		udelay(shortdelay_us);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_spin_lock_write_unlock(int tid __maybe_unused)
__releases(torture_spinlock)
{
	spin_unlock(&torture_spinlock);
}

static struct lock_torture_ops spin_lock_ops = {
	.writelock	= torture_spin_lock_write_lock,
	.write_delay	= torture_spin_lock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_spin_lock_write_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "spin_lock"
};

static int torture_spin_lock_write_lock_irq(int tid __maybe_unused)
__acquires(torture_spinlock)
{
	unsigned long flags;

	spin_lock_irqsave(&torture_spinlock, flags);
	cxt.cur_ops->flags = flags;
	return 0;
}

static void torture_lock_spin_write_unlock_irq(int tid __maybe_unused)
__releases(torture_spinlock)
{
	spin_unlock_irqrestore(&torture_spinlock, cxt.cur_ops->flags);
}

static struct lock_torture_ops spin_lock_irq_ops = {
	.writelock	= torture_spin_lock_write_lock_irq,
	.write_delay	= torture_spin_lock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_lock_spin_write_unlock_irq,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "spin_lock_irq"
};

static DEFINE_RWLOCK(torture_rwlock);

static int torture_rwlock_write_lock(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	write_lock(&torture_rwlock);
	return 0;
}

static void torture_rwlock_write_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/* We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	else
		udelay(shortdelay_us);
}

static void torture_rwlock_write_unlock(int tid __maybe_unused)
__releases(torture_rwlock)
{
	write_unlock(&torture_rwlock);
}

static int torture_rwlock_read_lock(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	read_lock(&torture_rwlock);
	return 0;
}

static void torture_rwlock_read_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 10;
	const unsigned long longdelay_ms = 100;

	/* We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealreaders_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	else
		udelay(shortdelay_us);
}

static void torture_rwlock_read_unlock(int tid __maybe_unused)
__releases(torture_rwlock)
{
	read_unlock(&torture_rwlock);
}

static struct lock_torture_ops rw_lock_ops = {
	.writelock	= torture_rwlock_write_lock,
	.write_delay	= torture_rwlock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_rwlock_write_unlock,
	.readlock       = torture_rwlock_read_lock,
	.read_delay     = torture_rwlock_read_delay,
	.readunlock     = torture_rwlock_read_unlock,
	.name		= "rw_lock"
};

static int torture_rwlock_write_lock_irq(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	unsigned long flags;

	write_lock_irqsave(&torture_rwlock, flags);
	cxt.cur_ops->flags = flags;
	return 0;
}

static void torture_rwlock_write_unlock_irq(int tid __maybe_unused)
__releases(torture_rwlock)
{
	write_unlock_irqrestore(&torture_rwlock, cxt.cur_ops->flags);
}

static int torture_rwlock_read_lock_irq(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	unsigned long flags;

	read_lock_irqsave(&torture_rwlock, flags);
	cxt.cur_ops->flags = flags;
	return 0;
}

static void torture_rwlock_read_unlock_irq(int tid __maybe_unused)
__releases(torture_rwlock)
{
	read_unlock_irqrestore(&torture_rwlock, cxt.cur_ops->flags);
}

static struct lock_torture_ops rw_lock_irq_ops = {
	.writelock	= torture_rwlock_write_lock_irq,
	.write_delay	= torture_rwlock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_rwlock_write_unlock_irq,
	.readlock       = torture_rwlock_read_lock_irq,
	.read_delay     = torture_rwlock_read_delay,
	.readunlock     = torture_rwlock_read_unlock_irq,
	.name		= "rw_lock_irq"
};

static DEFINE_MUTEX(torture_mutex);

static int torture_mutex_lock(int tid __maybe_unused)
__acquires(torture_mutex)
{
	mutex_lock(&torture_mutex);
	return 0;
}

static void torture_mutex_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 5);
	else
		mdelay(longdelay_ms / 5);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_mutex_unlock(int tid __maybe_unused)
__releases(torture_mutex)
{
	mutex_unlock(&torture_mutex);
}

static struct lock_torture_ops mutex_lock_ops = {
	.writelock	= torture_mutex_lock,
	.write_delay	= torture_mutex_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_mutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "mutex_lock"
};

#include <linux/ww_mutex.h>
/*
 * The torture ww_mutexes should belong to the same lock class as
 * torture_ww_class to avoid lockdep problem. The ww_mutex_init()
 * function is called for initialization to ensure that.
 */
static DEFINE_WD_CLASS(torture_ww_class);
static struct ww_mutex torture_ww_mutex_0, torture_ww_mutex_1, torture_ww_mutex_2;
static struct ww_acquire_ctx *ww_acquire_ctxs;

static void torture_ww_mutex_init(void)
{
	ww_mutex_init(&torture_ww_mutex_0, &torture_ww_class);
	ww_mutex_init(&torture_ww_mutex_1, &torture_ww_class);
	ww_mutex_init(&torture_ww_mutex_2, &torture_ww_class);

	ww_acquire_ctxs = kmalloc_array(cxt.nrealwriters_stress,
					sizeof(*ww_acquire_ctxs),
					GFP_KERNEL);
	if (!ww_acquire_ctxs)
		VERBOSE_TOROUT_STRING("ww_acquire_ctx: Out of memory");
}

static void torture_ww_mutex_exit(void)
{
	kfree(ww_acquire_ctxs);
}

static int torture_ww_mutex_lock(int tid)
__acquires(torture_ww_mutex_0)
__acquires(torture_ww_mutex_1)
__acquires(torture_ww_mutex_2)
{
	LIST_HEAD(list);
	struct reorder_lock {
		struct list_head link;
		struct ww_mutex *lock;
	} locks[3], *ll, *ln;
	struct ww_acquire_ctx *ctx = &ww_acquire_ctxs[tid];

	locks[0].lock = &torture_ww_mutex_0;
	list_add(&locks[0].link, &list);

	locks[1].lock = &torture_ww_mutex_1;
	list_add(&locks[1].link, &list);

	locks[2].lock = &torture_ww_mutex_2;
	list_add(&locks[2].link, &list);

	ww_acquire_init(ctx, &torture_ww_class);

	list_for_each_entry(ll, &list, link) {
		int err;

		err = ww_mutex_lock(ll->lock, ctx);
		if (!err)
			continue;

		ln = ll;
		list_for_each_entry_continue_reverse(ln, &list, link)
			ww_mutex_unlock(ln->lock);

		if (err != -EDEADLK)
			return err;

		ww_mutex_lock_slow(ll->lock, ctx);
		list_move(&ll->link, &list);
	}

	return 0;
}

static void torture_ww_mutex_unlock(int tid)
__releases(torture_ww_mutex_0)
__releases(torture_ww_mutex_1)
__releases(torture_ww_mutex_2)
{
	struct ww_acquire_ctx *ctx = &ww_acquire_ctxs[tid];

	ww_mutex_unlock(&torture_ww_mutex_0);
	ww_mutex_unlock(&torture_ww_mutex_1);
	ww_mutex_unlock(&torture_ww_mutex_2);
	ww_acquire_fini(ctx);
}

static struct lock_torture_ops ww_mutex_lock_ops = {
	.init		= torture_ww_mutex_init,
	.exit		= torture_ww_mutex_exit,
	.writelock	= torture_ww_mutex_lock,
	.write_delay	= torture_mutex_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_ww_mutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "ww_mutex_lock"
};

#ifdef CONFIG_RT_MUTEXES
static DEFINE_RT_MUTEX(torture_rtmutex);

static int torture_rtmutex_lock(int tid __maybe_unused)
__acquires(torture_rtmutex)
{
	rt_mutex_lock(&torture_rtmutex);
	return 0;
}

static void torture_rtmutex_boost(struct torture_random_state *trsp)
{
	const unsigned int factor = 50000; /* yes, quite arbitrary */

	if (!rt_task(current)) {
		/*
		 * Boost priority once every ~50k operations. When the
		 * task tries to take the lock, the rtmutex it will account
		 * for the new priority, and do any corresponding pi-dance.
		 */
		if (trsp && !(torture_random(trsp) %
			      (cxt.nrealwriters_stress * factor))) {
			sched_set_fifo(current);
		} else /* common case, do nothing */
			return;
	} else {
		/*
		 * The task will remain boosted for another ~500k operations,
		 * then restored back to its original prio, and so forth.
		 *
		 * When @trsp is nil, we want to force-reset the task for
		 * stopping the kthread.
		 */
		if (!trsp || !(torture_random(trsp) %
			       (cxt.nrealwriters_stress * factor * 2))) {
			sched_set_normal(current, 0);
		} else /* common case, do nothing */
			return;
	}
}

static void torture_rtmutex_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/*
	 * We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2 * shortdelay_us)))
		udelay(shortdelay_us);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rtmutex_unlock(int tid __maybe_unused)
__releases(torture_rtmutex)
{
	rt_mutex_unlock(&torture_rtmutex);
}

static struct lock_torture_ops rtmutex_lock_ops = {
	.writelock	= torture_rtmutex_lock,
	.write_delay	= torture_rtmutex_delay,
	.task_boost     = torture_rtmutex_boost,
	.writeunlock	= torture_rtmutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "rtmutex_lock"
};
#endif

static DECLARE_RWSEM(torture_rwsem);
static int torture_rwsem_down_write(int tid __maybe_unused)
__acquires(torture_rwsem)
{
	down_write(&torture_rwsem);
	return 0;
}

static void torture_rwsem_write_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 10);
	else
		mdelay(longdelay_ms / 10);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rwsem_up_write(int tid __maybe_unused)
__releases(torture_rwsem)
{
	up_write(&torture_rwsem);
}

static int torture_rwsem_down_read(int tid __maybe_unused)
__acquires(torture_rwsem)
{
	down_read(&torture_rwsem);
	return 0;
}

static void torture_rwsem_read_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealreaders_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 2);
	else
		mdelay(lon