FIG_RCU_EQS_DEBUG=y.
 */
noinstr void rcu_nmi_enter(void)
{
	long incby = 2;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);

	/* Complain about underflow. */
	WARN_ON_ONCE(rdp->dynticks_nmi_nesting < 0);

	/*
	 * If idle from RCU viewpoint, atomically increment ->dynticks
	 * to mark non-idle and increment ->dynticks_nmi_nesting by one.
	 * Otherwise, increment ->dynticks_nmi_nesting by two.  This means
	 * if ->dynticks_nmi_nesting is equal to one, we are guaranteed
	 * to be in the outermost NMI handler that interrupted an RCU-idle
	 * period (observation due to Andy Lutomirski).
	 */
	if (rcu_dynticks_curr_cpu_in_eqs()) {

		if (!in_nmi())
			rcu_dynticks_task_exit();

		// RCU is not watching here ...
		rcu_dynticks_eqs_exit();
		// ... but is watching here.

		instrumentation_begin();
		// instrumentation for the noinstr rcu_dynticks_curr_cpu_in_eqs()
		instrument_atomic_read(&rdp->dynticks, sizeof(rdp->dynticks));
		// instrumentation for the noinstr rcu_dynticks_eqs_exit()
		instrument_atomic_write(&rdp->dynticks, sizeof(rdp->dynticks));

		incby = 1;
	} else if (!in_nmi()) {
		instrumentation_begin();
		rcu_irq_enter_check_tick();
	} else  {
		instrumentation_begin();
	}

	trace_rcu_dyntick(incby == 1 ? TPS("Endirq") : TPS("++="),
			  rdp->dynticks_nmi_nesting,
			  rdp->dynticks_nmi_nesting + incby, atomic_read(&rdp->dynticks));
	instrumentation_end();
	WRITE_ONCE(rdp->dynticks_nmi_nesting, /* Prevent store tearing. */
		   rdp->dynticks_nmi_nesting + incby);
	barrier();
}

/**
 * rcu_irq_enter - inform RCU that current CPU is entering irq away from idle
 *
 * Enter an interrupt handler, which might possibly result in exiting
 * idle mode, in other words, entering the mode in which read-side critical
 * sections can occur.  The caller must have disabled interrupts.
 *
 * Note that the Linux kernel is fully capable of entering an interrupt
 * handler that it never exits, for example when doing upcalls to user mode!
 * This code assumes that the idle loop never does upcalls to user mode.
 * If your architecture's idle loop does do upcalls to user mode (or does
 * anything else that results in unbalanced calls to the irq_enter() and
 * irq_exit() functions), RCU will give you what you deserve, good and hard.
 * But very infrequently and irreproducibly.
 *
 * Use things like work queues to work around this limitation.
 *
 * You have been warned.
 *
 * If you add or remove a call to rcu_irq_enter(), be sure to test with
 * CONFIG_RCU_EQS_DEBUG=y.
 */
noinstr void rcu_irq_enter(void)
{
	lockdep_assert_irqs_disabled();
	rcu_nmi_enter();
}

/*
 * Wrapper for rcu_irq_enter() where interrupts are enabled.
 *
 * If you add or remove a call to rcu_irq_enter_irqson(), be sure to test
 * with CONFIG_RCU_EQS_DEBUG=y.
 */
void rcu_irq_enter_irqson(void)
{
	unsigned long flags;

	local_irq_save(flags);
	rcu_irq_enter();
	local_irq_restore(flags);
}

/*
 * Check to see if any future non-offloaded RCU-related work will need
 * to be done by the current CPU, even if none need be done immediately,
 * returning 1 if so.  This function is part of the RCU implementation;
 * it is -not- an exported member of the RCU API.  This is used by
 * the idle-entry code to figure out whether it is safe to disable the
 * scheduler-clock interrupt.
 *
 * Just check whether or not this CPU has non-offloaded RCU callbacks
 * queued.
 */
int rcu_needs_cpu(void)
{
	return !rcu_segcblist_empty(&this_cpu_ptr(&rcu_data)->cblist) &&
		!rcu_rdp_is_offloaded(this_cpu_ptr(&rcu_data));
}

/*
 * If any sort of urgency was applied to the current CPU (for example,
 * the scheduler-clock interrupt was enabled on a nohz_full CPU) in order
 * to get to a quiescent state, disable it.
 */
static void rcu_disable_urgency_upon_qs(struct rcu_data *rdp)
{
	raw_lockdep_assert_held_rcu_node(rdp->mynode);
	WRITE_ONCE(rdp->rcu_urgent_qs, false);
	WRITE_ONCE(rdp->rcu_need_heavy_qs, false);
	if (tick_nohz_full_cpu(rdp->cpu) && rdp->rcu_forced_tick) {
		tick_dep_clear_cpu(rdp->cpu, TICK_DEP_BIT_RCU);
		WRITE_ONCE(rdp->rcu_forced_tick, false);
	}
}

/**
 * rcu_is_watching - see if RCU thinks that the current CPU is not idle
 *
 * Return true if RCU is watching the running CPU, which means that this
 * CPU can safely enter RCU read-side critical sections.  In other words,
 * if the current CPU is not in its idle loop or is in an interrupt or
 * NMI handler, return true.
 *
 * Make notrace because it can be called by the internal functions of
 * ftrace, and making this notrace removes unnecessary recursion calls.
 */
notrace bool rcu_is_watching(void)
{
	bool ret;

	preempt_disable_notrace();
	ret = !rcu_dynticks_curr_cpu_in_eqs();
	preempt_enable_notrace();
	return ret;
}
EXPORT_SYMBOL_GPL(rcu_is_watching);

/*
 * If a holdout task is actually running, request an urgent quiescent
 * state from its CPU.  This is unsynchronized, so migrations can cause
 * the request to go to the wrong CPU.  Which is OK, all that will happen
 * is that the CPU's next context switch will be a bit slower and next
 * time around this task will generate another request.
 */
void rcu_request_urgent_qs_task(struct task_struct *t)
{
	int cpu;

	barrier();
	cpu = task_cpu(t);
	if (!task_curr(t))
		return; /* This task is not running on that CPU. */
	smp_store_release(per_cpu_ptr(&rcu_data.rcu_urgent_qs, cpu), true);
}

#if defined(CONFIG_PROVE_RCU) && defined(CONFIG_HOTPLUG_CPU)

/*
 * Is the current CPU online as far as RCU is concerned?
 *
 * Disable preemption to avoid false positives that could otherwise
 * happen due to the current CPU number being sampled, this task being
 * preempted, its old CPU being taken offline, resuming on some other CPU,
 * then determining that its old CPU is now offline.
 *
 * Disable checking if in an NMI handler because we cannot safely
 * report errors from NMI handlers anyway.  In addition, it is OK to use
 * RCU on an offline processor during initial boot, hence the check for
 * rcu_scheduler_fully_active.
 */
bool rcu_lockdep_current_cpu_online(void)
{
	struct rcu_data *rdp;
	bool ret = false;

	if (in_nmi() || !rcu_scheduler_fully_active)
		return true;
	preempt_disable_notrace();
	rdp = this_cpu_ptr(&rcu_data);
	/*
	 * Strictly, we care here about the case where the current CPU is
	 * in rcu_cpu_starting() and thus has an excuse for rdp->grpmask
	 * not being up to date. So arch_spin_is_locked() might have a
	 * false positive if it's held by some *other* CPU, but that's
	 * OK because that just means a false *negative* on the warning.
	 */
	if (rcu_rdp_cpu_online(rdp) || arch_spin_is_locked(&rcu_state.ofl_lock))
		ret = true;
	preempt_enable_notrace();
	return ret;
}
EXPORT_SYMBOL_GPL(rcu_lockdep_current_cpu_online);

#endif /* #if defined(CONFIG_PROVE_RCU) && defined(CONFIG_HOTPLUG_CPU) */

/*
 * When trying to report a quiescent state on behalf of some other CPU,
 * it is our responsibility to check for and handle potential overflow
 * of the rcu_node ->gp_seq counter with respect to the rcu_data counters.
 * After all, the CPU might be in deep idle state, and thus executing no
 * code whatsoever.
 */
static void rcu_gpnum_ovf(struct rcu_node *rnp, struct rcu_data *rdp)
{
	raw_lockdep_assert_held_rcu_node(rnp);
	if (ULONG_CMP_LT(rcu_seq_current(&rdp->gp_seq) + ULONG_MAX / 4,
			 rnp->gp_seq))
		WRITE_ONCE(rdp->gpwrap, true);
	if (ULONG_CMP_LT(rdp->rcu_iw_gp_seq + ULONG_MAX / 4, rnp->gp_seq))
		rdp->rcu_iw_gp_seq = rnp->gp_seq + ULONG_MAX / 4;
}

/*
 * Snapshot the specified CPU's dynticks counter so that we can later
 * credit them with an implicit quiescent state.  Return 1 if this CPU
 * is in dynticks idle mode, which is an extended quiescent state.
 */
static int dyntick_save_progress_counter(struct rcu_data *rdp)
{
	rdp->dynticks_snap = rcu_dynticks_snap(rdp);
	if (rcu_dynticks_in_eqs(rdp->dynticks_snap)) {
		trace_rcu_fqs(rcu_state.name, rdp->gp_seq, rdp->cpu, TPS("dti"));
		rcu_gpnum_ovf(rdp->mynode, rdp);
		return 1;
	}
	return 0;
}

/*
 * Return true if the specified CPU has passed through a quiescent
 * state by virtue of being in or having passed through an dynticks
 * idle state since the last call to dyntick_save_progress_counter()
 * for this same CPU, or by virtue of having been offline.
 */
static int rcu_implicit_dynticks_qs(struct rcu_data *rdp)
{
	unsigned long jtsq;
	struct rcu_node *rnp = rdp->mynode;

	/*
	 * If the CPU passed through or entered a dynticks idle phase with
	 * no active irq/NMI handlers, then we can safely pretend that the CPU
	 * already acknowledged the request to pass through a quiescent
	 * state.  Either way, that CPU cannot possibly be in an RCU
	 * read-side critical section that started before the beginning
	 * of the current RCU grace period.
	 */
	if (rcu_dynticks_in_eqs_since(rdp, rdp->dynticks_snap)) {
		trace_rcu_fqs(rcu_state.name, rdp->gp_seq, rdp->cpu, TPS("dti"));
		rcu_gpnum_ovf(rnp, rdp);
		return 1;
	}

	/*
	 * Complain if a CPU that is considered to be offline from RCU's
	 * perspective has not yet reported a quiescent state.  After all,
	 * the offline CPU should have reported a quiescent state during
	 * the CPU-offline process, or, failing that, by rcu_gp_init()
	 * if it ran concurrently with either the CPU going offline or the
	 * last task on a leaf rcu_node structure exiting its RCU read-side
	 * critical section while all CPUs corresponding to that structure
	 * are offline.  This added warning detects bugs in any of these
	 * code paths.
	 *
	 * The rcu_node structure's ->lock is held here, which excludes
	 * the relevant portions the CPU-hotplug code, the grace-period
	 * initialization code, and the rcu_read_unlock() code paths.
	 *
	 * For more detail, please refer to the "Hotplug CPU" section
	 * of RCU's Requirements documentation.
	 */
	if (WARN_ON_ONCE(!rcu_rdp_cpu_online(rdp))) {
		struct rcu_node *rnp1;

		pr_info("%s: grp: %d-%d level: %d ->gp_seq %ld ->completedqs %ld\n",
			__func__, rnp->grplo, rnp->grphi, rnp->level,
			(long)rnp->gp_seq, (long)rnp->completedqs);
		for (rnp1 = rnp; rnp1; rnp1 = rnp1->parent)
			pr_info("%s: %d:%d ->qsmask %#lx ->qsmaskinit %#lx ->qsmaskinitnext %#lx ->rcu_gp_init_mask %#lx\n",
				__func__, rnp1->grplo, rnp1->grphi, rnp1->qsmask, rnp1->qsmaskinit, rnp1->qsmaskinitnext, rnp1->rcu_gp_init_mask);
		pr_info("%s %d: %c online: %ld(%d) offline: %ld(%d)\n",
			__func__, rdp->cpu, ".o"[rcu_rdp_cpu_online(rdp)],
			(long)rdp->rcu_onl_gp_seq, rdp->rcu_onl_gp_flags,
			(long)rdp->rcu_ofl_gp_seq, rdp->rcu_ofl_gp_flags);
		return 1; /* Break things loose after complaining. */
	}

	/*
	 * A CPU running for an extended time within the kernel can
	 * delay RCU grace periods: (1) At age jiffies_to_sched_qs,
	 * set .rcu_urgent_qs, (2) At age 2*jiffies_to_sched_qs, set
	 * both .rcu_need_heavy_qs and .rcu_urgent_qs.  Note that the
	 * unsynchronized assignments to the per-CPU rcu_need_heavy_qs
	 * variable are safe because the assignments are repeated if this
	 * CPU failed to pass through a quiescent state.  This code
	 * also checks .jiffies_resched in case jiffies_to_sched_qs
	 * is set way high.
	 */
	jtsq = READ_ONCE(jiffies_to_sched_qs);
	if (!READ_ONCE(rdp->rcu_need_heavy_qs) &&
	    (time_after(jiffies, rcu_state.gp_start + jtsq * 2) ||
	     time_after(jiffies, rcu_state.jiffies_resched) ||
	     rcu_state.cbovld)) {
		WRITE_ONCE(rdp->rcu_need_heavy_qs, true);
		/* Store rcu_need_heavy_qs before rcu_urgent_qs. */
		smp_store_release(&rdp->rcu_urgent_qs, true);
	} else if (time_after(jiffies, rcu_state.gp_start + jtsq)) {
		WRITE_ONCE(rdp->rcu_urgent_qs, true);
	}

	/*
	 * NO_HZ_FULL CPUs can run in-kernel without rcu_sched_clock_irq!
	 * The above code handles this, but only for straight cond_resched().
	 * And some in-kernel loops check need_resched() before calling
	 * cond_resched(), which defeats the above code for CPUs that are
	 * running in-kernel with scheduling-clock interrupts disabled.
	 * So hit them over the head with the resched_cpu() hammer!
	 */
	if (tick_nohz_full_cpu(rdp->cpu) &&
	    (time_after(jiffies, READ_ONCE(rdp->last_fqs_resched) + jtsq * 3) ||
	     rcu_state.cbovld)) {
		WRITE_ONCE(rdp->rcu_urgent_qs, true);
		resched_cpu(rdp->cpu);
		WRITE_ONCE(rdp->last_fqs_resched, jiffies);
	}

	/*
	 * If more than halfway to RCU CPU stall-warning time, invoke
	 * resched_e = "stat",
		.seq_show = cpu_cfs_stat_show,
	},
#endif
#ifdef CONFIG_RT_GROUP_SCHED
	{
		.name = "rt_runtime_us",
		.read_s64 = cpu_rt_runtime_read,
		.write_s64 = cpu_rt_runtime_write,
	},
	{
		.name = "rt_period_us",
		.read_u64 = cpu_rt_period_read_uint,
		.write_u64 = cpu_rt_period_write_uint,
	},
#endif
#ifdef CONFIG_UCLAMP_TASK_GROUP
	{
		.name = "uclamp.min",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_uclamp_min_show,
		.write = cpu_uclamp_min_write,
	},
	{
		.name = "uclamp.max",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_uclamp_max_show,
		.write = cpu_uclamp_max_write,
	},
#endif
	{ }	/* Terminate */
};

static int cpu_extra_stat_show(struct seq_file *sf,
			       struct cgroup_subsys_state *css)
{
#ifdef CONFIG_CFS_BANDWIDTH
	{
		struct task_group *tg = css_tg(css);
		struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;
		u64 throttled_usec, burst_usec;

		throttled_usec = cfs_b->throttled_time;
		do_div(throttled_usec, NSEC_PER_USEC);
		burst_usec = cfs_b->burst_time;
		do_div(burst_usec, NSEC_PER_USEC);

		seq_printf(sf, "nr_periods %d\n"
			   "nr_throttled %d\n"
			   "throttled_usec %llu\n"
			   "nr_bursts %d\n"
			   "burst_usec %llu\n",
			   cfs_b->nr_periods, cfs_b->nr_throttled,
			   throttled_usec, cfs_b->nr_burst, burst_usec);
	}
#endif
	return 0;
}

#ifdef CONFIG_FAIR_GROUP_SCHED
static u64 cpu_weight_read_u64(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	struct task_group *tg = css_tg(css);
	u64 weight = scale_load_down(tg->shares);

	return DIV_ROUND_CLOSEST_ULL(weight * CGROUP_WEIGHT_DFL, 1024);
}

static int cpu_weight_write_u64(struct cgroup_subsys_state *css,
				struct cftype *cft, u64 weight)
{
	/*
	 * cgroup weight knobs should use the common MIN, DFL and MAX
	 * values which are 1, 100 and 10000 respectively.  While it loses
	 * a bit of range on both ends, it maps pretty well onto the shares
	 * value used by scheduler and the round-trip conversions preserve
	 * the original value over the entire range.
	 */
	if (weight < CGROUP_WEIGHT_MIN || weight > CGROUP_WEIGHT_MAX)
		return -ERANGE;

	weight = DIV_ROUND_CLOSEST_ULL(weight * 1024, CGROUP_WEIGHT_DFL);

	return sched_group_set_shares(css_tg(css), scale_load(weight));
}

static s64 cpu_weight_nice_read_s64(struct cgroup_subsys_state *css,
				    struct cftype *cft)
{
	unsigned long weight = scale_load_down(css_tg(css)->shares);
	int last_delta = INT_MAX;
	int prio, delta;

	/* find the closest nice value to the current weight */
	for (prio = 0; prio < ARRAY_SIZE(sched_prio_to_weight); prio++) {
		delta = abs(sched_prio_to_weight[prio] - weight);
		if (delta >= last_delta)
			break;
		last_delta = delta;
	}

	return PRIO_TO_NICE(prio - 1 + MAX_RT_PRIO);
}

static int cpu_weight_nice_write_s64(struct cgroup_subsys_state *css,
				     struct cftype *cft, s64 nice)
{
	unsigned long weight;
	int idx;

	if (nice < MIN_NICE || nice > MAX_NICE)
		return -ERANGE;

	idx = NICE_TO_PRIO(nice) - MAX_RT_PRIO;
	idx = array_index_nospec(idx, 40);
	weight = sched_prio_to_weight[idx];

	return sched_group_set_shares(css_tg(css), scale_load(weight));
}
#endif

static void __maybe_unused cpu_period_quota_print(struct seq_file *sf,
						  long period, long quota)
{
	if (quota < 0)
		seq_puts(sf, "max");
	else
		seq_printf(sf, "%ld", quota);

	seq_printf(sf, " %ld\n", period);
}

/* caller should put the current value in *@periodp before calling */
static int __maybe_unused cpu_period_quota_parse(char *buf,
						 u64 *periodp, u64 *quotap)
{
	char tok[21];	/* U64_MAX */

	if (sscanf(buf, "%20s %llu", tok, periodp) < 1)
		return -EINVAL;

	*periodp *= NSEC_PER_USEC;

	if (sscanf(tok, "%llu", quotap))
		*quotap *= NSEC_PER_USEC;
	else if (!strcmp(tok, "max"))
		*quotap = RUNTIME_INF;
	else
		return -EINVAL;

	return 0;
}

#ifdef CONFIG_CFS_BANDWIDTH
static int cpu_max_show(struct seq_file *sf, void *v)
{
	struct task_group *tg = css_tg(seq_css(sf));

	cpu_period_quota_print(sf, tg_get_cfs_period(tg), tg_get_cfs_quota(tg));
	return 0;
}

static ssize_t cpu_max_write(struct kernfs_open_file *of,
			     char *buf, size_t nbytes, loff_t off)
{
	struct task_group *tg = css_tg(of_css(of));
	u64 period = tg_get_cfs_period(tg);
	u64 burst = tg_get_cfs_burst(tg);
	u64 quota;
	int ret;

	ret = cpu_period_quota_parse(buf, &period, &quota);
	if (!ret)
		ret = tg_set_cfs_bandwidth(tg, period, quota, burst);
	return ret ?: nbytes;
}
#endif

static struct cftype cpu_files[] = {
#ifdef CONFIG_FAIR_GROUP_SCHED
	{
		.name = "weight",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_u64 = cpu_weight_read_u64,
		.write_u64 = cpu_weight_write_u64,
	},
	{
		.name = "weight.nice",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_s64 = cpu_weight_nice_read_s64,
		.write_s64 = cpu_weight_nice_write_s64,
	},
	{
		.name = "idle",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_s64 = cpu_idle_read_s64,
		.write_s64 = cpu_idle_write_s64,
	},
#endif
#ifdef CONFIG_CFS_BANDWIDTH
	{
		.name = "max",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_max_show,
		.write = cpu_max_write,
	},
	{
		.name = "max.burst",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_u64 = cpu_cfs_burst_read_u64,
		.write_u64 = cpu_cfs_burst_write_u64,
	},
#endif
#ifdef CONFIG_UCLAMP_TASK_GROUP
	{
		.name = "uclamp.min",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_uclamp_min_show,
		.write = cpu_uclamp_min_write,
	},
	{
		.name = "uclamp.max",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = cpu_uclamp_max_show,
		.write = cpu_uclamp_max_write,
	},
#endif
	{ }	/* terminate */
};

struct cgroup_subsys cpu_cgrp_subsys = {
	.css_alloc	= cpu_cgroup_css_alloc,
	.css_online	= cpu_cgroup_css_online,
	.css_released	= cpu_cgroup_css_released,
	.css_free	= cpu_cgroup_css_free,
	.css_extra_stat_show = cpu_extra_stat_show,
	.fork		= cpu_cgroup_fork,
	.can_attach	= cpu_cgroup_can_attach,
	.attach		= cpu_cgroup_attach,
	.legacy_cftypes	= cpu_legacy_files,
	.dfl_cftypes	= cpu_files,
	.early_init	= true,
	.threaded	= true,
};

#endif	/* CONFIG_CGROUP_SCHED */

void dump_cpu_task(int cpu)
{
	pr_info("Task dump for CPU %d:\n", cpu);
	sched_show_task(cpu_curr(cpu));
}

/*
 * Nice levels are multiplicative, with a gentle 10% change for every
 * nice level changed. I.e. when a CPU-bound task goes from nice 0 to
 * nice 1, it will get ~10% less CPU time than another CPU-bound task
 * that remained on nice 0.
 *
 * The "10% effect" is relative and cumulative: from _any_ nice level,
 * if you go up 1 level, it's -10% CPU usage, if you go down 1 level
 * it's +10% CPU usage. (to achieve that we use a multiplier of 1.25.
 * If a task goes up by ~10% and another task goes down by ~10% then
 * the relative distance between them is ~25%.)
 */
const int sched_prio_to_weight[40] = {
 /* -20 */     88761,     71755,     56483,     46273,     36291,
 /* -15 */     29154,     23254,     18705,     14949,     11916,
 /* -10 */      9548,      7620,      6100,      4904,      3906,
 /*  -5 */      3121,      2501,      1991,      1586,      1277,
 /*   0 */      1024,       820,       655,       526,       423,
 /*   5 */       335,       272,       215,       172,       137,
 /*  10 */       110,        87,        70,        56,        45,
 /*  15 */        36,        29,        23,        18,        15,
};

/*
 * Inverse (2^32/x) values of the sched_prio_to_weight[] array, precalculated.
 *
 * In cases where the weight does not change often, we can use the
 * precalculated inverse to speed up arithmetics by turning divisions
 * into multiplications:
 */
const u32 sched_prio_to_wmult[40] = {
 /* -20 */     48388,     59856,     76040,     92818,    118348,
 /* -15 */    147320,    184698,    229616,    287308,    360437,
 /* -10 */    449829,    563644,    704093,    875809,   1099582,
 /*  -5 */   1376151,   1717300,   2157191,   2708050,   3363326,
 /*   0 */   4194304,   5237765,   6557202,   8165337,  10153587,
 /*   5 */  12820798,  15790321,  19976592,  24970740,  31350126,
 /*  10 */  39045157,  49367440,  61356676,  76695844,  95443717,
 /*  15 */ 119304647, 148102320, 186737708, 238609294, 286331153,
};

void call_trace_sched_update_nr_running(struct rq *rq, int count)
{
        trace_sched_update_nr_running_tp(rq, count);
}
                               // SPDX-License-Identifier: GPL-2.0-only
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
	θόÿÿÿΓ΄&    v θόÿÿÿS‰Σθόÿÿÿ‰ΨΊ    [ιόÿÿÿfffθόÿÿÿ1ΐΉÿ  ;
‰ΑJMΒΓ΄&    fθόÿÿÿV‰ΘΎÿ  S‹	‹XƒΑƒΣ 1Ò9Ξ‰‰Φ‰Xή[^LΒΓt& θόÿÿÿΓ΄&    v θόÿÿÿSƒμ‹ϋÿ  …ΫtI‰Ϊθόÿÿÿ1ΐƒΔ[Γv λ   t‰ΪθόÿÿÿƒΔ1ΐ[ΓfΊKΧ ‰$θόÿÿÿ‹$‰Ϊθόÿÿÿλή΄&    Ί7Χ ‰$θόÿÿÿ‹$λ¥fffffffθόÿÿÿ‰Π‹    S‰ΣΑϋ;Ί    XLΒ[Γ΄&    ΄&    θόÿÿÿW‰ΘVS‹	‹XƒΑ‰‹    ƒΣ ‰X‰ΧΑÿ9ΚΊ    ί[^LΒ_Γ΄&    t& θόÿÿÿΓfffffθόÿÿÿU1ΙΊ
   WVS‰Γƒμd΅    ‰D$1ΐ‰ΰθόÿÿÿd΅    θόÿÿÿ‹-    ‹pθόÿÿÿ‹|$‹L$ƒξ‹$VUP‰ψΑο%ÿ  kΐdΑθP‰ΘΑι%ÿ  WkΐdΑθP‰ΠΑκ%ÿ  QkΐdΑθPRh$u SθόÿÿÿƒΔ,‹D$d+    u
ƒΔ1ΐ[^_]ΓθόÿÿÿfffffffθόÿÿÿΓ΄&    v θόÿÿÿUWVS‰Γƒμhd΅    ‰D$d1ΐD$$θόÿÿÿD$$θόÿÿÿθόÿÿÿ‹5L   1Ò‰D$΅   …φHς+t$@…ΐHΒ)ΖΈ    Hπ‹    1Ι…ÒHΡ‰Tƒΐƒψuηθόÿÿÿ1Ò‰Η΅   …ΐIΠ΅   ‰Υ1Ò…ΐIΠΑζ‰T$1Òj‹D$8RΊwΧ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$HRΊΧ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ½    1Ò1ÿjRΊΧ P‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Òj‹D$DRΊ―Χ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨθόÿÿÿΊΐΧ ‰ΨjWVθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅   1Òj…ΐHΒ1ÒRΊΡΧ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ‹t$‹D$1Òj‰D$πΑΰ‰t$RΊβΧ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ‹|$‹t$$1Òj7RΊσΧ ΑζΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$ RΊΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ½    1Ò1ÿjRΊΨ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$RΊ&Ψ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨθόÿÿÿΊ7Ψ ‰ΨjWVθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿj‹D$$Αΰ1ÒRΊHΨ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅   1Òj…ΐHΒ1ÒRΊYΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$lRΊjΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Òj‹D$XRΊ{Ψ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$D+D$`RΊΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$T+D$pRΊΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Òj‹D$HRΊ®Ψ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$XRΊΏΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅P   1Òj…ΐHΒ1ÒRΊΠΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅T   1Òj…ΐHΒ1ÒRΊαΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅D   1Òj…ΐHΒ1ÒRΊςΨ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅H   1Òj…ΐHΒ1ÒRΊΩ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Òj‹D$@RΊΩ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅   1Òj…ΐHΒ1ÒRΊ%Ω θΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹|$(RΊ6Ω /ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ1Ò­    jRΊGΩ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Ò½    jRΊXΩ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅”   1Ò…ΐHΒPhiΩ Sθόÿÿÿ΅   1ÒƒΔ$…ΐjHΒ1ÒRΊ‚Ω ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨθόÿÿÿΊ“Ω ‰Ψjj j θόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅    1Òj…ΐHΒ1ÒRΊ¤Ω ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅X   1Òj…ΐHΒ1ÒRΊµΩ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿθόÿÿÿ1ÒΑΰjRΊΖΩ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ1Òj‹D$$RΊΧΩ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅    -  ¶ %  ΰÿ+    -  ΐ Αθ
PhθΩ Sθόÿÿÿθόÿÿÿ1ÒΑΰjRΊΪ P‰ΨθόÿÿÿΉ   ΊΧ ‰ΨθόÿÿÿΊΪ ‰Ψjj j θόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿθόÿÿÿ1ÒΑΰjRΊ#Ϊ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅p   1Òj…ΐHΒ1ÒRΊ4Ϊ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅`   1Òj…ΐHΒ1ÒRΊEΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅d   1Òj…ΐHΒ1ÒRΊVΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅h   1Òj…ΐHΒ1ÒRΊgΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅l   1Òj…ΐHΒ1ÒRΊxΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰ΨƒΔ$θόÿÿÿ΅    1ÒjΑΰRΊ‰Ϊ P‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ΅(   1Òj…ΐHΒ1ÒRΊΪ ΑΰP‰ΨθόÿÿÿΉ   ΊΧ ‰Ψθόÿÿÿ‰Ψθόÿÿÿ‰ΨθόÿÿÿƒΔ‹D$dd+    u
ƒΔh1ΐ[^_]Γθόÿÿÿfffffθόÿÿÿ΅    S‰Σ1Ι‹    ƒΐΑΰPΊ`3 P‰ΨθόÿÿÿZ[ΓfθόÿÿÿU‰εS‰Γ£    r‹C0‹S4‹]όΙΓ‰Π1Òθόÿÿÿ‰Α!ΡƒωÿtβiΚθ  Ίθ  ‹]όΙχβΚΓ΄&    v θόÿÿÿU‰εS‰Γ£    r‹C(‹S,‹]όΙΓ‰Π1Òθόÿÿÿ‰Α!ΡƒωÿtβiΚθ  Ίθ  ‹]όΙχβΚΓ΄&    v θόÿÿÿU‰ΕΉ
   WVSμ  d΅    ‰„$  1ΐ$   ‰ίσ«„$„   θόÿÿÿ‹΄$„   ‹Ό$   d΅    ‹€¤  ‹P‹„$   +r$z(+B,‰ω1ÿ™R‰ςP„$   θόÿÿÿ‹„$   ‹”$   ‰„$   ‹„$    ‰”$   ‰„$”   XZ„$Δ   ΗD$    ΗD$     ΗD$t    ΗD$x    ΗD$l    ΗD$p    ΗD$d    ΗD$h    ΗD$\    ΗD$`    ΗD$T    ΗD$X    ΗD$L    ΗD$P    ΗD$D    ΗD$H    ΗD$<    ΗD$@    ΗD$4    ΗD$8    ΗD$,    ΗD$0    ΗD$$    ΗD$(    ΗD$ÿÿÿÿ‰D$Έ    ‰D$|‰¬$€   ‰ύ‹D$Ί    θόÿÿÿ‰D$;    ƒ’  ‹T$‹|$Ή   „$Δ   ‹t$|‰D$4•    σ¥‹΄$Δ   ‹Ό$Θ   t$$‹΄$Μ   |$(‹Ό$Π   t$,‹΄$Τ   |$0‹Ό$Ψ   t$4|$8θόÿÿÿD$<‹D$T$@‹T$θ1ύÿÿD$DΉ    ‹΄$δ   T$H‹Ό$θ   t$L‹΄$ά   |$P‹Ό$ΰ   t$T‹΄$ό   |$X‹Ό$   t$\‹΄$  |$`‹Ό$  t$d‹T$|$h‹΄$  ‹•    t$l‹Ό$  |$p1ÿ‹4‰Π‰$θόÿÿÿ‹$Ζ‰$‹\$ Χ‹T$t$t‰L$‹L$|$x‹<•    1φλ΄&    ‹$‹,²‹D$‹$°‹DΕ‰,²1ÒΑΣƒΖƒώ
uΩ‰\$ ‹¬$   ‰L$‹$ιZώÿÿ΄&    v ‰l$|‹¬$€   »ÿÿÿÿθόÿÿÿD$tT$x‰D$t‹D$$‰T$x‹T$(θόÿÿÿRΊ°Ϊ P‰θθόÿÿÿ‹D$4‹T$8θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$D‹T$HθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$T‹T$XθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$d‹T$hƒΔ θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$T‹T$XθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$d‹T$hθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$t‹T$xθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹„$„   ‹”$   ƒΔ θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$t‹T$xθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‰θΊ
   θόÿÿÿΈ    ƒΔ‰D$Tι  t& ‹|$‹t$TΉ   4    σ¥‹”$Π   ‹$Ψ   ‹„$Μ   ‹΄$Δ   ‰T$‹”$Τ   ‰L$$Δ   ‹Ό$Θ   ‰$‰Θ‰T$‰Ϊ‰L$θόÿÿÿ‰D$‹D$‰T$‰ΪθJϊÿÿ‹$θ   ‰T$(‹”$δ   ‰L$0‹$ΰ   ‰T$,‹”$ά   ‰L$8‹$   ‰T$4‹”$ό   ‰L$@‹$  ‰T$<‹”$  ‰L$H‹$  ‰T$D‹”$  ‰L$P‰D$$‰T$LShΈΪ Uθόÿÿÿ‰π‰ϊθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$‹T$θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$$‹T$(θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$4‹T$8ƒΔ$θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$,‹T$0θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$<‹T$@θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$L‹T$PθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$\‹T$`ƒΔ θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$L‹T$PθόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿ‹D$\‹T$`θόÿÿÿRΊ¶Ϊ P‰θθόÿÿÿΊ
   ‰θθόÿÿÿƒΔ‰ΨΊ    θόÿÿÿ‰Γ;    ‚Ούÿÿÿt$xΊΎΪ ‰θÿt$xθόÿÿÿ1ΐθόÿÿÿ‹    ^‰ΗX9ϋvg1Ϋt& ‰ώ)ήt'΄&    v »   Ί ` ‰θ9ήFήθόÿÿÿ)ήuγ‰ψ_θόÿÿÿ1ÒRΊ¶Ϊ P‰θθόÿÿÿ‰ΨθόÿÿÿZY‰Η΅    9Ηr¥‰Ϊ‰Γ)Σ…Ϋt v Ύ   Ί ` ‰θ9σFσ6θόÿÿÿ)σuγθόÿÿÿ‰D$θόÿÿÿ‹΄$„   ‹Ό$   ‰Γ΅    ‰$θόÿÿÿ‹L$QSÿt$WVRPhLu Uθόÿÿÿ‰θΊΔΪ ƒΔ$ÿt$ ÿt$ θόÿÿÿ$¨   Y^‹D$|‹t$λ
t& ‹ƒΓ1ÒRΊ¶Ϊ P‰θθόÿÿÿXZ9σuε‰θΊ
   θόÿÿÿ‹„$  d+    uΔ  1ΐ[^_]ΓθόÿÿÿfffffffθόÿÿÿU½    WVS»ÿÿÿÿƒμx‰D$d΅    ‰D$t1ΐΗ$    ΗD$    λ/΄&    f‰ΪD$$|$$Ή   ‹4    ξσ¥θόÿÿÿ$T$‰ΨΊ    θόÿÿÿ‰Γ;    rΔΈ   θόÿÿÿ‰Ρ‰ΒD$θόÿÿÿd΅    ‹€¤  ‹@‹H$‹X(L$‹@,\$D$™R‰Κ‰ΩPD$ θόÿÿÿ‹\$‹|$ ‹l$(XZϋÿΙ;v‰ΪΉƒK Ακ	‰ΠχαΑκiÒ Κ;)Σ‹4$Ή Κ;‰Ϊ‰πχρΉkΚ_k‰Ζ‰Π‰ΣχιΑϋ‰θΑύΑϊ)ΪRχιVΑϊ)κRWhΤΪ ÿt$θόÿÿÿƒΔ‹D$td+    u
ƒΔx1ΐ[^_]ΓθόÿÿÿfffffθόÿÿÿVS‹X‹pΎƒώ~<0t-‰Ω1Òλϊ—™™w’P‰π)ΘΨ…ΐ~Ύƒθ0ƒΑƒψ	vΪΊÿÿÿÿ‰Π[^Γθόÿÿÿ‰Αd΅    ‹¤  ‹RΒΓ   R‹¤  ‹RΒ‚   R‹€¤  ÿph    Qθόÿÿÿ1ΐƒΔΓffffθόÿÿÿUΊϊΪ W‰ΗVS»ÿÿÿÿθόÿÿÿλt& ShΫ WθόÿÿÿƒΔ‰ΨΊ    θόÿÿÿ‰Γ;    rΫΊ
   ‰ψ1φ½    θόÿÿÿ΄&    ÿ4µ    »ÿÿÿÿhΫ WθόÿÿÿƒΔλ t& Dµ     ÿphΫ WθόÿÿÿƒΔ‰ΨΊ    θόÿÿÿ‰Γ;    rΞΊ
   ‰ψƒΖθόÿÿÿƒώ
u[1ΐ^_]ΓfθόÿÿÿU‰ΝΉÿ΅  W‰ΧV‰ΖS‹@Tθόÿÿÿ…ΐt0‰hψ‰Γ‰ΒΗ@ΐa ‰ψθόÿÿÿ‰πΊ    θόÿÿÿ‰ς‰Ψ[^_]ιόÿÿÿf[Έώÿÿÿ^_]Γ¶    θόÿÿÿU‰ΕWVS‰Σ1Òƒμ‹@‹@Ψθόÿÿÿ…ΐ„™  ‹S‰Η‹C‰Φ	Ζ„©   ‰Αƒρ	Ρ„Χ   Ή   9ΑΉ    Ρ|Y4…80 ώd0 vλHv ƒCƒS ƒΖώd0 w2‹‹
‰T$‰Θ‰$θόÿÿÿ‹T$R‰ΪWh€'3 P‹L$‰θθόÿÿÿƒΔ„ΐu»OΈÿÿÿÿπΑGƒψ„Ν   1Ϋ…ΐΫ   ƒΔ‰Ψ[^_]Γ΄&    f‹Ej1ÒΉ   ‹@ ‹@(RΊ#Ϋ P‰Ψj j ‹3θόÿÿÿƒΔ…ΐu ΗC   ΗC    ‹U‹3BH‰$‰D$θόÿÿÿ‹$‹D$‹R‹R ‹R(‰$θόÿÿÿ1ÒΉ   j‹D$RΊ%Ϋ P‰ΨÿsÿsθόÿÿÿƒΔ…ΐ…:ÿÿÿΗC   Ύ@0 ΗC    ιπώÿÿt& ‰ψ1Ϋθόÿÿÿ‰ΨƒΔ[^_]Γt& ‰ΘΊ   θόÿÿÿ‰ΨƒΔ[^_]Γ»ώÿÿÿιÿÿÿ΄&    θόÿÿÿUW‰ΟV‰ΦSƒμ<‹@ d‹    ‰T$81Ò‹hψ‹@Ψθόÿÿÿ…ΐ„΅   Ί	   ‰ΓθόÿÿÿΉσÿÿÿ„ΐu0sΈÿÿÿÿπΑCƒψtN…ΐ~b‹D$8d+    upƒΔ<‰Θ[^_]Γv U‰ΩΊ2   D$
θόÿÿÿ‰ΑX…ΙxΈL$‰ϊ‰πθόÿÿÿ‰Αλ§΄&    ‰$‰Ψθόÿÿÿ‹$λ§΄&    fΊ   ‰π‰$θόÿÿÿ‹$λΉσÿÿÿλƒθόÿÿÿθόÿÿÿWVΎφÿÿÿSƒμd‹    ‰L$1Ι…ΐtM‹BΨ‹zψ1ÒΎσÿÿÿθόÿÿÿ‰Γ…ΐt5Ί	   θόÿÿÿ„ΐt‰ω‰Ϊ‰ΰθόÿÿÿ‰Ζ…ΐt0KΈÿÿÿÿπΑCƒψtF…ΐ~*‹D$d+    u>ƒΔ‰π[^_Γt& ‰ΰθόÿÿÿ‰ΖλΕt& Ί   ‰ΘθόÿÿÿλΘ΄&    v ‰ΨθόÿÿÿλµθόÿÿÿfθόÿÿÿU‰Υ1ÒWVSƒμ‹@Ψθόÿÿÿ‰D$…ΐ„   ‰,$‹}»@0 ‹3‹.‰θθόÿÿÿ9Ηu‹$‰ω‰κ‹@θόÿÿÿ…ΐt>ƒΓϋh0 uΣ»ώÿÿÿ‹T$ΈÿÿÿÿJπΑBƒψt0…ΐ~SƒΔ‰Ψ[^_]Γ΄&    v ‹,$‹T$‰ρ‰θθϋÿÿ‰ΓλΎt& ‹D$θόÿÿÿ‰ΨƒΔ[^_]Γt& ƒΔ»ώÿÿÿ‰Ψ[^_]Γ‰ΘΊ   θόÿÿÿ‰ΨƒΔ[^_]ΓfffffθόÿÿÿW‰ΟVS‹R‰Γd΅    ‹’  ‹
Ί   θόÿÿÿ…ΐtE‰Ζ΅   …ΫtJΉ   Ίΐ  θόÿÿÿ‰Γ…ΐtUVh(Ϋ SθόÿÿÿΗ    ‰_ƒΔ‰Ψ[^_Γ΄&    »ώÿÿÿ‰Ψ[^_Γt& Ή   Ί 
  θόÿÿÿ‰Γ…ΐu¶»φÿÿÿλΗt& »τÿÿÿλΌ΄&    fθόÿÿÿW|$ƒδπÿwόU‰εWVS‰Γƒμ<d΅    ‰Eδ‹ƒ  ‰EΔ‹C<‹x ƒΗx‰ψθόÿÿÿ‹C<Ί+Ϋ θόÿÿÿ…ΐ„   ‰Ζ‰Ψθόÿÿÿ‰Γ…ΐ„ƒ   ΅TK  ‰Ϊ‰C(EΘθόÿÿÿ‹MΘ‰KT‹UΜ‰SX‹EΠ‰S@‰SLΊÿ΅ÿÿ‰C\‰K<‰CD‰KH‰CP‰πf‰‰ΪΗC    ΗC    ΗC@b θόÿÿÿ‰ψθόÿÿÿ‹EΔ‰p1ΐ‹Uδd+    u,eτ[^_]gψ_Γ‰πθόÿÿÿ‰ψθόÿÿÿh”u θόÿÿÿXΈτÿÿÿλΘθόÿÿÿfθόÿÿÿUW‰ΟVS‰Γd‹5    ƒμ‹BΊ   ‹€  ‹‰π‰$θόÿÿÿ‹$1Ò‰Ε‰πθόÿÿÿ…ΐtM‰Ζ΅   …ΫtRΉ   Ίΐ  θόÿÿÿ‰Γ…ΐt]VUh0Ϋ SθόÿÿÿΗ    ‰_ƒΔƒΔ‰Ψ[^_]Γ΄&    v ƒΔ»ώÿÿÿ‰Ψ[^_]ΓΉ   Ί 
  θόÿÿÿ‰Γ…ΐu®»φÿÿÿλΐt& »τÿÿÿλµ΄&    fθόÿÿÿW|$ƒδπÿwόU‰εWVS‰Γƒμ<d΅    ‰Eδ‹ƒ  ‰EΔ‹C<‹x ƒΗx‰ψθόÿÿÿ‹C<Ί;Ϋ θόÿÿÿ…ΐ„   ‰Ζ‰Ψθόÿÿÿ‰Γ…ΐ„ƒ   ΅XK  ‰Ϊ‰C(EΘθόÿÿÿ‹MΘ‰KT‹UΜ‰SX‹EΠ‰S@‰SLΊÿ΅ÿÿ‰C\‰K<‰CD‰KH‰CP‰πf‰‰ΪΗC    ΗC    ΗCΐb θ