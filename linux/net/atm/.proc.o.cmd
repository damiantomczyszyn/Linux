 Push furthest requested GP to leaf node and rcu_data structure. */
	if (ULONG_CMP_LT(gp_seq_req, rnp->gp_seq_needed)) {
		WRITE_ONCE(rnp_start->gp_seq_needed, rnp->gp_seq_needed);
		WRITE_ONCE(rdp->gp_seq_needed, rnp->gp_seq_needed);
	}
	if (rnp != rnp_start)
		raw_spin_unlock_rcu_node(rnp);
	return ret;
}

/*
 * Clean up any old requests for the just-ended grace period.  Also return
 * whether any additional grace periods have been requested.
 */
static bool rcu_future_gp_cleanup(struct rcu_node *rnp)
{
	bool needmore;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);

	needmore = ULONG_CMP_LT(rnp->gp_seq, rnp->gp_seq_needed);
	if (!needmore)
		rnp->gp_seq_needed = rnp->gp_seq; /* Avoid counter wrap. */
	trace_rcu_this_gp(rnp, rdp, rnp->gp_seq,
			  needmore ? TPS("CleanupMore") : TPS("Cleanup"));
	return needmore;
}

/*
 * Awaken the grace-period kthread.  Don't do a self-awaken (unless in an
 * interrupt or softirq handler, in which case we just might immediately
 * sleep upon return, resulting in a grace-period hang), and don't bother
 * awakening when there is nothing for the grace-period kthread to do
 * (as in several CPUs raced to awaken, we lost), and finally don't try
 * to awaken a kthread that has not yet been created.  If all those checks
 * are passed, track some debug information and awaken.
 *
 * So why do the self-wakeup when in an interrupt or softirq handler
 * in the grace-period kthread's context?  Because the kthread might have
 * been interrupted just as it was going to sleep, and just after the final
 * pre-sleep check of the awaken condition.  In this case, a wakeup really
 * is required, and is therefore supplied.
 */
static void rcu_gp_kthread_wake(void)
{
	struct task_struct *t = READ_ONCE(rcu_state.gp_kthread);

	if ((current == t && !in_hardirq() && !in_serving_softirq()) ||
	    !READ_ONCE(rcu_state.gp_flags) || !t)
		return;
	WRITE_ONCE(rcu_state.gp_wake_time, jiffies);
	WRITE_ONCE(rcu_state.gp_wake_seq, READ_ONCE(rcu_state.gp_seq));
	swake_up_one(&rcu_state.gp_wq);
}

/*
 * If there is room, assign a ->gp_seq number to any callbacks on this
 * CPU that have not already been assigned.  Also accelerate any callbacks
 * that were previously assigned a ->gp_seq number that has since proven
 * to be too conservative, which can happen if callbacks get assigned a
 * ->gp_seq number while RCU is idle, but with reference to a non-root
 * rcu_node structure.  This function is idempotent, so it does not hurt
 * to call it repeatedly.  Returns an flag saying that we should awaken
 * the RCU grace-period kthread.
 *
 * The caller must hold rnp->lock with interrupts disabled.
 */
static bool rcu_accelerate_cbs(struct rcu_node *rnp, struct rcu_data *rdp)
{
	unsigned long gp_seq_req;
	bool ret = false;

	rcu_lockdep_assert_cblist_protected(rdp);
	raw_lockdep_assert_held_rcu_node(rnp);

	/* If no pending (not yet ready to invoke) callbacks, nothing to do. */
	if (!rcu_segcblist_pend_cbs(&rdp->cblist))
		return false;

	trace_rcu_segcb_stats(&rdp->cblist, TPS("SegCbPreAcc"));

	/*
	 * Callbacks are often registered with incomplete grace-period
	 * information.  Something about the fact that getting exact
	 * information requires acquiring a global lock...  RCU therefore
	 * makes a conservative estimate of the grace period number at which
	 * a given callback will become ready to invoke.	The following
	 * code checks this estimate and improves it when possible, thus
	 * accelerating callback invocation to an earlier grace-period
	 * number.
	 */
	gp_seq_req = rcu_seq_snap(&rcu_state.gp_seq);
	if (rcu_segcblist_accelerate(&rdp->cblist, gp_seq_req))
		ret = rcu_start_this_gp(rnp, rdp, gp_seq_req);

	/* Trace depending on how much we were able to accelerate. */
	if (rcu_segcblist_restempty(&rdp->cblist, RCU_WAIT_TAIL))
		trace_rcu_grace_period(rcu_state.name, gp_seq_req, TPS("AccWaitCB"));
	else
		trace_rcu_grace_period(rcu_state.name, gp_seq_req, TPS("AccReadyCB"));

	trace_rcu_segcb_stats(&rdp->cblist, TPS("SegCbPostAcc"));

	return ret;
}

/*
 * Similar to rcu_accelerate_cbs(), but does not require that the leaf
 * rcu_node structure's ->lock be held.  It consults the cached value
 * of ->gp_seq_needed in the rcu_data structure, and if that indicates
 * that a new grace-period request be made, invokes rcu_accelerate_cbs()
 * while holding the leaf rcu_node structure's ->lock.
 */
static void rcu_accelerate_cbs_unlocked(struct rcu_node *rnp,
					struct rcu_data *rdp)
{
	unsigned long c;
	bool needwake;

	rcu_lockdep_assert_cblist_protected(rdp);
	c = rcu_seq_snap(&rcu_state.gp_seq);
	if (!READ_ONCE(rdp->gpwrap) && ULONG_CMP_GE(rdp->gp_seq_needed, c)) {
		/* Old request still live, so mark recent callbacks. */
		(void)rcu_segcblist_accelerate(&rdp->cblist, c);
		return;
	}
	raw_spin_lock_rcu_node(rnp); /* irqs already disabled. */
	needwake = rcu_accelerate_cbs(rnp, rdp);
	raw_spin_unlock_rcu_node(rnp); /* irqs remain disabled. */
	if (needwake)
		rcu_gp_kthread_wake();
}

/*
 * Move any callbacks whose grace period has completed to the
 * RCU_DONE_TAIL sublist, then compact the remaining sublists and
 * assign ->gp_seq numbers to any callbacks in the RCU_NEXT_TAIL
 * sublist.  This function is idempotent, so it does not hurt to
 * invoke it repeatedly.  As long as it is not invoked -too- often...
 * Returns true if the RCU grace-period kthread needs to be awakened.
 *
 * The caller must hold rnp->lock with interrupts disabled.
 */
static bool rcu_advance_cbs(struct rcu_node *rnp, struct rcu_data *rdp)
{
	rcu_lockdep_assert_cblist_protected(rdp);
	raw_lockdep_assert_held_rcu_node(rnp);

	/* If no pending (not yet ready to invoke) callbacks, nothing to do. */
	if (!rcu_segcblist_pend_cbs(&rdp->cblist))
		return false;

	/*
	 * Find all callbacks whose ->gp_seq numbers indicate that they
	 * are ready to invoke, and put them into the RCU_DONE_TAIL sublist.
	 */
	rcu_segcblist_advance(&rdp->cblist, rnp->gp_seq);

	/* Classify any remaining callbacks. */
	return rcu_accelerate_cbs(rnp, rdp);
}

/*
 * Move and classify callbacks, but only if doing so won't require
 * that the RCU grace-period kthread be awakened.
 */
static void __maybe_unused rcu_advance_cbs_nowake(struct rcu_node *rnp,
						  struct rcu_data *rdp)
{
	rcu_lockdep_assert_cblist_protected(rdp);
	if (!rcu_seq_state(rcu_seq_current(&rnp->gp_seq)) || !raw_spin_trylock_rcu_node(rnp))
		return;
	// The grace period cannot end while we hold the rcu_node lock.
	if (rcu_seq_state(rcu_seq_current(&rnp->gp_seq)))
		WARN_ON_ONCE(rcu_advance_cbs(rnp, rdp));
	raw_spin_unlock_rcu_node(rnp);
}

/*
 * In CONFIG_RCU_STRICT_GRACE_PERIOD=y kernels, attempt to generate a
 * quiescent state.  This is intended to be invoked when the CPU notices
 * a new grace period.
 */
static void rcu_strict_gp_check_qs(void)
{
	if (IS_ENABLED(CONFIG_RCU_STRICT_GRACE_PERIOD)) {
		rcu_read_lock();
		rcu_read_unlock();
	}
}

/*
 * Update CPU-local rcu_data state to record the beginnings and ends of
 * grace periods.  The caller must hold the ->lock of the leaf rcu_node
 * structure corresponding to the current CPU, and must have irqs disabled.
 * Returns true if the grace-period kthread needs to be awakened.
 */
static bool __note_gp_changes(struct rcu_node *rnp, struct rcu_data *rdp)
{
	bool ret = false;
	bool need_qs;
	const bool offloaded = rcu_rdp_is_offloaded(rdp);

	raw_lockdep_assert_held_rcu_node(rnp);

	if (rdp->gp_seq == rnp->gp_seq)
		return false; /* Nothing to do. */

	/* Handle the ends of any preceding grace periods first. */
	if (rcu_seq_completed_gp(rdp->gp_seq, rnp->gp_seq) ||
	    unlikely(READ_ONCE(rdp->gpwrap))) {
		if (!offloaded)
			ret = rcu_advance_cbs(rnp, rdp); /* Advance CBs. */
		rdp->core_needs_qs = false;
		trace_rcu_grace_period(rcu_state.name, rdp->gp_seq, TPS("cpuend"));
	} else {
		if (!offloaded)
			ret = rcu_accelerate_cbs(rnp, rdp); /* Recent CBs. */
		if (rdp->core_needs_qs)
			rdp->core_needs_qs = !!(rnp->qsmask & rdp->grpmask);
	}

	/* Now handle the beginnings of any new-to-this-CPU grace periods. */
	if (rcu_seq_new_gp(rdp->gp_seq, rnp->gp_seq) ||
	    unlikely(READ_ONCE(rdp->gpwrap))) {
		/*
		 * If the current grace period is waiting for this CPU,
		 * set up to detect a quiescent state, otherwise don't
		 * go looking for one.
		 */
		trace_rcu_grace_period(rcu_state.name, rnp->gp_seq, TPS("cpustart"));
		need_qs = !!(rnp->qsmask & rdp->grpmask);
		rdp->cpu_no_qs.b.norm = need_qs;
		rdp->core_needs_qs = need_qs;
		zero_cpu_stall_ticks(rdp);
	}
	rdp->gp_seq = rnp->gp_seq;  /* Remember new grace-period state. */
	if (ULONG_CMP_LT(rdp->gp_seq_needed, rnp->gp_seq_needed) || rdp->gpwrap)
		WRITE_ONCE(rdp->gp_seq_needed, rnp->gp_seq_needed);
	WRITE_ONCE(rdp->gpwrap, false);
	rcu_gpnum_ovf(rnp, rdp);
	return ret;
}

static void note_gp_changes(struct rcu_data *rdp)
{
	unsigned long flags;
	bool needwake;
	struct rcu_node *rnp;

	local_irq_save(flags);
	rnp = rdp->mynode;
	if ((rdp->gp_seq == rcu_seq_current(&rnp->gp_seq) &&
	     !unlikely(READ_ONCE(rdp->gpwrap))) || /* w/out lock. */
	    !raw_spin_trylock_rcu_node(rnp)) { /* irqs already off, so later. */
		local_irq_restore(flags);
		return;
	}
	needwake = __note_gp_changes(rnp, rdp);
	raw_spin_unlock_irqrestore_rcu_node(rnp, flags);
	rcu_strict_gp_check_qs();
	if (needwake)
		rcu_gp_kthread_wake();
}

static void rcu_gp_slow(int delay)
{
	if (delay > 0 &&
	    !(rcu_seq_ctr(rcu_state.gp_seq) %
	      (rcu_num_nodes * PER_RCU_NODE_PERIOD * delay)))
		schedule_timeout_idle(delay);
}

static unsigned long sleep_duration;

/* Allow rcutorture to stall the grace-period kthread. */
void rcu_gp_set_torture_wait(int duration)
{
	if (IS_ENABLED(CONFIG_RCU_TORTURE_TEST) && duration > 0)
		WRITE_ONCE(sleep_duration, duration);
}
EXPORT_SYMBOL_GPL(rcu_gp_set_torture_wait);

/* Actually implement the aforementioned wait. */
static void rcu_gp_torture_wait(void)
{
	unsigned long duration;

	if (!IS_ENABLED(CONFIG_RCU_TORTURE_TEST))
		return;
	duration = xchg(&sleep_duration, 0UL);
	if (duration > 0) {
		pr_alert("%s: Waiting %lu jiffies\n", __func__, duration);
		schedule_timeout_idle(duration);
		pr_alert("%s: Wait complete\n", __func__);
	}
}

/*
 * Handler for on_each_cpu() to invoke the target CPU's RCU core
 * processing.
 */
static void rcu_strict_gp_boundary(void *unused)
{
	invoke_rcu_core();
}

/*
 * Initialize a new grace period.  Return false if no grace period required.
 */
static noinline_for_stack bool rcu_gp_init(void)
{
	unsigned long flags;
	unsigned long oldmask;
	unsigned long mask;
	struct rcu_data *rdp;
	struct rcu_node *rnp = rcu_get_root();

	WRITE_ONCE(rcu_state.gp_activity, jiffies);
	raw_spin_lock_irq_rcu_node(rnp);
	if (!READ_ONCE(rcu_state.gp_flags)) {
		/* Spurious wakeup, tell caller to go back to sleep.  */
		raw_spin_unlock_irq_rcu_node(rnp);
		return false;
	}
	WRITE_ONCE(rcu_state.gp_flags, 0); /* Clear all flags: New GP. */

	if (WARN_ON_ONCE(rcu_gp_in_progress())) {
		/*
		 * Grace period already in progress, don't start another.
		 * Not supposed to be able to happen.
		 */
		raw_spin_unlock_irq_rcu_node(rnp);
		return false;
	}

	/* Advance to a new grace period and initialize state. */
	record_gp_stall_check_time();
	/* Record GP times before starting GP, hence rcu_seq_start(). */
	rcu_seq_start(&rcu_state.gp_seq);
	ASSERT_EXCLUSIVE_WRITER(rcu_state.gp_seq);
	trace_rcu_grace_period(rcu_state.name, rcu_state.gp_seq, TPS("start"));
	raw_spin_unlock_irq_rcu_node(rnp);

	/*
	 * Apply per-leaf buffered online and offline operations to
	 * the rcu_node tree. Note that this new grace period need not
	 * wait for subsequent online CPUs, and that RCU hooks in the CPU
	 * offlining path, when combined with checks in this function,
	 * will handle CPUs that are currently going offline or that will
	 * go offline later.  Please also refer to "Hotplug CPU" section
	 * of RCU's Requirements documentation.
	 */
	WRITE_ONCE(rcu_state.gp_state, RCU_GP_ONOFF);
	/* Exclude CPU hotplug operations. */
	rcu_for_each_leaf_node(rnp) {
		local_irq_save(flags);
		arch_spin_lock(&rcu_state.ofl_lock);
		raw_spin_lock_rcu_node(rnp);
		if (rnp->qsmaskinit == rnp->qsmaskinitnext &&
		    !rnp->wait_blkd_tasks) {
			/* Nothing to do on this leaf rcu_node structure. */
			raw_spin_unlock_rcu_node(rnp);
			arch_spin_unlock(&rcu_stant_modules();
	if (irqs_disabled())
		print_irqtrace_events(prev);
	if (IS_ENABLED(CONFIG_DEBUG_PREEMPT)
	    && in_atomic_preempt_off()) {
		pr_err("Preemption disabled at:");
		print_ip_sym(KERN_ERR, preempt_disable_ip);
	}
	if (panic_on_warn)
		panic("scheduling while atomic\n");

	dump_stack();
	add_taint(TAINT_WARN, LOCKDEP_STILL_OK);
}

/*
 * Various schedule()-time debugging checks and statistics:
 */
static inline void schedule_debug(struct task_struct *prev, bool preempt)
{
#ifdef CONFIG_SCHED_STACK_END_CHECK
	if (task_stack_end_corrupted(prev))
		panic("corrupted stack end detected inside scheduler\n");

	if (task_scs_end_corrupted(prev))
		panic("corrupted shadow stack detected inside scheduler\n");
#endif

#ifdef CONFIG_DEBUG_ATOMIC_SLEEP
	if (!preempt && READ_ONCE(prev->__state) && prev->non_block_count) {
		printk(KERN_ERR "BUG: scheduling in a non-blocking section: %s/%d/%i\n",
			prev->comm, prev->pid, prev->non_block_count);
		dump_stack();
		add_taint(TAINT_WARN, LOCKDEP_STILL_OK);
	}
#endif

	if (unlikely(in_atomic_preempt_off())) {
		__schedule_bug(prev);
		preempt_count_set(PREEMPT_DISABLED);
	}
	rcu_sleep_check();
	SCHED_WARN_ON(ct_state() == CONTEXT_USER);

	profile_hit(SCHED_PROFILING, __builtin_return_address(0));

	schedstat_inc(this_rq()->sched_count);
}

static void put_prev_task_balance(struct rq *rq, struct task_struct *prev,
				  struct rq_flags *rf)
{
#ifdef CONFIG_SMP
	const struct sched_class *class;
	/*
	 * We must do the balancing pass before put_prev_task(), such
	 * that when we release the rq->lock the task is in the same
	 * state as before we took rq->lock.
	 *
	 * We can terminate the balance pass as soon as we know there is
	 * a runnable task of @class priority or higher.
	 */
	for_class_range(class, prev->sched_class, &idle_sched_class) {
		if (class->balance(rq, prev, rf))
			break;
	}
#endif

	put_prev_task(rq, prev);
}

/*
 * Pick up the highest-prio task:
 */
static inline struct task_struct *
__pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	const struct sched_class *class;
	struct task_struct *p;

	/*
	 * Optimization: we know that if all tasks are in the fair class we can
	 * call that function directly, but only if the @prev task wasn't of a
	 * higher scheduling class, because otherwise those lose the
	 * opportunity to pull in more work from other CPUs.
	 */
	if (likely(prev->sched_class <= &fair_sched_class &&
		   rq->nr_running == rq->cfs.h_nr_running)) {

		p = pick_next_task_fair(rq, prev, rf);
		if (unlikely(p == RETRY_TASK))
			goto restart;

		/* Assume the next prioritized class is idle_sched_class */
		if (!p) {
			put_prev_task(rq, prev);
			p = pick_next_task_idle(rq);
		}

		return p;
	}

restart:
	put_prev_task_balance(rq, prev, rf);

	for_each_class(class) {
		p = class->pick_next_task(rq);
		if (p)
			return p;
	}

	BUG(); /* The idle class should always have a runnable task. */
}

#ifdef CONFIG_SCHED_CORE
static inline bool is_task_rq_idle(struct task_struct *t)
{
	return (task_rq(t)->idle == t);
}

static inline bool cookie_equals(struct task_struct *a, unsigned long cookie)
{
	return is_task_rq_idle(a) || (a->core_cookie == cookie);
}

static inline bool cookie_match(struct task_struct *a, struct task_struct *b)
{
	if (is_task_rq_idle(a) || is_task_rq_idle(b))
		return true;

	return a->core_cookie == b->core_cookie;
}

static inline struct task_struct *pick_task(struct rq *rq)
{
	const struct sched_class *class;
	struct task_struct *p;

	for_each_class(class) {
		p = class->pick_task(rq);
		if (p)
			return p;
	}

	BUG(); /* The idle class should always have a runnable task. */
}

extern void task_vruntime_update(struct rq *rq, struct task_struct *p, bool in_fi);

static void queue_core_balance(struct rq *rq);

static struct task_struct *
pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	struct task_struct *next, *p, *max = NULL;
	const struct cpumask *smt_mask;
	bool fi_before = false;
	bool core_clock_updated = (rq == rq->core);
	unsigned long cookie;
	int i, cpu, occ = 0;
	struct rq *rq_i;
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

	if (!rq->nr_running) /* not forced idle */
		return;

	queue_balance_callback(rq, &per_cpu(core_balance_head, rq->cpu), sched_core_balance);
}

static void sched_core_cpu_starting(unsigned int cpu)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	struct rq *rq = cpu_rq(cpu), *core_rq = NULL;
	unsigned long flags;
	int t;

	sched_core_lock(cpu, &flags);

	WARN_ON_ONCE(rq->core != rq);

	/* if we're the first, we'll be our own leader */
	if (cpumask_weight(smt_mask) == 1)
		goto unlock;

	/* find the leader */
	for_each_cpu(t, smt_mask) {
		if (t == cpu)
			continue;
		rq = cpu_rq(t);
		if (rq->core == rq) {
			core_rq = rq;
			break;
		}
	}

	if (WARN_ON_ONCE(!core_rq)) /* whoopsie */
		goto unlock;

	/* install and validate core_rq */
	for_each_cpu(t, smt_mask) {
		rq = cpu_rq(t);

		if (t == cpu)
			rq->core = core_rq;

		WARN_ON_ONCE(rq->core != core_rq);
	}

unlock:
	sched_core_unlock(cpu, &flags);
}

static void sched_core_cpu_deactivate(unsigned int cpu)
{
	const struct cpumask *smt_mask = cpu_smt_mask(cpu);
	struct rq *rq = cpu_rq(cpu), *core_rq = NULL;
	unsigned long flags;
	int t;

	sched_core_lock(cpu, &flags);

	/* if we're the last man standing, nothing to do */
	if (cpumask_weight(smt_mask) == 1) {
		WARN_ON_ONCE(rq->core != rq);
		goto unlock;
	}

	/* if we're not the leader, nothing to do */
	if (rq->core != rq)
		goto unlock;

	/* find a new leader */
	for_each_cpu(t, smt_mask) {
		if (t == cpu)
			continue;
		core_rq = cpu_rq(t);
		break;
	}

	if (WARN_ON_ONCE(!core_rq)) /* impossible */
		goto unlock;

	/* copy the shared state to the new leader */
	core_rq->core_task_seq             = rq->core_task_seq;
	core_rq->core_pick_seq             = rq->core_pick_seq;
	core_rq->core_cookie               = rq->core_cookie;
	core_rq->core_forceidle_count      = rq->core_forceidle_count;
	core_rq->core_forceidle_seq        = rq->core_forceidle_seq;
	core_rq->core_forceidle_occupation = rq->core_forceidle_occupation;

	/*
	 * Accounting edge for forced idle is handled in pick_next_task().
	 * Don't need another one here, since the hotplug thread shouldn't
	 * have a cookie.
	 */
	core_rq->core_forceidle_start = 0;

	/* install new leader */
	for_each_cpu(t, smt_mask) {
		rq = cpu_rq(t);
		rq->core = core_rq;
	}

unlock:
	sched_core_unlock(cpu, &flags);
}

static inline void sched_core_cpu_dying(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (rq->core != rq)
		rq->core = rq;
}

#else /* !CONFIG_SCHED_CORE */

static inline void sched_core_cpu_starting(unsigned int cpu) {}
static inline void sched_core_cpu_deactivate(unsigned int cpu) {}
static inline void sched_core_cpu_dying(unsigned int cpu) {}

static struct task_struct *
pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	return __pick_next_task(rq, prev, rf);
}

#endif /* CONFIG_SCHED_CORE */

/*
 * Constants for the sched_mode argument of __schedule().
 *
 * The mode argument allows RT enabled kernels to differentiate a
 * preemption from blocking on an 'sleeping' spin/rwlock. Note that
 * SM_MASK_PREEMPT for !RT has all bits set, which allows the compiler to
 * optimize the AND operation out and just check for zero.
 */
#define SM_NONE			0x0
#define SM_PREEMPT		0x1
#define SM_RTLOCK_WAIT		0x2

#ifndef CONFIG_PREEMPT_RT
# define SM_MASK_PREEMPT	(~0U)
#else
# define SM_MASK_PREEMPT	SM_PREEMPT
#endif

/*
 * __schedule() is the main scheduler function.
 *
 * The main means of driving the scheduler and thus entering this function are:
 *
 *   1. Explicit blocking: mutex, semaphore, waitqueue, etc.
 *
 *   2. TIF_NEED_RESCHED flag is checked on interrupt and userspace return
 *      paths. For example, see arch/x86/entry_64.S.
 *
 *      To drive preemption between tasks, the scheduler sets the flag in timer
 *      interrupt handler scheduler_tick().
 *
 *   3. Wakeups don't really cause entry into schedule(). They add a
 *      task to the run-queue and that's it.
 *
 *      Now, if the new task added to the run-queue preempts the current
 *      task, then the wakeup sets TIF_NEED_RESCHED and schedule() gets
 *      called on the nearest possible occasion:
 *
 *       - If the kernel is preemptible (CONFIG_PREEMPTION=y):
 *
 *         - in syscall or exception context, at the next outmost
 *           preempt_enable(). (this might be as soon as the wake_up()'s
 *           spin_unlock()!)
 *
 *         - in IRQ context, return from interrupt-handler to
 *           preemptible context
 *
 *       - If the kernel is not preemptible (CONFIG_PREEMPTION is not set)
 *         then at the next:
 *
 *          - cond_resched() call
 *          - explicit schedule() call
 *          - return from syscall or exception to user-space
 *          - return from interrupt-handler to user-space
 *
 * WARNING: must be called with preemption disabled!
 */
static void __sched notrace __schedule(unsigned int sched_mode)
{
	struct task_struct *prev, *next;
	unsigned long *switch_count;
	unsigned long prev_state;
	struct rq_flags rf;
	struct rq *rq;
	int cpu;

	cpu = smp_processor_id();
	rq = cpu_rq(cpu);
	prev = rq->curr;

	schedule_debug(prev, !!sched_mode);

	if (sched_feat(HRTICK) || sched_feat(HRTICK_DL))
		hrtick_clear(rq);

	local_irq_disable();
	rcu_note_context_switch(!!sched_mode);

	/*
	 * Make sure that signal_pending_state()->signal_pending() below
	 * can't be reordered with __set_current_state(TASK_INTERRUPTIBLE)
	 * done by the caller to avoid the race with signal_wake_up():
	 *
	 * __set_current_state(@state)		signal_wake_up()
	 * schedule()				  set_tsk_thread_flag(p, TIF_SIGPENDING)
	 *					  wake_up_state(p, state)
	 *   LOCK rq->lock			    LOCK p->pi_state
	 *   smp_mb__after_spinlock()		    smp_mb__after_spinlock()
	 *     if (signal_pending_state())	    if (p->state & @state)
	 *
	 * Also, the membarrier system call requires a full memory barrier
	 * after coming from user-space, before storing to rq->curr.
	 */
	rq_lock(rq, &rf);
	smp_mb__after_spinlock();

	/* Promote REQ to ACT */
	rq->clock_update_flags <<= 1;
	update_rq_clock(rq);

	switch_count = &prev->nivcsw;

	/*
	 * We must load prev->state once (task_struct::state is volatile), such
	 * that:
	 *
	 *  - we form a control dependency vs deactivate_task() below.
	 *  - ptrace_{,un}freeze_traced() can change ->state underneath us.
	 */
	prev_state = READ_ONCE(prev->__state);
	if (!(sched_mode & SM_MASK_PREEMPT) && prev_state) {
		if (signal_pending_state(prev_state, prev)) {
			WRITE_ONCE(prev->__state, TASK_RUNNING);
		} else {
			prev->sched_contributes_to_load =
				(prev_state & TASK_UNINTERRUPTIBLE) &&
				!(prev_state & TASK_NOLOAD) &&
				!(prev->flags & PF_FROZEN);

			if (prev->sched_contributes_to_load)
				rq->nr_uninterruptible++;

			/*
			 * __schedule()			ttwu()
			 *   prev_state = prev->state;    if (p->on_rq && ...)
			 *   if (prev_state)		    goto out;
			 *     p->on_rq = 0;		  smp_acquire__after_ctrl_dep();
			 *				  p->state = TASK_WAKING
			 *
			 * Where __schedule() and ttwu() have matching control dependencies.
			 *
			 * After this, schedule() must not care about p->state any more.
			 */
			deactivate_task(rq, prev, DEQUEUE_SLEEP | DEQUEUE_NOCLOCK);

			if (prev->in_iowait) {
				atomic_inc(&rq->nr_iowait);
				delayacct_blkio_start();
			}
		}
		switch_count = &prev->nvcsw;
	}

	next = pick_next_task(rq, prev, &rf);
	clear_tsk_need_resched(prev);
	clear_preempt_need_resched();
#ifdef CONFIG_SCHED_DEBUG
	rq->last_seen_need_resched_ns = 0;
#endif

	if (likely(prev != next)) {
		rq->nr_switches++;
		/*
		 * RCU users of rcu_dereference(rq->curr) may not see
		 * changes to task_struct made by pick_next_task().
		 */
		RCU_INIT_POINTER(rq->curr, next);
		/*
		 * The membarrier system call requires each architecture
		 * to have a full memory barrier after updating
		 * rq->curr, before returning to user-space.
		 *
		 * Here are the schemes providing that barrier on the
		 * various architectures:
		 * - mm ? switch_mm() : mmdrop() for x86, s390, sparc, PowerPC.
		 *   switch_mm() rely on membarrier_arch_switch_mm() on PowerPC.
		 * - finish_lock_switch() for weakly-ordered
		 *   architectures where spin_unlock is a full barrier,
		 * - switch_to() for arm64 (weakly-ordered, spin_unlock
		 *   is a RELEASE barrier),
		 */
		++*switch_count;

		migrate_disable_switch(rq, prev);
		psi_sched_switch(prev, next, !task_on_rq_queued(prev));

		trace_sched_switch(sched_mode & SM_MASK_PREEMPT, prev, next, prev_state);

		/* Also unlocks the rq: */
		rq = context_switch(rq, prev, next, &rf);
	} else {
		rq->clock_update_flags &= ~(RQCF_ACT_SKIP|RQCF_REQ_SKIP);

		rq_unpin_lock(rq, &rf);
		__balance_callbacks(rq);
		raw_spin_rq_unlock_irq(rq);
	}
}

void __noreturn do_task_dead(void)
{
	/* Causes final put_task_struct in finish_task_switch(): */
	set_special_state(TASK_DEAD);

	/* Tell freezer to ignore us: */
	current->flags |= PF_NOFREEZE;

	__schedule(SM_NONE);
	BUG();

	/* Avoid "noreturn function does return" - but don't continue if BUG() is a NOP: */
	for (;;)
		cpu_relax();
}

static inline void sched_submit_work(struct task_struct *tsk)
{
	unsigned int task_flags;

	if (task_is_running(tsk))
		return;

	task_flags = tsk->flags;
	/*
	 * If a worker goes to sleep, notify and ask workqueue whether it
	 * wants to wake up a task to maintain concurrency.
	 */
	if (task_flags & (PF_WQ_WORKER | PF_IO_WORKER)) {
		if (task_flags & PF_WQ_WORKER)
			wq_worker_sleeping(tsk);
		else
			io_wq_worker_sleeping(tsk);
	}

	if (tsk_is_pi_blocked(tsk))
		return;

	/*
	 * If we are going to sleep and we have plugged IO queued,
	 * make sure to submit it to avoid deadlocks.
	 */
	blk_flush_plug(tsk->plug, true);
}

static void sched_update_worker(struct task_struct *tsk)
{
	if (tsk->flags & (PF_WQ_WORKER | PF_IO_WORKER)) {
		if (tsk->flags & PF_WQ_WORKER)
			wq_worker_running(tsk);
		else
			io_wq_worker_running(tsk);
	}
}

asmlinkage __visible void __sched schedule(void)
{
	struct task_struct *tsk = current;

	sched_submit_work(tsk);
	do {
		preempt_disable();
		__schedule(SM_NONE);
		sched_preempt_enable_no_resched();
	} while (need_resched());
	sched_update_worker(tsk);
}
EXPORT_SYMBOL(schedule);

/*
 * synchronize_rcu_tasks() makes sure that no task is stuck in preempted
 * state (have scheduled out non-voluntarily) by making sure that all
 * tasks have either left the run queue or have gone into user space.
 * As idle tasks do not do either, they must not ever be preempted
 * (schedule out non-voluntarily).
 *
 * schedule_idle() is similar to schedule_preempt_disable() except that it
 * never enables preemption because it does not call sched_submit_work().
 */
void __sched schedule_idle(void)
{
	/*
	 * As this skips calling sched_submit_work(), which the idle task does
	 * regardless because that function is a nop when the task is in a
	 * TASK_RUNNING state, make sure this isn't used someplace that the
	 * current task can be in any other state. Note, idle is always in the
	 * TASK_RUNNING state.
	 */
	WARN_ON_ONCE(current->__state);
	do {
		__schedule(SM_NONE);
	} while (need_resched());
}

#if defined(CONFIG_CONTEXT_TRACKING) && !defined(CONFIG_HAVE_CONTEXT_TRACKING_OFFSTACK)
asmlinkage __visible void __sched schedule_user(void)
{
	/*
	 * If we come here after a random call to set_need_resched(),
	 * or we have been woken up remotely but the IPI has not yet arrived,
	 * we haven't yet exited the RCU idle mode. Do it here manually until
	 * we find a better solution.
	 *
	 * NB: There are buggy callers of this function.  Ideally we
	 * should warn if prev_state != CONTEXT_USER, but that will trigger
	 * too frequently to make sense yet.
	 */
	enum ctx_state prev_state = exception_enter();
	schedule();
	exception_exit(prev_state);
}
#endif

/**
 * schedule_preempt_disabled - called with preemption disabled
 *
 * Returns with preemption disabled. Note: preempt_count must be 1
 */
void __sched schedule_preempt_disabled(void)
{
	sched_preempt_enable_no_resched();
	schedule();
	preempt_disable();
}

#ifdef CONFIG_PREEMPT_RT
void __sched notrace schedule_rtlock(void)
{
	do {
		preempt_disable();
		__schedule(SM_RTLOCK_WAIT);
		sched_preempt_enable_no_resched();
	} while (need_resched());
}
NOKPROBE_SYMBOL(schedule_rtlock);
#endif

static void __sched notrace preempt_schedule_common(void)
{
	do {
		/*
		 * Because the function tracer can trace preempt_count_sub()
		 * and it also uses preempt_enable/disable_notrace(), if
		 * NEED_RESCHED is set, the preempt_enable_notrace() called
		 * by the function tracer will call this function again and
		 * cause infinite recursion.
		 *
		 * Preemption must be disabled here before the function
		 * tracer can trace. Break up preempt_disable() into two
		 * calls. One to disable preemption without fear of being
		 * traced. The other to still record the preemption latency,
		 * which can also be traced by the function tracer.
		 */
		preempt_disable_notrace();
		preempt_latency_start(1);
		__schedule(SM_PREEMPT);
		preempt_latency_stop(1);
		preempt_enable_no_resched_notrace();

		/*
		 * Check again in case we missed a preemption opportunity
		 * between schedule and now.
		 */
	} while (need_resched());
}

#ifdef CONFIG_PREEMPTION
/*
 * This is the entry point to schedule() from in-kernel preemption
 * off of preempt_enable.
 */
asmlinkage __visible void __sched notrace preempt_schedule(void)
{
	/*
	 * If there is a non-zero preempt_count or interrupts are disabled,
	 * we do not want to preempt the current task. Just return..
	 */
	if (likely(!preemptible()))
		return;
	preempt_schedule_common();
}
NOKPROBE_SYMBOL(preempt_schedule);
EXPORT_SYMBOL(preempt_schedule);

#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#ifndef preempt_schedule_dynamic_enabled
#define preempt_schedule_dynamic_enabled	preempt_schedule
#define preempt_schedule_dynamic_disabled	NULL
#endif
DEFINE_STATIC_CALL(preempt_schedule, preempt_schedule_dynamic_enabled);
EXPORT_STATIC_CALL_TRAMP(preempt_schedule);
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
static DEFINE_STATIC_KEY_TRUE(sk_dynamic_preempt_schedule);
void __sched notrace dynamic_preempt_schedule(void)
{
	if (!static_branch_unlikely(&sk_dynamic_preempt_schedule))
		return;
	preempt_schedule();
}
NOKPROBE_SYMBOL(dynamic_preempt_schedule);
EXPORT_SYMBOL(dynamic_preempt_schedule);
#endif
#endif

/**
 * preempt_schedule_notrace - preempt_schedule called by tracing
 *
 * The tracing infrastructure uses preempt_enable_notrace to prevent
 * recursion and tracing preempt enabling caused by the tracing
 * infrastructure itself. But as tracing can happen in areas coming
 * from userspace or just about to enter userspace, a preempt enable
 * can occur before user_exit() is called. This will cause the scheduler
 * to be called when the system is still in usermode.
 *
 * To prevent this, the preempt_enable_notrace will use this function
 * instead of preempt_schedule() to exit user context if needed before
 * calling the scheduler.
 */
asmlinkage __visible void __sched notrace preempt_schedule_notrace(void)
{
	enum ctx_state prev_ctx;

	if (likely(!preemptible()))
		return;

	do {
		/*
		 * Because the function tracer can trace preempt_count_sub()
		 * and it also uses preempt_enable/disable_notrace(), if
		 * NEED_RESCHED is set, the preempt_enable_notrace() called
		 * by the function tracer will call this function again and
		 * cause infinite recursion.
		 *
		 * Preemption must be disabled here before the function
		 * tracer can trace. Break up preempt_disable() into two
		 * calls. One to disable preemption without fear of being
		 * traced. The other to still record the preemption latency,
		 * which can also be traced by the function tracer.
		 */
		preempt_disable_notrace();
		preempt_latency_start(1);
		/*
		 * Needs preempt disabled in case user_exit() is traced
		 * and the tracer calls preempt_enable_notrace() causing
		 * an infinite recursion.
		 */
		prev_ctx = exception_enter();
		__schedule(SM_PREEMPT);
		exception_exit(prev_ctx);

		preempt_latency_stop(1);
		preempt_enable_no_resched_notrace();
	} while (need_resched());
}
EXPORT_SYMBOL_GPL(preempt_schedule_notrace);

#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#ifndef preempt_schedule_notrace_dynamic_enabled
#define preempt_schedule_notrace_dynamic_enabled	preempt_schedule_notrace
#define preempt_schedule_notrace_dynamic_disabled	NULL
#endif
DEFINE_STATIC_CALL(preempt_schedule_notrace, preempt_schedule_notrace_dynamic_enabled);
EXPORT_STATIC_CALL_TRAMP(preempt_schedule_notrace);
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
static DEFINE_STATIC_KEY_TRUE(sk_dynamic_preempt_schedule_notrace);
void __sched notrace dynamic_preempt_schedule_notrace(void)
{
	if (!static_branch_unlikely(&sk_dynamic_preempt_schedule_notrace))
		return;
	preempt_schedule_notrace();
}
NOKPROBE_SYMBOL(dynamic_preempt_schedule_notrace);
EXPORT_SYMBOL(dynamic_preempt_schedule_notrace);
#endif
#endif

#endif /* CONFIG_PREEMPTION */

/*
 * This is the entry point to schedule() from kernel preemption
 * off of irq context.
 * Note, that this is called and return with irqs disabled. This will
 * protect us against recursive calling from irq.
 */
asmlinkage __visible void __sched preempt_schedule_irq(void)
{
	enum ctx_state prev_state;

	/* Catch callers which need to be fixed */
	BUG_ON(preempt_count() || !irqs_disabled());

	prev_state = exception_enter();

	do {
		preempt_disable();
		local_irq_enable();
		__schedule(SM_PREEMPT);
		local_irq_disable();
		sched_preempt_enable_no_resched();
	} while (need_resched());

	exception_exit(prev_state);
}

int default_wake_function(wait_queue_entry_t *curr, unsigned mode, int wake_flags,
			  void *key)
{
	WARN_ON_ONCE(IS_ENABLED(CONFIG_SCHED_DEBUG) && wake_flags & ~WF_SYNC);
	return try_to_wake_up(curr->private, mode, wake_flags);
}
EXPORT_SYMBOL(default_wake_function);

static void __setscheduler_prio(struct task_struct *p, int prio)
{
	if (dl_prio(prio))
		p->sched_class = &dl_sched_class;
	else if (rt_prio(prio))
		p->sched_class = &rt_sched_class;
	else
		p->sched_class = &fair_sched_class;

	p->prio = prio;
}

#ifdef CONFIG_RT_MUTEXES

static inline int __rt_effective_prio(struct task_struct *pi_task, int prio)
{
	if (pi_task)
		prio = min(prio, pi_task->prio);

	return prio;
}

static inline int rt_effective_prio(struct task_struct *p, int prio)
{
	struct task_struct *pi_task = rt_mutex_get_top_task(p);

	return __rt_effective_prio(pi_task, prio);
}

/*
 * rt_mutex_setprio - set the current priority of a task
 * @p: task to boost
 * @pi_task: donor task
 *
 * This function changes the 'effective' priority of a task. It does
 * not touch ->normal_prio like __setscheduler().
 *
 * Used by the rt_mutex code to implement priority inheritance
 * logic. Call site only calls if the priority of the task changed.
 */
void rt_mutex_setprio(struct task_struct *p, struct task_struct *pi_task)
{
	int prio, oldprio, queued, running, queue_flag =
		DEQUEUE_SAVE | DEQUEUE_MOVE | DEQUEUE_NOCLOCK;
	const struct sched_class *prev_class;
	struct rq_flags rf;
	struct rq *rq;

	/* XXX used to be waiter->prio, not waiter->task->prio */
	prio = __rt_effective_prio(pi_task, p->normal_prio);

	/*
	 * If nothing changed; bail early.
	 */
	if (p->pi_top_task == pi_task && prio == p->prio && !dl_prio(prio))
		return;

	rq = __task_rq_lock(p, &rf);
	update_rq_clock(rq);
	/*
	 * Set under pi_lock && rq->lock, such that the value can be used under
	 * either lock.
	 *
	 * Note that there is loads of tricky to make this pointer cache work
	 * right. rt_mutex_slowunlock()+rt_mutex_postunlock() work together to
	 * ensure a task is de-boosted (pi_task is set to NULL) before the
	 * task is allowed to run again (and can exit). This ensures the pointer
	 * points to a blocked task -- which guarantees the task is present.
	 */
	p->pi_top_task = pi_task;

	/*
	 * For FIFO/RR we only need to set prio, if that matches we're done.
	 */
	if (prio == p->prio && !dl_prio(prio))
		goto out_unlock;

	/*
	 * Idle task boosting is a nono in general. There is one
	 * exception, when PREEMPT_RT and NOHZ is active:
	 *
	 * The idle task calls get_next_timer_interrupt() and holds
	 * the timer wheel base->lock on the CPU and another CPU wants
	 * to access the timer (probably to cancel it). We can safely
	 * ignore the boosting request, as the idle CPU runs this code
	 * with interrupts disabled and will complete the lock
	 * protected section without being interrupted. So there is no
	 * real need to boost.
	 */
	if (unlikely(p == rq->idle)) {
		WARN_ON(p != rq->curr);
		WARN_ON(p->pi_blocked_on);
		goto out_unlock;
	}

	trace_sched_pi_setprio(p, pi_task);
	oldprio = p->prio;

	if (oldprio == prio)
		queue_flag &= ~DEQUEUE_MOVE;

	prev_class = p->sched_class;
	queued = task_on_rq_queued(p);
	running = task_current(rq, p);
	if (queued)
		dequeue_task(rq, p, queue_flag);
	if (running)
		put_prev_task(rq, p);

	/*
	 * Boosting condition are:
	 * 1. -rt task is running and holds mutex A
	 *      --> -dl task blocks on mutex A
	 *
	 * 2. -dl task is running and holds mutex A
	 *      --> -dl task blocks on mutex A and could preempt the
	 *          running task
	 */
	if (dl_prio(prio)) {
		if (!dl_prio(p->normal_prio) ||
		    (pi_task && dl_prio(pi_task->prio) &&
		     dl_entity_preempt(&pi_task->dl, &p->dl))) {
			p->dl.pi_se = pi_task->dl.pi_se;
			queue_flag |= ENQUEUE_REPLENISH;
		} else {
			p->dl.pi_se = &p->dl;
		}
	} else if (rt_prio(prio)) /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#ifndef _CX23885_REG_H_
#define _CX23885_REG_H_

/*
Address Map
0x00000000 -> 0x00009000   TX SRAM  (Fifos)
0x00010000 -> 0x00013c00   RX SRAM  CMDS + CDT

EACH CMDS struct is 0x80 bytes long

DMAx_PTR1 = 0x03040 address of first cluster
DMAx_PTR2 = 0x10600 address of the CDT
DMAx_CNT1 = cluster size in (bytes >> 4) -1
DMAx_CNT2 = total cdt size for all entries >> 3

Cluster Descriptor entry = 4 DWORDS
 DWORD 0 -> ptr to cluster
 DWORD 1 Reserved
 DWORD 2 Reserved
 DWORD 3 Reserved

Channel manager Data Structure entry = 20 DWORD
  0  IntialProgramCounterLow
  1  IntialProgramCounterHigh
  2  ClusterDescriptorTableBase
  3  ClusterDescriptorTableSize
  4  InstructionQueueBase
  5  InstructionQueueSize
...  Reserved
 19  Reserved
*/

/* Risc Instructions */
#define RISC_CNT_INC		 0x00010000
#define RISC_CNT_RESET		 0x00030000
#define RISC_IRQ1		 0x01000000
#define RISC_IRQ2		 0x02000000
#define RISC_EOL		 0x04000000
#define RISC_SOL		 0x08000000
#define RISC_WRITE		 0x10000000
#define RISC_SKIP		 0x20000000
#define RISC_JUMP		 0x70000000
#define RISC_SYNC		 0x80000000
#define RISC_RESYNC		 0x80008000
#define RISC_READ		 0x90000000
#define RISC_WRITERM		 0xB0000000
#define RISC_WRITECM		 0xC0000000
#define RISC_WRITECR		 0xD0000000
#define RISC_WRITEC		 0x50000000
#define RISC_READC		 0xA0000000


/* Audio and Video Core */
#define HOST_REG1		0x00000000
#define HOST_REG2		0x00000001
#define HOST_REG3		0x00000002

/* Chip Configuration Registers */
#define CHIP_CTRL		0x00000100
#define AFE_CTRL		0x00000104
#define VID_PLL_INT_POST	0x00000108
#define VID_PLL_FRAC		0x0000010C
#define AUX_PLL_INT_POST	0x00000110
#define AUX_PLL_FRAC		0x00000114
#define SYS_PLL_INT_POST	0x00000118
#define SYS_PLL_FRAC		0x0000011C
#define PIN_CTRL		0x00000120
#define AUD_IO_CTRL		0x00000124
#define AUD_LOCK1		0x00000128
#define AUD_LOCK2		0x0000012C
#define POWER_CTRL		0x00000130
#define AFE_DIAG_CTRL1		0x00000134
#define AFE_DIAG_CTRL3		0x0000013C
#define PLL_DIAG_CTRL		0x00000140
#define AFE_CLK_OUT_CTRL	0x00000144
#define DLL1_DIAG_CTRL		0x0000015C

/* GPIO[23:19] Output Enable */
#define GPIO2_OUT_EN_REG	0x00000160
/* GPIO[23:19] Data Registers */
#define GPIO2			0x00000164

#define IFADC_CTRL		0x00000180

/* Infrared Remote Registers */
#define IR_CNTRL_REG	0x00000200
#define IR_TXCLK_REG	0x00000204
#define IR_RXCLK_REG	0x00000208
#define IR_CDUTY_REG	0x0000020C
#define IR_STAT_REG	0x00000210
#define IR_IRQEN_REG	0x00000214
#define IR_FILTR_REG	0x00000218
#define IR_FIFO_REG	0x0000023C

/* Video Decoder Registers */
#define MODE_CTRL		0x00000400
#define OUT_CTRL1		0x00000404
#define OUT_CTRL2		0x00000408
#define GEN_STAT		0x0000040C
#define INT_STAT_MASK		0x00000410
#define LUMA_CTRL		0x00000414
#define HSCALE_CTRL		0x00000418
#define VSCALE_CTRL		0x0000041C
#define CHROMA_CTRL		0x00000420
#define VBI_LINE_CTRL1		0x00000424
#define VBI_LINE_CTRL2		0x00000428
#define VBI_LINE_CTRL3		0x0000042C
#define VBI_LINE_CTRL4		0x00000430
#define VBI_LINE_CTRL5		0x00000434
#define VBI_FC_CFG		0x00000438
#define VBI_MISC_CFG1		0x0000043C
#define VBI_MISC_CFG2		0x00000440
#define VBI_PAY1		0x00000444
#define VBI_PAY2		0x00000448
#define VBI_CUST1_CFG1		0x0000044C
#define VBI_CUST1_CFG2		0x00000450
#define VBI_CUST1_CFG3		0x00000454
#define VBI_CUST2_CFG1		0x00000458
#define VBI_CUST2_CFG2		0x0000045C
#define VBI_CUST2_CFG3		0x00000460
#define VBI_CUST3_CFG1		0x00000464
#define VBI_CUST3_CFG2		0x00000468
#define VBI_CUST3_CFG3		0x0000046C
#define HORIZ_TIM_CTRL		0x00000470
#define VERT_TIM_CTRL		0x00000474
#define SRC_COMB_CFG		0x00000478
#define CHROMA_VBIOFF_CFG	0x0000047C
#define FIELD_COUNT		0x00000480
#define MISC_TIM_CTRL		0x00000484
#define DFE_CTRL1		0x00000488
#define DFE_CTRL2		0x0000048C
#define DFE_CTRL3		0x00000490
#define PLL_CTRL		0x00000494
#define HTL_CTRL		0x00000498
#define COMB_CTRL		0x0000049C
#define CRUSH_CTRL		0x000004A0
#define SOFT_RST_CTRL		0x000004A4
#define CX885_VERSION		0x000004B4
#define VBI_PASS_CTRL		0x000004BC

/* Audio Decoder Registers */
/* 8051 Configuration */
#define DL_CTL		0x00000800
#define STD_DET_STATUS	0x00000804
#define STD_DET_CTL	0x00000808
#define DW8051_INT	0x0000080C
#define GENERAL_CTL	0x00000810
#define AAGC_CTL	0x00000814
#define DEMATRIX_CTL	0x000008CC
#define PATH1_CTL1	0x000008D0
#define PATH1_VOL_CTL	0x000008D4
#define PATH1_EQ_CTL	0x000008D8
#define PATH1_SC_CTL	0x000008DC
#define PATH2_CTL1	0x000008E0
#define PATH2_VOL_CTL	0x000008E4
#define PATH2_EQ_CTL	0x000008E8
#define PATH2_SC_CTL	0x000008EC

/* Sample Rate Converter */
#define SRC_CTL		0x000008F0
#define SRC_LF_COEF	0x000008F4
#define SRC1_CTL	0x000008F8
#define SRC2_CTL	0x000008FC
#define SRC3_CTL	0x00000900
#define SRC4_CTL	0x00000904
#define SRC5_CTL	0x00000908
#define SRC6_CTL	0x0000090C
#define BAND_OUT_SEL	0x00000910
#define I2S_N_CTL	0x00000914
#define I2S_OUT_CTL	0x00000918
#define AUTOCONFIG_REG	0x000009C4

/* Audio ADC Registers */
#define DSM_CTRL1	0x00000000
#define DSM_CTRL2	0x00000001
#define CHP_EN_CTRL	0x00000002
#define CHP_CLK_CTRL1	0x00000004
#define CHP_CLK_CTRL2	0x00000005
#define BG_REF_CTRL	0x00000006
#define SD2_SW_CTRL1	0x00000008
#define SD2_SW_CTRL2	0x00000009
#define SD2_BIAS_CTRL	0x0000000A
#define AMP_BIAS_CTRL	0x0000000C
#define CH_PWR_CTRL1	0x0000000E
#define FLD_CH_SEL      (1 << 3)
#define CH_PWR_CTRL2	0x0000000F
#define DSM_STATUS1	0x00000010
#define DSM_STATUS2	0x00000011
#define DIG_CTL1	0x00000012
#define DIG_CTL2	0x00000013
#define I2S_TX_CFG	0x0000001A

#define DEV_CNTRL2	0x00040000

#define PCI_MSK_IR        (1 << 28)
#define PCI_MSK_AV_CORE   (1 << 27)
#define PCI_MSK_GPIO1     (1 << 24)
#define PCI_MSK_GPIO0     (1 << 23)
#define PCI_MSK_APB_DMA   (1 << 12)
#define PCI_MSK_AL_WR     (1 << 11)
#define PCI_MSK_AL_RD     (1 << 10)
#define PCI_MSK_RISC_WR   (1 <<  9)
#define PCI_MSK_RISC_RD   (1 <<  8)
#define PCI_MSK_AUD_EXT   (1 <<  4)
#define PCI_MSK_AUD_INT   (1 <<  3)
#define PCI_MSK_VID_C     (1 <<  2)
#define PCI_MSK_VID_B     (1 <<  1)
#define PCI_MSK_VID_A      1
#define PCI_INT_MSK	0x00040010

#define PCI_INT_STAT	0x00040014
#define PCI_INT_MSTAT	0x00040018

#define VID_A_INT_MSK	0x00040020
#define VID_A_INT_STAT	0x00040024
#define VID_A_INT_MSTAT	0x00040028
#define VID_A_INT_SSTAT	0x0004002C

#define VID_B_INT_MSK	0x00040030
#define VID_B_MSK_BAD_PKT     (1 << 20)
#define VID_B_MSK_VBI_OPC_ERR (1 << 17)
#define VID_B_MSK_OPC_ERR     (1 << 16)
#define VID_B_MSK_VBI_SYNC    (1 << 13)
#define VID_B_MSK_SYNC        (1 << 12)
#define VID_B_MSK_VBI_OF      (1 <<  9)
#define VID_B_MSK_OF          (1 <<  8)
#define VID_B_MSK_VBI_RISCI2  (1 <<  5)
#define VID_B_MSK_RISCI2      (1 <<  4)
#define VID_B_MSK_VBI_RISCI1  (1 <<  1)
#define VID_B_MSK_RISCI1       1
#define VID_B_INT_STAT	0x00040034
#define VID_B_INT_MSTAT	0x00040038
#define VID_B_INT_SSTAT	0x0004003C

#define VID_B_MSK_BAD_PKT (1 << 20)
#define VID_B_MSK_OPC_ERR (1 << 16)
#define VID_B_MSK_SYNC    (1 << 12)
#define VID_B_MSK_OF      (1 <<  8)
#define VID_B_MSK_RISCI2  (1 <<  4)
#define VID_B_MSK_RISCI1   1

#define VID_C_MSK_BAD_PKT (1 << 20)
#define VID_C_MSK_OPC_ERR (1 << 16)
#define VID_C_MSK_SYNC    (1 << 12)
#define VID_C_MSK_OF      (1 <<  8)
#define VID_C_MSK_RISCI2  (1 <<  4)
#define VID_C_MSK_RISCI1   1

/* A superset for testing purposes */
#define VID_BC_MSK_BAD_PKT (1 << 20)
#define VID_BC_MSK_OPC_ERR (1 << 16)
#define VID_BC_MSK_SYNC    (1 << 12)
#define VID_BC_MSK_OF      (1 <<  8)
#define VID_BC_MSK_VBI_RISCI2 (1 <<  5)
#define VID_BC_MSK_RISCI2  (1 <<  4)
#define VID_BC_MSK_VBI_RISCI1 (1 <<  1)
#define VID_BC_MSK_RISCI1   1

#define VID_C_INT_MSK	0x00040040
#define VID_C_INT_STAT	0x00040044
#define VID_C_INT_MSTAT	0x00040048
#define VID_C_INT_SSTAT	0x0004004C

#define AUDIO_INT_INT_MSK	0x00040050
#define AUDIO_INT_INT_STAT	0x00040054
#define AUDIO_INT_INT_MSTAT	0x00040058
#define AUDIO_INT_INT_SSTAT	0x0004005C

#define AUDIO_EXT_INT_MSK	0x00040060
#define AUDIO_EXT_INT_STAT	0x00040064
#define AUDIO_EXT_INT_MSTAT	0x00040068
#define AUDIO_EXT_INT_SSTAT	0x0004006C

/* Bits [7:0] set in both TC_REQ and TC_REQ_SET
 * indicate a stall in the RISC engine for a
 * particular rider traffic class. This causes
 * the 885 and 888 bridges (unknown about 887)
 * to become inoperable. Setting bits in
 * TC_REQ_SET resets the corresponding bits
 * in TC_REQ (and TC_REQ_SET) allowing
 * operation to continue.
 */
#define TC_REQ		0x00040090
#define TC_REQ_SET	0x00040094

#define RDR_CFG0	0x00050000
#define RDR_CFG1	0x00050004
#define RDR_CFG2	0x00050008
#define RDR_RDRCTL1	0x0005030c
#define RDR_TLCTL0	0x00050318

/* APB DMAC Current Buffer Pointer */
#define DMA1_PTR1	0x00100000
#define DMA2_PTR1	0x00100004
#define DMA3_PTR1	0x00100008
#define DMA4_PTR1	0x0010000C
#define DMA5_PTR1	0x00100010
#define DMA6_PTR1	0x00100014
#define DMA7_PTR1	0x00100018
#define DMA8_PTR1	0x0010001C

/* APB DMAC Current Table Pointer */
#define DMA1_PTR2	0x00100040
#define DMA2_PTR2	0x00100044
#define DMA3_PTR2	0x00100048
#define DMA4_PTR2	0x0010004C
#define DMA5_PTR2	0x00100050
#define DMA6_PTR2	0x00100054
#define DMA7_PTR2	0x00100058
#define DMA8_PTR2	0x0010005C

/* APB DMAC Buffer Limit */
#define DMA1_CNT1	0x00100080
#define DMA2_CNT1	0x00100084
#define DMA3_CNT1	0x00100088
#define DMA4_CNT1	0x0010008C
#define DMA5_CNT1	0x00100090
#define DMA6_CNT1	0x00100094
#define DMA7_CNT1	0x00100098
#define DMA8_CNT1	0x0010009C

/* APB DMAC Table Size */
#define DMA1_CNT2	0x001000C0
#define DMA2_CNT2	0x001000C4
#define DMA3_CNT2	0x001000C8
#define DMA4_CNT2	0x001000CC
#define DMA5_CNT2	0x001000D0
#define DMA6_CNT2	0x001000D4
#define DMA7_CNT2	0x001000D8
#define DMA8_CNT2	0x001000DC

/* Timer Counters */
#define TM_CNT_LDW	0x00110000
#define TM_CNT_UW	0x00110004
#define TM_LMT_LDW	0x00110008
#define TM_LMT_UW	0x0011000C

/* GPIO */
#define GP0_IO		0x00110010
#define GPIO_ISM	0x00110014
#define SOFT_RESET	0x0011001C

/* GPIO (417 Microsoftcontroller) RW Data */
#define MC417_RWD	0x00110020

/* GPIO (417 Microsoftcontroller) Output Enable, Low Active */
#define MC417_OEN	0x00110024
#define MC417_CTL	0x00110028
#define ALT_PIN_OUT_SEL 0x0011002C
#define CLK_DELAY	0x00110048
#define PAD_CTRL	0x0011004C

/* Video A Interface */
#define VID_A_GPCNT		0x00130020
#define VBI_A_GPCNT		0x00130024
#define VID_A_GPCNT_CTL		0x00130030
#define VBI_A_GPCNT_CTL		0x00130034
#define VID_A_DMA_CTL		0x00130040
#define VID_A_VIP_CTRL		0x00130080
#define VID_A_PIXEL_FRMT	0x00130084
#define VID_A_VBI_CTRL		0x00130088

/* Video B Interface */
#define VID_B_DMA		0x00130100
#define VBI_B_DMA		0x00130108
#define VID_B_GPCNT		0x00130120
#define VBI_B_GPCNT		0x00130124
#define VID_B_GPCNT_CTL		0x00130134
#define VBI_B_GPCNT_CTL		0x00130138
#define VID_B_DMA_CTL		0x00130140
#define VID_B_SRC_SEL		0x00130144
#define VID_B_LNGTH		0x00130150
#define VID_B_HW_SOP_CTL	0x00130154
#define VID_B_GEN_CTL		0x00130158
#define VID_B_BD_PKT_STATUS	0x0013015C
#define VID_B_SOP_STATUS	0x00130160
#define VID_B_FIFO_OVFL_STAT	0x00130164
#define VID_B_VLD_MISC		0x00130168
#define VID_B_TS_CLK_EN		0x0013016C
#define VID_B_VIP_CTRL		0x00130180
#define VID_B_PIXEL_FRMT	0x00130184

/* Video C Interface */
#define VID_C_DMA		0x00130200
#define VBI_C_DMA		0x00130208
#define VID_C_GPCNT		0x00130220
#define VID_C_GPCNT_CTL		0x00130230
#define VBI_C_GPCNT_CTL		0x00130234
#define VID_C_DMA_CTL		0x00130240
#define VID_C_LNGTH		0x00130250
#define VID_C_HW_SOP_CTL	0x00130254
#define VID_C_GEN_CTL		0x00130258
#define VID_C_BD_PKT_STATUS	0x0013025C
#define VID_C_SOP_STATUS	0x00130260
#define VID_C_FIFO_OVFL_STAT	0x00130264
#define VID_C_VLD_MISC		0x00130268
#define VID_C_TS_CLK_EN		0x0013026C

/* Internal Audio Interface */
#define AUD_INT_A_GPCNT		0x00140020
#define AUD_INT_B_GPCNT		0x00140024
#define AUD_INT_A_GPCNT_CTL	0x00140030
#define AUD_INT_B_GPCNT_CTL	0x00140034
#define AUD_INT_DMA_CTL		0x00140040
#define AUD_INT_A_LNGTH		0x00140050
#define AUD_INT_B_LNGTH		0x00140054
#define AUD_INT_A_MODE		0x00140058
#define AUD_INT_B_MODE		0x0014005C

/* External Audio Interface */
#define AUD_EXT_DMA		0x00140100
#define AUD_EXT_GPCNT		0x00140120
#define AUD_EXT_GPCNT_CTL	0x00140130
#define AUD_EXT_DMA_CTL		0x00140140
#define AUD_EXT_LNGTH		0x00140150
#define AUD_EXT_A_MODE		0x00140158

/* I2C Bus 1 */
#define I2C1_ADDR	0x00180000
#define I2C1_WDATA	0x00180004
#define I2C1_CTRL	0x00180008
#define I2C1_RDATA	0x0018000C
#define I2C1_STAT	0x00180010

/* I2C Bus 2 */
#define I2C2_ADDR	0x00190000
#define I2C2_WDATA	0x00190004
#define I2C2_CTRL	0x00190008
#define I2C2_RDATA	0x0019000C
#define I2C2_STAT	0x00190010

/* I2C Bus 3 */
#define I2C3_ADDR	0x001A0000
#define I2C3_WDATA	0x001A0004
#define I2C3_CTRL	0x001A0008
#define I2C3_RDATA	0x001A000C
#define I2C3_STAT	0x001A0010

/* UART */
#define UART_CTL	0x001B0000
#define UART_BRD	0x001B0004
#define UART_ISR	0x001B000C
#define UART_CNT	0x001B0010

#endif /* _CX23885_REG_H_ */
                                                                                                                                                