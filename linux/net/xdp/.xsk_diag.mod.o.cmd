{
			req.ret = -ERANGE;
			return req;
		}

		req.util = req.percent << SCHED_CAPACITY_SHIFT;
		req.util = DIV_ROUND_CLOSEST_ULL(req.util, UCLAMP_PERCENT_SCALE);
	}

	return req;
}

static ssize_t cpu_uclamp_write(struct kernfs_open_file *of, char *buf,
				size_t nbytes, loff_t off,
				enum uclamp_id clamp_id)
{
	struct uclamp_request req;
	struct task_group *tg;

	req = capacity_from_percent(buf);
	if (req.ret)
		return req.ret;

	static_branch_enable(&sched_uclamp_used);

	mutex_lock(&uclamp_mutex);
	rcu_read_lock();

	tg = css_tg(of_css(of));
	if (tg->uclamp_req[clamp_id].value != req.util)
		uclamp_se_set(&tg->uclamp_req[clamp_id], req.util, false);

	/*
	 * Because of not recoverable conversion rounding we keep track of the
	 * exact requested value
	 */
	tg->uclamp_pct[clamp_id] = req.percent;

	/* Update effective clamps to track the most restrictive value */
	cpu_util_update_eff(of_css(of));

	rcu_read_unlock();
	mutex_unlock(&uclamp_mutex);

	return nbytes;
}

static ssize_t cpu_uclamp_min_write(struct kernfs_open_file *of,
				    char *buf, size_t nbytes,
				    loff_t off)
{
	return cpu_uclamp_write(of, buf, nbytes, off, UCLAMP_MIN);
}

static ssize_t cpu_uclamp_max_write(struct kernfs_open_file *of,
				    char *buf, size_t nbytes,
				    loff_t off)
{
	return cpu_uclamp_write(of, buf, nbytes, off, UCLAMP_MAX);
}

static inline void cpu_uclamp_print(struct seq_file *sf,
				    enum uclamp_id clamp_id)
{
	struct task_group *tg;
	u64 util_clamp;
	u64 percent;
	u32 rem;

	rcu_read_lock();
	tg = css_tg(seq_css(sf));
	util_clamp = tg->uclamp_req[clamp_id].value;
	rcu_read_unlock();

	if (util_clamp == SCHED_CAPACITY_SCALE) {
		seq_puts(sf, "max\n");
		return;
	}

	percent = tg->uclamp_pct[clamp_id];
	percent = div_u64_rem(percent, POW10(UCLAMP_PERCENT_SHIFT), &rem);
	seq_printf(sf, "%llu.%0*u\n", percent, UCLAMP_PERCENT_SHIFT, rem);
}

static int cpu_uclamp_min_show(struct seq_file *sf, void *v)
{
	cpu_uclamp_print(sf, UCLAMP_MIN);
	return 0;
}

static int cpu_uclamp_max_show(struct seq_file *sf, void *v)
{
	cpu_uclamp_print(sf, UCLAMP_MAX);
	return 0;
}
#endif /* CONFIG_UCLAMP_TASK_GROUP */

#ifdef CONFIG_FAIR_GROUP_SCHED
static int cpu_shares_write_u64(struct cgroup_subsys_state *css,
				struct cftype *cftype, u64 shareval)
{
	if (shareval > scale_load_down(ULONG_MAX))
		shareval = MAX_SHARES;
	return sched_group_set_shares(css_tg(css), scale_load(shareval));
}

static u64 cpu_shares_read_u64(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	struct task_group *tg = css_tg(css);

	return (u64) scale_load_down(tg->shares);
}

#ifdef CONFIG_CFS_BANDWIDTH
static DEFINE_MUTEX(cfs_constraints_mutex);

const u64 max_cfs_quota_period = 1 * NSEC_PER_SEC; /* 1s */
static const u64 min_cfs_quota_period = 1 * NSEC_PER_MSEC; /* 1ms */
/* More than 203 days if BW_SHIFT equals 20. */
static const u64 max_cfs_runtime = MAX_BW * NSEC_PER_USEC;

static int __cfs_schedulable(struct task_group *tg, u64 period, u64 runtime);

static int tg_set_cfs_bandwidth(struct task_group *tg, u64 period, u64 quota,
				u64 burst)
{
	int i, ret = 0, runtime_enabled, runtime_was_enabled;
	struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;

	if (tg == &root_task_group)
		return -EINVAL;

	/*
	 * Ensure we have at some amount of bandwidth every period.  This is
	 * to prevent reaching a state of large arrears when throttled via
	 * entity_tick() resulting in prolonged exit starvation.
	 */
	if (quota < min_cfs_quota_period || period < min_cfs_quota_period)
		return -EINVAL;

	/*
	 * Likewise, bound things on the other side by preventing insane quota
	 * periods.  This also allows us to normalize in computing quota
	 * feasibility.
	 */
	if (period > max_cfs_quota_period)
		return -EINVAL;

	/*
	 * Bound quota to defend quota against overflow during bandwidth shift.
	 */
	if (quota != RUNTIME_INF && quota > max_cfs_runtime)
		return -EINVAL;

	if (quota != RUNTIME_INF && (burst > quota ||
				     burst + quota > max_cfs_runtime))
		return -EINVAL;

	/*
	 * Prevent race between setting of cfs_rq->runtime_enabled and
	 * unthrottle_offline_cfs_rqs().
	 */
	cpus_read_lock();
	mutex_lock(&cfs_constraints_mutex);
	ret = __cfs_schedulable(tg, period, quota);
	if (ret)
		goto out_unlock;

	runtime_enabled = quota != RUNTIME_INF;
	runtime_was_enabled = cfs_b->quota != RUNTIME_INF;
	/*
	 * If we need to toggle cfs_bandwidth_used, off->on must occur
	 * before making related changes, and on->off must occur afterwards
	 */
	if (runtime_enabled && !runtime_was_enabled)
		cfs_bandwidth_usage_inc();
	raw_spin_lock_irq(&cfs_b->lock);
	cfs_b->period = ns_to_ktime(period);
	cfs_b->quota = quota;
	cfs_b->burst = burst;

	__refill_cfs_bandwidth_runtime(cfs_b);

	/* Restart the period timer (if active) to handle new period expiry: */
	if (runtime_enabled)
		start_cfs_bandwidth(cfs_b);

	raw_spin_unlock_irq(&cfs_b->lock);

	for_each_online_cpu(i) {
		struct cfs_rq *cfs_rq = tg->cfs_rq[i];
		struct rq *rq = cfs_rq->rq;
		struct rq_flags rf;

		rq_lock_irq(rq, &rf);
		cfs_rq->runtime_enabled = runtime_enabled;
		cfs_rq->runtime_remaining = 0;

		if (cfs_rq->throttled)
			unthrottle_cfs_rq(cfs_rq);
		rq_unlock_irq(rq, &rf);
	}
	if (runtime_was_enabled && !runtime_enabled)
		cfs_bandwidth_usage_dec();
out_unlock:
	mutex_unlock(&cfs_constraints_mutex);
	cpus_read_unlock();

	return ret;
}

static int tg_set_cfs_quota(struct task_group *tg, long cfs_quota_us)
{
	u64 quota, period, burst;

	period = ktime_to_ns(tg->cfs_bandwidth.period);
	burst = tg->cfs_bandwidth.burst;
	if (cfs_quota_us < 0)
		quota = RUNTIME_INF;
	else if ((u64)cfs_quota_us <= U64_MAX / NSEC_PER_USEC)
		quota = (u64)cfs_quota_us * NSEC_PER_USEC;
	else
		return -EINVAL;

	return tg_set_cfs_bandwidth(tg, period, quota, burst);
}

static long tg_get_cfs_quota(struct task_group *tg)
{
	u64 quota_us;

	if (tg->cfs_bandwidth.quota == RUNTIME_INF)
		return -1;

	quota_us = tg->cfs_bandwidth.quota;
	do_div(quota_us, NSEC_PER_USEC);

	return quota_us;
}

static int tg_set_cfs_period(struct task_group *tg, long cfs_period_us)
{
	u64 quota, period, burst;

	if ((u64)cfs_period_us > U64_MAX / NSEC_PER_USEC)
		return -EINVAL;

	period = (u64)cfs_period_us * NSEC_PER_USEC;
	quota = tg->cfs_bandwidth.quota;
	burst = tg->cfs_bandwidth.burst;

	return tg_set_cfs_bandwidth(tg, period, quota, burst);
}

static long tg_get_cfs_period(struct task_group *tg)
{
	u64 cfs_period_us;

	cfs_period_us = ktime_to_ns(tg->cfs_bandwidth.period);
	do_div(cfs_period_us, NSEC_PER_USEC);

	return cfs_period_us;
}

static int tg_set_cfs_burst(struct task_group *tg, long cfs_burst_us)
{
	u64 quota, period, burst;

	if ((u64)cfs_burst_us > U64_MAX / NSEC_PER_USEC)
		return -EINVAL;

	burst = (u64)cfs_burst_us * NSEC_PER_USEC;
	period = ktime_to_ns(tg->cfs_bandwidth.period);
	quota = tg->cfs_bandwidth.quota;

	return tg_set_cfs_bandwidth(tg, period, quota, burst);
}

static long tg_get_cfs_burst(struct task_group *tg)
{
	u64 burst_us;

	burst_us = tg->cfs_bandwidth.burst;
	do_div(burst_us, NSEC_PER_USEC);

	return burst_us;
}

static s64 cpu_cfs_quota_read_s64(struct cgroup_subsys_state *css,
				  struct cftype *cft)
{
	return tg_get_cfs_quota(css_tg(css));
}

static int cpu_cfs_quota_write_s64(struct cgroup_subsys_state *css,
				   struct cftype *cftype, s64 cfs_quota_us)
{
	return tg_set_cfs_quota(css_tg(css), cfs_quota_us);
}

static u64 cpu_cfs_period_read_u64(struct cgroup_subsys_state *css,
				   struct cftype *cft)
{
	return tg_get_cfs_period(css_tg(css));
}

static int cpu_cfs_period_write_u64(struct cgroup_subsys_state *css,
				    struct cftype *cftype, u64 cfs_period_us)
{
	return tg_set_cfs_period(css_tg(css), cfs_period_us);
}

static u64 cpu_cfs_burst_read_u64(struct cgroup_subsys_state *css,
				  struct cftype *cft)
{
	return tg_get_cfs_burst(css_tg(css));
}

static int cpu_cfs_burst_write_u64(struct cgroup_subsys_state *css,
				   struct cftype *cftype, u64 cfs_burst_us)
{
	return tg_set_cfs_burst(css_tg(css), cfs_burst_us);
}

struct cfs_schedulable_data {
	struct task_group *tg;
	u64 period, quota;
};

/*
 * normalize group quota/period to be quota/max_period
 * note: units are usecs
 */
static u64 normalize_cfs_quota(struct task_group *tg,
			       struct cfs_schedulable_data *d)
{
	u64 quota, period;

	if (tg == d->tg) {
		period = d->period;
		quota = d->quota;
	} else {
		period = tg_get_cfs_period(tg);
		quota = tg_get_cfs_quota(tg);
	}

	/* note: these should typically be equivalent */
	if (quota == RUNTIME_INF || quota == -1)
		return RUNTIME_INF;

	return to_ratio(period, quota);
}

static int tg_cfs_schedulable_down(struct task_group *tg, void *data)
{
	struct cfs_schedulable_data *d = data;
	struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;
	s64 quota = 0, parent_quota = -1;

	if (!tg->parent) {
		quota = RUNTIME_INF;
	} else {
		struct cfs_bandwidth *parent_b = &tg->parent->cfs_bandwidth;

		quota = normalize_cfs_quota(tg, d);
		parent_quota = parent_b->hierarchical_quota;

		/*
		 * Ensure max(child_quota) <= parent_quota.  On cgroup2,
		 * always take the min.  On cgroup1, only inherit when no
		 * limit is set:
		 */
		if (cgroup_subsys_on_dfl(cpu_cgrp_subsys)) {
			quota = min(quota, parent_quota);
		} else {
			if (quota == RUNTIME_INF)
				quota = parent_quota;
			else if (parent_quota != RUNTIME_INF && quota > parent_quota)
				return -EINVAL;
		}
	}
	cfs_b->hierarchical_quota = quota;

	return 0;
}

static int __cfs_schedulable(struct task_group *tg, u64 period, u64 quota)
{
	int ret;
	struct cfs_schedulable_data data = {
		.tg = tg,
		.period = period,
		.quota = quota,
	};

	if (quota != RUNTIME_INF) {
		do_div(data.period, NSEC_PER_USEC);
		do_div(data.quota, NSEC_PER_USEC);
	}

	rcu_read_lock();
	ret = walk_tg_tree(tg_cfs_schedulable_down, tg_nop, &data);
	rcu_read_unlock();

	return ret;
}

static int cpu_cfs_stat_show(struct seq_file *sf, void *v)
{
	struct task_group *tg = css_tg(seq_css(sf));
	struct cfs_bandwidth *cfs_b = &tg->cfs_bandwidth;

	seq_printf(sf, "nr_periods %d\n", cfs_b->nr_periods);
	seq_printf(sf, "nr_throttled %d\n", cfs_b->nr_throttled);
	seq_printf(sf, "throttled_time %llu\n", cfs_b->throttled_time);

	if (schedstat_enabled() && tg != &root_task_group) {
		struct sched_statistics *stats;
		u64 ws = 0;
		int i;

		for_each_possible_cpu(i) {
			stats = __schedstats_from_se(tg->se[i]);
			ws += schedstat_val(stats->wait_sum);
		}

		seq_printf(sf, "wait_sum %llu\n", ws);
	}

	seq_printf(sf, "nr_bursts %d\n", cfs_b->nr_burst);
	seq_printf(sf, "burst_time %llu\n", cfs_b->burst_time);

	return 0;
}
#endif /* CONFIG_CFS_BANDWIDTH */
#endif /* CONFIG_FAIR_GROUP_SCHED */

#ifdef CONFIG_RT_GROUP_SCHED
static int cpu_rt_runtime_write(struct cgroup_subsys_state *css,
				struct cftype *cft, s64 val)
{
	return sched_group_set_rt_runtime(css_tg(css), val);
}

static s64 cpu_rt_runtime_read(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	return sched_group_rt_runtime(css_tg(css));
}

static int cpu_rt_period_write_uint(struct cgroup_subsys_state *css,
				    struct cftype *cftype, u64 rt_period_us)
{
	return sched_group_set_rt_period(css_tg(css), rt_period_us);
}

static u64 cpu_rt_period_read_uint(struct cgroup_subsys_state *css,
				   struct cftype *cft)
{
	return sched_group_rt_period(css_tg(css));
}
#endif /* CONFIG_RT_GROUP_SCHED */

#ifdef CONFIG_FAIR_GROUP_SCHED
static s64 cpu_idle_read_s64(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	return css_tg(css)->idle;
}

static int cpu_idle_write_s64(struct cgroup_subsys_state *css,
				struct cftype *cft, s64 idle)
{
	return sched_group_set_idle(css_tg(css), idle);
}
#endif

static struct cftype cpu_legacy_files[] = {
#ifdef CONFIG_FAIR_GROUP_SCHED
	{
		.name = "shares",
		.read_u64 = cpu_shares_read_u64,
		.write_u64 = cpu_shares_write_u64,
	},
	{
		.name = "idle",
		.read_s64 = cpu_idle_read_s64,
		.write_s64 = cpu_idle_write_s64,
	},
#endif
#ifdef CONFIG_CFS_BANDWIDTH
	{
		.name = "cfs_quota_us",
		.read_s64 = cpu_cfs_quota_read_s64,
		.write_s64 = cpu_cfs_quota_write_s64,
	},
	{
		.name = "cfs_period_us",
		.read_u64 = cpu_cfs_period_read_u64,
		.write_u64 = cpu_cfs_period_write_u64,
	},
	{
		.name = "cfs_burst_us",
		.read_u64 = cpu_cfs_burst_read_u64,
		.write_u64 = cpu_cfs_burst_write_u64,
	},
	{
		.namELF                      p'      4     (                �������t?S�Ë��  ��t���t
ǂ$&      �����ǃ�      ǃ�      [Í�&    Í�&    �����UWV��S����d�    �D$1�����   ��  ����   ��,&  ����   �F�$    1ɺ����Tj �   h    ����������t   �   �   ��  �,$������Å��Z   �8�   �
   �h���  ǅ�      �x�(�G������G$�u���Dh   j P������ud�u�h@  jPW������؃�$��������   ��,&  �$���������   �    u-�D$d+    u��1�[^_]Ív �    t�������������   f�f�f�f�������@�@    �@    1�Í�&    �����1�Í�&    ������VS�p�^��������v���������[^Í�&    �t& �����UWVS�Ã��5   �$�ʅ��2  �h����   �}|����   �w`����   9��  ��4  ���7  ��1����S�D$�GT�D$����ȉщD$�D$ЉD$9�sv�t$�$�D$���������������C�t$��C�OT��)�9�C�s�s��C�GL9���   ����[^_]������-   ���  ��[^_]Í�&    �v �L$�D$)ʉы$�ΉL$������D$�L$+L$�$ȋL$��ʉ���4  ������T�����&    �   ���   ��[^_]Í�&    �v )���K��������[^_]�������    �   ���P����  ��&    ��    �����WVS�p���   ��T  �����1҉����������  �������ǃ(&      �   �����1�[^_Í�&    ��&    �����U��WVS��l�u|d�    �D$h�E��$�C��D$�Ǎ�   �������<&  ���  �|$d�H�D$\�D$��<&  �T$`��������"  ���  ��x��T  �D$����  s6�$�T$�    ������T$�D$hd+    ��   ��l��[^_]Ít& �Ѝ��   �   �   ��ǆ�    ǆ�      ǆ       ǆ  �   ǆ  ��  ǆ  ��  ǆ     ǆ     ǆ  � @ǆ!  1  ǆ(  b   ������<$�L$�o���   ǃ(&      ����  �D$������$�   ������$�����$�    ����������������������&    �����U1ɺ   W��VS���/d�    �D$1��GTjj ������ƃ�����  �G    �$��   �   �����1ɺ   j j �D$������\$�P   �C    ��X��`  �}d���������XZ�����  ��Pt����P��  ����������D$d+    u
����[^_]������S�@@��uh    h    �����XZ�6��$&  ��u��h    Ph0   ����������B�����ǃ$&      1�[�hh  ������    �   �����1�1�h    ������        h�  ��������h(   h    h`   ��������b  ��Ph   Vhl  ��������$��t������������h(   Vh  ��������b  ��j�h   Vh  �������뻃�Ph   Vh�   �������뵃�h(   Vh�   ��������b  ǆ,&      ��Ph   Vh�  ��������a���h(   h    �����Y[�b  h(   h    h�  ��������b  hd   hd   hp  ���������  hd   hL  �����^_��  Q�L$�t$Phd   h�  ������k�T$����tD�}|���3  �=    ��  hd   h$  �����XZ��  hd   h�  �����XZ��  �=    ��  hd   h�  �����Y[��  �H   ������@   �������PhP   Uh�  ��������p  �����hH  �����X1��    P   �           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   �              �  �      �                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P       �  �  �     �    �  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           ��     �           ��    �  �  s           GNU  �       �               GNU p�7���J��=O"��&��        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 ��       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 �            �   U   ?     �            �           �      #     �   =          O   Y       �        4           B  8       	 T           ��d  �       y  �       �  �  %     �          �    �     s   d       	 �  �  R     �    �    �  �   @    	   �  0     +  H       	 6  @       	 A  P       	 L  �   -     e  �        �          �  �   
    	 �           ���  �   #     �  "       �  ;         G  	       P       )  _  7     @           H          P             \           j             s                          �             �             �  U   ?     �             �             �             �             �             	                                      >             F             \             m             u             �             �             �             �             �             �             �                          9             A             U             c             p             �           �     �    �             �             �  �  �     �             �              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q     �     �   B  �   U  �     �   i  �   L  �     �     �       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  �  c  �    �  S  �    �  N  �  N  �  N  �  K  �  Z    N          �  I  �  _  �  R  �    �      I  @  I  R    Z    x  R  �  e  �    �    �  N  �  V  �  \  �  Y  �  a    N  #  E  C  V  m  X  �  a  �  E  U  ]  o  d  �  P  �  a  �  a  �  S  �  N  �    �  E  �  D  �          W  &  [  I  T  V    g    p  ^  {  E  �  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  �     �   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     �     �   Q  �     �     �   Q  �     �     �     �   Q  �     �     �   Q  �     �     �     �     �   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    �  Q  �    �    �    �  Q  �    �    �    �    �    �  Q  �    �    �  M  �    �  M  �    �    �  Q  �       N          Q     `       �     �     �     �     �                                                    $              B     f     c          B     f  $                     �   O  �  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   �                    	   @       �  �              )             �  �                  %   	   @       l"  �               8             d  �                 4   	   @       �"  H              K             X	                    G   	   @       <%  (               Z             �	  �                   V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       �%  P               �      2       }  +                 �             �  �                 �             @  (                  �   	   @       �%  @               �             h                    �   	   @       &                 �             t                     �             t  (                  �             �  $                  �             �  0                  �                           @       �   	   @       4&                                                       0          9                              9                                   <  �     A         	              �                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ���ΰgj�D~��
�C��W���jG�4<�*1���CvV����ZhTpzƟ'�X����ο��s~o�'[��\�^=�$���0����a`>��
���<���*f���,�@��f)���8xJ�UuKwt���w�)�ifړ���dF�`�[˨�E����¤����!L����4r��i1g��)-��st0��n��zi=�?>��!��,8Zv=��t#Lb�6���*���E�:M�;��K��F�V�TO<'o�[֟��@�Σ]-3�p�tɯ�0`�ӤD�Nr(��KAԬJN��u���F>W�,��|�-O|U8S�	�l�#��M��xSJ�Zl��J��{xp�h��<ɫR��Z��6��gM����2쟉�ɔ+B��X*%�y�yc{Ĥ�F�3U8�q��K#V�1u3y�}�p���=��FaҴ�6&*�X��u��ft�Y�վG�`�}�����$W�Ŕ���zx�C����.H�\��sTZ�Ʀ]ڢɚk��U��Xΐ�`�T����w�u�c:�u���-��2w6���=�9U����v��s����.Q����w?�(��(��	:�+K���1ez[�L��E�pW�szl�r/���E'�� '
n�S>^�\X���O�>��:��C���P��ۏ(�(k"��%���$��q��I-�A7n���!�ۚ���@��*�����§0�dH����|�ѣ�q�/�ϔ��{�io����(��0:��0��+kF���kW��n꼸��z��)9��������7x��p��!Wpʹ��#D�w#,�� d�k31ȈKPN)�I����~<�)�@�O��u%�N�p��Ī�*,{�"W��r�+���/*JK�f�����ӛd;�	��dq+��S�m �x���;:�.]��U�(@�\vN���2�<>�l�U D }:��D��������LW�)�	�)<hpQ@c�@9P[Κ���OV�4�FoXJc�� �7m߉�U:�nh��A(�g�M7���p��z<]�d��9yNHv/��oD�0k�ؙF������LC������$�'�D�.�l����G�%}���" �/��G�x5yIg�u�R��}�䬿O����M����Á��M�W��!|1�&/�ԌׇPݙ���r@������N!l,�n�ߙ�t�n����@ʺ ���G�@P�]�����l&O'u����+���]�wB`�'�d����ع��h�b.�F�Tr� ��5�^�h�È��vS�Iϑ���<�w������'�;9����r��v��VƚaIYJ��~3��I���y#�A��j�D�PF��K����%��OS��FK
A�hV� y^�4��*��S��7���۷�ɭ���"��⚁�B��+���
��� *�[��[�b�x�����)v2zD�)9,�=��|�(�0�@8��<8]o�O�E��핊�,�#-_t��V��d�&�dA��.��7Rx)d��w��Qi�QR^}�j��������s�q=O� 4�ۘ���"[p��8�A���:UrYk����F
�����RQ�`���QhZfъ��}�n&�bL'�<�?�j�����H�QwfaD�j�x:���ZI�}�,�mCQXN,���ES��du]C0_�h8\vz�o�%M@ggj4���az�w5�ȉ��������dɵ�~>Lc�IT,�n�r�<?�GD�y�����;;O1I1�0�yf<��t�#�I��3���eH�Hi�h���t�5���:��DO���`7�`������W�\5�@лE���JZd�->d��0ǥ4ʮܼ���6�r�%)ee{�h�J��>k4"$�W�jd[/�N����LI&C73�;����a�/|������d���7mR��d�?��T'�s��ՑwR|��P郥�܇�*(�g�yl�D�
;t=0?���k���u��\Tv�!ʦ���"nP���.Z�����p��!�Ƙ��c�Ov�_������ҍ_=�����S#ӆd�&�y�l��φq�f���s��Q�(.���E���XV~�T�z�L2�����l�'f�I@h4M ��>Qs4��E�Tq�@�����.i�4�Bh�ąQr���k,W�v�0/F�2YD?��������1-��]�s�o8�^� MR�m��)�ߐ��9C]J^�E,�҇�C�]
��ë�������f�ӻ��1�0�:�.�;5�׫{������#�m�����4���6�𨬁c7��2R!����ˁ�ɚ���t�?����5���a�/�Qn�Fo]�<����7R�0��g�H�*x������vW�_��L֍J��:L�2LM�3�A/ �-λ����GC׋U=�5w��q�qk��c^q�9L��=��W�:S.R��d�����_茅 [��]���q�n!���� 	�>�Cۨ�$j��Y��Њ�:s+~�z��N0�Q� RJ�3�]а�ZRb�-v��j�q89�aMڄ��W�=o@EcJR����蛶Y��]��Dpx�Y��G�bJ�#Ű�{��"��#��.�Ò�u~b��ܤ
�ѻ�����C^��Ѧ)>�z`�bgYz�����Й鉅���:��$x����]��E���|Qp?�������� ɚ,���?�Z�d+���Tڜ�;�L/8�!�8�҂/���������l�_��Ԥ�@���uH�(�<R��u^�~�0� )���[i���	TsH�*f��͚u�a�oƸ�hG�^"�Q���%��%]z�`�ʐeF!=?��mv�¥�n+^oe�҉V��A� �p�n�H�>Lh}���͉8��������米8��X%�m8E���i�r���$Q`�P�+Ϻ} /󯎒%�[��0�`D��ק�KU�ɞ��~���7Jz��G�WP1 `yD_]�D���| `�E��ff�� �q6�y|t}�]�^'��j�Y�X�MdN@���2�H��Ϛ"�֜Ϧ�*{B�0�W;�C�؁P�K|�/��蟗F��Gy��t��(��� ��r3��j.'6?�@�+�����������Y��6���l����wy���4��N[�W��(/���ȼ����"��E=��udf:�w��ܪ"=�C}�(�W�=�4H9DJ:h1|ľl/�_�J�1}�8fLȕ���O��[*$eC^�Yuu&����]i-�WA/|Kw�s��T��넗>\H������Ŋ�N�dc��];A��a-��d)�Qlg�R�$$��uH�y媞�G*���BB��`pN烊e+�ƶ4*Ԙ4�L� �>�<�Jr�,]S��������%��+�)7~dX�������pe�Xacg���7K��վ�.�$K�mx��!�
�zЃTMzd֯u�`SͶG����b�7I#i�u�Ց �܉m��X��D��f!W�5*��2o/�v���V�J��Q���Q
���*�v��Ư����~� �z^ �a�Nl�9`��c�QN���I�7��X��o�e|F!��b@�s�a�Z��?�(R�w$5��m.�a���E���|s%��i�$��(�D\�u�$"N~�3��.��;o sr��̡1MIˡ.c�Wb.���N��I�I�8�����m3����8m'\�D_��}6U�w_��Jz)ոm1����o8ɹq1�@G{z-�\��ԋ���ԛF��7$e��7�~�¥���#��S���,O"_�M|�q1�zr��q���=k��hK�v&s*�8=��z�뎉���j����p�;�����a=1���`�b�AZ�~`�^���E5c-Ƞ&%Fœ�vn��P��ӯ�F��~/������K����t���M�B�[i*��(����p���p��
�@*��,�u���r�]P��|��Ӳߦ�@W��΀bL��@�ǟ�n 8�s�O��ٖx[}I3i:K����h�8JBI�W��+�ܣ��T%�y�% @}Xi�-Ê���ϱ8<�& eE�U�// SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/export.h>
#include <linux/irqflags.h>

noinstr void warn_bogus_irq_restore(void)
{
	instrumentation_begin();
	WARN_ONCE(1, "raw_local_irq_restore() called with IRQs enabled\n");
	instrumentation_end();
}
EXPORT_SYMBOL(warn_bogus_irq_restore);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          