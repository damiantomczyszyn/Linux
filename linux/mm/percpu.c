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

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ��.ͅE?P�ycl��[�� ��"n�O��� �q�ۃ���C0K�݄=_t�c�b��0�g���!�����>q9�M>d�fӨ�&\�P��ܪ`�4��̄��	/JC��B��g=�J��y�2LS�@�SυFѻ.:X�/Y�zv�umem�*���؎_f�0��!N�D)�#�` G5!RH�Eِ��=q"��������3��AZs�u�0��+~�_��=Fe���7�)�^0�!x���HGeV�O�-��f!����å�a���	,�J֟��l�M+*�m��S��Q	�/���	-z=��-@<�����0ٍ�V�/M�>�`�e�R_�ڕ>�����k�A�f�|���"���,�/�����4�n���l�� h)6\�[{�����z�V��hs���HtZ2n�^`;�h�*���}��"7���6�O�e�e���a�ϱ��i���J�n�@�=�Y��S��0\@1��������$�	�������k��7���0@Tk�8�z8�-cv|#{����NИ�11h�O�_�m�E�p���/���
�$�tu<��f�D�����yÖB�eֺ�+fVhc�[@���׹�lk�S�Imz�"��]nF��@U��I?
�F�e� �5�=���!����P������>�]���q۲�|�{�3�"�%����hA�^�f��;��~�r�sE!�ă���.�nҐ�~C�Hi��n�ǓnKg�T��v��1���{�t��;6�/1��l@����Ų�</�@�e���޲[3��`�R�هU����p��݁6�ql�A7;@���������ne��_dL;�,ɭ�ѓ�v%.@�7�nW� ��.@(Z?wN�P�٘��jXs��6|��Z��*d�5��7l�������)~�$����m=ɔF]����~��H��X����G�j���3��Ќ��/��`��<�5w[�V�����a\��XV�Ӑd1vG�Ψ�jT�%���v�{XZ�NǤ's���M�������)�	W���َ�Й�Z��~u�#xE9\��փ�����y��9�'�Qn0��v�ֆ��b��7�sb��Ll?���4�l�э:��ab�-Z�c��:�,�Aח���*9Ԅ�=F_d$Nw��*�Y�'~R�d�����肦9�&��!��ÄE�����[#!��|���	y9���Q3��=��/�9*�d�F��S��5�V���K���r�;�j}Sҭ���r߆���P^��]/F������vK���kGJ�� �է	�`�4��F.IEɌ��
���-��2�����F�&���8
=����>�QJϬbt �-SD��Y20"A��$�pL�ؑ�4}KQo��wEr	�m7�ê5�>vl�������r��II=E�3vR�s䧔D�����{��V
��܊�-u�=��1r�	
�O���,^�)�1O]����A_��E�2ۆ��:;�� rO�߶.7�x,bE\�/>K��av"0I�1�E�pZ�"�+�!Z
�g��,�3#D{�qſ���a��i� �PC��7<��H�rG��F{�!R��L`�ſx�t'6)'���
���D�H�Z9�H�v}6o�9�O��,�_���j�27���V
m#�˵�d�m"�\-t-�j�e;�P����Q�c �RU�&s�sD�[�Ӯ����A���6�4r@�&�����M݃R�ť�� O=��Mz+>�B�� �G��F6q��*�;���M���c���~.G;�9�p���ָ"���po�)�ƶ3�n���AU3�^�i'P�qk���'�aW��W��m�������`�v��Pg}n�<H^�x�%`��Ӟ-.h��P���"z�`��>�
<M�X�X���v���{����U���c�� +�8�J�ѥ�+���I**�������H6����-WK^�1.�Q��^�?R�Rj�$���H����b"�S��
xT�����Mq���_��%�<l�sY���Hf� ǌ:N��{d�gTy�j�����I�k��yf)��(��+�ɛh��6��h>J���gs_ɬ��D�!��^�����*a_��?�> xa�Y�j�8��=�":$8+����8�hiY򀽇V�DC�Y�>�ǽ�L*��at���N�eF�����q:���~���׀ޢ���ys�{�/4Ő(�I� 7��1�t��В�r��T�R4�x�����l0ڰ�+�,��e�맔�VZ��b���	Dz0�i���"[���	�:��C���a�H��^�)D^s�ę|3����0�Ѥ�Ep�U�\/��T����.zt����G<C^�vE���|\��(�!��oY�)�N��X5��5G�C����)*�l��`-w}ضk�H�Kc͵4������"a~<>V=�8�s�w+^�5� )�r�����\I4%��@v�:�*N������v ��Qc���ԡ�n����\�j�YA�,����4�L����������+���U?��^�*������fk��KD�x�+XhF��z�5�����_�4 ��B5�Eݙ$�/�%��?���L�$��i�G$���Nz�H�00�^k�1�<A��P��aZ[5ؓ�6����R?�/��hk��|i�Dql����lPm��01{�뭰���܎[{�a5�ᬗs����s��u�3���f\�ʲ�UװصaQX�-?��� c/RF��8�bS%���l�`��*eh?��)	W4u&)���r�;$�u��cuPtA������߷��>��V�R�V.�9fXQM��t�eM��Yi���uh�V�X@cQx �oV���Ŝ�^R۳�/8�@9��-���Ҍ��Q�Io�Z>x� �tW�`F����^[�z�����2 E�!V��,T'���)�ͅ^'M�6�<uI,��N���Cd����z�\�ג���
��ē!}]���(���7�Ԛ%�;��y�Lk�b~���_���^���Z��lJ���</��su-Ʋ�M����".��#��=�ĸ!b��?�6����47��]k+����d`��aH�K�a�"d�o���ޖ՚�ԭ���"'_\�D!����(�'f��4M����<�h��&�vt�����R���ĒB3A3��H*gi�9�M�40���t�YP/'x�F�'o0Hh`~�q��*H2�_C�+��eW�N;K;��tQ���WՒ"%�3d[��v ����<��>k�H��ҒL\��Z��,N2h�RREP���$+����2/�t���7�f�`aH}��NW�bfr�����D���L��}�S�҆����P�3L ��SLD'�������o.��ӻ#�yˢJ�	�5>�g}CU<٭�m}V�e�坵^�(2k,t����l� �Ƣ��L�KY���s�>D�)�$Ґd~y��ń*|����:���� ��#by@���«�����F�4���-�P���	D�`pY}TG_��MJ׼�md13=��lf�*RB�Ʋ�:�=��x� �O(� V���;c��&{�������=S�D��Nϡj!^��ρ�d
+�r�J
�Ӳjcq^���+(�E���1�~�N�n�oyh�4�A5�V�r_�����z1�-��{�D�����Ĭ��W5r��B�_w���H��[h�o�B���(�.X�����K����gTm�F�}1�V�T��%v7k����X`�2��oq���ը,(E���r\��?�i]���2z��/&O��W$.Gݠ�e������
!_�u0�A�р����}�F���,����������[&Vx��0LD�~(�p���#���|P�1���s�m� ΖP�)���e��2��*��"-=�-�Q����Bd�wg�0��?Q"��6��[���y�u�.N��{����9�����7~:)^� �ʙ-KT��e�q�8}���x����L;�K@�٩�� ����V��Wx����=���.���w�{����M?�)DèE��l�@�[FK"2�2�^@Y��3�-D_���$B�9#����!;�ޔEaܽfg�Aj��o�C�Sm�F����pui�E0D����͇+2��ݿ/���b#Vn	V&U�7����Y��:6U3���Ew�w"��D*�u�[��� B5Ë����"�SeX�6���2�G�n�t�:��������^|������~��m$Mhv)��`������~&�@�ٓ���K������*Q^�c�x�����t��<�B�l(��6r�'P8h�7�Ru��*���c"#�z��h=��
��`1�ˏ 4ʨ��,����B�%�b{�m���*����a�F3����Y�ơ0�G�GB����-dV��Z�i���R�`�t!Jɤi*O���6o
ٚ��Ţ��c "����l��Y:ʌY�D���t������F>����$��D������ˊ��7+��C@����/��uȟ�1eE�j˗䘱_3[��E� ��*�G�?1�[NC@��mj�0lL��5��t���)�t���b��˟m-�2�2{�S4�z,�&����&8�C{J�X���R�#Rʔ�ၑ�NR�z���Һۢ�U+4l:�{	�>�v���"k�.��+M���QS�r�w�����I��| �h��c���K ���Au�|��]#��z�q���{��X���� ��Z�|����ݤ��X-dh˖k4Ts��Zn1B���(9������c��ҮS{�������f�
���9�f(bBt;��� c�Q�X*̮p����R�C���`�F���rS�d���-���&r^�@���C@����o������a�|?�|�n�øF�����O`�:ٶvf�v$�4����?�^�f)����}�� _Q�4��ۿ�u�Ӛ��Ͻ)�	�I^�6↹,��Y��zB���!	t	{��_$����"��Z�25�zg��K'�ƞ��U��|��d߹4�^Cuw����@���C���ĸ8�%#���>��0�ef�g��?�w3��
��ddX^Q��<�ї���g!'hTh��D���>)
0��p=�7`����G�@#G��}^+>�8C<-j�>R�%��弄��A�%���M�|�+!���}[�fn�bK�jGtIX�����&��[,&jBg�P9��N�>�s���Ð��t�k����@H�R5�mH?_Z<ّX���vu�35��3���x�3�{$t��nۨ ���/�ǆ���5�����IT I'�8;��:�9�е2u�Wnκ�ge4H�����wv!��H���%����j�/V�"|���cql4W��SN$�>XP�H�t��*qP���Z�[4���Y��׋m%n���c%��R(b&ڏ�5����$k"4;a��v��iw��C�YrM@Xe�!���*8Z'���{�S��l��nWdԣg�=�=�/�=&�P�]=�Ç��N}��������sϝr�U '��Ħ^V*2X��{����Z[���W��[IdT�n ��-��5��O[���"?
^��bn�L�}(ڙ�G3d�4=Z(I3��׮��L��&����zΛo��1q���"wW�9�����a���d�P���.�ܯ��mY�9�As0
�l�EM%�Ŝ7e�uB���e%���{�ǰd��U��]Ǜ�|9�Ⱥ��P�?Ó���Z&m-�_�p��	E�7���i�#��^8/�=vc�������k����cC�x�KK�#: _�o�뼒��D�a�sߊخu��mxJ��ߗ3�j퉽�<�ܜ�Gl�6{#�Rh�:IPG�I��O%_���X�p]?qX�y��7!])��P���郸��&������om!8;�DwB�9'a�o�F�0��:����^��^ Kf4�����L��A:4�2�b�}w��
�짴ˀ���K�uPψ~慔�4����O� vio���.؆V�,l����ZD��aR�W�цx��[iw�H	%Y�uV�0��#>Z�t����4v��8���NP{��	�źyqk�=�  9��
�~r�i��$������I�%�ǯ�"U��҅[�`ts>S'�0B�D�)��[ H��a8�y�""So���|&���h�ٿд�+��DR�l���3D)[vA�����h�洋6�?gxz�2��� �_+�Ud�X�|���O��D�'2���h	م�(��� �^��n5�>�|u����$ib���+mC�윂	}��:s�T�_RѬ�3�nP��%?q`��ة�� ����X��A��}�x�Ѐ\��A����{��P�Cj�>�.��I�^�fǮ0ە>�������r�rf�'JM���bޭ1Wǡ�5�����5Ta��A8�a�:	�&h�g�r�Y�<m�o| ��Lq���6��
,8%��M�L#:;`e��P�&��sR�zƺ��߽>2j�in$p)���sK�Q����J���\���6��2���O��%^"w��*��Hc F�+&�k����-�vR�BĘd��s����\�lJNE�@J+hR�����j}�9&E<9�4:'85��V�gKQq��_Ha37��?�% %�U�*����Pc�����;�Kⶵ�dz�:�[�K��fʩRw��p�j�[ ���p�	��/��������a�#�~ͺ�r��|)��ט�y|�M!����V��2��|���Po��E�U^8g��fEc����9�]�&)a�ޚL��ǖwTܽ�z6W#(o!O}���-sa	¥�iE��l���3���Ɗ��4�d>����`����i��B�9 Ң,�4��n㤂��5�����nܧ��Z5��ϙ�뀌9�%�B�-oeYQ������W�?	���+�	�� ���NM�z��`��biչ"�T�
<\�]̅DK.��	.x�7\�!�1#�<)	Hq�v���!S�_��k�gT(Y�m�ߤ"o��ꥀ��s~nߞ�$ki?V$�2�si�B���ipz��Ow���6dG���#d�D;ݎz�x��jX��H�M� �;h���C6T1�>��Ȯp��ϰ��킫���(�i���mO�&bvB��Ʃb>Px$�u*fT� K@�U��ȾAF�l�?�<%�W��P}�Z Q�B��Rޑ._�?���4_w��լ0�O�w�<�Ք_�j xT<<쇭1J��@5�Ǒ�����c4>�������1Sט�������Gcvt�"Ydp�'���I�O؃s|�X2J��}�Y[��&yT\-RX�	͋o��!�M�i�H��"H�� 1j��&�T�M�Om.�x��G|�/�ܒ��4x�����'�m�Q\�`�(�ai,do�r�w(�Kl��tf�V��,&F2�	4a�����F��(4��I�Y�J"�@IZ~4���ǒ����Ѩ>��Ǹ�g� 0��f4ۅx�Fk;�G����Tn��k����^�>/8{�"�խ�)�Ὣe"�:���H���m.c����׌����[QT�[�X�<�֗�1�,:k�S��W��+��r��ܛ��_��7к,�&W!�t�˩��q�;K���yPo���A�:��=��$/���:�f�t\	����,hcX�n�.����ϝ�+RJfP�!�^�2�hZRۤv��n�Tj�g�m�4�K�=���^���[��uF�d�����W������_M���U�<A�-��=m+�&}5�wW�ê��2��׃
��O*`*�<�6�I��n_�zH�k��%��E=S��F���3�ݡeʊۊFxW���357�+=Z�[7�.�h>)L�>�⩡	Z�k������~A@b0�4l q�I�ח����:���8��~��)��{�,w�wm�?��Oh�H���cE����E��Чct<T�r)yt�� ����s��*�5\�@���0��﫶`�I߾�O|hY�2���e��Ml>��}��B�^�P������SP�����S�{���[>9yS����~����蕌El4�����CO3^�y%6�rI�k����������@����Yg;�T��[/�b��/�������M��� �6�j	k�7�z���	�nu�D����G����]��m�<����_�r�`A��%���������s�ςz-�2�)�>E��y�x�؊��߭A�+ƾX�6e�uM�y
��	�g�� �9� �E��Vm�㪱m;�2��Tⅉڐ&N��k\s������X0��w�-ϣz��.h�K@فn�B�|�\r��V%~�j}X��YNusp��%G{ɠ����X��uy1���'��Y���z8�I�Ih�K��W��r��Z7s�0p����=,�5q�l�����~�bdn�d!���NWSq��ϩ{X�M�o�	f�8���{d���3�%��B'�S�PϡQ�6i�2`%r���d��[�.����4QG���#��Uu�1�Й��x����q�����%��m�g�A�E,t��Ih�Јkׇƹ�!��˪7zT!�q�(G�|�|��Ee�H�'q�:g����C� �)���'j�>\(�op���~\�2�m�� �9��N�Ț̟�\���圦�m.�r�dc^�W�.|`�|�Û�Q)��|	����M�Z�Erm��R��6�_�Ay���8C���Bt0V1�l�<����s+�q�-�*Nʭ�ֈ��󟚫�|1�F،"���=3�,.�`X. 	S�8geC˱*�� {v9>Ӌ�v�(u% �ʧarK�ˋ����D���`u��V�$&�4�̩x���5R�S���`�ғ���E�X�X$���h#���+/<o����F����f�	->|��-�N4.uki��H�'yB���L�/��~��4}��i����Tk��A��6����c#A=�s�ǵ*�G��2��fl����y�6 a�Ќ�����>�������oZ<�7��;}<��R���/N[\0�L)u�x�������J�iW�-���jts�:���r� ?��_;'��Z�۳#����c������E��[ӿ~Fj}.��C���C���R�&��#�hJ��b���Bo�Y���Z=]�o���I�=�&�����Az�k�\���y�����΁뭙���}�-對�
�!���VaF�27�c��b�~I�7X��Ҁ�ݻ��Z+P�+��o0���/�_���Ji_�$���B�)C��#?�K��e&���]��O쯯8<{�h���8�q����x{��I�;��R3�ַL��+z�/_UBh&������������x�hM�����>��v�e̡����NSrB��P��h�ѡb�2�^�%��BL�Dy�p���g�@`��D�ǖ���3����B�m8K��}���["����L���<��[���j ?$�gA��;��F IJ�A��ۈ�;QZD7�,�(���Ϙ��8*�k���y_��~�@�Њ��V�]U�:"
�w!|,'�)�k���ڵV�:�VTSq5�J6*0�(�$�rY=�`���D�+�����J%5��Xx�gb�Y&s�K(���4Pؖ�1(�]}K���m�;kX�V��y5�MmRS���Y�K?���_�Y݀=��/��ǌ�(�Z9H'�u�M� ]��S�U����ڰ7ǰښ[.�8O�D~Q��2�E���TZ�8 �� ��O�x���Og��5D��F_����O��̖�|�����[���5������X6+g&����N#�H΢@�����(&y�˦�m�_��:�Nϱ�`�����wr�DxE����!�/v�ߤj.զ�tmQ!����Ç��1���ܸ'�p�%��C��B�h�uݙ5f�oR�+�����SI)�b�����snǸX��f�`X�d���Y&=�ܮ�Ì��;b�/�ʄ)���%���[#��k�ݡ>����B8�e.�7��v6bs������h�_'���Z�ͳt.�z*����@<�*�(��ז<��xT�u��݁_�×��ꑳ��P6�2�O���)��`|�{�ޏ�`��"��}6�~ʠ�S"�L[�Em�4�g�Ϩt�i7�).j�����[�����O��>I�������8ƥ���a����k5v)�>���D�F2{b��*=�V3�E���4�b��k�j� ��KG~=rq���Q?]��e!��ɾ"���<��ɞo�Ү;�#@�S3�����8�a�7=w��O�(;:i��n��i��.��FF��	����$����d[W�)g�����Q�@���
{kE1��}$!;�>��`��s3�oÛlj`R}����^n{��h��У_��Z�jz폄��x�3��C�&�#���sz6�����K�f�����:��ӻ|>5Fzu�(��_��זӰ��0H������r��o�������q�a�'lg�io�6����� I�f�S��J��?�_Qj]łƃ�ՂF�;�o��p�j�0[tz�Ke�b��9���{�S�	��ɟk�)�n�K�${@�$�s
�0W%�VNE��3���3��镱K�1�K��Vs��ȅ���'�ѯT��u{�9����;%*�T�G�{���8$g$%>�^ꮉ��ɸe�{
�� �޼- ���9����������s���Wc�"���/J�wF���q
�-��)�
:H�Y�[9�Mx'��u]���/�x��<$�#���Z�,!��t�+��a���=]�tޙ�>�oS2�濘�a�jT�[뫔6K����DE'�����陬�lQ���븤�$8���8�*�wh�q�-��!"'�#>�V�`��8�Q�sj���0�#$��ʉB�&����F#\���׽��p�m��Xm,���Գ�� ���jf+��k {X�j\�O�<b���^y�ZUa��P��Kb�CP���'t��Y�`PS#�Tr����-.tq����v4�h�*�س��L7�&�I�S��ͼ �L#��N�}�bв6�h���S:N� z��J&g���Dp�&���<g�6By�KJ��5���.�_(#�2=��T���3��`-��R F�g��Y]J����(9�ʈ�4OqH�x)� X)|�}i%�v���e��N���׵J�6��#"��TWX�
�:M�'�u�5f���R�XV;}��J�ޞt��z��&�I���d��|��JŰLw�mi.2����+.˘ ��yz�6gm�C9����q��#�8<��Y�V��ϒi���~x0���>�Yzxh\��ȣ�� ���+��eC�>L1h8�8򑥣*�`�&�y�lxh>!�P��aVBQ������H�E�X�QO2�O��}C���e�ܓ{҄�U��о�:Td��b�2��<����eGX��`"�w�Z2Y3��O֡�3?u�Ժo����g��Z����MX]���󬧱[��;���X��*=��!:�O�4�`߫�jX�ۛxN�>eۇ��j�/���b"�bs[rp����G���dn/��+\�߹N&V.a�]:��"������/n�y����M^�f�De��u��pZ�px_�L&�+@P���G�;�����L"�TćS���BY��+���y ��iS1������V%W4�>@ȪK3�))`�]�'@�7V����qL"��Ö��5�/�{�Q�/��8��ۣ�� ��v��dD�NJ߰�}�����Y�li�ٱ�Up*�*(�n��zwj/Z7��
1_���2G@_���~[(V�.�W�&Ib�s�F��ə	<@IP�d�*KR;��1rh�XC�t&�,�4���ЎϜE��!-��"�i@��A��\Ej%"�l�;"����}Dy��G��_D�	��x�^�W�41�[���*�V��2A^y��,f� ��A���pn������(ٵ�Ȳ�ɎB\�K9yO��	�7��H`������_}y�%����w���'$�`���՝��1��R��2*���_�_TN��~�dlqd�06r�����?2��"s^/r�=N��>kr�F�#�>�8{�L�����j9�����FeP�GI�y�憞�0NZIt�
�U�jz���8Y��3_� C8@B�`٢��?3����
$�1�xϺ�?��:{r_���)$4�}��] �Bt�P��0�O�+�MN���f+S���lts]�@�*+�Z���6��줰A
�!?X�4�W���2�/��U�N�lDS�vDH���ƅ�~*�V[5��D�V,�gi.!V�5��u1
��Z���l�|���x��w�v�����i6�?��ĸ*Ƿ�2Cߛ�gsL�(�^�W
�����tN\" ׾��#<cY?tFɬ�ʅ����,sQ{~0�4��y�N���y�|s�I,�H��w4O_L��]��l���֩A�G�:���Vy�;��6;��y��-}�&mvz����(�Z:��o�r5���&�.��pt`�����^��?�i�)Yt�B�K�0%��&->�sn�͊<�y��\l�=�#G`�y.�)�ß�-��A]��]�kH�^�X�+��b'���9��b��/���KLOT?���poQv�C6�<���m��������#FA.H�W�"��HJ^/��Q(tD�O�Y6��~�{�����fe�Rːc�՘2?) ��FF�ҋ��N�'p����j�/�:�&�9�'���Ir�}����p�n�l�����xG3.:ӓO�-e;Y�c�d���ǥh�����1����&ۢ:hb!�&�ͺ5z`���y�Qd#��E.-�.5D�}�L$]qQά�>�\���Ŋ5Gx����(>�����_�8�v��d!6�e>g)��?1)��a����,,t2Ks���:�Y�{��*�`}�3�PBPsZ�1Ë�b�
�"���R��%zR���d�}���_��7�W^�|�B�OiqIf�ya�:��>G^�z��Wh��8u���0S���EXM�pP��;g��D���I|[�TH��z��k�BW��{�f�a+�
��f�N�U�=C�0�s�������&?��V8���������	t���(�db��9�8�*��7Sdw���@�+򞋔�V`�HMU$��R2��ip?��ăC�ګ|��n]N�H�$U�f<M6Z�@K愉���_�p���*�Т�K:%�<F��#�%ZWk�N ���y��f�����<.}�{*�x�J0�$�H�F[Kb�����|��b�@�Ħ���H���l'�#��T���p�1;WF�g��!�6���"��W"�
*A�p������[����e��W��c���S4�ڗvD�A8�]���uM��
�1Mr�����Sk���!o��Z۾EU� ��o}%m�5E��:5ey�w�Nđ*c�lz���T���ܥ	M"�v!��*��A$�J7A. ��Te*���������s�E*��(W�����i?.o:�q�KB�aa�5���:��z-´�T�#3��34��A>(��1Տ���w�Bc�]#���f���
f��FDm������"�Q�&;��B��tp�~U�^c��`���5:	iӥV�T��ɝ��E�%�Z_7�J� !Ǿ�L�Y_�(����gx��ڷ"4L���	v��}��A�G�L��p�W$�2�u��z����(*���h���0$@����W��J������F�-3��;	�� ��.��o�S�v�j^^�t��B��i=�=2�hȉXMg�Vf������X7�mn�\�
P.�hb~�N�S�����D�r����IŠ=�5�TvWn�ir�V\���BR���|�QT���I3�����>m���j#��5��
���%���2o�$�"9��[���J9'.�5�����t����{�z�Fe�a)����A�d�>=���y[��α���Z\��6�A>,�W+/�zp- �D-�Y5�n]��dA�W.:˃�OS�
��b#5Q$u�O��S�w]��ń�}�K�xl%Q�V��^��jc�Ç��j_)Km��P�
�+T~��!�W��� �]�8q{\{�Ǝ��1���t̀����V`P�/$���Ph��i���Bo�Kq��Ul�M��SR��-b)�7�Lm�T}����O22�6�� Ī㴔���eM��<W���TV�kŜ�3_�kt�]]�>�/�'�+�ߗʟ\9���e�V��7�4)
}�>4������$Yў<�趲��L�D�>���������0�F~ �(���s Իp.��m�'=�p�y����U�2��,;�O�_Α*Y���v`G�r�?��r��)�����˘�rO��/�������.�_Թ��f��E�Q̌���Y�L'�@U�Ai�_�v�!�yJT ��d�߶���=�=��`�V��j����^��օ�L膯�7Y���{M�*��l�b"L�wuL� =��TQ,�=����mj�6kY���ݷ2��Hn����eRh��P(���=w�^D����|(���aS�|(�9�$�ck�QΎ
�Ƀ�8�C�m�R����怼U�W-�1M����y
�=Zc����ay�y`��e�$�C���>(׍w�(�$5�mcYe�`����'�����Wmn�Omeʘە_-��������qi�!�MU��~v���x����o`G'#������F��A�A��Y��I��F,�����#W6�yV���MIV�*�Z�+H�)CIx��2��^��I8d��R,H�jT��a�P!� s��2�Ye{GKm �9:7���Y��t93v��&�,��vD�a�L�(g	��˸��e�1Ԡ�ưs��.�+\����@L���㈚�0:R���l;�1�r/�����3�l링/�'���0<���(]�&	� i����	�uK�+�'�xfrCbB�����Zrj`��p���u���Xc�ZK6��Klٻyb��䩬$���e��e�B�����!��L��G��즂��q�`�w�]�_[�V�_7ږ(�>�T��f�[8Uh_`����O�dl�`�������Y��z��Dx���0�'��B����2�+�dg�A�<�a��n`&#�90qt��"��]�?�����D��:�� ��>����t�o0\*�ƉЭ�V�����a���yQ|�	�\>C-������,����&bf�A-�����v8a�G!��C_�����Fj2B�{_�
�[��4�D��5��{��D��E�PZ��K��y�>��_z�&��J�2w�(7����`I�e�e�k�]JR�9�x�t��l9�B�/�.�}�
���&o�зg���˳����&�-�����[ ;��A�ʧ�Cx���zH�
����J��y@�Q����j���Eʳ~M�ઓ���P�p�@��M�0�� G���t@��m��ذ�e_5rF�`x�P�8[�`L=@�'���������ZD��k��6������Z��x{�Z�2yw�{�Zd��Z�H�-l�����Q �vѮ�,ؕ���w����-�j��Y�?�\;�g�X���D��i�'K����`����E����m�'�t�*,�U�}&�$	{! *b�ۃ�F�.�OV/�7�~��r0�l�# ���Wu%�[c�O�ק=�R]^������5r�a�4j�w$�����Ġ�ۜ�f�4�Y.P�n_�I�]��=�Jml��,�a�#~�xn찹y�"�'�"Y��CJŭb1R�� `ʶ�L��U��txf��0vI�m����X2/���]>��H�����φM��:a|��}��c�����KJ��wˌ)���F~R��Z��C&3uD���AU6�Oܢ=�U����z�6[� ��J�k�<���7�m-[6��n�_5εk��C�iLx��f��H;��}0��b~#��xṬ-L��|�W�i>������D��P�'����`�i�+�9S<�}�̆�\[�R��o�2 ��:e�Oίu]�1�ˀ\�>��T^%�)�w���y+<�������N��ʵY
����B�q����A�
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
  include/uapi/linuxELF                       j      4     (               �����S��X  ��T  1��Z[�
Ít& ��������|  �AA	  �1�Í�&    f���������u�A��u�A   1�Ít& �����Í�&    �v ������9w6�y   u-��D  �Q�Q��H  ��Q��H  ��A1�Í�&    ������Í�&    �v ��������   v��P�����w � � ���Ít& ����   ��Í�    1�Ít& �������P@��P  ��uÍt& ���L  j �@   ��   j j j@�������Í�&    ������P@��P  ��uÍt& ���L  j �@   ��   j j j �������Í�&    ���������L  ��d�    �L$1ɉ��������I$�T$d+    u����������&    ��&    ������UWVS�ÉЉʉ���@��	�t}����	�t<�Ɖׁ� ��������������	�tk���� �  uq��u}%  � ����Ƀ�����v �   �   �    ���������L  �(   �����1���[^_]Í�    �
   �
   �ƍt& �   �   붍t& �   �   릍t& �   �   떍�&    �t& ������WVS��X  ��T  1�1�	�td���  ���  ��tD�{�s!�!׉�	�tJ�Ӊ�T  �� �  ��X  ��ۃ�`���  ��H  [^_�����t& ���� 1�빍�&    1�[^_ø�������v �������tG�yuAS��D  �Y��H  �A   ���A   �Q�A   �    ���!   1�[Ít& ������Í�&    �v ��������   �   �����1�Í�&    ������WVS�Ӌ    ����   �{��1��    ����Cp    ��  )���t���f���$���  t/�z���t��J��9�w�v f	tC��9�v��$f	;���  u�[1�^_������S���t��u[�������&    �[�������&    �v �����[Í�&    f�������    U��WVS�x\����   ���  ��   1ۅ�tJf������ƌ   ��������  9�w��t*��@  1ۍ�&    �v �����ƌ   �����9��  w������������  �������   �������   �����1�[^_]Ít& �����V��S�zGREY��t<�{t1�[^Ð�{u�L  1ɺ   �������L  �   �   ������ː��L  �p   �   �����뭍�&    f������U��WVS��f�: �$��   1ۍv �$1�f�LX2�|X��T  ����� �X  �  ��� �sD�1ɍF��D$��wl��  1Ƀ�$��  9�t#�P���t��t	;0r;pv��$��  ��9�u݋|$�΋�L  ���   ���W����������L  �Wщ����������$f�LX�����>�����L  �   ��   �����1���[^_]Ë�L  1ɺ�   ��   �������L  1ɺ�   �������    ��L  �ڹ�   ����������   u��1�[^_]Í�&    f������VS�Ӄ���L  ��   d�    �D$1��L$�$    ���D$    ������L$��uk�ẇ   ��������$��u&�D$d+    ��   ���   [^Í�&    �v �L$��   ���D$    ������L$��t��   ��������f���   ��������D$�@t��    %�   ��t  ���7  �غ@  �����1���t   t��l  j �   �   j j P����������?�������������S���   �   �������1҅�x
��[Í�    ���   ��t�H�����   u��[Í�&    �t& �����VS����v  f=PQ��   1�f=QQ��   ��d  ��t�������D�    ���d  ��L  1ɺ   �������L  ��1�������@   �   ��d  E�L  j �P   j �   j P���������[^Ív f��x   �o�����p  1������4�    �V���1�f��x   �E����ԍ�&    ��    ������P(�H��	� tmv;��	� �   ��	� ��   �@|���������   ��4��������1�Ít& �� 	� th��	� ux�P|���   �Ѻ   �����1�Í�&    ��P|���   �Ѻ
   �����1�Í�&    ��P|���   �Ѻ   �����1�Í�&    ��P|���   �Ѻ	   �����1�ø�����f��������d  1҃���h  ��p  ����1�Í�&    f������U��W1�V��,     S�����  �D$��u�Z  f����ƌ   9��  �C  iǌ   1ҍ^�N`苀,  9�DV��@�F|   �$�   �D$1������D$�����������   �E`�ڋ@���������   �t$�D$��1�P�������ZY��x}�<$tO���_����T$1������$�$��d  �������D$��8  ��<  ���  ���4���������&    �t$�F��1��@P�������Y[��x��t������f����  ��t.��@  1ۉ׍�&    �v �����ƌ   �����9��  w������[^_]Ít& ��1�[��^_]Í�&    �t& ������UWVS�Ã��Bf�$�B����  ���$�σ��ʉL$���  ���~  �o��@  ��v ���   9��_  9�u�i��   �    ��(  ����  ���@  �   ���   �Љ$�������t!�$��������X  �@$    �@ �@$    ���   �Љ$�������t!�$��������#  �@$    �@ �@$    �D$$�������u~�N��h  ��d  ��t~ǃp     ���1������  ��tt�G���T  ���  �O���T  �o�1�!�1��X  ��tI��u�Ή������)���1���[^_]Í�&    ǃd     ��h  ��u4ǃp      ������1���[^_]Í�&    f��    ��M1�����ǃp     �������T$��������������t�t$$�p$�@ �p$�%����������  �W  �t& ������U��1�WVS��  ����L  1��Љ���t& �K��L  �������������u䋕P  ���  1ɺ   ��������   ��   ��������   �   ���������L  1ɺ�   ��   ��������t& ��ڹ�   ������������   u��  �,$�{$���  t�t& �G��u1���{$���  u�,$���R������  �����1���[^_]Ít& ��o��   ������f����������ͺ�   ��������t& ��K��   ���������9�u��놹   �   ��������   ��   �������1ɺ   ��������������&    ������U��WVS1ۃ��T$d�    �D$�Ѝx�H�@    ����@^    1�)���`����D$    �C�v �D$����  ����
��4��  ��;<$uX����	t$�L$f�DY����t`��T  ����� �X  ��  ��� �SD�1�����w��T�1����G�$��L  �L$����������i����  �t& ���L$f��D$d+    u
��1�[^_]��������&    �v �����S�Ӄ���L  d�    �T$1҉Ẉ   ������    ��I$�������  EC<�D$d+    u��1�[������������VS�Ã����   d�    �D$1�����   �   ����������$  �   �   ��  ��v �N��L  �����������u拓T  ��X  ��5��� 	���   ��������|  ����   ���|   ��P  ��ub��l  �@  ����������    ��   1������j ��L  �   �   j j V�����1҃��D$d+    ��   ����[^�1���t   t�둍v ǃl  	   �u����j ��L  �   �   j j j �����ǃl     ���E�������   ��t��H�����   �|�����v ��L  �Ẍ   �������I$������
v1�1�������t& ��    ��   ������������&    ������V��S�Ã�d�    �D$1���P  ��t,��t  ��uF1�1҉�V�D$d+    uc��1�[^Ív ��L  �Ẉ   ������$����������t���L  �Ẍ   �������I$������
w���   ��    �������v �����VS�Ã�d�    �D$��T  5��� �X  t�D$d+    ��   ��1�[^Í�    ��L  �Ẍ   �������\  ��`  ��I$������
vI	�t�ǃ\      ǃ`      ǃ@      ǃ<      ǃD  �  ǃH  @  �m����t& �4�    ��   1�1�	��O�����\  �� �  ��`  ǃ@      ǃ<      ǃD  �  t�ǃH  �  ��������������U1�WVS�Ã�d�    �D$��L  �L$���������  ��  ��&    �v �����UW��VS��d�    �D$�G�P�R�����%   =   ��  �o�   �  ���������= ����Y  ��t1�������   ������   �  ���������= ����&  ��t'��	  �����1҉�������   ��   ��   �������  ��  ��������ƅ��  j 1ɺ�   �������[��= �����   ��L  ��  ����������   ���N\���   �Fh�  ǆ|     �F8  �F  ǆ�      ǆ�      ǆ�      ǆ�      ǆ     ǆ     ������Å�u?���   ��   �$��L  j�L$�����Y�Å��$  ��  ��&    ���������D$d+    u����[^_]Ð������������������Ս�&    ��    �����UWV��S���Yd�    �D$1�����  �    �׉˅���  �k1��S��H��  9�O�1���Hи   �k9�OЉS��T  ������� �X  ��  % �  �����`�  )�j ��  ��   j )�)�kP��P�CP��j�������C�����  �������t�T$d+    �  ��[^_]Ít& ��<  �� �����  ��D  9C�O  ��T  ��L  �ʁ���� �X  �g  �����  ��  �K�   ������K��L  �   K)�������K��L  �   ��������K��L  �   ��������S�C�������L  �   ��������S�C�������L  �   ������$��N 9���   �����z0�� ����  �C��C�G�C�G�C�G1��������&    ��L  �Ẍ   ������S�k��I$���H��@  ��
�F�����    �(�����&    �v ��H  9C�������@  9C�������<  9C������a�����    �Ẍ   �������I$������
v+��L  �@  �z����v �����������������    ��    ��L  �?���������������f������V��S�˃��Id�    �D$1���to�������uE�C    �C    �C�  ��T  ��5��� �X  ��   �����   �C�  1��t& ��T$d+    ��   ��[^Í�&    f���K��t��u2��<  = ���wŋ�S�P�S�P�S�@�C1�몍�&    �v �����뙍�&    f���L  �Ẍ   �������I$������
v-�C@  1��_�����&    �v 9�s ������0�t�����    �����t& �������               �                 [                                                              �           �                                                         �                                               �       $                                                                                             @   p   �      @  �  �  �  @  F   �  �  @  �  0  �        p  `	  @
  p
    0  �  �  @    �  �             Set video std register to %d.
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
 tvp5150 include/media/v4l2-subdev.h Disabled Black screen �$Ph    ���   h    ������L$���D  RSh   ���   h    �����1����  �����UW��VS�ك��T$�l$0d�    �D$1��,$��9$~Q�4$)�;t$4Ot$41�9�}������|  �D<G��D$PVS�t$h   ���   h    �����\$P��몋D$d+    t�������[^_]�h0   ���   h    ���������  �@�Ph    ���   h    ��������  hX   ���   �����1�XZ�j  ���E   �B   D�PhH   ���   h    ���������  ��h   �Z   �a   Eх�t2���q   �~   E�j VRPh�   ���   h    ��������  �x   �׋L$$�D$��Q�T$�L$R�V�r�WR�uh�   ���   h    ������D$,�� ����  �=     �L$�   ��  Q�D$������   R���   �p�GP�G�ph�   ���   h    ������� �  �D$$��P�D$P�F�p�GP�G�ph�   ���   h    ������� ��  PhX   h�   ���   �����1����;  hX   ���   �����X1�Z�;  �D$Ph  ���   h    �������L  �   �L$����������'
  PhD  ���   h    �������L  �   �L$�����������	  Php  ���   h    �������L  �   �L$�����������	  Ph�  ���   h    �������L  �   �L$�����������	  Ph�  ���   h    �������L  �   �L$����������w	  Ph�  ���   h    �������L  �   �L$������ǃ����I	  ��L  �L$�   ������ƅ�x�t$��L  �L$�   �������x�D$WVPh   ���   h    �������L  �	   �L$�����������  Phh  ���   h    �������L  �
   �L$�����������  Ph�  ���   h    �������L  �   �L$�����������  Ph�  ���   h    �������L  �   �L$����������]  Ph�  ���   h    �������L  �   �L$����������1  Ph   ���   h    �������L  �   �L$����������  Ph4  ���   h    �������L  �   �L$������ƃ�����  ��L  �L$�   �������x�D$VPhd  ���   h    �������L  �   �L$������ƃ�����  ��L  �L$�   �������x�D$VPh�  ���   h    �������L  �   �L$����������C  Ph�  ���   h    �������L  �   �L$����������  Ph�  ���   h    �������L  �   �L$�����������  Ph  ���   h    �������L  �   �L$�����������  PhD  ���   h    �������L  �   �L$������ƃ�����  ��L  �L$�   �������x�D$VPhp  ���   h    �������L  �   �L$����������H  Ph�  ���   h    �������L  �   �L$����������  Ph�  ���   h    �������L  �   �L$�����������  Ph  ���   h    �������L  �(   �L$�����������  PhH  ���   h    �������L  �-   �L$������ƃ�����  ��L  �L$�,   �������x�D$VPhl  ���   h    �������L  �.   �L$����������M  Ph�  ���   h    �������L  �/   �L$����������!  Ph�  ���   h    �������L  �0   �L$�����������  ���   )�Rh�  ���   h    �������   ���|  ��   �Ɖ��|  VPh,  ���   h    �������   �؃�$�|  ��   �Ɖ��|  VPhL  ���   h    �������   ���|  ��   �Ɖ��|  VPht  ���   h    �������   �؃�(�|  Ph�  ���   h    �������   ���|  Ph�  ���   h    �������   �؃� �|  ��   �$���|  ��   �ŉ��|  ��   �ǉ��|  ��   �Ɖ��|  �$QUWVPh   ���   h    �������   ��   �؃� jh�   ������   ��   ��jh�   ������   ���|  Ph<  ���   h    �������   �؃� �|  Phh  ���   h    �������   ���|  Ph�  ���   h    �������   �؃� �|  Ph�  ���   h    �������   ���|  Ph�  ���   h    �������   �؃� �|  Ph  ���   h    �������   ���|  Ph@  ���   h    �������   �؃� �|  Ph�   ���   h    �������   ���|  Phl  ���   h    �������   �؃� �|  ��   �Ɖ��|  VPh�  ���   h    �������   ���|  Ph�  ���   h    �������   �؃�$�|  Ph�  ���   h    �������   ���|  Ph	  ���   h    �������   ��   �؃� jh�   ������   ��   ��jh�   �~�����   ��   ��jh�   �f�����   �   ��j
h�   �N����
  �ع�   �� jh�   �3���XZ�D$d+    ��   ��1�[^_]ËD$������D$������D$�(����D$�T����D$�����|$�����D$�����D$�B����D$�n����D$�����D$������D$������t$� ����t$�k����D$�����D$������D$�����D$�8����t$�f����D$�����D$������D$�����D$�3����t$�a����D$�����D$������D$����������D$�T$�$�L$f��f��f��v  ��f��x  �S�$��  R�S�R�T$RQPhh	  ���   �������v  ��f=PQ��   ��x  ���   f��!��   h  R�����XZ���  1�9���   iȌ   ��T  ���8  �$��X  �<  �ы$��X  ��T  �PhD	  �D$��P���������  ���   f=QQu$f��x   uhH  R�����XZ�s���f�� tPh�	  ���   ��������S���h2  R�������L  1ɺ0   �����Y[�/�����uǆT  ��� ǆX      ���  1ɺ   ǆ\      �$��j ǆ`      ǆd     ǆp     ������ 	� �  ��j h�   j jj h�   j j ������	� �  �؃�$j h�   j jj h�   j j ������	� �  �؃� j h�   j jj h�   j j ������	� �  �؃� j j j jj jj�j�������	� �  �؃� j h���j jj h���j h���������  �ع	� �� h  j j j j������^l��  ���څ�t�$�T$������\$��  ��L  �L$��   �������x�D$���@  ����
��   ���  ��H  1�ǆ@      ��P  ��ǆ<      ǆD  �  �,  ���  ��t&V1ɉ�h[  h   h   ����������R�������������A����=    ~����  1҉�������������1҉��������  ��    % �  ��҃�`���  �9����q�q�q�qhh   h�	  ���   h    ������� �B     W  �  �  c  �	P  c  �	������    �    ������    �����           �              �                                              @                �     tvp5150_get_vbi tvp5150_set_selection   tvp5150                                                                                                                                         @           �  @  
                                                (   (   ,   0   �   �   �   �   �   �   �   �   p
         �                                  �  �      @             �  �  @   p       @  @                                                  �  0  �  �      @
          �                        @                                                                          �                                          �              0                      @              `	                                                                                                                                                                                                      0               ����'. +�r                                                                                                                                                                                        �               �*�?Qni�	   '    @           [U�� qnB��   :                                                                                                             �               ������+��   ` o GT' ��                               ` 	�
� �G     �     ( ./� � � �Ȁ� � �N� �� � ���debug parm=debug:Debug level (0-2) parmtype=debug:int license=GPL v2 author=Mauro Carvalho Chehab description=Texas Instruments TVP5150A/TVP5150AM1/TVP5151 video decoder driver              ��      GCC: (GNU) 11.2.0           GNU  �       �                                  ��                                                                  1   @   &     H   p   F     `   �   ;     u      9     �   @  9     �   �  A                                	              
 �   �  �     y           �       %    
 �   �  �     �   @  V     �   %   !    
   F   �    
   �       (  �  �     A  �  �    Q  �       
 o  @  7     �  �  �     �  �   $    
 �  0  g     �  �  W    �  "      
 �          �  @  �       ;  -    
      E     $  p  �     3  h  Q    
 G  `	  �     V  @
  '     h  p
  �    {        �  �  �    
 �  0  �    �  �  X     �  �      �  �  9    
 �  X          �  _       @  �    $  �       8      X     B    �     S  �       d  �  6     w  �  X   
 �     �    �  �   �     �  �        �  �       �         �  (  �   
 �                      �    2  �  .    
 M  h        X    I                  n           �      �     �      
                   �  �   \     �  �   0     �  $       �  @  P     �  �  0     �  �  ,       @  P     #          3  �  @     C           X          q                    �  0        �  ?        �  \   O     �             �             �                                       +             8             D             V             }             �             �             �             �                          !             9             B             [             o             �             �             �             �             �                                        1             J             Q             `             s             �             �             �             �             �             �             �             	             -	             H	             Z	             p	             ~	             �	           �	             �	      
     �	  �   0     �	             �	              tvp5150.c tvp5150_g_std tvp5150_get_mbus_config tvp5150_enum_mbus_code tvp5150_enum_frame_size tvp5150_volatile_reg tvp5150_runtime_resume tvp5150_runtime_suspend tvp5150_read tvp5150_set_std tvp5150_set_std.cold tvp5150_s_std tvp5150_fill_fmt tvp5150_fill_fmt.cold dump_reg_range tvp5150_close tvp5150_g_sliced_vbi_cap vbi_ram_default tvp5150_g_sliced_vbi_cap.cold tvp5150_subscribe_event tvp5150_remove tvp5150_remove.cold tvp5150_s_raw_fmt tvp5150_s_sliced_fmt tvp5150_s_sliced_fmt.cold tvp5150_isr tvp5150_ev_fmt tvp5150_isr.cold tvp5150_open tvp5150_selmux tvp5150_selmux.cold tvp5150_s_ctrl tvp5150_s_routing tvp5150_registered tvp5150_link_setup tvp5150_link_setup.cold tvp5150_reset tvp5150_init_default tvp5150_g_sliced_fmt tvp5150_g_sliced_fmt.cold __func__.2 tvp5150_g_tuner tvp5150_s_stream tvp5150_init_enable CSWTCH.90 tvp5150_querystd tvp5150_init_cfg tvp5150_log_status tvp5150_log_status.cold tvp5150_probe tvp5150_config tvp5150_ops tvp5150_internal_ops tvp5150_sd_media_ops tvp5150_probe.cold tvp5150_ctrl_ops tvp5150_test_patterns tvp5150_set_selection tvp5150_set_selection.cold __func__.1 tvp5150_get_selection tvp5150_driver_init tvp5150_driver tvp5150_driver_exit tvp5150_pm_ops tvp5150_id tvp5150_readable_table tvp5150_readable_ranges tvp5150_core_ops tvp5150_tuner_ops tvp5150_video_ops tvp5150_vbi_ops tvp5150_pad_ops __UNIQUE_ID_debug283 __UNIQUE_ID_debugtype282 __param_debug __param_str_debug __UNIQUE_ID_license281 __UNIQUE_ID_author280 __UNIQUE_ID_description279 __fentry__ regmap_update_bits_base __stack_chk_guard regmap_read __stack_chk_fail regmap_write _dev_printk __pm_runtime_idle v4l2_src_change_event_subdev_subscribe v4l2_ctrl_subdev_subscribe_event v4l2_fwnode_connector_free media_device_unregister_entity v4l2_async_unregister_subdev v4l2_ctrl_handler_free __pm_runtime_disable __pm_runtime_set_status _dev_err v4l2_subdev_notify_event __pm_runtime_resume media_entity_pads_init media_device_register_entity media_create_pad_link media_entity_remote_pad media_entity_find_link v4l2_ctrl_handler_setup __x86_indirect_thunk_edx devm_gpiod_get_optional gpiod_set_value_cansleep msleep __const_udelay usleep_range_state devm_kmalloc __devm_regmap_init_i2c v4l2_i2c_subdev_init regmap_bulk_read _dev_info v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_new_std_menu_items devm_request_threaded_irq v4l2_async_register_subdev pm_runtime_enable v4l_bound_align_image __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tvp5150_id_device_table v4l2_event_subdev_unsubscribe param_ops_int     Z  !   Z  A   Z  q   Z  �   Z    Z  1  [  A  Z  q  [  �  Z  �  \  �  ]  �  \  �  ^  �  Z  <    T  _  �  Z  A  Z  {    �  Z  �  a  �  Z  �    �  I    I  4  I  A  Z  �  Z  �    �  d  �  e  �  f  	  g    h  !  i  1  Z  d  _  y  _  �  _  �  Z    I    I  -  I  U  _  h  _  �  _  �  _  �  _  �  _    Z    \  9  ]  O  ]  a  \  �  ]  �  _  �  _  �    �  I  �  k    [    ^  !  Z  4  l  q  Z  �    �  _  �  _  	  [  a	  Z  �	  _  �	  _  
  _  1
  _  A
  Z  q
  Z  �
  m    n  )  o  �  o  �  e    Z  }    �  p  �  q  �  p  �  q  �       q  1  Z  >  I  h  _  �  _  �  _  �  _  �  _  �  _  �  I  �  I    I  %  r  P  _  _  _  w  _  �  _  �  _  �  _  �  Z  �  \  *  I  �  ]  �  \  �  ^  �  Z  �  \    ]  -  \  ;  ^  A  Z  T  \  n  l  �  I  �  _  �  I  �  k    a  .  [  >  \  �  [  �  ]  �  I  �  I    ^    Z     \  O  \  n  ]  �  ]  �  I  �  I  �  ^  �  Z  �  \  �  \    ]  �  I  �  I  �  ^  �  Z  �  \    ]  !  Z  0  \  B  s  _    f  t  ~  u  �  v  �    �  t  �  w  �  u  �  x  �  y  �    �  z    I    {  6  I  G  I  �  m  �  |  �  \  �  ^    Z  #  \  6    �  �  �  \  X  _  p  _  �  _  �  _  �  _  �  _  6  ]  ]  I  �  ]  �  I    ^    Z  #  \  �  \  �  ]  C  I  U  ^  D    �    �    T  b  b  c  �    �    �    �    "    '        f    �            �    �    B                   \   I  h   I  �     �   I                                                 $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     �     �     �     �   E               `  (     3     8   `  G   Z  b   \  �     �     �     �   `  �   \  �   ^  �     �     �   `            `  #    .  j  >    C    L    W    \  `  p    u    �    �    �    �    �  `  �    �    �    �  `      ?    J    O  `  {    �    �  `  �  I  �    �  j  �    �  j  �    �    �  `  �  ]            `  /  ]  @    K    P  `  d  ]  u    �    �  `  �  ]  �    �    �  `  �  ]  �    �    �  `    ]  $  ]  B  ]  R    ]    b  `  v  ]  �    �    �  `  �  ]  �    �    �  `  �  ]  �    �      `    ]  &    1    6  `  J  ]  [    f    k  `    ]  �    �    �  `  �  ]  �  ]  �    �    �  `    ]  )  ]  8    C    H  `  \  ]  m    x    }  `  �  ]  �    �    �  `  �  ]  �    �    �  `  �  ]            `  0  ]  Q  ]  `    k    p  `  �  ]  �    �    �  `  �  ]  �    �    �  `  �  ]  �    
      `  #  ]  4    ?    D  `  X  ]  y  ]  �    �    �  `  �  ]  �    �    �  `  �  ]  �    �    	  `  	  ]  1	    <	    A	  `  M	    [	    b	    m	    r	  `  �	    �	    �	    �	    �	  `  �	    �	    �	    �	    �	  `  �	    �	    �	    �	  `  
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
    �
  `  �
    �
    �
    �
    �
    �
  `  �
    �
    �
    �
  `  	              `  .    4    ?    D  `  P    V    a    f  `  u    {    �    �  `  �    �    �    �  `  �    �    �    �  `  �    �    �    �  `              #    (  `  4    :    E    J  `  Y    _    j    o  `  {    �    �    �  `  �    �    �    �    �      \  $  ^  p    {  }  �    �  }        j  4    :  }  N    Y  }  f    l  }  ~  _  �  ~  �  I        I  /    9  I  Y    c  I  }    �  I  �    �  I  �  I  �  �  �  g    ]  [    o    y    ~  �  �  �  �    �    �  i  �  �  �  a  �  I  �  I  �    
      `  !     B     �         7    d    �    �        W    �    �    �        �    �                                           Z          �     �          �                   $  I  �    �    �    �  I  �  I  �  I  �  I  �  I  �    �    �    �    �    �    �        ,    0    4    8    @    H    L    X    h    �    �    �          �               I     �     �        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rodata.str1.1 .rel.text.unlikely .rel.smp_locks .rel__bug_table .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                      @   Y                    	   @       8U  �              )             �  4                  %   	   @       �[  8               /             �                    8             �  �                  4   	   @       0\                 E      2       d  �	                T      2       E'  �                g             �(                   c   	   @       P]  
     
         z             �:                    v   	   @       Xg                 �              ;                    �   	   @       hg                  �             ;                    �   	   @       �g                  �             ,;  
                  �   	   @       �g                 �             @;                    �   	   @       �g  (              �             ^A  �                  �             B                    �   	   @       �h                  �      0        B                   �              3B                     �             4B  (                                \B  �     Z         	              ,K  
                                i                                                                                                                                                                                                                                                                                                                            ���2�gJʍU�z�/UK\S��#H�&B-�������V���Xӂ�m� >�@\5	{@�Jc��bݝ'1P��`*���d�ܠ�OG�j��Z�),�UC�V��ݨ��W%ק 5k����#!T��Pտ�� k�@AlAa��2<< ��L���Bc%�ip+vj��BFЪ�C3�@eg�~)�5��w5p�44��`9�T�|	�j�LU����!VI��т��q��r9�A��XH�?f����:���Յ"e@M-N�jz1{�&g�;��k.�� !˺.��+��p	��ڙ���V]�,� 4����27�4Z~��&� �كR2χ����i,g�A�vIں�"	���i狵'��m��5� �ѥ��'x�|I���$���7�i}K��\�Q�Fd�d&&�iOG��3�vK�%[��iou���f��z��x:J�*p������"\"�z�
뼩�y�Q�_���ǙhS�h����L������<ma�
�@6���4����jX[=�}����V�X^�N�|����摫V���nH^����	�Yjq>��i����ʃg ��F^P˺n���:�;����D������N���}۠�q�[�MƋ�ۂ��h����Ya�
�`���x�KJ|O^ЦM�8���T(7��Ƅ�(�*��D��3
l�D,�\]Ł���d��2b��TG{�[��S�U)V�?Z�	�6_kΧ����2H�C�d���Ӂ�</�8ㆮ ���$'����I���������ې�~n����==/�D�M�]���
�D�5�"XØ>U������%��%�D�����I}���WB�X��/a�5F�;[��_���d���4����2��殠��i �/�]��U���`��0:�6 �K.����D��K�u h�j<w	�o����97��ל�ok�(h`\��-����aK�t�[�7�_��?0���k8Ӊ�y�$�̵�P�\�u��8���O�N�©��?�M���-� �0�6�n�O�N�"ܹr�i�@Og�)�8��94�Gcf��/��*��hĦ�F�����zɑ�2F���2bg�`����Յ���k��$�x���Z��A��h�2�#�N͸Å�%����x�`s��l�zZ�i�i*К@��cg����F�(Wg�emRt4���>kF�Pa�t� �T��uɍH�85nz��a�ς{z�t�"
�+�������X��êpF[��i٪(��g!i�O�h�wi����sv�h̠�/�`���jT��?,��Z���@�������F���*6I���t���An轙�wk�VX
Ϸ��`)i�5w�0�V�ݲ�"Wy���~Y�_!ζ<CL���#�М�#��)ٜi䞴�gT˷{�� m=Ps�A<ݡ��񌏤�[��=��k��ˠ�a�#
C�*F�#����$qH�A&��pm���p;��b�w�2�o��T�в'�ӷG�d���ӵ|���{�����_���~Om~-VPt�n��[^id��q����T�٪��~�U�+a5�1�ޮ8�;�G�<�QKhݿ�G�Ĉ���v�;�sr?m�+[s��R��(��we��ܛC��,J^�Fg{.�<#�h(.��=DJ�h`�O>vCœE@hrz(�ig:�_!FF��k�
�j;�!��i3v7W6�S�v3n�;�/ 8��#W����:y+ђ|"����u��E+' �ιbK���p�C��-g��������� P�.��і�x3���b���t�w�����XB7[Y���G��zY�:'�N�k']-��W 1�ǵMۤ�uk�5��<y��}��*�>@'7(�����ވ(Y�_�Jz:�a���rEld�!P�0賤v�j S�nhR^ӻ���.Q� Px
�f�����<tZ�|-�ܿ���_邃���J��<Q��{J���<����j�X�+sg�ꆓs�,���-Cn+�f�:��vE2�95K��B��#�V�y���ո$l�0�� "��A���?�D1����~ҝ>eP��]ԧ9��%$xV㞃[��±�²�M쿷��z���?v�1��CLC��}��q��0��i�ap�l���Vu���-�6�@,+�eɮ9�/���D9��Z�b�T�#�_ך������W�Qv��q���W�jXzH�g	�w_�zA���݄���C��y?�'��p��N�e2%ݮX����]@AS������C�}֚Niϳ��GӰS֭��D�;��Dُ[�:55x��cqT|<E��v�9 }x�gG{J����)�F���zǽ۬�X� ��`.�����澙#�٭��@'�ղR ��b��=}#f��Y�M������|I�{e ��L9��j���*	7`U*�m�v���sSU�����#�|��o��Hy1�3��k�o�� RA�a$���n
��ꔼՈ�?jh����қk�a�P��������>��S?!̣��k�?moOcB�xL���d�����D2����ܫ·s}�����P�Q�Cr�)�&��Ei�"����.	9�<�K��[����,�+�/�HFYT�{����T��O�ȟ�]���5Dj�E��騃)1���T�Bm4�xq�W�c�a��~OI�U�Lt�1[��k���W��z�j���ÊD�{��0�שh �J�T�t��X.	�KNO�l��i�0��)�m������{,Gk��_(� G��śކ����^��9]����u\]醢��*j3�m�c�~<Z�4�]/L������0=�W�O�OY��?~�,��Bgw]R�����X\-��lp1���#��'좎���g�	�?�#�<��kƃ��ˇՎ�\�{�4y��w)Պ(�z�u�fp���p��������8����YG���ʅF�u�����U�р�cn�W����h�:��5��	T�OD0��/Ӱ:i�2��S2B��&�V�T!P����G�t�ʵƖA�7��c��4�^ ̰K���^ֈ��8.��rz�Ц�L;f��q��y�]hW� �)��H���>��q`�]��b�@Un�-��?r���C}Pq}�)F���`L�v�����ifSڧN�'����rHh1�
8E�UMh�hh�ۜhI��I��Zh��9��H_��S�� �ĿC�be܊N|%�YJ� �=R�>��nq���#�A����%���yy:T����5ʷ��:���B�R����ފ�?F��C��:O���,���R�Y���΀EкU.߱�A��JY6�uU�x��c�۳r)!�Zwq��0�D�O�8��V�������O���4(	�e�c��5��	���L��V�`��h7�@KB}ۧ�p�A?��F}�?�aJ�q�G\�]�xl3��ˊ��|GiQQc�C
��R�����{�9h�	�	���0KV����Y�z��M��\�;MB8H����Q[f��'�]��=�s���G��p��{(�խ,�b�����H� K8����>.�;��L/��6��X�G��������N�aSrѬ���Os_�#�4�:u�y}�����5?0�pD�GHV;��qH�։�E�Ou�����f�s�ʁ7G�e�F�|�{�}�Ut5&污:zC����5�Z�^��*�F/N>�5tl��$��!�#H.�*n�1��F B6�7�4�<�.߱:+��FL��7@��f���<�ȹ�	?��)�Ϻ��C)u�0f���N�UT�Q"��I�;:
����G���1��	� ��@�W�xѺt�j�\���?4M�v�?<����cZ�<C��q��%�~7��!/�)�Rf���@�P����H�{���b��S	�����c�ث2����  b^zw�^�e�)p��HF�*�*���Ι|}�4=�b��g��*[�kw��g����r$�=�d;R0Z�����Ï9��B�񥌐��Wj�J�K�?�8�`kwLt*v!I�fC����)��Gh�?�sd6[�.�&Δ��G��@x���䘸g䀝;������p����J��wAP�8���?���A���훅��l�*�1��Hk��o�J�I:�8��&�� M��-�7�c� ��Ӻ�z����J0%�X�a��7TSC4��*p�4�:���ľ��]��3p7��qS;�ǁ�)U�N9���*2c�I�FږF�즥��/����ڟ��V�������kLZ=��aNcŅ<1� �ohr,턷Y�j�t�_�R�IΥɻA������tLm��k$J��G�+�ܬܤ�a_��S�pl��qd���bGd4b���(:]o�&>���F� h����)�j �M�{�{����R+ԝ�]�@l虉ƞ�c�J��#�ws˙�[o� 4E����Y3���a�&pK�8
\ݏ.����O��/�:L.���<����-v6m��{�Wܜ���Kd$�8/f3$|��Dɧҧ��Dl�s�ZG��4��<�EU`ɻ�b)"L$�a�[I_
$T."�QeZt��Ȱ�$�~vλ��S�Ɖ�l�Z��&X~��i�������	�.[چ)�\��v���ѱ��H�%�|7�`��#�3����^"��}U��z^K��b)���f��K�.����&3/T�R[���B�Fs�CD'^W��H.�����P���1�e7\h�<ۭg���O�2A ��.A2{h؜Аd}�>Dn����;t��m�����pBm�ن&#�J�p�h���t�S��\y?R�{(�&sUzz�S�?1��x����㤲�?c�a#v���y�,��\�v��Z��9׫�+�ҩj�����[N?x~��S����i��)*��"�z����J3
���ބk�Z��Z�_��&�UY\�4��*�*��A��-��|��>R�"��)���?�[�A}�
�E��/�8��=��࿒�m����kC��y�H�A<h���,#�jYgW�ٴY�:δ���
O�w�8R
#X�Q뽥#����X�K��Y�����q�����Ee�q��Ƌ���K�M�P cFd�>Dć:h���!��D�n�u	}�D(t�5"e��nMH���*FaX=�)Q�Y�����yY��J�2�#G���&�7�!��#+W��\I��?rv����O�_�������L'j���,����I{�X'V�2k�z��w��>;�8����y���5���c)4�?$�6����Q.&�� �Y/ɷ'ȣ���u������1Y��-�|W�jTd`v��_UҒ��>)ZN#�w�%�'I`�cR.9��&�9�z	5N�V�HA��E����o���� ���vۻ
j�Y����o�Ȓ6s{��<b���ǖd6-�\.ZKHxh�N��_:��Tq�~�2�{�o�l9�qD�V��Z�1�������\ȫ&y6KUi1�Fk�zk+O�?I���x��E�J�5s�f�x����7�=�ӊPZH�S�	�c��ao�\5!߻�m2�[=WU�P�Z��#l���te����;�#Y�Ǳ�&�<�.x��E��}��=b�"����>vT	��#���Eާ�g d�U� �K4Yo��<ƀ����՗>����o�ex"�X�m ]��(�s�1(r��ŗr�\��#���(�}t��GÂ�#��f�k`��EVi<�r�a�9\[�����CV�L�ڋS! ���c���+ �����#4�=�o��7�Yй�k�᥌ɋ/(��*�i0���s(�t��x(�g��:2H�Sk���(^���ĎWbK�.��p�.��5&\w`V�&��7�̍ԟ�l�m��������K��m:�$�p(9>���{���\�Rm�"��k���@�k��u��r�l����i7�r��u�P2�#g��B��p�A\^(xQyZ���#��v��R#����&C;�_*���ՙ�+�!bf+u�����l�'\D��n
:���>����O�ZƎ]p���V�ba��?�<�Ϝ��
��4�}�s<�[[�I��[^j���栓ظb�+j����E*gA���W�Z�-���ǁ��ɸk���c]v��6���r�w�*}$ZzO���@�qv�-���~��ν�ǆ��h���dp�4��'��*�}�o��a��n&�N?�h�M_�����zƕ�����M,2־^a@���{E����(��F�\��(Euޞ���c�ɳ�����o`�RP�g���IA��]��ٞ��ɕ.$��x��U_��$T5zt�a9:�N��I:�.x�ڼ`��>���~����y�+�/�@��a���:��!.�C�	�PՃ���cC=�τű8����5���۳��rd�&Q'�8nz#M�����TV+L�����U�x�n�Xo%:�󵣤W�:d�x
�'���<%f�"�.�z^JC�WT�z��ƽ����f�����V��Ui�+��1pŕi^�4��: :D�[rڢ���q���|��9�5���tc2U�{k+��e쭃A��$��=�K7h�D�uI��"��+n^��RZ�>ok[u�"R���z[�p������b���e ���1^Y��AZ�捼Y�R��"IY'�)�����1�(�ש�T������|<C@�@�84�W��!bixW�!�6ʗ��YV��#p�eR�UK`�8*H<Q ��2
��4(����Ż��-���z�PP�a��f2�Y�9�!pWT{�^�0��#y���~�gL�pL��f����@%b�l���K5�s�v����0��(���RGQD��]<�;Ҧ����{:r��%Tj��q�޳5����UG�9zdnAK�P��4D��,�CƑ9��jd�㷜�B�:e-V������A���g��f���*Z_��	xj���r-/Y�[I��$�8���
��y>��w��q�>41U<���T�2amN�2��C0w_>�z��p]�9�	����{Y@�j��h@�#�	�m%��po�ߤGU�
&��.C��BBb��^\�/�t4'�v�3
��F2ժtĹ�o�t[Vz=�T|ͫ��.�w���'وO�d��K%��G=n#Hv Z���r�Iz��p��<��"�(�4����2���P\4k$����û�D|�23�ǎ`�L���h�Z�=$�̘���EVW�5�On�C>5��!�d������bҒ+��8T��}�՚p��j0� �DM'����e�V��^(Z����*�~I�u��+�Vy1}Hsߵ7Q�����3�B������/�$�hT ����I/s�4Ix����g��>KcrG����;�f�a\|F���<�?��jු�z#�H�2[�*ű;��N���h��z%V��L] i�fg�����!��t�nC�����sQ�q�l��� ?���qȒ����X�$�)i�P8.{$w
O-נ��;J�4&�6!����l��%4��h������A-P��@�K(	dy�gP�T�%X-^�Z�D��\:8(�I��%D���P�/,�5Ϳ����g�80���`x����? טcEF�+��D�h��w�RQ��s&	�Pm�I���c�;�
��� �#Nа��1��R��e,%4=����v+�\�i{�����W��=a��f�?�T�ߙv�Sg�����k�����o-��z��B�K^�W�rr/`������ rc��S�:�nI���6U��"��C��g/p2�72|��,��Ɂ�r�!�0�l��py`6�B>��Te�B���]F�ыpC��]1���;����%�H?m(,�R��Ҙ�tw��a*0�K��$U,�T�ϡ�g����XA:�Fy,��J�aG�����������>Q�;P�L���A������^��8׽�l��T|�r�۔\��6��s<��r�q�"���opN�b���~�u�r1��ad�u%�!v���/4dU�c�K�0�k�$C���OF��e�|.���W��2EY���
�}�=�Yg俠���:����-ؙb��L��:Ρ�ZvyS,~8.���7_��p�b������K˿ћ2L����" )��r�d���0@à�mY0��_U?M�� .�l�u:pl!�ˢ��<+�wd����A,ܸk�*���У����|����_���SؗՏ���I��	Li�����+|� E�wǢ�<����^�}m��,�G���L�C���;R��F���iN#�P��ܫ��M��|��o3L�&������D��Y�f%&`QŤ,�P'[ޖ/8Q����W[�r&M5}2�6�p�eҖ� ���m�"��У:�ihT��˭�_5�Aލ���#a��>􉤬�r�R�_L	�o����Y���D�P����<v�Mu�y�a��d�����)%h�̱�}v5�7)���)@�Sx�P��U|֦�;���Y��V�dY|�Y�Iʔ=�k;���x,�?6~�^bul ��hQKT�{�%�~�v��O��p�f:-���2rW>Y���FԆ��� A��~ג?tSR��h�ٻ�U�U�fQ�\7�PW��Kk6t\*�r�����<.���!wn�᪣M>Z���@�t,X�f[�Ca���ù�/N� � ��Kժפni�AA�@s�����)]�7���Ar��y�XN�t`7�^커ݻg�A �U(ċĹ���+Vjӡ�9l�⓺ӄ�V�aD3;U�;زGĶG�w2��
���G����V��P+���l�rl�؏Kh]��D�Ng|WRe�v%T��S*����v��Nt.57&#^�L<N� 7$��ݻD8�/cO����d��P7���q��e��%��T������Ъ�#୨� ��K�e$Jshv�$�+�_Q�
>�������&��+7\�fK��3F\VZ��V���A��2k<l�Ҍ-�眧R^���׋��j"���l�����������P%OJ��R�oA��ͺ|j*^t�k�.z��oB��6����/�?�gp�8�^n)��~]?f���=4��8]�P,S#X,^����ӎ�%�/��^�ҧ�9�+̀|@�(VG�^k>s��y� �)�$��a�D4�l|�q�6��W[��.OT5ʏ�\��U{��W@���BZ룈�d�_l񄪪�'�a��0�~���}��c�=�uP���K��6�*�y��s���	�?�Y�Q������L�=6s}臚��d�QK�I�C�X�o���6�t)&�܏``��Owx�{�Z~�
��SV�Nj*�{53���R�O/y�e���˓�8&q�I+��"�.��U�m�챘��X$� S
5�y��nG���J��	�C����\|@�#z9�՚7W�4�;��}��h�t�ٔNE,uT��d+�|?��
L�^=&��͠:�-�$�|'��q٬�^�����圜��eaf���UD���ɋ����� ����ۼ�R�w��} y�N\�(9He���$K��XS�\0/��K���.�=�vQ�X��.ttAM�Y����&rz�&h�^8��O?�(VR�L�B4�B\��s�{31ڀ�U����
�ޟgS(�9���1Ȏw�&T?��P)�h=e����aA�f9(*D��u�9��N�@��b޵�����. �3��D����wo�y6}8��/��7B���B~g�ż��%k�H�(��h�W�O�T>2�i�����o�:�޸��i��"-��2�wV_Vd)�5tj�$����λ�J��c�q{�Y洶��r�f\�'���X ϼ�H�&d�`bG��U�'�/�(;$�(�;Ei�H<׆��fϫ;�P��
e����n�Nȕ�nJ���A{�ꐎ����[�J����eg.3X�+�|�Ik�D�^��y���q!��#.?iA�E��W2�)��s��<�=V��g6��ļ	H�ep�F$Y�@��j2z�b����!���w�;�{�_iT�k߄w������]J;%Fӕ����M���p�K�Yx+�6�|5
d��6UX#ǋQ�S�L�-?���:��Д�L��0����Awn�Q8?��б�=PH���ŕ��5��H�k�C�2�CĘ�-��ا�$?���3�΂d_�l��V��%���&ViU�{_�,P�$�fR0�n�QP��'N��Y�9_ ��8�����P���%����@�ێ���ˎ
B%����'�%ɟ���Qp���J�.x� �Q���-�Brk����p�X�ik���oAx�Em:��X�Tu�#�9o�Ƹ�����XY�Qᴠ�ch��d��M��.]����󀃪s�,WMd���W��`��LoH|�A6Rno���`�z�w�G`��#N'�x��֛{k��Eݏ��g�Q��ݦ����Y�d)Ӆ��f�
�^ da���ۄwQ�����m?*��:Hg�b9�|"�P{���0����8�gr���w�|1��k��Ze�l�Gpk��*�tR`�	Z5�y�E	�ۇ�3��yk�w�Z��<^{^���G$J��[��U�I��2�\g)���Ȇ×��+/r9)��R ���3p��{��_��Ge,�XHMY��g����U����!�܌-I�����e�H a\vm�� #��sHax(p��K�_��ww�T��UVe&�i��[ws��l��*�&�ּ�;!a�r������5��N���.���ݻ���H�&������y����a�E�X�A�����E߼�
l5��"�8�ĉ�i�$���IC`z�:����+?�M"�S*�"�Xz�2 �O�^�=
����?�D��IAa�ƣ���?V!���
z���bAAsw��S%�	v�N���Ip%(y��ʃ�`�р8%��Ç'd�AaI�XL>f-#�)�~�eU���Z��BZ�i�We�������a�z� ���Sh3��pHzь�-;����}�yK�&��W����f�_��^�cЙ�O���dw��* ��+d���Y�+F����!���2��Ȃ����W�6 �����Pm�f��X��wڃ��B��
/���Ӌ�"��~J�kض��u��'rI�����:m��X�)X�b�O3�7��ʳ38�	���>)��5Yb �����$�������ME����)�rW�(��p��md���}樇ǹ0��wt~N���2���p�iQY�;�J����a�F����U��h%h� �O_�=��v��,�	X���1�0�N2��h�jQ�1��C���t9$df�WS�m�:�\6��мɾ�rU�%*d��y�E[��ˬ��y�hʶ��i�����&�h����0���z�k�]O�Ra�R�8��/և��nN��b ����9
£��t����8��	�����q�ĩʇ���-�gZ?�߄Þ������F�O��9��\T�V�fXϏ�0�{�3{
v�Sc���_S���S�RoL>�ˬ����t�|����5����G"S<�l���'H/0���v�*J�#�T���7��qf��Xa�� �}W���z����d4��4@�0;е_��������4�e^A��^��r��
so�A������w��ӜK����i�6E�lz�*1�r�Z���y����k�?���Г�i�,�P��e=��|&��o>͆��f�~��Rb���~o*�ͬd���&�R�B5�Q�r)^�W�-�n2�1DG�۪���:T�Շ�҆LL��qS�V���"h�|W�C� �y �
�'w�K�A�i�M`���Pn@"A-B��H�G�2f���*|���`I~[��,>�{\�;R�����z,�s�(��o;P���z#� 3��� �v_M���'�'S�����m(�C�1r-��]k"S��&�[��ބ�i[�^Z�A�q����?^#-�u�U���������=b�\����;Zյ6g�%9�CC�Ǝ򇷆Fr�M���!�s��5����?�E��k�
��(b%�U���SPO^��ŮF>�ӣf�o+	FC�8��Y��H��FU�6�� ��2줍�g�ae�*��B�d��;�0&���� #�ߟ����"���wtp5o�֚��]�X�{�~�/Wg9���HH==���� �KW��b�1�� 6y"X�8�c����Y�E���3��]�N�?j$�@7+sdaFy.�����#Y\��J<o�́�D�=
*2_ؑ˃y�l���E��d r��f��it0�5s/H���HC5j�t�j� ����ՕS��ۇ�m��["�A����w��PT$��|�u�ыB->����c6�ϰq%�U�N�����>����n>�FR�;�n�{7��u���A��������	�+�K���{�ao,��(`��q�Ù�)�}�{�X�ﺛ3IZ?�ݍ�4&�5��E�Q�t���Z� �J���.�z<K��"�FD����Urń^=?A.��Ů�+�5��~&��e���� ÖD�c�����E ���|.Y��J�C4��݆W�GN#�g�����O%�s�õ�y�Q5�%�Q�_zI*q�*wD���zz�8��2�7K��"����3��$�G�%��]n�[��wޠ	!a�������a�k�}��:��/ʕ��&�Y�;���o��N�³Bf�>�PW�Z�J���u4�'T;���h����`e���,���(Be�H��p�CR�ߵ56`�+��w������ޚK𸙀�*�$��N⶷2da_Tѹ�Q�9,)���U��o!c��B_����dF�l3���e� M�>+k$p����W��yj�sک{s���e���0��Pd=�"e�������B�.H7d���9̣��96���J��O��Ш(�.�X#��GN�����EI^��	��%`y���ŃZ��1�h���t�m���6\�c�ဥZ���"�W���WM?M�I�����O��P<^��;�	����T�P62x�i�"5#�( $���^t2���=Pvc̘�ůJ6QL�Od/�-r�\A�h�>G���pE �7u{�����y��Rx��U���E��������Y+`A�voW��H��h�{R�|��v���{g�oI���K�TD��F�OG�>}[�ㄛ�ٖ0�����{m�]�>v���Fi�+a�^��l���B �j�w����\3D�QÝ�5t��k�QE�����~v~I6Q�ܫ8��=��j6\��=5�ٳ��6�v����G�^���~cg���p��o<����f�TM<��Y�)�wR�l㨌v�����7���I&��p }��f�>�B�iF��D� ����!b(��	��[L��V���~"�M�WX�3�g6.�0��q�}�w�ykq2�h��$*���v�雝�bϚ�*��jE���׳��؛S����
����#�Qql���X	[�+�'��bm�qDoM[�T?/|k��B,�.4,󰖇7e��/n�����Ę�.^>M�LdX��yYFG� �6����!|t�$��0?���NQn��i"23�k2r�L��|�Ճ�U�:pM5��`ڳ����d�?[�!ZyO�Ǝ�w���k,R�ג��X�D��N��U,5�5�*���_��W��p�y�O�o|7�>tz_�嘦�$��W���T
�rKQ-V_������i��K(3�]ȍ'��ie�T�q�1�JO8���w�6��`�n����ïq"r�A�w��i,��J5�w�n�f�S��0�VO�z�����x��� u�1^���S�+�J�a�KDэc�{��]�Ȃ1���枮��͠{�*��,h����v�؅˲�tT  ���%�S0��>�@E��mb���X���\>,0f�v]?i~f��Ǭ�Ͻ2�A�G{ԝ��9�	��l&����εu�̈́��(�u�fg�cFg.�@�v�^��2);�t�e�����Z�	�b�)ҖC�Ru���	���.wu	':�B|^ϡ�6��y���UsƇ�t���r�v��jׇ��	�F� LH�t
���)�6���g��ˁ�(�~�uR���ٝf��+��]깐J2�݈�?�kb&0�������W�b�'K�9-Aa�U?''ھ!�!�I
�Y�����P]^V��С�9g���J��D�]t0��K��X���{�І	�u~:U��xV�$�+Yw�\�*|���z��=���dކpˁ`*D��!r+��/�F��:)n��W�:w�k!���-��P3��?{�ˇEE�\��wcA)^��i�T�W�<U\�V�t0*d���"��.��-�|�^���b�d����vE�0Gr��m��E �۱2��H8B��kz�We��/����t=Gw���5hW��?K�}zק��~O��HP�մ��V��>L�h����!k�R�Ƙ�����f=�"�m�#�w
y
���V/�M��S�%F�QO��A�ҿ��p�۹p��VC��,�������4]��
��.�s��� =�{���<��s�K�`m��0�%n�o�q���Z�`�ډ�k����3Bt������]Ӂ�O�1h�-9�c�-Q��P�7���Wօ��/��Vp��q>A��?S+� �}#�*9럩�%/������i�_Ԛ�s@
��Ԅ��.�7-�Q���~v���y�N.x��N	�k^T�?9�s0:\=;��������,����8C5>�; ��?�80^�P����\���s�"]�0�a݂�+@�; Lf0u�<��ǚ������[3��5^��W��!��q蕾%�Z���B�l�*J6 ��cѢ?�ߟ�@-V5���B���~Z[g���v��T�eq&Jw+��F{�4��ѵ��p��`縞�1�	Z>�>��g�~�'% ��Y��s���`�^ޖ��,еv����݋��ad�O>�A@y)i�>S�A��=-؆��������<���~q�tZF�b� �5��<�8p_c�'��ż�)�KF�@��9cV��p�-Xy_�;>��R���J͹.����"���\;���sv8J����~4�Gb/�vN��4=�ů;2\��1.Yׅ�����M��J6�އ�HYt�#ݢpB��m��:�}gt������+�|����˞q� *6GO��f�첺����T��������Q��X�+�����Z����Z)�^Ŭ�AKˮn�ޕ�	�����׆���\vu=D��6o��]r�����3�?���;���%^�3\,ng�P��̎�)1�E>���a�²� �y�6�ыb�����,���UJ�˄=!��;o���<pI�F)�B$���'	����
h��oW>'S�\�K�$�+��w�sj�,�p���QX����JH��"c�/�t����\���<�z�G���������������`����`���x�cE��4x�ۓJ��j\�յ��Hk�ָ"� =G���+��&L[��Peֶ��x6��R�d-p���Id��VF hM\4����C����G�q�S��A���	�'��]�ŗ�0���P�%C�-�d-�<���ż�n�-�����R$�歄.�	
j��\�V�}7�|��D7�X�=�M�E���6am&+���<x��{���B�Ҹ?��������UI�ׄLV��A"?��[����~dV-�ZZ�:Z���<H���+��"X"��S#�al�����%��1�\8:����%��~/�㶺@pµ�!��U��r�H�O��6���/J���1�Q��k��p@�Tl��L
�&I~�i2�\�JF;G�&B�bYf`LF�f�As��YD���"�l��qJa�'�ɤ� �ÚruLû`ǭ�ȣ���7S?��!��I�@"��e�B]�S2� (���C@g
N�~G2҆�&g4Y�����m��X�7L�*����?�Z���B���Όh��s#���JZ���7z'*��.��/3)�����l��g	��1�����l��h���Zѡ�6}���8��@��H?��B(�|�➊&�d��ab���.m#�N��f�Ѳ��d><���^��yh��݉��s��V9���I�z�PT��׆J9{걘W�����,=:
���g�����ѩnclude/config/GENERIC_CLOCKEVENTS_BROADCAST) \
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