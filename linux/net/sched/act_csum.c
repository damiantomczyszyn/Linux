* use task_rq_lock() here and obtain the other rq->lock.
	 *
	 * Silence PROVE_RCU
	 */
	rcu_read_lock();
	__set_task_cpu(idle, cpu);
	rcu_read_unlock();

	rq->idle = idle;
	rcu_assign_pointer(rq->curr, idle);
	idle->on_rq = TASK_ON_RQ_QUEUED;
#ifdef CONFIG_SMP
	idle->on_cpu = 1;
#endif
	raw_spin_rq_unlock(rq);
	raw_spin_unlock_irqrestore(&idle->pi_lock, flags);

	/* Set the preempt count _outside_ the spinlocks! */
	init_idle_preempt_count(idle, cpu);

	/*
	 * The idle tasks have their own, simple scheduling class:
	 */
	idle->sched_class = &idle_sched_class;
	ftrace_graph_init_idle_task(idle, cpu);
	vtime_init_idle(idle, cpu);
#ifdef CONFIG_SMP
	sprintf(idle->comm, "%s/%d", INIT_TASK_COMM, cpu);
#endif
}

#ifdef CONFIG_SMP

int cpuset_cpumask_can_shrink(const struct cpumask *cur,
			      const struct cpumask *trial)
{
	int ret = 1;

	if (cpumask_empty(cur))
		return ret;

	ret = dl_cpuset_cpumask_can_shrink(cur, trial);

	return ret;
}

int task_can_attach(struct task_struct *p,
		    const struct cpumask *cs_cpus_allowed)
{
	int ret = 0;

	/*
	 * Kthreads which disallow setaffinity shouldn't be moved
	 * to a new cpuset; we don't want to change their CPU
	 * affinity and isolating such threads by their set of
	 * allowed nodes is unnecessary.  Thus, cpusets are not
	 * applicable for such threads.  This prevents checking for
	 * success of set_cpus_allowed_ptr() on all attached tasks
	 * before cpus_mask may be changed.
	 */
	if (p->flags & PF_NO_SETAFFINITY) {
		ret = -EINVAL;
		goto out;
	}

	if (dl_task(p) && !cpumask_intersects(task_rq(p)->rd->span,
					      cs_cpus_allowed)) {
		int cpu = cpumask_any_and(cpu_active_mask, cs_cpus_allowed);

		ret = dl_cpu_busy(cpu, p);
	}

out:
	return ret;
}

bool sched_smp_initialized __read_mostly;

#ifdef CONFIG_NUMA_BALANCING
/* Migrate current task p to target_cpu */
int migrate_task_to(struct task_struct *p, int target_cpu)
{
	struct migration_arg arg = { p, target_cpu };
	int curr_cpu = task_cpu(p);

	if (curr_cpu == target_cpu)
		return 0;

	if (!cpumask_test_cpu(target_cpu, p->cpus_ptr))
		return -EINVAL;

	/* TODO: This is not properly updating schedstats */

	trace_sched_move_numa(p, curr_cpu, target_cpu);
	return stop_one_cpu(curr_cpu, migration_cpu_stop, &arg);
}

/*
 * Requeue a task on a given node and accurately track the number of NUMA
 * tasks on the runqueues
 */
void sched_setnuma(struct task_struct *p, int nid)
{
	bool queued, running;
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(p, &rf);
	queued = task_on_rq_queued(p);
	running = task_current(rq, p);

	if (queued)
		dequeue_task(rq, p, DEQUEUE_SAVE);
	if (running)
		put_prev_task(rq, p);

	p->numa_preferred_nid = nid;

	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE | ENQUEUE_NOCLOCK);
	if (running)
		set_next_task(rq, p);
	task_rq_unlock(rq, p, &rf);
}
#endif /* CONFIG_NUMA_BALANCING */

#ifdef CONFIG_HOTPLUG_CPU
/*
 * Ensure that the idle task is using init_mm right before its CPU goes
 * offline.
 */
void idle_task_exit(void)
{
	struct mm_struct *mm = current->active_mm;

	BUG_ON(cpu_online(smp_processor_id()));
	BUG_ON(current != this_rq()->idle);

	if (mm != &init_mm) {
		switch_mm(mm, &init_mm, current);
		finish_arch_post_lock_switch();
	}

	/* finish_cpu(), as ran on the BP, will clean up the active_mm state */
}

static int __balance_push_cpu_stop(void *arg)
{
	struct task_struct *p = arg;
	struct rq *rq = this_rq();
	struct rq_flags rf;
	int cpu;

	raw_spin_lock_irq(&p->pi_lock);
	rq_lock(rq, &rf);

	update_rq_clock(rq);

	if (task_rq(p) == rq && task_on_rq_queued(p)) {
		cpu = select_fallback_rq(rq->cpu, p);
		rq = __migrate_task(rq, &rf, p, cpu);
	}

	rq_unlock(rq, &rf);
	raw_spin_unlock_irq(&p->pi_lock);

	put_task_struct(p);

	return 0;
}

static DEFINE_PER_CPU(struct cpu_stop_work, push_work);

/*
 * Ensure we only run per-cpu kthreads once the CPU goes !active.
 *
 * This is enabled below SCHED_AP_ACTIVE; when !cpu_active(), but only
 * effective when the hotplug motion is down.
 */
static void balance_push(struct rq *rq)
{
	struct task_struct *push_task = rq->curr;

	lockdep_assert_rq_held(rq);

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
 * around partition_sched_domains().
 *
 * If we come here as part of a suspend/resume, don't touch cpusets because we
 * want to restore it back to its original state upon resume anyway.
 */
static void cpuset_cpu_active(void)
{
	if (cpuhp_tasks_frozen) {
		/*
		 * num_cpus_frozen tracks how many CPUs are involved in suspend
		 * resume sequence. As long as this is not the last online
		 * operation in the resume sequence, just build a single sched
		 * domain, ignoring cpusets.
		 */
		partition_sched_domains(1, NULL, NULL);
		if (--num_cpus_frozen)
			return;
		/*
		 * This is the last CPU online operation. So fall through and
		 * restore the original sched domains by considering the
		 * cpuset configurations.
		 */
		cpuset_force_rebuild();
	}
	cpuset_update_active_cpus();
}

static int cpuset_cpu_inactive(unsigned int cpu)
{
	if (!cpuhp_tasks_frozen) {
		int ret = dl_cpu_busy(cpu, NULL);

		if (ret)
			return ret;
		cpuset_update_active_cpus();
	} else {
		num_cpus_frozen++;
		partition_sched_domains(1, NULL, NULL);
	}
	return 0;
}

int sched_cpu_activate(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	/*
	 * Clear the balance_push callback and prepare to schedule
	 * regular tasks.
	 */
	balance_push_set(cpu, false);

#ifdef CONFIG_SCHED_SMT
	/*
	 * When going up, increment the number of cores with SMT present.
	 */
	if (cpumask_weight(cpu_smt_mask(cpu)) == 2)
		static_branch_inc_cpuslocked(&sched_smt_present);
#endif
	set_cpu_active(cpu, true);

	if (sched_smp_initialized) {
		sched_update_numa(cpu, true);
		sched_domains_numa_masks_set(cpu);
		cpuset_cpu_active();
	}

	/*
	 * Put the rq online, if not already. This happens:
	 *
	 * 1) In the early boot process, because we build the real domains
	 *    after all CPUs have been brought up.
	 *
	 * 2) At runtime, if cpuset_cpu_active() fails to rebuild the
	 *    domains.
	 */
	rq_lock_irqsave(rq, &rf);
	if (rq->rd) {
		BUG_ON(!cpumask_test_cpu(cpu, rq->rd->span));
		set_rq_online(rq);
	}
	rq_unlock_irqrestore(rq, &rf);

	return 0;
}

int sched_cpu_deactivate(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;
	int ret;

	/*
	 * Remove CPU from nohz.idle_cpus_mask to prevent participating in
	 * load balancing when not active
	 */
	nohz_balance_exit_idle(rq);

	set_cpu_active(cpu, false);

	/*
	 * From this point forward, this CPU will refuse to run any task that
	 * is not: migrate_disable() or KTHREAD_IS_PER_CPU, and will actively
	 * push those tasks away until this gets cleared, see
	 * sched_cpu_dying().
	 */
	balance_push_set(cpu, true);

	/*
	 * We've cleared cpu_active_mask / set balance_push, wait for all
	 * preempt-disabled and RCU users of this state to go away such that
	 * all new such users will observe it.
	 *
	 * Specifically, we rely on ttwu to no longer target this CPU, see
	 * ttwu_queue_cond() and is_cpu_allowed().
	 *
	 * Do sync before park smpboot threads to take care the rcu boost case.
	 */
	synchronize_rcu();

	rq_lock_irqsave(rq, &rf);
	if (rq->rd) {
		update_rq_clock(rq);
		BUG_ON(!cpumask_test_cpu(cpu, rq->rd->span));
		set_rq_offline(rq);
	}
	rq_unlock_irqrestore(rq, &rf);

#ifdef CONFIG_SCHED_SMT
	/*
	 * When going down, decrement the number of cores with SMT present.
	 */
	if (cpumask_weight(cpu_smt_mask(cpu)) == 2)
		static_branch_dec_cpuslocked(&sched_smt_present);

	sched_core_cpu_deactivate(cpu);
#endif

	if (!sched_smp_initialized)
		return 0;

	sched_update_numa(cpu, false);
	ret = cpuset_cpu_inactive(cpu);
	if (ret) {
		balance_push_set(cpu, false);
		set_cpu_active(cpu, true);
		sched_update_numa(cpu, true);
		return ret;
	}
	sched_domains_numa_masks_clear(cpu);
	return 0;
}

static void sched_rq_cpu_starting(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	rq->calc_load_update = calc_load_update;
	update_max_interval();
}

int sched_cpu_starting(unsigned int cpu)
{
	sched_core_cpu_starting(cpu);
	sched_rq_cpu_starting(cpu);
	sched_tick_start(cpu);
	return 0;
}

#ifdef CONFIG_HOTPLUG_CPU

/*
 * Invoked immediately before the stopper thread is invoked to bring the
 * CPU down completely. At this point all per CPU kthreads except the
 * hotplug thread (current) and the stopper thread (inactive) have been
 * either parked or have been unbound from the outgoing CPU. Ensure that
 * any of those which might be on the way out are gone.
 *
 * If after this point a bound task is being woken on this CPU then the
 * responsible hotplug callback has failed to do it's job.
 * sched_cpu_dying() will catch it with the appropriate fireworks.
 */
int sched_cpu_wait_empty(unsigned int cpu)
{
	balance_hotplug_wait();
	return 0;
}

/*
 * Since this CPU is going 'away' for a while, fold any nr_active delta we
 * might have. Called from the CPU stopper task after ensuring that the
 * stopper is the last running task on the CPU, so nr_active count is
 * stable. We need to take the teardown thread which is calling this into
 * account, so we hand in adjust = 1 to the load calculation.
 *
 * Also see the comment "Global load-average calculations".
 */
static void calc_load_migrate(struct rq *rq)
{
	long delta = calc_load_fold_active(rq, 1);

	if (delta)
		atomic_long_add(delta, &calc_load_tasks);
}

static void dump_rq_tasks(struct rq *rq, const char *loglvl)
{
	struct task_struct *g, *p;
	int cpu = cpu_of(rq);

	lockdep_assert_rq_held(rq);

	printk("%sCPU%d enqueued tasks (%u total):\n", loglvl, cpu, rq->nr_running);
	for_each_process_thread(g, p) {
		if (task_cpu(p) != cpu)
			continue;

		if (!task_on_rq_queued(p))
			continue;

		printk("%s\tpid: %d, name: %s\n", loglvl, p->pid, p->comm);
	}
}

int sched_cpu_dying(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	/* Handle pending wakeups and then migrate everything off */
	sched_tick_stop(cpu);

	rq_lock_irqsave(rq, &rf);
	if (rq->nr_running != 1 || rq_has_pinned_tasks(rq)) {
		WARN(true, "Dying CPU not properly vacated!");
		dump_rq_tasks(rq, KERN_WARNING);
	}
	rq_unlock_irqrestore(rq, &rf);

	calc_load_migrate(rq);
	update_max_interval();
	hrtick_clear(rq);
	sched_core_cpu_dying(cpu);
	return 0;
}
#endif

void __init sched_init_smp(void)
{
	sched_init_numa(NUMA_NO_NODE);

	/*
	 * There's no userspace yet to cause hotplug operations; hence all the
	 * CPU masks are stable and all blatant races in the below code cannot
	 * happen.
	 */
	mutex_lock(&sched_domains_mutex);
	sched_init_domains(cpu_active_mask);
	mutex_unlock(&sched_domains_mutex);

	/* Move init over to a non-isolated CPU */
	if (set_cpus_allowed_ptr(current, housekeeping_cpumask(HK_TYPE_DOMAIN)) < 0)
		BUG();
	current->flags &= ~PF_NO_SETAFFINITY;
	sched_init_granularity();

	init_sched_rt_class();
	init_sched_dl_class();

	sched_smp_initialized = true;
}

static int __init migration_init(void)
{
	sched_cpu_starting(smp_processor_id());
	return 0;
}
early_initcall(migration_init);

#else
void __init sched_init_smp(void)
{
	sched_init_granularity();
}
#endif /* CONFIG_SMP */

int in_sched_functions(unsigned long addr)
{
	return in_lock_functions(addr) ||
		(addr >= (unsigned long)__sched_text_start
		&& addr < (unsigned long)__sched_text_end);
}

#ifdef CONFIG_CGROUP_SCHED
/*
 * Default task group.
 * Every task in system belongs to this group at bootup.
 */
struct task_group root_task_group;
LIST_HEAD(task_groups);

/* Cacheline aligned slab cache for task_group */
static struct kmem_cache *task_group_cache __read_mostly;
#endif

DECLARE_PER_CPU(cpumask_var_t, load_balance_mask);
DECLARE_PER_CPU(cpumask_var_t, select_idle_mask);

void __init sched_init(void)
{
	unsigned long ptr = 0;
	int i;

	/* Make sure the linker didn't screw up */
	BUG_ON(&idle_sched_class + 1 != &fair_sched_class ||
	       &fair_sched_class + 1 != &rt_sched_class ||
	       &rt_sched_class + 1   != &dl_sched_class);
#ifdef CONFIG_SMP
	BUG_ON(&dl_sched_class + 1 != &stop_sched_class);
#endif

	wait_bit_init();

#ifdef CONFIG_FAIR_GROUP_SCHED
	ptr += 2 * nr_cpu_ids * sizeof(void **);
#endif
#ifdef CONFIG_RT_GROUP_SCHED
	ptr += 2 * nr_cpu_ids * sizeof(void **);
#endif
	if (ptr) {
		ptr = (unsigned long)kzalloc(ptr, GFP_NOWAIT);

#ifdef CONFIG_FAIR_GROUP_SCHED
		root_task_group.se = (struct sched_entity **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.cfs_rq = (struct cfs_rq **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.shares = ROOT_TASK_GROUP_LOAD;
		init_cfs_bandwidth(&root_task_group.cfs_bandwidth);
#endif /* CONFIG_FAIR_GROUP_SCHED */
#ifdef CONFIG_RT_GROUP_SCHED
		root_task_group.rt_se = (struct sched_rt_entity **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.rt_rq = (struct rt_rq **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

#endif /* CONFIG_RT_GROUP_SCHED */
	}
#ifdef CONFIG_CPUMASK_OFFSTACK
	for_each_possible_cpu(i) {
		per_cpu(load_balance_mask, i) = (cpumask_var_t)kzalloc_node(
			cpumask_size(),3›·TŸq”zpªåDıömTIêr?âüÎ›ñ‚;·ïz¿ã—„yûçr¹”E,f7P$	l"Àx{¾¥È°íu®8î³¤KÔ^, ÷•;9ë˜rîQ‡8Ån¼‡”¬ıPp¾±Ç™’ëpƒ¨Ø„~_è/éSÇñõÆı<qøJP,Ğ'BŞ»º#İ…·2Êü]?h~÷IÂ›ÅLÇ(¶ã­‘’±,o›";É3,@6¹ŠÎÿkxÏƒ¡€Ä”.
h€lUdÒ#€ Š¡Â/P	PA#ƒäÖl½ô{şŒ‘=â¨ÃÓ(.ßğSi2*ë<„èÆüºZ`ù Tz€ò)¸-àdf1İrö ŠŠè³ìÃP”Š‰ÇycíGgå±¬-0ù??œ§¸—Áty<²ÙtÈ5!àÊc`¨ ¶±YÈÕo°Çı©œ9ƒ¶cE/HrnÑ{(vè¹H@¹€6›–©ùéc¸'„›r¤'¼¬^Ú»Ò*¯lL}ÖÑŸ&#ÆŞ7¯¨Rğ˜
M¼³|‡y¾Ÿ'ÆÓU;‰ˆLÓ*MÜóªœx¡æ"ÆnÆû+Naƒ3-İ»O
ÖÓ/ÙÒÊÿ
Èa
İW÷‘ì;ÁÅn(F^€›yæĞı»æ;O÷îNÁx1zqÔ¶Éƒ¿’dfnû¤™ÍÖó?ËšmrNŸ4spk¸ $HYß†<R¹¤nnrØğ¿\æä9ä2Sß¡"˜kâX\İ„óä>ˆn“«”6ƒ°ÈK2ç×”Ò(R¶î\Q2 iêéO>Ÿ¤µ2FiC;Š.{3¥ÀÒ!Ë(ğ4˜— c*h¾ÂJŠQß^*ÖÕq(¤bÀ,"pÆÜm¡©ôpë‚
M”<¾uï ©GIìFV¶&Q&»OÌá’ô$¹Å9D—Š.E­Éc†‚Ï¼úóğÌ4ŞzE&†	şùgàÖ¸ÔÀò,[Su÷Åğ^ñR±¹}J˜*S_ #Õ›ÀÆĞ³ã-©
¿cv?íŠ¸3ğ´5äRÎ‚@ÃÌ3æ#sœœ0+22±óà©)§Ûu#í;œ«•Çâ‚a(ÃÆqÀoÜA{) XN+7kV[ĞçÍãÎÔ—A‰¶2a!‰ÿj˜›¬‰¾VŸ+„À¹_YÅÅ¨)¾°d2ø¹Š^Ÿ´©árølTò4ÜÅvQp‡_çÕşv»hşYHÓ²®®ñEkáº=vrÇï’â!BSæL?ñâié@Ådë>L?ğxö“¬mÇõOúDÜêöB/'a å)­É§‚WÏ§Ø?w/Ñ‡R2ÀÉÑù¤S	ş‚áKLX­T8±ó|À‡°íU§º^F‘ÚLY¼O–¸áœúÓˆ­Ä‚oÎ5TUÇ¿İºÁ«Læa#Ó T˜±@½Ö#´Ëæ½<,Aõ+wyKçkß¡Yóæ5·¼p?œT—[ÿõÏİàÀ¶ÜÑOú7Qåã~â+„‰ƒ!%Í (yHŸS¥gÚ4ò˜ê¥1^œèMü:›İB¬é
F‚Lú$™ì‚Gt<¦BãÙZìªïn^¦¿8U~Æ[™¯Å'5
0ÓCs_ÙãUb¾’ÕÊÉ}KÖ…ŞË}Br+3õ2Ç—€p+…