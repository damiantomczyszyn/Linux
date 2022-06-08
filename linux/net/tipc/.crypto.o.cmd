move lock classes from memory that is going to be
 * freed; and possibly re-used by other modules.
 *
 * We will have had one synchronize_rcu() before getting here, so we're
 * guaranteed nobody will look up these exact classes -- they're properly dead
 * but still allocated.
 */
static void lockdep_free_key_range_reg(void *start, unsigned long size)
{
	struct pending_free *pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	pf = get_pending_free();
	__lockdep_free_key_range(pf, start, size);
	call_rcu_zapped(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	/*
	 * Wait for any possible iterators from look_up_lock_class() to pass
	 * before continuing to free the memory they refer to.
	 */
	synchronize_rcu();
}

/*
 * Free all lockdep keys in the range [start, start+size). Does not sleep.
 * Ignores debug_locks. Must only be used by the lockdep selftests.
 */
static void lockdep_free_key_range_imm(void *start, unsigned long size)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_free_key_range(pf, start, size);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_free_key_range(void *start, unsigned long size)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_free_key_range_imm(start, size);
	else
		lockdep_free_key_range_reg(start, size);
}

/*
 * Check whether any element of the @lock->class_cache[] array refers to a
 * registered lock class. The caller must hold either the graph lock or the
 * RCU read lock.
 */
static bool lock_class_cache_is_registered(struct lockdep_map *lock)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i, j;

	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			for (j = 0; j < NR_LOCKDEP_CACHING_CLASSES; j++)
				if (lock->class_cache[j] == class)
					return true;
		}
	}
	return false;
}

/* The caller must hold the graph lock. Does not sleep. */
static void __lockdep_reset_lock(struct pending_free *pf,
				 struct lockdep_map *lock)
{
	struct lock_class *class;
	int j;

	/*
	 * Remove all classes this lock might have:
	 */
	for (j = 0; j < MAX_LOCKDEP_SUBCLASSES; j++) {
		/*
		 * If the class exists we look it up and zap it:
		 */
		class = look_up_lock_class(lock, j);
		if (class)
			zap_class(pf, class);
	}
	/*
	 * Debug check: in the end all mapped classes should
	 * be gone.
	 */
	if (WARN_ON_ONCE(lock_class_cache_is_registered(lock)))
		debug_locks_off();
}

/*
 * Remove all information lockdep has about a lock if debug_locks == 1. Free
 * released data structures from RCU context.
 */
static void lockdep_reset_lock_reg(struct lockdep_map *lock)
{
	struct pending_free *pf;
	unsigned long flags;
	int locked;

	raw_local_irq_save(flags);
	locked = graph_lock();
	if (!locked)
		goto out_irq;

	pf = get_pending_free();
	__lockdep_reset_lock(pf, lock);
	call_rcu_zapped(pf);

	graph_unlock();
out_irq:
	raw_local_irq_restore(flags);
}

/*
 * Reset a lock. Does not sleep. Ignores debug_locks. Must only be used by the
 * lockdep selftests.
 */
static void lockdep_reset_lock_imm(struct lockdep_map *lock)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_reset_lock(pf, lock);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_reset_lock(struct lockdep_map *lock)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_reset_lock_imm(lock);
	else
		lockdep_reset_lock_reg(lock);
}

/*
 * Unregister a dynamically allocated key.
 *
 * Unlike lockdep_register_key(), a search is always done to find a matching
 * key irrespective of debug_locks to avoid potential invalid access to freed
 * memory in lock_class entry.
 */
void lockdep_unregister_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head = keyhashentry(key);
	struct lock_class_key *k;
	struct pending_free *pf;
	unsigned long flags;
	bool found = false;

	might_sleep();

	if (WARN_ON_ONCE(static_obj(key)))
		return;

	raw_local_irq_save(flags);
	lockdep_lock();

	hlist_for_each_entry_rcu(k, hash_head, hash_entry) {
		if (k == key) {
			hlist_del_rcu(&k->hash_entry);
			found = true;
			break;
		}
	}
	WARN_ON_ONCE(!found && debug_locks);
	if (found) {
		pf = get_pending_free();
		__lockdep_free_key_range(pf, key, 1);
		call_rcu_zapped(pf);
	}
	lockdep_unlock();
	raw_local_irq_restore(flags);

	/* Wait until is_dynamic_key() has finished accessing k->hash_entry. */
	synchronize_rcu();
}
EXPORT_SYMBOL_GPL(lockdep_unregister_key);

void __init lockdep_init(void)
{
	printk("Lock dependency validator: Copyright (c) 2006 Red Hat, Inc., Ingo Molnar\n");

	printk("... MAX_LOCKDEP_SUBCLASSES:  %lu\n", MAX_LOCKDEP_SUBCLASSES);
	printk("... MAX_LOCK_DEPTH:          %lu\n", MAX_LOCK_DEPTH);
	printk("... MAX_LOCKDEP_KEYS:        %lu\n", MAX_LOCKDEP_KEYS);
	printk("... CLASSHASH_SIZE:          %lu\n", CLASSHASH_SIZE);
	printk("... MAX_LOCKDEP_ENTRIES:     %lu\n", MAX_LOCKDEP_ENTRIES);
	printk("... MAX_LOCKDEP_CHAINS:      %lu\n", MAX_LOCKDEP_CHAINS);
	printk("... CHAINHASH_SIZE:          %lu\n", CHAINHASH_SIZE);

	printk(" memory used by lock dependency info: %zu kB\n",
	       (sizeof(lock_classes) +
		sizeof(lock_classes_in_use) +
		sizeof(classhash_table) +
		sizeof(list_entries) +
		sizeof(list_entries_in_use) +
		sizeof(chainhash_table) +
		sizeof(delayed_free)
#ifdef CONFIG_PROVE_LOCKING
		+ sizeof(lock_cq)
		+ sizeof(lock_chains)
		+ sizeof(lock_chains_in_use)
		+ sizeof(chain_hlocks)
#endif
		) / 1024
		);

#if defined(CONFIG_TRACE_IRQFLAGS) && defined(CONFIG_PROVE_LOCKING)
	printk(" memory used for stack traces: %zu kB\n",
	       (sizeof(stack_trace) + sizeof(stack_trace_hash)) / 1024
	       );
#endif

	printk(" per task-struct memory footprint: %zu bytes\n",
	       sizeof(((struct task_struct *)NULL)->held_locks));
}

static void
print_freed_lock_bug(struct task_struct *curr, const void *mem_from,
		     const void *mem_to, struct held_lock *hlock)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=========================\n");
	pr_warn("WARNING: held lock freed!\n");
	print_kernel_ident();
	pr_warn("-------------------------\n");
	pr_warn("%s/%d is freeing memory %px-%px, with a lock still held there!\n",
		curr->comm, task_pid_nr(curr), mem_from, mem_to-1);
	print_lock(hlock);
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static inline int not_in_range(const void* mem_from, unsigned long mem_len,
				const void* lock_from, unsigned long lock_len)
{
	return lock_from + lock_len <= mem_from ||
		mem_from + mem_len <= lock_from;
}

/*
 * Called when kernel memory is freed (or unmapped), or if a lock
 * is destroyed or reinitialized - this code checks whether there is
 * any held lock in the memory range of <from> to <to>:
 */
void debug_check_no_locks_freed(const void *mem_from, unsigned long mem_len)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	unsigned long flags;
	int i;

	if (unlikely(!debug_locks))
		return;

	raw_local_irq_save(flags);
	for (i = 0; i < curr->lockdep_depth; i++) {
		hlock = curr->held_locks + i;

		if (not_in_range(mem_from, mem_len, hlock->instance,
					sizeof(*hlock->instance)))
			continue;

		print_freed_lock_bug(curr, mem_from, mem_from + mem_len, hlock);
		break;
	}
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(debug_check_no_locks_freed);

static void print_held_locks_bug(void)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("====================================\n");
	pr_warn("WARNING: %s/%d still has locks held!\n",
	       current->comm, task_pid_nr(current));
	print_kernel_ident();
	pr_warn("------------------------------------\n");
	lockdep_print_held_locks(current);
	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

void debug_check_no_locks_held(void)
{
	if (unlikely(current->lockdep_depth > 0))
		print_held_locks_bug();
}
EXPORT_SYMBOL_GPL(debug_check_no_locks_held);

#ifdef __KERNEL__
void debug_show_all_locks(void)
{
	struct task_struct *g, *p;

	if (unlikely(!debug_locks)) {
		pr_warn("INFO: lockdep is turned off.\n");
		return;
	}
	pr_warn("\nShowing all locks held in the system:\n");

	rcu_read_lock();
	for_each_process_thread(g, p) {
		if (!p->lockdep_depth)
			continue;
		lockdep_print_held_locks(p);
		touch_nmi_watchdog();
		touch_all_softlockup_watchdogs();
	}
	rcu_read_unlock();

	pr_warn("\n");
	pr_warn("=============================================\n\n");
}
EXPORT_SYMBOL_GPL(debug_show_all_locks);
#endif

/*
 * Careful: only use this function if you are sure that
 * the task cannot run in parallel!
 */
void debug_show_held_locks(struct task_struct *task)
{
	if (unlikely(!debug_locks)) {
		printk("INFO: lockdep is turned off.\n");
		return;
	}
	lockdep_print_held_locks(task);
}
EXPORT_SYMBOL_GPL(debug_show_held_locks);

asmlinkage __visible void lockdep_sys_exit(void)
{
	struct task_struct *curr = current;

	if (unlikely(curr->lockdep_depth)) {
		if (!debug_locks_off())
			return;
		pr_warn("\n");
		pr_warn("================================================\n");
		pr_warn("WARNING: lock held when returning to user space!\n");
		print_kernel_ident();
		pr_warn("------------------------------------------------\n");
		pr_warn("%s/%d is leaving the kernel with locks still held!\n",
				curr->comm, curr->pid);
		lockdep_print_held_locks(curr);
	}

	/*
	 * The lock history for each syscall should be independent. So wipe the
	 * slate clean on return to userspace.
	 */
	lockdep_invariant_state(false);
}

void lockdep_rcu_suspicious(const char *file, const int line, const char *s)
{
	struct task_struct *curr = current;
	int dl = READ_ONCE(debug_locks);

	/* Note: the following can be executed concurrently, so be careful. */
	pr_warn("\n");
	pr_warn("=============================\n");
	pr_warn("WARNING: suspicious RCU usage\n");
	print_kernel_ident();
	pr_warn("-----------------------------\n");
	pr_warn("%s:%d %s!\n", file, line, s);
	pr_warn("\nother info that might help us debug this:\n\n");
	pr_warn("\n%srcu_scheduler_active = %d, debug_locks = %d\n%s",
	       !rcu_lockdep_current_cpu_online()
			? "RCU used illegally from offline CPU!\n"
			: "",
	       rcu_scheduler_active, dl,
	       dl ? "" : "Possible false positive due to lockdep disabling via debug_locks = 0\n");

	/*
	 * If a CPU is in the RCU-free window in idle (ie: in the section
	 * between rcu_idle_enter() and rcu_idle_exit(), then RCU
	 * considers that CPU to be in an "extended quiescent state",
	 * which means that RCU will be completely ignoring that CPU.
	 * Therefore, rcu_read_lock() and friends have absolutely no
	 * effect on a CPU running in that state. In other words, even if
	 * such an RCU-idle CPU has called rcu_read_lock(), RCU might well
	 * delete data structures out from under it.  RCU really has no
	 * choice here: we need to keep an RCU-free window in idle where
	 * the CPU may possibly enter into low power mode. This way we can
	 * notice an extended quiescent state to other CPUs that started a grace
	 * period. Otherwise we would delay any grace period as long as we run
	 * in the idle task.
	 *
	 * So complain bitterly if someone does call rcu_read_lock(),
	 * rcu_read_lock_bh() and so on from extended quiescent states.
	 */
	if (!rcu_is_watching())
		pr_warn("RCU used illegally from extended quiescent state!\n");

	lockdep_print_held_locks(curr);
	pr_warn("\nstack backtrace:\n");
	dump_stack();
}
EXPORT_SYMBOL_GPL(lockdep_rcu_suspicious);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        free cgroup_mutex mutex_lock mutex_unlock system_wq queue_work_on seq_printf __stack_chk_guard bpf_stats_enabled_key __x86_indirect_thunk_ecx sched_clock __x86_indirect_thunk_esi debug_smp_processor_id __per_cpu_offset __stack_chk_fail bpf_sysctl_set_new_value memcpy memset bpf_sysctl_get_current_value bpf_sysctl_get_new_value migrate_disable __rcu_read_lock __rcu_read_unlock migrate_enable strscpy strnlen fortify_panic bpf_sysctl_get_name bpf_ktime_get_coarse_ns_proto bpf_strtol_proto bpf_get_current_cgroup_id_proto bpf_get_current_uid_gid_proto bpf_base_func_proto bpf_strtoul_proto bpf_event_output_data_proto bpf_get_local_storage_proto __kmalloc css_next_descendant_pre percpu_ref_is_zero bpf_prog_put __x86_indirect_thunk_ebx bpf_prog_array_alloc bpf_prog_array_free static_key_slow_dec __x86_indirect_thunk_edx bpf_tcp_sock_proto bpf_sk_setsockopt_proto bpf_sk_storage_delete_proto bpf_sk_getsockopt_proto bpf_sk_storage_get_proto bpf_cgroup_storage_unlink bpf_cgroup_storage_free percpu_ref_exit cgroup_storage_lookup bpf_cgroup_storage_link kmalloc_caches kmem_cache_alloc_trace static_key_slow_inc bpf_cgroup_storage_alloc cgroup_bpf_offline percpu_ref_kill_and_confirm cgroup_bpf_inherit percpu_ref_init cgroup_bpf_prog_attach cgroup_get_from_fd bpf_prog_get_type_dev cgroup_bpf_prog_detach cgroup_bpf_link_attach bpf_link_init bpf_link_prime bpf_link_settle bpf_link_cleanup cgroup_bpf_prog_query bpf_prog_array_length bpf_prog_array_copy_to_user __cgroup_bpf_check_dev_permission __cgroup_bpf_run_filter_sysctl __kmalloc_track_caller __x86_indirect_thunk_edi __cgroup_bpf_run_filter_setsockopt __check_object_size _copy_from_user lock_sock_nested release_sock __cgroup_bpf_run_filter_getsockopt __get_user_4 __put_user_4 __cgroup_bpf_run_filter_getsockopt_kern cg_sockopt_prog_ops cg_sockopt_verifier_ops cg_sysctl_prog_ops cg_sysctl_verifier_ops cg_dev_verifier_ops cg_dev_prog_ops      F     G  !   F  *   G  A   F  Ò  F    J  1  F  Q  F  a  F  q  F  {  L  É  M  õ  L  †  N  —  F  Í  O  Ò      F    L    M  +  L  0  N  :    @  Q  m  R  è  T  ¶  R  ¡  U  “  V  ‹  W  „  X  ¯  U  °  Y  ±  F  	  [  A  F  j  [  ä  \  ß  \  ¡  F  Ò  F  	  \  a	  F  x	  R  
  _  
  `  %
  G  m
  G  x
  a  }
  b  ¥
  R  ˇ
  _    `    G  g  G  r  a  w  b  ≥  Y  ¡  F  Ì  c  	      d  7    <  c  Å  F  ó  R  º  c  œ  R    Y  !  F  ë  F  !  F  !  F  :  :  V  g  b  h  z  :  â  :  ü  i  ©  j  ¡  l  —  :  ·  :  Ò  :    m    n  !  F  b  o  °  F  ∞  L  ∏  M  Ê  :  N  p  v  p  ã  q  ∂  L  ª  N  1  p  F  r  q  F  á  R  °  _  ¶  `  Ø  G  ‚  T    G    a    b  -  R  A  U  R  V  \  W  c  X  x  U  ¢  Y  ±  F  ¿  R  ﬁ  _  „  `  Ï  G  "  T  N  G  Y  a  ^  b  m  R  Å  U  í  V  ú  W  £  X  ∏  U  ‚  Y  Ò  F  
  R  p  _  u  `  ~  G  ¥  T  Â  G    a  ı  b    R  !  U  4  s  >  W  E  X  Z  U  ü  Y  ±  F  ¬  i  ﬂ  :  Ò  j    :    m  !  n  1  F  À  t  q  F  Ç  p  ó  q  Ω  p  “  p  Ô  p     q    u  *  p  E  p  W  u  j  p  Å  F  •  :  ∫  K  ”  r  ‹  @  ·  v    F    L    M  F  L  K  N  a  `  Ç  L  ´  w  ¡  F  ·  F    F    x  3  :  J  y  a  z  u  {  ó  :  ≠  i  ª  j  —  |  ·  :  Ò  m    n    F    @  /  L  4  M  l  K  s  v  ú  r  ∏  `  ’  a  Ì  u  ¸  @  #  }  ,  ~  >  L  C  N  V  `  h  a  {    ™  T  ∑  `  Ù  w  !  w  1  F  M  R  ‘  :  π!  Ä  Ù!  R  â"  r  ù"  Å  ¨"  Å  ∂"  Ç  ≈"  É  *#  @  /#  Ñ  E#  Ö  ã#  ~  î#  ~  ‹#  ~  Â#  ~  $  K   $  ~  )$  ~  =$  Y  Q$  F  q$  `  Ä$  a  ±$  F  º$     $  R  Î$  â  %  `  %  a  v%  u  Ü%  `  ò%  a  ¨%    ∑%  R  Ò%  u  /&  w  9&  Y  A&  F  U&  ã  t&  L  &  M  ò&  L  ù&  N  ¿&  å  —&  L  ‹&  M  ı&  L  ˙&  N  '  r  '  `  "'  a  Q'  `  `'  a  ELF                      ¸      4     (               Ë¸ˇˇˇã@\Ë¸ˇˇˇ1¿√Ë¸ˇˇˇSâ√π   É√pÉÏd°    âD$ãC$jçT$Ë¸ˇˇˇ∂D$P∂D$P∂D$P∂D$P∂D$P∂D$P∂D$PSh    Ë¸ˇˇˇÉƒ(ãD$d+    uÉƒ1¿[√Ë¸ˇˇˇçt& Ë¸ˇˇˇUWâœVâ÷Sâ√ÉÏã´î   d°    âD$1¿Ä=     àT$àL$Ö   j π   âËçT$Ë¸ˇˇˇZÉ¯Ö¸ˇˇˇãD$d+    uÉƒ[^_]√Ë¸ˇˇˇêË¸ˇˇˇÉ˙wvVSâ√â–É‡É¯tX∂ã¬   â–¡‚∂≥ƒ   ¡‡É‚ àÉ¿   É·?ÉÊﬂàì¡   	¡	÷âÿ∂…1“Ë=ˇˇˇâ∫   ∂»âÿË,ˇˇˇ1¿[^√ç¥&    ∏ÍˇˇˇÎÔç¥&    fê∏Íˇˇˇ√ç¥&    çv Ë¸ˇˇˇUWVSâ√ã@ãPãRË¸ˇˇˇ%   =   Ñ<   ∏˚ˇˇˇ[^_]√                                 ê                                                                                                                                 ê      ê      6%s: Status: SA00=%02x SA01=%02x SA02=%02x SA03=%02x SA04=%02x SA05=%02x SA06=%02x
    7%s: write reg: %02x val: %02x
    3%s: I/O error write 0x%02x/0x%02x
    6%s %d-%04x: chip found @ 0x%x (%s)
 â¯É√p∂¯â∂WVSh|   Ë¸ˇˇˇÉƒÈ·   ∂¡P∂¬PçCpPhX   Ë¸ˇˇˇÉƒÈ≈   ãS∑Cçä  Qç QPˇ≤   ãCTˇ0h§   Ë¸ˇˇˇçCπ¿  ∫‹   Ë¸ˇˇˇâ≈ÉƒÖ¿u
∏ÙˇˇˇÈ∫  π`   â⁄çΩ¬   1€Ë¸ˇˇˇæ    π   f«Ö¿     Û§∂å¬   â⁄âËÉ√Ëå   É˚uÁ1¿È∫          †-)6›VH :†D`R¯S`         upd64083                                                        ‡           Ä                                                                                                                                                                 debug Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇupd64083 parm=debug:Debug level (0-1) parmtype=debug:bool license=GPL author=T. Adachi, Takeru KOMORIYA, Hans Verkuil description=uPD64083 driver             §ˇ      GCC: (GNU) 11.2.0           GNU  ¿        ¿                                  Òˇ                                                                          |                  	 0   ê   o     7           ?       @    	 S      Ü                   f   ê  3     u   @   ñ    	 â   `                       ñ            ´       Ä     ª       
                   –       0     ‹   ‡   0     Ó   Ä   P                          /           =         O  1        f  =   0     |  m        ó             ¢             ¿             “             Ï             Ù                                       +             @             N             b           n             }      
     å      0     ∞              upd64083.c upd64083_remove upd64083_log_status upd64083_write upd64083_write.cold upd64083_s_routing upd64083_probe upd64083_probe.cold upd64083_ops upd64083_driver_init upd64083_driver upd64083_driver_exit upd64083_id upd64083_core_ops upd64083_video_ops __UNIQUE_ID_debug270 __UNIQUE_ID_debugtype269 __param_debug __param_str_debug __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev __stack_chk_guard i2c_transfer_buffer_flags _printk __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__upd64083_id_device_table param_ops_bool        	   !        %   "  7   #  g     l   $  z   "  à   %  ë      ™   "  ∂     ◊   #  Ï   "  ˚   %       ë     •  &  ≈     ·     µ                   h                                            $  /     4   $  _     d   $  v   '  é     ù   (  ¢     ∆          <     â     “     `     l     Ä     ‡                   )     *          ,           )     /        .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rel.text.unlikely .rel.rodata .rel.init.text .rel.exit.text .rodata.str1.1 .modinfo .rel__param .comment .note.GNU-stack .note.gnu.property                                                         @   √                    	   @       8  ®               )                Ä                   %   	   @       ‡                  /             †                    8             †                    4   	   @          0               E      2       ∏                    X             Ç  ÷                  T   	   @       0  x      	         k             `                    g   	   @       ®                  w             v                    s   	   @       »                  Ü             ä  
                  Ç   	   @       Ë                 ë      2       î  	                 †             ù  â                  ≠             (                    ©   	   @       ¯                  µ      0       <                   æ              O                     Œ             P  (                                x                  	              x	  ø                                 ·                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              É'  w  ú'  w  ±'  F  ø'  ã  ‘'  å  ﬁ'  L  Î'  M  ˛'  L  (  N  (  r  !(  `  0(  a  Q(  M  d(  L  i(  N  É(  w  ë(  F  £(  R  ª(  ã  Õ(  Ç  ‹(  É  Ï(  :  ˆ(  è  
)  ê  )  L  )  M  4)  L  9)  N  G)  ë  T)  R  k)  K  v)  `  Ö)  a  ë)  í  √)  w  —)  Y  ·)  F  Ú)  R  ˝)  ã  *  L  *  M  .*  :  w*  î  å*  ï  •*  ï  &+  ï  h+  L  m+  N  ~+  R  ô+  `  ¨+  a  g,  w  {,  ñ  à,  Y  ë,  F  £,  R  ∆,  G  À,  `  Ï,  _  Ò,  `  ,-  T  X-  G  c-  a  h-  b  q-  a  |-  R  ë-  U  ¢-  V  ¨-  W  ≥-  X  »-  U  Ó-  Y  .  F  -.  R  t.  ô  ò.  ö  √.  `  Œ.  G  Î.  _  .  `  ,/  T  X/  G  c/  a  h/  b  q/  a  z/  K  î/  K  ü/  R  …/  U  ⁄/  V  ‰/  W  Î/  X   0  U  /0  K  _0  ô  ·0  Y  Ò0  F  11  R  ∂1  ú  ¡1  ù  ‚1  K  Ì1  R  2  û  "2  _  *2  `  02  G  d2  T  ê2  G  õ2  a  †2  b  ≠2  ü  !3  U  23  V  <3  W  C3  X  X3  U  ∫3  o  Õ3  [  Â3  Y  4  F  64  R  r4  G  •4  °  ”4  ú  ‡4  ù  Û4  û  5  _  
5  `  5  G  D5  T  p5  G  {5  a  Ä5  b  ç5  ü  º5  ú   5  ï  €5  ¢  5  K  ˚5  R  16  U  B6  V  L6  W  S6  X  h6  U  ö6  Y  °6  F  ø6  R  7  G  7  _  7  `  T7  T  Ä7  G  ã7  a  ê7  b  ∑7  R  …7  U  ⁄7  V  ‰7  W  Î7  X   8  U  78  Y  f  K  ˚  P  (    ≥  k  ˚  k  r  a  à  N  …  k  ‘  a  h$  á  é$  á      @                A                B     	     
      C                D                                                           $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     Ä     Ñ     à     å     ê     î     ò     ú     †     §     ®     ¨     ∞     ¥     ∏                   S               S                S  $     (     ,   S  0     4     8   S  <     @     D   S  H     L     P   S  T     X     \   S  `     d     h   S                                 $     (     0     4        :     e                                                 $     (     ,     0     4     8     @     `   I  †     §     ∞     ¿   Z     ^  @  ]  Ä  f  º    ¿    ‡  H     E  \    `    d    h    l    p        D     C     B     A     @   .symtab .strtab .shstrtab .rel.text .data .bss __ksymtab_strings .rel___ksymtab+cgroup_bpf_enabled_key .rel___ksymtab+__cgroup_bpf_run_filter_skb .rel___ksymtab+__cgroup_bpf_run_filter_sk .rel___ksymtab+__cgroup_bpf_run_filter_sock_addr .rel___ksymtab+__cgroup_bpf_run_filter_sock_ops .rel__mcount_loc .rodata.str1.4 .rel__jump_table .rodata.str1.1 .rel__bug_table .rel.text.unlikely .rel.smp_locks .rel.rodata .rel.discard.addressable .comment .note.GNU-stack .note.gnu.property                                                         @   ;8                    	   @       pW    #            %             {8                     +             Ä8  ∏                   0      2       Ä8  ñ                 F             9                    B   	   @       `e     #            l             $9                    h   	   @       xe     #            ó             09                    ì   	   @       êe     #   
         ¡             <9                    Ω   	   @       ®e     #            Ú             H9                    Ó   	   @       ¿e     #            "            T9  º                    	   @       ÿe  x  #            /     2       :  !                 B            4:  l                  >  	   @       Pg  ÿ   #            O     2       †:  I                 b            È:  <                  ^  	   @       (h  P   #            r            %;  
                  n  	   @       xh     #            Ö            0;  0                  Å  	   @       àh  `   #            î            `;  t                  ê  	   @       Ëh  ∞   #            †            ‘=                    ú  	   @       òi  (   #            µ     0       Ë=                   æ             ˚=                     Œ            ¸=  (                                $>  †
  $   @         	              ƒH  ™                               ¿i  ·                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_drivers/media/i2c/upd64083.o := gcc -Wp,-MMD,drivers/media/i2c/.upd64083.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"upd64083"' -DKBUILD_MODNAME='"upd64083"' -D__KBUILD_MODNAME=kmod_upd64083 -c -o drivers/media/i2c/upd64083.o drivers/media/i2c/upd64083.c 

source_drivers/media/i2c/upd64083.o := drivers/media/i2c/upd64083.c

deps_drivers/media/i2c/upd64083.o := \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
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
    $(wildcard incl