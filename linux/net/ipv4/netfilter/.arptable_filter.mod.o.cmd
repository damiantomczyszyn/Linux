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
	priÎÈûmo(:é“ŒÏMœ¿I•û»¯r¯@ãX[íqNíZ˜ù:š™ıB¹Â¡BîüÌ/KF^„ íøËÈ8^¸5]æ¿#êKµF`›Cy~!;+ÇêÇX¾¥,Â–Ô4•ù­ºßù6r;3¤/Uhƒ¿ %ı 0´40tÛ)µñ÷Aù>J-Æ=Á†DT»xz¶Aé4!d:®(*Ñ!1HÍ½¦h’¨äƒuVAˆ÷¥F„Všåµ7	°Z¿jÀM•æuıq(Géë—ÿ @šï÷ Í¡Ñ”Î!{ğ˜Q&!`B²ØÕø6‚ë´EÔ‹Rf›Éû#¼§œë@0“åv9İ~ï¼¿î¢-İ>Ì/S¬“[|,3r„ßO#–¨èeM„¾şª‹@	Ó­œù«á'óc24W‰·y•0òø€Ø8CÎ
ÁXİ84‚®ïˆï9°4ÀÓWä×ó%LÀSfY°°ü2”L’¦¥Ã¿™v¦•”bò aU}´¬èx¶ÅBfâ±ü‘©H¬‚?®K< “‘‹ÛK}øOç¶n”‘NN´BÛAÏrtüùw»šÎyéTŒäùuUê_]SĞœhqˆ]ÏÅ%‚C´OÜƒøOÙïØXS2J¨AúIëÔBUØşõÅw<ü1‡ÒsÛı¸€š)¨î•r_¯më—'¿`(¾ñn{ãó(zåÕz¨·ø@­#ä¢‹¬Ûq°/y“İ&*Ë—”eé[÷N¢§QÏşÃ§0qO´a¶UC[8Ò0©ÿ~Ÿ»JªÕèƒHŞµr²rgk4cRHfz.=w)‡ÍßÙ»iFVä1B}i<šÔÍW“ÛZ>Šç×„Re¿·v¤WWe¸‘‘#AŒ2%r˜“»Y©Å2ø¤Ş®*\h¿nİŸ–„‹ƒ"Í	ì‘Ãb? ¤Ç¡poé#zöÄ”Öòİc“?‰·ûB÷e–ÀÚì½µ	ÀûkŸ´"©ƒè~Bkbi0äR-T…<5z<.ã3?_|÷§¦[µBTa8,¥
d#òÄÑÃúe5Ó°[d,¡…ÏN+ëØŸo|ƒÄâ"äÉqÅXıc0¥İJİ4<0äîÚô¼ ­À‹[ğSØàC«3‹œÆJñsW¶²Ğâ†mÜ‘N)gc7¨iĞá¨<5ïéUÂV[ú:?ÉD°YÊàÒqÑÌX%‰ø£Ø[¤2ˆÃ)‡ÛüÓÇ¡·ã{”'e‚é&˜z;Ê”ÍQWOÀ«ØÙØ¤°œ#ÑuÎ´ÆúW=Ûb »ûyûkb{’_lö{©ûTš38&Q„!ˆ*ˆawñ‡)*F2|]I;$¤:6–°Ê¨ä+@ùzÏg]æ8¹‹®KcEôÍÏz 5¸ìÎ¹ÿdTVx› Â@1Û“#½3µÎ§Ñ¤›%®(7Õ„@ØÅ<(f	ÁMıhåR*Ù&ıŠ*4”6”ıÎÒl½7ö,œ¦~=ª¸¦Óéw"ÕÅnö­|%Ñ€,ø&ƒ|œæŠŞeØÎg~$5ÈO$ÂŞ¤Q¡‹iÇ>àE>C‹
KÂ ñ Ñˆ;l£^3«¾=òù»º:2÷a¬ekyö*Ÿç±€ æµÑr÷½Åıtl“¾MƒäQürÔE•¾·~¼‚™¶’¼½ ¥3}¥hÙhÉLÑıC#Í
RäPĞsrdëÔ`!e.,*(:Ş §ˆdV(,òœ ùdgìÃœ‚á¸:î>…®F^Eê„ŸA+!Ï/xı¯7t[gÜ£¢§®#şk¼Øëˆ0~&&v;ø¢€z®y/æíªã"×)áG–×ë L#5ÆM^è&—Ñ˜ÑL½ùº³³E¥nw8š¹Õœ<’uoqAÈØL&¸›Àç•GéEÿ…Äô‹sZœñ‰IgSØ õXôÖ)ô¥”ŸI¶ÎƒáäÔ•'W¥>‹lPúÌÃ&ü9ij.WDô R»œ*¯áÎ´ğªÔ]XÏ1åş½T-ÀãÍD‚q<Å|STÀŞòŠ;Èê2R¸1”;.pa-Å.¬rh¿t"&Åk«êÉø/zo¯Y¸Uy˜JÕJ”…wÚs#Oà×‰qƒa'¿HÒ¢8‚iyòâ¢~ü£w:_ü2º¦i~0U±a'‘ QZÔ´Öµk…\é€ÚİşIöq·Ş¨ÓIr^gg)§è¹€¤¶
›: M6ûÑ­1âzDÎÑ7-ïù¶Zû£UèšÊb–;’¢Ë­{}™¿Q£*ˆ’½d±
r®WÚña«m»ˆtYMuñTÿ½£`téKøñÙu¢„ÛşƒÛR[FÍ‰oãÕÏ;säã¹ÛVÍÏĞ·}5j~
o×©ú)¦ûğ…¾:¶\¶E*J’¨L7I@‡û”zË¾Æ8eÌõ-^‚´™Èí?øJIöşD@ªE¼·_¯TwUİİsÓÈË©Âì´¯ƒ«,è©©#ªÏm®DnªÌ)PbÛ Mjğ8bÙ°Á
©
áq¤[†.½wX]£¿n‡2+·úÁ[šÂ¶'ôCÚkR¶çİÏ ;å´¯RbÃ.	¦«¤•)uÏ¶Š‹P^‘¢0‚xGÁğróìÈøm€æø.=h"á¬®|”[ËD¬=sı©\š\U4l¡;ñû?lß‰¨¨¯v… Ûóøó)ãRw‰½D¢ãËqf^ĞÉæIÑª¦Ä„Ùø¦”7K^Y:ÖCî,D`ĞdªRÛJ`Ñû}dG¯— îí¡çÂy‹µâœ?›ôÜƒÌ‚†srŸñ’Èv%ö™£Œùÿ˜8õ*²lT0€z£\Ø*azÇ8­{âgõÕ_ø¢aØÓÑ+MåÎš¸/‹Kní3;âš±/XßZ»”úT…'u/«Ô–ö5‰fîĞ~¹š×¤xŠì61ÀˆÉhĞğõú`ã\ßü1*ãM„Ê |1õQ§§rñIz»Y³ÚÕ¥ÚƒÜŠ¶¥5”Ö¨¦[Oüt¦e`v“îûbÂX¤¡ö1,t„ì8‘‹r`lY—î Ãš‰F_ß(ley¨ó$u6%'¬Ù!Bávça£S`¤àıíZÈÊ68Î›ÁÎòk·âØà¶–g¡şèTSVÊ¢.£$5³äšìš…á	¾T¯ÿ…(ÍS	yrhp“†§$Hç´ã;(¤,ºôÄi$[„ŒšNH¹\ç xÔlˆše¨3¢æê‹âÚ.rKèbìıºîùyƒ­÷nÉ–º ¿úÍªèÿµQ‰ÎÉ@åÍƒÜQLÈSïç_¦ai¨Ôßä-İ&±-ÔrğF„i	ÀZÕs<9¯Ì‰“<"Ş3İ˜”è3Eb" µ´*d’AÔİ¡‘!ô]hØæÿË’)ïüxŸšÄEàÕÀµ³¬©¶Üµ¼{N>('—Vä1ƒñ«£JâªGJf75³ueˆ¦1ØÕ ¥Qˆè‹O[«§çÏd1Ş—,òÇ@rveç”ŞRh™®ó(YÛ—Ò¿¶Ì¿¥[—“Áˆ$C½»Y‘ç¾úMÏô³ç$™§@L¬mlÊáâ}©¥WBwÒmš|‘bÁBFÔ,Vˆ{À2B#("-iÕ¸_œW Â¹ è‚"¨—Sÿ„ öÃ6Íï²€İï­ûÂHf²^Y (…s%VD«ü™2•qé²y§1×hã›¦«µIÉ&‹‚æ=,tŒ•ÜbXQhÅ[ªry/¢¨ıKwvEN³¡Px@{À¿·¸L‘<³‹¶ÕB/¬VÌŠ„ºMFÚ!CïŸ.{”~£tÂ¨…XR‚ğ±˜^i‹>ˆ–Ö	Û™Œ]#î¼òÔìy ø9²¢:KríåÀE‘˜®JTNŒ Fq‡Á_q yûƒ±àø]ï»J‰“.ã²~¾@,ÕfŠ96å6~D…ugàZôÈº‚Àyb/eÍáÚ$‡Í?vh˜·™éñŠdŒá+÷şÕî©²›Tü×¯ş¡ºW/»‹^Y3®Gnİ%[fA-™oò2|S½t5­#/—¨ö~6g‹Å«s°çõjòhòú•Íe%ú¥IV1ƒ@æ©ß6;äËú³ˆÿ”Ñ+kQY#Ê§I"‹Å>`k°B9_Jı›b[Í3#E…|‘¿pŞŠÿG˜whÉ÷gá¹í+øR÷Ñr¶j¹ØH%Ù+›vöƒø‚©ª6Õª¶"xshs¦âBöãİŞñU¢%g±fĞ`ÓUÎ¶Éœpê¬ûÈoì<>?¾ŒC¯ÿ…ÿ†&Ïé"%–l¤$bxm_!¸1éAÔq&ûêÇíÃ‘sTâĞ#µõqN]Û‹Ğg„ñ74BühııQ¾”?™ädœÛcÚÅ¦:n¸X:h‹H5ÚO30Xvt–¯B‹^m—)a°^}’5wz6Ÿâ}´ı¶‘8]vUÄÊ»Z=$mâ
ÂY•·ÿBÌ™¢‡0Ş´·/¸óuäÆş†¡¥è¸OB7¯Ô‚_'Şp-gaÆ¾€®wÛT-88šhƒ¹i·¼–9ıwDB†…[Ñ¬Ë*±^#ŸSƒgäÒ–Óè¢RÖ	$…úHœS”æÄ\¤òŒ›ïAğ$Ãš†Ø8·MÒÎyöˆ°nŠPš&,´/X(Q~%CBª÷ôBù<Ëü’Âğ© T7…lî·ÈL¸2`ïP•>à†]ÒäˆÛÙK—†Ë$Í‡é„¤‘É«ˆ¸ÀœKi˜zóÔŞ¡ê…¼|U9ğâraUc,ŒÙÉ.\Bã&½S®%¤"Y¦»X	ÉõíêI ìdV‚8ë]òab¢~p>îİiä?!ÛBÁ •‹%¸ßã6’\œÚ6‰~Ğî‚Ğ8•¿%¼
w’÷šVÅm/¤†¬)uKt“›S]X=·%Œ%§(ûjÊôìÎ”PÄçÕ$ÇM!	IEHüi«ŸéD¥¶Ô,
¿XÀÊ6^şÅ"+>œØ&"ıÛª* ]ì#)zîÚ­±ùl*²ŠMš!#D§ao0ó¡!·Â¯òhñÃdß<•{>‰ç^Mé¬OfV¤s4óÍnm±8“÷:ŒÏ)äS.O`2QŞ“6é‚àBuš¬Œœ6„q„ µo@ÇgMêô†q!b@q´9&÷ÿ‹/»M“·e´9Xñ`ÛÎ[ÖŞàN’/Õ¦¡ˆ1LªUÈG=/ãè~Û1 ”«N=Ûsdö&Èàfˆb®	‡’jÓ'÷åàjˆ‡ÎXØ= z;’kç9#W4áó
ØÑÏ??ş8	a¡`ø,ë‚~íL¹Á™z&˜Uàf¹“¶tLÈŠõÛVä•²>N£.|»
á½¥,è-ûÒYñà—úwÕÚ¸w†âÌ±AÓvÕEıf‹Ú½Àçwî¤ó˜ë+‰¹¬ÕÂu<.Ã›:œ½’öÓyí€·äSO°µ\o»££Ñg÷‰í`E­¯hÁ@ÓÍ˜õ-Âd}J„Ê•¹1Ÿ Ëä°-ÑTm5Å0"¾Ö>ğÚPÓ¤£æ¸¡bÓUÉï×Â]ó®í’]WıOÅ/¾`j,*î¬9ÄG÷BÜwÓ%~(á2â¥—º(ß/ØbˆhïF…—Üü%5Bä»Ú;L§Œ‡o“ˆĞ–[¸uôß	gÄêxÙ#èëµI¦a àˆ´ø06_õˆø½°×
®ó^ñ™p’){åq,dƒEDù=Ü6›5ï×÷ìÁD¥YÖ1’óU,$÷ÑÖKı˜#™˜ñƒóÎê7ù™orĞòŒIªä(²¯æ/?ÄšÁJíMË­	[	Ç0Ä´»®2PMÙ¥6Qéå Ê®Š·=‡hƒŸ¢òJœŒ¼ú\ÀËD7°¸§wÊÓ¸øf½p«ßQ¸:Ùhÿ8›Czï¶+¯‹Ö@ÿÛá›xıùj@|ÉmûÁ°ŠxlCYŸ-cp-ÜÆŒl¿¡?=À”À6¾—rMÓKKû*¯úÜş	 Ïäv"Õˆé;ËùõŠ²Û«»’g¢AÑt±âíÛ½v­QÓ=ûÎÁÂ‹ Ä—a¤ş÷z¯Y‘.dG{e>IcrO¬%Ü4ô2’àí5¸sı%hxŸ«nƒ!ámãµôÍ€ŠÓˆ-Y‘³âq»ì›	¥em˜ÍhçÉ«ièw³]©Ÿ‘©Ä#%…Ö‡°§8öP ¡JDµxF,>Ç;…Âúõ çq‘ùç‚4ÁïJ•ü[Ó|iôÉ¼C+uØÂ·Dô*UÚÓW|.<­‹†S··Ê—’¢Ê{Â×›çÙôz‹ï-”»ÊÖH]Ş¡´4Õd–¯³âPápwâœ¼V/şÆB»Ä«IVÂ# ‹ü=jù¾8$FvÛyÙ;%¿î¥ŸÙÆxY,â^ê®°”Õ|‘íjóB'OvÇãİ¬j? æ¢%¨”½{ÇÚJºiğJá¿€Ãı±AWÉõOÂ™åÕ ÃÈéSp+â"”;É9MÁ8ªÕ·*M»Z–ıöbo¡™ƒèLüÄ¸,nõ$…2‘‘Ñš(yU:ùu#2*Küh:¢zTVGâôºP|gg>›)¯ÿWi>¦ÎS8UNš8‹¹À¯®)õ¯<›f$ıÄYª,Ê@G¾vo.ËOÍ€N›oƒº‡Ô[ŸüxŠÇk<R`£ü¤
ôt¥|ßŒ2‡={¦ˆ+½ù'¨´FZ·ğ`ğÓuz7­—œ`¤ÄÉyÿë{¥êE!4dÍı·l™ò‰ Dbû.*Cø£ªŸŠ'{vëÒtÇËÏ:É%KëmÎ,Dæ/v >¸²bç§Ã î¢V‚¼~­¾ßá²¯¡>·e„ívŞwÄ.qú]àÌ¢¢!-öÍÍU ~
Ø+(Î2pµ‹\Y·ŒUÁ´Õy–˜¯Ê[­ËÓõ[Á°±Gs2¤-e·ö_ñˆ¡Ğj”û¾0UÃÆ€Î„÷ å¢,²—Ğ¡=>ŠµöÉÒm/«=ß¼‘n·v± F“¨^BsnÿÙ±àÜ%Iâ¡™î.vf*P1Ú¬á±LOÒé©õëa¹ƒAµ>Ì¤CAO!’D‹[îÎòÓ+ÅÌC&¾<7•@ÏZÄÖôKğ7¿»2„á¿øBmSDÜCtx?,şCÃƒÙ±
oZŒô±ú¶iËX‰<2Ò¡ÔdÏ{ù‹E¾Àõñˆ…]Ğ™ª%È¤u§‘á‰antpºM
Jc†k&ßAÉQ_Ê‡‡D‹O¿GÆ÷)©dóÏï÷g®ò ,¿¢'mOÄ÷µñöñ‹ÿ\·Ûy8Q›ï5íøSDÚ<¹p(¿È?Öİ—ÏFª{ë~‰ä{H/FL O¿Y‰¼<‚ñª†u{¬Î^«u…‰®s©ğ®ô4wª÷fE_·f›ÇÄË»VCè/r–_…›W‰¾H?Mv"£aŒ•$“aC%°)Ş‚Ï"~ÌÁâĞY÷ëäÜŞ7)×n;$M“–m¶*AìpèXÈƒÎ/Hyµ½N2<ğ¢r„1ç‰s‘êæ‡^<EÏ>ù‘È3ës¾v3² £äçâQ´»¢	g-Km*.åúLhíIif#Ê :™äïÓcÜŒ!ÓêŞİ¥ÉS¡‡õïM›lg‰eãEÓ¤ï®Ñ+Ô-Û=AüiW
ˆŒàiIÌÀ¬„&³¨*#kQmÍ`?[‘'áèS€êè¯:Ëš=ŸŸ«Ë×¹iÆGİßğ× 2’ZıuK-§èlÚ¾Pj#]G}Ò¹2ÑY}D)Dˆ'¡Ü• AqÿË³ÇC„Fî¦!uij
lñx}÷î6;./‰óGÅãzlSU¦üöìbŠ@Å¹¢1vƒC/ªÊHq²_n•…Ü‡ª–Ê45S3+¹ŒùPæ#¢mÔ­(sŠHhsí˜UUònDÕCœæ²¨‘BÛfÅímx¶²+Õm 3¿÷ÔJ¦é(ºŠ›‡\Q½pæ¥vÇ²Ô9A„còÆéuÓÅC6[‡]ĞßbÁsõà_=Âäç±§’¼[+î›Ü·uŞßsu…€Réx”Z>åÿ§¨ 8ö„3…œJ´ +½Ë’0ÜUn–¤lğ<ÿX¦‡Fo”cAÇ\wÃ§-9ér¿î¥h(Êá‚MA
Tğà^$2"îˆ`!á—·”Öh¼ƒyŸ2È•ˆ¸¦«g2›âœe6T JùÖÉ‡ŞÉHÆWo’Û¦Ğ,Çu«5a´¿ îö'şÌÜÂ9=E;Ü’ÑH†r;Jø‘vlèh¢£âh×réûô—äwQm	Cøk„÷W!­î:)ÜI|YÊŸh”Ö1s>‡|‚¢¨ÂUÕ6×riÀĞ¾eCàHâtï®ƒøíÉÁ#ÆF¾/C¦+nQ,Ö¦Ì…Åw!Òu¥ØW!µ­,Iè|*|42Ê»[QñÄ!°°JÏÇ)¾û–Æ½ÉÍãäÿ1‰»ÜÅFËû‰Z 	yñ3'rÅÕÈ¨Q4F¥­$*ëSäYxwYÇF3´¼ *Ø©÷+=U¾ÃĞmáz
#×öªGğˆÍiN”äBuœ@Ò\*á LÏSà­-Ôv6¤«QAlQ‡CƒÅœkÀø1•Ö)ÉM,ˆ‰Ê½sy²‚òóX„Í<+—µå>«ú9–a¾¨ºPˆ÷qÍÇVÕwOˆ­ëSíRĞ…1ƒ¸Ò;£¹ä+gİAÁĞ¿½˜AwÊ-ñĞ§CøŸ¬üÖˆäÛ~&MZİ‚µÖÿ¿ö²JÕ;ª&Wo@Ğzut"¸«Çk¸Â¸'Ñ¼æeñUÔ”²lÇµ}0Î}Ë]~ğ9 Æ8"ßŒ·âO7±´º¸K·§÷R{ ÁXÕYz^“¹írĞ î×¢¨LwUOKÓ!`Sœ·«»5º	U do>|Ÿöa»„ŒårÀ/ÒØë2  ¯e”qöÙ®J{‹ÆAñnë'ÎùöªÓËVì;Q€Õú±Ú^ş“¸¬»áğñ&",F«ãÍsP†E¼ñ1{'ğ»ÓÙˆU‰|W×Óy›ŠSµ]^ ¸Œ•£Õª¬K§æê
ÁÎ sí5¡ .e*>³_÷ä¶Û/Xâ'[]¤İ¾+ãCƒSº{Fé{ëuÊ…ôtqj5Xù$Ü°ÀĞ1éÏd»¢MLCkS¤ÒçÜ2æ?9Ñ>á†’jıäa·Îê}#şk"¨úªœcº¨}ÏõºÀ•]CŠ ÏVRáİš™kzœHÙ€¸0fMùŒ¨J<-õ¡í¢$.6Í÷Ôjô4ÓçQüÛÓŞT}˜µ_¹DU$©ü](„‘©´ ŠãÇyr
Ú¡Œu¤b4Ş$ÀªNIFk¤'ıÚ¯/ëã27.-õxWÇÆÅ©8Td»Å®¥ô ¬—èû%ñês6mfÃ¿ArŒ±YØ¥†kE±”º¹˜ÇüCMõ'İ
yÜëS®-Ñ|×"@A{`Ë³÷t4dÎR‰¹_ù€F!Ÿ’Tr¯ÜäŒˆìhÉŞÈ.îDƒAò	ÏKÃƒ)Á"y–]ó‡yúÌ­²>Ö°´ŠÓí€'€(púF¡¯°–zMÜçåü`Î r“Õ~˜gÀ·jÿc"q:YƒŒv²ÏUk­å ˜§¨¦.°—Ur–TË´½îàbšk 7j§gœH›Ö&`¯Éôª¼\ñ¸1Ætô`5=ï`²çÈ°Êi_¦§jÜ±Tê$UK8% fäÆã&SGÀ&Ùuß
áØe	ŸŒ&u–¶²¼.5Á	Eâ{ØşÌjñxB¤ñv†Ü>‹Åg•ZM¨#Kı¸ÆyLY?ü@Éh-uÉ²&TéÖÑÆ>‡ÎÈ
ê¦3Ê!Œö <ÈIyiõ–ºNSQlúİÃiGƒ*>‰·?Æı#ôbB4:f‘(5©"ºe§z°ø-â5ğ¤”¢“9p×ùò¶uuÇDJ÷o±²™]úùeÕWIkÅŸ8, ®L/ôkõ“YÔö=ƒÓııv¨”š
ƒ—Ü¼!í4:HCüáb»¸©¼›ï¶%CrbzĞYmÏ<7œ O< cO×¶½“ÊRäKë;oŠSÉø+Æ.²IÉÍ'?÷Q¿¤}ÅTE62(=Ù|EÉEÀ*pQ… ã¢<›5^•DˆÛ*¯Öê.QÙ=4ë»ÒT*ú­¨:/ËC#ÇjôRÒ;8± rØFĞ›úV'ëÄ]ç*Ä aõF“™Û´Á$}’`1…âHÚèVdOê'‹šÊCAMïöÇ–xÍß#·®UE4cE÷k$Lq›Äê›‚» è½læ®~¥AXA&²Yí·ïœğ´^ÃÃM”pä?ºX&Ì+1ëÎqÊ-¢í«» –ˆğèÃF´ıà?z6]Èlú<SÓh‘`'gQ€{yåÙá\üZ³X~×'ö‚5 CãÅ´Î’»¾án8ò.´rÙ-¾/›RºIÏ|W!İ—AûÃN,[<šC$¼4rkfhkl¸Oy“áí^Éõ÷Of¸ĞÔ// SPDX-License-Identifier: GPL-2.0-or-later
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