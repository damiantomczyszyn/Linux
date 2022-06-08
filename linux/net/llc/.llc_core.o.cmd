(&torture_ww_mutex_0);
	ww_mutex_unlock(&torture_ww_mutex_1);
	ww_mutex_unlock(&torture_ww_mutex_2);
	ww_acquire_fini(ctx);
}

static struct lock_torture_ops ww_mutex_lock_ops = {
	.init		= torture_ww_mutex_init,
	.exit		= torture_ww_mutex_exit,
	.writelock	= torture_ww_mutex_lock,
	.write_delay	= torture_mutex_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_ww_mutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "ww_mutex_lock"
};

#ifdef CONFIG_RT_MUTEXES
static DEFINE_RT_MUTEX(torture_rtmutex);

static int torture_rtmutex_lock(int tid __maybe_unused)
__acquires(torture_rtmutex)
{
	rt_mutex_lock(&torture_rtmutex);
	return 0;
}

static void torture_rtmutex_boost(struct torture_random_state *trsp)
{
	const unsigned int factor = 50000; /* yes, quite arbitrary */

	if (!rt_task(current)) {
		/*
		 * Boost priority once every ~50k operations. When the
		 * task tries to take the lock, the rtmutex it will account
		 * for the new priority, and do any corresponding pi-dance.
		 */
		if (trsp && !(torture_random(trsp) %
			      (cxt.nrealwriters_stress * factor))) {
			sched_set_fifo(current);
		} else /* common case, do nothing */
			return;
	} else {
		/*
		 * The task will remain boosted for another ~500k operations,
		 * then restored back to its original prio, and so forth.
		 *
		 * When @trsp is nil, we want to force-reset the task for
		 * stopping the kthread.
		 */
		if (!trsp || !(torture_random(trsp) %
			       (cxt.nrealwriters_stress * factor * 2))) {
			sched_set_normal(current, 0);
		} else /* common case, do nothing */
			return;
	}
}

static void torture_rtmutex_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/*
	 * We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2 * shortdelay_us)))
		udelay(shortdelay_us);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rtmutex_unlock(int tid __maybe_unused)
__releases(torture_rtmutex)
{
	rt_mutex_unlock(&torture_rtmutex);
}

static struct lock_torture_ops rtmutex_lock_ops = {
	.writelock	= torture_rtmutex_lock,
	.write_delay	= torture_rtmutex_delay,
	.task_boost     = torture_rtmutex_boost,
	.writeunlock	= torture_rtmutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "rtmutex_lock"
};
#endif

static DECLARE_RWSEM(torture_rwsem);
static int torture_rwsem_down_write(int tid __maybe_unused)
__acquires(torture_rwsem)
{
	down_write(&torture_rwsem);
	return 0;
}

static void torture_rwsem_write_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 10);
	else
		mdelay(longdelay_ms / 10);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rwsem_up_write(int tid __maybe_unused)
__releases(torture_rwsem)
{
	up_write(&torture_rwsem);
}

static int torture_rwsem_down_read(int tid __maybe_unused)
__acquires(torture_rwsem)
{
	down_read(&torture_rwsem);
	return 0;
}

static void torture_rwsem_read_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealreaders_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 2);
	else
		mdelay(longdelay_ms / 2);
	if (!(torture_random(trsp) % (cxt.nrealreaders_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rwsem_up_read(int tid __maybe_unused)
__releases(torture_rwsem)
{
	up_read(&torture_rwsem);
}

static struct lock_torture_ops rwsem_lock_ops = {
	.writelock	= torture_rwsem_down_write,
	.write_delay	= torture_rwsem_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_rwsem_up_write,
	.readlock       = torture_rwsem_down_read,
	.read_delay     = torture_rwsem_read_delay,
	.readunlock     = torture_rwsem_up_read,
	.name		= "rwsem_lock"
};

#include <linux/percpu-rwsem.h>
static struct percpu_rw_semaphore pcpu_rwsem;

static void torture_percpu_rwsem_init(void)
{
	BUG_ON(percpu_init_rwsem(&pcpu_rwsem));
}

static void torture_percpu_rwsem_exit(void)
{
	percpu_free_rwsem(&pcpu_rwsem);
}

static int torture_percpu_rwsem_down_write(int tid __maybe_unused)
__acquires(pcpu_rwsem)
{
	percpu_down_write(&pcpu_rwsem);
	return 0;
}

static void torture_percpu_rwsem_up_write(int tid __maybe_unused)
__releases(pcpu_rwsem)
{
	percpu_up_write(&pcpu_rwsem);
}

static int torture_percpu_rwsem_down_read(int tid __maybe_unused)
__acquires(pcpu_rwsem)
{
	percpu_down_read(&pcpu_rwsem);
	return 0;
}

static void torture_percpu_rwsem_up_read(int tid __maybe_unused)
__releases(pcpu_rwsem)
{
	percpu_up_read(&pcpu_rwsem);
}

static struct lock_torture_ops percpu_rwsem_lock_ops = {
	.init		= torture_percpu_rwsem_init,
	.exit		= torture_percpu_rwsem_exit,
	.writelock	= torture_percpu_rwsem_down_write,
	.write_delay	= torture_rwsem_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_percpu_rwsem_up_write,
	.readlock       = torture_percpu_rwsem_down_read,
	.read_delay     = torture_rwsem_read_delay,
	.readunlock     = torture_percpu_rwsem_up_read,
	.name		= "percpu_rwsem_lock"
};

/*
 * Lock torture writer kthread.  Repeatedly acquires and releases
 * the lock, checking for duplicate acquisitions.
 */
static int lock_torture_writer(void *arg)
{
	struct lock_stress_stats *lwsp = arg;
	int tid = lwsp - cxt.lwsa;
	DEFINE_TORTURE_RANDOM(rand);

	VERBOSE_TOROUT_STRING("lock_torture_writer task started");
	set_user_nice(current, MAX_NICE);

	do {
		if ((torture_random(&rand) & 0xfffff) == 0)
			schedule_timeout_uninterruptible(1);

		cxt.cur_ops->task_boost(&rand);
		cxt.cur_ops->writelock(tid);
		if (WARN_ON_ONCE(lock_is_write_held))
			lwsp->n_lock_fail++;
		lock_is_write_held = true;
		if (WARN_ON_ONCE(atomic_read(&lock_is_read_held)))
			lwsp->n_lock_fail++; /* rare, but... */

		lwsp->n_lock_acquired++;
		cxt.cur_ops->write_delay(&rand);
		lock_is_write_held = false;
		WRITE_ONCE(last_lock_release, jiffies);
		cxt.cur_ops->writeunlock(tid);

		stutter_wait("lock_torture_writer");
	} while (!torture_must_stop());

	cxt.cur_ops->task_boost(NULL); /* reset prio */
	torture_kthread_stopping("lock_torture_writer");
	return 0;
}

/*
 * Lock torture reader kthread.  Repeatedly acquires and releases
 * the reader lock.
 */
static int lock_torture_reader(void *arg)
{
	struct lock_stress_stats *lrsp = arg;
	int tid = lrsp - cxt.lrsa;
	DEFINE_TORTURE_RANDOM(rand);

	VERBOSE_TOROUT_STRING("lock_torture_reader task started");
	set_user_nice(current, MAX_NICE);

	do {
		if ((torture_random(&rand) & 0xfffff) == 0)
			schedule_timeout_uninterruptible(1);

		cxt.cur_ops->readlock(tid);
		atomic_inc(&lock_is_read_held);
		if (WARN_ON_ONCE(lock_is_write_held))
			lrsp->n_lock_fail++; /* rare, but... */

		lrsp->n_lock_acquired++;
		cxt.cur_ops->read_delay(&rand);
		atomic_dec(&lock_is_read_held);
		cxt.cur_ops->readunlock(tid);

		stutter_wait("lock_torture_reader");
	} while (!torture_must_stop());
	torture_kthread_stopping("lock_torture_reader");
	return 0;
}

/*
 * Create an lock-torture-statistics message in the specified buffer.
 */
static void __torture_print_stats(char *page,
				  struct lock_stress_stats *statp, bool write)
{
	long cur;
	bool fail = false;
	int i, n_stress;
	long max = 0, min = statp ? data_race(statp[0].n_lock_acquired) : 0;
	long long sum = 0;

	n_stress = write ? cxt.nrealwriters_stress : cxt.nrealreaders_stress;
	for (i = 0; i < n_stress; i++) {
		if (data_race(statp[i].n_lock_fail))
			fail = true;
		cur = data_race(statp[i].n_lock_acquired);
		sum += cur;
		if (max < cur)
			max = cur;
		if (min > cur)
			min = cur;
	}
	page += sprintf(page,
			"%s:  Total: %lld  Max/Min: %ld/%ld %s  Fail: %d %s\n",
			write ? "Writes" : "Reads ",
			sum, max, min,
			!onoff_interval && max / 2 > min ? "???" : "",
			fail, fail ? "!!!" : "");
	if (fail)
		atomic_inc(&cxt.n_lock_torture_errors);
}

/*
 * Print torture statistics.  Caller must ensure that there is only one
 * call to this function at a given time!!!  This is normally accomplished
 * by relying on the module system to only have one copy of the module
 * loaded, and then by giving the lock_torture_stats kthread full control
 * (or the init/cleanup functions when lock_torture_stats thread is not
 * running).
 */
static void lock_torture_stats_print(void)
{
	int size = cxt.nrealwriters_stress * 200 + 8192;
	char *buf;

	if (cxt.cur_ops->readlock)
		size += cxt.nrealreaders_stress * 200 + 8192;

	buf = kmalloc(size, GFP_KERNEL);
	if (!buf) {
		pr_err("lock_torture_stats_print: Out of memory, need: %d",
		       size);
		return;
	}

	__torture_print_stats(buf, cxt.lwsa, true);
	pr_alert("%s", buf);
	kfree(buf);

	if (cxt.cur_ops->readlock) {
		buf = kmalloc(size, GFP_KERNEL);
		if (!buf) {
			pr_err("lock_torture_stats_print: Out of memory, need: %d",
			       size);
			return;
		}

		__torture_print_stats(buf, cxt.lrsa, false);
		pr_alert("%s", buf);
		kfree(buf);
	}
}

/*
 * Periodically prints torture statistics, if periodic statistics printing
 * was specified via the stat_interval module parameter.
 *
 * No need to worry about fullstop here, since this one doesn't reference
 * volatile state or register callbacks.
 */
static int lock_torture_stats(void *arg)
{
	VERBOSE_TOROUT_STRING("lock_torture_stats task started");
	do {
		schedule_timeout_interruptible(stat_interval * HZ);
		lock_torture_stats_print();
		torture_shutdown_absorb("lock_torture_stats");
	} while (!torture_must_stop());
	torture_kthread_stopping("lock_torture_stats");
	return 0;
}

static inline void
lock_torture_print_module_parms(struct lock_torture_ops *cur_ops,
				const char *tag)
{
	pr_alert("%s" TORTURE_FLAG
		 "--- %s%s: nwriters_stress=%d nreaders_stress=%d stat_interval=%d verbose=%d shuffle_interval=%d stutter=%d shutdown_secs=%d onoff_interval=%d onoff_holdoff=%d\n",
		 torture_type, tag, cxt.debug_lock ? " [debug]": "",
		 cxt.nrealwriters_stress, cxt.nrealreaders_stress, stat_interval,
		 verbose, shuffle_interval, stutter, shutdown_secs,
		 onoff_interval, onoff_holdoff);
}

static void lock_torture_cleanup(void)
{
	int i;

	if (torture_cleanup_begin())
		return;

	/*
	 * Indicates early cleanup, meaning that the test has not run,
	 * such as when passing bogus args when loading the module.
	 * However cxt->cur_ops.init() may have been invoked, so beside
	 * perform the underlying torture-specific cleanups, cur_ops.exit()
	 * will be invoked if needed.
	 */
	if (!cxt.lwsa && !cxt.lrsa)
		goto end;

	if (writer_tasks) {
		for (i = 0; i < cxt.nrealwriters_stress; i++)
			torture_stop_kthread(lock_torture_writer,
					     writer_tasks[i]);
		kfree(writer_tasks);
		writer_tasks = NULL;
	}

	if (reader_tasks) {
		for (i = 0; i < cxt.nrealreaders_stress; i++)
			torture_stop_kthread(lock_torture_reader,
					     reader_tasks[i]);
		kfree(reader_tasks);
		reader_tasks = NULL;
	}

	torture_stop_kthread(lock_torture_stats, stats_task);
	lock_torture_stats_print();  /* -After- the stats thread is stopped! */

	if (atomic_read(&cxt.n_lock_torture_errors))
		lock_torture_print_module_parms(cxt.cur_ops,
						"End of test: FAILURE");
	else if (torture_onoff_failures())
		lock_torture_print_module_parms(cxt.cur_ops,
						"End of test: LOCK_HOTPLUG");
	else
		lock_torture_print_module_parms(cxt.cur_ops,
						"End of test: SUCCESS");

	kfree(cxt.lwsa);
	cxt.lwsa = NULL;
	kfree(cxt.lrsa);
	cxt.lrsa = NULL;

end:
	if (cxt.init_called) {
		if (cxt.cur_ops->exit)
			cxt.cur_ops->exit();
		cxt.init_called = false;
	}
	torture_cleanup_end();
}

static int __init lock_torture_inøÏXrdÔâPİCş¡¸¿ˆ%tQÆ«ÎS6Ú0%>k·ª¬ÜúÊvåËö¾uşIõQLıBp­Üö8˜oŒ(ÿ—ÇHb½ãÑşº$‹^§íÁ©…î„pò¹ı++;†H%–ÇG#£Ü&fÁóÊõæª1x}{ñ}J”¢ßHÏÊŞÉs}¹»œnÌÍL„‘Q0µß‡ë\„g£‡+	Â°ƒ¬µ|_ŞPf»0Ü"÷9®¡øêfàt¥Tªÿı §¬Ô:Ù¸Å«+ßÂ3mÈ
e•RºÇÀAQ¶»‘âÇ>ş‚á³gævg–%	6\ïbBÿ8?¶5‘µ ¸>`o$›—‡†Y¹‰ó›ø6ç	ôM7”;l¼õèmò%Ôd³ ”^qÃ Áˆá«şÚ›Ş†l‘ı†%v¶2võJe&ãü…¨N›¶¹Î±WkÏx°€Ü”%ğƒ§$v6â$«iÅ63¶±çŒBu@`'/)ëXÆìb­ä~}ºÏq¿Ö®ü˜¤n5|Sü(íƒ}d§'ä£ç‰—Ë:Æ~a™öõp|"íÆ/HB7hA¶É5GÚ0|+qR¸]ê“6 BN—R®şˆíÿ¡ü÷sÓš'0Å`^õŠeèj‚FºT‹Y6(ó¸Ûé‡	IúªçãI @P`,ÊTšæĞ‡Ã‚!Bµüö*ËMs:,2NoÛ{ôÜoA—şÕszW9Æomöu?f½Ñ ú~aWîŠ¸1X…m<++Ô3`?¿nâFÎÇ²“]ºN8š¹ƒ½=D…<"ç˜p„¢ª,Š³;ñY'Ç~Íªí†ê+ğúÒ ¤ Dg¹Ì…#U«Uä½d’ı÷UîöÓtòMH¡j»â¸Ù;¥¦‡‰3I,Lñ.„ïõHÁY}ÔŸ»ÚdèÏ¾¤ÄI4áh)6XèDE4’Æu«QßOĞñ…ÖÖÓãÕáEÁ]´ŞÄŠ®úD;Ç2H ßè“Û5yRéÅ²ï3àÀyµ´˜#(É“mhê”‚“Á;d7k¥rtçü¥"ßJˆÿÊÙ‚kOağèÒlŞ«×^ii•v¬+áUË8PÇÕè0 õ¤Uã~ÿrû@I†É¼âEÎò,NË!d‰.¯âï4ì.X¥‚yÕkñ~=“ÓÈÈ²ÃØÖ>ß•Ÿ‰ŠU‡kÕKŠÓœÜ§º'Ë6AÛFâêªn NğE_à8XÎ²ßÇÒıU+Æ©™ï°CÖ'z@è¯‰¶¯ş’{÷Àï:tù€pÁj$gTãÅ¾¶Û¢S¤ÿs)5Ğj9ÿë4¢ƒXW»Ù]5	0ÖdJö&kÚ„Ì0-¦¾“êšsÎŠiİ¿¶İ¬ldn……ş0ßgì-´øD«ÇX­‚_fh¨Ùé‡=dâ¤l#=YïV…®açéx¨å%d²fÛ_æµ°…=ÆB@¶V¤ªp>fz3ÎËM¤1l/eQÑÁ
\+UvÖ1lÒR½Q›ù@õ)·.Îî³£Û×´‹5PºjTÅÀUN[ˆ{Uv´	)Ùbv”óq;ººÛ –'­KT¨]^Ùd{1;3 éHµâ¢!AÄåOĞãò“š WÑ€"?ËŠ'
¹! ğûü'Cù•¥çšÚº@S}©œ¿déÂ¬ÈE¶óÈµˆìÜXÑµû$OÃ6š¤/Ğ‚°*Ÿ—†ò©ƒñ'™‚¬©šÊê$íTjò
Ê¨C}3|ö# º-·¢ûiŠèÓg1äÙÈâ„>Ùèz¢1šs\å1ĞÙRc¬Dg¶¼;®ÜU0€y‚ÕÓ-¯êUFoã6B‰%£)è("·³(g®‚ó)¿¯Ç‹¼q€•wÛÏğêÆXn?Á-Šıíì§•kŞTl¶jš¼ËÜî «Ššl=½ÇãÛç	Ò†„´ÆópĞ³ìYÄêz“(ĞSç§Î[‘`V€¿d±÷w‡ç­n~&ˆ~\!É"×0\®À›µ>¨O¿ù„â÷Ys8§iE—×Hd	
’‡8r(‚cN‘,sxÃÿ€”B­«Ñƒ>ÚqW¸L&VËšÃg4"ñl;å•oá6ë]üYg«›`6‚è‚¸â62á{¹2TğÃV¢%ÁÄPîôR ¡¬3­ºl
¿÷²2b²úGrGMpu1ªÛáÁ~ÌW¯Ø‘Å†C™¡IŸ§%2¶Ë(TñÎı$uÉïôÏÔ/•ô(u_`Ààˆ§E/“&‚v^v‚½k	&X¢xÇ; „Ì-<lõ^õ2øê`¤¸p–û_…ØıÿÄ÷±ÛWöĞQ=AzéC JBBT>ÚÉ9ŸZˆı/vÌql	YN…”åE¹æi®6–™}	S1mwå(åŸA‘çSşÈ~WZÎ©ø†GØYÍvIwÓq÷,ŸûVBî;IPÈµåÇÂ’­ %õ0{³	.’Ì{1;æßÓ˜ä©Û5“˜*K?AÉs
.yF™¶Ç`YÛİ†´Qs©Ú­;İ¥¿LW›*dæîäK©°jŒêûc€gSíS™°±’It
)ë¸)n\E_;Eë„’	$U©v>R‡Y•µ”JB­íüõ$5ëz¦ñ±S$Dj3²ãEİ1¯@ÉÈMáá?ÿ2Ã}ƒÚøÌƒúEûçööÊù6'Yò:ç›‰dÑ•NV/ñ¢¥æØ¢gÁsád"Væ¥k±yKúV VvÅ·fcjÈÎìR®ÕûùÏ.:O)€¾ö*gpµÕJà=µ]3C¥tùıë>Ü.ˆƒ–İ"‚z¸„Ë{ZŠÜÑ}÷»Í¡šóCp‚øÓàF!ÒSjÃ_Ü”—^d8uîhÊ-ÔNYI]¡Glàk[w¢†óıpÄwbˆ iä}Z?µr_Ş&·ì)§œì¤™èj'g6lÛş®¹UÑå?2Ôşÿ.á÷è¶H‰_KÎ`«¶’/µG‚ÓÔnt¸Daí>êşXkHC½;3öqĞåRµÅíÈ¤¬Fcp·¦¥}Â–*VÜm”ÒQ‡‘[B†8±Ös·E ÄÇÿ•y…G©rSõ@ÿÒ%åòÅ
7½¹X5©.°é²å-bîTo/Ñ N+©àÚÑ×Z‰‘®ÉEÕH9ĞÁÌuÈ¹añ
50ºœ“ ÷à~ãã²}²7œ+ùK·µ
ëÍO'r\F?S+WS°Wjô¼4Ìt…%üÇLM¥è¸£`ovÛ;•abÔ;^Âa{Zzi~HÚº	xîİyÜ{0¨ıÄèí¼¡‰Zlaòç( ŠÂ™yŒŒil®å|ÑÓM›ôPrÓêi›ªÇÊU‚·Œ‡]{nx¿ººÌ©sïŠÖ¡§ìß2™ÿ8&ôæêÊçmµ^›`Í©:ÃŸ+,õ¾Â¸/ã&‘¯•A(ÚâúÂ<©Bëm)ŞØì·ï7ÅtjÅ°¬¨P?{T;Å®¤3|fÿEû¶ÓîS›Ã7çD—Õ‡_.™Ï;}*·5>|n›8¡ÎµåÎÿÑØh³ÆØE<K¦+öTî#¤4q§AÃ[Ì˜éHKà{€“&¯{ÚŸî+“O“ejqÚ&Jis/ş×a$‹w½°+a9ŒW²ÀäÄÒp.ÈÈBš h¬O÷fˆ*ÔïıIBŸ[Ño#ôY¡Ğà×~…~/èU¡•-í‘4ÃêøÈQÕT:ërY•ù„£%©Òı@±e¦%Ûåeûüú‹çYĞ£õñâ]šÙ£7š;£×K8…º¦œ6KmEÊÍ#}Z¸…›tÈ€²ñ7˜Ä:Àe(¤^a;äkz¡ÅgØ‡‰/9/ßv¿F2%C[dŒw+ãø3¼A³?¹U_İ–Ú‘†s“îTàa9cˆ‘:|dÆ)Sï)O3ĞëM!ƒF0GA¨²Y$Ø“9nÔéí‡Ó~\ıLf-ùàÕªÀÛ­½Sé&½±ø+½Ã“V\e@„œ{"`Ÿï­ÌškÃ£7;ÆŒcŒñ,Æ`şµšüöWàjé^xÎ&[shi˜È'Ë£*ëÌí¼p*æËÆpÿªåU¾™|öOxü1í0ˆ×X‚ˆ³ÏV_ÌÒ²à½NÏ/pî«íçR¸	´+
ß—Íï¨ùäö²£¾4-UùõFdR"K/ÜD;yãà>‚Ïÿ=[VEyÄYM[3`àxş>ËĞ¡”?Ş@÷å¢V–Æ©7ÅêÛÙYÃŒş‹¿zu ãá{=Œ äOºÔD¢U¡âkeË}Úó]FäT,qba¥Œ!@=g-^39J”¼e0øúV£d1+£í.²D2LÃe1J’QÜô3šÿƒæpq£‡ŞùûF£í&¦“áÂ$è…ıÁA¯«üHû«ëºáñf t¦¸y«)ÒÃ"^h¶dŸ$FÄÒ}t=ğ§ÒMOœ;ç®¤Ì#¬Z–6úÈå/»¦Õê?‰å¿ÜÉÆxŒt…OJÈ§Çû=Y®®¤CË’^ñÎÄ•ŸuÆB•ÀÑ¾u„“ËJ²[Jz„’]GŠ2ìiÆË€|¶h?)æø0.1¾oéùÁá«¯Gò¿h<—€·}&¹$ÁµÇüŸ|ƒ¹
9=Æû/ÌŸ!ü}5Ÿ4oè»ª>Şs08L´ÙµÛƒQ…G—Ûtì(ï˜u“#ÁAƒ¼qWód4D¹ØââÒGK²br:  °ïY»<ÜªÅ=î£œ.b£ÿ¤1×«`w¿«'ø$l1O•ø&z]Y$M÷÷UU^®Ï‹Ô?ù 2øUF2À«tÒSVe†¸¿Ó¹ùÿ+ È~³rU€hî!Š Ü¨
òĞ†cİ'‘F"zÇ€Á È7ùú¯DñWÒªºZºÉ6?JÖÈ=E„ÕÊmSês¥–q?tM´pS?˜Ò}%Ë)¯ìoÍ¢¾­K¿,g¸ŒÆJk=3i;fãEÿã7çqâ¥Âës[^'Š–8sM¨VØ8ælÁ¾aŒG·7˜„w·ÓhQ
SˆZÙ+²ÒHL€\›•w]'ÃÄÑR÷÷¯t+bÎò€ÏÓCì´|O´Æ¿gëÈˆRÔF•œbRI¶¼ØÖ×‰ä†~¯ôhºvÑ¿uleÎáùÉ|¿`X¿µÈHuNízm•¹Á”Á`@Q´r²ßÔÂ]àdC‹ŞÉPÛRª|œU±„UÆú‚;IQ‚'“Òš³§_~jğ¨ÔÉy£¢t{ç€ÜlLHñ#ûÿ-ø&İH8’Ù¥.ˆæ¶_ED®‹ã´×ÓÍó0énd‘{–Şà|¾ÜK-³Yá>!æCCA%pÓm9şÏøŠ?İõH;Í¦Öü¬ªûvª=åqßaN=¼P0·®|rò  '¸cÈ·iGO@šÎzƒ˜Ó’ùb„3×
œÆODÔ@g8B”RõZ†dk›¢aƒcÆ,;+ Îz'â^&K:ôÇ<ïıÚF0Zåö
 s#lÿ€hºÈJämà¼›Wsìé+d´·dã²$Zn]7ØÔ7	äö£|®ç‰ê5BsKNznŠ;üy“ö¸³	åßÓ*ƒÑI®-w2°ƒñ#Ög Àtk|ñ£Fåâûhš¼<â–í†îêyÎ­W4‡×0Ôª"s>ñ’Fä€àh¸ÇZÎÃUˆsÏˆ!g,ñ°Ò&šÓp¤?X3`Cµ¥¢šš†à¾{WßjÔt‚³¬‹hô|c9P¾Ç‡%æÇÈYq«¿„WÅ8ï«?³óË“Jb@%+ˆ<¹cEÜš½®±²j?WzËŸÎ}DÖ;£Z«ûÕ€–êQ_Êÿ™ç6DLd©b¦cfıéëX|›µ>‰Íê¬%úçäg%Ò‚l•D^«,"V£òY´¿ZTôf<Î¾—é‘%*îÿm5Vİsñ.æë™’#¢XÜHŠ ²ï¡cI jê!ıµ‹Í¢ïJë"¾,”ÂĞÚq¸ÿ$3± ;ëƒ
63v$Æ‚üòu|®óëv"h­ØîC±ß}ë+e_KvTqµŒ·ÚõHZ×½Á|œª÷Shó{LGùÃ×ÖùØ`÷ÌJ&ŞÅq?Z÷„VC¥0rÎ%4%ŞvèÅÍ=k8t>PdïO’8¾Y†Vk½”Ì495 €V¶Ïğa“jLmé^7Ã:¯d—>'#İ­Mš	-ôPÂP…š¬	´Úğ1ë…´¯LC£ñ®iÈˆäß@ü/z£ª¶=»¥P©çÌ–£¸§x)qDë¡J:o·m>$Å@ğã£6B-}aüìRŞûş,¡¶UÔ7q–xQŸ´Eö‡±VöpÈ7W—Âj‹üC½Å÷•DøYwAŸq62W9†Q€:å®€åë€ëì¸ÜŒEy(áY`AH(TBìvØ9–%£ˆAkób°Áöyu•OG?ºs×!ö8æ‡—	C¡~myø¢Í5Å±§@p{-]äÿ«ûÖ¨q±şÎ­ÄBßñ,3ĞâmÌÇcg±¨èÜT&^®ï )oÄtQ6–IŞ«Iİ›ı5Yim’ÁÜLI#*ÎMë	`ÊªˆdÌ4D*¼;l»4BD÷á`eŒH$ôIÀàR°3œ1™Ê¸ò?=tº…‚İë}õH¢oú%|ôÜ‡”]ÕÆÌÚ„Jëƒ§ùz6%~´ã Îü"ÏIí’n½î'éÛ²!È°zzc $·$Û!yw”¥Š0öé)Ÿ¢Ù()â¨Èyh÷öq+†ÀE¬á JcĞôCº¦–t‰tM}²uc¢;b^¤¡&œiîÊ©²LZTë	ıèóÀş@ùJL-3Vz†¦Õ„1¨4®Ô4JËN%·]­¢ºtw#ÊÂ†iŠ
iô>Í„äƒ-•—ëw»à¾„K×(ı3µp9üçÌõOÈ6˜ À…‹Êİ2ÇQß/
&Ó—%\óÍ^Üp ÜÜm8¶Òçj/ˆRÓËÜŠ½$,–©‚ùˆV[Ì0}R¢ßÔªŞk²Ö÷N©šk€æêÊÒx-_¨–o-×¨Û©ğ¯®oÌ$YœĞªÉQ8TÈ¤DHŞE€]7ËG˜ÅGß45k7ç•Pı½s2!¥²0Ÿ¯S³‘o²©™Æ¡š2˜ê1Kà dé~şE,ÑÆOŸBÈJƒ 5ÿTæR‹z•ŠâÍÖFJ"Õ5¯±ÉÀ
pöø8*Ã€&¾,aÊc%ùvBNqkwò¿éè‚%’„U½]¬×‡×pk1Í‘FãF¹¹ux8>~¥ÏÜ
úè¿/l×ƒ¡Ye¿0›şÚÎÃBäıä#Šnæ^÷ îzÄM 6Ëû>ßd_°í«ÍßÙ™;ÿì‚ˆ1;¹ğ…aèÛªšDá\Ë™6s¤˜…K¨Ø	Ìa‘ä÷4	U4ÊQAØ Di0'M@[1º—ââ¶BÄÙ¹°ŸÎ`Ö?Ó]%J"Ô §³Ç.âíT™aà#"òdïÀª`•¬­˜VB–Ív÷]|í:¥CpÆ}£hÈ>fºU´ğ_I¾„í=°ï·7Æ*lµ!±ÒV…;§e§ì[NT„ŞOtÏ?¨†’ğ«¸Ç”ı.{"çÀõÎ¯Ø:wo˜VVºDbÄôÈö?å™Kg:8¶­1´û‡?ë~Hw	ì·ëxÅRãZA`†ë~´5P'yuW¿‘ı‰Ì2—ÕˆL-”~¾Xà) „àˆñÄŒ½x!myı!³½éÙôèÕ±È+¸ë½k‘ñERŠLêSÛØ{ko8=´Ğs9hc’Õ:M
¾³ğ@y3:*ï_ô=÷š&	ÇBg°ğ4Ë¤h~˜Ğ+X®"òµ¨8ÛØ
Ô!fËîòñ®F–«9/²°ê Ïk“õ@Å5Y­½( =ÏÈğUÀKgN°İÀz2jkXø‰“°_òh±Tj%6uUrHÊÆ«fl¦~²˜.‹DüæIGïOÆ›¼åœ_CI!?e’×ú(€ş)˜Kxdùc-şü±§Q‡ñØOÿİ³Ş E*÷#³+;©î½Ä/2ËyêoÂ@V¥—•%Ég¹ŒıNCKM1ÄXr¢ÔÇN/ÎOÕÏåØM1ÔÜ+	éœÈàz„#'ˆÁ[R±ÑĞ+®#¬¶¦AŞ£õcNÈ¸„\İ+'ßi
…H"œ…Ç#{¬Ip~&=ÄijÖ‹–¯IÓùÁ¿fê–JlmSªXmÙ!£'±ãğv=.>¤ÒHjoÚÕs	è”·BÂ½bàÿuA…scÒşÛõDÎ:	X²4Y„pÂĞƒéCWîÚ¾í‘Q,P=)øŞàç/¢Wr@%ùqÅø²vtØ×;¹Áh†ÚXÏ?÷';’&†„ô”^åk¹áXD4B†&+LÄ_”J:¥óüÌ“9e§[ZÏzc÷ù­DÑCÂÏö{oIm.Ê78'™ŸÄ‰vÿ¿3=Ú ÚK±xÍ7¦#¿¹°ÿËª”‡‚|§ó¢ ¨@Áè³›nDÇ¨U HhŒ:ZÓŒqêßÎxrµ>ª1¡-‚ˆè,o¹4T€ìó£Îµ€ÑY£c…BÁË…vœU°j“fuu•'€ä’¿´âúÔoÁÑ|©jd}İFZ‰[r`ó"Ò~‰N‚}e:ğì½„Ê­ÈÍ¯zY\µ–(L¬¹rà$8‘C0<E1ı²ırŸ¹AıRÂ7TQY$ìÌbõ R_i	'Œ6 uùõÌH\}[çÿ>À	!@¯óã
Mc€
ÌZ€4¤Òùƒ]N­|¢ì÷ÃME&°RÑ1á«ÀD˜1¡¹¬Fµp»YÔsË#Æüá|¨‰1ÅÈ\GàÌ/‹æ "x=Sî•W³HSe†øôDænÔÉZKã œt£¤Q
óØ"C™æG$"ŞĞæH^ïa)½€Zá>(:’‰?½Íáá4¢ “³§@ÍùãÕ<İœı`öuwpŸ‹ïô¼ôî”Xûğ[kğ”ècfÍ
„RéÛ˜Lïzø¯í×PI4'¬2D©¾áÎ`€¦ÜS'R	¬ÖŞ#’\=äùN¯1äæ9‡›µÛàõ±˜Ø†–)Äú8Æ"íFmâTh»ÃV¢wp_†_-¬b’†Üe_ÚÜÇñœ+‰^ÛŸÀ£§B.mZ|õ*ÓıZ%Ô6x4ƒŞ÷üÆëEldÚiY
|àÖs2êg8ş?8ûİ1Ií"\*·ä>ó‡#:`Dcv%HéŠÁîÍµA9—ä)L%÷½¹|˜ì×ä€H­Ú1ô§Û‹?Kâ31ãÇYÙ«ÌzŒÚ)ùxòe–±V-,‚ääõD1‘Dú¤Óæ¶&`t<<.gmƒÂ¹‹åÆÆH9×¾=MRw*×Çê„ÅiŞQ5}Â»Ù.¢CàCÍì«:ó,\ËLş
Œ¦Kp]¡£§ö= =Ûâ|ğp¶=›Òa½R*æl“³l˜ND¿¦Oö÷Øfr($i´$¡†‡Z8»RƒlÑ3¡{òÚ4¥>‰…«õä;œ ßx“(<D†1~ÓŒ^\Îw_ğÖ,`ié0ø ñe•¥²_ş“2*dôºÃ~„ß!W`8]¼EŞ…hF<æAğ´Œ¶5AdÌv ,ŞÍ…o—¸Slıå”$Ró×€_Ÿù5³$÷‹MçŞ`±ü3DEœÆb_m¢Y«kHÅaõâ–*s¡lg‰.µòK?ùÂË€Ì;×½
dÏk/V„ØP¡É¡÷D¬ï¯…^QiŠßOår6%'Ã—$¸{<¹+ú}ÊR(¸¤”^¸õ9	F(¦AÍe“^ïîmŸ6úÑÎ;¥¤Ò:„²ª¹.˜¢ä×¯ŒHd-›=’ãê’Æ“N‰¹¤îH0~íê?Ñkx¸/óÅ¥ßaBÀ/+Õ}¾ZB½ÎÌ´R>åÊSkY#bÔUÒ)ãXk`Dº
ùÖœ÷Ûœ /c“y‘B±Zì™?NDÇŞEßÜ%ğq:ä€Â‹$f(H…Ej	ïÚ†9Ã©±ˆm¯'¦â%0%;0ñ:0SÌºZ·œìº²û|É]'Hi½©™_MÛ¿Û¾sKEiˆFº´iÍ»NÒGaœ’®Š™dq“o¿V«Eš±¿äy*îSî^9ª‘#2œï¿“]¯»í¦÷…ËPË3×b¶ı	#.òÁ%ßUº4I|› Nµå« 9i'Y“¤$ådñõœPVî8“§é¦¯Ÿ65Õı[Ùì“à"‚˜µ±(Í¿nÖRY-¦r_º¿’%nßÆï¸©õ)#\DÏK2qÉĞ
¦ÒGÒ®%—úe½3€LrCvMíaú$Ç™>¦YU¯…Š¢¯íõJ …lZ‹±qƒÙ–Üt°K…© Õ3yˆZ¨¥÷˜("”¤»i¸D=8Ê*KŠ¾ šÂÄ~.Œn{!&ºÑ×¸»ÉçİŒ£R+	¦À5Y.Ô¢G £ìğxG¼lÛó 2h^òY
f$ÇCÅÜØ¥à3,™lgşm¯ªx#‡pôÒt¥{ƒ>NÄ—!E‘,&÷=Ëÿ®ˆóÃPÂep¶".g?Ä£·+W¶+EóûñûQ´Å<nFœ{Oª3‡~¸Î¨(Y°\„¨ƒÈó‡McÃÎá'÷za(oñ·İ9H:Ûc…Â‘Ë¬/DÖHÉûö9ëÊ8ş¹_ÛK’]ì¡cı.¯¼‘ÒB€Ò¹¡Õû¬ÑÖÓ¶PœBsŸ²&$CgdS•
µîõ}H/0TÂ:”‹z… aGÎ3øc|¿aô&¾·§´GşT¯si…Ùê„5İsçeÂxbáf¹È%ÙLÌM¹¹üHd7‰I>áL²õ£%±(õ'QCa.Ç æ]ø^/Gåãİ6Wsà±%hö5EÊGKMUĞ×¿<ŠA¿8V,ì…Å ©r±ü¸I¦V¡Å´v[ríŠëüdMímXdë½'Ìîä"kÈ¥¯ŠPzŒ;	äÄä 5è;!ŞÍ„Ğ6˜*ğğB\ÀßèÇ/2ÆÙíƒ3kŠ]0ì‰î[÷s™š˜p3j÷5Ip ½¦ºdµæ£t‚ßÃ	‡§r
#¡)YH÷íb}áNMS+g*Øæ5G™BÖºÁôµªµ7]¢Šáš2F™&FEøZÛ¿s°Wt×säÓ(’–ú¨Ÿp‚qBS®ı)·vX†ñ{"Üşµ‚W€"¨ŒæŠEàD¥ƒ(Ô3»ÉG¦À²«0_Ó1CŞù	=¡8A–G'¼³€zûÀ)î]¨lhúffH­ßŸĞ] ‚&2Ò"6÷Â1VNÖ]ËhùBëTI>±®:Ø>şï€=ÁnÓ¿gİ¾–±Š¨¼€ÁÆCã%àÆŒ÷ÅcØË'TÀ„*IøbÅqVİ¯Ü×¯eõÊK±']¾¬2Ù¾œ?ÅíÍ8¹É<Æˆ¥S†4—ŠzmhiGõ7×=²¸<µn¬¾Û`X„ é‚ƒYóÒµ$C¿Ehé¦ô¡ÓyÆ;ZY?¹·R“´+á0¼vÛ\ÆÎ»¨ÏBŠ8	æ
-ª8‚ãI=ÛlB[7;tŒ >½§ulß4&IÔæ¨zP›4òÛ.Ùìı+[³L¢õ÷œ óŒ¾4« "RÌHÊDÅÄ5»tt}¹]²ß4#Ã’in6‘C¶’’{©‡ë±e­‰´ĞÉÂQç–œ¨I?Gm<#YØBÓHÄşßíEİ1ø“v®>–ºƒ©´Ä=(,]™Dl0¢¢ ’fQùïœ× dTqw ãóôêÊÊË4U1wÙRy²iQ‘á]ònåOÄ0zGZ—a#0ï€Óy»h-Aír»‚
İËøHşOŠ{ş»ãé©ïõTÌĞfC3Ø\Í6KŞx ´ÎÍ¢s&t¬d®mÁÏC›òç˜	â¹FœE6yüßèz§˜ "µ7¸öµ£>ø‹•0
K+f·LÎ/``-Ÿ· şÿşÄÊØzßûNgÏõ}0ŠG¥Ú èU¶L×Z8Şl‹§¨ø ÊÔ\a¶òÃtì9ËNæ¥ Wå´&L¿iß=ÇPŞUæĞ?—)¥É¦ÕúLz_˜îvšÜ™¬v}:»7s±\ÜÜ[iˆG@	Õ(pà+G®ÏÉgŸB+ĞÆmµe¬½ÏÆã]^:¤W;ğ%Îa;É[HoŒ V6íë]®Ò3ƒÕi&ö”şùÆ]]Ê²­—póZô¬®p#%‚ˆ`Ä¢³AoŒFFyµ¥½³–\&Œ&h||ÿ
2'úñb?Ğ\…x$)?¤Úßa{U‹<îÅZÏò({Øù„P³×"ÄÕ/Ñ[—¯”ŒM2ŸçjEæIÖ4‡q¤$ô„¨õ‰ØRhÁ…©añ`‡ŞÙ1+9o¹O³‹~x…-¹ŞÑØ>9–FÒO{ÖLíÖšó_x*'ÆIüûAKÑ‡·ßç¡Œ3M)İ)¼ë%®‹aÁÅÀû–šKf"OûsÙxıÜpY¨ÊgØ*5(ßÜîâÜo¯kJî^t0ÆÒãÔ9DçR4Ş ãßÊ;aÌgÊÁ ToX;¨b’¿#´½/µº¡šÛåFáYîGõş}¬VˆB’üód  jwe}…sˆ»ƒR_µ¬ÁúÈ$è„RfÒé[ap5ß…D‘65ç{È\ÇQŸ ƒÌ`Ÿl4ÖÎ…¢­G5"ıÂäs/ĞSaÄ•6g—Ù5OÙ'æÖt}céùş_ù‡n± d¥s…ÔĞ3}¸aÃ&0?Ô'»öBªJ9O$ŠP ŒwÚ1UXLLÆ­šêñzthI7¡K™çvëÑDd­Xƒ½Q|œ ]"_®3ş2q‘=U»‚%LÂ©µú.NêƒN0-BµÄ„KçRŒ2à³»ói=*Ôz@Úa¯Xùu#ÒÅX–fß©*¬µÿ=÷®n DÕËª|hª7N‘¹—/±|!’Œ§8Iyü­ˆê™Ì7µk÷«7c±ÆÔƒ¨oJ²†Ü3húŠü +é}¨E.[ÀÜWÙ·N>¶·‘ğ¯¸¿)ÉwµÀ`qêŒÈK%·…i˜’•rœ »7 c!•dšÉÈ½¸lˆş)ÕöUúƒÃ5`âzíî™O-ì).ˆxØx[«½yh9xÏG5És‘°JĞïğ«~sW‘pZRõĞxN}ÑŸ÷cyv§á%
·yŒZ—4¿jÂÕTƒ$öüˆv½ã$]L(ısêLtĞÑŒµ7.ÖÔÆÁÂÅı÷Ş)4³ëW»eårSØĞÛ6Ö<Ï³™·râƒ•6¤—@÷ıÆJèTt¢0¶¬iÏrjŞçÂáİ¥êE–F’)ÑBêŸŒ¾.¢¦Ró¶ÕN•÷R´© Äµ¿ Lœÿ²6•Ï~Ò¦ ºÕ˜U…tmòøí¹+t\»ÉjÄí˜R`bR3Á˜Bq¿íÛ€s"À\Å7œÃeæx’©y>,aÌ´1Ò²D>áœCÃy4J¯¥¼W×¿8ni'ÎA’Ck²xÌ3¾wékÇ»LÑ¹~Ê9øşî×+Q&åÚäÚµ 2Mù@æÛõ
£r;øªˆ¶×àÉÜÛ¿¾¨#:¥pÆIêİ´ÑÌ³Ïš/4¬wÈE‘™–¾6-€Ê+>5o5_ŠMl¤Õ¦¼Ë3°í^7XŸ3ÉèÖÔ(öb‡6åaàÃzPü°ª›ÔÄÎs×éÜ%`nm¦&ØP‡,¬Ğ¨¡û‡öÒ0ç.:bQ’QV¶Îåœá˜ìwĞ+áñÇÈİ×™âiKÓ®yHlÒ`mXµ²Ê»kBùŸ°V¶|±»*Öı”…†m¯¤ûSt8„®îwùäü!©-àEh`ÙpöM«Ò™,§Ï){©+æ}İB.J‘³zäÖf/‘×ğ{ÉóJ!è7D	"ñÖYJOcÇåñs‚[b~ÏÎPo=şdrî“»òÊ°iÁô_÷§	±ô ÈŠ!bÆ—‡U—õMym	L±Ø7Ú¶òï–Äšê®„È¦n¯ê¼¼ß,;ÔèÉú±ûpœW2ÒQÿÌ0|3Ü”„Lv,zŒCÿ…m¾ààá”f¬¼¸›¾¤cCîâ¸ŒPõŞˆ3
‚İ‚mò}$ÀÄ¦*?v—»ogÅ¾g'$šSívU‘R´–Ç¾ÆYF5Çì6…m2JÂª*Í½à  ¦2……ŸïzÚûñpK…aÖ£‡…ÓbÄ×:†“	CŒ¾YÁ+ª².+IEš´µX¨ÛH¯Du¢Ç"`%«âCÿÀ¯sXİş®UïÏê ˜:Ê ?j¾yPğÀ	0Iéñ6º¿×ÑfQ`ûv L·ø^{™ÆLXÓI ¾K~İ Á[$±ÌR4Êû'tæÆ‘ç·o´ˆ}ı\¬`pX8çnLL¯-ˆô…ÉiÂWØ#Ãoß’ŞÍ—Ìµb£¼c4gİ•R[V&­Á¦ äaà…+S—Ñ½‹ˆ&Íeb*…ÌXƒºö@tñÁH ·êCÛÅæ­6éó dÊbÕ4#ÉjDáÓtX9J,1Æ]‘_4@³/òÍ÷G²Äl™Á½ëö›7è«3%€‘.ÁAŠ«¨Aw*}«~êäÚwS 4hhæ}J|vîû#hs4ÒÏZ`tòÓyöMÏ‹ÜõÙz(Ağ’x:ãuTmØà…m„¯4>MºV=Àku$GêŞÖ6{l»¡‚à=73ÍÁŒ’Sıİ	U
†¡ğí‘Òø˜Á@a–Ñ‹øÂ£¹·™Ä÷#*±Ë#?ª°=µZ‰â½m¿sÙ±Ø´Aı8e0g÷âµ'_wª#(l§#~£_Q^Ë‘uó’§
 Ö¦~h°H@Âbã„Ğ® ¹g¢ ‚xjÑ{Ù‰(‰?åš)•’¿)lÔåX5•v!æ†Ü5ÿ.Cİ¢øXô„»ğCï"¼Ñe‹ÓXËHñĞTZ¹×8h›N©I×Ø,çÙ+pvµ=Ëc3kxÆ˜çKY*ı?Å¦gUŞ°ÈÏ¥ëaÊìu‘
L†y+è$Ş¯­ €r!»åŞş¼óŠªRv§Ëñ¥
´×½mÏøşøÿHš8.4Ø /y¯¼ ’Í<ªZŠOÙ†?b¶TÇ’¢"tW¾ç*ï¹¸\¶2T>Ã[€5Ï(ñô"9ë¥ i,-j§ä¼Ö„wîp	´O¿+GSv&‘¢ÇÍ‡f›!ÔÒ7†¼Ç<)…¡—aˆÒ³u^¹„˜	‡ÙÇ°Œ€$ÿ PØ¶HÊmmÜ«Å`UåjKß|!EúMU@a¥:RyTx&ş±svfâÔmhÂ†ÿ(CØÙ´J'šl/¼:ÏÔ½¿Ñá!Ä	ôo,¼÷æ)ü{-{#ôäşúFº­'çÚBfæ»(bQ{4“sû¡ûsÿ uÜ“âEˆN;Ë—fzgzÎ~G	½ù]7céuªt1":?*ŞÍK„ÁÙI¹ÇŸqZVƒİ×„Fñ‡6_óP‰<HŒ&åöBıN†ˆ‚”‡Å‹E“Ú!¸Î);7ĞTÎ"ûé«({p¯æĞ¾–ès[‰3âl¥T†Y‹X–tÚ6A I÷¦ş‚¼UØÁ}Q‰ÄRº\Ùå§ÿ³88kŞ5¬¯k_Xw<ñkxDCúU«ÒÒ]ís#€Ál¿FXxBUh˜*1éu"*ºKŞ`¯¬ƒ40>÷…ÌG[œBªğäÁ&Ñ2Ş€zˆğÛyw<ÿç®õÈvsÖ=úæ$öÎwF´ø†ÕÚd¯‡<]ÕÜ”ŞÀ…•­±ƒªç·†Â`Ã-wœjg.ô?P¹Ë:å1­Ä"._¥	–»UÄ•ÿI£Š;M,S
3ïk©\Æª©}»k>Ÿ ·ÈvÏ#×Ú¥wf³€§×èoÛ\|siJïİp¹İ5‘º±-À]ÒW }@k¡¨‡<–Gsg=Ğ8â£DJÀŠCâhæbŸ¶°ÃœÇÅ°¸ŸH«ÒJ‹Êş4Â¢#ğà#–#0Öì)>¹šÇa+ö¡÷…dè/dExšéİTJaJS5V«e	¥3w ’J-Z3â-£E2sğ.ruìõ´‡i­OØ²tœ‘Ïö>“ì¨DæÍ5¨›«™işë‰Lvùz{h0µ¿QQÆJ·½õƒ%¨Ø¼®DkúW;{Lù9òlÍ-OBe]XÎå›c±5 dw&JÙ¼Mü™|{'©ÒŸÙïÔd·×I€åê¡Ö³£±dØŠL¹6Àà|ÿ°Xœºmg³og\Iw´¨²øÍ¾êt¯_±ı‹£0ŞÂGaáÌÒyR_5Ñ¼¿©Ê.¬„BÌ—‘$öRƒƒğòD<»é~£¢§”4\%ÂŸèÍYeÑ4ËŠÕşË½©‹¨ÿü¡§n¬ã¤p9Y”ºczÇ#Pô-ĞÊò{pîòêCk¶ıªˆ3İ `_ ÑQN%nófôö™ßJ•'Šƒ%FZÀ¸Å<›µ|f:E *Íl1"dô|e‘Pèƒ);ç«º‘Ğ^o@µğ“|ıÕ!‘¾>º{¢½çôCY¼>DP 5;/Lÿæ!N4ÕT/~›µgÀ6*–_Ì	h°‡ñ0yĞŠKÑb‰]½M0wıp®ÇÄ0¿n'@¥ÜjèÂbæì'SÓÁ\oè¯‚¸)PÒqYf°§©"NXZÊMi9Ù¤n©9úæÇèX —ş(¯üÎRGÎjåÁ'^DZ¿¡{!¬­Ò+kRøh¦HTõûfı€Åqg8Ñw^›îÉÕJ‡Á ô75èæ¬Œ<,JÂŠü—ı•àwb	¤pèr^şñ†öç£ëèi2.Œ½³=usJì,‹ûÏ¯J©2d¬êOlIeğå›¾}?â„€ô¶ ¤0…\hÊöŒ¯’0Y` ¡- ûÉ
áÙ+ã&{ûÌXVPŠÂ4h~„Uw²Ú¥ÕIL¶b]ò©nÊ²¦µ»~¸éé’£¸Òz-dDc.mXĞmU”+„G9•*rœ«/o¦Ú‰ :Ì§uÂÕo²¦«ù¥27	ïì¨K¶†Wdİµq+şüKT;[Şî·Ä
§lÂÑän¬²|‹ùJ6Ì[íÀÜÍvj>rKDùèŸT‘r¸@—gÿú;£e k9z{jÌ,ğ4ì¾QˆãÂ¿•ar±‚vm«ü"…Í¡“™:t£< Tbß·TQ~®Jìu‹N‰ç}r‚±jM'…ó…ıíü‹èŞŸhüFÇé*®ár;àÖv$ÚË¸zlÍŸ l‰ò»'ÀÈÂÏÆ5;ÅÊyJ0ÇÄ7Õñ1˜ÓIú0ˆÄÊ›vo>8_Ö…Ÿ&i™C§n¨BæXæßo£÷a\“?´^§ãìÁõ—8¡hÁ/È·İOë]åèßÈf3Ä—V0B¤7}Ë"wPÖĞ<¥Ñı[v‰]¤°ñQL2î(mf\iŞ¼
h‹ãÖoG ÊŸtÖ®q÷D2âM•ó½¦~¾çpó=Ö‹B>¤‘ôE»£€–UÄQáó¹?¬+Ü
:Š`¤K>D
-±ñì!Vî±ÇS²ÇÀ.ÒrCúŠFL©x	Ÿ`YWÛ›×¨tñ·•¯ Y
l2´ğ‰Âülñİ’Ô®£@3H ZÍz µO´eKËdËœ}™¨-tIK‰²^Ü-¨a6GQr/D¼AÁ«¡MhÄ!nê<òZ„h|´÷åèÕš0 ~(myû³«Š¼`é—ä×Ìóv¢5÷¦}ş<6€Fà¢H`ó˜f¿oYškÍÃ(¼õ»5E\IAÓAµèTB^b¬O¢À‚ì¸+h'ˆ–àX/"üç=öŞ…ŒâÒB¸b«~hº­ÚÏ¼¿[Ô| c¥|69œR<@G¶uİ;µsÌ>oee¥K¹¾ÃüïÓú&mrĞ?ãï¤h–CMĞÌåÊÙX3Ïoë1ƒ*UÂö¾/
äˆ&¬½òd©ËçˆøNÜFeùo+üEÓ/NDë™ÀzåAkµe‡¥lcP(Ã—j¸T€™JÜ’´…´fkNB~˜µ¥Rİû¸óŠ¡•Â?Nº6·œ¹ıúğô¦:ç²|šæÃDÏ,—B´Õ‘Y_õ‚¢ùù+ı!æ xßOÚøïz›¬€–ÄÖçVPY'¦„Zª×øÜÈ8p]1smñbOƒlax÷ªÑKœ»IèÕ÷[¼
}ŒZMÏ‡^¦)A{ªwg<¨ùèƒİÈ€óÌ´o³åÑ¨/0Ç»•Š Ë)Ï'usUá2.®Jºy;3Ä Ün\ëâtí¾ºmíïYkÜX¥}X'51Ë‹Á"æºx1“-©Ÿ´Üì°fö¤V–vŠgw`ŞÖe¾Ä†•¬Vù9 Í.
È¤CXÌò>Š/¨L)¿Âı6¶ÕTÒ„ÿn‰ÚáŞ1ÄÿHÂ]{ÔmÄ¦3HAU¹ lˆªÖĞ¯ò«g44À|ä$@w†T9È ¹AA,ŞS4ªoâÀhp#Üï˜¶‡C'ì»E…—Zt|©pÈÊ3ñú¸‘Û
©¹AEhÊ®åiƒqÜH23ÓY®Ôı’3O›Ü‹äŒÜ0”DÅ’Iæ_^c(4äfá€¾Ğ¹ö¾İyk³9;A6úMÜçÿH øğ`M+©}ê©ıl³7-XC¢Â$–%o¨*¹sCQTîeY 0Ÿ¸}=MiÙyÄ‘	z„ˆ·›m@²û8­Ã+İp ³GË\DÚ4Û5/e‡ÕşPvá 	¦Ñ‰5Ux&ˆ“}š%»‹òNv}úÕNQÄq~s#ÿÍoáŞÄ´Í7`CÁ‘kÿT€fºx$˜r"ªÈŠvvwØ\ÚlÍã^?¶¾ı/Ö9ÔGa®J´Ã®Gt m‰zwş=Óº)
æ„Ò)pƒöØ…uHª‰á}‘3„ -0`#3×Ë]XiĞy÷Á±µe”*®!~³»—*“Q•ùÒRŸëEsÈn¬	u)÷w—)~Œb¥/N&çu!l:(O¿y­±o7sJT	8H[ø±š=jäœVIŸ$+{ ÁSw)‚&ÑN¼á_º±ù0ñlÆbICa&ä|KQm¦>pÃ±^²óÜTŒ”‘—ûÏB–:¯úı¬æã¶z.¬W0İã\U@< şiEvìjäÛ†_4i0í‰>ôêZ‹Ù@u€OÄÑ€Òß~¾æJÖQşWÈ®5Tl$ß0…DqÀ©ã€cÆÀkÉ»œiİ¤ÖW†K¸Ï¥£/Ú8J¬:óŞw
6ÁN¼ŒbÒ}* õºÄl»$Ç°Â†äÓWö¢0Gl¡ û¦O¸39›dT¥ƒ¿Übï»ˆxâ(¾´ZØ…¹GÄ¼ŸB]å Ô $¥&?a!e¢rÅƒÕq×liQ:ÿdšú¹èš”†÷§`,ÈâÕ˜x4zTnHps´d®‰_e¼p¹çdé©¨&¨(ğwòìÔØä½ı-şq©
ëÕö°Fy˜ŠÑ!‰²
¬ÂÌ'ˆÑt†hÓa}”Ù˜Ÿµj¬@€ànÈ¡#ğCÏŞı¦aÔr.¯[´¤…Ç¸c–#j²ñ›õ4†÷‰/AyJW~½,ufÔ6"Ÿ½–±À‚í-N)ãÃÎÊêĞÜ¬Vİ£Ğ¦<ïm%Âˆ”X(e¢ªu0Mğ‹œ DfË‘3SbµI
÷Á²´6!ïm…Aİ0z ƒ#N°  †o
äÒ
¯ÏÆ¥(P<?øËù=Rã jĞÁ¸¶ŞÔù™B˜Ë]îµäägcŠämXáÆñß“@†Ù–{í‰–J3.fXƒ!ğp¦;8uÃšÙÆŸ£ÔïY}§$|dtnŸ!àÈ¦mF<-*káÆöÒóKê“… £ä/Ïî•Ë-É‚±¦ÖÂ§¨ÎOM{N†ÙÇ¢»˜kX™ŒVI‚L‹ŒNdhŞH—E…¢¶³†›øÀæàõËZ1
I$®ÅQm´xšüÀc-Ñq\Í®_m¼]0quÒyËªş‘WÌ\ˆYÊP¹AÉôt¢«3@êÍULpÿ» Ì{ã¤dë$Â(!í´´I¤÷‰,áİ–I7l;úmn¨iæ""]”÷ŞmÖ%R1',´htV«É¨Ø.QèœŞ’y!/\<qŞÍ·ìæì?À[çNÌdì½ñ‰¥û=GmåQ
||}x9VºÍTP´ï³ˆ;9•d!Ùó@	$3çàQtÁŞ†fwÎ c7Ú#šZq…3¡«¥Cñ†ö»I¯JæQFVuD´üîÈ}¯-»Šu²¥I.çk"¿³½v#ÕíŒ)}Î¬›'çHVO™/4`aşì$…ÊÑr`ı25x%Œõ£`ÚÇö½CÏnã£YĞµõt/I¬ÃÉ¥$”y0cŸfr­Zj]KY4`<=ØyõJpRXÇ©¿šÃ§D<'b%ÉåÏU+ğ†Z×‰·T°îÏeÕ îˆ«î§ºæí~>snn–Ç0Lló…*¼ŸOÒ^¬såŠœ6Ú^o‡hËÇ‘P‘yM¶p"¾“OÌ
	çıu›ïRKT¬JY[V˜Ok™ÔLCâ£3P÷«’K®ÙÁÆ`Í¬WehùË:l]²î‰Grî`MÌóˆƒÜ«ë]œQÕ5E©)&|šÊùÍ¹¢Ä{™jpÚÚA¥; ÿŞ;_ã4ÎqyÄäk#X…À]Gşk< \Øë÷ÆLY´!Éùä²ø~
XãH{÷=lªë§Š<DV‡¾•1Ñ‡Áé»WG²ÂS?î\‚÷#[¥–˜^Nî™8û"HcŠ²Ø9”EŸşt¼úY	X¯ØÃ8Z¾:4	³ù½C†ó÷w­Nb>cõIl–Ä+ŠÇ^çhşÇ“ZêtG&¾œğû¸Ošî³¹$~.ç#CãÓÊËoè6.–‘ª+,&ÈmLQ3ÙÏRQ_ÃRíR•º…˜zÓãO½<j‘*Àw¯ºìı``¶“
VÂ¨|¤ÜÓc¢Y‘ñ1Qd÷«d~7A»RiiXWìEˆIğ%Ù•‘FêÀÔCOE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/NET_L3_MASTER_DEV) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/DECNET) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/MCTP) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/net_namespace.h \
    $(wildcard include/config/IEEE802154_6LOWPAN) \
    $(wildcard include/config/IP_SCTP) \
    $(wildcard include/config/NETFILTER) \
    $(wildcard include/config/WEXT_CORE) \
    $(wildcard include/config/MPLS) \
    $(wildcard include/config/CAN) \
    $(wildcard include/config/CRYPTO_USER) \
    $(wildcard include/config/SMC) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/NET_NS_REFCNT_TRACKER) \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/XFRM_STATISTICS) \
    $(wildcard include/config/TLS) \
  include/net/snmp.h \
  include/uapi/linux/snmp.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/IP_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_MROUTE) \
    $(wildcard include/config/IP_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_MULTIPATH) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/IPV6_MULTIPLE_TABLES) \
    $(wildcard include/config/IPV6_SUBTREES) \
    $(wildcard include/config/IPV6_MROUTE) \
    $(wildcard include/config/IPV6_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/NF_DEFRAG_IPV6) \
  include/net/dst_ops.h \
  include/uapi/linux/icmpv6.h \
  include/net/netns/nexthop.h \
  include/net/netns/ieee802154_6lowpan.h \
  include/net/netns/sctp.h \
  include/net/netns/netfilter.h \
    $(wildcard include/config/NETFILTER_FAMILY_ARP) \
    $(wildcard include/config/NETFILTER_FAMILY_BRIDGE) \
    $(wildcard include/config/NF_DEFRAG_IPV4) \
  include/linux/netfilter_defs.h \
  include/uapi/linux/netfilter.h \
  include/linux/in.h \
  include/uapi/linux/in.h \
  include/net/netns/conntrack.h \
    $(wildcard include/config/NF_CT_PROTO_DCCP) \
    $(wildcard include/config/NF_CT_PROTO_SCTP) \
    $(wildcard include/config/NF_FLOW_TABLE) \
    $(wildcard include/config/NF_CT_PROTO_GRE) \
    $(wildcard include/config/NF_CONNTRACK_EVENTS) \
    $(wildcard include/config/NF_CONNTRACK_LABELS) \
  include/linux/list_nulls.h \
  include/linux/netfilter/nf_conntrack_tcp.h \
  include/uapi/linux/netfilter/nf_conntrack_tcp.h \
  include/linux/netfilter/nf_conntrack_dccp.h \
  include/uapi/linux/netfilter/nf_conntrack_tuple_common.h \
  include/linux/netfilter/nf_conntrack_sctp.h \
  include/uapi/linux/netfilter/nf_conntrack_sctp.h \
  include/net/netns/nftables.h \
  include/net/netns/xfrm.h \
  include/uapi/linux/xfrm.h \
  include/net/netns/mpls.h \
  include/net/netns/can.h \
  include/net/netns/xdp.h \
  include/net/netns/smc.h \
  include/net/netns/bpf.h \
  include/net/netns/mctp.h \
  include/net/net_trackers.h \
  include/linux/ref_tracker.h \
    $(wildcard include/config/REF_TRACKER) \
  include/linux/seq_file_net.h \
  include/net/netprio_cgroup.h \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/SECURITY_NELF                       Œ      4     (            GNU  À       À          èüÿÿÿƒì‹    …Éüÿÿÿú   t2ú @ uÇ€x     1ÀƒÄÃ´&    v ¸êÿÿÿƒÄÃ´&    Ç€x      1ÀƒÄÃèüÿÿÿW‹JV‹2S#°d  #ˆh  ‹˜”   ‰2ƒ=    ‰J[1À^_Ã‹°d  ‹¸h  ƒÃ‹€T  èüÿÿÿWVPh    Sh   èüÿÿÿƒÄëÈ´&    t& èüÿÿÿ‹ˆL  …Éu‹R89  t‰  ‹ˆ”   ‰Èèüÿÿÿ1ÀÃèüÿÿÿUWVS‰Ãƒì8‹³”   d¡    ‰D$41Àƒ»P  ….   ‰ğèüÿÿÿé.   ´&    èüÿÿÿƒz$u=V‰Æƒì‹€L  …Àuƒ¾P  t,‹†”  ‰B4‹†  ƒJ(p‰B8ƒÄ1À^Ã´&    1ÀÃt& ‹†”   ‰$èüÿÿÿ‹$ëÁ´&    ¶    èüÿÿÿ‹€”   ‹@\‹¤  …Òt'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿX1ÀÃ´&    ´&    èüÿÿÿƒì‹    …É  ‹@@‹¤  …Òt'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿX1ÀƒÄÃèüÿÿÿWVS‹°L  ‹˜”   …öu‹°`  ‹¸\  1Î1×	ştJ‰\  ‰ˆ`  Ç€L      ‹C\‹¤  …Òt'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿX[1À^_Ãt& èüÿÿÿS‰Ã¹   ƒì8d¡    ‰D$41À·CÇD$    ƒ=    ÇD$    f‰$f‰D$f‰D$f‰D$$D$2ÇD$(    ÇD$  ÇD$   ÇD$  ÇD$   f‰L$ÇD$     ÇD$&  ‰D$,Ç  ‹C¹   ‰âèüÿÿÿƒø…¯  ‹C¹   T$èüÿÿÿƒø…¯  ‹C¹   T$èüÿÿÿƒø…¯  1À‹T$4d+    uƒÄ8[Ãèüÿÿÿ¶    èüÿÿÿU1íW‰Ï¹   V‰ÖS‰Ãƒì(d¡    ‰D$$1À·CÇD$    T$!ÇD$    f‰D$f‰D$D$‰D$FˆD$!‰øfÁÀÇD$    f‰D$"Cf‰L$‰T$ÇD$  ‰$‹C¹   T$èüÿÿÿƒø…  ·D$fÁÀƒ=    ·Àâ  ‹T$$d+    uƒÄ([^_]Ãèüÿÿÿv èüÿÿÿUW‰ÏV‰ÖS‰Ãƒìd¡    ‰D$1À‰ÈˆT$fÁÀf‰D$·D$$fÁÀƒ=    f‰D$
—  C1í‰$j ¹   ‰ØT$èüÿÿÿYƒø…R  1À‹T$d+    uƒÄ[^_]Ãèüÿÿÿ´&    èüÿÿÿUWVS‹p‹X|‹@(‹~Ô=	˜ „P  vN=	˜ „“  =
	˜ …  …Û¸   ¹   º   EØ‰øSèÿÿÿ€~~ Xuut& 1À[^_]Ã´&    f=	˜ ti=	˜ …Æ  …Ûƒÿ   ¹   º   HØ‰øÁûƒÃ€¶ÛÁãSèÅşÿÿ€~~ ]t®S¹0   º   ‰øè¬şÿÿYë™´&    fS¹3   ëà´&    ‹†Ü   ‹–à   ‹-    ‹@|‰Á	Ñ…  ‰Ùº€ €Áá)Ù‰È÷ê
ÁùÁû)ËÁã…í2  S1Éº   ‰øè<şÿÿX‰İƒÍU¹   º   ‰øè$şÿÿ€¾„    X…Ö   €~~ „üşÿÿS¹   éIÿÿÿ[º€ €Ááé  0 ‰È÷ê‰ø
Áùº   Áû)Ë¹   ÁãSèËıÿÿ€~~ X„°şÿÿS¹1   éışÿÿt& [º€ €Ááé  0 ‰È÷ê‰ø
Áùº   Áû)Ë¹   ÁãSè{ıÿÿ€~~ X„`şÿÿS¹2   é­şÿÿt& …í>Sº   1É‰ø½   èHıÿÿZéÿÿÿfU¹@   º   ‰øè.ıÿÿXéÿÿÿ¸êÿÿÿéşÿÿé½  é  t& èüÿÿÿ‹    SXä…Ò]  ‰Øèüÿÿÿ1À[Ã´&    ´&    èüÿÿÿV‰ÆS‹X\‰Øèüÿÿÿ‹ƒ¤  …Àt€‹´  èüÿÿÿ‰ğèüÿÿÿƒÀ   èüÿÿÿ1À[^ÃèüÿÿÿU‰ÅWVSƒì …Òu@¹   º+  èüÿÿÿ‰èèüÿÿÿƒøÿ„t  E¹À  ºø  ‰D$èüÿÿÿ‰Ã…À„f  ¹   ‰êèüÿÿÿÇC   ‹¸  ‰ØÇƒÔ     º   ÇƒĞ     Çƒô     Çƒğ     èüÿÿÿ‰Æ…Àˆª   Çƒ\   °  ƒ¨  ¹   ºF  Çƒ`      Çƒd  ÿÿÿ Çƒh      Çƒ     Çƒ„  ÿÿÿÿÇƒx      èüÿÿÿ¹   º   ‰èÇƒˆ      ÇƒŒ  B   è¥úÿÿ‰D$‰ƒ0  ƒøÿu&¡    …À¦  ´&    f¾íÿÿÿƒÄ ‰ğ[^_]Ã¹   º   ‰èè_úÿÿ‹»0  ‰ƒ4  ‰|$‹=    …ÿ}  ‹|$ƒÿÿt¸‰D$	øt°‹t$‰ğƒæÁøN@ƒà‰$ƒÀ‰D$‹D$¶Ô‰×ºÍÌÌÌ‰ø‰|$÷âÁê‰T$’‰úÀ’)Ç„N   ‹t$iÎ'  È‰ƒ8  ‹D$ƒàûƒø”ƒ<  ƒ<$FŸD$
ƒÿ¶D$
Áƒşˆƒ=  ŸÀƒÿD$¶t$ˆ‹>  	Æ	Èƒ<$C‰òˆƒA  ˆ“@  ŸÂ‰Ö!Îƒ|$¹   „  ˆ‹B  1Éƒ<$CˆƒC  NÁ¶T$¶L$
ˆƒD  ‰ğˆƒF  !Ñƒ<$Gˆ“G  ”Àƒÿˆ‹E  ”Á!Áƒÿˆ‹H  ”Á!Áƒ|$”Âˆ‹I  !Ğƒ|$”Â!ĞˆƒJ  ¡¨   ƒøv ƒ<$F~ÇƒP     ët& 1Àƒ<$CŸÀ‰ƒP  j »À   1Éº   ‰øèüÿÿÿ€»G   ^…Í   j ¹	˜ º0  ‰øh æ  j h  j hÿÿ  j j èüÿÿÿ¹	˜ º0  ƒÄ ‰ƒ˜  ‰øj h €  j h  j hÿÿ  j j èüÿÿÿ¹		˜ º0  ‰øƒÄ j j j jj jj j èüÿÿÿ‹³ü   ‰{l‰ƒœ  ƒÄ …ö„î   ‰øèüÿÿÿ‰ğƒÄ [^_]Ã´&    f‰ò„Ò…–   1Éénşÿÿ´&    j ¹	˜ º0  ‰øh €  j h  j hÿÿ  j j èüÿÿÿ¹	˜ º0  ‰øƒÄ j h €  j h  j hÿÿ  j j èüÿÿÿ¹
	˜ º0  ‰øƒÄ j j j jj jj j èüÿÿÿƒÄ é°şÿÿt& ÆƒB  ˆƒC  éâıÿÿ¶    ¡    …À›üÿÿéï  ¾ôÿÿÿé‘üÿÿ“˜  ¸   èüÿÿÿ‰øèüÿÿÿ€»=   „u  éW  é}  ´&    ´&    èüÿÿÿ‰Ñº   é¿öÿÿ´&    ´&    èüÿÿÿ‰Ñº   éŸöÿÿ´&    ´&    èüÿÿÿQ‰Ñº   èN÷ÿÿZÃ´&    t& èüÿÿÿQ‰Ñº   è.÷ÿÿZÃ´&    t& èüÿÿÿWVS‰Ã‰Ğ‰Ê‹s\‰†t  ƒøwƒùv?¹`  ‹=    ‰p  …ÿ*  Qº   ¹   ‰ØèÔöÿÿ€¾E   Zu:[^_Ã¶    É|‹<½`  ƒÿÿtã‹`  ÷Ñ#p  	ùë ´&    v ÿ¶x  ‰Ø¹@   º   èyöÿÿX[^_Ãt& èüÿÿÿUW‰×VÁïS‰Ã‰Èƒì‹³”   ‰<$‰×ƒç‰|$‰ÏÁéƒçƒá‰|$‰L$9“ˆ  „  ‰“ˆ  ¹   1ÿ‰ƒŒ  ¸   ‰ÕÓíƒåDÇƒÁƒùuî…Àº	   Çƒ”     EÂ‹T$1É‰ƒX  ‰ğèüÿÿÿ‹T$¹   ‰ğèüÿÿÿ‹T$¹   ‰ğèüÿÿÿ‰ğèüÿÿÿ¸»   º   ƒ»P  »0   EØ‰ğ‰ÙèÀôÿÿ‹$‰Ù%ÿş  Áââ   	Â‰ğRº   ènõÿÿ‹F\Z‹ˆ¤  …Ét'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿXƒÄ1À[^_]Ã´&    9ƒŒ  …òşÿÿëá´&    v èüÿÿÿUWV‰ÖS‰Ã»¨  d‹-    ƒìd¡    ‰D$1À‰â‰øÇ$    ÇD$    ÇD$    ‰l$ÇD$    èüÿÿÿèüÿÿÿ„Àu…öxN‰ğèüÿÿÿèüÿÿÿ‰â‰øèüÿÿÿèüÿÿÿ¡    d‹    …Àu4¶ƒ´  ƒà‹T$d+    u1ƒÄ[^_]Ãt& ¸   ‡Eèüÿÿÿë¯‰Ğèüÿÿÿ„ÀtÁ1Àèüÿÿÿë¸èüÿÿÿ´&    èüÿÿÿS‰Ã‹€˜  ‹@‹@èüÿÿÿ‹ƒ˜  ‹€   ‰P|‹ƒœ  ‹€   ‰P|‹ƒ˜  è~ôÿÿ‹ƒ˜  [‹@‹@éüÿÿÿ´&    t& èüÿÿÿVS‹˜L  ‹°”   …Ûug‹    ‰ÃÇ€L     …ÉT  ‹ƒP  €£´  ı…ÀtFƒèƒøw4‹F\‹¤  …Òt'¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿX1À[^Ãt& ‰ğº   èüÿÿÿ‰ğ¹rœ” ºrœ” èüÿÿÿ‰Øèüÿÿÿ1À[^Ãfffffffèüÿÿÿ€=     WV·òS‰Ã‹x\tW¹   º   èüÿÿÿ¹   º	   ‰Øèüÿÿÿ‰ñº
   ‰Øèüÿÿÿ‰ñº   ‰Øèüÿÿÿ‰ñº   ‰Øèüÿÿÿ€¿C   u$[^_Ã‰ñº   èüÿÿÿ‰ñº	   ‰Øèüÿÿÿë­t& ‰ñ‰ØºA   [^_éüÿÿÿ´&    ´&    èüÿÿÿWVS‰Ã‰Èƒì‹{\‹  ƒùw;‹44  ‹   …Àu7ÁæƒÎ ‰ñ‹5    ·Ò…öo  ƒÄ‰Ø[^_éüÿÿÿv ¹    ¾   …ÀtÉƒø
~€¿I   tpÁæ	ñë¹f‰ÆÁæ	ñë®´&    èüÿÿÿUº   WVS‹h\‰Ã‹µˆ  ‰÷Áïƒç‰ùèHÿÿÿ‰ùº   ‰Øè:ÿÿÿ‰ñº	   ‰ØÁéƒáè&ÿÿÿ‰ñº
   ‰ØÁéƒáèÿÿÿ€½C   uÁî‰Øº   [‰ñ^_ƒá]éñşÿÿ‰ñºA   ‰ØÁéƒáèÜşÿÿëÑ´&    v èüÿÿÿSº¸  ‰Ã1É¸¸  ë´&    v ‹B…Àt‹
ƒÂ9Ëuğ[Ã¶    ¸†  [ÃèüÿÿÿW‰×V‰Î‰ÑÁÿSáÿ  ‰Ãº“   èüÿÿÿ‰ùº›   ‰Øèüÿÿÿ‰ñº£   ‰Øáÿ  Áşèüÿÿÿ‰ñº«   ‰Øèüÿÿÿ‰Ø1É[ºV   ^_éüÿÿÿ´&    fèüÿÿÿUº†  WV‰ÆSƒì‹x\‹Ÿ  ƒûw‹H  ƒ¿P  ‹    ty‹‡X  ƒø	„:  ‹¯”  ƒıtoƒå„  ƒû½   Fİƒøu]…ÉB  ƒû„U  Ï  …Û…—  ‰ğ¹GZ ºGZ èüÿÿÿ¡    …À<  1Òë}t& …É   ƒÄ[^_]Ã1Ûƒøt££   ƒø„:  ƒø…œ   …É  ƒû„   ¢  …Û„¦  ‹    1Ò…Éü  ƒû„Ù   ƒû   …Û…˜   ƒ¿X  „«  ö‡”  ¸0   EĞ¡    …ÀØ  ƒÄ‰ğ[^_]éüÿÿ…Û•Ã¶ÛéUÿÿÿv ƒø„‡   ~…É€  ƒÄ[^_]Ãt& té…É^  ‹¯l  1Ò…í•ÂÁâéWÿÿÿƒûuœƒÊ ëŠ¶    ƒûtğëŠ´&    f‰ğ¹ €a º €f èüÿÿÿ¡    …À¹  1ÒƒÊéLÿÿÿ´&    …É›  1Òéÿşÿÿ…É¿  1Òéïşÿÿƒûu¹ €a º  d ‰ğèüÿÿÿ¡    …À  1ÒéÍşÿÿ¶    ƒûus‰ğ¹ €a º €f èüÿÿÿ¡    …Àã  1Òé×şÿÿ¶    …É•  º   é|şÿÿ¶    Cşƒø‡Zşÿÿ¹ 0  º   ‰ğèüÿÿÿ‹    1ÒéDşÿÿ¶    ƒû„Tÿÿÿé`ÿÿÿ´&    v ‹    …Òz  º   1É‰ğèüÿÿÿº   é>şÿÿ´&    v ¹0  º   ‰ğèüÿÿÿ¡    …À[  1Òé€şÿÿ´&    t& èüÿÿÿU‰ÕWVS‰Ãƒì‹@\‰$‹€ˆ  ‰Á‰D$¡    Áé…ÀÊ  ‹$|í Ááº»   Áçá   ¾   ‰¨X  ˜  Ç€”     ‰Øèüÿÿÿt& ‹Œ·`  º   ‰Øèüÿÿÿƒîsè¹   º   ‰Ø¾   èüÿÿÿ¹@   º   ‰Øèüÿÿÿ1Éº   ‰Øèüÿÿÿv ‹Œ·x  º   ‰ØèüÿÿÿƒîsèDí ºƒ   4Å`  ‰Ø‹N<èüÿÿÿ‹N4‹V0‰Øèüÿÿÿ·V@‰Øè&ùÿÿ¹   º   ‰Øèüÿÿÿ‹NDº   ‰Øèüÿÿÿ‹$€¸<   u
ƒÄ[^_]ÃfƒÄ‰Ø¹ Z  º   [^_]éüÿÿÿ´&    èüÿÿÿS‹P\‰Ã‹‚P  ƒø~ƒøuƒºT   t,‰Ø[éäùÿÿt& …Ày[Ã´&    v ‰Ø[éûÿÿ´&    ö‚”  t‹‚  ƒèƒàıt¹    º    ‰Øèüÿÿÿëª¶    ¹!   º    ‰Øèüÿÿÿë‘´&    ¶    èüÿÿÿUWV‰ÆSƒì‹x\‹ŸP  ƒû€   ƒûuº   èüÿÿÿ‰Å‰Á‰Â1Àå   ƒá@ƒ¿X  	tK¸   …ÉDØ‹‡”  ‰Ÿ”  …ítƒ¿T   „€  Ç‡”     »   ƒ=     L  9Ø•À¶ÀƒÄ[^_]Ãt& …ÛyƒÄ1À[^_]Ãf‹l  ‹‡X  ‰Ëƒø„Ù   Pû1ÀƒúwVº#   ‰ğèüÿÿÿƒ=    #  ‰Ã‹—”  ½   ƒãuTt& 9ê„ø   ‹    …É  ‰¯”  ‹l  ¸   9Ù„eÿÿÿ¡    …À/  ‰Ÿl  ƒÄ¸   [^_]ÃÑøƒàƒøt6½   …Àt›ƒø½   Eëë¶    ƒø
½   Eëéxÿÿÿ´&    ½   éfÿÿÿ¶    º   ‰ğèüÿÿÿ=ÿ    ÿÿOÂƒ=    î  ‹—”  1Û=    º= ğÿÿ½   ¸   Mèéÿÿÿv ƒË‰Ÿ”  éşÿÿf‹l  1Àéÿÿÿéx  ´&    ´&    èüÿÿÿU‰ÅWVSƒì‹X\¡    …À.	  Çƒd  ÿÿÿ Çƒh      èüÿÿÿE‰$v ƒ=    	  ºÿÿÿÿ‰Øèüÿÿÿ¡    ƒøš	  €£´  şèüÿÿÿ„À…E  ƒ»X  	„(  Çƒ      ‰Ø¾@   èüÿÿÿ‹»L  …ÿu‹ƒ\  % °  ƒøöƒæáƒÆ €£´  ıºÈ   ‰ØÇƒl      èüÿÿÿ‰Ç¡    …ÿ…¿  ƒ=    í  ƒş„   1Ò1Àë"´&    v ƒÀ@‹Õ¬  …Ét‹Õ   9Öuä‹    …ÒÎ  @‰³T  €=     Å   Çƒ”     ‹P‹@‰“|  ‰ƒ€  „]  ‹ƒL  …À…O  ÷ƒ\    ÿ „?  ƒş	„6  ‹    …É©  ¹	   º    ‰èÇƒT  	   èüÿÿÿt& €‹´  ÇƒX     Çƒl     ¹   º   ‰èèüÿÿÿ¹ 0  º   ‰èèüÿÿÿ€»<   …î  €»E   …É  ‰èèzöÿÿ‰ØÇƒ       èüÿÿÿöƒ´  „üıÿÿ¾   ëCt& ºè  ‰Øèüÿÿÿ…ÀuDƒî‰è‹}\èüÿÿÿ…ÀuO€=     t€§´  ıöƒ´  „²ıÿÿ…öu¾ºˆ  ‰Øèüÿÿÿ‰Æ…Àt¿¡    ƒøµıÿÿéd	  ´&    f‰èèüÿÿÿë¨´&    1Àë t& ƒÇ‹Å¬  …Ò„ë   ‹Å   9Æuà‹Å´  ‹Å°  ‰ƒd  ‰“h  ƒşFƒş	´  ƒş„D  ƒş	„“şÿÿƒîƒş‡¢şÿÿ€‹´  ÇƒX     éŒşÿÿ´&    fƒş „/  ƒş@…qşÿÿÇƒX     ‰èº   Çƒ”     èüÿÿÿ¹rœ” ºrœ” ‰èèüÿÿÿé;şÿÿ´&    ¡    …À	  Çƒ       ‰Øèüÿÿÿémüÿÿt& ¸ÿÿÿ 1Òé%ÿÿÿt& ‹‹x  º@   ‰èèüÿÿÿé şÿÿ¹ Z  º   ‰èèüÿÿÿéüıÿÿ´&    v ¹   º    ‰èèüÿÿÿë%t& º~   ‰èèüÿÿÿ=ş  ~eƒ=    Ê  ºd   ‰Øèüÿÿÿ…ÀtĞé7şÿÿv ¸   €‹´  ‰ƒX  Çƒl     éRıÿÿ´&    ÇƒX     é<ıÿÿ´&    f‰ÆéIüÿÿ´&    f¡    …ÀI	  ƒÄ1À[^_]Ã1Àƒş
”ÀƒÀë‘¸¸  ët& ƒÀ;1tF‹P‰Á…Òuğéº	  v ‰D$º¸  ¹¸  1Àë¶    ‹J…É„å  ‹ƒÂ9Æuì‹D$éî  é¿	  ´&    ´&    èüÿÿÿUWV‰ÆSƒì‹=    ‹X\…ÿ
  Çƒd  ÿÿÿ ~Çƒh      èüÿÿÿ¡    ‰<$t& ƒøu
  ºÿÿÿÿ‰Øèüÿÿÿ¡    ƒøU
  …À:
  €£´  şèüÿÿÿ„À…  ƒ»X  	„  ‹n\‰ğÇ…T     ‹½ˆ  Áïèüÿÿÿ€½E   …r  ‹F\‹L  …Ò…!  ‹\  ‹€`  ‰Ñ€õ 	Á„€  ‰Ñ€õ€	Á„  ‰Ññ  @ 	Á„{  ¸p  €æ·…İ  Áçº0   ‰ùá   	Á‰ğèüÿÿÿ‰ğèÍğÿÿ¹   º   ‰ğèüÿÿÿ¹ 0  º   ‰ğèüÿÿÿ€½<   …®  ‹    º"   ‰ğèüÿÿÿ‹ƒL  ¹@   …Àu€»J   ‹    tƒù¸    DÈ‰‹T  º    ‰ğèüÿÿÿ‹‹T  ƒùub¡    …À÷	  ¿
   t& ºd   ‰Øèüÿÿÿ…À…ä  º~   ‰ğèüÿÿÿ=ş  5  ƒ=    Ü	  ƒïuÃ‹‹T  ƒù„%  ‹-    …í‘  1Ò1ÀëƒÀ@‹<Õ¬  …ÿ„›   ‹Õ   9Ñuà@‹Å´  ‹Å°  ‰ƒd  ‰“h  ƒù	„b  ‰Øèüÿÿÿ‹‹p  º   ‰ğèüÿÿÿ…À…  ƒ»T   „¬   ¡    éšıÿÿf‹    …Éˆ  Çƒ       ‰Øèüÿÿÿ¡    éoıÿÿ´&    ¸ÿÿÿ 1Òéuÿÿÿt& ‹    …Éh  ¸   éşÿÿ´&    ¹ Z  º   ‰ğèüÿÿÿé<şÿÿ´&    v ‹x  º@   ‰ğèüÿÿÿéwıÿÿ´&    f€‹´  ‰ğ‹~\èüÿÿÿ…À…ï   €=     t.€§´  ıë%t& €=     t€§´  ıºˆ  ‰Øèüÿÿÿ…Àu(öƒ´  „÷şÿÿ‰ğ‹~\èüÿÿÿ…ÀtÅ‰ğèüÿÿÿë¼t& ¡    é™üÿÿ¶    ¹ |  º   ‰ğèüÿÿÿéˆşÿÿf¡    …ÀH  ¸   éıÿÿ´&    f¡    …À(  ¸   éìüÿÿ´&    f‰Á‰ƒT  ƒù…Ûıÿÿ¡    …Àúûÿÿéè
  t& ‰ğèüÿÿÿéÿÿÿt& ‹    …Ò  ¸@  é“üÿÿ¡    …ÀÈ
  ¸`  é|üÿÿ¡    …À«
  1ÀƒÄ[^_]ÃƒÄ¸ÿÿÿÿ[^_]Ã¸¸  1ÿº¸  ë´&    ‹P…Ò„
  ‹8ƒÀ9ùuìé
  ´&    èüÿÿÿUW‰ÇVSƒì‹p\¡    …À0  Ç†d  ÿÿÿ Ç†h      èüÿÿÿG‰D$fƒ=      ºÿÿÿÿ‰ğèüÿÿÿ¡    ƒøÒ  €¦´  şèüÿÿÿ„À…¥  ‹†L  …Àu[ƒ¾X  	tRÇ†      ‰ğèüÿÿÿ1Ò‰øèüÿÿÿ€¦´  ıºÈ   ‰ğÇ†l      èüÿÿÿ‰$…Àt<¡    ƒø~‘é·  t& ‹-    …íœ  Ç†       ‰ğèüÿÿÿé<ÿÿÿt& €=     „ƒ   ‹†\  ©  ÿ tv‹    …Ûl  ‹  ‰–|  ©  ÿ …  €¾<   „ÿ  ¡À   º   ‰†€  ‰øèüÿÿÿ€´  Ç†l     Ç†d  à 2 Ç†h      éĞ  v ÇD$ÿÿÿÿ1Û1íëD´&    v º   ‰øèüÿÿÿ  ÿÿ=ÿ  OÂ9Å}‰\$‰Å‹    …É.  ƒÃƒût.‹İ   ‰ø‰Ñèüÿÿÿºd   ‰ğèüÿÿÿ…Àt¦¡    é½şÿÿt& ‹D$ƒø…›   ÇD$   ½ğ   €=     …  ÇD$ÿÿÿÿ1ÛëFt& º   ‰øèüÿÿÿ  ÿÿ=ÿ  OÂ;$~‰$‰\$‹    …Ò  ƒÃ;\$Ÿ  ‹Tİ ‰ø‰Ñèüÿÿÿºd   ‰ğèüÿÿÿ…Àt¢¡    éşÿÿ´&    ƒø„  €=     …º  ‹Å   ÇD$ÿÿÿÿ‰–|  ƒ|$~Eƒ|$„  ‹D$ƒèƒø†“  ƒ|$ u?‹†\  éşÿÿ¡    …ÀQ  ƒÄ1À[^_]Ã‹D$…ÀuÇ†d   ·  Ç†h      ‰–€  ‰øº   èüÿÿÿ‹|  ‹–€  ‰øèüÿÿÿ‰øÇ†       è×êÿÿ‰ğèüÿÿÿ¡    …À…  ö†´  „–üÿÿ»   ëI´&    ºè  ‰ğèüÿÿÿ…À…>şÿÿ‰øƒë‹o\èüÿÿÿ…ÀuE€=     t€¥´  ıö†´  „Füÿÿ…Ûuººˆ  ‰ğèüÿÿÿ‰Ã…À…öıÿÿ‰ø‹o\èüÿÿÿ…Àt»‰øèüÿÿÿë²t& ‹D$‹Å   ‰–|  ƒø…¡şÿÿÇ†d    ‹D$Ç†h      …À„Ì  ƒ|$…Şşÿÿ€¾<   „Ñşÿÿ¡ø   º   ‰†€  ‰øèüÿÿÿ€´  Ç†l     é¶şÿÿ‹_\‹‹|  ;‹€  „“   …ÀK  ‹ƒX  Pûƒúveƒø…„   ‹    …É«şÿÿéÉ  ´&    fÇ†d     º   ‰øÇ†h      Ç†€    [ èüÿÿÿ€´  Ç†l     é%şÿÿ‹-    …íOşÿÿé„  t& …ÀmÿÿÿéÜ  v ƒø…,şÿÿ‹‹€  9‹|  „şÿÿ‹    …Òşÿÿé£  ´&    v ‹D$º   ‹ÅÀ   ‰†€  ‰øèüÿÿÿ€´  Ç†d  à 2 Ç†h      éˆıÿÿ‹†\  ©  ÿ „Qüÿÿ‹L$‹Í   ‰–|  ƒù„øşÿÿƒù„+ûÿÿ…É„&ıÿÿƒù…1ıÿÿÇ†d    Ç†h      ¡ğ   º   ‰†€  ‰øèüÿÿÿ€´  éıÿÿ¡À   º   ‰†€  ‰øèüÿÿÿ€´  Ç†d    À Ç†h      éÕüÿÿ‹†\  ©  ÿ …Mÿÿÿ‹D$‹Å   ‰–|  ƒøu4Ç†d    Ç†h      éƒüÿÿ€=     uÇD$   ½À   éTûÿÿÇD$ÿÿÿÿéóûÿÿ‹†\  ©  ÿ tÔéäşÿÿR‰T$h    ‹ˆ”   ƒÁ‰D$Qh   èüÿÿÿ‹D$‹T$ƒÄé   ~ƒÆÿ³4  ÿ³0  Vh    WèüÿÿÿCpPh8   j4t$ VèüÿÿÿƒÀ   ‰ô‰òèüÿÿÿ‹ƒX  ƒø	vL¸   ‹“P  …Ò…£   ‹“”  ¾+   ¹$   öÂDÎ€â¾3   º,   DÖQRPhU   WèüÿÿÿƒÄë‹…    u°PhG   Wèüÿÿÿ‰ôÿ³  h   Wèüÿÿÿÿ³Œ  ÿ³ˆ  h@   Wèüÿÿÿÿ³p  h‘   WèüÿÿÿƒÄ(‹D$4d+    ucƒÄ81À[^_]ÃƒêuPhG   WèüÿÿÿƒÄ‹ƒ”  º+   ½$   ¾,   ¨Dê¨¸3   Dğ‹ƒT  èüÿÿÿUVPhj   WèüÿÿÿƒÄéTÿÿÿèüÿÿÿh£   P‰D$h   èüÿÿÿ‹D$ƒÄé  ƒÃh¶   SèüÿÿÿXƒÈÿZé¯  Ch«   Ph   èüÿÿÿƒÄéa  PƒÃ‰D$WVhÉ   Sh   èüÿÿÿ‹D$ƒÄé  WVUƒÅhl   ÿt$èüÿÿÿ¸
   èüÿÿÿƒÄƒıuh   ÿt$èüÿÿÿ‰ØèüÿÿÿXƒÈÿZé  éL  WVUƒÅh´   ÿt$èüÿÿÿ¸
   èüÿÿÿƒÄƒıu@h   ÿt$èüÿÿÿ‰ØèüÿÿÿXƒÈÿZé
  Cÿt$$QRhå   Ph   èüÿÿÿƒÄéä  éì  ‹Ø   ‹I|…Ò„ƒ   …À½  ¸  º  DÅQ½   RPGhÜ   Ph   èüÿÿÿ1Éº   ‰øSèœ  ƒÄéf  ‹†Ø   ÿp|Gh  h  hÜ   Ph   èüÿÿÿ1Éº   ‰øSèœ  ƒÄéa  º  ¸  ë…h  Ph   èüÿÿÿƒÄé£  º+   €»<   ¸  DÂ‰×‹‹P  ÿ4ü   WRP‹E  P‹D$(ƒàP‹D$$¶ÄƒÀ@Pÿt$ÿt$<ÿt$(h(  ÿt$Dèüÿÿÿ‹ƒP  ƒÄ0ƒø‡Q	  ‹…    …À„Q	  hk  jÿUPèüÿÿÿ‰ÇƒÄ= ğÿÿ‡ÿ   èüÿÿÿ‰»¤  ‹E\ƒ¸¤   „Q	  ¶´  ¹   ¨  ƒâüƒÊˆPº   j èüÿÿÿ[éQ	  ¹+   º  ¸  €»<   DÑDÁ¿%  éÿÿÿÿ³4  ÿt$hQ  ÿt$$h   èüÿÿÿ‹ƒ0  ‰D$ ƒÄƒÀt‹ƒ4  ‹|$	Ç‰D$…œ	  ƒ=     L	  hü   ¾íÿÿÿÿt$h   èüÿÿÿƒÄéQ	  Eh3  ¾íÿÿÿPh   èüÿÿÿƒÄéQ	  ‰ƒ¤  hs  ÿt$èüÿÿÿZYéñşÿÿQRÿ4…@  Chd  Ph   èüÿÿÿ‹p  ƒÄé¶  Fh‹  Ph   èüÿÿÿƒÄéı  R‰T$Q‰L$PChŒ  Phƒ  èüÿÿÿ‹L$‹T$ƒÄé„  FRh  Phƒ  èüÿÿÿƒÄé  Fjh(  Phƒ  èüÿÿÿ1ÒƒÄé  RF‰T$h  Phƒ  èüÿÿÿ‹T$ƒÄém  F‰$Sh(  Phƒ  èüÿÿÿ‹T$ƒÄƒû„  ƒû  …Û„D  ƒë„Ñ  é`  FRh¢  Phƒ  èüÿÿÿƒÄé  FRh·  Phƒ  èüÿÿÿ‹    ƒÄé¶  ƒÆh  Vhƒ  èüÿÿÿƒÄéÔ  FRhÎ  Phƒ  èüÿÿÿ‹    1ÒƒÄé"  FRhü  Phƒ  èüÿÿÿ‹    1ÒƒÄé"  Fjh(  Phƒ  èüÿÿÿ1ÒƒÄé`  FSh(  Phƒ  èüÿÿÿ1ÒƒÄéÿÿÿƒÆShØ  Vhƒ  èüÿÿÿƒÄéÔ  Fj h(  Phƒ  èüÿÿÿ1ÒƒÄéD  Fjh(  Phƒ  èüÿÿÿ1ÒƒÄéÑ  Fh8  Phƒ  èüÿÿÿƒÄé
  FRhä  Phƒ  èüÿÿÿ‹    º   ƒÄé"  ƒû…`  éÑ  C‰L$RhN  Phƒ  èüÿÿÿ‹L$ƒÄé  PV‰D$h\  Rhƒ  èüÿÿÿ‹D$ƒÄé#  FURhD  Phƒ  èüÿÿÿƒÄér  ƒÆSQh  Vhƒ  èüÿÿÿƒÄé˜  ƒÆ‰$SUQRhd  Vhƒ  èüÿÿÿ‹Ÿ”  ‹D$ƒÄéè  ‰ÂÑúƒâR‰Âƒâ‰D$RVhx  Rhƒ  èüÿÿÿ‹D$ƒÄéF  Vh:  hÜ  ÿt$hƒ  èüÿÿÿƒÄé$  h  ÿt$hƒ  èüÿÿÿƒÄéÔ  ‹D$¹†  ‰D$VQh¼  ÿt$hƒ  èüÿÿÿ‹D$ƒÄé¶  h¿  ÿt$hƒ  èüÿÿÿƒÄéÉ  Eh§  Phƒ  èüÿÿÿƒÄéš  ƒÅhQ  Uhƒ  èüÿÿÿƒÄéI   hî  ÿt$hƒ  èüÿÿÿƒÄéã  h  ÿt$hƒ  èüÿÿÿƒÄé9  hÖ  ÿt$hƒ  èüÿÿÿ¡    ƒÄé%  º†  VRhœ  ÿt$hƒ  èüÿÿÿƒÄés  h  ÿt$hƒ  èüÿÿÿƒÄéÔ"  h   ÿt$hƒ  èüÿÿÿƒÄé“"  º†  QRh€  ÿt$hƒ  èüÿÿÿ‹‹T  ƒÄéö"  hî  ÿt$hƒ  èüÿÿÿƒÄé?!  h  ÿt$hƒ  èüÿÿÿ¡    ƒÄé7!  hx  ÿt$hƒ  èüÿÿÿƒÄé!  @h_  Phƒ  èüÿÿÿƒÄéë   ƒÆhQ  Vhƒ  èüÿÿÿ1ÀƒÄér%  hğ  ÿt$hƒ  èüÿÿÿ¸`  ƒÄéß!  hH  ÿt$hƒ  èüÿÿÿ¡    ƒÄé!  h¿  ÿt$hƒ  èüÿÿÿ¸@  ƒÄéß!  hÙ  ÿt$hƒ  èüÿÿÿ¸   ƒÄéß!  h  ÿt$hƒ  èüÿÿÿ¸   ƒÄéß!  h©  ÿt$hƒ  èüÿÿÿ¸   ƒÄéß!  h  ÿt$hƒ  èüÿÿÿƒÄéŠ#  ‰È»  ™÷û»è  ™÷û‰È»°â R™÷ûPh0  ÿt$hƒ  èüÿÿÿƒÄéY)  ‰È½  ™÷ı½è  ™÷ı‰È½°â R™÷ıPh   ÿt$hƒ  èüÿÿÿƒÄé‰*  h9  ÿt$hƒ  èüÿÿÿƒÄé	&  Gh!  Phƒ  èüÿÿÿƒÄéÚ%  ‰È½  ™÷ı½è  ™÷ı‰È½°â R™÷ıPhÄ  ÿt$hƒ  èüÿÿÿƒÄé‰*  ‹‹€  ½  ‰È™÷ı½è  ™÷ı‰È½°â R™÷ıPhè  ÿt$hƒ  èüÿÿÿ‹ƒX  ƒÄé—*  ‹‹€  ½  ‰È™÷ı½è  ™÷ı‰È½°â R™÷ıPh  ÿt$hƒ  èüÿÿÿ‹ƒX  ƒÄé$+  ÿtİPh“  ÿt$hƒ  èüÿÿÿƒÄé@(  ÿ4İ  Ph{  ÿt$hƒ  èüÿÿÿƒÄé'  GhQ  Phƒ  èüÿÿÿƒÄéé(  hh  ½À   ÿt$hƒ  èüÿÿÿƒÄÇD$   ÇD$   éî'  h  ÿt$hƒ  èüÿÿÿƒÄéª&  hî  ÿt$hƒ  èüÿÿÿƒÄé#&  hP  ÿt$hƒ  èüÿÿÿ¡    ƒÄé&  èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                                  ¤     ¼  Å  à  ò         %                          msp3400                                                                                                                                            4  ;  F                             `  @                             0                          Ğ  °          @  Ğ                                                   P          `                                                                                                                              0      `  d  h  l  p  w  3   ~                   ÿÿÿÿÿÿÿÿ      @  @         @     @      @  €0     €  €   €0      €        0  dolby   amsound standard    stereo_threshold    debug   once    opmode              0                                  «  °  ·  ½  Ã      K      $   #   '   (   K      $   #   '   (   cL cL Ğ           0  ÿÿÿÿÿÿÿÿøÿÿÿ   ;   ~   ÿÿÿÿÿÿÿÿøÿÿÿ   ;   ~   cL cL Ğ           0  øÿÿÿøÿÿÿ      N   k   øÿÿÿøÿÿÿ      N   k   rœ” rœ” Ğ   €       0           0   B   H            0   B   H   cL cL Ğ   €  0    0     	         !   %            0   B   H   GZ GZ Æ   €       0  şÿÿÿøÿÿÿöÿÿÿ
   2   V            0   B   H   cL cL Ğ   @       0        úÿÿÿüÿÿÿ(   ^            0   B   H   UUS UUS Ğ   @       0  şÿÿÿøÿÿÿöÿÿÿ
   2   V   üÿÿÿôÿÿÿ÷ÿÿÿ   O   ~   GZ GZ Æ   @      |              ¸  ÿÿÿ                Ú  ÿÿÿ         €> 9A ë   ·         cL ªÀO            GZ  êV #  à 2        GZ 9¤] A  à 2        GZ GZ _  à 2        GZ ªÀO x  à 2        cL  @Q :        	   GZ  @Q –    À     
   UUS ãøZ «            GZ  @Q À  à 2        GZ  @Q T  à 2        GZ  @Q t  à 2          €>  €> ×         !    €>  €> ê         0    €>  €>           @   rœ” rœ”   ÿÿÿ     P   GZ GZ 1  ÿÿÿ     Q    €a   d ?  ÿÿÿ     `     d   d U  ÿÿÿ     ÿÿÿÿ                    Setting I2S speed to %d
 7 unknown , dual  stereo mono %s: Audio:     Mode:     %s
 Mode:     %s (%s%s)
 Standard: %s (%s%s)
 Audmode:  0x%04x
 ACB:      0x%04x
 resume
 msp_reset
 chip reset failed
 msp_read(0x%x, 0x%x): 0x%x
 msp_write(0x%x, 0x%x, 0x%x)
 no on yes off suspend
 nicam  and  radio msp3400 msp3400 not found
 &state->wq rev1=0x%04x, rev2=0x%04x
 msp34xx kernel_thread() failed
 switching to radio mode
 AM (for carrier detect) FM Radio Terrestrial FM-mono/stereo Satellite FM-mono NICAM/FM (B/G, D/K) NICAM/FM (I) NICAM/AM (L) BTSC External input manual autodetect autodetect and autoselect in1 in2 in3 in4 in1 da in2 da mute 7 unknown FM set_audmode: %s
 SAT set_audmode: %s
 NICAM set_audmode: %s
 BTSC set_audmode: %s
 extern set_audmode: %s
 FM-Radio set_audmode: %s
 mono set_audmode
 set audmode %d
 switching to AM mono
 set_mode: %d
 stereo detect register: %d
 nicam sync=%d, mode=%d
 watch: nicam %d => %d
 msp3410 daemon started
 msp3410 thread: sleep
 msp3410 thread: wakeup
 thread: restart scan
 thread: no carrier scan
 detection still in progress
 5.5/5.85  B/G NICAM FM thread: exit
 msp34xxg daemon started
 msp34xxg thread: sleep
 msp34xxg thread: wakeup
 selected radio modus
 selected M (EIA-J) modus
 selected M (A2) modus
 selected SECAM-L modus
 selected M (BTSC) modus
 msp3400 daemon started
 msp3400 thread: sleep
 msp3400 thread: wakeup
 AM sound override
 carrier1 val: %5d / %s
 carrier2 val: %5d / %s
 mono stereo lang2 lang1 lang1+lang2 5.85  PAL D/K + SECAM NICAM 6.25  PAL D/K1 FM-stereo 6.74  PAL D/K2 FM-stereo 7.02  PAL SAT FM-stereo s/b 7.20  PAL SAT FM-stereo s 7.38  PAL SAT FM-stereo b 5.742 PAL B/G FM-stereo 5.85  PAL B/G NICAM 4.5   NTSC 5.5   PAL B/G 6.0   PAL I 6.5   PAL D/K + SAT + SECAM autodetect start 4.5/4.72  M Dual FM-Stereo 5.5/5.74  B/G Dual FM-Stereo 6.5/6.25  D/K1 Dual FM-Stereo 6.5/6.74  D/K2 Dual FM-Stereo 6.5  D/K FM-Mono (HDEV3) 6.5/5.74  D/K3 Dual FM-Stereo 6.5/5.85  L NICAM AM 6.0/6.55  I NICAM FM 6.5/5.85  D/K NICAM FM 4.5  M BTSC-Stereo 4.5  M BTSC-Mono + SAP 4.5  M EIA-J Japan Stereo 10.7  FM-Stereo Radio 6.5  SAT-Mono 7.02/7.20  SAT-Stereo 7.2  SAT ADR     `   Ğ      @  °     P  Ğ  Ğ     0    À           @  `  €  0    p  Ğ        @  Ğ  p  °     `  À  `  €  Ğ   À%    detected standard: %s(0x%08Lx)
 %s rev1 = 0x%04x rev2 = 0x%04x
 Routing:  0x%08x (input) 0x%08x (output)
   I/O error #%d (read 0x%02x/0x%02x)
 resetting chip, sound will go off.
 I/O error #%d (write 0x%02x/0x%02x)
    mute=%s scanning=%s volume=%d
  not an msp3400 (cannot read chip version)
  MSP%d4%02d%c-%c%d found on %s: supports %s%s%s, mode is %s
 scart switch: %s => %d (ACB=0x%04x)
    set source to %d (0x%x) for output %02x
    could not detect sound standard set_audmode called with mode=%d instead of set_source (ignored)
    set_audmode final source/matrix = 0x%x
 watch: rxsubchans %02x => %02x
 status=0x%x, stereo=%d, bilingual=%d -> rxsubchans=%d
  setting standard: %s (0x%04x)
  current standard: %s (0x%04x)
  autodetection failed, switching to backup standard: %s (0x%04x)
    started autodetect, waiting for result
 detection still in progress after 10 tries. giving up.
 detected standard: %s (0x%04x)
 mono sound carrier: %d.%03d MHz
    main sound carrier: %d.%03d MHz
    NICAM/FM carrier  : %d.%03d MHz
    NICAM/AM carrier  : %d.%03d MHz
    FM-stereo carrier : %d.%03d MHz
    6.5/5.85  D/K NICAM FM (HDEV2)  6.5/5.85  D/K NICAM FM (HDEV3) parm=dolby:Activates Dolby processing parm=amsound:Hardwire AM sound at 6.5Hz (France), FM can autoscan parm=standard:Specify audio standard: 32 = NTSC, 64 = radio, Default: Autodetect parm=stereo_threshold:Sets signal threshold to activate stereo parm=debug:Enable debug messages [0-3] parm=once:No continuous stereo monitoring parm=opmode:Forces a MSP3400 opmode. 0=Manual, 1=Autodetect, 2=Autodetect and autoselect parmtype=dolby:bool parmtype=amsound:bool parmtype=standard:int parmtype=stereo_threshold:int parmtype=debug:int parmtype=once:bool parmtype=opmode:int license=GPL author=Gerd Knorr description=device driver for msp34xx TV sound processor Ì          ¤ÿ     Ô          ¤ÿ     Ü          ¤ÿ     è          ¤ÿ     ü          ¤ÿ               ¤ÿ               $ÿ ¨             €                                  À                  +                                                                          `                            ÿÿÿÿ                     @Q Ï   êV ë  9¤]    €a     d 9   €f S  ªÀO m   @Q …   €> ™  cL ¤  UUS ²  GZ ¾   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                           ñÿ       `     '       2     A   `   e     N   Ğ   0     Z      9     i   2   a    }       (    
 ‡   @  c     “   °  B     £      P     ®   “        ¾   P  |     È           Ô           ß            ç   ³  3     ö   Ğ  Í     ÿ   æ  p          ˆ       V  k     &  0  \    1  Á        A    "     M  a       ^  À  @     i     ò    s          
 {           Å  ¨        ƒ  0      
   y  µ    Ÿ  ü       
 ª          
 ´  `  l    
 »  .  *     Î  @       
 Ú  0  V    è  Ğ  ±     ô  X                        €            
     0      \    
 ?  `   0    
 F     0    
 S  `  ,    
 a  @      
 o     P    
 }      &     ’  &   B     ©  h   Q     Á  ¹   ?     á  ø   '     ö    *     
  I  Y        ¢       9           G  Ì      
 Y  ¶       t          „  Ô      
 ˜  Ì       ´  (        Å  Ü  	    
 Ú  â       ş  <          è      
 4          M  P        [  ü      
 m         …  d        ’        
 £  &       ½  x        Ì        
 ß  :       ö  F         X  9     '           ñÿ:    ¡     I  @  ‰     ]  4      
 g         
 q  s  .     Š  Ğ  –     Ÿ        
 «     5    À  H      
 Ê  ¡  -    ä  `  @   
 ÷  Î  $       ò  »     %  ­  3    :  à	  Ç    O           l  À   0     ‡  ğ        ¢  §  O    ·             Ì          Ö          