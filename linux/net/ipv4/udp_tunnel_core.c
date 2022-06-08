
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
			chain_blo