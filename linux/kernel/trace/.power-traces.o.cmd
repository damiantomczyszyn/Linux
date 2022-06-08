g nr_stack_trace_entries;

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

	for (i = 0; i < A_do_set_cpus_allowed(p, cpumask_of(rq->cpu), SCA_MIGRATE_DISABLE);
}

void migrate_disable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled) {
		p->migration_disabled++;
		return;
	}

	preempt_disable();
	this_rq()->nr_pinned++;
	p->migration_disabled = 1;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_disable);

void migrate_enable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled > 1) {
		p->migration_disabled--;
		return;
	}

	if (WARN_ON_ONCE(!p->migration_disabled))
		return;

	/*
	 * Ensure stop_task runs either before or after this, and that
	 * __set_cpus_allowed_ptr(SCA_MIGRATE_ENABLE) doesn't schedule().
	 */
	preempt_disable();
	if (p->cpus_ptr != &p->cpus_mask)
		__set_cpus_allowed_ptr(p, &p->cpus_mask, SCA_MIGRATE_ENABLE);
	/*
	 * Mustn't clear migration_disabled() until cpus_ptr points back at the
	 * regular cpus_mask, otherwise things that race (eg.
	 * select_fallback_rq) get confused.
	 */
	barrier();
	p->migration_disabled = 0;
	this_rq()->nr_pinned--;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_enable);

static inline bool rq_has_pinned_tasks(struct rq *rq)
{
	return rq->nr_pinned;
}

/*
 * Per-CPU kthreads are allowed to run on !active && online CPUs, see
 * __set_cpus_allowed_ptr() and select_fallback_rq().
 */
static inline bool is_cpu_allowed(struct task_struct *p, int cpu)
{
	/* When not in the task's cpumask, no point in looking further. */
	if (!cpumask_test_cpu(cpu, p->cpus_ptr))
		return false;

	/* migrate_disabled() must be allowed to finish. */
	if (is_migration_disabled(p))
		return cpu_online(cpu);

	/* Non kernel threads are not allowed during either online or offline. */
	if (!(p->flags & PF_KTHREAD))
		return cpu_active(cpu) && task_cpu_possible(cpu, p);

	/* KTHREAD_IS_PER_CPU is always allowed. */
	if (kthread_is_per_cpu(p))
		return cpu_online(cpu);

	/* Regular kernel threads don't get to stay during offline. */
	if (cpu_dying(cpu))
		return false;

	/* But are allowed during online. */
	return cpu_online(cpu);
}

/*
 * This is how migration works:
 *
 * 1) we invoke migration_cpu_stop() on the target CPU using
 *    stop_one_cpu().
 * 2) stopper starts to run (implicitly forcing the migrated thread
 *    off the CPU)
 * 3) it checks whether the migrated task is still in the wrong runqueue.
 * 4) if it's in the wrong runqueue then the migration thread removes
 *    it and puts it into the right queue.
 * 5) stopper completes and stop_one_cpu() returns and the migration
 *    is done.
 */

/*
 * move_queued_task - move a queued task to new rq.
 *
 * Returns (locked) new rq. Old rq's lock is released.
 */
static struct rq *move_queued_task(struct rq *rq, struct rq_flags *rf,
				   struct task_struct *p, int new_cpu)
{
	lockdep_assert_rq_held(rq);

	deactivate_task(rq, p, DEQUEUE_NOCLOCK);
	set_task_cpu(p, new_cpu);
	rq_unlock(rq, rf);

	rq = cpu_rq(new_cpu);

	rq_lock(rq, rf);
	BUG_ON(task_cpu(p) != new_cpu);
	activate_task(rq, p, 0);
	check_preempt_curr(rq, p, 0);

	return rq;
}

struct migration_arg {
	struct task_struct		*task;
	int				dest_cpu;
	struct set_affinity_pending	*pending;
};

/*
 * @refs: number of wait_for_completion()
 * @stop_pending: is @stop_work in use
 */
struct set_affinity_pending {
	refcount_t		refs;
	unsigned int		stop_pending;
	struct completion	done;
	struct cpu_stop_work	stop_work;
	struct migration_arg	arg;
};

/*
 * Move (not current) task off this CPU, onto the destination CPU. We're doing
 * this because either it can't run here any more (set_cpus_allowed()
 * away from this CPU, or CPU going down), or because we're
 * attempting to rebalance this task on exec (sched_exec).
 *
 * So we race with normal scheduler movements, but that's OK, as long
 * as the task is no longer on this CPU.
 */
static struct rq *__migrate_task(struct rq *rq, struct rq_flags *rf,
				 struct task_struct *p, int dest_cpu)
{
	/* Affinity changed (again). */
	if (!is_cpu_allowed(p, dest_cpu))
		return rq;

	update_rq_clock(rq);
	rq = move_queued_task(rq, rf, p, dest_cpu);

	return rq;
}

/*
 * migration_cpu_stop - this will be executed by a highprio stopper thread
 * and performs thread migration by bumping thread off CPU then
 * 'pushing' onto another runqueue.
 */
static int migration_cpu_stop(void *data)
{
	struct migration_arg *arg = data;
	struct set_affinity_pending *pending = arg->pending;
	struct task_struct *p = arg->task;
	struct rq *rq = this_rq();
	bool complete = false;
	struct rq_flags rf;

	/*
	 * The original target CPU might have gone down and we might
	 * be on another CPU but it doesn't matter.
	 */
	local_irq_save(rf.flags);
	/*
	 * We need to explicitly wake pending tasks before running
	 * __migrate_task() such that we will not miss enforcing cpus_ptr
	 * during wakeups, see set_cpus_allowed_ptr()'s TASK_WAKING test.
	 */
	flush_smp_call_function_from_idle();

	raw_spin_lock(&p->pi_lock);
	rq_lock(rq, &rf);

	/*
	 * If we were passed a pending, then ->stop_pending was set, thus
	 * p->migration_pending must have remained stable.
	 */
	WARN_ON_ONCE(pending && pending != p->migration_pending);

	/*
	 * If task_rq(p) != rq, it cannot be migrated here, because we're
	 * holding rq->lock, if p->on_rq == 0 it cannot get enqueued because
	 * we're holding p->pi_lock.
	 */
	if (task_rq(p) == rq) {
		if (is_migration_disabled(p))
			goto out;

		if (pending) {
			p->migration_pending = NULL;
			complete = true;

			if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask))
				goto out;
		}

		if (task_on_rq_queued(p))
			rq = __migrate_task(rq, &rf, p, arg->dest_cpu);
		else
			p->wake_cpu = arg->dest_cpu;

		/*
		 * XXX __migrate_task() can fail, at which point we might end
		 * up running on a dodgy CPU, AFAICT this can only happen
		 * during CPU hotplug, at which point we'll get pushed out
		 * anyway, so it's probably not a big deal.
		 */

	} else if (pending) {
		/*
		 * This happens when we get migrated between migrate_enable()'s
		 * preempt_enable() and scheduling the stopper task. At that
		 * point we're a regular task again and not current anymore.
		 *
		 * A !PREEMPT kernel has a giant hole here, which makes it far
		 * more likely.
		 */

		/*
		 * The task moved before the stopper got to run. We're holding
		 * ->pi_lock, so the allowed mask is stable - if it got
		 * somewhere allowed, we're done.
		 */
		if (cpumask_test_cpu(task_cpu(p), p->cpus_ptr)) {
			p->migration_pending = NULL;
			complete = true;
			goto out;
		}

		/*
		 * When migrate_enable() hits a rq mis-match we can't reliably
		 * determine is_migration_disabled() and so have to chase after
		 * it.
		 */
		WARN_ON_ONCE(!pending->stop_pending);
		task_rq_unlock(rq, p, &rf);
		stop_one_cpu_nowait(task_cpu(p), migration_cpu_stop,
				    &pending->arg, &pending->stop_work);
		return 0;
	}
out:
	if (pending)
		pending->stop_pending = false;
	task_rq_unlock(rq, p, &rf);

	if (complete)
		complete_all(&pending->done);

	return 0;
}

int push_cpu_stop(void *arg)
{
	struct rq *lowest_rq = NULL, *rq = this_rq();
	struct task_struct *p = arg;

	raw_spin_lock_irq(&p->pi_lock);
	raw_spin_rq_lock(rq);

	if (task_rq(p) != rq)
		goto out_unlock;

	if (is_migration_disabled(p)) {
		p->migration_flags |= MDF_PUSH;
		goto out_unlock;
	}

	p->migration_flags &= ~MDF_PUSH;

	if (p->sched_class->find_lock_rq)
		lowest_rq = p->sched_class->find_lock_rq(p, rq);

	if (!lowest_rq)
		goto out_unlock;

	// XXX validate p is still the highest prio task
	if (task_rq(p) == rq) {
		deactivate_task(rq, p, 0);
		set_task_cpu(p, lowest_rq->cpu);
		activate_task(lowest_rq, p, 0);
		resched_curr(lowest_rq);
	}

	double_unlock_balance(rq, lowest_rq);

out_unlock:
	rq->push_busy = false;
	raw_spin_rq_unlock(rq);
	raw_spin_unlock_irq(&p->pi_lock);

	put_task_struct(p);
	return 0;
}

/*
 * sched_class::set_cpus_allowed must do the below, but is not required to
 * actually call this function.
 */
void set_cpus_allowed_common(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	if (flags & (SCA_MIGRATE_ENABLE | SCA_MIGRATE_DISABLE)) {
		p->cpus_ptr = new_mask;
		return;
	}

	cpumask_copy(&p->cpus_mask, new_mask);
	p->nr_cpus_allowed = cpumask_weight(new_mask);
}

static void
__do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	struct rq *rq = task_rq(p);
	bool queued, running;

	/*
	 * This here violates the locking rules for affinity, since we're only
	 * supposed to change these variables while holding both rq->lock and
	 * p->pi_lock.
	 *
	 * HOWEVER, it magically works, because ttwu() is the only code that
	 * accesses these variables under p->pi_lock and only does so after
	 * smp_cond_load_acquire(&p->on_cpu, !VAL), and we're in __schedule()
	 * before finish_task().
	 *
	 * XXX do further audits, this smells like something putrid.
	 */
	if (flags & SCA_MIGRATE_DISABLE)
		SCHED_WARN_ON(!p->on_cpu);
	else
		lockdep_assert_held(&p->pi_lock);

	queued = task_on_rq_queued(p);
	running = task_current(rq, p);

	if (queued) {
		/*
		 * Because __kthread_bind() calls this on blocked tasks without
		 * holding rq->lock.
		 */
		lockdep_assert_rq_held(rq);
		dequeue_task(rq, p, DEQUEUE_SAVE | DEQUEUE_NOCLOCK);
	}
	if (running)
		put_prev_task(rq, p);

	p->sched_class->set_cpus_allowed(p, new_mask, flags);

	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE | ENQUEUE_NOCLOCK);
	if (running)
		set_next_task(rq, p);
}

void do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask)
{
	__do_set_cpus_allowed(p, new_mask, 0);
}

int dup_user_cpus_ptr(struct task_struct *dst, struct task_struct *src,
		      int node)
{
	if (!src->user_cpus_ptr)
		return 0;

	dst->user_cpus_ptr = kmalloc_node(cpumask_size(), GFP_KERNEL, node);
	if (!dst->user_cpus_ptr)
		return -ENOMEM;

	cpumask_copy(dst->user_cpus_ptr, src->user_cpus_ptr);
	return 0;
}

static inline struct cpumask *clear_user_cpus_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = NULL;

	swap(p->user_cpus_ptr, user_mask);

	return user_mask;
}

void release_user_cpus_ptr(struct task_struct *p)
{
	kfree(clear_user_cpus_ptr(p));
}

/*
 * This function is wildly self concurrent; here be dragons.
 *
 *
 * When given a valid mask, __set_cpus_allowed_ptr() must block until the
 * designated task is enqueued on an allowed CPU. If that task is currently
 * running, we have to kick it out using the CPU stopper.
 *
 * Migrate-Disable comes along and tramples all over our nice sandcastle.
 * Consider:
 *
 *     Initial conditions: P0->cpus_mask = [0, 1]
 *
 *     P0@CPU0                  P1
 *
 *     migrate_disable();
 *     <preempted>
 *                              set_cpus_allowed_ptr(P0, [1]);
 *
 * P1 *cannot* return from this set_cpus_allowed_ptr() call until P0 executes
 * its outermost migrate_enable() (i.e. it exits its Migrate-Disable region).
 * This means we need the following scheme:
 *
 *     P0@CPU0                  P1
 *
 *     migrate_disable();
 *     <preempted>
 *                              set_cpus_allowed_ptr(P0, [1]);
 *                                <blocks>
 *     <resumes>
 *     migrate_enable();
 *       __set_cpus_allowed_ptr();
 *       <wakes local stopper>
 *                         `--> <woken on migration completion>
 *
 * Now the fun stuff: there may be several P1-like tasks, i.e. multiple
 * concurrent set_cpus_allowed_ptr(P0, [*]) calls. CPU affinity changes of any
 * task p are serialized by p->pi_lock, which we can leverage: the one that
 * should come into effect at the end of the Migrate-Disable region is the last
 * one. This means we only need to track a single cpumask (i.e. p->cpus_mask),
 * but we still need to properly signal those waiting tasks at the appropriate
 * moment.
 *
 * This is implemented using struct set_affinity_pending. The first
 * __set_cpus_allowed_ptr() caller within a given Migrate-Disable region will
 * setup an instance of that struct and install it on the targeted task_struct.
 * Any and all further callers will reuse that instance. Those then wait for
 * a completion signaled at the tail of the CPU stopper callback (1), triggered
 * on the end of the Migrate-Disable region (i.e. outermost migrate_enable()).
 *
 *
 * (1) In the cases covered above. There is one more where the completion is
 * signaled within affine_move_task() itself: when a subsequent affinity request
 * occurs after the stopper bailed out due to the targeted task still being
 * Migrate-Disable. Consider:
 *
 *     Initial conditions: P0->cpus_mask = [0, 1]
 *
 *     CPU0		  P1				P2
 *     <P0>
 *       migrate_disable();
 *       <preempted>
 *                        set_cpus_allowed_ptr(P0, [1]);
 *                          <blocks>
 *     <migration/0>
 *       migration_cpu_stop()
 *         is_migration_disabled()
 *           <bails>
 *                                                       set_cpus_allowed_ptr(P0, [0, 1]);
 *                                                         <signal completion>
 *                          <awakes>
 *
 * Note that the above is safe vs a concurrent migrate_enable(), as any
 * pending affinity completion is preceded by an uninstallation of
 * p->migration_pending done with p->pi_lock held.
 */
static int affine_move_task(struct rq *rq, struct task_struct *p, struct rq_flags *rf,
			    int dest_cpu, unsigned int flags)
{
	struct set_affinity_pending my_pending = { }, *pending = NULL;
	bool stop_pending, complete = false;

	/* Can the task run on the task's current CPU? If so, we're done */
	if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask)) {
		struct task_struct *push_task = NULL;

		if ((flags & SCA_MIGRATE_ENABLE) &&
		    (p->migration_flags & MDF_PUSH) && !rq->push_busy) {
			rq->push_busy = true;
			push_task = get_task_struct(p);
		}

		/*
		 * If there are pending waiters, but no pending stop_work,
		 * then complete now.
		 */
		pending = p->migration_pending;
		if (pending && !pending->stop_pending) {
			p->migration_pending = NULL;
			complete = true;
		}

		task_rq_unlock(rq, p, rf);

		if (push_task) {
			stop_one_cpu_nowait(rq->cpu, push_cpu_stop,
					    p, &rq->push_work);
		}

		if (complete)
			complete_all(&pending->done);

		return 0;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		/* serialized by p->pi_lock */
		if (!p->migration_pending) {
			/* Install the request */
			refcount_set(&my_pending.refs, 1);
			init_completion(&my_pending.done);
			my_pending.arg = (struct migration_arg) {
				.task = p,
				.dest_cpu = dest_cpu,
				.pending = &my_pending,
			};

			p->migration_pending = &my_pending;
		} else {
			pending = p->migration_pending;
			refcount_inc(&pending->refs);
			/*
			 * Affinity has changed, but we've already installed a
			 * pending. migration_cpu_stop() *must* see this, else
			 * we risk a completion of the pending despite having a
			 * task on a disallowed CPU.
			 *
			 * Serialized by p->pi_lock, so this is safe.
			 */
			pending->arg.dest_cpu = dest_cpu;
		}
	}
	pending = p->migration_pending;
	/*
	 * - !MIGRATE_ENABLE:
	 *   we'll have installed a pending if there wasn't one already.
	 *
	 * - MIGRATE_ENABLE:
	 *   we're here because the current CPU isn't matching anymore,
	 *   the only way that can happen is because of a concurrent
	 *   set_cpus_allowed_ptr() call, which should then still be
	 *   pending completion.
	 *
	 * Either way, we really should have a @pending here.
	 */
	if (WARN_ON_ONCE(!pending)) {
		task_rq_unlock(rq, p, rf);
		return -EINVAL;
	}

	if (task_running(rq, p) || READ_ONCE(p->__state) == TASK_WAKING) {
		/*
		 * MIGRATE_ENABLE gets here because 'p == current', but for
		 * anything else we cannot do is_migration_disabled(), punt
		 * and have the stopper function handle it all race-free.
		 */
		stop_pending = pending->stop_pending;
		if (!stop_pending)
			pending->stop_pending = true;

		if (flags & SCA_MIGRATE_ENABLE)
			p->migration_flags &= ~MDF_PUSH;

		task_rq_unlock(rq, p, rf);

		if (!stop_pending) {
			stop_one_cpu_nowait(cpu_of(rq), migration_cpu_stop,
					    &pending->arg, &pending->stop_work);
		}

		if (flags & SCA_MIGRATE_ENABLE)
			return 0;
	} else {

		if (!is_migration_disabled(p)) {
			if (task_on_rq_queued(p))
				rq = move_queued_task(rq, rf, p, dest_cpu);

			if (!pending->stop_pending) {
				p->migration_pending = NULL;
				complete = true;
			}
		}
		task_rq_unlock(rq, p, rf);

		if (complete)
			complete_all(&pending->done);
	}

	wait_for_completion(&pending->done);

	if (refcount_dec_and_test(&pending->refs))
		wake_up_var(&pending->refs); /* No UaF, just an address */

	/*
	 * Block the original owner of &pending until all subsequent callers
	 * have seen the completion and decremented the refcount
	 */
	wait_var_event(&my_pending.refs, !refcount_read(&my_pending.refs));

	/* ARGH */
	WARN_ON_ONCE(my_pending.stop_pending);

	return 0;
}

/*
 * Called with both p->pi_lock and rq->lock held; drops both before returning.
 */
static int __set_cpus_allowed_ptr_locked(struct task_struct *p,
					 const struct cpumask *new_mask,
					 u32 flags,
					 struct rq *rq,
					 struct rq_flags *rf)
	__releases(rq->lock)
	__releases(p->pi_lock)
{
	const struct cpumask *cpu_allowed_mask = task_cpu_possible_mask(p);
	const struct cpumask *cpu_valid_mask = cpu_active_mask;
	bool kthread = p->flags & PF_KTHREAD;
	struct cpumask *user_mask = NULL;
	unsigned int dest_cpu;
	int ret = 0;

	update_rq_clock(rq);

	if (kthread || is_migration_disabled(p)) {
		/*
		 * Kernel threads are allowed on online && !active CPUs,
		 * however, during cpu-hot-unplug, even these might get pushed
		 * away if not KTHREAD_IS_PER_CPU.
		 *
		 * Specifically, migration_disabled() tasks must not fail the
		 * cpumask_any_and_distribute() pick below, esp. so on
		 * SCA_MIGRATE_ENABLE, otherwise we'll not call
		 * set_cpus_allowed_common() and actually reset p->cpus_ptr.
		 */
		cpu_valid_mask = cpu_online_mask;
	}

	if (!kthread && !cpumask_subset(new_mask, cpu_allowed_mask)) {
		ret = -EINVAL;
		goto out;
	}

	/*
	 * Must re-check here, to close a race against __kthread_bind(),
	 * sched_setaffinity() is not guaranteed to observe the flag.
	 */
	if ((flags & SCA_CHECK) && (p->flags & PF_NO_SETAFFINITY)) {
		ret = -EINVAL;
		goto out;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		if (cpumask_equal(&p->cpus_mask, new_mask))
			goto out;

		if (WARN_ON_ONCE(p == current &&
				 is_migration_disabled(p) &&
				 !cpumask_test_cpu(task_cpu(p), new_mask))) {
			ret = -EBUSY;
			goto out;
		}
	}

	/*
	 * Picking a ~random cpu helps in cases where we are changing affinity
	 * for groups of tasks (ie. cpuset), so that load balancing is not
	 * immediately required to distribute the tasks within their new mask.
	 */
	dest_cpu = cpumask_any_and_distribute(cpu_valid_mask, new_mask);
	if (dest_cpu >= nr_cpu_ids) {
		ret = -EINVAL;
		goto out;
	}

	__do_set_cpus_allowed(p, new_mask, flags);

	if (flags & SCA_USER)
		user_mask = clear_user_cpus_ptr(p);

	ret = affine_move_task(rq, p, rf, dest_cpu, flags);

	kfree(user_mask);

	return ret;

out:
	task_rq_unlock(rq, p, rf);

	return ret;
}

/*
 * Change a given task's CPU affinity. Migrate the thread to a
 * proper CPU and schedule it away if the CPU it's executing on
 * is removed from the allowed bitmask.
 *
 * NOTE: the caller must have a valid reference to the task, the
 * task must not exit() & deallocate itself prematurely. The
 * call is not atomic; no spinlocks may be held.
 */
static int __set_cpus_allowed_ptr(struct task_struct *p,
				  const struct cpumask *new_mask, u32 flags)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(p, &rf);
	return __set_cpus_allowed_ptr_locked(p, new_mask, flags, rq, &rf);
}

int set_cpus_allowed_ptr(struct task_struct *p, const struct cpumask *new_mask)
{
	return __set_cpus_allowed_ptr(p, new_mask, 0);
}
EXPORT_SYMBOL_GPL(set_cpus_allowed_ptr);

/*
 * Change a given task's CPU affinity to the intersection of its current
 * affinity mask and @subset_mask, writing the resulting mask to @new_mask
 * and pointing @p->user_cpus_ptr to a copy of the old mask.
 * If the resulting mask is empty, leave the affinity unchanged and return
 * -EINVAL.
 */
static int restrict_cpus_allowed_ptr(struct task_struct *p,
				     struct cpumask *new_mask,
				     const struct cpumask *subset_mask)
{
	struct cpumask *user_mask = NULL;
	struct rq_flags rf;
	struct rq *rq;
	int err;

	if (!p->user_cpus_ptr) {
		user_mask = kmalloc(cpumask_size(), GFP_KERNEL);
		if (!user_mask)
			return -ENOMEM;
	}

	rq = task_rq_lock(p, &rf);

	/*
	 * Forcefully restricting the affinity of a deadline task is
	 * likely to cause problems, so fail and noisily override the
	 * mask entirely.
	 */
	if (task_has_dl_policy(p) && dl_bandwidth_enabled()) {
		err = -EPERM;
		goto err_unlock;
	}

	if (!cpumask_and(new_mask, &p->cpus_mask, subset_mask)) {
		err = -EINVAL;
		goto err_unlock;
	}

	/*
	 * We're about to butcher the task affinity, so keep track of what
	 * the user asked for in case we're able to restore it later on.
	 */
	if (user_mask) {
		cpumask_copy(user_mask, p->cpus_ptr);
		p->user_cpus_ptr = user_mask;
	}

	return __set_cpus_allowed_ptr_locked(p, new_mask, 0, rq, &rf);

err_unlock:
	task_rq_unlock(rq, p, &rf);
	kfree(user_mask);
	return err;
}

/*
 * Restrict the CPU affinity of task @p so that it is a subset of
 * task_cpu_possible_mask() and point @p->user_cpu_ptr to a copy of the
 * old affinity mask. If the resulting mask is empty, we warn and walk
 * up the cpuset hierarchy until we find a suitable mask.
 */
void force_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	cpumask_var_t new_mask;
	const struct cpumask *override_mask = task_cpu_possible_mask(p);

	alloc_cpumask_var(&new_mask, GFP_KERNEL);

	/*
	 * __migrate_task() can fail silently in the face of concurrent
	 * offlining of the chosen destination CPU, so take the hotplug
	 * lock to ensure that the migration succeeds.
	 */
	cpus_read_lock();
	if (!cpumask_available(new_mask))
		goto out_set_mask;

	if (!restrict_cpus_allowed_ptr(p, new_mask, override_mask))
		goto out_free_mask;

	/*
	 * We failed to find a valid subset of the affinity mask for the
	 * task, so override it based on its cpuset hierarchy.
	 */
	cpuset_cpus_allowed(p, new_mask);
	override_mask = new_mask;

out_set_mask:
	if (printk_ratelimit()) {
		printk_deferred("Overriding affinity for process %d (%s) to CPUs %*pbl\n",
				task_pid_nr(p), p->comm,
				cpumask_pr_args(override_mask));
	}

	WARN_ON(set_cpus_allowed_ptr(p, override_mask));
out_free_mask:
	cpus_read_unlock();
	free_cpumask_var(new_mask);
}

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask);

/*
 * Restore the affinity of a task @p which was previously restricted by a
 * call to force_compatible_cpus_allowed_ptr(). This will clear (and free)
 * @p->user_cpus_ptr.
 *
 * It is the caller's responsibility to serialise this with any calls to
 * force_compatible_cpus_allowed_ptr(@p).
 */
void relax_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = p->user_cpus_ptr;
	unsigned long flags;

	/*
	 * Try to restore the old affinity mask. If this fails, then
	 * we free the mask explicitly to avoid it being inherited across
	 * a subsequent fork().
	 */
	if (!user_mask || !__sched_setaffinity(p, user_mask))
		return;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	user_mask = clear_user_cpus_ptr(p);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

	kfree(user_mask);
}

void set_task_cpu(struct task_struct *p, unsigned int new_cpu)
{
#ifdef CONFIG_SCHED_DEBUG
	unsigned int state = READ_ONCE(p->__state);

	/*
	 * We should never call set_task_cpu() on a blocked task,
	 * ttwu() will sort out the placement.
	 */
	WARN_ON_ONCE(state != TASK_RUNNING && state != TASK_WAKING && !p->on_rq);

	/*
	 * Migrating fair class task must have p->on_rq = TASK_ON_RQ_MIGRATING,
	 * because schedstat_wait_{start,end} rebase migrating task's wait_start
	 * time relying on p->on_rq.
	 */
	WARN_ON_ONCE(state == TASK_RUNNING &&
		     p->sched_class == &fair_sched_class &&
		     (p->on_rq && !task_on_rq_migrating(p)));

#ifdef CONFIG_LOCKDEP
	/*
	 * The caller should hold either p->pi_lock or rq->lock, when changing
	 * a task's CPU. ->pi_lock for waking tasks, rq->lock for runnable tasks.
	 *
	 * sched_move_task() holds both and thus holding either pins the cgroup,
	 * see task_group().
	 *
	 * Furthermore, all task_rq users should acquire both locks, see
	 * task_rq_lock().
	 */
	WARN_ON_ONCE(debug_locks && !(lockdep_is_held(&p->pi_lock) ||
				      lockdep_is_held(__rq_lockp(task_rq(p)))));
#endif
	/*
	 * Clearly, migrating tasks to offline CPUs is a fairly daft thing.
	 */
	WARN_ON_ONCE(!cpu_online(new_cpu));

	WARN_ON_ONCE(is_migration_disabled(p));
#endif

	trace_sched_migrate_task(p, new_cpu);

	if (task_cpu(p) != new_cpu) {
		if (p->sched_class->migrate_task_rq)
			p->sched_class->migrate_task_rq(p, new_cpu);
		p->se.nr_migrations++;
		rseq_migrate(p);
		perf_event_task_migrate(p);
	}

	__set_task_cpu(p, new_cpu);
}

#ifdef CONFIG_NUMA_BALANCING
static void __migrate_swap_task(struct task_struct *p, int cpu)
{
	if (task_on_rq_queued(p)) {
		struct rq *src_rq, *dst_rq;
		struct rq_flags srf, drf;

		src_rq = task_rq(p);
		dst_rq = cpu_rq(cpu);

		rq_pin_lock(src_rq, &srf);
		rq_pin_lock(dst_rq, &drf);

		deactivate_task(src_rq, p, 0);
		set_task_cpu(p, cpu);
		activate_task(dst_rq, p, 0);
		check_preempt_curr(dst_rq, p, 0);

		rq_unpin_lock(dst_rq, &drf);
		rq_unpin_lock(src_rq, &srf);

	} else {
		/*
		 * Task isn't running anymore; make it appear like we migrated
		 * it before it went to sleep. This means on wakeup we make the
		 * previous CPU our target instead of where it really is.
		 */
		p->wake_cpu = cpu;
	}
}

struct migration_swap_arg {
	struct task_struct *src_task, *dst_task;
	int src_cpu, dst_cpu;
};

static int migrate_swap_stop(void *data)
{
	struct migration_swap_arg *arg = data;
	struct rq *src_rq, *dst_rq;
	int ret = -EAGAIN;

	if (!cpu_active(arg->src_cpu) || !cpu_active(arg->dst_cpu))
		return -EAGAIN;

	src_rq = cpu_rq(arg->src_cpu);
	dst_rq = cpu_rq(arg->dst_cpu);

	double_raw_lock(&arg->src_task->pi_lock,
			&arg->dst_task->pi_lock);
	double_rq_lock(src_rq, dst_rq);

	if (task_cpu(arg->dst_task) != arg->dst_cpu)
		goto unlock;

	if (task_cpu(arg->src_task) != arg->src_cpu)
		goto unlock;

	if (!cpumask_test_cpu(arg->dst_cpu, arg->src_task->cpus_ptr))
		goto unlock;

	if (!cpumask_test_cpu(arg->src_cpu, arg->dst_task->cpus_ptr))
		goto unlock;

	__migrate_swap_task(arg->src_task, arg->dst_cpu);
	__migrate_swap_task(arg->dst_task, arg->src_cpu);

	ret = 0;

unlock:
	double_rq_unlock(src_rq, dst_rq);
	raw_spin_unlock(&arg->dst_task->pi_lock);
	raw_spin_unlock(&arg->src_task->pi_lock);

	return ret;
}

/*
 * Cross migrate two tasks
 */
int migrate_swap(struct task_struct *cur, struct task_struct *p,
		int target_cpu, int curr_cpu)
{
	struct migration_swap_arg arg;
	int ret = -EINVAL;

	arg = (struct migration_swap_arg){
		.src_task = cur,
		.src_cpu = curr_cpu,
		.dst_task = p,
		.dst_cpu = target_cpu,
	};

	if (arg.src_cpu == arg.dst_cpu)
		goto out;

	/*
	 * These three tests are all lockless; this is OK since all of them
	 * will be re-checked with proper locks held further down the line.
	 */
	if (!cpu_active(arg.src_cpu) || !cpu_active(arg.dst_cpu))
		goto out;

	if (!cpumask_test_cpu(arg.dst_cpu, arg.src_task->cpus_ptr))
		goto out;

	if (!cpumask_test_cpu(arg.src_cpu, arg.dst_task->cpus_ptr))
		goto out;

	trace_sched_swap_numa(cur, arg.src_cpu, p, arg.dst_cpu);
	ret = stop_two_cpus(arg.dst_cpu, arg.src_cpu, migrate_swap_stop, &arg);

out:
	return ret;
}
#endif /* CONFIG_NUMA_BALANCING */

/*
 * wait_task_inactive - wait for a thread to unschedule.
 *
 * If @match_state is nonzero, it's the @p->state value just checked and
 * not expected to change.  If it changes, i.e. @p might have woken up,
 * then return zero.  When we succeed in waiting for @p to be off its CPU,
 * we return a positive number (its total switch count).  If a second call
 * a short while later returns the same number, the caller can be sure that
 * @p has remained unscheduled the whole time.
 *
 * The caller must ensure that the task *will* unschedule sometime soon,
 * else this function might spin for a *long* time. This function can't
 * be called with interrupts off, or it may introduce deadlock with
 * smp_call_function() if an IPI is sent by the same process we are
 * waiting to become inactive.
 */
unsigned long wait_task_inactive(struct task_struct *p, unsigned int match_state)
{
	int running, queued;
	struct rq_flags rf;
	unsigned long ncsw;
	struct rq *rq;

	for (;;) {
		/*
		 * We do the initial early heuristics without holding
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
			found 