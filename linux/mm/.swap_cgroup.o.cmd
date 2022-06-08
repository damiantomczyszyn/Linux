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

	rq_lock(rq, &rf);
	update_rq_clock(rq);
	rq->curr->sched_class->task_tick(rq, rq->curr, 1);
	rq_unlock(rq, &rf);

	return HRTIMER_NORESTART;
}

#ifdef CONFIG_SMP

static void __hrtick_restart(struct rq *rq)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	ktime_t time = rq->hrtick_time;

	hrtimer_start(timer, time, HRTIMER_MODE_ABS_PINNED_HARD);
}

/*
 * called from hardirq (IPI) context
 */
static void __hrtick_start(void *arg)
{
	struct rq *rq = arg;
	struct rq_flags rf;

	rq_lock(rq, &rf);
	__hrtick_restart(rq);
	rq_unlock(rq, &rf);
}

/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	s64 delta;

	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense and can cause timer DoS.
	 */
	delta = max_t(s64, delay, 10000LL);
	rq->hrtick_time = ktime_add_ns(timer->base->get_time(), delta);

	if (rq == this_rq())
		__hrtick_restart(rq);
	else
		smp_call_function_single_async(cpu_of(rq), &rq->hrtick_csd);
}

#else
/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense. Rely on vruntime for fairness.
	 */
	delay = max_t(u64, delay, 10000LL);
	hrtimer_start(&rq->hrtick_timer, ns_to_ktime(delay),
		      HRTIMER_MODE_REL_PINNED_HARD);
}

#endif /* CONFIG_SMP */

static void hrtick_rq_init(struct rq *rq)
{
#ifdef CONFIG_SMP
	INIT_CSD(&rq->hrtick_csd, __hrtick_start, rq);
#endif
	hrtimer_init(&rq->hrtick_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL_HARD);
	rq->hrtick_timer.function = hrtick;
}
#else	/* CONFIG_SCHED_HRTICK */
static inline void hrtick_clear(struct rq *rq)
{
}

static inline void hrtick_rq_init(struct rq *rq)
{
}
#endif	/* CONFIG_SCHED_HRTICK */

/*
 * cmpxchg based fetch_or, macro so it works for different integer types
 */
#define fetch_or(ptr, mask)						\
	({								\
		typeof(ptr) _ptr = (ptr);				\
		typeof(mask) _mask = (mask);				\
		typeof(*_ptr) _old, _val = *_ptr;			\
									\
		for (;;) {						\
			_old = cmpxchg(_ptr, _val, _val | _mask);	\
			if (_old == _val)				\
				break;					\
			_val = _old;					\
		}							\
	_old;								\
})

#if defined(CONFIG_SMP) && defined(TIF_POLLING_NRFLAG)
/*
 * Atomically set TIF_NEED_RESCHED and test for TIF_POLLING_NRFLAG,
 * this avoids any races wrt polling state changes and thereby avoids
 * spurious IPIs.
 */
static bool set_nr_and_not_polling(struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	return !(fetch_or(&ti->flags, _TIF_NEED_RESCHED) & _TIF_POLLING_NRFLAG);
}

/*
 * Atomically set TIF_NEED_RESCHED if TIF_POLLING_NRFLAG is set.
 *
 * If this returns true, then the idle task promises to call
 * sched_ttwu_pending() and reschedule soon.
 */
static bool set_nr_if_polling(struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	typeof(ti->flags) old, val = READ_ONCE(ti->flags);

	for (;;) {
		if (!(val & _TIF_POLLING_NRFLAG))
			return false;
		if (val & _TIF_NEED_RESCHED)
			return true;
		old = cmpxchg(&ti->flags, val, val | _TIF_NEED_RESCHED);
		if (old == val)
			break;
		val = old;
	}
	return true;
}

#else
static bool set_nr_and_not_polling(struct task_struct *p)
{
	set_tsk_need_resched(p);
	return true;
}

#ifdef CONFIG_SMP
static bool set_nr_if_polling(struct task_struct *p)
{
	return false;
}
#endif
#endif

static bool __wake_q_add(struct wake_q_head *head, struct task_struct *task)
{
	struct wake_q_node *node = &task->wake_q;

	/*
	 * Atomically grab the task, if ->wake_q is !nil already it means
	 * it's already queued (either by us or someone else) and will get the
	 * wakeup due to that.
	 *
	 * In order to ensure that a pending wakeup will observe our pending
	 * state, even in the failed case, an explicit smp_mb() must be used.
	 */
	smp_mb__before_atomic();
	if (unlikely(cmpxchg_relaxed(&node->next, NULL, WAKE_Q_TAIL)))
		return false;

	/*
	 * The head is context local, there can be no concurrency.
	 */
	*head->lastp = node;
	head->lastp = &node->next;
	return true;
}

/**
 * wake_q_add() - queue a wakeup for 'later' waking.
 * @head: the wake_q_head to add @task to
 * @task: the task to queue for 'later' wakeup
 *
 * Queue a task for later wakeup, most likely by the wake_up_q() call in the
 * same context, _HOWEVER_ this is not guaranteed, the wakeup can come
 * instantly.
 *
 * This function must be used as-if it were wake_up_process(); IOW the task
 * must be ready to be woken at this location.
 */
void wake_q_add(struct wake_q_head *head, struct task_struct *task)
{
	�����Í�&    �v �����S��������غ    [�����f�f�f������1���  ;
��JM�Í�&    f������V�Ⱦ�  S�	�X���� 1�9Ή�։X�[^L�Ít& �����Í�&    �v �����S������  ��tI�������1���[Ív ��   t���������1�[�f��K� �$������$��������ލ�&    �7� �$������$�f�f�f�f�f�f�f�������Ћ    S����;�    XL�[Í�&    ��&    �����W��VS�	�X����    �� �X����9ʺ    �[^L�_Í�&    �t& ������f�f�f�f�f������U1ɺ
   WVS�Ã�d�    �D$1��������d�    ������-    �p������|$�L$���$VUP����%�  k�d��P����%�  Wk�d��P����%�  Qk�d��PRh$u S�������,�D$d+    u
��1�[^_]������f�f�f�f�f�f�f������Í�&    �v �����UWVS�Ã�hd�    �D$d1��D$$������D$$�����������5L   1҉D$��   ��H�+t$@��H�)Ƹ    H���    1Ʌ�HщT����u������1҉ǡ   ��IС   ��1҅�I����T$1�j�D$8R�w� ��P��������   ��� �������1�j�D$HR��� ��P��������   ��� ���������    1�1�jR��� P��������   ��� �؃�$�����1�j�D$DR��� ��P��������   ��� ���������� ��jWV������   ��� ���������   1�j��H�1�R��� ��P��������   ��� �؃�$������t$�D$1�j�D$����t$R��� P��������   ��� ��������|$�t$$1�j�7R��� ����P��������   ��� �������1�j�D$ R�� ��P��������   ��� �؃�$�������    1�1�jR�� P��������   ��� �������1�j�D$R�&� ��P��������   ��� ��������7� ��jWV������   ��� �؃�$�����j�D$$��1�R�H� P��������   ��� ��������   1�j��H�1�R�Y� ��P��������   ��� �������1�j�D$lR�j� ��P��������   ��� �؃�$�����1�j�D$XR�{� ��P��������   ��� �������1�j�D$D+D$`R��� ��P��������   ��� �������1�j�D$T+D$pR��� ��P��������   ��� �؃�$�����1�j�D$HR��� ��P��������   ��� �������1�j�D$XR��� ��P��������   ��� ��������P   1�j��H�1�R��� ��P��������   ��� �؃�$������T   1�j��H�1�R��� ��P��������   ��� ��������D   1�j��H�1�R��� ��P��������   ��� ��������H   1�j��H�1�R�� ��P��������   ��� �؃�$�����1�j�D$@R�� ��P��������   ��� ���������   1�j��H�1�R�%� ���P��������   ��� �������1�j�|$(R�6� �/��P��������   ��� �؃�$�����1ҍ�    jR�G� P��������   ��� �������1ҍ�    jR�X� P��������   ��� ���������   1҅�H�Phi� S�������   1҃�$��jH�1�R��� ��P��������   ��� ���������� ��jj j ������   ��� ��������    1�j��H�1�R��� ��P��������   ��� �؃�$������X   1�j��H�1�R��� ��P��������   ��� ������������1���jR��� P��������   ��� �������1�j�D$$R��� ��P��������   ��� �؃�$������    -  � %  ��+    -  � ��
Ph�� S����������1���jR�� P��������   ��� ��������� ��jj j ������   ��� �؃�$����������1���jR�#� P��������   ��� ��������p   1�j��H�1�R�4� ��P��������   ��� ��������`   1�j��H�1�R�E� ��P��������   ��� �؃�$������d   1�j��H�1�R�V� ��P��������   ��� ��������h   1�j��H�1�R�g� ��P��������   ��� ��������l   1�j��H�1�R�x� ��P��������   ��� �؃�$������    1�j��R��� P��������   ��� ��������(   1�j��H�1�R��� ��P��������   ��� ������������������������D$dd+    u
��h1�[^_]������f�f�f�f�f��������    S��1ɋ    �����P�`3 P�������Z[�f������U��S���    r�C0�S4�]��Ð��1��������!у��t�i��  ��  �]�����Í�&    �v �����U��S���    r�C(�S,�]��Ð��1��������!у��t�i��  ��  �]�����Í�&    �v �����U�Ź
   WVS��  d�    ��$  1���$�   ��󫍄$�   �������$�   ��$�   d�    ���  �P��$�   +r$z(+B,��1��R��P��$�   �������$�   ��$�   ��$�   ��$�   ��$�   ��$�   XZ��$�   �D$    �D$     �D$t    �D$x    �D$l    �D$p    �D$d    �D$h    �D$\    �D$`    �D$T    �D$X    �D$L    �D$P    �D$D    �D$H    �D$<    �D$@    �D$4    �D$8    �D$,    �D$0    �D$$    �D$(    �D$�����D$�    �D$|��$�   ����D$�    ������D$;    ��  �T$�|$�   ��$�   �t$|�D$4�    󥋴$�   ��$�   t$$��$�   |$(��$�   t$,��$�   |$0��$�   t$4|$8�����D$<�D$T$@�T$�1���D$D�    ��$�   T$H��$�   t$L��$�   |$P��$�   t$T��$�   |$X��$   t$\��$  |$`��$  t$d�T$|$h��$  ��    t$l��$  |$p1��4�Љ$������$Ɖ$�\$ ׋T$t$t�L$�L$|$x�<�    1����&    �$�,��D$�$���Dŉ,�1��Ӄ���
uى\$ ��$�   �L$�$�Z�����&    �v �l$|��$�   ����������D$tT$x�D$t�D$$�T$x�T$(�����R��� P��������D$4�T$8�����R��� P��������D$D�T$H�����R��� P��������D$T�T$X�����R��� P��������D$d�T$h�� �����R��� P��������D$T�T$X�����R��� P��������D$d�T$h�����R��� P��������D$t�T$x�����R��� P���������$�   ��$�   �� �����R��� P��������D$t�T$x�����R��� P���������
   ������    ���D$T�  �t& �|$�t$T�   4�    󥋔$�   ��$�   ��$�   ��$�   �T$��$�   �L$��$�   ��$�   �$�ȉT$�ډL$������D$�D$�T$���J�����$�   �T$(��$�   �L$0��$�   �T$,��$�   �L$8��$   �T$4��$�   �L$@��$  �T$<��$  �L$H��$  �T$D��$  �L$P�D$$�T$LSh�� U��������������R��� P��������D$�T$�����R��� P��������D$$�T$(�����R��� P��������D$4�T$8��$�����R��� P��������D$,�T$0�����R��� P��������D$<�T$@�����R��� P��������D$L�T$P�����R��� P��������D$\�T$`�� �����R��� P��������D$L�T$P�����R��� P��������D$\�T$`�����R��� P��������
   ����������غ    �������;    ������t$x��� ���t$x�����1�������    ^��X9�vg1ۍt& ��)�t'��&    �v �   � ` ��9�Fލ�����)�u���_�����1�R��� P��������������ZY�ǡ    9�r��ډ�)Ӆ�t �v �   � ` ��9�F�6�����)�u�������D$�������$�   ��$�   �á    �$������L$QS�t$WVRPhLu U��������� ��$�t$ �t$ �������$�   Y^�D$|�t$�
�t& ����1�R��� P�������XZ9�u��
   �������$  d+    u��  1�[^_]������f�f�f�f�f�f�f�������U�    WVS�������x�D$d�    �D$t1��$    �D$    �/��&    f��ڍD$$�|$$�   �4�    �������$T$�غ    �������;    rĸ   ������щD$�����d�    ���  �@�H$�X(L$�@,\$D$�R�ʉ�P�D$ ������\$�|$ �l$(XZ���ɚ;v�ڹ�K ��	������i� ʚ;)Ӌ4$� ʚ;�ډ���k�_k�ƉЉ�����������)�R��V��)�RWh�� �t$��������D$td+    u
��x1�[^_]������f�f�f�f�f������VS�X�p���~<0t-��1��������w���P��)�؅�~���0����	vں������[^��������d�    ���  �R���   R���  �R�   R���  �ph    Q�����1����f�f�f�f�������U��� W��VS������������t& Sh� W��������غ    �������;    rۺ
   ��1��    �������&    ��4�    �����h� W�������� �t& �D� �    �ph� W��������غ    �������;    rκ
   �����������
u�[1�^_]�f�������U�͹��  W��V��S�@T�������t0�h��É��@�a ���������    ��������[^_]�����f�[�����^_]Í�    �����U��WVS��1҃��@�@����������  �S�ǋC��	���   ����	���   �   9��    �|Y�4�80 ��d0 v�H�v �C�S ����d0 w2��
�T$�ȉ$������T$R��Wh�'3 P�L$�����������u��O�������G����   1ۅ���   ����[^_]Í�&    f��Ej1ҹ   �@ �@(R�#� P��j j �3���������u��C   �C    �U�3�BH�$�D$������$�D$�R�R �R(�$�����1ҹ   j�D$R�%� P���s�s����������:����C   �@0 �C    ������t& ���1�������؃�[^_]Ít& ��Ⱥ   ������؃�[^_]û����������&    ������UW��V��S��<�@ d�    �T$81ҋh��@����������   �	   ��������������u0�s�������C��tN��~b�D$8d+    up��<��[^_]Ív U�ٺ2   �D$
�������X��x��L$�����������맍�&    �$��������$맍�&    f��   ���$������$늹���������������WV�����S��d�    �L$1Ʌ�tM�B؋z�1Ҿ����������Å�t5�	   �������t���ډ�������ƅ�t0�K�������C��tF��~*�D$d+    u>����[^_Ít& ����������ōt& ��   ��������ȍ�&    �v �������������f������U��1�WVS���@�������D$����   �,$�}�@0 ��3�.�������9�u�$����@�������t>����h0 uӻ�����T$������J��B��t0��~S����[^_]Í�&    �v �,$�T$���������뾍t& �D$������؃�[^_]Ít& ����������[^_]ÉȺ   ������؃�[^_]�f�f�f�f�f�������W��VS�R��d�    ��  �
�   �������tE�ơ   ��tJ�   ��  ������Å�tUVh(� S������    �_����[^_Í�&    �������[^_Ít& ��   � 
  ������Å�u�������Ǎt& �����뼍�&    f������W�|$����w�U��WVS�Ã�<d�    �E䋃  �EċC<�x ��x��������C<�+� ���������   �Ɖ�������Å���   �TK  �ډC(�E�������MȉKT�ỦSX�EЉS@�SL������C\�K<�CD�KH�CP��f����C    �C    �C@b �������������Eĉp1��U�d+    u,�e�[^_]�g�_É�������������h�u �����X������������f�������UW��VS��d�5    ���B�   ��  ����$������$1҉ŉ��������tM�ơ   ��tR�   ��  ������Å�t]VUh0� S������    �_������[^_]Í�&    �v ���������[^_]Ð�   � 
  ������Å�u���������t& �����뵍�&    f������W�|$����w�U��WVS�Ã�<d�    �E䋃  �EċC<�x ��x��������C<�;� ���������   �Ɖ�������Å���   �XK  �ډC(�E�������MȉKT�ỦSX�EЉS@�SL������C\�K<�CD�KH�CP��f����C    �C    �C�b �������������Eĉp1��U�d+    u,�e�[^_]�g�_É�������������h�u �����X������������f���������@u�@ �@�@������Ít& �����Í�&    �v ������@ �@�@������Í�&    f������U�ŉ�W��V��S������É�)�D=�  !�ى���������D�/�  [^_]Í�    [1�^_]Ð�����VS�t$�Ӊ�9��N�)��������D�[^Í�&    �t& �����WVS�ÉЉʋs �L$�   ��tA9Au<�Y����������Å�u3�v��t,�D� �~������F��t(�D� �   �������[^_Ív �   ��[^_Ít& ����t"��������øD� ������������čt& ��D� �����볍t& �����W�|$����w�U��WV��S�������<�Mċ~d�    �U�1����������   ��������C(�D� ������V���  �Eĉs�N(�S��C�F(�K�C��V(��t�P�F�D� ������ڍE�������MȉKT�ỦSX�EЉS@�C\�CD�CP�EĉSL�K<�KH�@��f�� �f�� @tOf ��C e f�ǃ�   �e �8��t`�C�UčKP�������X�E�d+    uj�e��[^_]�g�_Ít& f��C d ǃ�   �d ��x    u��������롍�&    ��C    �C    뢸D� ������ػ����������������v �����V��S�H�I<��tM�������d�    ��p  �@��u���غ����[��!�^��E�Ít& 1������������E��Ӎv �Z봍�&    �v �����WVS�z �u8�    ��������ƅ�u���    ��������   ���������[^_Í�    ������������U�   W��VS�Ӻd� ��d�    �D$1����D$    �l$,�D$    ���������   �G�O�$��.<��   ���B  ��������ǅ��1  �E ����   �4$hp� h�  ������ƃ����Q �/   �.   ������E �   ��D$� �D$j �D$�����Y��= ����5Q �D$�ډ�P�������Z���Q 9���Q 1҉����������Q �������1��T$d+    ��   ��[^_]Í�&    ��c �G� ��t& �F����tĉ��������u�F�$�������멍v �k� ������ƅ���Q ������E ��������}  ���������Q �v ������f����������&    f������UWVS�Ӄ��D$��tN�����    tN�[��t;�s��+F$������<���ȉ$�����P�T$ ��L$�����Z��yË[��uŃ���[^_]�f��D$���0����[^_]Í�&    ��&    �����UWV��S���:�$d�    �D$1�����   �T$�Չ���������V,��P�D$�+���Z��t�Pf�� �f�� @t&f�� �u�$9Ht%1��T$d+    uW��[^_]��Ef% �f= @uۋ]$��$��u��\$�t& ����$������V,��P�D$�����D$�@�;X��uظ   ��������&    �t& ������UW��V��S�� �l$4�$�] d�    �D$1��؉\$������ډ��D$��������D$�T$��������Å�t[�C �8�h(�������1҉�f����WRU�v�v��L$�T$ ������������T$d+    ��   �� [^_]Í�&    �D$�L$�D$    �D$�T$�D$���������= ���wn�    �m����GT�$��������= ���w=� f �������������������   u4���0����؁� ���wV������������   u(�������1��?����kH���������������������벍sH���������������������������1�������������&    �v ����������  U��WVS��,�D$    �D$    �Q���m  �2����  �D$    �D$    �L$ �ыQ���2  ����x  �D$    ���D$    �L$�w����   ����=  �$    1ɉ��^����   ����  1�1��C����   �8 ��   �L$(�T$$�(����T$$�L$(ǋC$��$��u���^$��$��u��$���D$�O$��$���k����L$�D$D$D$�Q$��$���"����L$ �D$D$D$�A$��$��������D$D$��,[^_]Ð�   �x�����    �$   �t����t& �D$   �x����v �D$   뉍�    �D$   뗍�    �   �&�����    �   �$�����    �   �'�����    �   �5�����    �   �C�����    1�Í�&    ��    �����V��S�X��d�    �T$�V ��tB�������= ���w4��0��������S,��P�D$����Z��t2�@�����f% �f= @D$�D$d+    u����[^Í�&    ��������������t& �����UW��V��S��d�    �D$�D� �������@�P4��t�������R �1�����= ���wD��(�$    ��������S,��P�D$�����Y�Å�tX�$�P��uN�@��it*��1۸D� ������D$d+    u,����[^_]Ív �Q��tω�������$�Ít& ��������������&    �t& ������UWVS���D$�@ �D$���NR �@ �D$���CR �X ���8R �s ���1R �~ ���*R �o ���#R �U ���R �J ���R �A ���R �L$�$�y����$�L$�R ��&    ��    �����UWVS�Ã�d�    �D$1��{�0 ��   �C �{�$�¸�0 ������= ���w{��+��u*�q�f�� ��]R 9x�]R �D$�s   �k$��$��tI��������V,��P�D$�C���Z���]R �Pf�� �f�� @u��Cf% �f= @�]R 몍�    �D$d+    u��[^_]��������    �����WVS�Ã�d�    �D$1��k�s ��   ��&    �v ����   ��������K����   �C�����D� ������D� �C(������D� ������;���t/���f���C �P,����$)���i�9��8��C$��������u׃kt1��k�s �r����D$d+    ��   ��[^_Í�&    �v ��tˉ�1҉����������&    �ku���t��D$d+    uD����1�[^_�����f��D� ��� �D$�$    ������D� �c�����������������������&    ������UWVS�Ë@��t���t
������&�D� ��������m����D� [^_]�������&    �p��|�,�o�������t���������������s݉�[^_]�������&    �����S���������t��[�g�����&    [Í�&    ��&    �����W��V��S�X踄0 ��DظD� ������C��uf�C�D� ������F�� ���w^�@��t� ���   �D� ������kt�D� �����1�[^_Í�    �C��t�������܍�&    �v �D� �����������͉��ɍ�&    �t& �����UWV��S�B�x ��0 �_��DظD� ������C��uq�C�D� ������o�� ���wi����    �������t�Ff% �fEf�F�D� ������kt�D� �����1�[^_]Ít& ��C��t��������f��D� �����������Չ��э�&    �t& �����W��VS�˃�t�f% �f= ���   �w踄0 ��D�D� ������F��uh�F�D� ������� ���wq�W��t;�ى��������øD� ������nu�F��u>�D� �������[^_Í�    �������E��ƍv �D� �����������ӻ�����������뻉����t& ������U��WV��S���x��0 �_��DظD� ������S����   �C�D� �������� �����   �G��tw�G��tp���   �$��t#�M �P���t�L$��t����l$������G�9$t9� ���   �D� �K   ������kt1�D� ���������[^_]Í�&    ��D� �E  ������kuϋC��t���������t& ��D� ������   뵍�&    �����U��WVS��$�D$� �T$�x    ��  �:    ��  �D$�L$�@�A �y�0 ��  �T$��0 �U����|$�D$�= ���w�O��������   ��:�$�Ѕ���  �v �4$)ƋD$��i�9��8�P �T$��p$���t$������T$�ŋZ,��,��u�M�t& ��R �C�S��t9�ËS��+B$�
�������0����������P������^��y��C�S��uǋD$�t$��@    �@    ��N �Q,�L$������$$�$�8����  ��6����D$�@�D� ������D$�(�x�E �|$���=  ��1��,$1ۉ�����&    �������$�ރ��l���u�؉\$�����ƍD��,$���D�,���  �$������$���D$�s  �D$�U ��,�<�D$�D�	�4�����tH�|$ �t& ��Љ$��$��$������$�x���������������s��f�C�D$�C��U ��uŋ|$ �L$�A�Q�L$�A�D$�9�y�A    �A   �A   �A    �Q�A     �A$�A(    ���t �D$�T$��&    f���$�P���/��u�D$�|$�x�D� ������D$�H��D$�:�����uU�|$�T$�������Å���   �������D$��i����Q,����   �@    ����1ۃ�$��[^_]Ë�?����D$������D$�O����D$��"����D� �����������D$�,����D$�8    t�D$�@     �D$�����؃�$[^_]ËD$� �@    �ӋD$������D$�������D$    �P   1�1������������M���������C����t& ������U��WV��S���^�d�    �D$1���0 ��DظD� ������K����  �C�D� ������D$    �� �����  �M�D� �}�$������$�S,Q���D$����Z���  �|$�$�G����   �G�D� �|$������$�L$�Af% �f= ���   �F����   ���������= ���w� f �����������������ƅ�t%�D� ������ou�G����   �D� ������D� ������ku�C����   �D� ������D$d+    ��   ����[^_]Í�&    �v ���)�����= ����b���렍�&    �v �D� ����������녍�&    �T$�D$������u?�L$�|$������&    �v �D� �����������g����t& ������*�����    �|$��������t& �������/�����������-���f������UW��V��S���$d�    �D$��h��0 �]��DظD� ������C����  �C�D� ������G�m�D$�� ����$  �<$���Ƀ��������������u�M����  �D$=��? vd������D� �$������k�$t1�D� �$������$�D$d+    ��  ����[^_]Í�    �C��tȉ$������$뻍t& ���������  ������D$��t��$��tB�T$���3  1ɉT$�D$������T$�D$��������T$9��  �T$�D$� >�t& �F�t$�D$�t$��V�u�L$�T$�����Y^����   �D$�$�t$����u/����   ��   �$������$����������t$�L$9�wz��ȉ$������$������    �F��D$jP�t$V�D$P�L$������������U����D$붍t& ������o�����    �D� �����������{����t& �Ⱥ�����~����t& 1��������������D$�_���������S�����������4�����&    ��    ������   �Q���������1��D����t& �����U��W��VS���$d�    �D$�A�H踄0 ��E��D$�ƸD� ������N���8  �F�D� ������� �����  �W�G��	���  ��	���  �D� ������D$��,������Å���   �C�P���%  �@�D� ������s+^$����ۍ���tf�   �t& �1҉��;O��G��   �Cf% �f= ���  �$����@S�!���Y��ur�D� ������n�.  �D� ������D� ������T$�B�$���Bu�D$�@����  �D� �����1��T$d+    ��  ��[^_]Ít& �o�G    +�D� ��i�9��8��^$������nu�F����  ��������Å��[����@�P��u�@�D� ������s+^$����ۍ���������/����v j�D$1ҹ   ��@�@ �@(R��� P��j j ���������������G   �G    �$��p�nH��������F�@ �p(�������1ҹ   ��jR��� V�w�w���������������G   �G    �������&    ��D� �t$�\$������F����   �F�D� ������� ����/����T$�D$�|����L$����   �   ���{����D� �L$�T$������L$�T$�iu�A����   �D� �T$������T$�������&    �F�����������������v ������D�����    �D� ����������������t& �$���@�t$�L$������X�L$���Q����Q����D� ������F�����������Å�������D�����&    �v ������e���������T$�0���������D$�T�����&    f��������t�� j ���   �   �����XÍ�&    ��&    �����V�ƸD� S��������F��t�V����t�B�F�    �C���C��t��t[�D� ^������t& ��1�������D� [^�������&    �t& �����UW��V��S�˃�$�)��$d�    �D$ 1�����  �ʋJ$��$����u�����,��  ������D$�����&  �y�+�@,�ڋ<$��Y�A    �A   �A   �A    �y�A     �A$�A(    �L$���O  ��&    �z$�H��$����u�1��N�t& 1��    ��   ��    ��   �{��S 	���S �Cf�I��S �K$��$����   �C����S �S��    ����    ��	ȁ�    ��	ȁ�    ��	ȁ�    ��	ȁ�    ��	ȁ�    ��	ȁ�    ��	ȁ�    ��	ȁ�    ���u��    �T����k����R �C���T �S��    �	������3S ��    �����S ��&    	�������&    f�����  �D� ������$�@�x�D� �|$������<$��t/��&    �/   ��������Å�tf�Ń�)���uh�ޅ�uݍt& ��D� ������T$�D$�	������$�  �����D� ������D$ d+    �h  �D$��$[^_]�f���������Ņ�t��|$�G�D$�D� ������W,��U�D$ �/���Z����   �@f% �f= @t8�����h�x ������D$����SVUUh� ������D$���������S ��T$1��Ё� ���wp�B= �����  �$�T$������T$�$��t�������D� ��������$�D$����������D� ������D$�����������D$    �����������S ��&    �D� ��������   ��  ������ǅ��+  �G0�W@���   �D$�T$�ȉ��D$������mA  �L$Ƅ/�    f�GL�D$�O@�T$�@�G    ��G�D$�W�G�D$�G   �G   �G    �G     �G$�G(    ��t��t��    ��$�x���
��u�D� ������D$��P,U�D$ �l���Z��t-�@f% �f= @�T �T$�Ё� ����s�������RT �D$���������������Q����ʉ��L�����&    ��,   �I���������D� ��������������������LS �t& ������щ¸�0 �������&    ��    ������¹    ��0 ������&    �v �����U��W��VS���\$�$�3���_  ��1�1����&    ��$���8 t�x u��$���8 u�������  ����   �A�$   ���  ��  ������ƅ��	  ���tO��ؐ�H��u9��B    ��$�J܋H�J��H�J�H�J�H�J��H�J�H�J��H �J��H$��$��u���D$�����������   �p�4$����3��tH��4$��&    f��C��t)����P�����tL�s����t$������E  ZY��u�K$��$��u�1���[^_]Ít& ��t��t���1��s�����&    �������[^_]Ív ���U���������������뷍�&    �t& �����U��WV��S���D$d�    �D$1�����   �����   ��������$�0   �   ��  ������ǅ��{  ��  �Å�tU�����&    �N����t>���O�����u��$    ��������D$d+    �*  �$��[^_]Í�&    �ËM ��څ�t1�F��t(�t& �^$��u���������t��v���t�F��u܉Ӄ<$u0�D$���������$���z����h�r����t& �$   �����$��  ��,   ������$���<����$�B,�j�ډD$�D$��V�t$�L$�����ZY�������D$�t$9D$s6��&    �v �P��T$�X��k�������t���������������D$9�wԋ$������$    ����������$    ������&    ��&    �������щ¸�0 �������&    ��    ��������dc ��0 �������&    �v ��������dc ��0 �����1�Í�&    �����W�xV����։ʍHS��)��@0    1���0���s��C�0 �C�0 �C   �C   �[ [^_Í�&    ��&    ������@0��uÍv Í�&    ��    �����S��  ��d�    �D$1��    �$    ��������lT h�33 ��1ɉڍD$P�>� j�j�j ������D$����t�������������D$d+    u��[������f�f�f�f�f�f�f����������҉$�t$�L$�����Z��t&�T$�@     �@�g �P$�T$�P0�$�������Í�    ��������҉$�t$�L$�����Z��t-�T$�@     �@�g �P$�T$�P0�T$�P(�$�������Í�&    ��&    ���������҉$�t$�L$�����Z��t�T$�@     �@@g �P$�$�������Í�&    ��    ��������҉$�t$�L$�����Z��t&�T$�@     �@@g �P$�T$�P(�$�������Í�    �����S�Ph�ø�� ������Ch[�����f������W��  V�ơ    S���������   �@P    �Í@h1҉p��@�   �@��@�    �C`�F4�Chnet ������ǃ��u�{<�F41�����������u�K@�����������1Ҹ�� jV�����ZY��t�^h�Fl1�[^_É������������������獴&    �t& �����VS��������C�1�������Å�t9��   ����������  ��t�[�   �K\��C\��t7�P	�x��������������[^Í�&    �   ��������ڍ�&    �v �   ���������f������UWV��S���$�@�H0��v|�F$t�P(��t~�@L�x,�o\�_\��tg�U����uL	�x0�������t�$�@�P$���������tI�81����[^_]Ít& �1҉؉L$������L$뽍t& ���롍t& 뀍t& ������½����뻸������G\��t���������   �������땉�����������뇍�&    ��&    �����V��S���   �Ӌ@D��������B\��t��~/�ډ������1�[^Í�&    f���������ډ������1�[^Ív �J\�   ��������ډ������1�[^Í�&    �t& �����V��S���   �ӋPD�������B\��t��~)�ډ�[^�������    ��������ډ�[^�������    �J\�   ��������ډ�[^�������    �����W��V��S�@�|�����tX�Hh�É��������������ƍC\��S\��t��~��[^_Ít& ���������[^_Ív �   �������[^_Í�&    �������Ǎ�&    f������W��V��S�������t3�Hh�É���������K\�Ƹ������C\��t!��~-��[^_Í�&    �����[��^_Ít& ����������[^_Ív �   ��������ō�&    �v �����W��VS�B�p ���y�������ø    �������t�Ch�K\�@8�G�������C\��t
��~[1�^_É������1�[^_Ív �   ���������f������UW��VS���h�EL�X,�K\�s\����   �Q������   	�xX�������t�U$�ى����������u����[^_]Ív �������C\��t1���   ���$������$�̍�&    f�1҉������띍t& ��$��������$��[��^_]Í�&    f����V�����&    f�������y�����    �����V��Sd�    ���  �X�   �K\��C\��t/�P	�x�1�[^Í�&    f��Ⱥ   ������1�[^Ít& ��Ⱥ   ������1�[^Ít& �������������J\��B\��t��~�f����������&    f��   �������f�f������U��W��VS�    �\1 ��\1 tw�ƹ   1ۍ�    �BB�   @�9�B؉ȃ���\1 u�����    �P�    ����������`  �E ���3  �T$% �����[^_]Í�&    �    1�볍�&    ��&    ���������   �����1�Í�&    ��    �����UWV��S�Ӄ��:�l$ �$ǉ�������o�$����D$�G���������������D�T$������������D������[^_]Í�&    ��&    ������UWVS��$d�    �D$ 1��\$�t$�D1 �\$�\$�t$�t$�����1��@1 ��uZ�D1 ������D$�9�t(��    �P�S��    �@"  ������؋9�uދD$ d+    �&  ��$[^_]Ít& ��   �   ��  ���������   �    �@   ��@   ���P�T$�D$�B��X�\1 �8��=\1 u�=��&    ��ϋH����w�(�H�M�)�L$�Q��p�T$�������\1 uˋD$9�t�`1 �D$�L$�Q�
� \1 �`1 �D$P�T$�L$�D$�?����`� �BD�BH    X�����@1    �D$9�������T$�D$�L$�Q�
�0�D$���������f������UWV��S�ø   ��������Ÿ   ���������   ��t����[^_]Ít& �0   �   ��  ��������   ����   �@1 ��t�   �Cp�u��K4�S8�sp9�u�`� 3PH3HD	�t��Sx�Љ$������`� �pD�xH�   ������Cp�s4�{8�Cp�   ������$d�    ��t����������[^_]Ív �K����x���������۽�����'��������������&    �t& �����UW��VS��D  ���   ��$X  �L$�$�\$d�    ��$@  1҉D$�D1 �����������D$ P�L$,�T$(�D$0�n����t$(X����  ��S�3   9��    �D$щT$�  �l$�D$�T$�^49؉Ѓ� ��  �D$(�D$�D$�T$�L$�9D$�ȃ� ��  1���$�   �$   �5    �|$4�   �D$0    ���D$5R�D$Pvmli�D$Tnux ������¸O   9�FT$`����  �L��׉L\�H����t$�D` ��  ��������ǅ���  h�   �,� ��$�   P��j�T$<�����,� j|�D$DP��j�T$H�����5    �,� d�    P��j�T$T������    ��+D$T��$�1� 9�G�P���5    j �T$<�����D$��D$()�9�G�)ڃ���ƅ��;  �¹   ��������$�����������  ��)��   ������L$4$��1�Q�+D$ ��   �%�  �Q)�9�G����Y  ������D1 ������D$��$@  d+    ��  ��D  [^_]�1�1ɉ��   ����r�ǻ   �    �T$,f��$�   �4   �   �4   f��$�   �D$)�Ǆ$�   ELF9�Ǆ$�    G؍�$�   Ǆ$�       Ǆ$�       �f��$�   Ǆ$�      Ǆ$�   4   f��$�   f��$�   ���  �   �ډ�������$�ى���������P  �L$�l$1҉�Q�D$�T$��Q)������$������D$+D$ �   ��   �%�  )�9�G��D$   1��x��&    ���f  ���P  �D$����������D$�   ��������T$�$�����������   �t& �L$)�1�9Q�<$�   9�F��D$���#����D$�P�����   �C9���   C9�v~���  ��   ��\1 ��