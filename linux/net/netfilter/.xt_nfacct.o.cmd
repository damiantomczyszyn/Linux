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
	class->name_version = c/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  include/linux/compiler_types.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
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
  include/linux/kstrtox.h \
  include/ELF                       "      4     (               èüÿÿÿS‹€”   ‹@\‹˜Ø   ‹ˆÔ   1À‰Z[‰
Ã´&    t& èüÿÿÿ‹€”   ƒ9‹@\uMƒywGÇA    ÇA    ÷€Ô    °  tÇA€  1ÀÇAà  Ãt& ÇA   1ÀÇA@  Ã´&    ¸êÿÿÿÃ´&    v èüÿÿÿ‹…Òu‹A…ÀuÇA   1ÀÃt& ¸êÿÿÿÃ´&    v èüÿÿÿÇÿ°  1ÀÇB    Ã´&    t& èüÿÿÿS‹X\‹ƒÈ   …Àtèüÿÿÿ‹ƒÀ   èüÿÿÿ‰Øèüÿÿÿ1À[ÃfèüÿÿÿUWVS‹¨À   ‰Ã‹¸”   …ít&‰èèüÿÿÿ‰Æ…Àt[‰ğ^_]Ã‰èèüÿÿÿ‰Æ…À…¯   ‹ƒÈ   …Àt1Òèüÿÿÿ¹   ºè  ¸ô  èüÿÿÿƒÇ¹   º    ‰øèüÿÿÿ‰ƒÌ   = ğÿÿ‡üÿÿÿ1ö…Àt—º   èüÿÿÿ¸ô  ¹   ºè  èüÿÿÿ‹ƒÌ   …Àt!1Òèüÿÿÿ¸ô  ¹   ºè  èüÿÿÿ‹ƒÌ   èüÿÿÿ1ö[‰ğ^_]Ã´&    ‰èèüÿÿÿé,ÿÿÿt& èüÿÿÿVS‹€”   ‹X\…Òt‰Ø[^éãşÿÿv ‹³À   ‰ğèüÿÿÿ‰ğèüÿÿÿ‹ƒÈ   …Àtº   èüÿÿÿ¹   ºè  ¸ô  èüÿÿÿ1À[^Ã´&    ´&    èüÿÿÿUWV‰ÆS~ƒì‹nX‹@…í„g   ‹P‹Rèüÿÿÿ%   =   …P   ¹À  ºà   ‰øèüÿÿÿ‰Ã…À„û   ‰¨Ä   ¹@   ‰òèüÿÿÿº   ‰øèüÿÿÿ‰ƒÀ   ƒøş„†   = ğÿÿ‡å   ¹   º<   ‰øèüÿÿÿ‰ƒÈ   = ğÿÿ‡  ‹n\‹…Ä   ‹ ƒèƒà÷…ı   ‹…”   ‹@\è¼ıÿÿ‰Â…ÀxF1Ò‰ğèüÿÿÿ‰Â‰ÆÁúƒæƒâ‰µÜ   ƒú…†   é~   t& ÇƒÀ       évÿÿÿ´&    f‹ƒÈ   …Àt‰$èüÿÿÿ‹$‹ƒÀ   ‰$èüÿÿÿ‹$ƒÄ‰Ğ[^_]Ãºôÿÿÿëï´&    fèüÿÿÿUWVSƒì‹”   ‹i‹r\…í…À   ‹¾Ğ   ‰È…ÿt4·W‰P‹–Ğ   ·RÇ@   Ç@   ‰PÇ@	   1ÀƒÄ[^_]Ã‹–Ô   öÆ°…   „Òtm½à   ¿@  _H‰D$O‰4$‰Øë3f·Q»€  ¾à  )Ó‰Ú÷ÚHÓ·Y)Ş‰ó÷ÛHŞÚ9ês‰Õ‰ÏƒÁ9ÈuË‹4$‹D$‰¾Ğ   …ÿ…Rÿÿÿ¶    ¸êÿÿÿémÿÿÿ¶    ¿   ë†´&    fèüÿÿÿWVS‹˜”   ‹C\÷Âÿ°  „Ã   ‰Ô   €æùºU   ‰ˆØ   ‰Øusèüÿÿÿ…Àˆ“   %ï   ºU   ¿   ¾   ‰Á‰Øèüÿÿÿ…Àur‰ñº   ‰ØÁéƒáƒÉèüÿÿÿ…ÀuW‰ùº   ‰Øèüÿÿÿ…ÀuE‰ğº	   ¶È‰Ø[^_éüÿÿÿt& èüÿÿÿ…Àx$%ï   ºU   ¿   ¾ğ   ƒÈ‰Á‰Øèüÿÿÿ…Àt[^_Ã´&    ¸êÿÿÿëî´&    fèüÿÿÿWVS‹°”   ‰Ó‹F\…Ò…Å   ‹€Ü   ¿   …À…š   º   ‰ğèüÿÿÿ…Àˆ~   %ø   º   	ø‰Á‰ğèüÿÿÿ…Àxeƒûº   ‰ğÿƒçƒûÛƒã@èüÿÿÿ…ÀxE%ñ   º   	ø‰Á‰ğèüÿÿÿ…Àx,º   ‰ğèüÿÿÿ…Àx¶Û%¿   º   	Ø[‰Á‰ğ^_éüÿÿÿ[^_Ãt& ƒø…À  ¿   éSÿÿÿt& ‹ˆĞ   …É„Ú  1ÿé9ÿÿÿ´&    t& èüÿÿÿUWVS‰Ëƒì‹ˆ”   ‰D$‹C‰T$‹Q\…À…  ‹C…À„  ƒø	…ò  ‹C‹KÇC   ÇC   ‰$‹‚Ô   öÄ°…ü   „À„\  ¾@  ¸   º@  zH‰\$1í‰Ë‰|$¿ÿÿÿÿë´&    ·B·r‹$)Á‰È÷ØHÁ·Î‰Ş)Î‰ñ÷ÙHÎÈ9øs‰Õ‰ÇƒÂ9T$uË‹\$…í„ï   ·Eƒ;‰C·u‰s„ˆ   ‹|$1ö‹‰‹C‰B‹C‰B‹C‰B‹C‰B‹C‰B‹C ‰B‹C$‰B‹C(‰B ‹C,‰B$‹C0‰B(‹C4‰B,ƒÄ‰ğ[^_]Ã´&    v ÇC	   éâşÿÿt& ¾à  ¸€  º   éÿÿÿt& ‹S…Òt	ƒú	…ç  ‹|$‹¿”   ‰|$‹\‹—Ô   ‰|$öÆ°u*„Ò„®  ÇD$@  º   ¹@  ë t& ¾êÿÿÿéjÿÿÿÇD$   º€  ¹à  ‰÷‰\$)Ï‰ù÷ÙHÏ‰Ç)×‰ú÷ÚH×‹|$,OƒÇH‰<$ë-·Q‰Ã‰÷)Ó‰Ú÷ÚHÓ·Y)ß‰û÷ÛHßÚ9ês‰L$‰ÕƒÁ9$uÎ‹|$‹D$‹\$‰‡Ğ   …À„¦  ‹|$º   ‰øèüÿÿÿ…Àxƒàº   €‰Á‰øèüÿÿÿ¹   ºˆ  ¸è  èüÿÿÿ‹D$‹l$º   ‹€Ä   ‹8‰èèüÿÿÿ‰Æ…ÀˆS  1Àƒÿº   ”Àæ¿   Áà‰ñ	Á‰èèüÿÿÿ‰Æ…Àˆ(  ‹D$º   ‹€Ä   ‹xGÿƒø¸    Gø‰èèüÿÿÿ‰Æ…Àˆ÷   ƒæøº   ‰è‰ñ	ù¶Éèüÿÿÿ‰Æ…Àˆ×   ‹D$º   ‹¸Ğ   ·O
¶G	Áùƒàƒáğ	Á‰è¶Éèüÿÿÿ‰Æ…Àˆ¡   ¶Oº   ‰èèüÿÿÿ‰Æ…Àˆ‡   ¶O
º   ‰èèüÿÿÿ‰Æ…Àxq¹L   ºk   ‰è‹}\èüÿÿÿ‰Æ…ÀxW¹`   ºl   ‰èèüÿÿÿ‰Æ…Àx@ƒ¿Ü   „™   ‹D$‹€Ğ   ·P·@…ö…Eıÿÿ‰S‰Cé:ıÿÿ¾êÿÿÿ´&    f‹|$º   ‰øèüÿÿÿ…Àxƒàº   €‰Á‰øèüÿÿÿ¸è  ¹   ºˆ  èüÿÿÿ‹D$Ç€Ğ       éŞüÿÿÇ‡Ğ       ¾êÿÿÿë¡éıÿÿ‹|$ºN   ‰øèüÿÿÿ‰Æ…Àx„æˆ   ºN   ‰ø‰ñèüÿÿÿ‰Æ…À‰3ÿÿÿéaÿÿÿ                       ğ                   ù                                                                                                       0       Ğ   ğ           ğ  ğ  à  à      rstb Unable to get GPIO "rstb" xti Unable to get xti clock
 pdn Unable to get GPIO "pdn" bus width error
 Product ID error %x:%x
 tw9910 Product ID %0x:%0x
 un-supported revision
 norm select error
 Field type %d invalid
 drivers/media/i2c/tw9910.c tw9910 PAL SQ PAL CCIR601 PAL SQ (CIF) PAL CCIR601 (CIF) PAL SQ (QCIF) PAL CCIR601 (QCIF) NTSC SQ NTSC CCIR601 NTSC SQ (CIF) NTSC CCIR601 (CIF) NTSC SQ (QCIF) NTSC CCIR601 (QCIF) h   Wèüÿÿÿ‹³À   ‰ğèüÿÿÿ‰ğèüÿÿÿ‹ƒÈ   ZY…Àtº   èüÿÿÿ¹   ºè  ¸ô  èüÿÿÿ‹³Ì   éD  h    WèüÿÿÿXZºûÿÿÿéÒ  h    WèüÿÿÿºêÿÿÿY[éÒ  ¨„°   VRhj   Wèüÿÿÿ‹…”   ‹p\‹¾À   ‰øèüÿÿÿ‰øèüÿÿÿ‹†È   ƒÄ…Àtoº   èüÿÿÿºè  ¹   ¸ô  èüÿÿÿºíÿÿÿé¬  h#   Wèüÿÿÿ‹“À   _]éÒ  hY   WèüÿÿÿXZºíÿÿÿé¬  h@   Wèüÿÿÿ‹“È   Y^éÁ  ºíÿÿÿé¬  Vjh‚   Wèüÿÿÿ‹…”   Ç…Ô    °  Ç…Ø       Ç…Ğ      ‹p\‹¾À   ‰øèüÿÿÿ‰øèüÿÿÿ‹†È   ƒÄ…Àtº   èüÿÿÿ¹   ºè  ¸ô  èüÿÿÿ‰Øèüÿÿÿ‰Â…À„Ò  é¬  ƒÆh   Vèüÿÿÿ¸êÿÿÿ[^éœ  ƒÆh´   VèüÿÿÿXƒÈÿZéœ  ƒÁP¾êÿÿÿhÇ   QèüÿÿÿƒÄé    TW9910: missing platform data!
 I2C-Adapter doesn't support I2C_FUNC_SMBUS_BYTE_DATA
 O  Ş   	èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿlicense=GPL v2 author=Kuninori Morimoto description=V4L2 driver for TW9910 video decoder                    tw9910                                                                                     `                   ğ  à  0                                                   ğ              Ğ           à                                                                                                                         @    Ğ@    €      h   2  À    @  ´                            S  €à  [  Ğà  h  @ğ   v  hğ   ‰    x   ˜  ´ x    GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                            
       $        0   f     ,       &     B   Ğ        S   ğ   .                                	 a      ü     q       T    	 †      a                   •     W    ¢   @         ´   T   p   	 Æ      H     Ù   ğ  ÷     è   @  H     ú   ğ  ç       à  õ       Ä  2    	 ,  à  ¯    ;  ö      	               O           f      €     x      
                ¦          ¼  (   1     ×      0                   á     0     ø      P       `   @     &             1             ;             C             `             l             w             ‡             š             ­             »             Å             Ñ             ê             ÷                                       -             6             Q             k             y                        ™             ¨      
     ·      0      tw9910.c tw9910_g_std tw9910_get_selection tw9910_enum_mbus_code tw9910_g_tvnorms tw9910_remove tw9910_power_on tw9910_power_on.cold tw9910_s_power tw9910_probe tw9910_subdev_ops tw9910_probe.cold tw9910_ntsc_scales tw9910_get_fmt tw9910_pal_scales tw9910_s_std tw9910_s_stream tw9910_s_stream.cold tw9910_set_fmt tw9910_set_fmt.cold tw9910_i2c_driver_init tw9910_i2c_driver tw9910_i2c_driver_exit __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 tw9910_id tw9910_subdev_core_ops tw9910_subdev_video_ops tw9910_subdev_pad_ops __fentry__ gpiod_put clk_put v4l2_async_unregister_subdev clk_prepare clk_enable gpiod_set_value usleep_range_state gpiod_get_optional clk_unprepare _dev_info clk_disable __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init clk_get i2c_smbus_read_byte_data _dev_err v4l2_async_register_subdev i2c_smbus_write_byte_data __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw9910_id_device_table       &  1   &  ¡   &  Ñ   &  ñ   &    '    (    )  !  &  >  *  S  +  n  ,  ‚  -    	  –  .  ·  ,  Ë  -  Ü  ,  ğ  -  û  '    /  !  &  I  1  P  /  d  ,  x  -  ‘  &  ¶  2  ×  3  ì  "  ó  4  ø  	  ÿ  5  #  	  *  .  o  6  ¾  '  Ï  (  ñ  &  i  "  á  "  ñ  &  &  6  K  9  f  9  x  9  ™  6  ½  9  á  &    6  3  9  S  6  l  9  |  6  á  &  W  "  K  "  ”  "  ¶  "  H	  6  _	  9  s	  -  	  6  º	  9  ë	  6  
  9  A
  9  [
  9  u
  9  
  9  ¦
  9  ü
  6    9  '  -  b  6  |  9  §  
  «  
  Æ  
    
  ;  
  R  
  ‹  
    
    9  ›  9  ­  
  Ê  
    
               	  h   "                                                 $     (     ,     0        	     0     1     /  1   ,  E   -  U     [   7  l     r   7     	  “   7  ©   1  °   /  Ç   ,  Û   -  ê   	  ğ   7    	    7    	    0  >  	  D  0  h  "  x  1    /  –  ,  ª  -  ±  8  È  	  Î  7  â  	  è  7     	    7  P     g     ~     å     ı         ,    6    »    À    Ú    ò                 	     &          :     ;          =  @   "  L   "  \   "  d     p     t     x     ¨     ¬     ¼     È          @  	  L  	  X  	  d  	  p  	  |  	     	  ¬  	  ¸  	  Ä  	  Ğ  	  Ü  	   .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel__bug_table .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @                       	   @       @  È              )             à  €                   %   	   @                         /             `                     8             `  4                  4   	   @       (  h               E      2       ”  ¬                X             @                   T   	   @         ˆ     	         g      2       T  V                 z             ª                    v   	   @                         Š             ¶                    †   	   @       (                   ™             Ê  
                  •   	   @       H                  ¤             Ô  Y                  ±             @  è                  ­   	   @       X   À               ¹      0       (                   Â              ;                     Ò             <  (                                d        &         	              d  Ù                               !  å                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      <      4     (               èüÿÿÿU1ÀWVSƒìèüÿÿÿ;    ƒ  ‰Å´&    ‰èèüÿÿÿpl‰Ã‰ğèüÿÿÿ‹S$‹{(‰$‹
…ÿ„B  ‹G(…À„7  öÅ…V  ÷Á  @ ‰L$„F  èüÿÿÿ‹$‰D$‹J…É‹L$„;  B‰$‹T$‹$‰L$èüÿÿÿ‹    ‹L$9Ğs'‹$ÇD$    ‹ #    tó¼À‰D$;T$†   ‹$‹T$£ƒĞ   ‰Ø‰$èüÿÿÿº   ‰Øèüÿÿÿ“€   ‹$„Àu‹C$PCá € ‰$‰L$u‹O…Ét‰T$èüÿÿÿ‹T$‹¹    #    tó¼À‰Á9    w‹K$º    ‹©    …Ÿ   ‹$1Éèüÿÿÿ…Àui‹D$…Àu‹W …Òt
‹$èüÿÿÿf‰ğèüÿÿÿEèüÿÿÿ‰Å9    ‡{şÿÿƒÄ[^_]Ãv 1Ò‰ØèüÿÿÿëÍt& ‹C$ƒÀ‰$é½şÿÿ´&    v ‰D$º    ¸    èüÿÿÿ‹L$…À„xÿÿÿéüÿÿÿt&   € ‰‰Øèüÿÿÿéuÿÿÿt& èüÿÿÿU‰ÅWVSƒìèüÿÿÿ1Àèüÿÿÿ;    s‰Ãt& ‰Øèüÿÿÿxl‰Æ‰øèüÿÿÿ‹F$÷     tB‹N@…Ét;‹V(…Òt4P£hs+‹F$‹ ©  € uw©   t`¸   ‰$èüÿÿÿ„Àu/´&    ‰øèüÿÿÿCèüÿÿÿ‰Ã9    w‡èüÿÿÿ1ÀƒÄ[^_]Ã¸   èüÿÿÿ‹V$‰Á‹ ÷Ğ#BtÀ£)‹$s¸F1Éèüÿÿÿë¬t& 1Éº   ‰ğèüÿÿÿë˜    ˆ  
                   4Eff. affinity %*pbl of IRQ %u contains only offline CPUs after offlining CPU %u
  4IRQ%u: set affinity failed(%d).
 QÿshT   èüÿÿÿƒÄés  ‰L$ÿt$ÿsÿt$Rh    èüÿÿÿ‹L$ƒÄéä           migrate_one_irq  GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ                                                                   
                    <                  
 A           ^              i              z              ‚                                          ´              Ä              Ï              á              ù                           )             =             V             g             t                          ˜     ğ     °             À             Ó             è             ı                          $             <              cpuhotplug.c __func__.0 _rs.1 irq_migrate_all_off_this_cpu.cold irq_migrate_all_off_this_cpu __fentry__ irq_get_next_irq nr_irqs irq_to_desc _raw_spin_lock debug_smp_processor_id cpumask_any_but nr_cpu_ids __cpu_online_mask irq_force_complete_move irq_fixup_move_pending __x86_indirect_thunk_ecx irq_do_set_affinity __x86_indirect_thunk_edx _raw_spin_unlock ___ratelimit irq_shutdown_and_deactivate _printk irq_affinity_online_cpu irq_lock_sparse _raw_spin_lock_irq housekeeping_enabled _raw_spin_unlock_irq irq_unlock_sparse housekeeping_cpumask irq_set_affinity_locked irq_startup                +     7     s     Ÿ     ¥     À     î     ú     /    @    N    X    o    Š    “    ›    £    ½    å  	  ê    ï        !    /    6    <    K    W    ™     «  !  ³    »    Â  "  Ö  #  ö  $  
  %  Ô               
     '     ,          8                 .symtab .strtab .shstrtab .rel.text .data .bss .rodata.str1.4 .rel.text.unlikely .rel__mcount_loc .rodata .comment .note.GNU-stack .note.gnu.property                                                       @                       	   @       	  H              %             P                    +             l                     0      2       l  w                 C             ã  <                  ?   	   @       d
  0               V                                 R   	   @       ”
                 c             (                    k      0       8                   t              K                     „             L  (                                t  `     
         	              Ô  H                               ¤
  —                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              cmd_kernel/irq/cpuhotplug.o := gcc -Wp,-MMD,kernel/irq/.cpuhotplug.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned    -DKBUILD_MODFILE='"kernel/irq/cpuhotplug"' -DKBUILD_BASENAME='"cpuhotplug"' -DKBUILD_MODNAME='"cpuhotplug"' -D__KBUILD_MODNAME=kmod_cpuhotplug -c -o kernel/irq/cpuhotplug.o kernel/irq/cpuhotplug.c 

source_kernel/irq/cpuhotplug.o := kernel/irq/cpuhotplug.c

deps_kernel/irq/cpuhotplug.o := \
    $(wildcard include/config/GENERIC_IRQ_EFFECTIVE_AFF_MASK) \
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
  include/linux/interrupt.h \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/PROC_FS) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wlinux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/kern_levels.h \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/ratelimit_types.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  include/linux/instruction_pointer.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/linux/sched.h \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/IOMMU_SVA) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcarcmd_drivers/media/i2c/tw9910.o := gcc -Wp,-MMD,drivers/media/i2c/.tw9910.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"tw9910"' -DKBUILD_MODNAME='"tw9910"' -D__KBUILD_MODNAME=kmod_tw9910 -c -o drivers/media/i2c/tw9910.o drivers/media/i2c/tw9910.c 

source_drivers/media/i2c/tw9910.o := drivers/media/i2c/tw9910.c

deps_drivers/media/i2c/tw9910.o := \
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
  include/linux/clk.h \
    $(wildcard include/config/COMMON_CLK) \
    $(wildcard include/config/HAVE_CLK_PREPARE) \
    $(wildcard include/config/HAVE_CLK) \
    $(wildcard include/config/OF) \
  include/linux/err.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
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
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
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
  include/linux/kstrtox.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/kern_levels.h \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/ratelimit_types.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  include/linux/instruction_pointer.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/linux/mutex.h \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
    $(wildcard include/config/PREEMPT_COUNT) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  inc