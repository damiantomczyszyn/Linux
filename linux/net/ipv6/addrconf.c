silent)
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
    $(wildcard inclu