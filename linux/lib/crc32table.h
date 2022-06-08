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
öÃtá·Lşf‰LşëÕ¶    …ÛtÍë‡f¸êÿÿÿëÅ´&    fèüÿÿÿV‰ÈSƒì$d‹    ‰T$ 1Ò‹t$0‰â‰ñèüÿÿÿ‰Ã…ÀxD¸€Ù èüÿÿÿ¡”Ù XĞ=”Ù t‹C,‰âèüÿÿÿ…Àt5‹C0XĞ=”Ù uå¸€Ù »íÿÿÿèüÿÿÿ‹D$ d+    u%ƒÄ$‰Ø[^Ãv ‰Øèíÿÿ‰Ã¸€Ù èüÿÿÿ…ÛDŞëÎèüÿÿÿ´&    fèüÿÿÿS¸€Ù ‰Ëèüÿÿÿ‹L$‰Øº@w èüÿÿÿ‰Ã…Àx1Àè1äÿÿ¸€Ù èüÿÿÿ‰Ø[ÃfèüÿÿÿU¡    1Ò‰å]Ã´&    ´&    èüÿÿÿU‰åWVët& ó‹    öÂuó‹5    ‹=   ‹    9Ñuß‰ğ‰ú^_]Ãt& èüÿÿÿU¹   ºÓMbW¿€w V¾ Ù Só¥‰Çˆô  ‰ÆÁÿÁæÇ¬w »Ñ  ¤Ç‰ÈÁùƒ¸w ÷ê‰Ğ‰ÓÁøÁû)È)Ë™ğú1ö‰Á…Òt‰Ğ1Ò÷ó‰Æ‰È÷ó‰ò‰ÁÑè1Ò  ÊšƒÒ;1í‰Æ…Òt‰Ğ1Ò÷ñ‰Å‰ğ÷ñ‰ê1ÉÁàº   £Œw ¸€w èüÿÿÿ1À[^_]ÃffffèüÿÿÿÃ´&    v èüÿÿÿWV‰ÖS‹XD‹Bt\ÇÿÿÿÿÆC ‹~‹6‰ø	ğt9¸ÿÿÿÿë‹º    èüÿÿÿ‰;    r€{ u4ÇÿÿÿÿÆCƒÆÿƒ×ÿ‰ø	ğuÎ‰Ø[^_Ã´&    èüÿÿÿ‰C‰Së—v 1Û‰Ø[^_Ã´&    èüÿÿÿ‹D$‹T$…ÀtL$èüÿÿÿÃt& D$PRèüÿÿÿXZÃfèüÿÿÿU‰Å‰ÑWVSƒì<‹D$P‰T$8‹T$T‰D$,¸        ‰T$0‰D$4‰ÆQhÓÑ  Uèÿÿÿ‰ğƒÀ8‰D$ƒÄÇD$(    ‰,$´&    ÿt$(hÜÑ  ‹|$Wè]ÿÿÿ‹t$VhèÑ  WèMÿÿÿÿvhüÑ  Wè?ÿÿÿƒÄ$ÿ5    hÒ  Wè+ÿÿÿÿvh(Ò  Wèÿÿÿÿv$ÿv h<Ò  WèÿÿÿƒÄ(hVÒ  Wèşşÿÿ‹D$4F ‰D$‹‹T$8V$1í1Û‰T$èüÿÿÿ‹N‰D$X‰ÎZ‹|$‹…ö„ô   t& ‹V‹N‹~‰T$‹V‰L$ ‹N‰T$‹T$‰L$¶N$ˆL$'èüÿÿÿWVShfÒ  ‹\$Sè~şÿÿ¶|$;WhwÒ  SèmşÿÿƒÄ h€Ò  ‰\$Sè[şÿÿ‹L$$‹\$(‰È‰Ú+D$T$RP‹t$$‹|$(‰ğ+D$‰úT$ ƒÅRPS1ÛQWVhDJ ÿt$,èşÿÿ‹|$4ƒÄ0‹èüÿÿÿ‹w‰D$…í„)ÿÿÿ…ö„!ÿÿÿ‰ğèüÿÿÿƒÃ…Àt9ërğ‹|$‰Æ‹…ö…ÿÿÿ‹T$èüÿÿÿƒD$(‹D$(ƒD$(ƒø…@şÿÿ‹,$‹t$4ÿv$ÿv h‚Ò  hÒ  Uè“ıÿÿ¶F1ÒRƒàPh¤Ò  h°Ò  Uèxıÿÿ‹F1ÒƒÄ(RPh¿Ò  h°Ò  Uè^ıÿÿ·F1ÒRPhÉÒ  h°Ò  UèFıÿÿ·F1ÒƒÄ(RPhÔÒ  h°Ò  Uè+ıÿÿ‹F1ÒRPhİÒ  h°Ò  Uèıÿÿ‹D$`ƒÄ(èüÿÿÿ1Ò‰Ã‹@,RPhëÒ  h°Ò  Uèïüÿÿÿs8ÿs4hõÒ  hÒ  UèÙüÿÿ¶C01ÒƒÄ(ĞèRƒàPhÿÒ  h°Ò  Uè¹üÿÿ‹CD1ÒRPhÓ  h°Ò  Uè¢üÿÿ‹CH1ÒƒÄ(RPhÓ  h°Ò  Uèˆüÿÿ‹CL1ÒRPh$Ó  h°Ò  UèqüÿÿƒÄ(ÿsTÿsPh0Ó  hÒ  UèXüÿÿÿs\ÿsXh?Ó  hÒ  UèBüÿÿƒÄ(ÿsdÿs`hMÓ  hÒ  Uè)üÿÿÿslÿshh[Ó  hÒ  UèüÿÿƒÄ(ÿstÿsphjÓ  hÒ  Uèúûÿÿ‹Cx1ÒRPh{Ó  h°Ò  UèãûÿÿƒÄ(ÿ³   ÿ³Œ   hˆÓ  h°Ò  UèÄûÿÿÿ³˜   ÿ³”   h“Ó  hÒ  Uè¨ûÿÿ¡    1ÒƒÄ(RPh Ó  Uè‘ûÿÿ‰l$`ÇD$d€Ò  ƒÄL[^_]éyûÿÿ´&    fèüÿÿÿWV‰ÖS‹|$‰ÃQhÀÓ  PèTûÿÿƒÄ…ÿˆ  WhíÓ  Sè=ûÿÿƒÄhÔ  Sè/ûÿÿXZ…ö„u  ÿv`hÔ  Sèûÿÿÿvÿvh"Ô  Sèûÿÿÿv ÿvh9Ô  SèõúÿÿƒÄ,ÿv$hPÔ  Sèäúÿÿÿv(heÔ  SèÖúÿÿÿv,hzÔ  SèÈúÿÿƒÄ$ÿvÿvhÔ  Sè´úÿÿÿvh«Ô  Sè¦úÿÿ‹FDƒÄ…ÀtPhÁÔ  SèúÿÿƒÄ‹F8…ÀtPh×Ô  SèzúÿÿƒÄ‹F<…ÀtPhíÔ  SèdúÿÿƒÄ‹F@…ÀtPhÕ  SèNúÿÿƒÄ‹FH…ÀtPhÕ  Sè8úÿÿƒÄÿ6h0Õ  Sè(úÿÿh€Ò  Sèúÿÿÿv4hFÕ  SèúÿÿƒÄ …ÿyh€Ò  SèıùÿÿXZ[^_Ã´&    ‰øèüÿÿÿº¹Ó  …Àt‹P`Rh\Õ  SèÑùÿÿƒÄëÄt& hÛÓ  Sè½ùÿÿYXé|şÿÿ¶    hÔ  Sè¥ùÿÿY[[^_Ã´&    ´&    èüÿÿÿS‰Ãèüÿÿÿ‹Hjÿ‹‰ØèşÿÿèüÿÿÿPÿ5    hoÕ  Sè^ùÿÿèüÿÿÿPÿ5    hxJ SèGùÿÿƒÄ$h€Ò  Sè9ùÿÿXZ[Ãt& èüÿÿÿWVS‹2‰Ã¶Bƒşÿt;„Àt‰ğèüÿÿÿ‹HV‹‰Øè“ıÿÿX1À[^_Ãt& ÿr‰Øÿr‰òèùÿÿ1ÀZY[^_Ã„Àt‰ØèEÿÿÿ1À[^_Ã´&    ‹r‹zhŠÕ  Sè·øÿÿjh¤Õ  SèªøÿÿWVhÁÕ  SèøÿÿƒÄ$h€Ò  Sèøÿÿ1À[^[^_Ã´&    èüÿÿÿS‹XDº    ƒƒQ ‹èüÿÿÿ‰;    r€{ uÇÿÿÿÿÆC‰Ø[Ã1ÛëøfèüÿÿÿWVSèüÿÿÿhŠÕ  ‰Æ‰×j è#øÿÿjh¤Õ  j èøÿÿWVhÁÕ  j èøÿÿƒÄ$h€Ò  j èø÷ÿÿ[»ÿÿÿÿXë´&    W‰Ú1ÀVèøÿÿZY‰Øº    èüÿÿÿ‰Ã;    rİ1À»ÿÿÿÿè)şÿÿë´&    ‰Øèüÿÿÿ‹HS‹1Àè,üÿÿX‰Øº    èüÿÿÿ‰Ã;    rÕ[^_ÃèüÿÿÿUW‰ÏVSƒì(d‹    ‰L$$1Ét$‹\$<¹€Q VèüÿÿÿY|$‰Æ‰ù‰œ  v ƒîÁ€Q xõ‰ÈºÅ³¢‘½   ÷âÁê‰SiÒ  )Ñº‰ˆˆˆ‰È÷âÁê‰SkÒ<)Ñº“$I’‰N‰È÷ê‰ÈÁøÊÁú)ÂÕ    )Ğ‰ÊL$)ÂBHĞ‰ğ‰S™Áà¤òb Ò   €‰D$‰T$ Qj h±: èüÿÿÿ‰Á‰Ö‹D$(‹T$,köd¬ĞºaÛ, D… 1í÷â‰l$‰Å‰T$‰×ºËÿO[‰è÷â¸d   Áê‰T$÷á‰D$‹D$‰T$‹T$t$`…A^Ò1’éÿ‰D$‰T$ƒÄ…ÿ„¢   ƒç”D$iD$]  1É ‰Â·ÀÁêıÇçxÖ—Á‰Ît$4$iÀqz  ÁèƒÀıÇçxÖvmƒê¹Îşÿÿîl  ·ÀL$‰S‰s‰C‰K‹D$$d+    uIƒÄ([^_]Ã´&    é€Q ƒÆùQ ïéeşÿÿ´&    v ƒá”D$éYÿÿÿv ¶L$ƒÁ;ë‘èüÿÿÿèüÿÿÿV‰ÖS‰Ã‰‰ğ‹èüÿÿÿ‰C‹D$‰S‹T$‰C1À‰S‹N1ÒÇC    öÁ ÇC     ”À•ÂÓàÓâƒÀÿƒÒÿ‰C‰S[^Ãt& èüÿÿÿU‰åWVS‰Ãƒäøƒì‹ ‹èüÿÿÿ‹3‹{‰$+C‰T$S‰Ñ#F#N÷f¯NÊ‹KCS !×!Á‰{ ‹|$‰K‹N1ö‰{­ĞÓêöÁ EÂEÖ‹4$CS‰C‰s‰Seô[^_]Ã´&    v èüÿÿÿU‰åWVS‰Ã‰Ğ‰Êƒäøƒì0‹‹s‹{‰$‰t$ ‹q‰|$$‹y‰t$(‹t$ ‰|$,‹|$$)ğ‰T$‰Æ‹D$(ú‹{#T$,!ğ‹s‰|$‰t$‹s ‹[‰t$‰\$‹Y‹I‰Ş‰L$‹L$(‹\$,¬ÙÑë9ÁÓs`‹D$(‹T$ +$‹L$$L$!Ğ‹T$,!Ê¶L$‰Ó÷æ¯ŞÚ+D$T$1ö­ĞÓêöÁ EÂEÖ‰Æ‹D$‰×‹T$)ğúeô[^_]Ã´&    ¯Ö¶L$‰Ó÷æÚD$T$1Û­ĞÓêƒá EÂEÓD$T$eô[^_]ÃfffffèüÿÿÿUWVS‹    …Ût)‰Ö‰Ï´&    f‹C‹+ƒÃÿt$‰ò‰ùèüÿÿÿ‹X…Òuä[1À^_]Ã´&    ¶    èüÿÿÿUWVS‹    ‹t$‹|$…Ût‰Õv ‹C‹ƒÃW‰êVèüÿÿÿ‹XZ…Éuç[1À^_]ÃèüÿÿÿUWVS‹    ‹t$‹|$…Ût‰Õv ‹C‹ƒÃW‰êVèüÿÿÿ‹XZ…Éuç[1À^_]ÃèüÿÿÿUWVS‹    ‹t$‹|$…Ût‰Õv ‹C‹ƒÃW‰êVèüÿÿÿ‹XZ…Éuç[1À^_]ÃèüÿÿÿS¸ x èüÿÿÿ‹x ‰Â¸ x èüÿÿÿ‰Ø[Ã´&    v èüÿÿÿU‰åWV‹p‹x‹@@@‹ÅŒx èüÿÿÿ)Æ×‰ğ^‰ú_]ÃfèüÿÿÿU‰åS‰Ë‰Ñ‹P`‹@\)ÈÚ‹]üÉÃt& èüÿÿÿóÃ´&    U‰åW‰ÏV‰ÖS‰Ãƒì‹S@d¡    ‰Eğ1À‰Uäèüÿÿÿ‹Uä…    ‹CD‰Uä…ÀtoMìUè¸   èüÿÿÿ…ÀtC‹M‹Uè‰J0¹`   f‰J4‹M‰j<ÇB8    ˆH‹Mì‰p‰xj ÿuäRº   j jSèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãt& ‹…Àu‹ëàèüÿÿÿ´&    t& U‰åW‰×V‰ÆSƒì‹^@d¡    ‰Eğ1Àèüÿÿÿ…    ‹FD‰]ä…À„ƒ   MìUè¸$   èüÿÿÿ…ÀtX‹M‹Uè‰J0¹`   ‰j<f‰J4ÇB8    ‰x‹O@ˆH‹O‹_‰H‹M‰X‹]‰H‹Mì‰Xj ÿuäRº$   j jVèüÿÿÿƒÄ‹Eğd+    ueô[^_]Ãv ‹…À…sÿÿÿëİèüÿÿÿ´&    W‰ÏV‰ÖS‰Ãƒìd¡    ‰D$‹C$©À  u@¹   ‰Ú‰àèüÿÿÿ…Àt‹T$,‰p‰xˆP‰àèüÿÿÿ‹D$d+    uƒÄ[^_Ã¶    öÄu»‰Øèüÿÿÿ„Àt°ëÔèüÿÿÿ´&    fV‰ÆS‰Óƒìd¡    ‰D$‹F$©À  uS¹    ‰ò‰àèüÿÿÿ…Àt*‰X‹S@ˆP‹K‹[‰H‹L$(‰X‹\$,‰H‰X‰àèüÿÿÿ‹D$d+    uƒÄ[^Ãt& öÄu¨‰ğèüÿÿÿ„Àtë×èüÿÿÿ´&    fU‰ÊWVS‰Ã‹¨x   èüÿÿÿƒøt[^_]Ãv ¶M‹uºÓÕ  ‹}h ´  Óà‰ÁC@èüÿÿÿWVPhœJ Sèüÿÿÿƒ`  ƒÄ[^_]éüÿÿÿU‰ÊWVS‰Ãƒì‹¨x   èüÿÿÿƒøtƒÄ[^_]Ãt& ‹M‹U‹u‹}‰L$¶M‰$ºÓÕ  hà³  Óà‰ÁC@èüÿÿÿWVÿt$ÿt$PÿuhÀJ Sèüÿÿÿƒ`  ƒÄ,[^_]éüÿÿÿW1ÿV‹t$WVèüÿÿÿXZ^_Ã´&    t& 1Ééüÿÿÿ´&    fèüÿÿÿW‰Ï1ÉV‰ÖRS‹Õ”x ‰Ã@èüÿÿÿ‰{<‰s@‰ÇC0 l ÇCD    [^_Ãt& èüÿÿÿW|$ƒäøÿwüU‰åWV‰ÆSƒì‰Eä‰Ğ‰Ê‰EØ‰Á‹‰Ó‰UÜ‹W‰ß‰Eè‹F‰Uì‹V‰Î)Æ×…ÿx?‰û;uè}ì‰ñ¿    ¾   }Aÿuìÿuèèüÿÿÿ‹]ä‰C‰SXZeô‰ğ‰ú[^_]gø_Ãv eô1ö1ÿ[‰ğ‰ú^_]gø_Ãt& ‹uì…öxQÿuì‰È‰Úÿuèèüÿÿÿ‰Æ‰×‹Eè‹Uì¯Ç¯Ö‹Eè÷æÚ‹]äCS‰C‰SY‹MØ[‹]Ü9ÁÓ|ˆƒÆƒ× égÿÿÿ¶    èüÿÿÿU‰åW‰ÏV‰ÖS‰Ã‹@@@‹ÅŒx èüÿÿÿW‰Ñ‰Â‰ØVèüÿÿÿY[eô[^_]Ã¶    èüÿÿÿUƒÀP‰åWV‹pì‹xğQR‰ò‰ùèüÿÿÿY^eø^_]Ã´&    fèüÿÿÿ‹PHÇ@H    …Òt‰Ğèüÿÿÿ1ÀÃv U¸   ‰å]éüÿÿÿfèüÿÿÿW|$ƒäğÿwüU‰åWVS‰Ãƒì,d¡    ‰Eä¸   èüÿÿÿ‰Ñ‰ÂEÈèüÿÿÿ‹EÈd‹    ‰‹EÌ‹3‰C‹EĞ‹{‰C‹’¤  ‹Rr$z(B,‰ù™R‰òPEØèüÿÿÿ‹EØ‹UÜ‰‹Eà‰S‰CXZ‹Eäd+    ueô[^_]gø_Ãèüÿÿÿ´&    U1À‰å]éüÿÿÿt& èüÿÿÿUWVS‰Ãƒì8‹x d¡    ‰D$41À…É…è   ‹ƒØ  ¨„ê   ‹C$½ÿÿÿÿö@xtp‹œ   …Òtf|$1À¹   ‰\$ó«D$ÇD$íÕ  ÇD$şÿÿÿèüÿÿÿ‰Æ= ğÿÿwº   Fèüÿÿÿ¸ x èüÿÿÿ‰Â¡x …Àt-¸ x èüÿÿÿ‰ğèüÿÿÿ‹D$4d+    ufƒÄ8‰è[^_]Ãt& ‹ƒÀ  ‰$èüÿÿÿ‹$„Àt¾‰Ø‰x 1ö1íèüÿÿÿ‹$ë¨t& ¸ x èüÿÿÿ‰Âë•f½ğÿÿÿë´&    f½ÿÿÿÿëèüÿÿÿt& èüÿÿÿUWVS‰Ãƒì‹x@Áâª€x ‰T$‰èèüÿÿÿ‰$Cèüÿÿÿ‰Æ…Àyw‹$‰èèüÿÿÿ‹ÅŒx èüÿÿÿ>t& ƒÄ‰ğ[^_]Ã´&    d‹    £    sßdÿ    ‹    …Ét‹IR‰ÚP‰ÈèüÿÿÿXZdÿ    uµèüÿÿÿë®´&    öCDtƒ‹T$‚„x ‰ÚèüÿÿÿƒcDşéiÿÿÿèüÿÿÿS‰Ãë¶    ó‰Øèüÿÿÿ…Àxó[ÃèüÿÿÿƒÀPéÿÿÿv èüÿÿÿUWVS‰Ãƒèƒì‹s,‰$vÁà¸€x ‰D$‰øèüÿÿÿ‰ÅöC0t‹D$‹$„x èüÿÿÿƒc0ş‰ê‰øèüÿÿÿ‹k(…í„á   v‹ÅŒx èüÿÿÿ‰Ñ‰Â‹$èüÿÿÿ‰Å‰øèüÿÿÿ‰Á…ítA‹Cø‹Sü‹l$‰C‰SÅ„x ‰C‰SöC0…¡   ‹$‰è‰L$½   èüÿÿÿƒK0‹L$‰ø‰Êèüÿÿÿv‹ÅŒx èüÿÿÿ>t& ƒÄ‰è[^_]Ã´&    d‹    £    sßdÿ    ‹    …Ét‹IRP‹T$‰ÈèüÿÿÿXZdÿ    u³èüÿÿÿë¬t& ‰øèüÿÿÿ‰Áé|ÿÿÿf‹$‰è‰L$èüÿÿÿ‹L$éHÿÿÿ´&    fèüÿÿÿUWVS‰Ã‹@@4@Áæ®€x Æ„x ‰èèüÿÿÿ‹S‹Kj ‰ÇCj ‰S j ‰K$‰S(‰K,èüÿÿÿƒÄöCDu‰Ú‰ğèüÿÿÿƒKD‰ú[‰è^_]éüÿÿÿt& ‰Ú‰ğèüÿÿÿë×t& èüÿÿÿUW‰ÏV‰ÖS‰Ãƒì‹h@Tm Áâ‚€x ‰T$‰$èüÿÿÿ‹T$‰s‰D$‰{²„x öCD…œ   ‰Ú‰ğsèüÿÿÿƒKD‹C‹Sj j j ‰Ñ‰Â‰ğèüÿÿÿ‹T$‹D$èüÿÿÿDm ‹ÅŒx èüÿÿÿƒÄ>t& ƒÄ[^_]Ãd‹    £    sèdÿ    ‹    …Ét‹IR‰ÚP‰ÈèüÿÿÿXZdÿ    u¾èüÿÿÿë·´&    ‰Ú‰ğèüÿÿÿéVÿÿÿ´&    v èüÿÿÿW‰ÏV‰ÖS‰Ã‹@@@‹ÅŒx èüÿÿÿR‰úP‰ğèüÿÿÿY^‰Ñ‰Â‰Ø[^_éÅşÿÿt& èüÿÿÿUW‰ÏV‰ÖS€|$ ‰Ã‹l$u&‹€   @‹ÅŒx èüÿÿÿR‰úP‰ğèüÿÿÿ‰Æ‰×XZ‰è„Àt‰s\‰{`[^_]Ã´&    CP‰ò[‰ù^_]éXşÿÿ´&    èüÿÿÿW|$ƒäğÿwüU‰åWVS‰Ãd‹5    ƒì<‰UÀ‹?‰MÄd¡    ‰Eä1À‰sH´&    ¸   ‡F‹UÀ‹MÄ‰Øèüÿÿÿ‹CH…Àtèüÿÿÿët& ó‰Øèüÿÿÿ…Àxó‹CH…Àt‹©   u‹¨tµt& d¡    Ç@    ‹[H…Û„“   ‹    …Ò…‰   d‹5    ‹†D  …À„½   ‹ÅŒx èüÿÿÿ‹MÀ‹]Ä)ÁÓ1À9ÈØ¸    |‹Uäd+    …   eô[^_]gø_Ã‰ÊEÈ‰Ùèüÿÿÿ‹EÈUØ‰EØ‹EÌ‰EÜ‹EĞ‰Eà†8  èüÿÿÿë¶1Àë²èüÿÿÿ„À„jÿÿÿ…ÿ„¾   ƒÿt9€=¡    …PÿÿÿWhøÕ  Æ¡   èüÿÿÿZYé5ÿÿÿ´&    ¸üıÿÿé_ÿÿÿ¸˜x ¾   ‹@èüÿÿÿ‹MÀ‹]Ä)Á¸Hx Ó‰M¸‰]¼èüÿÿÿ‹M¼‹Px ‰Ã¡Tx 9U¸Á|	Ğu(‹E¸‹U¼‰5`x £Px ‹EÀ‰Tx ‹UÄ£Xx ‰\x ‰Ú¸Hx èüÿÿÿé¤şÿÿ¸€x ¾   é{ÿÿÿèüÿÿÿ´&    v èüÿÿÿS¸ x èüÿÿÿ‹x ‰Â¸ x èüÿÿÿ…Ûtº x ‰Øèüÿÿÿ1À[Ã´&    v èüÿÿÿUhPWVS‰Ã‹p<‹x@‹€   @‹ÅŒx èüÿÿÿW‰Ñ‰Â‰èVèüÿÿÿC$XS(‹C\‹K`Z‰Â[‰è^_]éûÿÿ´&    ´&    èüÿÿÿV¸ x S‰Óèüÿÿÿ‹5x ‰Â¸ x èüÿÿÿ…ötÇ    ¡    ÇC    ‰C1À[^Ã¸êÿÿÿëö¶    èüÿÿÿWV‰ÖS1Ûƒøt1Ûƒø	”Ã\ÿ¸ x èüÿÿÿ‹=x ‰Â¸ x èüÿÿÿ…ÿt[‹Åx ‰ğèüÿÿÿ1À[^_Ã¸êÿÿÿëõt& èüÿÿÿU‰åVS1Ûƒøt1Ûƒø	”Ã\ÿ¸ x èüÿÿÿ‹5x ‰Â¸ x èüÿÿÿ…öt[[^‹ÅŒx ]éüÿÿÿ[¸êÿÿÿºÿÿÿÿ^]Ãv èüÿÿÿWVS‰Ã¸ x èüÿÿÿ‹5x ‰Â¸ x èüÿÿÿ…ötu¸#   èüÿÿÿ„Àt`‹C1öƒøtƒø	”À¶Àt ÿvCd1É‹Õ”x {Pèüÿÿÿ‰{P1ÀÇƒ€    l ÇƒŒ   Ğx ‰³   Çƒ”       [^_Ãt& ¸ÿÿÿÿëğ¸¡ÿÿÿëé´&    v èüÿÿÿU‰ÕW‰ÇV‰ÎS1ÛƒìX‰D$d¡    ‰D$T1Àƒÿt1Ûƒÿ	”Ã\ÿd¡    ‰$¸ x èüÿÿÿ‹=x ‰Â¸ x èüÿÿÿ…ÿ„†  ÷Åşÿÿÿ…B  ¸#   èüÿÿÿ„À„@  [1É‹Å”x D$èüÿÿÿ‹Nº}Á%D$ÇD$8 l ‰D$‹ÇD$D i ‹v9Âº   ‰\$HÊÇD$L    ŒË   º Êš;iÉ Êš;1ÿ÷âÊÆ×ƒıty[‹ÅŒx èüÿÿÿWVèüÿÿÿS‰×‰Æ‰Â‰ùD$èYúÿÿƒÄ=üıÿÿt‹T$Td+    …À   ƒÄX[^_]Ã¶    ‹$‰™@  ‰±L  ‰¹P  Ç<  ğ]  ëÀ´&    d¡    ‹€¤  ‹@=    uRS‰ò‰ùD$èäùÿÿZºşıÿÿ=üıÿÿDÂë…t& ¾ÿÿÿÿ¿ÿÿÿé;ÿÿÿ¸êÿÿÿégÿÿÿ¶    ¸ÿÿÿÿéWÿÿÿ¶    ƒÀ‰ù‰òP‹D$èüÿÿÿY‰Æ‰×ë–¸¡ÿÿÿé/ÿÿÿèüÿÿÿ´&    fèüÿÿÿU½€x WVSƒìH‰D$d¡    ‰D$D¸Hx èüÿÿÿ‹Px ‹Tx ÇPx     ÇTx     ‰Â¡`x ‰$‹Xx ‰\$‹\x ‰D$¸Hx ‰L$‰\$èüÿÿÿ¸ x èüÿÿÿ‹=x ‰Â¸ x ‰|$èüÿÿÿÇD$    …ÿ„‡  ‰èèüÿÿÿ‹]‰Â‰èèüÿÿÿ…ÛtD‹s‹{‹Eèüÿÿÿ)Æ‹$×‹T$‰Á	Ñt9Æ‰ùÑ}‹C‹S‰4$‰|$‰D$‹D$‰T$‰D$ƒÅƒ|$…¶   ‹l$‹<$‰è	ø„  ¸ÿ“5w9ø¸    è  >t& ‹t$º x ‰ğèüÿÿÿ‰ğT$ èüÿÿÿÿt$@ÿt$@ÿt$@ÿt$@ÿt$@ÿt$@ÿt$@ÿt$@ÿt$@èüÿÿÿƒÄ$j j D$T$Rº x P‰ğèüÿÿÿ‰ÃƒÄ…Àˆ–   ‹D$Dd+    …´   ƒÄH‰Ø[^_]Ãv ÇD$   éÔşÿÿv d¡    £    ƒQÿÿÿdÿ    ¡    …Àt‹@ÿt$‹T$‹L$èüÿÿÿXdÿ    …ÿÿÿèüÿÿÿéÿÿÿ´&    f1Ûésÿÿÿ´&    f‹D$1Éºè  èüÿÿÿéUÿÿÿ‹D$1ÉºĞ  »ğÿÿÿèüÿÿÿé;ÿÿÿèüÿÿÿ´&    t& èüÿÿÿUh°WxÀVS‰Ã‰øƒìèüÿÿÿÇCĞ    ‰Æ‹CğCìtD‹CäP‰Sä‰è1íèüÿÿÿ…Àt‹Sğ‹Cì‰Ñ‰$	Á‰T$u#‰ø‰òèüÿÿÿ‰èƒÄ[^_]Ã´&    1ÒëÁt& ‹C@½   @‹ÅŒx èüÿÿÿÿt$ÿt$‰Ñ‰Â‰ØèüÿÿÿÇCĞ   CÔXSØZƒCäëffffèüÿÿÿÇ    ¡    ÇB    ‰B1ÀÃv èüÿÿÿU‰åS‰Ë‰Ñ‹P`‹@\)ÈÚ‹]üÉÃt& èüÿÿÿóÃ´&    èüÿÿÿS‰Ã‹@ÇC<    ÇC@    ‹P4‰Øèüÿÿÿ…ÀxÇC     1À[Ã¶    ¸   [Ãèüÿÿÿ‹P1ÉƒÀPèüÿÿÿ1ÀÃ´&    t& èüÿÿÿU‰åWSXPÿp@ÿp<‰ØèüÿÿÿY[eø[_]Ã´&    ¶    èüÿÿÿƒÀPéüÿÿÿv èüÿÿÿU‰Õº}Á%WVS‰Ã‹‹y‹q9Âº   ú|+º Êš;iÏ Êš;1ÿ÷âÊğúS‰é÷ÑƒáèüÿÿÿZ[^_]Ãv ¸ÿÿÿÿºÿÿÿëŞt& èüÿÿÿU¸   ‰å]éüÿÿÿ´&    ¶    èüÿÿÿU¸   ‰å]éüÿÿÿ´&    ¶    èüÿÿÿU1À‰å]éüÿÿÿèüÿÿÿU‰åS‰Óƒäğƒì d¡    ‰D$¸   èüÿÿÿ‰Ñ‰Â‰àèüÿÿÿ‹$‰‹D$‰C‹D$‰C‹D$d+    u‹]ü1ÀÉÃèüÿÿÿt& èüÿÿÿU1É‰åS‰Óº@B ƒäğƒì d¡    ‰D$1À‰àèüÿÿÿ‹$‰‹D$‰C‹D$‰C‹D$d+    u‹]ü1ÀÉÃèüÿÿÿ´&    fèüÿÿÿU‰åWVS‰Ãƒäğƒì0‹K@‹s‰T$‹S<d¡    ‰D$,1À‹C8‰D$‰È	Ğuw‹C …Àuƒ|$uG‹C‹Vèüÿÿÿ‰$‰T$‹L$‹v0‰Ø‹$èüÿÿÿ‰Ñ1Ò|$9ÂÊŒ´   ƒ|$t‹D$Ç@   ‹D$,d+    …¸   eô[^_]Ã´&    fD$‰Çèüÿÿÿ‹D$‹L$‰‹D$‰A‹D$‰A‹C‹Vèüÿÿÿ‰$‰T$öC4uƒ|$u‹$‹L$‰Ø‹~,èüÿÿÿC$S(éKÿÿÿf‹L$‹v0‰Ø‹$èüÿÿÿ‰Ñ1Ò9ÂÊTÿÿÿ‰Â‰øèüÿÿÿ‹D$‹|$‰G‹D$‰G‹D$‰Gé7ÿÿÿèüÿÿÿt& èüÿÿÿUWV‰ÎS‰Ãƒì‹h‰$‹T$ …ÒtèüÿÿÿÇC<    ‰ØÇC@    ‹U4èüÿÿÿ‰Â¸   …ÒˆÖ   ‹C4ÇC     ÇC,    ƒÀÇC0    ƒàş‰C4‹FF„¹   ‹¸}Á%‹N‹~9Ğ¸   ÈŒå   ¸ Êš;iÉ Êš;‰|$÷âÇD$    ÊD$T$‰S@º}Á%‰C<‹F‹N‹v9Âº   ÊŒ·   º Êš;iÉ Êš;1ÿ÷âÊğú‹<$ƒçuR‹s81Éƒş”ÁQ‰Ñ‰Â‰ØW‹}8èüÿÿÿX1Àƒş•ÀZ‰C 1ÀƒÄ[^_]Ã´&    ‹F…À…<ÿÿÿƒÄ[^_]Ãt& d‹    ‹‰¤  ‹Iù    t–ƒÁ‹sQ‰Ñ‰Â‰ğèüÿÿÿYëv ¸ÿÿÿÿºÿÿÿé/ÿÿÿ´&    f¸ÿÿÿÿºÿÿÿéOÿÿÿèüÿÿÿ‰Ğèüÿÿÿ1ÀÃèüÿÿÿU‰å]éüÿÿÿfèüÿÿÿWVS‰Óƒìd¡    ‰D$‰Ğèüÿÿÿd¡    ‹€¤  ‹@‹p‹x3‹@ {C™‰ùR‰òPD$èüÿÿÿ‹D$‹T$‰‹D$‰S‰CXZ‹D$d+    u	ƒÄ1À[^_Ãèüÿÿÿèüÿÿÿ‰Ğéüÿÿÿt& èüÿÿÿ‰Ğèüÿÿÿ1ÀÃèüÿÿÿ‰Ğ1Òéüÿÿÿfèüÿÿÿdÿÿÿ¡à€ éüÿÿÿ´&    t& èüÿÿÿS‰Ã…Òu$‹CHèüÿÿÿ‹CLèüÿÿÿƒœ   ºÀ [éüÿÿÿv ¸Àx èüÿÿÿ‹K‰Â‹C‰…Àt‰HÇC"  ¸Àx èüÿÿÿë®fèüÿÿÿU‰ÍW‰×VS‰Ãƒì‹S‹t$ ‹D$$‰ñ‰D$CPƒñ‰$¶Á‰D$…Òu‰ğ¹`¶  „À¸ ¶  DÁ‰C‹$‹L$èüÿÿÿ‰ğÇCl€Š „Àu‹Cp‹@èüÿÿÿR‰êP‰øèüÿÿÿ‰Ç‰ÕXZ€|$ ‰{\‰k`‰{d‰khtƒÄ[^_]Ãt& j ‰ú‰éj j ‹D$èüÿÿÿƒÄƒÄ[^_]ÃfèüÿÿÿWVS1Û…Àxw‰Ç‰Öèüÿÿÿd¡    ‹¨  ‰Ğ1øiÀG†ÈaÁè‹…àx …ÀuëAv ‹C…Àt7ƒè‰Ãt0;PDuí;xuèx‰øèüÿÿÿ‰‰Âd¡    ‹€¨  9CDt‰øèüÿÿÿèüÿÿÿ1Û‰Ø[^_Ãt& èüÿÿÿ‰Ø[^_Ãt& èüÿÿÿUWV‰ÖS‰Ã‹h‹xèüÿÿÿ‹Cèüÿÿÿ‹U<…Òt‰Øèüÿÿÿèüÿÿÿ‰ò[‰ø^_]éÿÿÿ¶    ëät& èüÿÿÿV‰ÖSƒìd‹    ‰T$‰âèòşÿÿ…ÀtW‰Ã1Ò1À‰ƒÀƒørõ‹C…Àt6‹H$…Ét/‰ò‰Ø1öèüÿÿÿ‹$Cèüÿÿÿ‹D$d+    uƒÄ‰ğ[^Ãt& ¾êÿÿÿëÓ¾êÿÿÿë×èüÿÿÿ´&    t& èüÿÿÿU‰ÍW‰×VS‰Ãƒìd¡    ‰D$1À‹L$ …Ét1Ò‰ƒÀƒørõ‰ØT$‰$è?şÿÿ‹$…À‰Ãu4ëdt& ‹p…öt.Q‰ú‰é‰Øèüÿÿÿ‰ÆXƒşu!‰ØT$èªşÿÿ1É‰Ã…Àt2‹C…ÀuË¾êÿÿÿ‹T$Cèüÿÿÿ‹D$d+    uƒÄ‰ğ[^_]Ã¾êÿÿÿëáèüÿÿÿt& èüÿÿÿW|$ƒäğÿwüU‰åWVS‰Óƒì,d¡    ‰Eä¸   èüÿÿÿ‰Ñ‰ÂEÈèüÿÿÿ‹EÈd‹    ‰‹EÌ‹3‰C‹EĞ‹{‰C‹’¤  ‹Rr$z(B,‰ù™R‰òPEØèüÿÿÿ‹EØ‹UÜ‰‹Eà‰S‰CXZ‹Eäd+    ueô1À[^_]gø_Ãèüÿÿÿ¶    èüÿÿÿUhPWVS‰Ã‹p<‹x@‹@p‹@èüÿÿÿW‰Ñ‰Â‰èVèüÿÿÿC$‹CdS(‹Shj ‹K\‹[`)ÁÓSQ‰Ñ‰Â‰èèüÿÿÿƒÄ[^_]Ã´&    èüÿÿÿU‰Õº}Á%WVS‰Ã‹‹y‹q9Âº   ú|[º Êš;iÏ Êš;1ÿ÷âÊğúƒåuS‰éƒñèüÿÿÿZ[^_]Ãd‹    ‹‰¤  ‹Iù    t×ƒÁQ‰Ñ‰Â‰ØèüÿÿÿYëÅ¶    ¸ÿÿÿÿºÿÿÿë®t& èüÿÿÿUW‰ÇVSƒì$‰D$‰T$‰L$d¡    ‰D$ 1À…ÿˆ¬  ‹|$ƒÿÌ  ƒÿÀ!ø‹…à¶  ‰D$‹D$…À„®  ‹P…Ò„  ¡à€ ºÀ  èüÿÿÿ‰Å…À„  èüÿÿÿ‰EL…À„„  Ç@    PƒÀ1ÛÇ@    ƒàü)ÂJ 1Òƒáü‰ƒÂ9ÊröÇE    d¡    ‹˜¨  ‰l$‹CT‰$E‰D$f¸Àx èüÿÿÿ‹sT‰Ù1ñiÉG†ÈaÁé‹<àx ,àx …ÿ„  ‰øët& ‹@…À„ı  ƒè„ô  ;XDuç‹P9Öuà¾şÿÿÿƒÂ¸    HĞ‰ST9$t¸Àx èüÿÿÿƒşşt‹l$ëv ‹l$¸Àx ƒşş„Ğ  èüÿÿÿ‰t$…öˆ  ‹D$‹|$‰uÇE$ÿÿÿÿ‰E‹D$ÇE(ÿÿÿÿ‰E…ÿ„Ê  èüÿÿÿd‹5    ‹†¨  ‹˜  ‹Gƒø„(  ğ   …À„  ƒø…'  …Û„  ¸   ğÁ…À„_  P	Âˆ!  ‰]Hèüÿÿÿ‹EH…À„ÿ  ‹|$‹G‹W‰E8‹EL‰P‹EL‹‰P ‹EL‹U¹   ‰P‹ELT$Ç@şÿÿÿ‹D$èüÿÿÿ…À…ˆ  ‹D$‹P‰èèüÿÿÿº   ‰Æ…À…Ñ   d‹    ‹ƒ¬  èüÿÿÿ‹ƒ¨  ‰ED‹ƒ¨  ‹PX‰j‰U PX‰U‰hX‹ƒ¬  èüÿÿÿé•   1Ûƒø…:ÿÿÿ‹D$‹@èüÿÿÿ1Ò‰Ãèüÿÿÿ…Àt$‹¾¨  9¸¨  u‹D$‹@‰$ƒèƒø?†Ùşÿÿ1Ûéöşÿÿ‹D$‰x‰h‹D$‰àx …ÿt‹D$‰G‹sT‰òéóıÿÿèüÿÿÿ¾õÿÿÿ1ÒÇD$õÿÿÿ‰èè›÷ÿÿ‹D$ d+    …  ƒÄ$‰ğ[^_]Ã‹ELÇE8    Ç@   ‹ELÇ@     ‹EL‹U‰P d¡    ‹€¨  ‹˜  …Ût¸   ğÁ…À„   P	Âxu‰]Héişÿÿ‰øº    ƒàƒø¸    DÂ‰D$éUüÿÿº   ¾òÿÿÿéWÿÿÿ¾êÿÿÿéTÿÿÿ¡à€ ‰ê¾õÿÿÿèüÿÿÿé>ÿÿÿº   ¾êÿÿÿé(ÿÿÿº   ‰ØèüÿÿÿéÎıÿÿº   ‰Øèüÿÿÿézÿÿÿº   ‰Øèüÿÿÿéiÿÿÿº   ‰Øèüÿÿÿé›ıÿÿ¾¡ÿÿÿéáşÿÿ¾õÿÿÿé×şÿÿ1ÒéÉşÿÿèüÿÿÿ¶    èüÿÿÿWVS‰Óƒìd¡    ‰D$‰Ğèüÿÿÿd¡    ‹€¤  ‹@‹p‹x3‹@ {C™‰ùR‰òPD$èüÿÿÿ‹D$‹T$‰‹D$‰S‰CXZ‹D$d+    u	ƒÄ1À[^_ÃèüÿÿÿèüÿÿÿWVS‰Óƒìd¡    ‰D$‰Ğèüÿÿÿd¡    ‹€¤  ‹@‹p‹x3‹@ {C™‰ùR‰òPD$èüÿÿÿ‹D$‹T$‰‹D$‰S‰CXZ‹D$d+    u	ƒÄ1À[^_ÃèüÿÿÿèüÿÿÿUWxÀVS‰Ã‰øƒìèüÿÿÿ‹SğSìÇCĞ    ‰Æ„£   ‹CäƒÀ‰Cä‹Sü1É‰B$öCè‹Sø‹Cü”Á1íèüÿÿÿ…À~`‹CğCìtX‹C ‹@èüÿÿÿ¹?B ‰$‹Cì‰T$‹Sğ9ÁÕ|$@B ƒT$ R½   P‹T$‰Ø‹L$èüÿÿÿCÔXSØZƒCäÇCĞ   ‰ø‰òèüÿÿÿ‰èƒÄ[^_]Ã´&    1Àé_ÿÿÿ´&    fV¹Ğ   ‰ÖS‰ÃìÔ   d¡    ‰„$Ğ   1À‰àèüÿÿÿ…Àum…Ûxqƒûˆ   ƒûÀ!Ø‹…à¶  …Àtv‹H…Étv‰Ø‰âèüÿÿÿ‰Ã…Ày ‹„$Ğ   d+    u^ÄÔ   ‰Ø[^Ãt& ¹Ğ   ‰â‰ğèüÿÿÿ…ÀtÎ¶    »òÿÿÿëÁ‰Øº    ƒàƒø¸    EÂë´&    f»êÿÿÿë™»¡ÿÿÿë’èüÿÿÿ´&    ¶    W‰×V‰ÆSìÔ   d¡    ‰„$Ğ   ‰àèüÿÿÿ‰Ã…Àt)‹„$Ğ   d+    …™   ÄÔ   ‰Ø[^_Ã´&    f…öxLƒşgƒşÀ!ğ‹…à¶  …ÀtU‹H…ÉtX‰â‰ğèüÿÿÿ‰Ã…Àx¦‰â‰øèüÿÿÿ…À¸òÿÿÿEØë‘´&    v ‰ğº    ƒàƒø¸    EÂë°´&    f»êÿÿÿé]ÿÿÿ»¡ÿÿÿéSÿÿÿèüÿÿÿ´&    èüÿÿÿUWVS‰Ãƒìd¡    ‰D$‹C‰âè¾óÿÿ…Àt‰Å‹@@E<u ‹$Eèüÿÿÿ‹D$d+    uaƒÄ[^_]Ã‹C9E4uØ‹E‹P(‰èèüÿÿÿ‹E$‹U(ÇE    ƒE4‰E,‰U0ÇE$ÿÿÿÿÇE(ÿÿÿÿ‹s‰÷ÁÿÆ¸ÿÿÿ×1Ò9ğúLğ‰së‡èüÿÿÿfèüÿÿÿ‹HL‰Q$1É‹PHö@8‹@L”Áèüÿÿÿ…ÀŸÀ¶ÀÃ´&    èüÿÿÿVSƒìD‹‹pd‹    ‰T$@‹P…Òt?¹@   ‰àèüÿÿÿ‰Â¸òÿÿÿ…Òt‹T$@d+    u8ƒÄD[^Ãt& ‰ñ‰â‰Øè…öÿÿëÛv ‹D$@d+    uƒÄD‰ñ‰Ø1Ò[^écöÿÿèüÿÿÿ´&    ´&    èüÿÿÿSƒì‹X‹ d‹    ‰T$1Ò‰âè>óÿÿ…Àt‹T$d+    u!ƒÄ[Ã´&    ‰Ú‰àèüÿÿÿ…ÀtÙ¸òÿÿÿëÒèüÿÿÿ´&    èüÿÿÿSƒì‹X‹ d‹    ‰T$1Ò‰âèŞòÿÿ…Àt‹T$d+    u!ƒÄ[Ã´&    ‰Ú‰àèüÿÿÿ…ÀtÙ¸òÿÿÿëÒèüÿÿÿ´&    èüÿÿÿSƒì‹ d‹    ‰T$‰âè“ñÿÿ…Àt3‹P,¹ÿÿÿ1Û9ÑX0MÊ‹$ƒÀ‰Ëèüÿÿÿ‹D$d+    uƒÄ‰Ø[Ã»êÿÿÿëåèüÿÿÿèüÿÿÿUWVSƒì8‹X‹hd‹    ‰T$41Ò‹8‹Pt$1À…ÛEÆ‰$…ÒtZD$èüÿÿÿ…ÀuD|$ÿÉš;‹D$w?D$x9|$ÿÉš;w/ÿ4$‰ê‰øL$è_òÿÿZ…Àu…Ût‰Ú‰ğèüÿÿÿ…Àt¸òÿÿÿëf¸êÿÿÿ‹T$4d+    uƒÄ8[^_]ÃèüÿÿÿèüÿÿÿUWVSƒì8‹X‹hd‹    ‰T$41Ò‹8‹Pt$1À…ÛEÆ‰$…ÒtZD$èüÿÿÿ…ÀuD|$ÿÉš;‹D$w?D$x9|$ÿÉš;w/ÿ4$‰ê‰øL$è¯ñÿÿZ…Àu…Ût‰Ú‰ğèüÿÿÿ…Àt¸òÿÿÿëf¸êÿÿÿ‹T$4d+    uƒÄ8[^_]ÃèüÿÿÿèüÿÿÿVSƒì‹ d‹    ‰T$‰âèÒïÿÿ‰Ã…À„¨   ‹C…À„}   ‹P …Òtv‰Øèüÿÿÿƒøtrd‹5    ‹†¬  èüÿÿÿ‹‹C‰B‰Ç   ÇC"  ‹†¬  èüÿÿÿÇCD    ‹$Cèüÿÿÿ‰Øº   è8îÿÿ1À‹T$d+    u0ƒÄ[^Ãv ë’t& ‰Ø‰âèÏïÿÿ‰Ã…À…]ÿÿÿt& ¸êÿÿÿëÃèüÿÿÿt& èüÿÿÿUhXW‰ÇVS‹@X9ètfv ‹_Xsë‹P …Òt!‰Øèüÿÿÿƒøu‰ğèüÿÿÿ‰ğèüÿÿÿ‹C…ÀuØ‹‹C‰B‰‰ğÇ   ÇC"  èüÿÿÿ‰Øº   è|íÿÿ‹GX9èu[^_]ÃèüÿÿÿVSƒì‹d‹    ‰T$1Ò‹P…ÛxPƒûwkƒûÀ!Ø‹4…à¶  …ötY‹F…ÀtR‰àèüÿÿÿ…ÀuW‹N‰â‰Øèüÿÿÿ‹T$d+    uEƒÄ[^Ã´&    ‰Ø¾    ƒàƒø¸    Eğë¬´&    f¸êÿÿÿë¾´&    f¸òÿÿÿë®èüÿÿÿt& èüÿÿÿSƒì‹X‹ d‹    ‰T$1Ò…ÀxYƒøwlƒøÒ!Â‹•à¶  …ÒtZ‹J‰âèüÿÿÿ…Àt‹T$d+    uFƒÄ[Ã´&    v ‰Ú‰àèüÿÿÿ…Àt×¸òÿÿÿëĞt& ‰Â¹    ƒâƒúº    EÑë£¸êÿÿÿë­èüÿÿÿt& èüÿÿÿS‰Ó…Àx,ƒø?ƒøÒ!Â‹•à¶  …Òt-‹J…Ét-‰Ú[éüÿÿÿ¶    ‰Â¹    ƒâƒúº    EÑëĞ¸êÿÿÿ[Ã¸¡ÿÿÿ[Ãfèüÿÿÿ‹P‹ é1÷ÿÿèüÿÿÿSƒì‹X‹ d‹    ‰T$1Ò…ÀxYƒøwlƒøÒ!Â‹•à¶  …ÒtZ‹
‰âèüÿÿÿ…Àu…Ûu‹T$d+    uCƒÄ[Ã´&    ‰Ú‰àèüÿÿÿ…ÀtÚ¸òÿÿÿëÓt& ‰Â¹    ƒâƒúº    EÑë£¸êÿÿÿë°èüÿÿÿt& èüÿÿÿVSƒì‹d‹    ‰T$1Ò‹P…ÛxPƒûwkƒûÀ!Ø‹4…à¶  …ötY‹F…ÀtR‰àèüÿÿÿ…ÀuW‹N‰â‰Øèüÿÿÿ‹T$d+    uEƒÄ[^Ã´&    ‰Ø¾    ƒàƒø¸    Eğë¬´&    f¸êÿÿÿë¾´&    f¸òÿÿÿë®èüÿÿÿt& èüÿÿÿSƒì‹X‹ d‹    ‰T$1Ò…ÀxYƒøwlƒøÒ!Â‹•à¶  …ÒtZ‹J‰âèüÿÿÿ…Àt‹T$d+    uFƒÄ[Ã´&    v ‰Ú‰àèüÿÿÿ…Àt×¸òÿÿÿëĞt& ‰Â¹    ƒâƒúº    EÑë£¸êÿÿÿë­èüÿÿÿt& èüÿÿÿ‹P‹ éöÿÿèüÿÿÿSƒì‹X‹ d‹    ‰T$1Ò…ÀxYƒøwlƒøÒ!Â‹•à¶  …ÒtZ‹
‰âèüÿÿÿ…Àu…Ûu‹T$d+    uCƒÄ[Ã´&    ‰Ú‰àèüÿÿÿ…ÀtÚ¸òÿÿÿëÓt& ‰Â¹    ƒâƒúº    EÑë£¸êÿÿÿë°èüÿÿÿt& èüÿÿÿUWVSƒì‹‹pd‹    ‰T$1Ò‹x‹P…Ûˆ”   ƒûwoƒûÀ!Ø‹,…à¶  …ít]‹M…É„©   ‰àèüÿÿÿ…À…“   ‹D$…Àx;|$ÿÉš;w1÷Ç   ui1Ò…ö•Âd¡    ‰á‰D  ‰ú‰°H  ‰Ø‹uèüÿÿÿë¸êÿÿÿ‹T$d+    uLƒÄ[^_]Ã¶    ‰Ø½    ƒàƒø¸    Eèéeÿÿÿ¶    1Ò1öë˜´&    v ¸òÿÿÿë®¸¡ÿÿÿë§èüÿÿÿ´&    ¶    èüÿÿÿUWVSƒì‹‹pd‹    ‰T$1Ò‹x‹P…Ûˆœ   ƒûwwƒûÀ!Ø‹,…à¶  …íte‹M…É„©   ‰àèüÿÿÿ…À…“   ‹D$…ÀxC|$ÿÉš;w9…ötu÷Ç   umº   d¡    ‰á‰D  ‰ú‰°H  ‰Ø‹uèüÿÿÿë¶    ¸êÿÿÿ‹T$d+    uDƒÄ[^_]Ã¶    ‰Ø½    ƒàƒø¸    Eèé]ÿÿÿ¶    1ö1Òë’f¸òÿÿÿë¶¸¡ÿÿÿë¯èüÿÿÿffffffèüÿÿÿU‰åWVS‰Ã‰Ğ‰Êƒäøƒì ‹K<‹s\‹{`‰L$‹K@9ğ‰|$‰L$‰ÑL$‰t$‰|$‰t$‚ß   ‹L$‰Ï|$„Ï   ‹|$È‰Îú+D$T$‰D$)È‰T$ú9Á‰ùÑ¹    s4‰\$´&    ‹D$¤÷‹T$öƒÁ‰û)ğú9ÆÓrã‹\$…Éxsv ‹D$‹T$‰D$‹D$‰T$‹T$9ğ‰ĞørE‹D$‹T$ğú‰D$‰T$‰D$‰T$‰C\¸   ‰S`1Ò¥ÂÓàöÁ t‰Â1ÀC$S()t$|$¬şÑïƒés‹D$‹T$eô[^_]Ã´&    èüÿÿÿUWVS‰Óƒì‰$‹‹R‰×	ÇtD‹|$9D$‰Î×rF‹k‹{‰é	ùtÇÕ‰;‰k>t& ‹$‹D$ º   èüÿÿÿ‹S‹‰Ó	ÃuƒÄ[^_]Ã´&    ;‰Ó^sç‰‰VƒÄ[^_]Ã´&    v ‹$‹€¨  ‹ˆ  d¡    £    s–dÿ    ¡    …Àt‹T$ ‹@ÿt$ÿt$ƒêèüÿÿÿXZdÿ    …`ÿÿÿèüÿÿÿéVÿÿÿfèüÿÿÿWVS‹X‰ÙƒáÁáƒã¼
$  u‹’¨  ¼
È   O‹p\‹X`PP‰Hd‰Èèüÿÿÿ„Àt;7‰ØGs‰7‰_[^_Ã´&    èüÿÿÿ‰ÁWƒáV‰ÆÁøSƒæƒùtP÷Ğ…Àu d¡    …ötN‹¸”  ‰ø[^_Ã´&    v ‰Óèüÿÿÿ‰Ç…Àt…öu?„Ûtd¡    ;¸”  t‹G…ÀuÆ1ÿ‰ø[^_Ã¶    ‹€¨  [^‹¸  ‰ø_Ã¶    1Òèüÿÿÿ…ÀtÏd‹    ‹š¨  9˜¨  t€1ÿë¸´&    t& èüÿÿÿV‰ÖS‰Ãèüÿÿÿ1Ò‰Øè'ÿÿÿ…Àt7èüÿÿÿƒãÇ    ÇF    ƒûtÇF@B 1À[^Ãt& ÇF   1À[^Ãèüÿÿÿ¸êÿÿÿëàèüÿÿÿV‰ÆSèüÿÿÿ‹F1ÒèÈşÿÿ…ÀtR‰ÃFPÇF    ‰FP¸   ğÁ…Àt(P	Âx‰^hèüÿÿÿ1À[^Ãt& º   ‰Øèüÿÿÿëáfº   ‰ØèüÿÿÿëÑèüÿÿÿ¸êÿÿÿëÏ¶    èüÿÿÿÇ@úÿÿÿéoÿÿÿ´&    ´&    èüÿÿÿÇ@şÿÿÿéOÿÿÿ´&    ´&    èüÿÿÿU‰ÍW‰ÇVSƒì‰T$ÇD$    ‹D$‹t$‹_‹ğ‹Dğ‰$‰D$…Û„‹   ‹C‹t$‹S9$ÆrR¾   ‹CÇC$   …Àt‰ÚèüÿÿÿÇC    ‹ES‰U‰k‰C ‰‹_…ÛtAƒÆ‹S‹Cƒşt‹L$9$Ás³‰GƒÇƒD$‹D$‰Wğƒø…aÿÿÿƒÄ[^_]Ãt& ºÿÿÿÿ¸ÿÿÿÿëÌ´&    t& èüÿÿÿS‰Ãèüÿÿÿ1Ò‰Øè*ıÿÿ…Àtèüÿÿÿ¸ÿÿÿÿ[Ãèüÿÿÿ¸êÿÿÿ[Ã´&    ´&    èüÿÿÿVS‰Ãƒìd¡    ‰D$1Àèüÿÿÿ1Ò‹ChöC”Âèüÿÿÿ…Àtc‰â‰Æèüÿÿÿ…À„Í   ‹Ct…Àt6‹†¬  ‹$èüÿÿÿèüÿÿÿ¸   ‹T$d+    …Ú   ƒÄ[^Ã´&    ‹Cd…Àu)‹†¬  ‹$èüÿÿÿèüÿÿÿ‹Chèüÿÿÿ1Àë¼´&    v SPèüÿÿÿ‹SÇCd    ‰ĞƒàƒâuT‹–¨  Áà‹K\„È   ‹S`33P	ÊuœÇ     ‹$Ç@    ‹†¬  èüÿÿÿèüÿÿÿëv ‹Sd…Òt)éwÿÿÿfÁà„$  ‹‹P3K\3S`	Ê…Jÿÿÿë¬CP9CPuÏéHÿÿÿèüÿÿÿ´&    ´&    èüÿÿÿS‰Óèüÿÿÿd¡    ‹€”  …ÀtèüÿÿÿÇ    1ÀÇC    ÇC   [Ãèüÿÿÿ¸êÿÿÿ[Ã´&    v èüÿÿÿS‰Óèüÿÿÿd¡    ‹€¨  ‹€  …ÀtèüÿÿÿÇ    1ÀÇC    ÇC   [Ãèüÿÿÿ¸êÿÿÿ[Ãt& èüÿÿÿU‰åWVSƒäøƒì`‰D$$‰Ğ‹¸¨  d‹    ‰T$\1Ò‰|$‹—ø   …Ò…ì   „ÉuhT$(èüÿÿÿ‹D$0‹T$4‰Á‰Ó‰D$H‹D$8‰T$L‹T$<L$(\$,‰D$P‰L$@‰\$D‰T$T‹D$$‹TÄD‹DÄ@‹L$\d+    …Ì  eô[^_]Ã¶    Ÿ°   ‹—ø   ‰\$Ÿ¸   ‰\$ŸÀ   ‰\$…Ò„Œ   ‹L$èüÿÿÿ‰Æ‰×‹L$èüÿÿÿ‰D$‹L$‰T$èüÿÿÿ‹L$‹\$‰t$H‰|$Lñ‰D$Pû‰L$@‰\$D‰T$TéYÿÿÿt& ‹\$‹°   èüÿÿÿ‰Æ‰×‹¸   èüÿÿÿ‰D$‹À   ‰T$ë ´&    v T$(èüÿÿÿ‹D$0‹|$‰D$ ‹D$4‰D$ë‰È‹\$ ‹L$ğÇ1Ö3D$	Æt‰ùèüÿÿÿ;D$ ‰D$‰Á‰ĞD$‰ÖrÍ‹D$(‹|$‰D$ ‹D$,‰D$ë´&    ‰È‹\$ ‹L$ğÇ1Ö3D$	Æt‰ùèüÿÿÿ;D$ ‰D$‰Á‰ĞD$‰ÖrÍ‹D$8‹|$‰D$ ‹D$<‰D$ë´&    ‰È‹\$ ‹L$ğÇ1Ö3D$	Æt‰ùèüÿÿÿ;D$ ‰D$‰Á‰ĞD$‰ÖrÍ‹D$Ç€ø      énşÿÿèüÿÿÿt& èüÿÿÿU‰åW‰ÇVƒçS‰Óƒäğƒì0d‹    ‰T$,1Ò‰D$‰|$èüÿÿÿ‹D$º   ‰ÆèNøÿÿƒæ1Ò…ö”Âèüÿÿÿ…À„¢   ‰Â…öt1ƒ|$„†   ‹°Ø  ‹¸Ü  ‹D$…Àtbƒøt1ö1ÿët& ‹D$1Éèõüÿÿ‰Æ‰×èüÿÿÿ‰ò‰ùD$èüÿÿÿ‹D$‰‹D$‰C‹D$‰C1À‹T$,d+    u3eô[^_]Ãt& ²à  ºä  ë±fèüÿÿÿ‰Æ‰×ë¤èüÿÿÿ¸êÿÿÿëÀèüÿÿÿt& èüÿÿÿ¸úÿÿÿéáşÿÿèüÿÿÿ¸şÿÿÿéÑşÿÿèüÿÿÿWVS‰Ãƒì‹{d¡    ‰D$1Àèüÿÿÿ1Ò‹ChöC”Âèüÿÿÿ…Àth‰â‰Æèüÿÿÿ…Àt[‰øƒàöCt"ƒøt}‹–Ø  ‹Ü  …Àt]ƒøt1Ò1Éë¹   ‰òèäûÿÿ‰Ñ‰Â‰ØèIôÿÿ‰ò‰Øèpöÿÿ‹†¬  ‹$èüÿÿÿèüÿÿÿ‹D$d+    u-ƒÄ[^_Ã´&    f–à  ä  ë²f‰ğèüÿÿÿ‰Ñ‰Âë£èüÿÿÿ´&    ´&    èüÿÿÿS‰Ã‹@8ƒàûƒøt]‹CL…ÀtN‹C@C<u1Ò‰ØèüÿÿÿÇC\    ÇC`    [Ãt& ‹C4P‰Ø‰S4èüÿÿÿ…Àtå‰ØèµşÿÿƒC4[Ã´&    ‹CHèüÿÿÿÇC\    ÇC`    [ÃèüÿÿÿU‰åWV‰ÆSƒäğƒì`‹E‰T$‰L$ ‰D$8d¡    ‰D$\‹Fƒà‰Çèüÿÿÿ1Ò‹FhöF”Âèüÿÿÿ‰D$<…À„Š  ‹\$ º}Á%‹C‹K‹[9Âº   ÊŒ  º Êš;iÉ Êš;÷âÊ‰Ù1ÛÈÚ‰D$‰T$‰D$0‰T$4‹D$<T$Xèüÿÿÿ…À„-  ‹F<‹V@‰$‹F\‰T$‰D$‹F`‰D$‹Ft‰D$$…À…C  ‹Fd…ÀtVPèüÿÿÿÇFd    öF…Ó  ‹T$<¹   ‰øèãùÿÿ‰D$(‰T$,‹D$8…Àt#‹|$|$…  Ç@    Ç@    Ç@    ‹T$$…Ò…  ‹T$4‹D$0‰Ñ	Á„Â  öD$u‹L$(‹\$,ÈÚ‰D$0‰T$4‰D$‰T$‹D$‹T$‹L$0‹\$4‰F\‹D$(‰V`‹T$,9È‰ĞØs‹T$<‰ğèŞóÿÿ‹D$<‹T$X‹€¬  èüÿÿÿ‹\$ º}Á%‹‹K‹[9Âº   ÊŒS  º Êš;iÉ Êš;÷âÊ‰Ù1ÛÈÚ‹L$(‹\$,‰F<‹F4‰V@‹T$4‰ßÇF,    ƒÀÇF0    ƒàşÇF$ÿÿÿÿ‰F4‹D$0ÇF(ÿÿÿÿ9Á×r9‰×	Ç…d  ‹D$<T$Xèüÿÿÿ…Àt‹Fd…À„è  ‹D$<‹T$X‹€¬  èüÿÿÿèüÿÿÿ‹|$8…ÿt$‹$‹L$D$@èüÿÿÿ‹D$@‰‹D$D‰G‹D$H‰G‹D$\d+    …ì  ‹D$$eô[^_]Ã´&    ƒÿ‹D$<„«  ‹Ü  ‹€Ø  ‰T$,‰D$(…ÿ„Ç   ƒÿ„şÿÿÇD$(    ÇD$,    éüıÿÿ´&    ÇF\    ÇF`    éşÿÿt& ‹L$,‹T$(‰ğèğÿÿ‹L$(‹\$,9Á‰ß×r!‹D$8Ç@   Ç@    Ç@    éËıÿÿv )ÈÚ‰Ñ‰ÂD$@èüÿÿÿ‹D$@‹|$8‰G‹D$D‰G‹D$H‰Gé™ıÿÿ´&    f‰ğèÉûÿÿéşÿÿt& ‹D$<‹ˆà  ‹˜ä  L$(\$,é8ıÿÿv èüÿÿÿÇD$$ıÿÿÿéºşÿÿ¶    ÇD$0ÿÿÿÿÇD$4ÿÿÿÇD$ÿÿÿÿÇD$ÿÿÿéüÿÿv ÇFtÿÿÿÿÇD$$   é¿üÿÿt& ¸ÿÿÿÿºÿÿÿéµıÿÿ´&    f‹V‹t$<‰ĞƒàÁàƒâu<‹–¨  „È   Ç     Ç@    éåıÿÿt& èüÿÿÿ‰D$(‰T$,é{üÿÿ¶    „$  ëÈèüÿÿÿfèüÿÿÿUWV‰Î¹)   S‰ÃìÈ   ‰$l$ ‰ïd¡    ‰„$Ä   1Àó«‰è‰\$8ÇD$DÿÿÿÿÇD$Hÿÿÿÿd‹=    èòÿÿ‰|$h‰Ã…Àt"‹„$Ä   d+    …  ÄÈ   ‰Ø[^_]Ã‹ÇD$    ÇD$    ‰D$‹FÇD$    ‰D$‹F‰D$D$0èüÿÿÿ‰èj ‹T$L$èŠúÿÿY‰Ã…À…  ‹‰ø‰şâ   uE‹‹T$|‹„$€   ƒáu2	Ğ„æ   ÇF   D$0èüÿÿÿèüÿÿÿD$0èüÿÿÿ‹‰ğâ   t¼‹D$|‹”$€   ¹ä€ ‰$D$‰T$1ÒP‰èèúÿÿZ‰Æ…Àuz‰èèéòÿÿD$0èüÿÿÿ‹L$‰ÈL$™‹D$	Ğ	È„åşÿÿ‹<$‹l$d¡    ‰¸L  ˆ8  ‰¨P  ‹€D  …ÀtST$‰Èèüÿÿÿ‰Ãéªşÿÿt& D$0èüÿÿÿ‰èèxòÿÿ‰ÆD$0èüÿÿÿƒştàë†¶    ‰èèYòÿÿD$0èüÿÿÿéişÿÿ»üıÿÿé_şÿÿèüÿÿÿ´&    t& èüÿÿÿUW‰ÏV‰ÖS‰Ãd‹-    ƒì¨t!Áø‰$ƒøÿtX1Ò1É‰èèüÿÿÿ‰Â‹$÷Ğ9ĞtB‰ù‰ò‰Øè§ıÿÿ=üıÿÿtƒÄ[^_]Ã´&    ƒæu"‰@  Ç…<  à¬ ƒÄ[^_]Ãv ¸êÿÿÿëÉ¸şıÿÿëÂfèüÿÿÿU‰åSƒäğƒì ‹Xd‹    ‰T$1Ò‹P‹@‰Ñ‰Â‰àèüÿÿÿ‹$º   L$‰D$‹D$‰D$‹D$‰D$‰Øè	ıÿÿ‹T$d+    u‹]üÉÃèüÿÿÿfèüÿÿÿV¸úÿÿÿS‰Ód‹5    èÖüÿÿ=üıÿÿt[^Ãt& ƒãuÇ†@  úÿÿÿÇ†<  à¬ [^Ã¸şıÿÿëÖt& èüÿÿÿU‰åWV‰ÖS‰Ãƒäğƒì0‹S`d¡    ‰D$,‹C‰T$‰D$‹C\‰D$èüÿÿÿ1Ò‹ChöC”Âèüÿÿÿ…À„   ‹K@‹S<‰ÇD$èüÿÿÿ‹D$‹L$‰‹D$‰F‹D$‰F‹D$	Átm‹D$ƒà‰D$öCt|ƒø„Ó   ‹Ø  ‹ŸÜ  ‹‡à  ‹—ä  ‹|$…ÿ„   ƒÿt[‹T$‹L$D$èüÿÿÿ‹D$‰F‹D$‰F‹D$‰Ff‹D$,d+    …|   eô[^_]éüÿÿÿv ‹D$1É‰úèÃñÿÿ‰Á‰Ó‹D$‹T$‰ß9Á×r/ÇF   ÇF    ÇF    ë¨´&    ÁÓëË´&    v )L$\$é[ÿÿÿv ‰øèüÿÿÿ‰Á‰Óë¤èüÿÿÿ´& 