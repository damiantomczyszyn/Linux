g
		 * any task-queue locks at all. We'll only try to get
		 * the runqueue lock when things look like they will
		 * work out!
		 */
		rq = task_rq(p);

		/*
		 * If the task is actively running on another CPU
		 * still, just relax and busy-wait without holding
		 * any locks.
		 *
		 * NOTE! Since we don't hold any locks, it's not
		 * even sure that "rq" stays as the right runqueue!
		 * But we don't care, since "task_running()" will
		 * return false if the runqueue has changed and p
		 * is actually now running somewhere else!
		 */
		while (task_running(rq, p)) {
			if (match_state && unlikely(READ_ONCE(p->__state) != match_state))
				return 0;
			cpu_relax();
		}

		/*
		 * Ok, time to look more closely! We need the rq
		 * lock now, to be *sure*. If we're wrong, we'll
		 * just go back and repeat.
		 */
		rq = task_rq_lock(p, &rf);
		trace_sched_wait_task(p);
		running = task_running(rq, p);
		queued = task_on_rq_queued(p);
		ncsw = 0;
		if (!match_state || READ_ONCE(p->__state) == match_state)
			ncsw = p->nvcsw | LONG_MIN; /* sets MSB */
		task_rq_unlock(rq, p, &rf);

		/*
		 * If it changed from the expected state, bail out now.
		 */
		if (unlikely(!ncsw))
			break;

		/*
		 * Was it really running after all now that we
		 * checked with the proper locks actually held?
		 *
		 * Oops. Go back and try again..
		 */
		if (unlikely(running)) {
			cpu_relax();
			continue;
		}

		/*
		 * It's not enough that it's not actively running,
		 * it must be off the runqueue _entirely_, and not
		 * preempted!
		 *
		 * So if it was still runnable (but just not actively
		 * running right now), it's preempted, and we should
		 * yield - it could be a while.
		 */
		if (unlikely(queued)) {
			ktime_t to = NSEC_PER_SEC / HZ;

			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_hrtimeout(&to, HRTIMER_MODE_REL_HARD);
			continue;
		}

		/*
		 * Ahh, all good. It wasn't running, and it wasn't
		 * runnable, which means that it will never become
		 * running in the future either. We're all done!
		 */
		break;
	}

	return ncsw;
}

/***
 * kick_process - kick a running thread to enter/exit the kernel
 * @p: the to-be-kicked thread
 *
 * Cause a process which is running on another CPU to enter
 * kernel-mode, without any delay. (to get signals handled.)
 *
 * NOTE: this function doesn't have to take the runqueue lock,
 * because all it wants to ensure is that the remote task enters
 * the kernel. If the IPI races and the task has been migrated
 * to another CPU then no harm is done and the purpose has been
 * achieved as well.
 */
void kick_process(struct task_struct *p)
{
	int cpu;

	preempt_disable();
	cpu = task_cpu(p);
	if ((cpu != smp_processor_id()) && task_curr(p))
		smp_send_reschedule(cpu);
	preempt_enable();
}
EXPORT_SYMBOL_GPL(kick_process);

/*
 * ->cpus_ptr is protected by both rq->lock and p->pi_lock
 *
 * A few notes on cpu_active vs cpu_online:
 *
 *  - cpu_active must be a subset of cpu_online
 *
 *  - on CPU-up we allow per-CPU kthreads on the online && !active CPU,
 *    see __set_cpus_allowed_ptr(). At this point the newly online
 *    CPU isn't yet part of the sched domains, and balancing will not
 *    see it.
 *
 *  - on CPU-down we clear cpu_active() to mask the sched domains and
 *    avoid the load balancer to place new tasks on the to be removed
 *    CPU. Existing tasks will remain running there and will be taken
 *    off.
 *
 * This means that fallback selection must not select !active CPUs.
 * And can assume that any active CPU must be online. Conversely
 * select_task_rq() below may allow selection of !active CPUs in order
 * to satisfy the above rules.
 */
static int select_fallback_rq(int cpu, struct task_struct *p)
{
	int nid = cpu_to_node(cpu);
	const struct cpumask *nodemask = NULL;
	enum { cpuset, possible, fail } state = cpuset;
	int dest_cpu;

	/*
	 * If the node that the CPU is on has been offlined, cpu_to_node()
	 * will return -1. There is no CPU on the node, and we should
	 * select the CPU on the other node.
	 */
	if (nid != -1) {
		nodemask = cpumask_of_node(nid);

		/* Look foRRAY_SIZE(lock_classes); i++) {
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
��t��L�f�L��Ս�    ��t��f�������ō�&    f������V��S��$d�    �T$ 1ҋt$0���������Å�xD��� �������� �X�=�� t�C,���������t5�C0�X�=�� u帀� ������������D$ d+    u%��$��[^Ív �������ø�� �������D����������&    f������S��� ��������L$�غ@w ������Å�x1��1������ �������[�f�������U�    1҉�]Í�&    ��&    ������U��WV��t& �    ��u�5    �=   �    9�u߉���^_]Ít& ������U�   ��MbW��w V��� S�Ǎ��  ��������w ��  ��������w ��Љ�����)�)˙��1�����t��1���Ɖ�������1�  ʚ��;1�ƅ�t��1���ŉ����1����   ��w ��w �����1�[^_]�f�f�f�f������Í�&    �v �����WV��S�XD�Bt\������C �~�6��	�t9��������    ������;    r�{ u4������C��������	�uΉ�[^_Í�&    �������C�S뗍v 1ۉ�[^_Í�&    �������D$�T$��t�L$�����Ít& ��D$PR�����XZ�f������U�ŉ�WVS��<�D$P�T$8�T$T�D$,�    �    �T$0�D$4��Qh��  U��������8�D$���D$(    �,$��&    �t$(h��  �|$W�]����t$Vh��  W�M����vh��  W�?�����$�5    h�  W�+����vh(�  W�����v$�v h<�  W������(hV�  W������D$4F �D$��T$8V$1�1ۉT$������N�D$X��Z�|$�����   �t& �V�N�~�T$�V�L$ �N�T$�T$�L$�N$�L$'�����WVShf�  �\$S�~����|$;Whw�  S�m����� h��  �\$S�[����L$$�\$(�ȉ�+D$T$RP�t$$�|$(��+D$��T$ ��RPS1�QWVhDJ �t$,�����|$4��0�������w�D$���)������!��������������t9�r��|$�Ƌ�������T$������D$(�D$(�D$(���@����,$�t$4�v$�v h��  h��  U�����F1�R��Ph��  h��  U�x����F1҃�(RPh��  h��  U�^����F1�RPh��  h��  U�F����F1҃�(RPh��  h��  U�+����F1�RPh��  h��  U�����D$`��(�����1҉Ë@,RPh��  h��  U������s8�s4h��  h��  U������C01҃�(��R��Ph��  h��  U�����CD1�RPh�  h��  U�����CH1҃�(RPh�  h��  U�����CL1�RPh$�  h��  U�q�����(�sT�sPh0�  h��  U�X����s\�sXh?�  h��  U�B�����(�sd�s`hM�  h��  U�)����sl�shh[�  h��  U������(�st�sphj�  h��  U������Cx1�RPh{�  h��  U�������(���   ���   h��  h��  U��������   ���   h��  h��  U�����    1҃�(RPh��  U�����l$`�D$d��  ��L[^_]�y�����&    f������WV��S�|$��Qh��  P�T���������  Wh��  S�=�����h�  S�/���XZ���u  �v`h�  S�����v�vh"�  S�����v �vh9�  S�������,�v$hP�  S������v(he�  S������v,hz�  S�������$�v�vh��  S�����vh��  S�����FD����tPh��  S�������F8��tPh��  S�z������F<��tPh��  S�d������F@��tPh�  S�N������FH��tPh�  S�8������6h0�  S�(���h��  S�����v4hF�  S������ ��yh��  S�����XZ[^_Í�&    ����������  ��t�P`Rh\�  S��������čt& h��  S����YX�|�����    h�  S����Y[[^_Í�&    ��&    ������S��������Hj�������������P�5    ho�  S�^��������P�5    hxJ S�G�����$h��  S�9���XZ[Ít& ������WVS�2���B���t;��t��������HV�������X1�[^_Ít& �r���r������1�ZY[^_Ð��t���E���1�[^_Í�&    �r�zh��  S����jh��  S����WVh��  S������$h��  S����1�[^[^_Í�&    �����S�XD�    ��Q �������;    r�{ u������C��[�1���f������WVS�����h��  �Ɖ�j �#���jh��  j ����WVh��  j ������$h��  j �����[�����X���&    W��1�V����ZY�غ    �������;    r�1�������)������&    ��������HS�1��,���X�غ    �������;    r�[^_Ð�����UW��VS��(d�    �L$$1ɍt$�\$<��Q V�����Y|$�Ɖ���  �v �����Q x��Ⱥų���   �����Si�  )Ѻ�����������Sk�<)Ѻ�$I���N����������)�    )ЉʍL$)BHЉ��S�����b ��   ��D$�T$ Qj h�: ��������֋D$(�T$,k�d���a�, �D� 1���l$�ŉT$�׺��O[����d   ���T$��D$�D$�T$�T$t$`�A^��1����D$�T$������   ���D$iD$]  1� ����������x�����t$4$i�qz  ��������x�vm���������l  ��L$�S�s�C�K�D$$d+    uI��([^_]Í�&    ��Q ����Q ��e�����&    �v ���D$�Y����v �L$��;������������V��S�É���������C�D$�S�T$�C1��S�N1��C    �� �C     ��������������C�S[^Ít& ������U��WVS�Ã����� �������3�{�$+C�T$S��#F#N�f�NʋKCS !�!��{ �|$�K�N1��{������ E�E֋4$CS�C�s�S�e�[^_]Í�&    �v �����U��WVS�ÉЉʃ����0��s�{�$�t$ �q�|$$�y�t$(�t$ �|$,�|$$)��T$�ƋD$(��{#T$,!��s�|$�t$�s �[�t$�\$�Y�I�މL$�L$(�\$,����9��s`�D$(�T$ +$�L$$L$!ЋT$,!��L$�������+D$T$1������� E�E։ƋD$�׋T$)���e�[^_]Í�&    ���L$�����D$T$1������ E�E�D$T$�e�[^_]�f�f�f�f�f������UWVS�    ��t)�։ύ�&    f��C�+���t$���������X��u�[1�^_]Í�&    ��    �����UWVS�    �t$�|$��t�Սv �C���W��V������XZ��u�[1�^_]������UWVS�    �t$�|$��t�Սv �C���W��V������XZ��u�[1�^_]������UWVS�    �t$�|$��t�Սv �C���W��V������XZ��u�[1�^_]������S� x ������x �¸ x �������[Í�&    �v �����U��WV�p�x�@@�@�Ōx �����)�׉�^��_]�f������U��S�ˉыP`�@\)�ڋ]��Ít& ������Í�&    �U��W��V��S�Ã��S@d�    �E�1��U�������U��    �CD�U��to�M�U�   �������tC�M�U�J0�`   f�J4�M�j<�B8    �H�M�p�xj �u�R�   j jS��������E�d+    u�e�[^_]Ít& ���u����������&    �t& U��W��V��S���^@d�    �E�1�������    �FD�]����   �M�U�$   �������tX�M�U�J0�`   �j<f�J4�B8    �x�O@�H�O�_�H�M�X�]�H�M�Xj �u�R�$   j jV��������E�d+    u�e�[^_]Ív ����s������������&    W��V��S�Ã�d�    �D$�C$��  u@�   �ډ��������t�T$,�p�x�P��������D$d+    u��[^_Í�    ��u����������t����������&    f�V��S�Ӄ�d�    �D$�F$��  uS�    ����������t*�X�S@�P�K�[�H�L$(�X�\$,�H�X��������D$d+    u��[^Ít& ��u����������t����������&    f�U��WVS�Ë�x   �������t[^_]Ív �M�u���  �}h �  �����C@�����WVPh�J S�������`  ��[^_]�����U��WVS�Ã���x   �������t��[^_]Ít& ��M�U�u�}�L$�M�$���  h�  �����C@�����WV�t$�t$P�uh�J S�������`  ��,[^_]�����W1�V�t$WV�����XZ^_Í�&    �t& �1��������&    f������W��1�V�֍RS�Քx �Í@������{<�s@��C0 l �CD    [^_Ít& �����W�|$����w�U��WV��S���E�ЉʉE؉���ӉU܋W�߉E�F�U�V��)�ׅ�x?��;u�}��    �   }A�u��u�������]�C�SXZ�e����[^_]�g�_Ív �e�1�1�[����^_]�g�_Ít& �u��xQ�u�ȉ��u�������Ɖ׋E�U����֍�E���ڋ]�CS�C�SY�M�[�]�9��|����� �g�����    �����U��W��V��S�Ë@@�@�Ōx �����W�щ�V�����Y[�e�[^_]Í�    �����U��P��WV�p�x�QR��������Y^�e�^_]Í�&    f�������PH�@H    ��t�������1�Ív U�   ��]�����f������W�|$����w�U��WVS�Ã�,d�    �E�   ������щE�������E�d�    ��E̋3�C�EЋ{�C���  �Rr$z(B,���R��P�E�������E؋U܉�E��S�CXZ�E�d+    u�e�[^_]�g�_��������&    �U1���]������t& ������UWVS�Ã�8�x d�    �D$41�����   ���  ���   �C$������@xtp���   ��tf�|$1��   �\$�D$�D$��  �D$�����������= ���w�   �F������ x ������¡x ��t-� x �������������D$4d+    uf��8��[^_]Ít& ����  �$������$��t��؉x 1�1�������$먍t& �� x ��������f������띍�&    f�������������t& �����UWVS�Ã��x@������x �T$��������$�C������ƅ�yw�$���������Ōx �����>�t& ����[^_]Í�&    d�    �    s�d�    �    ��t�IR��P�������XZd�    u������뮍�&    �CDt��T$���x ��������cD��i��������S�����    ���������x�[Ð�������P�����v �����UWVS�Ã����s,�$�v�����x �D$����������C0t�D$�$�x ������c0����������k(����   �v�Ōx ������щ$������ŉ����������tA�C��S��l$�C�S�ńx �C�S�C0��   �$��L$�   ������K0�L$����������v�Ōx �����>�t& ����[^_]Í�&    d�    �    s�d�    �    ��t�IRP�T$�������XZd�    u������묍t& �����������|���f��$��L$������L$�H�����&    f������UWVS�Ë@@�4@�����x �Ƅx ��������S�Kj �ǍCj �S j �K$�S(�K,��������CDu�ډ�������KD��[��^_]������t& �ډ�������׍t& ������UW��V��S�Ã��h@�Tm �����x �T$�$������T$�s�D$�{���x �CD��   �ډ��s������KD�C�Sj j j �щ�������T$�D$������Dm �Ōx �������>�t& ��[^_]�d�    �    s�d�    �    ��t�IR��P�������XZd�    u����