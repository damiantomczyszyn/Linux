e if RCU thinks that the current CPU is not idle
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
	u64 period = tg_get_cfs_p