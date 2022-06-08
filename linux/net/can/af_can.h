p_assert_rq_held(rq);

	/*
	 * Ensure the thing is persistent until balance_push_set(.on = false);
	 */
	rq->balance_callback = &balance_push_callback;

	/*
	 * Only active while going offline and when invoked on the outgoing
	 * CPU.
	 */
	if (!cpu_dying(rq->cpu) || rq != this_rq())
		return;

	/*
	 * Both the cpu-hotplug and stop task are in this case and are
	 * required to complete the hotplug process.
	 */
	if (kthread_is_per_cpu(push_task) ||
	    is_migration_disabled(push_task)) {

		/*
		 * If this is the idle task on the outgoing CPU try to wake
		 * up the hotplug control thread which might wait for the
		 * last task to vanish. The rcuwait_active() check is
		 * accurate here because the waiter is pinned on this CPU
		 * and can't obviously be running in parallel.
		 *
		 * On RT kernels this also has to check whether there are
		 * pinned and scheduled out tasks on the runqueue. They
		 * need to leave the migrate disabled section first.
		 */
		if (!rq->nr_running && !rq_has_pinned_tasks(rq) &&
		    rcuwait_active(&rq->hotplug_wait)) {
			raw_spin_rq_unlock(rq);
			rcuwait_wake_up(&rq->hotplug_wait);
			raw_spin_rq_lock(rq);
		}
		return;
	}

	get_task_struct(push_task);
	/*
	 * Temporarily drop rq->lock such that we can wake-up the stop task.
	 * Both preemption and IRQs are still disabled.
	 */
	raw_spin_rq_unlock(rq);
	stop_one_cpu_nowait(rq->cpu, __balance_push_cpu_stop, push_task,
			    this_cpu_ptr(&push_work));
	/*
	 * At this point need_resched() is true and we'll take the loop in
	 * schedule(). The next pick is obviously going to be the stop task
	 * which kthread_is_per_cpu() and will push this task away.
	 */
	raw_spin_rq_lock(rq);
}

static void balance_push_set(int cpu, bool on)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	rq_lock_irqsave(rq, &rf);
	if (on) {
		WARN_ON_ONCE(rq->balance_callback);
		rq->balance_callback = &balance_push_callback;
	} else if (rq->balance_callback == &balance_push_callback) {
		rq->balance_callback = NULL;
	}
	rq_unlock_irqrestore(rq, &rf);
}

/*
 * Invoked from a CPUs hotplug control thread after the CPU has been marked
 * inactive. All tasks which are not per CPU kernel threads are either
 * pushed off this CPU now via balance_push() or placed on a different CPU
 * during wakeup. Wait until the CPU is quiescent.
 */
static void balance_hotplug_wait(void)
{
	struct rq *rq = this_rq();

	rcuwait_wait_event(&rq->hotplug_wait,
			   rq->nr_running == 1 && !rq_has_pinned_tasks(rq),
			   TASK_UNINTERRUPTIBLE);
}

#else

static inline void balance_push(struct rq *rq)
{
}

static inline void balance_push_set(int cpu, bool on)
{
}

static inline void balance_hotplug_wait(void)
{
}

#endif /* CONFIG_HOTPLUG_CPU */

void set_rq_online(struct rq *rq)
{
	if (!rq->online) {
		const struct sched_class *class;

		cpumask_set_cpu(rq->cpu, rq->rd->online);
		rq->online = 1;

		for_each_class(class) {
			if (class->rq_online)
				class->rq_online(rq);
		}
	}
}

void set_rq_offline(struct rq *rq)
{
	if (rq->online) {
		const struct sched_class *class;

		for_each_class(class) {
			if (class->rq_offline)
				class->rq_offline(rq);
		}

		cpumask_clear_cpu(rq->cpu, rq->rd->online);
		rq->online = 0;
	}
}

/*
 * used to mark begin/end of suspend/resume:
 */
static int num_cpus_frozen;

/*
 * Update cpusets according to cpu_active mask.  If cpusets are
 * disabled, cpuset_update_active_cpus() becomes a simple wrapper
 * around partition_sched_domains()