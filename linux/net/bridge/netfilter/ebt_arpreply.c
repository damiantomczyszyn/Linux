 0, 40 + 1 + 12 * (14 + 1));
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
	proc_create_single("