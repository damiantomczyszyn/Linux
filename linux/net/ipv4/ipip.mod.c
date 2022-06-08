_wake_remote);
				break;
			}
		}
		rcu_read_unlock();
	}

	if (wake_flags & WF_MIGRATED)
		__schedstat_inc(p->stats.nr_wakeups_migrate);
#endif /* CONFIG_SMP */

	__schedstat_inc(rq->ttwu_count);
	__schedstat_inc(p->stats.nr_wakeups);

	if (wake_flags & WF_SYNC)
		__schedstat_inc(p->stats.nr_wakeups_sync);
}

/*
 * Mark the task runnable and perform wakeup-preemption.
 */
static void ttwu_do_wakeup(struct rq *rq, struct task_struct *p, int wake_flags,
			   struct rq_flags *rf)
{
	check_preempt_curr(rq, p, wake_flags);
	WRITE_ONCE(p->__state, TASK_RUNNING);
	trace_sched_wakeup(p);

#ifdef CONFIG_SMP
	if (p->sched_cl