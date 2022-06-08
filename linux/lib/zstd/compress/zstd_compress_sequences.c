// SPDX-License-Identifier: GPL-2.0-only
/*
 *  kernel/sched/core.c
 *
 *  Core kernel scheduler code and related syscalls
 *
 *  Copyright (C) 1991-2002  Linus Torvalds
 */
#include <linux/highmem.h>
#include <linux/hrtimer_api.h>
#include <linux/ktime_api.h>
#include <linux/sched/signal.h>
#include <linux/syscalls_api.h>
#include <linux/debug_locks.h>
#include <linux/prefetch.h>
#include <linux/capability.h>
#include <linux/pgtable_api.h>
#include <linux/wait_bit.h>
#include <linux/jiffies.h>
#include <linux/spinlock_api.h>
#include <linux/cpumask_api.h>
#include <linux/lockdep_api.h>
#include <linux/hardirq.h>
#include <linux/softirq.h>
#include <linux/refcount_api.h>
#include <linux/topology.h>
#include <linux/sched/clock.h>
#include <linux/sched/cond_resched.h>
#include <linux/sched/debug.h>
#include <linux/sched/isolation.h>
#include <linux/sched/loadavg.h>
#include <linux/sched/mm.h>
#include <linux/sched/nohz.h>
#include <linux/sched/rseq_api.h>
#include <linux/sched/rt.h>

#include <linux/blkdev.h>
#include <linux/context_tracking.h>
#include <linux/cpuset.h>
#include <linux/delayacct.h>
#include <linux/init_task.h>
#include <linux/interrupt.h>
#include <linux/ioprio.h>
#include <linux/kallsyms.h>
#include <linux/kcov.h>
#include <linux/kprobes.h>
#include <linux/llist_api.h>
#include <linux/mmu_context.h>
#include <linux/mmzone.h>
#include <linux/mutex_api.h>
#include <linux/nmi.h>
#include <linux/nospec.h>
#include <linux/perf_event_api.h>
#include <linux/profile.h>
#include <linux/psi.h>
#include <linux/rcuwait_api.h>
#include <linux/sched/wake_q.h>
#include <linux/scs.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/vtime.h>
#include <linux/wait_api.h>
#include <linux/workqueue_api.h>

#ifdef CONFIG_PREEMPT_DYNAMIC
# ifdef CONFIG_GENERIC_ENTRY
#  include <linux/entry-common.h>
# endif
#endif

#include <uapi/linux/sched/types.h>

#include <asm/switch_to.h>
#include <asm/tlb.h>

#define CREATE_TRACE_POINTS
#include <linux/sched/rseq_api.h>
#include <trace/events/sched.h>
#undef CREATE_TRACE_POINTS

#include "sched.h"
#include "stats.h"
#include "autogroup.h"

#include "autogroup.h"
#include "pelt.h"
#include "smp.h"
#include "stats.h"

#include "../workqueue_internal.h"
#include "../../fs/io-wq.h"
#include "../smpboot.h"

/*
 * Export tracepoints that act as a bare tracehook (ie: have no trace event
 * associated with them) to allow external modules to probe them.
 */
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_cfs_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_rt_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_dl_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_irq_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_se_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(pelt_thermal_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_cpu_capacity_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_overutilized_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_util_est_cfs_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_util_est_se_tp);
EXPORT_TRACEPOINT_SYMBOL_GPL(sched_update_nr_running_tp);

DEFINE_PER_CPU_SHARED_ALIGNED(struct rq, runqueues);

#ifdef CONFIG_SCHED_DEBUG
/*
 * Debugging: various feature bits
 *
 * If SCHED_DEBUG is disabled, each compilation unit has its own copy of
 * sysctl_sched_features, defined in sched.h, to allow constants propagation
 * at compile time and compiler optimization based on features default.
 */
#define SCHED_FEAT(name, enabled)	\
	(1UL << __SCHED_FEAT_##name) * enabled |
const_debug unsigned int sysctl_sched_features =
#include "features.h"
	0;
#undef SCHED_FEAT

/*
 * Print a warning if need_resched is set for the given duration (if
 * LATENCY_WARN is enabled).
 *
 * If sysctl_resched_latency_warn_once is set, only one warning will be shown
 * per boot.
 */
__read_mostly int sysctl_resched_latency_warn_ms = 100;
__read_mostly int sysctl_resched_latency_warn_once = 1;
#endif /* CONFIG_SCHED_DEBUG */

/*
 * Number of tasks to iterate in a single balance run.
 * Limited because this is done with IRQs disabled.
 */
#ifdef CONFIG_PREEMPT_RT
const_debug unsigned int sysctl_sched_nr_migrate = 8;
#else
const_debug unsigned int sysctl_sched_nr_migrate = 32;
#endif

/*
 * period over which we measure -rt task CPU usage in us.
 * default: 1s
 */
unsigned int sysctl_sched_rt_period = 1000000;

__read_mostly int scheduler_running;

#ifdef CONFIG_SCHED_CORE

DEFINE_STATIC_KEY_FALSE(__sched_core_enabled);

/* kernel prio, less is more */
static inline int __task_prio(struct task_struct *p)
{
	if (p->sched_class == &stop_sched_class) /* trumps deadline */
		return -2;

	if (rt_prio(p->prio)) /* includes deadline */
		return p->prio; /* [-1, 99] */

	if (p->sched_class == &idle_sched_class)
		return MAX_RT_PRIO + NICE_WIDTH; /* 140 */

	return MAX_RT_PRIO + MAX_NICE; /* 120, squash fair */
}

/*
 * l(a,b)
 * le(a,b) := !l(b,a)
 * g(a,b)  := l(b,a)
 * ge(a,b) := !l(a,b)
 */

/* real prio, less is less */
static inline bool prio_less(struct task_struct *a, struct task_struct *b, bool in_fi)
{

	int pa = __task_prio(a), pb = __task_prio(b);

	if (-pa < -pb)
		return true;

	if (-pb < -pa)
		return false;

	if (pa == -1) /* dl_prio() doesn't work because of stop_class above */
		return !dl_time_before(a->dl.deadline, b->dl.deadline);

	if (pa == MAX_RT_PRIO + MAX_NICE)	/* fair */
		return cfs_prio_less(a, b, in_fi);

	return false;
}

static inline bool __sched_core_less(struct task_struct *a, struct task_struct *b)
{
	if (a->core_cookie < b->core_cookie)
		return true;

	if (a->core_cookie > b->core_cookie)
		return false;

	/* flip prio, so high prio is leftmost */
	if (prio_less(b, a, !!task_rq(a)->core->core_forceidle_count))
		return true;

	return false;
}

#define __node_2_sc(node) rb_entry((node), struct task_struct, core_node)

static inline bool rb_sched_core_less(struct rb_node *a, const struct rb_node *b)
{
	return __sched_core_less(__node_2_sc(a), __node_2_sc(b));
}

static inline int rb_sched_core_cmp(const void *key, const struct rb_node *node)
{
	const struct task_struct *p = __node_2_sc(node);
	unsigned long cookie = (unsigned long)key;

	if (cookie < p->core_cookie)
		return -1;

	if (cookie > p->core_cookie)
		return 1;

	return 0;
}

void sched_core_enqueue(struct rq *rq, struct task_struct *p)
{
	rq->core->core_task_seq++;

	if (!p->core_cookie)
		return;

	rb_add(&p->core_node, &rq->core_tree, rb_sched_core_less);
}

void sched_core_dequeue(struct rq *rq, struct task_struct *p, int flags)
{
	rq->core->core_task_seq++;

	if (sched_core_enqueued(p)) {
		rb_erase(&p->core_node, &rq->core_tree);
		RB_CLEAR_NODE(&p->core_node);
	}

	/*
	 * Migrating the last task off the cpu, with the cpu in forced idle
	 * state. Reschedule to create an accounting edge for forced idle,
	 * and re-examine whether the core is still in forced idle state.
	 */
	if (!(flags & DEQUEUE_SAVE) && rq->nr_running == 1 &&
	    rq->core->core_forceidle_count && rq->curr == rq->idle)
		resched_curr(rq);
}

/*
 * Find left-most (aka, highest priority) task matching @cookie.
 */
static struct task_struct *sched_core_find(struct rq *rq, unsigned long cookie)
{
	struct rb_node *node;

	node = rb_find_first((void *)cookie, &rq->core_tree, rb_sched_core_cmp);
	/*
	 * The idle task always matches any cookie!
	 */
	if (!node)
		return idle_sched_class.pick_task(rq);

	return __node_2_sc(node);
}

static struct task_struct *sched_core_next(struct task_struct *p, unsigned long cookie)
{
	struct rb_node *node = &p->core_node;

	node = rb_next(node);
	if (!node)
		return NULL;

	p = container_of(node, struct task_struct, core_node);
	if (p->core_cookie != cookie)
		return NULL;

	return p;
}

/*
 * Magic required such that:
 *
 *	raw_spin_rq_lock(rq);
 *	...
 *	raw_spin_rq_unlock(rq);
 *
 * ends up locking and unlocking the _same_ lock, and all CPUs
 * always agree on what rq has what lock.
 *
 * XXX entirely possible to selectively enable cores, don't bother for now.
 */

static DEFINE_MUTEX(sched_core_mutex);
static atomic_t sched_core_count;
static struct cpumask sched_core_mask;

static void sched_core_lock(int cpu, unsigned long *flags)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	int t, i = 0;

	local_irq_save(*flags);
	for_each_cpu(t, smt_mask)
		raw_spin_lock_nested(&cpu_rq(t)->__lock, i++);
}

static void sched_core_unlock(int cpu, unsigned long *flags)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	int t;

	for_each_cpu(t, smt_mask)
		raw_spin_unlock(&cpu_rq(t)->__lock);
	local_irq_restore(*flags);
}

static void __sched_core_flip(bool enabled)
{
	unsigned long flags;
	int cpu, t;

	cpus_read_lock();

	/*
	 * Toggle the online cores, one by one.
	 */
	cpumask_copy(&sched_core_mask, cpu_online_mask);
	for_each_cpu(cpu, &sched_core_mask) {
		const struct cpumask *smt_mask = cpu_smt_mask(cpu);

		sched_core_lock(cpu, &flags);

		for_each_cpu(t, smt_mask)
			cpu_rq(t)->core_enabled = enabled;

		cpu_rq(cpu)->core->core_forceidle_start = 0;

		sched_core_unlock(cpu, &flags);

		cpumask_andnot(&sched_core_mask, &sched_core_mask, smt_mask);
	}

	/*
	 * Toggle the offline CPUs.
	 */
	cpumask_copy(&sched_core_mask, cpu_possible_mask);
	cpumask_andnot(&sched_core_mask, &sched_core_mask, cpu_online_mask);

	for_each_cpu(cpu, &sched_core_mask)
		cpu_rq(cpu)->core_enabled = enabled;

	cpus_read_unlock();
}

static void sched_core_assert_empty(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		WARN_ON_ONCE(!RB_EMPTY_ROOT(&cpu_rq(cpu)->core_tree));
}

static void __sched_core_enable(void)
{
	static_branch_enable(&__sched_core_enabled);
	/*
	 * Ensure all previous instances of raw_spin_rq_*lock() have finished
	 * and future ones will observe !sched_core_disabled().
	 */
	synchronize_rcu();
	__sched_core_flip(true);
	sched_core_assert_empty();
}

static void __sched_core_disable(void)
{
	sched_core_assert_empty();
	__sched_core_flip(false);
	static_branch_disable(&__sched_core_enabled);
}

void sched_core_get(void)
{
	if (atomic_inc_not_zero(&sched_core_count))
		return;

	mutex_lock(&sched_core_mutex);
	if (!atomic_read(&sched_core_count))
		__sched_core_enable();

	smp_mb__before_atomic();
	atomic_inc(&sched_core_count);
	mutex_unlock(&sched_core_mutex);
}

static void __sched_core_put(struct work_struct *work)
{
	if (atomic_dec_and_mutex_lock(&sched_core_count, &sched_core_mutex)) {
		__sched_core_disable();
		mutex_unlock(&sched_core_mutex);
	}
}

void sched_core_put(void)
{
	static DECLARE_WORK(_work, __sched_core_put);

	/*
	 * "There can be only one"
	 *
	 * Either this is the last one, or we don't actually need to do any
	 * 'work'. If it is the last *again*, we rely on
	 * WORK_STRUCT_PENDING_BIT.
	 */
	if (!atomic_add_unless(&sched_core_count, -1, 1))
		schedule_work(&_work);
}

#else /* !CONFIG_SCHED_CORE */

static inline void sched_core_enqueue(struct rq *rq, struct task_struct *p) { }
static inline void
sched_core_dequeue(struct rq *rq, struct task_struct *p, int flags) { }

#endif /* CONFIG_SCHED_CORE */

/*
 * part of the period that we allow rt tasks to run in us.
 * default: 0.95s
 */
int sysctl_sched_rt_runtime = 950000;


/*
 * Serialization rules:
 *
 * Lock order:
 *
 *   p->pi_lock
 *     rq->lock
 *       hrtimer_cpu_base->lock (hrtimer_start() for bandwidth controls)
 *
 *  rq1->lock
 *    rq2->lock  where: rq1 < rq2
 *
 * Regular state:
 *
 * Normal scheduling state is serialized by rq->lock. __schedule() takes the
 * local CPU's rq->lock, it optionally removes the task from the runqueue and
 * always looks at the local rq data structures to find the most eligible task
 * to run next.
 *
 * Task enqueue is also under rq->lock, possibly taken from another CPU.
 * Wakeups from another LLC domain might use an IPI to transfer the enqueue to
 * the local CPU to avoid bouncing the runqueue state around [ see
 * ttwu_queue_wakelist() ]
 *
 * Task wakeup, specifically wakeups that involve migration, are horribly
 * complicated to avoid having to take two rq->locks.
 *
 * Special state:
 *
 * System-calls and anything external will use task_rq_lock() which acquires
 * both p->pi_lock and rq->lock. As a consequence the state they change is
 * stable while holding either lock:
 *
 *  - sched_setaffinity()/
 *    set_cpus_allowed_ptr():	p->cpus_ptr, p->nr_cpus_allowed
 *  - set_user_nice():		p->se.load, p->*prio
 *  - __sched_setscheduler():	p->sched_class, p->policy, p->*prio,
 *				p->se.load, p->rt_priority,
 *				p->dl.dl_{runtime, deadline, period, flags, bw, density}
 *  - sched_setnuma():		p->numa_preferred_nid
 *  - sched_move_task()/
 *    cpu_cgroup_fork():	p->sched_task_group
 *  - uclamp_update_active()	p->uclamp*
 *
 * p->state <- TASK_*:
 *
 *   is changed locklessly using set_current_state(), __set_current_state() or
 *   set_special_state(), see their respective comments, or by
 *   try_to_wake_up(). This latter uses p->pi_lock to serialize against
 *   concurrent self.
 *
 * p->on_rq <- { 0, 1 = TASK_ON_RQ_QUEUED, 2 = TASK_ON_RQ_MIGRATING }:
 *
 *   is set by activate_task() and cleared by deactivate_task(), under
 *   rq->lock. Non-zero indicates the task is runnable, the special
 *   ON_RQ_MIGRATING state is used for migration without holding both
 *   rq->locks. It indicates task_cpu() is not stable, see task_rq_lock().
 *
 * p->on_cpu <- { 0, 1 }:
 *
 *   is set by prepare_task() and cleared by finish_task() such that it will be
 *   set before p is scheduled-in and cleared after p is scheduled-out, both
 *   under rq->lock. Non-zero indicates the task is running on its CPU.
 *
 *   [ The astute reader will observe that it is possible for two tasks on one
 *     CPU to have ->on_cpu = 1 at the same time. ]
 *
 * task_cpu(p): is changed by set_task_cpu(), the rules are:
 *
 *  - Don't call set_task_cpu() on a blocked task:
 *
 *    We don't care what CPU we're not running on, this simplifies hotplug,
 *    the CPU assignment of blocked tasks isn't required to be valid.
 *
 *  - for try_to_wake_up(), called under p->pi_lock:
 *
 *    This allows try_to_wake_up() to only take one rq->lock, see its comment.
 *
 *  - for migration called under rq->lock:
 *    [ see task_on_rq_migrating() in task_rq_lock() ]
 *
 *    o move_queued_task()
 *    o detach_task()
 *
 *  - for migration called under double_rq_lock():
 *
 *    o __migrate_swap_task()
 *    o push_rt_task() / pull_rt_task()
 *    o push_dl_task() / pull_dl_task()
 *    o dl_task_offline_migration()
 *
 */

void raw_spin_rq_lock_nested(struct rq *rq, int subclass)
{
	raw_spinlock_t *lock;

	/* Matches synchronize_rcu() in __sched_core_enable() */
	preempt_disable();
	if (sched_core_disabled()) {
		raw_spin_lock_nested(&rq->__lock, subclass);
		/* preempt_count *MUST* be > 1 */
		preempt_enable_no_resched();
		return;
	}

	for (;;) {
		lock = __rq_lockp(rq);
		raw_spin_lock_nested(lock, subclass);
		if (likely(lock == __rq_lockp(rq))) {
			/* preempt_count *MUST* be > 1 */
			preempt_enable_no_resched();
			return;
		}
		raw_spin_unlock(lock);
	}
}

bool raw_spin_rq_trylock(struct rq *rq)
{
	raw_spinlock_t *lock;
	bool ret;

	/* Matches synchronize_rcu() in __sched_core_enable() */
	preempt_disable();
	if (sched_core_disabled()) {
		ret = raw_spin_trylock(&rq->__lock);
		preempt_enable();
		return ret;
	}

	for (;;) {
		lock = __rq_lockp(rq);
		ret = raw_spin_trylock(lock);
		if (!ret || (likely(lock == __rq_lockp(rq)))) {
			preempt_enable();
			return ret;
		}
		raw_spin_unlock(lock);
	}
}

void raw_spin_rq_unlock(struct rq *rq)
{
	raw_spin_unlock(rq_lockp(rq));
}

#ifdef CONFIG_SMP
/*
 * double_rq_lock - safely lock two runqueues
 */
void double_rq_lock(struct rq *rq1, struct rq *rq2)
{
	lockdep_assert_irqs_disabled();

	if (rq_order_less(rq2, rq1))
		swap(rq1, rq2);

	raw_spin_rq_lock(rq1);
	if (__rq_lockp(rq1) == __rq_lockp(rq2))
		return;

	raw_spin_rq_lock_nested(rq2, SINGLE_DEPTH_NESTING);
}
#endif

/*
 * __task_rq_lock - lock the rq @p resides on.
 */
struct rq *__task_rq_lock(struct task_struct *p, struct rq_flags *rf)
	__acquires(rq->lock)
{
	struct rq *rq;

	lockdep_assert_held(&p->pi_lock);

	for (;;) {
		rq = task_rq(p);
		raw_spin_rq_lock(rq);
		if (likely(rq == task_rq(p) && !task_on_rq_migrating(p))) {
			rq_pin_lock(rq, rf);
			return rq;
		}
		raw_spin_rq_unlock(rq);

		while (unlikely(task_on_rq_migrating(p)))
			cpu_relax();
	}
}

/*
 * task_rq_lock - lock p->pi_lock and lock the rq @p resides on.
 */
struct rq *task_rq_lock(struct task_struct *p, struct rq_flags *rf)
	__acquires(p->pi_lock)
	__acquires(rq->lock)
{
	struct rq *rq;

	for (;;) {
		raw_spin_lock_irqsave(&p->pi_lock, rf->flags);
		rq = task_rq(p);
		raw_spin_rq_lock(rq);
		/*
		 *	move_queued_task()		task_rq_lock()
		 *
		 *	ACQUIRE (rq->lock)
		 *	[S] ->on_rq = MIGRATING		[L] rq = task_rq()
		 *	WMB (__set_task_cpu())		ACQUIRE (rq->lock);
		 *	[S] ->cpu = new_cpu		[L] task_rq()
		 *					[L] ->on_rq
		 *	RELEASE (rq->lock)
		 *
		 * If we observe the old CPU in task_rq_lock(), the acquire of
		 * the old rq->lock will fully serialize against the stores.
		 *
		 * If we observe the new CPU in task_rq_lock(), the address
		 * dependency headed by '[L] rq = task_rq()' and the acquire
		 * will pair with the WMB to ensure we then also see migrating.
		 */
		if (likely(rq == task_rq(p) && !task_on_rq_migrating(p))) {
			rq_pin_lock(rq, rf);
			return rq;
		}
		raw_spin_rq_unlock(rq);
		raw_spin_unlock_irqrestore(&p->pi_lock, rf->flags);

		while (unlikely(task_on_rq_migrating(p)))
			cpu_relax();
	}
}

/*
 * RQ-clock updating methods:
 */

static void update_rq_clock_task(struct rq *rq, s64 delta)
{
/*
 * In theory, the compile should just see 0 here, and optimize out the call
 * to sched_rt_avg_update. But I don't trust it...
 */
	s64 __maybe_unused steal = 0, irq_delta = 0;

#ifdef CONFIG_IRQ_TIME_ACCOUNTING
	irq_delta = irq_time_read(cpu_of(rq)) - rq->prev_irq_time;

	/*
	 * Since irq_time is only updated on {soft,}irq_exit, we might run into
	 * this case when a previous update_rq_clock() happened inside a
	 * {soft,}irq region.
	 *
	 * When this happens, we stop ->clock_task and only update the
	 * prev_irq_time stamp to account for the part that fit, so that a next
	 * update will consume the rest. This ensures ->clock_task is
	 * monotonic.
	 *
	 * It does however cause some slight miss-attribution of {soft,}irq
	 * time, a more accurate solution would be to update the irq_time using
	 * the current rq->clock timestamp, except that would require using
	 * atomic ops.
	 */
	if (irq_delta > delta)
		irq_delta = delta;

	rq->prev_irq_time += irq_delta;
	delta -= irq_delta;
#endif
#ifdef CONFIG_PARAVIRT_TIME_ACCOUNTING
	if (static_key_false((&paravirt_steal_rq_enabled))) {
		steal = paravirt_steal_clock(cpu_of(rq));
		steal -= rq->prev_steal_time_rq;

		if (unlikely(steal > delta))
			steal = delta;

		rq->prev_steal_time_rq += steal;
		delta -= steal;
	}
#endif

	rq->clock_task += delta;

#ifdef CONFIG_HAVE_SCHED_AVG_IRQ
	if ((irq_delta + steal) && sched_feat(NONTASK_CAPACITY))
		update_irq_load_avg(rq, irq_delta + steal);
#endif
	update_rq_clock_pelt(rq, delta);
}

void update_rq_clock(struct rq *rq)
{
	s64 delta;

	lockdep_assert_rq_held(rq);

	if (rq->clock_update_flags & RQCF_ACT_SKIP)
		return;

#ifdef CONFIG_SCHED_DEBUG
	if (sched_feat(WARN_DOUBLE_CLOCK))
		SCHED_WARN_ON(rq->clock_update_flags & RQCF_UPDATED);
	rq->clock_update_flags |= RQCF_UPDATED;
#endif

	delta = sched_clock_cpu(cpu_of(rq)) - rq->clock;
	if (delta < 0)
		return;
	rq->clock += delta;
	update_rq_clock_task(rq, delta);
}

#ifdef CONFIG_SCHED_HRTICK
/*
 * Use HR-timers to deliver accurate preemption points.
 */

static void hrtick_clear(struct rq *rq)
{
	if (hrtimer_active(&rq->hrtick_timer))
		hrtimer_cancel(&rq->hrtick_timer);
}

/*
 * High-resolution timer tick.
 * Runs from hardirq context with interrupts disabled.
 */
static enum hrtimer_restart hrtick(struct hrtimer *timer)
{
	struct rq *rq = container_of(timer, struct rq, hrtick_timer);
	struct rq_flags rf;

	WARN_ON_ONCE(cpu_of(rq) != smp_processor_id());

	rq_lock(rq