 *rq_i;
	bool need_sync;

	if (!sched_core_enabled(rq))
		return __pick_next_task(rq, prev, rf);

	cpu = cpu_of(rq);

	/* Stopper task is switching into idle, no need core-wide selection. */
	if (cpu_is_offline(cpu)) {
		/*
		 * Reset core_pick so that we don't enter the fastpath when
		 * coming online. core_pick would already be migrated to
		 * another cpu during offline.
		 */
		rq->core_pick = NULL;
		return __pick_next_task(rq, prev, rf);
	}

	/*
	 * If there were no {en,de}queues since we picked (IOW, the task
	 * pointers are all still valid), and we haven't scheduled the last
	 * pick yet, do so now.
	 *
	 * rq->core_pick can be NULL if no selection was made for a CPU because
	 * it was either offline or went offline during a sibling's core-wide
	 * selection. In this case, do a core-wide selection.
	 */
	if (rq->core->core_pick_seq == rq->core->core_task_seq &&
	    rq->core->core_pick_seq != rq->core_sched_seq &&
	    rq->core_pick) {
		WRITE_ONCE(rq->core_sched_seq, rq->core->core_pick_seq);

		next = rq->core_pick;
		if (next != prev) {
			put_prev_task(rq, prev);
			set_next_task(rq, next);
		}

		rq->core_pick = NULL;
		goto out;
	}

	put_prev_task_balance(rq, prev, rf);

	smt_mask = cpu_smt_mask(cpu);
	need_sync = !!rq->core->core_cookie;

	/* reset state */
	rq->core->core_cookie = 0UL;
	if (rq->core->core_forceidle_count) {
		if (!core_clock_updated) {
			update_rq_clock(rq->core);
			core_clock_updated = true;
		}
		sched_core_account_forceidle(rq);
		/* reset after accounting force idle */
		rq->core->core_forceidle_start = 0;
		rq->core->core_forceidle_count = 0;
		rq->core->core_forceidle_occupation = 0;
		need_sync = true;
		fi_before = true;
	}

	/*
	 * core->core_task_seq, core->core_pick_seq, rq->core_sched_seq
	 *
	 * @task_seq guards the task state ({en,de}queues)
	 * @pick_seq is the @task_seq we did a selection on
	 * @sched_seq is the @pick_seq we scheduled
	 *
	 * However, preemptions can cause multiple picks on the same task set.
	 * 'Fix' this by also increasing @task_seq for every pick.
	 */
	rq->core->core_task_seq++;

	/*
	 * Optimize for common case where this CPU has no cookies
	 * and there are no cookied tasks running on siblings.
	 */
	if (!need_sync) {
		next = pick_task(rq);
		if (!next->core_cookie) {
			rq->core_pick = NULL;
			/*
			 * For robustness, update the min_vruntime_fi for
			 * unconstrained picks as well.
			 */
			WARN_ON_ONCE(fi_before);
			task_vruntime_update(rq, next, false);
			goto out_set_next;
		}
	}

	/*
	 * For each thread: do the regular task pick and find the max prio task
	 * amongst them.
	 *
	 * Tie-break prio towards the current CPU
	 */
	for_each_cpu_wrap(i, smt_mask, cpu) {
		rq_i = cpu_rq(i);

		/*
		 * Current cpu always has its clock updated on entrance to
		 * pick_next_task(). If the current cpu is not the core,
		 * the core may also have been updated above.
		 */
		if (i != cpu && (rq_i != rq->core || !core_clock_updated))
			update_rq_clock(rq_i);

		p = rq_i->core_pick = pick_task(rq_i);
		if (!max || prio_less(max, p, fi_before))
			max = p;
	}

	cookie = rq->core->core_cookie = max->core_cookie;

	/*
	 * For each thread: try and find a runnable task that matches @max or
	 * force idle.
	 */
	for_each_cpu(i, smt_mask) {
		rq_i = cpu_rq(i);
		p = rq_i->core_pick;

		if (!cookie_equals(p, cookie)) {
			p = NULL;
			if (cookie)
				p = sched_core_find(rq_i, cookie);
			if (!p)
				p = idle_sched_class.pick_task(rq_i);
		}

		rq_i->core_pick = p;

		if (p == rq_i->idle) {
			if (rq_i->nr_running) {
				rq->core->core_forceidle_count++;
				if (!fi_before)
					rq->core->core_forceidle_seq++;
			}
		} else {
			occ++;
		}
	}

	if (schedstat_enabled() && rq->core->core_forceidle_count) {
		rq->core->core_forceidle_start = rq_clock(rq->core);
		rq->core->core_forceidle_occupation = occ;
	}

	rq->core->core_pick_seq = rq->core->core_task_seq;
	next = rq->core_pick;
	rq->core_sched_seq = rq->core->core_pick_seq;

	/* Something should have been selected for current CPU */
	WARN_ON_ONCE(!next);

	/*
	 * Reschedule siblings
	 *
	 * NOTE: L1TF -- at this point we're no longer running the old task and
	 * sending an IPI (below) ensures the sibling will no longer be running
	 * their task. This ensures there is no inter-sibling overlap between
	 * non-matching user state.
	 */
	for_each_cpu(i, smt_mask) {
		rq_i = cpu_rq(i);

		/*
		 * An online sibling might have gone offline before a task
		 * could be picked for it, or it might be offline but later
		 * happen to come online, but its too late and nothing was
		 * picked for it.  That's Ok - it will pick tasks for itself,
		 * so ignore it.
		 */
		if (!rq_i->core_pick)
			continue;

		/*
		 * Update for new !FI->FI transitions, or if continuing to be in !FI:
		 * fi_before     fi      update?
		 *  0            0       1
		 *  0            1       1
		 *  1            0       1
		 *  1            1       0
		 */
		if (!(fi_before && rq->core->core_forceidle_count))
			task_vruntime_update(rq_i, rq_i->core_pick, !!rq->core->core_forceidle_count);

		rq_i->core_pick->core_occupation = occ;

		if (i == cpu) {
			rq_i->core_pick = NULL;
			continue;
		}

		/* Did we break L1TF mitigation requirements? */
		WARN_ON_ONCE(!cookie_match(next, rq_i->core_pick));

		if (rq_i->curr == rq_i->core_pick) {
			rq_i->core_pick = NULL;
			continue;
		}

		resched_curr(rq_i);
	}

out_set_next:
	set_next_task(rq, next);
out:
	if (rq->core->core_forceidle_count && next == rq->idle)
		queue_core_balance(rq);

	return next;
}

static bool try_steal_cookie(int this, int that)
{
	struct rq *dst = cpu_rq(this), *src = cpu_rq(that);
	struct task_struct *p;
	unsigned long cookie;
	bool success = false;

	local_irq_disable();
	double_rq_lock(dst, src);

	cookie = dst->core->core_cookie;
	if (!cookie)
		goto unlock;

	if (dst->curr != dst->idle)
		goto unlock;

	p = sched_core_find(src, cookie);
	if (p == src->idle)
		goto unlock;

	do {
		if (p == src->core_pick || p == src->curr)
			goto next;

		if (!is_cpu_allowed(p, this))
			goto next;

		if (p->core_occupation > dst->idle->core_occupation)
			goto next;

		deactivate_task(src, p, 0);
		set_task_cpu(p, this);
		activate_task(dst, p, 0);

		resched_curr(dst);

		success = true;
		break;

next:
		p = sched_core_next(p, cookie);
	} while (p);

unlock:
	double_rq_unlock(dst, src);
	local_irq_enable();

	return success;
}

static bool steal_cookie_task(int cpu, struct sched_domain *sd)
{
	int i;

	for_each_cpu_wrap(i, sched_domain_span(sd), cpu) {
		if (i == cpu)
			continue;

		if (need_resched())
			break;

		if (try_steal_cookie(cpu, i))
			return true;
	}

	return false;
}

static void sched_core_balance(struct rq *rq)
{
	struct sched_domain *sd;
	int cpu = cpu_of(rq);

	preempt_disable();
	rcu_read_lock();
	raw_spin_rq_unlock_irq(rq);
	for_each_domain(cpu, sd) {
		if (need_resched())
			break;

		if (steal_cookie_task(cpu, sd))
			break;
	}
	raw_spin_rq_lock_irq(rq);
	rcu_read_unlock();
	preempt_enable();
}

static DEFINE_PER_CPU(struct callback_head, core_balance_head);

static void queue_core_balance(struct rq *rq)
{
	if (!sched_core_enabled(rq))
		return;

	if (!rq->core->core_cookie)
		return;

	if (!rq->nr_running