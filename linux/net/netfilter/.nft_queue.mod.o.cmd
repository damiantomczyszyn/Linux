(&torture_ww_mutex_0);
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

static int __init lock_torture_in��Xrd��P�C�����%t�QƫΐS6�0%>k������v����u�I�QL�Bp���8�o�(���Hb������$�^������p��++;�H%��G#��&f�����1x}{�}�J���H����s}���n��L��Q0��߇�\�g��+	°���|_ޞPf�0�"�9����f�t�T��� ���:ٝ���+��3m�
e�R���AQ�����>���g�vg�%	6\�bB�8?�5����>`o$����Y����6�	�M7�;l���m�%�d���^qà����ڛކl���%v�2v�Je&����N����αWk�x��ܔ%���$v6�$�i�63���Bu@`'�/)�X��b��~}��q�֮���n5|S�(�}d�'����:�~a����p�|"��/HB7hA��5G�0|+qR�]��6�BN�R����������s���'0�`^���e�j�F��T�Y6(���	I�����I�@P`,�T��ЇÂ!B���*�Ms:,�2No�{��oA���szW9�om�u?�f�� �~aW�1X�m<++�3`?��n�F�ǲ�]�N8����=D��<"��p���,��;�Y'�~ͪ��+��Ҡ� Dg�̅#U�U�d���U���t�MH�j���;����3I,L�.���H�Y}ԟ��d�Ͼ��I4�h)6X�DE4��u�Q�O��������E���]�������D;�2H �荓�5yR�Ų�3��y���#(ɓmhꔂ��;d7k�rt���"�J�����قkOa���l����^ii�v�+�U�8P���0���U�~��r�@I�ɼ�E��,N�!d�.���4�.X��y�k�~=���Ȳ���>ߕ���U�k�K�Ӝܧ�'�6A�F���n�N�E_��8Xβ��Ҟ�U+����C�'z@诉����{���:t��p�j$gT����ۢS��s�)5�j9��4��XW��]5	0�dJ�&kڞ��0-�����sΊiݿ�ݬld�n���0�g�-��D���X��_�fh���=d��l#=�Y�V��a��x��%d�f�_浰�=�B@�V��p>fz3��M��1l/eQ���
\+Uv�1l�R�Q��@�)�.��������5P�jT��UN[�{Uv�	)ُbv��q;��� �'��KT�]^�d{1;3 �H��!A��O������W���"?��'
�!����'C����ں@S}���d����E��ȵ���Xѵ�$O�6��/���*�����'�������$�Tj�
ʐ�C}3|�# �-����i���g1����>��z�1�s\�1��Rc�Dg��;��U0�y���-��UFo�6B��%�)�(�"��(g���)��ǋ�q��w�����Xn?�-���징k��Tl�j����� ���l=�����	҆����pг�Y��z�(�S��[�`V��d��w��n~&�~\!�"�0\����>�O������Ys8�iE��Hd	
��8r(�cN�,sx����B����>�qW�L&�V˚�g4"�l;�o�6��]�Yg��`6�����62�{�2T��V�%��P��R���3��l
���2b��GrGMpu1����~�W�ؑņC��I��%2��(T���$u�����/��(u_`����E/���&�v^v��k�	&X�x�; ��-<l�^�2��`��p��_��������W��Q=Az�C JBBT>��9�Z��/v�ql	YN���E��i�6��}	S1mw�(�A��S��~W�ZΩ��G�Y�vIw�q�,��VB�;�IPȵ����� %�0{�	.��{1;��Ә��5��*K?A�s
.yF���`Y�݆�Qs�ڭ;ݥ��LW�*d���K��j���c�gS�S����It
)�)n\E_;E넒	$U��v>R�Y���JB����$5�z��S$Dj3��E�1�@��M��?�2�}�����̃�E������6'Y�:盉dѕNV/��آg�s��d"V�k�yK�V Vvŷfcj����R�����.:O)����*gp��J�=�]3C�t���>�.����"�z���{Z���}��͡�����Cp����F!�Sj�_ܔ�^d8u��hʏ-ԝNYI]�Gl��k[w����p�wb� i�}Z?�r_�&��)����줙�j'g6l����U��?2���.����H�_K�`���/��G����nt�Da�>��X�kHC�;3�q��R���Ȥ�Fcp���}*V��m��Q��[B�8��s�E ����y�G�rS�@��%���
7��X5�.���-b�To�/� N+�����Z����E�H9���uȹa�
50������~��}��7�+�K��
��O'r\F?S+WS�Wj��4̐t�%��LM�踣`ovۏ;�ab�;^�a{Zzi~Hں	x��y�{0�������Zla��( ��y��il��|��M��Pr��i����U�����]{nx���̩s������2��8&�����m�^�`ͩ:ß+,��¸/�&���A(����<�B�m)����7�tjŰ��P?{T;Ů�3|f�E����S��7�D�Շ_.��;}*�5>|n�8�ε�����h���E<K�+�T�#�4q�A�[̘�HK�{��&�{ڟ�+�O�ejq�&Jis/���a$�w��+a9�W�����p.��B� h�O�f�*���IB�[�o#�Y����~�~/�U��-�4����Q�T:�rY�����%���@�e�%��e�����YУ���]��٣7�;��K8����6KmE��#}Z����tȀ��7��:�e(�^a;�kz��g�؇�/9/�v�F2%C[d�w+��3�A�?�U_ݖ���s��T�a9c���:|d�)S�)O3��M!�F0GA��Y$ؓ9n����~\�Lf-��ժ��ۭ�S�&���+���V\e@��{"`��̚kã7;ƌc��,�`�����W�j�^x�&[shi��'ˣ*���p*���p����U��|�Ox�1�0���X����V_�Ҳ�N�/p���R�	�+
����������4-U��FdR"K/��D;y��>���=[VEy�YM[3`�x�>ːС�?�@��V��Ʃ7�����YÌ����zu ��{=����O��D��U��ke�}��]F�T,qba��!@=g-^39J���e0��V��d1+��.�D2L�e1J�Q��3����pq�����F��&����$���A�����H�����f t��y�)��"^h�d�$FĞ�}t=��MO�;箤�#�Z�6����/����?�������x�t�OJȧ��=Y���C˒^��ĕ�u�B��Ѿu���J�[Jz��]G��2�i�ˀ|�h?)��0.1�o���᫯G�h<���}&�$�����|��
9=��/��!�}5�4o���>�s08L�ٵۃQ�G��t�(�u�#�A��qW�d4D�����GK�br: ���Y�<ܪ�=.b���1׫`w��'�$l1O��&z]Y�$M��UU^�ϋ�?��2�UF2��t�SVe���ӹ��+ �~�rU�h�!��ܨ
�Іc�'�F"zǀ� �7���D�W���Z��6?J��=E���mS�s��q?tM�pS?��}%�)��ó���K�,g��ƞJk=3i;f�E��7�q���s�[^'��8sM�V�8�l��a�G�7��w��hQ
S�Z�+��HL�\��w]'���R���t+b����C�|O�ƿg�ȈR�F��bRI����׉�~��h�vѿule����|�`X���HuN��zm������`@Q�r����]�dC���P�R�|�U��U����;IQ�'�Қ��_~j���y��t{��lLH�#��-�&�H8�٥.��_ED��㎴����0�nd�{���|��K-�Y�>!�CCA%p�m9����?��H;��֏����v�=�q�aN=�P0��|r�� '�cȷiGO@��z��Ӓ�b�3�
��OD�@g8B�R��Z�dk��a�c�,;+ �z'�^&K:��<���F0Z��
 s#l��h��J�m༛Ws��+d��d��$Zn]7��7	���|���5BsKN�zn�;�y����	���*��I�-w2���#�g��t�k|�F���h��<����yέW4��0Ԫ"s>�F��h��Z���U�sψ!g,��&��p�?X3`C�������{W�j�t����h�|c9P�Ǉ%���Yq���W�8�?��˓Jb@%+�<�cEܚ����j?Wz˟�}D�;�Z��Հ��Q_�����6DLd�b�cf���X|��>����%���g%҂l�D^�,"V��Y��ZT�f<ξ���%*��m5V�s�.���#�X�H� ��cI�j�!�������J�"�,����q��$3��;�
63v$Ƃ��u|���v"h���C��}�+e_KvTq�����HZ׽�|�����Sh�{LG������`��J&��q?Z��VC�0r�%4%�v���=k8t>Pd�O�8�Y�Vk���495 �V���a�jLm�^7��:�d�>'#ݭM�	-�P�P���	�ڏ��1녴�LC��i����@�/z���=��P��̖���x)qD�J:�o�m>$�@��6B-}a��R���,���U�7q�xQ��E����V�p�7W��j��C�ŏ��D�YwA�q62W9�Q�:宀�����܌Ey(�Y`AH(TB�v�9�%��Ak�b����yu�OG?�s�!�8懗	C�~my���5ű�@p{-]�����֨q��έ��B��,3��m��cg����T&^�� )o�tQ6�IޫIݛ�5Yim���LI#*�M�	`���d̐4D*�;l�4BD��`e�H$�I��R�3�1�ʸ�?=t�����}�H�o�%|�܇�]���ڄJ냧�z6%~��Ώ�"�I�n��'�۲!Ȱzzc�$�$�!yw���0���)���()��yh��q+��E��JcЎ�C���t�tM}�uc�;b^��&�i�ʩ�LZT�	�����@�JL-3Vz��Մ1�4���4J�N%�]���tw�#�i�
i�>�̈́�-���w�྄K�(�3�p9����O�6�������2�Q�/
&ӗ%\��^�p���m8���j/�R��܊��$,�����V[�0�}R�����k���N��k������x-_��o-ר���o�$Y�Ъ�Q8TȤDH�E�]7�G��G�45k7�P��s2!��0���S��o���ơ�2��1K� d�~��E,��O�B�J� 5�T�R�z�����FJ"�5����
�p��8*À&�,a�c%�vBNqkw����%��U�]�ׇ�pk1��F�F��ux8>~���
��/l׃�Ye�0�����B���#�n�^���z�M�6��>�d_�����ٙ;�삈1;���a�۪�D�\˙6s���K���	�a���4	U4�QA� Di0'M@[1�����B�ٹ���`�?�]%J"� ���.��T�a�#"�d����`����VB��v���]|�:�Cp�}��h�>f�U��_I���=��7�*l�!��V�;�e��[NT��Ot�?����ǔ�.{"���ΐ��:wo�VV�Db����?�Kg:8��1���?�~Hw	���x�R�ZA`��~�5P'yuW�����2�Ո�L-�~�X�) ����Č�x!m�y�!�������ձ��+��k���ER�L�S��{ko8=��s9hc��:M
���@y3:*�_�=��&	�Bg��4ˤh~��+X�"�8��
�!f����F��9/��� �k��@�5Y��( =���U�KgN���z2jkX����_�h�Tj%6uUrH�ƫfl�~��.�D��IG�Oƛ��_CI!?e���(��)�K�xd�c-����Q���O�ݳ� E*�#�+;���/2�y�o�@V���%�g���NCKM1�Xr���N/�O����M1��+	���z��#'��[R���+��#���Aޣ�cNȸ�\�+'�i
�H"��ǐ#{�Ip~&=�ij����I����f�JlmS�X�m�!�'���v=.>��Hjo��s	蔷B½b��uA�sc����D�:	X�4Y�p�Ѓ�CW�ھ푍Q,P=)����/�Wr@%�q���vt��;��h��X�?�';�&����^�k��XD4B�&+L�_�J:���̓9e�[Z��zc���D�C���{oIm.�7�8'��ĉv��3=� �K�x�7�#���������|�� �@�賛nDǨU�Hh�:�Z��q���xr�>�1�-���,o�4T���ε��Y�c�B�˅�v�U�j�fuu�'�䒿����o��|�jd}�FZ�[r`�"�~�N�}e:�콄ʭȐͯzY\��(L���r�$8�C0<E1���r��A�R�7TQY$��b� R_i	'��6 u���H\}[��>�	!@���
Mc�
�Z�4������]N�|����ME&�R�1��D�1����F�p��Y�s��#���|���1��\G��/�� "x=S�W��HSe���D�n��ZK���t��Q
��"C��G$"���H^�a)��Z�>(:��?����4� ���@����<ݜ�`�uwp���������X���[k���cf�
�R�ۘL�z����PI4'�2D����`���S'R	���#�\=��N�1��9��������؆�)��8�"�Fm�Th��V�wp_�_-�b���e_����+�^۟���B.�mZ|�*��Z%�6x4������Eld��iY
|��s2�g8�?8���1I��"\*��>�#:`Dcv%H���͵A9��)L%���|����H��1��ۋ?K�31��Y���z��)��x�e��V-,�����D1�D����&`t<<.gm�¹����H9׾=MRw*����i��Q5}���.�C��C���:�,\�L�
��Kp]����= =��|�p�=�Ґa�R*�l��l�ND��O���fr($i�$���Z8�R�l�3�{��4�>�����;�� �x�(<D�1~ӌ^\�w_��,`i�0���e���_���2*d��Þ~��!W`8]�EޅhF<�A�����5Ad�v ,����o��Sl��$R�׀_��5��$��M��`���3DE��b_m��Y��kH�a��*s�l�g�.��K?��ˁ��;׏�
d�k/V��P�ɡ�D�﯅^Qi�ߞO�r6%'×$�{<��+�}�R(���^��9	F(�A�e�^��m�6����;���:����.�������Hd-��=���ƓN����H0~��?�kx�/����aB�/+��}�ZB��̴R>��SkY#b�U�)�Xk`D�
�֎��ۜ /c�y�B�Z�?ND��E��%��q:�$f�(H�Ej	�چ9é��m�'��%0%;0�:0S��Z��캲�|�]'Hi���_M��۾sKEi�F��i�ͻN�Ga�����dq�o�V�E������y*��S�^9��#2��ￓ]������P�3�b��	#.��%�U�4I|��N�� 9i'Y��$�d���PV�8��馯�65��[���"����(Ϳn�RY-�r_���%n�������)#\D�K2q��
��G��%��e�3�LrCvM�a�$Ǚ>�YU�������J��lZ��q�ٖ�t�K�� �3y�Z����("���i�D=8�*K�� ���~.��n{!&����������R+	��5Y.ԢG ���xG�l�� 2h^�Y
f$�C��إ��3,�lg�m��x#�p��t�{�>Nė!E�,&�=������P�ep�".g?���+W��+E����Q��<nF�{O�3�~�Ψ(Y�\�����Mc���'�za(o��9H:�c�ˬ/D�H���9��8��_�K��]��c�.����B�ҹ������ӶP�Bs��&$CgdS�
���}H/0T�:��z�� aG��3�c|�a�&����G�T�si���5�s�e�xb�f��%�L�M���Hd7�I>�L���%�(�'QCa.� �]�^/G���6Ws�%�h�5E�GKMU�׿<��A�8V,�� �r���I�V�Ŵv[r���dM��mXd�'���"kȥ��Pz�;	���� 5�;!�̈́��6�*��B\����/2���3k�]0��[�s���p3j�5Ip����d��t���	��r
#�)YH��b}�NMS+g*��5G�Bֺ�����7]���2F�&FE�Zۿs�Wt�s��(������p�qBS��)�vX���{"����W�"����E�D��(�3��G����0_�1C��	=�8A�G'���z���)�]�lh�ffH����] �&2�"6��1VN�]�h�B�TI>��:�>��=��nӿgݾ��������C�%�ƌ��c��'T��*I�b�qVݯ�ׯe��K��'�]��2پ��?����8��<���S�4��zmhiG�7�=��<�n���`�X���邃Y�ҵ$C�Eh����y�;ZY?��R��+�0�v�\�λ��B�8	�
-�8��I=�lB[7;t� >��ul�4&I��zP�4��.���+[�L��������4��"R�H�D��5��tt}�]��4�#Òin6�C���{���e������Q玖��I?Gm<#Y�B�H����E�1��v�>������=(,]�D�l0��� �fQ���� dTqw �������4U1w�Ry�iQ��]�n�O�0zGZ�a#0��y�h-A�r��
����H�O�{������T��fC3�\�6K��x���͢s&t�d�m��C���	�F�E6y���z���"��7�����>���0
K+f�L�/``-�� ������z��Ng��}0�G�� �U�L��Z8�l���� ��\a���t�9�N� W�&L�i�=�P�U��?�)�ɦ��Lz_��v�ܙ�v}:�7s�\��[i�G@	�(p�+G���g�B+��m�e�����]^:�W;�%�a;�[Ho� V6��]��3���i�&�����]]ʲ���p�Z���p#%��`�Ģ�Ao�FFy�����\&�&h||�
�2'��b?�\�x$)?���a{U�<��Z��({���P��"���/��[����M2��jE�I�4�q��$����Rh���a�`���1+9o�O��~x�-����>9��F�O{�L�֚�_x*'�I��AKс���硌3M)�)��%��a������Kf"O�s�x�ܐpY��g�*5(�����o�kJ�^t0����9D�R4� �ߎ�;a�g�� ToX;�b��#��/������F�Y�G��}�V�B���d  jwe�}�s���R_�����$��Rf��[ap5߅D�65�{�\�Q����`�l4�΅��G5"����s/�Saĕ6g��5O�'��t}c���_���n� d��s���3�}�a�&0?�'���B�J9O$�P���w�1UXLL�����zthI7�K��v��Dd�X��Q|� ]"_��3�2q�=U��%L©��.N�N0-B�Ą�K�R�2೻�i=*�z@�a�X�u#��X�fߩ*���=��n D�˪|h�7N���/�|!���8Iy�����7�k��7c��ԃ�oJ���3h����+�}�E.[��Wف�N>������)�w��`q��K%���i���r���7� c!�d��Ƚ�l��)��U���5`�z��O-�).�x�x[��yh9x�G5�s���J���~sW��pZR�ЍxN}џ�cyv��%
�y�Z�4�j��T�$����v��$]L(�s�Lt�ь�7.�Ԏ��������)4��W�e�rS���6�<�ϳ��r���6���@����J�Tt�0����i�rj����ݍ��E�F�)�BꟌ�.��R�ՎN��R�� ĵ���L���6��~Ҧ �՘U�tm���+t\��j��R`bR3��Bq����s"�\�7���e�x��y>,a̴1�ҲD>�C�y4J���W׿8ni'��A��Ck�x�3�w�kǻLѹ~�9����+Q&���ڵ�2M�@�ہ�
�r;���������ۿ��#:�p�I�ݴ�̳Ϛ/4�w�E����6-��+>5o5_�Ml�զ��3��^7X�3����(�b�6�a���zP�������s���%`�nm�&�P�,�Ш�����0�.:bQ�QV������w��+�����י�iKӮyHl�`mX��ʻkB���V�|��*�����m���St8���w���!�-�Eh`�p�M�ҙ,��){�+�}�B.J��z��f/����{��J!�7D	"��YJOc���s�[b~��Po=�dr�ʰi��_��	�� Ȋ!bƗ�U��Mym	L��7����ĚꮄȦn�꼼�,;������p�W2�Q��0|3ܔ�Lv,z�C��m����f������cC�⸌P�ވ3
�݂m�}$�Ħ*?v��og�žg'$�S�vU�R��Ǿ�YF5��6�m2Jª�*ͽ�  �2����z���pK�a�����b��:��	C��Y�+��.+IE���X��H�Du��"`%��C���sX���U��� �:��?j�yP��	0��I��6����fQ`�v L��^{��LX�I �K~ݠ�[$��R4ʐ�'t�Ƒ���o��}�\�`pX8�nLL��-���i�W�#�oߒ�̵͗b��c4gݕR[V&���� �a��+S�ѽ��&�eb�*��X���@t��H���C�Ŏ�6��d�b�4#�jD��tX9J,1�]�_4@�/���G��l�������7�3%��.�A����Aw*}�~���wS 4hh�}J|v��#hs4��Z`t��y�Mϋ���z(A�x:�uTm���m��4>M�V=�ku$G���6{l����=7�3����S��	U
���������@a�ы�£�����#*��#?��=�Z��m��sٱشA�8e0g�⏵'_w�#(l��#~�_Q^��u�
 ֦~h�H@�b㝄�� ��g� �xj�{ى(�?�)���)l��X5�v!��5�.Cݢ�X��C�"��e��X�H��TZ��8h���N�I��,��+pv�=�c3kxƘ�KY*�?ŦgUް�ϥ�a��u��
L�y+�$ޯ� �r!������Rv���
�׽m�����H�8.4ؠ/y�����<�Z�O��?b�Tǝ��"tW��*���\�2T>�[�5�(��"9� i,-j���քw�p	�O�+GSv&���͇f�!��7���<)���a�ҳu^���	��ǰ��$� PضH�mmܫ�`U�jK�|!E�MU@�a�:RyTx&���svf��mh�(C���J'�l/�:�Խ���!�	�o,���)�{-{#����F��'��Bf�(bQ{4�s���s��u���E�N;˗fzgz�~G	��]7c�u�t1":?*�͝K���I�ǟqZV����F�6_�P�<H�&��B�N�������E��!��);7�T�"��({p��о��s[�3�l�T�Y�X�t�6A�I�����U���}Q��R�\�姏���88k�5��k_Xw<�kxDC�U���]�s#���l�FXxB�Uh�*1�u"*��K�`���40>��́G�[�B����&�2��z��ێyw<�����vs�=��$��wF���Ս�d��<]�ܔ������������`�-w�jg.�?P��:�1��"._�	��Uĕ�I��;M,S
3�k�\���}�k>����v�#�ڥwf�������o�\|siJ��p��5���-�]�W }@k���<�Gsg=�8�DJ��C�h��b������Ű��H��J����4¢#��#�#0��)>����a+����d�/dEx���TJaJS5V�e	�3w �J-Z3�-�E2s�.ru����i�Oزt����>��D��5����i��Lv�z�{h0��QQ�J����%�؝��Dk��W;{L�9�l�-OBe]X��c��5 dw&JټM��|�{'�ҟ���d��I���ֳ��d؊L�6��|��X�����mg�og\Iw�������t�_����0ގGa���yR_5Ѽ���.��B̗�$�R����D<��~����4\%��Ye�4ˊ��˽�������n��p9Y���cz�#P��-���{p���C�k����3� `_ �QN%n�f����J�'��%FZ���<��|f:�E�*�l1"d�|e�P�);竏���^o@��|��!��>�{����CY�>DP� 5;/L��!N4�T/~��g�6*�_�	h���0yЊK�b�]�M0w�p���0�n'@��j�b��'S��\o���)P�qYf����"NXZ�Mi9٤n�9�����X ��(���RG�j��'^DZ���{!���+kR�h�HT��f���qg8�w^����J�� �75�欁�<,J������wb	�p�r^�������i2.����=usJ�,��ϯJ�2d��Ol�Ie�图}?����� �0��\h������0Y`��- ��
��+�&{��XVP��4h~��Uw��ڥ�IL�b]�nʐ����~��钣��z-dDc.mX�mU�+�G9�*r��/o��ډ�:̧u��o�����27	��K��Wdݵq+��KT;[���
�l���n��|��J6�[����vj>rKD��T�r�@�g���;�e k�9z{j̝,�4�Q�����ar��vm��"�͡��:t�< Tb߷TQ~�J�u�N��}r���jM'���������ޟh�F���*��r;��v$�˸zl͟�l��'�����5;��yJ0��7��1��I�0�ĝʛvo>8�_օ�&i�C�n�B�X�ߍo��a\�?�^������8�h�/ȷ�O�]����f3ėV0B�7}�"wP��<���[v�]���QL2�(mf\i޼
h���oG��ʟt�֮q�D2�M��~��p�=��B�>���E����U�Q��?�+�
:�`�K>D�
-���!V�ǞS���.�rC��FL�x	�`YWۛ׏�t񷕏��Y
l2�����l�ݒԮ�@3H Z�z �O�eK�d˜}��-tIK��^�-�a6GQr/D�A���Mh�!n�<�Z�h|����՚0�~(my�����`靗����v�5��}�<6�F�H`�f�oY�k��(���5E\IA�A��TB^b�O����+h'���X/"��=������B�b�~h���ϼ�[�| c�|69��R<@G�u�;�s�>oee�K�������&mr�?��h�C�M�����X3�o�1�*U���/
䐈&���d������N�Fe�o+�E�/ND��z�Ak�e��lcP(×j�T��Jܒ���fkNB~���R���󊡕��?N�6��������:�|���D�,�B�ՑY_�����+�!� x�O���z�������VPY'��Z�����8p]1sm�bO��lax���K��I���[�
}�ZMχ^�)A{�wg<����Ȁ�̴o��ѝ�/0ǻ�� ˞)�'usU�2.�J�y;3� �n\��t��m��Yk�X��}X'51ˋ�"�x1�-������f��V�v�gw`��e�����V�9��.
ȤCX��>�/�L)���6��T҄�n����1��H�]{�mĦ3HAU��l���Я�g44��|�$@w�T9Ƞ�AA,�S4�o��hp#��C'�E��Zt|�pȞ�3������
��AE�hʮ�i�q�H23�Y����3O�܋��0�DŒI�_^c(4�f��й����yk�9;A6�M���H ��`M+�}��l��7-XC��$�%o�*�sCQT�eY 0��}=Mi�yđ	z�����m@��8��+�p �G�\D�4�5/e���Pv�� 	����5Ux&��}�%���Nv}��NQ�q~s#��o��Ĵ�7`C��k�T�f�x$�r"���vvw�\�l��^?���/�9�Ga�J�îGt�m�zw�=Ӻ)
��)p��؅uH���}�3� -0`#3��]Xi�y����e�*��!~����*��Q���R��E�s�n��	u)�w�)~�b�/N&�u!l:(O�y��o7sJT	8H[���=j�VI�$+{ �Sw)�&�N��_���0�l�bICa&�|KQm��>pñ^���T������B�:�������z.�W�0��\U@< �iEv�j�ۆ_4i0��>��Z��@u�O�р��~��J�Q�WȮ5Tl$�0�Dq���c��kɻ�iݤ�W�K�ϥ�/�8J��:���w
6�N���b�}* ���l�$ǰ��W��0�Gl����O�39�dT����bﻈx�(���Z؅�Gļ�B]� Ԡ$�&?a!e�rŃ�q�liQ:�d���蚍����`,��՘x4zTnHps�d��_e�p��d驨&�(�w��Ԏ���-��q�
����Fy���!��
���'��t�h�a}�٘��j�@��nȡ#�C����a�r.�[�����c�#j���4���/AyJW~�,�uf�6"�������-N)������܎�VݣЦ<�m%�X(e���u0M��� Dfˑ3Sb�I
����6!�m�A�0z �#N�� ��o
��
����(P<?���=R�j��������B��]���gc��mX���ߓ@�ٖ{퉖J3.fX�!�p�;8u���Ɵ���Y}�$|dtn�!�Ȧ�mF<-*k�����Kꓞ����/���-ɂ�������OM{N��Ǣ��kX��VI�L��Ndh�H�E������������Z1
I$��Qm�x���c-�q\ͮ_m�]0qu�y˪��W�\�Y��P�A��t��3@��ULp�� �{�d�$�(!�I���,�ݖI7l;��mn�i�""]���m�%R1',�htV�ɨ�.�Q�ޒy!/\<q�ͷ���?�[�N�d����=Gm��Q
||}x9V��TP�ﳈ;9�d!��@	$3��Qt�ކfw� c7�#�Zq�3���C���I�J�QFVuD����}�-��u���I.�k"���v#��)}ά�'�HVO�/4`a��$���r`�25x%����`����C�n��Yе�t/I��ɥ$�y0c�fr�Zj]KY4`<=�y�JpRXǏ���çD<'b%���U+��Z�׉�T���e� ��~>snn��0Ll�*��O�^��s劜6�^o�h�ǑP��yM�p"��O�
	��u��RKT�JY[V�Ok��LC�3P����K����`ͬWeh��:l]��Gr�`M��ܫ�]�Q�5E�)&|���͹��{�jp��A���; ��;_�4�qy��k#X��]G�k< \����LY�!�����~
X�H{��=l�맊<DV���1�����WG��S�?�\��#[���^N��8�"Hc���9�E��t��Y	X���8Z�:4	���C���w�Nb>c�Il��+��^�h�ǓZ�tG&������O��$~.�#C����o�6.���+,&�mLQ3��RQ_�R�R����z��O�<j�*�w����``��
V¨�|���c�Y��1Qd��d~7A�RiiXW�E�I��%���F����COE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/NET_L3_MASTER_DEV) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/DECNET) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/MCTP) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/net_namespace.h \
    $(wildcard include/config/IEEE802154_6LOWPAN) \
    $(wildcard include/config/IP_SCTP) \
    $(wildcard include/config/NETFILTER) \
    $(wildcard include/config/WEXT_CORE) \
    $(wildcard include/config/MPLS) \
    $(wildcard include/config/CAN) \
    $(wildcard include/config/CRYPTO_USER) \
    $(wildcard include/config/SMC) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/NET_NS_REFCNT_TRACKER) \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/XFRM_STATISTICS) \
    $(wildcard include/config/TLS) \
  include/net/snmp.h \
  include/uapi/linux/snmp.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/IP_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_MROUTE) \
    $(wildcard include/config/IP_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_MULTIPATH) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/IPV6_MULTIPLE_TABLES) \
    $(wildcard include/config/IPV6_SUBTREES) \
    $(wildcard include/config/IPV6_MROUTE) \
    $(wildcard include/config/IPV6_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/NF_DEFRAG_IPV6) \
  include/net/dst_ops.h \
  include/uapi/linux/icmpv6.h \
  include/net/netns/nexthop.h \
  include/net/netns/ieee802154_6lowpan.h \
  include/net/netns/sctp.h \
  include/net/netns/netfilter.h \
    $(wildcard include/config/NETFILTER_FAMILY_ARP) \
    $(wildcard include/config/NETFILTER_FAMILY_BRIDGE) \
    $(wildcard include/config/NF_DEFRAG_IPV4) \
  include/linux/netfilter_defs.h \
  include/uapi/linux/netfilter.h \
  include/linux/in.h \
  include/uapi/linux/in.h \
  include/net/netns/conntrack.h \
    $(wildcard include/config/NF_CT_PROTO_DCCP) \
    $(wildcard include/config/NF_CT_PROTO_SCTP) \
    $(wildcard include/config/NF_FLOW_TABLE) \
    $(wildcard include/config/NF_CT_PROTO_GRE) \
    $(wildcard include/config/NF_CONNTRACK_E