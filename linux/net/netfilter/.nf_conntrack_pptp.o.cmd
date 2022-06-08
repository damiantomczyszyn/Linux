// SPDX-License-Identifier: GPL-2.0
/*
 * kernel/lockdep_proc.c
 *
 * Runtime locking correctness validator
 *
 * Started by Ingo Molnar:
 *
 *  Copyright (C) 2006,2007 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2007 Red Hat, Inc., Peter Zijlstra
 *
 * Code for /proc/lockdep and /proc/lockdep_stats:
 *
 */
#include <linux/export.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kallsyms.h>
#include <linux/debug_locks.h>
#include <linux/vmalloc.h>
#include <linux/sort.h>
#include <linux/uaccess.h>
#include <asm/div64.h>

#include "lockdep_internals.h"

/*
 * Since iteration of lock_classes is done without holding the lockdep lock,
 * it is not safe to iterate all_lock_classes list directly as the iteration
 * may branch off to free_lock_classes or the zapped list. Iteration is done
 * directly on the lock_classes array by checking the lock_classes_in_use
 * bitmap and max_lock_class_idx.
 */
#define iterate_lock_classes(idx, class)				\
	for (idx = 0, class = lock_classes; idx <= max_lock_class_idx;	\
	     idx++, class++)

static void *l_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct lock_class *class = v;

	++class;
	*pos = class - lock_classes;
	return (*pos > max_lock_class_idx) ? NULL : class;
}

static void *l_start(struct seq_file *m, loff_t *pos)
{
	unsigned long idx = *pos;

	if (idx > max_lock_class_idx)
		return NULL;
	return lock_classes + idx;
}

static void l_stop(struct seq_file *m, void *v)
{
}

static void print_name(struct seq_file *m, struct lock_class *class)
{
	char str[KSYM_NAME_LEN];
	const char *name = class->name;

	if (!name) {
		name = __get_key_name(class->key, str);
		seq_printf(m, "%s", name);
	} else{
		seq_printf(m, "%s", name);
		if (class->name_version > 1)
			seq_printf(m, "#%d", class->name_version);
		if (class->subclass)
			seq_printf(m, "/%d", class->subclass);
	}
}

static int l_show(struct seq_file *m, void *v)
{
	struct lock_class *class = v;
	struct lock_list *entry;
	char usage[LOCK_USAGE_CHARS];
	int idx = class - lock_classes;

	if (v == lock_classes)
		seq_printf(m, "all lock classes:\n");

	if (!test_bit(idx, lock_classes_in_use))
		return 0;

	seq_printf(m, "%p", class->key);
#ifdef CONFIG_DEBUG_LOCKDEP
	seq_printf(m, " OPS:%8ld", debug_class_ops_read(class));
#endif
	if (IS_ENABLED(CONFIG_PROVE_LOCKING)) {
		seq_printf(m, " FD:%5ld", lockdep_count_forward_deps(class));
		seq_printf(m, " BD:%5ld", lockdep_count_backward_deps(class));

		get_usage_chars(class, usage);
		seq_printf(m, " %s", usage);
	}

	seq_printf(m, ": ");
	print_name(m, class);
	seq_puts(m, "\n");

	if (IS_ENABLED(CONFIG_PROVE_LOCKING)) {
		list_for_each_entry(entry, &class->locks_after, entry) {
			if (entry->distance == 1) {
				seq_printf(m, " -> [%p] ", entry->class->key);
				print_name(m, entry->class);
				seq_puts(m, "\n");
			}
		}
		seq_puts(m, "\n");
	}

	return 0;
}

static const struct seq_operations lockdep_ops = {
	.start	= l_start,
	.next	= l_next,
	.stop	= l_stop,
	.show	= l_show,
};

#ifdef CONFIG_PROVE_LOCKING
static void *lc_start(struct seq_file *m, loff_t *pos)
{
	if (*pos < 0)
		return NULL;

	if (*pos == 0)
		return SEQ_START_TOKEN;

	return lock_chains + (*pos - 1);
}

static void *lc_next(struct seq_file *m, void *v, loff_t *pos)
{
	*pos = lockdep_next_lockchain(*pos - 1) + 1;
	return lc_start(m, pos);
}

static void lc_stop(struct seq_file *m, void *v)
{
}

static int lc_show(struct seq_file *m, void *v)
{
	struct lock_chain *chain = v;
	struct lock_class *class;
	int i;
	static const char * const irq_strs[] = {
		[0]			     = "0",
		[LOCK_CHAIN_HARDIRQ_CONTEXT] = "hardirq",
		[LOCK_CHAIN_SOFTIRQ_CONTEXT] = "softirq",
		[LOCK_CHAIN_SOFTIRQ_CONTEXT|
		 LOCK_CHAIN_HARDIRQ_CONTEXT] = "hardirq|softirq",
	};

	if (v == SEQ_START_TOKEN) {
		if (!nr_free_chain_hlocks)
			seq_printf(m, "(buggered) ");
		seq_printf(m, "all lock chains:\n");
		return 0;
	}

	seq_printf(m, "irq_context: %s\n", irq_strs[chain->irq_context]);

	for (i = 0; i < chain->depth; i++) {
		class = lock_chain_get_class(chain, i);
		if (!class->key)
			continue;

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
�F�e� �5�=���!����P������>�]���q۲�|�{�3�"�%����hA�^�f��;��~�r�sE!�ă���.�nҐ�~C�Hi��n�ǓnKg�T��v��1���{�t��;6�/1��l@����Ų�</�@�e���޲[3��`�R�هU����p��݁6�ql�A7;@���������ne��_dL;�,ɭ�ѓ�v%.@�7�nW� ��.@(Z?wN�P�٘��jXs��6|��Z��*d�5��7l�������)~�$����m=ɔF]����~��H��X����G�j���3��Ќ��/��`��<�5w[�V�����a\
���-��2�����F�&���8
=����>�QJϬbt �-SD��Y20"A��$�pL�ؑ�4}KQo��wEr	�m7�ê5�>vl�������r��II=E�3vR�s䧔D�����{��V
��܊�-u�=��1r�	
�O���,^�)�1O]����A_��E�2ۆ��:;�� rO�߶.7�x,bE\�/>K��av"0I�1�E�pZ�"�+�!Z
�g��,�3#D{�qſ���a��i� �PC��7<��H�rG��F{�!R��L`�ſx�t'6)'���
���D�H�Z9�H�v}6o�9�O��,�_���j�27���V
m#�˵�d�m"�\-t-�j�e
<M�X�X���v���{����U���c�� +�8�J�ѥ�+���I**�������H6����-WK^�1.�Q��^�?R�Rj�$���H����b"�S��
xT�����Mq���_��%�<l�sY���Hf� ǌ:N��{d�gTy�j�����I�k��yf)��(��+�ɛh��6��h>J���gs_ɬ��D�!��^�����*a_��?�> xa�Y�j�8��=�":$8+����8�hiY򀽇V�DC�Y�>�ǽ�L*��at���N�eF�����q:���~���׀ޢ���ys�{�/4Ő(�I� 7��1�t��В�r��T�R4�x�����l0ڰ�+�,��e�맔�VZ��b���	Dz0�i���"[���	�:��C���a�H��^�)D^s�ę|3����0�Ѥ�Ep�U�\/��T����.zt����G<C^�vE���|\��(�!��oY�)�N��X5��5G�C����)*�l��`-w}ضk�H�Kc͵4������"
���ē!}]���(���7�Ԛ%�;��y�Lk�b~���_���^���Z��lJ���</��su-Ʋ�M����".��#��=�ĸ!b��?�6����47��]k+����d`��aH�K�a�"d�o���ޖ՚�ԭ���"'_\�D!����(�'f��4M����<�h��&�vt�����R���ĒB3A3��H*gi�9�M�40���t�YP/'x�F�'o0Hh`~�q��*H2�_C�+��eW�N;K;��tQ���WՒ"%�3d[��v ��
+�r�J
�Ӳjcq^���
(�E���1�~�N�n�oyh�4�A5�V�r_�����z1�-�
!_�u0�A�р����}�F���,����������[&Vx��0LD�~(�p���#���|P�1���s�m� ΖP�)���e��2��*��"-=�-�Q����Bd�wg�0��?Q"��6��[���y�u�.N��{����9�����7~:)^� �ʙ-KT��e�q�
��`1�ˏ 4ʨ��,����B�%�b{�m���*����a�F3����Y�ơ0�G�GB����-dV��Z�i���R�`�t!Jɤi*O����6o
ٚ��Ţ��c "����l��Y:ʌY�D���t������F>����$��D������ˊ��7+��C@����/��uȟ�1eE�j˗䘱_3[��E� ��*�G�?1�[NC@��mj�0lL��5��t���)�t���b��˟m-�2�2{�S4�z,�&����&
���9�f(bBt;��� c�Q�X*̮p����R�C���`�F���rS�d���-���&r^�@���C@����o������a�|?�|�n�øF������O`�:ٶvf�v$�4����?�^�f)����}�� _Q�4��ۿ�u�Ӛ��Ͻ)�	�I^�6↹,��Y��zB���!	t	{��_$����"��Z�25�zg��K'�ƞ��U��|��d߹4�^Cuw����@���C���ĸ8�%#���>��0�ef�g��?�w3��
��ddX^Q��<�ї���g!'hTh��D���>)
0��p=�7`����G�@#G��}^+>�8C<-j�>R�%��弄��A�%���M�|�+!���}[�fn�bK�jGtIX�����&��[,&jBg�P9��N�>�s���Ð��t�k����@H�R5�mH?_Z<ّX���vu�35��3���x�3�{$t��nۨ ���/�ǆ���5�����IT I'�8;��:�9�е2u�Wnκ�ge4H�����wv!��H���
^��bn�L�}(ڙ�G3d�4=Z(I3��׮��L��&����zΛo��1q���"wW�9�����a���d�P���.�ܯ��mY�9�As0
�l�EM%�Ŝ7e�uB���e%���{�ǰd��U��]Ǜ�|9�Ⱥ��P�?Ó���Z&m-�_�p���	E�7���i�#��^8/�=vc�������k����cC�x�KK�#: _�o�뼒��D�a�sߊخu��mxJ��ߗ3�j퉽�<�ܜ�Gl�6{#�Rh�:IPG�I��O%_���X�p]?qX�y��7!])��P���郸��&������om!8;�DwB�9'a�o�F�0��:����^��^ Kf4�����L��A:4�2�b�}w��
�짴ˀ���K�uPψ~慔�4����O� vio���.؆V�,
�~r�i��$������I�%�ǯ�"U��҅[�`ts>S'�0B�D�)��[ H��a8�y�""So���|&���h�ٿд�+��DR�l���3D)[vA�����h�洋6�?gxz�2��� �_+�Ud�X�|���O��D�'2���h	م�(��� �^��n5�>�|u��
,8%��M�L#:;`e��P�&��sR�zƺ��߽>2j�in$p)���sK�Q����J���\���6��2���O��%^"w��*��Hc F�+&�k����-�vR�BĘd��s����\�lJNE�@J+hR�����j}�9&E<9�4:'85��V�gKQq��_H
<\�]̅DK.��	.x�7\�!�1#�<)	Hq�v���!S�_��k�gT(Y�m�ߤ"o��ꥀ��s~nߞ�$ki?V$�2�si�B���ipz��Ow���6dG���#d�D;ݎz�x��jX��H�M� �;h���C6T1�>��Ȯp��ϰ��킫���(�i���mO�&bvB��Ʃb>Px$�u*fT� K@�U��ȾAF�l�?�<%�W��P}�Z Q�B��Rޑ._�?���4_w��լ0�O�w�<�Ք_�j xT<<쇭1J��@5�Ǒ�����c4>�������1Sט�������Gcvt�"Ydp�'���I�O؃s|�X2J��}�Y[��&yT\-RX�	͋o��!�M�i�H��"H�� 1j��&�T�M�Om.�x��G|�/�ܒ��4x�����'�m�Q\�`�(�ai,do�r�w(�Kl��tf�V��,&F2�	4a�����F��(4��I�Y�J"�@IZ~4���ǒ����Ѩ>
��O*`*�<�6�I��n_�zH�k��%��E=S��F���3�ݡeʊۊFxW���357�+=Z�[7�.�h>)L�>�⩡	Z�k������~A@b0�4l q�I�ח����:���8��~��)��{�,w�wm�?��Oh�H���cE����E��Чct<T�r)yt�� �����s��*�5\�@���0��﫶`�I߾�O|hY�2���e��Ml>��}��B�^�P������SP�����S�{���[>9yS����~����蕌El4�����CO3^�y%6�rI�k����������@����Yg;�T��[/�b��/�������M��� �6�j	k�7�z���	�nu�D����G����]��m�<����_�r�`A��%���������s�ςz-�2�)�>E��y�x�؊��߭A�+ƾX�6e�uM�y
��	�g�� �9� �E��Vm�㪱m;�2��Tⅉڐ&N��k\s������X0��w�-ϣz��.h�K@فn�B�|�\r��V%~�j}X��YNusp��%G{ɠ����X��uy1���'��Y���z8�I�Ih�K��W��r��Z7s�0p����=,�5q�l�����~�bdn�d!���NWSq��ϩ{X�M�o�	f�8���{d���3�%��B'�S�PϡQ�6i�2`%r���d��[�.����4QG���#��Uu�1�Й��x�����q�����%��m�g�A�E,t��Ih�Јkׇƹ�!��˪7zT!�q�(G�|�|��Ee�H�'q�:g����C� �)���'j�>\(�op���~\�2�m�� �9��N�Ț̟�\���圦�m.�r�dc^�W�.|`�|�Û�Q)��|	����M�Z�Erm��R��6�_�Ay���8C���Bt0V1�l�<����s+�q�-�*Nʭ�ֈ��󟚫�|1�F،"���=3�,.�`X. 	S�8geC˱*�� {v9>Ӌ�v�(u% �ʧarK�ˋ����D���`u��V�$&�4�̩x���5R�S���`�ғ���E�X�X$���h#���+/<o����F����f�	->|��-�N4.uki��H�'yB���L�/��~��4}��i����Tk��A��6����c#A=�s�ǵ*�G��2��fl����y�6 a�Ќ�����>�������oZ<�7��;}<��R���/N[\0�L)u�x�������J�iW�-���jts�:���r� ?��_;'��Z�۳#����c��������E��[ӿ~Fj}.��C���C���R�&��#�hJ��b���Bo�Y���Z=]�o���I�=�&�����Az�k�\���y�����΁뭙���}�-對�
�!���VaF�27�c���b�~I�7X��Ҁ�ݻ��Z+P�+��o0���/�_���Ji_�$���B�)C��#?�K��e&���]��O쯯8<{�h���8�q����x{��I�;��R3�ַL��+z�/_UBh&������������x�hM�����>��v�e̡����NSrB��P��h�ѡb�2�^�%��BL�Dy�p���g�@`��D�ǖ���3����B�m8K��}���["����L���<��[���j ?$�gA��;��F IJ�A��ۈ�;QZD7�,�(���Ϙ��8*�k���y_��~�@�Њ��V�]U�:"
�w!|,'�)�k���ڵV�:�VTSq5�J6*0�(�$�rY=�`���D�+�����J%5��Xx�gb�Y&s�K(���4Pؖ�1(�]}K���m�;kX�V��y5�MmRS���Y�K
{kE1��}$!;�>��`��s3�oÛlj`R}����^n{��h��У
�0W%�VNE��3���3��镱K�1�K��Vs��ȅ���'�ѯT��u{�9����;%*�T�G�{���8$g$%>�^ꮉ��ɸe�{
�� �޼- ���9������������s���Wc�"���/J�wF���q
�-��)�
:H�Y
�:M�'�u�5f���R��XV;}��J�ޞt��z��&�I���d��|��JŰLw�mi.2����+.˘ ��yz�6gm�C9����q��#�8<��Y�V��ϒi���~x0���>�Yzxh\��ȣ�� ���+��eC�>L1h8�8򑥣*�`�&�y�lxh>!�P��aVBQ������H�E�X�QO2�O��}C���e�ܓ{҄�U��о�:Td��b�2��<����eGX��`"�w�Z2Y3��O֡�3?u�Ժo����g��Z����MX]���󬧱[��;���X��*=��!:�O�4�`߫�jX�ۛxN�>eۇ��j�/���b"�bs[rp����G���dn/��+\�߹N&V.a�]:��"������/n�y����M^�f�De��u��pZ�px_�L&�+@P���G�;�����L"�TćS���BY��+���y ��iS1������V%W4�>@ȪK3�))`�]�'@�7V����qL"��
1_���2G@_���~[(V�.�W�&Ib�s�F��ə	<@IP�d�*KR;��1rh�XC�t&�,�4���ЎϜE��!-��"�i@��A��\Ej%"�l�;"����}Dy��G��_D�	��x�^�W�41�[���*�V��2A^y��,f� ��A���pn������(ٵ�Ȳ�ɎB\�K9yO��	�7��H`������_}y�%����w���'$�`���՝��1��R��2*���_�_TN��~�dlqd�06r�����?2��"s^/r�=N��>kr�F�#�>�8{�L�����j9�����FeP�GI�y�憞�0NZIt�
�U�jz���8Y��3_� C8@B�`٢��?3����
$�1�xϺ�?��:{r_���)$4�}��] �Bt�P��0�O�+�MN���f+S���lts]�@�*+�Z���6��줰A
�!?X�4�W���2�/��U�N�lDS�vDH���ƅ�~*�V[5��D�V,�gi.!V�5��u1
��Z���l�|���x��w�v�����i6�?��ĸ*Ƿ�2Cߛ�gsL�(�^�W
�����tN\" ׾��#<cY?tFɬ�ʅ����,sQ{~0�4��y�N���y�|s�I,�H��w4O_L��]��l���֩A�G�:���Vy�;��6;��y��-}�&mvz����(�Z:��o�r5���&�.��pt`�����^��?�i�)Yt�B�K�0%��&->�sn�͊<�y��\l�=�#G`�y.�)�ß�-��A]��]�kH�^�X�+��b'���9��b��/���KLOT?���poQv�C6�<���m��������#FA.H�W�"��HJ^/��Q(tD�O�Y6��~�{�����fe�Rːc�՘2?) ��FF�ҋ��N�'p����j�/�:�&�9�'���Ir�}����p�n�l�����xG3.:ӓO�-e;Y�c�d���ǥh�����1����&ۢ:hb!�&�ͺ5z`���y�Qd#��E.-�.5D�}�L$]qQά�>�\���Ŋ5Gx����(>�����_�8�v��d!6�e>g)��?1)��a����,,t2Ks���:�Y�{��*�`}�3�PBPsZ�1Ë�b�
�"���R��%zR���d�}���_��7�W^�|�B�OiqIf�ya�:��>G^�z��Wh��8u���0S���EXM�pP��;g��D���I|[�TH��z��k�BW��{�f�a+�
��f�N�U�
*A�p������[����e��W��
�1Mr�����Sk���!o��Z۾EU� ��o}%m�5E��:5ey�w�Nđ*c�lz���T���ܥ	M"�v!��*��A$�J7A. ��Te*���������s�E*��(W�����i?.o:�
f��FDm������"�Q�&;��B��tp�~U�^c��`���5:	iӥV�T��ɝ��E�%�Z_7�J� !Ǿ�L�Y_�(����gx��ڷ"4L���	v��}��A�G�L��p�W$�2�u��z����(*���h���0$@����W��J������F�-3��;	�� ��.��o�S�v�j^^�t��B��i=�=2�hȉXMg�Vf������X7�mn�\�
P.�hb~�N�S�����D�r����IŠ=�5�TvWn�ir�V\���BR���|�QT���I3�����>m���j#��5��
���%���2o�$�"9��[���J9'.�5�����t����{�z�Fe�a)����A�d�>=���y[��α���Z\��6�A>,�W+/�zp- �D-�Y5�n]��dA�W.:˃�OS�
��b#5Q$u�O��S�w]��ń�}�K�xl%Q�V��^��jc�Ç��j_)Km��P�
�+T~��!�W��� �]�8q{\{�Ǝ��1���t̀����V`P�/$���Ph��i���Bo�Kq��Ul�
}�>4������$Yў<�趲��L�D�>���������0�F~ �(���s Իp.��m�'=�p�y����U�2��,;�O�_Α*Y���v`G�r�?��r��)�����˘�rO��
�Ƀ�8�C�m�R����怼U�W-�1M����y
�=Zc����ay�y`��e�$�C����>(׍w�(�$5�
�[��4�D��5��{��D��E�PZ��K�
���&o�зg���˳����&�-�����[ ;��A�ʧ�Cx���zH�
����J��y@�Q����j���Eʳ~M�ઓ���P�p�@��M�0�
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
Ít& ��������|  �AA	  �1�Í�&    f���������u�A��u�A   1�Ít& �����Í�&    �v ������9w6�y   u-��D  �Q�Q��H  ��Q��H  ��A1�Í�&    ������Í�&    �v ��������   v��P�����w � � ���Ít& ����   ��Í�    1�Ít& �������P@��P  ��uÍt& ���L  j �@   ��   j j j@�������Í�&    ������P@��P  ��uÍt& ���L  j �@   ��   j j j �������Í�&    ���������L  ��d�
   �
   �ƍt& �   �   붍t& �   �   릍t& �   �   떍�&    �t& ������WVS��X  ��T  1�1�	�td���  ���  ��tD�{�s!�!׉�	�tJ�Ӊ�T  �� �  ��X  ��ۃ�`���  ��H  [^_�����t& ���� 1�빍�&    1�[^_ø�������v �������tG�yuAS��D  �Y��H  �A   ���A   �Q�A   �
��[Í�    ���   ��t�H�����   u��[Í�&    �t& �����VS����v  f=PQ��   1�f=QQ��   ��d  ��t�������D�    ���d  ��L  1ɺ   �������L  ��1�������@   �   ��d  E�L  j �P   j �   j P���������[^Ív f��x   �o�����p  1������4�    �V���1�f��x   �E����ԍ�&    ��    ������P(�H��	� tmv;��	� �   ��	� ��   �@|���������   ��4��������1�Ít& �� 	� th��	� ux�P|���   �Ѻ   �����1�Í�&    ��P|���   �Ѻ
   �����1�Í�&    ��P|���   �Ѻ   �����1�Í�&    ��P|���   �Ѻ	   �����1�ø�����f��������d  1҃���h  ��p  ����1�Í�&    f������U��W1�V��,     S�����  �D$��u�Z  f����ƌ   9��  �C  iǌ   1ҍ^�N`苀,  9�DV��@�F|   �$�   �D$1������D$�����������   �E`�ڋ@���������   �t$�D$��1�P�������ZY��x}�<$tO���_����T$1������$�$��d  �������D$��8  ��<  ���  ���4���������&    �t$�F��1��@P�������Y[��x��t������f����  ��t.��@  1ۉ׍�&    �v �����ƌ   �����9��  w������[^_]Ít& ��1�[��^_]Í�&    �t& ������UWVS�Ã��Bf�$�B����  ���$�σ��ʉL$���  ���~  �o��@  ��v ���   9��_  9�u�i��   �
��4��  ��;<$uX����	t$�L$f�DY����t`��T  ����� �X  ��  ��� �SD�1�����w��T�1����G�$��L  �L$����������i����  �t& ���L$f��D$d+    u
��1�[^_]��������&    �v �����S�Ӄ���L  d�    �T$1҉Ẉ   ������    ��I$�������  EC<�D$d+    u��1�[������������VS�Ã����   d�    �D$1�����   �   ����������$  �   �   ��  ��v �N��L  �����������u拓T  ��X  ��5��� 	���   ��������|  ����   ���|   ��P  ��ub��l  �@  ����������    ��   1������j ��L  �   �
v
w���   ��    �������v �����VS�Ã�d�    �D$��T  5��� �X  t�D$d+    ��   ��1�[^Í�    ��L  �Ẍ   �������\  ��`  ��I$������
vI	�t�ǃ\      ǃ`      ǃ@      ǃ<      ǃD  �  ǃH  @  �m����t& �4�    ��   1�1�	��O�����\  �� �  ��`  ǃ@      ǃ<      ǃD  �  t�ǃH  �  ��������������U1�WVS�Ã�d�    �D$��L  �L$���������  ��  ��&    �v �����UW��VS��d�    �D$�G�P�R�����%   =   ��  �o�   �  ���������= ����Y  ��t1�������   ������   �  ���������= ����&  ��t'��	  �����1҉�������   ��   ��   �������
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
 tvp5150 include/media/v4l2-subdev.h Disabled Black screen �$Ph    ���   h    ������L$���D  RSh   ���   h    �����1����  �����UW��VS�ك��T$�l$0d�    �D$1��,$��9$~Q�4$)�;t$4Ot$41�9�}������|  �D<G��D$PVS�t$h   ���   h    �����\$P��몋D$d+    t�������[^_]�h0   ���   h    ���������  �@�Ph    ���   h    ��������  hX   ���   �����1�XZ�j  ���E   �B   D�PhH   ���   h    ���������  ��h   �Z   �a   Eх�t2���q   �~   E�j VRPh�   ���   h    ��������  �x   �׋L$$�D$��Q�T$�L$R�V�r�WR�uh�   ���   h    ������D$,�� ����
  PhD  ���   h    �������L  �   �L$�����������	  Php  ���   h    �������L  �   �L$�����������	  Ph�  ���   h    �������L  �   �L$�����������	  Ph�  ���   h    �������L  �   �L$����������w	  Ph�  ���   h    �������L  �   �L$������ǃ����I	  ��L  �L$�   ������ƅ�x�t$��L  �L$�   �������x�D$WVPh   ���   h    �������L  �	   �L$�����������  Phh  ���   h    �������L  �
   �L$�����������  Ph�  ���   h    �������L  �   �L$�����������  Ph�  ���   h    �������L  �   �L$����������]  Ph�  ���   h    �������L  �
h�   �N����
  �ع�   �� jh�   �3���XZ�D$d+    ��   ��1�[^_]ËD$������D$������D$�(����D$�T����D$�����|$�����D$�����D$�B����D$�n����D$�����D$������D$������t$� ����t$�k����D$�����D$������D$�����D$�8����t$�f����D$�����D$������D$�����D$�3����t$�a����D$�����D$������D$����������D$�T$�$�L$f��f��f��v  ��f��x  �S�$��  R�S�R�T$RQPhh	  ���   �������v  ��f=PQ��   ��x  ���   f��!��   h  R�����XZ���  1�9���   iȌ   ��T  ���8  �$��X  �<  �ы$��X  ��T  �PhD	  �D$��P���������  ���   f=QQu$f��x   uhH  R�����XZ�s���f�� tPh�	  ���   ��������S���h2  R�������L  1ɺ0   �����Y[�/�����uǆT  ��� ǆX      ���  1ɺ   ǆ\      �$��j ǆ`      ǆd     ǆp     ������ 	� �  ��j h�   j jj h�   j j ������	� �  �؃�$j h�   j jj h�   j j ������	� �  �؃� j h�   j jj h�   j j ������	� �  �؃� j j j jj jj�j�������	� �  �؃� j h���j jj h���j h���������  �ع	� �� h  j j j j������^l��  ���څ�t�$�T$������\$��  ��L  �L$��   �������x�D$���@  ����
��   ���  ��H  1�ǆ@      ��P  ��ǆ<      ǆD  �  �,  ���  ��t&V1ɉ�h[  h   h   ����������R�������������A����=    ~����  1҉�������������1҉��������  ��    % �  ��҃�`���  �9����q�q�q�qhh   h�	  ���   h    ������� �B     W  �  �  c  �	P  c  �	������    �    ������    �����           �              �                                              @                �     tvp5150_get_vbi tvp5150_set_selection   tvp5150                                                                                                                                         @           �  @  
                                                (   (   ,   0   �   �   �   �   �   �   �   �   p
         �                                  �  �      @             �  �  @   p       @  @                                                  �  0  �  �      @
          �                        @                                                                          �                                          �              0                      @              `	                                                                                                                                                                                                      0               ����'. +�r                                                                                                                                                                                        �               �*�?Qni�	   '    @           [U�� qnB��   :                                                                                                             �               ������+
� �
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
     �	  �   0     �	             �	              tvp5150.c tvp5150_g_std tvp5150_get_mbus_config tvp5150_enum_mbus_code tvp5150_enum_frame_size tvp5150_volatile_reg tvp5150_runtime_resume tvp5150_runtime_suspend tvp5150_read tvp5150_set_std tvp5150_set_std.cold tvp5150_s_std tvp5150_fill_fmt tvp5150_fill_fmt.cold dump_reg_range tvp5150_close tvp5150_g_sliced_vbi_cap vbi_ram_default tvp5150_g_sliced_vbi_cap.cold tvp5150_subscribe_event tvp5150_remove tvp5150_remove.cold tvp5150_s_raw_fmt tvp5150_s_sliced_fmt tvp5150_s_sliced_fmt.cold tvp5150_isr tvp5150_ev_fmt tvp5150_isr.cold tvp5150_open tvp5150_selmux tvp5150_selmux.cold tvp5150_s_ctrl tvp5150_s_routing tvp5150_registered tvp5150_link_setup tvp5150_link_setup.cold tvp5150_reset tvp5150_init_default tvp5150_g_sliced_fmt tvp5150_g_sliced_fmt.cold __func__.2 tvp5150_g_tuner tvp5150_s_stream tvp5150_init_enable CSWTCH.90 tvp5150_querystd tvp5150_init_cfg tvp5150_log_status tvp5150_log_status.cold tvp5150_probe tvp5150_config tvp5150_ops tvp5150_internal_ops tvp5150_sd_media_ops tvp5150_probe.cold tvp5150_ctrl_ops tvp5150_test_patterns tvp5150_set_selection tvp5150_set_selection.cold __func__.1 tvp5150_get_selection tvp5150_driver_init tvp5150_driver tvp5150_driver_exit tvp5150_pm_ops tvp5150_id tvp5150_readable_table tvp5150_readable_ranges tvp5150_core_ops tvp5150_tuner_ops tvp5150_video_ops tvp5150_vbi_ops tvp5150_pad_ops __UNIQUE_ID_debug283 __UNIQUE_ID_debugtype282 __param_debug __param_str_debug __UNIQUE_ID_license281 __UNIQUE_ID_author280 __UNIQUE_ID_description279 __fentry__ regmap_update_bits_base __stack_chk_guard regmap_read __stack_chk_fail regmap_write _dev_printk __pm_runtime_idle v4l2_src_change_event_subdev_subscribe v4l2_ctrl_subdev_subscribe_event v4l2_fwnode_con