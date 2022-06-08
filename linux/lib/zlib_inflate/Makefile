 int cpu, int wake_flags)
{
	struct rq *rq = cpu_rq(cpu);

	p->sched_remote_wakeup = !!(wake_flags & WF_MIGRATED);

	WRITE_ONCE(rq->ttwu_pending, 1);
	__smp_call_single_queue(cpu, &p->wake_entry.llist);
}

void wake_up_if_idle(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	rcu_read_lock();

	if (!is_idle_task(rcu_dereference(rq->curr)))
		goto out;

	rq_lock_irqsave(rq, &rf);
	if (is_idle_task(rq->curr))
		resched_curr(rq);
	/* Else CPU is not idle, do nothing here: */
	rq_unlock_irqrestore(rq, &rf);

out:
	rcu_read_unlock();
}

bool cpus_share_cache(int this_cpu, int that_cp