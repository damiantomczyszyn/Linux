f (should_resched(0)) {
		preempt_schedule_common();
		return 1;
	}
	/*
	 * In preemptible kernels, ->rcu_read_lock_nesting tells the tick
	 * whether the current CPU is in an RCU read-side critical section,
	 * so the tick can report quiescent states even for CPUs looping
	 * in kernel context.  In contrast, in non-preemptible kernels,
	 * RCU readers leave no in-memory hints, which means that CPU-bound
	 * processes executing in kernel context might never report an
	 * RCU quiescent state.  Therefore, the following code causes
	 * cond_resched() to report a quiescent state, but only when RCU
	 * is in urgent need of one.
	 */
#ifndef CONFIG_PREEMPT_RCU
	rcu_all_qs();
#endif
	return 0;
}
EXPORT_SYMBOL(__cond_resched);
#endif

#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#define cond_resched_dynamic_enabled	__cond_resched
#define cond_resched_dynamic_disabled	((void *)&__static_call_return0)
DEFINE_STATIC_CALL_RET0(cond_resched, __cond_resched);
EXPORT_STATIC_CALL_TRAMP(cond_resched);

#define might_resched_dynamic_enabled	__cond_resched
#define might_resched_dynamic_disabled	((void *)&__static_call_return0)
DEFINE_STATIC_CALL_RET0(might_resched, __cond_resched);
EXPORT_STATIC_CALL_TRAMP(might_resched);
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
static DEFINE_STATIC_KEY_FALSE(sk_dynamic_cond_resched);
int __sched dynamic_cond_resched(void)
{
	if (!static_branch_unlikely(&sk_dynamic_cond_resched))
		return 0;
	return __cond_resched();
}
EXPORT_SYMBOL(dynamic_cond_resched);

static DEFINE_STATIC_KEY_FALSE(sk_dynamic_might_resched);
int __sched dynamic_might_resched(void)
{
	if (!static_branch_unlikely(&sk_dynamic_might_resched))
		return 0;
	return __cond_resched();
}
EXPORT_SYMBOL(dynamic_might_resched);
#endif
#endif

/*
 * __cond_resched_lock() - if a reschedule is pending, drop the given lock,
 * call schedule, and on return reacquire the lock.
 *
 * This works OK both with and without CONFIG_PREEMPTION. We do strange low-level
 * operations here to prevent schedule() from being called twice (once via
 