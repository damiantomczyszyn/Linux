s);

/**
 * struct pending_free - information about data structures about to be freed
 * @zapped: Head of a list with struct lock_class elements.
 * @lock_chains_being_freed: Bitmap that indicates which lock_chains[] elements
 *	are about to be freed.
 */
struct pending_free {
	struct list_head zapped;
	DECLARE_BITMAP(lock_chains_being_freed, MAX_LOCKDEP_CHAINS);
};

/**
 * struct delayed_free - data structures used for delayed freeing
 *
 * A data structure for delayed freeing of data structures that may be
 * accessed by RCU readers at the time these were freed.
 *
 * @rcu_head:  Used to schedule an RCU callback for freeing data structures.
 * @index:     Index of @pf to which freed data structures are added.
 * @scheduled: Whether or not an RCU callback has been scheduled.
 * @pf:        Array with information about data structures about to be freed.
 */
static struct delayed_free {
	struct rcu_head		rcu_head;
	int			index;
	int			scheduled;
	struct pending_free	pf[2];
} delayed_free;

/*
 * The lockdep classes are in a hash-table as well, for fast lookup:
 */
#define CLASSHASH_BITS		(MAX_LOCKDEP_KEYS_BITS - 1)
#define CLASSHASH_SIZE		(1UL << CLASSHASH_BITS)
#define __classhashfn(key)	hash_long((unsigned long)key, CLASSHASH_BITS)
#define classhashentry(key)	(classhash_table + __classhashfn((key)))

static struct hlist_head classhash_table[CLASSHASH_SIZE];

/*
 * We put the lock dependency chains into a hash-table as well, to cache
 * their existence:
 */
#define CHAINHASH_BITS		(MAX_LOCKDEP_CHAINS_BITS-1)
#define CHAINHASH_SIZE		(1UL << CHAINHASH_BITS)
#define __chainhashfn(chain)	hash_long(chain, CHAINHASH_BITS)
#define chainhashentry(chain)	(chainhash_table + __chainhashfn((chain)))

static struct hlist_head chainhash_table[CHAINHASH_SIZE];

/*
 * the id of held_lock
 */
static inline u16 hlock_id(struct held_lock *hlock)
{
	BUILD_BUG_ON(MAX_LOCKDEP_KEYS_BITS + 2 > 16);

	return (hlock->class_idx | (hlock->read << MAX_LOCKDEP_KEYS_BITS));
}

static inline unsigned int chain_hlock_class_idx(u16 hlock_id)
{
	return hlock_id & (MAX_LOCKDEP_KEYS - 1);
}

/*
 * The hash key of the lock dependency chains is a hash itself too:
 * it's a hash of all locks taken up to that lock, including that lock.
 * It's a 64-bit hash, because it's important for the keys to be
 * unique.
 */
static inline u64 iterate_chain_key(u64 key, u32 idx)
{
	u32 k0 = key, k1 = key >> 32;

	__jhash_mix(idx, k0, k1); /* Macro that modifies arguments! */

	return k0 | (u64)k1 << 32;
}

void lockdep_init_task(struct task_struct *task)
{
	task->lockdep_depth = 0; /* no locks held yet */
	task->curr_chain_key = INITIAL_CHAIN_KEY;
	task->lockdep_recursion = 0;
}

static __always_inline void lockdep_recursion_inc(void)
{
	__this_cpu_inc(lockdep_recursion);
}

static __always_inline void lockdep_recursion_finish(void)
{
	if (WARN_ON_ONCE(__this_cpu_dec_return(lockdep_recursion)))
		__this_cpu_write(lockdep_recursion, 0);
}

void lockdep_set_selftest_task(struct task_struct *task)
{
	lockdep_selftest_task_struct = task;
}

/*
 * Debugging switches:
 */

#define VERBOSE			0
#define VERY_VERBOSE		0

#if VERBOSE
# define HARDIRQ_VERBOSE	1
# define SOFTIRQ_VERBOSE	1
#else
# define HARDIRQ_VERBOSE	0
# define SOFTIRQ_VERBOSE	0
#endif

#if VERBOSE || HARDIRQ_VERBOSE || SOFTIRQ_VERBOSE
/*
 * Quick filtering for interesting events:
 */
static int class_filter(struct lock_class *class)
{
#if 0
	/* Example */
	if (class->name_version == 1 &&
			!strcmp(class->name, "lockname"))
		return 1;
	if (class->name_version == 1 &&
			!strcmp(class->name, "&struct->lockfield"))
		return 1;
#endif
	/* Filter everything else. 1 would be to allow everything else */
	return 0;
}
#endif

static int verbose(struct lock_class *class)
{
#if VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static void print_lockdep_off(const char *bug_msg)
{
	printk(KERN_DEBUG "%s\n", bug_msg);
	printk(KERN_DEBUG "turning off the locking correctness validator.\n");
#ifdef CONFIG_LOCK_STAT
	printk(KERN_DEBUG "Please attach the output of /proc/lock_stat to the bug report\n");
#endif
}

unsigned long nr_stack_trace_entries;

#ifdef CONFIG_PROVE_LOCKING
/**
 * struct lock_trace - single stack backtrace
 * @hash_entry:	Entry in a stack_trace_hash[] list.
 * @hash:	jhash() of @entries.
 * @nr_entries:	Number of entries in @entries.
 * @entries:	Actual stack backtrace.
 */
struct lock_trace {
	struct hlist_node	hash_entry;
	u32			hash;
	u32			nr_entries;
	unsigned long		entries[] __aligned(sizeof(unsigned long));
};
#define LOCK_TRACE_SIZE_IN_LONGS				\
	(sizeof(struct lock_trace) / sizeof(unsigned long))
/*
 * Stack-trace: sequence of lock_trace structures. Protected by the graph_lock.
 */
static unsigned long stack_trace[MAX_STACK_TRACE_ENTRIES];
static struct hlist_head stack_trace_hash[STACK_TRACE_HASH_SIZE];

static bool traces_identical(struct lock_trace *t1, struct lock_trace *t2)
{
	return t1->hash == t2->hash && t1->nr_entries == t2->nr_entries &&
		memcmp(t1->entries, t2->entries,
		       t1->nr_entries * sizeof(t1->entries[0])) == 0;
}

static struct lock_trace *save_trace(void)
{
	struct lock_trace *trace, *t2;
	struct hlist_head *hash_head;
	u32 hash;
	int max_entries;

	BUILD_BUG_ON_NOT_POWER_OF_2(STACK_TRACE_HASH_SIZE);
	BUILD_BUG_ON(LOCK_TRACE_SIZE_IN_LONGS >= MAX_STACK_TRACE_ENTRIES);

	trace = (struct lock_trace *)(stack_trace + nr_stack_trace_entries);
	max_entries = MAX_STACK_TRACE_ENTRIES - nr_stack_trace_entries -
		LOCK_TRACE_SIZE_IN_LONGS;

	if (max_entries <= 0) {
		if (!debug_locks_off_graph_unlock())
			return NULL;

		print_lockdep_off("BUG: MAX_STACK_TRACE_ENTRIES too low!");
		dump_stack();

		return NULL;
	}
	trace->nr_entries = stack_trace_save(trace->entries, max_entries, 3);

	hash = jhash(trace->entries, trace->nr_entries *
		     sizeof(trace->entries[0]), 0);
	trace->hash = hash;
	hash_head = stack_trace_hash + (hash & (STACK_TRACE_HASH_SIZE - 1));
	hlist_for_each_entry(t2, hash_head, hash_entry) {
		if (traces_identical(trace, t2))
			return t2;
	}
	nr_stack_trace_entries += LOCK_TRACE_SIZE_IN_LONGS + trace->nr_entries;
	hlist_add_head(&trace->hash_entry, hash_head);

	return trace;
}

/* Return the number of stack traces in the stack_trace[] array. */
u64 lockdep_stack_trace_count(void)
{
	struct lock_trace *trace;
	u64 c = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(stack_trace_hash); i++) {
		hlist_for_each_entry(trace, &stack_trace_hash[i], hash_entry) {
			c++;
		}
	}

	return c;
}

/* Return the number of stack hash chains that have at least one stack trace. */
u64 lockdep_stack_hash_count(void)
{
	u64 c = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(stack_trace_hash); i++)
		if (!hlist_empty(&stack_trace_hash[i]))
			c++;

	return c;
}
#endif

unsigned int nr_hardirq_chains;
unsigned int nr_softirq_chains;
unsigned int nr_process_chains;
unsigned int max_lockdep_depth;

#ifdef CONFIG_DEBUG_LOCKDEP
/*
 * Various lockdep statistics:
 */
DEFINE_PER_CPU(struct lockdep_stats, lockdep_stats);
#endif

#ifdef CONFIG_PROVE_LOCKING
/*
 * Locking printouts:
 */

#define __USAGE(__STATE)						\
	[LOCK_USED_IN_##__STATE] = "IN-"__stringify(__STATE)"-W",	\
	[LOCK_ENABLED_##__STATE] = __stringify(__STATE)"-ON-W",		\
	[LOCK_USED_IN_##__STATE##_READ] = "IN-"__stringify(__STATE)"-R",\
	[LOCK_ENABLED_##__STATE##_READ] = __stringify(__STATE)"-ON-R",

static const char *usage_str[] =
{
#define LOCKDEP_STATE(__STATE) __USAGE(__STATE)
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	[LOCK_USED] = "INITIAL USE",
	[LOCK_USED_READ] = "INITIAL READ USE",
	/* abused as string storage for verify_lock_unused() */
	[LOCK_USAGE_STATES] = "IN-NMI",
};
#endif

const char *__get_key_name(const struct lockdep_subclass_key *key, char *str)
{
	return kallsyms_lookup((unsigned long)key, NULL, NULL, NULL, str);
}

static inline unsigned long lock_flag(enum lock_usage_bit bit)
{
	return 1UL << bit;
}

static char get_usage_char(struct lock_class *class, enum lock_usage_bit bit)
{
	/*
	 * The usage character defaults to '.' (i.e., irqs disabled and not in
	 * irq context), which is the safest usage category.
	 */
	char c = '.';

	/*
	 * The order of the following usage checks matters, which will
	 * result in the outcome character as follows:
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
�����<�T��H�J��r(�σ�z�v'��e�YȔp���@Q�3�BN��)-mw�=Zgr��Z�d��
�ߙ�Z�F��*���kD�=3�%C���`���R9���'3�p�u��ހz�Ng]��Q%��\��9���G
(}��ר�p��5mZ�9h�,���u"Y�k�S|{����E"^T/�\�7☘��������
S�,Ӳ
7\���q[j~N�[��_��Y��9��%D��J�1Q�����yVIS�T$8+�c�L\K-�B�t$�La��*.n30�F
�zxCY��Z
�
Fr|G��uM�Q�?	9]�у�/�;��zO�5�/3�0����M����nD�݋n4K�z��G�ulc�|��oI^���+��,�c �[�d������S����'�XL��}��ԋ���Zs|����T~�.4��x7D9kU��x����.��)����8�$�ۦ�*��һ�0=�X��h��q�i�W^~��́7JO�Q!�8��5�� �-�%4R����"y�_��(3p��V���"�Z����/2c���re�Y�`�����
�˕A��HOX��p\}�/�G��"m=�����6Q5����|G��.�p�`�V���"6msB�c�av�!CWt�Cq��A�?E�j1�d�� ���E��]���Z�h�@��V���}+Ɛ ��N��aĎ)�j�ޣC�N�]0T�8Z�����m��rl��}����;3��8m�EF:-���sp@:]Im�6۷��Ě�`2���g�̍� �+��%6[y�����r��|�3��32�,���Gt�vu�I��R���&�a���g���x|��'RؼY}=include/linux/memcontrol.h \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      ELF                      T      4     (               �����S�X\����������   �����1�[Ð�����UWV��S�����   �D$�    �$���.   �$��   �< ����������t���M   ������������u��[^_]Í�&    ��    �����V1ɉ�S�Ӊ�<  �	   �s�����D�  t+����  tS�� }  t3��   �	   �J���1�[^Ít& ��    �   ���/����ҍt& ��   �   ������뺍t& �1ɺ   �������������x(	� �P��   UW��@���VS�� ����Bx�p|�Bp�h|�Bt� �  �@|��������f�D$�   )�9�O��Ņ����  H�������)�����������)ʉ�1҃�fL$���j���� �  9�O��������  I�������)Ɖ�����2���   ��)��fL$���"���1���[^_]Í�&    ������Í�&    �v �����WVS�Ë@�P�R�����%   =   �k   �������[^_�                                        0                                                                                                               �             6%s: Frequency: %u Hz
 7%s: write: %02x %02x
 wm8739 �����V�ppS����<  Vh    ��������   �������1���[^�V�t$�D$��pPh   ��������E   �4$V�D$��pPh    ��������w   �S�C��  Q� QP��   �CT�0h0   ������C��
������I  �ڹ@   ���   �����1ɺ   ��j ������	� ��   ��j h0�  j h�  j h��  j j ������		� ��   ��$��0  ��j j j jj jj j ������	� ��   �� ��4  ��j h �  j h�  j h��  j j ��������   �~l��8  �� ��t��������I  ��0  �   �����1ɉ�   ǆ<  ��  �   1ɉ�   �   1ɉ�   �   ��I   �   �   1ɉ�   �   ��   �	   �   ��������I    3%s: I2C: cannot write %03x to register R%d
   6%s %d-%04x: chip found @ 0x%x (%s)
 ������    �    ������    �����                wm8739                                                          �       `                       �                                                                                          debug parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=T. Adachi, Hans Verkuil description=wm8739 driver  �           ��      GCC: (GNU) 11.2.0           GNU  �       �                                  ��                                          
                                       	        2    	               *       c     H           >   2   =    	 W   �   �     k      �     y      3     �   o   �   	 �   @         �   �                      �            �       �     �       
     �       0                   �   �   0       `                   '          @           N  �        `  0        w  <        �  [        �             �             �             �             �             
           0     )              wm8739.c wm8739_remove wm8739_log_status wm8739_write.isra.0 wm8739_write.isra.0.cold wm8739_s_clock_freq wm8739_s_ctrl wm8739_probe wm8739_probe.cold wm8739_ops wm8739_ctrl_ops wm8739_driver_init wm8739_driver wm8739_driver_exit wm8739_id wm8739_core_ops wm8739_audio_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free _printk v4l2_ctrl_handler_log_status i2c_smbus_write_byte_data __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_cluster v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__wm8739_id_device_table param_ops_int     "     #     $  !   "  :     [   '  s   '  �   "  !  "  !  "  4  (  E     h     D                   h                                            "          %  &   &  @     E   %  ^   	  c   %  �   	  �   %  �   )  �     �   *  �   +  �       ,      .  ,  8    a  ,  ~  $  �  -  �    �    �    �    �    �    �  .  M     k     �     �           "          /     0          2  @     H     `     �     �              /     5        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   S                    	   @          p               )             �  �                   %   	   @       �                  /                                  8                                  4   	   @       �  8               E      2       <  7                 X             s                   T   	   @       �       	         g      2       |  V                 z             �                    v   	   @       �                  �             �  
                  �   	   @                        �                �                   �   	   @       (  (               �             �  u                  �             8                    �   	   @       P                  �      0       L                   �              _                     �             `  (                                �  `     "         	              �
  7                               p  �                                                                                    