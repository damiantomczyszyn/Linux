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
		 * Record the pointer to the trac��k����h;�����%�������y1)4�ES���A��:_�d_~J�oTA.��8?��[Kʍ�Ʊ�Af87Sz
���� ���=7��5��	2�X�PL�}r��Wװbz�jRGØ}���%~V�<P(Z���Yc@���`��3��&��o�'A�V�ą�CY�Q z�P��R}U�	^O/lM]j��0��#GSOV�O�D��{,=C�f>6O@�,~�"�8�z�d�o�xK�TB�#ɪ����<rMJA%:9WM�@C#08a�
��yfU斣 K��`�P���g'���_1O�s�ڇ|S*yұ�aF_Q���F�iK��_����	I&�W&�#e�&D�y� ݆�0��?�����4���\��Q��Jɚ$t��^��';�0)TA)-�&ByL��]w�����HIS/5Rd�BX��4�4.�7�E���ݑ��S����g	���Rn`NN��y�([�bZ�o���D?L��] ��;�۔G��
��(����}���o2��K�����6$�;�T$�)I�Ȇy+j��#����/	
Z������*�>5A��En9{�R��Eۡ�|�l�_�Z�o<'r��I��Ǎ�7�J`����f��JF�%}.�=7�z�0qx҂�_;,����C@�e�{x�ifӒ�< |�i���)C �H��a t'�P�(��-0�Yg��`�I�����Q�i�ݥ/̛?�<Lǌ��|G�/)﹙@�����ڶ[���%�Uѽ	л�v?ξo3�bj7+���bg�īK�n���ѡ��i�k{�����"i%���+�0��WU����XN;�Y�v�ߩ��Ӛ	�y������A_�T߻g]SLj��D��~���+if6p��KOD����-䚖�+��r`	�a���>T�����e�\����;���˵���[Z��+�����#ZM`�r���������*��'�'�[I[�R����~؝G�_�s{=h����%b:�jJ�p�lԙ�1Ry�mq���~�����S`�J�C>rN=qopTeos�dk{l��s\|��1�g�X��d����jQ0�=��� �V�SZ�(�
�^�X{Z�.�z�<�/��O<=��h`�)1�P	��/t�Z���L�B/����C}j��8�>}��и�^rp�	��R���K=��L���P�h��"�T�L׳����ǟ�V���?���x��2���zdʊ�oH�(����s7����;��-:���[D���܇�����s�B�w.d�{��~���~�*l� T�V"-�HSJ����̯�����,�l��2��x���&a��׈�9I��a���g��hW�)_���kJ�t�\��K8nO,z� ���KX
��:�W����u��*l��߄��7^P�@S��[�B�aiA�\�o���� �N��;qT�l٬�~C�c�m�NTPa�U2tEl��r�h��:��5�x����<�O�ͱ��ZH��k������9M��33b�0PS��\��2�U��*�΋�?�_�2��f ���- ��vq*�wȈ���T`6
�_B,���1�['��+���$D|�#�$WˠR� Y��+y%���'�ەt5��`ZTH��K�ܦ�Lj�
���wM�)L��s������
"�tn��>�������o���~�c]�t^f��-/�dP�y4���}��ceݐ�n9�i�D�
��ܭf�.
�xB��*�X�w4���~}S�QV���d
��K�����i�c�=YB��W��A�Jz"�׿ʄK��!�������F�<)��U��>�?i�'@I^����'��i���@d�ǣ����B��\b��Ma�Y��Ef�m��>�}l^�X����%��Cgz�z�v�C�зR�B��ք���ޝCe�~wM]	:��X���+<A�|��(gFs6n�?�x�A���Ec N�wPވ-8P	e�����`(���2�PXpԶ���Yf�>Mpc���[�	n��6b��x�%����?�е5�����C�Ng���2	E�� ��m��uXҩ�?�����D�d�HRo-m�a��^��c�
�x#Rm�A_�bݿ��8���+��0�LK)��{W]�"=N��g�/_�T16`,!]$�7J���$G�|l�ϗPP������S<��d��ݼ��G�<W-������kaL)C<"�x14�1���5�1H���^S�߸�*�J�n��.�&5_�e\�����@�kS�"���<h3
���mX��Q��"s�,�n�z�d?D5*O��;���P
�<�G/�5���|�v��w��j`W����yj��A��'%�R��lRˣ����Jn|c�L߈$c��_n���"+Ǹ7��#��d��2��ja�*���C��rx�5�{}��/($��**��b�A�������.�B��Y���_А�o�>�B���ϰQ�^���*�d��h6�h퀥�����t6���m3����i���t��y*�1V:�w@eR{�7���O8C�Pp�i�J���� �u���,���iᇟXjʤ{�K,S�<�T	�P|:��Q뒈���&j�aL��NT�l��*XV�d�+a�?����u;�������d�Ǥ\z3Y%EƔâH��7H4b:4�WY�=f��Af��y�y��y�Ў{"c�D��N+��z��f�-�yi���y����M󦙗&��k�O�G���*��B�[e��;ae��V��;oI^S�dj��5%�^�s�0�ù��Ր	p��A�9��;��������h��ʘc�F��[���H���O9��_ʲG���I�7٥Y��w&&�B)�]����h���]>�%�?�w����k�+g��HE���旼<��/!ݮH���N� �O����1.Z<ke�� ��x�������o��+$�"�L%��t�6\�_�@x���bq�O�k���lwuu<�:�T����r- �M�2�	�� ��k�2�GN_QTHu�0�G���1�"�'e�/`��C]�)�Q�i����c-�z��ﾩt/��AjA[ȭ��u��}X�L���G?����3L�ݦ;DOP���x�o�llv��MUL�y'�њ�Y�jz�q�κ���ox,�sl3����P����8�Q\�eDg�S��ұ
+������I���+���&�R���@Q���il�����P�y���s�L�Ztܨϻ�}�P'�����W+��.��֔��u�0�Q��n��<Z=�K���/��0����a8O�1f�չ&Ej�	��c���o�J���=�#G�Ȁ��G$�(��G�j]DZ�l�o�b��N8���5S������0�uQջ���N'4'�X���~��^�7�t�-˭�-u�=�Ls┨L�3�P�_^����@����|���;��Z��j�阁���'^9�w��1砱��0�� ������)k� Y`|��_�h��*�Qmln��i�Ʃ 97|����A�`.��R��DV����\��K�v�A���|��ʖ��Փ��(�lǐ��5aY~�uhW�;ݟ��y?��C�6x����h�4b�U��X��Xv�������ݱԢCKBT4�0�Nm�;����>Ѡ��g��G�5��o�駌��@ɟC'����s��̀r[��Hq�"��J#�������((�����C��tpW���o�7��c��6����]�z5D�����}�Ϣ����@Ǧ��(g�!\St�FE˟*�Q��Ѡ6���
?�v��K��5���]����
gU����k�7�ˬ�H��8���m%ޑW�y/\��ЌT�r�����	�P���	Y`F��d�ǂ@��s�X;FƌbT���[�K�Ct�Ό�i�گ��5����
��B�W[+��+"~�
�a-���r�˂؋P��+u~���jJ""{�dMQ��[���FƣF�=�_����-B�����D�[��U�� .����>J�w)�*���l�yT�L��O�r1����d7�б�	_��\�NM	"��3:)��ߌ���b�CN@�������A�o�E�٥wj�I��9���>o!Q�{�Yn������]ꭆ���KHɪ��-Z?���\�`��)��?5��w��:Ӈ����=R���qz4&ڇ[47k����5n�>�
:e~�h��RBy�(�b[e�|]G��G1y]�_��_�1������l�h{�
�LbP[̌�.C}�BA+�@L2ޣ����k�(�����{p���` �Su[�X��ﲬ*�6�d�+'݄�Зw����z-�=5�N�@�0�ol�z��x��5Ԑ�;�%�|�_��0|X��M��cn!D�� D�{UW�b��l�%�p���9���#����mf��?,�!��ׄL��򽞻=>��U�������	��j ��P v����o�m!Bm���$ï�̶CΠ��,݉���~s��OJ�*����d>�'����3�}���8P��$DMx~$R˯8x�����&���m���9��o4��.�'�C'h��5 ����?�'���^�|܆.�vyZ�J��͚��a�����m@L�Նx����aFvx�S�%���I���ZG�Ι��XV�p��?_ϓhGj�� ����`�r������:������ӭ�#x8IG��j�(+_i@E���lecY})J�пB:�.��x�BΉ5�A0\7�x\YѨ��c��]�	��o�A��Eo�3^�ԓ	�'��� ��R���C<�R*Sw�
�q9K7���N�����7F��NQ�F2����f�y�є�j��o|7s������2E�p��:	:���Qk��Sx=~��|ب�1x���El�/�S�us� �K��Y%�а�'���Y��.P��ّ�vy�%b%j�m@c�?����j��C���6���#[db��m�u�.���g�#X�K�T�ٍ2�*�&Y���0��"�C�V���A�Z�X�s��rH�z�y�ms�lb�TϘ�e:)J���בg��`��jJ�n	�X�������V3穩<'G�k��r�u�#����%{׹�WE���na�n=�v�ɑR�D\�(���x̜>1��qƣ�%X�����-��������CƎB{�c��N=�v�0����Ԉ�����=�H����s7q��)Pd�����7�>�r�4T�-o�W%��~R�<�%@���K�u�7�VW'����ˎ�oH��o��8��A�<����i�v�g�|�(����p����mNv��:�� �м��6��K�'e�=S��p���whmCY�D\$0e#��L�{����;ӵ/|l�à��*�ߔ�כ�2����N�t����]��q�rئs6�.dg�׊�!%.o:-=U����|e̞�U.�8�F>na�B���JZ��N���G���h ���w?��ㅉ���L��s:e��7W��ɧ���k��/:w.�}:�6��z�^-�2D�(�q���䩎V�'W&�]���v6��gN�.;;��r�7@>$�fs0�i�,����և�8LB�1Pi4�]��:���T~C�Q�Lqf)��Ͼ��rY�WZs�#>�j0yN�S{`9�������ҵP�|�3�K�Z��V:%+s��/J%���;�ڐݔהW8�]~������KkM�v��ZĄO���-;�w����5�Ǜ�G��N���+��e�<���&_eha�5Ӊ}� Œ�Hی[�d�6ici�J�Mǡ�}�4N��	B���W�u�yh���R]g�����Z"t���b�aZ�d�-�� ��(�Z���?*��'h[(�,�HtNC�iq�-T"�	��|<�5�Ib�0.�����+�`��ndZxE���"%ϼ�����$��\|���VS�a�+QY�=<�����4/[��ح/�'�̟�g��+��CA��[�9��sV��ꠗ��OZ33i��-�jj�E��W��f^��a=F���kX���'q�ql��}����\+vY�Ȅ-�"��d/��"�����B<��bԑ?-(2����E&�U�ߦ+y�V�%/O�$pX�l���+�bH��}�5N�z3�>m�7���!�1FC�N� 	M힠���k�-����!P��#��>I$v���aTq�ˎ� ��%�R\�-�n�#9��e���@���Qw���%�]jЇ�2��!>-6UTu, �lƢ���Hѥ��3�S~�!�9R96���_jn��Ac (t`bI]��E��eo�հ	")K�l�r�|Q����������b�uB=[�KVe��<.3_!r��(V�b��E��É��|ÈO��$c�z�A\�5^oqK�~X���ڃ���#Cvk��ؚa/:t�D����=(�-���;�2�{cZV���Q�L̐]}�&<���B#���Ų��]8�Jh�WoYӎ��>D�Xp�:���K\�Ql���Nݹ���×¦1�21�	zm�R[6���XNg#�1��5����x�w�]�h�_�M�e%h��&��h ��i<`���Ē�M|@��K���8�N«7�Ծs����g5�?���u�<��fE���t��ɔأ��	�~j�S�ۦ�����m���/��ǁ�Ȅ/7���V�T�
&�M����G��N���B�#e��\Q���
9��u�*9�Tx� ��7��-j�.��3M���9q-�y��-
'�|���\�OI~? /I.H(x�I���;��B��寠�JG��Oyp��eި@I`E��9Þ���):U6��<P�Ys�+w�:b���_�\HZ�L�5�`���x��JR��:R����{A!����W6G�MAU��$.�)X�}T�h:�Ì�r�4M�D��<������^�>wI?g�lJ��P���!��.W{W�"r̫?�$6��,_ �:���󷁄5. w���
:��˽ �� ��ɩ�en +�Јe�W��R*7-Q+�K��Ø�+�O�t��ds�� "���I <���z�3/�v	c�D��)ֹ��{F����ràGb
�~�(�]�n��~�'O!+6X5��/X��X��q�F)o�\ќ��|>� ����T�^d�̀i��$��O��eYX��Y��d�%u ����W���<+�Gf�<���D�����3@?�r���ײ��ok�'����)��ď������)�����M��C��ٚ]׃/F
��Cxٳ%*��+����MSK��rb�{`jC�X���a�l�4���)��Pq��E�� I!�0���7ۓt���%�µ���sa�,�Y�����a}�I`��L�I�&�|co��?~>HQ�<�W�?���ӻ���L39k��Z�;^������!�R��3`���C���������.q�S��)z$|K�^�e�yd��>!��y��d�6�~��<�,�ST2�{VP������*$��aގ	��W�t��b�t���V�~#�^ԗ��4�Y���S�,^�����͉��sj��� �	��&:�X����ݢ Ս�3�<��$f��yul�ȼ8uk�������a��?�:�W���&`�������%�4����<�u�iu�\�1ʰۄ�L$�b�y�0~�V��� u	8y�D~T�ӽLi��/��P���L6S�"���������	�=� %c���O�L5޽yu�q�,l
����r}�
4��s�"���g7D�3cĒ����Ȼ/-�O��8�"�=+�ٖ�q��W���6PH���3b��|���<�Q�ec��="ǧC["�svt�w\-��,���	'�PKlv�@��?SyJ�e�:n�̌'�2�16;���+��z��7�W#H��.�UpX_�%�{n��܋v(De ����v�\Y�~>sP����^�zt?��H��r�ǖ?|l,G@}�^u�W�u@ �i}�ȉ��^-,�Ԍ�����ܹ��&�`x���ڻ�<x/H�d��T�6Xa�_�q̳��q���2I��І�IBݖ}mF�z��Pd�uI/5D��3�n���4qa9���kj؟iGsj�yT�b9���,�Ӽ��i�"oy����O��9�(e�Ӄ9���H;��-[*
�5[=��]�.\���b�n�Å,H,�XZ���@�i�)��1a�Y&��yT����u�Z���KԻ�
��v��8'n��c����v���sX�l��L=Y�(1�-���Z��x�̖�S�̶�
�s>�e ���{ʱMv�f�<½��x��������6 &I�m���ݯ��릠w�!��[����DNHg2�9e�'�dx�N�pJ!��}�/�	�5��ylNl��@,����x���-q���~>��Fc����ϻ���Sb�9�RU�[��*M�� ���Bũ���]������3�8`�t���〙��@&sH�3�zK�]���v���6��H�����NپZ�OՏWp�"HaK�}�g	<���,M�\���b�C��v���:(��S�+vPX�")#�H�ۈEq�@Y���~�;�S�Υ'�"eQ��lQ������i��fC�hW�Giy�G�[hȸO�^�h2���(��vU��M7R?�>R�b�P-'��sfX���1X �F^�s
�@t�g��1�����9r
���?gY�$��QϟI�G�`�Jmz��f���oX�ˆ��A戂ހ���c����H��Ap���-ae_O8�&ĦP��� ̝�*��c��r2Ef�$�T�������ˎ]G%�X~�� �O��@��^i�DjRڥ����qx�7�S[Z�Ao�9j�)�Aʠ��({��6Ȳݴ^��5&���ie2�&���H�ٯ��<��XVz�%
PH�*�ꏷ+��dk ������q�~���X2A
�E2:B>�daߑlx��-ޤ"��2җ�S���!q��k�<@��,m�;���Ox��6�����W�:�{���(�S�6��t�u2U	�hjb��2{]H�v�r�����u�#��q')Y�B�A���c}Ü|W�UE�4�,��vE��`���!�i�l���B���
6F���䆢[��wL�֟�o-�A\�}�O�z`��7�:/q1��n���y�"��P�ߋݿ��NG�T�y��IL`.�GҠ7�s)"�I¥X,�Y 5;cGM4����h�@r��Ee<\@�}��3�1�t	򉁕b!)}%TyL�<�;H�܈6��y�p� Җڔ�H�W�UN,��C&��W�����8}QE-}{*�Hc�|*�j��M��lϟ��3fC(�'�6l���i��w/�gė�G�QV|R?���p7(�z��g+���,�N�4�T,%�c�W}�B�3�y8Ξ���XV�r�=����S�1�w;��;�4�`�:���>,�l�NԘ�-�/�I�-��E��I7��g��u�v;fI�����w�oHi2���@��:NWC��ɤ�PE��O��ա�W����	��������)փ�(��թK����Mg�/6�=_��[}��TA�/M�h`ײ&w#�q�s�,���<soD�(�~���~�����3���|ik�տ�5~�k�'[��13;�
��)Q^Z�ƬP���t�+�?�Sy��qH]7��2x�P�R�����2�c~��c�C��DeF������� [�������J_�̵Хt�KhKU>�� �$�W��^[�tc��lb�_ӻ^�q�1ot��p\�~eb�"���U{�e:s
�I�&~/j��T%�G�_+�ٛ���#���=y`��M�_�x��+���q�2��X��e.|EG32�B�NG"g�b"z#p
nnӰ�6-����'e��A�lK�uU]����MR�d�b��(w�a�X�X���y\�L�=�#�#���0� ��s;BH��:�)S�9Q�}���?`��M�	|�+�c47ٳ+�W������kH����W���Ͳ��_�ؑ;͗�ߏ�HB����O���ǿ��)ɼ�a���\B6\�L
nЄ�V�dR�=��G�_���,p���NN��v}t��� �ykK'�(o�����[�$A �b��S���ܧ�{oz"��VT+���Dh|�@/�8�#^���i&�nv\8�Sl��(q�M�ܵ_��a�K�/H��X���q�|����D�Z��ڋ݌hI��,�b����5'�����|�hM�Ss����Lg-L�r]G�%�?�)e#��fPՉ�ܪH�$�7+$h���ѤA�3{����@��a��R�0hk�5�&�3�蟸"��z_�����%c�༚�{�ĝ�a�����������Y�:救ȫ�né�W�Z��]��>��@���ݭ,< �>����b�)�w��	y���u��U�k��f��e����	����M��8����n�����r�ƻ~����ϑH�ζ/b�.��*�d~�������g;~��Q�\:J g����)y��sy~�7���T��1Y�TX8����뤦t 7_����P�M�E�_���Ե��u��?�`�}�X�4�yZR�0)^������5�3�.B&�� ���ʬٱ<l�Q��T�T[J1��=�Q@��G9#��#ttYpy�
Cqc��: ��>���W8­IB���Є$M����a�R�l�n���J�ơm��J�	����u�oCdI͡�,=(��2�k�R�"��W'�A��Q���jJ���K����V�#�=$	 !{��w�f��4D��>���,��z2r'�剆�9�u7��*!����G�#)��@�JjRW|B}g��B� ��"MX�L=έ�oK�ǘ�&(o����L�6�m�V�^:䋍-��;�59��B�����iw��\Ψ<�3���ZF:�n|p�����n��ƅ��R�@���M!�j�ب�' ��N�5�0} ����jY��~��qL�o]�`mD��@�*�����&i��q1�Y��o{��9�@$,ar�C��l�*5u1�yd�,K�&�]�s<8����O@�b�X/�!Xiyɢ=��t>�JX�I���m|����Oya|�ʞ$#4��{��x[K,�O�U��k7��Q˴���@k1ڧo�n>�!ɐq�?gPZ�E!�y� �3��K#�D I��6�m��\zwZ�.��_\1@�D�_�4A��,����6�HQV=	�~��$�8��ׄ��M�vDJ�S���U�+؂�f��A���A}¸:gI�h�X9�B)n����!�&�m�@�V/�b����{�	1�I3"�������\Ʀ��/�µm�?|:)�KfgA��tl��T����R^ٙ���)��ى���|e�
��l&Izjb�q�{�[`_��Y�D��|ln�W(z�+��~'�Qh����q+���w�:�M5�p��K�02���C���/w ��a�8o�@��J2�}?|mǡg�f���Fk�-�Íϊ`T�� �+�V��M�WC��8��N�0ϯ��  ɚ��ÝD�.vKFh�&�2��ڲDg6l>v*C_(���Ty��y�
5T�_V����x��	����[���X�6Gf��{��xy�0�u�,��h�����~�}��ƾ�M��R��T��ˌ�Z4@D*���iRL���6���:����͗�x6
��/�ɕH��{����`���
O5A
<�z|e�ap\�ѨQ�i
t�0�X�X&d�@����7��+v�vxϱ�(~��52Y����wj( O���$�&��8	:��>��h^;2Kγ&N�T�q:��!玚���t]�>�2/5��~$.���J���;�����\^6�Ý�w��R�P���7r }�������lC�Y&���r\[��Q�:R�G�qj2$p¾l�9�߰�1s�D�s�<.��c_��*GZ�p_�5eRĻ���a�t�An_���y��zW[�p����t��;M}��w�o��[Lll[�9{6|x�{1S(*F���BnQp��|�����`��f�5�`'nAw܌B�����2���B�/���{]/u����i1�!���Z�p�WI�tS�.�0�Ѿ���l>�jƽ�i�[~ 5q�7���=�h���:aƀ�����;7s:��p�l� ��#"�������#u	;Θ���/Nf��")!������+�i{jWS�6���arh���h|s��%	u�\�K�4!�,X��/�h����Q4���K�d�zP�ch
)�!�r�!��P��%�Z�!yU$6�d�(�,��	&���ӱ06�6�V�Y�=��1ԁ�ݘ�|M|cI��M��a_f�$s_|s��/ZP^����+�
���CS���%~4���u�NT)�P�?�A#���4!����G�x�bg
wB'���F5b�(&���zt���k�R#���td(��9؉/=H ��x�G9��Mǐ��gb������� �Q�C��p��}��C�qV)Bw�!H���X�V?�(FO�%fq��)�D�W;%�쯻��f sWDd�����B��]�~xgj_l +Y^M�mO�K�V@���aFC0XŖWr-�
��_��1upO!�&=��.G���Ȫ�pqs���f�֔	���5|UyӺP�)mYh�]	f7v5������0X�vf��.�.��!C��!��kS{�ر��8'!N�肴y�E�sDb��^J�Ip��O��䜪����(��L_�h�Bg\E�`$k�DE{[�OYT�j]��1��"MsPҙ�Jj[)��,n~5?1H�v��nI�nߕA��ԇ����K��xg@u�P��f��^�*��ꑓ<}-7�hIy�Ӂޮ�a��Z�lF?�u�Ї��Ľ
���IO�bz�W4��m��״�ZY8��Ա�������\t�Ɯq%�����ۥ|p��R��d��X�̙r��/�4*��P]�!\G��1��H̱�D*!~�j�A����Ox���IP��oH��o����̑K̟
��*j6c�0NP:���Y3�K�n�P͈9FUJ�p���h���A*��E�j�Z�WV����XIm�FE����u��w��	܂�ރ���� ���(�I�ԔG��8?���{~���6 �oյ��ZG`t(�Sߕ��&A�����}��_輬�ZD��$������	��uY�����L����;���]���U�6����EBj�� l���0���j�i�?���!��Z������������g�`0�w���}�Ҍ?ھ$Jv`��X7R�#���F�%|Ӻ��t�àx��IlNq��8(FY�/������S�=�ٺ ��*6xYnɆ$�%k��O�4*�|��)h(�9'K�E�#,��O�!+�
�m��F�S����>�{魗:���.v6 �b�cza��V)���T��XI1�0��\��q��v��գ� R2R��_B59�J:I����Ji�%�����v�� ]2�������-&�/�_�Zm%���j��'�;�̧Ζ?�Rב�@�va�=jQ4c��f��h��E_[4�=r` s�g�ڃ��$G	�����o�����c��72�6Gt���O�]Ŝ�{�����2�c+!cK'�{pF����fb��E�FB=Th*D����VɔBf����^ʖ�n;u_��PY��ܷҁ��f�;U��me[cdƿv��� ��5��q�s��ղ��I�Q�X�D�J�d@Na�=f;3�őw��S����,C���M>�ɟ7�q���������kjs�N$oXA�/�^�6�1U��|D{
�
�ah�M��~�?�낓���}�1&8�J��I�I��0S�^mbt�x��Y�?WVv:��hA��9[�����m��	�����{0�;�8R���}l�4
���s����S&�Pb�o�Q�{����W�=����'�4�{	���峫J����3��
����9����!L����,�z�f(I�4x��F@�2��l�^�Ӛ�I�#����= �ૠ����:��@VS죎��q����������s�|�hzFVY�fI0��?�:J��4�����o��_g�e7g֗���u���Y��k3g�;�{�KB?Ĕ>XS��l�q> ��]�m����O���׌�ڸ֌�@7`��W�[�sI�W����Ihf�'��Yq|+zlZpz��,4�1���r(������>S�x��J�cG.3��.�k����;��6����E�m��Y���hX�gc���~y��H����z�?fqN�8\����1H8���VZ�+
�˲cz;�˱S���/U5�� �<�x�u�Rg�f�}�R�d��7]���F�D�?f������I�a]��+��s��p�x�d|����#tU.ܮ�l[�47@�8s�7���v&�e�ְ� Ր6�t6�N���+^�3�L�#�x�v�� �e�ju13p��"��y&����u]H4K���F4*KM6����^2/��P�^���|>��w+G�����N�	�%U�C��F��M��gO��\F�U�%����kk��!4(��i�7H���������赕�j����<�x�!�E{����cm��W�Ϳ�3��c��5Cݡ�ܕς ��Ҷ3X�=�ɔڶ�lw��g�M��
S�(ehX�w�4c��c�#�0⣍ǿ��`�<T��]�w@a�n�������1(��?AK��?r����mO
pKːלZƑP��#B%��}��u��Yq��J�d��¹�B`UM���6g�T�*���rD���rn�-;AΨ	mNu�� E)�"=1�+�w�+Kh-��%6�E4����ֆ��F-����ꃿ5�@xs����������Y��XJ� ��Rӧ2��kٓ���d��:x�O(k��q�̤��ly�]�����tL���~�my�a�CU��w�XlT�`��8lh������Ga<mn�,c���Џ�^�×�2�@����w[��J4�)_�ѦE�A�3&=_>�b�-J&�P�oQ�'r>�!Љƃ.8��҆Y�	�b��'��e=g�,�mo�F�nn1u��-�-��?K]�&���$�Z�&�+���g�����Er��Į��?��*���P�x&����DtLe��_&h��/5uP���X#�=8�������sT�±?�bU��c�3ʳ��2��ҽۚR(�Xʝ��v����J�G��6���|�1`�ֆ	�Ӈ���Wl����&��2�0= ������ؗ�X�C�<���u��4�:��l��Ń���Sy��9��2�Y&�F����hֲHr���Y����:x�6�MDY�/�sʑ���-�m���|��\ݶ�IAhBX�˾�*͋s$t�{�#�I�qq���' '}EN�'���y������Z�~f�/GE�7$4��N1�a���b�4��#�zȟ$��'�p`�j�b�C��;��y���hq�l�ܫ��Po �R��O�0�'�i�k6Ъ�(�����2����%;��T��.��UP��翟d��jUa�T^�Ʊ�����
��[�*�+�5����3zL��ʜL�W�MZ;J�~��vJUr�uuV���у�������S�.�	@����E'����-�M���I~#�d{D�v�}p ���I؟2�I0�'�OR'd&36dUM��~���O�3�ʨ"=�T�#�NI�_��+ὦ��W��MNס�M,���D*�RO9�JG�=��8��G ]<������O�r��cL�!�~a|����D�p��js�`�%ι�4�*�0���[߲m���z�aϊ�B��ªZ0����mآi���%]���:7��p����a���CU%f�g~R[	c�"�w�eǬ��m^JKz$C�'V�����T��V�$Z��-���1q^^0"g��^�n�dG�4Z�f²D̹7�^[f�w�F>Z�$df��ć�����p.�yz��W�#k��pS
�~v/h��H�<�_�D�K����Bw�k�ըܫ�nM��Q�xi�������a��ߔOu���W�+T����3�E� �^0��Lm:N d include/config/NO_HZ_FULL) \
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
  arch/x86/includeJ �a�|Q��G1D��)��U�A66���vy�7����rU�(f����6��Nm�)��$4�X���C�Y�+�օhh�谵� �@��^���7��-��Og׊P�VI�e	�;�%[N=r\(��U`�����}�0��Nmf�2�%��:��d_l�&�ݧ�v*�^��0&?�i��nA-�R`�n����};S"�Ɨ�:�S��*�
����u�����k"X�?��m�t�奌{PḦiŎ�G�l��Ϩ�R�4Q����Jq�`5�O�~d�����>dف?s&6�~K�0����'xI�nf'������9}����C��!�徲��Eve]�q��LGem�a8f P�S&;�M�1�$�-�l l t�H�wA�4�9�#�����T����b�D��Z��Ga�0e5��1"���5_]�ߧ��,^i�\����F���N�b�`f�����4ډQ=2�Y���%Aa��>��)v]Vd^�)��8�)){�������, b�"w�Gf��:B�9�Ʌe�3�N�񸇴�J��NC���H�y|M�-���i�<��!�\��ɞ��ê*UpcĂ
TǕ��g(�`�2I9�He��@N�]���\Hjz�૵3��:	����Ѐ^�J�+Myh�Ǟ:�{�n��o��g�1Llvr;������-��]���2��������/.洏I���-�R�!���1���������;�������#��h�{��V���?l7Ip�B�{8�P +X�L���J8�T�܈�63�ǐ��3��+�����µ����:=:�c�;-�t���j]$�I�� $��!O�OB��v�s�-��a���@t^��	-�Z%�T���tt��P^��x�?B�/{��c�8�f��jmޘe��#�&���sO��$Ӗ��v/9<ᵒg���m�Њ��4,9�s4��?V	��v�ij�#���f;*LPUv�n��e}���D��D�.8�!v�=�Z_8�2��<	Z�*cy�T���8�Ѭ�tE�^�k ���+��_h'��nk�U��nm��ĩ�Wͥ�zҍ���c�����l��b�hJ=V#���3�p���^�$\��hpU�IW$.=��0����^�R�K&�b��¯^ǐZ0�\@�ұ��Ǽ��*5��-@��ל�7���~���G1��*���jB���_X����`L{TÌ^�L�}E�m�L*�B�����^��sP�.m��x�Rtf.������_D�	&r�>����:r�\׭��2[&�RB�q�;t4��KZєp�^�ulf^LCӐ~�U� ?k $>X�\�G����K�e��z��LB=.B�`�zh�^���k�?�7Ym�PBeOȐ����z>U�Zܲ$��o~cTG/�ܽ�<v8���Db���C	���d�g���*�����ꥻ�W>jcS��ĄXhmE���ٮ���d� �w>B[�%�!m�Б��Ik�����F��L�_������K��y�H�W�;�B˗��]O�����RV1)�":T�����R�K�[g�H�*��m�����W�P�Wp��o��[n20"&��Y�������FH���:b�ۼ��<�1Q���(|ŏl.oN��5������E������DA��3���$v_B�L�B�%D�Q�v`�J@�Fƥ��ଣ��T��g=&��yџC�_�t:t�F���xX���Q�=�)!�L�JS ��^8r͘Cp����߾RtyhL`f�h����p+"���w-K�@D9l�+
ΣԜţH�e�������3(I}@e[O�z�M�Ts�j���K�����a5n=�<=i��DX�Ry%��������il�t�qR0��2�S��$�s~#��'��?��s�5�}Cp^R�y��O�c-3#����хyXzmq�d�TX�U�Xb$VSq����gk_LO��Զ��X�i!oG���uB�X���ڨ5�f�u���M�����T[f��mPu2��'��f@L|E���d��X�i�wU�w�}9*Y'�U]Ѧ�̉�v>��n�T��^�KJÿ���H I��b�Z�<(�NR�A]̸S,�r)���P�T�'��ཱི�?����.��QQV�dK�k�b�ݐ�a���5ˆD�S����Y�C�c.�ۀB�&u*��& �%Z3�S(�\�ܱ`,�Z����ͣ�Tg#�b���=
��#��st���fv$�gj�TNm9��7�n�48'wr��|�=y�_��t��2��Yr�8(� o[�IC/ p��V���_��/�jLqw��G��
�6Xn��ðo��hҊ��©n(/H6���f	�F�_nV� �/x�~�
�s�a,I�q�ۚ���o��)�ECo6U��=��K��*F~q��h�k��U�����S�Uz{8����c>Xfv�b�g��V�*�@�u��M�V���w^�Q&�'��̩Uhd	"���e�3v��# ��op_� wG#��i����� H����h�)<B�F� ܶY{�{o}������o�(���$����1Z�nX����=V�8��ڼ]��$�B�|�T&���k}��O\��0�&�|�0���c)�P�oJ���0P�|���X�C�S�i�H��W����$���I�����+Ăsb��n�����?�Z���*�U+��y����ӎ�����DK��=�Ub��=,��O)�ޛ
l��@���~���K��9N���nH�n��"�����̃O �x/�*�/��i�O������Z���d��n��Q�G�Z���=�a���s�R�������p��$5n���Y��u�6�֩�"��܄ѓ�]�]���������q"bvZ�̰G[�)W9���a���i:�z%�ȕZ��L`�e��;W>�� lj���� y5�O��X�H��<�2�� gs��:xՄH����]�8}��p�/��{���U���t�&+KY��A�M�{���&��a�Jmh��>�����0oK�l)�<�����\R�@�Z��9�m@=�O}�q��q�(�_X�2�*[wĴ��I���w��X �tt��\�ATD��n�2{������RH!�M�!�2�3��Ӄ9����g�-Jl����w��k�׼-Ӡx ��le���#��`��
q�[����ǰ�ges����J�ͭ�n9��E!��\i��$(��N�l�G�NI���W߁^#���f} Qz뀄��Yrt�m�F9�tނ���W�I���i�Kj?!��)Ho��k!�0Ü��A�E�Z|ϧ�g�?sfQ��m�sOm�	/���)f�uqqE��q:X��'�T�Wo���[����|�}���
�A�M�����3k�ܐ����'
{�H9}c+�*#ù_�3���5୬u~�1�蕖�{�0NҶ��a��?��=U�xH��`��:����H����w˭�#.��C(��o�1�W�����iӦC�n�ji����*�#���´��H��rY�$t��/l����D�1Pp�@��sf
�%�l[*���XF�UGU�sP+��e/#�m23@v�t������������:aH������t�>=� �÷�]o����]�o�(�/"99�i�5�$��C̑�(缱��p�Px�����FM-��WP�j�K����)�U�e�Oz�����v�M�Y�u�d�h5�G��U����J���()�����pPy$+F�IWS�ߦ��]s^��>>���� F�VC�rm,I�ab���b&�/"�˯U|g��"��ƣB��c%�
Q����]��[Q��	���@�R���$�'�D���Iμ HΫ{����7б�<Aɽ: Ϲ�6����f6{谭��4�c��'��{R�m�Dh�ՕI|vΪ�]	?���S��m�q�h��E|�����.U��Ɣ�/r&_,
������wþD-��6k<e�H��#rx^n�����ݣeɕX���V�%JƉ���%�t�6[�aŅ�P3׊���6t���$�^��>k�E<��i6�7�l��O9�GG�� ^忦~�<�X���j�番�l�?��Q��\?B��I8vJ�gTMmSZz@�c��}r�r�)6���I#_U�I��*�6���
'���\�Q��+<��h�=y$��?~�G}af��m 2�U[tU1W#c�f7*Ϊ���������lA�\6?x�%m��B�U\���1��'���J�Q�z�G%y�����|��7���9��� �=�Dզ�����o��2��(�\�ҷg.��j��;��_~�7<O�f]�Ѱl�jG��pY�V~B�d�*��<���$���w`��}�w˖��iV��I�Y��~Fm�$8��!�K��VG�2�N^�=�	��9��4Ν7��K�ƙ��Y`ي�� �<�D���2K�I��
z�#��oۋ��ô{f�y��R���F��qR�I�i'���d�*1�h�}�C�`z�(Ta�wP+@<��>�K�0�ēM��r1�c�;l����,�UY�D<�@R�Wt�BhsDj�92�~��!s����&t�䠜��.�0�vkV�ibG�t���o݇+�Z/ �,�P�]� ���r�[{dY?�)*����3S��n���&yb(���m��e���?�i_���$��2��;軩�d�@�ϟ��i3��b[1T?���T����gP�_$FG�pݺa�`"��蕦��Q�!��̀v�(i1�
;���0��k��P*�Ny�Ȑ4�D�.�۽
S�xԮ�[&��sH�M"�h��m ްf�%sY�N���6�9�����hnA���q&.��_|)�y<b�+�T4�&n���?n �R��@�U�A3��B%�xk/7�'/�H�& �J?a�X��L�G�����O����T�=7�����o������N�؃s�r�)?��^82��d6vt@ޕ�D!�zyn���-G�G��&�"U	�,at�0�⡓w���)�a�F�Y���Xu���fqZvi��Ir��ӝܤ�"�*������
���l�}���.��n�ԣA��%��@����^�0��!��f�����Lw��s�;T��>\�DpV�+ݬO�}v�j�������r"�x��c\�^���|����)X�1������I��V���F��݄!�|�Ȝ��u��I�I�,�V�o�F۲�i~�X�{��P��/J��,�:�0�s@ykk��K|���C����Dv��U� ���TR�w)
� �k�/H��9�!�1�._KN���b2�`�_)8`�)YI�vh8��$�4��A,�Q����[ ;׃	Nyᤝ��e�|0Xy�nª )� �qy�X�g�q����vw���N�h1������~jh��g�⥳n�ز�0�l�%������(�u��6^j�篊�t2��C��ٚJm.�5���.�H���F��k�~�1`�XN�m�t35�z^��SM$�D~�|��?��Gq�bi{1B<��lo&ܧ#����@c8`_e�ߒvoc�̝�|� K{�/�3:��1'U&�<�-�te���]9��$+�f���}����g\WZx��Z��-rr�حb����c�ee��.Pu�a��,�6l���J�V?<�MJ�><"8#�_l�O��A����z��w(�Z����!"KAi/�u{i"$�e�2��07��C�·|t~��TLoU����T۔��JԾ�ނ����Y-��^�]f�U/�5x(�V��'������<K�Kc/2�qW��ɉ^�ۘ:='�(De[x�Bٞ�?�{�2�����Z��*<� P��V���5�5l�(g��U|��ϙB�k��a�e��V+}�O1.Ĝ����%��P���a_�W��m��W*g���f�&7��y9�q��З�������K�d?�����Ҝ��9��)�g<ڃʼ}~��Ǜ�7Q9=��S�{�Y��:7��#Z��fj*V�¶ck+�_G�#&��j��l�Y$�|~��X��u�:��ڀ�O�KislA �gͭ�(	�t��A�W����XǡWk��&&�w��տ{+���ĽI�Cƻ'��C��BN!-41�_UL��&7t/���1oH�a���Y�hb*CZ�|����Z��
x��/v��E�r*gbn���c)�ii�)5z�o3�2j��ڷ�Nb��m�s)K>1M	���L��w�����Gܑ�9��}7�UOj]� ��FpH�w[:�o��ւ+��j6\�?�j^�q���Y���e��uy�m.0�۴i�kl0�Ʊ�{�%��4��_f�O�|��S�-J 5��l͇誖�S����R��LY������[[��-(ϸ�rD�n"GSþSv��?c[�^�u_L/~���+� ��t�B�4�1K�"��p���0�'���|�{����5s��,�Y�N0���x�}�tl��J��<��\W��y�
5��`e� �8ٮs(���|�Ǝ��&�l���W��T��u�>�� b��ٜ�-�T�ߧ�ۅ�e9��
�G���>SwSj���/�nʕ�@��ph�M���v ��d�����B��5�^�F#���i,='�dƯyA�;��E����p1���h	u<�-� 3_wx�L.��ƒ�V�+}�ͭ7�a�`�,�=pH@}urU;ڡ�J��Qh}����@�/�3>�v�`�sۑ����#�����{~�e~�O�(���R���j�WNRq���ֿ,�(�����p��.Q��i�R�ʶ6�%��Rt����=Z,�a}i?�3x��RvR-Qe��eL��lf��3��]3
����~���z_�k~�J��g0�-=�$��b�Vz��`��ApRz��|&fOC+�v��Ђ��p���L$u_۠���oZ�N�uN�)��u�7�~D�y_��q
0���U��.���R}2[�Im�<��Pv�ȟ�j^<P��~��T�i���d�h ����moZUw7�ӓ��6S�Ŭ�4EWe��.|q��� ^��M�,�P��S徍$>*ScZ%�:��Q����^d���*{ �����f&�@�r�:�� 4L
�N�d#��G=A� �V�(�m
ϒs�ヶҍ�K$�Dǀѭmc$P���N��	}�}2�$54�H��6�Q:@z��D��#zf:a��)�S�S\ �P�ĥ���W)HO�LS����.C%��[�ƺtO\9S�aq�]���f��<��J��,����̐��m=�1J�E/*��^e<zeZ��%C�7�yd����9�0N\�܌~�FȺy�?�t:P�c�����^�%�7$�oySVT��q8-v0����!<��������u_#��y��'����,T�S��Y�j����YiӸ�GJ,	J,F[g+��MV�@�� ���Gy�o\����b��h�yM��P��H���᧘��ǩ�ߤk��ɬU 輻R����wZ�kr��Q��w~�4�D�h�	,�&������AM�G!�$`!0޽��Bͫ�<���D��9�/��]3���������=u�rIx#�?��J�0�a3�����x����	��:b�&"�`T�<����i�&�S��:��3�=����Z�S�ڑ�qwvF?���*�&ݥ�̣���I�̾�j�mO�*�:��.ZD�������ͽf�
��-�щ�B�0XF�p�e%sR:O�6��SV��wЎ�'��ŝ�j�����/#�t�H��Rc}�H�#X�\dC���mԶ��{���������vw+�]��%�ɑ����F��4F嗻���ܧ��^8B�ig�]p�L����Hd���Ci�l�!�Sٍ�-
��b�ΞPo�Z����]��P�
'�Pt�
���6)�-\(f���F:�aΐ�(�F�b���[� �v��Y��"���7�ZH��j�v2Ӯ��aNTO��<�Ya �r���A���w�A0\�1��4��z�&PCMsOvUL��y�_#�޽[�J*$۴��b��n��A=��~� &+uϚ5J
Oq�Qo��`�=Ӊy�J/�Xh������݊W/�ߺ�_b���UH��� ��W�� Hڑ"��q�Ak2M0�Z�"ߡ��P�9��:p:�[H'
T��X$W�L���s�!.������[ޭv:����������O,�F�b@Ҕ#"]��87q
I��@I1�鸿6�p�u��
s�R��
[u�9�B'�K`��!�±�R�e��J��lw�����R��t/{���6�R,p��鉯宾1:�6F��%C|�����[L_��|�#�IOy\H�J�^dT� �|_�Ze/_�W���k���f)3!��+7T����o�C�}m�R|H�9��P��WM}To^cɝz\�$�U�2ME!�*tk~�:	���,�	�W�M��:t��S�@�1���
����g`�M���_l�@�(SA	/m#e�4��hkQs&��ݍt�? ڗ)5� 	�s��7�랐�9�Ȳ!�P�y���S�_��-�1�%�j��uȷ�ߺl�z3��2�D�g�/dk.��ԸA
(ߦ�K��k�O��T��5�>Og��8���C�j7���[;9��3�.�#c�c�,��'/���7I+���;Pb4������_pC��Ԉ��!�S�ˎf�
>�����R�C5�*�ő��b�G��cQ��[�i.�K%�\���M s�R���Z7V~�������٩�1��Z{�!�+�  �j$ݎu;��d,��c^�)liiN#f
� G���:K׵s������&|8�h&涋�󮏁���xdw�L߬ߪm�+_�r���aKaf��%7@�Ɋ4T�Q��Э�|�'���}�	p�H����!�s��I$\��+��� PL�"�mw��k�5�s�珴��2�!�Χ�Y�$���C�{�=���!���$���'��H��8�ocѱ����̨5�S,S�=���M �#� ���f�u*JM���
��>��#ž�9�Ju�=,�@��n̥���⋫<���ُ*=�l%�#-��w}� h�yx߽\i��J3p�uC�ޮ�~����e/e�-��9G_��Q�N�'nyr����Uxr�o�x�@�B���'rE̍�ZS S���|��jh��m�[+��x�k���P�ZJ[J6lP�]����*�&��P���?]D����Ŵ|֞�q��3�`˗����A��]g�Ru?�4\6Ҁ�t����ݪ�#��%�h�����H�<���Q���I4����u!�B�i'E�4�*�����
"HF�{
�h����+mRGN���9�T��g@i`�cZ��>$��[���)ٷ"|8`[l�|@���d�]��n��FsN���6�����
��XZ�2���VCSh(��f��)�!���á��JAA�RI��f�qh4ƷH���mB�e��#�C=�x�KGȦiTg�.VJ$7���b�70�fp:��҇�� ��g� Pl��[R(-m7��l��}&W�*���� w0l�7�Y��ϔw�t�T��?V����E�Æɖ�y�� Յ�r��֑�Y����h�wnQ��d�{���ɔC2+���-r7~����_\G��:m �P���� ��ҧ��ȸ3�j)D�m%�*鷎@U[�M�����,�Ҵ%���[���n?T��L�_��?k����3?�Bo�<���㇇X����m��ab�Srbkh?��H*X?iQL�Ի�{�n?���O� �Ks�5\�$sֿ�H��Q�$U�s�1�q����~�YAAu̅��%�nqK�d�쟓�c�nű]=��-���tp�f��FOe���"Kh��(~��^3�d>k�p�J���fJ ���{����:�>��� *%SOA�2&�Aʱ����t�9�ȑ$3���T̃���a��A���t�Q�6^�x����ъ�-a��t�]ˇ���J�k5��ʴ(M��7<d(��~s_�jbv�N\Ӣ��� �D�lz��r�"i�/}�w�Q(�n�p�_A�_�����R�k�[�U)Q�ڂ�_�:\��DD#�kS�t�� ~�Q%/fdR?n�s>hTx�4I�8|p�	���(�9q�vl����[��pQN�,��U��䇕CJ1g=��@T � K�ٝA���Rx�̓��o!�ؐ�XC�Ʀ�eijԈDDG�'�3[�A��_;8��~Ҙ1b�����BML꒶=����#���P�� QBɟ�|И0Bz!&�K�m�tȫ���#vI�I0�̂5�!:^g�� u�J'GsO�z���0�KOg����Pm�c�7������9-'�Lf���;
� ��`x�7�4��o��6��F�IqV�q�[	�hitٻ?�n/B���g�f����]��AǚQ�e^1ˉa��C����b�M[7��5��-�WP�,���6n�"����lsXN n��|d>��2�\%��#j�:��rImD�M-[~ZH�O�Of�@� '��>��OJ}++�(�U9�}��.�kf�x�
߽�QF��U�*��?q�P|�f?SQ�@1�GC|h�'n.�H�?�Dy	J,��_Ły)h��y���?|b^��;ӱ��W-��31�Jc�=�'���aY���҉K�!�~<D�q.ߪ�0�"i"�hH}�@�>X+���3)
m�$�"N;z��ꃥ���#�Z��.�;�y� ޯ�3�*
�a���?ɺs'�z-j����&��;U�ܜ�!Qď���~�F?5�9W��.A�E�SG����㵢�p�{��.����y�ǚ'	�פ4����t���CЮy�f�ýmM���^	�-���"*�Me<�+��j����-/�|�ٜ�a/�����(���4Y�� c��m
�V���+P#+Mi�f�z*n��Q�I�D���K�x��3�9&�l�$��4����h���7�ߔA�r�҃�N���.F�=9m�ET�r�q�
��]&��n	h��S�̐9�hVM�dy�P����m�����A/���b�N�BH}�㑺�~�.`�������Dŀ�/-���"�4��Q#w[s$�{��X.�v���%�~ܠ�ـke?7�ۮ
I�*��zr�H�ki��	6zjA22�]�5瀴D��b�~���N�9�ζ Elp�մ���=�XD������Y�^^w>=��x�P��,rIK������4��G!�~�
\d��7/QY'�ʫn�-�/)\{*�&���#g�n4�;���:���z����韺A�bu����N��#c�����dk���RE�i�i�6V1Pi�~r�b�l�I�9���X:�Μ/�*���(��U��䵼-�%��yU~Lt��M+���OA���3�qz�#����ѡj��q��A�;W�>�A�Ǭ��۟L�{����Fb�G="�ȶG��KWȎ�q���b<-�
�]�dp��qBT��N��/��y�����l����5g"���ߴ���'E�����@E)3�Jf2�W�����dzd�{ڊ��,��*�A.� ��&��=�(�C+�\����K�l|�����綖I۴PW�-.g.��	�f�J5Z�����������Z�6^H�ؚY���D��/t�V�V����<�C��#?2$b�b��'�P���}N-��M�@�x�e[4ETJ5��]\�5M��0� �^��,@�<�/�V��D�w4�l�|�n����Qy�����$�b���\�(_�p�Y��;s~��%�m�s|]ˆGϮRHF��Hd0Z�s��F!��Qh��	��B8��s�<��ܨ�#���g+6�^���;��I��� J�οq��2ƨ�H$2 �`V̐�5R��*Pmӓ��^�w��`�1 �Q�>�.1��r��
|"4ش&��>�u�B�a�M�>A'm�	m��U�lY����S�N�����na�����o�O��.��h��:��<��mB�u`��+��~|�tU��G�PL��v�|���6��5�-����!�0 $�Igj[j�>k���1J�&'`\�nj��7�y�����[֤��}�iǐ�Os3�nO�������!�pş��k�t\m2����C@,���LV�m|�m����W��<�!+���?���j��n�I�/��H���o�����ǧ���<��jF�����gO-2���u�
b����L
&sb?�N����Ʋ��u����~i��WZ�&"K:�2�FН�0��{���s�0f�Тg9\G�@��ᓰ���`�ű�b�\��%�M�Ǥ0��G��%��`=\gY�����u!�ƹ�<?�٦��"�y�dw"����o�>�艡�(��*���9�(<T�ji�nm��ޫq�gu��B��ѹ[O2��Css\���S������a���1�P!�5i���?a�n�P���&6ۦ����d1�#6�� =�	���������6�9�ؘq��h�Y��?��hzՇZ���@p��� �FR�q����jwn%�C���g�U�5&'�'��&�px�m� *.aN�D�P�4��P�@0�"����Ұ鹜��
�	�r��:/ӓ?w�����`pNz��~�.�\O��G�qn$Ad�:m��B�Jt�<�2#2ƨ�sޤ����	��?���=�\{� �Al����h����D<��{��9�hi�ID)��b�F�5�:�N8>z��A;h����2��})WA �QՑ��|���a��L�l�X�S����;h��ҝ~ߟT!��rw$����ç�lݔ�K�%�ѿ)����Do��Zu[����f>(��6	܉�{��X�:�"��O�P��\�� ���xp�k�{��4~i�V�Y�s��*�E�.��Ĝr�.�'���B"� �*�!�j�	�C8-9vvpL��%�.�UD�A$ۻ6{�;��%����,�^�M��,N��h�0��a*IT�	L��Lt'��?�}R�u�~��d_��]NTv�EL�V�w����eu�MYO5���C�W��_��o^��$�I�n˰L��ug�T�;p�,��l�,��dE��Iu���{�{g��g�i3A�n%������
pfV�?t�@(%�;ö�{�8���R�&����w|�q�uMk\]
�^����%�o��&���+JՋ�	G���_����� I��p5��ޑĵ�: ��	����X��2����)+P?V�W-v/I7-k?�&�@�"qIg���$�� r ����C�[n�6�FLj�����<qx�?	���bQq�t1+�� �T�0�\Rs���ZZ��C�X���D(0����+�S[����Ɇ�J���b�S�=�����m�t��7�� ��`Q�^y	���K�!gVl��������2� �7`?�'vٖ��/��n���Z��&��q}�����ݼ�k���ڊ<�g�Re��yw�$q��� �}4r�'��v�"�^�f�؁���Q�S�jrF�ր��1=t���'��.���� D�E_ߐï��w��&�@��|���^��I�+�`�I�4B�"��K������Y+���J�*t���\rChJ�$E:�s��E��w��v����w���?/�qeFx���ڸW�0�X��A�W�|�����kH�'/��T��w<Θ��-���"J��` v���ǭ'Y�ǇUx�q�[��XMl�{a�@\�M1���"Tn�J�o�щ�Z9z��"���9]d�l�+>��4�F��>-����e��!��A��ӛ�����vd/eJ:�Ѭʯ��BȒ����O������{�G{eT���Օ�H��v"^�A��%�(ex� u%#_���l����8A���l��3�ID���<�	���gܜ	��1\p�,k��?7_3�)�Z2`�҂Ӭ��L����� ��f7���<�o�؍�V!����q�0ق�C���y?m�q��{&o���$�Q�5�7��ȉ�%r�,d�aX��g^�0g��To}������lߚ�P[u ������0��� }Ծ�վDZ�V���Up��_D_T���K�M��ߚp*�@r��u��"!V$�F�pB���~��c{���T\8Le>�;����	���b(���VtQ-�+���@>�.���4�.��	�즩"�
�@|׌��3H�F�a@��|hǑ����Ge7!MG������t����(��~�Yf)��n����
< ˰x�Q$a�
�|����'>��?�
3;#����_A�7����Q2����� <Ÿ�P��O>���[�%���oԆ�����0�dae_*
G�� l��u$������'�-p�{��]Ɖc8Yό|�.��f/t�bB"�J���ߒ����=%�x����UGbM�����~����f��dp0�#�Y�P����c��1�j1
K��E������l�*4�R��
�ꤿ�4j�8�R�&R��?�!i�L[y�3D�~�~d8E��o0�&~J#w	,bW�0w�4K�P�h����Pq|�#/[��"��Zx[�T8�w#��|G	���5���-��%��,�q1�5�6ͥC����My�B���}�m|BS��V��@T�r��XCQ���b�`�BiU��%�b]�6[c�!?����W�˼����̞�;��KN�����i��c]��XN�}*BeX�?��ƠNGw��_��0�p�1�0͟n��	6��{�`d�;.�ђVM�T������<���9�+V.,jQ.�̀���&S2�=�]����40p���cU��!�Rr��ur΃ ��j������������矁;�(��M���)������\����ڡۧ陿leI���Y���͑���G7ڙn�0D/��J��e��{>�zR-���>V��G�/�︰
ED����P-,\�r�^���Q?���<t�46��T�r�ٱS���ͲƝCU9�1%/Zb△��r�ʆ�+j@������0%Z&N�ttEς����k��S2�,����ⷑ�X��K�@��w���ct7;�=ic�r��`��H<�
no��.V��&�ڇ�P�g��A�m�A��Fs�U�zժ�$Z��<e���)8��D�آ��~��y����Ja7)��=�l��9�(~�7�a��Ċ1� cϑ: ���$꩜��QG�f�t*���6��ɯ�R�jH�`D�Wd�ac����QĆ����E|�����7Pk�,?�V��1f;���Nűc�����)gj}W㣑j��8'�	H?�U�ֿ`\z�ea�n&E~ -	�����2�.ϯ,eH�ц�
�6,sVX�Ʃ2@�i/��;c�r�*�K��l�����AX��8�=��7r��V���eO��Y�)�}�2Aګ/6
{n����B [ym��igi75)
��
,��(���`�]����� �-�Z�/^�9�.߮���ڀ�Y=��~[6�pu��g�!��N�&p�VboN�t&�����T�Kܬ9z������=�.���𫀍b�NA�a� ��j������M�@H#�U�c3� !;�j�U�D��(�U�1�L�*T`�8�y�T����=�$����Y7Omء�W���_em�B�|�G��k����zn�c�">���-�$��Udv�nɯ�3���8*=���`;�F�/�]�
ϭ��%�����n�˱��D�a�\`�#Iv�e��Eʴ��*Z�,.��r�OX�:f�?UQ �Q�i�)�Q���_����8�~Mo��Ӥ��EJ�ڸ��춦E�N���#��̰�|��D��|�E����_�	������SDm����O�Tis�tx�#��<�v>hH�O���p����N;��gl����n.L��R`�݄:;�Zw��Q곪��2���A�h��6M!Z�A�q���hʥc�Edy�ǙO>��Vy�)v�X�Ͻ<�n`�z�6���l����|~C�7>��W����e�2��Rd�wH�*�XU��B��Jw���Qlz�l����$ǥ�0ɺ���+!žyG�L�E��`k���#();
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

	�    ��Í�&    ��@  ��Í�&    ���   ��Í�&    ��   �I�����    �    �9�����    �    �)�����    �����WVS��xT�Ɓ�   �׉Ӄ� �NщӉN��[^_Ð�   �   �� ��������F��t��F��[^_Í�&    ��������[^_û����뷍�&    ��&    �����UW��V��S�ø    ��������C(�$����  ��t	9{��  �C,��"��   ��    �D$����   �<$���   ��4  �n�D$���L$C9h��   �D�9�u�!  f�� 9��  ;Xu���C�$�D$1�������ƋD$8  �D$��u��   ��    �$��������ƅ���   ��T  �������u݉�1Ʌ�t5�D$�D�u*�mh��u��������    ������؃�[^_]Í�&    �D$�| �D$�D�9�u����&    �v 9�t ��� 9�t��x �Pu��t�z t�9�u��|$�C�I�����  �B�$��������ƅ��E����D$1�������f���������\���������R���������H����v �����U��WVS�����  d�    �T$1��D$    ����  �����������{d�    ��p  �t$�t$��p  �[��t:f��|$>�t& �K$�S0��������ƅ�u�|$ ���w�D$�����_����uȋ|$d�    ��p  �����������D$�T$d+    un��[^_]Ít& �������S0�s$�D$��������[��������    ��$��C��S ������L$1�)�CS�C�$   �M�����G����������&    �v �����U��WVS��d�    �D$���  �D$    ����  �����������{d�    ��p  �t$�t$��p  �[��t=�t& ��|$>�t& �K$�S0��������ƅ�u�|$ ���w�D$�����_����uȋ|$d�    ��p  �����������D$�T$d+    un��[^_]Ít& �������S0�s$�D$��������[��������    ��$��C��S ������L$1�)�CS�C�$   �M�����G����������&    �v �����UWV1�S�ÉЉʁ�   d�    ��$�   1ɋ�$�   �D$    �D$     ��$�   �L$$�K�\$����D$f����   ���.  ���  �D$    ����  �����������{d�    ��p  �T$�T$��p  �s��tB�v �|$>�t& �N$�V0�D$���������u�|$ ���w�D$��������	] �7��u��|$d�    ��p  �����������t$��$�   d+    ��   �Ĭ   ��[^_]�f�������V0�^$�D$�D$������v�������4�    ��$��F��V ������L$1�)�FV�F�$   �=�����7����t& ��t$(�    ���t$������������&    ��    �������Qtfw��tO�    ��Pu%���f�=�   t)=�   u��  ��Í�&    �v �    ��t��������   �č�&    f��    봍�&    f��    뤍�&    f������U1�WV���   S�É�8  �߃��t$�4�4  �T$�L$�4$�D$���t�t$�D�tD�$1ҍ4�D$�D�9�u�(��� 9�t�H��u��H��t�I��u� 9�u�t& �D �ŋh��u���  ������ƅ���   �D$1�8  �D$��~$�D$�D�u�[h��u�D$�01���[^_]�f��D$�<$�D��,;9�u'�Ӎt& �<R���L��R�x���9�x�y� 9�t��H��u؋H��t�I��t��ȸ����럍�&    f������UW�׉�V��1�S�������t=�Ít& ��T  �������u��P  ���������Ņ���   �؉�������Å�uɉ�1����   ������Å�u�Vf���P  ��u,�؉�������Å�t;��T  �������u؋�P  �D��T�������؉�ǃP      ������Å�u�1�[��^_]Ív ��1�������Å�t琋�P  ������؉�ǃP      ������Å�u�[��^_]Ít& ������UWV�Ɖ�S���T$(�$��"��   ����    ����   ���   ��8  �|$���|$�|>�T$�T���t��ud��uo�T$������D�9�tS�\��� �����   �C�C    ���C    �D$���P����Ņ�tz�D$�C�$�C����[^_]Ív ���������[^_]Ð�$�������	�t�\$�\�9�u��t& �9�t;Cu�;S�r����9�u�������[��^_]Í�&    �C��A����   �C"  ������D$�D�9�t&�D$��t������D$������������[^_]Ð�D$�D�    �̉��@�����&    �t& �����VS�ø    ������C(��tV�s,1҉��J���Z��uU�s(�C(    �    ������F,t[^Í�&    �������F�u,d�[^�������&    �v [�    ^������t& 맍t& �F��(u΋V�F�R�����뾍�&    ��&    �������P(��t�?�����&    Í�&    ������P(��t����1�Í�&    �t& ��������k��   w��9t[�    ��`u!��Í�    ��lt3��zuV�`   ��Í�    ��1u[�z�    �    E��Ǎ�&    ��    븍�&    f��z�    �    E�뜍t& �=�   tQ=�   u2��  낍v ��Qt[w��tD�    ��P�e����    ���W���������v �    �E�����    �   �5�����    �    �%�����    �    ������    �����U�    W��VS���D$�@�D$�    �������������-�  �D$��&    ��39�u'�   �v �B(    �؉��������������9�tc�ƋC�F�0�C�   �C"  ��t������S��t��J(�A,u��L$�$������L$�$�A���   d��$������$�f��t$���������������D$���   �J����D$�|$�@��X�q�9D$t���������؉�������F�p�C9�u�    ������D$�������t�������T  �utd�������[h��u�|$�G������������t.��[^_]ËA��(�4����A�I�$�I������$����������D$�������u4d���[^_]�������X  ��(u���X  ��T  �R������j����D$��������(u��L$�������������R�����륍�&    f������UWVS��H�t$\�$�L$�t$d�    �D$D�D$d�D$4    �D$8    �D$<    �D$@    ���D$���_  �D$d�����O  �ׅ�t	��A  �D$d�t$��������8��&ELF                             4     (               ������@\�����1�������V�   S�Ã�p��d�    �D$�C$j�T$������C$�t$�   j�T$
���������V�D$PSh    ��������D$d+    u��1�[^��������&    ������UW��V��S�Ã����   d�    �D$1��L$�    �T$���   j �   ��T$�����Z��������D$d+    u��[^_]������������    VS�����   ���>   ���1҃����a������1ҁ��   �P���1�[^Í�&    �t& �����UWVS�Ë@�P�R�����%   =   �S   �����[^_]Í�&    ��    �����U����W���V�4S��Ã� ���   ��������   ���   ����	ֈ��   �ψ��   ��?1҃�_	���	����   ���������   ��?�ȉ�����	�   ���ȉ��q����    ���   ����   ���1҃����K������1ҁ��   �:���1�[^_]�       P                                                                                                                                 �      P  �      6%s: Status: SA00=0x%02x SA01=0x%02x
  7%s: write reg: %02X val: %02X
    3%s: I/O error write 0x%02x/0x%02x
    7%s: changed input or channel
 6%s %d-%04x: chip found @ 0x%x (%s)
 ����p������WVShL   ���������   ����P��P�CpPh(   ���������   �@pPht   �����XZ�  �S�C��  Q� QP��   �CT�0h�   ������C��  ��   ������Ń���u
������z  �`   �ڍ��   1�������    �   ����   �ډ���   ��u�1��z  �CpPht   �����XZ�0                    �H���  ^ � �              upd64031a                                                          �       �                   �                                                                                                                                                                                                            debug ������    �    ������    �����upd64031a parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=T. Adachi, Takeru KOMORIYA, Hans Verkuil description=uPD64031A driver  P          ��      GCC: (GNU) 11.2.0           GNU  �        �                                  ��                                                                          x                  	 3   �   p     �           C       B    	 X      E     n   B       	               �   P  3     �   W   �    	 �   `         �   �  �     �   �       	               �            �       �           
                   &      0     3     0     F  �   ,     Z  �   P     n           �          �           �  P       �  0        �  <   0     �  l                                  -             ?             Y             a             r             �             �             �             �             �           �             �      
     �      0                   upd64031a.c upd64031a_remove upd64031a_log_status upd64031a_write upd64031a_write.cold upd64031a_s_frequency upd64031a_s_frequency.cold upd64031a_probe upd64031a_probe.cold upd64031a_ops upd64031a_s_routing upd64031a_s_routing.cold upd64031a_driver_init upd64031a_driver upd64031a_driver_exit upd64031a_id upd64031a_core_ops upd64031a_tuner_ops upd64031a_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev __stack_chk_guard i2c_transfer_buffer_flags _printk __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__upd64031a_id_device_table param_ops_int    $  	   %     $  &   &  8   '  P   '  b     g   (  u   &  �   )  �   $  �   &  �     �   '  �   &  �   )    $      Q  $  e  *  �  $  !    �     �         u    0                   h                                                 (  1     6   (  G     L   (  v     {   (  �   +  �     �   ,  �     �     �     �   (       >     S     �     �     �     `     d     l     �     �             $          -     .          0           -     3        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rel.text.unlikely .rel.rodata .rel.init.text .rel.exit.text .rodata.str1.1 .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   ]                    	   @       �  �               )             �  �                   %   	   @       �                  /                                  8                                  4   	   @       �  8               E      2       <  �                 X             �  �                  T   	   @       �  �      	         k                V                  g   	   @       �  0               w             V                    s   	   @       �                  �             j  
                  �   	   @       �                 �      2       t  
                 �             ~  �                  �                                 �   	   @       �                  �      0                          �              /                     �             0  (                                X  @     $         	              �
  ,                                 �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            �|$`"�  �D$`��    �D$���  ���   ��    �\$�q�L$$��t$�4$���vh݅�tz8  �l$ �D$�D$�l����uZ�D$��D$�D�9�u�>��&    ��� 9�t�X��u��X��t�[��u� 9�u�t& ���'  �(  �vh��u��l$ �4$�\$�މD$��4  9�t�D$9���  �  �D$��4  9��C  ��1���t& ���� 9�t�p��u��H��t�I��u� 9�u�t& ��?�1  �D$d�  �L$�T$�؍�&    �v ��t9xu9���   ��t	;P��   � 9�u܋T$�D$��u��   �v �9���  ;Cu�� �����  ������  �$�\$��1����   �P@�@<�D$(�D$`�T$,�D$0�C(���x  ��t1ɍT$(������D�4���w  ��tj�C(�   ���x  ��u���t& �������D$Dd+    �>  ��H��[^_]��D$d��  �t& ��D$��4  9��=���1��B�����&    �\$��tu�D$�k�{�L$�|$�C�D$4���C�D$8�C�$����  ������ƅ���   ����   ��������<$�L$`1��D$<��������L$`�D$@��������8����   �   ��  ������Å��C  �t$�L$�+��8  ��8  �C��D$�{�|$�C�D$4���C�D$8�C�$����  �U����ƅ���   �D$$����������U�����&    ����������D�4= �����   �D�<�h�����&    �D$�H�	����t& ��t[�k�D$<�C    ������D$@������P����v �������l$ ������������.����D$�����������1������D$<������D$@������C��B����   �C"  ���������������������D$<�����������D$@�������������������������������&    ������S���@,t��T  1�[������t& ������C�ud� �������T  1�[�������    �C�� �ލ�&    ��&    ������U�   ��  WV��S��dd�    �D$`1��l$��󫍆T  �$h�  �����Z�ǅ���   �^h��t"�������T  ���   d� ������[h��uލ�4  ���  �� �@��9�u�H  ��H  ��L  �����tx�L� �������������t�l$`���������9�u�^h��t�������T  �u{d�������[h��u�$�����������D$`d+    u{��d��[^_]Í�&    f�1ۍ�    �L� ����  ���   ���L��������u�1��f���X  �� ������X  ��(�x�����X  ��T  �R������_���������v �����U��W��VS�Ã�� �������= �����   �S�Ѓ���tE�    �[�$������$1ɉ�R��Sj �����ø    ��������F,th����[^_]Ít& �C��1��������= ���ws�S�    �[�$������$���R��Sj �>����ø    ���������t���������F,u�������F�uRd�������؃�[^_]Ã��É�[^_]Ít& �F,t
���]����v ������F�u-d���������?����t& ��F��(u��V�F�R�����똋F��(u͋V�F�R�����뽍�&    ��&    �����W��V��S� �������= ���wt�F��1�������v�Ǹ    �� ���wf�������1ɉ�V�����Ƹ    �����Z��t��������C,t��[^_Ð������C�uDd��������[^_Í�    ��[��^_Í�&    ������1�1҉�V�����Ƹ    �����X룋C��(u��S�C�R�����릍�&    �����UWV��S���Nd�    �D$1�����   �F���������= �����   �   �0   �� ������Å���   U�\  �   ������F�{(�T$�C,���������X��uS�s,�    �����1�1҉�jVS������Ƹ    �����������uJ������ƋD$d+    uv����[^_]Ít& ��������G,u�������G�u/d�������t& �������؍�&    f���멍t& �����랐�G��(uˋW�G�R�����뻾������������&    �t& �����UWV��S�Ã� d�    �D$��������= �����  �    ������C�$������"�<  ��    ���-  ���   ����  �k�L$���4  �L$����  �C�T$�L$���D  ��������D$�   �F�T$���������  �F�   �T$���������  �C�����   ����   �L$�$����   9��C  �D$�$�����C�~  �D$1�\��D$9�tc�|$���@�t& �@(�   �T$�@ �D$����������  ��;l$��   ���9���   �C��u��C��t��@뱐�$    �    ������G,t!�D$d+    ��   �$�� [^_]Ít& �������G���   d�������ō�    �L$�T��L$9�th�$���
����9�t�J��u��J��t�I��u���$������&    ��$�o�����&    ��|$�N�����&    �$    �����t& 1��D�����&    f��|$�$���������G��(�N����W�G�R������;����L$�D$��������$�����������t& �����UWVS���ۃ� d�    �D$1��D$4�L$�T$��	؉D$d�    ��������  �T$8�D$    �@8����  ������������p  �D$�D$��p  �^��tB�~�t& ��|$>�t& �K$�S0�D$������ƅ�u�|$ ���w�D$�����_����uƋL$d�    ��p  �����������\$������D$d+    uk�� ��[^_]Ít& ������S0�s$�ōD$������[��������    ��$��C��S �����1�)�CS�C�$   �K�����E����������&    ��&    �����U��WV�ι	   S�Ӄ�P�D$d�|$(�D$�D$h�D$d�    �D$L1��T$$��  �l$ �D$    �D$    �D$l�D$,   �t$<�L$`�D$@�   ������D$(���4  ���D$1�P�D$0P�؋{�����[_���  ��t�D$���t�D$�����  ������T$pd�    ���  �D$    �@8����  ������������p  �~�D$�D$��p  �^��t@��    �|$>�t& �K$�S0�D$ ������ƅ�u�|$ ���w�D$�����_����uƋL$d�    ��p  �����������\$������D$(�������u�D$8����   �D$0������D$Ld+    �7  ��P��[^_]Ít& ��D$,    ������v ������S0�s$�ōD$ ������[��������    ��$��C��S �����1�)�CS�C�$   ����������v �|$�������D$0�L$��D$4��P�����&    ��L$`��  �   ������   �D$0�D$� 9�G��D$4��tR�L$�1��s$���.�����
��!����L�f�L�������z����
�L��L�)��)����������D$4    ������������&    �t& �����UWV��S��p���  �D$��$�   �T$��$�   1҉D$��$�   �L$�D$d�    �D$l1��TL���� r�1�1��T,���� r�D$�|$�L$L�D$(�D$�� �D$4�D$� �D$8�   9�LЍD$(�����D$��xN�?�l$,9ǉ|$<O�����  ��1ɉ���������ډ�������ǅ�t=�l$,�D$�����D$L9�t��������D$ld+    ��  �D$��p[^_]Ít& ��D$1��������  �D$$    ������{�����d�    ��p  �L$�L$ ��p  �[��t;��|$ >�t& �K$�S0�D$(������ƅ�u�|$$ ���w�D$$�����_����uƋL$d�    ��p  �����������\$$�D$���������   �D$<�l$,�����   �����;D$������T$4�L$��T$8�L$�����   �L$��L$L9���   �D$�D$    �(������t& �������S0�s$�ōD$(������[��������    ��$��C��S �����1�)�CS�C�$   ������������v �\$�l$,�J����v �D$   �:����v � ����D$    �#����� ������Å�t%�L$<�T$,������D$�D$    �����������D$�����l$,�������    �����UWV��S��h���  1��D$�D$|��$�   �D$��$�   �D$d�    �D$d1��tD���� r�1�1��t$���� r�D$�T$,�L$0�L$D�D$ d�    �D$8��$�   �D$4�D$ �����D$���X  ��uL�D$������T$4���c  ���[  �D$�֋l$$9�O�1ɉ��������T$�����������,  �D$1��������  �|$������{�����d�    ��p  �L$�L$��p  �[��t;��|$>�t& �K$�S0�D$ ������ƅ�u�|$ ���w�D$�����_����uƋL$d�    ��p  �����������t$�D$���������   �\$4�D$$;\$u��xq�t$t6�   �ډ$������$�D$���������uF�D$4�L$�������u5�D$$�L$D9�t������D$dd+    ��   �D$��h[^_]Í�&    �D$$�D$��������&    �v ������S0�s$�ōD$ ������[��������    ��$��C��S �����1�)�CS�C�$   �����������v �t$�J��������f������UWV��S�Ã�<�|$T�D$P�|$d�    �T$8���  �\$��D$؉t$ �D$�D$X�L$$�D$��  �\$(�D$0    d�    �D$4    �\$,������������p  �D$�D$��p  �^��tC�~��    �|$>�t& �K$�S0�D$������ƅ�u�|$ ���w�D$�����_����uƋL$d�    ��p  �����������D$��x�L$�T$(;��   ��ux�T$8d+    uy��<[^_]Ív ������S0�s$�ōD$������[��������    ��$��C��S �����1�)�CS�C�$   �;�����5����v �넸�����z��������     cgroup_bpf_enabled_key  __cgroup_bpf_run_filter_skb  __cgroup_bpf_run_filter_sk  __cgroup_bpf_run_filter_sock_addr  __cgroup_bpf_run_filter_sock_ops                                                                        @   �  0  P  `  p  �     �  @  �  �  `	  �  �     �           �  p  �  �  �  0  p  �    �  �       0  P$  �$  @&  �'  �(  �)  �,   .  �0   4  �6  cgroup_id:	%llu
attach_type:	%d
    �  �      �  @        �      �         -  �-      /  �/      T2   3      45  06      D7  �7      include/linux/skbuff.h / kernel/bpf/cgroup.c include/linux/thread_info.h �      �	  �     @  �     �	�#     � 	�3  -   � 	�$   ����� �  �  �    �$  &  &  s'  �'  s(  �)  S,   ���	
��������� strnlen           P      0                                          	                                                     �          @                           	                                                            	                                                             	                                                             	         
                                   �                                             
                                                                                                               �  `  �  �     p                       GCC: (GNU) 11.2.0           GNU  �       �                                  ��              
              +             N              o             �   4          �              �   5            P          /            
 T  Q          �  s          �             �  t            �          2             ]  @   �    w  0      �  P       �  `  
     �  p  V     �  �  /     �                           V     )  `  E    @  @  y                 cmd_drivers/media/i2c/upd64031a.o := gcc -Wp,-MMD,drivers/media/i2c/.upd64031a.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"upd64031a"' -DKBUILD_MODNAME='"upd64031a"' -D__KBUILD_MODNAME=kmod_upd64031a -c -o drivers/media/i2c/upd64031a.o drivers/media/i2c/upd64031a.c 

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