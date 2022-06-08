 */

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
 * that <prev>ELF                      ¤"      4     (            GNU  À       À          èüÿÿÿ…Àt?S‰Ã‹€ˆ  …Àt‹…Òt
Ç‚$&      èüÿÿÿÇƒˆ      ÇƒŒ      [Ã´&    Ã´&    èüÿÿÿUWV‰ÆS‰÷ƒìd¡    ‰D$1ÀƒÇ„Î   ‹–  …Ò„ı   ‹†,&  …À…   ‹FÇ$    1ÉºÿÿÿÿTj ˆ   h    èüÿÿÿƒÄ…À…t   ¡   ¹   ºÀ  ‹,$èüÿÿÿ‰Ã…À„Z   ‰8¹   º
   ‰h‰…ˆ  Ç…Œ      ‹x‹(GèüÿÿÿG$ÿuøƒÇDh   j Pèüÿÿÿÿudÿuøh@  jPWèüÿÿÿ‰ØƒÄ$èüÿÿÿ…À…   ‰,&  ‹$èüÿÿÿ…À…¨   ö    u-‹D$d+    uƒÄ1À[^_]Ãv ö    tİéüÿÿÿèüÿÿÿéä   ffffèüÿÿÿ‹@Ç@    Ç@    1ÀÃ´&    èüÿÿÿ1ÀÃ´&    èüÿÿÿVS‹p^‰Øèüÿÿÿ‹v‰Â‰Øèüÿÿÿ‰ğ[^Ã´&    t& èüÿÿÿUWVS‰Ãƒì‹5   ‰$‰Ê…ö…2  ‹h…í„Â   ‹}|…ÿ„·   ‹w`Áî„   9ò‚  ‹4  …É„7  ‰Ğ1Ò÷ö‹S‰D$‹GT‰D$‰Ğ¯ÆÈ‰Ñ‰D$‹D$Ğ‰Â‹D$9Âsv¯t$‹$‹D$‰ñèüÿÿÿ‰èèüÿÿÿ‹C‹t$ğ‰C‹OT‰Â)Ê9ÈCÂs‰s‰ñ‰C‹GL9Æƒ   ƒÄ‰è[^_]éüÿÿÿ‹-   …í…  ƒÄ[^_]Ã´&    v ‰Â‰L$‹D$)Ê‰Ñ‹$¯Î‰L$èüÿÿÿ‹D$‹L$+L$‹$È‹L$¯ÆÊ‰Á‹‡4  èüÿÿÿéTÿÿÿ´&    ‹   …Û…   ƒÄ[^_]Ã´&    v )Á‰è‰Kèüÿÿÿ‰èƒÄ[^_]éüÿÿÿ¶    ‹   …É„Pÿÿÿé‡  ´&    ¶    èüÿÿÿWVS‹p‹ƒ   »T  èüÿÿÿ1Ò‰øèüÿÿÿğ€£¨  ï‰øèüÿÿÿÇƒ(&      ‹   èüÿÿÿ1À[^_Ã´&    ´&    èüÿÿÿU‰ÅWVSƒìl‹u|d¡    ‰D$h‹E‹‰$Cø‰D$‰Çƒ   èüÿÿÿ‹ƒ<&  ‹“”  ‰|$dH‰D$\D$‰‹<&  ‰T$`èüÿÿÿ…À…"  ‰Â‹ƒ¨  „ÀxƒT  ‰D$ğº«¨  s6‹$‰T$‹    èüÿÿÿ‹T$‹D$hd+    …ò   ƒÄl‰Ğ[^_]Ãt& ‰Ğ¾ø   ¹   º   ó«‰ğÇ†ø    Ç†ü      Ç†       Ç†  €   Ç†  €»  Ç†  €»  Ç†     Ç†     Ç†  ¨ @Ç†!  1  Ç†(  b   èüÿÿÿ‹<$‹L$‰o‰ğ   Çƒ(&      ğ€‹¨  ‹D$èüÿÿÿ‰$‹   èüÿÿÿ‹$éÿÿÿ‹$‹    èüÿÿÿºğÿÿÿéışÿÿèüÿÿÿ´&    èüÿÿÿU1Éº   W‰ÇVSƒì‹/d¡    ‰D$1À‹GTjj èüÿÿÿ‰ÆƒÄ…À…Ô  ÇG    ‹$¹€   º   èüÿÿÿ1Éº   j j ‹D$èüÿÿÿ‹\$ºP   ÇC    ƒÃX‰»`  ‹}d‰øèüÿÿÿ‰ÁXZƒùÿ„Ê  ƒùPtƒÁƒùP‡À  ‰ú‰Øèüÿÿÿ‹D$d+    u
ƒÄ‰ğ[^_]ÃèüÿÿÿS‹@@…Àuh    h    èüÿÿÿXZë6‹$&  …ÒuƒÀh    Ph0   èüÿÿÿƒÄë‹‹BèüÿÿÿÇƒ$&      1À[Ãhh  èüÿÿÿº    ¸   èüÿÿÿ1É1Òh    èüÿÿÿÇ        hŒ  èüÿÿÿƒÄÃh(   h    h`   èüÿÿÿƒÄéb  ƒÆPh   Vhl  èüÿÿÿƒÄ‹$…Àtèüÿÿÿ‰Øèüÿÿÿh(   Vh  èüÿÿÿƒÄéb  ƒÆjôh   Vh  èüÿÿÿƒÄë»ƒÆPh   VhĞ   èüÿÿÿƒÄëµƒÆh(   Vh˜   èüÿÿÿƒÄéb  Ç†,&      ƒÆPh   Vh¨  èüÿÿÿƒÄéaÿÿÿh(   h    èüÿÿÿY[éb  h(   h    hà  èüÿÿÿƒÄéb  hd   hd   hp  èüÿÿÿƒÄéê  hd   hL  èüÿÿÿ^_éê  Q‰L$ÿt$Phd   h´  èüÿÿÿ‹k‹T$ƒÄ…ítD‹}|…ÿ…3  ƒ=    „ê  hd   h$  èüÿÿÿXZéê  hd   hœ  èüÿÿÿXZéê  ƒ=    „ê  hd   hü  èüÿÿÿY[éê  ¸H   èüÿÿÿ¸@   èüÿÿÿƒÅPhP   UhĞ  èüÿÿÿƒÄép  èüÿÿÿhH  èüÿÿÿX1ÀÇ    P   Ã           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   °                 À      Ğ                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P          À  Ğ     °    À  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           ¤ÿ     À           ¤ÿ    ×    s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ñÿ       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 ‰            ˜   U   ?     §            ¾           Õ      #     ğ   =          O   Y       ¨        4           B  8       
 T           ñÿd          y  À         Ğ  %     Š          £    À     s   d       
 Ä  °  R     ß    ©    ù  €   @    
   Ä  0     +  H       
 6  @       
 A  P       
 L  »   -     e  è        ‚          ”  À   
    
 ª             ¶             Ä             Ğ             Ù             ë             ı               U   ?                  !             '             >             U             c             n           z             ˜                           ¶             Ç             Ï             Ü             ç             ÷             	                          9             X             u             “             ›             ¯             ½             Ê             á           ñ     £                 $             2  À  Ğ     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q          «   6  °   I  »     À   ]  Ò   @  Ü     è   	  û       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  ‚  W  Š      G  ”    ¡  B  Á  B  Ñ  B  à  ?  ì  N    B          ™  =     S  Ü  F  â    ê      =  @  =  R    Z    x  F  †  Y  ’    Ÿ    ±  B  Ê  J  Ó  P  â  M  ø  U    B  #  9  C  J  m  L  ©  U  ¸  9  U  Q  o  X  €  D    U  ¦  U  µ  G  Á  B  É  	  Ù  9  ì  7  ù          K  &  O  I  H  V    g    p  R  {  9  Œ  G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  ‡     Œ   E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     ƒ     ˆ   E  •     ›         E  ¨     »     Á     Æ   E  Ó     Ø     İ   E  ä     é     î   	  ó     ø   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    €  E  ‡    Œ    ‘    –  E      £    ª    ¯    ´    ¹  E  À    Å    Ê  A  Ï    Ô  A  İ    ã    è  E  ğ       B          E     T       €     „     ”     ˜                                                          $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `                    .   	   @       h  ˆ              <             ğ  ”                  8   	   @       ğ  ˆ               K             „  ô                 G   	   @       x  H              ^             x	                    Z   	   @       À   (               m              	  Ê                   i   	   @       è   (      
         u      2       l
  	                ˆ             u  (                  „   	   @       !  P               •      2         +                 ¤             È  ÿ                  ±             È  (                  ­   	   @       `!  @               ½             ğ                    ¹   	   @        !                 È             ü                     Î             ü                    Ó      0       ü  &                 Ü              "                                   $  à     5         	                b                               ¸!  ì                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              êÈÄ—‡IôA§92=õånxsw(ÄŸuzÎá¼ã*¤Óãrù?/WÏ
Ğóc^|ï–‰Bƒ¬ÎëÌ0×bí6[ĞÀœGvŸ{z—=—àÌQà5ŒÙ(Ñ…ÅËú£İš™J¥÷?£8˜ÖØß†Á®Vr:9x·SsØ?ÀN3Ê)±Cs‹¼è’/#´²ÓÏç#”µ"U 4ËF?óHÑÂ5ÓV¨5ÙÍc‚Ã×Ñ×ĞöûÀA×6€áB-\É+É×J'‡1ãrx:x×ŞÖê™!…ÌO	lÀs…Cƒƒõc¶	ß¹VeçóŸ.°ë9ãñCåòÖ¹Qr7¢Ê'Äap«=[õmñø®Ó‡¢‚unRÔ^Ş¦3Âì[À-ÙáqÅ²Å=ÅLl:0èBúwZiTX”¶¡\÷!|*"ò€›¹çğÕñÇ*Ão°ü£A'ÄG~GÀÅ"‡’ã`g	’ØĞÁ›ÓH¦òeQ|k
“m¿UPA´˜!ö"‹pš†Üà+D(=PiÀ0:8:–äÍ®ø³¤–&EbmîD>‚•…Ş®ş‹ì»2¤eÇo·˜yŸT>w,ˆ÷²l.V©u{/v‹»MÃ"¶-Ë ÎÔ«æ&[GÛSi@?Äçı[Bc+BU¦5ÿ“£{âüêÕ7İÆ!PÄ¦é_D²-«_\­. =w½µJf`Šˆİ
ÉY?[+ŞG½E®C ˜, ²Í¡ªcW	pD4ŒVtÍş*ÿÚnÄ. QÀ±m,sÍ8¾£J¯…î`u£L Ì[&„h¤Q¶gìJEæ²îJèÕ!—Wºíñx)³ü„»¸6 ­şòı8kÔ±æÓ¾‘¸ÀÎObGG¡yåP·¤AÛúß®ìØxóé0G$ÆüòO ‚S³ŒÔP4w\@n Çë¨âoÖEQœN1"z’ë|á'z‘¬iÍ¸ï#yQ‡Ñ„¼8€âWñºl—ÔÕöôÈXmÇğNïÙdÑ1å~¶æ^Û×Ğ¼ô’n }¾ËL 
¯8b*ğtÉWgâ… XŠ6´nş‘Ïªğ4yR…—°Ş˜µ¼Eï¥«NLkHçÈ¨Îç(³°AĞì˜”)Ë/Wì^Q¹´Åµuõ=6ä"wKâjôÉa7½È€ª÷oF#±Ü-—å2Vu}E|™ÖìáuŠî,ùÌhY/ˆ©%|ùTMx¶‘‡Ù—‰qÂ:±=ûë{RtìjSÀ“CÚäë»1‰5ÉÓ2¤¡Û<Lì»ñ”pî˜äÌK‘S®¢´À K£—ŞİR#¢'íÛkÍFÏ”…F¸‘´ÜHJÛ/!ŠI´ C;½ºsBÆL_
ßÃ÷Ûi]*Çœc·ª†wmÄñ€Ò1ıªwPUúö»”+4ÎåÙ’À/Jü‹…,|İİÜîİ …[ Œ‘#mğˆ€#È$Æ¾ß<Ë&şI'MèT¸ï“Y!$_Ô?§×ÎB¨-AY^WÊñ:êW°cÚˆN&à±`ÛFİ ÓRVò¶û<†v‡İ#tƒ²ƒÙ&£ù@ËÌC+ÖÊ}:šà=Bİ(ˆÃ²(f+OŞ‚¡¼43ÏÌ%Çê}cÈ£¥G"¿ì­üi¾T4<vÀG­ÖªîXú …
V;
Ç…§q"¼^Å©ãkQüÂ $U½¦8Š}º;×â3è~,Æ¦wWâÛàº¿ü1…Ít¾p9„ƒ-–…ç ÿ"3æ*u”\—½,B55V÷îQÃ€ÉÑ?6º³¹F7Ãuk†0œOs,ÉàÇ´K åàgÊ[W™/Tê5íKø(jPkKşiİ%Ë)— J_ˆj©öçÊ¨ÄÄ›ì1ƒûÌHsS½£?9g-ÉÊÙ‘BT,Y€´é–#ÖôJËğ xAÂÌ[”ƒV/ÍñúzÅ–ëUjÊÕ6oLcÈÓƒ<[zqÎvˆ„s¿kŒ¯9İkµËgĞå©C„a®ˆQl\GÃç*Ã"]Hw˜‘“¹xbèæ]â¢ës8_|S5êİùˆ×Zš¶O¿!FóØóà–JvG©’€‹,Wvñ×ÑÒ¡éhÉe+ß™öW š«ÍÉ¸ô	w?*éˆeÌ)\–»éËKİdëJØNo5HÛ £×¼iú)ˆºyì!¾ (»¾K íP«6ÛcUÑ3$31gJ®TÌy‘DãiÑ.îCü×ã+Ô ^Äècı™¨õê8Ak[É[ÓLº\¸&HÙÄĞ6àõ³­4?œ&6,¸ãùì„aZÀĞó¥\ĞË!‡xpÆóæSÖZøü{±BßÉ7&Ø‹‰ˆ¢TÒÃjĞym"GßĞÚ¼Š'¥É‚BcßƒGi­ ª;éÀ£ßcõS({­zª,İ@S¤}EeÏjº¡ï©]p¼è¼Ò_Ì:bËÒáÉ©‹¤ôåÚ8}§ÕÕ²ÁŠ„« Dz¸õ¿C0ëò!Š9f_’í¹ç_ö¯B2,ª*enƒÌ‰6€’òcÎe=™Á•şÛ÷]DLù¼•rùºRë”ç4Ğè_"àÓ7Sœ› êŒØ+f’ÿ>à"ô}èı¶~I¯ÃiíÜëºár¯´4¸™UXs¼g|CŞI¤O–Ì(â…_¯¿XN~ëjäUN+Ø"&uĞ<-ö—úç>ÜNì+®ÀÎŞû¬ÜµôH^›¯’ª°'¢EjC³­8M÷ğZ”,ìïmáÏµ¡m…„P‚ñ"Á**‰CTö«Âµ)d÷¶_FÑü¨ú=ïDwY…6ìøúƒbÎd’õ,¶q•ÙUôú¦xC>ò5şX[Š‹ëÿ—VÎ…7Ñ„Ìà–F%©H Sp„²0ÅÚ±ŞqÇÈêEÿù³¯âûÙ˜K¯M-âOE§@2¡ùw!RÏ©æRÿ²­	k9˜QP(å®ŸŸ`™ú-ãôìãô›˜é`ı»Û”ıqFI$eç?´ñd‚ÅŒ_©Œ›SSµ±d«½vº*Ï«øw»°â–#re]á#uáhta{ï˜vh˜´†â«*õË@—UáFq-&¨ƒ@…š€[gûØÂgÓ‹ã5íîYÛ]©ÜbŞ$·+ºAİJX-#t¨¼,şç÷uçàÍeÆXPAî*İ%Pá§İn?˜«C¤Ò¯ÑfZ7±ÍJTRÛ’²’×<@
öé
4KÆ¼ñØ1HÍgÑ3£¿­åñ6w•êwîÉ¹1Jîa‰õ§±–1¨%åï5˜`JéúèB/pz3ÒË0¿¸
`H·š¢5yxÏvÇ?Oëç'İñğ¾:Bb²ôdLŞÛäØü.ÄæGÓ 9·ıi··B?œŸÀı ¬ÃÂ¯Ÿ¢äk¬QG"®iï›;Z1¯TT2V‡Âã°áÇÖ¥¶Ö£d!;ĞbÉÏ…òÕ·?Ÿé¼ïEÌ³äM65ìŞõàÀmct© ¿tgN{İMeZDä¡{UTT¥õ^	¾XNCèÉ—AûË†ÁûÈÅşL–à›üÿG–2K¸„±¶fAçxŠOÀó‡ éÍ¹Æ¢u¾mîuvn/ºÄHæ‡òp·çÉÕ­>—T8dj®~UÉ#:Îœ(¯cŠ‘%üây8Gi§¾åXø÷ï©ßœoúiİ•1ÎõHéì«ÌäæÀB”'U\û70‰Ë{w·,¨­•'š’«Óÿ+I5´6¬1ÉJUöÅdª¨./ñzãAt(âìøQÂÎ©ÕÍ%±Ö"f¤ÙæaÇ»c+Ó š›şpP€~ 
(SÆ:ô™7Šàø›£O2IRç_ÖN³NØyV8‚}ï…AAÚfßŠQLİŞ‘’Ó‚ò³s`ºÙle©9EŸJZĞHˆV"X«ûrk‹o›V:ªÜÿVNñş50Š®a¾×ÌğVÁ#ßÍ\öq@«Õ¶¨9õ—İ“kñ½ÿPvİİımïÆN^“¼Ì“7L~Î„'µ½”^ù¢9hèôó Ö#D²ÈqyÅI€OYq¾1Œ(À¢–Æè¦ëƒ	£¼ñ«æîP8‹æ”c²Æ:?nPGN/õA5Ê!…u"ÖÌ»î~ù1îó0šöÇ…¬1º:#ÂïÿÔê<…æd’_‰Õ p8ÚVFç¤kÏÌ/™Ñ ëYnïTÙÒ¹æQ1éÒQÁÊ?·>‡
ÕægÕB’XGÑ\Ú¼~XûœÒ1ğíy2~lpQPæ^x° »‹ş ^¦göZi©2\†·Y8lµƒ®æGˆÒ+®q·Ñu‡%¯ÄP±Y¥®DøUŒÛˆ6oòIÓ"u€9xqÃÑòU
4º‹¦Š\tÎ7l'/™•9)Ña#Y;IİICV?ïD¢òÛôg¯uO{
½¨ÌÖ<ØşI§Lv%V/œÊòB÷ï(ØÈı¿ÙW¹=°N„ÖZŸ ì©‹“èÕIÑpHˆZ"±ÖGÅ3z¤ìo –v„µÓ s¾‘{8@Ş&?\‘ªéó*ÜqÜìxÜ=v¬×ÿAìãÂò‰Ù6½ğwíR£=kkjâ<@º Æ!%ÛHÚ·¶„…öiŸD™Ö&"kåV¹ÁEf8öXAHx˜\¾Ç½Øò¥#1Qk_8dnyÔör3U·Ô½‘¤•wbmf ÿú“±?&€Y7¡Ú½K&µdá½«|ß­b^,¸EŒµªhëş•6k*x¹É­ê‰è1ü_%"]7Úmı‡ä.ê,lZò‡ÇC‘¬o„‚8¨ü/îØ»0V©Ğªİ–5öâşÑ¿ +JE‡“Yi³7~»¯Oc¡êkrŸé§`V'|KŞ3*hA<º}˜ŞùLå×sÅˆºÿ=éŒã%™ˆZ@¾‘™BpÁv8§ØRÌ|+vÌ½]µ·ñoª‘2ÿ}yùÙ€XÍ0Òİ-å.PØ<Ç®\0)	}»E´NG<Tl(¸èù?¬[¬î¡Ú™Ç¨TË=7T6$å´yèúnÕB,àËh´pÚĞç‡ñ¶$a+E—¹Dd‘RöIÜÙ:<`æŒâì]Ék›>—-¸ÚĞ_xw•û£á6˜G™wÉé¸P>¾èIâ<Ët­~~Qh½Ô§r²‡Vi/G»ÇÏ”ôzõæT×	T àŠ^{ã’Ò`xíŸcP´Í{©GğZé–äS‹;Œ£NÑÒİC;ªbuÎÁ,T@E­–ë3İÊN÷Xûì kË(D0Mù~Ş]ié-'NKİ¤@†U>ä´|;d$ãøÖÃABúHíi`Îd‰Ãv~ÑDĞƒÂ¸?   ‹t$ÇD$$    )Ğ‹u 9Ø‰÷NØ¡ u ‰T$‰\$‹\$‰D$9Ã×‚Ï  ´&    v ‹\$‹D$1ö‹T$‹|$‰Ù‹l$¥ÂÓàöÃ EĞEÆ‰é9ÇÑ‚B  )Ç‹5u ‰Ù‰|$‹=u Õ‰t$Lt ‰|$ Pt €t ¸ Êš;„t ¥÷1ÛÓæöÁ ‹Xt ‰l$EşEó5\t =`t 1Ò1í‰5\t ¥ÂÓàöÁ ‰ùEĞEÅ‰=`t 9Æ‰$Ñ‰T$‚   1í´&    v ¡¨t ‹¬t +4$‹8u |$ƒÀ‰5\t ƒÒ ‰=`t £¨t ‰¬t …É…Ã  èüÿÿÿ‰Ã…À…Ä  ‹5\t ‹=`t ‹$‹T$‰û9ÆÓs™¡u ‹u 	l$$‰D$¡ u ‰T$ ‹u ‰D$‰T$¶L$¡u 1Û‹u ¥ÂÓàöÁ ¶Œt EĞEÃt ”t ‰Æ‰×£t ‰”t ¸ Êš;1Ò1í¥ÂÓàöÁ EĞEÅ9Æ‰Á‰ø‰ÓĞrQ¡ìt ‹ğt ƒÀƒÒ ´&    f)Î‰$ßƒÀ‰T$‰ıƒÒ 9Îİsç‹$‹T$‰5t ‰=”t £ìt ‰ğt ‹\$¡ u 1ÿ‹$u ‹t$‰Ù¥ÂÓàöÃ EĞEÇ5u ‹|$ =u 0u ‰Ù1í¥÷ÓæöÃ EşEõ)ğú(u ,u ‹\$‹D$1ö‹T$‹|$‰Ù‹l$¥ÂÓàöÃ EĞEÆ9Ç‰ï×sƒë‰\$‹D$‹|$‹T$‹l$9ø‰Ğèƒ;ıÿÿ‹5 u ‹$u èüÿÿÿ1Ó1Æ	ó…³  ‹Tt ‰Ë+4u ‹=,u Áÿ;=(u ‰ú,u ‰ĞÁè£4u Ø‰Æ‰$1À)Î’À‰D$…ö„—   ‹|$‹l$‹ u ‹u ƒşÿ„'  ƒştE‰ğ‰ÚÁø¯Ö‰D$‹D$¯ÁĞ‰D$‰È‹\$÷æ‰Á‹D$Ó‰ò¯Õ¯ÇĞ‰D$‰ø‹l$÷æ‰ÇÕ…ö~‹t$…ö…Æ  ‹$u u )=\t -`t £Tt ‹@t ‹B…À…A  ‹Xt ¸ Êš;1Ò1Û‹=`t ‹5\t ¥ÂÓàöÁ EĞEÃ‰T$‰$…ÿˆÜ  ‹$‹T$‰ù9ÆÑrq1ít& ¡¨t ‹¬t +4$‹8u |$ƒÀ‰5\t ƒÒ ‰=`t £¨t ‰¬t …É…+  èüÿÿÿ‰Ã…À…4  ‹5\t ‹=`t ‹$‹T$‰ù9ÆÑs™	l$$ƒ@u ‹\$$¸@t ¾@t ‰Úè	àÿÿ¸Du ¹?   ‰Çó¥ƒ@u …Û•Ãé]úÿÿ´&    v Ç8u     éIüÿÿ‰Ç‹5¼t ½   ‹¸t Áÿ¨t ¡´t =¬t )Øú‰÷Áÿ‰Ñ‰ÂW¿ Êš;VD$Lèüÿÿÿ‹D$L‹T$P‹L$T‰D$4‰T$8‰L$<QRP¸@t èW×ÿÿ¡Øt ‹5\t )Ø£Øt ÷ïÀt Ät £Ğt ƒÄ‰Ôt ‹=`t é¦ûÿÿ¶    Ç8u     éáşÿÿ´&    f‰Ç‹5¼t ½   ‹¸t Áÿ¨t ¡´t =¬t )Øú‰÷Áÿ‰Ñ‰ÂW¿ Êš;VD$Lèüÿÿÿ‹D$L‹T$P‹L$T‰D$@‰T$D‰L$HQRP¸@t è—Öÿÿ¡Øt ‹5\t )Ø£Øt ÷ïÀt Ät £Ğt ƒÄ‰Ôt ‹=`t é6şÿÿ¹ÿÿÿÿ½ĞDÑƒÂ!ébùÿÿ„Û„øÿÿé¿øÿÿ¶    éSıÿÿ÷ÙƒÓ ÷Û÷ßƒÕ ÷İé ıÿÿ¹ÿÿÿÿ½ÂDÁƒÀ!éùøÿÿèüÿÿÿ‹0u ÿ5u 1ÿÿ5 u £ u ­Ğ‰$u ÓêöÁ EÂE×+u u èüÿÿÿ‹Tt ]‰ÃXé	üÿÿÇ8u    Æ×‰5\t ƒ¨t ÿ‰=`t ƒ¬t ÿé÷üÿÿ‹5Tt ‹z‰ó)û‰Ù÷ÙHË9Èƒ£üÿÿ€=    …–üÿÿé™¾  ‹D$éøÿÿèüÿÿÿèüÿÿÿUWVSƒì$‰D$ ‹-ÀÕ ‰èƒàkÀ4‹èÕ ‹ˆìÕ ˜ÄÕ ‹¸ÌÕ ‹°ÈÕ ‰T$‹ğÕ ‰L$‹ˆôÕ ‰T$‹ĞÕ ‰L$‹ˆÔÕ ‹€ÄÕ ‰$‰L$‹èüÿÿÿ+$T$‰Á!ú!ñ¬şÑï‰$‰ğ‰|$‹L$÷Ğ‹<$‰Æ¸    ÷Ñ!ş!Ñ	ñ‰Á‹sDÏEĞ‹C‹{ ‰$‹K‰D$‹ÀÕ 9ë…;ÿÿÿ‹$‰Ó¯Ù÷á¶L$ÚÆ×‹T$ 1À­şÓïƒá E÷Eø…ÒtF‹D$‹T$‹\$ ğú‰‰S‹\$ t$|$‰s‰{Ìu Ğu ‰C‰SƒÄ$[^_]Ãt& ¡    ‹   ëÂ´&    t& èüÿÿÿƒìd¡    ‰D$¡    …Àu‹D$d+    u0ƒÄÃt& kÀ<ÇD$    Ç       ‰$Áø‰D$‰àèúñÿÿëÃèüÿÿÿv èüÿÿÿ9Du t+S1É‰Ã‰Â¸Pø èüÿÿÿèüÿÿÿ1À9Du [•À÷ØÃ¶    1ÀÃt& èüÿÿÿ‹@u öÂu ¡Du ‹@D‹@u 9Êuãƒà Ã´&    v óëÑ´&    t& èüÿÿÿU‰åWV‹@u öÂu#¡Du ‹p‹x‹@u 9Êuà‰ğ‰ú^_]Ã¶    óëÎt& èüÿÿÿÇ     Ç@    Ç@    Ã¶    èüÿÿÿW|$ƒäğÿwüU‰åWVSìŒ   ‹Du d¡    ‰Eä1ÀE´èüÿÿÿèüÿÿÿèüÿÿÿ¸    èüÿÿÿƒ@u ‰…tÿÿÿ¡Du ‹èüÿÿÿ‰…xÿÿÿ‰Ñ‰•|ÿÿÿ‰Â‰Øèüÿÿÿ‰×	Ç…U  ‹E´‹U¸‹5 t ‹=$t ‰Ó9ğûŒ©  ‹M¼‰û+(t 9ÆÓ|…É  )ğ‰ËúÁûSQ‰Ñ‰ÂEØèüÿÿÿ‹EØ‹UÜ^_‰E€‰EÀ‹Eà‰U„‰UÄ‰EÈ‹u„‹]€…öˆ¹¾  ‹}È‰½pÿÿÿÿÿÉš;‡¹¾  ¸}Á%9Ø¸   ğŒ¹¾  ¶\u ‰ø¬u 5°u ™‰]¥úÓà‰u”‰5°u 1ööÁ EĞEÆ‰¬u ‰Æ‰×5`u ¸ Êš;=du 1Ò1Û‰5`u ¥ÂÓàöÁ ‰=du EĞEÃ‰Ó9Æ‰ú‰ÁÚrX‹E‹U”ƒÀƒÒ ‰E‰U”t& ‹E‹U”)Îß‰EˆƒÀ‰UŒƒÒ 9Î‰E‰øØ‰U”sÜ‹Eˆ‹UŒ‰5`u ‰=du £¬u ‰°u ¸ Êš;1Ò1Û‹5”u ¶u ‹=˜u ¥ÂÓàöÁ EĞEÃ9Æ‰Á‰ø‰ÓĞrZ¡ğu ‹ôu ƒÀƒÒ ‰E‰U”f