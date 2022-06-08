tic void zap_class(struct pending_free *pf, struct lock_class *class)
{
	struct lock_list *entry;
	int i;

	WARN_ON_ONCE(!class->key);

	/*
	 * Remove all dependencies this lock is
	 * involved in:
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		entry = list_entries + i;
		if (entry->class != class && entry->links_to != class)
			continue;
		__clear_bit(i, list_entries_in_use);
		nr_list_entries--;
		list_del_rcu(&entry->entry);
	}
	if (list_empty(&class->locks_after) &&
	    list_empty(&class->locks_before)) {
		list_move_tail(&class->lock_entry, &pf->zapped);
		hlist_del_rcu(&class->hash_entry);
		WRITE_ONCE(class->key, NULL);
		WRITE_ONCE(class->name, NULL);
		nr_lock_classes--;
		__clear_bit(class - lock_classes, lock_classes_in_use);
		if (class - lock_classes == max_lock_class_idx)
			max_lock_class_idx--;
	} else {
		WARN_ONCE(true, "%s() failed for class %s\n", __func__,
			  class->name);
	}

	remove_class_from_lock_chains(pf, class);
	nr_zapped_classes++;
}

static void reinit_class(struct lock_class *class)
{
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	memset_startat(class, 0, key);
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
}

static inline int within(const void *addr, void *start, unsigned long size)
{
	return addr >= start && addr < start + size;
}

static bool inside_selftest(void)
{
	return current == lockdep_selftest_task_struct;
}

/* The caller must hold the graph lock. */
static struct pending_free *get_pending_free(void)
{
	return delayed_free.pf + delayed_free.index;
}

static void free_zapped_rcu(struct rcu_head *cb);

/*
 * Schedule an RCU callback if no RCU callback is pending. Must be called with
 * the graph lock held.
 */
static void call_rcu_zapped(struct pending_free *pf)
{
	WARN_ON_ONCE(inside_selftest());

	if (list_empty(&pf->zapped))
		return;

	if (delayed_free.scheduled)
		return;

	delayed_free.scheduled = true;

	WARN_ON_ONCE(delayed_free.pf + delayed_free.index != pf);
	delayed_free.index ^= 1;

	call_rcu(&delayed_free.rcu_head, free_zapped_rcu);
}

/* The caller must hold the graph lock. May be called from RCU context. */
static void __free_zapped_classes(struct pending_free *pf)
{
	struct lock_class *class;

	check_data_structures();

	list_for_each_entry(class, &pf->zapped, lock_entry)
		reinit_class(class);

	list_splice_init(&pf->zapped, &free_lock_classes);

#ifdef CONFIG_PROVE_LOCKING
	bitmap_andnot(lock_chains_in_use, lock_chains_in_use,
		      pf->lock_chains_being_freed, ARRAY_SIZE(lock_chains));
	bitmap_clear(pf->lock_chains_being_freed, 0, ARRAY_SIZE(lock_chains));
#endif
}

static void free_zapped_rcu(struct rcu_head *ch)
{
	struct pending_free *pf;
	unsigned long flags;

	if (WARN_ON_ONCE(ch != &delayed_free.rcu_head))
		return;

	raw_local_irq_save(flags);
	lockdep_lock();

	/* closed head */
	pf = delayed_free.pf + (delayed_free.index ^ 1);
	__free_zapped_classes(pf);
	delayed_free.scheduled = false;

	/*
	 * If there's anything on the open list, close and start a new callback.
	