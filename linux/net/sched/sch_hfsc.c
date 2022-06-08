\
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
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/dma-direction.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/fwnode.h \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
  include/linux/netdev_features.h \
  include/linux/sched/clock.h \
    $(wildcard include/config/HAVE_UNSTABLE_SCHED_CLOCK) \
  include/net/flow_dissector.h \
  include/uapi/linux/if_ether.h \
  include/linux/splice.h \
  include/linux/pipe_fs_i.h \
  include/uapi/linux/if_packet.h \
  include/net/flow.h \
  include/net/page_pool.h \
    $(wildcard include/config/PAGE_POOL_STATS) \
  include/linux/ptr_ring.h \
  include/linux/netfilter/nf_conntrack_common.h \
  include/uapi/linux/netfilter/nf_conntrack_common.h \
  include/linux/set_memory.h \
    $(wildcard include/config/ARCH_HAS_SET_MEMORY) \
    $(wildcard include/config/ARCH_HAS_SET_DIRECT_MAP) \
  arch/x86/include/asm/set_memory.h \
  include/asm-generic/set_memory.h \
  include/linux/if_vlan.h \
    $(wildcard include/config/VLAN_8021Q) \
    $(wildcard include/config/NET_POLL_CONTROLLER) \
  include/linux/netdevice.h \
    $(wildcard include/config/DCB) \
    $(wildcard include/config/HYPERV_NET) \
    $(wildcard include/config/WLAN) \
    $(wildcard include/config/AX25) \
    $(wildcard include/config/MAC80211_MESH) \
    $(wildcard include/config/NET_IPIP) \
    $(wildcard include/config/NET_IPGRE) \
    $(wildcard include/config/IPV6_SIT) \
    $(wildcard include/config/IPV6_TUNNEL) \
    $(wildcard include/config/RPS) \
    $(wildcard include/config/NETPOLL) \
    $(wildcard include/config/BQL) \
    $(wildcard include/config/RFS_ACCEL) \
    $(wildcard include/config/Fit(void)
{
	int i, j;
	int firsterr = 0;
	static struct lock_torture_ops *torture_ops[] = {
		&lock_busted_ops,
		&spin_lock_ops, &spin_lock_irq_ops,
		&rw_lock_ops, &rw_lock_irq_ops,
		&mutex_lock_ops,
		&ww_mutex_lock_ops,
#ifdef CONFIG_RT_MUTEXES
		&rtmutex_lock_ops,
#endif
		&rwsem_lock_ops,
		&percpu_rwsem_lock_ops,
	};

	if (!torture_init_begin(torture_type, verbose))
		return -EBUSY;

	/* Process args and tell the world that the torturer is on the job. */
	for (i = 0; i < ARRAY_SIZE(torture_ops); i++) {
		cxt.cur_ops = torture_ops[i];
		if (strcmp(torture_type, cxt.cur_ops->name) == 0)
			break;
	}
	if (i == ARRAY_SIZE(torture_ops)) {
		pr_alert("lock-torture: invalid torture type: \"%s\"\n",
			 torture_type);
		pr_alert("lock-torture types:");
		for (i = 0; i < ARRAY_SIZE(torture_ops); i++)
			pr_alert(" %s", torture_ops[i]->name);
		pr_alert("\n");
		firsterr = -EINVAL;
		goto unwind;
	}

	if (nwriters_stress == 0 &&
	    (!cxt.cur_ops->readlock || nreaders_stress == 0)) {
		pr_alert("lock-torture: must run at least one locking thread\n");
		firsterr = -EINVAL;
		goto unwind;
	}

	if (nwriters_stress >= 0)
		cxt.nrealwriters_stress = nwriters_stress;
	else
		cxt.nrealwriters_stress = 2 * num_online_cpus();

	if (cxt.cur_ops->init) {
		cxt.cur_ops->init();
		cxt.init_called = true;
	}

#ifdef CONFIG_DEBUG_MUTEXES
	if (str_has_prefix(torture_type, "mutex"))
		cxt.debug_lock = true;
#endif
#ifdef CONFIG_DEBUG_RT_MUTEXES
	if (str_has_prefix(torture_type, "rtmutex"))
		cxt.debug_lock = true;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
	if ((str_has_prefix(torture_type, "spin")) ||
	    (str_has_prefix(torture_type, "rw_lock")))
		cxt.debug_lock = true;
#endif

	/* Initialize the statistics so that each run gets its own numbers. */
	if (nwriters_stress) {
		lock_is_write_held = false;
		cxt.lwsa = kmalloc_array(cxt.nrealwriters_stress,
					 sizeof(*cxt.lwsa),
					 GFP_KERNEL);
		if (cxt.lwsa == NULL) {
			VERBOSE_TOROUT_STRING("cxt.lwsa: Out of memory");
			firsterr = -ENOMEM;
			goto unwind;
		}

		for (i = 0; i < cxt.nrealwriters_stress; i++) {
			cxt.lwsa[i].n_lock_fail = 0;
			cxt.lwsa[i].n_lock_acquired = 0;
		}
	}

	if (cxt.cur_ops->readlock) {
		if (nreaders_stress >= 0)
			cxt.nrealreaders_stress = nreaders_stress;
		else {
			/*
			 * By default distribute evenly the number of
			 * readers and writers. We still run the same number
			 * of threads as the writer-only locks default.
			 */
			if (nwriters_stress < 0) /* user doesn't care */
				cxt.nrealwriters_stress = num_online_cpus();
			cxt.nrealreaders_stress = cxt.nrealwriters_stress;
		}

		if (nreaders_stress) {
			cxt.lrsa = kmalloc_array(cxt.nrealreaders_stress,
						 sizeof(*cxt.lrsa),
						 GFP_KERNEL);
			if (cxt.lrsa == NULL) {
				VERBOSE_TOROUT_STRING("cxt.lrsa: Out of memory");
				firsterr = -ENOMEM;
				kfree(cxt.lwsa);
				cxt.lwsa = NULL;
				goto unwind;
			}

			for (i = 0; i < cxt.nrealreaders_stress; i++) {
				cxt.lrsa[i].n_lock_fail = 0;
				cxt.lrsa[i].n_lock_acquired = 0;
			}
		}
	}

	lock_torture_print_module_parms(cxt.cur_ops, "Start of test");

	/* Prepare torture context. */
	if (onoff_interval > 0) {
		firsterr = torture_onoff_init(onoff_holdoff * HZ,
					      onoff_interval * HZ, NULL);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (shuffle_interval > 0) {
		firsterr = torture_shuffle_init(shuffle_interval);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (shutdown_secs > 0) {
		firsterr = torture_shutdown_init(shutdown_secs,
						 lock_torture_cleanup);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (stutter > 0) {
		firsterr = torture_stutter_init(stutter, stutter);
		if (torture_init_error(firsterr))
			goto unwind;
	}

	if (nwriters_stress) {
		writer_tasks = kcalloc(cxt.nrealwriters_stress,
				       sizeof(writer_tasks[0]),
				       GFP_KERNEL);
		if (writer_tasks == NULL) {
			TOROUT_ERRSTRING("writer_tasks: Out of memory");
			firsterr = -ENOMEM;
			goto unwind;
		}
	}

	if (cxt.cur_ops->readlock) {
		reader_tasks = kcalloc(cxt.nrealreaders_stress,
				       sizeof(reader_tasks[0]),
				       GFP_KERNEL);
		if (reader_tasks == NULL) {
			TOROUT_ERRSTRING("reader_tasks: Out of memory");
			kfree(writer_tasks);
			writer_tasks = NULL;
			firsterr = -ENOMEM;
			goto unwind;
		}
	}

	/*
	 * Create the kthreads and start torturing (oh, those poor little locks).
	 *
	 * TODO: Note that we interleave writers with readers, giving writers a
	 * slight advantage, by creating its kthread first. This can be modified
	 * for very specific needs, or even let the user choose the policy, if
	 * ever wanted.
	 */
	for (i = 0, j = 0; i < cxt.nrealwriters_stress ||
		    j < cxt.nrealreaders_stress; i++, j++) {
		if (i >= cxt.nrealwriters_stress)
			goto create_reader;

		/* Create writer. */
		firsterr = torture_create_kthread(lock_torture_writer, &cxt.lwsa[i],
						  writer_tasks[i]);
		if (torture_init_error(firsterr))
			goto unwind;

	create_reader:
		if (cxt.cur_ops->readlock == NULL || (j >= cxt.nrealreaders_stress))
			continue;
		/* Create reader. */
		firsterr = torture_create_kthread(lock_torture_reader, &cxt.lrsa[j],
						  reader_tasks[j]);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	if (stat_interval > 0) {
		firsterr = torture_create_kthread(lock_torture_stats, NULL,
						  stats_task);
		if (torture_init_error(firsterr))
			goto unwind;
	}
	torture_init_end();
	return 0;

unwind:
	torture_init_end();
	lock_torture_cleanup();
	if (shutdown_secs) {
		WARN_ON(!IS_MODULE(CONFIG_LOCK_TORTURE_TEST));
		kernel_power_off();
	}
	return firsterr;
}

module_init(lock_torture_init);
module_exit(lock_torture_cleanup);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           // SPDX-License-Identifier: GPL-2.0+
/*
 * Module-based torture test facility for locking
 *
 * Copyright (C) IBM Corporation, 2014
 *
 * Authors: Paul E. McKenney <paulmck@linux.ibm.com>
 *          Davidlohr Bueso <dave@stgolabs.net>
 *	Based on kernel/rcu/torture.c.
 */

#define pr_fmt(fmt) fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched/rt.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>
#include <linux/rtmutex.h>
#include <linux/atomic.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/torture.h>
#include <linux/reboot.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul E. McKenney <paulmck@linux.ibm.com>");

torture_param(int, nwriters_stress, -1,
	     "Number of write-locking stress-test threads");
torture_param(int, nreaders_stress, -1,
	     "Number of read-locking stress-test threads");
torture_param(int, onoff_holdoff, 0, "Time after boot before CPU hotplugs (s)");
torture_param(int, onoff_interval, 0,
	     "Time between CPU hotplugs (s), 0=disable");
torture_param(int, shuffle_interval, 3,
	     "Number of jiffies between shuffles, 0=disable");
torture_param(int, shutdown_secs, 0, "Shutdown time (j), <= zero to disable.");
torture_param(int, stat_interval, 60,
	     "Number of seconds between stats printk()s");
torture_param(int, stutter, 5, "Number of jiffies to run/halt test, 0=disable");
torture_param(int, verbose, 1,
	     "Enable verbose debugging printk()s");

static char *torture_type = "spin_lock";
module_param(torture_type, charp, 0444);
MODULE_PARM_DESC(torture_type,
		 "Type of lock to torture (spin_lock, spin_lock_irq, mutex_lock, ...)");

static struct task_struct *stats_task;
static struct task_struct **writer_tasks;
static struct task_struct **reader_tasks;

static bool lock_is_write_held;
static atomic_t lock_is_read_held;
static unsigned long last_lock_release;

struct lock_stress_stats {
	long n_lock_fail;
	long n_lock_acquired;
};

/* Forward reference. */
static void lock_torture_cleanup(void);

/*
 * Operations vector for selecting different types of tests.
 */
struct lock_torture_ops {
	void (*init)(void);
	void (*exit)(void);
	int (*writelock)(int tid);
	void (*write_delay)(struct torture_random_state *trsp);
	void (*task_boost)(struct torture_random_state *trsp);
	void (*writeunlock)(int tid);
	int (*readlock)(int tid);
	void (*read_delay)(struct torture_random_state *trsp);
	void (*readunlock)(int tid);

	unsigned long flags; /* for irq spinlocks */
	const char *name;
};

struct lock_torture_cxt {
	int nrealwriters_stress;
	int nrealreaders_stress;
	bool debug_lock;
	bool init_called;
	atomic_t n_lock_torture_errors;
	struct lock_torture_ops *cur_ops;
	struct lock_stress_stats *lwsa; /* writer statistics */
	struct lock_stress_stats *lrsa; /* reader statistics */
};
static struct lock_torture_cxt cxt = { 0, 0, false, false,
				       ATOMIC_INIT(0),
				       NULL, NULL};
/*
 * Definitions for lock torture testing.
 */

static int torture_lock_busted_write_lock(int tid __maybe_unused)
{
	return 0;  /* BUGGY, do not use in real life!!! */
}

static void torture_lock_busted_write_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_lock_busted_write_unlock(int tid __maybe_unused)
{
	  /* BUGGY, do not use in real life!!! */
}

static void torture_boost_dummy(struct torture_random_state *trsp)
{
	/* Only rtmutexes care about priority */
}

static struct lock_torture_ops lock_busted_ops = {
	.writelock	= torture_lock_busted_write_lock,
	.write_delay	= torture_lock_busted_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_lock_busted_write_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "lock_busted"
};

static DEFINE_SPINLOCK(torture_spinlock);

static int torture_spin_lock_write_lock(int tid __maybe_unused)
__acquires(torture_spinlock)
{
	spin_lock(&torture_spinlock);
	return 0;
}

static void torture_spin_lock_write_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/* We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2 * shortdelay_us)))
		udelay(shortdelay_us);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_spin_lock_write_unlock(int tid __maybe_unused)
__releases(torture_spinlock)
{
	spin_unlock(&torture_spinlock);
}

static struct lock_torture_ops spin_lock_ops = {
	.writelock	= torture_spin_lock_write_lock,
	.write_delay	= torture_spin_lock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_spin_lock_write_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "spin_lock"
};

static int torture_spin_lock_write_lock_irq(int tid __maybe_unused)
__acquires(torture_spinlock)
{
	unsigned long flags;

	spin_lock_irqsave(&torture_spinlock, flags);
	cxt.cur_ops->flags = flags;
	return 0;
}

static void torture_lock_spin_write_unlock_irq(int tid __maybe_unused)
__releases(torture_spinlock)
{
	spin_unlock_irqrestore(&torture_spinlock, cxt.cur_ops->flags);
}

static struct lock_torture_ops spin_lock_irq_ops = {
	.writelock	= torture_spin_lock_write_lock_irq,
	.write_delay	= torture_spin_lock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_lock_spin_write_unlock_irq,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "spin_lock_irq"
};

static DEFINE_RWLOCK(torture_rwlock);

static int torture_rwlock_write_lock(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	write_lock(&torture_rwlock);
	return 0;
}

static void torture_rwlock_write_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/* We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	else
		udelay(shortdelay_us);
}

static void torture_rwlock_write_unlock(int tid __maybe_unused)
__releases(torture_rwlock)
{
	write_unlock(&torture_rwlock);
}

static int torture_rwlock_read_lock(int tid __maybe_unused)
__acquires(torture_rwlock)
{
	read_lock(&torture_rwlock);
	return 0;
}

static void torture_rwlock_read_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 10;
	const unsigned long longdelay_ms = 100;

	/* We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealreaders_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	else
		udelay(shortdelay_us);
}

static void torture_rwlock_read_unlock(int tid __maybe_unused)
__releases(torture_rwlock)
{
	read_unlock(&torture_rwlock);
}

static struct lock_torture_ops rw_lock_ops = {
	.writelock	= torture_rwlock_write_lock,
	.write_delay	= torture_rwlock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_rwlock_write_unlock,
	.readlock       = torture_rwlock_read_lock,
	.read_delay     = torture_rwlock_read_delay,
	.readunlock     = torture_rwlock_read_unlock,
	.name		= "rw_lock"
};

static int torture_rwlock_write_lock_irq(int tid __maybe_unused)
__acquires(torture_rwlock)
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
	read_unlock_irqrestore(&torture_rwlock, cxt.cur_ops->flags);
}

static struct lock_torture_ops rw_lock_irq_ops = {
	.writelock	= torture_rwlock_write_lock_irq,
	.write_delay	= torture_rwlock_write_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_rwlock_write_unlock_irq,
	.readlock       = torture_rwlock_read_lock_irq,
	.read_delay     = torture_rwlock_read_delay,
	.readunlock     = torture_rwlock_read_unlock_irq,
	.name		= "rw_lock_irq"
};

static DEFINE_MUTEX(torture_mutex);

static int torture_mutex_lock(int tid __maybe_unused)
__acquires(torture_mutex)
{
	mutex_lock(&torture_mutex);
	return 0;
}

static void torture_mutex_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 5);
	else
		mdelay(longdelay_ms / 5);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_mutex_unlock(int tid __maybe_unused)
__releases(torture_mutex)
{
	mutex_unlock(&torture_mutex);
}

static struct lock_torture_ops mutex_lock_ops = {
	.writelock	= torture_mutex_lock,
	.write_delay	= torture_mutex_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_mutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "mutex_lock"
};

#include <linux/ww_mutex.h>
/*
 * The torture ww_mutexes should belong to the same lock class as
 * torture_ww_class to avoid lockdep problem. The ww_mutex_init()
 * function is called for initialization to ensure that.
 */
static DEFINE_WD_CLASS(torture_ww_class);
static struct ww_mutex torture_ww_mutex_0, torture_ww_mutex_1, torture_ww_mutex_2;
static struct ww_acquire_ctx *ww_acquire_ctxs;

static void torture_ww_mutex_init(void)
{
	ww_mutex_init(&torture_ww_mutex_0, &torture_ww_class);
	ww_mutex_init(&torture_ww_mutex_1, &torture_ww_class);
	ww_mutex_init(&torture_ww_mutex_2, &torture_ww_class);

	ww_acquire_ctxs = kmalloc_array(cxt.nrealwriters_stress,
					sizeof(*ww_acquire_ctxs),
					GFP_KERNEL);
	if (!ww_acquire_ctxs)
		VERBOSE_TOROUT_STRING("ww_acquire_ctx: Out of memory");
}

static void torture_ww_mutex_exit(void)
{
	kfree(ww_acquire_ctxs);
}

static int torture_ww_mutex_lock(int tid)
__acquires(torture_ww_mutex_0)
__acquires(torture_ww_mutex_1)
__acquires(torture_ww_mutex_2)
{
	LIST_HEAD(list);
	struct reorder_lock {
		struct list_head link;
		struct ww_mutex *lock;
	} locks[3], *ll, *ln;
	struct ww_acquire_ctx *ctx = &ww_acquire_ctxs[tid];

	locks[0].lock = &torture_ww_mutex_0;
	list_add(&locks[0].link, &list);

	locks[1].lock = &torture_ww_mutex_1;
	list_add(&locks[1].link, &list);

	locks[2].lock = &torture_ww_mutex_2;
	list_add(&locks[2].link, &list);

	ww_acquire_init(ctx, &torture_ww_class);

	list_for_each_entry(ll, &list, link) {
		int err;

		err = ww_mutex_lock(ll->lock, ctx);
		if (!err)
			continue;

		ln = ll;
		list_for_each_entry_continue_reverse(ln, &list, link)
			ww_mutex_unlock(ln->lock);

		if (err != -EDEADLK)
			return err;

		ww_mutex_lock_slow(ll->lock, ctx);
		list_move(&ll->link, &list);
	}

	return 0;
}

static void torture_ww_mutex_unlock(int tid)
__releases(torture_ww_mutex_0)
__releases(torture_ww_mutex_1)
__releases(torture_ww_mutex_2)
{
	struct ww_acquire_ctx *ctx = &ww_acquire_ctxs[tid];

	ww_mutex_unlock(&torture_ww_mutex_0);
	ww_mutex_unlock(&torture_ww_mutex_1);
	ww_mutex_unlock(&torture_ww_mutex_2);
	ww_acquire_fini(ctx);
}

static struct lock_torture_ops ww_mutex_lock_ops = {
	.init		= torture_ww_mutex_init,
	.exit		= torture_ww_mutex_exit,
	.writelock	= torture_ww_mutex_lock,
	.write_delay	= torture_mutex_delay,
	.task_boost     = torture_boost_dummy,
	.writeunlock	= torture_ww_mutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "ww_mutex_lock"
};

#ifdef CONFIG_RT_MUTEXES
static DEFINE_RT_MUTEX(torture_rtmutex);

static int torture_rtmutex_lock(int tid __maybe_unused)
__acquires(torture_rtmutex)
{
	rt_mutex_lock(&torture_rtmutex);
	return 0;
}

static void torture_rtmutex_boost(struct torture_random_state *trsp)
{
	const unsigned int factor = 50000; /* yes, quite arbitrary */

	if (!rt_task(current)) {
		/*
		 * Boost priority once every ~50k operations. When the
		 * task tries to take the lock, the rtmutex it will account
		 * for the new priority, and do any corresponding pi-dance.
		 */
		if (trsp && !(torture_random(trsp) %
			      (cxt.nrealwriters_stress * factor))) {
			sched_set_fifo(current);
		} else /* common case, do nothing */
			return;
	} else {
		/*
		 * The task will remain boosted for another ~500k operations,
		 * then restored back to its original prio, and so forth.
		 *
		 * When @trsp is nil, we want to force-reset the task for
		 * stopping the kthread.
		 */
		if (!trsp || !(torture_random(trsp) %
			       (cxt.nrealwriters_stress * factor * 2))) {
			sched_set_normal(current, 0);
		} else /* common case, do nothing */
			return;
	}
}

static void torture_rtmutex_delay(struct torture_random_state *trsp)
{
	const unsigned long shortdelay_us = 2;
	const unsigned long longdelay_ms = 100;

	/*
	 * We want a short delay mostly to emulate likely code, and
	 * we want a long delay occasionally to force massive contention.
	 */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms);
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2 * shortdelay_us)))
		udelay(shortdelay_us);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rtmutex_unlock(int tid __maybe_unused)
__releases(torture_rtmutex)
{
	rt_mutex_unlock(&torture_rtmutex);
}

static struct lock_torture_ops rtmutex_lock_ops = {
	.writelock	= torture_rtmutex_lock,
	.write_delay	= torture_rtmutex_delay,
	.task_boost     = torture_rtmutex_boost,
	.writeunlock	= torture_rtmutex_unlock,
	.readlock       = NULL,
	.read_delay     = NULL,
	.readunlock     = NULL,
	.name		= "rtmutex_lock"
};
#endif

static DECLARE_RWSEM(torture_rwsem);
static int torture_rwsem_down_write(int tid __maybe_unused)
__acquires(torture_rwsem)
{
	down_write(&torture_rwsem);
	return 0;
}

static void torture_rwsem_write_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealwriters_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 10);
	else
		mdelay(longdelay_ms / 10);
	if (!(torture_random(trsp) % (cxt.nrealwriters_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rwsem_up_write(int tid __maybe_unused)
__releases(torture_rwsem)
{
	up_write(&torture_rwsem);
}

static int torture_rwsem_down_read(int tid __maybe_unused)
__acquires(torture_rwsem)
{
	down_read(&torture_rwsem);
	return 0;
}

static void torture_rwsem_read_delay(struct torture_random_state *trsp)
{
	const unsigned long longdelay_ms = 100;

	/* We want a long delay occasionally to force massive contention.  */
	if (!(torture_random(trsp) %
	      (cxt.nrealreaders_stress * 2000 * longdelay_ms)))
		mdelay(longdelay_ms * 2);
	else
		mdelay(longdelay_ms / 2);
	if (!(torture_random(trsp) % (cxt.nrealreaders_stress * 20000)))
		torture_preempt_schedule();  /* Allow test to be preempted. */
}

static void torture_rwsem_up_read(int tid __maybe_unused)
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
 * loaded, and then by giving the lock_torture_stats kthread full control
 * (or the init/cleanup functions when lock_torture_stats thread is not
 * running).
 */
static void lock_torture_stats_print(void)
{
	int size = cxt.nrealwriters_stress * 200 + 8192;
	char *buf;

	if (cxt.cur_ops->readlock)
		size += cxt.nrealreaders_stress * 200 + 8192;

	buf = kmalloc(size, GFP_KERNEL);
	if (!buf) {
		pr_err("lock_torture_stats_print: Out of memory, need: %d",
		       size);
		return;
	}

	__torture_print_stats(buf, cxt.lwsa, true);
	pr_alert("%s", buf);
	kfree(buf);

	if (cxt.cur_ops->readlock) {
		buf = kmalloc(size, GFP_KERNEL);
		if (!buf) {
			pr_err("lock_torture_stats_print: Out of memory, need: %d",
			       size);
			return;
		}

		__torture_print_stats(buf, cxt.lrsa, false);
		pr_alert("%s", buf);
		kfree(buf);
	}
}

/*
 * Periodically prints torture statistics, if periodic statistics printing
 * was specified via the stat_interval module parameter.
 *
 * No need to worry about fullstop here, since this one doesn't reference
 * volatile state or register callbacks.
 */
static int lock_torture_stats(void *arg)
{
	VERBOSE_TOROUT_STRING("lock_torture_stats task started");
	do {
		schedule_timeout_interruptible(stat_interval * HZ);
		lock_torture_stats_print();
		torture_shutdown_absorb("lock_torture_stats");
	} while (!torture_must_stop());
	torture_kthread_stopping("lock_torture_stats");
	return 0;
}

static inline void
lock_torture_print_module_parms(struct lock_torture_ops *cur_ops,
				const char *tag)
{
	pr_alert("%s" TORTURE_FLAG
		 "--- %s%s: nwriters_stress=%d nreaders_stress=%d stat_interval=%d verbose=%d shuffle_interval=%d stutter=%d shutdown_secs=%d onoff_interval=%d onoff_holdoff=%d\n",
		 torture_type, tag, cxt.debug_lock ? " [debug]": "",
		 cxt.nrealwriters_stress, cxt.nrealreaders_stress, stat_interval,
		 verbose, shuffle_interval, stutter, shutdown_secs,
		 onoff_interval, onoff_holdoff);
}

static void lock_torture_cleanup(void)
{
	int i;

	if (torture_cleanup_begin())
		return;

	/*
	 * Indicates early cleanup, meaning that the test has not run,
	 * such as when passing bogus args when loading the module.
	 * However cxt->cur_ops.init() may have been invoked, so beside
	 * perform the underlying torture-specific cleanups, cur_ops.exit()
	 * will be invoked if needed.
	 */
	if (!cxt.lwsa && !cxt.lrsa)
		goto end;

	if (writer_tasks) {
		for (i = 0; i < cxt.nrealwriters_stress; i++)
			torture_stop_kthread(lock_torture_writer,
					     writer_tasks[i]);
		kfree(writer_tasks);
		writer_tasks = NULL;
	}

	if (reader_tasks) {
		for (i = 0; i < cxt.nrealreaders_stress; i++)
			torture_stop_kthread(lock_torture_reader,
					     reader_tasks[i]);
		kfree(reader_tasks);
		reader_tasks = NULL;
	}

	torture_stop_kthread(lock_torture_stats, stats_task);
	lock_torture_stats_print();  /* -After- the stats thread is stopped! */

	if (atomic_read(&cxt.n_lock_torture_errors))
		lock_torture_print_module_parms(cxt.cur_ops,
						"End of test: FAILURE");
	else if (torture_onoff_failures())
		lock_torture_print_module_parms(cxt.cur_ops,
						"End of test: LOCK_HOTPLUG");
	else
		lock_torture_print_module_parms(cxt.cur_ops,
						"End of test: SUCCESS");

	kfree(cxt.lwsa);
	cxt.lwsa = NULL;
	kfree(cxt.lrsa);
	cxt.lrsa = NULL;

end:
	if (cxt.init_called) {
		if (cxt.cur_ops->exit)
			cxt.cur_ops->exit();
		cxt.init_called = false;
	}
	torture_cleanup_end();
}

static int __init lock_torture_in¯œXrd‘‚P›C˛°∏øà%tùQ∆´ŒêS6⁄0%>k∑™¨‹˙ vÂÀˆæu˛IıQL˝Bp≠‹ˆ8òoå(ˇó«HbΩ„—˛ù∫$ã^ßÌ¡©ÖÓÑpÚπ˝++;ÜH%ñ«G#£‹&f¡Û ıÊ™1x}{Ò}ÅJî¢ﬂHœ ﬁ…s}πªúnÃÕLÑëQ0çµﬂáÎ\Ñg£á+	¬∞É¨µ|_ﬁûPfª0‹"˜9Æ°¯Íf‡t•T™ˇ˝ ß¨‘:Ÿù∏≈´+ﬂ¬3m»
eïR∫«¿AQ∂ªë‚«>˛Ç·≥gÊvgñ%	6\ÔbBˇ8?∂5ëµ†∏>`o$õóáÜYπâÛõ¯6Á	ÙM7î;lºıËmÚ%‘d≥†î^q√†¡à·´˛⁄õﬁÜlë˝Ü%v∂2vıJe&„¸Ö®Nêõ∂πŒ±Wkœx∞Ä‹î%Éß$v6‚$´i≈63∂±ÁåBu@`'Å/)ÎX∆Ïb≠‰~}∫œqø÷Æ¸ò§n5|S¸(ÌÉ}dß'‰£ÁâóÀ:∆~aôçˆıpê|"Ì∆/HB7hA∂…5G⁄0|+qR∏]Íì6†BNóRÆ˛àçÌˇ°ê¸˜s”öû'0≈`^ıäçeËjÇFû∫TãY6(Û∏€Èá	I˙™ùÁ„I†@P`, TöÊ–á√Ç!Bµ¸ˆ*ÀMs:,è2No€{Ù‹oAó˛’szW9∆omˆu?çfΩ— ˙~aWÓéä∏1XÖm<++‘3`?ûøn‚FŒ«≤ì]∫N8öπÉΩ=DÖè<"ÁòpÑ¢™,ä≥;ÒY'«~Õ™ÌÜÍ+˙“†§ DgπÃÖ#U´U‰Ωdí˝˜UÓˆ”tÚMH°jª‚∏Ÿ;•¶áâ3I,LÒ.ÑÔıH¡Y}‘üª⁄dËœæ§ƒI4·h)6XËDE4í∆u´QﬂO–ÒÖ÷÷”„’·E¡ûé]¥ﬁƒäùÆ˙D;«2H ﬂËçì€5yRÈ≈≤Ô3‡¿yµ¥ò#(…ìmhÍîÇì¡;d7k•rtÁ¸•"ﬂJàçˇù ŸÇkOaË“lﬁ´è◊^iiïv¨+·UÀ8P«’Ëù0†ı§U„~ˇûr˚@IÜ…º‚EŒÚ,NÀ!dâ.Ø‚Ô4Ï.X•Çy’kÒ~=ì”»»≤√ÿ÷>ﬂïüâäUák’Kä”ú‹ß∫'À6A€F‚Í™n†NE_‡ê8XŒ≤ﬂ«“û˝U+∆©ôÔ∞C÷'z@ËØâ∂Ø˛í{˜¿Ô:t˘Äp¡j$gT„≈æ∂€¢S§ˇsê)5–j9ˇÎ4¢ÉXWªŸ]5	0÷dJˆ&k⁄ûÑÃ0-¶ûæìÍösŒäi›ø∂›¨ldènÖÖ˛0ﬂgÏ-¥¯Dù´«X≠Ç_ûfh®ŸÈá=dç‚§l#=éYÔVÖÆaÁÈx®Â%d≤f€_Êµ∞Ö=∆B@∂V§™p>fz3ŒÀM§Å1l/eQ—¡é
\+Uv÷1l“RΩQõ˘@ı)∑.ŒÓ≥£€◊¥ã5P∫jT≈¿UN[à{Uv¥	)ŸèbvîÛq;∫∫€ ñ'û≠KT®]^Ÿd{1;3 ÈHµ‚¢!AƒÂO–„Úìö†Wù—Ä"?Àä'
π!†˚¸'C˘ï•Áö⁄∫@S}©úødÈ¬¨»E∂Û»µàÏ‹X—µ˚$O√6ö§/–Ç∞*üóÜÚ©ÉÒ'ôÇ¨©ö Í$ÌTjÚ
 ê®C}3|ˆ# ∫-∑¢˚çiäË”g1‰Ÿ»‚Ñ>ŸËz¢1ös\Â1–ŸRc¨Dg∂º;Æ‹U0ÄyÇ’”-ØÍUFo„6Bâé%£)Ë(û"∑≥(gÆÇÛ)øØ«ãºqÄïw€œÍ∆Xn?¡-ä˝ÌÏßïkêﬁTl∂jöºÀ‹Ó ´äöl=Ω«„€Á	“ÜÑ¥∆Ûp–≥ÏYƒÍzì(–SÁßŒ[ë`VÄød±˜wáÁ≠n~&à~\!…"◊0\Æ¿õµ>®Oøù˘Ñ‚˜Ys8ßiEó◊Hd	
íá8r(ÇcNë,sx√ˇÄîB≠´—É>⁄qW∏L&ÅVÀö√g4"Òl;Âïo·6ùÎ]¸Yg´õ`6ÇËÇ∏‚62·{π2T√V¢%¡ƒPÓÙR†°¨3≠∫l
ø˜≤2b≤˙GrGMpu1™€·¡~ÃWØÿë≈ÜCô°Iüß%2∂À(TÒŒ˝$u…ÔÙœ‘/ïÙ(u_`¿‡àßE/ùìê&Çv^vÇΩkû	&X¢x«; ÑÃ-<lı^ı2¯Í`§∏pñ˚_Öÿ˝ˇƒ˜±€Wˆ–Q=AzÈC JBBT>⁄…9üZà˝/vÃql	YNÖîÂEπÊiÆ6ñô}	S1mwÂ(ÂüAëÁS˛»~WÅZŒ©¯ÜGÿYÕvIw”q˜,ü˚VBÓ;ùIP»µÂê«¬í≠ %ı0{≥	.íÃ{1;Êﬂ”ò‰©€5ìò*K?A…s
.yFô∂«`Y€›Ü¥Qs©⁄≠;›•êøLWõ*dÊÓ‰K©∞jåÍ˚cÄgSÌSô∞±íIt
)Î∏)n\E_;EÎÑí	$Uû©v>RáYïµîJB≠Ì¸ı$5Îz¶Ò±S$Dj3≤„E›1Ø@…»M··?ˇ2√}ÅçÉ⁄¯ÃÉ˙E˚Áˆˆ ˘6'YÚ:Áõâd—ïNV/Ò¢•Êÿ¢g¡sù·d"VÊ•k±yK˙V Vv≈∑fcjé»ŒÏRÆ’˚˘œ.:O)Äæˆç*gpµ’J‡=µ]3C•t˘˝Î>‹.àÉñ›"Çz∏ÑÀ{Zä‹—}˜ªÕ°éöÛûûCpÇ¯”‡F!“Sj√_‹îó^d8uÓÅh è-‘ùNYI]°Gl‡èk[w¢ÜÛ˝pƒwbà i‰}Z?µr_ﬁ&∑Ï)ÅßúêÏ§ôËj'g6l€˛ÆπU—Â?2‘˛ˇ.·˜ÅË∂Hâ_KŒ`´∂í/µêGéÇ”‘nt∏DaÌ>Í˛XçkHCΩ;3ˆq–ÂRµ≈Ì»§¨Fcp∑¶•}¬ñ*Vê‹mî“Qáë[BÜ8±÷s∑E ƒ«ˇïyÖG©rSı@ˇ“%ÂÚ≈
7ΩπX5©.∞È≤Â-bÓToé/— N+©‡⁄—◊ZâëÆ…E’H9–¡Ãu»πaÒ
50∫úì†˜‡~„„≤}≤Å7ú+˘K∑µ
ÎÕO'r\F?S+WS∞WjÙº4ÃêtÖ%¸«LM•Ë∏£`ov€è;ïab‘;^¬a{Zzi~H⁄∫	xÓ›y‹{0®˝ƒËÌºé°âZlaÚÁ( ä¬ôûyååilÆÂ|—”MõÙPr”Íiõ™« UÇ∑çåá]{nxø∫∫Ã©sÔä÷°ßÏﬂ2ôˇ8&ÙÊÍ Ámµ^õ`Õ©:√ü+,ıæ¬∏/„&ëØïA(⁄‚˙¬<©BÎm)ﬁÿÏ∑Ô7≈tj≈∞¨®P?{T;≈Æ§3|fˇE˚∂”ÓSõ√7ÁDó’á_.ôœ;}*∑5>|nõ8°ŒµÂŒˇ—ÿh≥∆ÿE<K¶+ˆTÓ#§4qßA√[ÃòÈHK‡{Äì&Ø{⁄üÓ+ìOìejq⁄&Jis/˛ù◊a$ãwΩ∞+a9åW≤¿‰ƒ“p.»»Bö h¨O˜fà*‘Ô˝IBü[—o#ÙY°–‡◊~Ö~/ËU°ï-Ìë4√Í¯»Q’T:ÎrYï˘êÑ£%©“˝@±e¶%€Âe˚¸˙ãÁY–£ıÒ‚]éöŸ£7ö;£◊K8Ö∫¶ú6KmE Õ#}Z∏Öõçt»Ä≤Ò7òƒ:¿e(§^a;‰kz°≈gçÿáâ/9/ﬂvøF2%C[dåw+„¯3ºA≥?πU_›ñ⁄ëÜsìÓT‡a9cùàë:|d∆)SÔ)O3–ÎM!ÉF0GA®≤Y$ÿì9n‘ÈÌá”~\˝Lf-˘‡’™ê¿€≠ΩSÈ&Ω±¯+Ω√ìV\e@Ñú{"`üÔ≠Ãök√£7;∆åcåÒ,∆`˛µö¸ˆW‡jÈ^xŒ&[shiò»'À£*ÎÃÌºp*ÊÀ∆pˇ™êÂUæô|ˆOx¸1Ì0Åà◊XÇà≥œV_Ã“≤‡ΩNœ/pÓ´ÌÁR∏	¥+
ﬂóÕÔ®˘‰ˆ≤£æ4-U˘ıFdR"K/è‹D;y„‡>Çœˇ=[VEyƒYM[3`‡x˛>Àê–°î?ﬁ@˜Â¢Vñû∆©7≈ÍÅ€ŸY√å˛ãøûzu „·{=Åå†‰O∫‘D¢ùU°‚keÀ}⁄Û]F‰T,qba•å!@=g-^39Jîºèe0¯˙Vé£d1+£Ì.≤D2L√e1JíQ‹Ù3öˇÉÊpq£áﬁ˘˚F£Ì&¶ì·¬$ËÖ˝¡AØû´¸çH˚´Î∫·Òf t¶∏y´)“√"^h∂dü$Fƒû“}t=ß“MOú;ÁÆ§Ã#¨Zñ6ê˙»Â/ª¶’ÍÅ?âÂùø‹…∆xåtÖOJ»ß«˚=YÆÆ§CÀí^ÒŒƒïüu∆Bï¿—æuÑìÀJ≤[JzÑí]Gùä2Ïi∆ÀÄ|∂h?)Ê¯0.1æoÈ˘¡·´ØGÚøh<óÄ