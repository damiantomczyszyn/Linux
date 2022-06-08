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
63v$Æ‚üòu|®óëv"h­ØîC±ß}ë+e_KvTqµŒ·ÚõHZ×½Á|œª÷Shó{LGùÃ×ÖùØ`÷ÌJ&ŞÅq?Z÷„VC¥0rÎ%4%ŞvèÅÍ=k8t>PdïO’8¾Y†Vk½”Ì495 €V¶Ïğa“jLmé^7Ã:¯d—>'#İ­Mš	-ô