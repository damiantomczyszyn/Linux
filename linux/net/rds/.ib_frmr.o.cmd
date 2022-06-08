(void *)&count, noop_count, NULL, &target_entry);

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
		 * Record the pointer to the trace for the next lock_list
		 * entry, see the comments for the function.
		 */
		trace = entry->trace;

		if (depth == 0 && (entry != root)) {
			printk("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}

		entry = get_lock_parent(entry);
		depth--;
	} while (entry && (depth >= 0));
}

static void
print_irq_lock_scenario(struct lock_list *safe_entry,
			struct lock_list *unsafe_entry,
			struct lock_class *prev_class,
			struct lock_class *next_class)
{
	struct lock_class *safe_class = safe_entry->class;
	struct lock_class *unsafe_class = unsafe_entry->class;
	struct lock_class *middle_class = prev_class;

	if (middle_class == safe_class)
		middle_class = next_class;

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
	if (middle_class != unsafe_class) {
		printk("Chain exists of:\n  ");
		__print_lock_name(safe_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(middle_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(unsafe_class);
		printk(KERN_CONT "\n\n");
	}

	printk(" Possible interrupt unsafe locking scenario:\n\n");
	printk("       CPU0                    CPU1\n");
	printk("       ----                    ----\n");
	printk("  lock(");
	__print_lock_name(unsafe_class);
	printk(KERN_CONT ");\n");
	printk("                               local_irq_disable();\n");
	printk("                               lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("                               lock(");
	__print_lock_name(middle_class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_bad_irq_dependency(struct task_struct *curr,
			 struct lock_list *prev_root,
			 struct lock_list *next_root,
			 struct lock_list *backwards_entry,
			 struct lock_list *forwards_entry,
			 struct held_lock *prev,
			 struct held_lock *next,
			 enum lock_usage_bit bit1,
			 enum lock_usage_bit bit2,
			 const char *irqclass)
{
	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================================\n");
	pr_warn("WARNING: %s-safe -> %s-unsafe lock order detected\n",
		irqclass, irqclass);
	print_kernel_ident();
	pr_warn("-----------------------------------------------------\n");
	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] is trying to acquire:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		curr->softirq_context, softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		curr->softirqs_enabled);
	print_lock(next);

	pr_warn("\nand this task is already holding:\n");
	print_lock(prev);
	pr_warn("which would create a new lock dependency:\n");
	print_lock_name(hlock_class(prev));
	pr_cont(" ->");
	print_lock_name(hlock_class(next));
	pr_cont("\n");

	pr_warn("\nbut this new dependency connects a %s-irq-safe lock:\n",
		irqclass);
	print_lock_name(backwards_entry->class);
	pr_warn("\n... which became %s-irq-safe at:\n", irqclass);

	print_lock_trace(backwards_entry->class->usage_traces[bit1], 1);

	pr_warn("\nto a %s-irq-unsafe lock:\n", irqclass);
	print_lock_name(forwards_entry->class);
	pr_warn("\n... which became %s-irq-unsafe at:\n", irqclass);
	pr_warn("...");

	print_lock_trace(forwards_entry->class->usage_traces[bit2], 1);

	pr_warn("\nother info that might help us debug this:\n\n");
	print_irq_lock_scenario(backwards_entry, forwards_entry,
				hlock_class(prev), hlock_class(next));

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
	excl |= (excl & LOCKF_  ƒ|$`"‡  ‹D$`¾€    ‰D$…Àˆ  ˜„   İ    ‰\$q‰L$$‰ó‰t$‹4$‰õ‹vhİ…ötz8  ‰l$ ‰D$‹D$‹l†‰êƒâuZ‹D$‹D$‹DÆ9Èuë>´&    ƒÂ‹ 9Át‹X…Ûuğ‹X…Ûtì‹[…Ûuâ‹ 9Áuåt& ƒú‡'  „(  ‹vh…öu—‹l$ ‹4$‹\$Ş‰D$‹€4  9Åt‹D$9„ì  …  ‹D$‹˜4  9İ„C  ‰Ø1Òët& ƒÂ‹ 9Åt‹p…öuğ‹H…Étì‹I…Éuâ‹ 9Åuåt& ƒú?‡1  öD$d„  ‹L$‹T$‰Ø´&    v …ÿt9xu9Ï…·   …Òt	;P„ª   ‹ 9ÅuÜ‹T$‹D$…ÒuéÕ   v ‹9İ„ª  ;Cuñû ğÿÿ‡Î  ‰ù…ÿ„ë  ‹$‰\$‰Ë1ö‹€”   ‹P@‹@<‰D$(‹D$`‰T$,‰D$0‹C(‹„°x  …Àt1ÉT$(èüÿÿÿ‰D´4…À„w  ƒştj‹C(¾   ‹„°x  …ÀuÑëæt& ¾êÿÿÿ‹D$Dd+    …>  ƒÄH‰ğ[^_]ÃöD$d…¬  t& ‹D$‹€4  9Å…=ÿÿÿ1ÛéBÿÿÿ´&    ‹\$…Ûtu‹D$‹k‰{‹L$‹|$‰C‹D$4‰ú‰C‹D$8‰C‹$‰Œ¸ì  èüõÿÿ‰Æ…À…ú   …í„Ÿ   ‰èèüÿÿÿ‹<$‹L$`1ö‹D$<‰úèüÿÿÿ‹L$`‹D$@‰úèüÿÿÿé8ÿÿÿ¡   ¹   ºÀ  èüÿÿÿ‰Ã…À„C  ‹t$‹L$‰+‹†8  ‰8  ‰C‰‹D$‰{‹|$‰C‹D$4‰ú‰C‹D$8‰C‹$‰Œ¸ì  èUõÿÿ‰Æ…À…²   ‹D$$àûÿÿèüÿÿÿéUÿÿÿ´&    ‰ò‰Øèüÿÿÿ‰D´4= ğÿÿ‡¾   ‰D´<éhşÿÿ´&    ‹D$‹Hé	şÿÿt& …ít[‰k‹D$<ÇC    èüÿÿÿ‹D$@èüÿÿÿéPşÿÿv éØüÿÿ‰è‹l$ ¨…Õüÿÿ¾ÿÿÿÿé.şÿÿ‹D$¾şÿÿÿ…À…şÿÿ1Ûéœıÿÿ‹D$<èüÿÿÿ‹D$@èüÿÿÿ‹C‹‰B‰‰ØÇ   ÇC"  èüÿÿÿéáıÿÿ¾şÿÿÿé×ıÿÿ‹D$<¾ôÿÿÿèüÿÿÿ‹D$@èüÿÿÿé»ıÿÿ¾ùÿÿÿé±ıÿÿèüÿÿÿ‰Şé¥ıÿÿ´&    èüÿÿÿS‰Ãö@,tƒT  1Ò[éüÿÿÿt& èüÿÿÿ‹C¨udÿ èüÿÿÿƒT  1Ò[éüÿÿÿ¶    ‹Cğƒ ëŞ´&    ´&    èüÿÿÿU¹   ºĞ  WV‰ÆSƒìdd¡    ‰D$`1Àl$‰ïó«†T  ‰$hÀ  èüÿÿÿZ‰Ç…À…¶   ‹^h…Ût"èüÿÿÿ‹ƒT  ¨…ì   dÿ èüÿÿÿ‹[h…ÛuŞ†4  ì  ‰ ‰@ƒÀ9Áuô†H  ‰†H  ‰†L  ëƒÇƒÿtxL½ ‰ú‰ğ‰ëèÉñÿÿ…Àtål$`‹ƒÃèüÿÿÿ9ëuò‹^h…Ûtèüÿÿÿ‹ƒT  ¨u{dÿèüÿÿÿ‹[h…Ûuâ‹$¿ôÿÿÿèüÿÿÿ‹D$`d+    u{ƒÄd‰ø[^_]Ã´&    f1Û¶    ‹L ‹„Ø  “ô   ƒÃ‰L–èüÿÿÿƒûuŞ1ÿë²f‹ƒX  ğƒ éÿÿÿ‹ƒX  ğƒ(…xÿÿÿ‹“X  ƒT  ‹Rèüÿÿÿé_ÿÿÿèüÿÿÿv èüÿÿÿU‰ÕW‰ÏVS‰Ãƒì‹ èüÿÿÿ‰Æ= ğÿÿ‡Ê   ‹S‰ĞƒàƒøtE¸    ‹[‰$èüÿÿÿ‹$1É‰ğR‰úSj è›øÿÿ‰Ã¸    èüÿÿÿƒÄöF,thƒÄ‰Ø[^_]Ãt& ‹C‰ê1Éèüÿÿÿ‰Å= ğÿÿws‹S¸    ‹[‰$èüÿÿÿ‹$‰é‰ğR‰úSj è>øÿÿ‰Ã¸    èüÿÿÿƒÄ…ítŸ‰èèüÿÿÿöF,u˜èüÿÿÿ‹F¨uRdÿèüÿÿÿ‰ØƒÄ[^_]ÃƒÄ‰Ã‰Ø[^_]Ãt& öF,t
‰ëé]ÿÿÿv èüÿÿÿ‹F¨u-dÿèüÿÿÿ‰ëé?ÿÿÿt& ‹Fğƒ(u¨‹VF‹Rèüÿÿÿë˜‹Fğƒ(uÍ‹VF‹Rèüÿÿÿë½´&    ´&    èüÿÿÿW‰×V‰ÆS‹ èüÿÿÿ‰Ã= ğÿÿwt‹F‰ú1Éèüÿÿÿ‹v‰Ç¸    ÿ ğÿÿwfèüÿÿÿ‰ú1É‰ØVè…ñÿÿ‰Æ¸    èüÿÿÿZ…ÿt‰øèüÿÿÿöC,t‰ğ[^_Ãèüÿÿÿ‹C¨uDdÿèüÿÿÿ‰ğ[^_Ã¶    ‰Æ[‰ğ^_Ã´&    èüÿÿÿ1É1Ò‰ØVèñÿÿ‰Æ¸    èüÿÿÿXë£‹Cğƒ(u¶‹SC‹Rèüÿÿÿë¦´&    èüÿÿÿUWV‰ÆSƒì‹Nd¡    ‰D$1À…É…ó   ‹F‰Õèüÿÿÿ‰Ç= ğÿÿ‡Ô   ¡   ¹0   ºÀ èüÿÿÿ‰Ã…À„ß   U¹\  º   èüÿÿÿ‹F‰{(T$‰C,‰Øèüÿÿÿ‰ÆX…öuS‹s,¸    èüÿÿÿ1É1Ò‰øjVSèÿõÿÿ‰Æ¸    èüÿÿÿƒÄ‰à…öuJèüÿÿÿ‰Æ‹D$d+    uvƒÄ‰ğ[^_]Ãt& ‰ØèüÿÿÿöG,uØèüÿÿÿ‹G¨u/dÿèüÿÿÿëÂt& èüÿÿÿëØ´&    f‰Æë©t& ¾êÿÿÿë‹Gğƒ(uË‹WG‹Rèüÿÿÿë»¾ôÿÿÿëŸèüÿÿÿ´&    t& èüÿÿÿUWV‰ÖS‰Ãƒì d¡    ‰D$‹èüÿÿÿ‰Ç= ğÿÿ‡ò  ¸    èüÿÿÿ‹CÇ$êÿÿÿƒø"‡<  ¾€    …Àˆ-  ˆ„   ‹”‡ì  ‹k‰L$ŒÇ4  ‰L$‹Œ‡Ø  ‹C‰T$‰L$ƒà„D  ‰Èèüÿÿÿ‰D$¹   FT$èüÿÿÿ…À…¬  F¹   T$èüÿÿÿ…À…“  ‹C‰î…À„¢   …í„š   ‹L$‰$…É„’   9ÈƒC  ‰D$Ç$äÿÿÿöC…~  ‹D$1í‹\Ç‹D$9Øtc‰|$‰Çë@t& ‹@(¹   T$‹@ ‰D$‰ğèüÿÿÿ…À…  ƒÅ;l$„Ñ   ‹ƒÆ9ß„Ä   ‹C…Àu½‹C…Àt¶‹@ë±Ç$    ¸    èüÿÿÿöG,t!‹D$d+    …ÿ   ‹$ƒÄ [^_]Ãt& èüÿÿÿ‹G¨…¨   dÿèüÿÿÿëÅ¶    ‹L$‹TÏ‹L$9Ñth‰$‰Ëë
ƒÀ‹9Ót‹J…Éuğ‹J…Étì‹I…Éuâëã‹$éƒşÿÿ´&    ‰$éoÿÿÿ´&    ‹|$éNÿÿÿ´&    Ç$    é¼şÿÿt& 1ÀéDşÿÿ´&    f‹|$Ç$òÿÿÿéÿÿÿ‹Gğƒ(…Nÿÿÿ‹WG‹Rèüÿÿÿé;ÿÿÿ‹L$‹D$‰êèüÿÿÿ‰$éàşÿÿèüÿÿÿt& èüÿÿÿUWVS‰Ã¿Ûƒì d¡    ‰D$1À¿D$4‰L$‰T$Áà	Ø‰D$d‹    èüÿÿÿ‹ƒÈ  ‹T$8ÇD$    ‹@8‹´Ø  èüÿÿÿèüÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ÛtB~t& ‰|$>t& ‹K$S0D$èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$èüÿÿÿ‹D$d+    ukƒÄ ‰Ø[^_]Ãt& èüÿÿÿS0‹s$‰ÅD$èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „KÿÿÿûéEÿÿÿèüÿÿÿ´&    ´&    èüÿÿÿU‰ÅWV‰Î¹	   S‰ÓƒìP‹D$d|$(‰D$‹D$h‰D$d¡    ‰D$L1À‰T$$ºÀ  ‰l$ ÇD$    ÇD$    ó«‹D$lÇD$,   ‰t$<‹L$`‰D$@¸   èüÿÿÿ‰D$(…À„4  ‰ÁD$1ÒPD$0P‰Ø‹{èüÿÿÿ[_…À…  …öt‹D$‹…Ét‹D$‹…Ò…  èüÿÿÿ‹T$pd‹    ‹ƒÈ  ÇD$    ‹@8‹´Ø  èüÿÿÿèüÿÿÿ‹ƒp  ~‰D$D$‰ƒp  ‹^…Ût@¶    ‰|$>t& ‹K$S0D$ èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$èüÿÿÿ‹D$(èüÿÿÿƒûu‹D$8…À…™   ‹D$0èüÿÿÿ‹D$Ld+    …7  ƒÄP‰Ø[^_]Ãt& ÇD$,    ELF                            4     (               èüÿÿÿÇ€À      1ÀÃ´&    ´&    èüÿÿÿÇ€À       1ÀÃ´&    ´&    èüÿÿÿ‹ˆÀ   …Éu‹€Ä   ÇB(p   ÇB4   ‰B81ÀÃ´&    èüÿÿÿ‹@\èüÿÿÿ1ÀÃèüÿÿÿV1É‰ÆS‰Óƒìd¡    ‰D$‹†”   f‰L$ÆD$ƒútAƒútLƒútGj ¹   T$èüÿÿÿYƒø…-   ‰Ä   ‹D$d+    u!ƒÄ[^Ãt& ÆD$ëÂ´&    fÆD$ë²èüÿÿÿt& èüÿÿÿ‹ˆÀ   …Ét	1ÀÃ¶    ‹R8èPÿÿÿ1ÀÃ´&    ¶    èüÿÿÿVS‰Ã‹@‹P‹Rèüÿÿÿ%   =   „B   ¸ûÿÿÿ[^Ã                     @  p                   "                                                                                                           @   p       €     @       (Radio)  6%s: Audio Mode: %u%s
 vp27smpx èüÿÿÿƒ¸À    ¹	   º    DÑƒÀpRÿpTPh
   èüÿÿÿ1ÀƒÄÃƒÆpVh    èüÿÿÿXZéÕ   ‹S·CŠ  Q QP‹CTÿ²   ÿ0h$   èüÿÿÿC¹À  ºÈ   èüÿÿÿ‰ÆƒÄ…Àu
¸ôÿÿÿéh  ¹@   ‰Úèüÿÿÿ‰ğº   Ç†Ä      è|   1Àéh   3%s: I/O error setting audmode
    6%s %d-%04x: chip found @ 0x%x (%s)
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ            vp27smpx                                                           À       `                                                                                                                                       @                                                                                         license=GPL author=Hans Verkuil description=vp27smpx driver  GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                                                    ,   @   )     =   p                                   	 M       1    	               a   €   Œ     v   1       	      #     ¡   @  /     °   F   v    	 Ä   @                       Ñ            æ       €     ö       
           0                        0     )  À   ,     <  `   P     O           f          |           —             ¢             À             È             Ú             ô                                       +             @             N             b           n             }      
     Œ      0      vp27smpx.c vp27smpx_s_radio vp27smpx_s_std vp27smpx_g_tuner vp27smpx_remove vp27smpx_log_status vp27smpx_set_audmode vp27smpx_set_audmode.cold vp27smpx_s_tuner vp27smpx_probe vp27smpx_probe.cold vp27smpx_ops vp27smpx_driver_init vp27smpx_driver vp27smpx_driver_exit vp27smpx_id vp27smpx_core_ops vp27smpx_tuner_ops vp27smpx_video_ops __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev _printk __stack_chk_guard i2c_transfer_buffer_flags __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vp27smpx_id_device_table      !     A     q     y          ’   !  Å   "  à   !    #      A    S  $  Ï   	  c  	                 h                             	                                      "     '      6     ;      e     j      |   %  ”     ›   &  ±     B          ¸                  '     (          *  @     D     L     l     Ä     Ô     Ø        	   .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .comment .note.GNU-stack .note.gnu.property                                                         @   o                    	   @       è
  x               )             À  €                   %   	   @       `                  /             @                     8             @  $                  4   	   @       €  H               E      2       d  +                 X               ¼                  T   	   @       È  €      	         g      2       L  J                 z             –                    v   	   @       H                  ‰             ª  
                  …   	   @       h                 ˜             À  0                  ”   	   @       x  @                             ğ  <                  ©      0       ,                   ²              ?                     Â             @  (                                h  Ğ              	              8  °                               ¸  Õ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          éáşÿÿv èüÿÿÿS0‹s$‰ÅD$ èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „ÿÿÿûéÿÿÿv ‹|$‹èüÿÿÿ‹D$0‹L$‰‹D$4‰éPÿÿÿ´&    ‹L$`ºÀ  ¸   èüÿÿÿ¹   ‰D$0‰Â‹D$‹ 9ÈGÁ‰D$4…ÒtR‹L$‹1ƒøs$…À„.şÿÿ¶ˆ
¨„!şÿÿ·Lşf‰Lşéşÿÿ‹zƒçü‰
‹Lü‰Lü)ú)ÖÁéó¥éïıÿÿÇD$4    éâıÿÿèüÿÿÿ´&    t& èüÿÿÿUWV‰ÆSƒìp‹¶ä  ‰D$‹„$ˆ   ‰T$‹œ$„   1Ò‰D$‹„$Œ   ‰L$‰D$d¡    ‰D$l1À‰TLƒÀƒø rô1Ò1À‰T,ƒÀƒø rô‹D$‹|$L$L‰D$(‹D$‹‹ ‰D$4‹D$‹ ‰D$8¸   9ÂLĞD$(èŞÿÿ‰D$…ÀxN‹?‹l$,9Ç‰|$<Oø…ÿˆñ  ‰ú1É‰èèüÿÿÿ‰ù‰Ú‰èèüÿÿÿ‰Ç…ÿt=‹l$,ÇD$òÿÿÿD$L9Åt‰èèüÿÿÿ‹D$ld+    …í  ‹D$ƒÄp[^_]Ãt& ‹D$1Òèüÿÿÿ‹  ÇD$$    èüÿÿÿ{èüÿÿÿd¡    ‹ˆp  ‰L$L$ ‰ˆp  ‹[…Ût;‰|$ >t& ‹K$S0D$(èüÿÿÿ‰Æ…öu|$$ ğÿÿwÇD$$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$$‹D$èüÿÿÿ…Û…Ç   ‹D$<‹l$,ƒøÿ„Æ   Œÿşÿÿ;D$õşÿÿ‹T$4‹L$‰‹T$8‹L$‰…À„±   ‹L$‰L$L9Í„¬   ‹D$ÇD$    ‰(éËşÿÿt& èüÿÿÿS0‹s$‰ÅD$(èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „óşÿÿûéíşÿÿv ‰\$‹l$,éJşÿÿv ÇD$   é:şÿÿv é şÿÿÇD$    é#şÿÿºÀ èüÿÿÿ‰Ã…Àt%‹L$<‹T$,èüÿÿÿ‹D$ÇD$    ‰éşÿÿèüÿÿÿÇD$ôÿÿÿ‹l$,éİıÿÿ¶    èüÿÿÿUWV‰ÆSƒìh‹ä  1ö‰D$‹D$|‹¼$ˆ   ‰D$‹„$€   ‰D$d¡    ‰D$d1À‰tDƒÀƒø rô1ö1À‰t$ƒÀƒø rô‹D$‰T$,‰L$0L$D‰D$ d¡    ‰D$8‹„$„   ‰D$4‰ÂD$ èÛÿÿ‰D$…ÀˆX  …ÿuL‹D$èüÿÿÿ‰T$4…Òˆc  …À…[  ‹D$‰Ö‹l$$9ÂOğ1É‰è‰òèüÿÿÿ‹T$‰ñ‰èèüÿÿÿ…À…,  ‹D$1Òèüÿÿÿ‹›  ‰|$èüÿÿÿ{èüÿÿÿd¡    ‹ˆp  ‰L$L$‰ˆp  ‹[…Ût;‰|$>t& ‹K$S0D$ èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹t$‹D$èüÿÿÿ…öˆ÷   ‹\$4‹D$$;\$u…Ûxq‰t$t6¹   ‰Ú‰$èüÿÿÿ‹$‹D$‰Ùèüÿÿÿ…ÀuF‹D$4‹L$èüÿÿÿ…Éu5‹D$$L$D9Ètèüÿÿÿ‹D$dd+    …”   ‹D$ƒÄh[^_]Ã´&    ‹D$$ÇD$òÿÿÿëÁ´&    v èüÿÿÿS0‹s$‰ÅD$ èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „Ãşÿÿûé½şÿÿv ‰t$éJÿÿÿèüÿÿÿfèüÿÿÿUWV‰ÖS‰Ãƒì<‹|$T‹D$P‰|$d‹    ‰T$8‹“ä  ‰\$‹‰D$Ø‰t$ ‰D$‹D$X‰L$$‰D$‹²  ‰\$(ÇD$0    d‹    ÇD$4    ‰\$,èüÿÿÿèüÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ÛtC~¶    ‰|$>t& ‹K$S0D$èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹D$…Àx‹L$‹T$(;€   …Òux‹T$8d+    uyƒÄ<[^_]Ãv èüÿÿÿS0‹s$‰ÅD$èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „;ÿÿÿûé5ÿÿÿv ‰ë„¸òÿÿÿézÿÿÿèüÿÿÿ     cgroup_bpf_enabled_key  __cgroup_bpf_run_filter_skb  __cgroup_bpf_run_filter_sk  __cgroup_bpf_run_filter_sock_addr  __cgroup_bpf_run_filter_sock_ops                                                                        @   ğ  0  P  `  p  Ğ     °  @  À  ğ  `	  À  €                   p  °  ğ  °  0  p  €    À  à       0  P$  °$  @&  °'  (  à)  ,   .  ğ0   4   6  cgroup_id:	%llu
attach_type:	%d
      À      Ô  @        €      ¤         -  -      /  È/      T2   3      45  06      D7  È7      include/linux/skbuff.h / kernel/bpf/cgroup.c include/linux/thread_info.h °      ö	  ­     @       ƒ	 #     Õ 	 3  -   é 	¸$   èüÿÿÿ ›  “  Ş    ›$  &  &  s'  Œ'  s(  ³)  S,   ÿÿÿ	
ÿÿÿÿÿÿÿÿÿ strnlen           P      0                                          	                                                               @                           	                                                            	                                                             	                                                             	         
                                   °                                             
                                                                                                               À  `  à        p                       GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ              
              +             N              o             •   4          ½              ã   5            P          /            
 T  Q          €  s          ®             Ú  t            •          2             ]  @   ª    w  0      •  P       ­  `  
     Å  p  V     ä  Ğ  /     ú                           V     )  `  E    @  @  y                                 R  À  ¼     a      
     u  $        €     i     ›    Œ     ²     ü     Í     ú     ß     <     ş  à  <       €  <     -  @  <     P  À   <     o     <     „     r     –     Í    ©      #     ´  °  w     Ê  0  7    â  p      ù  €  …        ¡     ,  À       D  à       [     
    q  `   <     ”  0      ¨  \                     ½           û          :          r          «          X       ¸     Ç   `	  W    9  °  6    ¸  ğ  ³    <  p  6    ß           î             ù                          ğ  3     2             ;             A             N             Y             f             p             ~             ‰             ›             ±             Ê             Ö             ï                                       (  °  Š     A             H             O  À  (     l  ğ  n     …             •             ¥             ·             Æ             Î             Ö             ä  €  ™     ø             	             '	             G	             e	             y	             ‹	             §	             Ã	             Í	             å	             ø	             
             
             3
             G
             [
             t
             ‡
             Ÿ
             »
             Ó
             ì
                                       .             D             \             k             ‚             –             ¯  P$  Q     Â             Ş  °$      ñ               @&  b                 +             A  °'  Ù     X  (  E    o             }             Œ             œ             ­  à)  ¬    Ã             ç             Ù             õ  ,  b       .  å    6             M             f  ğ0  
    ‰                          ­             ¾             Ë   4      î             û                6  ›    0  ,        D  0        \  œ        o           †  ¼       š  Ô        cgroup.c __kstrtab_cgroup_bpf_enabled_key __kstrtabns_cgroup_bpf_enabled_key __ksymtab_cgroup_bpf_enabled_key __kstrtab___cgroup_bpf_run_filter_skb __kstrtabns___cgroup_bpf_run_filter_skb __ksymtab___cgroup_bpf_run_filter_skb __kstrtab___cgroup_bpf_run_filter_sk __kstrtabns___cgroup_bpf_run_filter_sk __ksymtab___cgroup_bpf_run_filter_sk __kstrtab___cgroup_bpf_run_filter_sock_addr __kstrtabns___cgroup_bpf_run_filter_sock_addr __ksymtab___cgroup_bpf_run_filter_sock_addr __kstrtab___cgroup_bpf_run_filter_sock_ops __kstrtabns___cgroup_bpf_run_filter_sock_ops __ksymtab___cgroup_bpf_run_filter_sock_ops sysctl_convert_ctx_access cg_sockopt_convert_ctx_access cg_sockopt_get_prologue bpf_cgroup_link_dealloc bpf_cgroup_link_fill_link_info cgroup_bpf_release_fn cgroup_bpf_release bpf_cgroup_link_show_fdinfo __bpf_prog_run_save_cb copy_sysctl_value sysctl_cpy_dir sysctl_cpy_dir.cold __func__.0 cgroup_dev_is_valid_access sysctl_is_valid_access cg_sockopt_is_valid_access sysctl_func_proto bpf_sysctl_get_new_value_proto bpf_set_retval_proto bpf_sysctl_get_name_proto bpf_sysctl_get_current_value_proto bpf_sysctl_set_new_value_proto bpf_get_retval_proto sockopt_alloc_buf cgroup_bpf_replace CSWTCH.197 cgroup_dev_func_proto compute_effective_progs update_effective_progs __cgroup_bpf_detach bpf_cgroup_link_release.part.0 bpf_cgroup_link_release bpf_cgroup_link_detach cg_sockopt_func_proto bpf_get_netns_cookie_sockopt_proto __cgroup_bpf_attach bpf_cgroup_link_lops __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_ops491 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_addr490 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sk489 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_skb488 __UNIQUE_ID___addressable_cgroup_bpf_enabled_key483 bpf_get_retval __fentry__ current_task bpf_set_retval bpf_get_netns_cookie_sockopt init_net kcmd_drivers/media/i2c/vp27smpx.o := gcc -Wp,-MMD,drivers/media/i2c/.vp27smpx.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"vp27smpx"' -DKBUILD_MODNAME='"vp27smpx"' -D__KBUILD_MODNAME=kmod_vp27smpx -c -o drivers/media/i2c/vp27smpx.o drivers/media/i2c/vp27smpx.c 

source_drivers/media/i2c/vp27smpx.o := drivers/media/i2c/vp27smpx.c

deps_drivers/media/i2c/vp27smpx.o := \
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
    $(wildcar