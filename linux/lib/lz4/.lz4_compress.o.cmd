(likely(hlock_class(this)->usage_mask & new_mask))
		return 1;

	if (!graph_lock())
		return 0;
	/*
	 * Make sure we didn't race:
	 */
	if (unlikely(hlock_class(this)->usage_mask & new_mask))
		goto unlock;

	if (!hlock_class(this)->usage_mask)
		debug_atomic_dec(nr_unused_locks);

	hlock_class(this)->usage_mask |= new_mask;

	if (new_bit < LOCK_TRACE_STATES) {
		if (!(hlock_class(this)->usage_traces[new_bit] = save_trace()))
			return 0;
	}

	if (new_bit < LOCK_USED) {
		ret = mark_lock_irq(curr, this, new_bit);
		if (!ret)
			return 0;
	}

unlock:
	graph_unlock();

	/*
	 * We must printk outside of the graph_lock:
	 */
	if (ret == 2) {
		printk("\nmarked lock as {%s}:\n", usage_str[new_bit]);
		print_lock(this);
		print_irqtrace_events(curr);
		dump_stack();
	}

	return ret;
}

static inline short task_wait_context(struct task_struct *curr)
{
	/*
	 * Set appropriate wait type for the context; for IRQs we have to take
	 * into account force_irqthread as that is implied by PREEMPT_RT.
	 */
	if (lockdep_hardirq_context()) {
		/*
		 * Check if force_irqthreads will run us threaded.
		 */
		if (curr->hardirq_threaded || curr->irq_config)
			return LD_WAIT_CONFIG;

		return LD_WAIT_SPIN;
	} else if (curr->softirq_context) {
		/*
		 * Softirqs are always threaded.
		 */
		return LD_WAIT_CONFIG;
	}

	return LD_WAIT_MAX;
}

static int
print_lock_invalid_wait_context(struct task_struct *curr,
				struct held_lock *hlock)
{
	short curr_inner;

	if (!debug_locks_off())
		return 0;
	if (debug_locks_silent)
		return 0;

	pr_warn("\n");
	pr_warn("=============================\n");
	pr_warn("[ BUG: Invalid wait context ]\n");
	print_kernel_ident();
	pr_warn("-----------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("other info that might help us debug this:\n");

	curr_inner = task_wait_context(curr);
	pr_warn("context-{%d:%d}\n", curr_inner, curr_inner);

	lockdep_print_held_locks(curr);

	pr_warn("stack backtrace:\n");
	dump_stack();

	return 0;
}

/*
 * Verify the wait_type context.
 *
 * This check validates we take locks in the right wait-type order; that is it
 * ensures that we do not take mutexes inside spinlocks and do not attempt to
 * acquire spinlocks inside raw_spinlocks and the sort.
 *
 * The entire thing is slightly more complex because of RCU, RCU is a lock that
 * can be taken from (pretty much) any context but also has constraints.
 * However when taken in a stricter environment the RCU lock does not loosen
 * the constraints.
 *
 * Therefore we must look for the strictest environment in the lock stack and
 * compare that to the lock we're trying to acquire.
 */
static int check_wait_context(struct task_struct *curr, struct held_lock *next)
{
	u8 next_inner = hlock_class(next)->wait_type_inner;
	u8 next_outer = hlock_class(next)->wait_type_outer;
	u8 curr_inner;
	int depth;

	if (!next_inner || next->trylock)
		return 0;

	if (!next_outer)
		next_outer = next_inner;

	/*
	 * Find start of current irq_context..
	 */
	for (depth = curr->lockdep_depth - 1; depth >= 0; depth--) {
		struct held_lock *prev = curr->held_locks + depth;
		if (prev->irq_context != next->irq_context)
			break;
	}
	depth++;

	curr_inner = task_wait_context(curr);

	for (; depth < curr->lockdep_depth; depth++) {
		struct held_lock *prev = curr->held_locks + depth;
		u8 prev_inner = hlock_class(prev)->wait_type_inner;

		if (prev_inner) {
			/*
			 * We can have a bigger inner than a previous one
			 * when outer is smaller than inner, as with RCU.
			 *
			 * Also due to trylocks.
			 */
			curr_inner = min(curr_inner, prev_inner);
		}
	}

	if (next_outer > curr_inner)
		return print_lock_invalid_wait_context(curr, next);

	return 0;
}

#else /* CONFIG_PROVE_LOCKING */

static inline int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return 0;
}

static inline int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	return 0;
}

static inline int check_wait_context(struct task_struct *curr,
				     struct held_lock *next)
{
	return 0;
}

#endif /* CONFIG_PROVE_LOCKING */

/*
 * Initialize a lock instance's lock-class mapping info:
 */
void lockdep_init_map_type(struct lockdep_map *lock, const char *name,
			    struct lock_class_key *key, int subclass,
			    u8 inner, u8 outer, u8 lock_type)
{
	int i;

	for (i = 0; i < NR_LOCKDEP_CACHING_CLASSES; i++)
		lock->class_cache[i] = NULL;

#ifdef CONFIG_LOCK_STAT
	lock->cpu = raw_smp_processor_id();
#endif

	/*
	 * Can't be having no nameless bastards around this place!
	 */
	if (DEBUG_LOCKS_WARN_ON(!name)) {
		lock->name = "NULL";
		return;
	}

	lock->name = name;

	lock->wait_type_outer = outer;
	lock->wait_type_inner = inner;
	lock->lock_type = lock_type;

	/*
	 * No key, no joy, we need to hash something.
	 */
	if (DEBUG_LOCKS_WARN_ON(!key))
		return;
	/*
	 * Sanity check, the lock-class key must either have been allocated
	 * statically or must have been registered as a dynamic key.
	 */
	if (!static_obj(key) && !is_dynamic_key(key)) {
		if (debug_locks)
			printk(KERN_ERR "BUG: key %px has not been registered!\n", key);
		DEBUG_LOCKS_WARN_ON(1);
		return;
	}
	lock->key = key;

	if (unlikely(!debug_locks))
		return;

	if (subclass) {
		unsigned long flags;

		if (DEBUG_LOCKS_WARN_ON(!lockdep_enabled()))
			return;

		raw_local_irq_save(flags);
		lockdep_recursion_inc();
		register_lock_class(lock, subclass, 1);
		lockdep_recursion_finish();
		raw_local_irq_restore(flags);
	}
}
EXPORT_SYMBOL_GPL(lockdep_init_map_type);

struct lock_class_key __lockdep_no_validate__;
EXPORT_SYMBOL_GPL(__lockdep_no_validate__);

static void
print_lock_nested_lock_not_held(struct task_struct *curr,
				struct held_lock *hlock,
				unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("==================================\n");
	pr_warn("WARNING: Nested lock was not taken\n");
	print_kernel_ident();
	pr_warn("----------------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("\nbut this task is not holding:\n");
	pr_warn("%s\n", hlock->nest_lock->name);

	pr_warn("\nstack backtrace:\n");
	dump_stack();

	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static int __lock_is_held(const struct lockdep_map *lock, int read);

/*
 * This gets called for every mutex_lock*()/spin_lock*() operation.
 * We maintain the dependency maps and validate the locking attempt:
 *
 * The callers must make sure that IRQs are disabled before calling it,
 * otherwise we could get an interrupt which would want to take locks,
 * which would end up in lockdep again.
 */
static int __lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check, int hardirqs_off,
			  struct lockdep_map *nest_lock, unsigned long ip,
			  int references, int pin_count)
{
	struct task_struct *curr = current;
	struct lock_class *class = NULL;
	struct held_lock *hlock;
	unsigned int depth;
	int chain_head = 0;
	int class_idx;
	u64 chain_key;

	if (unlikely(!debug_locks))
		return 0;

	if (!prove_locking || lock->key == &__lockdep_no_validate__)
		check = 0;

	if (subclass < NR_LOCKDEP_CACHING_CLASSES)
		class = lock->class_cache[subclass];
	/*
	 * Not cached?
	 */
	if (unlikely(!class)) {
		class = register_lock_class(lock, subclass, 0);
		if (!class)
			return 0;
	}

	debug_class_ops_inc(class);

	if (very_verbose(class)) {
		printk("\nacquire class [%px] %s", class->key, class->name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		printk(KERN_CONT "\n");
		dump_stack();
	}

	/*
	 * Add the lock to the list of currently held locks.
	 * (we dont increase the depth just yet, up until the
	 * dependency checks are done)
	 */
	depth = curr->lockdep_depth;
	/*
	 * Ran out of static storage for our per-task lock stack again have we?
	 */
	if (DEBUG_LOCKS_WARN_ON(depth >= MAX_LOCK_DEPTH))
		return 0;

	class_idx = class - lock_classes;

	if (depth) { /* we're holding locks */
		hlock = curr->held_locks + depth - 1;
		if (hlock->class_idx == class_idx && nest_lock) {
			if (!references)
				references++;

			if (!hlock->references)
				hlock->references++;

			hlock->references += references;

			/* Overflow */
			if (DEBUG_LOCKS_WARN_ON(hlock->references < references))
				return 0;

			return 2;
		}
	}

	hlock = curr->held_locks + depth;
	/*
	 * Plain impossible, we just registered it and checked it weren't no
	 * NULL like.. I bet this mushroom I ate was good!
	 */
	if (DEBUG_LOCKS_WARN_ON(!class))
		return 0;
	hlock->class_idx = class_idx;
	hlock->acquire_ip = ip;
	hlock->instance = lock;
	hlock->nest_lock = nest_lock;
	hlock->irq_context = task_irq_context(curr);
	hlock->trylock = trylock;
	hlock->read = read;
	hlock->check = check;
	hlock->hardirqs_off = !!hardirqs_off;
	hlock->references = references;
#ifdef CONFIG_LOCK_STAT
	hlock->waittime_stamp = 0;
	hlock->holdtime_stamp = lockstat_clock();
#endif
	hlock->pin_count = pin_count;

	if (check_wait_context(curr, hlock))
		return 0;

	/* Initialize the lock usage bit */
	if (!mark_usage(curr, hlock, check))
		return 0;

	/*
	 * Calculate the chain hash: it's the combined hash of all the
	 * lock keys along the dependency chain. We save the hash value
	 * at every step so that we can get the current hash easily
	 * after unlock. The chain hash is then used to cache dependency
	 * results.
	 *
	 * The 'key ID' is what is the most compact key value to drive
	 * the hash, not class->key.
	 */
	/*
	 * Whoops, we did it again.. class_idx is invalid.
	 */
	if (DEBUG_LOCKS_WARN_ON(!test_bit(class_idx, lock_classes_in_use)))
		return 0;

	chain_key = curr->curr_chain_key;
	if (!depth) {
		/*
		 * How can we have a chain hash when we ain't got no keys?!
		 */
		if (DEBUG_LOCKS_WARN_ON(chain_key != INITIAL_CHAIN_KEY))
			return 0;
		chain_head = 1;
	}

	hlock->prev_chain_key = chain_key;
	if (separate_irq_context(curr, hlock)) {
		chain_key = INITIAL_CHAIN_KEY;
		chain_head = 1;
	}
	chain_key = iterate_chain_key(chain_key, hlock_id(hlock));

	if (nest_lock && !__lock_is_held(nest_lock, -1)) {
		print_lock_nested_lock_not_held(curr, hlock, ip);
		return 0;
	}

	if (!debug_locks_silent) {
		WARN_ON_ONCE(depth && !hlock_class(hlock - 1)->key);
		WARN_ON_ONCE(!hlock_class(hlock)->key);
	}

	if (!validate_chain(curr, hlock, chain_head, chain_key))
		return 0;

	curr->curr_chain_key = chain_key;
	curr->lockdep_depth++;
	check_chain_key(curr);
#ifdef CONFIG_DEBUG_LOCKDEP
	if (unlikely(!debug_locks))
		return 0;
#endif
	if (unlikely(curr->lockdep_depth >= MAX_LOCK_DEPTH)) {
		debug_locks_off();
		print_lockdep_off("BUG: MAX_LOCK_DEPTH too low!");
		printk(KERN_DEBUG "depth: %i  max: %lu!\n",
		       curr->lockdep_depth, MAX_LOCK_DEPTH);

		lockdep_print_held_locks(current);
		debug_show_all_locks();
		dump_stack();

		return 0;
	}

	if (unlikely(curr->lockdep_depth > max_lockdep_depth))
		max_lockdep_depth = curr->lockdep_depth;

	return 1;
}

static void print_unlock_imbalance_bug(struct task_struct *curr,
				       struct lockdep_map *lock,
				       unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================\n");
	pr_warn("WARNING: bad unlock balance detected!\n");
	print_kernel_ident();
	pr_warn("-------------------------------------\n");
	pr_warn("%s/%d is trying to release lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no more locks to release!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static noinstr int match_held_lock(const struct held_lock *hlock,
				   const struct lockdep_map *lock)
{
	if (hlock->instance == lock)
		return 1;

	if (hlock->references) {
		const struct lock_class *class = lock->class_cache[0];

		if (!class)
			class = look_up_lock_class(lock, 0);

		/*
		 * If look_up_lock_class() failed to find a class, we're trying
		 * to test if we hold a lock that has never yet been acquired.
		 * Clearly if the lock hasn't been acquired _ever_, we're not
		 * holding it either, so report failure.
		 */
		if (!class)
			return 0;

		/*
		 * References, but not a lock we're actually ref-counting?
		 * State got messed up, follow the sites that change ->references
		 * and try to make sense of it.
		 */
		if (DEBUG_LOCKS_WARN_ON(!hlock->nest_lock))
			return 0;

		if (hlock->class_idx == class - lock_classes)
			return 1;
	}

	return 0;
}

/* @depth must not be zero */
static struct held_lock *find_held_lock(struct task_struct *curr,
					struct lockdep_map *lock,
					unsigned int depth, int *idx)
{
	struct held_lock *ret, *hlock, *prev_hlock;
	int i;

	i = depth - 1;
	hlock = curr->held_locks + i;
	ret = hlock;
	if (match_held_lock(hlock, lock))
		goto out;

	ret = NULL;
	for (i--, prev_hlock = hlock--;
	     i >= 0;
	     i--, prev_hlock = hlock--) {
		/*
		 * We must not cross into another context:
		 */
		if (prev_hlock->irq_context != hlock->irq_context) {
			ret = NULL;
			break;
		}
		if (match_held_lock(hlock, lock)) {
			ret = hlock;
			break;
		}
	}

out:
	*idx = i;
	return ret;
}

static int reacquire_held_locks(struct task_struct *curr, unsigned int depth,
				int idx, unsigned int *merged)
{
	struct held_lock *hlock;
	int first_idx = idx;

	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return 0;

	for (hlock = curr->held_locks + idx; idx < depth; idx++, hlock++) {
		switch (__lock_acquire(hlock->instance,
				    hlock_class(hlock)->subclass,
				    hlock->trylock,
				    hlock->read, hlock->check,
				    hlock->hardirqs_off,
				    hlock->nest_lock, hlock->acquire_ip,
				    hlock->references, hlock->pin_count)) {
		case 0:
			return 1;
		case 1:
			break;
		case 2:
			*merged += (idx == first_idx);
			break;
		default:
			WARN_ON(1);
			return 0;
		}
	}
	return 0;
}

static int
__lock_set_class(struct lockdep_map *lock, const char *name,
		 struct lock_class_key *key, unsigned int subclass,
		 unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	struct lock_class *class;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	lockdep_init_map_waits(lock, name, key, 0,
			       lock->wait_type_inner,
			       lock->wait_type_outer);
	class = register_lock_class(lock, subclass, 0);
	hlock->class_idx = class - lock_classes;

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged))
		return 0;
	return 1;
}

static int __lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	WARN(hlock->read, "downgrading a read lock");
	hlock->read = 1;
	hlock->acquire_ip = ip;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/* Merging can't happen with unchanged classes.. */
	if (DEBUG_LOCKS_WARN_ON(merged))
		return 0;

	�    ��Í�&    ��@  ��Í�&    ���   ��Í�&    ��   �I�����    �    �9�����    �    �)�����    �����WVS��xT�Ɓ�   �׉Ӄ� �NщӉN��[^_Ð�   �   �� ��������F��t��F��[^_Í�&    ��������[^_û����뷍�&    ��&    �����UW��V��S�ø    ��������C(�$����  ��t	9{��  �C,��"��   ��    �D$����   �<$���   ��4  �n�D$���L$C9h��   �D�9�u�!  f�� 9��  ;Xu���C�$�D$1�������ƋD$8  �D$��u��   ��    �$��������ƅ���   ��T  �������u݉�1Ʌ�t5�D$�D�u*�mh��u��������    ������؃�[^_]Í�&    �D$�| �D$�D�9�u����&    �v 9�t ��� 9�t��x �Pu��t�z t�9�u��|$�C�I�����  �B�$��������ƅ��E����D$1�������f���������\���������R���������H����v �����U��WVS�����  d�    �T$1��D$    ����  �����������{d�    ��p  �t$�t$��p  �[��t:f��|$>�t& �K$�S0��������ƅ�u�|$ ���w�D$�����_����uȋ|$d�    ��p  �����������D$�T$d+    un��[^_]Ít& �������S0�s$�D$��������[��������    ��$��C��S ������L$1�)�CS�C�$   �M�����G����������&    �v �����U��WVS��d�    �D$���  �D$    ����  �����������{d�    ��p  �t$�t$��p  �[��t=�t& ��|$>�t& �K$�S0��������ƅ�u�|$ ���w�D$�����_����uȋ|$d�    ��p  �����������D$�T$d+    un��[^_]Ít& �������S0�s$�D$��������[��������    ��$��C��S ������L$1�)�CS�C�$   �M�����G����������&    �v �����UWV1�S�ÉЉʁ�   d�    ��$�   1ɋ�$�   �D$    �D$     ��$�   �L$$�K�\$����D$f����   ���.  ���  �D$    ����  �����������{d�    ��p  �T$�T$��p  �s��tB�v �|$>�t& �N$�V0�D$���������u�|$ ���w�D$��������	] �7��u��|$d�    ��p  �����������t$��$�   d+    ��   �Ĭ   ��[^_]�f�������V0�^$�D$�D$������v�������4�    ��$��F��V ������L$1�)�FV�F�$   �=�����7����t& ��t$(�    ���t$������������&    ��    �������Qtfw��tO�    ��Pu%���f�=�   t)=�   u��  ��Í�&    �v �    ��t��������   �č�&    f��    봍�&    f��    뤍�&    f������U1�WV���   S�É�8  �߃��t$�4�4  �T$�L$�4$�D$���t�t$�D�tD�$1ҍ4�D$�D�9�u�(��� 9�t�H��u��H��t�I��u� 9�u�t& �D �ŋh��u���  ������ƅ���   �D$1�8  �D$��~$�D$�D�u�[h��u�D$�01���[^_]�f��D$�<$�D��,;9�u'�Ӎt& �<R���L��R�x���9�x�y� 9�t��H��u؋H��t�I��t��ȸ����럍�&    f������UW�׉�V��1�S�������t=�Ít& ��T  �������u��P  ���������Ņ���   �؉�������Å�uɉ�1����   ������Å�u�Vf���P  ��u,�؉�������Å�t;��T  �������u؋�P  �D��T�������؉�ǃP      ������Å�u�1�[��^_]Ív ��1�������Å�t琋�P  ������؉�ǃP      ������Å�u�[��^_]Ít& ������UWV�Ɖ�S���T$(�$��"��   ����    ����   ���   ��8  �|$���|$�|>�T$�T���t��ud��uo�T$������D�9�tS�\��� �����   �C�C    ���C    �D$���P����Ņ�tz�D$�C�$�C����[^_]Ív ���������[^_]Ð�$�������	�t�\$�\�9�u��t& �9�t;Cu�;S�r����9�u�������[��^_]Í�&    �C��A����   �C"  ������D$�D�9�t&�D$��t������D$������������[^_]Ð�D$�D�    �̉��@�����&    �t& �����VS�ø    ������C(��tV�s,1҉��J���Z��uU�s(�C(    �    ������F,t[^Í�&    �������F�u,d�[^�������&    �v [�    ^������t& 맍t& �F��(u΋V�F�R�����뾍�&    ��&    �������P(��t�?�����&    Í�&    ������P(��t����1�Í�&    �t& ��������k��   w��9t[�    ��`u!��Í�    ��lt3��zuV�`   ��Í�    ��1u[�z�    �    E��Ǎ�&    ��    븍�&    f��z�    �    E�뜍t& �=�   tQ=�   u2��  낍v ��Qt[w��tD�    ��P�e����    ���W���������v �    �E�����    �   �5�����    �    �%�����    �    ������    �����U�    W��VS���D$�@�D$�    �������������-�  �D$��&    ��39�u'�   �v �B(    �؉��������������9�tc�ƋC�F�0�C�   �C"  ��t������S��t��J(�A,u��L$�$������L$�$�A���   d��$������$�f��t$���������������D$���   �J����D$�|$�@��X�q�9D$t���������؉�������F�p�C9�u�    ������D$�������t�������T  �utd�������[h��u�|$�G������������t.��[^_]ËA��(�4����A�I�$�I������$����������D$�������u4d���[^_]�������X  ��(u���X  ��T  �R������j����D$��������(u��L$�������������R�����륍�&    f������UWVS��H�t$\�$�L$�t$d�    �D$D�D$d�D$4    �D$8    �D$<    �D$@    ���D$���_  �D$d�����O  �ׅ�t	��A  �D$d�t$��������8��&ELF                             4     (               ������@\�����1�������V�   S�Ã�p��d�    �D$�C$j�T$������C$�t$�   j�T$
���������V�D$PSh    ��������D$d+    u��1�[^��������&    ������UW��V��S�Ã����   d�    �D$1��L$�    �T$���   j �   ��T$�����Z��������D$d+    u��[^_]������������    VS�����   ���>   ���1҃����a������1ҁ��   �P���1�[^Í�&    �t& �����UWVS�Ë@�P�R�����%   =   �S   �����[^_]Í�&    ��    �����U����W���V�4S��Ã� ���   ��������   ���   ����	ֈ��   �ψ��   ��?1҃�_	���	����   ���������   ��?�ȉ�����	�   ���ȉ��q����    ���   ����   ���1҃����K������1ҁ��   �:���1�[^_]�       P                                                                                                                                 �      P  �      6%s: Status: SA00=0x%02x SA01=0x%02x
  7%s: write reg: %02X val: %02X
    3%s: I/O error write 0x%02x/0x%02x
    7%s: changed input or channel
 6%s %d-%04x: chip found @ 0x%x (%s)
 ����p������WVShL   ���������   ����P��P�CpPh(   ���������   �@pPht   �����XZ�  �S�C��  Q� QP��   �CT�0h�   ������C��  ��   ������Ń���u
������z  �`   �ڍ��   1�������    �   ����   �ډ���   ��u�1��z  �CpPht   �����XZ�0                    �H���  ^ � �              upd64031a                                                          �       �                   �                                                                                                                                                                                                            debug ������    �    ������    �����upd64031a parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=T. Adachi, Takeru KOMORIYA, Hans Verkuil description=uPD64031A driver  P          ��      GCC: (GNU) 11.2.0           GNU  �        �                                  ��                                                                          x                  	 3   �   p     �           C       B    	 X      E     n   B       	               �   P  3     �   W   �    	 �   `         �   �  �     �   �       	               �            �       �           
                   &      0     3     0     F  �   ,     Z  �   P     n           �          �           �  P       �  0        �  <   0     �  l                                  -             ?             Y             a             r             �             �             �             �             �           �             �      
     �      0                   upd64031a.c upd64031a_remove upd64031a_log_status upd64031a_write upd64031a_write.cold upd64031a_s_frequency upd64031a_s_frequency.cold upd64031a_probe upd64031a_probe.cold upd64031a_ops upd64031a_s_routing upd64031a_s_routing.cold upd64031a_driver_init upd64031a_driver upd64031a_driver_exit upd64031a_id upd64031a_core_ops upd64031a_tuner_ops upd64031a_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev __stack_chk_guard i2c_transfer_buffer_flags _printk __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__upd64031a_id_device_table param_ops_int    $  	   %     $  &   &  8   '  P   '  b     g   (  u   &  �   )  �   $  �   &  �     �   '  �   &  �   )    $      Q  $  e  *  �  $  !    �     �         u    0                   h                                                 (  1     6   (  G     L   (  v     {   (  �   +  �     �   ,  �     �     �     �   (       >     S     �     �     �     `     d     l     �     �             $          -     .          0           -     3        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rel.text.unlikely .rel.rodata .rel.init.text .rel.exit.text .rodata.str1.1 .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   ]                    	   @       �  �               )             �  �                   %   	   @       �                  /                                  8                                  4   	   @       �  8               E      2       <  �                 X             �  �                  T   	   @       �  �      	         k                V                  g   	   @       �  0               w             V                    s   	   @       �                  �             j  
                  �   	   @       �                 �      2       t  
                 �             ~  �                  �                                 �   	   @       �                  �      0                          �              /                     �             0  (                                X  @     $         	              �
  ,                                 �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            �|$`"�  �D$`��    �D$���  ���   ��    �\$�q�L$$��t$�4$���vh݅�tz8  �l$ �D$�D$�l����uZ�D$��D$�D�9�u�>��&    ��� 9�t�X��u��X��t�[��u� 9�u�t& ���'  �(  �vh��u��l$ �4$�\$�މD$��4  9�t�D$9���  �  �D$��4  9��C  ��1���t& ���� 9�t�p��u��H��t�I��u� 9�u�t& ��?�1  �D$d�  �L$�T$�؍�&    �v ��t9xu9���   ��t	;P��   � 9�u܋T$�D$��u��   �v �9���  ;Cu�� �����  ������  �$�\$��1����   �P@�@<�D$(�D$`�T$,�D$0�C(���x  ��t1ɍT$(������D�4���w  ��tj�C(�   ���x  ��u���t& �������D$Dd+    �>  ��H��[^_]��D$d��  �t& ��D$��4  9��=���1��B�����&    �\$��tu�D$�k�{�L$�|$�C�D$4���C�D$8�C�$����  ������ƅ���   ����   ��������<$�L$`1��D$<��������L$`�D$@��������8����   �   ��  ������Å��C  �t$�L$�+��8  ��8  �C��D$�{�|$�C�D$4���C�D$8�C�$����  �U����ƅ���   �D$$����������U�����&    ����������D�4= �����   �D�<�h�����&    �D$�H�	����t& ��t[�k�D$<�C    ������D$@������P����v �������l$ ������������.����D$�����������1������D$<������D$@������C��B����   �C"  ���������������������D$<�����������D$@�������������������������������&    ������S���@,t��T  1�[������t& ������C�ud� �������T  1�[�������    �C�� �ލ�&    ��&    ������U�   ��  WV��S��dd�    �D$`1��l$��󫍆T  �$h�  �����Z�ǅ���   �^h��t"�������T  ���   d� ������[h��uލ�4  ���  �� �@��9�u�H  ��H  ��L  �����tx�L� �������������t�l$`���������9�u�^h��t�������T  �u{d�������[h��u�$�����������D$`d+    u{��d��[^_]Í�&    f�1ۍ�    �L� ����  ���   ���L��������u�1��f���X  �� ������X  ��(�x�����X  ��T  �R������_���������v �����U��W��VS�Ã�� �������= �����   �S�Ѓ���tE�    �[�$������$1ɉ�R��Sj �����ø    ��������F,th����[^_]Ít& �C��1��������= ���ws�S�    �[�$������$���R��Sj �>����ø    ���������t���������F,u�������F�uRd�������؃�[^_]Ã��É�[^_]Ít& �F,t
���]����v ������F�u-d��������