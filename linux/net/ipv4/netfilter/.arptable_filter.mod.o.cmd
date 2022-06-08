 we were to improve the
 * storage performance, we'd have a proportional reduction in IO-wait time.
 *
 * This all works nicely on UP, where, when a task blocks on IO, we account
 * idle time as IO-wait, because if the storage were faster, it could've been
 * running and we'd not be idle.
 *
 * This has been extended to SMP, by doing the same for each CPU. This however
 * is broken.
 *
 * Imagine for instance the case where two tasks block on one CPU, only the one
 * CPU will have IO-wait accounted, while the other has regular idle. Even
 * though, if the storage were faster, both could've ran at the same time,
 * utilising both CPUs.
 *
 * This means, that when looking globally, the current IO-wait accounting on
 * SMP is a lower bound, by reason of under accounting.
 *
 * Worse, since the numbers are provided per CPU, they are sometimes
 * interpreted per CPU, and that is nonsensical. A blocked task isn't strictly
 * associated with any one particular CPU, it can wake to another CPU than it
 * blocked on. This means the per CPU IO-wait number is meaningless.
 *
 * Task CPU affinities can make all that even more 'interesting'.
 */

unsigned int nr_iowait(void)
{
	unsigned int i, sum = 0;

	for_each_possible_cpu(i)
		sum += nr_iowait_cpu(i);

	return sum;
}

#ifdef CONFIG_SMP

/*
 * sched_exec - execve() is a valuable balancing opportunity, because at
 * this point the task has the smallest effective memory and cache footprint.
 */
void sched_exec(void)
{
	struct task_struct *p = current;
	unsigned long flags;
	int dest_cpu;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	dest_cpu = p->sched_class->select_task_rq(p, task_cpu(p), WF_EXEC);
	if (dest_cpu == smp_processor_id())
		goto unlock;

	if (likely(cpu_active(dest_cpu))) {
		struct migration_arg arg = { p, dest_cpu };

		raw_spin_unlock_irqrestore(&p->pi_lock, flags);
		stop_one_cpu(task_cpu(p), migration_cpu_stop, &arg);
		return;
	}
unlock:
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
}

#endif

DEFINE_PER_CPU(struct kernel_stat, kstat);
DEFINE_PER_CPU(struct kernel_cpustat, kernel_cpustat);

EXPORT_PER_CPU_SYMBOL(kstat);
EXPORT_PER_CPU_SYMBOL(kernel_cpustat);

/*
 * The function fair_sched_class.update_curr accesses the struct curr
 * and its field curr->exec_start; when called from task_sched_runtime(),
 * we observe a high rate of cache misses in practice.
 * Prefetching this data results in improved performance.
 */
static inline void prefetch_curr_exec_start(struct task_struct *p)
{
#ifdef CONFIG_FAIR_GROUP_SCHED
	struct sched_entity *curr = (&p->se)->cfs_rq->curr;
#else
	struct sched_entity *curr = (&task_rq(p)->cfs)->curr;
#endif
	prefetch(curr);
	prefetch(&curr->exec_start);
}

/*
 * Return accounted runtime for the task.
 * In case the task is currently running, return the runtime plus current's
 * pending runtime that have not been accounted yet.
 */
unsigned long long task_sched_runtime(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;
	u64 ns;

#if defined(CONFIG_64BIT) && defined(CONFIG_SMP)
	/*
	 * 64-bit doesn't need locks to atomically read a 64-bit value.
	 * So we have a optimization chance when the task's delta_exec is 0.
	 * Reading ->on_cpu is racy, but this is ok.
	 *
	 * If we race with it leaving CPU, we'll take a lock. So we're correct.
	 * If we race with it entering CPU, unaccounted time is 0. This is
	 * indistinguishable from the read occurring a few cycles earlier.
	 * If we see ->on_cpu without ->on_rq, the task is leaving, and has
	 * been accounted, so we're correct here as well.
	 */
	if (!p->on_cpu || !task_on_rq_queued(p))
		return p->se.sum_exec_runtime;
#endif

	rq = task_rq_lock(p, &rf);
	/*
	 * Must be ->curr _and_ ->on_rq.  If dequeued, we would
	 * project cycles that may never be accounted to this
	 * thread, breaking clock_gettime().
	 */
	if (task_current(rq, p) && task_on_rq_queued(p)) {
		prefetch_curr_exec_start(p);
		update_rq_clock(rq);
		p->sched_class->update_curr(rq);
	}
	ns = p->se.sum_exec_runtime;
	task_rq_unlock(rq, p, &rf);

	return ns;
}

#ifdef CONFIG_SCHED_DEBUG
static u64 cpu_resched_latency(struct rq *rq)
{
	int latency_warn_ms = READ_ONCE(sysctl_resched_latency_warn_ms);
	u64 resched_latency, now = rq_clock(rq);
	static bool warned_once;

	if (sysctl_resched_latency_warn_once && warned_once)
		return 0;

	if (!need_resched() || !latency_warn_ms)
		return 0;

	if (system_state == SYSTEM_BOOTING)
		return 0;

	if (!rq->last_seen_need_resched_ns) {
		rq->last_seen_need_resched_ns = now;
		rq->ticks_without_resched = 0;
		return 0;
	}

	rq->ticks_without_resched++;
	resched_latency = now - rq->last_seen_need_resched_ns;
	if (resched_latency <= latency_warn_ms * NSEC_PER_MSEC)
		return 0;

	warned_once = true;

	return resched_latency;
}

static int __init setup_resched_latency_warn_ms(char *str)
{
	long val;

	if ((kstrtol(str, 0, &val))) {
		pr_warn("Unable to set resched_latency_warn_ms\n");
		return 1;
	}

	sysctl_resched_latency_warn_ms = val;
	return 1;
}
__setup("resched_latency_warn_ms=", setup_resched_latency_warn_ms);
#else
static inline u64 cpu_resched_latency(struct rq *rq) { return 0; }
#endif /* CONFIG_SCHED_DEBUG */

/*
 * This function gets called by the timer code, with HZ frequency.
 * We call it with interrupts disabled.
 */
void scheduler_tick(void)
{
	int cpu = smp_processor_id();
	struct rq *rq = cpu_rq(cpu);
	struct task_struct *curr = rq->curr;
	struct rq_flags rf;
	unsigned long thermal_pressure;
	u64 resched_latency;

	arch_scale_freq_tick();
	sched_clock_tick();

	rq_lock(rq, &rf);

	update_rq_clock(rq);
	thermal_pressure = arch_scale_thermal_pressure(cpu_of(rq));
	update_thermal_load_avg(rq_clock_thermal(rq), rq, thermal_pressure);
	curr->sched_class->task_tick(rq, curr, 0);
	if (sched_feat(LATENCY_WARN))
		resched_latency = cpu_resched_latency(rq);
	calc_global_load_tick(rq);
	sched_core_tick(rq);

	rq_unlock(rq, &rf);

	if (sched_feat(LATENCY_WARN) && resched_latency)
		resched_latency_warn(cpu, resched_latency);

	perf_event_task_tick();

#ifdef CONFIG_SMP
	rq->idle_balance = idle_cpu(cpu);
	trigger_load_balance(rq);
#endif
}

#ifdef CONFIG_NO_HZ_FULL

struct tick_work {
	int			cpu;
	atomic_t		state;
	struct delayed_work	work;
};
/* Values for ->state, see diagram below. */
#define TICK_SCHED_REMOTE_OFFLINE	0
#define TICK_SCHED_REMOTE_OFFLINING	1
#define TICK_SCHED_REMOTE_RUNNING	2

/*
 * State diagram for ->state:
 *
 *
 *          TICK_SCHED_REMOTE_OFFLINE
 *                    |   ^
 *                    |   |
 *                    |   | sched_tick_remote()
 *                    |   |
 *                    |   |
 *                    +--TICK_SCHED_REMOTE_OFFLINING
 *                    |   ^
 *                    |   |
 * sched_tick_start() |   | sched_tick_stop()
 *                    |   |
 *                    V   |
 *          TICK_SCHED_REMOTE_RUNNING
 *
 *
 * Other transitions get WARN_ON_ONCE(), except that sched_tick_remote()
 * and sched_tick_start() are happy to leave the state in RUNNING.
 */

static struct tick_work __percpu *tick_work_cpu;

static void sched_tick_remote(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct tick_work *twork = container_of(dwork, struct tick_work, work);
	int cpu = twork->cpu;
	struct rq *rq = cpu_rq(cpu);
	struct task_struct *curr;
	struct rq_flags rf;
	u64 delta;
	int os;

	/*
	 * Handle the tick only if it appears the remote CPU is running in full
	 * dynticks mode. The check is racy by nature, but missing a tick or
	 * having one too much is no big deal because the scheduler tick updates
	 * statistics and checks timeslices in a time-independent way, regardless
	 * of when exactly it is running.
	 */
	if (!tick_nohz_tick_stopped_cpu(cpu))
		goto out_requeue;

	rq_lock_irq(rq, &rf);
	curr = rq->curr;
	if (cpu_is_offline(cpu))
		goto out_unlock;

	update_rq_clock(rq);

	if (!is_idle_task(curr)) {
		/*
		 * Make sure the next tick runs within a reasonable
		 * amount of time.
		 */
		delta = rq_clock_task(rq) - curr->se.exec_start;
		WARN_ON_ONCE(delta > (u64)NSEC_PER_SEC * 3);
	}
	curr->sched_class->task_tick(rq, curr, 0);

	calc_load_nohz_remote(rq);
out_unlock:
	rq_unlock_irq(rq, &rf);
out_requeue:

	/*
	 * Run the remote tick once per second (1Hz). This arbitrary
	 * frequency is large enough to avoid overload but short enough
	 * to keep scheduler internal stats reasonably up to date.  But
	 * first update state to reflect hotplug activity if required.
	 */
	os = atomic_fetch_add_unless(&twork->state, -1, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_OFFLINE);
	if (os == TICK_SCHED_REMOTE_RUNNING)
		queue_delayed_work(system_unbound_wq, dwork, HZ);
}

static void sched_tick_start(int cpu)
{
	int os;
	struct tick_work *twork;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_RUNNING);
	if (os == TICK_SCHED_REMOTE_OFFLINE) {
		twork->cpu = cpu;
		INIT_DELAYED_WORK(&twork->work, sched_tick_remote);
		queue_delayed_work(system_unbound_wq, &twork->work, HZ);
	}
}

#ifdef CONFIG_HOTPLUG_CPU
static void sched_tick_stop(int cpu)
{
	struct tick_work *twork;
	int os;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	/* There cannot be competing actions, but don't rely on stop-machine. */
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_OFFLINING);
	WARN_ON_ONCE(os != TICK_SCHED_REMOTE_RUNNING);
	/* Don't cancel, as this would mess up the state machine. */
}
#endif /* CONFIG_HOTPLUG_CPU */

int __init sched_tick_offload_init(void)
{
	tick_work_cpu = alloc_percpu(struct tick_work);
	BUG_ON(!tick_work_cpu);
	return 0;
}

#else /* !CONFIG_NO_HZ_FULL */
static inline void sched_tick_start(int cpu) { }
static inline void sched_tick_stop(int cpu) { }
#endif

#if defined(CONFIG_PREEMPTION) && (defined(CONFIG_DEBUG_PREEMPT) || \
				defined(CONFIG_TRACE_PREEMPT_TOGGLE))
/*
 * If the value passed in is equal to the current preempt count
 * then we just disabled preemption. Start timing the latency.
 */
static inline void preempt_latency_start(int val)
{
	if (preempt_count() == val) {
		unsigned long ip = get_lock_parent_ip();
#ifdef CONFIG_DEBUG_PREEMPT
		current->preempt_disable_ip = ip;
#endif
		trace_preempt_off(CALLER_ADDR0, ip);
	}
}

void preempt_count_add(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON((preempt_count() < 0)))
		return;
#endif
	__preempt_count_add(val);
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Spinlock count overflowing soon?
	 */
	DEBUG_LOCKS_WARN_ON((preempt_count() & PREEMPT_MASK) >=
				PREEMPT_MASK - 10);
#endif
	preempt_latency_start(val);
}
EXPORT_SYMBOL(preempt_count_add);
NOKPROBE_SYMBOL(preempt_count_add);

/*
 * If the value passed in equals to the current preempt count
 * then we just enabled preemption. Stop timing the latency.
 */
static inline void preempt_latency_stop(int val)
{
	if (preempt_count() == val)
		trace_preempt_on(CALLER_ADDR0, get_lock_parent_ip());
}

void preempt_count_sub(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON(val > preempt_count()))
		return;
	/*
	 * Is the spinlock portion underflowing?
	 */
	if (DEBUG_LOCKS_WARN_ON((val < PREEMPT_MASK) &&
			!(preempt_count() & PREEMPT_MASK)))
		return;
#endif

	preempt_latency_stop(val);
	__preempt_count_sub(val);
}
EXPORT_SYMBOL(preempt_count_sub);
NOKPROBE_SYMBOL(preempt_count_sub);

#else
static inline void preempt_latency_start(int val) { }
static inline void preempt_latency_stop(int val) { }
#endif

static inline unsigned long get_preempt_disable_ip(struct task_struct *p)
{
#ifdef CONFIG_DEBUG_PREEMPT
	return p->preempt_disable_ip;
#else
	return 0;
#endif
}

/*
 * Print scheduling while atomic bug:
 */
static noinline void __schedule_bug(struct task_struct *prev)
{
	/* Save this before calling printk(), since that will clobber it */
	unsigned long preempt_disable_ip = get_preempt_disable_ip(current);

	if (oops_in_progress)
		return;

	printk(KERN_ERR "BUG: scheduling while atomic: %s/%d/0x%08x\n",
		prev->comm, prev->pid, preempt_count());

	debug_show_held_locks(prev);
	pri΍��mo(:铌�M��I�����r�@�X[�qN�Z��:���B���B���/KF^� �����8^�5]�#�K�F`��Cy~!;+���X��,�4��������6r;3��/Uh���%� 0���40t�)���A�>�J-�=��DT�xz�A�4!d:�(*�!1H���h���uVA���F�V��7	�Z�j��M��u�q(G����@����͡є�!{�Q&!`�B����6�봍EԋRf���#����@0��v9�~／��-�>�/S��[|,3r�ߞO#���eM�����@	ӭ����'�c24W��y�0����8C�
�X�84�����9�4��W���%L�SfY���2�L���ÿ�v���b� aU}���x��Bf����H���?�K< ����K}�O�n��NN�B�A�rt��w���y�T���uU�_]SМhq��]��%�C�O܃�O���XS2J�A�I��BU����w<�1��s�����)�r_�m�'�`(���n{��(z��z���@�#䢋��q�/y��&*˗�e�[�N��Q��ç0qO�a�UC[8�0��~��J���H��r�rgk4cRHfz.=w)���ٻiFV�1B}i<���W��Z>��ׄRe��v�WW�e����#A�2�%r���Y��2��ޮ*\h�nݟ����"�	응����b?����po�#z�Ĕ���c�?���B�e�����	��k��"���~Bkbi0�R-T�<5z<.�3?_|���[�BTa8,�
d#�����e5Ӱ[d,���N+�؟o|���"��q�X�c0��J�4<0���������[�S��C�3���J�sW����mܑN)gc7�i���<5��U�V[�:?�D�Y���q��X%����[�2���)����ǡ��{�'e��&��z;ʔ�QWO����ؤ��#�uΝ���W=�b���y�kb{�_l�{��T�38&�Q�!�*�aw��)*F2|]I;$�:6��ʨ�+@�z�g]�8���KcE���z�5��ι�dTVx� ��@1ۓ#�3�ΧѤ�%�(7Ս�@��<(f	�M��h�R*�&��*4��6����l�7�,��~=������w"��n��|%р,�&�|���e��g~$5�O$�ޤQ��i�>�E>C�
K� � ш;l�^3��=����:2�a�eky�*�籀 ��r�����tl��M��Q�r�E���~������� ��3}�h�h�L��C#�
R�P�srd��`!e.,*(:� ��dV(,� �dg�Ü��:�>��F^EꄟA+!�/x��7t[gܣ���#�k���0~&&v;���z�y/���"�)�G���L#5�M^�&�ј�L�����E�nw8��՜<�uoqA��L&����G�E����sZ��IgSؠ��X��)����I�΃��ԕ'W�>�lP��Ý&�9i�j.WD� R��*��δ��]X�1���T-���D�q<�|ST����;���2R�1�;.pa-�.�rh�t"&�k����/zo�Y�Uy�J�J��w��s#O�׉q�a'��HҢ8�iy��~��w:_�2��i~0U�a'� QZ����k�\����I�q�ި�Ir^gg�)�蹀��
�: M6�ѭ1�zD��7-���Z���U��b�;��˭{}��Q�*����d�
r�W��a�m���tYMu�T����`t�K���u������R[F�͉o���;s���V��з}5j~
oש�)�����:�\�E*J���L7I@���z˾�8e��-^�����?�JI��D@�E��_�TwU��s�Ȑ��������,���#��m�Dn��)Pb۠Mj�8bٰ�
�
�q�[�.�wX]��n�2+����[�¶'��C�kR���Ϡ;��Rb�.	����)u϶��P^��0�xG��r����m���.=h"ᬮ|�[�D�=s��\�\U�4l�;��?l߉���v� ����)�Rw��D���qf^���IѪ�Ą����7K^Y:�C�,D`�d�R�J`��}dG�������y����?��܃̂�sr���v%�������8�*�lT�0�z�\�*az�8�{�g��_��a���+M�Κ�/�Kn�3;��/X�Z���T�'u�/�Ԗ��5�f��~��פx��61���h����`�\��1*�M�ʠ|1�Q��r�Iz�Y��եڃ܊��5�֨�[O�t�e`v���b�X���1,t��8��r`lY�� Ú�F_�(ley��$u6%'��!B�v�a�S`����Z��68Λ���k���ඖg���TSVʢ.�$5�䝚����	�T���(�S	yrhp���$H��;(�,���i$[���NH�\� x�l��e�3�����.rK�b�����y���nɖ� ��ͪ���Q���@�̓�QL�S��_�ai����-�&�-�r�F�i	�Z�s<9�̉�<"�3ݘ��3Eb"���*d�A�ݡ�!�]h���˒)��x���E��������ܵ�{N>('�V�1��J�GJf75�ue��1�� �Q��O[����d1ޗ,����@rve甞�Rh���(Yۗҿ�̿�[����$C��Y���M����$��@L�m�l���}��W�Bw�m�|�b��BF�,V�{�2B#(�"-iՏ�_�W ¹��"���S�����6�ﲀ����Hf�^Y (�s%VD���2�q�y�1�h㛦��I�&���=,t���bXQ�h�[�r�y/���KwvEN��Px@{�����L��<������B/�V����MF�!C�.{�~�t¨�XR��^i�>���	ۙ�]#�����y �9��:Kr���E���JTN� Fq��_q y�����]�J���.��~�@,�f�96�6~D�ug�Z�Ⱥ���yb/e���$��?vh������d��+����T�ׯ����W/��^Y3�Gn��%[fA-�o�2|S�t5�#�/����~6g�ūs���j�h�����e%��I�V1�@��6;��������+kQY#ʧI"��>`k�B9_J��b[�3#E�|���pފ�G�wh��g��+�R��r�j��H%�+�v������6ժ�"xshs���B�����U�%g�f�`ӎU����p���o�<>?��C�����&��"%�l�$bxm_!�1�A�q&��Ǐ�ÑsT��#��qN]ۋ�g��74B�h��Q��?��d��c�Ŧ:n�X:h�H5��O30Xvt��B�^m�)a�^}�5wz6��}����8]vU�ʻZ=$m�
�Y���B̙��0���/��u��������OB7�Ԃ_'�p-gaƍ���w۞T-88�h��i���9�wDB��[Ѭ�*�^#�S�g�Җ��R�	$��H�S���\���A�$Ú��8�M��y���n�P�&,�/X(Q~%CB���B�<�������T7�l��L�2`�P�>��]�������K���$͇鄤�ɫ����Ki�z��ޡ���|�U9���raUc,���.\B�&�S�%�"Y��X	����I �dV�8�]�ab�~p>��i�?!�B����%���6�\��6�~���8��%�
w���V�m/���)uKt��S]X=�%�%�(�j���ΔP����$�M!	IEH�i���D���,
�X��6^��"+>��&"�۪* ]�#)z�����l*��M�!#D�ao0�!�¯�h��d�<�{>��^M�OfV�s4��nm�8��:��)�S.O`2Qޓ6��Bu����6�q� �o@�gM��q!b@q�9&���/�M��e��9X�`��[���N�/զ��1L�U�G=/��~�1 ��N=�sd��&��f�b�	��j�'���j���X�= z;�k�9#�W4��
���??�8	a�`��,�~�L���z&�U�f���tLȊ��V��>N�.|�
ὥ,�-��Y����w�ڸw��̱A�v�E�f�ڽ���w���+�����u�<.Û:�����y퀷�SO��\o����g���`E��h�@�͘�-�d}J�ʕ�1� ��-�Tm5�0"��>��PӤ�渡b�U����]��]W�O�/�`j,*�9�G�B�w�%~(�2�����(�/�b�h�F�����%�5B���;L���o��Ж[�u��	g��x�#��I�a ����06_������
��^�p�){�q,d�ED�=�6�5������D�Y�1��U,$���K��#������7��or��I��(���/?Ě�J�M˭	[	�0Ĵ��2PM٥6Q�� ʮ��=�h����J����\��D7���w�Ӹ�f�p��Q�:�h�8�Cz�+���@���x��j@|�m����xlCY�-cp-�ƌl��?=���6��rM�KK�*����	���v"���;���������g�A�t���۽v�Q�=��� ėa����z�Y�.dG{e>IcrO�%�4�2���5�s�%hx��n�!�m��̀�ӈ-Y���q��	�em���h�ɫi�w�]�����#%�և��8�P �JD�xF,>�;���� �q���4��J���[�|i�ɼC��+u�·D�*U��W|.�<���S��ʗ���{�כ����z��-����H]���4��d����P�pw�����V/��B�īIV�# ��=j��8$Fv�y�;%�����xY,�^ꮰ��|��j�B'OvǞ�ݬj? �%���{��J�i�J῀���AW��O�ՠ���Sp+�"�;�9M�8�շ*M�Z���bo����L�ĸ,n�$�2��њ(yU:�u#2�*K�h:�zTVG��P|gg>�)��Wi>��S8UN�8�����)��<�f$��Y�,�@G��vo.�O��N�o����[��x��k<R`���
�t�|ߌ2�={��+��'��FZ��`��uz7���`���y��{��E!4d���l�� Db�.*C�����'�{v��t���:�%K�m�,D�/v�>��b�Ð �V��~���Ჯ�>�e��v�w�.q�]�̢��!-�͐�U ~
�+(�2p��\Y��U���y����[����[����Gs2��-e��_�����j���0U�ƀ΄���,��С=>�����m/�=߼�n�v� F��^Bsn�ٱ��%I⡙�.vf*P1ڬ�LOҁ���a��A�>̤CAO!�D�[����+��C�&�<7�@�Z���K�7��2���BmSD�Ctx?,�CÃٱ
��oZ�����i�X�<2ҡ�d�{��E����]�Й�%��u���antp�M
Jc�k&�A�Q_ʇ��D�O�G��)�d����g��,��'m�O�������\��y8Q��5��SD�<�p(��?�ݗ�F�{�~��{H/FL�O�Y���<���u{��^�u���s���4w���fE_�f���˻VC�/r�_��W��H?Mv"�a��$�aC%�)���"~����Y�����7)�n;$M���m�*A�p�Xȃ�/Hy����N2<�r�1�s���^<E�>����3�s�v�3������Q���	g-Km*.��Lh�Iif#� :����c܌!���ݥ�S����M�lg�e�EӤ��+�-�=A�iW
���iI����&��*#kQm�`?[�'��S���:˚=����׹i�G����נ2�Z�uK-��lھPj#]G}ҹ2�Y}D)D�'�ܕ�Aq�˳�C�F��!uij
l�x}��6;./��G��zlSU����b�@Ź�1v�C/��Hq�_n��܇���45S3+���P�#�mԭ(s�Hhs�UU�nD�C�����B�f��mx��+�m 3���J��(����\Q�p�vǲ�9A�c����u��C6[�]��b�s��_=��籧��[+�ܷu�ߐsu��R�x�Z>���� 8��3��J� +�˒0�Un��l�<�X��Fo�cA�\�wç-9�r��h(��MA
T��^$2"�`!�ᗷ��h��y�2ȕ����g2��e6T�J��ɇ��H�Wo����,�u�5a�� ��'����9=E;ܒ�H�r;J��vl�h���h�r�����wQm	C�k��W!��:)�I|Yʟh��1s>�|����U�6�ri�оeC�H�tﮃ����#�F�/C�+nQ,֦̅�w!�u��W�!��,�I�|*|42ʻ[Q��!��J��)���ƽ�����1����F����Z 	y�3'rŁ�ȨQ4F��$*�S�YxwY�F3���*ة�+=U���m�z
#���G���iN��Bu��@�\*� L�S�-��v6��QAlQ�C�Ŝk��1��)�M�,��ʽsy����X��<+���>��9��a���P���q��V�wO���S�R��1���;���+g�A�п��Awʁ-�Ч�C����ֈ��~&MZ݂������J�;�&Wo@�zut"���k�¸'Ѽ�e�UԔ��lǵ}0�}�]�~�9����8"ߌ��O7����K���R{ �X��Yz^���r� �ע�LwUOK�!`S����5�	U�do>|���a������r�/���2 ��e�q�ٮJ{��A�n�'������V�;Q�����^��������&",F���sP�E��1{�'��وU�|W��y��S�]^�����ժ��K���
�� s�5� .e*>�_�䶍�/X�'[]�ݾ+�C�S��{F�{�u���tqj5X�$ܰ��1��d��MLCkS����2�?9�>ᆒj��a���}#�k"����c���}������]C� ��VR�ݚ�kz�Hـ�0fM���J<-���$.6���j�4��Q����T}��_�DU$���](��������yr
ڡ�u�b4�$��NIFk�'���/��27.-�xW��ũ8Td�Ů�� ����%��s6m�fÿAr��Yإ�kE�������CM�'�
y��S�-�|�"@A{`˳�t4d�R���_��F��!��Tr�������h���.�D�A�	�KÃ)�"y�]�y�̭�>ְ����'�(p�F����zM����`� r��~�g��j�c"q:Y��v��Uk�吠����.��Ur��Tˁ����b�k 7j�g�H��&`�����\�1�t�`5=�`��Ȱ�i_��jܱT�$UK8%�f����&SG��&�u�
��e	��&u�����.5�	�E�{���j�xB��v��>��g�ZM�#K���yLY?�@�h-u��&T����>���
�3�!�� <�Iyi���NSQl���iG�*>��?���#��bB4:f�(5����"�e�z��-�5����9p���uu�DJ�o���]��e�WIkş8, �L/�k��Yԁ�=����v����
����!��4:HC��b������%Crbz�Ym�<7��O�<�cO׶���R�K�;o�S���+�.�I��'?�Q���}�TE62(=�|�E�E�*pQ� ��<�5^�D���*����.Q�=4��T*���:/�C#�j�R�;8��r�FЛ�V'��]�*� a�F��۴�$}�`1��H��VdO�'���CAM��ǖx��#��UE4cE�k$Lq��ꛂ���l�~�AXA&�Y���^��M�p�?�X&�+1�Ξq�-�� �����F���?z6]�l�<S�h�`'g�Q�{y���\�Z�X~�'��5 C�ŴΒ���n8�.�r�-�/�R�I�|W!��A��N,[<�C$�4rk�fhkl�Oy���^���Of�Ў�// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <media/v4l2-common.h>

static unsigned int i2c_debug;
module_param(i2c_debug, int, 0644);
MODULE_PARM_DESC(i2c_debug, "enable debug messages [i2c]");

static unsigned int i2c_scan;
module_param(i2c_scan, int, 0444);
MODULE_PARM_DESC(i2c_scan, "scan i2c bus at insmod time");

#define dprintk(level, fmt, arg...)\
	do { if (i2c_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: i2c:" fmt), \
			__func__, ##arg); \
	} while (0)

#define I2C_WAIT_DELAY 32
#define I2C_WAIT_RETRY 64

#define I2C_EXTEND  (1 << 3)
#define I2C_NOSTOP  (1 << 4)

static inline int i2c_slave_did_ack(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x01;
}

static inline int i2c_is_busy(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x02 ? 1 : 0;
}

static int i2c_wait_done(struct i2c_adapter *i2c_adap)
{
	int count;

	for (count = 0; count < I2C_WAIT_RETRY; count++) {
		if (!i2c_is_busy(i2c_adap))
			break;
		udelay(I2C_WAIT_DELAY);
	}

	if (I2C_WAIT_RETRY == count)
		return 0;

	return 1;
}

static int i2c_sendbytes(struct i2c_adapter *i2c_adap,
			 const struct i2c_msg *msg, int joined_rlen)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	u32 wdata, addr, ctrl;
	int retval, cnt;

	if (joined_rlen)
		dprintk(1, "%s(msg->wlen=%d, nextmsg->rlen=%d)\n", __func__,
			msg->len, joined_rlen);
	else
		dprintk(1, "%s(msg->len=%d)\n", __func__, msg->len);

	/* Deal with i2c probe functions with zero payload */
	if (msg->len == 0) {
		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, bus->i2c_period | (1 << 2));
		if (!i2c_wait_done(i2c_adap))
			return -EIO;
		if (!i2c_slave_did_ack(i2c_adap))
			return -ENXIO;

		dprintk(1, "%s() returns 0\n", __func__);
		return 0;
	}


	/* dev, reg + first byte */
	addr = (msg->addr << 25) | msg->buf[0];
	wdata = msg->buf[0];
	ctrl = bus->i2c_period | (1 << 12) | (1 << 2);

	if (msg->len > 1)
		ctrl |= I2C_NOSTOP | I2C_EXTEND;
	else if (joined_rlen)
		ctrl |= I2C_NOSTOP;

	cx_write(bus->reg_addr, addr);
	cx_write(bus->reg_wdata, wdata);
	cx_write(bus->reg_ctrl, ctrl);

	if (!i2c_wait_done(i2c_adap))
		goto eio;
	if (i2c_debug) {
		printk(KERN_DEBUG " <W %02x %02x", msg->addr << 1, msg->buf[0]);
		if (!(ctrl & I2C_NOSTOP))
			pr_cont(" >\n");
	}

	for (cnt = 1; cnt < msg->len; cnt++) {
		/* following bytes */
		wdata = msg->buf[cnt];
		ctrl = bus->i2c_period | (1 << 12) | (1 << 2);

		if (cnt < msg->len - 1)
			ctrl |= I2C_NOSTOP | I2C_EXTEND;
		else if (joined_rlen)
			ctrl |= I2C_NOSTOP;

		cx_write(bus->reg_addr, addr);
		cx_write(bus->reg_wdata, wdata);
		cx_write(bus->reg_ctrl, ctrl);

		if (!i2c_wait_done(i2c_adap))
			goto eio;
		if (i2c_debug) {
			pr_cont(" %02x", msg->buf[cnt]);
			if (!(ctrl & I2C_NOSTOP))
				pr_cont(" >\n");
		}
	}
	return msg->len;

 eio:
	retval = -EIO;
	if (i2c_debug)
		pr_err(" ERR: %d\n", retval);
	return retval;
}

static int i2c_readbytes(struct i2c_adapter *i2c_adap,
			 const struct i2c_msg *msg, int joined)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	u32 ctrl, cnt;
	int retval;


	if (i2c_debug && !joined)
		dprintk(1, "%s(msg->len=%d)\n", __func__, msg->len);

	/* Deal with i2c probe functions with zero payload */
	if (msg->len == 0) {
		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, bus->i2c_period | (1 << 2) | 1);
		if (!i2c_wait_done(i2c_adap))
			return -EIO;
		if (!i2c_slave_did_ack(i2c_adap))
			return -ENXIO;


		dprintk(1, "%s() returns 0\n", __func__);
		return 0;
	}

	if (i2c_debug) {
		if (joined)
			dprintk(1, " R");
		else
			dprintk(1, " <R %02x", (msg->addr << 1) + 1);
	}

	for (cnt = 0; cnt < msg->len; cnt++) {

		ctrl = bus->i2c_period | (1 << 12) | (1 << 2) | 1;

		if (cnt < msg->len - 1)
			ctrl |= I2C_NOSTOP | I2C_EXTEND;

		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, ctrl);

		if (!i2c_wait_done(i2c_adap))
			goto eio;
		msg->buf[cnt] = cx_read(bus->reg_rdata) & 0xff;
		if (i2c_debug) {
			dprintk(1, " %02x", msg->buf[cnt]);
			if (!(ctrl & I2C_NOSTOP))
				dprintk(1, " >\n");
		}
	}
	return msg->len;

 eio:
	retval = -EIO;
	if (i2c_debug)
		pr_err(" ERR: %d\n", retval);
	return retval;
}

static int i2c_xfer(struct i2c_adapter *i2c_adap,
		    struct i2c_msg *msgs, int num)
{
	int i, retval = 0;

	dprintk(1, "%s(num = %d)\n", __func__, num);

	for (i = 0 ; i < num; i++) {
		dprintk(1, "%s(num = %d) addr = 0x%02x  len = 0x%x\n",
			__func__, num, msgs[i].addr, msgs[i].len);
		if (msgs[i].flags & I2C_M_RD) {
			/* read */
			retval = i2c_readbytes(i2c_adap, &msgs[i], 0);
		} else if (i + 1 < num && (msgs[i + 1].flags & I2C_M_RD) &&
			   msgs[i].addr == msgs[i + 1].addr) {
			/* write then read from same address */
			retval = i2c_sendbytes(i2c_adap, &msgs[i],
					       msgs[i + 1].len);
			if (retval < 0)
				goto err;
			i++;
			retval = i2c_readbytes(i2c_adap, &msgs[i], 1);
		} else {
			/* write */
			retval = i2c_sendbytes(i2c_adap, &msgs[i], 0);
		}
		if (retval < 0)
			goto err;
	}
	return num;

 err:
	return retval;
}

static u32 cx23885_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_SMBUS_EMUL | I2C_FUNC_I2C;
}

static const struct i2c_algorithm cx23885_i2c_algo_template = {
	.master_xfer	= i2c_xfer,
	.functionality	= cx23885_functionality,
};

/* ----------------------------------------------------------------------- */

static const struct i2c_adapter cx23885_i2c_adap_template = {
	.name              = "cx23885",
	.owner             = THIS_MODULE,
	.algo              = &cx23885_i2c_algo_template,
};

static const struct i2c_client cx23885_i2c_client_template = {
	.name	= "cx23885 internal",
};

static char *i2c_devs[128] = {
	[0x10 >> 1] = "tda10048",
	[0x12 >> 1] = "dib7000pc",
	[0x1c >> 1] = "lgdt3303",
	[0x80 >> 1] = "cs3308",
	[0x82 >> 1] = "cs3308",
	[0x86 >> 1] = "tda9887",
	[0x32 >> 1] = "cx24227",
	[0x88 >> 1] = "cx25837",
	[0x84 >> 1] = "tda8295",
	[0x98 >> 1] = "flatiron",
	[0xa0 >> 1] = "eeprom",
	[0xc0 >> 1] = "tuner/mt2131/tda8275",
	[0xc2 >> 1] = "tuner/mt2131/tda8275/xc5000/xc3028",
	[0xc8 >> 1] = "tuner/xc3028L",
};

static void do_i2c_scan(char *name, struct i2c_client *c)
{
	unsigned char buf;
	int i, rc;

	for (i = 0; i < 128; i++) {
		c->addr = i;
		rc = i2c_master_recv(c, &buf, 0);
		if (rc < 0)
			continue;
		pr_info("%s: i2c scan: found device @ 0x%04x  [%s]\n",
		       name, i, i2c_devs[i] ? i2c_devs[i] : "???");
	}
}

/* init + register i2c adapter */
int cx23885_i2c_register(struct cx23885_i2c *bus)
{
	struct cx23885_dev *dev = bus->dev;

	dprintk(1, "%s(bus = %d)\n", __func__, bus->nr);

	bus->i2c_adap = cx23885_i2c_adap_template;
	bus->i2c_client = cx23885_i2c_client_template;
	bus->i2c_adap.dev.parent = &dev->pci->dev;

	strscpy(bus->i2c_adap.name, bus->dev->name,
		sizeof(bus->i2c_adap.name));

	bus->i2c_adap.algo_data = bus;
	i2c_set_adapdata(&bus->i2c_adap, &dev->v4l2_dev);
	i2c_add_adapter(&bus->i2c_adap);

	bus->i2c_client.adapter = &bus->i2c_adap;

	if (0 == bus->i2c_rc) {
		dprintk(1, "%s: i2c bus %d registered\n", dev->name, bus->nr);
		if (i2c_scan) {
			pr_info("%s: scan bus %d:\n",
					dev->name, bus->nr);
			do_i2c_scan(dev->name, &bus->i2c_client);
		}
	} else
		pr_warn("%s: i2c bus %d register FAILED\n",
			dev->name, bus->nr);

	/* Instantiate the IR receiver device, if present */
	if (0 == bus->i2c_rc) {
		struct i2c_board_info info;
		static const unsigned short addr_list[] = {
			0x6b, I2C_CLIENT_END
		};

		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "ir_video", I2C_NAME_SIZE);
		/* Use quick read command for probe, some IR chips don't
		 * support writes */
		i2c_new_scanned_device(&bus->i2c_adap, &info, addr_list,
				       i2c_probe_func_quick_read);
	}

	return bus->i2c_rc;
}

int cx23885_i2c_unregister(struct cx23885_i2c *bus)
{
	i2c_del_adapter(&bus->i2c_adap);
	return 0;
}

void cx23885_av_clk(struct cx23885_dev *dev, int enable)
{
	/* write 0 to bus 2 addr 0x144 via i2x_xfer() */
	char buffer[3];
	struct i2c_msg msg;
	dprintk(1, "%s(enabled = %d)\n", __func__, enable);

	/* Register 0x144 */
	buffer[0] = 0x01;
	buffer[1] = 0x44;
	if (enable == 1)
		buffer[2] = 0x05;
	else
		buffer[2] = 0x00;

	msg.addr = 0x44;
	msg.flags = I2C_M_TEN;
	msg.len = 3;
	msg.buf = buffer;

	i2c_xfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    