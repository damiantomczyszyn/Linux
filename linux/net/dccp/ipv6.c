RRAY_SIZE(lock_classes); i++) {
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
 * that <prev>��q��>�aʴ�a��B�\�m���c�ً�U��\r�8����`�|�߉��	Y��3@zB=;`9�,��`_�������7�����k[����C�tZ�*���Ej7(`!SP?�_e ������6藱/�P-Q����e؂4�[Xx���u;�,+��#��E�Q9TY}Kv����4����������^�p��Q:u?��ީ�~
�u)Fw�`�|�;�Nl�%��4��3ӣ����{�qjLT�z�m38����6s:����M�qE��.e�/W�n�t�/�<y���ɋ�I��PС��2[B������{�vјR\Xɒn: �M�6�����i3��
t�Ќ�V�<�UU��ne0茶6���ֺ�y�66@�Js`�SŒ�����j�UMfҏ5�I�1bxOn�!_�j� �h8߸M�/�^����fy%an��>i_��<��Ԃ�"�rO1( �0f��^
��7'��9F�9%�k�:%�a,�8����y��yw��J�V��N��P?��?�t�t)9���Wgd7��ߨ ��*ӧ���I��G�Q�nL~+�]�S_�'ҏ��+�\�Y8�e�M;�����;�%�C��Pm������zٌxyg,�����L��י��w�~��n�R@ެ�(F��@TN��P�U��bS�ZOH��}d
>�+�wVmؽC*��u�ʪ�f/'�������0���j=!`Q��r�Y�y�|?��ٕ����ʏ:7�t"��k��%+ү2J�~��|sMN����4QS��9��є�n �y��y�7���
�dk,W��p��x'�l7��,���ؕ�w(�y �|(�_�S)�Z�B���VwL�7G��gi��9:pU�KpXr(��RX�m� �'�L#N1�L�8�Zi@UoK\�kJ����^�|�N�(7w���c; M`�t�,2@�M�7Yu���q��'(�C.�-Ya�  ���]��Ka�
�����3P���W�ό7:Z��d��ݲp
�ѷ�0q�8�؆ti�I;�fq�����)�>'����PT��^�=j77ϙ2ᅿd�T+�}Y��W��bߊ7P�r�#�d9�FoM���jL����}�(�Jp������A���"���gqYO����KS3�y�"(W�U����Z8]T�L �$Tp��|;����b&�=s�
�'xѨO�S�N�mX���b�8$�`�'�K�&���{����q()Ȁ\3���o8�=�C5�Q!�aH��ߞ�E�T�s@tv�2���{ 
؊���-j�_�uR���as�z��C�
��r:.^�MW&�$�+i�u�HW�=u��{�c? ޢL�/�t��_���7 A
�c�Ε�<�O�|����T˾.�`2��lꯅâ#@���'V�ڵn����Y-�X��V-�3���Ej4����@nD���~�=ճFx13��� ��?��}M-
��� ��LU�Ã�B2bpOcA��/FH5��}�ΠwH/�H����/��o��[g]���`J3����R�kK��@+�ޮ��7���;R��r�ތ��Lk���� �}���ư~��������V̺�䗼Y�9���v٥���%\�ġ�z������I�~ �:�>*l����`E43���_�c^�
�r��.��S8�	�]w�Z4i���Ӕ�kFD�w�tg?�+���+��o7b���\�k;��O-Ed[�B�g����3����΂��O��Y�ƩL�}
j�Pu��|���T
^�LĽ!��7F3l A�tN |ݤ�:��{�(�RQdЄ@ۡ(��O���ܣC�l:�3�4�Tth�+kp����
�E�:��=�	7�EO~������̇n[$>|�ĉ��8��G���U�Y�S�w<���Ĕ�ϊa������nl�P��<]~�Z�.�K��u�R�9��Yg<�ל�e������B�A������Dn�6�]�z&=-�l���7q'��Z��������W~��Zyb4%�w�D����/Y��'-�T?*�����
R�(}��,4oуa(���/����a�� $�x.�j��~��xU�J	������Ps�L�#�	 ĸZ׋�������.D~��*��z�ܲ����s���V�=�H�4.ѐ���(m��%����z&�{�b��;4�9,6O�w��ל�Bܪ}�oG�~��q��P�E
c�������O��R���y�04�<�	����Fվ����QB-ʭ�Т={ ����b����r+��DG�F4ʅ\d��
_�ү��� ����a�f�17�YKf�%��Dm�M��{������[��`��(m�_(���%�)Ȗ�vB:�y��LAsW��6TQ���R��bjُIʷ�"'��&��,�XR�%����v����id{j���Ҥ��P�hI���Y�׳t��Y|7Ey
=���*�r~��5l�&!�D87zu���tq|�5	�T���ł�b�3|o5�.����*�7?K����#6�BEޯ��1*���+q�7�j�����AJ�k};�f;*v~�l�%����f��EGށ"j��A� ��cHUO�ϓ�)�vmX����O��0i�����47�{�zf< ��W�2q���+1t$i�]Lf~��d���9��tw�>�v|	�U�����/)�s��6�
���m��W#	<�m����Ӎ`|vV� @��Ј]�#���ȣ�����<��m,[m৏�l]�M~��>@���S��i�7� ���D��1�U�2�%B�,� :���BX}I?��x8n�]�3>.�_㐸�	�i�S*f���#�ٳ���Ͻg�0$4��?������A�C�ay��Y�%%�@�h�o����������|w�r[>�k_�DT��ƍ�0��'R��T7��r�w�ɬ9��z����+Ҷ�6�ԕl2*`�4�9�z�i8����R���;LK�����P`1��q�������x�¼I�(���O��I�~~�o���(!��ˬ��ү����+��e�nK$ �+�ګr@�_1#�#�9�l]�����I-0O�yw�Ç�֬����߸�f��YFV:�ɯ�@��"mͻ.�~��(͙9��/��nlO��Y� �b��Y��? M���0�U��0o)��)�&���얧>b��YC�x�G��_�~$pŏ��%V8L����hbpH�Z)-��m�[td�T1�؎���A��]�l���[�Ԛ��O+�
V�AX��r8o�ݦ5�n+�5�(�iFB!�;{�/���$�y�5<�-P~����d�_㧫�iO{Vxz�^7���g�IW=)�`c�=�q�f+�.(ܶ�SU.WI��z��	�������HZ�Ub�!S����=�r&�t�
n���#]����)��Y{3!����ΤT���P���b�l~�5
B�wnQ��غt��w[�Q�+E�vm�&pH�����&JhJ�v~X]~��5vڛ)q�_��R�����k���v�W#��d>�7���DBZҔP��$�+��� 6��#9���W7�j�?M�����u/��
�6)��9�,P<u�Z8)N�ķ��y����=�E6�`��Q�b��@ J������LU�5��_2�WJ^�>��-<���p�?sh�"������D��r�ٍ��f�Ǳ��Zhp�NZH��]���,��m��zl��4�$��"tt����].?g��I�0�ƨ�?j0�@(,�.H��i
|���%�ڤ�"�=��m�){;�۪���3����)Y�kwoD%fv����z�y*�-�&3���-gk�%mG�4����ژ�4UJ��\PA+���n��OXsא�n!
�dM�1��[��!(���������vn�\V\n���YB�*M8�Y��?���C��1���G1��-�'d�R_��Ë����}��\�Z|���I�"��`��)� ��V��h08l1�11���+��U6�N�����Pٞ�K.L|�H`��'���sk/|>uz���+}*��CY��g���ƍ����5}p5v�)�L��2$է��'	��_xMκ,��׷.Τ�sn��L-J�m�E��ݾm�C�0��\�Қ�Z%�JA^{
�dl�c{(�vtͬli�ei����o1��c�� ��cɳ��ۄ�.d]�-�/��:X�,��n,j����R��3�D)����C.�R�x���w��4r2<�,ЙT����OùK\a�$M㾂�F��˿p�Ű~��x��m����\�
����[��k�0��8N�d��D^و�Bj�|=π�r��^!��2��=jz�G�A*+D1+���@
��i9x��+`Y.1R(��k�o��1�=V3
&�.�
���.���֝ip-izn$�P��[M���5���0(O�{����Ekrՠ�+U�3ms'1����0T"t��#ѰW~�)�7mz�K	v�?p^:-N�`�Wpuͅ<\�y$$�1���M�iY���W/@��_"GP_ї�
���?da��f��BBa�6q��R=��r$]�����r�3K�*�X�ɧ�][�
�ˊ��܏�-�g$p�-�W9�����	c����^�r �1soު����L3�gk�08��u�8qqd�x*q"����k�(����\���B0]8�?�H�d������<Ŵ�A���If���~Vh�vn��Z1�E�l�X����`�T
8e$B��Ƚ�
�Ͷ�ބ�3�'K)O�Ar�y����V�&/D��.gor�2gq�n�_�ܞ�9��mj�؊T��vz3�z9��}�x����n׾-̘��������K�:pXԉ	qLs��f^���_�D۫��.��S�WfJ���瓃�MCÓ�	����*k$~lJv���Zƕp���g�*^���[����7��>;WM&ˎ�T#Ǖ�J}NLt�? ��\�=U���?�5��B��@�CJ=�7�w���\p�m�]'М��c�t c)��K9�r�"�yݓK3�e��1k��y������#*���������R��`0u�##�	��:}��w�KH?�Ł)�r�e�����2��F��vE�Ͳ��1]�x�ah���E���oe_S6�F�k��w�e�D���3ENE�4�HW��B��.�u�t�+(�T:Ss��(�ڐ� )����*a��\����+����� Q��(К�󞠡���w�Y�U��~��*�`-��b�E}�����G�cZl97	�r��3��n�a�n���x��y���6��9J
�-���^�M�e�����5�|�9PP,V��J��ܾ�z��Ib��54�}�EA���"����_�H��48j�(2�uT_��R7�i�Zo�K}w	��6�����m�7A�tC�
��y�����q����q�r��>��W���U����G.�&�"�0I��,�`�'��)k
�k���
�D
���>ryp|Wؼ���&��4�0]��q�ΗY_��e4i½\i��h�&��"~ȁ�M��eȓ>%N�g�籼����s�ՠZ"�+ǉZ�@�r�	5V�7cBK� �d��CQ\&��I�D!��~�\�&.@�`Hb�)�=�8E
�ٳ����l,��(Jr>P�Cx��3���jw��0�(�cU���T֩\�6��Yr 9h!1���~�CL���i2�� z�;,�o����í��t�'��j<��!mQ{]	x�*(��݂����
��g ���F�[�O��d��F&�}�!!�n�x���z%�]�9ݱ��M��>~�B�f���g��ϵ׼�4�ݸGh�hR�� 㷆��'���.M�q"o���ᶸ=��H3���g�ͣ#��}.f�N��NgkC��r���׋W�ѮH��'�����e�դ$���]CJ��b����k��
��tz�d'�wB�P�'�������� �?p��A��Iރ��Kسh�/ULfF�h-᱐m��i	oE��sv��H�����	���#��1H�~`��E�{Rja��`T�+���Zd����v�0�Cu*^Ôz�VV{X�̧oT�P�B�
���㩽�p\Ā1'MB��F�k��~0ʣ����3 xh��X`u��/�D� �o�QX5@?������{�n
2�,�O��H'��ݱ�8�J0���6�\~6�/^:�Q�5�S-8\ꔢeݟ��Ai��#��Ixd������"fz�՝r����;��Q+ }�P���F�%gr�G�;;�D�_!��v%gvZ���N G�ͪ����VN�(}GF��I���ߪ �n&�l8'��?m
L�"4�f�ד�ZU*>V�-[�6��A�є�Nx���#��U�[�a:����3�2-��W�'5܃)�s�}�����	_�A�oDnf:jt$�\wJ�
vkw7��\�n�C-���0X�`�d��}~d.���pHXx1�̖�8_���_�{E��K�[�;�V)�=pTG� Q8nd��>�"�1Ȭ`�N<X���D%b�ZԐ���e���tB��:e�5����>j'sK	��k�,��	2��gC�t��xq0)�\#����{���Oo0�������P
 8���/�y�z`wR���3�x�n��]d>v&�qW����?�`�K̞� 0��yF�5YZ�QY�@0]��~ؒ�sD E��o�5x��`M��T���U����V&U@R2~Èo�ߗ-��'5��|�a�Y��D҄�5���?����.����}�ܘ+6h`��Ӱ�Y˕t�_<�HQ�Sęf	6����Jh�6x���#
�1רEAk~a{z��t�Z����S�_��8�9$���ɧ��z�K�z_ۊׁ`Z� r�4Ɂ���J<`�;H���It������������:3�G�����a��{�
�D���m1�%�HezSs�^-�A����m���U�h�Q2�W��9oW
�Q��cU��m��K�B����K��Ml��h2B�1�����E�CP�N��3bz#X����xuű)�VL��q��{��L�!��%@������o.܅~/�#�&��b 9oi'�WN}��o����vĖ (����*Y`j�����$��R�)��DN�����}�
�j��!�r]O�,�>��r�ă�5Im���	�H�(��z�ɝ�)�
ۊ�����(����C<Ӳ�yov�Y�N�[x�QO�>�%�cq>yf��T�g�	���e�.�8�`���3����5����Z}��֝R��?�����3�Oɖ%��������w��<~grG���t@lє>��`6�<�@S�GO3�h�s[�C*�Q?Mᘢ�­�z-"͐@���G5������{/
���_[�)v���LO3�Z��ɍ����@H�㾾�^�5x=�79�(�\����1��%r�������O��(��3>�*��@�E�*X�^��)u�)�xDR�_���et�宅0b+���L)��^�O�J8#�S԰]��ᥰS��e�c��(�.*�=JW@��1]���G:~��qF�EPݒ)]'�}Q�Cl8��i��-���rK�V���!!�K��.��C����h�5H��v�p��Vٮ�6=�ʂ�`|*�*�����	T�D��?C��� ,��G��u)ko��2p��.�����l��I�	��3�Ry��|������I|oLGL�_���u�������6o`���Qo��aQ��Oy�.�ɦ�I���x�����*!�7~֋�tu���h�v5r�A�ד%7���bM�5Oe"���eZzR��zt�E �x�?_̓����	J(3��
O��u�3�X^�2ua�~���*�^L�h]RL}� u����U�����0P#A�L}�`�	�i�N��dK��������+̄���3�������ǯ�2T����w�Y��λ�$끍�j�����b�"x�N�D��$Xbb�
�k
�^k�:/�y�23)_���}�,Z�p
�����<�T��H�J��r(�