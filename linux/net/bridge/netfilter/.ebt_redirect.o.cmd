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
½¨ÌÖ<ØşI§Lv%V/œÊòB÷ï(ØÈı¿ÙW¹=°N„ÖZŸ ì©‹“èÕIÑpHˆZ"±ÖGÅ3z¤ìo –v„µÓ s¾‘{8@Ş&?\‘ªéó*ÜqÜìxÜ=v¬×ÿAìãÂò‰Ù6½ğwíR£=kkjâ<@º Æ!%ÛHÚ·¶„…öiŸD™Ö&"kåV¹ÁEf8öXAHx˜\¾Ç½Øò¥#1Qk_8dnyÔör3U·Ô½‘¤•wbmf ÿú“±?&€Y7¡Ú½K&µdá½«|ß­b^,¸EŒµªhëş•6k*x¹É­ê‰è1ü_%"]7Úmı‡ä.ê,lZò‡ÇC‘¬o„‚8¨ü/îØ»0V©Ğªİ–5öâşÑ¿ +JE‡“Yi³7~»¯Oc¡êkrŸé§`V'|KŞ3*hA<º}˜ŞùLå×sÅˆºÿ=éŒã%™ˆZ@¾‘™BpÁv8§ØRÌ|+vÌ½]µ·ñoª‘2ÿ}yùÙ€XÍ0Òİ-å.PØ<Ç®\0)	}»E´NG<Tl(¸èù?¬[¬î¡Ú™Ç¨TË=7T6$å´yèúnÕB,àËh´pÚĞç‡ñ¶$a+E—¹Dd‘RöIÜÙ:<`æŒâì]Ék›>—-¸ÚĞ_xw•û£á6˜G™wÉé¸P>¾èIâ<Ët­~~Qh½Ô§r²‡Vi/G»ÇÏ”ôzõæT×	T àŠ^{ã’Ò`xíŸcP´Í{©GğZé–äS‹;Œ£NÑÒİC;ªbuÎÁ,T@E­–ë3İÊN÷Xûì kË(D0Mù~Ş]ié-'NKİ¤@†U>ä´|;d$ãøÖÃABúHíi`Îd‰Ãv~ÑDĞƒÂ¸?   ‹t$ÇD$$    )Ğ‹u 9Ø‰÷NØ¡ u ‰T$‰\$‹\$‰D$9Ã×‚Ï  ´&    v ‹\$‹D$1ö‹T$‹|$‰Ù‹l$¥ÂÓàöÃ EĞEÆ‰é9ÇÑ‚B  )Ç‹5u ‰Ù‰|$‹=u Õ‰t$Lt ‰|$ Pt €t ¸ Êš;„t ¥÷1ÛÓæöÁ ‹Xt ‰l$EşEó5\t =`t 1Ò1í‰5\t ¥ÂÓàöÁ ‰ùEĞEÅ‰=`t 9Æ‰$Ñ‰T$‚   1í´&    v ¡¨t ‹¬t +4$‹8u |$ƒÀ‰5\t ƒÒ ‰=`t £¨t ‰¬t …É…Ã  èüÿÿÿ‰Ã…À…Ä  ‹5\t ‹=`t ‹$‹T$‰û9ÆÓs™¡u ‹u 	l$$‰D$¡ u ‰T$ ‹u ‰D$‰T$¶L$¡u 1Û‹u ¥ÂÓàöÁ ¶Œt EĞEÃt ”t ‰Æ‰×£t ‰”t ¸ Êš;1Ò1í¥ÂÓàöÁ EĞEÅ9Æ‰Á‰ø‰ÓĞrQ¡ìt ‹ğt ƒÀƒÒ ´&    f)Î‰$ßƒÀ‰T$‰ıƒÒ 9Îİsç‹$‹T$‰5t ‰=”t £ìt ‰ğt ‹\$¡ u 1ÿ‹$u ‹t$‰Ù¥ÂÓàöÃ EĞEÇ5u ‹|$ =u 0u ‰Ù1í¥÷ÓæöÃ EşEõ)ğú(u ,u ‹\$‹D$1ö‹T$‹|$‰Ù‹l$¥ÂÓàöÃ EĞEÆ9Ç‰ï×sƒë‰\$‹D$‹|$‹T$‹l$9ø‰Ğèƒ;ıÿÿ‹5 u ‹$u èüÿÿÿ1Ó1Æ	ó…³  ‹Tt ‰Ë+4u ‹=,u Áÿ;=(u ‰ú,u ‰ĞÁè£4u Ø‰Æ‰$1À)Î’À‰D$…ö„—   ‹|$‹l$‹ u ‹u ƒşÿ„'  ƒştE‰ğ‰ÚÁø¯Ö‰D$‹D$¯ÁĞ‰D$‰È‹\$÷æ‰Á‹D$Ó‰ò¯Õ¯ÇĞ‰D$‰ø‹l$÷æ‰ÇÕ…ö~‹t$…ö…Æ  ‹$u u )=\t -`t £Tt ‹@t ‹B…À…A  ‹Xt ¸ Êš;1Ò1Û‹=`t ‹5\t ¥ÂÓàöÁ EĞEÃ‰T$‰$…ÿˆÜ  ‹$‹T$‰ù9ÆÑrq1ít& ¡¨t ‹¬t +4$‹8u |$ƒÀ‰5\t ƒÒ ‰=`t £¨t ‰¬t …É…+  èüÿÿÿ‰Ã…À…4  ‹5\t ‹=`t ‹$‹T$‰ù9ÆÑs™	l$$ƒ@u ‹\$$¸@t ¾@t ‰Úè	àÿÿ¸Du ¹?   ‰Çó¥ƒ@u …Û•Ãé]úÿÿ´&    v Ç8u     éIüÿÿ‰Ç‹5¼t ½   ‹¸t Áÿ¨t ¡´t =¬t )Øú‰÷Áÿ‰Ñ‰ÂW¿ Êš;VD$Lèüÿÿÿ‹D$L‹T$P‹L$T‰D$4‰T$8‰L$<QRP¸@t èW×ÿÿ¡Øt ‹5\t )Ø£Øt ÷ïÀt Ät £Ğt ƒÄ‰Ôt ‹=`t é¦ûÿÿ¶    Ç8u     éáşÿÿ´&    f‰Ç‹5¼t ½   ‹¸t Áÿ¨t ¡´t =¬t )Øú‰÷Áÿ‰Ñ‰ÂW¿ Êš;VD$Lèüÿÿÿ‹D$L‹T$P‹L$T‰D$@‰T$D‰L$HQRP¸@t è—Öÿÿ¡Øt ‹5\t )Ø£Øt ÷ïÀt Ät £Ğt ƒÄ‰Ôt ‹=`t é6şÿÿ¹ÿÿÿÿ½ĞDÑƒÂ!ébùÿÿ„Û„øÿÿé¿øÿÿ¶    éSıÿÿ÷ÙƒÓ ÷Û÷ßƒÕ ÷İé ıÿÿ¹ÿÿÿÿ½ÂDÁƒÀ!éùøÿÿèüÿÿÿ‹0u ÿ5u 1ÿÿ5 u £ u ­Ğ‰$u ÓêöÁ EÂE×+u u èüÿÿÿ‹Tt ]‰ÃXé	üÿÿÇ8u    Æ×‰5\t ƒ¨t ÿ‰=`t ƒ¬t ÿé÷üÿÿ‹5Tt ‹z‰ó)û‰Ù÷ÙHË9Èƒ£üÿÿ€=    …–üÿÿé™¾  ‹D$éøÿÿèüÿÿÿèüÿÿÿUWVSƒì$‰D$ ‹-ÀÕ ‰èƒàkÀ4‹èÕ ‹ˆìÕ ˜ÄÕ ‹¸ÌÕ ‹°ÈÕ ‰T$‹ğÕ ‰L$‹ˆôÕ ‰T$‹ĞÕ ‰L$‹ˆÔÕ ‹€ÄÕ ‰$‰L$‹èüÿÿÿ+$T$‰Á!ú!ñ¬şÑï‰$‰ğ‰|$‹L$÷Ğ‹<$‰Æ¸    ÷Ñ!ş!Ñ	ñ‰Á‹sDÏEĞ‹C‹{ ‰$‹K‰D$‹ÀÕ 9ë…;ÿÿÿ‹$‰Ó¯Ù÷á¶L$ÚÆ×‹T$ 1À­şÓïƒá E÷Eø…ÒtF‹D$‹T$‹\$ ğú‰‰S‹\$ t$|$‰s‰{Ìu Ğu ‰C‰SƒÄ$[^_]Ãt& ¡    ‹   ëÂ´&    t& èüÿÿÿƒìd¡    ‰D$¡    …Àu‹D$d+    u0ƒÄÃt& kÀ<ÇD$    Ç       ‰$Áø‰D$‰àèúñÿÿëÃèüÿÿÿv èüÿÿÿ9Du t+S1É‰Ã‰Â¸Pø èüÿÿÿèüÿÿÿ1À9Du [•À÷ØÃ¶    1ÀÃt& èüÿÿÿ‹@u öÂu ¡Du ‹@D‹@u 9Êuãƒà Ã´&    v óëÑ´&    t& èüÿÿÿU‰åWV‹@u öÂu#¡Du ‹p‹x‹@u 9Êuà‰ğ‰ú^_]Ã¶    óëÎt& èüÿÿÿÇ     Ç@    Ç@    Ã¶    èüÿÿÿW|$ƒäğÿwüU‰åWVSìŒ   ‹Du d¡    ‰Eä1ÀE´èüÿÿÿèüÿÿÿèüÿÿÿ¸    èüÿÿÿƒ@u ‰…tÿÿÿ¡Du ‹èüÿÿÿ‰…xÿÿÿ‰Ñ‰•|ÿÿÿ‰Â‰Øèüÿÿÿ‰×	Ç…U  ‹E´‹U¸‹5 t ‹=$t ‰Ó9ğûŒ©  ‹M¼‰û+(t 9ÆÓ|…É  )ğ‰ËúÁûSQ‰Ñ‰ÂEØèüÿÿÿ‹EØ‹UÜ^_‰E€‰EÀ‹Eà‰U„‰UÄ‰EÈ‹u„‹]€…öˆ¹¾  ‹}È‰½pÿÿÿÿÿÉš;‡¹¾  ¸}Á%9Ø¸   ğŒ¹¾  ¶\u ‰ø¬u 5°u ™‰]¥úÓà‰u”‰5°u 1ööÁ EĞEÆ‰¬u ‰Æ‰×5`u ¸ Êš;=du 1Ò1Û‰5`u ¥ÂÓàöÁ ‰=du EĞEÃ‰Ó9Æ‰ú‰ÁÚrX‹E‹U”ƒÀƒÒ ‰E‰U”t& ‹E‹U”)Îß‰EˆƒÀ‰UŒƒÒ 9Î‰E‰øØ‰U”sÜ‹Eˆ‹UŒ‰5`u ‰=du £¬u ‰°u ¸ Êš;1Ò1Û‹5”u ¶u ‹=˜u ¥ÂÓàöÁ EĞEÃ9Æ‰Á‰ø‰ÓĞrZ¡ğu ‹ôu ƒÀƒÒ ‰E‰U”f‹E‹U”)Îß‰EˆƒÀ‰UŒƒÒ 9Î‰E‰øØ‰U”sÜ‹Eˆ‹UŒ‰5”u ‰=˜u £ğu ‰ôu ¡¸u +E€¾   ‹¼u ‹Àu U„+pÿÿÿ‰ËÁûSQ‰Ñ‰ÂEØèüÿÿÿ‹EØ‹UÜ‹Mà‰EÌQRP¸Du ‰UĞ‰MÔèÛÏÿÿ‹EÀ‹MÄº}Á%ƒÄ‹]È9ÂÎŒ   º Êš;iÉ Êš;÷âÊ‰Ù1ÛÈÚÌu Ğu £Ìu ‰Ñ‰Ğu ‰ÂE˜èüÿÿÿ‹E˜£øu ‹Eœ£üu ‹E £ v EÀèüÿÿÿ‹…xÿÿÿ‹•|ÿÿÿÇ,v     Ç0v     £Pu ‰Tu £„u ¸Du ‰ˆu º   Ç        è'×ÿÿ‹•tÿÿÿ¸    ƒ@u èüÿÿÿèüÿÿÿèüÿÿÿ‹Eäd+    uYeô[^_]gø_Ã¶    ‰Ñ‰ÂE˜èüÿÿÿ‹E˜‰EÀ‹Eœ‰EÄ‹E ‹UÄ‰EÈ‹EÀ‰U„‰E€éäüÿÿ´&    f¸ÿÿÿÿºÿÿÿéèşÿÿèüÿÿÿ´&    t& èüÿÿÿUWVSƒìd¡    ‰D$¸ t èüÿÿÿ¡$t  t …  ¡(t …À…ú  ¸    èüÿÿÿƒ@u ‰Ãèª×ÿÿ‹Pu ‹Tu Ç       ¡Du èüÿÿÿ€=,t  …Å  º   ¸Du èşÕÿÿ¡Du £às ¡Hu £äs ¡Lu £ès ¡Pu £ìs ¡Tu £ğs ¡Xu £ôs ¡\u £øs ¡`u £üs ¡du £ t ¡hu £t ¡lu £t ¡pu £t ¡tu £t ¡Du ‹èüÿÿÿÇàs @Ö £8t ¡Äu ‰<t hu ‹Èu lu £t ¸às ‰t ºÀÕ èZÏÿÿ¡|u º@Õ Çàs @Ö £äs ¡€u £ès ¡„u £ìs ¡ˆu £ğs ¡Œu £ôs ¡u £øs ¡”u £üs ¡˜u £ t ¡œu £t ¡ u £t ¡¤u £t ¡¨u £t ¸às èÉÎÿÿ¸    ‰Úƒ@u èüÿÿÿèüÿÿÿèüÿÿÿèüÿÿÿ‹D$d+    …S  ƒÄ1À[^_]Ãt& Æ,t éúıÿÿt& ‹\u ‹du ¡`u ‹5¬u ‹=°u +5 t =$t ­ĞÓêöÁ ‰ùEÂ+(t ™R‰òPD$èüÿÿÿ‹D$‹T$‹l$‰Æ‰×+5t ‰D$=t ‰é+t ‰T$‰È™R‰òQ‰ù|$‰øèüÿÿÿ‹D$‹|$$‹T$ ‰|$‰Ç÷ß‰ÑƒÑ ‰ş¿   ÷ÙHğHÊƒÄ9÷¾    Î}(‹4$‹|$‰-t ‰5t ‰=t é]ıÿÿ´&    v  t ‹t$$t 5(t ‰÷‰Ñ‰ÂÁÿWVD$èüÿÿÿ‹D$‹T$£ t ‹D$‰$t £(t XZéıÿÿèüÿÿÿ´&    ´&    èüÿÿÿ1Àè„ìÿÿ„ÀuÃ´&    éüÿÿÿv èüÿÿÿ1Ò       éüÿÿÿ´&    èüÿÿÿU‰åWVSƒäøƒì‰D$‰T$‰$‹@u öÃ…%  ¡hu ‹lu ‰D$¡Du ‰T$‹èüÿÿÿ+Pu Tu ‰×‹Hu ‹5Hu !Â¡Lu ‰T$‹T$!ø‹=Lu ¬şÑï‰ù÷Ö¿    ÷Ñ!Ö!Á	ñ‰ş‹Xu DòEÇ‹|$¯È‰ğ÷%Xu Ê‹\u `u du 1ö­ĞÓêöÁ EÂEÖD$T$‰Æ‰Ñ¡àu 9t<‰‹|$¡Äu ‹Èu ‰‰W‹<$¡Ìu ‹Ğu ‰‰W‹}¡Ôu ‹Øu ‰‰W;5èu ‰Èìu },¡@u 9Ã…Ûşÿÿeô‰ğ‰Ê[^_]Ãv óéÅşÿÿ´&    f¡Äu ‹Èu ‹|$ 6eÄƒÒÿ‰‰Wë¶fèüÿÿÿUWVS‰Ãì„   d¡    ‰„$€   ‹‰Ââ €  „â   ¨„Â  öÄ „É  öÄ„   ¸   èüÿÿÿ„À„Ã  ‹KT‹CP…Éˆ  ‹‰Öæ    …à  ¿?B 9ÇÎŒq  €æ„¸  iÀè  ‹sH‹{Ll$‰D$‰è‰t$‰|$èİæÿÿ‰Æ…À…e  ‹D$‹T$‰D$‹D$‰T$‰D$D$u…Òt=d¡    ‹€è  …Àt-‹…Éu'‹D$‹T$‹L$èüÿÿÿë´&    l$…À…œ  1À|$ ¹   ó«‰èèüÿÿÿ¸    èüÿÿÿƒ@u ‰$‹=Üu D$ ‰ê1í‰|$P‰ØL$èüÿÿÿ‰Æ‹D$Z9Çt7º Êš;£Üu ½   ÷êÄu Èu £Ôu ¸Du ‰Øu º   èœÏÿÿèüÿÿÿ‰Ñ£èu ‰Â÷Ğ‰Ï‰ìu ÷ÿÿÿ	øt‰Ğ‰Ê+Äu Èu £èu ‰ìu ‹$¸    ƒ@u èüÿÿÿd¡    ‹€è  …Àt‹ …Àtu÷@  u~‰è„À…Š   èüÿÿÿ‹„$€   d+    …ñ   Ä„   ‰ğ[^_]Ãt& l$éËşÿÿ´&    ¾ÿÉš;9Æ¾    ÎŒŒ   €æt×‹sH‹{Lé şÿÿt& D$ èüÿÿÿ÷@  t‚¸   è5èÿÿ	Å‰è„À„vÿÿÿ1Àèüÿÿÿéjÿÿÿt& ¸   ‰$èüÿÿÿ„ÀtT‹öÄ@„kıÿÿ‹sX‹{\¹Ğ  ‹$ÆØÜÿÿƒ×ÿ9ñúƒJıÿÿv ¾êÿÿÿé ÿÿÿ¶    ¸   èüÿÿÿ„Àt‹é"ıÿÿ¾ÿÿÿÿéûşÿÿèüÿÿÿffèüÿÿÿi     † ‹ v WVS‹¤v ‰Â1Àˆv Œv ÁÓ‰Şûç  vd‰ØºÓMb÷â‰×ÁïiÇè  )Æ‰ø‰ò£    ‰È¹è  ÷ñ‰ú‰ÁÀv [Äv +¸v ‰¸v ¼v ^‰=¼v £Àv _‰Äv Ãt& 1À1ÿë®´&    v èüÿÿÿ‹    ¹ÀÖ ¸    èüÿÿÿ1ÀÃv èüÿÿÿƒäÖ @Çv     ÇÜÖ  $ô ÇØÖ  $ô èÿÿÿ¡¸v ‹¼v Ç¨v     Ç¬v     £Àv ‰Äv ÇĞÖ ÿÿÿÿÇÔÖ ÿÿÿÃfèüÿÿÿ¡Àv ‹Äv U‰å]Ã´&    t& èüÿÿÿƒ=°v ¸ÿÿÿÿºÿÿÿtÃ´&    öäÖ tïU‹ÔÖ ‰åV‹5ĞÖ S»}Á%9ó»   Ë|¸ Êš;iÉ Êš;÷æÊ[^]Ãv èüÿÿÿW‰×V‰ÆSƒì‹°v d¡    ‰D$1Àƒû„æ  ø   ƒû„  ƒûu‹äÖ ƒã0„“  1Û¡ÜÖ ô  = $ô   £ÜÖ ¡¸v ‹¼v ‹5¨v ‹=¬v ‰$¡àÖ ‰T$‰úH‰ğ…ÿˆõ   ­ĞÓúöÁ t‰ĞÁú)Æ‰5¨v ‹5v ×$T$‰=¬v £Àv ‰Äv …öt5şô  Ö   şşÿÿŒâ  Çv     ‰÷1öğú£Àv ‰Äv ‹D$d+    …  ƒÄ‰Ø[^_Ãv …Û„8  ƒû…ÿÿÿöäÖ …ª   ÇĞÖ ÿÿÿÿ1ÛÇÔÖ ÿÿÿÇ°v     éäşÿÿt& ƒäÖ @ÇÜÖ  $ô éãşÿÿ´&    v ÷ØƒÒ ÷Ú­ĞÓúöÁ t‰ĞÁú÷ØƒÒ ÷Úéøşÿÿ¶    îô  Âô  £Àv ‰5v ‰Äv é1ÿÿÿ¶    Ç°v     é`şÿÿ‹ÔÖ ¡ĞÖ 1ú1ğ	Â…Fşÿÿéü¾  t& öäÖ  „)ÿÿÿ‹ÔÖ ¡ĞÖ 1ú1ğ	Â…şÿÿéÉ¾  ´&    ÇĞÖ ÿÿÿÿ1ÛÇÔÖ ÿÿÿÇ°v    éêıÿÿv ¡äÖ ¨…ƒ   ƒà ‰Ã„ÏıÿÿL$‰ğÇ°v    QƒÀ¹€Q ƒÒ èüÿÿÿ‹L$‰ğ‰ú‰ËÁû)ÈYÚ€Q ƒÒ £ĞÖ 1Û‰ÔÖ é‚ıÿÿv Æô  êô  £Àv ‰5v ‰Äv éşÿÿ¶    D$¹€Q Ç°v    P‰ğèüÿÿÿ‹T$‰ÑÁù)ÖÏÆ€Q ƒ× ‰5ĞÖ ^‰=ÔÖ éıÿÿèüÿÿÿèüÿÿÿƒì¸íÿÿÿƒÄÃ´&    ´&    èüÿÿÿWVSƒìd¡    ‰D$1ÀöäÖ @u¶„v ¨t‹D$d+    …`  ƒÄ[^_Ãt& ‰àèüÿÿÿ‹$‹\$‹D$ƒÁÿƒÓÿ eÍ™R‰Ê‰ÙPD$èüÿÿÿ‹L$‹D$‹T$[^ù?KL v]ƒÀƒÒ ùÀ~N;wOèüÿÿÿ¹}Á%9Á¹   ÑŒÎ   iÊ Êš;º Êš;÷âÊ /hYƒÒ j ‰Ñ‰Â¸`v j j èüÿÿÿƒÄéFÿÿÿv ‹    …Étk    <‰ËÁû)ÈÚ‰T$‹T$1É‰D$ÇD$    èüÿÿÿ‰Ãƒøí„ÿÿÿèüÿÿÿ‰Æ¸}Á%9ğ¸   Ğ|Q¸ Êš;iÊ Êš;÷æ‰×‰ÆÏ‰ğ‰ú cCÒ™   …Û„`ÿÿÿ‰ğ‰ú /hYƒÒ éOÿÿÿ¶    ¸ÿ.hYº   €é:ÿÿÿ¾ÿÿÿÿ¿ÿÿÿë¶èüÿÿÿ´&    ´&    èüÿÿÿöäÖ @u¶„v ¨tÃ¶    ‹    ¹ÀÖ ¸    éüÿÿÿ´&    t& èüÿÿÿU‰ÕWVS‰Ãƒì,‹‰L$‹|$@d¡    ‰D$(1À¡äÖ ‰$‰ÆöÆ€„  ¡v ‰D$Áø‰D$€æ „  ‹D$‹T$æ    ‰t$‰C¡ v ‰S‹¤v ‰D$‰T$‹=°v ÷$@  ¸   ‹T$DÇ‰|$ ¿»I‰Á‹D$¬ĞÁúiò»I÷çò‰×‰ş‰÷Áÿ…Òy÷ØƒÒ ÷Ú‰ĞÁú÷ØƒÒ ‰Æ÷Ú‰×¡ÜÖ ‰s‰{‰CÁø‰C¡ØÖ ÇC8   ‰C Áø‰C$‹$ÇC<    ‰C(¡àÖ ÇC@  ô‰C0Áø‰C4¡    ÇCD    ‰CX‹D$ÇC\    ‹8ÇC`    ÇCd    ‰»    ÇCh    ÇCl    ÇCp    ÇCx    ÇC|    Çƒ€       Çƒ„       Çƒˆ       ÇƒŒ       Çƒ       Çƒ”       Çƒ˜       Çƒœ       ‹E ‹U‰D$‰CH‰SL‹E‰T$‰CPÁø‰CT‹D$…Àu‹uºÓMb‰ğÁş÷êÁú)ò‰SPÁú‰ST‹U ‹E‹5ĞÖ ‹-ÔÖ ‰T$9ò‰ÂêÁ  ‹D$(d+    …C  ƒÄ,‰È[^_]Ã¶    ‹¬v ¡¨v ‰L$ …Ò…Á   ‹$‹5 v ‹=¤v á    ‰t$‰L$‰|$ºè  iL$ è  ÷âÊ‰×‰ÖÁÿ…Òy÷ØƒÒ ÷Ú‰ĞÁú÷ØƒÒ ‰Æ÷Ú‰×‹D$…Àt‰s‰{é¯ıÿÿf¹ÓMb‰ğÇC    ÷áÁê‰Sé’ıÿÿt& ‹C£v ‰D$èïõÿÿ‹D$‹T$‰GP‹D$‰WT‰GXÁø‰G\é1ıÿÿ´&    ‰‹D$ ‹¤v ‰w ‰G¡ v ‰W‰G‰ğÁø‰G$‹D$‹ ÇGD    ‰G0Áø‰G4¡    ‰G@‹öÁ…¢  öÅ t$    ‹$£äÖ öÅ…­  ‹$%    ‰D$öÁt^¸  è÷cis  è‰D$¸ ¡ ‰T$1Òt$;T$‰Æt$MT$¾à^øÿMD$‰T$ƒú ‰Âòº    MT$LÆ‰ v £¤v öÁt‹C£ÜÖ öÁt‹C £ØÖ öÁ t%‹C0‹t$P…öDÂº
   9ĞOÂ1Ò…ÀHÂ£àÖ öÁ€t‹S0¸ † 9Ğ¸    C4ƒµ  öÁ…ô   ‹5¬v ¡¨v ‰t$ öÅ@t‹SX‰    ‹á@  …á  ‹L$ ‹ v ‰G‰O‹¤v ‰T$‰O‰L$‹$‰W‰O(Áù‰O,‹L$‹‹    ÇGL    ‰W8Áú‰W<‰OHéıÿÿt& ƒ|$ „å  ‹T$ ƒú…ˆ  ö$ „ıÿÿGÿ‹T$¹   ‰ƒ    ‹D$ƒÀƒÒ ‰CH‰SLéöüÿÿ´&    $$ÿßÿÿ‹$ÇD$    £äÖ éCşÿÿt& ö$„ÿÿÿ‹T$‹C…ÒuºÀ½ğÿ9ĞLÂº@B 9ĞOÂiÀè  º ›2â9Ğ‰ÖMğ¸ eÍ9ÆOğèüÿÿÿ+˜v ‰ò‰$Áú‰t$‰T$öäÖ €…x  èüÿÿÿ‹$‰œv ‹äÖ £˜v ‰Ğ€ä¿£äÖ ùÿ   ,  ƒâuù     L$$€Ì@‹T$‰L$ £äÖ ‹D$Q‹L$¤ÂÁàèüÿÿÿ‰D$‰T$ X¡àÖ H¸   Óà‹$9ÈOÁ¹   +àÖ É÷î1ö¥ÂÓàöÁ EĞEÆ‰T$‹T$‰$‹D$ v ¤v ‰Ñ‰Â¸ ¡ $L$1ö9Ö‰ÆÎ¾    MòMÁ¹à^øÿ‰Âƒş Êº    MÖLÁ¹è  ‰ v ‹T$ÿt$ £¤v 1Àèüÿÿÿ‹5äÖ ‹‰T$$‰t$æ    ‰¬v ‰t$Z£¨v égıÿÿt& ‹C(‰Âƒâ÷Æ   „„   …Òu&Ç°v     ¾@   ÇĞÖ ÿÿÿÿÇÔÖ ÿÿÿ‰4$‹$0äâ ÿ  	Â‰$‰äÖ éüÿÿ´&    ‰D$èoñÿÿ‹D$éıÿÿ¶    ƒú…–úÿÿ‹T$1è1ò	Ğ¸   DÈéúÿÿ…ÒtŸèüÿÿÿ‹£˜v ¡äÖ ‰œv ‰$‹C(é€ÿÿÿ´&    v ö$„EúÿÿG‹T$¹   ‰ƒ    ‹D$ƒÀÿƒÒÿ‰CH‰SLéúÿÿ´&    ‹D$‰‹é>üÿÿv D$$ÇD$    ÇD$    ‰D$ éúıÿÿèüÿÿÿ%äÖ ÿ¿ÿÿ£˜v D$$‰œv Ç$    ÇD$    ÇD$    ‰D$ é¼ıÿÿèüÿÿÿffffèüÿÿÿ‹èüÿÿÿ£ğv ‰ôv Ã´&    èüÿÿÿU½    W‰ÏVSƒì‹t$4‰D$‰È‰T$÷æ‰Ğ1Ò‰Á‰ğ‰Ó÷ç€£   ‰ø1Ò»    ÇD$    Ñè‰T$‰D$‹D$0‰$‰D$ë	v ƒ,$tV¶$‹D$1Û‹T$¥ÂÓàöÁ EĞEÃD$T$1Û‰Á…Òt‰Ğ1Ò÷÷‰Ã‰È‰é÷÷‰Ú‰Ö‰Ã­ÓÓî1ÒöÁ EŞEò‰Ù	ñu¤‹|$‹$‰‹D$‰ƒÄ[^_]Ãt& ¬ÙÑëƒí‰Ø	È„Jÿÿÿ¬ÙÑëƒí‰Ø	ÈuŞé6ÿÿÿèüÿÿÿUWV‰ÆSèüÿÿÿ‹(w …Ò„Â   ‰Å¡”Ù =”Ù „°   ¡”Ù xĞ=”Ù „   ¡dw ‰ñ„Ét9ø„ª   …ít
öGD „œ   …ÿtxº    ¸@w èüÿÿÿƒø ‡”   „¿  ‰û…Àt=¡”Ù XĞ=”Ù tZ‰ğ„Àt9dw t?‹C,º@w èüÿÿÿ…Àu.‹CD¨ u…í…#¿  9dw t‰Øèüÿÿÿ…À„E¿  [^_]Ãv ‹C0XĞ=”Ù u¦‰ûëĞ´&    ‹W0zĞú”Ù …6ÿÿÿëÌé_¿  ´&    èüÿÿÿh6Ñ  jÿj hÀI èüÿÿÿƒÄ= ğÿÿwéüÿÿÿ´&    fÃ´&    ´&    èüÿÿÿU‰ÅW‰Ç¸ w VSƒìèüÿÿÿ‹5w ‰Ã‰è„À„$  ‹”Ù Ç$    Çw     BĞú”Ù „&  ‰ú‹$‰ß1í‰Óv ö@Du9Æu„Ûu…Ét‹Q89P8~‰Á½   ‹P0BĞú”Ù uÑ‰è‰û„À„í   ‰w …É„Ï   9Ît%‹xÙ B úxÙ tf‹P`ƒ`DïB úxÙ uî‹5üv …öu\…ÉtX¡xÙ =xÙ tLj 1ÉºB ¸w j èüÿÿÿ¡    Zº    Yô  £w ¡    …Àtó¼Ğ¸w èüÿÿÿÇüv    ƒÄ‰Ú¸ w [^_]éüÿÿÿt& ‹”Ù BĞú”Ù t‰4$ééşÿÿ‰ñ…ö…_ÿÿÿt& ‰5w ‰ñéMÿÿÿv ‹$éÿÿÿèüÿÿÿU¸€Ù W‰ÏVSèüÿÿÿ¡”Ù =”Ù tXĞ1ö½   t& èüÿÿÿ…ÀtöCD t#‰èº    ÿs,)ğhNÑ  HÂP7PèüÿÿÿÆƒÄ‹C0XĞ=”Ù uÁ»   ¸    )óHØ÷¸€Ù èüÿÿÿhRÑ  SWèüÿÿÿƒÄğ[^_]Ã»   1öë×´&    èüÿÿÿS¸€Ù ‰Ëèüÿÿÿ¡dw ÿp,hTÑ  h   Sèüÿÿÿ‰Ã¸€Ù èüÿÿÿ‰ØƒÄ[Ãv èüÿÿÿU‰ÅWVSƒì(Çèv     Çäv     èüÿÿÿ¸   èüÿÿÿ‹5`Ù …öˆQ  Çàv     ¸    „Ş   ‹    ƒú†Ï   ‹    …Û…G  ¿    »    èüÿÿÿ9Ãu‰øº    èüÿÿÿ‰Ç‰Ã;    ƒ2  ğ«=àv ¡    ¿   9ÆGğƒşëbfó¼Ğ‰Ğ9ÓvFğ«àv ƒÇ9÷tGèüÿÿÿ‹    1Ò÷óBÿº    èüÿÿÿ9ÃwÑ¡    …Àu¾º    ¸    9ÓwºƒÇ9÷uÀ´&    èüÿÿÿğ³àv ¡àv èüÿÿÿ…À„!  èüÿÿÿ‰D$$hàv ÿ5    Pÿu,hD èüÿÿÿ‹    ÇD$ÿÿÿÿƒÄÇD$ÿÿÿÿÇD$ÿÿÿÇD$    ÇD$    ¶    ‹$ºàv èüÿÿÿ‰$9Øƒp  ‹<$9|$$tß‹U ‰èèüÿÿÿ‰é‰D$‰ø‰T$º6 j‰|$èüÿÿÿ‹U ‰èèüÿÿÿ‹}‹u‰T$$‹ôv ‰D$ ¡ğv ‰Ó;D$\$Y…ûˆÃ   ‹L$‹\$ 9ÁÓ…ûˆß   ‹L$‹\$ +L$\$‰È‰Ú‰ñ‰û¬ùÑë!ğ!ú‰Î‰Ù¿    ‹\$÷Ö÷Ñ!Æ!Ñ	ñ‹t$EÇE×1ÿ¯U‰Ñ÷eÊ‹M­ĞÓêöÁ EÂE×‰÷‹t$9Ã×‰ß‹\$Lø‰|$‹|$Lú9Ø‰|$‰×÷‰ß‹    Lø‰ĞMD$‰|$‰D$éÇşÿÿ´&    ‹<$ğ«=äv ¡ğv ‹L$‹ôv ‹\$ 9Á‹}‹uÓ…û‰!ÿÿÿ‹$ğ«èv ‹u‹}éÿÿÿf¸   èüÿÿÿd¡    …À„   èüÿÿÿ‹èv …Ò…}¿  ¡äv …À…¥¿  ƒÄ([^_]Ã¸   èüÿÿÿd¡    …Àtjèüÿÿÿÿu,hàC èüÿÿÿ[^ƒÄ([^_]Ã¡    £àv èüÿÿÿğ³àv ¡àv éıÿÿ´&    ó¼Û‰ßé¸üÿÿt& èüÿÿÿéfÿÿÿ¡àv éTıÿÿèüÿÿÿë´&    t& èüÿÿÿ‹`Ù …ÒuÃéûûÿÿ´&    t& èüÿÿÿV‰ÂSƒì‹5`w „Àt
Ç`w     ¡”Ù XĞ=”Ù uë\´&    ‹C0XĞ=”Ù tH9Şu„ÒuëöCD€tå‹CP…À…À  ‹KT…É…À  ¡`w …Àt‹@89C8~¾‹C0‰`w XĞ=”Ù u¸ƒÄ[^Ã´&    ¶    èüÿÿÿWVS‰Ãƒì;w „¯   9dw „Ã   ;`w „ß   ¸ w èüÿÿÿ‰Â¡w 9ÃtöCDu,‹s0‹K4C0‰N‰1‰C0‰C4¸ w èüÿÿÿ1ÀƒÄ[^_Ãt& ‹{`‹sdK`‰w‰>‰K`‰Kd‹üv …Ét¶…Àt¡xÙ =xÙ u¦¸w ‰$èüÿÿÿ‹$Çüv     ëŠ¶    ¸   èş÷ÿÿ;w …;ÿÿÿ¸ğÿÿÿë‡v ¸   è~öÿÿ9dw tä;`w …+ÿÿÿ´&    v ¸   èFşÿÿéÿÿÿèüÿÿÿS‰Ã¸€Ù èüÿÿÿ‹S0C09Ât‰Øè½şÿÿ‰Ã¸€Ù èüÿÿÿ‰Ø[Ãt& 1Û¸€Ù èüÿÿÿ‰Ø[Ã´&    èüÿÿÿ‹PD‹H0ƒâÏƒÊ@‰PDP09Ñt5‹PX…Òtèüÿÿÿ¡(w …ÀuÃ´&    ‹    ¹hÙ ¸    éüÿÿÿv Ç@8    Ã´&    èüÿÿÿU¸ w WVSƒìXèüÿÿÿ‹-üv …í„{  ¡øv ‰D$L¡xÙ h =xÙ „*  ‰l$@ë1´&    v ‹5(w …ö…ú  ‹D$@‹@`x ‰|$@=xÙ „ó  ‹D$@‹@Dƒà@‰$uÊ´&    v ú¡w ‹èüÿÿÿ‰D$$‹D$@‰T$(‹èüÿÿÿ‰D$D¡w ‰T$H‹èüÿÿÿ‰D$¡w ‰T$‹èüÿÿÿ‰D$,‰T$0û‹-w ‹D$+D$$‹T$‹u‹}‰ÁT$(‰ğ!ñ¬ø‰L$‰Ñ‰ú‰ÃÑê!ù‹D$÷Ó‰\$‰Ó#D$÷Ó‰\$4!Ë	Ã„C  ‹$ƒø‡Ï  9    †Ã  ‹D$@‹@D‰$¨„Q  ‹øv …É…C  ‹\$@‹=w ‹Cp‹w‰|$‹St‹‰D$‹D$$+D$‰T$ ‰Å‹T$(T$ !õ¬ş‰l$‰Õ‰ò‹t$!ı÷ÒÑï‰Ğ‰ú‹|$÷Ò!ğ!ê	Â‹W‹D$DEñEé‰T$4‹W‰t$‹{‹sl‰l$8‹k‹[h‰T$‹T$H‰t$T)Ø‰\$Pò‰ş‰Ó‰ê!Æ‰øÑê¬è!ë‰T$0‹T$0÷Ğ‰ß!ğ÷Ò!Ú‹\$@	Â‹T$D‹D$$EñEù‹K‹k‰Sh‹T$(‰L$,‹L$H‰Cp¡øv ‰Kl‰St…À…öıÿÿ‹L$8‹D$¯L$÷d$Ê¶L$4­ĞÓêöÁ t‰Ğ1Ò‰D$‰è¯ı¶L$,‰T$÷æ1öú‹|$­ĞÓêöÁ EÂEÖ1í‰D$‰T$‹W ‰ïS ‹\$+\$‹t$‰Ùt$‰ğ÷ÙƒĞ ÷ØHËHÆ9ÊÇŒ  ‹D$@9dw „ø  ‹$ƒà!ƒø…Jıÿÿ¡w ö@D„;ıÿÿ‹4$‹|$@‰ğƒÈ ‰GD¡(w …À„ıÿÿ9=dw „ù  ‰ğ   ‰GDv ‹    ¹hÙ ¸    èüÿÿÿéìüÿÿ¶    ‹U‹E¯M‰T$1Ò‰D$8‹D$‰T$<÷eÊ¶L$­ĞÓêöÁ t‰Ğ1Ò¹ † ‰D$9Á‰T$Órıÿÿ‹L$,‹\$0+L$\$‰Ú‰È‹\$4!ú‹|$!ğ!Ó!Ç	ûu<‹t$8‹L$<¯Ö¯ÈÑ÷æ1öÊ¶L$­ĞÓêöÁ EÂEÖ‰Æ¸PÃ  ‰×9ğÓŒ'  ƒ$‹$;    †Vüÿÿ‹],èüÿÿÿÿ4$ÿt$ÿt$SPh F èüÿÿÿ‹D$Xè>ûÿÿƒÄéôûÿÿ‹|$L…ÿtğÿøv d¡    º    èüÿÿÿ;    sM‹w …Ò„Š   ƒÄX¸ w [^_]éüÿÿÿ‹$‹|$@‹T$(ƒÈ‰GD‹D$$‰Wt‹T$H‰Gp‹D$D‰Wl‰Ghé€ûÿÿ‹    ¸    …Òt¤ó¼Â‹w …Òu¤ë,‹P\‰Ç…Ò„ûıÿÿèüÿÿÿ‹GD‰$‹$ƒà!ƒø…:ûÿÿéëıÿÿw ô  ‰Â¸w èüÿÿÿé[ÿÿÿèüÿÿÿéûÿÿ‹],èüÿÿÿWVSPhüE èüÿÿÿÿt$$ÿt$$‹D$\ÿp,hPF èüÿÿÿƒÄ$éÙúÿÿ‹|$@‹_,èüÿÿÿSPhG èüÿÿÿ¡w ÿpÿpÿt$4ÿt$4ÿt$Dÿt$Dÿt$<ÿt$<ÿp,h€G èüÿÿÿƒÄ4ÿwÿwÿt$\ÿt$\ÿt$Xÿt$Xÿt$(ÿt$(ÿw,hàG èüÿÿÿ¡dw ƒÄ(9ø…*À  é`À  ‹],èüÿÿÿÿ4$SPh¨E èüÿÿÿƒÄéûÿÿèüÿÿÿ¡dw UWVS…Àt‹PDâ@  ú@  „7  ¸ w 1Ûèüÿÿÿ‹xÙ ‰Æ‹J x úxÙ u.éÀ   öÄt€äş»   ‰AD‹G`W`‰ùƒè`ÿÙ „™   ‰Ç‹AD¨@tĞ‹Y`‹Ad½”Ù ‰C‰‹A4‰Q`‰Qd‹Q0‰B‰¡”Ù ÇA0   ÇA4"  PĞ‰ÃÇA8    =”Ù uë"´&    ‹B0‰İPĞ=”Ù t	‰Ã‹B8…Àyè‹E Q0»   ‰P‰A0‰i4‰U ‹ADéIÿÿÿ´&    f‹üv …Òt¡w …Àt%¡xÙ =xÙ t‰ò¸ w èüÿÿÿ‰Ø[^_]Ã¶    ¸w èüÿÿÿÇüv     ëÑ‹`Ù …Ò„»şÿÿèßñÿÿé±şÿÿ´&    v èüÿÿÿ¸€Ù èüÿÿÿèlşÿÿ…Àu¸€Ù èüÿÿÿ1ÀÃv 1ÀèÁíÿÿ¸€Ù èüÿÿÿ1ÀÃt& èüÿÿÿUWV‰Æ¸€Ù S‰Óèüÿÿÿ¸ w èüÿÿÿ‹V0‰Å‹F4‰B‰¡”Ù ÇF0   ÇF4"  ‰^8=”Ù tjPĞ‰Á¿”Ù ë´&    f‹B0‰ÏPĞ=”Ù t‰Á;Z8~ê‹V0‰P‰F0¸ w ‰~4‰‰êèüÿÿÿ1Àèíÿÿ1Àèpîÿÿ1Àèùôÿÿ¸€Ù [^_]éüÿÿÿ¿”Ù ë¼t& èüÿÿÿVS‰Ã¸ w èüÿÿÿ‰ÆöCD@u‹S0C09Ât
‹S`C`9Ğt‰Øèºöÿÿ‰ò[¸ w ^éüÿÿÿt& ‹xÙ £xÙ ‰B‰S`ÇCdxÙ ëÍ´&    ´&    èüÿÿÿS‰Ã¡`w …Àt+9Øt;‹PH…Òtèüÿÿÿ…Àu¡`w ‹èüÿÿÿ£ w ‰$w [Ãf€=     „uÀ  [Ã‰ w [‰$w ÃfèüÿÿÿU‰åWVS‰Ãƒì‰D$‰Ğ‰Ê‹`w …É„  9Ù„ò   ‹‰Èèüÿÿÿ‹ w ‹$w 9Á‰ß×r5¡`w 1ö1ÿ‰D$‹D$9D$t‹PL…ÒtèüÿÿÿƒÄ‰ğ‰ú[^_]Ã¶    ‹=`w ‰|$)È‹|$Ú‹\$‹w‹‹K‹[!ú!ğ¬şÑï‰$‰ò‰L$‰ù÷Ò‰\$‹$÷Ñ!Â!Ù	Ñu~¶L$÷d$‰Æ‰×1ÀÓï­ÖöÁ E÷Eø‰Ø…Û„gÿÿÿ¹    +L$1Û÷d$¥ÂÓàƒá EĞEÃÆ×éAÿÿÿ´&    f‹ w ‹$w 9Á‰ß×r<ƒÄ1ö1ÿ[‰ğ‰ú^_]Ãt& ¶L$1À÷d$‰Æ‰×1ÀÓï­Öƒá E÷Eøéìşÿÿt& ‹|$‰|$éÿÿÿv èüÿÿÿ¡˜Ù =”Ù t'SXĞv ‹SP…Òt‰Øèüÿÿÿ‹C4XĞ=”Ù uå[Ãv Ã´&    èüÿÿÿ¡”Ù S=”Ù t!XĞv ‹ST…Òt‰Øèüÿÿÿ‹C0XĞ=”Ù uåğÿøv [Ãt& èüÿÿÿğÿøv Ãv èüÿÿÿU‰åWVS‰ËÁƒì‹u‹}‰D$¸ÿÿÿÿ‰T$ºÿÿÿÿÇD$    ‰$…Òt
‰Ğ1Ò÷ñ‰D$‹$÷ñ‰ù‹T$9ÆÑCğCú‹D$‰ù)Ø1Û¯È÷æÊ¶L$­ĞÓêƒá ‹MEÂEÓ…Ét‹]‰3‰{ƒÄ¬Ğ[Ñê^_]Ã´&    èüÿÿÿU‰ÍWV‰ÆSƒì…É…  ‹@ …À…Å  ÇF PÖÜ‹^¸   1ÿ÷ã‰$‰Ñ…Òt¿…ëQ‰Ğ÷ç‰Ğ‰×ÁèÁïkÀd)Á‰Ê‹$¹d   ÷ñ‰ú‰F‰Á…í„ˆ  ‹nëQ´&    fÑëƒí¸   1ÿ÷ã‰^‰n‰Ñ‰$…Òt¿…ëQ‰Ğ÷ç‰Ğ‰×ÁèÁïkÀd)Á‰Ê‹$¹d   ÷ñ‰ú‰F‰Á‰ØÈr²9Ër®‹F‹V‰$F$‰T$‰êP‰Øÿt$ÿt$èüÿÿÿR‰FP‰Vÿv(ÿv$ÿt$ ÿt$ ÿv,h˜I èüÿÿÿƒÄ4[^_]Ã‹@‰×‹V1Û‰$‰T$‰Á…Òt‰Ğ1Ò÷õ‰Ã‰È÷õ‰Ú1Û‰Á…Ò…¿   ‰È÷÷‰Ú‰Ó‰Ñ‰ú	Ãt/ƒ|$ †´   ºX  9Âº    Êƒ    i×X  ´&    v F^‰é‰$¸ Êš;R1Ò÷÷P‹T$‰Øèüÿÿÿ‹F Y[…ÿ„]şÿÿ…Àu&¯ı1Ò¸ Êš;÷÷º@ =? FÂ‰F éGşÿÿt& =? ‡7şÿÿé0şÿÿ‰ØÈr*‹néÊşÿÿ´&    ‰Ğ1Ò÷÷‰Ãé4ÿÿÿv ¯Ç‰Âéfÿÿÿ€=Ÿ    uÍÿv,hXI ÆŸ   èüÿÿÿX‹N‹n‹^Zéyşÿÿ´&    èüÿÿÿUW‰ÏV‰ÖS‰Ãèüÿÿÿƒ{<‡  ‹C@ƒø‡ŒÀ  ‰ù‰ò‰Øèüÿÿÿ¸€Ù èüÿÿÿ¸ w èüÿÿÿ‰Å¡”Ù PĞ‰Á=”Ù „æ   ‹{8¾”Ù ë¶    ‹B0‰ÎPĞ=”Ù t‰Á9z8}ê‹S0‰P‰C0C`‰s4‰‹SD‰C`‰CdöÂ„}   ‹xÙ £xÙ ‰BƒcDï‰S`ÇCdxÙ ‰ê¸ w èüÿÿÿ1Àè6æÿÿ1ÀèçÿÿöCD€t-‹sP…ö…©À  ‹KT…É…©À  ¡`w …Àt‹@89C8~‰`w ¸€Ù èüÿÿÿ1À[^_]Ãv öÂtšƒÊ ‰SDë’v ÇC<    éØşÿÿ¾”Ù é8ÿÿÿèüÿÿÿUWVSYÿƒìƒû‡ˆ   ‰Æ‰È€|ÿ
tu‰ËƒûrN‹j‰×ƒåü‰
‹Lü)ï‰Lü‰ñ)ùßƒçüƒÿr‰$ƒçü1ö‹1‰D5 ƒÆ9şrò‹$ÚÆ ƒÄ[^_]Ãt& …Ûtë¶ˆ
öÃtá·Lşf‰LşëÕ¶    …ÛtÍë‡f¸êÿÿÿëÅ´&    fèüÿÿÿV‰ÈSƒì$d‹    ‰T$ 1Ò‹t$0‰â‰ñèüÿÿÿ‰Ã…ÀxD¸€Ù èüÿÿÿ¡”Ù XĞ=”Ù t‹C,‰âèüÿÿÿ…Àt5‹C0XĞ=”Ù uå¸€Ù »íÿÿÿèüÿÿÿ‹D$ d+    u%ƒÄ$‰Ø[^Ãv ‰Øèíÿÿ‰Ã¸€Ù èüÿÿÿ…ÛDŞëÎèüÿÿÿ´&    fèüÿÿÿS¸€Ù ‰Ëèüÿÿÿ‹L$‰Øº@w èüÿÿÿ‰Ã…Àx1Àè1äÿÿ¸€Ù èüÿÿÿ‰Ø[ÃfèüÿÿÿU¡    1Ò‰å]Ã´&    ´&    èüÿÿÿU‰åWVët& ó‹    öÂuó‹5    ‹=   ‹    9Ñuß‰ğ‰ú^_]Ãt& èüÿÿÿU¹   ºÓMbW¿€w V¾ Ù Só¥‰Çˆô  ‰ÆÁÿÁæÇ¬w »Ñ  ¤Ç‰ÈÁùƒ¸w ÷ê‰Ğ‰ÓÁøÁû)È)Ë™ğú1ö‰Á…Òt‰Ğ1Ò÷ó‰Æ‰È÷ó‰ò‰ÁÑè1Ò  ÊšƒÒ;1í‰Æ…Òt‰Ğ1Ò÷ñ‰Å‰ğ÷ñ‰ê1ÉÁàº   £Œw ¸€w èüÿÿÿ1À[^_]ÃffffèüÿÿÿÃ´&    v èüÿÿÿWV‰ÖS‹XD‹Bt\ÇÿÿÿÿÆC ‹~‹6‰ø	ğt9¸ÿÿÿÿë‹º    èüÿÿÿ‰;    r€{ u4ÇÿÿÿÿÆCƒÆÿƒ×ÿ‰ø	ğuÎ‰Ø[^_Ã´&    èüÿÿÿ‰C‰Së—v 1Û‰Ø[^_Ã´&    èüÿÿÿ‹D$‹T$…ÀtL$èüÿÿÿÃt& D$PRèüÿÿÿXZÃfèüÿÿÿU‰Å‰ÑWVSƒì<‹D$P‰T$8‹T$T‰D$,¸        ‰T$0‰D$4‰ÆQhÓÑ  Uèÿÿÿ‰ğƒÀ8‰D$ƒÄÇD$(    ‰,$´&    ÿt$(hÜÑ  ‹|$Wè]ÿÿÿ‹t$VhèÑ  WèMÿÿÿÿvhüÑ  Wè?ÿÿÿƒÄ$ÿ5    hÒ  Wè+ÿÿÿÿvh(Ò  Wèÿÿÿÿv$ÿv h<Ò  WèÿÿÿƒÄ(hVÒ  Wèşşÿÿ‹D$4F ‰D$‹‹T$8V$1í1Û‰T$èüÿÿÿ‹N‰D$X‰ÎZ‹|$‹…ö„ô   t& ‹V‹N‹~‰T$‹V‰L$ ‹N‰T$‹T$‰L$¶N$ˆL$'èüÿÿÿWVShfÒ  ‹\$Sè~şÿÿ¶|$;WhwÒ  SèmşÿÿƒÄ h€Ò  ‰\$Sè[şÿÿ‹L$$‹\$(‰È‰Ú+D$T$RP‹t$$‹|$(‰ğ+D$‰úT$ ƒÅRPS1ÛQWVhDJ ÿt$,èşÿÿ‹|$4ƒÄ0‹èüÿÿÿ‹w‰D$…í„)ÿÿÿ…ö„!ÿÿÿ‰ğèüÿÿÿƒÃ…Àt9ërğ‹|$‰Æ‹…ö…ÿÿÿ‹T$èüÿÿÿƒD$(‹D$(ƒD$(ƒø…@şÿÿ‹,$‹t$4ÿv$ÿv h‚Ò  hÒ  Uè“ıÿÿ¶F1ÒRƒàPh¤Ò  h°Ò  Uèxıÿÿ‹F1ÒƒÄ(RPh¿Ò  h°Ò  Uè^ıÿÿ·F1ÒRPhÉÒ  h°Ò  UèFıÿÿ·F1ÒƒÄ(RPhÔÒ  h°Ò  Uè+ıÿÿ‹F1ÒRPhİÒ  h°Ò  Uèıÿÿ‹D$`ƒÄ(èüÿÿÿ1Ò‰Ã‹@,RPhëÒ  h°Ò  Uèïüÿÿÿs8ÿs4hõÒ  hÒ  UèÙüÿÿ¶C01ÒƒÄ(ĞèRƒàPhÿÒ  h°Ò  Uè¹üÿÿ‹CD1ÒRPhÓ  h°Ò  Uè¢üÿÿ‹CH1ÒƒÄ(RPhÓ  h°Ò  Uèˆüÿÿ‹CL1ÒRPh$Ó  h°Ò  UèqüÿÿƒÄ(ÿsTÿsPh0Ó  hÒ  UèXüÿÿÿs\ÿsXh?Ó  hÒ  UèBüÿÿƒÄ(ÿsdÿs`hMÓ  hÒ  Uè)üÿÿÿslÿshh[Ó  hÒ  UèüÿÿƒÄ(ÿstÿsphjÓ  hÒ  Uèúûÿÿ‹Cx1ÒRPh{Ó  h°Ò  UèãûÿÿƒÄ(ÿ³   ÿ³Œ   hˆÓ  h°Ò  UèÄûÿÿÿ³˜   ÿ³”   h“Ó  hÒ  Uè¨ûÿÿ¡    1ÒƒÄ(RPh Ó  Uè‘ûÿÿ‰l$`ÇD$d€Ò  ƒÄL[^_]éyûÿÿ´&    fèüÿÿÿWV‰ÖS‹|$‰ÃQhÀÓ  PèTûÿÿƒÄ…ÿˆ  WhíÓ  Sè=ûÿÿƒÄhÔ  Sè/ûÿÿXZ…ö„u  ÿv`hÔ  Sèûÿÿÿvÿvh"Ô  Sèûÿÿÿv ÿvh9Ô  SèõúÿÿƒÄ,ÿv$hPÔ  Sèäúÿÿÿv(heÔ  SèÖúÿÿÿv,hzÔ  SèÈúÿÿƒÄ$ÿvÿvhÔ  Sè´úÿÿÿvh«Ô  Sè¦úÿÿ‹FDƒÄ…ÀtPhÁÔ  SèúÿÿƒÄ‹F8…ÀtPh×Ô  SèzúÿÿƒÄ‹F<…ÀtPhíÔ  SèdúÿÿƒÄ‹F@…ÀtPhÕ  SèNúÿÿƒÄ‹FH…ÀtPhÕ  Sè8úÿÿƒÄÿ6h0Õ  Sè(úÿÿh€Ò  Sèúÿÿÿv4hFÕ  SèúÿÿƒÄ …ÿyh€Ò  SèıùÿÿXZ[^_Ã´&    ‰øèüÿÿÿº¹Ó  …Àt‹P`Rh\Õ  SèÑùÿÿƒÄëÄt& hÛÓ  Sè½ùÿÿYXé|şÿÿ¶    hÔ  Sè¥ùÿÿY[[^_Ã´&    ´&    èüÿÿÿS‰Ãèüÿÿÿ‹Hjÿ‹‰ØèşÿÿèüÿÿÿPÿ5    hoÕ  Sè^ùÿÿèüÿÿÿPÿ5    hxJ SèGùÿÿƒÄ$h€Ò  Sè9ùÿÿXZ[Ãt& èüÿÿÿWVS‹2‰Ã¶Bƒşÿt;„Àt‰ğèüÿÿÿ‹HV‹‰Øè“ıÿÿX1À[^_Ãt& ÿr‰Øÿr‰òèùÿÿ1ÀZY[^_Ã„Àt‰ØèEÿÿÿ1À[^_Ã´&    ‹r‹zhŠÕ  Sè·øÿÿjh¤Õ  SèªøÿÿWVhÁÕ  SèøÿÿƒÄ$h€Ò  Sèøÿÿ1À[^[^_Ã´&    èüÿÿÿS‹XDº    ƒƒQ ‹èüÿÿÿ‰;    r€{ uÇÿÿÿÿÆC‰Ø[Ã1ÛëøfèüÿÿÿWVSèüÿÿÿhŠÕ  ‰Æ‰×j è#øÿÿjh¤Õ  j èøÿÿWVhÁÕ  j èøÿÿƒÄ$h€Ò  j èø÷ÿÿ[»ÿÿÿÿXë´&    W‰Ú1ÀVèøÿÿZY‰Øº    èüÿÿÿ‰Ã;    rİ1À»ÿÿÿÿè)şÿÿë´&    ‰Øèüÿÿÿ‹HS‹1Àè,üÿÿX‰Øº    èüÿÿÿ‰Ã;    rÕ[^_ÃèüÿÿÿUW‰ÏVSƒì(d‹    ‰L$$1Ét$‹\$<¹€Q VèüÿÿÿY|$‰Æ‰ù‰œ  v ƒîÁ€Q xõ‰ÈºÅ³¢‘½   ÷âÁê‰SiÒ  )Ñº‰ˆˆˆ‰È÷âÁê‰SkÒ<)Ñº“$I’‰N‰È÷ê‰ÈÁøÊÁú)ÂÕ    )Ğ‰ÊL$)ÂBHĞ‰ğ‰S™Áà¤òb Ò   €‰D$‰T$ Qj h±: èüÿÿÿ‰Á‰Ö‹D$(‹T$,köd¬ĞºaÛ, D… 1í÷â‰l$‰Å‰T$‰×ºËÿO[‰è÷â¸d   Áê‰T$÷á‰D$‹D$‰T$‹T$t$`…A^Ò1’éÿ‰D$‰T$ƒÄ…ÿ„¢   ƒç”D$iD$]  1É ‰Â·ÀÁêıÇçxÖ—Á‰Ît$4$iÀqz  ÁèƒÀıÇçxÖvmƒê¹Îşÿÿîl  ·ÀL$‰S‰s‰C‰K‹D$$d+    uIƒÄ([^_]Ã´&    é€Q ƒÆùQ ïéeşÿÿ´&    v ƒá”D$éYÿÿÿv ¶L$ƒÁ;ë‘èüÿÿÿèüÿÿÿV‰ÖS‰Ã‰‰ğ‹èüÿÿÿ‰C‹D$‰S‹T$‰C1À‰S‹N1ÒÇC    öÁ ÇC     ”À•ÂÓàÓâƒÀÿƒÒÿ‰C‰S[^Ãt& èüÿÿÿU‰åWVS‰Ãƒäøƒì‹ ‹èüÿÿÿ‹3‹{‰$+C‰T$S‰Ñ#F#N÷f¯NÊ‹KCS !×!Á‰{ ‹|$‰K‹N1ö‰{­ĞÓêöÁ EÂEÖ‹4$CS‰C‰s‰Seô[^_]Ã´&    v èüÿÿÿU‰åWVS‰Ã‰Ğ‰Êƒäøƒì0‹‹s‹{‰$‰t$ ‹q‰|$$‹y‰t$(‹t$ ‰|$,‹|$$)ğ‰T$‰Æ‹D$(ú‹{#T$,!ğ‹s‰|$‰t$‹s ‹[‰t$‰\$‹Y‹I‰Ş‰L$‹L$(‹\$,¬ÙÑë9ÁÓs`‹D$(‹T$ +$‹L$$L$!Ğ‹T$,!Ê¶L$‰Ó÷æ¯ŞÚ+D$T$1ö­ĞÓêöÁ EÂEÖ‰Æ‹D$‰×‹T$)ğúeô[^_]Ã´&    ¯Ö¶L$‰Ó÷æÚD$T$1Û­ĞÓêƒá EÂEÓD$T$eô[^_]ÃfffffèüÿÿÿUWVS‹    …Ût)‰Ö‰Ï´&    f‹C‹+ƒÃÿt$‰ò‰ùèüÿÿÿ‹X…Òuä[1À^_]Ã´&    ¶    èüÿÿÿUWVS‹    ‹t$‹|$…Ût‰Õv ‹C‹ƒÃW‰êVèüÿÿÿ‹XZ…Éuç[1À^_]ÃèüÿÿÿUWVS‹    ‹t$‹|$…Ût‰Õv ‹C‹ƒÃW‰êVèüÿÿÿ‹XZ…Éuç[1À^_]ÃèüÿÿÿUWVS‹    ‹t$‹|$…Ût‰Õv ‹C‹ƒÃW‰êVèüÿÿÿ‹XZ…Éuç[1À^_]ÃèüÿÿÿS¸ x èüÿÿÿ‹x ‰Â¸ x èüÿÿÿ‰Ø[Ã´&    v èüÿÿÿU‰åWV‹p‹x‹@@@‹ÅŒx èüÿÿÿ)Æ×‰ğ^‰ú_]ÃfèüÿÿÿU‰åS‰Ë‰Ñ‹P`‹@\)ÈÚ‹]üÉÃt& èüÿÿÿóÃ´&    U‰åW‰ÏV‰ÖS‰Ãƒì‹S@d¡    ‰Eğ1À‰Uäèüÿÿÿ‹Uä…    ‹CD‰Uä…ÀtoMìUè¸   èüÿÿÿ…ÀtC‹M‹Uè‰J0¹`   f‰J4‹M‰j<ÇB8    ˆH‹Mì‰p‰xj ÿuäRº   j jSèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãt& ‹…Àu‹ëàèüÿÿÿ´&    t& U‰åW‰×V‰ÆSƒì‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD‰]ä…À„ƒ   MìUè¸$   èüÿÿÿ…ÀtX‹M‹Uè‰J0¹`   ‰j<f‰J4ÇB8    ‰x‹O@ˆH‹O‹_‰H‹M‰X‹]‰H‹Mì‰Xj ÿuäRº$   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãv ‹…À…sÿÿÿëİèüÿÿÿ´&    W‰ÏV‰ÖS‰Ãƒìd¡    ‰D$‹C$©À  u@¹   ‰Ú‰àèüÿÿÿ…Àt‹T$,‰p‰xˆP‰àèüÿÿÿ‹D$d+    uƒÄ[^_Ã¶    öÄu»‰Øèüÿÿÿ„Àt°ëÔèüÿÿÿ´&    fV‰ÆS‰Óƒìd¡    ‰D$‹F$©À  uS¹    ‰ò‰àèüÿÿÿ…Àt*‰X‹S@ˆP‹K‹[‰H‹L$(‰X‹\$,‰H‰X‰àèüÿÿÿ‹D$d+    uƒÄ[^Ãt& öÄu¨‰ğèüÿÿÿ„Àtë×èüÿÿÿ´&    fU‰ÊWVS‰Ã‹¨x   èüÿÿÿƒøt[^_]Ãv ¶M‹uºÓÕ  ‹}h ´  Óà‰ÁC@èüÿÿÿWVPhœJ Sèüÿÿÿƒ`  ƒÄ[^_]éüÿÿÿU‰ÊWVS‰Ãƒì‹¨x   èüÿÿÿƒøtƒÄ[^_]Ãt& ‹M‹U‹u‹}‰L$¶M‰$ºÓÕ  hà³  Óà‰ÁC@èüÿÿÿWVÿt$ÿt$PÿuhÀJ Sèüÿÿÿƒ`  ƒÄ,[^_]éüÿÿÿW1ÿV‹t$WVèüÿÿÿXZ^_Ã´&    t& 1Ééüÿÿÿ´&    fèüÿÿÿW‰Ï1ÉV‰ÖRS‹Õ”x ‰Ã@èüÿÿÿ‰{<‰s@‰ÇC0 l ÇCD    [^_Ãt& èüÿÿÿW|$ƒäøÿwüU‰åWV‰ÆSƒì‰Eä‰Ğ‰Ê‰EØ‰Á‹‰Ó‰UÜ‹W‰ß‰Eè‹F‰Uì‹V‰Î)Æ×…ÿx?‰û;uè}ì‰ñ¿    ¾   }Aÿuìÿuèèüÿÿÿ‹]ä‰C‰SXZeô‰ğ‰ú[^_]gø_Ãv eô1ö1ÿ[‰ğ‰ú^_]gø_Ãt& ‹uì…öxQÿuì‰È‰Úÿuèèüÿÿÿ‰Æ‰×‹Eè‹Uì¯Ç¯Ö‹Eè÷æÚ‹]äCS‰C‰SY‹MØ[‹]Ü9ÁÓ|ˆƒÆƒ× égÿÿÿ¶    èüÿÿÿU‰åW‰ÏV‰ÖS‰Ã‹@@@‹ÅŒx èüÿÿÿW‰Ñ‰Â‰ØVèüÿÿÿY[eô[^_]Ã¶    èüÿÿÿUƒÀP‰åWV‹pì‹xğQR‰ò‰ùèüÿÿÿY^eø^_]Ã´&    fèüÿÿÿ‹PHÇ@H    …Òt‰Ğèüÿÿÿ1ÀÃv U¸   ‰å]éüÿÿÿfèüÿÿÿW|$ƒäğÿwüU‰åWVS‰Ãƒì,d¡    ‰Eä¸   èüÿÿÿ‰Ñ‰ÂEÈèüÿÿÿ‹EÈd‹    ‰‹EÌ‹3‰C‹EĞ‹{‰C‹’¤  ‹Rr$z(B,‰ù™R‰òPEØèüÿÿÿ‹EØ‹UÜ‰‹Eà‰S‰CXZ‹Eäd+    ueô[^_]gø_Ãèüÿÿÿ´&    U1À‰å]éüÿÿÿt& èüÿÿÿUWVS‰Ãƒì8‹x d¡    ‰D$41À…É…è   ‹ƒØ  ¨„ê   ‹C$½ÿÿÿÿö@xtp‹œ   …Òtf|$1À¹   ‰\$ó«D$ÇD$íÕ  ÇD$şÿÿÿèüÿÿÿ‰Æ= ğÿÿwº   Fèüÿÿÿ¸ x èüÿÿÿ‰Â¡x …Àt-¸ x èüÿÿÿ‰ğèüÿÿÿ‹D$4d+    ufƒÄ8‰è[^_]Ãt& ‹ƒÀ  ‰$èüÿÿÿ‹$„Àt¾‰Ø‰x 1ö1íèüÿÿÿ‹$ë¨t& ¸ x èüÿÿÿ‰Âë•f½ğÿÿÿë´&    f½ÿÿÿÿëèüÿÿÿt& èüÿÿÿUWVS‰Ãƒì‹x@Áâª€x ‰T$‰èèüÿÿÿ‰$Cèüÿÿÿ‰Æ…Àyw‹$‰èèüÿÿÿ‹ÅŒx èüÿÿÿ>t& ƒÄ‰ğ[^_]Ã´&    d‹    £    sßdÿ    ‹    …Ét‹IR‰ÚP‰ÈèüÿÿÿXZdÿ    uµèüÿÿÿë®´&    öCDtƒ‹T$‚„x ‰ÚèüÿÿÿƒcDşéiÿÿÿèüÿÿÿS‰Ãë¶    ó‰Øèüÿÿÿ…Àxó[ÃèüÿÿÿƒÀPéÿÿÿv èüÿÿÿUWVS‰Ãƒèƒì‹s,‰$vÁà¸€x ‰D$‰øèüÿÿÿ‰ÅöC0t‹D$‹$„x èüÿÿÿƒc0ş‰ê‰øèüÿÿÿ‹k(…í„á   v‹ÅŒx èüÿÿÿ‰Ñ‰Â‹$èüÿÿÿ‰Å‰øèüÿÿÿ‰Á…ítA‹Cø‹Sü‹l$‰C‰SÅ„x ‰C‰SöC0…¡   ‹$‰è‰L$½   èüÿÿÿƒK0‹L$‰ø‰Êèüÿÿÿv‹ÅŒx èüÿÿÿ>t& ƒÄ‰è[^_]Ã´&    d‹    £    sßdÿ    ‹    …Ét‹IRP‹T$‰ÈèüÿÿÿXZdÿ    u³èüÿÿÿë¬t& ‰øèüÿÿÿ‰Áé|ÿÿÿf‹$‰è‰L$èüÿÿÿ‹L$éHÿÿÿ´&    fèüÿÿÿUWVS‰Ã‹@@4@Áæ®€x Æ„x ‰èèüÿÿÿ‹S‹Kj ‰ÇCj ‰S j ‰K$‰S(‰K,èüÿÿÿƒÄöCDu‰Ú‰ğèüÿÿÿƒKD‰ú[‰è^_]éüÿÿÿt& ‰Ú‰ğèüÿÿÿë×t& èüÿÿÿUW‰ÏV‰ÖS‰Ãƒì‹h@Tm Áâ‚€x ‰T$‰$èüÿÿÿ‹T$‰s‰D$‰{²„x öCD…œ   ‰Ú‰ğsèüÿÿÿƒKD‹C‹Sj j j ‰Ñ‰Â‰ğèüÿÿÿ‹T$‹D$èüÿÿÿDm ‹ÅŒx èüÿÿÿƒÄ>t& ƒÄ[^_]Ãd‹    £    sèdÿ    ‹    …Ét‹IR‰ÚP‰ÈèüÿÿÿXZdÿ    u¾èüÿ