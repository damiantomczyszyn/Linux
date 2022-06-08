atic void torture_rwsem_up_read(int tid __maybe_unused)
__releases(torture_rwsem)
{
	up_read(&torture_rwsem);
}

static struct lock_torture_ops rwsem_lock_ops = {
	.writelock	= torture_rwsem_down_write,
	.write_delay	= torture_rwsem_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_rwsem_up_write,
	.readlock       = torture_rwsem_down_read,
	.read_delay     = torture_rwsem_read_delay,
	.readunlock     = torture_rwsem_up_read,
	.name		= "rwsem_lock"
};

#include <linux/percpu-rwsem.h>
static struct percpu_rw_semaphore pcpu_rwsem;

static void torture_percpu_rwsem_init(void)
{
	BUG_ON(percpu_init_rwsem(&pcpu_rwsem));
}

static void torture_percpu_rwsem_exit(void)
{
	percpu_free_rwsem(&pcpu_rwsem);
}

static int torture_percpu_rwsem_down_write(int tid __maybe_unused)
__acquires(pcpu_rwsem)
{
	percpu_down_write(&pcpu_rwsem);
	return 0;
}

static void torture_percpu_rwsem_up_write(int tid __maybe_unused)
__releases(pcpu_rwsem)
{
	percpu_up_write(&pcpu_rwsem);
}

static int torture_percpu_rwsem_down_read(int tid __maybe_unused)
__acquires(pcpu_rwsem)
{
	percpu_down_read(&pcpu_rwsem);
	return 0;
}

static void torture_percpu_rwsem_up_read(int tid __maybe_unused)
__releases(pcpu_rwsem)
{
	percpu_up_read(&pcpu_rwsem);
}

static struct lock_torture_ops percpu_rwsem_lock_ops = {
	.init		= torture_percpu_rwsem_init,
	.exit		= torture_percpu_rwsem_exit,
	.writelock	= torture_percpu_rwsem_down_write,
	.write_delay	= torture_rwsem_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_percpu_rwsem_up_write,
	.readlock       = torture_percpu_rwsem_down_read,
	.read_delay     = torture_rwsem_read_delay,
	.readunlock     = torture_percpu_rwsem_up_read,
	.name		= "percpu_rwsem_lock"
};

/*
 * Lock torture writer kthread.  Repeatedly acquires and releases
 * the lock, checking for duplicate acquisitions.
 */
static int lock_torture_writer(void *arg)
{
	struct lock_stress_stats *lwsp = arg;
	int tid = lwsp - cxt.lwsa;
	DEFINE_TORTURE_RANDOM(rand);

	VERBOSE_TOROUT_STRING("lock_torture_writer task started");
	set_user_nice(current, MAX_NICE);

	do {
		if ((torture_random(&rand) & 0xfffff) == 0)
			schedule_timeout_uninterruptible(1);

		cxt.cur_ops->task_boost(&rand);
		cxt.cur_ops->writelock(tid);
		if (WARN_ON_ONCE(lock_is_write_held))
			lwsp->n_lock_fail++;
		lock_is_write_held = true;
		if (WARN_ON_ONCE(atomic_read(&lock_is_read_held)))
			lwsp->n_lock_fail++; /* rare, but... */

		lwsp->n_lock_acquired++;
		cxt.cur_ops->write_delay(&rand);
		lock_is_write_held = false;
		WRITE_ONCE(last_lock_release, jiffies);
		cxt.cur_ops->writeunlock(tid);

		stutter_wait("lock_torture_writer");
	} while (!torture_must_stop());

	cxt.cur_ops->task_boost(NULL); /* reset prio */
	torture_kthread_stopping("lock_torture_writer");
	return 0;
}

/*
 * Lock torture reader kthread.  Repeatedly acquires and releases
 * the reader lock.
 */
static int lock_torture_reader(void *arg)
{
	struct lock_stress_stats *lrsp = arg;
	int tid = lrsp - cxt.lrsa;
	DEFINE_TORTURE_RANDOM(rand);

	VERBOSE_TOROUT_STRING("lock_torture_reader task started");
	set_user_nice(current, MAX_NICE);

	do {
		if ((torture_random(&rand) & 0xfffff) == 0)
			schedule_timeout_uninterruptible(1);

		cxt.cur_ops->readlock(tid);
		atomic_inc(&lock_is_read_held);
		if (WARN_ON_ONCE(lock_is_write_held))
			lrsp->n_lock_fail++; /* rare, but... */

		lrsp->n_lock_acquired++;
		cxt.cur_ops->read_delay(&rand);
		atomic_dec(&lock_is_read_held);
		cxt.cur_ops->readunlock(tid);

		stutter_wait("lock_torture_reader");
	} while (!torture_must_stop());
	torture_kthread_stopping("lock_torture_reader");
	return 0;
}

/*
 * Create an lock-torture-statistics message in the specified buffer.
 */
static void __torture_print_stats(char *page,
				  struct lock_stress_stats *statp, bool write)
{
	long cur;
	bool fail = false;
	int i, n_stress;
	long max = 0, min = statp ? data_race(statp[0].n_lock_acquired) : 0;
	long long sum = 0;

	n_stress = write ? cxt.nrealwriters_stress : cxt.nrealreaders_stress;
	for (i = 0; i < n_stress; i++) {
		if (data_race(statp[i].n_lock_fail))
			fail = true;
		cur = data_race(statp[i].n_lock_acquired);
		sum += cur;
		if (max < cur)
			max = cur;
		if (min > cur)
			min = cur;
	}
	page += sprintf(page,
			"%s:  Total: %lld  Max/Min: %ld/%ld %s  Fail: %d %s\n",
			write ? "Writes" : "Reads ",
			sum, max, min,
			!onoff_interval && max / 2 > min ? "???" : "",
			fail, fail ? "!!!" : "");
	if (fail)
		atomic_inc(&cxt.n_lock_torture_errors);
}

/*
 * Print torture statistics.  Caller must ensure that there is only one
 * call to this function at a given time!!!  This is normally accomplished
 * by relying on the module system to only have one copy of the module
 * loaded, and then by 