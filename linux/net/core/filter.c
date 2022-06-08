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
  include/media/i2c/uda1342.h \

drivers/media/i2c/uda1342.o: $(deps_drivers/media/i2c/uda1342.o)

$(deps_drivers/media/i2c/uda1342.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ude/asm-generic/ioctl.h \
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
  arch/x86/includeJ ¬aèŒ|Q¥éG1D„¬)öñUŸA66èï’şvy–7û¥¡ärU¯(fñ¶«ä—Û6±Nmÿ)Úã$4œX°åÏCìœYÚ+ŠÖ…hhúè°µÏ Š@»€^´‹¯7ö÷-·êOg×ŠP½VIçe	¡;º%[N=r\(ÎÒU`àïÁ¦ø}¤0¾ÂNmfÓ2À%®À:şÀd_lÀ&˜İ§Ôv*^æó0&?ği—†nA-ÑR`ÌnËğçà};S"ŞÆ—º:èS­à*‹
·‹¦îu¹µ¨»Äk"Xä?ÓúmtÙå¥Œ{PHÌˆiÅ¸G«lı˜Ï¨ÄRÙ4Qª…®Jq½`5ªO€~dû“®¤Å>dÙ?s&6Ê~K 0áï’˜¿ƒ¢'xI¡nf'Œú–­Éğº9}ğ‹¶çCò¤Í!£å¾²öÆEve]Äq¾àLGeméa8f P«S&;ÄM•1À$-öl l t˜H¦wAĞ4¥9…#•Ù•µîŸTÜöÔîb÷D¨ŸZ‚GaŠ0e5­ı1"–ˆı5_]¥ß§®é—,^iÍ\ÀÈ¼¾Fı˜NÙbÇ`fÌÚô¼¿4Ú‰Q=2ÃYêáÅ%AaÓñ†>³½)v]Vd^Í)¿€8¾)){˜ùÁÜìíœ, b½"wØGfãñ:Bµ9ÊÉ…e‚3§Néñ¸‡´¨J“­NCöğËHÓy|M£-İáŸİiÍ<Ù !±\òÙÉ×İÃª*UpcÄ‚
TÇ•Ìıg(Í`Ø2I9ıHe½´@NŒ]ûÍæ\Hjz×à«µ3œ”:	†ĞãúĞ€^ÕJÎ+Myh Ç:Ş{ÕnºoºŠgö1Llvr;›ıõÈş¬-¡·]Œ‘¯2İÎº§Âã©ÏÖ/.æ´IÂş-ÊRá!‚‘¢1ğ³•ìŠÁ›é;å–çŠ›³™#­Ùh×{æ–Vçøæ?l7IpÁB®{8†P +X³Lõõ‡J8ªTîœÜˆİ63ç–ÇÓã3¸”+¸æ»ñÛÂµ¡œšÓ:=:cá;-Ét»¯Ÿj]$Iº© $Éâ!OËOB‚¡vòs-¡ªa¢©Î@t^’™	-æZ%èTÈü·ttˆºP^˜Õx¶?B¨/{ÕÛcê8Åfı’jmŞ˜eˆ#²&ì‰´sO£Î$Ó–—ïv/9<áµ’gª€æmâ¦ĞŠ÷Ê4,9ís4•„?V	Èçv˜ij°#ˆºéf;*LPUv‘n§ïe}í¥öâD—ŸD.8å!vÓ=ÅZ_8ˆ2õÉ<	Z*cyÜTÈä÷8àÑ¬ãtE^™k ÿ­€+ä´ñ_h'²Ònkæ…U²£nm˜ôÄ©ÛWÍ¥¤zÒê™íåc‰²ºúÏl¨ŞbóhJ=V#ÀŠõ3Ñp’Œ÷^¸$\î‰hpUÒIW$.=Úé0—ˆ™ø^ñRÖK&ÃbúıÂ¯^ÇZ0˜\@ûÒ±œşÇ¼†Ë*5œ-@Üğ×œì7š¬~¥÷ÌG1¤Ğ*Îğ§ÿjBı‡İ_X¯˜‘ô`L{TÃŒ^áL¸}E™mÕL*òB¿Úï“ûÌ^áò½sPÿ.mÆå„x³Rtf.ƒê„ÀŸî_D¦	&r>íÿ²š:rñ\×­®í2[&†RB“qå´;t4æÚKZÑ”pø^©ulf^LCÓ~şU ?k $>X‹\˜G£ “K©e§ÎzşLB=.B×`zhƒ^¢¸½k¨?â7YmùPBeOÈ”˜—z>UŠZÜ²$úˆo~cTG/·Ü½ç<v8›—÷DbÿãÄC	ûÄdùg¼í…Ú*§½«Àˆê¥»±W>jcSìÊÄ„XhmE†åïÙ®ìóÛd¤ òw>B[ß%…!m¢Ğ‘²œIk‡±ŞèÆF¨şL”_Ê¨û§¬³KîçyîH´W¤;àBË—“Ô]Oœ‘€’’RV1)Ü":TÙÃáÿïRšKô[gûH*ì¼Ñm¥¼ÂÅëW‡PÑWpÍñ¨o‡¿[n20"&™İYö²õ˜°¤FHƒÜÏ:b°Û¼ùÔ<´1Qøçñ(|Ål.oN»Í5…¬§Š€ıEëà—ÊêÀDA‹ä3‹¤È$v_BøL½B%D—Q‘v`ÇJ@†FÆ¥Âåà¬£Š T“äg=&çàyÑŸC¡_‡t:tëF×Ÿ“xXÎÖ÷QØ=²)!·LËJS £ÿ^8rÍ˜Cp›Úô ß¾RtyhL`fíh¥±öp+"™—Ÿw-Kã@D9l•+
Î£ÔœÅ£H²eôø½¹üº3(I}@e[OØz‡MŸTsÁjÎá»ßKÕ¤–ßa5n=¡<=iöòƒDXòRy%®²ı¹·Ïİåiló°tÂ„äqR0Çø2S™†$®s~#§Ö'”ş?¥™sî5ä·}Cp^R£y²„O¾c-3#°¤â÷Ñ…yXzmqØdÎTX·UÃXb$VSq¹Ô ¨gk_LOÀ­Ô¶—ùX³i!oGÂíuBªX†¨÷Ú¨5’f’uõê…M¥“ÀT[f´ĞmPu2½•'êÎf@L|E³‚°d¨ÃX€i—wU‹wÆ}9*Y'ùU]Ñ¦ıÌ‰Şv>nÏTÆá^ŠKJÃ¿ø°üH I¹úb×ZÇ<(¸NRñA]Ì¸S,År)ŸÅ³PêTš'Ç†à½³Ü?‹äÕî.ˆ°QQV–dKò©k…b€İšaåµœã—5Ë†DıSãˆ±—Y…Các.ó°Û€Bƒ&u*Ñÿ& ¿%Z3§S(Á\ÃÜ±`,ÎZÛÆïÚÍ£êTg#Úb¯á¥Ö=
›#ÑÄstÏº‡fv$Ägj³TNm9²7¬n¡48'wrø |Š=yº_ÇÍtº2úYrË8(Ÿ o[ÇIC/ pVøú¡_î¥Ê/ßjLqwğ›àGüÄ
Š6XnèäÃ°o´ˆhÒŠìÙÂ©n(/H6ÃïÄf	ŸF­_nV€ ”/x‡~
úsàa,IÓqùÛšõôæµoüÒ)ÓECo6UİÒ=©ÈKŒ—*F~q­Âhúk™€U¥°ØÉ€SıUz{8±‚êšc>Xfvíb—gßVˆ*¼@Ïu–åM´Væ¦õŸw^‰Q&²'Ğ»Ì©Uhd	"á‘Õe˜3v£# ¥Ìop_Ó wG#§Îi”·ööù H‰Á†hà)<Bì®F‹ Ü¶Y{¤{o}ÄğêöÏØoôˆ(„†ø$ŒõüÓ1Z¨nX§–ÄÆ=Vß8˜€Ú¼]¿ı$»B|ìT&«èÇk}¹ÿO\ç0Ğ&|­0¾»—c)ÈPÈoJ•î¥0P‡|ø•ÕXC S«i…H¾¥Wàæ¦ğ$¦óôIû­·°+Ä‚sb–¡nÈóøí?ÏZı°*·U+í…°y§ş½ñÓ¬™÷»¿DK®Å=§Ubïµ=,ıôO)¶Ş›
l±µ@­¡~³ê‰ó¯K¤¬9NÌÖçnHŞnËñ"ú„Çà›ÌƒO âx/„*ƒ/÷¡iûO‘ş£…¼®Zâ·ÊèdîÛnªQ’G˜Z–›=¢a÷†‚sÕR½¯Š®öâ p´ù$5n«û YŸ¯u›6ÉÖ©ïª"ûËÜ„Ñ“ª]ã]—½±¤×Ïÿ…ªq"bvZúÌ°G[±)W9¬ˆa€×i:Æz%ñ¯È•Z«åL`çe–¥;W>¯² lj¬¹” y5£O²¼XÔH÷Û<Ó2Œ¶ gsşÊ:xÕ„H‚ŞÄĞ]“8}¥‡pØ/¹¾{˜œæUîÇå¾tñ«”&+KY³…A©M•{ú¯ú&˜½aØJmhàê>ô¾‘¥Œ0oKÉl)½<ùĞì­ü¿\R£@ĞZŸ†9Ÿm@=ÛO}ŸqÃéqà(ş_X¥2î§*[wÄ´ŞI×äÍwäÁX ªtt‚í\¾ATDÁñnŠ2{«ïĞâ²ÎRH!ëM˜!ş2Ô3“”Óƒ9†ğ§í gé-JlÏ´Œó€wÔâk³×¼-Ó x ìğle„…#®¡`‹
q‰[‹ƒİÓÇ°€gesæâ±“JîÍ­Ãn9±™E!¼æ\i¼©$(øÉNúlÁGÔNI‡ŸúWß^#ÈôÖf}Â Qzë€„ÇüYrtÏmæF9ğtŞ‚—ù–WéIƒöùièKj?!µø)Ho×k!—0Ãœ«æA˜E®Z|Ï§Ìg¯?sfQ±ì—mâsOmÉ	/ÃÄ’)fËuqqEƒq:XÁ'ÇTŠWoÅ×Ù[úÂØÌ|Ü}¨½Ç
’AŒMÆ¼’Î–3k¦Ü†Èûò'
{“H9}c+ô*#Ã¹_ß3Œœ„5à­¬u~¸1‡è•–Ø{ü0NÒ¶¯ıa¯ï?‹ã=UœxH™½`åà:ÂÜñ­ÁHäÖÍüwË­õ#.½ÎC(¿ôo–1»W´€ì£”iÓ¦Cªnªjiš·èÃ*¦#×ıñÂ´êõHı©rYÔ$tíÀ/l½²¬Dö1Pp@£Êsf
Ô%íl[*üâæ€XFUGUˆsP+Óçe/#é³m23@vt›î´¼ ØôÙÂÄşÙ:aHÛÎŞÈ¨µt’>=¾ ®Ã·Ç]o‡†÷û]ño—(Ã/"99³iãœ5©$¹CÌ‘Ü(ç¼±ŸÌpßPxÏÑâ÷ÊFM-‚áWPŒj‡K¯“àó)ÂUé‹eOz­£ÂŸ·€·v¼M³Y®uŒdÜh5ÅG²ÊUëááÿJúä()–ú…ØÓpPy$+F‹IWSß¦ÑĞ]s^€ë>>ŸŞé FíVCë•rm,Iºabİôúb&™/"Ë¯U|gµë"³İÆ£Bò‡c%Ó
Q›²¾„]åŞ[Qúİ	ìÔİ@üR“€ˆ$«'ÀD³¹ÙIÎ¼ HÎ«{÷áø7Ğ±É<AÉ½: Ï¹Ø6Äõäàf6{è°­­Ÿ4¼c±‰'¸‘{Rßm’DhöÕ•I|vÎªé]	?™ª‹Sµ§mìqÁh¿¿E|ş½â÷é.UäîÆ”±/r&_,
Á•…êÉßwÃ¾D-å†è6k<e™HüÂ#rx^nı“÷š³İ£eÉ•X­V‚%JÆ‰áÇæ%ÃtÚ6[ŠaÅ…úP3×Šæãç6t¬Íî$Ê^ŞÍ>kÖE<ÒØi6¢7ãl—O9GGëí ^å¿¦~ô<ÊX «èj¼ç•ªäl?…öQ³ş\?BõßI8vJ–gTMmSZz@ôc™Ÿ}r„r´)6‚¾œI#_UÜI«Œ*˜6ë¯ú›
'Ÿ½Ÿ\ÍQ‹Õ+<í•øh=y$ëÌ?~âG}af¾ìm 2ƒU[tU1W#cƒf7*Îª‡³¹†ã¿ë¾äélA³\6?x%mòB¾U\Š‡1ÿé'—¢ğ«JºQÔzûG%yŒ÷¥çİ|‰7ø¸É9£¯â Ô=çDÕ¦±†òƒ¤˜½oœá2¿°(ñ\ÙÒ·g. µj›Ò;Öğ´_~¶7<O†f]ÿÑ°l—jG×ıpYìV~BdÒ*›×<ªÄã$–ÕÀw`âğ}ÍwË–öÂiV½£IÇYâò±~Fm‘$8š­!ï‹°»K°„VGù2İN^¥=Š	çÄ9Õë4Î7»§K°Æ™ÜY`ÙŠŞ ¦<ªDÇâĞ2KèI‹Ï
z³#ûöoÛ‹†ÄÃ´{fè¹y½öR¶İÿF¬æ°qRóI§i'³ÁîdÁ*1èh·}ÚCõ`zÚ(Ta²wP+@<ìÁ>ĞKá0Ä“M‹±r1£cü;l’©Èß,óUYèD<ğ@RÕWtıBhsDjË92Ê~”•!sÅÿèÏ&tùä œ˜.¸0çvkVŒibG têüá´oİ‡+³Z/ Â,«Pó]Ä ¬åÕr­[{dY?Â)*ö©œç¬3Sˆánš°ø&yb(Ôò§ám¨Åe¯áû?³i_ü˜Û$õë´2€®;è»©÷d–@äÏŸ©äi3·b[1T?ØÂèTÈ„ìõgP´_$FG…pİºaß`"¤õè•¦îÂQÀ!‚ùÌ€vÎ(i1Ÿ
;¹‡‡0ÊÅk½´P*ñ½§Ny‰È4‘D€.Û½
SıxÔ®ß[&„¤sHİM"ÆhÊÌm Ş°f×%sYÙN¨Îõ6ÿ9Šıı¨èhnA¥¿q&.¦ÿ_|)şy<b+ªT4Ä&n€–è?n ±R¤ß@‚U¸A3ì­B%´xk/7€'/í» H¥& ÂJ?aãXÑËL»G¥ÿ‚ïÏO‰Á´T´=7µ‹ñËo»Êß«„N¶ØƒsÕrÏ)?ô³^82º«d6vt@Ş•¶D!’zyn¶Æ-GÏGŠÅ&å"U	¾,at™0¾â¡“wƒ§Œ)ñaßFµYŞŞËXuÈÿÂfqZviı±IrÖæ³ÓÜ¤Ö"å*½ƒ›çÔô
úš£lŠ}ê¶ñ.šÇnÒÔ£A…·%®ä@±ä½°¤^ñ0ğš¿!ÅÉf™şñòÛLw‡ÔsÇ;Tşñ>\ëDpV’+İ¬Oï}vìj´Ú´ñ¡›Àr"x²Ûc\í^–ÆÒ|Õş‹Ú)XÍ1íïã»áî¼I…æVŸ½F“Şİ„!í|ºÈœ¸¿u¶°IÇIæ,õV‰ošFÛ²ói~ÿXº{×P€¶/J×ò,–:ğ0Ìs@ykk²²K|‡°÷C¢éÛßDvô´U¨ —„TRşw)
’ ¶kŞ/HÈ9²!Å1¼._KN¬“‹b2À`Ø_)8`Ó)YIêvh8ãÂ$ÿ4ßæA,–Q‰«ô [ ;×ƒ	Nyá¤Ñ“e³|0XyÙnÂª )œ úqy¿X™gªqÀÆÀòvwËØä´N¬h1ÿ„øø€»~jhø¥gßâ¥³nüØ²Û0Çlé%‹¦•²óß(ïuàÎ6^jç¯Šºt2´“C™£ÙšJm.®5ì®Ì.ëH÷Ø±F­ôkİ~´1`ŒXNúmÈt35z^ù™SM$…D~|ş‹?ÂäGq¨bi{1B<½ãlo&Ü§#˜­øó@c8`_eõß’vocØÌË|‚ K{Ã/ˆ3:Ôÿ1'U&é<È-‹te„ôß]9Šå$+Ãfó›}±ˆüÏg\WZx¿¼Zªï¡-rr«Ø­bÃÀ®½ceeˆÂ.PuÙa‚Ì,§6l«’®JûV?<ÆMJô><"8#¸_l¿O«¡Aá™ÀzÅÏw(ĞZ«¨İç!"KAi/àu{i"$Íeê2¿07«õCÎ‡|t~ÏÆTLoU–ÔÉTÛ”İÅJÔ¾ºŞ‚ ¾ÿ”Y-ƒë^]f›U/İ5x(V‹Ì'¼”Ì÷Œë<KØKc/2ùqWÄËÉ‰^ÎÛ˜:='Ò(De[xòˆBÙ?ı{Ì2áŸœ¨ÃZ·í*<¸ PªîVö˜…5ë5lÚ(gøãU|«¹Ï™Bök…aäe€»V+}•O1.ÄœƒŞİÍ%±ÉP„¯a_„WŒÑmßÊW*gœÌşf»&7íÎy9‹qİ×Ğ—Éû¼¯¦¼İK»d?Êèë´¬ÒœÏÅ9¯Ú)»g<ÚƒÊ¼}~óï´Ç›»7Q9=‡ŒSÕ{ñYº«:7á¬#Z‘˜fj*VáµÂ¶ck+§_G–#&²ój„ılÆY$Ş|~úÊXï§uì:±»Ú€ñµOÃKislA ™gÍ­½(	˜t×ÂAçWç‹ù³ÇXÇ¡WkÃÇ&&æwÕôˆÕ¿{+”êÊÄ½IÌCÆ»'˜ñCÍï›BN!-41¤_ULÄö&7t/¶ù½1oH—aËÿ©Yùhb*CZê|ÕüèËZïï¦
xÍ×/v‡ÌEé½r*gbn”Äˆc)»iiñ)5z o32jßúÚ·€Nb‹ùmŞs)K>1M	÷üLÊäw¤“íÇÆGÜ‘º9Ùú}7üUOj]‹ ¯‰FpHşw[:£o„ğÖ‚+¢ã‘j6\°?èj^ÚqİY¹ª³eôüuyÄm.0ŸÛ´i«kl0ÍÆ±ä{“%–Î4Œß_füOÔ|ËÅSı-J 5á•lÍ‡èª–»SâÔñR½LYİÿÜşº[[Ï«-(Ï¸ËrDßn"GSÃ¾Sv–è?c[½^Ğu_L/~‰¯+Ú …“tÔB“4†1K"‰Ğp”¿õ0û'õÖö|¡{Ê—Üî5s—ª,¡YÂN0Šòıx}¨tlèJñê<åÄ\W›™y¶
5¡ı`eÏ ™8Ù®s(§µæ|ËÆŒí&Ál»ÌøWÆ®T—·u˜>¯º bƒÎÙœÎ-èT§ß§ë†Û…e9»
çG§£ï>SwSj¤îÚ/nÊ•İ@íıph‡M¿Ú†v ÔÊdşÏû ŸB‹ß5»^¯F#¾ÒÜi,='…dÆ¯yAå;ŞÕEº×pÂ1ªÎh	u<‘-à« 3_wxĞL.•¨Æ’×VË+}ÑÍ­7ÏaÕ`İ,«=pH@}urU;Ú¡«JÕÇQh}Š„½İ@¡/è3>ªvÎ`ßsÛ‘©˜Ù#Âõ¬å{~ñe~ĞOÃ(¦Îî…Rã´Èòj’WNRq‡ÙÚÖ¿,¨(Âüêòîpˆ.Q´àiì†RÊ¶6Ç%ƒ¬RtÑÏí=Z,¿a}i? 3xÏØRvR-Qe¢õeL‰ÊlfÂŠ—ù3Š]3
„Œ£ ~÷ßáz_Îk~“J±ùg0‘-=‚$·«b‡Vzõ¾`˜ÍApRz—¢|&fOC+Šv§ªĞ‚”âp­¨‘L$u_Û ÂÊúoZ¿N¬uN—)ŞuÄ7±~Dy_«Åq
0’šåUÙú.¿™“R}2[ÎImú<‹óŠPvæÈŸÏj^<PÙê~°†Tåi»®‘d†h ÀŒ¤«moZUw7ƒÓ“×6SÂÅ¬û4EWe‚¹.|q¸à‘ ^ÑºMù,šP³ÜSå¾$>*ScZ%é:¢ÎQçşƒ˜^d§×*{ •çãüŞf&@¥rˆ:¶ê 4L
¦N¢d#éÖG=AÑ ³Vœ(Ëm
Ï’s¯ãƒ¶ÒëK$í¾DÇ€Ñ­mc$PÆÅNì±ì	}ù}2Ï$54€H¹›6İQ:@z—ÊDš·#zf:a²Õ)¢S…S\ µPèÄ¥ˆ“ÎW)HOÚLS»îæ¤.C%É[®ÆºtO\9Såaqå]ı¤©fº‡<¨‚JïÁ,–¤”¬ÌèÒm=«1Jí¦E/*”^e<zeZ‹¬%CÊ7ydÎó†¥9Ó0N\¢ÜŒ~ÅFÈºy¹?³t:PêcÑÿ ôÏ^ƒ%ù7$ƒoySVT™úq8-v0õÚêÃ!<½÷ëÓÖì²¹u_#şŒy­“'è¶ ÷,TƒSˆµY–j¬’÷“YiÓ¸¯GJ,	J,F[g+ùˆMVÀ@õÜ °ıGyÚo\®ÄĞéb¾hàyMÀ²P„œH¹¦ıá§˜ïòÇ©Îß¤k¹²É¬U è¼»Rà‘ÌäwZÅkräQ‡áw~€4äDôh”	,Ó&œ›õ’ÖÙAMG!½$`!0Ş½£İBÍ«ˆ<‰¹êD¢¥9º/€Î]3í‚ÔçŠ £µÒÒ=uärIx#‰?©ĞJí¸0¿a3–…ºúİxü—´è	îï–:b¥&"æ`Tù<†úÅÆiê&ÁSùéŠ:•Ç3ö=½Ö´óZÖS¾Ú‘”qwvF?¨–Ê*‘&İ¥ïÌ£Âš¼çIùÌ¾Äj©mO–*ç:¸¾.ZDÛÁ‹²ç¶ÆÍ½fŸ
ŠÜ-“Ñ‰ÙB0XFÖp–e%sR:O¸6Ê×SVËĞwĞ¾'±ÀÅj°¶öàÂ/# t»HºÜRc}éHş#X’\dC„ìÔmÔ¶¦®{¤Œ œ¼Š†ıvw+×]À¢%ïÉ‘–´öÖFğè4Få—»š©ÑÜ§ÿ¬^8BÇigˆ]p L÷ÑÒ°HdóÍ¬CiÉló!¦SÙ®-
¬åb¨ÎPo¥ZªŒÅÓ]ªPİ
'¯Ptø
¦µ±6)ş-\(f°ıF:ŒaÎ(™FÏb¡øº[Ó ­vşıY¦“"À§æš7ZH©Öjªv2Ó®–íaNTOÎÀ<ÿYa r –A°¹ë‡w¬A0\ê1ÉÛ4Ÿ×z &PCMsOvULŞŞyÕ_#ËŞ½[ÃJ*$Û´úŒb…”nÃÔA=ñé¿~Ø &+uÏš5J
OqŸQoòí`õ=Ó‰y¸J/ÒXh®šÈˆ°ËİŠW/şßº‘_bôØÕUH¹ƒß ‡»Wïÿ HÚ‘"ŸqÃAk2M0ÅZ"ß¡ĞP³9İä‹:p:Î[H'
T— X$W¶LÊÉásñ¦!.ÿó‹ÁËÑç[Ş­v:›ÀÀ¿ûµ˜O,²F‚b@Ò”#"]»Ü87q
I“£@I1Ìé¸¿6Çp×u°‘
sşR®Š
[u³9ÛB'‡K`¨è¨!ïÂ±‘R×e±£Jî™êlw½¬ƒıR°ƒt/{ş‹«6ËR,púÀé‰¯å®¾1:´6F”ñ%C|‚æçĞ[L_¡–|à#´IOy\H¦JÒ^dTù Ç|_±Ze/_¯Wü Ñkù—”f)3!Ïë+7T¨ÂÁ–o»Cı}mÈR|H¶9ŸƒP´ˆWM}To^cÉz\Ú$àUù2ME!˜*tk~…:	«â,ï	ÈW‰M:tÀSë@¥1şğĞ
¹éûg`ŸM¼¸ù_lã@‡(SA	/m#eá4şë–hkQs&ôİtÈ? Ú—)5Ş 	–s¡¯7¶ëÌ9È²!ËPySû_¢Š-•1“%ÉjŞ×uÈ·Ğßºl­z3ğ¦È2ûDÿgë/dk.Î»Ô¸A
(ß¦€K¼ñkıOõTó©ô5¿>Ogü¶8ş­§Cûj7¸¶İ[;9µÌ3 .Í#c¥cş,…Û'/Şó7I+Œæå;Pb4Â×÷¢†¸_pCü–Ôˆ¹ñ»!øS½Ëfâ
>°¥À„RÁC5É*İÅ‘“ğb¸G€ócQ†ã[¶i.ÒK%à°\–¼M sœRı¿µZ7V~à‘¦§ÿ¼¨Ù©1£ïZ{¬!ú+á  Íj$İu;âÂd,Ìâc^)liiN#f
à GààÄ:K×µs¦˜Œõ‘„&|8¿h&æ¶‹Óó®¯»Ñxdwé¦Lß¬ßªmô+_œrŒâ±îaKafĞ%7@ÚÉŠ4Tñ§Q…·Ğ­ğ|î'ˆ“ô‹‚}º	pšHã¾Èçã!ès’«I$\È¦+®Ÿ PLú"àºmwÄõké5ÉsÏç´ÁÙ2û!ŞÎ§ÇYÊ$—ºùC{™=¶ÁÉ!˜ˆş$ı¢§'¢›Hƒª8µocÑ±±½¾ŒÌ¨5ÒS,Sä=„˜ÌM #¢ ıÇì„fáu*JM§„à
û¼>¥®#Å¾º9¸JuŞ=,ø@„ŸnÌ¥ü¹¡â‹«<ÎİÙ*=òl%×#-¿¼w}¿ hûyxß½\i™J3páuCçŞ®î­ä~¥¦”×e/eø-ÓÜ9G_‡æQ¢NŞ'nyrŠ†«Uxr”ox@™BÒ¢‰'rEÌÍZS S“ûí|—îjhú„mĞ[+ğÖxók·ÓıPºZJ[J6lP£]®ÍÎÖ*ß&Ûá¤P’Ğ¥?]DÓÔÂœºÕÅ´|ÖâqŒ÷3‡`Ë—–´¥Aùç]gÆRu?¾4\6Ò€ıtŒşİªœ#¨¹%·h½õ­š¸Hö<ºáßQ£¾âI4ùÄÜÖu!¶BÒi'E4ñ*´¸„ı¨
"HFÔ{
±h¤ ü+mRGNíëæ9ÂT×§g@i` cZ‚>$’İ[®ÇÇ)Ù·"|8`[lé|@µµê¢då]·¢nµâFsN¿şè˜6ğÃ¯¶‡
•XZº2³„¿VCSh(Âôf…”)´!âµçûÃ¡±ÖJAA™RIËÉf½qh4Æ·H·ªşmB¬e”Î#ÏC=òx—KGÈ¦iTgÂ.VJ$7®ñb¸70÷fp:²ˆÒ‡¸ú ÉÃgè PlÆĞ[R(-m7îñl†Â}&WÔ*¿„ş» w0lİ7íY½ñÏ”wî‘tºT›ê“?V¬™¶ï¡EÀÃ†É–Ñy“© Õ…Ãr÷÷Ö‘¿Y‚±¥hûwnQ¬Ädê{­ò²¹ŞÉ”C2+—†ı-r7~Ü½Å_\G£Ñ:m ŞPı¡‡õ ÑÆÒ§óöÈ¸3†j)DÂŠm%â*é·@U[ÓM¡ş€©“,µÒ´%¦¼[ù¨ÿn?T§ÑL™_¡ü?k«¹¦å®3?ŸBoÌ<ÂßÕã‡‡XıäÍÛm›òab®Srbkh?ı×H*X?iQL’Ô»—{‘n?òåôOê ùKsô5\Â$sÖ¿šH£ÔQò$UÅsÈ1Èq¾ù¾Ã~ûYAAuÌ…‘¸%ÃnqKÏd˜ìŸ“ícnÅ±]=‚-©áñtpò˜•f·ÅFOe®†Ÿ"Khœ(~â^3åd>kÛpØJ©í©–€fJ  êù{… ©:Í>÷¿Í *%SOA2&‡AÊ±îøŸõtç9È‘$3¢”ØTÌƒµÇóa¾ÆA—ş“t‚QØ6^•xòÚØËÑŠş-a°¨t¦]Ë‡›¯öJ¯k5†­Ê´(M˜…7<d(şÙ~s_§jbvÙN\Ó¢—‹ €DÉlz¸r½"i«/}¬w³Q(Æn…p€_AÅ_°‹‹ŒÃR­k«[ÌU)Q†Ú‚êº_ë:\§¨DD#ôkSÄt¥ ~§Q%/fdR?n˜s>hTx†4I–8|p›	ƒ³‘(áº9qÈvl¸¶²Û[üòpQNñ,×ÀU¢˜ä‡•CJ1g=şÈ@T œ KõÙAÔòÂRxÍƒŠ¤o!¢Ø¿XC‰Æ¦ñeijÔˆDDGÁ'ı3[çAô_;8Øë§~Ò˜1b¤æÓø¬BMLê’¶=àá ÎÍ#ğ«ÎõP¥ QBÉŸÜ|Ğ˜0Bz!&è†K€m’tÈ«÷ÉË#vIÁI0ÙÌ‚5»!:^gôÌ u‰J'GsO‰zöËÃ0ËKOg¦‘°íPm•cˆ7¤ÅŒœÂ9-'×Lf™“¥;
¨ Š·`xÛ7è4Óöo›¥6¥ÃFÂIqVÍqª[	ëhitÙ»?æn/B»•€gfüû¥ı]±AÇšQ¦e^1Ë‰a‹ËCõÎÁb©M[7ô°5ïÍ-öWP…,‹¸Ø6n¶"™¡©lsXN nÒÁ|d>Êµ2ƒ\Â‚%í³#j§:›ÕrImDŠM-[~ZHƒO´OfŸ@å 'ÊÍ>ÜñOJ}++Ì(ÒU9¥}¸‚.škfÑx­
ß½üQFóU°*«Í?qòP|›f?SQÿ@1ÁGC|hç'n.æHş?ÂDy	J,ŠÄ_Åy)hÌ÷yÖÆó‡•?|b^¿;Ó±î‰õW-Úâ31¿Jc¤=£'úòÈaYˆ€Ò‰Kõ!ó~<DïŠq.ßª—0–"i"î™hH}±@×>X+ù‡¶3)
m±$´"N;zÈÌêƒ¥¯»ò#’Z±Ô.Æ;öyô Ş¯Œ3î¸*
›a¡ç?Éºs'Øz-jÁß­Ü&‡­;U¬Üœ¦!QÄıç¨~‰F?5Å9WõÛ.AçE†SGÕ–¶óãµ¢ùp™{¬İ.€„ÑîyİÇš'	¢×¤4Šã…ó‘Åt÷ØCĞ®y†f Ã½mMØèÉ^	×-°ßæšÂŸ"*¨Me<ª+ó–jÖÍİã-/œ|’Ùœa/Ùü(€ĞÍ4Yõ cãûm
¢Vãô©+P#+Mi†f™z*nĞÌQêIöDıåËKx´º3¾9&€l¦$Çô‹4µôšh¡øç 7æß”A¯rÿÒƒ½NÛÀô„.Fµ=9m”ETÀrÌqØ
²ô]&°ön	hÚÑSÒÌ9ÏhVMådyºPôÁ„m¢üë÷ÁA/ÈÿÙb³NÙBH}øã‘º˜~¾.`¹·–º‹ØÓDÅ€—/-®Úş"Š4ãÿQ#w[s$¬{­ğ X.ÜvƒÑŒ%™~Ü ÀÙ€ke?7ŸÛ®
Iß*Éázr›HĞkiÂÜ	6zjA22á]‹5ç€´Dåºb–~¬åÎN’9î¤Î¶ Elp›Õ´¶ƒğ=™Xï•ºD·¬õ³„ÉY·^^w>=áèxÇPŸ,rIK¶”ğæÀ¾4¼­G!ƒ~¸
\d¶À7/QY'ÙÊ«nø-Û/)\{*³&‰™œ#gˆn4Ö;¬ò±À:¼¬âz§õßåéŸºAàbu’¯´NÎó#cğ…‚ÈÉdkã›ï¶ÍREøiÏió6V1Pió~r¾blÕIµ9æøØX:‚Îœ/¦*ˆñ±(İùU×òäµ¼-†%ùÇyU~Lt¼¦M+Œ…OAÉú3™qzÑ#™êòÂÑ¡jÁğqâÍA;Wê>üAÇ¬ò„äÛŸLÕ{²ËşƒFbÀG="·È¶G‹µKWÈ»qÒ÷Ÿb<-â
•]ãdp«‹qBT´êªNú…/ıÓy¬ëñİ¤l›²­ó5g"“£…ß´Ø˜·'EÍûÍÿ«@E)3—Jf2W‰ ĞÂàdzdÅ{ÚŠû–,ùé*ôA.™ ²ó¶&‹ª=Ş(C+Å\íÖÎŞKÁl|ı®•šç¶–IÛ´PW…-.g.©ô	”f”J5ZéÓßò˜–Ô‡‘àãıÄZ°6^H¹ØšYÊ×Dä´Ø/tòVVª»š¢<€C¨ó#?2$b»bìé'–P ·Õ}N-‚ĞMË@øxıe[4ETJ5–ñ]\5MûÆ0ş Ù^®,@î<¸/­VŸğD‚w4¸l£|õn¶©”¨Qy‘§ÆëÌ$Ñb¸ú¢\î(_ñp¨YÔá;s~¾­%¤mÁs|]Ë†GÏ®RHFÂüHd0Z†s ÷F!¿­Qh“ã	½æB8û§s½<ãÿÜ¨„#ÿó•Âg+6Ç^õ°;ÂÕI€Àñ JÒÎ¿qîÅ2Æ¨õH$2 ²`VÌî5R£Ó*PmÓ“«İ^§w¢ú`€1 İQÍ>õ.1†Œrş›
Â|"4Ø´&”Ñ>òu‡Böa¯MÑ>A'mö	mÑéU¹lYô¿´šS¢N¤ƒ¦·ína‚íÜÀÊoËO­É.ûæŸh­›:¢Ã<°ùmBĞu`´¶+ÏÈ~|–tU¼ãGƒPL¾ç†vó|µöÏ6º5®-°¨ƒá!á0 $ŒIgj[jƒ>k­ªø1J³&'`\ínjáî¨7ƒy«•ïÔ[Ö¤Éÿ}±iÇ¬Os3—nOŠÔğÜÔãë!‘pÅŸ¨Õk¬t\m2”ÜîÉC@,¤œúLV–m|ÍmôÃW½À<™!+åÌï?Æú·j‡ÕnõI¾/öÒHØàâšo‹ÛÃÁÃÇ§—×ñ<‹âjF‘¯÷ÎögO-2‚‰ôu©
b®ÔïõL
&sb?èNÒ¨ı¤Æ²üØu®‘¹è~iòõWZœ&"K:ş2ÅFĞº0¼Ñ{£ÅÄsÛ0fıĞ¢g9\G³@äÃá“°Ş×é`³Å±Êbè\™ì%ÒMí¾Ç¤0İÏGÈÇ%ô`=\gYşê™ô°u!¥Æ¹–<?“Ù¦¹ì"¨ydw"£çåo¸>²è‰¡Ÿ(³Š*“§ë9(<T©jiˆnmÀ‹Ş«qùgu¬µB»÷Ñ¹[O2ª™Css\°ùÙSˆû”–ÀaµŸİ1‹P!·5i¥°Õ?aƒn•Pùğ&6Û¦âì¡–d1ø#6ó• =³	êÛØÊş†÷69¿Ø˜qî Áh…YğÒ?³÷hzÕ‡Z‘•ƒ@pÔÓÇ ¾FRĞqõğÚÙjwn%äC¾©Äg³UŞ5&'…'›ø&Épx”mÈ *.aN¹D—Pæ4íÿPÊ@0š"¨ñÄ×Ò°é¹œ¤«
Ì	¨r†¬:/Ó“?w¦›¥şá`pNzıœ~Õ.À\O¿ÌGªqn$Adµ:mÛãB½Jtß<•2#2Æ¨êsŞ¤âòÊû	ü·?×Ğë=Ï\{ş ŠAlØğ¾ùÎhÉ³’úD<¿ù{úï9å°hi¡ID)ÜÏb£FÃ5ş:ÍN8>z¬A;hà’ä„ñš2¬Í})WA ÎQÕ‘›ã|İíˆÕaËèL„lœX¨S¸àØ;hˆåÒ~ßŸT!àÒrw$™íçÃ§î¥lİ”Kì%ÄÑ¿)ÑçêšDo¶ÿZu[±´şğf>(›6	Ü‰“{¿šXÅ:ó„"öŞOPÃî\¯õ ·‚‹xpÔkò{ù”4~iå—VñYÏsõŒ*ÅE®.‡¦Äœr³.™'ùŒÕB"¿ •*µ!çjå	ÂC8-9vvpLªÈ%ã­.ãUDÍA$Û»6{¨;ë˜ã%ı„£,Ô^Mõ×,N·ºh 0—üa*IT‰	L†çLt'?}RıuÆ~³Äd_óô€]NTváEL¡VªwèØÒüeuúMYO5¢¦ùCïW§¹_¿£o^èµ$ÎIÕnË°L•çugæTë;p,µçl®,³ñdE´ıIu¶ºí{¸{gî‘Íg°i3Aîn%’°—Ôúƒ
pfVÊ?t’@(%à;Ã¶Æ{ë8úéí”RŒ&©êáÛw|–q¿uMk\]
Ä^¸§ıë%³o¬åŸ&÷«Ş+JÕ‹	GÒ”Æ_¼®şˆ  I´ïp5©‘Ş‘Äµ»: €ß	®¹íX´†2ú‡ö”)+P?VœW-v/I7-k?&ç@€"qIg¯œå$àä r ¥¯õC‘[nü6ÑFLjáú•ÙÑ<qx†?	ú ŠbQqİt1+ò ®T®0¤\Rs…ÇZZ¿ÙCXõÇD(0è¥÷íÇ+ÑS[°Àò†É†¶Jâ’Ÿb´S£=›¨Ëİúm‚tÇê7 Â Èû`Q»^y	ÁÌKƒ!gVlÏüúÈı»¼»2² 7`?µ'vÙ–çå/Öén’›¬Zéİ&è­ëq}şºŒÍõİ¼Ékş¢ßÚŠ<ØgÔRe÷éywî$q÷Ğò ÿ}4r´'Ïïvş"¢^´f·Øë°æóQÔS“jrF”Ö€ğ±æ1=t²ÆŞ'êâ.éÕÙ DÅE_ßÃ¯Öó¢wæ²&±@¹Á|ª©^ƒÚI°+×`ƒI¥4Bï"Â—Kùêù‚¸‘Y+—ïJó*t·æ€ç·\rChJò$E:ÜsªüE¬ƒw”vŠáôÈwÒîÖ?/«qeFx ©Ú¸WË0šXçıAàWé|ãûìÏkHŸ'/¿ñT ˆw<Î˜ÂÚ-³ó¨Ï"JÔ–` v·®£Ç­'Y’Ç‡Uxöq¶[¬ÊXMlÃ{aË@\ŠM1ô£È"Tn²J—oÔÑ‰±Z9z´ì"—îş9]d¬lØ+>ÓÕ4ĞF¸Â>-»ßãe¢ë!¹„A­·Ó›à‘¼¯×vd/eJ:¶Ñ¬Ê¯¬¤BÈ’ËëÕÙO¾¼öõÏ{ŞG{eTÄãÿÕ•ˆHÚ…v"^ÙA¸Ì%(exó u%#_ÌÜùl ôéò8AœÙÔl–¨3½ID—ş„<¾	•”ÖgÜœ	ÃÑ1\p›,kÏÀ?7_3Æ)½Z2`ÑÒ‚Ó¬·LÀŞøìœã ¥í¢f7Óï‰É<ÖoôØğ¾V!•ï…÷ÒqŒ0Ù‚‘C¼ºy?m²qİÖ{&oĞş®$–Qª5ã7›ÊÈ‰Ä%rµ,dšaX®Üg^ 0gñŠ„ĞTo}ÑÈÁŒı„lßšµP[u ÁıİåÎÖ0Á›Š }Ô¾»Õ¾DZ«VŞÜàUpôÛ_D_T”âĞK¬M‘Ôßšp*Ì@r­¾u‡"!V$ÅFÑpB»€¨~öµc{½”ÁT\8Le>³;‰ˆí°	ƒ­øb(ôÚå´VtQ-Å+›È@>©.²ÛÔ4‘.¿	÷ì¦©"Ò
ú@|×ŒŸŞ3Hñ¾Fæa@†|hÇ‘‰£ñGe7!MGĞĞïŠÂ÷ÓtëÖ÷°(şÃ~ıYf)ôØn¯¸Ú
< Ë°x„Q$a•
ç¹|™«¦É'>£Ÿ?½
3;#¦í÷Œ_Aß7‚†“âQ2ùï«Âî <Å¸µPÒ´O>¼æ÷[%®ÒÇoÔ†òíûé0Œdae_*
Gà„ lºÿu$¬†èòü½'Ü-p¬{‘]Æ‰c8YÏŒ|Ê.æ†f/tœbB"ãJ†ô£ß’êçïó=%¾x¬«ÛUGbMÀÄı×Ì~ô©—Şf¶Àdp0¬#ïYÑP†‡ÇcáÎ1îj1
KùŠEéÀ©ÑĞ«lĞ*4øRğ¶®ù
Èê¤¿¸4j8¥R¾Â„&R±É?¾!i÷L[yÊ3D×~Í~d8EÀªo0°&~J#w	,bWì0wÁ4KëP£h‚æ§¢Pq|û#/[ı³"öÍZx[ÀT8êw#‚Ş|G	óá5—Ø-Ì%ıã¤,­q1ä5Ô6Í¥C‡Åö¹MyåBã¢Ã}Úm|BS¶ÄVÜí¸@T¸rŸšXCQğåıbç`ÔBiU—ı%äb]Ï6[cæ!?¨ƒ¢WË¼³Ææ§ÎÌğ;òâKN”€™€iæãc]¥ûXN±}*BeXÕ?£²Æ NGwÌ_Š0Ûpñ§1”0ÍŸnŠ 	6•{ë`dó;.ÒÑ’VMÁTÒÃĞÄÁ»<õÌÄ9ú+V.,jQ.ÍÌ€ àş&S2¤=]úŞéÇ40pú¥çcUæ€ß!ÎRr½‹urÎƒ êjÔ§êïñùª”»®ŸğçŸ;È(ùšM½ìù)’®øŞß\¼¨úéÚ¡Û§é™¿leIĞĞúY‚„ŞÍ‘Şó·G7Ú™nó»²0D/ßÍJßâeîÚ{>zR-—–¡>Vö¹Gƒ/äï¸°
EDÃÿ–İP-,\ßrï^·üQ?ÿŒ”<t46 ŠT©rÑÙ±Sı‹Í²ÆCU9í1%/Zbâ–³ÑØr¶Ê†ç+j@¤øªá¾­0%Z&N‡ttEÏ‚ºøü”k¥¦S2±,¿Ÿœâ·‘şXùõK¦@˜üwşº¢ct7;„=ic¾rˆõ`ÖéH<Ñ
noÛÏ.V±ö&İÚ‡úPÙg›”AŠmAöŒFsàU¡zÕª’$ZçÖ<eşœ)8 ûDÌØ¢€ô~¥ì˜y“’ì¶Ja7)€¶=—l½9Ö(~í7ğa«ıÄŠ1Á cÏ‘: ùŸ÷$ê©œ€şQGífĞt*÷ƒ¹6¤°É¯éRìjH•`DWd–ac¼¿µ×QÄ†ƒúÀ˜E|û«¨¤7Pk«,?ÁVˆã’1f;È÷“NÅ±c…½ïõº)gj}Wã£‘j“Ë8'Š	H?ãUÙÖ¿`\zïeaæn&E~ -	‚ÚÃÉÖ2¾.Ï¯,eHôÑ†µ
³6,sVX§Æ©2@ài/ƒÿ;cë±r’*‘K­õl™‡—˜¨AXïŞ8ê=¡7r¢V×ğÙeO£ĞYÀ)ê}³2AÚ«/6
{nåüŠôB [ymèÜigi75)
Ùø
, ³(üÔÃ`½]Ûğ­ÿœ æ-ŸZéŒ/^9È.ß®€¸®Ú€ÏY=Óé~[6´puº±gó!‘ÜNê&pVboNât&ë›ÜÀë¥ÎTÄKÜ¬9z§–ªš…ï=Õ.Ùı»ğ«€bßNA¹aâ ²›j¾áÎùÿ¨M¢@H#ÙU‰c3† !;¹jÃU©D²¨(ÜÂˆUã1üLÓ*T`£8ßyë´T‹…ÌÇ=¦$Ğ¤ƒãY7OmØ¡’W”Óÿ_emÁB‚|Gå¤âk­´ÿ›zn§cÕ">á³î¥â«-ó$õ£Udv€nÉ¯ù3ò½ê8*=µ¨ú`;ôFĞ/¦]‹
Ï­¬æ%Òä•†në§Ë±œíD‹aã¯\`#IvİeŠ¬EÊ´ã*Zæ,.¢»rûOX¬:f×?UQ õQ¹ió)³QóÄè_›Ÿ–¤8¥~Mo…ÉÓ¤š‚EJöÚ¸—Öì¶¦EÂNÆà#§—Ì°ò¤|´ËDäÃ|¯E´ÀÎÙ_ı	‡˜ÏšœĞSDm„¤©¯OìTis¬txè#™æ<Âv>hHéšO´Œpï¼ÃÃN;Àîgl––öín.L†ÏR`¾İ„:;ÓZw¯ê·Qê³ª2°ı¨Aæh¥Œ6M!ZÙAœq•­İhÊ¥c²EdyÁÇ™O>¸‰VyÇ)v×X˜Ï½<Än`z÷6 ÏÈlíô‘|~Cï7>åãWûåĞÏeú2‚§Rd‚wHó*™XUàâBŒäJw†¥ùQlz l£Éëç´$Ç¥½0Éº…•õ+!Å¾yGˆLÕEşå`k°”ÿ#();
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
		trace->softirq_disable_event, (void *)trace->softirq_disable_ip,
		(void *)trace->softirq_disable_ip);
}

static int HARDIRQ_verbose(struct lock_class *class)
{
#if HARDIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int SOFTIRQ_verbose(struct lock_class *class)
{
#if SOFTIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int (*state_verbose_f[])(struct lock_class *class) = {
#define LOCKDEP_STATE(__STATE) \
	__STATE##_verbose,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline int state_verbose(enum lock_usage_bit bit,
				struct lock_class *class)
{
	return state_verbose_f[bit >> LOCK_USAGE_DIR_MASK](class);
}

typedef int (*check_usage_f)(struct task_struct *, struct held_lock *,
			     enum lock_usage_bit bit, const char *name);

static int
mark_lock_irq(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit new_bit)
{
	int excl_bit = exclusive_bit(new_bit);
	int read = new_bit & LOCK_USAGE_READ_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * Validate that this particular lock does not have conflicting
	 * usage states.
	 */
	if (!valid_state(curr, this, new_bit, excl_bit))
		return 0;

	/*
	 * Check for read in write conflicts
	 */
	if (!read && !valid_state(curr, this, new_bit,
				  excl_bit + LOCK_USAGE_READ_MASK))
		return 0;


	/*
	 * Validate that the lock dependencies don't have conflicting usage
	 * states.
	 */
	if (dir) {
		/*
		 * mark ENABLED has to look backwards -- to ensure no dependee
		 * has USED_IN state, which, again, would allow  recursion deadlocks.
		 */
		if (!check_usage_backwards(curr, this, excl_bit))
			return 0;
	} else {
		/*
		 * mark USED_IN has to look forwards -- to ensure no dependency
		 * has ENABLED state, which would allow recursion deadlocks.
		 */
		if (!check_usage_forwards(curr, this, excl_bit))
			return 0;
	}

	if (state_verbose(new_bit, hlock_class(this)))
		return 2;

	return 1;
}

/*
 * Mark all held locks with a usage bit:
 */
static int
mark_held_locks(struct task_struct *curr, enum lock_usage_bit base_bit)
{
	struct held_lock *hlock;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		enum lock_usage_bit hlock_bit = base_bit;
		hlock = curr->held_locks + i;

		if (hlock->read)
			hlock_bit += LOCK_USAGE_READ_MASK;

		BUG_ON(hlock_bit >= LOCK_USAGE_STATES);

		if (!hlock->check)
			continue;

		if (!mark_lock(curr, hlock, hlock_bit))
			return 0;
	}

	return 1;
}

/*
 * Hardirqs will be enabled:
 */
static void __trace_hardirqs_on_caller(void)
{
	struct task_struct *curr = current;

	/*
	 * We are going to turn hardirqs on, so set the
	 * usage bit for all held locks:
	 */
	if (!mark_held_locks(curr, LOCK_ENABLED_HARDIRQ))
		return;
	/*
	 * If we have softirqs enabled, then set the usage
	 * bit for all held locks. (disabled hardirqs prevented
	 * this bit from being set before)
	 */
	if (curr->softirqs_enabled)
		mark_held_locks(curr, LOCK_ENABLED_SOFTIRQ);
}

/**
 * lockdep_hardirqs_on_prepare - Prepare for enabling interrupts
 * @ip:		Caller address
 *
 * Invoked before a possible transition to RCU idle from exit to user or
 * guest mode. This ensures that all RCU operations are done before RCU
 * stops watching. After the RCU transition lockdep_hardirqs_on() has to be
 * invoked to set the final state.
 */
void lockdep_hardirqs_on_prepare(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs do not (and cannot) track lock dependencies, nothing to do.
	 */
	if (unlikely(in_nmi()))
		return;

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (unlikely(lockdep_hardirqs_enabled())) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * See the fine text that goes along with this variable definition.
	 */
	if (DEBUG_LOCKS_WARN_ON(early_boot_irqs_disabled))
		return;

	/*
	 * Can't allow enabling interrupts while in an interrupt handler,
	 * that's general bad form and such. Recursion, limited stack etc..
	 */
	if (DEBUG_LOCKS_WARN_ON(lockdep_hardirq_context()))
		return;

	current->hardirq_chain_key = current->curr_chain_key;

	lockdep_recursion_inc();
	__trace_hardirqs_on_caller();
	lockdep_recursion_finish();
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on_prepare);

void noinstr lockdep_hardirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs can happen in the middle of local_irq_{en,dis}able() where the
	 * tracking state and hardware state are out of sync.
	 *
	 * NMIs must save lockdep_hardirqs_enabled() to restore IRQ state from,
	 * and not rely on hardware state like normal interrupts.
	 */
	if (unlikely(in_nmi())) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;

		/*
		 * Skip:
		 *  - recursion check, because NMI can hit lockdep;
		 *  - hardware state check, because above;
		 *  - chain_key check, see lockdep_hardirqs_on_prepare().
		 */
		goto skip_checks;
	}

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (lockdep_hardirqs_enabled()) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * Ensure the lock stack remained unchanged between
	 * lockdep_hardirqs_on_prepare() and lockdep_hardirqs_on().
	 */
	DEBUG_LOCKS_WARN_ON(current->hardirq_chain_key !=
			    current->curr_chain_key);

skip_checks:
	/* we'll do an OFF -> ON transition: */
	__this_cpu_write(hardirqs_enabled, 1);
	trace->hardirq_enable_ip = ip;
	trace->hardirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(hardirqs_on_events);
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on);

/*
 * Hardirqs were disabled:
 */
void noinstr lockdep_hardirqs_off(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * Matching lockdep_hardirqs_on(), allow NMIs in the middle of lockdep;
	 * they will restore the software state. This ensures the software
	 * state is consistent inside NMIs as well.
	 */
	if (in_nmi()) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;
	} else if (__this_cpu_read(lockdep_recursion))
		return;

	/*
	 * So we're supposed to get called after you mask local IRQs, but for
	 * some reason the hardware doesn't quite think you did a proper job.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (lockdep_hardirqs_enabled()) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		__this_cpu_write(hardirqs_enabled, 0);
		trace->hardirq_disable_ip = ip;
		trace->hardirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(hardirqs_off_events);
	} else {
		debug_atomic_inc(redundant_hardirqs_off);
	}
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_off);

/*
 * Softirqs will be enabled:
 */
void lockdep_softirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c, avoids
	 * funny state and nesting things.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		debug_atomic_inc(redundant_softirqs_on);
		return;
	}

	lockdep_recursion_inc();
	/*
	 * We'll do an OFF -> ON transition:
	 */
	current->softirqs_enabled = 1;
	trace->softirq_enable_ip = ip;
	trace->softirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(softirqs_on_events);
	/*
	 * We are going to turn softirqs on, so set the
	 * usage bit for all held locks, if hardirqs are
	 * enabled too:
	 */
	if (lockdep_hardirqs_enabled())
		mark_held_locks(current, LOCK_ENABLED_SOFTIRQ);
	lockdep_recursion_finish();
}

/*
 * Softirqs were disabled:
 */
void lockdep_softirqs_off(unsigned long ip)
{
	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		current->softirqs_enabled = 0;
		trace->softirq_disable_ip = ip;
		trace->softirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(softirqs_off_events);
		/*
		 * Whoops, we wanted softirqs off, so why aren't they?
		 */
		DEBUG_LOCKS_WARN_ON(!softirq_count());
	} else
		debug_atomic_inc(redundant_softirqs_off);
}

static int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	if (!check)
		goto lock_used;

	/*
	 * If non-trylock use in a hardirq or softirq context, then
	 * mark the lock as used in these contexts:
	 */
	if (!hlock->trylock) {
		if (hlock->read) {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_HARDIRQ_READ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_SOFTIRQ_READ))
					return 0;
		} else {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock, LOCK_USED_IN_HARDIRQ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock, LOCK_USED_IN_SOFTIRQ))
					return 0;
		}
	}
	if (!hlock->hardirqs_off) {
		if (hlock->read) {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ_READ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ_READ))
					return 0;
		} else {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ))
					return 0;
		}
	}

lock_used:
	/* mark it as used: */
	if (!mark_lock(curr, hlock, LOCK_USED))
		return 0;

	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return LOCK_CHAIN_HARDIRQ_CONTEXT * !!lockdep_hardirq_context() +
	       LOCK_CHAIN_SOFTIRQ_CONTEXT * !!task->softirq_context;
}

static int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	unsigned int depth = curr->lockdep_depth;

	/*
	 * Keep track of points where we cross into an interrupt context:
	 */
	if (depth) {
		struct held_lock *prev_hlock;

		prev_hlock = curr->held_locks + depth-1;
		/*
		 * If we cross into another context, reset the
		 * hash key (this also prevents the checking and the
		 * adding of the dependency to 'prev'):
		 */
		if (prev_hlock->irq_context != hlock->irq_context)
			return 1;
	}
	return 0;
}

/*
 * Mark a lock with a usage bit, and validate the state transition:
 */
static int mark_lock(struct task_struct *curr, struct held_lock *this,
			     enum lock_usage_bit new_bit)
{
	unsigned int new_mask, ret = 1;

	if (new_bit >= LOCK_USAGE_STATES) {
		DEBUG_LOCKS_WARN_ON(1);
		return 0;
	}

	if (new_bit == LOCK_USED && this->read)
		new_bit = LOCK_USED_READ;

	new_mask = 1 << new_bit;

	/*
	 * If already set then do not dirty the cacheline,
	 * nor do any checks:
	 */
	if (likely(hlock_class(this)->usage_mask & new_mask))
		return 1;

	if (!graph_lock())
		return 0;
	/*
	 * Make sure we didn't race:
	 */
	if (unlikely(hlock_class(this)->usage_mask & new_mask))
		goto unlock;

	if (!hlock_class(this)->usage_mask)
		debug_atomic_dec(nr_unused_locks);

	hlock_class(this)->usage_mask |= new_mask;

	if (new_bit < LOCK_TRACE_STATES) {
		if (!(hlock_class(this)->usage_traces[new_bit] = save_trace()))
			return 0;
	}

	if (new_bit < LOCK_USED) {
		ret = mark_lock_irq(curr, this, new_bit);
		if (!ret)
			return 0;
	}

unlock:
	graph_unlock();

	/*
	 * We must printk outside of the graph_lock:
	 */
	if (ret == 2) {
		printk("\nmarked lock as {%s}:\n", usage_str[new_bit]);
		print_lock(this);
		print_irqtrace_events(curr);
		dump_stack();
	}

	return ret;
}

static inline short task_wait_context(struct task_struct *curr)
{
	/*
	 * Set appropriate wait type for the context; for IRQs we have to take
	 * into account force_irqthread as that is implied by PREEMPT_RT.
	 */
	if (lockdep_hardirq_context()) {
		/*
		 * Check if force_irqthreads will run us threaded.
		 */
		if (curr->hardirq_threaded || curr->irq_config)
			return LD_WAIT_CONFIG;

		return LD_WAIT_SPIN;
	} else if (curr->softirq_context) {
		/*
		 * Softirqs are always threaded.
		 */
		return LD_WAIT_CONFIG;
	}

	return LD_WAIT_MAX;
}

static int
print_lock_invalid_wait_context(struct task_struct *curr,
				struct held_lock *hlock)
{
	short curr_inner;

	if (!debug_locks_off())
		return 0;
	if (debug_locks_silent)
		return 0;

	pr_warn("\n");
	pr_warn("=============================\n");
	pr_warn("[ BUG: Invalid wait context ]\n");
	print_kernel_ident();
	pr_warn("-----------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("other info that might help us debug this:\n");

	curr_inner = task_wait_context(curr);
	pr_warn("context-{%d:%d}\n", curr_inner, curr_inner);

	lockdep_print_held_locks(curr);

	pr_warn("stack backtrace:\n");
	dump_stack();

	return 0;
}

/*
 * Verify the wait_type context.
 *
 * This check validates we take locks in the right wait-type order; that is it
 * ensures that we do not take mutexes inside spinlocks and do not attempt to
 * acquire spinlocks inside raw_spinlocks and the sort.
 *
 * The entire thing is slightly more complex because of RCU, RCU is a lock that
 * can be taken from (pretty much) any context but also has constraints.
 * However when taken in a stricter environment the RCU lock does not loosen
 * the constraints.
 *
 * Therefore we must look for the strictest environment in the lock stack and
 * compare that to the lock we're trying to acquire.
 */
static int check_wait_context(struct task_struct *curr, struct held_lock *next)
{
	u8 next_inner = hlock_class(next)->wait_type_inner;
	u8 next_outer = hlock_class(next)->wait_type_outer;
	u8 curr_inner;
	int depth;

	if (!next_inner || next->trylock)
		return 0;

	if (!next_outer)
		next_outer = next_inner;

	/*
	 * Find start of current irq_context..
	 */
	for (depth = curr->lockdep_depth - 1; depth >= 0; depth--) {
		struct held_lock *prev = curr->held_locks + depth;
		if (prev->irq_context != next->irq_context)
			break;
	}
	depth++;

	curr_inner = task_wait_context(curr);

	for (; depth < curr->lockdep_depth; depth++) {
		struct held_lock *prev = curr->held_locks + depth;
		u8 prev_inner = hlock_class(prev)->wait_type_inner;

		if (prev_inner) {
			/*
			 * We can have a bigger inner than a previous one
			 * when outer is smaller than inner, as with RCU.
			 *
			 * Also due to trylocks.
			 */
			curr_inner = min(curr_inner, prev_inner);
		}
	}

	if (next_outer > curr_inner)
		return print_lock_invalid_wait_context(curr, next);

	return 0;
}

#else /* CONFIG_PROVE_LOCKING */

static inline int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return 0;
}

static inline int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	return 0;
}

static inline int check_wait_context(struct task_struct *curr,
				     struct held_lock *next)
{
	return 0;
}

#endif /* CONFIG_PROVE_LOCKING */

/*
 * Initialize a lock instance's lock-class mapping info:
 */
void lockdep_init_map_type(struct lockdep_map *lock, const char *name,
			    struct lock_class_key *key, int subclass,
			    u8 inner, u8 outer, u8 lock_type)
{
	int i;

	for (i = 0; i < NR_LOCKDEP_CACHING_CLASSES; i++)
		lock->class_cache[i] = NULL;

#ifdef CONFIG_LOCK_STAT
	lock->cpu = raw_smp_processor_id();
#endif

	/*
	 * Can't be having no nameless bastards around this place!
	 */
	if (DEBUG_LOCKS_WARN_ON(!name)) {
		lock->name = "NULL";
		return;
	}

	lock->name = name;

	lock->wait_type_outer = outer;
	lock->wait_type_inner = inner;
	lock->lock_type = lock_type;

	/*
	 * No key, no joy, we need to hash something.
	 */
	if (DEBUG_LOCKS_WARN_ON(!key))
		return;
	/*
	 * Sanity check, the lock-class key must either have been allocated
	 * statically or must have been registered as a dynamic key.
	 */
	if (!static_obj(key) && !is_dynamic_key(key)) {
		if (debug_locks)
			printk(KERN_ERR "BUG: key %px has not been registered!\n", key);
		DEBUG_LOCKS_WARN_ON(1);
		return;
	}
	lock->key = key;

	if (unlikely(!debug_locks))
		return;

	if (subclass) {
		unsigned long flags;

		if (DEBUG_LOCKS_WARN_ON(!lockdep_enabled()))
			return;

		raw_local_irq_save(flags);
		lockdep_recursion_inc();
		register_lock_class(lock, subclass, 1);
		lockdep_recursion_finish();
		raw_local_irq_restore(flags);
	}
}
EXPORT_SYMBOL_GPL(lockdep_init_map_type);

struct lock_class_key __lockdep_no_validate__;
EXPORT_SYMBOL_GPL(__lockdep_no_validate__);

static void
print_lock_nested_lock_not_held(struct task_struct *curr,
				struct held_lock *hlock,
				unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("==================================\n");
	pr_warn("WARNING: Nested lock was not taken\n");
	print_kernel_ident();
	pr_warn("----------------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("\nbut this task is not holding:\n");
	pr_warn("%s\n", hlock->nest_lock->name);

	pr_warn("\nstack backtrace:\n");
	dump_stack();

	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static int __lock_is_held(const struct lockdep_map *lock, int read);

/*
 * This gets called for every mutex_lock*()/spin_lock*() operation.
 * We maintain the dependency maps and validate the locking attempt:
 *
 * The callers must make sure that IRQs are disabled before calling it,
 * otherwise we could get an interrupt which would want to take locks,
 * which would end up in lockdep again.
 */
static int __lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check, int hardirqs_off,
			  struct lockdep_map *nest_lock, unsigned long ip,
			  int references, int pin_count)
{
	struct task_struct *curr = current;
	struct lock_class *class = NULL;
	struct held_lock *hlock;
	unsigned int depth;
	int chain_head = 0;
	int class_idx;
	u64 chain_key;

	if (unlikely(!debug_locks))
		return 0;

	if (!prove_locking || lock->key == &__lockdep_no_validate__)
		check = 0;

	if (subclass < NR_LOCKDEP_CACHING_CLASSES)
		class = lock->class_cache[subclass];
	/*
	 * Not cached?
	 */
	if (unlikely(!class)) {
		class = register_lock_class(lock, subclass, 0);
		if (!class)
			return 0;
	}

	debug_class_ops_inc(class);

	if (very_verbose(class)) {
		printk("\nacquire class [%px] %s", class->key, class->name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		printk(KERN_CONT "\n");
		dump_stack();
	}

	/*
	 * Add the lock to the list of currently held locks.
	 * (we dont increase the depth just yet, up until the
	 * dependency checks are done)
	 */
	depth = curr->lockdep_depth;
	/*
	 * Ran out of static storage for our per-task lock stack again have we?
	 */
	if (DEBUG_LOCKS_WARN_ON(depth >= MAX_LOCK_DEPTH))
		return 0;

	class_idx = class - lock_classes;

	if (depth) { /* we're holding locks */
		hlock = curr->held_locks + depth - 1;
		if (hlock->class_idx == class_idx && nest_lock) {
			if (!references)
				references++;

			if (!hlock->references)
				hlock->references++;

			hlock->references += references;

			/* Overflow */
			if (DEBUG_LOCKS_WARN_ON(hlock->references < references))
				return 0;

			return 2;
		}
	}

	hlock = curr->held_locks + depth;
	/*
	 * Plain impossible, we just registered it and checked it weren't no
	 * NULL like.. I bet this mushroom I ate was good!
	 */
	if (DEBUG_LOCKS_WARN_ON(!class))
		return 0;
	hlock->class_idx = class_idx;
	hlock->acquire_ip = ip;
	hlock->instance = lock;
	hlock->nest_lock = nest_lock;
	hlock->irq_context = task_irq_context(curr);
	hlock->trylock = trylock;
	hlock->read = read;
	hlock->check = check;
	hlock->hardirqs_off = !!hardirqs_off;
	hlock->references = references;
#ifdef CONFIG_LOCK_STAT
	hlock->waittime_stamp = 0;
	hlock->holdtime_stamp = lockstat_clock();
#endif
	hlock->pin_count = pin_count;

	if (check_wait_context(curr, hlock))
		return 0;

	/* Initialize the lock usage bit */
	if (!mark_usage(curr, hlock, check))
		return 0;

	/*
	 * Calculate the chain hash: it's the combined hash of all the
	 * lock keys along the dependency chain. We save the hash value
	 * at every step so that we can get the current hash easily
	 * after unlock. The chain hash is then used to cache dependency
	 * results.
	 *
	 * The 'key ID' is what is the most compact key value to drive
	 * the hash, not class->key.
	 */
	/*
	 * Whoops, we did it again.. class_idx is invalid.
	 */
	if (DEBUG_LOCKS_WARN_ON(!test_bit(class_idx, lock_classes_in_use)))
		return 0;

	chain_key = curr->curr_chain_key;
	if (!depth) {
		/*
		 * How can we have a chain hash when we ain't got no keys?!
		 */
		if (DEBUG_LOCKS_WARN_ON(chain_key != INITIAL_CHAIN_KEY))
			return 0;
		chain_head = 1;
	}

	hlock->prev_chain_key = chain_key;
	if (separate_irq_context(curr, hlock)) {
		chain_key = INITIAL_CHAIN_KEY;
		chain_head = 1;
	}
	chain_key = iterate_chain_key(chain_key, hlock_id(hlock));

	if (nest_lock && !__lock_is_held(nest_lock, -1)) {
		print_lock_nested_lock_not_held(curr, hlock, ip);
		return 0;
	}

	if (!debug_locks_silent) {
		WARN_ON_ONCE(depth && !hlock_class(hlock - 1)->key);
		WARN_ON_ONCE(!hlock_class(hlock)->key);
	}

	if (!validate_chain(curr, hlock, chain_head, chain_key))
		return 0;

	curr->curr_chain_key = chain_key;
	curr->lockdep_depth++;
	check_chain_key(curr);
#ifdef CONFIG_DEBUG_LOCKDEP
	if (unlikely(!debug_locks))
		return 0;
#endif
	if (unlikely(curr->lockdep_depth >= MAX_LOCK_DEPTH)) {
		debug_locks_off();
		print_lockdep_off("BUG: MAX_LOCK_DEPTH too low!");
		printk(KERN_DEBUG "depth: %i  max: %lu!\n",
		       curr->lockdep_depth, MAX_LOCK_DEPTH);

		lockdep_print_held_locks(current);
		debug_show_all_locks();
		dump_stack();

		return 0;
	}

	if (unlikely(curr->lockdep_depth > max_lockdep_depth))
		max_lockdep_depth = curr->lockdep_depth;

	return 1;
}

static void print_unlock_imbalance_bug(struct task_struct *curr,
				       struct lockdep_map *lock,
				       unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================\n");
	pr_warn("WARNING: bad unlock balance detected!\n");
	print_kernel_ident();
	pr_warn("-------------------------------------\n");
	pr_warn("%s/%d is trying to release lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no more locks to release!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static noinstr int match_held_lock(const struct held_lock *hlock,
				   const struct lockdep_map *lock)
{
	if (hlock->instance == lock)
		return 1;

	if (hlock->references) {
		const struct lock_class *class = lock->class_cache[0];

		if (!class)
			class = look_up_lock_class(lock, 0);

		/*
		 * If look_up_lock_class() failed to find a class, we're trying
		 * to test if we hold a lock that has never yet been acquired.
		 * Clearly if the lock hasn't been acquired _ever_, we're not
		 * holding it either, so report failure.
		 */
		if (!class)
			return 0;

		/*
		 * References, but not a lock we're actually ref-counting?
		 * State got messed up, follow the sites that change ->references
		 * and try to make sense of it.
		 */
		if (DEBUG_LOCKS_WARN_ON(!hlock->nest_lock))
			return 0;

		if (hlock->class_idx == class - lock_classes)
			return 1;
	}

	return 0;
}

/* @depth must not be zero */
static struct held_lock *find_held_lock(struct task_struct *curr,
					struct lockdep_map *lock,
					unsigned int depth, int *idx)
{
	struct held_lock *ret, *hlock, *prev_hlock;
	int i;

	i = depth - 1;
	hlock = curr->held_locks + i;
	ret = hlock;
	if (match_held_lock(hlock, lock))
		goto out;

	ret = NULL;
	for (i--, prev_hlock = hlock--;
	     i >= 0;
	     i--, prev_hlock = hlock--) {
		/*
		 * We must not cross into another context:
		 */
		if (prev_hlock->irq_context != hlock->irq_context) {
			ret = NULL;
			break;
		}
		if (match_held_lock(hlock, lock)) {
			ret = hlock;
			break;
		}
	}

out:
	*idx = i;
	return ret;
}

static int reacquire_held_locks(struct task_struct *curr, unsigned int depth,
				int idx, unsigned int *merged)
{
	struct held_lock *hlock;
	int first_idx = idx;

	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return 0;

	for (hlock = curr->held_locks + idx; idx < depth; idx++, hlock++) {
		switch (__lock_acquire(hlock->instance,
				    hlock_class(hlock)->subclass,
				    hlock->trylock,
				    hlock->read, hlock->check,
				    hlock->hardirqs_off,
				    hlock->nest_lock, hlock->acquire_ip,
				    hlock->references, hlock->pin_count)) {
		case 0:
			return 1;
		case 1:
			break;
		case 2:
			*merged += (idx == first_idx);
			break;
		default:
			WARN_ON(1);
			return 0;
		}
	}
	return 0;
}

static int
__lock_set_class(struct lockdep_map *lock, const char *name,
		 struct lock_class_key *key, unsigned int subclass,
		 unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	struct lock_class *class;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	lockdep_init_map_waits(lock, name, key, 0,
			       lock->wait_type_inner,
			       lock->wait_type_outer);
	class = register_lock_class(lock, subclass, 0);
	hlock->class_idx = class - lock_classes;

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged))
		return 0;
	return 1;
}

static int __lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	WARN(hlock->read, "downgrading a read lock");
	hlock->read = 1;
	hlock->acquire_ip = ip;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/* Merging can't happen with unchanged classes.. */
	if (DEBUG_LOCKS_WARN_ON(merged))
		return 0;

	º    ‰ĞÃ´&    º@  ‰ĞÃ´&    ºÀ   ‰ĞÃ´&    º   éIÿÿÿ¶    º    é9ÿÿÿ¶    º    é)ÿÿÿ¶    èüÿÿÿWVS…ÒxT‰Æú   ‰×‰Óƒú ‰NÑ‰Ó‰N‰Ø[^_Ã»   ¿   ºÀ ‰øèüÿÿÿ‰F…Àtø‰F‰Ø[^_Ã´&    »êÿÿÿ‰Ø[^_Ã»ôÿÿÿë·´&    ´&    èüÿÿÿUW‰ÏV‰ÖS‰Ã¸    ƒìèüÿÿÿ‹C(‰$…À„™  …ÿt	9{…‚  ‹C,ƒø"‡Í   ¾€    ‰D$…Àˆº   ‹<$ˆ„   Å4  ‹n‰D$‰ú‰L$Â‹C9h…   ‹DÏ9Âué!  f‹ 9Â„  ;Xuñ‰ğ‡C‹$‰D$1Àèüÿÿÿ‰Æ‹D$8  ‰D$…öuéÕ   ¶    ‹$‰ğèüÿÿÿ‰Æ…À„»   †T  èüÿÿÿ„Àuİ‰õ1É…Ét5‹D$öD…u*‹mh…íuê»êÿÿÿ¸    èüÿÿÿ‰ØƒÄ[^_]Ã´&    ‹D$| ‹D$‹DÅ9ÇuëÀ´&    v 9Ót ƒÁ‹ 9Çt©ƒx ‹Puê…Òtíƒz tç9Óuà‹|$‹CIÁâ”¾Ø  ‰B‹$‰ğèüÿÿÿ‰Æ…À…Eÿÿÿ‹D$1Ûèüÿÿÿéfÿÿÿ»şÿÿÿé\ÿÿÿ»ÿÿÿÿéRÿÿÿ»½ÿÿÿéHÿÿÿv èüÿÿÿU‰ÕWVSƒì‹€ä  d‹    ‰T$1ÒÇD$    ‹œˆØ  èüÿÿÿèüÿÿÿ{d¡    ‹°p  ‰t$t$‰°p  ‹[…Ût:f‰|$>t& ‹K$S0‰èèüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÈ‹|$d¡    ‰¸p  èüÿÿÿèüÿÿÿ‹D$‹T$d+    unƒÄ[^_]Ãt& èüÿÿÿS0‹s$‰D$‰èèüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ‹L$1Ò)ÈCSƒC÷$   „MÿÿÿûéGÿÿÿèüÿÿÿ´&    v èüÿÿÿU‰ÅWVSƒìd¡    ‰D$‹…ä  ÇD$    ‹œØ  èüÿÿÿèüÿÿÿ{d¡    ‹°p  ‰t$t$‰°p  ‹[…Ût=t& ‰|$>t& ‹K$S0‰èèüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÈ‹|$d¡    ‰¸p  èüÿÿÿèüÿÿÿ‹D$‹T$d+    unƒÄ[^_]Ãt& èüÿÿÿS0‹s$‰D$‰èèüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ‹L$1Ò)ÈCSƒC÷$   „MÿÿÿûéGÿÿÿèüÿÿÿ´&    v èüÿÿÿUWV1öS‰Ã‰Ğ‰Êì¬   d‹    ‰Œ$¨   1É‹Œ$À   ÇD$    ÇD$     ‹¬$Ä   ‰L$$·K‰\$ƒá÷‰D$fƒù…«   …À„.  ‹ƒä  ÇD$    ‹œØ  èüÿÿÿèüÿÿÿ{d¡    ‹p  ‰T$T$‰p  ‹s…ötBv ‰|$>t& ‹N$V0D$èüÿÿÿ‰ÃöÃu|$ ğÿÿwÇD$ÿÿÿÿƒÇÑë	] ‹7…öuÁ‹|$d¡    ‰¸p  èüÿÿÿèüÿÿÿ‹t$‹„$¨   d+    …   Ä¬   ‰ğ[^_]ÃfèüÿÿÿV0‹^$‰D$D$èüÿÿÿ‹v‰Ãèüÿÿÿ4…    œ$úƒFƒƒV èüÿÿÿ‹L$1Ò)ÈFVƒF÷$   „=ÿÿÿûé7ÿÿÿt& t$(¹    ‰÷‰t$ó«é¼şÿÿèüÿÿÿ´&    ¶    èüÿÿÿƒøQtfwƒøtOº    ƒøPu%‰ĞÃf=º   t)=»   uºà  ‰ĞÃ´&    v º    ƒøtÑéüÿÿÿº   ëÄ´&    fº    ë´´&    fº    ë¤´&    fèüÿÿÿU1íWV²„   S‰Ã‰Ğ8  ‰ßƒì‰t$4Õ4  ‰T$‰L$‰4$‰D$‰è…ít‹t$öD·tD‹$1Ò4‹D$‹DÇ9Æuë(ƒÂ‹ 9Æt‹H…Éuğ‹H…Étì‹I…Éuâ‹ 9Æuåt& D ‰Å‹h…ÿu¤ºÀ  èüÿÿÿ‰Æ…À„‡   ‹D$1Ò8  ‰D$…Ò~$‹D$öDƒu‹[h…Ûuê‹D$‰01ÀƒÄ[^_]Ãf‹D$‹<$‹DÃ,;9Åu'ëÓt& <RƒÂ‰L¾R‹x‰9‹x‰y‹ 9èt®‹H…ÉuØ‹H…Étì‹I…ÉtåëÈ¸ôÿÿÿëŸ´&    fèüÿÿÿUW‰×‰ÂV‰Æ1ÀSèüÿÿÿ…Àt=‰Ãt& ƒT  èüÿÿÿ„Àu‹P  ‰ú‰Øè‚şÿÿ‰Å…À…ˆ   ‰Ø‰òèüÿÿÿ‰Ã…ÀuÉ‰ò1ÀÇô   èüÿÿÿ‰Ã…ÀuëVf‹ƒP  …Àu,‰Ø‰òèüÿÿÿ‰Ã…Àt;ƒT  èüÿÿÿ„ÀuØ‹“P  ‹D»‰T»èüÿÿÿ‰Ø‰òÇƒP      èüÿÿÿ‰Ã…ÀuÅ1í[‰è^_]Ãv ‰ò1Àèüÿÿÿ‰Ã…Àtç‹ƒP  èüÿÿÿ‰Ø‰òÇƒP      èüÿÿÿ‰Ã…ÀuÜ[‰è^_]Ãt& èüÿÿÿUWV‰Æ‰ĞSƒì‹T$(‰$ƒú"‡    ‰Ë¾Š    …Éˆ   ¹„   ‘8  ‰|$Áç‰|$|>‰T$‹T–…Àt…Ûudƒâuo‹T$½şÿÿÿ‹DÖ9ÇtS‹\Öû ğÿÿ‡ü   ‹CÇC    ‰ÊÇC    ‰D$‰ğèPşÿÿ‰Å…Àtz‹D$‰C‹$‰CƒÄ‰è[^_]Ãv ½êÿÿÿƒÄ‰è[^_]Ã‹$½êÿÿÿ‰Ó	Ãtç‹\$‹\Ş9ßuët& ‹9ßt;Cuõ;S„rÿÿÿ‹9ßuìƒÄ½şÿÿÿ[‰è^_]Ã´&    ‹C‹‰A‰‰ØÇ   ÇC"  èüÿÿÿ‹D$‹DÆ9Çt&‹D$…Àtèüÿÿÿ‹D$àûÿÿèüÿÿÿ‰èƒÄ[^_]Ã‹D$ÇD†    ëÌ‰İé@ÿÿÿ´&    t& èüÿÿÿVS‰Ã¸    èüÿÿÿ‹C(…ÀtVÿs,1Ò‰ÙèJşÿÿZ…ÀuU‹s(ÇC(    ¸    èüÿÿÿöF,t[^Ã´&    èüÿÿÿ‹F¨u,dÿ[^éüÿÿÿ´&    v [¸    ^éüÿÿÿt& ë§t& ‹Fğƒ(uÎ‹VF‹Rèüÿÿÿë¾´&    ´&    èüÿÿÿ‹P(…Òté?ÿÿÿ´&    Ã´&    èüÿÿÿ‹P(…Òtèÿÿÿ1ÀÃ´&    t& èüÿÿÿƒøk„Â   wƒø9t[¹    ƒø`u!‰ÈÃ¶    ƒølt3ƒøzuV¹`   ‰ÈÃ¶    ƒø1u[ƒz¹    ¸    EÈëÇ´&    ¹    ë¸´&    fƒz¹    ¸    EÈëœt& =º   tQ=»   u2¹à  ë‚v ƒøQt[wáƒøtD¹    ƒøP„eÿÿÿ¹    ƒø„Wÿÿÿéüÿÿÿv ¹    éEÿÿÿ¶    ¹   é5ÿÿÿ¶    ¹    é%ÿÿÿ¶    ¹    éÿÿÿ¶    èüÿÿÿU½    W‰ÇVSƒì‰D$@ì‰D$¸    èüÿÿÿ‰ø¿Øşÿÿ-„  ‰D$´&    ‹‹39ûu'éƒ   v ÇB(    ‰Ø‰óèüÿÿÿ‰èèüÿÿÿ‹9ştc‰Æ‹C‰F‰0‹CÇ   ÇC"  …Àtèüÿÿÿ‹S…ÒtÀ‹J(öA,u°‰L$‰$èüÿÿÿ‹L$‹$‹A¨…Â   dÿ‰$èüÿÿÿ‹$ë‚f‹t$ƒÅƒÇ‹èüÿÿÿ‰ğƒÀ‰D$ı¸   …Jÿÿÿ‹D$‹|$‹@ì‹Xäqä9D$t‰Øèüÿÿÿ‰Ø‰óèüÿÿÿ‹FpäC9øuã¸    èüÿÿÿ‹D$‹˜ûÿÿ…Ûtèüÿÿÿ‹ƒT  ¨utdÿèüÿÿÿ‹[h…Ûuâ‹|$Gøèüÿÿÿö‡Ğúÿÿt.ƒÄ[^_]Ã‹Ağƒ(…4ÿÿÿA‹I‰$‹Ièüÿÿÿ‹$éÿÿÿèüÿÿÿ‹D$‹€¬úÿÿ¨u4dÿƒÄ[^_]éüÿÿÿ‹ƒX  ğƒ(uƒ‹“X  ƒT  ‹Rèüÿÿÿéjÿÿÿ‹D$‹€°úÿÿğƒ(u¿‹L$‹‘°úÿÿ¬úÿÿ‹Rèüÿÿÿë¥´&    fèüÿÿÿUWVSƒìH‹t$\‰$‰L$‰t$d¡    ‰D$D‹D$dÇD$4    ÇD$8    ÇD$<    ÇD$@    ƒà‰D$ƒø„_  ‹D$dƒàƒø„O  ‰×…öt	Ñ…A  ‹D$d‹t$Áèƒà…ö•Â8Ğ…&ELF                             4     (               èüÿÿÿ‹@\èüÿÿÿ1ÀÃèüÿÿÿV¹   S‰ÃƒÃpƒìd¡    ‰D$‹C$jT$èüÿÿÿ‹C$¶t$¹   jT$
èüÿÿÿ‰ğ¶ğV¶D$PSh    èüÿÿÿƒÄ‹D$d+    uƒÄ1À[^Ãèüÿÿÿ´&    èüÿÿÿUW‰ÏV‰ÖS‰Ãƒì‹«”   d¡    ‰D$1ÀˆL$‹    ˆT$…É   j ¹   ‰èT$èüÿÿÿZƒø…üÿÿÿ‹D$d+    uƒÄ[^_]Ãèüÿÿÿèüÿÿÿ‹    VS‰Ã¶°À   …É>   ‰ñ‰Ø1ÒƒÉ¶Éèaÿÿÿ‰ñ‰Ø1Òáï   èPÿÿÿ1À[^Ã´&    t& èüÿÿÿUWVS‰Ã‹@‹P‹Rèüÿÿÿ%   =   „S   ¸ûÿÿÿ[^_]Ã´&    ¶    èüÿÿÿU‰ÕÁåWƒåÀV4S‰é‰Ãƒæ ˆ‹Ñ   ‰ñ‰ĞÁâˆ‹Ò   ¶‹À   Áàƒâ€	ÖˆƒĞ   ‰Ïˆ“Ó   ƒá?1Òƒç_	Á‰Ø	÷¶³È   ¶Éè˜şÿÿ‰øº   ƒæ?¶È‰Øè„şÿÿ	îº   ‰ğ¶È‰Øèqşÿÿ‹    ¶³À   …Éà   ‰ñ‰Ø1ÒƒÉ¶ÉèKşÿÿ‰ñ‰Ø1Òáï   è:şÿÿ1À[^_]Ã       P                                                                                                                                       P        6%s: Status: SA00=0x%02x SA01=0x%02x
  7%s: write reg: %02X val: %02X
    3%s: I/O error write 0x%02x/0x%02x
    7%s: changed input or channel
 6%s %d-%04x: chip found @ 0x%x (%s)
 ‰øƒÃp¶ø‰ğ¶ğWVShL   èüÿÿÿƒÄéâ   ‰ø¶ÀP¶ÂPCpPh(   èüÿÿÿƒÄéÆ   @pPht   èüÿÿÿXZé  ‹S·CŠ  Q QPÿ²   ‹CTÿ0h”   èüÿÿÿC¹À  ºÔ   èüÿÿÿ‰ÅƒÄ…Àu
¸ôÿÿÿéz  ¹`   ‰Ú½À   1Ûèüÿÿÿ¾    ¹   ó¥¶ŒÀ   ‰Ú‰èƒÃèŒ   ƒûuç1Àéz  CpPht   èüÿÿÿXZé0                    ¸HÒæ¯  ^ Ğ €              upd64031a                                                          à       €                                                                                                                                                                                                                               debug èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿupd64031a parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=T. Adachi, Takeru KOMORIYA, Hans Verkuil description=uPD64031A driver  P          ¤ÿ      GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ                                                                          x                  	 3      p     ¤           C       B    	 X      E     n   B       	               ‰   P  3     ™   W       	 ®   `         ¼     Í     Ğ   ä       	               é            ÿ       €           
                   &      0     3     0     F  à   ,     Z  €   P     n           ƒ          œ           ª  P       ¼  0        Ó  <   0     é  l                                  -             ?             Y             a             r             ‹             ˜             ­             »             Ï           Û             ê      
     ù      0                   upd64031a.c upd64031a_remove upd64031a_log_status upd64031a_write upd64031a_write.cold upd64031a_s_frequency upd64031a_s_frequency.cold upd64031a_probe upd64031a_probe.cold upd64031a_ops upd64031a_s_routing upd64031a_s_routing.cold upd64031a_driver_init upd64031a_driver upd64031a_driver_exit upd64031a_id upd64031a_core_ops upd64031a_tuner_ops upd64031a_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev __stack_chk_guard i2c_transfer_buffer_flags _printk __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__upd64031a_id_device_table param_ops_int    $  	   %     $  &   &  8   '  P   '  b     g   (  u   &  „   )  ‘   $  ª   &  º     Ø   '  í   &  ü   )    $      Q  $  e  *  ‘  $  !    Æ     â         u    0                   h                                                 (  1     6   (  G     L   (  v     {   (     +  ¥     ´   ,  ¹     Ô     é     î   (       >     S           à     õ     `     d     l     €     è             $          -     .          0           -     3        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rel.text.unlikely .rel.rodata .rel.init.text .rel.exit.text .rodata.str1.1 .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   ]                    	   @       Ä  Ø               )                €                   %   	   @       œ                  /                                  8                                  4   	   @       ¼  8               E      2       <  º                 X             ö  ù                  T   	   @       ô  ¨      	         k                V                  g   	   @       œ  0               w             V                    s   	   @       Ì                  †             j  
                  ‚   	   @       ì                 ‘      2       t  
                               ~  ‰                  ­                                 ©   	   @       ü                  µ      0                          ¾              /                     Î             0  (                                X  @     $         	              ˜
  ,                                 á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ƒ|$`"‡  ‹D$`¾€    ‰D$…Àˆ  ˜„   İ    ‰\$q‰L$$‰ó‰t$‹4$‰õ‹vhİ…ötz8  ‰l$ ‰D$‹D$‹l†‰êƒâuZ‹D$‹D$‹DÆ9Èuë>´&    ƒÂ‹ 9Át‹X…Ûuğ‹X…Ûtì‹[…Ûuâ‹ 9Áuåt& ƒú‡'  „(  ‹vh…öu—‹l$ ‹4$‹\$Ş‰D$‹€4  9Åt‹D$9„ì  …  ‹D$‹˜4  9İ„C  ‰Ø1Òët& ƒÂ‹ 9Åt‹p…öuğ‹H…Étì‹I…Éuâ‹ 9Åuåt& ƒú?‡1  öD$d„  ‹L$‹T$‰Ø´&    v …ÿt9xu9Ï…·   …Òt	;P„ª   ‹ 9ÅuÜ‹T$‹D$…ÒuéÕ   v ‹9İ„ª  ;Cuñû ğÿÿ‡Î  ‰ù…ÿ„ë  ‹$‰\$‰Ë1ö‹€”   ‹P@‹@<‰D$(‹D$`‰T$,‰D$0‹C(‹„°x  …Àt1ÉT$(èüÿÿÿ‰D´4…À„w  ƒştj‹C(¾   ‹„°x  …ÀuÑëæt& ¾êÿÿÿ‹D$Dd+    …>  ƒÄH‰ğ[^_]ÃöD$d…¬  t& ‹D$‹€4  9Å…=ÿÿÿ1ÛéBÿÿÿ´&    ‹\$…Ûtu‹D$‹k‰{‹L$‹|$‰C‹D$4‰ú‰C‹D$8‰C‹$‰Œ¸ì  èüõÿÿ‰Æ…À…ú   …í„Ÿ   ‰èèüÿÿÿ‹<$‹L$`1ö‹D$<‰úèüÿÿÿ‹L$`‹D$@‰úèüÿÿÿé8ÿÿÿ¡   ¹   ºÀ  èüÿÿÿ‰Ã…À„C  ‹t$‹L$‰+‹†8  ‰8  ‰C‰‹D$‰{‹|$‰C‹D$4‰ú‰C‹D$8‰C‹$‰Œ¸ì  èUõÿÿ‰Æ…À…²   ‹D$$àûÿÿèüÿÿÿéUÿÿÿ´&    ‰ò‰Øèüÿÿÿ‰D´4= ğÿÿ‡¾   ‰D´<éhşÿÿ´&    ‹D$‹Hé	şÿÿt& …ít[‰k‹D$<ÇC    èüÿÿÿ‹D$@èüÿÿÿéPşÿÿv éØüÿÿ‰è‹l$ ¨…Õüÿÿ¾ÿÿÿÿé.şÿÿ‹D$¾şÿÿÿ…À…şÿÿ1Ûéœıÿÿ‹D$<èüÿÿÿ‹D$@èüÿÿÿ‹C‹‰B‰‰ØÇ   ÇC"  èüÿÿÿéáıÿÿ¾şÿÿÿé×ıÿÿ‹D$<¾ôÿÿÿèüÿÿÿ‹D$@èüÿÿÿé»ıÿÿ¾ùÿÿÿé±ıÿÿèüÿÿÿ‰Şé¥ıÿÿ´&    èüÿÿÿS‰Ãö@,tƒT  1Ò[éüÿÿÿt& èüÿÿÿ‹C¨udÿ èüÿÿÿƒT  1Ò[éüÿÿÿ¶    ‹Cğƒ ëŞ´&    ´&    èüÿÿÿU¹   ºĞ  WV‰ÆSƒìdd¡    ‰D$`1Àl$‰ïó«†T  ‰$hÀ  èüÿÿÿZ‰Ç…À…¶   ‹^h…Ût"èüÿÿÿ‹ƒT  ¨…ì   dÿ èüÿÿÿ‹[h…ÛuŞ†4  ì  ‰ ‰@ƒÀ9Áuô†H  ‰†H  ‰†L  ëƒÇƒÿtxL½ ‰ú‰ğ‰ëèÉñÿÿ…Àtål$`‹ƒÃèüÿÿÿ9ëuò‹^h…Ûtèüÿÿÿ‹ƒT  ¨u{dÿèüÿÿÿ‹[h…Ûuâ‹$¿ôÿÿÿèüÿÿÿ‹D$`d+    u{ƒÄd‰ø[^_]Ã´&    f1Û¶    ‹L ‹„Ø  “ô   ƒÃ‰L–èüÿÿÿƒûuŞ1ÿë²f‹ƒX  ğƒ éÿÿÿ‹ƒX  ğƒ(…xÿÿÿ‹“X  ƒT  ‹Rèüÿÿÿé_ÿÿÿèüÿÿÿv èüÿÿÿU‰ÕW‰ÏVS‰Ãƒì‹ èüÿÿÿ‰Æ= ğÿÿ‡Ê   ‹S‰ĞƒàƒøtE¸    ‹[‰$èüÿÿÿ‹$1É‰ğR‰úSj è›øÿÿ‰Ã¸    èüÿÿÿƒÄöF,thƒÄ‰Ø[^_]Ãt& ‹C‰ê1Éèüÿÿÿ‰Å= ğÿÿws‹S¸    ‹[‰$èüÿÿÿ‹$‰é‰ğR‰úSj è>øÿÿ‰Ã¸    èüÿÿÿƒÄ…ítŸ‰èèüÿÿÿöF,u˜èüÿÿÿ‹F¨uRdÿèüÿÿÿ‰ØƒÄ[^_]ÃƒÄ‰Ã‰Ø[^_]Ãt& öF,t
‰ëé]ÿÿÿv èüÿÿÿ‹F¨u-dÿèüÿÿÿ‰ëé?ÿÿÿt& ‹Fğƒ(u¨‹VF‹Rèüÿÿÿë˜‹Fğƒ(uÍ‹VF‹Rèüÿÿÿë½´&    ´&    èüÿÿÿW‰×V‰ÆS‹ èüÿÿÿ‰Ã= ğÿÿwt‹F‰ú1Éèüÿÿÿ‹v‰Ç¸    ÿ ğÿÿwfèüÿÿÿ‰ú1É‰ØVè…ñÿÿ‰Æ¸    èüÿÿÿZ…ÿt‰øèüÿÿÿöC,t‰ğ[^_Ãèüÿÿÿ‹C¨uDdÿèüÿÿÿ‰ğ[^_Ã¶    ‰Æ[‰ğ^_Ã´&    èüÿÿÿ1É1Ò‰ØVèñÿÿ‰Æ¸    èüÿÿÿXë£‹Cğƒ(u¶‹SC‹Rèüÿÿÿë¦´&    èüÿÿÿUWV‰ÆSƒì‹Nd¡    ‰D$1À…É…ó   ‹F‰Õèüÿÿÿ‰Ç= ğÿÿ‡Ô   ¡   ¹0   ºÀ èüÿÿÿ‰Ã…À„ß   U¹\  º   èüÿÿÿ‹F‰{(T$‰C,‰Øèüÿÿÿ‰ÆX…öuS‹s,¸    èüÿÿÿ1É1Ò‰øjVSèÿõÿÿ‰Æ¸    èüÿÿÿƒÄ‰à…öuJèüÿÿÿ‰Æ‹D$d+    uvƒÄ‰ğ[^_]Ãt& ‰ØèüÿÿÿöG,uØèüÿÿÿ‹G¨u/dÿèüÿÿÿëÂt& èüÿÿÿëØ´&    f‰Æë©t& ¾êÿÿÿë‹Gğƒ(uË‹WG‹Rèüÿÿÿë»¾ôÿÿÿëŸèüÿÿÿ´&    t& èüÿÿÿUWV‰ÖS‰Ãƒì d¡    ‰D$‹èüÿÿÿ‰Ç= ğÿÿ‡ò  ¸    èüÿÿÿ‹CÇ$êÿÿÿƒø"‡<  ¾€    …Àˆ-  ˆ„   ‹”‡ì  ‹k‰L$ŒÇ4  ‰L$‹Œ‡Ø  ‹C‰T$‰L$ƒà„D  ‰Èèüÿÿÿ‰D$¹   FT$èüÿÿÿ…À…¬  F¹   T$èüÿÿÿ…À…“  ‹C‰î…À„¢   …í„š   ‹L$‰$…É„’   9ÈƒC  ‰D$Ç$äÿÿÿöC…~  ‹D$1í‹\Ç‹D$9Øtc‰|$‰Çë@t& ‹@(¹   T$‹@ ‰D$‰ğèüÿÿÿ…À…  ƒÅ;l$„Ñ   ‹ƒÆ9ß„Ä   ‹C…Àu½‹C…Àt¶‹@ë±Ç$    ¸    èüÿÿÿöG,t!‹D$d+    …ÿ   ‹$ƒÄ [^_]Ãt& èüÿÿÿ‹G¨…¨   dÿèüÿÿÿëÅ¶    ‹L$‹TÏ‹L$9Ñth‰$‰Ëë
ƒÀ‹9Ót‹J…Éuğ‹J…Étì‹I…Éuâëã‹$éƒşÿÿ´&    ‰$éoÿÿÿ´&    ‹|$éNÿÿÿ´&    Ç$    é¼şÿÿt& 1ÀéDşÿÿ´&    f‹|$Ç$òÿÿÿéÿÿÿ‹Gğƒ(…Nÿÿÿ‹WG‹Rèüÿÿÿé;ÿÿÿ‹L$‹D$‰êèüÿÿÿ‰$éàşÿÿèüÿÿÿt& èüÿÿÿUWVS‰Ã¿Ûƒì d¡    ‰D$1À¿D$4‰L$‰T$Áà	Ø‰D$d‹    èüÿÿÿ‹ƒÈ  ‹T$8ÇD$    ‹@8‹´Ø  èüÿÿÿèüÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ÛtB~t& ‰|$>t& ‹K$S0D$èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$èüÿÿÿ‹D$d+    ukƒÄ ‰Ø[^_]Ãt& èüÿÿÿS0‹s$‰ÅD$èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „KÿÿÿûéEÿÿÿèüÿÿÿ´&    ´&    èüÿÿÿU‰ÅWV‰Î¹	   S‰ÓƒìP‹D$d|$(‰D$‹D$h‰D$d¡    ‰D$L1À‰T$$ºÀ  ‰l$ ÇD$    ÇD$    ó«‹D$lÇD$,   ‰t$<‹L$`‰D$@¸   èüÿÿÿ‰D$(…À„4  ‰ÁD$1ÒPD$0P‰Ø‹{èüÿÿÿ[_…À…  …öt‹D$‹…Ét‹D$‹…Ò…  èüÿÿÿ‹T$pd‹    ‹ƒÈ  ÇD$    ‹@8‹´Ø  èüÿÿÿèüÿÿÿ‹ƒp  ~‰D$D$‰ƒp  ‹^…Ût@¶    ‰|$>t& ‹K$S0D$ èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$èüÿÿÿ‹D$(èüÿÿÿƒûu‹D$8…À…™   ‹D$0èüÿÿÿ‹D$Ld+    …7  ƒÄP‰Ø[^_]Ãt& ÇD$,    éáşÿÿv èüÿÿÿS0‹s$‰ÅD$ èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „ÿÿÿûéÿÿÿv ‹|$‹èüÿÿÿ‹D$0‹L$‰‹D$4‰éPÿÿÿ´&    ‹L$`ºÀ  ¸   èüÿÿÿ¹   ‰D$0‰Â‹D$‹ 9ÈGÁ‰D$4…ÒtR‹L$‹1ƒøs$…À„.şÿÿ¶ˆ
¨„!şÿÿ·Lşf‰Lşéşÿÿ‹zƒçü‰
‹Lü‰Lü)ú)ÖÁéó¥éïıÿÿÇD$4    éâıÿÿèüÿÿÿ´&    t& èüÿÿÿUWV‰ÆSƒìp‹¶ä  ‰D$‹„$ˆ   ‰T$‹œ$„   1Ò‰D$‹„$Œ   ‰L$‰D$d¡    ‰D$l1À‰TLƒÀƒø rô1Ò1À‰T,ƒÀƒø rô‹D$‹|$L$L‰D$(‹D$‹‹ ‰D$4‹D$‹ ‰D$8¸   9ÂLĞD$(èŞÿÿ‰D$…ÀxN‹?‹l$,9Ç‰|$<Oø…ÿˆñ  ‰ú1É‰èèüÿÿÿ‰ù‰Ú‰èèüÿÿÿ‰Ç…ÿt=‹l$,ÇD$òÿÿÿD$L9Åt‰èèüÿÿÿ‹D$ld+    …í  ‹D$ƒÄp[^_]Ãt& ‹D$1Òèüÿÿÿ‹  ÇD$$    èüÿÿÿ{èüÿÿÿd¡    ‹ˆp  ‰L$L$ ‰ˆp  ‹[…Ût;‰|$ >t& ‹K$S0D$(èüÿÿÿ‰Æ…öu|$$ ğÿÿwÇD$$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$$‹D$èüÿÿÿ…Û…Ç   ‹D$<‹l$,ƒøÿ„Æ   Œÿşÿÿ;D$õşÿÿ‹T$4‹L$‰‹T$8‹L$‰…À„±   ‹L$‰L$L9Í„¬   ‹D$ÇD$    ‰(éËşÿÿt& èüÿÿÿS0‹s$‰ÅD$(èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „óşÿÿûéíşÿÿv ‰\$‹l$,éJşÿÿv ÇD$   é:şÿÿv é şÿÿÇD$    é#şÿÿºÀ èüÿÿÿ‰Ã…Àt%‹L$<‹T$,èüÿÿÿ‹D$ÇD$    ‰éşÿÿèüÿÿÿÇD$ôÿÿÿ‹l$,éİıÿÿ¶    èüÿÿÿUWV‰ÆSƒìh‹ä  1ö‰D$‹D$|‹¼$ˆ   ‰D$‹„$€   ‰D$d¡    ‰D$d1À‰tDƒÀƒø rô1ö1À‰t$ƒÀƒø rô‹D$‰T$,‰L$0L$D‰D$ d¡    ‰D$8‹„$„   ‰D$4‰ÂD$ èÛÿÿ‰D$…ÀˆX  …ÿuL‹D$èüÿÿÿ‰T$4…Òˆc  …À…[  ‹D$‰Ö‹l$$9ÂOğ1É‰è‰òèüÿÿÿ‹T$‰ñ‰èèüÿÿÿ…À…,  ‹D$1Òèüÿÿÿ‹›  ‰|$èüÿÿÿ{èüÿÿÿd¡    ‹ˆp  ‰L$L$‰ˆp  ‹[…Ût;‰|$>t& ‹K$S0D$ èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹t$‹D$èüÿÿÿ…öˆ÷   ‹\$4‹D$$;\$u…Ûxq‰t$t6¹   ‰Ú‰$èüÿÿÿ‹$‹D$‰Ùèüÿÿÿ…ÀuF‹D$4‹L$èüÿÿÿ…Éu5‹D$$L$D9Ètèüÿÿÿ‹D$dd+    …”   ‹D$ƒÄh[^_]Ã´&    ‹D$$ÇD$òÿÿÿëÁ´&    v èüÿÿÿS0‹s$‰ÅD$ èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „Ãşÿÿûé½şÿÿv ‰t$éJÿÿÿèüÿÿÿfèüÿÿÿUWV‰ÖS‰Ãƒì<‹|$T‹D$P‰|$d‹    ‰T$8‹“ä  ‰\$‹‰D$Ø‰t$ ‰D$‹D$X‰L$$‰D$‹²  ‰\$(ÇD$0    d‹    ÇD$4    ‰\$,èüÿÿÿèüÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ÛtC~¶    ‰|$>t& ‹K$S0D$èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹D$…Àx‹L$‹T$(;€   …Òux‹T$8d+    uyƒÄ<[^_]Ãv èüÿÿÿS0‹s$‰ÅD$èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „;ÿÿÿûé5ÿÿÿv ‰ë„¸òÿÿÿézÿÿÿèüÿÿÿ     cgroup_bpf_enabled_key  __cgroup_bpf_run_filter_skb  __cgroup_bpf_run_filter_sk  __cgroup_bpf_run_filter_sock_addr  __cgroup_bpf_run_filter_sock_ops                                                                        @   ğ  0  P  `  p  Ğ     °  @  À  ğ  `	  À  €                   p  °  ğ  °  0  p  €    À  à       0  P$  °$  @&  °'  (  à)  ,   .  ğ0   4   6  cgroup_id:	%llu
attach_type:	%d
      À      Ô  @        €      ¤         -  -      /  È/      T2   3      45  06      D7  È7      include/linux/skbuff.h / kernel/bpf/cgroup.c include/linux/thread_info.h °      ö	  ­     @       ƒ	 #     Õ 	 3  -   é 	¸$   èüÿÿÿ ›  “  Ş    ›$  &  &  s'  Œ'  s(  ³)  S,   ÿÿÿ	
ÿÿÿÿÿÿÿÿÿ strnlen           P      0                                          	                                                               @                           	                                                            	                                                             	                                                             	         
                                   °                                             
                                                                                                               À  `  à        p                       GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ              
              +             N              o             •   4          ½              ã   5            P          /            
 T  Q          €  s          ®             Ú  t            •          2             ]  @   ª    w  0      •  P       ­  `  
     Å  p  V     ä  Ğ  /     ú                           V     )  `  E    @  @  y                 cmd_drivers/media/i2c/upd64031a.o := gcc -Wp,-MMD,drivers/media/i2c/.upd64031a.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"upd64031a"' -DKBUILD_MODNAME='"upd64031a"' -D__KBUILD_MODNAME=kmod_upd64031a -c -o drivers/media/i2c/upd64031a.o drivers/media/i2c/upd64031a.c 

source_drivers/media/i2c/upd64031a.o := drivers/media/i2c/upd64031a.c

deps_drivers/media/i2c/upd64031a.o := \
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
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
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
  include/media/i2c/upd64031a.h \

drivers/media/i2c/upd64031a.o: $(deps_drivers/media/i2c/upd64031a.o)

$(deps_drivers/media/i2c/upd64031a.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      R  À  ¼     a      
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
    ‰                          ­             ¾             Ë   4      î             û                6  ›    0  ,        D  0        \  œ        o           †  ¼       š  Ô        cgroup.c __kstrtab_cgroup_bpf_enabled_key __kstrtabns_cgroup_bpf_enabled_key __ksymtab_cgroup_bpf_enabled_key __kstrtab___cgroup_bpf_run_filter_skb __kstrtabns___cgroup_bpf_run_filter_skb __ksymtab___cgroup_bpf_run_filter_skb __kstrtab___cgroup_bpf_run_filter_sk __kstrtabns___cgroup_bpf_run_filter_sk __ksymtab___cgroup_bpf_run_filter_sk __kstrtab___cgroup_bpf_run_filter_sock_addr __kstrtabns___cgroup_bpf_run_filter_sock_addr __ksymtab___cgroup_bpf_run_filter_sock_addr __kstrtab___cgroup_bpf_run_filter_sock_ops __kstrtabns___cgroup_bpf_run_filter_sock_ops __ksymtab___cgroup_bpf_run_filter_sock_ops sysctl_convert_ctx_access cg_sockopt_convert_ctx_access cg_sockopt_get_prologue bpf_cgroup_link_dealloc bpf_cgroup_link_fill_link_info cgroup_bpf_release_fn cgroup_bpf_release bpf_cgroup_link_show_fdinfo __bpf_prog_run_save_cb copy_sysctl_value sysctl_cpy_dir sysctl_cpy_dir.cold __func__.0 cgroup_dev_is_valid_access sysctl_is_valid_access cg_sockopt_is_valid_access sysctl_func_proto bpf_sysctl_get_new_value_proto bpf_set_retval_proto bpf_sysctl_get_name_proto bpf_sysctl_get_current_value_proto bpf_sysctl_set_new_value_proto bpf_get_retval_proto sockopt_alloc_buf cgroup_bpf_replace CSWTCH.197 cgroup_dev_func_proto compute_effective_progs update_effective_progs __cgroup_bpf_detach bpf_cgroup_link_release.part.0 bpf_cgroup_link_release bpf_cgroup_link_detach cg_sockopt_func_proto bpf_get_netns_cookie_sockopt_proto __cgroup_bpf_attach bpf_cgroup_link_lops __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_ops491 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_addr490 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sk489 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_skb488 __UNIQUE_ID___addressable_cgroup_bpf_enabled_key483 bpf_get_retval __fentry__ current_task bpf_set_retval bpf_get_netns_cookie_sockopt init_net kµdNİ7Ò=Œh#9ÃsıŠ½ÊéyOT±,[9!µ&{¬S_1$´Ô >¾¦Ïwú‘*-'HŒĞ(Š¥âNÄĞlVÁa!JĞ{zí÷çh–X¾Cãm'M’â‚Zìˆ©õšŞØÅInpL27ä&87^Vc%ù@Ë¤÷BZÑ–“8.nÆw5!}‡Gİ	sÊ×õ¥R!‹y3‡"¶@Æ²Ñé@£*V”Er”éTÜZOĞS·G®ÅìŒ»àµ»ı‘SõjìYqÏÈ…§ÀÂú&ìe6MËEÄ¤®hû!íQkÕ¤\KÈzoO<ñÉı@Uâ•x¸aT¢¼Ş"›”Es=™®@Üş«nö$_¶<)¹	ªh”ßj(Ì®¦"9²|“ª¹ÇŞ¢x'ícŸ=âüÛE•Y­rBàÔl3*`Ÿ2ZŒ&Dİ<H¸Ôı £5ó2H[ó&VaâVÿT~®kısêx¢5Ú—^2ÚrŸ§‡';DÙ1ÿÇFíâŸ§;Ë*?>±ÅEÆG.¥$_åiXÖ›ÀÑ%Eñîü@ôîWF–ê+O`C˜ä<K{B™gïsÄ	*
J&¹iûgÒ‚ê…N:ºdİ²Âx°¸SàPá\9
°-µ‡jp…È…Ç±òÚ€Ql¡^Ä|@ŒA"[ŠèˆvFŸhnpà$±2ß7 RµÛíÙŒrÛ1POÈ	Ëø©Ä<Øœa)0Æ;»WæB‰[‚t$	™¶İ›
KØ³¥Y™Dg(‘Øøoµ2×mÈKà5«Î‚*—´Š­Òµï9ˆô‹”ÇH;¯ûGÙù?ï‘ô–ĞóîyëÈåŒ0_·SxğK»fÿ÷¹óùÿ¼´[ËÇ%ıs×†L±@M8¶3†zµ“æ«*míÔÛáÛt3ºç‰û@	/¼=vØŒ+[ş?r’ì£»Ón÷Æ‘¨³Îi«„8‘ ó’İÕ§ù‡"cÜ@‰7ƒmX÷|T —/İZÛÕ¼Ë‘^ÎğN"ñì+ç­”=&	,-¨¶´xèÀ¹ÿÄÒ,~7\µCúë}jP08¢ëı¯š2í&+–&'Á>›;œü½/pî7#JòdNÃeù5€¨Lfãåæêˆaš¼,ø½ó~?ü|²">•Dk©ùRù×òy.Ú\Uà'FÁe½òÊmFgà½cF78puâDKtAÿl|u2nÃƒçÌbÌ‚ºì³¹Ğô&iU”wfK,OQšBğrä"ß”Diê¢óKœ™ i=šì×[¹ò[k
 EE§älZ+~†Ğ“ˆİ¿Ì‹ªoÉg7².¸…Û•ù¯IÇ9¨”MbûÖú­ñ©Tnü&êbˆ­O‹ĞBDTd‰6Ù‡Œ†ªsşî+H›ä>ÕİØT^şñ%Çk YšÚ‘±È;h¼Øa‰YwG¶~Æí¸£äXNÔ—K_]n²t=©ÅâëSzhÆ~×>«‡}crqÇmqf:ø0Ö@¥ÆBl5%!¬ÃÜåò·“›µjH0Cäš?ÄjÂ8B†Z¥ãóè™4B¬PÑ{0(c:”Hn5³•‡/OJ˜TuÍ¤ 7ôø‹±ùAöãBæ^ºÖPTuØbÇfˆÕ†`óiwJŞÀs¶¿æ|¦¦Î‰òì=*¸@sà·¹0›¾JæõÁ¡æp†àùèƒ¢{QMÈ´°’u°ÛÈi;6Ò×Bœ%SçïĞü‰¨¨¡£î“H)- Ó R+ºŞZ2a1Ê{ƒ—V>¼0%÷™²É$XìäğFU?‘÷uÔœsŒ,t¢î+ ŞıA†?dğ‘†ÀŸ‡o“À×¨ˆÎc7¾µ¸ë	‰AFàËSq¦êŞ“ê]]`²—Íéu8
ø˜a‚9¹h|AQXŠGU¡	Ÿ¢úV-&:¿.¤¹\Âˆà5â¯óİ3‚¯û[™_¤ \]é]¹ùóÓ-ƒ~÷w› rõâˆ».Ük=¡UD=‡@Š{½¦k%Ï¼š³…H[i|Qj›ãTÉÛÌİQ×–’¸şµÇIß]™ÖJ®ÿéùaşè£G‘ø…ÅË_O²ÔGŒ±X÷Ï›mLn»%¼c4I#ò<¿Kã	Üq¸¯Ò4V½Œrëñ,ã‰)ÕÈÊ,£ëµ&¦+§Ğ¹ÃêJo	¿°%†×´*˜7è	ÍæKö’¥Ôv¨ÄZn÷ÆµÓ¿\ò*LÔ½ûÀ™}Zò{ü¨qTÂ_Ñ2Gˆ~?’Â“–TcG¨V8Ø´öPÿìètÀû”ï2¶×ê¹7KÖ.5aŞİ‡0Ã,ƒ’tec®¹åÛ†lŒu"YöàFá¯§?r‚/³Q,
ş˜ã‹0š’}µjXš”™ì&ZûÅ;€–ã ±Q’æôj<,aÈ„¢Jù%*¯”1›ú‰Q2›²º.00-ÊánŸ»1Ôú`º çbKõ€! Á"¬½~RÈŒËEÇjø£Àq¢x¥F¶€ZÓGü!cUyØ|ÂqĞR¡…”ãZê¦Ùg¥~ÇŠO=¨IÀ±êÌ‹r‹İNõBœ5•XyÉmrüœ7Å!İÎø‹?`CØ°—>Î*  û¸òİÎVî€Èq
°Ó0š5—‹ªNY‰ÍrÊan¯0ÌKÜàùA¿{
ãz°QúÑÓ èŸvÜwmƒQ¦W€©	Y¸_Ë™\ä]–7Nqğ,y(÷.÷ò¤%¢
Êû<Ğ[ü’VĞ|et[¾RÆÉÒËÊ}­†U-ÑCË³iİŸ·E'á’¡@f$Í*’IMGrI~løa¶ÙîÚğ}ÃšmCdF9ø/µ€±ú©ÅêÜ¨J"P‰¨j`5iÏNMµb3à{nrÖ£ü!™œ÷‹’Ï’»R•êhUyÃjÇ1¯TÍT‘ùîLÁ7Ò
rQµ#šy£‚[zq"×Ã/$vöÚR.ªG“©$¢I˜ØEñIÚpU[…!òEtQhÀ€f
‹3½m_l?	Q†ô'VšÆ:“ğò¿BAœ€4áƒ~ÿ‰ı.ˆ(\·?ãÖŠˆµõ@?zT8¼[ÏûŒPì)0«â`ä¸¸Ÿ’ìâ[u×‚É’Ûù}†ƒ"]™o/óKß°tên*åÊ{hP–4WX-º„£?}àŞ¤^¬[WFĞñé×KÔq¨Îq’¦&ÁgƒRXŒ¤mÙ¾˜©(dM6(L;Š_æ>iŸéš†?GÁ…­š`pˆ#^©³q4t$m¶˜›´Ô·¤rô±)&%‚ÿ «íE@•Ûš4È Lõ¾&ëıŠX2ÒZ×áo:Z[ü#¾³İ…ªº‘“7<Ëå	ŞĞe<|P\nØä®™_W¥™ ˜G÷MhHXĞx=¤é+2zäÀæĞu@jíØ-aA‰Œ‚x‚(4OIËº+6I„&•ùe`×ÅÔ£R,ÂãDğ“Ô¼ÒÏD'¤¿X‹ŠmÆÊ»³À)—ã/”†=>¼ehnÉËó,µ3ÇÇ‡Ğ†YìH–#2¿jêa°ÇœÙÊ1†MÁ«QÓ¬ÚæÂiuFĞ6[26}Ãb.ã4pñc’|ªâal¼u¢†÷z1Ê™¶ª4zuóÈu'kÈ	—-1H!ò?>2âÙKËóñ]¨ğEîOT—¸‘g{»–2Ò9@Û¨pê½%]m£+›l <¢@HtŞ*
ØX#¼ë²{ğ»|b 4!˜”—7ŞùºBÎoc* b_wRäGãS7aò¯U–J†Ûnõï9‘1Ô3°$–SáXI\‘™G}z·CtQOÓ»PÇ©7‚)äø":?w¸@º×XÇOhôİ„¯xÇLÛ° Fµ}	K÷‚¨›ñ6iM^ôJS¶@ƒ&œÜöt~}hĞch||fIG)ÉÈLv'eh¡Æ¬çÇL)`§¡”ê˜¥‡B)\ä~şÉ$}ÿ©V6t¶Kß/Iµg¯ÍñMqŸÅSàïÇu%?‚ğK
\mœô‡ìÉ+·k
şO$ÂÙ<›úBbxŠ¿(5ÇÿÛ=·9UÊºµ!ûâ1¥/]ñ-0A¶Ò»P8#Dº4sÁŞRÕ½B¹ä…¥Ÿ^„ÈKãÖ×¿(á|O,h"¥:ÒÆ‡L¸tøÍnt§€€=VÃæ';Õú*õ«»7öEı:²º`|ïïÀ(átB¹A»SPtI](ŠÊJlGë@ÁĞ†Œóå-4™EÖ‡ĞHê £Lcktkqc~ºòWT4‘¹(Iîšè†‡½ƒš§d]»Áy"kº'N‡É¬¾÷f<”óP}Ã~ŠüW¹”Š‰‘@­º‚…å =èÂ&,óNY«OÄÛê´H@S>¨ÈÌÒÆñö»´¢¬ú‰³j{Ù)Y%!ÚA'9‰ÈrÏİé3ˆ°‰¥Ä£òÇ%÷ŞJàé¼«§éµè–ñ($^ã„vbÏ¶ü8¯-9hôÕ—ò™éˆ-Ô[MT
¦šÅÛU_ó†7dö‰¾¤§,ÎH÷íbn'`»¬ÕsB%Õì³|¼™Z=.j¶¶©‹blÃ¬\Â“¨ƒÖ$‰ÖXÜWÿÿ}?&ˆµ½çâ¡ÆAüãPúùA2’=ÑÉ|ó¦¹9°…®Z$…kJÁÑ£2Aä¡ààùI»mÿÙä-Ô)ÏÍN“UyÖÕ
Z!VÿƒR(ˆ]ŠQT%ì·W2fÓ¦	Š’^LÜÿ¤oB@ºò2o_$ßÂÿrn¶â	:ÜÂÖÂopÑ#õˆZã4nôNşm'œ†$óMŸ
ØÕ#–jµPóümöa b7iø:™&–õRd4Û~Ê×¤>Ÿ"AÂ3k!ŞéNhB’f¢B;…P7MmÂ„’ÀVë¬€E\D`3á¯’—làƒ#Hh>:=]o™˜‚#@¤m’ ®Ñá1C4Åtd9|ñæÑl³x±Ô&<úvóÔxWÀñDBÙ çŸså4pCë™Õ8˜{yLÖã—êííãÛDcÈu”újQ.¨8ÚÇLÕ¯o'÷ùâ@8TŠ„ŸËG¦á5›QŸeFjÔ…|Œ.È	õGa(æYŒ	`9V~Eğe„Óú¢øçüö²¯@kN,c±lù,6¸hÙJggù*>:ü‡»Ek5¯O™SMş$È7¦ìowÈÀÒ…¿øEÄÃ©›ÎŞj¤	ˆ?®gHÇĞ…wÏ>Vï-EÕÏƒ*º$‚ùeV0Z=Ë¡bĞjÖp–î’2=ôË,Sk"àW=2²ãT"b~Eo.—;®6Qä"ù9,^î£ÚG¡ıYPœŒ·Qz•Ã;0P_<2§,|Ô0Ã–F}Î¼C¬±ÀÖˆ™jôÅqOÄ•‘´;bM\”]8†KFø¢šV,¤|xÏ@èpì
×—LyŒö¿ƒaóğÁùPşKÖvßd³
‚º2Œ4eY§ tœÈc4ãÆ9Nëà®-Ô.Á{ó¿-3u q{ƒ¥¿ÎîcÔu‹Uö÷XÏï	Ù½·@Aç
AIc`İo•ÀK‹Ç>SG"c#on’,…©ÿ±1sùü‡mØn†(!~òŠ®SØÎªÀ²êÔ#V l¯)Ê¬ŠáOV[ØÄüÈRU•l¯Şnƒ›e6A£l}ç'u<t
?¢Êh)Y¿"ûäªuàéy,Àó]ÉiFspÎªxJ1tçÎ6ç/äadIxKÒ–ëu	‡´ÃíS=Ñô#4®À:ö]~j:
(«Õë¢×—Wş’Ãp#Ut\‚å|)•áİ cR§‘©¦ê"ï·ÿU2ÖÜãúCæls+q1UÄ.e^Î£bí}n5íU/¢«Y½‰ã™DH:5C›{#Â
¾¦yôùÔ Bh	ÅèôÀW´ú=5œûŠkÖPÚï<Ê5“xa	¥ƒYSm'TòŒ@LÄ“Ë	‡½¬™•iK»º%8ù ³Á{•ƒ*ˆî>÷<w‘l•ŞZY¾¼OåD˜ÔŠqUu:^ËéÌœVEæQ£%“†Ám1ÇŸ…qø {:z#cÏTiUÏ}) ç<M”.B_ 3JÖBÿ½xµËŞø˜¶i¯PlÀCM½;²pé¢XH‚JéøF&ı^eíXS1)ú±Î–³a¹nÀ[y\´¤ÇlNÃ¼.¸)dqĞu¦1Ş`¨ÄAÃ¿ÛÛ<0‚@åP/€:·Ïp¶¯Ş­E«ıİ æiı„33ƒwk¸8¶Õ"Š]XÕ¢ëŞ$¹Ì ßĞ}d 7€e¡aÊœ°alº¯Ÿ¼ÆO´¢ªók×«go32jn ¾ØA‰¬s—z(¥[a”7Ñ°•cW6ÓYÏì]ˆ¯Y˜},:u÷RN·³)ü·p²B	ÎW›‚*g©YÉ˜µéW€D¦²RÔØÕ]D,nŠJÊÀq—5–¹ÙOåÉ×UB`#Ç&Ibm<l8ÍûZ×ÜJ/L)g;Í§Zë¦/MÆ+N·K%qb?×-Rn»¡k‘\#<”E®ìhõ±åùeŸà=â‘óTf-€vÃª~C=m2ªn)Ğc¤_ÜÎY K¥ršomÀU»£:±Fû®ñ–ıÌÔ«+¤àí™A[=¹ö/×<# D9"µDtÒU+/ÈÒ7Øjk’[×9Xàƒ|Ô[<ßôëiëÇ	™ñØ9Ò	o€
j]•fzÚ©FÄv¾*ú IQˆÅÜ)\ÃœGD!Šãê;4ôaA«Äe©ù³¾YóX[°÷ÚÜåR5†sVÍãéºÒV%­›[o|yñ¾š–gø^jİµ$|™Ğó¼Ä³*¬¿y2¢ƒ¤3^SÀ¸fÍIˆ¿$•üà×›¢*e„ÄèzÚŸK^ĞTÚÏ²Z¦ÿú–0÷üP¦—ú¨ÛÿbLJõE"P'šcç®«¦<Æë´û;t*ØÆïÌfïÍö6‚â—È†ívÏ”öÄ³0øs³ÿoŞWáóP³³‡a0ğöîŸ+[ˆ`VmO‰ö¾F‘IÆğvğb%;¯T·fg†®a~Â¯»ÑÌòƒ¬®pï@wÃ¨7$¸èz–#Å‰zqcômL}üğ"æ[£Ôs›’qæßm˜H±ğ±ø“é¶g)G9®¾Û)¾ş²Á˜ÑˆÉÙ€äCñp¸©cêñfØ?!Â‰¹M]ÿ\¥Ê¼0aœ¸˜p³š§W¾oâ¸ÿrØZ£2$<öá%mË¿òê„w# {…DÅ£Ê2ooCºW¾¦k”±)/—­I:¶Ê¤¢\Æ’’Âlr¶mÒ”‰ÙAyvŒ(f²î@èÁİü #%mu,^*ß=2(…ôõòOÎËhŸ•¡ü" Çúå^Kßïø>xq=ûßãhd8c€õA@ÅöŸ°v¸­­9\ğñê(mı5;[€röÛn!Ío3`àÕè1R×Ï«)Q3!…3Ó»EU„ÜOaÒCë¿)FÈAëC5ì_@S 1–7eDMî¦Â~)ĞU¶ŸHƒ™H:®R¦7#¬Áğgcêé›ïå…˜¸ßÄ¢k¥ù‡Vñ¬0w>¶]VãsêyÉ*ßŞÃ}1>A$š@1ÌzºÉD']¦ë³’k·-hK§çÂI`‹ĞYM/]]
hÚäÒL¹=lƒ-–â÷İ@ñj¼ñ½ù8ªÇôíë4qnwçé#9Ë4É7Ó:ÿâ¬ê#}‘Ö%IÚgˆ`¯ Så´G"°M­•X¦ÃƒFvgK^ò³š„<rWRúÿhIÒ7¸Ö½–[ªJ6WÂÂ=—ÏÖÛ¦Ì™)ÆŠ3Ãó’+#¸c!¾ÒÛSSŸ¼òHñ²I73œjqÌú>¾^«¢_·åùb)"=ƒO¿umğµ*ï
OÙşXXâ¢áuÑ¹ÏyKvğkí9ğ¼ê®£¥5ĞGózújfRÿîÂsI™‰ı"Ş:¯»,ÙŒñ¿äÇ ÄA!B—ÁøáK|ÈU6àÙ¡œ£G#˜Ëj¢ ‘L$ŠÍ­‚24hûÅÚÁB3¯›ªÓÖé…)4İ»>Í':ÈóÖú"£4¾ G­flwú…©Ÿ6 ¦ú*ékµ§sŠºpÿû+èfoû`g¡o5caxd–gZ$Ş;ŸgDêÁH3cbÓËºŞÑş·„ÊüÀÊ¸i–ÑN6%IfˆšdŒ	ls;.;ğ<!@¦*µAß‰ÀĞÌ±w¡ïĞ6)?w‰Æ2îIwPôYAÌŞ°µ®V8¤õá-5¾è–Ş*í!‡V˜¶–Ù •p$[‚!b¥m%”„© èOZ’	|¥lŞ­û’ÙzıÜ}Ï…» $0ÉÅ}k‹ôqxp¨¨TÏfä»Ïõ6‡-BN›(sÁáââ%°†ùùĞ¸G%p;9¥hMâ((ÍF(¨,Ÿ³…+|,BkDÿ Ï7ì¬ê"aÂûjIîÿï4Ê «¤>{c´ŠE¦÷R=iÙôb¸ŒÜ…[Ş/È@°P_Ÿ}¢°J{h†i1x¤xÎ¬
]CLõó°±®³V/l,Le¹™µ¬ÏH:ØšÏsŸR¾m-a1; ËC¸º+zô½{’XCö_®ğcŞ©™o®¼%®|§«4mvş‹jğoÇ¥“ÜåœìiHÚšz²•¬•zº`6˜yqR/
ğ<¼¬Z·™Ä¦¡3£gğœTmº­ÃFfªÈÏÏÈl@(FxcQÑ»±n‡°qç
IÇ•ÿ‚‚¼)–f×§‡GÜÏ‡U± ÎG¸=éT)e˜&èÑÿ…Åmg[5„°¿'-º7&yVë)Ù3qm°>–úŸ û]ÒI·òj7á€7²‚”ètSôÛ¿Q’IÑ¯åî"B·E†Ü„½I®,,#E‡0Ï‰¤Ìò,²Yá¼OııvÅ=ÄµÓë†© oúKğüŒB:k¬”Ù(wt¾b“€*€c
›Ûºy@]I¡î4#ÇKZ¶Õ—±ÍğÑ›IChî…EL]Øğòn‡f5kÎıÃ]Çœi¯dsb&Rÿ¥òDìñ@‰¤å$tQıæ«X×£BÁ	â¼Î‡s<\?™Ö(»c U4CØmm&Ëû§»½òV{,5ÍïEalÆqYVÓ²¥‹ÌƒË_ïØÜ»Ô†•ñÇõ2JÇ_ë$Ù_ÉwğØmD0U/›aÑ“IÍ
Gƒ‚\ÂEKb´K«¯•Ø<nîŞÊU4íFuM Éù§:ÿ›…4ş4SRç¯ô€=ó?¸ERÏ>º´ÑçöÃm™İ0ÿF„—+EZÙ¥`Mr¹…+„<(ºœ¢†±Qszÿk“\Àó‘é˜Ù</~9*?i:÷-òln\yIìÜyI¡@¯Óà¹ùT&4=R(Å~aÈÓL–ò­A«](©#©¦ÒÜøÑûjCP£2lXùĞÉ=tíg’ĞùëáEåEË0½L1L%­Ü‹É‰u=£-íF1UyÒ¨;¦®Î.À«&?N	ñö®®.Ç3W~X”nL:[eÅ?ßæY@ß3R¬hƒ¿ q}8g oò.(æ_\ öå‚_*šßhFœ* :©8’"Ë7&ÏjÖaD¢†ö)e05­ÂÚğ÷ü?¯cIÿ1Ü7n‘'vaN_' 
êz+u„"=r°ît˜•'ÎñW$†â	üáUºPq+õÏ€Õê©W¨S€¦U@­/…ÇAúòªd3¢Šø Zéxì@Ë½ñØ{7b¬{¥DKÿ¹ïı®]–c\g8a½#‚ƒ«ŸŒ;#ÃàìÚğ&{Ç‰Á˜Nù‘’\¤]+€[a¶wÓWşN‘!8WÈôıÚì{B;ìF¿[¤5>¼ã5çšüˆˆ-¬Æ’ «´¿ÕéÌ•s\±[_wİ;Ô8²iB"ğÑ——¯bvÕAœ?İÃİµ‚Gò
Ø >J¥”ê¬ÙµwxíDšVd,ÖóI˜=°±´òpŠ­Ä—x%şGÓ/X7U7¥¥UÌÅlz4¯¡"fÃ%ÿ%£jLÄ€×ÍeUf¹FR5‹åVÑ/5o7¿©@ØEØÖ±‚§)œ~ëv‚R”…6Jé
öo[-¹bÎP;.¨ÑPöt¦|<k¢	ñLo³RÃ\m>ÕÔTÄ8ßŞöL“¦	àÑŒø_eÑÈ˜[S¸ª³^Fsû“ÏĞ×1Ğ1zFLk3Ş®¡U™j8/Ykååàë“LQûJ\¢]U#ÓS>i’¡aI» õ·Ì_hü!1ZÉ’@Wj¯Õ"<Ã`E0Ó›$°õ9›g„jôr ÁO­æfÅ§t¯Ô(HSÀõ¸n8ˆ*óE¨hu :Â¡7©cÚú4€¥H+Ö‡Á"Æ¡6p©
Úe ƒÀv3‹øi6÷ô’¢!¹Îî¢ç^ö§eŠN8‘>ÙJî´~d©E4o‘Ã­Ÿ¨/.úƒåüÒ)°8õıD7ÕüD`HÆúGç«®Ø'ùcêS¦üØ<ĞåèÖ¾o»²5*•Ø.J¾R©hórPÕ%¬¿¹Rk¤|îhƒë{ëê›õúÚ,/øI {Lªc‰/b·gwIÃßµ\¶Ešn8:§·vÙ0îŠ?/î›ğ¦ğ€Qld7øÔ„…§v“›£¡Oû56'ƒ9x1B•.†ĞÑ/¦†eBHeëiÌÎb+Pà—½ÿ<Ú‘$ŞÁû_2xY)¤µcÂ½êİ«3ØÒ„GYL%ÓYrŠU‡„Ñğe-Ìó'6¥ª']sÇI_˜õ+:øÍ¥>Ï¢7†ıÖàËGD6ú=Ö”w‡{OA§½ïüª{ÃÒ™€‰s5¯ß…Øä$3û§t&‚Ò7Ù
ÅBö`ZœÁr>‚EïMQİ3Ç›ñÖô5c‰–ÿ&v?Ä=`ò>tÑÍFO®gì–ó¹?¸DÆZ¿r»UYäé­ z!Ğ%	-l«Ês•÷Ş1¦ã
ûyä—òŒ‚şs¥$L¢§à¼áïĞ4Ú±ù¶è)œ÷£¸¤êÏŸÖcü^ÜşÓİFw‡Ãg¯XhU©>“¼	*Q$‹&¨1l†  4Ó])Û—nå|KwÿÅ	+¾ÂÔ,Ô/Ã\^1ˆõ£ë?dƒ.Æş2au*C¤ğ:ëó”KWÛËcCeçø‚ÅÖo_ÚDlrğ>RŸqE¾–>Õ¥Fd0ÉÙE‹Ã—U²uˆ^’®XKœ—µí_-Ü>ÉùĞs7l¼‚áŞ‘î9d“Ë3`\+qğÑ¼¿r€0zõú ’í”Læ$&ĞçRŸ¾Rsc•¬ÎôÇÇöíVxo†LÌ|  Ì <->ªÕÊ Ìâëé„ôÃsø
Çô2¼öhb¡ ¾•ÄÄä]šiR,h?aõLíó’‹ZVQØ8Ã¸ûgJÉ¢é&Iãfƒƒ½Yàˆ©Ò;4 ”¥Œ–V¡
ôQ†á¢”@¡Ö½èŒìäÜáå0sÙ),d»91;}7µx^zÏjÇ"v«îƒíèi«LÇ»cñÆ™fşåc1_èîK4¿vOÔv§şÕ¥Ùµû8s«4zzUáiè«vÀàÈÂò¥GÏß¡úÅ6Íux4ÓCŒûH"?\Æf•(IÔKüôW`Â‡Üi·"+CÇxïw×ÃÖ²µ"ÔÂ¸û|öü!°ÌCúëáw'tC9s¯çÒì‚âS±¤—§ Ç'îË ã(›ùôÉt‚+zó>Í N`·ˆ”ä3fê9ušÔ=c—f_ÏÔøÃ;,døú”uE<ç­¿x-2‰ê§TÔ†ÕW
ğœˆB¶¦­,èn%S!:jiàaÊö`ç™WÀÓÏ"GA^)u±¡Ğ0ëk¯‡]u­gbƒñ°ÿ LêÌÔ:Z~è¸Nû[=éêäO°˜´"³Z0U²	Ò.ÿX|¾¹æ»&ˆ8d©9bÚ´œøÈJc|Ÿ‡PL0Ø4&V6‡?f	×¹üÇõuÌ¢ú¯×#ˆ1²„DÚÁöÌŸ«5±/XEöbvüÂBëZ˜BLÀ·ákp¶1ˆìÆà«PTş-Õóàƒi°kU|©€Á‹fÇõæ2v…öñêô!ÃhåáØ†éA
T6ÈÜRÑ¶êrá~hÛu3Haïœ_ğÌËêõ†wG{2.(M­ofv”íÜhM¼%(h$1¢á– ş5VéÁd35JÿZÃ<Ş\»¾jğü¦ã—tü©’	¶­‚¡LêÇKÌÍbXQ@ú„‰ÅÛŸ¸\íu@š¸…Ç¬÷Bôàú!
K•oşågºÄBFAgÙ\W0Tz…H›óŒ6çk³mÜ\¥Ñ×ÖGò8ù„»ò—nì7eı½ZràÉk`Ç„É¡hÇjÁARğı»~åmëöı¥ŒSCüCÑıœ‰Kd5ªŒ˜H³Õ™Æ‹Íù‹è/úadğæû²0Nb2İİ9„…®z«Ë7v[šá{ïƒÏYx¡ÕÆ*Gâu£øwo<yúÆ~×{›Cr”ôãqÑheËü2û{H$
3(oAE†í}Üù¿¸Sõ/95Å|›Y0.¾ªe{Gr:O÷;O"‚™ºsÒ!‰Gc¥’¬#;[ÿÖP†%ÿ‰“®‰ÿg<ã›|ä!ÅÄ»§Íüò†êƒ+ÈV†n½Z"•×ÿr;=õkA*á2Tü»ĞIqíôçÙ¾m²¾ø¸Îj¡D=ç1‘Qz'LR8hZöJ°%‡>Ø€0o“öïaB'¨G`¡.?Q”…˜À©’H5ÂJ#àÆˆ 5ã£ˆSÂn„U àüVV˜‹ˆ
M5ÖUO¤öå²[†}™;Œëãë ŒZ!Â?²Ğc-Xïÿ!tÊ‘…O¿H¤#Õt+î÷‘FH“­næÇ&ô_Ø°&Şc$¢Ìv:ÎL+ıƒ>ô„(Jo56İâÿÆÆ¡Æl/åà–|`¬†L¾KŸqÉ÷Åæç˜Ø€¨û€å+vt5]1ÈjØ>·ÂQní’÷#ùƒÑ,K'£÷”_7‘‡ÓRFŒF¥œXRÃØ@åÖš¯4©š>©Ğ•ÒC\²Zv©]ø²P¶%ïN9í÷ï„ĞÓ€8ÇÂ—UĞJì®‘ÜÕ0ü»¤4µR•Mü`—ÍGHøù4ÔA“£’Ò1jbOX(±!Ù)¦R¬‚©''ÖÎìù¾úº®bzK§-&TÉÛª~ÃA$8l:#PÏòoÕŒ/ÚrÔÛˆºî:aŸªMÂN!¦TV(‘CË†4·E'áìç%ñ½ñIü-:+íáz•f Ò7gF»Â¡<ÍJhO\Ö“É ò%ÚB¸¾*	O»Æêfå}oõbóP„|æİĞê…E. tîzrºâ î¿ĞbGÖ V8£…¶hL<>ÈNşâµÏ&¨×¸¬4fğ%›Ôëåô'`ƒÅ¸»ÿc¦ •Yrèşy)˜şÍ+ùÀ–1G«7£%8ğàNgÔ«°çìØ5HŠE
‡·Í¦¶\úÅ dÏÅÔ‰¢ïÌŒr¬VéÃxUôŞ«Q£~ôšED™‘XePÏ`¹k òAÊfàÆï5”¬ò1»“,ô>Í£<¨º­f6á£Ÿÿz¿JëÉFÏrÚÔKA1@n	›d…Äuİª#d.ÛW»2€"N•*<ğÈú•Ü”Ğ6ô­‡¶çŞÄ©YÏ<™Ëú2¶ãjøqà=0B2¶í®²ÁáUìpuâQ/@Ó»°8œïº<vÅ^íd&Õv²ğƒ~U±¶k¥ÀêW0Ğ…2‰½©i#Œ!I?íÃ/™‹‚T-1Á£ˆ—m«²ø‹6 ›¯ÿÆq‰ß°cégƒ±ZØr›&jÊâ¨Uj—`,Áê<m/£µ“í¨sN‹Lòä¥Ø×¼EÌT¥.lşWöfB¼¾ßoğ~¶*vÅLÏß0¥%(À ®Z–¨ë3Q¡œË4Ãw>½ƒ¸¡Në)×bä_YJ§ªÈ’	lBz±ßò^‹Úç…ÉàQ_*|éêàº'eèÆêõĞ}uõtÂ%ø±kLÜ"ú:.mënõ‰Ã§Åm©0µö™«ì»ãÇùcÊ+³ gªª;¿¨@~Ù<´f…iÂÍˆè]ğ#TrÏd$7ìÚ“Y½’&†.ê¨$áŠpSÃCÊı)õ¾¸´ /Îµ´ıÜ%p0_Î÷záØåtÌ¤å˜ıg,ñQ¹Uª2'j2Gzµy{·Ş?(¾Èk§RPÒî“öÜ¬ÖÇW{l’¢xS&|RÛ“*_h ëøcMƒqÇMfzŸ­5ß|`–ò÷‰úâ×U–lhHÿ‡‹x€2rd']c§ÑtH*µgÅ~Eö”§Q[İ¨DC/vÌ(O_‘ß¼ wkz6ªùtf;'·Gqã-¤fŒ«Fn/ƒ*p_&GáO¬D}ªÆs(’ĞñÂW€Œª€v×ó^“FÔ;F	«è°ƒû ü˜lÎèÌàt­ƒ¼UÇ/WÂ˜@ÎÈ› ­g*¬å¨8øvâ“'Î(î65Èmó^t«¡N.Ñ©aÇjÚ´,±|½€­-!ğnòÃùZn”?mËTéQ€.Pß$ò&ÑVÅ7ı=†QGoÊñ1xİH‰®EØøú3—ØAäi/Úø”EâE~tB¨Ç¼Oôó°ÇD(©µ²D>$YöåÉ–õˆ£Ğf[´Åˆü7K«úFÌ¬ª²Z[s‘ÏåÙ·¶cNRpù Ïqwm?äÏ™'nmZ	t¸Ñ¡ô¾/‘b›M'ıš¤Ô-÷&ê9‹´k°Ç5‰. ‹C‰³¢\ñTêw†¦‰]¡¯x	
I d|5NHÄªµãn ­ÜÕ(JK(„¦§V•…DŒ4ÌÅXÀV,±>™æ–Â(ı×[¥Şí©ÅGgß
å–n¼8°q/5‘ãv=á}¾w^Íd³ÎÕ=§æÉ˜Ø×eÌn¢Û‡Gğ'mcE™®d¢¯÷5Zy)9Òxa;A\v+‘…òRCRRá&9‹yjH‹íB\h:¨aófª‘zó0İ)åesäögÁ[—P£JZ•0ê]j"¹CS<ÑÓâÛ€”æ3ÍÖë(Î},pÃ²%£¢=¹&ûx»!ÁŞ£,zòNdÜÚï³ÿ¨Xÿ–v€UÓµİv¸ğh ÙÑ´âçBœ}^=*rø@Ş|²]z!EÃİ¢î4+Uoı²ââH|c±—ïU|+é¿•år‘J…Jşh1«ÿ«Æ^R¥6,“˜ı˜}’ıK¦İHvíq,ôeÅÒlÃM€s´$:~jÀkMû&ĞSM9xœOŠ‚îı p38›Ip?üøíZ†ªbÇk‰È²¾$ğÖ‹¡S!ÎJÛ ğ±™ÛÙ¬®Wj4–K‡'ÆºJ‘˜ğ…dQBÈæ;Y {NeÓ»‹ˆ"Y`7ËâÛM‰© r0S6¸)h®3U=•B´â]b›Gêæ4{LdïÛÎí’*¥™F«vôÊ_QÆP¦®óÌ·Àœ‰ÒIõœ.g¦PAØ°^Ùk"ŠğŞšëÊÌ¦”<ñ\ôB^<ZÁå9õ6	IùŠåKÙø®ğQüÚl›û£šµß±ÊÅ÷^±E	I9’Uh §H„GRú–-qùÉë•pØ•kÇ¦¹ÛV	 ÏñP¨Wi¯Eíá2ó}YJüÛNÒƒ–—cª‘gL¨:Ë‹n†ÀN:º»³¿áŒ¬÷¡SíŒ8ÒR3ÈúõF¦ôär9åOoó)ø‘¤ş­ UŒ=¶ôxg%yºğ«²Úşf††½O&á²˜-{»XCù'+s"šşv‘›(àõ…8³$n¤~#Q‰hîÅå› J_üc^/IF›ÀHi½‰ºyz³ü9šËy	´H_DôÓ¶§ ?ßåŞßnf­‰.’„VİkWnDòäÇ.EvhN¿2íîg»¹”`µå>ÛRÉàù…#a´ÒËÀò4&
©ízÙ™K‡8PH;ûŸŠøQĞ€uS¯nä
y—¹kN}¾:“oN””@úŠŠÚìÕ^A™“èvÀá™7ØDªóq˜ÕŞiSVqLb0u³D0á2[	º<·qÉŸæ,‡Ã\³#…\0O:ßàšœx!ğò¥+y\~Ó™õD’sî‡¶œğâz/ºINğñYô@6%"2¸N¬.¹Ÿ­Y×G…şNF¸a9Ø6|w<“£[ ¤±_	
ƒ[-âEœö»Œc,ìß8ö%´É¤yw¥³nUøo!]Å_¬çi’è•:U¢fAJˆYÉrÅy3JRİfğ®ãİ+Ùd$‹ráş$ò„øVaÀÊ?èjõU £?Í0ˆDçcdéêƒc3­9WÉ
å^qÊ·77&Tcºú‡_2~+Wğ!ª±jéKÔ:Ô…sÏø¶¢Åß45mbßl´â4&hZ®x¤|Œ¤CuyGŒ†<®«`TNÊ<'\'ßä!ó"9²YÿgJX&,;½`Û¾ø‡„Ê1šßÁ´(#â×Ahs€İ¾&úeYÕ4ı¨vì2$£ñú¸é\pv,ë^ÆôÆyğ	Ÿ‰/uÍ{›úí‚ÁÎUß%»«"NÇË»³Æ§Ì	vllŞÕÏ˜0H¤.8ä+vİÚS‚ ù§¹0Ğ'¢Ì¢8A'´âêv¢·jeá»®ĞÔ—|£¦¨“Ş˜_N1£‹sIåÂvÈˆnÛuà²LE¢Ì->úpXµ‘ğH	„H(ÄEu`6Ï°Ã…¦|òæÑç"¿ T	ÌD.Ğ5|Ì²©(óã€g+ëó¦ut§løv
şPı5„z°“Ì¹³šš3ı5dë ñÒ¨=8ŠÃ¾—úªlJMSJÌÙA6È? p™xV#zaõÒ{ïòP¶n-è‡¢˜<Õ³[‹Åà±g¬jç•Uóù¬$Áá g¯ÃnX¸¡‰Œsl•ğßÄÄYâ’)D»ñ"[ÏìFG¥IÀ×{z·T× Í×·ùßUOyzr¸!ÕEˆn-9]/ÉOwÎïzI>íDF™:³qmÀBû t(ğğ½	îùã~ÛÉj—gÊ%K’iXNÅ°‚ NZÈÈÉIÃ­k“»vÖYsI»’)?>ıIÂ]ä•g\¬áUl`
ÃŠ¾‘ØŸ´Ò@_( õo™ÂìË-©$ùPNT3¿;™ê‹k*K«e-Î³KäÇ>Z7Ò v²Ä¨mœğ+şÉ–z±õ“Wİà€Û¨ş]®Q0WFj/Êš‘Ù?ÎPD×2UCo0fÊq²NõPˆï€aÃ×_ÜÄ%ºMT
—(VÕQc»º(›ìÎÇìp1àV…\ÍÖ›õhÒ°koö®n‰ååßwÑ1}!Ò_ÉÁ<[5sğ™€ø*ó‰úˆŞ.|èÇY8§âÕ‚ ®ºU˜	ä‘ à¦QãWŠñn"ìB‚¡çËlZr„š¤‘º:î^Ñ®ùÓ%EÔ“ÅhŸ·;vE›\ˆ¦sZÛuÙ÷ÔH<İ@á1R.å’TÊdJğRf_M(ÏéÉOUf¤±7Ö7 Sf„<`(1p¡˜ÿ(Ùç¿¦äcßXë©T™Ç¤ÂcdÜ‰òš<]¯¸#0òÂùä²bÅ³"…"ÈáÁûyÕ»)mş«•hIl*pï½sÇÓ›•®·™5cj—DOç˜k7)÷şw[Ë¸ÄCï+ä˜~Üf&%AœSX
ı¨¶Ó=ÕÃµ§YE­Ëa/±µ¡NK“ş¦è›¯5–VvŸµ¸Às£H3°ãx(²*ÔP+İÒf&¢KhP{Ğ8¹­ü w•æQ…Ğ8#–­±FHì¬;¬’ÿ&KR“·$Î+D~‡|ù¡¨`PI‘c6²¼vpşj—Bv&ù„_¡Â3U•§aÏ~ï§>J·öŞ€FŠı$º5âxòŠaYöÈ½}gä’	»Àg[Ÿe<|kQ>Ä¹šª›J¬?¯è"ğÕe$é¸ú´Gbc5²MrQ†Pƒ—‘?~ÑUÃ¤Õr:K[q‡¨ÎA(K¬s2]§Í‚ÀùJÏèç+ÚpJZçaŒëü™àï†–-5Q3UANkñ—¹ZŠ’!Tı[OK°ó n4v6ú˜/ªÈCê¸õAâ/{’ å~…vV¡70!*@Ì¹ğrŒ¸ˆâzÅj?/÷æ:šÌOÒèŠÅ.ªÅÙ˜ÇÖí¨ÏGÙ5c6E¾£2Q•”Ä¤-+À+t^Pd“Ö³3/#Ãc¢©†{¿Øbµ¡7{_“ê×$â›E«—(´(áÇ´JP¬&&\%g›7(ºÙBóáõ(ËuR^~şHéöÓ£öÂ1£F©8]wÒÉ‰3©êFÌ°Ç„ ›ı7-xhÕ5ÖğçúóoaFŒØ–Väb0ä=Ó±º‘Ú€‹ÂNZÂ
­6ÇÖ’‡º«Ç™ş/$Æ«W¢:œ´wör’7Î­…ˆ¤ºÕK†Û%Ü¶Ş9÷¹rÛ…¾è?ÇH4{à AyTlf]U`ÇÏÛÀCX±^„Ç¯â³ÛÚÏ³øäƒz\±ˆfÊ‹§[ÏE#Æÿ˜ï¹½é„D3îê$¸_ıjÊczè4é‹«Ãù»Ñö`ş2ÆŠóEÔw[9`èçd‰ô/ŒÛUF†¿óŞ_‰õèug9n¢ù¥WÌxú5ˆi¸³vúÜóó®À†Ğù¡‡rÕ
iE½§Uì:¸”Í@Mï	€¶z™ ²UI²/uµ gÎ©÷zUqI¨çdjEf¸(1KÀÖ¨rĞì+rüxRHÈ°û4‚-nå˜úÜšqÈÖ’xlÁ7Ÿã­Ë˜7üZê('ƒËÉKs#¥ìæª÷Ù5;+'ÃG—Š×Îlø"£:"¥¼…zá•Å_6ÚKÊù³X\ğ±ÁÔPƒ	A‹ÂÜÁàeªœ—şï*5\k…ˆÏ>¥ +Ša~Ì,;-ÃxX&IÊs~;Ùñ{tÜ_ö’l‡j†G€.²³ÆİOãù—a"ab.ÌiËQÈ. Ş”]=MYF–•ªÌ†Š§jBi@B¸—)Ğ½e*`„ÿ†p‰z=^®$1H5:µv6>U ç‚)}%‡ä½³Ê4;dÚÙ˜÷hp#ÔMÄ(#:9«ÚNÅ­qÌæaè¦OKHï§²HEpi¨œ+½´òlÑÂîó¯,*•PÉ9‰dÊ+è-Ë„25V,¶'åk•rïZ>vèü¬OêêZ¯[{'ÎÁ_j‚lMujvÚqNOê2ÚM­	v[Hı
<Ã<µW©³^©5>Gg4¿2 *[ªèÇ Ü:”›h«Uã}Ö³)o!ÊIyJ”Ê6&¸zPv¿õ¼@9Éøé”Åñ óß	»ˆZè”¼ÆÉgn!.—ÎÂ›»_¬|¬H5ñˆÂ³‡ÁÛ…™•^?·÷™çŞ§%¶ñ%Ä±u~,¸xÄ	ßm›µö½şš|.›õ†?b@Òûãšƒ‘i6ğ%6"bB¢“êübtp–,àM/¶—cQãÙ¿¢ë÷!$·Í’2=œğ'ğK«8!sÅi™ZÁÅ	bdÒ±+úª:ĞãK«q'®é”ø7Ó›bÃƒ‹Lfve—FhÈvôØ2«_ÿ1ğ;Øsï /‚oK¥£*l~²¤e´Ïó—»8‡
‡Q‡â:Ùı–+èå:ew(YõæC„æ‰N
¥ş3Kº¬“VEéDr—ûF`È>5'~Í×2İN¹½-zåù·êtÙ<ÁnWT]53í!Å~<àLÖ	E^ø*
Í}@H‚ËSÆó¢­¬Åµ˜„2Tı©wJ‡TqšŞóò¢ü‰(B¢æ£^/}À¡F¿#CaìåJƒ³¼Dœı"¹h	¿¯yØYo)¡…ÉêcØ&­À©ÌOOR
g€’wß¾‹ãA§ÖQ}íW	½€ÓYpÉÊ@xöÕÒSnáO.YPßt)óÄ£»h©ĞôÄºôp¢ÎÑ4,3îğE•—4öPt\É s¼¦ªÄm#ê8QàÄ9ø65–¢™„ø89hTîä•ÏKXf¿c½W ¾³[átZ¹zò½¤DP)°ùí6yšævêĞµúÉ?èk™së²®ĞŸz#>ó˜:àçe§~‡`rD¶6Ë‹†¶%oYi“´íI§ÀñX¤G#en@C“‰³Q4ôûÍúêdì >öËuwK‡[o]'Ç=^ÖÀ1³	M/ºn®J^ıaûe^Æ  ›±öR¦\‰;ÚÄÜ7ê-Y`U+eìÂÊ'­3¾ºÎB÷WÎ­)ÌÍgÓOáÇÑ¯x–W›á0V.ÃJW~@œà>2"	YXÜ.°ñ¶¦Ê—éTjpÉÌZÆÄò2°Hc”ÖN‚ÂáÜ™Ÿ•	pû•	úÏ-òµŸhT_úŸ-²ÁˆSÕ¢ÿÄq#›¼·ıªµKâÎp'4:ƒæm‹mú¾w¨ÎU^áœ¨ªë÷‹:ì¥ÚDö¼OËçs&+OÅêºÍåˆYŸÁë
º$('Ñ;
NuÓ"U÷<7Òê‚b]iûüĞoú¬t¹U„.P¤Öùßá®{PO6Şl¯µp/7 Í(øîr3´ú¼ QUŞ™\4Iÿô˜›ÑRÿĞ$Ş§ïr‘ñ;”ÑszŠc	£°\|ÍƒÜ§›$Ğ’²ıÖ$´°ßQäÁVy†!¾®Píò	œ,––ã,ƒIùÜO H¶	õt*xYmb‡¢™úÌAÒh{ÁÖÂÀ†‹23Œ’;zÛèz#A…Ö’6üüƒêÌÙƒƒt{öéª×Í/½ß×ÏÈ
tå£èçM÷8QˆhAÌ2pOìç°±˜/«¨kr%Ô,jnŸ]åJ©L’§Õß[0›eu:áJÜÕ8GY',yõƒ7ÎÚ}¥l˜kşb¥iÕ6û%¢Q?²ÚäÑ	şØfğ›¤İeh4
¿'vV/q=%¯WË¥Jl ‘!U~	ïØf&6­»Ø²ZÕsi3â…¥ÎE½Ô¦÷ÊâRAS/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth))
		return 0;

	return 1;
}

/*
 * Remove the lock from the list of currently held locks - this gets
 * called on mutex_unlock()/spin_unlock*() (or on a failed
 * mutex_lock_interruptible()).
 */
static int
__lock_release(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 1;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * So we're all set to release this lock.. wait what lock? We don't
	 * own any locks, you've been drinking again?
	 */
	if (depth <= 0) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	/*
	 * Check whether the lock exists in the current stack
	 * of held locks:
	 */
	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	if (hlock->instance == lock)
		lock_release_holdtime(hlock);

	WARN(hlock->pin_count, "releasing a pinned lock\n");

	if (hlock->references) {
		hlock->references--;
		if (hlock->references) {
			/*
			 * We had, and after removing one, still have
			 * references, the current lock stack is still
			 * valid. We're done!
			 */
			return 1;
		}
	}

	/*
	 * We have the right lock to unlock, 'hlock' points to it.
	 * Now we remove it from the stack, and add back the other
	 * entries (if any), recalculating the hash along the way:
	 */

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	/*
	 * The most likely case is when the unlock is on the innermost
	 * lock. In this case, we are done!
	 */
	if (i == depth-1)
		return 1;

	if (reacquire_held_locks(curr, depth, i + 1, &merged))
		return 0;

	/*
	 * We had N bottles of beer on the wall, we drank one, but now
	 * there's not N-1 bottles of beer left on the wall...
	 * Pouring two of the bottles together is acceptable.
	 */
	DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged);

	/*
	 * Since reacquire_held_locks() would have called check_chain_key()
	 * indirectly via __lock_acquire(), we don't need to do it again
	 * on return.
	 */
	return 0;
}

static __always_inline
int __lock_is_held(const struct lockdep_map *lock, int read)
{
	struct task_struct *curr = current;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (read == -1 || !!hlock->read == read)
				return LOCK_STATE_HELD;

			return LOCK_STATE_NOT_HELD;
		}
	}

	return LOCK_STATE_NOT_HELD;
}

static struct pin_cookie __lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return cookie;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			/*
			 * Grab 16bits of randomness; this is sufficient to not
			 * be guessable and still allows some pin nesting in
			 * our u32 pin_count.
			 */
			cookie.val = 1 + (prandom_u32() >> 16);
			hlock->pin_count += cookie.val;
			return cookie;
		}
	}

	WARN(1, "pinning an unheld lock\n");
	return cookie;
}

static void __lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			hlock->pin_count += cookie.val;
			return;
		}
	}

	WARN(1, "pinning an unheld lock\n");
}

static void __lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (WARN(!hlock->pin_count, "unpinning an unpinned lock\n"))
				return;

			hlock->pin_count -= cookie.val;

			if (WARN((int)hlock->pin_count < 0, "pin count corrupted\n"))
				hlock->pin_count = 0;

			return;
		}
	}

	WARN(1, "unpinning an unheld lock\n");
}

/*
 * Check whether we follow the irq-flags state precisely:
 */
static noinstr void check_flags(unsigned long flags)
{
#if defined(CONFIG_PROVE_LOCKING) && defined(CONFIG_DEBUG_LOCKDEP)
	if (!debug_locks)
		return;

	/* Get the warning out..  */
	instrumentation_begin();

	if (irqs_disabled_flags(flags)) {
		if (DEBUG_LOCKS_WARN_ON(lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-off.\n");
		}
	} else {
		if (DEBUG_LOCKS_WARN_ON(!lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-on.\n");
		}
	}

#ifndef CONFIG_PREEMPT_RT
	/*
	 * We dont accurately track softirq state in e.g.
	 * hardirq contexts (such as on 4KSTACKS), so only
	 * check if not in hardirq contexts:
	 */
	if (!hardirq_count()) {
		if (softirq_count()) {
			/* like the above, but with softirqs */
			DEBUG_LOCKS_WARN_ON(current->softirqs_enabled);
		} else {
			/* lick the above, does it taste good? */
			DEBUG_LOCKS_WARN_ON(!current->softirqs_enabled);
		}
	}
#endif

	if (!debug_locks)
		print_irqtrace_events(current);

	instrumentation_end();
#endif
}

void lock_set_class(struct lockdep_map *lock, const char *name,
		    struct lock_class_key *key, unsigned int subclass,
		    unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_set_class(lock, name, key, subclass, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_set_class);

void lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_downgrade(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_downgrade);

/* NMI context !!! */
static void verify_lock_unused(struct lockdep_map *lock, struct held_lock *hlock, int subclass)
{
#ifdef CONFIG_PROVE_LOCKING
	struct lock_class *class = look_up_lock_class(lock, subclass);
	unsigned long mask = LOCKF_USED;

	/* if it doesn't have a class (yet), it certainly hasn't been used yet */
	if (!class)
		return;

	/*
	 * READ locks only conflict with USED, such that if we only ever use
	 * READ locks, there is no deadlock possible -- RCU.
	 */
	if (!hlock->read)
		mask |= LOCKF_USED_READ;

	if (!(class->usage_mask & mask))
		return;

	hlock->class_idx = class - lock_classes;

	print_usage_bug(current, hlock, LOCK_USED, LOCK_USAGE_STATES);
#endif
}

static bool lockdep_nmi(void)
{
	if (raw_cpu_read(lockdep_recursion))
		return false;

	if (!in_nmi())
		return false;

	return true;
}

/*
 * read_lock() is recursive if:
 * 1. We force lockdep think this way in selftests or
 * 2. The implementation is not queued read/write lock or
 * 3. The locker is at an in_interrupt() context.
 */
bool read_lock_is_recursive(void)
{
	return force_read_lock_recursive ||
	       !IS_ENABLED(CONFIG_QUEUED_RWLOCKS) ||
	       in_interrupt();
}
EXPORT_SYMBOL_GPL(read_lock_is_recursive);

/*
 * We are not always called with irqs disabled - do that here,
 * and also avoid lockdep recursion:
 */
void lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check,
			  struct lockdep_map *nest_lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquire(lock, subclass, trylock, read, check, nest_lock, ip);

	if (!debug_locks)
		return;

	if (unlikely(!lockdep_enabled())) {
		/* XXX allow trylock from NMI ?!? */
		if (lockdep_nmi() && !trylock) {
			struct held_lock hlock;

			hlock.acquire_ip = ip;
			hlock.instance = lock;
			hlock.nest_lock = nest_lock;
			hlock.irq_context = 2; // XXX
			hlock.trylock = trylock;
			hlock.read = read;
			hlock.check = check;
			hlock.hardirqs_off = true;
			hlock.references = 0;

			verify_lock_unused(lock, &hlock, subclass);
		}
		return;
	}

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_acquire(lock, subclass, trylock, read, check,
		       irqs_disabled_flags(flags), nest_lock, ip, 0, 0);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquire);

void lock_release(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_release(lock, ip);

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	if (__lock_release(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_release);

noinstr int lock_is_held_type(const struct lockdep_map *lock, int read)
{
	unsigned long flags;
	int ret = LOCK_STATE_NOT_HELD;

	/*
	 * Avoid false negative lockdep_assert_held() and
	 * lockdep_assert_not_held().
	 */
	if (unlikely(!lockdep_enabled()))
		return LOCK_STATE_UNKNOWN;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	ret = __lock_is_held(lock, read);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return ret;
}
EXPORT_SYMBOL_GPL(lock_is_held_type);
NOKPROBE_SYMBOL(lock_is_held_type);

struct pin_cookie lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return cookie;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	cookie = __lock_pin_lock(lock);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return cookie;
}
EXPORT_SYMBOL_GPL(lock_pin_lock);

void lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_repin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_repin_lock);

void lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_unpin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_unpin_lock);

#ifdef CONFIG_LOCK_STAT
static void print_lock_contention_bug(struct task_struct *curr,
				      struct lockdep_map *lock,
				      unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=================================\n");
	pr_warn("WARNING: bad contention detected!\n");
	print_kernel_ident();
	pr_warn("---------------------------------\n");
	pr_warn("%s/%d is trying to contend lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no locks held!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static void
__lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	int i, contention_point, contending_point;

	depth = curr->lockdep_depth;
	/*
	 * Whee, we contended on this lock, except it seems we're not
	 * actually trying to acquire anything much at all..
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, ip);
		return;
	}

	if (hlock->instance != lock)
		return;

	hlock->waittime_stamp = lockstat_clock();

	contention_point = lock_point(hlock_class(hlock)->contention_point, ip);
	contending_point = lock_point(hlock_class(hlock)->contending_point,
				      lock->ip);

	stats = get_lock_stats(hlock_class(hlock));
	if (contention_point < LOCKSTAT_POINTS)
		stats->contention_point[contention_point]++;
	if (contending_point < LOCKSTAT_POINTS)
		stats->contending_point[contending_point]++;
	if (lock->cpu != smp_processor_id())
		stats->bounces[bounce_contended + !!hlock->read]++;
}

static void
__lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	u64 now, waittime = 0;
	int i, cpu;

	depth = curr->lockdep_depth;
	/*
	 * Yay, we acquired ownership of this lock we didn't try to
	 * acquire, how the heck did that happen?
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, _RET_IP_);
		return;
	}

	if (hlock->instance != lock)
		return;

	cpu = smp_processor_id();
	if (hlock->waittime_stamp) {
		now = lockstat_clock();
		waittime = now - hlock->waittime_stamp;
		hlock->holdtime_stamp = now;
	}

	stats = get_lock_stats(hlock_class(hlock));
	if (waittime) {
		if (hlock->read)
			lock_time_inc(&stats->read_waittime, waittime);
		else
			lock_time_inc(&stats->write_waittime, waittime);
	}
	if (lock->cpu != cpu)
		stats->bounces[bounce_acquired + !!hlock->read]++;

	lock->cpu = cpu;
	lock->ip = ip;
}

void lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_contended(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_contended(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_contended);

void lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquired(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_acquired(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquired);
#endif

/*
 * Used by the testsuite, sanitize the validator state
 * after a simulated failure:
 */

void lockdep_reset(void)
{
	unsigned long flags;
	int i;

	raw_local_irq_save(flags);
	lockdep_init_task(current);
	memset(current->held_locks, 0, MAX_LOCK_DEPTH*sizeof(struct held_lock));
	nr_hardirq_chains = 0;
	nr_softirq_chains = 0;
	nr_process_chains = 0;
	debug_locks = 1;
	for (i = 0; i < CHAINHASH_SIZE; i++)
		INIT_HLIST_HEAD(chainhash_table + i);
	raw_local_irq_restore(flags);
}

/* Remove a class from a lock chain. Must be called with the graph lock held. */
static void remove_class_from_lock_chain(struct pending_free *pf,
					 struct lock_chain *chain,
					 struct lock_class *class)
{
#ifdef CONFIG_PROVE_LOCKING
	int i;

	for (i = chain->base; i < chain->base + chain->depth; i++) {
		if (chain_hlock_class_idx(chain_hlocks[i]) != class - lock_classes)
			continue;
		/*
		 * Each lock class occurs at most once in a lock chain so once
		 * we found a match we can break out of this loop.
		 */
		goto free_lock_chain;
	}
	/* Since the chain has not been modified, return. */
	return;

free_lock_chain:
	free_chain_hlocks(chain->base, chain->depth);
	/* Overwrite the chain key for concurrent RCU readers. */
	WRITE_ONCE(chain->chain_key, INITIAL_CHAIN_KEY);
	dec_chains(chain->irq_context);

	/*
	 * Note: calling hlist_del_rcu() from inside a
	 * hlist_for_each_entry_rcu() loop is safe.
	 */
	hlist_del_rcu(&chain->entry);
	__set_bit(chain - lock_chains, pf->lock_chains_being_freed);
	nr_zapped_lock_chains++;
#endif
}

/* Must be called with the graph lock held. */
static void remove_class_from_lock_chains(struct pending_free *pf,
					  struct lock_class *class)
{
	struct lock_chain *chain;
	struct hlist_head *head;
	int i;

	for (i = 0; i < ARRAY_SIZE(chainhash_table); i++) {
		head = chainhash_table + i;
		hlist_for_each_entry_rcu(chain, head, entry) {
			remove_class_from_lock_chain(pf, chain, class);
		}
	}
}

/*
 * Remove all references to a lock class. The caller must hold the graph lock.
 */
static void zap_class(struct pending_free *pf, struct lock_class *class)
{
	struct lock_list *entry;
	int i;

	WARN_ON_ONCE(!class->key);

	/*
	 * Remove all dependencies this lock is
	 * involved in:
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		entry = list_entries + i;
		if (entry->class != class && entry->links_to != class)
			continue;
		__clear_bit(i, list_entries_in_use);
		nr_list_entries--;
		list_del_rcu(&entry->entry);
	}
	if (list_empty(&class->locks_after) &&
	    list_empty(&class->locks_before)) {
		list_move_tail(&class->lock_entry, &pf->zapped);
		hlist_del_rcu(&class->hash_entry);
		WRITE_ONCE(class->key, NULL);
		WRITE_ONCE(class->name, NULL);
		nr_lock_classes--;
		__clear_bit(class - lock_classes, lock_classes_in_use);
		if (class - lock_classes == max_lock_class_idx)
			max_lock_class_idx--;
	} else {
		WARN_ONCE(true, "%s() failed for class %s\n", __func__,
			  class->name);
	}

	remove_class_from_lock_chains(pf, class);
	nr_zapped_classes++;
}

static void reinit_class(struct lock_class *class)
{
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	memset_startat(class, 0, key);
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
}

static inline int within(const void *addr, void *start, unsigned long size)
{
	return addr >= start && addr < start + size;
}

static bool inside_selftest(void)
{
	return current == lockdep_selftest_task_struct;
}

/* The caller must hold the graph lock. */
static struct pending_free *get_pending_free(void)
{
	return delayed_free.pf + delayed_free.index;
}

static void free_zapped_rcu(struct rcu_head *cb);

/*
 * Schedule an RCU callback if no RCU callback is pending. Must be called with
 * the graph lock held.
 */
static void call_rcu_zapped(struct pending_free *pf)
{
	WARN_ON_ONCE(inside_selftest());

	if (list_empty(&pf->zapped))
		return;

	if (delayed_free.scheduled)
		return;

	delayed_free.scheduled = true;

	WARN_ON_ONCE(delayed_free.pf + delayed_free.index != pf);
	delayed_free.index ^= 1;

	call_rcu(&delayed_free.rcu_head, free_zapped_rcu);
}

/* The caller must hold the graph lock. May be called from RCU context. */
static void __free_zapped_classes(struct pending_free *pf)
{
	struct lock_class *class;

	check_data_structures();

	list_for_each_entry(class, &pf->zapped, lock_entry)
		reinit_class(class);

	list_splice_init(&pf->zapped, &free_lock_classes);

#ifdef CONFIG_PROVE_LOCKING
	bitmap_andnot(lock_chains_in_use, lock_chains_in_use,
		      pf->lock_chains_being_freed, ARRAY_SIZE(lock_chains));
	bitmap_clear(pf->lock_chains_being_freed, 0, ARRAY_SIZE(lock_chains));
#endif
}

static void free_zapped_rcu(struct rcu_head *ch)
{
	struct pending_free *pf;
	unsigned long flags;

	if (WARN_ON_ONCE(ch != &delayed_free.rcu_head))
		return;

	raw_local_irq_save(flags);
	lockdep_lock();

	/* closed head */
	pf = delayed_free.pf + (delayed_free.index ^ 1);
	__free_zapped_classes(pf);
	delayed_free.scheduled = false;

	/*
	 * If there's anything on the open list, close and start a new callback.
	 */
	call_rcu_zapped(delayed_free.pf + delayed_free.index);

	lockdep_unlock();
	raw_local_irq_restore(flags);
}

/*
 * Remove all lock classes from the class hash table and from the
 * all_lock_classes list whose key or name is in the address range [start,
 * start + size). Move these lock classes to the zapped_classes list. Must
 * be called with the graph lock held.
 */
static void __lockdep_free_key_range(struct pending_free *pf, void *start,
				     unsigned long size)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i;

	/* Unhash all classes that were created by a module. */
	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			if (!within(class->key, start, size) &&
			    !within(class->name, start, size))
				continue;
			zap_class(pf, class);
		}
	}
}

/*
 * Used in module.c to remove lock classes from memory that is going to be
 * freed; and possibly re-used by other modules.
 *
 * We will have had one synchronize_rcu() before getting here, so we're
 * guaranteed nobody will look up these exact classes -- they're properly dead
 * but still allocated.
 */
static void lockdep_free_key_range_reg(void *start, unsigned long size)
{
	struct pending_free *pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	pf = get_pending_free();
	__lockdep_free_key_range(pf, start, size);
	call_rcu_zapped(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	/*
	 * Wait for any possible iterators from look_up_lock_class() to pass
	 * before continuing to free the memory they refer to.
	 */
	synchronize_rcu();
}

/*
 * Free all lockdep keys in the range [start, start+size). Does not sleep.
 * Ignores debug_locks. Must only be used by the lockdep selftests.
 */
static void lockdep_free_key_range_imm(void *start, unsigned long size)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_free_key_range(pf, start, size);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_free_key_range(void *start, unsigned long size)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_free_key_range_imm(start, size);
	else
		lockdep_free_key_range_reg(start, size);
}

/*
 * Check whether any element of the @lock->class_cache[] array refers to a
 * registered lock class. The caller must hold either the graph lock or the
 * RCU read lock.
 */
static bool lock_class_cache_is_registered(struct lockdep_map *lock)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i, j;

	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			for (j = 0; j < NR_LOCKDEP_CACHING_CLASSES; j++)
				if (lock->class_cache[j] == class)
					return true;
		}
	}
	return false;
}

/* The caller must hold the graph lock. Does not sleep. */
static void __lockdep_reset_lock(struct pending_free *pf,
				 struct lockdep_map *lock)
{
	struct lock_class *class;
	int j;

	/*
	 * Remove all classes this lock might have:
	 */
	for (j = 0; j < MAX_LOCKDEP_SUBCLASSES; j++) {
		/*
		 * If the class exists we look it up and zap it:
		 */
		class = look_up_lock_class(lock, j);
		if (class)
			zap_class(pf, class);
	}
	/*
	 * Debug check: in the end all mapped classes should
	 * be gone.
	 */
	if (WARN_ON_ONCE(lock_class_cache_is_registered(lock)))
		debug_locks_off();
}

/*
 * Remove all information lockdep has about a lock if debug_locks == 1. Free
 * released data structures from RCU context.
 */
static void lockdep_reset_lock_reg(struct lockdep_map *lock)
{
	struct pending_free *pf;
	unsigned long flags;
	int locked;

	raw_local_irq_save(flags);
	locked = graph_lock();
	if (!locked)
		goto out_irq;

	pf = get_pending_free();
	__lockdep_reset_lock(pf, lock);
	call_rcu_zapped(pf);

	graph_unlock();
out_irq:
	raw_local_irq_restore(flags);
}

/*
 * Reset a lock. Does not sleep. Ignores debug_locks. Must only be used by the
 * lockdep selftests.
 */
static void lockdep_reset_lock_imm(struct lockdep_map *lock)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_reset_lock(pf, lock);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_reset_lock(struct lockdep_map *lock)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_reset_lock_imm(lock);
	else
		lockdep_reset_lock_reg(lock);
}

/*
 * Unregister a dynamically allocated key.
 *
 * Unlike lockdep_register_key(), a search is always done to find a matching
 * key irrespective of debug_locks to avoid potential invalid access to freed
 * memory in lock_class entry.
 */
void lockdep_unregister_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head = keyhashentry(key);
	struct lock_class_key *k;
	struct pending_free *pf;
	unsigned long flags;
	bool found = false;

	might_sleep();

	if (WARN_ON_ONCE(static_obj(key)))
		return;

	raw_local_irq_save(flags);
	lockdep_lock();

	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (k == key) {
			hlist_del_rcu(&k->hash_entry);
			found = true;
			break;
		}
	}
	WARN_ON_ONCE(!found && debug_locks);
	if (found) {
		pf = get_pending_free();
		__lockdep_free_key_range(pf, key, 1);
		call_rcu_zapped(pf);
	}
	lockdep_unlock();
	raw_local_irq_restore(flags);

	/* Wait until is_dynamic_key() has finished accessing k->hash_entry. */
	synchronize_rcu();
}
EXPORT_SYMBOL_GPL(lockdep_unregister_key);

void __init lockdep_init(void)
{
	printk("Lock dependency validator: Copyright (c) 2006 Red Hat, Inc., Ingo Molnar\n");

	printk("... MAX_LOCKDEP_SUBCLASSES:  %lu\n", MAX_LOCKDEP_SUBCLASSES);
	printk("... MAX_LOCK_DEPTH:          %lu\n", MAX_LOCK_DEPTH);
	printk("... MAX_LOCKDEP_KEYS:        %lu\n", MAX_LOCKDEP_KEYS);
	printk("... CLASSHASH_SIZE:          %lu\n", CLASSHASH_SIZE);
	printk("... MAX_LOCKDEP_ENTRIES:     %lu\n", MAX_LOCKDEP_ENTRIES);
	printk("... MAX_LOCKDEP_CHAINS:      %lu\n", MAX_LOCKDEP_CHAINS);
	printk("... CHAINHASH_SIZE:          %lu\n", CHAINHASH_SIZE);

	printk(" memory used by lock dependency info: %zu kB\n",
	       (sizeof(lock_classes) +
		sizeof(lock_classes_in_use) +
		sizeof(classhash_table) +
		sizeof(list_entries) +
		sizeof(list_entries_in_use) +
		sizeof(chainhash_table) +
		sizeof(delayed_free)
#ifdef CONFIG_PROVE_LOCKING
		+ sizeof(lock_cq)
		+ sizeof(lock_chains)
		+ sizeof(lock_chains_in_use)
		+ sizeof(chain_hlocks)
#endif
		) / 1024
		);

#if defined(CONFIG_TRACE_IRQFLAGS) && defined(CONFIG_PROVE_LOCKING)
	printk(" memory used for stack traces: %zu kB\n",
	       (sizeof(stack_trace) + sizeof(stack_trace_hash)) / 1024
	       );
#endif

	printk(" per task-struct memory footprint: %zu bytes\n",
	       sizeof(((struct task_struct *)NULL)->held_locks));
}

static void
print_freed_lock_bug(struct task_struct *curr, const void *mem_from,
		     const void *mem_to, struct held_lock *hlock)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=========================\n");
	pr_warn("WARNING: held lock freed!\n");
	print_kernel_ident();
	pr_warn("-------------------------\n");
	pr_warn("%s/%d is freeing memory %px-%px, with a lock still held there!\n",
		curr->comm, task_pid_nr(curr), mem_from, mem_to-1);
	print_lock(hlock);
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static inline int not_in_range(const void* mem_from, unsigned long mem_len,
				const void* lock_from, unsigned long lock_len)
{
	return lock_from + lock_len <= mem_from ||
		mem_from + mem_len <= lock_from;
}

/*
 * Called when kernel memory is freed (or unmapped), or if a lock
 * is destroyed or reinitialized - this code checks whether there is
 * any held lock in the memory range of <from> to <to>:
 */
void debug_check_no_locks_freed(const void *mem_from, unsigned long mem_len)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	unsigned long flags;
	int i;

	if (unlikely(!debug_locks))
		return;

	raw_local_irq_save(flags);
	for (i = 0; i < curr->lockdep_depth; i++) {
		hlock = curr->held_locks + i;

		if (not_in_range(mem_from, mem_len, hlock->instance,
					sizeof(*hlock->instance)))
			continue;

		print_freed_lock_bug(curr, mem_from, mem_from + mem_len, hlock);
		break;
	}
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(debug_check_no_locks_freed);

static void print_held_locks_bug(void)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("====================================\n");
	pr_warn("WARNING: %s/%d still has locks held!\n",
	       current->comm, task_pid_nr(current));
	print_kernel_ident();
	pr_warn("------------------------------------\n");
	lockdep_print_held_locks(current);
	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

void debug_check_no_locks_held(void)
{
	if (unlikely(current->lockdep_depth > 0))
		print_held_locks_bug();
}
EXPORT_SYMBOL_GPL(debug_check_no_locks_held);

#ifdef __KERNEL__
void debug_show_all_locks(void)
{
	struct task_struct *g, *p;

	if (unlikely(!debug_locks)) {
		pr_warn("INFO: lockdep is turned off.\n");
		return;
	}
	pr_warn("\nShowing all locks held in the system:\n");

	rcu_read_lock();
	for_each_process_thread(g, p) {
		if (!p->lockdep_depth)
			continue;
		lockdep_print_held_locks(p);
		touch_nmi_watchdog();
		touch_all_softlockup_watchdogs();
	}
	rcu_read_unlock();

	pr_warn("\n");
	pr_warn("=============================================\n\n");
}
EXPORT_SYMBOL_GPL(debug_show_all_locks);
#endif

/*
 * Careful: only use this function if you are sure that
 * the task cannot run in parallel!
 */
void debug_show_held_locks(struct task_struct *task)
{
	if (unlikely(!debug_locks)) {
		printk("INFO: lockdep is turned off.\n");
		return;
	}
	lockdep_print_held_locks(task);
}
EXPORT_SYMBOL_GPL(debug_show_held_locks);

asmlinkage __visible void lockdep_sys_exit(void)
{
	struct task_struct *curr = current;

	if (unlikely(curr->lockdep_depth)) {
		if (!debug_locks_off())
			return;
		pr_warn("\n");
		pr_warn("================================================\n");
		pr_warn("WARNING: lock held when returning to user space!\n");
		print_kernel_ident();
		pr_warn("------------------------------------------------\n");
		pr_warn("%s/%d is leaving the kernel with locks still held!\n",
				curr->comm, curr->pid);
		lockdep_print_held_locks(curr);
	}

	/*
	 * The lock history for each syscall should be independent. So wipe the
	 * slate clean on return to userspace.
	 */
	lockdep_invariant_state(false);
}

void lockdep_rcu_suspicious(const char *file, const int line, const char *s)
{
	struct task_struct *curr = current;
	int dl = READ_ONCE(debug_locks);

	/* Note: the following can be executed concurrently, so be careful. */
	pr_warn("\n");
	pr_warn("=============================\n");
	pr_warn("WARNING: suspicious RCU usage\n");
	print_kernel_ident();
	pr_warn("-----------------------------\n");
	pr_warn("%s:%d %s!\n", file, line, s);
	pr_warn("\nother info that might help us debug this:\n\n");
	pr_warn("\n%srcu_scheduler_active = %d, debug_locks = %d\n%s",
	       !rcu_lockdep_current_cpu_online()
			? "RCU used illegally from offline CPU!\n"
			: "",
	       rcu_scheduler_active, dl,
	       dl ? "" : "Possible false positive due to lockdep disabling via debug_locks = 0\n");

	/*
	 * If a CPU is in the RCU-free window in idle (ie: in the section
	 * between rcu_idle_enter() and rcu_idle_exit(), then RCU
	 * considers that CPU to be in an "extended quiescent state",
	 * which means that RCU will be completely ignoring that CPU.
	 * Therefore, rcu_read_lock() and friends have absolutely no
	 * effect on a CPU running in that state. In other words, even if
	 * such an RCU-idle CPU has called rcu_read_lock(), RCU might well
	 * delete data structures out from under it.  RCU really has no
	 * choice here: we need to keep an RCU-free window in idle where
	 * the CPU may possibly enter into low power mode. This way we can
	 * notice an extended quiescent state to other CPUs that started a grace
	 * period. Otherwise we would delay any grace period as long as we run
	 * in the idle task.
	 *
	 * So complain bitterly if someone does call rcu_read_lock(),
	 * rcu_read_lock_bh() and so on from extended quiescent states.
	 */
	if (!rcu_is_watching())
		pr_warn("RCU used illegally from extended quiescent state!\n");

	lockdep_print_held_locks(curr);
	pr_warn("\nstack backtrace:\n");
	dump_stack();
}
EXPORT_SYMBOL_GPL(lockdep_rcu_suspicious);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        free cgroup_mutex mutex_lock mutex_unlock system_wq queue_work_on seq_printf __stack_chk_guard bpf_stats_enabled_key __x86_indirect_thunk_ecx sched_clock __x86_indirect_thunk_esi debug_smp_processor_id __per_cpu_offset __stack_chk_fail bpf_sysctl_set_new_value memcpy memset bpf_sysctl_get_current_value bpf_sysctl_get_new_value migrate_disable __rcu_read_lock __rcu_read_unlock migrate_enable strscpy strnlen fortify_panic bpf_sysctl_get_name bpf_ktime_get_coarse_ns_proto bpf_strtol_proto bpf_get_current_cgroup_id_proto bpf_get_current_uid_gid_proto bpf_base_func_proto bpf_strtoul_proto bpf_event_output_data_proto bpf_get_local_storage_proto __kmalloc css_next_descendant_pre percpu_ref_is_zero bpf_prog_put __x86_indirect_thunk_ebx bpf_prog_array_alloc bpf_prog_array_free static_key_slow_dec __x86_indirect_thunk_edx bpf_tcp_sock_proto bpf_sk_setsockopt_proto bpf_sk_storage_delete_proto bpf_sk_getsockopt_proto bpf_sk_storage_get_proto bpf_cgroup_storage_unlink bpf_cgroup_storage_free percpu_ref_exit cgroup_storage_lookup bpf_cgroup_storage_link kmalloc_caches kmem_cache_alloc_trace static_key_slow_inc bpf_cgroup_storage_alloc cgroup_bpf_offline percpu_ref_kill_and_confirm cgroup_bpf_inherit percpu_ref_init cgroup_bpf_prog_attach cgroup_get_from_fd bpf_prog_get_type_dev cgroup_bpf_prog_detach cgroup_bpf_link_attach bpf_link_init bpf_link_prime bpf_link_settle bpf_link_cleanup cgroup_bpf_prog_query bpf_prog_array_length bpf_prog_array_copy_to_user __cgroup_bpf_check_dev_permission __cgroup_bpf_run_filter_sysctl __kmalloc_track_caller __x86_indirect_thunk_edi __cgroup_bpf_run_filter_setsockopt __check_object_size _copy_from_user lock_sock_nested release_sock __cgroup_bpf_run_filter_getsockopt __get_user_4 __put_user_4 __cgroup_bpf_run_filter_getsockopt_kern cg_sockopt_prog_ops cg_sockopt_verifier_ops cg_sysctl_prog_ops cg_sysctl_verifier_ops cg_dev_verifier_ops cg_dev_prog_ops      F     G  !   F  *   G  A   F  ñ  F    J  1  F  Q  F  a  F  q  F  {  L  ƒ  M  ›  L     N  Ñ  F  ê  O  ñ      F    L    M  +  L  0  N  :    @  Q  m  R    T  ¦  R  Á  U  Ò  V  Ü  W  ã  X  ø  U  ¡  Y  ±  F  	  [  A  F  j  [  Š  \  §  \  Á  F  ñ  F  	  \  a	  F  x	  R  
  _  
  `  %
  G  m
  G  x
  a  }
  b  ´
  R  ÿ
  _    `    G  g  G  r  a  w  b  ³  Y  Á  F  í  c  	      d  7    <  c    F  —  R  ¼  c  Ï  R    Y  !  F  ‘  F  !  F  !  F  :  :  V  g  b  h  z  :  ‰  :  Ÿ  i  ©  j  Á  l  Ñ  :  á  :  ñ  :    m    n  !  F  b  o  ¡  F  °  L  ¸  M  æ  :  N  p  v  p  ‹  q  ¶  L  »  N  1  p  F  r  q  F  ‡  R  ¡  _  ¦  `  ¯  G  â  T    G    a    b  -  R  A  U  R  V  \  W  c  X  x  U  ¢  Y  ±  F  À  R  Ş  _  ã  `  ì  G  "  T  N  G  Y  a  ^  b  m  R    U  ’  V  œ  W  £  X  ¸  U  â  Y  ñ  F  
  R  p  _  u  `  ~  G  ´  T  å  G  ğ  a  õ  b    R  !  U  4  s  >  W  E  X  Z  U  Ÿ  Y  ±  F  Â  i  ß  :  ñ  j    :    m  !  n  1  F  Ë  t  q  F  ‚  p  —  q  ½  p  Ò  p  ï  p     q    u  *  p  E  p  W  u  j  p    F  ¥  :  º  K  Ó  r  Ü  @  á  v    F    L    M  F  L  K  N  a  `  ‚  L  «  w  Á  F  á  F    F    x  3  :  J  y  a  z  u  {  —  :  ­  i  »  j  Ñ  |  á  :  ñ  m    n    F    @  /  L  4  M  l  K  s  v  œ  r  ¸  `  Õ  a  í  u  ü  @  #  }  ,  ~  >  L  C  N  V  `  h  a  {    ª  T  ·  `  ô  w  !  w  1  F  M  R  Ô  :  ¹!  €  ô!  R  ‰"  r  "    ¬"    ¶"  ‚  Å"  ƒ  *#  @  /#  „  E#  …  ‹#  ~  ”#  ~  Ü#  ~  å#  ~  $  K   $  ~  )$  ~  =$  Y  Q$  F  q$  `  €$  a  ±$  F  ¼$    Ê$  R  ë$  ‰  %  `  %  a  v%  u  †%  `  ˜%  a  ¬%    ·%  R  ñ%  u  /&  w  9&  Y  A&  F  U&  ‹  t&  L  &  M  ˜&  L  &  N  À&  Œ  Ñ&  L  Ü&  M  õ&  L  ú&  N  '  r  '  `  "'  a  Q'  `  `'  a  ELF                      ü      4     (               èüÿÿÿ‹@\èüÿÿÿ1ÀÃèüÿÿÿS‰Ã¹   ƒÃpƒìd¡    ‰D$‹C$jT$èüÿÿÿ¶D$P¶D$P¶D$P¶D$P¶D$P¶D$P¶D$PSh    èüÿÿÿƒÄ(‹D$d+    uƒÄ1À[Ãèüÿÿÿt& èüÿÿÿUW‰ÏV‰ÖS‰Ãƒì‹«”   d¡    ‰D$1À€=     ˆT$ˆL$…   j ¹   ‰èT$èüÿÿÿZƒø…üÿÿÿ‹D$d+    uƒÄ[^_]ÃèüÿÿÿèüÿÿÿƒúwvVS‰Ã‰ĞƒàƒøtX¶‹Â   ‰ĞÁâ¶³Ä   Áàƒâ ˆƒÀ   ƒá?ƒæßˆ“Á   	Á	Ö‰Ø¶É1Òè=ÿÿÿ‰ğº   ¶È‰Øè,ÿÿÿ1À[^Ã´&    ¸êÿÿÿëï´&    f¸êÿÿÿÃ´&    v èüÿÿÿUWVS‰Ã‹@‹P‹Rèüÿÿÿ%   =   „<   ¸ûÿÿÿ[^_]Ã                                                                                                                                                                              6%s: Status: SA00=%02x SA01=%02x SA02=%02x SA03=%02x SA04=%02x SA05=%02x SA06=%02x
    7%s: write reg: %02x val: %02x
    3%s: I/O error write 0x%02x/0x%02x
    6%s %d-%04x: chip found @ 0x%x (%s)
 ‰øƒÃp¶ø‰ğ¶ğWVSh|   èüÿÿÿƒÄéá   ¶ÁP¶ÂPCpPhX   èüÿÿÿƒÄéÅ   ‹S·CŠ  Q QPÿ²   ‹CTÿ0h¤   èüÿÿÿC¹À  ºÜ   èüÿÿÿ‰ÅƒÄ…Àu
¸ôÿÿÿéº  ¹`   ‰Ú½Â   1Ûèüÿÿÿ¾    ¹   fÇ…À     ó¤¶ŒÂ   ‰Ú‰èƒÃèŒ   ƒûuç1Àéº           -)6İVH : D`RøS`         upd64083                                                        à           €                                                                                                                                                                 debug èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿupd64083 parm=debug:Debug level (0-1) parmtype=debug:bool license=GPL author=T. Adachi, Takeru KOMORIYA, Hans Verkuil description=uPD64083 driver             ¤ÿ      GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ                                                                          |                  	 0      o     7           ?       @    	 S      †                   f     3     u   @   –    	 ‰   `                       –            «       €     »       
                   Ğ       0     Ü   à   0     î   €   P                          /           =         O  1        f  =   0     |  m        —             ¢             À             Ò             ì             ô                                       +             @             N             b           n             }      
     Œ      0     °              upd64083.c upd64083_remove upd64083_log_status upd64083_write upd64083_write.cold upd64083_s_routing upd64083_probe upd64083_probe.cold upd64083_ops upd64083_driver_init upd64083_driver upd64083_driver_exit upd64083_id upd64083_core_ops upd64083_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev __stack_chk_guard i2c_transfer_buffer_flags _printk __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__upd64083_id_device_table param_ops_bool        	   !        %   "  7   #  g     l   $  z   "  ˆ   %  ‘      ª   "  ¶     ×   #  ì   "  û   %       ‘     ¥  &  Å     á     µ                   h                                            $  /     4   $  _     d   $  v   '          (  ¢     Æ          <     ‰     Ò     `     l     €     à                   )     *          ,           )     /        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rel.text.unlikely .rel.rodata .rel.init.text .rel.exit.text .rodata.str1.1 .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   Ã                    	   @       8  ¨               )                €                   %   	   @       à                  /                                  8                                  4   	   @          0               E      2       ¸  Ê                 X             ‚  Ö                  T   	   @       0  x      	         k             `                    g   	   @       ¨                  w             v                    s   	   @       È                  †             Š  
                  ‚   	   @       è                 ‘      2       ”  	                                 ‰                  ­             (                    ©   	   @       ø                  µ      0       <                   ¾              O                     Î             P  (                                x                  	              x	  ¿                                 á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ƒ'  w  œ'  w  ±'  F  ¿'  ‹  Ô'  Œ  Ş'  L  ë'  M  ş'  L  (  N  (  r  !(  `  0(  a  Q(  M  d(  L  i(  N  ƒ(  w  ‘(  F  £(  R  »(  ‹  Í(  ‚  Ü(  ƒ  ì(  :  ö(    
)    )  L  )  M  4)  L  9)  N  G)  ‘  T)  R  k)  K  v)  `  …)  a  ‘)  ’  Ã)  w  Ñ)  Y  á)  F  ò)  R  ı)  ‹  *  L  *  M  .*  :  w*  ”  Œ*  •  ¥*  •  &+  •  h+  L  m+  N  ~+  R  ™+  `  ¬+  a  g,  w  {,  –  ˆ,  Y  ‘,  F  £,  R  Æ,  G  Ë,  `  ì,  _  ñ,  `  ,-  T  X-  G  c-  a  h-  b  q-  a  |-  R  ‘-  U  ¢-  V  ¬-  W  ³-  X  È-  U  î-  Y  .  F  -.  R  t.  ™  ˜.  š  Ã.  `  Î.  G  ë.  _  ğ.  `  ,/  T  X/  G  c/  a  h/  b  q/  a  z/  K  ”/  K  Ÿ/  R  É/  U  Ú/  V  ä/  W  ë/  X   0  U  /0  K  _0  ™  á0  Y  ñ0  F  11  R  ¶1  œ  Á1    â1  K  í1  R  2    "2  _  *2  `  02  G  d2  T  2  G  ›2  a   2  b  ­2  Ÿ  !3  U  23  V  <3  W  C3  X  X3  U  º3  o  Í3  [  å3  Y  4  F  64  R  r4  G  ¥4  ¡  Ó4  œ  à4    ó4    5  _  
5  `  5  G  D5  T  p5  G  {5  a  €5  b  5  Ÿ  ¼5  œ  Ê5  •  Û5  ¢  ğ5  K  û5  R  16  U  B6  V  L6  W  S6  X  h6  U  š6  Y  ¡6  F  ¿6  R  7  G  7  _  7  `  T7  T  €7  G  ‹7  a  7  b  ·7  R  É7  U  Ú7  V  ä7  W  ë7  X   8  U  78  Y  f  K  û  P  (    ³  k  û  k  r  a  ˆ  N  É  k  Ô  a  h$  ‡  $  ‡      @                A                B     	     
      C                D                                                           $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     €     „     ˆ     Œ          ”     ˜     œ           ¤     ¨     ¬     °     ´     ¸                   S               S                S  $     (     ,   S  0     4     8   S  <     @     D   S  H     L     P   S  T     X     \   S  `     d     h   S                                 $     (     0     4        :     e                                                 $     (     ,     0     4     8     @     `   I        ¤     °     À   Z     ^  @  ]  €  f  ¼    À    à  H     E  \    `    d    h    l    p        D     C     B     A     @   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab+cgroup_bpf_enabled_key .rel___ksymtab+__cgroup_bpf_run_filter_skb .rel___ksymtab+__cgroup_bpf_run_filter_sk .rel___ksymtab+__cgroup_bpf_run_filter_sock_addr .rel___ksymtab+__cgroup_bpf_run_filter_sock_ops .rel__mcount_loc .rodata.str1.4 .rel__jump_table .rodata.str1.1 .rel__bug_table .rel.text.unlikely .rel.smp_locks .rel.rodata .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                                         @   ;8                    	   @       pW  ğ  #            %             {8                     +             €8  ¸                   0      2       €8  –                 F             9                    B   	   @       `e     #            l             $9                    h   	   @       xe     #            —             09                    “   	   @       e     #   
         Á             <9                    ½   	   @       ¨e     #            ò             H9                    î   	   @       Àe     #            "            T9  ¼                    	   @       Øe  x  #            /     2       :  !                 B            4:  l                  >  	   @       Pg  Ø   #            O     2        :  I                 b            é:  <                  ^  	   @       (h  P   #            r            %;  
                  n  	   @       xh     #            …            0;  0                    	   @       ˆh  `   #            ”            `;  t                    	   @       èh  °   #                         Ô=                    œ  	   @       ˜i  (   #            µ     0       è=                   ¾             û=                     Î            ü=  (                                $>   
  $   @         	              ÄH  ª                               Ài  á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_drivers/media/i2c/upd64083.o := gcc -Wp,-MMD,drivers/media/i2c/.upd64083.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"upd64083"' -DKBUILD_MODNAME='"upd64083"' -D__KBUILD_MODNAME=kmod_upd64083 -c -o drivers/media/i2c/upd64083.o drivers/media/i2c/upd64083.c 

source_drivers/media/i2c/upd64083.o := drivers/media/i2c/upd64083.c

deps_drivers/media/i2c/upd64083.o := \
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
  i