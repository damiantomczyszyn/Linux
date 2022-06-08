(void *)&count, noop_count, NULL, &target_entry);

	return count;
}
unsigned long lockdep_count_forward_deps(struct lock_class *class)
{
	unsigned long ret, flags;
	struct lock_list this;

	__bfs_init_root(&this, class);

	raw_local_irq_save(flags);
	lockdep_lock();
	ret = __lockdep_count_forward_deps(&this);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	return ret;
}

static unsigned long __lockdep_count_backward_deps(struct lock_list *this)
{
	unsigned long  count = 0;
	struct lock_list *target_entry;

	__bfs_bac