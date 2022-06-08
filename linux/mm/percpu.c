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
 * that <prev>ELF                      �"      4     (            GNU  �       �          �������t?S�Ë��  ��t���t
ǂ$&      �����ǃ�      ǃ�      [Í�&    Í�&    �����UWV��S����d�    �D$1�����   ��  ����   ��,&  ����   �F�$    1ɺ����Tj �   h    ����������t   �   �   ��  �,$������Å��Z   �8�   �
   �h���  ǅ�      �x�(�G������G$�u���Dh   j P������ud�u�h@  jPW������؃�$��������   ��,&  �$���������   �    u-�D$d+    u��1�[^_]Ív �    t�������������   f�f�f�f�������@�@    �@    1�Í�&    �����1�Í�&    ������VS�p�^��������v���������[^Í�&    �t& �����UWVS�Ã��5   �$�ʅ��2  �h����   �}|����   �w`����   9��  ��4  ���7  ��1����S�D$�GT�D$����ȉщD$�D$ЉD$9�sv�t$�$�D$���������������C�t$��C�OT��)�9�C�s�s��C�GL9���   ����[^_]������-   ���  ��[^_]Í�&    �v �L$�D$)ʉы$�ΉL$������D$�L$+L$�$ȋL$��ʉ���4  ������T�����&    �   ���   ��[^_]Í�&    �v )���K��������[^_]�������    �   ���P����  ��&    ��    �����WVS�p���   ��T  �����1҉����������  �������ǃ(&      �   �����1�[^_Í�&    ��&    �����U��WVS��l�u|d�    �D$h�E��$�C��D$�Ǎ�   �������<&  ���  �|$d�H�D$\�D$��<&  �T$`��������"  ���  ��x��T  �D$����  s6�$�T$�    ������T$�D$hd+    ��   ��l��[^_]Ít& �Ѝ��   �   �   ��ǆ�    ǆ�      ǆ       ǆ  �   ǆ  ��  ǆ  ��  ǆ     ǆ     ǆ  � @ǆ!  1  ǆ(  b   ������<$�L$�o���   ǃ(&      ����  �D$������$�   ������$�����$�    ����������������������&    �����U1ɺ   W��VS���/d�    �D$1��GTjj ������ƃ�����  �G    �$��   �   �����1ɺ   j j �D$������\$�P   �C    ��X��`  �}d���������XZ�����  ��Pt����P��  ����������D$d+    u
����[^_]������S�@@��uh    h    �����XZ�6��$&  ��u��h    Ph0   ����������B�����ǃ$&      1�[�hh  ������    �   �����1�1�h    ������        h�  ��������h(   h    h`   ��������b  ��Ph   Vhl  ��������$��t������������h(   Vh  ��������b  ��j�h   Vh  �������뻃�Ph   Vh�   �������뵃�h(   Vh�   ��������b  ǆ,&      ��Ph   Vh�  ��������a���h(   h    �����Y[�b  h(   h    h�  ��������b  hd   hd   hp  ���������  hd   hL  �����^_��  Q�L$�t$Phd   h�  ������k�T$����tD�}|���3  �=    ��  hd   h$  �����XZ��  hd   h�  �����XZ��  �=    ��  hd   h�  �����Y[��  �H   ������@   �������PhP   Uh�  ��������p  �����hH  �����X1��    P   �           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   �              �  �      �                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
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
     P       �  �  �     �    �  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           ��     �           ��    �  �  s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ��       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 �            �   U   ?     �            �           �      #     �   =          O   Y       �        4           B  8       
 T           ��d  �       y  �       �  �  %     �          �    �     s   d       
 �  �  R     �    �    �  �   @    
   �  0     +  H       
 6  @       
 A  P       
 L  �   -     e  �        �          �  �   
    
 �             �             �             �             �             �             �               U   ?                  !             '             >             U             c             n           z             �             �             �             �             �             �             �             �             	                          9             X             u             �             �             �             �             �             �           �     �                 $             2  �  �     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q     �     �   6  �   I  �     �   ]  �   @  �     �   	  �       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  �  W  �    �  G  �    �  B  �  B  �  B  �  ?  �  N    B          �  =  �  S  �  F  �    �      =  @  =  R    Z    x  F  �  Y  �    �    �  B  �  J  �  P  �  M  �  U    B  #  9  C  J  m  L  �  U  �  9  U  Q  o  X  �  D  �  U  �  U  �  G  �  B  �  	  �  9  �  7  �          K  &  O  I  H  V    g    p  R  {  9  �  G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  �     �   E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     �     �   E  �     �     �   E  �     �     �     �   E  �     �     �   E  �     �     �   	  �     �   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    �  E  �    �    �    �  E  �    �    �    �    �    �  E  �    �    �  A  �    �  A  �    �    �  E  �       B          E     T       �     �     �     �     �                                                    $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `   �                 .   	   @       h  �              <             �  �                  8   	   @       �  �               K             �  �                 G   	   @       x  H              ^             x	                    Z   	   @       �   (               m             �	  �                   i   	   @       �   (      
         u      2       l
  	                �             u  (                  �   	   @       !  P               �      2       �  +                 �             �  �                  �             �  (                  �   	   @       `!  @               �             �                    �   	   @       �!                 �             �                     �             �                    �      0       �  &                 �              "                                   $  �     5         	                b                               �!  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ��ė�I�A�92=��nxsw(ğuz����*���r�?/W�
��c^|��B�����0�b�6[���Gv�{z�=���Q�5��(х����ݚ�J���?�8���߆��Vr:9x�Ss�?�N3�)�Cs���/#�����#��"U 4�F?�H��5�V�5��c���������A�6��B-\�+��J'�1�rx:�x���ꁙ!��O	l�s�C���c�	߹Ve��.��9��C��ֹQr7��'Đap�=[�m���Ӈ��unR�^ަ3��[�-��qŲ�=�Ll:0�B�wZiTX���\�!|*"򀛹�����*�o���A'�G~G��"���`g	������H��eQ|k
�m�UPA��!�"�p����+D(=Pi�0�:8:��ͮ����&Ebm�D>���ޮ����2�e�o��y��T>w,���l.V�u{/v��M�"�-ˠ�ԫ�&[G�Si@?���[Bc+BU�5���{����7��!PĦ�_D�-�_\�. =w��Jf`���
�Y?[+�G�E�C �, �͡�cW	pD4�Vt��*��n�. Q��m,s�8��J���`u�L��[&�h�Q�g�JE��J���!�W���x)�����6�����8kԱ�Ӿ����ObGG�y�P��A��߮��x��0G$���O �S���P4w\@n ���o�EQ�N1"z��|���'z��i���#yQ�ф�8��W�l������Xm��N��d�1�~���^��м��n }��L�
�8b*�t�Wg� X�6�n�����4yR���ޘ��E��NLkH�Ȩ��(��A����)�/W�^Q��ŵu�=6�"wK�j��a7�����oF#��-��2Vu}E|����u��,��hY/��%|�TMx���ٗ�q�:�=����{Rt�j�S��C���1��5��2���<L����p���K�S����� K���ݞR�#�'��k�Fϔ�F����HJ�/!�I��C;���sB�L_
����i]*ǜc���wm����1��wP�U����+4��ْ�/J���,|����� �[ ��#m���#�$ƾ�<�&��I'M�T��Y!$_�?���B�-AY^W��:�W�c��N&���`��Fݠ�RV��<�v��#t����&��@��C+��}:��=B�(�ò(f+Oނ��43��%��}c���G"���i�T4<v��G�֪�X� �
V;
ǅ�q"�^ũ�kQ�� $U��8��}�;��3�~,ƦwW��຿�1��t�p9��-���� �"3�*u�\��,B55V��QÀ��?6���F7�uk�0�Os,��ǴK ��g�[W�/T�5�K�(jPkK�i�%�)� J_�j������ě�1���HsS��?9g-��ّBT,Y����#��J�� xA��[��V/����zŖ�Uj��6oLc�Ӄ<[zq�v��s�k��9�k��g��C�a��Ql\G��*��"]Hw����xb��]��s8_|S5�����Z��O�!F�����JvG����,Wv���ҡ�h�e+ߙ�W����ɸ�	w?*�e�)\����K�d�J�No5H� �׼i�)��y�!��(��K �P�6�cU�3$31gJ�T�y�D�i�.�C���+ԁ�^��c�����8A�k[�[�L�\�&H�ĝ�6����4?�&6,����aZ���\��!�xp���S�Z��{�B��7&�؋���T��j�ym"G��ڼ�'�ɂBc߃Gi� �;����c�S({�z�,�@S�}Ee�j���]p���_�:b���ɩ�����8}��ղ���� Dz���C0��!�9f_����_���B2,�*en�̉6���c�e=������]DL���r��R��4��_"��7S�� ��+f��>�"�}���~I��i����r��4��UXs�g|C�I�O��(�_��XN~�j�UN+�"&u�<-����>�N�+������ܵ�H^�����'�EjC��8M��Z�,��m����m��P��"�**�CT��µ)d��_F����=�DwY�6����b�d��,�q��U���xC>�5��X[�����V΅7ф���F%�H�Sp���0�ڱ�q���E��������K�M-�OE�@2��w!Rϩ�R���	k9�QP(実�`��-��������`������qFI$e�?��d��Ō_���S�S��d��v�*���w���#re]�#u�hta{��vh����*��@�U�Fq-&��@���[g���gӋ�5��Y�]��b�$�+�A�JX-#t��,���u���e�XPA�*ݝ%P��n?��C�ү�fZ7��JTRے���<@
��
4KƼ��1H�g�3�����6w��w�ɹ1J�a������1�%��5��`J����B/pz3��0��
`H�����5yx�v�?O��'���:Bb��dL�����.��G� 9���i��B?�������¯���k�QG"�i�;Z1�TT2V����֥�֣d!;�b�υ�շ?���E�̳�M65�������mct���tgN{�MeZD�{UTT��^	�XNC�ɗA�ˆ�����L�����G�2K����fA�x�O��� �͹Ƣu�m�uvn/��H��p���խ>�T8dj�~U�#:Μ(�c��%��y8Gi���X���ߜo�iݕ1��H������B�'U\�70��{w�,����'������+I5�6�1�JU��d��./�z�At(���Q�Ω��%��"f���aǻc+Ӡ����pP�~ 
(S�:��7�����O2IR�_�N�N�yV8�}��AA�fߊQL�ޑ�ӂ�s`��le�9E�JZ�H�V"X���rk�o�V�:���VN��50��a����V�#��\�q@����9��ݓk��Pv���m��N^��̓7L~΄'���^��9h��� �#D��qy�I�OYq�1�(������	������P8��c��:?nPGN/�A5�!�u"����~�1��0��ǅ�1�:#�����<��d�_�� p8�V�F�k��/�� ��Yn�T��ҹ�Q1��Q��?��>�
��g�B�XG�\ڼ~X���1��y2�~lpQP�^�x� ����^�g�Zi�2\��Y�8l����G��+�q��u�%��P�Y��D�U�ۈ6o�I�"u�9xq���U
4����\t�7l'/��9)�a#Y;I�IC�V?�D�����g�uO{
����<��I�Lv%V/���B���(�����W�=��N�֏Z� 쩋���I�pH�Z"��G��3z��o �v��Ӡs��{8@�&?\����*�q��x�=v���A�����6��w�R�=kk�j�<@���!%�Hڷ����i�D��&"k�V��Ef8�XAHx�\�ǽ��#1Qk_8�dny��r3U�Խ���wbmf ����?&�Y7�ڽK&�dὫ|߭b^,�E���h���6k*x�ɭ��1�_%"]7�m����.�,lZ��C��o��8���/�ػ0V�Ъ��5���ѿ�+JE��Yi�7~��Oc��kr��`V'|K�3*hA<�}���L��sň��=錁�%��Z@���Bp��v8��R�|+v̽]���o��2�}y�ـX�0���-�.P�<Ǯ\0)	}��E�NG<Tl(���?�[��ڙǨT�=7T6$�y��n�B,��h�p����$a+E��Dd�R�I��:<`���]�k��>�-���_xw�����6�G�w��P>��I�<�t�~~Qh�ԧr��Vi/G��ϔ�z���T�	T����^{��`x�cP��{�G�Z��S�;��N���C;�bu��,T@E���3��N�X��k�(D0M�~�]i�-'NKݤ@�U>�|;d$����AB�H�i`�d��v~�DЃ��?   �t$�D$$    )Ћu 9؉�Nء u �T$�\$�\$�D$9����  ��&    �v �\$�D$1��T$�|$�ًl$������ E�EƉ�9���B  )ǋ5u �ى|$�=u Չt$Lt �|$ Pt �t � ʚ;�t ��1����� �Xt �l$E�E�5\t =`t 1�1�5\t ������ ��E�Eŉ=`t 9Ɖ$щT$��   1퍴&    �v ��t ��t +4$�8u |$���5\t �� �=`t ��t ��t ����  ������Å���  �5\t �=`t �$�T$��9��s��u �u 	l$$�D$� u �T$ �u �D$�T$�L$�u 1ۋu ������ ��t E�E��t �t �Ɖף�t ��t � ʚ;1�1������� E�E�9Ɖ������rQ��t ��t ���� ��&    f�)Ή$߃��T$���� 9��s�$�T$�5�t �=�t ��t ��t �\$� u 1��$u �t$�������� E�E�5u �|$ =u 0u ��1������� E�E�)��(u ,u �\$�D$1��T$�|$�ًl$������ E�E�9ǉ��s���\$�D$�|$�T$�l$9�����;����5 u �$u �����1�1�	���  �Tt ��+4u �=,u ��;=(u ��,u �����4u ؉Ɖ$1�)����D$����   �|$�l$� u �u ����'  ��tE�������։D$�D$��ЉD$�ȋ\$����D$Ӊ�����ЉD$���l$���Յ�~�t$����  �$u u )=\t -`t �Tt �@t �B���A  �Xt � ʚ;1�1ۋ=`t �5\t ������ E�EÉT$�$����  �$�T$��9��rq1�t& ��t ��t +4$�8u |$���5\t �� �=`t ��t ��t ���+  ������Å��4  �5\t �=`t �$�T$��9��s�	l$$�@u �\$$�@t �@t ���	����Du �?   ���@u �����]�����&    �v �8u     �I�����ǋ5�t �   ��t ���t ��t =�t )�������щ�W� ʚ;V�D$L������D$L�T$P�L$T�D$4�T$8�L$<QRP�@t �W�����t �5\t )أ�t ���t �t ��t ����t �=`t ������    �8u     �������&    f��ǋ5�t �   ��t ���t ��t =�t )�������щ�W� ʚ;V�D$L������D$L�T$P�L$T�D$@�T$D�L$HQRP�@t ������t �5\t )أ�t ���t �t ��t ����t �=`t �6����������Dу�!�b����������������    �S����ك� ���߃� ��� ����������D���!�����������0u �5u 1��5 u � u �Љ$u ���� E�E�+u u ������Tt ]��X�	����8u    �׉5\t ��t ��=`t ��t �������5Tt �z��)�����H�9�������=�    �����陾  �D$��������������UWVS��$�D$ �-�� ���k�4���� ���� ���� ���� ���� �T$���� �L$���� �T$���� �L$���� ���� �$�L$������+$T$��!�!�����$���|$�L$�Ћ<$�Ƹ    ��!�!�	���sD�EЋC�{ �$�K�D$��� 9��;����$�������L$��׋T$ 1������ E�E���tF�D$�T$�\$ ����S�\$ t$|$�s�{�u �u �C�S��$[^_]Ít& ��    �   ��&    �t& �������d�    �D$�    ��u�D$d+    u0��Ít& k�<�D$    �       �$���D$���������������v �����9Du t+S1ɉÉ¸P� ����������1�9Du [����Í�    1�Ít& �������@u ��u �Du �@D�@u 9�u�� Í�&    �v ��э�&    �t& ������U��WV�@u ��u#�Du �p�x�@u 9�u�����^_]Í�    ��΍t& ������     �@    �@    Í�    �����W�|$����w�U��WVS��   �Du d�    �E�1��E�����������������    ������@u ��t����Du ��������x����щ�|������������	��U  �E��U��5 t �=$t ��9����  �M���+(t 9��|����  )������SQ�щE�������E؋U�^_�E��E��E��U��UĉEȋu��]������  �}ȉ�p������ɚ;���  �}�%9ظ   ����  �\u ���u 5�u ��]������u��5�u 1��� E�EƉ�u �Ɖ�5`u � ʚ;=du 1�1ۉ5`u ������ �=du E�EÉ�9Ɖ����rX�E��U����� �E��U��t& ��E��U�)�߉E����U��� 9ΉE���؉U�s܋E��U��5`u �=du ��u ��u � ʚ;1�1ۋ5�u ��u �=�u ������ E�E�9Ɖ������rZ��u ��u ���� �E��U�f��E��U�)�߉E����U��� 9ΉE���؉U�s܋E��U��5�u �=�u ��u ��u ��u +E��   ��u ��u U�+�p�������SQ�щE�������E؋U܋M��E�QRP�Du �UЉM�������E��Mĺ}�%���]�9���   � ʚ;i� ʚ;��ʉ�1����u �u ��u �щ�u �E�������E���u �E���u �E�� v �E��������x�����|����,v     �0v     �Pu �Tu ��u �Du ��u �   �        �'�����t����    �@u ����������������E�d+    uY�e�[^_]�g�_Í�    �щE�������E��E��E��EċE��UĉEȋE��U��E��������&    f����������������������&    �t& ������UWVS��d�    �D$� t ������$t  t �  �(t ����  �    ������@u �������Pu �Tu �       �Du ������=,t  ��  �   �Du ������Du ��s �Hu ��s �Lu ��s �Pu ��s �Tu ��s �Xu ��s �\u ��s �`u ��s �du � t �hu �t �lu �t �pu �t �tu �t �Du ��������s @� �8t ��u �<t hu ��u lu �t ��s �t ��� �Z����|u �@� ��s @� ��s ��u ��s ��u ��s ��u ��s ��u ��s ��u ��s ��u ��s ��u � t ��u �t ��u �t ��u �t ��u �t ��s ������    �ڃ@u ���������������������D$d+    �S  ��1�[^_]Ít& �,t ������t& �\u �du �`u �5�u �=�u +5 t =$t ������ ��E�+(t �R��P�D$������D$�T$�l$�Ɖ�+5t �D$=t ��+t �T$�șR��Q���|$��������D$�|$$�T$ �|$���߉у� ���   ��H�Hʃ�9��    �}(�4$�|$�-t �5t �=t �]�����&    �v  t �t$$t 5(t ���щ���WV�D$������D$�T$� t �D$�$t �(t XZ�����������&    ��&    �����1�������uÍ�&    ������v �����1�       �������&    ������U��WVS������D$�T$�$�@u ���%  �hu �lu �D$�Du �T$������+Pu Tu �׋Hu �5Hu !¡Lu �T$�T$!��=Lu ������ֿ    ��!�!�	���Xu D�Eǋ|$�ȉ��%Xu ʋ\u `u du 1������� E�E�D$T$�Ɖѡ�u 9t<��|$��u ��u ��W�<$��u ��u ��W�}��u ��u ��W;5�u ���u },�@u 9�������e����[^_]Ív ��������&    f���u ��u �|$ 6eă����W�f������UWVS�Á�   d�    ��$�   ��� �  ��   ���  �� ��  ���   �   ���������  �KT�CP����  ��ց�    ��  �?B 9���q  ����  i��  �sH�{L�l$�D$��t$�|$������ƅ��e  �D$�T$�D$�D$�T$�D$D$u��t=d�    ���  ��t-���u'�D$�T$�L$��������&    ��l$����  1��|$ �   ��������    ������@u �$�=�u �D$ ��1�|$P�؍L$������ƋD$Z9�t7� ʚ;��u �   ���u �u ��u �Du ��u �   ����������ѣ�u ���Љω�u �����	�t�Љ�+�u �u ��u ��u �$�    �@u �����d�    ���  ��t� ��tu�@  u~�����   �������$�   d+    ��   �Ą   ��[^_]Ít& ��l$�������&    ��ɚ;9ƾ    ���   ��t׋sH�{L� ����t& �D$ ������@  t��   �5���	ŉ���v���1�������j����t& ��   �$�������tT���@�k����sX�{\��  �$���������9���J����v ������ �����    �   �������t��"������������������f�f������i    �� ��v WVS��v ��1��v �v �Ӊށ��  vd�غ�Mb�����i��  )Ɖ���    �ȹ�  ������v [�v +�v ��v �v ^�=�v ��v _��v Ít& �1�1�뮍�&    �v ������    ��� �    �����1�Ív �������� @��v     ���  $� ���  $� ������v ��v ��v     ��v     ��v ��v ��� ������� ����f��������v ��v U��]Í�&    �t& ������=�v ���������tÍ�&    ��� t�U��� ��V�5�� S�}�%9�   �|� ʚ;i� ʚ;���[^]Ív �����W��V��S����v d�    �D$1�����  ��   ���  ��u��� ��0��  1ۡ�� �  = $� �  ��� ��v ��v �5�v �=�v �$��� �T$���H������   ������ t����)Ɖ5�v �5�v �$T$�=�v ��v ��v ��t5���  ��   �������  ��v     ��1�����v ��v �D$d+    �  ����[^_Ív ���8  ��������� ��   ��� ����1���� �����v     ������t& ���� @���  $� �������&    �v �؃� �������� t�����؃� ���������    ���  ���  ��v �5�v ��v �1�����    ��v     �`������� ��� 1�1�	��F������  �t& ���  �)������ ��� 1�1�	������ɾ  ��&    ��� ����1���� �����v    ������v ��� ���   �� ��������L$����v    Q����Q �� ������L$��������)�Y��Q �� ��� 1ۉ�� �����v ���  ���  ��v �5�v ��v ������    �D$��Q ��v    P��������T$����)�ρƀQ �� �5�� ^�=�� �����������������������Í�&    ��&    ������WVS��d�    �D$1���� @u��v �t�D$d+    �`  ��[^_Ít& ���������$�\$�D$������ e��R�ʉ�P�D$������L$�D$�T$[^��?KL v]���� ���~N;wO������}�%9��   ���   i� ʚ;� ʚ;��� /hY�� j �щ¸`v j j ��������F����v �    ��tk    <����)�ډT$�T$1ɉD$�D$    ������Ã������������Ƹ}�%9�   �|Q� ʚ;i� ʚ;��׉�ω��� cC��ҙ   ���`������� /hY�� �O�����    ��.hY�   ��:���������������������&    ��&    ��������� @u��v �tÍ�    �    ��� �    �������&    �t& �����U��WVS�Ã�,��L$�|$@d�    �D$(1���� �$���ƀ�  ��v �D$���D$�� ��  �D$�T$��    �t$�C��v �S��v �D$�T$�=�v �$@  �   �T$Dǉ|$ ��I���D$����i�I���׉�������y�؃� �ډ����؃� ���ډס�� �s�{�C���C��� �C8   �C ���C$�$�C<    �C(��� �C@  ��C0���C4�    �CD    �CX�D$�C\    �8�C`    �Cd    ���   �Ch    �Cl    �Cp    �Cx    �C|    ǃ�       ǃ�       ǃ�       ǃ�       ǃ�       ǃ�       ǃ�       ǃ�       �E �U�D$�CH�SL�E�T$�CP���CT�D$��u�u��Mb��������)�SP���ST�U �E�5�� �-�� �T$9�����  �D$(d+    �C  ��,��[^_]Í�    ��v ��v �L$ ����   �$�5�v �=�v ��    �t$�L$�|$��  iL$ �  ��ʉ׉�����y�؃� �ډ����؃� ���ډ׋D$��t�s�{����f���Mb���C    �����S�����t& ��C��v �D$������D$�T$�GP�D$�WT�GX���G\�1�����&    ��D$ ��v �w �G��v �W�G�����G$�D$� �GD    �G0���G4�    �G@�����  �� t�$    �$��� ����  �$%    �D$��t^�  ��cis  ��D$� � �T$1�t$;T$��t$MT$��^��MD$�T$�� ���    MT$LƉ�v ��v ��t�C��� ��t�C ��� �� t%�C0�t$�P��Dº
   9�O�1҅�H£�� ���t�S0��� 9и    C4��  ����   �5�v ��v �t$ ��@t�SX�    ���@  ��  �L$ ��v �G�O��v �T$�O�L$�$�W�O(���O,�L$��    �GL    �W8���W<�OH�����t& ��|$ ��  �T$ ����  �$ �����G��T$�   ���   �D$���� �CH�SL�������&    �$$�����$�D$    ��� �C����t& �$�����T$�C��u�����9�Lº@B 9�O�i��  � �2�9Љ�M� e�9�O������+�v ��$���t$�T$��� ��x  ������$��v ��� ��v �Ѐ俣�� ���   �,  ��u��   �  �L$$��@�T$�L$ ��� �D$Q�L$����������D$�T$ X��� �H�   ���$9�O��   +�� ���1������� E�EƉT$�T$�$�D$�v �v �щ¸ � $L$1�9։�ξ    M�M���^���� ʺ    M�L���  ��v �T$�t$ ��v 1�������5�� ��T$$�t$��    ��v �t$Z��v �g����t& �C(����   ��   ��u&��v     �@   ��� ������� ����4$�$0�� �  	$��� ������&    �D$�o����D$������    ��������T$1�1�	и   D�������t���������v ��� ��v �$�C(������&    �v �$�E����G�T$�   ���   �D$�������CH�SL������&    �D$���>����v �D$$�D$    �D$    �D$ �����������%�� ������v �D$$��v �$    �D$    �D$    �D$ ���������f�f�f�f��������������v ��v Í�&    ������U�    W��VS���t$4�D$�ȉT$���1҉���������   ��1һ    �D$    ��T$�D$�D$0�$�D$�	�v �,$tV�$�D$1ۋT$������ E�E�D$T$1ۉ���t��1����Éȉ����ډ։�����1��� E�E��	�u��|$�$��D$���[^_]Ít& ��������	��J����������	�u��6���������UWV��S������(w ����   �š�� =�� ��   ��� �x�=�� ��   �dw ���t9���   ��t
�GD ��   ��tx�    �@w ������� ��   ��  ����t=��� �X�=�� tZ����t9dw t?�C,�@w �������u.�CD� u���#�  9dw t����������E�  [^_]Ív �C0�X�=�� u����Ѝ�&    �W0�zЁ��� �6������_�  ��&    �����h6�  j�j h�I �������= ���w�������&    f�Í�&    ��&    ������U��W�Ǹ w VS��������5w �É���$  ��� �$    �w     �BЁ��� �&  ���$��1�Ӎv �@Du9�u��u��t�Q89P8~���   �P0�BЁ��� uщ������   �w ����   9�t%�x� �B���x� tf��P`�`D�B���x� u�5�v ��u\��tX�x� =x� tLj 1ɺB �w j ������    Z�    Y�  �w �    ��t��иw �������v    ���ڸ w [^_]������t& ���� �BЁ��� t�4$���������_����t& ��5w ���M����v �$���������U��� W��VS�������� =�� t�X�1��   �t& ��������t�CD t#��    �s,)�hN�  H�P�7P�����ƃ��C0�X�=�� u��   �    )�H����� �����hR�  SW��������[^_]û   1��׍�&    �����S��� ��������dw �p,hT�  h   S������ø�� ������؃�[Ív �����U��WVS��(��v     ��v     ������   ������5`� ���Q  ��v     �    ��   �    ����   �    ���G  �    �    �����9�u���    ������ǉ�;    �2  ��=�v �    �   9�G����bf���Љ�9�vF���v ��9�tG������    1���B��    �����9�wѡ    ��u��    �    9�w���9�u���&    ��������v ��v ��������!  ������D$$h�v �5    P�u,hD ������    �D$�������D$�����D$����D$    �D$    ��    �$��v ������$9��p  �<$9|$$tߋU ���������D$���T$��6 j�|$������U ��������}�u�T$$��v �D$ ��v ��;D$\$Y����   �L$�\$ 9�Ӆ���   �L$�\$ +L$\$�ȉډ������!�!��Ήٿ    �\$����!�!�	�t$E�E�1��U���eʋM������ E�E׉��t$9�׉ߋ\$L��|$�|$L�9؉|$����ߋ    L���MD$�|$�D$�������&    �<$��=�v ��v �L$��v �\$ 9��}�uӅ��!����$���v �u�}����f��   �����d�    ����   �������v ���}�  ��v �����  ��([^_]ø   �����d�    ��tj������u,h�C �����[^��([^_]á    ��v ��������v ��v ������&    ��ۉ������t& �������f�����v �T��������돍�&    �t& ������`� ��u��������&    �t& �����V��S���5`w ��t
�`w     ��� �X�=�� u�\��&    �C0�X�=�� tH9�u��u��CD�t�CP����  �KT����  �`w ��t�@89C8~��C0�`w �X�=�� u���[^Í�&    ��    �����WVS�Ã�;w ��   9dw ��   ;`w ��   � w ������¡w 9�t�CDu,�s0�K4�C0�N�1�C0�C4� w �����1���[^_Ít& ��{`�sd�K`�w�>�K`�Kd��v ��t���t�x� =x� u��w �$������$��v     늍�    �   �����;w �;��������뇍v �   �~���9dw t�;`w �+�����&    �v �   �F�������������S�ø�� ������S0�C09�t�������ø�� �������[Ít& �1۸�� �������[Í�&    �������PD�H0��σ�@�PD�P09�t5�PX��t������(w ��uÍ�&    �    �h� �    ������v �@8    Í�&    ������U� w WVS��X������-�v ���{  ��v �D$L�x� �h�=x� �*  �l$@�1��&    �v �5(w ����  �D$@�@`�x��|$@=x� ��  �D$@�@D��@�$uʍ�&    �v ��w �������D$$�D$@�T$(�������D$D�w �T$H�������D$�w �T$�������D$,�T$0��-w �D$+D$$�T$�u�}��T$(��!����L$�щ�����!��D$�Ӊ\$��#D$�Ӊ\$4!�	��C  �$����  9    ��  �D$@�@D�$��Q  ��v ���C  �\$@�=w �Cp�w�|$�St��D$�D$$+D$�T$ �ŋT$(T$ !����l$�Չ�t$!�����Љ��|$��!�!�	W�D$DE�E�T$4�W�t$�{�sl�l$8�k�[h�T$�T$H�t$T)؉\$P���Ӊ�!Ɖ�����!�T$0�T$0�Љ�!���!ڋ\$@	T$D�D$$E�E��K�k�Sh�T$(�L$,�L$H�Cp��v �Kl�St��������L$8�D$�L$�d$��L$4������ t��1҉D$�����L$,�T$��1���|$������ E�E�1�D$�T$�W ��S �\$+\$�t$��t$���ك� ��H�H�9����  �D$@9dw ��  �$��!���J����w �@D�;����4$�|$@���� �GD�(w ������9=dw ��  ��   �GD�v �    �h� �    ������������    �U�E�M�T$1҉D$8�D$�T$<�e��L$������ t��1ҹ�� �D$9��T$��r����L$,�\$0+L$\$�ډȋ\$4!��|$!�!�!�	�u<�t$8�L$<�������1���L$������ E�E։ƸP�  ��9���'  �$�$;    �V����],������4$�t$�t$SPh�F ������D$X�>�����������|$L��t���v d�    �    �����;    sM�w ����   ��X� w [^_]������$�|$@�T$(���GD�D$$�Wt�T$H�Gp�D$D�Wl�Gh�����    �    ��t���w ��u��,�P\�ǅ������������GD�$�$��!���:���������w �  �¸w ������[�������������],�����WVSPh�E ������t$$�t$$�D$\�p,hPF �������$������|$@�_,�����SPhG ������w �p�p�t$4�t$4�t$D�t$D�t$<�t$<�p,h�G �������4�w�w�t$\�t$\�t$X�t$X�t$(�t$(�w,h�G ������dw ��(9��*�  �`�  �],������4$SPh�E ������������������dw UWVS��t�PD��@  ��@  �7  � w 1�������x� �Ƌ�J��x���x� u.��   ��t����   �AD�G`�W`����`��� ��   �ǋAD�@tЋY`�Ad��� �C��A4�Q`�Qd�Q0�B���� �A0   �A4"  �PЉ��A8    =�� u�"��&    �B0�ݍP�=�� t	�ËB8��y�E �Q0�   �P�A0�i4�U �AD�I�����&    f���v ��t�w ��t%�x� =x� t�� w �������[^_]Í�    �w �������v     �ы`� ������������������&    �v �������� ������l�����u��� �����1�Ív 1��������� �����1�Ít& �����UWV�Ƹ�� S�������� w ������V0�ŋF4�B���� �F0   �F4"  �^8=�� tj�PЉ���� ���&    f��B0�ύP�=�� t��;Z8~��V0�P�F0� w �~4��������1�����1��p���1��������� [^_]�������� 뼍t& �����VS�ø w ��������CD@u�S0�C09�t
�S`�C`9�t��������[� w ^������t& �x� �x� �B�S`�Cdx� �͍�&    ��&    �����S�á`w ��t+9�t;�PH��t�������u�`w ������� w �$w [�f��=�    �u�  [Ð� w [�$w �f������U��WVS�Ã��D$�Љʋ`w ���  9���   ��������� w �$w 9����r5�`w 1�1��D$�D$9D$t�PL��t�����������[^_]Í�    �=`w �|$)ȋ|$ڋ\$�w��K�[!�!�����$��L$���҉\$�$��!�!�	�u~�L$�d$�Ɖ�1������� E�E��؅��g����    +L$1��d$������ E�E����A�����&    f�� w �$w 9����r<��1�1�[����^_]Ít& �L$1��d$�Ɖ�1����փ� E�E�������t& �|$�|$�����v �������� =�� t'S�XЍv �SP��t��������C4�X�=�� u�[Ív Í�&    �������� S=�� t!�XЍv �ST��t��������C0�X�=�� u����v [Ít& ��������v Ív �����U��WVS������u�}�D$������T$������D$    �$��t
��1���D$�$����T$9��C�C��D$��)�1�������L$����� �ME�EӅ�t�]�3�{����[��^_]Í�&    �����U��WV��S�����  �@ ����  �F P���^�   1���$�х�t���Q����Љ�����k�d)��ʋ$�d   ����F������  �n�Q��&    f�����   1���^�n�щ$��t���Q����Љ�����k�d)��ʋ$�d   ����F�����r�9�r��F�V�$�F$�T$��P���t$�t$�����R�FP�V�v(�v$�t$ �t$ �v,h�I �������4[^_]Ð�@�׋V1ۉ$�T$����t��1����É�����1ۉ�����   �����ډӉщ�	�t/�|$ ��   �X  9º    ���   i�X  ��&    �v �F�^��$� ʚ;R1���P�T$��������F Y[���]�����u&��1Ҹ ʚ;���@ =? FF �G����t& �=? �7����0������r*�n�������&    ���1������4����v �ǉ��f����=�    u��v,hXI ��   �����X�N�n�^Z�y�����&    �����UW��V��S��������{<�  �C@�����  ������������� ������ w ������š�� �PЉ�=�� ��   �{8��� ���    �B0�΍P�=�� t��9z8}��S0�P�C0�C`�s4��SD�C`�Cd���}   �x� �x� �B�cD�S`�Cdx� �� w �����1��6���1������CD�t-�sP�����  �KT�����  �`w ��t�@89C8~�`w ��� �����1�[^_]Ív ��t��� �SD뒍v �C<    �������� �8��������UWVS�Y�������   �ƉȀ|�
tu�˃�rN��j�׃���
�L�)�L���)�߃����r�$���1��1�D5 ��9�r�$�� ��[^_]Ít& ��t���
��t��L�f�L��Ս�    ��t��f�������ō�&    f������V��S��$d�    �T$ 1ҋt$0���������Å�xD��� �������� �X�=�� t�C,���������t5�C0�X�=�� u帀� ������������D$ d+    u%��$��[^Ív �������ø�� �������D����������&    f������S��� ��������L$�غ@w ������Å�x1��1������ �������[�f�������U�    1҉�]Í�&    ��&    ������U��WV��t& �    ��u�5    �=   �    9�u߉���^_]Ít& ������U�   ��MbW��w V��� S�Ǎ��  ��������w ��  ��������w ��Љ�����)�)˙��1�����t��1���Ɖ�������1�  ʚ��;1�ƅ�t��1���ŉ����1����   ��w ��w �����1�[^_]�f�f�f�f������Í�&    �v �����WV��S�XD�Bt\������C �~�6��	�t9��������    ������;    r�{ u4������C��������	�uΉ�[^_Í�&    �������C�S뗍v 1ۉ�[^_Í�&    �������D$�T$��t�L$�����Ít& ��D$PR�����XZ�f������U�ŉ�WVS��<�D$P�T$8�T$T�D$,�    �    �T$0�D$4��Qh��  U��������8�D$���D$(    �,$��&    �t$(h��  �|$W�]����t$Vh��  W�M����vh��  W�?�����$�5    h�  W�+����vh(�  W�����v$�v h<�  W������(hV�  W������D$4F �D$��T$8V$1�1ۉT$������N�D$X��Z�|$�����   �t& �V�N�~�T$�V�L$ �N�T$�T$�L$�N$�L$'�����WVShf�  �\$S�~����|$;Whw�  S�m����� h��  �\$S�[����L$$�\$(�ȉ�+D$T$RP�t$$�|$(��+D$��T$ ��RPS1�QWVhDJ �t$,�����|$4��0�������w�D$���)������!��������������t9�r��|$�Ƌ�������T$������D$(�D$(�D$(���@����,$�t$4�v$�v h��  h��  U�����F1�R��Ph��  h��  U�x����F1҃�(RPh��  h��  U�^����F1�RPh��  h��  U�F����F1҃�(RPh��  h��  U�+����F1�RPh��  h��  U�����D$`��(�����1҉Ë@,RPh��  h��  U������s8�s4h��  h��  U������C01҃�(��R��Ph��  h��  U�����CD1�RPh�  h��  U�����CH1҃�(RPh�  h��  U�����CL1�RPh$�  h��  U�q�����(�sT�sPh0�  h��  U�X����s\�sXh?�  h��  U�B�����(�sd�s`hM�  h��  U�)����sl�shh[�  h��  U������(�st�sphj�  h��  U������Cx1�RPh{�  h��  U�������(���   ���   h��  h��  U��������   ���   h��  h��  U�����    1҃�(RPh��  U�����l$`�D$d��  ��L[^_]�y�����&    f������WV��S�|$��Qh��  P�T���������  Wh��  S�=�����h�  S�/���XZ���u  �v`h�  S�����v�vh"�  S�����v �vh9�  S�������,�v$hP�  S������v(he�  S������v,hz�  S�������$�v�vh��  S�����vh��  S�����FD����tPh��  S�������F8��tPh��  S�z������F<��tPh��  S�d������F@��tPh�  S�N������FH��tPh�  S�8������6h0�  S�(���h��  S�����v4hF�  S������ ��yh��  S�����XZ[^_Í�&    ����������  ��t�P`Rh\�  S��������čt& h��  S����YX�|�����    h�  S����Y[[^_Í�&    ��&    ������S��������Hj�������������P�5    ho�  S�^��������P�5    hxJ S�G�����$h��  S�9���XZ[Ít& ������WVS�2���B���t;��t��������HV�������X1�[^_Ít& �r���r������1�ZY[^_Ð��t���E���1�[^_Í�&    �r�zh��  S����jh��  S����WVh��  S������$h��  S����1�[^[^_Í�&    �����S�XD�    ��Q �������;    r�{ u������C��[�1���f������WVS�����h��  �Ɖ�j �#���jh��  j ����WVh��  j ������$h��  j �����[�����X���&    W��1�V����ZY�غ    �������;    r�1�������)������&    ��������HS�1��,���X�غ    �������;    r�[^_Ð�����UW��VS��(d�    �L$$1ɍt$�\$<��Q V�����Y|$�Ɖ���  �v �����Q x��Ⱥų���   �����Si�  )Ѻ�����������Sk�<)Ѻ�$I���N����������)�    )ЉʍL$)BHЉ��S�����b ��   ��D$�T$ Qj h�: ��������֋D$(�T$,k�d���a�, �D� 1���l$�ŉT$�׺��O[����d   ���T$��D$�D$�T$�T$t$`�A^��1����D$�T$������   ���D$iD$]  1� ����������x�����t$4$i�qz  ��������x�vm���������l  ��L$�S�s�C�K�D$$d+    uI��([^_]Í�&    ��Q ����Q ��e�����&    �v ���D$�Y����v �L$��;������������V��S�É���������C�D$�S�T$�C1��S�N1��C    �� �C     ��������������C�S[^Ít& ������U��WVS�Ã����� �������3�{�$+C�T$S��#F#N�f�NʋKCS !�!��{ �|$�K�N1��{������ E�E֋4$CS�C�s�S�e�[^_]Í�&    �v �����U��WVS�ÉЉʃ����0��s�{�$�t$ �q�|$$�y�t$(�t$ �|$,�|$$)��T$�ƋD$(��{#T$,!��s�|$�t$�s �[�t$�\$�Y�I�މL$�L$(�\$,����9��s`�D$(�T$ +$�L$$L$!ЋT$,!��L$�������+D$T$1������� E�E։ƋD$�׋T$)���e�[^_]Í�&    ���L$�����D$T$1������ E�E�D$T$�e�[^_]�f�f�f�f�f������UWVS�    ��t)�։ύ�&    f��C�+���t$���������X��u�[1�^_]Í�&    ��    �����UWVS�    �t$�|$��t�Սv �C���W��V������XZ��u�[1�^_]������UWVS�    �t$�|$��t�Սv �C���W��V������XZ��u�[1�^_]������UWVS�    �t$�|$��t�Սv �C���W��V������XZ��u�[1�^_]������S� x ������x �¸ x �������[Í�&    �v �����U��WV�p�x�@@�@�Ōx �����)�׉�^��_]�f������U��S�ˉыP`�@\)�ڋ]��Ít& ������Í�&    �U��W��V��S�Ã��S@d�    �E�1��U�������U��    �CD�U��to�M�U�   �������tC�M�U�J0�`   f�J4�M�j<�B8    �H�M�p�xj �u�R�   j jS��������E�d+    u�e�[^_]Ít& ���u����������&    �t& U��W��V��S���^@d�    �E�1�������    �FD�]����   �M�U�$   �������tX�M�U�J0�`   �j<f�J4�B8    �x�O@�H�O�_�H�M�X�]�H�M�Xj �u�R�$   j jV��������E�d+    u�e�[^_]Ív ����s������������&    W��V��S�Ã�d�    �D$�C$��  u@�   �ډ��������t�T$,�p�x�P��������D$d+    u��[^_Í�    ��u����������t����������&    f�V��S�Ӄ�d�    �D$�F$��  uS�    ����������t*�X�S@�P�K�[�H�L$(�X�\$,�H�X��������D$d+    u��[^Ít& ��u����������t����������&    f�U��WVS�Ë�x   �������t[^_]Ív �M�u���  �}h �  �����C@�����WVPh�J S�������`  ��[^_]�����U��WVS�Ã���x   �������t��[^_]Ít& ��M�U�u�}�L$�M�$���  h�  �����C@�����WV�t$�t$P�uh�J S�������`  ��,[^_]�����W1�V�t$WV�����XZ^_Í�&    �t& �1��������&    f������W��1�V�֍RS�Քx �Í@������{<�s@��C0 l �CD    [^_Ít& �����W�|$����w�U��WV��S���E�ЉʉE؉���ӉU܋W�߉E�F�U�V��)�ׅ�x?��;u�}��    �   }A�u��u�������]�C�SXZ�e����[^_]�g�_Ív �e�1�1�[����^_]�g�_Ít& �u��xQ�u�ȉ��u�������Ɖ׋E�U����֍�E���ڋ]�CS�C�SY�M�[�]�9��|����� �g�����    �����U��W��V��S�Ë@@�@�Ōx �����W�щ�V�����Y[�e�[^_]Í�    �����U��P��WV�p�x�QR��������Y^�e�^_]Í�&    f�������PH�@H    ��t�������1�Ív U�   ��]�����f������W�|$����w�U��WVS�Ã�,d�    �E�   ������щE�������E�d�    ��E̋3�C�EЋ{�C���  �Rr$z(B,���R��P�E�������E؋U܉�E��S�CXZ�E�d+    u�e�[^_]�g�_��������&    �U1���]������t& ������UWVS�Ã�8�x d�    �D$41�����   ���  ���   �C$������@xtp���   ��tf�|$1��   �\$�D$�D$��  �D$�����������= ���w�   �F������ x ������¡x ��t-� x �������������D$4d+    uf��8��[^_]Ít& ����  �$������$��t��؉x 1�1�������$먍t& �� x ��������f������띍�&    f�������������t& �����UWVS�Ã��x@������x �T$��������$�C������ƅ�yw�$���������Ōx �����>�t& ����[^_]Í�&    d�    �    s�d�    �    ��t�IR��P�������XZd�    u������뮍�&    �CDt��T$���x ��������cD��i��������S�����    ���������x�[Ð�������P�����v �����UWVS�Ã����s,�$�v�����x �D$����������C0t�D$�$�x ������c0����������k(����   �v�Ōx ������щ$������ŉ����������tA�C��S��l$�C�S�ńx �C�S�C0��   �$��L$�   ������K0�L$����������v�Ōx �����>�t& ����[^_]Í�&    d�    �    s�d�    �    ��t�IRP�T$�������XZd�    u������묍t& �����������|���f��$��L$������L$�H�����&    f������UWVS�Ë@@�4@�����x �Ƅx ��������S�Kj �ǍCj �S j �K$�S(�K,��������CDu�ډ�������KD��[��^_]������t& �ډ�������׍t& ������UW��V��S�Ã��h@�Tm �����x �T$�$������T$�s�D$�{���x �CD��   �ډ��s������KD�C�Sj j j �щ�������T$�D$������Dm �Ōx �������>�t& ��[^_]�d�    �    s�d�    �    ��t�IR��P�������XZd�    u������뷍�&    �ډ�������V�����&    �v �����W��V��S�Ë@@�@�Ōx �����R��P�������Y^�щ�[^_������t& ������UW��V��S�|$ �Ël$u&���   �@�Ōx �����R��P��������Ɖ�XZ���t�s\�{`[^_]Í�&    �CP��[��^_]�X�����&    ������W�|$����w�U��WVS��d�5    ��<�U��?�M�d�    �E�1��sH��&    ��   �F�U��Mĉ�������CH��t�������t& ����������x�CH��t��   u��t��t& �d�    �@    �[H����   �    ����   d�5    ��D  ����   ��Ōx ������M��]�)��1�9�ظ    |�U�d+    �   �e�[^_]�g�_ÉʍEȉ�������EȍU؉E؋ẺE܋EЉE���8  ������1����������j�������   ��t9�=�    �P���Wh��  ��   �����ZY�5�����&    ������_�����x �   �@������M��]�)��Hx ӉM��]�������M��Px �áTx 9U��|	�u(�E��U��5`x �Px �E��Tx �UģXx �\x �ڸHx �����������x �   �{����������&    �v �����S� x ������x �¸ x �������t� x �������1�[Í�&    �v �����U�hPWVS�Ëp<�x@���   �@�Ōx �����W�щ�V�����C$XS(�C\�K`Z��[��^_]������&    ��&    ������V� x S��������5x �¸ x �������t�    �    �C    �C1�[^ø��������    �����WV��S1ۃ�t1ۃ�	�Í\�� x ������=x �¸ x �������t�[�Őx �������1�[^_ø�������t& ������U��VS1ۃ�t1ۃ�	�Í\�� x ������5x �¸ x �������t�[[^�Ōx ]�����[����������^]Ív �����WVS�ø x ������5x �¸ x �������tu�#   �������t`�C1���t��	�����t ��v�Cd1ɋՔx �{P������{P1�ǃ�    l ǃ�   �x ���   ǃ�       [^_Ít& ������������鍴&    �v �����U��W��V��S1ۃ�X�D$d�    �D$T1���t1ۃ�	�Í\�d�    �$� x ������=x �¸ x ���������  �������B  �#   ��������@  �[1ɋŔx �D$������N�}�%�D$�D$8 l �D$��D$D i �v9º   �\$H��D$L    ��   � ʚ;i� ʚ;1�����׃�ty�[�Ōx �����WV�����S�׉Ɖ��D$�Y�����=����t�T$Td+    ��   ��X[^_]Í�    �$��@  ��L  ��P  ǁ<  �]  ����&    d�    ���  �@=    uRS����D$�����Z�����=����D�녍t& ����������;����������g�����    ������W�����    ������P�D$�����Y�Ɖ�떸�����/����������&    f������U��x WVS��H�D$d�    �D$D�Hx ������Px �Tx �Px     �Tx     �¡`x �$�Xx �\$�\x �D$�Hx �L$�\$������ x ������=x �¸ x �|$������D$    ����  ��������]���������tD�s�{�E�����)Ƌ$׋T$��	�t9Ɖ��}�C�S�4$�|$�D$�D$�T$�D$���|$��   �l$�<$��	��  ���5w9��    ��  >�t& �t$� x ����������T$ ������t$@�t$@�t$@�t$@�t$@�t$@�t$@�t$@�t$@�������$j j D$T$R� x P��������Ã�����   �D$Dd+    ��   ��H��[^_]Ív �D$   ������v d�    �    �Q���d�    �    ��t�@�t$�T$�L$�����Xd�    ���������������&    f�1��s�����&    f��D$1ɺ�  ������U����D$1ɺ�  �����������;����������&    �t& ������U�h�W�x�VS�É���������C�    �ƋC�C�tD�C�P�S��1��������t�S��C�щ$	��T$u#������������[^_]Í�&    �1����t& �C@�   �@�Ōx ������t$�t$�щ�������C�   C�XS�Z�C��f�f�f�f��������    �    �B    �B1�Ív �����U��S�ˉыP`�@\)�ڋ]��Ít& ������Í�&    ������S�Ë@�C<    �C@    �P4���������x�C     1�[Í�    �   [Ð������P1Ƀ�P�����1�Í�&    �t& �����U��WS�XP�p@�p<�������Y[�e�[_]Í�&    ��    �������P������v �����U�պ}�%WVS�Ë�y�q9º   �|+� ʚ;i� ʚ;1������S���у������Z[^_]Ív ����������ލt& �����U�   ��]�������&    ��    �����U�   ��]�������&    ��    �����U1���]����������U��S�Ӄ���� d�    �D$�   ������щ�������$��D$�C�D$�C�D$d+    u�]�1���������t& �����U1ɉ�S�Ӻ@B ����� d�    �D$1���������$��D$�C�D$�C�D$d+    u�]�1����������&    f������U��WVS�Ã����0�K@�s�T$�S<d�    �D$,1��C8�D$��	�uw�C ��u�|$uG�C�V������$�T$�L$�v0�؋$�������1ҍ|$9����   �|$t�D$�@   �D$,d+    ��   �e�[^_]Í�&    f��D$��������D$�L$��D$�A�D$�A�C�V������$�T$�C4u�|$u�$�L$�؋~,�����C$S(�K���f��L$�v0�؋$�������1�9���T������������D$�|$�G�D$�G�D$�G�7���������t& �����UWV��S�Ã��h�$�T$ ��t������C<    ���C@    �U4������¸   ����   �C4�C     �C,    ���C0    ����C4�FF��   ��}�%�N�~9и   ���   � ʚ;i� ʚ;�|$���D$    �D$T$�S@�}�%�C<�F�N�v9º   ���   � ʚ;i� ʚ;1�������<$��uR�s81Ƀ���Q�щ�W�}8�����X1�����Z�C 1���[^_]Í�&    ��F���<�����[^_]Ít& �d�    ���  �I��    t����sQ�щ������Y끍v ����������/�����&    f�����������O����������������1�Ð�����U��]�����f������WVS�Ӄ�d�    �D$�������d�    ���  �@�p�x3�@ {C���R��P�D$������D$�T$��D$�S�CXZ�D$d+    u	��1�[^_��������������������t& ������������1�Ð�������1������f��������d������ �������&    �t& �����S�Å�u$�CH������CL��������   �� [������v ��x ������K�C���t�H�C"  ��x ������f������U��W��VS�Ã��S�t$ �D$$��D$�CP���$���D$��u��`�  �����  D��C�$�L$��������Cl�� ��u�Cp�@�����R��P��������ǉ�XZ�|$ �{\�k`�{d�kht��[^_]Ít& �j ����j j �D$���������[^_]�f������WVS1ۅ�xw�ǉ������d�    ���  ��1�i�G��a�����x ��u�A�v �C��t7����t0;PDu�;xu�x����������d�    ���  9CDt������������1ۉ�[^_Ít& ��������[^_Ít& ������UWV��S�Ëh�x�������C������U<��t��������������[��^_]������    ��t& �����V��S��d�    �T$���������tW��1�1������r��C��t6�H$��t/���1�������$�C������D$d+    u����[^Ít& ������Ӿ�������������&    �t& �����U��W��VS�Ã�d�    �D$1��L$ ��t1҉����r��؍T$�$�?����$����u4�d�t& �p��t.Q������������X��u!�؍T$����1ɉÅ�t2�C��u�������T$�C������D$d+    u����[^_]Ð�������������t& �����W�|$����w�U��WVS�Ӄ�,d�    �E�   ������щE�������E�d�    ��E̋3�C�EЋ{�C���  �Rr$z(B,���R��P�E�������E؋U܉�E��S�CXZ�E�d+    u�e�1�[^_]�g�_��������    �����U�hPWVS�Ëp<�x@�@p�@�����W�щ�V�����C$�CdS(�Shj �K\�[`)��SQ�щ��������[^_]Í�&    �����U�պ}�%WVS�Ë�y�q9º   �|[� ʚ;i� ʚ;1��������uS��������Z[^_]�d�    ���  �I��    t׃�Q�щ������Y�ō�    ���������뮍t& �����UW��VS��$�D$�T$�L$d�    �D$ 1�����  �|$����  ���!����  �D$�D$����  �P���  ��� ��  ������Ņ��  ������EL����  �@    �P��1��@    ���)J 1҃�����9�r��E    d�    ���  �l$�CT�$�E�D$f���x ������sT��1�i�G��a���<��x �,��x ���  ����t& �@����  ����  ;XDu�P9�u��������    HЉST9$t��x ��������t��l$��v �l$��x �����  ������t$���  �D$�|$�u�E$�����E�D$�E(�����E����  �����d�5    ���  ��  �G���(  ��   ���  ���'  ���  �   �����_  �P	��!  �]H������EH����  �|$�G�W�E8�EL�P�EL��P �EL�U�   �P�EL�T$�@�����D$���������  �D$�P��������   �ƅ���   d�    ���  ��������  �ED���  �PX�j�U �PX�U�hX���  ������   1ۃ��:����D$�@�����1҉��������t$���  9��  u�D$�@�$����?�����1��������D$�x�h�D$���x ��t�D$�G�sT�����������������1��D$�����������D$ d+    �  ��$��[^_]ËEL�E8    �@   �EL�@     �EL�U�P d�    ���  ��  ��t�   ������   �P	�xu�]H�i������    �����    DD$�U����   ������W���������T������ ������������>����   ������(����   �������������   ��������z����   ��������i����   �������������������������������1�������������    �����WVS�Ӄ�d�    �D$�������d�    ���  �@�p�x3�@ {C���R��P�D$������D$�T$��D$�S�CXZ�D$d+    u	��1�[^_������������WVS�Ӄ�d�    �D$�������d�    ���  �@�p�x3�@ {C���R��P�D$������D$�T$��D$�S�CXZ�D$d+    u	��1�[^_������������UW�x�VS�É���������S�S��C�    ����   �C���C�S�1ɉB$�C��S��C���1��������~`�C�C�tX�C �@������?B �$�C�T$�S�9��|�$@B �T$ R�   P�T$�؋L$�����C�XS�Z�C��C�   ������������[^_]Í�&    1��_�����&    f�V��   ��S�Á��   d�    ��$�   1����������um��xq����   ���!؋��  ��tv�H��tv�؉�������Å�y ��$�   d+    u^���   ��[^Ít& ���   ����������t΍�    ���������غ    �����    E�돍�&    f������뙻������������&    ��    W��V��S���   d�    ��$�   ��������Å�t)��$�   d+    ��   ���   ��[^_Í�&    f���xL��g���!����  ��tU�H��tX���������Å�x����������������E�둍�&    �v ��    �����    E�밍�&    f�������]���������S����������&    �����UWVS�Ã�d�    �D$�C��������t�ŋ@@E<u �$�E������D$d+    ua��[^_]ËC9E4u؋E�P(��������E$�U(�E    �E4�E,�U0�E$�����E(�����s����Ƹ����1�9��L��s������f�������HL�Q$1ɋPH�@8�@L�������������Í�&    ������VS��D��pd�    �T$@�P��t?�@   ��������¸������t�T$@d+    u8��D[^Ít& ����������ۍv �D$@d+    u��D���1�[^�c����������&    ��&    �����S���X� d�    �T$1҉��>�����t�T$d+    u!��[Í�&    ��ډ��������tٸ�������������&    �����S���X� d�    �T$1҉��������t�T$d+    u!��[Í�&    ��ډ��������tٸ�������������&    �����S��� d�    �T$��������t3�P,����1�9�X0Mʋ$����������D$d+    u����[û����������������UWVS��8�X�hd�    �T$41ҋ8�P�t$1���EƉ$��tZ�D$�������uD�|$�ɚ;�D$w?D$x9�|$�ɚ;w/�4$����L$�_���Z��u��t�ډ��������t������f�������T$4d+    u��8[^_]������������UWVS��8�X�hd�    �T$41ҋ8�P�t$1���EƉ$��tZ�D$�������uD�|$�ɚ;�D$w?D$x9�|$�ɚ;w/�4$����L$����Z��u��t�ډ��������t������f�������T$4d+    u��8[^_]������������VS��� d�    �T$��������Å���   �C���}   �P ��tv���������trd�5    ���  �������C�B��   �C"  ���  ������CD    �$�C������غ   �8���1��T$d+    u0��[^Ív 뒍t& �؉�������Å��]����t& ��������������t& �����U�hXW��VS�@X9�tf�v �_X�s��P ��t!���������u���������������C��u���C�B����   �C"  ������غ   �|����GX9�u�[^_]������VS���d�    �T$1ҋP��xP��wk���!؋4��  ��tY�F��tR���������uW�N���������T$d+    uE��[^Í�&    ��ؾ    �����    E�묍�&    f������뾍�&    f�������������t& �����S���X� d�    �T$1҅�xY��wl���!��  ��tZ�J���������t�T$d+    uF��[Í�&    �v �ډ��������t׸�����Ѝt& �¹    �����    E�룐������������t& �����S�Ӆ�x,��?���!��  ��t-�J��t-��[�������    �¹    �����    E��А�����[ø����[�f�������P� �1���������S���X� d�    �T$1҅�xY��wl���!��  ��tZ�
���������u��u�T$d+    uC��[Í�&    �ډ��������tڸ�����Ӎt& �¹    �����    E�룐������������t& �����VS���d�    �T$1ҋP��xP��wk���!؋4��  ��tY�F��tR���������uW�N���������T$d+    uE��[^Í�&    ��ؾ    �����    E�묍�&    f������뾍�&    f�������������t& �����S���X� d�    �T$1҅�xY��wl���!��  ��tZ�J���������t�T$d+    uF��[Í�&    �v �ډ��������t׸�����Ѝt& �¹    �����    E�룐������������t& ������P� ����������S���X� d�    �T$1҅�xY��wl���!��  ��tZ�
���������u��u�T$d+    uC��[Í�&    �ډ��������tڸ�����Ӎt& �¹    �����    E�룐������������t& �����UWVS����pd�    �T$1ҋx�P����   ��wo���!؋,��  ��t]�M����   �����������   �D$��x;�|$�ɚ;w1��   ui1҅���d�    �ቐD  ����H  �؋u������������T$d+    uL��[^_]Í�    �ؽ    �����    E��e�����    1�1�똍�&    �v �����뮸������������&    ��    �����UWVS����pd�    �T$1ҋx�P����   ��ww���!؋,��  ��te�M����   �����������   �D$��xC�|$�ɚ;w9��tu��   um�   d�    �ቐD  ����H  �؋u��������    ������T$d+    uD��[^_]Í�    �ؽ    �����    E��]�����    1�1��f������붸����������f�f�f�f�f�f�������U��WVS�ÉЉʃ���� �K<�s\�{`�L$�K@9��|$�L$��L$�t$�|$�t$��   �L$��|$��   �|$ȉ��+D$T$�D$)ȉT$�9���ѹ    s4�\$��&    ��D$���T$�����)��9��r�\$��xs�v �D$�T$�D$�D$�T$�T$9����rE�D$�T$���D$�T$�D$�T$�C\�   �S`1������� t��1�C$S()t$|$�����s��D$�T$�e�[^_]Í�&    ������UWVS�Ӄ��$��R��	�tD�|$9D$���rF�k�{��	�t�Չ;�k>�t& �$�D$ �   ������S���	�u��[^_]Í�&    �;��^s��V��[^_]Í�&    �v �$���  ��  d�    �    s�d�    �    ��t�T$ �@�t$�t$�������XZd�    �`���������V���f������WVS�X�ك�������
$  u���  ��
�   �O�p\�X`�PP�Hd���������t;7��Gs�7�_[^_Í�&    ��������W��V����S����tP�Ѕ�u d�    ��tN���  ��[^_Í�&    �v ��������ǅ�t��u?��td�    ;��  t�G��u�1���[^_Í�    ���  [^��  ��_Í�    1��������t�d�    ���  9��  t�1�븍�&    �t& ������V��S�������1҉��'�����t7��������    �F    ��t�F@B 1�[^Ít& ��F   1�[^������������������V��S������F1��������tR�ÍFP�F    �FP�   ����t(�P	�x�^h�����1�[^Ít& �   ���������f��   ��������������������ύ�    ������@�����o�����&    ��&    �������@�����O�����&    ��&    ������U��W��VS���T$�D$    �D$�t$�_���D��$�D$����   �C�t$�S9$�rR�   �C�C$   ��t��������C    �E�S�U�k�C ��_��tA���S�C��t�L$9$�s��G���D$�D$�W����a�����[^_]Ít& ������������̍�&    �t& ������S�������1҉��*�����t����������[�����������[Í�&    ��&    �����VS�Ã�d�    �D$1������1ҋCh�C���������tc������������   �Ct��t6���  �$�����������   �T$d+    ��   ��[^Í�&    �Cd��u)���  �$�����������Ch�����1�뼍�&    �v �SP������S�Cd    �Ѓ���uT���  ���K\���   �S`33P	�u��     �$�@    ���  ����������덍v �Sd��t)�w���f�����$  ��P3K\3S`	��J���묐�CP9CPu��H����������&    ��&    �����S�������d�    ���  ��t������    1��C    �C   [�����������[Í�&    �v �����S�������d�    ���  ��  ��t������    1��C    �C   [�����������[Ít& �����U��WVS�����`�D$$�Ћ��  d�    �T$\1҉|$���   ����   ��uh�T$(������D$0�T$4���ӉD$H�D$8�T$L�T$<L$(\$,�D$P�L$@�\$D�T$T�D$$�T�D�D�@�L$\d+    ��  �e�[^_]Í�    ���   ���   �\$���   �\$���   �\$����   �L$������Ɖ׋L$������D$�L$�T$������L$�\$�t$H�|$L�D$P��L$@�\$D�T$T�Y����t& �\$���   ������Ɖ׍��   ������D$���   �T$렍�&    �v �T$(������D$0�|$�D$ �D$4�D$���ȋ\$ �L$��1�3D$	�t�������;D$ �D$����D$��r͋D$(�|$�D$ �D$,�D$���&    �ȋ\$ �L$��1�3D$	�t�������;D$ �D$����D$��r͋D$8�|$�D$ �D$<�D$���&    �ȋ\$ �L$��1�3D$	�t�������;D$ �D$����D$��r͋D$ǀ�      �n���������t& ������U��W��V��S�Ӄ����0d�    �T$,1҉D$�|$������D$�   ���N�����1҅������������   ��t1�|$��   ���  ���  �D$��tb��t1�1���t& ��D$1�������Ɖ����������D$������D$��D$�C�D$�C1��T$,d+    u3�e�[^_]Ít& ��  ��  �f�������Ɖ��������������������t& �������������������������������������WVS�Ã��{d�    �D$1������1ҋCh�C���������th����������t[�����Ct"��t}���  ���  ��t]��t1�1���   ��������щ��I�������p������  �$�����������D$d+    u-��[^_Í�&    f���  ��  �f���������щ���������&    ��&    �����S�Ë@8�����t]�CL��tN�C@C<u1҉�������C\    �C`    [Ít& ��C4�P�؉S4�������t�������C4[Í�&    �CH������C\    �C`    [������U��WV��S�����`�E�T$�L$ �D$8d�    �D$\�F���������1ҋFh�F��������D$<����  �\$ �}�%�C�K�[9º   ���  � ʚ;i� ʚ;��ʉ�1��ډD$�T$�D$0�T$4�D$<�T$X��������-  �F<�V@�$�F\�T$�D$�F`�D$�Ft�D$$���C  �Fd��t�VP������Fd    �F��  �T$<�   ��������D$(�T$,�D$8��t#�|$|$�  �@    �@    �@    �T$$���  �T$4�D$0��	���  �D$u�L$(�\$,�ډD$0�T$4�D$�T$�D$�T$�L$0�\$4�F\�D$(�V`�T$,9ȉ��s�T$<��������D$<�T$X���  ������\$ �}�%��K�[9º   ��S  � ʚ;i� ʚ;��ʉ�1��ڋL$(�\$,�F<�F4�V@�T$4���F,    ���F0    ����F$�����F4�D$0�F(����9��r9��	��d  �D$<�T$X�������t�Fd����  �D$<�T$X���  �����������|$8��t$�$�L$�D$@������D$@��D$D�G�D$H�G�D$\d+    ��  �D$$�e�[^_]Í�&    ���D$<��  ���  ���  �T$,�D$(����   �������D$(    �D$,    �������&    �F\    �F`    �����t& ��L$,�T$(�������L$(�\$,9����r!�D$8�@   �@    �@    ������v )�ډщD$@������D$@�|$8�G�D$D�G�D$H�G������&    f�������������t& �D$<���  ���  L$(\$,�8����v ������D$$����������    �D$0�����D$4����D$�����D$��������v �Ft�����D$$   �����t& ���������������&    f��V�t$<�Ѓ�����u<���  ���   �     �@    ������t& �������D$(�T$,�{�����    ��$  �������f������UWV�ι)   S�Á��   �$�l$ ��d�    ��$�   1���\$8�D$D�����D$H����d�=    �����|$h�Å�t"��$�   d+    ��  ���   ��[^_]Ð��D$    �D$    �D$�F�D$    �D$�F�D$�D$0�������j �T$�L$����Y�Å��  �������   uE���T$|��$�   ��u2	���   �F   �D$0�����������D$0����������   t��D$|��$�   �� �$�D$�T$1�P������Z�ƅ�uz��������D$0������L$��L$��D$	�	�������<$�l$d�    ��L  ��8  ��P  ��D  ��tS�T$��������������t& �D$0��������x����ƍD$0�������t�농�    ���Y����D$0������i���������_����������&    �t& ������UW��V��S��d�-    ���t!���$���tX1�1ɉ�������$��9�tB���������=����t��[^_]Í�&    ���u"��@  ǅ<  � ��[^_]Ív ������ɸ������f������U��S����� �Xd�    �T$1ҋP�@�щ�������$�   �L$�D$�D$�D$�D$�D$���	����T$d+    u�]��������f������V�����S��d�5    �����=����t[^Ít& ��uǆ@  ����ǆ<  � [^ø�����֍t& ������U��WV��S�Ã����0�S`d�    �D$,�C�T$�D$�C\�D$�����1ҋCh�C�����������   �K@�S<�ǍD$������D$�L$��D$�F�D$�F�D$	�tm�D$���D$�Ct|����   ���  ���  ���  ���  �|$����   ��t[�T$�L$�D$������D$�F�D$�F�D$�Ff��D$,d+    �|   �e�[^_]������v �D$1ɉ���������ӋD$�T$��9��r/�F   �F    �F    먍�&    ����ˍ�&    �v )L$\$�[����v �������������������&    ��&    �����W�|$����w�U��WV�U�Sd�    ��Ld�    �E�1��EȉEȉẺ����������  �    �CH����  ��$  ��(  #�4  #�8  #�D  #�H  !Ѓ����   ���  ���  �Mȉ�����  �u���  �EЉUԋ��   ���   �}܉E���$  �U�U��`������  ��\  ���tJi�H  �  ��`  9��  ����  �    ���G�  �   �	   d�    ��������  ���   ���7  ���   ���)  Ǉ�      ���   ���   ������E����   �U�������E����   �U�������E��E��U��U�U��M��E؋E�M��U��UЋU��E��EЉU܋U��M��MԍMȉU���g����񍗐   ��j�u��u�� ����؍��   ���   j�u��u��������  �����  ����M  ���  ���M�� ʚ;��E��ȋM��U��e�9E���r  �}���h  �    ����  �   �	   d�    ������t& ǃd      ���  �U�������Uȋ�Z��p��E�9�u�U�t& �ƍ{�U���������Cp�Kl�U��A��Ct�Sl�Sp�Ct    ��x���������}ȉ�������Fl�Vl��l9�u��E�d+    �]  �e�[^_]�g�_Ív 9������=    �����  �   �   ��@B d�    ��������  ��\  ���  �������&    ���   ���   �E����   #��   ���   !�#��   #E�!Ѓ��u
Ǉ�       Ǉ�       ������t& �E��M��U�9ȉ�E�r7�    �����  �   �   ��d�    ������E� ʚ;�U� ���  ���   �E��U����   9��u�����U������   ���   �r����t& ����  ���������ٺ   ���  �   �����������������&    ��    �����W�xV����΍H)�S�Ã�0�@    �@4    ��1�����������C����	��C�����C�����C �����C$����t� ʚ;i� ʚ;�C0   ����S[^_Ð�����UWVi� ʚ;S�Ã�d�    �D$1��T$���������tg���  �   ��1�$1������$��;��   ��   r-���  �T$�����1��T$d+    u$��[^_]Í�&    ���   ���   �Ÿ�������������&    ��&    �����U��WVS�����  ���   ��tR���   ������Ɖ׍��   ������$���   �T$������$�\$�u�}�E��M �]�U��[^_]Ð몍�&    �t& ������WV��S��0  ��t&��,  ��    �ډ�������C    ��0  ��u拞@  ��<  ��t ��    �ډ�������C    ��@  ��u拞P  ��L  ��t ��    �ډ�������C    ��P  ��u�[^_�f������WVS���  ���   ��t"���   f��ډ�������C    ���   ��u拞�   ���   ��t ��    �ډ�������C    ���   ��u拞�   ���   ��t ��    �ډ�������C    ���   ��u�[^_�f������ǀ\      ǀ`  0� ǀd      Í�&    �t& ������UWVSd�    ����d  ��t��[^_]Ív ��4  ��D  ��$  ��8  ��(  ��H  �t$��!։$��#t$!|$�׉L$!�!����t_���   ���   ���  ���  �L$�T$������  ��  ;$���   ;t$��D$��   �T$�t$9t$���   ���  ���   ��ty���   ��uo���   ������ǉՍ��   ������$���   �T$������D$�$�ыT$��;��   ����   s:;��   ����   s*�D$;��   ��   s�SH����������  �������d  ��u$��\  �ع   ǃd     ��[^_]�������v�����&    f������UWVS���l$����   �Ӌ��  �ωع   ������$�T$��tl�U�E ��	�t�L$9$�r]�E @B �E    �W���	�t$T$��W�����;��   ��   s���   ���   ��[^_]Í�&    ��W�ɐ+$T$�E �U�W���	�t�뢍�    ��[^_]�f�f�f������W��� V�ֺ�  S�Ã�h�{(�������� �  ���������������ǅ��u�  ��sd�CL��[^_�f������WV1�S���   �ӋW ��t	��������ƋGd�������ǃ�       [^_Í�    �����UWV��S����|  �{@��������{X uD�K��k؅�t�V$���������u�C<��������   1҉��$������$��[^_]Ít& �������ߐ�����VS�ËPd�@(�sh������������ƃ�   ��������Cd[^�������&    �v �����U��W��V��S�������tt�Ë@������x8� t�������[��^_]Í�    ���   ��t����   �}h����������    u�n1���uſ����뷍v �������������F    럿����럍�&    �t& ������UW��VS��d�    �T$���@����Å�u8�,$�t$������E$t�N�������t����������ÍFh�������������D$d+    u
����[^_]��������    �����WV��S��d�    �T$��������Å�u+�|$������O��t���������ÍGh������$������D$d+    u	����[^_�������t& ������WV��S��d�    �T$���Q����Å�u+�|$������O��t���������ÍGh������$������D$d+    u	����[^_�������t& ������UW��VS��d�    �T$��������Å�u8�,$�t$������E$t�N�������t����������ÍFh�������������D$d+    u
����[^_]��������    �����UWV��S�����   �$�{h����������    �$u3��tO�k1҅�t�����������$������$����[^_]Ít& ��������   ����[^_]Í�&    �v �   �ˍ�&    f������UWVS�����   �T$�{h�$����������    �$�T$u-��t?�k�������t	��������Ɖ����������[^_]Ív ����������������[^_]þ�����֍v �����UWV��S�����   �$�{h�L$����������    uB��tT�k$�������t�t$�V �؋L$�������X���$������$����[^_]Í�    ����������������[^_]ú������f�������U��WV��S�˃����P���  �T$�D$d�    �D$L1����  ������D$1���ǋ��   ���   ���   ���   �D$�T$1ҋD$D$uw�D$�T$�L$���  ������T$�L$�D$ ������D$ ����C�D$$�C�D$(�C�D$ ������D$ ��D$$�C�D$(�C�D$Ld+    u`�e�[^_]Ít& �D$�T$0������D$�L$�T�4�D�0�T$�T$�D$9�L$�@B �    �J����T$�L$+T$L$�5����������&    ��&    ������W�|$����w�U��WV��S�Ã�l�U��Q�E��?d�    �E䋃�  �����E��A�}�%9��   ѹ����|+=��>ډӃ���  i� ʚ;� ʚ;��ʋN�����Ӌ�}�%�M܋N�]�9º   �E�������E����|1=��>ډ˃����  � ʚ;i� ʚ;��ʋN�����ډE��U��E����  ������]���]����   ���   ���   �Eԋ��   �U��E��E܉U؋��   	��M��M��U��U�	щM�M�t)��	��  �EԍM�P�U��E�������E܋U��E��U�Y�E��U����   �E����   �U����   ���   >�t& �E����  �������t?�UԋM؍E�������E��U��M��G�E��G�E��G�E�������E���E��G�E��G�E�d+    ��   �e�[^_]�g�_Í�&    f�d�    �    �w���d�    �    ��t�@�   +U��u��u��������XZd�    �@���������6����t& @B �� �E܉U������1ɻ   ������E�    �E�   ��R��������������W�|$����w�U��WVS��d�5    ��<�U�d�    �U�1҃��  ����  ���������   �E�d+    �j  �e��[^_]�g�_�f�1�1�;MċM�ȸ�  LEĉEĸ    MȋUčE�������E؉C�E܉C�E��C���  ���   ���   �щE�������Eȉ�ẺC�EЉC���������yy���  �����󐋆�  ��������  �x`��tк   ��������Eĉ��U���������C����E�    1��U����t& �Q�M��   ��� ���Z1��������&    ��E��P�H�X���  �Eĸ}�%9и   ��  � ʚ;i� ʚ;��ʉ�1��ӉM��؉]�	���   �E���X�H�}�%9и   ���   � ʚ;i� ʚ;���1��ڋ]ĉ��   �����   jj j �U��M��������>�t& ���  �����1��0���Q�M�1҉��C���1�X�����v �E�ǀ�       ǀ�       븍�&    d�    �    s�d�    �    ��t�@j 1�j �M������Y[d�    �w���������m�����&    �E������E���������������������������������UWVSd�    ��4�(�xd�    �T$01҃�t6���-  ���������   �T$0d+    �I  ��4[^_]Í�&    �L$�   ��������l$�D$�\$ ��Mb�D$������D$$�D$��)ڋ\$,�T$������   ����)ډT$��������������u����t& ����  ��������  �   ��`�$������D$�$���������u}1�1��l$�ʉ��������D$�D$$�D$�D$(�D$�D$,���  ���   ���   �щD$��������  ����������v �L$1҉�������l$�������&    �v �L$��9���  M�M��m�������������������   >�t& �   �   �����1���Í�&    �d�    �    s�d�    �    ��t�@j 1�j �L$�����XZ�$d�    u������띍�&    �v �������4� d�    �T$01Һ����L$�D$    9��D$    G��D$    �$    �D$1��D$    �����D$$�L$(	�u�T$,��u�T$0d+    u��4�f��|$,�d�~�����������&    �t& ������UWVS��H�p�d�    �T$D1ҋP���  �   �D$,��������8  �L$4���%  �D$8=?B �  �|$,���
  �T$0��?B ��   i��  �|$��i��  �L$ ���|$�L$$�D$(�T$�T$����   �؍L$,�����Å�uW�D$,�|$4��Mb�D$������D$8�D$��)��|$@�T$������   ����)��T$�T$������������E؋D$Dd+    u]��H��[^_]Ít& ��T$1�1������r��=�    ���  ���T�����&    ���1�������맍v �����띻����������f�f�f�f�f�f��� �����Í�&    ������U��W1�V��S�����(�Z$�L$�J(�D$��D$�|$��1��� E�E�����   ��1҉$����T$����D$�T$���������� t��1҉D$ �D$�T$$3D$ �T$3T$$	�tF���������������ƅ�u_1ɉ����  ��1ɉ�9؉�ȉ�C�Cƍe�[^��_]Í�&    f����T$��9D$�����s��|$ u%t$|$�����ƅ�t���1����뙍�&    f�1�1��� ��������;$��D$�d���t$|$빍�    �   �B$   �D$    �D$    �$   �D$    �������&    ��&    �����U1ɉ�]�p��������U��WVS�   �<��    �M$���e$�E4��ʋM(������ �ME��������t$�����  �u�} ��������},�E�Uu�1�[^_]������V��S�@,    �@p����   =    ��   ��� �������� �ÍFt�B�Vt�Fx�� �*��    ��� �P��p��Q�
��� �B��@�� ��� ���������� =�� uĉڸ�� [^�������    �    �F�����wA�����������)�   �Fp=    �S����v`hL ������Fp    XZ�6���뻍�&    �t& ������V�Ƹ�� S����������� tK���  �    �    1���t� ��t�p`hF�  h   S������ƃ���� �������[^Í�    �������������W�ǉ�VS��,d�    �T$(1ҋL$<�T$������Å���   ��� �������� �������� �X���=�� u�   �v �Ct�X���=�� tq�C`�T$�������u�K,���  ��tt��    �    9��� ��   �������� ������������    �D$(d+    ��   ��,��[^_Í�    ��� ������������� ������ōt& ��Cx�St�B���� �st�sx�������� ������\$<땍t& ���������  ��� �$�D$����j�L$������\$��� �����X��t��R����������&    ��    �����U��1�WV1�S�Ã��@\�ǅ�t��1���Ɖ�1����s$�T$�l$�S(�$�   9$�C$P��Q� ʚ;������CX1ɉ������   �C�C\�S �������C�S��[^_]Í�&    ��������HX�L$�H\�@0t�$�P����$���U����t& ������V��S�Ӄ�d�    �D$��� �������� �؉4$�D$����j�L$�������� �\$�����X�D$d+    u����[^��������&    ������UW�Ǹ�� VS�����������7�V,����   ��    �    ;4t������G[��� ^_]�����f���� �X�=�� t�1퍴&    9�t4�C,��u-�ډ��������t �C|�������t��tH�E|���������    �Ct�X�=�� u���t���������Nt�Vx�Ft�Q�
�Ft�Fx1��j����t& ����čt& �����V��S�@,9�u[^Ít& ��ӋV0��uN��tyw��ub��t݋V8��u�F,   �͍t& ���uÃ�uw�V@��t����������u��t& �^,룍v �^,��u��F$��u��F$   농�    �VD��u��ύ�&    ���g����V<��t����������Q�����F,   므=�    �:���PhJ�  ��   �����ZY������&    ��&    ������S�x,��u�C�����C���[�f��@0u�PD��t	�������u��C,   �C�����C���[Ív ������PH��t�������&    1�Ít& ������UWVS�Ã��D$�$����   �C,�S�։ωK����   ����   �k0��t�k�ى����������[^_]Í�    �����)��9���}   �C�S9�����K M�MƋs��9��L�M��c$�K$ʋK(������ �KE��������t��<$ t�����[^_]�P�����1�[^_]Í�    �������[^_]Ð�<$ ������S�������&    �=�    �'���PhJ�  ��   �����]X���������S���@0t�=����S,��t-1���t[Ít& �S0��u��t%�S8��t��[������t& �S�Kj �������Z[ø����[Í�&    ��&    ������W��V��S�[���������t��t�[^_Ít& �����������捴&    ��    �����Í�&    �v �����V��S��t7�Ë@|������S,��ur�Sx�Kt�Ct�Q�
��� ��� �B�St�Cx�� ��t=�F,��uq�F0u�VD��t���������u��&    f��F,   �F�����F���[^Í�&    ��C0u�SD��t����������r����t& �C,    �b����t& ��&    ��&    �������� =�� t/S�X��v �SP��t�C,��t��������Cx�X�=�� u�[Ít& Í�&    ��&    ��������� =�� t/S�X��v �ST��t�C,��t��������Ct�X�=�� u�[Ít& Í�&    ��&    ������S�ø�� ��������������� [�������&    �t& ������UWV�Ƹ�� S������ǉ��������� ��C��J����� t)�t& �Xx�Z��@t   �@x"  �Qt�ȍJ�=X� uۡ�� ��X��i�=�� u�g�t& ��Et���t��`� tR�ŋCp�0s�Cp�V�����uى��������u΋C,��u9�Cx�St�B��Ct   �Cx"  �Et���t��`� u�[��^��� _]�����f�f������9    t3���  �����d�    �@4��f������������   ������v �    ������    �    @B �   �    ������    �    ����������뉍�&    �t& ������UWVS��������s�{���S����;    t[^_]Í�    �{,u&    �v ��@B j �؃� ��������Z��t��������t܉������Ӎv �����V��S�    �������    ��@0u1�[^Í�&    ���[^�������&    ������¸    �    Í�&    �t& ���������  �����1�d�    ��t�R0��t���   t�������Í�&    f������WVS��������C0�uU�u^�    �uM�5    �=   �    9�u�   ����������&    f���@B �� j ����������Z��u�[^_Ð�릍t& �������u��غ   [^_����������UW��V��S�Ӄ��D$ �$���tq�P�H�(�     �T$�L$��$�Cp�
9t�Ch����������������u+�N��u����1�[^_]������t$����t$�����XZ��[^_]Ít& ��=    �t�F    1��D$    �D$    끍t& �    ������    �   �������WV��S�    ������    �������ǋ���������������)Љ��   P�������X�F0u[^_Ív [^_����������UWV��S��������Sp���   ���������̓�)ǋCp��    9tr�Ch��yK����   �Fp��    9tE�C0u�F0u9�������u0�Fd9Cd*�Vp�Cp[^� 9��_�ŉ�]�f��������u���&    1�[��^_]Í�&    �C0u*��u���    �������t��Սt& ��C0u��荴&    ���u�뼍�&    �v �����UWV��S�������Ǹ    �    �(�É��������tY�F|�������u[^_]É��������uM������������������)Љ��   P���m���X�F0t�[^_]������t& �[����^_]������v ��1������먍�&    ��    ����������;    tÍt& ��    �    ��t��£    Í�&    ��&    �������¸    V�    S�0�@    ��t�F,    ��1҉�������    �    [^Í�&    ��    �����S�    �������    �[����������VS�    ������    ������Ƌ���������u�C��u�1������[^������t& ������[^�������&    �t& ������S�    �������    ������[�������&    �t& �����������a���������V�� S������� �    ���� 9�t(������    ��    �������� [^������v �����>�t& �       ������� [^������v ��  d�    �    s�d�    �    ��t�rj���ډ������Xd�    u������������V�� S������    9� t"������-� �� [^�������&    �v ������       �����>�t& �ȍt& ���  d�    �    s�d�    �    ��t�rj ���ډ������Xd�    u�������x���f�f���������� ��� Í�&    �v �����U��WVS�x,�Ët$�|$t
�   �����j����������X�C0 t7��Kp������)��    ��    9t�Sp�Ch[^_]�������    [^_]Í�&    �t& ������=�    �y�  Í�&    ��    �����S������ � r4�á � ��t �����    �    ��PL� � �������[Ít& �,� �� � �Z0������볍�&    �t& ������UWV�    S��������,�    ��  ������,� �x,tG�    #(� � � �J����{,��tB��  ���������t�D5 [^_�]������v [^_]Ív [��  ^_]�������&    �v �C�Sj@B �� �щ������X�f������UWVS�Ã��������tG�Ɓ;�� tO��� �(� �S,� � ��5 � � � 	� ��u��u0�C�����C�����[^_]Í�&    �v ��[^_]�����t& �    �    ������    �   �$�    �T$������غ   ������-    ������\$� � �����9�s)��    �:��t�$�\$�J�Z� � �����9�r׋\$�t$���t$���.���XZ�E�����&    �����UW�Ǹ�  V�    S����������������    �40��t�F0�t�t���ڸ�  [^_]�����������-,� �(� ��t4��t%��ta��tʋ(� ����   ��t���������f���    ��$� ��(� rÅ�t��E0%�   0� u����$������$뤐�� ��u���$� ��(� s��=0� ���u���1҉��$������$�a���f����%����0� ��u�   �������������    �������������&    �t& ��     Í�&    f������UWVS�Ã��D$��  ������C�����C����    � �     ������D$    �ǉָ�����$�����D$���f��� �����;    sN��    ����J�R9��|�� � ��� �čt& ;$��l$}��D$�$�T$먍�&    ��������� �    � � � ��     �ѣ � �х�us�f����4$�|$�É��5�����	�tWV�T$�D$����XZ��  �������u��[^_]Í�&    ������    ��    ����[^_]�������    ! � 끍�&    �t& �������,� Ít& �������(� Ít& �������¸�w  ��    �Í�&    f������UWV��S�ø�w  ����    �-,� �����   �C0�u��B��B�~   �X��   �=0� ��   ����   �Ed9Cd��   �C|���������   �ډ�������E     �(� �,� ����   �C0t}�=0� ��   ��[^_]������v ���������)��{p��    9�a�����t�yd9{d�Q����C|�$������$��tu��� �ȉڿ�w  �������    �8��[^_]Í�&    ������獶    �C|�������tՉ�1�������1�����    �   ��������-�����&    �C0�������  ������0�    �á,� ��t�������ڸ�  [^_]�������&    f��������t9,� ����Í�&    �v 1�Í�&    ��    �����V�����S����t�þ����;,� t����[^Í�    ��  �$������$��������Ƹ�  ���������[^Ít& ������U�-,� WV��S�ø�  ������ǋC0�uj�uF��5(� �5$� ��   �0� ����   1ۃ�u���6�������  �������[^_]Ív �CL��u��=�    ��   �CL� 륍v �KL�    ����   ��5(� �0� ��un�   ��t��   �������둍�&    �(� ��t��t:�E0�u41��5(� ���e����v ��t��������ٍv ��5(� �*����v 1��;������   �D����)�����&    �=�    t�CL� �T�����  ��  ��&    f������S�    �������    ���t���t�����1�[ø����[ø����[Ít& ������빍�&    f���������Ҹ    ��� EÍ�&    �����VS�ø�  �������(� ��$� ��    ��w  ���t�01��������    �0    ��� ��� ��� �,� ��u[��  ^������t& ��0� (� u��������  [^�����������S��  ������á,� ��t������ڸ�  [������v �����1��=0� uÍ�&    ������(� ��Í�&    ������V��  S������5,� �Å�t*��������0� ��t+��u�(� ��t�   ��������ڸ�  [^������v �(� ��t�   ���������������� Ít& ������������� ����Í�&    ������S������� r[Í�&    �v ������    �    �{u݋�   [�������&    �t& ������U��W�    VS�������<�    �������7��u2��    ��w  �<��t����  1ۃ�t`����[^_]Í�    �,� ���+  ��  ������=,� ��tn��� ��  1۸�  ������؃�[^_]Í�&    �v ��   ��������   ������F�Vj�щ������X�؃�[^_]Í�&    �v �G0��n  �G�W5�����	�t	;_l��   �0� ����   ��� �`����� �}  �G0�t6������G0��W�ŋGt��������$����$t;olt�N9V�|�   ��������� rR�V�F;G��O�����R��P���C���^]�G0�������G�W5�����	������;_l�������� ������������    �,��   ��1ۉ�[^_]Í�&    �v �   ���������� �f����F�V5�����	��Q�����������F�v9��|.��� 1��1�����&    �0� ������������t& ��   ��������������&    ����������,� ��t��  �����������|�����&    ��&    ������S��  ������0�    �á,� ��t�����ڸ�  [�������&    f������UWV�Ƹ�  S������á,� ��t!�@0�t�x�H���ʁ������	�t;plt�ڸ�  [^_]�����j���������X��  [^_]������t& ������1��=0� ��Í�&    ��&    ������,� 1���t�B0����f�f�f������� � � � �����1�Í�&    �������@� �����1�Í�&    ��&    �����W��V��S�����������@� j
j j ������`� � �@�Cl�����1���[^_Í�&    ��&    ������@� �   �   ������ � �\� `� �����f�������W��V�Ƹ��  S�Ӄ�������ډ�������d�    	�t�x,tI1҅�����R�������Z��[^_ú   �$������$�@�����@�����1�[^_Í�&    f��   �$������$륍�&    ��&    �����S���  ������   d�    ������������j�щ������X[Í�&    ������WVS�t$�É�|$�   �����������D$   [^_�������&    �t& ������W��VS�    ������    �3�����  �F0����  �u �C   ��   �>����������1�[^_����  ��&    �����S�[����  �����d�   ��������t�[Í�    ������    �a���������U�    ��VS������    �t� �5p� ��	�u�5    �   �5p� �t� �    �    ���������[^]�f������U��WVS�Ã�����T$�	��&    �    �u��5p� �=t� �    �    9�uۉKx���   ���   �L$������D$����   ����@B �� �D$�T$������C0�"  ������D$�    �T$;D$t�����   �D$�����D$���������L$����)��9D$�����Ѻ���r5�t$�|$9Ɖ�щ�C�C��Ή������   ���K|�e�[^_]Í�    �D$�T$��뽍�&    �v ��������.�����  �����df�    ������D$����������@B �D$���   )��T$���   �9��L$�� ���������t& ��C0����������&    f�1�1�1�1��B����v ������@0td�    �Au8�@<    �@@    �B4��f������������   �������&    �v �@D��&    �v �����UW��V��S�    �Ã���   �-    �   �    9�u�9�����   �    �����;5    ��   ��   �    �?B +5    =   9����   �p� �t� �   @B �� 1�    �p�    @B �t� �� �    �   �    ������    �����[^_]�������    ������[�    ^_]�����f�[^_]Ív ����1ۉ։���t���C���������i�@B )ƿ@B �ȉ����ڍH����p� t� 1��A�����&    ������W��V��S��������    ���tb9�tc�    9��   t1ǃ�       �    ���   �C0�t���C0[^_Í�&    �v ���   �����   ��uЉ����#���밐�    ��������������UWVS�ø�  �����d�-    ������׉Ɖ؉��@�����u<�C<    �C@    �C0u7j �����h@B �����X�   Z[^_]Í�&    �v ����7����C0t�[1�^_]Í�&    �v �����UWVS��w  ���$������    ��  �����d�-    ������$�׉Ɖ��A�����A������������������C0u+j �����h@B ������C�S�   ��[^_]�����f���[^_]Í�&    ������W��V��S��������C4�S8��j �C�S�C�؉S��h@B ������{,XZt)�C�S�   ������C<    �C@    [^_Í�&    �C�Sj
+CSR�KP�S����������ō�    �����UWVS���\$$�B0uZ��tJ�T$�D$ �ڃ� ��HD$ HT$�ǉЉ�1��։���um�ȉ��  ��L$ ����y�؃� �ډ�S��[^_]Ít& �ՋL$ �T$+UPMT�$�L$�������u@�$�T$EhUl�D$�T$ �EP�UT�f�����Mb����Љ�����i��  )��s����$�L$UpMt뾐�����W�|$����w�U��WVS��w  ���5    �U����   �E��    ������щE����   P�E�QR��������Ch�Sl���E�U�M�]�ʉ��ڃ� ��H�HщǉЉ�1��։Å�t��Mb����Љ�����i��  )ƹ�  �؉���M����y�؃� �ڍe�[^_]�g�_Í�    �C0u2�Ch�Sl�E�U��z�����e�����������[^_]�g�_Í�&    �v �E�U�M��������u��U�M�Љ�ChSl+CPST�E�U��&�����&    ��    �����W�|$����w�U��WVS��w  ���5    �U����   �E��    ������щE����   P�E�QR���v����Cp�St���E�U�M�]�ʉ��ڃ� ��H�HщǉЉ�1��։Å�t��Mb����Љ�����i��  )ƹ�  �؉���M����y�؃� �ڍe�[^_]�g�_Í�    �C0u2�Cp�St�E�U��z�����e�����������[^_]�g�_Í�&    �v �E�U�M��������t��U�M�Љ�CpSt+CPST�E�U��&�����&    ��    �����S�    ��s>�B,��t'd�    � �u��  �����df�    f��u=��[Ð1ۉ�[Í�&    �v ;    u
�    �������B<    [�B@    Ít& ������    뵍�&    �v ������¸�w  �    Í�&    �t& ������S��w  �������    �D0[���Í�&    ��    ������¸�w  ��    �D0����f������UWVS��w  ��$������    ������Ƌ��   ��   �Z  �{|���   1��CH9��|%ǃ�       ǃ�       ��$[^_]Í�&    f��C0����D$��  ��������   ǃ�       d�    �D$���   �T$�D$�C|ǃ�       �$���   �D$�    9���  ���t�c0��C0��   �����������C�S�K0�C4�S8>�t& ��&    ��$�D$�S,�K<�щC@5���	���  ���X  �$�T$�   �C�S�C�S������CL�|$ ���   ���   ������C0������Cx�CD��$��[^_]�����f��ډ��g������������������ǉ������t& ��K@�T$�C<1ʉL$�$1�	��;����T$���с����	��j����S�K�T$�T$�L$ 3D$3T$ 	��H����=�    ���������   ������t$ ���t$ P�D$ �p�p�s@�s<�t$,�t$,h(M �������(�C0�����������    �K0�    �����C0�0����\����v j
��j j �T$�L$������������v d�    �    �M���d�    �    ��t�@1ɺ   �����d�    �!��������������&    ���L�����������@�����&    �t& �����S��w  �������    Ǆ�       Ǆ�       [�������&    ��    �����S��������w  �    ���   ��   u"�K0������K0�CP�ST������[Í�&    �ڍt& �����S��w  ������    �C0t������K0�CP�ST[�������    [�����f������S��w  �����1��    �C0�t���   �C0��[������U��  �������]d�    �P�@������U��WV�Ƹ�  S��w  ����������d�    �T$������    ������ST�C0�ǋCP�T$�T$�D$��   �B�R+D$T$����V���H�����u��V�e�[^_]Í�&    ������������Љ�	�tى؉L$������L$��9��C�C��T$�T$�$�$+D$T$�e�[^_]Ít& �u�����&    f�������¸�w  ��    �DHÍ�&    ������S��w  �������    �DH[Ív �����WVS��w  ������    �C0u	[^_Ít& �������Ɖ��*��������������c0���؉��1����    +CD�s`�P��{d�����w�[^_����������UWVS��w  ��������    ��C0��   ���   ��   ��   �C0������C0���������u���u���[^_]Ít& �$������$�Ɖׄ�t������C0uY�c0���������t�����������L��������������c0���؉��S����    +CD�s`�P��{d�����w�������{������+SPKT�$�L$�������tF�$�L$SpKt�sP�{T�u�����    ���   ��   �����t& ������&    f��$�T$ChSl븐�����WVS��w  �������������    �C0u	��[^_�f���������C0�רu7�t�+�  �������w  d    �0�x�[������4�����t��볍t& ������C0u�c0�������C0믍�    ���+SPKT�$�L$�������u�$�T$ChSl�sP�{T뼍�&    f��$�L$SpKt��������UWVS��w  ��������    ������   �Ɖ׉غ   ������CP� ������C�C�h� �S�S��tK�    1�1��������� � �$��t��1���ŋ$���$��������$�����CSCSj �����h@B ������s�{j
�C�S��)��R��P����������=     t�C,   ��-     s��[^_]Í�    ��[^_]������t& �����WS��w  �    �C ��t��������{�    1�ǃ�       ���)����   ���[_Í�&    �����S�������w  ���    ��    ���J(�    �����;    r�[Í�    �����S��w  ������    ��K([�f������WV��S��w  �������    ���r( s�J,��t1���[^_Ít& �������t��������t�   ��t�������=     �4�    tĸ�� ������ǅ�u��   �   �������������C�C�C �S�S�@�����j �щ�h@B ������S�C�   ������=     XZ�T����C,   ��-     �>���������6���f�f�f�f������UWVS�Ã��    ���    ��   ����   ��@@��   �Sl�D$�   �Ch�$   �    �s�{ �t   �p   �5(   1��=,   �K�S �C������ E�E֣x   �|   �Ct�SxChSl��   1��   �K�{ �s�T$�$�C|������ ��ɚ;E�E����9��    �s(1ɿ�ɚ;1��v  6eĉ������9��r�1�$|$��   �$��   �T$��   �    ��   ��   �D$��u%�    ���    ��   ����   ��[^_]Ív �C�S�   �   �C�S�   �   �C�SD�   �C�   �C@��   ��   �C8�S<�   �  �CH�  �CL�  �Ct�SxChSl�$1��T$�0   �4   �C|�K������� E�E�CS �Ɖ׸ ʚ;1�1������� ��E�E�9��rJ��    )�׃$�$�T$ �T$1�0   � ʚ;�4   �K�    ������ ��E�E�9��s��$�T$�58   ��   �=<   ��   �K�$1틃�   �T$������� E�Eŋl$Ɖ-�   � ʚ;�1҉t$1��|$�<$�=�   �K������ E�EƋt$9D$�rc���t$�� �<$�|$�l$f�)Ƌ$׋T$1�   � ʚ;��   �K�    ������ ��E�EŃ$�T$ 9��s��t$�|$�D$�T$��   ���   ��   ���   �P  �T  �CP�ST�X  �\  ���   �ChSl��   ��   �C�S ��   ��   �~�����&    �t& ������    ��   �   ��   Í�    ������    ������    �Q��   �    �Q��   �f�������¡    ���    ��   ����   �    �����f�f����������к�
 ���  �������&    �����U��M W��V�   S1��������M ���������&    ����� ��t'�ى��P��K���R��1��D�Qh<�  W����������� u�[1�^_]Í�&    ��&    ������������DʉȺ   ��9�O��� �f�f�f�f�������@�Í�&    �����V��   S�É���������  ��t0�[�   �K��C��t,�P	�x5��������C��E؉�[^Ð���������[^Ít& �   ���������f��   ��������f������V��   S�É���������  ��t0�[�   �K��C��t,�P	�x5��������C��E؉�[^Ð���������[^Ít& �   ���������f��   ��������f������U��WV�p����S�ˉыUt��tY�ډ�[^_]Í�&    ��B�z�r�}�%9º   �|O� ʚ;i� ʚ;���1���9����|")�Ӊȉ�[^_]Ít& ���z�r뷍�    1�1ɉډ�[^_]Ív ���������뺍�&    �t& ������UWV��S������   d�    ��p  �׹   ������P��������Ņ���   �P   �8   ��@ ������Å��Q  �@   1Ҹ�@ ������C0����   �C    �C���������   �k�C    ��t&���   �   ����   ����   �P	���   �F�;�C4 �C�F�C�F �C �F$�C$�F(�C(�F,�C,����[^_]Ív �I�   ��F��tw�P��	�yٺ   ��������ˍ�&    �������؉��������   ������؃�[^_]Ít& �$�C01�������4$�ˍ�    �   ��������H�����&    �   �ȉ�������X����t& ��   ����������������농�&    ������S�Ë@�   ��������t���   ���������   ��t%��~)�C������C01��������[�������&    ������ؐ�   ���������f�������������P��t��~Í�&    �H����f�����    S�X�   ��[�������&    ��&    ������U��W�z�VS�Ӌp��������`  �C��   ��������;  �E�   �@l��������#  �k/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <waiman.long@hpe.com>
 */

/*
 * Collect locking event counts
 */
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/fs.h>

#include "lock_events.h"

#undef  LOCK_EVENT
#define LOCK_EVENT(name)	[LOCKEVENT_ ## name] = #name,

#define LOCK_EVENTS_DIR		"lock_event_counts"

/*
 * When CONFIG_LOCK_EVENT_COUNTS is enabled, event counts of different
 * types of locks will be reported under the <debugfs>/lock_event_counts/
 * directory. See lock_events_list.h for the list of available locking
 * events.
 *
 * Writing to the special ".reset_counts" file will reset all the above
 * locking event counts. This is a very slow operation and so should not
 * be done frequently.
 *
 * These event counts are implemented as per-cpu variables which are
 * summed and computed whenever the corresponding debugfs files are read. This
 * minimizes added overhead making the counts usable even in a production
 * environment.
 */
static const char * const lockevent_names[lockevent_num + 1] = {

#include "lock_events_list.h"

	[LOCKEVENT_reset_cnts] = ".reset_counts",
};

/*
 * Per-cpu counts
 */
DEFINE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * The lockevent_read() function can be overridden.
 */
ssize_t __weak lockevent_read(struct file *file, char __user *user_buf,
			      size_t count, loff_t *ppos)
{
	char buf[64];
	int cpu, id, len;
	u64 sum = 0;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	id = (long)file_inode(file)->i_private;

	if (id >= lockevent_num)
		return -EBADF;

	for_each_possible_cpu(cpu)
		sum += per_cpu(lockevents[id], cpu);
	len = snprintf(buf, sizeof(buf) - 1, "%llu\n", sum);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

/*
 * Function to handle write request
 *
 * When idx = reset_cnts, reset all the counts.
 */
static ssize_t lockevent_write(struct file *file, const char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	int cpu;

	/*
	 * Get the counter ID stored in file->f_inode->i_private
	 */
	if ((long)file_inode(file)->i_private != LOCKEVENT_reset_cnts)
		return count;

	for_each_possible_cpu(cpu) {
		int i;
		unsigned long *ptr = per_cpu_ptr(lockevents, cpu);

		for (i = 0 ; i < lockevent_num; i++)
			WRITE_ONCE(ptr[i], 0);
	}
	return count;
}

/*
 * Debugfs data structures
 */
static const struct file_operations fops_lockevent = {
	.read = lockevent_read,
	.write = lockevent_write,
	.llseek = default_llseek,
};

#ifdef CONFIG_PARAVIRT_SPINLOCKS
#include <asm/paravirt.h>

static bool __init skip_lockevent(const char *name)
{
	static int pv_on __initdata = -1;

	if (pv_on < 0)
		pv_on = !pv_is_native_spin_unlock();
	/*
	 * Skip PV qspinlock events on bare metal.
	 */
	if (!pv_on && !memcmp(name, "pv_", 3))
		return true;
	return false;
}
#else
static inline bool skip_lockevent(const char *name)
{
	return false;
}
#endif

/*
 * Initialize debugfs for the locking event counts.
 */
static int __init init_lockevent_counts(void)
{
	struct dentry *d_counts = debugfs_create_dir(LOCK_EVENTS_DIR, NULL);
	int i;

	if (!d_counts)
		goto out;

	/*
	 * Create the debugfs files
	 *
	 * As reading from and writing to the stat files can be slow, only
	 * root is allowed to do the read/write to limit impact to system
	 * performance.
	 */
	for (i = 0; i < lockevent_num; i++) {
		if (skip_lockevent(lockevent_names[i]))
			continue;
		if (!debugfs_create_file(lockevent_names[i], 0400, d_counts,
					 (void *)(long)i, &fops_lockevent))
			goto fail_undo;
	}

	if (!debugfs_create_file(lockevent_names[LOCKEVENT_reset_cnts], 0200,
				 d_counts, (void *)(long)LOCKEVENT_reset_cnts,
				 &fops_lockevent))
		goto fail_undo;

	return 0;
fail_undo:
	debugfs_remove_recursive(d_counts);
out:
	pr_warn("Could not create '%s' debugfs entries\n", LOCK_EVENTS_DIR);
	return -ENOMEM;
}
fs_initcall(init_lockevent_counts);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef __LOCKING_LOCK_EVENTS_H
#define __LOCKING_LOCK_EVENTS_H

enum lock_events {

#include "lock_events_list.h"

	lockevent_num,	/* Total number of lock event counts */
	LOCKEVENT_reset_cnts = lockevent_num,
};

#ifdef CONFIG_LOCK_EVENT_COUNTS
/*
 * Per-cpu counters
 */
DECLARE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * Increment the statistical counters. use raw_cpu_inc() because of lower
 * overhead and we don't care if we loose the occasional update.
 */
static inline void __lockevent_inc(enum lock_events event, bool cond)
{
	if (cond)
		raw_cpu_inc(lockevents[event]);
}

#define lockevent_inc(ev)	  __lockevent_inc(LOCKEVENT_ ##ev, true)
#define lockevent_cond_inc(ev, c) __lockevent_inc(LOCKEVENT_ ##ev, c)

static inline void __lockevent_add(enum lock_events event, int inc)
{
	raw_cpu_add(lockevents[event], inc);
}

#define lockevent_add(ev, c)	__lockevent_add(LOCKEVENT_ ##ev, c)

#else  /* CONFIG_LOCK_EVENT_COUNTS */

#define lockevent_inc(ev)
#define lockevent_add(ev, c)
#define lockevent_cond_inc(ev, c)

#endif /* CONFIG_LOCK_EVENT_COUNTS */
#endif /* __LOCKING_LOCK_EVENTS_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          