ires(torture_rwlock)
{
	unsigned long flags;

	write_lock_irqsave(&torture_rwlock, flags);
	cxt.cur_ops->flags = flags;
	return 0;
}

static void torture_rwlock_write_unlock_irq(int tid __maybe_unused)
__releases(torture_rwlock)
{
	write_unlock_irqrestore(&torture_rwlock, cxt.cur_ops->flags);
}

static int torture_rwlock_read_lock_irq(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	unsigned long flags;

	read_lock_irqsave(&torture_rwlock, flags);
	cxt.cur_ops->flags = flags;
	return 0;
}

static void torture_rwlock_read_unlock_irq(int tid __maybe_unused)
__releases(torture_rwlock)
{
	read_unlock_irqrestore(&torture_rwlock, cxt.cur_ops->flags)