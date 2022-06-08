
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
	dump_stackƒ'  w  œ'  w  ±'  F  ¿'  ‹  Ô'  Œ  Ş'  L  ë'  M  ş'  L  (  N  (  r  !(  `  0(  a  Q(  M  d(  L  i(  N  ƒ(  w  ‘(  F  £(  R  »(  ‹  Í(  ‚  Ü(  ƒ  ì(  :  ö(    
)    )  L  )  M  4)  L  9)  N  G)  ‘  T)  R  k)  K  v)  `  …)  a  ‘)  ’  Ã)  w  Ñ)  Y  á)  F  ò)  R  ı)  ‹  *  L  *  M  .*  :  w*  ”  Œ*  •  ¥*  •  &+  •  h+  L  m+  N  ~+  R  ™+  `  ¬+  a  g,  w  {,  –  ˆ,  Y  ‘,  F  £,  R  Æ,  G  Ë,  `  ì,  _  ñ,  `  ,-  T  X-  G  c-  a  h-  b  q-  a  |-  R  ‘-  U  ¢-  V  ¬-  W  ³-  X  È-  U  î-  Y  .  F  -.  R  t.  ™  ˜.  š  Ã.  `  Î.  G  ë.  _  ğ.  `  ,/  T  X/  G  c/  a  h/  b  q/  a  z/  K  ”/  K  Ÿ/  R  É/  U  Ú/  V  ä/  W  ë/  X   0  U  /0  K  _0  ™  á0  Y  ñ0  F  11  R  ¶1  œ  Á1    â1  K  í1  R  2    "2  _  *2  `  02  G  d2  T  2  G  ›2  a   2  b  ­2  Ÿ  !3  U  23  V  <3  W  C3  X  X3  U  º3  o  Í3  [  å3  Y  4  F  64  R  r4  G  ¥4  ¡  Ó4  œ  à4    ó4    5  _  
5  `  5  G  D5  T  p5  G  {5  a  €5  b  5  Ÿ  ¼5  œ  Ê5  •  Û5  ¢  ğ5  K  û5  R  16  U  B6  V  L6  W  S6  X  h6  U  š6  Y  ¡6  F  ¿6  R  7  G  7  _  7  `  T7  T  €7  G  ‹7  a  7  b  ·7  R  É7  U  Ú7  V  ä7  W  ë7  X   8  U  78  Y  f  K  û  P  (    ³  k  û  k  r  a  ˆ  N  É  k  Ô  a  h$  ‡  $  ‡      @                A                B     	     
      C                D                                                           $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     €     „     ˆ     Œ          ”     ˜     œ           ¤     ¨     ¬     °     ´     ¸                   S               S                S  $     (     ,   S  0     4     8   S  <     @     D   S  H     L     P   S  T     X     \   S  `     d     h   S                                 $     (     0     4        :     e                                                 $     (     ,     0     4     8     @     `   I        ¤     °     À   Z     ^  @  ]  €  f  ¼    À    à  H     E  \    `    d    h    l    p        D     C     B     A     @   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab+cgroup_bpf_enabled_key .rel___ksymtab+__cgroup_bpf_run_filter_skb .rel___ksymtab+__cgroup_bpf_run_filter_sk .rel___ksymtab+__cgroup_bpf_run_filter_sock_addr .rel___ksymtab+__cgroup_bpf_run_filter_sock_ops .rel__mcount_loc .rodata.str1.4 .rel__jump_table .rodata.str1.1 .rel__bug_table .rel.text.unlikely .rel.smp_locks .rel.rodata .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                                         @   ;8                    	   @       pW  ğ  #            %             {8                     +             €8  ¸                   0      2       €8  –                 F             9                    B   	   @       `e     #            l             $9                    h   	   @       xe     #            —             09                    “   	   @       e     #   
         Á             <9                    ½   	   @       ¨e     #            ò             H9                    î   	   @       Àe     #            "            T9  ¼                    	   @       Øe  x  #            /     2       :  !                 B            4:  l                  >  	   @       Pg  Ø   #            O     2        :  I                 b            é:  <                  ^  	   @       (h  P   #            r            %;  
                  n  	   @       xh     #            …            0;  0                    	   @       ˆh  `   #         ELF                      "      4     (               èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[ÃèüÿÿÿWV‰ÆS»
   d‹=    ´&    v ‹†”   º)   èüÿÿÿƒàt¸Æ§  èüÿÿÿ‹¨uƒëuÖ¸ÿÿÿÿ[^_Ãt& èüÿÿÿëæèüÿÿÿVfÁÂS‹°”   ‰Ã·Êº&   ‰ğèüÿÿÿƒøÿt)‰Øèrÿÿÿ…Àx+º(   ‰ğèüÿÿÿfƒøÿtfÁÀ·À[^Ãf¡    …Àüÿÿÿ¸ÿÿÿÿëç´&    t& èüÿÿÿUW‰ÇV‰ÖS‰Ëƒì…É„–   ‹A‹)‰$ºó   ‰øèbÿÿÿ‰Á¡    …À   …Éx)‰Èƒà t0Ç$    1í¸   …Ût‹<$‰+‰{1É…öt‰ƒÄ‰È[^_]Ãt& ƒáº   Óâ÷Â uVâ  u6ƒùu¿Ç$    å  ÿ ë°´&    v Ç$    ½ÿÿÿ éaÿÿÿ´&    Ç$    å °  é|ÿÿÿ¶    ‰êÇ$    ¶êéeÿÿÿ´&    ´&    èüÿÿÿS‰Ãƒì‹    …É9   ƒÄ‰Ø1É[éÚşÿÿ´&    v èüÿÿÿS‰Ã‰Ñƒì¡    …ÀT   ƒÄ‰Ø1Ò[é©şÿÿ´&    fèüÿÿÿWfÁÂV‰ÆS‹¸”   ‰Ë‰Ñá ÿ  º'   ‰øèüÿÿÿƒøÿt1‰ğèšıÿÿ…ÀxfÁÃº(   ‰ø·Ëèüÿÿÿƒøÿt[^_Ã´&    v ¡    …Ào   ëãèüÿÿÿV‰Æ‰ÑSƒì¡    …ÀŠ   1Û…Ét
»   ¹   ‹†”   ‹P\ˆš"  ºò   èüÿÿÿ1ÀƒÄ[^Ãt& èüÿÿÿƒúwvV‰ÖS‰Ã¡    …À·   ‹ƒ”   ‹õ    ‹P\ˆŠc  º3   ¶Éèüÿÿÿºò   ‰Øè$ıÿÿƒàßx*‹õ   ºò   Áá	Á‰ØƒÉ·ÉèÑşÿÿ¸Æ§  èüÿÿÿ1À[^Ãf¸êÿÿÿÃ´&    v èüÿÿÿVS‹X‹P(³@ÿÿÿú	˜ „‰   w7ú 	˜ tWú	˜ uG‹H|‹CÔ‹P\ƒé@ˆŠ  ¶Éºç   èüÿÿÿ1À[^Ãv ú	˜ u·H|º   ‰ğè@şÿÿ1À[^Ãv ¸êÿÿÿ[^Ã‹H|‹CÔ‹P\ˆŠ  ¶Éºæ   èüÿÿÿ1À[^Ã´&    ·H|º    ‰ğèøıÿÿ1À[^Ãv èüÿÿÿU‰ÍW‰×ºò   V‰ÆSƒìèüÿÿ‹    ‰$…Û  ‰ø% °  …Ş   ‰ø„À…Œ   ‰ø%  ÿ „  ¸   ºˆ   »à  ·Èë
v ··KƒÃ¶Ò‰ğètıÿÿû  uä‹    …ÉÔ   ‰¾0  ·<$ºò   ‰ğ‰®4  ƒÏ·Ïè<ıÿÿ¸Æ§  èüÿÿÿ1ÀƒÄ[^_]Ã´&    ¸   ºˆ   »   ·Èët& ··KƒÃ¶Ò‰ğèôüÿÿûH  uä¡    …À~…éé   ¶    1Éº   »`  ë´&    v ··KƒÃ¶Ò‰ğè¬üÿÿûŒ  uä‹    …É8ÿÿÿéş   ¸êÿÿÿéVÿÿÿ¶    èüÿÿÿW1Éºò   V‰ÆS»   ë´&    ¶¶K‹†”   ƒÃ‹x\ˆŒ0  èüÿÿÿ…ÀxûÄ  uÕ1ÉºY   »`  ë´&    ··KƒÃ¶Ò‰ğèüÿÿûˆ  uä‹†0  öÄ°…}   ¹   ºˆ   „Àu?©  ÿ „¤   »à  ët& ··KƒÃ¶Ò‰ğè¼ûÿÿû  uä[1À^_Ã¶    »   ë··KƒÃ¶Ò‰ğèŒûÿÿûH  uä[1À^_Ã¶    1Éº   »`  ë´&    v ··KƒÃ¶Ò‰ğèLûÿÿûŒ  uä[1À^_Ã¶    »   ë··KƒÃ¶Ò‰ğèûÿÿûH  uä[1À^_Ã´&    ´&    èüÿÿÿUWV‰ÆSƒì‹@‹P‹Rèüÿÿÿ%  x =  x …d  F¹À  º@  èüÿÿÿ‰Ã…À„O  ¹    ‰ò»À   èüÿÿÿ1Éº   ‰øj Çƒ0  ÿ   Çƒ4      Çƒ8      Çƒ<     èüÿÿÿ¹ 	˜ ºP  ‰øj j j jj jjÿj€èüÿÿÿ¹	˜ ºP  ‰øƒÄ$j j j jj j?j j èüÿÿÿ¹	˜ ºP  ‰øƒÄ j h   j jj hÿ  j j èüÿÿÿ¹	˜ ºP  ‰øƒÄ j j j jj hÿ  jÿh şÿÿèüÿÿÿ‹«ü   ‰{l‰øƒÄ …íuCèüÿÿÿ1Ò‰ğèüÿÿÿº   ‰$‰ğèüÿÿÿº   ‰Ç‰ğèüÿÿÿÁç‰Â‰øĞ€<$ì…P  é+  èüÿÿÿƒÄ‰è[^_]Ã½íÿÿÿëî½ôÿÿÿëç                      P                      $                                                                          @                               €   ğ   ğ     P  À       `  Ğ  P      7%s: %s: failed
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
 vpx3220 internal composite svideo ƒÃph   Sh    èüÿÿÿƒÈÿƒÄéÇ   QƒÇp‰L$Wh   èüÿÿÿ‹L$ƒÄé)  @p‰$Ph(   èüÿÿÿXZ‹$é  Cp‰$Ph>   èüÿÿÿXZ‹$é6  ƒÆph(   Vh    èüÿÿÿƒÄé  ‰$…Ò¸N   ºQ   DÂPFpPhU   èüÿÿÿ‹L$ƒÄé×  Cpÿ4•Œ  Phh   èüÿÿÿƒÄé)  FpPhÏ   èüÿÿÿXZéî  FpPh³   èüÿÿÿ[Xéî  FpPh–   èüÿÿÿXZéî  FpUWPh„   èüÿÿÿƒÄé‰  f=€F„³   f=`B„â   ºí   f=€B„   ‹N·À‘  R·V1öÒRP¶|$CpWPh    èüÿÿÿƒÄ‹ƒ”   ¶¡  ¶–   ‹x\ˆŒ0  èüÿÿÿ…Àyq1ö·b  ¶–`  ‰ØƒÆèL  ƒş(uã1ö·"  ¶–   ‰ØƒÆèL  ƒş(uãéÑ  ºÿ   ‹F  P·F1öÀPCpRPh  èüÿÿÿƒÄéhÿÿÿƒÆƒş$t‡é[ÿÿÿºö   ëÃ6%s: chip (%02x:%04x) found @ 0x%x (%s)
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                                vpx3220_fp_read vpx3220_fp_write        vpx3220a                vpx3216b                vpx3214c                                                   À                             `                     ğ  À                                                              Ğ                                                     Y     £   ¤   ¨  ²  ¾  X   &   K ˜                        ò 3Ø¨ àÿá âã€äå€æ çàèøêğŠñøù$                            ˆ  ‰  Š  ‹  Œ     ğ w ò Õç A                        ˆ  ‰  Š  ‹  Œ     ğ w ò Ñç A                           ˆ  ‰ ğ Š ğ ‹   Œ € €  ğ s ò  ç á,  5  ?  debug parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=Laurent Pinchart description=vpx3220a/vpx3216b/vpx3214c video decoder driver   ˜            ÿ      GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                                                             _                                	 ,   €   d     Å           <           	 Q           \   ğ   ñ     k          	    ğ  &     –   =       	 ²      '     Ã   X       	 Ù   P  o     ñ   s       	   (          À  L     *     -    	 @    †     R           _  »       	 v  Œ       }     ½     Œ  `  j    š  à  (     ¥     (     ®  `  ,     ¸  Ø   W    	 Ë  Ğ  r    Ø     $     ä  `  (                   ì  P      ú              P         /  ÿ    	               *           >      €     M      
                   a  @   `     l     0     }  À   P                ¤          ½           Ë  ˜       İ  0        ô  <        
  T   <     %             0             N             e             r             ‹             š             ®             È             á             é                                       )             >             [             m             …             “             §           ³             Â      
     Ñ  @   `     ô              vpx3220.c vpx3220_remove vpx3220_fp_status vpx3220_fp_read vpx3220_fp_read.cold __func__.2 vpx3220_status vpx3220_status.cold vpx3220_g_input_status vpx3220_g_input_status.cold vpx3220_querystd vpx3220_querystd.cold vpx3220_fp_write.isra.0 vpx3220_fp_write.isra.0.cold __func__.1 vpx3220_s_stream vpx3220_s_stream.cold vpx3220_s_routing input_vals.3 vpx3220_s_routing.cold inputs vpx3220_s_ctrl vpx3220_s_std init_secam init_pal init_ntsc vpx3220_s_std.cold vpx3220_init init_common init_fp vpx3220_probe vpx3220_ops vpx3220_ctrl_ops vpx3220_probe.cold vpx3220_driver_init vpx3220_driver vpx3220_driver_exit vpx3220_id vpx3220_core_ops vpx3220_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free current_task i2c_smbus_read_byte_data __const_udelay __SCT__cond_resched i2c_smbus_write_word_data i2c_smbus_read_word_data _printk i2c_smbus_write_byte_data __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vpx3220_id_device_table param_ops_int      9     :     ;  !   9  2   <  L   =  [   >  y   ?     9     @  º   A  Ñ     ñ   9  !    ñ  9  ı    !  9  .    Q  9  v  @  ™  @  ±    Á  9  Ï       C    9  !    6  .  L  C  d  .  …  >  ¡  9  ì  C  8  C  a  9  ~    ¼  .  Ş  .  æ      >  ;  .  ^  .  e    €  .  ¦  .  ®    Ñ  9 