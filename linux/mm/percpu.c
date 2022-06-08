		seq_printf(m, "[%p] ", class->key);
		print_name(m, class);
		seq_puts(m, "\n");
	}
	seq_puts(m, "\n");

	return 0;
}

static const struct seq_operations lockdep_chains_ops = {
	.start	= lc_start,
	.next	= lc_next,
	.stop	= lc_stop,
	.show	= lc_show,
};
#endif /* CONFIG_PROVE_LOCKING */

static void lockdep_stats_debug_show(struct seq_file *m)
{
#ifdef CONFIG_DEBUG_LOCKDEP
	unsigned long long hi1 = debug_atomic_read(hardirqs_on_events),
			   hi2 = debug_atomic_read(hardirqs_off_events),
			   hr1 = debug_atomic_read(redundant_hardirqs_on),
			   hr2 = debug_atomic_read(redundant_hardirqs_off),
			   si1 = debug_atomic_read(softirqs_on_events),
			   si2 = debug_atomic_read(softirqs_off_events),
			   sr1 = debug_atomic_read(redundant_softirqs_on),
			   sr2 = debug_atomic_read(redundant_softirqs_off);

	seq_printf(m, " chain lookup misses:           %11llu\n",
		debug_atomic_read(chain_lookup_misses));
	seq_printf(m, " chain lookup hits:             %11llu\n",
		debug_atomic_read(chain_lookup_hits));
	seq_printf(m, " cyclic checks:                 %11llu\n",
		debug_atomic_read(nr_cyclic_checks));
	seq_printf(m, " redundant checks:              %11llu\n",
		debug_atomic_read(nr_redundant_checks));
	seq_printf(m, " redundant links:               %11llu\n",
		debug_atomic_read(nr_redundant));
	seq_printf(m, " find-mask forwards checks:     %11llu\n",
		debug_atomic_read(nr_find_usage_forwards_checks));
	seq_printf(m, " find-mask backwards checks:    %11llu\n",
		debug_atomic_read(nr_find_usage_backwards_checks));

	seq_printf(m, " hardirq on events:             %11llu\n", hi1);
	seq_printf(m, " hardirq off events:            %11llu\n", hi2);
	seq_printf(m, " redundant hardirq ons:         %11llu\n", hr1);
	seq_printf(m, " redundant hardirq offs:        %11llu\n", hr2);
	seq_printf(m, " softirq on events:             %11llu\n", si1);
	seq_printf(m, " softirq off events:            %11llu\n", si2);
	seq_printf(m, " redundant softirq ons:         %11llu\n", sr1);
	seq_printf(m, " redundant softirq offs:        %11llu\n", sr2);
#endif
}

static int lockdep_stats_show(struct seq_file *m, void *v)
{
	unsigned long nr_unused = 0, nr_uncategorized = 0,
		      nr_irq_safe = 0, nr_irq_unsafe = 0,
		      nr_softirq_safe = 0, nr_softirq_unsafe = 0,
		      nr_hardirq_safe = 0, nr_hardirq_unsafe = 0,
		      nr_irq_read_safe = 0, nr_irq_read_unsafe = 0,
		      nr_softirq_read_safe = 0, nr_softirq_read_unsafe = 0,
		      nr_hardirq_read_safe = 0, nr_hardirq_read_unsafe = 0,
		      sum_forward_deps = 0;

#ifdef CONFIG_PROVE_LOCKING
	struct lock_class *class;
	unsigned long idx;

	iterate_lock_classes(idx, class) {
		if (!test_bit(idx, lock_classes_in_use))
			continue;

		if (class->usage_mask == 0)
			nr_unused++;
		if (class->usage_mask == LOCKF_USED)
			nr_uncategorized++;
		if (class->usage_mask & LOCKF_USED_IN_IRQ)
			nr_irq_safe++;
		if (class->usage_mask & LOCKF_ENABLED_IRQ)
			nr_irq_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_SOFTIRQ)
			nr_softirq_safe++;
		if (class->usage_mask & LOCKF_ENABLED_SOFTIRQ)
			nr_softirq_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_HARDIRQ)
			nr_hardirq_safe++;
		if (class->usage_mask & LOCKF_ENABLED_HARDIRQ)
			nr_hardirq_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_IRQ_READ)
			nr_irq_read_safe++;
		if (class->usage_mask & LOCKF_ENABLED_IRQ_READ)
			nr_irq_read_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_SOFTIRQ_READ)
			nr_softirq_read_safe++;
		if (class->usage_mask & LOCKF_ENABLED_SOFTIRQ_READ)
			nr_softirq_read_unsafe++;
		if (class->usage_mask & LOCKF_USED_IN_HARDIRQ_READ)
			nr_hardirq_read_safe++;
		if (class->usage_mask & LOCKF_ENABLED_HARDIRQ_READ)
			nr_hardirq_read_unsafe++;

		sum_forward_deps += lockdep_count_forward_deps(class);
	}

#ifdef CONFIG_DEBUG_LOCKDEP
	DEBUG_LOCKS_WARN_ON(debug_atomic_read(nr_unused_locks) != nr_unused);
#endif

#endif
	seq_printf(m, " lock-classes:                  %11lu [max: %lu]\n",
			nr_lock_classes, MAX_LOCKDEP_KEYS);
	seq_printf(m, " direct dependencies:           %11lu [max: %lu]\n",
			nr_list_entries, MAX_LOCKDEP_ENTRIES);
	seq_printf(m, " indirect dependencies:         %11lu\n",
			sum_forward_deps);

	/*
	 * Total number of dependencies:
	 *
	 * All irq-safe locks may nest inside irq-unsafe locks,
	 * plus all the other known dependencies:
	 */
	seq_printf(m, " all direct dependencies:       %11lu\n",
			nr_irq_unsafe * nr_irq_safe +
			nr_hardirq_unsafe * nr_hardirq_safe +
			nr_list_entries);

#ifdef CONFIG_PROVE_LOCKING
	seq_printf(m, " dependency chains:             %11lu [max: %lu]\n",
			lock_chain_count(), MAX_LOCKDEP_CHAINS);
	seq_printf(m, " dependency chain hlocks used:  %11lu [max: %lu]\n",
			MAX_LOCKDEP_CHAIN_HLOCKS -
			(nr_free_chain_hlocks + nr_lost_chain_hlocks),
			MAX_LOCKDEP_CHAIN_HLOCKS);
	seq_printf(m, " dependency chain hlocks lost:  %11u\n",
			nr_lost_chain_hlocks);
#endif

#ifdef CONFIG_TRACE_IRQFLAGS
	seq_printf(m, " in-hardirq chains:             %11u\n",
			nr_hardirq_chains);
	seq_printf(m, " in-softirq chains:             %11u\n",
			nr_softirq_chains);
#endif
	seq_printf(m, " in-process chains:             %11u\n",
			nr_process_chains);
	seq_printf(m, " stack-trace entries:           %11lu [max: %lu]\n",
			nr_stack_trace_entries, MAX_STACK_TRACE_ENTRIES);
#if defined(CONFIG_TRACE_IRQFLAGS) && defined(CONFIG_PROVE_LOCKING)
	seq_printf(m, " number of stack traces:        %11llu\n",
		   lockdep_stack_trace_count());
	seq_printf(m, " number of stack hash chains:   %11llu\n",
		   lockdep_stack_hash_count());
#endif
	seq_printf(m, " combined max dependencies:     %11u\n",
			(nr_hardirq_chains + 1) *
			(nr_softirq_chains + 1) *
			(nr_process_chains + 1)
	);
	seq_printf(m, " hardirq-safe locks:            %11lu\n",
			nr_hardirq_safe);
	seq_printf(m, " hardirq-unsafe locks:          %11lu\n",
			nr_hardirq_unsafe);
	seq_printf(m, " softirq-safe locks:            %11lu\n",
			nr_softirq_safe);
	seq_printf(m, " softirq-unsafe locks:          %11lu\n",
			nr_softirq_unsafe);
	seq_printf(m, " irq-safe locks:                %11lu\n",
			nr_irq_safe);
	seq_printf(m, " irq-unsafe locks:              %11lu\n",
			nr_irq_unsafe);

	seq_printf(m, " hardirq-read-safe locks:       %11lu\n",
			nr_hardirq_read_safe);
	seq_printf(m, " hardirq-read-unsafe locks:     %11lu\n",
			nr_hardirq_read_unsafe);
	seq_printf(m, " softirq-read-safe locks:       %11lu\n",
			nr_softirq_read_safe);
	seq_printf(m, " softirq-read-unsafe locks:     %11lu\n",
			nr_softirq_read_unsafe);
	seq_printf(m, " irq-read-safe locks:           %11lu\n",
			nr_irq_read_safe);
	seq_printf(m, " irq-read-unsafe locks:         %11lu\n",
			nr_irq_read_unsafe);

	seq_printf(m, " uncategorized locks:           %11lu\n",
			nr_uncategorized);
	seq_printf(m, " unused locks:                  %11lu\n",
			nr_unused);
	seq_printf(m, " max locking depth:             %11u\n",
			max_lockdep_depth);
#ifdef CONFIG_PROVE_LOCKING
	seq_printf(m, " max bfs queue depth:           %11u\n",
			max_bfs_queue_depth);
#endif
	seq_printf(m, " max lock class index:          %11lu\n",
			max_lock_class_idx);
	lockdep_stats_debug_show(m);
	seq_printf(m, " debug_locks:                   %11u\n",
			debug_locks);

	/*
	 * Zapped classes and lockdep data buffers reuse statistics.
	 */
	seq_puts(m, "\n");
	seq_printf(m, " zapped classes:                %11lu\n",
			nr_zapped_classes);
#ifdef CONFIG_PROVE_LOCKING
	seq_printf(m, " zapped lock chains:            %11lu\n",
			nr_zapped_lock_chains);
	seq_printf(m, " large chain blocks:            %11u\n",
			nr_large_chain_blocks);
#endif
	return 0;
}

#ifdef CONFIG_LOCK_STAT

struct lock_stat_data {
	struct lock_class *class;
	struct lock_class_stats stats;
};

struct lock_stat_seq {
	struct lock_stat_data *iter_end;
	struct lock_stat_data stats[MAX_LOCKDEP_KEYS];
};

/*
 * sort on absolute number of contentions
 */
static int lock_stat_cmp(const void *l, const void *r)
{
	const struct lock_stat_data *dl = l, *dr = r;
	unsigned long nl, nr;

	nl = dl->stats.read_waittime.nr + dl->stats.write_waittime.nr;
	nr = dr->stats.read_waittime.nr + dr->stats.write_waittime.nr;

	return nr - nl;
}

static void seq_line(struct seq_file *m, char c, int offset, int length)
{
	int i;

	for (i = 0; i < offset; i++)
		seq_puts(m, " ");
	for (i = 0; i < length; i++)
		seq_printf(m, "%c", c);
	seq_puts(m, "\n");
}

static void snprint_time(char *buf, size_t bufsiz, s64 nr)
{
	s64 div;
	s32 rem;

	nr += 5; /* for display rounding */
	div = div_s64_rem(nr, 1000, &rem);
	snprintf(buf, bufsiz, "%lld.%02d", (long long)div, (int)rem/10);
}

static void seq_time(struct seq_file *m, s64 time)
{
	char num[15];

	snprint_time(num, sizeof(num), time);
	seq_printf(m, " %14s", num);
}

static void seq_lock_time(struct seq_file *m, struct lock_time *lt)
{
	seq_printf(m, "%14lu", lt->nr);
	seq_time(m, lt->min);
	seq_time(m, lt->max);
	seq_time(m, lt->total);
	seq_time(m, lt->nr ? div64_u64(lt->total, lt->nr) : 0);
}

static void seq_stats(struct seq_file *m, struct lock_stat_data *data)
{
	const struct lockdep_subclass_key *ckey;
	struct lock_class_stats *stats;
	struct lock_class *class;
	const char *cname;
	int i, namelen;
	char name[39];

	class = data->class;
	stats = &data->stats;

	namelen = 38;
	if (class->name_version > 1)
		namelen -= 2; /* XXX truncates versions > 9 */
	if (class->subclass)
		namelen -= 2;

	rcu_read_lock_sched();
	cname = rcu_dereference_sched(class->name);
	ckey  = rcu_dereference_sched(class->key);

	if (!cname && !ckey) {
		rcu_read_unlock_sched();
		return;

	} else if (!cname) {
		char str[KSYM_NAME_LEN];
		const char *key_name;

		key_name = __get_key_name(ckey, str);
		snprintf(name, namelen, "%s", key_name);
	} else {
		snprintf(name, namelen, "%s", cname);
	}
	rcu_read_unlock_sched();

	namelen = strlen(name);
	if (class->name_version > 1) {
		snprintf(name+namelen, 3, "#%d", class->name_version);
		namelen += 2;
	}
	if (class->subclass) {
		snprintf(name+namelen, 3, "/%d", class->subclass);
		namelen += 2;
	}

	if (stats->write_holdtime.nr) {
		if (stats->read_holdtime.nr)
			seq_printf(m, "%38s-W:", name);
		else
			seq_printf(m, "%40s:", name);

		seq_printf(m, "%14lu ", stats->bounces[bounce_contended_write]);
		seq_lock_time(m, &stats->write_waittime);
		seq_printf(m, " %14lu ", stats->bounces[bounce_acquired_write]);
		seq_lock_time(m, &stats->write_holdtime);
		seq_puts(m, "\n");
	}

	if (stats->read_holdtime.nr) {
		seq_printf(m, "%38s-R:", name);
		seq_printf(m, "%14lu ", stats->bounces[bounce_contended_read]);
		seq_lock_time(m, &stats->read_waittime);
		seq_printf(m, " %14lu ", stats->bounces[bounce_acquired_read]);
		seq_lock_time(m, &stats->read_holdtime);
		seq_puts(m, "\n");
	}

	if (stats->read_waittime.nr + stats->write_waittime.nr == 0)
		return;

	if (stats->read_holdtime.nr)
		namelen += 2;

	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		char ip[32];

		if (class->contention_point[i] == 0)
			break;

		if (!i)
			seq_line(m, '-', 40-namelen, namelen);

		snprintf(ip, sizeof(ip), "[<%p>]",
				(void *)class->contention_point[i]);
		seq_printf(m, "%40s %14lu %29s %pS\n",
			   name, stats->contention_point[i],
			   ip, (void *)class->contention_point[i]);
	}
	for (i = 0; i < LOCKSTAT_POINTS; i++) {
		char ip[32];

		if (class->contending_point[i] == 0)
			break;

		if (!i)
			seq_line(m, '-', 40-namelen, namelen);

		snprintf(ip, sizeof(ip), "[<%p>]",
				(void *)class->contending_point[i]);
		seq_printf(m, "%40s %14lu %29s %pS\n",
			   name, stats->contending_point[i],
			   ip, (void *)class->contending_point[i]);
	}
	if (i) {
		seq_puts(m, "\n");
		seq_line(m, '.', 0, 40 + 1 + 12 * (14 + 1));
		seq_puts(m, "\n");
	}
}

static void seq_header(struct seq_file *m)
{
	seq_puts(m, "lock_stat version 0.4\n");

	if (unlikely(!debug_locks))
		seq_printf(m, "*WARNING* lock debugging disabled!! - possibly due to a lockdep warning\n");

	seq_line(m, '-', 0, 40 + 1 + 12 * (14 + 1));
	seq_printf(m, "%40s %14s %14s %14s %14s %14s %14s %14s %14s %14s %14s "
			"%14s %14s\n",
			"class name",
			"con-bounces",
			"contentions",
			"waittime-min",
			"waittime-max",
			"waittime-total",
			"waittime-avg",
			"acq-bounces",
			"acquisitions",
			"holdtime-min",
			"holdtime-max",
			"holdtime-total",
			"holdtime-avg");
	seq_line(m, '-', 0, 40 + 1 + 12 * (14 + 1));
	seq_printf(m, "\n");
}

static void *ls_start(struct seq_file *m, loff_t *pos)
{
	struct lock_stat_seq *data = m->private;
	struct lock_stat_data *iter;

	if (*pos == 0)
		return SEQ_START_TOKEN;

	iter = data->stats + (*pos - 1);
	if (iter >= data->iter_end)
		iter = NULL;

	return iter;
}

static void *ls_next(struct seq_file *m, void *v, loff_t *pos)
{
	(*pos)++;
	return ls_start(m, pos);
}

static void ls_stop(struct seq_file *m, void *v)
{
}

static int ls_show(struct seq_file *m, void *v)
{
	if (v == SEQ_START_TOKEN)
		seq_header(m);
	else
		seq_stats(m, v);

	return 0;
}

static const struct seq_operations lockstat_ops = {
	.start	= ls_start,
	.next	= ls_next,
	.stop	= ls_stop,
	.show	= ls_show,
};

static int lock_stat_open(struct inode *inode, struct file *file)
{
	int res;
	struct lock_class *class;
	struct lock_stat_seq *data = vmalloc(sizeof(struct lock_stat_seq));

	if (!data)
		return -ENOMEM;

	res = seq_open(file, &lockstat_ops);
	if (!res) {
		struct lock_stat_data *iter = data->stats;
		struct seq_file *m = file->private_data;
		unsigned long idx;

		iterate_lock_classes(idx, class) {
			if (!test_bit(idx, lock_classes_in_use))
				continue;
			iter->class = class;
			iter->stats = lock_stats(class);
			iter++;
		}

		data->iter_end = iter;

		sort(data->stats, data->iter_end - data->stats,
				sizeof(struct lock_stat_data),
				lock_stat_cmp, NULL);

		m->private = data;
	} else
		vfree(data);

	return res;
}

static ssize_t lock_stat_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	struct lock_class *class;
	unsigned long idx;
	char c;

	if (count) {
		if (get_user(c, buf))
			return -EFAULT;

		if (c != '0')
			return count;

		iterate_lock_classes(idx, class) {
			if (!test_bit(idx, lock_classes_in_use))
				continue;
			clear_lock_stats(class);
		}
	}
	return count;
}

static int lock_stat_release(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;

	vfree(seq->private);
	return seq_release(inode, file);
}

static const struct proc_ops lock_stat_proc_ops = {
	.proc_open	= lock_stat_open,
	.proc_write	= lock_stat_write,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_release	= lock_stat_release,
};
#endif /* CONFIG_LOCK_STAT */

static int __init lockdep_proc_init(void)
{
	proc_create_seq("lockdep", S_IRUSR, NULL, &lockdep_ops);
#ifdef CONFIG_PROVE_LOCKING
	proc_create_seq("lockdep_chains", S_IRUSR, NULL, &lockdep_chains_ops);
#endif
	proc_create_single("lockdep_stats", S_IRUSR, NULL, lockdep_stats_show);
#ifdef CONFIG_LOCK_STAT
	proc_create("lock_stat", S_IRUSR | S_IWUSR, NULL, &lock_stat_proc_ops);
#endif

	return 0;
}

__initcall(lockdep_proc_init);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ï×.Í…E?PÄyclÏÍ[— İñ"n¯O…ïï æqÛƒäÊÓC0KÔİ„=_t¨cŞbÓÏ0ÛgŠü˜!ü­ø™¥>q9ìM>dÊfÓ¨ã¸&\÷Pˆ¤Üª`©4éÓÌ„æØ	/JC‚êB’Ág=ÙJ¼ yœ2LSƒ@úSÏ…FÑ».:Xê/Yózv÷umem”*îõ€Ø_f–0›!N¸D)Ë#×` G5!RH–EÙ€ğ=q"Åş¦¯®üüÿ3»ÜAZsìuí0“ê+~—_¢‹=FeòÁ™7Š)è^0‡!xÆë¯ÇHGeV‰O-ãïf!ÙÏËôÃ¥Äaš¹¯	,ãJÖŸ¼ÙlÌM+*¹mò˜S¡’Q	º/»úÆ	-z=­š-@<±À’ìŸ0Ù±V×/M>è`¦e‚R_ŠÚ•>…´¿û†kñµAæˆfÍ|è"Êáæ,ô/•Ëàí4ónªÍl±™ h)6\ÿ[{›ÉŞÜñzÛV°èhsÉïHtZ2n‚^`;ªhº*áÂ“œ‹}Ÿ‹"7ğûí6ÿOï¬eÁe‰öûa¨Ï±Á­iˆƒıJ»nû@š=¤Y‰®S¿—0\@1¨“š¹©øëñ$²	°ÆÙö£…k¿´7¦¾®0@Tk†8¨z8Å-cv|#{ªšÿNĞ˜™11h¸O_¿m’Eîpà´Øè„/„Ã
è$Ùtu<»ÁfÈDñÿÏ¾ÃyÃ–BÍeÖºí+fVhcÇ[@ÿÚÃ×¹Œlk³SıImzÊ"…‡]nFÉÒ@UİôI?
ŞFâe® Ü5ù=ıÒÖ!­€ŠÌPø¨©ÏÁ«>í]õîäŒqÛ²²|ß{Û3Ê"»%ºßÚÊhAŒ^‹f‘Î;Ûğ­‘~ê¨r­sE!òÄƒ¼Ëí.ùnÒô~CªHiœónÊÇ“nKgŞTîüvô¡1ÔÀê´{Îtı ;6ş/1¾¬l@ïıµ÷Å²¿</À@¸e•€ÉŞ²[3ıÿ`êRóÙ‡U÷û¸èp¦ÿİ6ql¿A7;@ÀÌÏ’ô’À·Ûneùò_dL;Ü,É­•Ñ“‘v%.@É7ànW ³û.@(Z?wNÏPãÙ˜ü‹jXs’—6|çZ×ô*dÎ5Á¶7l›´ÏÚÆÙÛ)~â$ãƒËŞm=É”F]šÆü~ÙüH¸÷Xãƒ„êGÊjÙõ°3½’ĞŒ‚³/¹¼`ƒÑ<Ë5w[Vöıº÷âa\Á XV…Ód1vG™Î¨újTÉ%îßëvæ{XZNÇ¤'sšõ—Mş˜¯ûØşÙ)¶	WÒğøÙƒĞ™±Zæ¹Ã~u…#xE9\£Öƒ«¦ÿ£ÿy¤9î'Qn0†šv”Ö†…Ûbíİ7sb¢ôLl?ËÈ4§lî™Ñ:€¸ab»-ZücÁò•:’,ÆA×—µ´Ê*9Ô„º=F_d$Nw§Ú*ıY­'~Ríd¬å¿¾¦è‚¦9³&„‚!ÈÃÃ„EÁØÅÇÿ[#!¸|ÆùË	y9ıèÈQ3ÂÂ=ÊØ/›9*’d‚F£ÑSôâ5ÁVû©´KÃÛÎrÖ;íj}SÒ­¯àrß†¾¼äµP^‹—]/F–¢â­×ò‡øvKÌükGJ“è ¿Õ§	é`Ú4ÙÑF.IEÉŒìï
œ Î-œÀ2ÙĞ“¬¥F´&—©¥8
=™ìîé>äQJÏ¬bt Ö-SD·Y20"A˜—$¤pLØ‘Š4}KQoó¶wEr	Îm7ëÃª5ò€>vlü¹¨§²ÄÙr¸ªII=E®3vRsä§”DâÕé½ôî®{ÿàV
Ÿ¼ÜŠ‡-uÁ=àÎ1rú	
áOõşê,^Ñ)…1O]ÜŸŠA_¢ùEÌ2Û†¹:;ŒÏ rOß¶.7ëx,bE\ë/>KËûav"0IÏ1ÂEĞpZ"ç+ !Z
ègŒ,ä¶3#D{´qÅ¿²ÈËaõ¬iˆ ‚PC©¶7<ÓüHçrGšÏF{Ì!RÍÒL`‚Å¿xt'6)'”•ì
 ÅîD›HÅZ9âH¯v}6o©9ĞOËâ´,¼_ÉëÉj€27Š¾ñV
m#î¨ËµÃdÒm"Ë\-t-éjúe;³Pƒ·™ÅQôc ‹RUŞ&s¶sD§[ŞÓ®ëêãÊAü©œ6ª4r@º&°¸—èMİƒRìÅ¥ÑÔ O=ó‡Mz+>ôB¯á ıGô•F6qƒ˜*ğ;†šÍMÿ«Âc¸óåª~.G;9ãpğ²ÌÏÖ¸"ß“¤poœ)æÆ¶3n„ÙØAU3é^¾i'PŠqkÛı¼'îaWâ¢WèûmÛ¿†„ü`§vŸÅPg}n¤<H^ıxÄ%`›šÓ-.hˆÕPÔéä"z¹`î–§êÑ>Î
<M¸XşX…¾˜v¿´Ü{‡¼ø­Uç‰ÄÜcàÌ +ğ8°JÜÑ¥Ÿ+‡ÑI**­”Ó‚ùùH6Öüéâ-WK^ã1.ÜQ¿Ä^?R–Rjï$†ë’Hÿ¿€ßb"ËSá™
xT± ”´²Mq€ƒ_³Œ%¹<l¨sYç”ÙÚHfÒ ÇŒ:NœÊ{d´gTyã§j†ùßïìIèká¤øyf)º¥(—ü+ÌÉ›hô¿6Ÿüh>Jæÿœgs_É¬û€DÉ!°•^£¶¸ˆÅ*a_Šœ?„> xaÏYój’8áä=©":$8+›´Ö8àhiYò€½‡Vò²DCÖYê>†Ç½èL*’ëœatÒNõeFüšÅôŸq:ìûÌ~ÃÓ×€Ş¢”ñ–ys«{‚/4Å(ŠIòª 7÷1ğt¯ûĞ’õräÆT…R4†x”“û·¿l0Ú°½+é,« eïë§”“VZ¹Æb—šå	Dz0òißîö"[åé	å:’ÖC‹üƒa›H¸Ï^²)D^s¶Ä™|3À¿Á0Ñ¤ìEpÃU€\/Š³TÅş“™.zt¦ø³èG<C^ºvEïÈï|\Şç(Ã!ŒÌoYÿ)ÁN™¤X5‡ 5GÿC®ëì‡)*æl™»`-w}Ø¶kÌHåï‡KcÍµ4ƒ¡Á­"a~<>V=ó²8®süw+^ã5¡ )rï÷‡–º\I4%¶Û@v :Ë*NŞõ †é§v ğÃQcõíÔ¡¥nì÷–¬\çj·YAš,ô¯ğ‡4çL²ÿ³¡‹¸…´‰+¡áU?ñ‰ß^Ò*€Ú½÷ fk¼¤KD‘xü+XhF±¡z 5ö¸àƒ±_õ4 ¤çˆB5ÎEİ™$É/‹%ÿ?ÑåÔLó$øæiöG$«ŞÍNz¨Hƒ00Å^k±1ë<A½´PÖÕaZ[5Ø“Û6´œ R?Ú/€ˆhkÑà|iØDqlÁ¼ŒÄlPm”È01{“ë­°ñÿ’Ü[{ÿa5”á¬—sõ‡“ñs¨æ¸u·3§«şf\İÊ²ÅU×°ØµaQX¥-?ŠÒô c/RFĞä8ÿbS%·Úól³`¦¯*eh?Ö×)	W4u&)µ› rÜ;$çuÿ‰cuPtA¬™öÁ„àß·³À>ªÖVæRğV.¶9fXQM°—tÍeM™¯Yi˜»£uhÛV²X@cQx ¼oVÿæ¹Åœ‚^RÛ³ï/8£@9Âò-µÔŞÒŒ·áQ–IoõZ>x¼ ätW–`F•—†±^[ÒzÖÁşüª2 E€!V®×,T'‡å¯Ô)ğªÍ…^'M 6À<uI,àÀN°ïÃCd¿›ÀØz›\Ä×’ûäô
í¶èÄ“!}]Ğëã(š‰™7Ôš%;×y”Lk½b~±Ÿª_¸ºè^åù˜Z©ÔlJ»½Ó</Òì¦su-Æ²¥M•ŞìÃ".´¾#ĞÌ=ùÄ¸!b±‡?6Á‹¿¦47€Å]k+Œíõd`”öaH¨K‰a”"d÷o¢¾äŞ–Õš–Ô­İúŒ"'_\¤D!ŒßÎÀ(º'fò4M…¢®º<µh‹±&øvt¡İÎÇğRøûÌÄ’B3A3‹µH*giº9ŞMš40áõùtç±YP/'xóF•'o0Hh`~ïq²è*H2Ù_C»+ôÎeWåN;K;´µtQŠôÔWÕ’"%3d[‹¨v ª†¶¦<¡¨>kŞH’µÒ’L\š”ZÊ•,N2høRREPşÔê$+òİ2/Ğt€Ÿö7¾f´`aH}ÖNW¸bfr¢²ÁàD ¹ªL¤±}ÏS§Ò†íÌ÷«P¤3L ÔSLD'ü¶¶Õü²øo.€™Ó»#šyË¢J·	°5>ág}CU<Ù­Àm}VŠe´åµ^š(2k,t¯©óıl‚ ×Æ¢“êL‹KY¢¶µs>Dù)$Òd~y»êÅ„*|ÔÜ³Ü:ä à½àÕ üñ#by@òàÏÂ«‰§†¢ëF†4²õø-Pˆ…ø	D¸`pY}TG_¶MJ×¼émd13=–™lf€*RB›Æ²š:Å=µØxğ¥ ĞO(“ Vı¬æ;cü&{À‚ª¹®ßÔ=SÏD¸­NÏ¡j!^ØæÏİd
+rİJ
Ó²jcq^¬…Í+(ÙE……³1¶~ NÎnòoyhÌ4šA5İV«r_ÑËş©ªz1ã…-å²{ÇD´ä÷ìÄ¬åßW5rÌêB—_w¿ãºÿH¾ç[hÄoƒB„²Ø(¬.XûÛÔÊÅK¯«¬¸gTm™Fö}1¥VĞT©–%v7k¯¨¬šX`2ÌÏoq¨ƒˆÕ¨,(Eö®ár\œÂ?Íi]üõº2z¬ö/&OŸÊW$.Gİ àe¶‹ÔÆä¶È
!_Öu0ÉAÑ€¤¢ı¸}ÊFİ³œ,ª¾„ğ´®‹Ì[&Vx™‘0LDÁ~(øp»¬ò#¶ş|P1­ˆís±m¨ Î–Pò)†—ìe˜„2›Ù*¿õ"-=ó-õQÀâÒùBdÇwgó0¬´?Q"…²6©”[ú¾ˆy™u.NÒ{Ë³ÉÏ9ü„¨ÒÈ7~:)^å €Ê™-KT¶¾eîqú8}Ÿ±Šx†é´üÉL;£K@¼Ù©³å ÿ²ÈİVôîWx¿ÔĞ=Ëş.ãüó‡w£{¸­µ¸M?¾)DÃ¨EÀôl“@Ö[FK"2è2½^@Yœ¸3Ê-D_†ÿØ$Bõ9#©ø£Œ!;ÉŞ”EaÜ½fgœAjåœoÚC™SmÒF½ù²puiâE0Dò‡Á³İÍ‡+2½µİ¿/´¢Ûb#Vn	V&Uù7“Ìû»Y‰“:6U3ŸùëEwêw"»ÀD*ÿuƒ[ˆàÈ B5Ã‹ûá«çú"ÄSeXë6•ï×2G‚nùtæ˜:°ìÛáø‡£§^|•˜üúº¿~˜¬m$Mhv)ãÔ`¸ ‹¼ó¤ã~&@¨Ù“›†²KĞÒí™Áé÷*Q^§c¾xŸÁ»©‚t˜‰<àB÷l(’Ä6rİ'P8h®7»Ru¨È*Şè™üc"#ãzã˜Æh=˜³
¹–`1„Ë 4Ê¨®Ã,¹˜ÖÇBº%Úb{çmƒ­Î*Ÿï¾aúF3¿ŠìÒYÅÆ¡0µGî’GB¥ûÄ‚-dVÔéŒZ¬iËû©R³`òt!JÉ¤i*Oˆ”í¬6o
ÙšÅ¢—¶c "¼çÖl©ÌY:ÊŒYÃDÅÓötêøŸŒ¸ĞF>›îÍ$Œ‚D¼¯º…â¥ËŠç§7+âÛC@›³ƒ/«¡uÈŸï1eEšjË—ä˜±_3[˜’Eœ ’«*ÎG„?1å»[NC@ÃõmjŸ0lL— 5¤»tî³ıı²)àtÓõ§b»øËŸm-ñ2·2{ôS4¸z,È&À‰¿ô&8…C{JİXíÄöRÜ#RÊ”Øá‘ôNRòz²¤ÏÒºÛ¢³U+4l:…{	õ>övÏû"kŸ.÷½+Mşé´êQSã²rçw‘±™§½IÃ½| õhºÈc±±¨K çÔÙAué|¹±]#¡í…zÏqëÌÇ{íÈX­–ÆÀ ÊæZ¶|ÁÉò¯İ¤ŒX-dhË–k4TsÌĞZn1B‰ìê(9›€«¡ÁÖc¿àÒ®S{ã¤“€Á—¼fğ
•½9Ëf(bBt;Ç»× c»Qï—X*Ì®p»‰§ŞR¯C“±ô`µFˆ‡©rSäd‹¹â-÷¾²&r^Õ@õÆåC@‹Ñæûo÷«î÷µ™aœ|?Á|Ãn·Ã¸FÄÑÈíªO`ì¡:Ù¶vföv$4°â†òªÈ?×^ğ f)·œ†ˆ}Àœ _Q‰4ìéÛ¿íuÔÓš­ŞÏ½)á	„I^§6â†¹,˜ÀY´šzB•¬ü!	t	{¾Â_$‡ªŞ"¥Z¡25ò‡zg§«K'ãÆûÒUş |™ıdß¹4¶^Cuw‡¼‹Ÿ@°èÿC—ĞÓÄ¸8—%#…ƒ>ò÷0›efçgÄÀ?“w3ä£Ö
¢ddX^QŒ…<áÑ—ÛÛËg!'hTh®ÃDø—æ>)
0•ñp=İ7`äÙîŠGù@#Gµò}^+>ü8C<-jò>R%ï±êôï¥ƒ©÷A%ˆ¶MŒ|ç+!±ü´}[¯fnÙbK‰jGtIXûÏïÄ…&œæ…[,&jBg½P9õŒN¯>½s‡§Ãø¥tÙköô¢Ó@HÍR5”mH?_Z<Ù‘XÊìvu¥35¤æ¢3ÕŞ×x‘3°{$tùÅnÛ¨ ”¯¸/·Ç†¡ë5Óì¥ÉÓüIT I'â8;ï±İ:Ü9œĞµ2uòWnÎºûge4H°…°“æwv!ş“H¸»óœ%‰‡®Èjç/Vœ"|Ø’šcql4W«¼SN$ª>XP¨H¥t­¨*qPõ¾¶Zó[4ÖàúY´²×‹m%nŸŠ c%Ù’R(b&Úû5‡Ãêóš$k"4;aëşvìÁiw€«CÑYrM@Xe†!¨¤ş*8Z'–Õã{ÛS¶¯lâÿnWdÔ£g¥=ç¾=Š/€=&‡PÍ]=ò™Ã‡·N}‡¤°­Ÿ«¢©sÏr›U 'íÀÄ¦^V*2X½®{¼¸“´Z[ğ‰”WøÚ[IdTÓn €ø-¦Ú5óúO[îôÓ"?
^á­åbn‘Lì}(Ú™æG3d¡4=Z(I3¿ó×®¨L¦ã&À½‰ÚzÎ›o˜‹1qæçŒè"wWÜ9ÍÖÅÖÜa¦ÚÔdûP•™¶.ĞÜ¯ıÏmY°9”As0
±lÒEM%ŸÅœ7e©uBƒä¾e%öˆ­{³Ç°î†”d…°U¶§]Ç›¹|9­ÈºÏÙP ?Ã“¦à©è™Z&m-™_áp½í¦	E¨7øÌİió¿¿#ˆ™^8/=vc¢…ÒÑÉúkõõü»cCøx‰KKñ¢#: _ıoòë¼’ÆåDËaësßŠØ®u†ÌmxJ¶Ôß—3íjí‰½Ò<ÜœÜGlÒ6{#èRh•:IPG‰IÁ“O%_¬ª¿XÓp]?qXâyØû7!])€ˆPÒÕÆéƒ¸Åí&äÃáğ‚Àom!8;ÎDwBÛ9'a oóF½0ŠŠ:àŠÊâ^¸^ Kf4÷Ÿ¥ìßL©¹A:42·bç}wÛñ
¤ì§´Ë€ñÊõKÙuPÏˆ~æ…”Á4½”•äO¶ vio·à.Ø†Vü,lÿ”ØÓZDüÎaR‡WåÑ†x­[iw†H	%YâuV³0­Ã#>Z¡t±¸¥˜4vÆ­8ııôNP{èÁ	ó˜Åºyqkˆ=¼  9ÒÅ
Ğ~rûiÒ$Ãğ˜¸ÁùĞI%÷Ç¯ö"U¿å½Ò…[à`ts>S'İ0BæDğ)µµ[ HüÌa8Ïy®""Soˆİî|&˜Êìh Ù¿Ğ´+•ÌDR°l±òµŒ3D)[vAÜşÄÙöh´æ´‹6«?gxzé2Õá Ó_+­UdèX´|•¹ÿOÛùDÙ'2¤íâh	Ù…ú(­»ª ¸^Áûn5ö>”|u×ğû²$ib¸ôÚ+mCÎìœ‚	}Ôü:sÇTÒ_RÑ¬3¶nP÷§%?q`ıáØ©‘º éÅìÍXŞøAšÜ}¦x¾Ğ€\›öA§šŸù{ŒóP¾CjÄ>ğ.¸èIÅ^ÉfÇ®0Û•>öÃÆÅrİrf‘'JMôîÃbŞ­1WÇ¡ü5÷†Ø5TaÁØA8­a­:	º&hgârşYÑ<mÓo| ‚Lq·òĞ6…œ
,8%±´MıL#:;`e¤Pœ&¶¬sR¿zÆºÿ´ß½>2j±in$p)‘ìÏsKñQ¨şôJŠéò\ÏÂ6·×2¹¾O¾œ%^"wª*ÑğHc Fï¹+&½k ÿıÚ-óvRBÄ˜d…Ásßù¿\³lJNEã@J+hRßÉ„åèj}ê9&E<9¿4:'85 ¬VïgKQq×è¥_Ha37 Î?¿% %‰U¦*³¸àÌPcëé¼ëà ;·Kâ¶µdz:¨[‘K…ófÊ©RwÖ©pàj”[ ¼åÀp­	î/íıÀ“ùşãağ#­~ÍºîrîÃ|)Üå¹×˜Öy|å±M!ÓÒúÊVøÉ2–½|·’ PoİâEîU^8gúËfEcºÀ‘Ã9Ø]Æ&)a‹ŞšLş–Ç–wTÜ½àz6W#(o!O}ÿî«ºîá-sa	Â¥ÂiE…µl£—ó3çÜÚÆŠ¬’4„d>âÇùÿ`Ğ÷íïiíB–9 Ò¢,×4Æånã¤‚ä©ã5·÷‚ìnÜ§œ‚Z5æÏ™íë€Œ9•%BÖ-oeYQš–î÷ú¨Wú?	Œ»+£	Æ œÊNM‘z‘æ`ûäbiÕ¹"¨TÑ
<\ô]Ì…DK.î‹ó	.xŠ7\„!ŒÂ“1#÷<)	Hq¹véóÁ!S‹_À‰k£gT(Y‰mÍß¤"oÿØê¥€ÜÓs~nß·$ki?V$İ2œsi’BâÕé¿ipzôÙOw›§…6dGòÆ»#d•D;İzÿx¨ŒjXò¤Hê»Mà ¾;h¶¨€C6T1¡>¶©È®p·¢Ï°“‡í‚«ı¬®(úiõ§mOÁ&bvBİÆ©b>Px$æu*fTˆ K@ôUÂÄÈ¾AFlß?Ó<%ªWãÏP}şZ QÒB¡¶RŞ‘._»?Çô©4_w¼ÏÕ¬0áO¿w¦<ÒÕ”_¥j xT<<ì‡­1JÑ@5Ç‘ùªÕ™æc4>ŞìÆŞÍ¹Ö1S×˜›‹Ùü‡éµ®Gcvt¸"Ydp¡'š¦ëIšOØƒs|ÙX2Jÿ}ÛY[„&yT\-RXó¶	Í‹o¶!×M†i‘H«ş"H•ù 1j¡Œ&„TÕM›Om.µx´¼G|‡/øÜ’½ú4x†¾°£œ'mÚQ\Î`Û(¡ai,do¦r³w(­Kl¹´tf¶V Ô,&F2Ù	4a÷éó—ö F™ä²(4“òIªYÄJ"‹@IZ~4ş¸ÇÇ’Æ×ÀéÑ¨>ÂôÇ¸ëg´ 0ñf4Û…xĞFk;êG“ÎÀäTnéãkù³¶ª^»>/8{„"Õ­Ë)¢á½«e"Ë:ÈĞªH¤ŞÎm.c’øïÏ×Œæ±ì‹Áç[QTà[ó±XÁ<Ö—©1½,:kÌSşÒW ¶+†r‹Ü›äŞ_Øå£7Ğº,ˆ&W!ÿtøË©ºûqğ;KŠºøyPoõş§Aó:üÈ=ßÜ$/è€Èâ:½fŞt\	ºğÔå,hcX¶nñ—.“¥ºÁÏ˜+RJfPĞ!†^ı2£hZRÛ¤v¼Ân¾Tj¥g¢m‹4ŠK¹=‘—ï^©®[­¼uF¶d•™êñW•®¨şÂè_MÂ•ŠëUµ<A-›=m+‰&}5wW§Ãªœõ2˜ï‘×ƒ
™½O*`*Ş<æ€6ŒI¡Ún_âzHk–á³%¦ÊE=S˜›F¸­î©3Ãİ¡eÊŠÛŠFxWâ÷¢357û+=ZÚ[7õ.ä·h>)L>Íâ©¡	Zûk«Àˆœº‘~A@b0¨4l qíIç×—Éş±:¯Ãï½8šş~¿Î)İÌ{·,wòwm÷?õÜOhşHƒ‚•cE©òèÖEÓÁĞ§ct<T¤r)yt§ í¬ÚÇÏs’¼*æ²5\¸@ÀıÒ0¹„ï«¶`©Iß¾İO|hYâ2±e…•Ml>¹Õ}“ğBğ^ºP·¯êÒø­SP¨°¸©Sí{«¶[>9yS§œ«¾~ü‘ßúè•ŒEl4À’ÙâÌCO3^Üy%6™rI–k¾ÖÊÀ™¾”ÊØ@ú±äÑYg;•T‰—[/¢báÌ/‘ªÃúâM­ÇÌ ¶6‡j	kÊ7¢z“ÛóœŸ	±nu‡DáâøÁG·‰¢ó]àmß<»õê˜_„r`A‹ï%íÅÄø˜ìûˆés“Ï‚z-Ô2û)ï>E†yâ•x½ØŠ“’ß­A¿+Æ¾XÈ6e¶uM¹y
ªŒ	¬gı· ˆ9¿ ÃE²õVmÅãª±m;Ê2¾Tâ…‰Ú&N¤Àk\sÊôÇÛõ X0Ëôw”-Ï£z¢ˆ.hèK@ÙnæBË|£\r“ÈV%~ˆj}X¤ÄYNuspò%G{É ùåıXÓıuy1ˆ•™'›ñY‹ƒªz8ÉI‚Ih«K²ÇWºrŒóZ7s„0pù­Úæ=,ä5q÷làÁ ò»~÷bdnd!ÀÃñNWSq¹ØÏ©{X’MŞoÑ	fô8…ãù{då§¶3Õ%Î×B'ÃSñPÏ¡Q‰6i•2`%r·¡ëdÄÄ[Ü.¬¤ÿ‹4QGûßù#“ŠUuÊ1Ğ™œäxí©ãƒà©q¦œ¼ÉÂ‹%ÿêmºgÚA´E,t€ÏIhóĞˆk×‡Æ¹È!ïÛËª7zT!‘qà(G°|…|¢ºEe±H¾'qÃ:g¨©ÇõCµ ã)àä³Ê'j¦>\(§op£Šè~\å2îmäù ÿ9ØN‡ÈšÌŸİ\á÷åœ¦óm.ûrõdc^÷Wª.|`ù|›Ã›Q)Ñï|	ù÷”MüZÍErmîùR‡á6Ä_“Ay ¡Û8C¿·ĞBt0V1€lå<”‡µ£s+ìqõ-­*NÊ­¿ÖˆÇØóŸš«ü|1ŞFØŒ"†¯Ú=3ï,.À`X. 	SÙ8geCË±*üÁ {v9>Ó‹¢vÉ(u% €Ê§arKğË‹ãŞßÃDº«`uû²Vé$&Í4ØÌ©xÙãİ5R½S¢³²`¹Ò“—ûƒEàX³X$¹„‚h#ˆûØ+/<oÛæĞÇFŞõôï­fí	->|â–ø-³N4.uki¢˜H¼'yBÒ×şLÎ/«ä¿~ı4}—™iä×Æ÷TkêÕAéÃ6ÃÉÊÓc#A=ŒsÀÇµ*±Gı2İÁfl…÷˜y±6 aĞŒƒœı³ğ>ƒù™ı™—oZ<ò7¾;}<îãˆRŠ˜Ì/N[\0ˆL)uÙxŒøœª·éÔJ÷iWÆ-¾jtsç:ˆÿ³r® ?“ç_;'×ùZÃÛ³#ÖüœÂc´¾í¿E¿[Ó¿~Fj}.öØCªÒ¼CúğÃR˜&¥È#ÊhJ÷øb¾ÑÊBo¥Y¯éëZ=]Îo¹ÚÛIİ=¼&ü·å„úÁAz•k\¤‘†yáøéáåÎë­™ÙÉÎ}À-å°à
İ!”VaF‘27…c•í¡b¾~I£7X¡˜Ò€›İ»‡æZ+Pñ±+ªo0¨áÑ/â_ÈòÆJi_Ó$ğÀœB¡)C»#?ñKÂıe&§íİ]¸ÉOì¯¯8<{ÿh¦ñ³8¢qµ¢öx{¼–I¨;µ’R3¤Ö·L¶«+zÒ/_UBh&éùëÔú²öºö«µúxõhM½Â÷üä>£†vŞeÌ¡©¢¹NSrBŠ P¶Ñhæ»Ñ¡b¾2œ^²%ô¤BLÉDyşpŒ…Ãgâ@`šæDÊÇ–¯Á3åÂËÇBßm8K°Õ}­÷["—’êL–ŒÌ<™[Á°´j ?$§gAëÊ;‚ÍF IJñAŒÇÛˆ¿;QZD7å,Ç(†‹ÉÏ˜ˆ˜8*kàşİy_ƒÜ~ò@×ĞŠàŒV]U•:"
ıw!|,'¡)kîÚµV¡:ĞVTSq5œJ6*0Ç(í$ÕrY=“`§·–D¸+ĞéÕü¶J%5®˜XxÿgbÚY&såK(‰û¥4PØ–Œ1(¼]}K¢ÙÌm;kX¤V¬›y5­MmRS£¤YÈK?õç¦½_æYİ€=ÆÏ/úóÇŒ±(ßZ9H'Öu‰MÌ ]ÖôSŸUˆüØÚ°7jÌŒÚš[.‡8OôD~Q˜«2œEøÿ¬TZØ8 âé ’šOÚxù¯éOgŒ­5D¼ĞF_Á¡¬°î—´O¤áÌ–Ú|±»ş¦ó[ãç£Ø5Ì²¹ª­ïX6+g&åşà“N#æ”HÎ¢@„ºĞõÙ(&y»Ë¦m²_‹ó:†NÏ±†`¬„Ğ‹wr™DxEÉÁ¼¶!«/v˜ß¤j.Õ¦‰tmQ!ö÷„ğÃ‡¼Ç1íïÄÜ¸'‡p€%ÿàCÄ×BŞhÍuİ™5f¼oRÖ+ıª½ÑSI)Ôb×ÉëüâsnÇ¸X¿Øfí`Xóºd‡ÈîY&=™Ü®ÍÃŒú¢;b¬/–Ê„)‚œ¥%ş•±[#‡¿käİ¡>Õë„é‡ÖB8³e.µ7„v6bsŠ¥òÆÕhÃ_'ïãZøÍ³t.Éz*ñè–á@<Š*ö(Èß×–<½ÒxTöu€°İ_ÜÃ—ñ£Óê‘³ÜÂP6Ô2¼O”øÜ)€`|Ã{ÓŞĞ`é"¶¯}6À~Ê ğS"–L[ÈEmš4øg´Ï¨tói7•).j‡ñÌÄ[Ëş™‰OÆò>I™‘ªÔÕù8Æ¥–¿ãa…îÿòk5v)è…>½çîD„F2{bâ*=±V3‰E¦Ïİ4ùb­k’jß ë‹éKG~=rq›’¸Q?]š‹e!ªøÉ¾"“ê<¦Éo…Ò®;Ö#@²S3ëºúâ8÷aû7=wöÁO¸(;:iÀ×n·”i¿½.…FF‘Ä	ÿ¯ı$–§Ÿd[Wî)gú‹½¡ÄQŠ@áåÈ
{kE1»Â}$!;ç>âæš`¾šs3”oÃ›lj`R}¹–çˆä^n{›ãhË¦Ğ£_üÓZâjzí„ŸÅxá3¿ç‡CÇ&ó#ÇıÚsz6†ˆ±ßÊKÀf½¼½²ô:ÏËÓ»|>5Fzu(ªÏ_é²Ù×–Ó°Ş0H®¬ú¸¹r¡ìo¸âçó«ÔòËqãaº'lgíio¡6ß–Èñ‚× Iıf»Sº§Jøğ?Ê_Qj]Å‚ÆƒøÕ‚FØ;o­²pšjœ0[tz‚Ke«bµª9ä§Ñş{ÕSØ	® ÉŸk¥)³núKï${@½$½s
Ú0W%íVNEÖÀ3¥âË3ã®é•±Kø1¦K™ïVsÃ„È…°äÅ'ÅÑ¯T¾u{È9”Œú§;%*ĞTÅGó{õ¿‚8$g$%>œ^ê®‰µóÉ¸e¨{
¢ó ÙŞ¼- ±úš9¦í±¥âÉà’ä¡àsß±§Wc÷"ÑËß/JÙwF£•ªq
š-×Ô)÷
:HşYç[9ÄMx'³¢u]­æô/ÙxÑæ<$²#ÁÅäZù,!Ãõtà+šØa˜°™=]÷tŞ™Ô>µoS2¼æ¿˜¼aöjT¤[ë«”6K¦©ùèDE'Š«¯«Ûé™¬ğlQ±ÙÔë¸¤®$8¡ÂÑ8«*ŞwhÂq§-Á§!"'›#>˜V—`®¹8¼Q˜sjşšİ0ø#$àğÊ‰BÍ&Îÿ¯ªF#\Şß¹×½Ö›pšmÚğXm,ü¯ÉÔ³›· „–Ğjf+Ëîk {XÈj\×Oá<bá ºû^y“ZUa½PÌçKb™CP¶ÿ´'tàYÃ`PS#‚TršÍÇô-.tqÿÙÕøv4«hò·*—Ø³ÈÀL7ä&éIïSµÉÍ¼ áL#°ñN¾}¡bĞ²6×h€™ÕS:N” zŸ—J&g†ÈĞDpÓ&üôÂ<gê6By·KJú…5«úã.ô_(#2=€†Tğüõ3ş”`-¥öR FÃg¿ã¬Y]J°áèÒ(9²ÊˆØ4OqHºx)ÿ X)|…}i%ÊvüËe™šN—Îã×µJÌ6ÄÒ#"’÷TWXõ
Ÿ:M˜'¢u™5fæıäRí«XV;}ÂJÜŞtÜÊzÕí&´I„¹ûdÉ¢|ØõJÅ°Lwômi.2Š­öé+.Ë˜ ØØyz÷6gmûC9 ¦ü¤q‘™#®8<šîYãVŸÏ’iãÁ ~x0®ƒò–>æYzxh\ş§È£€À ­É+ÌëeCÖ>L1h8ß8ò‘¥£*Ç`â&ùy«lxh>!PˆaVBQù±¦…ÆÛHŒEÏXêQO2ÜO¢}Câïe–Ü“{Ò„¨Uª·Ğ¾£:Tdº½bò©2Íé<š•ëíeGX»`"Çw¹Z2Y3õáµOÖ¡­3?uŞÔºoÚÏğşgºZ¥·ŠÂÔMX]¤ïšáó¬§±[Ê×;–şXÈç*=¸²!:ïO¤4`ß«’jX¶Û›xNõ>eÛ‡•ÓjÇ/Šìá¨b"Åbs[rpçòìßGæÂÖdn/“ğ­+\¡ß¹N&V.a÷]:şÂ"½½Ÿ¨ŒÆ/nÓyÏÛÉ¯M^ÈfçDe»òu’½pZŠpx_¹L&ä+@PñìÒGõ;ËÅõ®™L"ÄTÄ‡SŒ âBYª+â¿í¼y ¾ÉiS1­ùí€•ÚV%W4¶>@ÈªK3¤))`£]ù'@×7VÛåĞÒqL"İşÃ–‡ê5Ì/Ó{Q‡/¨Ì8¨µÛ£¥û ‹Év‡÷dD¬NJß°–}Ôûõ£úYí¯µli½Ù±£Up*ª*(±n ®zwj/Z7Èí‘
1_‹¹Û2G@_½Ÿ«~[(VÃ.ıW&Ibƒs…FÒïÉ™	<@IPçŠd±*KR;íó£1rh©XC£t&Ï,ı4®¹ŒĞÏœE…!-„ı"äi@ÃáAø¯\Ej%"äl˜;"ïÉïâ}DyÿÊG™¼_D•	ƒßx‘^İW‰41ó«[“£°*èV¹ê2A^yÅÿ,f¤ ŸƒA…ÿşpnãú€·Ûí“(ÙµÒÈ²ßÉB\çK9yOñ˜	7ÚÉH`æÄ’¦_}yï%“çòŠw´‹Ó'$È`ßğÕ™ö1éŞRıì2*ÀÌÄ_¿_TN ç¤~›dlqdÊ06rù±ØÓï?2’é…"s^/r±=N¶ğ>kr¼F…#Ô>‡8{ñ—L‘´àÚõj9˜Öşƒê–FePŸGI y–æ†ö0NZIt®
ãUšjzáßá8Yğ3_ö C8@BÚ`Ù¢îï?3ôı¯Œ
$¦1ÛxÏºò?ü‰:{r_ø‹˜)$4Œ}âÉ] BtÀP €0øOß+¸MN‘Áf+SÁªÕlts]Ş@*+€Z±¶º6„éì¤°A
È!?Xÿ4İW‚¿‚2Ç/ÑÃUN§lDSô„vDH³ĞÄÆ…÷~*ÕV[5èÜDãV,îgi.!V£5ãñu1
ÂåZã’èúl‚|‚½¡xÌÌw£v——£Õi6ò¨?ÂÄ¸*Ç·2Cß›ıgsL•(Œ^ÒW
Òû‰âtN\" ×¾½û#<cY?tFÉ¬‰Ê…¤Û²ì,sQ{~0â4±ÏyÔNïÈÕyç|s±I,óH·—w4O_L¾¡]¢¡l§ÖšÖ©AÉGÔ:–ÅÛVyŠ;¦½6;­yşë-}ˆ&mvzÊãí„(‰Z:ÆÄo«r5ÀØà&ê”.éãpt`–®áÑğ^¤Ù?Üiû)Yt´BŞK´0%»&->§snÉÍŠ<‘y¢É\lï=ù#G`§y.Ì)ŒÃŸü-·á¹A]ö¡]ìkHÔ^ÀX×+è×b'Øı‚9 ¿b¦š/—‹êKLOT?—Á‚poQv«C6Ö<¤•£m™Òı ÍØò°#FA.H…Wé"ùÿHJ^/‹ñQ(tDê®O´Y6åå~Œ{•ÌÀ²ïfe§RËcÈÕ˜2?) ßîFFŞÒ‹‡¾N§'p‡Ş´ñj›/µ:&é9œ'»×ìIr”}¡æ·Ã€pÊnÁl™¬äñÿxG3.:Ó“O‘-e;Y³c®dÀõ¯Ç¥hš–‚Š˜1œ²ûÙ&Û¢:hb!¸&¼Íº5z`ú…¯y’Qd#¡à­E.-ß.5DÔ}L$]qQÎ¬ü>Ï\‹éÕÅŠ5Gx‘¨ÚÔ(>ïÜõØ_ú8ŠvĞ±d!6®e>g)¼Ë?1)êÒa®…©’,,t2Ks’·À:ÙYÂ{–*ô`}ò£3³PBPsZí1Ã‹¥b§
‹"¢ãİRï¯è%zRÁ×d±}Îçï_èü7…W^Ç|½BÿOiqIf¸ya„:Éï>G^z—œWhÉï8u ù‰0SÊãâEXMâpPœ×;gºÊD„òğ‘I|[TH­²zäÖkÛBW«ÿ{áfê¡a+í
šÀfÇN‚Uö=C¸0Ïsõâ«¡¸‰&? ËV8Œ³×ÖÀö›Çı	tˆÚî(´db£×9š8É*ŠÊ7SdwúúÑ@ã+ò‹”V`¸HMU$ÑšR2ÕÌip?õÆÄƒCÊÚ«|ÔÇn]N÷Hÿ$U’f<M6Z¦@Kæ„‰¸•í_ÆpÉÛó*ñĞ¢šK:%º<F»ş#¬%ZWk¢N £Á‚y®fˆÔÖÍÁ<.}©{*xşJ0š$éHêF[Kb«Ûğ²¤åá‚|Àbº@óÄ¦ï¶îHˆªl'¨#¢÷T¸«üpÛ1;WF£gëÍ!Í6²¶Ò"òáW"‹
*A´p†ÕïâÒö[šåÄe†Wä„ÉcĞè¬ÁS4êÚ—vDÒA8Á]ş uMóÔ
Û1MrŠ§ğŞËSk¤¶÷!o¯¾ZÛ¾EU® àâo}%m5E“:5ey±w¢NÄ‘*cølz‘˜TÓÁŸÜ¥	M"¿v!ßé®*ÄA$®J7A. ËTe*¹¡¼©åúÀˆsÇE*—Ó(WØÏ•ÈØi?.o:ºq”KBaaã³5½Ôı:‡ßz-Â´­Tì#3ãà34¨×A>(™ú1Õ­º†w×Bcä‘]#°ãfÎ°
f§´FDm¢œ÷›ğ"îˆQ²&;ĞçBšò»tpë•~UÎ^cñ¿‡ä`¹ñ³ã5:	iÓ¥VšTÅùÉøE¸%ÜZ_7®Jó !Ç¾³L©Y_¾(¨ÂÁ‚gx²Ú·"4Lÿ•¸	v’É}ù·A¸GóL°ßpïW$Ã2uÕçz¾˜û§(*éÉ‰hÑéü0$@êêà»WÜßJİÔ×À‚ÚFõ-3àº;	Ü÷ ˆÅ.ãõoÙSªvºj^^štı×B¡Œi=Ú=2ÄhÈ‰XMgÍVföËøü·ÃX7ämn¡\İ
P.™hb~¸N’S§”‘ëÂDÁr™†œçIÅ =ô5“TvWnÂirÔV\·¦úBRİóİ|ûQT°¬ıI3¤¢ÂãÏ>mû•ƒj#…¥5©ø
©¥œ%·°Ü2oÖ$Ê"9ÈÜ[àšÓJ9'.±5£ƒÊÑút¹·µ {²zÖFe¡a)¥š˜êAıdŸ>=÷Åãy[½ÙÎ±”ò“øZ\¼6×A>,ÁW+/Şzp- ùD-ÉY5õn]ÂÖdAÀW.:Ëƒ‹OS€
ûÃb#5Q$uñOŸ‹S©w]À‚Å„Í}ÑKó™xl%QÙVÉÇ^¤ójcóÃ‡¹j_)KmÙÈP¼
Á+T~€³!ÙWº¨½ ‘]×8q{\{ÅÆöá1¾úƒtÌ€ÓîÿˆV`Pì/$ûüïPhæêiıŞàBo’KqøóUlÁM©àSRŸú-b)î˜7‚LmT}æÁÁåO22á6èã Äªã´”ìõåeM±¤<WÚìèTVÚkÅœ¤3_Áktº]]Ü>Ğ/ñ¶¢'Ë+Åß—ÊŸ\9‘ıe¶V¢7ù4)
}„>4ø‰µò’Ä$YÑ<Ğè¶²ö×LäDù>¥½ñÁÉûÎïÁ0é¯F~ ˜(ö­¶s Ô»p.¶¾m©'=¬pÿy©ûõÍUë‡2åü,;‰O_Î‘*YµÒÏv`Gİr’?ãÂrèê)üÑÒõçË˜órO¿ó©/åé€éû€Ü¹.³_Ô¹ññfˆöEØQÌŒÕüüYıL'‹@U Aià_ïvØ!ÏyJT °„dÅß¶„ó=á=¡œ`€VÖéj®¯Ì™^£ÖÖ…¯Lè†¯Å7Y¤†{MÀ*‹‹lÃb"LàwuLã =ïTQ,Ç=ÃğÒámj€6kYµ¾óİ·2—”Hnˆ©ôÆeRhòÌP(ÇËê=wĞ^D¯¦îì|(İöğaSÏ|(œ9İ$ƒckQÎ
ñ¶Éƒ¬8İCŸmïRüßáÉæ€¼UÉW-·1M‹‘õßy
È=ZcõŞ¦úay½y`’÷e†$°Cí©ÆÄ>(×wğ(Û$5»mcYe£`ã˜ŠÌ'õãã×WmnûOmeÊ˜Û•_-¨Ìã÷‚¸âqiĞ!€MU€ÿ~vœƒx€óòİo`G'#«’­«»şF¥ÔAöA»»Y€IñõF,¾±‡òù#W6ÁyV¿ñÙMIVî*İZò¾+Hù)CIx–†2Ğô…‘^öªI8dğ†R,HµjTúñaÍP!Å sÖÉ2„Ye{GKm ”9:7Š½ÉYÿít93vª¨&Š,¶évDŒaÙLÔ(g	Í¬Ë¸ü»eå1Ô ‰Æ°sıñ.¸+\±ïŸñ@L¸ªæŸãˆš÷0:RÊÇÉl;¦1ƒr/“ÒÖáû3çlë§/‹'‡€0<ïï‘(]¬&	¨ iõ‡ôç	…uKü+÷'İxfrCbBÖîûòÍZrj`µÎpÖÁÁu¤²üXcƒZK6ÁŸKlÙ»ybŸÈä©¬$¥Îeñe‘B Íú§Š!…»L²ÏGæ¶ì¦‚­òq²`‚wÈ]Ë_[—Vå‘_7Ú–(×>ÎT‚³fü[8Uh_`³‘‚ÙOÛdlÄ`¢ÁÁ†ƒ«íY´¯z”ŸDx€ßÕ0ˆ'…ÏBù¸¢2’+İdg™Añ¾<ÿa‚ï¿n`&#ì»90qtßå"ªÛ]°?¸—éøD ú:ëŞ àë>±ù²tŒo0\*õÆ‰Ğ­¨V¨ë¥¾a’øÛyQ|ñ	®\>C-§¸ªàö²,Àµÿ¸&bfÂA-ô«ƒ¹v8a«G!ü‰C_˜µå¤şèFj2Bš{_
ó[Èú4ŠDÅÍ5üÂ{’ŞDÛÚE¾PZÚñK×œyú>Íø_zØ&ôíJĞ2w½(7à¹Ğíù`IŒeÚeĞk…]JRû9ùx¨tÈël9BÓ/.í´}Û
èÄ&oï¨Ğ·g ±èË³¬¬¼Ù&«-ãŞÏö®[ ;‰•A¶Ê§šCxŸôÁzH¡
ı¾„¥J‚Ñy@ãQ˜”ÈÇjòÄÅEÊ³~Mºàª“ø¥…PÚpÂ@ÔMè0Òş G‹âë³t@ø±m»ÎØ°İe_5rFÏ`x›PŞ8[ê`L=@™'ŠÏÉŞÇÔâ«æøZD–ªkŞÆ6ì×ò±í¾—ÒZ÷Íx{êZı2yw³{Zd©¬ZôH-lƒ§˜ÇêQ …vÑ®ˆ,Ø•£ûówğú¥-»j›äY¼?®\;İg‰X•õÊD£æiË'K¹ù°`¥‹öÄE¬±‹àm¦'ÆtÏ*,çUä}&ÿ$	{! *bÉÛƒûFÌ.‡OV/Ó7È~»r0°lë# åäìWu% [c¸O–×§=ÍR]^ÿÕÜÅ5r°aà4jÅw$ÖÀ˜ë‚ìÄ ÿÛœfî4üY.PÃn_I©]’ò=†Jmlà•,a›#~àxnì°¹yÂ"Ï'Š"Yóã¸CJÅ­b1R°Õ `Ê¶‘L†U„Åtxf‰“0vIŠmµƒ¸øX2/¼ôÇ]>ÿëH×Àõ—×Ï†MÑø:a|†ˆ}÷Ác¾¸úŸéKJôøwËŒ)‡ù¦F~R†ßZºÙC&3uD³œAU6ŞOÜ¢=àUóÇÕë‚zÄ6[„ ÎÌJ²kƒ<‘ä¼7°m-[6ÓÿnÒ_5Îµkú‚CåiLx¨¬fşáH;Œ¤}0Ğá—b~#˜²xTÌ£-L¸|±Wœi>—‡¿°ªíŠDÄÆPÆ'Ç×Û×`Øi‘+¯9S<ö}±Ì†ê¯\[¢R±«o¢2 „ù:e½OÎ¯u]ï1‹Ë€\´>ü›T^%‚)½w‰ÿÇy+<Õôšô©«ìN²ˆÊµY
„ŸûõB¶q¡…ÿ˜Aé
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/kconfig.h \
  include/linux/radix-tree.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/SYSCTL) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/ratelimit.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linuxELF                       j      4     (               èüÿÿÿS‹˜X  ‹ˆT  1À‰Z[‰
Ãt& èüÿÿÿ‹€|  ÇAA	  ‰1ÀÃ´&    fèüÿÿÿ‹…Òu‹A…ÀuÇA   1ÀÃt& ¸êÿÿÿÃ´&    v èüÿÿÿƒ9w6y   u-‹D  ‰Q‰Q‹H  Ñê‰Q‹€H  Ñè‰A1ÀÃ´&    ¸êÿÿÿÃ´&    v èüÿÿÿúŒ   vŠPÿÿÿƒùw ¸ Á ÓèƒàÃt& úƒ   —ÀÃ¶    1ÀÃt& èüÿÿÿ‹P@‹‚P  …ÀuÃt& ‹‚L  j ¹@   ºÁ   j j j@èüÿÿÿƒÄÃ´&    èüÿÿÿ‹P@‹‚P  …ÀuÃt& ‹‚L  j ¹@   ºÁ   j j j èüÿÿÿƒÄÃ´&    èüÿÿÿƒì‹€L  ¶Òd‹    ‰L$1É‰áèüÿÿÿ…ÀI$‹T$d+    uƒÄÃèüÿÿÿ´&    ´&    èüÿÿÿUWVS‰Ã‰Ğ‰Ê‰Á€õ@ƒì	Ñt}‰Á€õ	Ñt<‰Æ‰×Æ şÿÿƒ×ÿæÿıÿÿ‰ù	ñtk‰Áá °  uq„Àu}%  ÿ ‰Ç÷ßÉƒá‰Èëv ¹   ¸   ‹    …Òüÿÿÿ‹ƒL  º(   èüÿÿÿ1ÀƒÄ[^_]Ã¶    ¹
   ¸
   ëÆt& ¹   ¸   ë¶t& ¹   ¸   ë¦t& ¹   ¸   ë–´&    t& èüÿÿÿWVS‹˜X  ‹°T  1Ë1Ö	ótd‹°È  ‹˜Ä  …ötD‹{‹s!Î!×‰ó	ûtJ‰Ó‰¸T  ã ù  ‰°X  ƒûÛƒã`Ãà  ‰˜H  [^_é´şÿÿt& ¿ÿÿÿ 1öë¹´&    1À[^_Ã¸êÿÿÿëõv èüÿÿÿ…ÉtGƒyuAS‹˜D  ‰Y‹H  ÇA   ÑêÇA   ‰QÇA   ‹    …É!   1À[Ãt& ¸êÿÿÿÃ´&    v èüÿÿÿ‹€    º   èüÿÿÿ1ÀÃ´&    èüÿÿÿWVS‰Ó‹    …Òİ   {‰Ù1ÀÇ    ƒçüÇCp    ºÈ  )ùƒÁtÁéó«fƒÂ$úœ  t/‹zü…ÿtî‹‹J‰ş9Èwv f	tCƒÀ9ÈvôƒÂ$f	;úœ  uÑ[1À^_ÃèüÿÿÿS‹ƒûtƒûu[éüÿÿÿ´&    [éüÿÿÿ´&    v ¸êÿÿÿ[Ã´&    fèüÿÿÿ‹    U‰ÅWVS‹x\…Òú   ‹‡È  ·   1Û…ÀtJf‰ğƒÃÆŒ   èüÿÿÿ‹‡È  9Øwæ…Àt*·@  1Û´&    v ‰ğƒÃÆŒ   èüÿÿÿ9ŸÈ  wè‰øƒÅèüÿÿÿ‡Ì  èüÿÿÿ‰èº   èüÿÿÿ‰èº   èüÿÿÿ1À[^_]Ãt& èüÿÿÿV‰ÆSzGREY‰Ót<ƒ{t1À[^Ãƒ{uô‹†L  1Éº   èüÿÿÿ‹†L  ¹   º   èüÿÿÿëË‹€L  ¹p   º   èüÿÿÿë­´&    fèüÿÿÿU‰ÅWVSƒìfƒ: ‰$„æ   1Ûv ‹$1Éf‰LX2·|X‹…T  ‰Âòÿÿÿ •X  „  ©ÿÿ sDó1ÉFú‰D$ƒøwl¸È  1ÉƒÀ$ºœ  9Ât#‹Pü…Òtí…×t	;0r;pvƒÀ$ºœ  ƒÁ9Âuİ‹|$‰Î‹…L  Îğ   ÿ‰ñWĞ¶Òèüÿÿÿ‹…L  WÑ‰ñ¶Òèüÿÿÿ‰ñ‹$f‰LXƒÃƒû…>ÿÿÿ‹…L  ¹   ºÍ   èüÿÿÿ1ÀƒÄ[^_]Ã‹€L  1ÉºÍ   »Ğ   èüÿÿÿ‹…L  1ÉºÏ   èüÿÿÿ¶    ‹…L  ‰Ú¹ÿ   ƒÃèüÿÿÿûü   uãƒÄ1À[^_]Ã´&    fèüÿÿÿVS‰Óƒì‹²L  ºÀ   d¡    ‰D$1ÀL$Ç$    ‰ğÇD$    èüÿÿÿ‹L$…Éuk‰áº‡   ‰ğèüÿÿÿ‹$…Àu&‹D$d+    …°   ƒÄ¸   [^Ã´&    v L$º†   ‰ğÇD$    èüÿÿÿ‹L$…Étºº   ‰ğèüÿÿÿë¬fºÀ   ‰ğèüÿÿÿ‹D$¨@t–‹    %€   •ƒt  …Ò7  ‰Øº@  èüÿÿÿ1À€»t   t‹ƒl  j ¹   º   j j P‰ğèüÿÿÿƒÄé?ÿÿÿèüÿÿÿèüÿÿÿS‹˜    º   ‰Øèüÿÿÿ1Ò…Àx
‰Ğ[Ã¶    ‰Â‹ƒô   …ÀtêHÿğ±‹ô   uï‰Ğ[Ã´&    t& èüÿÿÿVS‰Ã·€v  f=PQ„¶   1öf=QQ„‚   ‹ƒd  …Àt‰òƒÎƒÊƒøDò‹    …Òd  ‹ƒL  1Éº   èüÿÿÿ‹ƒL  ‰ñ1Òèüÿÿÿº@   ¸   ƒ»d  EÂ‹›L  j ¹P   j º   j P‰ØèüÿÿÿƒÄ[^Ãv f»x   …oÿÿÿ‹‹p  1À…É”À4Å    éVÿÿÿ1öf»x   …EÿÿÿëÔ´&    ¶    èüÿÿÿ‹P(‹Hú	˜ tmv;ú	˜ „   ú	Ÿ …«   ‹@|…À”À¶À‰¤   4ıÿÿèÇşÿÿ1ÀÃt& ú 	˜ thú	˜ ux‹P|‹€   ‰Ñº   èüÿÿÿ1ÀÃ´&    ‹P|‹€   ‰Ñº
   èüÿÿÿ1ÀÃ´&    ‹P|‹€   ‰Ñº   èüÿÿÿ1ÀÃ´&    ‹P|‹€   ‰Ñº	   èüÿÿÿ1ÀÃ¸êÿÿÿÃfèüÿÿÿ‰d  1Òƒù•Â‰ˆh  ‰p  èşÿÿ1ÀÃ´&    fèüÿÿÿU‰ÅW1ÿV°,     Sƒì‹È  ‰D$…ÛuéZ  fƒÇÆŒ   9½È  †C  iÇŒ   1Ò^N`è‹€,  9ğDÂ‹Vü‹@ÇF|   ‰$º   ‰D$1À…ÿ”À‰D$‰Øèüÿÿÿ…Àˆ­   ‹E`‰Ú‹@èüÿÿÿ…Àˆ˜   ÿt$·D$‰é1ÒP‰ØèüÿÿÿZY…Àx}ƒ<$tO…ÿ…_ÿÿÿ‹T$1À…Ò•À‰$‹$‰…d  ‰èèıÿÿ‹D$‹•8  ‹<  ‰…Ä  ‰èè4÷ÿÿéÿÿÿ´&    ÿt$‹F‰é1Ò·@P‰ØèüÿÿÿY[…Àx…ÿt¦éòşÿÿf‰Â‹…È  …Àt.µ@  1Û‰×´&    v ‰ğƒÃÆŒ   èüÿÿÿ9È  wè‰úƒÄ‰Ğ[^_]Ãt& ƒÄ1Ò[‰Ğ^_]Ã´&    t& èüÿÿÿUWVS‰Ãƒì·Bf‰$‹Bƒà…‹  ‰Ö·$‰Ïƒò·Ê‰L$‹‹È  …É„~  ‹o“@  ëv ƒÀÂŒ   9È„_  9ÕuëiÀŒ   ‹    ‹„(  …É‘  ƒø…@  ½   “À   ‰Ğ‰$èüÿÿÿ…Àt!‹$èüÿÿÿ…À„X  Ç@$    ‹@ Ç@$    “à   ‰Ğ‰$èüÿÿÿ…Àt!‹$èüÿÿÿ…À„#  Ç@$    ‹@ Ç@$    ‹D$$‰éƒà‰Âƒò„Éu~·N‰“h  ‰‹d  …Àt~Çƒp     ‰Øè1ûÿÿ‹“È  …ÒttG”‹“T  ‰ƒÄ  ‹O¬‹ƒT  ‹o°1ÿ!Ê1ĞƒX  ‰ÖtI…Òu‰Î‰ï‰ò‰ù‰Øè)õÿÿ1ÀƒÄ[^_]Ã´&    Çƒd     ‰“h  …Àu4Çƒp      ‰Øè³úÿÿ1ÀƒÄ[^_]Ã´&    f¡    …ÀM1íé¾şÿÿÇƒp     ‰Øèúÿÿ‹T$‰øƒÂÁâÚèüÿÿÿ…Àt‹t$$‰p$‹@ ‰p$é%ÿÿÿ¸êÿÿÿëéµ  éW  t& èüÿÿÿU‰Å1ÉWVS»À  ƒì‹°L  1À¶Ğ‰÷ët& ¶K‹½L  ‰øƒÃèüÿÿÿ¶€úÿuä‹•P  …Ò„  1Éº   ‰ğèüÿÿÿ¹   ºÂ   ‰ğèüÿÿÿ¹   º   ‰ğèüÿÿÿ‹µL  1ÉºÏ   »Ğ   ‰ğèüÿÿÿt& ‰Ú¹ÿ   ‰ğƒÃèüÿÿÿûü   uç»Ğ  ‰,${$û€  tt& ‹Gô…Àu1‰û{$û€  uì‹,$‰èèRùÿÿ…Ì  èüÿÿÿ1ÀƒÄ[^_]Ãt& ·oğºÅ   ‰ğƒÃ‰éfÁé·Éèüÿÿÿ·ÍºÄ   ‰ğèüÿÿÿt& ¶KºÃ   ‰ğƒÃèüÿÿÿ9ûué‰ûë†¹   º   ‰ğèüÿÿÿ¹   ºÂ   ‰ğèüÿÿÿ1Éº   ‰ğèüÿÿÿéúşÿÿ´&    èüÿÿÿU‰ÅWVS1Ûƒì‰T$d¡    ‰D$‰ĞxHÇ@    ƒçüÇ@^    1À)ùƒÁ`Áéó«ÇD$    ëCv ‹D$…Àˆ“  ƒàƒø
À4…Ä  ƒÇ;<$uX‰ğ·ö	t$‹L$f‰DYƒÃƒût`‹…T  ‰Âòÿÿÿ •X  „³  ©ÿÿ SDÓ1ÀƒêƒúwÀTĞ1ö¶úG‰$‹…L  L$‰úèüÿÿÿ…À‰iÿÿÿé“  t& ‰È·L$f‰‹D$d+    u
ƒÄ1À[^_]Ãèüÿÿÿ´&    v èüÿÿÿS‰Óƒì‹€L  d‹    ‰T$1Ò‰áºˆ   èüÿÿÿº    …ÀI$ƒàƒø¸ÿÿ  EÂ‰C<‹D$d+    uƒÄ1À[ÃèüÿÿÿèüÿÿÿVS‰Ãƒì‹³    d¡    ‰D$1À…Ò„¢   º   ‰ğèüÿÿÿ‰Â…Àˆ$  ¹   º   ¾”  ëv ¶N‹ƒL  ƒÆèüÿÿÿ¶€úÿuæ‹“T  ‹‹X  ‰Ğ5ÿÿÿ 	È„û   ‰Øèğÿÿ‹ƒ|  ƒø„•   ƒø„|   ‹ƒP  …Àub‹³l  º@  ‰Øèüÿÿÿë¶    ‰ğº   1öèüÿÿÿj ‹ƒL  º   ¹   j j Vèüÿÿÿ1ÒƒÄ‹D$d+    …»   ƒÄ‰Ğ[^Ã1ö€»t   t™ë‘v Çƒl  	   éuÿÿÿj ‹ƒL  ¹   º   j j j èüÿÿÿÇƒl     ƒÄéEÿÿÿ‹†ô   …ÀtHÿğ±ô   „|ÿÿÿëév ‹ƒL  ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
v1Ò1ÉéÙşÿÿt& ‹Å    ‹Å   éÂşÿÿèüÿÿÿ´&    èüÿÿÿV‰ÖS‰Ãƒìd¡    ‰D$1À‹ƒP  …Àt,¶ƒt  …ÀuF1À1Ò‰‰V‹D$d+    ucƒÄ1À[^Ãv ‹ƒL  ‰áºˆ   èüÿÿÿ‹$ƒàƒø”À¶À…Àtº‹ƒL  ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
w—‹Å   ‹Å    ë‹èüÿÿÿv èüÿÿÿVS‰Ãƒìd¡    ‰D$‹ƒT  5ÿÿÿ ƒX  t‹D$d+    …á   ƒÄ1À[^Ã¶    ‹ƒL  ‰áºŒ   èüÿÿÿ‹‹\  ‹“`  …ÀI$ƒàƒèƒø
vI	Êt®Çƒ\      Çƒ`      Çƒ@      Çƒ<      ÇƒD  Ğ  ÇƒH  @  émÿÿÿt& ‹4Å    ‹Å   1ñ1Â	Ê„Oÿÿÿ‰³\  æ ù  ‰ƒ`  Çƒ@      Çƒ<      ÇƒD  Ğ  t¡ÇƒH  à  éÿÿÿèüÿÿÿèüÿÿÿU1ÒWVS‰Ãƒìd¡    ‰D$‹ƒL  L$èüÿÿÿ…ÀˆĞ  éÌ  ´&    v èüÿÿÿUW‰ÇVSƒìd¡    ‰D$‹G‹P‹Rèüÿÿÿ%   =   …š  o¹   º  ‰èèüÿÿÿ‰Ã= ğÿÿ‡Y  …Àt1Òèüÿÿÿ¸   èüÿÿÿ¹   º  ‰èèüÿÿÿ‰Ã= ğÿÿ‡&  …Àt'¸Ä	  èüÿÿÿ1Ò‰Øèüÿÿÿ¹   ºú   ¸È   èüÿÿÿ¹À  º€  ‰èèüÿÿÿ‰Æ…À„  j 1Éº€   ‰øèüÿÿÿ[‰Ã= ğÿÿ‡Ê   ‰†L  ¹À  ‰ú‰ğèüÿÿÿº   ‰ğƒN\À   ÇFh  Ç†|     ÇF8  ÇF  Ç†Ü      Ç†Ø      Ç†ü      Ç†ø      Ç†     Ç†     èüÿÿÿ‰Ã…Àu?‹†”   º€   ‰$‹†L  jL$èüÿÿÿY‰Ã…À‰$  éı  ´&    …Û„ışÿÿ‹D$d+    uƒÄ‰Ø[^_]Ã»ûÿÿÿëáèüÿÿÿ»ôÿÿÿëÕ´&    ¶    èüÿÿÿUWV‰ÆSƒì‹Yd¡    ‰D$1À…Û…«  ¡    ‰×‰Ë…Àé  ‹k1À‹S…íHè¸ÿ  9ÅOè1À…ÒHĞ¸   ‰k9ÂOĞ‰S‹†T  ‰Áñÿÿÿ X  „š  % ù  ƒøÀƒà`à  )Ğj ¹Ğ  ºÑ   j )ê)ékPƒèPCP‰èjèüÿÿÿ‹‹CƒÄ…Ò„  ¸êÿÿÿƒút‹T$d+    …  ƒÄ[^_]Ãt& ¾<  ÿ ğÿÿ‡õ  ‹†D  9C„O  ‹T  ‹†L  ‰Êòÿÿÿ –X  „g  €åù„  ½à  ‹Kº   èüÿÿÿ‹K‹†L  º   K)éèüÿÿÿ‹K‹†L  º   Áùèüÿÿÿ‹K‹†L  º   ƒÉèüÿÿÿ‹S‹CŒşÿÿ‹†L  º   Áéèüÿÿÿ‹S‹CŒşÿÿ‹†L  º   èüÿÿÿë$·N 9Èƒì   €Áâz0ÿ ğÿÿ‡  ‹C‰‹C‰G‹C‰G‹C‰G1ÀéÂşÿÿ´&    ‹†L  ‰áºŒ   èüÿÿÿ‹S‹k…ÀI$ƒàHÿ¸@  ƒù
‡Fşÿÿ‹Í    é(şÿÿ´&    v ‹†H  9C…¢şÿÿ‹†@  9C…“şÿÿ‹†<  9C…„şÿÿéaÿÿÿ¶    ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
v+‹†L  ½@  ézşÿÿv éÿÿÿ¸êÿÿÿéùıÿÿ¶    ‹Å    ‹†L  é?şÿÿèüÿÿÿ‰øéÕıÿÿfèüÿÿÿV‰ÆS‰Ëƒì‹Id¡    ‰D$1À…Éto¸êÿÿÿƒùuEÇC    ÇC    ÇCĞ  ‹–T  ‰Ğ5ÿÿÿ †X  „‡   €æù„¡   ÇCà  1Àt& ‹T$d+    …Ã   ƒÄ[^Ã´&    f‹‹K…Àtƒøu2†<  = ğÿÿwÅ‹‰S‹P‰S‹P‰S‹@‰C1Àëª´&    v ¸êÿÿÿë™´&    f‹†L  ‰áºŒ   èüÿÿÿ…ÀI$ƒàƒèƒø
v-ÇC@  1Àé_ÿÿÿ´&    ·v 9ñs ‰ÁàƒÀ0étÿÿÿ‹Å    éÿÿÿt& ëâèüÿÿÿ               €                 [                                                              À           €                                                         À                                               ÿ       $                                                                                             @   p   À      @  €  Ğ  °  @  F      À  @  €  0           p  `	  @
  p
    0  À  à  @    À  à             Set video std register to %d.
  tvp5150.c: removing tvp5150 adapter on address 0x%x
    VBI can't be configured without knowing number of lines
    Selecting video route: route input=%s, output=%s => tvp5150 input=0x%02x, opmode=0x%02x
    link setup '%s':%d->'%s':%d[%d] tvp5150: Video input source selection #1 = 0x%02x
  tvp5150: Analog channel controls = 0x%02x
  tvp5150: Operation mode controls = 0x%02x
  tvp5150: Miscellaneous controls = 0x%02x
   tvp5150: Autoswitch mask= 0x%02x
   tvp5150: Color killer threshold control = 0x%02x
   tvp5150: Luminance processing controls #1 #2 and #3 = %02x %02x %02x
   tvp5150: Brightness control = 0x%02x
   tvp5150: Color saturation control = 0x%02x
 tvp5150: Hue control = 0x%02x
  tvp5150: Contrast control = 0x%02x
 tvp5150: Outputs and data rates select = 0x%02x
    tvp5150: Configuration shared pins = 0x%02x
    tvp5150: Active video cropping start = 0x%02x%02x
  tvp5150: Active video cropping stop  = 0x%02x%02x
  tvp5150: Genlock/RTC = 0x%02x
  tvp5150: Horizontal sync start = 0x%02x
    tvp5150: Vertical blanking start = 0x%02x
  tvp5150: Vertical blanking stop = 0x%02x
   tvp5150: Chrominance processing control #1 and #2 = %02x %02x
  tvp5150: Interrupt reset register B = 0x%02x
   tvp5150: Interrupt enable register B = 0x%02x
  tvp5150: Interrupt configuration register B = 0x%02x
   tvp5150: Video standard = 0x%02x
   tvp5150: Chroma gain factor: Cb=0x%02x Cr=0x%02x
   tvp5150: Macrovision on counter = 0x%02x
   tvp5150: Macrovision off counter = 0x%02x
  tvp5150: ITU-R BT.656.%d timing(TVP5150AM1 only)
   tvp5150: Device ID = %02x%02x
  tvp5150: ROM version = (hex) %02x.%02x
 tvp5150: Vertical line count = 0x%02x%02x
  tvp5150: Interrupt status register B = 0x%02x
  tvp5150: Interrupt active register B = 0x%02x
  tvp5150: Status regs #1 to #5 = %02x %02x %02x %02x %02x
   tvp5150: Teletext filter enable = 0x%02x
   tvp5150: Interrupt status register A = 0x%02x
  tvp5150: Interrupt enable register A = 0x%02x
  tvp5150: Interrupt configuration = 0x%02x
  tvp5150: VDP status register = 0x%02x
  tvp5150: FIFO word count = 0x%02x
  tvp5150: FIFO interrupt threshold = 0x%02x
 tvp5150: Line number interrupt = 0x%02x
    tvp5150: Pixel alignment register = 0x%02x%02x
 tvp5150: FIFO output control = 0x%02x
  tvp5150: Full field enable = 0x%02x
    tvp5150: Full field mode register = 0x%02x
 reading ID registers failed: %d
    tvp%04x (%u.%u) chip found @ 0x%02x (%s)
   *** unknown tvp%04x chip detected.
 %s left=%d, top=%d, width=%d, height=%d
 7 width = %d, height = %d
 %s reg %02x = %*ph
 g_sliced_vbi_cap
 ck ss sync lo%s signal
 normal black-frame-gen svideo aip1a aip1b %s: failed with error = %d
 Teletext filter 1 Teletext filter 2 tvp5150: FIFO reset = 0x%02x
 CC   data WSS  data VPS  data VITC data Line mode pdn reset tvp5150a detected.
 tvp5150am1 detected.
 tvp5151 detected.
 tvp5150 include/media/v4l2-subdev.h Disabled Black screen ‰$Ph    ÿ³    h    èüÿÿÿ‹L$ƒÄéD  RSh   ÿ°    h    èüÿÿÿ1ÀƒÄé…  èüÿÿÿUW‰ÇVS¶Ùƒì‰T$¶l$0d¡    ‰D$1À‰,$‰ı9$~Q‹4$)Ş;t$4Ot$41ÿ9÷}‰è¶Òè|  ˆD<GëèD$PVSÿt$h   ÿµ    h    èüÿÿÿ\$PƒÄëª‹D$d+    tèüÿÿÿƒÄ[^_]Ãh0   ÿ°    h    èüÿÿÿƒÄéÔ  ·@ÀPh    ÿ·    h    èüÿÿÿƒÄé˜  hX   ÿµ    èüÿÿÿ1ÉXZéj  …ÀºE   ¸B   DÂPhH   ÿ³    h    èüÿÿÿƒÄéÚ  ƒ»h   ºZ   ¹a   EÑ…Àt2ƒø¹q   ¸~   EÁj VRPh”   ÿ³    h    èüÿÿÿƒÄé¸  ¸x   ë×‹L$$‰D$ƒáQ·T$‰L$R‹Vÿr·WRÿuhğ   ÿ³    h    èüÿÿÿ‹D$,ƒÄ ƒø…Õ  ƒ=     ‹L$½   š  Q‹D$ÁàØ·Ô   R‹€Ğ   ÿp·GP‹Gÿphğ   ÿ³    h    èüÿÿÿƒÄ éš  ‹D$$ƒàP·D$P‹Fÿp·GP‹Gÿphğ   ÿ³    h    èüÿÿÿƒÄ éÕ  PhX   h„   ÿµ    èüÿÿÿ1ÀƒÄé;  hX   ÿµ    èüÿÿÿX1ÀZé;  ‹D$Ph  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰'
  PhD  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰û	  Php  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰Ï	  Phœ  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰£	  PhÈ  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰w	  Phì  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿ‰ÇƒÄ…À‰I	  ‹ƒL  L$º   èüÿÿÿ‰Æ…Àx‹t$‹ƒL  L$º   èüÿÿÿ…Àx‹D$WVPh   ÿ³    h    èüÿÿÿ‹ƒL  º	   L$èüÿÿÿƒÄ…À‰á  Phh  ÿ³    h    èüÿÿÿ‹ƒL  º
   L$èüÿÿÿƒÄ…À‰µ  Ph  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰‰  Ph¼  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰]  PhÜ  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰1  Ph   ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰  Ph4  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿ‰ÆƒÄ…À‰×  ‹ƒL  L$º   èüÿÿÿ…Àx‹D$VPhd  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿ‰ÆƒÄ…À‰Œ  ‹ƒL  L$º   èüÿÿÿ…Àx‹D$VPh˜  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰C  PhÌ  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰  Phì  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰ë  Ph  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰¿  PhD  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿ‰ÆƒÄ…À‰‘  ‹ƒL  L$º   èüÿÿÿ…Àx‹D$VPhp  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰H  Ph°  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰  Phà  ÿ³    h    èüÿÿÿ‹ƒL  º   L$èüÿÿÿƒÄ…À‰ğ  Ph  ÿ³    h    èüÿÿÿ‹ƒL  º(   L$èüÿÿÿƒÄ…À‰Ä  PhH  ÿ³    h    èüÿÿÿ‹ƒL  º-   L$èüÿÿÿ‰ÆƒÄ…À‰–  ‹ƒL  L$º,   èüÿÿÿ…Àx‹D$VPhl  ÿ³    h    èüÿÿÿ‹ƒL  º.   L$èüÿÿÿƒÄ…À‰M  Ph   ÿ³    h    èüÿÿÿ‹ƒL  º/   L$èüÿÿÿƒÄ…À‰!  PhÌ  ÿ³    h    èüÿÿÿ‹ƒL  º0   L$èüÿÿÿƒÄ…À‰õ  ƒàº   )ÂRhø  ÿ³    h    èüÿÿÿº   ‰Øè|  º€   ‰Æ‰Øè|  VPh,  ÿ³    h    èüÿÿÿºƒ   ‰ØƒÄ$è|  º‚   ‰Æ‰Øè|  VPhL  ÿ³    h    èüÿÿÿº…   ‰Øè|  º„   ‰Æ‰Øè|  VPht  ÿ³    h    èüÿÿÿº†   ‰ØƒÄ(è|  Ph   ÿ³    h    èüÿÿÿº‡   ‰Øè|  PhĞ  ÿ³    h    èüÿÿÿºŒ   ‰ØƒÄ è|  º‹   ‰$‰Øè|  ºŠ   ‰Å‰Øè|  º‰   ‰Ç‰Øè|  ºˆ   ‰Æ‰Øè|  ‹$QUWVPh   ÿ³    h    èüÿÿÿ¹±   º    ‰ØƒÄ jhµ   è¤õÿÿ¹¶   º²   ‰Øjhº   èŒõÿÿº»   ‰Øè|  Ph<  ÿ³    h    èüÿÿÿºÀ   ‰ØƒÄ è|  Phh  ÿ³    h    èüÿÿÿºÁ   ‰Øè|  Ph˜  ÿ³    h    èüÿÿÿºÂ   ‰ØƒÄ è|  PhÈ  ÿ³    h    èüÿÿÿºÆ   ‰Øè|  Phô  ÿ³    h    èüÿÿÿºÇ   ‰ØƒÄ è|  Ph  ÿ³    h    èüÿÿÿºÈ   ‰Øè|  Ph@  ÿ³    h    èüÿÿÿºÉ   ‰ØƒÄ è|  PhÄ   ÿ³    h    èüÿÿÿºÊ   ‰Øè|  Phl  ÿ³    h    èüÿÿÿºË   ‰ØƒÄ è|  ºÌ   ‰Æ‰Øè|  VPh˜  ÿ³    h    èüÿÿÿºÍ   ‰Øè|  PhÈ  ÿ³    h    èüÿÿÿºÏ   ‰ØƒÄ$è|  Phğ  ÿ³    h    èüÿÿÿºü   ‰Øè|  Ph	  ÿ³    h    èüÿÿÿ¹   ºâ   ‰ØƒÄ jh“   è–óÿÿ¹”   ºì   ‰Øjh™   è~óÿÿ¹š   ºö   ‰Øjh¦   èfóÿÿ¹§   º   ‰Øj
h¯   èNóÿÿº
  ‰Ø¹Ğ   ƒÄ jhû   è3óÿÿXZ‹D$d+    …ı   ƒÄ1À[^_]Ã‹D$éĞõÿÿ‹D$éüõÿÿ‹D$é(öÿÿ‹D$éTöÿÿ‹D$é€öÿÿ‹|$é®öÿÿ‹D$é÷ÿÿ‹D$éB÷ÿÿ‹D$én÷ÿÿ‹D$éš÷ÿÿ‹D$éÆ÷ÿÿ‹D$éò÷ÿÿ‹t$é øÿÿ‹t$ékøÿÿ‹D$é´øÿÿ‹D$éàøÿÿ‹D$éùÿÿ‹D$é8ùÿÿ‹t$éfùÿÿ‹D$é¯ùÿÿ‹D$éÛùÿÿ‹D$éúÿÿ‹D$é3úÿÿ‹t$éaúÿÿ‹D$éªúÿÿ‹D$éÖúÿÿ‹D$éûÿÿèüÿÿÿ·D$·T$‹$¶L$fÁÀfÁÂf‰†v  ·Àf‰–x  ‹S‰$Â  R·SÒR¶T$RQPhh	  ÿ¶    èüÿÿÿ·†v  ƒÄf=PQ…‰   ·x  ‹–    fù!…š   h  RèüÿÿÿXZ‹È  1À9Ø„Á   iÈŒ   ‹–T  ƒÀ”8  ‰$‹–X  ”<  ‰Ñ‹$‰X  ‰–T  ë¿PhD	  ‹D$ƒÀPèüÿÿÿƒÄéÔ  ‹–    f=QQu$f¾x   uhH  RèüÿÿÿXZésÿÿÿfù tPh”	  ÿ¶    èüÿÿÿƒÄéSÿÿÿh2  Rèüÿÿÿ‹†L  1Éº0   èüÿÿÿY[é/ÿÿÿ…ÀuÇ†T  ÿÿÿ Ç†X      †Ì  1Éº   Ç†\      ‰$‰Ãj Ç†`      Ç†d     Ç†p     èüÿÿÿ¹ 	˜ º  ‰Øj h€   j jj hÿ   j j èüÿÿÿ¹	˜ º  ‰ØƒÄ$j h€   j jj hÿ   j j èüÿÿÿ¹	˜ º  ‰ØƒÄ j h€   j jj hÿ   j j èüÿÿÿ¹	˜ º  ‰ØƒÄ j j j jj jjÿj€èüÿÿÿ¹	Ÿ º  ‰ØƒÄ j hÀü›j jj hÀü›j hÀü›èüÿÿÿº  ‰Ø¹	Ÿ ƒÄ h  j j j jèüÿÿÿ‰^l‹  ƒÄ‰Ú…Ût‹$‰T$èüÿÿÿ‹\$éÔ  ‹†L  L$ºŒ   èüÿÿÿ…Àx‹D$ƒàº@  ƒèƒø
†¨   ‹‡à  ‰–H  1ÒÇ†@      ‰†P  ‰ğÇ†<      Ç†D  Ğ  è,  ‹—à  …Òt&V1É‰èh[  h   h   èüÿÿÿ‰ÂƒÄ…À…Rÿÿÿ‰ğèüÿÿÿ‰Â…ÀˆAÿÿÿƒ=    ~‰ğèÜ  1Ò‰èèüÿÿÿ‰èèüÿÿÿ1Ò‰èèüÿÿÿéÔ  ‹Å    % ù  ƒøÒƒâ`Âà  é9ÿÿÿÿqÿqÿqÿqhh   h¸	  ÿ¶    h    èüÿÿÿƒÄ éB     W  ­  Ø  c  ë	P  c  ë	èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ           °              ÿ                                              @                ÿ     tvp5150_get_vbi tvp5150_set_selection   tvp5150                                                                                                                                         @           ˆ  @  
                                                (   (   ,   0   €   Œ      »   À   Í   Ï   ü   p
                                            à         @             à  À  @   p       @  @                                                  À  0  À         @
          °                        @                                                                          à                                          à              0                      @              `	                                                                                                                                                                                                      0               ªªÿÿ'. +¦r                                                                                                                                                                                        ğ               ª*ÿ?QniŒ	   '    @           [UÅÿ qnB¦Í   :                                                                                                                            ªªÿÿºÎ+¦Ú   ` o GT' ÿÿ                               ` 	€
€ €G     €     ( ./» À Á ÂÈ€É Ê ËNÌ ÍÏ Ğ üÿÿdebug parm=debug:Debug level (0-2) parmtype=debug:int license=GPL v2 author=Mauro Carvalho Chehab description=Texas Instruments TVP5150A/TVP5150AM1/TVP5151 video decoder driver              ¤ÿ      GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                                                                  1   @   &     H   p   F     `   À   ;     u      9     Œ   @  9     ¤   €  A                                	              
 ±   Ğ  Ô     y           Á       %    
 Ö   °       ä   @  V     õ   %   !    
   F   ›    
           (  À  €     A  À  Ô    Q  á       
 o  @  7     ‡  €  ¬     –  ş   $    
 ª  0  g     ¼     W    Ñ  "      
 ë          ÷  @  €       ;  -    
      E     $  p  ã     3  h  Q    
 G  `	  Ş     V  @
  '     h  p
  ”    {          ¹  Ş    
 ¦  0  ˆ    ´  À  X     É  À      Ş  —  9    
 ø  X          à  _       @  È    $  ”       8      X     B    ­     S  À       d  à  6     w  Ğ  X   
      ã      €   ´     ¬  À        ¸         Í         â  (  Å   
 õ                      ş    2  í  .    
 M  h        X    I                  n           ‚      €     ‘      
                   ¥  À   \     ´  €   0     ¿  $       Ö  @  P     î  à  0     ÿ     ,       @  P     #          3  à  @     C           X          q                    ‘  0        ¨  ?        ¾  \   O     Ù             ä             ü                                       +             8             D             V             }                          ¹             Ø             õ                          !             9             B             [             o             †             £             ¹             Ñ             è                                        1             J             Q             `             s             €             —             ¬             ½             Ç             ä             ö             	             -	             H	             Z	             p	             ~	             ’	           	             ­	      
     ¼	  €   0     ß	             ı	              tvp5150.c tvp5150_g_std tvp5150_get_mbus_config tvp5150_enum_mbus_code tvp5150_enum_frame_size tvp5150_volatile_reg tvp5150_runtime_resume tvp5150_runtime_suspend tvp5150_read tvp5150_set_std tvp5150_set_std.cold tvp5150_s_std tvp5150_fill_fmt tvp5150_fill_fmt.cold dump_reg_range tvp5150_close tvp5150_g_sliced_vbi_cap vbi_ram_default tvp5150_g_sliced_vbi_cap.cold tvp5150_subscribe_event tvp5150_remove tvp5150_remove.cold tvp5150_s_raw_fmt tvp5150_s_sliced_fmt tvp5150_s_sliced_fmt.cold tvp5150_isr tvp5150_ev_fmt tvp5150_isr.cold tvp5150_open tvp5150_selmux tvp5150_selmux.cold tvp5150_s_ctrl tvp5150_s_routing tvp5150_registered tvp5150_link_setup tvp5150_link_setup.cold tvp5150_reset tvp5150_init_default tvp5150_g_sliced_fmt tvp5150_g_sliced_fmt.cold __func__.2 tvp5150_g_tuner tvp5150_s_stream tvp5150_init_enable CSWTCH.90 tvp5150_querystd tvp5150_init_cfg tvp5150_log_status tvp5150_log_status.cold tvp5150_probe tvp5150_config tvp5150_ops tvp5150_internal_ops tvp5150_sd_media_ops tvp5150_probe.cold tvp5150_ctrl_ops tvp5150_test_patterns tvp5150_set_selection tvp5150_set_selection.cold __func__.1 tvp5150_get_selection tvp5150_driver_init tvp5150_driver tvp5150_driver_exit tvp5150_pm_ops tvp5150_id tvp5150_readable_table tvp5150_readable_ranges tvp5150_core_ops tvp5150_tuner_ops tvp5150_video_ops tvp5150_vbi_ops tvp5150_pad_ops __UNIQUE_ID_debug283 __UNIQUE_ID_debugtype282 __param_debug __param_str_debug __UNIQUE_ID_license281 __UNIQUE_ID_author280 __UNIQUE_ID_description279 __fentry__ regmap_update_bits_base __stack_chk_guard regmap_read __stack_chk_fail regmap_write _dev_printk __pm_runtime_idle v4l2_src_change_event_subdev_subscribe v4l2_ctrl_subdev_subscribe_event v4l2_fwnode_connector_free media_device_unregister_entity v4l2_async_unregister_subdev v4l2_ctrl_handler_free __pm_runtime_disable __pm_runtime_set_status _dev_err v4l2_subdev_notify_event __pm_runtime_resume media_entity_pads_init media_device_register_entity media_create_pad_link media_entity_remote_pad media_entity_find_link v4l2_ctrl_handler_setup __x86_indirect_thunk_edx devm_gpiod_get_optional gpiod_set_value_cansleep msleep __const_udelay usleep_range_state devm_kmalloc __devm_regmap_init_i2c v4l2_i2c_subdev_init regmap_bulk_read _dev_info v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_new_std_menu_items devm_request_threaded_irq v4l2_async_register_subdev pm_runtime_enable v4l_bound_align_image __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tvp5150_id_device_table v4l2_event_subdev_unsubscribe param_ops_int     Z  !   Z  A   Z  q   Z  Á   Z    Z  1  [  A  Z  q  [    Z  ”  \  ¡  ]  ²  \  ½  ^  Ñ  Z  <    T  _  ±  Z  A  Z  {    ¡  Z  ±  a  Á  Z  Ì    ğ  I    I  4  I  A  Z    Z  ‡    ¼  d  ì  e  ş  f  	  g    h  !  i  1  Z  d  _  y  _  ‘  _  ¡  Z    I    I  -  I  U  _  h  _  “  _  ´  _  Æ  _  á  _    Z    \  9  ]  O  ]  a  \  ”  ]  ¨  _  ¸  _  Æ    á  I  æ  k    [    ^  !  Z  4  l  q  Z  °    Ê  _  Ù  _  	  [  a	  Z  Ñ	  _  ñ	  _  
  _  1
  _  A
  Z  q
  Z  ÷
  m    n  )  o  ›  o  Ü  e    Z  }    ª  p  ¶  q  ß  p  ë  q  Ñ       q  1  Z  >  I  h  _  Œ  _    _  ®  _  Ç  _  İ  _  ê  I  ö  I    I  %  r  P  _  _  _  w  _    _  ¡  _  ¯  _  Á  Z  Ö  \  *  I  ›  ]  Á  \  Ò  ^  á  Z  ô  \    ]  -  \  ;  ^  A  Z  T  \  n  l  ‡  I    _  õ  I  ü  k    a  .  [  >  \  ‰  [  Î  ]  ó  I  ú  I    ^    Z     \  O  \  n  ]  “  ]  «  I  ²  I  ¹  ^  Á  Z  Î  \  ğ  \    ]  ƒ  I  Š  I  Ü  ^  á  Z  ò  \    ]  !  Z  0  \  B  s  _    f  t  ~  u  ˆ  v  ’    ™  t  ´  w  ½  u  Ñ  x  â  y  õ    ü  z    I    {  6  I  G  I    m  ´  |  ß  \  ø  ^    Z  #  \  6    Å  „  ê  \  X  _  p  _  †  _  œ  _  ¼  _  Ù  _  6  ]  ]  I  °  ]  ó  I    ^    Z  #  \  ‡  \  ş  ]  C  I  U  ^  D    ƒ    Ô    T  b  b  c  ˜    å    Ú    ¸    "    '        f    ¨            ¿    Ä    B                   \   I  h   I  ¤     Ü   I                                                 $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     €     „     ˆ     Œ   E               `  (     3     8   `  G   Z  b   \  “     ª     µ     º   `  Î   \  Õ   ^  â     í     ò   `            `  #    .  j  >    C    L    W    \  `  p    u    „    ‰    –    ¡    ¦  `  ³    Ş    é    î  `      ?    J    O  `  {    †    ‹  `  ™  I      ©  j  ¸    Ã  j  Ö    á    æ  `  ú  ]            `  /  ]  @    K    P  `  d  ]  u    €    …  `  ™  ]  ª    µ    º  `  Î  ]  ß    ê    ï  `    ]  $  ]  B  ]  R    ]    b  `  v  ]  ‡    ’    —  `  «  ]  ¼    Ç    Ì  `  à  ]  ñ    ü      `    ]  &    1    6  `  J  ]  [    f    k  `    ]      ›       `  ´  ]  Õ  ]  ä    ï    ô  `    ]  )  ]  8    C    H  `  \  ]  m    x    }  `  ‘  ]  ¢    ­    ²  `  Æ  ]  ×    â    ç  `  û  ]            `  0  ]  Q  ]  `    k    p  `  „  ]  •         ¥  `  ¹  ]  Ê    Õ    Ú  `  î  ]  ÿ    
      `  #  ]  4    ?    D  `  X  ]  y  ]  ˆ    “    ˜  `  ¬  ]  ½    È    Í  `  á  ]  ò    ı    	  `  	  ]  1	    <	    A	  `  M	    [	    b	    m	    r	  `  	    	    –	    ¡	    ¦	  `  ²	    À	    Ç	    Ò	    ×	  `  æ	    ì	    ÷	    ü	  `  
    
    
    
  `  -
    <
    J
    X
    f
    s
    ~
    ƒ
  `  
    ¨
    Â
    È
    Ó
    Ø
  `  ç
    í
    ø
    ı
  `  	              `  .    4    ?    D  `  P    V    a    f  `  u    {    †    ‹  `  —        ¨    ­  `  ¼    Â    Í    Ò  `  Ş    ä    ï    ô  `              #    (  `  4    :    E    J  `  Y    _    j    o  `  {        Œ    ‘  `  ›    ¶    Î    æ    ù      \  $  ^  p    {  }  ¬    ²  }        j  4    :  }  N    Y  }  f    l  }  ~  _  Ş  ~  è  I        I  /    9  I  Y    c  I  }    ‡  I  ª    ¯  I  ¾  I  Ë  €  é  g    ]  [    o    y    ~    ’  ‚  ¢    ¬    µ  i  ¼  ƒ  Å  a  Ñ  I  ú  I  ÿ    
      `  !     B     ú         7    d    ®    ş        W    “    ³    Ì        ò    Ê                                           Z          …     †          ˆ                   $  I      ˜    œ    À  I  Ä  I  Ì  I  Ğ  I  Ü  I  à    ä    è    ğ    ô    ø    ü        ,    0    4    8    @    H    L    X    h    ´    à    ğ          ‹               I     …     Œ        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rodata.str1.1 .rel.text.unlikely .rel.smp_locks .rel__bug_table .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                      @   Y                    	   @       8U  À              )                4                  %   	   @       ø[  8               /             Ô                    8             Ô                    4   	   @       0\                 E      2       d  á	                T      2       E'  •                g             Ú(                   c   	   @       P]  
     
         z             ø:                    v   	   @       Xg                 ‰              ;                    …   	   @       hg                  ™             ;                    •   	   @       ˆg                  ¨             ,;  
                  ¤   	   @       ¨g                 ·             @;                    ³   	   @       ¸g  (              ¿             ^A  «                  Ì             B                    È   	   @       àh                  Ô      0        B                   İ              3B                     í             4B  (                                \B  Ğ     Z         	              ,K  
                                i                                                                                                                                                                                                                                                                                                                            ½–˜2¡gJÊU”z°/UK\S´Õ#H&B-’”‹Æş¾ˆV¯úéXÓ‚¹mõ >Ø@\5	{@ÓJcÅøbİ'1PÁİ`*„ì•ôdÙÜ ÜOGÅjÍşZ…),£UCŠV¡öİ¨’¼W%×§ 5köˆñø#!TĞPÕ¿© kì‰@AlAa­§2<< åLœäÃBc%êip+vj¶BFĞªæC3ß@egß~)ç5Ö÷w5pí44‘é`9ñTé¥|	îjğLUÏóåš!VIòîÑ‚³ÄqÚôr9öAåªÑXHË?féŸ…±:‹åÙÕ…"e@M-NÏjz1{‘&gì©;û¤k.óâ !Ëº.í÷+¶¡p	¼ÑÚ™åİîV]ß,¬ 4‘¬éÂ27Á4Z~öş&ğ ïÙƒR2Ï‡÷‹Ái,gè¹A¢vIÚº‹"	€–ıiç‹µ'èÒmÆú5­ ÇÑ¥ËÇ'xñ|I‰¬Ñ$ßÀÓ7™i}Køã\àQŞFd‰dÂ„&&‹iOG×3ÜvKä%[™©iou«…«fÇäzÀ–x:J’*pĞÁ…‡ˆ„"\"ÖzŒ
ë¼©¾yªQœ_ùÄíÇ™hSîhâÑùîL¿¢éÿáã<maï
Ï@6¡”Ê4®ş²éjX[=Ï}ƒÉÈúVØX^ÓN¿|²ÓÀçæ‘«V·—ènH^×ğ‹»	êYjq>û‰iì¹¹ØÊƒg êF^PËºnüƒ›:²;ùŞı½D¨«Ãø¡õNì¶•}Û Ğq–[¶MÆ‹ãÛ‚ú hÜòäêYa¯
ı`†éøx KJ|O^Ğ¦Mó8ÖƒáT(7áÆÆ„¦(»*‡D¼3
lÆD,ø\]ÅÂøÉd¨ú2b«TG{õ[ÀS•U)VÁ?Z®	¹6_kÎ§ÀÁ¤2HĞC×dš¿÷ÓŠ</î8ã†® •¿Œ$'£ĞåúI¾ Šê®×äØŠ†Ûƒ~n‹áÖ—==/ßD×Mâ]¶ô 
İDÜ5€"XÃ˜>U£Éîâ–Ç%ÓÜ%D¹•ŒÒI}¨üŒWBì™X‚‹/a«5Fü;[Íş_ÿÚÆd“…ñ4ÑøÈŞ2Äúæ® ¡œi Ã/Ñ]ÚõU‹Š¦`…0:¨6 ÚK.Ùé¢ÇáD£ÒKåu h¼j<w	øoáıˆÛ97œÆ×œÒokº(h`\íá°-«ÆÉíaKÈt†[Š7ï_ûÄ?0£†çk8Ó‰æy¥$ıÌµÓP·\Èu´8äÚî…ONÂ©µ‰?ÊM¡µ¬-ï ğ0à6énOæN´"Ü¹ròiÊ@Og÷)Ï8ñë94åGcfÕø/¤Æ*ÚçhÄ¦€F¾º²¹¾zÉ‘«2Fåš¿2bgŠ`Á°–Õ…ÃÓök£˜$¦xáèÙZüµAä‹hÍ2¤#ÁNÍ¸Ã…Â%ØßèÂxÕ`såêlÛzZóiÔi*Ğš@úà¶cgÌÿ‡ïFã(WgµemRt4‰ç>kF©Pa½tî ûT‡çuÉHÁ85nz»ÍaĞÏ‚{z½tô"
Ü+¡ş¹øº±„XƒÛÃªpF[÷àiÙª(‚âg!i›Oühàwiáú°¸sv©hÌ Á/Í`‹È÷jTÈË?,¡Z¼ı@«å¶íÎœÚøF«™ÿ*6I¨ü÷tŠÎÍAnè½™«wk£VX
Ï·õô`)i‰5wñ0æVÍİ²â"WyúáĞ~Yü_!Î¶<CLıí€È#ÂĞœÅ#•¹)Ùœiä´¼gTË·{Ñì m=PsáA<İ¡­áñŒ¤’[®ƒ=ıŠk»İË Œaü#
Cø*Fø#“ØãÄ$qH¹A&öåpm’û p;Ù¤b’wî2´oŒó¶TıĞ²'ßÓ·G–døû·Óµ|‡’{¹ ·ØÏ_­Ñş~Om~-VPtçnıÚ[^idàq²¿ÖÂTÔÙªıÌ~îU¡+a5ì¢1ïŞ®8Ş;çGÈ<İQKhİ¿çG«ÄˆßËÈvÙ;ósr?mò+[sİßR»(»‘weĞìÜ›Cò¸,J^äFg{.¹<#âh(.‚µ=DJ‰h`ŠO>vCÅ“E@hrz(£ig:”_!FFîªkİ
Ôj;€!ÌÓi3v7W6ÃSÆv3n§;´/ 8àö#WñµšÈøİ:y+Ñ’|"ôŞ¿©u‹¯E+' ÍÎ¹bKÌÙî¶pçC–Ê-g¨å·ĞäÒ¼¼ùà På.ÊğÑ–àx3…¸•bŸî–Òtàw’‡‚œ›XB7[Y‘í³äGò§zY…:'ŒNğk']-ğéW 1äÇµMÛ¤…ukÂ5 ò–<yÌÌ}¼•*ü>@'7(´¢ÌÏöŞˆ(Yƒ_ÒJz:¼aü™ˆrEldÈ!P¤0è³¤vƒj S™nhR^Ó»ù  .Q Px
òfœù‡’®<tZÍ|-ÜÜ¿õÂÚ_é‚ƒÃÖÑJ‹á¢<QÖóª{JÃÖë<‹ÍÿÌjâXÆ+sg‹ê†“sí,ë•Ïç›-Cn+„fØ:ŞÓvE2Ú95KŒ…B¾‘#İVày˜æøÕ¸$lï0ÃÛ "ÙìA…¾ø?D1Á‰…ô~Ò>ePÌÀ]Ô§9ş°%$xVãƒ[¿ÔÂ±êÂ²óMì¿·Šúz«“?vÁ1“åCLCÿå}«–qğı0úğiÕaplÂ±¾Vuêçí-™6Ğ@,+ñeÉ®9¿/ÕŠD9ŸZ¤b’Tş#î_×š·ÇÀ¸ıÈWÂQvä³å‰q‚€õW¸jXzH¢g	äw_ázAü¾İ„»ÊôCùÅy?¾'›ápş¤NÓe2%İ®X©¾ãÅ]@ASà²®¦‰Cû}ÖšNiÏ³í·ëGÓ°SÖ­ÈÂDå;§ïDÙ[Ø:55x•cqT|<Eíªvé9 }xgG{JÕÏ“¯)±Fˆ¤ÄzÇ½Û¬ÂXè ãÅ`.¢¤×ÚÍæ¾™#¶Ù­éÁ@'êÕ²R ¨è£bİâ=}#fªãYØMäÙÎñø½|IĞ{e šõL9ö÷j· Ÿ*	7`U*’mˆvˆàÔsSUÃñãŠØ#Â|ø¶oøÔHy1Ë3ƒ¯k¢o’ RA™a$ßÈÌn
·òê”¼Õˆ¼?jhñìÀ»Ò›ka–P½—ìÿ™£™¸>˜íS?!Ì£Ãøkû?moOcBÈxL„½dİÜÒäßD2·¯€àÜ«Î‡s}…º±şüP‡Q…CrÜ)Ç&âûEiú"œ“¤è¤.	9Á<ĞK¬¨[ìıíÿ,ƒ+ã/üHFYT«{ú“€§TÒèO§ÈŸÈ]§ÊÉ5Dj³EŸé¨ƒ)1¸ßTÍBm4˜xqñWácÉa‰£~OIUğLtÑ1[Şk¾¦ÁWéàz“jÚòÆÃŠDè{˜Ñ0İ×©h ä¸J«T“tŒ¾X.	’KNOÈläÄi²0÷)êm¸ÀÀËãµ{,Gk…_(” G¤ÓÅ›Ş†ìŠÏÄ÷^”¡9]øÙÑÖu\]é†¢¨°*j3Ñm›c¬~<Zñ4±]/Lú¶üĞÚ0= WÍO‰OY½‡?~—,ª–Bgw]Rå¬ûÒÍX\-°álp1’çş#§'ì¢¨‰ág¿	÷? #ş<‡¿kÆƒş‡Ë‡Õ\ò{ÿ4yß®w)ÕŠ(ÔzÍuîfp†¿×pæøô’ø°öª8œ¼Ò×YG˜òóÊ…FñuˆÓğÜÍUÆÑ€Ücnä•W€í èh€:Ôâ¬5…Ä	TâOD0© /Ó°:iô2ª¶S2BŸ½&šVÃT!PÅá½ÀG¥tÁÊµÆ–AÂ7‹c±Ü4Ã^ Ì°KõÄã€^Öˆ“¤8.ùÄrzê‘Ğ¦ëL;f°èq•ÁyÛ]hW ß) ©H‘À»>»q`Ç]Æµbµ@UnÊ-ÍÅ?rµâC}Pq}è³)Fš¬©`Lêv©ØèşáifSÚ§NŠ'¾ƒµÆrHh1ì
8EŒUMhìhh¯ÛœhI®ÕIáâZhëû9£H_ÎöSçæ ¥Ä¿CÄbeÜŠN|%ĞYJ™ ñ=RÅ>¤„nq»æšÅ#–A˜Ê‘%ÄàÍyy:T²ÕßÙ5Ê·¢•:äøÒBä†RéÊ”ïŞŠ¡?FÌÂCÀ›:O™€é,ƒëÄR¼Y¢ ›Î€EĞºU.ß±ÍAŸäJY6ÚuUÜxácèÛ³r)!’Zwq‹ãœ0‚DçO²8ëĞVô½ª‘şîOØø°4(	šeácŒ¬5¯	¦ŒãL©ÂVŠ`‰ìh7Ë@KB}Û§¬pøA?ããF}ú?¿aJªqşG\ü]Úxl3ËŠƒœ|GiQQc C
˜¦R¨¿¥ùê{ã¤9hì	­	§«î0KV›¦î×YËzüõM®¦\‡;MB8Háóº¾Q[fëô'ø]üé=ös“Ãô‹GÄ¼põõ{(¹Õ­,ñb¨•¡¬ŒHş K8ëèÍî>.â¬;şìL/¢¨6†€XÄG…œ…ûçş¡NaSrÑ¬¿°ò®Os_ #¬4ƒ:uïy}êùğ³ğ5?0¦pDä¼GHV;‰qHÊÖ‰¦EºOu™…†ÙÀfìså‚Ê7GæeÇFĞ|¢{Ø}èŠUt5&æ±¡:zCÏß÷À5ÒZÈ^úÈ*ÙF/N>Í5tl³á$èö!ö#H.÷*nº1§àF B6ã7Ñ4Ç<ğ.ß±:+Ğ¾FLˆ›7@–ÉfßÒì<§È¹ã	?©í­)ñÏº¹¼C)uÓ0f–‘ÁN¢UTÎQ"ÿÌI…;:
©–ÍGİı÷1¤Ö	¶ êå@çWËxÑºtjë\ùìÉ?4M›v÷?<í«ÊcZƒ<C†ò«qì¼è%ô~7´à!/Ö)¯Rf¼@îPæù——H¨{¬ù©b±ÏS	ããùàŞcçØ«2áõÛß  b^zw´^©eŞ)p·İHFª*”*·ˆáÎ™|}Ë4=¼bĞÅgî*[‹kwŞËg®À¸Ör$=Äd;R0Z®ìûâêÃ9²¥BÂñ¥ŒÒŞWj³JÓKŸ?á8ï`kwLt*v!IÜfCœƒùï¥)…ÂGh›?ásd6[ä.ó&Î”£ĞG–‡@xŒÕØä˜¸gä€;¤™éÙñáp¤Ôõ¤J‘˜wAPÇ8şæ?€Ç¿A­ºí›…ÉÑl—*ù1ˆÌHk¦Ùo¨JñºI:²8†Š&ïş M›Â-¶7şcë éøÓºˆzüŠ¿J0%ÄX¯a­¨7TSC4·É*pµ4˜:æÄ¾ÔÒ]©‘3p7…áqS;ÇÒ)U®N9‰â*2cøI„FÚ–F³ì¦¥¢­/“±¤ÚŸ‘÷VşÙÜâı÷ğkLZ=›”aNcÅ…<1É Ëohr,í„·YÃjÚt×_¢R¸IÎ¥É»Aœ¾ıºÆátLmÎÓk$J™ƒGï+ŒÜ¬Ü¤ªa_ŸáS¬plô¡qdĞöŞbGd4bœÑÇ(:]oü&>ø”íF” hâıŞÄ)°j ÌMô{ğ{úÅË»R+ÔÊ]ç@lè™‰ÆÕcíJ–À#úwsË™À[o˜ 4EÏúøúY3±¯aŞ&pKù8
\İ.·øíıO°ö/À:L.§©µ<¼—üš-v6mÍä{°WÜœü§‹Kd$Î8/f3$|¹âDÉ§Ò§‚­Dl˜sÆZG4œò<–EU`É»Ïb)"L$¿aæ[I_
$T."ì«QeZtÍíÈ°×$¯~vÎ»üèSˆÆ‰›lªZ½è&X~„Øi¸‹›‡Œöì	Ï.[Ú†)Ù\½£v·õñÑ±ÎH¸%×|7ó£`Æ¸#Á3¯îúÌ^"ÍÒ}UÉÿz^K›Šb)ÍêÓf•­KÅ.ª†è&3/TÈR[¾‘ØBÚFs°CD'^WÚçH.¹ó¨ĞûPöÆÎ1üe7\h÷<Û­gÜôìOˆ2A ì…æ.A2{hØœĞd}§>DnÔÂùÎ;tÍê´mÀÁ†‡pBm™Ù†&#´JŒpûhè§ë‹ÉtÄS¼£\y?R¸{(ù&sUzzìS’?1ì—Ùx†ÿ¼­ã¤²Š?cßa#v×ä•y ,†§\“vìòZŠÑ9×«®+²Ò©j‚öúç¹[N?x~îµüSøĞˆ¼iğÃ)*ÿÚ"zÁñëJ3
º²„Ş„k²Z°şZû_‚»&›UY\ç²4¯Î*Ì*ôòA’Û-°æ|¿…>R—"¼×)ÿ­³?±[ìA}¦
íE»â/ì8¡Î=¬à¿’ m”ÿàîkC™òyûHıA<h‹ï,#®jYgW¯Ù´YÍ:Î´™Œ
O¤w¢8R
#X…Që½¥#ŞâÎŞXåK¤‡YÛœíùôq—ÓĞóEe‚qËøÆ‹¸ğKÆMüP cFdó>DÄ‡:h„¤Æ!ËDÓn¼u	}÷D(t›5"eÌènMHù *FaX=¹)QÎYå‡Áù÷áyY¸÷J2Ø#GĞåÖ&ğ7š!ôÜ#+Wªá\I¹º?rv“ƒ©»O·_üÚü­éªŞL'j²—È,ßÌßëI{ËX'V2k¶z€òw‘>;Ë8«ªÁŒyÒ°ı5ä™Çûc)4?$À6´äÜÉQ.&ÅÆ Y/É·'È£„õÚu¹ô™š˜©1Y‘û-ó‘|Wç°jTd`vÍ×_UÒ’ª˜>)ZN#ÎwÍ%˜'I`äcR.9Î×&”9ôz	5NV‡HAŠùEëû÷o³£— “¾òvÛ»
j¡Y½ÇÚØo·È’6s{®ì<bÍ‰Ç–d6-Ø\.ZKHxhN¬“_:üTq’~Ğ2™{·o®l9øqD–VáùZı1£ùˆˆğ\È«&y6KUi1ÅFkzk+Oö?Iäáıx–·E‚J·5s¿f’xô‰Öï7º=ÈÓŠPZHñªSì	‹c…ñaoÄ\5!ß»Õm2 [=WU³PÌZŠ#lâöáte­—æ;°#Y‚Ç±Î&íª<–.xš¼E«ê}º—=bš"Ÿ¢ÄÏ>vT	#’´ÂEŞ§Îg dÙUÒ ŠK4YoĞÊ<Æ€ê’ßŠÕ—>ëÀ¤Øoã¬ex"ØXÒm ]·®(ÇsÈ1(ró÷Å—rº\æØ#¶‚Ï(‚}tş‚GÃ‚Ú#ÿÑf÷k`ï¿ÈEVi<Ûr¬aß9\[Š–éíªCVáLãÚ‹S! ¥ÍÚc§§Ú+ èìÖõ•#4€=ÆoÍö7ŒYĞ¹Ók»á¥ŒÉ‹/(µè*ïi0ùñ–s(¸tßËx(¥gŠÓ:2HèSk–¬È(^–ÿ¦ÄWbKı.õæ€pœ.â†Ö5&\w`V‰&´ü7™ÌÔŸlÑm®ˆøçåëÏKš»m:­$Îp(9>Ù¸§{Ë€¥\ûRm¼"ÀìkˆÊş@‡kµªuáòró«l‚’¨ái7Şr“£uÕP2Ş#góéBÏÏp¥A\^(xQyZ‰ğ#»™v¡”R#êÓŸ‹&C;­_*ÔİÕ™ğ+¢!bf+u¬úİ ßl'\D™ón
:‚âö>‘ü†ñ½¤O½ZÆ]p‡ïûV§baóÏ?è<ì´Ïœƒü
©·4ù}às<‚[[ªIğÎ[^j©‹Úæ “Ø¸b½+jà…„E*gAœõ·W–ZŠ-¨˜¦ÇãĞÉ¸kşŠœc]v‡ç¾6Æö²rwµ*}$ZzO…õŠ@‚qv-£‡î~ÈÆÎ½‰Ç†ĞæhŠûÂdpı4ÿ'¶î€*Ç}öoŒÅa‘Án&N?şhÃM_ºüŞÊØzÆ•ŸÌğÿ‚M,2Ö¾^a@²ĞÑ{E¡®Á(ú•F¯\ã(EuŞ†ªÁc‰É³ñğöƒûo`êRPËgœø¹IAá™Ù]ÛŸÙ©«É•.$„‰xñ¢ìU_‚Ê$T5ztˆa9:…N¦I:Ü.xˆÚ¼`å§>«½¨~¨ÎõøyÆ+Ø/á@¯¨a£ê¢:°³!.ûC¬	ìPÕƒ¤‡®cC=ÈÏ„Å±8ö´Ÿ5ö»¼Û³½Êrd³&Q'¾8nz#M…Áòæ¹TV+L³Œ¿óUÃxnÙXo%:Ãóµ£¤W–:d±x
 'æÎù<%fë"‘.ëz^JCWTÆzßñÆ½ªˆûføøÖöçVéÌUi£+±Ü1pÅ•i^Ô4¥É: :D´[rÚ¢ô¯qèêÖ|áé9ò5áıÒtc2Uí–{k+ü¿eì­ƒA´®$š·=ëK7h—Dé©uI–¥"ê+n^õ–RZ§>ok[uŞ"R±ä†Õz[µp°ò…†àébÿı©e ìíà1^Yç”ıAZÎæ¼Yì RøÏ"IY'Ø)Æõ¹1Ã(Ï×©åT­ÿ©îËË|<C@µ@Â84ˆWè²¹!bixW“!û6Ê—¢“YV´í#pÁeRˆUK`Ã8*H<Q ­÷2
¹ò4(’ í ğ°Å»©ê-·±šz¨PP€a²f2§Y©9!pWT{ò^•0ÂÙ#yõ×î¥~¾gL’pL—’f›Úóí@%bŒl‰æÖK5æ€sšvÕ€²0œ˜(ùçİRGQDı´]<ç;Ò¦ÉûÆè{:rƒ£%Tjš»q¬Ş³5ŸªäıUGä9zdnAKÛP£Ü4Dº›,óCÆ‘9†Äjdúã·œÈB÷:e-Vš¼¹ŒØêAÿ¯g»½fäÖş*Z_»Û	xjÅÍïr-/Yÿ[IÎİ$ñ ›8éÓÇ
“÷y>ÒwÑÇq¢>41U<ÖÖôTÈ2amN“2ù…C0w_>z¬ó²‘p]’9¸	û²õæ{Y@á£j©‰h@Ü#›	åºm%µ°po¨ß¤GUš
&±Ä.CÃõBBb›ö^\ã/´t4'üv’3
óÏF2ÕªtÄ¹Œoøt[Vz=´T|Í«‡ì.Àw”Áµ'ÙˆOµdîK%ÏîG=n#Hv ZˆãrôIz¡øpŒò<” "Ş(ñ4¬š™÷2Îğã¤P\4k$›â÷«Ã»âD|Ô23öÇ`LĞ™ŠhæZÔ=$¯Ì˜ôßéEVWÑ5OnÀC>5İÄ!¼dÀê¯ÍèğbÒ’+¤ğ8Tåé}¯Õšp¾°j0ğ „DM'¬±Åe†VÀ­^(ZÉğ‚ü*»~I£uŸ™+³Vy1}Hsßµ7Qºû›Ïú3¿Â“B‚¢‚›ú®/Ñ$ähT –èÏà¨I/sëŠ4IxˆŞg¼>KcrGüƒ´ò;°fëa\|FğÃÿ<š?îğjà·”ñ z#óH«2[á*Å±;¿¥N§h«¡z%VİL] iØfgÁ¯›¥»!¸Òt¬nCüü¶÷™sQ¾qèlÕá÷ ?ÂÌóqÈ’ïıèüX«$ƒ)iâP8.{$w
O-× ìş;JÊ4&µ6!ÁêÒ£l§è%4ûæhŠååÀ±²A-P¨ß@âK(	dyÑgPºT÷%X-^šZóDåß\:8(ÄI«Ê%D®ºÇPµ/,Æ5Í¿·±®àg80°ÃÎ`xª»è? ×˜cEF+åDÆhßâw¼RQ“ßs&	–Pm£Iìşîcì;é©
”ò  “#NĞ°ğÏ1÷İR–÷e,%4=‹°ÇÕv+…\£i{€Úè¥êßWËõ=a‹ôfÄ?¡Tß™v®Sg®‰‘ƒ˜kåì›Ìá¦o-¸ÏzŠªBÓK^ÛWËrr/`Àå‘öŸ rcœ‡S×:§nIüö´6Uˆ¤"¼ÍC‘ºg/p2Í72|„,ôÚÉ–r!ë¹0şlÆúpy`6ËB>øÙTe×BıôÂ]FöÑ‹pCñ¡©ò]1’‡ç¹;ƒà€%˜H?m(,¿R‚ÕÒ˜ßtw•ïa*0ŒKÜÅ$U,ëT³Ï¡úgæ°—ÂXA:©Fy,œÇJ³aGˆú—¾°ùÈäÀ²ª>Q©;PŞL¸ÌÚA¼”ú›€ø^³ú8×½±lµœT|²rşÛ”\´6¹†s<ğÒr…q„"³†“opNşb™Øâ~¤uår1­»adøu%¬!v›ˆ‰/4dUùcÄK²0økÈ$C–¥“OF­øeÒ|.‡W¶Ä2EYƒÃĞ
 }õ=›Ygä¿ ´ôÁ:Ÿ»ƒæ-Ø™bÇÉLûÂ:Î¡ùZvyS,~8.µëÁ7_íŠópƒb—îšº‡ÇÁKË¿Ñ›2L²¢ô" )±±rÈdúê‡0@Ã ØmY0§ä_U?MİÒ .™lœu:pl!‚Ë¢¬æ<+ºwdŠø±A,Ü¸k¢*¦ĞêĞ£˜À§¶|—§ÉÈ_º›ŒSØ—ÕÓ÷±Iê¿	Li·¯–êÆ+|Ô E…wÇ¢²<òõ´ì^‰}m÷Ø,ŒGö°™L¼Cš©ì;Râ¦FŒØÍiN#¹P£‹Ü«áØMéÂ|¨Îo3Lı& œéüÊ¹DáÛYÚf%&`QÅ¤,âP'[Ş–/8Qş«’¨W[ªr&M5}2Ò6ËpìeÒ–û óºşömÛ"ŞÇĞ£:õihT—Ë­_5³AŞ‡¥Á#aŒ‹>ô‰¤¬àrùR_L	ôoÿŒßüYåÍÈD²P®¤†î<vâMuÏyúaßïd°üêñĞ)%h³Ì±¹}v5Ö7)ÿæß)@ÊSx¢P…U|Ö¦ã;š÷YŸ–V˜dY|µY‚IÊ”=Ïk;¥ŸŞx,²?6~æ^bul õÛhQKTç{«%„~ï€v”¶O†ÄpÙf:-ış„2rW>YöºıFÔ†”Œ« A¦Œ~×’?tSR•†hîÙ»šU…Uí£fQ˜\7ôPWæÏKk6t\*¹r’Á¸ú¿<.®¹Ç!wnïáª£M>Z‚Ğ¦@°t,XÇf[ƒCa§ƒÃ¹ì/Nì š åıKÕª×¤niÜAA…@sòÊçÖû)]7·ˆİAr±„yáXN‘t`7š^ì»¤İ»gâ A U(Ä‹Ä¹äÖÛ+VjÓ¡ñ¢9lªâ“ºÓ„±VæaD3;Uœ;Ø²GÄ¶Gw2şØ
”¡‡GõÌÜÖVƒ¼P+æèİlÈrlúØKh]ĞÕDØNg|WReïv%T³®S*’£²vºÑNt.57&#^°L<N  7$Ùóİ»D8å/cO¡ÔÏüdò²ËP7µq›¦e®Œ%ÓÒT¸¿„²ŸĞªˆ#à­¨ë† ›ãKÛe$Jshv¤$ì+Í_QÏ
>©»ÇËù¶æ&úã+7\ÔfK´ì3F\VZ¸­VĞğàA²¦2k<lŞÒŒ-òçœ§R^ıü¾×‹²Àj"°àˆl‰½Ÿ¡µïø’P%OJÇÚRúoA¬ÒÍº|j*^tkÕ.z¨¦oBÑÊ6¼¿¹¹/¿?³gpõ8ö^n)³ğ~]?f¬½=4»†8]ÕP,S#X,^€´¾­Ó˜%¸/¨ñ^ØÒ§è9Á+Í€|@¼(VGŞ^k>sôŸyâ ç)‚$–Îa¥D4Úl|úqî6µçW[Š .OT5ÊÔ\±ùU{¥î¤W@İÕBZë£ˆŸdú_lñ„ªªù'šaÎó0ã~ÏûŠ}¶›cš=¼uPúéíK÷û6Á*¨yƒês¼‰ã	ä?˜YèQˆÄÌ‹ë¢şLñ=6s}è‡š¸ÙdÀQKÉI¢C¨XÿoÄîñ6ót)&õÜ``¾ÏOwx€{–Z~Í
úSVÍNj*²{53”¾½RâO/yõeÒÀ¯Ë“8&qîI+ÜÆ"×.ÈşUõmÆì±˜ƒåX$© S
5ï‹yºœnG‚æ§–J–ı	ÖC£¿à…\|@Í#z9Õš7W½4‘;Ñí}ºÂhñt’Ù”NE,uTñ…Ïd+¿|?±ù
LÁ^=&‚»Í :©-¿$½|'¤®qÙ¬¾^¿ç€¿àåœœóóeaf»¾¸UD½øÉ‹ÛÂæúŞ ÁùóÛ¼¦RÅw”ê} yğN\¸(9HeÀ·$KïĞXSÃ\0/ŒK•è•½.€=ÄvQĞXİ·.ttAM×Y‡„õ©&rzõ&hó^8™ªO?ì(VR£LõB4£B\Ùñs¥{31Ú€U¬µÌÅ
óŞŸgS(ß9›Ÿİ1Èwî&T?Ûï²œP)h=eÊ—ÏaAf9(*D·uÃ9ÈæNñ@ˆëbŞµ‚ÆıÂœşº. ë3ìİDãëÁwoy6}8‡/§7BÎB~gÿÅ¼ÙÁ%k—H(…³hùWÿO—T>2¿i´ õô«o¶:€Ş¸ƒ¸i–É"-ó˜2¥wV_Vd)¡5tjø$ÆÚÜÈÎ»ÔJ˜ÇcÀq{Yæ´¶ÿr«f\Ï'Šá¢X Ï¼•Hò&d€`bG”ÄUÇ'Ù/­(;$Ÿ(Ø;Ei’H<×†ÆÏfÏ«;ÊPüÏ
e¼­„¢n£NÈ•önJĞÑÙA{¬êœ”ÄÄ[J’ÆêËeg.3X•+Ã|‰IkÔD…^Çëy…Ôëq!Õ©#.?iAûEÂñœW2)…¸s»¸<¥=V¥Êg6º¢Ä¼	HùepòF$Yú@½¯j2zíb®¼Ïª!ˆª‰wŒ;Æ{”_iTkß„wºô½ºìù]J;%FÓ•ïú‹¸MáÀ¤p¦KÅYx+ó6ƒ|5
d‹6UX#Ç‹QİS»Lç-?®›Ó:ÒåĞ”²L®—0 •AwnËQ8?ŸíĞ±Ê=PHÙÆÆÅ•èü5ÄêHık£C‡2éCÄ˜Œ-ÌıØ§á$?¯ø˜3²Î‚d_‚lº…V¡³%ÆğØ&ViUÇ{_ÿ,P¤$fR0¬n”QPÒñ'NôµY¢9_ ÷8äˆ¶¥öPãöî%Îÿãã@Û¢ŠÛË
B%ßıˆş'Ÿ%ÉŸ”…êQpÙÆïJ´.x– ‹Q”‚Î-ßBrkãõüÌpŞX§ik¨¸¹oAx¼Em:³æX°Tu…#×9o²Æ¸µ ®ÎáXYâQá´ Úch¯ÓdŒ»MíÄ.]áÿî‹ó€ƒªs÷,WMd…²ƒWªæ`İÆLoH|A6Rno†ˆ`Êz“wéG`óê#N'òœxªÀÖ›{k¡£Eİ¸”gŞQ¤›İ¦‘õïYÒd)Ó…š³f’
¬^ da‘—Û„wQƒ§Ä“Óm?*­¡:Hg¯b9š|"æP{¦ôÛ0²Ïö•8ïgr‘Ñìwş|1Òêk·ïZe’läGpk«é*ÊtR`à	Z5ÅyÖE	ÊÛ‡–3÷ÊykƒwáZÜÂ<^{^€ì¬ßG$J‹…[ÏÕUÖI«Í2â\g)¥Óô€È†Ã—¹û+/r9)´ÑR À˜õ3pÍÁ{à_ÅÖGe,ÎXHMYªŒg”ş‚ŒU“Ôû‚!ÜŒ-I¼…şèeÈH a\vmãÀ #×øsHax(pş®Kä_Üë´wwÀT“ÉUVe&¥i°Í[wsæãl›*¸&ÒÖ¼²;!aªr·ÁÛÆı5°ÜN´ç¤.´ˆ¤İ»¼Á™H©&¿¼‰ÕŞşyÕüÃÔa•E¾XîAÆïù•½Eß¼Ì
l5â"Ö8é¤Ä‰øi$èı·IC`zŸ:§•²„+?µM"ŞS*ª"ÃXz´2 ÁOÃ^ì·=
–îğ?ÈDû¿IAaíÆ£ƒ¶»?V!§Œ
z€ÑëbAAswµ‚S%	vÄN¦•€Ip%(y€Êƒ‚`ùÑ€8%êÒÃ‡'d¨AaIóXL>f-#’)Ç~ıeUİõãZûıBZÙi¶Weú‡ˆ·ŸØa©zÕ ¥×ØSh3 ÄpHzÑŒ¿-;û½©¨}İyKï&Œ£W‡Çòéf°_¼÷^¯cĞ™ªOŠìdwüè* Ûú+d²†µYÑ+F€¦èÏ!Òı2êÖÈ‚êÚçæWÉ6 ›´Šˆ¯Pmëf¹¡X­ûwÚƒ”ñB–í
/ü£ÉÓ‹©"Øæ’~J‡kØ¶¹»u¡…'rI©èÍşß:m÷†X‹)Xüb³O3É7²–Ê³38ş	ÎÍÜ>)ãİ5Yb èæ««Ù$»ŠÁÍôƒññMEƒëÌı)¹rWä(˜¥pÓûmd´èä}æ¨‡Ç¹0Ğ÷wt~N¥ÜĞ2•ï­pçiQYÉ;³JÁöŠàaÕFı‰Ê‡U”­h%h¯ ½O_¤=©¡v¤æ½,ö	Xû•ˆ1œ0úN2Ñ¨h‚jQï1­á›C’¦¦t9$df¯WSmã:Â\6±ÂĞ¼É¾ÙrUŠ%*dÅy”E[ŠùË¬´ıyâhÊ¶”½iæ–®¹Ÿ&h•Š¡§0“üòzkâ]OıRa¡R™8óô/Ö‡ÁnNÔÂb Ìœ ñ9
Â£™ûtµşû8›Å	Üà‹êõqÕÄ©Ê‡ÃÜè-ÛgZ?ˆß„Ã¯ú’¾¢FĞOğø9²\T¨VôfXÏ‡0Ó{š3{
vßScÎØ¢_SÁ€½SŞRoL>•Ë¬‚°¹ tê|©»“¥5÷£²óG"S<êléËĞ'H/0·ÃÜvö*J¥#‚Tø†ı7ŒŠqfŸÓXa…„ ¹}WåİÌz˜ıøÑd4Ãì4@ä0;Ğµ_”±™ àØÍ4e^AµŸ^“Órˆ±
soìAñÙëÙü–wŠÓœKñËÈÇi¥6E¶lzÏ*1šrÔZµ—ôyœÁkÀ?«‡áĞ“Ëiˆ,—PÁe=…Ñ|&¨o>Í†éŠf¾~»ÓRbÆÙÒ~o*ñÍ¬d³®ä³& RÀB5éQ÷r)^ƒW§-ƒn2ì1DGÉÛªãü¦:TÅÕ‡µÒ†LLŞè”qSØVùª¨"hï|W£C¯ Šy ¼
¬'w‚KéAéiÀM`·ÅüPn@"A-B½ïHêGÙ2fÍù±*|’È`I~[£Í,>½{\Ç;R°›¸Áz,ğs«(£èo;P£²z#© 3ãüè v_M¤½É'ş'S¸Ôå´m(çC†1r-¸·]k"S´•&¢[ØÕŞ„Ši[¯^ZÑAÅq®”¿¥?^#-õuÃUñõü„©ß¨„=bÓ\ñëûè;ZÕµ6gÂ%9‘CC”Æò‡·†FríM•!sÁ5¡Šø?›EéùkÄ
­(b%«Uõ´æ‡SPO^ïïÅ®F>ŞÓ£fıo+	FCÍ8‡Y¬©HîËFUª6ÃÂ ‹Û2ì¤ág½ae¡*‚‡BädÂì‰;È0&ÁŠŸ #ûßŸùŒ‹÷"ìœüwtp5o÷Öš‹Ğ]á›Xœ{ñ~¼/Wg9ı·–HH==¯¦³• ÒKWÊÒbŒ1©ğ 6y"X…8„c²”ïçY°E˜™å3™Â]‰NÎ?j$å@7+sdaFy.¶£“â#Y\ÄòJ<oëÍŒD·=
*2_Ø‘ËƒyÖl ”¢Eóıd rÆãf‡§it0¨5s/H­ŞûHC5jŞtêjİ —¯çöÕ•Sùá½Û‡¿mäÀ["½A¥ÙÒ†wñç²PT$«ä|ğuöÑ‹B->®›Ìc6ÕÏ°q%ßU¼N„ïàŠˆ>ÕìŒn>ŠFRÊ;nš{7 ÂuÎ‹¹AŸ©çĞ­«ÎîšÍ	+ôK¶ˆ{ùao,ğÒ(`µçqçÃ™Ü)˜}¿{ùX†ïº›3IZ?¨İä4&Õ5ª¹EÎQƒtÂŞ“Z› îJ«”î.ñ†z<KÁ×"éFD¼ğ»„ÔÑUrÅ„^=?A.´¡Å®È+»5’¸~&º¹eËàôÊ Ã–D›còô¸¥ŒE ´ÿ|.Y¿€J“C4öÀİ†WüGN#ŒgÏÁÿËÎO%Ûs¦ÃµÍy¾Q5¶%•Q¡_zI*qÒ*wDĞòÑzz¬8­í2â7KÓÏ"ëøêŞ3àå¹$ŠGØ%ü]nß[ÂÊwŞ 	!aÆÖ¶‹’…aÿkÆ}¨:Øí/Ê•ùù&æYÄ;åòˆáo³üN•Â³Bfä>™PWÕZ¶Jˆå’Çu4å'T;¤¹ğh‰¤¬ó`e­ïá,Ë¥Å(Be¹H’°påCRÆßµ56` +æówµş‘Ëı³ŞšKğ¸™€¹*£$‘è©Nâ¶·2da_TÑ¹‡Qè9,)ôıã¥U æo!cìÉB_®ëùƒdFŠl3Ì©eå‰ MÒ>+k$púƒˆW•éºyjîsÚ©{s§ëçe¯¾ş0Ÿ±Pd="e°ãÑ¬„ÁB­.H7dëĞ9Ì££Æ96ª¡²Jõ§O‚Ğ¨(¹.µX#ÀıGNÉû˜‘ÏEI^Ó¶	§%`yÿ–‹ÅƒZØì1óh…Üğtém•à 6\ˆc©á€¥Zş·Ô"ãWÊô…ˆôWM?M–IÁçıâOğ†P<^Í´;Ö	ğğßÃTÊP62xœi¿"5#’( $ÔÎ‰^t2¥Ä=PvcÌ˜™Å¯J6QLäOd/ó-r´\A°h†>G¹»‰pE ñ7u{÷Äı‘yø¿Rx¹ôU÷ÓEé©ëîó®¥™‡Y+`AîvoW½ñHÜÁhÎ{R·|Ÿ¼vı‰‚{g¢oI‰ÿÆKéTDíÁF‹OGÆ>}[èã„›ïÙ–0›Š–ˆ{m¥]î>vş€¿FiÛ+a‰^œ€l¸ÑÕB Òj—wÅĞİÛ\3D•QÃ¹5t±këQEÀ’÷Ù~v~I6QÜ«8Ùï…=÷ëj6\»Ó=5µÙ³¨Ã6ûvçÜ“ÀG¡^ˆº~cg»÷èpËáo<¡ÛùèfˆTM< ìYã†)ÿwRılã¨ŒvõÌÃàœ7ıàìI&±Îp }–f>éBÃiFËÙD¸ “Âë¸!b(Øù	Ÿš[L‘îVáÜØ~"ÆMĞWX•3àg6.æ±0õÌqã}¶w‘ykq2¾hñƒ$*ÀŸ¢vîé›ìbÏšî*‡ùjEœíı×³„ªØ›S‚¶û‘
ÃÿçÌ#˜Qqlú„ˆX	[Ù+Ï'˜bm‰qDoM[«T?/|kóàB,.4,ó°–‡7eı›/nïóçÁ†Ä˜è.^>MÕLdXâôyYFGº Õ6èã×ş!|tõ$“¨0?á¾–NQn¸¿i"23±k2r­Lãï|ªÕƒÎUÇ:pM5ê›`Ú³‹ÉÏÍdí?[í!ZyOÆöwÈøk,Rå™×’‡ÇXDı¥Nƒè™U,5¤5*ÚçÒ_…¬W¢pÖy•OÍo|7í>tz_«å˜¦º$ßïWª¤T
÷rKQ-V_†ñŸ–Á•ä»i… K(3ş]È'øúieĞTq§1‘JO8üÿ…w‰6¸—`¸n…áÈá©Ã¯q"rğA¿wóÅi,¬İJ5³wÈnóf®S­Ï0ÕVO¸z·€×ÕØx¬ó uå²1^ÒáµİS“+•JÎaÜKDÑcÄ{µã]ÖÈ‚1»¨æ€æ®¶èÍ {¼*³Ê,hşÔÁév³Ø…Ë²åtT  •ÕØ%êS0¨Æ>å@EÛmb ùÃXĞÂË\>,0fêv]?i~fö­Ç¬óÏ½2ÅAÕG{Ô¤ö9“	Œ©l&üÄÿğÎµuÉÍ„ñ²õ(³uÈfgñ¿cFg.ü@¼v¿^Áª2);„té¼eÕÓù¾«Z	õbƒ)Ò–C¼Ru‡İñ	îÓÁ.wu	':àB|^Ï¡‡6ª›yØÍUsÆ‡¼tÕàr”vâÃj×‡Ÿñ	óˆF½ LH·t
ò¼ÿö)6‘‰§g˜ÊË–(Ÿ~‹uRô¡­ÙføŞ+¡Ö]ê¹J2îİˆŸ?¡kb&0ÉŸµ“Œ®ïW b°'Kô9-AaóU?''Ú¾!’!ÁI
‘Yë‰Ñ›éP]^VıéĞ¡İ9g‹£åJ½ßD]t0óŸ²KöáXã÷{Ğ†	²u~:UƒŸxV¼$ö+YwÎ\Ô*| ñz®±= ğødŞ†pË`*DÇØ!r+ã’È/ŒFáâ©:)nªWÍ:wÿk!¿Š«-­¯P3†ö?{ˆË‡EEÓÂœ\éğ´wcA)^Ãêi»TïWˆ<U\‰V„t0*d®İş"ØÎ.ˆÓ-‰|™^¼çëbÌd±şüÿvE‡0Gr’Õm‹¥E ¬Û±2¬H8BæÄkz†WeÁê£/ñîÁ­t=Gw…¿æ5hWˆË?KÃ}z×§Ï~OÒßHPûÕ´ ĞV ç>LÌhÌŸö!kŒRç”Æ˜ì”½øøĞf=Õ"»mş#Şw
y
¡–V/ÿMëıS¾%F‘QOËúA™Ò¿’ËpìÛ¹páÙVC€Ü,ÓÅîÜäòè4]Öã
’óµ.°sùƒï =§{ÔÊ×<´ÏsŒKí`míÿ0Ë%nÓo q›¢Z°`ÏÚ‰ªk›¾¾„3Bt°ãëçÛò]Ó…OŠ1hù-9ãc²-Q½«P¢7íû…WÖ…ôÕ/–…Vp„€q>AŒä?S+Ï Û}# *9ëŸ©Ã%/¢™‡„¡iç_Ôšœs@
¡™Ô„·ä.Ê7-·Qüœ~všÓçy”N.xì×N	€k^Tö?9£s0:\=;Êé½ÿƒ˜Ó÷,·ƒŞÇ8C5>Û; ø×?û80^£P¼¹ë¾û\£íÒsÃ"]á0éaİ‚Ê+@ñ¦™; Lf0uÖ<¯ºÇš¹»²“ù‡[3­¬5^ŒÖWúÜ!ÇÛqè•¾%°Z¯ã¿Bï¸l˜*J6 ØûcÑ¢?³ßŸ¥@-V5«©¿B°üì~Z[gšÏòv·™T¶eq&Jw+ó¨F{§4şŞÑµ‡Ìp¼¬`ç¸Ú1é”	Z>ö>º¯gÉ~ç'% ËÃYİÜs»÷æ`ÿ^Ş–õ±,Ğµv•ßİ‹ÀadO>³A@y)i†>SŞA‰Ï=-Ø†Ïá©ÎîÇßˆØ<®Àˆ~q¦tZFÍb® Ø5‡ş<‰8p_c¬'µ°Å¼ä)ÚKFí@º“9cVºp®-Xy_Ç;>Š‰R¿çJÍ¹.Ôõâ"ÿ¸–\;ÌÙÔsv8JËÿ¤æ~4‡Gb/‚vN†ß4=÷Å¯;2\Œ1.Y×…ïÈó±ÇÁMœ„J6Ş‡ËHYtì#İ¢pBçömóş:è}gt¦–««Ÿ¼+Ã|à©Ùè×Ëqÿ *6GOÌùf ì²º„ˆ¡‘T“ºÆÀ‚¶‡²Q“ãX½+ƒ—§³¸Z³ºÉöZ)^Å¬€AKË®n·Ş•ô	ı ûâ×†³šÇ\vu=D“•6oÌÊ]rñÅäÛÅ3¢?Êá;ÚÙÜ%^†3\,ng¥PÒæÌ)1úE>ı¹ğa…Â²Õ ˆy•6õÑ‹bšàóƒã‰,îä·ŞUJšË„=!óú;oúÌ<pIèF)“B$áÿÁ'	Ğ¹‚
h¹•oW>'Sß\ÓK®$Ú+”ÆwŠsj¸,p”»ÕQX©ˆ²˜JHŸë"c /t¿ê‰\ÿˆ<ÛzàG‘ú¼ëÅıû³†œèş±À´`ùìáÊ`‰“‚xâcEáé4x»Û“J·Åj\ğÕµüíHkîÖ¸"À =G¯”÷+Ø&L[âŠäPeÖ¶Œ«x6ù˜R©d-p§ĞId¹¯VF hM\4Š‰¹ıC¥“¿ŠG€q¸S ®A’ÎÀ	ˆ'°ò¼]ĞÅ—³0ïåŞPà%C¹-òd-Ã<•‚Å¼ÆnÌ-¬„¸©™R$ºæ­„.ë	
j¶ö\ÌVë‚}7Õ|ŸıD7·XÅ=×MãEÆ³¼6am&+âÍÎ<xµ¾{’ËêBÚÒ¸?õßÇø„óù­UIÌ×„LV¥—A"?»ó[®é÷õ~dV-£ZZ¹:Z˜Á¦<H–¼Â+†Ş"X"ØõS#ûalš÷Úòú%ÆË1æ\8:¤ŒäÌ%Ò~/Èã¶º@pÂµŒ!±¬U¤‘rH‡O‹ê«6ıüå¸/JùŠÔ1ºQ¿çk‰åp@ÅTlˆ‡L
­&I~ği2Ø\ÔJF;GÂ&BëbYf`LFÅfAsÓåYDï€«•ƒ‹"±lõÂqJa†'ªÉ¤¤ ‰ÃšruLÃ»`Ç­ÊÈ£òÀê7S?¬û!æãI™@"ˆ‘eàB]ÖS2Œ (‹øîC@g
Nò·~G2Ò†î&g4Y¶•‹ş°mòõX7LÄ*Ûùªæ?¸Z…œB®¤ÎŒhÃİs#ššòJZÀÍ÷7z'*Å«.Šá/3)ÔØÀİÃlôög	¶¿1È‘‰Ÿ”lÔhæñßZÑ¡Ô6}ƒ”…8Ïï@äÇH?šÈB(í‡|õâŠ&Ãd½ab‘ìæ‹.m#ËNÏfËÑ²´¥d>< êŸ^§üyhşÒİ‰ÄÃsöÈV9ôİËI†zPTÈà×†J9{ê±˜W°¶Õõ†,=:
±•ägêæÀ§óÑ©nclude/config/GENERIC_CLOCKEVENTS_BROADCAST) \
  include/linux/clockchips.h \
    $(wildcard include/config/ARCH_HAS_TICK_BROADCAST) \
  include/linux/clocksource.h \
    $(wildcard include/config/ARCH_CLOCKSOURCE_DATA) \
    $(wildcard include/config/GENERIC_GETTIMEOFDAY) \
    $(wildcard include/config/CLOCKSOURCE_WATCHDOG) \
    $(wildcard include/config/ARCH_CLOCKSOURCE_INIT) \
    $(wildcard include/config/TIMER_PROBE) \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  arch/x86/include/asm/clocksource.h \
  arch/x86/include/asm/vdso/clocksource.h \
  include/vdso/clocksource.h \
  kernel/irq/internals.h \
    $(wildcard include/config/GENERIC_IRQ_CHIP) \
  include/linux/kernel_stat.h \
  include/linux/pm_runtime.h \
    $(wildcard include/config/PM) \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  include/asm-generic/fixmap.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/sched/clock.h \
    $(wildcard include/config/HAVE_UNSTABLE_SCHED_CLOCK) \
  kernel/irq/debug.h \
  kernel/irq/settings.h \

kernel/irq/cpuhotplug.o: $(deps_kernel/irq/cpuhotplug.o)

$(deps_kernel/irq/cpuhotplug.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                /dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ctype.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
    $(wildcard include/config/HYPERV) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/filter.h \
    $(wildcard include/config/BPF_JIT_ALWAYS_ON) \
    $(wildcard include/config/HAVE_EBPF_JIT) \
    $(wildcard include/config/IPV6) \
  include/linux/bpf.h \
  include/uapi/linux/bpf.h \
    $(wildcard include/config/BPF_LIRC_MODE2) \
    $(wildcard include/config/EFFICIENT_UNALIGNED_ACCESS) \
    $(wildcard include/config/IP_ROUTE_CLASSID) \
    $(wildcard include/config/BPF_KPROBE_OVERRIDE) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
    $(wildcard include/config/XFRM) \
  include/uapi/linux/bpf_common.h \
  include/linux/file.h \
  include/linux/rbtree_latch.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgcmd_drivers/media/i2c/tvp5150.o := gcc -Wp,-MMD,drivers/media/i2c/.tvp5150.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"tvp5150"' -DKBUILD_MODNAME='"tvp5150"' -D__KBUILD_MODNAME=kmod_tvp5150 -c -o drivers/media/i2c/tvp5150.o drivers/media/i2c/tvp5150.c 

source_drivers/media/i2c/tvp5150.o := drivers/media/i2c/tvp5150.c

deps_drivers/media/i2c/tvp5150.o := \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
    $(wildcard include/config/MEDIA_CONTROLLER) \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
    $(wildcard include/config/OF) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/RETPOLINE) \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/dt-bindings/media/tvp5150.h \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
    $(wildcard include/config/ACPI) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/X86) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/ioport.h \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
  include/linux/bits.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/vdso/bits.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
    $(wildcard include/config/64BIT) \
  include/uapi/asm-generic/bitsperlong.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/minmax.h \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/irqhandler.h \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/bitops.h \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
    $(wildcard include/config/SMP) \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/err.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/sysfs.h \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
    $(wildcard include/config/SYSFS) \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/mutex.h \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/std