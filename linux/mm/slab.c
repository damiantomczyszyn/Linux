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
�0�D$���������f������UWV��S�ø   ��������Ÿ   ���������   ��t����[^_]Ít& �0   �   ��  ��������   ����   �@1 ��t�   �Cp�u��K4�S8�sp9�u�`� 3PH3HD	�t��Sx�Љ$������`� �pD�xH�   ������Cp�s4�{8�Cp�   ������$d�    ��t����������[^_]Ív �K����x���������۽�����'��������������&    �t& �����UW��VS��D  ���   ��$X  �L$�$�\$d�    ��$@  1҉D$�D1 �����������D$ P�L$,�T$(�D$0�n����t$(X����  ��S�3   9��    �D$щT$�  �l$�D$�T$�^49؉Ѓ� ��  �D$(�D$�D$�T$�L$�9D$�ȃ� ��  1���$�   �$   �5    �|$4�   �D$0    ���D$5R�D$Pvmli�D$Tnux ������¸O   9�FT$`����  �L��׉L\�H����t$�D` ��  ��������ǅ���  h�   �,� ��$�   P��j�T$<�����,� j|�D$DP��j�T$H�����5    �,� d�    P��j�T$T������    ��+D$T��$�1� 9�G�P���5    j �T$<�����D$��D$()�9�G�)ڃ���ƅ��;  �¹   ��������$�����������  ��)��   ������L$4$��1�Q�+D$ ��   �%�  �Q)�9�G����Y  ������D1 ������D$��$@  d+    ��  ��D  [^_]�1�1ɉ��   ����r�ǻ   �    �T$,f��$�   �4   �   �4   f��$�   �D$)�Ǆ$�   ELF9�Ǆ$�    G؍�$�   Ǆ$�       Ǆ$�       �f��$�   Ǆ$�      Ǆ$�   4   f��$�   f��$�   ���  �   �ډ�������$�ى���������P  �L$�l$1҉�Q�D$�T$��Q)������$������D$+D$ �   ��   �%�  )�9�G��D$   1��x��&    ���f  ���P  �D$����������D$�   ��������T$�$�����������   �t& �L$)�1�9Q�<$�   9�F��D$���#����D$�P�����   �C9���   C9�v~���  ��   ��\1 ��   �C����   �D�������   ����   �   ����������$������������X���������D1 ����������������&    �\1 ��\1 t&��&    �v �C9�wC9��X������\1 u���  t<�$���������u�1�������=   �{T �$���������������k����v ��������������������t& ��   @������    ��t���    �t���������T$�ȅ�t��A%  �=   �t��ȉL$������T$��t�L$�A�\� ���b�����������$����������#����������    ���3�����T$`��$����T�f�T^����f���  ��������D$���  �D$�L$(�\1 �H�� �L$�L$ �@�   �X��   @��\1 t\�\$���    �@   �J��H�J�H�Z��t	�������u��   @�p�J�� �@�   �H�H����\1 u��\$�D$�ދL$� )ƍ|�9�G���x_�   ���������$����������uC�D$������L$1҉�Q�D$�T$��Q)��0���4$�p�����������v����D$���g���������D1 ��������������������������l� ������l� �h� �����1�Í�&    ��    ������   Ít& ������V�pS�Ép�p�l� �������1 �=h�  �5�1 �C�1 �C�0t�=   ��T [�l� ^�������&    �t& �����S�øl� ������C�S�=h�  �B��C"  t�=   ��T �l� �������1 [�������������� �����������Dѡ�� ���������� ���     ��������     Í�&    f��������x7W��V��S�ˉ�   ��������ى�������[^��_����Í�&    �v �����Í�&    ������1�Í�&    ������Í�&    �v ������������    �����UWV��S��L�BP�:�J�T$1������D$    )��D$ �Ӊ$�L$8���$�T$$�T$��$�;$��T$�T$C�C�9x� �|� ��  �F,���  ����T$$1��FL@h    �D$<    �F,��� 9D$ �D$@    ��'  ��� �|$ 1ۋl$$�9׉����  ��2 �D$=�2 u��  �t& ��D$� �D$=�2 ��  �D$�l$$�X�H�ǉډ�GW�|$ 9ǉ��sŉǋD$8+|$ ��l$$1҉�9��ƋD$ C��|$�t$,�֋T$$C�GW)�ډt$0�t$,���T$�$�ǋD$<�t$D�D$H�D$�B$�R(�D$��1 �T$������D$4��1 =�1 ��  �    1�D$(������D$�Ɖ��9���   ���1 �w��X�=�1 t&��    ���t����������t�C�X�=�1 u��G9|$vx��뻍v 94$s.�$�D$�t$�t$)΍(��V�D$�����������   ��D$�t$�t$h   �(�L$4�D$�����������   �<$��   �G9|$w�9<$rV1��D$4������   �T$4��1 ��������~  �|$,�l$0|$ l$$|$<�|$Dl$@)|$8����1���L[^_]ËD$�t$�t$�L$�(��)���P�D$����������{����D$�������@ j �T$�����Z�D$4�����c����\����t$�t$�t$4�L$�T$ �D$������Ã��'����|$ �t$8�l$$)���9�G�)��D$�� �ӋT$<�T$HSj �����ZY����   )މt$8�&���1��Չ|$ \$<�l$$T$@������T$ �t$8)�9�G��� �щ��ËD$��   @���p(�p$S���������uJ)މt$8�����1�\$<��� \$ �l$@l$$�v����|$�G j ��L$L�����X�������L[^_]ø�����~���������t���������j�����&    ��&    ������������    �����UWVS���D$�D$0�$�Q�D$��L$�����Ƹ�1 ���  ������D$�D$    ��&    �$�   )�9�G血1 �X�=�1 t"f����t����������t}�C�X�=�1 u��|$ ��   �t$,���W�T$�����[_����   �|$1�/W��1�l$l$),$�x����D$������   �T$��1 ������D$��[^_]ËD$,����   ��so��t��D$�  ��   t��D$1�f�T(�농�&    �t$,���W�T$�����ZY���c������D$����uq�T$��1 ������\$�D$��[^_]�f��D$�x�     ����D(�    )����D$,���������t& �D$������������������������닍�&    �t& �����S�\$��u
1�[Í�    �ۉ\$[�������&    �t& �������u1�Ít& j j �����ZYÍt& �����UWVS��4�t$H�T$$�L$�t$d�    �D$01��D$     ���h  �^�͋�5x� �=|� ��9���I  �����D$    )ȉ,$�9��D$s��)ȉD$��� ���D$     9��� �*  �5�� ���9�� �m  �-�2 ���2 ��   �v �D$�U��X�T$�E�߉D$EU9����   )�ډƋD$�׉4$1ҋ4$�|$;$��|$B��ȉt$�֋|$��Ct$EU+D$T$�t$�D$(�T$,��t&j ��j�D$,�L$0����ZY���  �D$��X�ډ�D$T$|$$�\$|$ )|$��St�m ���2 �*�����&    �v �D$0d+    ��   �D$ ��4[^_]Ív �\$)ȉ�9�G��� �Ɖ��D$$���������   �D$1ۉ�t$ X)t$��Xt�t$$��� ������    �|$)Ήʉ�9��t$$G�)��� �����t�����u/�L$1�Q\$ )߉|$�E���މt$$�E����D$ �1����D$ �����$����������&    ��&    ��������u1�Ít& j j �'���ZYÍt& ������p� ��t3������p�     �"���2 ��P�Q�
�    �@"  �������2 =�2 u��^���f�f�f�f�f�f�f������j1�1�1������X1�Í�&    �v �����V��S���@!tj1�1ɸ	   �����Z��t�D$   �ى�[�   ^����������[^Í�&    �v �����j1�1ɸ   �����ZÍ�&    f������S��t��Ѻ    ��t	��t�����j1�1ɸ	   �����Z[�����A�f�f�f������U��WVS���    �\$�T$�ʃ���� �[����	׃��  ����1�9���   )�9�G���   �����$��    ��1�1�������� wG��    ��t<��    ��t0������م�t#�i�]���Dً��u�y�}R��&    �v ���������um���������;<$u�+|$�D$������X�D$��X����[^_]Í�    ��   u�A�t��������랍�    �A���돍�&    +|$�t$���>�����V멿����뢍�&    �v �����UWVS�Ӄ��    �$�x��D$���� �P� ����	̓���   ����1�9���   )�9�G�����   ����������������9�tq��1�1�������� w-��    ��t"��    ��t������م�t	�������1҉��������t�+,$�\$��+�����S����[^_]Í�&    �v �\$+,$��S�\$���S��[^_]ý�����Í�&    ��������C  U��WVS�Ã����� �S�D$    �D$�D$�B���D�1�1�� ����  �S�B���D��@�:  �S�B���DË@����u
���    �֋�   t��΀�֋C�t
���   �։����������  ���   �֋C��  ���   ���  �K�Q���DӋR���I  ��  ���   �u
���  � ��%  �=   �u
���   �֋C�T$�<$�D$��	�T$������	�	��D$�C�t,�S�|$�B���DËP�J���E�������DǉD$�T$�D$�|$��������������ЋT$����	��D$���T$�������t$��	��D$�������ЋT$%   	��D$���T$���� 	��D$��������	���������	�S����   ��@	ȍJ�	�D$��Eً��   t��� ��E\$�L$���\$���ϋL$��   �������ً\$�� @  	ϋL$���\$�	�L$�����\$�΋L$������   �\$	։ʋL$����	�\$�4�    �L$����	֋\$��    �L$����	�\$�΋L$����
���\$	։ʋL$������ ��	��@	�	�$�e�[^_]Ít& ���   u�C���   �S�B���DËH�Q���DЋ��uE�H�Q���D��Bu3�    9��6������  @���'�����    ����������t& ���  @ ��������&    f�����������t& �������1��������Ɖ��\����t& �����    �؃��)���i�����;    ��������   ��������&    f����������t����֋C������    �   1�Í�&    ������UWVS�Ӄ��    �$�x��D$���� �P� ����	̓���   ����1�9��}   )�9�G�����   ����������������9�ta��1�������� w"��    ��t��    1���t	���������������������t�+,$�\$��+�����S����[^_]�f��\$+,$��S�\$���S��[^_]ý������f�f�f���������� ��t�����1��f�f�f�f����������B ��t���  �P��DЋR,�B$��t"S���t�ȉ�[�������&    f�1�[Ít& 1�Ít& �������9���  ~ S�   �X@�H<��   �Z[�
Í�&    �   ��   Ít& ��������Ћ@D��  �H9Jt1�Ív �@9B����Ív �����WV��S�ӋR ��t���  �B��DH,�A$��t�x��t�������[^_Ív �ډ��� �����1�[^_Í�&    �t& �������J�A    �������&    �t& �������P ��t���  �R�@T������������&    ��&    �����WVS�Ë|$�D$��~m��~d=�   ua��  �J��@������ƅ�tT����t��������ǉ��������t:���؉�������É��������t"��[^_�������&    ����[1�^_Í�    [�����^_Í�&    �����j�t$�P���ZYÍ�&    ��    �����j �t$�0���ZYÍ�&    ��    ������x     t
1�Í�&    ��  �@Í�&    ��&    ������z     ��   UWV��S�z<��t�GH������F���|   ����   ��    �@��u���&    9�tZ�����&    ���t<�Ë@9�u��t/�k�������������������ŉ�������� ���w!�ډ�볍v ������������[��^_]Ív ��[^��_]��=   ��T ���������������������  �@Ð�����U�   ��  WV��S���x�   ��������D  ����3 �P�W�FD�P�P�P�P� �3 ���������= ���w�h<��t�EH������n$1���[^_]Í�    ��  ��G��  ��C   �K4�C�O�C     �K8�Cd    �J�At�C,�i ǃ     �x<ǃ�      ���$������$�B���������$��������$��tX���������tM�K0   @�C<��ǃ|      ������E�U�H(�Q�M�H(�M�P(��������k<�����������v ���������������������������t& ������S�Ë@D������CD    [Í�&    �����WV��S��  �C�x<��������S�C�B����C   �C"  ��������������[^_�����f������W��VS�ø�2 ��������sHt��t��2 �������[^_Í�    �    ��  ������CH�ƅ�t��     �@�@�    ������CH�P�P�P�P�P�P�CH�P�P �P�P$�P�P(�CH�P,�@4    �P,�P0�CH�@8    �CH�@<    �sH��2 �������[^_Í�&    �����W��V��S�Ë@ ��t���  �   ������tW�P,�C �������Z[^_ø��������&    ��    �����S�É��KF�SHf���tA�
�H�J�H�J�H<�J�H@�J�HD�J�HH�J�HL�J�HP�J �HT�J$�HX�R(�P\�SD��f��t
[Í�&    ��S$[��������t& �����UW��VS���B�p ���  �n`�A�$��D��X,��<���������������$���:�������    �������������������1���[^_]Ít& �����UWVS�����u_�΋��  �Ӎk`�A�$��D��x,��<���������������$��������ډ�    ������É�����������������[^_]Ð������&    f������UWVS��d�    �T$�T$ �\$�|$$��������  �ƋC��DË@,�@��   �   ����������   �[H��,��t}�   ��S8�����   H�l$(����S<�L �������   $T����t$0U���������u�,$��x�t& ��)k<�f��������K8�D$d+    u@����[^_]�T��1��t$0�t$0������D$����x��)C<���v �����뺺�����������t& ������UWVS���L$$�T$�t$�l$ �$�|$(������   �Ë��  ������t&�$�|$�ڃ�,�D$     �L$����[^_]������������[^_]Í�&    ��&    ������UWVS�T$�ˋl$�|$��������  �Ƹ�2 ������[H��2 �������t�|$�C,��[��^_]�����[�����^_]Ív �����WV��S�Ӌ:�   �X�����t|��   t�S���   t�S�P��   t�S�P�S�P�S �P��    t�S$�P�S(�P�S,�P��@   t�S0�P �S4�P$�S8�P(1���t�Sf�VF[^_ø������������UWVS���B ���  �$��td�E�Ӊυ�Dŋp,��<��������ډ��    ������Å�t�����������[^_]Í�    ����������Å�uڋ$���    ������ɻ�����ɍ�&    f������W��VS�Ë@��DËp,��<�����������������É��������[^_Í�&    �����U��V��S����� d�    �T$�V<������Å�t�@tu�D$d+    ��   �e���[^]Ít& ���������C ���  ���@X    �C�j �F@���  �FFf���������$�KT�T$�SX�D$�SL�C\�CP�S@�ډCD���KH�K<�j����FD��f��tMf��t7f��t	��&    �C    ǃ�       �FEuA��������6����t& ��C    �捴&    �F,�V0ǃ�       �C4�S8�Ǎ�&    ��������������f������VS�Ë��  ��   ��������������[^�������    �����U��W��VS�ø�2 �t$������[H��2 �������t�t$�C,��[��^_]�����[�����^_]Ít& �����V��S�Ӻ   �+�����tj ��,����t$�t$�������[^ø������f�f������S�Ӆ�t�x �H� t�P��[�������    ��w+��t��t1���J� ���9�r�� �   [Ít& �   �א�����V��S���������t(���&    f���������������4P9�u�i�G��a[��%��������^�PDº���9�G�Í�&    ��&    �����UW��VS���D$�$d�    �D$1��D$/.. ����  �Յ���  ;l$�y  �$����  �|$�M�W�ɉ�DŅ�E��,�|$9x,�  �?���D$�D$9��0  ���(  ��1��
��    ��t�ƃ��@9�u��D$���D$�8�D$9|$�$  ���  �\$��1��f�9�t�Ë@����u��\$9���  ������9�u���9|$u��  ��&    f��@�9�u����Q  9D$��  1��|$ u�  ��    ��t�Ӄ��R9�u�9��  �|$ �  1����&    f���t�ʃ��I9�u��$�  ���|   �t$�|$01�\$1�f���1��   )�9�G��D$�����������T ��T 9�r�F���t&�4$ޅ�t�L$1��L���9�r�L$� ˃�;l$u��t$1����~   �v �D$1҅�t��    ���@9�u�;\$0s7�T$0�$)ڍ,1Ƀ���t�D$1���H� �D ��9�r�D$�D  �L$0���P�$)�9\$0F�������Ã�s��؋T$d+    �]  ��[^_]Ã|$0��   �T$0�   1Ʌ�t͋$� �   뿋|$�G��Dǋ@,�(;l$�D���뻃|$0v,�   �$1���J� ���9�r�$� �   �o����L$0�   ���^����L$0��u��Ӎ�    �$�   1���H� ���t��b����$1��  �������������9�sK�����&    f��@��9�u�9��x����@���g����D$���D$�8�D$9|$t
1ۄ����������<�����&    f�1�������D$1��^���1��W�������������������&    ������U��W��VS�ø�� ������t$����Ɖ�������ø�� �����X��[^_]��������t	���t	�� Ít& ���t& �����UW�ǉ�VS��T�L$h�T$�L$d�    �T$P��  ������$���\  �    ��  ������Å��3  ��  �������� ������G�ڹ   �D$h 
  j ������o ��XZ��x9wv���o �w��� �����������   �����d�    ����   ����   ���   ��C�s<�C�$	Չk@�C�D$�C  �f�CF�D$tf�CD�D$pD$lt41��|$�T$�   �D$l�D$   �D$�D$p�D$���������x2�D$��t���������u�D$Pd+    uL��T��[^_]Í�&    ��S<�D$������    ��������$1������뽍t& 1�뵍t& ����������������������@�
  WVS�Ë@ ��tv���  �F��DƋx,��<��������F��x�C�@ ��t���  9Ft�������1�[^_�f��V�C�������u�F��t�@D u|��������   [^_Ít& �{H��������C�@ ��t<���  ��t2��������F��DƋx,��<��������CX9F0��t��z�����&    ��������   �i�����&    �CT�N��  9H�D����j�����    ������f������U��WV��S�����PD�X(����f�� f�T$����8�uP��������Ǎt& ���t#;{rw-;srw&�S����������yX�[��u݃�1�[^_]Í�&    �[�Ív f�|$ �j� �a� UD��pRhH{ �������1���[^_]Í�&    u����C�[^_]Í�&    �t& �����UW��V��S�����  �C��DËh,��<�������1��CD t�G��  �H�V�����������t�@��xJ�G�����������t*= ���w�C0�FX��$������$���[^_]�����f�������܍�&    f��������1���[^_]Í�&    ��&    ������V��S�P�Z(��(��u+�C��&    �;Crw*�S����������yc�C�S��t�ËF�N�S9V r�vϋC�S��u�F�^�F    �F    ��N�Q(������FD��f��t�F�@01�[^Ív u������[^Í�    �F�@$�؍�&    ��&    ������U��W��VS�Ë@��DËp,��<��������؉�������Å�t	� ��t�����������[^_]Ít& �獴&    �t& ������UWVS�Ã��p����DË@,�D$���$�C=  ���   �t& ��CD��f����   �C������CH��tB�h,�H,�} 9�t)�\$���ύv �E��������������9�u�\$�CH�¡    �������� ������S<�$�������� ������    �����������   ����   ��v�C=  ��O����=   �B����  �K�r� �S��t�FQRPhl{ �������������&    �v �C$���������������������f��$������D$��[^_]�������&    ���[^_]Í�&    ��������t��u
�m����t& �Í�&    ��������   ��t
��u�G���1�Ít& �����UWV��S1ۃ�d�    �D$1���t.9�t\�C��������\  �[��tE�C��x���C  ���u��FD���f��uڍv �C(�������tˍX��@4��f��t��u��FD���f��t�#��&    �X��@4��f��u�C(�������u��t����s  ���FD��   �F=  ��_  �C��t\�{9{�8  �SD��f��u�h$�C�@0�C�P(��������C�{��t!�xH��t�G ������G �G�G$�G�G(�G��t��u���������u�������9������D$d+    ��   ��[^_]Ív �C(�������������X��@4��f��t������v �C��DËx,�C����   �o<�������������C=  �t71҉���0�������v ������   ���������C=  �u����������������������������&    ��� ����'������������t��������������U��WVS�Ã��D$$�t$�|$ ��t.�H1҅�x	9h��   ��u�$�D$$�����$�D$$��uO���������������$1�;$�T$D$r�M(��uS��&    �v 1���[^_]Í�    �ȍ�    ��t�P��x9Xtك��������u��ɍ�&    f��A��tɉ��A1҉�9ƃ� |�9����}��A�ߍt& �P 1�	������8�����    �����U��WVS��(�X�D$�S ���e  ���  �D$$�D$�M���   �ωD$�E	���  ��	���  �L$$�A��D�1��@,�x<��������L$$�AD �X  �t$���u�u�T$0�o����Ã����	  �CF�S�t$f���T$���Ɖ�������T$���C<�D$�C �E    �E�D$���   �����   �L$ �T$���������V�D$�u �D$���D$    �t$�t$�u�u�L$4�T$0�����������   ��������ES�D$�D$    �t$�t$�T$0�D$ �����Ã���t9�t$��    �C�������t#�Ã�t�P��x�;p�����C�������u݉�������D$ǀ�       �E����E    ��(1�[^_]ËCT��  �p���������j�B(1ҹ   R�u �s� P��j j ���������u��E   �E    �D$�} �p�FH�D$������V�D$�R �r(�����1ҹ   ��jR�u� V�u�u����������X����E   �E    ������D$$    ������&    ��&    �����U��W��VS�ø�� ���������Ɖ��Z�����ø�� �������[^_]Ít& ������V�Ƹ�� S�������� �   �É�����h�� hx� �����X��� Z��[^�������&    �t& �����VS�ø�� �����1ҹ�� h   �Ɖ�����Z����T �U ��&    �v �����S�ø�� ������[��t	���t����� �������[Í�    ��t& �������t�@��x�H��Ju��Ð1҉�Í�&    �t& �������t�������P��  �tÍt& �P��D�1ɋB,j �   ��0�����X�f������WVS�Y ��t���  �D$�T$���  �C��DË@,�p$��u[��tG�F��t@���������tL���������tH�D$�v���H��������Ɖ���������������[^_Ð�������&    f�������������ۉؾ����������͍t& �����VS�Z ��t���  �C��DË@,�p$��t*�F��t#���������t�V��������Ɖ��������[^þ�������������f������UWVS���  �|$�C��DË@,�h$��t;�E��t4�؉��������t.�V�ϋu��������Ɖ������[��^_]Í�&    �������������f�������PT�z     t1�Ít& �@ ��t􋀐  Í�&    �v �����S�Ë@�Ʌ�DË@,�t$�t$�t$Q�щ��t�������t��t	���t���X[Ív ��t& �����W��V��S�ø�� ������C���������t7�Å�u,�CDt+���t%�P��u󸠁 �������[^_Í�&    ;p@tϸ�� 1��������[^_Ð�����U��WV��S�Ã�d�    �D$�C��DË@,��<�$�������� �������������ǉD$��  9�Fǿ�� ��r�����1ɨt�f��   �t���|$�  ƀ��  w�D$�� ��&    �H� �D$�������t��tX�: t�؉���������֍�    ��� �������tD���t:���$������D$d+    u*����[^_]Í�&    �v ��� 1����������1���������t& ������� Í�&    ������WV��S�@��D�1ۋx,��<���������tM9�t�C���������   �[��th�CDuۋC��t
�C9Cu�C=  �u��k  �f�KD��u��FD���f��t빍t& ��X��@4��f��u��C(�������u��u�[��^_�������    �C(��������t����X��@4��f��t��_����v �z�����&    f������UWVS�Ã��x�G��Dǋp,��<��������S�GD�҉$����f�� ��f�T$����8���   ����f��t����������������[^_]���uk�t�G��x`�C�$�5����C ���{����Ņ�uċH��t�G ������G �G�G$�G�G(�G��������C��DË@,�@u��������뉍v ������u�����    f�|$ �j� �a� �WD��sRPhH{ �������������;�����&    �t& ������V�ƀ�@S�@�ɻ������DƋ@,j�t$�t$Q�щ��M�������t3�Å�t	���tJ���s�F,�C,�D$�C�D$�C8��������ƅ�u��[^Í�    ��u���|������[^Ít& �벍�&    �t& ������V�Ɖ�S�@��������DƋ@,jj j hmA  ��������t9�Å�t	���tP���sf�KD �F,�C    �C,���C8    ������ƅ�u��[^Í�    ��u����������[^Ít& �묍�&    �t& ������U�͹T   W��VS���   �$��  ���������   �@    �Í@<��� �@�  ��� �@�    �@�    �@�    ������C(�r� 1҉C(�C,���C    jj j hmA  �����ƃ���tP�h8��� ��� �X,�$�{$�C�C0�3������Ct����[^_]Ít& ���������؃�[^_]Í�&    �C������ػ����������؃�[^_]û����봍v �������t?VS�Ë@��DËp,��<��������C��t�C9Ct���8�����[^�������&    Í�&    �����S�Ë ��t���t$�� ���������t��u[�1����[Í�    �؍t& �������t�������P��  �tÍt& �P��D�1ɋB,j �   ��0�����X�f��������@Í�    �����WVS�Ã�d�    �D$�C��DËp,��<���������������CD����   �C�$    �D$    ��DËx,d�    �D$�D$��0�D$�D$��v ��������������������   ���������CEtًC=  �uω���������C9CuY1���C��������D$d+    uC����[^_Ív ��f�CD��t�S��t�S9St�������CD���   f�CD��������t& �����UW��VS��t_�Ë@�ͅ�DËp,��<�������������������t)�P��t�P9Pt�%����������1�[^_]Í�&    ��������������Rh�{ ����������Y[�Ѝ�&    �t& ������UWVS�Ã��@�$���r  �x,�ֽ������<��������C��x)�F��x"�FEu9st/�L$�$������������t8�����������[^_]Í�&    �D$9CuȋC�$������Ņ�u��̍t& �,$�D$�C��������L$��t��  �������������   �k9kt.�CD��f����   �C�$�@0�C�P(��������k�$��t�����   ����� �$������D$�$�k�s1��C��t�s�K��� ������S�C�d����C ��������t��M u��������1������������v �C�h$�\������������������������l���������VS�XD��q(�v�s8�I$[^�I�������&    ��&    �����W��VS�p(�$3 ��������ȑ �������t�K<�C@�A��C<   �C@"  ��u)�G(    �ȑ ������$3 �������[^_������t& 1��ڍ�&    �t& ������UW��VS�����   �$�XD��@D���   �k���������������te��$�@$�p,��������ƅ�u�$9xTt�����������������[^_]Ít& ��@L�CLu��t+�@��t$������ȍt& �9CPu��␾����뻍�&    f��$�KL�AL�CP�AL@m 땍�&    �v �����뒍�&    f������UWVS�Ë@T���   �pD�FP��t:���������{`��t�FP�H8��t���������ǋ�����[��^_]Í�&    �{`[^��_]Í�&    ��&    �����UW1�VS�Ë@T���   �pD�FP��t+����������t%�FP1��H4��t���������ǋ�����[��^_]ÿ������&    �����UWV��S���@T���   �XD�{P��tO��L$�$�������t=�CP������h,��t�t$ ���t$ �T$�L$�������XZ����������[^_]�f�������퍴&    f������WVS�Ë �@T���   �pD�FP��t1��������t&�FP�   �P��t	��������ǋ�������[^_Ð�   [^��_Ít& ������VS�Ë@T���   �pD�FP��t$��������t�FP���t��������[^�����[^Í�&    ��    �����W��VS�pD�ÍF�������������t^����@$�H�؅�t������ƃ��t��[^_Ít& [^_������{D��P$�J��t�غ�����������������[^_Í�&    ������[��^_Ít& ������WVS�Ë �xT���   �pD�FP��t1��������t&�FP�P$��t,��������Ë�������[^_Í�    �   ��[^_Ít& ���1�������ҍt& ������UWV��S��� �$���   �XD��@D@��   �$�{D�P�   9�FD$����   �C$������C�D$���������������   ��P(�R�S8�P$�j����   ���v���v�L$������ŋ������D$�����XZ��x'�   ���������$��������9�uT��nV9{DtP�����������[^_]Ít& ��D$��  ������ǅ��G���������Ґ����[^_]�����f������9{Du��C$��������[^_]Ít& ������D$����������뀍v �D$�����������j����t& ������UWVS��d�    �D$1���&    ��đ ������5 3 �� 3 �T  �F4�F4    � 3 �đ ������F��DƋ@,��<�D$������F����D֋R,�R(�j��H��&    j ��S�D$P�  �L$Q�   ������D$����������������������m�F�����UDƋ@,��(9���   �V<�E ������Å�tЋ~��������D$���|$�D$    ������ǅ�t�P<�E ����������\�����������C���  ���u�����   j ��Sf% �j f= @j �  @E���������G�����    �D$�����������������v �D$d+    u���đ [^_]������������&    ��    �����UWVS�����  �D$�G�$��Dǋp,�����������  �o$�   �E�],���w  �E$�   ���W  �FtA�$�@$�t�t$����  ����  �t�D$f� $��  ����  ��&    �   �T   ��  ������ƅ��o  �@��t;�đ ��� ������$�>�F�U�VH�}  td�M��t)�D$�����FD�  ��đ ��� ������Í�&    �ҍB�  Dº�  ������FD����  �F$�đ ��� ������U����  �$��m ������D$�D$����   �$���   �F�pD�A$��R  1۸$3 ������ȑ ������G(����   �� �P�N<���H�V@�F<�ȑ �
������$3 �������������U ��t@��������Å�t3����r����\$�$�D$������FD�������������	��D$������������D$��[^_]Ít& ���t�_(��1��_����ȑ ������$3 ������   �   ��  ������Å���   �    �C�đ ��� �C   ������C�C�C������1҅��������t& �E���~���1Ʌ����r�����&    f����A$�����t& ��D$���������v �$1�������D$�U����D$���������D$����������D$�����������    �����VS���@D��f����   �ȑ ������ƋC(��t��@j �   ���   �����Z��ȑ ������đ ������ƋC4��t��[�đ ^������t& ��������� 3 �3 �    � 3 �C4�    �������[�đ ^�����[^Í�&    �t& �����UWV��S�Ë��  ���   �xD�EE u.�����E������������GD������������1�[^_]Í�    �$3 ������GLu�E$�P��������OL�$3 �����륍v �����U��WVS���Z�$� ���   �xD�GH��tZ9��2  �wD��t\�G$���������   1ɉډ��������ډ�����������9�tL9wD��   �����������[^_]�f��   �wD9�G؅�u��C��  ������ƅ�u�������ɍt& �� �G�D$���������������   ��P$�j$��tm�$����p�p��������ŋ������D$�����XZ���Y����$��iQ9wD�M����G$��������[^_]Í�    1�������&    ������D$��������������������
�����    �D$�����������������&    ��    �����VS�XD���t��q$�v��t��������������C[^�������&    ��    �����WVS�Ë@D� �@$�p��t��������ƃ��t��[^_Ív ��Q 1�[��^_Ð�{D��P$�J��t�غ�����������������[^_Í�&    ������f�@D� u�f�WV�Ƹȑ S������~(����   ���ȑ ��������$3 �������X�9�u)�Q��� t�CLu�F$�P��������KL�C<�X�9�t*�FD��tҋC1�1ɋ@�@ jj j ������FD��뱐�$3 �������[1�^_�������&    �v [�ȑ ^_������v �����W��VS�Ë@�P�R ��t���  �r(��t�9�V���t	��t������F�O  9C8�E  [E�^_������WV��S���   �@�zD�X ��t���  ���������t2�C$��H(����t������Ɖ��������[^_���������ꍴ&    �O  [��^_Ít& ������V���  S�t$�̀j�t$�t$���������t^�p$�ËD$�T$�C,�D$$�S0�C�D$ �C8�N��tf�KD@�V,��tf�KD� �F��tf�KD  ��������ƅ�u��[^Ít& ��������[^Í�    �؉��������[^�f�f�f�f�f������UWVS���L$���m  �0   �ӹ   ��  ������D$�Ņ��z  ���  �p�x$�F��DƋ@,��<�D$������L$�,$�n�$��tI�o�����&    f�9�tT�Ë@��u�9�tG��)ȃ�=�  ��   �../ �v���^��u��$��o9�u�D$������������   �v �$9�t��1���tۋ@������t��m����9���u��~��$��+D$�=�  {�,$�	�t& �9�t3�W�Љ$������$)Ɖ��D5 �������t���D5 /��G��uɋD$������D$�L$�     �H�D$��[^_]��D$�����D$��[^_]ËD$�����������D$������\$���D$����������W��V��S�AH��t]��Xj��SQ���  ������Ã���tO�GD t�F�C�s$���������������ƅ�u��[^_Í�    �؉��������[^_Ð1�1�뢍�&    �v �������[^_�f�f�������U1�WVS����t@�Ë@�։ʋH�i4�I8��	�u6�;�O8�Y��tZ��8R�t$ ���t$ V������Ń�����[^_]Ít& �|$9l$�,$�    �}�1��|$l$9<$�}��$+T$뚽����뻍�&    f������WVS�8�ӉʋG�p8�N��t�I1���t�G8R�y�ى������Z[^_Ít& ������UWV�։�S�x�ËO�A4�I8��	�t�l$9D$�}<+D$9�N�1���t(��H8�Y��t(�@�@8R�t$���t$��V�������[^_]ø������������퍴&    f������S�Ӌ�@�J8�R�R8S�Y�����Z[Í�&    ��&    �������R8�R��tS�X��������   1�[Ít& 1�Ít& ������WV��S�X�����ǅ�t8��t41ɉ�������������ǅ�t����u(��t>���������[^_������v ��������Ս�&    ��1ɉ�������Ɖ���������u�[^_�f������U��W��VS�Ӄ���D$�t$�$�B�@8�H��t�I9_DuT�9�R8��������ǅ�x%�$��	�t���9��} )��3�؉������9�O�����[^_]Ív ��1�[��^_]Ít& 1���[��^_]Í�&    �v �����V��S��D�@�d�    �L$@1��������tD���@Ff������$   f% �	Ɖ�f�t$������Ɖ�������D$@d+    u��D��[^þ�������������&    f������V��S�Ã�������C�1��������t�$������$��[^Í�&    �t& ������VS�Ë@�p8����������������[^�������&    �v ������@������������WV��S���t �Z�t& ��~�1Ƀ���������C���u�[^_Í�&    �t& ������V��S�X��t0�ʉ�1�������Å�t���1��������[^������v [^Ít& ���������ԍ�&    ������@�1��������&    ��&    ������V1�S�@��������t��������Ɖ��������[^Ít& 1�[��^Í�&    ������D$��t��  tPh/� �����YX1�Í�&    �v �T$R�L$�   �����ZÍ�&    �t& ������D$��t��  t�t$Ph�{ �����1���Ít& ��|$�  wڍL$D$�   +T$Q�L$�����ZÍ�&    f������UWVS�Ã��hD�E �@�@8�p�$��t�v�����   �C�K9���   �    rr��)ǁ��  vv�z��1��    ǂ�      ���)���   ���ыE �>�@8�$�������x=�  �7U C;CwT�C1���[^_]Í�&    ���)���  w��K��1�[^_]Ív �������[^_]Í�&    f���    ��    �����WV��S��Dd�    �L$@1Ʌ�tb�@ t\�@������Å�tU�|$1��   ���D$T�t$�$   �D$��������Ɖ�������D$@d+    u��D��[^_Í�    ������ݾ�������������&    ��    �����UWVS�Ã�Dd�    �D$@1��C t`�͍|$�   ���C�T$���$   �l$�������u�S��t�RU��������Z�T$@d+    u��D[^_]Í�&    �v �������������t& �����WV��S�ÉȋK8�Q�R��tu�
�R��uS����   �ҹ�p � p D�j %�  1��t$����VR�j h   �t$,�t$,������� = ���wA��[^_Ív ��tL�ҹ�o �@o D�믍v �1h,| ����������XZ��[^_Ít& ��ǃ��u���������뭍�    �ҹ�o ��p E��`����t& �ҹ@p ��p E��H�����&    �t& ������WVS�Ã�d�    �D$1���tS�C��tL�ׅ�tF�Ή�L$��������O�C��V�t$�t$��������T$d+    u��[^_Í�&    f�������������f������UW��VS���te��1����&    f���u,1ɉ�������É�������u��u��[^_]Í�&    �v �4��t& ���E1ɋ���������9�u��[^_]�1��ō�    �����W��V��S���^d�    �D$1���t^�ʉ�1�������Å�tV���L$��������O����j �t$�t$������Ɖ���������D$d+    u ����[^_Í�&    �������릾�������������&    �t& �����WV�Ƹ@n S�z�Ӆ�t8j 1ҁ��  1��t$SP�CR�P���t$,�t$,������� = ���w&��[^_ËB�R��t.��� o ��n D�뭍t& ��ǃ��uӋ��������ȍ�    ����p ��n D��|����t& �����VS�Ã�d�    �D$1���tL�C��tE�օ�t?�L$���������N�C��j �t$�t$��������T$d+    u��[^Ít& ��������������&    �v �����WV�։�S��Dd�    �L$@1Ʌ���   �@ ��   �F ��   �N�@�I������Å�tt�@D�������f��t"��������D$@d+    ue��D��[^_Ít& ��C$9p8u֍|$1��   ���D$T�$   �D$�D$X�D$���������륍�&    �����뗍�&    f������������f�f�������@1������������W�   �׺�  V�ơ0   S������Å��OU h   ��1҉������X�OU �����WV��S�x1ۉ������= ���w��[^_Ít& ��Ã��u���������㍶    �����WVS�Ã�d�    �D$1���tc�C�=    �օ�t�x��to�L$�����������  ��VS�t$�t$�������= ���w.�C1��D$d+    u5����[^_Ív ������ލ�&    �ƃ��uЋ��������ž����������f������VS�Ëp�    ������C    �    �������t�FD��f��u��[^�������&    [^Ít& ��䍴&    �t& ������U��W��V��S�@��������ËFU��������Ɖ������X��[^_]Í�&    f������S�@��t�R��t�XQ�������Z[Í�    �X�    Q�������Z[�f�f������UWVS��tk�ƅ�te�׸    ����������t8��������    ��������ډ�������ŉ������1��� ���w1[^_]Ít& �    ����������[^_]Ít& �����[^_]Ít& ���u�D$t�ډ�������v ��볍�&    �t& ��������t�@��tj�9���ZÍ�&    �    ������Ív �������t�@1�������t& ��    1��������&    �t& ������W�=    V�։�S��t�x�N��t�I��������Å�tG�@D��f��u3�C$9p8u+�t$�L$�����������X���������[^_Í�&    �v �������������f��������t�@��tj �Y���ZÍ�&    �    ������Ív �����j�4���Z�f������W��V��S�ø    ������v��t�C�@D t �v�    ������C��[��^_������t& 1���f�f������VS��������Ɖ��������[�   ^�������&    �t& �����VS�p�ËV��t
�   ��������������[^������v �����WVS���@J@u�   ���������   �   �   ��  ������ƅ���   �   ������Freeb�F�ǡԑ �s���p ��tK�C(��t���   ���������   ��t7��~S�w4��t���   �   ����   ��tM�P	�x�s(�KZ1�[^_Ð������ƍ�&    f��   ���������f��   �������럸�����ƍv �   �������문����뮍v �����VS�Ëp�������u�~ t
�S$�RT�J4[^�f�f�f�f������W��V��S�Z��u��v �1ɉ�����������u�_��u��v �1ɉ�����������u�[^_������WV��S��X��t5��1ɉ�������Å�t4�؉��������t���������[^_������t& ��������Ӎ�&    �7�6h�| �������[^_Ð�����UW��VS��$�D$�T$�$d�    �D$ 1����+  �G���  ���  �$�H���  �t$�T$�L$��������$�����  �l$�F���;  1�������D$���mU �D$������D$�D$�D$�D$�$�h����   �E ����   1��n��&    �v �$�T� �x��t�D$���������f��t9�T� ����K�  �  j ���t$��  �t$�D$����������  ���D� ��t�|$�X��t���D$1������끐�$�x����   �1���ug�   �t& ��$���h��t�D$���������f��t4������K�  ��   j ���t$��  �t$�D$���������uu������t#�L$�X��t���D$1������눍�&    �1ۋD$������$���t�D$t�D$������t& �D$ d+    �;  ��$��[^_]Ít& ��$�ËD$������$� ��t��D$�����똍�    �P�������� ��� ��D�P�D$�0h�| ������������넍t& P�2h4} ����������������    j ��  �t$�t$$�t$$������D$��= ���������Ã���+����4$�D$��@����������t& �P�2h4} ������T� ��������t& ��D$�@�D$�X�����&    ��������������������    ����������������&    �������1�����f�������Ѻ   �o�����&    ��&    ������U1�WV��S�Ӄ��d�    �D$�F���������   �T$�ōL$��������[�����   1����uT�Jj ����t$�����t$�������D$����uҋ$��u#��������D$d+    uG�$��[^_]Ít& ��    )Å�xЍv �D��1ɋ���������s���$    ��$������������&    �t& �����V1�S�Ӌ@��������t/�[�Ƌ��t�t& �1ɉ�����������u��[^������v [^Ít& ������V��1�S�@�������t�É�1��������[^������t& �[^Í�&    ��    �����V��1�S�@�������t�L$���������Ɖ��������[^þ������&    �����U�Ÿ    W��V��S���\$������v����   ��������    �����1ɉ������������t|�ۋE�$D߉�������$����t'���������������� ���w1�����[^_]Ít& ��E��������$����������������[^_]Ít& ��    ����������븉����������몍�&    f������UWV��S�Ã�L�T$�|$�ʹ   d�    �D$H1��D$   �D$`�T$�D$�C �=  ��C����   ������Å��-  �T$��������$��t'��������D$Hd+    �  �$��L[^_]Ít& �D$�p��te���u+�]�t& ��T$������ŉ����������   �F����t4�1ɉ�������ǅ�u��$����냍�&    �v ������[�R����v �D$�h���Z����E ��u4�N����t& ��T$������D$��������T$��u4�E���������1ɉ�������ƅ�u�뀍v �,$������&    ��$������$����������$����������������&    �t& �����UWVS�l$�@ tD�Ӆ�t7���t1�Ɖσ����&    ������tU���������Z��t�[^_]Ív [1�^_]ø������f��������t/V�Ɖ�S���t�X��&    �����������S���u�[^Ít& Í�&    �����UWV��S���	��tT�ǉ�1���t& ������t>����=�������t�k���t*�������$���������$u����[^_]Í�    1Ƀ���[^_]Í�&    �t& �������х�t1��n�����    1�Ít& �������х�t�   �K����v 1��f�f�������U��WV��S�����0�	�D$d�    �D$,�B �L$�$�B\�D$���o  �X,�ׅ���   �D$����������ǅ�ul�D$t�F�C�D$t�F�C�D$t�F�C�F�C�F �C �D$ t�F$�C$�F(�C(�F,�C,�D$@t�F0�C0�F4�C4�F8�C8�D$u �D$,d+    ��   �e��[^_]Ít& ��$�v�@���������   �D$f�p$f�s뻍v �   �@   ��  ������Å���   �D$�$�@$�C    �C    f�C�D$������L$�K0�T$�S4�D$�K$�C8�C,�C �D$�S(�K�S�X,�������&    f��   ������������D��T����������������������������&    �t& ������U��W��V�Ɖ�S����� d�    �T$1�������Å�ti������C(�C �@X    �C@q �G,��ta�Pf��P�S�P�S�P�S<�P�S@�P �SD�P$�SH�P(�SL�P,�SP�P0�ST�P4�SX�@8�C\�D$d+    u?�e��[^_]Ðf�3�ډ�������$�KT�T$�SX�D$�KH�C\�SL�CP�K<�S@�CD��������&    �����U��VS�Ã���� d�    �D$1���t~�C �������t�D$d+    ul�e���[^]Ít& �s\�KT��������ƅ�t2�C�X ����������$�KT�T$�SX�D$�KH�C\�SL�CP뢍t& �����뗍�&    f�������������t& �������t'�H��t �P ��"u1���t�Í�    �@(�@Ð��&    ��&    �����WVS�X(��tQ�{H�։�������S ��t.�C��t'�CL���������������F ��[^_�������&    �[��^_�������    [^_Í�&    �t& ������U��W��VS���P\�@ �$��t{��x������$�Z�B�s�9�t;�<$�ǋF��t"��tK�V ��"u;1Ƀ�t�����������t=�^�s�9�u͋<$�G ��[^��x_]�������    �F(�H����    ��[^_]ø    �<$������V�F�B��    �^�^�����������������t,��u��F u��F,������F0������    ��������m�����f�f�f�f�������S���   �C@������C��t1�������������1�[Ív �����UW��VS��� �T$�P���   �R\�C�$�j0������{- �h  �C(�C,��um�K4�o�w�ș9�־    };�C0�)�É��%  ��   ������L$�����������GW���)  �$���������[^_]Í�&    f��u���t$������} ��   �SD1ɋC<�r1�������ƋD$�������~��C8��t9���   ��������C0����   �s4�D$������} tD�D$�����������C0������C4    �C0    �P����v ������������<�����&    �KD�S0�C<�i��������ƋD$�������x��C(    ����f������������t& ������������������������������������������&    ������UWV��S��� �$���   �{��������{, ��   �$�S4�C-�F�i�9�rU�K0�L$�l$�)���   1ɉ�������$�ډ�������É���F��^���������   ����[^_]Ív �C8��t9�r}��������D$����   �S0��t�K4�D$������C0������D$1҉�C4+K4������D$�k4��C0�F�R�����&    ����������������[^_]Í�&    �v �������������Z�����&    �������������@����������������/���������%�����&    �v �����UWVS���r�D$�F\�T$��  �$�H   �x0�   ��������k  �o�É������� �N  �F�@\�@�C<���  �F\�x���  �4$�{D��t�G�C8�G�C@���������   �C<�@,����   �@�C�D$�@$�t9�L$����   �$t�O����   ��t�SD�R ����   ��&    f��t.�D$f� $��   �$t�O���|   ��t
�CD�P��tn�C�ؑ ��� 1������1��C(   f�C,�D$���   �����������[^_]�f������������������������[^_]Ív ������ލ�&    f��C@�����������Ɛ�����뾾������f�������   �A����������   �1���������UW��VS�Ã�� ���   �$�u��������E����   ����  ������E(   �ǋE�8 ����   �����������[^_]Í�&    f��$�@�@\�P0�B�T$�$������T$�z ue�MD�U�E<�i��������ǋ$�������~����{S땍�&    1Ҹ�  ������E���Q���������m�����&    ������\����$�����������J����t& �����UWVS�Ã��8�$���   �u��������U(��uf�U �C�K�    9Ѓ� }8�}�)���   ���   ������$�����������CS����   �����������[^_]Í�&    ��G�@\�x0�E��th�G�D$������ u>�MD�U�E<�I������ǋD$�������x���   ^�E(    ���} �B����t& ��D$�����������t����t& ���  ������E��u�������T����t& �������C����t& ������3���������)���f������UWVS���   �Ӏ~- u9�F0��������   �C@������C��t1�������������1�[^_]Í�&    �B�@\�h0�}��������} u�nD�N4�V0�F<�m ������������둍�&    �v �����UWV��S�x0�Z�G �o\f�����xf�� ���������u0��j1҉�S������ËG ��x������؃�[^_]Í�&    �t& �����UWV��S�x0�Z�G �o\f�����xf�� ���������u0��j1҉�S������ËG ��x������؃�[^_]�f�f�f�f�f�������U��WVS�ˉу�����U��tT������������u9�Ή�p<x@���-  �P@�H<1�1�	�t�   �p<�H9�    �x@�|�e����[^_]ÉΉ�뿍�&    �v �Q\���   ����������D$�    �T$�$�\$������D$�$�X�P�\$�Ë@�B��T$�C   �C"  �\$�B��	�tB�T$��9�t9�t$�|$��t& ����� �����	�t9�tN�p��u� ��v �t$�|$�B�\$�L$�Z�Q�A��    ������e����[^_]Í�&    �v �t$�|$�������������������&    �����U�պ�  WVS�Ã��    �$�t$���������   �    �Ǎ@�G�G�$�o�G �    ������C!u8��t���K�G�w0�A�O�K�C�    �O���������[^_]Ít& ��    ������    ����������������[^_]ÿ�����ōt& �����V��S�H@��@�9�t5�A��Z��t& ��HL�J��@H   �@L"  ������؋SH�HH�Z�9�u�[^Ív �����WVS��td�p\��t]��9Vt;�F���X�9�u�7��&    �C�X�9�t&�C %  ��u�C(��������u�[1�^_Ít& �[�����^_Í�&    ��&    ��&    �����S�ø    ������C0�P\�B��~���B�    [�������&    ��&    ������H,��t$�I��t�I��t���������������&    f���&    ��&    �����U�ŉ�WVS����������   �{(�މ��������u�w<�
��&    �(���������t�9�tV�G0�@ ��x������o0�M\�Q���B�9�t�t& ��@ t;pte�P�B�9�u������E ��x�����[��^_]Ít& �F0�@\�p(�    ������ډ��D����ƅ�u
�C0�@\�@�    �������[^_]Ív ��t��p(�    ������ډ������ƅ�u
�C0�@\�@�    ������o0�n���������o�����&    �v �����VS�P�X�` ����9�t�r��F �u�F�p�9�u�[^�f��F(�@\�������v ������H�H    ����   W��VS�P�X�r�9�t9�F��t(�F �u!��t5��u@�F(���@\������t[^_Ív �F�p�9�u�[1�^_Í�&    �������ڍ�&    f��������tȋ^(��t�CH�����������밸����Í�&    f������U��WVS�Ã��P�M���   �BT�ΉD$�B\�$�E	��*  ��	��X  �G�D$�    ������$���D$�E��E��  �_9\$��   ��   �v �@ ����   �@(�$�    �������������������1҉��Cf����P�D$R��u P���u�u���������uz�    ������G�W�L$�B���H�_�G��˃E�U �9\$t8�C�s���t�C ���[����    ������D$�   ������$�V����v �    �������1�[^_]Ít& j�B 1ҹ   �u �@(R�� P��j j ���������u��E   �E    �s�] �FH�D$������V�D$�R �r(�����1ҹ   ��jR�� V�u�u����������m����E   �E    �I�����&    ��W�G�t$�B��$�B�p�G�D$�G�r�A�����    �����WV��S�ӉB$��tm������C(�K�P8��4�H�C�؉S �
������F<��t+�C<�C@�{@�p�9�t��������FH�p�9�u�[^_Ít& ���������t���ƍ�&    f�1����&    �v �����WV��S���������= �����   �@ ��x������O\�Q���B�9�t��&    �@ t;Xt%�P�B�9�u������G ��x������������[^_Å�t�X(�    �������������Å�u
�F0�@\�@�    �����봉É�[^_�f������UWV��S�z�G �o\��x������    ������] �    ��   �������u3j 1�1ɉ�����Z= ������   G؋G ��x�������[^_]Ít& �������⍴&    f������WV��S�@@�~@9�t�X���������CH�X�9�u�^(�F<    ��t:�N�V �F�Q�
�F�F ���F(    �F$    �������[^_�������&    [^_Í�&    �t& ������UWVS��8�P�` �������D$ �z��|$9�t �D$�@ ��u�D$�@�X��\$9D$ u���8[^_]Ët$����F �F�^�\$�x��|$9�tŋD$�@ ��u%�t& ��D$�@�x��|$9D$t��D$�@ ��t��T$����B �B���T$�x��<$9�t$�@ ��u$��&    �$�@�p��4$9D$t��$�@ ��t�$����C �C�{�|$�H�9�tɋA ��u��A�H�9D$t��A ��t����q�A �A�t$�P�9�tՉL$,�ыA ��u�A�H�9D$u�L$,뵍t& �����y�A �A�|$�P�9�t҉L$(�ՋE ��u�E�h�9D$u�L$(벍�&    f�����M�E �E�x�9�t҉l$$�G ��u�G�x�9�u�l$$붐����W�G �G�X�9�t܋C ��u�t& ��C�X�9�tŋC ��t����k�C �C�p�9�t��F!u�t& ��F�p�9�t��F!t����T$4�L$0������T$4�L$0�׍�&    �����VS�r���   �F ��x������    ������K�S�C�Q�
�C�C�    ������F ��x������C t1�[^Í�&    �C,������C0��t��t�    �������1�[^Ív ����������������   UW�Ǎ@HVS��������o\�U����Z��p�9�u"�   ��&    ��F�V���9���   �ƋC�$��t��C t۸    ������K�C�$�A��    �S�S������؉���������tT��u��C u��C,������C0��t��t9�    �ډ�������F�V��9��y�������[^_]�������&    ����������t& �Í�&    ��&    ������U�����WVS�z�   v	[��^_]�f��B�ֽ�����x\�    ������    �_ �������   ��uɸ    ���������X�9�u�   ��&    �C�X�9���   �C t�n����������������u׋C�xf���f�� ���t�����   ���^\�    �s(������׉��������= ���wZ�C t:�@4    �@8    ǀ�       �f��    1����������1��������[^_]��@4   �@8    ǀ�       �ԋ��t?��������C ������C,������C0��t��t�    ������������;����������ݍ�&    ��������tgS�ø    ������K�S�C�Q�
�C�C�    ��������t>��t	[Í�&    �C u�C,������C0��t��t�    ��[������Í�&    뾍t& ������ۍ�&    f��������tGVS�Í@H������s��t�FH������C\�D����C ��t
�F ����������������[^������v Í�&    ��&    ������UW��VS���h,�$���   �E����   ��   ��u�   ��&    f��E�0�����   ��������Å�t�E��t���ur�t& ��G0������4$�F �$��x������G0���vH�
����F؃H��������   ��������G �$��x�������������؃�[^_]ËE��t9���t3�   ��t& �E�0����t��������Å�t��i�����&    f���1ۉ�[^_]û�����R�����&    �v ���������   UW�Ǎ@HVS��������W\�B����X��i�9�u�   ��&    ��ŋC��tl�C �tf�s(�$�F ��x������[�C0�����C0������C0�^H������F �H��������   ��������F ��x��������������������$�E�M���9��x�������[^_]�������&    �v Í�&    ��&    ������UWVS���r\d�    �D$1��F ��  �Z�׍SH�Љ$������k�$��t�C\�$�@������$�ŉ�������E<�D$����  �U,����  �D$�@,����  � �D$��    �    ������    ������F��tE�    �����������    �������������D$d+    ��  ����[^_]Í�&    �v �G\�T$�T����Å��:  �    ������    ������N0�Q�L$�Љ$������$�L$������  �A��������OH�ȉ$������W�$��t�G\�@������$�ȉ$�������������$�z,��t�?�D$�T$��P�F @�$�B0��   �b����T$�B0�����T$�B0�����$������E,�T$���Q  �@��t�H��t�T$��������T$�ЉT$������T$��T$�����$������D$�������������D$�����������    �G\������    ������    ���������_����D$�@ ��x������D$�@ ��x������D$������������&    �����T$�B0������$������E,�T$��tp�@��t�H��t�T$��������T$�r(���%����B �J�T$�A��B�B�B ���B(    �B$    �������������T$�������&    ����������    �����������G\�����    �������������d���������������S���������v �����UWV��S���@<�^0�$�F$�x0�C\�P0�j�T$��������T$��kH�B������G ��x������    ������C\1��n����    ������F0�����F0�T����F0�|����C �H��������   ���������������G �  p t�S �   �
f�� �f�� @�  @D�H���  ��u5��x�������������$��P����������r����؃�[^_]������t& �Kj j Q�   P��������G ��뮍v 맍�&    �t& ������S���������[�������&    �t& �����UWV��S���X0�CT�x<�C\�P09{��U �j�$��������$���B������G ��x������C ��x������    ������    ������C\1����������U �    �kH������    ������F0�����F0������F0������C �H��������   ��������C ��x�������������G �  p ��   �S �   �
f�� �f�� @�  @D�H���  ��u>��x�������������d3 �������������d3 �������[^_]�������&    ��Kj j Q�   P��������G ��륍v 랍�&    �t& ������W��VS�X\��t&�Ƹ    �����9s(tV�    ��������t]��t[��^_�������&    �C u�C,������C0��t
��u������    ��������v �C(    �    ��������u�럍�&    ��    �������t��tÐ�������&    �t& �������t�� Ív �����VS�t$�Ӊ��t$�L$����Z��= ���wf�p$1҉X(��t��t	���t�� �C\1҉�[^Í�    �鍴&    �t& ������UWVS���L$�J�D$�$����   �$�z�֋@\�P�X�j�9�t3��    �E��t������������������   �E�h�9�uً$�@\�t$��h  h�A  �L$������Ã���uR��A  ���������= ���w]�@    ǀ�       ����������������t�FH������$�@ ������D$�p0����