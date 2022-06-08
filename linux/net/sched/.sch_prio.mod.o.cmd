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
çv ∑∑KÉ√∂“âËt˝ˇˇÅ˚  u‰ã    Ö…è‘   âæ0  ∑<$∫Ú   ââÆ4  Éœ∑œË<˝ˇˇ∏∆ß  Ë¸ˇˇˇ1¿Éƒ[^_]√ç¥&    ê∏   ∫à   ª   ∑»Îçt& ∑∑KÉ√∂“âËÙ¸ˇˇÅ˚H  u‰°    Ö¿~ÖÈÈ   ç∂    1…∫   ª`  Îç¥&    çv ∑∑KÉ√∂“âË¨¸ˇˇÅ˚å  u‰ã    Ö…é8ˇˇˇÈ˛   ê∏ÍˇˇˇÈVˇˇˇç∂    Ë¸ˇˇˇW1…∫Ú   Vâ∆Sª†  Îç¥&    ê∂∂KãÜî   É√ãx\àå0  Ë¸ˇˇˇÖ¿xÅ˚ƒ  u’1…∫Y   ª`  Îç¥&    ∑∑KÉ√∂“âË¸ˇˇÅ˚à  u‰ãÜ0  ˆƒ∞Ö}   π   ∫à   Ñ¿u?©  ˇ Ñ§   ª‡  Îçt& ê∑∑KÉ√∂“âËº˚ˇˇÅ˚  u‰[1¿^_√ç∂    ª   Îê∑∑KÉ√∂“âËå˚ˇˇÅ˚H  u‰[1¿^_√ç∂    1…∫   ª`  Îç¥&    çv ∑∑KÉ√∂“âËL˚ˇˇÅ˚å  u‰[1¿^_√ç∂    ª   Îê∑∑KÉ√∂“âË˚ˇˇÅ˚H  u‰[1¿^_√ç¥&    ç¥&    Ë¸ˇˇˇUWVâ∆SÉÏã@ãPãRË¸ˇˇˇ%  x =  x Öd  çFπ¿  ∫@  Ë¸ˇˇˇâ√Ö¿ÑO  π†   âÚçª¿   Ë¸ˇˇˇ1…∫   â¯j «É0  ˇ   «É4      «É8      «É<     Ë¸ˇˇˇπ 	ò ∫P  â¯j j j jj jjˇjÄË¸ˇˇˇπ	ò ∫P  â¯Éƒ$j j j jj j?j j Ë¸ˇˇˇπ	ò ∫P  â¯Éƒ j h   j jj hˇ  j j Ë¸ˇˇˇπ	ò ∫P  â¯Éƒ j j j jj hˇ  jˇh ˛ˇˇË¸ˇˇˇã´¸   â{lâ¯Éƒ ÖÌuCË¸ˇˇˇ1“âË¸ˇˇˇ∫   â$âË¸ˇˇˇ∫   â«âË¸ˇˇˇ¡Áâ¬â¯–Ä<$ÏÖP  È+  Ë¸ˇˇˇÉƒâË[^_]√êΩÌˇˇˇÎÓΩÙˇˇˇÎÁ                      P                      $                                                                          @                               Ä           P  ¿    †  `  –  P      7%s: %s: failed
 7%s: status: 0x%04x
 7%s: g_input_status
 7%s: querystd
 on off 7%s: s_stream %s
 7%s: input switched to %s
 7%s: s_std %llx
 7%s: norm switched to NTSC
 7%s: norm switched to PAL
 7%s: norm switched to SECAM
 vpx3214c vpx3216b vpx3220a 6%s: %s found @ 0x%x (%s)
 vpx3220 internal composite svideo É√ph   Sh    Ë¸ˇˇˇÉ»ˇÉƒÈ«   QÉ«pâL$Wh   Ë¸ˇˇˇãL$ÉƒÈ)  ç@pâ$Ph(   Ë¸ˇˇˇXZã$È  çCpâ$Ph>   Ë¸ˇˇˇXZã$È6  É∆ph(   Vh    Ë¸ˇˇˇÉƒÈû  â$Ö“∏N   ∫Q   D¬PçFpPhU   Ë¸ˇˇˇãL$ÉƒÈ◊  çCpˇ4ïå  Phh   Ë¸ˇˇˇÉƒÈ)  çFpPhœ   Ë¸ˇˇˇXZÈÓ  çFpPh≥   Ë¸ˇˇˇ[XÈÓ  çFpPhñ   Ë¸ˇˇˇXZÈÓ  çFpUWPhÑ   Ë¸ˇˇˇÉƒÈâ  f=ÄFÑ≥   f=`BÑ‚   ∫Ì   f=ÄBÑù   ãN∑¿çë  R∑V1ˆ“RP∂|$çCpWPh    Ë¸ˇˇˇÉƒãÉî   ∂é°  ∂ñ†  ãx\àå0  Ë¸ˇˇˇÖ¿yq1ˆ∑éb  ∂ñ`  âÿÉ∆ËL  É˛(u„1ˆ∑é"  ∂ñ   âÿÉ∆ËL  É˛(u„È—  ∫ˇ   ãF  P∑F1ˆ¿PçCpRPh  Ë¸ˇˇˇÉƒÈhˇˇˇÉ∆É˛$táÈ[ˇˇˇ∫ˆ   Î√6%s: chip (%02x:%04x) found @ 0x%x (%s)
 Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇ                                vpx3220_fp_read vpx3220_fp_write        vpx3220a                vpx3216b                vpx3214c                                                   ¿                             `                       ¿                                                              –                                              †      Y   † £   §   ®  ≤  æ  X   &   K ò                        Ú 3ÿ® ‡ˇ· ‚„Ä‰ÂÄÊ Á‡Ë¯ÍäÒ¯˘$                            à  â  ä  ã  å  ç è   w Ú ’Á A                        à  â  ä  ã  å  ç è   w Ú —Á A                           à  â  ä  ã   å Äç Äè   s Ú  Á ·,  5  ?  debug parm=debug:Debug level (0-1) parmtype=debug:int license=GPL author=Laurent Pinchart description=vpx3220a/vpx3216b/vpx3214c video decoder driver   ò            ˇ      GCC: (GNU) 11.2.0           GNU  ¿       ¿                                  Òˇ                                                             _                                	 ,   Ä   d     ≈           <           	 Q           \      Ò     k          	      &     ñ   =       	 ≤      '     √   X       	 Ÿ   P  o     Ò   s       	   (          ¿  L     *  é   -    	 @    Ü     R           _  ª       	 v  å       }  †  Ω     å  `  j    ö  ‡  (     •     (     Æ  `  ,     ∏  ÿ   W    	 À  –  r    ÿ  †  $     ‰  `  (                   Ï  P  û    ˙  †           P         /  ˇ    	               *           >      Ä     M      
                   a  @   `     l     0     }  ¿   P     è           §          Ω           À  ò       ›  0        Ù  <        
  T   <     %             0             N             e             r             ã             ö             Æ             »             ·             È                                       )             >             [             m             Ö             ì             ß           ≥             ¬      
     —  @   `     Ù              vpx3220.c vpx3220_remove vpx3220_fp_status vpx3220_fp_read vpx3220_fp_read.cold __func__.2 vpx3220_status vpx3220_status.cold vpx3220_g_input_status vpx3220_g_input_status.cold vpx3220_querystd vpx3220_querystd.cold vpx3220_fp_write.isra.0 vpx3220_fp_write.isra.0.cold __func__.1 vpx3220_s_stream vpx3220_s_stream.cold vpx3220_s_routing input_vals.3 vpx3220_s_routing.cold inputs vpx3220_s_ctrl vpx3220_s_std init_secam init_pal init_ntsc vpx3220_s_std.cold vpx3220_init init_common init_fp vpx3220_probe vpx3220_ops vpx3220_ctrl_ops vpx3220_probe.cold vpx3220_driver_init vpx3220_driver vpx3220_driver_exit vpx3220_id vpx3220_core_ops vpx3220_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free current_task i2c_smbus_read_byte_data __const_udelay __SCT__cond_resched i2c_smbus_write_word_data i2c_smbus_read_word_data _printk i2c_smbus_write_byte_data __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std v4l2_ctrl_handler_setup __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vpx3220_id_device_table param_ops_int      9     :     ;  !   9  2   <  L   =  [   >  y   ?  Å   9  û   @  ∫   A  —     Ò   9  !    Ò  9  ˝    !  9  .    Q  9  v  @  ô  @  ±    ¡  9  œ       C    9  !    6  .  L  C  d  .  Ö  >  °  9  Ï  C  8  C  a  9  ~    º  .  ﬁ  .  Ê      >  ;  .  ^  .  e    Ä  .  ¶  .  Æ    —  9  ‚  .    C    .  #  .  F  .  u  .  ñ  .  ©  .  ∆  .  ‡  .    .    .  6  .  Q  9  h  D  ä  E  ô  .  ¶  F  ﬁ  G  Ë  .  ˇ  H  	  .  #  H  -  .  M  H  W  .  w  H  é  I  ó  =  ¶  =  ¥  =  —  ;  Ÿ     )        6    π    ◊    )    â    Ó    n    ª    «    Ã                   h   .                                                 $     (     ,     0     4   *     .  
        B  (     -   B  E     J   B  `     e   B  w   .  }     Ç   B  î     ô     ¶     ´   B  ¡   .  «     Ã   B  ›     ‚   B  Ú     ˜   B        B      #  B  F    v  %  {  B  ã  .  í  .  °  C  Æ  .  µ  .  ø    Õ  .  ‘  .  ﬁ    Ì    	      B  (         9     T     o     ä     ∑     ‘     È     ˛         +    Ë       9          J     K          M  †   .  ¨   .  ¿     Ã     ÿ     ‰     Ë     (    X    å    ê    î        .     J     P        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   Ó                    	   @       T  ‡              )             @	  Ä                   %   	   @       4                  /             ¿	                    8             ¿	  8                  4   	   @       T  p               E      2       ¯	  F                X             >  .                 T   	   @       ƒ  ∏     	         g      2       l  *                 z             ñ                    v   	   @       |                   â             ™  
                  Ö   	   @       ú                  ò             ¿  û                  î   	   @       ¨   `               †             ^  ê                  ≠                                 ©   	   @       !                  µ      0                          æ                                   Œ               (                                @       9         	              P                                 ,!  ·                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
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
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
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

kernel/irq/migration.o: $(deps_kernel/irq/migration.o)

$(deps_kernel/irq/migration.o):
                                                                                                                                                                                                                   ELF                      <      4     (               Ë¸ˇˇˇU1¿WVSÉÏË¸ˇˇˇ;    Éé  â≈ç¥&    âËË¸ˇˇˇçplâ√âË¸ˇˇˇãS$ã{(â$ã
ÖˇÑB  ãG(Ö¿Ñ7  ˆ≈ÖV  ˜¡  @ âL$ÑF  Ë¸ˇˇˇã$âD$ãJÖ…ãL$Ñ;  çBâ$ãT$ã$âL$Ë¸ˇˇˇã    ãL$9–s'ã$«D$    ã #    tÛº¿âD$;T$Ü   ã$ãT$£É–   âÿâ$Ë¸ˇˇˇ∫   âÿË¸ˇˇˇçìÄ   ã$Ñ¿uãC$çPçCÅ· Ä â$âL$uãOÖ…tâT$Ë¸ˇˇˇãT$ãπ    #    tÛº¿â¡9    wãK$∫    ã©    Öü   ã$1…Ë¸ˇˇˇÖ¿uiãD$Ö¿uãW Ö“t
ã$Ë¸ˇˇˇfêâË¸ˇˇˇçEË¸ˇˇˇâ≈9    á{˛ˇˇÉƒ[^_]√çv 1“âÿË¸ˇˇˇÎÕçt& êãC$É¿â$ÈΩ˛ˇˇç¥&    çv âD$∫    ∏    Ë¸ˇˇˇãL$Ö¿ÑxˇˇˇÈ¸ˇˇˇçt&   Ä ââÿË¸ˇˇˇÈuˇˇˇçt& êË¸ˇˇˇUâ≈WVSÉÏË¸ˇˇˇ1¿Ë¸ˇˇˇ;    sâ√çt& âÿË¸ˇˇˇçxlâ∆â¯Ë¸ˇˇˇãF$˜     tBãN@Ö…t;ãV(Ö“t4çP£hs+ãF$ã ©  Ä uw©   t`∏   â$Ë¸ˇˇˇÑ¿u/ç¥&    â¯Ë¸ˇˇˇçCË¸ˇˇˇâ√9    wáË¸ˇˇˇ1¿Éƒ[^_]√∏   Ë¸ˇˇˇãV$â¡ã ˜–#Bt¿£)ã$s∏çF1…Ë¸ˇˇˇÎ¨çt& 1…∫   âË¸ˇˇˇÎò    à  
                   4Eff. affinity %*pbl of IRQ %u contains only offline CPUs after offlining CPU %u
  4IRQ%u: set affinity failed(%d).
 QˇshT   Ë¸ˇˇˇÉƒÈs  âL$ˇt$ˇsˇt$Rh    Ë¸ˇˇˇãL$ÉƒÈ‰           migrate_one_irq  GCC: (GNU) 11.2.0           GNU  ¿        ¿                                  Òˇ                                                                   
                    <                  
 A           ^              i              z              Ç              é              ù              ¥              ƒ              œ              ·              ˘                           )             =             V             g             t             ê             ò          ∞             ¿             ”             Ë             ˝                          $             <              cpuhotplug.c __func__.0 _rs.1 irq_migrate_all_off_this_cpu.cold irq_migrate_all_off_this_cpu __fentry__ irq_get_next_irq nr_irqs irq_to_desc _raw_spin_lock debug_smp_processor_id cpumask_any_but nr_cpu_ids __cpu_online_mask irq_force_complete_move irq_fixup_move_pending __x86_indirect_thunk_ecx irq_do_set_affinity __x86_indirect_thunk_edx _raw_spin_unlock ___ratelimit irq_shutdown_and_deactivate _printk irq_affinity_online_cpu irq_lock_sparse _raw_spin_lock_irq housekeeping_enabled _raw_spin_unlock_irq irq_unlock_sparse housekeeping_cpumask irq_set_affinity_locked irq_startup                +     7     s     ü     •     ¿     Ó     ˙     /    @    N    X    o    ä    ì    õ    £    Ω    Â  	  Í    Ô        !    /    6    <    K    W    ô     ´  !  ≥    ª    ¬  "  ÷  #  ˆ  $  
  %  ‘               
     '     ,          8                 .symtab .strtab .shstrtab .rel.text .data .bss .rodata.str1.4 .rel.text.unlikely .rel__mcount_loc .rodata .comment .note.GNU-stack .note.gnu.property                                                       @                       	   @       	  H              %             P                    +             l    