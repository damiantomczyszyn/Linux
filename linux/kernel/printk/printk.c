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

static int __init lock_torture_in��Xrd��P�C�����%t�QƫΐS6�0%>k������v����
e�R���AQ�����>���g�vg�%	6\
\+Uv�1l�R�Q��@�)�.��������5P�jT��UN[�{Uv�	)ُbv��q;��� �'��KT�]^�d{1;3 �H��!A��O������W���"?��'
�!�
ʐ�C}3|�# �-����i���g1����>��z�1�s\�1��Rc�Dg��;��U0�y���-��UFo�6B��%�)�(�"��(g���)��ǋ�q��w�����Xn?�-���징k��Tl�j����� ���l=�����	҆����pг�Y��z�(�S��[�`V��d��w��n~&�~\!�"�0\����>�O������Ys8�iE��Hd	
��8r(�cN�,sx����B����>�qW�L&�V˚�g4"�l;�o�6��]�Yg��`6
���2b��GrGMpu1����~�W�ؑņC��I��%2��(T���$u�����/��(u_`����E/���&�v^v��k�	&X�x�; ��-<l�^�2��`��p��_��������W��Q=Az�C JBBT>��9�Z��/v�ql	YN���E��i�6��}	S1mw�(�A��S��~W�ZΩ��G�Y�vIw�q�,��VB�;�IPȵ����� %�0{�	.��{1;��Ә��5��*K?A�s
.yF���`Y�݆�Qs�ڭ;ݥ��LW�*d���K��j
)�)n\E_;E넒	$U��v>R�Y���JB����$5�z��S$Dj3��E�1�@��M��?�2�}�����̃�E������6'Y�:盉dѕNV/��آg�s��d"V�k�yK�V Vvŷfcj����R�����.:O)����*gp��J�=�]3C�t���>�.����"�z���{Z���}��͡�����Cp����F!�Sj�_ܔ�^d8u��hʏ-ԝNYI]�Gl��k[w����p�wb� i�}Z?�r_�&��)����줙�j'g6l����U��?2���.����H�_K�`���/��G����nt�Da�>��X�kHC�;3�q��R���Ȥ�Fcp���}*V��m��Q��[B�8��s�E ����y�G�rS�@��%���
7��X5�.���-b�To�/� N+�����Z����E�H9���uȹa�
50������~��}��7�+�K��
��O'r\F?S+WS�Wj��4̐t�%��LM�踣`ovۏ;�ab�;^�a{Zzi~Hں	x��y�{0���������Zla��( ��y��il��|��M��Pr��i����U�����]{nx���̩s������2��8&�����m�^�`ͩ:ß+,��¸/�&���A(����<�B�m)����7�tjŰ��P?{T;Ů�3|f�E����S��7�D�Շ_.��;}*�5>|n�8�ε�����h���E<K�+�T�#�4q�A�[̘�HK�{��&�{ڟ�+�O�ejq�&Jis/���a$�w��+a9�W�����p.��B� h�O�f�*���IB�[�o#�Y����~�~/�U��-�4����Q�T:�rY�����%���@�e�%��e�����YУ���]��٣7�;��K8����6KmE��#}Z����tȀ��7��:�e(�^a;�kz��g�؇�/9/�v�F2%C[d�w+��3�A�?�U_ݖ���s��T�a9c���:|d�)S�)O3��M!�F0GA��Y$ؓ9n����~\�Lf-��ժ��ۭ�S�&���+���V\e@��{"`��̚kã7;ƌc��,�`�����W�j�^
����������4-U��FdR"K/��D;y��>�
9=��/��!�}5�4o���>�s08L�ٵۃQ�G��t�(�u�#�A��qW�d4D�����GK�br: ���Y�<ܪ�=.b���1׫`w��'�$l1O��&z]Y�$M��UU^�ϋ�?��2�UF2��t�SVe���ӹ��+ �~�rU�h�!��ܨ
�Іc�'�F"zǀ� �7���D�W���Z��6?J��=E���mS�s��q?tM�pS?��}%�)��ó���K�,g��ƞJk=3i;f�E��7�q���s�[^'��8sM�V�8�l��a�G�7��w��hQ
S�Z�+��HL�\��w]'���R���t+b����C�|O�ƿg�ȈR�F��bRI����׉�~��h�vѿule����|�`X���HuN��zm������`@Q�r����]�dC���P�R�|�U��U����;IQ�'�Қ��_~j���y��t{��lLH�#��-�&�H8�٥.��_ED��㎴����0�nd�{���|��K-�Y�>!�CCA%p�m9����?��H;��֏����v�=�q�aN=�P0��|r�� '�cȷiGO@��z��Ӓ�b�3�
��OD�@g8B�R��Z�dk��a�c�,;+ �z'�^&K:��<���F0Z��
 s#l��h��J�m༛Ws��+d��d��$Zn]7��7	���|���5BsKN�zn�;�y����	���*��I�-w2���#�g��t�k|�F���h��<����yέW4��0Ԫ"s>�F��h��Z���U�sψ!g,��&��p�?X3`C�������{W�j�t����h�|c9P�Ǉ%���Yq���W�8�?��˓Jb@
63v$Ƃ��u|��
i�>�̈́�-���w�྄K�(�3�p9����O�6�������2�Q�/
&ӗ%\��^�p���m8���j/�R��܊��$,�����V[�0�}R�����k���N��k������x-_��o-ר���o�$Y�Ъ�Q8TȤDH�E�]7�G��G�45k7�P��s2!��0���S��o���ơ�2��1K� d�~��E,��O�B�J� 5�T�R�z�����FJ"�5����
�p��8*À&�,a�c%�vBNqkw����%��U�]�ׇ�pk1��F�F��ux8>~���
��/l׃�Ye�0�����B���#�n�^���z�M�6��>�d_�����ٙ;�삈1;���a�۪�D�\˙6s���K���	�a
���@y3:*�
�!f����F��9/��� �k��@�5Y��( =���U�KgN���z2jkX����_�h�Tj%6uUrH�ƫfl�~��.�D��IG�Oƛ��_CI!?e���(��)�K�xd�c-����Q���O�ݳ� E*�#�+;���/2�y�o�@V���%�g���NCKM1�Xr���N/�O����M1��+	���z��#'��[R���+��#���Aޣ�cNȸ�\�+'�i
�H"��ǐ#{�Ip~&=�ij����I����f�JlmS�X�m�!�'���v=.>��Hjo��s	蔷B½b��uA�sc����D�:	X�4Y�p�Ѓ�CW�ھ푍Q,P=)����/�Wr@%�q���vt��;��h��X�?�';�&����^�k��XD4B�&+L�_�J:���̓9e�[Z��zc���D�C���{oIm.�7�8'��ĉv��3=� �K�x�7�#���������|�� �@�賛nDǨU�Hh�:�Z��q���xr�>�1�-���,o�4T���ε��Y�c�B�˅�v�U�j�fuu�'�䒿����o��|�jd}�FZ�[r`�"�~�N�}e:�콄ʭȐͯzY\��(L���r�$8�C0<E1���r��A�R�7TQY$��b� R_i	'��6 u���H\}[��>�	!@���
Mc�
�Z�4������]N�|����ME&�R�1��D�1����F�p��Y�s��#���|���1��\G��/�� "x=S�W��HSe���D�n��ZK���t��Q
��"C��G$"���H^�a)��Z�>(:��?����4� ���@����<ݜ�`�uwp���������X���[k���cf�
�R�ۘL�z����PI4'�2D����`���S'R	���#�\=��N�1��9��������؆�)��8�"�Fm�Th��V�wp_�_-�b���
|��s2�g8�?8���1I��"\*��>�#:`Dcv%H���͵A9��)L%���|����H��1��ۋ?K�31��Y���z��
��Kp]����= =��|�p�=�Ґa�R*�l��l�ND��O���fr($i�$���Z8�R�l�3�{��4�>�����;�� �x�(<D�1~ӌ^\�w_��,`i�0���e���_���2*d��Þ~��!W`8]�EޅhF
d�k/V��P�ɡ�D�﯅^Qi�ߞO�r6%'×$�{<��+�}�R(���^��9	F(�A�e�^��m�6����;���:����.�������Hd-��=���ƓN����H0~��?�kx�/����aB�/+��}�ZB��̴R>��SkY#b�U�)�Xk`D�
�֎��ۜ /c�y�B�Z�?ND��E��%��q:�$f�(H�Ej	�چ9é��m�'��%0%;0�:0S
��G��%��e�3�LrCvM�a�$Ǚ>�YU���
f$�C��إ��3,�lg�m��x#�p��t�{�>Nė!E�,&�=������P�ep�".g?���+W��+E����Q��<nF�{O�3�~�Ψ(Y�\�����Mc���'�za(o��9H:�c�ˬ/D�H���9��8��_�K��]��c�.����B�ҹ������ӶP�Bs��&$CgdS�
���}H/0T�:��z�� aG��3�c|�a�&����G�T�si���5�s�e�xb�f��%�L�M���Hd7�I>�L���%�(�'QCa.� �]�^/G���6Ws�%�h�5E�GKMU�׿<��A�8V,�� �r���I�V�Ŵv[r���dM��mXd�'���"kȥ��Pz�;	���� 5�;!�̈́��6�*��B\����/2���3k�]0��[�s���p3j�5Ip����d��t���	��r
#�)YH��b}�NMS+g*��5G�Bֺ�����7]���2F�&FE�Zۿs�Wt�s��(������p�qBS��)�vX���{"����W�"����E�D��(�3��G����0_�1C��	=�8A�G'���z���)�]�lh�ffH����] �&2�"6��1VN�]�h�B�TI>��:�>��=��nӿgݾ��������C�%�ƌ��c��'T��*I�b�qVݯ�ׯe��K��'�]��2پ��?����8��<���S�4��zmhiG�7�=��<�n���`�X���邃Y�ҵ$C�Eh����y�;ZY?��R��+�0�v�\�λ��B�8	�
-�8��I=�lB[7;t� >��ul�4&I��zP�4��.���+[�L��������4��"R�H�D��5��tt}�]��4�#Òin6�C���{���e������Q玖��I?Gm<#Y�B�H����E�1��v�>������=(,]�D�l0��� �fQ���� dTqw �������4U1w�Ry�iQ��]�n�O�0zGZ�a#0��y�h-A�r��
����H�O�{������T��fC3�\�6K��x���͢s&t�d�m��C���	�F�E6y���z���"��7�����>���0
K+f�L�/``-�� ������z��Ng��}0�G�� �U�L��Z8�l���� 
�2'��b?�\�x$)?���a{U�<��Z��({���P��"���/��[����M2��jE�I�4�q��$����Rh���a�`���1+9o�O��~x�-����>9��F�O{�L�֚�_x*'�I��AKс���硌3M)�)��%��a������Kf"O�s�x�ܐpY��g�*5(�����o�kJ�^t0�
�y�Z�4�j��T�$����v��$]L(�s�Lt�ь�7.�Ԏ��������)4��W�e�rS���6�<�ϳ��r���6���@����J�Tt�0����i�rj����ݍ��E�F�)�BꟌ�.��R�ՎN��R�� ĵ���L���6��~Ҧ �՘U�tm����+t\��j��R`bR3��Bq����s"�\�7���e�x��y>,a̴1�ҲD>�C�y4J���W׿8ni'��A��Ck�x�3�w�kǻLѹ~�9����+Q&���ڵ�2M�@�ہ�
�r;���������ۿ��#:�p�I�ݴ�̳Ϛ/4�w�E����
�݂m�}$�Ħ*?v��og�žg'$�S�vU�R��Ǿ�YF5��6�m2Jª�*ͽ�  �2����z���pK�a�����b��:��	C��Y�+��.+IE���X��H�Du��"`%��C���sX���U��� �:��?j�yP��	0��I��6����fQ`�v L��^{��LX�I �K~ݠ�[$��R4ʐ�'t�Ƒ���o��}�\�`pX8�nLL��-���i�W�#�oߒ�̵͗b��c4gݕR[V&���� �a��+S�ѽ��&�eb�*��X���@t��H���C�Ŏ�6��d�b�4#�jD��tX9J,1�]�_4@�/���G��l�������7�3%��.�A����Aw*}�~���wS 4hh�}J|v��#hs4��Z`t��y�Mϋ���z(A�x:�uTm���m��4>M�V=�ku$G���6{l����=7�3����S��	U
���������@a�ы�£�����#*��#?��=
 ֦~h�H@�b㝄�� ��g� �xj�{ى(�?�)���)l��X5�v!��5�.Cݢ�X��C�"��e��X�H��TZ��8h���N�I��,��+p
L�y+�$ޯ� �r!������Rv���
�׽m�����H�8.4ؠ/y�����<�Z�O��?b�Tǝ��"tW��*���\�2T>�[�5�(��"9� i,-j���քw�p	�O�+GSv&���͇f�!��7���<)���a�ҳu^���	��ǰ��$� PضH�mmܫ�`U�jK�|!E�MU@�a�:RyTx&���svf��mh�(C���J'�l/�:�Խ���!�	�o,���)�{-{#����F��'��Bf�(bQ{4�s���s��u���E�N;˗fzgz�~G	��]7c�
3�k�\���}�k>����v�#�ڥwf�������o�\|siJ��p��5���-�]�W }@k���<�Gsg=�8�DJ��C�h��b������Ű��H��J����4¢#��#�#0��)>����a+����d�/dEx���TJaJS5V�e	�3w �J-Z3�-�E2s�.ru����i�Oزt����>��D��5����i��Lv�z�{h0��QQ�J����%�؝��Dk��W;{L�9�l�-OBe]X��c��5 dw&JټM��|�{'�ҟ���d��I���ֳ��d؊L�6��|��X�����mg�og\Iw�������t�_����0ގGa���yR_5Ѽ���.��B̗�$�R����D<��~����4\%��Ye�4ˊ��˽�������n��p9Y���cz�#P��-���{p���C�k����3� `_ �QN%n�f����J�'��%FZ���<��|f:�E�*�l1"d�|e�P�);竏���^o@��|��!��>
��+�&{��XVP��4h~��Uw��ڥ�IL�b]�nʐ����~��钣��z-dDc.mX�mU�+�G9�*r��/o��ډ�:̧u��o�����27	��K��Wdݵq+��KT;[���
�l���n��|��J6�[����vj>rKD��T�r�@�g���;�e k�9z{j̝,�4�Q�����ar��vm��"�͡��:t�< Tb߷TQ~�J�u�N��}r���jM'���������ޟh�F���*��r;��v$�˸zl͟�l��'�����5;��yJ0��7��1��I�0�ĝʛvo>8�_օ�&i�C�n�B�X�ߍo��a\�?�^������8�h�/ȷ�O�]����f3ėV0B�7}�"wP��<���[v�]���QL2�(mf\i޼
h���oG��ʟt�֮q�D2�M��~��p�=��B�>���E����U�Q��?�+�
:�`�K>D�
-���!V�ǞS���.�rC��FL�x	�`YWۛ׏�t񷕏��Y
l2�����l�ݒԮ�@3H Z�z �O�eK�d˜}��-tIK��^�-�a6GQr/D�A���Mh�!n�<�Z�h|����՚0�~(my�����`靗����v�5��}�<6�F�H`�f�oY�k��(���5E\IA�A��TB^b�O����+h'���X/"��=������B�b�~h���ϼ�[�| c�|69��R<@G�u�;�s�>oee�K�������&mr�?��h�C�M�����X3�o�1�*U���/

}�ZMχ^�)A{�wg<����Ȁ�̴o��ѝ�/0ǻ�� ˞)�'usU�2.�J�y;3� �n\��t���m��Yk�X��}X'51ˋ�"�x1�-������f��V�v�gw`��e�����V�9��.
ȤCX��>�/�L)���6��T҄�n����1��H�]{�mĦ3HAU��l���Я�g44��|�$@w�T9Ƞ�AA,�S4�o��hp#��C'�E��Zt|�pȞ�3������
��AE�hʮ�i�q�H23�Y����3O�܋��0�DŒI�_^c(4�f��й����yk�9;A6�M���H ��`M+�}��l��7-XC��$�%o�*�sCQT�eY 0��}=Mi�yđ	z�����m@��8��+�p �G�\D�4�5/e���Pv�� 	����5Ux&��}�%���Nv}��NQ�q~s#��o��Ĵ�7`C��k�T�f�x$�r"���vvw�\�l��^?���/�9�Ga�J�îGt�m�zw�=Ӻ)
��)p��؅uH���}�3� -0`#3��]Xi�y����e�*��!~����*��Q���R��E�s�n��	u)�w�)~�b�/N&�u!l:(O�y��o7sJT	8H[���=j�VI�$+{ �Sw)�&�N��_���0�l�bICa&�|KQm��>pñ^���T������B�:�������z.�W�0��\U@< �iEv�j�ۆ_4i0��>��Z��@u�O�р��~��J�Q�WȮ5Tl$�0�Dq���c��kɻ�iݤ�W�K�ϥ�/�8J��:���w
6�N���b�}* ���l�$ǰ��W��0�Gl����O�39�dT����bﻈx�(���Z؅�Gļ�B]� Ԡ$�&?a!e�rŃ�q�liQ:�d���蚍����`,��՘x4zTnHps�d��_e�p��d驨&�(�w��Ԏ���-��q�
����Fy���!��
���'��t�h�a}�٘��j�@��nȡ#�C����a�r.�[����
����6!�m�A�0z �#N�� ��o
��
����(P<?���=R�j��������B��]���gc��mX���ߓ@�ٖ{퉖J3.fX�!�p�;8u���Ɵ���Y}�$|dtn�!�Ȧ�mF<-*k����
I$��Qm�x���c-�q\ͮ_m�]0qu�y˪��W�\�Y��P�A��t��3@��ULp�� �{�d�$�(!���I���,�ݖI7l;��mn�i�""]���m�%R1',�htV�ɨ�.�Q�ޒy!/\<q�ͷ���?�[�N�d����=Gm��Q
||}x9V��TP�ﳈ;9�d!��@	$3��Qt�ކfw� c7�#�Zq�3���C���I�J�QFVuD����}�-
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
    $(wildcard include/config/NF_CONNTRACK_EVENTS) \
    $(wildcard include/config/NF_CONNTRACK_LABELS) \
  include/linux/list_nulls.h \
  include/linux/netfilter/nf_conntrack_tcp.h \
  include/uapi/linux/netfilter/nf_conntrack_tcp.h \
  include/linux/netfilter/nf_conntrack_dccp.h \
  include/uapi/linux/netfilter/nf_conntrack_tuple_common.h \
  include/linux/netfilter/nf_conntrack_sctp.h \
  include/uapi/linux/netfilter/nf_conntrack_sctp.h \
  include/net/netns/nftables.h \
  include/net/netns/xfrm.h \
  include/uapi/linux/xfrm.h \
  include/net/netns/mpls.h \
  include/net/netns/can.h \
  include/net/netns/xdp.h \
  include/net/netns/smc.h \
  include/net/netns/bpf.h \
  include/net/netns/mctp.h \
  include/net/net_trackers.h \
  include/linux/ref_tracker.h \
    $(wildcard include/config/REF_TRACKER) \
  include/linux/seq_file_net.h \
  include/net/netprio_cgroup.h \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/SECURITY_NELF                       �      4     (            GNU  �       �          ��������
��  �C1�$j �   �؍T$�����Y���R  1��T$d+    u��[^_]��������&    ������UWVS�p�X|�@(�~�=	� �P  vN=	� ��  =
	� �  �۸   �   �   E؉�S�����~~ Xuu�t& �1�[^_]Í�&    f�=	� ti=	� ��  �ۍ��   �   �   H؉����À����S������~~ ]t�S�0   �   ������Y뙍�&    f�S�3   ����&    ����   ���   �-    �@|��	��  �ٺ� ���)ى���
����)������2  S1ɺ   ���<���X�݃�U�   �   ���$������    X��   �~~ �����S�   �I�����[�� �����  0 ������
���   ��)˹   ��S������~~ X�����S�1   ������t& ��[�� �����  0 ������
���   ��)˹   ��S�{����~~ X�`���S�2   �����t& ���>S�   1ɉ��   �H���Z����f�U�@   �   ���.���X��������������  �  �t& ������    S�X���]  �������1�[Í�&    ��&    �����V��S�X\����������  ��t���  ���������������   �����1�[^������U��WVS�� ��u�@�   �+  ����������������t  �E��
���D$
������=  �����D$�t$��>  	�	ȃ<$C��A  ��@  ��!΃|$�   ��  ��B  1Ƀ<$C��C  N��T$�L$
��D  ����F  !у<$G��G  ������E  ��!�����H  ��!��|$��I  !Ѓ|$��!Ј�J  ��   ��v �<$F~ǃP     ��t& �1��<$C����P  j ���   1ɺ   ���������G   ^��   j �	� �0  ��h �  j h�  j h��  j j ������	� �0  �� ���  ��j h �  j h�  j h��  j j ������		� �0  ���� j j j jj jj j ��������   �{l���  �� ����   ����������� [^_]Í�&    f������   1��n�����&    j �	� �0  ��h �  j h�  j h��  j j ������	� �0  ���� j h �  j h�  j h��  j j ������
	� �0  ���� j j j jj jj j ������� �����t& �ƃB  ��C  �������    �    ���������  ������������  �   ��������������=   �u  �W  �}  ��&    ��&    ������Ѻ   ������&    ��&    �������Ѻ   ������&    ��&    ������Q�Ѻ   �N���ZÍ�&    �t& ������Q�Ѻ   �.���ZÍ�&    �t& ������WVS�ÉЉʋs\��t  ��w��v?�`  �=    ��p  ���*  Q�   �   ���������E   Zu:[^_Í�    �ɍ|�<�`  ���t��`  ��#�p  	�렍�&    �v ��x  �ع@   �   �y���X[^_Ít& �����UW��V��S�Éȃ����   �<$�׃��|$���������|$�L$9��  �  ���  �   1����  �   �����Dǃ���u���	   ǃ�     ET$1ɉ�X  ��������T$�   ��������T$�   ����������������   �   ��P  �0   E؉���������$��%��  ����   	�R�   �n����F\Z���  ��t'���  �   �  ������P�   j �����X��1�[^_]Í�&    9��  ������፴&    �v �����UWV��S�Í��  d�-    ��d�    �D$1�����$    �D$    �D$    �l$�D$    ������������u��xN��������������������������    d�    ��u4���  ���T$d+    u1��[^_]Ít& ��   �E�����믐���������t�1��������������&    �����S�Ë��  �@�@��������  ���   �P|���  ���   �P|���  �~������  [�@�@�������&    �t& ������VS��L  ���   ��ug�
   ���������   ���������   ���������C   u$[^_É�   �������	   �������뭍t& ��غA   [^_�������&    ��&    ������WVS�Éȃ��{\���  ��w;�4�4  ��   ��u7���� ��5    �҅��o  ����[^_������v �    �   ��tɃ�
~��I   t�p��	��f�����	�뮍�&    �����U�   WVS�h\�Ë��  ���������H������   ���:�����	   �������&�����
   ������������C   u���غ   [��^_��]��������A   ������������э�&    �v �����S��  ��1ɸ�  ���&    �v �B��t�
��9�u�[Í�    ��  [Ð�����W��V�Ή���S���  �ú�   ���������   ���������   �؁��  ���������   ���������1�[�V   ^_�������&    f������U��  WV��S���x\���  ��w��H  ��P  �
��[^_]�f����ع Z  �   [^_]�������&    ������S�P\�Ë�P  ��~��u��T   t,��[������t& ��y[Í�&    �v ��[������&    ����  t���  �����t�    �    �������몍�    �!   �    �������둍�&    ��    �����UWV��S���x\��P  ����   ��u�   ������ŉ���1���   ��@��X  	tK�   ��D؋��  ���  ��t��T   ��  Ǉ�     �   �=     �L  9�������[^_]Ít& ��y��1�[^_]�f���l  ��X  �˃���   �P�1���wV�#   ��������=    �#  �Ë��  �   ��uT�t& 9���   �
�   E��x�����&    ��   �f�����    �   �������=�  ��  ��O=    ��  ���  1�=    �= ����   �   M������v �����  ����f���l  1������x  ��&    ��&    �����U��WVS���X\�    ���.	  ǃd  ��� ǃh      ������E�$�v �=    �	  �������������    ����	  ���  ���������E  ��X  	�(  ǃ�     �ؾ@   �������L  ��u��\  % �  ������� ���  ���   ��ǃl      ������ǡ    ����  �=    ��  ���   1�1��"��&    �v ���@�լ  ��t�ՠ  9�u�    ����  �@��T  �=     �Š  ǃ�     �P�@��|  ���  �]  ��L  ���O  ��\    � �?  ��	�6  �
����둸�  ��t& ���;1tF�P����u��	  �v �D$��  ��  1����    �J����  ���9�u�D$��  �	  ��&    ��&    �����UWV��S���=    �X\����
  ǃd  ��� �~ǃh      ������    �<$�t& ����u
  �������������    ���U
  ���:
  ���  ���������  ��X  	�  �n\��ǅT     ���  ���������E   �r  �F\��L  ���!  ��\  ��`  �р� 	���  �р��	��  �с�  @ 	��{  �p  ����  ���0   ����   	����������������   �
   �t& �d   �����������  �~   �������=�  �5  �=    ��	  ��uË�T  ���%  �-    ����  1�1�����@�<լ  ����   �ՠ  9�u��@�Ŵ  �Ű  ��d  ��h  ��	�b  ���������p  �   ����������  ��T   ��   �    ����f��
  �t& �������������t& �    ���  �@  �����    ����
  �`  �|����    ����
  1���[^_]Ã������[^_]ø�  1���  ���&    �P���
  �8��9�u��
  ��&    �����UW��VS���p\�    ���0  ǆd  ��� ǆh      ������G�D$f��=    �  �������������    ����
   ���������uh�   �t$������������X���Z�  �L  WVU��h�   �t$������
   ���������u@h�   �t$������������X���Z�
  �C�t$$QRh�   Ph   ���������  ��  ���   �I|����   ���  �  �  D�Q�   RP�Gh�   Ph   �����1ɺ   ��S�  ���f  ���   �p|�Gh  h  h�   Ph   �����1ɺ   ��S�  ���a  �  �  �h  Ph   ��������  �+   ��<   �  D׋�P  �4��   WRP�E  P�D$(��P�D$$�ă�@P�t$�t$<�t$(h(  �t$D�������P  ��0���Q	  ��    ���Q	  hk  j�UP������ǃ�= �����   ��������  �E\���   �Q	  ���  �   �  ������P�   j �����[�Q	  �+   �  �  ��<   D�D��%  ������4  �t$hQ  �t$$h   �������0  �D$ ����t��4  �|$	ǉD$��	  �=     �L	  h�   ������t$h   ��������Q	  �Eh3  �����Ph   ��������Q	  ���  hs  �t$�����ZY�����QR�4�@  �Chd  Ph   �������p  ���
  �FRh�  Ph�  ������
   2   V            0   B   H   �cL �cL �   @       0        ��������(   ^            0   B   H   UUS UUS �   @       0  ������������
   2   V   ������������   O   ~   GZ GZ �   @      |              �  ���                �  ���         �> 9�A �   �         �cL ��O            GZ  �V #  � 2        GZ 9�] A  � 2        GZ GZ _  � 2        GZ ��O x  � 2        �cL  @Q :        	   GZ  @Q �    �     
   UUS ��Z �            GZ  @Q �  � 2        GZ  @Q T  � 2     
 7 unknown , dual  stereo mono %s: Audio:     Mode:     %s
 Mode:     %s (%s%s)
 Standard: %s (%s%s)
 Audmode:  0x%04x
 ACB:      0x%04x
 resume
 msp_reset
 chip reset failed
 msp_read(0x%x, 0x%x): 0x%x
 msp_write(0x%x, 0x%x, 0x%x)
 no on yes off suspend
 nicam  and  radio msp3400 msp3400 not found
 &state->wq rev1=0x%04x, rev2=0x%04x
 msp34xx kernel_thread() failed
 switching to radio mode
 AM (for carrier detect) FM Radio Terrestrial FM-mono/stereo Satellite FM-mono NICAM/FM (B/G, D/K) NICAM/FM (I) NICAM/AM (L) BTSC External input manual autodetect autodetect and autoselect in1 in2 in3 in4 in1 da in2 da mute 7 unknown FM set_audmode: %s
 SAT set_audmode: %s
 NICAM set_audmode: %s
 BTSC set_audmode: %s
 extern set_audmode: %s
 FM-Radio set_audmode: %s
 mono set_audmode
 set audmode %d
 switching to AM mono
 set_mode: %d
 stereo detect register: %d
 nicam sync=%d, mode=%d
 watch: nicam %d => %d
 msp3410 daemon started
 msp3410 thread: sleep
 msp3410 thread: wakeup
 thread: restart scan
 thread: no carrier scan
 detection still in progress
 5.5/5.85  B/G NICAM FM thread: exit
 msp34xxg daemon started
 msp34xxg thread: sleep
 msp34xxg thread: wakeup
 selected radio modus
 selected M (EIA-J) modus
 selected M (A2) modus
 selected SECAM-L modus
 selected M (BTSC) modus
 msp3400 daemon started
 msp3400 thread: sleep
 msp3400 thread: wakeup
 AM sound override
 carrier1 val: %5d / %s
 carrier2 val: %5d / %s
 mono stereo lang2 lang1 lang1+lang2 5.85  PAL D/K + SECAM NICAM 6.25  PAL D/K1 FM-stereo 6.74  PAL D/K2 FM-stereo 7.02  PAL SAT FM-stereo s/b 7.20  PAL SAT FM-stereo s 7.38  PAL SAT FM-stereo b 5.742 PAL B/G FM-stereo 5.85  PAL B/G NICAM 4.5   NTSC 5.5   PAL B/G 6.0   PAL I 6.5   PAL D/K + SAT + SECAM autodetect start 4.5/4.72  M Dual FM-Stereo 5.5/5.74  B/G Dual FM-Stereo 6.5/6.25  D/K1 Dual FM-Stereo 6.5/6.74  D/K2 Dual FM-Stereo 6.5  D/K FM-Mono (HDEV3) 6.5/5.74  D/K3 Dual FM-Stereo 6.5/5.85  L NICAM AM 6.0/6.55  I NICAM FM 6.5/5.85  D/K NICAM FM 4.5  M BTSC-Stereo 4.5  M BTSC-Mono + SAP 4.5  M EIA-J Japan Stereo 10.7  FM-Stereo Radio 6.5  SAT-Mono 7.02/7.20  SAT-Stereo 7.2  SAT ADR     `   �      @  �     P  �  �  �  0  �  �      
 %s rev1 = 0x%04x rev2 = 0x%04x
 Routing:  0x%08x (input) 0x%08x (output)
   I/O error #%d (read 0x%02x/0x%02x)
 resetting chip, sound will go off.
 I/O error #%d (write 0x%02x/0x%02x)
    mute=%s scanning=%s volume=%d
  not an msp3400 (cannot read chip version)
  MSP%d4%02d%c-%c%d found on %s: supports %s%s%s, mode is %s
 scart switch: %s => %d (ACB=0x%04x)
    set source to %d (0x%x) for output %02x
    could not detect sound standard set_audmode called with mode=%d instead of set_source (ignored)
    set_audmode final source/matrix = 0x%x
 watch: rxsubchans %02x => %02x
 status=0x%x, stereo=%d, bilingual=%d -> rxsubchans=%d
  setting standard: %s (0x%04x)
  current standard: %s (0x%04x)
  autodetection failed, switching to backup standard: %s (0x%04x)
    started autodetect, waiting for result
 detection still in progress after 10 tries. giving up.
 detected standard: %s (0x%04x)
 mono sound carrier: %d.%03d MHz
    main sound carrier: %d.%03d MHz
    NICAM/FM carrier  : %d.%03d MHz
    NICAM/AM carrier  : %d.%03d MHz
    FM-stereo carrier : %d.%03d MHz
    6.5/5.85  D/K NICAM FM (HDEV2)  6.5/5.85  D/K NICAM FM (HDEV3) parm=dolby:Activates Dolby processing parm=amsound:Hardwire AM sound at 6.5Hz (France), FM can autoscan parm=standard:Specify audio standard: 32 = NTSC, 64 = radio, Default: Autodetect parm=stereo_threshold:Sets signal threshold to activate stereo parm=debug:Enable debug messages [0-3] parm=once:No continuous stereo monitoring parm=opmode:Forces a MSP3400 opmode. 0=Manual, 1=Autodetect, 2=Autodetect and autoselect parmtype=dolby:bool parmtype=amsound:bool parmtype=standard:int parmtype=stereo_threshold:int parmtype=debug:int parmtype=once:bool parmtype=opmode:int license=GPL author=Gerd Knorr description=device driver for msp34xx TV sound processor �          ��     �          ��     �          ��     �          ��     �          ��               ��               $� �             �                                  �                  +                                                              �           `                       �     ����                     @Q �   �V �  9�]    �a     d 9   �f S  ��O m   @Q �   �> �  �cL �  UUS �  GZ �   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                            
 �   @  c     �   �  B     �      P     �   �        �   P  |     �           �           �            �   �  3     �   �  �     �   �  p     
 {           �  �        �  0      
 �  y  �    �  �       
 �          
 �  `  l    
 �  .  *     �  @       
 �  0  V    �  �  �     �  X                        �            
     0  �   \    
 ?  `   0    
 F     0    
 S  `  ,    
 a  @      
 o  �  P    
 }      &     �  &   B     �  h   Q     �  �   ?     �  �   '     �    *     
  I  Y        �       9           G  �      
 Y  �       t          �  �      
 �  �       �  (        �  �  	    
 �  �       �  <          �      
 4          M  P        [  �      
 m         �  d        �        
 �  &       �  x        �        
 �  :       �  F         X  9     '           ��:  �  �     I  @  �     ]  4      
 g         
 q  s  .     �  �  �     �  �     
 �     5    �  H      
 �  �  -    �  `  @   
 �  �  $       �  �     %  �  3    :  �	  �    O           l  �   0     �  �        �  �  O    �             �          �             �             �                          
     [  �        m  `
    �	  `  X    �	  @
             
  p  T     )
             1
             >
             M
             _
             v
             �
             �
  `   0    
 �
             �
   
   
             �
             �
               �   �                 -          6              msp3400-driver.c msp_s_i2s_clock_freq msp_s_i2s_clock_freq.cold msp_querystd msp_s_tuner msp_log_status msp_log_status.cold CSWTCH.74 msp_g_tuner msp_s_frequency msp_resume msp_resume.cold msp_s_std reset_off.1 reset_on.2 write.3 msp_reset.cold msp_read msp_read.cold msp_write msp_write.cold msp_s_ctrl msp_s_ctrl.cold msp_suspend msp_suspend.cold msp_remove msp_probe msp_ops __key.4 msp_ctrl_ops msp_probe.cold opmode_str CSWTCH.84 scarts msp_set_scart.cold scart_names msp_s_routing msp_s_radio msp_s_radio.cold msp_driver_init msp_driver msp_driver_exit msp3400_pm_ops msp_id msp_core_ops msp_tuner_ops msp_audio_ops msp_video_ops __UNIQUE_ID_dolby282 __UNIQUE_ID_amsound281 __UNIQUE_ID_standard280 __UNIQUE_ID_stereo_threshold279 __UNIQUE_ID_debug278 __UNIQUE_ID_once277 __UNIQUE_ID_opmode276 __UNIQUE_ID_dolbytype275 __param_dolby __param_str_dolby __UNIQUE_ID_amsoundtype274 __param_amsound __param_str_amsound __UNIQUE_ID_standardtype273 __param_standard __param_str_standard __UNIQUE_ID_stereo_thresholdtype272 __param_stereo_threshold __param_str_stereo_threshold __UNIQUE_ID_debugtype271 __param_debug __param_str_debug __UNIQUE_ID_oncetype270 __param_once __param_str_once __UNIQUE_ID_opmodetype269 __param_opmode __param_str_opmode __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 msp3400-kthreads.c msp_set_source msp34xxg_set_source CSWTCH.91 CSWTCH.92 msp34xxg_set_source.cold msp34xxg_set_sources msp_stdlist msp3400c_set_audmode strmode.0 msp3400c_set_audmode.cold msp3400c_init_data msp3400c_set_mode.cold msp_detect_stereo.cold msp3410d_thread.cold msp34xxg_thread.cold msp3400c_carrier_detect_main msp3400c_carrier_detect_65 msp3400c_carrier_detect_55 msp3400c_thread.cold v4l2_i2c_subdev_init msp_debug devm_kmalloc __msecs_to_jiffies __this_module snprintf __stack_chk_guard msp3410d_thread v4l2_ctrl_handler_init_class cleanup_module msp_stereo_thresh msp_write_dsp set_freezable msp_dolby schedule_timeout_interruptible msp_amsound add_wait_queue msp3400c_set_carrier i2c_transfer_buffer_flags kthread_should_stop __wake_up msp_detect_stereo __fentry__ init_module wake_up_process __refrigerator msp_standard_std_name msp_reset v4l2_ctrl_handler_log_status v4l2_ctrl_handler_setup schedule __stack_chk_fail msp_set_audmode _dev_info i2c_register_driver v4l2_device_unregister_subdev _dev_err msp_standard mutex_lock msp_sleep media_entity_pads_init kthread_stop freezing_slow_path current_task _dev_warn msp_set_scart default_wake_function __init_waitqueue_head msp3400c_thread msp3400c_set_mode msp_write_dem kthread_create_on_node msp_update_volume strscpy mutex_unlock param_ops_bool v4l2_ctrl_new_std v4l2_ctrl_handler_free system_freezing_cnt i2c_transfer __mod_i2c__msp_id_device_table remove_wait_queue msp_read_dsp msp_read_dem i2c_del_driver param_ops_int _dev_printk msp34xxg_thread v4l2_ctrl_cluster msp_once __SCT__might_resched     �  
   u       a   �  �   u  �   �  �   	  �     �   �  �   �  �   �    �    z  )    0  �  5    A  �  �  �  �  �  �  �    �  
  u      E  �  Q  �  �  �  �  �  �  z  �  u  2    B  
    )  |  C    a  �  k    �  �  �    �  �  �  �      .  �  8    [  �  e      �  �  u  �    �  �  �  �  �    �    �    
    $    4    R  �  W  u  _    �  �  �  u  �    �    �    �  u    u  
      �  =    B  u  J    a  �  �  u  �    �    �  �  �    �  �  �  �    �    �  #    /  �  D    N  �  [  �  w    �    �    �  �  4  �  M  �  a  �  �  �  �  u  �    9  �  ?  u  j  u  r    �  u  �      �    u  #    n    �  �  �  u  �    �  �  �  u  �    �  �  �  u  �    �  �    �  W  �  ^  u  l  u  �    �    �  u  �  �  �      u  $    ?  �  p    �    �  �  �  �  �  �  �  �    �  %  u  3    C  �  a    p    ~    �      �    �  1  u  9    J  �  v  �  �    �  �  �  �  �  u  �    �  �  A   u  I     e     �     �     �   	  �     �     �     �   �  �   u  �     !  �  !  u  !    )!  �  .!  u  7!    ?!    K!  �  }!  �  �!  �  "    !"    4"  ~  @"  �  \"  �  {"  �  �"  u  �"    �"  �  �"  �  �"  u  �"    �"  u  	#    #    &#    -#    I#  �  [#    u#  u  �#  u  �#    �#  �  �#  u  �#  u  �#    �#    $  �  -$  �  ;$  �  R$  �  h$  �  �$  �  �$  �  �$  u  �$    �$  u  �$    �$  u  �$    %  u  %    +%  �  :%  u  B%    Q%  u  Y%    h%  u  p%    �%    �%  	  �%    �%    �%  �  �%  u  �%    �%  �  &  u  	&    &  �  &  u  #&    /&  �  [&  �  d&  �  �&  �  �&  u  �&    �&  u  �&    �&  �  �&  �  �&  u  �&    �&    '    ''  �  x'  �  �'  u  �'    �'    �'  �  �'  �  �'  u  �'    �'  �  (  �  8(  u  @(    Z(  �  f(  �  o(  u  �(  �  �(    �(  u  �(    !)  �  4)  �  L)  �  Q)  u  �)  �  �)  �  �)  �  �)  �  �)  �  �)  �  �)    C*    U*  �  �*    �*  u  �*    �*  �  +  u  +    !+    E+  u  R+    l+    y+  �  �+    �+    	,  �  ,    ,,  �  h,    �,  �  �,                 �  .     F   	  L   �  U     a   y  p   �  �     �     �     �     �     �     �   �  �     �     �   �  �     �   �     	    �        �  %  z  <    B  �  P    U    Z    f    t  �  |    �  �  �  �  �    �    �  �  �    �    �  �  �    �    �    �  �  �    �    �    �  �        	    �  &  �  3  	  <  �  C  �  M    R    ]  	  f  �  p  �  }  	  �  �  �  �  �    �    �    �  �  �    �    �    �    �    �  	  �    �  �          %    *    /  	  5    :  �  I    Q    V    [    b    h    m  �  u    z    �    �    �  	  �  �  �    �    �    �       �    �  +    Q  �  W    \    a    f    x    �    �    �  �  �    �  u  �    �  	  �    �  �  �    �          �          #  �  3    ;  	  A    F  �  T    \    b    g  �  o    �  	  �    �  �  �    �    �    �  �  �    �    �    �  �  �    �  	  �    �  �  �              �  #    4    =    B    K    Q    V  �  ^    g    m    r  �  x  u  �    �    �    �  �  �    �    �    �  �  �  u  �    �    �    �  �  �  u  �    �    �    �  �                �  )  	  /    4  �  <    F    L    Q  �  [    e    k    p  �  z    �    �    �  �  �    �    �    �  �  �  u  �    �    �    �    �    �  �  �    �          �        	  "    '  �  /    9    ?    D  �  L    [  	  a    f  �  x    �    �    �  �  �    �    �  	  �    �  �  �    �    �    �  �  �    �    �  	  	    	  �  	    	    !	    &	  �  .	    6	    <	    A	  �  I	    Q	    W	    \	  �  d	    i	    r	    w	  �  	    �	    �	    �	  �  �	    �	    �	    �	  �  �	  u  �	    �	    �	  	  �	    �	  �  �	    �	    �	    �	  �  �	    �	  	  
    

  �  
    
    
  	  '
    ,
  �  :
    ?
    H
    M
  �  U
    Z
    c
    h
  �  m
  u  u
    z
    �
    �
  �  �
    �
    �
    �
  �  �
    �
    �
    �
  �  �
    �
    �
    �
  �  �
    �
  	  �
    �
  �     u      
                  Z   	   @       l�                 m             @;  �                  i   	   @       |�  0     
         u      2       �B  b                �             RK  �                  �   	   @       ��  (     
  include/linux/security.h \
    $(wildcard include/config/SECURITY_INFINIBAND) \
    $(wildcard include/config/SECURITY_NETWORK_XFRM) \
    $(wildcard include/config/SECURITY_PATH) \
    $(wildcard include/config/SECURITYFS) \
  include/linux/kernel_read_file.h \
  include/uapi/linux/netlink.h \
  include/uapi/linux/netdevice.h \
  include/linux/if_ether.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
  include/uapi/linux/if_bonding.h \
  include/uapi/linux/pkt_cls.h \
  include/uapi/linux/pkt_sched.h \
  include/linux/hashtable.h \
  include/linux/etherdevice.h \
  include/linux/crc32.h \
  include/linux/bitrev.h \
    $(wildcard include/config/HAVE_ARCH_BITREVERSE) \
  arch/x86/include/generated/asm/unaligned.h \
  include/asm-generic/unaligned.h \
  include/linux/unaligned/packed_struct.h \
  include/linux/rtnetlink.h \
    $(wildcard include/config/NET_INGRESS) \
    $(wildcard include/config/NET_EGRESS) \
  include/uapi/linux/rtnetlink.h \
  include/uapi/linux/if_addr.h \
  include/uapi/linux/if_vlan.h \
  include/crypto/sha1.h \
  include/net/sch_generic.h \
  include/uapi/linux/pkt_cls.h \
  include/net/gen_stats.h \
  include/uapi/linux/gen_stats.h \
  include/net/rtnetlink.h \
  include/net/netlink.h \
  include/net/flow_offload.h \
  include/uapi/linux/filter.h \
  include/linux/bpf-cgroup.h \
  include/net/sock.h \
    $(wildcard include/config/SOCK_RX_QUEUE_MAPPING) \
    $(wildcard include/config/SOCK_VALIDATE_XMIT) \
  include/linux/page_counter.h \
  include/linux/memcontrol.h \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/rculist_nulls.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/indirect_call_wrapper.h \
  include/net/dst.h \
  include/net/neighbour.h \
  include/net/tcp_states.h \
  include/uapi/linux/net_tstamp.h \
  include/net/l3mdev.h \
  include/net/fib_rules.h \
  include/uapi/linux/fib_rules.h \
  include/net/fib_notifier.h \
  include/net/bpf_sk_storage.h \
  include/uapi/linux/sock_diag.h \
  include/uapi/linux/btf.h \
  include/linux/bpf_local_storage.h \
  kernel/bpf/../cgroup/cgroup-internal.h \
  include/linux/fs_parser.h \
    $(wildcard include/config/VALIDATE_FS_PARSER) \
  include/linux/fs_context.h \

kernel/bpf/cgroup.o: $(deps_kernel/bpf/cgroup.o)

$(deps_kernel/bpf/cgroup.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    *pf;
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0 */
/*
 * kernel/lockdep_internals.h
 *
 * Runtime locking correctness validator
 *
 * lockdep subsystem internal functions and variables.
 */

/*
 * Lock-class usage-state bits:
 */
enum lock_usage_bit {
#define LOCKDEP_STATE(__STATE)		\
	LOCK_USED_IN_##__STATE,		\
	LOCK_USED_IN_##__STATE##_READ,	\
	LOCK_ENABLED_##__STATE,		\
	LOCK_ENABLED_##__STATE##_READ,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	LOCK_USED,
	LOCK_USED_READ,
	LOCK_USAGE_STATES,
};

/* states after LOCK_USED_READ are not traced and printed */
static_assert(LOCK_TRACE_STATES == LOCK_USAGE_STATES);

#define LOCK_USAGE_READ_MASK 1
#define LOCK_USAGE_DIR_MASK  2
#define LOCK_USAGE_STATE_MASK (~(LOCK_USAGE_READ_MASK | LOCK_USAGE_DIR_MASK))

/*
 * Usage-state bitmasks:
 */
#define __LOCKF(__STATE)	LOCKF_##__STATE = (1 << LOCK_##__STATE),

enum {
#define LOCKDEP_STATE(__STATE)						\
	__LOCKF(USED_IN_##__STATE)					\
	__LOCKF(USED_IN_##__STATE##_READ)				\
	__LOCKF(ENABLED_##__STATE)					\
	__LOCKF(ENABLED_##__STATE##_READ)
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	__LOCKF(USED)
	__LOCKF(USED_READ)
};

#define LOCKDEP_STATE(__STATE)	LOCKF_ENABLED_##__STATE |
static const unsigned long LOCKF_ENABLED_IRQ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_USED_IN_##__STATE |
static const unsigned long LOCKF_USED_IN_IRQ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_ENABLED_##__STATE##_READ |
static const unsigned long LOCKF_ENABLED_IRQ_READ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_USED_IN_##__STATE##_READ |
static const unsigned long LOCKF_USED_IN_IRQ_READ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKF_ENABLED_IRQ_ALL (LOCKF_ENABLED_IRQ | LOCKF_ENABLED_IRQ_READ)
#define LOCKF_USED_IN_IRQ_ALL (LOCKF_USED_IN_IRQ | LOCKF_USED_IN_IRQ_READ)

#define LOCKF_IRQ (LOCKF_ENABLED_IRQ | LOCKF_USED_IN_IRQ)
#define LOCKF_IRQ_READ (LOCKF_ENABLED_IRQ_READ | LOCKF_USED_IN_IRQ_READ)

/*
 * CONFIG_LOCKDEP_SMALL is defined for sparc. Sparc requires .text,
 * .data and .bss to fit in required 32MB limit for the kernel. With
 * CONFIG_LOCKDEP we could go over this limit and cause system boot-up problems.
 * So, reduce the static allocations for lockdeps related structures so that
 * everything fits in current required size limit.
 */
#ifdef CONFIG_LOCKDEP_SMALL
/*
 * MAX_LOCKDEP_ENTRIES is the maximum number of lock dependencies
 * we track.
 *
 * We use the per-lock dependency maps in two ways: we grow it by adding
 * every to-be-taken lock to all currently held lock's own dependency
 * table (if it's not there yet), and we check it for lock order
 * conflicts and deadlocks.
 */
#define MAX_LOCKDEP_ENTRIES	16384UL
#define MAX_LOCKDEP_CHAINS_BITS	15
#define MAX_STACK_TRACE_ENTRIES	262144UL
#define STACK_TRACE_HASH_SIZE	8192
#else
#define MAX_LOCKDEP_ENTRIES	(1UL << CONFIG_LOCKDEP_BITS)

#define MAX_LOCKDEP_CHAINS_BITS	CONFIG_LOCKDEP_CHAINS_BITS

/*
 * Stack-trace: tightly packed array of stack backtrace
 * addresses. Protected by the hash_lock.
 */
#define MAX_STACK_TRACE_ENTRIES	(1UL << CONFIG_LOCKDEP_STACK_TRACE_BITS)
#define STACK_TRACE_HASH_SIZE	(1 << CONFIG_LOCKDEP_STACK_TRACE_HASH_BITS)
#endif

/*
 * Bit definitions for lock_chain.irq_context
 */
#define LOCK_CHAIN_SOFTIRQ_CONTEXT	(1 << 0)
#define LOCK_CHAIN_HARDIRQ_CONTEXT	(1 << 1)

#define MAX_LOCKDEP_CHAINS	(1UL << MAX_LOCKDEP_CHAINS_BITS)

#define MAX_LOCKDEP_CHAIN_HLOCKS (MAX_LOCKDEP_CHAINS*5)

extern struct lock_chain lock_chains[];

#define LOCK_USAGE_CHARS (2*XXX_LOCK_USAGE_STATES + 1)

extern void get_usage_chars(struct lock_class *class,
			    char usage[LOCK_USAGE_CHARS]);

extern const char *__get_key_name(const struct lockdep_subclass_key *key,
				  char *str);

struct lock_class *lock_chain_get_class(struct lock_chain *chain, int i);

extern unsigned long nr_lock_classes;
extern unsigned long nr_zapped_classes;
extern unsigned long nr_zapped_lock_chains;
extern unsigned long nr_list_entries;
long lockdep_next_lockchain(long i);
unsigned long lock_chain_count(void);
extern unsigned long nr_stack_trace_entries;

extern unsigned int nr_hardirq_chains;
extern unsigned int nr_softirq_chains;
extern unsigned int nr_process_chains;
extern unsigned int nr_free_chain_hlocks;
extern unsigned int nr_lost_chain_hlocks;
extern unsigned int nr_large_chain_blocks;

extern unsigned int max_lockdep_depth;
extern unsigned int max_bfs_queue_depth;
extern unsigned long max_lock_class_idx;

extern struct lock_class lock_classes[MAX_LOCKDEP_KEYS];
extern unsigned long lock_classes_in_use[];

#ifdef CONFIG_PROVE_LOCKING
extern unsigned long lockdep_count_forward_deps(struct lock_class *);
extern unsigned long lockdep_count_backward_deps(struct lock_class *);
#ifdef CONFIG_TRACE_IRQFLAGS
u64 lockdep_stack_trace_count(void);
u64 lockdep_stack_hash_count(void);
#endif
#else
static inline unsigned long
lockdep_count_forward_deps(struct lock_class *class)
{
	return 0;
}
static inline unsigned long
lockdep_count_backward_deps(struct lock_class *class)
{
	return 0;
}
#endif

#ifdef CONFIG_DEBUG_LOCKDEP

#include <asm/local.h>
/*
 * Various lockdep statistics.
 * We want them per cpu as they are often accessed in fast path
 * and we want to avoid too much cache bouncing.
 */
struct lockdep_stats {
	unsigned long  chain_lookup_hits;
	unsigned int   chain_lookup_misses;
	unsigned long  hardirqs_on_events;
	unsigned long  hardirqs_off_events;
	unsigned long  redundant_hardirqs_on;
	unsigned long  redundant_hardirqs_off;
	unsigned long  softirqs_on_events;
	unsigned long  softirqs_off_events;
	unsigned long  redundant_softirqs_on;
	unsigned long  redundant_softirqs_off;
	int            nr_unused_locks;
	unsigned int   nr_redundant_checks;
	unsigned int   nr_redundant;
	unsigned int   nr_cyclic_checks;
	unsigned int   nr_find_usage_forwards_checks;
	unsigned int   nr_find_usage_backwards_checks;

	/*
	 * Per lock class locking operation stat counts
	 */
	unsigned long lock_class_ops[MAX_LOCKDEP_KEYS];
};

DECLARE_PER_CPU(struct lockdep_stats, lockdep_stats);

#define __debug_atomic_inc(ptr)					\
	this_cpu_inc(lockdep_stats.ptr);

#define debug_atomic_inc(ptr)			{		\
	WARN_ON_ONCE(!irqs_disabled());				\
	__this_cpu_inc(lockdep_stats.ptr);			\
}

#define debug_atomic_dec(ptr)			{		\
	WARN_ON_ONCE(!irqs_disabled());				\
	__this_cpu_dec(lockdep_stats.ptr);			\
}

#define debug_atomic_read(ptr)		({				\
	struct lockdep_stats *__cpu_lockdep_stats;			\
	unsigned long long __total = 0;					\
	int __cpu;							\
	for_each_possible_cpu(__cpu) {					\
		__cpu_lockdep_stats = &per_cpu(lockdep_stats, __cpu);	\
		__total += __cpu_lockdep_stats->ptr;			\
	}								\
	__total;							\
})

static inline void debug_class_ops_inc(struct lock_class *class)
{
	int idx;

	idx = class - lock_classes;
	__debug_atomic_inc(lock_class_ops[idx]);
}

static inline unsigned long debug_class_ops_read(struct lock_class *class)
{
	int idx, cpu;
	unsigned long ops = 0;

	idx = class - lock_classes;
	for_each_possible_cpu(cpu)
		ops += per_cpu(lockdep_stats.lock_class_ops[idx], cpu);
	return ops;
}

#else
# define __debug_atomic_inc(ptr)	do { } while (0)
# define debug_atomic_inc(ptr)		do { } while (0)
# define debug_atomic_dec(ptr)		do { } while (0)
# define debug_atomic_read(ptr)		0
# define debug_class_ops_inc(ptr)	do { } while (0)
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      /* SPDX-License-Identifier: GPL-2.0 */
/*
 * kernel/lockdep_internals.h
 *
 * Runtime locking correctness validator
 *
 * lockdep subsystem internal functions and variables.
 */

/*
 * Lock-class usage-state bits:
 */
enum lock_usage_bit {
#define LOCKDEP_STATE(__STATE)		\
	LOCK_USED_IN_##__STATE,		\
	LOCK_USED_IN_##__STATE##_READ,	\
	LOCK_ENABLED_##__STATE,		\
	LOCK_ENABLED_##__STATE##_READ,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	LOCK_USED,
	LOCK_USED_READ,
	LOCK_USAGE_STATES,
};

/* states after LOCK_USED_READ are not traced and printed */
static_assert(LOCK_TRACE_STATES == LOCK_USAGE_STATES);

#define LOCK_USAGE_READ_MASK 1
#define LOCK_USAGE_DIR_MASK  2
#define LOCK_USAGE_STATE_MASK (~(LOCK_USAGE_READ_MASK | LOCK_USAGE_DIR_MASK))

/*
 * Usage-state bitmasks:
 */
#define __LOCKF(__STATE)	LOCKF_##__STATE = (1 << LOCK_##__STATE),

enum {
#define LOCKDEP_STATE(__STATE)						\
	__LOCKF(USED_IN_##__STATE)					\
	__LOCKF(USED_IN_##__STATE##_READ)				\
	__LOCKF(ENABLED_##__STATE)					\
	__LOCKF(ENABLED_##__STATE##_READ)
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	__LOCKF(USED)
	__LOCKF(USED_READ)
};

#define LOCKDEP_STATE(__STATE)	LOCKF_ENABLED_##__STATE |
static const unsigned long LOCKF_ENABLED_IRQ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_USED_IN_##__STATE |
static const unsigned long LOCKF_USED_IN_IRQ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_ENABLED_##__STATE##_READ |
static const unsigned long LOCKF_ENABLED_IRQ_READ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKDEP_STATE(__STATE)	LOCKF_USED_IN_##__STATE##_READ |
static const unsigned long LOCKF_USED_IN_IRQ_READ =
#include "lockdep_states.h"
	0;
#undef LOCKDEP_STATE

#define LOCKF_ENABLED_IRQ_ALL (LOCKF_ENABLED_IRQ | LOCKF_ENABLED_IRQ_READ)
#define LOCKF_USED_IN_IRQ_ALL (LOCKF_USED_IN_IRQ | LOCKF_USED_IN_IRQ_READ)

#define LOCKF_IRQ (LOCKF_ENABLED_IRQ | LOCKF_USED_IN_IRQ)
#define LOCKF_IRQ_READ (LOCKF_ENABLED_IRQ_READ | LOCKF_USED_IN_IRQ_READ)

/*
 * CONFIG_LOCKDEP_SMALL is defined for sparc. Sparc requires .text,
 * .data and .bss to fit in required 32MB limit for the kernel. With
 * CONFIG_LOCKDEP we could go over this limit and cause system boot-up problems.
 * So, reduce the static allocations for lockdeps related structures so that
 * everything fits in current required size limit.
 */
#ifdef CONFIG_LOCKDEP_SMALL
/*
 * MAX_LOCKDEP_ENTRIES is the maximum number of lock dependencies
 * we track.
 *
 * We use the per-lock dependency maps in two ways: we grow it by adding
 * every to-be-taken lock to all currently held lock's own dependency
 * table (if it's not there yet), and we check it for lock order
 * conflicts and deadlocks.
 */
#define MAX_LOCKDEP_ENTRIES	16384UL
#define MAX_LOCKDEP_CHAINS_BITS	15
#define MAX_STACK_TRACE_ENTRIES	262144UL
#define STACK_TRACE_HASH_SIZE	8192
#else
#define MAX_LOCKDEP_ENTRIES	(1UL << CONFIG_LOCKDEP_BITS)

#define MAX_LOCKDEP_CHAINS_BITS	CONFIG_LOCKDEP_CHAINS_BITS

/*
 * Stack-trace: tightly packed array of stack backtrace
 * addresses. Protected by the hash_lock.
 */
#define MAX_STACK_TRACE_ENTRIES	(1UL << CONFIG_LOCKDEP_STACK_TRACE_BITS)
#define STACK_TRACE_HASH_SIZE	(1 << CONFIG_LOCKDEP_STACK_TRACE_HASH_BITS)
#endif

/*
 * Bit definitions for lock_chain.irq_context
 */
#define LOCK_CHAIN_SOFTIRQ_CONTEXT	(1 << 0)
#define LOCK_CHAIN_HARDIRQ_CONTEXT	(1 << 1)

#define MAX_LOCKDEP_CHAINS	(1UL << MAX_LOCKDEP_CHAINS_BITS)

#define MAX_LOCKDEP_CHAIN_HLOCKS (MAX_LOCKDEP_CHAINS*5)

extern struct lock_chain lock_chains[];

#define LOCK_USAGE_CHARS (2*XXX_LOCK_USAGE_STATES + 1)

extern void get_usage_chars(struct lock_class *class,
			    char usage[LOCK_USAGE_CHARS]);

extern const char *__get_key_name(const struct lockdep_subclass_key *key,
				  char *str);

struct lock_class *lock_chain_get_class(struct lock_chain *chain, int i);

extern unsigned long nr_lock_classes;
extern unsigned long nr_zapped_classes;
extern unsigned long nr_zapped_lock_chains;
extern unsigned long nr_list_entries;
long lockdep_next_lockchain(long i);
unsigned long lock_chain_count(void);
extern unsigned long nr_stack_trace_entries;

extern unsigned int nr_hardirq_chains;
extern unsigned int nr_softirq_chains;
extern unsigned int nr_process_chains;
extern unsigned int nr_free_chain_hlocks;
extern unsigned int nr_lost_chain_hlocks;
extern unsigned int nr_large_chain_blocks;

extern unsigned int max_lockdep_depth;
ext