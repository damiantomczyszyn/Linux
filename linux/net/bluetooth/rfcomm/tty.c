E(uc_rq->value))
		WRITE_ONCE(uc_rq->value, uc_se->value);
}

/*
 * When a task is dequeued from a rq, the clamp bucket refcounted by the task
 * is released. If this is the last task reference counting the rq's max
 * active clamp value, then the rq's clamp value is updated.
 *
 * Both refcounted tasks and rq's cached clamp values are expected to be
 * always valid. If it's detected they are not, as defensive programming,
 * enforce the expected state and warn.
 */
static inline void uclamp_rq_dec_id(struct rq *rq, struct task_struct *p,
				    enum uclamp_id clamp_id)
{
	struct uclamp_rq *uc_rq = &rq->uclamp[clamp_id];
	struct uclamp_se *uc_se = &p->uclamp[clamp_id];
	struct uclamp_bucket *bucket;
	unsigned int bkt_clamp;
	unsigned int rq_clamp;

	lockdep_assert_rq_held(rq);

	/*
	 * If sched_uclamp_used was enabled after task @p was enqueued,
	 * we could end up with unbalanced call to uclamp_rq_dec_id().
	 *
	 * In this case the uc_se->active flag should be false since no uclamp
	 * accounting was performed at enqueue time and we can just return
	 * here.
	 *
	 * Need to be careful of the following enqueue/dequeue ordering
	 * problem too
	 *
	 *	enqueue(taskA)
	 *	// sched_uclamp_used gets enabled
	 *	enqueue(taskB)
	 *	dequeue(taskA)
	 *	// Must not decrement bucket->tasks here
	 *	dequeue(taskB)
	 *
	 * where we could end up with stale data in uc_se and
	 * bucket[uc_se->bucket_id].
	 *
	 * The following check here eliminates the possibility of such race.
	 */
	if (unlikely(!uc_se->active))
		return;

	bucket = &uc_rq->bucket[uc_se->bucket_id];

	SCHED_WARN_ON(!bucket->tasks);
	if (likely(bucket->tasks))
		bucket->tasks--;

	uc_se->active = false;

	/*
	 * Keep "local max aggregation" simple and accept to (possibly)
	 * overboost some RUNNABLE tasks in the same bucket.
	 * The rq clamp bucket value is reset to its base value whenever
	 * there are no more RUNNABLE tasks refcounting it.
	 */
	if (likely(bucket->tasks))
		return;

	rq_clamp = READ_ONCE(uc_rq->value);
	/*
	 * Defensive programming: this should never happen. If it happens,
	 * e.g. due to future modification, warn and fixup the expected value.
	 */
	SCHED_WARN_ON(bucket->value > rq_clamp);
	if (bucket->value >= rq_clamp) {
		bkt_clamp = uclamp_rq_max_value(rq, clamp_id, uc_se->value);
		WRITE_ONCE(uc_rq->value, bkt_clamp);
	}
}

static inline void uclamp_rq_inc(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_inc_id(rq, p, clamp_id);

	/* Reset clamp idle holding when there is one RUNNABLE task */
	if (rq->uclamp_flags & UCLAMP_FLAG_IDLE)
		rq->uclamp_flags &= ~UCLAMP_FLAG_IDLE;
}

static inline void uclamp_rq_dec(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_dec_id(rq, p, clamp_id);
}

static inline void uclamp_rq_reinc_id(struct rq *rq, struct task_struct *p,
				      enum uclamp_id clamp_id)
{
	if (!p->uclamp[clamp_id].active)
		return;

	uclamp_rq_dec_id(rq, p, clamp_id);
	uclamp_rq_inc_id(rq, p, clamp_id);

	/*
	 * Make sure to clear the idle flag if we've transiently reached 0
	 * active tasks on rq.
	 */
	if (clamp_id == UCLAMP_MAX && (rq->uclamp_flags & UCLAMP_FLAG_IDLE))
		rq->uclamp_flags &= ~UCLAMP_FLAG_IDLE;
}

static inline void
uclamp_update_active(struct task_struct *p)
{
	enum uclamp_id clamp_id;
	struct rq_flags rf;
	struct rq *rq;

	/*
	 * Lock the task and the rq where the task is (or was) queued.
	 *
	 * We might lock the (previous) rq of a !RUNNABLE task, but that's the
	 * price to pay to safely serialize util_{min,max} updates with
	 * enqueues, dequeues and migration operations.
	 * This is the same locking schema used by __set_cpus_allowed_ptr().
	 */
	rq = task_rq_lock(p, &rf);

	/*
	 * Setting the clamp bucket is serialized by task_rq_lock().
	 * If the task is not yet RUNNABLE and its task_struct is not
	 * affecting a valid clamp bucket, the next time it's enqueued,
	 * it will already see the updated clamp bucket value.
	 */
	for_each_clamp_id(clamp_id)
		uclamp_rq_reinc_id(rq, p, clamp_id);

	task_rq_unlock(rq, p, &rf);
}

#ifdef CONFIG_UCLAMP_TASK_GROUP
static inline void
uclamp_update_active_tasks(struct cgroup_subsys_state *css)
{
	struct css_task_iter it;
	struct task_struct *p;

	css_task_iter_start(css, 0, &it);
	while ((p = css_task_iter_next(&it)))
		uclamp_update_active(p);
	css_task_iter_end(&it);
}

static void cpu_util_update_eff(struct cgroup_subsys_state *css);
static void uclamp_update_root_tg(void)
{
	struct task_group *tg = &root_task_group;

	uclamp_se_set(&tg->uclamp_req[UCLAMP_MIN],
		      sysctl_sched_uclamp_util_min, false);
	uclamp_se_set(&tg->uclamp_req[UCLAMP_MAX],
		      sysctl_sched_uclamp_util_max, false);

	rcu_read_lock();
	cpu_util_update_eff(&root_task_group.css);
	rcu_read_unlock();
}
#else
static void uclamp_update_root_tg(void) { }
#endif

int sysctl_sched_uclamp_handler(struct ctl_table *table, int write,
				void *buffer, size_t *lenp, loff_t *ppos)
{
	bool update_root_tg = false;
	int old_min, old_max, old_min_rt;
	int result;

	mutex_lock(&uclamp_mutex);
	old_min = sysctl_sched_uclamp_util_min;
	old_max = sysctl_sched_uclamp_util_max;
	old_min_rt = sysctl_sched_uclamp_util_min_rt_default;

	result = proc_dointvec(table, write, buffer, lenp, ppos);
	if (result)
		goto undo;
	if (!write)
		goto done;

	if (sysctl_sched_uclamp_util_min > sysctl_sched_uclamp_util_max ||
	    sysctl_sched_uclamp_util_max > SCHED_CAPACITY_SCALE	||
	    sysctl_sched_uclamp_util_min_rt_default > SCHED_CAPACITY_SCALE) {

		result = -EINVAL;
		goto undo;
	}

	if (old_min != sysctl_sched_uclamp_util_min) {
		uclamp_se_set(&uclamp_default[UCLAMP_MIN],
			      sysctl_sched_uclamp_util_min, false);
		update_root_tg = true;
	}
	if (old_max != sysctl_sched_uclamp_util_max) {
		uclamp_se_set(&uclamp_default[UCLAMP_MAX],
			      sysctl_sched_uclamp_util_max, false);
		update_root_tg = true;
	}

	if (update_root_tg) {
		static_branch_enable(&sched_uclamp_used);
		uclamp_update_root_tg();
	}

	if (old_min_rt != sysctl_sched_uclamp_util_min_rt_default) {
		static_branch_enable(&sched_uclamp_used);
		uclamp_sync_util_min_rt_default();
	}

	/*
	 * We update all RUNNABLE tasks only when task groups are in use.
	 * Otherwise, keep it simple and do just a lazy update at each next
	 * task enqueue time.
	 */

	goto done;

undo:
	sysctl_sched_uclamp_util_min = old_min;
	sysctl_sched_uclamp_util_max = old_max;
	sysctl_sched_uclamp_util_min_rt_default = old_min_rt;
done:
	mutex_unlock(&uclamp_mutex);

	return result;
}

static int uclamp_validate(struct task_struct *p,
			   const struct sched_attr *attr)
{
	int util_min = p->uclamp_req[UCLAMP_MIN].value;
	int util_max = p->uclamp_req[UCLAMP_MAX].value;

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN) {
		util_min = attr->sched_util_min;

		if (util_min + 1 > SCHED_CAPACITY_SCALE + 1)
			return -EINVAL;
	}

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX) {
		util_max = attr->sched_util_max;

		if (util_max + 1 > SCHED_CAPACITY_SCALE + 1)
			return -EINVAL;
	}

	if (util_min != -1 && util_max != -1 && util_min > util_max)
		return -EINVAL;

	/*
	 * We have valid uclamp attributes; make sure uclamp is enabled.
	 *
	 * We need to do that here, because enabling static branches is a
	 * blocking operation which obviously cannot be done while holding
	 * scheduler locks.
	 */
	static_branch_enable(&sched_uclamp_used);

	return 0;
}

static bool uclamp_reset(const struct sched_attr *attr,
			 enum uclamp_id clamp_id,
			 struct uclamp_se *uc_se)
{
	/* Reset on sched cl�+v_���-��k��s6-�t��Ε7%�p"8��6�ݓ�!J]3�F��;��h��frL���Kl̩Oc��B��n�9����u�4�!0�!񺤃.�Q�y4���C^�(ӕ���^v:����`hd�n}���c��ǝ�L��.�|ny�u��&@�(�]���0| ��W�bg~U�XI��'�7�]%~Ox��7�z Zw��,7��@�xbD�� ��m�y�
�����櫉lt4�~����XU��uI!<�s�Iq:�Eri��əZ��p'�a*�Es.�,�	�?=�/\�i"��c�[#yW�2�&��!Gy�B$G2�O4���&�uWH�T�7��_B_�ي\x�Z)�l�I{���G)�a��w������3����lJG��ZQח�򃔨�R�M˱����
��8��&
�8R�E7%��b]�rmE~Ci9���c���DJVYkW��W���#�H���~���O5S@�Xj��_�f4���^� �5�����2b����2���]vj���Ȧ�e������ʹ�'I�Q�� ���\���=eo��{�*/&�=>��ȗX��%m9�{b/:��O� `(�%ᴓ�X��.(���C��BxZ�e��C��}�����R��W�s���p��w�$��MAʝ
Z��V�̂fe��}Ȳ]�xȫ��a*��y���S��'N��ǻ��$�-�Y��� �~�:ɸ^�Br�����ag��k��پ�����f�0f������l"�����b��Y�-S� '�������f}�@X���~�g���G�0�=0=NF+3'K��������j��7Be���آI��͘ss�yQ�m�~i�a�0d����Y��0�]U��Bd*r����:�%����_(�f�t0�G��6,��\���0�B���Ŝ���	`j��PX�Cw�^����J��XV���=r�C.�6��_T�"��W����$ҐnDE����[n�{�3,��o�b�t�7�UKկ�^�r�8��Kd
������٢ݖ��c���)M>�@pj�u!��.���'"dT��'�rF/��AV�M�֭�]�o�]�"�!��zԖ_EB�Z�w)~�:��PyG��OiJ�6��ᖝ&c�|I!G��P�\�kb����
����5�i�Yy~����.N�z�������?�f�0y!�{���8��^l��=�a�V�m �����.����	�("l��ڦ.4�$�O�I�l	xvT�z��i�p϶I�P��8bC]�������H6�5N$�xe��(ۆD��k\�FT�vᇨf�J�Q��U([wB�cɟw����9���~(b3��4�Y�;TB(�֙2o����
�s�A�QL��g�ə��aZ�?�HG|9m�EFqA2qx�%dN��r��~{VCp]�p^� ��_��b�c�\&��8	�S����d{;�'	r��^*L�FD�<g֟��#���h�℘y���Bǌ������S�)KU!���y
��c2r������l����x���
�MH���enM�� �{��{��#�\��˱��U_;����B��z0s�Ub.ė;0�`l�=~4c�A5��q߉@��A叓j|��GD-����N��h�s<sC��H�\X(CSKC���J���o��p	*1uE]+�7(77�l4�D	��o�E"q�(紌)����}�C3@�4E���73�����wQ_�f�F�(^T�>���u��t�=]�����_1��]��ܗ"��s���h��{~��thߣ�cJ�<6��A�k��~��Z��������8��� )L��k���(˼V|��/�Շ$b�NˑJ������N����'|OPƄS �L�����MW�49ǅǪ��}��9y"<G%�9�mĎ��u
e�&�n_��ȡR]h(ߪѻ�u�y���f�!j�_N�������u�{P[d�=7P�U����j����4	���F5�q	`Ԯ8�qv���J�})Tj�s_�js�@U;�ﵼ��Jd.�G�N�?�BUt��:XƌpY0�eV����sǐD�Ojy��E���%;#ʼ�퉠�6��fI?0L˂���ccVd�I������]{Bj^�oU&�V+U�O8Y��o������~���F^;�7;"(�U��Ȇ����)o��/���Sl�"eP^�1>v�Vχ��=�d�$�5�3i�
b�J�s𻍙�p�-�@���M8	�tՉO�G�D!���p��q�xG���ixS��
���B;t�t��Kg�����ȑi���q?�G����[�|���� ����#��|��0�ڕ�?|U��Q�~ � 1��6��+u����'�X�� %e��F�ϧc||��>�IULe"vO�tH��ֻ� �M��qc��/��+����#.���h�fEz��BN��V׀�F�[��<��Unau�pd����D�V��P�&�����%LP�DTI�tn�[>�-]9��N�yH-H���l���YNT�p3&���L�tI^(�C�7�����MqP�Z��Y-��΁aS���yؙF���Q�>!�.ZP'!Y'Z���'��²3+����T�[�}�I�l��E�W+��E�u-��i�^�+gI�-�wF�XL(�����"�����߈����g�7�|�>Y�o2���1�-��ׅ9w4��ۭ$�����<�:�xX�s*��JQ�}�ʽ\'�>e�2��?��p��#@�L	o`�����68��wqm���㝙���V��[l�fY�MK��Rs�Ռ^���h�31�3�i�����k���QP;Dr�Y�n�%�#�V��X=D�k�p�C�>dH(0P�M�=v|���}�4���dkM��u>?�����1O!�!:;_�7�B� �<K�ث�wq$��֬�+Ѩ<�Ɠ�;�h�L�����]HW���h[��^����W~1_���3Ze`�7�% }�G��@H8b�H�!�
��T��>e��_��٢��W~'�/.�Q �>Y�Gq�T��e�룩X��B[ 3%\����C� �cI<�\�x�	b��U����jQ���3C������.�5f��9n�46Gk��M$���Z\X`����`��&1�;��"�������_�.c���w�`������n���\�V'�"t���v�0����ں�����yE]���X�ծ��H� m������z[Q��>�6?��`��T��k�׺r����n����逢涄���y�P��ҜI���q�Z�/���i�6"=��)���_��3�F�c�t=$�J�4{
�V�T,&���r�ȅ%풘�!p��x+�L���s�U���Β��t���x!��舳�|OK��{�^�!u��)��?>��A�.��`ȑI� �!Ze�.�xxX��W�NC�~�e5���"$4[J�QD��I���B���3^�S�7)�#�]sd���kec�nJ��y��ЙH�z&ҫ��W0�d�L����]�
la��h�n�4P�D�m5��$��-�C��ƛ
d �����	����(����,�Hg�b2�7�N��4�Τ �<�%�)+�vY	��[F'�Mij�l�tS"㻹�:���Pb5]�5|Z�LTNs���=r,<��[�n�y�z��X���;t���?WwB�#Б���c9Ut���?��w�1�ЀO���x��Dw�2��!�H��_e�ɨ��T,G�˘��w}���ҭ�Uَ��,�[�Ǖ�]o6H��i9�� �挠y�9t�`�x�":��ϑ\J:ښG=���|�yR�@�|gT��%	袛t�K_��eB�x���A������b6���#G�mV?|N͔l���CJ����45N�'Fq\�Z�����|��U��!ͣԅuaQ�'yFN�����;XVL:��U�U��Y�L"�uu=`S�_�z�[�H���0����(�7��8��{��9ܧ�{��XZ�@�0�����6L���a>ܫ������_Ս�NkS�I ���)s�q*n��f���J�h4>��f���F{��J�%�]�^c�@-0�!�>�}@�����ą��'y`�j��0͢X���S��%���6"��v�W�'g�b���^Ⱥ!�B̀�	�}���!��Y�����6��~9�#r�z��ßB����u��#��n%��2L����h,љ�S���
�e�oC,	�^�H(�T�K�u�U�k%��Ƽ�#|��YB�7pdtEI�rFֿS�3.����vq!�آ����tz����F�B���@|?�Oéh�q���s)�a��G]x6W�[���aȑ]�3�-���#ws@�q����N��Lf��Z��5!B̟���iH��75�6�ޑG��Y$Vר��OE���"h��>ڎ)+���C��|=���Eq�E�"�,9�
Z�t��ao��sm[�NfT5��F�M�[�Ôik��)c���E;}ͽ����K�[3�H8�a@@w!�q��E_ݫ��V]�T��`�P�6K����s�{^X� ,E��ίaP�q�gs��%�1zF,��%5q<����)�
?�����`"4�F��.��g�66	��9\ڌ��=����mI��
ȹ�?�ʥ�V�W�p�S��C.����Q!N�1��"Y��o_b[�?�#�pB)�~��|\�p�!�c�9���I�gx���ߟ�8fw��KX��L�G_]F��3*oz\��&$a�"k2~��-ޞ�J�NM��G#�j!s	��d�?ZO	���mf5��n�?Qi-�E�}9l�Q�Y�yЮ�jI�b�;*�!���Eג��F�y��cGoc�$ QF;��Zr��ra���_�CE�h�
�k[0��H2#�~��6��l���ۻu㥫V��b`F5�g߾."T̎꽰�R�t�Q9_v��m}���l���ڋ�3c�=v�#�Ɓ�`8��tzK�r�z��ʿ���0�����P#��82�8w�����l�BM��P��`	Z�߷�8��D��L�Xc�����1�U�.`)������\�hF<Y������o�d��ƝS��EtF`vw:����CVO����`b�]��{�.Q�l,g���S�P �8�*�c��J���z��,������V'I�����+��"�+]x�������d�'���kN1����g.�´��� �y���Mr;��t82RҢ�M1����q\��G�H�� %Ƿm�,C><te3�����oΡ}�3�M	f�o ��u�n�F�Ԥʄ������r�
~g����A[�Ěl�?ne��(^;5.7�Z/1��1hm>bl�uG��T�#���>D~h>Y��p��͑Dq��n�'j��o�<���VΞ�%�i)��[>W]|<-�]"M�����ů�eFg�����@�|_�L��G|N%���8H3�~�n�_���)<��Xj��~wj^I �m���:I+@�=7g�����wP3H��Ɩ�v~���L�T
��Z�	/��&����ڞ���d�`����6׉;�����$��cl(�J��i�9���P/�e5����]��#2�¡Ϟ�(^Rw^b�K$|==���D�������P�*�>f�r�"�Q[>�M�������S��G�~���։�(�#%8�����^��t0*�;��%��0�M����J$8��x@)�+3O?~1�dFߺ�_� ���"RL�5�2x#���n��w�9��`�(
+�l�u*�t@����%"���n�t�ۇ���E�C�'oR�w U䱖Z85sUB���9��ʻ�	,�1C{��ˈ������n����Agz�̕�aDb)���P1� 2����=������!�zb�UA�Y�	MkXI(L�N~��&��p1v�Jx���o���nc���2^��;�#�sJ~�c5���pk桓���Z�	z���c?jܹ�Z���	�*$��(����U5���q�o�ΰ�]��>��-?r��:�� �aA%��Zyݤ6#��@o�KrF1c�1Q���*b�ϠŻt.���u^�,����5��0��T;��G=�r��:ƅ~ޡ��L���U��ث+뛑w�<&�Y��Y"����P�)��*��G�ӛ aZa�}+D�u�!��F��p�$L\rm���O!�����>�<v]�ୟX�V��_�̷њ�㐌��Z���J�&5Po�AD_�g�s�2M���P�>��d)��%���W<�e� 	!n)�����vu�#9�H�ՏT��Vh�r	��.*�X�pn�-F�����t�u�u�u�3�v�Ż�dR�:-?	����śR�]$���+:���o1� WD�
c-�C.|y�7���"�����G�y���<_�Q�h��f�16���ep��.���=���؏䩠m �F����J�0��kL�ZPf:)9��#��=q [ּ��3�щ�DX�zJ��������\��c�)��~x���� �+[y�e��5��x�<nsK��$N��
<[S=ƀQޓR��D���Yl�X��#>���UdEK����J���V���E��&��>�H6Mh]��s	�����(��l7��%��4M�e֫�$D&�a�
�^=�K�	Fo�Y]K����N�:-����`n��w�Z�+���±������y����'�C��.��=�#�}�gJ�
^[g��v$w�ۉ�Ov�l�}�?��òb�#'�tbl�C�H�ʪ��J6�a�[���s"����J?�+�	r�Gw����\π2�
��?��e�jW��u��{<��"��|~�O+�
���7ղ!c&�N��� Ir�҇}�$��A��V�D8}:��T?1T�9��|�'i`p��~��@	��u4=<\����O�x'xeC���X��,&wF��ݛ<��0K�넾܆��<I$���|��j؄�A�[[�Ho�j!��I�
ɢ���뎁����h�`�>^1C��VӘh�a���*$-{〱���yi"�<ޙ�%D`���W^��Cѷ;�Y< �L�Ǩ㩲��rCJ��OQ~c�J�͉���Z[~
I�����c�QXg���w1�贘t�ѯ�xomA�sӀR����QƦ�t�r�γf���r|��mF���O�����>�"-y\m�v�f7I�E�e>��=gD/"�n�KSy�j�(�o|t�F�m�4��{�2����N�7ʓ��6� �,�F�)�B6N����\kx���@ֈ'{,YȌQ糏��0����j�5�mď�� �C�M�Bp�;������l`2;eCw�������ۇ��W����.��������Fh;�%���;,أQ�_��"���3C������Q����Q�L7��m��7�z��|W����hY�Ϫ)����(��Mh8
gB@;],_G@:+I��N�%��n�XTn��6��ُ9�(:�@�O�v��;K�27႐��lw�ŏ�]�1���э�} w݌��@ֱWGW%aN�p��l"�R�7V�{2;���;8p����h\*�m�� ���l� �q�͹W�hhr���&N�c�W�q߷�³�g$�%���*�(�O�{c��	�Z���6R��K�@VlC.�$w�߁���@�%D���� *��ǟ�鷴�C��dw�۹�v������K���2=,6�0%�TG��F�U[r-\@"��c��Q �+'ϟ`�i�'B �_ʙ�UM;��JD��_��� R��$�*A��KU�gXz�X�軒-r:�'�	�k����?����j���tN.w*�R��5?ώ�	Z�!��=�O7��$�V<V8�4�B�����t�R�x�/�m�\r=�9r,���A�lM}��E�H$��ve&���Kς�_9��c���5��%�[�fɩ,`suPͥӀTVB����mWK�����S��o�����H�j�a?�����v��c��K�f#/A���:C_ڿٞI�9wy!���(�S0��R=������V��BZ��A��1~37���9�9���m��vgw>�$��&�4��3��I�a:(� �1:�Rx<�;��;ELF                      �2     4     (            GNU  �       �          �����WV�ƉЉ�S���   �|$��  ��������   ����  	؉�  ���      � 1ۀ�_������	Ë��   ��   �؉�������E����   �� ��   ��  ��������   ��   ��[^_�f������S�Áü  ��Hd�    �D$D��������D$,=9� �L  =7� �   =Ո �6  =ӈ �   =Q� �   =O� �   =� �
  =� �   =Wu �   ��  =9K �   ��  =�� �   ��  =�� �   ��  =�N �   ��  =L �   ��  =o> �   ��  =e> �   �w  =�8 �   �f  =77 �   �U  =�6 �   �D  =�6 �   �3  =�6 �   �"  =�4 �   �  =�4 �   �   ='3 �   =�2 ��   =�2 �?   =�0 �   ��   =�, �   ��   =�, �   ��   =�, �   ��   =�, �   ��   =9+ �   w}=? �   wp=+ �   = w^= �W   = �   wF= �   w9=� �   w,=� �   w=a �   w=V  �����=�U  wPSh    ��������   �o   ��&    �v ������Q�UWVS����   �8�΅���   ���  ��w��ws[��^_]Ã���   ��,��   w��*u������   ���   ��  �
!ȉ��   ��    �X�A �������u��   ��  ��  	ŉ*[��^_]ú   ����`2 u���u@���p���������   뎍v ������u����P�����&    1�[��^_]Í�&    �h��t���(����C����P���������[^_]ø�����2�����&    �����S�Ë��  ���	  ww��	�#  �   ���'  �Q  ��t�  ����  �   �غ `  ������ `  ��������d   ������غ @  ������d   [�������&    ��)�  ��'�  ��!��  �G  ��&�r  ��'��  ���   (  ��������     ���   ��   ��*  �'  ��������     ���  ��   ��*  �'  ��������     ���������   ��*  �'  ��������     ���  ��   ��*  �'  ��������     ���������   ��*  �'  ��������     ���  ��   ��*  �'  ��������     ���  ��   ��*  �'  ��������     �����������     ���  ����     �����������   ��  ��    ��<   [�������&    f���vP����   �   �   ��������   ��������d   ������   ��������d   [�������$�[  �   �غ`  ������غ`  ������   ��������   ������غ   [������v ���/  v]��uT���     ���   ��   ��������     ������   ��������   ��  ��    �[�f���t���u��   ��  ��    �[Ív ���  �  ���h�����u����   �6   ��(  �   ���   ��$  ���      ������   ��������      �������   ��������   ��   ��   �����   [�������&    ����  ��   ��t	���������     ���   ��   ��������     ������   ��������   ��  ��    �[Ã�
��������     ���   ��   ��������     ������   ��������   ��  ��    �[Í�    ���a������n������A����   �غ �  ������ �  ��������d   ������ �  ��������d   ������   �غ   ������   ��������   ������   ��������   ������غ   ������   [������t& ��1��  ��   ��5w\��3wc���   �7   ��(  �   �غ  ������  ��������d   ������غ  [�����f���%������>���f���8���0����   �   ��������   ��������d   ������   �=�����-�c�����   ��1��������     ���  ����     ���������d   ��������     ���  ����     ���������7   ���   ��(  �   ���   ��$  ���   � �  ��   1ҋ��   ��  [Ã�*�������,�|���[Í�    ���     ���   ����     ���������d   ��������     �����7   ���   ��(  � P  ���   ��$  �   ���   �X�����&    ��   �غ   ������   ��������d   ������غ   [������t& ���     ���   ����     ������   ������b���f��   �   ��������   ��������d   ������   �I�����&    ���     ���   ����     ���������d   ��������     ���  �-�����&    �t& ������UWVS�Ã�0���  d�    �D$,1����w  vE����w�   �� �ud���  ��t71��D$,d+    ��  ��0��[^_]Ít& �����   1���vЃ�uh    1�j�����XZ빍�&    f���u��5   ��t��   ����������  ���?  ���  ���w����Pd����j����Z���_����    �   ����  �r����  �   �����1��2���f��   ��� ����   ����������  ����  ���  ��������Pd���������Z��������    �    ���x  �r���m  �   �����1������v ��������ƅ�������   ����������  ���  ���}����Pd����p����Z���e����    �   ����  �z����  �   ������:�����&    �v ��������ƅ������   ����������  �����  ��tA�Pd���t8�z��t1�    �    ���@  �j���5  �   ��������  �v ��������Ad�@����   �x����   �   �����   �h����   ����������  �D$ �D$
 �D$���_����Ad�@��tw�x��tp�   �����   �h����   ����������  �D$
�������Ad�@�������X�������   ���tN�x��tG�������������t& ��D$
뽍�&    f��D$ �D$
 �D$�[�������������  �2���������������   ��������  ���������������  �N����   ������h����   1�������W����   ������F���������>����������&    f���������  ��3wpS�Ã�v���   �� �u�u3[Ð��t
��t&��u�غ   �����ǃ�      [Í�&    f��غ   ������������ǃ�      [ÐÍ�&    ��&    ���������  ��3w��v���   ���� �u��u,Ív ��t
��t��u틐�  ��t�   ������t& ����  ��tʺ   ������t& �����WVS�Ã���`  d�    �D$1����9  ���  ��	t��  ����  �H����1  ��&��  ��2��  ��  ��5�f  ��  ��;��  ��  �Hă��\  ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ��"��  �J��   ���X���  j ��
  �9   �CjD��������  ZY����  ���   ���   ���  ǀ�      ���  ����  �Pd�����  �R����  �    ���|  �I���q  ��������  �o  ���t3��   �H����X  ǃ     ǃ      ǃ$     �v ǃ�     ǃ�     ǃ�     ��"��  ��������B�����������"  ���  �D$d+    �3  ��[^_�f��Hʃ��%����P   ���  �   �    f���  �������`  ���
  ���  ���������  ǃ�    ���   �   ǃ�     ǃ�     ǃ�      ǃ�  �G ��� ǃ     ���  ǃ      ǃ$     ������t& �������v ���  ��6��   j ���   �C�A   jA��������  _X��������D$d+    �  ����[^_������Hȃ��J����  f���3��   ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     �B܃��M������}���>�����6�?�����7�U���j ���   �s�A   j@������������A   ��j jA��������  �������t& ��4��  ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     �K�����    ��)wK��'�  ǃ�     ǃ�     ǃ�     ǃ    ǃ      ǃ$     �N������-�����v,��2�;  ǃ�     ǃ�     ǃ�     �������,������
  �v ����   ����  ��������f����	  ����   ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ������&    �P   �    ��l  �   f��n  ��������  ����   �H܃�������   ����ƅ ��H  ��   �m�����&������H�   ��������������������&��  ǃ     ��ǃ      ǃ$     ������t& ���r���v�H�������뾍�&    �v���믅�t����   ����Ɔ<��   ��`  u]���������`  ���u����=�   �tK��   ����������  �������t& ���m����5   ���O����1����t& ��`  ���������   �������|������`  ��t������ǃ�     ǃ�     ǃ�     ������t& �ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     ������&    ǃ�     ���   ǃ�     ǃ�       ǃ     ǃ      ǃ$     ���  ��   �'  ��  ��������     ������   �'  ��  1�������v ���     ������   �'  ��  ��������   ��  �:����������    ���������ȃ���!�	ȃ���   �'  ��  ���������Hu����     �������  �O�����&    f�ǃ�     ǃ�     ǃ�     ǃ     ǃ      ǃ$     �5�����&    ǃ�     ǃ�     ǃ�     ������`   �{����D����؉\$�D$H   �D$    ��������   �    �������F  ���6  �1  ������������P(�   ���  ���  ���  �B�T$���1�Í�&    ��&    ������UWVS�Ã�� ���  �p(���  �����  �B���  �   p���  ���  ���B���  �@    ����������  ���  ��x  9�tS���  �M    ���  ���  ��x  ��|  �E �E���  �=x  �H��  ����[^_]������t& ���  ���  ��x  ��|  �E �=x  ��  ����[^_]�����f������UWVS�h(���   @  �������  ���  ��������Ƌ��  9�t@�v ���  �P�-x  �Q�
�   ǀx     ǀ|  "  ��������  9�u�[���^_]�������    �����VS�������(  �F��9�t$�X���Cd� ��t���t��������CL�X�9�u�1�[^Í�&    �t& �����WV��S�������(  ���  ��4t��8��wP�F   ���  ���F��X�9�t(�t& �Cd�@��t�H��t	���������CL�X�9�u�1�[^_Í�    ���  u�������捴&    ������WV��S�������(  ���  ��4t��8��wH���uK�G���X�9�t)�t& ��Cd�@��t�H��t	���������CL�X�9�u�1�[^_Í�    ���  u�������������WV��S�������(  ���  ��4t��8��wX���u[�F�   ��  ���������X�9�t(�t& �Cd�@��t�H��t	���������CL�X�9�u�1�[^_Í�    ���  u�������������S��������;��(  uH�C����w=�C    �C    �C�  ��x  % �  �����`�  �C1�[Í�&    f������[Í�&    f������V��S�\$�������(  ��x  %�� ��u(��҃�Ճ�6������σ�;�C1�[^Í�&    �������&    f������VS���������(  �x  ���  ��x  ��|  �1��S[^Í�&    f������VS���������(  ���  �C���  �K���  �s���  �6�s���  �B�C   ���ȉC1��K[^Ít& ��������u��I  �A,1�Í�&    �v �����Í�&    �v ������ �@(�������&    ��&    ������U1�WVS�Ã�� �p(��������k�ǋ��  �@���  �����  ����   ���  ����  ���  ����   wT���+  ����  ���   ���  ���  j P��j j����������=x  �6  1���[^_]Í�    ���  ���J  ���  ���   ���  ��Q��j P��j Q��������륍�&    �v ���  9ST�  �SP���  ���@����x  ��x   �  ti��    ��  ����   ���p  �$    �����   ���  �T$���  ��R��QQP�t$��D$��������	�����    ���   ���  ���  j Pj�j �����������������    ���  ���   ���  ��Q��j PQ�ɍ�&    �v �������[^_]Ív ����  �$��1��I�����������������VS��������5�I  ��(  9s��   ��x  �S% �  �����`�  ��t^��wy����   �   j �s��  j P�C�S�0   j P��j������C��I  �C   �����C�C�C1�[^Ð���   ��9Kw��   ��랍�&    f��J����   C�노����[^Í�&    �������j����t& ������U��WV��S�Ã�pd�    �D$l��������   ��(  �|$�D$   �$1��=x  ���  ���������D$�����U  �<$��   ���]  ��`  ���O  ���  ���A  ��I  9E��I  E��=x  ���  �E���  �U���  �M���  ��  �E�]0�D$$   �<$�u�M�U�D$(�D$�E(���t$ f�D$0f�D$�E,�L$,f�D$2f�D$��f�\$4��T$�\$��L;|$tY�t& �Cd�@��t�p��t�L$1҉�������CL�X�9�u؋D$(�T$�t$ �L$,�D$�D$0f�D$�D$2f�D$�D$4�U�T$�E0�$�U�T$�u�U(�T$�M�U,���  �E�D$ld+    u�D$��p[^_]��D$�������������&    �t& ������UWVS���������  �   ��(  ��������    i��    ��    ������������  �k���� t���� ��  ������������   ���   ��u���   P�C0h�  P������CT ����  ����4t��8��w�CT �1�[^_]Ít& ����  u�[1�^_]������W��V��S��@�P�V������������������������=x  �  ��������C�B���   �   �C"  [^_�������&    f������WVS�áx  ���+  ���wr�s�{1��C    ������F,    )���0���<��   �    �������������Q  �� t���� �G  ����������C$   1�[^_Í�&    �������퍴&    f���������T����t& �����S���������(  k�l  i��    ��,   �   ����v��p  ��x  ���[  ��[�������&    �t& �����U��WV��S�Ã��x  ���x  �������t]�ǹ�   �C󥉂�  ���  ǂ      ��0  i��    U��    ���  h�  j �T$P������T$����(  ����[^_]Í�&    ��&    ������V�x  S�X(���  ����  ���   @  ������d  �N�v����������   �   ��0  ǃ       �غ   ��������      �������      ��� �