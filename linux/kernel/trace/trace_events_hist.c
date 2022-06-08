haracter as follows:
	 *
	 * - '+': irq is enabled and not in irq context
	 * - '-': in irq context and irq is disabled
	 * - '?': in irq context and irq is enabled
	 */
	if (class->usage_mask & lock_flag(bit + LOCK_USAGE_DIR_MASK)) {
		c = '+';
		if (class->usage_mask & lock_flag(bit))
			c = '?';
	} else if (class->usage_mask & lock_flag(bit))
		c = '-';

	return c;
}

void get_usage_chars(struct lock_class *class, char usage[LOCK_USAGE_CHARS])
{
	int i = 0;

#define LOCKDEP_STATE(__STATE) 						\
	usage[i++] = get_usage_char(class, LOCK_USED_IN_##__STATE);	\
	usage[i++] = get_usage_char(class, LOCK_USED_IN_##__STATE##_READ);
#include "lockdep_states.h"
#undef LOCKDEP_STATE

	usage[i] = '\0';
}

static void __print_lock_name(struct lock_class *class)
{
	char str[KSYM_NAME_LEN];
	const char *name;

	name = class->name;
	if (!name) {
		name = __get_key_name(class->key, str);
		printk(KERN_CONT "%s", name);
	} else {
		printk(KERN_CONT "%s", name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		if (class->subclass)
			printk(KERN_CONT "/%d", class->subclass);
	}
}

static void print_lock_name(struct lock_class *class)
{
	char usage[LOCK_USAGE_CHARS];

	get_usage_chars(class, usage);

	printk(KERN_CONT " (");
	__print_lock_name(class);
	printk(KERN_CONT "){%s}-{%d:%d}", usage,
			class->wait_type_outer ?: class->wait_type_inner,
			class->wait_type_inner);
}

static void print_lockdep_cache(struct lockdep_map *lock)
{
	const char *name;
	char str[KSYM_NAME_LEN];

	name = lock->name;
	if (!name)
		name = __get_key_name(lock->key->subkeys, str);

	printk(KERN_CONT "%s", name);
}

static void print_lock(struct held_lock *hlock)
{
	/*
	 * We can be called locklessly through debug_show_all_locks() so be
	 * extra careful, the hlock might have been released and cleared.
	 *
	 * If this indeed happens, lets pretend it does not hurt to continue
	 * to print the lock unless the hlock class_idx does not point to a
	 * registered class. The rationale here is: since we don't attempt
	 * to distinguish whether we are in this situation, if it just
	 * happened we can't count on class_idx to tell either.
	 */
	struct lock_class *lock = hlock_class(hlock);

	if (!lock) {
		printk(KERN_CONT "<RELEASED>\n");
		return;
	}

	printk(KERN_CONT "%px", hlock->instance);
	print_lock_name(lock);
	printk(KERN_CONT ", at: %pS\n", (void *)hlock->acquire_ip);
}

static void lockdep_print_held_locks(struct task_struct *p)
{
	int i, depth = READ_ONCE(p->lockdep_depth);

	if (!depth)
		printk("no locks held by %s/%d.\n", p->comm, task_pid_nr(p));
	else
		printk("%d lock%s held by %s/%d:\n", depth,
		       depth > 1 ? "s" : "", p->comm, task_pid_nr(p));
	/*
	 * It's not reliable to print a task's held locks if it's not sleeping
	 * and it's not the current task.
	 */
	if (p != current && task_is_running(p))
		return;
	for (i = 0; i < depth; i++) {
		printk(" #%d: ", i);
		print_lock(p->held_locks + i);
	}
}

static void print_kernel_ident(void)
{
	printk("%s %.*s %s\n", init_utsname()->release,
		(int)strcspn(init_utsname()->version, " "),
		init_utsname()->version,
		print_tainted());
}

static int very_verbose(struct lock_class *class)
{
#if VERY_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

/*
 * Is this the address of a static object:
 */
#ifdef __KERNEL__
/*
 * Check if an address is part of freed initmem. After initmem is freed,
 * memory can be allocated from it, and such allocations would then have
 * addresses within the range [_stext, _end].
 */
#ifndef arch_is_kernel_initmem_freed
static int arch_is_kernel_initmem_freed(unsigned long addr)
{
	if (system_state < SYSTEM_FREEING_INITMEM)
		return 0;

	return init_section_contains((void *)addr, 1);
}
#endif

static int static_obj(const void *obj)
{
	unsigned long start = (unsigned long) &_stext,
		      end   = (unsigned long) &_end,
		      addr  = (unsigned long) obj;

	if (arch_is_kernel_initmem_freed(addr))
		return 0;

	/*
	 * static variable?
	 */
	if ((addr >= start) && (addr < end))
		return 1;

	/*
	 * in-kernel percpu var?
	 */
	if (is_kernel_percpu_address(addr))
		return 1;

	/*
	 * module static or percpu var?
	 */
	return is_module_address(addr) || is_module_percpu_address(addr);
}
#endif

/*
 * To make lock name printouts unique, we calculate a unique
 * class->name_version generation counter. The caller must hold the graph
 * lock.
 */
static int count_matching_names(struct lock_class *new_class)
{
	struct lock_class *class;
	int count = 0;

	if (!new_class->name)
		return 0;

	list_for_each_entry(class, &all_lock_classes, lock_entry) {
		if (new_class->key - new_class->subclass == class->key)
			return class->name_version;
		if (class->name && !strcmp(class->name, new_class->name))
			count = max(count, class->name_version);
	}

	return count + 1;
}

/* used from NMI context -- must be lockless */
static noinstr struct lock_class *
look_up_lock_class(const struct lockdep_map *lock, unsigned int subclass)
{
	struct lockdep_subclass_key *key;
	struct hlist_head *hash_head;
	struct lock_class *class;

	if (unlikely(subclass >= MAX_LOCKDEP_SUBCLASSES)) {
		instrumentation_begin();
		debug_locks_off();
		printk(KERN_ERR
			"BUG: looking up invalid subclass: %u\n", subclass);
		printk(KERN_ERR
			"turning off the locking correctness validator.\n");
		dump_stack();
		instrumentation_end();
		return NULL;
	}

	/*
	 * If it is not initialised then it has never been locked,
	 * so it won't be present in the hash table.
	 */
	if (unlikely(!lock->key))
		return NULL;

	/*
	 * NOTE: the class-key must be unique. For dynamic locks, a static
	 * lock_class_key variable is passed in through the mutex_init()
	 * (or spin_lock_init()) call - which acts as the key. For static
	 * locks we use the lock object itself as the key.
	 */
	BUILD_BUG_ON(sizeof(struct lock_class_key) >
			sizeof(struct lockdep_map));

	key = lock->key->subkeys + subclass;

	hash_head = classhashentry(key);

	/*
	 * We do an RCU walk of the hash, see lockdep_free_key_range().
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return NULL;

	hlist_for_each_entry_rcu_notrace(class, hash_head, hash_entry) {
		if (class->key == key) {
			/*
			 * Huh! same key, different name? Did someone trample
			 * on some memory? We're most confused.
			 */
			WARN_ON_ONCE(class->name != lock->name &&
				     lock->key != &__lockdep_no_validate__);
			return class;
		}
	}

	return NULL;
}

/*
 * Static locks do not have their class-keys yet - for them the key is
 * the lock object itself. If the lock is in the per cpu area, the
 * canonical address of the lock (per cpu offset removed) is used.
 */
static bool assign_lock_key(struct lockdep_map *lock)
{
	unsigned long can_addr, addr = (unsigned long)lock;

#ifdef __KERNEL__
	/*
	 * lockdep_free_key_range() assumes that struct lock_class_key
	 * objects do not overlap. Since we use the address of lock
	 * objects as class key for static objects, check whether the
	 * size of lock_class_key objects does not exceed the size of
	 * the smallest lock object.
	 */
	BUILD_BUG_ON(sizeof(struct lock_class_key) > sizeof(raw_spinlock_t));
#endif

	if (__is_kernel_percpu_address(addr, &can_addr))
		lock->key = (void *)can_addr;
	else if (__is_module_percpu_address(addr, &can_addr))
		lock->key = (void *)can_addr;
	else if (static_obj(lock))
		lock->key = (void *)lock;
	else {
		/* Debug-check: all keys must be persistent! */
		debug_locks_off();
		pr_err("INFO: trying to register non-static key.\n");
		pr_err("The code is fine but needs lockdep annotation, or maybe\n");
		pr_err("you didn't initialize this object before use?\n");
		pr_err("turning off the locking correctness validator.\n");
		dump_stack();
		return false;
	}

	return true;
}

#ifdef CONFIG_DEBUG_LOCKDEP

/* Check whether element @e occurs in list @h */
static bool in_list(struct list_head *e, struct list_head *h)
{
	struct list_head *f;

	list_for_each(f, h) {
		if (e == f)
			return true;
	}

	return false;
}

/*
 * Check whether entry @e occurs in any of the locks_after or locks_before
 * lists.
 */
static bool in_any_class_list(struct list_head *e)
{
	struct lock_class *class;
	int i;

	for (i = 0; i < ARRAY_SIZE(lock_classes); i++) {
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
 * that <prev>úËqìéŸ>¸aÊ´‘aÍÊBé\ÉmØ¥ÔcéŠÙ‹„UÉõ\rº8ˆ°§İ`‚|ğß‰üâ	Y¹3@zB=;`9ì”,“ı`_¹ŠÌÈ©‚7ÜÄÉŞËk[ô©ãã‹CêtZ¨*ô«°î’¹Ej7(`!SP?ä_e ÄèÎ€ø÷6è—±/P-Qø›òÃeØ‚4·[Xxúìşu;,+ë…#ãéEÁQ9TY}Kv­Œ”Ë4õ–Ï‡¶æÖŞÃ^÷p´ôQ:u?ÕŞ©í~Ñ(6¬vÒ•!=k\åîäÀ¿^’ıš³ôws/˜3;øzu§ëLÅK81:ÿ•ˆjhÆ¥ÙÃdgƒä<Yòµ†ùšâûÈÈ\@0úŞØÉHxWñ®œŸ"¼o-p[ûb†.<¤í K$!¢Cà•WpÔ°‡ë“uµ{Y€„a¤êÕO ¸ûÃÇY¹X„+Ÿ.ÚÛ*¦Ôëè†µõ»ccl‰Z»Ä¡]uQw< êÊ×ÇÈ Fk±üífÙæÉøiÊ¸Ä@2gÑ€e¾ |IĞx¸(0,bz
Àu)Fwş`–|œ;°Nl‹%®‹4ÊÑ3Ó£óîúß{òqjLT·zØm38¥¥’6s:Íéô¨M¶qE®Ê.eë¦/Wùn¸t£/Ë<yâğõÉ‹âIâ„äPĞ¡¤É2[B˜—ÖòÅé{÷vÑ˜R\XÉ’n: ÓMê6ø›»i3üÿ;¾6±'cZ›äô|Sn’ÿDù«
t†ĞŒĞV‡<ìUUÏòne0ï§¾6‹îÖº’yÂ‹éš66@ï©Js`ŸSÅ’¬÷á¨„júUMfÒ5ÜIÏ1bxOnˆ!_ójâ Çh8ß¸M´/ğ^ÎÏÂfy%an€•>i_öœ<µÔ‚Ã"„rO1( İ0f«†^Ò(/Âj;Q6ït3˜PÄë&À©ãŠ>„Ù0{Î5e&å² ƒæaÿíxÿ«‚uŸ*ª|­¯|Ò¥oK¨µ
»ş7'¬İ9FÈ9%ˆkØ:%Èa,ı8„ËöìyÔóywîÙJÖVÁ™NşãP?ÑÀ?ætºt)9ºíüWgd7ôëß¨ ’â*Ó§ñ·µIíıGõQÇnL~+„]˜S_›'Ò·+ò\¹Y8îe÷M;Õ¬ÛµÉ;©%®CòœÕPm•ìºÏ“zÙŒxyg,èÙàşÀLŠ‹×™µåw‘~Ÿ›n’R@Ş¬Ã(F ñ@TNøÔP–UˆÑbSÌZOH‹ô}d
>ã+ÀwVmØ½C*’®u¤Êª˜f/'©©§±±ĞË0Äúêj=!`QÌÊrıYyß|?ÓèÙ•›Ÿ›Ê:7“t"´•kƒ%+Ò¯2J—~ùŒ|sMNãÛÍÖ4QS¬¿9ÃøÑ”®n Ğyÿîyç7ÜÕçµ
­dk,Wùèp»x'·l7£,†ïÅØ•òw(—yÂ ñ|(ë_‚S)à Z¢B…şÊVwLÕ7GÍë¢giçË9:pUÁKpXr(à€RXúm© ’'âL#N1ÅLã8‘Zi@UoK\¦kJô¦ƒò€^Š|¾N½(7wÿº·c; M`ÿt½,2@ĞM…7Yu¦¾qñË'(ÊC.Ş-Yaˆ  ¶èö]ÅKa„÷;¾9$N=³š
ª¿ÍàĞ3P»©óW¼ÏŒ7:ZèÁd›êİ²pHRèV^>"ºfŞáZš¿
¥Ñ·Ò0qÿ8ïØ†tiŸI;‰fq²®®¯)™>'íäƒõÔPTœÌ^À=j77Ï™2á…¿dúT+ğ}Yè‹ÂW†çbßŠ7P¬rç#î d9÷FoMµˆïjL¹Õï}º(ïJpĞş“ªäA¦—ü"ô¬ãgqYOºüŠÚKS3¢yì"(WàUú¾©ÙZ8]T½L ü$Tp½|;ÀÂéÃb&Î=sÜ
ô'xÑ¨OúS¡N®mX Àbñ8$—`Ó'üK®&ı²{£†öq()È€\3—çÃo8©=ßC5İQ!ãaH¸Ôß´E®T½s@tvîˆ2´î›î{ 7ÛzTm‚8k¾é5Æ’ª+ÿV…ÅQ1Âzë0a‚lyÓÒïåK©2ƒpìáÈ)‚tÁPyÚÅ»NĞVZ– XbÛmæö˜ˆçm@ë  0İ¥??Şv 3¬pû!³ÍŒ˜ÿ¢áÌ¾ó@€"÷y¤‘Ãæ=âCj(ÎÑüÕx¡$åÖáTg!÷»Z‡s>c¯s¿?fèï?µ°ÓhÓÑƒø®´p„Zc{üôyî¥s ŠÛˆÃè¹ğwkäœ5Jxáõµ_a­LRÑÂÇ©éÛ¼Œ€'İdóÕè3ºÑC‘$t–Ut}¸Ä6ş­©TÛÁÄ¬QÎd,í¦é¬İ†ËÃF&Òô×P¤Âá5mğ¿¹Úàw8ğŞêS{î õ×`j°vˆïÈ	PŞ<ã¢Ä€R\Ñª½¶x”×“]ª_™€Ñg%DaôÔsÒÅ52	¬÷ùŸl#ø’ùÉäÕsR½EÓBîåTY“5ƒ0-S¯Íp ªî\Ë3z·tÒD¹HY‰€,@½87<G¼Ïü”A
ØŠ¨”§-jµ_´uRÁ¶asÆzéùCğ
«Ñr:.^ÉMW&È$¹+iáuãHWŒ=u”{†c? Ş¢L²/ÅtÜÈ_³ôÍ7 A–W,<ïÛ‚›®~^S(>d´³;çƒu”çyKßPmÔúKÖ[T:Ua»Üi©`­İÃä<Çy¹¦s÷WØ†¥ğ‚C&…XÎËëÃ<êa~–«1££Çsnşo	¾4’S¢`§wnÑˆ€¿®|`óÍib7…«ó/îtÊèjÜ{ìŞ6Ÿ4Ò3éï:ÿİ¦L¶©ÇB|Vİ}“ÎOog4@®ĞmXÎzî&ÅÛ×¤•´NµŠ²cïîID„.ä ±´R‡½ÔFûG'ëx›İşbkŠ<éŸ{[‰ pr“N…"İ>ö4WLÑ%²¹ÒAE3OÍ
cÔÎ•Ä<‰OØ|ù£şŸTË¾.`2†·lê¯…Ã¢#@“ãş'VÚµn‰÷¼šY-¦XÛÕV-â3â÷‘Ej4ùñ„çÅ@nDàá¡è~­=Õ³Fx13ÿ½ ’Ã?ø}M-
—óş ©ëLUñÃƒ÷B2bpOcAºæ»/FH5å˜ı}üÎ wH/¶Hˆ¦Á‹/ÓÔo¤ä[g]¸€ú`J3€÷±íRÛkK¨°@+¨Ş®ôĞ7¶¯‹;R°¡r†ŞŒ”ÎLkÍÔø“ ã}³á·üÆ°~»Üû®®ˆŠ›VÌº¯ä—¼Yâ½9Õ÷ÔvÙ¥¾á…ß%\ç¯Ä¡÷z¾Í½½÷ëIé ~ ˆ:£>*lâ¨ß÷ƒ`E43Á±_¶c^è‰
”r‘½.êÆS8§	æ]wóZ4iİğêÓ”‚kFD wtg?ÿ+êı«+¾é¯o7bá¬ı\¿k;¡ O-Ed[Bİg„Ÿ²3¸‰·§Î‚şºOÆİYøÆ©LÍ}
jÜPuæŸ|˜ŸÈT
^±LÄ½!¡É7F3l AÖtN |İ¤Å:Íì{è’(°RQdĞ„@Û¡(ÂéOŞı™Ü£C×l:î3ƒ4ïTth+kpö’Æú9Ëé¦¾3ªoQr
ÔE¯:î=Â	7—EO~Êşù¼úáÌ‡n[$>|ã³Ä‰’8¦©GûáU‚YËSğw<§ÖâÄ”ÛÏŠaèÓÀ°­ŒnlP ‘<]~‚Z›.Šîƒ†KûéuÖRÙ9Œ©Yg<ë†×œ¬eç†ì‰ëâú•B©A»¦ğ±ş¬Dn¡6æ]z&=-ólóİõ7q'âæZ©’¡ú¶ıŸàW~”Zyb4%Õw¿Dµ‡³Õ/Y¶Ú'-„T?*·°¼îò~İˆ"ğ.¨gr$Ÿ‹‘²¿›&×}%JMª¯)µ.*Óiõî;ä/:)bW™0ÚşÕºÖhqNfêUbâ¿äâYdş¸IÑ0xå½µ«ÿO&•İÃ%lù÷yBÏ²&' °v¢Œ_}-Ù6¹Å`n¡r¡4–¿ÛvzøÀª]şllò@ÍRfÑ ÀF/>ÅğrRgÁûd;yoé
Rç(}úÎ,4oÑƒa(¶‚à/›¼œaºŞ $x.÷jŒÕ~ÅõxUçJ	¸û›¼ÔèPsùL»#â„	 Ä¸Z×‹‡¥²âÊğÃ.D~²ó*÷¸z»Ü²†ËÜªs…§¯V·=ÊHÏ4.Ñ§úã(m·Î%ïğÄôz&õ{ábËÛ;4Ö9,6O²wÏş×œãBÜª}’oGˆ~¿µqøµPàE
c„àÀ¸¿óƒáOÓRóüã®yî04Ú<½	™ï¦ıFÕ¾›³åŒQB-Ê­æĞ¢={ ’ ˆÒbøÇÁr+è‡ÔDGÕF4Ê…\dÆæíÃÕyŠ*új¾>D»ßf¸…`‡g¯>=³}!*ºL!™wn†!Ó!% <>èó&» Î„xŸ«¯”\¥(©`>×€?ó¥	ÛÕ`üj…‡)gÅ}€ûš^µëÙ:§ÍÚ'Ï¢ld5“RĞ)u¶†)©NİkÒ%éP¼¼¸º
_¿Ò¯¤­¢ ğÁ¢üa¡fŞ17áYKf÷%«ÿDmøM¿é{“¢‚Íîé[¼Ğ`•Ÿ(mÚ_(¡°%¤)È–ŒvB:İy‚ãLAsWªï6TQÁ—¸RèÜbjÙIÊ·ã"'İñ&Ÿ,²XRÓ%™‰Æúvø ìÎid{j”˜”Ò¤‚ïPÈhIËÍY§×³t§šY|7Ey
=ÑÁ*Ør~Ñü5lÔ&!”D87zu‘ğÀtq|†5	–T˜ƒêÅ‚¶bœ3|o5‰.×Ëˆ±*Ş7?K›É²“#6ÁBEŞ¯½Ò1*ÇÙ„+qí7jÿ»°½AJŠk};f;*v~l´%ŒîÇåfàöEGŞ"jşİAĞ ğÏcHUOıÏ“Š)úvmX¹Ğâ¿‰OÎª0i¨«Œ—Õ47{¨zf< ³ãWĞ2qğ¶¾ìÊ+1t$ií]Lf~÷ødù£Œ9ÍètwÆ>ôv|	ñUıåëÎ¸/)¾sÃé‹6œ
ù¾ŒmŒÅW#	<ùmïîã¶ŸÓ`|vV» @¥â¹Ğˆ]¯#¯ÖâÈ£¿ìû˜Ê<•—m,[mà§Ãl]´M~ÔÖ>@»‰ÂS…€iš7À Ìâ–ÎD¼‹1ÜUç2ø%B„,Ñ :¸ûµBX}I?«Äx8n]3>.Ø_ã¸ö	ºiÕS*f••İ#ŞÙ³ŒÕÏ½g0$4”à? ’‚ÀAŞCÂayÖïYú%%ù@€h¥oáÿş²éÿ‘©ÈÕ|w°r[>úk_˜DTëÑÆÈ0ï•é'Rî®ˆÄÂT7ÄšrğwÆÉ¬9‰ˆzˆõ–ı+Ò¶Ø6ıÔ•l2*`Ô4â9êz”i8¥¦ÿôR­–û;LK÷¹ÇŒ½P`1ëçq¥½¥û¤ĞÄxÍÂ¼I´(ñá…çOŒÁIí~~¦oœ·å¿(!‘îË¬Ÿ‰Ò¯¥¸Ôè+’åe¸nK$ Ş+ìÚ«r@È_1#ğ#ú9‚l]¨Œ®ÃÉI-0O¸ywÕÃ‡æ¢Ö¬†Îõõß¸›fù÷YFV:êÉ¯ï™@›Û"mÍ».~†º(Í™9™/ËnlO•åY¨ ¢bÖÑY—Ô? M©¯‰0²UÜä0o)´¦)ê&Óø÷ì–§>böõYCöxªG€å_›~$pÅ¦%V8L®¯·ÔhbpH’Z)-®êm[tdíT1÷Ø‹÷âAää]Ól€…â[½Ôšû†O+û
VæAX÷¡r8oõİ¦5şn+°5«(¼iFB!½;{ù/Šìà$Äyˆ5<ì-P~€‹—¡dÜ_ã§«ÊiO{VxzÈ^7”üÕgåIW=)˜`cê=¬qÊf+¥.(Ü¶¥SU.WIÖãzµû	ï©ÆòÎùî¾âHZúUbë!SÅÒ­â=är&¢tÄ
n‚ñÉ#]ŒÈÕ)ÍØY{3!î³ëÎ¤T©¦“PÃååbùl~Á5¤Èï^ÈÌ;n â¼2¢ TÙ]•ğõ˜ÜP±ä@4d©iYîÁ/Eh®D°o\5ˆ0ğH¾Uü/ÛzZİİoy÷¾å=¿Ä‹~KA,Xµç/J,íØ`›¶äNÄ
BøwnQÍæØºtÔw[–Qƒ+EvmÅ&pHİÊôñ¡£&JhJïv~X]~ûã5vÚ›)q¼_€¢Ršøš€kšõ‡vˆW#ÙÖd>ç¼7ŒƒïDBZÒ”PáÃ$Ï+ò‰Öª 6±ù#9±ÖÖW7Œj¿?M÷¶ºu/¯»«³r•‚f»³×+øiû­…¢`Hè·ùáAƒ&Ë‚‰xıáó«m¥«ÀAd	ç¼Â¦½Ó³2Ar!*Uwi“V“Hk®¶[ÑÙ¹3p+•çbÙÔ‹ÅĞímXç[ÕRë•^U„PC¥W ùtÃ¼Ï<š7{Ïúœ
ì6)‹Ş9‰,P<u×Z8)N—Ä·ÇÊy†ÃıÄ=¯E6‘`µìQ‡bÓÊ@ JÊéõ¥¦„LUŸ5ÃÉ_2ÈWJ^á>¢‡-<„™°p‹?shØ"ÓŸÖ÷ˆ¿D‹×rğÙ fƒÇ±ÒÜZhp–NZH°•]†êô,ÓÍmãËzlè¯æ4ì$«Ê"ttÆÔÅÀ].?g¡üI…0’Æ¨Õ?j0Ò@(,‹.H´Îiõ[‚ï–šÀ€æÿ0t¾)~ç#Qğ-9Z‘Sº¨Ö¦§WxK~×ç^Vnå-µS.h%ø¾²&³¤dthõÅÅ¥z¨)´áÍ1÷`z—§ıg"S‹°U÷vôI"oí‡Sÿ–R7ƒ!qT/ò¸ÇXŸşÇCNÑ-º­ÆPüãWÅ‚«—Ç“ò§”vûîW<¿*'¬k\_àB6ø/	l‹ÂJu¶Êëø@aŸ/Ûi}'h1ÀFİHî+lã³•ÀÃŒ^æ" ã{¡÷CY‘à:(ÜKöà,SÕ&¦ëBy¥Œj›­=»G÷±KÒÉWM˜ú§XDÿğPg8¶”ûËØ‰ K+1?ï¶Ù>“¼¶oU´RïfHfï¶o)mnŞKB¼ëÏ±5-¯^°®E1¯K 0d±_ïR0rqËMÍF‹Ü”ív‘á¯|ßqéjšª3APJ˜•R	Òëáš'K–µ¹(§SÕÿ•ydòÄÇ6‡ô¾ˆF§/ ò5Zñf&Aù‘G’&µ€¶*o‘Ò	HMâ$AÉ°&dDáò—{ÂdrcQ0ù`éI‚®läÕr\6­¶2Ïê5Çj¢xQÎ¸QäƒÙF`Gç°éŠEê9^Ã!»WP•Û‚-²ËÒ0ÀòdÖ£×€°ÿêŠœ:¿^Zß8›:Õ‡®I[·±Á·F–fj0/™ÍÓ`×ğúÜ°Œ&7'€¤}-ôÄaOèÑì.ÄÛÉøT›³´¥\Z·«!XÑW¡Æ‚«Ù3Ëüc¸Åï¶#DT® î­Óz§(y–‚¶úÕ¶-“LHËG‘NbFãaMÿÎSi=’vËò.JËM¿ÃSÖœa£á4F´>İïËş„=q…½»3w½ã°‚«äiŒ	ˆQ¹é…R…iŒC4£xâ*öFã¸ì(W¢ZY
|ùñ§ÿ%ÜÚ¤¸"Ú=´¯m†){;ÇÛª‡ŸÂ3 §‹é)Yó²kwoD%fv‡¶ƒ²zßy*Ò-È&3¼å-gkÃ%mGê4à“—Ú˜¢4UJ—Â\PA+ÍãÆn¿èOXs×´n!çz}Óg¬û.ı½Ïd,r4w	†¦(‰;®İ—w»üÎî e>€R}ûÛ>ÏS²•»³/-BN Œßnÿ£ˆš¨[¹HV!;õWÓÓ»F!å1h¤ø·šVTø{•LòÕ%İ/ë·²œr½ÈÉ8{MßÜ€‰E·/obÃg¯©MuË¬<Ÿ$Äˆ_¥ÄÖïÎçûöW¢€è§9yÌ—)³ñ†·˜rPŸuÏP>‘!&YÅ@ ±|…ğOúìx=ZÉ}R~ÑræÓqê‘dKA¢z7ÌÏö,'ğ§K	ç+Õ¾™67¼Rëú8?s&q(Õ.Š¢ g¿'¿Å#€Şe+¨!põzB¸ï'p\Œıë©”µë“¦J~“¾~1°±¢CUaFŒmPùYŠ”Ò2T}a™š8Jğ®}sˆò…J·[Ôø¬õ·Ôj™Kş ÈºtØ¹H{±×´üEy0û¿  5(±nè~	/Iarû¯_×@z¢È”¹	i å/ıpc!_¸£ˆæ‹¼ÉnQpKøo¬h•‘N‘7%ÖÙ—è“:ê¹×õ+CtN¡ùñM8ö­ë’9~óHP{WóŒÁ™Ö_ÄtËˆ1!R§\8»OóıÄ–~v˜ùf;e;“ÿ}{ºc÷ ï³'çp“`­§ˆüÔµ|\{xK»r²àKy«ÉæN?	3 ÙMŸóW¼!×]C¥“Y kºÈ4å?¡Ñ€”dÒLÌ+<NŞušÌ+…;aÈ\gê’_©µ)MÒ¿à»ÛÕ‹™ÜÓÁÆØA—aÌ%œÇ!ĞÕ7h#úèï®*­Ëå¸RÎ9ùb•³Ş‡ø£bû}år³Y’óXGhëÓĞ†9Q÷
•dMÓ1Ö¿[½!(±ªÑö¡àü˜Ævné\V\náî—ºYBî*M8ÏYğ?Æ÷€Cİß1—–êG1ñ-Ø'dĞR_•ïÃ‹ £«Å}ŞÒ\ŠZ|ÿÖë¾I­"Îø`¢Ì)÷ ØêVÿÜh08l1ª11³¢¹+£øU6†N¬¸ûƒûPÙïK.L|ÚH`½·'ÍÌøsk/|>uz©ª’+}*ÄËCY„¹gÁÇÄÆ³¥²ö5}p5v¿)ÇL¹ñ2$Õ§¨'	ÁÎ_xMÎº,£ñ¸×·.Î¤Âsn¿îL-Jşm…E´Ùİ¾mŸCÓ0Œò\üÒš”Z%ÕJA^{
dlÀc{(ìvtÍ¬liåei¼ˆ°Şo1øĞc¬æ ‚ÁcÉ³…îÛ„˜.d]ã-Ş/…Ã:XÒ,Öán,j£´ØRã€3ãD)ÀŠC.ÌRóxËĞÑw˜ç4r2<Â,Ğ™T¹êÆëOÃ¹K\a’$Mã¾‚âF†ÚË¿péÅ°~«Àx‘´m»³‡ª\»‹şíLgoÔt½ª2€ŒF1\Y€‹gáW¿”|ÊÔoêã•»Ç.×ŠeÎGtw%*ŠÄ^æt.ÍŞÂÿö_è”ŒàÌ9g÷¨n.,İÙË“s«n¹K*Š#	r-¥O´@` •¿™2Œãx ÊÊ}–Z…wÿ$Öù‘ÀºŠ,=Qç”QÔ‚‚3ĞêrÜÔü«ÂJN*N¡‘‚7 "‚i	scr²ùØ´7qÿ-„Ç‡šq)áR:<waéfÊfüu5/šÚ÷Î'\7÷€ïë^''ôpeİ¢x Š¤öœîÚ'ö¶$ugiê››w0ÅÚş=eôv†}€
†Š‰[š†k¹0Á²8N«dß×D^ÙˆÃBj‰|=Ï€ã«ršé^!—Å2ò=jzªGäA*+D1+ôÒ÷@
Ùi9x£¿+`Y.1R(ä¸Íká”oœº1Î=V3
&Ê.½…öÙ>tø’°¨È+93Ø6Sß=}ìï <õjZ!1¼t—íw¼9Çxİ»9‹ğü”œÊÒh½—FˆÒóÊfßéˆ$Ş:qgê½ïD2à&W,<”F†àFµ“H„™–N “vPš]°d‚ŒÑÈıg¼R0:uğöûMWùšò‹0\*(íâ
ÉìÄ.ƒĞÖip-izn$ÃPš[MÜËú5‚‹ë0(Oí{¸Êíé¾EkrÕ ¨+UÄ3ms'1”œì¡0T"t·±#Ñ°W~À)İ7mz†K	vÜ?p^:-Nû`šWpuÍ…<\ïy$$î1ÒşôM×iYñìW/@«±_"GP_Ñ—ø6‘°İÓÄClcH™<°a´zVÎã‰J_r¾A(•á“`âfAVxÔš]&1×•¸°tn)…DéÌëm¡kQa™E¡òİá@XJ,6öfj£ÊÅ°²$âĞÑ¸:î{÷’»Š$ú~êôk.Ú:¡õr§‘Y·ù R.´U @Gàê®	…mªZº¢Jp¡:Lv_ß>ôxx ¥]3‘Ğhß¢=¶Bkà¨o¯<¹Ù>nŒ Ó¢Md5ìHİ?ƒ·Ğ($“	ß|• †¡!£èw×Œ8Eo.ÂÅõõ²qmÊĞP
Çôñ?dafÃùBBaŸ6q®ÅR=€™r$]ª±÷ê«rş3Kã*µXëÉ§É][İ
±ËŠŠØÜ¾-ïg$pß-§W9öıìùµ	c³²ı‰^ƒr ‘1soŞª¾ƒ½ÖL3úgk°08õÏuÉ8qqdµx*q"òËÃËkî(…ĞñÆ\¿ªòB0]8¼?¹HûdÀÆßó È<Å´„Aˆ¦íIf¹–Ó~VhvnúZ1ÂEµlğXâú»‰`ÉTà„À½dM>}/3Q?ŒÍÖ0ğ¯"é¤Œì‰®¨Z6y:Íœ”¶Ğ¸É/Mu×·¹<ÒsŒ£K—mšû÷«Ü™<Á81¤…êPllI{\Ÿ	mrğ’Q œ"¬EÓ:Â|¾áÕT++lÑZFˆC³GÑÎ"²éôÁº£Z® 0–^õ£çÜFãc`úÇ_F‚C6hMP™víÜÔ	M—|'Î„.Ä´¶<V¹™wÏ¢4¯HV:,Ì{-¶
8e$B‹¥È½¥ƒaEkÊ¨Ûxw/¡PÀ“Ü»¿}d]ğ×oƒf®lWN×ü„äi)EJNÂ‚‚Pˆ <cìsßfÕ;ŸbóúÿŒx)nY ½åñ{6Âø‡7ÈaøamŸ¤|š5ş(ÑNÎ4{d2!{­UPoSµYúºy?¶€Á`¬aÏÇ‰àŒß†‡uYĞuÖpªÜ±?–‹ÚüŠÿæ$ÔS—½ª®uú²Ğ¢’§àSSIhœÏ|ÍôéOP‡³'pØ9Ò&
»Í¶‰Ş„·3Â'K)O‹Aryƒ…¼÷Vª&/D‘».gor­2gqîn¸_»Üğ9ÏûmjÑØŠT¢ºvz3éz9‘­}Àx†²¸én×¾-Ì˜€¦™àËÌØıK·:pXÔ‰	qLs¦öf^ş²_­DÛ«½«.¿İS•WfJï¨Òç“ƒ‰MCÃ“¿	ÿ‚±©*k$~lJvı’æZÆ•pó¡Ñıg…*^€¶¤[¦”˜±7¯Å>;WM&ËßT#Ç•ñ©J}NLt¼? ŒÏ\õ=UİÃî?×5ÉÍBÏ@“CJ=7½wæÊ¢\p´m¸]'Ğœ‡è»cût c)ûëK9Ìrã"ôyİ“K3•e‘¼1kä”Ëy§’»©€#*éş›³µ¡‡îÂRáÜ`0uï##ƒ	Š©:}®îwã•KH?¶Å)¥rˆe¬…Á„°2üıF‘†vEŞÍ²È”1] x³ahâáùE³ô·oe_S6€FáköÊwÙe•Dıà¼ü3ENE„4‹HWƒB†ç.Ÿuét…+(åT:Ss´©(Ú  )üª—Ë*aŠë\«€£õ+Àö„˜ Qüø(Ğšñ•ó ¡½€©wYÕUíü~‹ê´*Õ`-æ¼bÓE}÷ÿºş…G™cZl97	“r¡Â3ÆnˆaÍn¦Ÿx¯¸yµ¸6Áæ9J
û-¶€„^şM¹e’®û›ö5Ÿ|®9PP,V­µJœÏÜ¾˜z·ÿIbƒ®54ë}©EAºÓü"ö†º_ÍHã48jû(2ÀuT_‰R7½iæZošK}w	º¹6€¨ÑÃmú7AÁtC˜]¯÷i™=¦ÒXÒVÄªa”Ä{GFjAÙT0Ô/ZÃÓóõw%Ç%×“‹²ê¤ú÷'À»ŸAJ3ƒH…:=° pL«r¤9JJĞšã0õOIĞp3ÎU’¨ø	»–Ùãc™™|4ßÊGSP¹ÄõQCÄ¸½ş¿C{ü4§Fëcˆ/8 T&Ä›¥º¡1ŞTSc„¬áÓ˜\# d.î>–#7ó%Ì‡Ù˜÷ErˆZº¾ÉYÏYJ·N-Ë¶ã¦˜c4ü¦b{§¶ ãĞC‹Ş7f8u?éÇ:!îåQÜ{¯š
±‚y¸àßü÷q«¸ğqÑr†¡>ÛWùõµU­‰§ÑG.’&±"¦0I¶ñ,¾`Ñ'½²)k
•k«íØ
íDëŸÆ-’~õcºï›o+tRvÙğ¾uê¶£°1$â ãê®¨º&mìÖ5grü§¤jÅWY¨/fqß'\¸££DŞWZEûÒ=C±5ª¶GyrŠÉeÇé¢SM¼VÖ÷ıòõŒ$Ü~¿ĞêÍQĞy]—/)îÉÏ®ÌºÔÑ¹2ò5÷W~ù<“«3Wwa6Ïæ™ªfú;^­M4êã£›æºf`$B\K3Ì§ef»{w‡ñ†qYNúçp„°Z„@v¥+)œH9xÎÊÏûµ:´ÁK¥LHôÿıg/ÿèŸ–4KÄ™¨’DñÃ¡?ßn®ècÍáœÔØäà!…ô‡w[,´ø*n"­¯â(ş±®Õ¤õMàó_¶=I È‹ĞJ EçC¯íˆ[3{9knlÕfbé(ƒ‘*Šis:€"…]}ŞU?ŠXÕLÅ[Gî‚¥õ9pÜ\´l}Éüàà½}½“Æ4¼·ñHè¼Y'Ğ£/ç—òù!7.ygb ¶•ëäçbJdËâáå¶s´ÏÜ±`å¼ ÊØ+Û(Ü­ÇUÙõ¯<šëx‰Ô'8Ş›`Í’¯¢Ã”t¡ÂŠtóH‰{mUpÈ?£üºGŒwêS¶ÒÚ©öR?ql9#nÅguY	²®Ö¯H]”UåñÁ']tw(‰.u/6¥§²7
§‘æŒ>ryp|WØ¼òíá&Ì4Æ0]†ôq¸Î—Y_’e4iÂ½\iŠ«hÈ&à†"~ÈåM‚eÈ“>%NŒg€ç±¼õ™„ÆsÕ Z"°+Ç‰ZÀ@êr–	5Vó«7cBK¢ ™dæÁCQ\&æûI¤D!úË~À\…&.@æ¯`Hb¦)À=‘8E
Ù³€¥´úl,¿®(Jr>PÕCxæÆ3–Š€jw0×(™cUö¬TÖ©\˜6ºÛYr 9h!1˜µÂ~ÕCLÿÚşi2š‹ zÉ;,êoÊı‚©Ã­±Ñt¶'¹¸j<™‘!mQ{]	x*(‰¡İ‚’Œì¦ß
’Œg ’ö¬F¯[ëOÌÈd¯¨F&¼}§!!“nx‡µ­z%»]9İ±‡ŒMäî>~BÓfåñëgøÍÏµ×¼Ç4‰İ¸Gh•hR¡é ã·†û¥'‹©.M“q"o†öäá¶¸=Â‰‹¼H3™ô gÍ£#ƒ¾}.fßNÎìNgkCÓérµ‰×‹W×Ñ®Hã×'±Œ´±eÿÕ¤$ÿ‚î®]CJİ×b“œöákÍË
¬Œtzãd'ÛwBøPã«'òÓÏÓÀ¹ù Ñ?pÈşA…¨IŞƒ˜·KØ³h/ULfFıh-á±m¤†i	oEÕìsv“H‹¦ğëÜ	†ÉÙ#İç‡1H—~`±½Eˆ{RjaŸú`Tù+€’ÍZd­ª¾äv™0¸Cu*^Ã”zóVV{X½Ì§oTûPïBõYH$‹ú¦á·Bj÷HI5\xƒµs€Îé4¤bg1rî¸çZßCo‚U£Cêê Å(¡'ÑSõë\–-3­{â@²Şş–î“
ƒƒêã©½ëp\Ä€1'MB¯¸FÊkö­~0Ê£éüºº3 xhøX`u«”/ÇDÓ øo™QX5@?ü«¯‰àš{–nïı5ÊÄ\×Kzƒm™täşöGä@Éút£UæÅ ×xò}C¿>­*¸Ş¸ –7õ»³‰ı¾Úö\ÕhŠ1Æ--1ô™´U^ì"”k˜½#'pg„8Í\AwpÉ’±ÍLøÌôªj}Ÿ¹&4ŠkiÄôÂÇonèY<¨µS‘r%@>ë“ÈØ²ï<¸.jĞñ¹ù¸ÿQæúĞp¯Øêpa¡hAt0áÀiÆ”‘©­@¯†sJgƒÚu/½Xˆ\i%ÖWRØå+‘ê‹w2VLÙo/À#r®õˆ÷6Ÿ>ÛÖ^¿ÄÜEbsiıufª·lø(Ã»±8?é±cÿ/o¯§
2†,‘OÍÍH'øêİ±­8ŞJ0…Šù6Ä\~6‹/^:“Qç”5ÍS-8\ê”¢eİŸ÷ŠAi¹–#ˆIxdµµóòÔÅ"fzÕrÂäıÓ;ê·Q+ }‚P¨±ğ¶Fä%grÅG¡;;öD÷_!ü™v%gvZÿ±N Gó‚Íª™®ù¼VNû(}GFÃÅIœÓßª ¥n&‹l8'µ?m
LĞ"4–fí×“óƒZU*>Vá-[Â6­ÂAºÑ”…Nx™§Ø#œÁU‰[‹a:¤¨·»3¾2-¨êW™'5Üƒ)å¦sØ}¢¡Œ­	_ÆAôoDnf:jt$£\wJ´ÌiÍš¸üï¬Á=•ÀA¯Ø/·“ãG+ÖV_í…ÉÂ34ËØè|s£áy±y=ò8óÙú45Æ3Úù#%²±Ï[*zß°4j§ Vœ‡ZfçÍw9KÜ†eàëlŸ±æŸà6ãBğkmÇç{d‚è‚û°6è°1`äªWÁ ‹¾&Şß ‡Çój¼m"ş{vi<U?ÕÙûìxß™$¿¡t+zLÿyûDâsÙÅü¾ä%Uä2øû&ùõ“Í*Â;Ëåª›á9ÏYÖíµÜGèoıc{'còa9ÁäÈ™«3!ïnP’Òo›ZvÙ¤½@¢MçÔ[ÀPãû~y)k\y¯¦7òMY†)Û‰1Ç—cğÕ„«ß™Ö¸iUø¤0"1ÕÀ·ôÛÙUÔ‡Ñö*×¢ê,‹ş¬’:¨iIÌ	nZÏúşİ½*Â	ş„×‡à‰A%|­?Ç²—nIá4.<^û¿øéÅÂ{˜ J†BR¡E`E2îu1°Õ‘ÓÖI¡_CAUH;"DµT”S—fÛ?,T5Më«2íb+uÌ7}¤/j~KêIvX©+©­‡'¶/Gu™YR¤ÒàhÎÈu¬/–mõÜ¡`?ô†ÆÛ‹G#©^5ÂõaĞ•¹ànM»÷Â”òÙÕân#_Q’š71RtÑ”$%š¿µXeº½ 	>øI½}…U—"ó»â¯‹î¦šÅ€wöû{è ³lQÍ£>³ØùÍÇ”ìjIéùÛ_AtPĞH4#	bà‹ù/Añ×:K1±WzkLÔ÷—_%u›T¸4N=èu®™ecµÂjÒQlù¬ÉõÒ•ÃÅgÄòa!éÙğ¥èd¿!GCîí«Âš‘¢PL}üCny›~ïøów}OíC·WhËİ½Là% ?(õÀùV¹Æğ0OÑäå%-Dõ	
vkw7º·\änâC-œÔ0Xï¿`ËdÒÅ}~d.¤ëô‰pHXx1€Ì–è8_±ø«_ô{E²K°[¸;ûV)«=pTGŠ Q8ndò´•í˜>ˆ"È1È¬`ùN<XÅâš÷D%b³ZÔù¾õe§ŠtBŸ:e­5šâö>j'sK	Úk ,ƒù	2ĞågCê‘tû˜xq0)ù\#¥¸«ù{§°ÈOo0Œ‰Ÿ©ê”âP>4œ-ÏS#éÊĞç=ÄVC\-–ğ×L‚…)ĞK:ÿ¨™ãëïá	7ğÜg›JQ:tyâ<—n¢`¸)üÙ¢¡ÏF¹U”ü:Á‘´áæŒc‘@ÉHC.Kâ?\—×Ğê>Õ³w²À÷Vá{’Õ7AHE9~ªü7Å¥ØqA[ š^;Y¦ıßÕµ{hĞòá…ÊËgÁŠÆ(vÉæ‘\.NØj¡±hÒRHh=‘­³’Z+@öµ›ÿRÏ:!1@rğ¤€Ab lÄyœè	Â(ü±®Ş‹g‡ØO-ŠÖt€0Ê¶Œx88‚^ïò;Áé“	Ã¤:y	"ŸTÕóã,w4Á³2Cë¹ŠÕr†íp*˜ı‚iëG½RÇÿTÚ¨u^:|ÓâéG0¹Q›gã3Å¡İ°õ6tôxµtÒ*JéY
 8¼˜›/ØyÖz`wR˜³Û3˜x¾nƒÑ]d>v&“qW÷š×ß?ì`Â€ıKÌï 0 ôyFÉ5YZàQY³@0]šá~Ø’ÂsD EŸ‡oÜ5xÌï`MÆÉT—›ƒUòôóıV&U@R2~Ãˆo¼ß—-åÊ'5ûø|Ía¯Y¿ÀDÒ„”5„ —?èƒúÒÍ.Áƒ˜}†Ü˜+6h`¬çÓ°†YË•t¬_<¯HQÙSÄ™f	6¤Êş´Jh6xÑîÙ#wë+Í+fÃjs÷ªGz%Âï„æáÄ.…œNQë•:©omOoaèJ4…éUXló¬OÔK.ûÜİ¦¨nAœğSXU„î¡iæd~ğıíHª½hÆ,Øš¢»½}ÍDù}eORŠ£v~ê/]Ab›\2
µ1×¨EAk~a{zÚât™ZÓğ£ÇÚSæ_íí8µ9$ŒğÉ§‚®zëKØz_ÛŠ×`ZÀ rï4Éîü¯J<`Õ;HüÙßIt‹¡¬€¤ª®­ÎÄÇ:3î»G™À¼½ğa¾{Ÿÿ‡¤Ê/O ¢$•ˆ›šŠÖ'®?ç œôy
ÊDÓım1”%ÄHezSsä^-¨A”íõ¥m«¬ÏU™hßQ2ËW¿Ğ9oW
âQ†ÉcU¶ômÌK±BÇÚÍåK¯¼Ml­»h2B¶1‰ÓäÖõEÊCPğN“Ñ3bz#X½ş…áxuÅ±)«VLµçqŒª{ÇŸL­!××%@úµßÅü’o.Ü…~/ş#‚&ÖábÂ 9oi'±WN}íÓo‹Ÿ·¿vÄ– (Şñ³ò*Y`j™ —•ã$¤•R‹)¾ÙDN¢ˆ¤–²}ù
ôjº“!r]OÊ,…>½·r²Äƒß5Im†¶İ	¢Hœ(şøz«É)„ƒêkªm·ÊŠÏ4SÇ ïŠ3LœÀ7ª­;r¦=…ŠEµÌÙX>c³²³Ÿ÷D£‚üwÓP	† ¥û íz]Ì¼#Që-HDOâ­QL†£_e¨xe¥‚©qaÑ«°‡ğ
ÛŠ—ú˜ã°(²€‰«C<Ó²›yovåYÃN™[xæQO¾>«%Ïcq>yfšèTïg 	İéÿeÙ.†8Ù`Â´”3Œâí½5²êø†Z}ğ‡ÖR¥Ò?©å€¡ğé3åOÉ–%«¾Ç¼›ÀêÍwÇæ<~grG Îét@lÑ”>…`6‰<˜@SŸGO3Ìh©s[ËC*Q?Má˜¢£Â­Ÿz-"Í@º®±G5Çÿ½úíÿ{/
Æ›_[“)vÙâ•LO3ôZñÊÉÆü…ƒ@Hèã¾¾Ó^Ú5x=Ë79½(Ï\ö§Î×1Éá%r¿—Ùèó÷§O¦¥(›Ä3>º*´á@›E¨*X¥^ı)u)ËxDR_¡ÑÑetîå®…0b+»£ÏL)Ÿ«^ÕOÆJ8#°SÔ°]Ü¦á¥°Sˆãe®cÚà(ø.*£=JW@¬¯1]ö¦ê½G:~¿„qFŸEPİ’)]'˜}Q¾Cl8“Ãi™Ü-©÷”rK°V±Âİ!!ÏK­›.ÌÉC¨‚ğ–ıhŸ5Hœûv»p¯ƒVÙ®Ù6=ÇÊ‚¤`|*Ê*¹¬ØİÆ	TÉD‡‰?CÒôœ ,«·Góçu)koİ…2pÖå•.Ó˜ßù‘l»ÓIÄ	Ëğ3Ryîğ|ÅûØÁ„êI|oLGL»_˜ğŸçuÆåÖÁ¸”ô6o`ø®ì‚QoúÔaQµÈOyÀ.™É¦½I…öã†xŸ¹©…â*!µ7~Ö‹ÿtuÅóhÇv5rá AŞ×“%7”êbMÑ5Oe"¢“ÔeZzRÖÚzt²E „xµ?_ÍƒÀ•¶²	J(3¿™>ÇS¡"4ÏñÉÛæ}Ãºy_¹R8TÁ¾ ÊõâÔ•·¾ÅÀBVûPË4/Æú4ˆº¾K^>DÜTº•o}»P·¬ÿ;&¬O:¥Ïé%Pñª&ôr0ËOî lz­µl7Y²qF85üF—Éß!°Ãİ
OÆŞu´3X^­2uaİ~áÉê*¾^LÌh]RL}³ u…‘‰àUò‹Î¶ç¢0P#A¦L}½`Œ	 i³NêÒdKŸŸøŠô¨åá+Ì„¡›Ç3øšµÈú´¦Ç¯§2Tğø€wºYšñÎ»¹$ëújÔÄøÆÙbµ"xN•DÑ$Xbbš
Ùk·g›k›à$CvÏºoÇ~’÷.,ì¢øşì6Ê¼ÖˆúÇTÖ»çƒæº»êÚ
š^kÊ:/èy¯23)_§›‘}ì,Zòp
ÑÜâİÆ<¨T¥ÈH¯Jšár(áÏƒ¸z®v'§eÛYÈ”pó¡İî@Q‘3ïBN )-mwı=ZgrªÚZòd±à
´ß™‡Zñ»FÜø*›°…kDŒ=3›%Càí`’ÀÇR9·‰É'3€pÍu„àŞ€z§Ng]ğ ÜQ%ù£\ÿ 9‰µĞG
(}Èòº×¨íƒpõŠ5mZò9h…,‡™Ìu"YĞk×S|{å³Ùü»E"^T/ \ï7â˜˜úÙ¦¡óÿÂè
Sˆ,Ó²7N`•qhŞşdÿÖŸÿ–¬şµ¿ÆÇÈ°"ÂT»1l‡êÄ·Ç7ˆíªŸ]ànÀ¯°®Çî@Ú×"Û	0‹ê'ŠÑúÛTï#B‰ıÊ‘¡¯…×øÔ«<îÕoÿfà;éçùLÓ81;ûNæFÈúÒ{¯©³°r…šO-HñpñA0„;ùLs_)y=$àV±âm©@(œ¿Åİ½/DìÑòÑGí½ä7ŸŸr³zC‰Ã{ªí/ğ÷Â© ÚM¢U{¼âAÄXª'okŞÒ˜Æ›fãHpÛòØìL¹M×Á\Ô‰û¼e÷G¥sWÂZo
7\¢“…q[j~N©[Çş_¹³Y¯ô9ü‚%DêáJ1Q“ìêÕöyVISôT$8+Ğc•L\K-™BŞt$ÔLaÆü*.n30”F
¶zxCY©¿Zşğ”Â4ÿDçÒn´œ7øŸ¡k_‡Ó¼q<ü+ÖX{®L¯_?ƒç_àOª¸`Ze2QİÙì—œ©0ár¹{ü+ye6‡·Éaèğv2Äù%åèÍFNi},soolÒÎİxÿyÌèŞw/Å5Y‹ı¥úFË9dAsâ–¿dœ{eÊpe«‡Ã²N‰ëOêJÊ 9óƒèé²7w^T«ö§Eóìc'c•Ï²Ò‘€ˆ…"~ÌƒšCà¼OX—PğıGÁM€Û“X{.ğV©È3ã&ôÚªq%½u‚»İïYŠêc8+'@™	#¢]â­½¿Ì4êw@x¯f–hfOkÕ4©:r1µcx &)û·ãSãfCÎótWüß”CÈ‡¤ğ7Ï~êt?£âéß¡Ş¢œ›hoŠÄÒŒŠ±Ú»äódÉ !Âêï­Œ±£¦:Q6üLXV¶õ£@©ĞƒwÔˆ_º…›Äİ˜ÿsMqb@ˆ·XöC¼ï,Rı7Ÿ¾ë¸úyS|WW ŞZH²"TI9y?]Ü/Ó@ß³šçæ›NÂÂÎCml<OK—¤11³qÚ»>íy9Ä¿Ü¬´mmŒ Æs»UÉ„ÛúêÃ¼Ò‘…ÃÄ œë@y¶ş/\ßlGç¢Ì`Hú¸‡fõ[büân
y5xv±4·‘Œ2F²–’Æ
Fr|GüÑuM¨QĞ?	9]¨Ñƒú/„;ÉİzOë5€/3ö0±…çâM¼÷»¾nDÏİ‹n4KşzÀíGöulcï ½|œ£oI^…¸+”õ,¦c Ã[Ádë±ªØîÍSù§“'áXL•Ò}ÇîÔ‹¦¨Zs|Ã†ÈıT~¹.4Üúx7D9kUÿØxäÁª.èÕ)µ’¢‚8°$«Û¦Ø*ŒÑÒ»‚0=»XÚãh­˜qµi­W^~ªİÌ7JOëQ!ƒ8ó×5ÉÄ ¸-Ğ%4Rµ×æ­"yß_æÅ(3p áV›ãö"¾Zş ¿µ/2cü¿ıreçYÑ`Ñİè³Åw0t¦|0RÇïoÑâ¡÷B
ÄË•AºÌHOXÜ÷p\}£/®GìÔ"m=õš›ì6Q5óªääŠ|Gğñ.‚pç`öVè¯¬"6msB”cÌavç!CWt˜CqçŞAı?Ej1íd¿“ È©Eåı]Ïôò¸ZÄhø@ìæ¾VÜÏø}+Æ õºNŠ™aÄ)ÇjØŞ£CôNÕ]0TÎ8Z†”¥éÈm²rlüì}–§Èı;3œ¸8mÑEF:-ˆ†üsp@:]ImŸ6Û·°íÄš’`2¸gøÌØ Ÿ+œ%6[yŸ•«ÙÉròë|Õ3—Š32¸,…GtÕvuİI–¤Rî±í†Ò&óaüåÁg—¥·x|åø'RØ¼Y}=include/linux/memcontrol.h \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/rculist_nulls.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/indirect_call_wrapper.h \
  include/net/dst.h \
  include/linux/rtnetlink.h \
    $(wildcard include/config/NET_INGRESS) \
    $(wildcard include/config/NET_EGRESS) \
  include/uapi/linux/rtnetlink.h \
  include/uapi/linux/if_addr.h \
  include/net/neighbour.h \
  include/net/rtnetlink.h \
  include/net/netlink.h \
  include/net/tcp_states.h \
  include/uapi/linux/net_tstamp.h \
  include/net/l3mdev.h \
  include/net/fib_rules.h \
  include/uapi/linux/fib_rules.h \
  include/net/fib_notifier.h \
  include/uapi/linux/sock_diag.h \
  include/net/sock_reuseport.h \
  include/linux/filter.h \
    $(wildcard include/config/HAVE_EBPF_JIT) \
  include/linux/compat.h \
    $(wildcard include/config/ARCH_HAS_SYSCALL_WRAPPER) \
    $(wildcard include/config/COMPAT_OLD_SIGACTION) \
    $(wildcard include/config/ODD_RT_SIGACTION) \
  include/uapi/linux/aio_abi.h \
  arch/x86/include/asm/compat.h \
  include/linux/sched/task_stack.h \
    $(wildcard include/config/DEBUG_STACK_USAGE) \
  include/uapi/linux/magic.h \
  arch/x86/include/asm/user32.h \
  include/asm-generic/compat.h \
    $(wildcard include/config/COMPAT_FOR_U64_ALIGNMENT) \
  arch/x86/include/asm/syscall_wrapper.h \
  include/linux/set_memory.h \
    $(wildcard include/config/ARCH_HAS_SET_MEMORY) \
    $(wildcard include/config/ARCH_HAS_SET_DIRECT_MAP) \
  arch/x86/include/asm/set_memory.h \
  include/asm-generic/set_memory.h \
  include/linux/if_vlan.h \
  include/linux/etherdevice.h \
  include/linux/crc32.h \
  include/linux/bitrev.h \
    $(wildcard include/config/HAVE_ARCH_BITREVERSE) \
  arch/x86/include/generated/asm/unaligned.h \
  include/asm-generic/unaligned.h \
  include/linux/unaligned/packed_struct.h \
  include/uapi/linux/if_vlan.h \
  include/crypto/sha1.h \
  include/net/sch_generic.h \
  include/uapi/linux/pkt_cls.h \
  include/net/gen_stats.h \
  include/uapi/linux/gen_stats.h \
  include/net/flow_offload.h \
  include/uapi/linux/filter.h \

kernel/bpf/reuseport_array.o: $(deps_kernel/bpf/reuseport_array.o)

$(deps_kernel/bpf/reuseport_array.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      ELF                      T      4     (               èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[ÃèüÿÿÿUWV·ñSƒì‹¨”   ‰D$¡    ‰$…À.   ‹$‰ñ»   < ‰è‰úèüÿÿÿ…Àtƒë„M   ‰ñ‰ú‰èèüÿÿÿ…ÀuèƒÄ[^_]Ã´&    ¶    èüÿÿÿV1É‰ÆS‰Ó‰<  º	   èsÿÿÿûD¬  t+û€»  tSû }  t3‰ğ¹   º	   èJÿÿÿ1À[^Ãt& ¹    º   ‰ğè/ÿÿÿëÒt& ¹   º   ‰ğèÿÿÿëºt& 1Éº   ‰ğèÿÿÿë¥èüÿÿÿx(	˜ ‹P…Û   UWº@ÿÿÿVS»€ €ƒì‹Bx‹p|‹Bp‹h|‹Btº €  ‹@|…À•À¶ÀÁàf‰D$¸   )ğ9ĞOÂ¯Å…Àÿ  HÂÁø‰ÁÁá)Á‰È÷ë‰øÊÁùÁú)Ê‰Ñ1ÒƒáfL$·Éèjşÿÿ¸ €  9ÆOğ¯õ…ö†ÿ  IÆÁø‰ÆÁæ)Æ‰ğ÷ë‰ø2Áşº   Áù)ñƒáfL$·Éè"şÿÿ1ÀƒÄ[^_]Ã´&    ¸êÿÿÿÃ´&    v èüÿÿÿWVS‰Ã‹@‹P‹Rèüÿÿÿ%   =   „k   »ûÿÿÿ‰Ø[^_Ã                                        0                                                                                                                            6%s: Frequency: %u Hz
 7%s: write: %02x %02x
 wm8739 èüÿÿÿVppS‰Ãÿ°<  Vh    èüÿÿÿƒÀ   ‰òèüÿÿÿ1ÀƒÄ[^ÃVÿt$‹D$ƒÀpPh   èüÿÿÿƒÄéE   ÿ4$V‹D$ƒÀpPh    èüÿÿÿƒÄéw   ‹S·CŠ  Q QPÿ²   ‹CTÿ0h0   èüÿÿÿC¹À  º@  èüÿÿÿ‰ÆƒÄ…Àu
»ôÿÿÿéI  ‰Ú¹@   ¾À   èüÿÿÿ1Éº   ‰øj èüÿÿÿ¹	˜ º°   ‰øj h0Æ  j h  j hÿÿ  j j èüÿÿÿ¹		˜ º°   ƒÄ$‰†0  ‰øj j j jj jj j èüÿÿÿ¹	˜ º°   ƒÄ ‰†4  ‰øj h €  j h  j hÿÿ  j j èüÿÿÿ‹ü   ‰~l‰†8  ƒÄ …Ût‰øèüÿÿÿéI  –0  ¸   èüÿÿÿ1É‰ğº   Ç†<  €»  è   1É‰ğº   è   1É‰ğº   è   ‰ğ¹I   º   è   1É‰ğº   è   ‰ğ¹   º	   è   ‰øèüÿÿÿéI    3%s: I2C: cannot write %03x to register R%d
   6%s %d-%04x: chip found @ 0x%x (%s)
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                wm8739                                                          €       `                                                                                                                 debug parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=T. Adachi, Hans Verkuil description=wm8739 driver  ¼           ¤ÿ      GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                                          
                                       	        2    	               *       c     H           >   2   =    	 W           k      ö     y      3     †   o   ˜   	 ˜   @         £   °                      ³            Æ       €     Ô       
     ç       0                   ñ   €   0       `                   '          @           N  ¼        `  0        w  <          [        ¨             ³             Ñ             è             ğ                          '             @             M             b                          ‘             £             »             É             İ           é             ø      
           0     )              wm8739.c wm8739_remove wm8739_log_status wm8739_write.isra.0 wm8739_write.isra.0.cold wm8739_s_clock_freq wm8739_s_ctrl wm8739_probe wm8739_probe.cold wm8739_ops wm8739_ctrl_ops wm8739_driver_init wm8739_driver wm8739_driver_exit wm8739_id wm8739_core_ops wm8739_audio_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free _printk v4l2_ctrl_handler_log_status i2c_smbus_write_byte_data __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_cluster v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__wm8739_id_device_table param_ops_int     "     #     $  !   "  :     [   '  s   '  ‘   "  !  "  !  "  4  (  E     h     D                   h                                            "          %  &   &  @     E   %  ^   	  c   %     	  “   %  ¥   )  ¿     Ê   *  Ú   +  ä       ,      .  ,  8    a  ,  ~  $  “  -  «    ¹    Ç    Ø    æ    ÷    ş  .  M     k     ¸     ƒ           "          /     0          2  @     H     `     €     ¸              /     5        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   S                    	   @          p               )                €                   %   	   @                         /                                  8                                  4   	   @       °  8               E      2       <  7                 X             s                   T   	   @       è       	         g      2       |  V                 z             Ò                    v   	   @       ø                  ‰             æ  
                  …   	   @                        ˜                Â                   ”   	   @       (  (                             Â  u                  ­             8                    ©   	   @       P                  µ      0       L                   ¾              _                     Î             `  (                                ˆ  `     "         	              è
  7                               p  á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      ELF                      ¤k      4     ( & %             èüÿÿÿUd¡    ‹€p  ‰å]‹@™Ã¶    èüÿÿÿU‰Âd¡    ‹€p  ‰å‰P1À1Ò]ÃèüÿÿÿWVS‰Ó‰Ê·Kf…É„‡   fƒùuq¶K‰ÎÀéƒæƒø„¤   Áá¸    Æa	ñf‰BBˆJÇB    ¶K¶ÇB    ƒãƒËaˆZ‰ËƒáÁã	ÙˆJ	1Éf‰J
‹|$)ĞÁøÇ   [^_Ã[1À^_Ã´&    v ¶‹|$¶KƒàÇ   [ƒÈa^_ˆJˆ¸   f‰B¸   ÇB    Ãv €ù	„¿   ‰ğ<	„   ¹	   ‰ğ¶øAÿƒà9ÏDÈ‰ÈÆ{ÁàÇB    	ğ¾$   f‰r¶ñˆB¶CÁæ‰÷ÇB    Îy $ ƒàÏa   Áà	ø1ÿ‰B¶CÆBcƒàğf‰z	ÈÇB    ˆB¶KB ÇB    ƒáÁá	ñ‰Jéúşÿÿ¶    €ù•ÁƒÁéjÿÿÿ´&    v ¹   éFÿÿÿ¶    èüÿÿÿU‰å…Àt‹ ]‹@$‹  ‹€   Ã¸    ]‹  ‹€   Ã´&    ¶    èüÿÿÿUWV‰ÆS‰Ó‰Ê·Cfƒø„“  ifƒø„Ç  fƒøu)¶CÆaÇA    ˆA¸   f‰A¸   [^_]Ãt& f…À…ÿ   ¶CÆaˆA1É[¸   ^_f‰JÇB    ]Ã¶    fƒø „Ş   fƒø$…„   ¶C‰ÁÀèƒáƒş„o  Áà¾   Æa¿p  	Èf‰r½   ˆBÇB    ¶CÆBa‰Áƒàf‰z
ÁáÇB    	ÈˆB	¶CÆBa‰Áƒàf‰jÁáÇB    	ÈˆB¸   é.ÿÿÿfƒøu:¶C‰ÁÀèƒáƒş„İ   ÆaÁà[^ÇB    	Á¸   _]f‰B¸   ˆJÃ[1À^_]Ã´&    f¶C‰ÁÀèƒáƒş„k  ÆaÁà[^ÇB    	Á¸   _]f‰B¸   ˆJÃ¶    ¶C‰ÁÀèƒáƒş„#  ÆaÁà[^ÇB    	Á¸   _]f‰B¸   ˆJÃ¶    ¶CÆaˆA¹   [¸   ^_f‰JÇB    ]Ãt& Æcéÿÿÿ<	„Ø   ¾	   €ù	„Ô   Fÿ¶ùƒà9÷Dğ‰ğÆ{ÁàÇB    	È‰ñˆB¶ù¸   f‰B‰ù¶CÁçÁáÇB    ‰ÍƒàÇB    ÁàÍa  	è‰B‰ÈÉy  a p	ø‰B¶CÆBcƒàğÇB    	ğˆB¸   f‰B¶CÇB$    ƒàÁà	È‰B ¸   éiıÿÿv ÆcéØşÿÿÆcéşÿÿ´&    ¾   é,ÿÿÿ<•Àpé-ÿÿÿ´&    fèüÿÿÿ1ÀÃ´&    èüÿÿÿéüÿÿÿ¶    èüÿÿÿUWV‰Æ¸    S‰Óèüÿÿÿ‹F(…Àt2‹€”   ‹x<‹h@¸    èüÿÿÿ‰{‰k‹F,‰C[1À^_]Ã´&    f1ÿ1íëÔ´&    v èüÿÿÿPÇ@àÿÿÿH‰P‰P‹    Ç@  ¸    éüÿÿÿèüÿÿÿUWV‰ÖS‰Ã¸    èüÿÿÿ‹C(…Àt2‹€”   ‹x<‹h@¸    èüÿÿÿÿs,UWh    VèüÿÿÿƒÄ[^_]Ãt& 1ÿ1íëÔ´&    v UW‰×VS‰Ãƒì d¡    ‰D$1ÀöC…§   >t& ‹K$S0‰øèüÿÿÿ‰ÅöC…Ù   ‹D$d+    …ğ   ƒÄ ‰è[^_]Ã¶    èüÿÿÿ‹s$S0‰D$‰øèüÿÿÿ‹s‰Åèüÿÿÿ4…    œ$úƒFƒƒV èüÿÿÿ‹L$1Ò)ÈFVƒF÷$   „zÿÿÿûétÿÿÿ´&    ‹B ÇB     ‰D$‹B$ÇB$    ‰D$‹B(ÇB(    ‰D$‹B,ÇB,    ‰D$‹B0ÇB0    ‰D$éÿÿÿt& ‹D$‰G ‹D$‰G$‹D$‰G(‹D$‰G,‹D$‰G0éÿşÿÿèüÿÿÿ´&    t& èüÿÿÿU‰åWVSƒì‹P‹M‹u…Òt]‹x‰Ãºÿÿÿÿ¸êÿÿÿ…ÿt=ƒ{ t7‰$…Ét=…öt9¸ùÿÿÿºÿÿÿÿşÿ  w‹$‰ñ‰øèüÿÿÿ‰s1À1ÒÇC   ƒÄ[^_]Ãt& ƒÄ¸êÿÿÿºÿÿÿÿ[^_]Ã¶    èüÿÿÿWVS‹t$…Àtb‰Ó…ÒtB‰Ç‰Ê…ÉtB…öt>9ó‰ñFËèüÿÿÿ9ówÆDÿ ¸ùÿÿÿ[^_Ã)ó71Ò‰Ùèüÿÿÿ‰ğ[^_Ãt& ¸ùÿÿÿëİ‰Ù1Ò‰øèüÿÿÿ¸êÿÿÿëÊ¸êÿÿÿëÃ´&    èüÿÿÿU‰åW}S‹H‹‹Wÿp‰ØèaÿÿÿYeø[™_]Ã´&    èüÿÿÿU‰åW}S‹‹W‹x…ÿu?…Ût#…Òt‰Ñ‰Ø1Òèüÿÿÿeø¸êÿÿÿºÿÿÿÿ[_]Ãv eø¸êÿÿÿºÿÿÿÿ[_]Ã´&    ‹Hÿp‰ØèëşÿÿYeø[™_]ÃfèüÿÿÿUWV‰ÆS‰Óƒì‹º¤   d¡    ‰D$1À‹‚    ·’”   ‰$…ö„6  ‰Í¶N¸   Óà%¿ïÿÿ„û   ·Fƒà÷fƒø…  ‹K$)×‹†ä  ‰s‰L$‹KT‹s8‰“¤   ù‰KT+KXÊ‰4$‰S8ƒı„ó   ‹„¨Ø  ÇD$    ‰D$èüÿÿÿèüÿÿÿ‹D$L$d‹    h‹²p  ‰Šp  ‹@…Àt,‰Ú‰l$èüÿÿ…Àu|$ ğÿÿwÇD$ÿÿÿÿ‹EƒÅ…ÀuÕd¡    ‰°p  èüÿÿÿèüÿÿÿ‹D$…ÀuO‹ST‹$)ú‰K8‰ST;SX‚  »¤   ‹|$‰{‹T$d+    …ô   ƒÄ[^_]Ã´&    v 1ÀëÙt& = ğÿÿºòÿÿÿFÂë¢´&    f‹¨Ü  ÇD$    èüÿÿÿèüÿÿÿud¡    ‹ˆp  ‰L$L$‰ˆp  ‹E1í…Àt3f‰Ú‰t$è%ûÿÿ¨u|$ ğÿÿwÇD$ÿÿÿÿÑèƒÆ	Å‹…ÀuÒƒå‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹T$D- …Ò„şşÿÿú ğÿÿ¸òÿÿÿFĞ…í¸   DÂéáşÿÿ´&    èüÿÿÿ´&    fèüÿÿÿUW‰ÏV‰ÖS‰Ãƒì‹@ …Àtèâÿÿÿ‰Å…Àx|‹‹‹‹èüÿÿÿ‰Ã…ÀxS‹)Á‰ëtGº   ¸   ‰$‹.9ÑFÑèüÿÿÿ‹$ƒøv9Á…üÿÿÿP9Á‰èEÊº   èüÿÿÿ…Àx,Ã)ƒÄ‰Ø[^_]Ãt& 1íëˆt& ƒÄ‰Ã‰Ø[^_]Ãt& ƒÄ‰Ã‰Ø[^_]Ãt& èüÿÿÿU‰åV‰ÆSƒì‹M‹Ud¡    ‰D$1À‹E‰L$‰$…ÀtYƒât,1Û‹V‹èüÿÿÿÃ…ÀIÃ™‹L$d+    u?ƒÄ[^]Ãt& ‹…Àt"‹@ ‰âL$èÌşÿÿ‰Ã™…ÀxÍ‹L$‹$ë²t& ¸êÿÿÿºÿÿÿÿë´èüÿÿÿ´&    èüÿÿÿ…ÀxOƒùtJV‰ÖS‰Ãƒøw4‰Ø™÷ş…Òu+ƒû6‹D$Ç@   ƒşwVÿ¸   …òt´&    v 1À[^Ãv 1ÀÃt& ƒş[^”ÀÃ´&    èüÿÿÿ…Àx?W‰Ï1ÉV‰ÖS‰Ãƒøw!‰Ø™÷ş…Òuƒû~&ƒëƒûwƒş”Áƒÿt3[‰È^_Ãt& 1ÀÃt& ƒÿuè‹D$Ç@   ƒşwØë´&    f‹D$1ÉÇ@   ƒşw»Fÿ…ğ”Áë±t& èüÿÿÿƒø'wVV‰Ö™S‰Ã÷ş1À…Òu"ƒùtrƒû„¡   …Ût;ƒûuNƒş„­   [^Ãfƒû$u;1Àƒşuï‹D$ƒx”Àëâ´&    1ÀÃt& ƒşuÎ‹D$Ç    ¸   ë½fƒş”Àë³´&    ƒû të!ƒãûƒûuœƒşu—‹D$ƒx”ÀëŠ´&    ƒû$tˆéyÿÿÿ¶    ƒş…jÿÿÿ‹D$Ç 	   ¸   éVÿÿÿv ‹D$Ç    ¸   é?ÿÿÿt& èüÿÿÿƒøh„²   w ƒøf„—   º   ƒøguE‰ĞÃ´&    v ƒøjtkº    =    tâº    ƒøitØ=º   t~=»   u9ºà  ‰ĞÃ´&    º€  ƒøet±ƒøQtywÒƒøtbº    ƒøPt›º    ƒøt‘éüÿÿÿ´&    fº    ‰ĞÃ´&    º@  ‰ĞÃ´&    ºÀ   ‰ĞÃ´&    º   éIÿÿÿ¶    º    é9ÿÿÿ¶    º    é)ÿÿÿ¶    èüÿÿÿWVS…ÒxT‰Æú   ‰×‰Óƒú ‰NÑ‰Ó‰N‰Ø[^_Ã»   ¿   ºÀ ‰øèüÿÿÿ‰F…Àtø‰F‰Ø[^_Ã´&    »êÿÿÿ‰Ø[^_Ã»ôÿÿÿë·´&    ´&    èüÿÿÿUW‰ÏV‰ÖS‰Ã¸    ƒìèüÿÿÿ‹C(‰$…À„™  …ÿt	9{…‚  ‹C,ƒø"‡Í   ¾€    ‰D$…Àˆº   ‹<$ˆ„   Å4  ‹n‰D$‰ú‰L$Â‹C9h…   ‹DÏ9Âué!  f‹ 9Â„  ;Xuñ‰ğ‡C‹$‰D$1Àèüÿÿÿ‰Æ‹D$8  ‰D$…öuéÕ   ¶    ‹$‰ğèüÿÿÿ‰Æ…À„»   †T  èüÿÿÿ„Àuİ‰õ1É…Ét5‹D$öD…u*‹mh…íuê»êÿÿÿ¸    èüÿÿÿ‰ØƒÄ[^_]Ã´&    ‹D$| ‹D$‹DÅ9ÇuëÀ´&    v 9Ót ƒÁ‹ 9Çt©ƒx ‹Puê…Òtíƒz tç9Óuà‹|$‹CIÁâ”¾Ø  ‰B‹$‰ğèüÿÿÿ‰Æ…À…Eÿÿÿ‹D$1Ûèüÿÿÿéfÿÿÿ»şÿÿÿé\ÿÿÿ»ÿÿÿÿéRÿÿÿ»½ÿÿÿéHÿÿÿv èüÿÿÿU‰ÕWVSƒì‹€ä  d‹    ‰T$1ÒÇD$    ‹œˆØ  èüÿÿÿèüÿÿÿ{d¡    ‹°p  ‰t$t$‰°p  ‹[…Ût:f‰|$>t& ‹K$S0‰èèüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÈ‹|$d¡    ‰¸p  èüÿÿÿèüÿÿÿ‹D$‹T$d+    unƒÄ[^_]Ãt& èüÿÿÿS0‹s$‰D$‰èèüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ‹L$1Ò)ÈCSƒC÷$   „MÿÿÿûéGÿÿÿèüÿÿÿ´&    v èüÿÿÿU‰ÅWVSƒìd¡    ‰D$‹…ä  ÇD$    ‹œØ  èüÿÿÿèüÿÿÿ{d¡    ‹°p  ‰t$t$‰°p  ‹[…Ût=t& ‰|$>t& ‹K$S0‰èèüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÈ‹|$d¡    ‰¸p  èüÿÿÿèüÿÿÿ‹D$‹T$d+    unƒÄ[^_]Ãt& èüÿÿÿS0‹s$‰D$‰èèüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ‹L$1Ò)ÈCSƒC÷$   „MÿÿÿûéGÿÿÿèüÿÿÿ´&    v èüÿÿÿUWV1öS‰Ã‰Ğ‰Êì¬   d‹    ‰Œ$¨   1É‹Œ$À   ÇD$    ÇD$     ‹¬$Ä   ‰L$$·K‰\$ƒá÷‰D$fƒù…«   …À„.  ‹ƒä  ÇD$    ‹œØ  èüÿÿÿèüÿÿÿ{d¡    ‹p  ‰T$T$‰p  ‹s…ötBv ‰|$>t& ‹N$V0D$èüÿÿÿ‰ÃöÃu|$ ğÿÿwÇD$ÿÿÿÿƒÇÑë	] ‹7…öuÁ‹|$d¡    ‰¸p  èüÿÿÿèüÿÿÿ‹t$‹„$¨   d+    …   Ä¬   ‰ğ[^_]ÃfèüÿÿÿV0‹^$‰D$D$èüÿÿÿ‹v‰Ãèüÿÿÿ4…    œ$úƒFƒƒV èüÿÿÿ‹L$1Ò)ÈFVƒF÷$   „=ÿÿÿûé7ÿÿÿt& t$(¹    ‰÷‰t$ó«é¼şÿÿèüÿÿÿ´&    ¶    èüÿÿÿƒøQtfwƒøtOº    ƒøPu%‰ĞÃf=º   t)=»   uºà  ‰ĞÃ´&    v º    ƒøtÑéüÿÿÿº   ëÄ´&    fº    ë´´&    fº    ë¤´&    fèüÿÿÿU1íWV²„   S‰Ã‰Ğ8  ‰ßƒì‰t$4Õ4  ‰T$‰L$‰4$‰D$‰è…ít‹t$öD·tD‹$1Ò4‹D$‹DÇ9Æuë(ƒÂ‹ 9Æt‹H…Éuğ‹H…Étì‹I…Éuâ‹ 9Æuåt& D ‰Å‹h…ÿu¤ºÀ  èüÿÿÿ‰Æ…À„‡   ‹D$1Ò8  ‰D$…Ò~$‹D$öDƒu‹[h…Ûuê‹D$‰01ÀƒÄ[^_]Ãf‹D$‹<$‹DÃ,;9Åu'ëÓt& <RƒÂ‰L¾R‹x‰9‹x‰y‹ 9èt®‹H…ÉuØ‹H…Étì‹I…ÉtåëÈ¸ôÿÿÿëŸ´&    fèüÿÿÿUW‰×‰ÂV‰Æ1ÀSèüÿÿÿ…Àt=‰Ãt& ƒT  èüÿÿÿ„Àu‹P  ‰ú‰Øè‚şÿÿ‰Å…À…ˆ   ‰Ø‰òèüÿÿÿ‰Ã…ÀuÉ‰ò1ÀÇô   èüÿÿÿ‰Ã…ÀuëVf‹ƒP  …Àu,‰Ø‰òèüÿÿÿ‰Ã…Àt;ƒT  èüÿÿÿ„ÀuØ‹“P  ‹D»‰T»èüÿÿÿ‰Ø‰òÇƒP      èüÿÿÿ‰Ã…ÀuÅ1í[‰è^_]Ãv ‰ò1Àèüÿÿÿ‰Ã…Àtç‹ƒP  èüÿÿÿ‰Ø‰òÇƒP      èüÿÿÿ‰Ã…ÀuÜ[‰è^_]Ãt& èüÿÿÿUWV‰Æ‰ĞSƒì‹T$(‰$ƒú"‡    ‰Ë¾Š    …Éˆ   ¹„   ‘8  ‰|$Áç‰|$|>‰T$‹T–…Àt…Ûudƒâuo‹T$½şÿÿÿ‹DÖ9ÇtS‹\Öû ğÿÿ‡ü   ‹CÇC    ‰ÊÇC    ‰D$‰ğèPşÿÿ‰Å…Àtz‹D$‰C‹$‰CƒÄ‰è[^_]Ãv ½êÿÿÿƒÄ‰è[^_]Ã‹$½êÿÿÿ‰Ó	Ãtç‹\$‹\Ş9ßuët& ‹9ßt;Cuõ;S„rÿÿÿ‹9ßuìƒÄ½şÿÿÿ[‰è^_]Ã´&    ‹C‹‰A‰‰ØÇ   ÇC"  èüÿÿÿ‹D$‹DÆ9Çt&‹D$…Àtèüÿÿÿ‹D$àûÿÿèüÿÿÿ‰èƒÄ[^_]Ã‹D$ÇD†    ëÌ‰İé@ÿÿÿ´&    t& èüÿÿÿVS‰Ã¸    èüÿÿÿ‹C(…ÀtVÿs,1Ò‰ÙèJşÿÿZ…ÀuU‹s(ÇC(    ¸    èüÿÿÿöF,t[^Ã´&    èüÿÿÿ‹F¨u,dÿ[^éüÿÿÿ´&    v [¸    ^éüÿÿÿt& ë§t& ‹Fğƒ(uÎ‹VF‹Rèüÿÿÿë¾´&    ´&    èüÿÿÿ‹P(…Òté?ÿÿÿ´&    Ã´&    èüÿÿÿ‹P(…Òtèÿÿÿ1ÀÃ´&    t& èüÿÿÿƒøk„Â   wƒø9t[¹    ƒø`u!‰ÈÃ¶    ƒølt3ƒøzuV¹`   ‰ÈÃ¶    ƒø1u[ƒz¹    ¸    EÈëÇ´&    ¹    ë¸´&    fƒz¹    ¸    EÈëœt& =º   tQ=»   u2¹à  ë‚v ƒøQt[wáƒøtD¹    ƒøP„eÿÿÿ¹    ƒø„Wÿÿÿéüÿÿÿv ¹    éEÿÿÿ¶    ¹   é5ÿÿÿ¶    ¹    é%ÿÿÿ¶    ¹    éÿÿÿ¶    èüÿÿÿU½    W‰ÇVSƒì‰D$@ì‰D$¸    èüÿÿÿ‰ø¿Øşÿÿ-„  ‰D$´&    ‹‹39ûu'éƒ   v ÇB(    ‰Ø‰óèüÿÿÿ‰èèüÿÿÿ‹9ştc‰Æ‹C‰F‰0‹CÇ   ÇC"  …Àtèüÿÿÿ‹S…ÒtÀ‹J(öA,u°‰L$‰$èüÿÿÿ‹L$‹$‹A¨…Â   dÿ‰$èüÿÿÿ‹$ë‚f‹t$ƒÅƒÇ‹èüÿÿÿ‰ğƒÀ‰D$ı¸   …Jÿÿÿ‹D$‹|$‹@ì‹Xäqä9D$t‰Øèüÿÿÿ‰Ø‰óèüÿÿÿ‹FpäC9øuã¸    èüÿÿÿ‹D$‹˜ûÿÿ…Ûtèüÿÿÿ‹ƒT  ¨utdÿèüÿÿÿ‹[h…Ûuâ‹|$Gøèüÿÿÿö‡Ğúÿÿt.ƒÄ[^_]Ã‹Ağƒ(…4ÿÿÿA‹I‰$‹Ièüÿÿÿ‹$éÿÿÿèüÿÿÿ‹D$‹€¬úÿÿ¨u4dÿƒÄ[^_]éüÿÿÿ‹ƒX  ğƒ(uƒ‹“X  ƒT  ‹Rèüÿÿÿéjÿÿÿ‹D$‹€°úÿÿğƒ(u¿‹L$‹‘°úÿÿ¬úÿÿ‹Rèüÿÿÿë¥´&    fèüÿÿÿUWVSƒìH‹t$\‰$‰L$‰t$d¡    ‰D$D‹D$dÇD$4    ÇD$8    ÇD$<    ÇD$@    ƒà‰D$ƒø„_  ‹D$dƒàƒø„O  ‰×…öt	Ñ…A  ‹D$d‹t$Áèƒà…ö•Â8Ğ…&cmd_drivers/media/i2c/wm8739.o := gcc -Wp,-MMD,drivers/media/i2c/.wm8739.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"wm8739"' -DKBUILD_MODNAME='"wm8739"' -D__KBUILD_MODNAME=kmod_wm8739 -c -o drivers/media/i2c/wm8739.o drivers/media/i2c/wm8739.c 

source_drivers/media/i2c/wm8739.o := drivers/media/i2c/wm8739.c

deps_drivers/media/i2c/wm8739.o := \
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
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
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
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/wm8739.o: $(deps_drivers/media/i2c/wm8739.o)

$(deps_drivers/media/i2c/wm8739.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/refcount.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/generated/bounds.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
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
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.ho build the kernel author=Joel Fernandes license=GPL v2 depends= retpoline=Y intree=Y name=kheaders vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII  kheaders.tar.xz           GNU  À       À                GNU ])u)UŞoÇŸÚC	a·¢¼¥        Linux                Linux                                   $                                                                      kheaders                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ñÿ       #            #     *             8            J       F    
 d   F       
 y   \       
             ñÿ   k   	    
 ´   t       
 Ì   €   	    
 á   ‰       
 ô   —   7    
                                 )             ?           N             U             a             l      #     x            Œ  0?;       ¤              kheaders.c ikheaders_read ikheaders_init kheaders_attr ikheaders_cleanup __UNIQUE_ID_description96 __UNIQUE_ID_author95 __UNIQUE_ID_license94 kheaders.mod.c __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module sysfs_create_bin_file cleanup_module memcpy kernel_kobj __fentry__ init_module kernel_headers_data kernel_headers_data_end sysfs_remove_bin_file      &     (     $     &     )          (          %     "     %          *                        Ì   '  ô  #   .symtab .strtab .shstrtab .rel.text .rel.init.text .rel.exit.text .rodata .rel__mcount_loc .modinfo .rodata.str1.1 .note.gnu.property .note.gnu.build-id .note.Linux .data .rel.data..ro_after_init .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   #                     	   @       H;                )             c   #                  %   	   @       ,H; 8               8             †                     4   	   @       dH;                C             •   0?;                O             Å?;                   K   	   @       |H;                \             Í?; Î                  e      2       ›@;                  t             ¬@; (                  ‡             Ô@; $                  š             ø@; 0                  ¦             (A;                    °             @A;                     ¬   	   @       ŒH;                É             €A;            @       Å   	   @       œH;                ã             €C;                    è      0       €C; &                 ñ              ¦C;                                  ¨C; °     !         	              XF; º                               ¬H;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  cmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        cmd_kernel/kheaders.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o kernel/kheaders.ko kernel/kheaders.o kernel/kheaders.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        # SPDX-License-Identifier: GPL-2.0
# Any varying coverage in these files is non-deterministic
# and is generally not a function of system call inputs.
KCOV_INSTRUMENT		:= n

obj-y += mutex.o semaphore.o rwsem.o percpu-rwsem.o

# Avoid recursion lockdep -> KCSAN -> ... -> lockdep.
KCSAN_SANITIZE_lockdep.o := n

ifdef CONFIG_FUNCTION_TRACER
CFLAGS_REMOVE_lockdep.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_lockdep_proc.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_mutex-debug.o = $(CC_FLAGS_FTRACE)
endif

obj-$(CONFIG_DEBUG_IRQFLAGS) += irqflag-debug.o
obj-$(CONFIG_DEBUG_MUTEXES) += mutex-debug.o
obj-$(CONFIG_LOCKDEP) += lockdep.o
ifeq ($(CONFIG_PROC_FS),y)
obj-$(CONFIG_LOCKDEP) += lockdep_proc.o
endif
obj-$(CONFIG_SMP) += spinlock.o
obj-$(CONFIG_LOCK_SPIN_ON_OWNER) += osq_lock.o
obj-$(CONFIG_PROVE_LOCKING) += spinlock.o
obj-$(CONFIG_QUEUED_SPINLOCKS) += qspinlock.o
obj-$(CONFIG_RT_MUTEXES) += rtmutex_api.o
obj-$(CONFIG_PREEMPT_RT) += spinlock_rt.o ww_rt_mutex.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock.o
obj-$(CONFIG_DEBUG_SPINLOCK) += spinlock_debug.o
obj-$(CONFIG_QUEUED_RWLOCKS) += qrwlock.o
obj-$(CONFIG_LOCK_TORTURE_TEST) += locktorture.o
obj-$(CONFIG_WW_MUTEX_SELFTEST) += test-ww_mutex.o
obj-$(CONFIG_LOCK_EVENT_COUNTS) += lock_events.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       // SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/export.h>
#include <linux/irqflags.h>

noinstr void warn_bogus_irq_restore(void)
{
	instrumentation_begin();
	WARN_ONCE(1, "raw_local_irq_restore() called with IRQs enabled\n");
	instrumentation_end();
}
EXPORT_SYMBOL(warn_bogus_irq_restore);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            // SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/export.h>
#include <linux/irqflags.h>

noinstr void warn_bogus_irq_restore(void)
{
	instrumentation_begin();
	WARN_ONCE(1, "raw_local_irq_restore() called with IRQs enabled\n");
	instrumentation_end();
}
EXPORT_SYMBOL(warn_bogus_irq_restore);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            /* SPDX-License-Identifier: GPL-2.0 */
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     Ù~ğ$!H§qšÚH¼µ’{rèMPĞ"Üì4EjáÊØĞT˜†r*6ô3fVT»#àÌë¶¿³#y¡«Äé¿\×ÿÆ‹dÄV#L•¸øÂ	gîAç90-;êâa¶§ÒZñÒV‡&EbÔFÙ ã†>F°<•¡‹’¢ˆßÌIT°nUéĞ+Ë˜Ñ)®%?ºZ¶—½%%ÓD”¦d@öÿ’-Øl‡ˆüqœõM¶%pcÈá‚ ™8w"óšöó÷S²²ecxåêbÔ-OR0\<mÅ‰/×Fâêì&İ0]ÌØw…Ûì'‹:» M“T;2İİ„r¬ ¿,ûlÚÎT0cBÉÛfS¤¤ôtLCF8ê<|õh½„ĞyŸ¦ã_=*¦OQ©¡¦Íwq0Î…é\¨‘êçÖ/ı†–İ‘o?«Û,ƒİ'v€è¡7	¼.5…Z<‘fwô2T™KŞZ~E0QÛ9#†‘DËÒ1fS$FÑU5H„ZNÎ‹W«İÌ¾ ·¬¥•:z9…ïCyœZ=âÿŞ\èÆÜ^ôğı>|Óq5Møó®D¿XûœïS±@T2»4PÂ ¬8 ^/Z];N·²¸†»Ò¥êHb‚ƒ·!°ÑÓdj HÕeãÎËù	Vë”±¥CİwÚÒ¸åm€¨'ZÔiŞ3 íì÷iĞ^‘{èÆ?sÉtâíQCÁe`i`x¸)©\ô»Ón§cHš	ESE¯F±7 –Ô×”¿ÊúMÑûA«–ïŸÆb$	 y·×‚W§4àèÒL ;™*ép@Ûšãş…¸÷…Š£¬F°HÙeØøu6ÙVš-xxL=}†lúSå@µxƒj?Æø`‘¹ú¿
'àñÅ
&lÏN|q™¶fçæy½à€VÂÒSò­µKÿïÓ–RadB/O¿`&.|ßtÆ0Rƒ nxÇ"ÜA#~ŸÕy9¹}³ŞĞJÈ„õt\U .Sño¨[:J€1ô­CÍÉj|x©RqÔ†tíZ0(cÌõ¿p<Rª­é-HéÉ.ëÅ–ös>Å?’Øbd$ÀD^EÓ,â”@6Ëjµp¯—hLº©ÖúRÌ«<Kˆ+O%İäŞ­ç Â ş-ö,7l#íPåÕĞ¹ŸºıÍxbapğ¾)–Ü°µ©Cç.zmv$däBÁ‘çjšVİrËËµ Šì¹}²:]R­-Ö$¹œßaŞ9ïÓ»#ü”¯V;Pl²*Ø€[Ü\+w;QÛòçÓîê 4íœÖ3Çã[ÏGúkèm¢%øäI%Ë5µñ_YJiËXè…À©f7mâ¥B‰l¸;õ¯c ÏĞ¿™32Xî™]öšm‡Í¹dØ¿è]êAÕ.k‡TvHÆ?^*;ùÉ»U#¿zÚ·<×AÉq2/„y¥+¾[e©-‹òºµä6â§§%âÙJ2KĞiÜpŞ0Çf@+0~ßĞêë	LwÙ.y™+gí§=…“ßÁôKQbıĞÇñ_Hk¤GD‰ÌæUÂÑ#âòj;aß\Ájáw\€Ø8®#ªD¼ÑÿóPAÇH¿İ],¯3lù,FÓÅõ{XöÀË™ø«gÍĞM‡â®ßÄéAÔp*A¦û,GêÊAî–M-·r	áÆÍã[÷×[Ó%†â¯ïXÌ1
u8´/™ÌIf‡Û¦Ëœ‹à³LhûOw%²(Ê*–ä	±>VıRF“6J~JküFÎËî»éJÅJÍ>¹òR	™cìJ-©1şY»•-ûc¥9ç³O‹¸x9RCôû°Ë·»¶ë1†¹)q=wô@x§	'yÓÛ;köA0@şñ÷ù'Ù_İ|~NJ‚Ï”Áî¦L³ŸÊ‰)—‚ô…gş$‘s·s'Iğ·xC0ÙPYğŞşŸØcÄ<â:*ÿÃ Ìô5Ç¨,f†A6À<MXLÊ›ÏîxĞëiîŒ%ñİ|G»°ò:¶»
d|5×J—ÿÍyvçJ0Íï9Up¬o”á+Š-:²­Rã­*x3
±^–›‘òÂ±g¹Gx(¡ÏXÛ2Ÿ”ëß¡«ßÉŸ­÷z”U—ø¡G0pV¬ysDí–:Ûh¡ÕK B)É¾ŒÌoWÍwÓdñ #p‡L^™í¼m¿QöqÄ!­œ¥rÆ?ˆS½áõ`ï;Yå–
¼ÂÑÒçòé(`S #5¦ ]+¨3ªyGcëÍK€äé	§ÀXú…Ä–h÷”g+–g1°‡¿yÁIylid©XVèÜ“<óSùg±=lõ÷Ç®´¤{ñüO+¸V>i

õÀWJßı&z‘øğÚc«óÁa=>¥9¿š<0–^JM(*Bo	@)TÎ-+?J4Ràº¡, 1$xE;r#å!/®¢x„ÑÙğä¨<½÷ÿ°Ÿ®,téûõ¢Èt¼‘–×m†«)P€ÏÒâ£µù";9«ÂøµÁ_q `-‹üş›ï72xè±½ŠP¼ÉÉa—£“lyâŸ`‡ùg*Å‰±a¬›„ä
E»øÇ/pı	ÃR–€é©N­Í’5j•İõÔàHäš‰îë^»[1[…âlä¶é¬1…ïBNO‰ûS„=L%ù¹Ö2*ä‰c4lêşÇ˜JÚí>Òœğ•‰6u­sìkÖ`1ù¬ëEè|‡–‘mŒ¦[cÅı ä‰'e`ù°EajDeAÌæ
ÂDlîŒ>éµMá û¸NˆÊ'Ó¾Ô	sßr/¹§A¬*èš§yD>ÊbC'ü	æ(&æ&^ Ù
‚æbéÒüv†'@)ÄßQ±‘Îç Â¹±E·»¦‹û™áÌ<ş®~TK¶ÑõfƒU#ñ¿'7¾•1fŒ±„ÎÏ.LL>.sîAÒ¶‡	e×){ƒ×æ]€´´>~xSq‡Êtº²Zb	q$Ú^Z¾ıW‘á|-1>Ü'FÒ/[’“$ M¡i¹«4*‚lú“ûr¤l.nú†1ìãpbêq,õkzÙ²ûA:¡0P‘ãoºI»3RÙ¤U6Ó0v~â`ª[Ş=}©kIz>Ûu7ÿ­P9_Y¸â=ÛnÂeº¡ró%„â³æË´t}‚¾­Ãsç%:¡(òo¡~w\Ó«çNŞ¥X»›#±ô‹ºÕåªW¿ Šq×F,Ô9¸½º›GvÒ¼êbÒ	ˆz¹±üı0–*ÌGŸØ4`£m½çOºÕ›â(N hğ ÕŠãú =Ï÷dşbá|‰¢ˆkáIãÖ<ŞŒb6o¼nÌ¦Ü³êwG‚%JÒ»xWG²Fƒo’â»´*>) Äù—Š:œ¤ïï­g¨'3ç‘†à%”)õ42	±#&wLêf³sÁÈ£[‘3™˜îÁ¥÷ñ÷7ì8®â(Ìm$Lå{j#OÍV!m #-a‘nvoE‰D‰ôy‚Hz¿¼7úÇh†µ9¨{?¼@ÎôQûáTÅ£%*şHÿ0áÛnÕßYaæòüŸIY™Fwÿ«o0ƒü=¥mU
ê@„O‰?ˆ,ìiØ*<6ÍñPtô—?›4éÿL™›ì¿áöNyÿ
çcòªí[_¦ÇáÀyÆ‚Şn
VûGGÑd˜ˆû†3]%Ø‡€CŠÄmØPh7§Êy$*A-¦#áp‡­´WÅ^L¢éW„2 çél«<î@ƒEüb]ÏÇY|!ƒ¬6P®‰Ri÷Ğ»r“Æ¿t°%‡&W&L£í…jY b“CB/£Ñ0@‚’©¤®À¤KGnŸûcî˜{ZñC€v,¾,L
‚²¦õÇÒ¥«y²š¤6‰‚HçB³øÔÂ+GÅqyæÀæìVaC”—1šHvçòø¯;\¹ıÔfZV&©êïw½‰¾şá$ú#½xÿFeAŒÕ4Ú(	âf¹È€2±¾Œƒ%âÍƒÃ*´¤~Õø½€üXÁk£¼7Ø]ƒF)K§±=Ãû.Î±‰½>“ã’\ÓçíXå"¸IkÑƒ§É°UFÊ”[Ã`Ò7¹;ğKq›qaäèj½™¸[²…ÕÓÂMŞÈ×3=×‚Œ–mETä¦#Œ÷·´ªÆ›VnÍ.!š  F[§Ğ(î{Úp°`óæö:»54r1L]£ŸáBı9E{Ä 'IÇöO}Tªy9Ä+[G´ŸŠÏ@´/ÿë‰ cİüoÊ3*Ñ	\ê•2×4Î–Ê*®ËÒÜ'­åÅSªğ3édeY¥©éæ@ÔÙû$Ç Xxmëa?•ş¼™O\OşTãİVœLE»P«Å÷ö‚WÊi‡Şñ¹*4'Æ~Öîg^•ñë6Zº+ìfñ[«B^Ÿyxiã·‡tŒŸßÑß Ğ,éq b±B%ØîŠ¤¡‘]d°ŠÙCËS
1¥û•ôséƒi`5hµÊ‰.=*×f«í×e•¤ª·|?tQÚÄ7?ÂE±Âø(H¤ãÀ—ÊØÎªfT«2è7Dõw£şæ£„ş»ã§w‹Ş5<…Mí—Ğ†\@ÿ°e•bõ25Ës—‘8AÚñdWÑ6ñ‡€"…Ê&»L†.Æb€º¯§) sk}À…°ö’zòº¬y
ïd‚óu×ôœoí“…qdpp061·\ñj§iWç®«XÚû0v×ß—ÆÕ6ˆæî”ÄÓ:w)\µÏd‚?„Š:ıÎk<Ñ™$@Ó3ƒ¿7@L#ãÿ¥Ç6)ÀŠòmJÖ(G‘˜Í(^QŒ]±—; †R÷è5‡vëIİ˜x|Úİƒ†Ê6äÊ#8×„f57èôMÒ=“¹ÈVRìzrwfÖR Xv9ãvƒÕêè‡æŒw<¾%Şõh@ó=0sL¸«,h>8;H/¦6ñçù`%qW¨ÅÀ£¤hÁOä-×ÇÒÃ;Êcú0tm0î¸ 2!>ô˜o‹µ%n@ä Öoqyrê¡N°õ0}TŞ­0Ü$‰:ñaûqÊ$:$hÍöoRğ×Èqy) >õª¶€W‹5mAN‰év¨F>ß§ÄÖEzş–w­Bviµå©ÙJ5‘[üvÿPzfZï–”!`ã2… ƒh³((Ğ‰O³`£,¦”æq£ûá.Ë%}Hdl*şH >ê½åíéåøXfÕ» 6|hecÄ–†!¾ÖwÅşû¢oú°Ù}wQQ€°µU»Ø÷ø3'ƒ€Ú”Á®ŸäPÒ–”Û½aGƒ¤!©‡´¸†ÖÊ\M(6$®FÜs3¸†.&z¾„ÏÖ,ˆíQ; ìrª)ãBB)Ç#ÌæúPfÁàW>Ã;m:dnéŠ%#,+[„B‚¼5?ddĞíí Ÿ%>Pı1R5¾ˆÅI#‰Ñ3+>xE«(ö ;{ä.Å®"Q³Àœæ»¯¬Ø@}é ĞfQ´nFåŞcS·Ì^¶Ç­õ#Uh*Eş©{õ¤²IıKÕËôO¡.]E•ªÑ…xÿ²şÒØµl—!0G†Ç:R)³×JÕ|º_ÅµÜër?ÏøNl¡*ÏïËø¹GõgÈ91^~qÏ'Ô¸’ú#ÌoyÂ/ôtÑºjÊ‡¨Šù“É>Õ¸ãH·‹ÏNacO7¡”^-£­‘s©¡M/s0’=­n4[Àè0Ù_!‰G˜¥öŞ2t|¸qçKZ­×ÊÃùìPeô®_ö @÷S2èS¿µğÜÊÿ£õ{Ù©»nŠµá£:º—b,	CqTGç‹æú;f]‘à©ÑJsGæ%+y¾óFÔDXèlÓæ-;<ñg2×…ÈE)v„ô‰‡ç”®oß'øò‰…êdò–åÃ§ÛD@ƒg…S·?Bı<×vU¼ÕOßÿ÷z®áÏv¿yØú¬’ZÂë"Y¨2"Ö;ëç]Xã@œöÒ
†U°,[/pIÌ5s‘g	ö>ûZV`ø ¹®Ä)4Ÿş€X#Ù¨ §ÜMáòD¶ÜÃØBv¦8.Ü®)’8ñ(€q?³›ÃHm;Ä ®“á	Şâôx¦ÑG¸şé¾%}ÙşèÕˆ¿iÎ$Ë–•G¢g»yJkV’í6o	ˆá•êüLßÕÙÜaàsÒ?#"ÂCë$;æG™i˜¡tAš.F@½ÄÙ¬ñHtÃÍ½Åkˆ0Q{9•¿«õ{µiï·ıÍ2(ÒPOôp"ä/Òµäu%\==]A¶°¡Mxü.®÷ñ¿”bÜHËÖj9K„şà7ñP [òÛ«ŞGéá#j§Ñ5;$| ®AAº·¢›c^78]ÉrÇë¹¯fu¦éqSÕRyÂPfUÀàÈ Ñ-ÌqæÊ¹DjúÿI\9W	…
ø†P¢î…È;'’Ï·ÔˆÇ‡.e¥üEø‹cÖ¤¡ªY2ÁvìéHSM¡xÍııN¶¢–bUz»¦iaŠ´›Èq¸õd½Ò¦øz$}Ğ†+JdLalŠ& C ±{~TÇ{‡ôQp¾GOáéX‚;²Ğßšóò`²2i£'¢Ş|3Œ_Åªñ¿BtÈ÷îÜëè7Oıáü¼`S'V_¸7ÃÍú;¨QÜ1ÏÏŠæt™Q	lş“}¹çh§^,öÕ½•<Ë5Cx:”`E¼Vedé¼ğ ½ó¿.úds¹îÂàïå'àÏ&UM‘Sõ†És®¶‚.-†[îmì·šxÇv,¬VéA*¢Gz£O¢#^¿b¶6\È>lõ³Üx0'ëNğÛè3‚&Ï},“/g5Ñf$·>|ù„%›ÙgİiSš“ÑD½_mÛ=tDqj	Ø:^*¸ …¾ùLlÔTWİ³çy ¹«Æıâ¥è0j£±ÍÔ