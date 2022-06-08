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
º£ÂrƒÄ¸êÿÿÿ[^_]Ã¿   éÔşÿÿ¶    ‰Øè‰ıÿÿ¸È   èüÿÿÿéPşÿÿ¸êÿÿÿÃt& èüÿÿÿ98  „   UWV‰ÆS‰Óƒì…Òt8ºíÿÿÿƒûu ƒÄ[^_]é:ıÿÿ´&    v Ç†8      ƒÄ‰Ğ[^_]Ãt& ‹¸”   hp