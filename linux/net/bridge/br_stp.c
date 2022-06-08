atic void torture_rwsem_up_read(int tid __maybe_unused)
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
µîõ}H/0TÂ:”‹z… aGÎ3øc|¿aô&¾·§´GşT¯si…Ùê„5İsçeÂxbáf¹È%ÙLÌM¹¹üHd7‰I>áL²õ£%±(õ'QCa.Ç æ]ø^/Gåãİ6Wsà±%hö5EÊGKMUĞ×¿<ŠA¿8V,ì…Å ©r±ü¸I¦V¡Å´v[ríŠëüdMímXdë½'Ìîä"kÈ¥¯ŠPzŒ;	äÄä 5