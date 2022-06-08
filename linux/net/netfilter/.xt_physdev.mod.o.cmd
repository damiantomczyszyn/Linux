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

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ïŽ×.Í…E?PÄyclÏÍ[— Ýñ"n¯O…ïï æqÛƒäÊÓC0KÔÝ„=_t¨cÞbÓÏ0ÛgŠü˜!ü­ø™¥>q9ìM>dÊfÓ¨ã¸&\÷Pˆ¤Üª`©4éÓÌ„æØ	/JC‚êB’Ág=ÙJ¼ yœ2LSƒ@úSÏ…FÑ».:Xê/Yózv÷umem”*îõ€ØŽ_f–0Ž›!N¸D)Ë#×` G5!RH–EÙ€ð=q"Åþ¦¯®üüÿ3»ÜAZsìuí0“ê+~—_¢‹=FeòÁ™7Š)è^0‡!xÆë¯ÇHGeV‰O-ãïf!ÙÏËôÃ¥Äaš¹¯	,ãJÖŸ¼ÙlÌM+*¹mò˜S¡’Q	º/»úÆ	-z=­š-@<±À’ìŸ0Ù±V×/M>è`¦e‚R_ŠÚ•>…´¿û†kñµAæˆfÍ|Žè"Êáæ,ô/•Ëàí4ónªÍl±™ h)6\ÿ[{›ÉÞÜñzÛV°èhsÉïHtZ2n‚^`;ªhº*áÂ“œ‹}Ÿ‹"7ðûí6ÿOï¬eÁe‰öûa¨Ï±Á­iˆƒýJ»nû@š=¤Y‰®S¿—0\@1¨“š¹©øëñ$²	°ÆÙö£…k¿´7¦¾®0@Tk†8¨z8Å-cv|#{ªšÿNÐ˜™11h¸O_¿m’Eîpà´Øè„/„Ã
è$Ùtu<»ÁfÈDñÿÏ¾ÃyÃ–BÍeÖºí+fVhcÇ[@ÿÚÃ×¹Œlk³SýImzÊ"…‡]nFÉÒ@UÝôI?
ÞFâe® Ü5ù=ýÒÖ!­€ŠÌPø¨©ÏÁ«>í]õîäŒqÛ²²|ß{Û3Ê"»%ºßÚÊhAŒ^‹f‘Î;Ûð­‘~ê¨r­sE!òÄƒ¼Ëí.ùnÒô~CªHiœónÊÇ“nKgÞTîüvô¡1ÔÀê´{Îtý ;6þ/1¾¬l@ïýµ÷Å²¿</À@¸e•€ÉÞ²[3ýÿ`êRóÙ‡U÷û¸èp¦ÿÝ6ql¿A7;@ÀÌÏ’ô’À·Ûneùò_dL;Ü,É­•Ñ“‘v%.@É7ànW ³û.@(Z?wNÏPãÙ˜ü‹jXs’—6|çZ×ô*dÎ5Á¶7l›´ÏÚÆÙÛ)~â$ãƒËÞm=É”F]šÆüž~ÙüH¸÷Xãƒ„êGÊjÙõ°3½’ÐŒ‚³/¹¼`ƒÑ<Ë5w[Vöýº÷âa\Á XV…Ód1vG™Î¨újTÉ%îßëvæ{XZNÇ¤'sšõ—Mþ˜¯ûØþÙ)¶	WÒðøÙŽƒÐ™±Zæ¹Ã~u…#xE9\£Öƒ«¦ÿ£ÿy¤9î'žQn0†šv”Ö†…ÛbíÝ7sb¢ôLl?ËÈ4§lî™Ñ:€¸ab»-ZücÁò•:’,ÆA×—µ´Ê*9Ô„º=F_d$Nw§Ú*ýY­'~Ríd¬å¿¾¦è‚¦9³&„‚!ÈÃÃ„EÁØÅÇÿ[#!¸Ž|ÆùË	y9ýèÈQ3ÂÂ=ÊØ/›9*’d‚F£ÑSôâ5ÁVû©´KÃÛÎrÖ;íj}SÒ­¯àrß†¾¼äµP^‹—]/F–¢â­×ò‡øvKÌükGJ“è ¿Õ§	é`Ú4ÙÑF.IEÉŒìï
œ Î-œÀ2ÙÐ“¬¥F´&—©¥8
=™ìîé>äQJÏ¬bt Ö-SD·Y20"A˜—$¤pLØ‘Š4}KQoó¶wEr	Îm7ëÃª5ò€>vlü¹¨§²ÄÙr¸ªII=E®3vRsä§”DâÕé½ôî®{ÿàV
Ÿ¼ÜŠ‡-uÁ=àÎ1rú	
áOõþê,^Ñ)…1O]ÜŸŠA_¢ùEÌ2Û†¹:;ŒÏ rOß¶.7ëx,bE\ëŽ/>KËûav"0IÏ1ÂEÐpZ"ç+ !Z
ègŒ,ä¶3#D{´qÅ¿²ÈËaõ¬iˆ ‚PC©¶7<ÓüHçrGšÏF{Ì!RÍÒL`‚Å¿xt'6)'”•ì
 ÅîD›HÅZ9âH¯v}6o©9ÐOËâ´,¼_ÉëÉj€27Š¾ñV
m#î¨ËµÃdÒm"Ë\-t-éjúe;³Pƒ·™ÅQôc ‹RUÞ&s¶sD§[ÞÓ®ëêãÊAü©œ6ª4r@º&°¸—èMÝƒRìÅ¥ÑÔ O=ó‡Mz+>ôB¯á ýGô•F6qƒ˜*ð;†šÍMÿ«Âc¸óåª~.G;Ž9ãpð²ÌÏÖ¸"ß“¤poœ)æÆ¶3n„ÙØAU3é^¾i'PŠqkÛý¼'îaWâ¢WèûmÛ¿†„üŽ`§vŸÅPg}n¤<H^ýxÄ%`›šÓž-.hˆÕPÔéä"z¹`î–§êÑ>Î
<M¸XþX…¾˜v¿´Ü{‡¼ø­Uç‰ÄÜcàÌ +ð8°JÜÑ¥Ÿ+‡ÑI**­”Ó‚ùùžH6Öüéâ-WK^ã1.ÜQ¿Ä^?R–Rjï$†žë’Hÿ¿€ßb"ËSá™
xT± ”´²Mq€ƒ_³Œ%¹<l¨sYç”ÙÚHfÒ ÇŒ:NœÊ{d´gTyã§j†ùßïìIèká¤øyf)º¥(—ü+ÌÉ›hô¿6Ÿüh>Jæÿœgs_É¬û€DÉ!°•^£¶¸ˆÅ*a_Šœ?„> xaÏYój’8áä=©":$8+›´Ö8àhiYò€½‡Vò²DCÖYê>†Ç½èL*’ëœatŽÒNõeFüšÅôŸq:ìûÌ~ÃÓ×€Þ¢”ñ–ys«{‚/4Å(ŠIòª 7÷1ðt¯ûÐ’õräÆT…R4†x”“û·¿l0Ú°½+é,« eïë§”“VZ¹Æb—šå	Dz0òißîö"[åé	å:’ÖC‹üƒa›H¸Ï^²)D^s¶Ä™|3À¿Á0Ñ¤ìEpÃU€\/Š³TÅþ“™.zt¦ø³èG<C^ºvEïÈï|\Þç(Ã!ŒÌoYÿ)ÁN™¤X5‡ 5GÿC®ëì‡)*æl™»`-w}Ø¶kÌHåï‡KcÍµ4ƒ¡Á­"a~<>V=ó²8®süw+^ã5¡ )rï÷‡–º\I4%¶Û@v :Ë*NÞõ †žé§v ðÃQcžõíÔ¡¥nì÷–¬\çj·YAš,ô¯ð‡4çL²ÿ³ž¡‹¸…´‰+Ž¡áU?ñ‰ß^Ò*€Ú½ž÷ fk¼¤KD‘xü+XhF±¡z 5ö¸àƒ±_õ4 ¤çˆB5ÎEÝ™$É/‹%ÿ?ÑåÔLó$øæiöG$«ÞÍNz¨Hƒ00Å^k±1ë<A½´PÖÕaZ[5Ø“Û6´œ ŽR?Ú/€ˆhkÑà|iØDqlÁ¼ŒÄlPm”È01{“ë­°ñÿ’ÜŽ[{ÿa5”á¬—sõ‡“ñs¨æ¸u·3§«þf\ÝÊ²ÅU×°ØµaQX¥-?ŠÒô c/RFÐä8ÿbS%·Úól³`¦¯*eh?Ö×)	W4u&)µ› rÜ;$çuÿ‰cuPtA¬™öÁ„àß·³À>ªÖVæRðV.¶9fXQM°—tÍeM™¯Yi˜»£uhÛV²X@cQx ¼oVÿæ¹Åœ‚^RÛ³ï/8£@9Âò-µÔÞÒŒ·áQ–IoõZ>x¼ ätW–`F•—†±^[ÒzÖÁþüª2 E€!V®×,T'‡å¯Ô)ðªÍ…^'M 6À<uI,àÀN°ïÃCd¿›ÀØz›\Ä×’ûäô
í¶èÄ“!}]Ðëã(š‰™7Ôš%;×y”Lk½b~±Ÿª_¸ºè^åù˜Z©ÔlJ»½Ó</Òì¦su-Æ²¥M•ÞìÃ".´¾#ÐÌ=ùÄ¸!b±‡?6Á‹¿¦47€Å]k+ŒíõŽd`”öaH¨K‰a”"d÷o¢¾äÞ–Õš–Ô­ÝúŒ"'_\¤D!ŒßÎÀ(º'fò4M…¢®º<µh‹±&øvt¡ÝÎÇðRøûÌÄ’B3A3‹µH*giº9ÞMš40áõùtç±YP/'xóF•'o0Hh`~ïq²è*H2Ù_C»+ôÎeWåN;K;´µtQŠôÔWÕ’"%3d[‹¨v ª†¶¦<¡¨>kÞH’µÒ’L\š”ZÊ•,N2høRREPþÔê$+òÝ2/Ðt€Ÿö7¾f´`aH}ÖNW¸bfr¢²ÁàD ¹ªL¤±}ÏS§Ò†íÌ÷«P¤3L ÔSLD'ü¶¶Õü²øo.€™Ó»#šyË¢J·	°5>ág}CU<Ù­Àm}VŠe´åµ^š(2k,t¯©óýl‚ ×Æ¢“êL‹KY¢¶µs>Dù)Ž$Òd~y»êÅ„*|ÔÜ³Ü:ä à½àÕ üñ#by@òàÏÂ«‰§†¢ëF†4²õø-žPˆ…ø	D¸`pY}TG_¶MJ×¼émd13=–™lf€*RB›Æ²š:Å=µØxð¥ ÐO(“ Vý¬æ;cü&{À‚ª¹®ßÔ=SÏD¸­NÏ¡j!^ØæÏÝd
+ŽrÝJ
Ó²jcq^¬…Í+(ÙE……³1¶~ NÎnòoyhÌ4šA5ÝV«r_ÑËþ©ªz1ã…-å²{ÇD´ä÷ìÄ¬åßW5rÌêB—_w¿ãºÿH¾ç[hÄoƒB„²Ø(¬.XûÛÔÊÅK¯«¬¸gTm™Fö}1¥VÐT©–%v7k¯¨¬šX`ž2ÌÏoq¨ƒˆÕ¨,(Eö®ár\œÂ?Íi]üõº2z¬ö/&OŸÊW$.GÝ àe¶‹ÔÆä¶È
!_Öu0ÉAÑ€¤¢ý¸}ÊFÝ³œ,ª¾„ð´®‹Ì[&Vx™‘0LDÁ~(øp»¬ò#¶þ|P1­ˆís±m¨ Î–Pò)†—ìe˜„2›Ù*¿õ"-=ó-õQÀâÒùBdÇwgó0¬´?Q"…²6©”[ú¾ˆy™u.NÒ{Ë³ÉÏ9ü„¨ÒÈ7~:)^å €Ê™-KT¶¾eîqú8}Ÿ±Šx†é´üÉL;£K@¼Ù©³å ÿ²ÈÝVôîWx¿ÔÐ=ËþŽ.ãüó‡w£{¸­µ¸M?¾)DÃ¨EÀôl“@Ö[FK"2è2½^@Yœ¸3Ê-D_†ÿØ$Bõ9#©ø£Œ!;ÉÞ”EaÜ½fgœAjåœoÚC™SmÒF½ù²puiâE0Dò‡Á³ÝÍ‡+2½µÝ¿/´¢Ûb#Vn	V&Uù7“Ìû»Y‰“:6U3ŸùëEwêw"»ÀD*ÿuƒ[ˆàÈ B5Ã‹ûá«çú"ÄSeXë6•ï×2G‚nùtæ˜:°ìÛáø‡£§^|•˜üúº¿~˜¬m$Mhv)ãÔ`¸ ‹¼ó¤ã~&@¨Ù“›†²KÐÒí™Áé÷*Q^§c¾xŸÁ»©‚t˜‰<àB÷l(’Ä6rÝ'P8h®7»Ru¨È*Þè™üc"#ãzã˜Æh=˜³
¹–`1„Ë 4Ê¨®Ã,¹˜ÖÇBº%Úb{çmƒ­Î*ŸŽï¾aúF3¿ŠìÒYÅÆ¡0µGî’GB¥ûÄ‚-dVÔéŒZ¬iËû©R³`òt!JÉ¤i*Oˆ”í¬6o
ÙšŽÅ¢—¶c "¼çÖl©ÌY:ÊŒYÃDÅÓötêøŸŒ¸ÐF>›îÍ$Œ‚D¼¯º…â¥ËŠç§7+âÛC@›³ƒž/«¡uÈŸï1eEšjË—ä˜±_3[˜’Eœ ’«*ÎG„?1å»[NC@ÃõmjŸ0lL— 5¤»tî³ýý²)àtÓõ§b»øËŸm-ñ2·2{ôS4¸z,È&À‰¿ô&8…C{JÝXíÄöRÜ#RÊ”Øá‘ôNRòz²¤ÏÒºÛ¢³U+4l:…{	õ>övÏû"kŸ.÷½+Mþé´êQSã²rçw‘±™§½IÃ½| õhºÈc±±¨K çÔÙAué|¹±]#¡í…zÏqëÌÇ{íÈX­–ÆÀ ÊæZ¶|ÁÉò¯Ý¤žŒX-dhË–k4TsÌÐZn1B‰ìê(9›€«¡ÁÖc¿àÒ®S{ã¤“€Á—¼fð
•½9Ëf(bBt;Ç»× c»Qï—X*Ì®p»‰§ÞR¯C“±ô`µFˆ‡©rSäd‹¹â-÷¾²&r^Õ@õÆåC@‹Ñæûo÷«î÷µ™aœ|?Á|Ãn·Ã¸FÄÑÈíªO`ì¡:Ù¶vföv$4°â†òªÈ?×^ð f)·œ†ˆ}Àœ _Q‰4ìéÛ¿íuÔÓš­ÞÏ½)á	„I^§6â†¹,˜ÀY´šzB•¬ü!	t	{¾Â_$‡žªÞ"ž¥Z¡25ò‡zg§«K'ãÆžûÒUþ |™ýdß¹4¶^Cuw‡¼‹Ÿ@°èÿC—ÐÓÄ¸8—%#…ƒ>ò÷0›efçgÄÀ?“w3ä£Ö
¢ddX^QŒ…<áÑ—ÛÛËg!'hTh®ÃDø—æ>)
0•ñp=Ý7`äÙîŠGù@#Gµò}^+>ü8C<-jò>R%ïž±êôï¥ƒ©÷A%ˆ¶MŒ|ç+!±ü´}[¯fnÙbK‰jGtIXûÏïÄ…&œæ…[,&jBg½P9õŒN¯>½s‡§Ãø¥tÙköô¢Ó@HÍR5”mH?_Z<Ù‘XÊìvu¥35¤æ¢3ÕÞ×x‘3°{$tùÅnÛ¨ ”¯¸/·Ç†¡ë5Óì¥ÉÓüIT I'â8;ï±Ý:Ü9œÐµ2uòWnÎºûge4H°…°“æwv!þ“H¸»óœ%‰‡®Èjç/Vœ"|Ø’šcql4W«¼SN$ª>XP¨H¥t­¨*qPõ¾¶Zó[4ÖàúY´²×‹m%nŸŠ c%Ù’R(b&Úû5‡Ãêóš$k"4;aëþvìÁiw€«CÑYrM@Xe†!¨¤þ*8Z'–Õã{ÛS¶¯lâÿnWdÔ£g¥=ç¾=Š/€=&‡PÍ]=ò™Ã‡·N}‡¤°­Ÿ«¢©sÏr›U 'íÀÄ¦^V*2X½®{¼¸“´Z[ð‰”WøÚ[IdTÓn €ø-¦Ú5óúO[îôÓ"?
^á­åbn‘Lì}(Ú™æG3d¡4=Z(I3¿ó×®¨L¦ã&À½‰ÚzÎ›o˜‹1qæçŒè"wWÜ9ÍÖÅÖÜa¦ÚÔdûP•™¶.ÐÜ¯ýÏmY°9”As0
±lÒEM%ŸÅœ7e©uBƒä¾e%öˆ­{³Ç°î†”d…°U¶§]Ç›¹|9­ÈºÏÙP ?Ã“¦à©è™Z&m-™_áp½í¦	E¨7øÌÝió¿¿#ˆ™^8/=vc¢…ÒÑÉúkõõü»cCøx‰KKñ¢#: _ýoòë¼’ÆåDËaësßŠØ®u†ÌmxJ¶Ôß—3íjí‰½Ò<ÜœÜGlÒ6{#èRh•:IPG‰IÁ“O%_¬ª¿XÓp]?qXâyØû7!])€ˆPÒÕÆéƒ¸Åí&äÃáð‚Àom!8;ÎDwBÛ9'a oóF½0ŠŠ:àŠÊâ^¸^ Kf4÷Ÿ¥ìßL©¹A:42·bç}wÛñ
¤ì§´Ë€ñÊõKÙuPÏˆ~æ…”Á4½”•äO¶ vio·à.Ø†Vü,lÿ”ØÓZDüÎaR‡WåÑ†x­[iw†H	%YâuV³0­Ã#>Z¡t±¸¥˜4vÆ­8ýýôNP{èÁ	ó˜Åºyqkˆ=¼  9ÒÅ
Ð~rûiŽÒ$Ãð˜¸ÁùÐI%÷Ç¯ö"U¿å½Ò…[à`ts>S'Ý0BæDð)µµ[ HüÌa8Ïy®""SoˆÝî|&˜Êìh Ù¿Ð´+•ÌDR°l±òµŒ3D)[vAÜþÄÙöh´æ´‹6«?gxzé2ŽÕá Ó_+­UdèX´|•¹ÿOÛùDÙ'2¤íâh	Ù…ú(­»ª ¸^Áûn5ö>”|u×ðû²$ib¸ôÚ+mCÎìœ‚	}Ôü:sÇTÒ_RÑ¬3¶nP÷§%?q`ýáØ©‘º éÅìÍXÞøAšÜ}¦x¾Ð€\›öA§šŸù{ŒóP¾CjÄ>ð.¸èIÅ^ÉfÇ®0Û•>žöÃÆÅrÝrf‘'JMôîÃbÞ­1WÇ¡ü5÷†ŽØ5TaÁØA8­a­:	º&hgârþYÑ<mÓo| ‚Lq·òÐ6…œ
,8%±´MýL#:;`e¤Pœ&¶¬sR¿zÆºÿ´ß½>2j±in$p)‘ìÏsKñQ¨þôJŠéò\ÏÂ6·×2¹¾O¾œ%^"wª*ÑðHc Fï¹+&½k ÿýÚ-óvRBÄ˜d…Ásßù¿\³lJNEã@J+hRßÉ„åèj}ê9&E<9¿4:'85 ¬VïgKQq×è¥_Ha37 Î?¿% %‰U¦*³¸àÌPcëé¼ëà ;·Kâ¶µdzž:¨[‘K…ófÊ©RwÖ©pàj”[ ¼åÀp­	î/íýÀ“ùþŽãað#­~ÍºîrîÃ|)Üå¹×˜Öy|å±M!ÓÒúÊVøÉ2–½|·’ PoÝâEîU^8gúËfEcºÀ‘Ã9Ø]Æ&)a‹ÞšLþ–Ç–wTÜ½àz6W#(o!O}ÿî«ºîá-sa	Â¥ÂiE…µl£—ó3çÜÚÆŠ¬’4„d>âÇùÿ`Ð÷íïiíB–9 Ò¢,×4Æånã¤‚ä©ã5·÷‚ìnÜ§œ‚Z5æÏ™íë€Œ9•%žBÖ-oeYQš–î÷ú¨Wú?	žŒ»+£	žÆ œÊNM‘z‘æ`ûäbiÕ¹"¨TÑ
<\ô]Ì…DK.î‹ó	.xŠ7\„!ŒÂ“1#÷<)	Hq¹véóÁ!S‹_À‰k£gT(Y‰mÍß¤"oÿØê¥€ÜÓs~nßž·$ki?V$Ý2œsi’BâÕé¿ipzôÙOw›§…6dGòÆ»#d•D;ÝŽzÿx¨ŒjXò¤Hê»Mà ¾;h¶¨€C6T1¡>¶©È®p·¢Ï°“‡í‚«ý¬®(úiŽõ§mOÁ&bvBÝÆ©b>Px$æu*fTˆ K@ôUÂÄÈ¾AFlß?Ó<%ªWãÏP}þZ QÒB¡¶RÞ‘._»?Çô©4_w¼ÏÕ¬0áO¿w¦<ÒÕ”_¥j xT<<ì‡­1JžÑ@5ŽÇ‘ùªÕ™æc4>ÞìÆÞÍ¹Ö1S×˜›‹Ùü‡éµ®Gcvt¸"Ydp¡'š¦ëIšOØƒs|ÙX2JŽÿ}ÛY[„ž&yT\-RXó¶	Í‹o¶!×M†i‘H«þ"H•ù 1j¡Œ&„TÕM›Om.µx´¼G|‡/øÜ’½ú4x†¾°£œ'žmÚQ\Î`Û(¡ai,do¦r³w(­Kl¹´tf¶V Ô,&F2Ù	4a÷éó—ö F™ä²(4“òIªYÄJ"‹@IZ~4þ¸ÇÇ’Æ×ÀéÑ¨>ÂôÇ¸ëg´ 0ñf4Û…xÐFk;êG“ÎÀäTnéãkù³¶ª^»>/8{„"Õ­Ë)¢á½«e"Ë:ÈÐªH¤ÞÎm.c’øïÏ×Œæ±ì‹Áç[QTà[ó±XÁ<Ö—©1½,:kÌSþÒW ¶+†r‹Ü›äÞ_Øå£7Ðº,ˆ&W!ÿtøË©ºûqð;KŠºøyPoõþ§Aó:üÈ=ßÜ$/è€Èâ:½fÞt\	ºðÔå,hcX¶nñ—.“¥ºÁÏ˜+RJfPÐ!†^ý2£hZRÛ¤v¼Ân¾Tj¥g¢m‹4ŠK¹=‘—ï^Ž©®[­¼uF¶d•™êñW•®¨þÂè_MÂ•ŠëUµ<A-›=m+‰&}5žwW§Ãªœõ2˜ï‘×ƒ
™½O*`*Þ<æ€6ŒI¡Ún_âzHžk–á³%¦ÊE=S˜›F¸­î©3ÃÝ¡eÊŠÛŠFxWâ÷¢357û+=ZÚ[7õ.ä·h>)L>Íâ©¡	Zûk«Àˆœº‘~A@b0¨4l qíIç×—Éþ±Ž:¯Ãï½8šþ~¿Î)ÝÌ{·,wòwm÷?õÜOhþHƒ‚•cE©òèÖEÓÁÐ§ct<T¤r)yt§ í¬ÚÇÏs’¼*æ²5\¸@ÀýÒ0¹„ï«¶`©Iß¾ÝO|hYâ2±že…•Ml>¹Õ}“ðBð^ºP·¯êÒø­SP¨°¸©Sí{«¶[>9yS§œ«¾~ü‘ßúè•ŒEl4À’ÙâÌCO3^Üy%6™rI–k¾ÖÊÀ™Ž¾”ÊØ@ú±äÑYg;•T‰—[/¢báÌ/‘ŽŽªÃúâM­ÇÌ ¶6‡j	kÊ7¢z“ÛóœŸ	±nu‡DáâøÁG·‰¢ó]àmß<»õžê˜_„rž`A‹ï%íÅÄø˜ìûˆés“Ï‚z-Ô2û)ï>E†yâ•x½ØŠ“’ß­A¿+Æ¾XÈ6e¶uM¹y
ªŒ	¬gý· ˆ9¿ ÃE²õVmÅãª±m;Ê2¾Tâ…‰Ú&N¤Àk\sÊôÇÛõ X0Ëôw”-Ï£z¢ˆ.hèK@ÙnæBË|£\r“ÈV%~ˆj}X¤ÄYNuspò%G{É ùåýXÓýuy1ˆ•™'›ñY‹ƒªz8ÉI‚Ih«K²ÇWºžrŒóZ7s„0pù­Úæ=,ä5q÷làÁ ò»~÷bdnd!ÀÃñNWSq¹ØÏ©{X’MÞoÑ	fô8…ãù{då§¶3Õ%Î×B'ÃSñPÏ¡Q‰6i•2`%r·¡ëdÄÄ[Ü.¬¤ÿ‹4QGûßù#“ŠUuÊ1Ð™œäxí©ãƒà©q¦œ¼ÉÂ‹%ÿêmºgÚA´E,t€ÏIhóÐˆk×‡Æ¹È!ïÛËª7zT!‘qà(G°|…|¢ºEe±H¾'qÃ:g¨©ÇõCµ ãŽ)àä³Ê'j¦>\(§op£Šè~\å2îmäù ÿ9žØN‡ÈšÌŸÝ\á÷åœ¦óm.ûrõdc^÷Wª.|`ù|›Ã›Q)Ñï|	ù÷”MüZÍErmîùR‡á6Ä_“Ay ¡Û8C¿·ÐBt0V1€lå<”‡µ£s+ìqõ-­*NÊ­¿ÖˆÇØóŸš«ü|1ÞFØŒ"†¯Ú=3ï,.À`X. 	SÙ8geCË±*üÁ {v9>Ó‹¢vÉ(u% €Ê§arKðË‹ãÞßÃDº«ž`uû²Vé$&Í4ØÌ©xÙãÝ5R½S¢³²`¹Ò“—ûƒEàX³X$¹„‚h#ˆûØ+/<oÛæÐÇFÞõôï­fí	->|â–ø-³N4.uki¢˜H¼'yBÒ×þLÎ/«ä¿~Žý4}—™iä×Æ÷TkêÕAéÃ6ÃÉÊÓc#A=ŒsÀÇµ*±Gý2ÝÁfl…ž÷˜y±6 aÐŒƒœý³ð>ƒù™ý™Ž—oZ<ò7Ž¾;}<îãˆRŠ˜Ì/N[\0ˆL)uÙxŒøœª·éÔJ÷iWÆ-ž¾jtsç:ˆÿ³r® ?“ç_;'×ùZÃÛ³#ÖüœÂc´¾í¿ŽEŽ¿[Ó¿~Fj}.öØCªÒ¼CúðÃR˜&¥È#ÊhJ÷øb¾ÑÊBo¥Y¯éëZ=]Îo¹ÚÛIÝ=¼&ü·å„úÁAz•k\¤‘†yáøéáåÎë­™ÙÉÎ}À-å°à
Ý!”VaF‘27…c•í¡b¾~I£7X¡˜Ò€›Ý»‡æZ+Pñ±+ªo0¨áÑ/â_ÈòÆJi_Ó$ðÀœB¡)C»#?ñKÂýe&§íÝ]¸ÉOì¯¯8<{ÿh¦ñ³8¢qŽµ¢öx{¼–I¨;µ’R3¤Ö·L¶«+zÒ/_UBh&éùëÔú²öºö«µúxõhM½Â÷üä>£†vÞeÌ¡©¢ž¹NSrBŠ P¶Ñhæ»Ñ¡b¾2œ^²%ô¤BLÉDyþpŒ…Ãgâ@`šæDÊÇ–¯Á3åÂËÇBßm8K°Õ}­÷["—’êžL–ŒÌ<ž™[Á°´j ?$§gAëÊ;‚ÍF IJñAŒÇÛˆ¿;QZD7å,Ç(†‹ÉÏ˜ˆ˜8*kàþÝy_ƒÜ~ò@×ÐŠàŒV]U•:"
ýw!|,'¡)kîžÚµV¡:ÐVTSq5œJ6*0Ç(í$ÕrY=“`§·–D¸+ÐéÕü¶J%5®˜XxÿgbÚY&såK(‰û¥4PØ–Œ1(¼]}K¢ÙÌmž;kX¤V¬›y5­MmRS£¤YÈK?õç¦½_æYÝ€=ÆÏ/úóÇŒ±(ßZ9H'Öu‰MÌ ]ÖôSŸUˆüØÚ°7jÌŒÚš[.‡8OôD~Q˜«2œEøÿ¬TZØ8 âé ’šOÚxù¯éOgŒ­5D¼ÐF_Á¡¬°î—´O¤áÌ–Ú|±»þ¦ó[ãç£Ø5Ì²¹ª­ïX6+g&åþà“N#æ”HÎ¢@„ºÐõÙ(&y»Ë¦m²_‹ó:†NÏ±†`¬Ž„Ð‹wr™DxEÉÁ¼¶!«/v˜ß¤j.Õ¦‰tmQ!ö÷„ðÃ‡¼Ç1íïÄÜ¸'‡p€%ÿàCÄ×BÞhÍuÝ™5f¼oRÖ+ýª½ÑSI)Ôb×ÉëüâsnÇ¸X¿Øfí`Xóºd‡ÈîY&=™Ü®ÍÃŒú¢;b¬/–Ê„)‚œ¥%þ•±[#‡¿käÝ¡>Õë„é‡ÖB8³e.µ7„žv6bsŠ¥ŽòÆÕhÃ_'ïãZøÍ³t.Éz*ñè–á@<Š*ö(Èß×–<½ÒxTöu€°Ý_ÜÃ—ñ£Óê‘³ÜÂP6Ô2¼O”øÜ)€`|Ã{ÓÞÐ`é"¶¯}6À~Ê ðS"–L[ÈEmš4øg´Ï¨tói7•).j‡ñÌÄ[Ëþ™‰OÆò>I™‘ªÔÕù8Æ¥–¿ãa…îÿòk5v)è…>½çîD„F2{bâ*=±V3‰E¦ÏÝ4ùb­k’jß ë‹éKG~=rq›’¸Q?]š‹e!ªøÉ¾"“ê<¦Éžo…Ò®;Ö#@²S3ëºúâ8÷aû7=wöÁO¸(;:iÀ×n·”i¿½.Ž…FF‘Ä	ÿ¯ý$–§Ÿd[Wî)gú‹½¡ÄQŠ@áåÈ
{kE1»Â}$!;ç>âæš`¾šs3”oÃ›lj`R}¹–çˆä^n{›ãhË¦Ð£_üÓZâjzí„ŸÅxá3¿ç‡CÇ&ó#ÇýÚsz6†ˆ±ßÊKÀf½¼½²ô:ÏËÓ»|>5FzuŽ(ªÏ_é²Ù×–Ó°Þ0H®¬ú¸¹r¡ìo¸âçó«ÔòËqãaº'lgíio¡6ß–Èñ‚× Iýf»Sº§Jøð?Ê_Qj]Å‚ÆƒøÕ‚FØ;o­²pšjœ0[tz‚Ke«bµª9ä§Ñþ{ÕSØ	® ÉŸk¥)³núKï${@½$½s
Ú0W%íVNEÖÀ3¥âË3ã®é•±Kø1¦K™ïVsÃ„È…°äÅ'ÅÑ¯T¾žu{È9”Œú§;%*ÐTÅGó{õ¿‚8$g$%>œ^ê®‰µóÉ¸e¨{
¢ó ÙÞ¼- ±úš9¦í±¥âÉà’ä¡àsß±§Wc÷"ÑËß/JÙwF£•ªq
š-×Ô)÷
:HþYç[9ÄMx'³¢u]­æô/ÙxÑæ<$²#ÁÅäZù,!Ãõtà+šØa˜°™=]÷tÞ™Ô>µoS2¼æ¿˜¼aöjT¤[ë«”6K¦©ùèDE'Š«¯«Ûé™¬ðlQ±ÙÔë¸¤®$8¡ÂÑ8«*ÞwhÂq§-Á§!"'›#>˜V—`®¹8¼Q˜sjþšÝ0ø#$àðÊ‰BÍ&Îÿ¯ªF#\Þß¹×½Ö›pšmÚðXm,ü¯ÉÔ³›· „–Ðjf+Ëîk {XÈj\×Oá<bá ºû^y“ZUa½PÌçKb™CP¶ÿ´'tàYÃ`PS#‚TršÍÇô-.tqÿÙÕøv4«hò·*—Ø³ÈÀL7ä&éžIïSµÉÍ¼ áL#°ñN¾}¡bÐ²6×h€™ÕS:N” zŸ—J&g†ÈÐDpÓ&üôÂ<gê6By·KJú…5«úã.ô_(#2=€†Tðüõ3þ”`-¥öR FÃg¿ã¬Y]J°áèÒ(9²ÊˆØ4OqHºx)ÿ X)|…}i%ÊvžüËe™šN—Îã×µJÌ6ÄÒ#"’÷TWXõ
Ÿ:M˜'¢u™5fæýäRí«XV;}ÂŽJÜÞžtÜÊzÕí&´I„¹ûdÉ¢|ØõJÅ°Lwômi.2Š­öé+.Ë˜ ØØyz÷6gmûC9 ¦ü¤q‘™#®8<šîYãVŸÏ’iãÁ ~x0®ƒò–>æYzxh\þ§È£€À ž­É+ÌëeCÖ>L1h8ß8ò‘¥£*Ç`â&ùy«lxh>!PˆaVBQù±¦…ÆÛHŒEÏXêQO2ÜO¢}Câïe–Ü“{Ò„¨Uª·Ð¾£:Tdº½bò©2Íé<š•ëíeGX»`"Çw¹Z2Y3õáµOÖ¡­3?uÞÔºoÚÏðþgºZ¥·ŠÂÔMX]¤ïšáó¬§±[Ê×;–þXÈç*=¸²!:ïO¤4`ß«’jX¶Û›xNõ>eÛ‡•ÓjÇ/Šìá¨b"Åbs[rpçòìßGæÂÖdn/“ð­+\¡ß¹N&V.a÷]:þÂ"½½Ÿ¨ŒÆ/nÓyÏÛÉ¯M^ÈfçDe»òu’½pZŠpx_¹L&ä+@PñìÒGõ;ËÅõ®™L"ÄTÄ‡SŒ âBYª+â¿í¼y ¾ÉiS1­ùí€•ÚV%W4¶>@ÈªK3¤))`£]ù'@×7VÛåÐÒqL"ÝþÃ–‡ê5Ì/Ó{Q‡/¨Ì8¨µÛ£¥û ‹Év‡÷dD¬NJß°–}Ôûõ£úYí¯µli½Ù±£Up*ª*(±n ®zwj/Z7Èí‘
1_‹¹Û2G@_½Ÿ«~[(VÃ.ýWŽ&Ibƒs…FÒïÉ™	<@IPçŠd±*KR;íó£1rh©XC£t&Ï,ý4®¹ŒÐŽÏœE…!-„ý"äi@ÃáAø¯\Ej%"äl˜;"ïÉïâ}DyÿÊG™¼_D•	ƒßx‘^ÝW‰41ó«[“£°*èV¹ê2A^yÅÿ,f¤ ŸƒA…ÿþpnãú€·Ûí“(ÙµÒÈ²ßÉŽB\çK9yOñ˜	7ÚÉH`ŽæÄž’¦_}yï%“çòŠw´‹Ó'$È`ßðÕ™ö1éÞRýì2*ÀÌÄ_¿_TN ç¤~›dlqdÊ06rù±ØÓï?2’é…"s^/r±=N¶ð>kr¼F…#Ô>‡8{ñ—L‘´àÚõj9˜Öþƒê–FePŸGI y–æ†žö0NZIt®
ãUšjzáßá8Yð3_ö C8@BÚ`Ù¢îŽï?3ôý¯Œ
$¦1ÛxÏºò?ü‰:{r_ø‹˜)$4Œ}âÉ] BtÀP €0øOß+¸MN‘Áf+SÁªÕlts]Þ@*+€Z±¶º6„éì¤°A
È!?Xÿ4ÝW‚¿‚2Ç/ÑÃUžN§lDSô„vDH³ÐÄÆ…÷~*ÕV[5èÜDãV,îgi.!V£5ãñu1
ÂåZã’èúl‚|‚½¡xÌÌw£v——£Õi6ò¨?ÂÄ¸*Ç·2Cß›ýgsL•(Œ^ÒW
Òû‰âtN\" ×¾½û#<cY?tFÉ¬‰Ê…¤Û²ì,sQ{~0â4±ÏyÔNïÈÕyç|s±I,óH·—w4O_L¾¡]¢¡l§ÖšÖ©AÉGÔ:–ÅÛVyŠ;¦½6;ž­yþë-}ˆ&mvzÊãí„(‰Z:ÆÄo«r5ÀØà&ê”.éãpt`–®áÑð^¤Ù?Üiû)Yt´BÞK´0%»Ž&->§snÉÍŠ<‘y¢É\lï=ù#G`§y.Ì)ŒÃŸü-·á¹A]ö¡]ìkHÔ^ÀX×+è×b'Øý‚9 ¿b¦š/—‹êKLOT?—Á‚poQv«C6Ö<¤•£m™Òý ÍØò°#FA.H…Wé"ùÿHJ^/‹ñQ(tDê®O´Y6åå~Œ{•ÌÀ²ïfe§RËcÈÕ˜2?) ßîFFÞÒ‹‡¾N§'p‡Þ´ñj›/µ:Ž&é9œ'»×ìIr”}¡æ·Ã€pÊnÁl™¬äñÿxG3.:Ó“O‘-e;Y³c®dÀõ¯Ç¥hš–‚Š˜1œ²ûÙ&Û¢:hb!¸&¼Íº5z`ú…¯y’Qd#¡à­E.-ß.5DÔ}L$]qQÎ¬ü>Ï\‹éÕÅŠ5Gx‘¨ÚÔ(>ïÜõØ_ú8ŠvÐ±d!6®e>g)¼Ë?1)êÒa®…©’,,t2Ks’·À:ÙYÂ{–*ô`}ò£3³PBPsZí1Ã‹¥b§
‹"¢ãÝRï¯è%zRÁ×d±}Îçï_èü7…W^Ç|½BÿOiqIf¸ya„:Éï>G^z—œWhÉï8u ù‰0SÊãâEXMâpPœ×;gºÊD„òð‘I|[TH­²zäÖkÛBW«ÿ{áfê¡a+í
šÀfÇN‚Uö=C¸0Ïsõâ«¡¸‰&? ËV8Œ³×ÖÀö›Çý	tˆÚî(´db£×9š8É*ŠÊ7SdwúúÑ@ã+òž‹”ŽV`¸HMU$ÑšR2ÕÌip?õÆÄƒCÊÚ«|ÔÇn]N÷Hÿ$U’f<M6Z¦@Kæ„‰¸•í_ÆpÉÛó*ñÐ¢šK:%º<F»þ#¬%ZWk¢N £Á‚y®ŽfˆÔÖÍÁ<.}©{*xþJ0š$éHêŽF[Kb«Ûð²¤åá‚|Àbº@óÄ¦ï¶îHžˆªl'¨#¢÷T¸«üpÛ1;WF£gëÍ!Í6²¶Ò"òáW"‹
*A´p†ÕïâÒö[šåÄe†Wä„ÉcÐè¬ÁS4êÚ—vDÒA8Á]þŽ uMóÔ
Û1MrŠ§ðÞËSk¤¶÷!o¯¾ZÛ¾EU® àâo}%mŽ5E“:5ey±w¢NÄ‘*cølz‘Ž˜TÓÁŸÜ¥	M"¿v!ßé®*ÄA$®J7A. ËTe*¹¡ž¼©åúÀˆsÇE*—Ó(WØÏ•ÈØi?.o:ºq”KBaaã³5½Ôý:‡ßz-Â´­Tì#3ãà34¨×A>(™ú1Õ­º†w×Bcä‘]#°ãfÎ°
f§´FDm¢œ÷›ð"îˆQ²&;ÐçŽBšò»tpë•~UÎ^cñ¿‡ä`¹ñ³ã5:	iÓ¥VšTÅùÉøE¸%ÜZ_7®Jó !Ç¾³L©Y_¾(¨ÂÁ‚gx²Ú·"4Lÿ•¸	v’É}ù·A¸GóL°ßpïW$Ã2uÕçz¾˜û§(*éÉ‰hÑéü0$@êêà»WÜßJÝÔ×À‚ÚFõ-3àº;	Ü÷ ˆÅ.ãõoÙSªvºj^^štý×B¡Œi=Ú=2ÄhÈ‰XMgÍVföËøü·ÃX7ämn¡\Ý
P.™hb~¸N’S§”‘ëÂDÁr™†œçIÅ =ô5“TvWnÂirÔV\·¦úBRÝóÝ|ûQT°¬ýI3¤¢ÂãÏ>mû•ƒj#…¥5©ø
©¥œ%·°Ü2oÖ$Ê"9ÈÜ[àšÓJ9'.±5£ƒÊÑút¹·µ {²zÖFe¡a)¥š˜êAýdŸ>=÷Åãy[½ÙÎ±”ò“øZ\¼6×A>,ÁW+/Þzp- ùD-ÉY5õn]ÂÖdAÀW.:Ëƒ‹OS€
ûÃb#5Q$uñOŸ‹S©w]À‚Å„Í}ÑKó™xl%QÙVÉÇ^¤ójcóÃ‡¹j_)KmÙÈP¼
Á+T~€³!ÙWº¨½ ‘]×8q{\{ÅÆŽöá1¾úƒtÌ€ÓîÿˆV`Pì/$ûüïPhæêiýÞàBo’KqøóUlÁM©àSRŸú-b)î˜7‚LmT}æÁÁåO22á6èã Äªã´”ìõåeM±¤<WÚìèTVÚkÅœ¤3_Áktº]]Ü>Ð/ñ¶¢'Ë+Åß—ÊŸ\9‘ýe¶V¢7ù4)
}„>4ø‰µò’Ä$YÑž<Ðè¶²ö×LäDù>¥½ñÁÉûÎïÁ0é¯F~ ˜(ö­¶s Ô»p.¶¾m©'=¬pÿy©ûõÍUë‡2åü,;‰O_Î‘*YµÒÏv`GÝr’?ãÂrèê)üÑÒõçË˜órO¿ó©/åé€éû€Ü¹.³_Ô¹ññfˆöEØQÌŒÕüüYýL'‹@U Aià_ïvØ!ÏyJT °„dÅß¶„ó=á=¡œ`€VÖéj®¯Ì™^£ÖÖ…¯Lè†¯Å7Y¤†{MÀ*‹‹lÃb"LàwuLã =ïTQ,Ç=ÃðÒámj€6kYµ¾óÝ·2—”Hnˆ©ôÆeRhòÌP(ÇËê=wÐ^D¯¦îì|(ÝöðaSÏ|(œ9Ý$ƒckQÎŽ
ñ¶Éƒ¬8ÝCŸmïRüßáÉæ€¼UÉW-·1M‹‘õßy
È=ZcõÞ¦úay½y`’÷e†$°Cí©ÆÄ>(×wð(Û$5»mcYe£`ã˜ŠÌ'žõãã×WmnûOmeÊ˜Û•_-¨Ìã÷‚¸âqiÐ!€MU€ÿ~vœƒx€óòÝo`G'#«’­«»þF¥ÔAöA»»Y€IñõF,¾±‡òù#W6ÁyV¿ñÙMIVî*ÝZò¾+Hù)CIx–†2Ðô…‘^öªI8dð†R,HµjTúñaÍP!Å sÖÉ2„Ye{GKm ”9:7Š½ÉYÿít93vª¨&Š,¶évDŒaÙLÔ(g	Í¬Ë¸ü»eå1Ô ‰Æ°sýñ.¸+\±ïŸñ@L¸ªæŸãˆš÷0:RÊÇÉl;¦1ƒr/“ÒÖáû3çlë§/‹'‡€0<ïï‘(]¬&	¨ iõ‡ôç	…uKü+÷'ÝxfrCbBÖîûòÍZrj`µÎpÖÁÁu¤²üXcƒZK6ÁŸKlÙ»ybŸÈä©¬$¥žÎeñe‘B Íú§Š!…»L²ÏGæ¶ì¦‚­òq²`‚wÈ]Ë_[—Vå‘_7Ú–(×>ÎT‚³fü[8Uh_`³‘‚ÙOÛdlÄ`¢ÁÁ†ƒ«íY´¯z”ŸDx€ßÕ0ˆ'…ÏBù¸¢2’+Ýdg