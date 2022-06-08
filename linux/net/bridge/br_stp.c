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
���}H/0T�:��z�� aG��3�c|�a�&����G�T�si���5�s�e�xb�f��%�L�M���Hd7�I>�L���%�(�'QCa.� �]�^/G���6Ws�%�h�5E�GKMU�׿<��A�8V,�� �r���I�V�Ŵv[r���dM��mXd�'���"kȥ��Pz�;	���� 5