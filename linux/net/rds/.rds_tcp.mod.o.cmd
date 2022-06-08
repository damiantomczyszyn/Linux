 GFP_KERNEL, cpu_to_node(i));
		per_cpu(select_idle_mask, i) = (cpumask_var_t)kzalloc_node(
			cpumask_size(), GFP_KERNEL, cpu_to_node(i));
	}
#endif /* CONFIG_CPUMASK_OFFSTACK */

	init_rt_bandwidth(&def_rt_bandwidth, global_rt_period(), global_rt_runtime());

#ifdef CONFIG_SMP
	init_defrootdomain();
#endif

#ifdef CONFIG_RT_GROUP_SCHED
	init_rt_bandwidth(&root_task_group.rt_bandwidth,
			global_rt_period(), global_rt_runtime());
#endif /* CONFIG_RT_GROUP_SCHED */

#ifdef CONFIG_CGROUP_SCHED
	task_group_cache = KMEM_CACHE(task_group, 0);

	list_add(&root_task_group.list, &task_groups);
	INIT_LIST_HEAD(&root_task_group.children);
	INIT_LIST_HEAD(&root_task_group.siblings);
	autogroup_init(&init_task);
#endif /* CONFIG_CGROUP_SCHED */

	for_each_possible_cpu(i) {
		struct rq *rq;

		rq = cpu_rq(i);
		raw_spin_lock_init(&rq->__lock);
		rq->nr_running = 0;
		rq->calc_load_active = 0;
		rq->calc_load_update = jiffies + LOAD_FREQ;
		init_cfs_rq(&rq->cfs);
		init_rt_rq(&rq->rt);
		init_dl_rq(&rq->dl);
#ifdef CONFIG_FAIR_GROUP_SCHED
		INIT_LIST_HEAD(&rq->leaf_cfs_rq_list);
		rq->tmp_alone_branch = &rq->leaf_cfs_rq_list;
		/*
		 * How much CPU bandwidth does root_task_group get?
		 *
		 * In case of task-groups formed thr' the cgroup filesystem, it
		 * gets 100% of the CPU resources in the system. This overall
		 * system CPU resource is divided among the tasks of
		 * root_task_group and its child task-groups in a fair manner,
		 * based on each entity's (task or task-group's) weight
		 * (se->load.weight).
		 *
		 * In other words, if root_task_group has 10 tasks of weight
		 * 1024) and two child groups A0 and A1 (of weight 1024 each),
		 * then A0's share of the CPU resource is:
		 *
		 *	A0's bandwidth = 1024 / (10*1024 + 1024 + 1024) = 8.33%
		 *
		 * We achieve this by letting root_task_group's tasks sit
		 * directly in rq->cfs (i.e root_task_group->se[] = NULL).
		 */
		init_tg_cfs_entry(&root_task_group, &rq->cfs, NULL, i, NULL);
#endif /* CONFIG_FAIR_GROUP_SCHED */

		rq->rt.rt_runtime = def_rt_bandwidth.rt_runtime;
#ifdef CONFIG_RT_GROUP_SCHED
		init_tg_rt_entry(&root_task_group, &rq->rt, NULL, i, NULL);
#endif
#ifdef CONFIG_SMP
		rq->sd = NULL;
		rq->rd = NULL;
		rq->cpu_capacity = rq->cpu_capacity_orig = SCHED_CAPACITY_SCALE;
		rq->balance_callback = &balance_push_callback;
		rq->active_balance = 0;
		rq->next_balance = jiffies;
		rq->push_cpu = 0;
		rq->cpu = i;
		rq->online = 0;
		rq->idle_stamp = 0;
		rq->avg_idle = 2*sysctl_sched_migration_cost;
		rq->wake_stamp = jiffies;
		rq->wake_avg_idle = rq->avg_idle;
		rq->max_idle_balance_cost = sysctl_sched_migration_cost;

		INIT_LIST_HEAD(&rq->cfs_tasks);

		rq_attach_root(rq, &def_root_domain);
#ifdef CONFIG_NO_HZ_COMMON
		rq->last_blocked_load_update_tick = jiffies;
		atomic_set(&rq->nohz_flags, 0);

		INIT_CSD(&rq->nohz_csd, nohz_csd_func, rq);
#endif
#ifdef CONFIG_HOTPLUG_CPU
		rcuwait_init(&rq->hotplug_wait);
#endif
#endif /* CONFIG_SMP */
		hrtick_rq_init(rq);
		atomic_set(&rq->nr_iowait, 0);

#ifdef CONFIG_SCHED_CORE
		rq->core = rq;
		rq->core_pick = NULL;
		rq->core_enabled = 0;
		rq->core_tree = RB_ROOT;
		rq->core_forceidle_count = 0;
		rq->core_forceidle_occupation = 0;
		rq->core_forceidle_start = 0;

		rq->core_cookie = 0UL;
#endif
	}

	set_load_weight(&init_task, false);

	/*
	 * The boot idle thread does lazy MMU switching as well:
	 */
	mmgrab(&init_mm);
	enter_lazy_tlb(&init_mm, current);

	/*
	 * The idle task doesn't need the kthread struct to function, but it
	 * is dressed up as a per-CPU kthread and thus needs to play the part
	 * if we want to avoid special-casing it in code that deals with per-CPU
	 * kthreads.
	 */
	WARN_ON(!set_kthread_struct(current));

	/*
	 * Make us the idle thread. Technically, schedule() should not be
	 * called from this thread, however somewhere below it might be,
	 * but because we are the idle thread, we just pick up running again
	 * when this runqueue becomes "idle".
	 */
	init_idle(current, smp_processor_id());

	calc_load_update = jiffies + LOAD_FREQ;

#ifdef CONFIG_SMP
	idle_thread_set_boot_cpu();
	balance_push_set(smp_processor_id(), false);
#endif
	init_sched_fair_class();

	psi_init();

	init_uclamp();

	preempt_dynamic_init();

	scheduler_running = 1;
}

#ifdef CONFIG_DEBUG_ATOMIC_SLEEP

void __might_sleep(const char *file, int line)
{
	unsigned int state = get_current_state();
	/*
	 * Blocking primitives will set (and therefore destroy) current->state,
	 * since we will exit with TASK_RUNNING make sure we enter with it,
	 * otherwise we will destroy state.
	 */
	WARN_ONCE(state != TASK_RUNNING && current->task_state_change,
			"do not call blocking ops when !TASK_RUNNING; "
			"state=%x set at [<%p>] %pS\n", state,
			(void *)current->task_state_change,
			(void *)current->task_state_change);

	__might_resched(file, line, 0);
}
EXPORT_SYMBOL(__might_sleep);

static void print_preempt_disable_ip(int preempt_offset, unsigned long ip)
{
	if (!IS_ENABLED(CONFIG_DEBUG_PREEMPT))
		return;

	if (preempt_count() == preempt_offset)
		return;

	pr_err("Preemption disabled at:");
	print_ip_sym(KERN_ERR, ip);
}

static inline bool resched_offsets_ok(unsigned int offsets)
{
	unsigned int nested = preempt_count();

	nested += rcu_preempt_depth() << MIGHT_RESCHED_RCU_SHIFT;

	return nested == offsets;
}

void __might_resched(const char *file, int line, unsigned int offsets)
{
	/* Ratelimiting timestamp: */
	static unsigned long prev_jiffy;

	unsigned long preempt_disable_ip;

	/* WARN_ON_ONCE() by default, no rate limit required: */
	rcu_sleep_check();

	if ((resched_offsets_ok(offsets) && !irqs_disabled() &&
	     !is_idle_task(current) && !current->non_block_count) ||
	    system_state == SYSTEM_BOOTING || system_state > SYSTEM_RUNNING ||
	    oops_in_progress)
		return;

	if (time_before(jiffies, prev_jiffy + HZ) && prev_jiffy)
		return;
	prev_jiffy = jiffies;

	/* Save this before calling printk(), since that will clobber it: */
	preempt_disable_ip = get_preempt_disable_ip(current);

	pr_err("BUG: sleeping function called from invalid context at %s:%d\n",
	       file, line);
	pr_err("in_atomic(): %d, irqs_disabled(): %d, non_block: %d, pid: %d, name: %s\n",
	       in_atomic(), irqs_disabled(), current->non_block_count,
	       current->pid, current->comm);
	pr_err("preempt_count: %x, expected: %x\n", preempt_count(),
	       offsets & MIGHT_RESCHED_PREEMPT_MASK);

	if (IS_ENABLED(CONFIG_PREEMPT_RCU)) {
		pr_err("RCU nest depth: %d, expected: %u\n",
		       rcu_preempt_depth(), offsets >> MIGHT_RESCHED_RCU_SHIFT);
	}

	if (task_stack_end_corrupted(current))
		pr_emerg("Thread overran stack, or stack corrupted\n");

	debug_show_held_locks(current);
	if (irqs_disabled())
		print_irqtrace_events(current);

	print_preempt_disable_ip(offsets & MIGHT_RESCHED_PREEMPT_MASK,
				 preempt_disable_ip);

	dump_stack();
	add_taint(TAINT_WARN, LOCKDEP_STILL_OK);
}
EXPORT_SYMBOL(__might_resched);

void __cant_sleep(const char *file, int line, int preempt_offset)
{
	static unsigned long prev_jiffy;

	if (irqs_disabled())
		return;

	if (!IS_ENABLED(CONFIG_PREEMPT_COUNT))
		return;

	if (preempt_count() > preempt_offset)
		return;

	if (time_before(jiffies, prev_jiffy + HZ) && prev_jiffy)
		return;
	prev_jiffy = jiffies;

	printk(KERN_ERR "BUG: assuming atomic context at %s:%d\n", file, line);
	printk(KERN_ERR "in_atomic(): %d, irqs_disabled(): %d, pid: %d, name: %s\n",
			in_atomic(), irqs_disabled(),
			current->pid, current->comm);

	debug_show_held_locks(current);
	dump_stack();
	add_taint(TAINT_WARN, LOCKDEP_STILL_OK);
}
EXPORT_SYMBOL_GPL(__cant_sleep);

#ifdef CONFIG_SMP
void __cant_migrate(const char *file, int line)
{
	static unsigned long prev_jiffy;

	if (irqs_disabled())
		return;

	if (is_migration_disabled(current))
		return;

	if (!IS_ENABLED(CONFIG_PREEMPT_COUNT))
		return;

	if (preempt_count() > 0)
		return;

	if (time_before(jiffies, prev_jiffy + HZ) && prev_jiffy)
		return;
	prev_jiffy = jiffies;

	pr_err("BUG: assuming non migratable context at %s:%d\n", file, line);
	pr_err("in_atomic(): %d, irqs_disabled(): %d, migration_disabled() %u pid: %d, name: %s\n",
	       in_atomic(), irqs_disabled(), is_migration_disabled(current),
	       current->pid, current->comm);

	debug_show_held_locks(current);
	dump_stack();
	add_taint(TAINT_WARN, LOCKDEP_STILL_OK);
}
EXPORT_SYMBOL_GPL(__cant_migrate);
#endif
#endif

#ifdef CONFIG_MAGIC_SYSRQ
void normalize_rt_tasks(void)
{
	struct task_struct *g, *p;
	struct sched_attr attr = {
		.sched_policy = SCHED_NORMAL,
	};

	read_lock(&tasklist_lock);
	for_each_process_thread(g, p) {
		/*
		 * Only normalize user tasks:
		 */
		if (p->flags & PF_KTHREAD)
			continue;

		p->se.exec_start = 0;
		schedstat_set(p->stats.wait_start,  0);
		schedstat_set(p->stats.sleep_start, 0);
		schedstat_set(p->stats.block_start, 0);

		if (!dl_task(p) && !rt_task(p)) {
			/*
			 * Renice negative nice level userspace
			 * tasks back to 0:
			 */
			if (task_nice(p) < 0)
				set_user_nice(p, 0);
			continue;
		}

		__sched_setscheduler(p, &attr, false, false);
	}
	read_unlock(&tasklist_lock);
}

#endif /* CONFIG_MAGIC_SYSRQ */

#if defined(CONFIG_IA64) || defined(CONFIG_KGDB_KDB)
/*
 * These functions are only useful for the IA64 MCA handling, or kdb.
 *
 * They can only be called when the whole system has been
 * stopped - every CPU needs to be quiescent, and no scheduling
 * activity can take place. Using them for anything else would
 * be a serious bug, and as a result, they aren't even visible
 * under any other configuration.
 */

/**
 * curr_task - return the current task for a given CPU.
 * @cpu: the processor in question.
 *
 * ONLY VALID WHEN THE WHOLE SYSTEM IS STOPPED!
 *
 * Return: The current task for @cpu.
 */
struct task_struct *curr_task(int cpu)
{
	return cpu_curr(cpu);
}

#endif /* defined(CONFIG_IA64) || defined(CONFIG_KGDB_KDB) */

#ifdef CONFIG_IA64
/**
 * ia64_set_curr_task - set the current task for a given CPU.
 * @cpu: the processor in question.
 * @p: the task pointer to set.
 *
 * Description: This function must only be used when non-maskable interrupts
 * are serviced on a separate stack. It allows the architecture to switch the
 * notion of the current task on a CPU in a non-blocking manner. This function
 * must be called with all CPU's synchronized, and interrupts disabled, the
 * and caller must save the original value of the current task (see
 * curr_task() above) and restore that value before reenabling interrupts and
 * re-starting the system.
 *
 * ONLY VALID WHEN THE WHOLE SYSTEM IS STOPPED!
 */
void ia64_set_curr_task(int cpu, struct task_struct *p)
{
	cpu_curr(cpu) = p;
}

#endif

#ifdef CONFIG_CGROUP_SCHED
/* task_group_lock serializes the addition/removal of task groups */
static DEFINE_SPINLOCK(task_group_lock);

static inline void alloc_uclamp_sched_group(struct task_group *tg,
					    struct task_group *parent)
{
#ifdef CONFIG_UCLAMP_TASK_GROUP
	enum uclamp_id clamp_id;

	for_each_clamp_id(clamp_id) {
		uclamp_se_set(&tg->uclamp_req[clamp_id],
			      uclamp_none(clamp_id), false);
		tg->uclamp[clamp_id] = parent->uclamp[clamp_id];
	}
#endif
}

static void sched_free_group(struct task_group *tg)
{
	free_fair_sched_group(tg);
	free_rt_sched_group(tg);
	autogroup_free(tg);
	kmem_cache_free(task_group_cache, tg);
}

static void sched_free_group_rcu(struct rcu_head *rcu)
{
	sched_free_group(container_of(rcu, struct task_group, rcu));
}

static void sched_unregister_group(struct task_group *tg)
{
	unregister_fair_sched_group(tg);
	unregister_rt_sched_group(tg);
	/*
	 * We have to wait for yet another RCU grace period to expire, as
	 * print_cfs_stats() might run concurrently.
	 */
	call_rcu(&tg->rcu, sched_free_group_rcu);
}

/* allocate runqueue etc for a new task group */
struct task_group *sched_create_group(struct task_group *parent)
{
	struct task_group *tg;

	tg = kmem_cache_alloc(task_group_cache, GFP_KERNEL | __GFP_ZERO);
	if (!tg)
		return ERR_PTR(-ENOMEM);

	if (!alloc_fair_sched_group(tg, parent))
		goto err;

	if (!alloc_rt_sched_group(tg, parent))
		goto err;

	alloc_uclamp_sched_group(tg, parent);

	return tg;

err:
	sched_free_group(tg);
	return ERR_PTR(-ENOMEM);
}

void sched_online_group(struct task_group *tg, struct task_group *parent)
{
	unsigned long flags;

	spin_lock_irqsave(&task_group_lock, flags);
	list_add_rcu(&tg->list, &task_groups);

	/* Root should already exist: */
	WARN_ON(!parent);

	tg->parent = parent;
	INIT_LIST_HEAD(&tg->children);
	list_add_rcu(&tg->siblings, &parent->children);
	spin_unlock_irqrestore(&task_group_lock, flags);

	online_fair_sched_group(tg);
}

/* rcu callback to free various structures associated with a task group */
static void sched_unregister_group_rcu(struct rcu_head *rhp)
{
	/* Now it should be safe to free those cfs_rqs: */
	sched_unregister_group(container_of(rhp, struct task_group, rcu));
}

void sched_destroy_group(struct task_group *tg)
{
	/* Wait for possible concurrent references to cfs_rqs complete: */
	call_rcu(&tg->rcu, sched_unregister_group_rcu);
}

void sched_release_group(struct task_group *tg)
{
	unsigned long flags;

	/*
	 * Unlink first, to avoid walk_tg_tree_from() from finding us (via
	 * sched_cfs_period_timer()).
	 *
	 * For this to be effective, we have to wait for all pending users of
	 * this task group to leave their RCU critical section to ensure no new
	 * user will see our dying task group any more. Specifically ensure
	 * that tg_unthrottle_up() won't add decayed cfs_rq's to it.
	 *
	 * We therefore defer calling unregister_fair_sched_group() to
	 * sched_unregister_group() which is guarantied to get called only after the
	 * current RCU grace period has expired.
	 */
	spin_lock_irqsave(&task_group_lock, flags);
	list_del_rcu(&tg->list);
	list_del_rcu(&tg->siblings);
	spin_unlock_irqrestore(&task_group_lock, flags);
}

static void sched_change_group(struct task_struct *tsk, int type)
{
	struct task_group *tg;

	/*
	 * All callers are synchronized by task_rq_lock(); we do not use RCU
	 * which is pointless here. Thus, we pass "true" to task_css_check()
	 * to prevent lockdep warnings.
	 */
	tg = container_of(task_css_check(tsk, cpu_cgrp_id, true),
			  struct task_group, css);
	tg = autogroup_task_group(tsk, tg);
	tsk->sched_task_group = tg;

#ifdef CONFIG_FAIR_GROUP_SCHED
	if (tsk->sched_class->task_change_group)
		tsk->sched_class->task_change_group(tsk, type);
	else
#endif
		set_task_rq(tsk, task_cpu(tsk));
}

/*
 * Change task's runqueue when it moves between groups.
 *
 * The caller of this function should have put the task in its new group by
 * now. This function just updates tsk->se.cfs_rq and tsk->se.parent to reflect
 * its new group.
 */
void sched_move_task(struct task_struct *tsk)
{
	int queued, running, queue_flags =
		DEQUEUE_SAVE | DEQUEUE_MOVE | DEQUEUE_NOCLOCK;
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(tsk, &rf);
	update_rq_clock(rq);

	running = task_current(rq, tsk);
	queued = task_on_rq_queued(tsk);

	if (queued)
		dequeue_task(rq, tsk, queue_flags);
	if (running)
		put_prev_task(rq, tsk);

	sched_change_group(tsk, TASK_MOVE_GROUP);

	if (queued)
		enqueue_task(rq, tsk, queue_flags);
	if (running) {
		set_next_task(rq, tsk);
		/*
		 * After changing group, the running task may have joined a
		 * throttled one but it's still the running task. Trigger a
		 * resched to make sure that task can still run.
		 */
		resched_curr(rq);
	}

	task_rq_unlock(rq, tsk, &rf);
}

static inline struct task_group *css_tg(struct cgroup_subsys_state *css)
{
	return css ? container_of(css, struct task_group, css) : NULL;
}

static struct cgroup_subsys_state *
cpu_cgroup_css_alloc(struct cgroup_subsys_state *parent_css)
{
	struct task_group *parent = css_tg(parent_css);
	struct task_group *tg;

	if (!parent) {
		/* This is early initialization for the top cgroup */
		return &root_task_group.css;
	}

	tg = sched_create_group(parent);
	if (IS_ERR(tg))
		return ERR_PTR(-ENOMEM);

	return &tg->css;
}

/* Expose task group only after completing cgroup initialization */
static int cpu_cgroup_css_online(struct cgroup_subsys_state *css)
{
	struct task_group *tg = css_tg(css);
	struct task_group *parent = css_tg(css->parent);

	if (parent)
		sched_online_group(tg, parent);

#ifdef CONFIG_UCLAMP_TASK_GROUP
	/* Propagate the effective uclamp value for the new group */
	mutex_lock(&uclamp_mutex);
	rcu_read_lock();
	cpu_util_update_eff(css);
	rcu_read_unlock();
	mutex_unlock(&uclamp_mutex);
#endif

	return 0;
}

static void cpu_cgroup_css_released(struct cgroup_subsys_state *css)
{
	struct task_group *tg = css_tg(css);

	sched_release_group(tg);
}

static void cpu_cgroup_css_free(struct cgroup_subsys_state *css)
{
	struct task_group *tg = css_tg(css);

	/*
	 * Relies on the RCU grace period between css_released() and this.
	 */
	sched_unregister_group(tg);
}

/*
 * This is called before wake_up_new_task(), therefore we really only
 * have to set its group bits, all the other stuff does not apply.
 */
static void cpu_cgroup_fork(struct task_struct *task)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(task, &rf);

	update_rq_clock(rq);
	sched_change_group(task, TASK_SET_GROUP);

	task_rq_unlock(rq, task, &rf);
}

static int cpu_cgroup_can_attach(struct cgroup_taskset *tset)
{
	struct task_struct *task;
	struct cgroup_subsys_state *css;
	int ret = 0;

	cgroup_taskset_for_each(task, css, tset) {
#ifdef CONFIG_RT_GROUP_SCHED
		if (!sched_rt_can_attach(css_tg(css), task))
			return -EINVAL;
#endif
		/*
		 * Serialize against wake_up_new_task() such that if it's
		 * running, we're sure to observe its full state.
		 */
		raw_spin_lock_irq(&task->pi_lock);
		/*
		 * Avoid calling sched_move_task() before wake_up_new_task()
		 * has happened. This would lead to problems with PELT, due to
		 * move wanting to detach+attach while we're not attached yet.
		 */
		if (READ_ONCE(task->__state) == TASK_NEW)
			ret = -EINVAL;
		raw_spin_unlock_irq(&task->pi_lock);

		if (ret)
			break;
	}
	return ret;
}

static void cpu_cgroup_attach(struct cgroup_taskset *tset)
{
	struct task_struct *task;
	struct cgroup_subsys_state *css;

	cgroup_taskset_for_each(task, css, tset)
		sched_move_task(task);
}

#ifdef CONFIG_UCLAMP_TASK_GROUP
static void cpu_util_update_eff(struct cgroup_subsys_state *css)
{
	struct cgroup_subsys_state *top_css = css;
	struct uclamp_se *uc_parent = NULL;
	struct uclamp_se *uc_se = NULL;
	unsigned int eff[UCLAMP_CNT];
	enum uclamp_id clamp_id;
	unsigned int clamps;

	lockdep_assert_held(&uclamp_mutex);
	SCHED_WARN_ON(!rcu_read_lock_held());

	css_for_each_descendant_pre(css, top_css) {
		uc_parent = css_tg(css)->parent
			? css_tg(css)->parent->uclamp : NULL;

		for_each_clamp_id(clamp_id) {
			/* Assume effective clamps matches requested clamps */
			eff[clamp_id] = css_tg(css)->uclamp_req[clamp_id].value;
			/* Cap effective clamps with parent's effective clamps */
			if (uc_parent &&
			    eff[clamp_id] > uc_parent[clamp_id].value) {
				eff[clamp_id] = uc_parent[clamp_id].value;
			}
		}
		/* Ensure protection is always capped by limit */
		eff[UCLAMP_MIN] = min(eff[UCLAMP_MIN], eff[UCLAMP_MAX]);

		/* Propagate most restrictive effective clamps */
		clamps = 0x0;
		uc_se = css_tg(css)->uclamp;
		for_each_clamp_id(clamp_id) {
			if (eff[clamp_id] == uc_se[clamp_id].value)
				continue;
			uc_se[clamp_id].value = eff[clamp_id];
			uc_se[clamp_id].bucket_id = uclamp_bucket_id(eff[clamp_id]);
			clamps |= (0x1 << clamp_id);
		}
		if (!clamps) {
			css = css_rightmost_descendant(css);
			continue;
		}

		/* Immediately update descendants RUNNABLE tasks */
		uclamp_update_active_tasks(css);
	}
}

/*
 * Integer 10^N with a given N exponent by casting to integer the literal "1eN"
 * C expression. Since there is no way to convert a macro argument (N) into a
 * character constant, use two levels of macros.
 */
#define _POW10(exp) ((unsigned int)1e##exp)
#define POW10(exp) _POW10(exp)

struct uclamp_request {
#define UCLAMP_PERCENT_SHIFT	2
#define UCLAMP_PERCENT_SCALE	(100 * POW10(UCLAMP_PERCENT_SHIFT))
	s64 percent;
	u64 util;
	int ret;
};

static inline struct uclamp_request
capacity_from_percent(char *buf)
{
	struct uclamp_request req = {
		.percent = UCLAMP_PERCENT_SCALE,
		.util = SCHED_CAPACITY_SCALE,
		.ret = 0,
	};

	buf = strim(buf);
	if (strcmp(buf, "max")) {
		req.ret = cgroup_parse_float(buf, UCLAMP_PERCENT_SHIFT,
					     &req.percent);
		if (req.ret)
			return req;
		if ((u64)req.percent > UCLAMP_PERCENT_SCALE) {
			req.ret = -ERANGE;
			return req;
		}

		req.util = req.percent << SCHED_CAPACITY_SHIFT;
		req.util = DIV_ROUND_CLOSEST_ULL(req.util, UCLAMP_PERCENT_SCALE);
	}

	return req;
}

static ssize_t cpu_uclamp_write(struct kernfs_open_file *of, char *buf,
				size_t nbytes, loff_t off,
				enum uclamp_id clamp_id)
{
	struct uclamp_request req;
	struct task_group *tg;

	req = capacity_from_percent(buf);
	if (req.ret)
		return req.ret;

	static_branch_enable(&sched_uclamp_used);

	mutex_lock(&uclamp_mutex);
	rcu_read_lock();

	tg = css_tg(of_css(of));
	if (tg->uclamp_req[clamp_id].value != req.util)
		uclamp_se_set(&tg->uclamp_req[clamp_id], req.util, false);

	/*
	 * Because of not recoverable conversion rounding we keep track of the
	 * exact requested value
	 */
	tg->uclamp_pct[clamp_id] = req.percent;

	/* Update effective clamps to track the most restrictive value */
	cpu_util_update_eff(of_css(of));

	rcu_read_unlock();
	mutex_unlock(&uclamp_mutex);

	return nbytes;
}

static ssize_t cpu_uclamp_min_write(struct kernfs_open_file *of,
				    char *buf, size_t nbytes,
				    loff_t off)
{
	return cpu_uclamp_write(of, buf, nbytes, off, UCLAMP_MIN);
}

static ssize_t cpu_uclamp_max_write(struct kernfs_open_file *of,
				    char *buf, size_t nbytes,
				    loff_t off)
{
	return cpu_uclamp_write(of, buf, nbytes, off, UCLAMP_MAX);
}

static inline void cpu_uclamp_print(struct seq_file *sf,
				    enum uclamp_id clamp_id)
{
	struct task_group *tg;
	u64 util_clamp;
	u64 percent;
	u32 rem;

	rcu_read_lock();
	tg = css_tg(seq_css(sf));
	util_clamp = tg->uclamp_req[clamp_id].value;
	rcu_read_unlock();

	if (util_clamp == SCHED_CAPACITY_SCALE) {
		seq_puts(sf, "max\n");
		return;
	}

	percent = tg->uclamp_pct[clamp_id];
	percent = div_u64_rem(percent, POW10(UCLAMP_PERCENT_SHIFT), &rem);
	seq_printf(sf, "%llu.%0*u\n", percent, UCLAMP_PERCENT_SHIFT, rem);
}

static int cpu_uclamp_min_show(struct seq_file *sf, void *v)
{
	cpu_uclamp_print(sf, UCLAMP_MIN);
	return 0;
}

static int cpu_uclamp_max_show(struct seq_file *sf, void *v)
{
	cpu_uclamp_print(sf, UCLAMP_MAX);
	return 0;
}
#endif /* CONFIG_UCLAMP_TASK_GROUP */

#ifdef CONFIG_FAIR_GROUP_SCHED
static int cpu_shares_write_u64(struct cgroup_subsys_state *css,
				struct cftype *cftype, u64 shareval)
{
	if (shareval > scale_load_down(ULONG_MAX))
		shareval = MAX_SHARES;
	return sched_group_set_shares(css_tg(css), scale_load(shareval));
}

static u64 cpu_shares_read_u64(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	struct task_group *tg = css_tg(css);

	return (u64) scale_load_down(tg->shares);
}

#ifdef CONFIG_CFS_BANDWIDTH
static DEFINE_MUTEX(cfs_constraints_mutex);

const u64 max_cfs_quota_period = 1 * NSEC_PER_SEC; /* 1s */
static const u64 min_cfs_quota_period = 1 * NSEC_PER_MSEC; /* 1ms */
/* More than 203 days if BW_SHIFT equals 20. */
static const u64 max_cfs_runtime = MAX_BW * NSEC_PER_USEC;

static int __cfs_schedulable(struct task_group *tg, u64 period, u64 runtime);

static int tg_set_cfs_bandwidth(struct task_group *tg, u64 period, u64 quota,
				u64 burst)
{
	int i, ret = 0, runtime_enabled, runtime_was_enabled;
	struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;

	if (tg == &root_task_group)
		return -EINVAL;

	/*
	 * Ensure we have at some amount of bandwidth every period.  This is
	 * to prevent reaching a state of large arrears when throttled via
	 * entity_tick() resulting in prolonged exit starvation.
	 */
	if (quota < min_cfs_quota_period || period < min_cfs_quota_period)
		return -EINVAL;

	/*
	 * Likewise, bound things on the other side by preventing insane quota
	 * periods.  This also allows us to normalize in computing quota
	 * feasibility.
	 */
	if (period > max_cfs_quota_period)
		return -EINVAL;

	/*
	 * Bound quota to defend quota against overflow during bandwidth shift.
	 */
	if (quota != RUNTIME_INF && quota > max_cfs_runtime)
		return -EINVAL;

	if (quota != RUNTIME_INF && (burst > quota ||
				     burst + quota > max_cfs_runtime))
		return -EINVAL;

	/*
	 * Prevent race between setting of cfs_rq->runtime_enabled and
	 * unthrottle_offline_cfs_rqs().
	 */
	cpus_read_lock();
	mutex_lock(&cfs_constraints_mutex);
	ret = __cfs_schedulable(tg, period, quota);
	if (ret)
		goto out_unlock;

	runtime_enabled = quota != RUNTIME_INF;
	runtime_was_enabled = cfs_b->quota != RUNTIME_INF;
	/*
	 * If we need to toggle cfs_bandwidth_used, off->on must occur
	 * before making related changes, and on->off must occur afterwards
	 */
	if (runtime_enabled && !runtime_was_enabled)
		cfs_bandwidth_usage_inc();
	raw_spin_lock_irq(&cfs_b->lock);
	cfs_b->period = ns_to_ktime(period);
	cfs_b->quota = quota;
	cfs_b->burst = burst;

	__refill_cfs_bandwidth_runtime(cfs_b);

	/* Restart the period timer (if active) to handle new period expiry: */
	if (runtime_enabled)
		start_cfs_bandwidth(cfs_b);

	raw_spin_unlock_irq(&cfs_b->lock);

	for_each_online_cpu(i) {
		struct cfs_rq *cfs_rq = tg->cfs_rq[i];
		struct rq *rq = cfs_rq->rq;
		struct rq_flags rf;

		rq_lock_irq(rq, &rf);
		cfs_rq->runtime_enabled = runtime_enabled;
		cfs_rq->runtime_remaining = 0;

		if (cfs_rq->throttled)
			unthrottle_cfs_rq(cfs_rq);
		rq_unlock_irq(rq, &rf);
	}
	if (runtime_was_enabled && !runtime_enabled)
		cfs_bandwidth_usage_dec();
out_unlock:
	mutex_unlock(&cfs_constraints_mutex);
	cpus_read_unlock();

	return ret;
}

static int tg_set_cfs_quota(struct task_group *tg, long cfs_quota_us)
{
	u64 quota, period, burst;

	period = ktime_to_ns(tg->cfs_bandwidth.period);
	burst = tg->cfs_bandwidth.burst;
	if (cfs_quota_us < 0)
		quota = RUNTIME_INF;
	else if ((u64)cfs_quota_us <= U64_MAX / NSEC_PER_USEC)
		quota = (u64)cfs_quota_us * NSEC_PER_USEC;
	else
		return -EINVAL;

	return tg_set_cfs_bandwidth(tg, period, quota, burst);
}

static long tg_get_cfs_quota(struct task_group *tg)
{
	u64 quota_us;

	if (tg->cfs_bandwidth.quota == RUNTIME_INF)
		return -1;

	quota_us = tg->cfs_bandwidth.quota;
	do_div(quota_us, NSEC_PER_USEC);

	return quota_us;
}

static int tg_set_cfs_period(struct task_group *tg, long cfs_period_us)
{
	u64 quota, period, burst;

	if ((u64)cfs_period_us > U64_MAX / NSEC_PER_USEC)
		return -EINVAL;

	period = (u64)cfs_period_us * NSEC_PER_USEC;
	quota = tg->cfs_bandwidth.quota;
	burst = tg->cfs_bandwidth.burst;

	return tg_set_cfs_bandwidth(tg, period, quota, burst);
}

static long tg_get_cfs_period(struct task_group *tg)
{
	u64 cfs_period_us;

	cfs_period_us = ktime_to_ns(tg->cfs_bandwidth.period);
	do_div(cfs_period_us, NSEC_PER_USEC);

	return cfs_period_us;
}

static int tg_set_cfs_burst(struct task_group *tg, long cfs_burst_us)
{
	u64 quota, period, burst;

	if ((u64)cfs_burst_us > U64_MAX / NSEC_PER_USEC)
		return -EINVAL;

	burst = (u64)cfs_burst_us * NSEC_PER_USEC;
	period = ktime_to_ns(tg->cfs_bandwidth.period);
	quota = tg->cfs_bandwidth.quota;

	return tg_set_cfs_bandwidth(tg, period, quota, burst);
}

static long tg_get_cfs_burst(struct task_group *tg)
{
	u64 burst_us;

	burst_us = tg->cfs_bandwidth.burst;
	do_div(burst_us, NSEC_PER_USEC);

	return burst_us;
}

static s64 cpu_cfs_quota_read_s64(struct cgroup_subsys_state *css,
				  struct cftype *cft)
{
	return tg_get_cfs_quota(css_tg(css));
}

static int cpu_cfs_quota_write_s64(struct cgroup_subsys_state *css,
				   struct cftype *cftype, s64 cfs_quota_us)
{
	return tg_set_cfs_quota(css_tg(css), cfs_quota_us);
}

static u64 cpu_cfs_period_read_u64(struct cgroup_subsys_state *css,
				   struct cftype *cft)
{
	return tg_get_cfs_period(css_tg(css));
}

static int cpu_cfs_period_write_u64(struct cgroup_subsys_state *css,
				    struct cftype *cftype, u64 cfs_period_us)
{
	return tg_set_cfs_period(css_tg(css), cfs_period_us);
}

static u64 cpu_cfs_burst_read_u64(struct cgroup_subsys_state *css,
				  struct cftype *cft)
{
	return tg_get_cfs_burst(css_tg(css));
}

static int cpu_cfs_burst_write_u64(struct cgroup_subsys_state *css,
				   struct cftype *cftype, u64 cfs_burst_us)
{
	return tg_set_cfs_burst(css_tg(css), cfs_burst_us);
}

struct cfs_schedulable_data {
	struct task_group *tg;
	u64 period, quota;
};

/*
 * normalize group quota/period to be quota/max_period
 * note: units are usecs
 */
static u64 normalize_cfs_quota(struct task_group *tg,
			       struct cfs_schedulable_data *d)
{
	u64 quota, period;

	if (tg == d->tg) {
		period = d->period;
		quota = d->quota;
	} else {
		period = tg_get_cfs_period(tg);
		quota = tg_get_cfs_quota(tg);
	}

	/* note: these should typically be equivalent */
	if (quota == RUNTIME_INF || quota == -1)
		return RUNTIME_INF;

	return to_ratio(period, quota);
}

static int tg_cfs_schedulable_down(struct task_group *tg, void *data)
{
	struct cfs_schedulable_data *d = data;
	struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;
	s64 quota = 0, parent_quota = -1;

	if (!tg->parent) {
		quota = RUNTIME_INF;
	} else {
		struct cfs_bandwidth *parent_b = &tg->parent->cfs_bandwidth;

		quota = normalize_cfs_quota(tg, d);
		parent_quota = parent_b->hierarchical_quota;

		/*
		 * Ensure max(child_quota) <= parent_quota.  On cgroup2,
		 * always take the min.  On cgroup1, only inherit when no
		 * limit is set:
		 */
		if (cgroup_subsys_on_dfl(cpu_cgrp_subsys)) {
			quota = min(quota, parent_quota);
		} else {
			if (quota == RUNTIME_INF)
				quota = parent_quota;
			else if (parent_quota != RUNTIME_INF && quota > parent_quota)
				return -EINVAL;
		}
	}
	cfs_b->hierarchical_quota = quota;

	return 0;
}

static int __cfs_schedulable(struct task_group *tg, u64 period, u64 quota)
{
	int ret;
	struct cfs_schedulable_data data = {
		.tg = tg,
		.period = period,
		.quota = quota,
	};

	if (quota != RUNTIME_INF) {
		do_div(data.period, NSEC_PER_USEC);
		do_div(data.quota, NSEC_PER_USEC);
	}

	rcu_read_lock();
	ret = walk_tg_tree(tg_cfs_schedulable_down, tg_nop, &data);
	rcu_read_unlock();

	return ret;
}

static int cpu_cfs_stat_show(struct seq_file *sf, void *v)
{
	struct task_group *tg = css_tg(seq_css(sf));
	struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;

	seq_printf(sf, "nr_periods %d\n", cfs_b->nr_periods);
	seq_printf(sf, "nr_throttled %d\n", cfs_b->nr_throttled);
	seq_printf(sf, "throttled_time %llu\n", cfs_b->throttled_time);

	if (schedstat_enabled() && tg != &root_task_group) {
		struct sched_statistics *stats;
		u64 ws = 0;
		int i;

		for_each_possible_cpu(i) {
			stats = __schedstats_from_se(tg->se[i]);
			ws += schedstat_val(stats->wait_sum);
		}

		seq_printf(sf, "wait_sum %llu\n", ws);
	}

	seq_printf(sf, "nr_bursts %d\n", cfs_b->nr_burst);
	seq_printf(sf, "burst_time %llu\n", cfs_b->burst_time);

	return 0;
}
#endif /* CONFIG_CFS_BANDWIDTH */
#endif /* CONFIG_FAIR_GROUP_SCHED */

#ifdef CONFIG_RT_GROUP_SCHED
static int cpu_rt_runtime_write(struct cgroup_subsys_state *css,
				struct cftype *cft, s64 val)
{
	return sched_group_set_rt_runtime(css_tg(css), val);
}

static s64 cpu_rt_runtime_read(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	return sched_group_rt_runtime(css_tg(css));
}

static int cpu_rt_period_write_uint(struct cgroup_subsys_state *css,
				    struct cftype *cftype, u64 rt_period_us)
{
	return sched_group_set_rt_period(css_tg(css), rt_period_us);
}

static u64 cpu_rt_period_read_uint(struct cgroup_subsys_state *css,
