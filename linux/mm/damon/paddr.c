*pf;
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
	pr