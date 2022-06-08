
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
	int i = get_first_held_lock(curr, h