();
}

/*
 * Print out an error if an invalid bit is set:
 */
static inline int
valid_state(struct task_struct *curr, struct held_lock *this,
	    enum lock_usage_bit new_bit, enum lock_usage_bit bad_bit)
{
	if (unlikely(hlock_class(this)->usage_mask & (1 << bad_bit))) {
		graph_unlock();
		print_usage_bug(curr, this, bad_bit, new_bit);
		return 0;
	}
	return 1;
}


/*
 * print irq inversion bug:
 */
static void
print_irq_inversion_bug(struct task_struct *curr,
			struct lock_list *root, struct lock_list *other,
			struct held_lock *this, int forwards,
			const char *irqclass)
{
	struct lock_list *entry = other;
	struct lock_list *middle = NULL;
	int depth;

	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("========================================================\n");
	pr_warn("WARNING: possible irq lock inversion dependency detected\n");
	print_kernel_ident();
	pr_warn("--------------------------------------------------------\n");
	pr_warn("%s/%d just changed the state of lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(this);
	if (forwards)
		pr_warn("but this lock took another, %s-unsafe lock in the past:\n", irqclass);
	else
		pr_warn("but this lock was taken by another, %s-safe lock in the past:\n", irqclass);
	print_lock_name(other->class);
	pr_warn("\n\nand interrupts could create inverse lock ordering between them.\n\n");

	pr_warn("\nother info that might help us debug this:\n");

	/* Find a middle lock (if one exists) */
	depth = get_lock_depth(other);
	do {
		if (depth == 0 && (entry != root)) {
			pr_warn("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}
		middle = entry;
		entry = get_lock_parent(entry);
		depth--;
	} while (entry && entry != root && (depth >= 0));
	if (forwards)
		print_irq_lock_scenario(root, other,
			middle ? middle->class : root->class, other->class);
	else
		print_irq_lock_scenario(other, root,
			middle ? middle->class : other->class, root->class);

	lockdep_print_held_locks(curr);

	pr_warn("\nthe shortest dependencies between 2nd lock and 1st lock:\n");
	root->trace = save_trace();
	if (!root->trace)
		return;
	print_shortest_lock_dependencies(other, root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Prove that in the forwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_forwards(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_root(&root, this);
	ret = find_usage_forwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(read_bit));
	}

	return 0;
}

/*
 * Prove that in the backwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_backwards(struct task_struct *curr, struct held_lock *this,
		      enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_rootb(&root, this);
	ret = find_usage_backwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(read_bit));
	}

	return 0;
}

void print_irqtrace_events(struct task_struct *curr)
{
	const struct irqtrace_events *trace = &curr->irqtrace;

	printk("irq event stamp: %u\n", trace->irq_events);
	printk("hardirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->hardirq_enable_event, (void *)trace->hardirq_enable_ip,
		(void *)trace->hardirq_enable_ip);
	printk("hardirqs last disabled at (%u): [<%px>] %pS\n",
		trace->hardirq_disable_event, (void *)trace->hardirq_disable_ip,
		(void *)trace->hardirq_disable_ip);
	printk("softirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->softirq_enable_event, (void *)trace->softirq_enable_ip,
		(void *)trace->softirq_enable_ip);
	printk("softirqs last disabled at (%u): [<%px>] %pS\n",
		trace->softirq_disable_event, (void *)trace->softirq_disable_ip,
		(void *)trace->softirq_disable_ip);
}

static int HARDIRQ_verbose(struct lock_class *class)
{
#if HARDIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int SOFTIRQ_verbose(struct lock_class *class)
{
#if SOFTIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int (*state_verbose_f[])(struct lock_class *class) = {
#define LOCKDEP_STATE(__STATE) \
	__STATE##_verbose,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline int state_verbose(enum lock_usage_bit bit,
				struct lock_class *class)
{
	return state_verbose_f[bit >> LOCK_USAGE_DIR_MASK](class);
}

typedef int (*check_usage_f)(struct task_struct *, struct held_lock *,
			     enum lock_usage_bit bit, const char *name);

static int
mark_lock_irq(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit new_bit)
{
	int excl_bit = exclusive_bit(new_bit);
	int read = new_bit & LOCK_USAGE_READ_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * Validate that this particular lock does not have conflicting
	 * usage states.
	 */
	if (!valid_state(curr, this, new_bit, excl_bit))
		return 0;

	/*
	 * Check for read in write conflicts
	 */
	if (!read && !valid_state(curr, this, new_bit,
				  excl_bit + LOCK_USAGE_READ_MASK))
		return 0;


	/*
	 * Validate that the lock dependencies don't have conflicting usage
	 * states.
	 */
	if (dir) {
		/*
		 * mark ENABLED has to look backwards -- to ensure no dependee
		 * has USED_IN state, which, again, would allow  recursion deadlocks.
		 */
		if (!check_usage_backwards(curr, this, excl_bit))
			return 0;
	} else {
		/*
		 * mark USED_IN has to look forwards -- to ensure no dependency
		 * has ENABLED state, which would allow recursion deadlocks.
		 */
		if (!check_usage_forwards(curr, this, excl_bit))
			return 0;
	}

	if (state_verbose(new_bit, hlock_class(this)))
		return 2;

	return 1;
}

/*
 * Mark all held locks with a usage bit:
 */
static int
mark_held_locks(struct task_struct *curr, enum lock_usage_bit base_bit)
{
	struct held_lock *hlock;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		enum lock_usage_bit hlock_bit = base_bit;
		hlock = curr->held_locks + i;

		if (hlock->read)
			hlock_bit += LOCK_USAGE_READ_MASK;

		BUG_ON(hlock_bit >= LOCK_USAGE_STATES);

		if (!hlock->check)
			continue;

		if (!mark_lock(curr, hlock, hlock_bit))
			return 0;
	}

	return 1;
}

/*
 * Hardirqs will be enabled:
 */
static void __trace_hardirqs_on_caller(void)
{
	struct task_struct *curr = current;

	/*
	 * We are going to turn hardirqs on, so set the
	 * usage bit for all held locks:
	 */
	if (!mark_held_locks(curr, LOCK_ENABLED_HARDIRQ))
		return;
	/*
	 * If we have softirqs enabled, then set the usage
	 * bit for all held locks. (disabled hardirqs prevented
	 * this bit from being set before)
	 */
	if (curr->softirqs_enabled)
		mark_held_locks(curr, LOCK_ENABLED_SOFTIRQ);
}

/**
 * lockdep_hardirqs_on_prepare - Prepare for enabling interrupts
 * @ip:		Caller address
 *
 * Invoked before a possible transition to RCU idle from exit to user or
 * guest mode. This ensures that all RCU operations are done before RCU
 * stops watching. After the RCU transition lockdep_hardirqs_on() has to be
 * invoked to set the final state.
 */
void lockdep_hardirqs_on_prepare(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs do not (and cannot) track lock dependencies, nothing to do.
	 */
	if (unlikely(in_nmi()))
		return;

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (unlikely(lockdep_hardirqs_enabled())) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * See the fine text that goes along with this variable definition.
	 */
	if (DEBUG_LOCKS_WARN_ON(early_boot_irqs_disabled))
		return;

	/*
	 * Can't allow enabling interrupts while in an interrupt handler,
	 * that's general bad form and such. Recursion, limited stack etc..
	 */
	if (DEBUG_LOCKS_WARN_ON(lockdep_hardirq_context()))
		return;

	current->hardirq_chain_key = current->curr_chain_key;

	lockdep_recursion_inc();
	__trace_hardirqs_on_caller();
	lockdep_recursion_finish();
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on_prepare);

void noinstr lockdep_hardirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs can happen in the middle of local_irq_{en,dis}able() where the
	 * tracking state and hardware state are out of sync.
	 *
	 * NMIs must save lockdep_hardirqs_enabled() to restore IRQ state from,
	 * and not rely on hardware state like normal interrupts.
	 */
	if (unlikely(in_nmi())) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;

		/*
		 * Skip:
		 *  - recursion check, because NMI can hit lockdep;
		 *  - hardware state check, because above;
		 *  - chain_key check, see lockdep_hardirqs_on_prepare().
		 */
		goto skip_checks;
	}

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (lockdep_hardirqs_enabled()) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * Ensure the lock stack remained unchanged between
	 * lockdep_hardirqs_on_prepare() and lockdep_hardirqs_on().
	 */
	DEBUG_LOCKS_WARN_ON(current->hardirq_chain_key !=
			    current->curr_chain_key);

skip_checks:
	/* we'll do an OFF -> ON transition: */
	__this_cpu_write(hardirqs_enabled, 1);
	trace->hardirq_enable_ip = ip;
	trace->hardirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(hardirqs_on_events);
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on);

/*
 * Hardirqs were disabled:
 */
void noinstr lockdep_hardirqs_off(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * Matching lockdep_hardirqs_on(), allow NMIs in the middle of lockdep;
	 * they will restore the software state. This ensures the software
	 * state is consistent inside NMIs as well.
	 */
	if (in_nmi()) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;
	} else if (__this_cpu_read(lockdep_recursion))
		return;

	/*
	 * So we're supposed to get called after you mask local IRQs, but for
	 * some reason the hardware doesn't quite think you did a proper job.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (lockdep_hardirqs_enabled()) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		__this_cpu_write(hardirqs_enabled, 0);
		trace->hardirq_disable_ip = ip;
		trace->hardirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(hardirqs_off_events);
	} else {
		debug_atomic_inc(redundant_hardirqs_off);
	}
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_off);

/*
 * Softirqs will be enabled:
 */
void lockdep_softirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c, avoids
	 * funny state and nesting things.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		debug_atomic_inc(redundant_softirqs_on);
		return;
	}

	lockdep_recursion_inc();
	/*
	 * We'll do an OFF -> ON transition:
	 */
	current->softirqs_enabled = 1;
	trace->softirq_enable_ip = ip;
	trace->softirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(softirqs_on_events);
	/*
	 * We are going to turn softirqs on, so set the
	 * usage bit for all held locks, if hardirqs are
	 * enabled too:
	 */
	if (lockdep_hardirqs_enabled())
		mark_held_locks(current, LOCK_ENABLED_SOFTIRQ);
	lockdep_recursion_finish();
}

/*
 * Softirqs were disabled:
 */
void lockdep_softirqs_off(unsigned long ip)
{
	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		current->softirqs_enabled = 0;
		trace->softirq_disable_ip = ip;
		trace->softirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(softirqs_off_events);
		/*
		 * Whoops, we wanted softirqs off, so why aren't they?
		 */
		DEBUG_LOCKS_WARN_ON(!softirq_count());
	} else
		debug_atomic_inc(redundant_softirqs_off);
}

static int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	if (!check)
		goto lock_used;

	/*
	 * If non-trylock use in a hardirq or softirq context, then
	 * mark the lock as used in these contexts:
	 */
	if (!hlock->trylock) {
		if (hlock->read) {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_HARDIRQ_READ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_SOFTIRQ_READ))
					return 0;
		} else {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock, LOCK_USED_IN_HARDIRQ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock, LOCK_USED_IN_SOFTIRQ))
					return 0;
		}
	}
	if (!hlock->hardirqs_off) {
		if (hlock->read) {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ_READ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ_READ))
					return 0;
		} else {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ))
					return 0;
		}
	}

lock_used:
	/* mark it as used: */
	if (!mark_lock(curr, hlock, LOCK_USED))
		return 0;

	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return LOCK_CHAIN_HARDIRQ_CONTEXT * !!lockdep_hardirq_context() +
	       LOCK_CHAIN_SOFTIRQ_CONTEXT * !!task->softirq_context;
}

static int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	unsigned int depth = curr->lockdep_depth;

	/*
	 * Keep track of points where we cross into an interrupt context:
	 */
	if (depth) {
		struct held_lock *prev_hlock;

		prev_hlock = curr->held_locks + depth-1;
		/*
		 * If we cross into another context, reset the
		 * hash key (this also prevents the checking and the
		 * adding of the dependency to 'prev'):
		 */
		if (prev_hlock->irq_context != hlock->irq_context)
			return 1;
	}
	return 0;
}

/*
 * Mark a lock with a usage bit, and validate the state transition:
 */
static int mark_lock(struct task_struct *curr, struct held_lock *this,
			     enum lock_usage_bit new_bit)
{
	unsigned int new_mask, ret = 1;

	if (new_bit >= LOCK_USAGE_STATES) {
		DEBUG_LOCKS_WARN_ON(1);
		return 0;
	}

	if (new_bit == LOCK_USED && this->read)
		new_bit = LOCK_USED_READ;

	new_mask = 1 << new_bit;

	/*
	 * If already set then do not dirty the cacheline,
	 * nor do any checks:
	 */
	if (likely(hlock_class(this)->usage_mask & new_mask))
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
	if (DEBUG_LOCKS_WARN_ON(depth >= MAX/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ctype.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
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
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
    $(wildcard include/config/HYPERV) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MSI) \
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
  include/linux/filter.h \
    $(wildcard include/config/BPF_JIT_ALWAYS_ON) \
    $(wildcard include/config/HAVE_EBPF_JIT) \
    $(wildcard include/config/IPV6) \
  include/linux/bpf.h \
  include/uapi/linux/bpf.h \
    $(wildcard include/config/BPF_LIRC_MODE2) \
    $(wildcard include/config/EFFICIENT_UNALIGNED_ACCESS) \
    $(wildcard include/config/IP_ROUTE_CLASSID) \
    $(wildcard include/config/BPF_KPROBE_OVERRIDE) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
    $(wildcard include/config/XFRM) \
  include/uapi/linux/bpf_common.h \
  include/linux/file.h \
  include/linux/rbtree_latch.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgELF                      °      4     (               èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[ÃèüÿÿÿS‹P(‹Xú	˜ tú	˜ tT¹êÿÿÿú 	˜ t'‰È[Ãv ¶H|‹CÔº   èüÿÿÿ1É[‰ÈÃ´&    f¶H|‹CÔº   èüÿÿÿ1É[‰ÈÃ´&    f¶H|‹CÔº   èüÿÿÿ1É[‰ÈÃ´&    fèüÿÿÿ‹€”   º   ƒÉ@¶Éèüÿÿÿ1ÀÃ´&    ´&    èüÿÿÿUWVS‰Ã‹@‹P‹Rèüÿÿÿ%   =   „7   ¾ûÿÿÿ[‰ğ^_]Ã´&    t& èüÿÿÿUöÆù‰ÍW‰×º   V‰Æ¸    S»   DØƒÃë´&    ¶SƒÃ„Òt¶‹†”   èüÿÿÿ…Àyã[‰¾0  1À‰®4  ^_]Ã    à                                                                                                                                  °       à          6%s: Standard: %d Hz
 tw9906 èüÿÿÿS‰Ã‹€0  % ù  ƒøÀƒàöƒÀ<PCpPh    èüÿÿÿ‰Øèüÿÿÿ1ÀƒÄ[Ã‹S·CŠ  Q QPÿ²   ‹CTÿ0h    èüÿÿÿC¹À  º8  èüÿÿÿ‰ÇƒÄ…Àu
¾ôÿÿÿé
  ¹`   ‰Ú¯À   èüÿÿÿ1Éº   ‰èj èüÿÿÿ¹ 	˜ º  ‰èj j j jj jjÿj€èüÿÿÿ¹	˜ º  ‰èƒÄ$j j`j jj hÿ   j j èüÿÿÿ¹	˜ º  ‰èƒÄ j j j jj jjÿj€èüÿÿÿ‹·ü   ‰olƒÄ …öt‰èèüÿÿÿé
  Ç‡0   °  ½   º   Ç‡4      ¶M‹‡”   èüÿÿÿ…ÀyƒÃ¾êÿÿÿSh(   èüÿÿÿXZé
  ¶UƒÅ„ÒuÈé
   6%s %d-%04x: chip found @ 0x%02x (%s)
 3%s: error initializing TW9906
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                 	     	ğ            tw9906                                                          à           €                   °                                                                                                                                                              @¢	ğ
Ğ   `~~ W@)U k&l6mğnAo­p  license=GPL v2 description=TW9906 I2C subdev driver  GCC: (GNU) 11.2.0             GNU  À       À                                  ñÿ                            
                   ‡     &   °   !                                	 =       ;    	               O   à   5     \   ;   R   	 n   `         y          ‰      :     ›      `     ¨       
     ¶      
                   Ä            ×       €     å       
                   ø       0       à   0       €   P     #           :     %     U             `             ~             •             ¯             ·             Ó             ì             ù                          +             =             K             _           k             z      
     ‰      0      tw9906.c tw9906_remove tw9906_s_ctrl tw9906_s_video_routing tw9906_log_status tw9906_probe tw9906_probe.cold tw9906_ops tw9906_ctrl_ops initial_registers tw9906_s_std config_50hz.2 config_60hz.1 tw9906_driver_init tw9906_driver tw9906_driver_exit tw9906_id tw9906_core_ops tw9906_video_ops __UNIQUE_ID_license267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free i2c_smbus_write_byte_data _printk v4l2_ctrl_subdev_log_status __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw9906_id_device_table                 !     ]      }            ±     Ê      á     õ   #  !    7    =    e                        h                             