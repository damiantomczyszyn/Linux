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
	percpu_free_rwsem(&pcpu_r