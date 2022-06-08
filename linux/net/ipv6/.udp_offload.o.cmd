e for the next lock_list
		 * entry, see the comments for the function.
		 */
		trace = entry->trace;

		if (depth == 0 && (entry != root)) {
			printk("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}

		entry = get_lock_parent(entry);
		depth--;
	} while (entry && (depth >= 0));
}

static void
print_irq_lock_scenario(struct lock_list *safe_entry,
			struct lock_list *unsafe_entry,
			struct lock_class *prev_class,
			struct lock_class *next_class)
{
	struct lock_class *safe_class = safe_entry->class;
	struct lock_class *unsafe_class = unsafe_entry->class;
	struct lock_class *middle_class = prev_class;

	if (middle_class == safe_class)
		middle_class = next_class;

	/*
	 * A direct locking problem where unsafe_class lock is taken
	 * directly by safe_class lock, then all we need to show
	 * is the deadlock scenario, as it is obvious that the
	 * unsafe lock is taken under the safe lock.
	 *
	 * But if there is a chain instead, where the safe lock takes
	 * an intermediate lock (middle_class) where this lock is
	 * not the same as the safe lock, then the lock chain is
	 * used to describe the problem. Otherwise we would need
	 * to show a different CPU case for each link in the chain
	 * from the safe_class lock to the unsafe_class lock.
	 */
	if (middle_class != unsafe_class) {
		printk("Chain exists of:\n  ");
		__print_lock_name(safe_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(middle_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(unsafe_class);
		printk(KERN_CONT "\n\n");
	}

	printk(" Possible interrupt unsafe locking scenario:\n\n");
	printk("       CPU0                    CPU1\n");
	printk("       ----                    ----\n");
	printk("  lock(");
	__print_lock_name(unsafe_class);
	printk(KERN_CONT ");\n");
	printk("                               local_irq_disable();\n");
	printk("                               lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("                               lock(");
	__print_lock_name(middle_class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_bad_irq_dependency(struct task_struct *curr,
			 struct lock_list *prev_root,
			 struct lock_list *next_root,
			 struct lock_list *backwards_entry,
			 struct lock_list *forwards_entry,
			 struct held_lock *prev,
			 struct held_lock *next,
			 enum lock_usage_bit bit1,
			 enum lock_usage_bit bit2,
			 const char *irqclass)
{
	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================================\n");
	pr_warn("WARNING: %s-safe -> %s-unsafe lock order detected\n",
		irqclass, irqclass);
	print_kernel_ident();
	pr_warn("-----------------------------------------------------\n");
	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] is trying to acquire:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		curr->softirq_context, softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		curr->softirqs_enabled);
	print_lock(next);

	pr_warn("\nand this task is already holding:\n");
	print_lock(prev);
	pr_warn("which would create a new lock dependency:\n");
	print_lock_name(hlock_class(prev));
	pr_cont(" ->");
	print_lock_name(hlock_class(next));
	pr_cont("\n");

	pr_warn("\nbut this new dependency connects a %s-irq-safe lock:\n",
		irqclass);
	print_lock_name(backwards_entry->class);
	pr_warn("\n... which became %s-irq-safe at:\n", irqclass);

	print_lock_trace(backwards_entry->class->usage_traces[bit1], 1);

	pr_warn("\nto a %s-irq-unsafe lock:\n", irqclass);
	print_lock_name(forwards_entry->class);
	pr_warn("\n... which became %s-irq-unsafe at:\n", irqclass);
	pr_warn("...");

	print_lock_trace(forwards_entry->class->usage_traces[bit2], 1);

	pr_warn("\nother info that might help us debug this:\n\n");
	print_irq_lock_scenario(backwards_entry, forwards_entry,
				hlock_class(prev), hlock_class(next));

	lockdep_print_held_locks(curr);

	pr_warn("\nthe dependencies between %s-irq-safe lock and the holding lock:\n", irqclass);
	print_shortest_lock_dependencies_backwards(backwards_entry, prev_root);

	pr_warn("\nthe dependencies between the lock to be acquired");
	pr_warn(" and %s-irq-unsafe lock:\n", irqclass);
	next_root->trace = save_trace();
	if (!next_root->trace)
		return;
	print_shortest_lock_dependencies(forwards_entry, next_root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static const char *state_names[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE),
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static const char *state_rnames[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE)"-READ",
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline const char *state_name(enum lock_usage_bit bit)
{
	if (bit & LOCK_USAGE_READ_MASK)
		return state_rnames[bit >> LOCK_USAGE_DIR_MASK];
	else
		return state_names[bit >> LOCK_USAGE_DIR_MASK];
}

/*
 * The bit number is encoded like:
 *
 *  bit0: 0 exclusive, 1 read lock
 *  bit1: 0 used in irq, 1 irq enabled
 *  bit2-n: state
 */
static int exclusive_bit(int new_bit)
{
	int state = new_bit & LOCK_USAGE_STATE_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * keep state, bit flip the direction and strip read.
	 */
	return state | (dir ^ LOCK_USAGE_DIR_MASK);
}

/*
 * Observe that when given a bitmask where each bitnr is encoded as above, a
 * right shift of the mask transforms the individual bitnrs as -1 and
 * conversely, a left shift transforms into +1 for the individual bitnrs.
 *
 * So for all bits whose number have LOCK_ENABLED_* set (bitnr1 == 1), we can
 * create the mask with those bit numbers using LOCK_USED_IN_* (bitnr1 == 0)
 * instead by subtracting the bit number by 2, or shifting the mask right by 2.
 *
 * Similarly, bitnr1 == 0 becomes bitnr1 == 1 by adding 2, or shifting left 2.
 *
 * So split the mask (note that LOCKF_ENABLED_IRQ_ALL|LOCKF_USED_IN_IRQ_ALL is
 * all bits set) and recompose with bitnr1 flipped.
 */
static unsigned long invert_dir_mask(unsigned long mask)
{
	unsigned long excl = 0;

	/* Invert dir */
	excl |= (mask & LOCKF_ENABLED_IRQ_ALL) >> LOCK_USAGE_DIR_MASK;
	excl |= (mask & LOCKF_USED_IN_IRQ_ALL) << LOCK_USAGE_DIR_MASK;

	return excl;
}

/*
 * Note that a LOCK_ENABLED_IRQ_*_READ usage and a LOCK_USED_IN_IRQ_*_READ
 * usage may cause deadlock too, for example:
 *
 * P1				P2
 * <irq disabled>
 * write_lock(l1);		<irq enabled>
 *				read_lock(l2);
 * write_lock(l2);
 * 				<in irq>
 * 				read_lock(l1);
 *
 * , in above case, l1 will be marked as LOCK_USED_IN_IRQ_HARDIRQ_READ and l2
 * will marked as LOCK_ENABLE_IRQ_HARDIRQ_READ, and this is a possible
 * deadlock.
 *
 * In fact, all of the following cases may cause deadlocks:
 *
 * 	 LOCK_USED_IN_IRQ_* -> LOCK_ENABLED_IRQ_*
 * 	 LOCK_USED_IN_IRQ_*_READ -> LOCK_ENABLED_IRQ_*
 * 	 LOCK_USED_IN_IRQ_* -> LOCK_ENABLED_IRQ_*_READ
 * 	 LOCK_USED_IN_IRQ_*_READ -> LOCK_ENABLED_IRQ_*_READ
 *
 * As a result, to calculate the "exclusive mask", first we invert the
 * direction (USED_IN/ENABLED) of the original mask, and 1) for all bits with
 * bitnr0 set (LOCK_*_READ), add those with bitnr0 cleared (LOCK_*). 2) for all
 * bits with bitnr0 cleared (LOCK_*_READ), add those with bitnr0 set (LOCK_*).
 */
static unsigned long exclusive_mask(unsigned long mask)
{
	unsigned long excl = invert_dir_mask(mask);

	excl |= (excl & LOCKF_IRQ_READ) >> LOCK_USAGE_READ_MASK;
	excl |= (excl & LOCKF_IRQ) << LOCK_USAGE_READ_MASK;

	return excl;
}

/*
 * Retrieve the _possible_ original mask to which @mask is
 * exclusive. Ie: this is the opposite of exclusive_mask().
 * Note that 2 possible original bits can match an exclusive
 * bit: one has LOCK_USAGE_READ_MASK set, the other has it
 * cleared. So both are returned for each exclusive bit.
 */
static unsigned long original_mask(unsigned long mask)
{
	unsigned long excl = invert_dir_mask(mask);

	/* Include read in existing usages */
	excl |= (excl & LOCKF_IRQ_READ) >> LOCK_USAGE_READ_MASK;
	excl |= (excl & LOCKF_  ƒ|$`"‡  ‹D$`Ύ€    ‰D$…ΐ  „   έ    ‰\$q‰L$$‰σ‰t$‹4$‰υ‹vhέ…φtz8  ‰l$ ‰D$‹D$‹l†‰κƒβuZ‹D$‹D$‹DΖ9Θuλ>΄&    ƒΒ‹ 9Αt‹X…Ϋuπ‹X…Ϋtμ‹[…Ϋuβ‹ 9Αuεt& ƒϊ‡'  „(  ‹vh…φu—‹l$ ‹4$‹\$ή‰D$‹€4  9Εt‹D$9„μ  …  ‹D$‹4  9έ„C  ‰Ψ1Òλt& ƒΒ‹ 9Εt‹p…φuπ‹H…Ιtμ‹I…Ιuβ‹ 9Εuεt& ƒϊ?‡1  φD$d„  ‹L$‹T$‰Ψ΄&    v …ÿt9xu9Ο…·   …Òt	;P„ª   ‹ 9Εuά‹T$‹D$…ÒuιΥ   v ‹9έ„ª  ;Cuρϋ πÿÿ‡Ξ  ‰ω…ÿ„λ  ‹$‰\$‰Λ1φ‹€”   ‹P@‹@<‰D$(‹D$`‰T$,‰D$0‹C(‹„°x  …ΐt1ΙT$(θόÿÿÿ‰D΄4…ΐ„w  ƒώtj‹C(Ύ   ‹„°x  …ΐuΡλζt& Ύκÿÿÿ‹D$Dd+    …>  ƒΔH‰π[^_]ΓφD$d…¬  t& ‹D$‹€4  9Ε…=ÿÿÿ1ΫιBÿÿÿ΄&    ‹\$…Ϋtu‹D$‹k‰{‹L$‹|$‰C‹D$4‰ϊ‰C‹D$8‰C‹$‰Έμ  θόυÿÿ‰Ζ…ΐ…ϊ   …ν„   ‰θθόÿÿÿ‹<$‹L$`1φ‹D$<‰ϊθόÿÿÿ‹L$`‹D$@‰ϊθόÿÿÿι8ÿÿÿ΅   Ή   Ίΐ  θόÿÿÿ‰Γ…ΐ„C  ‹t$‹L$‰+‹†8  ‰8  ‰C‰‹D$‰{‹|$‰C‹D$4‰ϊ‰C‹D$8‰C‹$‰Έμ  θUυÿÿ‰Ζ…ΐ…²   ‹D$$ΰϋÿÿθόÿÿÿιUÿÿÿ΄&    ‰ς‰Ψθόÿÿÿ‰D΄4= πÿÿ‡Ύ   ‰D΄<ιhώÿÿ΄&    ‹D$‹Hι	ώÿÿt& …νt[‰k‹D$<ΗC    θόÿÿÿ‹D$@θόÿÿÿιPώÿÿv ιΨόÿÿ‰θ‹l$ ¨…ΥόÿÿΎÿÿÿÿι.ώÿÿ‹D$Ύώÿÿÿ…ΐ…ώÿÿ1Ϋιύÿÿ‹D$<θόÿÿÿ‹D$@θόÿÿÿ‹C‹‰B‰‰ΨΗ   ΗC"  θόÿÿÿιαύÿÿΎώÿÿÿιΧύÿÿ‹D$<Ύτÿÿÿθόÿÿÿ‹D$@θόÿÿÿι»ύÿÿΎωÿÿÿι±ύÿÿθόÿÿÿ‰ήι¥ύÿÿ΄&    θόÿÿÿS‰Γφ@,tƒT  1Ò[ιόÿÿÿt& θόÿÿÿ‹C¨udÿ θόÿÿÿƒT  1Ò[ιόÿÿÿ¶    ‹Cπƒ λή΄&    ΄&    θόÿÿÿUΉ   ΊΠ  WV‰ΖSƒμdd΅    ‰D$`1ΐl$‰οσ«†T  ‰$hΐ  θόÿÿÿZ‰Η…ΐ…¶   ‹^h…Ϋt"θόÿÿÿ‹ƒT  ¨…μ   dÿ θόÿÿÿ‹[h…Ϋuή†4  μ  ‰ ‰@ƒΐ9Αuτ†H  ‰†H  ‰†L  λƒΗƒÿtxL½ ‰ϊ‰π‰λθΙρÿÿ…ΐtεl$`‹ƒΓθόÿÿÿ9λuς‹^h…Ϋtθόÿÿÿ‹ƒT  ¨u{dÿθόÿÿÿ‹[h…Ϋuβ‹$Ώτÿÿÿθόÿÿÿ‹D$`d+    u{ƒΔd‰ψ[^_]Γ΄&    f1Ϋ¶    ‹L ‹„Ψ  “τ   ƒΓ‰L–θόÿÿÿƒϋuή1ÿλ²f‹ƒX  πƒ ιÿÿÿ‹ƒX  πƒ(…xÿÿÿ‹“X  ƒT  ‹Rθόÿÿÿι_ÿÿÿθόÿÿÿv θόÿÿÿU‰ΥW‰ΟVS‰Γƒμ‹ θόÿÿÿ‰Ζ= πÿÿ‡Κ   ‹S‰ΠƒΰƒψtEΈ    ‹[‰$θόÿÿÿ‹$1Ι‰πR‰ϊSj θ›ψÿÿ‰ΓΈ    θόÿÿÿƒΔφF,thƒΔ‰Ψ[^_]Γt& ‹C‰κ1Ιθόÿÿÿ‰Ε= πÿÿws‹SΈ    ‹[‰$θόÿÿÿ‹$‰ι‰πR‰ϊSj θ>ψÿÿ‰ΓΈ    θόÿÿÿƒΔ…νt‰θθόÿÿÿφF,uθόÿÿÿ‹F¨uRdÿθόÿÿÿ‰ΨƒΔ[^_]ΓƒΔ‰Γ‰Ψ[^_]Γt& φF,t
‰λι]ÿÿÿv θόÿÿÿ‹F¨u-dÿθόÿÿÿ‰λι?ÿÿÿt& ‹Fπƒ(u¨‹VF‹Rθόÿÿÿλ‹Fπƒ(uΝ‹VF‹Rθόÿÿÿλ½΄&    ΄&    θόÿÿÿW‰ΧV‰ΖS‹ θόÿÿÿ‰Γ= πÿÿwt‹F‰ϊ1Ιθόÿÿÿ‹v‰ΗΈ    ÿ πÿÿwfθόÿÿÿ‰ϊ1Ι‰ΨVθ…ρÿÿ‰ΖΈ    θόÿÿÿZ…ÿt‰ψθόÿÿÿφC,t‰π[^_Γθόÿÿÿ‹C¨uDdÿθόÿÿÿ‰π[^_Γ¶    ‰Ζ[‰π^_Γ΄&    θόÿÿÿ1Ι1Ò‰ΨVθρÿÿ‰ΖΈ    θόÿÿÿXλ£‹Cπƒ(u¶‹SC‹Rθόÿÿÿλ¦΄&    θόÿÿÿUWV‰ΖSƒμ‹Nd΅    ‰D$1ΐ…Ι…σ   ‹F‰Υθόÿÿÿ‰Η= πÿÿ‡Τ   ΅   Ή0   Ίΐ θόÿÿÿ‰Γ…ΐ„ί   UΉ\  Ί   θόÿÿÿ‹F‰{(T$‰C,‰Ψθόÿÿÿ‰ΖX…φuS‹s,Έ    θόÿÿÿ1Ι1Ò‰ψjVSθÿυÿÿ‰ΖΈ    θόÿÿÿƒΔ‰ΰ…φuJθόÿÿÿ‰Ζ‹D$d+    uvƒΔ‰π[^_]Γt& ‰ΨθόÿÿÿφG,uΨθόÿÿÿ‹G¨u/dÿθόÿÿÿλΒt& θόÿÿÿλΨ΄&    f‰Ζλ©t& Ύκÿÿÿλ‹Gπƒ(uΛ‹WG‹Rθόÿÿÿλ»Ύτÿÿÿλθόÿÿÿ΄&    t& θόÿÿÿUWV‰ΦS‰Γƒμ d΅    ‰D$‹θόÿÿÿ‰Η= πÿÿ‡ς  Έ    θόÿÿÿ‹CΗ$κÿÿÿƒψ"‡<  Ύ€    …ΐ-  „   ‹”‡μ  ‹k‰L$Η4  ‰L$‹‡Ψ  ‹C‰T$‰L$ƒΰ„D  ‰Θθόÿÿÿ‰D$Ή   FT$θόÿÿÿ…ΐ…¬  FΉ   T$θόÿÿÿ…ΐ…“  ‹C‰ξ…ΐ„Ά   …ν„   ‹L$‰$…Ι„’   9ΘƒC  ‰D$Η$δÿÿÿφC…~  ‹D$1ν‹\Η‹D$9Ψtc‰|$‰Ηλ@t& ‹@(Ή   T$‹@ ‰D$‰πθόÿÿÿ…ΐ…  ƒΕ;l$„Ρ   ‹ƒΖ9ί„Δ   ‹C…ΐu½‹C…ΐt¶‹@λ±Η$    Έ    θόÿÿÿφG,t!‹D$d+    …ÿ   ‹$ƒΔ [^_]Γt& θόÿÿÿ‹G¨…¨   dÿθόÿÿÿλΕ¶    ‹L$‹TΟ‹L$9Ρth‰$‰Λλ
ƒΐ‹9Σt‹J…Ιuπ‹J…Ιtμ‹I…Ιuβλγ‹$ιƒώÿÿ΄&    ‰$ιoÿÿÿ΄&    ‹|$ιNÿÿÿ΄&    Η$    ιΌώÿÿt& 1ΐιDώÿÿ΄&    f‹|$Η$ςÿÿÿιÿÿÿ‹Gπƒ(…Nÿÿÿ‹WG‹Rθόÿÿÿι;ÿÿÿ‹L$‹D$‰κθόÿÿÿ‰$ιΰώÿÿθόÿÿÿt& θόÿÿÿUWVS‰ΓΏΫƒμ d΅    ‰D$1ΐΏD$4‰L$‰T$Αΰ	Ψ‰D$d‹    θόÿÿÿ‹ƒΘ  ‹T$8ΗD$    ‹@8‹΄Ψ  θόÿÿÿθόÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ΫtB~t& ‰|$>t& ‹K$S0D$θόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹\$θόÿÿÿ‹D$d+    ukƒΔ ‰Ψ[^_]Γt& θόÿÿÿS0‹s$‰ΕD$θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „KÿÿÿϋιEÿÿÿθόÿÿÿ΄&    ΄&    θόÿÿÿU‰ΕWV‰ΞΉ	   S‰ΣƒμP‹D$d|$(‰D$‹D$h‰D$d΅    ‰D$L1ΐ‰T$$Ίΐ  ‰l$ ΗD$    ΗD$    σ«‹D$lΗD$,   ‰t$<‹L$`‰D$@Έ   θόÿÿÿ‰D$(…ΐ„4  ‰ΑD$1ÒPD$0P‰Ψ‹{θόÿÿÿ[_…ΐ…  …φt‹D$‹…Ιt‹D$‹…Ò…  θόÿÿÿ‹T$pd‹    ‹ƒΘ  ΗD$    ‹@8‹΄Ψ  θόÿÿÿθόÿÿÿ‹ƒp  ~‰D$D$‰ƒp  ‹^…Ϋt@¶    ‰|$>t& ‹K$S0D$ θόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹\$θόÿÿÿ‹D$(θόÿÿÿƒϋu‹D$8…ΐ…™   ‹D$0θόÿÿÿ‹D$Ld+    …7  ƒΔP‰Ψ[^_]Γt& ΗD$,    ELF                            4     (               θόÿÿÿΗ€ΐ      1ΐΓ΄&    ΄&    θόÿÿÿΗ€ΐ       1ΐΓ΄&    ΄&    θόÿÿÿ‹ΐ   …Ιu‹€Δ   ΗB(p   ΗB4   ‰B81ΐΓ΄&    θόÿÿÿ‹@\θόÿÿÿ1ΐΓθόÿÿÿV1Ι‰ΖS‰Σƒμd΅    ‰D$‹†”   f‰L$ΖD$ƒϊtAƒϊtLƒϊtGj Ή   T$θόÿÿÿYƒψ…-   ‰Δ   ‹D$d+    u!ƒΔ[^Γt& ΖD$λΒ΄&    fΖD$λ²θόÿÿÿt& θόÿÿÿ‹ΐ   …Ιt	1ΐΓ¶    ‹R8θPÿÿÿ1ΐΓ΄&    ¶    θόÿÿÿVS‰Γ‹@‹P‹Rθόÿÿÿ%   =   „B   Έϋÿÿÿ[^Γ                     @  p                   "                                                                                                           @   p       €     @       (Radio)  6%s: Audio Mode: %u%s
 vp27smpx θόÿÿÿƒΈΐ    Ή	   Ί    DΡƒΐpRÿpTPh
   θόÿÿÿ1ΐƒΔΓƒΖpVh    θόÿÿÿXZιΥ   ‹S·C  Q QP‹CTÿ²   ÿ0h$   θόÿÿÿCΉΐ  ΊΘ   θόÿÿÿ‰ΖƒΔ…ΐu
Έτÿÿÿιh  Ή@   ‰Ϊθόÿÿÿ‰πΊ   Η†Δ      θ|   1ΐιh   3%s: I/O error setting audmode
    6%s %d-%04x: chip found @ 0x%x (%s)
 θόÿÿÿΊ    Έ    ιόÿÿÿΈ    ιόÿÿÿ            vp27smpx                                                           ΐ       `                                                                                                                                       @                                                                                         license=GPL author=Hans Verkuil description=vp27smpx driver  GCC: (GNU) 11.2.0           GNU  ΐ       ΐ                                  ρÿ                                                    ,   @   )     =   p                                   	 M       1    	               a   €        v   1       	      #     ΅   @  /     °   F   v    	 Δ   @                       Ρ            ζ       €     φ       
           0                        0     )  ΐ   ,     <  `   P     O           f          |           —             Ά             ΐ             Θ             Ϊ             τ                                       +             @             N             b           n             }      
           0      vp27smpx.c vp27smpx_s_radio vp27smpx_s_std vp27smpx_g_tuner vp27smpx_remove vp27smpx_log_status vp27smpx_set_audmode vp27smpx_set_audmode.cold vp27smpx_s_tuner vp27smpx_probe vp27smpx_probe.cold vp27smpx_ops vp27smpx_driver_init vp27smpx_driver vp27smpx_driver_exit vp27smpx_id vp27smpx_core_ops vp27smpx_tuner_ops vp27smpx_video_ops __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev _printk __stack_chk_guard i2c_transfer_buffer_flags __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vp27smpx_id_device_table      !     A     q     y          ’   !  Ε   "  ΰ   !    #      A    S  $  Ο   	  c  	                 h                             	                                      "     '      6     ;      e     j      |   %  ”     ›   &  ±     B          Έ                  '     (          *  @     D     L     l     Δ     Τ     Ψ        	   .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .comment .note.GNU-stack .note.gnu.property                                                         @   o                    	   @       θ
  x               )             ΐ  €                   %   	   @       `                  /             @                     8             @  $                  4   	   @       €  H               E      2       d  +                 X               Ό                  T   	   @       Θ  €      	         g      2       L  J                 z             –                    v   	   @       H                  ‰             ª  
                  …   	   @       h                              ΐ  0                  ”   	   @       x  @                             π  <                  ©      0       ,                   ²              ?                     Β             @  (                                h  Π              	              8  °                               Έ  Υ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ιαώÿÿv θόÿÿÿS0‹s$‰ΕD$ θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „ÿÿÿϋιÿÿÿv ‹|$‹θόÿÿÿ‹D$0‹L$‰‹D$4‰ιPÿÿÿ΄&    ‹L$`Ίΐ  Έ   θόÿÿÿΉ   ‰D$0‰Β‹D$‹ 9ΘGΑ‰D$4…ÒtR‹L$‹1ƒψs$…ΐ„.ώÿÿ¶
¨„!ώÿÿ·Lώf‰Lώιώÿÿ‹zƒηό‰
‹Lό‰Lό)ϊ)ΦΑισ¥ιούÿÿΗD$4    ιβύÿÿθόÿÿÿ΄&    t& θόÿÿÿUWV‰ΖSƒμp‹¶δ  ‰D$‹„$   ‰T$‹$„   1Ò‰D$‹„$   ‰L$‰D$d΅    ‰D$l1ΐ‰TLƒΐƒψ rτ1Ò1ΐ‰T,ƒΐƒψ rτ‹D$‹|$L$L‰D$(‹D$‹‹ ‰D$4‹D$‹ ‰D$8Έ   9ΒLΠD$(θήÿÿ‰D$…ΐxN‹?‹l$,9Η‰|$<Oψ…ÿρ  ‰ϊ1Ι‰θθόÿÿÿ‰ω‰Ϊ‰θθόÿÿÿ‰Η…ÿt=‹l$,ΗD$ςÿÿÿD$L9Εt‰θθόÿÿÿ‹D$ld+    …ν  ‹D$ƒΔp[^_]Γt& ‹D$1Òθόÿÿÿ‹  ΗD$$    θόÿÿÿ{θόÿÿÿd΅    ‹p  ‰L$L$ ‰p  ‹[…Ϋt;‰|$ >t& ‹K$S0D$(θόÿÿÿ‰Ζ…φu|$$ πÿÿwΗD$$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹\$$‹D$θόÿÿÿ…Ϋ…Η   ‹D$<‹l$,ƒψÿ„Ζ   ÿώÿÿ;D$υώÿÿ‹T$4‹L$‰‹T$8‹L$‰…ΐ„±   ‹L$‰L$L9Ν„¬   ‹D$ΗD$    ‰(ιΛώÿÿt& θόÿÿÿS0‹s$‰ΕD$(θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „σώÿÿϋινώÿÿv ‰\$‹l$,ιJώÿÿv ΗD$   ι:ώÿÿv ι ώÿÿΗD$    ι#ώÿÿΊΐ θόÿÿÿ‰Γ…ΐt%‹L$<‹T$,θόÿÿÿ‹D$ΗD$    ‰ιώÿÿθόÿÿÿΗD$τÿÿÿ‹l$,ιέύÿÿ¶    θόÿÿÿUWV‰ΖSƒμh‹δ  1φ‰D$‹D$|‹Ό$   ‰D$‹„$€   ‰D$d΅    ‰D$d1ΐ‰tDƒΐƒψ rτ1φ1ΐ‰t$ƒΐƒψ rτ‹D$‰T$,‰L$0L$D‰D$ d΅    ‰D$8‹„$„   ‰D$4‰ΒD$ θΫÿÿ‰D$…ΐX  …ÿuL‹D$θόÿÿÿ‰T$4…Òc  …ΐ…[  ‹D$‰Φ‹l$$9ΒOπ1Ι‰θ‰ςθόÿÿÿ‹T$‰ρ‰θθόÿÿÿ…ΐ…,  ‹D$1Òθόÿÿÿ‹›  ‰|$θόÿÿÿ{θόÿÿÿd΅    ‹p  ‰L$L$‰p  ‹[…Ϋt;‰|$>t& ‹K$S0D$ θόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹t$‹D$θόÿÿÿ…φχ   ‹\$4‹D$$;\$u…Ϋxq‰t$t6Ή   ‰Ϊ‰$θόÿÿÿ‹$‹D$‰Ωθόÿÿÿ…ΐuF‹D$4‹L$θόÿÿÿ…Ιu5‹D$$L$D9Θtθόÿÿÿ‹D$dd+    …”   ‹D$ƒΔh[^_]Γ΄&    ‹D$$ΗD$ςÿÿÿλΑ΄&    v θόÿÿÿS0‹s$‰ΕD$ θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „Γώÿÿϋι½ώÿÿv ‰t$ιJÿÿÿθόÿÿÿfθόÿÿÿUWV‰ΦS‰Γƒμ<‹|$T‹D$P‰|$d‹    ‰T$8‹“δ  ‰\$‹‰D$Ψ‰t$ ‰D$‹D$X‰L$$‰D$‹²  ‰\$(ΗD$0    d‹    ΗD$4    ‰\$,θόÿÿÿθόÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ΫtC~¶    ‰|$>t& ‹K$S0D$θόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΖ‹L$d΅    ‰p  θόÿÿÿθόÿÿÿ‹D$…ΐx‹L$‹T$(;€   …Òux‹T$8d+    uyƒΔ<[^_]Γv θόÿÿÿS0‹s$‰ΕD$θόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ1Ò)θCSƒCχ$   „;ÿÿÿϋι5ÿÿÿv ‰λ„Έςÿÿÿιzÿÿÿθόÿÿÿ     cgroup_bpf_enabled_key  __cgroup_bpf_run_filter_skb  __cgroup_bpf_run_filter_sk  __cgroup_bpf_run_filter_sock_addr  __cgroup_bpf_run_filter_sock_ops                                                                        @   π  0  P  `  p  Π     °  @  ΐ  π  `	  ΐ  €                   p  °  π  °  0  p  €    ΐ  ΰ       0  P$  °$  @&  °'  (  ΰ)  ,   .  π0   4   6  cgroup_id:	%llu
attach_type:	%d
      ΐ      Τ  @        €      ¤         -  -      /  Θ/      T2   3      45  06      D7  Θ7      include/linux/skbuff.h / kernel/bpf/cgroup.c include/linux/thread_info.h °      φ	  ­     @       ƒ	 #     Υ 	 3  -   ι 	Έ$   θόÿÿÿ ›  “  ή    ›$  &  &  s'  '  s(  ³)  S,   ÿÿÿ	
ÿÿÿÿÿÿÿÿÿ strnlen           P      0                                          	                                                               @                           	                                                            	                                                             	                                                             	         
                                   °                                             
                                                                                                               ΐ  `  ΰ        p                       GCC: (GNU) 11.2.0           GNU  ΐ       ΐ                                  ρÿ              
              +             N              o             •   4          ½              γ   5            P          /            
 T  Q          €  s          ®             Ϊ  t            •          2             ]  @   ª    w  0      •  P       ­  `  
     Ε  p  V     δ  Π  /     ϊ                           V     )  `  E    @  @  y                                 R  ΐ  Ό     a      
     u  $        €     i     ›         ²     ό     Ν     ϊ     ί     <     ώ  ΰ  <       €  <     -  @  <     P  ΐ   <     o     <     „     r     –     Ν    ©      #     ΄  °  w     Κ  0  7    β  p      ω  €  …        ΅     ,  ΐ       D  ΰ       [     
    q  `   <     ”  0      ¨  \                     ½           ϋ          :          r          «          X       Έ     Η   `	  W    9  °  6    Έ  π  ³    <  p  6    ί           ξ             ω                          π  3     2             ;             A             N             Y             f             p             ~             ‰             ›             ±             Κ             Φ             ο                                       (  °       A             H             O  ΐ  (     l  π  n     …             •             ¥             ·             Ζ             Ξ             Φ             δ  €  ™     ψ             	             '	             G	             e	             y	             ‹	             §	             Γ	             Ν	             ε	             ψ	             
             
             3
             G
             [
             t
             ‡
             
             »
             Σ
             μ
                                       .             D             \             k             ‚             –             ―  P$  Q     Β             ή  °$      ρ               @&  b                 +             A  °'  Ω     X  (  E    o             }                                       ­  ΰ)  ¬    Γ             η             Ω             υ  ,  b       .  ε    6             M             f  π0  
    ‰                          ­             Ύ             Λ   4      ξ             ϋ                6  ›    0  ,        D  0        \          o           †  Ό         Τ        cgroup.c __kstrtab_cgroup_bpf_enabled_key __kstrtabns_cgroup_bpf_enabled_key __ksymtab_cgroup_bpf_enabled_key __kstrtab___cgroup_bpf_run_filter_skb __kstrtabns___cgroup_bpf_run_filter_skb __ksymtab___cgroup_bpf_run_filter_skb __kstrtab___cgroup_bpf_run_filter_sk __kstrtabns___cgroup_bpf_run_filter_sk __ksymtab___cgroup_bpf_run_filter_sk __kstrtab___cgroup_bpf_run_filter_sock_addr __kstrtabns___cgroup_bpf_run_filter_sock_addr __ksymtab___cgroup_bpf_run_filter_sock_addr __kstrtab___cgroup_bpf_run_filter_sock_ops __kstrtabns___cgroup_bpf_run_filter_sock_ops __ksymtab___cgroup_bpf_run_filter_sock_ops sysctl_convert_ctx_access cg_sockopt_convert_ctx_access cg_sockopt_get_prologue bpf_cgroup_link_dealloc bpf_cgroup_link_fill_link_info cgroup_bpf_release_fn cgroup_bpf_release bpf_cgroup_link_show_fdinfo __bpf_prog_run_save_cb copy_sysctl_value sysctl_cpy_dir sysctl_cpy_dir.cold __func__.0 cgroup_dev_is_valid_access sysctl_is_valid_access cg_sockopt_is_valid_access sysctl_func_proto bpf_sysctl_get_new_value_proto bpf_set_retval_proto bpf_sysctl_get_name_proto bpf_sysctl_get_current_value_proto bpf_sysctl_set_new_value_proto bpf_get_retval_proto sockopt_alloc_buf cgroup_bpf_replace CSWTCH.197 cgroup_dev_func_proto compute_effective_progs update_effective_progs __cgroup_bpf_detach bpf_cgroup_link_release.part.0 bpf_cgroup_link_release bpf_cgroup_link_detach cg_sockopt_func_proto bpf_get_netns_cookie_sockopt_proto __cgroup_bpf_attach bpf_cgroup_link_lops __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_ops491 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_addr490 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sk489 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_skb488 __UNIQUE_ID___addressable_cgroup_bpf_enabled_key483 bpf_get_retval __fentry__ current_task bpf_set_retval bpf_get_netns_cookie_sockopt init_net kcmd_drivers/media/i2c/vp27smpx.o := gcc -Wp,-MMD,drivers/media/i2c/.vp27smpx.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"vp27smpx"' -DKBUILD_MODNAME='"vp27smpx"' -D__KBUILD_MODNAME=kmod_vp27smpx -c -o drivers/media/i2c/vp27smpx.o drivers/media/i2c/vp27smpx.c 

source_drivers/media/i2c/vp27smpx.o := drivers/media/i2c/vp27smpx.c

deps_drivers/media/i2c/vp27smpx.o := \
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
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
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
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
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
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
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
  include/vdso/math64.h \
  include/linux/time64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
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
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/ratelimit_types.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/msr.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/bug.h \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/COMPAT) \
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
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/osq_lock.h \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/debug_locks.h \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
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
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/generated/bounds.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
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
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
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
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
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
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  inclu