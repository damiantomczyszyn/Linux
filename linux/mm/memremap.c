ority = p->rt_priority;
	rcu_read_unlock();

	/*
	 * This one might sleep, we cannot do it with a spinlock held ...
	 */
	retval = copy_to_user(param, &lp, sizeof(*param)) ? -EFAULT : 0;

	return retval;

out_unlock:
	rcu_read_unlock();
	return retval;
}

/*
 * Copy the kernel size attribute structure (which might be larger
 * than what user-space knows about) to user-space.
 *
 * Note that all cases are valid: user-space buffer can be larger or
 * smaller than the kernel-space buffer. The usual case is that both
 * have the same size.
 */
static int
sched_attr_copy_to_user(struct sched_attr __user *uattr,
			struct sched_attr *kattr,
			unsigned int usize)
{
	unsigned int ksize = sizeof(*kattr);

	if (!access_ok(uattr, usize))
		return -EFAULT;

	/*
	 * sched_getattr() ABI forwards and backwards compatibility:
	 *
	 * If usize == ksize then we just copy everything to user-space and all is good.
	 *
	 * If usize < ksize then we only copy as much as user-space has space for,
	 * this keeps ABI compatibility as well. We skip the rest.
	 *
	 * If usize > ksize then user-space is using a newer version of the ABI,
	 * which part the kernel doesn't know about. Just ignore it - tooling can
	 * detect the kernel's knowledge of attributes from the attr->size value
	 * which is set to ksize in this case.
	 */
	kattr->size = min(usize, ksize);

	if (copy_to_user(uattr, kattr, kattr->size))
		return -EFAULT;

	return 0;
}

/**
 * sys_sched_getattr - similar to sched_getparam, but with sched_attr
 * @pid: the pid in question.
 * @uattr: structure containing the extended parameters.
 * @usize: sizeof(attr) for fwd/bwd comp.
 * @flags: for future extension.
 */
SYSCALL_DEFINE4(sched_getattr, pid_t, pid, struct sched_attr __user *, uattr,
		unsigned int, usize, unsigned int, flags)
{
	struct sched_attr kattr = { };
	struct task_struct *p;
	int retval;

	if (!uattr || pid < 0 || usize > PAGE_SIZE ||
	    usize < SCHED_ATTR_SIZE_VER0 || flags)
		return -EINVAL;

	rcu_read_lock();
	p = find_process_by_pid(pid);
	retval = -ESRCH;
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	kattr.sched_policy = p->policy;
	if (p->sched_reset_on_fork)
		kattr.sched_flags |= SCHED_FLAG_RESET_ON_FORK;
	get_params(p, &kattr);
	kattr.sched_flags &= SCHED_FLAG_ALL;

#ifdef CONFIG_UCLAMP_TASK
	/*
	 * This could race with another potential updater, but this is fine
	 * because it'll correctly read the old or the new value. We don't need
	 * to guarantee who wins the race as long as it doesn't return garbage.
	 */
	kattr.sched_util_min = p->uclamp_req[UCLAMP_MIN].value;
	kattr.sched_util_max = p->uclamp_req[UCLAMP_MAX].value;
#endif

	rcu_read_unlock();

	return sched_attr_copy_to_user(uattr, &kattr, usize);

out_unlock:
	rcu_read_unlock();
	return retval;
}

#ifdef CONFIG_SMP
int dl_task_check_affinity(struct task_struct *p, const struct cpumask *mask)
{
	int ret = 0;

	/*
	 * If the task isn't a deadline task or admission control is
	 * disabled then we don't care about affinity changes.
	 */
	if (!task_has_dl_policy(p) || !dl_bandwidth_enabled())
		return 0;

	/*
	 * Since bandwidth control happens on root_domain basis,
	 * if admission test is enabled, we only admit -deadline
	 * tasks allowed to run on all the CPUs in the task's
	 * root_domain.
	 */
	rcu_read_lock();
	if (!cpumask_subset(task_rq(p)->rd->span, mask))
		ret = -EBUSY;
	rcu_read_unlock();
	return ret;
}
#endif

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask)
{
	int retval;
	cpumask_var_t cpus_allowed, new_mask;

	if (!alloc_cpumask_var(&cpus_allowed, GFP_KERNEL))
		return -ENOMEM;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL)) {
		retval = -ENOMEM;
		goto out_free_cpus_allowed;
	}

	cpuset_cpus_allowed(p, cpus_allowed);
	cpumask_and(new_mask, mask, cpus_allowed);

	retval = dl_task_check_affinity(p, new_mask);
	if (retval)
		goto out_free_new_mask;
again:
	retval = __set_cpus_allowed_ptr(p, new_mask, SCA_CHECK | SCA_USER);
	if (retval)
		goto out_free_new_mask;

	cpuset_cpus_allowed(p, cpus_allowed);
	if (!cpumask_subset(new_mask, cpus_allowed)) {
		/*
		 * We must have raced with a concurrent cpuset update.
		 * Just reset the cpumask to the cpuset's cpus_allowed.
		 */
		cpumask_copy(new_mask, cpus_allowed);
		goto again;
	}

out_free_new_mask:
	free_cpumask_var(new_mask);
out_free_cpus_allowed:
	free_cpumask_var(cpus_allowed);
	return retval;
}

long sched_setaffinity(pid_t pid, const struct cpumask *in_mask)
{
	struct task_struct *p;
	int retval;

	rcu_read_lock();

	p = find_process_by_pid(pid);
	if (!p) {
		rcu_read_unlock();
		return -ESRCH;
	}

	/* Prevent p going away */
	get_task_struct(p);
	rcu_read_unlock();

	if (p->flags & PF_NO_SETAFFINITY) {
		retval = -EINVAL;
		goto out_put_task;
	}

	if (!check_same_owner(p)) {
		rcu_read_lock();
		if (!ns_capable(__task_cred(p)->user_ns, CAP_SYS_NICE)) {
			rcu_read_unlock();
			retval = -EPERM;
			goto out_put_task;
		}
		rcu_read_unlock();
	}

	retval = security_task_setscheduler(p);
	if (retval)
		goto out_put_task;

	retval = __sched_setaffinity(p, in_mask);
out_put_task:
	put_task_struct(p);
	return retval;
}

static int get_user_cpu_mask(unsigned long __user *user_mask_ptr, unsigned len,
			     struct cpumask *new_mask)
{
	if (len < cpumask_size())
		cpumask_clear(new_mask);
	else if (len > cpumask_size())
		len = cpumask_size();

	return copy_from_user(new_mask, user_mask_ptr, len) ? -EFAULT : 0;
}

/**
 * sys_sched_setaffinity - set the CPU affinity of a process
 * @pid: pid of the process
 * @len: length in bytes of the bitmask pointed to by user_mask_ptr
 * @user_mask_ptr: user-space pointer to the new CPU mask
 *
 * Return: 0 on success. An error code otherwise.
 */
SYSCALL_DEFINE3(sched_setaffinity, pid_t, pid, unsigned int, len,
		unsigned long __user *, user_mask_ptr)
{
	cpumask_var_t new_mask;
	int retval;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL))
		return -ENOMEM;

	retval = get_user_cpu_mask(user_mask_ptr, len, new_mask);
	if (retval == 0)
		retval = sched_setaffinity(pid, new_mask);
	free_cpumask_var(new_mask);
	return retval;
}

long sched_getaffinity(pid_t pid, struct cpumask *mask)
{
	struct task_struct *p;
	unsigned long flags;
	int retval;

	rcu_read_lock();

	retval = -ESRCH;
	p = find_process_by_pid(pid);
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	cpumask_and(mask, &p->cpus_mask, cpu_active_mask);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

out_unlock:
	rcu_read_unlock();

	return retval;
}

/**
 * sys_sched_getaffinity - get the CPU affinity of a process
 * @pid: pid of the process
 * @len: length in bytes of the bitmask pointed to by user_mask_ptr
 * @user_mask_ptr: user-space pointer to hold the current CPU mask
 *
 * Return: size of CPU mask copied to user_mask_ptr on success. An
 * error code otherwise.
 */
SYSCALL_DEFINE3(sched_getaffinity, pid_t, pid, unsigned int, len,
		unsigned long __user *, user_mask_ptr)
{
	int ret;
	cpumask_var_t mask;

	if ((len * BITS_PER_BYTE) < nr_cpu_ids)
		return -EINVAL;
	if (len & (sizeof(unsigned long)-1))
		return -EINVAL;

	if (!alloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	ret = sched_getaffinity(pid, mask);
	if (ret == 0) {
		unsigned int retlen = min(len, cpumask_size());

		if (copy_to_user(user_mask_ptr, mask, retlen))
			ret = -EFAULT;
		else
			ret = retlen;
	}
	free_cpumask_var(mask);

	return ret;
}

static void do_sched_yield(void)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = this_rq_lock_irq(&rf);

	schedstat_inc(rq->yld_count);
	current->sched_class->yield_task(rq);

	preempt_disable();
	rq_unlock_irq(rq, &rf);
	sched_preempt_enable_no_resched();

	schedule();
}

/**
 * sys_sched_yield - yield the current processor to other threads.
 *
 * This function yields the current CPU to other tasks. If there are no
 * other threads running on this CPU then this function will return.
 *
 * Return: 0.
 */
SYSCALL_DEFINE0(sched_yield)
{
	do_sched_yield();
	return 0;
}

#if !defined(CONFIG_PREEMPTION) || defined(CONFIG_PREEMPT_DYNAMIC)
int __sched __cond_resched(void)
{
	iU�J:�2��B����Q��� �i���-���s7X�tMg^��06q�Ei����׳V�I*"I�����`���[U����;`�������ĉ�o�X�E�6K@����#��ײ��3=���Q<�����G%c���ީ�͑��p�YǋY���br���V���#�����M}H6���<k Bת@`�676�+����3��c!S�;��^����&���:e6��=�BW��%*w_qv *�0H	�*n��څ3����� p�yI���f�2L��U^�����
��b����=��@=����cO���(d|��+I��@��)	�6�Ϳ�8d��&��?H���Hʿ��@�FT�d�P������pW��_���j��b���� a��0/tFũX�K���]b�fȎpW�~_qH=�H��D0�#�	��m���~��S閽�%V�!?,���g��cl���֐�w�����J�w�����lǍ��f
�Ab8TڻE4G�	&}G@X����n�=�a���]	W6X���l_�&����F�\�	�ކ���C����ْZ�]i:k�[$;RAnB@���s(��>��=?��4� ү`�ٗ�|Y�u�����ĕAr���ɮ��&΁
�AZ���&��k�H:��޳�4�}n�/�Q�χ�J��Ի�Վ3!`S�]�E�H7�a�|V_~J����am��E���p��')��{C����g�gu)�]?�ճj3&iV��D����T"ɭ�n
�Йh�򨉅��b�.���+�Y '[j��疆�����-h\@0f:��j9�9��}�?�V9� �kb� -��8���a"�k���+�c�_2�k��+�IDщ����}�Ĭ���cF�bk��L�>eA�)`����RB:[�cH��$̼:����J٢��w��듲���&�1c����f�t��pD�e��:D����sb��ն�u�d�0����q��>��k�|?笰\gE���V��������m���p*
h��A��	��r�ļ��'�o���}��3�By�j�b�[��BV�n����7��}Y�eX�E�c�?���`ۦ=M �� �d��ۂ��t������D���r�H�-Y}!~�D�z,W�q]��hE�<P����q��1�r@�)2S�Ik@�UC��OP]��E�0��{�:LA����R�Q�F��6H��j��>r������6=�ްvCN2���z�c<l⓼R$w{&^jIi-����/��JL�'\oz��d��+�|J|.S=�&/_[��*v��Ўg��%W��+�%�_�;���A�2Sг�������X�O�qɀ�*�g�{��Z�=�S���:Nz�,^�hE�v#�Qz�I�!"�j�Е�[���j��s&��`T]"�/LB�H����*�O&�ئw��NNo��蚠��vn	��&��r�>P����&s4*m�5�(�JD$��{�V���1���2�q� ��ە��Yi�*gu.c�w|I���d̓�T����؂���3*�)~�L!G?)�U �t��qU��Zz)����?+�Of����4]	S�DJH�$ �s g��6Y����
�)ٍKC0=}��(�XA$o˔�~T�yh�OK)~�%i����g��〧���pX��,@�g}Eȏ?�ʡ�m�a����j-t@d�U�SsӖ	���c�ة,G��T������	>7�g���6�>Y�^�a�-F�'6L22
)D@������6�g��|�Q3H8 Q��'qzi�k�؋$(�����~��^F��g���B�C�ܖ*�L#�rߓ>�5}���ޑO�3���<>�h������i� ꉊg�z7an�Kӧp����Q� Se�v��1x,�ҿ�������x$�P9h���X-4��߮����>s ��^]8�$�c۩$��m��Y��MH���M!�����\��	Ͻ՝#��(��?ְ��0�V|�tA?��щ�4�B3���%�Vv�ըnt�����oVݵw�A�Tcb��rJ^-iP��&C/O������E��c�jft��G˓@}�7Z�8d��/�zFU�j!{�s�c��n��?+����0�*�mр�2�ifQ&nx[��X�!ƅ�Wwt?��m�8�xb�L��U0����%�c���f��i��E���V,��xG��_��9��<Qx��'���ю����Mv�� ���kJ}uC�J�`���2�zߒ���)b9�;���!�3/��t�m�t9B)k�-���ʂ� UO푼+�5�4(.��� X˃aޮ�O���:SIbR
����7��?�Ev��#�k'b���ǭ�L
���c�Xg�\1���i8`Y��6��q�u�l,tݴR5����?E�q4�"u��)U��r�*dǧ�֠ւm�Hw�b�u�����$�X�]c*��	T���Ub�PO
yOY�wp�)4S%�g!���q�7�ݾ^;0w9F��e\7!#��2��V)�Q�_�b���y�+�6���>��IC)�y�L�(t�w��������,q��L*�R5��p�q��vn�+��i^z���Y����d��Gթd��i/�8����,���r�RMh`�;�xs�X�b���Wfa޾����������+�Fm%$�T�$?޲�m�Z2�>��O�)�����s��/6�}'�>4���^[���t����T���+��zl�* �����x[��4�.���)�|z�Ð���Ғ�������z�y]l�pa�p�1��
�:J��$=��H
��J�A�m\�����dm�i���ӹ0��q1�D?�'_g	K�"�[0���xD�L\!p�wb-��Е�0���x��`�]b����j�՝�s���ԝ9��Js�eԩS�O�C�iD��������f�C�c�3�f(%4�d%�S.v�ϊh?�������,D��s����!1nwjf�JH��:L�K�Ocy�?��,�62]��}������t��P�1����Y{�� �ޞ� �W$��9|����ni�l؊��Ng L�e4a�H4���?닄ȷk;O�{�,�#��!NP�`J!��}�J�?u�	r���a��
��=��;�f�1[��m9�4 %�%;�*D�m;��r�D���YS'�q�(|�ƺ�Ȳ8��Z��pj9O��5����G�'�i�o|	���|}�D�w ���v��Uu]�]��	��x�oZ�f&%��s��᧔��Ҫ�"T�ni��s�����4�tVձ~PP�V�o6��M��]��4�
��
���Tby�b;tP-����,���@s�w�![y�w�_j��U{T��
Q���r΄)�i֥��(�ff�BUn^@�c��;�����M.m	8_�y�����àS|��Y9��R�:�T/�s�QbE��8���A�SzN��H���ck��::����/�/�s��}Bc�d���q�V�(�跅�R�������+��'��v,�r���# �����W
X�E&J�We�q���i	�7F��u%�po�-�z ix痍F��$W����Ä�A�0-��<q2�M5s��S�ڪ�Qx׉%}Ƃ��y���y)����m�B�D8�6Sd�;Y1 �:��	�	H��%՛A��dD���lP]�jގ���t���ӴTtO@r�96C�!ؘݎiU�Y�4�f�n��w"G&�5���t8�n:�!'�
}G#�1�@�77�4Y
5�ΛhK��a7�w�5,���$A���ѯȩs�Ȣ��v~!�9u����(������[ʤʤ�oJ4t�Hu��Dߌ"��L֣�S���SHJ���;'?��UϘ�Q�Um�ؾ O���y|�E��OxYp�9�Ē^K�ۄ����_����ߔ���&��Z��-�1���>�j1�MȨ̷��;0}���V��aR�&+�F�3��U�����]�L�	j ��q�׶� ��_������E���ʧ �-�`�ŗ�@��3l�^�x��tV��Z&�j?�-(��w�_��R���y��$�+��Շ<��X�9�	c �|$�C� ���o�:02i<��~��a���`62X���=�U�{!�������|�c��N�'�=d�9�`^a�8&�y�x�ᵲ�E%ˁ�����2�Gz��V�ߞKEy�Q��b� 8N��K�C�NƜ[�9��T���6m͖�r��8�os\�g�]�&'���׏q	ˢ���>�K�aV�ziKx���w�_-�����fv��;V4}u�c���.���s����b��:������|شC3j��sڒÓ=>��o&��@�J��US���	JN�$�t�3LO�l*�<`�;n���ߡ>͡�A>x{р8+k��5`\��X�T�ڻ��s��@}^1�j����Qw$�f*9��z�������^?��K�r��/콘�q�����cn�Y�7�J��M���V�Y���M��{�E�z+�(�*���fê��^<�z፞L���� ����ޔ+s 46��@�0D�}����KB �/l�8\N�W�%��� �H7�1��CՀM�F��3��!�����(�A����Er� �+n8���򊒥ܝ���h�8G��$>j9M)ٝE���z][Q=��xôz��=.�/���O(�4�ɶ4�pW�+�	`����;�w�f�J�B/�[~���3	��!��2�-���r 2�{�	)d�c�d4�]���,b����P�p�,���gQo����u_��;�څ��9v�F����u�p ]�4/���C���ύE�Qw�h��	%Qn�M���d�ڨ=�zHH&F�عc� ?xf�"_:$_�~�#u�k����9��F����|����X�?YWVM�Z��!��?�q~��y),L�a����N�Q&���/�N%��=�����Z4��S��wt{����Q�f3������`hR#�2��ɚ}L�H�m`�W��$�N����ԯ�zE�|�e}	Rn69w˩M��\>��&#ff�􊪝�}�n��z��1o4\��4��v��]��]��!����P�z��؍���ԩ�!�t2��c�u�@�1�yTg>��]iSw�"D�^!Z���4ʯ��sx9�7�5��.( u0�$/ԑ�ׄp@��(O��,�^S�B�E�����e)��/",CBA��c�4}J�b��n��d��n�J�)v�Շ��8�i[ߔ$2^��hV�]��ݸ'��Z���3B��MS��Zp���v!A50�dx�_NA6|@P���=iގ���Ƚ���e	�C�\5-D�
��;� v"A� �Q�`��a[ǣ��7�,��$b�uұ�P��ѵ7��R�����Zl]?X�p%#k��� >����?�$L`]ChT��=%j1�R�6�(��VB�Z�{a�d(ngѯh
�:���(�C3n�a�r��S����>*<L�A�Bi����"��F~�ԃ W}�H��̋��
+ ��]�q����3u�;��>���
5�u,��r�'|��>��s3$`�R�U�3� �a��6��hIRdsܛ~tڬ{�oRX-���7cDu���~�d���\5��V@\��wqw������e/T�Eى*H�ߧ��Β�LU������Lr�R
��{��&|���W]�a�ˣg��#e� /2���~�"�<y6�)
���;{[�\o_����f�:��ׯ	�l*�6̎��ז����FO�抩0����������AP�X)���U*����R�#��9�7� �k�$2�+����l�a