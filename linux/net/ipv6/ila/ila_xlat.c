st_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;
	int i, j;

	/*
	 * The caller must hold the graph lock, ensure we've got IRQs
	 * disabled to make this an IRQ-safe lock.. for recursion reasons
	 * lockdep won't complain about its own locking errors.
	 */
	if (lockdep_assert_locked())
		return 0;

	chain = alloc_lock_chain();
	if (!chain) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAINS too low!");
		dump_stack();
		return 0;
	}
	chain->chain_key = chain_key;
	chain->irq_context = hlock->irq_context;
	i = get_first_held_lock(curr, hlock);
	chain->depth = curr->lockdep_depth + 1 - i;

	BUILD_BUG_ON((1UL << 24) <= ARRAY_SIZE(chain_hlocks));
	BUILD_BUG_ON((1UL << 6)  <= ARRAY_SIZE(curr->held_locks));
	BUILD_BUG_ON((1UL << 8*sizeof(chain_hlocks[0])) <= ARRAY_SIZE(lock_classes));

	j = alloc_chain_hlocks(chain->depth);
	if (j < 0) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAIN_HLOCKS too low!");
		dump_stack();
		return 0;
	}

	chain->base = j;
	for (j = 0; j < chain->depth - 1; j++, i++) {
		int lock_id = hlock_id(curr->held_locks + i);

		chain_hlocks[chain->base + j] = lock_id;
	}
	chain_hlocks[chain->base + j] = hlock_id(hlock);
	hlist_add_head_rcu(&chain->entry, hash_head);
	debug_atomic_inc(chain_lookup_misses);
	inc_chains(chain->irq_context);

	return 1;
}

/*
 * Look up a dependency chain. Must be called with either the graph lock or
 * the RCU read lock held.
 */
static inline struct lock_chain *lookup_chain_cache(u64 chain_key)
{
	struct hlist_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;

	hlist_for_each_entry_rcu(chain, hash_head, entry) {
		if (READ_ONCE(chain->chain_key) == chain_key) {
			debug_atomic_inc(chain_lookup_hits);
			return chain;
		}
	}
	return NULL;
}

/*
 * If the key is not present yet in dependency chain cache then
 * add it and return 1 - in this case the new dependency chain is
 * validated. If the key is already hashed, return 0.
 * (On return with 1 graph_lock is held.)
 */
static inline int lookup_chain_cache_add(struct task_struct *curr,
					 struct held_lock *hlock,
					 u64 chain_key)
{
	struct lock_class *class = hlock_class(hlock);
	struct lock_chain *chain = lookup_chain_cache(chain_key);

	if (chain) {
cache_hit:
		if (!check_no_collision(curr, hlock, chain))
			return 0;

		if (very_verbose(class)) {
			printk("\nhash chain already cached, key: "
					"%016Lx tail class: [%px] %s\n",
					(unsigned long long)chain_key,
					class->key, class->name);
		}

		return 0;
	}

	if (very_verbose(class)) {
		printk("\nnew hash chain, key: %016Lx tail class: [%px] %s\n",
			(unsigned long long)chain_key, class->key, class->name);
	}

	if (!graph_lock())
		return 0;

	/*
	 * We have to walk the chain again locked - to avoid duplicates:
	 */
	chain = lookup_chain_cache(chain_key);
	if (chain) {
		graph_unlock();
		goto cache_hit;
	}

	if (!add_chain_cache(curr, hlock, chain_key))
		return 0;

	return 1;
}

static int validate_chain(struct task_struct *curr,
			  struct held_lock *hlock,
			  int chain_head, u64 chain_key)
{
	/*
	 * Trylock needs to maintain the stack of held locks, but it
	 * does not add new dependencies, because trylock can be done
	 * in any order.
	 *
	 * We look up the chain_key and do the O(N^2) check and update of
	 * the dependencies only if this is a new dependency chain.
	 * (If lookup_chain_cache_add() return with 1 it acquires
	 * graph_lock for us)
	 */
	if (!hlock->trylock && hlock->check &&
	    lookup_chain_cache_add(curr, hlock, chain_key)) {
		/*
		 * Check whether last held lock:
		 *
		 * - is irq-safe, if this lock is irq-unsafe
		 * - is softirq-safe, if this lock is hardirq-unsafe
		 *
		 * And check whether the new lock's dependency graph
		 * could lead back to the previous lock:
		 *
		 * - within the current held-lock stack
		 * - across our accumulated lock dependency records
		 *
		 * any of these scenarios could lead to a deadlock.
		 */
		/*
		 * The simple case: does the current hold the same lock
		 * already?
		 */
		int ret = check_deadlock(curr, hlock);

		if (!ret)
			return 0;
		/*
		 * Add dependency only if this lock is not the head
		 * of the chain, and if the new lock introduces no more
		 * lock dependency (because we already hold a lock with the
		 * same lock class) nor deadlock (because the nest_lock
		 * serializes nesting locks), see the comments for
		 * check_deadlock().
		 */
		if (!chain_head && ret != 2) {
			if (!check_prevs_add(curr, hlock))
				return 0;
		}

		graph_unlock();
	} else {
		/* after lookup_chain_cache_add(): */
		if (unlikely(!debug_locks))
			return 0;
	}

	return 1;
}
#else
static inline int validate_chain(struct task_struct *curr,
				 struct held_lock *hlock,
				 int chain_head, u64 chain_key)
{
	return 1;
}

static void init_chain_block_buckets(void)	{ }
#endif /* CONFIG_PROVE_LOCKING */

/*
 * We are building curr_chain_key incrementally, so double-check
 * it from scratch, to make sure that it's done correctly:
 */
static void check_chain_key(struct task_struct *curr)
{
#ifdef CONFIG_DEBUG_LOCKDEP
	struct held_lock *hlock, *prev_hlock = NULL;
	unsigned int i;
	u64 chain_key = INITIAL_CHAIN_KEY;

	for (i = 0; i < curr->lockdep_depth; i++) {
		hlock = curr->held_locks + i;
		if (chain_key != hlock->prev_chain_key) {
			debug_locks_off();
			/*
			 * We got mighty confused, our chain keys don't match
			 * with what we expect, someone trample on our task state?
			 */
			WARN(1, "hm#1, depth: %u [%u], %016Lx != %016Lx\n",
				curr->lockdep_depth, i,
				(unsigned long long)chain_key,
				(unsigned long long)hlock->prev_chain_key);
			return;
		}

		/*
		 * hlock->class_idx can't go beyond MAX_LOCKDEP_KEYS, but is
		 * it registered lock class index?
		 */
		if (DEBUG_LOCKS_WARN_ON(!test_bit(hlock->class_idx, lock_classes_in_use)))
			return;

		if (prev_hlock && (prev_hlock->irq_context !=
							hlock->irq_context))
			chain_key = INITIAL_CHAIN_KEY;
		chain_key = iterate_chain_key(chain_key, hlock_id(hlock));
		prev_hlock = hlock;
	}
	if (chain_key != curr->curr_chain_key) {
		debug_locks_off();
		/*
		 * More smoking hash instead of calculating it, damn see these
		 * numbers float.. I bet that a pink elephant stepped on my memory.
		 */
		WARN(1, "hm#2, depth: %u [%u], %016Lx != %016Lx\n",
			curr->lockdep_depth, i,
			(unsigned long long)chain_key,
			(unsigned long long)curr->curr_chain_key);
	}
#endif
}

#ifdef CONFIG_PROVE_LOCKING
static int mark_lock(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit new_bit);

static void print_usage_bug_scenario(struct held_lock *lock)
{
	struct lock_class *class = hlock_class(lock);

	printk(" Possible unsafe locking scenario:\n\n");
	printk("       CPU0\n");
	printk("       ----\n");
	printk("  lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_usage_bug(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit prev_bit, enum lock_usage_bit new_bit)
{
	if (!debug_locks_off() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("================================\n");
	pr_warn("WARNING: inconsistent lock state\n");
	print_kernel_ident();
	pr_warn("--------------------------------\n");

	pr_warn("inconsistent {%s} -> {%s} usage.\n",
		usage_str[prev_bit], usage_str[new_bit]);

	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] takes:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		lockdep_softirq_context(curr), softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		lockdep_softirqs_enabled(curr));
	print_lock(this);

	pr_warn("{%s} state was registered at:\n", usage_str[prev_bit]);
	print_lock_trace(hlock_class(this)->usage_traces[prev_bit], 1);

	print_irqtrace_events(curr);
	pr_warn("\nother info that might help us debug this:\n");
	print_usage_bug_scenario(this);

	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stackÉ'  w  ú'  w  ±'  F  ø'  ã  ‘'  å  ﬁ'  L  Î'  M  ˛'  L  (  N  (  r  !(  `  0(  a  Q(  M  d(  L  i(  N  É(  w  ë(  F  £(  R  ª(  ã  Õ(  Ç  ‹(  É  Ï(  :  ˆ(  è  
)  ê  )  L  )  M  4)  L  9)  N  G)  ë  T)  R  k)  K  v)  `  Ö)  a  ë)  í  √)  w  —)  Y  ·)  F  Ú)  R  ˝)  ã  *  L  *  M  .*  :  w*  î  å*  ï  •*  ï  &+  ï  h+  L  m+  N  ~+  R  ô+  `  ¨+  a  g,  w  {,  ñ  à,  Y  ë,  F  £,  R  ∆,  G  À,  `  Ï,  _  Ò,  `  ,-  T  X-  G  c-  a  h-  b  q-  a  |-  R  ë-  U  ¢-  V  ¨-  W  ≥-  X  »-  U  Ó-  Y  .  F  -.  R  t.  ô  ò.  ö  √.  `  Œ.  G  Î.  _  .  `  ,/  T  X/  G  c/  a  h/  b  q/  a  z/  K  î/  K  ü/  R  …/  U  ⁄/  V  ‰/  W  Î/  X   0  U  /0  K  _0  ô  ·0  Y  Ò0  F  11  R  ∂1  ú  ¡1  ù  ‚1  K  Ì1  R  2  û  "2  _  *2  `  02  G  d2  T  ê2  G  õ2  a  †2  b  ≠2  ü  !3  U  23  V  <3  W  C3  X  X3  U  ∫3  o  Õ3  [  Â3  Y  4  F  64  R  r4  G  •4  °  ”4  ú  ‡4  ù  Û4  û  5  _  
5  `  5  G  D5  T  p5  G  {5  a  Ä5  b  ç5  ü  º5  ú   5  ï  €5  ¢  5  K  ˚5  R  16  U  B6  V  L6  W  S6  X  h6  U  ö6  Y  °6  F  ø6  R  7  G  7  _  7  `  T7  T  Ä7  G  ã7  a  ê7  b  ∑7  R  …7  U  ⁄7  V  ‰7  W  Î7  X   8  U  78  Y  f  K  ˚  P  (    ≥  k  ˚  k  r  a  à  N  …  k  ‘  a  h$  á  é$  á      @                A                B     	     
      C                D                                                           $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     Ä     Ñ     à     å     ê     î     ò     ú     †     §     ®     ¨     ∞     ¥     ∏                   S               S                S  $     (     ,   S  0     4     8   S  <     @     D   S  H     L     P   S  T     X     \   S  `     d     h   S                                 $     (     0     4        :     e                                                 $     (     ,     0     4     8     @     `   I  †     §     ∞     ¿   Z     ^  @  ]  Ä  f  º    ¿    ‡  H     E  \    `    d    h    l    p        D     C     B     A     @   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab+cgroup_bpf_enabled_key .rel___ksymtab+__cgroup_bpf_run_filter_skb .rel___ksymtab+__cgroup_bpf_run_filter_sk .rel___ksymtab+__cgroup_bpf_run_filter_sock_addr .rel___ksymtab+__cgroup_bpf_run_filter_sock_ops .rel__mcount_loc .rodata.str1.4 .rel__jump_table .rodata.str1.1 .rel__bug_table .rel.text.unlikely .rel.smp_locks .rel.rodata .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                                         @   ;8                    	   @       pW    #            %             {8                     +             Ä8  ∏                   0      2       Ä8  ñ                 F             9                    B   	   @       `e     #            l             $9                    h   	   @       xe     #            ó             09                    ì   	   @       êe     #   
         ¡             <9                    Ω   	   @       ®e     #            Ú             H9                    Ó   	   @       ¿e     #            "            T9  º                    	   @       ÿe  x  #            /     2       :  !                 B            4:  l                  >  	   @       Pg  ÿ   #            O     2       †:  I                 b            È:  <                  ^  	   @       (h  P   #            r            %;  
                  n  	   @       xh     #            Ö            0;  0                  Å  	   @       àh  `   #         ELF                      "      4     (               Ë¸ˇˇˇSãX\âÿË¸ˇˇˇçÉ¿   Ë¸ˇˇˇ1¿[√êË¸ˇˇˇWVâ∆Sª
   dã=    ç¥&    çv ãÜî   ∫)   Ë¸ˇˇˇÉ‡t∏∆ß  Ë¸ˇˇˇã®uÉÎu÷∏ˇˇˇˇ[^_√çt& êË¸ˇˇˇÎÊêË¸ˇˇˇVf¡¬Sã∞î   â√∑ ∫&   âË¸ˇˇˇÉ¯ˇt)âÿËrˇˇˇÖ¿x+∫(   âË¸ˇˇˇfÉ¯ˇtf¡¿∑¿[^√fê°    Ö¿è¸ˇˇˇ∏ˇˇˇˇÎÁç¥&    çt& êË¸ˇˇˇUWâ«Vâ÷SâÀÉÏÖ…Ññ   ãAã)â$∫Û   â¯Ëbˇˇˇâ¡°    Ö¿è   Ö…x)â»É‡ t0«$    1Ì∏   Ö€tã<$â+â{1…ÖˆtâÉƒâ»[^_]√çt& É·∫   ”‚˜¬ uVÅ‚  u6É˘uø«$    ÅÂ  ˇ Î∞ç¥&    çv «$    Ωˇˇˇ Èaˇˇˇç¥&    «$    ÅÂ ∞  È|ˇˇˇç∂    âÍ«$    ∂ÍÈeˇˇˇç¥&    ç¥&    êË¸ˇˇˇSâ√ÉÏã    Ö…è9   Éƒâÿ1…[È⁄˛ˇˇç¥&    çv Ë¸ˇˇˇSâ√â—ÉÏ°    Ö¿èT   Éƒâÿ1“[È©˛ˇˇç¥&    fêË¸ˇˇˇWf¡¬Vâ∆Sã∏î   âÀâ—Å· ˇ  ∫'   â¯Ë¸ˇˇˇÉ¯ˇt1âËö˝ˇˇÖ¿xf¡√∫(   â¯∑ÀË¸ˇˇˇÉ¯ˇt[^_√ç¥&    çv °    Ö¿èo   Î„êË¸ˇˇˇVâ∆â—SÉÏ°    Ö¿èä   1€Ö…t
ª   π   ãÜî   ãP\àö"  ∫Ú   Ë¸ˇˇˇ1¿Éƒ[^√çt& Ë¸ˇˇˇÉ˙wvVâ÷Sâ√°    Ö¿è∑   ãÉî   ãı    ãP\àäc  ∫3   ∂…Ë¸ˇˇˇ∫Ú   âÿË$˝ˇˇÉ‡ﬂx*ãı   ∫Ú   ¡·	¡âÿÉ…∑…Ë—˛ˇˇ∏∆ß  Ë¸ˇˇˇ1¿[^√fê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇVSãXãP(ç≥@ˇˇˇÅ˙	ò Ñâ   w7Å˙ 	ò tWÅ˙	ò uGãH|ãC‘ãP\ÉÈ@àä  ∂…∫Á   Ë¸ˇˇˇ1¿[^√çv Å˙	ò u∑H|∫   âË@˛ˇˇ1¿[^√çv ∏Íˇˇˇ[^√ãH|ãC‘ãP\àä  ∂…∫Ê   Ë¸ˇˇˇ1¿[^√ç¥&    ∑H|∫†   âË¯˝ˇˇ1¿[^√çv Ë¸ˇˇˇUâÕWâ◊∫Ú   Vâ∆SÉÏË¸ˇˇã    â$Ö€è  â¯% ∞  Öﬁ   â¯Ñ¿Öå   â¯%  ˇ Ñ  ∏   ∫à   ª‡  ∑»Î
çv ∑∑KÉ√∂“âËt˝ˇˇÅ˚  u‰ã    Ö…è‘   âæ0  ∑<$∫Ú   ââÆ4  Éœ∑œË<˝ˇˇ∏∆ß  Ë¸ˇˇˇ1¿Éƒ[^_]√ç¥&    ê∏   ∫à   ª   ∑»Îçt& ∑∑KÉ√∂“âËÙ¸ˇˇÅ˚H  u‰°    Ö¿~ÖÈÈ   ç∂    1…∫   ª`  Îç¥&    çv ∑∑KÉ√∂“âË¨¸ˇˇÅ˚å  u‰ã    Ö…é8ˇˇˇÈ˛   ê∏ÍˇˇˇÈVˇˇˇç∂    Ë¸ˇˇˇW1…∫Ú   Vâ∆Sª†  Îç¥&    ê∂∂KãÜî   É√ãx\àå0  Ë¸ˇˇˇÖ¿xÅ˚ƒ  u’1…∫Y   ª`  Îç¥&    ∑∑KÉ√∂“âË¸ˇˇÅ˚à  u‰ãÜ0  ˆƒ∞Ö}   π   ∫à   Ñ¿u?©  ˇ Ñ§   ª‡  Îçt& ê∑∑KÉ√