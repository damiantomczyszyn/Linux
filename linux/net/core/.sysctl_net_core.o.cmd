r_completion(&pending->done);

	if (refcount_dec_and_test(&pending->refs))
		wake_up_var(&pending->refs); /* No UaF, just an address */

	/*
	 * Block the original owner of &pending until all subsequent callers
	 * have seen the completion and decremented the refcount
	 */
	wait_var_event(&my_pending.refs, !refcount_read(&my_pending.refs));

	/* ARGH */
	WARN_ON_ONCE(my_pending.stop_pending);

	return 0;
}

/*
 * Called with both p->pi_lock and rq->lock held; drops both before returning.
 */
static int __set_cpus_allowed_ptr_locked(struct task_struct *p,
					 const struct cpumask *new_mask,
					 u32 flags,
					 struct rq *rq,
					 struct rq_flags *rf)
	__releases(rq->lock)
	__releases(p->pi_lock)
{
	const struct cpumask *cpu_allowed_mask = task_cpu_possible_mask(p);
	const struct cpumask *cpu_valid_mask = cpu_active_mask;
	bool kthread = p->flags & PF_KTHREAD;
	struct cpumask *user_mask = NULL;
	unsigned int dest_cpu;
	int ret = 0;

	update_rq_clock(rq);

	if (kthread || is_migration_disabled(p)) {
		/*
		 * Kernel threads are allowed on online && !active CPUs,
		 * however, during cpu-hot-unplug, even these might get pushed
		 * away if not KTHREAD_IS_PER_CPU.
		 *
		 * Specifically, migration_disabled() tasks must not fail the
		 * cpumask_any_and_distribute() pick below, esp. so on
		 * SCA_MIGRATE_ENABLE, otherwise we'll not call
		 * set_cpus_allowed_common() and actually reset p->cpus_ptr.
		 */
		cpu_valid_mask = cpu_online_mask;
	}

	if (!kthread && !cpumask_subset(new_mask, cpu_allowed_mask)) {
		ret = -EINVAL;
		goto out;
	}

	/*
	 * Must re-check here, to close a race against __kthread_bind(),
	 * sched_setaffinity() is not guaranteed to observe the flag.
	 */
	if ((flags & SCA_CHECK) && (p->flags & PF_NO_SETAFFINITY)) {
		ret = -EINVAL;
		goto out;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		if (cpumask_equal(&p->cpus_mask, new_mask))
			goto out;

		if (WARN_ON_ONCE(p == current &&
				 is_migration_disabled(p) &&
				 !cpumask_test_cpu(task_cpu(p), new_mask))) {
			ret = -EBUSY;
			goto out;
		}
	}

	/*
	 * Picking a ~random cpu helps in cases where we are changing affinity
	 * for groups of tasks (ie. cpuset), so that load balancing is not
	 * immediately required to distribute the tasks within their new mask.
	 */
	dest_cpu = cpumask_any_and_distribute(cpu_valid_mask, new_mask);
	if (dest_cpu >= nr_cpu_ids) {
		ret = -EINVAL;
		goto out;
	}

	__do_set_cpus_allowed(p, new_mask, flags);

	if (flags & SCA_USER)
		user_mask = clear_user_cpus_ptr(p);

	ret = affine_move_task(rq, p, rf, dest_cpu, flags);

	kfree(user_mask);

	return ret;

out:
	task_rq_unlock(rq, p, rf);

	return ret;
}

/*
 * Change a given task's CPU affinity. Migrate the thread to a
 * proper CPU and schedule it away if the CPU it's executing on
 * is removed from the allowed bitmask.
 *
 * NOTE: the caller must have a valid reference to the task, the
 * task must not exit() & deallocate itself prematurely. The
 * call is not atomic; no spinlocks may be held.
 */
static int __set_cpus_allowed_ptr(struct task_struct *p,
				  const struct cpumask *new_mask, u32 flags)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(p, &rf);
	return __set_cpus_allowed_ptr_locked(p, new_mask, flags, rq, &rf);
}

int set_cpus_allowed_ptr(struct task_struct *p, const struct cpumask *new_mask)
{
	return __set_cpus_allowed_ptr(p, new_mask, 0);
}
EXPORT_SYMBOL_GPL(set_cpus_allowed_ptr);

/*
 * Change a given task's CPU affinity to the intersection of its current
 * affinity mask and @subset_mask, writing the resulting mask to @new_mask
 * and pointing @p->user_cpus_ptr to a copy of the old mask.
 * If the resulting mask is empty, leave the affinity unchanged and return
 * -EINVAL.
 */
static int restrict_cpus_allowed_ptr(struct task_struct *p,
				     struct cpumask *new_mask,
				     const struct cpumask *subset_mask)
{
	struct cpumask *user_mask = NULL;
	struct rq_flags rf;
	struct rq *rq;
	int err;

	if (!p->user_cpus_ptr) {
		user_mask = kmalloc(cpumask_size(), GFP_KERNEL);
		if (!user_mask)
			return -ENOMEM;
	}

	rq = task_rq_lock(p, &rf);

	/*
	 * Forcefully restricting the affinity of a deadline task is
	 * likely to cause problems, so fail and noisily override the
	 * mask entirely.
	 */
	if (task_has_dl_policy(p) && dl_bandwidth_enabled()) {
		err = -EPERM;
		goto err_unlock;
	}

	if (!cpumask_and(new_mask, &p->cpus_mask, subset_mask)) {
		err = -EINVAL;
		goto err_unlock;
	}

	/*
	 * We're about to butcher the task affinity, so keep track of what
	 * the user asked for in case we're able to restore it later on.
	 */
	if (user_mask) {
		cpumask_copy(user_mask, p->cpus_ptr);
		p->user_cpus_ptr = user_mask;
	}

	return __set_cpus_allowed_ptr_locked(p, new_mask, 0, rq, &rf);

err_unlock:
	task_rq_unlock(rq, p, &rf);
	kfree(user_mask);
	return err;
}

/*
 * Restrict the CPU affinity of task @p so that it is a subset of
 * task_cpu_possible_mask() and point @p->user_cpu_ptr to a copy of the
 * old affinity mask. If the resulting mask is empty, we warn and walk
 * up the cpuset hierarchy until we find a suitable mask.
 */
void force_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	cpumask_var_t new_mask;
	const struct cpumask *override_mask = task_cpu_possible_mask(p);

	alloc_cpumask_var(&new_mask, GFP_KERNEL);

	/*
	 * __migrate_task() can fail silently in the face of concurrent
	 * offlining of the chosen destination CPU, so take the hotplug
	 * lock to ensure that the migration succeeds.
	 */
	cpus_read_lock();
	if (!cpumask_available(new_mask))
		goto out_set_mask;

	if (!restrict_cpus_allowed_ptr(p, new_mask, override_mask))
		goto out_free_mask;

	/*
	 * We failed to find a valid subset of the affinity mask for the
	 * task, so override it based on its cpuset hierarchy.
	 */
	cpuset_cpus_allowed(p, new_mask);
	override_mask = new_mask;

out_set_mask:
	if (printk_ratelimit()) {
		printk_deferred("Overriding affinity for process %d (%s) to CPUs %*pbl\n",
				task_pid_nr(p), p->comm,
				cpumask_pr_args(override_mask));
	}

	WARN_ON(set_cpus_allowed_ptr(p, override_mask));
out_free_mask:
	cpus_read_unlock();
	free_cpumask_var(new_mask);
}

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask);

/*
 * Restore the affinity of a task @p which was previously restricted by a
 * call to force_compatible_cpus_allowed_ptr(). This will clear (and free)
 * @p->user_cpus_ptr.
 *
 * It is the caller's responsibility to serialise this with any calls to
 * force_compatible_cpus_allowed_ptr(@p).
 */
void relax_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = p->user_cpus_ptr;
	unsigned long flags;

	/*
	 * Try to restore the old affinity mask. If this fails, then
	 * we free the mask explicitly to avoid it being inherited across
	 * a subsequent fork().
	 */
	if (!user_mask || !__sched_setaffinity(p, user_mask))
		return;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	user_mask = clear_user_cpus_ptr(p);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

	kfree(user_mask);
}

void set_task_cpu(struct task_struct *p, unsigned int new_cpu)
{
#ifdef CONFIG_SCHED_DEBUG
	unsigned int state = READ_ONCE(p->__state);

	/*
	 * We should never call set_task_cpu() on a blocked task,
	 * ttwu() will sort out the placement.
	 */
	WARN_ON_ONCE(state != TASK_RUNNING && state != TASK_WAKING && !p->on_rq);

	/*
	 * Migrating fair class task must have p->on_rq = TASK_ON_RQ_MIGRATING,
	 * because schedstat_wait_{start,end} rebase migrating task's wait_start
	 * time relying on p->on_rq.
	 */
	WARN_ON_ONCE(state == TASK_RUNNING &&
		     p->sched_class == &fair_sched_class &&
		     (p->on_rq && !task_on_rq_migrating(p)));

#ifdef CONFIG_LOCKDEP
	/*
	 * The caller should hold either p->pi_lock or rq->lock, when changing
	 * a task's CPU. ->pi_lock for waking tasks, rq->lock for runnable tasks.
	 *
	 * sched_move_task() holds both and thus holding either pins the cgroup,
	 * see task_group().
	 *
	 * Furthermore, all task_rq users should acquire both locks, see
	 * task_rq_lock().
	 */
	WARN_ON_ONCE(debug_locks && !(lockdep_is_held(&p->pi_lock) ||
				      lockdep_is_held(__rq_lockp(task_rq(p)))));
#endif
	/*
	 * Clearly, migrati�����>�д_҈�4��Ŝgl�T�؜h\
�w����%&|����Z�YE�,���Q�m�'%-KhЪ�zP��7�L[�lQ��^{r��P^���,��ޯ{ʱ��ث��x�hϸgA%�=>��p�G��hk��l���U�O�-���S%��J�T�F�3�n���ʮL�d՚`l�B�|j������Hv��/�^7���v�v���s����A���B;N��#v�ޣ�t�t֐��u� G���Q,YR)߳��%�+��k�0dA})$�lo�]�Sl�I�'+ �Tdw�{�VH���i֢�ˇ��d�A��83�XRE�hB���;��cb�.^��?|h0u�P���@���a��1]N45ഈFk�b_<u� 11�Htݗ.��oL���E���� �([����f܀���[���]I4�,J�8oc �MBc���/<��;Mo�hw ��6�i޻J/��	���x��{�'}��{S�%��9ܯ'v��t�* I���*ˁ7��5�){��X����i���,q��z���4�5��L�ǻ�Z*����]����@jXF�.R�:����}�g�բ�h����22�T�N��rLa���q�%[���@��������s������O W�2j�l��>��
����|K�������)�v�'E�\�#(�(�9���WPmEu�"v�,S��5�\��BZ�6Y��%-l�
z�^�YӎD��e��,S"� �u�Yϣa5�ڪ]u|�z��	�L��q^(�AP�$�N�M��Ԧ��XxH�*�?��X/-�������`�2&�:h)�l�D�@B����k���2��R[,.�����p������A^��%�ݻ��Esހ���T�Sp߾q�\D}��s�SA��50�ke?�q�<��N0M�N�x�
��,S~�*����eO������n�}�ʙ�ŋ~w8�:�|�כgс�%�2��8e�k����/�^�� i�*z�~L�S�4\�Wř�W�U�P<�c��w�������Nꊯ;�ڽHLA��։�"�^V���s�k�K�W~X�f��T�'>誘V�o�'<>`-��4X�T;p`���#+�a�$�o ��+�%4�S�S��_q���z�$.{�
.�lQa�S.,�Ǝ.Sq]i\�����OX=x�����>�h��~x����q2b?p{w*�A4�0J,c}.2�.�M�\�A��T2� �9nh�����z����r�Rl��\���#��������JM9)�C�J�-����# �O2�H;�OH�,аo9�g!ϻ��O_d�24��4-u+��U��Cq�:U�5LdH�!��	L��j`� 9����e��'ߤ�n/ucU6ÓR�oN~�
�S�jĆ���t�۵9�2�+^ꥄZ�Au{�:�b�ڻaP���Ċ����eӔ�a3��9�b�T���_7:f�H�~�4����+�aB�xhν��S�����L�Px�X�b�������!�%��6S͖��f�xʔx>6R�<��s~�е���b<ue�0��Vi4��%�=��3�1��/��ЁtF��a��`ݭ+���n�g�H2�G C�+(�v�I��5/������6gg�Y�=�w�WE�C��`���4s6�ڷ�������krf����շ�{p/ �	S�no�h���ѡ�z�x&�*x����cgқ�G�8��9#�J�H����f`c̰ޯ���͢�G5�%*5���=x�ȶ��w/xz�\#��V�Ԇ߳Q�M�n�o�D����]x��[%uL�8�m��������#�D�{� �Oft�ի�D�qy�g2��T��/٦1�H䆖-Ɗ��/��BԄ�ӥ����F$�XA���dJPď)����7��b'��WU!�.�g�)}��ӽ��ڑ{��?��[A���]oE6��(�ELv
�!�n�����Ô�{��j����}�[����>L3#��x��a���vt�k�>8�!��ah2��s��,����p�C_���GƎ2>!:�YO_�2��.����5w�PO�Zlc5��/Om���~��a�xO����N��[%Ac"I,�@ܲ�����W����(�{l� H?@�B/zX+;N3�mw�@=ʲ��$t� }-���N��ｆ�HlP�#͆�^,!������V�N/����Y�r�<�u���7G���u)rS#��8�S65gR��Wc_T�2�r�.؄/��^U}�2dǦJ�[�ގ���H	>ʦL}�:�����8Ն�%��
E�4j�ɴzA�+Й�&}U>մ~�3w�g���o��}���|�8�c
ޜ8�NA���&j˵�t}@���M�0;�ɂ��(���s��	�T�7]�&6kI���	[R�(�����&|s�Z�lV��d,�e������v���cI@�*���CG 9���H7�m���!)����voY�iλ_n��_NCh��1c-Q"G+�8�4Hg��pfV�K����h�1��l3�J~Y؝��Eݍ��7u#��uX����նW�mѸe�6�R_��$�����%��Nv+޼���+��/s1&*k����W2
$��S�Tm�!��<vI'2��ʝ[5���ʺ�6�Y��u��Qy,�7��*�D���G᥷�:��:5XCןS�x�Zde��
�a��"73�W
�t�p
&m�����4R؁�>�e<����L�D����ޔ��=����
�x$�.jIxRUYT{B�Cz;��}
&��b`��"gJi���>]?v������)� ~tXR"z�����5'�%��1���{��r��u�:[�R
��#Þ솜2w=mÇ܀`ygH�x�hf��@��ea�ڗ)�{]"�t��J��-���尒��E���6��)
M��k�Qq*>�G���k�NF��M����bh�7N{�~�W8�Ӕ�8���R�_�o�n����U���g�B:{��l*����M��z�{�n���{`ўKi��]P+����[&�Cn�kNc�D>�;�A^*h��昨�MȔMA��
fQg��v�h�Ж���0s;[I�;[f�ބ���.$��&AR���t�-[�R�yִP��ܾ��6(�d(�3ϒ�yH6H,V�����M%MW��)���C�MX₆n7��5��vs�iaG}�-Wę���[|��!��� d}�)��d��o��g������[[!f/�g�]�:'&S���T%��9�}r�Y�2��T1�B�iSތ(\��η�zg���M�[�)8�C�� MnN��R�W�H�	��������p�U-�ǫY^*+����	�{��8�)����p�L��y6T��ŗփ�],m2E��FS�k�
�m����H�J�o�2h
Da_f�������\WOd+�;[�1�7]���F������"H�/P�Wwj�!�W���*0��h��(W��H�pZ�~.���y>�A޺mO�#�7�=i����*=ځFF�(#�湜DVcs�]��a7	�
`k�L���a��� ,�}��T��U�ڮrWb7,��p6��.8���N]ǋ;(�'�{J��(g�4��B7#�*(�^�Xb#2B����㼯���+��g�[U;���c����po8�Wz��v�N]��qY�d¥�6m�{�aIy��L�u,fvJݫ(�\�ƚ�Wk;4��r��$G;�K����|
k$౜�H��e���8�qg���9Ƶ�3��m��*E��A��X��=Ů�]�7�$���D���8	]����7��.b��Ή��l���l����J

P r��]�G����1�й��p��yY=XY@V2:<��3�*��Z�K%�x6CFO&���\(�xK��K�Í�ņ��ܒ��������� |	����y��~�婷����Qz��k��BY��Y�����h舘�&%Sh"�
h4��N#0N9g�j�ǭ'o���Gf�f����f�j`Kh�5�**��|h�ٚ�j61���sVτ�@�_k��!`0.��Q�-�@�)#�d�-<����Ĝ=&����iи�:v������1�%�(�'���x���f��a��So:�Į�
���!�2�����~Pm1�����[E!C���<3�?E��c�$#��bfeGg������H�|�����.�rU9���ｳZ���b��4�q�+�7��ͳpQS,���������~�����g�U�YR��%��k#����!RI(V?�=rO<XXTZ�
^�`c&h(�Ͱ�RzZ�v�A0e�ݑ�g��8?��1�C3V�o�,! �H4��^ha �:r�#�k��d�^��U�2L��[����w#h�?�(�{�$9(�hK�pdBŁ�G�6�*	�1��5��٫F(;�B2²5�Ů+c��X�/315�*y��!�z�3Z0��8xR/���y
h�O���k)��l<�EǤo@#vW�im%��q3KLR�|_vV3j(�������XF�{�L�V؞�k�������F�!gheKIx�<<{���ǞP�Z��YS�/Xǝ�A>�T����������W����-�}ɛ�qlI�����N�-LE��HW_�I��u�I��|�b�WL44�HV
$�U����Q���t�߇X�i���#��2{���gc.�=�I���g���)g�\��
r�㏦t�7 S�Mw�4�mR���b�t3ǅR�j�._+6��KOM�y�c@���s0V��:>� �ոᢥ��Q� �D��'3<� |? Ϗ�_?����M���B��]L�r&��i��Ynq�<Ue��h}��裼��#`�>�EuF��+Pf,��ߔKu�8��Hc��Ԯq�^@	Y/���q��$�S���x���S�p
��T� ���e�[l��G0c�Qpn�Wfk�m[��=1˅Gm&z���r���_�c� �!��������<�8]��=��y����d=n���R���7�qC�6�S�~�b��W�t�C��2	��u�4�ն\2"2�r���D����=xl�mu[���@�ݬ8ʴ�4&c
Yt�5�SȎ�Dġ�&@�n��J��?nk��m�[�U��V�
%{�ޞ�K{ꯙ��x�U����T�X �Dm�Ý��k�&�@�;?����p�Es��P�6~M}M� Fڞ���C�B�/AH�2��d�%�XS6pUUﶬ�D�#>�m)�f����fV��P�6R�p���t��
��? ,E�������Kߍ�m�1۟��7k�Z��Sv��x�B��	y�*4h��HR]�y1U�Uc36ӀS���IP�:�{�[���_��9̐�iq�ߓr9Jp,�D��g�#ՑvM�w�8�d7h��W����o�'���L�������ڐ܊ٜ���=o�I����]N��]#`1�W���E�|�ɒm��}�
��,N�u����T��e����ώ�i�`E���_��f�wp��p)X�h��SZ������֟�?��A�Ȱ0E�g0\��� �՟�h�T��<T�W��PZ �u�.'4d�Q�{�ق"̓Ml冡^�-`�//���M�����mJs��\���u�Q�W�����>��x%ta{��D�PQ��Ĳ�����'��&N�#]� 2Jz�TH���e�J�<��tB�S�aky -Sv�CsӴ�b!�'��n�v�Q�CR�.� [����X/�=��13y�bN䘚��l�����8	�g����C�
��H�m5���*���>�SV���ȸ6�Q���V�L|�2�~��p�����KD1x���,68�Zf�72 }2�T��s_����vJ�P��4�"�k6��,6�drop˴��D�7B4�\�Ɂݸ bn'���!��&���Ag�kH��*xa]51U�8�܋�ʝ���R�Ĭ"�фG�֗k@c=fL��\J��w8��֏��n /N���L��r��?��Y%����R"��։үI�I���R�-���L#���ԇ��O�@�;]���(YUܵ��Y��ɔ2��a��0�c��\rD�U����ھ��{��3���%3�ݶ$Q�m���S����5�i�ș�(��)����8��Ib�g,�4#��)�}h�1�B����C����.��?������hƒ�m�َ�D�r���
)�s��SF?��1�g�Ai�R>�T�,�ۺBxy��t�L��1��� /��.��+���_T޲�w2"����wg+�Bjځ��lj���^%����Z6$���E}�+�F�ٺ�a+����hG/!�aI �I엮��tX��}n�l  pa��:�J�F/���H.�l��@�a˟���i�e�dk�
qO����G�'&����(�@�=�QP׬A�������xf��ΰa���/2X�;�FHӁ8&�y��{,�C� ��)�܍+�'��E.i�¾���T�)9����x��?F#*L�#�\1���	�^Ҩہ� �_甓�0��DՒ���_G�u�+�`�͉a��(1�j���)AY�XS���|�R�	��6R�aG7�*?�2�s��Y�T��nw����gv��������>�I]��(0{��>����Ve��!J����D�I�ʽ��t&�at��7dr�~��e��ڭ���~4�����xY0,tT¬�T�;:���>�!��*��� ���I����-4j)r��}��{�4#��Bڵv<�s�jز���� �
T�t'"�n=ObȜ�Ar��-xٸ�h��ӭ�-QVH�W[qz�P����g���[챥��5;?͂�h�j��:%~�$3�)UHt���S�h�(���7�=m-G�Y�ֲ4�9���$1� &����yFIx}��O<+��>	�u�(v����cvG.�*���HV䋢��h�9&����!@v\�Tai?�����pkv��v,A�xFS��%ڦ�O
�Yw�n [���Q��{��v˴g=���O1R��6Gnc�	��pCx���{�𛱁���Ub)t�N8�͈�go�@��#��h]��=Ѣ���CC�E	���zWa�7�*�T���3�_�Z6.���g�Y=
����<T?�&�t�vx幖e!k�	��4I�ї�A�e2�!�M�Pg���ք�z��l>�J�/�^�_�+J�k�!���Z:�Җ�ۤ�́�g/�q	
Hk־�F� ����;b����w�O���:!p�Dι=x��nc7����!��y�f.��*ɚrs�-��r��	�ճq����F�.�u�vpz|�p>c�x���Dҽ/WdG�O��Ϯg`A�0��}@P�s2w$<�|��@J��t�p��O� x+0�������L�>&  s���3��J�gw�\���N+�v�ͅ�Lw��޹��^�[h��EF�@S'�"�H��ݯ�e_5�`^���S9Y�����):؃q�O#��6��Y���	��y���q��o��(�3T�C_\:Z��ޭE����y�Z���ѯ�`R�5��W/���J&G(��
��JC)��:TIN���ЀW��H�����@ùM��N5B,q�M���8�����0��?�VV���8�2�	r�m��ƃ^k��� �J6��F��$�qt������>K,?I+�����ԉ��:�V>N7r�j ҙ݈�Hk䎯tV5'��F��.� �[�1'�������r�����Q�v��p�g=+ْ�VD���O�*�U���!/�>b2�82��oA<�h#���WX�PL��O�A%�]=��Z�0+NVx�ΨE��Gu�A�N�u�eY�Q�Ы�]���ߘ��ڃ��Aa�N�n�5Ŧ��&��>�q�����:�&���^-sQ�U`D�j�/P��T���ݙ��A*���o�������/����B�F�o%�O�@��z��b܅0 9�We���7�;B����.*-3�%
砰�������(�|
�G�{5��ߣ�g�Λ�.F*,�֘)7_=?����	K��"�Ѿ;G�F�[^-o��t� f��S�U`�drM�9uK�oЈ��(	O/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/pci.h>
#include <linux/i2c.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ctrls.h>
#include <media/tuner.h>
#include <media/tveeprom.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dvb.h>
#include <media/rc-core.h>

#include "cx23885-reg.h"
#include "media/drv-intf/cx2341x.h"

#include <linux/mutex.h>

#define CX23885_VERSION "0.0.4"

#define UNSET (-1U)

#define CX23885_MAXBOARDS 8

/* Max number of inputs by card */
#define MAX_CX23885_INPUT 8
#define INPUT(nr) (&cx23885_boards[dev->board].input[nr])

#define BUFFER_TIMEOUT     (HZ)  /* 0.5 seconds */

#define CX23885_BOARD_NOAUTO               UNSET
#define CX23885_BOARD_UNKNOWN                  0
#define CX23885_BOARD_HAUPPAUGE_HVR1800lp      1
#define CX23885_BOARD_HAUPPAUGE_HVR1800        2
#define CX23885_BOARD_HAUPPAUGE_HVR1250        3
#define CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP   4
#define CX23885_BOARD_HAUPPAUGE_HVR1500Q       5
#define CX23885_BOARD_HAUPPAUGE_HVR1500        6
#define CX23885_BOARD_HAUPPAUGE_HVR1200        7
#define CX23885_BOARD_HAUPPAUGE_HVR1700        8
#define CX23885_BOARD_HAUPPAUGE_HVR1400        9
#define CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP 10
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP 11
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H 12
#define CX23885_BOARD_COMPRO_VIDEOMATE_E650F   13
#define CX23885_BOARD_TBS_6920                 14
#define CX23885_BOARD_TEVII_S470               15
#define CX23885_BOARD_DVBWORLD_2005            16
#define CX23885_BOARD_NETUP_DUAL_DVBS2_CI      17
#define CX23885_BOARD_HAUPPAUGE_HVR1270        18
#define CX23885_BOARD_HAUPPAUGE_HVR1275        19
#define CX23885_BOARD_HAUPPAUGE_HVR1255        20
#define CX23885_BOARD_HAUPPAUGE_HVR1210        21
#define CX23885_BOARD_MYGICA_X8506             22
#define CX23885_BOARD_MAGICPRO_PROHDTVE2       23
#define CX23885_BOARD_HAUPPAUGE_HVR1850        24
#define CX23885_BOARD_COMPRO_VIDEOMATE_E800    25
#define CX23885_BOARD_HAUPPAUGE_HVR1290        26
#define CX23885_BOARD_MYGICA_X8558PRO          27
#define CX23885_BOARD_LEADTEK_WINFAST_PXTV1200 28
#define CX23885_BOARD_GOTVIEW_X5_3D_HYBRID     29
#define CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF 30
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000 31
#define CX23885_BOARD_MPX885                   32
#define CX23885_BOARD_MYGICA_X8507             33
#define CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL 34
#define CX23885_BOARD_TEVII_S471               35
#define CX23885_BOARD_HAUPPAUGE_HVR1255_22111  36
#define CX23885_BOARD_PROF_8000                37
#define CX23885_BOARD_HAUPPAUGE_HVR4400        38
#define CX23885_BOARD_AVERMEDIA_HC81R          39
#define CX23885_BOARD_TBS_6981                 40
#define CX23885_BOARD_TBS_6980                 41
#define CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200 42
#define CX23885_BOARD_HAUPPAUGE_IMPACTVCBE     43
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2 44
#define CX23885_BOARD_DVBSKY_T9580             45
#define CX23885_BOARD_DVBSKY_T980C             46
#define CX23885_BOARD_DVBSKY_S950C             47
#define CX23885_BOARD_TT_CT2_4500_CI           48
#define CX23885_BOARD_DVBSKY_S950              49
#define CX23885_BOARD_DVBSKY_S952              50
#define CX23885_BOARD_DVBSKY_T982              51
#define CX23885_BOARD_HAUPPAUGE_HVR5525        52
#define CX23885_BOARD_HAUPPAUGE_STARBURST      53
#define CX23885_BOARD_VIEWCAST_260E            54
#define CX23885_BOARD_VIEWCAST_460E            55
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB     56
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC    57
#define CX23885_BOARD_HAUPPAUGE_HVR1265_K4     58
#define CX23885_BOARD_HAUPPAUGE_STARBURST2     59
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885 60
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885 61
#define CX23885_BOARD_AVERMEDIA_CE310B         62

#define GPIO_0 0x00000001
#define GPIO_1 0x00000002
#define GPIO_2 0x00000004
#define GPIO_3 0x00000008
#define GPIO_4 0x00000010
#define GPIO_5 0x00000020
#define GPIO_6 0x00000040
#define GPIO_7 0x00000080
#define GPIO_8 0x00000100
#define GPIO_9 0x00000200
#define GPIO_10 0x00000400
#define GPIO_11 0x00000800
#define GPIO_12 0x00001000
#define GPIO_13 0x00002000
#define GPIO_14 0x00004000
#define GPIO_15 0x00008000

/* Currently unsupported by the driver: PAL/H, NTSC/Kr, SECAM B/G/H/LC */
#define CX23885_NORMS (\
	V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_443 | \
	V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK    |  V4L2_STD_PAL_I    | \
	V4L2_STD_PAL_M  |  V4L2_STD_PAL_N     |  V4L2_STD_PAL_Nc   | \
	V4L2_STD_PAL_60 |  V4L2_STD_SECAM_L   |  V4L2_STD_SECAM_DK)

struct cx23885_fmt {
	u32   fourcc;          /* v4l2 format id */
	int   depth;
	int   flags;
	u32   cxformat;
};

struct cx23885_tvnorm {
	char		*name;
	v4l2_std_id	id;
	u32		cxiformat;
	u32		cxoformat;
};

enum cx23885_itype {
	CX23885_VMUX_COMPOSITE1 = 1,
	CX23885_VMUX_COMPOSITE2,
	CX23885_VMUX_COMPOSITE3,
	CX23885_VMUX_COMPOSITE4,
	CX23885_VMUX_SVIDEO,
	CX23885_VMUX_COMPONENT,
	CX23885_VMUX_TELEVISION,
	CX23885_VMUX_CABLE,
	CX23885_VMUX_DVB,
	CX23885_VMUX_DEBUG,
	CX23885_RADIO,
};

enum cx23885_src_sel_type {
	CX23885_SRC_SEL_EXT_656_VIDEO = 0,
	CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO
};

struct cx23885_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

/* buffer for one video frame */
struct cx23885_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head queue;

	/* cx23885 specific */
	unsigned int           bpl;
	struct cx23885_riscmem risc;
	struct cx23885_fmt     *fmt;
	u32                    count;
};

struct cx23885_input {
	enum cx23885_itype type;
	unsigned int    vmux;
	unsigned int    amux;
	u32             gpio0, gpio1, gpio2, gpio3;
};

typedef enum {
	CX23885_MPEG_UNDEFINED = 0,
	CX23885_MPEG_DVB,
	CX23885_ANALOG_VIDEO,
	CX23885_MPEG_ENCODER,
} port_t;

struct cx23885_board {
	char                    *name;
	port_t			porta, portb, portc;
	int		num_fds_portb, num_fds_portc;
	unsigned int		tuner_type;
	unsigned int		radio_type;
	unsigned char		tuner_addr;
	unsigned char		radio_addr;
	unsigned int		tuner_bus;

	/* Vendors can and do run the PCIe bridge at different
	 * clock rates, driven physically by crystals on the PCBs.
	 * The core has to accommodate this. This allows the user
	 * to add new boards with new frequencys. The value is
	 * expressed in Hz.
	 *
	 * The core framework will default this value based on
	 * current designs, but it can vary.
	 */
	u32			clk_freq;
	struct cx23885_input    input[MAX_CX23885_INPUT];
	int			ci_type; /* for NetUP */
	/* Force bottom field first during DMA (888 workaround) */
	u32                     force_bff;
};

struct cx23885_subid {
	u16     subvendor;
	u16     subdevice;
	u32     card;
};

struct cx23885_i2c {
	struct cx23885_dev *dev;

	int                        nr;

	/* i2c i/o */
	struct i2c_adapter         i2c_adap;
	struct i2c_client          i2c_client;
	u32                        i2c_rc;

	/* 885 registers used for raw address */
	u32                        i2c_period;
	u32                        reg_ctrl;
	u32                        reg_stat;
	u32                        reg_addr;
	u32                        reg_rdata;
	u32                        reg_wdata;
};

struct cx23885_dmaqueue {
	struct list_head       active;
	u32                    count;
};

struct cx23885_tsport {
	struct cx23885_dev *dev;

	unsigned                   nr;
	int                        sram_chno;

	struct vb2_dvb_frontends   frontends;

	/* dma queues */
	struct cx23885_dmaqueue    mpegq;
	u32                        ts_packet_size;
	u32                        ts_packet_count;

	int                        width;
	int                        height;

	spinlock_t                 slock;

	/* registers */
	u32                        reg_gpcnt;
	u32                        reg_gpcnt_ctl;
	u32                        reg_dma_ctl;
	u32                        reg_lngth;
	u32                        reg_hw_sop_ctrl;
	u32                        reg_gen_ctrl;
	u32                        reg_bd_pkt_status;
	u32                        reg_sop_status;
	u32                        reg_fifo_ovfl_stat;
	u32                        reg_vld_misc;
	u32                        reg_ts_clk_en;
	u32                        reg_ts_int_msk;
	u32                        reg_ts_int_stat;
	u32                        reg_src_sel;

	/* Default register vals */
	int                        pci_irqmask;
	u32                        dma_ctl_val;
	u32                        ts_int_msk_val;
	u32                        gen_ctrl_val;
	u32                        ts_clk_en_val;
	u32                        src_sel_val;
	u32                        vld_misc_val;
	u32                        hw_sop_ctrl_val;

	/* Allow a single tsport to have multiple frontends */
	u32                        num_frontends;
	void                (*gate_ctrl)(struct cx23885_tsport *port, int open);
	void                       *port_priv;

	/* Workaround for a temp dvb_frontend that the tuner can attached to */
	struct dvb_frontend analog_fe;

	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	struct i2c_client *i2c_client_sec;
	struct i2c_client *i2c_client_ci;

	int (*set_frontend)(struct dvb_frontend *fe);
	int (*fe_set_voltage)(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage);
};

struct cx23885_kernel_ir {
	struct cx23885_dev	*cx;
	char			*name;
	char			*phys;

	struct rc_dev		*rc;
};

struct cx23885_audio_buffer {
	unsigned int		bpl;
	struct cx23885_riscmem	risc;
	void			*vaddr;
	struct scatterlist	*sglist;
	int			sglen;
	unsigned long		nr_pages;
};

struct cx23885_audio_dev {
	struct cx23885_dev	*dev;

	struct pci_dev		*pci;

	struct snd_card		*card;

	spinlock_t		lock;

	atomic_t		count;

	unsigned int		dma_size;
	unsigned int		period_size;
	unsigned int		num_periods;

	struct cx23885_audio_buffer   *buf;

	struct snd_pcm_substream *substream;
};

struct cx23885_dev {
	atomic_t                   refcount;
	struct v4l2_device	   v4l2_dev;
	struct v4l2_ctrl_handler   ctrl_handler;

	/* pci stuff */
	struct pci_dev             *pci;
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   E������������5wDf���>��{i����Ԩ_`��x��X�k��
?X�a-@Ώ���oO��5j�a��&D@,0�:-�M���Y#�"TV�����oYYfJ�-����@�,�cr�~�T�&��m�C���&#j�P�x��9M��C��p���VEC�{z�k2h��[wK��Pa�?�r��4*׵�>4���HGX߫7\����\�ͪ��mE�:�����TQ�tt�n�r���E���ś�ok#�k���b�EY�
ٰT����o�|����Ԓ̵7��ȋ����d�_��Ț��6��&h^��n�I� ����k��羑t�{��2Ӿ�@�\ HW{��+����Z�$��r�"ƒ{��(s��5<Ǌ���/4�U�`���^���L�f�w��ٶ$��Ѽ0��&2�\ �h(d��H��l�vM�oT�g����V�2�x�4��{��U�֌�|4p{��J`��N�tT�����G Jx�=@ c��@rh{�j	-�v{�ڐ����OX��e��e��0ѹl��5!����I��ڌ�� �a�qz1%��%�y��Cw�L�6©��Μ�L�ʻ��[��c����~.���*�M�z+ձ���U��:>������*tPhi��Q�(ᖛ z�/�D�@޷/���}���I~rmW��/��D@S@:Qp����<CJu�[��F�]D��R@y�&��&
�f���T �gJ�j@3E�v^�)C�X��/��lʨT�KN3jf�ͪ���a�?\i�g��6,�=|Y����S�g�����pI��Wnoݬ�e]1��,k)��]%��p�:]H����ĪbM� ��Z��З��yPZ���+zg@t�K�٘1���B���`�
 �/��P-��l��I�H<�ے1��\��jE���k������8�ޅ����쭢U��c�E�ٲ?b,񗊱��,n�Y��J��$>M�2rW"��3��_`����8Y+�T6-w� ���B@Z�zq��)�����0�����}�'���ׂ�z�h��a�1"�W7m.���h�q�S9���A�t��_�N�R�2����S�7��p����:���i�ZCH�a����yz0�^m��p�u#X$g4�"U�Ї��Cu�!xcٔC�^��>UH�����R݀�/�/��i�/���LT�DہK��ؔ��U5UC���j��p��-�l��`����6�?�r#�����chQ[�jL��"j2~����>H1)qJ$�U��Q~Q��.�Be��?�7�J��G�y����.�r�U�0�)���l��)S���;�@��$���������'�cR1o#_��9^��z22�*��Pk`l���*���м#|'�YJ�SV����L8+�B�	Ӟ�+���;0����m��w���M��L�8u��K��>S%(S7k�.� "�w�gY�7��'�0!y�å�L���"�/���zϡ����MU�D���$���"�Q����+59���-� ������F�����y y�l�cjg�Թy�v�"�&�D��3�T�2�P��L7�p��5��q)m��yS��ʬl��!�/O�,N��Zp���Pҋc�^Q~��M�Zͬm5��n�ڏ⒛�Ω�ˡ�Z��P�\�*��tɣ�X�D��(i�G^�ѕ"�֧��N�����J���K�6}�R#:�u�騒q��Qm�̩���@FMg���pM��7P*e�2*a�mAk~Oj;y\q���;��I�N]Ă��[o^�F��\b3"���W����r	����ӑK9��kgt�0ԐJx���$�խ���&�CqP�%��7��i���J�ox��1�����*�sA0�����<	vw��t �S�|�����A㇘u�2s�{���zJ�⽈ɹ6f?sg�6���wQ�R�ك�� ���
�~���@((%Om�F�8�5�c�`�2=�K��hMT��l�5l92A��L�-��f�҃v����(�4����C>_x�AB��j� ���~O�@���|�b�B���fvqx%#��T��W��N���e��������}�Qs��g�����tٳ�������Q�+�_�'�i;o�Μ�}�J�=�*�a-��(K�	��"�6��g�ë��_��#,�9'mJ����1���dC��=JfI�t�bV'�������hh��(S��o�
�]��HD��9&��lx��Gr1R�$����y��>3�\��(� �B/6����Λ� �'a�L5��(��r�jG��M�Dj�1���e���6B/��F ���_���1����%%iU
D�15$en����׋�ꁶ����ٿ�f�B�O����a[�!5őh܄�vI�M�eo����<VY`�5��w �*Vm}%�V�\7sL�xf߽�^LfhB�51n�(2���8u�����ۨ��P�.y���VVM��[�A�M�V@��c��sӸ�wj6�����J���X��w"�;�z�~�"��(�������@����I�n��ʘ�B�Q��kT�gz3�t�nP!)�|�8��4]A`j:�(�����d��
����V��H�mZ0򩪋��0CZ�H�i��B�z�u;�R��6���R^�$Cv�?C{�"���|�6bQt\v>���]!�e��@�N���$�����w�>t��P�L������9
ԡ�L|�S[��bZ�����v��э��l�:����m�)�( ���S�� �<c9�����������q3��:+�!��P)io��ѫ�"t1)Հ��.	^�%����ۃG�b+���m�d?�s�;�;,�S��N�{qD�T�J}WXuڄ!�eŘmm1��AMh9�7=�~�2�gu����G�P4�'� S���9YҀ�����??&6B�� p��=�:����f�B�v ��m<e�1^[��V�ݡ~��O3�����gyv��K��J�NYfHyG��⩥��e����jg�n����ܐ���h�1���l�x������S�;�Uρ?�B��%�{�י���˻� 9[N��>u��T� �\y ��pttf�W@���7�t�7�4��$�	1�c��r���>Fm��A�^����cP9Lޣ�r�l���/�h4�+)3`�FX�yU�d��*Z���H���0K�.j�D�TW��z7T8ap����Q�H��Th}�AO&���Ry�s'X�%-	��|j	"}�F�y���=����&v��<'�#���K}�oٙZ7��UJڷ,Yiu߱���Y��e�d���'��[%c��k�z�L��0�uw��r�����e�������7�|����'ĭ`P���ݜq*h�f�$���?�B'L�O׺~���iUK�k3��YF�e+�sN�e��7j?)8�l90��C�v���㺃c>��%ϻ����޴ζ�?���"��G#���U�@��!Y�2���ayE�A.n��0�٪�7bL��"�ah]�����DM�we���"*�G��U�#����� �v�����b����Iݭ�8��7$���J)��(��tM-v*:G�|�Wx�"���K�j�`��[�ַ�n"U�9]��Ȯv0��6�\E�X�R�(\Gŋ~������`7%�أ�D��Ι�U1vK�]GU�y�X�Ɇ++/A5QS�y)
��n:C����f��5=�N�`�?�~��
=/�b�L����mv�Y��k L`�0��/��3H[ѹݹ0��b��M5V0������j��|�'t��;��@���>˫q
N�I4 �}����aAD�FpǪ�6^�^��rg,m���ɜ�G��Mm����䅵��pf�B��c�k�6��X�m��6y\�O飡Q�������>����>,n��Z���c�_�/�5��!6.��vO�m�?Ip�9=��K��b2ؖ*�⯯��$�� /W����f�~LC| �Y�h��L�33�7G9�7�9/�鳎��H��u������*{<A��cYL@��m�S�e�&?����������'S����W�6��[�k����3��&��}�0���1���V'6�}��-؈��}Xi�=[�{�������e{-/����L���������c����U��	&u�4K8�.�n.�����e6F-0,�yP9y~��<R4�\�"C-���]�t@��)9���,�׭��ɴ:.fM�c�1G��<�%�/�U"g��Q?�f�;���#��5VWC}����^�+�N;�Զ�]b �ձ�iS��g=�=��+6e��x�\�wp0#����E���8�N���J[V�(��yy�0Gj2��n^\��G� �.�׺W��T�4�-���j�X��j;���#�L�Jȗ|��Nd5#ϗ�B��Z�yo��k���K/���
	3�3fAgw�����o�����qTJb���|�R9tR���0��p浯ח��-ծ��9V"�v素������4��rΗ��w���,�Q6ָǯ�6Q�f���B#LՌ��7<���L����	��o��Ϥ�b�"W��{r�,T��I�9��?�AQ�w�Ved��o=׈,�i��*ϫⳔ��
6~�^�~NkIk�iţ�Xk�%�%�)�+�ʞ����Jr�6�}�Q'ވ�H���ۂ\�H,)��S��gP�C��e�L��oE��sV�eΐG�H�T��ϻ�Y�G�&C��6@�e�23�Q��/I���h�G�C �Qa��I (zP�޼�J��k�ɛ�´��m?�S�!�[���-M�Eʯr�c���e�_��X�	G �'ZIڲz�]�^���z�"A�Ea�L�g7"�y ���v�'12�
ڔ�j[/=V���,���1l[�g�aC�A���)i��v�V���y�g���Q��(���6��<�i�1M�CQΖ=�P�+�K��=]�R�|�(T�:*٤$m�n,�gZ0ձ���,�P53.�'~��5����M���y�}���両0Z�~ʯg*��'xڣP�	��ȁ��"ǁ�~U� ��f�s���qq��uך��:~%����}q'��� ��*�5=�<�Y��`�h��3��	��=��^t
1�]dysS� �"��W?��Y�C�x�L4��6�Ġ��e��
��X�:�K��A��K�(i {.zSC&(���l�MU #��-1+�������E�9F?�i+YEBEL����*v�,Ƨ:���qXJ��<�΀ZNd�HHcp�^����Q|��m�@Lx�˾h {L)����-��>1��(6��|�(_��!�eIHz���`���H�������|n�j\��m9������|�;�ȥ������'MY�#�Gb��*:ߜ'�s$uk�xM;��ܷ���N��1�d޽���x���h1��*ཀྵ����-k��)A�i�ܿD� :�q����}w��xb��S]��r$��Zx������K�O-ԓ���({ƣ;(�Ⱦz|;Y��v�Fޏ"������@8��S�� ��`���ɻ!�A��4]�_���
������5?����Ws��qX4Cw��+���F���oa���+�#S��K���z��e��A�!�*��ɤ�V'�J=�ͬ�6VF�h[��
s����F> 7�u���ta��,M������ص
�y��md����Ƭ��ߖ7�MV9%����v�F�a4��&�����γ��8����3bk��-���yw���i��l�A����ޙ.fzgE��?K�	�}��Ź���W��o-@���G����O�U0h~�z�-�E�u�*�=�NA �Zx~�z�m�+�Y0	o�W�;���ت���'��q������=��N�&�E�ê(��~G���?����5��L��u�
�3a� ��C}A��N'BU�q��M�"���?��Ӱ��p���?jMR�+al���$P�g]t�Rܨ� �X�!�ߥl`X��p��J���T[-毄���	������5�쐔�}"�5��0
T�����Z�͋y��q�\=y_M��|��hg�([ml�B+����܍�>�,q����H�/���H�E�u��ݨ�+�Ø���|�+8Z�J�z���<�Tf�hY�Ma��8����rL.2��i�lS$��U��i�����E{�w�	�����K�V�4JqsƓ;��0@QV�ա��G �B-�~q�X�=>�2%HE��R}ϭ��=.Ӣ�ݓ7�z�Π�=
��R|��mXw���7��E��o�],\��έ� �&�[�j�	.R�
L�^�� ���ܖK�-��
�ǲi�O	S,�%FM�Cf��bg|��gL�L��bɽ7o�C�x�e��Ɂ�TH�j����^���O����fn��f�SP��p�l`w)�`�d�EC�Ef�Fd����§�n���X���{T$-���^��JS�+�==��?�4`W���Z��`[������53�sR��{VW_C��-�zK������Q_�X�����$��R ;>�`٧縮@ �#Ǐ-��Wf� �o�fj�$�Y��\�B��bY�P˫\�٘��Ui0�>k�쐧���H�����"U^����?0>�?��>����"5�D��k~��z���J���` t�����OI|�0��ei-Z�����%;�$g}U�5q)��C"���f(���5���ll^��^<��i�%�d��O`,5H���X�G�|DRQ��;�M��D��:��� vB�uV|���2��*����^R�[t|��d7G�)�����9x�ʋ��k��d�&;G�'�!ҟ�R��3�9*\��~o�۾��5��M�|���Q�e&Y{�<^��g '�>�q[��eH\a�,�y����V�|BC��?��r��] �uK_�8�x��X�!�L�6�QS����9�N��d��ZE���\9���9�4KHXI([?�P��je���P����#�!�36Է����mʔx#0P��W��%��.'�/K>=L��eHL$$�R;�R 5�lKI�$|�8��CwD7��uz�ix6�Q[d��߽��9�<𺐿�n:m���&Uנ'E�n�ް8v/=)U3������
��X�(��p����5gu�`bB.�7ݙ],9������Z%�[�k}�]��0��nrļ�SLd]� ���b�᝱�o�L�u��A�,[�v�=�z]�':��
���UET�m �0e`����"GR���&C�~������N֛���y9\r)�@�M�[���@����P�w�\8@�[ݮ\7��>~��	���V�k����%ǻ���^�W|d֯ʗ�
�-�$��BR5sEp1ݶӦ��˷@h����R��csh�1O���>��G�����$<��� 'A��.�ƭG�g�����"q\�㗇�Z{K ����9�K�UD�.�[e��}4W���((����[]_�e,�_ي�j�\�a��4�;���!E� �ئS���x�<�mp��@~Aƽ<uо=(�0I�3a��ss��Y0���)���~^6[��%_�@ɢ%�Z�M5اH��a�=���Sp��3Ņ#4"�
~�wzkEis�ҩr��W���
�;���(V��1	'�#gN�t,5�q��^����~evh%kQ�z�o��Τt��Z	ej%�[��(k���������
B��k/� g����_�wj�ѱ3���e��z�jF�Y�"��j9�g<C�^GF?Z����͛��tL#���7;����v!!��j$��������m~8�ru����yq�'O �P ��D�$k�&�xXZd.쎴}��'w/�+���~v�2�����M,_��-�o��:B��SS�2�2�U_`S���,EX\t������3B�	L*l��ӁN$���
�����2<y)?"fͷ�6v���	a/����`���3}�&�8!,h-D�Gī���B�y~��O��s
�Jbڄ�ްN�㍻Ud��:����8(o���O+œ�@���w����ja�^m��I�SԼL|}�/�4���lqtc�Od����j��R!!�}�z��Y|a��m�W���L��l*�2��+4��0��i��_��}��a��7\�+���<'u�����+.�;4_>�LUc?Q ��L;���ɽ�![+�5����q �ݝ���+Ě��1s�j�l���S��VFR*"m��L�`����z�ھ�����Õ(u?���O��~G�3����1g��Tc;P����EQ�Ё���˱��q\��v\=��M _s���	�I[�5��D�_�v�Uki��zl��3����B+~��U�0p�������`��v�E��*ᛆ�!�v�q�aM�>e�Lh	�#&��lOi��}Y� �|(ޒ�N-]��z���;��6�ᒔ	(S)i8Ag�@�:���.x���B�>!��ݜ��t��ߪk���/�����e�|tS!�/�����mM���m׽��q*�f��[i%�bjL�*}��z�G����'	�΍�l���=4Ѣ�R3rwF�pJ���n��T΢/p��[x���
$9n�0���K��.^��t��=����L���	��o��}�EK!J�r�%��=�5�^��gPDat۞M��{�� ���̱�m��|�h&���UJ���V6�d�ҡ�Em_w����tD�,�����4���_:��v I�'ϯ���w$��k�m���&b�'�N�2U����U;Ht	o�D[�v��������gZ-��@N��s�����$.}�}�������C��C��:h0*27Y�N������X�����|��[35#��_�zјfĨ�$�J.xzfY�	�]�>�����������g�9�4��AӋ�(:~���>2�|E�g�)쫪��ٜ-�Kn�PQE���+pw4~�����_��q!~���q����v�o鶊����GD�zse�x��KC�W�y������jz�����dz؋�X
�o��(2����{A�v����Ѕ�/��s+H(i��k)�BhdaH�eq��P]���u/�j݊��|@A�c)z�h�h�S�W�]����$���oݥt7SO%�� -`P��N�HC�lޒ# �!sC�	��L��l0,�D��lҐ�ځ$��{�7�zEۭ����r�qjk��{��^ȇ �tG�(�=v$[�i�V�>5*���U�X�`��<W��Pd��y�(�M^��!�'��� W�M�pX&��2�����#��Aҕ��Dz��0�)J�(}���-��c��E����xN�'�u�^U�4���Rì�愁{����yO�͞&�I1�j[��c;/=Ķ>$kӦ�Y�[��:!3��TT�) 4+�� ��Q���g�w(�yU��j�.hj*V�������>����M#���.j�p�w[~/JƖ�	O�5�4B���=�tA�k���`��'(�V�en�.����DB�(�gV�l�#�Ƣ]+�aGi��^g��h�lB����	U㟓�W��k��0߫��+ᮐ���v���c�	hu۠�!��KJ�zoۣii�Q�Ĝ'mh����KB$d���G��A�o���~q���L*n8���.����a|о\�*��2�O�a+5��'�10���'��>��t�r����ߌ|�r�VxYONIF^$!�Q�[������^9��G�0�O���Gچ�?�$5�e"Bd��І�m���n�u��̿�Sd%@tL�Eʇr�.���)��!1�M�D~<�M�%(�`<�)��[⬽����:}�!�9�)+�{����Ÿ�q5��������u�e��!^F�kZIRO�'0q$	;�,ϼ�c27�@*1���Y�#���	3������(K��dP� ����0���n�nY�y����vI�s�K�1ɉ2ɰ����,��Yޗ���0E1����@�g��-Zl�.��'��X��M��}3���+�d��F�dȇ����^xG��-��?�U�ش�ι�6�RqyŚ�m�L�� ��^��Ϭ?��B=)��"1u��R�-�%�'�+�q0s��Q$1��_�������Y(��kv$�2�;�'>���P �vH�t�	L��WDx�?ȑ����V+�k�@m�R<�#�kd܏{� �K�ܿtpZPr7�J�5|��|)��mTd�N�ĹeC��*M��&����hX�%��ے��8��)!L~���.����%��e6H�HX�Rm'��Y�k��"���$¶��ۃ0/�F�L��N��~�PL�]�-v�������Ͼ�[�+q(P��|7�G�1�[tgxh�S�6��Ü��+��E�<�S���������݈Z7���9�p&>�*9��̽t�~�H�G�`�p�˷�᳐4�i*H��_8��m�Zu�6� �)�~��K�r�W�z�v}nOz:{));�������:5^�Xי��D�4���=l���@<�aH�������P�1��`�+��/����L� �t���-��7R��Ct潧�a���A�_PBc1{�I�8C9��"TF}��qgf�����xݵ�S�R�{Q�y��X�0�����.���!�/��+�WKe"���!�|�W�,;����1-+�;^�����S̈>X��\�}ߚ��u�ăQ�	��������g�EI��/�;aR�����iNy�Q�@�}uWCU˨�e�g�*(�C�<8"��&(j?+G�ƒ��S~es�Q �t�S�Y��5	�a�:���`�u����S����I?�	E�^�h�w*��8�"���ri#���������L���Y	������!֨C�� μG[ǌc��o-��կ�gt�)���������S-8ӝ���������os>�b	h��l�o��l򁬱�y=ךŰ����"�RU����{�@��l�$_W;NB��7[}��d2{�	��~y̫���H��1�4	� 
H�����#>O��>���S�=ӵ�ݨ�j���3�ܨ�v�"�����q1xiº����㶵��mk>�T����'�Ai4��h磯J^/ƑX j�~'ٸ����@��P\�_gO_��6�5epH_o��1�h���s��7� ]Vv�&�;xRa��J���u��^GO��^�w�t-w�Z�r����ǖ�3;g��ҡ���b�g�u��?;�R�fsz��������:�̘<��`b��'r��둓��0�L��j
s^� ���3��[��������@]���C�]�X�Ӭ���.����"�5T`4J��Ѡ�+9���`�6de��3QM�: ^���S�[�p��i�!|PP�:������b꜁6�]ǽ�̇�A��/[G��A�+�|��� ����In<������۔*�2`l.�_\[w��J\0L���/]	�+�Q.�<gK�,βdƝ�C�K�zm��聳���Jʵ�߈��)����`?�oQ}�޲䩠�x<rgh����E]*߫�x�ȁ%i����=	��4.)̲��B�ip����}�0��X4j��%9��{S#�93
%�zm�7�ģ^�9,��.��1y5ܬ4]Q�d�w�/\[����$�*�͢׷����L�a���.�;U�!���q����D�̫�8���S�CN�Bƹ����0�_�!���َLOI�u���O���Y<՛��b8��Q���M'�tPc�^�
��I�� ���i����l���mG��1�X�ip�؃�pAe��a�أQ�/��$�]�	e�7��x��<d/|w#��:�[�Q����S��������Rk��Q��@�P���nꆌ�]��b�����LP"��`�-[p�1�!J��׎o(V�ڧR���=�,� �K<�p�j��k5%:�t�j�&�E��f�/��y�������= �0�9��V�*&�	TO2c'�,�b:d������61�}4�1�F��nߝ�"^,(s\*7�%^�_�A��� ^AtR�<_� �ѿ��H�m��A5F��B��w ]� j��?Aּ�p�Pg[(`�wu�L-Z�d�@�015:?��,��fM�qJ���5���(���	*������<�=�qVˡi��r�Ro���8͔�%�z��y؝��Z6��Y�/_��.��P�Q�����\�G����}���s֔�azT:m�QE�(�Əٲ�r�?%�H�o�c̫{p�9OL �4��Q�FZ�O�̋x
�ls"�5���센��P��t%�ɘj�_��-.��6�ڜ��R�X5�ea���S��`�.&��NX5�����wt瀛�%�����ם�҂qv޷Pd!hGƌr�6]+մ��m˭n	����^�;(�s����"��Oɶ��MW>�mY��rT`v�Z��uV�~�NH��ڡ����!>7v[n%��T������s���ㇶ:�p���Sa<M�(/>��b_%���������@���<��_����$��uR���i�/8�KV5_�df%�1��v��Q�>�+c6�pS�S��z܈��I>�E��x-̐��{2vh���q�	�=&r�Ezj��Y`Σ.�d�m�ê�����+�l������si�pu����nsCQ#��_�Oؿe��nq�M�c���LUK �AQpn��4�Z��d�b4w`�ȧ�Yˈ=��=���Eb�H^�k_�^�	:�2�3�?��(��]v���c�����!��o�뿪n���Ӷiܠf�:[�z]��s�q��О��b�_`ΰ,pIy?�zS��Ae*�b� [�#M�=@�5p�J�PM0\����7�h�(ks�E�����%�}3J�r��+�y�Czk(�{��#tj�ޖ>�1�Zl�O���m�l8�4�سH�F#\E��D*��z,ہ��#k���!*^;E����A7pY�ּ�^�E(���^7���`��R��8C�m�[w��$������:������	Q�ft��@1L���WZ%�gu�a�`r�KNΪ�$�(�N�;Hi���<�]��e���φ�8o4���e�c��0����Gv�jF�9G�[�q���6X����E�j�B��C�ޗ�==TA��_Y;��V�5�iL�PiϤ_��ʘ��#E,��q}���W�������W��csѿ���#O}�ۂD��P��'�g�[�������Z��a�@"�?w�y����87Ԛw ����K�Qb����y���c�!��`Ȋ� .�Mk6T�q�0���&��TpZ�ݒ+B���k�=P����m64��?���)/�X����X�u)�c�]��|�b�qH��(��ǁ]�F=P�۰b��bD4����:#�����2b���t2p6�b+�s�	�w�*$C��:��*���r�����m�-N�<�7����_�+ma�j�y�`��%�_�h�+/��m�Z��`J�^L$�7;XS�b��n ;N3{�:a�)��u�����B��`T��E!ӚR^�z����k�B.R���kRQ|�lJ�$ڕ$�&�*���,���J�8aϟ�,}O[�g3X�w�M�S�&!\�����1�yI��e�s�y7\�G���*ShO�&G�����A3�E�ы�SeY���L>�xI@����X���Gb Mh�k��)w����^�`�鹙��yY�v��#��cu��°��	��i�����o�=�n+����}��0���J��EP�\������n(N<��~��yI4-������
$%�
�F��p�B~hj��N��d��)��}�gLޠ����"��L),�F���v~�)!��`��ї:�z??S��&�j`3��%��Y�\�s����v�"7Կ@���x��J5.�����6�-�!0���e5�������H���oL��M��p5b@�ӽ'p��O��'d���]qW&��QU��~P�0]MĊ$JM�_8�1Φ56�fC�|;Ǹ����k
�#iȔ_�,�w��Qx��A���/xW�Xm6�ڀe9��$O�O+���v�]B�oӳ���Ԯ� �:�K3���W`�k�&N���Ta�����"_O`d�9��Rn7փ��������2�U.G{����+�xl����E��t�B� .3�Q"�@*����F����\�����Af��ӵrIh�O�ڶTO��?�&������̄>I�Ty��,%�/�w��o��G �\�b��~��ܩ�b��1� ����(��t�x)��-KE2H�[�f������\��jB����i�����,�4�І��41�=vl�4�|�����)^��]���Y�v��5qZ��n�����������MYJ��k<o���IB��e_�a.��Pى�:W�?��K���ޮD�`�4�}"sI��/vG�E�:l"4	�=!ӘBP]���kv;�8s�:�]���c��;)-�W�ux{;5�E|z��o����?�Nƺ�� ��k�Jw��]x"�{���R��A�l,CԽ� ݏ���hS��o��+Y�!Wf�G�s!��m� �8~��ʐ�����c�͓L�� ���jgk}JYOѹ^~m�_�>L�Hk�nE�yd���٥��3nç)�6��7l�V��Y�!OE�vZC�r����v��)ŖO3Y��[z6L?= ~:+'"Fc$��j��]?�� Z�I��A82^�����l`d��J���J�Pb�y�`ɶQ�5�AЩ`J�-��8O#�a_G�u�����R��RoBҨ�xݝx����ؽs��
�@�m����j��}�`6��?����A�.��xGb�� ��}.�,�"[�����l%�V�~Q�e���Z�> ����Ta�EC"��ǚpڼ��������ؗeo�NqQ�\�٤dN��U�7���wO��O���~�<u��բ��<L��iR�ȅ�����bJ�2'�f��pQ�;䣔���yt�E]K�z�ۿ�Σ"��DU�2"IU?�v!q̰��I��)뀵p���F�Dȿ��V�6Nw��~B�6��K��ꤤ"%���E�4�9� 4��ɼ�Q���(K_hy#�Ĭ���ᨨ��>�B,�DD}�t���Y,����g���*!( ﮣ͖�H�9>�?k���m�����XB���8���G7�a	�CH���B�k����l���Oa���!�yQC��%��^ԇ�~¤���K�SCΥ��oQ��<NJ9t��<�66-~
l�t�ͭ�R�a�j�xÑt?�b'l.˟#ްڦ�=vt�1�-�*��!rQ@����ΊCȩ/��4�Ak"'&d��	ټ3���*g1�#��|��)X�I��i������/��d��<�7�m �����[C�N��=	�O@I�|r��8�L'`�si�tK��W'z�8,%��Ȓ��|M�i
���k��	���g��y��a_^I�YkvX0�5P6����6���5<0&�Υ�R_]Ӹ�䍶L�-�,Ξ`���n��'O���O,~��3�T���){!�yD�eH9�/9��'4o�3�;�݌���¹�}x��を�6O9d�H7-ř�[]�஌=��Ώ�����\�ږ�(�vP���4��?�L�n\Ku"q��n�m�|.}2���$�1�h��a��.qͽkl=''�
]'W�lĿzY?�tr%g0¸��-~d���gb+�t����8�MQC�y��Xd�2J������Q���5���l6�܄��o(SQ����o,f�g��៶O�ٞC�h��]o^�2�^8��O�7Ɨ��+qBq�����S�w&4ĝ_k�H�4��Z5��:K���9
�ԥ�7c�i9�^������-VkQ�;IHx����+۵�jSo�N��a:���p˛*yY���\z���2��S՞�hIS.p�3U����3�@���C*(A$c�E��>�D����k���d�.���S��B6L�5	�#��d�?$��!n�3ވݴuk_�oq��T�*���@���ː�/��ҧyv�u��ŧ��1�`b�dٿ����2C���]� �]�"�2ԟ1��^�	���~KeO�wI�L1m�N�&�Z�&�:%��|��"��E�x	l��gq����\��_	 �Ʋ�ѵdr�w`���r�S( ��_��Z	��vF����
J�TvxOb���Ь�QC��:U�J0�֏?�qS�2ORRU����*g��.AX+�VW'�xy�	a�h�y�1N��g_����y��f�ҍ���KwRs��� �ͩa���آ`^������� �?@��z*�+Ck8\.q���7L"Sz��Q�d���X �[c{�OL8j���B��'q.�C�G�����2X��"lZ�]����Shn$�+�}h�h,	���-����r���PN{t��_��4��X�C
��5*�!��;AV:� ��rA�w����躗g�U23�w�H+��O�@�`Î7�w�1 ����$y�^����&�e�H�"4�2�m�!�婧��������OB$����e�W�]	�~C�bw�X[5�2�����ً�ޜ�p��U�v���ɷ@�Qj����+�	��m{`r�«ٚ	�*���'�������a��dy �k��!���{;ڗ�;�;=W+�pN5]U�k��e��h?���'(��$����o$�Nڨ�����AO�ͼ�^ ��Oh��K��n
:��J��{]!(
Hm��Q��ނ��u�K�َ*�.�ee}��(��PC�h��l�~1gw雉<�`��5!Fi�`�9W���˯��b�5��������it���K�y�����>�*�c��hN�@���B��`K������"3�8�G��Ͼ��3q13Ra��Ⱥ��m�8�?Bw���z*c��I���Kݓ�&���=<j2�8_�HϫY���"��������E[�7�b��(V<{�ڀ�l��In���Ŗ1(�I��dx�ē�������.h�;�iU}3�;�:��2t�A@�Bx��>���6����n@� �HE��S��~���M��*������p���T�S,�r0k�ұ|������!L,6�Gb�˙9Yc�7&���Io�0��)1�/U��˟̍2C�i�3C��hjd�*F������O,}G'e�s��P'%,�c�eJ0�61W�N�V������FXAN?ެU��o�$ڍ߼uK��'�	�)��ӀA��^x���`�e��?�n�\�y
�`I	h*�P&�+����_�~�ꝁ�}������=�G�d�8��e(���"V�N�v�ɬ�����,�F��Tp6���a��q���Q�X���Ѩ�����EG����Ԯ�`~�o����bS���3�ž�c��q�%�P�{�٣y�;GO�E�����̭���o]po|~�C#��&��k�Z�P8�a!���(p����$bv���d���Vɍ[d�c$�B��eŝ*�K���������"ذ�I�q�(D@�7�qV`��
��0t��|	�8�z~���C��[���._10�`��!������[d��:�/o
��,7��Ep��B�vl�/��W#��p���mNp\�k��V�Ѹ츠hEg�'�w�*��%�N�	�I�9d�Z�����L�Ğ�������Rq�lo��6{�Ka�p}��X���{5���#`ٙ�o�ɀ�zP���V_��
Wu��Ǎ�͹���gQ�쥻�9f���q�y�{�/�u+"�ߒ� �κ�c8�S�	kq��M6����P:�q��9:0d����%i4U���:��a���n���s���,�.H$6MϺ'm�&?Я����[��ƻ+��:�f�}&Eԑ�ᾜ�hǀ��ON��}J���:Dg�/�� LA�=�{��)�ӿw����ph��y�!D� Ɔ$^�uVO !<Nw�5��20��S��Օ�7�ԹC�!�d�j�<#d�[SO��ZO�Z�l���;"�{��Qd�Bz%�yާP��چ|�?n�qȢ'���D&	sM�{C�j���ʳ[F��.&ί���Y�+�Q��9�,w�Y��Y@*C��[���w���QV/�̕�v�[(��ht-�m
>�y�x�!�~�r9�b��0;E�&�O('�oo��W��D�c�<�lw�i8��@��MI�,���I��ª�0�w
�I	:����9��!-�1���hO�ş����~i؞=b-V�OdWKg,���Oբ�"{dP���1Dҽ�Q_1@�^,��JhIJ-u�
%���X�\ƹ�ډ�+k��6�<7��I&s ZS��=vYmu赩�D s�u*�ن
�7��2ó|s��h!��U��u]�%���^�D�G����a1���L^���_���\��S{p@Hk:xo��|�+�;��x�݂�䄌�#�����0Ӄs@d������a���x�[���� �?dM����ny-<Cd&��M<}��ǪmXq3W����^j�g���~����z�0���1ʇ�@G|<�L��V�c49/D"��7�(�]�D���9H��Z�R���5����	�%^���L��LJ�fC��t)�.�)��A�=��iw����!J9rr9X����73Îqy������K!�}��|\6��kY��`�j�0|ca��q��J2�v�P�4��(����F�����j4b��-�r��;?�� o"������q��܊t��1d�c�����z�����z`[$����(�fAU���퓓�]��8k�"20��[�@ ��`���ox��M¸n��	8]V�҃�x�	�zR��o�c�^����5y���Ȩ�� �'ǩ��mːSx�'<�\_#�IR��P��Z����[�0��#ʸ;��6�J�~�sp�؍&l���@"g�7�+j�LE:�+��dt-������b'�0�YC�4�A�Fł6D��e5Ih&݊��t<�T���9�6�T��}�C�G��ud�k��˵��[*�fQ���^�ER�e{�=o�z���+{gN"^�]�n깤a���sֿx�����H.�CĪ.��6�"�̞�!�OF������[�sH�k1�4�'[n��c��[瓲�iK	��r���,q;��K��]�%��V^8腆�-�P��*͗e����yW�;GJǒ�\8ż�������G���L�"�\���[�M��)�1|
�Mtk�/�E9�|}#�F����X:�r�����V̻G���$+ ����2�w$�ؾ!��q�7#"`'��h��!�Q�Sh���rhť����M��X!�SUwC�'��Ex��&�6QM�0�ώ��D�/s\a�v@;�}����˙6z�}	��@`��`UdwZ�\ �TE&�㳁ʳ��ę�ž(J����N�R��&7��^�����qb��lNZ�$,��'�a�8�&��y>^[��8섴t�ֆ���u;�0|���2"�4�d�Wd�2�*N�	��Ϻz��>�MO"T�������M"��;5@���T&y$h���