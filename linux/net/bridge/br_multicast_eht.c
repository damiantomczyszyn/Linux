ount_matching_names(class);
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
 * that <prev>��q��>�aʴ�a��B�\�m���c�ً�U��\r�8����`�|�߉��	Y��3@zB=;`9�,��`_�������7�����k[����C�tZ�*���Ej7(`!SP?�_e ������6藱/�P-Q����e؂4�[Xx���u;�,+��#��E�Q9TY}Kv����4����������^�p��Q:u?��ީ�~�(6��v��!=k\�����^�����ws/�3;�zu��L�K81:���jhƥ��dg��<Y�������\@0��؎�HxW��"�o-p[��b�.<�� K$!�C��Wp԰��u�{Y��a���O �����Y�X�+�.��*���������ccl��Z�ġ]uQw< ��ׁǎ� Fk����f����iʸ�@2gрe��|I�x�(0,bz
�u)Fw�`�|�;�Nl�%��4��3ӣ����{�qjLT�z�m38����6s:����M�qE��.e�/W�n�t�/�<y���ɋ�I��PС��2[B������{�vјR\Xɒn: �M�6�����i3��;�6�'cZ���|Sn��D���
t�Ќ�V�<�UU��ne0茶6���ֺ�y�66@�Js`�SŒ�����j�UMfҏ5�I�1bxOn�!_�j� �h8߸M�/�^����fy%an��>i_��<��Ԃ�"�rO1( �0f��^�(/�j;Q6�t�3�P��&����>��0{�5e&� ��a��x���u�*�|��|ҥoK��
��7'��9F�9%�k�:%�a,�8����y��yw��J�V��N��P?��?�t�t)9���Wgd7��ߨ ��*ӧ���I��G�Q�nL~+�]�S_�'ҏ��+�\�Y8�e�M;�����;�%�C��Pm������zٌxyg,�����L��י��w�~��n�R@ެ�(F��@TN��P�U��bS�ZOH��}d
>�+�wVmؽC*��u�ʪ�f/'�������0���j=!`Q��r�Y�y�|?��ٕ����ʏ:7�t"��k��%+ү2J�~��|sMN����4QS��9��є�n �y��y�7���
�dk,W��p��x'�l7��,���ؕ�w(�y �|(�_�S)�Z�B���VwL�7G��gi��9:pU�KpXr(��RX�m� �'�L#N1�L�8�Zi@UoK\�kJ����^�|�N�(7w���c; M`�t�,2@�M�7Yu���q��'(�C.�-Ya�  ���]��Ka��;�9$N=���
�����3P���W�ό7:Z��d��ݲpHR�V^>"�f��Z��
�ѷ�0q�8�؆ti�I;�fq�����)�>'����PT��^�=j77ϙ2ᅿd�T+�}Y��W��bߊ7P�r�#�d9�FoM���jL����}�(�Jp������A���"���gqYO����KS3�y�"(W�U����Z8]T�L �$Tp��|;����b&�=s�
�'xѨO�S�N�mX���b�8$�`�'�K�&���{����q()Ȁ\3���o8�=�C5�Q!�aH��ߞ�E�T�s@tv�2���{ 7��zTm�8k��5ƒ��+�V��Q1�z�0a�lyӁ���K�2�p���)�t�Py�ŻN��VZ�� Xb�m�����m@�� 0ݥ??�v 3��p�!�������̾�@�"�y����=�Cj(����x�$���Tg!��Z�s>c�s�?f��?���h�у���p�Zc{��y��s��ۈ���wk䞜5J�x���_a�LR��ǩ�ۼ��'�d���3��C�$t�Ut}��6���T��ĬQ�d,��݆��F&��׍P����5m���w8���S{� ���`j�v���	P�<�ĀR\Ѫ��x�ד]�_����g%Da��s��52	����l#������sR�E�B��TY�5�0-S��p ��\�3z�t�D�HY��,@�87<G����A�
؊���-j�_�uR���as�z��C�
��r:.^�MW&�$�+i�u�HW�=u��{�c? ޢL�/�t��_���7 A�W,<�ۂ��~^S(>d��;�u��yK�Pm��K�[T:Ua��i�`�����<�y��s�W؆����C&�X����<�a~���1����sn�o	�4�S�`�wnш���|`��ib7���/�t��j�{��6�4�3��:�ݦL���B|V�}��Oog4@��mX�z�&��פ��N���c��ID�.�䠱�R���F�G'�x���bk��<�{[� pr�N�"�>�4WL�%���AE3O�
�c�Ε�<�O�|����T˾.�`2��lꯅâ#@���'V�ڵn����Y-�X��V-�3���Ej4����@nD���~�=ճFx13��� ��?��}M-
��� ��LU�Ã�B2bpOcA��/FH5��}�ΠwH/�H����/��o��[g]���`J3����R�kK��@+�ޮ��7���;R��r�ތ��Lk���� �}���ư~��������V̺�䗼Y�9���v٥���%\�ġ�z������I�~ �:�>*l����`E43���_�c^�
�r��.��S8�	�]w�Z4i���Ӕ�kFD�w�tg?�+���+��o7b���\�k;��O-Ed[�B�g����3����΂��O��Y�ƩL�}
j�Pu��|���T
^�LĽ!��7F3l A�tN |ݤ�:��{�(�RQdЄ@ۡ(��O���ܣC�l:�3�4�Tth�+kp����9�馾3�oQr
�E�:��=�	7�EO~������̇n[$>|�ĉ��8��G���U�Y�S�w<���Ĕ�ϊa������nl�P��<]~�Z�.�K��u�R�9��Yg<�ל�e������B�A������Dn�6�]�z&=-�l���7q'��Z��������W~��Zyb4%�w�D����/Y��'-�T?*�����~݈"�.�gr$�������&��}%JM��)�.*Ӟi��;�/:)bW�0��պ�hqNf�Ub⿎��Yd��I�0x�����O&���%l��yB��&' �v��_}-�6��`n�r�4����vz���]�ll�@�RfѠ�F/>��rRg��d;yo�
R�(}��,4oуa(���/����a�� $�x.�j��~��xU�J	������Ps�L�#�	 ĸZ׋�������.D~��*��z�ܲ����s���V�=�H�4.ѐ���(m��%����z&�{�b��;4�9,6O�w��ל�Bܪ}�oG�~��q��P�E
c�������O��R���y�04�<�	����Fվ����QB-ʭ�Т={ ����b����r+��DG�F4ʅ\d�����y�*�j�>D���f��`�g�>=�}!*�L!�wn�!�!% <>��&��΄x����\�(�`>׀?�	��`�j��)g�}���^���:���'Ϣld5�R�)u��)�N�k�%�P����
_�ү��� ����a�f�17�YKf�%��Dm�M��{������[��`��(m�_(���%�)Ȗ�vB:�y��LAsW��6TQ���R��bjُIʷ�"'��&��,�XR�%����v����id{j���Ҥ��P�hI���Y�׳t��Y|7Ey
=���*�r~��5l�&!�D87zu���tq|�5	�T���ł�b�3|o5�.����*�7?K����#6�BEޯ��1*���+q�7�j�����AJ�k};�f;*v~�l�%����f��EGށ"j��A� ��cHUO�ϓ�)�vmX����O��0i�����47�{�zf< ��W�2q���+1t$i�]Lf~��d���9��tw�>�v|	�U�����/)�s��6�
���m��W#	<�m����Ӎ`|vV� @��Ј]�#���ȣ�����<��m,[m৏�l]�M~��>@���S��i�7� ���D��1�U�2�%B�,� :���BX}I?��x8n�]�3>.�_㐸�	�i�S*f���#�ٳ���Ͻg�0$4��?������A�C�ay��Y�%%�@�h�o����������|w�r[>�k_�DT��ƍ�0��'R��T7��r�w�ɬ9��z����+Ҷ�6�ԕl2*`�4�9�z�i8����R���;LK�����P`1��q�������x�¼I�(���O��I�~~�o���(!��ˬ��ү����+��e�nK$ �+�ګr@�_1#�#�9�l]�����I-0O�yw�Ç�֬����߸�f��YFV:�ɯ�@��"mͻ.�~��(͙9��/��nlO��Y� �b��Y��? M���0�U��0o)��)�&���얧>b��YC�x�G��_�~$pŏ��%V8L����hbpH�Z)-��m�[td�T1�؎���A��]�l���[�Ԛ��O+�
V�AX��r8o�ݦ5�n+�5�(�iFB!�;{�/���$�y�5<�-P~����d�_㧫�iO{Vxz�^7���g�IW=)�`c�=�q�f+�.(ܶ�SU.WI��z��	�������HZ�Ub�!S����=�r&�t�
n���#]����)��Y{3!����ΤT���P���b�l~�5���^��;n �2� T�]�����P��@4d�iY���/Eh�D�o\5�0�H�U�/�zZ��oy���=�ċ~KA,X��/J,��`���N�
B�wnQ��غt��w[�Q�+E�vm�&pH�����&JhJ�v~X]~��5vڛ)q�_��R�����k���v�W#��d>�7���DBZҔP��$�+��� 6��#9���W7�j�?M�����u/����r��f���+�i����`H���A�&˂�x����m���Ad	�¦�ӳ2Ar�!*Uwi�V�Hk��[�ٹ3p+��b�ԋ���mX�[�R�^U�PC�W �tü�<�7{���
�6)��9�,P<u�Z8)N�ķ��y����=�E6�`��Q�b��@ J������LU�5��_2�WJ^�>��-<���p�?sh�"������D��r�ٍ��f�Ǳ��Zhp�NZH��]���,��m��zl��4�$��"tt����].?g��I�0�ƨ�?j0�@(,�.H��i�[�������0t�)~�#Q�-9Z�S��֦�WxK~��^Vn�-�S.h%���&��dth����z�)���1�`z���g"S��U�v�I"o�S��R7�!�qT/��X���CN�-���P��Wł��Ǔ���v��W<�*'�k\_�B6�/	l��Ju����@a�/�i}'h1�F�H�+l㳕�Ì^�" �{��CY��:(�K��,S��&��By��j��=�G��K��W�M���XD���Pg8�������K+1?��>���oU�R�fHf�o)mn�KB��ϱ5�-�^��E1�K 0d�_�R0�rq�M�F�ܔ��v��|�q�j��3APJ��R	�Ҏ���'K���(�S���yd���6����F�/ �5Z�f&A��G�&���*o���	HM�$Aɰ&dD��{�drcQ0�`�I��l��r\6��2��5��j�xQθQ��F`G��E�9^�!�WP���-���0��d֣׀��ꊜ:�^Z�8�:Շ�I[����F�fj0/���`���ܰ�&7'��}-��aO���.����T����\Z��!X�W�Ƃ��3��c���#DT� ��z�(y����ն-�LH�G��NbF�aM��Si=�v��.J�M���S֜a��4F�>�����=q���3w�㰂��i�	�Q�配R�i�C4�x�*�F㸞�(W�ZY
|���%�ڤ�"�=��m�){;�۪���3����)Y�kwoD%fv����z�y*�-�&3���-gk�%mG�4����ژ�4UJ��\PA+���n��OXsא�n!�z}�g��.���d,r4w	��(�;�ݗw������e>�R}���>�S����/-BN ��n�����[�HV!;�W�ӻF!�1h����VT�{�L��%�/뷲�r���8{M�܀�E�/ob�g��Muˬ<�$Ĉ_��������W���9�y̗)�񆷘rP�u�P>�!�&Y�@��|��O��x=Z�}R~��r��q�dKA��z7����,'�K	�+���67�R��8?s&q(�.�� g�'���#��e+�!p�zB��'p\���������J~��~1���CUaF�mP�Y���2T}a��8�J�}s��J�[�����ԁj�K� ȺtعH{�״�Ey0��  5(�n�~	/Iar��_�@z�Ȕ�	i �/��pc!_���拼�nQpK�o�h��N�7%�ٗ�:���+CtN���M8���9~�HP{W����_�tˈ1!R�\8�O��Ė~�v��f;e;��}{�c���'�p�`����Ե|\{xK�r��Ky���N?	3 �M��W�!�]C��Y k��4�?�р�d�L�+<N�u��+�;a�\g�_��)Mҿ໎���������A�a�%��!��7h#���*���R�9�b��އ��b�}�r�Y��XGh��І9Q��
�dM�1��[��!(���������vn�\V\n���YB�*M8�Y��?���C��1���G1��-�'d�R_��Ë����}��\�Z|���I�"��`��)� ��V��h08l1�11���+��U6�N�����Pٞ�K.L|�H`��'���sk/|>uz���+}*��CY��g���ƍ����5}p5v�)�L��2$է��'	��_xMκ,��׷.Τ�sn��L-J�m�E��ݾm�C�0��\�Қ�Z%�JA^{
�dl�c{(�vtͬli�ei����o1��c�� ��cɳ��ۄ�.d]�-�/��:X�,��n,j����R��3�D)����C.�R�x���w��4r2<�,ЙT����OùK\a�$M㾂�F��˿p�Ű~��x��m����\����Lgo�t��2��F1\Y��g�W��|��o�㕻�.׊e�Gtw%*��^�t.�����_蔌��9g��n.�,��˓s�n�K*�#	r-�O���@`����2��x ��}�Z�w�$������,=Q�QԂ�3��r�����JN*N���7 "�i	scr��ش7q�-��Ǉ�q)�R:<wa�f�f�u5/�����'\7����^''�peݢx ������'��$ugi���w0���=e�v�}�
����[��k�0��8N�d��D^و�Bj�|=π�r��^!��2��=jz�G�A*+D1+���@
��i9x��+`Y.1R(��k�o��1�=V3
&�.����>t�����+93�6S�=}�� <�jZ!�1�t��w�9�xݻ9��������h��F����f��$�:qg���D2�&W,<�F��F��H���N��vP�]�d������g�R0:u���MW���0\*(��
���.���֝ip-izn$�P��[M���5���0(O�{����Ekrՠ�+U�3ms'1����0T"t��#ѰW~�)�7mz�K	v�?p^:-N�`�Wpuͅ<\�y$$�1���M�iY���W/@��_"GP_ї�6�����ClcH�<�a�zV��J_r�A(���`�fAVxԚ]&1�ו��tn)�D���m��kQa�E����@XJ,6�fj��Ű�$��Ѹ:��{����$�~��k.�:��r��Y�� R.�U @G��	�m�Z���Jp�:Lv_�>�xx �]3��hߢ=�Bk�o�<��>n��ӢMd5�H�?���($�	�|� ��!��w׌8Eo.�����qm��P
���?da��f��BBa�6q��R=��r$]�����r�3K�*�X�ɧ�][�
�ˊ��܏�-�g$p�-�W9�����	c����^�r �1soު����L3�gk�08��u�8qqd�x*q"����k�(����\���B0]8�?�H�d������<Ŵ�A���If���~Vh�vn��Z1�E�l�X����`�T�����dM>}/3Q?���0��"餌����Z6y:͜��и�/Mu׷�<�s��K�m����܏�<�81���PllI{\��	mr�Q��"�E�:�|���T++l�ZF�C�G��"�������Z� �0�^����F�c`��_F�C6hMP�v���	M�|'��.Ĵ�<V��wϢ4�HV:,�{-�
8e$B��Ƚ��aEkʨ�xw/�P��ܻ�}d]��o�f��lWN�����i)E�JN�P��<c�s�f�;�b����x)nY ���{6���7�a�am��|�5�(�N�4{d2!{�UPoS�Y��y?����`�a�ǉ��߆�uY�u�p�ܱ?�������$�S����u��Т���SSIh��|����OP��'p�9�&
�Ͷ�ބ�3�'K)O�Ar�y����V�&/D��.gor�2gq�n�_�ܞ�9��mj�؊T��vz3�z9��}�x����n׾-̘��������K�:pXԉ	qLs��f^���_�D۫��.��S�WfJ���瓃�MCÓ�	����*k$~lJv���Zƕp���g�*^���[����7��>;WM&ˎ�T#Ǖ�J}NLt�? ��\�=U���?�5��B��@�CJ=�7�w���\p�m�]'М��c�t c)��K9�r�"�yݓK3�e��1k��y������#*���������R��`0u�##�	��:}��w�KH?�Ł)�r�e�����2��F��vE�Ͳ��1]�x�ah���E���oe_S6�F�k��w�e�D���3ENE�4�HW��B��.�u�t�+(�T:Ss��(�ڐ� )����*a��\����+����� Q��(К�󞠡���w�Y�U��~��*�`-��b�E}�����G�cZl97	�r��3��n�a�n���x��y���6��9J
�-���^�M�e�����5�|�9PP,V��J��ܾ�z��Ib��54�}�EA���"����_�H��48j�(2�uT_��R7�i�Zo�K}w	��6�����m�7A�tC�]���i�=��X�VĪa��{�GFjA�T0�/Z����w%�%ד������'����AJ3�H�:=� pL�r�9JJЍ��0�OI�p3�U���	����c��|4��GSP���QCĸ���C{�4�F�c�/8�T&ě���1�TSc���Ә\# d.�>�#7�%�̇٘�Er�Z���Y�YJ�N-˶�㦘�c4��b{�� ��C��7f8u?��:!��Q�{��
��y�����q����q�r��>��W���U����G.�&�"�0I��,�`�'��)k
�k���
�D��-�~�c�o+��tRv��u궣�1$� �ꏮ��&�m��5gr���j�WY�/fq�'\���D�WZE��=C�5��Gyr��e��SM�V������$�~����Q�y]�/�)��Ϯ̺�ѹ2�5�W�~�<��3Wwa6�枙�f�;^�M4�㣛�f`$B\K3��ef�{�w��qYN��p��Z�@v��+)�H9x�����:����K�LH���g/��蟖4Kę��D�á?�n��c������!��w[,��*n"���(�������M��_�=I�ȋ�J E�C��[3{9knl�fb�(��*�is:�"�]}�U�?�X�L�[G���9p�\��l�}ɝ����}���4����H�Y'У/痍��!7.y�gb �����bJdː���s��ܱ`张��+�(ܭ�U���<��x��'8ޛ`͒��Ôt�t�H�{mUp�?���G�w�S��ک�R?ql9#n�guY	����H]�U���']tw(�.u/6���7
���>ryp|Wؼ���&��4�0]��q�ΗY_��e4i½\i��h�&��"~ȁ�M��eȓ>%N�g�籼����s�ՠZ"�+ǉZ�@�r�	5V�7cBK� �d��CQ\&��I�D!��~�\�&.@�`Hb�)�=�8E
�ٳ����l,��(Jr>P�Cx��3���jw��0�(�cU���T֩\�6��Yr 9h!1���~�CL���i2�� z�;,�o����í��t�'��j<��!mQ{]	x�*(��݂����
��g ���F�[�O��d��F&�}�!!�n�x���z%�]�9ݱ��M��>~�B�f���g��ϵ׼�4�ݸGh�hR�� 㷆��'���.M�q"o���ᶸ=��H3���g�ͣ#��}.f�N��NgkC��r���׋W�ѮH��'�����e�դ$���]CJ��b����k��
��tz�d'�wB�P�'�������� �?p��A��Iރ��Kسh�/ULfF�h-᱐m��i	oE��sv��H�����	���#��1H�~`��E�{Rja��`T�+���Zd����v�0�Cu*^Ôz�VV{X�̧oT�P�B�YH$����Bj�HI5\�x��s���4�bg1r��Z�Co�U�C�� �(�'�S��\�-3�{�@�����
���㩽�p\Ā1'MB��F�k��~0ʣ����3 xh��X`u��/�D� �o�QX5@?������{�n��5��\�Kz�m�t���G�@��t�U�� �x�}C�>�*�޸��7���������\�h�1�--1���U^�"�k��#'pg�8�\Awpɒ��L����j}��&4�ki����on��Y<��S�r�%@>��ز�<�.j�����Q���p���pa�hAt0��iƔ���@��sJg��u/�X�\i%�WR��+��w2VL�o/�#r����6�>��^���Ebsi�uf��l�(û�8?��c�/o���
2�,�O��H'��ݱ�8�J0���6�\~6�/^:�Q�5�S-8\ꔢeݟ��Ai��#��Ixd������"fz�՝r����;��Q+ }�P���F�%gr�G�;;�D�_!��v%gvZ���N G�ͪ����VN�(}GF��I���ߪ �n&�l8'��?m
L�"4�f�ד�ZU*>V�-[�6��A�є�Nx���#��U�[�a:����3�2-��W�'5܃)�s�}�����	_�A�oDnf:jt$�\wJ���i͚����=��A��/���G+�V_����34���|�s���y�y=�8���45�3��#%���[*z߰4j��V��Zf��w9K�܆e��l����6�B�km��{d����6�1`�W� ��&�� ���j�m"�{vi<U?����xߙ$��t+zL�y�D�s�����%U�2��&���͎�*�;�����9�Y����G�o�c{'c�a9��ș�3!�nP��Ҏo��Zv٤�@�M��[�P��~y)k\y��7�MY�)ۉ1Ǘc�Մ�ߙָiU��0"1������Uԇ��*ע�,����:�iI�	nZ���ݽ*�	��ׇ��A%|�?ǲ��nI�4.<^������{� J�BR�E`E2�u1�Ց���I�_CAU�H;"D�T�S�f�?,T5M�2�b+u̎7}�/j~K�I�vX�+���'�/Gu�YR���h��u�/�m�ܡ`?���ۋG#�^5���aЕ��nM������n#_Q��71Rt��$%���Xe�� 	>�I�}�U�"�⯋ŀw��{蠳lQ��>����ǔ�jI���_AtP�H4#	b���/A��:K1�WzkL���_%u�T�4N=�u��ec��j�Ql�����ҕ��g��a!����d���!G�C�����PL}�Cny�~���w}O�C�Wh�ݽL�%�?(���V���0O���%-D�	
vkw7��\�n�C-���0X�`�d��}~d.���pHXx1�̖�8_���_�{E��K�[�;�V)�=pTG� Q8nd��>�"�1Ȭ`�N<X���D%b�ZԐ���e���tB��:e�5����>j'sK	��k�,��	2��gC�t��xq0)�\#����{���Oo0�������P>4�-�S#��Ў�=ĎVC\-���L���)�K:�������	7��g�JQ:ty�<�n��`�)�����F�U��:�����挏c�@�HC.K�?\����>ճw���V�{��7AHE9~��7ť�qA[ �^;�Y���յ{h�����g���(v��\.N�j���h�RHh=����Z+@����R�:!1@r�Ab�l�y��	�(���ދg��O-��t�0ʶ�x8�8�^��;��	ä:y	"�T����,w4��2C빊�r��p�*���i�G�R��Tڨu^:|���G0�Q�g�3šݰ�6t�x�t�*J�Y
 8���/�y�z`wR���3�x�n��]d>v&�qW����?�`�K̞� 0��yF�5YZ�QY�@0]��~ؒ�sD E��o�5x��`M��T���U����V&U@R2~Èo�ߗ-��'5��|�a�Y��D҄�5���?����.����}�ܘ+6h`��Ӱ�Y˕t�_<�HQ�Sęf	6����Jh�6x���#w�+�+f�js���Gz�%�����.��NQ�:�omOoa�J4��UXl�O�K.���ݦ�nA��SXU�i�d~���H��h�,ؚ���}�D�}eOR��v~�/]Ab�\2
�1רEAk~a{z��t�Z����S�_��8�9$���ɧ��z�K�z_ۊׁ`Z� r�4Ɂ���J<`�;H���It������������:3�G�����a��{�����/O �$������'�?砜�y
�D���m1�%�HezSs�^-�A����m���U�h�Q2�W��9oW
�Q��cU��m��K�B����K��Ml��h2B�1�����E�CP�N��3bz#X����xuű)�VL��q��{��L�!��%@������o.܅~/�#�&��b 9oi'�WN}��o����vĖ (����*Y`j�����$��R�)��DN�����}�
�j��!�r]O�,�>��r�ă�5Im���	�H�(��z�ɝ�)����k�m�ʊ�4S� �3L��7��;r�=��E���X>c�����D���w�P	� ����z]̼#Q��-HDO�QL��_e�xe���qaѫ���
ۊ�����(����C<Ӳ�yov�Y�N�[x�QO�>�%�cq>yf��T�g�	���e�.�8�`���3����5����Z}��֝R��?�����3�Oɖ%��������w��<~grG���t@lє>��`6�<�@S�GO3�h�s[�C*�Q?Mᘢ�­�z-"͐@���G5������{/
���_[�)v���LO3�Z��ɍ����@H�㾾�^�5x=�79�(�\����1��%r�������O��(��3>�*��@�E�*X�^��)u�)�xDR�_���et�宅0b+���L)��^�O�J8#�S԰]��ᥰS��e�c��(�.*�=JW@��1]���G:~��qF�EPݒ)]'�}Q�Cl8��i��-���rK�V���!!�K��.��C����h�5H��v�p��Vٮ�6=�ʂ�`|*�*�����	T�D��?C��� ,��G��u)ko��2p��.�����l��I�	��3�Ry��|������I|oLGL�_���u�������6o`���Qo��aQ��Oy�.�ɦ�I�