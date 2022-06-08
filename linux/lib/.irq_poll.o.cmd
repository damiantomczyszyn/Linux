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
			nr_list_entries, MAX_LOCKDEP_E/uapi/asm/sigcontext.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/TRACEPOINTS) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  include/linux/rcutree.h \
  include/linux/wait.h \
  include/linux/spinlock.h \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/uapi/linux/wait.h \
  include/linux/refcount.h \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/rhashtable-types.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
  include/linux/osq_lock.h \
  include/linux/debug_locks.h \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcaELF                      ¼2      4     (               èüÿÿÿ‰ÁSk€t  L|  ‹X4‹H01À‰J‰Z[Ã´&    t& èüÿÿÿ‰ÁSk€t  L|  ‹X4‹H01À‰J‰Z[Ã´&    t& èüÿÿÿƒy…Á   VSy   ‰Ã‰È…   ƒy…”   k“t  L‹I;Š`  …~   ‹²d  9pus‰‹¨  ‹P‰“¬  ‹P‰“°  ‹P‰“´  ‹P‰“¸  ‹P‰“¼  ‹P ‰“À  ‹P$‰“Ä  ‹P(‰“È  ‹P,‰“Ì  ‹P0‰“Ğ  ‹@4‰ƒÔ  1À[^Ãt& ¸êÿÿÿëñ´&    f¸êÿÿÿÃ´&    v èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[Ãèüÿÿÿ‰ÊWƒÁ1ÀzS‹YüƒçüÇB    )ùÇB,    ƒÁ(Áéó«…ÛuÇB   [_Ãv ¸êÿÿÿëó´&    fèüÿÿÿUWVS‰Ãƒì‹@X‰T$‰$…À„   ‹C‹P‹Rèüÿÿÿ%   =   …z  C¹À  ºÜ  èüÿÿÿ‰Å…À„e  1À¹÷   ‰ï¾€   ó«½4  0  Ç…<  Ğ  Ç…@  à  ƒçüÇ…D  UYVY)ùÇ…H     )ÎÁ   Ç…L     ÁéÇ…Q  Œ
 Ç…l     Ç…p     Ç…x     Ç…|  `  ¡€   ‰…0  ¡|  ‰…,  ó¥¹   ‹D$‹<$‹@‰½0  ‰…Ø  ¶À	…Ô  ¶G¶WÇ…D      ÁàÁâ	Ğ	…Ä  ‰Ú‰èèüÿÿÿƒM\º   ‰èÇ…¤     ˆ  ÇE  èüÿÿÿ‰Ã…Àˆüÿÿÿj µÀ   1Éº   ‰ğèüÿÿÿ¹ 	˜ ºP  ‰ğj h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºP  ‰ğƒÄ$j h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºP  ‰ğƒÄ j h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºP  ‰ğƒÄ j j j h´   j h´   jÿhLÿÿÿèüÿÿÿ¹	˜ ºP  ‰ğƒÄ j jj jj jj j èüÿÿÿ‹ü   ‰ulƒÄ …Ût…Ûx,ƒÄ‰Ø[^_]Ã¶    ‰ğèüÿÿÿ‰èèüÿÿÿ‰Ã…À„-   …ÛyÔ‰ğèüÿÿÿ‰ØƒÄ[^_]Ãv »ûÿÿÿë¹»ôÿÿÿë²´&    v èüÿÿÿ‰Â‰È‹	ƒx …»   ƒùt>Ç@   kŠt  L‹‰`  ‰Hk’t  LÇ@   Ç@   ‹’d  ‰P1ÀÃ¶    ‹Š¨  ‰H‹Š¬  ‰H‹Š°  ‰H‹Š´  ‰H‹Š¸  ‰H‹Š¼  ‰H‹ŠÀ  ‰H ‹ŠÄ  ‰H$‹ŠÈ  ‰H(‹ŠÌ  ‰H,‹ŠĞ  ‰H0‹’Ô  ‰P41ÀÃ´&    f¸êÿÿÿÃ´&    v èüÿÿÿU‰ÁWVS‰Óƒì‰D$¶Qp‰T$<„}   ´&    v <t\<t`‹D$¶kÇ$    ¶{‹°”   ‰é‰ú‰ğèüÿÿÿ…Àt8ƒ<$…D   P‰D$ÿt$h-   èüÿÿÿ‹T$ƒÄƒÄ‰Ğ[^_]Ãf‹Cèüÿÿÿ¶CƒÃ<uƒÄ1Ò[‰Ğ^_]ÃèüÿÿÿUhpWV1öS‹¸”   ¶Ú‰Ú‰øèüÿÿÿƒşuPSUhd   èüÿÿÿƒÄ[^_]Ã¶    …Àˆl   ëÛ¶    èüÿÿÿUWVSƒì‹¨x  ‰$…íÒ   ‹€|  ‰Ö‰Ï1Ò‰D$ƒÀë¶    ƒÂƒÀL9Õ„š   ‹‹H!ó!ù	Ùtåƒú„„   kÚL‹D$‰ÖØ¶h‹$ÇD$    ‹¸”   ƒÀp‰D$º   ‰é‰øèüÿÿÿ‰Â…Àtƒ|$…   ƒÄ‰Ğ[^_]Ãt& ‹<$Ÿ|  ‰·t  ¶C€=     ‰‡D  …¶   ƒÄ‰Ğ[^_]ÃƒÄºêÿÿÿ[‰Ğ^_]Ãtï‹$1Û1ö‹€|  édÿÿÿ´&    ¶    èüÿÿÿUWVS‰Ãƒì‹Ø  è×ıÿÿ‰ÆCp‰D$…ö…Ü   ‹»”   1íº€   ‰øèüÿÿÿ…Ày	ƒı…  ‰D$‹«”   Ç$    º   ‰èèüÿÿÿƒ<$t…Àˆç  ‰$‹«”   ºp   ‰èèüÿÿÿƒşt…ÀˆÃ  €=     ‰Â„  éö   t& èüÿÿÿ…Ò„É  UWV‰ÖS‰Ãƒì‹€8  …À„š  Cp‹«”   1ÿ‰$º   ‰èèüÿÿÿƒÿ…ç   ‰Çƒç„ï   ƒà<„!  <…©   ‹ƒ€  ½   ƒø‡  ºw  ¿   £Âƒ  Cp‹‹”   ÇD$    ‰D$‰ğ‰ş‰$‰Ç‰Ëº:   ‰Øèüÿÿÿ…Àyƒ|$…¬  ‰ù‰÷‹$‰Î!ø‰ù8Èu6kíL‹‹|  ‹‹F#T)#D)‰€=     ‰F…f  ƒÄ1À[^_]Ãt& Ç    1ÀÇF    ƒÄ[^_]Ã´&    f…Àˆˆ  ‰Çƒç…ÿÿÿCp‹«”   ‰$º?   ‰èèüÿÿÿƒÿ„ğşÿÿ…ÀˆB  ƒà<…èşÿÿ´&    f‹ƒ€  1íƒø†êşÿÿƒèDƒøw
º£ÂrƒÄ¸êÿÿÿ[^_]Ã¿   éÔşÿÿ¶    ‰Øè‰ıÿÿ¸È   èüÿÿÿéPşÿÿ¸êÿÿÿÃt& èüÿÿÿ98  „   UWV‰ÆS‰Óƒì…Òt8ºíÿÿÿƒûu ƒÄ[^_]é:ıÿÿ´&    v Ç†8      ƒÄ‰Ğ[^_]Ãt& ‹¸”   hpº   ¹   ‰øèüÿÿÿ‰Â…ÀtÈƒû…Õ  ‰$UhX  èüÿÿÿXZ‹$ë´´&    v 1ÀÃ´&    ¶    èüÿÿÿUWVSƒì‰$ƒú^‡ğ   ‰Ãƒù‡å   Cp‹³”   ‰Õ1ÿ‰D$1Ò‰é‰ğèüÿÿÿ‰Â…Àtƒÿ…ö  ƒÄ‰Ğ[^_]Ã´&    ‹ƒ”   1ö{p‰D$‹D$º3   èüÿÿÿƒşt…Àˆ  ‰ÁCp‰\$‹³”   ‰D$ƒá$1ÿ‰Ëº3   ‰Ù‰ğèüÿÿÿ‰Â…Àtƒÿ…;  ƒÄ‰Ğ[^_]Ãf‰Ù‹$‹\$€=     ‰«4  ‰‹Ì  ‰«€  ‰ƒ„  „Pÿÿÿé_  t& ºêÿÿÿé<ÿÿÿ´&    ´&    èüÿÿÿUWVS‰Ãƒì‹p‹x|‹@(=	˜ „Î   v\=	˜ „I  =	˜ …  ƒÿF°Ç$    ‹nÔÉ‰D$ƒáı‰|$ƒÁ‰Ïº   ‰ù‰èèüÿÿÿ‰Â…À„z  ƒ<$t^é¥  = 	˜ „¥   =	˜ …©  ‰øÇ$    ‹nÔ¶ÈF°‰|$‰D$‰Ïº
   ‰ù‰èèüÿÿÿ‰Â…À„  ƒ<$…@  t& €=     …  ƒÄ‰Ğ[^_]Ã‰øÇ$    ‹nÔ¶ÈF°‰|$‰D$‰Ïº   ‰ù‰èèüÿÿÿ‰Â…À„»   ƒ<$t®é}  ‰øÇ$    ‹nÔ¶ÈF°‰|$‰D$‰Ïº	   ‰ù‰èèüÿÿÿ‰Â…À„–   ƒ<$„jÿÿÿéÍ  t& ÿ´   „œ   ÿLÿÿÿ„    ‰ø¶ÈF°Ç$    ‹nÔ‰D$‰|$‰Ïº   ‰ù‰èèüÿÿÿ‰Â…ÀtIƒ<$„ÿÿÿéõ  ‹|$‰¾Ä   éùşÿÿ‹|$‰¾Ì   éêşÿÿ‹|$‰~|éŞşÿÿ‹|$‰¾¼   éÏşÿÿ‹|$‰¾Ô   éÀşÿÿ´&    ¹   ¿   éfÿÿÿ¹€   ¿€   éVÿÿÿºêÿÿÿéşÿÿ           °  @                  É                                                                                                                                ?                              	  €    
  €      €                                €      €      €            €           U           %                      @                          !      "     #      $     %      &      '      (  Ì   )      *      +      ,      -      .      /      0      1       2       3       4      5  î    6  ¯    7  ÿ    8  ÿ    9                                                        0   `   @  `  °  €  `    `  p      
  °
  Ğ      No platform data
 4%s: Write: retry ... %d
 3%s: Write failed. Err[%d]
 4%s: Read: retry ... %d
 6%s: Reg(0x%.2X): 0x%.2X
 7%s: Standard set to: %s
 7%s: Current STD: %s
 7%s: Input set to: %d
 tvp514x    3%s: %s decoder driver failed to register !!
  6%s: %s decoder driver registered !!
  3%s: Unable to turn on decoder
    7%s: chip id detected msb:0x%x lsb:0x%x rom version:0x%x
  3%s: chip id mismatch msb:0x%x lsb:0x%x
   3%s: Unable to detect decoder
 6%s: %s (Version - 0x%.2x) found at 0x%x (%s)
 3%s: Unable to configure decoder
  3%s: Unable to turn off decoder
   7%s: Set Control: ID - %d - %d
 ƒÅpUUh    èüÿÿÿƒÄé$  ƒÃh    S»êÿÿÿèüÿÿÿXZé$  ƒÅpUUh0   èüÿÿÿƒÄé$  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄé­  VƒÆUhJ   èüÿÿÿ¸
   èüÿÿÿƒÄé#  ÿt$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéß  ƒÃ‰T$S‹D$ƒÀpPh   èüÿÿÿ‹T$ƒÄéş  ÿt$hX   èüÿÿÿY[ƒÄ‰ğ[^_]Ã¶À‰T$P¶D$P¶D$Pÿt$h|   èüÿÿÿ‹T$ ƒÄ€|$Qt/¶$P¶D$P‹t$Vh¸   èüÿÿÿV¾íÿÿÿhä   èüÿÿÿƒÄë—¶$ƒèF<wÆ¶ÂƒÇ‰ƒ4  ‹O‘  R·WşÒRPWÿt$h  èüÿÿÿ“0  ‰Øè\  ‰ÆƒÄ…À…Œ   €=     …”   Çƒ8     é)ÿÿÿVƒÆÿt$hJ   èüÿÿÿ¸
   èüÿÿÿƒÄéë  ÿ4$ÿt$hJ   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéÈ  UƒÅÿt$hJ   èüÿÿÿ¸
   èüÿÿÿƒÄé  ÿt$h4  èüÿÿÿXZé¤şÿÿ1Ò‰Øè  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº	   è  ‰Øº
   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº   è  ‰Øº    è  ‰Øº!   è  ‰Øº"   è  ‰Øº#   è  ‰Øº$   è  ‰Øº%   è  ‰Øº2   è  ‰Øº3   è  ‰Øº4   è  ‰Øº5   è  ‰Øº6   è  ‰Øº7   è  ‰Øº8   è  º9   ‰Øè  érıÿÿWƒÇÿt$hJ   èüÿÿÿ¸
   èüÿÿÿƒÄék	  «|  ƒÃpEPShš   èüÿÿÿ1ÀƒÄé;	  WƒÇÿt$hJ   èüÿÿÿ¸
   èüÿÿÿƒÄéP  ÿt$ÿt$hJ   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéÊ  SƒÃUh   èüÿÿÿ¸
   èüÿÿÿƒÄé]
  WƒÇÿt$h   èüÿÿÿ¸
   èüÿÿÿƒÄéà
  VƒÆWhJ   èüÿÿÿ¸
   èüÿÿÿƒÄé  WƒÇÿt$h   èüÿÿÿ¸
   èüÿÿÿƒÄéS  ƒÃp‰$USh±   èüÿÿÿ‹T$ƒÄéú
  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéØ  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄé(  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄé  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄéx  ƒîP‰$ÿs|ÿs(Vh|  èüÿÿÿ‹T$ƒÄé±  ÿ4$ÿt$h   èüÿÿÿ¸
   ƒD$èüÿÿÿƒÄé~  èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ     tvp5146             @  tvp5146m2           x   tvp5147                 tvp5147m1           x                                                                è      é       ê  €    à      è  `    é       ê  °    à      è      é       ê       à      è  `    é       ê  °    à                                   è      é       ê  €    à      è  `    é       ê  °    à      à                                                     À      `          €  `                                           °
          `                          
          0                                   Ğ      Ğ  à          °      NTSC                    é  0u                    Ğ  @        ÿ       PAL                           q                                     8-bit UYVY 4:2:2 Format         UYVY                debug license=GPL description=TVP514X linux decoder driver author=Texas Instruments parm=debug:Debug level (0-1) parmtype=debug:bool    @          ¤ÿ      GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ                                                 %     $   0   %     =   `   Ö     T   `  ˜     e   @       t   `  G                                	              
 ‹   °  Æ    ™   €        ²      @     Ã            Ï   P       à       H    
 ó   €  Ö     
  `  ¯       H   (    
 5    J     >  p   !    
 L  `      ^           Z  ‘   O    
 m  p  ¬     …  à   f   
 ¢     Ü    ³  F  “    
 É   
  £     Ú  Ù  !    
 ğ  °
        ú  ‡    
   Ğ  I    (    ë    
               <           P      €     _      
     s      x                   ~  @  `     “  x        ©      ˜     ¾     P     Ğ  À  @     à           ÷     )       5        (  N        =  k        V           d  @       v                                       µ             Î             Û             ğ                          $             6             N             i             q             z             ”             ›             °             É             ×             ë           ÷                   
           x     8              tvp514x.c tvp514x_g_frame_interval tvp514x_s_frame_interval tvp514x_set_pad_format tvp514x_std_list tvp514x_remove tvp514x_enum_mbus_code tvp514x_probe tvp514x_reg_list_default tvp514x_fmt_list tvp514x_ops tvp514x_ctrl_ops tvp514x_probe.cold tvp514x_get_pad_format tvp514x_write_regs tvp514x_write_regs.cold dump_reg dump_reg.cold tvp514x_s_std tvp514x_s_std.cold tvp514x_s_stream.part.0 tvp514x_s_stream.part.0.cold tvp514x_querystd tvp514x_querystd.cold tvp514x_s_stream tvp514x_s_stream.cold tvp514x_s_routing tvp514x_s_routing.cold tvp514x_s_ctrl tvp514x_s_ctrl.cold tvp514x_driver_init tvp514x_driver tvp514x_driver_exit tvp514x_id tvp5146_init_reg_seq tvp514xm_init_reg_seq tvp5147_init_reg_seq tvp514x_video_ops tvp514x_pad_ops __UNIQUE_ID_license270 __UNIQUE_ID_description269 __UNIQUE_ID_author268 __UNIQUE_ID_debug267 __UNIQUE_ID_debugtype266 __param_debug __param_str_debug __fentry__ v4l2_async_unregister_subdev v4l2_ctrl_handler_free __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init media_entity_pads_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup v4l2_async_register_subdev _printk _dev_err i2c_smbus_write_byte_data msleep msleep_interruptible i2c_smbus_read_byte_data __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tvp514x_id_device_table param_ops_bool     8  1   8  a   8  ˜   +  ¤   +  A  8  L  9  W  :  a  8  ±  8  Ú  ;  ü  <      ‚  +  –  +  ›    ¦    ³  +  ı  =  $  >  D  ?  N  +  k  @  u  +  •  @  Ÿ  +  ¿  @  É  +  ì  @  ö  +    @  ;  A  B  B  W  :    8  ª  +  È  +  a  8  ¸  E  Ô    Ù  C  ô  F    8  ,  H  9    >  C  a  8  í  E  %    q  8  ª  H  Ô  H  ÷  H  
    !  8  \  H  Ö  H  	    w	  H  í	  F  
  8  n
  E  †
    ‹
  C  ±
  8  ë
  E  )  H  a  E  ‹    Ñ  8  6  E  Œ  E  ª    æ  E  &  E  †  E  Ì    .    L    Æ    T    ş    2    –    ·    â                å    	    T	    ˆ	    }
    ú
    6    p    ¯    K         ±    û    ?    ›                   h   +                                                 $     (     ,     0     4     8     <   &          C       &   D  7     <   C  P     U   C  d   G  v     {   C  …   G  š     Ÿ   C  ®   G  Ë     Ğ   C  å     ê   C        C  ;    @  C  K    P  C  ˆ      C  š    ­    Ğ    Õ  C  ß  G  ó    ø  C    G      !  C  +  G  <    A  C  Q    ]    i    u            ™    ¥    ±    ½    É    Õ    á    í    ù                )    5    A    M    Y    e    q    }    ‰    •    ¡    ­    ¹    Å    Ñ    İ    é    õ                %    1    =    O    T  C  ^  G  y    ~  C  •    š  C  ¤  G  ¹    ¾  C  Í  G  ß    ä  C  î  G        C    G  $    )  C  3  G  H    M  C  W  G  l    q  C  ‰      C    G  ±    ¶  C  Å  G  Ù    Ş  C  í  G        C    G  /    4  C  L    Q  C  `  G       -     D     l          ¶     Ü     ç        3    f    ˆ    ¬    Õ    ö        ;    _    }    ¥    Í    õ        @    h       8          I     J          L     +  ,   +  D   +  \   +  ¬  +  ¼  +  Ä    Ğ    Ô                 (    0    4    X        +     I     O        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rodata.str1.4 .rel.text.unlikely .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @                       	   @       €(  @              )             `  €                  %   	   @       À+                  /             à                    8             à  @                  4   	   @       à+  €               E      2          Ñ                 T      2       ô                  g             ‘  l                 c   	   @       `,  ¨     
         z             ı                    v   	   @       1                  ‰               
                  …   	   @       (1                 ˜                F                  ”   	   @       81  €                             f                    ­             è                    ©   	   @       ¸1                  µ      0       ü                   ¾                                   Î               (                                8        8         	              8#  G                               Ø1  á                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              linux/rbtree_latch.h \
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
  include/linux/dma-direction.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/STACK_GROWSUP) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/percpu-refcount.h \
  include/linux/bit_spinlock.h \
  include/linux/shrinker.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/CGROUP_WRITEBACK) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/cIO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
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
  include/linux/kref.h \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/apicdef.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/linux/ratelimit.h \
  include/linux/irq.h \
    $(wildcard include/config/GENERIC_IRQ_IPI) \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/DEPRECATED_IRQ_CPU_ONOFFLINE) \
    $(wildcard include/config/GENERIC_IRQ_MIGRATION) \
    $(wildcard include/config/GENERIC_PENDING_IRQ) \
    $(wildcard include/config/HARDIRQS_SW_RESEND) \
    $(wildcard include/config/GENERIC_IRQ_LEGACY) \
    $(wildcard include/config/GENERIC_IRQ_MULTI_HANDLER) \
  include/linux/irqhandler.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/CMA) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/generated/bounds.h \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/uprobes.h \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  include/asm-generic/topology.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
    $(wildcard include/config/PCI) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/fwnode.h \
  include/linux/vmalloc.h \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  include/linux/overflow.h \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/gfp.h \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  arch/x86/include/generated/asm/irq_regs.h \
  include/asm-generic/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/SPARSE_IRQ) \
    $(wildcard include/config/IRQ_DOMAIN) \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  arch/x86/include/asm/hw_irq.h \
  include/linux/profile.h \
    $(wildcard include/config/PROFILING) \
  include/linux/sched/isolation.h \
    $(wildcard include/config/CPU_ISOLATION) \
  include/linux/tick.h \
    $(wildcard include/config/GENERIC_CLOCKEVENTS) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/TICK_ONESHOT) \
    $(wildcard ird include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/tsc.h \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/nodemask.h \
    $(wildcard include/config/HIGHMEM) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/mm_types_task.h \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/uapi/linucmd_drivers/media/i2c/tvp514x.o := gcc -Wp,-MMD,drivers/media/i2c/.tvp514x.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -min