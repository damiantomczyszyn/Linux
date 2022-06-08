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
				  const s