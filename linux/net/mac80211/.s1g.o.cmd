d domains by considering the
		 * cpuset configurations.
		 */
		cpuset_force_rebuild();
	}
	cpuset_update_active_cpus();
}

static int cpuset_cpu_inactive(unsigned int cpu)
{
	if (!cpuhp_tasks_frozen) {
		int ret = dl_cpu_busy(cpu, NULL);

		if (ret)
			return ret;
		cpuset_update_active_cpus();
	} else {
		num_cpus_frozen++;
		partition_sched_domains(1, NULL, NULL);
	}
	return 0;
}

int sched_cpu_activate(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	/*
	 * Clear the balance_push callback and prepare to schedule
	 * regular tasks.
	 */
	balance_push_set(cpu, false);

#ifdef CONFIG_SCHED_SMT
	/*
	 * When going up, increment the number of cores with SMT present.
	 */
	if (cpumask_weight(cpu_smt_mask(cpu)) == 2)
		static_branch_inc_cpuslocked(&sched_smt_present);
#endif
	set_cpu_active(cpu, true);

	if (sched_smp_initialized) {
		sched_update_numa(cpu, true);
		sched_domains_numa_masks_set(cpu);
		cpuset_cpu_active();
	}

	/*
	 * Put the rq online, if not already. This happens:
	 *
	 * 1) In the early boot process, because we build the real domains
	 *    after all CPUs have been brought up.
	 *
	 * 2) At runtime, if cpuset_cpu_active() fails to rebuild the
	 *    domains.
	 */
	rq_lock_irqsave(rq, &rf);
	if (rq->rd) {
		BUG_ON(!cpumask_test_cpu(cpu, rq->rd->span));
		set_rq_online(rq);
	}
	rq_unlock_irqrestore(rq, &rf);

	return 0;
}

int sched_cpu_deactivate(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;
	int ret;

	/*
	 * Remove CPU from nohz.idle_cpus_mask to prevent participating in
	 * load balancing when not active
	 */
	nohz_balance_exit_idle(rq);

	set_cpu_active(cpu, false);

	/*
	 * From this point forward, this CPU will refuse to run any task that
	 * is not: migrate_disable() or KTHREAD_IS_PER_CPU, and will actively
	 * push those tasks away until this gets cleared, see
	 * sched_cpu_dying().
	 */
	balance_push_set(cpu, true);

	/*
	 * We've cleared cpu_active_mask / set balance_push, wait for all
	 * preempt-disabled and RCU users of this state to go away such that
	 * all new such users will observe it.
	 *
	 * Specifically, we rely on ttwu to no longer target this CPU, see
	 * ttwu_queue_cond() and is_cpu_allowed().
	 *
	 * Do sync before park smpboot threads to take care the rcu boost case.
	 */
	synchronize_rcu();

	rq_lock_irqsave(rq, &rf);
	if (rq->rd) {
		update_rq_clock(rq);
		BUG_ON(!cpumask_test_cpu(cpu, rq->rd->span));
		set_rq_offline(rq);
	}
	rq_unlock_irqrestore(rq, &rf);

#ifdef CONFIG_SCHED_SMT
	/*
	 * When going down, decrement the number of cores with SMT present.
	 */
	if (cpumask_weight(cpu_smt_mask(cpu)) == 2)
		static_branch_dec_cpuslocked(&sched_smt_present);

	sched_core_cpu_deactivate(cpu);
#endif

	if (!sched_smp_initialized)
		return 0;

	sched_update_numa(cpu, false);
	ret = cpuset_cpu_inactive(cpu);
	if (ret) {
		balance_push_set(cpu, false);
		set_cpu_active(cpu, true);
		sched_update_numa(cpu, true);
		return ret;
	}
	sched_domains_numa_masks_clear(cpu);
	return 0;
}

static void sched_rq_cpu_starting(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	rq->calc_load_update = calc_load_update;
	update_max_interval();
}

int sched_cpu_starting(unsigned int cpu)
{
	sched_core_cpu_starting(cpu);
	sched_rq_cpu_starting(cpu);
	sched_tick_start(cpu);
	return 0;
}

#ifdef CONFIG_HOTPLUG_CPU

/*
 * Invoked immediately before the stopper thread is invoked to bring the
 * CPU down completely. At this point all per CPU kthreads except the
 * hotplug thread (current) and the stopper thread (inactive) have been
 * either parked or have been unbound from the outgoing CPU. Ensure that
 * any of those which might be on the way out are gone.
 *
 * If after this point a bound task is being woken on this CPU then the
 * responsible hotplug callback has failed to do it's job.
 * sched_cpu_dying() will catch it with the appropriate fireworks.
 */
int sched_cpu_wait_empty(unsigned int cpu)
{
	balance_hotplug_wait();
	return 0;
}

/*
 * Since this CPU is going 'away' for a while, fold any nr_active delta we
 * might have. Called from the CPU stopper task after ensuring that the
 * stopper is the last running task on the CPU, so nr_active count is
 * stable. We need to take the teardown thread which is calling this into
 * account, so we hand in adjust = 1 to the load calculation.
 *
 * Also see the comment "Global load-average calculations".
 */
static void calc_load_migrate(struct rq *rq)
{
	long delta = calc_load_fold_active(rq, 1);

	if (delta)
		atomic_long_add(delta, &calc_load_tasks);
}

static void dump_rq_tasks(struct rq *rq, const char *loglvl)
{
	struct task_struct *g, *p;
	int cpu = cpu_of(rq);

	lockdep_assert_rq_held(rq);

	printk("%sCPU%d enqueued tasks (%u total):\n", loglvl, cpu, rq->nr_running);
	for_each_process_thread(g, p) {
		if (task_cpu(p) != cpu)
			continue;

		if (!task_on_rq_queued(p))
			continue;

		printk("%s\tpid: %d, name: %s\n", loglvl, p->pid, p->comm);
	}
}

int sched_cpu_dying(unsigned int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	/* Handle pending wakeups and then migrate everything off */
	sched_tick_stop(cpu);

	rq_lock_irqsave(rq, &rf);
	if (rq->nr_running != 1 || rq_has_pinned_tasks(rq)) {
		WARN(true, "Dying CPU not properly vacated!");
		dump_rq_tasks(rq, KERN_WARNING);
	}
	rq_unlock_irqrestore(rq, &rf);

	calc_load_migrate(rq);
	update_max_interval();
	hrtick_clear(rq);
	sched_core_cpu_dying(cpu);
	return 0;
}
#endif

void __init sched_init_smp(void)
{
	sched_init_numa(NUMA_NO_NODE);

	/*
	 * There's no userspace yet to cause hotplug operations; hence all the
	 * CPU masks are stable and all blatant races in the below code cannot
	 * happen.
	 */
	mutex_lock(&sched_domains_mutex);
	sched_init_domains(cpu_active_mask);
	mutex_unlock(&sched_domains_mutex);

	/* Move init over to a non-isolated CPU */
	if (set_cpus_allowed_ptr(current, housekeeping_cpumask(HK_TYPE_DOMAIN)) < 0)
		BUG();
	current->flags &= ~PF_NO_SETAFFINITY;
	sched_init_granularity();

	init_sched_rt_class();
	init_sched_dl_class();

	sched_smp_initialized = true;
}

static int __init migration_init(void)
{
	sched_cpu_starting(smp_processor_id());
	return 0;
}
early_initcall(migration_init);

#else
void __init sched_init_smp(void)
{
	sched_init_granularity();
}
#endif /* CONFIG_SMP */

int in_sched_functions(unsigned long addr)
{
	return in_lock_functions(addr) ||
		(addr >= (unsigned long)__sched_text_start
		&& addr < (unsigned long)__sched_text_end);
}

#ifdef CONFIG_CGROUP_SCHED
/*
 * Default task group.
 * Every task in system belongs to this group at bootup.
 */
struct task_group root_task_group;
LIST_HEAD(task_groups);

/* Cacheline aligned slab cache for task_group */
static struct kmem_cache *task_group_cache __read_mostly;
#endif

DECLARE_PER_CPU(cpumask_var_t, load_balance_mask);
DECLARE_PER_CPU(cpumask_var_t, select_idle_mask);

void __init sched_init(void)
{
	unsigned long ptr = 0;
	int i;

	/* Make sure the linker didn't screw up */
	BUG_ON(&idle_sched_class + 1 != &fair_sched_class ||
	       &fair_sched_class + 1 != &rt_sched_class ||
	       &rt_sched_class + 1   != &dl_sched_class);
#ifdef CONFIG_SMP
	BUG_ON(&dl_sched_class + 1 != &stop_sched_class);
#endif

	wait_bit_init();

#ifdef CONFIG_FAIR_GROUP_SCHED
	ptr += 2 * nr_cpu_ids * sizeof(void **);
#endif
#ifdef CONFIG_RT_GROUP_SCHED
	ptr += 2 * nr_cpu_ids * sizeof(void **);
#endif
	if (ptr) {
		ptr = (unsigned long)kzalloc(ptr, GFP_NOWAIT);

#ifdef CONFIG_FAIR_GROUP_SCHED
		root_task_group.se = (struct sched_entity **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.cfs_rq = (struct cfs_rq **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.shares = ROOT_TASK_GROUP_LOAD;
		init_cfs_bandwidth(&root_task_group.cfs_bandwidth);
#endif /* CONFIG_FAIR_GROUP_SCHED */
#ifdef CONFIG_RT_GROUP_SCHED
		root_task_group.rt_se = (struct sched_rt_entity **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

		root_task_group.rt_rq = (struct rt_rq **)ptr;
		ptr += nr_cpu_ids * sizeof(void **);

#endif /* CONFIG_RT_GROUP_SCHED */
	}
#ifdef CONFIG_CPUMASK_OFFSTACK
	for_each_possible_cpu(i) {
		per_cpu(load_balance_mask, i) = (cpumask_var_t)kzalloc_node(
			cpumask_size(),3��T��q�zp��D��mTI�r?���Λ��;��z���y��r��E,f7P$	l"�x{���Ȱ�u�8K�^, ��;�9�r�Q�8�n������Pp��Ǚ��p��؄~_�/�S�����<�q��JP,�'B޻�#ݞ��2��]?h~�I�L��(�㭑��,o�";�3,@6����kxσ��Ĕ.
h�lUd�#����/P	PA#���l��{���=���(.��Si2*�<�������Z`��Tz��)�-�df1�r� �����P����yc�Gg屬-0�??������ty<��t��5!��c`����Y��o�����9��c�E/Hrn�{(v�H@��6�����c�'���r�'��^ڻ�*�lL}�џ&#���7��R�
M��|�y��'��U;��L�*M��x��"�n��+Na�3-ݻO
��/����
�a
�W���;��n(F^��y�����;O���N�x1zqԶɃ��dfn������?˚mrN�4spk� $HY߆<�R��nnr��\��9�2Sߡ"�k�X\݄��>�n���6���K2�ה�(R����\Q2 i��O>����2Fi�C;�.{3����!�(�4�� c*h��J��Q�^*��q(�b�,"p��m����p�
M�<�uGI�FV�&�Q&�O������$��9D��.E��c���ϼ����4�zE&�	��g�ָ���,[Su���^�R��}J�*S_�#՛�����-�
�cv?트3�5�R΂@��3�#s��0+22���)���u#�;�����a(��q�o�A{) XN+7kV[�����ԗA��2a!��j������V�+���_Y���)��d2���^����r�lT�4��vQp�_���v�h�YHӲ����Ek��=vr���!BS�L?��i�@�d�>�L?�x���m��O�D���B/'a��)�ɧ�Wϧ�?w/чR2������S	���KLX�T8��|����U��^F��L�Y�O����ӈ�Ăo�5TUǿݺ��L�a#� T��@��#������<,A��+wyK�kߡY��5���p?�T�[���������O�7Q��~�+���!%� (yH�S�g�4��1^��M��:��B��
F�L��$��Gt<�B��Z��n^��8U~�[���'5
0�Cs_��Ub�����}K����}Br+3�2Ǘ�p+����]-�ϵ�qژ��Er��r"������MrMk͎�
��� ��#�&d�an4��Z�#O�����T��k޻���G@)bkiY�a���X���&����j�\�ӭ��+�?�Ko���wl�5�D��K��P��:��d����Y~Ŭ6�Q~_|����o��K@BT-2������x�9g�E����q&Tz�	(� ����,JЂ�"�5�x���_�QN��κ�:�=�Z�Y��,��k'��"S�)��H`E���J ���Ҁ0�,D}v��HzMD߸�Q��g���TfGtY|�)*=.l��XP����хO'L�yi>�UR������/�rz������υF%�/�>m巙&}&����	�9����F*:Aw\k�U�^��M�<��	��������y�����1�I�T{��dN��������~e��>>�\�����6�ejgn��t�g�������e^�����5���N��峉.�+���Q�L�C�;
�O�����4��E�1p���MulZ�Q�˄�V�縃d
���҃���`E��T�ׁ��Y,�̂k���/�s�m#ح����9�Av�v��=��S� cPzߜTT�>B�,b����5�Qm�s��wyW�r7?!ʓ�oxr�eR�B>�q�J5
�=b���6�N�p�Y8���F���EY�6�qx�H�T�t�_::�d� k���W�&�lU�<8���7x��v6��
�hL�G�5��y�����o��D=�Z-{-��i��7,����S�92$��t�n��F���ly3!@�5�٨% &�h���3��{��.�����9R�T o%yq^c&c�o��z�n<a��᜶�H<:��:r��7�]7�>��p���wXܭ��U2Vӱ�X�a�$��#2?�<	&"}z��?h����}��:�U���f�]}[>-:>ME�����}�#�}���l�ņ�P`qV����>ߑ�ہ��Z���6_sE��\42���$� �J4cI��N
��&fYKJ� q��<���f��B_�Kv�ezymJ:�Z���UzzIxڮ��7�����wKB_�����#�sL\<5PD/�@��-/�Ɵ�6_�뛍98|6�Hc��]�Wù�w�'��Z9 ����`u�.��J)]�
1�;/�yN5.Gp�{b+��*,�׳�.v�|���|��I[.w���`�(�<>vpe�\�n:�S�q����L���H
�፻uKqb��.%.�W�����dԊl�_��t�ʌ������)w�Lӈ����(�x�o?�P�4�6��uU�����kun�m;���� &\l��']�0��S�d�`m�XP�,p1�Q����:��W;}���v�2��^��ce
�o������[) u���QVȑ�ٶ���Fܩ�;5�$yf��UB�􌑁oD+��0@$�@�;���D�B@x�3.�]�>f-�e�%�T{�ɶ���e��*��a�؄���+a5�6�Lo�8�z_aES����Z6��.T #�CU�*�
^��W��=��A���(O�%��E�i���Z����&`�� ���v,@]�v��^U��="�zM�;�T�ߡk2��5�ڎ�K6����	��u�>�x�c�$�C��1)b�5 a�����9�.� �r�'�5e�<(,���~u����Br��sf/x���.ȁ?�LCbo���JA��7�s}$eQ��N�:H��Ĉ�^k�~79�@k�sv�{3�ɇy�TM�6u�����,]�ER+c��+@�a �U�Pń<�
oe�"su��@S�����u�>6�iy�t�Ki�(+a�Y�w�WF֩�r�i*,(�)�E���/!B��z���������`���mqYD\3�

�A�>�l��VZ;�"��B�_��Z�>��V�@�}˽G�0S�ۧ��QK,��-c�[� �rw(	��v�ӆ~ n�솮n�k��Њ���f)�n���[1�/z�|���b�:^���z"��T�G�ҹڹ�8a:?h���y��̨q�2����I� N+����!���>MvFc�F�\�I�'bǷhkj��j�s7�O�$O%��@�stKh�x-���M���kX��}�/2���@�������*W@�LD��ԙEp�0}�+�w2Lp��2�P�,*w}R��>ݯ�ጓ$�����ɠ��)�E=|t�j�ilr&�0�8_<9c"�=(��O�!�JRN[����k#���-�վ�Z�����P����9|��7D��`(׎p=�q����$2tvޣ˴2r.�}���� �T���k<-�Uz��7��[����m�@X	�Cn���n��q`})�������w�kW���juè���o��i�1�H6D�UB���ر�d_��tۦ����Zs]�6g���.ԿR*�N�]��0lR�v�'$>k6�b��7�9C�5&1���?����a�}��Kh�
  -���(�,�#jv0^l����i��'H0�.r#�TXgÉӠ(4N��q�Z�V��4���I��|n��3�O��HQ�v�^J��y)ٖ���xX��G|;�T��@�����DU��Ց�r��J�JoS��Z��,e���d�ܒ��d�=*�-����q��yP��-��R���8
T>?xZΖ����F�%\�NC�VA�m�������[Y�,���7��W ��j�k�R��������E�k>,,Qy����!_: �.��C[k��Xf*qN�R���P��N
�0��n��tCᐎD���A�5	}V�:�P�������j�x/��5O��\S��Ǒ%��� e�9�4g��0�ѐu��"W���dLO��s��I5%�9^��2�b2�pG�	��؟d�(�	�WN��,$��s���/T���䎅�~*��j&��E�hK�V�3�qp�G�itF�	��;��-_Q�o���m�xӡ�;�h�����Y4\������%��	�eL~7-S��������.��\����٨�����`hc��G͢�c5�զC��F�R�Z+��Y�1@����9�efx���O�-��H��D�e/V�sT��f�����3ws�ί�P���]�^_Z��э�!�݊�������^����A�pL��8�Al�=��<>bn�Y�՗��7Ssn6���ǁ���#^�dc�m��LW2�|��3l�X^6S�5�:|��[�>H�1�]� ˲o�MKM(/E.�$x�e�)O]h\��{_��#?ڀH�IsR�
8/�j7��E"s<���l���2j��)�G�e�=��j첈�����f!4)Wn$WE;h��ݢ�
�:qM8C��H�#���UI����d��c�9�jҔ�!��7�4)����E�������d8jU���bsĶje�$�`���Ce�<�=֟&��Mh��?nԢB�W�!��qZE�����)$s��qH��X �Ӌ��5k��-�m���"�'K@��=��v�§�G���u��������G����:��ߟ�]��yO
Ӕ���:91e��1&E47���ǋ)�f������|�SB���#�=S��QɄ�qh�6jK;�Dޟ�"��"@��D;���q��-����K|�!��H�$+�6�T���X�XU�Y����3�HE�W���k��!T>�J��/�4^g���e&D�S��$eU#��;.�`����*{<ܽ'���O�4���g̷WP��2̘&�l�`|��5�"r`5�+<�'��ڂ]�H��n͎�m��a��Z�� =-���uLl;�f7ʱ�xs��qOȺ'�9�*�Aʽ�>ZQ�{ʁ4��t��Pٵ�������(^�HL��u�f�� ��;[�X;��d˷�M�@�f7�ӊ?C:;�r�|!կ�
xщ^ء�������i3	 Ը�@<�>���g��}2�&����`�QL� k29&�y���;�
&��7;6�����K�����&�Q6�({)¦�bk�:������<g���׍��w�j~�l'ǫ3�в��_�e��~T����U��_��x�RJJ���I��^#4�V�9G����x�{�0�^d�L�qO��^��d`>�Ն$'��ĄhL��( ���707'�mo�)E�C��P+w�@��T� ��_���+���V��V��-��WL�~O��{R	�z6�s�FoR���]�3^�'��(�:5�Vd]l���0���K_;J�;�q�� WƎ��y����\I�t�r����o��50��d�����^����U�.��?�f�0�	�T��A�?�wHl:�����B�9���:[]��`Ō��|q�m]�'R�@71��7HO�*�+R}*EM4���%P-�W��,ˤ�*_���u�Ns�U�ij�L����}��FN��] ��vf��k&�_Q�ǆ�Ds�?��t�Ų)����1<����n�����ȩ�Q�0�H8/�\򩼣f���g���rm��m���z��u���>���Q�O��Ħ���R����b�g'C.�"{�M�rA_�Of����#��յeo��ќ>ǯ�,Qq;hy��(a���o-$���b�%��ґ-�ͭ�-��|�E2cf�eݟ�-�
E��0��oq7��7�3,�� �Q��e�E���T�uA$ҧc�k?\V�����2Ҵ��4��������C��=�;(�־�0Y�����'�5|��y7���O�y���r�&E�蒂���k��q��^9�y�8��D`�i�	`$`wmr�.��nx4s�A�v��Ӫ��Eu
�!�!��I��:����Y��� �u�4EȦu�/��,��UURʼ[Ǒ�)E�/1����&5��-�-'���1��|�2�P�b���ܿ����i_"�6����(��7����x��D�w�S�Q��#uN}C�a��]����ԥ���I�E���\�f�2:Xض�)�2��N�/���}ά1����"���KҔ����V_�(ֳ�3�ѳ��tH����ŷ��"Yq-��u�M��Q�����g���:�>��f���T�= �</�Y1Y��s!cT��,�^�8{W��ł������HD�~�{
s���x'B��i� �+*��p����F�D�.��Q�E��rǑ���<�����Z�5�S�O8b��(�aTL��k��1�K����S��W�4`-f|�O�[��[��Y9��<��\����%��-�3�8"��	�$�FsV��*��#(��>!E��B�V�M�aVDH&a?'�l+��cd^.�s넀�]t
��"�!��1�
L���g`��R9�N���0�7C�l2f�XG;q����0����`ҿ���ޞ��Q�~�ύ3�pz�*p	��+	���7a�=R���h�)�֩f&����."q�ji����Y��[�?�n噂�e�sY��cM�B�k֚n�yϲ�p�ћ%�x�[w.�1��
�%�)([`kB��4���x��ȣ�����,+��6~��)�u��P}��0P���K�2wK�o��Cj��(�]�D�ڋn�@䅐�"��+,��ܒ~X
�����u��y]6+�U���h�J�N~I߰)�f!(�ϼ��� �D�n[�RY��{bu���+'��͵X�W	O�lLS��0"&^X��o9�2p0�h��r�WKB�8 �l1��,���o�P�M	�׈�$��8�vt���&ì�οb!3#q���(��������T����� .����"�:�є|�a�$I����	��0s��G�M��H�5���}b��'�o�/e0�=���n�!��:�9������ʪ-I��淆>�شJ�:(�7K�qU����$����^)ީ�QɅ/��	l�R�XS�Dz�#+����d>H���_��J�<Z�2uSF��L�#p�p�Q���nr��j����������`�C�Ő��A��C�[_@by�!Bט38O�c��i��Z�γ����PD��JC�G�HIY��WB8� ��,�W0�,O�&L;�Eڑ�N*qBc�͓�����
Z�(�T6�
�)�F�b;�)Kc�(�������G����q��'I�.=��; v�zg;������K!X߿��ތ4q�c�s
��VP;ԑ�-W#�c�C�NbH����-g������$��\-���a���� �J��}���W�G���HYfM�2�s����^�ʂ��t���j,W�mKTxk%��]vzm���i�R�F���7���9�X�g��(11.D�����"��zϴ����d�n1|C_�ew�pn���鸗?¦�Na�&���^��~�������9���^�&��H�'����A]�K[�ͦ�U�k�����Yd��8��D�M�VX��~E_׆ޜ��#a�6u`/I�h�H��Fo����$����3�%)Z�Q�) ���*W����f׊M[��G.pg�Tp��9�ɰ+��Ko�,9�Qb�w�<��9������_�>�_=y�Z�+���W���!dU��=����Ҏj��l#1�����@�E`,��ǜ�5�HU�6����N�mVV{m�G[R|�j����1�)�T�d�*�pn!�r.�YYkW{�9A��.�,Z �P��<�+3B��(=!<��Z$�'�,�[�~��0̓��3vJu�u3����a�و�QVW�5���H�3���J��?B�۪�7�l��i!� �*�t��ʟp70�d1�~8EMX��� ������������@�	YV��6-�qm��ǅY1�7Mb�ܮƒXř���7qKX"�p���,�>�&�i�W�H+�3�]6C�������R�(VAkδ���s���ެQ�9 }}�;�#x��.�䎧��JB�x�r�>�*��h�I�5��+�Q������䷕��q���@}yaj(�QU�oG�3y����)��iZ��w[ſ����@�����<X���!D��Gn�#�8ϓ��x�Z"�{�m�z�E>���\��v{,/69�p7�^i�2�ޭ_��K��s����;d��U�N�ȗ�
O�zM""e�iծ�V�:N20�o-%T�c=���euDF��KNW;?�/4b<�=����˭���e�)��d6l��h#�_�6�H2��w�bn��u�n[�2T^v䊁�D�)	����(r�XuE�15�~/�|���8t���!\�u'm�of��\�
���sB2Aä=І��f��=�3l�rk |>i\Ԑ
�2��K��E_D����oqA��TH`_���4�_�;{ ~�ʗ!$���W��z��P����|@=�ǽ\a��g7��DG�f����)c"{p^�����3�'pΧ�x'Jp����3�8. Gk�O�#�f�g3�zm���xAŜ���j�����8���Ɖ\IH뷲~`cf���V���_��^��Sdi
�\�m�q-��V{	�X�HŽ8�ǈ��@m�rP�M(���$�Q����а� �͉آ�#�L�����F��>��M2���o�r��N��4�$���(�(*s��Wuz�j��h���H���f��6��]�K;G�������s�rEY�yA��$݌ڜUl^�����,m��9�����fz���*�/��]j�8lWJ��t�$�ø� �~���`���[�"��ڠ����1D��0��r�ԥ�4�b6��E\O0+;�n^��9��J�@*��>m1AeZ��k~�>�o(��g'.��B�18���)S��j��@�3�+����W���k*��\������+)u���㵞��NM8�9!�g��X9����e4�G VM?f�lc��뷝L,s���f��K�N�j����_�
Uw���խ����g	�">}�5�=V�d�^��R^���� ��>�H��9v�p�Ia��(�B6uy.U����aM3�2�Ӆ~r���9.��1AG�$U͉ƙ(ݦu��Y!{�ɧ��
���O~�o8:A:K0�%`�:6V��a��K��73\h�]��t�-�\�"-쿺H�2��δk�2q�je��C8�P�q��8�R֤��}���"� ���"��a�b	(\�'T/3���ً�����bA/q�%�x���pg�:BE[�Z=���0� �iS�%�a]��p�F#�@pz��n�Q�ց/��Y�[޵H�38?z)Ĥϒu�N�$S��0ٚ���ճ�L�%�3��� w~�K��Q<F�9M�EGe`���?V꒱ZLkŇ�![4i������b�,��X�˝[��]�kkZ�}M<�����tߒ��.ԕ��,sQ g7���J�W83�<	��|�����H�_�<�������|a��6�`�6�p='���P"��+��U!$��BJ�8 ��Wa�qу9�"RA����Xj��`g��S]�Ol�ǝNZ�&}�%Mz�p1�)LC�c��C����z^�[�� \kycŉ�c4���_��b�Z���Hm�.)+?}f6�y!H#�d0�@�`��꼈������G�pdiB���"[���}��SЬ�5��gD�zo5���-��.%ᝆ3%��������ǧ\dor#��JN��aD��ٵ��)�
��t�K����i��q�|_;�b�$;�m�6����ܻ�x�3" Ė����mK����i�ȏϷަ���.5@�f��T���.�?k�~ C�Cv2*��l/�� �q��C�U��.D��M�8�N$��K�BK�ΰ�rnʴW�으��G�ROeXn�w�[����U��Bҧ�Hb|f���$^a"��F���]�%��*l����cr�p�.(3���탞t�(�L�UCl��E Y����~�k���nCx��͜!<C���ƿ�"}$�f��;9��I�p\3�_���D�XU��_�L��~�������Yc��{k��i&�9��.lxp�+Zg�)_�D�D
�I�*�|�/Kp-iVe!��ڄ+]�������k��C?0�`�-��A�y9�U�=�����,)f�D�Q��t��M�w��J0B�6�=WPS,� ,	N Up��}D�a�li�8
�;]�aAv�I\ߢ���i�3�)�ޙ�?����u��0d{��l{��]�7e�EQ]�������3` W\�>�z�eMҊ�.��O���!�׿uk� }�!�	�{���*DF����h��'EL���\�U��*��lY�ORNsn��l<B9t��L��O�[�myPh�+���MXs�a!�����Ph��n�Ӡ���@�b�<��k�L�4�s箔%[�,�_�f�9�E�l�֒�՟�G�*��:_0g�5�dS��a�}�Q8����J� ha@�{�J11r3E5�iqx���ў�ڐ�JʫD��˅̩��d��<��&�L�a͆��U��WYԽ���r�F��o���`;zSg `4T'A�X�d2D����ZKb�|.���F��w>��d�M���AQ��^�7���穈`�DI��{X�`������
��}KH���<�a�[�~W�{Eu� �:_N`���+"��9s?�@�1K�����W�m��l����	�ks����,�k('�D��)�h@����I�-�(�5ݍ�tp(�
XB���n)�>G��E�i#
H����!UҰ���H2��!���&5Wl���2�$t�� ��b��Hz����,��k�����%���0hVݫ-�z�4��i�xS��n;h&�C��H���/(�
�ư4ret}�I�2c��5��ދ[9;B�n6��/M����Za��hӺT")zP�{�s�f�F+E�D1��-��ajI/0B�;^	�y���-o$�RD���/���Y��=E�	�G����7,�F�^��h�z�E��	;��4��§8!Z���Ё���j>f�G�>S�G5�fz��x�g]�ZLYr��e�` /aB�'U�]ڗz�:A�ґ�#��C��� 5e�吪V�WA����O��U�!��b�<	��c,מ'��l�1n�2�_�.�-�z���I�K�H�%S���ώ���▯e/�|��]��[:T�
���<a�t'P<f����u �Uv���7>��F2�g�gZ����(XM� ����#*����J�உ��|�y~�3���߲�vZ�b|oL�C�8
�L���^ewk��ggG��+ñ[��G����K��B|wB��������O
��l�S��+���W�,Gu�>��HE���iSC>�Q���#h��o�A�jQYAI�W�e,C� �J+cݔ$��s��\���� ��8�<oc��7����شE���BT,�7���*.j.9Ri���Հ[��`�ꍡ2t׫�F��
eC��z	:�d~aJ"�
��M8
ۼ������lL'�%d�u�$7�6�ӆ#�m?�_�^;�������7$������w>$ȕxz��R�\bCkHxЯ],�D;#X�+/���NI��i��"��y�GQ��
[���&���y����n�agG!ϰ:�nW���!�D32�<��x��OIx��֙�;�ۦ���(3~�pʬ���(��^����!VL���Q
,�������L+Q�+Kdـ��g���/(�G�2=^�rC��u����1��H)9a���~)h裙���'�e6y\T�E7�9@�W��a:҆�C�Zm*���H$��b ��n�\ro~.&�����C~�f�	��	�8$�4����� w�,�[R��0+ѿ���Q�&��a����P���!ϋ �?3}O���'�yb��EyE�wA~ۿ�v*�V��˂ȌM�J�@�chȆ�0Hy�F�����~�h����LAt�C"�2n����	~�gZ_��%�Z'��b�ף�d�n�n����li@,��5r��~s�NV�P|5�ou(/���/o�pi�Fٶ�����t�*q�_];Vѣ�qm�U.n
�!A[���b��-���0�̌l���=!��&�[G.\/M���7J�ǭ��H�����h��Wݜ�|���bM!��-n÷��p��h��c ��ƺ�8_�T��8S��Hf�m�	�`�_bL�.?�����q!p�cK�#!Ā�2ՒԈ�3^D �e�Q�����n�����|B0/�XZ	���R� i�I ��jFM��߀⯟{�}}����62���wH*	1���..N[��_��>���y$�x��a�ô�"`Ԫ�h���#������ە�Wb��z����,[&�h.��?��3�+��%_5W�w 孴͔��c�)޺��*��)�86��s��,I��É�$�1�����d�(_���A�4��E��vܛ� ��?@��Hb�* ���bs�+h��V��
ct#m��8��]j=$D�Q��d�fb7-5]G�w
��&7��8`Ԕ��y���W'Ǭ^��Bt��݇p�oX���t47�y���AP-Ru��39e�������3�P��H�����w�[�VLK�Y}��<�?QӋ���Yd��E��&�h�(~�b6_@�����9v��u,S���B����f�!��ML��� 9؂�͸R��U�k�K+�
�/�:�G��I��ӣk���6�����VSH��C(WD���Oc	�@
Vb��%��{{��&�d��<",{tB����Vݣ��`ȳ;/�i�Ƌ���&���ǣ 1-0�4ʽ�<���%�i��e@ԍ�W����#iB��$u\+&�v	�<"�n}zXL�+�^I*m1�\k{eY�Ip����96`��dd��QZ���Qn����^6��'���9�3��Ga�Ȱp�� �yv|�%�\��Y&.�)%MX��{�T�SMP��䈾�A�`T0[P�Te(bB�8�#^�?vM��V�8�E��6�8V��� !�Sή�&�K'ю�9���i�w~Xdn�����O]Q��G�4J�r4�؈�C�`��g�(�>���8yظ��.h�_O�u���� ��G0�����ɝ�7�� '\I�U�]���e,�ty�z`1 ���So˥0����xNZFxԮ� �F�3ӮI���䆣�+���þHy�#>;�m�(B�Ӷ,�AI)�,�z��"�;2��@�s p�^J�?����+������/��HV�V�&�L �f,5��S!�k���o2����k�
\���H��{!�K�&M!F2%�(�t7�I]݃O	 d���w�Ad*��$�r�)aPu1}�غ�P�	���H�j&#^�b���!+�"�*Op�FA�ԝ�A�V�Q1�,OZA��bCX ��링ݤ�[J���=�R��Ps��sQ��ǅ�W����kl�G6�IMXz�w�څѵtx� ����r�^��)E;�Ч�.=Yv|��/J
"/�[`�,����Ut�+k��%��؇�%C���Y�U�:�+�W��B냼�t���ͶKܛ�"�J&6U!z��+2�����m0N7��xd�U���$�I�n��Ə�{���կQ�"�ίA��lb�h�%�zQ��F�n���A�i^7��c�A��O��I�&�A�ʤz�B�Kj��GJ>ܥ]6e����z�Q
�/I�,\��ù��Ңw��&/u�$����FR֒�b�Ίߵ"�O�Ya�o��(���N�����_b��t�Zd+oM�׆t�֡����QE�(["PL|e�f�݅���e|H�A��p�ɸ��CA�)1�j5<��b�b�ڛw_[|�Bj�n��+����X[1nS�h�i*6��19ðal�E���8��Oy�	��>a��(S����V�GL����S�=��W�i_�?�$�
��xy|MI��5A^IKm�	 ���Vj�Ǉ�{ƅҐv:�-]�{T$�7��>[��dDX~��
�C:��s�+W�Z�+|�D� ��r�iX�9���":%D���PYVF
�9�y2�Q�9>EB22Z;vRUg��mD-�M%���
� ʃ�D��z>�)���A6h�MO?�V�B�G5��5�/J}N<���#��CI�)�=�O�Y%��D�n�ؾۮ#�A�{&�Y���N�|1	m�!�N^���m���6����bu�����?���[Ր,~F�j�HY'iU�|�o<����/	�Oiu��$,�
��$�dr���N�Y�J�DqF����J���5fWY �I�TӍ
����'0�8ҹ)�y�Ȋk��]�`��@G�S#���|�1"�a���$�P����+���ƪ��s_�eZ�ہ�i�l��eʾ�,�hR6[���⊎@Έ�XG��
&�(D&9��֊]2�K6��~��$�1$�]*t�gt;T�l�lM,]��r{��'I�� �N-��CCT�#}�DJV`b��/�2�-c�O�:jY��EB#��k2��S��b
�5
B~�:q,� ῘK�bN`�$I��J%�S��/�D7lɱf	`pv��ZZM+|�"��\eL�ћ.����̮y]�������Ӵ�ŬlMXcI��G�Cw�Ed��۴�˵�:���^�3GA�� l�p>5璬L���PzHP�}�l������M�U��D�o��<�9�3{�N&�8T1��5��Ɩ�Rr'��Sy~kbi��nP8�PQ��rB��.�I��T|МW(qM������mQtcqEs7U�V-�o"!c�
�2��P<��7��V�Z�/#�6������S����B�������~�k��_ҷ����lVZ�hڞC3[��~�^��=r!$t_W�I��S٦�9���զ��V9h\�=w�<lT8J�����9��1��¯�G��0�9���~���!�\/�;f�yQ���7���yje=/nKt$Wh\�^��m�6
��e���#�����y<D<j1X�J��o�;�(=ݵ9�pJ���M>��������#4wy��|�ֲ.? ��>6-g�� ��M���*�7�I.@�H��|4b�s�
c�()Ww���>#"�W�UZ'(��]c�ŖU�@t+U���Yě�U���}����H�����y�x��E�(��<�*�v5��o�x�KPs,�\�6;j��V���}1������L�p��OFF=��5�n���+ݒ��Z���	�R��mOh7N�.���Xк�f�F����veZ���P���ו	�.�E٧s�2P��~�KsAj��hdm��\EB�l]rv���d�HIa_Hʺu��A�_[U�1ߥ�(yn>��.�\}��B���z�����kN��WM.�\^|`���<VN��Sr��ȆW�� &Q��}�Q����[��b�J�l��4�x��a	�o0E�¾.k	/ؕZ��t��|��7 ��t^��7��9�mĿ��`�'�6��[��]�NY�M0u3��1P�K����H��O��x����FOI�ܗ_8	�[d�uZ.�J*�c�|b�I��u��N���7&)} -.y5�@LM�R�h׏j�)/��+�Jq���	�D�u��s_�W�Z���){V���c[�!����&�df�S�]�������,��}2
���+�W��ZF�-�q$O���'-�J+D�����Do�R��Or���|���eu�$ѐ��R9�T��zWN9q��EU
�hE�тߒ�]H�6��jc�>")�c��x�},m�#�Ǻ��E��[��=�ܱ&����6$�}�Yu����t��ٔz�]��Na5��]�������5I�v��ric�!#JT�����O���.�8��2طB7�q�np�g|z�!�]H�)��	C)A��@M��Eʀ�]��3�<k�@����f�|�x�=��D�2�<��Y{t'D�8cif���WS���4��-SXE]�(xⷩP����*� ��D����G+!�a��A����s��fK+��]�+�׳>2�9Yk�H�^1��PvD!V+�١-��L3!{6�%�0Q.�z���	���������&tx�x���[
��?|�:K����߃��hXh��<��F9NH�ၷ`5�Deo���G�(��2����C�- �~�̃�Nt���6ȴD*��u5u�T����Q���'eCU4TΥ�0�1_!����*� �f<�OV��r"6����ٕ[T(n�WM�������Ή�I��L{-���*�I�(��%Yo},��oA&|
�X#,�B����B��^B���<@��Bqd�v�Y�V����u�n	F'/�+pI������g�w�x��4)/ ����V�x"6��\�`-���?餂Vg��"dVR'"��q��o<c�u��ɶj* Y�W��-.���z����C�w�}���([5��P�c�d�V��X�Q����Y��p�?P���v��d����}�*Uyv>��5�d��2�K`���|�Y$�M�ƈ<;��oG��њ��
���-(l;mi��龷tOg��¡ϝ��/a�7ۍ�BR4P�;�/I�E�1�_�J�5EW���s����p,u�<x�Cp�oW��[�`�J�57��B%��p#�N��>#;CS3r?D� ��Ԯ燵&RAV/LK�����V�x"B����U���6ȕ��Ԋ���V�M�lh�|�lo],Hz0.�Q�H�#�s��L���7�����-O�Q#9|^�{���l}�H)F�4���78֑�K��R K�@�t��G����B�6sgn�`ڞ�"X�P�u���N�HZSA֠�">�51����
�$}.�D�_#�-�&heKgf���@����|����uR*��s��ٳ,ͷ�\i3r�}A'�*ף¶�
VK��b�)�~ŷ}!]�l=��y�n�{����kՕT��al9�-ڴa�]b$(v��@��:Z9�0,���G�Rty����Z�n��D��do���\"D�&���0$�-	�)��u��^6��X��H%��zy�5��L��n�M&�n�#pE߇�x�I-����H��e� %�~b5��������#Χ-J��]���%{v
��q�<��B�l�u]t���sXzZh�"-���z,�ҧ{�f��PE��޹e0��X~'qL\O2�5�'�*�$#ռ@9�a��Bj�	��@Ӛk�q�¨��b�E�:��*<�k�p<��l*�g����[br:����2�-��6Ҕ�X/�l�U��{��=��n��J{V���&�m�M��&��
�MѺ�`�BW�{���F��9���;�>sǚ��D�f����y*~h�)1ܞ��x
J���q>t(|?8���eZ]�ilYX*�[���Ks�M��&�Q�k#��8��,9�Y�#�z���a�3���'�Zc.}�mΫ��Â"r�K"�f��}��p������J���!Ǭ����Ld�)���פ"�9[��~n`ؼ��,rj��pO/-d��&"��`�deOaB��?/�Gq�nr�D'�R��p{EgW��J���[,yk�l>1�|������D�]��:m�%�:�Y�{�ȳz�~	�����E��oҗAr��Z���R����ɯ
��.KYnp�zn�}
%Կ������:�X��d�Z�Q���0>d�{����!�5�/f0��2S���m��G�FU��A�j��R�PlX�;��r˲.����5�U�,ca`��bɶ��a�J����ým�w���̤e7z#��S��`�w
v�(#.�=�;��̂� aYA�l�Һ��Y2V!�=�K�+3K�ئ�7/f�9�ۭ�H|؎K�RN��1�u���_�4���;�ʻ;_��^��y&)���܃f�u��󽞫0(S�7�L<"�׼�t�����`*>yh�e���w2����A�^�_8sN4�2(z�K=���U�e���K	�	�l�f�v���R��(�L���k�Ԛ�"Z%�8��u�?_+U�X��
Oő�<Y�*O���}|�WvOfc�qOl ��L�����")��l B���*n9/m5/���US�J��� =��,Z�}�U)3�K"C��/D6�M~�v�}B�P�m&��+n�ޮh��#hpN;l��9����� ���C�v2`/W��9�^�1�1������s����omgB=�GEAxCk�|!����no-��(��. �����D�p	T�/T�{t��I�G�N��]�q ���Hg�?v�#��8�:%��+��	����xy�mtH�o90\0r
��:��	���8ۺ��i҆w���v�����ۊe���
��J��!\~) ���Q�A?���F0�������\V�D����-�F��e�<d1�ƽi.��C��N���$Y��9o�������滄�N���}#�@_��X�r/XT&"�<ż#���8����&@2�x䁾��Qo�H�"��5|�k�ł��n�[���̣�L ��d�S	yE�im�?Ǔ_e��-��|��/.HB��BlqPͻa1DY����gp�Pf�1���;SK�Oq�#�ځ�Ԯ�Έ3�ZHL��Q�Α��1â�����@{7e�?����7�^d�i�=��d�bo��y� D�gS��R���zr��<����x��>�fw�
����{�9ɣ b�z~�AhUx�M�L�=B�i�}�G�s����z(y,�Ly�c��s�[��E��sK�$�+�oCa�f�a��_	ݩF���;�!�1�J���do�p����C���@����ǦT��ǒ�P�	�R�&�D/��ti�c�\OT!�f6Y��ɘ*^lm��F2���S1��?e8��WP0���������w�z���xC�4�}\���8X%���uwR_f��\����h
:��̾�]�� ���Z�^<�/��n3�zYu�a�f�"�S˜�[Uæ���Q2#=D,��?81�%Fơ�]���x���ϡ)~.|����5'�>���4�>3lUR�����I4nv��ƑI:b%�{�/S�-u�kց����CQW���Ewt�Pn����T|�i��Aj����ga�"�[�plz$H�W�����M�ݔĶ�x��m�6)�&W�7%{�i�[�
V*�l�cQ���ש�W�ZP+תQ2���ܠ�6(�����:�B�b�(YMN����������� ���Ak��Jb�򹻘��9`��0���Srؔ�X��}<��!����#�7��T�t����x�&�ū�Y	�Qs ��5a��t��>��ר��+��g���e��IulLW�uS�ܪb������ս�l�ɔ@�QuPB�솂$���vBc����N��G�Iiә���>��D���x�ʴ���YjF��`��R�;�*�,V.������ٽ.ۣ��Dr�C��<�$�n���alǅ]�b�[�Lh3�o
�ݞz5�
@�)H�VJZ����	B�,Gw�\�t��|����܂7C.�2M��4�W��	��R|/1;���d�����Ķ8Ĺ����OQ��p�}X��{�ݡ�A�JcLs7��4���J�[v�y�y� ,N�s��Y)n��Fltip0�mZyB/�Q]a9�G_��`���e�ņ�v��}f�?�g:�:42��Iށ˦�pEP-�%?ч�κ"-(��L�c1m '4iA�Nc
���jLB�D'�ׁ�X�1dfz>�Yt�)����6KL�3I�G�?����e6Ds#"�	��hF��KتA@��*���@�����z׉��q����Z�wY{�*�
�]�(���8��A������cV�K%DJ��Ci7�k�K��:�'h����A��`���q�p+���!72��P(�O�>�S�$��"�z�^cƻ1�x�}�Jo��Ku|�wo�4�2(Ө�f�Fp�^�4��Ͽ��i�PC��e�cf�O���
��,'�' ���N�G���_�Pl�85d��\���)�Q��	f�Da%EXy�*8w�3�<�Q�����%��j3 \�:��ċ�X�J ;wO�L�zk{F�%�9���[X��,�eqk��2������1�o�a)�H��?�Fo�|����C��Oea��n�X�Z;�Ҹ���믱�q�%R=��E�c/�aEH����M��O{1�t��o����z���$v��r��T���}��"̲3�����@Sh�N�>k'. ����IJ�w�Қ.P��?�H��Hg�Z$�}��}� �;�=j�ی�ܟ�|3��P�g�8��Uچ�[���y�uP�+���-"�}W�ܥ5"���j׮�p�~��_�}���@r7����Ck� �r�^)�7����ZC��5��T�2_d���t��B�
��['VI�(�I�>	xO��x�-����J�{x�{�Ṽ¢�H}Ϳ��m�Q/`T�&����Ŗǻ�/�Z� �|���t$?TZ �����BW��	3�������I�"@�d�?��X����}I���I>	}ϴ-�\����n�/K}�G;�Ն�-f�$���¤�\�a/"[��T�2�����$���i!#���q�D0;;>
NU ���)�`v��MU@(O����^�fj�Y+/FD(Rsum ��S�N�yp"}yr�p��?�ڝ�4���9�w�t�#t��Y�Xh�6W���/ڿX�������T�.��������b��pZ�pa��#�e�B��Q��P����z���W��!ӛ���G/(;{h�fij��m�x|�����9B� ň����	��<5Me��<�vI��$����q=�˕H�Rf>k��}�ֻ�V+I�ؽ���FeYĭ68wQ�������%]��W6�����w	�jsT�о����3H�B�UTlob@�0�ܛh�(�śCMLFA-��p&ψ�����ff�I��*1����1#��w�����=#��7Uw)x�wvY�'ޔ��	5�H�[��K����ؓ��1w��9�u�0&{���#m��l�V�l7��h���o`��6�O)3x�0F�ʾ�|��ږ#'�S�� D��C
G�w0�\������h��H<Jja�|Z�a���`]����8��K�}���zy�]0�;">�g:0�3����]�⿰}>�\�u*$�F$�g��d>C�*����y��S���bP�5�!1�d���/�Ⱥ�$���,kj��p�x��+�P�iZ�Y�h����t�kd��zM<���@�n*��D*Z��J�\��p���{�z�Ѣ�R���s���ʆ���y�I)�N(�jj֔�g��T�����s}�Dʏ?��]��&Qʔ������c���2fcF	歷H�+t5]�r��X.�SW؆MN����;b�W��渓�ە��z~e�
�fO�hb�f��>}/�o��q�2/��;�8X�"��*Df �<��0�����ɭ�Z��ު�' S|h�L˽��I^j_j&�J|��_f�ϝ�?6�����hA� Ȟ�5n ^]���>�zhE&Q>s5x\=#I��}@<�ǈkO^��w"�NЕrH�� ↲;�|�?�x�rhg�h!
|����@R��y�p?ht"j���[|ǿ#��њӊc!hTe��I�)m�9��b��=���-���-�v���z�_%��G�ϒ��i����c �i�95cK��D��Bm݉�R���T�F��+E�}�M���K[�Gj�	��r\��q
�h��.Zgd��sl�8\���5��9�R���I�$�Ŏ�����
<@��'�A<7<kc��KO6T� `��dIl@2����^� �b�W@��#�sָ��C�F�K���p?fT��������z���2�|Κ'�����[�3����+���dW�*�#���-1�� ��H|��4�%���-vӚڦ�I?�|�@��ܕ0Ő-=t���L�cjT�]�~��d#���mˊ9R��dܡl�qb8������A��@����^!VC6����[`���gJ|�#��@��z�PY�����K� $�tb��(�ݣf���1µnx%M�����h�Vjj�O��TM}�p�*����H�]kGBex*�G�����a(L$��U��j�%�V��m=�w�VyE����/Ն��΃<�t�Z�3�7,f�m�G�z�-軇5Dy$e׭& ������[C"@�M9Q�K'���+��� _l�D��5%!`B�L7"�s��9�`V_��#?%j �7ZnB�q�l��豠D��,�͝�/��=ØMo�n�q��L	�*�v�:Q)�����k�E#>;ڝ|�% �z��Ηh@U=UZ�o֑�촭��w�*�s���1���_�ߚi&A�n�4�u~��[�u��b�˥�a!2�X!?q'�>���%�i{�!h����N�����!�,4���Q&F�7&L*�۽�3@<�i}�[��k�xi��[L�nQ,d"�۳Q����hf��6�|��V1���p�М��aHN�8c�E��� ��VB_��=���`�o煉�z��u�L`��&�[m�D6b�߃�XJ!o���p�Z�Ҟ����gNǭx�W��O�i�nX���gL�G�a��I�/��ƙ�>v�7ȝ`�
)�]����{P�v�������0JD���H���=�ē♈���r{(�ozS�Ji�u���G����k����T�k���Z��w��uC���箉����%�=�G�Dל�)�A��a,��\虊d��X�U���*k�jv�X>�/!���Y}���ls���E�.�z-�;��Q[�h��Qg�Q�7�{\C����n�wR�
�$-�y�ۮ8Y��^5~#��.泂��1�l��213K�:KM����5���CN��=��T͝�p�`�db��|.�J°E�<^�?]�I�?�����l��9�.k�+A�����kJ��}aފ���	Q��1{d��W�^+�o�H���ƅ��e�>��+2$����W��ڱy��� l�҆�|��][E�w>ǯA}�`�p�av��%^�.�-��AQ��u��eZ�2}2$6!�,UɅ�M΢���h����3㔀��l��.1Q�g< �
bP�M�/-SڂC���{J;{�Gq>e��x#^�)fm!BV,������xh'�$@-��Y�����K8{��i4[�-�4���"n���E�lyx���t��)���=���ؗ'P�r�^�jL؇��m��7�R�;�v�(�-^|4is����@��E�7�.��l���dV�B��T�ݥn�_�vZ��c(+�)�:���O�X6r��X�{���s�W�~�me%�od�q�q���~�F�x��*\��!��!��BC�TI�% =� � �G��N��&d+gP~K]�x�q�to��"�	k���js�3��ܡ"���e�PG�PQ����!���#�%��[��/+5m���`�=�sS��<)�T�a
���&�m�s>P��:Q ڡ��2& ��J�K��o���{5�&�lE�8:��M�g��nO@��5Fo!���b�3a�i]���aߥ�IY9C��15��}	<��p�#o'dq��@7,���1<����'��?߸B��ӡK��z�H:�-b]oiB#gk/�?0��˳ˈ����wo.��&>@o�`劤;��㐥�"�H+|�}�$����TZK�vzh�{��Q������H���z�!w�~��󀽠��lX��-�w�<,�pu#1��!}|��Վ�	�5ezq�m�CS������<"��op��L!���1�^1T��RTe˦�ҭ�9*z��' �z�9�h���W�5a���Wۨ]�C{�(�c�I~�T����cQ���>{�����XC��5��s�LV��<��
q�a����l���C�W���?��������)!�,G�s�]~�b��3�.��iK.B����l�E6�p�d!�{��:�0��<�[E�2 _]?a&Ü�#'��O�}�� �u|lԺ���U:oDؚ,D7���v�}"��.��]�// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kmod.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/div64.h>
#include <linux/firmware.h>

#include "cimax2.h"
#include "altera-ci.h"
#include "cx23888-ir.h"
#include "cx23885-ir.h"
#include "cx23885-av.h"
#include "cx23885-input.h"

MODULE_DESCRIPTION("Driver for cx23885 based TV cards");
MODULE_AUTHOR("Steven Toth <stoth@linuxtv.org>");
MODULE_LICENSE("GPL");
MODULE_VERSION(CX23885_VERSION);

/*
 * Some platforms have been found to require periodic resetting of the DMA
 * engine. Ryzen and XEON platforms are known to be affected. The symptom
 * encountered is "mpeg risc op code error". Only Ryzen platforms employ
 * this workaround if the option equals 1. The workaround can be explicitly
 * disabled for all platforms by setting to 0, the workaround can be forced
 * on for any platform by setting to 2.
 */
static unsigned int dma_reset_workaround = 1;
module_param(dma_reset_workaround, int, 0644);
MODULE_PARM_DESC(dma_reset_workaround, "periodic RiSC dma engine reset; 0-force disable, 1-driver detect (default), 2-force enable");

static unsigned int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "enable debug messages");

static unsigned int card[]  = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };
module_param_array(card,  int, NULL, 0444);
MODULE_PARM_DESC(card, "card type");

#define dprintk(level, fmt, arg...)\
	do { if (debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: " fmt), \
		       __func__, ##arg); \
	} while (0)

static unsigned int cx23885_devcount;

#define NO_SYNC_LINE (-1U)

/* FIXME, these allocations will change when
 * analog arrives. The be reviewed.
 * CX23887 Assumptions
 * 1 line = 16 bytes of CDT
 * cmds size = 80
 * cdt size = 16 * linesize
 * iqsize = 64
 * maxlines = 6
 *
 * Address Space:
 * 0x00000000 0x00008fff FIFO clusters
 * 0x00010000 0x000104af Channel Management Data Structures
 * 0x000104b0 0x000104ff Free
 * 0x00010500 0x000108bf 15 channels * iqsize
 * 0x000108c0 0x000108ff Free
 * 0x00010900 0x00010e9f IQ's + Cluster Descriptor Tables
 *                       15 channels * (iqsize + (maxlines * linesize))
 * 0x00010ea0 0x00010xxx Free
 */

static struct sram_channel cx23885_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x10380,
		.cdt		= 0x104c0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "ch2",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10400,
		.cdt		= 0x10580,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH06] = {
		.name		= "TS2 C",
		.cmds_start	= 0x10140,
		.ctrl_start	= 0x10440,
		.cdt		= 0x105e0,
		.fifo_start	= 0x6000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH07] = {
		.name		= "TV Audio",
		.cmds_start	= 0x10190,
		.ctrl_start	= 0x10480,
		.cdt		= 0x10a00,
		.fifo_start	= 0x7000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA6_PTR1,
		.ptr2_reg	= DMA6_PTR2,
		.cnt1_reg	= DMA6_CNT1,
		.cnt2_reg	= DMA6_CNT2,
	},
	[SRAM_CH08] = {
		.name		= "ch8",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA7_PTR1,
		.ptr2_reg	= DMA7_PTR2,
		.cnt1_reg	= DMA7_CNT1,
		.cnt2_reg	= DMA7_CNT2,
	},
	[SRAM_CH09] = {
		.name		= "ch9",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA8_PTR1,
		.ptr2_reg	= DMA8_PTR2,
		.cnt1_reg	= DMA8_CNT1,
		.cnt2_reg	= DMA8_CNT2,
	},
};

static struct sram_channel cx23887_sram_channels[] = {
	[SRAM_CH01] = {
		.name		= "VID A",
		.cmds_start	= 0x10000,
		.ctrl_start	= 0x105b0,
		.cdt		= 0x107b0,
		.fifo_start	= 0x40,
		.fifo_size	= 0x2800,
		.ptr1_reg	= DMA1_PTR1,
		.ptr2_reg	= DMA1_PTR2,
		.cnt1_reg	= DMA1_CNT1,
		.cnt2_reg	= DMA1_CNT2,
	},
	[SRAM_CH02] = {
		.name		= "VID A (VBI)",
		.cmds_start	= 0x10050,
		.ctrl_start	= 0x105F0,
		.cdt		= 0x10810,
		.fifo_start	= 0x3000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA2_PTR1,
		.ptr2_reg	= DMA2_PTR2,
		.cnt1_reg	= DMA2_CNT1,
		.cnt2_reg	= DMA2_CNT2,
	},
	[SRAM_CH03] = {
		.name		= "TS1 B",
		.cmds_start	= 0x100A0,
		.ctrl_start	= 0x10630,
		.cdt		= 0x10870,
		.fifo_start	= 0x5000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA3_PTR1,
		.ptr2_reg	= DMA3_PTR2,
		.cnt1_reg	= DMA3_CNT1,
		.cnt2_reg	= DMA3_CNT2,
	},
	[SRAM_CH04] = {
		.name		= "ch4",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA4_PTR1,
		.ptr2_reg	= DMA4_PTR2,
		.cnt1_reg	= DMA4_CNT1,
		.cnt2_reg	= DMA4_CNT2,
	},
	[SRAM_CH05] = {
		.name		= "ch5",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH06] = {
		.name		= "TS2 C",
		.cmds_start	= 0x10140,
		.ctrl_start	= 0x10670,
		.cdt		= 0x108d0,
		.fifo_start	= 0x6000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA5_PTR1,
		.ptr2_reg	= DMA5_PTR2,
		.cnt1_reg	= DMA5_CNT1,
		.cnt2_reg	= DMA5_CNT2,
	},
	[SRAM_CH07] = {
		.name		= "TV Audio",
		.cmds_start	= 0x10190,
		.ctrl_start	= 0x106B0,
		.cdt		= 0x10930,
		.fifo_start	= 0x7000,
		.fifo_size	= 0x1000,
		.ptr1_reg	= DMA6_PTR1,
		.ptr2_reg	= DMA6_PTR2,
		.cnt1_reg	= DMA6_CNT1,
		.cnt2_reg	= DMA6_CNT2,
	},
	[SRAM_CH08] = {
		.name		= "ch8",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA7_PTR1,
		.ptr2_reg	= DMA7_PTR2,
		.cnt1_reg	= DMA7_CNT1,
		.cnt2_reg	= DMA7_CNT2,
	},
	[SRAM_CH09] = {
		.name		= "ch9",
		.cmds_start	= 0x0,
		.ctrl_start	= 0x0,
		.cdt		= 0x0,
		.fifo_start	= 0x0,
		.fifo_size	= 0x0,
		.ptr1_reg	= DMA8_PTR1,
		.ptr2_reg	= DMA8_PTR2,
		.cnt1_reg	= DMA8_CNT1,
		.cnt2_reg	= DMA8_CNT2,
	},
};

static void cx23885_irq_add(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask |= mask;

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask |= mask;
	cx_set(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask)
{
	u32 v;
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	v = mask & dev->pci_irqmask;
	if (v)
		cx_set(PCI_INT_MSK, v);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static inline void cx23885_irq_enable_all(struct cx23885_dev *dev)
{
	cx23885_irq_enable(dev, 0xffffffff);
}

void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	cx_clear(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static inline void cx23885_irq_disable_all(struct cx23885_dev *dev)
{
	cx23885_irq_disable(dev, 0xffffffff);
}

void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask &= ~mask;
	cx_clear(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static u32 cx23885_irq_get_mask(struct cx23885_dev *dev)
{
	u32 v;
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	v = cx_read(PCI_INT_MSK);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
	return v;
}

static int cx23885_risc_decode(u32 risc)
{
	static char *instr[16] = {
		[RISC_SYNC    >> 28] = "sync",
		[RISC_WRITE   >> 28] = "write",
		[RISC_WRITEC  >> 28] = "writec",
		[RISC_READ    >> 28] = "read",
		[RISC_READC   >> 28] = "readc",
		[RISC_JUMP    >> 28] = "jump",
		[RISC_SKIP    >> 28] = "skip",
		[RISC_WRITERM >> 28] = "writerm",
		[RISC_WRITECM >> 28] = "writecm",
		[RISC_WRITECR >> 28] = "writecr",
	};
	static int incr[16] = {
		[RISC_WRITE   >> 28] = 3,
		[RISC_JUMP    >> 28] = 3,
		[RISC_SKIP    >> 28] = 1,
		[RISC_SYNC    >> 28] = 1,
		[RISC_WRITERM >> 28] = 3,
		[RISC_WRITECM >> 28] = 3,
		[RISC_WRITECR >> 28] = 4,
	};
	static char *bits[] = {
		"12",   "13",   "14",   "resync",
		"cnt0", "cnt1", "18",   "19",
		"20",   "21",   "22",   "23",
		"irq1", "irq2", "eol",  "sol",
	};
	int i;

	printk(KERN_DEBUG "0x%08x [ %s", risc,
	       instr[risc >> 28] ? instr[risc >> 28] : "INVALID");
	for (i = ARRAY_SIZE(bits) - 1; i >= 0; i--)
		if (risc & (1 << (i + 12)))
			pr_cont(" %s", bits[i]);
	pr_cont(" count=%d ]\n", risc & 0xfff);
	return incr[risc >> 28] ? incr[risc >> 28] : 1;
}

static void cx23885_wakeup(struct cx23885_tsport *port,
			   struct cx23885_dmaqueue *q, u32 count)
{
	struct cx23885_buffer *buf;
	int count_delta;
	int max_buf_done = 5; /* service maximum five buffers */

	do {
		if (list_empty(&q->active))
			return;
		buf = list_entry(q->active.next,
				 struct cx23885_buffer, queue);

		buf->vb.vb2_buf.timestamp = ktime_get_ns();
		buf->vb.sequence = q->count++;
		if (count != (q->count % 65536)) {
			dprintk(1, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
				buf->vb.vb2_buf.index, count, q->count);
		} else {
			dprintk(7, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
				buf->vb.vb2_buf.index, count, q->count);
		}
		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
		max_buf_done--;
		/* count register is 16 bits so apply modulo appropriately */
		count_delta = ((int)count - (int)(q->count % 65536));
	} while ((count_delta > 0) && (max_buf_done > 0));
}

int cx23885_sram_channel_setup(struct cx23885_dev *dev,
				      struct sram_channel *ch,
				      unsigned int bpl, u32 risc)
{
	unsigned int i, lines;
	u32 cdt;

	if (ch->cmds_start == 0) {
		dprintk(1, "%s() Erasing channel [%s]\n", __func__,
			ch->name);
		cx_write(ch->ptr1_reg, 0);
		cx_write(ch->ptr2_reg, 0);
		cx_write(ch->cnt2_reg, 0);
		cx_write(ch->cnt1_reg, 0);
		return 0;
	} else {
		dprintk(1, "%s() Configuring channel [%s]\n", __func__,
			ch->name);
	}

	bpl   = (bpl + 7) & ~7; /* alignment */
	cdt   = ch->cdt;
	lines = ch->fifo_size / bpl;
	if (lines > 6)
		lines = 6;
	BUG_ON(lines < 2);

	cx_write(8 + 0, RISC_JUMP | RISC_CNT_RESET);
	cx_write(8 + 4, 12);
	cx_write(8 + 8, 0);

	/* write CDT */
	for (i = 0; i < lines; i++) {
		dprintk(2, "%s() 0x%08x <- 0x%08x\n", __func__, cdt + 16*i,
			ch->fifo_start + bpl*i);
		cx_write(cdt + 16*i, ch->fifo_start + bpl*i);
		cx_write(cdt + 16*i +  4, 0);
		cx_write(cdt + 16*i +  8, 0);
		cx_write(cdt + 16*i + 12, 0);
	}

	/* write CMDS */
	if (ch->jumponly)
		cx_write(ch->cmds_start + 0, 8);
	else
		cx_write(ch->cmds_start + 0, risc);
	cx_write(ch->cmds_start +  4, 0); /* 64 bits 63-32 */
	cx_write(ch->cmds_start +  8, cdt);
	cx_write(ch->cmds_start + 12, (lines*16) >> 3);
	cx_write(ch->cmds_start + 16, ch->ctrl_start);
	if (ch->jumponly)
		cx_write(ch->cmds_start + 20, 0x80000000 | (64 >> 2));
	else
		cx_write(ch->cmds_start + 20, 64 >> 2);
	for (i = 24; i < 80; i += 4)
		cx_write(ch->cmds_start + i, 0);

	/* fill registers */
	cx_write(ch->ptr1_reg, ch->fifo_start);
	cx_write(ch->ptr2_reg, cdt);
	cx_write(ch->cnt2_reg, (lines*16) >> 3);
	cx_write(ch->cnt1_reg, (bpl >> 3) - 1);

	dprintk(2, "[bridge %d] sram setup %s: bpl=%d lines=%d\n",
		dev->bridge,
		ch->name,
		bpl,
		lines);

	return 0;
}

void cx23885_sram_channel_dump(struct cx23885_dev *dev,
				      struct sram_channel *ch)
{
	static char *name[] = {
		"init risc lo",
		"init risc hi",
		"cdt base",
		"cdt size",
		"iq base",
		"iq size",
		"risc pc lo",
		"risc pc hi",
		"iq wr ptr",
		"iq rd ptr",
		"cdt current",
		"pci target lo",
		"pci target hi",
		"line / byte",
	};
	u32 risc;
	unsigned int i, j, n;

	pr_warn("%s: %s - dma channel status dump\n",
		dev->name, ch->name);
	for (i = 0; i < ARRAY_SIZE(name); i++)
		pr_warn("%s:   cmds: %-15s: 0x%08x\n",
			dev->name, name[i],
			cx_read(ch->cmds_start + 4*i));

	for (i = 0; i < 4; i++) {
		risc = cx_read(ch->cmds_start + 4 * (i + 14));
		pr_warn("%s:   risc%d: ", dev->name, i);
		cx23885_risc_decode(risc);
	}
	for (i = 0; i < (64 >> 2); i += n) {
		risc = cx_read(ch->ctrl_start + 4 * i);
		/* No consideration for bits 63-32 */

		pr_warn("%s:   (0x%08x) iq %x: ", dev->name,
			ch->ctrl_start + 4 * i, i);
		n = cx23885_risc_decode(risc);
		for (j = 1; j < n; j++) {
			risc = cx_read(ch->ctrl_start + 4 * (i + j));
			pr_warn("%s:   iq %x: 0x%08x [ arg #%d ]\n",
				dev->name, i+j, risc, j);
		}
	}

	pr_warn("%s: fifo: 0x%08x -> 0x%x\n",
		dev->name, ch->fifo_start, ch->fifo_start+ch->fifo_size);
	pr_warn("%s: ctrl: 0x%08x -> 0x%x\n",
		dev->name, ch->ctrl_start, ch->ctrl_start + 6*16);
	pr_warn("%s:   ptr1_reg: 0x%08x\n",
		dev->name, cx_read(ch->ptr1_reg));
	pr_warn("%s:   ptr2_reg: 0x%08x\n",
		dev->name, cx_read(ch->ptr2_reg));
	pr_warn("%s:   cnt1_reg: 0x%08x\n",
		dev->name, cx_read(ch->cnt1_reg));
	pr_warn("%s:   cnt2_reg: 0x%08x\n",
		dev->name, cx_read(ch->cnt2_reg));
}

static void cx23885_risc_disasm(struct cx23885_tsport *port,
				struct cx23885_riscmem *risc)
{
	struct cx23885_dev *dev = port->dev;
	unsigned int i, j, n;

	pr_info("%s: risc disasm: %p [dma=0x%08lx]\n",
	       dev->name, risc->cpu, (unsigned long)risc->dma);
	for (i = 0; i < (risc->size >> 2); i += n) {
		pr_info("%s:   %04d: ", dev->name, i);
		n = cx23885_risc_decode(le32_to_cpu(risc->cpu[i]));
		for (j = 1; j < n; j++)
			pr_info("%s:   %04d: 0x%08x [ arg #%d ]\n",
				dev->name, i + j, risc->cpu[i + j], j);
		if (risc->cpu[i] == cpu_to_le32(RISC_JUMP))
			break;
	}
}

static void cx23885_clear_bridge_error(struct cx23885_dev *dev)
{
	uint32_t reg1_val, reg2_val;

	if (!dev->need_dma_reset)
		return;

	reg1_val = cx_read(TC_REQ); /* read-only */
	reg2_val = cx_read(TC_REQ_SET);

	if (reg1_val && reg2_val) {
		cx_write(TC_REQ, reg1_val);
		cx_write(TC_REQ_SET, reg2_val);
		cx_read(VID_B_DMA);
		cx_read(VBI_B_DMA);
		cx_read(VID_C_DMA);
		cx_read(VBI_C_DMA);

		dev_info(&dev->pci->dev,
			"dma in progress detected 0x%08x 0x%08x, clearing\n",
			reg1_val, reg2_val);
	}
}

static void cx23885_shutdown(struct cx23885_dev *dev)
{
	/* disable RISC controller */
	cx_write(DEV_CNTRL2, 0);

	/* Disable all IR activity */
	cx_write(IR_CNTRL_REG, 0);

	/* Disable Video A/B activity */
	cx_write(VID_A_DMA_CTL, 0);
	cx_write(VID_B_DMA_CTL, 0);
	cx_write(VID_C_DMA_CTL, 0);

	/* Disable Audio activity */
	cx_write(AUD_INT_DMA_CTL, 0);
	cx_write(AUD_EXT_DMA_CTL, 0);

	/* Disable Serial port */
	cx_write(UART_CTL, 0);

	/* Disable Interrupts */
	cx23885_irq_disable_all(dev);
	cx_write(VID_A_INT_MSK, 0);
	cx_write(VID_B_INT_MSK, 0);
	cx_write(VID_C_INT_MSK, 0);
	cx_write(AUDIO_INT_INT_MSK, 0);
	cx_write(AUDIO_EXT_INT_MSK, 0);

}

static void cx23885_reset(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	cx23885_shutdown(dev);

	cx_write(PCI_INT_STAT, 0xffffffff);
	cx_write(VID_A_INT_STAT, 0xffffffff);
	cx_write(VID_B_INT_STAT, 0xffffffff);
	cx_write(VID_C_INT_STAT, 0xffffffff);
	cx_write(AUDIO_INT_INT_STAT, 0xffffffff);
	cx_write(AUDIO_EXT_INT_STAT, 0xffffffff);
	cx_write(CLK_DELAY, cx_read(CLK_DELAY) & 0x80000000);
	cx_write(PAD_CTRL, 0x00500300);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
	msleep(100);

	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH01],
		720*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH03],
		188*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH04], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH05], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH06],
		188*4, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH07], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH08], 128, 0);
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH09], 128, 0);

	cx23885_gpio_setup(dev);

	cx23885_irq_get_mask(dev);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
}


static int cx23885_pci_quirks(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	/* The cx23885 bridge has a weird bug which causes NMI to be asserted
	 * when DMA begins if RDR_TLCTL0 bit4 is not cleared. It does not
	 * occur on the cx23887 bridge.
	 */
	if (dev->bridge == CX23885_BRIDGE_885)
		cx_clear(RDR_TLCTL0, 1 << 4);

	/* clear dma in progress */
	cx23885_clear_bridge_error(dev);
	return 0;
}

static int get_resources(struct cx23885_dev *dev)
{
	if (request_mem_region(pci_resource_start(dev->pci, 0),
			       pci_resource_len(dev->pci, 0),
			       dev->name))
		return 0;

	pr_err("%s: can't get MMIO memory @ 0x%llx\n",
	       dev->name, (unsigned long long)pci_resource_start(dev->pci, 0));

	return -EBUSY;
}

static int cx23885_init_tsport(struct cx23885_dev *dev,
	struct cx23885_tsport *port, int portno)
{
	dprintk(1, "%s(portno=%d)\n", __func__, portno);

	/* Transport bus init dma queue  - Common settings */
	port->dma_ctl_val        = 0x11; /* Enable RISC controller and Fifo */
	port->ts_int_msk_val     = 0x1111; /* TS port bits for RISC */
	port->vld_misc_val       = 0x0;
	port->hw_sop_ctrl_val    = (0x47 << 16 | 188 << 4);

	spin_lock_init(&port->slock);
	port->dev = dev;
	port->nr = portno;

	INIT_LIST_HEAD(&port->mpegq.active);
	mutex_init(&port->frontends.lock);
	INIT_LIST_HEAD(&port->frontends.felist);
	port->frontends.active_fe_id = 0;

	/* This should be hardcoded allow a single frontend
	 * attachment to this tsport, keeping the -dvb.c
	 * code clean and safe.
	 */
	if (!port->num_frontends)
		port->num_frontends = 1;

	switch (portno) {
	case 1:
		port->reg_gpcnt          = VID_B_GPCNT;
		port->reg_gpcnt_ctl      = VID_B_GPCNT_CTL;
		port->reg_dma_ctl        = VID_B_DMA_CTL;
		port->reg_lngth          = VID_B_LNGTH;
		port->reg_hw_sop_ctrl    = VID_B_HW_SOP_CTL;
		port->reg_gen_ctrl       = VID_B_GEN_CTL;
		port->reg_bd_pkt_status  = VID_B_BD_PKT_STATUS;
		port->reg_sop_status     = VID_B_SOP_STATUS;
		port->reg_fifo_ovfl_stat = VID_B_FIFO_OVFL_STAT;
		port->reg_vld_misc       = VID_B_VLD_MISC;
		port->reg_ts_clk_en      = VID_B_TS_CLK_EN;
		port->reg_src_sel        = VID_B_SRC_SEL;
		port->reg_ts_int_msk     = VID_B_INT_MSK;
		port->reg_ts_int_stat    = VID_B_INT_STAT;
		port->sram_chno          = SRAM_CH03; /* VID_B */
		port->pci_irqmask        = 0x02; /* VID_B bit1 */
		break;
	case 2:
		port->reg_gpcnt          = VID_C_GPCNT;
		port->reg_gpcnt_ctl      = VID_C_GPCNT_CTL;
		port->reg_dma_ctl        = VID_C_DMA_CTL;
		port->reg_lngth          = VID_C_LNGTH;
		port->reg_hw_sop_ctrl    = VID_C_HW_SOP_CTL;
		port->reg_gen_ctrl       = VID_C_GEN_CTL;
		port->reg_bd_pkt_status  = VID_C_BD_PKT_STATUS;
		port->reg_sop_status     = VID_C_SOP_STATUS;
		port->reg_fifo_ovfl_stat = VID_C_FIFO_OVFL_STAT;
		port->reg_vld_misc       = VID_C_VLD_MISC;
		port->reg_ts_clk_en      = VID_C_TS_CLK_EN;
		port->reg_src_sel        = 0;
		port->reg_ts_int_msk     = VID_C_INT_MSK;
		port->reg_ts_int_stat    = VID_C_INT_STAT;
		port->sram_chno          = SRAM_CH06; /* VID_C */
		port->pci_irqmask        = 0x04; /* VID_C bit2 */
		break;
	default:
		BUG();
	}

	return 0;
}

static void cx23885_dev_checkrevision(struct cx23885_dev *dev)
{
	switch (cx_read(RDR_CFG2) & 0xff) {
	case 0x00:
		/* cx23885 */
		dev->hwrevision = 0xa0;
		break;
	case 0x01:
		/* CX23885-12Z */
		dev->hwrevision = 0xa1;
		break;
	case 0x02:
		/* CX23885-13Z/14Z */
		dev->hwrevision = 0xb0;
		break;
	case 0x03:
		if (dev->pci->device == 0x8880) {
			/* CX23888-21Z/22Z */
			dev->hwrevision = 0xc0;
		} else {
			/* CX23885-14Z */
			dev->hwrevision = 0xa4;
		}
		break;
	case 0x04:
		if (dev->pci->device == 0x8880) {
			/* CX23888-31Z */
			dev->hwrevision = 0xd0;
		} else {
			/* CX23885-15Z, CX23888-31Z */
			dev->hwrevision = 0xa5;
		}
		break;
	case 0x0e:
		/* CX23887-15Z */
		dev->hwrevision = 0xc0;
		break;
	case 0x0f:
		/* CX23887-14Z */
		dev->hwrevision = 0xb1;
		break;
	default:
		pr_err("%s() New hardware revision found 0x%x\n",
		       __func__, dev->hwrevision);
	}
	if (dev->hwrevision)
		pr_info("%s() Hardware revision = 0x%02x\n",
			__func__, dev->hwrevision);
	else
		pr_err("%s() Hardware revision unknown 0x%x\n",
		       __func__, dev->hwrevision);
}

/* Find the first v4l2_subdev member of the group id in hw */
struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw)
{
	struct v4l2_subdev *result = NULL;
	struct v4l2_subdev *sd;

	spin_lock(&dev->v4l2_dev.lock);
	v4l2_device_for_each_subdev(sd, &dev->v4l2_dev) {
		if (sd->grp_id == hw) {
			result = sd;
			break;
		}
	}
	spin_unlock(&dev->v4l2_dev.lock);
	return result;
}

static int cx23885_dev_setup(struct cx23885_dev *dev)
{
	int i;

	spin_lock_init(&dev->pci_irqmask_lock);
	spin_lock_init(&dev->slock);

	mutex_init(&dev->lock);
	mutex_init(&dev->gpio_lock);

	atomic_inc(&dev->refcount);

	dev->nr = cx23885_devcount++;
	sprintf(dev->name, "cx23885[%d]", dev->nr);

	/* Configure the internal memory */
	if (dev->pci->device == 0x8880) {
		/* Could be 887 or 888, assume an 888 default */
		dev->bridge = CX23885_BRIDGE_888;
		/* Apply a sensible clock frequency for the PCIe bridge */
		dev->clk_freq = 50000000;
		dev->sram_channels = cx23887_sram_channels;
	} else
	if (dev->pci->device == 0x8852) {
		dev->bridge = CX23885_BRIDGE_885;
		/* Apply a sensible clock frequency for the PCIe bridge */
		dev->clk_freq = 28000000;
		dev->sram_channels = cx23885_sram_channels;
	} else
		BUG();

	dprintk(1, "%s() Memory configured for PCIe bridge type %d\n",
		__func__, dev->bridge);

	/* board config */
	dev->board = UNSET;
	if (card[dev->nr] < cx23885_bcount)
		dev->board = card[dev->nr];
	for (i = 0; UNSET == dev->board  &&  i < cx23885_idcount; i++)
		if (dev->pci->subsystem_vendor == cx23885_subids[i].subvendor &&
		    dev->pci->subsystem_device == cx23885_subids[i].subdevice)
			dev->board = cx23885_subids[i].card;
	if (UNSET == dev->board) {
		dev->board = CX23885_BOARD_UNKNOWN;
		cx23885_card_list(dev);
	}

	if (dev->pci->device == 0x8852) {
		/* no DIF on cx23885, so no analog tuner support possible */
		if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC)
			dev->board = CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885;
		else if (dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB)
			dev->board = CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885;
	}

	/* If the user specific a clk freq override, apply it */
	if (cx23885_boards[dev->board].clk_freq > 0)
		dev->clk_freq = cx23885_boards[dev->board].clk_freq;

	if (dev->board == CX23885_BOARD_HAUPPAUGE_IMPACTVCBE &&
		dev->pci->subsystem_device == 0x7137) {
		/* Hauppauge ImpactVCBe device ID 0x7137 is populated
		 * with an 888, and a 25Mhz crystal, instead of the
		 * usual third overtone 50Mhz. The default clock rate must
		 * be overridden so the cx25840 is properly configured
		 */
		dev->clk_freq = 25000000;
	}

	dev->pci_bus  = dev->pci->bus->number;
	dev->pci_slot = PCI_SLOT(dev->pci->devfn);
	cx23885_irq_add(dev, 0x001f00);

	/* External Master 1 Bus */
	dev->i2c_bus[0].nr = 0;
	dev->i2c_bus[0].dev = dev;
	dev->i2c_bus[0].reg_stat  = I2C1_STAT;
	dev->i2c_bus[0].reg_ctrl  = I2C1_CTRL;
	dev->i2c_bus[0].reg_addr  = I2C1_ADDR;
	dev->i2c_bus[0].reg_rdata = I2C1_RDATA;
	dev->i2c_bus[0].reg_wdata = I2C1_WDATA;
	dev->i2c_bus[0].i2c_period = (0x9d << 24); /* 100kHz */

	/* External Master 2 Bus */
	dev->i2c_bus[1].nr = 1;
	dev->i2c_bus[1].dev = dev;
	dev->i2c_bus[1].reg_stat  = I2C2_STAT;
	dev->i2c_bus[1].reg_ctrl  = I2C2_CTRL;
	dev->i2c_bus[1].reg_addr  = I2C2_ADDR;
	dev->i2c_bus[1].reg_rdata = I2C2_RDATA;
	dev->i2c_bus[1].reg_wdata = I2C2_WDATA;
	dev->i2c_bus[1].i2c_period = (0x9d << 24); /* 100kHz */

	/* Internal Master 3 Bus */
	dev->i2c_bus[2].nr = 2;
	dev->i2c_bus[2].dev = dev;
	dev->i2c_bus[2].reg_stat  = I2C3_STAT;
	dev->i2c_bus[2].reg_ctrl  = I2C3_CTRL;
	dev->i2c_bus[2].reg_addr  = I2C3_ADDR;
	dev->i2c_bus[2].reg_rdata = I2C3_RDATA;
	dev->i2c_bus[2].reg_wdata = I2C3_WDATA;
	dev->i2c_bus[2].i2c_period = (0x07 << 24); /* 1.95MHz */

	if ((cx23885_boards[dev->board].portb == CX23885_MPEG_DVB) ||
		(cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER))
		cx23885_init_tsport(dev, &dev->ts1, 1);

	if ((cx23885_boards[dev->board].portc == CX23885_MPEG_DVB) ||
		(cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER))
		cx23885_init_tsport(dev, &dev->ts2, 2);

	if (get_resources(dev) < 0) {
		pr_err("CORE %s No more PCIe resources for subsystem: %04x:%04x\n",
		       dev->name, dev->pci->subsystem_vendor,
		       dev->pci->subsystem_device);

		cx23885_devcount--;
		return -ENODEV;
	}

	/* PCIe stuff */
	dev->lmmio = ioremap(pci_resource_start(dev->pci, 0),
			     pci_resource_len(dev->pci, 0));

	dev->bmmio = (u8 __iomem *)dev->lmmio;

	pr_info("CORE %s: subsystem: %04x:%04x, board: %s [card=%d,%s]\n",
		dev->name, dev->pci->subsystem_vendor,
		dev->pci->subsystem_device, cx23885_boards[dev->board].name,
		dev->board, card[dev->nr] == dev->board ?
		"insmod option" : "autodetected");

	cx23885_pci_quirks(dev);

	/* Assume some sensible defaults */
	dev->tuner_type = cx23885_boards[dev->board].tuner_type;
	dev->tuner_addr = cx23885_boards[dev->board].tuner_addr;
	dev->tuner_bus = cx23885_boards[dev->board].tuner_bus;
	dev->radio_type = cx23885_boards[dev->board].radio_type;
	dev->radio_addr = cx23885_boards[dev->board].radio_addr;

	dprintk(1, "%s() tuner_type = 0x%x tuner_addr = 0x%x tuner_bus = %d\n",
		__func__, dev->tuner_type, dev->tuner_addr, dev->tuner_bus);
	dprintk(1, "%s() radio_type = 0x%x radio_addr = 0x%x\n",
		__func__, dev->radio_type, dev->radio_addr);

	/* The cx23417 encoder has GPIO's that need to be initialised
	 * before DVB, so that demodulators and tuners are out of
	 * reset before DVB uses them.
	 */
	if ((cx23885_boards[dev->board].portb == CX23885_MPEG_ENCODER) ||
		(cx23885_boards[dev->board].portc == CX23885_MPEG_ENCODER))
			cx23885_mc417_init(dev);

	/* init hardware */
	cx23885_reset(dev);

	cx23885_i2c_register(&dev->i2c_bus[0]);
	cx23885_i2c_register(&dev->i2c_bus[1]);
	cx23885_i2c_register(&dev->i2c_bus[2]);
	cx23885_card_setup(dev);
	c