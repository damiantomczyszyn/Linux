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
		 * Record the pointer to the trac��k����h;�����%�������y1)4�ES���A��:_�d_~J�oTA.��8?��[Kʍ�Ʊ�Af87Sz
���� ���=7��5��	2�X�PL�}r��Wװbz�jRGØ}���%~V�<P(Z���Yc@���`��3��&��o�'A�V�ą�CY�Q z�P��R}U�	^O/lM]j��0��#GSOV�O�D��{,=C�f>6O@�,~�"�8�z�d�o�xK
��y
��(����}���o2��K�����6$�;�T$�)I�Ȇy+j��#����/	
Z������*�>5A��En9{�R��Eۡ�|�l�_�Z�o<'r��I��Ǎ�7�J`����f��JF�%}.�=7�z�0qx҂�_;,����C@�e�{x�ifӒ�< |�i���)C �H��a t'�P�(��-0�Yg��`�I�����Q�i�ݥ/̛?�<Lǌ��|G�/)﹙@�����ڶ[���%�Uѽ	л�v?ξo3�bj7+���bg�īK�n���ѡ��i�k{�����"i%���+�0��WU����XN;�Y�v�ߩ��Ӛ	�y������A_�T߻g]SLj��D��~���+if6p��KOD����-䚖�+��r`	�a���>T�����e�\����;���˵���[Z��+�����#ZM`�r���������*��'�'�[I[�R����~؝G�_�s{=h����%b:�jJ�p�lԙ�1Ry�mq���~�����S`�J�C>rN=qopTeos�dk{l��s\|��1�
�^�X{Z�.�z�<�/��O<=��h`�)
��:�W����u��*l��߄��7^P�@S��[�B�aiA�\�o���� �N��;qT�l٬�~C�c�m�NTPa�U2tEl��r�h��:�
�_B,���1�['��+���$D|�#�$WˠR� Y��+y%���'�ەt5��`ZTH��K�ܦ�Lj�
���wM�)L��s������
"�tn��>�������o���~�c]�t^f��-/�dP�y4���}�
��ܭf�.
�xB��*�X��w4���~}S�QV���d
��K�����i�c�=YB��W��A�Jz"�׿ʄK��!�������F�<)��U��>�?i�'@I^����'��i���@d�ǣ����B��\b��Ma�Y��Ef�m��>�}l^�X����%��Cgz�z�v�C�зR�B��ք���ޝCe�~wM]	:��X���+<A�|��(gFs6n
�x#Rm�A_�bݿ��8���+��0�LK)��{W]�"=N��g�/_�T16`,!]$�7J���$G�|l�ϗPP������S<��d��ݼ��G�<W-������kaL)C<"�x14�1���5�1H���^S�߸�*�J�n��.�&5_�e\�����@�kS�"���<h3
���mX��Q��"s�,�n�z�d?D5*O��;���P
�<�G/�5���|�v��w��j`W����yj��A��'%�R��lRˣ����Jn|c�L߈$c��_n���"+Ǹ7��#��d��2��ja�*���C��rx�5�{}��/($��**��b�A�������.�B��Y���_А�o�>�B���ϰQ�^���*�d��h6�h퀥�����t6���m3����i���t��y*�1V:�w@eR{�7���O8C�Pp�i�J���� �u���,���iᇟXjʤ{�K,S�<�T	�P|:��Q뒈���&j�aL��NT�l��*XV�d�+a�?����u;�������d�Ǥ\z3Y%EƔâH��7H4b:4�WY�=f��Af��y�y��y�Ў{"c�D��N+��z��f�-�yi
+������I���+���&�R���@Q���il�����P�y���s�L�Ztܨϻ�}�P'�����W+��.��֔��u�0�Q��n��<Z=�K���/��0����a8O�1f�չ&Ej�	��c�
?�v��K��5���]����
gU����k�7�ˬ�H��8���m%ޑW�y/\
��B�W[+��+"~�
�a-���r�˂؋P��+u~���jJ""{�dMQ��[���FƣF�=�_����-B�����D�[��U�� .����>J�w)�*���l�yT�L��O�r1����d7�б�	_��\�NM	"��3:)��ߌ���b�CN@�������A�o�E�٥wj�I��9���>o!Q�{�Yn������]ꭆ���KHɪ��-Z?���\�`��)��?5��w��:Ӈ����=R���qz4&ڇ[47k����5n�>�
:e~�h��RBy�(�b[e�|]G��G1y]�_��_�1������l�h{�
�LbP[̌�.C}�BA+�@L2ޣ����k�(�����{p���` �Su[�X��ﲬ*�6�d�+'݄�Зw����z-�=5�N�@�0�ol�z��x��5Ԑ�;�%�|�_��0|X��M��cn!
�q9K7���N�����7F��NQ�F2����f�y�є�j��o|7s������2E�p��:	:���Qk��Sx=~��|ب�1x���El�/�S�us� �K��Y%�а�'���Y��.P��ّ�vy�%b%j�m@c�?����j��C���6���#[db��m�u�.���g�#X�K�T�ٍ2�*�&Y���0��"�C�V���A�Z�X�s��rH�z�y�ms�lb�TϘ�e:)J���בg��`��jJ�n	�X���
�bH��}�5N�z3�>m�7���!�1FC�N� 	M힠���k�-����!P��#��>I$v���aTq�ˎ� ��%�R\�-�n�#9��e���@���
&�M
9��u�*9�Tx� ��7��-
'�|���\�OI~? /I.H(x�I���;��B��寠�JG��Oyp��eި@I`E��9Þ���):U6��<P�Ys�+w�:b���_�\HZ�L�5�`���x��JR��:R����{A!����W6G�MAU��$.�)X�}T�h:�Ì�r�4M�D��<������^�>wI?g�lJ��P���!��.W{W�"r̫?�$6��,_ �:���󷁄5. w���
:��˽ �� ��ɩ�en +�Јe�W��R*7-Q+�K��Ø�+�O�t��ds�� "���I <���z�3/�v	c�D��)ֹ��{F����ràGb
�~�(�]�n��~�'O!+6X5��/X��X��q�F)o�\ќ��|>� ����T�^d�̀i��$��O��eYX��Y��d�%u ����W���<+�Gf�<���D�����3@?�r���ײ��ok�'����)��ď������)�����M��C��ٚ]׃/F
��Cxٳ%*��+����MSK��rb�{`jC�X���a�l�4���)��Pq��E�� I!�0���7ۓt���%�µ���sa�,�Y�����a}�I`��L�I�&�|co��?~>HQ�<�W�?���ӻ���L39k��Z�;^������!�R��3`���C���������.q�S��)z$|K�^�e�yd��>!��y��d�6�~��<�,�ST2�{VP������*$��aގ	��W�t��b�t���V�~#�^ԗ��4�Y���S�,^����
����r}�
4��s�"���g7D�3cĒ����Ȼ/-�O��8�"�=+�ٖ�q��W���6PH���3b��|���<�Q�ec��="ǧC["�svt�w\-��,���	'�PKlv�@��?SyJ�e�:n�̌'�2�16;���+��z��7�W#H��.�UpX_�%�{n��܋v(De ����v�\Y�~>sP����^�zt?��H��r�ǖ?|l,G@}�^u�W�u@ �i}�ȉ��^-,�Ԍ�����ܹ��&�`x���ڻ�<x/H�d��T�6Xa�_�q̳��q���2I��І�IBݖ}mF�z��Pd�uI/5D��3�n���4qa9���kj؟iGsj�yT�b9���,�Ӽ��i�"oy����O��9�(e�Ӄ9���H;��-[*
�5[=��]�.\���b�n�Å,H,�XZ���@�i�)��1a�Y&��yT����u�Z���KԻ�
��v��8'n��c����v���sX�l��L=Y�(1�-���
�s>�e ���{ʱMv�f�<½��x��������6 &I�m���ݯ��릠w�!��[����DNHg2�9e�'�dx�N�pJ!��}�/�	�5��ylNl��@,����x���-q���~>��Fc����ϻ���Sb�
�@t�g��1�����9r
���?gY�$��QϟI�G�`�Jmz��f���oX�ˆ��A戂ހ���c����H��Ap���-ae_O8�&ĦP��� ̝�*��c��r2Ef�$�T�������ˎ]G%�X~�� �O��@��^i�DjRڥ����qx�7�S[Z�Ao�9j�)�Aʠ��({��6Ȳݴ^��5&���ie2�&���H�ٯ��<��XVz�%
PH�*�ꏷ+��dk ������q�~���X2A
�E2:B>�daߑlx��-ޤ"��2җ�S���!q��k�<@��,m�;���Ox��6�����W�:�{���(�S�6��t�u2U	�hjb��2{]H�v�r�����u
6F���䆢[��wL�֟�o-�A\�}�O�z`��7�:/q1��n���y�"��P�ߋݿ
��)Q^Z�ƬP���t�+�?�Sy��qH]7��2x�P�R�����2�c~��c�C��DeF������� [�������J_�̵Хt�KhKU>�� �$�W��^[�tc��lb�_ӻ^�q�1ot��p\�~eb�"���U{�e:s
�I�&~/j��T%�G�_+�ٛ���#���=y`��M�_�x��+���q�2��X��e.|EG32�B�NG"g�b"z#p
nnӰ�6-����'e��A�lK�uU]����MR�d�b��(w�a�X�X���y\�L�=�#�#���0� ��s;BH��:�)S�9Q�}���?`��M�	|�+�c47ٳ+�W������kH����W���Ͳ��_�ؑ;͗�ߏ�HB����O���ǿ��)ɼ�a���\B6\�L
nЄ�V�dR�=��G�_���,p���NN��v}t��� �ykK'�(o�����[�$A �b��S���ܧ�{oz"��VT+���Dh|�@/�8�#^���i&�nv\8�Sl��(q�M�ܵ_��a�K�/H��X���q�|����D�Z��ڋ݌hI��,�b����5'�����|�hM�Ss����Lg-L�r]G�%�?�)e#��fPՉ�ܪH�$�7+$h���ѤA�3{����@��a��R�0hk�5�&�3�蟸"��z_�����%c�༚�{�ĝ�a�����������Y�:救ȫ�né�W�Z��]��>��@���ݭ,< �>����b�)�w��	y���u��U�k��f��e����	����M��8����n�����r�ƻ~����ϑH
Cqc���: ��>���W8­IB���Є$M����a�R�l�n���J�ơm��J�	����u�oCdI͡�,=(��2�k�R�"��W'�A��Q���jJ���K����V�#�=$	 !{��w�f��4D��>���,��z2r'�剆�9�u7��*!��
��l&Izjb�q�{�[`_��Y�D��|ln�W(z�+��~'�Qh����q+���w�:�M5�p��K�02���C���/w ��a�8o�@��J2�}?|mǡg�f���Fk�-�Íϊ`T�� �+�V��M�WC��8��N�0ϯ��  ɚ��ÝD�.vKFh�&�2��ڲDg6l>v*C_(���Ty��y�
5T�_V����x��	����[���X�6Gf��{��xy�0�u�,��h�����~�}��ƾ�M��R��T��ˌ�Z4@D*���iRL���6���:����͗�x6
���/�ɕH��{����`���
O5A
<�z|e�ap\�ѨQ�i
t�0�X�X&d�@����7��+v�vxϱ�(~��52Y����wj( O���$�&��8	:��>��h^;2Kγ&N�T�q:��!玚���t]�>�2/5��~$.���J���;�����\^6�Ý�w��R�P���7r }�������lC�Y&���r\[��Q�:R�G�qj2$p¾l�9�߰�1s�D�s�<.��c_��*GZ�p_�5eRĻ���a�t�An_���y��zW[�p����t��;M}��w�o��[Lll[�9{6|x�{1S(*F���BnQp��|�����`��f�5�`'nAw܌B�����2���B�/���{]/u����i1�!���Z�p�WI�tS�.�0�Ѿ���l>�jƽ�i�[~ 5q�7���=
)�!�r�!��P��%�Z�!yU$6�d�(�,��	&���ӱ06�6�V�Y�=��1ԁ�ݘ�|M|cI��M��a_f�$s_|s��/ZP^����+�
���CS���%~4���u�NT)�P�?�A#���4!����G�x
wB'���F5b�(&���zt���k�R#���td(��9؉/=H ��x�G9��Mǐ��gb������� �Q�C��p��}��C�qV)Bw�!H���X�V?�(FO�%fq��)�D�W;%�쯻��f sWDd�����B��]�~xgj_l +Y^M�mO�K�V@���aFC0XŖWr-�
��_��1upO!�&=��.G���Ȫ�pqs���f�֔	���5|UyӺP�)mYh�]	f7v5������0X�vf��.�.��!C��!�
���IO�bz�W4��m��״�ZY8��Ա�������\t�Ɯq%�����ۥ|p��R��d��X�̙r��/�4*�
��*j6c�0NP:���Y3�K�n�P͈9FUJ�p���h���A*��E�j�Z�WV����XIm�FE����u��w��	܂�ރ���� ���(�I�ԔG��8?���{~���6 �oյ��ZG`t(�Sߕ��&A�����}��_輬�ZD��$������	��uY�����L����;���]���U�6����EBj�� l���0���j�i�?���!��Z������������g�`0�w���}�Ҍ?ھ$Jv`��X7R�#���F�%|Ӻ��t�àx��IlNq��8(FY�/������S�=�ٺ ��*6xYnɆ$�%k��O�4*�|��)h(�9'K�E�#,��O�!+�
�m��F�S����>�{魗:���.v6 �b�cza��V)���T��XI1�0��\��q��v��գ� R2R��_B59�J:I����Ji�%�����v�� ]2�������-&�/�_�Zm%���j��'�;�̧Ζ?�Rב�@�va�=jQ4c��f��h��E_[4�=r` s�g�ڃ��$G	�����o�����c��72�6Gt���O�]Ŝ�{�����2�c+!cK'�{pF����fb��E�FB=Th*D����VɔBf����^ʖ�n;u_��PY��ܷҁ��f�;U��me[cdƿv��� ��5��q�s��ղ��I�Q�X�D�J�d@Na�=f;3�őw��S����,C���M>�ɟ7�q���������kjs�N$oXA�/�^�6�1U��|D{
�
�ah�M��~�?�낓���}�1&8�J��I�I��0S�^mbt�x��Y�?WVv:��hA��9[����
���s����S&�Pb�o�Q�{����W�=����'�4�{	���峫J����3��
����9����!L����,�z�f(I�4x��F@�2��l�^�Ӛ�I�#����= �ૠ����:�
�˲cz;�˱S���/U5�� �<�x�u�Rg�f�}�R�d��7]���F�D�?f������I�a]��+��s��p�x�d|����#tU.ܮ�l[�47@�8s�7���v&�e�ְ� Ր6�t6�N���+^�3�L�#�x
S�(ehX�w�4c��c�#�0⣍ǿ��`�<T��]�w@a�n�������1(��?AK��?r����mO
pKːלZƑP��#B%��}��u��Yq��J�d��¹�B`UM���6g�T�*���rD���rn�-;AΨ	mNu�� E)�"=1��+�w�+Kh-��%6�E4����ֆ��F-����ꃿ5�@xs����������Y��XJ� ��Rӧ2��kٓ���d��:x�O(k��q�̤��ly�]�����tL���~�my�a�CU��w�XlT�
��[�*�+�5����3zL��ʜL�W�MZ;J�~��vJUr�uuV���у�������S�.�	@����E'����-�M���I~#�d{D�v�}p ���I؟2�I0�'�OR'd&36dUM��~�����O�3�ʨ"=�T�#�NI�_��
ὦ��W��MNס�M,���D*�RO9�JG�=��8��G ]<������O�r��cL�!�~a|����D�p��js�`�%ι�4�*�0���[߲m���z�aϊ�B��ªZ0����mآi���%]���:7��p����a���CU%f�g~R[	c�"�w�eǬ��m^JKz$C�'V�����T��V�$Z��-���1q^^0"g��^�n�dG�4Z�f²D̹7�^[f�w�F>Z�$df��ć�����p.�yz��W�#k��pS
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
    $(wildcard i