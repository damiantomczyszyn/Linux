k.hardirqs_off = true;
			hlock.references = 0;

			verify_lock_unused(lock, &hlock, subclass);
		}
		return;
	}

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_acquire(lock, subclass, trylock, read, check,
		       irqs_disabled_flags(flags), nest_lock, ip, 0, 0);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquire);

void lock_release(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_release(lock, ip);

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	if (__lock_release(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_release);

noinstr int lock_is_held_type(const struct lockdep_map *lock, int read)
{
	unsigned long flags;
	int ret = LOCK_STATE_NOT_HELD;

	/*
	 * Avoid false negative lockdep_assert_held() and
	 * lockdep_assert_not_held().
	 */
	if (unlikely(!lockdep_enabled()))
		return LOCK_STATE_UNKNOWN;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	ret = __lock_is_held(lock, read);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return ret;
}
EXPORT_SYMBOL_GPL(lock_is_held_type);
NOKPROBE_SYMBOL(lock_is_held_type);

struct pin_cookie lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return cookie;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	cookie = __lock_pin_lock(lock);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return cookie;
}
EXPORT_SYMBOL_GPL(lock_pin_lock);

void lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_repin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_repin_lock);

void lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_unpin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_unpin_lock);

#ifdef CONFIG_LOCK_STAT
static void print_lock_contention_bug(struct task_struct *curr,
				      struct lockdep_map *lock,
				      unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=================================\n");
	pr_warn("WARNING: bad contention detected!\n");
	print_kernel_ident();
	pr_warn("---------------------------------\n");
	pr_warn("%s/%d is trying to contend lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no locks held!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static void
__lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	int i, contention_point, contending_point;

	depth = curr->lockdep_depth;
	/*
	 * Whee, we contended on this lock, except it seems we're not
	 * actually trying to acquire anything much at all..
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, ip);
		return;
	}

	if (hlock->instance != lock)
		return;

	hlock->waittime_stamp = lockstat_clock();

	contention_point = lock_point(hlock_class(hlock)->contention_point, ip);
	contending_point = lock_point(hlock_class(hlock)->contending_point,
				      lock->ip);

	stats = get_lock_stats(hlock_class(hlock));
	if (contention_point < LOCKSTAT_POINTS)
		stats->contention_point[contention_point]++;
	if (contending_point < LOCKSTAT_POINTS)
		stats->contending_point[contending_point]++;
	if (lock->cpu != smp_processor_id())
		stats->bounces[bounce_contended + !!hlock->read]++;
}

static void
__lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	u64 now, waittime = 0;
	int i, cpu;

	depth = curr->lockdep_depth;
	/*
	 * Yay, we acquired ownership of this lock we didn't try to
	 * acquire, how the heck did that happen?
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, _RET_IP_);
		return;
	}

	if (hlock->instance != lock)
		return;

	cpu = smp_processor_id();
	if (hlock->waittime_stamp) {
		now = lockstat_clock();
		waittime = now - hlock->waittime_stamp;
		hlock->holdtime_stamp = now;
	}

	stats = get_lock_stats(hlock_class(hlock));
	if (waittime) {
		if (hlock->read)
			lock_time_inc(&stats->read_waittime, waittime);
		else
			lock_time_inc(&stats->write_waittime, waittime);
	}
	if (lock->cpu != cpu)
		stats->bounces[bounce_acquired + !!hlock->read]++;

	lock->cpu = cpu;
	lock->ip = ip;
}

void lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_contended(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_contended(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_contended);

void lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquired(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_acquired(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquired);
#endif

/*
 * Used by the testsuite, sanitize the validator state
 * after a simulated failure:
 */

void lockdep_reset(void)
{
	unsigned long flags;
	int i;

	raw_local_irq_save(flags);
	lockdep_init_task(current);
	memset(current->held_locks, 0, MAX_LOCK_DEPTH*sizeof(struct held_lock));
	nr_hardirq_chains = 0;
	nr_softirq_chains = 0;
	nr_process_chains = 0;
	debug_locks = 1;
	for (i = 0; i < CHAINHASH_SIZE; i++)
		INIT_HLIST_HEAD(chainhash_table + i);
	raw_local_irq_restore(flags);
}

/* Remove a class from a lock chain. Must be called with the graph lock held. */
static void remove_class_from_lock_chain(struct pending_free *pf,
					 struct lock_chain *chain,
					 struct lock_class *class)
{
#ifdef CONFIG_PROVE_LOCKING
	int i;

	for (i = chain->base; i < chain->base + chain->depth; i++) {
		if (chain_hlock_class_idx(chain_hlocks[i]) != class - lock_classes)
			continue;
		/*
		 * Each lock class occurs at most once in a lock chain so once
		 * we found a match we can break out of this loop.
		 */
		goto free_lock_chain;
	}
	/* Since the chain has not been modified, return. */
	return;

free_lock_chain:
	free_chain_hlocks(chain->base, chain->depth);
	/* Overwrite the chain key for concurrent RCU readers. */
	WRITE_ONCE(chain->chain_key, INITIAL_CHAIN_KEY);
	dec_chains(chain->irq_context);

	/*
	 * Note: calling hlist_del_rcu() from inside a
	 * hlist_for_each_entry_rcu() loop is safe.
	 */
	hlist_del_rcu(&chain->entry);
	__set_bit(chain - lock_chains, pf->lock_chains_being_freed);
	nr_zapped_lock_chains++;
#endif
}

/* Must be called with the graph lock held. */
static void remove_class_from_lock_chains(struct pending_free *pf,
					  struct lock_class *class)
{
	struct lock_chain *chain;
	struct hlist_head *head;
	int i;

	for (i = 0; i < ARRAY_SIZE(chainhash_table); i++) {
		head = chainhash_table + i;
		hlist_for_each_entry_rcu(chain, head, entry) {
			remove_class_from_lock_chain(pf, chain, class);
		}
	}
}

/*
 * Remove all references to a lock class. The caller must hold the graph lock.
 */
static void zap_class(struct pending_free *pf, struct lock_class *class)
{
	struct lock_list *entry;
	int i;

	WARN_ON_ONCE(!class->key);

	/*
	 * Remove all dependencies this lock is
	 * involved in:
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		entry = list_entries + i;
		if (entry->class != class && entry->links_to != class)
			continue;
		__clear_bit(i, list_entries_in_use);
		nr_list_entries--;
		list_del_rcu(&entry->entry);
	}
	if (list_empty(&class->locks_after) &&
	    list_empty(&class->locks_before)) {
		list_move_tail(&class->lock_entry, &pf->zapped);
		hlist_del_rcu(&class->hash_entry);
		WRITE_ONCE(class->key, NULL);
		WRITE_ONCE(class->name, NULL);
		nr_lock_classes--;
		__clear_bit(class - lock_classes, lock_classes_in_use);
		if (class - lock_classes == max_lock_class_idx)
			max_lock_class_idx--;
	} else {
		WARN_ONCE(true, "%s() failed for class %s\n", __func__,
			  class->name);
	}

	remove_class_from_lock_chains(pf, class);
	nr_zapped_classes++;
}

static void reinit_class(struct lock_class *class)
{
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	memset_startat(class, 0, key);
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
}

static inline int within(const void *addr, void *start, unsigned long size)
{
	return addr >= start && addr < start + size;
}

static bool inside_selftest(void)
{
	return current == lockdep_selftest_task_struct;
}

/* The caller must hold the graph lock. */
static struct pending_free *get_pending_free(void)
{
	return delayed_free.pf + delayed_free.index;
}

static void free_zapped_rcu(struct rcu_head *cb);

/*
 * Schedule an RCU callback if no RCU callback is pending. Must be called with
 * the graph lock held.
 */
static void call_rcu_zapped(struct pending_free *pf)
{
	WARN_ON_ONCE(inside_selftest());

	if (list_empty(&pf->zapped))
		return;

	if (delayed_free.scheduled)
		return;

	delayed_free.scheduled = true;

	WARN_ON_ONCE(delayed_free.pf + delayed_free.index != pf);
	delayed_free.index ^= 1;

	call_rcu(&delayed_free.rcu_head, free_zapped_rcu);
}

/* The caller must hold the graph lock. May be called from RCU context. */
static void __free_zapped_classes(struct pending_free *pf)
{
	struct lock_class *class;

	check_data_structures();

	list_for_each_entry(class, &pf->zapped, lock_entry)
		reinit_class(class);

	list_splice_init(&pf->zapped, &free_lock_classes);

#ifdef CONFIG_PROVE_LOCKING
	bitmap_andnot(lock_chains_in_use, lock_chains_in_use,
		      pf->lock_chains_being_freed, ARRAY_SIZE(lock_chains));
	bitmap_clear(pf->lock_chains_being_freed, 0, ARRAY_SIZE(lock_chains));
#endif
}

static void free_zapped_rcu(struct rcu_head *ch)
{
	struct pending_free *pf;
	unsigned long flags;

	if (WARN_ON_ONCE(ch != &delayed_free.rcu_head))
		return;

	raw_local_irq_save(flags);
	lockdep_lock();

	/* closed head */
	pf = delayed_free.pf + (delayed_free.index ^ 1);
	__free_zapped_classes(pf);
	delayed_free.scheduled = false;

	/*
	 * If there's anything on the open list, close and start a new callback.
	 */
	call_rcu_zapped(delayed_free.pf + delayed_free.index);

	lockdep_unlock();
	raw_local_irq_restore(flags);
}

/*
 * Remove all lock classes from the class hash table and from the
 * all_lock_classes list whose key or name is in the address range [start,
 * start + size). Move these lock classes to the zapped_classes list. Must
 * be called with the graph lock held.
 */
static void __lockdep_free_key_range(struct pending_free *pf, void *start,
				     unsigned long size)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i;

	/* Unhash all classes that were created by a module. */
	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			if (!within(class->key, start, size) &&
			    !within(class->name, start, size))
				continue;
			zap_class(pf, class);
		}
	}
}

/*
 * Used in module.c to remove lock classes from memory that is going to be
 * freed; and possibly re-used by other modules.
 *
 * We will have had one synchronize_rcu() before getting here, so we're
 * guaranteed nobody will look up these exact classes -- they're properly dead
 * but still allocated.
 */
static void lockdep_free_key_range_reg(void *start, unsigned long size)
{
	struct pending_free *pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	pf = get_pending_free();
	__lockdep_free_key_range(pf, start, size);
	call_rcu_zapped(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	/*
	 * Wait for any possible iterators from look_up_lock_class() to pass
	 * before continuing to free the memory they refer to.
	 */
	synchronize_rcu();
}

/*
 * Free all lockdep keys in the range [start, start+size). Does not sleep.
 * Ignores debug_locks. Must only be used by the lockdep selftests.
 */
static void lockdep_free_key_range_imm(void *start, unsigned long size)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_free_key_range(pf, start, size);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_free_key_range(void *start, unsigned long size)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_free_key_range_imm(start, size);
	else
		lockdep_free_key_range_reg(start, size);
}

/*
 * Check whether any element of the @lock->class_cache[] array refers to a
 * registered lock class. The caller must hold either the graph lock or the
 * RCU read lock.
 */
static bool lock_class_cache_is_registered(struct lockdep_map *lock)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i, j;

	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			for (j = 0; j < NR_LOCKDEP_CACHING_CLASSES; j++)
				if (lock->class_cache[j] == class)
					return true;
		}
	}
	return false;
}

/* The caller must hold the graph lock. Does not sleep. */
static void __lockdep_reset_lock(struct pending_free *pf,
				 struct lockdep_map *lock)
{
	struct lock_class *class;
	int j;

	/*
	 * Remove all classes this lock might have:
	 */
	for (j = 0; j < MAX_LOCKDEP_SUBCLASSES; j++) {
		/*
		 * If the class exists we look it up and zap it:
		 */
		class = look_up_lock_class(lock, j);
		if (class)
			zap_class(pf, class);
	}
	/*
	 * Debug check: in the end all mapped classes should
	 * be gone.
	 */
	if (WARN_ON_ONCE(lock_class_cache_is_registered(lock)))
		debug_locks_off();
}

/*
 * Remove all information lockdep has about a lock if debug_locks == 1. Free
 * released data structures from RCU context.
 */
static void lockdep_reset_lock_reg(struct lockdep_map *lock)
{
	struct pending_free *pf;
	unsigned long flags;
	int locked;

	raw_local_irq_save(flags);
	locked = graph_lock();
	if (!locked)
		goto out_irq;

	pf = get_pending_free();
	__lockdep_reset_lock(pf, lock);
	call_rcu_zapped(pf);

	graph_unlock();
out_irq:
	raw_local_irq_restore(flags);
}

/*
 * Reset a lock. Does not sleep. Ignores debug_locks. Must only be used by the
 * lockdep selftests.
 */
static void lockdep_reset_lock_imm(struct lockdep_map *lock)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_reset_lock(pf, lock);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_reset_lock(struct lockdep_map *lock)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_reset_lock_imm(lock);
	else
		lockdep_reset_lock_reg(lock);
}

/*
 * Unregister a dynamically allocated key.
 *
 * Unlike lockdep_register_key(), a search is always done to find a matching
 * key irrespective of debug_locks to avoid potential invalid access to freed
 * memory in lock_class entry.
 */
void lockdep_unregister_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head = keyhashentry(key);
	struct lock_class_key *k;
	struct pending_free /x86/include/uapi/asm/bitsperlong.h \
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
  include/ELF                      0      4     (               �����S�X\����������   �����1�[Ð�����S�Ë@�HԋC(=	� tHw&=	� tW=	� u�>   ��������C|1�[Ít& �=$	� u�=   ��������C|1�[Í�    �<   ��������C|1�[Ít& ��?   ��������C|1�[Ít& ������WVS�Ëp�@(�~�=	� �R  wp=	� �U  v3=	� �`  =	� ��   �K|�?   [��^_�������&    �v = 	� ty=	� ub�Vp�K|����[��^��_�������&    =	� tYv=$	� u0�K|�=   롍t& �=	� ty=	� u�K|�<   냍�&    �����[^_Í�&    �Vp�K|������Vp���������������x̓{|�   ���%�   	��Vp�������P����t& �Vp���������������x��S|���ɀ��u$���Vp������������    �K|�>   �����f��Vp�K|����������t& ��Vp�K|����������t& ������W��VS�@�P�R�����%   =   �0  �G��
   j ������ 	� ��   ��j h�   j jj h�   j j ������	� ��   �؃�$j h�   j jj h�   j j ������	� ��   �؃� j h�   j jj h�   j j ������	� ��   �؃� j h�   j jj h�   j j ������	� ��   �؃� j j j jj jj j ������	� ��   �؃� j j j jj jj j ������	� ��   �؃� j h�   j jj h�   j j ������� ��t�Ht�   j �$	� ��   ��h�   j jj h�   j j ������� ��t�Ht�   j �	� ��   ��jzj jj h�   j j ������� ��t�Ht�   j �	� ��   ��jzj jj h�   j j ������� ��t�Ht�   �^l���   ���d   ���������[^_Ít& �������������獴&    �v �����UWV��S��(�T$d�    �D$$���   �D$�D$	�D$
�D$�D$�D$
�T$��@��ۃ�@����\$���L$���\$���|$���\$�D$�D$!������L$�L$f�D$"�$�   ��0  ����t& �G��<�t	���Љ��������y�D$��4  �D$$d+    u
��(1�[^_]�������t& �����UW��V��S�����   �D$��tz��0  �X��у�9�tg����   �������	�0  ����   1ۺ9   ��v ��r  �������   ��q  ���������y��   ��    �4$�t& �����   ��0  1�������9�t[�����"���������xG�����������0  Eȉ�����"�����������x��0  �������	���0  1���[^_]Í�&    ��0  �����4$1۸   ������v ��  ��<��>���	���  �Љ��������y���   ������                               `                      D                                                                                                           �       `  �        60 Hz 50 Hz 6%s: Standard: %s
 6%s: Channel: %d
 6%s: Input: %d
 tw2804 �����V�   S����4   �  �    DspPVh   �������0  ��PVh    �������0  ����PVh3   ������؃�$[^������W�G��  Q� QP�GT��   �0h    ��������  ��ht   U�����X�����Z�  ��Ph(   U�������������  ��0  ����PhL   U�������������   6%s %d-%04x: chip found @ 0x%02x (%s)
 channel %d is not between 1 and 4!
 error initializing TW2804 channel %d
   error initializing TW2804 global registers
 ������    �    ������    �����author=Micronas USA Inc description=TW2804/TW2802 V4L2 i2c driver license=GPL v2                  tw2804                                                          `           �                                                                                             �                                                                          �       �� � �� 	
��
                   �     /   �   �                               	 =       h    	               O   `  f    \   @         g   �        w   h   0    	 �   �  <    �     u    �   p       �      p     �   �   \    	               �            �       �     
                 6     *     Q  B        h      0                   r  `   0     �  �   P     �             �             �             �             �                                       *             C             P             e             �             �             �             �             �             �             �           �             �      
           0      tw2804.c tw2804_remove tw2804_g_volatile_ctrl tw2804_s_ctrl tw2804_log_status tw2804_probe tw2804_ops tw2804_ctrl_ops tw2804_probe.cold tw2804_s_std tw2804_s_video_routing global_registers channel_registers tw2804_s_video_routing.cold tw2804_driver_init tw2804_driver tw2804_driver_exit __UNIQUE_ID_author268 __UNIQUE_ID_description267 __UNIQUE_ID_license266 tw2804_id tw2804_core_ops tw2804_video_ops __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free i2c_smbus_read_byte_data i2c_smbus_write_byte_data _printk v4l2_ctrl_subdev_log_status __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std __stack_chk_guard __stack_chk_fail _dev_err __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw2804_id_device_table                 !  !     P   "  o   "  �   "  �   "  �     �  "  �  "  a    t  &  �  '  �    �  (  �  )  �    �  *      $  *  .    N  *  X    x  *  �    �  *  �    �  *  �    �  *        *  9    Q  *  k    �  *  �  !  �    �  +  �  #  �  +    ,      s    �    �  #  �  "  �  #  S    g    q  #    #  5  #  �    F    �    z                   h                                                             &     +   $  ;     @   $  S     X   $  �   
  �   $  �   
  �   -  �   
  �   -  �   
  �   -  d   %  �     �     �     �                  .     /          1  @     L     `     �     �     �     �      .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   �                    	   @         �              )             �  �                   %   	   @       �                  /             `                     8             `                     4   	   @          @               E      2       �  K                 X             �  �                  T   	   @       @  �      	         g      2       �	  �                 z             `
                    v   	   @       �                  �             t
  
                  �   	   @                        �             ~
  Q                  �             �
  �                  �   	   @          8               �      0       b                   �              u                     �             x  (                                �  @              	              �  .                               X  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          linux/log2.h \
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
    $(wildca