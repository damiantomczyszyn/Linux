atic inline int check_wait_context(struct task_struct *curr,
				     struct held_lock *next)
{
	return 0;
}

#endif /* CONFIG_PROVE_LOCKING */

/*
 * Initialize a lock instance's lock-class mapping info:
 */
void lockdep_init_map_type(struct lockdep_map *lock, const char *name,
			    struct lock_class_key *key, int subclass,
			    u8 inner, u8 outer, u8 lock_type)
{
	int i;

	for (i = 0; i < NR_LOCKDEP_CACHING_CLASSES; i++)
		lock->class_cache[i] = NULL;

#ifdef CONFIG_LOCK_STAT
	lock->cpu = raw_smp_processor_id();
#endif

	/*
	 * Can't be having no nameless bastards around this place!
	 */
	if (DEBUG_LOCKS_WARN_ON(!name)) {
		lock->name = "NULL";
		return;
	}

	lock->name = name;

	lock->wait_type_outer = outer;
	lock->wait_type_inner = inner;
	lock->lock_type = lock_type;

	/*
	 * No key, no joy, we need to hash something.
	 */
	if (DEBUG_LOCKS_WARN_ON(!key))
		return;
	/*
	 * Sanity check, the lock-class key must either have been allocated
	 * statically or must have been registered as a dynamic key.
	 */
	if (!static_obj(key) && !is_dynamic_key(key)) {
		if (debug_locks)
			printk(KERN_ERR "BUG: key %px has not been registered!\n", key);
		DEBUG_LOCKS_WARN_ON(1);
		return;
	}
	lock->key = key;

	if (unlikely(!debug_locks))
		return;

	if (subclass) {
		unsigned long flags;

		if (DEBUG_LOCKS_WARN_ON(!lockdep_enabled()))
			return;

		raw_local_irq_save(flags);
		lockdep_recursion_inc();
		register_lock_class(lock, subclass, 1);
		lockdep_recursion_finish();
		raw_local_irq_restore(flags);
	}
}
EXPORT_SYMBOL_GPL(lockdep_init_map_type);

struct lock_class_key __lockdep_no_validate__;
EXPORT_SYMBOL_GPL(__lockdep_no_validate__);

static void
print_lock_nested_lock_not_held(struct task_struct *curr,
				struct held_lock *hlock,
				unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("==================================\n");
	pr_warn("WARNING: Nested lock was not taken\n");
	print_kernel_ident();
	pr_warn("----------------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("\nbut this task is not holding:\n");
	pr_warn("%s\n", hlock->nest_lock->name);

	pr_warn("\nstack backtrace:\n");
	dump_stack();

	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static int __lock_is_held(const struct lockdep_map *lock, int read);

/*
 * This gets called for every mutex_lock*()/spin_lock*() operation.
 * We maintain the dependency maps and validate the locking attempt:
 *
 * The callers must make sure that IRQs are disabled before calling it,
 * otherwise we could get an interrupt which would want to take locks,
 * which would end up in lockdep again.
 */
static int __lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check, int hardirqs_off,
			  struct lockdep_map *nest_lock, unsigned long ip,
			  int references, int pin_count)
{
	struct task_struct *curr = current;
	struct lock_class *class = NULL;
	struct held_lock *hlock;
	unsigned int depth;
	int chain_head = 0;
	int class_idx;
	u64 chain_key;

	if (unlikely(!debug_locks))
		return 0;

	if (!prove_locking || lock->key == &__lockdep_no_validate__)
		check = 0;

	if (subclass < NR_LOCKDEP_CACHING_CLASSES)
		class = lock->class_cache[subclass];
	/*
	 * Not cached?
	 */
	if (unlikely(!class)) {
		class = register_lock_class(lock, subclass, 0);
		if (!class)
			return 0;
	}

	debug_class_ops_inc(class);

	if (very_verbose(class)) {
		printk("\nacquire class [%px] %s", class->key, class->name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		printk(KERN_CONT "\n");
		dump_stack();
	}

	/*
	 * Add the lock to the list of currently held locks.
	 * (we dont increase the depth just yet, up until the
	 * dependency checks are done)
	 */
	depth = curr->lockdep_depth;
	/*
	 * Ran out of static storage for our per-task lock stack again have we?
	 */
	if (DEBUG_LOCKS_WARN_ON(depth >= MAX_LOCK_DEPTH))
		return 0;

	class_idx = class - lock_classes;

	if (depth) { /* we're holding locks */
		hlock = curr->held_locks + depth - 1;
		if (hlock->class_idx == class_idx && nest_lock) {
			if (!references)
				references++;

			if (!hlock->references)
				hlock->references++;

			hlock->references += references;

			/* Overflow */
			if (DEBUG_LOCKS_WARN_ON(hlock->references < references))
				return 0;

			return 2;
		}
	}

	hlock = curr->held_locks + depth;
	/*
	 * Plain impossible, we just registered it and checked it weren't no
	 * NULL like.. I bet this mushroom I ate was good!
	 */
	if (DEBUG_LOCKS_WARN_ON(!class))
		return 0;
	hlock->class_idx = class_idx;
	hlock->acquire_ip = ip;
	hlock->instance = lock;
	hlock->nest_lock = nest_lock;
	hlock->irq_context = task_irq_context(curr);
	hlock->trylock = trylock;
	hlock->read = read;
	hlock->check = check;
	hlock->hardirqs_off = !!hardirqs_off;
	hlock->references = references;
#ifdef CONFIG_LOCK_STAT
	hlock->waittime_stamp = 0;
	hlock->holdtime_stamp = lockstat_clock();
#endif
	hlock->pin_count = pin_count;

	if (check_wait_context(curr, hlock))
		return 0;

	/* Initialize the lock usage bit */
	if (!mark_usage(curr, hlock, check))
		return 0;

	/*
	 * Calculate the chain hash: it's the combined hash of all the
	 * lock keys along the dependency chain. We save the hash value
	 * at every step so that we can get the current hash easily
	 * after unlock. The chain hash is then used to cache dependency
	 * results.
	 *
	 * The 'key ID' is what is the most compact key value to drive
	 * the hash, not class->key.
	 */
	/*
	 * Whoops, we did it again.. class_idx is invalid.
	 */
	if (DEBUG_LOCKS_WARN_ON(!test_bit(class_idx, lock_classes_in_use)))
		return 0;

	chain_key = curr->curr_chain_key;
	if (!depth) {
		/*
		 * How can we have a chain hash when we ain't got no keys?!
		 */
		if (DEBUG_LOCKS_WARN_ON(chain_key != INITIAL_CHAIN_KEY))
			return 0;
		chain_head = 1;
	}

	hlock->prev_chain_key = chain_key;
	if (separate_irq_context(curr, hlock)) {
		chain_key = INITIAL_CHAIN_KEY;
		chain_head = 1;
	}
	chain_key = iterate_chain_key(chain_key, hlock_id(hlock));

	if (nest_lock && !__lock_is_held(nest_lock, -1)) {
		print_lock_nested_lock_not_held(curr, hlock, ip);
		return 0;
	}

	if (!debug_locks_silent) {
		WARN_ON_ONCE(depth && !hlock_class(hlock - 1)->key);
		WARN_ON_ONCE(!hlock_class(hlock)->key);
	}

	if (!validate_chain(curr, hlock, chain_head, chain_key))
		return 0;

	curr->curr_chain_key = chain_key;
	curr->lockdep_depth++;
	check_chain_key(curr);
#ifdef CONFIG_DEBUG_LOCKDEP
	if (unlikely(!debug_locks))
		return 0;
#endif
	if (unlikely(curr->lockdep_depth >= MAX_LOCK_DEPTH)) {
		debug_locks_off();
		print_lockdep_off("BUG: MAX_LOCK_DEPTH too low!");
		printk(KERN_DEBUG "depth: %i  max: %lu!\n",
		       curr->lockdep_depth, MAX_LOCK_DEPTH);

		lockdep_print_held_locks(current);
		debug_show_all_locks();
		dump_stack();

		return 0;
	}

	if (unlikely(curr->lockdep_depth > max_lockdep_depth))
		max_lockdep_depth = curr->lockdep_depth;

	return 1;
}

static void print_unlock_imbalance_bug(struct task_struct *curr,
				       struct lockdep_map *lock,
				       unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================\n");
	pr_warn("WARNING: bad unlock balance detected!\n");
	print_kernel_ident();
	pr_warn("-------------------------------------\n");
	pr_warn("%s/%d is trying to release lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no more locks to release!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static noinstr int match_held_lock(const struct held_lock *hlock,
				   const struct lockdep_map *lock)
{
	if (hlock->instance == lock)
		return 1;

	if (hlock->references) {
		const struct lock_class *class = lock->class_cache[0];

		if (!class)
			class = look_up_lock_class(lock, 0);

		/*
		 * If look_up_lock_class() failed to find a class, we're trying
		 * to test if we hold a lock that has never yet been acquired.
		 * Clearly if the lock hasn't been acquired _ever_, we're not
		 * holding it either, so report failure.
		 */
		if (!class)
			return 0;

		/*
		 * References, but not a lock we're actually ref-counting?
		 * State got messed up, follow the sites that change ->references
		 * and try to make sense of it.
		 */
		if (DEBUG_LOCKS_WARN_ON(!hlock->nest_lock))
			return 0;

		if (hlock->class_idx == class - lock_classes)
			return 1;
	}

	return 0;
}

/* @depth must not be zero */
static struct held_lock *find_held_lock(struct task_struct *curr,
					struct lockdep_map *lock,
					unsigned int depth, int *idx)
{
	struct held_lock *ret, *hlock, *prev_hlock;
	int i;

	i = depth - 1;
	hlock = curr->held_locks + i;
	ret = hlock;
	if (match_held_lock(hlock, lock))
		goto out;

	ret = NULL;
	for (i--, prev_hlock = hlock--;
	     i >= 0;
	     i--, prev_hlock = hlock--) {
		/*
		 * We must not cross into another context:
		 */
		if (prev_hlock->irq_context != hlock->irq_context) {
			ret = NULL;
			break;
		}
		if (match_held_lock(hlock, lock)) {
			ret = hlock;
			break;
		}
	}

out:
	*idx = i;
	return ret;
}

static int reacquire_held_locks(struct task_struct *curr, unsigned int depth,
				int idx, unsigned int *merged)
{
	struct held_lock *hlock;
	int first_idx = idx;

	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return 0;

	for (hlock = curr->held_locks + idx; idx < depth; idx++, hlock++) {
		switch (__lock_acquire(hlock->instance,
				    hlock_class(hlock)->subclass,
				    hlock->trylock,
				    hlock->read, hlock->check,
				    hlock->hardirqs_off,
				    hlock->nest_lock, hlock->acquire_ip,
				    hlock->references, hlock->pin_count)) {
		case 0:
			return 1;
		case 1:
			break;
		case 2:
			*merged += (idx == first_idx);
			break;
		default:
			WARN_ON(1);
			return 0;
		}
	}
	return 0;
}

static int
__lock_set_class(struct lockdep_map *lock, const char *name,
		 struct lock_class_key *key, unsigned int subclass,
		 unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	struct lock_class *class;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	lockdep_init_map_waits(lock, name, key, 0,
			       lock->wait_type_inner,
			       lock->wait_type_outer);
	class = register_lock_class(lock, subclass, 0);
	hlock->class_idx = class - lock_classes;

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged))
		return 0;
	return 1;
}

static int __lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	WARN(hlock->read, "downgrading a read lock");
	hlock->read = 1;
	hlock->acquire_ip = ip;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/* Merging can't happen with unchanged classes.. */
	if (DEBUG_LOCKS_WARN_ON(merged))
		return 0;

	Ί    ‰ΠΓ΄&    Ί@  ‰ΠΓ΄&    Ίΐ   ‰ΠΓ΄&    Ί   ιIÿÿÿ¶    Ί    ι9ÿÿÿ¶    Ί    ι)ÿÿÿ¶    θόÿÿÿWVS…ÒxT‰Ζϊ   ‰Χ‰Σƒϊ ‰NΡ‰Σ‰N‰Ψ[^_Γ»   Ώ   Ίΐ ‰ψθόÿÿÿ‰F…ΐtψ‰F‰Ψ[^_Γ΄&    »κÿÿÿ‰Ψ[^_Γ»τÿÿÿλ·΄&    ΄&    θόÿÿÿUW‰ΟV‰ΦS‰ΓΈ    ƒμθόÿÿÿ‹C(‰$…ΐ„™  …ÿt	9{…‚  ‹C,ƒψ"‡Ν   Ύ€    ‰D$…ΐΊ   ‹<$„   Ε4  ‹n‰D$‰ϊ‰L$Β‹C9h…   ‹DΟ9Βuι!  f‹ 9Β„  ;Xuρ‰π‡C‹$‰D$1ΐθόÿÿÿ‰Ζ‹D$8  ‰D$…φuιΥ   ¶    ‹$‰πθόÿÿÿ‰Ζ…ΐ„»   †T  θόÿÿÿ„ΐuέ‰υ1Ι…Ιt5‹D$φD…u*‹mh…νuκ»κÿÿÿΈ    θόÿÿÿ‰ΨƒΔ[^_]Γ΄&    ‹D$| ‹D$‹DΕ9Ηuλΐ΄&    v 9Σt ƒΑ‹ 9Ηt©ƒx ‹Puκ…Òtνƒz tη9Σuΰ‹|$‹CIΑβ”ΎΨ  ‰B‹$‰πθόÿÿÿ‰Ζ…ΐ…Eÿÿÿ‹D$1Ϋθόÿÿÿιfÿÿÿ»ώÿÿÿι\ÿÿÿ»ÿÿÿÿιRÿÿÿ»½ÿÿÿιHÿÿÿv θόÿÿÿU‰ΥWVSƒμ‹€δ  d‹    ‰T$1ÒΗD$    ‹Ψ  θόÿÿÿθόÿÿÿ{d΅    ‹°p  ‰t$t$‰°p  ‹[…Ϋt:f‰|$>t& ‹K$S0‰θθόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΘ‹|$d΅    ‰Έp  θόÿÿÿθόÿÿÿ‹D$‹T$d+    unƒΔ[^_]Γt& θόÿÿÿS0‹s$‰D$‰θθόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ‹L$1Ò)ΘCSƒCχ$   „MÿÿÿϋιGÿÿÿθόÿÿÿ΄&    v θόÿÿÿU‰ΕWVSƒμd΅    ‰D$‹…δ  ΗD$    ‹Ψ  θόÿÿÿθόÿÿÿ{d΅    ‹°p  ‰t$t$‰°p  ‹[…Ϋt=t& ‰|$>t& ‹K$S0‰θθόÿÿÿ‰Ζ…φu|$ πÿÿwΗD$ÿÿÿÿ‹_ƒΗ…ΫuΘ‹|$d΅    ‰Έp  θόÿÿÿθόÿÿÿ‹D$‹T$d+    unƒΔ[^_]Γt& θόÿÿÿS0‹s$‰D$‰θθόÿÿÿ‹[‰Ζθόÿÿÿ…    $ϊƒCƒƒS θόÿÿÿ‹L$1Ò)ΘCSƒCχ$   „MÿÿÿϋιGÿÿÿθόÿÿÿ΄&    v θόÿÿÿUWV1φS‰Γ‰Π‰Κμ¬   d‹    ‰$¨   1Ι‹$ΐ   ΗD$    ΗD$     ‹¬$Δ   ‰L$$·K‰\$ƒαχ‰D$fƒω…«   …ΐ„.  ‹ƒδ  ΗD$    ‹Ψ  θόÿÿÿθόÿÿÿ{d΅    ‹p  ‰T$T$‰p  ‹s…φtBv ‰|$>t& ‹N$V0D$θόÿÿÿ‰ΓφΓu|$ πÿÿwΗD$ÿÿÿÿƒΗΡλ	] ‹7…φuΑ‹|$d΅    ‰Έp  θόÿÿÿθόÿÿÿ‹t$‹„$¨   d+    …   Δ¬   ‰π[^_]ΓfθόÿÿÿV0‹^$‰D$D$θόÿÿÿ‹v‰Γθόÿÿÿ4…    $ϊƒFƒƒV θόÿÿÿ‹L$1Ò)ΘFVƒFχ$   „=ÿÿÿϋι7ÿÿÿt& t$(Ή    ‰χ‰t$σ«ιΌώÿÿθόÿÿÿ΄&    ¶    θόÿÿÿƒψQtfwƒψtOΊ    ƒψPu%‰ΠΓf=Ί   t)=»   uΊΰ  ‰ΠΓ΄&    v Ί    ƒψtΡιόÿÿÿΊ   λΔ΄&    fΊ    λ΄΄&    fΊ    λ¤΄&    fθόÿÿÿU1νWV²„   S‰Γ‰Π8  ‰ίƒμ‰t$4Υ4  ‰T$‰L$‰4$‰D$‰θ…νt‹t$φD·tD‹$1Ò4‹D$‹DΗ9Ζuλ(ƒΒ‹ 9Ζt‹H…Ιuπ‹H…Ιtμ‹I…Ιuβ‹ 9Ζuεt& D ‰Ε‹h…ÿu¤Ίΐ  θόÿÿÿ‰Ζ…ΐ„‡   ‹D$1Ò8  ‰D$…Ò~$‹D$φDƒu‹[h…Ϋuκ‹D$‰01ΐƒΔ[^_]Γf‹D$‹<$‹DΓ,;9Εu'λΣt& <RƒΒ‰LΎR‹x‰9‹x‰y‹ 9θt®‹H…ΙuΨ‹H…Ιtμ‹I…ΙtελΘΈτÿÿÿλ΄&    fθόÿÿÿUW‰Χ‰ΒV‰Ζ1ΐSθόÿÿÿ…ΐt=‰Γt& ƒT  θόÿÿÿ„ΐu‹P  ‰ϊ‰Ψθ‚ώÿÿ‰Ε…ΐ…   ‰Ψ‰ςθόÿÿÿ‰Γ…ΐuΙ‰ς1ΐΗτ   θόÿÿÿ‰Γ…ΐuλVf‹ƒP  …ΐu,‰Ψ‰ςθόÿÿÿ‰Γ…ΐt;ƒT  θόÿÿÿ„ΐuΨ‹“P  ‹D»‰T»θόÿÿÿ‰Ψ‰ςΗƒP      θόÿÿÿ‰Γ…ΐuΕ1ν[‰θ^_]Γv ‰ς1ΐθόÿÿÿ‰Γ…ΐtη‹ƒP  θόÿÿÿ‰Ψ‰ςΗƒP      θόÿÿÿ‰Γ…ΐuά[‰θ^_]Γt& θόÿÿÿUWV‰Ζ‰ΠSƒμ‹T$(‰$ƒϊ"‡    ‰ΛΎ    …Ι   Ή„   ‘8  ‰|$Αη‰|$|>‰T$‹T–…ΐt…Ϋudƒβuo‹T$½ώÿÿÿ‹DΦ9ΗtS‹\Φϋ πÿÿ‡ό   ‹CΗC    ‰ΚΗC    ‰D$‰πθPώÿÿ‰Ε…ΐtz‹D$‰C‹$‰CƒΔ‰θ[^_]Γv ½κÿÿÿƒΔ‰θ[^_]Γ‹$½κÿÿÿ‰Σ	Γtη‹\$‹\ή9ίuλt& ‹9ίt;Cuυ;S„rÿÿÿ‹9ίuμƒΔ½ώÿÿÿ[‰θ^_]Γ΄&    ‹C‹‰A‰‰ΨΗ   ΗC"  θόÿÿÿ‹D$‹DΖ9Ηt&‹D$…ΐtθόÿÿÿ‹D$ΰϋÿÿθόÿÿÿ‰θƒΔ[^_]Γ‹D$ΗD†    λΜ‰έι@ÿÿÿ΄&    t& θόÿÿÿVS‰ΓΈ    θόÿÿÿ‹C(…ΐtVÿs,1Ò‰ΩθJώÿÿZ…ΐuU‹s(ΗC(    Έ    θόÿÿÿφF,t[^Γ΄&    θόÿÿÿ‹F¨u,dÿ[^ιόÿÿÿ΄&    v [Έ    ^ιόÿÿÿt& λ§t& ‹Fπƒ(uΞ‹VF‹RθόÿÿÿλΎ΄&    ΄&    θόÿÿÿ‹P(…Òtι?ÿÿÿ΄&    Γ΄&    θόÿÿÿ‹P(…Òtθÿÿÿ1ΐΓ΄&    t& θόÿÿÿƒψk„Β   wƒψ9t[Ή    ƒψ`u!‰ΘΓ¶    ƒψlt3ƒψzuVΉ`   ‰ΘΓ¶    ƒψ1u[ƒzΉ    Έ    EΘλΗ΄&    Ή    λΈ΄&    fƒzΉ    Έ    EΘλt& =Ί   tQ=»   u2Ήΰ  λ‚v ƒψQt[wαƒψtDΉ    ƒψP„eÿÿÿΉ    ƒψ„Wÿÿÿιόÿÿÿv Ή    ιEÿÿÿ¶    Ή   ι5ÿÿÿ¶    Ή    ι%ÿÿÿ¶    Ή    ιÿÿÿ¶    θόÿÿÿU½    W‰ΗVSƒμ‰D$@μ‰D$Έ    θόÿÿÿ‰ψΏΨώÿÿ-„  ‰D$΄&    ‹‹39ϋu'ιƒ   v ΗB(    ‰Ψ‰σθόÿÿÿ‰θθόÿÿÿ‹9ώtc‰Ζ‹C‰F‰0‹CΗ   ΗC"  …ΐtθόÿÿÿ‹S…Òtΐ‹J(φA,u°‰L$‰$θόÿÿÿ‹L$‹$‹A¨…Β   dÿ‰$θόÿÿÿ‹$λ‚f‹t$ƒΕƒΗ‹θόÿÿÿ‰πƒΐ‰D$ύΈ   …Jÿÿÿ‹D$‹|$‹@μ‹Xδqδ9D$t‰Ψθόÿÿÿ‰Ψ‰σθόÿÿÿ‹FpδC9ψuγΈ    θόÿÿÿ‹D$‹ϋÿÿ…Ϋtθόÿÿÿ‹ƒ