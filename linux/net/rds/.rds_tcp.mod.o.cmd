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
    $(wildcaELF                      Ό2      4     (               θόÿÿÿ‰ΑSk€t  L|  ‹X4‹H01ΐ‰J‰Z[Γ΄&    t& θόÿÿÿ‰ΑSk€t  L|  ‹X4‹H01ΐ‰J‰Z[Γ΄&    t& θόÿÿÿƒy…Α   VSy   ‰Γ‰Θ…   ƒy…”   k“t  L‹I;`  …~   ‹²d  9pus‰‹¨  ‹P‰“¬  ‹P‰“°  ‹P‰“΄  ‹P‰“Έ  ‹P‰“Ό  ‹P ‰“ΐ  ‹P$‰“Δ  ‹P(‰“Θ  ‹P,‰“Μ  ‹P0‰“Π  ‹@4‰ƒΤ  1ΐ[^Γt& Έκÿÿÿλρ΄&    fΈκÿÿÿΓ΄&    v θόÿÿÿS‹X\‰Ψθόÿÿÿƒΐ   θόÿÿÿ1ΐ[Γθόÿÿÿ‰ΚWƒΑ1ΐzS‹YόƒηόΗB    )ωΗB,    ƒΑ(Αισ«…ΫuΗB   [_Γv Έκÿÿÿλσ΄&    fθόÿÿÿUWVS‰Γƒμ‹@X‰T$‰$…ΐ„   ‹C‹P‹Rθόÿÿÿ%   =   …z  CΉΐ  Ίά  θόÿÿÿ‰Ε…ΐ„e  1ΐΉχ   ‰οΎ€   σ«½4  0  Η…<  Π  Η…@  ΰ  ƒηόΗ…D  UYVY)ωΗ…H     )ΞΑ   Η…L     ΑιΗ…Q  
 Η…l     Η…p     Η…x     Η…|  `  ΅€   ‰…0  ΅|  ‰…,  σ¥Ή   ‹D$‹<$‹@‰½0  ‰…Ψ  ¶ΐ	…Τ  ¶G¶WΗ…D      ΑΰΑβ	Π	…Δ  ‰Ϊ‰θθόÿÿÿƒM\Ί   ‰θΗ…¤       ΗE  θόÿÿÿ‰Γ…ΐόÿÿÿj µΐ   1ΙΊ   ‰πθόÿÿÿΉ 	 ΊP  ‰πj h€   j jj hÿ   j j θόÿÿÿΉ	 ΊP  ‰πƒΔ$j h€   j jj hÿ   j j θόÿÿÿΉ	 ΊP  ‰πƒΔ j h€   j jj hÿ   j j θόÿÿÿΉ	 ΊP  ‰πƒΔ j j j h΄   j h΄   jÿhLÿÿÿθόÿÿÿΉ	 ΊP  ‰πƒΔ j jj jj jj j θόÿÿÿ‹ό   ‰ulƒΔ …Ϋt…Ϋx,ƒΔ‰Ψ[^_]Γ¶    ‰πθόÿÿÿ‰θθόÿÿÿ‰Γ…ΐ„-   …ΫyΤ‰πθόÿÿÿ‰ΨƒΔ[^_]Γv »ϋÿÿÿλΉ»τÿÿÿλ²΄&    v θόÿÿÿ‰Β‰Θ‹	ƒx …»   ƒωt>Η@   kt  L‹‰`  ‰Hk’t  LΗ@   Η@   ‹’d  ‰P1ΐΓ¶    ‹¨  ‰H‹¬  ‰H‹°  ‰H‹΄  ‰H‹Έ  ‰H‹Ό  ‰H‹ΐ  ‰H ‹Δ  ‰H$‹Θ  ‰H(‹Μ  ‰H,‹Π  ‰H0‹’Τ  ‰P41ΐΓ΄&    fΈκÿÿÿΓ΄&    v θόÿÿÿU‰ΑWVS‰Σƒμ‰D$¶Qp‰T$<„}   ΄&    v <t\<t`‹D$¶kΗ$    ¶{‹°”   ‰ι‰ϊ‰πθόÿÿÿ…ΐt8ƒ<$…D   P‰D$ÿt$h-   θόÿÿÿ‹T$ƒΔƒΔ‰Π[^_]Γf‹Cθόÿÿÿ¶CƒΓ<uƒΔ1Ò[‰Π^_]ΓθόÿÿÿUhpWV1φS‹Έ”   ¶Ϊ‰Ϊ‰ψθόÿÿÿƒώuPSUhd   θόÿÿÿƒΔ[^_]Γ¶    …ΐl   λΫ¶    θόÿÿÿUWVSƒμ‹¨x  ‰$…νÒ   ‹€|  ‰Φ‰Ο1Ò‰D$ƒΐλ¶    ƒΒƒΐL9Υ„   ‹‹H!σ!ω	Ωtεƒϊ„„   kΪL‹D$‰ΦΨ¶h‹$ΗD$    ‹Έ”   ƒΐp‰D$Ί   ‰ι‰ψθόÿÿÿ‰Β…ΐtƒ|$…   ƒΔ‰Π[^_]Γt& ‹<$|  ‰·t  ¶C€=     ‰‡D  …¶   ƒΔ‰Π[^_]ΓƒΔΊκÿÿÿ[‰Π^_]Γtο‹$1Ϋ1φ‹€|  ιdÿÿÿ΄&    ¶    θόÿÿÿUWVS‰Γƒμ‹Ψ  θΧύÿÿ‰ΖCp‰D$…φ…ά   ‹»”   1νΊ€   ‰ψθόÿÿÿ…ΐy	ƒύ…  ‰D$‹«”   Η$    Ί   ‰θθόÿÿÿƒ<$t…ΐη  ‰$‹«”   Ίp   ‰θθόÿÿÿƒώt…ΐΓ  €=     ‰Β„  ιφ   t& θόÿÿÿ…Ò„Ι  UWV‰ΦS‰Γƒμ‹€8  …ΐ„  Cp‹«”   1ÿ‰$Ί   ‰θθόÿÿÿƒÿ…η   ‰Ηƒη„ο   ƒΰ<„!  <…©   ‹ƒ€  ½   ƒψ‡  Ίw  Ώ   £Βƒ  Cp‹‹”   ΗD$    ‰D$‰π‰ώ‰$‰Η‰ΛΊ:   ‰Ψθόÿÿÿ…ΐyƒ|$…¬  ‰ω‰χ‹$‰Ξ!ψ‰ω8Θu6kνL‹‹|  ‹‹F#T)#D)‰€=     ‰F…f  ƒΔ1ΐ[^_]Γt& Η    1ΐΗF    ƒΔ[^_]Γ΄&    f…ΐ  ‰Ηƒη…ÿÿÿCp‹«”   ‰$Ί?   ‰θθόÿÿÿƒÿ„πώÿÿ…ΐB  ƒΰ<…θώÿÿ΄&    f‹ƒ€  1νƒψ†κώÿÿƒθDƒψw
Ί£ΒrƒΔΈκÿÿÿ[^_]ΓΏ   ιΤώÿÿ¶    ‰Ψθ‰ύÿÿΈΘ   θόÿÿÿιPώÿÿΈκÿÿÿΓt& θόÿÿÿ98  „   UWV‰ΖS‰Σƒμ…Òt8Ίνÿÿÿƒϋu ƒΔ[^_]ι:ύÿÿ΄&    v Η†8      ƒΔ‰Π[^_]Γt& ‹Έ”   hpΊ   Ή   ‰ψθόÿÿÿ‰Β…ΐtΘƒϋ…Υ  ‰$UhX  θόÿÿÿXZ‹$λ΄΄&    v 1ΐΓ΄&    ¶    θόÿÿÿUWVSƒμ‰$ƒϊ^‡π   ‰Γƒω‡ε   Cp‹³”   ‰Υ1ÿ‰D$1Ò‰ι‰πθόÿÿÿ‰Β…ΐtƒÿ…φ  ƒΔ‰Π[^_]Γ΄&    ‹ƒ”   1φ{p‰D$‹D$Ί3   θόÿÿÿƒώt…ΐ  ‰ΑCp‰\$‹³”   ‰D$ƒα$1ÿ‰ΛΊ3   ‰Ω‰πθόÿÿÿ‰Β…ΐtƒÿ…;  ƒΔ‰Π[^_]Γf‰Ω‹$‹\$€=     ‰«4  ‰‹Μ  ‰«€  ‰ƒ„  „Pÿÿÿι_  t& Ίκÿÿÿι<ÿÿÿ΄&    ΄&    θόÿÿÿUWVS‰Γƒμ‹p‹x|‹@(=	 „Ξ   v\=	 „I  =	 …  ƒÿF°Η$    ‹nΤΙ‰D$ƒαύ‰|$ƒΑ‰ΟΊ   ‰ω‰θθόÿÿÿ‰Β…ΐ„z  ƒ<$t^ι¥  = 	 „¥   =	 …©  ‰ψΗ$    ‹nΤ¶ΘF°‰|$‰D$‰ΟΊ
   ‰ω‰θθόÿÿÿ‰Β…ΐ„  ƒ<$…@  t& €=     …  ƒΔ‰Π[^_]Γ‰ψΗ$    ‹nΤ¶ΘF°‰|$‰D$‰ΟΊ   ‰ω‰θθόÿÿÿ‰Β…ΐ„»   ƒ<$t®ι}  ‰ψΗ$    ‹nΤ¶ΘF°‰|$‰D$‰ΟΊ	   ‰ω‰θθόÿÿÿ‰Β…ΐ„–   ƒ<$„jÿÿÿιΝ  t& ÿ΄   „   ÿLÿÿÿ„    ‰ψ¶ΘF°Η$    ‹nΤ‰D$‰|$‰ΟΊ   ‰ω‰θθόÿÿÿ‰Β…ΐtIƒ<$„ÿÿÿιυ  ‹|$‰ΎΔ   ιωώÿÿ‹|$‰ΎΜ   ικώÿÿ‹|$‰~|ιήώÿÿ‹|$‰ΎΌ   ιΟώÿÿ‹|$‰ΎΤ   ιΐώÿÿ΄&    Ή   Ώ   ιfÿÿÿΉ€   Ώ€   ιVÿÿÿΊκÿÿÿιώÿÿ           °  @                  Ι                                                                                                                                ?                              	  €    
  €      €                                €      €      €            €           U           %                      @                          !      "     #      $     %      &      '      (  Μ   )      *      +      ,      -      .      /      0      1       2       3       4      5  ξ    6  ―    7  ÿ    8  ÿ    9                                                        0   `   @  `  °  €  `    `  p      
  °
  Π      No platform data
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
 ƒΕpUUh    θόÿÿÿƒΔι$  ƒΓh    S»κÿÿÿθόÿÿÿXZι$  ƒΕpUUh0   θόÿÿÿƒΔι$  ÿ4$ÿt$h   θόÿÿÿΈ
   ƒD$θόÿÿÿƒΔι­  VƒΖUhJ   θόÿÿÿΈ
   θόÿÿÿƒΔι#  ÿt$ÿt$h   θόÿÿÿΈ
   ƒD$θόÿÿÿƒΔιί  ƒΓ‰T$S‹D$ƒΐpPh   θόÿÿÿ‹T$ƒΔιώ  ÿt$hX   θόÿÿÿY[ƒΔ‰π[^_]Γ¶ΐ‰T$P¶D$P¶D$Pÿt$h|   θόÿÿÿ‹T$ ƒΔ€|$Qt/¶$P¶D$P‹t$VhΈ   θόÿÿÿVΎνÿÿÿhδ   θόÿÿÿƒΔλ—¶$ƒθF<wΖ¶ΒƒΗ‰ƒ4  ‹O‘  R·WώÒRPWÿt$h  θόÿÿÿ“0  ‰Ψθ\  ‰ΖƒΔ…ΐ…   €=     …”   Ηƒ8     ι)ÿÿÿVƒΖÿt$hJ   θόÿÿÿΈ
   θόÿÿÿƒΔιλ  ÿ4$ÿt$hJ   θόÿÿÿΈ
   ƒD$θόÿÿÿƒΔιΘ  UƒΕÿt$hJ   θόÿÿÿΈ
   θόÿÿÿƒΔι  ÿt$h4  θόÿÿÿXZι¤ώÿÿ1Ò‰Ψθ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ	   θ  ‰ΨΊ
   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ   θ  ‰ΨΊ    θ  ‰ΨΊ!   θ  ‰ΨΊ"   θ  ‰ΨΊ#   θ  ‰ΨΊ$   θ  ‰ΨΊ%   θ  ‰ΨΊ2   θ  ‰ΨΊ3   θ  ‰ΨΊ4   θ  ‰ΨΊ5   θ  ‰ΨΊ6   θ  ‰ΨΊ7   θ  ‰ΨΊ8   θ  Ί9   ‰Ψθ  ιrύÿÿWƒΗÿt$hJ   θόÿÿÿΈ
   θόÿÿÿƒΔιk	  «|  ƒΓpEPSh   θόÿÿÿ1ΐƒΔι;	  WƒΗÿt$hJ   θόÿÿÿΈ
   θόÿÿÿƒΔιP  ÿt$ÿt$hJ   θόÿÿÿΈ
   ƒD$θόÿÿÿƒΔιΚ  SƒΓUh   θόÿÿÿΈ
   θόÿÿÿƒΔι]
  WƒΗÿt$h   θόÿÿÿΈ
   θόÿÿÿƒΔιΰ
  VƒΖWhJ   θόÿÿÿΈ
   θόÿÿÿƒΔι  WƒΗÿt$h   θόÿÿÿΈ
   θόÿÿÿƒΔιS  ƒΓp‰$USh±   θόÿÿÿ‹T$ƒΔιϊ
  ÿ4$ÿt$h   θόÿÿÿΈ
   ƒD$θόÿÿÿƒΔιΨ  ÿ4$ÿt$h   θόÿÿÿΈ
   ƒD$θόÿÿÿƒΔι(  ÿ4$ÿt$h   θόÿÿÿΈ
   ƒD$θόÿÿÿƒΔι  ÿ4$ÿt$h   θόÿÿÿΈ
   ƒD$θόÿÿÿƒΔιx  ƒξP‰$ÿs|ÿs(Vh|  θόÿÿÿ‹T$ƒΔι±  ÿ4$ÿt$h   θόÿÿÿΈ
   ƒD$θόÿÿÿƒΔι~  θόÿÿÿΊ    Έ    ιόÿÿÿΈ    ιόÿÿÿ     tvp5146             @  tvp5146m2           x   tvp5147                 tvp5147m1           x                                                                θ      ι       κ  €    ΰ      θ  `    ι       κ  °    ΰ      θ      ι       κ       ΰ      θ  `    ι       κ  °    ΰ                                   θ      ι       κ  €    ΰ      θ  `    ι       κ  °    ΰ      ΰ                                                     ΐ      `          €  `                                           °
          `                          
          0                                   Π      Π  ΰ          °      NTSC                    ι  0u                    Π  @        ÿ       PAL                           q                                     8-bit UYVY 4:2:2 Format         UYVY                debug license=GPL description=TVP514X linux decoder driver author=Texas Instruments parm=debug:Debug level (0-1) parmtype=debug:bool    @          ¤ÿ      GCC: (GNU) 11.2.0           GNU  ΐ        ΐ                                  ρÿ                                                 %     $   0   %     =   `   Φ     T   `       e   @       t   `  G                                	              
 ‹   °  Ζ    ™   €        ²      @     Γ            Ο   P       ΰ       H    
 σ   €  Φ     
  `  ―       H   (    
 5    J     >  p   !    
 L  `      ^           Z  ‘   O    
 m  p  ¬     …  ΰ   f   
 Ά     ά    ³  F  “    
 Ι   
  £     Ϊ  Ω  !    
 π  °
        ϊ  ‡    
   Π  I    (    λ    
               <           P      €     _      
     s      x                   ~  @  `     “  x        ©           Ύ     P     Π  ΐ  @     ΰ           χ     )       5        (  N        =  k        V           d  @       v                                       µ             Ξ             Ϋ             π                          $             6             N             i             q             z             ”             ›             °             Ι             Χ             λ           χ                   
           x     8              tvp514x.c tvp514x_g_frame_interval tvp514x_s_frame_interval tvp514x_set_pad_format tvp514x_std_list tvp514x_remove tvp514x_enum_mbus_code tvp514x_probe tvp514x_reg_list_default tvp514x_fmt_list tvp514x_ops tvp514x_ctrl_ops tvp514x_probe.cold tvp514x_get_pad_format tvp514x_write_regs tvp514x_write_regs.cold dump_reg dump_reg.cold tvp514x_s_std tvp514x_s_std.cold tvp514x_s_stream.part.0 tvp514x_s_stream.part.0.cold tvp514x_querystd tvp514x_querystd.cold tvp514x_s_stream tvp514x_s_stream.cold tvp514x_s_routing tvp514x_s_routing.cold tvp514x_s_ctrl tvp514x_s_ctrl.cold tvp514x_driver_init tvp514x_driver tvp514x_driver_exit tvp514x_id tvp5146_init_reg_seq tvp514xm_init_reg_seq tvp5147_init_reg_seq tvp514x_video_ops tvp514x_pad_ops __UNIQUE_ID_license270 __UNIQUE_ID_description269 __UNIQUE_ID_author268 __UNIQUE_ID_debug267 __UNIQUE_ID_debugtype266 __param_debug __param_str_debug __fentry__ v4l2_async_unregister_subdev v4l2_ctrl_handler_free __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init media_entity_pads_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup v4l2_async_register_subdev _printk _dev_err i2c_smbus_write_byte_data msleep msleep_interruptible i2c_smbus_read_byte_data __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tvp514x_id_device_table param_ops_bool     8  1   8  a   8     +  ¤   +  A  8  L  9  W  :  a  8  ±  8  Ϊ  ;  ό  <      ‚  +  –  +  ›    ¦    ³  +  ύ  =  $  >  D  ?  N  +  k  @  u  +  •  @    +  Ώ  @  Ι  +  μ  @  φ  +    @  ;  A  B  B  W  :    8  ª  +  Θ  +  a  8  Έ  E  Τ    Ω  C  τ  F    8  ,  H  9    >  C  a  8  ν  E  %    q  8  ª  H  Τ  H  χ  H  
    !  8  \  H  Φ  H  	    w	  H  ν	  F  
  8  n
  E  †
    ‹
  C  ±
  8  λ
  E  )  H  a  E  ‹    Ρ  8  6  E    E  ª    ζ  E  &  E  †  E  Μ    .    L    Ζ    T    ώ    2    –    ·    β                ε    	    T	    	    }
    ϊ
    6    p    ―    K         ±    ϋ    ?    ›                   h   +                                                 $     (     ,     0     4     8     <   &          C       &   D  7     <   C  P     U   C  d   G  v     {   C  …   G          C  ®   G  Λ     Π   C  ε     κ   C        C  ;    @  C  K    P  C        C      ­    Π    Υ  C  ί  G  σ    ψ  C    G      !  C  +  G  <    A  C  Q    ]    i    u            ™    ¥    ±    ½    Ι    Υ    α    ν    ω                )    5    A    M    Y    e    q    }    ‰    •    ΅    ­    Ή    Ε    Ρ    έ    ι    υ                %    1    =    O    T  C  ^  G  y    ~  C  •      C  ¤  G  Ή    Ύ  C  Ν  G  ί    δ  C  ξ  G        C    G  $    )  C  3  G  H    M  C  W  G  l    q  C  ‰      C    G  ±    ¶  C  Ε  G  Ω    ή  C  ν  G        C    G  /    4  C  L    Q  C  `  G       -     D     l          ¶     ά     η        3    f        ¬    Υ    φ        ;    _    }    ¥    Ν    υ        @    h       8          I     J          L     +  ,   +  D   +  \   +  ¬  +  Ό  +  Δ    Π    Τ                 (    0    4    X        +     I     O        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rodata.str1.4 .rel.text.unlikely .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @                       	   @       €(  @              )             `  €                  %   	   @       ΐ+                  /             ΰ                    8             ΰ  @                  4   	   @       ΰ+  €               E      2          Ρ                 T      2       τ                  g             ‘  l                 c   	   @       `,  ¨     
         z             ύ                    v   	   @       1                  ‰               
                  …   	   @       (1                                 F                  ”   	   @       81  €                             f                    ­             θ                    ©   	   @       Έ1                  µ      0       ό                   Ύ                                   Ξ               (                                8        8         	              8#  G                               Ψ1  α                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              linux/rbtree_latch.h \
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
    $(wildcard include/config/KASAN