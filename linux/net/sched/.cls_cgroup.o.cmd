_name(read_bit));
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

	º    ‰ĞÃ´&    º@  ‰ĞÃ´&    ºÀ   ‰ĞÃ´&    º   éIÿÿÿ¶    º    é9ÿÿÿ¶    º    é)ÿÿÿ¶    èüÿÿÿWVS…ÒxT‰Æú   ‰×‰Óƒú ‰NÑ‰Ó‰N‰Ø[^_Ã»   ¿   ºÀ ‰øèüÿÿÿ‰F…Àtø‰F‰Ø[^_Ã´&    »êÿÿÿ‰Ø[^_Ã»ôÿÿÿë·´&    ´&    èüÿÿÿUW‰ÏV‰ÖS‰Ã¸    ƒìèüÿÿÿ‹C(‰$…À„™  …ÿt	9{…‚  ‹C,ƒø"‡Í   ¾€    ‰D$…Àˆº   ‹<$ˆ„   Å4  ‹n‰D$‰ú‰L$Â‹C9h…   ‹DÏ9Âué!  f‹ 9Â„  ;Xuñ‰ğ‡C‹$‰D$1Àèüÿÿÿ‰Æ‹D$8  ‰D$…öuéÕ   ¶    ‹$‰ğèüÿÿÿ‰Æ…À„»   †T  èüÿÿÿ„Àuİ‰õ1É…Ét5‹D$öD…u*‹mh…íuê»êÿÿÿ¸    èüÿÿÿ‰ØƒÄ[^_]Ã´&    ‹D$| ‹D$‹DÅ9ÇuëÀ´&    v 9Ót ƒÁ‹ 9Çt©ƒx ‹Puê…Òtíƒz tç9Óuà‹|$‹CIÁâ”¾Ø  ‰B‹$‰ğèüÿÿÿ‰Æ…À…Eÿÿÿ‹D$1Ûèüÿÿÿéfÿÿÿ»şÿÿÿé\ÿÿÿ»ÿÿÿÿéRÿÿÿ»½ÿÿÿéHÿÿÿv èüÿÿÿU‰ÕWVSƒì‹€ä  d‹    ‰T$1ÒÇD$    ‹œˆØ  èüÿÿÿèüÿÿÿ{d¡    ‹°p  ‰t$t$‰°p  ‹[…Ût:f‰|$>t& ‹K$S0‰èèüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÈ‹|$d¡    ‰¸p  èüÿÿÿèüÿÿÿ‹D$‹T$d+    unƒÄ[^_]Ãt& èüÿÿÿS0‹s$‰D$‰èèüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ‹L$1Ò)ÈCSƒC÷$   „MÿÿÿûéGÿÿÿèüÿÿÿ´&    v èüÿÿÿU‰ÅWVSƒìd¡    ‰D$‹…ä  ÇD$    ‹œØ  èüÿÿÿèüÿÿÿ{d¡    ‹°p  ‰t$t$‰°p  ‹[…Ût=t& ‰|$>t& ‹K$S0‰èèüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÈ‹|$d¡    ‰¸p  èüÿÿÿèüÿÿÿ‹D$‹T$d+    unƒÄ[^_]Ãt& èüÿÿÿS0‹s$‰D$‰èèüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ‹L$1Ò)ÈCSƒC÷$   „MÿÿÿûéGÿÿÿèüÿÿÿ´&    v èüÿÿÿUWV1öS‰Ã‰Ğ‰Êì¬   d‹    ‰Œ$¨   1É‹Œ$À   ÇD$    ÇD$     ‹¬$Ä   ‰L$$·K‰\$ƒá÷‰D$fƒù…«   …À„.  ‹ƒä  ÇD$    ‹œØ  èüÿÿÿèüÿÿÿ{d¡    ‹p  ‰T$T$‰p  ‹s…ötBv ‰|$>t& ‹N$V0D$èüÿÿÿ‰ÃöÃu|$ ğÿÿwÇD$ÿÿÿÿƒÇÑë	] ‹7…öuÁ‹|$d¡    ‰¸p  èüÿÿÿèüÿÿÿ‹t$‹„$¨   d+    …   Ä¬   ‰ğ[^_]ÃfèüÿÿÿV0‹^$‰D$D$èüÿÿÿ‹v‰Ãèüÿÿÿ4…    œ$úƒFƒƒV èüÿÿÿ‹L$1Ò)ÈFVƒF÷$   „=ÿÿÿûé7ÿÿÿt& t$(¹    ‰÷‰t$ó«é¼şÿÿèüÿÿÿ´&    ¶    èüÿÿÿƒøQtfwƒøtOº    ƒøPu%‰ĞÃf=º   t)=»   uºà  ‰ĞÃ´&    v º    ƒøtÑéüÿÿÿº   ëÄ´&    fº    ë´´&    fº    ë¤´&    fèüÿÿÿU1íWV²„   S‰Ã‰Ğ8  ‰ßƒì‰t$4Õ4  ‰T$‰L$‰4$‰D$‰è…ít‹t$öD·tD‹$1Ò4‹D$‹DÇ9Æuë(ƒÂ‹ 9Æt‹H…Éuğ‹H…Étì‹I…Éuâ‹ 9Æuåt& D ‰Å‹h…ÿu¤ºÀ  èüÿÿÿ‰Æ…À„‡   ‹D$1Ò8  ‰D$…Ò~$‹D$öDƒu‹[h…Ûuê‹D$‰01ÀƒÄ[^_]Ãf‹D$‹<$‹DÃ,;9Åu'ëÓt& <RƒÂ‰L¾R‹x‰9‹x‰y‹ 9èt®‹H…ÉuØ‹H…Étì‹I…ÉtåëÈ¸ôÿÿÿëŸ´&    fèüÿÿÿUW‰×‰ÂV‰Æ1ÀSèüÿÿÿ…Àt=‰Ãt& ƒT  èüÿÿÿ„Àu‹P  ‰ú‰Øè‚şÿÿ‰Å…À…ˆ   ‰Ø‰òèüÿÿÿ‰Ã…ÀuÉ‰ò1ÀÇô   èüÿÿÿ‰Ã…ÀuëVf‹ƒP  …Àu,‰Ø‰òèüÿÿÿ‰Ã…Àt;ƒT  èüÿÿÿ„ÀuØ‹“P  ‹D»‰T»èüÿÿÿ‰Ø‰òÇƒP      èüÿÿÿ‰Ã…ÀuÅ1í[‰è^_]Ãv ‰ò1Àèüÿÿÿ‰Ã…Àtç‹ƒP  èüÿÿÿ‰Ø‰òÇƒP      èüÿÿÿ‰Ã…ÀuÜ[‰è^_]Ãt& èüÿÿÿUWV‰Æ‰ĞSƒì‹T$(‰$ƒú"‡    ‰Ë¾Š    …Éˆ   ¹„   ‘8  ‰|$Áç‰|$|>‰T$‹T–…Àt…Ûudƒâuo‹T$½şÿÿÿ‹DÖ9ÇtS‹\Öû ğÿÿ‡ü   ‹CÇC    ‰ÊÇC    ‰D$‰ğèPşÿÿ‰Å…Àtz‹D$‰C‹$‰CƒÄ‰è[^_]Ãv ½êÿÿÿƒÄ‰è[^_]Ã‹$½êÿÿÿ‰Ó	Ãtç‹\$‹\Ş9ßuët& ‹9ßt;Cuõ;S„rÿÿÿ‹9ßuìƒÄ½şÿÿÿ[‰è^_]Ã´&    ‹C‹‰A‰‰ØÇ   ÇC"  èüÿÿÿ‹D$‹DÆ9Çt&‹D$…Àtèüÿÿÿ‹D$àûÿÿèüÿÿÿ‰èƒÄ[^_]Ã‹D$ÇD†    ëÌ‰İé@ÿÿÿ´&    t& èüÿÿÿVS‰Ã¸    èüÿÿÿ‹C(…ÀtVÿs,1Ò‰ÙèJşÿÿZ…ÀuU‹s(ÇC(    ¸    èüÿÿÿöF,t[^Ã´&    èüÿÿÿ‹F¨u,dÿ[^éüÿÿÿ´&    v [¸    ^éüÿÿÿt& ë§t& ‹Fğƒ(uÎ‹VF‹Rèüÿÿÿë¾´&    ´&    èüÿÿÿ‹P(…Òté?ÿÿÿ´&    Ã´&    èüÿÿÿ‹P(…Òtèÿÿÿ1ÀÃ´&    t& èüÿÿÿƒøk„Â   wƒø9t[¹    ƒø`u!‰ÈÃ¶    ƒølt3ƒøzuV¹`   ‰ÈÃ¶    ƒø1u[ƒz¹    ¸    EÈëÇ´&    ¹    ë¸´&    fƒz¹    ¸    EÈëœt& =º   tQ=»   u2¹à  ë‚v ƒøQt[wáƒøtD¹    ƒøP„eÿÿÿ¹    ƒø„Wÿÿÿéüÿÿÿv ¹    éEÿÿÿ¶    ¹   é5ÿÿÿ¶    ¹    é%ÿÿÿ¶    ¹    éÿÿÿ¶    èüÿÿÿU½    W‰ÇVSƒì‰D$@ì‰D$¸    èüÿÿÿ‰ø¿Øşÿÿ-„  ‰D$´&    ‹‹39ûu'éƒ   v ÇB(    ‰Ø‰óèüÿÿÿ‰èèüÿÿÿ‹9ştc‰Æ‹C‰F‰0‹CÇ   ÇC"  …Àtèüÿÿÿ‹S…ÒtÀ‹J(öA,u°‰L$‰$èüÿÿÿ‹L$‹$‹A¨…Â   dÿ‰$èüÿÿÿ‹$ë‚f‹t$ƒÅƒÇ‹èüÿÿÿ‰ğƒÀ‰D$ı¸   …Jÿÿÿ‹D$‹|$‹@ì‹Xäqä9D$t‰Øèüÿÿÿ‰Ø‰óèüÿÿÿ‹FpäC9øuã¸    èüÿÿÿ‹D$‹˜ûÿÿ…Ûtèüÿÿÿ‹ƒT  ¨utdÿèüÿÿÿ‹[h…Ûuâ‹|$Gøèüÿÿÿö‡Ğúÿÿt.ƒÄ[^_]Ã‹Ağƒ(…4ÿÿÿA‹I‰$‹Ièüÿÿÿ‹$éÿÿÿèüÿÿÿ‹D$‹€¬úÿÿ¨u4dÿƒÄ[^_]éüÿÿÿ‹ƒX  ğƒ(uƒ‹“X  ƒT  ‹Rèüÿÿÿéjÿÿÿ‹D$‹€°úÿÿğƒ(u¿‹L$‹‘°úÿÿ¬úÿÿ‹Rèüÿÿÿë¥´&    fèüÿÿÿUWVSƒìH‹t$\‰$‰L$‰t$d¡    ‰D$D‹D$dÇD$4    ÇD$8    ÇD$<    ÇD$@    ƒà‰D$ƒø„_  ‹D$dƒàƒø„O  ‰×…öt	Ñ…A  ‹D$d‹t$Áèƒà…ö•Â8Ğ…&ELF                             4     (               èüÿÿÿ‹@\èüÿÿÿ1ÀÃèüÿÿÿV¹   S‰ÃƒÃpƒìd¡    ‰D$‹C$jT$èüÿÿÿ‹C$¶t$¹   jT$
èüÿÿÿ‰ğ¶ğV¶D$PSh    èüÿÿÿƒÄ‹D$d+    uƒÄ1À[^Ãèüÿÿÿ´&    èüÿÿÿUW‰ÏV‰ÖS‰Ãƒì‹«”   d¡    ‰D$1ÀˆL$‹    ˆT$…É   j ¹   ‰èT$èüÿÿÿZƒø…üÿÿÿ‹D$d+    uƒÄ[^_]Ãèüÿÿÿèüÿÿÿ‹    VS‰Ã¶°À   …É>   ‰ñ‰Ø1ÒƒÉ¶Éèaÿÿÿ‰ñ‰Ø1Òáï   èPÿÿÿ1À[^Ã´&    t& èüÿÿÿUWVS‰Ã‹@‹P‹Rèüÿÿÿ%   =   „S   ¸ûÿÿÿ[^_]Ã´&    ¶    èüÿÿÿU‰ÕÁåWƒåÀV4S‰é‰Ãƒæ ˆ‹Ñ   ‰ñ‰ĞÁâˆ‹Ò   ¶‹À   Áàƒâ€	ÖˆƒĞ   ‰Ïˆ“Ó   ƒá?1Òƒç_	Á‰Ø	÷¶³È   ¶Éè˜şÿÿ‰øº   ƒæ?¶È‰Øè„şÿÿ	îº   ‰ğ¶È‰Øèqşÿÿ‹    ¶³À   …Éà   ‰ñ‰Ø1ÒƒÉ¶ÉèKşÿÿ‰ñ‰Ø1Òáï   è:şÿÿ1À[^_]Ã       P                                                                                                                                       P        6%s: Status: SA00=0x%02x SA01=0x%02x
  7%s: write reg: %02X val: %02X
    3%s: I/O error write 0x%02x/0x%02x
    7%s: changed input or channel
 6%s %d-%04x: chip found @ 0x%x (%s)
 ‰øƒÃp¶ø‰ğ¶ğWVShL   èüÿÿÿƒÄéâ   ‰ø¶ÀP¶ÂPCpPh(   èüÿÿÿƒÄéÆ   @pPht   èüÿÿÿXZé  ‹S·CŠ  Q QPÿ²   ‹CTÿ0h”   èüÿÿÿC¹À  ºÔ   èüÿÿÿ‰ÅƒÄ…Àu
¸ôÿÿÿéz  ¹`   ‰Ú½À   1Ûèüÿÿÿ¾    ¹   ó¥¶ŒÀ   ‰Ú‰èƒÃèŒ   ƒûuç1Àéz  CpPht   èüÿÿÿXZé0                    ¸HÒæ¯  ^ Ğ €              upd64031a                                                          à       €                                                                                                                                                                                                                               debug èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿupd64031a parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=T. Adachi, Takeru KOMORIYA, Hans Verkuil description=uPD64031A driver  P          ¤ÿ      GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ                                                                          x                  	 3      p     ¤           C       B    	 X      E     n   B       	               ‰   P  3     ™   W       	 ®   `         ¼     Í     Ğ   ä       	               é            ÿ       €           
                   &      0     3     0     F  à   ,     Z  €   P     n           ƒ          œ           ª  P       ¼  0        Ó  <   0     é  l                                  -             ?             Y             a             r             ‹             ˜             ­             »             Ï           Û             ê      
     ù      0                   upd64031a.c upd64031a_remove upd64031a_log_status upd64031a_write upd64031a_write.cold upd64031a_s_frequency upd64031a_s_frequency.cold upd64031a_probe upd64031a_probe.cold upd64031a_ops upd64031a_s_routing upd64031a_s_routing.cold upd64031a_driver_init upd64031a_driver upd64031a_driver_exit upd64031a_id upd64031a_core_ops upd64031a_tuner_ops upd64031a_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev __stack_chk_guard i2c_transfer_buffer_flags _printk __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__upd64031a_id_device_table param_ops_int    $  	   %     $  &   &  8   '  P   '  b     g   (  u   &  „   )  ‘   $  ª   &  º     Ø   '  í   &  ü   )    $      Q  $  e  *  ‘  $  !    Æ     â         u    0                   h                                                 (  1     6   (  G     L   (  v     {   (     +  ¥     ´   ,  ¹     Ô     é     î   (       >     S           à     õ     `     d     l     €     è             $          -     .          0           -     3        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rel.text.unlikely .rel.rodata .rel.init.text .rel.exit.text .rodata.str1.1 .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   ]                    	   @       Ä  Ø               )                €                   %   	   @       œ                  /                                  8                                  4   	   @       ¼  8               E      2       <  º                 X             ö  ù                  T   	   @       ô  ¨      	         k                V                  g   	   @       œ  0               w             V                    s   	   @       Ì                  †             j  
                  ‚   	   @       ì                 ‘      2       t  
                               ~  ‰                  ­                                 ©   	   @       ü                  µ      0                          ¾              /                     Î             0  (                                X  @     $         	              ˜
  ,                                 á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ƒ|$`"‡  ‹D$`¾€    ‰D$…Àˆ  ˜„   İ    ‰\$q‰L$$‰ó‰t$‹4$‰õ‹vhİ…ötz8  ‰l$ ‰D$‹D$‹l†‰êƒâuZ‹D$‹D$‹DÆ9Èuë>´&    ƒÂ‹ 9Át‹X…Ûuğ‹X…Ûtì‹[…Ûuâ‹ 9Áuåt& ƒú‡'  „(  ‹vh…öu—‹l$ ‹4$‹\$Ş‰D$‹€4  9Åt‹D$9„ì  …  ‹D$‹˜4  9İ„C  ‰Ø1Òët& ƒÂ‹ 9Åt‹p…öuğ‹H…Étì‹I…Éuâ‹ 9Åuåt& ƒú?‡1  öD$d„  ‹L$‹T$‰Ø´&    v …ÿt9xu9Ï…·   …Òt	;P„ª   ‹ 9ÅuÜ‹T$‹D$…ÒuéÕ   v ‹9İ„ª  ;Cuñû ğÿÿ‡Î  ‰ù…ÿ„ë  ‹$‰\$‰Ë1ö‹€”   ‹P@‹@<‰D$(‹D$`‰T$,‰D$0‹C(‹„°x  …Àt1ÉT$(èüÿÿÿ‰D´4…À„w  ƒştj‹C(¾   ‹„°x  …ÀuÑëæt& ¾êÿÿÿ‹D$Dd+    …>  ƒÄH‰ğ[^_]ÃöD$d…¬  t& ‹D$‹€4  9Å…=ÿÿÿ1ÛéBÿÿÿ´&    ‹\$…Ûtu‹D$‹k‰{‹L$‹|$‰C‹D$4‰ú‰C‹D$8‰C‹$‰Œ¸ì  èüõÿÿ‰Æ…À…ú   …í„Ÿ   ‰èèüÿÿÿ‹<$‹L$`1ö‹D$<‰úèüÿÿÿ‹L$`‹D$@‰úèüÿÿÿé8ÿÿÿ¡   ¹   ºÀ  èüÿÿÿ‰Ã…À„C  ‹t$‹L$‰+‹†8  ‰8  ‰C‰‹D$‰{‹|$‰C‹D$4‰ú‰C‹D$8‰C‹$‰Œ¸ì  èUõÿÿ‰Æ…À…²   ‹D$$àûÿÿèüÿÿÿéUÿÿÿ´&    ‰ò‰Øèüÿÿÿ‰D´4= ğÿÿ‡¾   ‰D´<éhşÿÿ´&    ‹D$‹Hé	şÿÿt& …ít[‰k‹D$<ÇC    èüÿÿÿ‹D$@èüÿÿÿéPşÿÿv éØüÿÿ‰è‹l$ ¨…Õüÿÿ¾ÿÿÿÿé.şÿÿ‹D$¾şÿÿÿ…À…şÿÿ1Ûéœıÿÿ‹D$<èüÿÿÿ‹D$@èüÿÿÿ‹C‹‰B‰‰ØÇ   ÇC"  èüÿÿÿéáıÿÿ¾şÿÿÿé×ıÿÿ‹D$<¾ôÿÿÿèüÿÿÿ‹D$@èüÿÿÿé»ıÿÿ¾ùÿÿÿé±ıÿÿèüÿÿÿ‰Şé¥ıÿÿ´&    èüÿÿÿS‰Ãö@,tƒT  1Ò[éüÿÿÿt& èüÿÿÿ‹C¨udÿ èüÿÿÿƒT  1Ò[éüÿÿÿ¶    ‹Cğƒ ëŞ´&    ´&    èüÿÿÿU¹   ºĞ  WV‰ÆSƒìdd¡    ‰D$`1Àl$‰ïó«†T  ‰$hÀ  èüÿÿÿZ‰Ç…À…¶   ‹^h…Ût"èüÿÿÿ‹ƒT  ¨…ì   dÿ èüÿÿÿ‹[h…ÛuŞ†4  ì  ‰ ‰@ƒÀ9Áuô†H  ‰†H  ‰†L  ëƒÇƒÿtxL½ ‰ú‰ğ‰ëèÉñÿÿ…Àtål$`‹ƒÃèüÿÿÿ9ëuò‹^h…Ûtèüÿÿÿ‹ƒT  ¨u{dÿèüÿÿÿ‹[h…Ûuâ‹$¿ôÿÿÿèüÿÿÿ‹D$`d+    u{ƒÄd‰ø[^_]Ã´&    f1Û¶    ‹L ‹„Ø  “ô   ƒÃ‰L–èüÿÿÿƒûuŞ1ÿë²f‹ƒX  ğƒ éÿÿÿ‹ƒX  ğƒ(…xÿÿÿ‹“X  ƒT  ‹Rèüÿÿÿé_ÿÿÿèüÿÿÿv èüÿÿÿU‰ÕW‰ÏVS‰Ãƒì‹ èüÿÿÿ‰Æ= ğÿÿ‡Ê   ‹S‰ĞƒàƒøtE¸    ‹[‰$èüÿÿÿ‹$1É‰ğR‰úSj è›øÿÿ‰Ã¸    èüÿÿÿƒÄöF,thƒÄ‰Ø[^_]Ãt& ‹C‰ê1Éèüÿÿÿ‰Å= ğÿÿws‹S¸    ‹[‰$èüÿÿÿ‹$‰é‰ğR‰úSj è>øÿÿ‰Ã¸    èüÿÿÿƒÄ…ítŸ‰èèüÿÿÿöF,u˜èüÿÿÿ‹F¨uRdÿèüÿÿÿ‰ØƒÄ[^_]ÃƒÄ‰Ã‰Ø[^_]Ãt& öF,t
‰ëé]ÿÿÿv èüÿÿÿ‹F¨u-dÿèüÿÿÿ‰ëé?ÿÿÿt& ‹Fğƒ(u¨‹VF‹Rèüÿÿÿë˜‹Fğƒ(uÍ‹VF‹Rèüÿÿÿë½´&    ´&    èüÿÿÿW‰×V‰ÆS‹ èüÿÿÿ‰Ã= ğÿÿwt‹F‰ú1Éèüÿÿÿ‹v‰Ç¸    ÿ ğÿÿwfèüÿÿÿ‰ú1É‰ØVè…ñÿÿ‰Æ¸    èüÿÿÿZ…ÿt‰øèüÿÿÿöC,t‰ğ[^_Ãèüÿÿÿ‹C¨uDdÿèüÿÿÿ‰ğ[^_Ã¶    ‰Æ[‰ğ^_Ã´&    èüÿÿÿ1É1Ò‰ØVèñÿÿ‰Æ¸    èüÿÿÿXë£‹Cğƒ(u¶‹SC‹Rèüÿÿÿë¦´&    èüÿÿÿUWV‰ÆSƒì‹Nd¡    ‰D$1À…É…ó   ‹F‰Õèüÿÿÿ‰Ç= ğÿÿ‡Ô   ¡   ¹0   ºÀ èüÿÿÿ‰Ã…À„ß   U¹\  º   èüÿÿÿ‹F‰{(T$‰C,‰Øèüÿÿÿ‰ÆX…öuS‹s,¸    èüÿÿÿ1É1Ò‰øjVSèÿõÿÿ‰Æ¸    èüÿÿÿƒÄ‰à…öuJèüÿÿÿ‰Æ‹D$d+    uvƒÄ‰ğ[^_]Ãt& ‰ØèüÿÿÿöG,uØèüÿÿÿ‹G¨u/dÿèüÿÿÿëÂt& èüÿÿÿëØ´&    f‰Æë©t& ¾êÿÿÿë‹Gğƒ(uË‹WG‹Rèüÿÿÿë»¾ôÿÿÿëŸèüÿÿÿ´&    t& èüÿÿÿUWV‰ÖS‰Ãƒì d¡    ‰D$‹èüÿÿÿ‰Ç= ğÿÿ‡ò  ¸    èüÿÿÿ‹CÇ$êÿÿÿƒø"‡<  ¾€    …Àˆ-  ˆ„   ‹”‡ì  ‹k‰L$ŒÇ4  ‰L$‹Œ‡Ø  ‹C‰T$‰L$ƒà„D  ‰Èèüÿÿÿ‰D$¹   FT$èüÿÿÿ…À…¬  F¹   T$èüÿÿÿ…À…“  ‹C‰î…À„¢   …í„š   ‹L$‰$…É„’   9ÈƒC  ‰D$Ç$äÿÿÿöC…~  ‹D$1í‹\Ç‹D$9Øtc‰|$‰Çë@t& ‹@(¹   T$‹@ ‰D$‰ğèüÿÿÿ…À…  ƒÅ;l$„Ñ   ‹ƒÆ9ß„Ä   ‹C…Àu½‹C…Àt¶‹@ë±Ç$    ¸    èüÿÿÿöG,t!‹D$d+    …ÿ   ‹$ƒÄ [^_]Ãt& èüÿÿÿ‹G¨…¨   dÿèüÿÿÿëÅ¶    ‹L$‹TÏ‹L$9Ñth‰$‰Ëë
ƒÀ‹9Ót‹J…Éuğ‹J…Étì‹I…Éuâëã‹$éƒşÿÿ´&    ‰$éoÿÿÿ´&    ‹|$éNÿÿÿ´&    Ç$    é¼şÿÿt& 1ÀéDşÿÿ´&    f‹|$Ç$òÿÿÿéÿÿÿ‹Gğƒ(…Nÿÿÿ‹WG‹Rèüÿÿÿé;ÿÿÿ‹L$‹D$‰êèüÿÿÿ‰$éàşÿÿèüÿÿÿt& èüÿÿÿUWVS‰Ã¿Ûƒì d¡    ‰D$1À¿D$4‰L$‰T$Áà	Ø‰D$d‹    èüÿÿÿ‹ƒÈ  ‹T$8ÇD$    ‹@8‹´Ø  èüÿÿÿèüÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ÛtB~t& ‰|$>t& ‹K$S0D$èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$èüÿÿÿ‹D$d+    ukƒÄ ‰Ø[^_]Ãt& èüÿÿÿS0‹s$‰ÅD$èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „KÿÿÿûéEÿÿÿèüÿÿÿ´&    ´&    èüÿÿÿU‰ÅWV‰Î¹	   S‰ÓƒìP‹D$d|$(‰D$‹D$h‰D$d¡    ‰D$L1À‰T$$ºÀ  ‰l$ ÇD$    ÇD$    ó«‹D$lÇD$,   ‰t$<‹L$`‰D$@¸   èüÿÿÿ‰D$(…À„4  ‰ÁD$1ÒPD$0P‰Ø‹{èüÿÿÿ[_…À…  …öt‹D$‹…Ét‹D$‹…Ò…  èüÿÿÿ‹T$pd‹    ‹ƒÈ  ÇD$    ‹@8‹´Ø  èüÿÿÿèüÿÿÿ‹ƒp  ~‰D$D$‰ƒp  ‹^…Ût@¶    ‰|$>t& ‹K$S0D$ èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$èüÿÿÿ‹D$(èüÿÿÿƒûu‹D$8…À…™   ‹D$0èüÿÿÿ‹D$Ld+    …7  ƒÄP‰Ø[^_]Ãt& ÇD$,    éáşÿÿv èüÿÿÿS0‹s$‰ÅD$ èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „ÿÿÿûéÿÿÿv ‹|$‹èüÿÿÿ‹D$0‹L$‰‹D$4‰éPÿÿÿ´&    ‹L$`ºÀ  ¸   èüÿÿÿ¹   ‰D$0‰Â‹D$‹ 9ÈGÁ‰D$4…ÒtR‹L$‹1ƒøs$…À„.şÿÿ¶ˆ
¨„!şÿÿ·Lşf‰Lşéşÿÿ‹zƒçü‰
‹Lü‰Lü)ú)ÖÁéó¥éïıÿÿÇD$4    éâıÿÿèüÿÿÿ´&    t& èüÿÿÿUWV‰ÆSƒìp‹¶ä  ‰D$‹„$ˆ   ‰T$‹œ$„   1Ò‰D$‹„$Œ   ‰L$‰D$d¡    ‰D$l1À‰TLƒÀƒø rô1Ò1À‰T,ƒÀƒø rô‹D$‹|$L$L‰D$(‹D$‹‹ ‰D$4‹D$‹ ‰D$8¸   9ÂLĞD$(èŞÿÿ‰D$…ÀxN‹?‹l$,9Ç‰|$<Oø…ÿˆñ  ‰ú1É‰èèüÿÿÿ‰ù‰Ú‰èèüÿÿÿ‰Ç…ÿt=‹l$,ÇD$òÿÿÿD$L9Åt‰èèüÿÿÿ‹D$ld+    …í  ‹D$ƒÄp[^_]Ãt& ‹D$1Òèüÿÿÿ‹  ÇD$$    èüÿÿÿ{èüÿÿÿd¡    ‹ˆp  ‰L$L$ ‰ˆp  ‹[…Ût;‰|$ >t& ‹K$S0D$(èüÿÿÿ‰Æ…öu|$$ ğÿÿwÇD$$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$$‹D$èüÿÿÿ…Û…Ç   ‹D$<‹l$,ƒøÿ„Æ   Œÿşÿÿ;D$õşÿÿ‹T$4‹L$‰‹T$8‹L$‰…À„±   ‹L$‰L$L9Í„¬   ‹D$ÇD$    ‰(éËşÿÿt& èüÿÿÿS0‹s$‰ÅD$(èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „óşÿÿûéíşÿÿv ‰\$‹l$,éJşÿÿv ÇD$   é:şÿÿv é şÿÿÇD$    é#şÿÿºÀ èüÿÿÿ‰Ã…Àt%‹L$<‹T$,èüÿÿÿ‹D$ÇD$    ‰éşÿÿèüÿÿÿÇD$ôÿÿÿ‹l$,éİıÿÿ¶    èüÿÿÿUWV‰ÆSƒìh‹ä  1ö‰D$‹D$|‹¼$ˆ   ‰D$‹„$€   ‰D$d¡    ‰D$d1À‰tDƒÀƒø rô1ö1À‰t$ƒÀƒø rô‹D$‰T$,‰L$0L$D‰D$ d¡    ‰D$8‹„$„   ‰D$4‰ÂD$ èÛÿÿ‰D$…ÀˆX  …ÿuL‹D$èüÿÿÿ‰T$4…Òˆc  …À…[  ‹D$‰Ö‹l$$9ÂOğ1É‰è‰òèüÿÿÿ‹T$‰ñ‰èèüÿÿÿ…À…,  ‹D$1Òèüÿÿÿ‹›  ‰|$èüÿÿÿ{èüÿÿÿd¡    ‹ˆp  ‰L$L$‰ˆp  ‹[…Ût;‰|$>t& ‹K$S0D$ èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹t$‹D$èüÿÿÿ…öˆ÷   ‹\$4‹D$$;\$u…Ûxq‰t$t6¹   ‰Ú‰$èüÿÿÿ‹$‹D$‰Ùèüÿÿÿ…ÀuF‹D$4‹L$èüÿÿÿ…Éu5‹D$$L$D9Ètèüÿÿÿ‹D$dd+    …”   ‹D$ƒÄh[^_]Ã´&    ‹D$$ÇD$òÿÿÿëÁ´&    v èüÿÿÿS0‹s$‰ÅD$ èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „Ãşÿÿûé½şÿÿv ‰t$éJÿÿÿèüÿÿÿfèüÿÿÿUWV‰ÖS‰Ãƒì<‹|$T‹D$P‰|$d‹    ‰T$8‹“ä  ‰\$‹‰D$Ø‰t$ ‰D$‹D$X‰L$$‰D$‹²  ‰\$(ÇD$0    d‹    ÇD$4    ‰\$,èüÿÿÿèüÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ÛtC~¶    ‰|$>t& ‹K$S0D$èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹D$…Àx‹L$‹T$(;€   …Òux‹T$8d+    uyƒÄ<[^_]Ãv èüÿÿÿS0‹s$‰ÅD$èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „;ÿÿÿûé5ÿÿÿv ‰ë„¸òÿÿÿézÿÿÿèüÿÿÿ     cgroup_bpf_enabled_key  __cgroup_bpf_run_filter_skb  __cgroup_bpf_run_filter_sk  __cgroup_bpf_run_filter_sock_addr  __cgroup_bpf_run_filter_sock_ops                                                                        @   ğ  0  P  `  p  Ğ     °  @  À  ğ  `	  À  €                   p  °  ğ  °  0  p  €    À  à       0  P$  °$  @&  °'  (  à)  ,   .  ğ0   4   6  cgroup_id:	%llu
attach_type:	%d
      À      Ô  @        €      ¤         -  -      /  È/      T2   3      45  06      D7  È7      include/linux/skbuff.h / kernel/bpf/cgroup.c include/linux/thread_info.h °      ö	  ­     @       ƒ	 #     Õ 	 3  -   é 	¸$   èüÿÿÿ ›  “  Ş    ›$  &  &  s'  Œ'  s(  ³)  S,   ÿÿÿ	
ÿÿÿÿÿÿÿÿÿ strnlen           P      0                                          	                                                               @                           	                                                            	                                                             	                                                             	         
                                   °                                             
                                                                                                               À  `  à        p                       GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ              
              +             N              o             •   4          ½              ã   5            P          /            
 T  Q          €  s          ®             Ú  t            •          2             ]  @   ª    w  0      •  P       ­  `  
     Å  p  V     ä  Ğ  /     ú                           V     )  `  E    @  @  y                 cmd_drivers/media/i2c/upd64031a.o := gcc -Wp,-MMD,drivers/media/i2c/.upd64031a.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"upd64031a"' -DKBUILD_MODNAME='"upd64031a"' -D__KBUILD_MODNAME=kmod_upd64031a -c -o drivers/media/i2c/upd64031a.o drivers/media/i2c/upd64031a.c 

source_drivers/media/i2c/upd64031a.o := drivers/media/i2c/upd64031a.c

deps_drivers/media/i2c/upd64031a.o := \
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
 