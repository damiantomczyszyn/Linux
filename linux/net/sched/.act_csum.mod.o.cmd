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

ex