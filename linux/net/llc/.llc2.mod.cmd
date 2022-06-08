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
	WARN_ON_ONCE(!found && debug_l