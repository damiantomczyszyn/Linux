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
	dump_stack�'  w  �'  w  �'  F  �'  �  �'  �  �'  L  �'  M  �'  L  (  N  (  r  !(  `  0(  a  Q(  M  d(  L  i(  N  �(  w  �(  F  �(  R  �(  �  �(  �  �(  �  �(  :  �(  �  
)  �  )  L  )  M  4)  L  9)  N  G)  �  T)  R  k)  K  v)  `  �)  a  �)  �  �)  w  �)  Y  �)  F  �)  R  �)  �  *  L  *  M  .*  :  w*  �  �*  �  �*  �  &+  �  h+  L  m+  N  ~+  R  �+  `  �+  a  g,  w  {,  �  �,  Y  �,  F  �,  R  �,  G  �,  `  �,  _  �,  `  ,-  T  X-  G  c-  a  h-  b  q-  a  |-  R  �-  U  �-  V  �-  W  �-  X  �-  U  �-  Y  .  F  -.  R  t.  �  �.  �  �.  `  �.  G  �.  _  �.  `  ,/  T  X/  G  c/  a  h/  b  q/  a  z/  K  �/  K  �/  R  �/  U  �/  V  �/  W  �/  X   0  U  /0  K  _0  �  �0  Y  �0  F  11  R  �1  �  �1  �  �1  K  �1  R  2  �  "2  _  *2  `  02  G  d2  T  �2  G  �2  a  �2  b  �2  �  !3  U  23  V  <3  W  C3  X  X3  U  �3  o  �3  [  �3  Y  4  F  64  R  r4  G  �4  �  �4  �  �4  �  �4  �  5  _  
5  `  5  G  D5  T  p5  G  {5  a  �5  b  �5  �  �5  �  �5  �  �5  �  �5  K  �5  R  16  U  B6  V  L6  W  S6  X  h6  U  �6  Y  �6  F  �6  R  7  G  7  _  7  `  T7  T  �7  G  �7  a  �7  b  �7  R  �7  U  �7  V  �7  W  �7  X   8  U  78  Y  f  K  �  P  (    �  k  �  k  r  a  �  N  �  k  �  a  h$  �  �$  �      @                A                B     	     
      C                D                                                           $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     �     �     �     �     �     �     �     �     �     �     �     �     �     �     �                   S               S                S  $     (     ,   S  0     4     8   S  <     @     D   S  H     L     P   S  T     X     \   S  `     d     h   S                                 $     (     0     4        :     e                                                 $     (     ,     0     4     8     @     `   I  �     �     �     �   Z     ^  @  ]  �  f  �    �    �  H     E  \    `    d    h    l    p        D     C     B     A     @   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab+cgroup_bpf_enabled_key .rel___ksymtab+__cgroup_bpf_run_filter_skb .rel___ksymtab+__cgroup_bpf_run_filter_sk .rel___ksymtab+__cgroup_bpf_run_filter_sock_addr .rel___ksymtab+__cgroup_bpf_run_filter_sock_ops .rel__mcount_loc .rodata.str1.4 .rel__jump_table .rodata.str1.1 .rel__bug_table .rel.text.unlikely .rel.smp_locks .rel.rodata .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                                         @   ;8                    	   @       pW  �  #            %             {8                     +             �8  �                   0      2       �8  �                 F             9                    B   	   @       `e     #            l             $9                    h   	   @       xe     #            �             09                    �   	   @       �e     #   
         �             <9                    �   	   @       �e     #            �             H9                    �   	   @       �e     #            "            T9  �                    	   @       �e  x  #            /     2       :  !                 B            4:  l                  >  	   @       Pg  �   #            O     2       �:  I                 b            �:  <                  ^  	   @       (h  P   #            r            %;  
                  n  	   @       xh     #            �            0;  0                  �  	   @       �h  `   #         ELF                      "      4     (               �����S�X\����������   �����1�[Ð�����WV��S�
   d�=    ��&    �v ���   �)   �������t�Ƨ  �������u��uָ����[^_Ít& �������������Vf��S���   ���ʺ&   ����������t)���r�����x+�(   �������f���tf����[^�f��    �������������獴&    �t& ������UW��V��S�˃�����   �A�)�$��   ���b������    ���   ��x)�ȃ� t0�$    1�   ��t�<$�+�{1Ʌ�t�����[^_]Ít& ���   ���� uV��  u6��u��$    ��  � 밍�&    �v �$    ���� �a�����&    �$    �� �  �|�����    ���$    ���e�����&    ��&    ������S�Ã��    ���9   ����1�[�������&    �v �����S�Éу��    ���T   ����1�[������&    f������Wf��V��S���   �ˉс� �  �'   ����������t1��������xf���(   ������������t[^_Í�&    �v �    ���o   �������V�Ɖ�S���    ����   1ۅ�t
�   �   ���   �P\��"  ��   �����1���[^Ít& �������wvV��S�á    ����   ���   ��    �P\��c  �3   ���������   ���$������x*��   ��   ��	��؃���������Ƨ  �����1�[^�f������Í�&    �v �����VS�X�P(��@�����	� ��   w7�� 	� tW��	� uG�H|�CԋP\��@��  �ɺ�   �����1�[^Ív ��	� u�H|�   ���@���1�[^Ív �����[^ËH|�CԋP\��  �ɺ�   �����1�[^Í�&    �H|��   �������1�[^Ív �����U��W�׺�   V��S�������    �$���  ��% �  ��   ������   ��%  � �  �   ��   ��  ���
�v ��K���҉��t�����  u�    ����   ��0  �<$��   ����4  �����<����Ƨ  �����1���[^_]Í�&    ��   ��   �   ����t& ��K���҉��������H  u�    ��~���   ��    1ɺ   �`  ���&    �v ��K���҉��������  u�    ���8�����   �������V�����    �����W1ɺ�   V��S��  ���&    ���K���   ���x\��0  �������x���  u�1ɺY   �`  ���&    ��K���҉��������  u䋆0  �İ�}   �   ��   ��u?�  � ��   ��  ��t& ���K��